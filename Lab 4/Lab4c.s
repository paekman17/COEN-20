.syntax unified
.cpu cortex-m4
.text

.global MxPlusB
.thumb_func
.align
MxPlusB:	PUSH {R4,LR}
			MUL R1,R0,R1
			MUL R0,R1,R2
			ASR R0,R0,31
			MUL R0,R0,R2
			LSL R0,R0,1
			ADDS R0,R0,R2
			MOV R4,R2
			LDR R2,=2
			SDIV R0,R0,R2
			ADDS R0,R0,R1
			SDIV R0,R0,R4
			ADDS R0,R0,R3
			POP {R4,PC}
			
.end