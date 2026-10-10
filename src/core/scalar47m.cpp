// SPDX-FileCopyrightText: 2002-2026 PCSX2 Dev Team
// SPDX-License-Identifier: GPL-3.0+
#include "../../third_party/pcsx2-mmi47k/mmi_compat.h"
#include "scalar47m.h"
#define _Imm_ static_cast<s16>(cpuRegs.code)
#define _ImmU_ static_cast<u16>(cpuRegs.code)
namespace rr_scalar47m {
void ADDIU(rr::MmiState& cpuRegs)
{
	if (!_Rt_) return;
	cpuRegs.GPR.r[_Rt_].UD[0] = u64(s64(s32(cpuRegs.GPR.r[_Rs_].UL[0] + u32(s32(_Imm_)))));
}
void DADDIU(rr::MmiState& cpuRegs)
{
	if (!_Rt_) return;
	cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] + u64(s64(_Imm_));
}
void ANDI(rr::MmiState& cpuRegs) 	{ if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] & (u64)_ImmU_; }
void ORI(rr::MmiState& cpuRegs) 	    { if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] | (u64)_ImmU_; }
void XORI(rr::MmiState& cpuRegs) 	{ if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0] ^ (u64)_ImmU_; }
void SLTI(rr::MmiState& cpuRegs)     { if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = (cpuRegs.GPR.r[_Rs_].SD[0] < (s64)(_Imm_)) ? 1 : 0; }
void SLTIU(rr::MmiState& cpuRegs)    { if (!_Rt_) return; cpuRegs.GPR.r[_Rt_].UD[0] = (cpuRegs.GPR.r[_Rs_].UD[0] < (u64)(_Imm_)) ? 1 : 0; }
void ADDU(rr::MmiState& cpuRegs) 	{ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = u64(s64(s32(cpuRegs.GPR.r[_Rs_].UL[0]  + cpuRegs.GPR.r[_Rt_].UL[0]))); }
void DADDU(rr::MmiState& cpuRegs)    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]  + cpuRegs.GPR.r[_Rt_].UD[0]; }
void SUBU(rr::MmiState& cpuRegs) 	{ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = u64(s64(s32(cpuRegs.GPR.r[_Rs_].UL[0]  - cpuRegs.GPR.r[_Rt_].UL[0]))); }
void DSUBU(rr::MmiState& cpuRegs) 	{ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]  - cpuRegs.GPR.r[_Rt_].UD[0]; }
void AND(rr::MmiState& cpuRegs) 	    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]  & cpuRegs.GPR.r[_Rt_].UD[0]; }
void OR(rr::MmiState& cpuRegs) 	    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]  | cpuRegs.GPR.r[_Rt_].UD[0]; }
void XOR(rr::MmiState& cpuRegs) 	    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]  ^ cpuRegs.GPR.r[_Rt_].UD[0]; }
void NOR(rr::MmiState& cpuRegs) 	    { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] =~(cpuRegs.GPR.r[_Rs_].UD[0] | cpuRegs.GPR.r[_Rt_].UD[0]); }
void SLT(rr::MmiState& cpuRegs)		{ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (cpuRegs.GPR.r[_Rs_].SD[0] < cpuRegs.GPR.r[_Rt_].SD[0]) ? 1 : 0; }
void SLTU(rr::MmiState& cpuRegs)		{ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (cpuRegs.GPR.r[_Rs_].UD[0] < cpuRegs.GPR.r[_Rt_].UD[0]) ? 1 : 0; }
void DIV(rr::MmiState& cpuRegs)
{
	if (cpuRegs.GPR.r[_Rs_].UL[0] == 0x80000000 && cpuRegs.GPR.r[_Rt_].UL[0] == 0xffffffff)
	{
		cpuRegs.LO.SD[0] = (s32)0x80000000;
		cpuRegs.HI.SD[0] = (s32)0x0;
	}
    else if (cpuRegs.GPR.r[_Rt_].SL[0] != 0)
    {
        cpuRegs.LO.SD[0] = cpuRegs.GPR.r[_Rs_].SL[0] / cpuRegs.GPR.r[_Rt_].SL[0];
        cpuRegs.HI.SD[0] = cpuRegs.GPR.r[_Rs_].SL[0] % cpuRegs.GPR.r[_Rt_].SL[0];
    }
	else
	{
		cpuRegs.LO.SD[0] = (cpuRegs.GPR.r[_Rs_].SL[0] < 0) ? 1 : -1;
		cpuRegs.HI.SD[0] = cpuRegs.GPR.r[_Rs_].SL[0];
	}
}
void DIVU(rr::MmiState& cpuRegs)
{
	if (cpuRegs.GPR.r[_Rt_].UL[0] != 0)
	{
		// note: DIVU has no sign extension when assigning back to 64 bits
		// note 2: reference material strongly disagrees. (air)
		cpuRegs.LO.SD[0] = (s32)(cpuRegs.GPR.r[_Rs_].UL[0] / cpuRegs.GPR.r[_Rt_].UL[0]);
		cpuRegs.HI.SD[0] = (s32)(cpuRegs.GPR.r[_Rs_].UL[0] % cpuRegs.GPR.r[_Rt_].UL[0]);
	}
	else
	{
		cpuRegs.LO.SD[0] = -1;
		cpuRegs.HI.SD[0] = cpuRegs.GPR.r[_Rs_].SL[0];
	}
}
void MULT(rr::MmiState& cpuRegs)
{
	s64 res = (s64)cpuRegs.GPR.r[_Rs_].SL[0] * cpuRegs.GPR.r[_Rt_].SL[0];

	// Sign-extend into 64 bits:
	cpuRegs.LO.SD[0] = (s32)(res & 0xffffffff);
	cpuRegs.HI.SD[0] = (s32)(res >> 32);

	if( _Rd_ ) cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[0];
}
void MULTU(rr::MmiState& cpuRegs)
{
	u64 res = (u64)cpuRegs.GPR.r[_Rs_].UL[0] * cpuRegs.GPR.r[_Rt_].UL[0];

	// Note: sign-extend into 64 bits even though it's an unsigned mult.
	cpuRegs.LO.SD[0] = (s32)(res & 0xffffffff);
	cpuRegs.HI.SD[0] = (s32)(res >> 32);

	if( _Rd_ ) cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[0];
}
void LUI(rr::MmiState& cpuRegs) {
	if (!_Rt_) return;
	cpuRegs.GPR.r[_Rt_].UD[0] = (s32)(cpuRegs.code << 16);
}
void MFHI(rr::MmiState& cpuRegs) { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.HI.UD[0]; }
void MFLO(rr::MmiState& cpuRegs) { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.LO.UD[0]; }
void MTHI(rr::MmiState& cpuRegs) { cpuRegs.HI.UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]; }
void MTLO(rr::MmiState& cpuRegs) { cpuRegs.LO.UD[0] = cpuRegs.GPR.r[_Rs_].UD[0]; }
void SRA(rr::MmiState& cpuRegs)   { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].SL[0] >> _Sa_); }
void SRL(rr::MmiState& cpuRegs)   { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].UL[0] >> _Sa_); }
void SLL(rr::MmiState& cpuRegs)   { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].UL[0] << _Sa_); }
void DSLL(rr::MmiState& cpuRegs)  { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (u64)(cpuRegs.GPR.r[_Rt_].UD[0] << _Sa_); }
void DSLL32(rr::MmiState& cpuRegs){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (u64)(cpuRegs.GPR.r[_Rt_].UD[0] << (_Sa_+32));}
void DSRA(rr::MmiState& cpuRegs)  { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = cpuRegs.GPR.r[_Rt_].SD[0] >> _Sa_; }
void DSRA32(rr::MmiState& cpuRegs){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = cpuRegs.GPR.r[_Rt_].SD[0] >> (_Sa_+32);}
void DSRL(rr::MmiState& cpuRegs)  { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rt_].UD[0] >> _Sa_; }
void DSRL32(rr::MmiState& cpuRegs){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rt_].UD[0] >> (_Sa_+32);}
void SLLV(rr::MmiState& cpuRegs) { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].UL[0] << (cpuRegs.GPR.r[_Rs_].UL[0] &0x1f));}
void SRAV(rr::MmiState& cpuRegs) { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].SL[0] >> (cpuRegs.GPR.r[_Rs_].UL[0] &0x1f));}
void SRLV(rr::MmiState& cpuRegs) { if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s32)(cpuRegs.GPR.r[_Rt_].UL[0] >> (cpuRegs.GPR.r[_Rs_].UL[0] &0x1f));}
void DSLLV(rr::MmiState& cpuRegs){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (u64)(cpuRegs.GPR.r[_Rt_].UD[0] << (cpuRegs.GPR.r[_Rs_].UL[0] &0x3f));}
void DSRAV(rr::MmiState& cpuRegs){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].SD[0] = (s64)(cpuRegs.GPR.r[_Rt_].SD[0] >> (cpuRegs.GPR.r[_Rs_].UL[0] &0x3f));}
void DSRLV(rr::MmiState& cpuRegs){ if (!_Rd_) return; cpuRegs.GPR.r[_Rd_].UD[0] = (u64)(cpuRegs.GPR.r[_Rt_].UD[0] >> (cpuRegs.GPR.r[_Rs_].UL[0] &0x3f));}
void MOVZ(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;
	if (cpuRegs.GPR.r[_Rt_].UD[0] == 0) {
		cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0];
	}
}
void MOVN(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;
	if (cpuRegs.GPR.r[_Rt_].UD[0] != 0) {
		cpuRegs.GPR.r[_Rd_].UD[0] = cpuRegs.GPR.r[_Rs_].UD[0];
	}
}
void MFSA(rr::MmiState& cpuRegs) {
	if (!_Rd_) return;
	cpuRegs.GPR.r[_Rd_].UD[0] = (u64)cpuRegs.sa;
}
void MTSA(rr::MmiState& cpuRegs) {
	cpuRegs.sa = (u32)cpuRegs.GPR.r[_Rs_].UD[0];
}
void MTSAB(rr::MmiState& cpuRegs) {
	cpuRegs.sa = ((cpuRegs.GPR.r[_Rs_].UL[0] & 0xF) ^ (_Imm_ & 0xF));
}
void MTSAH(rr::MmiState& cpuRegs) {
	cpuRegs.sa = ((cpuRegs.GPR.r[_Rs_].UL[0] & 0x7) ^ (_Imm_ & 0x7)) << 1;
}
}
static bool supported(uint32_t instruction) {
    switch(instruction>>26) {
    case 0: switch(instruction&63u) {
        case 0: return true;
        case 2: return true;
        case 3: return true;
        case 4: return true;
        case 6: return true;
        case 7: return true;
        case 10: return true;
        case 11: return true;
        case 16: return true;
        case 17: return true;
        case 18: return true;
        case 19: return true;
        case 20: return true;
        case 22: return true;
        case 23: return true;
        case 24: return true;
        case 25: return true;
        case 26: return true;
        case 27: return true;
        case 33: return true;
        case 35: return true;
        case 36: return true;
        case 37: return true;
        case 38: return true;
        case 39: return true;
        case 40: return true;
        case 41: return true;
        case 42: return true;
        case 43: return true;
        case 45: return true;
        case 47: return true;
        case 56: return true;
        case 58: return true;
        case 59: return true;
        case 60: return true;
        case 62: return true;
        case 63: return true;
        default: return false; }
    case 1: switch((instruction>>16)&31u) {
        case 24: return true;
        case 25: return true;
        default: return false; }
    case 9: return true;
    case 10: return true;
    case 11: return true;
    case 12: return true;
    case 13: return true;
    case 14: return true;
    case 15: return true;
    case 25: return true;
    default: return false;
    }
}
bool rr::executeScalar47m(MmiState& cpuRegs,uint32_t instruction) noexcept {
    if(!supported(instruction)) return false;
    cpuRegs.code=instruction;
    switch(instruction>>26) {
    case 0: switch(instruction&63u) {
        case 0: rr_scalar47m::SLL(cpuRegs); return true;
        case 2: rr_scalar47m::SRL(cpuRegs); return true;
        case 3: rr_scalar47m::SRA(cpuRegs); return true;
        case 4: rr_scalar47m::SLLV(cpuRegs); return true;
        case 6: rr_scalar47m::SRLV(cpuRegs); return true;
        case 7: rr_scalar47m::SRAV(cpuRegs); return true;
        case 10: rr_scalar47m::MOVZ(cpuRegs); return true;
        case 11: rr_scalar47m::MOVN(cpuRegs); return true;
        case 16: rr_scalar47m::MFHI(cpuRegs); return true;
        case 17: rr_scalar47m::MTHI(cpuRegs); return true;
        case 18: rr_scalar47m::MFLO(cpuRegs); return true;
        case 19: rr_scalar47m::MTLO(cpuRegs); return true;
        case 20: rr_scalar47m::DSLLV(cpuRegs); return true;
        case 22: rr_scalar47m::DSRLV(cpuRegs); return true;
        case 23: rr_scalar47m::DSRAV(cpuRegs); return true;
        case 24: rr_scalar47m::MULT(cpuRegs); return true;
        case 25: rr_scalar47m::MULTU(cpuRegs); return true;
        case 26: rr_scalar47m::DIV(cpuRegs); return true;
        case 27: rr_scalar47m::DIVU(cpuRegs); return true;
        case 33: rr_scalar47m::ADDU(cpuRegs); return true;
        case 35: rr_scalar47m::SUBU(cpuRegs); return true;
        case 36: rr_scalar47m::AND(cpuRegs); return true;
        case 37: rr_scalar47m::OR(cpuRegs); return true;
        case 38: rr_scalar47m::XOR(cpuRegs); return true;
        case 39: rr_scalar47m::NOR(cpuRegs); return true;
        case 40: rr_scalar47m::MFSA(cpuRegs); return true;
        case 41: rr_scalar47m::MTSA(cpuRegs); return true;
        case 42: rr_scalar47m::SLT(cpuRegs); return true;
        case 43: rr_scalar47m::SLTU(cpuRegs); return true;
        case 45: rr_scalar47m::DADDU(cpuRegs); return true;
        case 47: rr_scalar47m::DSUBU(cpuRegs); return true;
        case 56: rr_scalar47m::DSLL(cpuRegs); return true;
        case 58: rr_scalar47m::DSRL(cpuRegs); return true;
        case 59: rr_scalar47m::DSRA(cpuRegs); return true;
        case 60: rr_scalar47m::DSLL32(cpuRegs); return true;
        case 62: rr_scalar47m::DSRL32(cpuRegs); return true;
        case 63: rr_scalar47m::DSRA32(cpuRegs); return true;
        default: return false; }
    case 1: switch((instruction>>16)&31u) {
        case 24: rr_scalar47m::MTSAB(cpuRegs); return true;
        case 25: rr_scalar47m::MTSAH(cpuRegs); return true;
        default: return false; }
    case 9: rr_scalar47m::ADDIU(cpuRegs); return true;
    case 10: rr_scalar47m::SLTI(cpuRegs); return true;
    case 11: rr_scalar47m::SLTIU(cpuRegs); return true;
    case 12: rr_scalar47m::ANDI(cpuRegs); return true;
    case 13: rr_scalar47m::ORI(cpuRegs); return true;
    case 14: rr_scalar47m::XORI(cpuRegs); return true;
    case 15: rr_scalar47m::LUI(cpuRegs); return true;
    case 25: rr_scalar47m::DADDIU(cpuRegs); return true;
    default: return false;
    }
}
