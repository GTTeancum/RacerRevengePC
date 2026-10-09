#include "runtime_allocations50fl.h"
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_contact_bank50hw_fixture.h"
#include "native_racer_rig50hw_fixture.h"
#include "native_racer_handling50hw_fixture.h"
#include "original_racer_visual50hw_fixture.h"
#include "native_racer_cables50hw_fixture.h"
#include "native_racer_grid50hn_fixture.h"
#include "native_racer_draw50hn_fixture.h"
#include "race_session50hm_fixture.h"
#include "race_audio50hq_fixture.h"
#include "wrong_way_progress50go.h"
#include "host_course_driver50gk.h"
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
#include "track_start50ad.h"
#include "track_navigation50eu.h"
#include <memory>
#include <cstdlib>
#include <chrono>
#include <cmath>
#include <thread>
#include <limits>
#include <stdexcept>
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
namespace recovery50FI=checkpoint_recovery50fi_fixture;
enum class SessionExit50DU { Quit, Restart, Checkpoint };
SessionExit50DU runSession50DU(const char* corpusPath,bool headlessLive,const char* capturePath,
    const recovery50FI::Plan* checkpointInput,recovery50FI::Plan& checkpointOutput,
    lap_drive50GJ::Continuity& lapContinuity,std::uint64_t sessionIndex){
 require50DU(std::fesetround(FE_TONEAREST)==0,"round-to-nearest setup failed");
 const char* raceSetting=std::getenv("RR_RACE");
 const bool raceEnabled=!(raceSetting&&std::string(raceSetting)=="0");
 const char* disableRacerContacts=std::getenv("RR_DISABLE_RACER_CONTACTS");
 const bool racerContactsEnabled=raceEnabled&&
     !(disableRacerContacts&&std::string(disableRacerContacts)=="1");
 constexpr std::uint32_t raceRacerCount=rr::game::defaultHostRaceRacers50HT;
 constexpr std::uint32_t raceOpponentCount=raceRacerCount-1;
 static_assert(maxHostRaceOpponents50HT==rr::platform::maxVehicleWindowOpponents50DS);
 static_assert(maxHostRaceRacers50HT<=native_racer_contact50HW::maxRacers);
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
 for(const auto&row:corpus["cases"])if(row["name"]=="podboxm.col-explicit37-authored-placements-forward")chosen=&row;
 require50DU(chosen,"authored placement case present");const auto&row=*chosen;const auto&qj=row["query"];
 auto shape=cold::shape(corpus["shapes"][qj["shape"].get<std::size_t>()]);
 std::vector<cold::Mesh> meshes;meshes.reserve(corpus["meshes"].size());for(const auto&j:corpus["meshes"])meshes.push_back(cold::mesh(j));
 auto qfilter=cold::filter(qj["filter"]);Type2StaticQuery49X query{};query.resource=&shape.view;query.filter=&qfilter;query.queryScene.value=77;
 query.collisionTypeTag=qj["collision_tag"];query.groupTypeTag=qj["group_tag"];query.callbackEnabled=qj["callback"];
 cold::words(query.copied40.data(),qj["copied40"]);cold::words(query.copied50.data(),qj["copied50"]);
 query.skipMask=row["masks"][0];query.dynamicMask=row["masks"][1];query.materialMask=row["masks"][2];
 std::vector<std::string> names(row["candidates"].size());for(std::size_t i=0;i<names.size();++i)names[i]=cold::unhex(row["candidates"][i]["name_hex"]);
 std::vector<Type2StaticCandidate49X> candidates;candidates.reserve(row["order"].size());std::size_t stackCount=1;
 for(auto index:row["order"].get<std::vector<std::size_t>>()){
  const auto&j=row["candidates"][index];auto&m=meshes[j["mesh"].get<std::size_t>()];candidates.emplace_back();auto&out=candidates.back();
  out.nodeRadii=m.radii.data();out.nodeRadiusCount=m.radii.size();out.maxDepth=m.maxDepth;out.candidate.hasMaterialScene=j["has_scene"];out.candidate.sceneName=names[index].data();out.candidate.sceneNameBytes=names[index].size();
  auto&v=out.candidate.collision;v.geometry={&m.mesh,cold::matrix(j["world"]),j["id"].get<std::uint32_t>()};v.filter=cold::filter(j["filter"]);v.sphere=cold::sphere(j["sphere"]);v.typeTag=j["type_tag"];
  cold::words(v.copied40.data(),j["copied40"]);cold::words(v.copied50.data(),j["copied50"]);stackCount=std::max(stackCount,1+shape.view.maxDepth+m.maxDepth);
 }
 f::Fixture x(0);auto&s=x.state;auto&m=s.bodies.main;
 // Permanent original-resource owners precede the snapshot bank and all draw
 // publications. Only their immutable borrowed views enter the live arena.
 std::array<std::unique_ptr<native_racer50HW::Rig>,raceRacerCount> racerRigs;
 std::array<std::unique_ptr<native_racer_collision50HW::Owner>,raceRacerCount> racerCollisions;
 std::array<std::unique_ptr<native_racer50HW::Visual>,raceRacerCount> racerVisuals;
 std::array<std::unique_ptr<native_racer50HW::Cables>,raceRacerCount> racerCables;
 std::array<std::string,raceRacerCount> racerNames;
 racerNames[0]="Anakin Skywalker (ep1)";
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
  const auto roster=native_racer50HW::rig_detail::json(rosterRoot/"roster.json");
  require50DU(roster.at("player_index")==0&&roster.at("racers").size()==raceRacerCount,
      "original roster must contain the player and seven original opponents");
  for(std::size_t racer=0;racer<raceRacerCount;++racer){
   const auto& entry=roster.at("racers").at(racer);
   const auto id=entry.at("pod_id").get<std::string>();
   require50DU(id.size()==5&&id.substr(0,3)=="POD"&&
       id[3]>='0'&&id[3]<='9'&&id[4]>='0'&&id[4]<='9'&&
       (racer!=0||id=="POD01"),"original roster identity rejected");
   for(std::size_t previous=0;previous<racer;++previous)
    require50DU(roster.at("racers").at(previous).at("pod_id")!=id,"duplicate original racer");
   if(racer){
    const auto relative=std::filesystem::path(entry.at("path").get<std::string>());
    require50DU(!relative.is_absolute()&&relative.lexically_normal()==relative&&
        relative.string().find("..") == std::string::npos,"original roster path rejected");
    racerRigs[racer]=std::make_unique<native_racer50HW::Rig>(".",rosterRoot/relative);
    require50DU(racerRigs[racer]->podId==id,"original rig and roster identity differ");
   }
   racerCollisions[racer]=std::make_unique<native_racer_collision50HW::Owner>();
   std::string error;
   const auto archive=std::filesystem::path("work/native/project/disc/PODS")/id/(id+".RES");
   require50DU(pod_materials50ed_fixture::detail::sha256(f::readBytes(archive))==
       entry.at("resource_sha256"),"original racer resource archive identity differs");
   const bool collisionReady=racerCollisions[racer]->prepare(
       {archive.string(),"Pods\\Pod"+id.substr(3)+"\\"},endpointPolicy,error);
   require50DU(collisionReady,error.empty()?"original racer collision load failed":error.c_str());
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


 auto ground=authored_runtime_ground50cp_fixture::load({4,16,32,64});
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
 require50DU(candidates.size()==37,"interactive scene requires exactly 37 retained terrain owners");
 std::uint32_t candidateZones[37]{};
 ContactMaterial50HI audioContactMaterials[37]{};
 for(std::size_t k=0;k<candidates.size();++k){const auto&j=row["candidates"][row["order"][k].get<std::size_t>()];const auto name=authored_runtime_ground50cp_fixture::lower(corpus["meshes"][j["mesh"].get<std::size_t>()]["name"].get<std::string>());const authored_runtime_ground50cp_fixture::Owner*match=nullptr;for(const auto&o:ground.owners)if(authored_runtime_ground50cp_fixture::lower(o.name)==name){bool equal=true;for(unsigned n=12;n<15;++n)if(o.instance.world.v[n]!=candidates[k].candidate.collision.geometry.worldMatrix.v[n])equal=false;if(equal){match=&o;break;}}require50DU(match,"actual collider placement matches explicit candidate");candidateZones[k]=match->zone;std::string audioMaterialError;require50DU(rr::assets::readTrackContactMaterial50HI(ground.world,match->zone,match->ordinal,candidates[k].candidate.collision.filter.identity,audioContactMaterials[k],audioMaterialError),audioMaterialError.empty()?"actual contact material mapping failed":audioMaterialError.c_str());auto&radiusOwner=meshes[j["mesh"].get<std::size_t>()];require50DU(radiusOwner.radii.size()==match->mesh.nodes.size(),"actual mesh radius extent");std::vector<std::uint32_t> topology(match->mesh.nodes.size()+match->mesh.faces.size());require50DU(prepareStaticTargetRadii49X(match->mesh,radiusOwner.radii.data(),radiusOwner.radii.size(),{topology.data(),topology.size()},radiusOwner.maxDepth)==Type2ResourceStatus49X::Complete,"radii prepared from owned actual mesh");candidates[k].maxDepth=radiusOwner.maxDepth;stackCount=std::max(stackCount,1+shape.view.maxDepth+radiusOwner.maxDepth);auto&v=candidates[k].candidate.collision;v.geometry.mesh=&match->mesh;v.geometry.worldMatrix=match->instance.world;v.filter.categoryA4=match->instance.category;v.sphere={{{match->instance.sphere.center.v[0],match->instance.sphere.center.v[1],match->instance.sphere.center.v[2]}},match->instance.sphere.radius};}
 ground.audit=J{};
 require50DU(setLocalMatrix49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.main190,pose)&&refreshSceneWorld49P({s.nodes,s.sceneNodeCount},x.bindings.lifecycle.main190),"supplied initial pose");f::project(s,x.bindings);
 FreshPodGeometrySources49O sources{};sources.chariot=x.assets.hierarchy[2];sources.rightEngine=x.assets.hierarchy[1];sources.forward=x.assets.motions[4];sources.back=x.assets.motions[5];rr::assets::MotionBindingDiagnostic49O bindingDiagnostic{};
 require50DU(rr::assets::readMotionNodeBindings49O(x.assets.hierarchy[2],reinterpret_cast<const char*>(x.assets.vut[2].data()),x.assets.vut[2].size(),sources.chariotBindings,bindingDiagnostic),"actual motion bindings");
 FreshPodGeometryWorkspace49O geometryWorkspace{};FreshPodGeometryResult49O geometry{};FreshPodGeometryDiagnostic49O geometryDiagnostic{};
 require50DU(buildFreshPodGeometry49O(sources,geometryWorkspace,geometry,geometryDiagnostic),"actual pod geometry");
 const auto phy=runtime_startup50fl::readBytes("work/impactAuthoring49Y/Pod01.phy");const AuthoredSelectionContext49N selection{AuthoredClass49N::A,false,true,false,false,{}};
 const auto authored=runtime_startup50fl::bind(phy,geometry,selection,x.parameters,s,x.selectors,x.secondaryInputs);
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
 const auto tuning=runtime_startup50fl::readBytes("work/impactAuthoring49Y/ta.tun");require50DU(rr::assets::readEnvironmentImpact49Y(reinterpret_cast<const char*>(tuning.data()),tuning.size(),x.parameters.impactTuning,diagnostic),"actual TA impact tuning");x.parameters.impactTuningBound=true;
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

 const J*secondaryCase=nullptr;for(const auto&item:corpus["cases"])if(item["name"]=="chariotm.col-explicit37-authored-placements-forward")secondaryCase=&item;
 require50DU(secondaryCase,"actual chariot query present");const auto&sqj=(*secondaryCase)["query"];
 auto secondaryShape=cold::shape(corpus["shapes"][sqj["shape"].get<std::size_t>()]);auto secondaryFilter=cold::filter(sqj["filter"]);
 Type2StaticQuery49X secondaryQuery{};secondaryQuery.resource=&secondaryShape.view;secondaryQuery.filter=&secondaryFilter;secondaryQuery.queryScene.value=78;
 secondaryQuery.collisionTypeTag=sqj["collision_tag"];secondaryQuery.groupTypeTag=sqj["group_tag"];secondaryQuery.callbackEnabled=sqj["callback"];
 cold::words(secondaryQuery.copied40.data(),sqj["copied40"]);cold::words(secondaryQuery.copied50.data(),sqj["copied50"]);
 secondaryQuery.skipMask=(*secondaryCase)["masks"][0];secondaryQuery.dynamicMask=(*secondaryCase)["masks"][1];secondaryQuery.materialMask=(*secondaryCase)["masks"][2];
 require50DU((*secondaryCase)["order"]==row["order"]&&(*secondaryCase)["candidates"]==row["candidates"],"same explicit ordered terrain inputs");
 StaticSpringState50G secondarySpring{};secondarySpring.records.generatedCapacity88=60;
 SpringRecord48N secondaryRecords[60]{},secondaryQueryRecords[60]{},secondaryScratchRecords[60]{};
 if(raceEnabled)require50DU(matchesEndpointPolicy(secondaryShape.view),
     "retained secondary endpoint-W host policy differs");
 secondaryPairCapacity=std::max(secondaryPairCapacity,secondaryShape.view.rayCount);
 std::size_t targetDepth=0;
 for(const auto& candidate:candidates)targetDepth=std::max(targetDepth,candidate.maxDepth);
 for(const auto& owner:racerCollisions)if(owner){
  native_racer_collision50HW::ScratchRequirements required{};
  require50DU(owner->requirements(targetDepth,required),"original query scratch extent rejected");
  stackCount=std::max({stackCount,required.mainStack,required.secondaryStack});
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
 std::vector<Type3Candidate48P> movementCandidates;movementCandidates.reserve(candidates.size());for(const auto&c:candidates){Type3Candidate48P target{};target.collision=c.candidate.collision;target.shapeType=0;target.hasMaterialScene=c.candidate.hasMaterialScene;target.sceneName=c.candidate.sceneName;target.sceneNameBytes=c.candidate.sceneNameBytes;movementCandidates.push_back(target);}
 MovementRays50N movementInitial,movementScratch,movementDestination;SpringVector4_48N endpoints[2]{};std::uint32_t endpointScenes[2]{};
 std::vector<TerrainCandidateEffects48J> movementScratchEffects(candidates.size()),movementEffects(candidates.size());std::vector<std::uint32_t> movementStack(stackCount);
 Type3QuerySource48P movementSource{movementInitial.source(),endpoints,2,endpointScenes,2,nullptr,0};
 Type3QueryWorkspace48P movementWorkspace{{movementScratch.view(),nullptr,0,movementScratchEffects.data(),movementScratchEffects.size()},movementStack.data(),movementStack.size()};
 Type3QueryBuffers48P movementOutput{movementDestination.view(),nullptr,0,movementEffects.data(),movementEffects.size()};Type3QueryResult48P movementResult{};std::uint32_t movementFlag=0;
 // Explicit supplied cold registration order. Authentic SearchList storage and
 // iteration produce the separate late-movement query view.
 SearchStorage50BS registry{};require50DU(candidates.size()==37,"retained37 terrain owners");
 require50DU(initializeColdSearchStorage50BU(registry,1024)==SearchStorageStatus50BS::Complete,"source factory list capacity");std::uint32_t registeredSlot=0;
 require50DU(adoptFirstSearchBlock50BU(registry,registeredSlot)==SearchStorageStatus50BS::Complete,"caller-owned first registration block");
 for(std::size_t i=0;i<candidates.size();++i){require50DU(selectSearchSlot50BS(registry,0,registeredSlot)==SearchStorageStatus50BS::Complete&&registeredSlot==i+1,"ordered registration slot");require50DU(insertSearchSlot50BS(registry,registeredSlot,static_cast<std::uint32_t>((i+1)*4))==SearchStorageStatus50BS::Complete,"register actual terrain owner handle");}
 Type3Candidate48P registeredMovement[37]{};std::size_t registeredIndices[37]{},registeredCount=0;
 std::size_t probeIndices[37]{},probeCount=0;
 SearchListBlock50BR registrationBlocks[2]{};SearchListInput50BR registrationView{};SearchListCursor50BR registrationCursor{};
 auto gatherRegistered=[&](){if(prepareSearchView50BT(registry,registrationBlocks,2,registrationView)!=SearchStorageStatus50BS::Complete)return false;resetSearchList50BR(registrationCursor,true);registeredCount=0;
  for(;;){std::uint32_t id=0;if(nextSearchList50BR(registrationView,registrationCursor,id)!=SearchListStatus50BR::Complete)return false;if(!id)break;if((id&3)||id/4<1||id/4>candidates.size()||registeredCount>=37)return false;auto index=id/4-1;registeredIndices[registeredCount]=index;registeredMovement[registeredCount]=movementCandidates[index];++registeredCount;}return true;};

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
 // Terrain refs1..37 stay fixed; the two live spring query owners are38/39.
 // They use their own E0 chains over the same persistent zone registries.
 constexpr CollectionRef50BX mainQueryRef=38,secondaryQueryRef=39,terrainQueryRef=40;
 constexpr std::size_t collectionObjectCount=40,zoneCount=9;
 require50DU(ground.world.zones.size()==zoneCount,"retained nine-zone world");
 CollectionObject50BX collectionObjects[collectionObjectCount]{};
 CollectionObjectState50BX collectionSource[collectionObjectCount]{},collectionScratch[collectionObjectCount]{},collectionFirst[collectionObjectCount]{},collectionSecond[collectionObjectCount]{};
 SearchStorage50BS zoneRegistries[18]{};ZoneRegistration50CJ zoneRecords[collectionObjectCount]{};
 ZoneRegistrationScratch50CJ zoneScratch{};ZoneLifecycle50CL zoneLifecycle{};
 SearchListBlock50BR zoneBlocks[36]{};
 CollectionTree50BX collectionTrees[18]{};
 CollectionTreeState50BX treeSource[18]{},treeScratch[18]{},treeFirst[18]{},treeSecond[18]{};
 CollectionZone50BX collectionZones[zoneCount]{};
 const ZoneLifecycleStorage50CL zoneStorage{zoneRegistries,18,zoneRecords,collectionObjectCount,{collectionTrees,18,collectionZones,zoneCount,zoneBlocks,36}};
 require50DU(initializeZoneLifecycle50CL(zoneLifecycle,zoneStorage,zoneCount,collectionObjectCount,1024,zoneScratch)==SearchStorageStatus50BS::Complete,"persistent zone lifecycle initialization");
 ZoneRegistrationCommand50CQ zoneEvents[128]{};ZoneRegistrationQueue50CQ zoneQueue{zoneEvents,128,0,0};
 for(unsigned i=0;i<37;++i){require50DU(enqueueZoneRegistration50CQ(zoneQueue,{ZoneRegistrationOperation50CQ::Set,i+1,candidateZones[i],ZoneMethod50CL::First})==ZoneQueueStatus50CQ::Complete,"queue authored zone owner registration");}
 const auto coldRegistrations=drainZoneRegistrations50CQ(zoneQueue,zoneLifecycle,zoneScratch,128);
 require50DU(coldRegistrations.status==ZoneQueueStatus50CQ::Complete&&!coldRegistrations.remaining,
             "apply cold terrain registrations before live query attachment");

 // Exact World48F text is parsed once with the retained binary32 parser.
 // Directed links preserve original portal order and27F3B0 head insertion.
 const auto authoredPortals=loadLivePortalInputs50EP(ground.world);
 std::vector<CollectionRef50BX> portalHeads(zoneCount);
 std::vector<LiveZonePortalLink50EN> portalLinks(authoredPortals.size()*2);
 std::vector<PortalGeometry50EO> portalGeometry(authoredPortals.size());
 LiveZoneWorld50EN liveWorld{};
 require50DU(prepareLiveZoneWorld50EN(authoredPortals.data(),authoredPortals.size(),zoneCount,
     {portalHeads.data(),portalHeads.size(),portalLinks.data(),portalLinks.size(),
      portalGeometry.data(),portalGeometry.size()},liveWorld)==LiveZoneStatus50EN::Complete,
     "prepare authored live portal world");
 // Cold original TunnelTrack owners, separate from the grid-start projection.
 // Share the existing serial source-libm owner with49T; keep every node/NextN.
 auto navigationMap=std::make_unique<TrackNavigation50EU>();
 auto navigationAI=std::make_unique<TrackNavigation50EU>();
 const char* autoSetting=std::getenv("RR_AUTODRIVE");
 bool autoDriveEnabled=autoSetting&&std::string(autoSetting)=="1";
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
 auto materialAudio=std::make_unique<material_audio50HI::Provider>();
 const char* audioSetting=std::getenv("RR_AUDIO");
 const bool audioEnabled=!(audioSetting&&std::string(audioSetting)=="0");
 rr::platform::AudioOutputDiagnostic50HD audioDiagnostic{};
 PodSpeechCounts50HG speechCounts{};PodSpeechState50HG speechState{};
 DamageSpeechEvent50HG pendingSpeech{};
 float playerSpeechChance=0.f;
 SpeechPosition50HG audioListener{};bool audioListenerReady=false;
 std::uint64_t speechEvents=0,speechCues=0,announcerRequestsUnjoined=0;
 if(audioEnabled){
  const char* suppliedAudioRoot=std::getenv("RR_AUDIO_ROOT");
  const std::filesystem::path audioRoot=suppliedAudioRoot?suppliedAudioRoot:
      "work/native/project/disc/AUDIO";
  std::string audioError;
  require50DU(liveAudio->prepare("work/native/project/disc",audioRoot,audioError),
      audioError.empty()?"native audio bank preparation failed":audioError.c_str());
  auto* materialMixer=liveAudio->mixerForMaterialOwner();
  if(raceEnabled)require50DU(materialMixer&&raceAudio->prepare("work/native/project/disc",
      audioRoot,*materialMixer,audioError),
      audioError.empty()?"original race audio preparation failed":audioError.c_str());
  require50DU(materialMixer&&materialAudio->prepare("work/native/project/disc",audioRoot,
      *materialMixer,64,audioError),audioError.empty()?"native material audio preparation failed":audioError.c_str());
  require50DU(pod_speech50HG::readPod01Counts(speechCounts,audioError),
      audioError.empty()?"Pod01 speech authoring failed":audioError.c_str());
  require50DU(pod_speech50HG::readPlayerSpeechChance(playerSpeechChance,audioError,
      audioRoot/"SPCHPCT.INI"),audioError.empty()?"actual speech probability missing":audioError.c_str());
  require50DU(initializePodSpeech50HG(speechCounts,s.noise.sharedRandomState,speechState)==
      PodSpeechStatus50HG::Complete,"native speech cursor initialization failed");
  const auto& bank=liveAudio->bank();
  std::cerr<<"Audio bank ready/missing/unsupported/invalid "<<bank.ready<<'/'<<bank.missing
      <<'/'<<bank.unsupported<<'/'<<bank.invalid<<" of "<<bank.clips.size()<<'\n';
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
  const auto bytes=runtime_startup50fl::readBytes("work/native/project/disc/TRACKS/TA.RES");
  require50DU(rr::assets::decodeResource48C(bytes,navigationArchive,error),
      "decode actual TA navigation archive");
  auto scratch=std::make_unique<TrackNavigationScratch50EU>();
  auto readNavigation=[&](const char* member,TrackNavigation50EU& destination){
   std::vector<std::uint8_t> text;
   require50DU(rr::assets::extractResMember48C(navigationArchive,member,text,error),
       "extract actual M/A navigation spline");
   require50DU(reportNavigation(readTrackNavigation50EU(
       reinterpret_cast<const char*>(text.data()),text.size(),s.animationShared.libm,
       *scratch,destination,navigationDiagnostic),member),
       "prepare source navigation gates");
   if(&destination==navigationAI.get())
    require50DU(rr::host::readCourseSpeedLimits50GK(reinterpret_cast<const char*>(text.data()),
        text.size(),destination.track.count,courseLimits,courseDiagnostic),"actual ATA speed metadata rejected");
   require50DU(reportNavigation(bindTrackNavigationZones50EU(
       liveWorld,destination,navigationDiagnostic),member),
       "bind source navigation portal indices");
  };
  readNavigation("tracks\\mta.spl",*navigationMap);
  require50DU(prepareLapProgressTrack50GF(*scratch,lapOwner.track,lapOwner.diagnostic),
      "actual authored lap progress markers rejected");
  readNavigation("tracks\\ata.spl",*navigationAI);
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
 require50DU(lap_sound50GH::prepareDisabledFromPod01(lapOwner.sound,lapOwner.provider,lapSoundError),
     "actual event sound owner preparation failed");
 std::cerr<<"HOST LAPS uses authored markers and explicit config mode2; lap audio provider="
     <<(audioEnabled?"native PCM (see asset availability)":"explicitly disabled")
     <<"; source wrong-way progression enabled; race-finish rules remain unjoined.\n";
 TrackNavigationProgress50EU navigationProgress{};
 initializeTrackNavigationProgress50EU(navigationProgress);
 //20F5C0..20F5C8 initializes CA0=(0,0,0,1). The first gate query therefore
 // takes1A0940's explicit zero-XZ miss; do not seed this with a guessed pose.
 float navigationCA0[4]{0.f,0.f,0.f,1.f};
 const CollectionRef50BX initialCollisionZone=checkpointInput?checkpointInput->primaryZone:1;
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
 const auto membershipCapacity=authoredPortals.size()+1;
 std::vector<CollectionZoneLink50BX> mainMembershipLinks(membershipCapacity),secondaryMembershipLinks(membershipCapacity),membershipScratchLinks(membershipCapacity);
 std::vector<LiveZoneEntry50EN> mainMembershipEntries(membershipCapacity),secondaryMembershipEntries(membershipCapacity),membershipScratchEntries(membershipCapacity);
 std::vector<LiveZonePortalLink50EN> portalScratchLinks(portalLinks.size());
 SearchStorage50BS membershipScratchRegistries[18]{};
 LiveZoneWorkspace50EN membershipWork{membershipScratchRegistries,18,
     membershipScratchLinks.data(),membershipScratchEntries.data(),membershipCapacity,
     portalScratchLinks.data(),portalScratchLinks.size(),{}};
 LiveZoneMembership50EN mainMembership{},secondaryMembership{},terrainMembership{};
 std::vector<CollectionZoneLink50BX> terrainMembershipLinks(membershipCapacity);
 std::vector<LiveZoneEntry50EN> terrainMembershipEntries(membershipCapacity);
 const LiveZoneStorage50EN terrainMembershipStorage{terrainMembershipLinks.data(),terrainMembershipEntries.data(),membershipCapacity};
 live_terrain_probe50fr::Owner terrainLiveOwner{};
 require50DU(live_terrain_probe50fr::initialize(terrainLiveOwner,terrainOwner.initial.segment),"initialize actual single-pair terrain owner");
 const LiveZoneStorage50EN mainMembershipStorage{mainMembershipLinks.data(),mainMembershipEntries.data(),membershipCapacity};
 const LiveZoneStorage50EN secondaryMembershipStorage{secondaryMembershipLinks.data(),secondaryMembershipEntries.data(),membershipCapacity};
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
 LiveZoneBounds50EN mainInitialBounds{},secondaryInitialBounds{};
 require50DU(prepareQueryCache(query,s.nodes[static_cast<int>(x.bindings.lifecycle.main190)].world100,mainInitialBounds)&&
     prepareQueryCache(secondaryQuery,s.nodes[static_cast<int>(x.bindings.lifecycle.physical19C)].world100,secondaryInitialBounds),
     "source initial spring matrix and sphere preparation");
 // Retained grid-start1AE578..1AE594 ->2169C0 supplies initial zone1.
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
 TerrainCandidate48J* terrainTargets[37]{};
 for(unsigned i=0;i<37;++i)terrainTargets[i]=&candidates[i].candidate.collision;
 TerrainCandidate48J terrainSelected[37]{};
 require50DU(live_terrain_probe50fr::reserveSelected(terrainSelected,37,terrainTargets,37),"reserve terrain query filter storage");
 TerrainCandidateEffects48J terrainScratchEffects[37]{},terrainEffects[37]{};
 std::vector<std::uint32_t> terrainStack(stackCount);
 live_terrain_probe50fr::Context terrainContext{zoneLifecycle,liveWorld,terrainMembership,membershipWork,
     terrainQueryRef,terrainTargets,37,terrainSelected,37,
     {terrainScratchEffects,37,terrainStack.data(),terrainStack.size()},terrainEffects,37};
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

 std::uint32_t scratchIds[64]{},firstIds[64]{},secondIds[64]{},mergeScratch[64]{},mergedIds[64]{};
 std::size_t resolveScratch[64]{};CandidateHandle50CB ownerHandles[37]{};
 for(std::size_t i=0;i<37;++i)ownerHandles[i]={candidates[i].candidate.collision.filter.identity,i};
 std::sort(std::begin(ownerHandles),std::end(ownerHandles),[](const auto&a,const auto&b){return a.identity<b.identity;});
 auto collectionBuffer=[](CollectionObjectState50BX*objects,CollectionTreeState50BX*trees,std::uint32_t*ids){CandidateCollectionBuffers50BX b{};b.objects=objects;b.objectCapacity=collectionObjectCount;b.trees=trees;b.treeCapacity=18;b.identities=ids;b.identityCapacity=64;return b;};
 CandidateCollectionInput50BX collectionInput{};collectionInput.objects=collectionObjects;collectionInput.objectCount=collectionObjectCount;
 collectionInput.trees=collectionTrees;collectionInput.treeCount=18;collectionInput.zones=collectionZones;collectionInput.zoneCount=9;
 collectionInput.zoneLinks=mainMembership.storage.links;collectionInput.zoneLinkCount=mainMembership.storage.capacity;collectionInput.zoneHead=1;collectionInput.query=mainQueryRef;
 collectionInput.bounds.shapeTag=2;collectionInput.type2Resource=query.resource;collectionInput.directTypeTag=query.collisionTypeTag;collectionInput.groupTypeTag=query.groupTypeTag;
 CandidateCollectionState50BX collectionInitial{};collectionInitial.objects=collectionSource;collectionInitial.objectCapacity=collectionObjectCount;collectionInitial.trees=treeSource;collectionInitial.treeCapacity=18;
 VehicleCandidateWorkspace50CA collectionWork{};collectionWork.collector.buffers=collectionBuffer(collectionScratch,treeScratch,scratchIds);
 collectionWork.first=collectionBuffer(collectionFirst,treeFirst,firstIds);collectionWork.second=collectionBuffer(collectionSecond,treeSecond,secondIds);
 collectionWork.mergeScratch={mergeScratch,64};collectionWork.merged={mergedIds,64};
 std::uint64_t collectedCalls=0;std::size_t peakCollected=0;
 auto collectRegistered=[&](){
  const auto applied=drainZoneRegistrations50CQ(zoneQueue,zoneLifecycle,zoneScratch,128);
  if(applied.status!=ZoneQueueStatus50CQ::Complete||applied.remaining)return false;
  if(prepareSearchView50BT(registry,registrationBlocks,2,registrationView)!=SearchStorageStatus50BS::Complete)return false;
  //50CL keeps method descriptors current after each explicit registry change.
  for(std::size_t i=0;i<37;++i){const auto&owner=candidates[i].candidate.collision;auto projected=projectCollectionObject50CF(owner.filter,owner.sphere,owner.typeTag,0,0);if(!projected.valid)return false;collectionObjects[i]=projected.object;collectionSource[i]=projected.state;}
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
  if(resolveCandidateHandles50CB(mergedIds,result.count,ownerHandles,37,resolveScratch,64,registeredIndices,37)!=CandidateHandleStatus50CB::Complete)return false;
  if(resolveCandidateHandles50CB(secondIds,result.second.count,ownerHandles,37,resolveScratch,64,probeIndices,37)!=CandidateHandleStatus50CB::Complete)return false;
  probeCount=static_cast<std::size_t>(result.second.count);
  terrainOwner.metadata.filter.flagsF8=collectionSecond[terrainQueryRef-1].flagsF8;
  registeredCount=static_cast<std::size_t>(result.count);peakCollected=std::max(peakCollected,registeredCount);addCounter50DU(collectedCalls);
  for(unsigned i=0;i<37;++i)candidates[i].candidate.collision.filter.flagsF8=collectionSecond[i].flagsF8;
  qfilter.flagsF8=collectionSecond[mainQueryRef-1].flagsF8;
  secondaryFilter.flagsF8=collectionSecond[secondaryQueryRef-1].flagsF8;query.initialQueryCurrentIdentity=result.second.currentIdentity;
  std::memcpy(treeSource,treeSecond,sizeof treeSource);return true;
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
  if(autoDriveEnabled&&(input.down[26]||input.down[22]||input.down[82]||input.down[81])){
   autoDriveEnabled=false;courseState={};courseOutput={};
   std::cerr<<"Host course driver disabled by manual throttle/brake input.\n";
  }
  require50DU(platform::composeKeyboardPacket50DR({true,live},
      {input.down.data(),input.down.size()},keyMapping,
      platform::KeyboardUnavailablePolicy50DR::ReleaseAll,packetTemplate,packet,
      keyboardDiagnostic),"keyboard packet composition failed");
  packetLive=live;
 };

 auto applyCourseInput=[&](){
  rr::host::CourseDriverInput50GK command{};
  command.enabled=autoDriveEnabled;command.elapsedSeconds=.016f;
  command.pose=s.nodes[static_cast<unsigned>(x.bindings.lifecycle.main190)].world100;
  command.worldVelocity=m.components.root.body.velocity;
  command.yawRate=m.components.root.body.angularVelocity.v[1];
  command.previousNode=navigationProgress.vehicleC84;
  command.currentNode=navigationProgress.reset.vehicleC80;
  const bool driving=rr::host::driveCourse50GK(*navigationAI,courseLimits,courseTuning,
      command,courseState,courseOutput,courseDiagnostic);
  require50DU(rr::host::applyCourseThrottlePacket50GK(courseOutput,packet),"host course packet rejected");
  if(!driving&&!autoStopReported){
   autoStopReported=true;
   std::cerr<<"Host course driver stopped: status="<<unsigned(courseDiagnostic.status)
       <<" node="<<courseDiagnostic.node<<" cross_track="<<courseDiagnostic.crossTrack
       <<" stalled_s="<<courseDiagnostic.stalledSeconds<<". Neutral input; manual takeover remains available.\n";
  }
 };

 bool opponentFrame=false,raceSetupFrame=false;
 // This host fallback abandons the failed frame; it never supplies a miss
 // or repairs source list topology. Only the exact retained boundary qualifies.
 auto recoverPortalBoundary=[&](const LiveZoneResult50EN& failure,const char* owner){
  if(failure.status!=LiveZoneStatus50EN::UnsupportedLinkRemoval)return false;
  recoveryPaused=true;
  if(opponentFrame)return true; // Keep the unsupported prefix in this AI slot only.
  recovery50FI::Plan nextCheckpoint{};recovery50FI::Diagnostic recoveryDiagnostic{};
  BodyMatrix48G groundedCheckpoint{};
  const bool prepared=recovery50FI::preparePortalRecovery(*navigationMap,navigationProgress,s,
      x.bindings,failure,ground.lift,liveWorld,nextCheckpoint,recoveryDiagnostic);
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
  in.vehicle60=0xffffffffu; // Unrepresented racer ID; announcer request stays unjoined.
  in.vehicle60Known=false;in.playerSpeechChance138=playerSpeechChance;
  in.config58=s.fields.applicationByte58;
  in.position={m.components.root.body.position.v[0],m.components.root.body.position.v[2]};
  in.listeners={audioListenerReady?&audioListener:nullptr,audioListenerReady?1u:0u};
  return in;
 };
 auto advanceFrame=[&](){
 if(audioEnabled&&!opponentFrame&&!raceSetupFrame)require50DU(advanceSpeechTimers50HG(.016f,lapContinuity.prefix,speechState),
     "native speech countdown failed");
 // The live path does not call fixture/oracle callbacks; retain a bounded
 // diagnostic frame label for any future cold-fixture diagnostics.
 x.frame=static_cast<unsigned>(std::min<std::uint64_t>(completedFrames,
     std::numeric_limits<unsigned>::max()));
 FrameReader49K read{};read.preparedFactor=.016f;read.codes=x.actionCodes;
 read.context.previousFlagsB9C=s.bodies.main.controls.player.flagsB9C;
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
 //228724..228730:28B180 copies only local XYZ from the actual340->288
 // binding, without world-cache refresh, immediately before217DE0 here.
 // The supplied preserving cut covers intervening unrelated source effects.
 require50DU(!m.applicationActive11C&&!s.fields.vehicle3C&&m.domain.sceneAttached,
     "source CA0 capture owner unavailable");
 std::memcpy(navigationCA0,
     s.nodes[static_cast<unsigned>(x.bindings.animation.bodyScene288)].localC0.v+12,12);
 addCounter50DU(navigationCA0Copies);
 PostPlayerAt217DE0_49K post{};post.player=s.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=x.configurationEntryNonzero;
 runNative([&]{return bindVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,x.workspace,x.diagnostic);},"persistent binding");
 const BoostInput50FK boostInput{m.frameFactor,m.components.engine.smoothedDrive[0],
     m.components.engine.smoothedDrive[1],m.cachedSpeed34,m.controls.player.flagsB9C,
     s.fields.vehicle58,2,0,false}; // No D40 audio/control owner in this baseline topology.
 // Native host subscriber at the original boost boundary; this does not claim
 // that the separate original D40 topology guard has been closed.
 if(audioEnabled&&!opponentFrame&&!raceSetupFrame){
  const bool heldBoost=(m.controls.player.flagsB9C&1u)&&!(m.controls.player.flagsB9C&2u);
  const float heating=boostParameters.heatRate88*(boostInput.engine70+boostInput.engine74);
  require50DU(liveAudio->boostAtBoundary(heldBoost,heating,boostState.vehicle64).okay(),
      "source boost audio event rejected");
 }
 runNative([&]{return advancePlayerBoost50FK(boostInput,boostParameters,boostState,
     boostState,boostDiagnostic);},"native boost controller");
 x.selectors.vehicle64=boostState.vehicle64;
 runNative([&]{return graphicsVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::Base2176E0BeforeGraphics),x.workspace,x.diagnostic);},"persistent graphics");
 ImpactConsumerInput49Z consumer{};consumer.vehicle64=boostState.vehicle64;consumer.managerFlags58=2;consumer.profile34=2;consumer.finalDamageThreshold64=1;VehicleImpactEvents49Z consumerEvents{};
 runNative([&]{return consumeVehicleImpacts49Z(s,x.bindings,consumer,consumerEvents,x.workspace,x.diagnostic);},"persistent impact consumers");runNative([&]{return repairVehicleImpacts50A(s,x.bindings,x.workspace,x.diagnostic);},"persistent repair");
 if(audioEnabled&&!opponentFrame&&!raceSetupFrame){
  const float damage[]={s.fields.damage7D0,s.fields.damage7D4,s.fields.damage7D8,s.fields.damage7DC};
  require50DU(liveAudio->repairCompletion(m.controls.player.flagsB9C,s.fields.vehicle3C,damage).okay(),
      "source repair-completion audio rejected");
  require50DU(afterRepairSpeechState50HG(s.fields.damage7D0,s.fields.damage7D4,
      s.fields.damage7DC,lapContinuity.prefix),"native repair speech state rejected");
  const auto produced=produceDamageSpeechEvent50HG(speechInput(),lapContinuity.prefix,pendingSpeech);
  require50DU(produced.status==PodSpeechStatus50HG::Complete,"native damage speech enqueue rejected");
  if(produced.eventProduced)++speechEvents;
 }
 ImpactEffectsInput50B owners{};owners.models[0].node=x.bindings.visual.left1BC;owners.models[1].node=x.bindings.visual.right1B8;owners.models[2].node=owners.models[3].node=SceneNodeIndex49P::none;std::size_t count=0;owners.damage00Matches=namesForEffects.damage00;owners.nameCount=s.sceneNodeCount;for(unsigned side=0;side<2;++side){auto&model=owners.models[side];require50DU(rr::assets::effectModelRoot50BL(namesForEffects,model.node),"bound model name scope");model.flags03=3;model.effect800={nativeEffects[2+side].id,nativeEffects[2+side].state.flags1D4};model.effect804={nativeEffects[4+side].id,nativeEffects[4+side].state.flags1D4};model.effect80C={nativeEffects[6+side].id,nativeEffects[6+side].state.flags1D4};}
 runNative([&]{return effectsVehicleImpacts50O(s,x.bindings,owners,effectsWorkspace,generatedEffects,maxImpactEffectEvents50B,count,x.workspace,x.diagnostic);},"actual effect control bindings");runNative([&]{return consumeEffects(generatedEffects,count);},"consume impact effect controls");
 if(audioEnabled&&!opponentFrame&&!raceSetupFrame){
  MaterialAudioInput50HI materialInput{};
  materialInput.channelArgument=s.impactSupplement.timers820[4]>0.f?2u:3u;
  require50DU(resolveContactMaterial50HI(spring.records.currentCandidate130,
      audioContactMaterials,37,materialInput.contactMaterialA9),"actual audio contact identity unresolved");
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
  if(opponentFrame)return false; // A failed AI never restarts the human session.
  recovery50FI::Plan nextCheckpoint{};recovery50FI::Diagnostic recoveryDiagnostic{};
  BodyMatrix48G groundedCheckpoint{};
  const bool prepared=recovery50FI::prepareDamageRecovery(*navigationMap,navigationProgress,s,
      x.bindings,decision,x.diagnostic,0x3f800000u,0,ground.lift,liveWorld,
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
 runNative([&]{return damageReady;},"ordinary damage decision");PlainEffectOwners50Q plainOwners{};for(unsigned side=0;side<2;++side){for(unsigned array=0;array<3;++array){const auto&owner=nativeEffects[2+array*2+side];plainOwners.slots[array*4+side]={owner.id,owner.state.flags1D4};}}PlainEffectResult50Q plain{};
 runNative([&]{return plainEffectsVehicleTail50Q(s,x.bindings,plainOwners,plain,x.workspace,x.diagnostic);},"plain effects");runNative([&]{return consumeEffects(plain.events,plain.count);},"consume plain effect controls");runNative([&]{return packVehicleFrame50S(s,x.bindings,x.configurationEntryNonzero,x.workspace,x.diagnostic);},"packet");BaseTailBindings50X baseBinding{77,Probe128Binding50X::Null};
 const bool baseAdvanced=advanceVehicleBaseThroughEngine50X(s,x.bindings,clock,
     baseBinding,baseTail,baseGlobals,coldFlame,baseWorkspace,x.workspace,x.diagnostic);
 if(!baseAdvanced&&x.diagnostic.error==VehicleFrameError49W::ExternalCut&&
    x.diagnostic.sourceStage==0x228BBC){
  addCounter50DU(nativeCalls);recoveryPaused=true;
  if(opponentFrame)return false; // A failed AI never restarts the human session.
  recovery50FI::Plan nextCheckpoint{};recovery50FI::Diagnostic recoveryDiagnostic{};
  BodyMatrix48G groundedCheckpoint{};
  const bool prepared=recovery50FI::prepare(*navigationMap,navigationProgress,s,x.bindings,
      x.diagnostic,ground.lift,liveWorld,nextCheckpoint,recoveryDiagnostic);
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
 PostWorld49W world{};world.point=s.bodies.vehicle838?WorldCut49W::First358Secondary36CAnd219420:WorldCut49W::First358And219420;world.effects=contactEffects49W(s);world.applicationGateByte58=2;
 runNative([&]{return produceVehicleForces49W(s,x.bindings,x.parameters,world,x.workspace,x.diagnostic);},"native main forces");
 ResponseStorage50K responseStorage;auto responseWorkspace=responseStorage.view(x.workspace);
 runNative([&]{return respondMainStaticVehicle50G(s,x.bindings,x.parameters,settings,response,x.secondaryInputs,query.queryScene,1,spring,records,40,responseWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual main contact response");
 require50DU(s.phase==(s.bodies.vehicle838?VehicleFramePhase49W::Post368:VehicleFramePhase49W::MainIntegration),"source-selected response phase");

 if(s.bodies.vehicle838){
 ResponseStorage50K secondaryStorage;auto secondaryWorkspace=secondaryStorage.view(x.workspace);
 runNative([&]{return respondSecondaryStaticVehicle50J(s,x.bindings,x.parameters,secondarySettings,secondaryResponse,{78},2,secondarySpring,secondaryRecords,60,secondaryWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual secondary contact and integration");
 require50DU(s.phase==VehicleFramePhase49W::MainIntegration,"secondary response integration phase");
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
  PodEngineParameters50HE engineParameters{}; // Authenticated Pod01's explicit tuning.
  PodEngineInput50HE engineInput{boostState.speedRatio70,1.f,liveAudio->boostPitch()};
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
 auto sampleRaceField=[&](){
  require50DU(race_session50HM::sampleRacer(race,0,observeLiveRacer()),"player race observation rejected");
  for(std::size_t racer=1;racer<racerBank.count();++racer){
   // An abandoned partial frame cannot publish progression or display geometry.
   // Keep the last completed observation for a visibly stopped opponent.
   const auto observed=racerBank.active(racer)?racerBank.visit(racer,observeLiveRacer):
       race.racers[racer].observed;
   require50DU(race_session50HM::sampleRacer(race,static_cast<std::uint32_t>(racer),observed),
       "opponent race observation rejected");
  }
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
  racerBank.seal(raceRacerCount);
  for(std::size_t racer=1;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
   require50DU(completedFrames==0&&m.frameFactor==0.f,"original racer installation requires fresh state");
   const auto& rig=*racerRigs[racer];const auto& collision=*racerCollisions[racer];
   rig.install(s,x.bindings,x.handRecords);
   const auto receipt=native_racer50HW::bindColdHandling(rig.phy,rig.hierarchy[1],
       rig.hierarchy[0],rig.geometry,collision.mainTarget().source,selection,x,
       {boostParameters,boostCoolingCF8,settings,response,secondarySettings,secondaryResponse});
   native_racer50HW::placeColdHandlingRoots(x);
   for(unsigned component=0;component<3;++component)rebaseModels[component]=rig.rebaseModels[component];
   require50DU(rebaseColdPodScene50FB({s.nodes,s.sceneNodeCount},rebaseModels,
       rebaseWorkspace,rebaseBounds,rebaseDiagnostic),"original racer cold model rebase failed");
   rig.buildEffectNames(s,namesForEffects);
   query.resource=&collision.main();secondaryQuery.resource=&collision.secondary();
   terrain_probe_authoring50fq_fixture::Owner ownProbe{};std::string probeError;
   const bool probeReady=terrain_probe_authoring50fq_fixture::prepare(ownProbe,query,x.parameters,
       rig.phy,terrainBindings,probeError,collision);
   require50DU(probeReady,probeError.empty()?"original racer probe authoring failed":probeError.c_str());
   for(unsigned axis=0;axis<3;++axis)x.parameters.bodies.main.probeLocal140[axis]=ownProbe.lowerEndpoint140.v[axis];
   x.parameters.bodies.main.probeLocal140[3]=1.f;
   const bool probeInstalled=terrain_probe_authoring50fq_fixture::installProbeCold(terrainOwner,
       terrainLiveOwner,ownProbe,probeError);
   require50DU(probeInstalled,probeError.empty()?"original racer cold probe installation failed":probeError.c_str());
   // This slot has never advanced. Replace inherited enabled-probe construction
   // with captured pre-enable construction, then use its own native constructor.
   s.terrainProbe=freshTerrainProbe;s.bodies.main.components.probe348=freshMainProbe;
   s.bodies.main.domain.probe100=freshProbe100;
   require50DU(enableVehicleTerrain50FP(s,x.bindings,terrainOwner.initial,
       terrainOwner.follow,x.workspace,x.diagnostic),"enable original racer terrain probe");
   racerVisuals[racer]=std::make_unique<native_racer50HW::Visual>(".",
       "evidence/originalRoster50HW/original-eight-visuals",rig);
   racerCables[racer]=std::make_unique<native_racer50HW::Cables>(rig,s,x.bindings);
   coldFlame=native_racer50HW::buildColdFlames(rig,racerVisuals[racer]->drawAssets,x.bindings);
   racerNames[racer]=racerVisuals[racer]->displayName;
   contactBindings[racer]={&collision,settings,response};
   std::cerr<<"ORIGINAL RACER "<<racer<<": "<<racerNames[racer]<<" ["<<rig.podId
       <<"] nodes="<<s.sceneNodeCount<<" mass="<<x.parameters.bodies.main.body.mass
       <<" lift="<<receipt.liftHeight<<" main_rays="<<collision.main().rayCount
       <<" secondary_rays="<<collision.secondary().rayCount<<'\n';
  });
#include "native_racer_grid50hn.inc"
  for(std::size_t racer=1;racer<racerBank.count();++racer)placeOpponentAtGrid50HN(racer);
  if(racerContactsEnabled){
   racerContactOwner=std::make_unique<native_racer_contact50HW::Owner>();
   std::string error;
   const bool contactsReady=racerContactOwner->prepare(contactBindings.data(),contactBindings.size(),error);
   require50DU(contactsReady,error.empty()?"original racer contact world preparation failed":error.c_str());
  }
  for(std::size_t racer=1;racer<racerBank.count();++racer){
   const auto index=racer-1;auto& visual=*racerVisuals[racer];
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
  }
  // One neutral, unscored native setup tick establishes actual graphics/ground
  // before countdown rendering. After this all physics freezes until GO.
  const platform::WindowInput50DQ noKeys{};
  raceSetupFrame=true;
  auto initializeGraphics=[&](){
   composePacket(false,noKeys);sampleController();
   require50DU(advanceFrame(),"native race grid setup tick rejected");
   require50DU(lapContinuity.state.vehicleCC8==0,"cold race setup unexpectedly completed a lap");
  };
  for(std::size_t racer=1;racer<racerBank.count();++racer){
   opponentFrame=true;
   try{racerBank.visit(racer,initializeGraphics);}
   catch(const std::exception& error){
    opponentFrame=false;
    throw std::runtime_error("Original racer setup failed for "+racerNames[racer]+": "+error.what());
   }
   opponentFrame=false;
  }
  initializeGraphics();raceSetupFrame=false;
  require50DU(race_session50HM::initialize(race,*navigationMap,lapOwner.track,raceRacerCount),
      "authored race route/markers rejected");
  std::uint32_t gridOrder[maxHostRaceRacers50HT]{};
  for(std::uint32_t index=0;index<raceOpponentCount;++index)gridOrder[index]=index+1;
  gridOrder[raceOpponentCount]=0;
  require50DU(race_session50HM::seedGridOrder(race,gridOrder,raceRacerCount),
      "explicit host starting-grid order rejected");
  sampleRaceField();require50DU(race_session50HM::commitFrame(race,0),"race grid observations rejected");
  std::cerr<<"HOST RACE: "<<raceRacerCount<<" distinct original racers, 3 laps, 3-second countdown. "
      "Field count matches the original normal race; the grounded starting grid and opponent controls are host-authored. "
      "Original AI remains unjoined. "
      "Each racer owns "<<racerBank.bytesPerRacer()<<" bytes of native state.\n";
  std::cerr<<"RACER CONTACTS: "<<(racerContactsEnabled?"enabled":"disabled")
      <<"; each original main-body geometry and authored response, host world-frame scheduling. "
        "Empty-root material0 is a deterministic host policy; original padding parity is unclaimed. "
        "Secondary dynamic collision, damage attribution and opponent event audio remain unjoined.\n";
 }

 // Actual retained TA visual mesh, already placed by the49G/48F exporter.
 // This visual set is broader than the37-collider diagnostic physics scope.
 rr::assets::Mesh48B trackMesh;std::string trackLoadError;
 const bool trackLoaded=rr::assets::loadMesh48B(
     "evidence/visual49G/export-placed-normal/ta-world.rrmesh",trackMesh,trackLoadError);
 require50DU(trackLoaded,trackLoadError.empty()?"retained TA visual mesh load failed":trackLoadError.c_str());
 namespace trackTextures=track_materials50fa_fixture;
 auto trackMaterialOwner=std::make_unique<trackTextures::Owner>();
 trackTextures::Request trackMaterialRequest{};
 trackMaterialRequest.root="evidence/visual49G";
 trackMaterialRequest.exportReport="evidence/visual49G/export-placed-normal/export-report.json";
 trackMaterialRequest.choice={1,1,rr::assets::MaterialTextureRequirement50EA::optionalUntextured,
     trackTextures::MipMode::largestRetained,0};
 trackMaterialRequest.coldSourceByteCapacity=32u*1024u*1024u;
 trackMaterialRequest.sourceArchive="work/native/project/disc/TRACKS/TA.RES";
 trackMaterialRequest.decodeSourceTexture=&source_texture50fc_fixture::decodeSourceTexture;
 trackMaterialRequest.allowStaticAnimationFrame=true;
 trackMaterialRequest.textureBudgets={32,1024,8u*1024u*1024u,8u*1024u*1024u,1024};
 trackTextures::Diagnostic trackMaterialDiagnostic{};
 require50DU(trackMaterialOwner->load(trackMaterialRequest,trackMesh,trackMaterialDiagnostic),
     trackMaterialDiagnostic.message.empty()?"TA material load failed":trackMaterialDiagnostic.message.c_str());
 std::vector<rr::assets::WorldNormal50HK> trackNormals;
 track_normals50hk_fixture::Diagnostic trackNormalDiagnostic{};
 require50DU(track_normals50hk_fixture::load(trackMaterialRequest.root,
     trackMaterialRequest.exportReport,trackMesh,trackNormals,trackNormalDiagnostic),
     trackNormalDiagnostic.message.empty()?"TA authored normal load failed":trackNormalDiagnostic.message.c_str());
 std::cerr<<"TA source normals "<<trackNormalDiagnostic.normals
     <<" zero/fallback "<<trackNormalDiagnostic.zeroNormals<<'\n';
 auto skyOwner=std::make_unique<sky50gw_fixture::Owner>();
 sky50gw_fixture::Request skyRequest{};
 skyRequest.archive="work/native/project/disc/TRACKS/TA.RES";
 skyRequest.textureRoot="evidence/sky50GW";
 sky50gw_fixture::Diagnostic skyDiagnostic{};
 require50DU(skyOwner->load(skyRequest,skyDiagnostic),
     skyDiagnostic.message.empty()?"actual TA sky load failed":skyDiagnostic.message.c_str());
 // Host is declared first so renderer destruction always precedes its context.
 platform::GameHost50ER host;
 platform::MaterialTextureGl50EB gpuTextures,trackGpuTextures,cableGpuTextures,skyGpuTextures;
 std::array<platform::MaterialTextureGl50EB,raceOpponentCount> opponentGpuTextures,opponentCableGpuTextures;
 platform::MaterialTextureGlDiagnostic50EB textureDiagnostic{};
 platform::VehicleWindowGl50DS renderer;
 platform::GameHostDiagnostic50ER hostDiagnostic{};
 platform::VehicleWindowDiagnostic50DS rendererDiagnostic{},completionDiagnostic{};
 bool restartRequested=false;
 auto closeWindow=[&](){
  bool clean=true;
  if(!liveAudio->close(audioDiagnostic)){
   std::cerr<<"audio close failed "<<static_cast<unsigned>(audioDiagnostic.error)<<'\n';clean=false;
  }
  if(renderer.ownsResources()&&!renderer.close(rendererDiagnostic)){
   reportRenderer50DU(rendererDiagnostic);clean=false;
  }
  // Never destroy a context beneath unresolved GPU ownership. Destructors may
  // retry cleanup in the same correct order; neither silently leaks resources.
  if(renderer.ownsResources())return false;
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
  windowRequest.title=raceEnabled?"Racer Revenge | Native race | W/S + Up/Down | F5 restart | Esc quits":
      "Racer Revenge native development drive | W/S + Up/Down | F5 restart | Esc quits";
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
  const auto* textureBank=materialOwner->bank();require50DU(textureBank,"cold texture bank missing");
  std::vector<platform::MaterialTextureSampler50EB> samplers(textureBank->textureCount(),
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
  std::vector<platform::VehicleWindowMaterialBinding50DS> materialBindings[3];
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
  renderRequest.mode=platform::VehicleWindowMode50DS::callerMaterialTextures;
  renderRequest.track.mesh=&trackMesh;
  renderRequest.track.sourceNormals=trackNormals.data();
  renderRequest.track.sourceNormalCount=static_cast<std::uint32_t>(trackNormals.size());
  renderRequest.track.maximumNormalBytes=2u*1024u*1024u;
  // Bounded host rendering optimization; an explicit debugging opt-out keeps
  // the same scene and physics available for comparisons.
  const char* disableCulling=std::getenv("RR_DISABLE_TRACK_CULLING");
  renderRequest.track.enableFrustumCulling=!(disableCulling&&std::string(disableCulling)=="1");
  renderRequest.track.maximumVertexBytes=4u*1024u*1024u;
  renderRequest.track.maximumIndexBytes=2u*1024u*1024u;
  const auto* trackBank=trackMaterialOwner->bank();
  const auto* trackSpans=trackMaterialOwner->spans();
  require50DU(trackBank&&trackSpans,"TA material publication missing");
  std::vector<platform::MaterialTextureSampler50EB> trackSamplers(trackBank->textureCount(),
      {platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat});
  auto trackTextureRequest=textureRequest;
  trackTextureRequest.samplers=trackSamplers.data();
  trackTextureRequest.samplerCount=static_cast<std::uint32_t>(trackSamplers.size());
  trackTextureRequest.maximumGpuBytes=8u*1024u*1024u;
  require50DU(trackGpuTextures.create(trackTextureRequest,*trackBank,textureDiagnostic),
      "actual TA texture upload failed");
  std::vector<platform::VehicleWindowTrackSpan50DS> trackBindings;
  trackBindings.reserve(trackSpans->size());
  for(const auto& span:*trackSpans){
   platform::VehicleWindowTrackSpan50DS binding{};
   binding.indexStart=span.indexStart;binding.indexCount=span.indexCount;
   binding.vertexStart=span.vertexStart;binding.vertexCount=span.vertexCount;
   binding.material.mode=span.textureIndex==trackTextures::unresolved?
       platform::VehicleWindowMaterialMode50DS::untextured:
       platform::VehicleWindowMaterialMode50DS::requiredTexture;
   binding.material.textureIndex=span.textureIndex;
   const auto* trackSelections=trackMaterialOwner->selections();
   require50DU(trackSelections&&span.materialSelection<trackSelections->size(),"TA material selection missing");
   material_alpha50gx_fixture::Policy alpha{};
   require50DU(material_alpha50gx_fixture::select(
       (*trackSelections)[span.materialSelection],alpha),"TA material alpha policy mismatch");
   using AlphaBlend=material_alpha50gx_fixture::Blend;
   binding.material.blend=alpha.blend==AlphaBlend::additive?
       platform::VehicleWindowBlend50DS::additive:alpha.blend==AlphaBlend::sourceAlpha?
       platform::VehicleWindowBlend50DS::sourceAlpha:platform::VehicleWindowBlend50DS::opaque;
   binding.material.depthWrite=alpha.depthWrite;
   binding.material.alphaScale=alpha.alphaScale;
   // These authenticated beam placements explicitly disable source lighting.
   binding.material.unlit=alpha.unlit;
   binding.discardZeroAlpha=alpha.discardZeroAlpha;
   trackBindings.push_back(binding);
  }
  const auto* cableBank=cableOwner->bank();require50DU(cableBank,"cable bank missing");
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
  renderRequest.track.vertexColorScale=2.f; // Explicit host exposure of retained TA colors.
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
    const auto ownSamplers=visual.samplers();
    auto ownRequest=textureRequest;
    ownRequest.samplers=ownSamplers.data();ownRequest.samplerCount=static_cast<std::uint32_t>(ownSamplers.size());
    ownRequest.maximumGpuBytes=8u*1024u*1024u;
    require50DU(opponentGpuTextures[index].create(ownRequest,visual.materials,textureDiagnostic),
        "original opponent texture upload failed");
    const auto* ownCableBank=racerCables[racer]->bank();
    require50DU(ownCableBank&&opponentCableGpuTextures[index].create(cableTextureRequest,
        *ownCableBank,textureDiagnostic),"original opponent cable texture upload failed");
    auto& archetype=racerArchetypes[index];archetype.assets=&visual.drawAssets;
    archetype.materials=visual.windowMaterials(&opponentGpuTextures[index]);
    archetype.cables.textures=&opponentCableGpuTextures[index];archetype.cables.textureIndex=0;
   }
   renderRequest.racerArchetypes=racerArchetypes.data();renderRequest.racerArchetypeCount=raceOpponentCount;
  }
  if(!renderer.create(renderRequest,drawAssets,rendererDiagnostic)){
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
  auto captureCommitted=[&](){
   if(!capturePending||!presenter.hasPublished)return;
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
   std::cerr<<"capture_state wrong_way="<<lastCommittedWrongWay<<" tick="<<lastCommittedTick<<'\n';
   if(captureFirstWrongWay&&lastCommittedWrongWay)capturedFirstWrongWay=true;
   capturePending=false;
  };
  std::uint64_t lastStatusNs=monotonicNow50DU();
  std::uint64_t lastStatusRenderCount=0;
  std::cerr<<"Interactive development client: W/S left engine throttle/brake; "
               "Up/Down right engine throttle/brake; W+Up both throttle; Space boosts; "
               "S+Down both brake; hold R to repair; Left Shift powerslides; Backspace resets to checkpoint; F5 restarts the grid; Escape quits. "
               "Opposite keys on one axis center.\n"
               "Preset4 is explicit. R maps native repair flag69C button4; "
               "Space maps native boost flag698 button5; Left Shift maps powerslide flag6B4 button7; other action buttons stay released. 16ms host ticks, "
               "maximum 4 catch-up ticks. Focus loss/minimize pauses.\n"
               "Actual textured pod meshes and source TA material spans with host lighting; "
               "37 colliders with live main/secondary portal memberships across nine zones.\n"
           <<"Navigation: "<<navigationMap->track.count<<" map / "
           <<navigationAI->track.count<<" AI nodes; original gates and portal indices bound. "
             "Live progression is tracked; automatic source-derived host checkpoint recovery is enabled.\n"
           <<"Repair CF4 "<<s.fields.repairCF4
           <<" from actual Pod01 selected authoring; profile overwrite is explicitly absent.\n";
  if(autoDriveEnabled)std::cerr<<"HOST course-check autopilot enabled; pace="
      <<(courseTuning.authoredSpeedOnly?"authored-limits":"conservative-host")
      <<" speed-limit-to-body-scale="<<courseTuning.speedLimitToBodyScale
      <<"; not original AI. Manual W/S/Up/Down takes over. No automatic reset.\n";
  if(headlessLive)std::cerr<<"Stdin host commands include capture, checkpoint, restart, and quit. "
      "Restart destroys this session and repeats the authored-grid cold setup.\n";
  for(;;){
   phase_timing50GK::Scope inputTiming(phaseTiming,phase_timing50GK::Phase::Input);
   if(!host.poll(hostInput,hostDiagnostic)){
    reportHost50DU(hostDiagnostic);throw std::runtime_error("window event pump failed");
   }
   inputTiming.finish();
   if(hostInput.inputError){terminalInputError=hostInput.inputError;std::cerr<<"stdin input error "<<terminalInputError<<'\n';}
   // Escape is a host quit event, never a native vehicle action button.
   if(input.quitRequested||input.pressed[41]||input.down[41]){
    composePacket(false,input);sampleController(); // Route a final held release before cleanup.
    break;
   }
   if(hostInput.restartRequested){
    composePacket(false,input);sampleController();
    restartRequested=true;
    break;
   }
   if(hostInput.checkpointRequested){
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
         x.bindings,ground.lift,liveWorld,manualCheckpoint,manualDiagnostic);
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
   if(recoveryPaused){
    // Never prepare or submit a draw batch from the interrupted native state.
    // Readback uses only the renderer's already committed private target.
    if(audioEnabled)liveAudio->pause(true);
    captureCommitted();hostClock.pause(now);
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    continue;
   }
   const auto& info=host.info();
   const bool oversize=info.drawableWidth>maximumWidth||info.drawableHeight>maximumHeight;
   if(oversize&&!oversizeReported){
    std::cerr<<"Paused: drawable exceeds 1280x720; resize smaller to resume.\n";
    oversizeReported=true;
   }else if(!oversize&&oversizeReported){
    std::cerr<<"Drawable fits again.\n";oversizeReported=false;
   }
   const bool paused=!input.focused||input.minimized||input.eventBudgetExhausted||
       info.drawableWidth<=0||info.drawableHeight<=0||oversize;
   composePacket(!paused&&(!raceEnabled||race_session50HM::controlsAllowed(race,0)),input);
   if(paused){
    if(audioEnabled)liveAudio->pause(true);
    if(!wasPaused)sampleController(); // Publish the declared release-all sample.
    hostClock.pause(now);wasPaused=true;
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
    continue;
   }
   if(wasPaused){hostClock.pause(now);wasPaused=false;if(audioEnabled)liveAudio->pause(false);}
   // Manual input samples every host poll. The optional host course driver
   // instead emits one ordinary packet per actual 16 ms simulation step.
   if(!autoDriveEnabled)sampleController();
   platform::FixedStepPlan50DT plan{};
   require50DU(hostClock.plan(now,plan),"fixed-step host clock planning failed");
   addCounter50DU(discardedHostNs,plan.discardedNanoseconds);
   for(std::uint32_t step=0;step<plan.steps;++step){
    if(raceEnabled&&!race_session50HM::advanceCountdown(race,.016)){
     require50DU(race.error==race_session50HM::Error::None,"race countdown rejected");
     if(audioEnabled){
      liveAudio->pause(false);
      require50DU(raceAudio->observe(race).accepted,"countdown/result audio observation rejected");
      // Countdown and completed-result display have a real host clock even
      // while native vehicle bodies are held. Advance the shared mixer ONCE.
      if(liveAudio->outputOpen())require50DU(liveAudio->committedStep(audioDiagnostic),
          "native countdown/result audio step failed");
     }
     continue;
    }
    const bool playerRunning=!raceEnabled||race_session50HM::controlsAllowed(race,0);
    if(audioEnabled)liveAudio->pause(false);
    // Recompose at GO so a key held during countdown enters this exact native
    // frame. Finished vehicles are held without modifying their source state.
    if(playerRunning){
     composePacket(true,input);
     if(autoDriveEnabled)applyCourseInput();
     sampleController();
    }
    const auto nativeFrameAllocations=::allocations;
    phase_timing50GK::Scope physicsTiming(phaseTiming,phase_timing50GK::Phase::Physics);
    const bool frameAdvanced=!playerRunning||advanceFrame();
    if(frameAdvanced)require50DU(::allocations==nativeFrameAllocations,
        "successful player physics frame allocated C++ storage");
    if(frameAdvanced&&raceEnabled){
     const platform::WindowInput50DQ noKeys{};
     for(std::size_t racer=1;racer<racerBank.count();++racer){
      if(!racerBank.active(racer)||!race_session50HM::controlsAllowed(race,static_cast<std::uint32_t>(racer)))continue;
      opponentFrame=true;
      try {
       const bool advanced=racerBank.step(racer,true,[&](){
        composePacket(true,noKeys);applyCourseInput();sampleController();
        const auto beforeOpponentFrame=::allocations;
        const bool completed=advanceFrame();
        if(completed)require50DU(::allocations==beforeOpponentFrame,
            "successful opponent physics frame allocated C++ storage");
        return completed;
       });
       if(!advanced)std::cerr<<racerNames[racer]<<" stopped at a native source boundary; other racers continue.\n";
      }catch(const std::exception& error){
       racerBank.stop(racer);
       std::cerr<<racerNames[racer]<<" stopped: "<<error.what()<<". Other racers continue.\n";
      }
      opponentFrame=false;
     }
     // Exactly one host contact pass after all successful native participants
     // are Ready, before race observations and the single world audio mix.
     // A failed/nonfinished eligibility boundary never borrows a stale snapshot.
     if(racerContactsEnabled){
      std::array<bool,native_racer_contact50HW::maxRacers> contactParticipant{};
      std::array<native_racer_contact50HW::Pair,
          native_racer_contact50HW::maxRacers*(native_racer_contact50HW::maxRacers-1)> contactOrder{};
      std::size_t contactPairCount=0;
      for(std::size_t racer=0;racer<racerBank.count();++racer)
       contactParticipant[racer]=racerBank.active(racer)&&!race.racers[racer].finished&&
           race_session50HM::controlsAllowed(race,static_cast<std::uint32_t>(racer));
      for(std::size_t primary=0;primary<racerBank.count();++primary)if(contactParticipant[primary])
       for(std::size_t other=0;other<racerBank.count();++other)
        if(primary!=other&&contactParticipant[other])contactOrder[contactPairCount++]={primary,other};
      if(contactPairCount){
       native_racer_contact50HW::Receipt contactReceipt{};
       const auto beforeRacerContacts=::allocations;
       const auto contactStatus=native_racer_contact50HW::applyAtWorldBoundary(
           racerBank,*racerContactOwner,s,x.bindings,x.parameters,racerContactIds,racerContactMaterials,
           contactOrder.data(),contactPairCount,contactReceipt);
       racerContactTotals.lastStatus=contactStatus;
       if(contactStatus!=native_racer_contact50HW::Status::Complete){
        addCounter50DU(racerContactTotals.failures);
        std::cerr<<"RACER CONTACT FAILURE: player_tick="<<completedFrames<<" ordered_pairs="<<contactPairCount
            <<" status="<<static_cast<unsigned>(contactStatus)<<"; rejected body outputs were not committed.\n";
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
     sampleRaceField();
     require50DU(race_session50HM::commitFrame(race,.016),"completed race world frame rejected");
     if(race.racers[0].finished&&playerRunning){
      std::cerr<<"RACE FINISH: player place="<<race.racers[0].finishPlace
          <<"/"<<race.racerCount<<" time="<<race.racers[0].finishSeconds<<" seconds.\n";
      // Explicit host finish policy stops held engine/warning loops once,
      // before the genuine observed finish edge starts its authored speech.
      if(audioEnabled)liveAudio->resetVoices();
     }
     if(audioEnabled)require50DU(raceAudio->observe(race).accepted,
         "committed race audio observation rejected");
    }
    physicsTiming.finish();
    if(!frameAdvanced){
     if(audioEnabled)liveAudio->pause(true);
     composePacket(false,input);sampleController();
     hostClock.pause(monotonicNow50DU());wasPaused=true;
     break;
    }
    // Source player speech runs only on the human's accepted native step.
    if(audioEnabled&&playerRunning){
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
    // All racer/audio subscribers feed one mixer. Continue the finish clip and
    // any remaining field while the human is finished; never mix per opponent.
    if(audioEnabled&&liveAudio->outputOpen())
     require50DU(liveAudio->committedStep(audioDiagnostic),"native mixed audio world step failed");
   }
   if(recoveryPaused){captureCommitted();if(checkpointRequested)break;continue;}

   if(captureFirstWrongWay&&!capturedFirstWrongWay&&(lapContinuity.prefix.flags10CC&2u))capturePending=true;
   if(completedFrames&&(plan.steps||input.resized||!presenter.hasPublished||capturePending)){
    phase_timing50GK::Scope renderTiming(phaseTiming,phase_timing50GK::Phase::RenderPrepareSubmit);
    {
     render_diagnostics50GQ::Scope censusTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::Census);
     renderDiagnostics.observeScene(s,completedFrames);
    }
    render_diagnostics50GQ::Scope bankTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::DrawBank);
    const auto bank=static_cast<std::uint8_t>(presenter.hasPublished?(presenter.publishedCpuBank^1u):0u);
    VehicleDrawBatchInput50CO drawInput{};
    require50DU(drawAssets.batchInput(bank,ModelPalettePhase49S::selectedRenderPreparation,
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
    runNative([&]{return buildPodCables50FD(s,x.bindings,cableBindings,cameraBackward,
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
     hud.raceFinished=race.racers[0].finished;
     hud.raceLap=race_session50HM::displayedLap(race,0);
     hud.raceLapTarget=race.config.lapTarget;hud.racePlace=race.racers[0].place;
     hud.raceRacerCount=race.racerCount;
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
    platform::VehicleWindowMaterialUv50DS flameUvs[2]{};
    for(unsigned side=0;side<2;++side){
     auto& uv=flameUvs[side];
     uv.component=side?VehicleDrawComponent50CM::rightEngine:VehicleDrawComponent50CM::leftEngine;
     uv.materialIndex=1;
     require50DU(flameOutput.uv[side][1]==0.f&&flameOutput.uv[side][3]==0.f,
         "source flame affine cross terms unsupported");
     uv.uScale=flameOutput.uv[side][0];uv.uOffset=flameOutput.uv[side][2];
     uv.vScale=flameOutput.uv[side][4];uv.vOffset=flameOutput.uv[side][5];
    }
    platform::VehicleWindowFrame50DS renderFrame{info.drawableWidth,info.drawableHeight,
        view.worldViewProjection,clearColor,&cableMesh,&hud};
    renderFrame.materialUvs=flameUvs;renderFrame.materialUvCount=2;
    renderFrame.skyWorldViewProjection=skyView.worldViewProjection;
    platform::VehicleWindowOpponent50DS opponents[maxHostRaceOpponents50HT]{};
    if(raceEnabled){
     for(std::size_t racer=1;racer<racerBank.count();++racer){
      const auto index=racer-1;opponentFlamePrepared[index]=false;
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
      // A retired opponent retains its last complete display batch, never a
      // palette projected from an abandoned native source-cut state.
      require50DU(opponentDraw[index].batch(),"opponent has no completed display batch");
      opponents[index]={opponentDraw[index].batch(),&opponentDraw[index].cables,
          opponentUvs[index].data(),static_cast<std::uint32_t>(opponentUvs[index].size()),
          static_cast<std::uint32_t>(racer)};
     }
     renderFrame.opponents=opponents;renderFrame.opponentCount=raceOpponentCount;
    }
    render_diagnostics50GQ::Scope submitTiming(renderDiagnostics,render_diagnostics50GQ::Subphase::DriverSubmit);
    if(!renderer.submit(next,renderFrame,rendererDiagnostic)){
     reportRenderer50DU(rendererDiagnostic);
     throw std::runtime_error("vehicle submission failed; rejected frame was not swapped");
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
    lastCommittedTick=completedFrames;lastCommittedWrongWay=hud.wrongWay;
    captureCommitted();
   }
   const auto afterWork=monotonicNow50DU();
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
     std::cerr<<"audio speech_events/cues/unjoined_announcer "<<speechEvents<<'/'<<speechCues
         <<'/'<<announcerRequestsUnjoined<<'\n';
     if(raceEnabled){const auto& r=raceAudio->counters();
      std::cerr<<"audio race ready/starts/countdown/lap/finish/position/unavailable "
          <<raceAudio->bank().ready<<'/'<<r.starts<<'/'<<r.countdownEvents<<'/'<<r.lapEvents
          <<'/'<<r.finishEvents<<'/'<<r.positionEvents<<'/'<<r.unavailable<<'\n';
     }
     std::cerr<<"audio material ready/starts/controls/stops/unavailable "<<materialAudio->bank().ready
         <<'/'<<materialAudio->counters().starts<<'/'<<materialAudio->counters().controls
         <<'/'<<materialAudio->counters().stops<<'/'<<materialAudio->counters().unavailable<<'\n';
    }
    if(raceEnabled){
     std::cerr<<"race phase="<<unsigned(race.phase)<<" countdown="<<race_session50HM::countdownNumber(race)
         <<" seconds="<<race.raceSeconds<<" player_place="<<race.racers[0].place<<"/"<<race.racerCount
         <<" opponent_draws="<<rendererDiagnostic.opponentRacersDrawn;
     for(std::size_t racer=0;racer<race.racerCount;++racer){const auto& r=race.racers[racer];
      std::cerr<<" racer"<<racer+1<<"["<<racerNames[racer]<<"]="<<r.place<<","<<r.observed.completedLaps<<","<<r.observed.checkpointCursor
          <<","<<r.observed.position[0]<<","<<r.observed.position[2]<<","<<(racer==0||racerBank.active(racer));
     }
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
             "Run from the repository root. W/S: left throttle/brake; "
             "Up/Down: right throttle/brake; Space: boost; R: hold repair; Left Shift: powerslide; Backspace/checkpoint: current checkpoint; F5/restart: new grid session; Escape/quit: quit.\n";
  return 2;
 }
 // Every return destroys the complete old session, including native state and
 // asset owners. Only an explicit host request enters another cold setup.
 recovery50FI::Plan activeCheckpoint{},nextCheckpoint{};bool useCheckpoint=false;
 lap_drive50GJ::Continuity lapContinuity{};
 std::uint64_t sessionIndex=0;
 for(;;){
  const auto exit=runSession50DU(argv[1],headlessLive,headlessLive?argv[3]:nullptr,
      useCheckpoint?&activeCheckpoint:nullptr,nextCheckpoint,lapContinuity,sessionIndex);
  if(exit==SessionExit50DU::Quit)break;
  require50DU(sessionIndex<std::numeric_limits<std::uint64_t>::max()-1,"audio session index exhausted");
  ++sessionIndex;
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
