	.text
	.file	"addSource.cpp"
	.globl	_Z9addSourcemPfPKff             # -- Begin function _Z9addSourcemPfPKff
	.p2align	4, 0x90
	.type	_Z9addSourcemPfPKff,@function
_Z9addSourcemPfPKff:                    # @_Z9addSourcemPfPKff
	.cfi_startproc
# %bb.0:
	addq	$2, %rdi
	imulq	%rdi, %rdi
	testq	%rdi, %rdi
	je	.LBB0_13
# %bb.1:
	cmpq	$8, %rdi
	jae	.LBB0_4
# %bb.2:
	xorl	%eax, %eax
	jmp	.LBB0_3
.LBB0_4:
	cmpq	$64, %rdi
	jae	.LBB0_6
# %bb.5:
	xorl	%eax, %eax
	jmp	.LBB0_10
.LBB0_6:
	vbroadcastss	%xmm0, %zmm1
	movq	%rdi, %rax
	andq	$-64, %rax
	xorl	%ecx, %ecx
	.p2align	4, 0x90
.LBB0_7:                                # =>This Inner Loop Header: Depth=1
	vmovups	(%rdx,%rcx,4), %zmm2
	vmovups	64(%rdx,%rcx,4), %zmm3
	vmovups	128(%rdx,%rcx,4), %zmm4
	vmovups	192(%rdx,%rcx,4), %zmm5
	vfmadd213ps	(%rsi,%rcx,4), %zmm1, %zmm2 # zmm2 = (zmm1 * zmm2) + mem
	vfmadd213ps	64(%rsi,%rcx,4), %zmm1, %zmm3 # zmm3 = (zmm1 * zmm3) + mem
	vfmadd213ps	128(%rsi,%rcx,4), %zmm1, %zmm4 # zmm4 = (zmm1 * zmm4) + mem
	vfmadd213ps	192(%rsi,%rcx,4), %zmm1, %zmm5 # zmm5 = (zmm1 * zmm5) + mem
	vmovups	%zmm2, (%rsi,%rcx,4)
	vmovups	%zmm3, 64(%rsi,%rcx,4)
	vmovups	%zmm4, 128(%rsi,%rcx,4)
	vmovups	%zmm5, 192(%rsi,%rcx,4)
	addq	$64, %rcx
	cmpq	%rcx, %rax
	jne	.LBB0_7
# %bb.8:
	cmpq	%rax, %rdi
	je	.LBB0_13
# %bb.9:
	testb	$56, %dil
	je	.LBB0_3
.LBB0_10:
	vbroadcastss	%xmm0, %ymm1
	movq	%rax, %rcx
	movq	%rdi, %rax
	andq	$-8, %rax
	.p2align	4, 0x90
.LBB0_11:                               # =>This Inner Loop Header: Depth=1
	vmovups	(%rdx,%rcx,4), %ymm2
	vfmadd213ps	(%rsi,%rcx,4), %ymm1, %ymm2 # ymm2 = (ymm1 * ymm2) + mem
	vmovups	%ymm2, (%rsi,%rcx,4)
	addq	$8, %rcx
	cmpq	%rcx, %rax
	jne	.LBB0_11
	jmp	.LBB0_12
.LBB0_3:
	vmovss	(%rdx,%rax,4), %xmm1            # xmm1 = mem[0],zero,zero,zero
	vfmadd213ss	(%rsi,%rax,4), %xmm0, %xmm1 # xmm1 = (xmm0 * xmm1) + mem
	vmovss	%xmm1, (%rsi,%rax,4)
	incq	%rax
.LBB0_12:
	cmpq	%rax, %rdi
	jne	.LBB0_3
.LBB0_13:
	vzeroupper
	retq
.Lfunc_end0:
	.size	_Z9addSourcemPfPKff, .Lfunc_end0-_Z9addSourcemPfPKff
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 19.1.7 (Fedora 19.1.7-3.fc41)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
