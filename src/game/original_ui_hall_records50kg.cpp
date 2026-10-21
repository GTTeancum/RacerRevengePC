#include "original_ui_hall_records50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
namespace rr::game {
namespace {
// Authenticated SLUS_202.68 SHA256
// c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30.
// Source1801D0 calls rank1/2/3 direct-write branches of185910/185BC0/185EC0.
// Bits are retained exactly, not reconstructed from rounded decimal displays.
struct Seed {std::uint8_t course,category,rank;std::uint32_t bits,textID,callPC,nameCtorPC;};
constexpr Seed seeds[]={
    {0,0,0,0x423f51ecu,192,0x18030c,0x17e4a0},
    {0,0,1,0x4250a3d7u,193,0x1803cc,0x17e4f0},
    {0,0,2,0x426e0a3du,178,0x18048c,0x17e5dc},
    {0,1,0,0x432b2148u,192,0x18054c,0x17e4a0},
    {0,1,1,0x43328a3du,178,0x18060c,0x17e5dc},
    {0,1,2,0x4336a666u,186,0x1806cc,0x17e544},
    {0,2,0,0x5u,191,0x180784,0x17e908},
    {0,2,1,0x3u,184,0x18083c,0x17e584},
    {0,2,2,0x2u,185,0x1808f4,0x17e690},
    {1,0,0,0x4286a8f6u,181,0x1809b4,0x17e7b4},
    {1,0,1,0x4293d70au,189,0x180a74,0x17e75c},
    {1,0,2,0x429a6148u,184,0x180b34,0x17e584},
    {1,1,0,0x435c07aeu,191,0x180bf4,0x17e908},
    {1,1,1,0x4363947bu,184,0x180cb4,0x17e584},
    {1,1,2,0x4368f0a4u,183,0x180d74,0x17e78c},
    {1,2,0,0x5u,191,0x180e2c,0x17e908},
    {1,2,1,0x4u,184,0x180ee4,0x17e584},
    {1,2,2,0x2u,196,0x180f9c,0x17e848},
    {2,0,0,0x42bef5c3u,191,0x18105c,0x17e908},
    {2,0,1,0x42d1a8f6u,192,0x18111c,0x17e4a0},
    {2,0,2,0x42de7ae1u,197,0x1811dc,0x17e8c0},
    {2,1,0,0x439b2f5cu,190,0x18129c,0x17e8e4},
    {2,1,1,0x439ee8f6u,192,0x18135c,0x17e4a0},
    {2,1,2,0x43a3170au,191,0x18141c,0x17e908},
    {2,2,0,0x3u,191,0x1814d4,0x17e908},
    {2,2,1,0x2u,190,0x18158c,0x17e8e4},
    {2,2,2,0x1u,196,0x181644,0x17e848},
    {4,0,0,0x4286b333u,193,0x181704,0x17e4f0},
    {4,0,1,0x428a23d7u,193,0x1817c4,0x17e4f0},
    {4,0,2,0x4294947bu,178,0x181884,0x17e5dc},
    {4,1,0,0x435b147bu,193,0x181944,0x17e4f0},
    {4,1,1,0x43607852u,192,0x181a04,0x17e4a0},
    {4,1,2,0x4365e666u,178,0x181ac4,0x17e5dc},
    {4,2,0,0x4u,180,0x181b7c,0x17e640},
    {4,2,1,0x2u,188,0x181c34,0x17e6bc},
    {4,2,2,0x2u,185,0x181cec,0x17e690},
    {5,0,0,0x428bb333u,179,0x181dac,0x17e6f0},
    {5,0,1,0x42946666u,181,0x181e6c,0x17e7b4},
    {5,0,2,0x429a0f5cu,189,0x181f2c,0x17e75c},
    {5,1,0,0x435cee14u,179,0x181fec,0x17e6f0},
    {5,1,1,0x436470a4u,192,0x1820ac,0x17e4a0},
    {5,1,2,0x4369bae1u,196,0x18216c,0x17e848},
    {5,2,0,0x4u,184,0x182224,0x17e584},
    {5,2,1,0x3u,181,0x1822dc,0x17e7b4},
    {5,2,2,0x1u,195,0x182394,0x17e724},
    {6,0,0,0x42a1428fu,190,0x182454,0x17e8e4},
    {6,0,1,0x42aa8f5cu,192,0x182514,0x17e4a0},
    {6,0,2,0x42b3dc29u,191,0x1825d4,0x17e908},
    {6,1,0,0x437f07aeu,197,0x182694,0x17e8c0},
    {6,1,1,0x43836a3du,192,0x182754,0x17e4a0},
    {6,1,2,0x43872b85u,197,0x182814,0x17e8c0},
    {6,2,0,0x3u,190,0x1828cc,0x17e8e4},
    {6,2,1,0x2u,192,0x182984,0x17e4a0},
    {6,2,2,0x1u,195,0x182a3c,0x17e724},
    {7,0,0,0x4252999au,195,0x182afc,0x17e724},
    {7,0,1,0x4268a3d7u,196,0x182bbc,0x17e848},
    {7,0,2,0x42773d71u,179,0x182c7c,0x17e6f0},
    {7,1,0,0x43336148u,195,0x182d3c,0x17e724},
    {7,1,1,0x433ce8f6u,184,0x182dfc,0x17e584},
    {7,1,2,0x43404a3du,183,0x182ebc,0x17e78c},
    {7,2,0,0x4u,189,0x182f74,0x17e75c},
    {7,2,1,0x3u,184,0x18302c,0x17e584},
    {7,2,2,0x2u,183,0x1830e4,0x17e78c},
    {3,0,0,0x428c851fu,191,0x1831a4,0x17e908},
    {3,0,1,0x429ac28fu,195,0x183264,0x17e724},
    {3,0,2,0x42a40f5cu,190,0x183324,0x17e8e4},
    {3,1,0,0x435df333u,192,0x1833e4,0x17e4a0},
    {3,1,1,0x4363c7aeu,195,0x1834a4,0x17e724},
    {3,1,2,0x436a51ecu,197,0x183564,0x17e8c0},
    {3,2,0,0x3u,191,0x18361c,0x17e908},
    {3,2,1,0x2u,184,0x1836d4,0x17e584},
    {3,2,2,0x1u,184,0x18378c,0x17e584},
    {8,0,0,0x423128f6u,188,0x18384c,0x17e6bc},
    {8,0,1,0x424aeb85u,184,0x18390c,0x17e584},
    {8,0,2,0x4259cccdu,193,0x1839cc,0x17e4f0},
    {8,1,0,0x430bd70au,184,0x183a8c,0x17e584},
    {8,1,1,0x4311ee14u,180,0x183b4c,0x17e640},
    {8,1,2,0x431628f6u,188,0x183c0c,0x17e6bc},
    {8,2,0,0x5u,184,0x183cc4,0x17e584},
    {8,2,1,0x3u,192,0x183d7c,0x17e4a0},
    {8,2,2,0x2u,180,0x183e34,0x17e640},
    {9,0,0,0x42922e14u,179,0x183ef4,0x17e6f0},
    {9,0,1,0x429ba3d7u,192,0x183fb4,0x17e4a0},
    {9,0,2,0x42a0ae14u,196,0x184074,0x17e848},
    {9,1,0,0x436b7ae1u,179,0x184134,0x17e6f0},
    {9,1,1,0x43734a3du,181,0x1841f4,0x17e7b4},
    {9,1,2,0x437a6e14u,184,0x1842b4,0x17e584},
    {9,2,0,0x5u,196,0x18436c,0x17e848},
    {9,2,1,0x3u,189,0x184424,0x17e75c},
    {9,2,2,0x2u,195,0x1844dc,0x17e724},
    {10,0,0,0x42a73852u,190,0x18459c,0x17e8e4},
    {10,0,1,0x42b3b333u,195,0x18465c,0x17e724},
    {10,0,2,0x42bcc28fu,192,0x18471c,0x17e4a0},
    {10,1,0,0x43888148u,190,0x1847dc,0x17e8e4},
    {10,1,1,0x438c50a4u,184,0x18489c,0x17e584},
    {10,1,2,0x43906b85u,182,0x18495c,0x17e7d4},
    {10,2,0,0x3u,191,0x184a14,0x17e908},
    {10,2,1,0x2u,184,0x184acc,0x17e584},
    {10,2,2,0x1u,192,0x184b84,0x17e4a0},
    {11,0,0,0x42897ae1u,178,0x184c44,0x17e5dc},
    {11,0,1,0x4293a8f6u,186,0x184d04,0x17e544},
    {11,0,2,0x429a0f5cu,188,0x184dc4,0x17e6bc},
    {11,1,0,0x4361deb8u,186,0x184e84,0x17e544},
    {11,1,1,0x43694ccdu,184,0x184f44,0x17e584},
    {11,1,2,0x436dbd71u,193,0x185004,0x17e4f0},
    {11,2,0,0x4u,180,0x1850bc,0x17e640},
    {11,2,1,0x3u,186,0x185174,0x17e544},
    {11,2,2,0x2u,192,0x18522c,0x17e4a0},
    {12,0,0,0x4289f0a4u,197,0x1852ec,0x17e8c0},
    {12,0,1,0x429747aeu,192,0x1853ac,0x17e4a0},
    {12,0,2,0x42a25c29u,191,0x18546c,0x17e908},
    {12,1,0,0x4364bd71u,197,0x18552c,0x17e8c0},
    {12,1,1,0x436b30a4u,192,0x1855ec,0x17e4a0},
    {12,1,2,0x4373d47bu,182,0x1856ac,0x17e7d4},
    {12,2,0,0x4u,190,0x185764,0x17e8e4},
    {12,2,1,0x2u,191,0x18581c,0x17e908},
    {12,2,2,0x1u,182,0x1858d4,0x17e7d4},
};
static_assert(sizeof(seeds)/sizeof(seeds[0])==117,"13 courses, 3 categories, 3 ranks");
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559,"binary32 source records");
bool fail(OriginalUiHallRecordsDiagnostic50KG& d,OriginalUiHallRecordsDiagnostic50KG::Error e,
    std::uint32_t pc,std::uint32_t id=0) noexcept {d={e,pc,id};return false;}
bool authenticated(const assets::OriginalUiEnglish50KG& english) noexcept {
    using Id=assets::OriginalUiArchiveId50KG;
    const auto* archive=assets::originalUiArchiveIdentity50KG(Id::localization);
    if(!archive||!english.loaded())return false;
    bool names=false,text=false;
    for(std::size_t i=0;i<archive->memberCount;++i){
        const auto* member=assets::originalUiMemberIdentity50KG(Id::localization,i);
        if(member==english.indexIdentity()&&!std::strcmp(member->name,"loc\\english\\UI.loc"))names=true;
        if(member==english.textIdentity()&&!std::strcmp(member->name,"loc\\english\\L_UI_ENG.bin"))text=true;
    }
    return names&&text;
}
}
bool initializeOriginalUiHallRecords50KG(const assets::OriginalUiEnglish50KG& english,
    OriginalUiHallRecords50KG& out,OriginalUiHallRecordsDiagnostic50KG& d) noexcept {
    d={};using E=OriginalUiHallRecordsDiagnostic50KG::Error;
    if(!authenticated(english))return fail(d,E::localization,0x180254);
    OriginalUiHallRecords50KG next;
    for(const auto& seed:seeds){
        const auto* name=english.at(seed.textID);
        if(!name)return fail(d,E::name,seed.callPC,seed.textID);
        auto& row=next.courses[seed.course];
        auto& names=seed.category==0?row.lapNames102FC:seed.category==1?row.raceNames10348:row.knockoutNames10394;
        //309980(strncpy) copies at most20 bytes; the source writes byte20=0.
        auto& target=names[seed.rank];
        std::memcpy(target.data(),name->text.data(),std::min<std::size_t>(name->text.size(),20));
        target[20]=0;
        if(seed.category==2)row.knockouts10388[seed.rank]=seed.bits;
        else {float seconds;std::memcpy(&seconds,&seed.bits,sizeof(seconds));
            if(!std::isfinite(seconds)||seconds<=0)return fail(d,E::numeric,seed.callPC);
            (seed.category==0?row.lap102F0:row.race1033C)[seed.rank]=seconds;}
    }
    next.namesSource=english.indexIdentity();next.textSource=english.textIdentity();out=next;return true;
}
bool originalUiHallTime18900050KG(float seconds,std::array<char,9>& out) noexcept {
    if(!std::isfinite(seconds))return false;
    if(seconds<=0||seconds>=21474836.f){std::memcpy(out.data(),"--:--.--",9);return true;}
    // Preserve source separate binary32 rounding even under host contraction.
    const volatile float scaled=seconds*100.f;
    const volatile float rounded=.5f+scaled;
    if(!std::isfinite(rounded)||rounded>=2147483648.f)return false;
    const auto ticks=static_cast<std::int32_t>(rounded);
    const auto minutes=std::min((ticks/100)/60,99),second=(ticks/100)%60,hundredths=ticks%100;
    out={{char('0'+minutes/10),char('0'+minutes%10),':',char('0'+second/10),
        char('0'+second%10),'.',char('0'+hundredths/10),char('0'+hundredths%10),0}};
    return true;
}
}
