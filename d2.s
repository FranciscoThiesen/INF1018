.text
.globl tmp

tmp:
	pushq %rbp
	movq  %rsp, %rbp

	movq  $0x1122334455667788, %r9
	movq  (%r9), %rdi


	movq  $0x1122334455667788, %rax
	call *%rax


	leave
	ret
	
