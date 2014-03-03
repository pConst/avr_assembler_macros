//--------------------------------------------------------------------------------
// Konstantin Pavlov, pavlovconst@gmail.com
//--------------------------------------------------------------------------------

// INFO --------------------------------------------------------------------------------
//
// // WARNING: Cycle calculations could be inaccurate
//

/*
// The definitions must be done before using folowing macros

M_LDIL R0,$00			
M_LDIL R1,$01
M_LDIL R2,$FF

.DEF Reg00 = R0
.DEF Reg01 = R1
.DEF RegFF = R2

.DEF RegTMP1 = R16
.DEF RegTMP2 = R17

.DEF MacroTMP1 = R26
.DEF MacroTMP2 = R27
*/

.IFNDEF	M_UTILS_INCLUDED
.EQU M_UTILS_INCLUDED = 1

//--------------------------------------------------------------------------------
.MACRO U_FIFO_WRITE
			// Write byte into FIFO
			// 47 cycles
			/* U_FIFO_WRITE 
						@0   R16,
						@1   RD_LCD_FIFO,
						@2   LCD_FIFO_LEN,
						@3   RD_LCD_WPTR,
						@4   RD_LCD_RPTR  - usage example */

.DEF WPTR = R26	// = MacroTMP1
.DEF RPTR = R27	// = MacroTMP2
.DEF T = R28

			W_PUSH X
			push R28
			W_PUSH Z								;destination address

				lds WPTR,@3
				lds RPTR,@4

				mov T,WPTR
				inc T								;T is a new value for WPTR
				cpi T,@2
				brne PC+2
					M_CLR T

				cp T,RPTR
				breq U_FIFO_WRITE_LBL1				;skip further logic if there is no space to write data
					W_LDI Z,@1						;load destination address
					W_ADDB Z,WPTR					;apply offset
					st Z,@0

					sts @3,T						;save WPTR

				clt									;"No error" indicator
				rjmp PC+2
U_FIFO_WRITE_LBL1:		
					set								;"Error" indicator

			W_POP Z
			pop R28
			W_POP X

.UNDEF WPTR
.UNDEF RPTR
.UNDEF T

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_FIFO_BLOCK_WRITE
			// Write block of bytes into FIFO
			// min 51 (+17 per each additional byte in the block) cycles
			/* U_FIFO_BLOCK_WRITE 
						@0   RD_SOMEDATA,
						@1   RD_SOMEDATA_LEN,
						@2   RD_LCD_FIFO,
						@3   LCD_FIFO_LEN,
						@4   RD_LCD_WPTR,
						@5   RD_LCD_RPTR  - usage example */

.DEF WPTR = R16	// = RegTMP1
.DEF RPTR = R17	// = RegTMP2
.DEF T = R18

			push WPTR
			push RPTR
			push T
			W_PUSH X								;Data, Counter
			W_PUSH Y								;source base address
			W_PUSH Z								;destination address

				lds WPTR,@4
				lds RPTR,@5
				
				mov T,WPTR
				W_LDI Y,@0
				ldi MacroTMP2,@1
U_FIFO_BLOCK_WRITE_LBL1:
					inc T							;T is a new value for WPTR
					cpi T,@3
					brne PC+2
						M_CLR T

					cp T,RPTR
					breq U_FIFO_BLOCK_WRITE_LBL2	;skip further logic if there is no space to write data
						ld MacroTMP1,Y+				;get data to convert
						W_LDI Z,@2					;load destination address
						W_ADDB Z,WPTR				;apply offset
						st Z,MacroTMP1
						mov WPTR,T					;update WPTR

					dec MacroTMP2
				brne U_FIFO_BLOCK_WRITE_LBL1

				clt									;"No error" indicator
				rjmp PC+2
U_FIFO_BLOCK_WRITE_LBL2:					
					set								;"Error" indicator

				sts @4,WPTR							;save WPTR

			W_POP Z
			W_POP Y
			W_POP X
			pop T
			pop RPTR
			pop WPTR

.UNDEF WPTR
.UNDEF RPTR
.UNDEF T

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_FIFO_READ
			// Read byte from FIFO
			// max 47 cycles
			/* U_FIFO_READ
						@0   R16,
						@1   RD_LCD_FIFO,
						@2   LCD_FIFO_LEN,
						@3   RD_LCD_WPTR,
						@4   RD_LCD_RPTR  - usage example */

.DEF WPTR = R26	// = MacroTMP1
.DEF RPTR = R27	// = MacroTMP2
.DEF T = R28

			W_PUSH X
			push R28
			W_PUSH Z	

				lds WPTR,@3
				lds RPTR,@4

				cp WPTR,RPTR
				breq U_FIFO_READ_LBL1				;skip further logic if there is no space to write data
					W_LDI Z,@1						;load destination address
					W_ADDB Z,RPTR					;apply offset
					ld @0,Z

					inc RPTR						;update RPTR
					cpi RPTR,@2
					brne PC+2
						M_CLR RPTR

					sts @4,RPTR						;save RPTR
					
				clt								;"No error" indicator
				rjmp PC+2
U_FIFO_READ_LBL1:	set							;"Error" indicator

			W_POP Z
			pop R28
			W_POP X

.UNDEF WPTR
.UNDEF RPTR
.UNDEF T

.ENDM

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
.MACRO U_CIRCBUFFER_WRITE
			// Write byte into CIRCBUFFER (new bytes just overwrite old ones circularly)
			// U_CIRCBUFFER_WRITE Rs,k,l,m,n   0<=s<=25,$0000<=k,l,m,n<=$FFFF		,24 cycles
			/* U_CIRCBUFFER_WRITE 
						@0   R16,
						@1   RD_ADC_BUFFER,
						@2   ADC_BUFFER_LEN,
						@3   RD_ADC_WPTR  - usage example */

.DEF WPTR = R26	// = MacroTMP1
.DEF T = R27 // = MacroTMP2

			W_PUSH X
			W_PUSH Z
			
				; T is the new value of WPTR
				lds WPTR,@3
				mov T,WPTR
				inc T
				cpi T,@2
				brne PC+2
					mov T,Reg00

				; Set data
				W_LDI Z,@1
				W_ADDB Z,WPTR
				st Z,@0
				; Update WPTR
				sts @3,T

			W_POP Z
			W_POP X

.UNDEF WPTR
.UNDEF T

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_CIRCBUFFER_READMEAN8
			// Read mean value from CIRCBUFFER. CIRCBUFFER size is considered to be 8 bytes
			// U_CIRCBUFFER_READMEAN8 Rd,k   0<=d<=25,$0000<=k<=$FFFF		,84 cycles
			/* U_CIRCBUFFER_READMEAN8 
						@0   R16,
						@1   RD_ADC_BUFFER  - usage example */

			W_PUSH X
			W_PUSH Y
			W_PUSH Z

				W_CLR Y								; collecting summ here
				W_LDI Z,@1
				ldi MacroTMP1,8
U_CIRCBUFFER_READMEAN8_LBL1:
					ld MacroTMP2,Z+					; get data
					W_ADDB Y,MacroTMP2				; add it alltogether
				dec MacroTMP1
				brne U_CIRCBUFFER_READMEAN8_LBL1

				; Divide by 8
				lsr YH
				ror YL								; /2
				lsr YH
				ror YL								; /4
				lsr YH
				ror YL								; /8

				mov @0,YL

			W_POP Z
			W_POP Y
			W_POP X

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_CIRCBUFFER_READMEAN16
			// Read mean value from CIRCBUFFER. CIRCBUFFER size is considered to be 8 bytes
			// U_CIRCBUFFER_READMEAN16 Rd,k   0<=d<=25,$0000<=k<=$FFFF		,? cycles
			/* U_CIRCBUFFER_READMEAN16 
						@0   R16,
						@1   RD_ADC_BUFFER  - usage example */

			W_PUSH X
			W_PUSH Y
			W_PUSH Z

				W_CLR Y								; collecting summ here
				W_LDI Z,@1
				ldi MacroTMP1,16
U_CIRCBUFFER_READMEAN16_LBL1:
					ld MacroTMP2,Z+					; get data
					W_ADDB Y,MacroTMP2				; add it alltogether
				dec MacroTMP1
				brne U_CIRCBUFFER_READMEAN16_LBL1

				; Divide by 16
				lsr YH
				ror YL								; /2
				lsr YH
				ror YL								; /4
				lsr YH
				ror YL								; /8
				lsr YH
				ror YL								; /16

				mov @0,YL

			W_POP Z
			W_POP Y
			W_POP X

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_CIRCBUFFER_READMEAN32
			// Read mean value from CIRCBUFFER. CIRCBUFFER size is considered to be 8 bytes
			// U_CIRCBUFFER_READMEAN32 Rd,k   0<=d<=25,$0000<=k<=$FFFF		,? cycles
			/* U_CIRCBUFFER_READMEAN32 
						@0   R16,
						@1   RD_ADC_BUFFER  - usage example */

			W_PUSH X
			W_PUSH Y
			W_PUSH Z

				W_CLR Y								; collecting summ here
				W_LDI Z,@1
				ldi MacroTMP1,32
U_CIRCBUFFER_READMEAN32_LBL1:
					ld MacroTMP2,Z+					; get data
					W_ADDB Y,MacroTMP2				; add it alltogether
				dec MacroTMP1
				brne U_CIRCBUFFER_READMEAN32_LBL1

				; Divide by 32
				lsr YH
				ror YL								; /2
				lsr YH
				ror YL								; /4
				lsr YH
				ror YL								; /8
				lsr YH
				ror YL								; /16
				lsr YH
				ror YL								; /32

				mov @0,YL

			W_POP Z
			W_POP Y
			W_POP X

.ENDM

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
.MACRO U_BIN8TOBCH
			// 8-bit Binary to BCH conversion
			// U_BIN8TOBCD16 Rs,k   0<=s<=25,$0000<=k<=$FFFFd		,max ~70 cycles
			/* U_BIN8TOBCD16 
						@0   R16,
						@1   RD_SEG_DATA - usage example */

			W_PUSH X
			
				M_CLR MacroTMP2						;MSD
				mov MacroTMP1,@0					;LSD
					subi MacroTMP1,16
					brcs PC+3						;abort if carry bit is set
					inc	MacroTMP2					;inc MSD
				rjmp PC-3							;loop again
				subi MacroTMP1,-16					;compensate extra subtraction, result is LSD

				sts (@1+0), Reg00
				sts (@1+1), MacroTMP2				;Little endian!
				sts (@1+2), MacroTMP1				;Little endian!

			W_POP X

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_BIN8TOBCD
			// 8-bit Binary to BCD conversion
			// U_BIN8TOBCD Rs,k   0<=s<=25,$0000<=k<=$FFFF		,max ~50 cycles
			/* U_BIN8TOBCD 
						@0   R16,
						@1   RD_SEG_DATA - usage example */

			W_PUSH X

				M_CLR MacroTMP2						;MSD 1
				mov	MacroTMP1,@0					;LSD register is a temp one
					subi MacroTMP1,100
					brcs PC+3						;abort if carry bit is set
					inc	MacroTMP2					;inc MSD
				rjmp PC-3							;loop again
				subi MacroTMP1,-100					;compensate extra subtraction
				sts (@1+0), MacroTMP2

				M_CLR MacroTMP2						;MSD 2
					subi MacroTMP1,10
					brcs PC+3						;abort if carry bit is set
					inc	MacroTMP2					;inc MSD
				rjmp PC-3							;loop again
				subi MacroTMP1,-10					;compensate extra subtraction,, result is LSD

				sts (@1+1), MacroTMP2				;Little endian!			
				sts (@1+2), MacroTMP1				;Little endian!

			W_POP X

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_BIN16TOBCD
			// 16-bit Binary to BCD conversion
			// U_BIN16TOBCD Rs,k   X<=s<=Z,$0000<=k<=$FFFF   ,max ~220 cycles
			/* U_BIN16TOBCD 
						@0   X,
						@1   RD_SEG_DATA - usage example */
			
.DEF BIN16L = R18
.DEF BIN16H = R19
	
			push RegTMP1							;The source data is somewhere in X,Y,Z registers
			push BIN16L								;So cant use common MacroTMP* registers here
			push BIN16H

				mov BIN16L,@0L
				mov BIN16H,@0H

				M_CLR RegTMP1
				sts (@1+0),RegTMP1

				//M_CLR RegTMP1
					subi BIN16L,Low(10000)			;10000
					sbci BIN16H,High(10000)
					brcs PC+3						;abort if carry bit is set
					inc	RegTMP1						;inc digit
				rjmp PC-4							;loop again
				subi BIN16L,Low(-10000)				;compensate extra subtraction
				sbci BIN16H,High(-10000)
				sts (@1+1),RegTMP1

				M_CLR RegTMP1
					subi BIN16L,Low(1000)			;1000
					sbci BIN16H,High(1000)
					brcs PC+3						;abort if carry bit is set
					inc	RegTMP1						;inc digit
				rjmp PC-4							;loop again
				subi BIN16L,Low(-1000)				;compensate extra subtraction
				sbci BIN16H,High(-1000)
				sts (@1+2),RegTMP1

				M_CLR RegTMP1
					subi BIN16L,Low(100)			;100
					sbci BIN16H,High(100)
					brcs PC+3						;abort if carry bit is set
					inc	RegTMP1						;inc digit
				rjmp PC-4							;loop again
				subi BIN16L,Low(-100)				;compensate extra subtraction
				sbci BIN16H,High(-100)
				sts (@1+3),RegTMP1

				M_CLR RegTMP1
					subi BIN16L,Low(10)				;10
					sbci BIN16H,High(10)
					brcs PC+3						;abort if carry bit is set
					inc	RegTMP1						;inc digit
				rjmp PC-4							;loop again
				subi BIN16L,Low(-10)				;compensate extra subtraction
				sbci BIN16H,High(-10)
				sts (@1+4),RegTMP1

				sts (@1+5),BIN16L

			pop BIN16H
			pop BIN16L
			pop RegTMP1
			
.UNDEF BIN16L
.UNDEF BIN16H

.ENDM

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
.MACRO U_TABLEENCODE
			// Convert BCD or BCH code to segment or LCD indicator code (in-memory), according to static encode table in flash memory
			// U_TABLEENCODE k,f   $0000<=k,f<=$FFFF   ,17 cycles
			/* U_TABLEENCODE 
						@0   RD_SEG_DATA,
						@1   FD_BCDtoSEG - usage example */

			push MacroTMP1
			W_PUSH Z

				lds MacroTMP1,@0					;get data to convert
				W_LDI Z,(@1*2)						;load base adress
				W_ADDB Z,MacroTMP1					;apply offset
				lpm MacroTMP1,Z						;get converted data
				sts @0,MacroTMP1					;store it

			W_POP Z
			pop MacroTMP1

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_BLOCK_TABLEENCODE
			// Convert block of BCD or BCH codes to segment or LCD  indicator code (in-memory), according to static encode table in flash memory
			// U_BLOCK_TABLEENCODE k,K,f   $0000<=k<=$FFFF,$00<=K<=$FF   ,min 26 (+14 per each additional byte in the block) cycles
			/* U_BLOCK_TABLEENCODE 
						@0   RD_SEG_DATA,
						@1   SEG_DATA_LEN,
						@2   FD_BCDtoSEG - usage example */

			W_PUSH X
			W_PUSH Y
			W_PUSH Z

				W_LDI Y,@0							;data pointer
				ldi MacroTMP2,@1
U_BLOCK_TABLEENCODE_LBL1:
					ld MacroTMP1,Y					;get data to convert				
					W_LDI Z,(@2*2)					;load base adress
					W_ADDB Z,MacroTMP1				;apply offset
					lpm MacroTMP1,Z					;get converted data
					st Y+,MacroTMP1					;store it
				dec MacroTMP2
				brne U_BLOCK_TABLEENCODE_LBL1

			W_POP Z
			W_POP Y
			W_POP X

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_FLASH_BLOCK_TABLEENCODE
			// Convert block of BCD or BCH codes to segment or LCD  indicator code (from flash to memory), according to static encode table in flash memory
			// U_FLASH_BLOCK_TABLEENCODE k,K,f   $0000<=k<=$FFFF,$00<=K<=$FF   ,min 48 (+11 per each additional byte in the block) cycles
			/* U_FLASH_BLOCK_TABLEENCODE 
						@0   FD_LCDMenuItems,
						@1   RD_LCD_MENU_ITEMS_LEN,
						@2   RD_LCD_MENU_ITEMS,
						@3   FD_ASCIItoLCD - usage example */

			push RegTMP1
			push RegTMP2
			W_PUSH X								;input
			W_PUSH Y								;output
			W_PUSH Z								;table

				W_LDI X,(@0*2)
				W_LDI Y,@2

				ldi RegTMP1,@1
U_FLASH_BLOCK_TABLEENCODE_LBL1:	
					movw R30,R26
					lpm RegTMP2,Z+					;get data to convert
					movw R26,R30
								
					W_LDI Z,(@3*2)					;load base adress
					W_ADDB Z,RegTMP2				;apply offset
					lpm RegTMP2,Z					;get converted data
													;reusing RegTMP2 here!
					st Y+,RegTMP2					;store it
				dec RegTMP1
				brne U_FLASH_BLOCK_TABLEENCODE_LBL1

			W_POP Z
			W_POP Y
			W_POP X
			pop RegTMP2
			pop RegTMP1

.ENDM

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
.MACRO U_LCD_INIT
			// Put "initialize LCD" command into LCD FIFO
			// 336 cycles, 9 bytes in FIFO
			/* U_LCD_INIT - usage example */

			; DB4 DB5 DB6 DB7   RS RW E -
			; 7   6   5   4     3  2  1 0
			
			push RegTMP1

				; Configure 4-line mode
				; 0010 1000

				ldi RegTMP1,0b_0010_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR
				; start from the beginning in 4-wire mode
				ldi RegTMP1,0b_0010_0010	
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR
				ldi RegTMP1,0b_1000_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR

				; Cursor style
				; 0000 1111

				; first nibble
				ldi RegTMP1,0b_0000_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR
				; second nibble
				ldi RegTMP1,0b_1100_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR

				; Cursor move direction
				; 0000 0110

				; first nibble
				ldi RegTMP1,0b_0000_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR
				; second nibble
				ldi RegTMP1,0b_0110_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR

				; Clear
				; 0000 0001

				; first nibble
				ldi RegTMP1,0b_0000_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR
				; second nibble
				ldi RegTMP1,0b_0001_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR

			pop RegTMP1

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_LCD_CLR
			// Put "Clear LCD" command into LCD FIFO
			// 96 cycles, 2 bytes in FIFO
			/* U_LCD_CLR - usage example */

			push RegTMP1

				; Clear
				; 0000 0001

				; first nibble
				ldi RegTMP1,0b_0000_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR
				; second nibble
				ldi RegTMP1,0b_0001_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR

			pop RegTMP1

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_LCD_DDADDR
			// Put "Set DDRAM address" command into LCD FIFO
			// 110 cycles, 2 bytes in FIFO
			/* U_LCD_DDADDR 
						@0   R16 - usage example */

			push RegTMP1
			push RegTMP1

				; Write address
				;1DDD DDDD
				
				; first nibble
				mov RegTMP1,@0
				andi RegTMP1,0b_1111_0000					;masking
				ori RegTMP1,0b_1000_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR
				
			pop RegTMP1									;restore spoiled data when @0==RegTMP

				; second nibble
				mov RegTMP1,@0
				swap RegTMP1
				andi RegTMP1,0b_1111_0000					;masking
				ori RegTMP1,0b_0000_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR

			pop RegTMP1
.ENDM

//--------------------------------------------------------------------------------
.MACRO U_LCD_CGADDR
			// Put "Set SGRAM address" command into LCD FIFO
			// 105 cycles, 2 bytes in FIFO
			/* U_LCD_CGADDR
						@0   R16 - usage example */

			push RegTMP1
			push RegTMP1

				; Write address
				; 01GG GGGG

				; first nibble
				mov RegTMP1,@0
				andi RegTMP1,0b_0111_0000					;masking
				ori RegTMP1,0b_0100_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR
				
				pop RegTMP1									;restore spoiled data when @0==RegTMP
				
				; second nibble
				mov RegTMP1,@0
				swap RegTMP1
				andi RegTMP1,0b_1111_0000					;masking
				ori RegTMP1,0b_0000_0010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR

			pop RegTMP1
.ENDM

//--------------------------------------------------------------------------------
.MACRO U_LCD_DATA
			// Put data into LCD FIFO and shift cursor right
			// 105 cycles, 2 bytes in FIFO
			/* U_LCD_DATA
						@0   R16 - usage example */

			push RegTMP1
			push RegTMP1
				
				; Write data
				; dddd dddd
				
				; first nibble
				mov RegTMP1,@0
				andi RegTMP1,0b_1111_0000					;masking
				ori RegTMP1,0b_0000_1010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR

				pop RegTMP1									;restore spoiled data when @0==RegTMP

				; second nibble
				mov RegTMP1,@0
				swap RegTMP1
				andi RegTMP1,0b_1111_0000					;masking
				ori RegTMP1,0b_0000_1010
				U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR

			pop RegTMP1
.ENDM

//--------------------------------------------------------------------------------
.MACRO U_LCD_BLOCK_DATA
			// Put block of data into LCD FIFO and shift cursor right
			// ~100 (+~100 per each additional byte) cycles, 2 bytes in FIFO per each data byte
			/* U_LCD_BLOCK_DATA
						@0   RD_SOMEDATA,
						@1   RD_SOMEDATA_LEN - usage example */

			push RegTMP1
			push RegTMP2
			W_PUSH Y

				W_LDI Y,@0
				ldi RegTMP2,@1

					; Write data
					; dddd dddd

U_LCD_BLOCK_DATA_LBL1:			
					; first nibble
					ld RegTMP1,Y
					andi RegTMP1,0b_1111_0000					;masking
					ori RegTMP1,0b_0000_1010
					U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR
					; second nibble
					ld RegTMP1,Y+
					swap RegTMP1
					andi RegTMP1,0b_1111_0000					;masking
					ori RegTMP1,0b_0000_1010
					U_FIFO_WRITE RegTMP1,RD_LCD_FIFO,LCD_FIFO_LEN,RD_LCD_WPTR,RD_LCD_RPTR

					dec RegTMP2
				breq PC+3
				jmp U_LCD_BLOCK_DATA_LBL1

			W_POP Y
			pop RegTMP2
			pop RegTMP1

.ENDM

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
.MACRO U_BLOCK_MOVE
			// Move data block fron SRAM to SRAM
			// 33 (+4 per each additional byte) cycles 
			/* U_BLOCK_MOVE
						@0   RD_SOURCE,
						@1   RD_DESTINATION
						@2   BLOCK_LEN - usage example */

			W_PUSH X
			W_PUSH Y
			W_PUSH Z
	
				W_LDI Y,@0
				W_LDI Z,@1
	
				ldi MacroTMP1,@2
U_BLOCK_MOVE_LBL1:
					ld MacroTMP2,Y+
					st Z+,MacroTMP2
					dec MacroTMP1
				brne U_BLOCK_MOVE_LBL1
	
			W_POP Z
			W_POP Y
			W_POP X

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_BLOCK_CLEAR
			// Clear SRAM data block
			// 15 (+4 per each additional byte) cycles 
			/* U_BLOCK_MOV
						@0   RD_SOURCE,
						@1   BLOCK_LEN - usage example */

			W_PUSH Y
	
				W_LDI Y,@0
	
				ldi MacroTMP1,@1
U_BLOCK_CLEAR_LBL1:
					st Y+,Reg00
					dec MacroTMP1
				brne U_BLOCK_CLEAR_LBL1
	
			W_POP Y

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_BLOCK_SEGDISTRIB8
			// Redistribute data from temporary buffer into segindicator-dedicated one
			// 12 cycles 
			/* U_BLOCK_SEGDISTRIB8
						@0   RD_SOURCE,
						@1   RD_DESTINATION - usage example */

			lds MacroTMP1,(@0+0)
			sts (@0+4),MacroTMP1
			lds MacroTMP1,(@0+1)
			sts (@0+2),MacroTMP1
			lds MacroTMP1,(@0+2)
			sts (@0+0),MacroTMP1

.ENDM

//--------------------------------------------------------------------------------
.MACRO U_BLOCK_SEGDISTRIB16
			// Redistribute data from temporary buffer into segindicator-dedicated one
			// 24 cycles 
			/* U_BLOCK_SEGDISTRIB16
						@0   RD_SOURCE,
						@1   RD_DESTINATION - usage example */

			lds MacroTMP1,(@0+0)
			sts (@0+5),MacroTMP1
			lds MacroTMP1,(@0+1)
			sts (@0+3),MacroTMP1
			lds MacroTMP1,(@0+2)
			sts (@0+1),MacroTMP1

			lds MacroTMP1,(@0+3)
			sts (@0+4),MacroTMP1
			lds MacroTMP1,(@0+4)
			sts (@0+2),MacroTMP1
			lds MacroTMP1,(@0+5)
			sts (@0+0),MacroTMP1

.ENDM

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

.ENDIF

