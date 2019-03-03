.syntax unified
.cpu cortex-m4
.text

.global CopyCell
.thumb_func
.align
CopyCell:   //R0 = dst, R1 = src
            PUSH {LR}
            LDR R2,=0
            STR R2,row
            STR R2,col
outerLoop:  LDR R2,row //R2 = row
            CMP R2,60 //row < 60?
            BHS Done
innerLoop:  LDR R2,col //R2 = col
            CMP R2,60
            BHS innerDone //col < 60?
            LDR R3,[R1,R2,LSL 2] //R3 = src[col]
            STR R3,[R0,R2,LSL 2] //R3 -> dst[col]
            ADD R2,R2,1 //col++
            STR R2,col
            B innerLoop
innerDone:  LDR R2,row
            ADD R2,R2,1 //row++
            STR R2,row
            B outerLoop
Done:       ADD R0,R0,960
            ADD R1,R1,960
            POP {PC}

.global FillCell
.thumb_func
.align
FillCell:   //R0 = dst, R1 = pixel
            PUSH {LR}
            LDR R2,=0
            STR R2,row
            STR R2,col
outerLoop:  LDR R2,row //R2 = row
            CMP R2,60
            BHS Done
innerLoop:  LDR R2,col //R2 = col
            CMP R2,60
            BHS innerDone
            LDR R3,pixel
            STR R3,[R0,R2,LSL 2] //dst[col] = pixel
            ADD R2,R2,1 //col++
            STR R2,col
            B innerLoop
innerDone:  LDR R2,row
            ADD R2,R2,1 //row++
            STR R2,row
            B outerLoop
Done:       ADD R0,R0,960
            POP {PC}

.end
