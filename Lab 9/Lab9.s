.syntax unified
.cpu cortex-m4
.text

.global SIMD_USatAdd
.thumb_func
.align
SIMD_USatAdd: //R0 = uint8_t &bytes[0], R1 = uint32_t count, R2 = uint8_t amount
              PUSH {R4-R11}
              BFI R2,R2,8,8 //use BFI to other parts of the 4 bytes of R2 so we can add multiply
              BFI R2,R2,16,16 //bytes at a time
addLoop:      CMP R1,40
              BLT addCleanLoop //if less than 40 bytes remain, go to the next loop
              LDMIA R0,{R3-R12} //load registers from bytes array and add current amount
              UQADD8 R3,R3,R2
              UQADD8 R4,R4,R2
              UQADD8 R5,R5,R2
              UQADD8 R6,R6,R2
              UQADD8 R7,R7,R2
              UQADD8 R8,R8,R2
              UQADD8 R9,R9,R2
              UQADD8 R10,R10,R2
              UQADD8 R11,R11,R2
              UQADD8 R12,R12,R2
              STMIA R0!,{R3-R12} //store back into array
              SUB R1,R1,40
              B addLoop
addCleanLoop: CBZ R1, addDone //add 4 bytes at a time until we reach zero for count
              LDR R3,[R0]
              UQADD8 R3,R3,R2
              STR R3,[R0],4
              SUB R1,R1,4
              B addCleanLoop
addDone:      POP {R4-R11}
              BX LR

.global SIMD_USatSub
.thumb_func
.align
SIMD_USatSub: //R0 = uint8_t &bytes[0], R1 = uint32_t count, R2 = uint8_t amount
              PUSH {R4-R11}
              BFI R2,R2,8,8 //use BFI to other parts of the 4 bytes of R2 so we can add multiple
              BFI R2,R2,16,16 //bytes at a time
subLoop:      CMP R1,40
              BLT subCleanLoop //if less than 40 bytes remain, go to the next loop
              LDMIA R0,{R3-R12} //load registers from bytes array and subtract from current amount
              UQSUB8 R3,R3,R2
              UQSUB8 R4,R4,R2
              UQSUB8 R5,R5,R2
              UQSUB8 R6,R6,R2
              UQSUB8 R7,R7,R2
              UQSUB8 R8,R8,R2
              UQSUB8 R9,R9,R2
              UQSUB8 R10,R10,R2
              UQSUB8 R11,R11,R2
              UQSUB8 R12,R12,R2
              STMIA R0!,{R3-R12} //store back into array
              SUB R1,R1,40
              B subLoop
subCleanLoop: CBZ R1, subDone //subtract 4 bytes at a time until we reach zero for count
              LDR R3,[R0]
              UQSUB8 R3,R3,R2
              STR R3,[R0],4
              SUB R1,R1,4
              B subCleanLoop
subDone:      POP {R4-R11}
              BX LR

.end
