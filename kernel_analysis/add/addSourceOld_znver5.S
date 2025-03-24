	.text
	.file	"addSourceOld.cpp"
	.globl	_Z10add_sourcejPfPKff           # -- Begin function _Z10add_sourcejPfPKff
	.p2align	4, 0x90
	.type	_Z10add_sourcejPfPKff,@function
_Z10add_sourcejPfPKff:                  # @_Z10add_sourcejPfPKff
	.cfi_startproc
# %bb.0:
	addl	$2, %edi
	imull	%edi, %edi
	testl	%edi, %edi
	je	.LBB0_19
# %bb.1:
	movl	%edi, %eax
	cmpl	$8, %edi
	jb	.LBB0_2
# %bb.3:
	leaq	(%rdx,%rax,4), %r8
	leaq	(%rsi,%rax,4), %rcx
	cmpq	%rsi, %r8
	seta	%r8b
	cmpq	%rdx, %rcx
	seta	%cl
	testb	%cl, %r8b
	je	.LBB0_5
.LBB0_2:
	xorl	%ecx, %ecx
.LBB0_14:
	movq	%rax, %r8
	andq	$5, %r8
	movq	%rcx, %rdi
	je	.LBB0_17
# %bb.15:
	movq	%rcx, %rdi
	.p2align	4, 0x90
.LBB0_16:                               # =>This Inner Loop Header: Depth=1
	vmovss	(%rdx,%rdi,4), %xmm1            # xmm1 = mem[0],zero,zero,zero
	vfmadd213ss	(%rsi,%rdi,4), %xmm0, %xmm1 # xmm1 = (xmm0 * xmm1) + mem
	vmovss	%xmm1, (%rsi,%rdi,4)
	incq	%rdi
	decq	%r8
	jne	.LBB0_16
.LBB0_17:
	subq	%rax, %rcx
	cmpq	$-8, %rcx
	ja	.LBB0_19
	.p2align	4, 0x90
.LBB0_18:                               # =>This Inner Loop Header: Depth=1
	vmovss	(%rdx,%rdi,4), %xmm1            # xmm1 = mem[0],zero,zero,zero
	vfmadd213ss	(%rsi,%rdi,4), %xmm0, %xmm1 # xmm1 = (xmm0 * xmm1) + mem
	vmovss	%xmm1, (%rsi,%rdi,4)
	vmovss	4(%rdx,%rdi,4), %xmm1           # xmm1 = mem[0],zero,zero,zero
	vfmadd213ss	4(%rsi,%rdi,4), %xmm0, %xmm1 # xmm1 = (xmm0 * xmm1) + mem
	vmovss	%xmm1, 4(%rsi,%rdi,4)
	vmovss	8(%rdx,%rdi,4), %xmm1           # xmm1 = mem[0],zero,zero,zero
	vfmadd213ss	8(%rsi,%rdi,4), %xmm0, %xmm1 # xmm1 = (xmm0 * xmm1) + mem
	vmovss	%xmm1, 8(%rsi,%rdi,4)
	vmovss	12(%rdx,%rdi,4), %xmm1          # xmm1 = mem[0],zero,zero,zero
	vfmadd213ss	12(%rsi,%rdi,4), %xmm0, %xmm1 # xmm1 = (xmm0 * xmm1) + mem
	vmovss	%xmm1, 12(%rsi,%rdi,4)
	vmovss	16(%rdx,%rdi,4), %xmm1          # xmm1 = mem[0],zero,zero,zero
	vfmadd213ss	16(%rsi,%rdi,4), %xmm0, %xmm1 # xmm1 = (xmm0 * xmm1) + mem
	vmovss	%xmm1, 16(%rsi,%rdi,4)
	vmovss	20(%rdx,%rdi,4), %xmm1          # xmm1 = mem[0],zero,zero,zero
	vfmadd213ss	20(%rsi,%rdi,4), %xmm0, %xmm1 # xmm1 = (xmm0 * xmm1) + mem
	vmovss	%xmm1, 20(%rsi,%rdi,4)
	vmovss	24(%rdx,%rdi,4), %xmm1          # xmm1 = mem[0],zero,zero,zero
	vfmadd213ss	24(%rsi,%rdi,4), %xmm0, %xmm1 # xmm1 = (xmm0 * xmm1) + mem
	vmovss	%xmm1, 24(%rsi,%rdi,4)
	vmovss	28(%rdx,%rdi,4), %xmm1          # xmm1 = mem[0],zero,zero,zero
	vfmadd213ss	28(%rsi,%rdi,4), %xmm0, %xmm1 # xmm1 = (xmm0 * xmm1) + mem
	vmovss	%xmm1, 28(%rsi,%rdi,4)
	addq	$8, %rdi
	cmpq	%rdi, %rax
	jne	.LBB0_18
.LBB0_19:
	vzeroupper
	retq
.LBB0_5:
	cmpl	$64, %edi
	jae	.LBB0_7
# %bb.6:
	xorl	%ecx, %ecx
	jmp	.LBB0_11
.LBB0_7:
	vbroadcastss	%xmm0, %zmm1
	movl	%eax, %ecx
	andl	$-64, %ecx
	xorl	%edi, %edi
	.p2align	4, 0x90
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	vmovups	(%rdx,%rdi,4), %zmm2
	vmovups	64(%rdx,%rdi,4), %zmm3
	vmovups	128(%rdx,%rdi,4), %zmm4
	vmovups	192(%rdx,%rdi,4), %zmm5
	vfmadd213ps	(%rsi,%rdi,4), %zmm1, %zmm2 # zmm2 = (zmm1 * zmm2) + mem
	vfmadd213ps	64(%rsi,%rdi,4), %zmm1, %zmm3 # zmm3 = (zmm1 * zmm3) + mem
	vfmadd213ps	128(%rsi,%rdi,4), %zmm1, %zmm4 # zmm4 = (zmm1 * zmm4) + mem
	vfmadd213ps	192(%rsi,%rdi,4), %zmm1, %zmm5 # zmm5 = (zmm1 * zmm5) + mem
	vmovups	%zmm2, (%rsi,%rdi,4)
	vmovups	%zmm3, 64(%rsi,%rdi,4)
	vmovups	%zmm4, 128(%rsi,%rdi,4)
	vmovups	%zmm5, 192(%rsi,%rdi,4)
	addq	$64, %rdi
	cmpq	%rdi, %rcx
	jne	.LBB0_8
# %bb.9:
	cmpl	%eax, %ecx
	je	.LBB0_19
# %bb.10:
	testb	$56, %al
	je	.LBB0_14
.LBB0_11:
	vbroadcastss	%xmm0, %ymm1
	movq	%rcx, %rdi
	movl	%eax, %ecx
	andl	$-8, %ecx
	.p2align	4, 0x90
.LBB0_12:                               # =>This Inner Loop Header: Depth=1
	vmovups	(%rdx,%rdi,4), %ymm2
	vfmadd213ps	(%rsi,%rdi,4), %ymm1, %ymm2 # ymm2 = (ymm1 * ymm2) + mem
	vmovups	%ymm2, (%rsi,%rdi,4)
	addq	$8, %rdi
	cmpq	%rdi, %rcx
	jne	.LBB0_12
# %bb.13:
	cmpl	%eax, %ecx
	je	.LBB0_19
	jmp	.LBB0_14
.Lfunc_end0:
	.size	_Z10add_sourcejPfPKff, .Lfunc_end0-_Z10add_sourcejPfPKff
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 19.1.7 (Fedora 19.1.7-3.fc41)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
