	.file	"math.c"
	.section	.rodata
.LC0:
	.string	"Ola!"
	.text
	.globl	math
	.type	math, @function
math:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$.LC0, %edi
	call	puts
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	math, .-math
	.ident	"GCC: (GNU) 4.9.2 20141101 (Red Hat 4.9.2-1)"
	.section	.note.GNU-stack,"",@progbits
