#define RR_FIXTURE_THREAD_LOCAL_ALLOCATIONS 1
// One bounded, headless use of the same textured vehicle + retained TA renderer
// as interactive50DU. Cold fixture loading is still present; this is neither a
// packaged game nor a replacement for the frozen offscreen50DH baseline.
#include "../../platform/linux/native/offscreen_gl_context50dc.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "render_allocation_stats50eh.h"
#include "../../platform/linux/native/vehicle_view_camera50dg.h"
#include "control_presets49d.h"
#include "pod_materials50ed_fixture.h"
#include "../../platform/linux/native/material_texture_functions50ee.h"
#include "vehicle_present50da.h"
#include "controller_ingress50cx.h"
#include "vehicle_draw_submit50cs.h"
#include "vehicle_draw_assets50cr.h"
#include "zone_registration_events50cq.h"
#include "zone_lifecycle50cl.h"
#include "global_zone_chain50ck.h"
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
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include "vehicle_base_finish50y.h"
#include "pod_flame50w.h"
#include "../../evidence/frameImpact49Y/impact_join49y_fixture.h"
#include "../../evidence/frameBridge49W/connected/authored_inputs49w.h"
#include "type2_fixture_io50i.h"
#include "vehicle_static_query50h.h"
#include "movement_static_query50l.h"
#include "vehicle_secondary_contact50j.h"
#include "vehicle_static_response50g.h"
#include "../../src/assets/spring_authoring50f.h"
namespace f=frame49w_test;namespace c=connected49r;namespace cold=cold50i;
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
void require50EK(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
void addCounter50EK(std::uint64_t& counter, std::uint64_t amount = 1) noexcept {
    const auto room = std::numeric_limits<std::uint64_t>::max() - counter;
    counter += std::min(room, amount);
}
J allocationTotals50EK(const rr::test::RenderAllocationTotals50EH& totals) {
    return {{"frames",totals.frames},{"valid_frames",totals.validFrames},
        {"invalid_frames",totals.frames-totals.validFrames},
        {"nonzero_frames",totals.nonzeroFrames},{"sum",totals.sum},
        {"maximum",totals.maximum},{"sum_overflow",totals.sumOverflow}};
}
J allocationSummary50EK(const rr::test::RenderAllocationSummary50EH& summary) {
    return {{"first_frame",{{"observed",summary.all.frames!=0},
            {"valid",summary.firstFrame.valid},
            {"allocations",summary.firstFrame.valid?J(summary.firstFrame.allocations):J(nullptr)}}},
        {"all",allocationTotals50EK(summary.all)},
        {"after_first",allocationTotals50EK(summary.afterFirst)},
        {"capacity_overflow",summary.capacityOverflow},
        {"rejected_frames",summary.rejectedFrames},
        {"rejected_frames_overflow",summary.rejectedFramesOverflow}};
}
void reportContext50EK(const rr::platform::OffscreenGlDiagnostic50DC& diagnostic) {
    std::cerr<<J{{"case","headless_drive50ek"},{"failure_domain","context"},
        {"error",static_cast<unsigned>(diagnostic.error)},
        {"operation",diagnostic.operation?diagnostic.operation:"unknown"},
        {"egl_error",diagnostic.eglError},{"gl_error",diagnostic.glError},
        {"cleanup_error",static_cast<unsigned>(diagnostic.cleanupError)},
        {"cleanup_operation",diagnostic.cleanupOperation?diagnostic.cleanupOperation:"none"},
        {"cleanup_egl_error",diagnostic.cleanupEglError}}.dump()<<'\n';
}
void reportRenderer50EK(const rr::platform::VehicleWindowDiagnostic50DS& diagnostic) {
    std::cerr<<J{{"case","headless_drive50ek"},{"failure_domain","renderer"},
        {"error",static_cast<unsigned>(diagnostic.error)},
        {"operation",diagnostic.operation?diagnostic.operation:"unknown"},
        {"gl_error",diagnostic.glError},{"cleanup_gl_error",diagnostic.cleanupGlError},
        {"component",diagnostic.component},{"material",diagnostic.material},
        {"texture",diagnostic.texture},{"framebuffer_status",diagnostic.framebufferStatus},
        {"batch_accepted",diagnostic.batchAccepted},
        {"default_framebuffer_may_be_modified",diagnostic.defaultFramebufferMayBeModified},
        {"readback_width",diagnostic.readbackWidth},{"readback_height",diagnostic.readbackHeight},
        {"readback_bytes",diagnostic.readbackBytes},
        {"readback_buffer_may_be_modified",diagnostic.readbackBufferMayBeModified},
        {"cleanup_incomplete",diagnostic.cleanupIncomplete}}.dump()<<'\n';
}
void reportTextures50EK(const rr::platform::MaterialTextureGlDiagnostic50EB& diagnostic) {
    std::cerr<<J{{"case","headless_drive50ek"},{"failure_domain","textures"},
        {"error",static_cast<unsigned>(diagnostic.error)},
        {"operation",diagnostic.operation?diagnostic.operation:"unknown"},
        {"texture",diagnostic.texture},{"gl_error",diagnostic.glError},
        {"cleanup_gl_error",diagnostic.cleanupGlError},
        {"cleanup_incomplete",diagnostic.cleanupIncomplete}}.dump()<<'\n';
}
}
int main(int argc,char**argv){
 rr::test::RenderAllocationStats50EH submissionAllocations;
 std::uint64_t completedFrames=0,preparedDrawFrames=0,submittedDrawFrames=0;
 std::uint64_t nativeCalls=0,nativeAllocationFailures=0;
 std::uint32_t maximumDrawVertices=0;
 bool collectorFailed=false;
 unsigned collectorFailurePhase=0,collectorFailureStatus=0;
 J result{{"case","headless_drive50ek"},{"passed",false},
     {"requested_frames",160},{"scenario_runs",0},{"warmup_frames",0},
     {"renderer","vehicle_window_gl50ds"},{"output","headless"}};
 int exitCode=1;
 try{
 if(argc!=3){exitCode=2;throw std::runtime_error(
     "usage: terrain_headless_drive50ek CORPUS_JSON OUTPUT_PPM (run from repository root)");}
 require50EK(std::fesetround(FE_TONEAREST)==0,"round-to-nearest setup failed");
 std::ifstream file(argv[1]);require50EK(bool(file),"cannot open collision corpus");J corpus;file>>corpus;const J*chosen=nullptr;
 for(const auto&row:corpus["cases"])if(row["name"]=="podboxm.col-explicit37-authored-placements-forward")chosen=&row;
 require50EK(chosen,"authored placement case present");const auto&row=*chosen;const auto&qj=row["query"];
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
 // Cold diagnostic setup only: replace retained generic0x84 fixture flags with
 // authenticated fresh graphics-model/source-node flags (49O creation proof).
 // This is before reset setters; never used to coerce a live cache into admission.
 s.nodes[static_cast<int>(x.bindings.lifecycle.model1B4)].sceneFlags190=0x80;
 for(unsigned i=0;i<18;++i)s.nodes[static_cast<int>(x.bindings.lifecycle.model1B4)+1+i].sceneFlags190=0xa4;


 auto ground=authored_runtime_ground50cp_fixture::load({4,16,32,64});require50EK(ground.lift==x.parameters.visual.verticalLimit7EC,"same actual Pod01 lift");std::vector<std::uint32_t> groundStack(ground.stackCount);const auto groundAllocations=::allocations;auto groundProbe=authored_runtime_ground50cp_fixture::resolve(ground,16,0,groundStack.data(),groundStack.size());require50EK(groundProbe.phase==ResetProbePhase50AG::Hit&&::allocations==groundAllocations,"actual source ground height without cold-query allocations");BodyMatrix48G pose=ground.pose;for(unsigned i=0;i<4;++i)pose.v[12+i]=std::bit_cast<float>(groundProbe.staged[i]);
 require50EK(candidates.size()==37,"headless scene requires exactly 37 retained terrain owners");
 std::uint32_t candidateZones[37]{};
 for(std::size_t k=0;k<candidates.size();++k){const auto&j=row["candidates"][row["order"][k].get<std::size_t>()];const auto name=authored_runtime_ground50cp_fixture::lower(corpus["meshes"][j["mesh"].get<std::size_t>()]["name"].get<std::string>());const authored_runtime_ground50cp_fixture::Owner*match=nullptr;for(const auto&o:ground.owners)if(authored_runtime_ground50cp_fixture::lower(o.name)==name){bool equal=true;for(unsigned n=12;n<15;++n)if(o.instance.world.v[n]!=candidates[k].candidate.collision.geometry.worldMatrix.v[n])equal=false;if(equal){match=&o;break;}}require50EK(match,"actual collider placement matches explicit candidate");candidateZones[k]=match->zone;auto&radiusOwner=meshes[j["mesh"].get<std::size_t>()];require50EK(radiusOwner.radii.size()==match->mesh.nodes.size(),"actual mesh radius extent");std::vector<std::uint32_t> topology(match->mesh.nodes.size()+match->mesh.faces.size());require50EK(prepareStaticTargetRadii49X(match->mesh,radiusOwner.radii.data(),radiusOwner.radii.size(),{topology.data(),topology.size()},radiusOwner.maxDepth)==Type2ResourceStatus49X::Complete,"radii prepared from owned actual mesh");candidates[k].maxDepth=radiusOwner.maxDepth;stackCount=std::max(stackCount,1+shape.view.maxDepth+radiusOwner.maxDepth);auto&v=candidates[k].candidate.collision;v.geometry.mesh=&match->mesh;v.geometry.worldMatrix=match->instance.world;v.filter.categoryA4=match->instance.category;v.sphere={{{match->instance.sphere.center.v[0],match->instance.sphere.center.v[1],match->instance.sphere.center.v[2]}},match->instance.sphere.radius};}
 ground.audit=J{};
 require50EK(setLocalMatrix49P({s.nodes,53},x.bindings.lifecycle.main190,pose)&&refreshSceneWorld49P({s.nodes,53},x.bindings.lifecycle.main190),"supplied initial pose");f::project(s,x.bindings);
 FreshPodGeometrySources49O sources{};sources.chariot=x.assets.hierarchy[2];sources.rightEngine=x.assets.hierarchy[1];sources.forward=x.assets.motions[4];sources.back=x.assets.motions[5];rr::assets::MotionBindingDiagnostic49O bindingDiagnostic{};
 require50EK(rr::assets::readMotionNodeBindings49O(x.assets.hierarchy[2],reinterpret_cast<const char*>(x.assets.vut[2].data()),x.assets.vut[2].size(),sources.chariotBindings,bindingDiagnostic),"actual motion bindings");
 FreshPodGeometryWorkspace49O geometryWorkspace{};FreshPodGeometryResult49O geometry{};FreshPodGeometryDiagnostic49O geometryDiagnostic{};
 require50EK(buildFreshPodGeometry49O(sources,geometryWorkspace,geometry,geometryDiagnostic),"actual pod geometry");
 const auto phy=c::e::readBytes("work/impactAuthoring49Y/Pod01.phy");const AuthoredSelectionContext49N selection{AuthoredClass49N::A,false,true,false,false,{}};
 const auto authored=frame49w_authored::bind(phy,geometry,selection,x.parameters,s,x.selectors,x.secondaryInputs);
 auto secondaryPose=pose;for(unsigned i=0;i<3;++i){volatile float a=pose.v[i]*x.secondaryInputs.targetRelative2F0.v[0];volatile float b=pose.v[4+i]*x.secondaryInputs.targetRelative2F0.v[1];volatile float c=pose.v[8+i]*x.secondaryInputs.targetRelative2F0.v[2];volatile float ab=a+b;volatile float abc=ab+c;volatile float position=pose.v[12+i]+abc;secondaryPose.v[12+i]=position;}
 require50EK(setLocalMatrix49P({s.nodes,53},x.bindings.lifecycle.physical19C,secondaryPose)&&refreshSceneWorld49P({s.nodes,53},x.bindings.lifecycle.physical19C),"supplied secondary initial pose");
 require50EK(setLocalMatrix49P({s.nodes,53},x.bindings.lifecycle.model1B4,secondaryPose),"supplied initial displayed pose");f::project(s,x.bindings);
 ResetBoundNode50BA resetModelNodes[18]{};ResetBoundsWorkspace50BA resetBoundsWorkspace{};ModelBoundsResult49O resetModelBounds{};
 require50EK(x.assets.hierarchy[2].nodeCount==18,"actual chariot source node count");
 for(unsigned i=0;i<18;++i){rr::assets::PsgNode49O authoredNode{};require50EK(rr::assets::readPsgNode49O(x.assets.hierarchy[2],i,authoredNode),"actual reset model bound");const auto index=static_cast<std::int32_t>(x.bindings.lifecycle.model1B4)+1+static_cast<std::int32_t>(i);resetModelNodes[i].node=static_cast<SceneNodeIndex49P>(index);const auto expectedParent=static_cast<std::int32_t>(x.bindings.lifecycle.model1B4)+1+authoredNode.parent;require50EK(static_cast<std::int32_t>(s.nodes[index].parent180)==expectedParent,"source flat model parent mapping");for(unsigned j=0;j<3;++j){resetModelNodes[i].center140.v[j]=authoredNode.bounds[j];resetModelNodes[i].halfExtent150.v[j]=authoredNode.bounds[3+j];}}
 const auto beforeResetBounds=::allocations;
 require50EK(refreshResetModelBounds50BA({s.nodes,53},x.bindings.lifecycle.model1B4,
     resetModelNodes,18,resetBoundsWorkspace,resetModelBounds),"reset model cache admission");
 require50EK(::allocations==beforeResetBounds,"reset model bounds no allocation");

 SecondaryActivationGlobals49V resetGlobals{};initializedSecondaryActivationGlobals49V(resetGlobals);resetMainRigidBody50AR(m,s.bodies.mainSphere,s.mainSupplement,resetGlobals);resetSecondaryActivation49V(s.bodies.secondary,s.secondarySupplement,resetGlobals);
 for(auto*body:{&m.components.root,&s.bodies.secondary.root}){require50EK(matrixToQuaternion48G(body->scene,body->body.orientation),"prepared initial orientation");std::memcpy(body->body.position.v,body->scene.v+12,12);}
 ResetLifecycleGlobals50AT resetLifecycle{};initializedResetLifecycleGlobals50AT(resetLifecycle);require50EK(resetLifecycleRecords50AT(s.bodies.vehicle838,s.lifecycle,resetLifecycle),"source reset lifecycle records");s.fields.applicationByte58=2;m.controls.player.flagsB9C=0x80;
 SpringGeneratedParameters48K settings{};settings.impact54=true;settings.friction50=true;settings.impact.disableLeverArms5C=true;settings.impact.correctPenetration64=true;settings.impact.normalForceScale98=.97f;settings.impact.positionCorrectionScaleCC=.2f;
 SpringResponseParameters48L response{};response.maxSpeed94=10000;rr::assets::SpringCollisionAuthoring50F collision{};rr::assets::PodPhysicsDiagnostic49N diagnostic{};
 require50EK(rr::assets::readSpringCollision50F(reinterpret_cast<const char*>(phy.data()),phy.size(),collision,diagnostic)&&rr::assets::applySpringCollision50F(collision,x.parameters.bodies.main.body.mass,settings,response),"authored response settings");
 const auto tuning=c::e::readBytes("work/impactAuthoring49Y/ta.tun");require50EK(rr::assets::readEnvironmentImpact49Y(reinterpret_cast<const char*>(tuning.data()),tuning.size(),x.parameters.impactTuning,diagnostic),"actual TA impact tuning");x.parameters.impactTuningBound=true;
 rr::assets::DefenseAuthoring49Y defense{};require50EK(rr::assets::readDefenseAuthoring49Y(reinterpret_cast<const char*>(phy.data()),phy.size(),defense,diagnostic)&&defense.baseDefense.present,"explicit base-profile defense");s.fields.defenseCF0=defense.baseDefense.value;
 StaticSpringState50G spring{};spring.records.generatedCapacity88=40;SpringRecord48N records[60]{},queryRecords[60]{},scratchRecords[60]{};

 const J*secondaryCase=nullptr;for(const auto&item:corpus["cases"])if(item["name"]=="chariotm.col-explicit37-authored-placements-forward")secondaryCase=&item;
 require50EK(secondaryCase,"actual chariot query present");const auto&sqj=(*secondaryCase)["query"];
 auto secondaryShape=cold::shape(corpus["shapes"][sqj["shape"].get<std::size_t>()]);auto secondaryFilter=cold::filter(sqj["filter"]);
 Type2StaticQuery49X secondaryQuery{};secondaryQuery.resource=&secondaryShape.view;secondaryQuery.filter=&secondaryFilter;secondaryQuery.queryScene.value=78;
 secondaryQuery.collisionTypeTag=sqj["collision_tag"];secondaryQuery.groupTypeTag=sqj["group_tag"];secondaryQuery.callbackEnabled=sqj["callback"];
 cold::words(secondaryQuery.copied40.data(),sqj["copied40"]);cold::words(secondaryQuery.copied50.data(),sqj["copied50"]);
 secondaryQuery.skipMask=(*secondaryCase)["masks"][0];secondaryQuery.dynamicMask=(*secondaryCase)["masks"][1];secondaryQuery.materialMask=(*secondaryCase)["masks"][2];
 require50EK((*secondaryCase)["order"]==row["order"]&&(*secondaryCase)["candidates"]==row["candidates"],"same explicit ordered terrain inputs");
 StaticSpringState50G secondarySpring{};secondarySpring.records.generatedCapacity88=60;
 SpringRecord48N secondaryRecords[60]{},secondaryQueryRecords[60]{},secondaryScratchRecords[60]{};
 const auto srays=secondaryShape.view.rayCount;
 std::vector<SpringCumulativePair48N> sip(srays),ssp(srays),sdp(srays);std::vector<Type2PairMetadata49X> sim(srays),ssm(srays),sdm(srays);
 std::vector<TerrainCandidateEffects48J> sse(candidates.size()),sde(candidates.size());std::vector<Type2WorkItem49X> ss(stackCount+secondaryShape.view.maxDepth);
 Type2StaticInitial49X si{};si.records=secondarySpring.records;si.pairs=sip.data();si.metadata=sim.data();si.pairSlots=srays;si.recordSlots=secondaryRecords;si.recordCount=60;
 Type2StaticWorkspace49X sw{ssp.data(),srays,ssm.data(),srays,secondaryScratchRecords,60,sse.data(),sse.size(),ss.data(),ss.size(),nullptr,0};
 Type2StaticDestination49X sd{sdp.data(),srays,sdm.data(),srays,secondaryQueryRecords,60,sde.data(),sde.size()};Type2StaticResult49X secondaryResult{};
 SpringGeneratedParameters48K secondarySettings{};secondarySettings.impact54=true;secondarySettings.impact.disableLeverArms5C=false;secondarySettings.impact.correctPenetration64=true;secondarySettings.impact.normalForceScale98=1;secondarySettings.impact.positionCorrectionScaleCC=.025f;
 SpringResponseParameters48L secondaryResponse{};secondaryResponse.maxSpeed94=10000;
 require50EK(rr::assets::applySpringCollision50F(collision,x.parameters.bodies.secondary.body.mass,secondarySettings,secondaryResponse),"authored secondary response settings");
 const auto rays=shape.view.rayCount;
 std::vector<SpringCumulativePair48N> initialPairs(rays),scratchPairs(rays),pairs(rays);std::vector<Type2PairMetadata49X> initialMetadata(rays),scratchMetadata(rays),metadata(rays);
 std::vector<TerrainCandidateEffects48J> scratchEffects(candidates.size()),effects(candidates.size());std::vector<Type2WorkItem49X> stack(stackCount);
 Type2StaticInitial49X initial{};initial.records=spring.records;initial.pairs=initialPairs.data();initial.metadata=initialMetadata.data();initial.pairSlots=rays;initial.recordSlots=records;initial.recordCount=40;
 Type2StaticWorkspace49X workspace{scratchPairs.data(),rays,scratchMetadata.data(),rays,scratchRecords,40,scratchEffects.data(),scratchEffects.size(),stack.data(),stack.size(),nullptr,0};
 Type2StaticDestination49X destination{pairs.data(),rays,metadata.data(),rays,queryRecords,40,effects.data(),effects.size()};Type2StaticResult49X queryResult{};

 // Explicit diagnostic movement-query state and complete configured static order.
 // Registration/collection and original query70/84 cache ownership are external.
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
 // iteration now produce query views; live zone membership/order remains outside.
 SearchStorage50BS registry{};require50EK(candidates.size()==37,"retained37 terrain owners");
 require50EK(initializeColdSearchStorage50BU(registry,1024)==SearchStorageStatus50BS::Complete,"source factory list capacity");std::uint32_t registeredSlot=0;
 require50EK(adoptFirstSearchBlock50BU(registry,registeredSlot)==SearchStorageStatus50BS::Complete,"caller-owned first registration block");
 for(std::size_t i=0;i<candidates.size();++i){require50EK(selectSearchSlot50BS(registry,0,registeredSlot)==SearchStorageStatus50BS::Complete&&registeredSlot==i+1,"ordered registration slot");require50EK(insertSearchSlot50BS(registry,registeredSlot,static_cast<std::uint32_t>((i+1)*4))==SearchStorageStatus50BS::Complete,"register actual terrain owner handle");}
 Type3Candidate48P registeredMovement[37]{};std::size_t registeredIndices[37]{},registeredCount=0;
 SearchListBlock50BR registrationBlocks[2]{};SearchListInput50BR registrationView{};SearchListCursor50BR registrationCursor{};
 auto gatherRegistered=[&](){if(prepareSearchView50BT(registry,registrationBlocks,2,registrationView)!=SearchStorageStatus50BS::Complete)return false;resetSearchList50BR(registrationCursor,true);registeredCount=0;
  for(;;){std::uint32_t id=0;if(nextSearchList50BR(registrationView,registrationCursor,id)!=SearchListStatus50BR::Complete)return false;if(!id)break;if((id&3)||id/4<1||id/4>candidates.size()||registeredCount>=37)return false;auto index=id/4-1;registeredIndices[registeredCount]=index;registeredMovement[registeredCount]=movementCandidates[index];++registeredCount;}return true;};

 // Explicit enabled collector admission: old direct-query fixtures only needed
 // target-enabled bit4. Collection consumes query-enabled bit2 as well.
 qfilter.flagsF8=static_cast<std::uint8_t>(qfilter.flagsF8|2u);
 // Authored zone ownership over the retained37 owners; numeric zone-chain
 // Global query chain now uses source first-zone-only head. Full318 model
 // registration and explicit per-object membership chains remain separate.
 // This does not infer the actual9-zone world membership or registration order.
 CollectionObject50BX collectionObjects[38]{};
 CollectionObjectState50BX collectionSource[38]{},collectionScratch[38]{},collectionFirst[38]{},collectionSecond[38]{};
 SearchStorage50BS zoneRegistries[18]{};ZoneRegistration50CJ zoneRecords[37]{};
 ZoneRegistrationScratch50CJ zoneScratch{};ZoneLifecycle50CL zoneLifecycle{};
 SearchListBlock50BR zoneBlocks[36]{};
 CollectionTree50BX collectionTrees[18]{};
 CollectionTreeState50BX treeSource[18]{},treeScratch[18]{},treeFirst[18]{},treeSecond[18]{};
 CollectionZone50BX collectionZones[9]{};GlobalZoneLink50CK globalLink{};
 const ZoneLifecycleStorage50CL zoneStorage{zoneRegistries,18,zoneRecords,37,{collectionTrees,18,collectionZones,9,zoneBlocks,36}};
 require50EK(initializeZoneLifecycle50CL(zoneLifecycle,zoneStorage,9,37,1024,zoneScratch)==SearchStorageStatus50BS::Complete,"persistent zone lifecycle initialization");
 ZoneRegistrationCommand50CQ zoneEvents[128]{};ZoneRegistrationQueue50CQ zoneQueue{zoneEvents,128,0,0};
 for(unsigned i=0;i<37;++i){require50EK(enqueueZoneRegistration50CQ(zoneQueue,{ZoneRegistrationOperation50CQ::Set,i+1,candidateZones[i],ZoneMethod50CL::First})==ZoneQueueStatus50CQ::Complete,"queue authored zone owner registration");}

 require50EK(initializeGlobalZoneLink50CK(1,9,0,globalLink),"source global chain first-zone projection; opaque search identity supplied zero");
 std::uint32_t scratchIds[64]{},firstIds[64]{},secondIds[64]{},mergeScratch[64]{},mergedIds[64]{};
 std::size_t resolveScratch[64]{};CandidateHandle50CB ownerHandles[37]{};
 for(std::size_t i=0;i<37;++i)ownerHandles[i]={candidates[i].candidate.collision.filter.identity,i};
 std::sort(std::begin(ownerHandles),std::end(ownerHandles),[](const auto&a,const auto&b){return a.identity<b.identity;});
 auto collectionBuffer=[](CollectionObjectState50BX*objects,CollectionTreeState50BX*trees,std::uint32_t*ids){CandidateCollectionBuffers50BX b{};b.objects=objects;b.objectCapacity=38;b.trees=trees;b.treeCapacity=18;b.identities=ids;b.identityCapacity=64;return b;};
 CandidateCollectionInput50BX collectionInput{};collectionInput.objects=collectionObjects;collectionInput.objectCount=38;
 collectionInput.trees=collectionTrees;collectionInput.treeCount=18;collectionInput.zones=collectionZones;collectionInput.zoneCount=9;
 collectionInput.zoneLinks=&globalLink.collection;collectionInput.zoneLinkCount=1;collectionInput.zoneHead=1;collectionInput.query=38;
 collectionInput.bounds.shapeTag=2;collectionInput.type2Resource=query.resource;collectionInput.directTypeTag=query.collisionTypeTag;collectionInput.groupTypeTag=query.groupTypeTag;
 CandidateCollectionState50BX collectionInitial{};collectionInitial.objects=collectionSource;collectionInitial.objectCapacity=38;collectionInitial.trees=treeSource;collectionInitial.treeCapacity=18;
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
  if(!query.resource||query.resource->root>=query.resource->nodeCount)return false;
  const auto&root=query.resource->nodes[query.resource->root];QuerySpherePrepareInput48O prepare{};prepare.preparedScene=s.nodes[static_cast<int>(x.bindings.lifecycle.main190)].world100;prepare.shapeType=2;prepare.hasPairTree=true;
  for(unsigned i=0;i<3;++i){prepare.treeCenter.v[i]=root.center[i];prepare.treeHalf.v[i]=root.half[i];}
  QuerySpherePrepareOutput48O prepared{};if(prepareQuerySphere48O(prepare,prepared)!=QuerySpherePrepareStatus48O::Ready)return false;
  CollisionSphere48J sphere{};for(unsigned i=0;i<3;++i)sphere.center.v[i]=prepared.center.v[i];sphere.radius=prepared.radius;
  auto projected=projectCollectionObject50CF(qfilter,sphere,2,2,0);if(!projected.valid)return false;collectionObjects[37]=projected.object;collectionSource[37]=projected.state;
  collectionInput.bounds.shapeWorld=prepared.world;collectionInput.initialCurrentIdentity=query.initialQueryCurrentIdentity;
  const auto result=collectVehicleCandidates50CA(collectionInput,collectionInitial,collectionWork);
  if(result.phase!=VehicleCandidateResult50CA::Phase::Complete){
   collectorFailed=true;collectorFailurePhase=static_cast<unsigned>(result.phase);
   collectorFailureStatus=static_cast<unsigned>(result.collectionStatus);return false;
  }
  if(resolveCandidateHandles50CB(mergedIds,result.count,ownerHandles,37,resolveScratch,64,registeredIndices,37)!=CandidateHandleStatus50CB::Complete)return false;
  registeredCount=static_cast<std::size_t>(result.count);peakCollected=std::max(peakCollected,registeredCount);addCounter50EK(collectedCalls);
  for(unsigned i=0;i<37;++i)candidates[i].candidate.collision.filter.flagsF8=collectionSecond[i].flagsF8;
  qfilter.flagsF8=collectionSecond[37].flagsF8;query.initialQueryCurrentIdentity=result.second.currentIdentity;
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
 require50EK(decodeResource48C(raw,res,error),"decode actual Pod01");
 auto scene=[&](const char*name,Scene48D&out){return extractResMember48C(res,name,member,error)&&decodeScene48D(member,out,error);};
 auto material=[&](const char*name,Material48E&out){return extractResMember48C(res,name,member,error)&&decodeMaterial48E(member,out,error);};
 require50EK(scene("pods\\pod01\\englt1.psg",left)&&scene("pods\\pod01\\engrt1.psg",right),"actual engine scenes");
 require50EK(material("textures\\01_1.psm",shared)&&material("textures\\flame01l.psm",fl)&&material("textures\\flame01r.psm",fr),"actual flame materials");PodFlameError50W diag{};
 require50EK(buildPodFlameOwners50W(left,right,shared,fl,fr,static_cast<unsigned>(x.bindings.visual.left1BC)+1,static_cast<unsigned>(x.bindings.visual.right1B8)+1,coldFlame,diag),"cold flame owners");
 }


 BaseTailState50X baseTail=coldBaseTailState50X(0x3f800000u,0x3f800000u);BaseTailGlobals50X baseGlobals{};BaseTailWorkspace50X baseWorkspace{};BasePoseHistoryState50Y poseHistory{};poseHistory.nextFD4=19;
 // Explicit diagnostic cold history: all positions start at supplied pod pose.
 for(auto&slot:poseHistory.historyD50){std::memcpy(slot,pose.v+12,12);slot[3]=0x3f800000u;}
 s.impactSupplement.partnerId=0xffffffffu;FrameTimingState49I clock{};clock.lastDispatched23C=.016f;

 // Fresh unrenamed native model roots; later renames/clones are not admitted.
 rr::assets::ModelRootName50BL rootNames[2]{};SceneNodeIndex49P namedIndices[2][14];rr::assets::EffectNameModel50BL namedModels[2]{};rr::assets::EffectNames50BL namesForEffects{};
 for(unsigned side=0;side<2;++side){rr::assets::initializeFreshModelRootName50BL(rootNames[side]);const auto root=side?x.bindings.visual.right1B8:x.bindings.visual.left1BC;require50EK(x.assets.hierarchy[side].nodeCount==14,"actual engine node count");for(unsigned i=0;i<14;++i)namedIndices[side][i]=static_cast<SceneNodeIndex49P>(static_cast<int>(root)+1+i);namedModels[side]={root,&rootNames[side],x.assets.hierarchy[side],namedIndices[side],14};}
 require50EK(rr::assets::buildEffectNames50BL({s.nodes,53},namedModels,2,namesForEffects),"actual effect attachment names");
 EffectOwner50BE nativeEffects[8]{};const char*effectFiles[]={"podLeftEngineDust.emt","podRightEngineDust.emt","podLeftEngineSmoke.emt","podRightEngineSmoke.emt","podSparks.emt","podSparks.emt","podFire.emt","podFire.emt"};
 for(unsigned i=0;i<8;++i){auto raw=c::e::readBytes(std::string("evidence/effectAuthoring50BF/members/")+effectFiles[i]);const auto profile=i<4?EffectProfile50BE::Particle19A5E0:i<6?EffectProfile50BE::Ordinary15F100:EffectProfile50BE::Fire15DE00;require50EK(rr::assets::buildEffectControlOwner50BJ(i+1,profile,reinterpret_cast<const char*>(raw.data()),raw.size(),{0,0},nativeEffects[i]),"actual effect control construction");}
 ResetEffectBindings50BK resetEffectBindings{{1,2},{0,0},{5,6,0,0},false};ResetEffectState50BK resetEffectState{};std::memcpy(resetEffectState.timers820,s.impactSupplement.timers820,16);ResetEffectWorkspace50BK resetEffectWorkspace{};
 const auto resetAllocationCount=::allocations;require50EK(resetEffectStops50BK(resetEffectBindings,nativeEffects,8,resetEffectState,resetEffectWorkspace).status==EffectDispatchStatus50BE::Complete&&::allocations==resetAllocationCount,"native reset effect stop sequence");std::memcpy(s.impactSupplement.timers820,resetEffectState.timers820,16);
 ImpactEffectsWorkspace50B effectsWorkspace{};ImpactEffectEvent50B generatedEffects[maxImpactEffectEvents50B]{};EffectDispatchWorkspace50BE effectDispatch{};
 std::uint64_t effectCommands=0;
 auto consumeEffects=[&](const ImpactEffectEvent50B*events,std::size_t count){
  const auto result=consumeEffectControls50BE(events,count,nativeEffects,8,effectDispatch);
  if(result.status!=EffectDispatchStatus50BE::Complete)return false;
  addCounter50EK(effectCommands,count);return true;
 };
 std::uint64_t mainContactFrames=0,secondaryContactFrames=0,brakeFrames=0;
 float peakDrive=0,peakDriveDifference=0;
 auto runNative=[&](auto&&fn,const char*label){
  const std::uint64_t before=::allocations;
  const bool ok=fn();
  const std::uint64_t after=::allocations;
  addCounter50EK(nativeCalls);
  if(after!=before)addCounter50EK(nativeAllocationFailures);
  if(!ok||after!=before){std::ostringstream message;message<<"native frame "<<completedFrames
   <<" "<<label<<" error="<<static_cast<unsigned>(x.diagnostic.error)
   <<" source-stage="<<std::hex<<x.diagnostic.sourceStage
   <<" body="<<static_cast<unsigned>(x.diagnostic.body)
   <<" allocation-counter-before="<<std::dec<<before<<" after="<<after;
   throw std::runtime_error(message.str());}
 };

 // Actual retained chariot and engine meshes, loaded once before creating GL.
 rr::assets::VehicleDrawAssets50CR drawAssets;rr::assets::VehicleDrawAssetsInput50CR drawInput{};
 rr::assets::VehicleDrawAssetDiagnostic50CR drawLoadDiagnostic{};
 std::vector<std::uint8_t> drawBytes[3];SceneNodeIndex49P drawNodes[3][18]{};std::uint8_t drawPalette[3][18]{};
 const char*drawKeys[]={"cb6d91807cca98eecb547dfd4b030345e8e88e6ab9363fc436012732487863b5","210c524d329161a97e79baecded9c34bb6949b6910dea148ce2bf5cb0ddb2fc3","f073a4149ace5c36bf51d3390c867cddabedca939e1a876d0efef8cf8d438d60"};
 const unsigned drawCounts[]={18,14,14},drawBases[]={31,1,16},drawMapped[]={2,0,1};
 for(unsigned component=0;component<3;++component){
  drawBytes[component]=c::e::readBytes(std::string("evidence/visual49G/cacheobjects/")+drawKeys[component]+".rrscene");
  const auto&psg=x.assets.psg[drawMapped[component]];const auto padded=(drawCounts[component]+3)&~3u;
  require50EK(psg.size()>=12+padded,"actual palette suffix bounds");const auto suffix=psg.size()-(12+padded);
  auto word=[&](std::size_t at){return std::uint32_t(psg[at])|(std::uint32_t(psg[at+1])<<8)|(std::uint32_t(psg[at+2])<<16)|(std::uint32_t(psg[at+3])<<24);};
  require50EK(word(suffix)==1&&word(suffix+4)==padded&&word(suffix+8+padded)==drawCounts[component],"actual single palette record");
  for(unsigned n=0;n<drawCounts[component];++n){drawPalette[component][n]=psg[suffix+8+n];drawNodes[component][n]=static_cast<SceneNodeIndex49P>(drawBases[component]+n);}
  drawInput.component[component]={&drawBytes[component],drawNodes[component],drawCounts[component],drawPalette[component],drawCounts[component]};
 }
 require50EK(drawAssets.load(drawInput,drawLoadDiagnostic),"cold native draw owners");
 namespace podTextures=pod_materials50ed_fixture;
 auto materialOwner=std::make_unique<podTextures::Owner>();
 podTextures::Request materialRequest{};materialRequest.root="evidence/visual49G";
 materialRequest.coldSourceByteCapacity=8u*1024u*1024u;
 materialRequest.textureBudgets={5,26,1024u*1024u,1024u*1024u,256};
 for(unsigned component=0;component<3;++component){
  const auto* scene=drawAssets.scene(static_cast<VehicleDrawComponent50CM>(component));
  require50EK(scene,"material scene missing");
  materialRequest.choices[component].assign(scene->materials.size(),
      podTextures::Choice{1,1,rr::assets::MaterialTextureRequirement50EA::required,
                         podTextures::MipMode::largestRetained,0});
 }
 podTextures::Diagnostic materialDiagnostic{};
 const bool materialsLoaded=materialOwner->load(materialRequest,drawAssets,materialDiagnostic);
 require50EK(materialsLoaded,materialDiagnostic.message.empty()?"Pod material load failed":materialDiagnostic.message.c_str());
 // Pin this one case to the same exact retained five-texture selection. These
 // are provenance checks, not filenames guessed from scene material names.
 struct ExpectedTexture {const char*name;const char*key;std::uint32_t mip,width,height;};
 const ExpectedTexture expectedTextures[]={
  {"01_2","1e31063ae9ea71f8e9d3bd6098b538c27100b91a3285fa382e2904919f04d5e0",4,256,256},
  {"01_3","e9c2db8ff9cfcc7bd26cf2e01d37e7c3368bee355c5b10b0de453088e164d65e",0,128,128},
  {"pod_wndw","394bcabc19c83b150c6b00a682b710e5de8b3efdd6f4d39a1826c27b902ad1dc",2,64,64},
  {"01_1","da9d5d84a83c02b496872f7be04ce69135ec08c3ece3191d2c2d0671434f583a",4,256,256},
  {"flame","abec7a3d99994cdedfaf7cab18576f51dc903f89afb20e977e791d97743ac378",0,64,64}
 };
 const auto*textureSelections=materialOwner->textures();
 require50EK(textureSelections&&textureSelections->size()==5,"exact five retained texture selections required");
 result["textures"]=J::array();
 for(const auto&expected:expectedTextures){
  const auto matches=std::count_if(textureSelections->begin(),textureSelections->end(),
      [&](const auto&actual){return actual.key==expected.key&&actual.mipIndex==expected.mip&&
          actual.width==expected.width&&actual.height==expected.height;});
  require50EK(matches==1,"retained texture key/mip/dimensions differ from50DU selection");
  result["textures"].push_back({{"name",expected.name},{"mip",expected.mip},
      {"width",expected.width},{"height",expected.height}});
 }
 VehicleDrawWorkspace50CM drawWorkspace{};
 // Keep native held queries on the explicit advanced preset4, not on the
 // old fixture's unsupported negative flag-button codes.
 ControlPresets49D controlPresets{};initializeControlPresets49D(controlPresets);
 require50EK(selectControlPreset49D(controlPresets,0,4),"explicit advanced control preset");SelectedControlPreset49D selectedControls{};
 require50EK(resolveControlPreset49D(controlPresets,0,0,selectedControls),"separate explicit action/configuration slots");
 x.actionCodes=selectedControls.actions;x.configurationEntryNonzero=selectedControls.configurationEntryNonzero;
 DeviceButtonBank48Y initialControllerButtons{};ControllerIngress50CX controller(initialControllerButtons);
 const auto deviceDescriptors=initialDualShockDescriptors48Y();
 namespace platform=rr::platform;
 DevicePacket48Y packet{};
 // The identical single differential schedule from frozen50DH. Bytes 4/6
 // stay centered; packet7 is left engine, packet5 right engine. All action
 // buttons remain released. No renderer warm-up or alternate scenario exists.
 auto composeScriptPacket=[&](unsigned frame){
  packet={};packet.bytes[2]=packet.bytes[3]=0xff;
  for(unsigned byte=4;byte<8;++byte)packet.bytes[byte]=128;
  if(frame>=10&&frame<40)packet.bytes[5]=packet.bytes[7]=0;
  if(frame>=40&&frame<80){packet.bytes[5]=70;packet.bytes[7]=0;}
  if(frame>=100)packet.bytes[5]=packet.bytes[7]=255;
 };
 auto sampleController=[&](){
  runNative([&]{return controller.samplePacket(true,true,packet,deviceDescriptors,
      dualShockActiveButtons48Y,0);},"native controller sample");
  require50EK(controller.deviceSample().status==DevicePacketStatus48Y::Accepted,
              "controller did not accept the scripted50DH packet");
  require50EK(controller.deviceSample().transitionCount==0,
              "unexpected action-button transition; native dispatcher required");
 };

 auto advanceFrame=[&](){
 // The native path does not call fixture/oracle callbacks; retain a bounded
 // diagnostic frame label for any future cold-fixture diagnostics.
 x.frame=static_cast<unsigned>(std::min<std::uint64_t>(completedFrames,
     std::numeric_limits<unsigned>::max()));
 FrameReader49K read{};read.preparedFactor=.016f;read.codes=x.actionCodes;
 read.context.previousFlagsB9C=s.bodies.main.controls.player.flagsB9C;
 sampleController();
 runNative([&]{return controller.prepareFrameReader({false,0,0,0},read.context,
     read.codes,read.preparedFactor,read);},"native held provider");
 runNative([&]{return readVehicleFrame49W(s,x.bindings,read,x.workspace,x.diagnostic);},"persistent reader");PostPlayerAt217DE0_49K post{};post.player=s.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=x.configurationEntryNonzero;
 runNative([&]{return bindVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,x.workspace,x.diagnostic);},"persistent binding");
 const auto&liveDrive=m.components.bindings;
 peakDrive=std::max(peakDrive,std::max(liveDrive.engineDrive[0],liveDrive.engineDrive[1]));
 peakDriveDifference=std::max(peakDriveDifference,std::abs(liveDrive.engineDrive[0]-liveDrive.engineDrive[1]));
 if(liveDrive.aeroBrakeEC>0)addCounter50EK(brakeFrames);
 runNative([&]{return graphicsVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::Base2176E0BeforeGraphics),x.workspace,x.diagnostic);},"persistent graphics");
 ImpactConsumerInput49Z consumer{};consumer.managerFlags58=2;consumer.profile34=2;consumer.finalDamageThreshold64=1;VehicleImpactEvents49Z consumerEvents{};
 runNative([&]{return consumeVehicleImpacts49Z(s,x.bindings,consumer,consumerEvents,x.workspace,x.diagnostic);},"persistent impact consumers");runNative([&]{return repairVehicleImpacts50A(s,x.bindings,x.workspace,x.diagnostic);},"persistent repair");
 ImpactEffectsInput50B owners{};owners.models[0].node=x.bindings.visual.left1BC;owners.models[1].node=x.bindings.visual.right1B8;owners.models[2].node=owners.models[3].node=SceneNodeIndex49P::none;std::size_t count=0;owners.damage00Matches=namesForEffects.damage00;owners.nameCount=53;for(unsigned side=0;side<2;++side){auto&model=owners.models[side];require50EK(namesForEffects.modelRoots&(std::uint64_t(1)<<static_cast<unsigned>(model.node)),"bound model name scope");model.flags03=3;model.effect800={nativeEffects[2+side].id,nativeEffects[2+side].state.flags1D4};model.effect804={nativeEffects[4+side].id,nativeEffects[4+side].state.flags1D4};model.effect80C={nativeEffects[6+side].id,nativeEffects[6+side].state.flags1D4};}
 runNative([&]{return effectsVehicleImpacts50O(s,x.bindings,owners,effectsWorkspace,generatedEffects,maxImpactEffectEvents50B,count,x.workspace,x.diagnostic);},"actual effect control bindings");runNative([&]{return consumeEffects(generatedEffects,count);},"consume impact effect controls");OptionalProbeOwners50P optionalOwners{1,1,{1,2}};OptionalProbeResult50P optional{};
 runNative([&]{return optionalProbeVehicleImpacts50P(s,x.bindings,optionalOwners,optional,x.workspace,x.diagnostic);},"supplied manager-present disabled-probe context");runNative([&]{return consumeEffects(optional.events,optional.count);},"consume dust stop controls");ImpactTransitionResult50C decision{};
 runNative([&]{return decideVehicleImpacts50P(s,x.bindings,0x3f800000u,decision,x.workspace,x.diagnostic);},"ordinary damage decision");PlainEffectOwners50Q plainOwners{};for(unsigned side=0;side<2;++side){for(unsigned array=0;array<3;++array){const auto&owner=nativeEffects[2+array*2+side];plainOwners.slots[array*4+side]={owner.id,owner.state.flags1D4};}}PlainEffectResult50Q plain{};
 runNative([&]{return plainEffectsVehicleTail50Q(s,x.bindings,plainOwners,plain,x.workspace,x.diagnostic);},"plain effects");runNative([&]{return consumeEffects(plain.events,plain.count);},"consume plain effect controls");runNative([&]{return packVehicleFrame50S(s,x.bindings,x.configurationEntryNonzero,x.workspace,x.diagnostic);},"packet");BaseTailBindings50X baseBinding{77,Probe128Binding50X::Null};
 runNative([&]{return advanceVehicleBaseThroughEngine50X(s,x.bindings,clock,baseBinding,baseTail,baseGlobals,coldFlame,baseWorkspace,x.workspace,x.diagnostic);},"persistent base engine");
 runNative([&]{return finishVehicleBaseAndMainCache50Y(s,x.bindings,baseTail,true,poseHistory,f::cut(PreservingCut49W::OtherChildrenBeforeMain340),x.workspace,x.diagnostic);},"persistent base pose history");
 runNative([&]{return refreshSecondaryVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::OtherChildrenBetween340And35C),x.workspace,x.diagnostic);},"secondary cache");runNative([&]{return scheduleVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::OtherChildrenAfter35C),60,x.workspace,x.diagnostic);},"schedule");
 require50EK(s.bodies.main.schedule.count==1,"16ms diagnostic step requires one native substep");

 runNative([&]{return beginVehicleSubstep49W(s,x.bindings,x.selectors,x.workspace,x.diagnostic);},"native selector");
 MainStaticDetectorAdmission50H admission{true,true,true,false,false,false,true,true,2,0,x.bindings.lifecycle.main190};
 runNative(collectRegistered,"ordered prepared-list vehicle candidates");
 runNative([&]{return queryMainIndexedStaticVehicle50CD(s,x.bindings,admission,query,candidates.data(),candidates.size(),registeredIndices,registeredCount,initial,workspace,destination,queryResult)==Type2StaticStatus49X::Complete;},"actual terrain query");
 runNative([&]{return commitCandidateEffects(effects);},"commit main query candidate effects");qfilter.flagsF8=queryResult.queryFlagsF8;query.initialQueryCurrentIdentity=queryResult.currentCandidateIdentity;initial.selected=queryResult.selected;
 spring.records=queryResult.records;std::memcpy(records,queryRecords,sizeof records);

 if(s.bodies.vehicle838){
 MainStaticDetectorAdmission50H secondaryAdmission=admission;secondaryAdmission.queryScene=x.bindings.lifecycle.physical19C;
 runNative([&]{return prepareSecondaryQueryScene50J(s,x.bindings,secondaryAdmission,x.workspace)==SecondaryQuerySceneStatus50J::Ready;},"early original secondary scene getter");
 // Secondary uses the same first-substep merged list, without recollection.

 runNative([&]{return querySecondaryIndexedStaticVehicle50CD(s,x.bindings,secondaryAdmission,secondaryQuery,candidates.data(),candidates.size(),registeredIndices,registeredCount,si,sw,sd,secondaryResult)==Type2StaticStatus49X::Complete;},"actual secondary terrain query");
 runNative([&]{return commitCandidateEffects(sde);},"commit secondary query candidate effects");secondaryFilter.flagsF8=secondaryResult.queryFlagsF8;secondaryQuery.initialQueryCurrentIdentity=secondaryResult.currentCandidateIdentity;si.selected=secondaryResult.selected;
 secondarySpring.records=secondaryResult.records;std::memcpy(secondaryRecords,secondaryQueryRecords,sizeof secondaryRecords);
 
 }
 PostWorld49W world{};world.point=s.bodies.vehicle838?WorldCut49W::First358Secondary36CAnd219420:WorldCut49W::First358And219420;world.effects=contactEffects49W(s);world.applicationGateByte58=2;
 runNative([&]{return produceVehicleForces49W(s,x.bindings,x.parameters,world,x.workspace,x.diagnostic);},"native main forces");
 ResponseStorage50K responseStorage;auto responseWorkspace=responseStorage.view(x.workspace);
 runNative([&]{return respondMainStaticVehicle50G(s,x.bindings,x.parameters,settings,response,x.secondaryInputs,query.queryScene,1,spring,records,40,responseWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual main contact response");
 require50EK(s.phase==(s.bodies.vehicle838?VehicleFramePhase49W::Post368:VehicleFramePhase49W::MainIntegration),"source-selected response phase");

 if(s.bodies.vehicle838){
 ResponseStorage50K secondaryStorage;auto secondaryWorkspace=secondaryStorage.view(x.workspace);
 runNative([&]{return respondSecondaryStaticVehicle50J(s,x.bindings,x.parameters,secondarySettings,secondaryResponse,{78},2,secondarySpring,secondaryRecords,60,secondaryWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual secondary contact and integration");
 require50EK(s.phase==VehicleFramePhase49W::MainIntegration,"secondary response integration phase");
 }
 runNative([&]{return integrateVehicleMain49W(s,x.bindings,x.parameters,x.workspace,x.diagnostic);},"contact-driven body integration");
 require50EK(s.phase==VehicleFramePhase49W::Query219120,"real late-query boundary");
 std::memcpy(&endpoints[0],s.bodies.main.priorPoint7B0,16);std::memcpy(&endpoints[1],s.bodies.main.currentPoint7C0,16);
 runNative(gatherRegistered,"registered movement candidates");
 runNative([&]{const auto outcome=queryMovementStaticCandidates50L({true,true},movement,movementSource,registeredMovement,registeredCount,movementWorkspace,movementOutput,movementResult,movementFlag);movementFlag=outcome.vehicle7A0;return outcome.status==Type3QueryStatus48P::Complete;},"actual movement segment against terrain");
 runNative([&]{return commitCandidateEffects(movementEffects);},"commit movement query candidate effects");movement.filter.flagsF8=movementResult.queryFlags;movement.initial=movementResult.state;movement.initialQueryCurrent=movementResult.currentCandidate;movementInitial=movementDestination;
 MovementQueryCompletion50M completion{};completion.resultFlagsF8=movementResult.queryFlags;std::memcpy(&completion.selectedPoint,&movementResult.state.selectedPoint,16);completion.pushDistance434710=10;completion.representedStatePreserved=completion.omittedBB0WNotConsumed=true;
 runNative([&]{return finishVehicleMovementQuery50M(s,x.bindings,x.parameters,completion,x.workspace,x.diagnostic);},"native actual movement result and frame tail");
 require50EK(s.phase==VehicleFramePhase49W::Ready,"one-substep diagnostic frame complete");
 addCounter50EK(completedFrames);
 if(spring.records.contact3C)addCounter50EK(mainContactFrames);
 if(s.bodies.vehicle838&&secondarySpring.records.contact3C)addCounter50EK(secondaryContactFrames);
 };

 // Actual retained TA visual mesh, already placed by the49G/48F exporter.
 // This visual set is broader than the37-collider diagnostic physics scope.
 rr::assets::Mesh48B trackMesh;std::string trackLoadError;
 const bool trackLoaded=rr::assets::loadMesh48B(
     "evidence/visual49G/export-placed-normal/ta-world.rrmesh",trackMesh,trackLoadError);
 require50EK(trackLoaded,trackLoadError.empty()?"retained TA visual mesh load failed":trackLoadError.c_str());
 // Declaration and explicit close order both keep context alive beneath all
 // borrowers: renderer -> immutable GPU texture publication -> EGL context.
 platform::OffscreenGlContext50DC context;
 platform::MaterialTextureGl50EB gpuTextures;
 platform::MaterialTextureGlDiagnostic50EB textureDiagnostic{};
 platform::VehicleWindowGl50DS renderer;
 platform::OffscreenGlDiagnostic50DC contextDiagnostic{};
 platform::VehicleWindowDiagnostic50DS rendererDiagnostic{};
 auto closeHeadless=[&](){
  bool clean=true;
  if(renderer.ownsResources()&&!renderer.close(rendererDiagnostic)){
   reportRenderer50EK(rendererDiagnostic);clean=false;
  }
  if(renderer.ownsResources())return false;
  if(gpuTextures.ownsResources()&&!gpuTextures.close(textureDiagnostic)){
   reportTextures50EK(textureDiagnostic);clean=false;
  }
  if(gpuTextures.ownsResources())return false;
  if(context.ownsResources()&&!context.close(contextDiagnostic)){
   reportContext50EK(contextDiagnostic);clean=false;
  }
  return clean;
 };
 try {
  constexpr std::int32_t width=320,height=240;
  constexpr std::size_t rgbaBytes=std::size_t(width)*std::size_t(height)*4;
  platform::OffscreenGlRequest50DC contextRequest{};
  contextRequest.platform=platform::OffscreenGlPlatform50DC::MesaSurfaceless;
  contextRequest.width=width;contextRequest.height=height;
  contextRequest.major=3;contextRequest.minor=2;
  contextRequest.profile=platform::OffscreenGlProfile50DC::Core;
  contextRequest.redBits=contextRequest.greenBits=contextRequest.blueBits=contextRequest.alphaBits=8;
  contextRequest.depthBits=24;contextRequest.stencilBits=0;contextRequest.samples=0;
  if(!context.create(contextRequest,contextDiagnostic)){
   reportContext50EK(contextDiagnostic);throw std::runtime_error("surfaceless EGL context creation failed");
  }
  const auto&contextInfo=context.info();
  result["context"]={{"platform","MesaSurfaceless"},{"width",contextInfo.width},
      {"height",contextInfo.height},{"gl_major",contextInfo.glMajor},{"gl_minor",contextInfo.glMinor},
      {"core_profile",contextInfo.profile==platform::OffscreenGlProfile50DC::Core}};
  platform::VehicleWindowRequest50DS renderRequest{};
  renderRequest.context=&context;
  renderRequest.procAddress=[](void* context,const char* name) noexcept -> platform::VehicleWindowGlProc50DS {
   return static_cast<platform::OffscreenGlContext50DC*>(context)->procAddress(name);
  };
  renderRequest.isCurrent=[](void* context) noexcept {
   return static_cast<platform::OffscreenGlContext50DC*>(context)->current();
  };
  renderRequest.maximumWidth=width;renderRequest.maximumHeight=height;
  renderRequest.output=platform::VehicleWindowOutput50DS::headless;
  // Explicit diagnostic material policy: authored pass1/slot1, largest retained
  // mip, linear filtering and repeat. This does not reproduce original shaders.
  platform::MaterialTextureGlRequest50EB textureRequest{};
  textureRequest.context=&context;textureRequest.isCurrent=renderRequest.isCurrent;
  const char* missingTextureProc=nullptr;
  if(!platform::loadMaterialTextureFunctions50EE(&context,renderRequest.procAddress,
      textureRequest.gl,missingTextureProc)){
   result["missing_texture_entry_point"]=missingTextureProc?missingTextureProc:"unknown";
   throw std::runtime_error("required texture GL entry point unavailable");
  }
  const auto* textureBank=materialOwner->bank();require50EK(textureBank,"cold texture bank missing");
  std::vector<platform::MaterialTextureSampler50EB> samplers(textureBank->textureCount(),
      {platform::MaterialTextureFilter50EB::linear,platform::MaterialTextureFilter50EB::linear,
       platform::MaterialTextureWrap50EB::repeat,platform::MaterialTextureWrap50EB::repeat});
  textureRequest.samplers=samplers.data();textureRequest.samplerCount=static_cast<std::uint32_t>(samplers.size());
  textureRequest.maximumGpuBytes=1024u*1024u;
  require50EK(textureBank->textureCount()==5,"exactly five retained Pod textures required");
  if(!gpuTextures.create(textureRequest,*textureBank,textureDiagnostic)){
   reportTextures50EK(textureDiagnostic);throw std::runtime_error("actual Pod texture upload failed");
  }
  result["texture_count"]=gpuTextures.textureCount();
  result["texture_rgba_bytes"]=gpuTextures.gpuByteCount();
  require50EK(gpuTextures.gpuByteCount()==622592,"exact five-texture RGBA byte extent required");
  std::vector<platform::VehicleWindowMaterialBinding50DS> materialBindings[3];
  constexpr std::size_t expectedMaterialCounts[]={10,8,8};
  for(unsigned component=0;component<3;++component){
   const auto* selected=materialOwner->component(component);require50EK(selected,"dense material selection missing");
   require50EK(selected->size()==expectedMaterialCounts[component],"exact26 dense original material slots required");
   for(const auto& entry:*selected){
    require50EK(entry.pass==1&&entry.slot==1&&
        entry.requirement==rr::assets::MaterialTextureRequirement50EA::required&&
        entry.textureIndex<5,"all material selections must require exact pass1/slot1 textures");
    materialBindings[component].push_back(
       {platform::VehicleWindowMaterialMode50DS::requiredTexture,entry.textureIndex});
   }
   renderRequest.materials.component[component]={materialBindings[component].data(),
       static_cast<std::uint32_t>(materialBindings[component].size())};
  }
  renderRequest.materials.textures=&gpuTextures;
  renderRequest.mode=platform::VehicleWindowMode50DS::callerMaterialTextures;
  renderRequest.track.mesh=&trackMesh;
  renderRequest.track.maximumVertexBytes=4u*1024u*1024u;
  renderRequest.track.maximumIndexBytes=2u*1024u*1024u;
  renderRequest.track.mode=platform::VehicleWindowMode50DS::viewerNeutralUntextured;
  if(!renderer.create(renderRequest,drawAssets,rendererDiagnostic)){
   reportRenderer50EK(rendererDiagnostic);throw std::runtime_error("headless shared50DS renderer creation failed");
  }
  // Renderer owns its cold GPU copy; release CPU-only track staging promptly.
  trackMesh=rr::assets::Mesh48B{};
  materialOwner.reset();
  samplers.clear();
  for(auto& bindings:materialBindings)std::vector<platform::VehicleWindowMaterialBinding50DS>{}.swap(bindings);
  VehiclePresentState50DA presenter{};
  std::vector<std::uint8_t> capturedRgba(rgbaBytes);
  result["scenario_runs"]=1;
  for(unsigned frame=0;frame<160;++frame){
   composeScriptPacket(frame);
   advanceFrame();
   platform::VehicleViewCamera50DG camera{};
   for(unsigned axis=0;axis<3;++axis)camera.target[axis]=m.components.root.body.position.v[axis];
   // Same world-aligned diagnostic follow camera as50DH/50DU, not game parity.
   camera.eye[0]=camera.target[0]-45.f;camera.eye[1]=camera.target[1]+25.f;
   camera.eye[2]=camera.target[2]-65.f;camera.up[1]=1.f;
   camera.verticalFovRadians=1.0471975512f;
   camera.aspectWidthOverHeight=float(width)/float(height);
   camera.nearPlane=1.f;camera.farPlane=2000.f;
   platform::VehicleViewMatrix50DG view{};platform::VehicleViewCameraError50DG cameraError{};
   runNative([&]{return platform::composeVehicleViewCamera50DG(camera,view,cameraError);},
       "diagnostic vehicle camera rejected native pose");
   const auto bank=static_cast<std::uint8_t>(presenter.hasPublished?(presenter.publishedCpuBank^1u):0u);
   VehicleDrawBatchInput50CO frameDrawInput{};
   VehicleDrawBatch50CO next{};VehicleDrawBatchDiagnostic50CO drawDiagnostic{};
   const float clearColor[]={.035f,.045f,.08f,1.f};
   // Match50DH's logical present interval: CPU bank/preparation + renderer
   // submission. No logging, formatting, assertions or readback in this span.
   // Record even a rejected submission before inspecting its diagnostics.
   const std::uint64_t allocationsBeforeSubmit=::allocations;
   const bool bankReady=drawAssets.batchInput(bank,ModelPalettePhase49S::selectedRenderPreparation,
       ModelPaletteSceneState49S::completeCurrentScene,frameDrawInput);
   const bool prepared=bankReady&&buildVehicleDrawBatch50CO(s,x.bindings,frameDrawInput,
       next,drawWorkspace,drawDiagnostic);
   const bool accepted=prepared&&renderer.submit(next,
       {width,height,view.worldViewProjection,clearColor},rendererDiagnostic);
   const std::uint64_t allocationsAfterSubmit=::allocations;
   const auto observation=submissionAllocations.observe(allocationsBeforeSubmit,allocationsAfterSubmit);
   if(prepared)addCounter50EK(preparedDrawFrames);
   if(accepted){
    presenter.published=next;presenter.publishedCpuBank=bank;presenter.hasPublished=true;
    addCounter50EK(presenter.committedFrames);addCounter50EK(submittedDrawFrames);
    maximumDrawVertices=std::max(maximumDrawVertices,next.vertices);
   }
   if(!bankReady)throw std::runtime_error("native draw bank preparation failed");
   if(!prepared){
    std::cerr<<J{{"case","headless_drive50ek"},{"failure_domain","draw_preparation"},
        {"frame",frame},{"component",drawDiagnostic.component},
        {"error",static_cast<unsigned>(drawDiagnostic.detail.error)}}.dump()<<'\n';
    throw std::runtime_error("actual vehicle mesh preparation failed");
   }
   if(!accepted){
    reportRenderer50EK(rendererDiagnostic);throw std::runtime_error("shared50DS headless submission failed");
   }
   require50EK(!rendererDiagnostic.defaultFramebufferMayBeModified,
       "headless submission must not write default framebuffer");
   require50EK(observation==rr::test::RenderAllocationObservation50EH::recorded,
       "submission allocation counter invalid or collector capacity exceeded");
  }
  result["peak_drive"]=peakDrive;result["peak_drive_difference"]=peakDriveDifference;
  result["brake_frames"]=brakeFrames;
  result["main_contact_frames"]=mainContactFrames;result["secondary_contact_frames"]=secondaryContactFrames;
  result["final_velocity"]=J::array({m.components.root.body.velocity.v[0],m.components.root.body.velocity.v[1],m.components.root.body.velocity.v[2]});
  result["final_position"]=J::array({m.components.root.body.position.v[0],m.components.root.body.position.v[1],m.components.root.body.position.v[2]});
  require50EK(completedFrames==160&&submittedDrawFrames==160,"one complete160-frame run required");
  require50EK(mainContactFrames>0&&secondaryContactFrames>0,"real support from source grid");
  require50EK(peakDrive>.9f&&peakDriveDifference>.1f&&brakeFrames>=30,
      "throttle differential steering and braking exercised");
  const auto& allocationResult=submissionAllocations.summary();
  require50EK(allocationResult.all.frames==160&&allocationResult.all.validFrames==160&&
      !allocationResult.all.sumOverflow&&!allocationResult.afterFirst.sumOverflow&&
      !allocationResult.capacityOverflow&&!allocationResult.rejectedFramesOverflow,
      "complete valid nonoverflowed render allocation observations required");
  if(!renderer.readbackRgba(capturedRgba.data(),capturedRgba.size(),rendererDiagnostic)){
   reportRenderer50EK(rendererDiagnostic);throw std::runtime_error("final committed RGBA readback failed");
  }
  require50EK(rendererDiagnostic.readbackWidth==width&&rendererDiagnostic.readbackHeight==height&&
      rendererDiagnostic.readbackBytes==rgbaBytes,"final commit readback extent mismatch");
  // OpenGL row zero is the bottom of the image. P6 stores the top row first;
  // flip rows once while dropping alpha, without redrawing or mutating physics.
  {
   std::ofstream image(argv[2],std::ios::binary);
   require50EK(bool(image),"PPM output path cannot be opened");
   image<<"P6\n"<<width<<' '<<height<<"\n255\n";
   for(std::int32_t row=height;row>0;--row)
    for(std::int32_t column=0;column<width;++column){
     const auto at=(std::size_t(row-1)*std::size_t(width)+std::size_t(column))*4;
     image.write(reinterpret_cast<const char*>(capturedRgba.data()+at),3);
    }
   image.close();require50EK(bool(image),"PPM write/close failed");
  }
  result["capture"]={{"path",argv[2]},{"width",width},{"height",height},
      {"rgba_bytes",rgbaBytes},{"ppm_row_order","top_first"},{"committed_frame",submittedDrawFrames}};
 }catch(...){result["cleanup_complete"]=closeHeadless();throw;}
 result["cleanup_complete"]=closeHeadless();
 require50EK(result["cleanup_complete"].get<bool>(),"renderer/texture/context cleanup reported a failure");
 result["control_scenario"]="50DH differential";
 result["controller_provider"]="persistent native48Y held state, preset4";
 result["collector_calls"]=collectedCalls;result["peak_collected"]=peakCollected;
 result["collection_domain"]="authored9-zone retained37, source global first-zone link";
 result["registered_candidates"]=registeredCount;result["effect_commands"]=effectCommands;
 result["phase"]=static_cast<unsigned>(s.phase);
 result["oracle_comparisons_in_frame_loop"]=false;
 result["track_shading"]="viewerNeutralUntextured";
 result["pod_material_policy"]="required pass1/slot1 largest-retained mip, linear repeat";
 result["original_shader_parity"]=false;
 result["passed"]=true;exitCode=0;
 }catch(const std::exception&e){result["failure"]=e.what();}
 catch(...){result["failure"]="unknown exception";}
 result["completed_frames"]=completedFrames;
 result["prepared_draw_frames"]=preparedDrawFrames;
 result["submitted_draw_frames"]=submittedDrawFrames;
 result["maximum_draw_vertices"]=maximumDrawVertices;
 result["checked_native_calls"]=nativeCalls;
 result["native_allocation_failures"]=nativeAllocationFailures;
 if(collectorFailed)result["collector_failure"]={
     {"phase",collectorFailurePhase},{"status",collectorFailureStatus}};
 result["render_allocations"]=allocationSummary50EK(submissionAllocations.summary());
 result["allocation_scope"]="thread-local C++ counter around bank preparation, draw batch and50DS submit; excludes cold setup, camera, readback and cleanup; owners unattributed";
 std::cout<<result.dump()<<'\n';
 return exitCode;
}
