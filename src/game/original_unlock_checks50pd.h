#pragma once
// Native typed form of the original unlock checks 187E30 (the OR of
// 187EE0, 187F90, 188010, 188070, 1880F0, 188170, 1881F0, 1882D0) and their
// grants (187F40, 187FF0, 188050, 1880D0, 188150, 1881D0, 1882B0, 188440).
// Every fact is read from the profile owner; offsets in the comments are the
// original profile (P = App2DC) addresses the checks read. No guest memory.
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
struct OriginalUnlockFacts50PD {
    std::array<std::uint32_t,27> unlock10FAC{};  // U[r]: P+10FAC+4r, nonzero = unlocked
    std::uint32_t word7E8=0;                     // P+7E8, stored by 1AB5A8 (1ABE70)
    std::int32_t rival11280=-1;                  // signed byte P+11280+12*(1125F&15)
    std::uint32_t career100C0=0;                 // P+100C0
    std::uint32_t order187B10=0;                 // 1882D0's own scan, same result as 187B10
    // M[c][c][o]: P+1938+c*0x894+c*0x50+o. Only the diagonal is ever read.
    std::array<std::array<std::uint8_t,13>,27> diagonalMedals{};
    // Hall slot 0 per course: defaults copied once to P+7EC at the end of
    // 1801D0 (memcpy 1858EC); live records at P+102F0. Row stride 0xE4.
    std::array<float,13> defaultLap7EC{},liveLap102F0{};
    std::array<float,13> defaultRace838{},liveRace1033C{};
    std::array<std::uint32_t,13> defaultKnockouts884{},liveKnockouts10388{};
    std::uint8_t availability11018=0;            // gallery tier in bits 5..6
    // PORT EXTENSION, NOT ORIGINAL (DLC career, src\game\dlc_career50dl.h):
    // the SELECTED career is an add-on pod's (its 100C0 holds a stock anchor).
    // The two checks that read the selected career's own diagonal (187F90-class
    // check 3 and the 1882D0 gallery) read these medals instead of
    // M[100C0][100C0]; the fixed-career checks keep reading the stock careers.
    // false: every check reads exactly the original rows.
    bool selectedAddonCareer=false;
    std::array<std::uint8_t,13> selectedAddonMedals{};
};
enum class OriginalUnlockCheck50PD:std::uint8_t {
    rival187EE0,youngAnakin187F90,sebulba188010,clegg188070,
    watto1880F0,maul188170,vader1881F0,gallery1882D0
};
struct OriginalUnlockChecks50PD {
    std::array<bool,8> pending{};
    std::uint32_t galleryTier=0; // 1882D0's return value N when pending[7]
    bool any()const noexcept{for(const bool p:pending)if(p)return true;return false;}
    int first()const noexcept{for(std::size_t i=0;i<pending.size();++i)if(pending[i])return int(i);return -1;}
};

// Fixed pods granted by checks 2..7 (187FF0..1882B0).
inline constexpr std::array<std::int32_t,8> originalUnlockFixedPod50PD{{-1,1,14,3,25,21,26,-1}};

inline bool originalUnlockMedalPodium50PD(std::uint8_t medal)noexcept {return medal>0&&medal<4;}

inline OriginalUnlockChecks50PD evaluateOriginalUnlocks187E30(const OriginalUnlockFacts50PD& f)noexcept {
    OriginalUnlockChecks50PD r;
    const auto U=[&](std::size_t pod)noexcept {return pod<f.unlock10FAC.size()&&f.unlock10FAC[pod]!=0;};
    const auto M=[&](std::size_t career,std::size_t order)noexcept->std::uint8_t {
        return career<f.diagonalMedals.size()&&order<13?f.diagonalMedals[career][order]:0;};
    // The selected career's own medal (M[C][C][o]); an add-on career's own row (DLC).
    const auto S=[&](std::size_t order)noexcept->std::uint8_t {
        if(f.selectedAddonCareer)return order<13?f.selectedAddonMedals[order]:0;
        return M(f.career100C0,order);};
    // 187EE0: U[R]==0 and 7E8 != 0. R outside 0..26 would index another
    // profile word in the original; no authored course has such a rival.
    r.pending[0]=f.rival11280>=0&&f.rival11280<27&&!U(std::size_t(f.rival11280))&&f.word7E8!=0;
    // 187F90 / 1880F0: every default (7EC / 838) strictly slower than live.
    bool lap=true,race=true,knockouts=true;
    for(std::size_t i=0;i<13;++i){
        if(!(f.defaultLap7EC[i]>f.liveLap102F0[i]))lap=false;          // c.ole.s exits
        if(!(f.defaultRace838[i]>f.liveRace1033C[i]))race=false;
        if(!(f.defaultKnockouts884[i]<f.liveKnockouts10388[i]))knockouts=false; // sltu
    }
    r.pending[1]=!U(1)&&lap;
    r.pending[2]=!U(14)&&originalUnlockMedalPodium50PD(M(18,12));        // byte B94C
    r.pending[3]=!U(3)&&f.career100C0<27&&S(3)==1;                         // +193B
    r.pending[4]=!U(25)&&race;
    r.pending[5]=!U(21)&&knockouts;
    r.pending[6]=!U(26)&&U(1)&&U(25)&&U(21)&&
        (originalUnlockMedalPodium50PD(M(1,12))||originalUnlockMedalPodium50PD(M(25,12))||
         originalUnlockMedalPodium50PD(M(21,12)));                         // 2228/F788/D3F8
    // 1882D0: course order+1 decides the circuit; only its finale counts.
    const auto next=f.order187B10+1u;
    const bool finale=next==4u||next==8u||next==13u;
    if(finale&&f.career100C0<27){
        std::uint32_t wins=0;
        for(const std::size_t o:{std::size_t(3),std::size_t(7),std::size_t(12)})if(S(o)==1)++wins;
        const std::uint32_t tier=(f.availability11018>>5)&3u;
        if(tier<wins){r.pending[7]=true;r.galleryTier=wins;}
    }
    return r;
}
// Pod a check grants, or -1 for the gallery check.
inline std::int32_t originalUnlockPod50PD(const OriginalUnlockFacts50PD& f,std::size_t check)noexcept {
    if(check==0)return f.rival11280;
    return check<originalUnlockFixedPod50PD.size()?originalUnlockFixedPod50PD[check]:-1;
}
}
