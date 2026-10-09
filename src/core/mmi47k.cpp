// SPDX-FileCopyrightText: 2002-2026 PCSX2 Dev Team
// SPDX-License-Identifier: GPL-3.0+

#include "../../third_party/pcsx2-mmi47k/mmi_compat.h"

namespace rr_mmi47k {
namespace Interpreter {
namespace OpcodeImpl {

	/////////////////////////////////////////////////////////////////
	// Non-MMI Instructions!
	//
	// Several instructions in the MMI opcode class are actually just regular
	// instructions which have been added to "extend" the R5900's instruction
	// set.  They're here, because if not here they'd be homeless.
	//  - The Pcsx2 team, doing their part to fight homelessness

	void MADD(rr::MmiState& cpuRegs) {
		s64 temp = (s64)((u64)cpuRegs.LO.UL[0] | ((u64)cpuRegs.HI.UL[0] << 32)) +
				  ((s64)cpuRegs.GPR.r[_Rs_].SL[0] * (s64)cpuRegs.GPR.r[_Rt_].SL[0]);

		cpuRegs.LO.SD[0] = (s32)(temp & 0xffffffff);
		cpuRegs.HI.SD[0] = (s32)(temp >> 32);

		if (_Rd_) cpuRegs.GPR.r[_Rd_].SD[0] = cpuRegs.LO.SD[0];
	}

	void MADDU(rr::MmiState& cpuRegs) {
		u64 tempu =	(u64)((u64)cpuRegs.LO.UL[0] | ((u64)cpuRegs.HI.UL[0] << 32)) +
				   ((u64)cpuRegs.GPR.r[_Rs_].UL[0] * (u64)cpuRegs.GPR.r[_Rt_].UL[0]);

		cpuRegs.LO.SD[0] = (s32)(tempu & 0xffffffff);
		cpuRegs.HI.SD[0] = (s32)(tempu >> 32);

		if (_Rd_) cpuRegs.GPR.r[_Rd_].SD[0] = cpuRegs.LO.SD[0];
	}

	void MADD1(rr::MmiState& cpuRegs) {
		s64 temp = (s64)((u64)cpuRegs.LO.UL[2] | ((u64)cpuRegs.HI.UL[2] << 32)) +
				  ((s64)cpuRegs.GPR.r[_Rs_].SL[0] * (s64)cpuRegs.GPR.r[_Rt_].SL[0]);

		cpuRegs.LO.SD[1] = (s32)(temp & 0xffffffff);
		cpuRegs.HI.SD[1] = (s32)(temp >> 32);

		if (_Rd_) cpuRegs.GPR.r[_Rd_].SD[0] = cpuRegs.LO.SD[1];
	}

	void MADDU1(rr::MmiState& cpuRegs) {
		u64 tempu = (u64)((u64)cpuRegs.LO.UL[2] | ((u64)cpuRegs.HI.UL[2] << 32)) +
				   ((u64)cpuRegs.GPR.r[_Rs_].UL[0] * (u64)cpuRegs.GPR.r[_Rt_].UL[0]);

		cpuRegs.LO.SD[1] = (s32)(tempu & 0xffffffff);
		cpuRegs.HI.SD[1] = (s32)(tempu >> 32);

		if (_Rd_) cpuRegs.GPR.r[_Rd_].SD[0] = cpuRegs.LO.SD[1];
	}

	void MFHI1(rr::MmiState& cpuRegs) {
		if (!_Rd_) return;
		cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.HI.UD[1];
	}

	void MFLO1(rr::MmiState& cpuRegs) {
		if (!_Rd_) return;
		cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[1];
	}

	void MTHI1(rr::MmiState& cpuRegs) {
		cpuRegs.HI.UD[1] = cpuRegs.GPR.r[_Rs_].UD[0];
	}

	void MTLO1(rr::MmiState& cpuRegs) {
		cpuRegs.LO.UD[1] = cpuRegs.GPR.r[_Rs_].UD[0];
	}

	void MULT1(rr::MmiState& cpuRegs) {
		s64 temp = (s64)cpuRegs.GPR.r[_Rs_].SL[0] * cpuRegs.GPR.r[_Rt_].SL[0];

		// Sign-extend into 64 bits:
		cpuRegs.LO.SD[1] = (s32)(temp & 0xffffffff);
		cpuRegs.HI.SD[1] = (s32)(temp >> 32);

		if (_Rd_) cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[1];
	}

	void MULTU1(rr::MmiState& cpuRegs) {
		u64 tempu = (u64)cpuRegs.GPR.r[_Rs_].UL[0] * cpuRegs.GPR.r[_Rt_].UL[0];

		// According to docs, sign-extend into 64 bits even though it's an unsigned mult.
		cpuRegs.LO.SD[1] = (s32)(tempu & 0xffffffff);
		cpuRegs.HI.SD[1] = (s32)(tempu >> 32);

		if (_Rd_) cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[1];
	}

	void DIV1(rr::MmiState& cpuRegs) {
		if (cpuRegs.GPR.r[_Rs_].UL[0] == 0x80000000 && cpuRegs.GPR.r[_Rt_].UL[0] == 0xffffffff)
		{
				cpuRegs.LO.SD[1] = (s32)0x80000000;
				cpuRegs.HI.SD[1] = (s32)0x0;
		}
		else if (cpuRegs.GPR.r[_Rt_].SL[0] != 0)
		{
			cpuRegs.LO.SD[1] = cpuRegs.GPR.r[_Rs_].SL[0] / cpuRegs.GPR.r[_Rt_].SL[0];
			cpuRegs.HI.SD[1] = cpuRegs.GPR.r[_Rs_].SL[0] % cpuRegs.GPR.r[_Rt_].SL[0];
		}
		else
		{
			cpuRegs.LO.SD[1] = (cpuRegs.GPR.r[_Rs_].SL[0] < 0) ? 1 : -1;
			cpuRegs.HI.SD[1] = cpuRegs.GPR.r[_Rs_].SL[0];
		}
	}

	void DIVU1(rr::MmiState& cpuRegs)
	{
		if (cpuRegs.GPR.r[_Rt_].UL[0] != 0)
		{
			// note: DIVU has no sign extension when assigning back to 64 bits
			// note 2: reference material strongly disagrees. (air)
			cpuRegs.LO.SD[1] = (s32)(cpuRegs.GPR.r[_Rs_].UL[0] / cpuRegs.GPR.r[_Rt_].UL[0]);
			cpuRegs.HI.SD[1] = (s32)(cpuRegs.GPR.r[_Rs_].UL[0] % cpuRegs.GPR.r[_Rt_].UL[0]);
		}
		else
		{
			cpuRegs.LO.SD[1] = -1;
			cpuRegs.HI.SD[1] = cpuRegs.GPR.r[_Rs_].SL[0];
		}
	}

namespace MMI {

//*****************MMI OPCODES*********************************

void PLZCW(rr::MmiState& cpuRegs) {
	if (!_Rd_)
		return;

	// Return the leading sign bits, excluding the original bit
	cpuRegs.GPR.r[_Rd_].UL[0] = Common::CountLeadingSignBits(cpuRegs.GPR.r[_Rs_].SL[0]) - 1;
	cpuRegs.GPR.r[_Rd_].UL[1] = Common::CountLeadingSignBits(cpuRegs.GPR.r[_Rs_].SL[1]) - 1;
}

__fi void PMFHL_CLAMP(u16& dst, s32 src)
{
    if      (src >  0x7fff)	dst = 0x7fff;
    else if (src < -0x8000)	dst = 0x8000;
    else					dst = (u16)src;
}

void PMFHL(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	switch (_Sa_) {
		case 0x00: // LW
			cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
			cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
			cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
			cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
			break;

		case 0x01: // UW
			cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[1];
			cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[1];
			cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[3];
			cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[3];
			break;

		case 0x02: // SLW
			{
				s64 TempS64 = ((u64)cpuRegs.HI.UL[0] << 32) | (u64)cpuRegs.LO.UL[0];

				if (TempS64 >= 0x000000007fffffffLL) {
					cpuRegs.GPR.r[_Rd_].UD[0] = 0x000000007fffffffLL;
				} else if (TempS64 <= -0x80000000LL) {
					cpuRegs.GPR.r[_Rd_].UD[0] = 0xffffffff80000000LL;
				} else {
					cpuRegs.GPR.r[_Rd_].UD[0] = (s64)cpuRegs.LO.SL[0];
				}

				TempS64 = ((u64)cpuRegs.HI.UL[2] << 32) | (u64)cpuRegs.LO.UL[2];

				if (TempS64 >= 0x000000007fffffffLL) {
					cpuRegs.GPR.r[_Rd_].UD[1] = 0x000000007fffffffLL;
				} else if (TempS64 <= -0x80000000LL) {
					cpuRegs.GPR.r[_Rd_].UD[1] = 0xffffffff80000000LL;
				} else {
					cpuRegs.GPR.r[_Rd_].UD[1] = (s64)cpuRegs.LO.SL[2];
				}
			}
			break;

		case 0x03: // LH
			cpuRegs.GPR.r[_Rd_].US[0] = cpuRegs.LO.US[0];
			cpuRegs.GPR.r[_Rd_].US[1] = cpuRegs.LO.US[2];
			cpuRegs.GPR.r[_Rd_].US[2] = cpuRegs.HI.US[0];
			cpuRegs.GPR.r[_Rd_].US[3] = cpuRegs.HI.US[2];
			cpuRegs.GPR.r[_Rd_].US[4] = cpuRegs.LO.US[4];
			cpuRegs.GPR.r[_Rd_].US[5] = cpuRegs.LO.US[6];
			cpuRegs.GPR.r[_Rd_].US[6] = cpuRegs.HI.US[4];
			cpuRegs.GPR.r[_Rd_].US[7] = cpuRegs.HI.US[6];
			break;

		case 0x04: // SH
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[0], cpuRegs.LO.UL[0]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[1], cpuRegs.LO.UL[1]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[2], cpuRegs.HI.UL[0]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[3], cpuRegs.HI.UL[1]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[4], cpuRegs.LO.UL[2]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[5], cpuRegs.LO.UL[3]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[6], cpuRegs.HI.UL[2]);
			PMFHL_CLAMP(cpuRegs.GPR.r[_Rd_].US[7], cpuRegs.HI.UL[3]);
			break;
	}
}

void PMTHL(rr::MmiState& cpuRegs) {
	if (_Sa_ != 0) return;

	cpuRegs.LO.UL[0] = cpuRegs.GPR.r[_Rs_].UL[0];
	cpuRegs.HI.UL[0] = cpuRegs.GPR.r[_Rs_].UL[1];
	cpuRegs.LO.UL[2] = cpuRegs.GPR.r[_Rs_].UL[2];
	cpuRegs.HI.UL[2] = cpuRegs.GPR.r[_Rs_].UL[3];
}

static __fi void _PSLLH(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rt_].US[n] << ( _Sa_ & 0xf );
}

void PSLLH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSLLH(cpuRegs, 0); _PSLLH(cpuRegs, 1); _PSLLH(cpuRegs, 2); _PSLLH(cpuRegs, 3);
	_PSLLH(cpuRegs, 4); _PSLLH(cpuRegs, 5); _PSLLH(cpuRegs, 6); _PSLLH(cpuRegs, 7);
}

static __fi void _PSRLH(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rt_].US[n] >> ( _Sa_ & 0xf );
}

void PSRLH (rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSRLH(cpuRegs, 0); _PSRLH(cpuRegs, 1); _PSRLH(cpuRegs, 2); _PSRLH(cpuRegs, 3);
	_PSRLH(cpuRegs, 4); _PSRLH(cpuRegs, 5); _PSRLH(cpuRegs, 6); _PSRLH(cpuRegs, 7);
}

static __fi void _PSRAH(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rt_].SS[n] >> ( _Sa_ & 0xf );
}

void PSRAH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSRAH(cpuRegs, 0); _PSRAH(cpuRegs, 1); _PSRAH(cpuRegs, 2); _PSRAH(cpuRegs, 3);
	_PSRAH(cpuRegs, 4); _PSRAH(cpuRegs, 5); _PSRAH(cpuRegs, 6); _PSRAH(cpuRegs, 7);
}

static __fi void _PSLLW(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rt_].UL[n] << _Sa_;
}

void PSLLW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSLLW(cpuRegs, 0); _PSLLW(cpuRegs, 1); _PSLLW(cpuRegs, 2); _PSLLW(cpuRegs, 3);
}

static __fi void _PSRLW(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rt_].UL[n] >> _Sa_;
}

void PSRLW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSRLW(cpuRegs, 0); _PSRLW(cpuRegs, 1); _PSRLW(cpuRegs, 2); _PSRLW(cpuRegs, 3);
}

static __fi void _PSRAW(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rt_].SL[n] >> _Sa_;
}

void PSRAW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSRAW(cpuRegs, 0); _PSRAW(cpuRegs, 1); _PSRAW(cpuRegs, 2); _PSRAW(cpuRegs, 3);
}

//*****************END OF MMI OPCODES**************************
//*************************MMI0 OPCODES************************

static __fi void _PADDW(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rs_].UL[n] + cpuRegs.GPR.r[_Rt_].UL[n];
}

void PADDW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PADDW(cpuRegs, 0); _PADDW(cpuRegs, 1); _PADDW(cpuRegs, 2); _PADDW(cpuRegs, 3);
}

static __fi void _PSUBW(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rs_].UL[n] - cpuRegs.GPR.r[_Rt_].UL[n];
}

void PSUBW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSUBW(cpuRegs, 0); _PSUBW(cpuRegs, 1); _PSUBW(cpuRegs, 2); _PSUBW(cpuRegs, 3);
}

static __fi void _PCGTW(rr::MmiState& cpuRegs, int n)
{
	if (cpuRegs.GPR.r[_Rs_].SL[n] > cpuRegs.GPR.r[_Rt_].SL[n])
		cpuRegs.GPR.r[_Rd_].UL[n] = 0xFFFFFFFF;
	else
		cpuRegs.GPR.r[_Rd_].UL[n] = 0x00000000;
}

void PCGTW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PCGTW(cpuRegs, 0);  _PCGTW(cpuRegs, 1);  _PCGTW(cpuRegs, 2);  _PCGTW(cpuRegs, 3);
}

static __fi void _PMAXW(rr::MmiState& cpuRegs, int n)
{
	if (cpuRegs.GPR.r[_Rs_].SL[n] > cpuRegs.GPR.r[_Rt_].SL[n])
		cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rs_].UL[n];
	else
		cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rt_].UL[n];
}

void PMAXW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PMAXW(cpuRegs, 0);  _PMAXW(cpuRegs, 1);  _PMAXW(cpuRegs, 2);  _PMAXW(cpuRegs, 3);
}

static __fi void _PADDH(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rs_].US[n] + cpuRegs.GPR.r[_Rt_].US[n];
}

void PADDH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PADDH(cpuRegs, 0);  _PADDH(cpuRegs, 1);  _PADDH(cpuRegs, 2);  _PADDH(cpuRegs, 3);
	_PADDH(cpuRegs, 4);  _PADDH(cpuRegs, 5);  _PADDH(cpuRegs, 6);  _PADDH(cpuRegs, 7);
}

static __fi void _PSUBH(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rs_].US[n] - cpuRegs.GPR.r[_Rt_].US[n];
}

void PSUBH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSUBH(cpuRegs, 0);  _PSUBH(cpuRegs, 1);  _PSUBH(cpuRegs, 2);  _PSUBH(cpuRegs, 3);
	_PSUBH(cpuRegs, 4);  _PSUBH(cpuRegs, 5);  _PSUBH(cpuRegs, 6);  _PSUBH(cpuRegs, 7);
}

static __fi void _PCGTH(rr::MmiState& cpuRegs, int n)
{
	if (cpuRegs.GPR.r[_Rs_].SS[n] > cpuRegs.GPR.r[_Rt_].SS[n])
		cpuRegs.GPR.r[_Rd_].US[n] = 0xFFFF;
	else
		cpuRegs.GPR.r[_Rd_].US[n] = 0x0000;
}

void PCGTH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PCGTH(cpuRegs, 0);  _PCGTH(cpuRegs, 1);  _PCGTH(cpuRegs, 2);  _PCGTH(cpuRegs, 3);
	_PCGTH(cpuRegs, 4);  _PCGTH(cpuRegs, 5);  _PCGTH(cpuRegs, 6);  _PCGTH(cpuRegs, 7);
}

static __fi void _PMAXH(rr::MmiState& cpuRegs, int n)
{
	if (cpuRegs.GPR.r[_Rs_].SS[n] > cpuRegs.GPR.r[_Rt_].SS[n])
		cpuRegs.GPR.r[_Rd_].US[n] =  cpuRegs.GPR.r[_Rs_].US[n];
	else
		cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rt_].US[n];
}

void PMAXH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PMAXH(cpuRegs, 0);  _PMAXH(cpuRegs, 1);  _PMAXH(cpuRegs, 2);  _PMAXH(cpuRegs, 3);
	_PMAXH(cpuRegs, 4);  _PMAXH(cpuRegs, 5);  _PMAXH(cpuRegs, 6);  _PMAXH(cpuRegs, 7);
}

static __fi void _PADDB(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].SC[n] = cpuRegs.GPR.r[_Rs_].SC[n] + cpuRegs.GPR.r[_Rt_].SC[n];
}

void PADDB(rr::MmiState& cpuRegs) {
	int i;
	if (!_Rd_) return;

	for( i=0; i<16; i++ )
		_PADDB(cpuRegs,  i );
}

static __fi void _PSUBB(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].SC[n] = cpuRegs.GPR.r[_Rs_].SC[n] - cpuRegs.GPR.r[_Rt_].SC[n];
}

void PSUBB(rr::MmiState& cpuRegs) {
	int i;
	if (!_Rd_) return;

	for( i=0; i<16; i++ )
		_PSUBB(cpuRegs,  i );
}

static __fi void _PCGTB(rr::MmiState& cpuRegs, int n)
{
	if (cpuRegs.GPR.r[_Rs_].SC[n] > cpuRegs.GPR.r[_Rt_].SC[n])
		cpuRegs.GPR.r[_Rd_].UC[n] = 0xFF;
	else
		cpuRegs.GPR.r[_Rd_].UC[n] = 0x00;
}

void PCGTB(rr::MmiState& cpuRegs) {
	int i;
	if (!_Rd_) return;

	for( i=0; i<16; i++ )
		_PCGTB(cpuRegs,  i );
}

static __fi void _PADDSW(rr::MmiState& cpuRegs, int n)
{
	s64 sTemp64;

	sTemp64 = (s64)cpuRegs.GPR.r[_Rs_].SL[n] + (s64)cpuRegs.GPR.r[_Rt_].SL[n];
	if (sTemp64 > 0x7FFFFFFF)
		cpuRegs.GPR.r[_Rd_].UL[n] = 0x7FFFFFFF;
	else if ((sTemp64 < (s32)0x80000000) )
		cpuRegs.GPR.r[_Rd_].UL[n] = 0x80000000LL;
	else
		cpuRegs.GPR.r[_Rd_].UL[n] = (s32)sTemp64;
}

void PADDSW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PADDSW(cpuRegs, 0); _PADDSW(cpuRegs, 1); _PADDSW(cpuRegs, 2); _PADDSW(cpuRegs, 3);
}

static __fi void _PSUBSW(rr::MmiState& cpuRegs, int n)
{
	s64 sTemp64;

	sTemp64 = (s64)cpuRegs.GPR.r[_Rs_].SL[n] - (s64)cpuRegs.GPR.r[_Rt_].SL[n];

	if (sTemp64 >= 0x7FFFFFFF)
		cpuRegs.GPR.r[_Rd_].UL[n] = 0x7FFFFFFF;
	else if ((sTemp64 < (s32)0x80000000))
		cpuRegs.GPR.r[_Rd_].UL[n] = 0x80000000;
	else
		cpuRegs.GPR.r[_Rd_].UL[n] = (s32)sTemp64;
}

void PSUBSW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSUBSW(cpuRegs, 0);
	_PSUBSW(cpuRegs, 1);
	_PSUBSW(cpuRegs, 2);
	_PSUBSW(cpuRegs, 3);
}

void PEXTLW(rr::MmiState& cpuRegs) {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rs.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rt.UL[1];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rs.UL[1];
}

void PPACW(rr::MmiState& cpuRegs) {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rt.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rs.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rs.UL[2];
}

__fi void  _PADDSH(rr::MmiState& cpuRegs, int n)
{
	s32 sTemp32;
	sTemp32 = (s32)cpuRegs.GPR.r[_Rs_].SS[n] + (s32)cpuRegs.GPR.r[_Rt_].SS[n];

	if (sTemp32 > 0x7FFF)
		cpuRegs.GPR.r[_Rd_].US[n] = 0x7FFF;
	else if ((sTemp32 < (s32)0xffff8000) )
		cpuRegs.GPR.r[_Rd_].US[n] = 0x8000;
	else
		cpuRegs.GPR.r[_Rd_].US[n] = (s16)sTemp32;
}

void PADDSH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PADDSH(cpuRegs, 0); _PADDSH(cpuRegs, 1); _PADDSH(cpuRegs, 2); _PADDSH(cpuRegs, 3);
	_PADDSH(cpuRegs, 4); _PADDSH(cpuRegs, 5); _PADDSH(cpuRegs, 6); _PADDSH(cpuRegs, 7);
}

__fi void  _PSUBSH(rr::MmiState& cpuRegs, int n)
{
	s32 sTemp32;
	sTemp32 = (s32)cpuRegs.GPR.r[_Rs_].SS[n] - (s32)cpuRegs.GPR.r[_Rt_].SS[n];

	if (sTemp32 >= 0x7FFF)
		cpuRegs.GPR.r[_Rd_].US[n] = 0x7FFF;
	else if ((sTemp32 < (s32)0xffff8000) )
		cpuRegs.GPR.r[_Rd_].US[n] = 0x8000;
	else
		cpuRegs.GPR.r[_Rd_].US[n] = (s16)sTemp32;
}

void PSUBSH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSUBSH(cpuRegs, 0); _PSUBSH(cpuRegs, 1); _PSUBSH(cpuRegs, 2); _PSUBSH(cpuRegs, 3);
	_PSUBSH(cpuRegs, 4); _PSUBSH(cpuRegs, 5); _PSUBSH(cpuRegs, 6); _PSUBSH(cpuRegs, 7);
}

void PEXTLH(rr::MmiState& cpuRegs) {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rs.US[0];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[1];
	cpuRegs.GPR.r[_Rd_].US[3] = Rs.US[1];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[5] = Rs.US[2];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[3];
	cpuRegs.GPR.r[_Rd_].US[7] = Rs.US[3];
}

void PPACH(rr::MmiState& cpuRegs) {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[3] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[4] = Rs.US[0];
	cpuRegs.GPR.r[_Rd_].US[5] = Rs.US[2];
	cpuRegs.GPR.r[_Rd_].US[6] = Rs.US[4];
	cpuRegs.GPR.r[_Rd_].US[7] = Rs.US[6];
}

__fi void  _PADDSB(rr::MmiState& cpuRegs, int n)
{
	s16 sTemp16;
	sTemp16 = (s16)cpuRegs.GPR.r[_Rs_].SC[n] + (s16)cpuRegs.GPR.r[_Rt_].SC[n];

	if (sTemp16 > 0x7F)
		cpuRegs.GPR.r[_Rd_].UC[n] = 0x7F;
	else if (sTemp16 < /*(s16)0xff80*/(s16)-128) // be sure
		cpuRegs.GPR.r[_Rd_].UC[n] = 0x80;
	else
		cpuRegs.GPR.r[_Rd_].UC[n] = (s8)sTemp16;
}

void PADDSB(rr::MmiState& cpuRegs) {
	int i;
	if (!_Rd_) return;

	for( i=0; i<16; i++ )
		_PADDSB(cpuRegs, i);
}

static __fi void _PSUBSB(rr::MmiState& cpuRegs,  u8 n )
{
	s16 sTemp16;
	sTemp16 = (s16)cpuRegs.GPR.r[_Rs_].SC[n] - (s16)cpuRegs.GPR.r[_Rt_].SC[n];

	if (sTemp16 >= 0x7F)
		cpuRegs.GPR.r[_Rd_].UC[n] = 0x7F;
	else if (sTemp16 < /*(s16)0xff80*/(s16)-128) // be sure
		cpuRegs.GPR.r[_Rd_].UC[n] = 0x80;
	else
		cpuRegs.GPR.r[_Rd_].UC[n] = (s8)sTemp16;
}

void PSUBSB(rr::MmiState& cpuRegs) {
	int i;
	if (!_Rd_) return;

	for( i=0; i<16; i++ )
		_PSUBSB(cpuRegs, i);
}

void PEXTLB(rr::MmiState& cpuRegs) {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UC[0]  = Rt.UC[0];
	cpuRegs.GPR.r[_Rd_].UC[1]  = Rs.UC[0];
	cpuRegs.GPR.r[_Rd_].UC[2]  = Rt.UC[1];
	cpuRegs.GPR.r[_Rd_].UC[3]  = Rs.UC[1];

	cpuRegs.GPR.r[_Rd_].UC[4]  = Rt.UC[2];
	cpuRegs.GPR.r[_Rd_].UC[5]  = Rs.UC[2];
	cpuRegs.GPR.r[_Rd_].UC[6]  = Rt.UC[3];
	cpuRegs.GPR.r[_Rd_].UC[7]  = Rs.UC[3];

	cpuRegs.GPR.r[_Rd_].UC[8]  = Rt.UC[4];
	cpuRegs.GPR.r[_Rd_].UC[9]  = Rs.UC[4];
	cpuRegs.GPR.r[_Rd_].UC[10] = Rt.UC[5];
	cpuRegs.GPR.r[_Rd_].UC[11] = Rs.UC[5];

	cpuRegs.GPR.r[_Rd_].UC[12] = Rt.UC[6];
	cpuRegs.GPR.r[_Rd_].UC[13] = Rs.UC[6];
	cpuRegs.GPR.r[_Rd_].UC[14] = Rt.UC[7];
	cpuRegs.GPR.r[_Rd_].UC[15] = Rs.UC[7];
}

void PPACB(rr::MmiState& cpuRegs) {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UC[0]  = Rt.UC[0];
	cpuRegs.GPR.r[_Rd_].UC[1]  = Rt.UC[2];
	cpuRegs.GPR.r[_Rd_].UC[2]  = Rt.UC[4];
	cpuRegs.GPR.r[_Rd_].UC[3]  = Rt.UC[6];

	cpuRegs.GPR.r[_Rd_].UC[4]  = Rt.UC[8];
	cpuRegs.GPR.r[_Rd_].UC[5]  = Rt.UC[10];
	cpuRegs.GPR.r[_Rd_].UC[6]  = Rt.UC[12];
	cpuRegs.GPR.r[_Rd_].UC[7]  = Rt.UC[14];

	cpuRegs.GPR.r[_Rd_].UC[8]  = Rs.UC[0];
	cpuRegs.GPR.r[_Rd_].UC[9]  = Rs.UC[2];
	cpuRegs.GPR.r[_Rd_].UC[10] = Rs.UC[4];
	cpuRegs.GPR.r[_Rd_].UC[11] = Rs.UC[6];

	cpuRegs.GPR.r[_Rd_].UC[12] = Rs.UC[8];
	cpuRegs.GPR.r[_Rd_].UC[13] = Rs.UC[10];
	cpuRegs.GPR.r[_Rd_].UC[14] = Rs.UC[12];
	cpuRegs.GPR.r[_Rd_].UC[15] = Rs.UC[14];
}

__fi void  _PEXT5(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].UL[n] =
		((cpuRegs.GPR.r[_Rt_].UL[n] & 0x0000001F) <<  3) |
		((cpuRegs.GPR.r[_Rt_].UL[n] & 0x000003E0) <<  6) |
		((cpuRegs.GPR.r[_Rt_].UL[n] & 0x00007C00) <<  9) |
		((cpuRegs.GPR.r[_Rt_].UL[n] & 0x00008000) << 16);
}

void PEXT5(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PEXT5(cpuRegs, 0); _PEXT5(cpuRegs, 1); _PEXT5(cpuRegs, 2); _PEXT5(cpuRegs, 3);
}

__fi void  _PPAC5(rr::MmiState& cpuRegs, int n)
{
	cpuRegs.GPR.r[_Rd_].UL[n] =
		((cpuRegs.GPR.r[_Rt_].UL[n] >>  3) & 0x0000001F) |
		((cpuRegs.GPR.r[_Rt_].UL[n] >>  6) & 0x000003E0) |
		((cpuRegs.GPR.r[_Rt_].UL[n] >>  9) & 0x00007C00) |
		((cpuRegs.GPR.r[_Rt_].UL[n] >> 16) & 0x00008000);
}

void PPAC5(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PPAC5(cpuRegs, 0); _PPAC5(cpuRegs, 1); _PPAC5(cpuRegs, 2); _PPAC5(cpuRegs, 3);
}

//***END OF MMI0 OPCODES******************************************
//**********MMI1 OPCODES**************************************

static __fi void _PABSW(rr::MmiState& cpuRegs, int n)
{
	if (cpuRegs.GPR.r[_Rt_].UL[n] == 0x80000000)
		cpuRegs.GPR.r[_Rd_].UL[n] = 0x7fffffff; //clamp
	else if (cpuRegs.GPR.r[_Rt_].SL[n] < 0)
		cpuRegs.GPR.r[_Rd_].UL[n] = - cpuRegs.GPR.r[_Rt_].SL[n];
	else
		cpuRegs.GPR.r[_Rd_].UL[n] = cpuRegs.GPR.r[_Rt_].SL[n];
}

void PABSW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PABSW(cpuRegs, 0);  _PABSW(cpuRegs, 1);  _PABSW(cpuRegs, 2);  _PABSW(cpuRegs, 3);
}

static __fi void _PCEQW(rr::MmiState& cpuRegs, int n)
{
	if (cpuRegs.GPR.r[_Rs_].UL[n] == cpuRegs.GPR.r[_Rt_].UL[n])
		cpuRegs.GPR.r[_Rd_].UL[n] = 0xFFFFFFFF;
	else
		cpuRegs.GPR.r[_Rd_].UL[n] = 0x00000000;
}

void PCEQW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PCEQW(cpuRegs, 0); _PCEQW(cpuRegs, 1); _PCEQW(cpuRegs, 2); _PCEQW(cpuRegs, 3);
}

static __fi void _PMINW(rr::MmiState& cpuRegs,  u8 n )
{
	if (cpuRegs.GPR.r[_Rs_].SL[n] < cpuRegs.GPR.r[_Rt_].SL[n])
		cpuRegs.GPR.r[_Rd_].SL[n] = cpuRegs.GPR.r[_Rs_].SL[n];
	else
		cpuRegs.GPR.r[_Rd_].SL[n] = cpuRegs.GPR.r[_Rt_].SL[n];
}

void PMINW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PMINW(cpuRegs, 0); _PMINW(cpuRegs, 1); _PMINW(cpuRegs, 2); _PMINW(cpuRegs, 3);
}

void PADSBH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSUBH(cpuRegs, 0); _PSUBH(cpuRegs, 1); _PSUBH(cpuRegs, 2); _PSUBH(cpuRegs, 3);
	_PADDH(cpuRegs, 4); _PADDH(cpuRegs, 5); _PADDH(cpuRegs, 6); _PADDH(cpuRegs, 7);
}

static __fi void _PABSH(rr::MmiState& cpuRegs, int n)
{
	if (cpuRegs.GPR.r[_Rt_].US[n] == 0x8000)
		cpuRegs.GPR.r[_Rd_].US[n] = 0x7fff; //clamp
	else if (cpuRegs.GPR.r[_Rt_].SS[n] < 0)
		cpuRegs.GPR.r[_Rd_].US[n] = - cpuRegs.GPR.r[_Rt_].SS[n];
	else
		cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rt_].SS[n];
}

void PABSH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PABSH(cpuRegs, 0); _PABSH(cpuRegs, 1); _PABSH(cpuRegs, 2); _PABSH(cpuRegs, 3);
	_PABSH(cpuRegs, 4); _PABSH(cpuRegs, 5); _PABSH(cpuRegs, 6); _PABSH(cpuRegs, 7);
}

static __fi void  _PCEQH(rr::MmiState& cpuRegs,  u8 n )
{
	if (cpuRegs.GPR.r[_Rs_].US[n] == cpuRegs.GPR.r[_Rt_].US[n])
		cpuRegs.GPR.r[_Rd_].US[n] = 0xFFFF;
	else
		cpuRegs.GPR.r[_Rd_].US[n] = 0x0000;
}

void PCEQH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PCEQH(cpuRegs, 0); _PCEQH(cpuRegs, 1); _PCEQH(cpuRegs, 2); _PCEQH(cpuRegs, 3);
	_PCEQH(cpuRegs, 4); _PCEQH(cpuRegs, 5); _PCEQH(cpuRegs, 6); _PCEQH(cpuRegs, 7);
}

static __fi void  _PMINH(rr::MmiState& cpuRegs,  u8 n )
{
	if (cpuRegs.GPR.r[_Rs_].SS[n] < cpuRegs.GPR.r[_Rt_].SS[n])
		cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rs_].US[n];
	else
		cpuRegs.GPR.r[_Rd_].US[n] = cpuRegs.GPR.r[_Rt_].US[n];
}

void PMINH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PMINH(cpuRegs, 0); _PMINH(cpuRegs, 1); _PMINH(cpuRegs, 2); _PMINH(cpuRegs, 3);
	_PMINH(cpuRegs, 4); _PMINH(cpuRegs, 5); _PMINH(cpuRegs, 6); _PMINH(cpuRegs, 7);
}

__fi void  _PCEQB(rr::MmiState& cpuRegs, int n)
{
	if (cpuRegs.GPR.r[_Rs_].UC[n] == cpuRegs.GPR.r[_Rt_].UC[n])
		cpuRegs.GPR.r[_Rd_].UC[n] = 0xFF;
	else
		cpuRegs.GPR.r[_Rd_].UC[n] = 0x00;
}

void PCEQB(rr::MmiState& cpuRegs) {
	int i;
	if (!_Rd_) return;

	for( i=0; i<16; i++ )
		_PCEQB(cpuRegs, i);
}

__fi void  _PADDUW(rr::MmiState& cpuRegs, int n)
{
	s64 tmp;
	tmp = (s64)cpuRegs.GPR.r[_Rs_].UL[n] + (s64)cpuRegs.GPR.r[_Rt_].UL[n];

	if (tmp > 0xffffffff)
		 cpuRegs.GPR.r[_Rd_].UL[n] = 0xffffffff;
	else
		cpuRegs.GPR.r[_Rd_].UL[n] = (u32)tmp;
}

void PADDUW (rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PADDUW(cpuRegs, 0); _PADDUW(cpuRegs, 1); _PADDUW(cpuRegs, 2); _PADDUW(cpuRegs, 3);
}

__fi void  _PSUBUW(rr::MmiState& cpuRegs, int n)
{
	s64 sTemp64;
	sTemp64 = (s64)cpuRegs.GPR.r[_Rs_].UL[n] - (s64)cpuRegs.GPR.r[_Rt_].UL[n];

	if (sTemp64 <= 0x0)
		cpuRegs.GPR.r[_Rd_].UL[n] = 0x0;
	else
		cpuRegs.GPR.r[_Rd_].UL[n] = (u32)sTemp64;
}

void PSUBUW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSUBUW(cpuRegs, 0); _PSUBUW(cpuRegs, 1); _PSUBUW(cpuRegs, 2); _PSUBUW(cpuRegs, 3);
}

void PEXTUW(rr::MmiState& cpuRegs) {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rs.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rt.UL[3];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rs.UL[3];
}

__fi void  _PADDUH(rr::MmiState& cpuRegs, int n)
{
	s32 sTemp32;
	sTemp32 = (s32)cpuRegs.GPR.r[_Rs_].US[n] + (s32)cpuRegs.GPR.r[_Rt_].US[n];

	if (sTemp32 > 0xFFFF)
		cpuRegs.GPR.r[_Rd_].US[n] = 0xFFFF;
	else
		cpuRegs.GPR.r[_Rd_].US[n] = (u16)sTemp32;
}

void PADDUH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PADDUH(cpuRegs, 0); _PADDUH(cpuRegs, 1); _PADDUH(cpuRegs, 2); _PADDUH(cpuRegs, 3);
	_PADDUH(cpuRegs, 4); _PADDUH(cpuRegs, 5); _PADDUH(cpuRegs, 6); _PADDUH(cpuRegs, 7);
}

__fi void  _PSUBUH(rr::MmiState& cpuRegs, int n)
{
	s32 sTemp32;
	sTemp32 = (s32)cpuRegs.GPR.r[_Rs_].US[n] - (s32)cpuRegs.GPR.r[_Rt_].US[n];

	if (sTemp32 <= 0x0)
		cpuRegs.GPR.r[_Rd_].US[n] = 0x0;
	else
		cpuRegs.GPR.r[_Rd_].US[n] = (u16)sTemp32;
}

void PSUBUH(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	_PSUBUH(cpuRegs, 0); _PSUBUH(cpuRegs, 1); _PSUBUH(cpuRegs, 2); _PSUBUH(cpuRegs, 3);
	_PSUBUH(cpuRegs, 4); _PSUBUH(cpuRegs, 5); _PSUBUH(cpuRegs, 6); _PSUBUH(cpuRegs, 7);
}

void PEXTUH(rr::MmiState& cpuRegs) {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[1] = Rs.US[4];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[5];
	cpuRegs.GPR.r[_Rd_].US[3] = Rs.US[5];

	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[5] = Rs.US[6];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[7];
	cpuRegs.GPR.r[_Rd_].US[7] = Rs.US[7];
}

__fi void  _PADDUB(rr::MmiState& cpuRegs, int n)
{
	u16 Temp16;
	Temp16 = (u16)cpuRegs.GPR.r[_Rs_].UC[n] + (u16)cpuRegs.GPR.r[_Rt_].UC[n];

	if (Temp16 > 0xFF)
		cpuRegs.GPR.r[_Rd_].UC[n] = 0xFF;
	else
		cpuRegs.GPR.r[_Rd_].UC[n] = (u8)Temp16;
}

void PADDUB(rr::MmiState& cpuRegs) {
	int i;
	if (!_Rd_) return;

	for( i=0; i<16; i++ )
		 _PADDUB(cpuRegs, i);
}

__fi void  _PSUBUB(rr::MmiState& cpuRegs, int n) {
	s16 sTemp16;
	sTemp16 = (s16)cpuRegs.GPR.r[_Rs_].UC[n] - (s16)cpuRegs.GPR.r[_Rt_].UC[n];

	if (sTemp16 <= 0x0)
		cpuRegs.GPR.r[_Rd_].UC[n] = 0x0;
	else
		cpuRegs.GPR.r[_Rd_].UC[n] = (u8)sTemp16;
}

void PSUBUB(rr::MmiState& cpuRegs) {
	int i;
	if (!_Rd_) return;

	for( i=0; i<16; i++ )
		 _PSUBUB(cpuRegs, i);
}

void PEXTUB(rr::MmiState& cpuRegs) {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UC[0]  = Rt.UC[8];
	cpuRegs.GPR.r[_Rd_].UC[1]  = Rs.UC[8];
	cpuRegs.GPR.r[_Rd_].UC[2]  = Rt.UC[9];
	cpuRegs.GPR.r[_Rd_].UC[3]  = Rs.UC[9];
	cpuRegs.GPR.r[_Rd_].UC[4]  = Rt.UC[10];
	cpuRegs.GPR.r[_Rd_].UC[5]  = Rs.UC[10];
	cpuRegs.GPR.r[_Rd_].UC[6]  = Rt.UC[11];
	cpuRegs.GPR.r[_Rd_].UC[7]  = Rs.UC[11];
	cpuRegs.GPR.r[_Rd_].UC[8]  = Rt.UC[12];
	cpuRegs.GPR.r[_Rd_].UC[9]  = Rs.UC[12];
	cpuRegs.GPR.r[_Rd_].UC[10] = Rt.UC[13];
	cpuRegs.GPR.r[_Rd_].UC[11] = Rs.UC[13];
	cpuRegs.GPR.r[_Rd_].UC[12] = Rt.UC[14];
	cpuRegs.GPR.r[_Rd_].UC[13] = Rs.UC[14];
	cpuRegs.GPR.r[_Rd_].UC[14] = Rt.UC[15];
	cpuRegs.GPR.r[_Rd_].UC[15] = Rs.UC[15];
}

//int saZero = 0;
void QFSRV(rr::MmiState& cpuRegs) {				// JayteeMaster: changed a bit to avoid screw up
	GPR_reg Rd;
	if (!_Rd_) return;

	u32 sa_amt = cpuRegs.sa << 3;
	if (sa_amt == 0) {
		cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rt_].UD[0];
		cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rt_].UD[1];
		//saZero++;
		//if( saZero >= 388800 )
			//Console.WriteLn( "SA Is Zero, Bitch: %d zeros and counting.", saZero );
	} else {
		//Console.WriteLn( "SA Properly Valued at: %d (after %d zeros)", sa_amt, saZero );
		//saZero = 0;
		if (sa_amt < 64) {
			/*
			cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rt_].UD[0] >> sa_amt;
			cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rt_].UD[1] >> sa_amt;
			cpuRegs.GPR.r[_Rd_].UD[0]|= cpuRegs.GPR.r[_Rt_].UD[1] << (64 - sa_amt);
			cpuRegs.GPR.r[_Rd_].UD[1]|= cpuRegs.GPR.r[_Rs_].UD[0] << (64 - sa_amt);
			*/
			Rd.UD[0] = cpuRegs.GPR.r[_Rt_].UD[0] >> sa_amt;
			Rd.UD[1] = cpuRegs.GPR.r[_Rt_].UD[1] >> sa_amt;
			Rd.UD[0]|= cpuRegs.GPR.r[_Rt_].UD[1] << (64 - sa_amt);
			Rd.UD[1]|= cpuRegs.GPR.r[_Rs_].UD[0] << (64 - sa_amt);
			cpuRegs.GPR.r[_Rd_] = Rd;
		} else {
			/*
			cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rt_].UD[1] >> (sa_amt - 64);
			cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rs_].UD[0] >> (sa_amt - 64);
			cpuRegs.GPR.r[_Rd_].UD[0]|= cpuRegs.GPR.r[_Rs_].UD[0] << (128 - sa_amt);
			cpuRegs.GPR.r[_Rd_].UD[1]|= cpuRegs.GPR.r[_Rs_].UD[1] << (128 - sa_amt);
			*/
			Rd.UD[0] = cpuRegs.GPR.r[_Rt_].UD[1] >> (sa_amt - 64);
			Rd.UD[1] = cpuRegs.GPR.r[_Rs_].UD[0] >> (sa_amt - 64);
			if (sa_amt != 64) {
				// A 64 bit shift is equivalent to a 0 bit shift because value is masked
				// on 6 bits
				Rd.UD[0]|= cpuRegs.GPR.r[_Rs_].UD[0] << (128u - sa_amt);
				Rd.UD[1]|= cpuRegs.GPR.r[_Rs_].UD[1] << (128u - sa_amt);
			}
			cpuRegs.GPR.r[_Rd_] = Rd;
		}
	}
}

//********END OF MMI1 OPCODES***********************************

//*********MMI2 OPCODES***************************************

static __fi void _PMADDW(rr::MmiState& cpuRegs, int dd, int ss)
{
	s64 temp = ((s64)cpuRegs.GPR.r[_Rs_].SL[ss] * (s64)cpuRegs.GPR.r[_Rt_].SL[ss]);
	s64 temp2 = temp + ((s64)cpuRegs.HI.SL[ss] << 32);

	//PlayStation 2 division voodoo, for some reason only the lower half is affected
	if (ss == 0)
	{
		if (((cpuRegs.GPR.r[_Rt_].SL[ss] & 0x7FFFFFFF) == 0 || (cpuRegs.GPR.r[_Rt_].SL[ss] & 0x7FFFFFFF) == 0x7FFFFFFF) &&
			cpuRegs.GPR.r[_Rs_].SL[ss] != cpuRegs.GPR.r[_Rt_].SL[ss])
			temp2 += 0x70000000;
	}
	//Multiplication error on the PS2 causes this not to be exactly >> 32 (off by 1)
	temp2 = (s32)(temp2 / 4294967295);

	cpuRegs.LO.SD[dd] = (s32)(temp & 0xffffffff) + cpuRegs.LO.SL[ss];
	cpuRegs.HI.SD[dd] = (s32)temp2;

	if (_Rd_)
	{
		cpuRegs.GPR.r[_Rd_].UL[dd * 2] = cpuRegs.LO.UL[dd * 2];
		cpuRegs.GPR.r[_Rd_].UL[(dd * 2) + 1] = cpuRegs.HI.UL[dd * 2];
	}
}

void PMADDW(rr::MmiState& cpuRegs) {
	_PMADDW(cpuRegs, 0, 0);
	_PMADDW(cpuRegs, 1, 2);
}

void PSLLVW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].SD[0] = (s64)(s32)(cpuRegs.GPR.r[_Rt_].UL[0] <<
										  (cpuRegs.GPR.r[_Rs_].UL[0] & 0x1F));
	cpuRegs.GPR.r[_Rd_].SD[1] = (s64)(s32)(cpuRegs.GPR.r[_Rt_].UL[2] <<
										  (cpuRegs.GPR.r[_Rs_].UL[2] & 0x1F));
}

void PSRLVW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].SD[0] = (s64)(s32)(cpuRegs.GPR.r[_Rt_].UL[0] >>
										  (cpuRegs.GPR.r[_Rs_].UL[0] & 0x1F));
	cpuRegs.GPR.r[_Rd_].SD[1] = (s64)(s32)(cpuRegs.GPR.r[_Rt_].UL[2] >>
										  (cpuRegs.GPR.r[_Rs_].UL[2] & 0x1F));
}

__fi void  _PMSUBW(rr::MmiState& cpuRegs, int dd, int ss)
{
	s64 temp = ((s64)cpuRegs.GPR.r[_Rs_].SL[ss] * (s64)cpuRegs.GPR.r[_Rt_].SL[ss]);
	s64 temp2 = ((s64)cpuRegs.HI.SL[ss] << 32) - temp;

	//Multiplication error on the PS2 causes this not to be exactly >> 32 (off by 1)
	temp2 = (s32)(temp2 / 4294967295);

	cpuRegs.LO.SD[dd] = cpuRegs.LO.SL[ss] - (s32)(temp & 0xffffffff);
	cpuRegs.HI.SD[dd] = (s32)temp2;

	if (_Rd_)
	{
		cpuRegs.GPR.r[_Rd_].UL[dd * 2] = cpuRegs.LO.UL[dd * 2];
		cpuRegs.GPR.r[_Rd_].UL[(dd * 2) + 1] = cpuRegs.HI.UL[dd * 2];
	}
}

void PMSUBW(rr::MmiState& cpuRegs) {
	_PMSUBW(cpuRegs, 0, 0);
	_PMSUBW(cpuRegs, 1, 2);
}

void PMFHI(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.HI.UD[0];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.HI.UD[1];
}

void PMFLO(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[0];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.LO.UD[1];
}

void PINTH(rr::MmiState& cpuRegs) {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rs.US[4];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[1];
	cpuRegs.GPR.r[_Rd_].US[3] = Rs.US[5];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[5] = Rs.US[6];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[3];
	cpuRegs.GPR.r[_Rd_].US[7] = Rs.US[7];
}

__fi void  _PMULTW(rr::MmiState& cpuRegs, int dd, int ss)
{
	s64 temp = (s64)cpuRegs.GPR.r[_Rs_].SL[ss] * (s64)cpuRegs.GPR.r[_Rt_].SL[ss];

	cpuRegs.LO.UD[dd] = (s32)(temp & 0xffffffff);
	cpuRegs.HI.UD[dd] = (s32)(temp >> 32);

	if (_Rd_) cpuRegs.GPR.r[_Rd_].SD[dd] = temp;
}

void PMULTW(rr::MmiState& cpuRegs) {
	_PMULTW(cpuRegs, 0, 0);
	_PMULTW(cpuRegs, 1, 2);
}

__fi void  _PDIVW(rr::MmiState& cpuRegs, int dd, int ss)
{
	if (cpuRegs.GPR.r[_Rs_].UL[ss] == 0x80000000 && cpuRegs.GPR.r[_Rt_].UL[ss] == 0xffffffff)
	{
		cpuRegs.LO.SD[dd] = (s32)0x80000000;
		cpuRegs.HI.SD[dd] = (s32)0;
	}
	else if (cpuRegs.GPR.r[_Rt_].SL[ss] != 0)
	{
		cpuRegs.LO.SD[dd] = cpuRegs.GPR.r[_Rs_].SL[ss] / cpuRegs.GPR.r[_Rt_].SL[ss];
		cpuRegs.HI.SD[dd] = cpuRegs.GPR.r[_Rs_].SL[ss] % cpuRegs.GPR.r[_Rt_].SL[ss];
	}
	else
	{
		cpuRegs.LO.SD[dd] = (cpuRegs.GPR.r[_Rs_].SL[ss] < 0) ? 1 : -1;
		cpuRegs.HI.SD[dd] = cpuRegs.GPR.r[_Rs_].SL[ss];
	}
}

void PDIVW(rr::MmiState& cpuRegs) {
	_PDIVW(cpuRegs, 0, 0);
	_PDIVW(cpuRegs, 1, 2);
}

void PCPYLD(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	// note: first _Rs_, since the other way when _Rd_ equals
	// _Rs_ or _Rt_ this would screw up
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rs_].UD[0];
	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rt_].UD[0];
}

void PMADDH(rr::MmiState& cpuRegs) {			// JayteeMaster: changed a bit to avoid screw up
	s32 temp;

    temp = cpuRegs.LO.UL[0] + (s32)cpuRegs.GPR.r[_Rs_].SS[0] * (s32)cpuRegs.GPR.r[_Rt_].SS[0];
    cpuRegs.LO.UL[0] = temp;
    /* if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[0] = temp; */

    temp = cpuRegs.LO.UL[1] + (s32)cpuRegs.GPR.r[_Rs_].SS[1] * (s32)cpuRegs.GPR.r[_Rt_].SS[1];
    cpuRegs.LO.UL[1] = temp;

    temp = cpuRegs.HI.UL[0] + (s32)cpuRegs.GPR.r[_Rs_].SS[2] * (s32)cpuRegs.GPR.r[_Rt_].SS[2];
    cpuRegs.HI.UL[0] = temp;
    /* if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[1] = temp; */

    temp = cpuRegs.HI.UL[1] + (s32)cpuRegs.GPR.r[_Rs_].SS[3] * (s32)cpuRegs.GPR.r[_Rt_].SS[3];
    cpuRegs.HI.UL[1] = temp;

    temp = cpuRegs.LO.UL[2] + (s32)cpuRegs.GPR.r[_Rs_].SS[4] * (s32)cpuRegs.GPR.r[_Rt_].SS[4];
    cpuRegs.LO.UL[2] = temp;
    /* if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[2] = temp; */

    temp = cpuRegs.LO.UL[3] + (s32)cpuRegs.GPR.r[_Rs_].SS[5] * (s32)cpuRegs.GPR.r[_Rt_].SS[5];
    cpuRegs.LO.UL[3] = temp;

    temp = cpuRegs.HI.UL[2] + (s32)cpuRegs.GPR.r[_Rs_].SS[6] * (s32)cpuRegs.GPR.r[_Rt_].SS[6];
    cpuRegs.HI.UL[2] = temp;
    /* if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[3] = temp; */

    temp = cpuRegs.HI.UL[3] + (s32)cpuRegs.GPR.r[_Rs_].SS[7] * (s32)cpuRegs.GPR.r[_Rt_].SS[7];
    cpuRegs.HI.UL[3] = temp;

	if (_Rd_) {
		cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
		cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
	}

}

// JayteeMaster: changed a bit to avoid screw up
__fi void  _PHMADH_LO(rr::MmiState& cpuRegs, int dd, int n)
{
	s32 firsttemp =		   (s32)cpuRegs.GPR.r[_Rs_].SS[n+1] * (s32)cpuRegs.GPR.r[_Rt_].SS[n+1];
	s32 temp = firsttemp + (s32)cpuRegs.GPR.r[_Rs_].SS[n]   * (s32)cpuRegs.GPR.r[_Rt_].SS[n];

	cpuRegs.LO.UL[dd] = temp;
	cpuRegs.LO.UL[dd+1] = firsttemp;
}

__fi void  _PHMADH_HI(rr::MmiState& cpuRegs, int dd, int n)
{
	s32 firsttemp =		   (s32)cpuRegs.GPR.r[_Rs_].SS[n+1] * (s32)cpuRegs.GPR.r[_Rt_].SS[n+1];
	s32 temp = firsttemp + (s32)cpuRegs.GPR.r[_Rs_].SS[n]   * (s32)cpuRegs.GPR.r[_Rt_].SS[n];

	cpuRegs.HI.UL[dd] = temp;
	cpuRegs.HI.UL[dd+1] = firsttemp;
}

void PHMADH(rr::MmiState& cpuRegs) {				// JayteeMaster: changed a bit to avoid screw up. Also used 0,2,4,6 instead of 0,1,2,3
	_PHMADH_LO(cpuRegs, 0, 0);
	_PHMADH_HI(cpuRegs, 0, 2);
	_PHMADH_LO(cpuRegs, 2, 4);
	_PHMADH_HI(cpuRegs, 2, 6);
	if (_Rd_) {
		cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
		cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
	}
}

void PAND(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] & cpuRegs.GPR.r[_Rt_].UD[0];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rs_].UD[1] & cpuRegs.GPR.r[_Rt_].UD[1];
}

void PXOR(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] ^ cpuRegs.GPR.r[_Rt_].UD[0];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rs_].UD[1] ^ cpuRegs.GPR.r[_Rt_].UD[1];
}

void PMSUBH(rr::MmiState& cpuRegs) {			// JayteeMaster: changed a bit to avoid screw up
	s32 temp;

    temp = cpuRegs.LO.UL[0] - (s32)cpuRegs.GPR.r[_Rs_].SS[0] * (s32)cpuRegs.GPR.r[_Rt_].SS[0];
    cpuRegs.LO.UL[0] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[0] = temp;*/

    temp = cpuRegs.LO.UL[1] - (s32)cpuRegs.GPR.r[_Rs_].SS[1] * (s32)cpuRegs.GPR.r[_Rt_].SS[1];
    cpuRegs.LO.UL[1] = temp;

    temp = cpuRegs.HI.UL[0] - (s32)cpuRegs.GPR.r[_Rs_].SS[2] * (s32)cpuRegs.GPR.r[_Rt_].SS[2];
    cpuRegs.HI.UL[0] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[1] = temp;*/

    temp = cpuRegs.HI.UL[1] - (s32)cpuRegs.GPR.r[_Rs_].SS[3] * (s32)cpuRegs.GPR.r[_Rt_].SS[3];
    cpuRegs.HI.UL[1] = temp;

    temp = cpuRegs.LO.UL[2] - (s32)cpuRegs.GPR.r[_Rs_].SS[4] * (s32)cpuRegs.GPR.r[_Rt_].SS[4];
    cpuRegs.LO.UL[2] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[2] = temp;*/

    temp = cpuRegs.LO.UL[3] - (s32)cpuRegs.GPR.r[_Rs_].SS[5] * (s32)cpuRegs.GPR.r[_Rt_].SS[5];
    cpuRegs.LO.UL[3] = temp;

    temp = cpuRegs.HI.UL[2] - (s32)cpuRegs.GPR.r[_Rs_].SS[6] * (s32)cpuRegs.GPR.r[_Rt_].SS[6];
    cpuRegs.HI.UL[2] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[3] = temp;*/

    temp = cpuRegs.HI.UL[3] - (s32)cpuRegs.GPR.r[_Rs_].SS[7] * (s32)cpuRegs.GPR.r[_Rt_].SS[7];
    cpuRegs.HI.UL[3] = temp;

	if (_Rd_) {
		cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
		cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
	}
}

// JayteeMaster: changed a bit to avoid screw up
static __fi void _PHMSBH_LO(rr::MmiState& cpuRegs, int dd, int n)
{
	s32 firsttemp =        (s32)cpuRegs.GPR.r[_Rs_].SS[n+1] * (s32)cpuRegs.GPR.r[_Rt_].SS[n+1];
	s32 temp = firsttemp - (s32)cpuRegs.GPR.r[_Rs_].SS[n]   * (s32)cpuRegs.GPR.r[_Rt_].SS[n];

	cpuRegs.LO.UL[dd] = temp;
	cpuRegs.LO.UL[dd+1] = ~firsttemp;  // undocumented behaviour
}
static __fi void _PHMSBH_HI(rr::MmiState& cpuRegs, int dd, int n)
{
	s32 firsttemp =        (s32)cpuRegs.GPR.r[_Rs_].SS[n+1] * (s32)cpuRegs.GPR.r[_Rt_].SS[n+1];
	s32 temp = firsttemp - (s32)cpuRegs.GPR.r[_Rs_].SS[n]   * (s32)cpuRegs.GPR.r[_Rt_].SS[n];

	cpuRegs.HI.UL[dd] = temp;
	cpuRegs.HI.UL[dd+1] = ~firsttemp;  // undocumented behaviour
}

void PHMSBH(rr::MmiState& cpuRegs) {		// JayteeMaster: changed a bit to avoid screw up
	_PHMSBH_LO(cpuRegs, 0, 0);
	_PHMSBH_HI(cpuRegs, 0, 2);
	_PHMSBH_LO(cpuRegs, 2, 4);
	_PHMSBH_HI(cpuRegs, 2, 6);
	if (_Rd_) {
		cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
		cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
	}
}

void PEXEH(rr::MmiState& cpuRegs) {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[1] = Rt.US[1];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[3] = Rt.US[3];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[5] = Rt.US[5];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[7] = Rt.US[7];
}

void PREVH (rr::MmiState& cpuRegs) {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[3];
	cpuRegs.GPR.r[_Rd_].US[1] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[1];
	cpuRegs.GPR.r[_Rd_].US[3] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[7];
	cpuRegs.GPR.r[_Rd_].US[5] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[5];
	cpuRegs.GPR.r[_Rd_].US[7] = Rt.US[4];
}

void PMULTH(rr::MmiState& cpuRegs) {			// JayteeMaster: changed a bit to avoid screw up
	s32 temp;

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[0] * (s32)cpuRegs.GPR.r[_Rt_].SS[0];
    cpuRegs.LO.UL[0] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[0] = temp;*/

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[1] * (s32)cpuRegs.GPR.r[_Rt_].SS[1];
    cpuRegs.LO.UL[1] = temp;

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[2] * (s32)cpuRegs.GPR.r[_Rt_].SS[2];
    cpuRegs.HI.UL[0] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[1] = temp;*/

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[3] * (s32)cpuRegs.GPR.r[_Rt_].SS[3];
    cpuRegs.HI.UL[1] = temp;

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[4] * (s32)cpuRegs.GPR.r[_Rt_].SS[4];
    cpuRegs.LO.UL[2] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[2] = temp;*/

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[5] * (s32)cpuRegs.GPR.r[_Rt_].SS[5];
    cpuRegs.LO.UL[3] = temp;

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[6] * (s32)cpuRegs.GPR.r[_Rt_].SS[6];
    cpuRegs.HI.UL[2] = temp;
    /*if (_Rd_) cpuRegs.GPR.r[_Rd_].UL[3] = temp;*/

    temp = (s32)cpuRegs.GPR.r[_Rs_].SS[7] * (s32)cpuRegs.GPR.r[_Rt_].SS[7];
    cpuRegs.HI.UL[3] = temp;

	if (_Rd_) {
		cpuRegs.GPR.r[_Rd_].UL[0] = cpuRegs.LO.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[1] = cpuRegs.HI.UL[0];
		cpuRegs.GPR.r[_Rd_].UL[2] = cpuRegs.LO.UL[2];
		cpuRegs.GPR.r[_Rd_].UL[3] = cpuRegs.HI.UL[2];
	}
}

__fi void  _PDIVBW(rr::MmiState& cpuRegs, int n)
{
	if (cpuRegs.GPR.r[_Rs_].UL[n] == 0x80000000 && cpuRegs.GPR.r[_Rt_].US[0] == 0xffff)
	{
		cpuRegs.LO.SL[n] = (s32)0x80000000;
		cpuRegs.HI.SL[n] = (s32)0x0;
	}
    else if (cpuRegs.GPR.r[_Rt_].US[0] != 0)
    {
        cpuRegs.LO.SL[n] = cpuRegs.GPR.r[_Rs_].SL[n] / cpuRegs.GPR.r[_Rt_].SS[0];
        cpuRegs.HI.SL[n] = cpuRegs.GPR.r[_Rs_].SL[n] % cpuRegs.GPR.r[_Rt_].SS[0];
    }
	else
	{
		cpuRegs.LO.SL[n] = (cpuRegs.GPR.r[_Rs_].SL[n] < 0) ? 1 : -1;
		cpuRegs.HI.SL[n] = cpuRegs.GPR.r[_Rs_].SL[n];
	}
}

void PDIVBW(rr::MmiState& cpuRegs) {
	_PDIVBW(cpuRegs, 0); _PDIVBW(cpuRegs, 1); _PDIVBW(cpuRegs, 2); _PDIVBW(cpuRegs, 3);
}

void PEXEW(rr::MmiState& cpuRegs) {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rt.UL[1];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rt.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rt.UL[3];
}

void PROT3W(rr::MmiState& cpuRegs) {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[1];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rt.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rt.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rt.UL[3];
}

//*****END OF MMI2 OPCODES***********************************

//*************************MMI3 OPCODES************************

static __fi void _PMADDUW(rr::MmiState& cpuRegs, int dd, int ss)
{
	u64 tempu =	(u64)((u64)cpuRegs.LO.UL[ss] | ((u64)cpuRegs.HI.UL[ss] << 32)) + \
			   ((u64)cpuRegs.GPR.r[_Rs_].UL[ss] * (u64)cpuRegs.GPR.r[_Rt_].UL[ss]);

	cpuRegs.LO.SD[dd] = (s32)(tempu & 0xffffffff);
	cpuRegs.HI.SD[dd] = (s32)(tempu >> 32);

	if (_Rd_) cpuRegs.GPR.r[_Rd_].UD[dd] = tempu;
}

void PMADDUW(rr::MmiState& cpuRegs) {
	_PMADDUW(cpuRegs, 0, 0);
	_PMADDUW(cpuRegs, 1, 2);
}

void PSRAVW(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].SD[0] = (s64)(cpuRegs.GPR.r[_Rt_].SL[0] >>
									 (cpuRegs.GPR.r[_Rs_].UL[0] & 0x1F));
	cpuRegs.GPR.r[_Rd_].SD[1] = (s64)(cpuRegs.GPR.r[_Rt_].SL[2] >>
									 (cpuRegs.GPR.r[_Rs_].UL[2] & 0x1F));
}

void PMTHI(rr::MmiState& cpuRegs) {
	cpuRegs.HI.UD[0] = cpuRegs.GPR.r[_Rs_].UD[0];
	cpuRegs.HI.UD[1] = cpuRegs.GPR.r[_Rs_].UD[1];
}

void PMTLO(rr::MmiState& cpuRegs) {
	cpuRegs.LO.UD[0] = cpuRegs.GPR.r[_Rs_].UD[0];
	cpuRegs.LO.UD[1] = cpuRegs.GPR.r[_Rs_].UD[1];
}

void PINTEH(rr::MmiState& cpuRegs) {
	GPR_reg Rs, Rt;

	if (!_Rd_) return;

	Rs = cpuRegs.GPR.r[_Rs_]; Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rs.US[0];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[3] = Rs.US[2];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[5] = Rs.US[4];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[7] = Rs.US[6];
}

__fi void  _PMULTUW(rr::MmiState& cpuRegs, int dd, int ss)
{
   u64 tempu = (u64)cpuRegs.GPR.r[_Rs_].UL[ss] * (u64)cpuRegs.GPR.r[_Rt_].UL[ss];

   cpuRegs.LO.UD[dd] = (s32)(tempu & 0xffffffff);
   cpuRegs.HI.UD[dd] = (s32)(tempu >> 32);

   if (_Rd_) cpuRegs.GPR.r[_Rd_].UD[dd] = tempu;
}


void PMULTUW(rr::MmiState& cpuRegs) {
	_PMULTUW(cpuRegs, 0, 0);
	_PMULTUW(cpuRegs, 1, 2);
}

__fi void  _PDIVUW(rr::MmiState& cpuRegs, int dd, int ss)
{
	if (cpuRegs.GPR.r[_Rt_].UL[ss] != 0) {
		cpuRegs.LO.SD[dd] = (s32)(cpuRegs.GPR.r[_Rs_].UL[ss] / cpuRegs.GPR.r[_Rt_].UL[ss]);
		cpuRegs.HI.SD[dd] = (s32)(cpuRegs.GPR.r[_Rs_].UL[ss] % cpuRegs.GPR.r[_Rt_].UL[ss]);
	}
	else
	{
		cpuRegs.LO.SD[dd] = -1;
		cpuRegs.HI.SD[dd] = cpuRegs.GPR.r[_Rs_].SL[ss];
	}
}

void PDIVUW(rr::MmiState& cpuRegs) {
	_PDIVUW(cpuRegs, 0, 0);
	_PDIVUW(cpuRegs, 1, 2);
}

void PCPYUD(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	// note: first _Rs_, since the other way when _Rd_ equals
	// _Rs_ or _Rt_ this would screw up
	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[1];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rt_].UD[1];
}

void POR(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] | cpuRegs.GPR.r[_Rt_].UD[0];
	cpuRegs.GPR.r[_Rd_].UD[1] = cpuRegs.GPR.r[_Rs_].UD[1] | cpuRegs.GPR.r[_Rt_].UD[1];
}

void PNOR (rr::MmiState& cpuRegs) {
	if (!_Rd_) return;

	cpuRegs.GPR.r[_Rd_].UD[0] = ~(cpuRegs.GPR.r[_Rs_].UD[0] | cpuRegs.GPR.r[_Rt_].UD[0]);
	cpuRegs.GPR.r[_Rd_].UD[1] = ~(cpuRegs.GPR.r[_Rs_].UD[1] | cpuRegs.GPR.r[_Rt_].UD[1]);
}

void PEXCH(rr::MmiState& cpuRegs) {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rt.US[2];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[1];
	cpuRegs.GPR.r[_Rd_].US[3] = Rt.US[3];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[5] = Rt.US[6];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[5];
	cpuRegs.GPR.r[_Rd_].US[7] = Rt.US[7];
}

void PCPYH(rr::MmiState& cpuRegs) {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].US[0] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[1] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[2] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[3] = Rt.US[0];
	cpuRegs.GPR.r[_Rd_].US[4] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[5] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[6] = Rt.US[4];
	cpuRegs.GPR.r[_Rd_].US[7] = Rt.US[4];
}

void PEXCW(rr::MmiState& cpuRegs) {
	GPR_reg Rt;

	if (!_Rd_) return;

	Rt = cpuRegs.GPR.r[_Rt_];
	cpuRegs.GPR.r[_Rd_].UL[0] = Rt.UL[0];
	cpuRegs.GPR.r[_Rd_].UL[1] = Rt.UL[2];
	cpuRegs.GPR.r[_Rd_].UL[2] = Rt.UL[1];
	cpuRegs.GPR.r[_Rd_].UL[3] = Rt.UL[3];
}

//**********************END OF MMI3 OPCODES********************

// obs:
// QFSRV not verified

}}} } // end namespace R5900::Interpreter::OpcodeImpl::MMI

static bool validMmi(uint32_t instruction) {
    if ((instruction >> 26) != 0x1cu) return false;
    switch (instruction & 63u) {
    case 0: return true;
    case 1: return true;
    case 4: return true;
    case 8: return ((0xcfff07ffu >> ((instruction >> 6) & 31u)) & 1u) != 0;
    case 9: return ((0xfc3f771du >> ((instruction >> 6) & 31u)) & 1u) != 0;
    case 16: return true;
    case 17: return true;
    case 18: return true;
    case 19: return true;
    case 24: return true;
    case 25: return true;
    case 26: return true;
    case 27: return true;
    case 32: return true;
    case 33: return true;
    case 40: return ((0x0f7704feu >> ((instruction >> 6) & 31u)) & 1u) != 0;
    case 41: return ((0x4c0c7709u >> ((instruction >> 6) & 31u)) & 1u) != 0;
    case 48: return true;
    case 49: return true;
    case 52: return true;
    case 54: return true;
    case 55: return true;
    case 60: return true;
    case 62: return true;
    case 63: return true;
    default: return false;
    }
}
bool rr::executeMmi(MmiState& cpuRegs, uint32_t instruction) noexcept {
    if (!validMmi(instruction)) return false;
    cpuRegs.code=instruction;
    switch (instruction & 63u) {
    case 0: rr_mmi47k::Interpreter::OpcodeImpl::MADD(cpuRegs); return true;
    case 1: rr_mmi47k::Interpreter::OpcodeImpl::MADDU(cpuRegs); return true;
    case 4: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PLZCW(cpuRegs); return true;
    case 8: switch ((instruction >> 6) & 31u) {
        case 0: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PADDW(cpuRegs); return true;
        case 1: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSUBW(cpuRegs); return true;
        case 2: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PCGTW(cpuRegs); return true;
        case 3: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMAXW(cpuRegs); return true;
        case 4: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PADDH(cpuRegs); return true;
        case 5: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSUBH(cpuRegs); return true;
        case 6: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PCGTH(cpuRegs); return true;
        case 7: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMAXH(cpuRegs); return true;
        case 8: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PADDB(cpuRegs); return true;
        case 9: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSUBB(cpuRegs); return true;
        case 10: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PCGTB(cpuRegs); return true;
        case 16: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PADDSW(cpuRegs); return true;
        case 17: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSUBSW(cpuRegs); return true;
        case 18: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PEXTLW(cpuRegs); return true;
        case 19: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PPACW(cpuRegs); return true;
        case 20: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PADDSH(cpuRegs); return true;
        case 21: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSUBSH(cpuRegs); return true;
        case 22: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PEXTLH(cpuRegs); return true;
        case 23: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PPACH(cpuRegs); return true;
        case 24: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PADDSB(cpuRegs); return true;
        case 25: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSUBSB(cpuRegs); return true;
        case 26: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PEXTLB(cpuRegs); return true;
        case 27: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PPACB(cpuRegs); return true;
        case 30: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PEXT5(cpuRegs); return true;
        case 31: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PPAC5(cpuRegs); return true;
        default: return false; }
    case 9: switch ((instruction >> 6) & 31u) {
        case 0: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMADDW(cpuRegs); return true;
        case 2: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSLLVW(cpuRegs); return true;
        case 3: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSRLVW(cpuRegs); return true;
        case 4: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMSUBW(cpuRegs); return true;
        case 8: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMFHI(cpuRegs); return true;
        case 9: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMFLO(cpuRegs); return true;
        case 10: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PINTH(cpuRegs); return true;
        case 12: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMULTW(cpuRegs); return true;
        case 13: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PDIVW(cpuRegs); return true;
        case 14: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PCPYLD(cpuRegs); return true;
        case 16: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMADDH(cpuRegs); return true;
        case 17: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PHMADH(cpuRegs); return true;
        case 18: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PAND(cpuRegs); return true;
        case 19: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PXOR(cpuRegs); return true;
        case 20: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMSUBH(cpuRegs); return true;
        case 21: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PHMSBH(cpuRegs); return true;
        case 26: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PEXEH(cpuRegs); return true;
        case 27: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PREVH(cpuRegs); return true;
        case 28: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMULTH(cpuRegs); return true;
        case 29: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PDIVBW(cpuRegs); return true;
        case 30: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PEXEW(cpuRegs); return true;
        case 31: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PROT3W(cpuRegs); return true;
        default: return false; }
    case 16: rr_mmi47k::Interpreter::OpcodeImpl::MFHI1(cpuRegs); return true;
    case 17: rr_mmi47k::Interpreter::OpcodeImpl::MTHI1(cpuRegs); return true;
    case 18: rr_mmi47k::Interpreter::OpcodeImpl::MFLO1(cpuRegs); return true;
    case 19: rr_mmi47k::Interpreter::OpcodeImpl::MTLO1(cpuRegs); return true;
    case 24: rr_mmi47k::Interpreter::OpcodeImpl::MULT1(cpuRegs); return true;
    case 25: rr_mmi47k::Interpreter::OpcodeImpl::MULTU1(cpuRegs); return true;
    case 26: rr_mmi47k::Interpreter::OpcodeImpl::DIV1(cpuRegs); return true;
    case 27: rr_mmi47k::Interpreter::OpcodeImpl::DIVU1(cpuRegs); return true;
    case 32: rr_mmi47k::Interpreter::OpcodeImpl::MADD1(cpuRegs); return true;
    case 33: rr_mmi47k::Interpreter::OpcodeImpl::MADDU1(cpuRegs); return true;
    case 40: switch ((instruction >> 6) & 31u) {
        case 1: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PABSW(cpuRegs); return true;
        case 2: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PCEQW(cpuRegs); return true;
        case 3: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMINW(cpuRegs); return true;
        case 4: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PADSBH(cpuRegs); return true;
        case 5: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PABSH(cpuRegs); return true;
        case 6: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PCEQH(cpuRegs); return true;
        case 7: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMINH(cpuRegs); return true;
        case 10: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PCEQB(cpuRegs); return true;
        case 16: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PADDUW(cpuRegs); return true;
        case 17: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSUBUW(cpuRegs); return true;
        case 18: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PEXTUW(cpuRegs); return true;
        case 20: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PADDUH(cpuRegs); return true;
        case 21: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSUBUH(cpuRegs); return true;
        case 22: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PEXTUH(cpuRegs); return true;
        case 24: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PADDUB(cpuRegs); return true;
        case 25: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSUBUB(cpuRegs); return true;
        case 26: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PEXTUB(cpuRegs); return true;
        case 27: rr_mmi47k::Interpreter::OpcodeImpl::MMI::QFSRV(cpuRegs); return true;
        default: return false; }
    case 41: switch ((instruction >> 6) & 31u) {
        case 0: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMADDUW(cpuRegs); return true;
        case 3: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSRAVW(cpuRegs); return true;
        case 8: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMTHI(cpuRegs); return true;
        case 9: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMTLO(cpuRegs); return true;
        case 10: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PINTEH(cpuRegs); return true;
        case 12: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMULTUW(cpuRegs); return true;
        case 13: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PDIVUW(cpuRegs); return true;
        case 14: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PCPYUD(cpuRegs); return true;
        case 18: rr_mmi47k::Interpreter::OpcodeImpl::MMI::POR(cpuRegs); return true;
        case 19: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PNOR(cpuRegs); return true;
        case 26: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PEXCH(cpuRegs); return true;
        case 27: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PCPYH(cpuRegs); return true;
        case 30: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PEXCW(cpuRegs); return true;
        default: return false; }
    case 48: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMFHL(cpuRegs); return true;
    case 49: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PMTHL(cpuRegs); return true;
    case 52: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSLLH(cpuRegs); return true;
    case 54: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSRLH(cpuRegs); return true;
    case 55: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSRAH(cpuRegs); return true;
    case 60: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSLLW(cpuRegs); return true;
    case 62: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSRLW(cpuRegs); return true;
    case 63: rr_mmi47k::Interpreter::OpcodeImpl::MMI::PSRAW(cpuRegs); return true;
    default: return false;
    }
}
