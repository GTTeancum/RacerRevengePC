// New reconstruction after rollback; source-only, unverified.
#include "original_ui_rider_profile50kg.h"
#include <cmath>
#include <limits>
#include <string>
#include <utility>

namespace rr::platform {
namespace {
using Diagnostic=OriginalUiRiderProfileDiagnostic50KG;
using Error=Diagnostic::Error;
bool fail(Diagnostic& d,Error e,std::uint32_t pc) noexcept {d.error=e;d.sourcePC=pc;return false;}
struct AuthoredRow {
    bool present=false;
    std::array<float,6> base0180{},maximum0408{};
    OriginalUiRiderProfileColors50KG colors{};
};
struct ColorRow {std::uint32_t rider,accent,secondary;};
// Direct constructor17DF80 constants, read through17FA00/17FA10. These are
// original packed RGB words, not a replacement palette or display-normalized RGB.
constexpr ColorRow sourceColors[]={
    {1,0x3674B3,0x7895B3}, //17E938/17E944
    {2,0x994553,0xA27A80}, //17E5F8/17E614
    {3,0xAC5D4E,0xAF8B82}, //17E700/17E718
    {4,0x4E9DAC,0x82A6AF}, //17E658/17E67C
    {7,0x618451,0x7E9077}, //17E7BC/17E7C4
    {8,0x8E966B,0x9CA18B}, //17E800/17E818
    {9,0xAFB236,0xA4A766}, //17E798/17E7A4
    {10,0xB26A36,0xA78366}, //17E59C/17E5BC
    {11,0x9F5748,0x9D786F}, //17E6A0/17E6B0
    {12,0x9A8875,0xA2998E}, //17E554/17E564
    {14,0xB37B4E,0xAC8F76}, //17E95C/17E968
    {15,0x91C67A,0xA4C198}, //17E6CC/17E6E4
    {16,0xB1B36E,0xADAE25}, //17E770/17E780
    {17,0x699B83,0x7F998D}, //17E8F0/17E8FC
    {18,0xA5A45E,0x9E9E79}, //17E914/17E920
    {19,0x67829C,0x7D8B99}, //17E4BC/17E4D8
    {20,0x859C67,0x8D997D}, //17E508/17E524
    {21,0xA5454B,0x966467}, //17E9A4/17E9B0
    {22,0x918B42,0x85825C}, //17E734/17E74C
    {23,0x7DA656,0x84996F}, //17E878/17E8A0
    {24,0xB76F5E,0xA78078}, //17E8CC/17E8D8
    {25,0xB79C5E,0xA79778}, //17E980/17E98C
    {26,0x717171,0x5D5D5D}  //17E9C8/17E9D4
};
constexpr const char* abilityKeys[]={"MaxThrust","TheoreticalTopSpeed","SpoilerYawMoment",
    "Defense","BaseRepairAmount","BaseCoolingTime"};
bool sameChange(const game::OriginalUiProfileChange50KG& a,const game::OriginalUiProfileChange50KG& b) noexcept {
    return a.field==b.field&&a.operation==b.operation&&a.value==b.value&&
        a.replaceMask==b.replaceMask&&a.sourcePC==b.sourcePC;
}
}
struct OriginalUiRiderProfileOwner50KG::Storage {
    std::shared_ptr<OriginalUiMainProfile50KG> common;
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    std::array<AuthoredRow,27> authored{};
    // Only the exact diagonal displayed by1EBBB4/1EBBF0. Source reset copies
    // base[P] to career[C].pod[P]; the UI uses C==P==selected rider.
    std::array<std::array<float,6>,27> current1920{};
    std::array<std::array<std::uint8_t,13>,27> medals1938{};
    std::array<std::uint8_t,27> careerTrack219C{};
    std::array<std::uint32_t,27> unlock10FAC{},hidden0730{};
    std::array<std::uint32_t,4> remembered{};
    std::array<std::uint32_t,7> rememberedCourse{};
    std::uint8_t packed1125D=0,packed1125E=0,packed1125F=0;
    std::uint32_t subtype11258=1;
    std::uint32_t comparison100C0=0;
    bool cardFlag100BC=false;
    std::array<OriginalUiCourseProfileColors50KG,6> planetColors{};
    std::array<std::uint8_t,13> courseOrder100C4{};
    std::uint64_t generation=1;
    bool readable() const noexcept {return common&&common->known&&common->mode34<=2;}
};
OriginalUiRiderProfileOwner50KG::OriginalUiRiderProfileOwner50KG() noexcept=default;
OriginalUiRiderProfileOwner50KG::~OriginalUiRiderProfileOwner50KG()=default;
bool OriginalUiRiderProfileOwner50KG::initializeAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyInitialized,0x17F970);
    if(!common||!common->known)return fail(d,Error::commonProfile,0x17F970);
    if(!archive||!archive->loaded()||!archive->identity()||
       archive->identity()->id!=assets::OriginalUiArchiveId50KG::podPhysics)
        return fail(d,Error::archive,0x1870FC);
    try {
        auto next=std::make_unique<Storage>();next->common=std::move(common);next->archive=std::move(archive);
        //187124..18728C iterates1..26. Missing5/6/13 leave the constructor's
        // initialized slots unchanged; those absent pods have no Rider controls.
        // Here they remain explicitly unavailable instead of publishing stats.
        for(const auto& source:sourceColors){
            const auto rider=source.rider;d.rider=rider;
            const auto id=(rider<10?"0":"")+std::to_string(rider);
            const auto name="pods/pod"+id+"/pod"+id+".phy";
            assets::OriginalUiDat50KG text;
            if(!assets::readOriginalUiDat50KG(*next->archive,name,text,d.parse,
                assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,Error::parse,0x187148);
            const auto* base=text.section("BaseAbilities");const auto* maximum=text.section("MaxAbilities");
            if(!base||!maximum)return fail(d,Error::member,0x187178);
            auto& authored=next->authored[rider];
            for(unsigned lane=0;lane<6;++lane){
                // Full-key admission is deliberately stricter than source missing
                // lookup skip-write. No guessed ability fills an unread source.
                if(!base->find(abilityKeys[lane])||!maximum->find(abilityKeys[lane]))
                    return fail(d,Error::member,0x18718C+lane*20);
                if(!assets::originalUiNumber50KG(*base,abilityKeys[lane],0,authored.base0180[lane],d.parse,0x18718C+lane*20)||
                   !assets::originalUiNumber50KG(*maximum,abilityKeys[lane],0,authored.maximum0408[lane],d.parse,0x187214+lane*20))
                    return fail(d,Error::parse,d.parse.sourcePC);
                // Original pod26 speed is1.75: retain it;188D70 caps the
                // converted notch count, not this authored ability.
                if(!std::isfinite(authored.base0180[lane])||authored.base0180[lane]<0||
                   !std::isfinite(authored.maximum0408[lane])||authored.maximum0408[lane]<0)
                    return fail(d,Error::range,0x18718C+lane*20);
            }
            authored.colors={source.accent,source.secondary};authored.present=true;
            next->current1920[rider]=authored.base0180; //17FFEC..180028, diagonal
        }
        //17FFB0 clears every13-byte medal row and low five track bits. The
        // owner stores only those represented low bits, never claims upper bits.
        for(auto& medals:next->medals1938)medals.fill(0);
        next->careerTrack219C.fill(0);
        //186EF0's exact source roster. This is an application reset, not an
        // unlock-all option nor an inference from which art happens to exist.
        next->unlock10FAC.fill(0);next->hidden0730.fill(0);
        for(const auto rider:{2u,4u,10u,11u,12u,15u,19u,20u})next->unlock10FAC[rider]=1;
        for(const auto rider:{1u,14u,21u,25u,26u})next->hidden0730[rider]=1;
        //186678..186684, after unlock reset. Mode stays in the shared owner.
        next->remembered.fill(19);
        // Original constructor/reset fields, same retained fresh profile.
        next->rememberedCourse={{1,3,8,1,1,8,3}}; //186688..1866A8
        next->packed1125D=0x98;next->packed1125E=0x12;
        next->comparison100C0=0xffffffffu; //17E17C
        next->cardFlag100BC=false; //17E2C0
        next->courseOrder100C4={{0,11,4,8,7,1,5,9,12,6,10,3,2}};
        //17E7D8..17E880 ->17F800..17F884; stride12, primary+79C/secondary+7A0.
        next->planetColors={{{0xc2a684,0x80674a},{0x8cc46e,0x628036},{0xc0b5a5,0x8c8479},
            {0xf4f1a2,0xa8a66f},{0xce766d,0xffbab2},{0xb26a36,0xe5a373}}};
        storage_=std::move(next);d={};return true;
    }catch(...){return fail(d,Error::allocation,0x17F970);}
}
bool OriginalUiRiderProfileOwner50KG::initialized() const noexcept {return bool(storage_);}
const OriginalUiMainProfile50KG* OriginalUiRiderProfileOwner50KG::commonProfile() const noexcept {
    return storage_?storage_->common.get():nullptr;
}
OriginalUiRiderProfileStamp50KG OriginalUiRiderProfileOwner50KG::stamp() const noexcept {
    return storage_?OriginalUiRiderProfileStamp50KG{storage_->generation,storage_->common->mode34}:OriginalUiRiderProfileStamp50KG{};
}
bool OriginalUiRiderProfileOwner50KG::selectionSnapshot(game::OriginalUiRiderSelectionProfile50KG& out) const noexcept {
    if(!storage_||!storage_->readable())return false;
    return selectionSnapshotForMode(storage_->common->mode34,out);
}
bool OriginalUiRiderProfileOwner50KG::selectionSnapshotForMode(std::uint32_t mode,
    game::OriginalUiRiderSelectionProfile50KG& out) const noexcept {
    if(!storage_||!storage_->common||!storage_->common->known||mode>2)return false;const auto& s=*storage_;
    game::OriginalUiRiderSelectionProfile50KG result;result.ownerReady=true;result.mode34=mode;
    result.remembered6F0=s.remembered[0];result.remembered6F4=s.remembered[1];
    result.remembered6F8=s.remembered[2];result.remembered6FC=s.remembered[3];
    for(unsigned rider=0;rider<27;++rider){auto& row=result.riders[rider];
        row.ownerReady=true;row.unlock10FAC=s.unlock10FAC[rider];row.hidden730=s.hidden0730[rider];
        row.careerByteOwnerReady=true;row.career1938=s.medals1938[rider][0];
    }
    out=result;return true;
}
bool OriginalUiRiderProfileOwner50KG::statSnapshot(std::uint32_t rider,float panelY,
    game::OriginalUiRiderStatInput50KG& out) const noexcept {
    if(!storage_||!storage_->readable()||rider>=27||!storage_->authored[rider].present||!std::isfinite(panelY))return false;
    const auto& s=*storage_;game::OriginalUiRiderStatInput50KG result;
    result.profileReady=true;result.selectedRider=rider;result.observedRider=rider;result.mode34=s.common->mode34;
    result.unlocked10FAC=s.unlock10FAC[rider];result.panelY4DC=panelY;
    result.accentColor17FA00=s.authored[rider].colors.accent17FA00;
    result.current1920=s.current1920[rider];result.maximum0408=s.authored[rider].maximum0408;
    result.medal1938=s.medals1938[rider];result.careerTrack219C=s.careerTrack219C[rider];out=result;return true;
}
bool OriginalUiRiderProfileOwner50KG::colors(std::uint32_t rider,OriginalUiRiderProfileColors50KG& out) const noexcept {
    if(!storage_||!storage_->readable()||rider>=27||!storage_->authored[rider].present)return false;
    out=storage_->authored[rider].colors;return true;
}
bool OriginalUiRiderProfileOwner50KG::commitBack(const game::OriginalUiRouteDecision50KG& decision,
    OriginalUiRiderProfileStamp50KG expected) noexcept {
    if(!storage_||!storage_->readable()||expected.generation!=storage_->generation||expected.mode34!=storage_->common->mode34||
       storage_->generation==std::numeric_limits<std::uint64_t>::max()||!decision.admitted||!decision.profileProposalComplete||
       decision.blockers||decision.action!=game::OriginalUiRouteAction50KG::request||decision.request.sourceScreen!=5||
       decision.request.sourcePage<1||decision.request.sourcePage>2||decision.selectedRider<0||decision.selectedRider>=27||
       !storage_->authored[static_cast<unsigned>(decision.selectedRider)].present||decision.profileChanges.count!=1)return false;
    // Reconstruct with the source leaf, admitting only the actual Back target.
    // It creates no destination ownership; caller already admitted the manager.
    game::OriginalUiRiderContext50KG context;context.playerIndex=decision.request.sourcePage-1;
    context.selectedRider=static_cast<unsigned>(decision.selectedRider);context.mode34=storage_->common->mode34;
    context.admission.sourceControllerReady=true;context.admission.selectionOwnerReady=true;context.admission.profileOwnerReady=true;
    context.admission.nativePageMasks[context.playerIndex?5:4]=1u<<1;
    game::OriginalUiRouteDecision50KG exact;
    if(game::dispatchOriginalUiRider50KG(context.playerIndex,game::OriginalUiRouteInput50KG::back,context,exact)!=game::OriginalUiRouteStatus50KG::ok||
       exact.inputPC!=decision.inputPC||exact.selectedSlot!=decision.selectedSlot||
       exact.request.kind!=decision.request.kind||exact.request.targetScreen!=decision.request.targetScreen||
       exact.request.targetPage!=decision.request.targetPage||exact.request.sourcePC!=decision.request.sourcePC||
       !sameChange(exact.profileChanges.values[0],decision.profileChanges.values[0]))return false;
    const unsigned remembered=context.playerIndex?2:(context.mode34==0?0:(context.mode34==1?1:3));
    storage_->remembered[remembered]=context.selectedRider;++storage_->generation;return true;
}
bool OriginalUiRiderProfileOwner50KG::coursePlanetColors(std::uint32_t planet,OriginalUiCourseProfileColors50KG& out)const noexcept{
    if(!storage_||!storage_->readable()||planet>=storage_->planetColors.size())return false;
    out=storage_->planetColors[planet];return true;
}
bool OriginalUiRiderProfileOwner50KG::courseSnapshot(game::OriginalUiCourseRememberedProfile50KG& out)const noexcept{
    if(!storage_||!storage_->readable())return false;const auto& s=*storage_;
    game::OriginalUiCourseRememberedProfile50KG p;p.ownerReady=true;p.mode34=s.common->mode34;
    p.course700=s.rememberedCourse[0];p.laps704=s.rememberedCourse[1];p.entrants708=s.rememberedCourse[2];p.subtype70C=s.rememberedCourse[3];
    p.course710=s.rememberedCourse[4];p.entrants714=s.rememberedCourse[5];p.laps718=s.rememberedCourse[6];
    p.packed1125E=s.packed1125E;p.cardFlag100BC=s.cardFlag100BC;p.availability11018=s.common->availability11018;
    p.planet0Primary79C=s.planetColors[0].primary79C;p.planet0Secondary7A0=s.planetColors[0].secondary7A0;p.courseOrder100C4=s.courseOrder100C4;out=p;return true;
}
bool OriginalUiRiderProfileOwner50KG::riderConfirmSnapshot(std::uint8_t& packed,std::uint32_t& comparison)const noexcept{
    if(!storage_||!storage_->readable())return false;packed=storage_->packed1125D;comparison=storage_->comparison100C0;return true;
}
bool OriginalUiRiderProfileOwner50KG::commitRiderConfirmToCourse(const game::OriginalUiRouteDecision50KG& d,OriginalUiRiderProfileStamp50KG expected)noexcept{
    if(!storage_||!storage_->readable()||storage_->common->mode34!=0||expected.mode34!=0||expected.generation!=storage_->generation||
       storage_->generation==std::numeric_limits<std::uint64_t>::max()||!d.admitted||!d.profileProposalComplete||d.blockers||
       d.action!=game::OriginalUiRouteAction50KG::request||d.request.sourceScreen!=5||d.request.sourcePage!=1||(d.selectedRider!=19&&d.selectedRider!=20)||d.profileChanges.count!=3)return false;
    game::OriginalUiRiderContext50KG c;c.mode34=0;c.selectedRider=d.selectedRider;c.playerIndex=0;c.packed1125C=storage_->common->playerBits1125C;
    c.unlockOwnerReady=true;c.selectedUnlockWord10FAC=storage_->unlock10FAC[d.selectedRider];c.comparisonRider100C0=storage_->comparison100C0;
    c.admission.sourceControllerReady=true;c.admission.selectionOwnerReady=true;c.admission.profileOwnerReady=true;c.admission.nativePageMasks[6]=1u<<1;
    game::OriginalUiRouteDecision50KG exact;
    if(game::dispatchOriginalUiRider50KG(0,game::OriginalUiRouteInput50KG::activate,c,exact)!=game::OriginalUiRouteStatus50KG::ok||
       exact.inputPC!=d.inputPC||exact.selectedSlot!=d.selectedSlot||exact.selectedRider!=d.selectedRider||
       exact.request.kind!=d.request.kind||exact.request.targetScreen!=d.request.targetScreen||exact.request.targetPage!=d.request.targetPage||
       exact.request.sourcePC!=d.request.sourcePC||exact.profileChanges.count!=3)return false;
    for(std::size_t i=0;i<3;++i)if(!sameChange(exact.profileChanges.values[i],d.profileChanges.values[i]))return false;
    storage_->remembered[0]=d.selectedRider;storage_->packed1125D=static_cast<std::uint8_t>((storage_->packed1125D&7u)|(d.selectedRider<<3));
    storage_->comparison100C0=d.selectedRider;++storage_->generation;return true;
}
bool OriginalUiRiderProfileOwner50KG::commitCourseEntry(const game::OriginalUiCourseSelectionData50KG& data,
    const game::OriginalUiCourseEntryPrefix50KG& plan,OriginalUiRiderProfileStamp50KG expected)noexcept{
    if(!storage_||!storage_->readable()||storage_->common->mode34!=0||expected.mode34!=0||expected.generation!=storage_->generation||
       storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
    game::OriginalUiCourseRememberedProfile50KG profile;game::OriginalUiCourseEntryPrefix50KG exact;
    if(!courseSnapshot(profile)||game::planOriginalUiCourseEntryPrefix50KG(data,profile,exact)!=game::OriginalUiCourseSelectionStatus50KG::ok||
       plan.sourcePC!=exact.sourcePC||plan.promptMask42C!=exact.promptMask42C||plan.courseSlot14E4!=exact.courseSlot14E4||
       plan.subtypeSlot14F4!=exact.subtypeSlot14F4||plan.laps14EC!=exact.laps14EC||plan.entrants14F0!=exact.entrants14F0||
       plan.selectedControl.controlId!=exact.selectedControl.controlId||plan.selectedControl.planet40!=exact.selectedControl.planet40||
       plan.selectedControl.course44!=exact.selectedControl.course44||plan.showPopup438!=exact.showPopup438||!sameChange(plan.planetProfileChange,exact.planetProfileChange))return false;
    const auto& change=exact.planetProfileChange;
    storage_->packed1125E=static_cast<std::uint8_t>((storage_->packed1125E&~change.replaceMask)|(change.value&change.replaceMask));++storage_->generation;return true;
}
bool OriginalUiRiderProfileOwner50KG::commitCourseMove(const game::OriginalUiCourseSelectionData50KG& data,
    const game::OriginalUiNavigation50KG& before,bool forward,const game::OriginalUiCourseMove50KG& plan,
    OriginalUiRiderProfileStamp50KG expected)noexcept{
    if(!storage_||!storage_->readable()||storage_->common->mode34!=0||expected.mode34!=0||
       expected.generation!=storage_->generation||storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
    game::OriginalUiCourseRememberedProfile50KG p;game::OriginalUiCourseMove50KG exact;
    if(!courseSnapshot(p)||game::planOriginalUiCourseMove50KG(data,before,p,forward,exact)!=game::OriginalUiCourseSelectionStatus50KG::ok||
       plan.sourcePC!=exact.sourcePC||plan.courseSlot14E4!=exact.courseSlot14E4||plan.deferred14D0!=exact.deferred14D0||
       plan.selectedControl.controlId!=exact.selectedControl.controlId||plan.selectedControl.planet40!=exact.selectedControl.planet40||
       plan.selectedControl.course44!=exact.selectedControl.course44||!sameChange(plan.planetProfileChange,exact.planetProfileChange))return false;
    const auto& change=exact.planetProfileChange;
    storage_->packed1125E=static_cast<std::uint8_t>((storage_->packed1125E&~change.replaceMask)|(change.value&change.replaceMask));
    ++storage_->generation;return true; //Remembered700 changes only at actual Back/Confirm.
}
bool OriginalUiRiderProfileOwner50KG::courseRaceSnapshot(CourseRaceSelection& out)const noexcept{
    if(!storage_||!storage_->readable())return false;const auto& s=*storage_;
    out={std::uint32_t(s.packed1125F&15u),std::uint32_t(s.packed1125E>>5),
        std::uint32_t(s.packed1125D>>3),s.subtype11258,
        std::uint32_t(s.common->playerBits1125C&31u),s.rememberedCourse[2],
        static_cast<std::uint16_t>(s.common->playerBits1125C|(std::uint16_t(s.packed1125D)<<8))};return true;
}
bool OriginalUiRiderProfileOwner50KG::commitCourseLaunch(const game::OriginalUiRouteDecision50KG& d,
    const game::OriginalUiCourseContext50KG& source,OriginalUiRiderProfileStamp50KG expected)noexcept{
    if(!storage_||!storage_->readable()||storage_->common->mode34!=0||expected.mode34!=0||source.mode34!=0||
       expected.generation!=storage_->generation||storage_->generation==std::numeric_limits<std::uint64_t>::max()||
       !d.admitted||!d.profileProposalComplete||d.blockers||d.action!=game::OriginalUiRouteAction50KG::request||
       d.request.sourceScreen!=6||d.request.sourcePage!=1||d.profileChanges.count!=8||
       source.subtypeSlot14F4!=1||source.laps14EC!=3||source.entrants14F0!=8||
       source.selectedControlWord44!=8||source.courseSlot14E4!=4||storage_->packed1125D>>3!=19||
       storage_->packed1125E>>5!=1||((storage_->common->playerBits1125C>>5)&3)!=1)return false;
    auto c=source;c.admission={};c.admission.sourceControllerReady=true;
    c.admission.selectionOwnerReady=true;c.admission.profileOwnerReady=true;
    game::OriginalUiRouteDecision50KG exact;
    if(game::dispatchOriginalUiCourse50KG(0,game::OriginalUiRouteInput50KG::activate,c,exact)!=game::OriginalUiRouteStatus50KG::unavailable||
       exact.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::courseLaunchOwner)||
       exact.inputPC!=d.inputPC||exact.selectedSlot!=d.selectedSlot||exact.request.kind!=d.request.kind||
       exact.request.targetScreen!=d.request.targetScreen||exact.request.targetPage!=d.request.targetPage||
       exact.request.sourcePC!=d.request.sourcePC||exact.profileChanges.count!=8)return false;
    for(std::size_t i=0;i<8;++i)if(!sameChange(exact.profileChanges.values[i],d.profileChanges.values[i]))return false;
    auto& s=*storage_;auto packed=std::uint16_t(s.common->playerBits1125C|(std::uint16_t(s.packed1125D)<<8));
    // Exact source field writes, preserving all unrelated rider and packed bits.
    s.packed1125F=static_cast<std::uint8_t>((s.packed1125F&0xf0u)|(c.selectedControlWord44&15u));
    packed=static_cast<std::uint16_t>((packed&~0x780u)|(7u<<7));
    packed=static_cast<std::uint16_t>((packed&~31u)|c.laps14EC);
    s.common->playerBits1125C=static_cast<std::uint8_t>(packed);s.packed1125D=static_cast<std::uint8_t>(packed>>8);
    s.subtype11258=c.subtypeSlot14F4;s.rememberedCourse[0]=c.courseSlot14E4;s.rememberedCourse[1]=c.laps14EC;
    s.rememberedCourse[2]=c.entrants14F0;s.rememberedCourse[3]=c.subtypeSlot14F4;++s.generation;return true;
}
bool OriginalUiRiderProfileOwner50KG::commitCourseBack(const game::OriginalUiRouteDecision50KG& d,const game::OriginalUiCourseContext50KG& source,
    OriginalUiRiderProfileStamp50KG expected)noexcept{
    if(!storage_||!storage_->readable()||storage_->common->mode34!=0||expected.mode34!=0||source.mode34!=0||
       expected.generation!=storage_->generation||storage_->generation==std::numeric_limits<std::uint64_t>::max()||
       !d.admitted||!d.profileProposalComplete||d.blockers||d.action!=game::OriginalUiRouteAction50KG::request||
       d.request.sourceScreen!=6||d.request.sourcePage!=1||d.profileChanges.count!=4)return false;
    auto c=source;c.admission={};c.admission.sourceControllerReady=true;c.admission.selectionOwnerReady=true;c.admission.profileOwnerReady=true;c.admission.nativePageMasks[5]=1u<<1;
    game::OriginalUiRouteDecision50KG exact;
    if(game::dispatchOriginalUiCourse50KG(0,game::OriginalUiRouteInput50KG::back,c,exact)!=game::OriginalUiRouteStatus50KG::ok||
       exact.inputPC!=d.inputPC||exact.selectedSlot!=d.selectedSlot||exact.selectedRider!=d.selectedRider||
       exact.request.kind!=d.request.kind||exact.request.targetScreen!=d.request.targetScreen||exact.request.targetPage!=d.request.targetPage||
       exact.request.sourcePC!=d.request.sourcePC||exact.profileChanges.count!=4)return false;
    for(std::size_t i=0;i<4;++i)if(!sameChange(exact.profileChanges.values[i],d.profileChanges.values[i]))return false;
    storage_->rememberedCourse[0]=c.courseSlot14E4;storage_->rememberedCourse[1]=c.laps14EC;
    storage_->rememberedCourse[2]=c.entrants14F0;storage_->rememberedCourse[3]=c.subtypeSlot14F4;++storage_->generation;return true;
}

}
