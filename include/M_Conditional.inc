//--------------------------------------------------------------------------------
// Konstantin Pavlov, pavlovconst@gmail.com
//--------------------------------------------------------------------------------

// INFO --------------------------------------------------------------------------------
//
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

.IFNDEF	M_CONDITIONAL_INCLUDED
.EQU M_CONDITIONAL_INCLUDED = 1

//--------------------------------------------------------------------------------
.MACRO C_IEQ
			// If equal
			// M_IEQ Rd,Rs,k,l   0<=d<=31,0<=s<=31,0<=k<=$FFFF,0<=l<=$FFFF   4-5 cycles

			// M_IEQ R16,R17,LBL_A_Act1,LBL_A_Act2 - usage example
			// use secondary rjmp/jmp just after the M_IEQ instruction
			// to make jumps longer than 64 instructions

			cp @0,@1				;1
			breq @2					;1-2		// limited by 64 instructions
			rjmp @3					;2
.ENDM

//--------------------------------------------------------------------------------
.MACRO C_IEQI
			// If equal
			// M_IEQI Rd,K,k,l   0<=d<=31,0<=K<=$FF,0<=k<=$FFFF,0<=l<=$FFFF   4-5 cycles

			// M_IEQI R16,0,LBL_A_Act1,LBL_A_Act2 - usage example
			// use secondary rjmp/jmp just after the M_IEQI instruction
			// to make jumps longer than 64 instructions

			cpi @0,@1
			breq @2							// limited by 64 instructions
			rjmp @3
.ENDM

//--------------------------------------------------------------------------------
.MACRO C_IGT
			// If greater then
			// M_IGT Rd,Rs,k,l   0<=d<=31,0<=s<=31,0<=k<=$FFFF,0<=l<=$FFFF   4-5 cycles

			// M_IGT R16,R17,LBL_A_Act1,LBL_A_Act2 - usage example
			// use secondary rjmp/jmp just after the M_IGT instruction
			// to make jumps longer than 64 instructions

			cp @1,@0
			brcs @2							// limited by 64 instructions
			rjmp @3
.ENDM

//--------------------------------------------------------------------------------
.MACRO C_IGTI
			// If greater then
			// M_IGTI Rd,K,k,l   0<=d<=31,0<=K<=$FF,0<=k<=$FFFF,0<=l<=$FFFF   4-5 cycles

			// M_IGTI R16,0,LBL_A_Act1,LBL_A_Act2 - usage example
			// use secondary rjmp/jmp just after the M_IGTI instruction
			// to make jumps longer than 64 instructions

			cpi @1,@0
			brcs @2							// limited by 64 instructions
			rjmp @3
.ENDM

//--------------------------------------------------------------------------------
.MACRO C_ILT
			// If lower then
			// M_ILT Rd,Rs,k,l   0<=d<=31,0<=s<=31,0<=k<=$FFFF,0<=l<=$FFFF   4-5 cycles

			// M_ILT R16,R17,LBL_A_Act1,LBL_A_Act2 - usage example
			// use secondary rjmp/jmp just after the M_ILT instruction
			// to make jumps longer than 64 instructions

			cp @0,@1
			brcs @2							// limited by 64 instructions
			rjmp @3
.ENDM

//--------------------------------------------------------------------------------
.MACRO C_ILTI
			// If lower then
			// M_ILTI Rd,K,k,l   0<=d<=31,0<=K<=$FF,0<=k<=$FFFF,0<=l<=$FFFF   4-5 cycles

			// M_ILTI R16,0,LBL_A_Act1,LBL_A_Act2 - usage example
			// use secondary rjmp/jmp just after the M_ILTI instruction
			// to make jumps longer than 64 instructions

			cpi @0,@1
			brcs @2							// limited by 64 instructions
			rjmp @3
.ENDM

//--------------------------------------------------------------------------------
.MACRO C_IBR
			// If register bit set
			// M_IBR Rd,b,k,l   0<=d<=31,0<=b<=7,0<=k<=$FFFF,0<=l<=$FFFF   5-7 cycles

			// M_IBR R16,3,LBL_A_Act1,LBL_A_Act2 - usage example

			sbrc @0,@1				;1-3   skips the next instruction if the bit is cleared.
			rjmp @2					;2
			rjmp @3					;2
.ENDM

//--------------------------------------------------------------------------------
.MACRO C_IBI
			// If I/O register bit set
			// M_IBI Rd,b,k,l   0<=d<=31,0<=b<=7,0<=k<=$FFFF,0<=l<=$FFFF   6-9 cycles

			// M_IBI PIND,4,LBL_A_Act1,LBL_A_Act2 - usage example

			M_IN ZL,@0			;1-2
			C_IBR ZL,@1,@2,@3	;5-7
.ENDM

//--------------------------------------------------------------------------------
.MACRO C_IBEQ
			// If register bits equal
			// M_IBEQ Rd,b,Rs,c,k,l   0<=d<=29,0<=s<=29,0<=b<=7,0<=c<=7,
			//							0<=k<=$FFFF,0<=l<=$FFFF   11-13 cycles

			// M_IBEQ R16,3,R17,7,LBL_A_Act1,LBL_A_Act2 - usage example

			mov @0,ZL				;1
			lsr ZL,@1				;1

			mov @2,ZH				;1
			lsr ZH,@3				;1

			com ZH					;1
			and ZL,ZH				;1

			sbrc ZL,0				;1-3
			rjmp @4					;2
			rjmp @5					;2
.ENDM

//--------------------------------------------------------------------------------
.MACRO CS_IEQ
			// If equal
			// M_IEQ Rd,Rs   0<=d<=31,0<=s<=31   4-5 cycles

			// M_IEQ R16,R17,LBL_A_Act1,LBL_A_Act2 - usage example
			// use secondary rjmp/jmp just after the M_IEQ instruction
			// to make jumps longer than 64 instructions

			cp @0,@1				;1
			breq PC+2				;1-2		// limited by 64 instructions
			rjmp PC+2				;2
.ENDM

//--------------------------------------------------------------------------------
.MACRO CS_IEQI
			// If equal
			// M_IEQI Rd,K   0<=d<=31,0<=K<=$FF   4-5 cycles

			// M_IEQI R16,0,LBL_A_Act1,LBL_A_Act2 - usage example
			// use secondary rjmp/jmp just after the M_IEQI instruction
			// to make jumps longer than 64 instructions

			cpi @0,@1
			breq PC+2							// limited by 64 instructions
			rjmp PC+2
.ENDM

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

.ENDIF


