.syntax	unified
.cpu	cortex-m4
.text


.global	Ten32
.thumb_func
.align
Ten32:	PUSH {LR}
		LDR R0,=10
		POP {PC}
		
.global	Ten64
.thumb_func
.align
Ten64:	PUSH {LR}
		LDR R0,=10
		LDR R1,=0
		POP {PC}
		
.global	Incr
.thumb_func
.align
Incr:	PUSH {LR}
		ADD R0,R0,1
		POP {PC}

.global	Nested1
.thumb_func
.align
Nested1:	PUSH {R4,LR}
			MOV R4,R0
			BL rand
			ADD R0,R0,1
			POP {R4,PC}
			
.global	Nested2
.thumb_func
.align
Nested2:	PUSH {R4,R5,LR}
			MOV R4,R0
			BL rand
			MOV R5,R0
			BL rand
			ADD R0,R0,R5
			POP {R4,R5,PC}

.global	PrintTwo
.thumb_func
.align
PrintTwo:	PUSH {R4,R5,LR}
			MOV R4,R0
			MOV R5,R1
			BL printf
			ADD R5,R5,1
			MOV R0,R4
			MOV R1,R5
			BL printf
			POP {R4,R5,PC}
			
.end