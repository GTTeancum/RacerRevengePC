// Common rewrite R10: the ONE race roster, any pod (PLAN.md 2.9; B-C18, B-P04,
// DLC seams). Declarations: common_roster50cr.h (W0, frozen).
//
// Every row comes from data: the stock A5 table (racer60 -> Pod%02d,
// original_racer_selection50IJ / original_player_identity50IK) or an add-on
// registry pod row (DLC, racer 32..99, PORT EXTENSION A12). Every slot loads
// its own pod's files through the same code; humans load before every AI
// (1AE280). No slot, pod or pilot is named here. The racer-keyed rules kept
// are the original's own: A8 pod id 0 / >=100 refused (210824).
#include "common_roster50cr.h"
#include "common_track50cr.h"
#include "native_racer_rig50hw_fixture.h"
#include "native_wreck_debris50pd.h"
#include "original_player_identity50ik_fixture.h"
#include "original_pod_load50pl.h"
#include "runtime_asset_paths50lb.h"
#include "runtime_startup50fl_fixture.h"
#include "../../src/game/dlc_rider_entries50dl.h"
#include "../../src/game/original_opponent_selection50kg.h"
#include "../../src/game/original_racer_selection50ij.h"
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace common_roster50CR {
namespace log=common_race_log50CR;
namespace {
namespace selection=rr::game::original_racer_selection50IJ;
namespace identities=original_player_identity50IK;

constexpr std::uint32_t pcFieldLimit1AD520=0x1AD520u;
constexpr std::uint32_t pcManager1AE280=0x1AE280u;
constexpr std::uint32_t pcHuman1ADF80=0x1ADF80u;
constexpr std::uint32_t pcAi1AE3CC=0x1AE3CCu;
constexpr std::uint32_t pcPodName21081C=0x21081Cu;
constexpr std::uint32_t pcPodBound210824=0x210824u;
constexpr std::uint32_t pcPodLoad2129C0=0x2129C0u;

bool refuse(Diagnostic& d,std::size_t slot,std::uint32_t pc,std::string reason)noexcept {
    d.slot=slot;d.pc=pc;d.refused=true;
    try{d.reason=std::move(reason);}catch(...){}
    return false;
}
// One refused slot line (common_roster ... result=refused) for a row that never
// became a Row (the racer id is the only identity known).
void rosterRefused(std::size_t slot,std::uint32_t racer60,bool human,const Diagnostic& d)noexcept {
    try{log::roster(slot,racer60,Source::stock,human,false,d.reason);}catch(...){}
}
std::filesystem::path podsRoot(){return runtime_asset_paths50LB::resolve("work/native/project/disc/PODS");}
std::filesystem::path discRoot(){return runtime_asset_paths50LB::resolve("work/native/project/disc");}
// Shipped AUDIO/UIACCEPT.INI digest: a receipt (B-P01, PLAN 4.2), never a gate.
// The one place it is defined; the rig (1E7C74 reader) reads whatever file the disc holds.
constexpr std::string_view stockUiAcceptSha256="245b0bd530e6797637ac72bbf2de8f101f3b1da280e00e8fc24e4b4930d79f3b";
// Host read of one disc file for a receipt: false when unreadable (logged as differs).
bool receiptBytes(const std::filesystem::path& path,std::vector<std::uint8_t>& out)noexcept {
    try{out=runtime_startup50fl::readBytes(path);return true;}catch(...){out.clear();return false;}
}

// Human/AI order of a field (1AE280): every human row before every AI row,
// humans34 == number of human rows, no racer twice, count 1..8.
bool fieldShapeValid(const Field& f,std::string& why)noexcept {
    if(!f.count||f.count>maxSlots){why="field size outside 1..8 (1AD520)";return false;}
    if(f.humans34>f.count){why="more humans than racers";return false;}
    for(std::size_t i=0;i<f.count;++i){
        if(f.rows[i].human!=(i<f.humans34)){why="humans must precede every AI (1AE280)";return false;}
        for(std::size_t k=0;k<i;++k)if(f.rows[k].racer60==f.rows[i].racer60){why="racer twice in the field";return false;}
    }
    return true;
}
bool fill(const std::uint32_t* racer60,std::size_t entries,std::size_t humans,bool addonCourse,
    FieldKind kind,Field& out,Diagnostic& d)noexcept {
    d={};
    Field next;next.kind=kind;
    if(!racer60&&entries)return refuse(d,0,pcFieldLimit1AD520,"no field entries");
    if(!entries||entries>maxSlots){
        log::storageRefused("roster_field",entries,maxSlots,pcFieldLimit1AD520);
        return refuse(d,0,pcFieldLimit1AD520,"field size outside 1..8 (1AD520)");
    }
    if(humans>entries)return refuse(d,0,pcFieldLimit1AD520,"more humans than racers");
    next.humans34=static_cast<std::uint32_t>(humans);
    for(std::size_t i=0;i<entries;++i){
        Row row;
        const bool human=i<humans;
        d.slot=i;
        if(!rowForRacer(racer60[i],human,addonCourse,row,d)){d.slot=i;rosterRefused(i,racer60[i],human,d);return false;}
        for(std::size_t k=0;k<i;++k)if(next.rows[k].racer60==row.racer60){
            refuse(d,i,human?pcHuman1ADF80:pcAi1AE3CC,"racer twice in the field");
            rosterRefused(i,racer60[i],human,d);return false;
        }
        if(!next.append(row,pcFieldLimit1AD520))return refuse(d,i,pcFieldLimit1AD520,"field append refused");
    }
    out=std::move(next);return true;
}
} // namespace

bool Field::append(const Row& row,std::uint32_t pc)noexcept {
    if(count>=maxSlots){
        log::storageRefused("roster_field",count+1u,maxSlots,pc);
        return false;
    }
    try{rows[count]=row;}catch(...){log::storageRefused("roster_field",count+1u,count,pc);return false;}
    ++count;return true;
}

bool rowForRacer(std::uint32_t racer60,bool human,bool addonCourse,Row& out,Diagnostic& d)noexcept {
    try {
        // A8, 210824: pod id 0 and >= 100 are refused by the original itself.
        if(!racer60||racer60>=100u)return refuse(d,d.slot,pcPodBound210824,"pod id outside 1..99 (210824)");
        const auto* entry=selection::findAnyByRacer60(racer60);
        const auto* identity=identities::find(racer60);
        if(!entry||!identity)return refuse(d,d.slot,pcPodName21081C,"racer has no pod row (A5 table or registry)");
        // DLC seam (PLAN 2.9): an add-on pod joins an AI field only by its
        // registry rule (aiOnAddonCourses, add-on courses only). Stock racers
        // answer true, so a stock field is unchanged with or without DLC.
        if(!human&&!rr::game::dlc50DL::addonRiderMayBeAi(racer60,addonCourse))
            return refuse(d,d.slot,pcAi1AE3CC,"add-on pod not admitted as AI on this course (aiOnAddonCourses)");
        Row row;
        row.racer60=racer60;
        row.podId=std::string(entry->podId);
        row.source=entry->addon?Source::registry:Source::stock;
        row.human=human;
        row.archive=identities::podArchive(podsRoot(),row.podId).string();
        row.camera=identities::podCamera(podsRoot(),row.podId).string();
        row.archiveSha256=identity->archiveSha256?identity->archiveSha256:"";
        row.archiveBytes=identity->archiveBytes;
        out=std::move(row);return true;
    }catch(const std::exception& e){return refuse(d,d.slot,pcPodName21081C,e.what());}
    catch(...){return refuse(d,d.slot,pcPodName21081C,"pod row construction failed");}
}

bool buildTournamentField188470(std::uint32_t klass,std::uint32_t humanRacer60,Field& out,Diagnostic& d)noexcept {
    d={};
    namespace original=rr::game::original_opponent_selection50KG;
    std::array<original::RacerId,7> opponents{};
    if(!original::deterministic188470(klass,static_cast<original::RacerId>(humanRacer60),opponents))
        return refuse(d,0,0x188470u,"188470 deterministic field rejected (class)");
    std::array<std::uint32_t,maxSlots> ids{};
    ids[0]=humanRacer60;
    for(std::size_t i=0;i<opponents.size();++i){
        if(opponents[i]<=0)return refuse(d,i+1,0x188470u,"188470 returned no racer");
        ids[i+1]=static_cast<std::uint32_t>(opponents[i]);
    }
    // Tournament courses are stock rows; the opponents are stock racers.
    return fill(ids.data(),ids.size(),1,false,FieldKind::tournament188470,out,d);
}
bool buildCourseField(const std::uint32_t* racer60,std::size_t entries,std::size_t humans,
    bool addonCourse,FieldKind kind,Field& out,Diagnostic& d)noexcept {
    return fill(racer60,entries,humans,addonCourse,kind,out,d);
}
bool buildTimeTrialField(std::uint32_t humanRacer60,Field& out,Diagnostic& d)noexcept {
    return fill(&humanRacer60,1,1,false,FieldKind::timeTrialSolo,out,d);
}
bool buildSpectatorField(const std::uint32_t* racer60,std::size_t entries,bool addonCourse,Field& out,Diagnostic& d)noexcept {
    return fill(racer60,entries,0,addonCourse,FieldKind::all8Spectator,out,d);
}

bool Owner::build(const Field& field,Diagnostic& d)noexcept {
    d={};
    if(built_)return refuse(d,0,pcFieldLimit1AD520,"roster already built (one field per race)");
    std::string why;
    if(!fieldShapeValid(field,why))return refuse(d,0,pcFieldLimit1AD520,why);
    try{field_=field;}catch(...){return refuse(d,0,pcFieldLimit1AD520,"field copy failed");}
    built_=true;return true;
}

bool Owner::loadAll(const common_track50CR::Track& track,
    std::array<std::unique_ptr<native_racer50HW::Rig>,maxSlots>& rigs,Diagnostic& d)noexcept {
    d={};
    if(!built_){
        refuse(d,0,pcManager1AE280,"roster loaded before its field was built");
        log::load(log::Step::roster1AA510,false,0,d.reason);return false;
    }
    // The track is data here: an add-on course (registry row) admits the AI
    // add-on rows its registry allows; the field builder already applied the
    // same rule, this re-checks it against the race's actual track.
    const bool addonCourse=track.source()==Source::registry;
    const std::size_t racers=field_.count,humans=field_.humans34;
    std::size_t loaded=0;
    for(std::size_t ordinal=0;ordinal<racers;++ordinal){
        std::size_t slot=0;
        if(!original_pod_load50PL::managerOrderSlot50PL(ordinal,humans,racers,slot)){
            refuse(d,ordinal,pcManager1AE280,"1AE280 manager order refused the field counts");break;
        }
        const Row& row=field_.rows[slot];
        const std::uint32_t pathPc=row.human?pcHuman1ADF80:pcAi1AE3CC;
        try {
            if(!row.human&&!rr::game::dlc50DL::addonRiderMayBeAi(row.racer60,addonCourse)){
                refuse(d,slot,pathPc,"add-on pod not admitted as AI on this course (aiOnAddonCourses)");
                log::roster(slot,row.racer60,row.source,row.human,false,d.reason);break;
            }
            if(rigs[slot]){
                refuse(d,slot,pathPc,"slot already owns a rig");
                log::roster(slot,row.racer60,row.source,row.human,false,d.reason);break;
            }
            // 2129C0 inputs: this slot's own pod archive (stock Pods\PodNN\ or
            // the registry dlc:/ member), the same constructor for every pod.
            rigs[slot]=std::make_unique<native_racer50HW::Rig>(".",row.archive);
            if(rigs[slot]->podId!=row.podId){
                refuse(d,slot,pcPodLoad2129C0,"rig and roster pod identity differ");
                log::roster(slot,row.racer60,row.source,row.human,false,d.reason);break;
            }
            // The archive this pod was admitted with must be the file read now
            // (admission integrity, the same rule every pod reader applies).
            const auto disc=original_player_identity50IK::podArchive(podsRoot(),row.podId);
            const auto bytes=runtime_startup50fl::readBytes(disc);
            if((row.archiveBytes&&bytes.size()!=row.archiveBytes)||
               (!row.archiveSha256.empty()&&pod_materials50ed_fixture::detail::sha256(bytes)!=row.archiveSha256)){
                refuse(d,slot,pcPodLoad2129C0,"pod archive differs from its admitted identity");
                log::roster(slot,row.racer60,row.source,row.human,false,d.reason);break;
            }
            // Wreck chain 165E90: this pod's own [Wreckage] pieces (7 or 9 on
            // the stock disc, whatever its files hold). Presentation only: a
            // rejected piece is logged, never a load failure (as before).
            {
                std::string wreckLog;bool inventoried=false;
                try{inventoried=native_wreck_debris50PD::loadRacer(slot,*rigs[slot],bytes,wreckLog);}
                catch(const std::exception& e){wreckLog=e.what();}catch(...){wreckLog="unknown failure";}
                std::cerr<<"wreck_debris_cold slot="<<slot<<" pod="<<row.podId<<" pc=165E90 inventoried="
                         <<inventoried<<' '<<wreckLog<<'\n';
            }
            log::roster(slot,row.racer60,row.source,row.human,true);
            logPodReceipts(slot);
            ++loaded;
        }catch(const std::exception& e){
            refuse(d,slot,pcPodLoad2129C0,std::string("pod load: ")+e.what());
            log::roster(slot,row.racer60,row.source,row.human,false,d.reason);break;
        }catch(...){
            refuse(d,slot,pcPodLoad2129C0,"pod load: non-standard exception");
            log::roster(slot,row.racer60,row.source,row.human,false,d.reason);break;
        }
    }
    const bool ok=loaded==racers&&!d.refused;
    log::load(log::Step::roster1AA510,ok,loaded,ok?std::string_view{}:std::string_view(d.reason),nullptr,
        ok?0u:d.pc);
    return ok;
}

void Owner::logPodReceipts(std::size_t slot)const {
    const Row* r=row(slot);
    if(!r)return;
    // Receipts never refuse and never size anything (PLAN 4.2). An add-on pod
    // has no stock row: receipt=none.
    const auto* stock=identities::stockFind(r->racer60);
    const auto* actual=identities::find(r->racer60);
    if(r->source==Source::registry||!stock||!actual){
        log::podReceipt(r->racer60,"archive",log::Receipt::none);
        log::podReceipt(r->racer60,"phy",log::Receipt::none);
        log::podReceipt(r->racer60,"sounds.ini",log::Receipt::none);
        log::podReceipt(r->racer60,"UIACCEPT.INI",log::Receipt::none);
        log::podReceipt(r->racer60,"ANN.INI",log::Receipt::none);   // add-on pods: generic lines only
        return;
    }
    const auto same=[](const char* a,const char* b){return a&&b&&std::string_view(a)==std::string_view(b);};
    log::podReceipt(r->racer60,"archive",actual->archiveBytes==stock->archiveBytes&&
        same(actual->archiveSha256,stock->archiveSha256)?log::Receipt::match:log::Receipt::differs);
    log::podReceipt(r->racer60,"phy",same(actual->phySha256,stock->phySha256)?log::Receipt::match:log::Receipt::differs);
    log::podReceipt(r->racer60,"sounds.ini",same(identities::speechCountsSha256(*actual),
        identities::stockSpeechCountsSha256(*stock))?log::Receipt::match:log::Receipt::differs);
    // The shared UI speech file the 1E7C74 first-event table reads, and this pod's
    // named announcer INI (2C4B30): each file's bytes and SHA-256 against the
    // stock identity. Read from the disc here, so the receipt works after build()
    // alone (R11 host data check) as well as after loadAll.
    namespace d=pod_materials50ed_fixture::detail;
    std::vector<std::uint8_t> bytes;
    const bool ui=receiptBytes(discRoot()/"AUDIO"/"UIACCEPT.INI",bytes)&&d::sha256(bytes)==stockUiAcceptSha256;
    log::podReceipt(r->racer60,"UIACCEPT.INI",ui?log::Receipt::match:log::Receipt::differs);
    const bool ann=receiptBytes(identities::announcerPath(discRoot(),*stock),bytes)&&
        bytes.size()==stock->announcerBytes&&same(d::sha256(bytes).c_str(),stock->announcerSha256);
    log::podReceipt(r->racer60,"ANN.INI",ann?log::Receipt::match:log::Receipt::differs);
}

void Owner::release()noexcept {
    field_=Field{};built_=false;
}

} // namespace common_roster50CR
