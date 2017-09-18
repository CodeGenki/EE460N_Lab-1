.ORIG x3000

LEA R0, READ		
LDB R0, R0, #0		;R0 = 0x4000
LEA R1, WRITE		
LDB R1, R1, #0		;R1 = 0x4005
LEA R2, MASK	
LDB R2, R2, #0		;R2 = 0x4004

; Processing Byte 1 
LDB R3, R2, #0		;R3 has memory contents in 0x4004
AND R3, R3, x3		;Isolate right 2 bits

ADD R3, R3, R0		;R3 now has memory address of target data
LDB R3, R3, #0		;R3 has the target data
STB R3, R1, #0		;Writing R3 to 0x4005


; Processing Byte 2
LDB R3, R2, #0
RSHFL R3, R3, #2	;Logical shift right 2 bits
AND R3, R3, x3		;Isolate right 2 bits

ADD R3, R3, R0		
LDB R3, R3, #0		
STB R3, R1, #1		;Writing R3 to 0x4005 + 1


; Processing Byte 3
LDB R3, R2, #0

RSHFL R3, R3, #4	;Logical shift right 4 bits
AND R3, R3, x3		;Isolate right 2 bits

ADD R3, R3, R0		
LDB R3, R3, #0		
STB R3, R1, #2		;Writing R3 to 0x4005 + 2


; Processing Byte 4
LDB R3, R2, #0

RSHFL R3, R3, #6	;Logical shift right 6 bits
AND R3, R3, x3		;Isolate right 2 bits

ADD R3, R3, R0		
LDB R3, R3, #0		
STB R3, R1, #3		;Writing R3 to 0x4005 + 3



TRAP x25		; Halts the machine


READ .FILL x4000
MASK .FILL x4004
WRITE .FILL x4005

.END