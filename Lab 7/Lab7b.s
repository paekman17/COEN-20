.syntax unified
.cpu cortex-m4
.text

.global Times45
.thumb_func
.align
Times45: //45 = 3 * 3 * 5
         ADD R1,R0,R0,LSL 1 //R1 = 3*R0
         RSB R0,R1,R1,LSL 4 //R0 = 48 - 3 = 45
         BX LR

.global Times55
.thumb_func
.align
Times55: //55 = 5 * 11
         RSB R1,R0,R0,LSL 3 //R1 = 7*R0
         RSB R0,R0,R1,LSL 3 //R0 = 56 - 1 = 55
         BX LR

.global Times106
.thumb_func
.align
Times106: //106 = 105 + 1 = (15*7) + 1
          RSB R1,R0,R0,LSL 4 //R1 = 15*R0
          RSB R1,R1,R1,LSL 3 //R1 = 15*7*R0
          ADD R0,R0,R1 //R0 = 105 + 1
          BX LR

.global Times43691
.thumb_func
.align
Times43691: //43691 = 43690 + 1
            ADD R1,R0,R0,LSL 4 //R1 = 17*R0
            ADD R1,R1,R1,LSL 6 //R1 = 17*257*R0
            ADD R1,R1,R1,LSL 2 //R1 = 17*257*5*R0
            ADD R0,R0,R1,LSL 1 //R0 = 43690 + 1
            BX LR

.end
