.org 0x0
.section .iv,"a"

_start:		

interrupt_vector:

    b RESET_HANDLER
.org 0x18
    b IRQ_HANDLER



.org 0x100
.text

    @ Zera o contador
    ldr r2, =CONTADOR  @lembre-se de declarar esse contador em uma secao de dados! 
    mov r0,#0
    str r0,[r2]

RESET_HANDLER:

    @Set interrupt table base address on coprocessor 15.
    ldr r0, =interrupt_vector
    mcr p15, 0, r0, c12, c0, 0


.data
CONTADOR:
	.word 0x0
