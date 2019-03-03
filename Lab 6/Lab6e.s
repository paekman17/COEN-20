.syntax unified
.cpu cortex-m4
.text

.set BITBANDING,1 //comment out if not using bit banding

.ifdef BITBANDING

.global GetBit
.thumb_func
.align
GetBit: //R0 = uint16_t *bits, R1 = uint32_t row, R2 = uint32_t col
		SUB R3,R0,0x20000000
		LSL R3,R3,5
		LDR R12,=4
		MUL R1,R1,R12
		ADD R1,R1,R2 //index = R1 = (row*4) + col
		ADD R3,R3,R1,LSL 2
		ADD R3,R3,0x22000000
		LDRH R0,[R3]
		BX LR

.global PutBit
.thumb_func
.align
PutBit: //R0 = BOOL value, R1 = *bits, R2 = row, R3 = col
		PUSH {R4,LR}
		SUB R12,R1,0x20000000
		LSL R12,R12,5
		LDR R4,=4
		MUL R2,R2,R4
		ADD R2,R2,R3 //index = R2 = (row*4) + col
		ADD R12,R12,R2,LSL 2
		ADD R12,R12,0x22000000
		CMP R0,0 //R0 = 0 or 1
		ITE EQ
		LDREQ R4,=0
		LDRNE R4,=1
		STRH R4,[R12]
		POP {R4,PC}
		
.else

.global GetBit
.thumb_func
.align
GetBit: //R0 = uint16_t *bits, R1 = uint32_t row, R2 = uint32_t col
		LDRH R0,[R0]
		LDR R3,=4
		MUL R1,R1,R3
		ADD R1,R1,R2 //index = (row*4)+col
		LDR R12,=1
		LSL R12,R12,R1
		AND R0,R0,R12
		LSR R0,R0,R1
		BX LR

.global PutBit
.thumb_func
.align
PutBit: //R0 = BOOL value, R1 = *bits, R2 = row, R3 = col
		PUSH {R4,R5,LR}
		LDRH R5,[R1]
		LDR R12,=4
		MUL R2,R2,R12
		ADD R2,R2,R3 //index = R2 = (row*4)+col
		LDR R4,=1
		LSL R4,R4,R2
		CMP R0,0
		ITE EQ
		BICEQ R5,R5,R4
		ORRNE R5,R5,R4
		STRH R5,[R1]
		POP {R4,R5,PC}

.endif
.end