#!/usr/bin/python
NEON_INST = [
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VABA.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VABA.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VABAL.<dt> <Qd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VABD.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VABD.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VABDL.<dt> <Qd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VABD<c>.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VABD<c>.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VABS.<dt> <Qd>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VABS.<dt> <Dd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VACGE.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VACGE.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VACGT.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VACGT.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32', 'I64')},
        'fmt': 'VADD.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32', 'I64')},
        'fmt': 'VADD.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VADD.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VADD.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('I16', 'I32', 'I64')},
        'fmt': 'VADDHN.<dt> <Dd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VADDL.<dt> <Qd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VADDW.<dt> <Qd>, <Qn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VAND <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VAND <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('I16', 'I32')},
        'fmt': 'VBIC.<dt> <Qd>, #<mimm>'
    },
    {
        'opt': {'dt': ('I16', 'I32')},
        'fmt': 'VBIC.<dt> <Dd>, #<mimm>'
    },
    {
        'opt': {},
        'fmt': 'VBIC <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VBIC <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VBIF <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VBIF <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VBIT <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VBIT <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VBSL <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VBSL <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32')},
        'fmt': 'VCEQ.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32')},
        'fmt': 'VCEQ.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VCEQ.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VCEQ.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32', 'F32')},
        'fmt': 'VCEQ.<dt> <Qd>, <Qm>, #0'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32', 'F32')},
        'fmt': 'VCEQ.<dt> <Dd>, <Dm>, #0'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VCGE.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VCGE.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VCGE.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VCGE.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VCGE.<dt> <Qd>, <Qm>, #0'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VCGE.<dt> <Dd>, <Dm>, #0'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VCGT.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VCGT.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VCGT.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VCGT.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VCGT.<dt> <Qd>, <Qm>, #0'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VCGT.<dt> <Dd>, <Dm>, #0'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VCLE.<dt> <Qd>, <Qm>, #0'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VCLE.<dt> <Dd>, <Dm>, #0'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32')},
        'fmt': 'VCLS.<dt> <Qd>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32')},
        'fmt': 'VCLS.<dt> <Dd>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VCLT.<dt> <Qd>, <Qm>, #0'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VCLT.<dt> <Dd>, <Dm>, #0'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32')},
        'fmt': 'VCLZ.<dt> <Qd>, <Qm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32')},
        'fmt': 'VCLZ.<dt> <Dd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VCNT.8 <Qd>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VCNT.8 <Dd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VCVT.S32.F32 <Qd>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VCVT.S32.F32 <Dd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VCVT.U32.F32 <Qd>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VCVT.U32.F32 <Dd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VCVT.F32.S32 <Qd>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VCVT.F32.S32 <Dd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VCVT.F32.U32 <Qd>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VCVT.F32.U32 <Dd>, <Dm>'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VCVT.<dt>.F32 <Qd>, <Qm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VCVT.<dt>.F32 <Dd>, <Dm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VCVT.F32.<dt> <Qd>, <Qm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VCVT.F32.<dt> <Dd>, <Dm>, #<imm5>+1'
    },
    {
        'opt': {},
        'fmt': 'VCVT.F32.F16 <Qd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VCVT.F16.F32 <Dd>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VDUP.8 <Qd>, <Dm>[<imm3>]'
    },
    {
        'opt': {},
        'fmt': 'VDUP.8 <Dd>, <Dm>[<imm3>]'
    },
    {
        'opt': {},
        'fmt': 'VDUP.16 <Qd>, <Dm>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VDUP.16 <Dd>, <Dm>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VDUP.32 <Qd>, <Dm>[<imm1>]'
    },
    {
        'opt': {},
        'fmt': 'VDUP.32 <Dd>, <Dm>[<imm1>]'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VDUP<c>.<size> <Qd>, <Rt>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VDUP<c>.<size> <Dd>, <Rt>'
    },
    {
        'opt': {},
        'fmt': 'VEOR <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VEOR <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VEXT.8 <Qd>, <Qn>, <Qm>, #<imm4>'
    },
    {
        'opt': {},
        'fmt': 'VEXT.8 <Dd>, <Dn>, <Dm>, #<imm3>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VHADD.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VHADD.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VHSUB.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VHSUB.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VMAX.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VMAX.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VMIN.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VMIN.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VMAX.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VMAX.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VMIN.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VMIN.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32')},
        'fmt': 'VMLA.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32')},
        'fmt': 'VMLA.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32')},
        'fmt': 'VMLS.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32')},
        'fmt': 'VMLS.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VMLAL.<dt> <Qd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VMLSL.<dt> <Qd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VMLA.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VMLA.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VMLS.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VMLS.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VMLA.I16 <Qd>, <Qn>, D<imm3>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VMLA.I16 <Dd>, <Dn>, D<imm3>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VMLS.I16 <Qd>, <Qn>, D<imm3>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VMLS.I16 <Dd>, <Dn>, D<imm3>[<imm2>]'
    },
    {
        'opt': {'dt': ('I32', 'F32')},
        'fmt': 'VMLA.<dt> <Qd>, <Qn>, D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('I32', 'F32')},
        'fmt': 'VMLA.<dt> <Dd>, <Dn>, D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('I32', 'F32')},
        'fmt': 'VMLS.<dt> <Qd>, <Qn>, D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('I32', 'F32')},
        'fmt': 'VMLS.<dt> <Dd>, <Dn>, D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VMLAL.<dt> <Qd>, <Dn>, D<imm3>[<imm2>]'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VMLSL.<dt> <Qd>, <Dn>, D<imm3>[<imm2>]'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VMLAL.<dt> <Qd>, <Dn>, D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VMLSL.<dt> <Qd>, <Dn>, D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32', 'I64', 'F32')},
        'fmt': 'VMOV.<dt> <Qd>, #<mimm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32', 'I64', 'F32')},
        'fmt': 'VMOV.<dt> <Dd>, #<mimm>'
    },
    {
        'opt': {},
        'fmt': 'VMOV <Qd>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VMOV <Dd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VMOV<c>.8  <Dd>[<imm3>], <Rt>'
    },
    {
        'opt': {},
        'fmt': 'VMOV<c>.16 <Dd>[<imm2>], <Rt>'
    },
    {
        'opt': {},
        'fmt': 'VMOV<c>.32 <Dd>[<imm1>], <Rt>'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VMOV<c>.<dt> <Rt>, <Dn>[<imm3>]'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VMOV<c>.<dt> <Rt>, <Dn>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VMOV<c>.32 <Rt>, <Dn>[<imm1>]'
    },
    {
        'opt': {},
        'fmt': 'VMOV<c> <Sm>, <Sm1>, <Rt>, <Rt2>'
    },
    {
        'opt': {},
        'fmt': 'VMOV<c> <Rt>, <Rt2@D>, <Sm>, <Sm1>'
    },
    {
        'opt': {},
        'fmt': 'VMOV<c> <Dm>, <Rt>, <Rt2>'
    },
    {
        'opt': {},
        'fmt': 'VMOV<c> <Rt>, <Rt2@D>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VMOVL.<dt> <Qd>, <Dm>'
    },
    {
        'opt': {'dt': ('I16', 'I32', 'I64')},
        'fmt': 'VMOVN.<dt> <Dd>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VMRS<c> <Rt>, FPSCR'
    },
    {
        'opt': {},
        'fmt': 'VMSR<c> FPSCR, <Rt>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32', 'P8')},
        'fmt': 'VMUL.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32', 'P8')},
        'fmt': 'VMUL.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32', 'P8')},
        'fmt': 'VMULL.<dt> <Qd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VMUL.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VMUL.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VMUL.I16 <Qd>, <Qn>, D<imm3>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VMUL.I16 <Dd>, <Dn>, D<imm3>[<imm2>]'
    },
    {
        'opt': {'dt': ('I32', 'F32')},
        'fmt': 'VMUL.<dt> <Qd>, <Qn>, D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('I32', 'F32')},
        'fmt': 'VMUL.<dt> <Dd>, <Dn>, D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VMULL.<dt> <Qd>, <Dn>, D<imm3>[<imm2>]'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VMULL.<dt> <Qd>, <Dn>, D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('I16', 'I32')},
        'fmt': 'VMVN.<dt> <Qd>, #<mimm>'
    },
    {
        'opt': {'dt': ('I16', 'I32')},
        'fmt': 'VMVN.<dt> <Dd>, #<mimm>'
    },
    {
        'opt': {},
        'fmt': 'VMVN <Qd>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VMVN <Dd>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VNEG.<dt> <Qd>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'F32')},
        'fmt': 'VNEG.<dt> <Dd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VORN <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VORN <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('I16', 'I32')},
        'fmt': 'VORR.<dt> <Qd>, #<mimm>'
    },
    {
        'opt': {'dt': ('I16', 'I32')},
        'fmt': 'VORR.<dt> <Dd>, #<mimm>'
    },
    {
        'opt': {},
        'fmt': 'VORR <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VORR <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VPADAL.<dt> <Qd>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VPADAL.<dt> <Dd>, <Dm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32')},
        'fmt': 'VPADD.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VPADD.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VPADDL.<dt> <Qd>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VPADDL.<dt> <Dd>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VPMAX.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VPMIN.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VPMAX.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VPMIN.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32')},
        'fmt': 'VQABS.<dt> <Qd>,<Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32')},
        'fmt': 'VQABS.<dt> <Dd>,<Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'S64', 'U8', 'U16', 'U32', 'U64')},
        'fmt': 'VQADD.<dt> <Qd>,<Qn>,<Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'S64', 'U8', 'U16', 'U32', 'U64')},
        'fmt': 'VQADD.<dt> <Dd>,<Dn>,<Dm>'
    },
    {
        'opt': {'dt': ('S16', 'S32')},
        'fmt': 'VQDMLAL.<dt> <Qd>,<Dn>,<Dm>'
    },
    {
        'opt': {'dt': ('S16', 'S32')},
        'fmt': 'VQDMLSL.<dt> <Qd>,<Dn>,<Dm>'
    },
    {
        'opt': {},
        'fmt': 'VQDMLAL.S16 <Qd>,<Dn>,D<imm3>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VQDMLSL.S16 <Qd>,<Dn>,D<imm3>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VQDMLAL.S32 <Qd>,<Dn>,D<imm4>[<imm1>]'
    },
    {
        'opt': {},
        'fmt': 'VQDMLSL.S32 <Qd>,<Dn>,D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('S16', 'S32')},
        'fmt': 'VQDMULH.<dt> <Qd>,<Qn>,<Qm>'
    },
    {
        'opt': {'dt': ('S16', 'S32')},
        'fmt': 'VQDMULH.<dt> <Dd>,<Dn>,<Dm>'
    },
    {
        'opt': {},
        'fmt': 'VQDMULH.S16 <Qd>,<Qn>,D<imm3>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VQDMULH.S16 <Dd>,<Dn>,D<imm3>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VQDMULH.S32 <Qd>,<Qn>,D<imm4>[<imm1>]'
    },
    {
        'opt': {},
        'fmt': 'VQDMULH.S32 <Dd>,<Dn>,D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('S16', 'S32')},
        'fmt': 'VQDMULL.<dt> <Qd>,<Dn>,<Dm>'
    },
    {
        'opt': {},
        'fmt': 'VQDMULL.S16 <Qd>,<Dn>,D<imm3>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VQDMULL.S32 <Qd>,<Dn>,D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('S16', 'S32', 'S64', 'U16', 'U32', 'U64')},
        'fmt': 'VQMOVN.<dt> <Dd>, <Qm>'
    },
    {
        'opt': {'dt': ('S16', 'S32', 'S64')},
        'fmt': 'VQMOVUN.<dt> <Dd>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32')},
        'fmt': 'VQNEG.<dt> <Qd>,<Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32')},
        'fmt': 'VQNEG.<dt> <Dd>,<Dm>'
    },
    {
        'opt': {'dt': ('S16', 'S32')},
        'fmt': 'VQRDMULH.<dt> <Qd>,<Qn>,<Qm>'
    },
    {
        'opt': {'dt': ('S16', 'S32')},
        'fmt': 'VQRDMULH.<dt> <Dd>,<Dn>,<Dm>'
    },
    {
        'opt': {},
        'fmt': 'VQRDMULH.S16 <Qd>,<Qn>,D<imm3>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VQRDMULH.S16 <Dd>,<Dn>,D<imm3>[<imm2>]'
    },
    {
        'opt': {},
        'fmt': 'VQRDMULH.S32 <Qd>,<Qn>,D<imm4>[<imm1>]'
    },
    {
        'opt': {},
        'fmt': 'VQRDMULH.S32 <Dd>,<Dn>,D<imm4>[<imm1>]'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'S64', 'U8', 'U16', 'U32', 'U64')},
        'fmt': 'VQRSHL.<dt> <Qd>,<Qm>,<Qn>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'S64', 'U8', 'U16', 'U32', 'U64')},
        'fmt': 'VQRSHL.<dt> <Dd>,<Dm>,<Dn>'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VQRSHRN.<dt> <Dd>,<Qm>,#<imm3>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VQRSHRN.<dt> <Dd>,<Qm>,#<imm4>+1'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VQRSHRN.<dt> <Dd>,<Qm>,#<imm5>+1'
    },
    {
        'opt': {},
        'fmt': 'VQRSHRUN.S16 <Dd>,<Qm>,#<imm3>+1'
    },
    {
        'opt': {},
        'fmt': 'VQRSHRUN.S32 <Dd>,<Qm>,#<imm4>+1'
    },
    {
        'opt': {},
        'fmt': 'VQRSHRUN.S64 <Dd>,<Qm>,#<imm5>+1'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'S64', 'U8', 'U16', 'U32', 'U64')},
        'fmt': 'VQSHL.<dt> <Qd>,<Qm>,<Qn>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'S64', 'U8', 'U16', 'U32', 'U64')},
        'fmt': 'VQSHL.<dt> <Dd>,<Dm>,<Dn>'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VQSHL.<dt> <Qd>,<Qm>,#<imm3>'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VQSHL.<dt> <Dd>,<Dm>,#<imm3>'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VQSHL.<dt> <Qd>,<Qm>,#<imm4>'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VQSHL.<dt> <Dd>,<Dm>,#<imm4>'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VQSHL.<dt> <Qd>,<Qm>,#<imm5>'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VQSHL.<dt> <Dd>,<Dm>,#<imm5>'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VQSHL.<dt> <Qd>,<Qm>,#<imm6>'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VQSHL.<dt> <Dd>,<Dm>,#<imm6>'
    },
    {
        'opt': {},
        'fmt': 'VQSHLU.S8 <Qd>,<Qm>,#<imm3>'
    },
    {
        'opt': {},
        'fmt': 'VQSHLU.S8 <Dd>,<Dm>,#<imm3>'
    },
    {
        'opt': {},
        'fmt': 'VQSHLU.S16 <Qd>,<Qm>,#<imm4>'
    },
    {
        'opt': {},
        'fmt': 'VQSHLU.S16 <Dd>,<Dm>,#<imm4>'
    },
    {
        'opt': {},
        'fmt': 'VQSHLU.S32 <Qd>,<Qm>,#<imm5>'
    },
    {
        'opt': {},
        'fmt': 'VQSHLU.S32 <Dd>,<Dm>,#<imm5>'
    },
    {
        'opt': {},
        'fmt': 'VQSHLU.S64 <Qd>,<Qm>,#<imm6>'
    },
    {
        'opt': {},
        'fmt': 'VQSHLU.S64 <Dd>,<Dm>,#<imm6>'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VQSHRN.<dt> <Dd>,<Qm>,#<imm3>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VQSHRN.<dt> <Dd>,<Qm>,#<imm4>+1'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VQSHRN.<dt> <Dd>,<Qm>,#<imm5>+1'
    },
    {
        'opt': {},
        'fmt': 'VQSHRUN.S16 <Dd>,<Qm>,#<imm3>+1'
    },
    {
        'opt': {},
        'fmt': 'VQSHRUN.S32 <Dd>,<Qm>,#<imm4>+1'
    },
    {
        'opt': {},
        'fmt': 'VQSHRUN.S64 <Dd>,<Qm>,#<imm5>+1'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'S64', 'U8', 'U16', 'U32', 'U64')},
        'fmt': 'VQSUB.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'S64', 'U8', 'U16', 'U32', 'U64')},
        'fmt': 'VQSUB.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('I16', 'I32', 'I64')},
        'fmt': 'VRADDHN.<dt> <Dd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('U32', 'F32')},
        'fmt': 'VRECPE.<dt> <Qd>, <Qm>'
    },
    {
        'opt': {'dt': ('U32', 'F32')},
        'fmt': 'VRECPE.<dt> <Dd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VRECPS.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VRECPS.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VREV16.8 <Qd>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VREV16.8 <Dd>, <Dm>'
    },
    {
        'opt': {'size': (8, 16)},
        'fmt': 'VREV32.<size> <Qd>, <Qm>'
    },
    {
        'opt': {'size': (8, 16)},
        'fmt': 'VREV32.<size> <Dd>, <Dm>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VREV64.<size> <Qd>, <Qm>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VREV64.<size> <Dd>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VRHADD.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VRHADD.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VRSHL.<dt> <Qd>, <Qm>, <Qn>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VRSHL.<dt> <Dd>, <Dm>, <Dn>'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VRSHR.<dt> <Qd>, <Qm>, #<imm3>+1'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VRSHR.<dt> <Dd>, <Dm>, #<imm3>+1'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VRSHR.<dt> <Qd>, <Qm>, #<imm4>+1'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VRSHR.<dt> <Dd>, <Dm>, #<imm4>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VRSHR.<dt> <Qd>, <Qm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VRSHR.<dt> <Dd>, <Dm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VRSHR.<dt> <Qd>, <Qm>, #<imm6>+1'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VRSHR.<dt> <Dd>, <Dm>, #<imm6>+1'
    },
    {
        'opt': {},
        'fmt': 'VRSHRN.I16 <Dd>, <Qm>, #<imm3>+1'
    },
    {
        'opt': {},
        'fmt': 'VRSHRN.I32 <Dd>, <Qm>, #<imm4>+1'
    },
    {
        'opt': {},
        'fmt': 'VRSHRN.I64 <Dd>, <Qm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('U32', 'F32')},
        'fmt': 'VRSQRTE.<dt> <Qd>, <Qm>'
    },
    {
        'opt': {'dt': ('U32', 'F32')},
        'fmt': 'VRSQRTE.<dt> <Dd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VRSQRTS.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VRSQRTS.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VRSRA.<dt> <Qd>, <Qm>, #<imm3>+1'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VRSRA.<dt> <Dd>, <Dm>, #<imm3>+1'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VRSRA.<dt> <Qd>, <Qm>, #<imm4>+1'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VRSRA.<dt> <Dd>, <Dm>, #<imm4>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VRSRA.<dt> <Qd>, <Qm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VRSRA.<dt> <Dd>, <Dm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VRSRA.<dt> <Qd>, <Qm>, #<imm6>+1'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VRSRA.<dt> <Dd>, <Dm>, #<imm6>+1'
    },
    {
        'opt': {'dt': ('I16', 'I32', 'I64')},
        'fmt': 'VRSUBHN.<dt> <Dd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VSHL.I8 <Qd>, <Qm>, #<imm3>'
    },
    {
        'opt': {},
        'fmt': 'VSHL.I8 <Dd>, <Dm>, #<imm3>'
    },
    {
        'opt': {},
        'fmt': 'VSHL.I16 <Qd>, <Qm>, #<imm4>'
    },
    {
        'opt': {},
        'fmt': 'VSHL.I16 <Dd>, <Dm>, #<imm4>'
    },
    {
        'opt': {},
        'fmt': 'VSHL.I32 <Qd>, <Qm>, #<imm5>'
    },
    {
        'opt': {},
        'fmt': 'VSHL.I32 <Dd>, <Dm>, #<imm5>'
    },
    {
        'opt': {},
        'fmt': 'VSHL.I64 <Qd>, <Qm>, #<imm6>'
    },
    {
        'opt': {},
        'fmt': 'VSHL.I64 <Dd>, <Dm>, #<imm6>'
    },
    {
        'opt': {'dt': ('U8', 'U16', 'U32', 'U64', 'S8', 'S16', 'S32', 'S64')},
        'fmt': 'VSHL.<dt> <Qd>, <Qm>, <Qn>'
    },
    {
        'opt': {'dt': ('U8', 'U16', 'U32', 'U64', 'S8', 'S16', 'S32', 'S64')},
        'fmt': 'VSHL.<dt> <Dd>, <Dm>, <Dn>'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VSHLL.<dt> <Qd>, <Dm>, #<imm3>'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VSHLL.<dt> <Qd>, <Dm>, #<imm4>'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VSHLL.<dt> <Qd>, <Dm>, #<imm5>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VSHLL.I<size> <Qd>, <Dm>, #<size>'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VSHR.<dt> <Qd>, <Qm>, #<imm3>+1'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VSHR.<dt> <Dd>, <Dm>, #<imm3>+1'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VSHR.<dt> <Qd>, <Qm>, #<imm4>+1'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VSHR.<dt> <Dd>, <Dm>, #<imm4>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VSHR.<dt> <Qd>, <Qm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VSHR.<dt> <Dd>, <Dm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VSHR.<dt> <Qd>, <Qm>, #<imm6>+1'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VSHR.<dt> <Dd>, <Dm>, #<imm6>+1'
    },
    {
        'opt': {},
        'fmt': 'VSHRN.I16 <Dd>, <Qm>, #<imm3>+1'
    },
    {
        'opt': {},
        'fmt': 'VSHRN.I32 <Dd>, <Qm>, #<imm4>+1'
    },
    {
        'opt': {},
        'fmt': 'VSHRN.I64 <Dd>, <Qm>, #<imm5>+1'
    },
    {
        'opt': {},
        'fmt': 'VSLI.8 <Qd>, <Qm>, #<imm3>'
    },
    {
        'opt': {},
        'fmt': 'VSLI.8 <Dd>, <Dm>, #<imm3>'
    },
    {
        'opt': {},
        'fmt': 'VSLI.16 <Qd>, <Qm>, #<imm4>'
    },
    {
        'opt': {},
        'fmt': 'VSLI.16 <Dd>, <Dm>, #<imm4>'
    },
    {
        'opt': {},
        'fmt': 'VSLI.32 <Qd>, <Qm>, #<imm5>'
    },
    {
        'opt': {},
        'fmt': 'VSLI.32 <Dd>, <Dm>, #<imm5>'
    },
    {
        'opt': {},
        'fmt': 'VSLI.64 <Qd>, <Qm>, #<imm6>'
    },
    {
        'opt': {},
        'fmt': 'VSLI.64 <Dd>, <Dm>, #<imm6>'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VSRA.<dt> <Qd>, <Qm>, #<imm3>+1'
    },
    {
        'opt': {'dt': ('S8', 'U8')},
        'fmt': 'VSRA.<dt> <Dd>, <Dm>, #<imm3>+1'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VSRA.<dt> <Qd>, <Qm>, #<imm4>+1'
    },
    {
        'opt': {'dt': ('S16', 'U16')},
        'fmt': 'VSRA.<dt> <Dd>, <Dm>, #<imm4>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VSRA.<dt> <Qd>, <Qm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('S32', 'U32')},
        'fmt': 'VSRA.<dt> <Dd>, <Dm>, #<imm5>+1'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VSRA.<dt> <Qd>, <Qm>, #<imm6>+1'
    },
    {
        'opt': {'dt': ('S64', 'U64')},
        'fmt': 'VSRA.<dt> <Dd>, <Dm>, #<imm6>+1'
    },
    {
        'opt': {},
        'fmt': 'VSRI.8 <Qd>, <Qm>, #<imm3>+1'
    },
    {
        'opt': {},
        'fmt': 'VSRI.8 <Dd>, <Dm>, #<imm3>+1'
    },
    {
        'opt': {},
        'fmt': 'VSRI.16 <Qd>, <Qm>, #<imm4>+1'
    },
    {
        'opt': {},
        'fmt': 'VSRI.16 <Dd>, <Dm>, #<imm4>+1'
    },
    {
        'opt': {},
        'fmt': 'VSRI.32 <Qd>, <Qm>, #<imm5>+1'
    },
    {
        'opt': {},
        'fmt': 'VSRI.32 <Dd>, <Dm>, #<imm5>+1'
    },
    {
        'opt': {},
        'fmt': 'VSRI.64 <Qd>, <Qm>, #<imm6>+1'
    },
    {
        'opt': {},
        'fmt': 'VSRI.64 <Dd>, <Dm>, #<imm6>+1'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32', 'I64')},
        'fmt': 'VSUB.<dt> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('I8', 'I16', 'I32', 'I64')},
        'fmt': 'VSUB.<dt> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VSUB.F32 <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VSUB.F32 <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('I16', 'I32', 'I64')},
        'fmt': 'VSUBHN.<dt> <Dd>, <Qn>, <Qm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VSUBL.<dt> <Qd>, <Dn>, <Dm>'
    },
    {
        'opt': {'dt': ('S8', 'S16', 'S32', 'U8', 'U16', 'U32')},
        'fmt': 'VSUBW.<dt> <Qd>, <Qn>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VSWP <Qd>, <Qm>'
    },
    {
        'opt': {},
        'fmt': 'VSWP <Dd>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VTBL.8 <Dd>, <Dlist>, <Dm>'
    },
    {
        'opt': {},
        'fmt': 'VTBX.8 <Dd>, <Dlist>, <Dm>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VTRN.<size> <Qd>, <Qm>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VTRN.<size> <Dd>, <Dm>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VTST.<size> <Qd>, <Qn>, <Qm>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VTST.<size> <Dd>, <Dn>, <Dm>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VUZP.<size> <Qd>, <Qm>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VUZP.<size> <Dd>, <Dm>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VZIP.<size> <Qd>, <Qm>'
    },
    {
        'opt': {'size': (8, 16, 32)},
        'fmt': 'VZIP.<size> <Dd>, <Dm>'
    }]
