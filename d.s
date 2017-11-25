.text
.globl d

d:
	pushq %rbp
	movq  %rsp, %rbp
	movq  $0x1122334455667788, %r9
	movq  $0x1122334455667788, %r10
	movl  (%r9), %edi
	movl  (%r10), %esi
	movq  $0x1122112211221122, %rax
	call  *%rax
	
	leave
	ret


