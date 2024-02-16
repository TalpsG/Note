	.file	"Screen.cc"
	.text
	.align 2
	.globl	_ZNK6Screen3getEjj
	.type	_ZNK6Screen3getEjj, @function
_ZNK6Screen3getEjj:
.LFB1679:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movl	%edx, -32(%rbp)
	movq	-24(%rbp), %rax
	movl	8(%rax), %eax
	movl	-28(%rbp), %edx
	imull	%edx, %eax
	movl	%eax, -4(%rbp)
	movq	-24(%rbp), %rax
	addq	$16, %rax
	movl	-4(%rbp), %ecx
	movl	-32(%rbp), %edx
	addl	%ecx, %edx
	movl	%edx, %edx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEixEm@PLT
	movzbl	(%rax), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1679:
	.size	_ZNK6Screen3getEjj, .-_ZNK6Screen3getEjj
	.align 2
	.globl	_ZN6Screen3setEjjc
	.type	_ZN6Screen3setEjjc, @function
_ZN6Screen3setEjjc:
.LFB1680:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -40(%rbp)
	movl	%esi, -44(%rbp)
	movl	%edx, -48(%rbp)
	movl	%ecx, %eax
	movb	%al, -52(%rbp)
	movq	-40(%rbp), %rax
	movl	8(%rax), %eax
	movl	-44(%rbp), %edx
	imull	%edx, %eax
	movl	%eax, -20(%rbp)
	movzbl	-52(%rbp), %ebx
	movq	-40(%rbp), %rax
	addq	$16, %rax
	movl	-20(%rbp), %ecx
	movl	-48(%rbp), %edx
	addl	%ecx, %edx
	movl	%edx, %edx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEixEm@PLT
	movb	%bl, (%rax)
	movq	-40(%rbp), %rax
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1680:
	.size	_ZN6Screen3setEjjc, .-_ZN6Screen3setEjjc
	.align 2
	.globl	_ZN6Screen3setEc
	.type	_ZN6Screen3setEc, @function
_ZN6Screen3setEc:
.LFB1681:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$24, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -24(%rbp)
	movl	%esi, %eax
	movb	%al, -28(%rbp)
	movzbl	-28(%rbp), %ebx
	movq	-24(%rbp), %rax
	addq	$16, %rax
	movq	-24(%rbp), %rdx
	movl	(%rdx), %edx
	movl	%edx, %edx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEixEm@PLT
	movb	%bl, (%rax)
	movq	-24(%rbp), %rax
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1681:
	.size	_ZN6Screen3setEc, .-_ZN6Screen3setEc
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
