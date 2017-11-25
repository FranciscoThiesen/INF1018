.text
.globl tmp

tmp:
	pushq %rbp
	movq  %rsp, %rbp

	
	movq  %rdi, %rdx
	movq  $0x1122334455667788, %rax
	call *%rax


	leave
	ret
	
