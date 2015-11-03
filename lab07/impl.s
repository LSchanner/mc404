	@ Global symbol
        .global set_speed_motor
        .global set_speed_motors
        .global read_sonar
        .global read_sonars

	.align 4
set_speed_motor:
    stmfd sp!, {r7, lr}	@ Save the callee-save registers
		                        @ and the return address.
    cmp r1, #0
    beq set_motor0
set_motor1:
    mov r7, #127
    svc 0x0
    b end

set_motor0:
    mov r7, #126
    svc 0x0
end:
    ldmfd sp!, {r7, pc} @ Restore the registers and return

set_speed_motors:
    stmfd sp!, {r7, lr}  @ Save the callee-save registers
		              @ and the return address.
    mov r7, #124
    svc 0x0
    ldmfd sp!, {r7, pc}  @ Restore the registers and return

read_sonar:
    stmfd sp!, {r7, lr}  @ Save the callee-save registers
    mov r7, #125
    svc 0x0
    ldmfd sp!, {r7, pc}  @ Restore the registers and return

read_sonars:
    stmfd sp!, {r0-r11, lr}  @ Save the callee-save registers
    mov r2, r0
    mov r1, #0
loop_sonars:
    mov r0, r1
    mov r7, #125
    svc 0x0
    str r0, [r2], #1
    add r1, r1, #1
    cmp r1, #16
    bls loop_sonars

    ldmfd sp!, {r0-r11, pc}  @ Restore the registers and return
