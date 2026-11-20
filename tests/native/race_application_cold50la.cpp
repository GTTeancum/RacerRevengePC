// Common rewrite (R1, PLAN 2.1 steps 1-2, B-C04/B-C05): the race-application cold
// file. Renamed from ra_application_cold50la.cpp, whose RA-only cold driver
// (interactive_ra_cold50DU::Context: RA aggregate decode/authoring/query storage
// and the RA-only consumed prefix) retires into the common path:
//   step 01  original_consumed_stage50DU::Owner::begin (one stage, every course)
//            audio counted storage -> 1A65C0 -> 28C -> 1A6AA0 -> 1AA5A0 -> 1A6B5C
//   step 02  1A6BC0 -> 1B2870 -> 141300 -> 2350D0(World4D8) -> 1B2918
//            (common_race_world50CR::construct1A6BC0), then 1A6BE0/1A9240
//   step 05  the caller (I2) runs Application::loadConsumedRaceTrack1A9EE0 (R3)
//            into the World's Track.
// Nothing here names a course or a pod: the launch's course code is data and
// must equal the Track identity's code.
#include "original_consumed_stage50du.h"
#include "common_race_world50cr.h"
#include "common_race_log50cr.h"
#include "original_race_native_retirement50la.h"
#include "original_course_race_targets50kx.h"
#include "live_audio50hf_fixture.h"
#include <cstring>
#include <exception>
#include <limits>
#include <new>

namespace common_race_world50CR {
World::~World(){
    // Never die while counted: the race's own retirement (1B2870 -> 235150 ->
    // 1B5F64) must have run first, as for every counted entity today.
    if(counted)std::terminate();
}

bool World::appendList500(rr::game::OriginalWreckComponent50LA& child,std::uint32_t pc)noexcept {
    namespace log=common_race_log50CR;
    if(!counted||!prefixComplete||!list500){
        log::storageRefused("world_list500",std::size_t(listCount50C)+1u,listCapacity504,pc);return false;
    }
    if(listCount50C>=listCapacity504){
        // 1B2870's own growth rule: capacity grows by +508 when full. Refused
        // (never written) when the grown size would not fit 32 bits or the host
        // allocation fails.
        const std::uint64_t grown=std::uint64_t(listCapacity504)+listGrowth508;
        if(!listGrowth508||grown>std::numeric_limits<std::uint32_t>::max()){
            log::storageRefused("world_list500",std::size_t(listCount50C)+1u,listCapacity504,pc);return false;
        }
        std::unique_ptr<rr::game::OriginalWreckComponent50LA*[]> next(
            new(std::nothrow) rr::game::OriginalWreckComponent50LA*[static_cast<std::size_t>(grown)]{});
        if(!next){log::storageRefused("world_list500",static_cast<std::size_t>(grown),listCapacity504,pc);return false;}
        for(std::uint32_t i=0;i<listCount50C;++i)next[i]=list500[i];
        list500=std::move(next);listCapacity504=static_cast<std::uint32_t>(grown);
        log::storage("world_list500",listCount50C,static_cast<std::size_t>(grown)*sizeof(void*),listCapacity504);
    }
    if(listCount50C>=listCapacity504){
        log::storageRefused("world_list500",std::size_t(listCount50C)+1u,listCapacity504,pc);return false;
    }
    list500[listCount50C++]=&child;
    return true;
}

bool construct1A6BC0(rr::game::OriginalRaceConstructor50LA& race,
    const original_main_entry50KG::Application& application,
    std::shared_ptr<common_track50CR::Track> track,
    std::shared_ptr<World>& out,std::uint32_t& pc)noexcept {
    namespace log=common_race_log50CR;
    out.reset();pc=0x1a6bc0;
    if(race.world74_){log::load(log::Step::world1A6BC0,false,0,"race already holds its World74");return false;}
    if(!race.constructWorld1A6BC0(application,std::move(track),pc)){
        log::load(log::Step::world1A6BC0,false,0,"1A6BC0/1B2870 World74 prefix refused",nullptr,pc);
        // A reached partial prefix is retained for the stage's ordered close.
        return false;
    }
    out=race.world74_;
    log::load(log::Step::world1A6BC0,true,0x550u,"World74 counted, World4D8 group, list500 capacity 10 growth 100000");
    return true;
}

bool retire1B2870(rr::game::OriginalRaceConstructor50LA& race,World& world,std::uint32_t& pc)noexcept {
    pc=0x1b2870;
    if(race.world74_.get()!=&world)return false;
    return race.retireNativeWorldPrefix(pc);
}

const rr::game::OriginalWreckComponent50LA* introWorldEntity(const rr::game::OriginalRaceConstructor50LA& race)noexcept {
    return race.nativeIntroWorldEntity();
}
} // namespace common_race_world50CR

namespace original_consumed_stage50DU {
Owner::Owner(const original_main_entry50KG::Application& app,live_audio50HF::Owner& audio)noexcept
    :application_(app),audio_(audio){}

Owner::~Owner()noexcept {if(!close())std::terminate();}

bool Owner::begin(original_course_targets50KX::ConsumedLaunch& launch,std::uint64_t session,
    std::uint32_t racer,rr::game::OriginalRacerModelRenderGlobals50LA& modelGlobals,
    std::shared_ptr<common_track50CR::Track> track,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d,std::uint32_t& pc)noexcept {
    namespace log=common_race_log50CR;
    d={};pc=0;
    const auto refuse=[&](const char* why)noexcept {
        log::load(log::Step::race1A65C0,false,0,why,nullptr,pc?pc:0x1a65c0u);return false;
    };
    if(attempted_)return refuse("consumed stage already attempted");
    if(!track||track->loaded()||!track->identity.code.valid())return refuse("race Track absent, already loaded or without a course code");
    if(!application_.riderProfile)return refuse("rider profile absent");
    // The launch matches by its course code as data (stock or registry row).
    if(!launch.matchesCourse(*application_.riderProfile,session,track->code(),racer))
        return refuse("consumed launch does not name the Track's course code");
    attempted_=true;track_=track;
    if(!audio_.beginCountedRaceAudioStorage())return refuse("counted race audio storage refused");
    auto* mixer=audio_.mixerForCountedConstruction();
    auto* groups=audio_.groupsForCountedConstruction();
    if(!mixer||!groups)return refuse("counted race mixer/groups absent");
    race_=application_.constructConsumedRaceCold1A65C0(launch,session,pc);
    if(!race_)return refuse("1A65C0 race constructor refused");
    if(!application_.publishConsumedRace28C(race_))return refuse("Application28C race publication refused");
    if(!application_.beginConsumedRaceLoad1A6AA0(*race_,modelGlobals,d)){pc=d.sourcePC?d.sourcePC:0x1a6aa0u;return refuse("1A6AA0 race load prefix refused");}
    if(!application_.populateConsumedRaceResources1AA5A0(*race_,d)){pc=d.sourcePC?d.sourcePC:0x1aa5a0u;return refuse("1AA5A0 race resources refused");}
    if(!race_->constructRaceGroups1A6B5C(*groups,*mixer,pc))return refuse("1A6B5C six race audio groups refused");
    log::load(log::Step::race1A65C0,true,racer,"1A65C0 race, 28C, 1A6AA0, 1AA5A0, 1A6B5C");
    // Step 02 (logged by common_race_world50CR::construct1A6BC0), then the
    // 1A6BE0 sound reset / 1B0 low2 and the counted 1A9240 Camera34 prefix.
    if(!application_.constructConsumedRaceWorld1A6BC0(*race_,track,pc))return false;
    // The 1A9EE0 track load (step 05) runs next through
    // Application::loadConsumedRaceTrack1A9EE0, into this World's Track.
    return true;
}

bool Owner::close()noexcept {
    if(!attempted_)return true;
    closeAudioDiagnostic={};closeMaterialDiagnostic={};
    if(!audio_.close(closeAudioDiagnostic))return false;
    if(race_){
        // World retirement 1B2870 runs inside (retireNativeWorldAndGroups).
        if(!application_.closeNativeConsumedRace(*race_,closeMaterialDiagnostic))return false;
        race_.reset();
    }
    if(track_){
        // The Track's own reverse-order role release, then its archive (R3).
        if(!track_->release())return false;
        track_.reset();
    }
    return true;
}
} // namespace original_consumed_stage50DU
