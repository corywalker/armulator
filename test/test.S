    mov     R11, #0
    mov     R12, #0
    mov     R13, #255, 4
for_start:
    add     R11, R11, #1
    cmp     R11, R13
    addne   R12, R12, #1
    cmp     R11, R12
    beq     for_start
