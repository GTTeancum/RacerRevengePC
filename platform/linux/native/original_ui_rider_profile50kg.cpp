// New reconstruction after rollback; source-only, unverified.
#include "original_ui_rider_profile50kg.h"
#include <cstdio>
#include "../../../src/game/original_course_descriptor50kg.h"
#include "original_save_profile_owner50kg.h"
#include "original_save_native_read_stage50kg.h"
#include "original_ui_main_audio50kg.h"
#include "original_sound_mode50kg.h"
#include "../../../src/game/original_ui_garage_state50kg.h"
#include "../../../src/game/original_ui_controls_controller50kg.h"
#include "../../../src/game/original_course_load_selection50kg.h"
#include "../../../src/game/original_hall_record_queries50kg.h"
#include "../../../src/assets/dlc_progress50dl.h"
#include "../../../src/assets/dlc_registry50dl.h"
#include "../../../src/game/dlc_rider_entries50dl.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <type_traits>
#include <cstring>

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
// Actual profile constructor17E434..17E9BC words at3C+4*rider.
// Missing0/5/6/13 retain the source -1, not invented localized names.
constexpr std::array<std::int32_t,27> sourceRacerNames3C{{
    -1,177,178,179,180,-1,-1,181,182,183,184,185,186,-1,
    187,188,189,190,191,192,193,194,195,196,197,198,199}};
struct TrackTextRow {
    std::int32_t title1127C=-1;
    std::int8_t planet11281=-1;
    std::int8_t rival11280=-1;
    std::array<char,3> name11278{}; //Original two bytes plus copied terminator.
};
// Direct constructor stores, in actual track-ID order (stride12). Planet
// bytes are localization IDs consumed by signed LB1EF3F0, not planet indices.
constexpr std::array<TrackTextRow,13> sourceTrackText{{
    {106,11}, //17EA08/17EA30
    {111,11}, //17EBF8/17EC20
    {115,11}, //17ED24/17ED4C
    {117,12}, //17ED88/17EDB0
    {110,13}, //17EAD0/17EAF4
    {114,13}, //17EC5C/17EC84
    {118,13}, //17EDEC/17EE14
    {108,12}, //17EA6C/17EA94
    {107,15}, //17EB30/17EB58
    {112,15}, //17ECC0/17ECE8
    {116,15}, //17EE50/17EE78
    {109,14}, //17EB94/17EBBC
    {119,14}  //17EEB4/17EEDC
}};
constexpr const char* abilityKeys[]={"MaxThrust","TheoreticalTopSpeed","SpoilerYawMoment",
    "Defense","BaseRepairAmount","BaseCoolingTime"};
bool sameChange(const game::OriginalUiProfileChange50KG& a,const game::OriginalUiProfileChange50KG& b) noexcept {
    return a.field==b.field&&a.operation==b.operation&&a.value==b.value&&
        a.replaceMask==b.replaceMask&&a.sourcePC==b.sourcePC;
}
bool sameCourseRace(const OriginalUiRiderProfileOwner50KG::CourseRaceSelection& a,
    const OriginalUiRiderProfileOwner50KG::CourseRaceSelection& b) noexcept {
    return a.course==b.course&&a.planet==b.planet&&a.rider==b.rider&&a.subtype==b.subtype&&
        a.laps==b.laps&&a.entrants==b.entrants&&a.packed1125C==b.packed1125C&&a.mode34==b.mode34;
}
}
struct OriginalUiRiderProfileOwner50KG::Storage {
    std::shared_ptr<OriginalUiMainProfile50KG> common;
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    std::array<AuthoredRow,27> authored{};
    // Same sole career bank used by Rider projections and real result commit.
    // Source17FFB0 owns all27x27 rows, including off-diagonal AI abilities.
    game::OriginalTournamentProfile50KG tournament{};
    bool tournamentResultArmed=false;
    // Native generation bumps made by the race-end stores 1AB598 (7E4) and
    // 1AB5A8 (7E8) while a launch is armed. In the original these run in the
    // same 1AB520 pass just before 1875C0, so they never invalidate it.
    std::uint64_t tournamentEndStoreBumps=0;
    OriginalUiRiderProfileStamp50KG tournamentLaunchStamp{};
    CourseRaceSelection tournamentLaunchSelection{};
    const game::LapTimingManager50ID* tournamentLaunchManager=nullptr;
    OriginalUiMainApplication50KG* tournamentLaunchApplication=nullptr;
    std::array<std::uint32_t,27> unlock10FAC{},hidden0730{};
    std::array<std::uint32_t,4> remembered{};
    std::array<std::uint32_t,7> rememberedCourse{};
    std::uint8_t packed1125D=0,packed1125E=0,packed1125F=0;
    std::uint32_t subtype11258=1;
    std::uint32_t comparison100C0=0;
    OriginalSaveProfileOwner50KG saveProfile;
    //17E330..33C clears only bit0, preserving opaque upper7E4 bits.
    //17E348 initializes the complete7E8 word. No synthetic full7E4 byte.
    bool bit0_7E4=false;
    std::uint32_t word7E8=0;
    std::uint32_t testRider=0; // Legacy fixture identity remains unchanged.
    bool gasganoScTest=false;
    bool tzidikTcTest=false;
    bool sebulbaTbTest=false;
    bool kraidGbTest=false;
    bool youngAnakinMcTest=false;
    bool scorchRcTest=false; // Distinct immutable native startup provenance.
    std::array<OriginalUiCourseProfileColors50KG,6> planetColors{};
    std::array<std::uint8_t,13> courseOrder100C4{};
    // The source180164 LH view of this profile's thirteen constructor words
    //100C8+40*i. All actual17EEF0..17F6D8 values fit signed16. Retained once
    //at startup, outside save payloads and mutable per-pod remaining prizes.
    std::array<std::int16_t,13> constructorPrizeHalves100C8{};
    TournamentCourseRecordSnapshot firstTournamentCourseRecord{};
    // Actual constructor-owned metadata, not a visual PAR guess. These
    // arrays are assigned at the same fresh-profile construction as100C4.
    std::array<std::int32_t,13> resultsPar100E8{};
    std::array<std::array<std::int32_t,7>,13> resultsKillPercent100CC{};
    std::array<std::int32_t,8> resultsPlacePercent102D0{};
    std::array<std::int32_t,27> racerNames3C{};
    std::array<TrackTextRow,13> trackText{};
    game::OriginalUiHallRecords50KG hallRecords{};
    // 1801D0 ends with memcpy(P+7EC, P+102F0, 0xC78) at 1858EC: the fresh
    // default table, retained for the 187F90/1880F0/188170 comparisons.
    game::OriginalUiHallRecords50KG defaultHall7EC{};
    // Unknown until the genuine1AC440 store leaf executes. Single-human
    // writes11260/64/68 only; no synthetic zeroes for second-human ranks.
    std::optional<game::OriginalRaceRecordInsertionStage50KG::Ranks> hallRanks11260;
    std::shared_ptr<const game::OriginalHallProfileIdentity50KG> hallProfileIdentity;
    bool hallRecordsInitialized=false;
    std::uint32_t rememberedHall6EC=1; //186674, shared fresh-profile reset.
    std::uint64_t generation=1;
    BetaPersistenceStatus betaStatus=BetaPersistenceStatus::notStarted;
    std::filesystem::path betaPath;
    native_beta50kg::ProcessLock betaLock;
    bool betaWritable=false;
    std::shared_ptr<const game::OriginalUiGameSettingsProfile50KG> betaSettings;
    bool readable() const noexcept {return common&&common->known&&common->mode34<=2;}
};
OriginalUiRiderProfileOwner50KG::OriginalUiRiderProfileOwner50KG() noexcept=default;
OriginalUiRiderProfileOwner50KG::~OriginalUiRiderProfileOwner50KG()=default;
bool OriginalUiRiderProfileOwner50KG::readWorldProfileByte1B44E8(
    const OriginalUiMainProfile50KG& actualCommon,std::uint8_t& out)const noexcept {
    return readSharedWorldProfileByteNative(actualCommon,out);
}
bool OriginalUiRiderProfileOwner50KG::readWorldProfileByte1AA2F4(
    const OriginalUiMainProfile50KG& actualCommon,std::uint8_t& out)const noexcept {
    return readSharedWorldProfileByteNative(actualCommon,out);
}
bool OriginalUiRiderProfileOwner50KG::readWorldCourseByte1AA354(
    const OriginalUiMainProfile50KG& actualCommon,std::uint8_t& out)const noexcept {
    if(!storage_||!storage_->readable()||storage_->common.get()!=&actualCommon)return false;
    out=storage_->packed1125F;return true;
}
bool OriginalUiRiderProfileOwner50KG::readSharedWorldProfileByteNative(
    const OriginalUiMainProfile50KG& actualCommon,std::uint8_t& out)const noexcept {
    if(!storage_||!storage_->readable()||storage_->common.get()!=&actualCommon)return false;
    out=storage_->common->playerBits1125C;return true;
}
bool OriginalUiRiderProfileOwner50KG::selectedTrackName187330(
    std::array<char,3>& out)const noexcept {
    if(!storage_||!storage_->readable())return false;
    const auto& s=*storage_;const auto index=std::uint32_t(s.packed1125F&15u);
    if(index>=s.trackText.size())return false; //Native supported-table bound.
    const auto& name=s.trackText[index].name11278;
    if(!name[0]||!name[1]||name[2])return false;
    out=name;return true;
}
bool OriginalUiRiderProfileOwner50KG::beginPodRace1A66CC(
    const OriginalUiMainProfile50KG& actualCommon)noexcept {
    if(!storage_||storage_->common.get()!=&actualCommon||!actualCommon.known||
       storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
    //1A66D8..E4 masks bit0 then ORs0;1A66F0 writes the same profile7E8.
    // These are actual stored source lanes, not a race-ready flag or receipt.
    storage_->bit0_7E4=false;
    storage_->word7E8=0;
    ++storage_->generation; //Native observation identity, not a guest field.
    return true;
}

bool OriginalUiRiderProfileOwner50KG::initializeAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    return initializeStartupProfile(std::move(common),std::move(archive),0,d);
}
bool OriginalUiRiderProfileOwner50KG::initializeGasganoTestProfileAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    return initializeStartupProfile(std::move(common),std::move(archive),7,d);
}
bool OriginalUiRiderProfileOwner50KG::initializeVisibleRiderTestProfileAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,std::uint32_t rider,Diagnostic& d) noexcept {
    if(rider!=3&&rider!=8&&rider!=22&&rider!=14&&rider!=21&&rider!=25&&rider!=26){d={};return fail(d,Error::range,0x17F970);}
    return initializeStartupProfile(std::move(common),std::move(archive),rider,d);
}
std::uint32_t OriginalUiRiderProfileOwner50KG::visibleRiderTestProfile() const noexcept {
    return storage_&&(storage_->testRider==3||storage_->testRider==8||storage_->testRider==22||storage_->testRider==14||storage_->testRider==21||storage_->testRider==25||storage_->testRider==26)?storage_->testRider:0;
}
bool OriginalUiRiderProfileOwner50KG::gasganoTestProfile() const noexcept {
    return storage_&&storage_->testRider==7;
}
bool OriginalUiRiderProfileOwner50KG::initializeMawhonicRylothTestProfileAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    return initializeStartupProfile(std::move(common),std::move(archive),9,d);
}
bool OriginalUiRiderProfileOwner50KG::mawhonicRylothTestProfile() const noexcept {
    return storage_&&storage_->testRider==9;
}
bool OriginalUiRiderProfileOwner50KG::initializeWanMbTestProfileAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    return initializeStartupProfile(std::move(common),std::move(archive),16,d);
}
bool OriginalUiRiderProfileOwner50KG::wanMbTestProfile() const noexcept {
    return storage_&&storage_->testRider==16;
}
bool OriginalUiRiderProfileOwner50KG::initializeGasganoScTestProfileAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    return initializeStartupProfile(std::move(common),std::move(archive),0,d,true);
}
bool OriginalUiRiderProfileOwner50KG::gasganoScTestProfile() const noexcept {
    return storage_&&storage_->gasganoScTest;
}
bool OriginalUiRiderProfileOwner50KG::initializeTzidikTcTestProfileAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    return initializeStartupProfile(std::move(common),std::move(archive),0,d,false,true);
}
bool OriginalUiRiderProfileOwner50KG::tzidikTcTestProfile() const noexcept {
    return storage_&&storage_->tzidikTcTest;
}
bool OriginalUiRiderProfileOwner50KG::initializeScorchRcTestProfileAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    return initializeStartupProfile(std::move(common),std::move(archive),0,d,false,false,true);
}
bool OriginalUiRiderProfileOwner50KG::scorchRcTestProfile() const noexcept {
    return storage_&&storage_->scorchRcTest;
}
bool OriginalUiRiderProfileOwner50KG::initializeSebulbaTbTestProfileAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    return initializeStartupProfile(std::move(common),std::move(archive),0,d,false,false,false,true);
}
bool OriginalUiRiderProfileOwner50KG::sebulbaTbTestProfile() const noexcept {
    return storage_&&storage_->sebulbaTbTest;
}
bool OriginalUiRiderProfileOwner50KG::initializeKraidGbTestProfileAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d) noexcept {
    return initializeStartupProfile(std::move(common),std::move(archive),0,d,false,false,false,false,true);
}
bool OriginalUiRiderProfileOwner50KG::kraidGbTestProfile() const noexcept {
    return storage_&&storage_->kraidGbTest;
}
bool OriginalUiRiderProfileOwner50KG::initializeYoungAnakinMcTestProfileAtApplicationStartup(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,Diagnostic& d,std::uint32_t mcRider) noexcept {
    if(mcRider!=1&&mcRider!=14&&mcRider!=21&&mcRider!=25&&mcRider!=26){d={};return fail(d,Error::range,0x17F970);}
    return initializeStartupProfile(std::move(common),std::move(archive),0,d,false,false,false,false,false,true,mcRider);
}
bool OriginalUiRiderProfileOwner50KG::youngAnakinMcTestProfile() const noexcept {
    return storage_&&storage_->youngAnakinMcTest;
}
bool OriginalUiRiderProfileOwner50KG::initializeStartupProfile(
    std::shared_ptr<OriginalUiMainProfile50KG> common,
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive,std::uint32_t testRider,Diagnostic& d,bool gasganoSc,bool tzidikTc,bool scorchRc,bool sebulbaTb,bool kraidGb,bool youngAnakinMc,std::uint32_t mcRider) noexcept {
    d={};if(storage_)return fail(d,Error::alreadyInitialized,0x17F970);
    if((youngAnakinMc&&(kraidGb||sebulbaTb||scorchRc||tzidikTc||gasganoSc||testRider!=0))||(kraidGb&&(sebulbaTb||scorchRc||tzidikTc||gasganoSc||testRider!=0))||(sebulbaTb&&(scorchRc||tzidikTc||gasganoSc||testRider!=0))||(scorchRc&&(tzidikTc||gasganoSc||testRider!=0))||(tzidikTc&&(gasganoSc||testRider!=0))||(gasganoSc&&testRider!=0)||(testRider!=0&&testRider!=3&&testRider!=7&&testRider!=8&&testRider!=9&&testRider!=16&&testRider!=22&&testRider!=14&&testRider!=21&&testRider!=25&&testRider!=26))return fail(d,Error::range,0x17F970);
    if(!common||!common->known)return fail(d,Error::commonProfile,0x17F970);
    if(!archive||!archive->loaded()||!archive->identity()||
       archive->identity()->id!=assets::OriginalUiArchiveId50KG::podPhysics)
        return fail(d,Error::archive,0x1870FC);
    try {
        auto next=std::make_unique<Storage>();next->common=std::move(common);next->archive=std::move(archive);
        next->hallProfileIdentity=std::shared_ptr<const game::OriginalHallProfileIdentity50KG>(
            new game::OriginalHallProfileIdentity50KG());
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
            next->tournament.base0180[rider]=authored.base0180;
            next->tournament.maximum0408[rider]=authored.maximum0408;
        }
        // Actual constructor100C8+40*i values, not reset-time defaults or
        // spent per-pod purses. The retained owner is published only after
        // all startup preparation succeeds; save loads do not replace it.
        next->constructorPrizeHalves100C8=game::originalTournamentPrizes50KG;
        // Authenticated constructor row profile100C4..100E8. Writes at
        //17EF00/17EEF0,17EF24/34/44/54/64/78/88 and17EF14 respectively.
        next->firstTournamentCourseRecord.slot=1;
        next->firstTournamentCourseRecord.course100C4=0;
        next->firstTournamentCourseRecord.constructorPurse100C8=next->constructorPrizeHalves100C8[0];
        next->firstTournamentCourseRecord.killPercent100CC={{30,50,60,70,80,90,100}};
        next->firstTournamentCourseRecord.par100E8=2;
        //17FFB0 initializes every career/pod row, retaining real prize/order
        // identity. Constructor-zero absent pods0/5/6/13 stay original zero.
        for(auto& career:next->tournament.careers){
            for(std::size_t pod=0;pod<27;++pod){
                career.pods[pod].abilities1920=next->tournament.base0180[pod];
                career.pods[pod].medals1938.fill(0);
                career.pods[pod].remainingPrizes1946=next->constructorPrizeHalves100C8;
            }
        }
        next->saveProfile.recordFreshNames(); //180098..B0 /1864CC..F4
        //186EF0's exact source roster. This is an application reset, not an
        // unlock-all option nor an inference from which art happens to exist.
        next->unlock10FAC.fill(0);next->hidden0730.fill(0);
        for(const auto rider:{2u,4u,10u,11u,12u,15u,19u,20u})next->unlock10FAC[rider]=1;
        for(const auto rider:{1u,14u,21u,25u,26u})next->hidden0730[rider]=1;
        // Explicit isolated test setup for the requested racer capture.
        // This does not execute187F40 or fabricate a qualification/result.
        if(testRider){next->unlock10FAC[testRider]=1;next->testRider=testRider;
            // Only this explicitly requested process-local test identity is revealed.
            if(testRider==14||testRider==21||testRider==25||testRider==26)next->hidden0730[testRider]=0;}
        if(gasganoSc){next->unlock10FAC[7]=1;next->gasganoScTest=true;}
        if(tzidikTc){next->unlock10FAC[24]=1;next->tzidikTcTest=true;}
        if(scorchRc){next->unlock10FAC[17]=1;next->scorchRcTest=true;}
        if(sebulbaTb){next->unlock10FAC[18]=1;next->sebulbaTbTest=true;}
        if(kraidGb){next->unlock10FAC[23]=1;next->kraidGbTest=true;}
        if(youngAnakinMc){next->unlock10FAC[mcRider]=1;next->hidden0730[mcRider]=0;next->youngAnakinMcTest=true;}
        // Only the explicitly selected MC test identity is revealed. Other
        // original hidden bonuses retain their actual reset visibility.
        //186678..186684, after unlock reset. Mode stays in the shared owner.
        next->remembered.fill(19);
        // Test profile enters through the original remembered-selection search.
        // No visible locked neighbors are skipped or hidden by navigation.
        if(testRider)next->remembered[0]=testRider;
        if(gasganoSc)next->remembered[0]=7;
        if(tzidikTc)next->remembered[0]=24;
        if(scorchRc)next->remembered[0]=17;
        if(sebulbaTb)next->remembered[0]=18;
        if(kraidGb)next->remembered[0]=23;
        if(youngAnakinMc)next->remembered[0]=mcRider;
        // Original constructor/reset fields, same retained fresh profile.
        next->rememberedCourse={{1,3,8,1,1,8,3}}; //186688..1866A8
        if(testRider==16)next->rememberedCourse[0]=7; // explicit native MB test override
        if(youngAnakinMc)next->rememberedCourse[0]=10; // exact MC authored slot
        if(kraidGb)next->rememberedCourse[0]=8; // exact GB authored slot
        if(sebulbaTb)next->rememberedCourse[0]=6; // exact TB authored slot
        if(scorchRc)next->rememberedCourse[0]=12; // exact RC authored slot
        if(tzidikTc)next->rememberedCourse[0]=13; // exact TC authored slot
        if(gasganoSc)next->rememberedCourse[0]=9; // exact SC authored slot, not course ID
        next->packed1125D=0x98;next->packed1125E=0x12;
        next->comparison100C0=0xffffffffu; //17E17C
        // saveProfile owns constructor-known100BC bits0/1; no duplicate bool.
        next->courseOrder100C4={{0,11,4,8,7,1,5,9,12,6,10,3,2}};
        //17EF14/17F30C write2; the other eleven +E8+40*i words use
        //unchanged V1=3 from17E7B0 through17F6FC (no intervening call).
        next->resultsPar100E8={{2,3,3,3,3,3,2,3,3,3,3,3,3}};
        for(std::size_t i=0;i<13;++i)next->resultsKillPercent100CC[i]=
            (i==0||i==6)?std::array<std::int32_t,7>{{30,50,60,70,80,90,100}}:
                         std::array<std::int32_t,7>{{20,30,50,60,70,80,100}};
        //17F77C..17F7F4: place1..8.102CC is separate mutable state,
        //not the zero-th percentage; unsupported place0 never indexes it.
        next->resultsPlacePercent102D0={{100,70,50,30,20,10,5,0}};
        next->racerNames3C=sourceRacerNames3C;next->trackText=sourceTrackText;
        //Actual constructor-owned byte11280+12*track. The same source seeds
        //also serve cold AI selection; no second rival table or player proxy.
        for(std::size_t track=0;track<next->trackText.size();++track){
            const auto& seed=game::originalFreshCourseDescriptors50KG[track];
            next->trackText[track].rival11280=seed.rival11280;
            //17E9F0..17EED0 copies lowercase source names, whereas the
            //shared cold-loader descriptor spells its identifiers uppercase.
            next->trackText[track].name11278={{static_cast<char>(seed.first-'A'+'a'),
                static_cast<char>(seed.second-'A'+'a'),'\0'}};
        }
        //17E7D8..17E880 ->17F800..17F884; stride12, primary+79C/secondary+7A0.
        next->planetColors={{{0xc2a684,0x80674a},{0x8cc46e,0x628036},{0xc0b5a5,0x8c8479},
            {0xf4f1a2,0xa8a66f},{0xce766d,0xffbab2},{0xb26a36,0xe5a373}}};
        // Commit fixture-only shared availability after every fallible preparation.
        // The source course order and career matrix stay untouched.
        if(testRider==9)next->common->availability11018=
            static_cast<std::uint8_t>((next->common->availability11018&0xe0u)|5u);
        if(testRider==16)next->common->availability11018=
            static_cast<std::uint8_t>((next->common->availability11018&0xe0u)|7u);
        if(gasganoSc)next->common->availability11018=
            static_cast<std::uint8_t>((next->common->availability11018&0xe0u)|9u);
        if(tzidikTc)next->common->availability11018=
            static_cast<std::uint8_t>((next->common->availability11018&0xe0u)|13u);
        if(scorchRc)next->common->availability11018=
            static_cast<std::uint8_t>((next->common->availability11018&0xe0u)|12u);
        if(sebulbaTb)next->common->availability11018=
            static_cast<std::uint8_t>((next->common->availability11018&0xe0u)|6u);
        if(kraidGb)next->common->availability11018=
            static_cast<std::uint8_t>((next->common->availability11018&0xe0u)|8u);
        if(youngAnakinMc)next->common->availability11018=
            static_cast<std::uint8_t>((next->common->availability11018&0xe0u)|10u);
        storage_=std::move(next);d={};return true;
    }catch(...){return fail(d,Error::allocation,0x17F970);}
}
bool OriginalUiRiderProfileOwner50KG::initializeHallRecordsAtApplicationStartup(
    const assets::OriginalUiEnglish50KG& english,Diagnostic& d) noexcept {
    d={};if(!storage_||!storage_->common||!storage_->common->known)
        return fail(d,Error::commonProfile,0x17DF30);
    if(storage_->hallRecordsInitialized)return fail(d,Error::alreadyInitialized,0x1801D0);
    if(storage_->generation==std::numeric_limits<std::uint64_t>::max())return fail(d,Error::range,0x1801D0);
    game::OriginalUiHallRecords50KG next;game::OriginalUiHallRecordsDiagnostic50KG records;
    if(!game::initializeOriginalUiHallRecords50KG(english,next,records))
        return fail(d,records.error==game::OriginalUiHallRecordsDiagnostic50KG::Error::localization?Error::archive:Error::range,records.sourcePC);
    if(!storage_->saveProfile.recordFreshHall(next))return fail(d,Error::allocation,0x1801D0);
    storage_->hallRecords=next;storage_->defaultHall7EC=next;
    storage_->hallRecordsInitialized=true;++storage_->generation;
    loadNativeBetaProgressAtStartup();return true;
}
bool OriginalUiRiderProfileOwner50KG::hallRecordsReady() const noexcept {
    return storage_&&storage_->common&&storage_->common->known&&storage_->hallRecordsInitialized&&
        storage_->saveProfile.hallRecordsCurrent(storage_->hallRecords,
            storage_->hallProfileIdentity,storage_->generation);
}
namespace {
// PORT EXTENSION, NOT ORIGINAL: one add-on record waiting for its stock-stage
// commit (owner thread only; host memory). Cleared at every commit.
struct PendingDlcRecord50DL {
    bool armed=false;const void* owner=nullptr;std::uint64_t generation=0;
    assets::dlc50DL::ProgressRaceResult result;
};
PendingDlcRecord50DL& pendingDlcRecord() noexcept {static PendingDlcRecord50DL pending;return pending;}
// PORT EXTENSION, NOT ORIGINAL (DESIGN.md, Hall decision 2026-11-16): the
// stock course's Hall rows merged at display/qualification time with the DLC
// side file's add-on pod rows for that course. Stock rows are never written.
// A DLC slot joins only when its racer key is an admitted add-on pod (or
// "stock:NN"), so a removed package drops out and the stock rows show again.
// Each DLC entry is inserted with the original 1AC440 ordering (before the
// first stock entry it strictly beats: lower lap/race time, higher knockout
// count), then each table keeps its 3 entries. An add-on entry's name lane
// carries the pod's package name (at most 20 characters + NUL).
const assets::dlc50DL::AddonPod* admittedPodByKey(const std::string& key) noexcept {
    for(std::size_t i=0;i<assets::dlc50DL::addonPodCount();++i){
        const auto* p=assets::dlc50DL::addonPodAt(i);if(p&&p->key==key)return p;}
    return nullptr;
}
struct HallMergeCounts50DL {unsigned lap=0,race=0,knockouts=0;};
bool mergeDlcHallRows50DL(std::uint32_t course,game::OriginalUiHallCourseRecords50KG& row,
    HallMergeCounts50DL& counts) noexcept {
    counts={};
    try {
        assets::dlc50DL::CourseRow stock;
        if(!assets::dlc50DL::courseRow(course,stock)||stock.addon)return false;
        assets::dlc50DL::ProgressHallView view;
        if(!assets::dlc50DL::progressOwner().stockCourseAddonPodHall(stock.code,view))return false;
        using Lane=std::array<char,21>;
        const auto laneOf=[](const std::string& key,const Lane& stored,Lane& out){
            out.fill(0);
            if(key.rfind("stock:",0)==0){
                if(!assets::dlc50DL::validRacerReference(key))return false;
                out=stored;out[out.size()-1]=0;return true;
            }
            const auto* pod=admittedPodByKey(key);if(!pod)return false;
            for(std::size_t k=0;k<pod->name.size()&&k+1<out.size()&&k<assets::dlc50DL::ProgressLimits::nameChars;++k)out[k]=pod->name[k];
            return true;
        };
        // One table: values v (3 stock), lanes n; candidate (value,lane) inserted.
        const auto table=[&](auto& values,std::array<Lane,3>& names,const auto& dlcValues,
            const std::array<Lane,3>& dlcNames,const std::array<std::string,3>& racers,bool higherWins,unsigned& added){
            for(std::size_t i=0;i<3;++i){
                if(racers[i].empty())continue;
                Lane lane;if(!laneOf(racers[i],dlcNames[i],lane))continue;
                const auto value=dlcValues[i];
                std::size_t at=3;
                for(std::size_t k=0;k<3;++k){
                    const bool better=higherWins?value>values[k]:value<values[k];
                    if(better){at=k;break;}
                }
                if(at>=3)continue;   // bounds: only slots 0..2 are written
                for(std::size_t k=2;k>at;--k){values[k]=values[k-1];names[k]=names[k-1];}
                values[at]=value;names[at]=lane;++added;
            }
        };
        auto next=row;
        table(next.lap102F0,next.lapNames102FC,view.rows.lap102F0,view.rows.lapNames102FC,view.lapRacers,false,counts.lap);
        table(next.race1033C,next.raceNames10348,view.rows.race1033C,view.rows.raceNames10348,view.raceRacers,false,counts.race);
        table(next.knockouts10388,next.knockoutNames10394,view.rows.knockouts10388,view.rows.knockoutNames10394,
            view.knockoutRacers,true,counts.knockouts);
        if(!counts.lap&&!counts.race&&!counts.knockouts)return false;
        row=next;return true;
    }catch(...){counts={};return false;}
}
}
bool OriginalUiRiderProfileOwner50KG::prepareSingleHumanRecordInsertion1AC440(
    const game::OriginalRaceRecordRacerObservation50KG& row0,std::uint8_t race1B0,
    std::optional<game::OriginalRaceRecordInsertionStage50KG>& out,
    game::OriginalRaceRecordInsertionStage50KG::Diagnostic& d)const noexcept {
    using Stage=game::OriginalRaceRecordInsertionStage50KG;
    d={Stage::Status::profile,0x1ac440u};
    if(!hallRecordsReady()||!storage_->readable()||
       (storage_->common->mode34!=0u&&storage_->common->mode34!=2u))return false;
    const auto& s=*storage_;
    OriginalPersistedProfileName50KG first,second;
    //1AC49C uses career219D ONLY for mode2; otherwise1AC4B8 uses player0.
    //1AC4D8 copies player1 even for the single-human path. These are retained
    // profile names, never rider localization or native Results text input.
    if(!s.saveProfile.persistedName(s.common->mode34==2u,
           s.common->mode34==2u?s.comparison100C0:0u,first)){
        d={Stage::Status::name,s.common->mode34==2u?0x1ac49cu:0x1ac4b8u};return false;
    }
    if(!s.saveProfile.persistedName(false,1u,second)){
        d={Stage::Status::name,0x1ac4d8u};return false;
    }
    std::array<char,21> name{};
    //Only the known C-string prefix is read. A fresh persisted name has an
    //unknown saved-field tail; strncpy20 does not need or fabricate that tail.
    for(std::size_t i=0;i<name.size();++i){
        name[i]=first.c_str()[i];if(name[i]==0)break;
    }
    const auto packed=static_cast<std::uint16_t>(s.common->playerBits1125C|
        (std::uint16_t(s.packed1125D)<<8));
    if(!Stage::prepareSingleHuman(s.hallRecords,s.subtype11258,
        s.common->playerBits1125C,packed,s.packed1125F,race1B0,row0,name,
        s.hallProfileIdentity,s.generation,s.common->mode34,out,d))return false;
    // PORT EXTENSION, NOT ORIGINAL (DLC decision D7 b): a free-play race run
    // with an add-on pod never changes the stock Hall. The stock stage keeps
    // its gates (owner, course, name, generation) but carries the unchanged
    // rows and zero ranks; the record goes to the DLC side file at commit.
    const auto profileRider=std::uint32_t(s.packed1125D>>3);
    const auto raced=game::dlc50DL::racedRider(profileRider);
    if(out&&s.common->mode34==0u&&raced!=profileRider&&assets::dlc50DL::addonPod(raced))try{
        auto& pending=pendingDlcRecord();
        pending={};
        assets::dlc50DL::CourseRow row;std::string upper;
        if(!assets::dlc50DL::courseRow(out->course(),row)||row.addon||
           !assets::dlc50DL::stockCourseCode(row.code,upper))return false;
        auto& r=pending.result;
        r.stockCode=upper;r.subtype11258=s.subtype11258;r.lapCount1125C=s.common->playerBits1125C;
        r.packed1125C=packed;r.unlock10FAC=s.unlock10FAC;r.race1B0Low2=static_cast<std::uint8_t>(race1B0&3u);
        r.racers[0].racer=game::dlc50DL::riderKey(raced);r.racers[0].name=std::string(name.data());
        r.racers[0].values={row0,0u}; // 1AC440 is reached only for the finished, unretired row0
        pending.owner=this;pending.generation=s.generation;pending.armed=true;
        out->after_=out->before_;out->ranks_={};
    }catch(...){pendingDlcRecord()=PendingDlcRecord50DL{};return false;}
    return true;
}
bool OriginalUiRiderProfileOwner50KG::prepareTwoHumanRecordInsertion1AC440(
    const std::array<game::OriginalRaceRecordRacerObservation50KG,2>& rows,
    std::uint8_t race1B0,std::optional<game::OriginalRaceRecordInsertionStage50KG>& out,
    game::OriginalRaceRecordInsertionStage50KG::Diagnostic& d)const noexcept {
    using Stage=game::OriginalRaceRecordInsertionStage50KG;
    d={Stage::Status::profile,0x1ac440u};
    //Pure preparation can observe all existing readable modes0..2, including
    //mode1's two-player profile. This does not enable any launch or result path.
    if(!hallRecordsReady()||!storage_->readable())return false;
    const auto& s=*storage_;
    std::array<OriginalPersistedProfileName50KG,2> names;
    if(!s.saveProfile.persistedName(s.common->mode34==2u,
           s.common->mode34==2u?s.comparison100C0:0u,names[0])){
        d={Stage::Status::name,s.common->mode34==2u?0x1ac49cu:0x1ac4b8u};return false;
    }
    if(!s.saveProfile.persistedName(false,1u,names[1])){
        d={Stage::Status::name,0x1ac4d8u};return false;
    }
    std::array<std::array<char,21>,2> copiedNames{};
    for(std::size_t player=0;player<names.size();++player){
        //Read each known C-string prefix only; no claim about unknown saved
        //tail bytes. The downstream strncpy20 effect supplies its own padding.
        for(std::size_t i=0;i<copiedNames[player].size();++i){
            copiedNames[player][i]=names[player].c_str()[i];
            if(copiedNames[player][i]==0)break;
        }
    }
    const auto packed=static_cast<std::uint16_t>(s.common->playerBits1125C|
        (std::uint16_t(s.packed1125D)<<8));
    return Stage::prepareTwoHumans(s.hallRecords,s.subtype11258,
        s.common->playerBits1125C,packed,s.packed1125F,race1B0,rows,copiedNames,
        s.hallProfileIdentity,s.generation,s.common->mode34,out,d);
}
bool OriginalUiRiderProfileOwner50KG::prepareRaceRecordQualification1ABFC0(
    std::uint32_t selectorA1,std::optional<std::uint8_t> race1B0Low2,
    const game::OriginalRaceRecordQualificationRacerObservation50KG* rows,
    std::size_t rowCount,std::optional<game::OriginalRaceRecordQualification50KG>& out,
    game::OriginalRaceRecordQualification50KG::Diagnostic& d)const noexcept {
    using Query=game::OriginalRaceRecordQualification50KG;
    d={Query::Status::profile,0x1abfc0u,0,0};
    if(!storage_||!storage_->readable())return false;
    const auto& s=*storage_;
    //A genuine early exclusion does not consume Hall data or racer facts.
    //Otherwise retain the SAME fresh/import/native-beta Hall provenance gate;
    //neither qualification nor ordinary observations can bypass that owner.
    if(s.subtype11258!=2u&&!s.unlock10FAC[0]&&!s.unlock10FAC[5]&&
       !s.unlock10FAC[13]&&!hallRecordsReady())return false;
    const game::LapRecordConfig50ID config{s.subtype11258,s.common->playerBits1125C,
        static_cast<std::uint16_t>(s.common->playerBits1125C|
            (std::uint16_t(s.packed1125D)<<8))};
    // PORT EXTENSION, NOT ORIGINAL: a free-play run with a latched add-on pod
    // qualifies against the merged Hall (stock + DLC rows of this course), the
    // same table the Hall shows. Stock runs, with or without DLC, read the
    // stock rows exactly as before.
    const auto profileRider=std::uint32_t(s.packed1125D>>3);
    const auto raced=game::dlc50DL::racedRider(profileRider);
    if(s.common->mode34==0u&&raced!=profileRider&&assets::dlc50DL::addonPod(raced)){
        const auto course=std::uint32_t(s.packed1125F&15u);
        if(course<s.hallRecords.courses.size()){
            auto merged=s.hallRecords; // host copy; the stock rows stay untouched
            HallMergeCounts50DL counts;
            if(mergeDlcHallRows50DL(course,merged.courses[course],counts))
                std::fprintf(stderr,"dlc_record_qualify course=%u table=merged lap_addon=%u race_addon=%u knockout_addon=%u\n",
                    course,counts.lap,counts.race,counts.knockouts);
            return Query::prepare(config,merged,s.unlock10FAC,s.packed1125F,race1B0Low2,
                selectorA1,rows,rowCount,s.generation,out,d);
        }
    }
    return Query::prepare(config,s.hallRecords,s.unlock10FAC,s.packed1125F,race1B0Low2,
        selectorA1,rows,rowCount,s.generation,out,d);
}
bool OriginalUiRiderProfileOwner50KG::prepareRaceRivalQualification1ABE70(
    const game::OriginalRaceRivalRacerObservation50KG* rows,std::size_t availableRows,
    std::int32_t actualManagerCount30,
    std::optional<game::OriginalRaceRivalQualification50KG>& out,
    game::OriginalRaceRivalQualification50KG::Diagnostic& d)const noexcept {
    using Query=game::OriginalRaceRivalQualification50KG;
    d={Query::Status::profile,0x1abe70u,0};
    if(!storage_||!storage_->readable())return false;
    const auto& s=*storage_;const auto course=std::uint32_t(s.packed1125F&15u);
    //1ABED4 reads signed11280 before either row0 getter. Selection is the
    //actual packed low nibble, not tournament order or a caller-selected pod.
    if(course>=s.trackText.size()){
        d={Query::Status::course,0x1abed4u,0};return false;
    }
    return Query::prepare(course,s.trackText[course].rival11280,s.unlock10FAC,
        rows,availableRows,actualManagerCount30,s.generation,out,d);
}
bool OriginalUiRiderProfileOwner50KG::storeEndRecordBit1AB598(
    const game::OriginalRaceRecordQualification50KG& query,
    OriginalUiRiderProfileStamp50KG expected,OriginalUiRiderProfileStamp50KG& after)noexcept {
    if(!storage_||!storage_->readable())return false;
    auto& s=*storage_;
    const bool value=(query.originalV0()&1u)!=0;
    const bool changed=s.bit0_7E4!=value;
    if(s.subtype11258!=1u||expected.generation!=s.generation||expected.mode34!=s.common->mode34||
       query.profileGeneration()!=s.generation||query.selectorA1()!=0u||
       query.course()!=std::uint32_t(s.packed1125F&15u)||
       (changed&&s.generation==std::numeric_limits<std::uint64_t>::max()))return false;
    //1AB578 masksV0 bit0;1AB598 is the DELAY SLOT before1ABE70 starts.
    //Only this represented bit changes. Opaque upper7E4 bits stay untouched.
    s.bit0_7E4=value; //The source store occurs even when its value is unchanged.
    if(changed){++s.generation;if(s.tournamentResultArmed)++s.tournamentEndStoreBumps;} //Native mutation version, never a guest store counter.
    after={s.generation,s.common->mode34};return true;
}
bool OriginalUiRiderProfileOwner50KG::storeEndRivalWord1AB5A8(
    const game::OriginalRaceRivalQualification50KG& query,
    OriginalUiRiderProfileStamp50KG expected,OriginalUiRiderProfileStamp50KG& after)noexcept {
    if(!storage_||!storage_->readable())return false;
    auto& s=*storage_;const auto course=std::uint32_t(s.packed1125F&15u);
    const auto value=query.originalV0();const bool changed=s.word7E8!=value;
    if(s.subtype11258!=1u||expected.generation!=s.generation||expected.mode34!=s.common->mode34||
       query.profileGeneration()!=s.generation||query.course()!=course||course>=s.trackText.size()||
       query.rival11280()!=s.trackText[course].rival11280||
       (changed&&s.generation==std::numeric_limits<std::uint64_t>::max()))return false;
    s.word7E8=value; //Full source word store, even when unchanged, AFTER1ABE70.
    if(changed){++s.generation;if(s.tournamentResultArmed)++s.tournamentEndStoreBumps;}
    after={s.generation,s.common->mode34};return true;
}
bool OriginalUiRiderProfileOwner50KG::recordInsertionIdentityCurrent(
    const game::OriginalRaceRecordInsertionStage50KG& stage)const noexcept {
    if(!hallRecordsReady()||!storage_->readable())return false;
    const auto& s=*storage_;
    return stage.profileIdentity_&&stage.profileIdentity_.get()==s.hallProfileIdentity.get()&&
        stage.profileGeneration()==s.generation&&stage.profileMode34()==s.common->mode34&&
        stage.course()==std::uint32_t(s.packed1125F&15u)&&
        s.generation!=std::numeric_limits<std::uint64_t>::max()&&
        !s.testRider&&!s.gasganoScTest&&!s.tzidikTcTest&&!s.scorchRcTest&&
        !s.sebulbaTbTest&&!s.kraidGbTest&&!s.youngAnakinMcTest;
}
bool OriginalUiRiderProfileOwner50KG::prepareHallRecordProvenance1AC440(
    const game::OriginalRaceRecordInsertionStage50KG& stage,
    std::unique_ptr<OriginalSaveProfileOwner50KG>& out)const noexcept {
    if(!recordInsertionIdentityCurrent(stage))return false;
    const auto& s=*storage_;
    std::unique_ptr<OriginalSaveProfileOwner50KG> prepared;
    if(!s.saveProfile.prepareHallRecordDescendant(s.hallRecords,stage,
        s.hallProfileIdentity,s.generation,prepared))return false;
    // Allocation happened only in detached metadata. Revalidate actual owner,
    // generation, root and preimage before publishing even this preparation.
    if(!hallRecordProvenancePreparationCurrent1AC440(stage,*prepared))return false;
    out=std::move(prepared);return true;
}
bool OriginalUiRiderProfileOwner50KG::hallRecordProvenancePreparationCurrent1AC440(
    const game::OriginalRaceRecordInsertionStage50KG& stage,
    const OriginalSaveProfileOwner50KG& prepared)const noexcept {
    if(!recordInsertionIdentityCurrent(stage))return false;
    const auto& s=*storage_;
    return s.saveProfile.hallRecordPreparationMatches(prepared,s.hallRecords,stage,
        s.hallProfileIdentity,s.generation);
}
bool OriginalUiRiderProfileOwner50KG::commitSingleHumanHallRecords1AC440(
    const game::OriginalRaceRecordInsertionStage50KG& stage,
    OriginalSaveProfileOwner50KG& prepared,OriginalUiRiderProfileStamp50KG expected,
    OriginalUiRiderProfileStamp50KG& after)noexcept {
    if(!storage_||&prepared==&storage_->saveProfile||stage.secondHumanRanks()||
       expected.generation!=storage_->generation||expected.mode34!=stage.profileMode34()||
       !hallRecordProvenancePreparationCurrent1AC440(stage,prepared))return false;
    auto& s=*storage_;auto& course=s.hallRecords.courses[stage.course()];
    const auto& next=stage.after();
    // All allocation/identity/preimage checks precede this interval. Assign
    // original category effects then immediate rank stores, in source order.
    // The reviewed stage represents skipped leaf effects as unchanged data,
    // not as an invented successful record. Rank zero is still a real store.
    if(!s.hallRanks11260)s.hallRanks11260.emplace();
    course.lap102F0=next.lap102F0;course.lapNames102FC=next.lapNames102FC;
    s.hallRanks11260->lap11260=stage.ranks().lap11260; //1AC540
    course.race1033C=next.race1033C;course.raceNames10348=next.raceNames10348;
    s.hallRanks11260->race11264=stage.ranks().race11264; //1AC578
    course.knockouts10388=next.knockouts10388;course.knockoutNames10394=next.knockoutNames10394;
    s.hallRanks11260->knockouts11268=stage.ranks().knockouts11268; //1AC5AC
    s.saveProfile.hallRecordDescendant_.swap(prepared.hallRecordDescendant_);
    ++s.generation; // Preflight rejects MAX; consumes this exact stage once.
    after={s.generation,s.common->mode34};
    // PORT EXTENSION, NOT ORIGINAL: the add-on pod's record staged by
    // prepareSingleHumanRecordInsertion1AC440 for this exact stage.
    {auto& pending=pendingDlcRecord();
     if(pending.armed&&pending.owner==this&&pending.generation==stage.profileGeneration()){
         const auto outcome=assets::dlc50DL::progressOwner().submit(pending.result);
         std::fprintf(stderr,"dlc_record racer=%s course=%s status=%u lap_rank=%u race_rank=%u knockout_rank=%u\n",
             pending.result.racers[0].racer.c_str(),pending.result.stockCode.c_str(),unsigned(outcome.status),
             unsigned(outcome.first.lap),unsigned(outcome.first.race),unsigned(outcome.first.knockouts));
     }
     pending={};}
    // Native autosave (as the tournament result and unlock grants): the new
    // Hall rows persist even if the session ends before the next save point.
    if(s.betaSettings&&(s.betaSettings->flags11050&1u))(void)saveNativeBetaProgress();
    return true;
}
bool OriginalUiRiderProfileOwner50KG::commitSingleHumanRaceRecords1AC440(
    const game::OriginalRaceRecordRacerObservation50KG& row0,std::uint8_t race1B0,
    OriginalUiRiderProfileStamp50KG expected,OriginalUiRiderProfileStamp50KG& after,
    game::OriginalRaceRecordInsertionStage50KG::Diagnostic& d)noexcept {
    using Stage=game::OriginalRaceRecordInsertionStage50KG;
    d={Stage::Status::profile,0x1ac440u};
    if(!storage_||expected.generation!=storage_->generation||!storage_->common||
       expected.mode34!=storage_->common->mode34)return false;
    std::optional<Stage> stage;
    if(!prepareSingleHumanRecordInsertion1AC440(row0,race1B0,stage,d)||!stage||
       stage->secondHumanRanks()||stage->profileMode34()!=expected.mode34)return false;
    std::unique_ptr<OriginalSaveProfileOwner50KG> prepared;
    if(!prepareHallRecordProvenance1AC440(*stage,prepared)||!prepared){d={Stage::Status::profile,0x1ac440u};return false;}
    if(!commitSingleHumanHallRecords1AC440(*stage,*prepared,expected,after)){d={Stage::Status::profile,0x1ac440u};return false;}
    d={Stage::Status::ok,0x1ac8bcu};return true; // prepared releases the displaced descendant here.
}
bool OriginalUiRiderProfileOwner50KG::markResultsHallDirty1CA6E4(
    OriginalUiRiderProfileStamp50KG expected)noexcept {
    if(!storage_||!storage_->readable()||expected.mode34!=0u||
       storage_->common->mode34!=expected.mode34||storage_->generation!=expected.generation||
       storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
    // Shared actual save-image owner preserves every unrelated/unknown bit.
    // This sets the dirty bit only; no file write, persisted receipt or award.
    storage_->saveProfile.markDirty1AB648();
    ++storage_->generation;
    return true;
}
bool OriginalUiRiderProfileOwner50KG::hallSnapshotForMode(std::uint32_t mode,
    game::OriginalUiHallProfile50KG& out) const noexcept {
    if(!hallRecordsReady()||(mode!=5&&mode!=2))return false;
    const auto& s=*storage_;game::OriginalUiHallProfile50KG p;
    p.ownerReady=true;p.mode34=mode;p.remembered6EC=s.rememberedHall6EC;
    p.packed1125E=s.packed1125E;p.availability11018=s.common->availability11018;
    p.bit0_7E4=s.bit0_7E4;p.courseOrder100C4=s.courseOrder100C4;
    if(mode==2){
        // No prospective mode2: 1AB5BC's 17DE10(6,11,1) leaves P+34 alone.
        game::OriginalRaceAckFacts50PD f;
        if(s.common->mode34!=2u||!raceAcknowledgementFacts1AB5BC(f)||f.mode34!=2u)return false;
        p.order187B10=f.order187B10;p.boundary102CC=f.boundary102CC;
        p.finalMedal1938=f.finalMedal;p.unlockPending187E30=f.unlockPending187E30;
        p.raceCourse1125F=static_cast<std::uint8_t>(s.packed1125F&15u);
        //11260/64/68 are the last 1AC440 rank stores. 1126C/70/74 are cleared
        // with them at race start (1A77E0/1A7804, 1ACCB0/1ACCD4) and written only
        // by the two-human 1AC440 branch, which no native race reaches: zero.
        if(s.hallRanks11260)p.ranks11260={s.hallRanks11260->lap11260,
            s.hallRanks11260->race11264,s.hallRanks11260->knockouts11268};
    }
    out=p;return true;
}
bool OriginalUiRiderProfileOwner50KG::hallSnapshot(game::OriginalUiHallProfile50KG& out) const noexcept {
    return hallRecordsReady()&&hallSnapshotForMode(storage_->common->mode34,out);
}
bool OriginalUiRiderProfileOwner50KG::hallRecordSnapshot(std::uint32_t course,
    game::OriginalUiHallCourseRecords50KG& out) const noexcept {
    if(!hallRecordsReady()||course>=storage_->hallRecords.courses.size())return false;
    // Loading Screen1CADE0 reads these same records for its actual committed
    // Course in mode0. This immutable observation does not enter Hall mode,
    // change a record, or admit a race/renderer. Hall browsing remains mode5.
    // The same Loading Screen is reached from a Tournament (mode2) launch; it
    // reads the identical committed-course records, so mode2 is admitted too.
    if(storage_->common->mode34!=5&&
       !((storage_->common->mode34==0||storage_->common->mode34==2)&&course==(storage_->packed1125F&15u)))return false;
    out=storage_->hallRecords.courses[course];return true;
}
bool OriginalUiRiderProfileOwner50KG::hallPlanetColors(std::uint32_t planet,
    OriginalUiCourseProfileColors50KG& out) const noexcept {
    if(!hallRecordsReady()||storage_->common->mode34!=5||planet>=storage_->planetColors.size())return false;
    out=storage_->planetColors[planet];return true;
}
bool OriginalUiRiderProfileOwner50KG::hallBrowseRecordSnapshot(std::uint32_t course,
    game::OriginalUiHallCourseRecords50KG& out) const noexcept {
    if(!hallRecordsReady()||(storage_->common->mode34!=5&&storage_->common->mode34!=2)||
       course>=storage_->hallRecords.courses.size())return false;
    out=storage_->hallRecords.courses[course];
    // PORT EXTENSION, NOT ORIGINAL: add-on pod records merged at display time
    // (mergeDlcHallRows50DL). Without DLC rows nothing changes or is logged.
    HallMergeCounts50DL counts;
    if(mergeDlcHallRows50DL(course,out,counts)){
        // One line per course and merged shape (the page renders every frame).
        static std::array<std::uint32_t,13> logged{};
        const auto shape=1u+counts.lap+4u*counts.race+16u*counts.knockouts;
        if(course<logged.size()&&logged[course]!=shape){
            logged[course]=shape;
            std::fprintf(stderr,"dlc_hall_merge course=%u lap_addon=%u race_addon=%u knockout_addon=%u icon=absent "
                "reason=\"add-on UI archive art is not catalogued for the Hall\"\n",
                course,counts.lap,counts.race,counts.knockouts);
        }
    }
    return true;
}
bool OriginalUiRiderProfileOwner50KG::hallBrowsePlanetColors(std::uint32_t planet,
    OriginalUiCourseProfileColors50KG& out) const noexcept {
    //1E3438 reads P+79C+planet*12 in every mode.
    if(!hallRecordsReady()||(storage_->common->mode34!=5&&storage_->common->mode34!=2)||
       planet>=storage_->planetColors.size())return false;
    out=storage_->planetColors[planet];return true;
}
bool OriginalUiRiderProfileOwner50KG::commitHallPlanet(std::uint32_t planet,
    OriginalUiRiderProfileStamp50KG expected) noexcept {
    if(!hallRecordsReady()||(storage_->common->mode34!=5&&storage_->common->mode34!=2)||
       expected.mode34!=storage_->common->mode34||
       expected.generation!=storage_->generation||planet>=storage_->planetColors.size()||
       storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
    //1E4834 opening,1E43D0 next,1E4470 previous. Preserve lower five bits.
    storage_->packed1125E=static_cast<std::uint8_t>((storage_->packed1125E&0x1fu)|((planet&7u)<<5u));
    ++storage_->generation;return true;
}
bool OriginalUiRiderProfileOwner50KG::commitHallBack(std::uint16_t slot,
    const game::OriginalUiRouteDecision50KG& decision,OriginalUiRiderProfileStamp50KG expected) noexcept {
    if(!hallRecordsReady()||storage_->common->mode34!=5||expected.mode34!=5||
       expected.generation!=storage_->generation||storage_->generation==std::numeric_limits<std::uint64_t>::max()||
       slot<1||slot>13||slot>(storage_->common->availability11018&31u)||
       !decision.admitted||!decision.profileProposalComplete||decision.blockers||
       decision.action!=game::OriginalUiRouteAction50KG::request||decision.selectedSlot!=slot||
       decision.inputPC!=0x1E3F70||decision.request.kind!=game::OriginalUiRouteKind50KG::changeScreen||
       decision.request.sourceScreen!=11||decision.request.sourcePage!=1||
       decision.request.targetScreen!=12||decision.request.targetPage!=1||decision.request.sourcePC!=0x1E4260||
       decision.profileChanges.count!=1)return false;
    const auto& change=decision.profileChanges.values[0];
    if(change.field!=game::OriginalUiProfileField50KG::hallSlot6EC||
       change.operation!=game::OriginalUiProfileWrite50KG::word||change.value!=slot||
       change.replaceMask!=0xffffffffu||change.sourcePC!=0x1E4254)return false;
    storage_->rememberedHall6EC=slot;++storage_->generation;return true;
}
bool OriginalUiRiderProfileOwner50KG::commitHallRender(OriginalUiRiderProfileStamp50KG expected) noexcept {
    if(!hallRecordsReady()||(storage_->common->mode34!=5&&storage_->common->mode34!=2)||
       expected.mode34!=storage_->common->mode34||
       expected.generation!=storage_->generation||storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
    //1E3F40..1E3F4C clears only profile7E4 bit0 after genuine page render.
    storage_->bit0_7E4=false;++storage_->generation;return true;
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
bool OriginalUiRiderProfileOwner50KG::riderSelectionFacts(std::uint32_t rider,
    RiderSelectionFacts& out) const noexcept {
    if(!storage_||!storage_->readable()||rider>=storage_->authored.size()||
       !storage_->authored[rider].present)return false;
    out={rider,storage_->unlock10FAC[rider],storage_->hidden0730[rider]};return true;
}
bool OriginalUiRiderProfileOwner50KG::courseRiderSelectionSnapshot(RiderSelectionFacts& out) const noexcept {
    if(!storage_||!storage_->readable()||(storage_->common->mode34!=0&&storage_->common->mode34!=2))return false;
    const auto rider=std::uint32_t(storage_->packed1125D>>3);RiderSelectionFacts facts;
    const auto remembered=storage_->common->mode34==2?3u:0u;
    if(rider!=storage_->remembered[remembered]||rider!=storage_->comparison100C0||
       !riderSelectionFacts(rider,facts)||!facts.unlock10FAC)return false;
    out=facts;return true;
}
bool OriginalUiRiderProfileOwner50KG::selectionSnapshotForMode(std::uint32_t mode,
    game::OriginalUiRiderSelectionProfile50KG& out) const noexcept {
    if(!storage_||!storage_->common||!storage_->common->known||mode>2)return false;const auto& s=*storage_;
    game::OriginalUiRiderSelectionProfile50KG result;result.ownerReady=true;result.mode34=mode;
    result.remembered6F0=s.remembered[0];result.remembered6F4=s.remembered[1];
    result.remembered6F8=s.remembered[2];result.remembered6FC=s.remembered[3];
    for(unsigned rider=0;rider<27;++rider){auto& row=result.riders[rider];
        row.ownerReady=true;row.unlock10FAC=s.unlock10FAC[rider];row.hidden730=s.hidden0730[rider];
        row.careerByteOwnerReady=true;row.career1938=s.tournament.careers[rider].pods[rider].medals1938[0];
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
    const auto& career=s.tournament.careers[rider];
    result.current1920=career.pods[rider].abilities1920;result.maximum0408=s.authored[rider].maximum0408;
    result.medal1938=career.pods[rider].medals1938;result.careerTrack219C=career.progress219C&31u;out=result;return true;
}
bool OriginalUiRiderProfileOwner50KG::colors(std::uint32_t rider,OriginalUiRiderProfileColors50KG& out) const noexcept {
    if(!storage_||!storage_->readable()||rider>=27||!storage_->authored[rider].present)return false;
    out=storage_->authored[rider].colors;return true;
}
bool OriginalUiRiderProfileOwner50KG::canResetRiderCareer1800E0(
    std::uint32_t rider,OriginalUiRiderProfileStamp50KG expected) const noexcept {
    if(!storage_||!storage_->readable()||storage_->common->mode34!=2||expected.mode34!=2||
       expected.generation!=storage_->generation||
       storage_->generation==std::numeric_limits<std::uint64_t>::max()||
       storage_->tournamentResultArmed||rider>=storage_->authored.size()||
       !storage_->authored[rider].present||!storage_->saveProfile.namesReady())return false;
    const auto& s=*storage_;
    //1EA870 requires prompt42C bit2. Its actual1EAFB0 producer requires
    //mode2, an unlocked selected rider, and that career's diagonal medal0.
    //The session, not this numeric predicate, owns the selected4C4/result2.
    return s.unlock10FAC[rider]!=0&&s.tournament.careers[rider].pods[rider].medals1938[0]!=0;
}
bool OriginalUiRiderProfileOwner50KG::resetRiderCareer1800E0(
    std::uint32_t rider,OriginalUiRiderProfileStamp50KG expected) noexcept {
    if(!canResetRiderCareer1800E0(rider,expected))return false;
    auto& s=*storage_;
    auto next=s.tournament.careers[rider];
    auto nextName=s.saveProfile.careerNames_[rider];
    //180108..194: all27 pod rows, including off-diagonal/absent pods.
    //Base0180 is the retained constructor/PHY table, never current abilities.
    for(std::size_t pod=0;pod<next.pods.size();++pod){
        next.pods[pod].abilities1920=s.tournament.base0180[pod];
        next.pods[pod].medals1938.fill(0);
        next.pods[pod].remainingPrizes1946=s.constructorPrizeHalves100C8;
    }
    //180148..154 repeatedly clears bit5;1801A8..1CC then clears bits0..4.
    //Both opaque high bits survive, rather than replacing the whole byte.
    next.progress219C=static_cast<std::uint8_t>(next.progress219C&0xc0u);
    next.kills2198=0;next.earned2190=0;next.unspent2194=0; //18019C..1A4.
    //1801B4/1C4/1C8: strcpy(name219D, empty3A89A0) writes byte0 ONLY.
    //The existing value owner preserves suffix bytes and their known extent.
    if(!nextName.replaceCString({}))return false;
    static_assert(std::is_nothrow_copy_assignable_v<decltype(next)>);
    static_assert(std::is_nothrow_copy_assignable_v<decltype(nextName)>);
    //Single nonfailing native publication, with no callbacks/IO or rejection
    //below. All other careers, source selection and save metadata survive.
    s.tournament.careers[rider]=next;
    s.saveProfile.careerNames_[rider]=nextName;
    ++s.generation; //Native observer invalidation, not original dirty/save state.
    return true;
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
    game::OriginalUiCourseRememberedProfile50KG p;p.ownerReady=true;p.mode34=s.common->mode34;p.mawhonicRylothTest=s.testRider==9;p.wanMbTest=s.testRider==16;p.gasganoScTest=s.gasganoScTest;p.tzidikTcTest=s.tzidikTcTest;p.scorchRcTest=s.scorchRcTest;p.sebulbaTbTest=s.sebulbaTbTest;p.kraidGbTest=s.kraidGbTest;p.youngAnakinMcTest=s.youngAnakinMcTest;
    p.course700=s.rememberedCourse[0];p.laps704=s.rememberedCourse[1];p.entrants708=s.rememberedCourse[2];p.subtype70C=s.rememberedCourse[3];
    p.course710=s.rememberedCourse[4];p.entrants714=s.rememberedCourse[5];p.laps718=s.rememberedCourse[6];
    p.packed1125E=s.packed1125E;p.cardFlag100BC=s.saveProfile.dirty100BCBit0();p.availability11018=s.common->availability11018;
    p.nativeBetaStorage=s.betaStatus!=BetaPersistenceStatus::notStarted&&s.betaStatus!=BetaPersistenceStatus::disabledFixture;
    p.planet0Primary79C=s.planetColors[0].primary79C;p.planet0Secondary7A0=s.planetColors[0].secondary7A0;p.courseOrder100C4=s.courseOrder100C4;out=p;return true;
}
bool OriginalUiRiderProfileOwner50KG::riderConfirmSnapshot(std::uint8_t& packed,std::uint32_t& comparison)const noexcept{
    if(!storage_||!storage_->readable())return false;packed=storage_->packed1125D;comparison=storage_->comparison100C0;return true;
}
bool OriginalUiRiderProfileOwner50KG::commitRiderConfirmToCourse(const game::OriginalUiRouteDecision50KG& d,OriginalUiRiderProfileStamp50KG expected)noexcept{
    RiderSelectionFacts rider;
    if(!storage_||!storage_->readable()||storage_->common->mode34!=0||expected.mode34!=0||expected.generation!=storage_->generation||
       storage_->generation==std::numeric_limits<std::uint64_t>::max()||!d.admitted||!d.profileProposalComplete||d.blockers||
       d.action!=game::OriginalUiRouteAction50KG::request||d.request.sourceScreen!=5||d.request.sourcePage!=1||
       d.selectedRider<0||!riderSelectionFacts(static_cast<std::uint32_t>(d.selectedRider),rider)||!rider.unlock10FAC||
       d.profileChanges.count!=3)return false;
    game::OriginalUiRiderContext50KG c;c.mode34=0;c.selectedRider=d.selectedRider;c.playerIndex=0;c.packed1125C=storage_->common->playerBits1125C;
    c.unlockOwnerReady=true;c.selectedUnlockWord10FAC=rider.unlock10FAC;c.comparisonRider100C0=storage_->comparison100C0;
    c.admission.sourceControllerReady=true;c.admission.selectionOwnerReady=true;c.admission.profileOwnerReady=true;c.admission.nativePageMasks[6]=1u<<1;
    game::OriginalUiRouteDecision50KG exact;
    if(game::dispatchOriginalUiRider50KG(0,game::OriginalUiRouteInput50KG::activate,c,exact)!=game::OriginalUiRouteStatus50KG::ok||
       exact.inputPC!=d.inputPC||exact.selectedSlot!=d.selectedSlot||exact.selectedRider!=d.selectedRider||
       exact.request.kind!=d.request.kind||exact.request.targetScreen!=d.request.targetScreen||exact.request.targetPage!=d.request.targetPage||
       exact.request.sourcePC!=d.request.sourcePC||exact.profileChanges.count!=3)return false;
    for(std::size_t i=0;i<3;++i)if(!sameChange(exact.profileChanges.values[i],d.profileChanges.values[i]))return false;
    storage_->remembered[0]=d.selectedRider;storage_->packed1125D=static_cast<std::uint8_t>((storage_->packed1125D&7u)|(d.selectedRider<<3));
    storage_->comparison100C0=d.selectedRider;++storage_->generation;
    if(storage_->betaSettings&&(storage_->betaSettings->flags11050&1u))(void)saveNativeBetaProgress();return true;
}
bool OriginalUiRiderProfileOwner50KG::canPrepareRiderConfirmToTournament(const game::OriginalUiRouteDecision50KG& d,
    OriginalUiRiderProfileStamp50KG expected)const noexcept {
    if(d.admitted||d.action!=game::OriginalUiRouteAction50KG::unavailable||
       d.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::destinationOwner))return false;
    // A local shape projection for shared validation, not an admission receipt.
    // The real manager, retained destination and private commit remain mandatory.
    auto admitted=d;admitted.admitted=true;admitted.action=game::OriginalUiRouteAction50KG::request;admitted.blockers=0;
    return canCommitRiderConfirmToTournament(admitted,expected);
}
bool OriginalUiRiderProfileOwner50KG::canCommitRiderConfirmToTournament(const game::OriginalUiRouteDecision50KG& d,
    OriginalUiRiderProfileStamp50KG expected)const noexcept {
    if(!storage_||!storage_->readable()||storage_->common->mode34!=2||expected.mode34!=2||expected.generation!=storage_->generation||
       storage_->generation==std::numeric_limits<std::uint64_t>::max()||!d.admitted||!d.profileProposalComplete||d.blockers||
       d.action!=game::OriginalUiRouteAction50KG::request||d.request.sourceScreen!=5||d.request.sourcePage!=1||
       d.selectedRider<0||static_cast<std::size_t>(d.selectedRider)>=storage_->authored.size()||
       !storage_->authored[static_cast<std::size_t>(d.selectedRider)].present||
       d.profileChanges.count!=3||storage_->tournamentResultArmed)return false;
    game::OriginalUiRiderContext50KG c;c.mode34=2;c.selectedRider=d.selectedRider;c.playerIndex=0;
    c.packed1125C=storage_->common->playerBits1125C;c.unlockOwnerReady=true;
    c.selectedUnlockWord10FAC=storage_->unlock10FAC[d.selectedRider];c.comparisonRider100C0=storage_->comparison100C0;
    c.admission.sourceControllerReady=true;c.admission.selectionOwnerReady=true;c.admission.profileOwnerReady=true;c.admission.nativePageMasks[9]=1u<<1;
    game::OriginalUiRouteDecision50KG exact;
    if(game::dispatchOriginalUiRider50KG(0,game::OriginalUiRouteInput50KG::activate,c,exact)!=game::OriginalUiRouteStatus50KG::ok||
       exact.inputPC!=d.inputPC||exact.selectedSlot!=d.selectedSlot||exact.selectedRider!=d.selectedRider||
       exact.request.kind!=d.request.kind||exact.request.targetScreen!=d.request.targetScreen||exact.request.targetPage!=d.request.targetPage||
       exact.request.sourcePC!=d.request.sourcePC||exact.profileChanges.count!=3)return false;
    for(std::size_t i=0;i<3;++i)if(!sameChange(exact.profileChanges.values[i],d.profileChanges.values[i]))return false;
    return true;
}
bool OriginalUiRiderProfileOwner50KG::commitRiderConfirmToTournament(const game::OriginalUiRouteDecision50KG& d,
    OriginalUiRiderProfileStamp50KG expected)noexcept {
    if(!canCommitRiderConfirmToTournament(d,expected))return false;
    //1EA54C/1EA58C/1EA5A8. This chooses a genuine career; no progress write.
    storage_->remembered[3]=d.selectedRider;storage_->packed1125D=static_cast<std::uint8_t>((storage_->packed1125D&7u)|(d.selectedRider<<3));
    storage_->comparison100C0=d.selectedRider;++storage_->generation;
    if(storage_->betaSettings&&(storage_->betaSettings->flags11050&1u))(void)saveNativeBetaProgress();return true;
}
bool OriginalUiRiderProfileOwner50KG::resultsSubtype11258(std::uint32_t& out)const noexcept {
    if(!storage_||!storage_->readable())return false;
    out=storage_->subtype11258;return true;
}
bool OriginalUiRiderProfileOwner50KG::resultsRecordBit7E4(bool& out)const noexcept {
    if(!storage_||!storage_->readable())return false;
    out=storage_->bit0_7E4;return true; // Actual represented bit only; no upper-byte inference.
}
bool OriginalUiRiderProfileOwner50KG::resultsCareerPageFields1C4CD8(
    ResultsCareerPageFields& out)const noexcept {
    if(!storage_||!storage_->readable())return false;
    const auto& s=*storage_;const auto career=s.comparison100C0;
    const auto selected=std::uint32_t(s.packed1125F&15u);
    if(career>=s.tournament.careers.size()||!s.authored[career].present||selected>=s.trackText.size())return false;
    ResultsCareerPageFields next;
    //187B10's unmatched scan returns0. Do not use progress or quietly clamp
    //the selected source track into a different course-order row.
    for(std::size_t i=0;i<s.courseOrder100C4.size();++i)
        if(s.courseOrder100C4[i]==selected){next.order=static_cast<std::uint32_t>(i);break;}
    const auto& diagonal=s.tournament.careers[career].pods[career];
    next.priorMedal1938=diagonal.medals1938[next.order];
    next.remaining1946=diagonal.remainingPrizes1946[next.order];
    next.title1127C=s.trackText[selected].title1127C;
    next.par100E8=s.resultsPar100E8[next.order];
    next.placePercent102D0=s.resultsPlacePercent102D0;
    // Native bounded8 backing: index0 is not source100C8 and is never read.
    for(std::size_t i=0;i<7;++i)next.killPercent100CC[i+1]=s.resultsKillPercent100CC[next.order][i];
    next.stamp=stamp();if(next.title1127C<0)return false;
    out=next;return true;
}
bool OriginalUiRiderProfileOwner50KG::resultsCareerProgress219C(std::uint8_t& out)const noexcept {
    if(!storage_||!storage_->readable())return false;
    const auto& s=*storage_;const auto career=s.comparison100C0;
    if(career>=s.tournament.careers.size()||!s.authored[career].present)return false;
    out=s.tournament.careers[career].progress219C&31u;return true;
}
bool OriginalUiRiderProfileOwner50KG::resultsSelectedOrder187B10(std::uint32_t& out)const noexcept {
    if(!storage_||!storage_->readable())return false;
    const auto& s=*storage_;const auto selected=std::uint32_t(s.packed1125F&15u);
    std::uint32_t order=0; //Original unmatched13-entry scan returns0.
    for(std::size_t i=0;i<s.courseOrder100C4.size();++i)
        if(s.courseOrder100C4[i]==selected){order=static_cast<std::uint32_t>(i);break;}
    out=order;return true;
}
bool OriginalUiRiderProfileOwner50KG::unlockFacts187E30(game::OriginalUnlockFacts50PD& out)const noexcept {
    if(!storage_||!storage_->readable()||!hallRecordsReady())return false;
    const auto& s=*storage_;const auto track=std::uint32_t(s.packed1125F&15u);
    if(track>=s.trackText.size()||s.comparison100C0>=s.tournament.careers.size())return false;
    game::OriginalUnlockFacts50PD f;
    f.unlock10FAC=s.unlock10FAC;f.word7E8=s.word7E8;f.rival11280=s.trackText[track].rival11280;
    f.career100C0=s.comparison100C0;
    if(!resultsSelectedOrder187B10(f.order187B10))return false;
    for(std::size_t c=0;c<s.tournament.careers.size();++c)
        f.diagonalMedals[c]=s.tournament.careers[c].pods[c].medals1938;
    for(std::size_t i=0;i<13;++i){
        const auto& d=s.defaultHall7EC.courses[i];const auto& l=s.hallRecords.courses[i];
        f.defaultLap7EC[i]=d.lap102F0[0];f.liveLap102F0[i]=l.lap102F0[0];
        f.defaultRace838[i]=d.race1033C[0];f.liveRace1033C[i]=l.race1033C[0];
        f.defaultKnockouts884[i]=d.knockouts10388[0];f.liveKnockouts10388[i]=l.knockouts10388[0];
    }
    f.availability11018=s.common->availability11018;
    out=f;return true;
}
bool OriginalUiRiderProfileOwner50KG::unlockSpritePods209BE0(std::uint32_t& out)const noexcept {
    if(!storage_||!storage_->readable())return false;
    const auto& s=*storage_;std::uint32_t mask=0;
    for(std::size_t o=4;o<=12;++o){
        if(o>=s.courseOrder100C4.size())return false;
        const auto course=std::size_t(s.courseOrder100C4[o]);
        if(course>=s.trackText.size())return false;
        const auto rival=s.trackText[course].rival11280;
        if(rival<1||rival>26)return false;
        mask|=1u<<rival;
    }
    for(const auto pod:{3u,1u,14u,25u,21u,26u})mask|=1u<<pod;
    out=mask;return true;
}
bool OriginalUiRiderProfileOwner50KG::raceAcknowledgementFacts1AB5BC(game::OriginalRaceAckFacts50PD& out)const noexcept {
    game::OriginalUnlockFacts50PD u;
    if(!unlockFacts187E30(u))return false;
    const auto& s=*storage_;
    game::OriginalRaceAckFacts50PD f;
    f.mode34=s.common->mode34;f.order187B10=u.order187B10;f.boundary102CC=s.tournament.tournamentBoundary102CC;
    f.record7E4=s.bit0_7E4;
    //1AB730 reads M[C][C][s1] with s1 = order; only taken when order+1 == 13.
    f.finalMedal=u.order187B10<13?u.diagonalMedals[u.career100C0][u.order187B10]:0;
    f.unlockPending187E30=game::evaluateOriginalUnlocks187E30(u).any();
    out=f;return true;
}
bool OriginalUiRiderProfileOwner50KG::grantUnlock20B480(std::size_t check,std::uint32_t galleryTier,
    OriginalUiRiderProfileStamp50KG expected,OriginalUiRiderProfileStamp50KG& after)noexcept {
    if(!storage_||!storage_->readable()||check>=8||expected.generation!=storage_->generation||
       expected.mode34!=storage_->common->mode34||
       storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
    game::OriginalUnlockFacts50PD f;
    if(!unlockFacts187E30(f))return false;
    // The page grants only a check it found pending at its activation; a
    // grant is refused unless that check is still pending on this profile.
    const auto pending=game::evaluateOriginalUnlocks187E30(f);
    if(!pending.pending[check])return false;
    auto& s=*storage_;
    if(check==7){
        if(galleryTier!=pending.galleryTier)return false;
        //188440: byte11018 = (byte & ~0x60) | ((N & 3) << 5).
        s.common->availability11018=static_cast<std::uint8_t>((s.common->availability11018&0x9fu)|((galleryTier&3u)<<5));
    }else{
        const auto pod=game::originalUnlockPod50PD(f,check);
        if(pod<0||pod>=27)return false;
        s.unlock10FAC[std::size_t(pod)]=1u;
    }
    ++s.generation;after={s.generation,s.common->mode34};
    if(s.betaSettings&&(s.betaSettings->flags11050&1u))(void)saveNativeBetaProgress();
    return true;
}
bool OriginalUiRiderProfileOwner50KG::markAcknowledgementSavePending1AB648(
    OriginalUiRiderProfileStamp50KG expected,OriginalUiRiderProfileStamp50KG& after)noexcept {
    if(!storage_||!storage_->readable()||expected.generation!=storage_->generation||
       expected.mode34!=storage_->common->mode34||storage_->common->mode34==2u)return false;
    storage_->saveProfile.markDirty1AB648();
    after={storage_->generation,storage_->common->mode34};return true;
}
bool OriginalUiRiderProfileOwner50KG::resultsNextCourseTitle187480(std::int32_t& out)const noexcept {
    std::uint8_t progress=0;if(!resultsCareerProgress219C(progress))return false;
    const auto& s=*storage_;const auto next=std::size_t(progress)+1;
    //Original returns null for next>=13. Do not replace it with current title
    //or invent a string for the source's unsupported null-formatting domain.
    if(next>=s.courseOrder100C4.size())return false;
    const auto selected=s.courseOrder100C4[next];
    if(selected>=s.trackText.size()||s.trackText[selected].title1127C<0)return false;
    out=s.trackText[selected].title1127C;return true;
}
bool OriginalUiRiderProfileOwner50KG::tournamentCareerSnapshot(std::uint32_t index,TournamentCareerSnapshot& out)const noexcept{
    if(!storage_||!storage_->readable()||index>=27||!storage_->authored[index].present)return false;
    const auto& c=storage_->tournament.careers[index];TournamentCareerSnapshot p;
    p.career100C0=index;p.earned2190=c.earned2190;p.unspent2194=c.unspent2194;p.kills2198=c.kills2198;
    p.progress219C=c.progress219C;p.availability11018=storage_->common->availability11018;
    p.cardFlag100BC=storage_->saveProfile.dirty100BCBit0();
    p.nativeBetaStorage=storage_->betaStatus!=BetaPersistenceStatus::notStarted&&storage_->betaStatus!=BetaPersistenceStatus::disabledFixture;
    p.medals1938=c.pods[index].medals1938;p.remainingPrizes1946=c.pods[index].remainingPrizes1946;p.stamp=stamp();out=p;return true;
}
bool OriginalUiRiderProfileOwner50KG::selectedGarageProfileSnapshot(GarageProfileSnapshot& out)const noexcept {
    if(!storage_||!storage_->readable()||storage_->common->mode34!=2)return false;
    const auto& s=*storage_;const auto selected=s.comparison100C0;
    // The bounded normal CareerInfo predecessor has a committed selection.
    // Do not manufacture the original -1 fallback for a missing native owner.
    if(selected>=s.authored.size()||!s.authored[selected].present||
       (s.packed1125D>>3)!=selected)return false;
    GarageProfileSnapshot next;const auto before=stamp();
    if(!tournamentCareerSnapshot(selected,next.career))return false;
    next.base0180=s.authored[selected].base0180;
    next.maximum0408=s.authored[selected].maximum0408;
    next.current1920=s.tournament.careers[selected].pods[selected].abilities1920;
    next.colors=s.authored[selected].colors;
    const auto after=stamp();
    if(next.career.stamp.generation!=before.generation||next.career.stamp.mode34!=before.mode34||
       after.generation!=before.generation||after.mode34!=before.mode34)return false;
    out=next;return true;
}
bool OriginalUiRiderProfileOwner50KG::canApplyGaragePurchase1FEB90(
    const GarageProfileSnapshot& expected,const game::OriginalUiGarageState50KG& page)const noexcept {
    if(!storage_||storage_->tournamentResultArmed||!page.known||page.selected4E4>=6||page.available34B0<0||
       storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
    GarageProfileSnapshot actual;
    if(!selectedGarageProfileSnapshot(actual)||actual.career.career100C0==0||
       actual.career.career100C0!=expected.career.career100C0||
       actual.career.stamp.generation!=expected.career.stamp.generation||
       actual.career.stamp.mode34!=expected.career.stamp.mode34||
       actual.career.unspent2194!=expected.career.unspent2194||
       actual.career.earned2190!=expected.career.earned2190||
       actual.career.kills2198!=expected.career.kills2198||
       actual.career.progress219C!=expected.career.progress219C||
       actual.career.cardFlag100BC!=expected.career.cardFlag100BC||
       actual.base0180!=expected.base0180||actual.maximum0408!=expected.maximum0408||
       std::memcmp(actual.current1920.data(),expected.current1920.data(),sizeof(actual.current1920)))return false;
    std::int64_t cost=0;bool changed=false;
    for(std::size_t row=0;row<6;++row){
        std::int32_t base=0,current=0,maximum=0;
        if(!game::convertOriginalUiGarageStat50KG(actual.base0180[row],base)||
           !game::convertOriginalUiGarageStat50KG(actual.current1920[row],current)||
           !game::convertOriginalUiGarageStat50KG(actual.maximum0408[row],maximum)||
           page.base[row]!=base||page.current4E8[row]!=current||page.maximum[row]!=maximum||
           base<0||current<base||maximum<current||maximum>20||page.delta500[row]<base-current||
           page.delta500[row]>maximum-current)return false;
        for(std::int32_t n=1;n<=page.delta500[row];++n)cost+=50*(current+n)+300;
        //1FE374: each sold owned notch refunds half its current full price.
        // Summation across all rows admits mixed buys/sales but no free credits.
        for(std::int32_t n=0;n>page.delta500[row];--n)cost-=(50*(current+n)+300)/2;
        changed=changed||page.delta500[row]!=0;
    }
    return changed&&std::int64_t(actual.career.unspent2194)-cost==page.available34B0;
}
bool OriginalUiRiderProfileOwner50KG::applyGaragePurchase1FEB90(
    const GarageProfileSnapshot& expected,const game::OriginalUiGarageState50KG& page,
    GarageProfileSnapshot& after) noexcept {
    if(!canApplyGaragePurchase1FEB90(expected,page))return false;
    auto next=expected;
    // Exact188E40 conversion: binary32(integer notch)*0x3d4ccccd.
    // 1FEB90 writes all six diagonal lanes, even ones with zero pending delta.
    for(std::size_t row=0;row<6;++row){
        volatile float count=static_cast<float>(page.current4E8[row]+page.delta500[row]);
        volatile float ability=0.05f*count;next.current1920[row]=ability;
    }
    next.career.unspent2194=static_cast<std::uint32_t>(page.available34B0);
    next.career.cardFlag100BC=true;++next.career.stamp.generation;
    auto& s=*storage_;auto& career=s.tournament.careers[next.career.career100C0];
    career.pods[next.career.career100C0].abilities1920=next.current1920; //1FEBF0
    career.unspent2194=next.career.unspent2194;                       //1FEC30
    s.saveProfile.markDirty1AB648();                                //1FEC58 bit0
    ++s.generation;after=next;return true;
}
void OriginalUiRiderProfileOwner50KG::autosaveGaragePurchase() noexcept {
    if(storage_&&storage_->betaSettings&&(storage_->betaSettings->flags11050&1u))
        (void)saveNativeBetaProgress();
}
bool OriginalUiRiderProfileOwner50KG::selectedTournamentCourseSnapshot(
    TournamentCareerSnapshot& career,game::OriginalUiCourseRememberedProfile50KG& course) const noexcept {
    if(!storage_||!storage_->readable()||storage_->common->mode34!=2)return false;
    const auto selected=storage_->comparison100C0;
    if(selected>=storage_->authored.size()||!storage_->authored[selected].present||
       (storage_->packed1125D>>3)!=selected)return false;
    TournamentCareerSnapshot c;game::OriginalUiCourseRememberedProfile50KG p;
    const auto before=stamp();
    if(!tournamentCareerSnapshot(selected,c)||!courseSnapshot(p)||
       c.stamp.generation!=before.generation||c.stamp.mode34!=before.mode34||
       stamp().generation!=before.generation||stamp().mode34!=before.mode34)return false;
    career=c;course=p;return true;
}
bool OriginalUiRiderProfileOwner50KG::selectedTournamentCourseRecordSnapshot(
    TournamentCourseRecordSnapshot& out) const noexcept {
    TournamentCareerSnapshot career;game::OriginalUiCourseRememberedProfile50KG course;
    if(!selectedTournamentCourseSnapshot(career,course)||!storage_||(career.progress219C&31u)>13)return false;
    const auto order=std::min<std::uint32_t>(career.progress219C&31u,12u);
    if(course.courseOrder100C4!=game::originalTournamentCourseOrder50KG)return false;
    TournamentCourseRecordSnapshot row;
    row.slot=order+1;row.course100C4=course.courseOrder100C4[order];
    row.constructorPurse100C8=storage_->constructorPrizeHalves100C8[order];
    // Same constructor percentages consumed by original result1875C0.
    row.killPercent100CC=(order==0||order==6)?std::array<std::int32_t,7>{{30,50,60,70,80,90,100}}:
        std::array<std::int32_t,7>{{20,30,50,60,70,80,100}};
    // Only TA's original PAR is recovered. -1 explicitly means unknown;
    // native Course omits that label/highlight rather than inventing a value.
    row.par100E8=order==0?2:-1;row.stamp=career.stamp;out=row;return true;
}
bool OriginalUiRiderProfileOwner50KG::tournamentPodCareerSnapshot(std::uint32_t career,std::uint32_t pod,
    game::OriginalTournamentPodCareer50KG& out)const noexcept{
    if(!storage_||!storage_->readable()||career>=27||pod>=27||!storage_->authored[career].present||!storage_->authored[pod].present)return false;
    out=storage_->tournament.careers[career].pods[pod];return true;
}
bool OriginalUiRiderProfileOwner50KG::tournamentTextMetadataSnapshot(std::uint32_t career,
    TournamentTextMetadataSnapshot& out)const noexcept {
    if(!storage_||!storage_->readable()||career>=storage_->authored.size()||
       !storage_->authored[career].present)return false;
    const auto& s=*storage_;const auto progress=std::uint8_t(s.tournament.careers[career].progress219C&31u);
    if(progress>s.courseOrder100C4.size()||s.racerNames3C[career]<0)return false;
    // Original1EF230..244 retains progress13 for the completed text branch,
    // but clamps its local course-order index to12. Never read a 14th course.
    const auto order=progress==13?12u:unsigned(progress);
    const auto course=s.courseOrder100C4[order];if(course>=s.trackText.size())return false;
    const auto& text=s.trackText[course];if(text.title1127C<0||text.planet11281<0)return false;
    TournamentTextMetadataSnapshot next;next.career100C0=career;next.racerName3C=s.racerNames3C[career];
    next.courseTitle1127C=text.title1127C;next.planetTitle11281=text.planet11281;
    next.progressIndex=progress;next.course100C4=course;next.stamp=stamp();out=next;return true;
}
bool OriginalUiRiderProfileOwner50KG::resultsRacerName3C(std::uint32_t racer,
    std::int32_t& name)const noexcept {
    if(!storage_||!storage_->readable()||racer>=storage_->racerNames3C.size()||
       storage_->racerNames3C[racer]<0)return false;
    name=storage_->racerNames3C[racer];return true;
}
bool OriginalUiRiderProfileOwner50KG::resultsNameCourse1C9C80(std::uint32_t& course)const noexcept {
    if(!storage_||!storage_->readable())return false;
    course=storage_->packed1125F&15u;return true;
}
bool OriginalUiRiderProfileOwner50KG::resultsFirstName1C9CA4(
    OriginalPersistedProfileName50KG& out)const noexcept {
    if(!storage_||!storage_->readable())return false;
    // Source1C9CAC: mode2 uses actual career100C0/219D; other modes use
    // player0's10F68. Existing owned-name readers preserve known NUL prefix.
    return storage_->common->mode34==2u?careerName219D(storage_->comparison100C0,out):
        playerName10F68(0,out);
}
game::LapRecordQueryResult50ID OriginalUiRiderProfileOwner50KG::resultsLapQuery1861A0(
    std::uint32_t course,float seconds)const noexcept {
    using S=game::LapRecordQueryStatus50ID;
    if(!storage_||!storage_->readable()||!hallRecordsReady())return {S::Storage,0,0x1861a0u};
    const auto& s=*storage_;
    const game::LapRecordConfig50ID config{s.subtype11258,s.common->playerBits1125C,
        static_cast<std::uint16_t>(s.common->playerBits1125C|(std::uint16_t(s.packed1125D)<<8))};
    return game::queryLapRecord50ID(config,
        game::LapRecordTableView50ID<game::OriginalUiHallCourseRecords50KG>{s.hallRecords.courses.data(),s.hallRecords.courses.size()},
        course,seconds);
}
game::LapRecordQueryResult50ID OriginalUiRiderProfileOwner50KG::resultsRaceQuery186280(
    std::uint32_t course,float seconds)const noexcept {
    using S=game::LapRecordQueryStatus50ID;
    if(!storage_||!storage_->readable()||!hallRecordsReady())return {S::Storage,0,0x186280u};
    const auto& s=*storage_;
    const game::LapRecordConfig50ID config{s.subtype11258,s.common->playerBits1125C,
        static_cast<std::uint16_t>(s.common->playerBits1125C|(std::uint16_t(s.packed1125D)<<8))};
    return game::queryRaceRecord50ID(config,
        game::LapRecordTableView50ID<game::OriginalUiHallCourseRecords50KG>{s.hallRecords.courses.data(),s.hallRecords.courses.size()},
        course,seconds);
}
game::LapRecordQueryResult50ID OriginalUiRiderProfileOwner50KG::resultsKnockoutQuery1863B0(
    std::uint32_t course,std::uint32_t observedTimingCD0)const noexcept {
    using S=game::LapRecordQueryStatus50ID;
    if(!storage_||!storage_->readable()||!hallRecordsReady())return {S::Storage,0,0x1863b0u};
    const auto& s=*storage_;
    const game::LapRecordConfig50ID config{s.subtype11258,s.common->playerBits1125C,
        static_cast<std::uint16_t>(s.common->playerBits1125C|(std::uint16_t(s.packed1125D)<<8))};
    // Read-only qualification of an observed value, never earned CD0 custody.
    return game::queryOriginalHallKnockoutRecord50KG(config,s.hallRecords,course,observedTimingCD0);
}
bool OriginalUiRiderProfileOwner50KG::armTournamentResultForCommittedRace(const CourseRaceSelection& selection,OriginalUiRiderProfileStamp50KG expected,
    const game::LapTimingManager50ID& manager,OriginalUiMainApplication50KG& application)noexcept{
    if(!storage_||!storage_->readable()||storage_->common->mode34!=2||expected.mode34!=2||expected.generation!=storage_->generation||
       storage_->tournamentResultArmed||storage_->comparison100C0>=27||selection.rider!=storage_->comparison100C0||
       selection.subtype!=1||selection.laps!=3||selection.entrants!=8||((storage_->common->playerBits1125C>>5)&3u)!=1||
       !application.known||(manager.flags58&3u)!=2u)return false;
    CourseRaceSelection actual;
    if(!courseRaceSnapshot(actual)||!sameCourseRace(actual,selection))return false;
    storage_->tournamentResultArmed=true;storage_->tournamentEndStoreBumps=0;storage_->tournamentLaunchStamp=expected;storage_->tournamentLaunchSelection=selection;
    storage_->tournamentLaunchManager=&manager;storage_->tournamentLaunchApplication=&application;return true;
}
bool OriginalUiRiderProfileOwner50KG::commitTournamentRaceResult(const game::LapTimingManager50ID& manager,
    const game::OriginalTournamentResultRacer50KG* racers,std::size_t count,std::uint32_t humans,
    const CourseRaceSelection& launch,OriginalUiRiderProfileStamp50KG expected,
    OriginalUiMainApplication50KG& application,game::OriginalTournamentResultReceipt50KG& receipt)noexcept{
    receipt={};
    if(storage_)std::fprintf(stderr,"tournament_commit_check expected_gen=%llu bumps=%llu gen=%llu armed=%d launch_gen=%llu manager=%d app=%d flags58=%u count=%zu entrants=%u pod=%u rider=%u class=%u\n",
        static_cast<unsigned long long>(expected.generation),static_cast<unsigned long long>(storage_->tournamentEndStoreBumps),
        static_cast<unsigned long long>(storage_->generation),int(storage_->tournamentResultArmed),
        static_cast<unsigned long long>(storage_->tournamentLaunchStamp.generation),int(storage_->tournamentLaunchManager==&manager),
        int(storage_->tournamentLaunchApplication==&application),unsigned(manager.flags58),count,unsigned(launch.entrants),
        racers?unsigned(racers[0].pod60):0u,unsigned(launch.rider),racers?unsigned(racers[0].class58):0u);
    if(!storage_||!storage_->readable()||storage_->common->mode34!=2||expected.mode34!=2||
       expected.generation+storage_->tournamentEndStoreBumps!=storage_->generation||
       storage_->generation==std::numeric_limits<std::uint64_t>::max()||!storage_->tournamentResultArmed||!application.known||
       storage_->tournamentLaunchStamp.generation!=expected.generation||storage_->tournamentLaunchStamp.mode34!=expected.mode34||
       storage_->tournamentLaunchManager!=&manager||storage_->tournamentLaunchApplication!=&application||
       !sameCourseRace(storage_->tournamentLaunchSelection,launch)||(manager.flags58&3u)!=3u||
       humans!=((storage_->common->playerBits1125C>>5)&3u)||count!=launch.entrants||!racers||
       racers[0].pod60!=launch.rider||racers[0].class58!=0)return false;
    CourseRaceSelection current;
    if(!courseRaceSnapshot(current)||!sameCourseRace(current,launch))return false;
    receipt=game::applyOriginalTournamentResult50KG(storage_->tournament,storage_->comparison100C0,storage_->packed1125F,
        racers,count,humans,storage_->common->availability11018,application.rawRandom308030);
    if(receipt.status!=game::OriginalTournamentResultStatus50KG::complete){
        std::fprintf(stderr,"tournament_commit_receipt status=%u pc=%x\n",unsigned(receipt.status),receipt.sourcePC);return false;}
    storage_->saveProfile.markDirty1AB648(); //actual1AB648 AFTER187D80.
    storage_->tournamentResultArmed=false;storage_->tournamentLaunchManager=nullptr;storage_->tournamentLaunchApplication=nullptr;
    ++storage_->generation;
    if(storage_->betaSettings&&(storage_->betaSettings->flags11050&1u))
        (void)saveNativeBetaProgress(); // Real result already committed; IO failure never re-awards it.
    return true;
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
bool OriginalUiRiderProfileOwner50KG::commitCourseMode2Entry(
    const game::OriginalUiCourseSelectionData50KG& data,
    const game::OriginalUiCourseMode2EntryPlan50KG& plan,OriginalUiRiderProfileStamp50KG expected) noexcept {
    if(!storage_||!storage_->readable()||storage_->common->mode34!=2||expected.mode34!=2||
       expected.generation!=storage_->generation||storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
    TournamentCareerSnapshot career;game::OriginalUiCourseRememberedProfile50KG remembered;
    if(!selectedTournamentCourseSnapshot(career,remembered)||(career.cardFlag100BC&&!career.nativeBetaStorage)||
       (career.progress219C&31u)>=13)return false;
    for(const auto medal:career.medals1938)if(medal>8)return false;
    game::OriginalUiCourseMode2EntryInput50KG input;
    input.mode34=2;input.career100C0=career.career100C0;input.progress219C=career.progress219C;
    game::OriginalUiCourseMode2EntryPlan50KG exact;
    if(game::planOriginalUiCourseMode2Entry50KG(data,input,exact)!=game::OriginalUiCourseSelectionStatus50KG::ok||
       plan.sourcePC!=exact.sourcePC||plan.courseSlot14E4!=exact.courseSlot14E4||
       plan.subtypeSlot14F4!=exact.subtypeSlot14F4||plan.tier14E8!=exact.tier14E8||
       plan.selectedControl.controlId!=exact.selectedControl.controlId||
       plan.selectedControl.planet40!=exact.selectedControl.planet40||
       plan.selectedControl.course44!=exact.selectedControl.course44||
       !sameChange(plan.planetProfileChange,exact.planetProfileChange))return false;
    // Original 1DA618: only this masked planet lane changes. No remembered
    // mode0 values, laps/entrants, medals, availability, money or progress.
    const auto& change=exact.planetProfileChange;
    storage_->packed1125E=static_cast<std::uint8_t>((storage_->packed1125E&~change.replaceMask)|(change.value&change.replaceMask));
    ++storage_->generation;return true;
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
        static_cast<std::uint16_t>(s.common->playerBits1125C|(std::uint16_t(s.packed1125D)<<8)),s.common->mode34};
    // Runtime extent always comes from the committed source count lanes.
    // rememberedCourse[2] is a menu preference, consumed only by subtype2;
    // changing back to subtype1/3 must not inherit its smaller live roster.
    out.entrants=((out.packed1125C>>5)&3u)+((out.packed1125C>>7)&15u);
    return true;
}
// Actual racer-manager1AD520 prefix runs at native cold entry after Course
// request validation, or again for an explicit native Restart. This does NOT
// certify the broader counted Race constructor. It changes the SAME1125C/1125D owner, never menu memory
// or the immutable outgoing Course receipt. The source subtype3 branch
//1AD5A4..5DC clears opponent bits before it constructs its single live human.
bool OriginalUiRiderProfileOwner50KG::commitRacerManager1AD520(
    const CourseRaceSelection& expected,CourseRaceSelection& actual)noexcept {
    CourseRaceSelection current;
    if(!storage_||!storage_->readable()||!courseRaceSnapshot(current)||
       !sameCourseRace(current,expected)||current.subtype<1||current.subtype>3||
       ((current.packed1125C>>5u)&3u)!=1u)return false;
    if(current.subtype==3u){
        if(storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
        const auto packed=std::uint16_t(current.packed1125C&~std::uint16_t(0x780u));
        storage_->common->playerBits1125C=static_cast<std::uint8_t>(packed);
        storage_->packed1125D=static_cast<std::uint8_t>(packed>>8u);
        ++storage_->generation;
        current.packed1125C=packed;current.entrants=1u;
    }
    actual=current;return true;
}
bool OriginalUiRiderProfileOwner50KG::commitCourseLaunch(const game::OriginalUiRouteDecision50KG& d,
    const game::OriginalUiCourseContext50KG& source,OriginalUiRiderProfileStamp50KG expected)noexcept{
    RiderSelectionFacts rider;
    const bool tournament=source.mode34==2;
    const std::size_t writes=tournament?4u:8u;
    if(!storage_||!storage_->readable()||(source.mode34!=0&&!tournament)||
       storage_->common->mode34!=source.mode34||expected.mode34!=source.mode34||
       expected.generation!=storage_->generation||storage_->generation==std::numeric_limits<std::uint64_t>::max()||
       !d.admitted||!d.profileProposalComplete||d.blockers||d.action!=game::OriginalUiRouteAction50KG::request||
       d.request.sourceScreen!=6||d.request.sourcePage!=1||d.profileChanges.count!=writes||
       !courseRiderSelectionSnapshot(rider))return false;
    // Mode2 retains the authenticated selected Career predecessor. It does not
    // reinterpret mode0, unlock careers, advance progress or touch save data.
    if(source.subtypeSlot14F4<1||source.subtypeSlot14F4>3||
       (tournament&&source.subtypeSlot14F4!=1)||
       ((storage_->common->playerBits1125C>>5)&3)!=1)return false;
    if(tournament){
        TournamentCareerSnapshot career;game::OriginalUiCourseRememberedProfile50KG remembered;
        if(!selectedTournamentCourseSnapshot(career,remembered)||career.career100C0!=rider.rider)return false;
        const auto progress=std::uint32_t(career.progress219C&31u);
        // Same next-course mapping as the actual mode2 entry and widgets.
        // Earnings and unsaved native progress are not fresh-profile gates:
        // the genuine selected career must be able to launch its next course.
        // Completed careers remain outside this bounded continuation route.
        if(progress>=remembered.courseOrder100C4.size()||
           (career.cardFlag100BC&&!career.nativeBetaStorage)||
           source.courseSlot14E4!=progress+1u||
           source.selectedControlWord44!=remembered.courseOrder100C4[progress]||
           source.packed1125C!=std::uint16_t(storage_->common->playerBits1125C|(std::uint16_t(storage_->packed1125D)<<8)))return false;
        for(const auto medal:career.medals1938)if(medal>8)return false;
    }else if(source.laps14EC<1||source.laps14EC>25||source.entrants14F0<1||source.entrants14F0>8)return false;
    auto c=source;c.admission={};c.admission.sourceControllerReady=true;
    c.admission.selectionOwnerReady=true;c.admission.profileOwnerReady=true;
    game::OriginalUiRouteDecision50KG exact;
    if(game::dispatchOriginalUiCourse50KG(0,game::OriginalUiRouteInput50KG::activate,c,exact)!=game::OriginalUiRouteStatus50KG::unavailable||
       exact.blockers!=static_cast<std::uint32_t>(game::OriginalUiRouteBlocker50KG::courseLaunchOwner)||
       exact.inputPC!=d.inputPC||exact.selectedSlot!=d.selectedSlot||exact.request.kind!=d.request.kind||
       exact.request.targetScreen!=d.request.targetScreen||exact.request.targetPage!=d.request.targetPage||
       exact.request.sourcePC!=d.request.sourcePC||exact.profileChanges.count!=writes)return false;
    for(std::size_t i=0;i<writes;++i)if(!sameChange(exact.profileChanges.values[i],d.profileChanges.values[i]))return false;
    auto& s=*storage_;auto packed=std::uint16_t(s.common->playerBits1125C|(std::uint16_t(s.packed1125D)<<8));
    // Exact source field writes, preserving all unrelated rider and packed bits.
    s.packed1125F=static_cast<std::uint8_t>((s.packed1125F&0xf0u)|(c.selectedControlWord44&15u));
    packed=game::originalCourseLaunchPackedMode050KG(packed,tournament?1u:c.subtypeSlot14F4,
        tournament?3u:c.laps14EC,tournament?8u:c.entrants14F0);
    s.common->playerBits1125C=static_cast<std::uint8_t>(packed);s.packed1125D=static_cast<std::uint8_t>(packed>>8);
    s.subtype11258=tournament?1u:c.subtypeSlot14F4;
    if(!tournament){s.rememberedCourse[0]=c.courseSlot14E4;s.rememberedCourse[1]=c.laps14EC;
        s.rememberedCourse[2]=c.entrants14F0;s.rememberedCourse[3]=c.subtypeSlot14F4;}
    ++s.generation;return true;
}
bool OriginalUiRiderProfileOwner50KG::commitCourseBack(const game::OriginalUiRouteDecision50KG& d,const game::OriginalUiCourseContext50KG& source,
    OriginalUiRiderProfileStamp50KG expected)noexcept{
    if(!storage_||!storage_->readable()||storage_->common->mode34!=0||expected.mode34!=0||source.mode34!=0||
       expected.generation!=storage_->generation||storage_->generation==std::numeric_limits<std::uint64_t>::max()||
       !d.admitted||!d.profileProposalComplete||d.blockers||d.action!=game::OriginalUiRouteAction50KG::request||
       d.request.sourceScreen!=6||d.request.sourcePage!=1||d.profileChanges.count!=4||
       source.subtypeSlot14F4<1||source.subtypeSlot14F4>3||
       source.laps14EC<1||source.laps14EC>25||source.entrants14F0<1||source.entrants14F0>8)return false;
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

bool OriginalUiRiderProfileOwner50KG::careerName219D(std::uint32_t career,
    OriginalPersistedProfileName50KG& out)const noexcept {
    return storage_&&storage_->common&&storage_->common->known&&
        storage_->saveProfile.persistedName(true,career,out);
}
bool OriginalUiRiderProfileOwner50KG::storeResultsNoTime1CA4DC(
    OriginalUiRiderProfileStamp50KG expected)noexcept {
    if(!storage_||!storage_->readable()||expected.generation!=storage_->generation||
       expected.mode34!=storage_->common->mode34||
       storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
    auto& s=*storage_;
    //1CA4C4..4DC reads, clears and sets bit0; all other actual bits survive.
    s.common->flags11054=std::uint8_t((s.common->flags11054&0xfeu)|1u);
    ++s.generation;if(s.tournamentResultArmed)++s.tournamentEndStoreBumps; // Results store before 1AB5BC. // Native observer invalidation, not a persistence receipt.
    return true;
}
bool OriginalUiRiderProfileOwner50KG::storeResultsName1CA528(std::uint32_t mode,
    std::string_view name,OriginalUiRiderProfileStamp50KG expected)noexcept {
    if(!storage_||!storage_->readable()||!storage_->saveProfile.namesReady()||
       (mode!=3u&&mode!=4u)||name.size()>10u||
       expected.generation!=storage_->generation||expected.mode34!=storage_->common->mode34||
       storage_->generation==std::numeric_limits<std::uint64_t>::max())return false;
    auto& s=*storage_;auto& names=s.saveProfile;
    OriginalPersistedProfileName50KG* target=nullptr;
    if(mode==3u&&s.common->mode34==2u){
        if(s.comparison100C0>=names.careerNames_.size())return false;
        target=&names.careerNames_[s.comparison100C0]; //1CA514..528.
    }else target=&names.playerNames_[mode==3u?0u:1u]; //1CA544/1CA56C.
    if(!target->replaceCString(name))return false;
    ++s.generation;if(s.tournamentResultArmed)++s.tournamentEndStoreBumps; // Results store before 1AB5BC. // Native observer invalidation, never a source save receipt.
    return true;
}
bool OriginalUiRiderProfileOwner50KG::playerName10F68(std::uint32_t player,
    OriginalPersistedProfileName50KG& out)const noexcept {
    return storage_&&storage_->common&&storage_->common->known&&
        storage_->saveProfile.persistedName(false,player,out);
}
bool OriginalUiRiderProfileOwner50KG::installNativeReadOwners(
    const std::shared_ptr<const OriginalSaveNativeReadStage50KG>& read,
    OriginalSaveProfileOwner50KG& prepared,OriginalUiMainProfile50KG& common,
    OriginalUiRiderProfileStamp50KG expected,game::OriginalUiGameSettingsProfile50KG& settings,
    game::ControlPresets49D& controls,OriginalUiMainAudio50KG& audio,OriginalSoundMode50KG& sound)noexcept {
    if(!read||!storage_||storage_->common.get()!=&common||!common.known||common.mode34!=4||
       expected.mode34!=4||expected.generation!=storage_->generation||
       storage_->generation==std::numeric_limits<std::uint64_t>::max()||
       !hallRecordsReady()||!storage_->saveProfile.namesReady()||storage_->tournamentResultArmed||
       storage_->comparison100C0>=27||(storage_->packed1125F&15u)>=13)return false;
    auto& s=*storage_;auto& live=s.saveProfile;const auto& stage=read->projection();
    if(s.testRider||s.gasganoScTest||s.tzidikTcTest||s.scorchRcTest||
       s.sebulbaTbTest||s.kraidGbTest||s.youngAnakinMcTest)return false;
    for(const auto course:s.courseOrder100C4)if(course>=13)return false;
    const auto& before=stage.settingsBefore();
    if(settings.known!=before.known||settings.savedGroup6E8!=before.savedGroup6E8||
       settings.soundMode1101C!=before.soundMode1101C||settings.volume11020!=before.volume11020||
       settings.flags11050!=before.flags11050||common.flags11054!=stage.flags11054Before()||
       !game::sameOriginalUiControlsPresets50KG(controls,stage.controlsBefore())||
       prepared.importedRead_.get()!=read.get()||prepared.payloadRecords_.get()!=stage.retainedEnvelope().get()||
       prepared.hallOrigin_!=OriginalSaveProfileOwner50KG::HallOrigin::originalPayload||
       prepared.hallRecordDescendant_||prepared.nativeBetaHall_||prepared.freshHall_||
       !prepared.namesReady()||!prepared.identity191C_||
       (prepared.cardFlags_.value&0xfcu)!=(live.cardFlags_.value&0xfcu)||
       (prepared.cardFlags_.knownMask&0xfcu)!=(live.cardFlags_.knownMask&0xfcu))return false;
    // Positive nonfailure proof is ONLY for the actual retained producer and
    // current mixer/sound attachment. No legacy unbound-transport fallback.
    game::OriginalSoundModeFields50KG soundFields;
    const auto mode=stage.settingsAfter().soundMode1101C;
    if(!audio.retainedGarageStorageCurrent()||!audio.canApplySoundMode18874050KG(mode)||
       !audio.mixer_||audio.mixer_->originalSoundMode50KG()!=&sound||
       !game::projectOriginalSoundMode18874050KG(mode,soundFields))return false;
    const auto& shared=stage.records().sharedFields();
    static_assert(std::is_nothrow_copy_assignable_v<decltype(s.tournament.careers)>);
    static_assert(std::is_nothrow_copy_assignable_v<decltype(live.careerNames_)>);
    static_assert(std::is_nothrow_copy_assignable_v<decltype(s.hallRecords.courses)>);
    static_assert(std::is_nothrow_copy_assignable_v<decltype(live.playerNames_)>);
    static_assert(std::is_nothrow_copy_assignable_v<game::OriginalUiGameSettingsProfile50KG>);
    static_assert(std::is_nothrow_copy_assignable_v<game::OriginalSoundModeFields50KG>);
    // BEGIN single nonfailing store interval: fixed value stores, noexcept
    // accessors/swaps only; no allocation, callbacks, branch-to-failure or
    // destruction of last retained ownership below.
    live.identity191C_=prepared.identity191C_;                         //186D58
    s.tournament.careers=stage.careers();
    live.careerNames_=prepared.careerNames_;
    s.hallRecords.courses=stage.records().courses();
    s.hallRecords.namesSource=nullptr;s.hallRecords.textSource=nullptr;
    live.playerNames_=prepared.playerNames_;live.namesInitialized_=true;
    s.unlock10FAC=shared.unlock10FAC;common.availability11018=shared.availability11018;
    settings=stage.settingsAfter();
    // Copy only persisted words. Existing action rows are NEVER assigned.
    for(std::size_t i=0;i<game::controlPresetSlots49D;++i){
        auto& dst=controls.slots[i];const auto& src=stage.controlsAfter().slots[i];
        dst.configuration11040=src.configuration11040;dst.preset11048=src.preset11048;
        dst.field10F94=src.field10F94;dst.field10F9C=src.field10F9C;dst.field10FA4=src.field10FA4;
        dst.field11030=src.field11030;dst.field11038=src.field11038;
    }
    live.hallOrigin_=prepared.hallOrigin_;live.localizedNames_=nullptr;live.localizedText_=nullptr;
    live.payloadRecords_.swap(prepared.payloadRecords_);live.importedRead_.swap(prepared.importedRead_);
    // New loaded root has no inherited earned descendant. Retain displaced
    // owners in prepared until the existing exclusion/store interval ends.
    live.nativeBetaHall_.swap(prepared.nativeBetaHall_);
    live.freshHall_.swap(prepared.freshHall_);
    live.hallRecordDescendant_.swap(prepared.hallRecordDescendant_);
    live.cardFlags_=prepared.cardFlags_;                              //186D94..DE0
    // EXACT successful188740 effects: projected fields + sound revision,
    // then Main transport control revision. Existing implementation has no
    // mixer/gain/device writes. All predicates were proved above and no
    // preceding store touches them. No bool-returning re-entry/rollback.
    sound.fields_=soundFields;++sound.revision_;++audio.controlRevision_;
    common.flags11054=stage.flags11054After();                        //186DE4..E18
    ++s.generation; //one native publication; no source-selection/reset writes.
    return true;
}
bool OriginalUiRiderProfileOwner50KG::prepareNativeReadMetadata(
    const std::shared_ptr<const OriginalSaveNativeReadStage50KG>& read,
    const OriginalUiMainProfile50KG& common,OriginalUiRiderProfileStamp50KG expected,
    std::unique_ptr<OriginalSaveProfileOwner50KG>& out)const noexcept {
    if(!read||!storage_||storage_->common.get()!=&common||!common.known||
       common.mode34!=4||expected.mode34!=common.mode34||expected.generation!=storage_->generation||
       storage_->generation==std::numeric_limits<std::uint64_t>::max()||
       !hallRecordsReady()||!storage_->saveProfile.namesReady()||storage_->tournamentResultArmed||
       storage_->comparison100C0>=27||(storage_->packed1125F&15u)>=13||
       common.flags11054!=read->projection().flags11054Before())return false;
    const auto& s=*storage_;
    // Test-profile authority cannot be relabeled as an imported original save.
    // Reject this unsupported coexistence; do not silently clear route guards.
    if(s.testRider||s.gasganoScTest||s.tzidikTcTest||s.scorchRcTest||
       s.sebulbaTbTest||s.kraidGbTest||s.youngAnakinMcTest)return false;
    for(const auto course:s.courseOrder100C4)if(course>=13)return false;
    // Metadata now retains the genuine full-read/stage join, not merely a
    // structurally parsed arbitrary envelope. Output is still temporary only.
    return s.saveProfile.preparePayloadMetadata(read,out);
}

}

namespace rr::platform {
namespace {
using BetaBytes=native_beta50kg::Bytes;
struct BetaReader {
 const BetaBytes& bytes;std::size_t at=0;bool valid=true;
 std::uint32_t word(){if(at>bytes.size()||bytes.size()-at<4){valid=false;return 0;}std::uint32_t n=0;for(unsigned i=0;i<4;++i)n|=std::uint32_t(bytes[at++])<<(8*i);return n;}
 std::string name(){std::string n;bool ended=false;for(unsigned i=0;i<21;++i){auto v=word();if(v>255)valid=false;if(!v)ended=true;else if(!ended)n.push_back(static_cast<char>(v));else valid=false;}if(!ended)valid=false;return n;}
 float real(){const auto bits=word();float f=0;std::memcpy(&f,&bits,4);if(!std::isfinite(f)||f<0||std::fpclassify(f)==FP_SUBNORMAL)valid=false;return f;}
};
void betaName(BetaBytes& b,const char* p){bool ended=false;for(unsigned i=0;i<21;++i){if(!ended&&!p[i])ended=true;native_beta50kg::put32(b,ended?0:static_cast<std::uint8_t>(p[i]));}}
void betaReal(BetaBytes& b,float f){std::uint32_t bits;std::memcpy(&bits,&f,4);native_beta50kg::put32(b,bits);}
std::uint32_t betaPhysicsIdentity(const game::OriginalTournamentProfile50KG& p){
 BetaBytes b;for(const auto& pod:p.base0180)for(const auto f:pod)betaReal(b,f);
 for(const auto& pod:p.maximum0408)for(const auto f:pod)betaReal(b,f);
 return native_beta50kg::checksum(b);
}
}
OriginalUiRiderProfileOwner50KG::BetaPersistenceStatus OriginalUiRiderProfileOwner50KG::betaPersistenceStatus()const noexcept {
 return storage_?storage_->betaStatus:BetaPersistenceStatus::notStarted;
}
bool OriginalUiRiderProfileOwner50KG::reloadNativeBetaProgressFromMenu() noexcept {
 if(!storage_||!storage_->common||!storage_->common->known||storage_->common->mode34!=4||
    storage_->tournamentResultArmed||storage_->betaStatus==BetaPersistenceStatus::disabledFixture)return false;
 // The Options menu caller excludes input/update/draw during this synchronous
 // transaction. No source snapshot is reused after the generation changes.
 storage_->betaWritable=false;storage_->betaStatus=BetaPersistenceStatus::notStarted;
 loadNativeBetaProgressAtStartup();return storage_->betaStatus==BetaPersistenceStatus::loaded;
}
void OriginalUiRiderProfileOwner50KG::bindNativeBetaAutosave(std::shared_ptr<const game::OriginalUiGameSettingsProfile50KG> settings) noexcept {
 if(storage_)storage_->betaSettings=std::move(settings);
}
void OriginalUiRiderProfileOwner50KG::cancelTournamentResultForRace(const game::LapTimingManager50ID& manager)noexcept {
 if(!storage_||storage_->tournamentLaunchManager!=&manager)return;
 storage_->tournamentResultArmed=false;storage_->tournamentLaunchManager=nullptr;
 storage_->tournamentLaunchApplication=nullptr;storage_->tournamentLaunchStamp={};
}
void OriginalUiRiderProfileOwner50KG::loadNativeBetaProgressAtStartup() noexcept {
 if(!storage_||storage_->betaStatus!=BetaPersistenceStatus::notStarted)return;
 auto& s=*storage_;using Status=BetaPersistenceStatus;using R=native_beta50kg::Result;
 if(s.testRider||s.gasganoScTest||s.tzidikTcTest||s.scorchRcTest||s.sebulbaTbTest||s.kraidGbTest||s.youngAnakinMcTest){s.betaStatus=Status::disabledFixture;return;}
 try {
  const auto directory=s.betaPath.empty()?native_beta50kg::defaultDirectory():s.betaPath.parent_path();if(directory.empty()){s.betaStatus=Status::unavailable;return;}
  std::error_code ec;std::filesystem::create_directories(directory,ec);if(ec){s.betaStatus=Status::readError;return;}
  s.betaPath=directory/"progress-v1.bin";
  if(s.betaLock.acquire(directory/"progress-v1.lock")!=R::ok){s.betaStatus=Status::locked;return;}
  BetaBytes bytes;const auto result=native_beta50kg::readFile(s.betaPath,bytes);
  if(result==R::missing){s.betaWritable=true;s.betaStatus=Status::ready;return;}
  if(result!=R::ok){s.betaStatus=result==R::invalid?Status::invalid:Status::readError;return;}
  s.betaStatus=Status::invalid;
  BetaReader r{bytes};if(r.word()!=0x42505252u||r.word()!=1u)return;
  const auto length=r.word(),crc=r.word();if(length!=bytes.size()-16||crc!=native_beta50kg::checksum(bytes,16))return;
  // Stage every field before touching any live profile. Authored physics stays
  // owned by the installed original archives, not by the user file.
  auto careers=s.tournament.careers;auto unlock=s.unlock10FAC;auto hidden=s.hidden0730;
  auto remembered=s.remembered;auto courses=s.rememberedCourse;
  if(r.word()!=betaPhysicsIdentity(s.tournament))return;
  const auto availability=r.word();if(availability>255||(availability&31u)<4||(availability&31u)>13)return;
  const auto mainSlot=r.word();if(mainSlot<1||mainSlot>5)return;
  for(auto& v:unlock){v=r.word();if(v>1)return;}
  for(auto& v:hidden){v=r.word();if(v>1)return;}
  for(auto& v:remembered){v=r.word();if(v>=27||!s.authored[v].present)return;}
  for(auto& v:courses){v=r.word();if(v<1||v>13)return;}
  for(auto& career:careers){
   career.earned2190=r.word();career.unspent2194=r.word();career.kills2198=r.word();
   const auto progress=r.word();if(progress>255||(progress&31u)>13)return;career.progress219C=static_cast<std::uint8_t>(progress);
   for(auto& pod:career.pods){
    for(auto& ability:pod.abilities1920)ability=r.real();
    for(auto& medal:pod.medals1938){auto value=r.word();if(value>8)return;medal=static_cast<std::uint8_t>(value);}
    for(auto& prize:pod.remainingPrizes1946){auto value=r.word();if(value>65535)return;prize=static_cast<std::int16_t>(value<32768?static_cast<std::int32_t>(value):static_cast<std::int32_t>(value)-65536);}
   }
  }
  auto careerNames=s.saveProfile.careerNames_;auto playerNames=s.saveProfile.playerNames_;
  for(auto& name:careerNames)if(!name.replaceCString(r.name()))return;
  for(auto& name:playerNames)if(!name.replaceCString(r.name()))return;
  auto hall=std::make_shared<game::OriginalUiHallRecords50KG>();
  for(auto& course:hall->courses){
   for(auto& f:course.lap102F0)f=r.real();for(auto& f:course.race1033C)f=r.real();
   for(auto& n:course.knockouts10388)n=r.word();
   for(auto* names:{&course.lapNames102FC,&course.raceNames10348,&course.knockoutNames10394})
    for(auto& name:*names){const auto text=r.name();if(text.size()>=name.size())return;std::copy(text.begin(),text.end(),name.begin());}
  }
  if(!r.valid||r.at!=bytes.size()||s.generation==std::numeric_limits<std::uint64_t>::max())return;
  s.tournament.careers=careers;s.unlock10FAC=unlock;s.hidden0730=hidden;s.remembered=remembered;s.rememberedCourse=courses;
  s.common->availability11018=static_cast<std::uint8_t>(availability);s.common->savedSlot6E0=mainSlot;
  s.saveProfile.careerNames_=careerNames;s.saveProfile.playerNames_=playerNames;s.saveProfile.namesInitialized_=true;
  // Keep displaced root/descendant ownership until this synchronous load has
  // published. No pending old preparation can match the new root/generation.
  auto displacedHallDescendant=std::move(s.saveProfile.hallRecordDescendant_);
  auto displacedPayload=std::move(s.saveProfile.payloadRecords_);
  auto displacedRead=std::move(s.saveProfile.importedRead_);
  auto displacedBeta=std::move(s.saveProfile.nativeBetaHall_);
  auto displacedFresh=std::move(s.saveProfile.freshHall_);
  s.hallRecords=*hall;s.saveProfile.nativeBetaHall_=std::move(hall);
  s.saveProfile.hallOrigin_=OriginalSaveProfileOwner50KG::HallOrigin::nativeBeta;
  s.saveProfile.localizedNames_=nullptr;s.saveProfile.localizedText_=nullptr;
  s.saveProfile.cardFlags_.value&=static_cast<std::uint8_t>(~1u);

  // Selection/mode/pending-result identities are new lifecycle state; never
  // resurrect a manager, pending award, pointer, or original card receipt.
  s.betaWritable=true;s.betaStatus=Status::loaded;++s.generation;
 }catch(...){s.betaStatus=Status::readError;}
}
bool OriginalUiRiderProfileOwner50KG::saveNativeBetaProgress() noexcept {
 if(!storage_||!storage_->betaWritable||!storage_->betaLock.held()||storage_->tournamentResultArmed)return false;
 auto& s=*storage_;using R=native_beta50kg::Result;
 try {
  BetaBytes bytes;bytes.reserve(100000);native_beta50kg::put32(bytes,0x42505252u);native_beta50kg::put32(bytes,1);native_beta50kg::put32(bytes,0);native_beta50kg::put32(bytes,0);
  native_beta50kg::put32(bytes,betaPhysicsIdentity(s.tournament));
  native_beta50kg::put32(bytes,s.common->availability11018);native_beta50kg::put32(bytes,s.common->savedSlot6E0);
  for(const auto v:s.unlock10FAC)native_beta50kg::put32(bytes,v);
  for(const auto v:s.hidden0730)native_beta50kg::put32(bytes,v);
  for(const auto v:s.remembered)native_beta50kg::put32(bytes,v);
  for(const auto v:s.rememberedCourse)native_beta50kg::put32(bytes,v);
  for(const auto& career:s.tournament.careers){
   native_beta50kg::put32(bytes,career.earned2190);native_beta50kg::put32(bytes,career.unspent2194);native_beta50kg::put32(bytes,career.kills2198);native_beta50kg::put32(bytes,career.progress219C);
   for(const auto& pod:career.pods){for(const auto f:pod.abilities1920)betaReal(bytes,f);for(const auto v:pod.medals1938)native_beta50kg::put32(bytes,v);for(const auto v:pod.remainingPrizes1946)native_beta50kg::put32(bytes,static_cast<std::uint16_t>(v));}
  }
  for(const auto& name:s.saveProfile.careerNames_)betaName(bytes,name.c_str());
  for(const auto& name:s.saveProfile.playerNames_)betaName(bytes,name.c_str());
  for(const auto& course:s.hallRecords.courses){
   for(const auto f:course.lap102F0)betaReal(bytes,f);for(const auto f:course.race1033C)betaReal(bytes,f);
   for(const auto n:course.knockouts10388)native_beta50kg::put32(bytes,n);
   for(const auto* names:{&course.lapNames102FC,&course.raceNames10348,&course.knockoutNames10394})for(const auto& name:*names)betaName(bytes,name.data());
  }
  const auto size=static_cast<std::uint32_t>(bytes.size()-16),crc=native_beta50kg::checksum(bytes,16);
  for(unsigned i=0;i<4;++i){bytes[8+i]=static_cast<std::uint8_t>(size>>(8*i));bytes[12+i]=static_cast<std::uint8_t>(crc>>(8*i));}
  const auto result=native_beta50kg::writeFile(s.betaPath,bytes);
  if(result==R::ok){s.betaStatus=BetaPersistenceStatus::saved;s.saveProfile.cardFlags_.value&=static_cast<std::uint8_t>(~1u);
   // PORT EXTENSION, NOT ORIGINAL: the DLC side file is written at this same
   // stock save moment, only when its progress changed (silent otherwise).
   (void)assets::dlc50DL::progressOwner().saveIfChanged();
   return true;}
  s.betaStatus=result==R::durabilityUnknown?BetaPersistenceStatus::durabilityUnknown:BetaPersistenceStatus::writeError;return false;
 }catch(...){s.betaStatus=BetaPersistenceStatus::writeError;return false;}
}
}

namespace rr::platform {
bool OriginalUiRiderProfileOwner50KG::readHumanPodSelection1ADFF4(
    const OriginalUiMainProfile50KG& actualCommon,std::uint32_t humanIndex,std::uint32_t& selected)const noexcept {
    if(!storage_||!storage_->readable()||storage_->common.get()!=&actualCommon||humanIndex>1)return false;
    selected=humanIndex==0?std::uint32_t(storage_->packed1125D>>3):std::uint32_t(storage_->packed1125E&31u);
    return true;
}
}
