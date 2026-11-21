#include "dlc_career50dl.h"
// PORT EXTENSION, NOT ORIGINAL BEHAVIOUR: see dlc_career50dl.h.
#include "../assets/dlc_progress50dl.h"
#include "../assets/dlc_registry50dl.h"
#include <cmath>
#include <cstdio>

namespace rr::game::dlc50DL {
namespace reg=rr::assets::dlc50DL;
namespace {
// Host memory only (profile-owner thread): the latched career and the stock
// constructor tables a fresh career is seeded from. Everything that persists
// lives in the DLC side save (rrpc-dlc-progress/3: scalars, own row, ai[27],
// name, boundary), written only through DlcProgress50DL::commitCareer.
struct State {
    bool latched=false;
    AddonCareer50DL career{};
    bool tournamentLaunch=false;
    bool stockBound=false;                                // bindStockCareerTables
    std::array<std::array<float,6>,27> stockBase{};
    std::array<std::int16_t,13> stockPrizes{};
};
State& state() noexcept {static State s;return s;}

bool finiteNonNegative(float f) noexcept {return std::isfinite(f)&&f>=0.f;}
bool validName(std::string_view name) noexcept {
    if(name.size()>addonCareerNameChars)return false;
    for(const char c:name)if(static_cast<unsigned char>(c)<0x20u||static_cast<unsigned char>(c)>0x7Eu)return false;
    return true;
}
// The rows 1800E0 gives a fresh career: abilities = base0180, medals 0,
// remaining prizes = the constructor halves.
bool seedAi(const std::array<std::array<float,6>,27>& base0180,const std::array<std::int16_t,13>& prizes,
    std::array<OriginalTournamentPodCareer50KG,27>& out) noexcept {
    std::array<OriginalTournamentPodCareer50KG,27> next{};
    for(std::size_t pod=0;pod<next.size();++pod){
        for(std::size_t lane=0;lane<6;++lane){
            if(!finiteNonNegative(base0180[pod][lane]))return false;
            next[pod].abilities1920[lane]=base0180[pod][lane];
        }
        next[pod].medals1938.fill(0);next[pod].remainingPrizes1946=prizes;
    }
    out=next;return true;
}
reg::ProgressCareer progressOf(const AddonCareer50DL& c) {
    reg::ProgressCareer p;
    p.earned2190=c.career.earned2190;p.unspent2194=c.career.unspent2194;p.kills2198=c.career.kills2198;
    p.progress219C=c.career.progress219C;p.pod=c.own;
    p.aiStored=true;for(std::size_t i=0;i<27;++i)p.ai[i]=c.career.pods[i];
    p.name=c.name;p.boundary102CC=c.boundary102CC;return p;
}
// The own row stays inside the pod's own PHY range (the Garage rule).
bool ownRowFits(const AddonCareer50DL& c,const std::array<float,6>& abilities) noexcept {
    for(std::size_t lane=0;lane<6;++lane){
        const float v=abilities[lane],hi=c.maximum[lane];
        if(!finiteNonNegative(v)||!(v<=hi+hi*1e-6f+1e-6f))return false;
    }
    return true;
}
// Persist `next` (all or nothing) and publish it as the latched career.
bool commit(State& s,const AddonCareer50DL& next,const char* what) noexcept {
    try{
        const auto p=progressOf(next);
        if(!reg::validCareer(p)||!reg::progressOwner().commitCareer(next.key,p))return false;
        s.career=next;return true;
    }catch(...){(void)what;return false;}
}
void line(const char* what,const AddonCareer50DL& c,const char* status,const char* reason=nullptr) noexcept {
    std::fprintf(stderr,"dlc_career %s key=%s number=%u anchor=%u progress=%u unspent=%u status=%s%s%s%s\n",what,
        c.key.c_str(),c.number,c.anchor,unsigned(c.career.progress219C&31u),unsigned(c.career.unspent2194),status,
        reason?" reason=\"":"",reason?reason:"",reason?"\"":"");
}
} // namespace

bool latchTournamentCareer(unsigned anchorRider,unsigned addonNumber,
    const std::array<std::array<float,6>,27>& base0180,
    const std::array<std::int16_t,13>& constructorPrizes) noexcept {
    auto& s=state();
    const auto* pod=reg::addonPod(addonNumber);
    AddonCareer50DL refused;refused.number=addonNumber;refused.anchor=anchorRider;
    try{refused.key=pod?pod->key:std::string();}catch(...){}
    if(!pod||anchorRider<1u||anchorRider>26u){line("latch",refused,"refused","not an admitted add-on or no stock anchor");return false;}
    try{
        reg::ProgressCareer stored;
        if(!reg::progressOwner().careerSnapshot(pod->key,stored)){line("latch",refused,"refused","no side-save career");return false;}
        AddonCareer50DL next;next.number=addonNumber;next.anchor=anchorRider;next.key=pod->key;
        next.career.earned2190=stored.earned2190;next.career.unspent2194=stored.unspent2194;
        next.career.kills2198=stored.kills2198;next.career.progress219C=stored.progress219C;next.own=stored.pod;
        for(std::size_t lane=0;lane<6;++lane){next.base[lane]=pod->baseAbilities[lane];next.maximum[lane]=pod->maxAbilities[lane];}
        if(stored.aiStored){for(std::size_t p=0;p<27;++p)next.career.pods[p]=stored.ai[p];}
        else{
            // A /2 entry or a fresh career: the AI rows 1800E0 gives a fresh career.
            std::array<OriginalTournamentPodCareer50KG,27> ai{};
            if(!seedAi(base0180,constructorPrizes,ai)){line("latch",refused,"refused","stock base0180 not finite");return false;}
            for(std::size_t p=0;p<27;++p)next.career.pods[p]=ai[p];
        }
        next.name=stored.name;next.boundary102CC=stored.boundary102CC;
        s.career=next;s.latched=true;s.tournamentLaunch=false;
        line("latch",s.career,stored.aiStored?"latched":"latched_seeded_ai");return true;
    }catch(...){line("latch",refused,"refused","allocation");return false;}
}
bool latchTournamentCareer(unsigned anchorRider,unsigned addonNumber) noexcept {
    const auto& s=state();
    if(!s.stockBound){AddonCareer50DL r;r.number=addonNumber;r.anchor=anchorRider;line("latch",r,"refused","stock tables not bound");return false;}
    const auto base=s.stockBase;const auto prizes=s.stockPrizes;
    return latchTournamentCareer(anchorRider,addonNumber,base,prizes);
}
void clearTournamentCareer() noexcept {
    auto& s=state();
    if(s.latched)line("clear",s.career,"cleared");
    s.latched=false;s.career=AddonCareer50DL{};s.tournamentLaunch=false;
}
const AddonCareer50DL* activeAddonCareer(unsigned comparison100C0) noexcept {
    const auto& s=state();
    if(!s.latched||s.career.anchor!=comparison100C0||!reg::addonPod(s.career.number))return nullptr;
    return &s.career;
}
void noteCourseLaunch(bool tournament) noexcept {state().tournamentLaunch=tournament;}
unsigned tournamentRaceAddon(unsigned profileRider) noexcept {
    const auto& s=state();
    return s.tournamentLaunch&&activeAddonCareer(profileRider)?s.career.number:0u;
}

bool commitAddonGarage(unsigned comparison100C0,const std::array<float,6>& abilities,
    std::uint32_t unspent2194) noexcept {
    auto& s=state();
    if(!activeAddonCareer(comparison100C0))return false;
    if(!ownRowFits(s.career,abilities)){line("garage",s.career,"refused","ability outside the pod's PHY range");return false;}
    auto next=s.career;next.own.abilities1920=abilities;next.career.unspent2194=unspent2194;
    if(!commit(s,next,"garage")){line("garage",s.career,"refused","side save refused the career");return false;}
    line("garage",s.career,"committed");return true;
}
bool commitAddonResult(unsigned comparison100C0,std::uint8_t selectedTrack,
    const OriginalTournamentResultRacer50KG* rows,std::size_t count,std::uint32_t humans,
    const std::array<std::array<float,6>,27>& base0180,const std::array<std::array<float,6>,27>& maximum0408,
    std::uint8_t availability11018,std::uint32_t& sharedRandom,
    OriginalTournamentResultReceipt50KG& receipt) noexcept {
    receipt={};auto& s=state();
    if(!activeAddonCareer(comparison100C0))return false;
    auto next=s.career;auto rng=sharedRandom;auto available=availability11018; // merged read value; output re-derived
    receipt=applyAddonTournamentResult50DL(next.career,next.own,next.number,base0180,maximum0408,
        selectedTrack,rows,count,humans,available,next.boundary102CC,rng);
    if(receipt.status!=OriginalTournamentResultStatus50KG::complete){line("result",s.career,"refused","1875C0 receipt incomplete");return false;}
    if(!commit(s,next,"result")){line("result",s.career,"refused","side save refused the career");return false;}
    sharedRandom=rng;
    std::fprintf(stderr,"dlc_career result key=%s order=%u prize=%u kill_bonus=%u advanced=%d availability_rule=%d draws=%u stock_save=untouched\n",
        s.career.key.c_str(),receipt.courseOrder,receipt.prize,receipt.killBonus,int(receipt.careerAdvanced),
        int(receipt.availabilityAdvanced),receipt.randomDraws);
    return true;
}
bool storeAddonCareerName(unsigned comparison100C0,std::string_view name) noexcept {
    auto& s=state();
    if(!activeAddonCareer(comparison100C0)||!validName(name))return false;
    try{
        auto next=s.career;next.name=std::string(name);
        if(!commit(s,next,"name")){line("name",s.career,"refused","side save refused the career");return false;}
        return true;
    }catch(...){return false;}
}
void bindStockCareerTables(const std::array<std::array<float,6>,27>& base0180,
    const std::array<std::int16_t,13>& constructorPrizes) noexcept {
    auto& s=state();s.stockBase=base0180;s.stockPrizes=constructorPrizes;s.stockBound=true;
}
bool addonCareerView(unsigned addonNumber,AddonCareerView50DL& out) noexcept {
    const auto& s=state();const auto* pod=reg::addonPod(addonNumber);
    if(!pod)return false;
    AddonCareerView50DL v;
    if(s.latched&&s.career.number==addonNumber){
        v.own=s.career.own;v.earned2190=s.career.career.earned2190;v.unspent2194=s.career.career.unspent2194;
        v.kills2198=s.career.career.kills2198;v.progress219C=s.career.career.progress219C;out=v;return true;
    }
    try{
        reg::ProgressCareer stored;
        if(!reg::progressOwner().careerSnapshot(pod->key,stored))return false;
        v.own=stored.pod;v.earned2190=stored.earned2190;v.unspent2194=stored.unspent2194;
        v.kills2198=stored.kills2198;v.progress219C=stored.progress219C;out=v;return true;
    }catch(...){return false;}
}
bool canResetAddonCareer(unsigned addonNumber) noexcept {
    AddonCareerView50DL v;
    return addonCareerView(addonNumber,v)&&v.own.medals1938[0]!=0; // 1EAFB0: career diagonal medal 0
}
bool resetAddonCareer(unsigned addonNumber) noexcept {
    auto& s=state();const auto* pod=reg::addonPod(addonNumber);
    if(!pod||!s.stockBound||!canResetAddonCareer(addonNumber))return false;
    try{
        // 1800E0 on the add-on career: own row from the pod's own base, AI rows
        // from base0180, medals 0, remaining prizes = constructor halves,
        // scalars 0 (progress keeps its opaque high bits 5..7), name empty.
        AddonCareerView50DL before;if(!addonCareerView(addonNumber,before))return false;
        reg::ProgressCareer p=reg::seededCareer(pod->baseAbilities);
        p.pod.remainingPrizes1946=s.stockPrizes;p.progress219C=static_cast<std::uint8_t>(before.progress219C&0xc0u);
        if(!seedAi(s.stockBase,s.stockPrizes,p.ai))return false;
        p.aiStored=true;
        if(!reg::validCareer(p)||!reg::progressOwner().commitCareer(pod->key,p))return false;
        if(s.latched&&s.career.number==addonNumber){
            s.career.career=OriginalTournamentCareer50KG{};
            for(std::size_t i=0;i<27;++i)s.career.career.pods[i]=p.ai[i];
            s.career.career.progress219C=p.progress219C;
            s.career.own=p.pod;s.career.name.clear();s.career.boundary102CC=0;
        }
        AddonCareer50DL shown;shown.number=addonNumber;shown.key=pod->key;line("reset",shown,"reset");
        return true;
    }catch(...){return false;}
}
std::uint8_t addonCareerAvailability() noexcept {
    try{
        const auto& s=state();unsigned best=0;
        const auto& owner=reg::progressOwner();
        for(const auto& [key,c]:owner.state().careers){
            if(!owner.podUnlocked(key))continue; // removed package: not shown
            unsigned progress=c.progress219C&31u;
            if(s.latched&&s.career.key==key)progress=s.career.career.progress219C&31u;
            const unsigned a=progress+1u>13u?13u:progress+1u;
            if(progress>0u&&a>best)best=a; // a career that never advanced earns nothing beyond the stock start
        }
        return static_cast<std::uint8_t>(best);
    }catch(...){return 0;}
}

} // namespace rr::game::dlc50DL
