// Common rewrite R6 (PLAN B-C14, 2.2 rows fog/sky/lights, 2.4): the three
// 1A9EE0 sub-step readers of the one track load for every course.
//   05.05 readFog273C30   273C30 fog (278570 reader, 273C64/13E830 World3EC)
//   05.11 readSky274AE0   274AE0 sky (276C00 EnvironmentInfo selection, cube images)
//   05.13 prepareLights   SCB Lights/LightN (2783D0), flashers, ambient flash
// Each reads the Track's own decoded documents, sizes its storage from the
// counts it reads, publishes its one owner into Track::roles, logs its
// common_load and common_track_data lines (stock values are receipts only,
// never sizes or gates) and refuses rather than guessing. No course code is
// compared: the code is used only to look up receipts.
#include "common_track50cr.h"
#include "stock_course_receipts50cr.h"
#include "original_track_sky50kx_fixture.h"
#include "../../src/game/original_track_fog50cr.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include <bit>
#include <cstring>
#include <exception>
#include <memory>
#include <string>

namespace common_track50CR {
namespace {
namespace lighting=rr::game::original_gc_gameplay_lighting_candidate;
namespace receipts=stock_course_receipts50CR;
using log::Step;

// Receipt lookup: the course's own row, else the shared ALL row (stock only;
// an add-on course has no receipts).
const receipts::Receipt* stockRow(const Track& t,std::string_view role)noexcept {
    if(t.source()!=Source::stock)return nullptr;
    if(const auto* r=receipts::find(t.code(),role))return r;
    return receipts::find("ALL",role);
}
void count(const Track& t,std::string_view role,std::string_view file,std::uint64_t value){
    const auto* r=stockRow(t,role);
    log::trackDataNamed(role,t.code(),file,value,r!=nullptr,r?r->value:0u);
}
// Decimal receipts are stored as the bit pattern of the double the receipt
// text parses to (note "f64:"); the reader's value is a binary32. They match
// when the receipt rounds to the same binary32.
void decimal(const Track& t,std::string_view role,std::string_view file,float value){
    const auto* r=stockRow(t,role);
    const auto bits=std::bit_cast<std::uint64_t>(static_cast<double>(value));
    if(!r){log::trackDataNamed(role,t.code(),file,bits,false,0u);return;}
    const bool f64=r->note.substr(0,4)=="f64:";
    const double stock=f64?std::bit_cast<double>(r->value):static_cast<double>(r->value);
    const bool same=static_cast<float>(stock)==value;
    log::trackDataNamed(role,t.code(),file,same?r->value:bits,true,r->value);
}
void digest(const Track& t,std::string_view role,std::string_view file,const std::string& sha){
    const auto* h=t.source()==Source::stock?receipts::findHash(t.code(),role):nullptr;
    log::trackDataNamed(role,t.code(),file,h&&h->sha256==sha?1u:0u,h!=nullptr,1u);
}
std::string member(const Track& t,const char* ext){return "tracks\\"+t.identity.code.lower()+ext;}
bool refuse(Diagnostic& d,Step s,std::uint32_t pc,std::uint64_t value,std::string why){
    d.value=value;
    log::load(s,false,value,why,nullptr,pc);
    d.refuse(s,pc,std::move(why));
    return false;
}
} // namespace

// ---------------------------------------------------------------- 05.05 fog
bool readFog273C30(Track& track,const LoadInputs&,Diagnostic& d)noexcept {
    try {
        const auto scb=member(track,".scb");
        auto fog=std::make_shared<rr::game::OriginalTrackFogRuntime50CR>();
        rr::game::OriginalTrackFogDiagnostic50CR fd;
        if(!fog->prepare(track.documents.world,fd)){
            if(fd.status==rr::game::OriginalTrackFogStatus50CR::noFogBlock){
                // 270B4C: no Fog block frees the record and World3AC stays NULL;
                // the track has no fog (data branch, not a refusal).
                track.roles.fog.reset();
                count(track,"fog.block",scb,0u);
                log::load(Step::fog273C30,true,0,"no [Fog] block: World3AC NULL, no fog");
                return true;
            }
            return refuse(d,Step::fog273C30,fd.sourcePC?fd.sourcePC:0x273C30u,fd.zone,
                std::string("[Fog] rejected: ")+rr::game::originalTrackFogStatusName50CR(fd.status)+
                " offset="+std::to_string(fd.sourceOffset)+" zone="+std::to_string(fd.zone));
        }
        const auto& data=fog->fog();
        // The zone table (World350 fog lanes) is sized once from the SCB zone
        // count, bounded by the fog_zones block ceiling (originalTrackFogZoneLimit50CR).
        const auto& row=blockRow(BlockId::fogZones);
        log::storage(row.name,data.zones350.size(),data.zones350.size()*sizeof(rr::game::OriginalTrackFogZone50CR),row.ceiling);
        const auto* fields=fog->fields3EC();
        if(!fields)return refuse(d,Step::fog273C30,0x273C64u,0,"World3EC fields not loaded");
        count(track,"fog.color.r",scb,static_cast<std::uint64_t>(data.global3AC.color00[0]));
        count(track,"fog.color.g",scb,static_cast<std::uint64_t>(data.global3AC.color00[1]));
        count(track,"fog.color.b",scb,static_cast<std::uint64_t>(data.global3AC.color00[2]));
        count(track,"fog.color_packed",scb,fields->color30);
        decimal(track,"fog.visibility",scb,data.global3AC.visibility10);
        decimal(track,"fog.haziness",scb,data.global3AC.haziness14);
        count(track,"fog.block_offset",scb,data.fogBlockOffset);
        for(const auto& b:track.documents.world.parameters.blocks)if(b.name=="fog"){
            for(const auto& f:b.fields){
                if(f.name=="colorrgb"){count(track,"fog.rgb_offset",scb,f.value_offset);count(track,"fog.color_offset",scb,f.value_offset);}
                if(f.name=="visibility")count(track,"fog.visibility_offset",scb,f.value_offset);
                if(f.name=="haziness")count(track,"fog.haziness_offset",scb,f.value_offset);
            }
            break;
        }
        std::size_t differs=0;for(const auto& z:data.zones350)if(z.differs)++differs;
        log::trackData(log::Role::fog,track.code(),scb,data.zones350.size(),false,0u);
        track.roles.fog=std::move(fog);
        d.value=data.zones350.size();
        log::load(Step::fog273C30,true,data.zones350.size(),
            differs?"zone fog differs (World4F0 bit0): per-zone controllers built":"global fog only");
        return true;
    }catch(const std::exception& e){return refuse(d,Step::fog273C30,0x273C30u,0,std::string("fog reader exception: ")+e.what());}
    catch(...){return refuse(d,Step::fog273C30,0x273C30u,0,"fog reader allocation failure");}
}

// ---------------------------------------------------------------- 05.11 sky
bool readSky274AE0(Track& track,const LoadInputs&,Diagnostic& d)noexcept {
    try {
        const auto scb=member(track,".scb");
        auto sky=std::make_shared<original_track_sky50KX::Owner>();
        std::string error;
        if(!sky->prepareFromCourseData(track.documents.world,error))
            return refuse(d,Step::sky274AE0,0x276C00u,0,"sky selection: "+error);
        track.counts.skyCubes=sky->cubeCount();
        count(track,"sky.zone_rows",scb,sky->zoneCount());
        count(track,"sky.cubes",scb,sky->cubeCount());
        if(sky->cubeCount()){
            const auto* archive=track.archive.decoded();
            if(!archive)return refuse(d,Step::sky274AE0,0x274AE0u,0,"course archive not decoded for the sky cube");
            sky50gw_fixture::Diagnostic sd;
            if(!sky->loadImages(*archive,track.documents.world,sd))
                return refuse(d,Step::sky274AE0,0x274AE0u,0,"sky cube: "+sd.message);
            const auto* images=sky->images();
            const auto file=images&&images->cubeMember()?*images->cubeMember():std::string("-");
            count(track,"sky.cub_bytes",file,images?images->cubeBytes():0u);
            digest(track,"sky.cub_sha256",file,images?images->cubeSha256():std::string());
            count(track,"sky.tiles",file,images&&images->spans()?images->spans()->size():0u);
            count(track,"sky.vertices",file,images&&images->mesh()?images->mesh()->vertices.size():0u);
            count(track,"sky.indices",file,images&&images->mesh()?images->mesh()->indices.size():0u);
        }
        log::trackData(log::Role::sky,track.code(),scb,sky->cubeCount(),false,0u);
        d.value=sky->cubeCount();
        track.roles.sky=std::move(sky);
        log::load(Step::sky274AE0,true,track.counts.skyCubes,
            track.counts.skyCubes?"one authored cube, zone rows from CubeFileForZoneN":"zero cubes: no sky drawn");
        return true;
    }catch(const std::exception& e){return refuse(d,Step::sky274AE0,0x274AE0u,0,std::string("sky reader exception: ")+e.what());}
    catch(...){return refuse(d,Step::sky274AE0,0x274AE0u,0,"sky reader allocation failure");}
}

// ---------------------------------------------------------------- 05.13 lights
bool prepareLights(Track& track,const LoadInputs& inputs,Diagnostic& d)noexcept {
    try {
        const auto scb=member(track,".scb");
        auto owner=std::make_shared<lighting::Owner>();
        // The one visual-zone lane is planned first, from the course's own
        // [Animations] row count (Counts::animations, read before this step).
        if(!owner->planTrackAnimationRows(track.counts.animations))
            return refuse(d,Step::lights,0x1A9EE0u,track.counts.animations,
                "track animation rows exceed the lighting lane ceiling");
        lighting::Diagnostic ld;
        if(!owner->prepare(track,inputs.racers,ld)){
            const char* why="lighting prepare refused";
            switch(ld.status){
            case lighting::Status::sourceFields:why="SCB light field refused";break;
            case lighting::Status::sourceShape:why="SCB light shape refused";break;
            case lighting::Status::unsupportedScene:why="zone/portal count above the topology ceiling or zone search not a list";break;
            case lighting::Status::bounds:why="NumberOfLights above the lights block ceiling";break;
            case lighting::Status::identity:why="racer count outside 1..8";break;
            case lighting::Status::registration:why="visibility topology registration refused";break;
            case lighting::Status::arithmetic:why="light arithmetic refused";break;
            case lighting::Status::allocation:why="lighting allocation failed";break;
            default:break;
            }
            return refuse(d,Step::lights,ld.sourcePC?ld.sourcePC:0x2783D0u,0,why);
        }
        const auto* tally=owner->tally();
        if(!tally)return refuse(d,Step::lights,0x2783D0u,0,"lighting tally missing");
        const auto& row=blockRow(BlockId::lights);
        log::storage(row.name,tally->numberOfLights,owner->retainedPayloadBytes(),row.ceiling);
        track.counts.lights=tally->numberOfLights;
        count(track,"lights",scb,tally->numberOfLights);
        count(track,"lights.zones",scb,tally->zones);
        count(track,"lights.portals",scb,tally->portals);
        count(track,"lights.ambient",scb,tally->ambient);
        count(track,"lights.directional",scb,tally->directional);
        count(track,"lights.points",scb,tally->points);
        count(track,"lights.emitting_points",scb,tally->emittingPoints);
        count(track,"lights.spotlights",scb,tally->spotlights);
        count(track,"lights.local",scb,tally->local);
        count(track,"lights.omitted_nonemitting",scb,tally->omittedNonEmitting);
        count(track,"lights.flashers",scb,tally->flashers);
        count(track,"lights.missing_slots",scb,tally->missingSlots);
        count(track,"lights.light92_repeats",scb,tally->missingSlots);
        count(track,"lights.scb_bytes",scb,track.documents.scb.size());
        digest(track,"lights.scb_sha256",scb,pod_materials50ed_fixture::detail::sha256(track.documents.scb));
        log::trackDataNamed("lights.environment3A4",track.code(),scb,tally->environment3A4?1u:0u,false,0u);
        if(tally->ambientFlash){
            decimal(track,"lights.flash_min_idle",scb,tally->flashMinIdle);
            decimal(track,"lights.flash_idle",scb,tally->flashIdle);
            decimal(track,"lights.flash_min_duration",scb,tally->flashMinDuration);
            decimal(track,"lights.flash_duration",scb,tally->flashDuration);
            count(track,"lights.flash_color.r",scb,static_cast<std::uint64_t>(tally->flashColor[0]));
            count(track,"lights.flash_color.g",scb,static_cast<std::uint64_t>(tally->flashColor[1]));
            count(track,"lights.flash_color.b",scb,static_cast<std::uint64_t>(tally->flashColor[2]));
        }
        if(tally->timedFlashers){
            count(track,"lights.flasher1_ordinal",scb,tally->timedFlasherOrdinals[0]);
            if(tally->timedFlashers>1u)count(track,"lights.flasher2_ordinal",scb,tally->timedFlasherOrdinals[1]);
            decimal(track,"lights.flasher_period",scb,tally->flasherPeriod);
            decimal(track,"lights.flasher_filter",scb,tally->flasherFilter);
            // Receipt is the halved Light::Init value (127.5 = 255/2).
            decimal(track,"lights.flasher_rgb",scb,tally->flasherRgb[0]*.5f);
        }
        log::trackData(log::Role::lights,track.code(),scb,tally->numberOfLights,false,0u);
        d.value=tally->numberOfLights;
        track.roles.lights=std::move(owner);
        log::load(Step::lights,true,tally->numberOfLights,
            tally->timedFlashers||tally->ambientFlash?"numeric lights read; timed lights authored":"numeric lights read",
            nullptr,0x2783D0u);
        return true;
    }catch(const std::exception& e){return refuse(d,Step::lights,0x2783D0u,0,std::string("lighting exception: ")+e.what());}
    catch(...){return refuse(d,Step::lights,0x2783D0u,0,"lighting allocation failure");}
}

} // namespace common_track50CR
