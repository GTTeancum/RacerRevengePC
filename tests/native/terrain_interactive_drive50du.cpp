#include "runtime_allocations50fl.h"
#include "original_racer_selection50ij.h"
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_contact_bank50hw_fixture.h"
#include "native_dynamic_contact_peers50iw_fixture.h"
#include "../../src/game/dynamic_impact_events50iw.h"
#include "native_racer_rig50hw_fixture.h"
#include "native_racer_handling50hw_fixture.h"
#include "original_racer_visual50hw_fixture.h"
#include "native_racer_cables50hw_fixture.h"
#include "native_racer_grid50hn_fixture.h"
#include "native_racer_draw50hn_fixture.h"
#include "race_session50hm_fixture.h"
#include "race_audio50hq_fixture.h"
#include "race_mode1_host50id_fixture.h"
#include "source_announcer50ie_fixture.h"
#include "race_music50ie_fixture.h"
#include "opponent_engine_audio50hx_fixture.h"
#include "engine_damage_audio50ia.h"
#include "wrong_way_progress50go.h"
#include "host_course_driver50gk.h"
#include "player_finish50ia.h"
#include "phase_timing50gk_fixture.h"
#include "render_diagnostics50gq_fixture.h"
#include "lap_drive50gj_fixture.h"
#include "live_audio50hf_fixture.h"
#include "pod_speech_authoring50hg_fixture.h"
#include "material_audio_provider50hi_fixture.h"
#include "host_chase_camera50fn_fixture.h"
#include "capture_visibility50fo_fixture.h"
#include "terrain_probe_authoring50fq_fixture.h"
#include "live_terrain_probe50fr_fixture.h"
#include "runtime_startup50fl_fixture.h"
#include "boost_controls50fk_fixture.h"
#include "checkpoint_recovery50fi_fixture.h"
#include "pod_cables50fd_fixture.h"
#include "source_texture50fc_fixture.h"
#include "pod_scene_rebase50fb.h"
#include "track_materials50fa_fixture.h"
#include "track_normals50hk_fixture.h"
#include "material_alpha50gx_fixture.h"
#include "sky50gw_fixture.h"
// Interactive Linux development client. Cold ownership/setup is adapted from
// terrain_offscreen_drive50dh.cpp, retained as a separate harness. Test-only
// retained-data loading remains, but source-word validation runs separately.
#include "../../platform/linux/native/game_host50er.h"
#include "../../platform/linux/native/host_session_menu50if.h"
#include "../../platform/linux/native/host_race_results50il.h"
#include "../../platform/linux/native/keyboard_packet50dr.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../platform/linux/native/fixed_step_clock50dt.h"
#include "../../platform/linux/native/vehicle_view_camera50dg.h"
#include "control_presets49d.h"
#include "pod_materials50ed_fixture.h"
#include "../../platform/linux/native/material_texture_functions50ee.h"
#include "vehicle_present50da.h"
#include "controller_ingress50cx.h"
#include "controller_actions50ec.h"
#include "vehicle_draw_submit50cs.h"
#include "vehicle_draw_assets50cr.h"
#include "zone_registration_events50cq.h"
#include "zone_lifecycle50cl.h"
#include "live_portals50ep_fixture.h"
#include "vehicle_live_query50eq.h"
#include "zone_search_views50cg.h"
#include "vehicle_candidates50ca.h"
#include "candidate_handles50cb.h"
#include "collection_projection50cf.h"
#include "query_prepare48o.h"
#include "vehicle_indexed_query50cd.h"
#include "search_cold50bu.h"
#include "search_view50bt.h"
#include "effect_names50bl.h"
#include "effect_cold50bj.h"
#include "reset_effects50bk.h"
#include "reset_model_bounds50ba.h"
#include "reset_lifecycle50at.h"
#include "main_reset50ar.h"
#include "authored_runtime_ground50cp_fixture.h"
#include "track_runtime_storage50ig_fixture.h"
#include "track_static_candidates50ig_fixture.h"
#include "gc_animated_candidates50ig_fixture.h"
#include "gc_dynamic_window50ig_fixture.h"
#include "gc_atm_window_fixture.h"
#include "source_grid50ig_fixture.h"
#include "track_session_assets50ig_fixture.h"
#include "track_start50ad.h"
#include "track_navigation50eu.h"
#include <memory>
#include <cstdlib>
#include <chrono>
#include <cmath>
#include <thread>
#include <limits>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include "vehicle_base_finish50y.h"
#include "pod_flame50w.h"
#include "engine_flare50gv.h"
#include "type2_fixture_io50i.h"
#include "vehicle_static_query50h.h"
#include "movement_static_query50l.h"
#include "vehicle_secondary_contact50j.h"
#include "vehicle_static_response50g.h"
#include "../../src/assets/spring_authoring50f.h"
#include "../../src/assets/repair_authoring50a.h"
namespace f=runtime_startup50fl;namespace cold=cold50i;
using namespace rr::game;using J=nlohmann::json;
struct ResponseStorage50K {
 SpringGeneratedRecord48K projected[60]{};SpringRecord48N staged[60]{};
 std::uint8_t flags[60]{},nextFlags[60]{};SpringModelBody48K scratch{},prepared{};
 VehicleStaticResponseWorkspace50G view(VehicleFrameWorkspace49W&w){return {{projected,60,{&scratch,1,flags,60},{&prepared,1,nextFlags,60}},staged,60,&w};}
};
struct MovementRays50N {
 Vector3_48C localStart[1]{},localDirection[1]{};SpringVector4_48N start[1]{},direction[1]{},normal[1]{},point[1]{};
 std::uint32_t hit[1]{};float fraction[1]{},length[1]{};
 Type3RayBuffers48P view(){return {localStart,localDirection,start,direction,normal,point,hit,fraction,1,length,1};}
 Type3RaySource48P source()const{return {localStart,localDirection,start,direction,normal,point,hit,fraction,1,length,1};}
};
namespace {
void require50DU(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
// Cold startup only: add resource identity/stage without changing successful
// operations. Keep the original exception nested and its detail in what().
template<class Operation>
decltype(auto) coldRacerStage50DU(const std::string& podId,const char* stage,
    Operation&& operation) {
    try { return operation(); }
    catch(const std::exception& error) {
        std::throw_with_nested(std::runtime_error(
            "Original racer "+podId+" ["+stage+"]: "+error.what()));
    }
    catch(...) {
        std::throw_with_nested(std::runtime_error(
            "Original racer "+podId+" ["+stage+"]: non-standard exception"));
    }
}
void addCounter50DU(std::uint64_t& counter, std::uint64_t amount = 1) noexcept {
    const auto room = std::numeric_limits<std::uint64_t>::max() - counter;
    counter += std::min(room, amount);
}
std::uint64_t monotonicNow50DU() noexcept {
    using Clock = std::chrono::steady_clock;
    static const auto origin = Clock::now();
    return static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - origin).count());
}
void reportHost50DU(const rr::platform::GameHostDiagnostic50ER& diagnostic) {
    std::cerr << "host error " << static_cast<unsigned>(diagnostic.error)
              << " at " << (diagnostic.operation ? diagnostic.operation : "unknown")
              << " errno=" << diagnostic.systemError
              << " window=" << static_cast<unsigned>(diagnostic.window.error)
              << " EGL=" << static_cast<unsigned>(diagnostic.headless.error) << '\n';
}
void reportRenderer50DU(const rr::platform::VehicleWindowDiagnostic50DS& diagnostic) {
    std::cerr << "renderer error " << static_cast<unsigned>(diagnostic.error)
              << " at " << (diagnostic.operation ? diagnostic.operation : "unknown")
              << " GL=" << diagnostic.glError << " cleanup=" << diagnostic.cleanupGlError
              << " component=" << diagnostic.component << '\n';
}
}
#include "native_ai_session50iv_fixture.h"
#include <cctype>

namespace recovery50FI=checkpoint_recovery50fi_fixture;
enum class SessionExit50DU { Quit, Restart, Checkpoint };
SessionExit50DU runSession50DU(const char* corpusPath,bool headlessLive,const char* capturePath,
    const recovery50FI::Plan* checkpointInput,recovery50FI::Plan& checkpointOutput,
    lap_drive50GJ::Continuity& lapContinuity,std::uint64_t sessionIndex,
    HostMonotonicClock50ID& applicationClock,bool startAtMenu,std::uint32_t& selectedRacer60,
    const rr::game::RaceAudioSettings50IC& effectiveAudioSettings,
    rr::game::RaceAudioSettings50IC& selectedAudioSettings,
    rr::platform::HostTrack50IM effectiveTrack,rr::platform::HostTrack50IM& selectedTrack,
    const rr::platform::HostTrackAdmission50IM& trackAdmission){
 // Capture the immutable loaded identity before any front-menu choice changes
 // the next-session selection. The alternate-player integration consumes it.
 const std::uint32_t playerRacer60=selectedRacer60;
 const auto loadedTrack=effectiveTrack;
 require50DU(rr::platform::validHostTrack50IM(loadedTrack),"unsupported loaded track identity");
 const char* trackSelectionError=nullptr;
 const auto* trackSession=track_session_assets50IG::select(rr::platform::hostTrackCode50IM(loadedTrack),trackSelectionError);
 require50DU(trackSession,trackSelectionError?trackSelectionError:"original track admission failed");
 if(trackSession->originalStaticCandidates){
  const char* incomplete=std::getenv("RR_GC_INCOMPLETE_WORLD");
  require50DU(incomplete&&std::string(incomplete)=="1",
      "GC native world is incomplete: zone11 opening trigger, authored LostAI branch triggers and Gate sound are unjoined; RR_GC_INCOMPLETE_WORLD=1 explicitly accepts these omissions");
  std::cerr<<"GC INCOMPLETE WORLD: real original track/navigation and both animated model/type1 collision owners are enabled; "
      "zone11 opening retains its authored inactive state because its Gate trigger is not joined. "
      "Authored LostAI branch triggers and Gate sound are not yet joined. "
      "This is not complete-track support.\n";
 }

 // Pending choices only apply on an explicit cold Start/Restart.
 const auto loadedAudioSettings=effectiveAudioSettings;
 const auto* selectedOriginal=original_racer_selection50IJ::findByRacer60(playerRacer60);
 require50DU(selectedOriginal&&selectedOriginal->humanJoinImplemented,"unsupported selected racer identity");
 require50DU(std::fesetround(FE_TONEAREST)==0,"round-to-nearest setup failed");
 const char* raceSetting=std::getenv("RR_RACE");
 const bool raceEnabled=!(raceSetting&&std::string(raceSetting)=="0");
 const char* allAiSetting=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
 const bool allAiEnabled=allAiSetting&&std::string(allAiSetting)=="1";
 const char* originalAiSetting=std::getenv("RR_ORIGINAL_AI_FIRST_SLICE");
 const bool originalAiEnabled=allAiEnabled||(originalAiSetting&&std::string(originalAiSetting)=="1");
 const char* dynamicContactSetting=std::getenv("RR_ORIGINAL_AI_DYNAMIC_CONTACT");
 const bool dynamicContactEnabled=allAiEnabled||(dynamicContactSetting&&std::string(dynamicContactSetting)=="1");
 require50DU(!dynamicContactEnabled||originalAiEnabled,
     "dynamic source contacts require the bounded original AI session");
 constexpr std::size_t originalAiSlot=1;
 require50DU(!originalAiEnabled||(raceEnabled&&track_session_assets50IG::codeEquals(trackSession->world->code,"GC")),
     "original AI first slice requires the real GC fleet");
 // Source21079C retains constructor a1;21081C stores vehicle60 and210820/24
 // formats the original Pod%02d resource prefix. Native bank slot0 is a role.
 const bool playerUsesRosterRig=allAiEnabled||playerRacer60!=1u;
 require50DU(raceEnabled||!playerUsesRosterRig,"alternate original selection requires the native race");
 const std::string playerPodId(selectedOriginal->podId);
 const auto playerArchive=std::filesystem::path("work/native/project/disc/PODS")/
     playerPodId/(playerPodId+".RES");
 const char* raceModeSetting=std::getenv("RR_RACE_MODE");
 require50DU(!raceModeSetting||std::string(raceModeSetting)=="1"||std::string(raceModeSetting)=="2",
     "RR_RACE_MODE must be 1 (source events) or 2 (HOST fallback)");
 const bool mode1Requested=raceEnabled&&(!raceModeSetting||std::string(raceModeSetting)=="1");
 require50DU(!mode1Requested||trackSession->nativeMode1,
     "GC original record-row/selection configuration is unjoined; choose RR_RACE_MODE=2 explicitly");
 require50DU(rr::platform::hostTrackAdmitted50IM(loadedTrack,trackAdmission),
     "GC track selection requires RR_GC_INCOMPLETE_WORLD=1 and RR_RACE_MODE=2 explicitly");
 // Default mode1 is committed only after ALL required providers cold-prepare.
 // Failure is explicit; RR_RACE_MODE=2 is the selectable existing fallback.
 bool mode1Enabled=false;
 require50DU(!originalAiEnabled||!mode1Requested,
     "original AI first slice currently owns its declared mode2 manager timing boundary");
 const char* disableRacerContacts=std::getenv("RR_DISABLE_RACER_CONTACTS");
 const bool racerContactsEnabled=raceEnabled&&
     !(disableRacerContacts&&std::string(disableRacerContacts)=="1");
 require50DU(!originalAiEnabled||racerContactsEnabled,
     "original AI first slice requires actual racer contacts and their source boundary");
 constexpr std::uint32_t raceRacerCount=rr::game::defaultHostRaceRacers50HT;
 constexpr std::uint32_t raceOpponentCount=raceRacerCount-1; // Camera-relative presentation count.
 // Native selected roster, not the retail human-dependent boot root1AE620.
 // Counts use the original1125C layout; every AI retains its real pod/profile.
 const std::uint32_t originalAiHumanCount=allAiEnabled?0u:1u;
 const std::uint32_t originalAiOpponentCount=raceRacerCount-originalAiHumanCount;
 const std::uint16_t originalAiSettings1125C=static_cast<std::uint16_t>(
     3u|(originalAiHumanCount<<5u)|(originalAiOpponentCount<<7u));
 std::uint64_t allAiHostDriverCalls=0,allAiControllerSamples=0,allAiKeyboardPackets=0;
 static_assert(raceRacerCount<=rr::platform::maxVehicleWindowStandings50DS);
 static_assert(maxHostRaceOpponents50HT==rr::platform::maxVehicleWindowOpponents50DS);
 static_assert(maxHostRaceRacers50HT<=native_racer_contact50HW::maxRacers);
 static_assert(live_audio50HF::lapFirstVoice+live_audio50HF::lapVoiceCount<=64);
 static_assert(64+material_audio50HI::soundCount<=race_audio50HQ::firstVoice);
 static_assert(race_audio50HQ::firstVoice+race_audio50HQ::voiceCount<=opponent_audio50HX::firstVoice);
 static_assert(opponent_audio50HX::firstVoice+opponent_audio50HX::voiceCount<=live_audio50HF::voiceCount);
 static_assert(race_music50IE::voice<material_audio50HI::extraFirstVoice);
 static_assert(material_audio50HI::extraFirstVoice+material_audio50HI::maximumExtraVoices<=live_audio50HF::voiceCount);
 if(raceEnabled){
  require50DU(!checkpointInput,"a race must restart every racer together from the grid");
  lapContinuity={};
 }
 const char* tracePhases=std::getenv("RR_TRACE_PHASES");
 phase_timing50GK::Interval phaseTiming{tracePhases&&std::string(tracePhases)=="1"};
 const char* traceRender=std::getenv("RR_TRACE_RENDER");
 render_diagnostics50GQ::Interval renderDiagnostics{traceRender&&std::string(traceRender)=="1"};
 const char* captureWrongSetting=std::getenv("RR_CAPTURE_WRONG_WAY");
 const bool captureFirstWrongWay=headlessLive&&captureWrongSetting&&std::string(captureWrongSetting)=="1";
 bool capturedFirstWrongWay=false,lastCommittedWrongWay=false;
 std::ifstream file(corpusPath);require50DU(bool(file),"cannot open collision corpus");J corpus;file>>corpus;const J*chosen=nullptr;
 for(const auto&row:corpus["cases"])if(row["name"]==trackSession->mainCollisionCase)chosen=&row;
 require50DU(chosen,"authored placement case present");const auto&row=*chosen;const auto&qj=row["query"];
 auto shape=cold::shape(corpus["shapes"][qj["shape"].get<std::size_t>()]);
 std::vector<cold::Mesh> meshes;
 if(!trackSession->originalStaticCandidates){meshes.reserve(corpus["meshes"].size());
  for(const auto&j:corpus["meshes"])meshes.push_back(cold::mesh(j));}
 auto qfilter=cold::filter(qj["filter"]);Type2StaticQuery49X query{};query.resource=&shape.view;query.filter=&qfilter;query.queryScene.value=77;
 query.collisionTypeTag=qj["collision_tag"];query.groupTypeTag=qj["group_tag"];query.callbackEnabled=qj["callback"];
 cold::words(query.copied40.data(),qj["copied40"]);cold::words(query.copied50.data(),qj["copied50"]);
 query.skipMask=row["masks"][0];query.dynamicMask=row["masks"][1];query.materialMask=row["masks"][2];
 std::vector<std::string> names;
 std::vector<Type2StaticCandidate49X> candidates;std::size_t stackCount=1;
 if(!trackSession->originalStaticCandidates){
 names.resize(row["candidates"].size());for(std::size_t i=0;i<names.size();++i)names[i]=cold::unhex(row["candidates"][i]["name_hex"]);
 candidates.reserve(row["order"].size());
 for(auto index:row["order"].get<std::vector<std::size_t>>()){
  const auto&j=row["candidates"][index];auto&m=meshes[j["mesh"].get<std::size_t>()];candidates.emplace_back();auto&out=candidates.back();
  out.nodeRadii=m.radii.data();out.nodeRadiusCount=m.radii.size();out.maxDepth=m.maxDepth;out.candidate.hasMaterialScene=j["has_scene"];out.candidate.sceneName=names[index].data();out.candidate.sceneNameBytes=names[index].size();
  auto&v=out.candidate.collision;v.geometry={&m.mesh,cold::matrix(j["world"]),j["id"].get<std::uint32_t>()};v.filter=cold::filter(j["filter"]);v.sphere=cold::sphere(j["sphere"]);v.typeTag=j["type_tag"];
  cold::words(v.copied40.data(),j["copied40"]);cold::words(v.copied50.data(),j["copied50"]);stackCount=std::max(stackCount,1+shape.view.maxDepth+m.maxDepth);
 }
 } // TA retained diagnostic candidate inputs only.
 f::Fixture x(0);auto&s=x.state;auto&m=s.bodies.main;
 native_ai50IV::NoiseLifetime originalNoiseLifetime(s.noise,originalAiEnabled);
 AiRuntimeState50IV originalAi{};AiRuntimeReceipt50IV originalAiReceipt{};
 native_ai50IV::Session originalAiSession{};
 AiTrackMetadata50IU originalAiMapMetadata{},originalAiMetadata{};
 AiTrackTuning50IU originalAiTuning{};
 rr::assets::AiAuthoringDiagnostic50IU originalAiAuthoringDiagnostic{};
 // Explicit HOST fresh-session policy: this Fixture reconstructs the native
 // raw RNG, so reset its shared Dax/category6 lazy slot at that SAME boundary.
 // It is not banked or private audio RNG. Original cross-race static lifetime
 // is not claimed; the monotonic clock backend itself remains application-wide.
 LapEventRandomOwner50ID sessionLazy{};
 // Shared source owners: never bank these along with the reused racer arena.
 // Fresh17DF80/17F970 profile, no save/cheat/menu continuation admitted.
 // The consumed track bits stay cold-clear only before the retained destruction
 // and menu boundaries. They are not reset on each contact.
 DynamicImpactProfile50IW dynamicProfile{};
 dynamicProfile.game34=3;dynamicProfile.game10FC4=0;dynamicProfile.game10FE0=0;
 dynamicProfile.track1B1=0;dynamicProfile.bound=dynamicContactEnabled;
 DynamicContactProfile50IW dynamicMusicProfile{1u,dynamicContactEnabled};
 //186644..186654 fresh initialization sets11050bit2; no options override.
 const std::uint32_t dynamicConsumerFlags11050=4u;
 DynamicImpactTuning50IW dynamicTuning{};
 rr::assets::FinalImpactAuthoring49Y dynamicFinalTuning{};
 DynamicContactEventBindings50IW dynamicEventBindings{};
 DynamicContactRangeOwner50IW dynamicRange{};
 DynamicContactMusicCursor50IW dynamicMusicCursor{};
 DynamicContactEventCursor50IW dynamicEventCursor{};
 DynamicContactPodEffect50IW dynamicPodEffect{};
 DamageEventCursor50ID dynamicDamageCursor{};
 RaceEventQueue50ID<128> dynamicEvents{};
 native_dynamic_contact50IW::Registry dynamicRegistry;
 native_dynamic_contact50IW::Snapshot dynamicPeers{};
 std::size_t dynamicSelfSlot=0;
 struct DynamicActors50IW {
  std::array<RaceActorSnapshot50ID,raceRacerCount> values{};
  std::array<bool,raceRacerCount> bound{};
 } dynamicActors;
 std::array<std::uint32_t,raceRacerCount> dynamicRacer60{};
 const RaceActorResolver50ID dynamicActorResolver{&dynamicActors,
  [](void* opaque,RaceActorId50ID id,RaceActorSnapshot50ID& out)noexcept{
   const auto& actors=*static_cast<const DynamicActors50IW*>(opaque);
   if(!id||id>actors.values.size()||!actors.bound[id-1])return false;
   out=actors.values[id-1];return true;
  }};
 // Permanent original-resource owners precede the snapshot bank and all draw
 // publications. Only their immutable borrowed views enter the live arena.
 std::array<std::unique_ptr<native_racer50HW::Rig>,raceRacerCount> racerRigs;
 std::array<std::unique_ptr<native_racer_collision50HW::Owner>,raceRacerCount> racerCollisions;
 std::array<std::unique_ptr<native_racer50HW::Visual>,raceRacerCount> racerVisuals;
 std::array<std::unique_ptr<native_racer50HW::Cables>,raceRacerCount> racerCables;
 std::array<std::string,raceRacerCount> racerNames;
 std::array<std::array<char,rr::platform::maxVehicleWindowRacerName50DS+1>,raceRacerCount> racerHudNames{};
 std::size_t mainPairCapacity=shape.view.rayCount,secondaryPairCapacity=0;
 // COL stores XYZ only. Preserve the retained corpus's explicit HOST poison
 // policy for opaque W words; these are not claimed loaded original values.
 const native_racer_collision50HW::EndpointPolicy endpointPolicy{nullptr,
     [](void*,native_racer_collision50HW::QueryBody,std::size_t index,
        Type2OpaqueEndpointWords49X& out) noexcept {
      if(index>=32768)return false;
      out={0x80000000u+static_cast<std::uint32_t>(index),
           0x7fc10000u+static_cast<std::uint32_t>(index)};return true;
     }};
 const auto matchesEndpointPolicy=[&](const Type2ResourceView49X& resource){
  for(std::size_t index=0;index<resource.rayCount;++index){
   Type2OpaqueEndpointWords49X words{};
   if(!endpointPolicy.words(nullptr,native_racer_collision50HW::QueryBody::Main,index,words)||
      std::bit_cast<std::uint32_t>(resource.rays[index].first.v[3])!=words.first||
      std::bit_cast<std::uint32_t>(resource.rays[index].second.v[3])!=words.second)return false;
  }
  return true;
 };
 if(raceEnabled){
  require50DU(matchesEndpointPolicy(shape.view),"retained main endpoint-W host policy differs");
  const auto rosterRoot=std::filesystem::path("evidence/originalRoster50HW/raw-imports");
  auto roster=native_racer50HW::rig_detail::json(rosterRoot/"roster.json");
  require50DU(roster.at("player_index")==0&&roster.at("racers").size()==raceRacerCount,
      "original roster must contain the player and seven original opponents");
  require50DU(roster.at("racers").at(0).at("pod_id")=="POD01",
      "retained original roster player identity differs");
  std::size_t selectedSlot=raceRacerCount;
  for(std::size_t racer=0;racer<raceRacerCount;++racer)
   if(roster.at("racers").at(racer).at("pod_id")==playerPodId){
    require50DU(selectedSlot==raceRacerCount,"duplicate selected original racer");selectedSlot=racer;
   }
  require50DU(selectedSlot<raceRacerCount,"selected original racer absent from retained roster");
  // Swap complete authenticated entries, preserving all eight distinct racers.
  // The selected pod's former opponent slot receives Anakin's original resources.
  if(selectedSlot)std::swap(roster.at("racers").at(0),roster.at("racers").at(selectedSlot));
  for(std::size_t racer=0;racer<raceRacerCount;++racer){
   const auto& entry=roster.at("racers").at(racer);
   const auto id=entry.at("pod_id").get<std::string>();
   coldRacerStage50DU(id,"roster identity",[&](){
    require50DU(id.size()==5&&id.substr(0,3)=="POD"&&
        id[3]>='0'&&id[3]<='9'&&id[4]>='0'&&id[4]<='9'&&
        (racer!=0||id==playerPodId),"original roster identity rejected");
    for(std::size_t previous=0;previous<racer;++previous)
     require50DU(roster.at("racers").at(previous).at("pod_id")!=id,"duplicate original racer");
   });
   if(racer||playerUsesRosterRig)coldRacerStage50DU(id,"rig construction",[&](){
    const auto relative=std::filesystem::path(entry.at("path").get<std::string>());
    require50DU(!relative.is_absolute()&&relative.lexically_normal()==relative&&
        relative.string().find("..") == std::string::npos,"original roster path rejected");
    racerRigs[racer]=std::make_unique<native_racer50HW::Rig>(".",rosterRoot/relative);
    require50DU(racerRigs[racer]->podId==id,"original rig and roster identity differ");
   });
   coldRacerStage50DU(id,"collision owner construction",[&](){
    racerCollisions[racer]=std::make_unique<native_racer_collision50HW::Owner>();
   });
   const auto archive=std::filesystem::path("work/native/project/disc/PODS")/id/(id+".RES");
   coldRacerStage50DU(id,"resource archive verification",[&](){
    require50DU(pod_materials50ed_fixture::detail::sha256(f::readBytes(archive))==
        entry.at("resource_sha256").get<std::string>(),"original racer resource archive identity differs");
   });
   if(!racer)coldRacerStage50DU(id,"original display name",[&](){
    racerNames[racer]=native_racer50HW::loadVerifiedDisplayName(
        "evidence/originalRoster50HW/original-eight-visuals",id,
        entry.at("resource_sha256").get<std::string>());
   });
   coldRacerStage50DU(id,"collision resource preparation",[&](){
    std::string error;
    const bool collisionReady=racerCollisions[racer]->prepare(
        {archive.string(),"Pods\\Pod"+id.substr(3)+"\\"},endpointPolicy,error);
    require50DU(collisionReady,error.empty()?"original racer collision load failed":error.c_str());
   });
   mainPairCapacity=std::max(mainPairCapacity,racerCollisions[racer]->main().rayCount);
   secondaryPairCapacity=std::max(secondaryPairCapacity,racerCollisions[racer]->secondary().rayCount);
  }
 }
 // Cold diagnostic setup only: replace retained generic0x84 fixture flags with
 // authenticated fresh graphics-model/source-node flags (49O creation proof).
 // This is before reset setters; never used to coerce a live cache into admission.
 const SceneNodeIndex49P rebaseRoots[3]={x.bindings.lifecycle.model1B4,
     x.bindings.visual.right1B8,x.bindings.visual.left1BC};
 const unsigned rebaseAsset[3]={2,1,0};
 SceneNodeIndex49P rebaseNodes[3][18]{};
 PodSceneRebaseModel50FB rebaseModels[3]{};
 for(unsigned component=0;component<3;++component){
  const auto root=static_cast<int>(rebaseRoots[component]);
  const auto& hierarchy=x.assets.hierarchy[rebaseAsset[component]];
  require50DU(hierarchy.nodeCount<=18,"source model rebase capacity");
  s.nodes[root].sceneFlags190=0x80;
  for(unsigned i=0;i<hierarchy.nodeCount;++i){
   rebaseNodes[component][i]=static_cast<SceneNodeIndex49P>(root+1+i);
   s.nodes[root+1+i].sceneFlags190=0xa4;
  }
  rebaseModels[component]={rebaseRoots[component],hierarchy,rebaseNodes[component],hierarchy.nodeCount};
 }


 authored_runtime_ground50cp_fixture::Request groundRequest{};
 groundRequest.archive=trackSession->archive;groundRequest.worldMember=trackSession->world->worldMember;
 groundRequest.mapSpline=trackSession->mapSpline;groundRequest.gridRank=trackSession->gridRank;
 groundRequest.archiveSha256=trackSession->archiveSha256;groundRequest.archiveBytes=trackSession->archiveBytes;
 auto ground=authored_runtime_ground50cp_fixture::load({4,16,32,64},groundRequest);
 require50DU(ground.lift==x.parameters.visual.verticalLimit7EC,"same actual Pod01 lift");
 std::vector<std::uint32_t> groundStack(ground.stackCount);
 const auto groundAllocations=::allocations;BodyMatrix48G pose=ground.pose;
 if(checkpointInput){
  recovery50FI::Diagnostic recoveryDiagnostic{};
  require50DU(recovery50FI::resolveGround(ground,*checkpointInput,16,0,
      groundStack.data(),groundStack.size(),pose,recoveryDiagnostic),
      "checkpoint actual ground resolution failed");
 }else{
  const auto groundProbe=authored_runtime_ground50cp_fixture::resolve(ground,16,0,groundStack.data(),groundStack.size());
  require50DU(groundProbe.phase==ResetProbePhase50AG::Hit,"actual grid ground height");
  for(unsigned i=0;i<4;++i)pose.v[12+i]=std::bit_cast<float>(groundProbe.staged[i]);
 }
 require50DU(::allocations==groundAllocations,"actual ground query without allocations");
 std::vector<track_runtime_storage50IG::Binding> trackBindings;
 std::vector<track_static_candidates50IG::Radii> originalStaticRadii;
 if(trackSession->originalStaticCandidates){
  track_static_candidates50IG::Bindings staticBindings{};
  staticBindings.firstIdentity=0x10000000u; // Explicit collision-object native namespace.
  staticBindings.collisionTypeTag=query.collisionTypeTag;staticBindings.groupTypeTag=query.groupTypeTag;
  staticBindings.preservedFlags31=static_cast<std::uint8_t>(qfilter.flags31&0xf0u);
  staticBindings.preservedFlagsF8=static_cast<std::uint8_t>(qfilter.flagsF8&0xc0u);
  track_static_candidates50IG::prepare(ground,staticBindings,candidates,originalStaticRadii,trackBindings);
  for(const auto& candidate:candidates)stackCount=std::max(stackCount,1+shape.view.maxDepth+candidate.maxDepth);
 }else{
 trackBindings.resize(candidates.size());
 for(std::size_t k=0;k<candidates.size();++k){const auto&j=row["candidates"][row["order"][k].get<std::size_t>()];const auto name=authored_runtime_ground50cp_fixture::lower(corpus["meshes"][j["mesh"].get<std::size_t>()]["name"].get<std::string>());const authored_runtime_ground50cp_fixture::Owner*match=nullptr;for(const auto&o:ground.owners)if(authored_runtime_ground50cp_fixture::lower(o.name)==name){bool equal=true;for(unsigned n=12;n<15;++n)if(o.instance.world.v[n]!=candidates[k].candidate.collision.geometry.worldMatrix.v[n])equal=false;if(equal){match=&o;break;}}require50DU(match,"actual collider placement matches explicit candidate");trackBindings[k]={static_cast<std::size_t>(match-ground.owners.data()),&candidates[k]};auto&radiusOwner=meshes[j["mesh"].get<std::size_t>()];require50DU(radiusOwner.radii.size()==match->mesh.nodes.size(),"actual mesh radius extent");std::vector<std::uint32_t> topology(match->mesh.nodes.size()+match->mesh.faces.size());require50DU(prepareStaticTargetRadii49X(match->mesh,radiusOwner.radii.data(),radiusOwner.radii.size(),{topology.data(),topology.size()},radiusOwner.maxDepth)==Type2ResourceStatus49X::Complete,"radii prepared from owned actual mesh");candidates[k].maxDepth=radiusOwner.maxDepth;stackCount=std::max(stackCount,1+shape.view.maxDepth+radiusOwner.maxDepth);auto&v=candidates[k].candidate.collision;v.geometry.mesh=&match->mesh;v.geometry.worldMatrix=match->instance.world;v.filter.categoryA4=match->instance.category;v.sphere={{{match->instance.sphere.center.v[0],match->instance.sphere.center.v[1],match->instance.sphere.center.v[2]}},match->instance.sphere.radius};}
 }
 require50DU(candidates.size()==trackSession->terrainOwners&&ground.owners.size()==candidates.size(),
     "original track binding must cover every retained terrain owner");
 std::unique_ptr<gc_animated_world50IG::Owner> gcAnimatedWorld;
 gc_animated_world50IG::Diagnostic gcAnimationDiagnostic{};
 // Explicit same fresh-session lifetime as the reconstructed native RNG and
 // existing Dax sessionLazy. This is not a claim of original cross-race statics.
 gc_animated_world50IG::ServiceShared gcAnimationLazy{};
 const std::size_t animatedCandidateFirst=candidates.size();
 if(trackSession->originalStaticCandidates){
  gcAnimatedWorld=std::make_unique<gc_animated_world50IG::Owner>();
  require50DU(gcAnimatedWorld->load(trackSession->archive,s.animationShared.libm,
      s.animationShared.motion,gcAnimationDiagnostic),
      gcAnimationDiagnostic.message.empty()?"original GC animated resource load failed":gcAnimationDiagnostic.message.c_str());
  gc_animated_candidates50IG::prepare(*gcAnimatedWorld,candidates,trackBindings,
      query.collisionTypeTag,qfilter.flags31,qfilter.flagsF8);
  for(const auto& candidate:candidates)stackCount=std::max(stackCount,1+shape.view.maxDepth+candidate.maxDepth);
  std::cerr<<"GC ANIMATED WORLD: original opening/windmill owners=2, vertices=1195, indices=2337, surfaces=6, "
      "COL1 children=4, CHARACTER native mask=1, source node mappings=1/2 and 0/1; opening inactive, windmill loop15s.\n";
 }
 ground.audit=J{};
 // Cold, immovable, count-derived storage. Candidate order and every borrowed
 // source mesh/radius remain owned above; publication checks complete coverage.
 track_runtime_storage50IG::Owner trackStorageOwner;
 track_runtime_storage50IG::Diagnostic trackStorageDiagnostic{};
 require50DU(trackStorageOwner.prepare(ground,trackBindings.data(),trackBindings.size(),{},
     trackStorageDiagnostic),trackStorageDiagnostic.message.empty()?"original track storage preparation failed":
     trackStorageDiagnostic.message.c_str());
 auto& trackStorage=*trackStorageOwner.get();
 const auto& trackCounts=trackStorage.count;
 std::array<std::unique_ptr<track_runtime_storage50IG::Membership>,gc_animated_world50IG::ownerCount> gcAnimatedMembership;
 if(gcAnimatedWorld)for(auto& membership:gcAnimatedMembership)
  membership=std::make_unique<track_runtime_storage50IG::Membership>(trackCounts.membership);
 const auto terrainOwnerCount=trackCounts.terrain;
 auto* audioContactMaterials=trackStorage.contactMaterials.data();
 require50DU(setLocalMatrix49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.main190,pose)&&refreshSceneWorld49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.main190),"supplied initial pose");f::project(s,x.bindings);
 FreshPodGeometrySources49O sources{};sources.chariot=x.assets.hierarchy[2];sources.rightEngine=x.assets.hierarchy[1];sources.forward=x.assets.motions[4];sources.back=x.assets.motions[5];rr::assets::MotionBindingDiagnostic49O bindingDiagnostic{};
 require50DU(rr::assets::readMotionNodeBindings49O(x.assets.hierarchy[2],reinterpret_cast<const char*>(x.assets.vut[2].data()),x.assets.vut[2].size(),sources.chariotBindings,bindingDiagnostic),"actual motion bindings");
 FreshPodGeometryWorkspace49O geometryWorkspace{};FreshPodGeometryResult49O geometry{};FreshPodGeometryDiagnostic49O geometryDiagnostic{};
 require50DU(buildFreshPodGeometry49O(sources,geometryWorkspace,geometry,geometryDiagnostic),"actual pod geometry");
 const auto phy=runtime_startup50fl::readBytes("work/impactAuthoring49Y/Pod01.phy");
 // Original214954..214AA0 selects initial PHY class from the loaded track's
 // lowercase two-letter identity. Use the existing source selector, including
 // its sb/ra/sc exceptions; never carry the old TA-only class into a GC session.
 const auto sourceTrackLetter=[](char c){return c>='A'&&c<='Z'?char(c+'a'-'A'):c;};
 const char authoredTrackName[]={sourceTrackLetter(trackSession->world->code[0]),
     sourceTrackLetter(trackSession->world->code[1]),'\0'};
 AuthoredClass49N trackClass=AuthoredClass49N::A;
 require50DU(selectAuthoredClass49N(authoredTrackName,2,trackClass),
     "loaded original track's authored handling class unavailable");
 const AuthoredSelectionContext49N selection{trackClass,false,true,false,false,{}};
 auto authored=runtime_startup50fl::bind(phy,geometry,selection,x.parameters,s,x.selectors,x.secondaryInputs);
 // Player1A0858..5C preserves the fully authored aero350+100 as10A4.
 // Capture once, before the finish leaf changes the live steering coefficient.
 float authoredFinishSteering10A4=x.parameters.bodies.main.aero.steering100;
 auto secondaryPose=pose;for(unsigned i=0;i<3;++i){volatile float a=pose.v[i]*x.secondaryInputs.targetRelative2F0.v[0];volatile float b=pose.v[4+i]*x.secondaryInputs.targetRelative2F0.v[1];volatile float c=pose.v[8+i]*x.secondaryInputs.targetRelative2F0.v[2];volatile float ab=a+b;volatile float abc=ab+c;volatile float position=pose.v[12+i]+abc;secondaryPose.v[12+i]=position;}
 require50DU(setLocalMatrix49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.physical19C,secondaryPose)&&refreshSceneWorld49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.physical19C),"supplied secondary initial pose");
 require50DU(setLocalMatrix49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.model1B4,secondaryPose),"supplied initial displayed pose");f::project(s,x.bindings);
 ResetBoundNode50BA resetModelNodes[18]{};ResetBoundsWorkspace50BA resetBoundsWorkspace{};ModelBoundsResult49O resetModelBounds{};
 require50DU(x.assets.hierarchy[2].nodeCount==18,"actual chariot source node count");
 for(unsigned i=0;i<18;++i){rr::assets::PsgNode49O authoredNode{};require50DU(rr::assets::readPsgNode49O(x.assets.hierarchy[2],i,authoredNode),"actual reset model bound");const auto index=static_cast<std::int32_t>(x.bindings.lifecycle.model1B4)+1+static_cast<std::int32_t>(i);resetModelNodes[i].node=static_cast<SceneNodeIndex49P>(index);const auto expectedParent=static_cast<std::int32_t>(x.bindings.lifecycle.model1B4)+1+authoredNode.parent;require50DU(static_cast<std::int32_t>(s.nodes[index].parent180)==expectedParent,"source flat model parent mapping");for(unsigned j=0;j<3;++j){resetModelNodes[i].center140.v[j]=authoredNode.bounds[j];resetModelNodes[i].halfExtent150.v[j]=authoredNode.bounds[3+j];}}
 const auto beforeResetBounds=::allocations;
 if(!refreshResetModelBounds50BA({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.model1B4,resetModelNodes,18,resetBoundsWorkspace,resetModelBounds)){std::cerr<<"reset root flags "<<unsigned(s.nodes[static_cast<int>(x.bindings.lifecycle.model1B4)].sceneFlags190)<<" descendants";for(const auto&n:resetModelNodes)std::cerr<<' '<<unsigned(s.nodes[static_cast<int>(n.node)].sceneFlags190);std::cerr<<'\n';require50DU(false,"reset model cache admission");}
 require50DU(::allocations==beforeResetBounds,"reset model bounds no allocation");
 PodSceneRebaseWorkspace50FB rebaseWorkspace{};
 ModelBoundsResult49O rebaseBounds[3]{};PodSceneRebaseDiagnostic50FB rebaseDiagnostic{};
 if(!rebaseColdPodScene50FB({s.nodes,s.sceneNodeCount},rebaseModels,rebaseWorkspace,rebaseBounds,rebaseDiagnostic)){
  std::cerr<<"cold pod rebase error "<<unsigned(rebaseDiagnostic.error)
           <<" model "<<rebaseDiagnostic.model<<" node "<<rebaseDiagnostic.sourceNode<<'\n';
  require50DU(false,"source first-base pod scene rebase failed");
 }


 SecondaryActivationGlobals49V resetGlobals{};initializedSecondaryActivationGlobals49V(resetGlobals);resetMainRigidBody50AR(m,s.bodies.mainSphere,s.mainSupplement,resetGlobals);resetSecondaryActivation49V(s.bodies.secondary,s.secondarySupplement,resetGlobals);
 for(auto*body:{&m.components.root,&s.bodies.secondary.root}){require50DU(matrixToQuaternion48G(body->scene,body->body.orientation),"prepared initial orientation");std::memcpy(body->body.position.v,body->scene.v+12,12);}
 ResetLifecycleGlobals50AT resetLifecycle{};initializedResetLifecycleGlobals50AT(resetLifecycle);require50DU(resetLifecycleRecords50AT(s.bodies.vehicle838,s.lifecycle,resetLifecycle),"source reset lifecycle records");s.fields.applicationByte58=2;m.controls.player.flagsB9C=0x80;
 SpringGeneratedParameters48K settings{};settings.impact54=true;settings.friction50=true;settings.impact.disableLeverArms5C=true;settings.impact.correctPenetration64=true;settings.impact.normalForceScale98=.97f;settings.impact.positionCorrectionScaleCC=.2f;
 SpringResponseParameters48L response{};response.maxSpeed94=10000;rr::assets::SpringCollisionAuthoring50F collision{};rr::assets::PodPhysicsDiagnostic49N diagnostic{};
 require50DU(rr::assets::readSpringCollision50F(reinterpret_cast<const char*>(phy.data()),phy.size(),collision,diagnostic)&&rr::assets::applySpringCollision50F(collision,x.parameters.bodies.main.body.mass,settings,response),"authored response settings");
 const auto tuning=[&](){
  if(!trackSession->tuningInArchive)return runtime_startup50fl::readBytes(trackSession->impactTuning);
  rr::assets::ResArchive48C source;rr::assets::DecodeError48C error;std::vector<std::uint8_t> bytes;
  require50DU(rr::assets::decodeResource48C(runtime_startup50fl::readBytes(trackSession->archive),source,error)&&
      rr::assets::extractResMember48C(source,trackSession->impactTuning,bytes,error),
      "actual selected original track tuning unavailable");return bytes;
 }();require50DU(rr::assets::readEnvironmentImpact49Y(reinterpret_cast<const char*>(tuning.data()),tuning.size(),x.parameters.impactTuning,diagnostic),"actual selected-track impact tuning");x.parameters.impactTuningBound=true;
 if(originalAiEnabled){
  require50DU(rr::assets::readAIEnvironmentImpact49Y(reinterpret_cast<const char*>(tuning.data()),
      tuning.size(),x.parameters.impactTuning,diagnostic),"original AI environment tuning unavailable");
  require50DU(rr::assets::readAiTrackTuning50IU(reinterpret_cast<const char*>(tuning.data()),
      tuning.size(),originalAiTuning,originalAiAuthoringDiagnostic),"original AI track tuning unavailable");
 }
 if(dynamicContactEnabled){
  require50DU(rr::assets::readDynamicImpactAuthoring50IW(reinterpret_cast<const char*>(tuning.data()),
      tuning.size(),dynamicTuning,diagnostic),"original dynamic contact tuning unavailable");
  require50DU(rr::assets::readFinalImpactAuthoring49Y(reinterpret_cast<const char*>(tuning.data()),
      tuning.size(),dynamicFinalTuning,diagnostic),"original final impact tuning unavailable");
 }
 rr::assets::DefenseAuthoring49Y defense{};require50DU(rr::assets::readDefenseAuthoring49Y(reinterpret_cast<const char*>(phy.data()),phy.size(),defense,diagnostic)&&defense.baseDefense.present,"explicit base-profile defense");s.fields.defenseCF0=defense.baseDefense.value;
 // Profile overwrite and authored class selection are separate source gates.
 // Use the same UseValues/class decision as the already-bound physics.
 rr::assets::RepairAuthoring50A repairAuthoring{};
 require50DU(!selection.applyProfileOverwrite&&rr::assets::readRepairAuthoring50A(
     reinterpret_cast<const char*>(phy.data()),phy.size(),repairAuthoring,diagnostic),
     "explicit no-profile repair authoring unavailable");
 using RepairKey=rr::assets::RepairAuthoringKey50A;
 float currentRepair=0.f;
 if(authored.usedAuthoredClassValues){
  RepairKey key=RepairKey::Count;
  switch(authored.selectedClass){
   case AuthoredClass49N::A:key=RepairKey::RepairA;break;
   case AuthoredClass49N::B:key=RepairKey::RepairB;break;
   case AuthoredClass49N::C:key=RepairKey::RepairC;break;
  }
  require50DU(key!=RepairKey::Count,"unknown authored repair class");
  const auto& amount=repairAuthoring.values[static_cast<unsigned>(key)];
  require50DU(amount.present,"selected authored repair class missing");
  currentRepair=amount.value;
 }else{
  const auto& minimum=repairAuthoring.values[static_cast<unsigned>(RepairKey::MinRepair)];
  const auto& maximum=repairAuthoring.values[static_cast<unsigned>(RepairKey::MaxRepair)];
  const auto& baseAbility=repairAuthoring.values[static_cast<unsigned>(RepairKey::BaseAbilityRepair)];
  const auto& maxAbility=repairAuthoring.values[static_cast<unsigned>(RepairKey::MaxAbilityRepair)];
  require50DU(minimum.present&&maximum.present&&baseAbility.present&&maxAbility.present&&
      baseAbility.value==1.0f&&maxAbility.value==1.0f,
      "interpolated repair requires retained equal-unit ability endpoints");
  volatile float difference=maxAbility.value-baseAbility.value;
  volatile float delta=0.0f*difference;
  volatile float ability=baseAbility.value+delta;
  volatile float range=maximum.value-minimum.value;
  volatile float scaled=ability*range;
  currentRepair=minimum.value+scaled;
 }
 require50DU(std::isfinite(currentRepair)&&currentRepair>0.f,"authored repair amount unavailable");
 s.fields.repairCF4=currentRepair;
 BoostParameters50FK boostParameters{};float boostCoolingCF8=0.f;
 require50DU(boost50fk_fixture::bind(reinterpret_cast<const char*>(phy.data()),phy.size(),
     selection,authored,boostParameters,boostCoolingCF8),"actual boost authoring unavailable");
 BoostState50FK boostState{};BoostDiagnostic50FK boostDiagnostic{};
 if(!playerUsesRosterRig)std::cerr<<"ORIGINAL HANDLING: slot=0 track="<<authoredTrackName
     <<" class="<<static_cast<unsigned>(authored.selectedClass)
     <<" class_values="<<authored.usedAuthoredClassValues
     <<" max_thrust="<<x.parameters.bodies.main.engine.maxThrust
     <<" top_speed_body="<<x.parameters.bodies.main.engine.theoreticalTopSpeed
     <<" steering100="<<x.parameters.bodies.main.aero.steering100
     <<" repair="<<s.fields.repairCF4<<" cooling="<<boostCoolingCF8<<'\n';

 // Explicit native namespace shared with the retained collider/query owners.
 // These IDs/masks are host bindings, not reconstructed guest addresses.
 terrain_probe_authoring50fq_fixture::Owner terrainOwner{};
 terrain_probe_authoring50fq_fixture::Bindings terrainBindings{};
 terrainBindings.identity=0xf0000003u;
 terrainBindings.physicsBodyCategory=2;terrainBindings.staticObjectCategory=4;
 terrainBindings.floorCategory=16;terrainBindings.tubeCategory=64;
 terrainBindings.collisionTypeTag=query.collisionTypeTag;
 terrainBindings.groupTypeTag=query.groupTypeTag;
 terrainBindings.flags31=3;terrainBindings.preservedFlagsF8=0;
 std::string terrainAuthoringError;
 const bool terrainAuthored=terrain_probe_authoring50fq_fixture::prepare(terrainOwner,query,
     x.parameters,phy,terrainBindings,terrainAuthoringError);
 require50DU(terrainAuthored,terrainAuthoringError.c_str());
 for(unsigned axis=0;axis<3;++axis)x.parameters.bodies.main.probeLocal140[axis]=terrainOwner.lowerEndpoint140.v[axis];
 x.parameters.bodies.main.probeLocal140[3]=1.f;
 // Preserve the actual fresh constructor records before the player's probe
 // is enabled. Fresh opponent slots install these cold records before enabling
 // their own authored probe; no already-running state is reset this way.
 require50DU(s.phase==VehicleFramePhase49W::Ready&&m.frameFactor==0.f&&
     !s.terrainProbe.enabled&&!s.bodies.main.domain.probe100,
     "source pre-enable terrain constructor state unavailable");
 const auto freshTerrainProbe=s.terrainProbe;
 const auto freshMainProbe=s.bodies.main.components.probe348;
 const auto freshProbe100=s.bodies.main.domain.probe100;
 require50DU(enableVehicleTerrain50FP(s,x.bindings,terrainOwner.initial,
     terrainOwner.follow,x.workspace,x.diagnostic),"enable source-authored terrain probe");

 StaticSpringState50G spring{};spring.records.generatedCapacity88=40;SpringRecord48N records[60]{},queryRecords[60]{},scratchRecords[60]{};

 const J*secondaryCase=nullptr;for(const auto&item:corpus["cases"])if(item["name"]==trackSession->secondaryCollisionCase)secondaryCase=&item;
 require50DU(secondaryCase,"actual chariot query present");const auto&sqj=(*secondaryCase)["query"];
 auto secondaryShape=cold::shape(corpus["shapes"][sqj["shape"].get<std::size_t>()]);auto secondaryFilter=cold::filter(sqj["filter"]);
 Type2StaticQuery49X secondaryQuery{};secondaryQuery.resource=&secondaryShape.view;secondaryQuery.filter=&secondaryFilter;secondaryQuery.queryScene.value=78;
 secondaryQuery.collisionTypeTag=sqj["collision_tag"];secondaryQuery.groupTypeTag=sqj["group_tag"];secondaryQuery.callbackEnabled=sqj["callback"];
 cold::words(secondaryQuery.copied40.data(),sqj["copied40"]);cold::words(secondaryQuery.copied50.data(),sqj["copied50"]);
 secondaryQuery.skipMask=(*secondaryCase)["masks"][0];secondaryQuery.dynamicMask=(*secondaryCase)["masks"][1];secondaryQuery.materialMask=(*secondaryCase)["masks"][2];
 if(gcAnimatedWorld)require50DU(gc_animated_candidates50IG::admitNonBodyResponse(*gcAnimatedWorld,
     candidates.data(),candidates.size(),animatedCandidateFirst,query,secondaryQuery),
     "GC original CHARACTER non-body spring response routing differs");
 if(!trackSession->originalStaticCandidates)require50DU((*secondaryCase)["order"]==row["order"]&&(*secondaryCase)["candidates"]==row["candidates"],"same explicit ordered terrain inputs");
 StaticSpringState50G secondarySpring{};secondarySpring.records.generatedCapacity88=60;
 SpringRecord48N secondaryRecords[60]{},secondaryQueryRecords[60]{},secondaryScratchRecords[60]{};
 if(raceEnabled)require50DU(matchesEndpointPolicy(secondaryShape.view),
     "retained secondary endpoint-W host policy differs");
 secondaryPairCapacity=std::max(secondaryPairCapacity,secondaryShape.view.rayCount);
 std::size_t targetDepth=0;
 for(const auto& candidate:candidates)targetDepth=std::max(targetDepth,candidate.maxDepth);
 for(std::size_t racer=0;racer<racerCollisions.size();++racer)if(racerCollisions[racer]){
  coldRacerStage50DU(racerRigs[racer]?racerRigs[racer]->podId:playerPodId,"collision scratch preparation",[&](){
   native_racer_collision50HW::ScratchRequirements required{};
   require50DU(racerCollisions[racer]->requirements(targetDepth,required),"original query scratch extent rejected");
   stackCount=std::max({stackCount,required.mainStack,required.secondaryStack});
  });
 }
 const auto srays=secondaryPairCapacity;
 std::vector<SpringCumulativePair48N> sip(srays),ssp(srays),sdp(srays);std::vector<Type2PairMetadata49X> sim(srays),ssm(srays),sdm(srays);
 std::vector<TerrainCandidateEffects48J> sse(candidates.size()),sde(candidates.size());std::vector<Type2WorkItem49X> ss(stackCount+secondaryShape.view.maxDepth);
 Type2StaticInitial49X si{};si.records=secondarySpring.records;si.pairs=sip.data();si.metadata=sim.data();si.pairSlots=srays;si.recordSlots=secondaryRecords;si.recordCount=60;
 Type2StaticWorkspace49X sw{ssp.data(),srays,ssm.data(),srays,secondaryScratchRecords,60,sse.data(),sse.size(),ss.data(),ss.size(),nullptr,0};
 Type2StaticDestination49X sd{sdp.data(),srays,sdm.data(),srays,secondaryQueryRecords,60,sde.data(),sde.size()};Type2StaticResult49X secondaryResult{};
 SpringGeneratedParameters48K secondarySettings{};secondarySettings.impact54=true;secondarySettings.impact.disableLeverArms5C=false;secondarySettings.impact.correctPenetration64=true;secondarySettings.impact.normalForceScale98=1;secondarySettings.impact.positionCorrectionScaleCC=.025f;
 SpringResponseParameters48L secondaryResponse{};secondaryResponse.maxSpeed94=10000;
 require50DU(rr::assets::applySpringCollision50F(collision,x.parameters.bodies.secondary.body.mass,secondarySettings,secondaryResponse),"authored secondary response settings");
 const auto rays=mainPairCapacity;
 std::vector<SpringCumulativePair48N> initialPairs(rays),scratchPairs(rays),pairs(rays);std::vector<Type2PairMetadata49X> initialMetadata(rays),scratchMetadata(rays),metadata(rays);
 std::vector<TerrainCandidateEffects48J> scratchEffects(candidates.size()),effects(candidates.size());std::vector<Type2WorkItem49X> stack(stackCount);
 Type2StaticInitial49X initial{};initial.records=spring.records;initial.pairs=initialPairs.data();initial.metadata=initialMetadata.data();initial.pairSlots=rays;initial.recordSlots=records;initial.recordCount=40;
 Type2StaticWorkspace49X workspace{scratchPairs.data(),rays,scratchMetadata.data(),rays,scratchRecords,40,scratchEffects.data(),scratchEffects.size(),stack.data(),stack.size(),nullptr,0};
 Type2StaticDestination49X destination{pairs.data(),rays,metadata.data(),rays,queryRecords,40,effects.data(),effects.size()};Type2StaticResult49X queryResult{};

 // Explicit diagnostic movement-query state and complete configured static order.
 // The late movement detector retains its separate explicit terrain order.
 Type3QueryInput48P movement{};movement.filter=qfilter;movement.filter.identity=0xf0000001u;movement.filter.categoryA4=2;movement.filter.acceptMaskB8=4;movement.filter.flagsF8=6;movement.filter.flags31=1;
 movement.pairCount=1;movement.normalMode=0;movement.callbackEnabled=false;movement.collisionTypeTag=query.collisionTypeTag;movement.groupTypeTag=query.groupTypeTag;movement.dynamicMask=2;movement.sentinel={{0,0,0,1}};
 movement.copied40=query.copied40;movement.copied50=query.copied50;for(unsigned i:{0u,5u,10u,15u})movement.bodyScene.v[i]=1;
 auto& movementCandidates=trackStorage.movement;
 MovementRays50N movementInitial,movementScratch,movementDestination;SpringVector4_48N endpoints[2]{};std::uint32_t endpointScenes[2]{};
 std::vector<TerrainCandidateEffects48J> movementScratchEffects(candidates.size()),movementEffects(candidates.size());std::vector<std::uint32_t> movementStack(stackCount);
 Type3QuerySource48P movementSource{movementInitial.source(),endpoints,2,endpointScenes,2,nullptr,0};
 Type3QueryWorkspace48P movementWorkspace{{movementScratch.view(),nullptr,0,movementScratchEffects.data(),movementScratchEffects.size()},movementStack.data(),movementStack.size()};
 Type3QueryBuffers48P movementOutput{movementDestination.view(),nullptr,0,movementEffects.data(),movementEffects.size()};Type3QueryResult48P movementResult{};std::uint32_t movementFlag=0;
 // Preserve the separate ordered movement registry, prepared once before
 // publication. Hot gather copies only through capacity-checked native views.
 auto& registry=trackStorage.movementRegistry;
 auto* registeredMovement=trackStorage.registeredMovement.data();
 auto* registeredIndices=trackStorage.registeredIndices.data();
 auto& registeredCount=trackStorage.registeredCount;
 auto* probeIndices=trackStorage.probeIndices.data();auto& probeCount=trackStorage.probeCount;
 auto& registrationBlocks=trackStorage.movementBlocks;
 auto& registrationView=trackStorage.movementView;
 auto gatherRegistered=[&](){return trackStorage.gatherMovement();};

 // Source spring constructor295424 ->11B570(query,2): enable querying,
 // disable target participation and temporary-registration bits. Both spring
 // query objects may be registered without becoming their own static targets.
 qfilter.flagsF8=static_cast<std::uint8_t>((qfilter.flagsF8&~0x0eu)|2u);
 secondaryFilter.flagsF8=static_cast<std::uint8_t>((secondaryFilter.flagsF8&~0x0eu)|2u);
 // The isolated corpus cases both used identity100. These are now two live
 // objects in one collector table, so assign a distinct opaque native identity
 // to secondary; this is not a recovered guest address or a collision filter.
 constexpr std::uint32_t secondaryNativeIdentity=0xf0000002u;
 require50DU(qfilter.identity!=secondaryNativeIdentity&&movement.filter.identity!=secondaryNativeIdentity,
     "reserved secondary query identity conflicts with another native query");
 for(const auto& candidate:candidates)require50DU(candidate.candidate.collision.filter.identity!=secondaryNativeIdentity,
     "reserved secondary query identity conflicts with terrain");
 secondaryFilter.identity=secondaryNativeIdentity;
 // Static references preserve authored candidate order; three real query
 // owners follow them. No fixed TA zone/collider dimensions remain here.
 const CollectionRef50BX mainQueryRef=trackCounts.mainQuery,
     secondaryQueryRef=trackCounts.secondaryQuery,terrainQueryRef=trackCounts.terrainQuery;
 const auto collectionObjectCount=trackCounts.objects,zoneCount=trackCounts.zones;
 auto* collectionObjects=trackStorage.objects.data();
 auto* collectionSource=trackStorage.source.data();
 auto* collectionSecond=trackStorage.second.data();
 auto* treeSource=trackStorage.treeSource.data();auto* treeSecond=trackStorage.treeSecond.data();
 auto& zoneScratch=trackStorage.registrationScratch;
 auto& zoneLifecycle=trackStorage.lifecycle;auto& zoneQueue=trackStorage.queue;
 auto& liveWorld=trackStorage.world;
 // Cold original TunnelTrack owners, separate from the grid-start projection.
 // Share the existing serial source-libm owner with49T; keep every node/NextN.
 auto navigationMap=std::make_unique<TrackNavigation50EU>();
 auto navigationAI=std::make_unique<TrackNavigation50EU>();
 const char* autoSetting=std::getenv("RR_AUTODRIVE");
 bool autoDriveEnabled=autoSetting&&std::string(autoSetting)=="1";
 require50DU(!allAiEnabled||!autoDriveEnabled,
     "all-original-AI ownership requires RR_AUTODRIVE=0; camera slot0 is an AI");
 bool autoStopReported=false;
 rr::host::CourseSpeedLimits50GK courseLimits{};
 rr::host::CourseDriverTuning50GK courseTuning{};
 // Default automated demonstrations use authored speed limits. The previous
 // conservative host policy remains an explicit replay option, never a change
 // to native physics or the simulation clock.
 courseTuning.authoredSpeedOnly=true;
 const char* autoPace=std::getenv("RR_AUTODRIVE_PACE");
 if(autoPace){
  require50DU(std::string(autoPace)=="authored"||std::string(autoPace)=="conservative",
      "autodrive pace must be authored or conservative");
  courseTuning.authoredSpeedOnly=std::string(autoPace)=="authored";
 }
 // Original AI leaf173798..1737A4 compares desired speed against this
 // conversion times cached body speed. Reuse its units, not its unjoined
 // race/vehicle target selection. This host driver measures planar speed.
 if(courseTuning.authoredSpeedOnly)
  courseTuning.speedLimitToBodyScale=1.f/std::bit_cast<float>(std::uint32_t{0x3f2e8bc1u});
 rr::host::CourseDriverState50GK courseState{};
 rr::host::CourseDriverDiagnostic50GK courseDiagnostic{};
 rr::host::CourseDriverOutput50GK courseOutput{};
 if(track_session_assets50IG::codeEquals(trackSession->world->code,"GC")){
  require50DU(rr::host::readCourseSteeringLookahead50GK(
      reinterpret_cast<const char*>(tuning.data()),tuning.size(),
      courseTuning.authoredSteeringLookaheadSeconds,courseDiagnostic),
      "actual GC AIDriving.Lookahead unavailable for host steering preview");
  courseTuning.fullThrottleSteeringRange=true;
 }
 const char* autoPolarity=std::getenv("RR_AUTODRIVE_POLARITY");
 if(autoPolarity){
  require50DU(std::string(autoPolarity)=="1"||std::string(autoPolarity)=="-1","autodrive polarity must be 1 or -1");
  courseTuning.steeringPolarity=std::string(autoPolarity)=="-1"?-1.f:1.f;
 }
 WrongWayProgressContinuation50GO wrongWayContinuation{};
 WrongWayProgressReceipt50GO wrongWayReceipt{};
 WrongWayProgressDiagnostic50GO wrongWayDiagnostic{};
 std::uint64_t wrongWayUpdates=0,wrongWayActiveTicks=0;
 // Explicit native-host normal mode; no original app34C constructor is claimed.
 constexpr std::uint32_t applicationWrongWayBypass34C=0;
 lap_drive50GJ::Owner lapOwner;lapOwner.continuity=&lapContinuity;lapOwner.fields=&s.fields;
 auto liveAudio=std::make_unique<live_audio50HF::Owner>();
 auto raceAudio=std::make_unique<race_audio50HQ::Owner>();
 auto sourceAnnouncer=std::make_unique<source_announcer50IE::Owner>();
 auto raceMusic=std::make_unique<race_music50IE::Owner>();
 std::unique_ptr<race_mode1_host50ID::Owner> mode1;
 auto materialAudio=std::make_unique<material_audio50HI::Provider>();
 // This owner dies before the live mixer and owns only voices96..109.
 auto opponentAudio=std::make_unique<opponent_audio50HX::Owner>();
 const char* audioSetting=std::getenv("RR_AUDIO");
 const bool audioEnabled=!(audioSetting&&std::string(audioSetting)=="0");
 require50DU(!audioEnabled||trackSession->materialAudio,
     "Selected original track material audio provider is unavailable; RR_AUDIO=0 must be explicit");
 require50DU(!dynamicContactEnabled||audioEnabled,"dynamic source contacts require genuine audio providers");
 require50DU(!mode1Requested||audioEnabled,
     "mode1 requires original audio providers; use RR_RACE_MODE=2 explicitly for RR_AUDIO=0");
 rr::platform::AudioOutputDiagnostic50HD audioDiagnostic{};
 PodSpeechCounts50HG speechCounts{};PodSpeechState50HG speechState{};
 // Selected-player emitter state only; opponent bank visits never mutate it.
 // Fresh session reconstructs original19EBA0 values. Pause retains its timers.
 EngineDamageState50IA engineDamage{};
 std::uint64_t engineDamageUpdates=0,engineDamageDraws=0,engineDamageDips=0;
 std::uint64_t engineDamageReturns=0,engineDamageCompletions=0;
 DamageSpeechEvent50HG pendingSpeech{};
 float playerSpeechChance=0.f;
 SpeechPosition50HG audioListener{};bool audioListenerReady=false;
 std::uint64_t speechEvents=0,speechCues=0,announcerRequestsUnjoined=0;
 if(audioEnabled){
  const char* suppliedAudioRoot=std::getenv("RR_AUDIO_ROOT");
  const std::filesystem::path audioRoot=suppliedAudioRoot?suppliedAudioRoot:
      "work/native/project/disc/AUDIO";
  std::string audioError;
  require50DU(liveAudio->prepare("work/native/project/disc",audioRoot,audioError,playerRacer60,loadedAudioSettings),
      audioError.empty()?"native audio bank preparation failed":audioError.c_str());
  auto* materialMixer=liveAudio->mixerForMaterialOwner();
  auto* audioGroups=liveAudio->groupsForProviders();
  auto* sharedAnnouncer=liveAudio->sharedAnnouncer();
  require50DU(audioGroups&&sharedAnnouncer,"shared native audio group/episode owner unavailable");
  if(raceEnabled)require50DU(materialMixer&&raceAudio->prepare("work/native/project/disc",
      audioRoot,*materialMixer,*audioGroups,*sharedAnnouncer,audioError,playerRacer60),
      audioError.empty()?"original race audio preparation failed":audioError.c_str());
  if(mode1Requested||dynamicContactEnabled){
   require50DU(materialMixer&&sourceAnnouncer->prepare("work/native/project/disc",audioRoot,
       *materialMixer,*audioGroups,*sharedAnnouncer,sessionLazy,applicationClock,
       s.noise.sharedRandomState,audioError,playerRacer60),
       audioError.empty()?"source Dax cold preparation failed; explicit fallback RR_RACE_MODE=2":audioError.c_str());
   require50DU(materialMixer&&raceMusic->prepare("work/native/project/disc",audioRoot,
       *materialMixer,*audioGroups,audioError,dynamicContactEnabled),
       audioError.empty()?"source music cold preparation failed; explicit fallback RR_RACE_MODE=2":audioError.c_str());
   require50DU(sourceAnnouncer->prepared()&&raceMusic->bank().ready==
       (dynamicContactEnabled?race_music50IE::totalClipCount:race_music50IE::clipCount),
       "mode1 source providers/music PCM incomplete; no silent fallback (RR_RACE_MODE=2 is explicit)");
   require50DU(raceAudio->setHostSpeechEnabled(false),"mode1 HOST Dax suppression binding failed");
   mode1Enabled=mode1Requested;
   if(dynamicContactEnabled){
    const auto bytes=runtime_startup50fl::readBytes(audioRoot/"SPCHPCT.INI");
    require50DU(bindOriginalDynamicContactChances50IW(bytes.size(),
        pod_materials50ed_fixture::detail::sha256(bytes).c_str(),dynamicEventBindings),
        "authentic original collision speech chances unavailable");
    //21080C..210818 vehicle33C is the same application28C->10C event owner.
    dynamicEventBindings.eventOwner33CBound=true;
   }
  }
  require50DU(materialMixer&&materialAudio->prepare("work/native/project/disc",audioRoot,
      *materialMixer,64,audioError,trackSession->originalStaticCandidates?material_audio50HI::Track::gc:material_audio50HI::Track::ta),
      audioError.empty()?"native material audio preparation failed":audioError.c_str());
  if(trackSession->originalStaticCandidates){
   //271600 applies actual AUB assignments before the creator publishes A9.
   for(std::size_t i=0;i<trackBindings.size();++i){
    const auto& binding=trackBindings[i];
    // Animated CHARACTER owners retain constructor materialA9=0; AUB's
    // static SCB assignments do not index their sentinel originalOwner.
    if(binding.animated)continue;
    require50DU(binding.originalOwner<ground.owners.size(),"GC static material owner index outside authored ground");
    const auto& original=ground.owners[binding.originalOwner];
    require50DU(rr::assets::applyTrackMaterialAssignments50HI(materialAudio->catalog(),ground.world,
        original.zone,original.ordinal,trackStorage.contactMaterials[i],audioError),
        audioError.empty()?"original GC contact material assignment failed":audioError.c_str());}
  }
  require50DU(race_audio_groups50IC::bindProviderRange(*audioGroups,
      race_audio_groups50IC::ProviderRole::MaterialCollision,64,material_audio50HI::soundCount),
      "material group ownership rejected");
  if(materialAudio->extraVoiceCount())require50DU(race_audio_groups50IC::bindProviderRange(*audioGroups,
      race_audio_groups50IC::ProviderRole::MaterialCollision,material_audio50HI::extraFirstVoice,
      materialAudio->extraVoiceCount()),"extra original material sound group ownership rejected");
  if(raceEnabled){
   std::array<opponent_audio50HX::Source,opponent_audio50HX::maxOpponents> sources{};
   for(std::size_t racer=1;racer<raceRacerCount;++racer)
    sources[racer-1]={racerRigs[racer]->podId,&racerRigs[racer]->phy};
   const bool prepared=opponentAudio->prepare(sources,audioRoot,*materialMixer,audioError);
   require50DU(prepared,audioError.empty()?"original opponent engine audio preparation failed":audioError.c_str());
   require50DU(race_audio_groups50IC::bindProviderRange(*audioGroups,
       race_audio_groups50IC::ProviderRole::EngineBinderBoost,
       opponent_audio50HX::firstVoice,opponent_audio50HX::voiceCount),
       "opponent engine group ownership rejected");
   const auto& opponentBank=opponentAudio->bank();
   std::cerr<<"Opponent engine audio ready/missing/unsupported/invalid "<<opponentBank.ready<<'/'
       <<opponentBank.missing<<'/'<<opponentBank.unsupported<<'/'<<opponentBank.invalid
       <<" unique original samples; exclusive voices96..109; host distance attenuation.\n";
   for(const auto& mapping:opponentAudio->mappings())
    std::cerr<<"OPPONENT AUDIO "<<mapping.podId<<" engine="<<mapping.engineName
        <<" binder="<<mapping.binderName<<'\n';
  }
  speechCounts=liveAudio->speechCounts();
  require50DU(pod_speech50HG::readPlayerSpeechChance(playerSpeechChance,audioError,
      audioRoot/"SPCHPCT.INI"),audioError.empty()?"actual speech probability missing":audioError.c_str());
  require50DU(initializePodSpeech50HG(speechCounts,s.noise.sharedRandomState,speechState)==
      PodSpeechStatus50HG::Complete,"native speech cursor initialization failed");
  const auto& bank=liveAudio->bank();
  std::cerr<<"Audio bank ready/missing/unsupported/invalid "<<bank.ready<<'/'<<bank.missing
      <<'/'<<bank.unsupported<<'/'<<bank.invalid<<" of "<<bank.clips.size()<<'\n';
  const auto& audioConfig=liveAudio->audioSettings();
  std::cerr<<"Audio configured original levels effects/music/character/Dax "
      <<audioConfig.effects11020<<'/'<<audioConfig.music11024<<'/'
      <<audioConfig.character11028<<'/'<<audioConfig.announcer1102C
      <<"; no saved audio preferences loaded; "<<(mode1Enabled?"source mode1":"HOST mode2")
      <<" Dax policy, shared voice84 completion.\n";
  lapOwner.audioContext=liveAudio.get();
  lapOwner.audioDispatch=[](void* context,std::uint32_t key,std::uint32_t flags,
                           std::uint32_t a3)noexcept{
   const auto result=static_cast<live_audio50HF::Owner*>(context)->dispatchLap(key,flags,a3);
   return lap_drive50GJ::AudioDispatchResult{result.completed(),
       result.status==pod_audio50HE::DispatchStatus::CompletedStarted,
       result.status==pod_audio50HE::DispatchStatus::CompletedUnavailable,result.originalV0};
  };
 }
 TrackNavigationDiagnostic50EU navigationDiagnostic{};
 auto reportNavigation=[&](bool ok,const char* operation){
  if(!ok)std::cerr<<operation<<" navigation error "
      <<static_cast<unsigned>(navigationDiagnostic.error)<<" source-stage 0x"
      <<std::hex<<navigationDiagnostic.sourcePC<<std::dec
      <<" node "<<navigationDiagnostic.node<<'\n';
  return ok;
 };
 {
  rr::assets::ResArchive48C navigationArchive;rr::assets::DecodeError48C error;
  const auto bytes=runtime_startup50fl::readBytes(trackSession->archive);
  require50DU(rr::assets::decodeResource48C(bytes,navigationArchive,error),
      "decode actual selected-track navigation archive");
  auto scratch=std::make_unique<TrackNavigationScratch50EU>();
  auto readNavigation=[&](const char* member,TrackNavigation50EU& destination){
   std::vector<std::uint8_t> text;
   require50DU(rr::assets::extractResMember48C(navigationArchive,member,text,error),
       "extract actual M/A navigation spline");
   require50DU(reportNavigation(readTrackNavigation50EU(
       reinterpret_cast<const char*>(text.data()),text.size(),s.animationShared.libm,
       *scratch,destination,navigationDiagnostic),member),
       "prepare source navigation gates");
   if(originalAiEnabled)require50DU(rr::assets::readAiTrackMetadata50IU(
       reinterpret_cast<const char*>(text.data()),text.size(),destination,
       &destination==navigationAI.get()?originalAiMetadata:originalAiMapMetadata,
       originalAiAuthoringDiagnostic),"original AI/map metadata unavailable");
   if(&destination==navigationAI.get())
    require50DU(rr::host::readCourseSpeedLimits50GK(reinterpret_cast<const char*>(text.data()),
        text.size(),destination.track.count,courseLimits,courseDiagnostic),"actual selected A spline speed metadata rejected");
   require50DU(reportNavigation(bindTrackNavigationZones50EU(
       liveWorld,destination,navigationDiagnostic),member),
       "bind source navigation portal indices");
  };
  readNavigation(trackSession->mapSpline,*navigationMap);
  require50DU(prepareLapProgressTrack50GF(*scratch,lapOwner.track,lapOwner.diagnostic),
      "actual authored lap progress markers rejected");
  readNavigation(trackSession->aiSpline,*navigationAI);
 }
 if(!lapContinuity.initialized){
  lapContinuity.state.vehicleCD8=s.fields.impactFlagsCD8;
  initializePlayerLapRaceFields50GF(lapContinuity.state);
  initializePlayerLapCursor50GF(lapContinuity.state);
  lapContinuity.initialized=true;
 }
 // Actual owned reset-prefix projection; dynamic wrong-way1A2E30 is not joined.
 resetUndertrackPlayerPrefix50ET(lapContinuity.prefix);
 s.fields.impactFlagsCD8=static_cast<std::uint8_t>((s.fields.impactFlagsCD8&~7u)|
     (lapContinuity.state.vehicleCD8&7u));
 std::string lapSoundError;
 require50DU(lap_sound50GH::prepareDisabledFromPod01(lapOwner.sound,lapOwner.provider,lapSoundError,
     playerArchive,playerRacer60),
     "actual event sound owner preparation failed");
 std::cerr<<"HOST LAPS uses authored markers and explicit config mode2; lap audio provider="
     <<(audioEnabled?"native PCM (see asset availability)":"explicitly disabled")
     <<"; source wrong-way progression enabled; race-finish rules remain unjoined.\n";
 TrackNavigationProgress50EU navigationProgress{};
 initializeTrackNavigationProgress50EU(navigationProgress);
 //20F5C0..20F5C8 initializes CA0=(0,0,0,1). The first gate query therefore
 // takes1A0940's explicit zero-XZ miss; do not seed this with a guessed pose.
 float navigationCA0[4]{0.f,0.f,0.f,1.f};
 const CollectionRef50BX initialCollisionZone=checkpointInput?checkpointInput->primaryZone:trackSession->initialCollisionZone;
 if(checkpointInput){
  require50DU(checkpointInput->zoneCount==liveWorld.zoneCount,"checkpoint world namespace changed");
  require50DU(checkpointInput->progress.reset.vehicleC80<navigationAI->track.count&&
      checkpointInput->progress.vehicleC84<navigationAI->track.count&&
      checkpointInput->progress.reset.vehicleC88<navigationMap->track.count&&
      checkpointInput->progress.reset.vehicleC8C<navigationMap->track.count&&
      checkpointInput->selection.anchorNode<navigationMap->track.count&&
      navigationMap->nodeIndex78[checkpointInput->selection.anchorNode]==checkpointInput->nodeIndex78,
      "checkpoint navigation identity changed");
  recovery50FI::Diagnostic recoveryDiagnostic{};
  require50DU(recovery50FI::restoreContinuity(*checkpointInput,s.fields,navigationProgress,
      navigationCA0,pose,recoveryDiagnostic),"checkpoint continuity restore failed");
  std::cerr<<"Checkpoint restored: anchor="<<checkpointInput->selection.anchorNode
           <<" zone="<<initialCollisionZone<<" position="<<pose.v[12]<<','<<pose.v[13]<<','<<pose.v[14]
           <<" damage="<<s.fields.damage7D4<<'/'<<s.fields.damage7D0<<'\n';
 }
 const auto validNavigationScene=[&](SceneNodeIndex49P node){
  const auto index=static_cast<std::int32_t>(node);
  return index>=0&&index<static_cast<std::int32_t>(s.sceneNodeCount);
 };
 require50DU(validNavigationScene(x.bindings.animation.bodyScene288)&&validNavigationScene(x.bindings.lifecycle.main190)&&
     m.domain.sceneAttached&&!m.domain.sceneHasParent&&!s.fields.vehicle58,
     "source player navigation scene ownership unavailable");
 std::uint64_t navigationUpdates=0,navigationGateSkips=0,navigationCA0Copies=0;
 std::uint64_t navigationAISequential=0,navigationMapSequential=0;
 std::uint64_t navigationAIScan=0,navigationMapScan=0;
 auto& mainMembership=trackStorage.mainMembership.state;
 auto& secondaryMembership=trackStorage.secondaryMembership.state;
 auto& terrainMembership=trackStorage.terrainMembership.state;
 auto& membershipWork=trackStorage.membershipWork;
 const auto mainMembershipStorage=trackStorage.mainMembership.storage();
 const auto secondaryMembershipStorage=trackStorage.secondaryMembership.storage();
 const auto terrainMembershipStorage=trackStorage.terrainMembership.storage();
 live_terrain_probe50fr::Owner terrainLiveOwner{};
 require50DU(live_terrain_probe50fr::initialize(terrainLiveOwner,terrainOwner.initial.segment),"initialize actual single-pair terrain owner");
 auto prepareQueryCache=[&](Type2StaticQuery49X& source,const BodyMatrix48G& scene,
                            LiveZoneBounds50EN& bounds){
  if(!source.resource||!source.resource->nodeCount||(source.resource->root&0x8000u)||
     source.resource->root>=source.resource->nodeCount)return false;
  const auto& root=source.resource->nodes[source.resource->root];
  QuerySpherePrepareInput48O input{};input.preparedScene=scene;input.shapeType=2;input.hasPairTree=true;
  for(unsigned i=0;i<3;++i){input.treeCenter.v[i]=root.center[i];input.treeHalf.v[i]=root.half[i];}
  QuerySpherePrepareOutput48O prepared{};
  if(prepareQuerySphere48O(input,prepared)!=QuerySpherePrepareStatus48O::Ready)return false;
  source.worldMatrix=prepared.world;
  for(unsigned i=0;i<3;++i)source.sphere.center.v[i]=prepared.center.v[i];
  source.sphere.radius=prepared.radius;
  bounds={};bounds.bounds.shapeTag=2;bounds.bounds.shapeWorld=prepared.world;
  bounds.bounds.cachedCenter=prepared.center;bounds.bounds.cachedRadius=prepared.radius;
  bounds.type2Resource=source.resource;return true;
 };
 auto reportMembership=[&](const LiveZoneResult50EN& result,const char* operation){
  if(result.status==LiveZoneStatus50EN::Complete)return true;
  std::cerr<<operation<<" membership status "<<static_cast<unsigned>(result.status)
           <<" bounds "<<static_cast<unsigned>(result.boundsStatus)
           <<" geometry "<<static_cast<unsigned>(result.geometryStatus)
           <<" registry "<<static_cast<unsigned>(result.registryStatus);
  if(result.status==LiveZoneStatus50EN::UnsupportedLinkRemoval)
   std::cerr<<" UnsupportedLinkRemoval";
  std::cerr<<'\n';return false;
 };
 if(gcAnimatedWorld)for(std::size_t i=0;i<gcAnimatedMembership.size();++i){
  auto& membership=*gcAnimatedMembership[i];const auto bounds=gc_animated_candidates50IG::bounds(*gcAnimatedWorld,i);
  const auto ref=static_cast<CollectionRef50BX>(animatedCandidateFirst+i+1);
  //2D93F0 ->120070(method2,radius0), then2D94B0 ->120230 after cold pose.
  require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      membership.storage(),ref,gcAnimatedWorld->source(i).startingZone,ZoneMethod50CL::Second,0.f,
      bounds,membershipWork),"GC original animation initial attachment"),"GC original animation zone attachment failed");
  require50DU(reportMembership(updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,membership.state,
      bounds,membershipWork),"GC original animation cold update"),"GC original animation cold membership failed");
 }
 auto refreshAnimatedCandidates=[&](){
  if(!gcAnimatedWorld)return true;
  gc_animated_candidates50IG::sync(*gcAnimatedWorld,candidates.data(),candidates.size(),animatedCandidateFirst);
  for(std::size_t i=0;i<gcAnimatedMembership.size();++i){
   const auto bounds=gc_animated_candidates50IG::bounds(*gcAnimatedWorld,i);
   const auto result=updateLiveZoneMembership50EN(zoneLifecycle,liveWorld,gcAnimatedMembership[i]->state,bounds,membershipWork);
   if(!reportMembership(result,"GC original animation live update"))return false;
   const auto index=animatedCandidateFirst+i;auto& mirror=movementCandidates[index].collision;
   mirror.geometry.worldMatrix=candidates[index].candidate.collision.geometry.worldMatrix;
   mirror.sphere=candidates[index].candidate.collision.sphere;
  }
  return true;
 };
 LiveZoneBounds50EN mainInitialBounds{},secondaryInitialBounds{};
 require50DU(prepareQueryCache(query,s.nodes[static_cast<int>(x.bindings.lifecycle.main190)].world100,mainInitialBounds)&&
     prepareQueryCache(secondaryQuery,s.nodes[static_cast<int>(x.bindings.lifecycle.physical19C)].world100,secondaryInitialBounds),
     "source initial spring matrix and sphere preparation");
 // Source grid-start1AE578..1AE594 uses collision4C4[0], the first actual zone.
 // Collision registration120070 passes overlap radius0, independently of cache.
 require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,mainMembership,
     mainMembershipStorage,mainQueryRef,initialCollisionZone,ZoneMethod50CL::Second,0.0f,mainInitialBounds,membershipWork),
     "main initial attachment"),"attach main live spring membership");
 require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,secondaryMembership,
     secondaryMembershipStorage,secondaryQueryRef,initialCollisionZone,ZoneMethod50CL::Second,0.0f,secondaryInitialBounds,membershipWork),
     "secondary initial attachment"),"attach secondary live spring membership");

 TerrainPrepareInput48M coldTerrainInput{};
 coldTerrainInput.state=terrainOwner.initial;
 coldTerrainInput.scene=s.nodes[static_cast<int>(x.bindings.lifecycle.main190)].world100;
 coldTerrainInput.lowerEndpoint140=terrainOwner.lowerEndpoint140;
 TerrainPrepareState48M coldTerrainPrepared{};
 require50DU(prepareTerrainSinglePair48M(coldTerrainInput,coldTerrainPrepared)==TerrainPrepareStatus48M::QueryReady,
     "prepare initial authored terrain query bounds");
 const auto terrainInitialBounds=live_terrain_probe50fr::bounds(terrainLiveOwner,coldTerrainPrepared);
 require50DU(reportMembership(attachLiveZoneMembership50EN(zoneLifecycle,liveWorld,terrainMembership,
     terrainMembershipStorage,terrainQueryRef,initialCollisionZone,ZoneMethod50CL::Second,0.f,
     terrainInitialBounds,membershipWork),"terrain initial attachment"),"attach terrain probe membership");
 std::vector<std::uint32_t> terrainStack(stackCount);
 live_terrain_probe50fr::Context terrainContext{zoneLifecycle,liveWorld,terrainMembership,membershipWork,
     terrainQueryRef,trackStorage.terrainTargets.data(),terrainOwnerCount,
     trackStorage.terrainSelected.data(),trackCounts.selected,
     {trackStorage.terrainScratchEffects.data(),trackCounts.selected,terrainStack.data(),terrainStack.size()},
     trackStorage.terrainEffects.data(),trackCounts.selected};
 std::uint64_t terrainQueries=0,terrainHitFrames=0,terrainSupportedFrames=0;
 const VehicleLiveQueryContext50EQ mainLiveQuery{zoneLifecycle,liveWorld,mainMembership,membershipWork,mainQueryRef};
 const VehicleLiveQueryContext50EQ secondaryLiveQuery{zoneLifecycle,liveWorld,secondaryMembership,membershipWork,secondaryQueryRef};
 std::uint64_t mainCrossedPortals=0,secondaryCrossedPortals=0;
 auto acceptLiveQuery=[&](const VehicleLiveQueryResult50EQ& attempt,
                         Type2StaticQuery49X& source,const LiveZoneMembership50EN& membership,
                         std::uint64_t& crossedPortals,const char* operation){
  // Prefix writes precede narrowphase in the source. Keep accepted cache/E0
  // changes even if the later indexed query rejects and the client stops.
  if(attempt.prefixCommitted){
   addCounter50DU(crossedPortals,static_cast<std::uint64_t>(attempt.prefix.crossedPortals));
   source.worldMatrix=membership.lastShapeWorld;
   for(unsigned i=0;i<3;++i)source.sphere.center.v[i]=membership.bounds.cachedCenter.v[i];
   source.sphere.radius=membership.bounds.cachedRadius;
  }
  if(attempt.status==VehicleLiveQueryStatus50EQ::Complete)return true;
  std::cerr<<operation<<" live-query status "<<static_cast<unsigned>(attempt.status)
           <<" query "<<static_cast<unsigned>(attempt.queryStatus)
           <<" prefix-committed "<<attempt.prefixCommitted
           <<" revision "<<attempt.membershipRevision<<'\n';
  if(attempt.status==VehicleLiveQueryStatus50EQ::PrefixRejected)
   reportMembership(attempt.prefix,operation);
  return false;
 };

 auto* mergedIds=trackStorage.mergedIds.data();auto* secondIds=trackStorage.secondIds.data();
 auto* resolveScratch=trackStorage.resolveScratch.data();auto* ownerHandles=trackStorage.handles.data();
 auto& collectionInput=trackStorage.collectionInput;
 collectionInput.zoneLinks=mainMembership.storage.links;collectionInput.zoneLinkCount=mainMembership.storage.capacity;collectionInput.zoneHead=1;collectionInput.query=mainQueryRef;
 collectionInput.bounds.shapeTag=2;collectionInput.type2Resource=query.resource;collectionInput.directTypeTag=query.collisionTypeTag;collectionInput.groupTypeTag=query.groupTypeTag;
 auto collectionInitial=trackStorage.initialCollection();
 auto collectionWork=trackStorage.collectionWorkspace();
 std::uint64_t collectedCalls=0;std::size_t peakCollected=0;
 auto collectRegistered=[&](){
  // Each HOST racer bank restores its own collection cache. Before it queries,
  // publish the one shared world's latest accepted compound pose/membership.
  if(!refreshAnimatedCandidates())return false;
  const auto applied=drainZoneRegistrations50CQ(zoneQueue,zoneLifecycle,zoneScratch,trackCounts.events);
  if(applied.status!=ZoneQueueStatus50CQ::Complete||applied.remaining)return false;
  if(prepareSearchView50BT(registry,registrationBlocks,2,registrationView)!=SearchStorageStatus50BS::Complete)return false;
  //50CL keeps method descriptors current after each explicit registry change.
  for(std::size_t i=0;i<terrainOwnerCount;++i){const auto&owner=candidates[i].candidate.collision;auto projected=projectCollectionObject50CF(owner.filter,owner.sphere,owner.typeTag,candidates[i].shapeType,0);if(!projected.valid)return false;collectionObjects[i]=projected.object;collectionSource[i]=projected.state;}
  if(!mainMembership.attached||!secondaryMembership.attached||
     mainMembership.object!=mainQueryRef||secondaryMembership.object!=secondaryQueryRef||
     mainMembership.lastShapeTag!=2||secondaryMembership.lastShapeTag!=2||
     mainMembership.lastType2Resource!=query.resource||
     secondaryMembership.lastType2Resource!=secondaryQuery.resource)return false;
  // Outer219AF0/219B14 consumes the prior accepted matrix/sphere and E0.
  //1223E4 calls1202E0 directly; fresh11FE50/11F680/120230 belongs only to
  // the later spring prefix. Secondary will reuse this exact collected list.
  LiveZoneBounds50EN collectionBounds{};collectionBounds.bounds.shapeTag=2;
  collectionBounds.bounds.shapeWorld=mainMembership.lastShapeWorld;
  collectionBounds.bounds.cachedCenter=mainMembership.bounds.cachedCenter;
  collectionBounds.bounds.cachedRadius=mainMembership.bounds.cachedRadius;
  collectionBounds.type2Resource=query.resource;
  CollisionSphere48J mainCollectionSphere{},secondaryCollectionSphere{};
  for(unsigned i=0;i<3;++i){
   mainCollectionSphere.center.v[i]=mainMembership.bounds.cachedCenter.v[i];
   secondaryCollectionSphere.center.v[i]=secondaryMembership.bounds.cachedCenter.v[i];
  }
  mainCollectionSphere.radius=mainMembership.bounds.cachedRadius;
  secondaryCollectionSphere.radius=secondaryMembership.bounds.cachedRadius;
  const auto mainProjection=projectCollectionObject50CF(qfilter,mainCollectionSphere,query.collisionTypeTag,2,
      std::bit_cast<std::uint32_t>(collectionBounds.bounds.cachedCenter.v[3]));
  const auto secondaryProjection=projectCollectionObject50CF(secondaryFilter,secondaryCollectionSphere,
      secondaryQuery.collisionTypeTag,2,std::bit_cast<std::uint32_t>(secondaryMembership.bounds.cachedCenter.v[3]));
  CollisionSphere48J terrainCollectionSphere{};
  for(unsigned i=0;i<3;++i)terrainCollectionSphere.center.v[i]=terrainMembership.bounds.cachedCenter.v[i];
  terrainCollectionSphere.radius=terrainMembership.bounds.cachedRadius;
  const auto terrainProjection=projectCollectionObject50CF(terrainOwner.metadata.filter,
      terrainCollectionSphere,terrainOwner.metadata.collisionTypeTag,2,
      std::bit_cast<std::uint32_t>(terrainMembership.bounds.cachedCenter.v[3]));
  if(!mainProjection.valid||!secondaryProjection.valid||!terrainProjection.valid)return false;
  collectionObjects[terrainQueryRef-1]=terrainProjection.object;
  collectionSource[terrainQueryRef-1]=terrainProjection.state;
  collectionObjects[mainQueryRef-1]=mainProjection.object;collectionSource[mainQueryRef-1]=mainProjection.state;
  collectionObjects[secondaryQueryRef-1]=secondaryProjection.object;collectionSource[secondaryQueryRef-1]=secondaryProjection.state;
  collectionInput.zoneLinks=mainMembership.storage.links;collectionInput.zoneLinkCount=mainMembership.storage.capacity;
  collectionInput.zoneHead=1;collectionInput.bounds=collectionBounds.bounds;
  collectionInput.initialCurrentIdentity=query.initialQueryCurrentIdentity;
  const auto result=collectVehicleCandidates50CA(collectionInput,collectionInitial,collectionWork);
  if(result.phase!=VehicleCandidateResult50CA::Phase::Complete){std::cerr<<"collection phase "<<int(result.phase)<<" status "<<int(result.collectionStatus)<<'\n';return false;}
  if(resolveCandidateHandles50CB(mergedIds,result.count,ownerHandles,terrainOwnerCount,resolveScratch,track_runtime_storage50IG::sourceSelectionCapacity,registeredIndices,trackCounts.selected)!=CandidateHandleStatus50CB::Complete)return false;
  if(resolveCandidateHandles50CB(secondIds,result.second.count,ownerHandles,terrainOwnerCount,resolveScratch,track_runtime_storage50IG::sourceSelectionCapacity,probeIndices,trackCounts.selected)!=CandidateHandleStatus50CB::Complete)return false;
  probeCount=static_cast<std::size_t>(result.second.count);
  terrainOwner.metadata.filter.flagsF8=collectionSecond[terrainQueryRef-1].flagsF8;
  registeredCount=static_cast<std::size_t>(result.count);peakCollected=std::max(peakCollected,registeredCount);addCounter50DU(collectedCalls);
  for(std::size_t i=0;i<terrainOwnerCount;++i)candidates[i].candidate.collision.filter.flagsF8=collectionSecond[i].flagsF8;
  qfilter.flagsF8=collectionSecond[mainQueryRef-1].flagsF8;
  secondaryFilter.flagsF8=collectionSecond[secondaryQueryRef-1].flagsF8;query.initialQueryCurrentIdentity=result.second.currentIdentity;
  std::memcpy(treeSource,treeSecond,trackCounts.methods*sizeof(*treeSource));return true;
 };
 auto commitCandidateEffects=[&](const std::vector<TerrainCandidateEffects48J>&changed){
  if(!registeredCount)return true;
  if(changed.size()<registeredCount)return false;
  for(std::size_t i=0;i<registeredCount;++i)if(changed[i].identity!=candidates[registeredIndices[i]].candidate.collision.filter.identity)return false;
  for(std::size_t i=0;i<registeredCount;++i){auto&primary=candidates[registeredIndices[i]].candidate.collision;auto&movementView=movementCandidates[registeredIndices[i]].collision;primary.filter.flagsF8=changed[i].flagsF8;primary.copied40=changed[i].copied40;primary.copied50=changed[i].copied50;movementView.filter.flagsF8=changed[i].flagsF8;movementView.copied40=changed[i].copied40;movementView.copied50=changed[i].copied50;}
  return true;
 };
 rr::assets::PodFlameOwners50W coldFlame{};
 {using namespace rr::assets;std::ifstream input("work/native/project/disc/PODS/POD01/POD01.RES",std::ios::binary);std::vector<std::uint8_t> raw((std::istreambuf_iterator<char>(input)),{}),member;ResArchive48C res;DecodeError48C error;Scene48D left,right;Material48E shared,fl,fr;
 require50DU(decodeResource48C(raw,res,error),"decode actual Pod01");
 auto scene=[&](const char*name,Scene48D&out){return extractResMember48C(res,name,member,error)&&decodeScene48D(member,out,error);};
 auto material=[&](const char*name,Material48E&out){return extractResMember48C(res,name,member,error)&&decodeMaterial48E(member,out,error);};
 require50DU(scene("pods\\pod01\\englt1.psg",left)&&scene("pods\\pod01\\engrt1.psg",right),"actual engine scenes");
 require50DU(material("textures\\01_1.psm",shared)&&material("textures\\flame01l.psm",fl)&&material("textures\\flame01r.psm",fr),"actual flame materials");PodFlameError50W diag{};
 require50DU(buildPodFlameOwners50W(left,right,shared,fl,fr,static_cast<unsigned>(x.bindings.visual.left1BC)+1,static_cast<unsigned>(x.bindings.visual.right1B8)+1,coldFlame,diag),"cold flame owners");
 }


 BaseTailState50X baseTail=coldBaseTailState50X(0x3f800000u,0x3f800000u);BaseTailGlobals50X baseGlobals{};BaseTailWorkspace50X baseWorkspace{};BasePoseHistoryState50Y poseHistory{};poseHistory.nextFD4=19;
 // Explicit diagnostic cold history: all positions start at supplied pod pose.
 for(auto&slot:poseHistory.historyD50){std::memcpy(slot,pose.v+12,12);slot[3]=0x3f800000u;}
 s.impactSupplement.partnerId=0xffffffffu;FrameTimingState49I clock{};clock.lastDispatched23C=.016f;

 // Fresh unrenamed native model roots; later renames/clones are not admitted.
 rr::assets::ModelRootName50BL rootNames[2]{};SceneNodeIndex49P namedIndices[2][14];rr::assets::EffectNameModel50BL namedModels[2]{};rr::assets::EffectNames50BL namesForEffects{};
 for(unsigned side=0;side<2;++side){rr::assets::initializeFreshModelRootName50BL(rootNames[side]);const auto root=side?x.bindings.visual.right1B8:x.bindings.visual.left1BC;require50DU(x.assets.hierarchy[side].nodeCount==14,"actual engine node count");for(unsigned i=0;i<14;++i)namedIndices[side][i]=static_cast<SceneNodeIndex49P>(static_cast<int>(root)+1+i);namedModels[side]={root,&rootNames[side],x.assets.hierarchy[side],namedIndices[side],14};}
 require50DU(rr::assets::buildEffectNames50BL({s.nodes,s.sceneNodeCount},namedModels,2,namesForEffects),"actual effect attachment names");
 EffectOwner50BE nativeEffects[8]{};const char*effectFiles[]={"podLeftEngineDust.emt","podRightEngineDust.emt","podLeftEngineSmoke.emt","podRightEngineSmoke.emt","podSparks.emt","podSparks.emt","podFire.emt","podFire.emt"};
 for(unsigned i=0;i<8;++i){auto raw=runtime_startup50fl::readBytes(std::string("evidence/effectAuthoring50BF/members/")+effectFiles[i]);const auto profile=i<4?EffectProfile50BE::Particle19A5E0:i<6?EffectProfile50BE::Ordinary15F100:EffectProfile50BE::Fire15DE00;require50DU(rr::assets::buildEffectControlOwner50BJ(i+1,profile,reinterpret_cast<const char*>(raw.data()),raw.size(),{0,0},nativeEffects[i]),"actual effect control construction");}
 ResetEffectBindings50BK resetEffectBindings{{1,2},{0,0},{5,6,0,0},false};ResetEffectState50BK resetEffectState{};std::memcpy(resetEffectState.timers820,s.impactSupplement.timers820,16);ResetEffectWorkspace50BK resetEffectWorkspace{};
 const auto resetAllocationCount=::allocations;require50DU(resetEffectStops50BK(resetEffectBindings,nativeEffects,8,resetEffectState,resetEffectWorkspace).status==EffectDispatchStatus50BE::Complete&&::allocations==resetAllocationCount,"native reset effect stop sequence");std::memcpy(s.impactSupplement.timers820,resetEffectState.timers820,16);
 ImpactEffectsWorkspace50B effectsWorkspace{};ImpactEffectEvent50B generatedEffects[maxImpactEffectEvents50B]{};EffectDispatchWorkspace50BE effectDispatch{};
 std::uint64_t effectCommands=0;
 auto consumeEffects=[&](const ImpactEffectEvent50B*events,std::size_t count){
  const auto result=consumeEffectControls50BE(events,count,nativeEffects,8,effectDispatch);
  if(result.status!=EffectDispatchStatus50BE::Complete)return false;
  addCounter50DU(effectCommands,count);return true;
 };
 std::uint64_t completedFrames=0,mainContactFrames=0,secondaryContactFrames=0,repairActiveTicks=0;
 std::uint64_t submittedDrawFrames=0,windowSwaps=0,nativeCalls=0,discardedHostNs=0;
 auto runNative=[&](auto&&fn,const char*label){
  const bool ok=fn();addCounter50DU(nativeCalls);
  if(!ok){std::ostringstream message;message<<"native frame "<<completedFrames
   <<" "<<label<<" error="<<static_cast<unsigned>(x.diagnostic.error)
   <<" source-stage="<<std::hex<<x.diagnostic.sourceStage
   <<" body="<<static_cast<unsigned>(x.diagnostic.body);
   throw std::runtime_error(message.str());}
 };

 // Actual retained chariot and engine meshes, loaded once before creating GL.
 rr::assets::VehicleDrawAssets50CR drawAssets;rr::assets::VehicleDrawAssetsInput50CR drawInput{};
 rr::assets::VehicleDrawAssetDiagnostic50CR drawLoadDiagnostic{};
 std::vector<std::uint8_t> drawBytes[3];SceneNodeIndex49P drawNodes[3][18]{};std::uint8_t drawPalette[3][18]{};
 const char*drawKeys[]={"cb6d91807cca98eecb547dfd4b030345e8e88e6ab9363fc436012732487863b5","210c524d329161a97e79baecded9c34bb6949b6910dea148ce2bf5cb0ddb2fc3","f073a4149ace5c36bf51d3390c867cddabedca939e1a876d0efef8cf8d438d60"};
 const unsigned drawCounts[]={18,14,14},drawBases[]={31,1,16},drawMapped[]={2,0,1};
 for(unsigned component=0;component<3;++component){
  drawBytes[component]=runtime_startup50fl::readBytes(std::string("evidence/visual49G/cacheobjects/")+drawKeys[component]+".rrscene");
  const auto&psg=x.assets.psg[drawMapped[component]];const auto padded=(drawCounts[component]+3)&~3u;
  require50DU(psg.size()>=12+padded,"actual palette suffix bounds");const auto suffix=psg.size()-(12+padded);
  auto word=[&](std::size_t at){return std::uint32_t(psg[at])|(std::uint32_t(psg[at+1])<<8)|(std::uint32_t(psg[at+2])<<16)|(std::uint32_t(psg[at+3])<<24);};
  require50DU(word(suffix)==1&&word(suffix+4)==padded&&word(suffix+8+padded)==drawCounts[component],"actual single palette record");
  for(unsigned n=0;n<drawCounts[component];++n){drawPalette[component][n]=psg[suffix+8+n];drawNodes[component][n]=static_cast<SceneNodeIndex49P>(drawBases[component]+n);}
  drawInput.component[component]={&drawBytes[component],drawNodes[component],drawCounts[component],drawPalette[component],drawCounts[component]};
 }
 require50DU(drawAssets.load(drawInput,drawLoadDiagnostic),"cold native draw owners");
 namespace podTextures=pod_materials50ed_fixture;
 auto materialOwner=std::make_unique<podTextures::Owner>();
 podTextures::Request materialRequest{};materialRequest.root="evidence/visual49G";
 materialRequest.coldSourceByteCapacity=8u*1024u*1024u;
 materialRequest.textureBudgets={5,26,1024u*1024u,1024u*1024u,256};
 for(unsigned component=0;component<3;++component){
  const auto* scene=drawAssets.scene(static_cast<VehicleDrawComponent50CM>(component));
  require50DU(scene,"material scene missing");
  materialRequest.choices[component].assign(scene->materials.size(),
      podTextures::Choice{1,1,rr::assets::MaterialTextureRequirement50EA::required,
                         podTextures::MipMode::largestRetained,0});
 }
 podTextures::Diagnostic materialDiagnostic{};
 const bool materialsLoaded=materialOwner->load(materialRequest,drawAssets,materialDiagnostic);
 require50DU(materialsLoaded,materialDiagnostic.message.empty()?"Pod material load failed":materialDiagnostic.message.c_str());
 auto cableOwner=std::make_unique<pod_cables50fd_fixture::Owner>();
 pod_cables50fd_fixture::Diagnostic cableAssetDiagnostic{};
 require50DU(cableOwner->load(drawAssets,x.bindings,s,cableAssetDiagnostic),
     cableAssetDiagnostic.message.empty()?"actual cable asset load failed":cableAssetDiagnostic.message.c_str());
 const PodCableBindings50FD cableBindings=cableOwner->bindings();
 PodCableMesh50FD cableMesh{};PodCableWorkspace50FD cableWorkspace{};
 PodCableDiagnostic50FD cableDiagnostic{};
 VehicleDrawWorkspace50CM drawWorkspace{};
 // Keep native held queries on the explicit advanced preset4, not on the
 // old fixture's unsupported negative flag-button codes.
 constexpr std::uint32_t deviceIndex=0,playerSource5C=0,playerActionIndexFFC=0;
 ControlPresets49D controlPresets{};initializeControlPresets49D(controlPresets);
 require50DU(selectControlPreset49D(controlPresets,playerActionIndexFFC,4),"explicit advanced control preset");SelectedControlPreset49D selectedControls{};
 require50DU(resolveControlPreset49D(controlPresets,playerActionIndexFFC,playerSource5C,selectedControls),"separate explicit action/configuration slots");
 require50DU(selectedControls.actions.flag69C==4&&selectedControls.eventAction.action6AC==8,
              "retained preset repair/action identities changed");
 x.actionCodes=selectedControls.actions;x.configurationEntryNonzero=selectedControls.configurationEntryNonzero;
 DeviceButtonBank48Y initialControllerButtons{};ControllerIngress50CX controller(initialControllerButtons);
 const auto deviceDescriptors=initialDualShockDescriptors48Y();
 const auto repairButtonCode=static_cast<std::uint32_t>(selectedControls.actions.flag69C);
 const auto boostButtonCode=static_cast<std::uint32_t>(selectedControls.actions.flag698);
 const auto powerslideButtonCode=static_cast<std::uint32_t>(selectedControls.actions.flag6B4);
 require50DU(powerslideButtonCode==7,"retained powerslide button identity changed");
 require50DU(boostButtonCode==5,"retained boost button identity changed");
 // Explicit development-host topology: one enabled player under the real48Z
 // application router, with no event-handling descendants under that player.
 // The supplemental49A fields are persistent cold storage, not another frame
 // or a claim to reproduce the original player/application constructors.
 PlayerEventState49A playerEvents{};
 playerEvents.source5C=playerSource5C;playerEvents.indexFFC=playerActionIndexFFC;
 const auto playerActionRows=playerEventPresetRows49D(controlPresets);
 PlayerEventEnvironment49A playerEnvironment{};
 playerEnvironment.actionRows=playerActionRows.rows;
 playerEnvironment.actionRowCount=controlPresetSlots49D;
 playerEnvironment.flags29=1; // Supplied enabled application->28C game object.
 // Category0 context/debug and category1 auxiliary sources do not exist in
 // this host. Leave them absent, without fabricated successful-miss callbacks.
 EventContext48Z eventContext{false,0,{nullptr,0}};
 EventRecordSources48Z eventRecords{};
 PlayerEffectCall49A unavailableEffect{};
 PlayerEventBindings49A playerEventBindings{};
 playerEventBindings.effects={&unavailableEffect,
     [](void* opaque,const PlayerEffectCall49A& call) noexcept {
         *static_cast<PlayerEffectCall49A*>(opaque)=call;
         return false; // Reached unsupported effects must stop native dispatch.
     }};
 playerEventBindings.children={0x600u,noEventChild48Z,nullptr,0};
 playerEventBindings.childVisitBudget=0;
 EventChild48Z applicationChildren[1]{{noEventChild48Z,1}};
 JoystickApplicationBridge48Z applicationEvents{};
 applicationEvents.rootFlags228=0;
 applicationEvents.root={0x600u,0,applicationChildren,1};
 applicationEvents.visitBudget=1;
 PlayerEventChildBinding49A playerEventChild{};
 playerEventChild.childIndex=0;playerEventChild.player=&playerEvents;
 playerEventChild.environment=&playerEnvironment;playerEventChild.bindings=&playerEventBindings;
 ManagerEventCallbacks48Z eventCallbacks{};
 require50DU(bindControllerPlayerActions50EC(controller,deviceIndex,eventRecords,
     applicationEvents,playerEventChild,eventCallbacks),"live one-player action binding");
 InputEvent48Z controllerEventStorage[32]{};
 InputEventQueue48Z controllerEvents{controllerEventStorage,32,0};
 ControllerActions50EC controllerActions(controller,controllerEvents,deviceIndex);
 std::uint64_t controllerPresses=0,controllerReleases=0;
 bool recoveryPaused=false,checkpointRequested=false;
 namespace platform=rr::platform;
 platform::KeyboardPacketMapping50DR keyMapping{};
 for(auto& axis:keyMapping.axes)axis.source=platform::KeyboardAxisSource50DR::Centered;
 // Declared development controls, not recovered keyboard bindings. packet7 is
 // left engine (W/S), packet5 right engine (Up/Down); opposite keys center.
 keyMapping.axes[3]={platform::KeyboardAxisSource50DR::KeyPair,26,22,
                     platform::KeyboardOpposingKeys50DR::Center};
 keyMapping.axes[1]={platform::KeyboardAxisSource50DR::KeyPair,82,81,
                     platform::KeyboardOpposingKeys50DR::Center};
 for(auto& button:keyMapping.buttons)button.source=platform::KeyboardButtonSource50DR::Released;
 keyMapping.buttons[repairButtonCode]={platform::KeyboardButtonSource50DR::Key,21}; // R
 keyMapping.buttons[boostButtonCode]={platform::KeyboardButtonSource50DR::Key,44}; // Space
 keyMapping.buttons[powerslideButtonCode]={platform::KeyboardButtonSource50DR::Key,225}; // Left Shift
 DevicePacket48Y packetTemplate{};packetTemplate.bytes[2]=packetTemplate.bytes[3]=0xff;
 DevicePacket48Y packet{};platform::KeyboardPacketDiagnostic50DR keyboardDiagnostic{};
 bool packetLive=false;
 auto requireActionStatus=[&](ControllerActionStatus50EC status,const char* operation){
  addCounter50DU(nativeCalls);
  if(status!=ControllerActionStatus50EC::Ok){
   std::cerr<<"controller action "<<operation<<" status "<<static_cast<unsigned>(status)
            <<" phase "<<static_cast<unsigned>(controllerActions.phase())<<'\n';
   throw std::runtime_error("native controller action lease failed");
  }
 };
 // Every accepted sample is appended, genuinely dispatched and acknowledged
 // before another poll can overwrite its transitions, including pause releases.
 auto sampleController=[&](){
  // UI capture/quit/pause paths may reach this call site, but there is no
  // player device owner to sample in the explicitly selected all-AI roster.
  if(allAiEnabled)return;
  ++allAiControllerSamples;
  // The one declared source-cut pause only samples release-all packets. Keep
  // its partial native phase intact; input release is not frame continuation.
  require50DU(s.phase==VehicleFramePhase49W::Ready||(recoveryPaused&&!packetLive),
              "controller dispatch requires frame boundary or source-cut release");
  playerEvents.field40=m.vehicleState40;
  playerEnvironment.gate58=s.fields.applicationByte58;
  playerEnvironment.held.rootFlags228=applicationEvents.rootFlags228;
  playerEnvironment.held.context1898=eventContext.mask1898;
  requireActionStatus(controllerActions.samplePacket(true,true,packet,deviceDescriptors,
      dualShockActiveButtons48Y),"sample");
  require50DU(controller.deviceSample().status==DevicePacketStatus48Y::Accepted,
              "controller did not accept the composed keyboard packet");
  requireActionStatus(controllerActions.appendSample(),"append");
  const auto result=controllerActions.dispatch(eventContext,eventRecords,eventCallbacks);
  addCounter50DU(nativeCalls);
  if(result.status!=ControllerActionStatus50EC::Ok||!result.dispatchAttempted||
     result.dispatch.status!=EventDispatchStatus48Z::Complete){
   std::cerr<<"controller dispatch unavailable: status "<<static_cast<unsigned>(result.status)
            <<" manager "<<static_cast<unsigned>(result.dispatch.status)
            <<" completed "<<result.dispatch.eventsCompleted
            <<" callbacks "<<result.dispatch.callbacksInvoked;
   if(result.dispatchAttempted&&result.dispatch.callbacksInvoked){
    std::cerr<<" application route "<<static_cast<unsigned>(applicationEvents.lastRoute.status);
    if(applicationEvents.lastRoute.callbacksInvoked){
     const auto& playerResult=playerEventChild.lastResult;
     std::cerr<<" player "<<static_cast<unsigned>(playerResult.status)
              <<" held query "<<static_cast<unsigned>(playerResult.lastQuery);
     if(playerResult.effectsInvoked)
      std::cerr<<" unavailable effect "<<static_cast<unsigned>(unavailableEffect.kind);
    }
   }
   std::cerr<<'\n';
   throw std::runtime_error("native action unavailable; pending batch retained without replay");
  }
  const auto& sample=controller.deviceSample();
  require50DU(result.dispatch.eventsCompleted==sample.transitionCount&&
      result.dispatch.callbacksInvoked==sample.transitionCount,
      "controller transition batch was not fully dispatched");
  for(std::uint32_t i=0;i<sample.transitionCount;++i)
   addCounter50DU(sample.transitions[i].held?controllerPresses:controllerReleases);
  requireActionStatus(controllerActions.acknowledgeComplete(),"acknowledge");
 };
 auto composePacket=[&](bool live,const platform::WindowInput50DQ& input){
  if(allAiEnabled)return;
  ++allAiKeyboardPackets;
  if(live&&autoDriveEnabled&&(input.down[26]||input.down[22]||input.down[82]||input.down[81])){
   autoDriveEnabled=false;courseState={};courseOutput={};
   std::cerr<<"Host course driver disabled by manual throttle/brake input.\n";
  }
  require50DU(platform::composeKeyboardPacket50DR({true,live},
      {input.down.data(),input.down.size()},keyMapping,
      platform::KeyboardUnavailablePolicy50DR::ReleaseAll,packetTemplate,packet,
      keyboardDiagnostic),"keyboard packet composition failed");
  packetLive=live;
 };

 auto applyCourseInput=[&](std::size_t racerSlot=0){
  require50DU(!allAiEnabled,"all-original-AI path reached forbidden HOST course driver");
  ++allAiHostDriverCalls;
  rr::host::CourseDriverInput50GK command{};
  command.enabled=autoDriveEnabled;command.elapsedSeconds=.016f;
  command.pose=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].world100;
  command.worldVelocity=m.components.root.body.velocity;
  command.yawRate=m.components.root.worldAngularVelocityA0.v[1];
  command.previousNode=navigationProgress.vehicleC84;
  command.currentNode=navigationProgress.reset.vehicleC80;
  const bool newNode=!courseState.initialized||courseState.observedNode!=command.currentNode;
  const bool driving=rr::host::driveCourse50GK(*navigationAI,courseLimits,courseTuning,
      command,courseState,courseOutput,courseDiagnostic);
  require50DU(rr::host::applyCourseThrottlePacket50GK(courseOutput,packet),"host course packet rejected");
  if(driving&&newNode&&courseTuning.authoredSteeringLookaheadSeconds>0.f)
   std::cerr<<"Host course node: slot="<<racerSlot<<" tick="<<completedFrames
       <<" previous="<<command.previousNode<<" node="<<command.currentNode
       <<" position="<<command.pose.v[12]<<','<<command.pose.v[13]<<','<<command.pose.v[14]
       <<" speed="<<courseDiagnostic.bodySpeed<<" target="<<courseDiagnostic.targetSpeed
       <<" cross_track="<<courseDiagnostic.crossTrack<<" half_width="<<courseDiagnostic.halfWidth
       <<" speed_preview="<<courseDiagnostic.speedLookahead
       <<" steering_preview="<<courseDiagnostic.steeringLookahead
       <<" steering_segments="<<courseDiagnostic.steeringSegments
       <<" aim="<<courseDiagnostic.aim[0]<<','<<courseDiagnostic.aim[1]<<','<<courseDiagnostic.aim[2]
       <<" error="<<courseDiagnostic.headingError<<" turn="<<courseDiagnostic.turn
       <<" demand="<<courseDiagnostic.speedDemand<<" mixed_demand="<<courseDiagnostic.mixedDemand
       <<" left/right="<<courseOutput.left<<'/'<<courseOutput.right
       <<" packet7/5="<<unsigned(packet.bytes[7])<<'/'<<unsigned(packet.bytes[5])<<'\n';
  if(!driving&&!autoStopReported){
   autoStopReported=true;
   std::cerr<<"Host course driver stopped: status="<<unsigned(courseDiagnostic.status)
       <<" node="<<courseDiagnostic.node<<" cross_track="<<courseDiagnostic.crossTrack
       <<" stalled_s="<<courseDiagnostic.stalledSeconds
       <<" slot="<<racerSlot<<" tick="<<completedFrames<<" previous="<<command.previousNode
       <<" position="<<command.pose.v[12]<<','<<command.pose.v[13]<<','<<command.pose.v[14]
       <<" speed="<<courseDiagnostic.bodySpeed<<" half_width="<<courseDiagnostic.halfWidth
       <<". Neutral input; manual takeover remains available.\n";
  }
 };

 // The original one-human manager starts results when that human finishes,
 // independently of unfinished AI. This explicit host clock remains separate
 // from the sealed race timer and never drives a vehicle pose.
 bool humanResultsActive=false,humanResultsReady=false;
 float humanResultsCountdown=0.f;
 std::uint64_t finishDriverTicks=0,finishManualInputTicks=0;
 bool finishDriverAppliedThisFrame=false;
 bool opponentFrame=false,raceSetupFrame=false;
 // This host fallback abandons the failed frame; it never supplies a miss
 // or repairs source list topology. Only the exact retained boundary qualifies.
 auto recoverPortalBoundary=[&](const LiveZoneResult50EN& failure,const char* owner){
  if(failure.status!=LiveZoneStatus50EN::UnsupportedLinkRemoval)return false;
  recoveryPaused=true;
  if(originalAiEnabled){
   std::cerr<<"ORIGINAL AI source-recovery boundary: kind=portal owner="<<owner
       <<" phase="<<static_cast<unsigned>(s.phase)
       <<" status="<<static_cast<unsigned>(failure.status)<<"; no checkpoint prepared or session rebuilt.\n";
   require50DU(false,"original AI single session reached native portal boundary");
  }
  if(opponentFrame)return true; // Keep the unsupported prefix in this AI slot only.
  recovery50FI::Plan nextCheckpoint{};recovery50FI::Diagnostic recoveryDiagnostic{};
  BodyMatrix48G groundedCheckpoint{};
  const bool prepared=recovery50FI::preparePortalRecovery(*navigationMap,navigationProgress,s,
      x.bindings,failure,x.parameters.visual.verticalLimit7EC,liveWorld,nextCheckpoint,recoveryDiagnostic);
  const bool repeated=prepared&&checkpointInput&&
      checkpointInput->policy==recovery50FI::Policy::PortalLinkRemoval&&
      checkpointInput->primaryZone==nextCheckpoint.primaryZone&&
      checkpointInput->selection.anchorNode==nextCheckpoint.selection.anchorNode&&
      checkpointInput->progress.reset.vehicleC80==nextCheckpoint.progress.reset.vehicleC80&&
      checkpointInput->progress.vehicleC84==nextCheckpoint.progress.vehicleC84&&
      checkpointInput->progress.reset.vehicleC88==nextCheckpoint.progress.reset.vehicleC88&&
      checkpointInput->progress.reset.vehicleC8C==nextCheckpoint.progress.reset.vehicleC8C;
  const bool grounded=prepared&&!repeated&&recovery50FI::resolveGround(ground,nextCheckpoint,16,0,
      groundStack.data(),groundStack.size(),groundedCheckpoint,recoveryDiagnostic);
  if(grounded){
   checkpointOutput=nextCheckpoint;checkpointRequested=true;
   std::cerr<<"Host portal-boundary checkpoint recovery: owner="<<owner
      <<" completed_ticks="<<completedFrames<<" anchor="<<nextCheckpoint.selection.anchorNode
      <<" zone="<<nextCheckpoint.primaryZone<<". Failed frame abandoned; damage/progress retained.\n";
  }else{
   std::cerr<<"Portal boundary paused: owner="<<owner<<" repeated_without_progress="<<repeated
      <<" recovery_error="<<unsigned(recoveryDiagnostic.error)
      <<". Last committed image retained; F5/restart returns to grid.\n";
  }
  return true;
 };
 auto speechInput=[&](){
  DamageSpeechInput50HG in{};
  in.damage7D0=s.fields.damage7D0;in.damage7D4=s.fields.damage7D4;in.damage7DC=s.fields.damage7DC;
  in.vehicle3C=s.fields.vehicle3C;in.vehicle40=m.vehicleState40;in.vehicle58=s.fields.vehicle58;
  in.vehicle60=playerRacer60;
  // Source21081C/210820 authenticates the resource identity, not native bank slot0.
  in.vehicle60Known=true;in.playerSpeechChance138=playerSpeechChance;
  in.config58=s.fields.applicationByte58;
  in.position={m.components.root.body.position.v[0],m.components.root.body.position.v[2]};
  in.listeners={audioListenerReady?&audioListener:nullptr,audioListenerReady?1u:0u};
  return in;
 };
 struct Mode1EffectsContext50ID {
  std::unique_ptr<race_mode1_host50ID::Owner>* mode=nullptr;
  source_announcer50IE::Owner* dax=nullptr;race_music50IE::Owner* music=nullptr;
  live_audio50HF::Owner* audio=nullptr;decltype(speechInput)* input=nullptr;
  std::uint64_t* speechCues=nullptr;
 } mode1Context{&mode1,sourceAnnouncer.get(),raceMusic.get(),liveAudio.get(),&speechInput,&speechCues};
 race_mode1_host50ID::Effects mode1Effects{};mode1Effects.context=&mode1Context;
 mode1Effects.announcer=[](void* opaque,std::uint32_t type,std::uint32_t racer,std::uint32_t variant)noexcept{
  auto& c=*static_cast<Mode1EffectsContext50ID*>(opaque);
  return *c.mode&&c.dax->request(type,racer,variant,
      (*c.mode)->config.mode11258,(*c.mode)->manager.flags58);
 };
 mode1Effects.music=[](void* opaque,std::uint32_t key,std::uint32_t argumentA2)noexcept{
  auto& c=*static_cast<Mode1EffectsContext50ID*>(opaque);
  return *c.mode&&c.audio->outputOpen()&&c.music->request(key,argumentA2,
      (*c.mode)->config.mode11258).completed();
 };
 mode1Effects.audio=[](void* opaque,const AudioCommand50HE& command)noexcept{
  auto& c=*static_cast<Mode1EffectsContext50ID*>(opaque);
  if(!c.audio->outputOpen()||!c.audio->applySourceCommand(command).okay())return false;
  ++*c.speechCues;return true;
 };
 mode1Effects.damageInput=[](void* opaque,DamageSpeechInput50HG& input)noexcept{
  auto& c=*static_cast<Mode1EffectsContext50ID*>(opaque);input=(*c.input)();return true;
 };
 auto refreshDynamicSelfActor=[&](){
  require50DU(dynamicSelfSlot<raceRacerCount,"dynamic actor slot unavailable");
  dynamicActors.values[dynamicSelfSlot]={s.fields.vehicle3C,s.fields.vehicle58,
      dynamicRacer60[dynamicSelfSlot],lapContinuity.state.vehicleCC8,
      static_cast<std::uint32_t>(originalAiSession.rank[dynamicSelfSlot].placeCCC),s.fields.impactFlagsCD8};
  dynamicActors.bound[dynamicSelfSlot]=true;
 };
 auto dynamicBoundary=[&](std::uint32_t pc,const char* owner){
  std::cerr<<"ORIGINAL AI dynamic-contact boundary: slot="<<dynamicSelfSlot
      <<" pc="<<std::hex<<pc<<std::dec<<" owner="<<owner
      <<" phase="<<static_cast<unsigned>(s.phase)<<" contacts20C="<<m.components.body20C
      <<" partner="<<s.impactSupplement.partnerId<<" partner_state="<<s.impactSupplement.partnerState
      <<" partner_amount="<<s.impactSupplement.partnerAmount0C
      <<" CFC="<<s.fields.vehicleCFC<<" D00="<<s.fields.vehicleD00
      <<" event_count="<<dynamicEvents.count150<<"; reached native state retained.\n";
  require50DU(false,owner);
 };
 auto fulfillDynamicMusic=[&](){
  using Status=DynamicContactEffectStatus50IW;
  if(beginDynamicContactMusic50IW(dynamicMusicCursor).status!=Status::Continue)
   dynamicBoundary(0x22a400u,"collision music continuation already pending");
  for(;;){
   const auto step=advanceDynamicContactMusic50IW(dynamicMusicCursor,sessionLazy,dynamicRange);
   if(step.status==Status::Complete)return;
   if(step.status!=Status::NeedHelper)dynamicBoundary(step.sourcePC,"collision music source continuation");
   std::uint32_t actualV0=0;bool returned=false;
   if(step.request.kind==LapEventCall50ID::Music1A46E0){
    returned=dynamicMusicProfile.bound&&liveAudio->outputOpen()&&
        raceMusic->request(step.request.argumentA1,step.request.argumentA2,
            dynamicMusicProfile.mode11258).completed();
   }else returned=applyHostLapClockOrRandom50ID(step.request,applicationClock,
       s.noise.sharedRandomState,actualV0);
   if(!returned)dynamicBoundary(step.sourcePC,"collision music actual helper pending");
   if(!acceptDynamicContactMusicReturn50IW(dynamicMusicCursor,step.request.sequence,
       step.request.kind,actualV0))dynamicBoundary(step.sourcePC,"collision music return mismatch");
  }
 };
 auto advanceFrame=[&](){
 finishDriverAppliedThisFrame=false;
 if(audioEnabled&&!opponentFrame&&!raceSetupFrame)require50DU(advanceSpeechTimers50HG(.016f,lapContinuity.prefix,speechState),
     "native speech countdown failed");
 // The live path does not call fixture/oracle callbacks; retain a bounded
 // diagnostic frame label for any future cold-fixture diagnostics.
 x.frame=static_cast<unsigned>(std::min<std::uint64_t>(completedFrames,
     std::numeric_limits<unsigned>::max()));
 FrameReader49K read{};read.preparedFactor=.016f;read.codes=x.actionCodes;
 read.context.previousFlagsB9C=s.bodies.main.controls.player.flagsB9C;
 if(s.fields.vehicle58==1u){
  runNative([&]{return readAiVehicleFrame50IV(s,x.bindings,read.preparedFactor,x.workspace,x.diagnostic);},"original AI frame entry");
  AiRuntimeInput50IV aiInput{};aiInput.applicationGlobals=&native_ai50IV::applicationNoise().ai;aiInput.mapFF0=navigationMap.get();aiInput.aiFF4=navigationAI.get();
  aiInput.orientationTrackBC=navigationAI.get();aiInput.aiMetadata=&originalAiMetadata;aiInput.tuning=&originalAiTuning;
  aiInput.racers=originalAiSession.observed;aiInput.racerCount=raceRacerCount;
  aiInput.raceFields=&originalAiSession.fields[originalAi.managerIndexFE0];aiInput.timing=&lapContinuity.timing;
  aiInput.actualZoneOrdinal48=mainMembership.storage.links[0].zone?mainMembership.storage.links[0].zone-1u:0u;
  aiInput.managerHumanCount34=originalAiHumanCount;
  // Consumers here read1125C low byte; the native roster owns the complete
  // packed count word, with the authentic three-lap low5 selection retained.
  aiInput.settings1125C=static_cast<std::uint8_t>(originalAiSettings1125C);
  aiInput.aiTrackLength3C=originalAiMetadata.trackLength3C;
  aiInput.raceOwnersBound=originalAiSession.rankBound&&mainMembership.storage.links[0].zone!=0;
  if(!driveOriginalAi50IV(s,x.bindings,x.parameters,navigationProgress,navigationCA0,
      lapContinuity.state.vehicleCC8,boostState,originalAi,aiInput,originalAiReceipt)){
   std::cerr<<"ORIGINAL AI source boundary: slot="<<originalAi.managerIndexFE0
       <<" state="<<originalAiReceipt.stateFE8<<" pc="<<std::hex<<originalAiReceipt.sourceBoundary<<std::dec
       <<" ai_dispatches="<<originalAi.dispatchedFrames<<" brakes="<<originalAi.brakingFrames<<'\n';
   recoveryPaused=true;return false;
  }
  lapContinuity.state.vehicleCD8=s.fields.impactFlagsCD8;
 }else{
 const DeviceQueryContext48Y queryContext{eventContext.present,applicationEvents.rootFlags228,
     playerEnvironment.held.context518,eventContext.mask1898};
 runNative([&]{return controller.prepareFrameReader(queryContext,read.context,
     read.codes,read.preparedFactor,read);},"native held provider");
 runNative([&]{return readVehicleFrame49W(s,x.bindings,read,x.workspace,x.diagnostic);},"persistent reader");
 //1A15E4/1A160C follows1A1A20 and precedes1A25D0/228620. Consume the
 // prior outer frame's CA0 before228724 copies this frame's local body pose.
 // This owns progression words only; the existing reset cut remains in place.
 require50DU(s.phase==VehicleFramePhase49W::PostPlayer&&!s.fields.vehicle58,
     "source player navigation phase unavailable");
 if((s.fields.applicationByte58&3u)==2u||(s.fields.impactFlagsCD8&1u)){
  TrackNavigationInput50EU navigationInput{};
  std::memcpy(navigationInput.previousCA0,navigationCA0,sizeof navigationCA0);
  std::memcpy(navigationInput.currentLocal190,
      s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].localC0.v+12,12);
  navigationInput.currentLocal190[3]=1.f;
  navigationInput.mainBody104=m.components.probe348.field104;
  TrackNavigationReceipt50EU receipt{};
  const TrackNavigationMapEffects50GH lapEffects{&lapOwner,&lap_drive50GJ::Owner::accepted};
  const bool advanced=advancePlayerTrackNavigationEffects50GH(*navigationMap,*navigationAI,
      navigationInput,lapEffects,navigationProgress,receipt,navigationDiagnostic);
  addCounter50DU(nativeCalls);
  require50DU(reportNavigation(advanced,"live1A0DA0"),"source player navigation rejected");
  addCounter50DU(navigationUpdates);
  addCounter50DU(navigationAISequential,receipt.aiSequential);
  addCounter50DU(navigationMapSequential,receipt.mapSequential);
  addCounter50DU(navigationAIScan,receipt.aiScan);
  addCounter50DU(navigationMapScan,receipt.mapScan);
 }else addCounter50DU(navigationGateSkips);
 //1A15EC packing-reference projection is joined at the serial arena boundary
 // immediately after this player update, before any AI update.
 // Original1A1614: once the human manager gate ends, a finished player runs
 //1A36A0 after navigation and before wrong-way/base processing. Opponents still
 // use the explicitly host-authored course adapter, not this player leaf.
 if(!opponentFrame&&!raceSetupFrame&&humanResultsActive){
  require50DU((s.fields.applicationByte58&3u)!=2u&&(s.fields.impactFlagsCD8&1u),
      "native finish driver requires ended human gate and source CD8");
  const auto current=navigationProgress.reset.vehicleC88;
  require50DU(current<navigationMap->track.count,"finished player map target unavailable");
  const auto& scene=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)];
  PlayerFinishInput50IA finishInput{};
  finishInput.resultsCountdown94=humanResultsCountdown;
  finishInput.authoredSteering10A4=authoredFinishSteering10A4;
  finishInput.bodyCachedSpeed34=m.cachedSpeed34;
  finishInput.bodyWorldAngularA4=m.components.root.worldAngularVelocityA0.v[1];
  finishInput.engineDifference5C=authored.engineDifference5C;
  for(unsigned axis=0;axis<3;++axis){
   finishInput.bodySceneWorldPosition[axis]=scene.world100.v[12+axis];
   finishInput.mapCurrentNodePosition[axis]=navigationMap->track.nodes[current].position[axis];
   finishInput.mainSceneLocalForward[axis]=scene.localC0.v[8+axis];
  }
  finishInput.mainSceneLocalUpY=scene.localC0.v[5];
  finishInput.previousFlagsB9C=m.controls.player.flagsB9C;
  PlayerFinishResult50IA finishResult{};
  runNative([&](){return driveFinishedPlayer50IA(finishInput,finishResult);},
      "original finished-player controls rejected");
  m.controls.player=finishResult.player;
  // This source model write precedes the first frame use of parameters in
  //217DE0; keep the resulting descriptor unchanged for the rest of this frame.
  x.parameters.bodies.main.aero.steering100=finishResult.aeroSteering100;
  finishDriverAppliedThisFrame=true;
 }
 //1A1620: after the entire navigation branch, before undertrack processing.
 // Navigation above consumed the previous update's actual flags10CC bit1.
 WrongWayProgressInput50GO wrongWayInput{};
 wrongWayInput.application34C=applicationWrongWayBypass34C;
 wrongWayInput.vehicle40=m.vehicleState40;
 wrongWayInput.vehicleC8C=navigationProgress.reset.vehicleC8C;
 wrongWayInput.vehicleC88=navigationProgress.reset.vehicleC88;
 wrongWayInput.config58=s.fields.applicationByte58;
 wrongWayInput.vehicleCD8=s.fields.impactFlagsCD8;
 std::memcpy(wrongWayInput.localForwardE0,
     s.nodes[static_cast<unsigned>(x.bindings.animation.bodyScene288)].localC0.v+8,12);
 // The original native HUD owner is genuinely absent; the separate HOST overlay
 // below observes state and is not substituted as a completed original UI call.
 wrongWayInput.warningOwner=nullptr;
 WrongWayProgressState50GO wrongWayState{lapContinuity.state.vehicleFF8,
     lapContinuity.prefix.flags10CC};
 require50DU(beginWrongWayProgress50GO(navigationMap->track,lapOwner.track,wrongWayInput,
     wrongWayState,wrongWayContinuation,wrongWayReceipt,wrongWayDiagnostic),
     "native wrong-way progression rejected");
 require50DU(wrongWayContinuation.pending.operation==WrongWayHudOperation50GO::None,
     "unowned original warning callback pending");
 lapContinuity.state.vehicleFF8=wrongWayState.vehicleFF8;
 lapContinuity.prefix.flags10CC=wrongWayState.flags10CC;
 addCounter50DU(wrongWayUpdates);
 if(wrongWayReceipt.wrongWay)addCounter50DU(wrongWayActiveTicks);
 }
 //228724..228730:28B180 copies only local XYZ from the actual340->288
 // binding, without world-cache refresh, immediately before217DE0 here.
 // The supplied preserving cut covers intervening unrelated source effects.
 require50DU(!m.applicationActive11C&&!s.fields.vehicle3C&&m.domain.sceneAttached,
     "source CA0 capture owner unavailable");
 std::memcpy(navigationCA0,
     s.nodes[static_cast<unsigned>(x.bindings.animation.bodyScene288)].localC0.v+12,12);
 addCounter50DU(navigationCA0Copies);
 PostPlayerAt217DE0_49K post{};post.player=s.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=x.configurationEntryNonzero;
 if(s.fields.vehicle58==1u){
  runNative([&]{return bindAiVehicleFrame50IV(s,x.bindings,x.parameters,x.workspace,x.diagnostic);},"original AI native binding");
  if(m.components.bindings.aeroBrakeEC==1.f&&m.controls.player.raw.values[0]!=0.f)
   ++originalAi.boundBrakeSteerFrames;
 }
 else runNative([&]{return bindVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,x.workspace,x.diagnostic);},"persistent binding");
 const BoostInput50FK boostInput{m.frameFactor,m.components.engine.smoothedDrive[0],
     m.components.engine.smoothedDrive[1],m.cachedSpeed34,m.controls.player.flagsB9C,
     s.fields.vehicle58,dynamicContactEnabled?dynamicProfile.game34:2u,0,false};
 // Fresh186EF0 index5 explicitly clears10FC0. D40 remains the existing
 // admitted-absent native topology; no owner is synthesized here.
 // Native host subscriber at the original boost boundary; this does not claim
 // that the separate original D40 topology guard has been closed.
 if(audioEnabled&&!opponentFrame&&!raceSetupFrame){
  const bool heldBoost=(m.controls.player.flagsB9C&1u)&&!(m.controls.player.flagsB9C&2u);
  const float heating=boostParameters.heatRate88*(boostInput.engine70+boostInput.engine74);
  require50DU(liveAudio->boostAtBoundary(heldBoost,heating,boostState.vehicle64).okay(),
      "source boost audio event rejected");
 }
 runNative([&]{return s.fields.vehicle58==1u?
     advanceAiThermal50IT(boostInput,boostParameters,boostState,boostState,boostDiagnostic):
     advancePlayerBoost50FK(boostInput,boostParameters,boostState,boostState,boostDiagnostic);},"native boost controller");
 x.selectors.vehicle64=boostState.vehicle64;
 runNative([&]{return graphicsVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::Base2176E0BeforeGraphics),x.workspace,x.diagnostic);},"persistent graphics");
 ImpactConsumerInput49Z consumer{};consumer.vehicle64=boostState.vehicle64;consumer.managerFlags58=s.fields.applicationByte58;consumer.profile34=2;consumer.finalDamageThreshold64=1;VehicleImpactEvents49Z consumerEvents{};
 if(dynamicContactEnabled){
  consumer.profile34=dynamicProfile.game34;consumer.profile10FAC=0;
  //186654 sets the consumed fresh profile11050 bit2; no options override.
  consumer.profileFlags11050=dynamicConsumerFlags11050;
  consumer.stopBeforeControllerFeedback50IW=true;
  consumer.finalDamageThreshold64=dynamicFinalTuning.threshold64;
  consumer.finalDamageIncrement68=dynamicFinalTuning.increment68;
  consumer.dynamicPartnerEventsBound=true;consumer.partnerEventThresholdBound=dynamicTuning.bound;
  consumer.partnerEventThreshold48=dynamicTuning.points3C[3];
 }
 const bool consumersCompleted=consumeVehicleImpacts49Z(s,x.bindings,consumer,consumerEvents,x.workspace,x.diagnostic);
 if(dynamicContactEnabled&&(consumersCompleted||
     (x.diagnostic.error==VehicleFrameError49W::ExternalCut&&
      (x.diagnostic.sourceStage==0x222d70u||x.diagnostic.sourceStage==0x222d8cu)))){
  refreshDynamicSelfActor();
  //222CFC event0 precedes the first feedback helper. The terminal continuation
  // returns only its exact prefix and request, never later damage/timer writes.
  for(std::size_t index=0;index<consumerEvents.partners.count;++index){
   const auto& event=consumerEvents.partners.values[index];
   if(event.partnerId>=raceRacerCount)dynamicBoundary(event.sourcePC,"original partner actor index unavailable");
   const auto appended=appendDynamicContactEvent1_50IW(dynamicEvents,dynamicActorResolver,
       static_cast<RaceActorId50ID>(dynamicSelfSlot+1),event.partnerId+1,event.word8,
       dynamicEventBindings.eventOwner33CBound);
   if(!appended.helperReturned())dynamicBoundary(event.sourcePC,"original shared contact event enqueue pending");
  }
  if(consumerEvents.count)dynamicBoundary(consumerEvents.values[0].sourcePC,
      "actual vehicle5C/controller registry and feedback helper are not joined");
 }
 runNative([&]{return consumersCompleted;},"persistent impact consumers");
 if(s.phase==VehicleFramePhase49W::PostImpactConsumers){
 runNative([&]{return repairVehicleImpacts50A(s,x.bindings,x.workspace,x.diagnostic);},"persistent repair");
 if(audioEnabled&&!opponentFrame&&!raceSetupFrame){
  const float damage[]={s.fields.damage7D0,s.fields.damage7D4,s.fields.damage7D8,s.fields.damage7DC};
  require50DU(liveAudio->repairCompletion(m.controls.player.flagsB9C,s.fields.vehicle3C,damage).okay(),
      "source repair-completion audio rejected");
  require50DU(afterRepairSpeechState50HG(s.fields.damage7D0,s.fields.damage7D4,
      s.fields.damage7DC,lapContinuity.prefix),"native repair speech state rejected");
  if(dynamicContactEnabled)refreshDynamicSelfActor();
  const auto produced=dynamicContactEnabled?
      produceQueuedDamageSpeech50ID(speechInput(),lapContinuity.prefix,1u,dynamicActorResolver,dynamicEvents):
      (mode1Enabled?mode1->produceDamage(speechInput()):
      produceDamageSpeechEvent50HG(speechInput(),lapContinuity.prefix,pendingSpeech));
  require50DU(produced.status==PodSpeechStatus50HG::Complete,"native damage speech enqueue rejected");
  if(produced.eventProduced)++speechEvents;
 }
 ImpactEffectsInput50B owners{};owners.models[0].node=x.bindings.visual.left1BC;owners.models[1].node=x.bindings.visual.right1B8;owners.models[2].node=owners.models[3].node=SceneNodeIndex49P::none;std::size_t count=0;owners.damage00Matches=namesForEffects.damage00;owners.nameCount=s.sceneNodeCount;for(unsigned side=0;side<2;++side){auto&model=owners.models[side];require50DU(rr::assets::effectModelRoot50BL(namesForEffects,model.node),"bound model name scope");model.flags03=3;model.effect800={nativeEffects[2+side].id,nativeEffects[2+side].state.flags1D4};model.effect804={nativeEffects[4+side].id,nativeEffects[4+side].state.flags1D4};model.effect80C={nativeEffects[6+side].id,nativeEffects[6+side].state.flags1D4};}
 runNative([&]{return effectsVehicleImpacts50O(s,x.bindings,owners,effectsWorkspace,generatedEffects,maxImpactEffectEvents50B,count,x.workspace,x.diagnostic);},"actual effect control bindings");runNative([&]{return consumeEffects(generatedEffects,count);},"consume impact effect controls");
 if(audioEnabled&&!opponentFrame&&!raceSetupFrame){
  MaterialAudioInput50HI materialInput{};
  materialInput.channelArgument=s.impactSupplement.timers820[4]>0.f?2u:3u;
  require50DU(resolveContactMaterial50HI(spring.records.currentCandidate130,
      audioContactMaterials,terrainOwnerCount,materialInput.contactMaterialA9),"actual audio contact identity unresolved");
  materialInput.speedRatio70=boostState.speedRatio70;
  if(audioListenerReady){
   const auto& p=m.components.root.body.position;
   materialInput.listenerDistance=std::hypot(p.v[0]-audioListener.x,p.v[2]-audioListener.z);
  }
  require50DU(materialAudio->update(materialInput,s.noise.sharedRandomState).okay(),
      "native material audio subscriber rejected");
 }
 OptionalProbeOwners50P optionalOwners{}; // Development topology has no optional dust owner.
 OptionalProbeResult50P optional{};
 runNative([&]{return optionalProbeVehicleImpacts50P(s,x.bindings,optionalOwners,optional,x.workspace,x.diagnostic);},"explicit absent optional dust-owner context");runNative([&]{return consumeEffects(optional.events,optional.count);},"consume dust stop controls");ImpactTransitionResult50C decision{};
 const bool damageReady=decideVehicleImpacts50P(s,x.bindings,0x3f800000u,0,
     decision,x.workspace,x.diagnostic);
 if(!damageReady&&x.diagnostic.error==VehicleFrameError49W::ExternalCut&&
    decision.status==ImpactTransitionStatus50C::unsupportedTransition){
  addCounter50DU(nativeCalls);recoveryPaused=true;
  if(originalAiEnabled){
   std::cerr<<"ORIGINAL AI source-recovery boundary: kind=damage pc="<<std::hex
       <<decision.sourcePC<<std::dec<<" phase="<<static_cast<unsigned>(s.phase)
       <<"; no checkpoint prepared or session rebuilt.\n";
   require50DU(false,"original AI single session reached native damage boundary");
  }
  if(opponentFrame)return false; // A failed AI never restarts the human session.
  recovery50FI::Plan nextCheckpoint{};recovery50FI::Diagnostic recoveryDiagnostic{};
  BodyMatrix48G groundedCheckpoint{};
  const bool prepared=recovery50FI::prepareDamageRecovery(*navigationMap,navigationProgress,s,
      x.bindings,decision,x.diagnostic,0x3f800000u,0,x.parameters.visual.verticalLimit7EC,liveWorld,
      nextCheckpoint,recoveryDiagnostic);
  const bool grounded=prepared&&recovery50FI::resolveGround(ground,nextCheckpoint,16,0,
      groundStack.data(),groundStack.size(),groundedCheckpoint,recoveryDiagnostic);
  if(grounded){
   checkpointOutput=nextCheckpoint;checkpointRequested=true;
   std::cerr<<"Damage checkpoint recovery after "<<completedFrames<<" completed ticks: source=0x"
            <<std::hex<<decision.sourcePC<<std::dec<<" anchor="<<nextCheckpoint.selection.anchorNode
            <<" zone="<<nextCheckpoint.primaryZone<<". Fresh vehicle; progress retained.\n";
  }else{
   std::cerr<<"Damage recovery unavailable: error="<<static_cast<unsigned>(recoveryDiagnostic.error)
            <<". Last committed image retained; F5/restart returns to grid.\n";
  }
  return false;
 }
 runNative([&]{return damageReady;},"ordinary damage decision");
 }
 require50DU(s.phase==VehicleFramePhase49W::AfterImpactBlock,"source impact outer skip continuation unavailable");
 PlainEffectOwners50Q plainOwners{};for(unsigned side=0;side<2;++side){for(unsigned array=0;array<3;++array){const auto&owner=nativeEffects[2+array*2+side];plainOwners.slots[array*4+side]={owner.id,owner.state.flags1D4};}}PlainEffectResult50Q plain{};
 runNative([&]{return plainEffectsVehicleTail50Q(s,x.bindings,plainOwners,plain,x.workspace,x.diagnostic);},"plain effects");runNative([&]{return consumeEffects(plain.events,plain.count);},"consume plain effect controls");runNative([&]{return packVehicleFrame50S(s,x.bindings,x.configurationEntryNonzero,x.workspace,x.diagnostic);},"packet");BaseTailBindings50X baseBinding{77,Probe128Binding50X::Null};
 const bool baseAdvanced=advanceVehicleBaseThroughEngine50X(s,x.bindings,clock,
     baseBinding,baseTail,baseGlobals,coldFlame,baseWorkspace,x.workspace,x.diagnostic);
 if(!baseAdvanced&&x.diagnostic.error==VehicleFrameError49W::ExternalCut&&
    x.diagnostic.sourceStage==0x228BBC){
  addCounter50DU(nativeCalls);recoveryPaused=true;
  if(originalAiEnabled){
   std::cerr<<"ORIGINAL AI source-recovery boundary: kind=undertrack pc="<<std::hex
       <<x.diagnostic.sourceStage<<std::dec<<" phase="<<static_cast<unsigned>(s.phase)
       <<"; no checkpoint prepared or session rebuilt.\n";
   require50DU(false,"original AI single session reached native undertrack boundary");
  }
  if(opponentFrame)return false; // A failed AI never restarts the human session.
  recovery50FI::Plan nextCheckpoint{};recovery50FI::Diagnostic recoveryDiagnostic{};
  BodyMatrix48G groundedCheckpoint{};
  const bool prepared=recovery50FI::prepare(*navigationMap,navigationProgress,s,x.bindings,
      x.diagnostic,x.parameters.visual.verticalLimit7EC,liveWorld,nextCheckpoint,recoveryDiagnostic);
  const bool grounded=prepared&&recovery50FI::resolveGround(ground,nextCheckpoint,16,0,
      groundStack.data(),groundStack.size(),groundedCheckpoint,recoveryDiagnostic);
  if(grounded){
   checkpointOutput=nextCheckpoint;checkpointRequested=true;
   std::cerr<<"Automatic checkpoint recovery after "<<completedFrames<<" completed ticks: anchor="
            <<nextCheckpoint.selection.anchorNode<<" zone="<<nextCheckpoint.primaryZone
            <<". Rebuilding native vehicle session; damaged values and progress retained.\n";
  }else{
   std::cerr<<"Checkpoint recovery unavailable: error="<<static_cast<unsigned>(recoveryDiagnostic.error)
            <<" source="<<static_cast<unsigned>(recoveryDiagnostic.source.error)
            <<" ground="<<static_cast<unsigned>(recoveryDiagnostic.groundPhase)
            <<". Last committed image retained. F5/restart returns to grid; quit exits.\n";
  }
  return false;
 }
 runNative([&]{return baseAdvanced;},"persistent base engine");
 runNative([&]{return finishVehicleBaseAndMainCache50Y(s,x.bindings,baseTail,true,poseHistory,f::cut(PreservingCut49W::OtherChildrenBeforeMain340),x.workspace,x.diagnostic);},"persistent base pose history");
 runNative([&]{return refreshSecondaryVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::OtherChildrenBetween340And35C),x.workspace,x.diagnostic);},"secondary cache");runNative([&]{return scheduleVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::OtherChildrenAfter35C),60,x.workspace,x.diagnostic);},"schedule");
 require50DU(s.bodies.main.schedule.count==1,"16ms diagnostic step requires one native substep");

 runNative([&]{return beginVehicleSubstep49W(s,x.bindings,x.selectors,x.workspace,x.diagnostic);},"native selector");
 MainStaticDetectorAdmission50H admission{true,true,true,false,false,false,true,true,2,mainQueryRef,x.bindings.lifecycle.main190};
 runNative(collectRegistered,"ordered prepared-list vehicle candidates");
 const auto mainQueryAttempt=queryMainLiveIndexedStaticVehicle50EQ(s,x.bindings,admission,mainLiveQuery,query,candidates.data(),candidates.size(),registeredIndices,registeredCount,initial,workspace,destination,queryResult);
 if(mainQueryAttempt.status==VehicleLiveQueryStatus50EQ::PrefixRejected&&
    recoverPortalBoundary(mainQueryAttempt.prefix,"main")){addCounter50DU(nativeCalls);return false;}
 runNative([&]{return acceptLiveQuery(mainQueryAttempt,query,mainMembership,mainCrossedPortals,"main");},
     "registered main terrain query");
 runNative([&]{return commitCandidateEffects(effects);},"commit main query candidate effects");qfilter.flagsF8=queryResult.queryFlagsF8;query.initialQueryCurrentIdentity=queryResult.currentCandidateIdentity;initial.selected=queryResult.selected;
 spring.records=queryResult.records;std::memcpy(records,queryRecords,sizeof records);

 if(s.bodies.vehicle838){
 MainStaticDetectorAdmission50H secondaryAdmission=admission;secondaryAdmission.queryScene=x.bindings.lifecycle.physical19C;secondaryAdmission.registrationE0=secondaryQueryRef;
 runNative([&]{return prepareSecondaryLiveQueryScene50EQ(s,x.bindings,secondaryAdmission,secondaryLiveQuery,x.workspace)==SecondaryQuerySceneStatus50J::Ready;},"registered original secondary scene getter");
 // Secondary uses the same first-substep merged list, without recollection.

 const auto secondaryQueryAttempt=querySecondaryLiveIndexedStaticVehicle50EQ(s,x.bindings,secondaryAdmission,secondaryLiveQuery,secondaryQuery,candidates.data(),candidates.size(),registeredIndices,registeredCount,si,sw,sd,secondaryResult);
 if(secondaryQueryAttempt.status==VehicleLiveQueryStatus50EQ::PrefixRejected&&
    recoverPortalBoundary(secondaryQueryAttempt.prefix,"secondary")){addCounter50DU(nativeCalls);return false;}
 runNative([&]{return acceptLiveQuery(secondaryQueryAttempt,secondaryQuery,secondaryMembership,secondaryCrossedPortals,"secondary");},
     "registered secondary terrain query");
 runNative([&]{return commitCandidateEffects(sde);},"commit secondary query candidate effects");secondaryFilter.flagsF8=secondaryResult.queryFlagsF8;secondaryQuery.initialQueryCurrentIdentity=secondaryResult.currentCandidateIdentity;si.selected=secondaryResult.selected;
 secondarySpring.records=secondaryResult.records;std::memcpy(secondaryRecords,secondaryQueryRecords,sizeof secondaryRecords);
 
 }
 TerrainPrepareState48M terrainPrepared{};TerrainQueryResult48M terrainResult{};
 runNative([&]{return prepareVehicleTerrain50FP(s,x.bindings,x.parameters,terrainOwner.initial.segment,
     terrainPrepared,x.workspace,x.diagnostic);},"prepare enabled native terrain probe");
 const auto terrainAttempt=live_terrain_probe50fr::query(terrainLiveOwner,terrainPrepared,
     terrainOwner.metadata,terrainContext,probeIndices,probeCount,terrainResult);
 if(terrainAttempt.status==live_terrain_probe50fr::Status::PrefixRejected&&
    recoverPortalBoundary(terrainAttempt.prefix,"terrain")){addCounter50DU(nativeCalls);return false;}
 runNative([&]{
   if(terrainAttempt.status!=live_terrain_probe50fr::Status::Complete)
    std::cerr<<"terrain query status "<<unsigned(terrainAttempt.status)<<" prefix "<<unsigned(terrainAttempt.prefix.status)
       <<" query "<<unsigned(terrainAttempt.queryStatus)<<" prefix_committed "<<terrainAttempt.prefixCommitted<<'\n';
   return terrainAttempt.status==live_terrain_probe50fr::Status::Complete;
 },"actual registered terrain limited-list query");
 for(std::size_t i=0;i<probeCount;++i){const auto index=probeIndices[i];
  const auto& source=candidates[index].candidate.collision;auto& mirror=movementCandidates[index].collision;
  mirror.filter.flagsF8=source.filter.flagsF8;mirror.copied40=source.copied40;mirror.copied50=source.copied50;
 }
 runNative([&]{return commitVehicleTerrainQuery50FP(s,x.bindings,terrainResult,x.workspace,x.diagnostic);},
     "commit actual terrain surface");
 ++terrainQueries;if(terrainResult.terrain.surface.hit110||terrainResult.terrain.surface.hit114)++terrainHitFrames;
 PostWorld49W world{};world.point=s.bodies.vehicle838?WorldCut49W::First358Secondary36CAnd219420:WorldCut49W::First358And219420;world.effects=contactEffects49W(s);world.applicationGateByte58=s.fields.applicationByte58;
 runNative([&]{return produceVehicleForces49W(s,x.bindings,x.parameters,world,x.workspace,x.diagnostic);},"native main forces");
 ResponseStorage50K responseStorage;auto responseWorkspace=responseStorage.view(x.workspace);
 runNative([&]{return respondMainStaticVehicle50G(s,x.bindings,x.parameters,settings,response,x.secondaryInputs,query.queryScene,1,spring,records,40,responseWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual main contact response");
 require50DU(s.phase==(s.bodies.vehicle838?VehicleFramePhase49W::Post368:VehicleFramePhase49W::MainIntegration),"source-selected response phase");

 if(s.bodies.vehicle838){
 ResponseStorage50K secondaryStorage;auto secondaryWorkspace=secondaryStorage.view(x.workspace);
 runNative([&]{return respondSecondaryStaticVehicle50J(s,x.bindings,x.parameters,secondarySettings,secondaryResponse,{78},2,secondarySpring,secondaryRecords,60,secondaryWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual secondary contact and integration");
 require50DU(s.phase==VehicleFramePhase49W::MainIntegration,"secondary response integration phase");
 }
 if(dynamicContactEnabled){
  native_dynamic_contact50IW::Diagnostic peerDiagnostic{};
  if(!dynamicPeers.refreshSelfAtImpact(s,peerDiagnostic))
   dynamicBoundary(peerDiagnostic.sourcePC,"dynamic impact live self projection unavailable");
  DynamicVehicleImpactCursor50IW impact{};
  auto status=beginVehicleDynamicImpact50IW(s,x.bindings,x.parameters,poseHistory,
      dynamicPeers.managerView(),dynamicTuning,dynamicProfile,impact,x.diagnostic);
  if(status==DynamicVehicleImpactStatus50IW::NeedFirstContact){
   fulfillDynamicMusic();
   status=resumeVehicleDynamicImpact50IW(s,poseHistory,impact,x.diagnostic);
  }
  if(status!=DynamicVehicleImpactStatus50IW::Complete)
   dynamicBoundary(x.diagnostic.sourceStage,"original dynamic impact continuation rejected");
 }
 runNative([&]{return integrateVehicleMain49W(s,x.bindings,x.parameters,x.workspace,x.diagnostic);},"contact-driven body integration");
 require50DU(s.phase==VehicleFramePhase49W::Query219120,"real late-query boundary");
 std::memcpy(&endpoints[0],s.bodies.main.priorPoint7B0,16);std::memcpy(&endpoints[1],s.bodies.main.currentPoint7C0,16);
 runNative(gatherRegistered,"registered movement candidates");
 runNative([&]{const auto outcome=queryMovementStaticCandidates50L({true,true},movement,movementSource,registeredMovement,registeredCount,movementWorkspace,movementOutput,movementResult,movementFlag);movementFlag=outcome.vehicle7A0;return outcome.status==Type3QueryStatus48P::Complete;},"actual movement segment against terrain");
 runNative([&]{return commitCandidateEffects(movementEffects);},"commit movement query candidate effects");movement.filter.flagsF8=movementResult.queryFlags;movement.initial=movementResult.state;movement.initialQueryCurrent=movementResult.currentCandidate;movementInitial=movementDestination;
 MovementQueryCompletion50M completion{};completion.resultFlagsF8=movementResult.queryFlags;std::memcpy(&completion.selectedPoint,&movementResult.state.selectedPoint,16);completion.pushDistance434710=10;completion.representedStatePreserved=completion.omittedBB0WNotConsumed=true;
 runNative([&]{return finishVehicleMovementQuery50M(s,x.bindings,x.parameters,completion,x.workspace,x.diagnostic);},"native actual movement result and frame tail");
 require50DU(s.phase==VehicleFramePhase49W::Ready,"one-substep diagnostic frame complete");
 if(audioEnabled&&!opponentFrame&&!raceSetupFrame){
  const auto& engineParameters=liveAudio->engineParameters(); // Selected original PHY tuning.
  // Original19EEA0 order: cooldown, active curve, damage selection, then
  // engine pitch controls. The same player RNG already used by effects and
  // material audio must also receive the source's two healthy-state draws.
  const EngineDamageInput50IA damageInput{m.frameFactor,s.fields.damage7D0,
      s.fields.damage7D4,s.fields.damage7DC};
  const auto damageReceipt=updateEngineDamageAudio50IA(damageInput,engineDamage,
      engineDamage,s.noise.sharedRandomState);
  require50DU(damageReceipt.okay(),"original engine damage modulation rejected");
  ++engineDamageUpdates;engineDamageDraws+=damageReceipt.randomDraws;
  if(damageReceipt.damageDipStarted)++engineDamageDips;
  if(damageReceipt.returnStarted)++engineDamageReturns;
  if(damageReceipt.dipFinished)++engineDamageCompletions;
  PodEngineInput50HE engineInput{boostState.speedRatio70,engineDamage.scale484,liveAudio->boostPitch()};
  require50DU(liveAudio->engine(engineParameters,engineInput).okay(),"native engine audio controls rejected");
  PodWarningInput50HE warning{};
  warning.damage7D0=s.fields.damage7D0;warning.damage7D4=s.fields.damage7D4;
  warning.damage7DC=s.fields.damage7DC;warning.speedRatio70=boostState.speedRatio70;
  warning.vehicle64=boostState.vehicle64;warning.flagsCD8=s.fields.impactFlagsCD8;
  warning.flags10CC=lapContinuity.prefix.flags10CC;
  warning.emitter22C=liveAudio->boostPitch()?2u:0u;
  warning.sprayInputKnown=false; // Water probe118 is not represented by this client.
  require50DU(liveAudio->warnings(warning).okay(),"native warning audio projection rejected");
 }
 addCounter50DU(completedFrames);
 if(m.components.probe348.field104)++terrainSupportedFrames;
 if(m.controls.player.flagsB9C&2u)addCounter50DU(repairActiveTicks);
 if(spring.records.contact3C)addCounter50DU(mainContactFrames);
 if(s.bodies.vehicle838&&secondarySpring.records.contact3C)addCounter50DU(secondaryContactFrames);
 return true;
 };

 // Original racers have distinct immutable resource owners and share only the
 // serial execution arena, never tuning, poses, contacts or progression histories.
 native_racer50HN::Bank racerBank;
 native_racer50IZ::FrameReads racerFrameReads;
 native_ai50IV::ObservationReads originalAiReads;
 auto captureDynamicBeforeFrame=[&](std::size_t slot){
  if(!dynamicContactEnabled)return;
  native_dynamic_contact50IW::Diagnostic diagnostic{};
  if(!dynamicRegistry.captureBeforeFrame(racerBank,racerFrameReads,slot,dynamicPeers,diagnostic))
   dynamicBoundary(diagnostic.sourcePC,"every dynamic peer must be active and Ready");
  dynamicSelfSlot=slot;
  for(std::size_t actor=0;actor<racerBank.count();++actor){
   dynamicActors.values[actor]={racerBank.read(actor,racerFrameReads.vehicle3C),
       racerBank.read(actor,racerFrameReads.vehicle58),dynamicRacer60[actor],
       racerBank.read(actor,originalAiReads.lapCC8),
       static_cast<std::uint32_t>(originalAiSession.rank[actor].placeCCC),
       racerBank.read(actor,racerFrameReads.flagsCD8)};
   dynamicActors.bound[actor]=true;
  }
 };
 auto requireOriginalAiFleetReady=[&](){
  // Every actual racer is an input to the joined source manager. A failed
  // native frame is retained by Bank::visit, but is not a manager observation.
  // Check the entire fleet before any observation, timing or rank mutation;
  // never synthesize Ready or silently remove a failed racer from the field.
  for(std::size_t slot=0;slot<racerBank.count();++slot){
   const bool active=racerBank.active(slot);
   if(active&&racerBank.read(slot,racerFrameReads.phase)==VehicleFramePhase49W::Ready&&
      (!allAiEnabled||(racerBank.read(slot,racerFrameReads.vehicle58)==1u&&
         racerBank.read(slot,racerFrameReads.aiOwnersBound)&&
         racerBank.read(slot,originalAiReads.initialized)&&
         racerBank.read(slot,originalAiReads.managerIndexFE0)==slot)))continue;
   std::cerr<<"ORIGINAL AI world boundary: slot="<<slot<<" active="<<active
       <<" phase="<<static_cast<unsigned>(racerBank.read(slot,racerFrameReads.phase))
       <<" vehicle58="<<racerBank.read(slot,racerFrameReads.vehicle58)
       <<" ai_source_boundary="<<std::hex<<racerBank.read(slot,originalAiReads.sourceBoundary)
       <<" frame_source_stage="<<racerBank.read(slot,originalAiReads.frameSourceStage)<<std::dec
       <<"; native partial state retained; manager was not advanced.\n";
   require50DU(false,"original AI requires every actual racer active and Ready");
  }
 };
 auto observeOriginalAiFleet=[&](){
  requireOriginalAiFleetReady();
  for(std::size_t slot=0;slot<racerBank.count();++slot)
   originalAiReads.observe(originalAiSession,slot,racerBank,racerFrameReads,*navigationMap,originalAiMapMetadata);
 };
 auto rankOriginalAiFleet=[&](){
  observeOriginalAiFleet();
  require50DU(native_ai50IV::rank(originalAiSession,racerBank.count()),"original AI source rank/remaining boundary");
 };
 auto updateOriginalAiPacking=[&](){
  //1A15EC belongs to the human update. With no humans it is never called;
  // every actual AI still executes171C60 and its authentic FF8<0 return.
  if(allAiEnabled)return;
  // Original1A15EC follows the sole human's navigation. The serial arena
  // transports its source writes before any AI frame, without nested visits.
  if((s.fields.applicationByte58&3u)!=2u)return;
  AiPackingRacerObservation50IU rows[aiRaceRankCapacity50IV]{};
  for(std::size_t slot=0;slot<racerBank.count();++slot){const auto&o=originalAiSession.observed[slot];
   rows[slot]={o.vehicle58,o.flagsCD8,originalAiSession.rank[slot].placeCCC,
       o.vehicle58==1u?&originalAiSession.packingReferences[slot]:nullptr};
  }
  AiPackingReferenceDiagnostic50IU diagnostic{};
  require50DU(updateAiPackingReferences50IU(0,originalAiSession.rank[0].placeCCC,1,
      originalAiTuning,{},rows,racerBank.count(),diagnostic),"original AI packing-reference boundary");
  racerBank.visit(originalAiSlot,[&](){originalAi.referenceFF8=originalAiSession.packingReferences[originalAiSlot];});
 };
 auto reportOriginalAiFleet=[&](const char* reason){
  if(!originalAiEnabled)return;
  std::cerr<<"original_ai_all enabled="<<allAiEnabled<<" racer_count="<<racerBank.count()
      <<" human_count="<<originalAiHumanCount<<" ai_count="<<(allAiEnabled?racerBank.count():1u)
      <<" camera_slot=0 camera_racer60="<<playerRacer60
      <<" host_driver_calls="<<allAiHostDriverCalls<<" controller_samples="<<allAiControllerSamples
      <<" keyboard_packets="<<allAiKeyboardPackets<<" reason="<<reason<<'\n';
  for(std::size_t slot=0;slot<racerBank.count();++slot){
   if(!allAiEnabled&&slot!=originalAiSlot)continue;
   const bool active=racerBank.active(slot);
   racerBank.visit(slot,[&](){
    const auto& fields=originalAiSession.fields[originalAi.managerIndexFE0];
    std::cerr<<"original_ai slot="<<slot<<" source_index="<<originalAi.managerIndexFE0
        <<" racer60="<<originalAi.racer60<<" vehicle58="<<s.fields.vehicle58
        <<" initialized="<<originalAi.initialized<<" owners_bound="<<s.fields.nativeAiOwnersBound50IV
        <<" active="<<active<<" phase="<<static_cast<unsigned>(s.phase)
        <<" native_ticks="<<completedFrames<<" dispatched="<<originalAi.dispatchedFrames
        <<" state="<<originalAi.state.aiStateFE8<<" CB8="<<lapContinuity.timing.timeCB8
        <<" startup="<<originalAi.startupFrames<<" ordinary="<<originalAi.ordinaryFrames
        <<" combat_scans="<<originalAi.combatScans<<" combat_ai_rejections="<<originalAi.combatAiRejections
        <<" target1008="<<(originalAi.target1008Present?static_cast<std::int64_t>(originalAi.target1008ManagerIndex):-1)
        <<" brake_commands="<<originalAi.brakingFrames<<" bound_brake_steer="<<originalAi.boundBrakeSteerFrames
        <<" steer="<<m.controls.player.raw.values[0]<<" aeroBrakeEC="<<m.components.bindings.aeroBrakeEC
        <<" aeroDrive="<<m.components.bindings.aeroDrive[0]<<'/'<<m.components.bindings.aeroDrive[1]
        <<" target="<<originalAiReceipt.desiredSpeed
        <<" B7C/B80="<<m.controls.player.actionB7C<<'/'<<m.controls.player.actionB80
        <<" B94="<<x.parameters.rates.x<<" C80="<<navigationProgress.reset.vehicleC80
        <<" CC4="<<fields.remainingCC4<<" CCC="<<fields.placeCCC
        <<" FF8="<<originalAi.referenceFF8<<" FFC="<<originalAi.packing.scaleFFC
        <<" 1038/103C="<<originalAi.lists.counts[0]<<'/'<<originalAi.lists.counts[1]
        <<" cooldown109C="<<originalAi.boost.cooldown109C
        <<" route_selections="<<originalAi.navigation.routeSelections
        <<" route_rng_draws="<<originalAi.navigation.routeRandomDraws
        <<" route1080/1084="<<originalAi.navigation.choice1080<<'/'<<originalAi.navigation.choice1084
        <<" source_boundary="<<std::hex<<originalAiReceipt.sourceBoundary<<std::dec<<'\n';
   });
  }
 };
 auto advanceOriginalAiManager=[&](float factor){
  // Declared accepted-world schedule: genuine1AF9C0 clock,1AFF50 racer
  // timing and1B0400 rank. Exact original application root order is unclaimed.
  // beginLapManagerClock mutates shared time, so validate before that call.
  requireOriginalAiFleetReady();
  require50DU(beginLapManagerClock50ID(originalAiSession.manager,factor,false)==LapManagerClockStatus50ID::NeedRacerPass,
      "original AI manager clock boundary");
  for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
   require50DU(!(s.fields.impactFlagsCD8&3u),"original AI first slice reached lap/finish continuation");
   LapTimingInput50ID input{factor,dynamicContactEnabled?dynamicMusicProfile.mode11258:2u,s.fields.vehicle58,static_cast<std::uint32_t>(slot),
       lapContinuity.state.vehicleCC8,s.fields.impactFlagsCD8};
   const auto result=beginRacerLapTiming50ID(lapContinuity.timing,originalAiSession.manager,input,lapContinuity.timingCursor);
   require50DU(result.status==LapTimingStatus50ID::Complete,"original AI timing event boundary");
  });
  rankOriginalAiFleet();
 };
 race_session50HM::Session race{};
 // One cold-owned contact world, outside every racer snapshot. Geometry and
 // scratch never borrow an opponent arena pointer across a bank visit.
 std::unique_ptr<native_racer_contact50HW::Owner> racerContactOwner;
 std::array<std::uint32_t,native_racer_contact50HW::maxRacers> racerContactIds{};
 std::array<std::uint32_t,native_racer_contact50HW::maxRacers> racerContactMaterials{};
 struct RacerContactTotals50HV {
  std::uint64_t passes=0,noPairs=0,testedPairs=0,geometryPairs=0,suppressedPairs=0;
  std::uint64_t respondedPairs=0,generatedRecords=0,activeRecords=0,failures=0;
  native_racer_contact50HW::Status lastStatus=native_racer_contact50HW::Status::Complete;
 } racerContactTotals;
 const auto reportRacerContacts=[&](){
  std::cerr<<"racer_contacts enabled="<<racerContactsEnabled
      <<" passes="<<racerContactTotals.passes<<" no_pairs="<<racerContactTotals.noPairs
      <<" tested_pairs="<<racerContactTotals.testedPairs<<" geometry_pairs="<<racerContactTotals.geometryPairs
      <<" suppressed_pairs="<<racerContactTotals.suppressedPairs<<" responded_pairs="<<racerContactTotals.respondedPairs
      <<" generated_records="<<racerContactTotals.generatedRecords<<" active_records="<<racerContactTotals.activeRecords
      <<" failures="<<racerContactTotals.failures<<" last_status="<<static_cast<unsigned>(racerContactTotals.lastStatus)<<'\n';
 };
 std::array<native_racer50HN::DrawOwner,maxHostRaceOpponents50HT> opponentDraw;
 std::array<std::vector<platform::VehicleWindowMaterialUv50DS>,maxHostRaceOpponents50HT> opponentUvs;
 EngineFlareOutput50GV opponentFlames[maxHostRaceOpponents50HT]{};
 bool opponentFlamePrepared[maxHostRaceOpponents50HT]{};
 // Last accepted render transaction, separate from the full race/standings
 // field. These host presentation counters never change a native racer.
 struct RacePresentation50HZ {
  bool valid=false;
  std::uint32_t submittedOpponents=0,submittedFinishedOpponents=0;
  std::uint32_t retainedStoppedOpponents=0,standingsRows=0;
 } acceptedRacePresentation;
 auto observeLiveRacer=[&](){
  race_session50HM::Observation observed{};
  observed.completedLaps=lapContinuity.state.vehicleCC8;
  observed.checkpointCursor=lapContinuity.state.vehicleFF8;
  observed.previousMapNode=navigationProgress.reset.vehicleC8C;
  observed.currentMapNode=navigationProgress.reset.vehicleC88;
  for(unsigned axis=0;axis<3;++axis)observed.position[axis]=m.components.root.body.position.v[axis];
  observed.wrongWay=(lapContinuity.prefix.flags10CC&2u)!=0;
  observed.lapCallbackPending=lapOwner.pending.pending.requested;
  return observed;
 };
 // Publish trivial engine inputs during the existing race observation visits;
 // no extra full-state bank visits, ownership copies, or per-mix allocations.
 std::array<opponent_audio50HX::Input,opponent_audio50HX::maxOpponents> opponentEngineInputs{};
 std::array<std::uint64_t,maxHostRaceRacers50HT> observedNativeTicks{};
 std::array<std::uint8_t,maxHostRaceRacers50HT> observedNativeFinishFlags{};
 auto sampleRaceField=[&](){
  observedNativeTicks[0]=completedFrames;
  observedNativeFinishFlags[0]=s.fields.impactFlagsCD8;
  require50DU(race_session50HM::sampleRacer(race,0,observeLiveRacer()),"player race observation rejected");
  for(std::size_t racer=1;racer<racerBank.count();++racer){
   // An abandoned partial frame cannot publish progression or display geometry.
   // Keep the last completed observation for a visibly stopped opponent.
   opponentEngineInputs[racer-1]={};
   const auto observed=racerBank.active(racer)?racerBank.visit(racer,[&](){
    observedNativeTicks[racer]=completedFrames;
    observedNativeFinishFlags[racer]=s.fields.impactFlagsCD8;
    if(audioEnabled){
     require50DU(s.phase==VehicleFramePhase49W::Ready,"opponent audio needs a completed native frame");
     auto& input=opponentEngineInputs[racer-1];const auto& p=m.components.root.body.position;
     input.active=true;input.position={p.v[0],p.v[1],p.v[2]};
     input.speedRatio70=boostState.speedRatio70;
     // The opponent boost/damage-dip emitter owners are not reconstructed.
     // Keep explicit undamaged, nonboosted baselines; never borrow player state.
     input.enginePitchScale484=1.f;input.boostPitch=false;
    }
    return observeLiveRacer();
   }):race.racers[racer].live;
   require50DU(race_session50HM::sampleRacer(race,static_cast<std::uint32_t>(racer),observed),
       "opponent race observation rejected");
  }
 };
 auto updateOpponentAudio=[&](){
  if(!audioEnabled||!raceEnabled)return;
  const auto beforeOpponentAudio=::allocations;
  auto inputs=opponentEngineInputs;
  for(std::size_t i=0;i<inputs.size();++i)
   inputs[i].active=inputs[i].active&&racerBank.active(i+1)&&
       race_session50HM::simulationAllowed(race,static_cast<std::uint32_t>(i+1));
  // The listener follows the committed player body, so distance is independent
  // of render cadence, resize, and uncommitted chase-camera preparation.
  const auto& p=m.components.root.body.position;
  const auto receipt=opponentAudio->update({p.v[0],p.v[1],p.v[2]},inputs);
  require50DU(receipt.okay(),"original opponent engine audio world update rejected");
  require50DU(::allocations==beforeOpponentAudio,"opponent engine audio update allocated C++ storage");
 };
 if(raceEnabled){
  std::array<native_racer_contact50HW::Binding,raceRacerCount> contactBindings{};
  contactBindings[0]={racerCollisions[0].get(),settings,response};
  for(std::size_t racer=0;racer<raceRacerCount;++racer){
   racerContactIds[racer]=static_cast<std::uint32_t>(racer+1);
   // Empty physical-root material0 is explicit HOST policy for every racer.
   racerContactMaterials[racer]=native_racer_contact50HW::emptyPhysicalRootMaterial;
  }
#include "native_racer_owners50hn.inc"
  for(auto& membership:gcAnimatedMembership)if(membership){
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  racerBank.seal(raceRacerCount);
  if(originalAiEnabled){
   // Handles bind registered locations only. Actual values are read afresh
   // after every native step; no per-world peer cache or arena swap is used.
   racerFrameReads.bind(racerBank,s);
   originalAiReads.bind(racerBank,x.bindings,navigationProgress,
       lapContinuity.state.vehicleCC8,lapContinuity.timing.wordCD4,
       originalAi,originalAiReceipt,x.diagnostic);
  }
  // Alternate camera selections and every all-AI slot use their actual
  // original-resource cold path before any tick. Only the legacy mixed POD01
  // selection retains the fixture startup. No live identity is flipped.
  for(std::size_t racer=playerUsesRosterRig?0u:1u;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
   const auto& rig=*racerRigs[racer];const auto& collision=*racerCollisions[racer];
   coldRacerStage50DU(rig.podId,"rig installation",[&](){
    require50DU(completedFrames==0&&m.frameFactor==0.f,"original racer installation requires fresh state");
    rig.install(s,x.bindings,x.handRecords);
   });
   auto racerSelection=selection;
   if(originalAiEnabled&&(allAiEnabled||racer==originalAiSlot))racerSelection.mode58Nonzero=true;
   const auto receipt=coldRacerStage50DU(rig.podId,"handling bind",[&](){
    return native_racer50HW::bindColdHandling(rig.phy,rig.hierarchy[1],
        rig.hierarchy[0],rig.geometry,collision.mainTarget().source,racerSelection,x,
        {boostParameters,boostCoolingCF8,settings,response,secondarySettings,secondaryResponse});
   });
   if(originalAiEnabled&&(allAiEnabled||racer==originalAiSlot)){
    rr::assets::ResArchive48C source;rr::assets::DecodeError48C error;std::vector<std::uint8_t> profileText;
    const auto sourceBytes=runtime_startup50fl::readBytes(rig.resourceFile);
    std::string lowerPod=rig.podId;for(char&c:lowerPod)c=static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    const std::string member="Pods\\"+lowerPod+"\\"+lowerPod+".ai";
    require50DU(rr::assets::decodeResource48C(sourceBytes,source,error)&&
        rr::assets::extractResMember48C(source,member.c_str(),profileText,error),"selected original racer .ai member unavailable");
    AiProfile50IU profile{};
    require50DU(rr::assets::readAiProfile50IU(reinterpret_cast<const char*>(profileText.data()),profileText.size(),
        profile,originalAiAuthoringDiagnostic),"selected original AI profile rejected");
    // Original GC track ID10 ->187B10 result10 ->16E5B8 class3. This is
    // independent of the PHY filename/class exceptions used by49N.
    AiGcCreateSelection50IU createSelection{};
    require50DU(bindFreshGcAiCreateSelection50IU(trackSession->world->code,2,createSelection)&&
        applyAiCreateClass50IU(createSelection.actualRaceClass,profile),"original GC AI create class unavailable");
    const auto* identity=original_racer_selection50IJ::findByPodId(rig.podId.c_str());
    require50DU(identity,"original AI racer60 identity unavailable");
    AiCreateIdentity50IU createIdentity{};
    require50DU(bindAiCreateIdentity50IU(static_cast<std::uint32_t>(racer-originalAiHumanCount),originalAiHumanCount,
        static_cast<std::int32_t>(identity->racer60),createSelection.freshCourseRecordPod11280,createIdentity),
        "original AI create identity unavailable");
    initializeAiRuntime50IV(createIdentity.ordinalFE0,identity->racer60,
        static_cast<std::uint32_t>(createSelection.freshCourseRecordPod11280),
        profile,receipt.authored.engineDifference5C,originalAi);
    originalAiSession.packingReferences[racer]=-1;
    AiColdTrackBindings50IU tracks{};
    require50DU(bindAiColdTracks50IU(*navigationMap,*navigationAI,originalAiMapMetadata,
        originalAiMetadata,navigationProgress,tracks),"original AI cold track owners unavailable");
    s.fields.vehicle58=1u;s.fields.nativeAiOwnersBound50IV=true;s.fields.applicationByte58=0u;
    x.parameters.rates.x=4.f;
    initializeLapTimingFields50ID(lapContinuity.timing,dynamicContactEnabled?dynamicMusicProfile.mode11258:2u);lapContinuity.timingInitialized=true;
   }
   coldRacerStage50DU(rig.podId,"handling root placement",[&](){
    native_racer50HW::placeColdHandlingRoots(x);
   });
   coldRacerStage50DU(rig.podId,"model rebase",[&](){
    for(unsigned component=0;component<3;++component)rebaseModels[component]=rig.rebaseModels[component];
    require50DU(rebaseColdPodScene50FB({s.nodes,s.sceneNodeCount},rebaseModels,
        rebaseWorkspace,rebaseBounds,rebaseDiagnostic),"original racer cold model rebase failed");
   });
   coldRacerStage50DU(rig.podId,"effect name binding",[&](){rig.buildEffectNames(s,namesForEffects);});
   query.resource=&collision.main();secondaryQuery.resource=&collision.secondary();
   terrain_probe_authoring50fq_fixture::Owner ownProbe{};std::string probeError;
   coldRacerStage50DU(rig.podId,"terrain probe authoring",[&](){
    const bool probeReady=terrain_probe_authoring50fq_fixture::prepare(ownProbe,query,x.parameters,
        rig.phy,terrainBindings,probeError,collision);
    require50DU(probeReady,probeError.empty()?"original racer probe authoring failed":probeError.c_str());
   });
   for(unsigned axis=0;axis<3;++axis)x.parameters.bodies.main.probeLocal140[axis]=ownProbe.lowerEndpoint140.v[axis];
   x.parameters.bodies.main.probeLocal140[3]=1.f;
   coldRacerStage50DU(rig.podId,"terrain probe installation",[&](){
    const bool probeInstalled=terrain_probe_authoring50fq_fixture::installProbeCold(terrainOwner,
        terrainLiveOwner,ownProbe,probeError);
    require50DU(probeInstalled,probeError.empty()?"original racer cold probe installation failed":probeError.c_str());
   });
   // This slot has never advanced. Replace inherited enabled-probe construction
   // with captured pre-enable construction, then use its own native constructor.
   s.terrainProbe=freshTerrainProbe;s.bodies.main.components.probe348=freshMainProbe;
   s.bodies.main.domain.probe100=freshProbe100;
   coldRacerStage50DU(rig.podId,"terrain probe enable",[&](){
    require50DU(enableVehicleTerrain50FP(s,x.bindings,terrainOwner.initial,
        terrainOwner.follow,x.workspace,x.diagnostic),"enable original racer terrain probe");
   });
   coldRacerStage50DU(rig.podId,"visual construction",[&](){
    racerVisuals[racer]=std::make_unique<native_racer50HW::Visual>(".",
        "evidence/originalRoster50HW/original-eight-visuals",rig);
   });
   coldRacerStage50DU(rig.podId,"cable construction",[&](){
    racerCables[racer]=std::make_unique<native_racer50HW::Cables>(rig,s,x.bindings);
   });
   coldRacerStage50DU(rig.podId,"flame construction",[&](){
    coldFlame=native_racer50HW::buildColdFlames(rig,racerVisuals[racer]->drawAssets,x.bindings);
   });
   racerNames[racer]=racerVisuals[racer]->displayName;
   contactBindings[racer]={&collision,settings,response};
   if(!racer){
    // Human finish controls retain this racer's selected tuning as well.
    authored=receipt.authored;
    authoredFinishSteering10A4=x.parameters.bodies.main.aero.steering100;
    // Recovery's cold scene carries the human lift admission. Terrain owners
    // and mesh data remain unchanged; opponent grid queries pass their own lift.
    ground.lift=x.parameters.visual.verticalLimit7EC;
   }
   std::cerr<<"ORIGINAL RACER "<<racer<<": "<<racerNames[racer]<<" ["<<rig.podId
       <<"] nodes="<<s.sceneNodeCount<<" mass="<<x.parameters.bodies.main.body.mass
       <<" lift="<<receipt.liftHeight<<" main_rays="<<collision.main().rayCount
       <<" secondary_rays="<<collision.secondary().rayCount
       <<" track="<<authoredTrackName<<" class="<<static_cast<unsigned>(receipt.authored.selectedClass)
       <<" class_values="<<receipt.authored.usedAuthoredClassValues
       <<" max_thrust="<<x.parameters.bodies.main.engine.maxThrust
       <<" top_speed_body="<<x.parameters.bodies.main.engine.theoreticalTopSpeed
       <<" steering100="<<x.parameters.bodies.main.aero.steering100
       <<" repair="<<s.fields.repairCF4<<" cooling="<<boostCoolingCF8<<'\n';
  });
  // Keep exact original names in fixed cold storage for allocation-free HUD publication.
  for(std::size_t racer=0;racer<raceRacerCount;++racer){
   require50DU(!racerNames[racer].empty()&&racerNames[racer].size()<=platform::maxVehicleWindowRacerName50DS,
       "original racer display name exceeds fixed HUD capacity");
   std::memcpy(racerHudNames[racer].data(),racerNames[racer].c_str(),racerNames[racer].size()+1);
  }
#include "native_racer_grid50hn.inc"
  if(playerUsesRosterRig)coldRacerStage50DU(playerPodId,"human grounded grid placement",[&](){
   placeOpponentAtGrid50HN(0);
   require50DU(query.resource==&racerCollisions[0]->main()&&
       secondaryQuery.resource==&racerCollisions[0]->secondary()&&
       mainMembership.lastType2Resource==query.resource&&
       secondaryMembership.lastType2Resource==secondaryQuery.resource,
       "selected human retains another racer's static contact resource");
  });
  for(std::size_t racer=1;racer<racerBank.count();++racer)
   coldRacerStage50DU(racerRigs[racer]->podId,"grounded grid placement",[&](){placeOpponentAtGrid50HN(racer);});
  if(racerContactsEnabled){
   racerContactOwner=std::make_unique<native_racer_contact50HW::Owner>();
   std::string error;
   const bool contactsReady=racerContactOwner->prepare(contactBindings.data(),contactBindings.size(),error);
   require50DU(contactsReady,error.empty()?"original racer contact world preparation failed":error.c_str());
  }
  for(std::size_t racer=1;racer<racerBank.count();++racer){
   const auto index=racer-1;auto& visual=*racerVisuals[racer];
   coldRacerStage50DU(racerRigs[racer]->podId,"draw storage preparation",[&](){
    VehicleDrawBatchInput50CO source{};
    require50DU(visual.drawAssets.batchInput(0,ModelPalettePhase49S::selectedRenderPreparation,
        ModelPaletteSceneState49S::completeCurrentScene,source),"original opponent draw descriptors unavailable");
    require50DU(opponentDraw[index].initialize(source),"original opponent draw storage initialization failed");
    for(unsigned component=1;component<3;++component)
     for(const auto material:visual.flameMaterialIndices(component)){
      platform::VehicleWindowMaterialUv50DS uv{};
      uv.component=static_cast<VehicleDrawComponent50CM>(component);uv.materialIndex=material;
      opponentUvs[index].push_back(uv);
     }
   });
  }
  if(dynamicContactEnabled){
   std::array<native_dynamic_contact50IW::ManagerBinding,raceRacerCount> bindings{};
   for(std::size_t slot=0;slot<raceRacerCount;++slot){
    bindings[slot]={slot,{racerContactIds[slot]}};
    // Default camera POD01 may use the retained fixture in the mixed path;
    // selectedOriginal is already authenticated before any cold installation.
    require50DU(slot==0||racerRigs[slot]!=nullptr,"actual contact actor rig unavailable");
    const auto* selection=slot==0?selectedOriginal:
        original_racer_selection50IJ::findByPodId(racerRigs[slot]->podId.c_str());
    require50DU(selection,"actual contact actor original racer60 unavailable");
    dynamicRacer60[slot]=selection->racer60;
   }
   std::string error;
   require50DU(dynamicRegistry.prepare(bindings.data(),bindings.size(),originalAiHumanCount,error),
       error.empty()?"original dynamic manager registry unavailable":error.c_str());
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    require50DU(s.phase==VehicleFramePhase49W::Ready&&m.components.body20C==0&&
        s.impactSupplement.partnerId==0xffffffffu&&s.impactSupplement.partnerState==0,
        "dynamic owners require original fresh body/D34 state");
    s.fields.dynamicOwnersBound50IW=true;
   });
  }
  // One neutral, unscored native setup tick establishes actual graphics/ground
  // before countdown rendering. After this all physics freezes until GO.
  const platform::WindowInput50DQ noKeys{};
  raceSetupFrame=true;
  if(originalAiEnabled)for(std::size_t slot=0;slot<racerBank.count();++slot)
   racerBank.visit(slot,[&](){s.fields.applicationByte58=originalAiSession.manager.flags58;});
  auto initializeGraphics=[&](){
   if(s.fields.vehicle58==0u){composePacket(false,noKeys);sampleController();}
   require50DU(advanceFrame(),"native race grid setup tick rejected");
   require50DU(lapContinuity.state.vehicleCC8==0,"cold race setup unexpectedly completed a lap");
  };
  for(std::size_t racer=1;racer<racerBank.count();++racer){
   opponentFrame=true;
   try{
    captureDynamicBeforeFrame(racer);
    if(originalAiEnabled)observeOriginalAiFleet();
    racerBank.visit(racer,initializeGraphics);
   }
   catch(const std::exception& error){
    opponentFrame=false;
    std::throw_with_nested(std::runtime_error("Original racer "+racerRigs[racer]->podId+
        " [initial native graphics tick, "+racerNames[racer]+"]: "+error.what()));
   }
   opponentFrame=false;
  }
  captureDynamicBeforeFrame(0);
  if(originalAiEnabled)observeOriginalAiFleet();
  initializeGraphics();raceSetupFrame=false;
  if(originalAiEnabled){
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
    initializeLapTimingFields50ID(lapContinuity.timing,dynamicContactEnabled?dynamicMusicProfile.mode11258:2u);lapContinuity.timingInitialized=true;
   });
   rankOriginalAiFleet();
  }
  require50DU(race_session50HM::initialize(race,*navigationMap,lapOwner.track,raceRacerCount),
      "authored race route/markers rejected");
  std::uint32_t gridOrder[maxHostRaceRacers50HT]{};
  for(std::uint32_t index=0;index<raceOpponentCount;++index)gridOrder[index]=index+1;
  gridOrder[raceOpponentCount]=0;
  require50DU(race_session50HM::seedGridOrder(race,gridOrder,raceRacerCount),
      "explicit host starting-grid order rejected");
  sampleRaceField();require50DU(race_session50HM::commitFrame(race,0),"race grid observations rejected");
  if(mode1Enabled){
   mode1=std::make_unique<race_mode1_host50ID::Owner>(racerBank,lapOwner,s.fields,race);
   std::array<race_mode1_host50ID::RosterEntry,raceRacerCount> roles{};
   roles[0]={playerRacer60,0,true,true}; // Authenticated selected identity, one human.
   // Bot IDs are not synthesized. Category6 sees HOST AI role and returns
   // before identity-dependent speech; arena vehicle58 remains unchanged.
   require50DU(mode1->initialize(roles.data(),roles.size()),"cold mode1 lap/record/FIFO binding failed");
  }
  std::cerr<<"RACE MODE "<<(mode1Enabled?1:2)
      <<(mode1Enabled?": source lap/records/event FIFO; explicit HOST world/rank/finish and distance projection.\n":
         ": existing HOST observer/audio fallback explicitly selected.\n");
  std::cerr<<"HOST RACE: "<<raceRacerCount<<" distinct original racers, 3 laps, 3-second countdown. "
      "Field count matches the original normal race; the grounded starting grid and opponent controls are host-authored. "
      "Original AI remains unjoined. "
      "Each racer owns "<<racerBank.bytesPerRacer()<<" bytes of native state.\n";
  std::cerr<<"NATIVE FINISH CONTINUATION: source CD8 finish state retains native physics, "
      "engine audio, all original racer draws and sealed named standings. "
      "Original player finish controls retain the selected twin-engine config; "
      "RESULTS after the normal four-second countdown retains the live scene "
      "as HOST presentation until restart/quit.\n";
  std::cerr<<"RACER CONTACTS: "<<(racerContactsEnabled?"enabled":"disabled")
      <<"; each original main-body geometry and authored response, host world-frame scheduling. "
        "Empty-root material0 is a deterministic host policy; original padding parity is unclaimed. "
        "Secondary dynamic collision, damage attribution and opponent speech/collision/boost-event audio remain unjoined.\n";
  if(dynamicContactEnabled)std::cerr<<"ORIGINAL AI DYNAMIC CONTACTS: ordered body20C/210 and live D34 peers, "
      "source severity/common maxima, authentic profile11258=1 music, shared event1/event5 FIFO. "
      "Terminal boundaries retain missing vehicle5C/controller feedback, opponent D48, collision announcer and music fade. "
      "Native serial world timing is explicit; no full original scheduler claim.\n";
 }

 // Cold-selected player publications borrow stable owners through shutdown.
 // POD01 fixture resources never supply another original racer's draw or cable.
 auto& playerDrawAssets=playerUsesRosterRig?racerVisuals[0]->drawAssets:drawAssets;
 const auto playerCableBindings=playerUsesRosterRig?racerCables[0]->bindings():cableBindings;
 std::vector<platform::VehicleWindowMaterialUv50DS> playerFlameUvs;
 for(unsigned component=1;component<3;++component){
  if(playerUsesRosterRig){
   for(const auto material:racerVisuals[0]->flameMaterialIndices(component)){
    platform::VehicleWindowMaterialUv50DS uv{};
    uv.component=static_cast<VehicleDrawComponent50CM>(component);uv.materialIndex=material;
    playerFlameUvs.push_back(uv);
   }
  }else{
   platform::VehicleWindowMaterialUv50DS uv{};
   uv.component=static_cast<VehicleDrawComponent50CM>(component);uv.materialIndex=1;
   playerFlameUvs.push_back(uv);
  }
 }

 // HOST audio/world scheduling: once after each actual common16ms mix,
 // including countdown. Never rendered-frame, paused or per-racer updates.
 auto advanceSourceAudioWorld=[&](std::uint64_t beforeMix){
  if(!mode1Enabled&&!dynamicContactEnabled)return;
  // Dynamic update performs its own source activity read after the timer.
  // Legacy category6-only mode keeps the existing explicit completion poll.
  if(!dynamicContactEnabled)require50DU(raceMusic->pollCompletion(),
      "mode1 music transport ownership failed after shared mix");
  const auto afterMix=liveAudio->mixedFrames();
  require50DU(afterMix>=beforeMix&&(afterMix-beforeMix==0u||afterMix-beforeMix==768u),
      "mode1 shared mix must advance one actual768-frame step or none");
  if(afterMix==beforeMix)return;
  // Output may fail AFTER the actual mix. Source completion/update still runs
  // exactly once for those consumed frames before surfacing the output error.
  if(dynamicContactEnabled){
   const auto musicUpdate=raceMusic->update(.016f);
   if(!musicUpdate.completed())dynamicBoundary(musicUpdate.sourcePC,
       "original collision music fade/transport helper pending");
  }
  AnnouncerLeader50IE leader{};
  if(dynamicContactEnabled){
   require50DU(originalAiSession.rankBound,"dynamic audio needs actual original race rank");
   for(std::size_t slot=0;slot<racerBank.count();++slot)
    if(originalAiSession.rank[slot].placeCCC==1){
     require50DU(!leader.exists,"original manager leader is not unique");
     leader.exists=true;leader.remainingDistance34=originalAiSession.rank[slot].remainingCC4;
     racerBank.visit(slot,[&](){leader.vehicle58=s.fields.vehicle58;leader.cachedSpeed34=m.cachedSpeed34;});
    }
   require50DU(leader.exists,"original manager leader is unavailable");
  }else if(race.racerCount){
   const auto slot=race.ordered[0];leader.exists=true;
   leader.vehicle58=slot==0?0u:1u; // HOST role, not arena player-physics gate.
   leader.remainingDistance34=static_cast<float>(race.racers[slot].remainingDistance);
   leader.cachedSpeed34=racerBank.visit(slot,[&](){return m.cachedSpeed34;});
   leader.hostRemainingProjection=true;
  }
  // Exact source remaining/speed<10 priority formula; remaining distance is
  // HOST interpolation, not original cross-section or ranking-tie arithmetic.
  require50DU(sourceAnnouncer->update(.016f,leader),"mode1 shared announcer update unresolved");
 };

 // Actual selected visual mesh, authenticated by its selected source profile.
 // Visual and collision member closures remain separate admissions.
 rr::assets::Mesh48B trackMesh;std::string trackLoadError;
 const bool trackLoaded=rr::assets::loadMesh48B(
     trackSession->visualMesh,trackMesh,trackLoadError);
 require50DU(trackLoaded,trackLoadError.empty()?"selected-track visual mesh load failed":trackLoadError.c_str());
 namespace trackTextures=track_materials50fa_fixture;
 auto trackMaterialOwner=std::make_unique<trackTextures::Owner>();
 const auto trackSource=trackSession->originalStaticCandidates?
     track_source_fixture::Track::gc:track_source_fixture::Track::ta;
 trackTextures::Request trackMaterialRequest{};
 trackMaterialRequest.track=trackSource;
 trackMaterialRequest.root=trackSession->visualRoot;
 trackMaterialRequest.exportReport=trackSession->visualReport;
 trackMaterialRequest.choice={1,1,trackSession->originalStaticCandidates?rr::assets::MaterialTextureRequirement50EA::required:
     rr::assets::MaterialTextureRequirement50EA::optionalUntextured,
     trackTextures::MipMode::largestRetained,0};
 trackMaterialRequest.coldSourceByteCapacity=32u*1024u*1024u;
 trackMaterialRequest.sourceArchive=trackSession->archive;
 trackMaterialRequest.decodeSourceTexture=trackSession->originalStaticCandidates?
     &source_texture50fc_fixture::decodeSourceTextureGc:&source_texture50fc_fixture::decodeSourceTexture;
 trackMaterialRequest.includeAllAuthoredPasses=trackSession->originalStaticCandidates;
 trackMaterialRequest.allowStaticAnimationFrame=true;
 trackMaterialRequest.textureBudgets={trackSession->originalStaticCandidates?64u:32u,1024,8u*1024u*1024u,8u*1024u*1024u,1024};
 trackTextures::Diagnostic trackMaterialDiagnostic{};
 require50DU(trackMaterialOwner->load(trackMaterialRequest,trackMesh,trackMaterialDiagnostic),
     trackMaterialDiagnostic.message.empty()?"selected-track material load failed":trackMaterialDiagnostic.message.c_str());
 std::vector<rr::assets::WorldNormal50HK> trackNormals;
 track_normals50hk_fixture::Diagnostic trackNormalDiagnostic{};
 require50DU(track_normals50hk_fixture::load(trackMaterialRequest.root,
     trackMaterialRequest.exportReport,trackMesh,trackNormals,trackNormalDiagnostic,trackSource),
     trackNormalDiagnostic.message.empty()?"selected-track authored normal load failed":trackNormalDiagnostic.message.c_str());
 std::cerr<<trackSession->world->code<<" source normals "<<trackNormalDiagnostic.normals
     <<" zero/fallback "<<trackNormalDiagnostic.zeroNormals<<'\n';
 auto skyOwner=std::make_unique<sky50gw_fixture::Owner>();
 sky50gw_fixture::Request skyRequest{};skyRequest.track=trackSource;
 skyRequest.archive=trackSession->archive;
 skyRequest.textureRoot=trackSession->skyTextureRoot;
 sky50gw_fixture::Diagnostic skyDiagnostic{};
 require50DU(skyOwner->load(skyRequest,skyDiagnostic),
     skyDiagnostic.message.empty()?"actual selected-track sky load failed":skyDiagnostic.message.c_str());
 // Host is declared first so renderer destruction always precedes its context.
 platform::GameHost50ER host;
 gc_dynamic_window50IG::Owner gcDynamicWindow;
 gc_atm_window_fixture::Owner gcAtm;
 if(gcAnimatedWorld){
  gc_dynamic_materials_fixture::Diagnostic materialDiagnostic{};
  // Finish each cold call before reading its diagnostic: function argument
  // evaluation order must not select the fallback from a still-empty message.
  const bool gcMaterialsLoaded=gcDynamicWindow.loadSource(trackSession->archive,*gcAnimatedWorld,materialDiagnostic);
  if(!gcMaterialsLoaded)std::cerr<<"GC dynamic material load diagnostic material="<<materialDiagnostic.material
      <<" source_code="<<materialDiagnostic.source.code<<" source_offset="<<materialDiagnostic.source.offset
      <<" source_detail="<<materialDiagnostic.source.detail
      <<" bank_error="<<static_cast<unsigned>(materialDiagnostic.bank.error)
      <<" bank_texture="<<materialDiagnostic.bank.texture<<" bank_slot="<<materialDiagnostic.bank.slot
      <<" bank_offset="<<materialDiagnostic.bank.byteOffset<<" bank_detail="<<materialDiagnostic.bank.parserMessage<<'\n';
  require50DU(gcMaterialsLoaded,
      materialDiagnostic.message.empty()?"original GC dynamic materials rejected":materialDiagnostic.message.c_str());
  gc_atm_animation_fixture::Diagnostic atmDiagnostic{};
  const bool gcAtmLoaded=gcAtm.loadSource(trackSession->archive,*trackMaterialOwner,atmDiagnostic);
  if(!gcAtmLoaded)std::cerr<<"GC ATM load diagnostic animation="<<atmDiagnostic.animation<<" frame="<<atmDiagnostic.frame
      <<" source_code="<<atmDiagnostic.source.code<<" source_offset="<<atmDiagnostic.source.offset
      <<" source_detail="<<atmDiagnostic.source.detail
      <<" bank_error="<<static_cast<unsigned>(atmDiagnostic.bank.error)
      <<" bank_texture="<<atmDiagnostic.bank.texture<<" bank_slot="<<atmDiagnostic.bank.slot
      <<" bank_offset="<<atmDiagnostic.bank.byteOffset<<" bank_detail="<<atmDiagnostic.bank.parserMessage<<'\n';
  require50DU(gcAtmLoaded,
      atmDiagnostic.message.empty()?"original GC ATM source bank rejected":atmDiagnostic.message.c_str());
 }
 std::uint32_t sourceApplicationTimestamp=0;
 std::uint64_t gcAnimatedWorldSteps=0;
 auto advanceWorldAnimations=[&](){
  if(!gcAnimatedWorld)return;
  // Source1AAD98 registers the complete racer group before1B4A58's animation
  // service. Current HOST participant order stays explicit; world service runs
  // once after that group, including enabled countdown world traversal.
  const auto before=::allocations;
  require50DU(sampleHostTimestamp50ID(applicationClock,sourceApplicationTimestamp),"shared source application timestamp rejected");
  require50DU(gcAnimatedWorld->service(.016f,sourceApplicationTimestamp,s.noise.sharedRandomState,
      gcAnimationLazy,s.animationShared.motion,gcAnimationDiagnostic),
      gcAnimationDiagnostic.message.empty()?"original GC world animation rejected":gcAnimationDiagnostic.message.c_str());
  if(gcAtm.loaded())require50DU(gcAtm.advance(sourceApplicationTimestamp),"original GC ATM time update rejected");
  require50DU(::allocations==before,"successful GC world animation allocated C++ storage");
  ++gcAnimatedWorldSteps;
 };
 platform::MaterialTextureGl50EB gpuTextures,trackGpuTextures,cableGpuTextures,skyGpuTextures;
 std::array<platform::MaterialTextureGl50EB,raceOpponentCount> opponentGpuTextures,opponentCableGpuTextures;
 platform::MaterialTextureGlDiagnostic50EB textureDiagnostic{};
 platform::VehicleWindowGl50DS renderer;
 platform::GameHostDiagnostic50ER hostDiagnostic{};
 platform::VehicleWindowDiagnostic50DS rendererDiagnostic{},completionDiagnostic{};
 bool restartRequested=false;
 auto closeWindow=[&](){
  bool clean=true;
  if(!sourceAnnouncer->reset())clean=false;raceMusic->reset();
  raceAudio->reset(); // Stop shared Dax and restore groups before mixer closure.
  opponentAudio->close(); // Release only96..109 before the live mixer closes.
  if(!liveAudio->close(audioDiagnostic)){
   std::cerr<<"audio close failed "<<static_cast<unsigned>(audioDiagnostic.error)<<'\n';clean=false;
  }
  if(renderer.ownsResources()&&!renderer.close(rendererDiagnostic)){
   reportRenderer50DU(rendererDiagnostic);clean=false;
  }
  // Never destroy a context beneath unresolved GPU ownership. Destructors may
  // retry cleanup in the same correct order; neither silently leaks resources.
  if(renderer.ownsResources())return false;
  if(gcDynamicWindow.ownsResources()&&!gcDynamicWindow.close(textureDiagnostic)){
   std::cerr<<"GC dynamic texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;
  }
  if(gcDynamicWindow.ownsResources())return false;
  if(gcAtm.ownsResources()&&!gcAtm.close(textureDiagnostic)){
   std::cerr<<"GC ATM texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;
  }
  if(gcAtm.ownsResources())return false;
  for(std::size_t index=0;index<raceOpponentCount;++index){
   for(auto* texture:{&opponentGpuTextures[index],&opponentCableGpuTextures[index]}){
    if(texture->ownsResources()&&!texture->close(textureDiagnostic)){
     std::cerr<<"original racer texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;
    }
    if(texture->ownsResources())return false;
   }
  }
  if(gpuTextures.ownsResources()&&!gpuTextures.close(textureDiagnostic)){
   std::cerr<<"texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;
  }
  if(gpuTextures.ownsResources())return false;
  if(trackGpuTextures.ownsResources()&&!trackGpuTextures.close(textureDiagnostic)){
   std::cerr<<"track texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;
  }
  if(trackGpuTextures.ownsResources())return false;
  if(cableGpuTextures.ownsResources()&&!cableGpuTextures.close(textureDiagnostic)){
   std::cerr<<"cable texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;
  }
  if(cableGpuTextures.ownsResources())return false;
  if(skyGpuTextures.ownsResources()&&!skyGpuTextures.close(textureDiagnostic)){
   std::cerr<<"sky texture cleanup error "<<static_cast<unsigned>(textureDiagnostic.error)<<'\n';clean=false;
  }
  if(skyGpuTextures.ownsResources())return false;
  if(host.ownsResources()&&!host.close(hostDiagnostic)){
   reportHost50DU(hostDiagnostic);clean=false;
  }
  return clean;
 };
 try {
  platform::GameHostRequest50ER hostRequest{};
  hostRequest.mode=headlessLive?platform::GameHostMode50ER::HeadlessLive:platform::GameHostMode50ER::Window;
  auto& windowRequest=hostRequest.window;
  windowRequest.title=raceEnabled?"Racer Revenge | Native race | W/S + Up/Down | F5 restart | Esc menu":
      "Racer Revenge native development drive | W/S + Up/Down | F5 restart | Esc menu";
  windowRequest.width=640;windowRequest.height=480;
  windowRequest.major=3;windowRequest.minor=2;
  windowRequest.profile=platform::WindowGlProfile50DQ::Core;
  windowRequest.samples=0;windowRequest.swapInterval=0;windowRequest.resizable=true;
  hostRequest.headless={platform::OffscreenGlPlatform50DC::MesaSurfaceless,640,480,3,2,platform::OffscreenGlProfile50DC::Core,8,8,8,8,24,0,0};
  if(!host.create(hostRequest,hostDiagnostic)){
   reportHost50DU(hostDiagnostic);throw std::runtime_error("window/context creation failed");
  }
  if(audioEnabled){
   const char* audioWav=std::getenv("RR_AUDIO_WAV");
   std::string sessionAudioPath;
   if(audioWav&&sessionIndex){
    const std::filesystem::path base(audioWav);
    sessionAudioPath=(base.parent_path()/(base.stem().string()+"-session-"+
        std::to_string(sessionIndex+1)+base.extension().string())).string();
    audioWav=sessionAudioPath.c_str();
   }
   if(liveAudio->bank().ready){
    platform::AudioOutputRequest50HD audioRequest{};
    audioRequest.wavPath=audioWav;audioRequest.tryDevice=!headlessLive;
    const char* deviceSetting=std::getenv("RR_AUDIO_DEVICE");
    if(deviceSetting)audioRequest.tryDevice=std::string(deviceSetting)=="1";
    if(!liveAudio->openOutput(audioRequest,audioDiagnostic)){
     std::cerr<<"Audio output open error="<<static_cast<unsigned>(audioDiagnostic.error)
         <<" detail="<<audioDiagnostic.detail.data()<<'\n';
     require50DU(false,"native audio output open failed");
    }
    const auto audioInfo=liveAudio->info();
    std::cerr<<"Audio output device="<<platform::audioDeviceStatusName50HD(audioInfo.deviceStatus)
        <<" capture="<<audioInfo.captureActive<<" rate="<<audioInfo.sampleRate
        <<" detail="<<audioInfo.deviceDetail.data()<<'\n';
    if(audioWav)std::cerr<<"Audio recording session="<<sessionIndex+1<<" path="<<audioWav<<'\n';
   }else{
    require50DU(!audioWav,"original audio samples missing; refusing a silent proof recording");
    std::cerr<<"Audio unavailable: no original samples decoded; no device or WAV opened.\n";
   }
  }
  if(mode1Enabled||dynamicContactEnabled)require50DU(liveAudio->outputOpen(),"source events need the real shared output backend");
  if(audioEnabled&&raceEnabled){
   require50DU(raceAudio->observe(race).accepted,"initial race audio observation rejected");
   std::cerr<<"Race audio ready/missing/unsupported/invalid "<<raceAudio->bank().ready<<'/'
       <<raceAudio->bank().missing<<'/'<<raceAudio->bank().unsupported<<'/'
       <<raceAudio->bank().invalid<<" of "<<raceAudio->bank().clips.size()<<'\n';
  }
  using GlStringFunction=const unsigned char*(*)(unsigned int);
  const auto glString=reinterpret_cast<GlStringFunction>(host.procAddress("glGetString"));
  if(glString){
   for(const auto field:{0x1f00u,0x1f01u,0x1f02u}){
    const auto* value=glString(field);
    std::cerr<<"GL "<<field<<": "<<(value?reinterpret_cast<const char*>(value):"unavailable")<<'\n';
   }
  }
  constexpr std::int32_t maximumWidth=1280,maximumHeight=720;
  platform::VehicleWindowRequest50DS renderRequest{};
  renderRequest.context=&host;
  renderRequest.procAddress=[](void* context,const char* name) noexcept -> platform::VehicleWindowGlProc50DS {
   return static_cast<platform::GameHost50ER*>(context)->procAddress(name);
  };
  renderRequest.isCurrent=[](void* context) noexcept {
   return static_cast<platform::GameHost50ER*>(context)->current();
  };
  renderRequest.maximumWidth=maximumWidth;renderRequest.maximumHeight=maximumHeight;
  renderRequest.enableHostHud=true;
  const char* disableNativeShadows=std::getenv("RR_DISABLE_NATIVE_SHADOWS");
  renderRequest.enableNativePodShadows=!(disableNativeShadows&&std::string(disableNativeShadows)=="1");
  renderRequest.output=headlessLive?platform::VehicleWindowOutput50DS::headless:platform::VehicleWindowOutput50DS::defaultBackBuffer;
  // Explicit diagnostic material policy: authored pass1/slot1, largest retained
  // mip, linear filtering and repeat. This does not reproduce original shaders.
  platform::MaterialTextureGlRequest50EB textureRequest{};
  textureRequest.context=&host;textureRequest.isCurrent=renderRequest.isCurrent;
  const char* missingTextureProc=nullptr;
  require50DU(platform::loadMaterialTextureFunctions50EE(&host,renderRequest.procAddress,
      textureRequest.gl,missingTextureProc),"required texture GL entry point unavailable");
  std::vector<platform::MaterialTextureSampler50EB> samplers;
  std::vector<platform::VehicleWindowMaterialBinding50DS> materialBindings[3];
  if(playerUsesRosterRig){
   const auto& visual=*racerVisuals[0];samplers=visual.samplers();
   auto playerTextureRequest=textureRequest;
   playerTextureRequest.samplers=samplers.data();
   playerTextureRequest.samplerCount=static_cast<std::uint32_t>(samplers.size());
   playerTextureRequest.maximumGpuBytes=8u*1024u*1024u;
   require50DU(gpuTextures.create(playerTextureRequest,visual.materials,textureDiagnostic),
       "selected original player texture upload failed");
   renderRequest.materials=visual.windowMaterials(&gpuTextures);
  }else{
  const auto* textureBank=materialOwner->bank();require50DU(textureBank,"cold texture bank missing");
  samplers.assign(textureBank->textureCount(),
      {platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat});
  // Both actual Flame PSMs specify vertical clamp. Repeating V wraps the
  // bright tail row into the nearly-zero tip UV, producing a detached ring.
  for(unsigned component=1;component<3;++component){
   const auto* selected=materialOwner->component(component);
   require50DU(selected,"engine sampler material selection missing");
   for(const auto& entry:*selected)if(entry.materialIndex==1){
    require50DU(entry.textureIndex<samplers.size(),"flame sampler bank index");
    samplers[entry.textureIndex].t=platform::MaterialTextureWrap50EB::clampToEdge;
   }
  }
  textureRequest.samplers=samplers.data();textureRequest.samplerCount=static_cast<std::uint32_t>(samplers.size());
  textureRequest.maximumGpuBytes=1024u*1024u;
  require50DU(gpuTextures.create(textureRequest,*textureBank,textureDiagnostic),"actual Pod texture upload failed");
  for(unsigned component=0;component<3;++component){
   const auto* selected=materialOwner->component(component);require50DU(selected,"dense material selection missing");
   for(const auto& entry:*selected){
    platform::VehicleWindowMaterialBinding50DS binding{
        platform::VehicleWindowMaterialMode50DS::requiredTexture,entry.textureIndex};
    if(component>0&&entry.materialIndex==1){
     auto name=entry.materialName;
     for(char& c:name)if(c>='A'&&c<='Z')c=static_cast<char>(c-'A'+'a');
     require50DU(name==(component==1?"flame01l":"flame01r"),"actual engine flame material identity");
     binding.blend=platform::VehicleWindowBlend50DS::additive;
     binding.fixedBlendAlpha=true;binding.alphaScale=255.f/128.f;
     binding.depthWrite=false;
    }
    materialBindings[component].push_back(binding);
   }
   renderRequest.materials.component[component]={materialBindings[component].data(),
       static_cast<std::uint32_t>(materialBindings[component].size())};
  }
  renderRequest.materials.textures=&gpuTextures;
  }
  renderRequest.mode=platform::VehicleWindowMode50DS::callerMaterialTextures;
  renderRequest.track.mesh=&trackMesh;
  // Reconstruction gate: RGB is authenticated by skyOwner; distance constants
  // and formula come from the retained pre-loss design, not recovered bytes.
  // Keep off by default until that source provenance is independently restored.
  const char* authoredFogSetting=std::getenv("RR_AUTHORED_WORLD_FOG");
  require50DU(!authoredFogSetting||std::string(authoredFogSetting)=="0"||
      std::string(authoredFogSetting)=="1","RR_AUTHORED_WORLD_FOG must be 0 or 1");
  if(authoredFogSetting&&std::string(authoredFogSetting)=="1"){
   require50DU(trackSource==track_source_fixture::Track::ta||
       trackSource==track_source_fixture::Track::gc,"unsupported reconstructed fog track");
   auto& fog=renderRequest.track.worldFog;
   const float* color=skyOwner->clearRgba();
   require50DU(color,"authenticated world fog RGB missing");
   for(unsigned i=0;i<3;++i)fog.color[i]=color[i];
   constexpr float minimum=1333.300048828125f,maximum=4000.f,haziness=.5f;
   const float visibility=trackSource==track_source_fixture::Track::ta?1.f:.5f;
   fog.end=minimum+visibility*(maximum-minimum);
   fog.start=(fog.end-64.f)*(1.f-haziness);fog.enabled=true;
   std::cerr<<"Opt-in reconstructed native world fog: start "<<fog.start
       <<" end "<<fog.end<<"; distance provenance pending, no retail shader parity claim\n";
  }
  renderRequest.track.sourceNormals=trackNormals.data();
  renderRequest.track.sourceNormalCount=static_cast<std::uint32_t>(trackNormals.size());
  renderRequest.track.maximumNormalBytes=trackSession->originalStaticCandidates?4u*1024u*1024u:2u*1024u*1024u;
  if(trackSession->originalStaticCandidates){
   const auto* projectionUvs=trackMaterialOwner->projectionUvs();
   require50DU(projectionUvs,"original GC projected UV publication missing");
   renderRequest.track.sourceProjectionUvs=projectionUvs->data();
   renderRequest.track.sourceProjectionCount=projectionUvs->size();
   renderRequest.track.maximumProjectionBytes=projectionUvs->size()*sizeof(rr::assets::TrackProjectedUv);
  }
  // Bounded host rendering optimization; an explicit debugging opt-out keeps
  // the same scene and physics available for comparisons.
  const char* disableCulling=std::getenv("RR_DISABLE_TRACK_CULLING");
  renderRequest.track.enableFrustumCulling=!(disableCulling&&std::string(disableCulling)=="1");
  renderRequest.track.maximumVertexBytes=trackSession->originalStaticCandidates?8u*1024u*1024u:4u*1024u*1024u;
  renderRequest.track.maximumIndexBytes=2u*1024u*1024u;
  const auto* trackBank=trackMaterialOwner->bank();
  const auto* trackSpans=trackMaterialOwner->spans();
  require50DU(trackBank&&trackSpans,"selected-track material publication missing");
  std::vector<platform::MaterialTextureSampler50EB> trackSamplers(trackBank->textureCount(),
      {platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat});
  if(trackSession->originalStaticCandidates){
   const auto* selected=trackMaterialOwner->selections();
   require50DU(selected,"original GC sampler selections missing");
   for(const auto& entry:*selected){
    require50DU(entry.textureIndex<trackSamplers.size(),"original GC sampler texture unresolved");
    auto& sampler=trackSamplers[entry.textureIndex];
    sampler.s=entry.clampU?platform::MaterialTextureWrap50EB::clampToEdge:platform::MaterialTextureWrap50EB::repeat;
    sampler.t=entry.clampV?platform::MaterialTextureWrap50EB::clampToEdge:platform::MaterialTextureWrap50EB::repeat;
   }
  }
  auto trackTextureRequest=textureRequest;
  trackTextureRequest.samplers=trackSamplers.data();
  trackTextureRequest.samplerCount=static_cast<std::uint32_t>(trackSamplers.size());
  trackTextureRequest.maximumGpuBytes=8u*1024u*1024u;
  require50DU(trackGpuTextures.create(trackTextureRequest,*trackBank,textureDiagnostic),
      "actual selected-track texture upload failed");
  std::vector<platform::VehicleWindowTrackSpan50DS> trackBindings;
  trackBindings.reserve(trackSpans->size());
  for(const auto& span:*trackSpans){
   platform::VehicleWindowTrackSpan50DS binding{};
   binding.indexStart=span.indexStart;binding.indexCount=span.indexCount;
   binding.vertexStart=span.vertexStart;binding.vertexCount=span.vertexCount;
   binding.sourcePass=span.sourcePass;binding.sourcePassCount=span.sourcePassCount;
   binding.projectionPass=span.projectionPass;
   binding.material.mode=span.textureIndex==trackTextures::unresolved?
       platform::VehicleWindowMaterialMode50DS::untextured:
       platform::VehicleWindowMaterialMode50DS::requiredTexture;
   binding.material.textureIndex=span.textureIndex;
   const auto* trackSelections=trackMaterialOwner->selections();
   require50DU(trackSelections&&span.materialSelection<trackSelections->size(),"selected-track material selection missing");
   material_alpha50gx_fixture::Policy alpha{};
   require50DU(material_alpha50gx_fixture::select(
       (*trackSelections)[span.materialSelection],alpha),"selected-track material alpha policy mismatch");
   using AlphaBlend=material_alpha50gx_fixture::Blend;
   binding.material.blend=alpha.blend==AlphaBlend::additive?
       platform::VehicleWindowBlend50DS::additive:alpha.blend==AlphaBlend::sourceAlpha?
       platform::VehicleWindowBlend50DS::sourceAlpha:platform::VehicleWindowBlend50DS::opaque;
   binding.material.depthWrite=alpha.depthWrite;binding.material.depthEqual=alpha.depthEqual;
   binding.material.alphaScale=alpha.alphaScale;
   // These authenticated beam placements explicitly disable source lighting.
   binding.material.unlit=alpha.unlit;
   binding.discardZeroAlpha=alpha.discardZeroAlpha;
   trackBindings.push_back(binding);
  }
  const auto* cableBank=playerUsesRosterRig?racerCables[0]->bank():cableOwner->bank();
  require50DU(cableBank,"selected player cable bank missing");
  const platform::MaterialTextureSampler50EB cableSampler{
      platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
      platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat};
  auto cableTextureRequest=textureRequest;
  cableTextureRequest.samplers=&cableSampler;cableTextureRequest.samplerCount=1;
  cableTextureRequest.maximumGpuBytes=64u*64u*4u;
  require50DU(cableGpuTextures.create(cableTextureRequest,*cableBank,textureDiagnostic),
      "actual cable texture upload failed");
  renderRequest.cables.textures=&cableGpuTextures;
  renderRequest.cables.textureIndex=0;
  renderRequest.track.textures=&trackGpuTextures;
  renderRequest.track.spans=trackBindings.data();
  renderRequest.track.spanCount=static_cast<std::uint32_t>(trackBindings.size());
  renderRequest.track.mode=platform::VehicleWindowMode50DS::callerMaterialTextures;
  renderRequest.track.vertexColorScale=trackSession->originalStaticCandidates?255.f/128.f:2.f; // GC source RGB scale; TA host exposure unchanged.
  const auto* skyBank=skyOwner->bank();const auto* skyMesh=skyOwner->mesh();
  const auto* skySpans=skyOwner->spans();
  require50DU(skyBank&&skyMesh&&skySpans,"actual sky publication missing");
  std::vector<platform::MaterialTextureSampler50EB> skySamplers(skyBank->textureCount(),
      {platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureWrap50EB::clampToEdge,platform::MaterialTextureWrap50EB::clampToEdge});
  auto skyTextureRequest=textureRequest;
  skyTextureRequest.samplers=skySamplers.data();
  skyTextureRequest.samplerCount=static_cast<std::uint32_t>(skySamplers.size());
  skyTextureRequest.maximumGpuBytes=48u*128u*128u*4u;
  require50DU(skyGpuTextures.create(skyTextureRequest,*skyBank,textureDiagnostic),
      "actual TA sky texture upload failed");
  std::vector<platform::VehicleWindowTrackSpan50DS> skyBindings;
  skyBindings.reserve(skySpans->size());
  for(const auto& span:*skySpans){
   platform::VehicleWindowTrackSpan50DS binding{};
   binding.indexStart=span.indexStart;binding.indexCount=span.indexCount;
   binding.vertexStart=span.vertexStart;binding.vertexCount=span.vertexCount;
   binding.material.mode=platform::VehicleWindowMaterialMode50DS::requiredTexture;
   binding.material.textureIndex=span.textureIndex;
   binding.material.depthWrite=false;binding.material.unlit=true;
   skyBindings.push_back(binding);
  }
  renderRequest.sky.mesh=skyMesh;renderRequest.sky.textures=&skyGpuTextures;
  renderRequest.sky.maximumVertexBytes=192u*sizeof(rr::assets::Vertex48B);
  renderRequest.sky.maximumIndexBytes=288u*sizeof(std::uint32_t);
  renderRequest.sky.spans=skyBindings.data();
  renderRequest.sky.spanCount=static_cast<std::uint32_t>(skyBindings.size());
  std::array<platform::VehicleWindowRacerArchetype50DS,raceOpponentCount> racerArchetypes{};
  if(raceEnabled){
   for(std::size_t racer=1;racer<racerBank.count();++racer){
    const auto index=racer-1;const auto& visual=*racerVisuals[racer];
    coldRacerStage50DU(racerRigs[racer]->podId,"material GPU upload",[&](){
     const auto ownSamplers=visual.samplers();
     auto ownRequest=textureRequest;
     ownRequest.samplers=ownSamplers.data();ownRequest.samplerCount=static_cast<std::uint32_t>(ownSamplers.size());
     ownRequest.maximumGpuBytes=8u*1024u*1024u;
     require50DU(opponentGpuTextures[index].create(ownRequest,visual.materials,textureDiagnostic),
         "original opponent texture upload failed");
    });
    coldRacerStage50DU(racerRigs[racer]->podId,"cable GPU upload",[&](){
     const auto* ownCableBank=racerCables[racer]->bank();
     require50DU(ownCableBank&&opponentCableGpuTextures[index].create(cableTextureRequest,
         *ownCableBank,textureDiagnostic),"original opponent cable texture upload failed");
    });
    coldRacerStage50DU(racerRigs[racer]->podId,"renderer archetype publication",[&](){
     auto& archetype=racerArchetypes[index];archetype.assets=&visual.drawAssets;
     archetype.materials=visual.windowMaterials(&opponentGpuTextures[index]);
     archetype.cables.textures=&opponentCableGpuTextures[index];archetype.cables.textureIndex=0;
    });
   }
   renderRequest.racerArchetypes=racerArchetypes.data();renderRequest.racerArchetypeCount=raceOpponentCount;
  }
  if(gcAnimatedWorld){
   require50DU(gcDynamicWindow.createGpu(textureRequest,*gcAnimatedWorld,textureDiagnostic),"original GC dynamic texture upload failed");
   require50DU(gcDynamicWindow.bindRequest(renderRequest,*gcAnimatedWorld),"original GC dynamic renderer publication failed");
   require50DU(gcAtm.createGpu(textureRequest,textureDiagnostic),"original GC ATM GPU bank rejected");
   require50DU(gcAtm.bindRequest(renderRequest),"original GC ATM render binding rejected");
  }
  if(!renderer.create(renderRequest,playerDrawAssets,rendererDiagnostic)){
   reportRenderer50DU(rendererDiagnostic);throw std::runtime_error("window vehicle renderer creation failed");
  }
  // Renderer owns its cold GPU copy; release CPU-only track staging promptly.
  if(!headlessLive)trackMesh=rr::assets::Mesh48B{}; // Capture-only VM geometry evidence.
  materialOwner.reset();
  samplers.clear();
  for(auto& bindings:materialBindings)std::vector<platform::VehicleWindowMaterialBinding50DS>{}.swap(bindings);
  VehiclePresentState50DA presenter{};
  platform::FixedStepClock50DT hostClock;
  constexpr std::uint64_t stepNs=16000000,maximumSleepNs=4000000;
  require50DU(hostClock.initialize(monotonicNow50DU(),stepNs,4),"fixed-step host clock setup failed");
  platform::GameHostInput50ER hostInput{};auto& input=hostInput.keys;
  // Root enables this only together with complete selected-original loading.
  // A menu-only merge must never advertise a racer that still loads POD01.
  constexpr bool alternateHumanReady50IF=true;
  platform::HostSessionMenu50IF sessionMenu(startAtMenu,raceEnabled,playerRacer60,
      alternateHumanReady50IF,audioEnabled,loadedAudioSettings,selectedAudioSettings,
      loadedTrack,selectedTrack,trackAdmission);
  platform::HostRaceResults50IL resultsUi;
  std::uint64_t drawnMenuRevision=0;
  std::int32_t drawnMenuWidth=0,drawnMenuHeight=0;
  bool menuImagePublished=false,suppressResumeKeys=false,forceSceneRender=false;
  bool capturePending=false;int terminalInputError=0;
  std::vector<std::uint8_t> captureBuffer(headlessLive?640u*480u*4u:0u);
  std::vector<std::uint8_t> captureRgbRow(headlessLive?640u*3u:0u);
  const char* visibilitySetting=std::getenv("RR_CAPTURE_VISIBILITY");
  const bool captureVisibility=visibilitySetting&&std::string(visibilitySetting)=="1";
  bool wasPaused=true,oversizeReported=false;
  std::uint64_t lastCommittedTick=0;
  host_chase_camera50fn_fixture::State cameraFraming{};
  std::uint64_t cameraAdjustedFrames=0,cameraLimitedFrames=0;
  float viewForwardX=0.f,viewForwardZ=1.f; // Host chase view, preserved at vertical attitudes.
  auto reportUiState=[&](const char* reason){
   // Evidence from the actual immutable bank owner and clocks, not a synthetic
   // pause counter. Capture may request this repeatedly without any world step.
   std::cerr<<"host_ui_state {\"reason\":\""<<reason<<"\",\"session\":"<<(sessionIndex+1)
       <<",\"audio_bank_id\":"<<reinterpret_cast<std::uintptr_t>(&liveAudio->bank())
       <<",\"menu\":"<<(sessionMenu.open()?"true":"false")<<",\"page\":"<<unsigned(sessionMenu.page())
       <<",\"loaded_racer\":"<<playerRacer60<<",\"selected_racer\":"<<selectedRacer60
       <<",\"loaded_track\":\""<<platform::hostTrackCode50IM(loadedTrack)
       <<"\",\"selected_track\":\""<<platform::hostTrackCode50IM(selectedTrack)<<'"'
       <<",\"track_pending\":"<<(sessionMenu.trackChangesPending()?"true":"false")
       <<",\"loaded_track_incomplete\":"<<(loadedTrack==platform::HostTrack50IM::GC?"true":"false")
       <<",\"gc_incomplete_opt_in\":"<<(trackAdmission.gcIncompleteWorld?"true":"false")
       <<",\"gc_mode2_admitted\":"<<(trackAdmission.gcMode2?"true":"false")
       <<",\"results_visible\":"<<(humanResultsReady&&!sessionMenu.open()?"true":"false")
       <<",\"results_action\":"<<resultsUi.selectedAction()
       <<",\"results_confirming\":"<<(resultsUi.confirming()?"true":"false")
       <<",\"audio_enabled\":"<<(audioEnabled?"true":"false")
       <<",\"audio_pending\":"<<(sessionMenu.audioChangesPending()?"true":"false")
       <<",\"audio_effective\":["<<loadedAudioSettings.effects11020<<','<<loadedAudioSettings.music11024
       <<','<<loadedAudioSettings.character11028<<','<<loadedAudioSettings.announcer1102C<<']'
       <<",\"audio_requested\":["<<selectedAudioSettings.effects11020<<','<<selectedAudioSettings.music11024
       <<','<<selectedAudioSettings.character11028<<','<<selectedAudioSettings.announcer1102C<<']'
       <<",\"tick\":"<<completedFrames<<",\"mixed_frames\":"<<liveAudio->mixedFrames()<<"}\n";
  };
  auto captureCommitted=[&](){
   if(!capturePending||(!presenter.hasPublished&&!menuImagePublished))return;
   require50DU(headlessLive,"capture path not configured");
   phase_timing50GK::Scope captureTiming(phaseTiming,phase_timing50GK::Phase::Capture);
   platform::VehicleWindowDiagnostic50DS readbackDiagnostic{};
   require50DU(renderer.readbackRgba(captureBuffer.data(),captureBuffer.size(),readbackDiagnostic),"live frame readback failed");
   const auto width=readbackDiagnostic.readbackWidth,height=readbackDiagnostic.readbackHeight;
   require50DU(width>0&&static_cast<std::size_t>(width)*3<=captureRgbRow.size(),"capture RGB row capacity");
   std::ofstream image(capturePath,std::ios::binary);
   require50DU(bool(image),"capture file open failed");
   image<<"P6\n"<<width<<' '<<height<<"\n255\n";
   for(int row=height;row>0;--row){
    const auto rowOffset=static_cast<std::size_t>(row-1)*width*4;
    for(int column=0;column<width;++column){
     const auto source=rowOffset+static_cast<std::size_t>(column)*4;
     const auto target=static_cast<std::size_t>(column)*3;
     for(unsigned channel=0;channel<3;++channel)captureRgbRow[target+channel]=captureBuffer[source+channel];
    }
    image.write(reinterpret_cast<const char*>(captureRgbRow.data()),static_cast<std::streamsize>(width)*3);
   }
   image.close(); // Publish the complete file before the capture acknowledgment.
   require50DU(bool(image),"capture file write failed");
   std::cerr<<"live capture committed at tick "<<lastCommittedTick
            <<(recoveryPaused?" (retained before undertrack pause)":"")<<'\n';
   std::cerr<<"capture_state wrong_way="<<lastCommittedWrongWay<<" tick="<<lastCommittedTick
       <<" host_menu="<<menuImagePublished<<'\n';
   reportUiState("capture");
   if(!menuImagePublished&&captureFirstWrongWay&&lastCommittedWrongWay)capturedFirstWrongWay=true;
   capturePending=false;
  };
  std::uint64_t lastStatusNs=monotonicNow50DU();
  std::uint64_t lastAllAiStatusNs=lastStatusNs;
  std::uint64_t lastStatusRenderCount=0;
  std::cerr<<"Interactive development client: W/S left engine throttle/brake; "
               "Up/Down right engine throttle/brake; W+Up both throttle; Space boosts; "
               "S+Down both brake; hold R to repair; Left Shift powerslides; Backspace resets to checkpoint; F5 restarts the grid; Escape opens the pause menu. "
               "Opposite keys on one axis center.\n"
               "Preset4 is explicit. R maps native repair flag69C button4; "
               "Space maps native boost flag698 button5; Left Shift maps powerslide flag6B4 button7; other action buttons stay released. 16ms host ticks, "
               "maximum 4 catch-up ticks. Focus loss/minimize pauses.\n"
               "Actual textured pod meshes and selected-track material spans with host lighting; "
               <<terrainOwnerCount<<" original colliders with live portal memberships across "<<zoneCount<<" zones.\n"
           <<"Navigation: "<<navigationMap->track.count<<" map / "
           <<navigationAI->track.count<<" AI nodes; original gates and portal indices bound. "
             "Live progression is tracked; original-AI admission stops before checkpoint recovery.\n"
           <<"Repair CF4 "<<s.fields.repairCF4
           <<" from actual "<<playerPodId<<" selected authoring; profile overwrite is explicitly absent.\n";
  if(autoDriveEnabled)std::cerr<<"HOST course-check autopilot enabled; pace="
      <<(courseTuning.authoredSpeedOnly?"authored-limits":"conservative-host")
      <<" speed-limit-to-body-scale="<<courseTuning.speedLimitToBodyScale
      <<" steering-preview-seconds="<<courseTuning.authoredSteeringLookaheadSeconds
      <<" full-throttle-steering="<<courseTuning.fullThrottleSteeringRange
      <<"; not original AI. Manual W/S/Up/Down takes over. No automatic reset.\n";
  if(headlessLive)std::cerr<<"Stdin host commands include capture, checkpoint, restart, pause, resume, menu, and quit. "
      "Menus use down/up Up, Down, Left, Right, Enter, Escape. Audio edits apply on Start/Restart only. "
      "Restart destroys this session and repeats the authored-grid cold setup.\n";
  reportUiState("ready");
  if(allAiEnabled){
   requireOriginalAiFleetReady();
   reportOriginalAiFleet("ready");
  }
  for(;;){
   phase_timing50GK::Scope inputTiming(phaseTiming,phase_timing50GK::Phase::Input);
   if(!host.poll(hostInput,hostDiagnostic)){
    reportHost50DU(hostDiagnostic);throw std::runtime_error("window event pump failed");
   }
   inputTiming.finish();
   if(hostInput.inputError){terminalInputError=hostInput.inputError;std::cerr<<"stdin input error "<<terminalInputError<<'\n';}
   // Window close, explicit stdin quit, and EOF remain terminal. Escape is
   // host navigation only, never a native vehicle action or an implicit exit.
   if(input.quitRequested){
    composePacket(false,input);sampleController(); // Route a final held release before cleanup.
    break;
   }
   if(hostInput.restartRequested){
    composePacket(false,input);sampleController();
    restartRequested=true;
    break;
   }
   const auto resultsRevision=resultsUi.revision();
   const bool resultsAvailable=humanResultsReady&&!sessionMenu.open()&&!recoveryPaused&&
       s.phase==VehicleFramePhase49W::Ready&&input.focused&&!input.minimized&&!input.eventBudgetExhausted;
   const bool resultsBack=resultsAvailable&&resultsUi.confirming()&&input.pressed[41]&&
       !hostInput.menuRequested&&!hostInput.pauseRequested;
   const bool resultsAdmitted=resultsAvailable&&!hostInput.menuRequested&&!hostInput.pauseRequested&&
       (!input.pressed[41]||resultsBack);
   const auto resultsAction=resultsUi.poll(input,resultsAdmitted);
   if(resultsBack)input.pressed[41]=0; // Escape cancels this live confirmation, not Pause.
   if(resultsUi.revision()!=resultsRevision){forceSceneRender=true;reportUiState("results");}
   if(resultsAction!=platform::HostRaceResults50IL::Action::None){
    std::cerr<<"host_results_action action="<<unsigned(resultsAction)<<" tick="<<completedFrames
        <<" mixed_frames="<<liveAudio->mixedFrames()<<'\n';
    if(resultsAction==platform::HostRaceResults50IL::Action::MainMenu){
     // An explicit confirmed Results choice uses the existing front-menu
     // request. Merely displaying/navigating Results never enters this path.
     hostInput.menuRequested=true;
    }else{
     composePacket(false,input);sampleController();
     restartRequested=resultsAction==platform::HostRaceResults50IL::Action::Restart;
     break;
    }
   }
   if(humanResultsReady&&!sessionMenu.open()){
    // These keys belong to Results navigation. Native finish controls retain
    // sole ownership; UI arrows/Space never count as attempted manual driving.
    constexpr std::uint16_t gameplayKeys[]={26,22,82,81,44,21,225};
    for(const auto key:gameplayKeys)input.down[key]=input.pressed[key]=0;
   }
   const bool menuWasOpen=sessionMenu.open();
   const auto menuRevision=sessionMenu.revision();
   if(input.focused&&!input.minimized&&!input.eventBudgetExhausted){
    const auto action=sessionMenu.poll(hostInput,!recoveryPaused&&s.phase==VehicleFramePhase49W::Ready);
    selectedRacer60=sessionMenu.selectedRacer60();
    selectedAudioSettings=sessionMenu.selectedAudioSettings();
    selectedTrack=sessionMenu.selectedTrack();
    if(action==platform::HostSessionMenu50IF::Result::Restart||
       action==platform::HostSessionMenu50IF::Result::Quit){
     composePacket(false,input);sampleController();
     restartRequested=action==platform::HostSessionMenu50IF::Result::Restart;
     break;
    }
   }
   if(sessionMenu.revision()!=menuRevision){
    std::cerr<<"host_menu open="<<sessionMenu.open()<<" page="<<unsigned(sessionMenu.page())<<" tick="<<completedFrames
        <<" mixed_frames="<<liveAudio->mixedFrames()<<'\n';
    reportUiState("transition");
   }
   if(menuWasOpen&&!sessionMenu.open()){
    suppressResumeKeys=true;forceSceneRender=true;
    // Never spend the time in any host menu as catch-up simulation.
    hostClock.pause(monotonicNow50DU());
    // Menu wall time/frames are not part of gameplay FPS or phase intervals.
    lastStatusNs=monotonicNow50DU();lastStatusRenderCount=submittedDrawFrames;
    phaseTiming.resetInterval();renderDiagnostics.resetInterval();
   }
   if(hostInput.checkpointRequested&&!sessionMenu.open()){
    composePacket(false,input);sampleController();
    if(raceEnabled){
     std::cerr<<"Race restart requested: checkpoint rebuild restarts every racer and countdown together.\n";
     restartRequested=true;break;
    }
    if(recoveryPaused||s.phase!=VehicleFramePhase49W::Ready){
     std::cerr<<"Manual checkpoint unavailable at a paused native boundary; F5/restart returns to grid.\n";
    }else{
     recovery50FI::Plan manualCheckpoint{};recovery50FI::Diagnostic manualDiagnostic{};
     BodyMatrix48G manualGrounded{};
     const bool prepared=recovery50FI::prepareManualCheckpoint(*navigationMap,navigationProgress,s,
         x.bindings,x.parameters.visual.verticalLimit7EC,liveWorld,manualCheckpoint,manualDiagnostic);
     const bool grounded=prepared&&recovery50FI::resolveGround(ground,manualCheckpoint,16,0,
         groundStack.data(),groundStack.size(),manualGrounded,manualDiagnostic);
     if(grounded){
      checkpointOutput=manualCheckpoint;checkpointRequested=true;
      std::cerr<<"Manual checkpoint requested: anchor="<<manualCheckpoint.selection.anchorNode
          <<" zone="<<manualCheckpoint.primaryZone<<" damage="<<manualCheckpoint.damage.damage7D4
          <<'/'<<manualCheckpoint.damage.damage7D0<<". Progress/damage retained; held input released.\n";
      break;
     }
     recoveryPaused=true;
     std::cerr<<"Manual checkpoint preflight failed: error="<<unsigned(manualDiagnostic.error)
         <<". Paused on last committed image; F5/restart returns to grid.\n";
    }
   }
   capturePending=capturePending||hostInput.captureRequested;
   const auto now=monotonicNow50DU();
   const auto& info=host.info();
   if(sessionMenu.open()){
    composePacket(false,input);
    if(!wasPaused||!menuWasOpen)sampleController();
    if(audioEnabled){liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);}
    hostClock.pause(now);wasPaused=true;
    const bool drawable=!info.minimized&&info.drawableWidth>0&&info.drawableHeight>0&&
        info.drawableWidth<=maximumWidth&&info.drawableHeight<=maximumHeight;
    if(drawable&&(sessionMenu.revision()!=drawnMenuRevision||
       info.drawableWidth!=drawnMenuWidth||info.drawableHeight!=drawnMenuHeight||input.resized||capturePending)){
     const auto menu=sessionMenu.view(!recoveryPaused&&s.phase==VehicleFramePhase49W::Ready);
     if(!renderer.submitHostMenu(menu,info.drawableWidth,info.drawableHeight,rendererDiagnostic)){
      reportRenderer50DU(rendererDiagnostic);throw std::runtime_error("host menu submission failed; not swapped");
     }
     if(host.hasWindow()&&!host.swapWindow(hostDiagnostic)){
      reportHost50DU(hostDiagnostic);throw std::runtime_error("host menu window swap failed");
     }
     menuImagePublished=true;drawnMenuRevision=sessionMenu.revision();
     drawnMenuWidth=info.drawableWidth;drawnMenuHeight=info.drawableHeight;
    }
    captureCommitted();
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    continue;
   }
   if(recoveryPaused){
    // Never prepare or submit a draw batch from the interrupted native state.
    // Readback uses only the renderer's already committed private target.
    if(audioEnabled){liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);}
    captureCommitted();hostClock.pause(now);
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    continue;
   }
   const bool oversize=info.drawableWidth>maximumWidth||info.drawableHeight>maximumHeight;
   if(oversize&&!oversizeReported){
    std::cerr<<"Paused: drawable exceeds 1280x720; resize smaller to resume.\n";
    oversizeReported=true;
   }else if(!oversize&&oversizeReported){
    std::cerr<<"Drawable fits again.\n";oversizeReported=false;
   }
   const bool paused=!input.focused||input.minimized||input.eventBudgetExhausted||
       info.drawableWidth<=0||info.drawableHeight<=0||oversize;
   if(suppressResumeKeys){
    constexpr std::uint16_t gameplayKeys[]={26,22,82,81,44,21,225};
    bool held=false;
    for(const auto key:gameplayKeys)held=held||input.down[key];
    suppressResumeKeys=held;
    // Keep UI navigation/activation from becoming throttle/boost or an
    // autopilot override. Release the relevant physical keys to re-arm them.
    for(const auto key:gameplayKeys)input.down[key]=input.pressed[key]=0;
   }
   composePacket(!paused&&(!raceEnabled||race_session50HM::controlsAllowed(race,0)),input);
   if(paused){
    if(audioEnabled){liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);}
    if(!wasPaused)sampleController(); // Publish the declared release-all sample.
    hostClock.pause(now);wasPaused=true;
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    continue;
   }
   if(wasPaused){hostClock.pause(now);wasPaused=false;if(audioEnabled){liveAudio->pause(false);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(false);}}
   // Manual input samples every host poll. The optional host course driver
   // instead emits one ordinary packet per actual 16 ms simulation step.
   if(!autoDriveEnabled)sampleController();
   platform::FixedStepPlan50DT plan{};
   require50DU(hostClock.plan(now,plan),"fixed-step host clock planning failed");
   addCounter50DU(discardedHostNs,plan.discardedNanoseconds);
   for(std::uint32_t step=0;step<plan.steps;++step){
    if(raceEnabled&&!race_session50HM::advanceCountdown(race,.016)){
     require50DU(race.error==race_session50HM::Error::None,"race countdown rejected");
     advanceWorldAnimations();
     if(audioEnabled){
      liveAudio->pause(false);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(false);
      require50DU(raceAudio->observe(race).accepted,"countdown/result audio observation rejected");
      updateOpponentAudio(); // Only countdown holds native vehicle updates.
      // Countdown has a real host clock while native vehicle bodies are held.
      // Advance the shared mixer ONCE; completed results keep normal world steps.
      if(liveAudio->outputOpen()){
       const auto beforeMix=liveAudio->mixedFrames();
       const bool mixed=liveAudio->committedStep(audioDiagnostic);
       advanceSourceAudioWorld(beforeMix);
       require50DU(mixed,"native countdown/result audio step failed");
      }
     }
     continue;
    }
    if(originalAiEnabled&&!(originalAiSession.manager.flags58&2u)){
     // One actual started-byte owner for the whole native slice. The selected
     // HOST countdown publishes its GO edge once, before any racer executes.
     originalAiSession.manager.flags58=static_cast<std::uint8_t>(originalAiSession.manager.flags58|2u);
     for(std::size_t slot=0;slot<racerBank.count();++slot)racerBank.visit(slot,[&](){
      s.fields.applicationByte58=originalAiSession.manager.flags58;
     });
    }
    const bool playerAdvancing=!raceEnabled||race_session50HM::simulationAllowed(race,0);
    const bool playerControls=!raceEnabled||race_session50HM::controlsAllowed(race,0);
    const bool playerFinishDriver=humanResultsActive;
    const bool manualRaceInputHeld=input.down[26]||input.down[22]||input.down[82]||
        input.down[81]||input.down[44]||input.down[21]||input.down[225];
    if(audioEnabled){liveAudio->pause(false);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(false);}
    // Recompose at GO so held race input enters this exact native frame.
    // After the human finish, the recovered source driver owns controls while
    // the ordinary coupled-body/native frame keeps advancing.
    if(playerAdvancing&&!allAiEnabled){
     composePacket(playerControls,input);
     if(playerControls&&autoDriveEnabled)applyCourseInput();
     sampleController();
    }
    const auto nativeFrameAllocations=::allocations;
    phase_timing50GK::Scope physicsTiming(phaseTiming,phase_timing50GK::Phase::Physics);
    if(playerAdvancing){
     captureDynamicBeforeFrame(0);
     // Source AI peer reads resolve live racers in serial update order. This
     // copies current owners only: manager timing/CC4/CCC do not advance here.
     if(originalAiEnabled)observeOriginalAiFleet();
    }
    const bool frameAdvanced=!playerAdvancing||advanceFrame();
    if(originalAiEnabled&&!frameAdvanced){
     std::cerr<<"ORIGINAL AI world boundary: slot=0 phase="<<static_cast<unsigned>(s.phase)
         <<" frame_source_stage="<<std::hex<<x.diagnostic.sourceStage<<std::dec
         <<"; native partial state retained; manager was not advanced.\n";
     require50DU(false,"original AI requires the camera-followed racer frame to complete");
    }
    if(frameAdvanced)require50DU(::allocations==nativeFrameAllocations,
        "successful player physics frame allocated C++ storage");
    if(frameAdvanced&&raceEnabled){
     if(playerFinishDriver){
      require50DU(finishDriverAppliedThisFrame,"finished player frame missed native control owner");
      ++finishDriverTicks;
      if(manualRaceInputHeld)++finishManualInputTicks;
     }
     if(originalAiEnabled){observeOriginalAiFleet();updateOriginalAiPacking();}
     const platform::WindowInput50DQ noKeys{};
     for(std::size_t racer=1;racer<racerBank.count();++racer){
      if(!racerBank.active(racer)||!race_session50HM::simulationAllowed(race,static_cast<std::uint32_t>(racer)))continue;
      opponentFrame=true;
      try {
       captureDynamicBeforeFrame(racer);
       // Outside Bank::step/visit: no nested arena lease, no stale earlier
       // racer position/C80/speed after that racer completed its native frame.
       if(originalAiEnabled)observeOriginalAiFleet();
       const bool advanced=racerBank.step(racer,true,[&](){
        if(s.fields.vehicle58==0u){composePacket(true,noKeys);applyCourseInput(racer);sampleController();}
        const auto beforeOpponentFrame=::allocations;
        const bool completed=advanceFrame();
        if(completed)require50DU(::allocations==beforeOpponentFrame,
            "successful opponent physics frame allocated C++ storage");
        return completed;
       });
       if(!advanced)std::cerr<<racerNames[racer]<<" stopped at a native source boundary"
           <<(originalAiEnabled?"; joined AI world cannot continue.\n":"; other racers continue.\n");
      }catch(const std::exception& error){
       racerBank.stop(racer);
       std::cerr<<racerNames[racer]<<" stopped: "<<error.what()
           <<(originalAiEnabled?". Joined AI world cannot continue.\n":". Other racers continue.\n");
      }
      opponentFrame=false;
      // Deliberately outside the per-opponent catch: the optional source-AI
      // world cannot swallow a requisite racer's failure and advance later
      // racers, animations, contacts, manager timing or rank from that frame.
      if(originalAiEnabled)requireOriginalAiFleetReady();
     }
     advanceWorldAnimations();
     // Exactly one host contact pass after all successful native participants
     // are Ready, before race observations and the single world audio mix.
     // A failed native slot never participates through an abandoned snapshot.
     // Sealed results alone do not remove a Ready native vehicle from contacts.
     if(racerContactsEnabled){
      std::array<bool,native_racer_contact50HW::maxRacers> contactParticipant{};
      std::array<native_racer_contact50HW::Pair,
          native_racer_contact50HW::maxRacers*(native_racer_contact50HW::maxRacers-1)> contactOrder{};
      std::size_t contactPairCount=0;
      for(std::size_t racer=0;racer<racerBank.count();++racer)
       contactParticipant[racer]=racerBank.active(racer)&&
           race_session50HM::simulationAllowed(race,static_cast<std::uint32_t>(racer));
      for(std::size_t primary=0;primary<racerBank.count();++primary)if(contactParticipant[primary])
       for(std::size_t other=0;other<racerBank.count();++other)
        if(primary!=other&&contactParticipant[other])contactOrder[contactPairCount++]={primary,other};
      if(contactPairCount){
       native_racer_contact50HW::Receipt contactReceipt{};
       native_racer_contact50HW::AiContactBoundary50IV aiContactBoundary{};
       const auto beforeRacerContacts=::allocations;
       const auto contactStatus=native_racer_contact50HW::applyAtWorldBoundary(
           racerBank,*racerContactOwner,s,x.bindings,x.parameters,racerContactIds,racerContactMaterials,
           contactOrder.data(),contactPairCount,contactReceipt,&aiContactBoundary,
           dynamicContactEnabled,dynamicContactEnabled?&dynamicRegistry:nullptr);
       racerContactTotals.lastStatus=contactStatus;
       if(contactStatus!=native_racer_contact50HW::Status::Complete){
        addCounter50DU(racerContactTotals.failures);
        std::cerr<<"RACER CONTACT FAILURE: player_tick="<<completedFrames<<" ordered_pairs="<<contactPairCount
            <<" status="<<static_cast<unsigned>(contactStatus)<<"; rejected body outputs were not committed.\n";
        if(aiContactBoundary.unsupported)std::cerr<<"ORIGINAL AI dynamic-contact owner boundary: slot="
            <<aiContactBoundary.racerSlot<<" pc="<<std::hex<<aiContactBoundary.sourcePC<<std::dec
            <<" contacts="<<aiContactBoundary.contactCount20C<<"; no pair results committed.\n";
        reportRacerContacts();
        require50DU(false,"native racer contact world pass rejected");
       }
       require50DU(::allocations==beforeRacerContacts,"successful racer contact world pass allocated C++ storage");
       addCounter50DU(racerContactTotals.passes);
       addCounter50DU(racerContactTotals.testedPairs,contactReceipt.testedPairs);
       addCounter50DU(racerContactTotals.geometryPairs,contactReceipt.geometryPairs);
       addCounter50DU(racerContactTotals.suppressedPairs,contactReceipt.suppressedPairs);
       addCounter50DU(racerContactTotals.respondedPairs,contactReceipt.respondedPairs);
       addCounter50DU(racerContactTotals.generatedRecords,contactReceipt.generatedRecords);
       addCounter50DU(racerContactTotals.activeRecords,contactReceipt.activeRecords);
      }else addCounter50DU(racerContactTotals.noPairs);
     }
     if(originalAiEnabled)advanceOriginalAiManager(m.frameFactor);
     if(mode1Enabled){
      const auto timing=mode1->beforeHostCommit(mode1->acceptedWorld+1u,.016f);
      require50DU(timing.status==race_mode1_host50ID::Status::AwaitHostCommit,
          "mode1 source timing boundary unresolved; accepted world not replayed");
     }
     sampleRaceField();
     const auto previousFinishCount=race.finishCount;
     const bool playerWasFinished=race.racers[0].finished;
     require50DU(race_session50HM::commitFrame(race,.016),"completed race world frame rejected");
     // Original1B035C sets the finished record+48 bit0, alias vehicle+CD8.
     // Project each newly sealed edge into its independent native state before
     // another frame. No body pose/velocity or source lap counter is written.
     for(std::size_t racer=0;racer<racerBank.count();++racer)
      if(race.racers[racer].finished&&race.racers[racer].finishPlace>previousFinishCount)
       racerBank.visit(racer,[&](){
        s.fields.impactFlagsCD8=static_cast<std::uint8_t>(s.fields.impactFlagsCD8|1u);
        lapContinuity.state.vehicleCD8=s.fields.impactFlagsCD8;
        observedNativeFinishFlags[racer]=s.fields.impactFlagsCD8;
       });
     if(race.racers[0].finished&&!playerWasFinished){
      //1AF9C0 closes the manager after its completed record pass: preserve all
      // genuine same-world-frame finish edges, then freeze official standings.
      require50DU(race_session50HM::closeResults(race),"human finish result closure rejected");
      std::cerr<<"RACE FINISH: player place="<<race.racers[0].finishPlace
          <<"/"<<race.racerCount<<" time="<<race.racers[0].finishSeconds<<" seconds.\n";
      // Source CD8 warning subscribers stop their own warning loops. Engine,
      // binder, material and other-racer voices keep their original owners;
      // never stop the shared mixer at a human finish.
      // Original manager ends once every human finishes: this host has one.
      // Its ended-bit projection belongs to the human bank. Native AI checks
      // only started-bit1; the unjoined AI's HOST player-frame adapters retain
      // their started gate2 so they can continue their real navigation/physics.
      s.fields.applicationByte58=static_cast<std::uint8_t>(s.fields.applicationByte58|1u);
      humanResultsActive=true;humanResultsCountdown=4.f;
     }
     if(mode1Enabled){
      // HOST menus admit no world/event flush at all. No original modal menu
      // or application1B0 projection is installed: explicit1B0 remains0 here.
      const auto boundary=mode1->afterHostCommit(mode1->acceptedWorld+1u,0);
      require50DU(boundary.status==race_mode1_host50ID::Status::NeedFlush||
          boundary.status==race_mode1_host50ID::Status::Complete,"mode1 HOST observer handoff failed");
     }
     if(audioEnabled)require50DU(raceAudio->observe(race).accepted,
         "committed race audio observation rejected");
    }
    if(frameAdvanced&&!raceEnabled)advanceWorldAnimations();
    physicsTiming.finish();
    if(!frameAdvanced){
     if(audioEnabled){liveAudio->pause(true);if(mode1Enabled||dynamicContactEnabled)raceMusic->pause(true);}
     composePacket(false,input);sampleController();
     hostClock.pause(monotonicNow50DU());wasPaused=true;
     break;
    }
    // Source player speech runs only on the human's accepted native step.
    if(mode1Enabled&&mode1->phase==race_mode1_host50ID::Phase::Flush){
     const auto flushed=mode1->flush(applicationClock,sessionLazy,s.noise.sharedRandomState,
         speechCounts,speechState,mode1Effects);
     if(flushed.status!=race_mode1_host50ID::Status::Complete)
      std::cerr<<"MODE1 FIFO BLOCKED: source_pc=0x"<<std::hex<<flushed.sourcePC<<std::dec
          <<" ticket="<<flushed.subscriber.ticket<<" lap_helper="<<unsigned(flushed.subscriber.lapHelper)
          <<" damage_helper="<<unsigned(flushed.subscriber.damageHelper)<<"; no acknowledgement.\n";
     require50DU(flushed.status==race_mode1_host50ID::Status::Complete,
         "mode1 real downstream request unresolved; refusing later world events");
    }
    if(dynamicContactEnabled){
     // Declared accepted-world flush: all actual frames and the source manager
     // rank pass completed. Read actors afresh, not enqueue-time snapshots.
     captureDynamicBeforeFrame(0);
     const auto began=beginRaceEventFlush50ID(dynamicEvents,originalAiSession.manager.flags58,0u);
     if(began!=RaceEventStatus50ID::FlushSuppressed){
      if(began!=RaceEventStatus50ID::Ready&&began!=RaceEventStatus50ID::Empty)
       dynamicBoundary(0x1a4c9cu,"original shared event flush admission");
      while(const auto* entry=frontRaceEvent50ID(dynamicEvents)){
       const auto ticket=entry->ticket;
       if(entry->event.type4==1u){
        using Status=DynamicContactEffectStatus50IW;
        if(beginDynamicContactEvent1Subscriber50IW(dynamicEvents,dynamicEventCursor).status!=Status::Continue)
         dynamicBoundary(0x1a5940u,"contact subscriber entry pending");
        for(;;){
         const auto step=advanceDynamicContactEvent1Subscriber50IW(dynamicEventCursor,
             dynamicActorResolver,dynamicEventBindings,s.noise.sharedRandomState);
         if(step.status==Status::Complete)break;
         if(step.status!=Status::NeedHelper)dynamicBoundary(step.sourcePC,"contact event source continuation");
         const auto& request=step.request;
         if(request.kind==DynamicContactEventCall50IW::PodTaunt2C1C00||
            request.kind==DynamicContactEventCall50IW::PodCurse2C1A90){
          // Only actor1's authentic D48 constructor/counts/speech bank have
          // been joined. Never reuse its state for an opponent or call a miss.
          if(request.actor!=1u)dynamicBoundary(request.callPC,"actual opponent D48 speech owner unavailable");
          const auto live=speechInput();
          auto selected=beginDynamicContactPodSpeech50IW(request,1u,speechCounts,
              live.position,live.listeners,s.noise.sharedRandomState,speechState,dynamicPodEffect);
          if(selected.status==Status::NeedHelper){
           if(!liveAudio->outputOpen()||!liveAudio->applySourceCommand(dynamicPodEffect.command).okay())
            dynamicBoundary(selected.sourcePC,"actual contact speech audio helper pending");
           if(!acceptDynamicContactPodAudioReturn50IW(dynamicPodEffect,request.sequence,1u,speechState))
            dynamicBoundary(selected.sourcePC,"contact speech audio return mismatch");
           ++speechCues;
          }else if(selected.status!=Status::Complete)
           dynamicBoundary(selected.sourcePC,"actual contact speech selector pending");
         }else if(request.kind==DynamicContactEventCall50IW::Announcer2C2C20){
          // Provider preserves its own unsupported type1/2 boundary. Passing
          // actual11258=1 cannot masquerade as mode2 suppression.
          if(!sourceAnnouncer->request(request.type,request.racer60,request.variant,
              dynamicMusicProfile.mode11258,originalAiSession.manager.flags58))
           dynamicBoundary(request.callPC,"original collision announcer helper pending");
         }else dynamicBoundary(request.callPC,"unknown original contact event helper");
         if(!acceptDynamicContactEventReturn50IW(dynamicEventCursor,request.sequence,request.kind))
          dynamicBoundary(request.callPC,"original contact event return mismatch");
        }
       }else if(entry->event.type4==5u){
        if(entry->event.actor0C!=1u)dynamicBoundary(0x1a52b0u,"actual event5 D48 owner unavailable");
        const auto started=beginDamageEvent5Subscriber50ID(dynamicEvents,speechInput(),speechCounts,
            s.noise.sharedRandomState,speechState,dynamicDamageCursor);
        if(started!=DamageEventStatus50ID::Complete&&started!=DamageEventStatus50ID::NeedEffect)
         dynamicBoundary(0x1a52b0u,"original damage subscriber pending");
        if(!dynamicDamageCursor.complete){
         bool returned=false;
         if(dynamicDamageCursor.pending==DamageEventEffect50ID::AudioCommand){
          returned=liveAudio->outputOpen()&&liveAudio->applySourceCommand(dynamicDamageCursor.command).okay();
          if(returned)++speechCues;
         }else if(dynamicDamageCursor.pending==DamageEventEffect50ID::Announcer2C2C20){
          const auto& request=dynamicDamageCursor.receipt;
          returned=request.announcerArgumentKnown&&sourceAnnouncer->request(request.announcerType,
              request.announcerRacer,request.announcerVariant,dynamicMusicProfile.mode11258,
              originalAiSession.manager.flags58);
         }
         if(!returned)dynamicBoundary(0x1a52b0u,"actual damage event effect helper pending");
         if(acceptDamageEventEffectReturn50ID(dynamicDamageCursor,ticket,dynamicDamageCursor.pending)!=
             DamageEventStatus50ID::Complete)dynamicBoundary(0x1a52b0u,"damage event helper return mismatch");
        }
       }else dynamicBoundary(0x1a4d30u,"unjoined actual shared event subscriber");
       if(acceptRaceEventReturn50ID(dynamicEvents,ticket)!=RaceEventStatus50ID::Complete)
        dynamicBoundary(0x1a5c40u,"original FIFO acknowledgement mismatch");
      }
      if(finishRaceEventFlush50ID(dynamicEvents)!=RaceEventStatus50ID::Complete)
       dynamicBoundary(0x1a5c40u,"original FIFO flush remains pending");
     }
    }
    if(audioEnabled&&playerAdvancing&&!mode1Enabled&&!dynamicContactEnabled){
     AudioCommand50HE speechCommand{};std::size_t speechCount=0;
     const auto dispatched=dispatchDamageSpeechEvent50HG(speechInput(),speechCounts,pendingSpeech,
         s.noise.sharedRandomState,speechState,&speechCommand,1,speechCount);
     require50DU(dispatched.status==PodSpeechStatus50HG::Complete,"native damage speech dispatch rejected");
     if(dispatched.announcerRequested)++announcerRequestsUnjoined;
     if(speechCount){
      require50DU(liveAudio->applySourceCommand(speechCommand).okay(),"genuine speech playback request rejected");
      ++speechCues;
     }
    }
    if(playerFinishDriver){
     // Normal source finish countdown is four seconds. Audio/event waiting and
     // the original menu transition remain unjoined. At zero the native client
     // offers a live Results overlay without stopping this scene or its audio.
     humanResultsCountdown=std::max(0.f,humanResultsCountdown-.016f);
     humanResultsReady=humanResultsCountdown<=0.f;
    }
    updateOpponentAudio(); // After the complete native world commit, before the one shared mix.
    // All racer/audio subscribers feed one mixer. Continue the finish clip and
    // any remaining field while the human is finished; never mix per opponent.
    if(audioEnabled&&liveAudio->outputOpen()){
     const auto beforeMix=liveAudio->mixedFrames();
     const bool mixed=liveAudio->committedStep(audioDiagnostic);
     advanceSourceAudioWorld(beforeMix);
     require50DU(mixed,"native mixed audio world step failed");
    }
   }
   if(recoveryPaused){captureCommitted();if(checkpointRequested)break;continue;}

   if(captureFirstWrongWay&&!capturedFirstWrongWay&&(lapContinuity.prefix.flags10CC&2u))capturePending=true;
   if(completedFrames&&(plan.steps||input.resized||!presenter.hasPublished||capturePending||forceSceneRender)){
    phase_timing50GK::Scope renderTiming(phaseTiming,phase_timing50GK::Phase::RenderPrepareSubmit);
    {
     render_diagnostics50GQ::Scope censusTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::Census);
     renderDiagnostics.observeScene(s,completedFrames);
    }
    render_diagnostics50GQ::Scope bankTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::DrawBank);
    const auto bank=static_cast<std::uint8_t>(presenter.hasPublished?(presenter.publishedCpuBank^1u):0u);
    VehicleDrawBatchInput50CO drawInput{};
    require50DU(playerDrawAssets.batchInput(bank,ModelPalettePhase49S::selectedRenderPreparation,
        ModelPaletteSceneState49S::completeCurrentScene,drawInput),"native draw bank preparation failed");
    bankTiming.finish();
    render_diagnostics50GQ::Scope projectionTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::VehicleProjection);
    VehicleDrawBatch50CO next{};VehicleDrawBatchDiagnostic50CO drawDiagnostic{};
    if(!buildVehicleDrawBatch50CO(s,x.bindings,drawInput,next,drawWorkspace,drawDiagnostic)){
     std::cerr<<"draw preparation component "<<drawDiagnostic.component<<" error "
              <<static_cast<unsigned>(drawDiagnostic.detail.error)<<'\n';
     throw std::runtime_error("actual vehicle mesh preparation failed");
    }
    projectionTiming.finish();
    render_diagnostics50GQ::Scope cameraTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::HostCamera);
    platform::VehicleViewCamera50DG camera{};
    // Host chase-camera policy follows the actual main scene heading. This
    // changes only presentation; it is not the original camera controller.
    const auto& pose=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].world100;
    const float headingLength=std::sqrt(pose.v[8]*pose.v[8]+pose.v[10]*pose.v[10]);
    if(std::isfinite(headingLength)&&headingLength>0.0001f){
     viewForwardX=pose.v[8]/headingLength;viewForwardZ=pose.v[10]/headingLength;
    }
    const auto& position=m.components.root.body.position;
    camera.eye[0]=position.v[0]-viewForwardX*52.f;
    camera.eye[1]=position.v[1]+12.f;
    camera.eye[2]=position.v[2]-viewForwardZ*52.f;
    camera.target[0]=position.v[0]+viewForwardX*38.f;
    camera.target[1]=position.v[1]+3.f;
    camera.target[2]=position.v[2]+viewForwardZ*38.f;camera.up[1]=1.f;
    camera.verticalFovRadians=1.0471975512f;
    camera.aspectWidthOverHeight=float(info.drawableWidth)/float(info.drawableHeight);
    camera.nearPlane=1.f;camera.farPlane=5400.f; // Requested80% increase over3000.
    platform::VehicleViewCamera50DG framedCamera{};
    host_chase_camera50fn_fixture::Diagnostic framingDiagnostic{};
    require50DU(host_chase_camera50fn_fixture::framePod(camera,next,cameraFraming,
        framedCamera,framingDiagnostic),"host camera framing rejected prepared bounds");
    if(framingDiagnostic.adjusted)++cameraAdjustedFrames;
    if(framingDiagnostic.limited)++cameraLimitedFrames;
    camera=framedCamera;
    audioListener={camera.eye[0],camera.eye[2]};audioListenerReady=true;
    platform::VehicleViewMatrix50DG view{};platform::VehicleViewCameraError50DG cameraError{};
    require50DU(platform::composeVehicleViewCamera50DG(camera,view,cameraError),
                "host chase camera rejected live pose");
    auto skyCamera=camera;
    for(unsigned axis=0;axis<3;++axis){
     skyCamera.target[axis]=camera.target[axis]-camera.eye[axis];skyCamera.eye[axis]=0.f;
    }
    skyCamera.farPlane=100000.f; // Actual sky half-size25600; source Y+1024 is in mesh.
    platform::VehicleViewMatrix50DG skyView{};
    require50DU(platform::composeVehicleViewCamera50DG(skyCamera,skyView,cameraError),
        "source-centered sky camera rejected");
    Vector3_48C cameraBackward{{camera.eye[0]-camera.target[0],
        camera.eye[1]-camera.target[1],camera.eye[2]-camera.target[2]}};
    cameraTiming.finish();
    render_diagnostics50GQ::Scope cablesTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::SourceCables);
    runNative([&]{return buildPodCables50FD(s,x.bindings,playerCableBindings,cameraBackward,
        cableMesh,cableWorkspace,cableDiagnostic);},"source cable mesh generation");
    cablesTiming.finish();
    render_diagnostics50GQ::Scope hudTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::HostHud);
    platform::VehicleWindowHostHud50DS hud{};
    hud.heat01=std::clamp(boostState.heat7C,0.f,1.f);
    hud.leftDamage01=std::clamp(s.fields.damage7D4,0.f,1.f);
    hud.rightDamage01=std::clamp(s.fields.damage7D0,0.f,1.f);
    hud.boostActive=boostState.vehicle64==1||boostState.vehicle64==2;
    hud.overheated=boostState.vehicle64==2;
    // Host route cue only: C88 is the current source map target, not a
    // nearest invented waypoint. Camera framing changes translation only.
    hud.wrongWay=(lapContinuity.prefix.flags10CC&2u)!=0;
    hud.lapCountValid=true;hud.completedLaps=std::min(lapContinuity.state.vehicleCC8,
        platform::maxVehicleWindowCompletedLaps50DS);
    if(raceEnabled){
     hud.raceEnabled=true;
     hud.raceCountdown=race_session50HM::countdownNumber(race);
     hud.raceGo=race.phase==race_session50HM::Phase::Racing&&race.raceSeconds<1.0;
     hud.raceFinished=race.racers[0].finished;hud.raceResultsReady=humanResultsReady;
     if(humanResultsReady){
      require50DU(race.resultsClosed&&race.racers[0].finished,"results overlay requires the sealed human finish");
      hud.resultsPanelVisible=true;
      hud.resultsFinishPlace=race.racers[0].finishPlace;
      hud.resultsFinishSeconds=race.racers[0].finishSeconds;
      hud.resultsSelectedAction=resultsUi.selectedAction();
      hud.resultsConfirming=resultsUi.confirming();hud.resultsConfirmAccept=resultsUi.confirmAccept();
     }
     hud.raceLap=race_session50HM::displayedLap(race,0);
     hud.raceLapTarget=race.config.lapTarget;hud.racePlace=race.racers[0].place;
     hud.raceRacerCount=race.racerCount;
     if(mode1Enabled&&lapContinuity.timingInitialized){
      // Read the committed native fields, never HOST raceSeconds/finishSeconds.
      // The one-human manager closes on finish and leaves CB8 at that boundary.
      // Renderer omits CB0 after finish because it has restarted the next lap.
      hud.raceClocksValid=true;
      hud.raceElapsedSeconds=lapContinuity.timing.timeCB8;
      hud.currentLapSeconds=lapContinuity.timing.timeCB0;
      // Render only source-completed entries after the accepted world/timing
      // commit. CB0 has already restarted the next lap, including at finish.
      hud.completedLapTimesValid=true;
      hud.completedLapTimeCount=std::min(race.racers[0].observed.completedLaps,
          platform::maxVehicleWindowLapTimes50DS);
      for(std::uint32_t lap=0;lap<hud.completedLapTimeCount;++lap)
       hud.completedLapSeconds[lap]=lapContinuity.timing.lapTimesCC0[lap];
      hud.bestLapSeconds=lapContinuity.timing.bestLapCB4;
     }
     require50DU(race.racerCount==racerHudNames.size(),"standings require the complete original race field");
     hud.standingsCount=race.racerCount;hud.standingsPlayerIndex=0;
     for(std::uint32_t place=0;place<race.racerCount;++place){
      const auto racer=race.ordered[place];
      require50DU(racer<race.racerCount,"committed standings racer index rejected");
      const auto& source=race.racers[racer];auto& row=hud.standings[place];
      require50DU(source.observedValid&&source.place==place+1,"standings require committed racer observations");
      std::memcpy(row.displayName,racerHudNames[racer].data(),sizeof(row.displayName));
      row.racerIndex=racer;row.place=source.place;
      row.completedLaps=source.observed.completedLaps;row.finished=source.finished;
     }
    }
    const auto routeTarget=navigationProgress.reset.vehicleC88;
    if(routeTarget<navigationMap->track.count){
     const auto& nextGate=navigationMap->track.nodes[routeTarget];
     const double dx=double(nextGate.position[0])-position.v[0];
     const double dz=double(nextGate.position[2])-position.v[2];
     const double right=-double(viewForwardZ)*dx+double(viewForwardX)*dz;
     const double ahead=double(viewForwardX)*dx+double(viewForwardZ)*dz;
     const double length=std::sqrt(right*right+ahead*ahead);
     if(std::isfinite(length)&&length>0.0001){
      hud.routeValid=true;hud.routeX=static_cast<float>(right/length);
      hud.routeY=static_cast<float>(ahead/length);
     }
    }
    // Original viewport clear consumes the authenticated track Fog.ColorRGB.
    // Unauthored sky tiles remain absent; do not invent lower sky geometry.
    const float* clearColor=skyOwner->clearRgba();
    require50DU(clearColor,"authored track clear color unavailable");
    hudTiming.finish();
    EngineFlareOutput50GV flameOutput{};EngineFlareDiagnostic50GV flameDiagnostic{};
    require50DU(advanceEngineFlare50GV(s,baseTail,coldFlame,
        static_cast<std::uint32_t>(submittedDrawFrames%0xffffffffull),flameOutput,flameDiagnostic),
        "actual engine flame callback rejected");
    for(auto& uv:playerFlameUvs){
     const unsigned side=uv.component==VehicleDrawComponent50CM::leftEngine?0u:1u;
     require50DU(flameOutput.uv[side][1]==0.f&&flameOutput.uv[side][3]==0.f,
         "source flame affine cross terms unsupported");
     uv.uScale=flameOutput.uv[side][0];uv.uOffset=flameOutput.uv[side][2];
     uv.vScale=flameOutput.uv[side][4];uv.vOffset=flameOutput.uv[side][5];
    }
    platform::VehicleWindowFrame50DS renderFrame{info.drawableWidth,info.drawableHeight,
        view.worldViewProjection,clearColor,&cableMesh,&hud};
    if(gcAnimatedWorld){
     require50DU(gcDynamicWindow.bindFrame(renderFrame,*gcAnimatedWorld),"original GC dynamic frame publication failed");
     require50DU(gcAtm.bindFrame(renderFrame),"original GC ATM frame selection rejected");
    }
    renderFrame.materialUvs=playerFlameUvs.data();
    renderFrame.materialUvCount=static_cast<std::uint32_t>(playerFlameUvs.size());
    renderFrame.skyWorldViewProjection=skyView.worldViewProjection;
    renderFrame.worldFogCamera=camera.eye;
    platform::VehicleWindowOpponent50DS opponents[maxHostRaceOpponents50HT]{};
    RacePresentation50HZ nextRacePresentation{};
    if(raceEnabled){
     for(std::size_t racer=1;racer<racerBank.count();++racer){
      const auto index=racer-1;opponentFlamePrepared[index]=false;
      // Source result sealing does not remove a racer from the world.
      // Submit every original pod, including its cables/flames/shadow casters.
      if(race.racers[racer].finished)++nextRacePresentation.submittedFinishedOpponents;
      if(racerBank.active(racer))racerBank.visit(racer,[&](){
       VehicleDrawBatchDiagnostic50CO opponentDiagnostic{};
       require50DU(opponentDraw[index].prepare(s,x.bindings,drawWorkspace,opponentDiagnostic),
           "actual opponent component projection failed");
       require50DU(buildPodCables50FD(s,x.bindings,racerCables[racer]->bindings(),cameraBackward,
           opponentDraw[index].cables,cableWorkspace,cableDiagnostic),"actual opponent cable mesh failed");
       EngineFlareDiagnostic50GV flareDiagnostic{};
       require50DU(advanceEngineFlare50GV(s,baseTail,coldFlame,
           static_cast<std::uint32_t>(submittedDrawFrames%0xffffffffull),opponentFlames[index],flareDiagnostic),
           "actual opponent engine flame callback failed");
       for(auto& uv:opponentUvs[index]){
        const auto side=uv.component==VehicleDrawComponent50CM::leftEngine?0u:1u;
        const auto& source=opponentFlames[index].uv[side];
        require50DU(source[1]==0.f&&source[3]==0.f,"original opponent flame affine cross terms unsupported");
        uv.uScale=source[0];uv.uOffset=source[2];uv.vScale=source[4];uv.vOffset=source[5];
       }
       opponentFlamePrepared[index]=true;
      });
      // A stopped native opponent retains its last complete display
      // batch, never a palette from an abandoned native source-cut state.
      if(!racerBank.active(racer))++nextRacePresentation.retainedStoppedOpponents;
      require50DU(opponentDraw[index].batch(),"opponent has no completed display batch");
      // Keep the original racer archetype and immutable material ownership.
      opponents[nextRacePresentation.submittedOpponents++]={opponentDraw[index].batch(),&opponentDraw[index].cables,
          opponentUvs[index].data(),static_cast<std::uint32_t>(opponentUvs[index].size()),
          static_cast<std::uint32_t>(racer)};
     }
     require50DU(nextRacePresentation.submittedOpponents==raceOpponentCount,
         "native continuation must submit the complete opponent field");
     nextRacePresentation.standingsRows=hud.standingsCount;
     renderFrame.opponentCount=nextRacePresentation.submittedOpponents;
     // Preserve the renderer's null/zero invariant; this eight-racer session
     // retains all seven opponent descriptors after their results seal.
     renderFrame.opponents=renderFrame.opponentCount?opponents:nullptr;
    }
    render_diagnostics50GQ::Scope submitTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::DriverSubmit);
    if(!renderer.submit(next,renderFrame,rendererDiagnostic)){
     reportRenderer50DU(rendererDiagnostic);
     throw std::runtime_error("vehicle submission failed; rejected frame was not swapped");
    }
    if(raceEnabled){
     nextRacePresentation.valid=true;acceptedRacePresentation=nextRacePresentation;
    }
    commitEngineFlare50GV(flameOutput,s,baseTail,coldFlame);
    if(raceEnabled)for(std::size_t racer=1;racer<racerBank.count();++racer)
     if(opponentFlamePrepared[racer-1])racerBank.visit(racer,[&](){
      commitEngineFlare50GV(opponentFlames[racer-1],s,baseTail,coldFlame);
     });
    submitTiming.finish();
    renderTiming.finish();
    if(headlessLive&&capturePending&&captureVisibility){
     phase_timing50GK::Scope captureTiming(phaseTiming,phase_timing50GK::Phase::Capture);
     capture_visibility50fo_fixture::Report visibility{};
     capture_visibility50fo_fixture::Diagnostic visibilityDiagnostic{};
     require50DU(capture_visibility50fo_fixture::inspect(trackMesh,next,camera,
         visibility,visibilityDiagnostic),"capture visibility evidence rejected");
     const auto pointJson=[](const auto& p){return nlohmann::json::array({p.x,p.y,p.z});};
     const auto hitJson=[&](const auto& h){return nlohmann::json{{"hit",h.hit},
         {"t",h.t},{"triangle",h.triangle},{"point",pointJson(h.point)}};};
     nlohmann::json receipt{{"tick",completedFrames},{"wrong_way",hud.wrongWay},{"hud_vertices",rendererDiagnostic.hostHudVertices},{"cockpit_center",pointJson(visibility.cockpitCenter)},
         {"camera_eye",pointJson(visibility.cameraEye)},
         {"camera_to_cockpit",hitJson(visibility.cameraToCockpit)},
         {"cockpit_above",hitJson(visibility.cockpitVertical.above)},
         {"cockpit_below",hitJson(visibility.cockpitVertical.below)},
         {"camera_above",hitJson(visibility.cameraVertical.above)},
         {"camera_below",hitJson(visibility.cameraVertical.below)},
         {"secondary_active",s.bodies.vehicle838},{"lifecycle_mode",s.lifecycle.vehicle83C},
         {"main_contact_ticks",mainContactFrames},{"secondary_contact_ticks",secondaryContactFrames},
         {"triangles",visibility.trianglesScanned}};
     std::cerr<<"capture_visibility "<<receipt.dump()<<'\n';
    }
    // Cloud acceptance timing counts GPU-completed frames, not queued submits.
    // Explicit completion is limited to the headless test host.
    if(headlessLive){
     phase_timing50GK::Scope completionTiming(phaseTiming,phase_timing50GK::Phase::CompletionWait);
     require50DU(renderer.finishSubmittedWork(completionDiagnostic),
        "GPU completion failed during headless timing");
    }
    // Publish CPU-bank ownership only after the renderer accepts this exact batch.
    presenter.published=next;presenter.publishedCpuBank=bank;presenter.hasPublished=true;
    addCounter50DU(presenter.committedFrames);
    if(host.hasWindow()){
     phase_timing50GK::Scope swapTiming(phaseTiming,phase_timing50GK::Phase::CompletionWait);
     if(!host.swapWindow(hostDiagnostic)){
      reportHost50DU(hostDiagnostic);throw std::runtime_error("window swap submission failed");
     }
     addCounter50DU(windowSwaps);
    }
    addCounter50DU(submittedDrawFrames);
    menuImagePublished=false;forceSceneRender=false;
    lastCommittedTick=completedFrames;lastCommittedWrongWay=hud.wrongWay;
    captureCommitted();
   }
   const auto afterWork=monotonicNow50DU();
   if(allAiEnabled&&afterWork-lastAllAiStatusNs>=1000000000ull){
    reportOriginalAiFleet("status");lastAllAiStatusNs=afterWork;
   }
   if(afterWork-lastStatusNs>=5000000000ull){
    if(phaseTiming.enabled){
     for(std::size_t phase=0;phase<phase_timing50GK::phaseCount;++phase){
      const auto& total=phaseTiming.totals[phase];
      std::cerr<<"phase_timing "<<phase_timing50GK::phaseName(static_cast<phase_timing50GK::Phase>(phase))
          <<" calls="<<total.calls<<" wall_calls="<<total.wallCalls<<" cpu_calls="<<total.cpuCalls
          <<" wall_ns="<<total.wallNs<<" cpu_ns="<<total.cpuNs<<" max_wall_ns="<<total.maxWallNs
          <<" cpu_at_max_wall_ns="<<total.cpuAtMaxWallNs<<" cpu_at_max_valid="<<total.cpuAtMaxWallValid
          <<" clock_failure="<<total.clockFailure<<" overflow="<<total.overflow<<'\n';
     }
     phaseTiming.resetInterval();
    }
    if(renderDiagnostics.enabled){
     for(std::size_t phase=0;phase<render_diagnostics50GQ::subphaseCount;++phase){
      const auto& total=renderDiagnostics.totals[phase];
      const auto& scene=renderDiagnostics.sceneAtMaxWall[phase];
      std::cerr<<"render_subphase "<<render_diagnostics50GQ::subphaseName(static_cast<render_diagnostics50GQ::Subphase>(phase))
          <<" calls="<<total.calls<<" wall_calls="<<total.wallCalls<<" cpu_calls="<<total.cpuCalls
          <<" wall_ns="<<total.wallNs<<" cpu_ns="<<total.cpuNs<<" max_wall_ns="<<total.maxWallNs
          <<" cpu_at_max_wall_ns="<<total.cpuAtMaxWallNs<<" cpu_at_max_valid="<<total.cpuAtMaxWallValid
          <<" clock_failure="<<total.clockFailure<<" overflow="<<total.overflow
          <<" scene_at_max_valid="<<scene.valid<<" scene_tick="<<scene.tick
          <<" subnormal_words="<<scene.subnormalWords
          <<" subnormal_local/ready_world/dirty_world="<<scene.localSubnormalWords<<'/'
          <<scene.readyWorldSubnormalWords<<'/'<<scene.dirtyWorldSubnormalWords
          <<" first_source="<<render_diagnostics50GQ::matrixSourceName(scene.firstSource)
          <<" first_node="<<scene.firstNode<<" first_lane="<<scene.firstLane<<" first_bits_u32="<<scene.firstBits
          <<" mxcsr_available="<<scene.mxcsrAvailable<<" mxcsr_u32="<<scene.mxcsr<<'\n';
     }
     const auto& scene=renderDiagnostics.latestScene;
     std::cerr<<"render_census samples="<<renderDiagnostics.sceneSamples
         <<" frames_with_subnormal="<<renderDiagnostics.sceneFramesWithSubnormal
         <<" frames_with_nonfinite="<<renderDiagnostics.sceneFramesWithNonfinite
         <<" max_subnormal_words="<<renderDiagnostics.maxSubnormalWords
         <<" last_tick="<<scene.tick<<" last_valid="<<scene.valid
         <<" nodes="<<scene.nodes<<" words="<<scene.words<<" dirty_nodes="<<scene.dirtyNodes
         <<" last_subnormal_words="<<scene.subnormalWords<<" last_nonfinite_words="<<scene.nonfiniteWords
         <<" overflow="<<renderDiagnostics.overflow<<'\n';
     renderDiagnostics.resetInterval();
    }
    if(gcAnimatedWorld)std::cerr<<"GC WORLD RUNTIME: accepted_service_steps="<<gcAnimatedWorldSteps
        <<" source_application_ms="<<sourceApplicationTimestamp
        <<" windmill_source_time="<<gcAnimatedWorld->state(1).sequence.current34
        <<" opening_active="<<gcAnimatedWorld->state(0).sequence.active90
        <<" compound_owners=2 ordered_parts=4 registered_candidates="<<terrainOwnerCount
        <<" dynamic_draws/vertices/indices="<<rendererDiagnostic.dynamicWorldDraws<<'/'
        <<rendererDiagnostic.dynamicWorldVerticesUploaded<<'/'<<rendererDiagnostic.dynamicWorldIndices
        <<" atm_unique=76 atm_images=35\n";
    const auto& drive=m.components.bindings;
    const auto& cockpitCg=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.model1B4)+1].localC0;
    float bodyDistanceSquared=0.f;
    for(unsigned axis=0;axis<3;++axis){
     const float delta=m.components.root.body.position.v[axis]-s.bodies.secondary.root.body.position.v[axis];
     bodyDistanceSquared+=delta*delta;
    }
    const auto& cockpitPose=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.model1B4)].localC0;
    const auto& leftPose=s.nodes[static_cast<unsigned>(x.bindings.visual.left1BC)].localC0;
    const auto& rightPose=s.nodes[static_cast<unsigned>(x.bindings.visual.right1B8)].localC0;
    float renderDistanceSquared=0.f,enginePairSquared=0.f;
    for(unsigned axis=0;axis<3;++axis){
     const float delta=cockpitPose.v[12+axis]-(leftPose.v[12+axis]+rightPose.v[12+axis])*.5f;
     const float separation=leftPose.v[12+axis]-rightPose.v[12+axis];
     renderDistanceSquared+=delta*delta;enginePairSquared+=separation*separation;
    }
    const double measuredSeconds=double(afterWork-lastStatusNs)*1e-9;
    const double measuredFps=double(submittedDrawFrames-lastStatusRenderCount)/measuredSeconds;
    std::cerr<<(headlessLive?"gpu_completed_fps ":"window_submit_fps ")<<measuredFps
             <<" interval_s "<<measuredSeconds<<" ticks "<<completedFrames<<" render_commits "<<submittedDrawFrames<<" window_swaps "<<windowSwaps
             <<" track_draws/textured/untextured "<<rendererDiagnostic.trackDraws<<'/'
             <<rendererDiagnostic.trackTexturedDraws<<'/'<<rendererDiagnostic.trackUntexturedDraws
             <<" track_cull candidates/retained/culled/fallback "<<rendererDiagnostic.trackCandidateDraws<<'/'
             <<rendererDiagnostic.trackRetainedDraws<<'/'<<rendererDiagnostic.trackCulledDraws<<'/'<<rendererDiagnostic.trackCullingFallback
             <<" cable_draws/vertices/indices "<<rendererDiagnostic.cableDraws<<'/'
             <<rendererDiagnostic.cableVertices<<'/'<<rendererDiagnostic.cableIndices
             <<" camera_adjusted/limited "<<cameraAdjustedFrames<<'/'<<cameraLimitedFrames
             <<" camera_shift "<<cameraFraming.right<<','<<cameraFraming.up<<','<<cameraFraming.retreat
             <<" terrain_queries/hits/supported "<<terrainQueries<<'/'<<terrainHitFrames<<'/'<<terrainSupportedFrames
             <<" terrain_state/hits "<<m.components.probe348.field104<<'/'<<m.components.probe348.field110<<'/'<<m.components.probe348.field114
             <<" hud_draws "<<rendererDiagnostic.hostHudDraws
             <<" position "<<m.components.root.body.position.v[0]<<','
             <<m.components.root.body.position.v[1]<<','<<m.components.root.body.position.v[2]
             <<" left/right drive "<<drive.engineDrive[0]<<'/'<<drive.engineDrive[1]
             <<" brake "<<drive.aeroBrakeEC
             <<" native_cached_speed34_internal "<<m.cachedSpeed34
             <<" boost held/state/active "<<controller.buttons().records[boostButtonCode].held
             <<'/'<<boostState.vehicle64<<'/'<<m.selectedEngineFlags.engine34
             <<" boost heat/duration "<<boostState.heat7C<<'/'<<boostState.duration6C
             <<" slide held/flag/D8 "<<controller.buttons().records[powerslideButtonCode].held
             <<'/'<<((m.controls.player.flagsB9C&4u)!=0)<<'/'<<m.components.bindings.aeroTriggerD8
             <<" aero timers140/144/low "<<m.components.aero.timer140<<'/'<<m.components.aero.timer144<<'/'<<m.components.aero.lowSpeedDc
             <<" repair held/active "<<controller.buttons().records[repairButtonCode].held
             <<'/'<<((m.controls.player.flagsB9C&2u)!=0)
             <<" repair_active_ticks "<<repairActiveTicks
             <<" main/secondary primary zone "<<mainMembership.storage.links[0].zone
             <<'/'<<secondaryMembership.storage.links[0].zone
             <<" main/secondary crossed portals "<<mainCrossedPortals<<'/'<<secondaryCrossedPortals
             <<" host_auto/status/node/error/turn/speed/target/left/right "<<autoDriveEnabled<<'/'
             <<unsigned(courseDiagnostic.status)<<'/'<<courseDiagnostic.node<<'/'<<courseDiagnostic.headingError<<'/'
             <<courseDiagnostic.turn<<'/'<<courseDiagnostic.bodySpeed<<'/'<<courseDiagnostic.targetSpeed<<'/'
             <<courseOutput.left<<'/'<<courseOutput.right
             <<" wrong_way/updates/active_ticks/dot/angle "<<((lapContinuity.prefix.flags10CC&2u)!=0)<<'/'
             <<wrongWayUpdates<<'/'<<wrongWayActiveTicks<<'/'<<wrongWayReceipt.directionDot<<'/'<<wrongWayReceipt.sourceAngle
             <<" host_laps/marker_cursor/callbacks/unavailable "<<lapContinuity.state.vehicleCC8<<'/'
             <<lapContinuity.state.vehicleFF8<<'/'<<lapOwner.completedCallbacks<<'/'<<lapOwner.unavailableSounds
             <<" nav C80/C84/C88/C8C "<<navigationProgress.reset.vehicleC80<<'/'
             <<navigationProgress.vehicleC84<<'/'<<navigationProgress.reset.vehicleC88<<'/'
             <<navigationProgress.reset.vehicleC8C
             <<" anchor_index78 "<<static_cast<unsigned>(
                 navigationMap->nodeIndex78[navigationProgress.reset.vehicleC8C])
             <<" nav map/AI sequential "<<navigationMapSequential<<'/'<<navigationAISequential
             <<" scan "<<navigationMapScan<<'/'<<navigationAIScan
             <<" left/right damage "<<s.fields.damage7D4<<'/'<<s.fields.damage7D0
             <<" cockpit_cg_local "<<cockpitCg.v[12]<<','<<cockpitCg.v[13]<<','<<cockpitCg.v[14]
             <<" secondary_mode "<<s.bodies.vehicle838<<'/'<<s.lifecycle.vehicle83C
             <<" dormant_or_active_body_spacing "<<std::sqrt(bodyDistanceSquared)
             <<" rendered_spacing "<<std::sqrt(renderDistanceSquared)
             <<" engine_pair_spacing "<<std::sqrt(enginePairSquared)
             <<" dropped host ms "<<discardedHostNs/1000000<<'\n';
    if(audioEnabled){
     const auto ai=liveAudio->info();
     std::cerr<<"audio ready/active/starts/mixed_frames/captured_frames/underruns "
         <<liveAudio->bank().ready<<'/'<<liveAudio->activeVoices()<<'/'
         <<liveAudio->counters().starts<<'/'<<ai.mixedFrames<<'/'<<ai.capturedFrames
         <<'/'<<ai.deviceUnderrunFrames<<" clipped_samples="<<liveAudio->clippedSamples()<<'\n';
     std::cerr<<"audio engine_damage updates/draws/dips/returns/completions "
         <<engineDamageUpdates<<'/'<<engineDamageDraws<<'/'<<engineDamageDips<<'/'
         <<engineDamageReturns<<'/'<<engineDamageCompletions
         <<" pitch_scale="<<engineDamage.scale484
         <<" cooldown="<<engineDamage.cooldown474<<'\n';
     std::cerr<<"audio speech_events/cues/unjoined_announcer "<<speechEvents<<'/'<<speechCues
         <<'/'<<announcerRequestsUnjoined<<'\n';
     if(raceEnabled){const auto& r=raceAudio->counters();
      std::cerr<<"audio race ready/starts/countdown/lap/finish/position/unavailable "
          <<raceAudio->bank().ready<<'/'<<r.starts<<'/'<<r.countdownEvents<<'/'<<r.lapEvents
          <<'/'<<r.finishEvents<<'/'<<r.positionEvents<<'/'<<r.unavailable<<'\n';
      const auto& episode=liveAudio->announcerCounters();
      std::cerr<<"audio shared_Dax starts/begins/ends/completions/preemptions/rejected "
          <<episode.starts<<'/'<<episode.begins<<'/'<<episode.ends<<'/'
          <<episode.completions<<'/'<<episode.preemptions<<'/'<<episode.rejected
          <<" group_failures="<<liveAudio->counters().groupFailures;
      if(const auto* groups=liveAudio->groupsForProviders()){
       std::cerr<<" group_levels=";
       for(const auto level:groups->levels().volume38)std::cerr<<level<<'/';
      }
      std::cerr<<'\n';
      const auto& o=opponentAudio->counters();
      std::cerr<<"audio opponent ready/updates/starts/controls/stops/unavailable/normalized "
          <<opponentAudio->bank().ready<<'/'<<o.updates<<'/'<<o.starts<<'/'<<o.controls
          <<'/'<<o.stops<<'/'<<o.unavailable<<'/'<<o.normalizedUpdates
          <<" rejected="<<o.rejected<<'\n';
     }
     std::cerr<<"audio material ready/starts/controls/stops/unavailable "<<materialAudio->bank().ready
         <<'/'<<materialAudio->counters().starts<<'/'<<materialAudio->counters().controls
         <<'/'<<materialAudio->counters().stops<<'/'<<materialAudio->counters().unavailable<<'\n';
    }
    if(mode1Enabled){
     const auto& dax=sourceAnnouncer->state();const auto& last=sourceAnnouncer->lastRequest();
     std::cerr<<"mode1 world="<<mode1->acceptedWorld<<" phase="<<unsigned(mode1->phase)
         <<" manager_seconds="<<mode1->manager.time30<<" lap_seconds="<<lapContinuity.timing.timeCB0
         <<" best_lap="<<lapContinuity.timing.bestLapCB4<<" fifo_count="<<mode1->events.count150
         <<" fifo_last_ticket="<<mode1->events.lastTicket<<" dax_ready="<<sourceAnnouncer->bank().ready
         <<" dax_last_type="<<last.request.type<<" dax_admitted="<<last.admitted
         <<" dax_started="<<last.started<<" dax_queued="<<last.queued
         <<" dax_pending="<<int(dax.pendingKey19C)<<" dax_episode_ac="<<dax.episodeAC
         <<" dax_delay_b0="<<dax.timerB0<<" dax_priority_b4="<<dax.priorityB4
         <<" music_song="<<raceMusic->currentSong()
         <<" music_starts="<<raceMusic->counters().starts<<" music_active="<<raceMusic->active()<<'\n';
    }
    if(originalAiEnabled&&!allAiEnabled)reportOriginalAiFleet("status");
    if(raceEnabled){
     std::cerr<<"race phase="<<unsigned(race.phase)<<" countdown="<<race_session50HM::countdownNumber(race)
         <<" seconds="<<race.raceSeconds<<" player_place="<<race.racers[0].place<<"/"<<race.racerCount
         <<" opponent_draws="<<rendererDiagnostic.opponentRacersDrawn
         <<" field_racers="<<race.racerCount<<" field_opponents="<<raceOpponentCount
         <<" sealed_finishes="<<race.finishCount
         <<" presentation_valid="<<acceptedRacePresentation.valid
         <<" opponents_submitted="<<acceptedRacePresentation.submittedOpponents
         <<" opponents_submitted_finished="<<acceptedRacePresentation.submittedFinishedOpponents
         <<" opponents_hidden_finished=0"
         <<" opponents_retained_stopped="<<acceptedRacePresentation.retainedStoppedOpponents
         <<" standings_rows="<<acceptedRacePresentation.standingsRows;
     for(std::size_t racer=0;racer<race.racerCount;++racer){const auto& r=race.racers[racer];
      std::cerr<<" racer"<<racer+1<<"["<<racerNames[racer]<<"]="<<r.place<<","<<r.observed.completedLaps<<","<<r.observed.checkpointCursor
          <<","<<r.observed.position[0]<<","<<r.observed.position[2]<<","<<(racer==0||racerBank.active(racer));
     }
     std::cerr<<'\n';
     std::size_t activeNative=0;
     for(std::size_t racer=0;racer<racerBank.count();++racer)
      if(racerBank.active(racer))++activeNative;
     std::cerr<<"finish_continuation player_finished="<<race.racers[0].finished
         <<" player_cd8="<<unsigned(observedNativeFinishFlags[0])
         <<" player_manager_gate="<<unsigned(s.fields.applicationByte58)
         <<" finish_driver="<<humanResultsActive
         <<" finish_driver_ticks="<<finishDriverTicks
         <<" finish_manual_input_ticks="<<finishManualInputTicks
         <<" player_native_ticks="<<observedNativeTicks[0]
         <<" sealed_place="<<race.racers[0].finishPlace
         <<" sealed_time="<<race.racers[0].finishSeconds
         <<" sealed_laps="<<race.racers[0].observed.completedLaps
         <<" results_closed="<<race.resultsClosed
         <<" sealed_finish_count="<<race.finishCount
         <<" results_countdown="<<humanResultsCountdown
         <<" host_results_ready="<<humanResultsReady
         <<" active_native="<<activeNative
         <<" opponents_submitted="<<acceptedRacePresentation.submittedOpponents
         <<" standings_rows="<<acceptedRacePresentation.standingsRows;
     for(std::size_t racer=0;racer<racerBank.count();++racer)
      std::cerr<<" racer"<<racer<<"_native_ticks="<<observedNativeTicks[racer]
          <<" racer"<<racer<<"_cd8="<<unsigned(observedNativeFinishFlags[racer])
          <<" racer"<<racer<<"_live_laps="<<race.racers[racer].live.completedLaps
          <<" racer"<<racer<<"_x="<<race.racers[racer].live.position[0]
          <<" racer"<<racer<<"_z="<<race.racers[racer].live.position[2];
     std::cerr<<'\n';
     reportRacerContacts();
    }
    lastStatusNs=afterWork;lastStatusRenderCount=submittedDrawFrames;
   }
   // Account for work already spent, requesting at most4ms active sleep
   // before polling again rather than sleeping a whole tick. Native/GL
   // work and OS scheduling can delay the next poll; no latency guarantee.
   const auto workNs=afterWork>=now?afterWork-now:0;
   if(plan.waitNanoseconds>workNs){
    phase_timing50GK::Scope sleepTiming(phaseTiming,phase_timing50GK::Phase::HostSleep);
    std::this_thread::sleep_for(std::chrono::nanoseconds(
        std::min(maximumSleepNs,plan.waitNanoseconds-workNs)));
   }
  }
  require50DU(terminalInputError==0,"live input stream failed");
 }catch(...){closeWindow();throw;}
 require50DU(closeWindow(),"window/renderer cleanup reported a failure");
 if(raceEnabled)reportRacerContacts();
 std::cout<<(restartRequested?"Development session restarting: ticks=":"Development drive stopped: ticks=")<<completedFrames
          <<" render_commits="<<submittedDrawFrames<<" window_swaps="<<windowSwaps<<" native_calls="<<nativeCalls
          <<" collected_calls="<<collectedCalls<<" peak_collected="<<peakCollected
          <<" main_contact_ticks="<<mainContactFrames<<" secondary_contact_ticks="<<secondaryContactFrames
          <<" repair_active_ticks="<<repairActiveTicks
          <<" main_primary_zone="<<mainMembership.storage.links[0].zone
          <<" secondary_primary_zone="<<secondaryMembership.storage.links[0].zone
          <<" main_crossed_portals="<<mainCrossedPortals<<" secondary_crossed_portals="<<secondaryCrossedPortals
          <<" nav_C80="<<navigationProgress.reset.vehicleC80<<" nav_C84="<<navigationProgress.vehicleC84
          <<" nav_C88="<<navigationProgress.reset.vehicleC88<<" nav_C8C="<<navigationProgress.reset.vehicleC8C
          <<" nav_anchor_index78="<<static_cast<unsigned>(
              navigationMap->nodeIndex78[navigationProgress.reset.vehicleC8C])
          <<" nav_updates="<<navigationUpdates<<" nav_gate_skips="<<navigationGateSkips
          <<" nav_CA0_copies="<<navigationCA0Copies
          <<" nav_map_sequential="<<navigationMapSequential<<" nav_AI_sequential="<<navigationAISequential
          <<" nav_map_scan="<<navigationMapScan<<" nav_AI_scan="<<navigationAIScan
          <<" controller_presses="<<controllerPresses<<" controller_releases="<<controllerReleases
          <<" effect_commands="<<effectCommands<<" discarded_host_ns="<<discardedHostNs<<'\n';
 lapContinuity.state.vehicleCD8=s.fields.impactFlagsCD8;
 std::cout<<"Host lap state: completed="<<lapContinuity.state.vehicleCC8<<" marker_cursor="
          <<lapContinuity.state.vehicleFF8<<" callbacks="<<lapOwner.completedCallbacks
          <<" unavailable_sound_calls="<<lapOwner.unavailableSounds<<'\n';
 return checkpointRequested?SessionExit50DU::Checkpoint:(restartRequested?SessionExit50DU::Restart:SessionExit50DU::Quit);
}
int main(int argc,char**argv){try{
 const bool headlessLive=argc==4&&std::string(argv[2])=="--headless-live";
 if(argc!=2&&!headlessLive){
  std::cerr<<"usage: terrain_interactive_drive50du CORPUS_JSON [--headless-live CAPTURE_PPM]\n"
             "Run from the repository root. RR_RACE_MODE=1 (default with fully prepared providers); "
             "RR_RACE_MODE=2 selects the existing HOST audio/timing fallback. W/S: left throttle/brake; "
             "Up/Down: right throttle/brake; Space: boost; R: hold repair; Left Shift: powerslide; Backspace/checkpoint: current checkpoint; F5/restart: new grid session; Escape: menu; quit: exit. RR_START_MENU=1/0 overrides initial front-menu display.\n";
  return 2;
 }
 // Every return destroys the complete old session, including native state and
 // asset owners. Only an explicit host request enters another cold setup.
 recovery50FI::Plan activeCheckpoint{},nextCheckpoint{};bool useCheckpoint=false;
 lap_drive50GJ::Continuity lapContinuity{};
 // One monotonic backend epoch; each cold native session explicitly resets
 // its shared selector lazy slot alongside the freshly constructed physics RNG.
 HostMonotonicClock50ID applicationClock{};
 const char* startMenuSetting=std::getenv("RR_START_MENU");
 require50DU(!startMenuSetting||std::string(startMenuSetting)=="0"||std::string(startMenuSetting)=="1",
     "RR_START_MENU must be 0 or 1");
 bool startAtMenu=startMenuSetting?std::string(startMenuSetting)=="1":!headlessLive;
 std::uint32_t selectedRacer60=1; // Persist only explicit next-session front-menu selection.
 // In-memory host settings only. Checkpoint recovery retains effective levels;
 // explicit Start/Restart alone commits the requested configuration.
 auto selectedAudioSettings=rr::game::freshOriginalAudioSettings50IC();
 auto effectiveAudioSettings=selectedAudioSettings;
 // Resolve the process default once. Pending menu selection never mutates the
 // environment or rebinds a live world. Unknown/unbound metadata still fails.
 const char* initialTrackError=nullptr;
 const auto* initialTrack=track_session_assets50IG::select(std::getenv("RR_TRACK"),initialTrackError);
 require50DU(initialTrack,initialTrackError?initialTrackError:"original track admission failed");
 require50DU(track_session_assets50IG::codeEquals(initialTrack->world->code,"TA")||
     track_session_assets50IG::codeEquals(initialTrack->world->code,"GC"),"track is not in the explicit menu admission list");
 auto effectiveTrack=track_session_assets50IG::codeEquals(initialTrack->world->code,"GC")?
     rr::platform::HostTrack50IM::GC:rr::platform::HostTrack50IM::TA;
 auto selectedTrack=effectiveTrack;
 const char* incompleteWorldSetting=std::getenv("RR_GC_INCOMPLETE_WORLD");
 const char* selectedModeSetting=std::getenv("RR_RACE_MODE");
 const rr::platform::HostTrackAdmission50IM trackAdmission{
     incompleteWorldSetting&&std::string(incompleteWorldSetting)=="1",
     selectedModeSetting&&std::string(selectedModeSetting)=="2"};
 std::uint64_t sessionIndex=0;
 for(;;){
  const auto exit=runSession50DU(argv[1],headlessLive,headlessLive?argv[3]:nullptr,
      useCheckpoint?&activeCheckpoint:nullptr,nextCheckpoint,lapContinuity,sessionIndex,
      applicationClock,startAtMenu,selectedRacer60,effectiveAudioSettings,selectedAudioSettings,
      effectiveTrack,selectedTrack,trackAdmission);
  if(exit==SessionExit50DU::Quit)break;
  const char* originalAiSetting=std::getenv("RR_ORIGINAL_AI_FIRST_SLICE");
  const char* allAiSetting=std::getenv("RR_ORIGINAL_AI_ALL_RACERS");
  if((originalAiSetting&&std::string(originalAiSetting)=="1")||
     (allAiSetting&&std::string(allAiSetting)=="1")){
   std::cerr<<"ORIGINAL AI session-rebuild boundary: exit="<<static_cast<unsigned>(exit)
       <<" session="<<sessionIndex<<"; optional first slice permits one cold session.\n";
   require50DU(false,"original AI first slice rejected session rebuild");
  }
  if(exit==SessionExit50DU::Restart){effectiveAudioSettings=selectedAudioSettings;effectiveTrack=selectedTrack;}
  require50DU(sessionIndex<std::numeric_limits<std::uint64_t>::max()-1,"audio session index exhausted");
  ++sessionIndex;
  startAtMenu=false; // An explicit restart/start immediately enters its fresh session.
  const char* raceSetting=std::getenv("RR_RACE");
  const bool restartingRace=!(raceSetting&&std::string(raceSetting)=="0");
  useCheckpoint=exit==SessionExit50DU::Checkpoint&&!restartingRace;
  if(restartingRace&&exit==SessionExit50DU::Checkpoint)
   std::cerr<<"Native checkpoint rebuild restarts the entire host race, all lap owners and countdown.\n";
  if(useCheckpoint){activeCheckpoint=nextCheckpoint;std::cerr<<"Starting source-derived host checkpoint session.\n";}
  else {lapContinuity={};std::cerr<<"Starting a fresh authored-grid session.\n";}
 }
 return 0;
}catch(const std::exception&e){std::cerr<<"interactive drive: "<<e.what()<<'\n';return 1;}}
