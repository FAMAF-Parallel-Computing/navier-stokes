	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_v1p0_zicsr2p0_zifencei2p0_zmmul1p0_zve32f1p0_zve32x1p0_zve64d1p0_zve64f1p0_zve64x1p0_zvl128b1p0_zvl32b1p0_zvl64b1p0"
	.file	"addSource.cpp"
	.globl	_Z9addSourcemPfPKff             # -- Begin function _Z9addSourcemPfPKff
	.p2align	1
	.type	_Z9addSourcemPfPKff,@function
_Z9addSourcemPfPKff:                    # @_Z9addSourcemPfPKff
	.cfi_startproc
# %bb.0:
	addi	a0, a0, 2
	mul	a7, a0, a0
	beqz	a7, .LBB0_8
# %bb.1:
	csrr	a4, vlenb
	srli	t0, a4, 1
	bgeu	a7, t0, .LBB0_3
# %bb.2:
	li	a6, 0
	j	.LBB0_6
.LBB0_3:
	neg	a0, t0
	slli	a4, a4, 1
	mv	a5, a1
	and	a6, a7, a0
	vsetvli	a0, zero, e32, m2, ta, ma
	vfmv.v.f	v8, fa0
	mv	a0, a2
	mv	a3, a6
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	vl2re32.v	v10, (a0)
	add	a0, a0, a4
	sub	a3, a3, t0
	vl2re32.v	v12, (a5)
	vfmacc.vv	v12, v8, v10
	vs2r.v	v12, (a5)
	add	a5, a5, a4
	bnez	a3, .LBB0_4
# %bb.5:
	beq	a7, a6, .LBB0_8
.LBB0_6:
	sub	a0, a7, a6
	slli	a6, a6, 2
	add	a1, a1, a6
	add	a2, a2, a6
.LBB0_7:                                # =>This Inner Loop Header: Depth=1
	flw	fa5, 0(a2)
	flw	fa4, 0(a1)
	addi	a0, a0, -1
	addi	a2, a2, 4
	fmadd.s	fa5, fa0, fa5, fa4
	fsw	fa5, 0(a1)
	addi	a1, a1, 4
	bnez	a0, .LBB0_7
.LBB0_8:
	ret
.Lfunc_end0:
	.size	_Z9addSourcemPfPKff, .Lfunc_end0-_Z9addSourcemPfPKff
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 19.1.7 (https://github.com/llvm/llvm-project.git cd708029e0b2869e80abe31ddb175f7c35361f90)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
