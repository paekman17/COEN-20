.syntax unified
.cpu cortex-m4
.text

.global FloatPoly
.thumb_func
.align
FloatPoly: //S0 = x, R0 = &a[0], R1 = n
		   VSUB.F32 S1,S1,S1 //sum = 0.0
		   VMOV S2,1.0
L1:		   CBZ R1,L2 //go to L2 if n = 0
		   VLDMIA R0!,{S3} //S3 = Ai
		   VMUL.F32 S4,S3,S2 //S4 = Ai*Xi
		   VADD.F32 S1,S1,S4 //total += Ai*Xi
		   VMUL.F32 S2,S2,S0 //Xi*X(i+1)
		   SUB R1,R1,1 //n--
		   B L1
L2:		   VMOV S0,S1
		   BX LR

.global FixedPoly
.thumb_func
.align
FixedPoly: //R0 = x, R1 = &a[0], R2 = n
		   PUSH {R4,R5,R6,R7,LR}
		   MOV R7,R0
		   LDR R3,=0 //R3 = sum
		   LDR R4,=1
		   LSL R4,R4,16//we have to multiply by 2^16 so that binary point is in middle
L3:		   CBZ R2,L4
		   LDMIA R1!,{R5} //R5 = Ai
		   SMULL R6,R5,R4,R5 //R6 = Ai*Xi (Q32.32)
		   LSR R6,R6,16 //Extract middle 32 bits
		   ORR R6,R6,R5,LSL 16
		   ADD R3,R3,R6 //total += Ai*Xi
		   SMULL R4,R7,R4,R0
		   LSR R4,R4,16
		   ORR R4,R4,R7,LSL 16 //R4 = Xi*X(i+1)
		   SUB R2,R2,1 //n--
		   B L3
L4:		   MOV R0,R3
		   POP {R4,R5,R6,R7,PC}
		   
.end