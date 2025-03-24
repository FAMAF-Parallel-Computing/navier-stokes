	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_v1p0_zicsr2p0_zifencei2p0_zmmul1p0_zve32f1p0_zve32x1p0_zve64d1p0_zve64f1p0_zve64x1p0_zvl128b1p0_zvl32b1p0_zvl64b1p0"
	.file	"addSourceOld.cpp"
	.globl	_Z10add_sourcejPfPKff           # -- Begin function _Z10add_sourcejPfPKff
	.p2align	1
	.type	_Z10add_sourcejPfPKff,@function
_Z10add_sourcejPfPKff:                  # @_Z10add_sourcejPfPKff
	.cfi_startproc
# %bb.0:
	addi	a0, a0, 2
	mulw	a0, a0, a0
	beqz	a0, .LBB0_7
# %bb.1:
	slli	a0, a0, 32
	li	a3, 16
	srli	t0, a0, 32
	csrr	a0, vlenb
	srli	t1, a0, 1
	slli	a6, t0, 2
	bgeu	a3, t1, .LBB0_12
# %bb.11:
	mv	a3, t1
.LBB0_12:
	bltu	t0, a3, .LBB0_4
# %bb.2:
	add	a3, a2, a6
	bgeu	a1, a3, .LBB0_8
# %bb.3:
	add	a3, a1, a6
	bgeu	a2, a3, .LBB0_8
.LBB0_4:
	li	a7, 0
.LBB0_5:
	slli	a7, a7, 2
	add	a0, a2, a7
	add	a1, a1, a7
	add	a2, a2, a6
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	flw	fa5, 0(a0)
	flw	fa4, 0(a1)
	addi	a0, a0, 4
	fmadd.s	fa5, fa0, fa5, fa4
	fsw	fa5, 0(a1)
	addi	a1, a1, 4
	bne	a0, a2, .LBB0_6
.LBB0_7:
	ret
.LBB0_8:
	srli	a3, a0, 3
	slli	a4, a0, 1
	mv	a0, a1
	slli	a3, a3, 2
	neg	a3, a3
	and	a7, a3, t0
	vsetvli	a3, zero, e32, m2, ta, ma
	vfmv.v.f	v8, fa0
	mv	a3, a2
	mv	a5, a7
.LBB0_9:                                # =>This Inner Loop Header: Depth=1
	vl2re32.v	v10, (a3)
	add	a3, a3, a4
	sub	a5, a5, t1
	vl2re32.v	v12, (a0)
	vfmacc.vv	v12, v8, v10
	vs2r.v	v12, (a0)
	add	a0, a0, a4
	bnez	a5, .LBB0_9
# %bb.10:
	bne	a7, t0, .LBB0_5
	j	.LBB0_7
.Lfunc_end0:
	.size	_Z10add_sourcejPfPKff, .Lfunc_end0-_Z10add_sourcejPfPKff
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 19.1.7 (https://github.com/llvm/llvm-project.git cd708029e0b2869e80abe31ddb175f7c35361f90)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
