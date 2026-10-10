// Adaptation glue; scalar equivalent of PCSX2 CountLeadingSignBits.
#pragma once
#include "mmi.h"
using u8=uint8_t; using s8=int8_t; using u16=uint16_t; using s16=int16_t;
using u32=uint32_t; using s32=int32_t; using u64=uint64_t; using s64=int64_t;
using GPR_reg=rr::MmiRegister;
#define __fi inline
#define _Rs_ ((cpuRegs.code >> 21) & 31u)
#define _Rt_ ((cpuRegs.code >> 16) & 31u)
#define _Rd_ ((cpuRegs.code >> 11) & 31u)
#define _Sa_ ((cpuRegs.code >> 6) & 31u)
namespace Common {
inline u32 CountLeadingSignBits(s32 n) {
    u32 word=static_cast<u32>(n); if(n<0) word=~word;
    u32 count=0; for(u32 mask=0x80000000u; mask && !(word&mask); mask>>=1) ++count;
    return count;
}
}
