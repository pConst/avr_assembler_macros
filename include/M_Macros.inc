//--------------------------------------------------------------------------------
// Konstantin Pavlov, pavlovconst@gmail.com
//--------------------------------------------------------------------------------

// INFO --------------------------------------------------------------------------------
//
// TODO: All word operations must be atomic!!!
// WARNING: Cycle calculations could be inaccurate
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

.IFNDEF	M_MACROS_INCLUDED
.EQU M_MACROS_INCLUDED = 1

//--------------------------------------------------------------------------------
// STACK
//--------------------------------------------------------------------------------
/*.MACRO	M_PUSH
			// Push byte
			// M_PUSH Rs   0<=s<=32   ,2 cycles

			// M_PUSH R0 - usage example

			push @0
.ENDM */

//--------------------------------------------------------------------------------
/*.MACRO	M_POP
			// Pop byte
			// M_POP Rd   0<=d<=32   ,2 cycles

			// M_POP R0 - usage example

			pop	@0
.ENDM */

//--------------------------------------------------------------------------------
.MACRO	W_PUSH
			// Push word
			// W_PUSH Rs   X<=s<=Z   ,4 cycles

			// W_PUSH X - usage example


			push @0L
			push @0H
.ENDM

//--------------------------------------------------------------------------------
.MACRO	W_POP
			// Pop word
			// W_POP Rd   X<=d<=Z   ,4 cycles

			// W_POP X - usage example

			pop	@0H
			pop	@0L
.ENDM

//--------------------------------------------------------------------------------
.MACRO	SR_PUSH
			// Push SREG
			// SR_PUSH   ,8 cycles

			// SR_PUSH - usage example

			push ZL
			sts SREG,ZL
			push ZL
.ENDM

//--------------------------------------------------------------------------------
.MACRO	SR_POP
			// Pop SREG
			// SR_POP   ,8 cycles

			// SR_POP - usage example

			pop ZL
			lds ZL,SREG
			pop ZL
.ENDM


//--------------------------------------------------------------------------------
// ADD
//--------------------------------------------------------------------------------
.MACRO M_ADD
			// Add
			// M_ADD Rd,Rs   0<=d<=31,0<=s<=31   ,1 cycle

			// M_ADD R16,R17 - usage example
			
			add @0,@1
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_ADDI
			// Load immediate
			// M_ADDI Rd,K   0<=d<=31,0<=K<=$FF   ,6 cycles

			// M_ADDI R16,$FF - usage example
			
			///subi @0,-@1
			push ZL
				ldi ZL,@1
				add @0,ZL
			pop ZL
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_ADDC
			// Add with carry
			// M_ADDC Rd,Rs   0<=d<=31,0<=s<=31   ,1 cycle

			// M_ADDC R16,R17 - usage example
			
			adc @0,@1
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_ADDCI
			// Add immediate with carry
			// M_ADDCI Rd,K   0<=d<=30,0<=K<=$FF   ,6 cycles

			// M_ADDCI R16,$FF - usage example
			
			///sbci @0,-@1
			push ZL
				ldi ZL,@1
				adc @0,ZL
			pop ZL
.ENDM


//--------------------------------------------------------------------------------
// SUBTRACT
//--------------------------------------------------------------------------------
.MACRO M_SUB
			// Subtract
			// M_SUB Rd,Rs   0<=d<=31,0<=s<=31   ,1 cycle

			// M_SUB R16,R17 - usage example
			
			sub @0,@1
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_SUBI
			// Subtract immediate
			// M_SUBI Rd,K   0<=d<=30,0<=K<=$FF   ,6 cycles

			// M_SUBI R16,$FF - usage example
			
			///subi @0,@1
			push ZL
				ldi ZL,@1
				sub @0,ZL
			pop ZL
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_SUBC
			// Subtract with carry
			// M_SUBC Rd,Rs   0<=d<=31,0<=s<=31   ,1 cycle

			// M_SUBC R16,R17 - usage example
			
			sbc @0,@1
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_SUBCI
			// Subtract immediate with carry
			// M_SUBCI Rd,K   0<=d<=30,0<=K<=$FF   ,6 cycles

			// M_SUBCI R16,$FF - usage example
			
			///sbci @0,@1
			push ZL
				ldi ZL,@1
				sbc @0,ZL
			pop ZL
.ENDM

//--------------------------------------------------------------------------------
// WORD ADD
//--------------------------------------------------------------------------------
.MACRO W_ADD
			// Word add
			// W_ADD Rd,Rs   X<=d,s<=Z   ,2 cycles

			// W_ADD X,Y - usage example

			add @0L,@1L
			adc @0H,@1H
.ENDM

//--------------------------------------------------------------------------------
.MACRO	W_ADDI
			// Word add immediate
			// W_ADDI Rd,k   X<=d<=Z,$0000<=k<=$FFFF   ,2 cycles

			// W_ADDI X,$1234 - usage example

			subi @0L,Low(-@1)
			sbci @0H,High(-@1)
.ENDM

//--------------------------------------------------------------------------------
.MACRO W_ADDB
			// Word add byte
			// W_ADDB Rd,Rs   X<=d<=Z,0<=s<=31   ,2 cycles

			// W_ADDB X,R16 - usage example

			add @0L,@1
			adc @0H,Reg00
.ENDM

//--------------------------------------------------------------------------------
// WORD SUBTRACT
//--------------------------------------------------------------------------------
.MACRO W_SUB
			// Word subtract
			// W_SUB Rd,Rs   X<=d,s<=Z   ,2 cycles

			// W_SUB X,Y - usage example

			sub @0L,@1L
			sbc @0H,@1H
.ENDM

//--------------------------------------------------------------------------------
.MACRO	W_SUBI
			// Word subtract immediate
			// W_SUBI Rd,k   X<=d<=Z,$0000<=k<=$FFFF   ,2 cycles

			// W_SUBI X,$1234 - usage example

			subi @0L,Low(@1)
			sbci @0H,High(@1)
.ENDM

//--------------------------------------------------------------------------------
.MACRO W_SUBB
			// Word subtract byte
			// W_SUBB Rd,Rs   X<=d<=Z,0<=s<=31   ,2 cycles

			// W_SUBB X,R16 - usage example

			sub @0L,@1
			sbc @0H,Reg00
.ENDM

//--------------------------------------------------------------------------------
.MACRO	W_INC
			// Word increase
			// W_INC Rd   X<=d<=Z   ,2 cycles

			// W_INC X - usage example

			W_ADDB @0,Reg01
.ENDM

//--------------------------------------------------------------------------------
.MACRO	W_DEC
			// Word decrease
			// W_DEC Rd   X<=d<=Z   ,2 cycles

			// W_DEC X - usage example

			W_SUBB @0,Reg01
.ENDM

//--------------------------------------------------------------------------------
// WORD COMPARE
//--------------------------------------------------------------------------------
.MACRO W_COM
			// Word one`s complement (inverse all bits)
			// W_COM Rd   X<=d<=Z   ,2 cycles

			// W_COM X - usage example

			com @0L
			com @0H
.ENDM

//--------------------------------------------------------------------------------
.MACRO W_NEG
			// Word two`s complement
			// W_NEG Rd   X<=d<=Z   ,4 cycles

			// W_NEG X - usage example

			com @0L
			com @0H
			subi @0L,Low(-1)
			sbci @0H,High(-1)
.ENDM

//--------------------------------------------------------------------------------
.MACRO W_CP
			// Word compare
			// W_CP Rd,Rs   X<=d,s<=Z   ,2 cycles

			// W_CP X,Y - usage example

			cp @0L,@1L
			cpc @0H,@1H
.ENDM

//--------------------------------------------------------------------------------
.MACRO W_CPI
			// Word compare immediate
			// W_CPI Rd,k   X<=d<=Z,$0000<=k<=$FFFF   ,7 cycles

			// W_CPI X,$1234 - usage example
			
			cpi @0L,low(@1)
			push R16
				ldi R16,high(@1)
				cpc @0H,R16
			pop R16
.ENDM

//--------------------------------------------------------------------------------
// LOAD IMMEDIATE
//--------------------------------------------------------------------------------
/*.MACRO M_LDI
			// Load immediate to R16-R31 registers
			// M_LDI Rd,K   16<=d<=31,0<=K<=$FFFF   ,1 cycle

			// M_LDI R16,$AFAF - usage example
			
			ldi @0,@1
.ENDM */

//--------------------------------------------------------------------------------
.MACRO M_LDIL
			// Load immediate to R0-R16 registers
			// M_LDIL Rd,K   0<=d<=15,0<=K<=$FFFF   ,4 cycles

			// M_LDIL R0,$AFAF - usage example
			
			push ZL
				ldi ZL,@1
				mov @0,ZL
			pop ZL
.ENDM

//--------------------------------------------------------------------------------
.MACRO W_LDI
			// Load immediate word to X,Y,Z registers
			// W_LDI Rd,k   X<=d<=Z,$0000<=k<=$FFFF   ,2 cycles

			// W_LDI X,$AFAF - usage example
			
			ldi @0L,Low(@1)
			ldi @0H,High(@1)
.ENDM

//--------------------------------------------------------------------------------
// PROGRAM MEMORY
//--------------------------------------------------------------------------------
/*.MACRO M_LPM
			// Read (byte) data from program memory
			// M_LPM Rd,Z   0<=d<=29,0<=Z<=$FFFF   ,13 cycles

			// M_LPM R16,FD_data - usage example

			ldi ZL,Low(@1*2)
			ldi ZH,High(@1*2)
			lpm @0,Z
.ENDM */

//--------------------------------------------------------------------------------
// IO MEMORY
//--------------------------------------------------------------------------------
.MACRO M_IN
			// Universal I/O register input (memory mapped registers workaround)
			// M_IN Rd,k   0<=d<=31,0<=k<=$FF   ,max 2 cycles

			// M_IN R16,UDR - usage example

.IF @1<$40
			in @0,@1
.ELSE
			lds @0,@1
.ENDIF
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_OUT
			// Universal I/O register output (memory mapped registers workaround)
			// M_OUT k,Rs   0<=s<=31,0<=k<=$FF   ,max 2 cycles

			// M_OUT UDR,R16 - usage example

.IF @0<$40
			out @0,@1
.ELSE
			sts @0,@1
.ENDIF
.ENDM

//--------------------------------------------------------------------------------
// DATA MEMORY
//--------------------------------------------------------------------------------
/*.MACRO M_LDS
			// Load from data space
			// M_LDS Rd,K   0<=d<=31,0<=K<=$FF   ,2 cycles

			// M_LDS R16,RD_ByteVariable - usage example

			lds @0,@1
.ENDM */

//--------------------------------------------------------------------------------
/*.MACRO M_STS
			// Store to data space
			// M_STS K,Rs   0<=s<=31,0<=K<=$FF   ,2 cycles

			// M_STS RD_ByteVariable,R16 - usage example

			sts @0,@1
.ENDM */
//--------------------------------------------------------------------------------
.MACRO W_LDS
			// Load word from data space
			// W_LDS Rd,K   X<=d<=Z,$0000<=K<=$FFFF   ,4 cycles

			// W_LDS X,RD_WordVariable - usage example

			lds @0L,@1
			lds @0H,@1+1
.ENDM

//--------------------------------------------------------------------------------
.MACRO W_STS
			// Store word to data space
			// W_STS K,Rs   X<=s<=Z,$0000<=K<=$FFFF   ,4 cycles

			// W_STS RD_WordVariable,X - usage example

			sts @0,@1L
			sts @0+1,@1H
.ENDM

//--------------------------------------------------------------------------------
// REGISTER BITS
//--------------------------------------------------------------------------------

.MACRO M_CLR
			// Clear register
			// M_CLR Rd   0<=Rd<=31   ,1 cycle

			// M_CLR R16 - usage example

			clr @0
.ENDM

//--------------------------------------------------------------------------------
.MACRO W_CLR
			// Clear register pair
			// W_CLR Rd   X<=Rd<=Z   ,2 cycles

			// W_CLR X - usage example

			clr @0L
			clr @0H
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_SBR
			// Set register bit
			// M_SBR Rd,b   0<=Rd<=30,0<=b<=7   ,6 cycles
			//original sbr works only with 16<=Rd<=31 registers

			// M_SBR R16,0 - usage example
			
			push ZL
				ldi ZL,1<<@1
				or @0,ZL
			pop ZL
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_CBR
			// Clear register bit
			// M_CBR Rd,b   0<=Rd<=30,0<=b<=7   ,6 cycles
			//original cbr works only with 16<=Rd<=31 registers

			// M_CBR R16,0 - usage example

			push ZL
				ldi ZL,~(1<<@1)
				and @0,ZL
			pop ZL
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_IBR
			// Inverse register bit
			// M_IBR Rd,b   0<=Rd<=30,0<=b<=7   ,6 cycles

			// M_IBR R16,0 - usage example

			push ZL
				ldi ZL,1<<@1
				eor @0,ZL
			pop ZL
.ENDM

//--------------------------------------------------------------------------------
// IO BITS
//--------------------------------------------------------------------------------
.MACRO M_SBI
			// Set IO register bit (memory mapped registers workaround)
			// M_SBI k,b   $00<=k<=$FF,0<=b<=7   ,max 7 cycles

			// M_SBI DDRA,0 - usage example

.IF @0<$20
			sbi @0,@1				;1
.ELSE
			push ZL					;1
	.IF @0<$40
				in ZL,@0			;1
				ori ZL,1<<@1		;1
				out @0,ZL			;1
	.ELSE							; Memory mapped
				lds ZL,(@0+$0020)	;2
				ori ZL,1<<@1		;1
				sts (@0+$0020),ZL	;2
	.ENDIF
			pop ZL					;1
.ENDIF
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_CBI
			// Clear IO register bit (memory mapped registers workaround)
			// M_CBI k,b   $00<=k<=$FF,0<=K<=$FF   ,max 7 cycles

			// M_CBI DDRA,0 - usage example

.IF @0<$20
			cbi @0,@1
.ELSE
			push ZL
	.IF @0<$40
				in ZL,@0
				andi ZL,~(1<<@1)
				out @0,ZL
	.ELSE							; Memory mapped
				lds ZL,(@0+$0020)
				andi ZL,~(1<<@1)
				sts (@0+$0020),ZL
	.ENDIF
			pop ZL
.ENDIF
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_IBI
			// Inverse IO register bit (memory mapped registers workaround)
			// M_IBI k,b   $00<=k<=$FF,0<=K<=$FF   ,max 8 cycles

			// M_IBI DDRA,0 - usage example


			W_PUSH Z
.IF @0<$40
				in ZL,@0
				ldi ZH,(1<<@1)
				eor ZL,ZH
				out @0,ZL
.ELSE							; Memory mapped
				lds ZL,(@0+$0020)
				ldi ZH,(1<<@1)
				eor ZL,ZH
				sts (@0+$0020),ZL
.ENDIF
			W_POP Z
.ENDM

//--------------------------------------------------------------------------------
// MASKS
//--------------------------------------------------------------------------------
.MACRO M_SBRM
			// Set register by bit mask
			// M_SBRM Rd,K   0<=Rd<=30,0<=K<=$FF   ,6 cycles

			// M_SBRM R16,0b01010101 - usage example

			push ZL
				ldi ZL,@1
				or @0,ZL
			pop ZL
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_CBRM
			// Clear register by bit mask
			// M_CBRM Rd,b   0<=Rd<=30,0<=K<=$FF   ,7 cycles

			// M_CBRM R16,0b01010101 - usage example

			push ZL
				ldi ZL,@1
				com ZL
				and @0,ZL
			pop ZL
.ENDM

//--------------------------------------------------------------------------------
.MACRO M_IBRM
			// Inverse register by bit mask
			// M_IBRM Rd,b   0<=Rd<=31,0<=K<=$FF   ,6 cycles

			// M_IBRM R16,0b01010101 - usage example

			push ZL
				ldi ZL,@1
				eor @0,ZL
			pop ZL
.ENDM

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

.ENDIF

