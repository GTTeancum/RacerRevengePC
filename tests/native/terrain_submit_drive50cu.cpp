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
#include <chrono>
#include <algorithm>
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
int main(int argc,char**argv){try{
 f::require(argc==2||argc==3,"verified query fixture required");const bool balanced=argc==3&&std::string(argv[2])=="--balanced";const bool noSupport=argc==3&&std::string(argv[2])=="--no-support";std::fesetround(FE_TONEAREST);
 std::ifstream file(argv[1]);J corpus;file>>corpus;const J*chosen=nullptr;
 for(const auto&row:corpus["cases"])if(row["name"]=="podboxm.col-explicit37-authored-placements-forward")chosen=&row;
 f::require(chosen,"authored placement case present");const auto&row=*chosen;const auto&qj=row["query"];
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


 auto ground=authored_runtime_ground50cp_fixture::load({4,16,32,64});f::require(ground.lift==x.parameters.visual.verticalLimit7EC,"same actual Pod01 lift");std::vector<std::uint32_t> groundStack(ground.stackCount);const auto groundAllocations=::allocations;auto groundProbe=authored_runtime_ground50cp_fixture::resolve(ground,16,0,groundStack.data(),groundStack.size());f::require(groundProbe.phase==ResetProbePhase50AG::Hit&&::allocations==groundAllocations,"actual source ground height without hot allocations");BodyMatrix48G pose=ground.pose;for(unsigned i=0;i<4;++i)pose.v[12+i]=std::bit_cast<float>(groundProbe.staged[i]);
 std::uint32_t candidateZones[37]{};
 for(std::size_t k=0;k<candidates.size();++k){const auto&j=row["candidates"][row["order"][k].get<std::size_t>()];const auto name=authored_runtime_ground50cp_fixture::lower(corpus["meshes"][j["mesh"].get<std::size_t>()]["name"].get<std::string>());const authored_runtime_ground50cp_fixture::Owner*match=nullptr;for(const auto&o:ground.owners)if(authored_runtime_ground50cp_fixture::lower(o.name)==name){bool equal=true;for(unsigned n=12;n<15;++n)if(o.instance.world.v[n]!=candidates[k].candidate.collision.geometry.worldMatrix.v[n])equal=false;if(equal){match=&o;break;}}f::require(match,"actual collider placement matches explicit candidate");candidateZones[k]=match->zone;auto&radiusOwner=meshes[j["mesh"].get<std::size_t>()];f::require(radiusOwner.radii.size()==match->mesh.nodes.size(),"actual mesh radius extent");std::vector<std::uint32_t> topology(match->mesh.nodes.size()+match->mesh.faces.size());f::require(prepareStaticTargetRadii49X(match->mesh,radiusOwner.radii.data(),radiusOwner.radii.size(),{topology.data(),topology.size()},radiusOwner.maxDepth)==Type2ResourceStatus49X::Complete,"radii prepared from owned actual mesh");candidates[k].maxDepth=radiusOwner.maxDepth;stackCount=std::max(stackCount,1+shape.view.maxDepth+radiusOwner.maxDepth);auto&v=candidates[k].candidate.collision;v.geometry.mesh=&match->mesh;v.geometry.worldMatrix=match->instance.world;v.filter.categoryA4=match->instance.category;v.sphere={{{match->instance.sphere.center.v[0],match->instance.sphere.center.v[1],match->instance.sphere.center.v[2]}},match->instance.sphere.radius};}
 ground.audit=J{};
 f::require(setLocalMatrix49P({s.nodes,53},x.bindings.lifecycle.main190,pose)&&refreshSceneWorld49P({s.nodes,53},x.bindings.lifecycle.main190),"supplied initial pose");f::project(s,x.bindings);
 FreshPodGeometrySources49O sources{};sources.chariot=x.assets.hierarchy[2];sources.rightEngine=x.assets.hierarchy[1];sources.forward=x.assets.motions[4];sources.back=x.assets.motions[5];rr::assets::MotionBindingDiagnostic49O bindingDiagnostic{};
 f::require(rr::assets::readMotionNodeBindings49O(x.assets.hierarchy[2],reinterpret_cast<const char*>(x.assets.vut[2].data()),x.assets.vut[2].size(),sources.chariotBindings,bindingDiagnostic),"actual motion bindings");
 FreshPodGeometryWorkspace49O geometryWorkspace{};FreshPodGeometryResult49O geometry{};FreshPodGeometryDiagnostic49O geometryDiagnostic{};
 f::require(buildFreshPodGeometry49O(sources,geometryWorkspace,geometry,geometryDiagnostic),"actual pod geometry");
 const auto phy=c::e::readBytes("work/impactAuthoring49Y/Pod01.phy");const AuthoredSelectionContext49N selection{AuthoredClass49N::A,false,true,false,false,{}};
 const auto authored=frame49w_authored::bind(phy,geometry,selection,x.parameters,s,x.selectors,x.secondaryInputs);
 auto secondaryPose=pose;for(unsigned i=0;i<3;++i){volatile float a=pose.v[i]*x.secondaryInputs.targetRelative2F0.v[0];volatile float b=pose.v[4+i]*x.secondaryInputs.targetRelative2F0.v[1];volatile float c=pose.v[8+i]*x.secondaryInputs.targetRelative2F0.v[2];volatile float ab=a+b;volatile float abc=ab+c;volatile float position=pose.v[12+i]+abc;secondaryPose.v[12+i]=position;}
 f::require(setLocalMatrix49P({s.nodes,53},x.bindings.lifecycle.physical19C,secondaryPose)&&refreshSceneWorld49P({s.nodes,53},x.bindings.lifecycle.physical19C),"supplied secondary initial pose");
 f::require(setLocalMatrix49P({s.nodes,53},x.bindings.lifecycle.model1B4,secondaryPose),"supplied initial displayed pose");f::project(s,x.bindings);
 ResetBoundNode50BA resetModelNodes[18]{};ResetBoundsWorkspace50BA resetBoundsWorkspace{};ModelBoundsResult49O resetModelBounds{};
 f::require(x.assets.hierarchy[2].nodeCount==18,"actual chariot source node count");
 for(unsigned i=0;i<18;++i){rr::assets::PsgNode49O authoredNode{};f::require(rr::assets::readPsgNode49O(x.assets.hierarchy[2],i,authoredNode),"actual reset model bound");const auto index=static_cast<std::int32_t>(x.bindings.lifecycle.model1B4)+1+static_cast<std::int32_t>(i);resetModelNodes[i].node=static_cast<SceneNodeIndex49P>(index);const auto expectedParent=static_cast<std::int32_t>(x.bindings.lifecycle.model1B4)+1+authoredNode.parent;f::require(static_cast<std::int32_t>(s.nodes[index].parent180)==expectedParent,"source flat model parent mapping");for(unsigned j=0;j<3;++j){resetModelNodes[i].center140.v[j]=authoredNode.bounds[j];resetModelNodes[i].halfExtent150.v[j]=authoredNode.bounds[3+j];}}
 const auto beforeResetBounds=::allocations;
 if(!refreshResetModelBounds50BA({s.nodes,53},x.bindings.lifecycle.model1B4,resetModelNodes,18,resetBoundsWorkspace,resetModelBounds)){std::cerr<<"reset root flags "<<unsigned(s.nodes[static_cast<int>(x.bindings.lifecycle.model1B4)].sceneFlags190)<<" descendants";for(const auto&n:resetModelNodes)std::cerr<<' '<<unsigned(s.nodes[static_cast<int>(n.node)].sceneFlags190);std::cerr<<'\n';f::require(false,"reset model cache admission");}
 f::require(::allocations==beforeResetBounds,"reset model bounds no allocation");

 SecondaryActivationGlobals49V resetGlobals{};initializedSecondaryActivationGlobals49V(resetGlobals);resetMainRigidBody50AR(m,s.bodies.mainSphere,s.mainSupplement,resetGlobals);resetSecondaryActivation49V(s.bodies.secondary,s.secondarySupplement,resetGlobals);
 for(auto*body:{&m.components.root,&s.bodies.secondary.root}){f::require(matrixToQuaternion48G(body->scene,body->body.orientation),"prepared initial orientation");std::memcpy(body->body.position.v,body->scene.v+12,12);}
 ResetLifecycleGlobals50AT resetLifecycle{};initializedResetLifecycleGlobals50AT(resetLifecycle);f::require(resetLifecycleRecords50AT(s.bodies.vehicle838,s.lifecycle,resetLifecycle),"source reset lifecycle records");s.fields.applicationByte58=2;m.controls.player.flagsB9C=0x80;
 SpringGeneratedParameters48K settings{};settings.impact54=true;settings.friction50=true;settings.impact.disableLeverArms5C=true;settings.impact.correctPenetration64=true;settings.impact.normalForceScale98=.97f;settings.impact.positionCorrectionScaleCC=.2f;
 SpringResponseParameters48L response{};response.maxSpeed94=10000;rr::assets::SpringCollisionAuthoring50F collision{};rr::assets::PodPhysicsDiagnostic49N diagnostic{};
 f::require(rr::assets::readSpringCollision50F(reinterpret_cast<const char*>(phy.data()),phy.size(),collision,diagnostic)&&rr::assets::applySpringCollision50F(collision,x.parameters.bodies.main.body.mass,settings,response),"authored response settings");
 const auto tuning=c::e::readBytes("work/impactAuthoring49Y/ta.tun");f::require(rr::assets::readEnvironmentImpact49Y(reinterpret_cast<const char*>(tuning.data()),tuning.size(),x.parameters.impactTuning,diagnostic),"actual TA impact tuning");x.parameters.impactTuningBound=true;
 rr::assets::DefenseAuthoring49Y defense{};f::require(rr::assets::readDefenseAuthoring49Y(reinterpret_cast<const char*>(phy.data()),phy.size(),defense,diagnostic)&&defense.baseDefense.present,"explicit base-profile defense");s.fields.defenseCF0=defense.baseDefense.value;
 StaticSpringState50G spring{};spring.records.generatedCapacity88=40;SpringRecord48N records[60]{},queryRecords[60]{},scratchRecords[60]{};

 const J*secondaryCase=nullptr;for(const auto&item:corpus["cases"])if(item["name"]=="chariotm.col-explicit37-authored-placements-forward")secondaryCase=&item;
 f::require(secondaryCase,"actual chariot query present");const auto&sqj=(*secondaryCase)["query"];
 auto secondaryShape=cold::shape(corpus["shapes"][sqj["shape"].get<std::size_t>()]);auto secondaryFilter=cold::filter(sqj["filter"]);
 Type2StaticQuery49X secondaryQuery{};secondaryQuery.resource=&secondaryShape.view;secondaryQuery.filter=&secondaryFilter;secondaryQuery.queryScene.value=78;
 secondaryQuery.collisionTypeTag=sqj["collision_tag"];secondaryQuery.groupTypeTag=sqj["group_tag"];secondaryQuery.callbackEnabled=sqj["callback"];
 cold::words(secondaryQuery.copied40.data(),sqj["copied40"]);cold::words(secondaryQuery.copied50.data(),sqj["copied50"]);
 secondaryQuery.skipMask=(*secondaryCase)["masks"][0];secondaryQuery.dynamicMask=(*secondaryCase)["masks"][1];secondaryQuery.materialMask=(*secondaryCase)["masks"][2];
 f::require((*secondaryCase)["order"]==row["order"]&&(*secondaryCase)["candidates"]==row["candidates"],"same explicit ordered terrain inputs");
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
 f::require(rr::assets::applySpringCollision50F(collision,x.parameters.bodies.secondary.body.mass,secondarySettings,secondaryResponse),"authored secondary response settings");
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
 SearchStorage50BS registry{};f::require(candidates.size()==37,"retained37 terrain owners");
 f::require(initializeColdSearchStorage50BU(registry,1024)==SearchStorageStatus50BS::Complete,"source factory list capacity");std::uint32_t registeredSlot=0;
 f::require(adoptFirstSearchBlock50BU(registry,registeredSlot)==SearchStorageStatus50BS::Complete,"caller-owned first registration block");
 for(std::size_t i=0;i<candidates.size();++i){f::require(selectSearchSlot50BS(registry,0,registeredSlot)==SearchStorageStatus50BS::Complete&&registeredSlot==i+1,"ordered registration slot");f::require(insertSearchSlot50BS(registry,registeredSlot,static_cast<std::uint32_t>((i+1)*4))==SearchStorageStatus50BS::Complete,"register actual terrain owner handle");}
 if(noSupport)for(std::size_t i=0;i<candidates.size();++i)f::require(removeSearchSlot50BS(registry,static_cast<std::uint32_t>(i+1),static_cast<std::uint32_t>((i+1)*4))==SearchStorageStatus50BS::Complete,"unregister terrain negative control");
 Type2StaticCandidate49X registeredCandidates[37]{};Type3Candidate48P registeredMovement[37]{};std::size_t registeredIndices[37]{},registeredCount=0;
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
 f::require(initializeZoneLifecycle50CL(zoneLifecycle,zoneStorage,9,37,1024,zoneScratch)==SearchStorageStatus50BS::Complete,"persistent zone lifecycle initialization");
 ZoneRegistrationCommand50CQ zoneEvents[128]{};ZoneRegistrationQueue50CQ zoneQueue{zoneEvents,128,0,0};
 for(unsigned i=0;i<37;++i){f::require(enqueueZoneRegistration50CQ(zoneQueue,{ZoneRegistrationOperation50CQ::Set,i+1,candidateZones[i],ZoneMethod50CL::First})==ZoneQueueStatus50CQ::Complete,"queue authored zone owner registration");if(noSupport)f::require(enqueueZoneRegistration50CQ(zoneQueue,{ZoneRegistrationOperation50CQ::Remove,i+1,0,ZoneMethod50CL::First})==ZoneQueueStatus50CQ::Complete,"queue removal admission");}

 f::require(initializeGlobalZoneLink50CK(1,9,0,globalLink),"source global chain first-zone projection; opaque search identity supplied zero");
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
 std::size_t collectedCalls=0,peakCollected=0;
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
  if(result.phase!=VehicleCandidateResult50CA::Phase::Complete){std::cerr<<"collection phase "<<int(result.phase)<<" status "<<int(result.collectionStatus)<<'\n';return false;}
  if(resolveCandidateHandles50CB(mergedIds,result.count,ownerHandles,37,resolveScratch,64,registeredIndices,37)!=CandidateHandleStatus50CB::Complete)return false;
  registeredCount=static_cast<std::size_t>(result.count);peakCollected=std::max(peakCollected,registeredCount);++collectedCalls;
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
 f::require(decodeResource48C(raw,res,error),"decode actual Pod01");
 auto scene=[&](const char*name,Scene48D&out){return extractResMember48C(res,name,member,error)&&decodeScene48D(member,out,error);};
 auto material=[&](const char*name,Material48E&out){return extractResMember48C(res,name,member,error)&&decodeMaterial48E(member,out,error);};
 f::require(scene("pods\\pod01\\englt1.psg",left)&&scene("pods\\pod01\\engrt1.psg",right),"actual engine scenes");
 f::require(material("textures\\01_1.psm",shared)&&material("textures\\flame01l.psm",fl)&&material("textures\\flame01r.psm",fr),"actual flame materials");PodFlameError50W diag{};
 f::require(buildPodFlameOwners50W(left,right,shared,fl,fr,static_cast<unsigned>(x.bindings.visual.left1BC)+1,static_cast<unsigned>(x.bindings.visual.right1B8)+1,coldFlame,diag),"cold flame owners");
 }


 BaseTailState50X baseTail=coldBaseTailState50X(0x3f800000u,0x3f800000u);BaseTailGlobals50X baseGlobals{};BaseTailWorkspace50X baseWorkspace{};BasePoseHistoryState50Y poseHistory{};poseHistory.nextFD4=19;
 // Explicit diagnostic cold history: all positions start at supplied pod pose.
 for(auto&slot:poseHistory.historyD50){std::memcpy(slot,pose.v+12,12);slot[3]=0x3f800000u;}
 s.impactSupplement.partnerId=0xffffffffu;FrameTimingState49I clock{};clock.lastDispatched23C=.016f;

 // Fresh unrenamed native model roots; later renames/clones are not admitted.
 rr::assets::ModelRootName50BL rootNames[2]{};SceneNodeIndex49P namedIndices[2][14];rr::assets::EffectNameModel50BL namedModels[2]{};rr::assets::EffectNames50BL namesForEffects{};
 for(unsigned side=0;side<2;++side){rr::assets::initializeFreshModelRootName50BL(rootNames[side]);const auto root=side?x.bindings.visual.right1B8:x.bindings.visual.left1BC;f::require(x.assets.hierarchy[side].nodeCount==14,"actual engine node count");for(unsigned i=0;i<14;++i)namedIndices[side][i]=static_cast<SceneNodeIndex49P>(static_cast<int>(root)+1+i);namedModels[side]={root,&rootNames[side],x.assets.hierarchy[side],namedIndices[side],14};}
 f::require(rr::assets::buildEffectNames50BL({s.nodes,53},namedModels,2,namesForEffects),"actual effect attachment names");
 EffectOwner50BE nativeEffects[8]{};const char*effectFiles[]={"podLeftEngineDust.emt","podRightEngineDust.emt","podLeftEngineSmoke.emt","podRightEngineSmoke.emt","podSparks.emt","podSparks.emt","podFire.emt","podFire.emt"};
 for(unsigned i=0;i<8;++i){auto raw=c::e::readBytes(std::string("evidence/effectAuthoring50BF/members/")+effectFiles[i]);const auto profile=i<4?EffectProfile50BE::Particle19A5E0:i<6?EffectProfile50BE::Ordinary15F100:EffectProfile50BE::Fire15DE00;f::require(rr::assets::buildEffectControlOwner50BJ(i+1,profile,reinterpret_cast<const char*>(raw.data()),raw.size(),{0,0},nativeEffects[i]),"actual effect control construction");}
 ResetEffectBindings50BK resetEffectBindings{{1,2},{0,0},{5,6,0,0},false};ResetEffectState50BK resetEffectState{};std::memcpy(resetEffectState.timers820,s.impactSupplement.timers820,16);ResetEffectWorkspace50BK resetEffectWorkspace{};
 const auto resetAllocationCount=::allocations;f::require(resetEffectStops50BK(resetEffectBindings,nativeEffects,8,resetEffectState,resetEffectWorkspace).status==EffectDispatchStatus50BE::Complete&&::allocations==resetAllocationCount,"native reset effect stop sequence");std::memcpy(s.impactSupplement.timers820,resetEffectState.timers820,16);
 ImpactEffectsWorkspace50B effectsWorkspace{};ImpactEffectEvent50B generatedEffects[maxImpactEffectEvents50B]{};EffectDispatchWorkspace50BE effectDispatch{};
 unsigned effectCommands=0,effectStarts=0,effectPositions=0,effectStops=0;
 auto consumeEffects=[&](const ImpactEffectEvent50B*events,std::size_t count){const auto result=consumeEffectControls50BE(events,count,nativeEffects,8,effectDispatch);if(result.status!=EffectDispatchStatus50BE::Complete)return false;effectCommands+=static_cast<unsigned>(count);for(std::size_t i=0;i<count;++i){effectStarts+=events[i].kind==ImpactEffectKind50B::Start80;effectPositions+=events[i].kind==ImpactEffectKind50B::Position15ACB0;effectStops+=events[i].kind==ImpactEffectKind50B::Stop84;}return true;};
 unsigned completedFrames=0,mainContactFrames=0,secondaryContactFrames=0,brakeFrames=0;float peakDrive=0,peakDriveDifference=0;
 std::uint64_t nativeNs=0,nativeCalls=0,frameTimes[160]{};
 auto measuredHot=[&](auto&&fn,const char*label){x.hot([&]{const auto begin=std::chrono::steady_clock::now();const bool ok=fn();const auto end=std::chrono::steady_clock::now();nativeNs+=std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();++nativeCalls;return ok;},label);};

 // Load retained semantic model assets once. The next hourly smoke will exercise
 // this new connection; there is no renderer/device execution at cold load.
 rr::assets::VehicleDrawAssets50CR drawAssets;rr::assets::VehicleDrawAssetsInput50CR drawInput{};
 rr::assets::VehicleDrawAssetDiagnostic50CR drawLoadDiagnostic{};
 std::vector<std::uint8_t> drawBytes[3];SceneNodeIndex49P drawNodes[3][18]{};std::uint8_t drawPalette[3][18]{};
 const char*drawKeys[]={"cb6d91807cca98eecb547dfd4b030345e8e88e6ab9363fc436012732487863b5","210c524d329161a97e79baecded9c34bb6949b6910dea148ce2bf5cb0ddb2fc3","f073a4149ace5c36bf51d3390c867cddabedca939e1a876d0efef8cf8d438d60"};
 const unsigned drawCounts[]={18,14,14},drawBases[]={31,1,16},drawMapped[]={2,0,1};
 for(unsigned component=0;component<3;++component){
  drawBytes[component]=c::e::readBytes(std::string("evidence/visual49G/cacheobjects/")+drawKeys[component]+".rrscene");
  const auto&psg=x.assets.psg[drawMapped[component]];const auto padded=(drawCounts[component]+3)&~3u;
  f::require(psg.size()>=12+padded,"actual palette suffix bounds");const auto suffix=psg.size()-(12+padded);
  auto word=[&](std::size_t at){return std::uint32_t(psg[at])|(std::uint32_t(psg[at+1])<<8)|(std::uint32_t(psg[at+2])<<16)|(std::uint32_t(psg[at+3])<<24);};
  f::require(word(suffix)==1&&word(suffix+4)==padded&&word(suffix+8+padded)==drawCounts[component],"actual single palette record");
  for(unsigned n=0;n<drawCounts[component];++n){drawPalette[component][n]=psg[suffix+8+n];drawNodes[component][n]=static_cast<SceneNodeIndex49P>(drawBases[component]+n);}
  drawInput.component[component]={&drawBytes[component],drawNodes[component],drawCounts[component],drawPalette[component],drawCounts[component]};
 }
 f::require(drawAssets.load(drawInput,drawLoadDiagnostic),"cold native draw owners");
 VehicleDrawWorkspace50CM drawWorkspace{};VehicleDrawBatch50CO drawBatch{};VehicleDrawBatchDiagnostic50CO drawDiagnostic{};
 std::size_t preparedDrawFrames=0;std::uint32_t maximumDrawVertices=0;
 std::vector<rr::assets::Vertex48B> recordedVertices[2][3];VehicleDrawCommand50CS recordedCommands[2][maxVehicleDrawCommands50CS]{};
 VehicleDrawRecordingStorage50CS recordStorage[2]{};
 for(unsigned bank=0;bank<2;++bank){for(unsigned component=0;component<3;++component){const auto*mesh=drawAssets.preparedMesh(static_cast<VehicleDrawComponent50CM>(component));f::require(mesh,"loaded draw owner");recordedVertices[bank][component].resize(mesh->vertexCount);recordStorage[bank].vertices[component]={recordedVertices[bank][component].data(),mesh->vertexCount};}recordStorage[bank].commands=recordedCommands[bank];recordStorage[bank].commandCapacity=maxVehicleDrawCommands50CS;}
 VehicleDrawRecorder50CS drawRecorder(recordStorage[0],recordStorage[1]);const auto drawBackend=drawRecorder.backend();VehicleDrawSubmitDiagnostic50CS submitDiagnostic{};
 std::size_t submittedDrawFrames=0;std::uint32_t maximumDrawCommands=0;

 for(unsigned frame=0;frame<160;++frame){x.frame=frame;const auto beforeNs=nativeNs;
 FrameReader49K read{};read.packet={128,128,128,128};
 // Explicit advanced dual-engine profile: packet7/5 are throttle/brake axes.
 if(frame>=10&&frame<40)read.packet={128,0,128,0};
 if(frame>=40&&frame<80)read.packet={128,static_cast<std::uint8_t>(balanced?0:70),128,0};
 if(frame>=100)read.packet={128,255,128,255};read.preparedFactor=.016f;read.actions={nullptr,f::noAction};read.codes=x.actionCodes;read.context.previousFlagsB9C=s.bodies.main.controls.player.flagsB9C;
 measuredHot([&]{return readVehicleFrame49W(s,x.bindings,read,x.workspace,x.diagnostic);},"persistent reader");PostPlayerAt217DE0_49K post{};post.player=s.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=x.configurationEntryNonzero;
 measuredHot([&]{return bindVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,x.workspace,x.diagnostic);},"persistent binding");const auto&liveDrive=s.bodies.main.components.bindings;peakDrive=std::max(peakDrive,std::max(liveDrive.engineDrive[0],liveDrive.engineDrive[1]));peakDriveDifference=std::max(peakDriveDifference,std::abs(liveDrive.engineDrive[0]-liveDrive.engineDrive[1]));if(liveDrive.aeroBrakeEC>0)++brakeFrames;
 measuredHot([&]{return graphicsVehicleFrame49W(s,x.bindings,x.parameters,f::cut(PreservingCut49W::Base2176E0BeforeGraphics),x.workspace,x.diagnostic);},"persistent graphics");
 ImpactConsumerInput49Z consumer{};consumer.managerFlags58=2;consumer.profile34=2;consumer.finalDamageThreshold64=1;VehicleImpactEvents49Z consumerEvents{};
 measuredHot([&]{return consumeVehicleImpacts49Z(s,x.bindings,consumer,consumerEvents,x.workspace,x.diagnostic);},"persistent impact consumers");measuredHot([&]{return repairVehicleImpacts50A(s,x.bindings,x.workspace,x.diagnostic);},"persistent repair");
 ImpactEffectsInput50B owners{};owners.models[0].node=x.bindings.visual.left1BC;owners.models[1].node=x.bindings.visual.right1B8;owners.models[2].node=owners.models[3].node=SceneNodeIndex49P::none;std::size_t count=0;owners.damage00Matches=namesForEffects.damage00;owners.nameCount=53;for(unsigned side=0;side<2;++side){auto&model=owners.models[side];f::require(namesForEffects.modelRoots&(std::uint64_t(1)<<static_cast<unsigned>(model.node)),"bound model name scope");model.flags03=3;model.effect800={nativeEffects[2+side].id,nativeEffects[2+side].state.flags1D4};model.effect804={nativeEffects[4+side].id,nativeEffects[4+side].state.flags1D4};model.effect80C={nativeEffects[6+side].id,nativeEffects[6+side].state.flags1D4};}
 measuredHot([&]{return effectsVehicleImpacts50O(s,x.bindings,owners,effectsWorkspace,generatedEffects,maxImpactEffectEvents50B,count,x.workspace,x.diagnostic);},"actual effect control bindings");measuredHot([&]{return consumeEffects(generatedEffects,count);},"consume impact effect controls");OptionalProbeOwners50P optionalOwners{1,1,{1,2}};OptionalProbeResult50P optional{};
 measuredHot([&]{return optionalProbeVehicleImpacts50P(s,x.bindings,optionalOwners,optional,x.workspace,x.diagnostic);},"supplied manager-present disabled-probe context");measuredHot([&]{return consumeEffects(optional.events,optional.count);},"consume dust stop controls");ImpactTransitionResult50C decision{};
 measuredHot([&]{return decideVehicleImpacts50P(s,x.bindings,0x3f800000u,0,decision,x.workspace,x.diagnostic);},"ordinary damage decision");PlainEffectOwners50Q plainOwners{};for(unsigned side=0;side<2;++side){for(unsigned array=0;array<3;++array){const auto&owner=nativeEffects[2+array*2+side];plainOwners.slots[array*4+side]={owner.id,owner.state.flags1D4};}}PlainEffectResult50Q plain{};
 measuredHot([&]{return plainEffectsVehicleTail50Q(s,x.bindings,plainOwners,plain,x.workspace,x.diagnostic);},"plain effects");measuredHot([&]{return consumeEffects(plain.events,plain.count);},"consume plain effect controls");measuredHot([&]{return packVehicleFrame50S(s,x.bindings,x.configurationEntryNonzero,x.workspace,x.diagnostic);},"packet");BaseTailBindings50X baseBinding{77,Probe128Binding50X::Null};
 measuredHot([&]{return advanceVehicleBaseThroughEngine50X(s,x.bindings,clock,baseBinding,baseTail,baseGlobals,coldFlame,baseWorkspace,x.workspace,x.diagnostic);},"persistent base engine");
 measuredHot([&]{return finishVehicleBaseAndMainCache50Y(s,x.bindings,baseTail,true,poseHistory,f::cut(PreservingCut49W::OtherChildrenBeforeMain340),x.workspace,x.diagnostic);},"persistent base pose history");
 measuredHot([&]{return refreshSecondaryVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::OtherChildrenBetween340And35C),x.workspace,x.diagnostic);},"secondary cache");measuredHot([&]{return scheduleVehicleFrame49W(s,x.bindings,f::cut(PreservingCut49W::OtherChildrenAfter35C),60,x.workspace,x.diagnostic);},"schedule");
 f::require(s.bodies.main.schedule.count==1,"single substep diagnostic cadence");if(!frame)f::require(s.bodies.vehicle838==1,"original initial lifecycle activates secondary body");

 measuredHot([&]{return beginVehicleSubstep49W(s,x.bindings,x.selectors,x.workspace,x.diagnostic);},"native selector");
 MainStaticDetectorAdmission50H admission{true,true,true,false,false,false,true,true,2,0,x.bindings.lifecycle.main190};
 measuredHot(collectRegistered,"ordered prepared-list vehicle candidates");
 measuredHot([&]{return queryMainIndexedStaticVehicle50CD(s,x.bindings,admission,query,candidates.data(),candidates.size(),registeredIndices,registeredCount,initial,workspace,destination,queryResult)==Type2StaticStatus49X::Complete;},"actual terrain query");
 measuredHot([&]{return commitCandidateEffects(effects);},"commit main query candidate effects");qfilter.flagsF8=queryResult.queryFlagsF8;query.initialQueryCurrentIdentity=queryResult.currentCandidateIdentity;initial.selected=queryResult.selected;
 std::cerr<<"frame "<<frame<<" main_hit "<<queryResult.hasGeometryHit<<" contacts "<<queryResult.records.generatedCount84<<" position "<<m.components.root.body.position.v[0]<<","<<m.components.root.body.position.v[1]<<","<<m.components.root.body.position.v[2]<<"\n"; spring.records=queryResult.records;std::memcpy(records,queryRecords,sizeof records);

 if(s.bodies.vehicle838){
 MainStaticDetectorAdmission50H secondaryAdmission=admission;secondaryAdmission.queryScene=x.bindings.lifecycle.physical19C;
 measuredHot([&]{return prepareSecondaryQueryScene50J(s,x.bindings,secondaryAdmission,x.workspace)==SecondaryQuerySceneStatus50J::Ready;},"early original secondary scene getter");
 // Secondary uses the same first-substep merged list, without recollection.

 measuredHot([&]{return querySecondaryIndexedStaticVehicle50CD(s,x.bindings,secondaryAdmission,secondaryQuery,candidates.data(),candidates.size(),registeredIndices,registeredCount,si,sw,sd,secondaryResult)==Type2StaticStatus49X::Complete;},"actual secondary terrain query");
 measuredHot([&]{return commitCandidateEffects(sde);},"commit secondary query candidate effects");secondaryFilter.flagsF8=secondaryResult.queryFlagsF8;secondaryQuery.initialQueryCurrentIdentity=secondaryResult.currentCandidateIdentity;si.selected=secondaryResult.selected;
 secondarySpring.records=secondaryResult.records;std::memcpy(secondaryRecords,secondaryQueryRecords,sizeof secondaryRecords);
 
 }
 const auto queryStage=s;
 PostWorld49W world{};world.point=s.bodies.vehicle838?WorldCut49W::First358Secondary36CAnd219420:WorldCut49W::First358And219420;world.effects=contactEffects49W(s);world.applicationGateByte58=2;
 measuredHot([&]{return produceVehicleForces49W(s,x.bindings,x.parameters,world,x.workspace,x.diagnostic);},"native main forces");
 const auto beforeResponse=s;ResponseStorage50K responseStorage;auto responseWorkspace=responseStorage.view(x.workspace);
 measuredHot([&]{return respondMainStaticVehicle50G(s,x.bindings,x.parameters,settings,response,x.secondaryInputs,query.queryScene,1,spring,records,40,responseWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual main contact response");
 f::require(s.phase==(s.bodies.vehicle838?VehicleFramePhase49W::Post368:VehicleFramePhase49W::MainIntegration),"source-selected response phase");
 if(queryResult.records.generatedCount84)f::require(std::memcmp(&beforeResponse.bodies.main.components.root,&s.bodies.main.components.root,sizeof s.bodies.main.components.root)!=0,"contact response changes canonical body");

 if(s.bodies.vehicle838){
 ResponseStorage50K secondaryStorage;auto secondaryWorkspace=secondaryStorage.view(x.workspace);
 measuredHot([&]{return respondSecondaryStaticVehicle50J(s,x.bindings,x.parameters,secondarySettings,secondaryResponse,{78},2,secondarySpring,secondaryRecords,60,secondaryWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual secondary contact and integration");
 f::require(s.phase==VehicleFramePhase49W::MainIntegration,"secondary response integration phase");
 }
 measuredHot([&]{return integrateVehicleMain49W(s,x.bindings,x.parameters,x.workspace,x.diagnostic);},"contact-driven body integration");
 f::require(s.phase==VehicleFramePhase49W::Query219120,"real late-query boundary");
 std::memcpy(&endpoints[0],s.bodies.main.priorPoint7B0,16);std::memcpy(&endpoints[1],s.bodies.main.currentPoint7C0,16);
 measuredHot(gatherRegistered,"registered movement candidates");
 measuredHot([&]{const auto outcome=queryMovementStaticCandidates50L({true,true},movement,movementSource,registeredMovement,registeredCount,movementWorkspace,movementOutput,movementResult,movementFlag);movementFlag=outcome.vehicle7A0;return outcome.status==Type3QueryStatus48P::Complete;},"actual movement segment against terrain");
 measuredHot([&]{return commitCandidateEffects(movementEffects);},"commit movement query candidate effects");movement.filter.flagsF8=movementResult.queryFlags;movement.initial=movementResult.state;movement.initialQueryCurrent=movementResult.currentCandidate;movementInitial=movementDestination;
 MovementQueryCompletion50M completion{};completion.resultFlagsF8=movementResult.queryFlags;std::memcpy(&completion.selectedPoint,&movementResult.state.selectedPoint,16);completion.pushDistance434710=10;completion.representedStatePreserved=completion.omittedBB0WNotConsumed=true;
 measuredHot([&]{return finishVehicleMovementQuery50M(s,x.bindings,x.parameters,completion,x.workspace,x.diagnostic);},"native actual movement result and frame tail");
 f::require(s.phase==VehicleFramePhase49W::Ready,"one-substep diagnostic frame complete");
 VehicleDrawBatchInput50CO drawFrame{};
 measuredHot([&]{return drawAssets.batchInput(frame&1u,ModelPalettePhase49S::selectedRenderPreparation,ModelPaletteSceneState49S::completeCurrentScene,drawFrame)&&buildVehicleDrawBatch50CO(s,x.bindings,drawFrame,drawBatch,drawWorkspace,drawDiagnostic);},"native animated vehicle draw preparation");
 ++preparedDrawFrames;maximumDrawVertices=std::max(maximumDrawVertices,drawBatch.vertices);
 measuredHot([&]{return submitVehicleDrawBatch50CS(drawBatch,drawBackend,submitDiagnostic);},"native ordered vehicle draw submission");
 ++submittedDrawFrames;const auto*recording=drawRecorder.recording();f::require(recording,"committed draw record");maximumDrawCommands=std::max(maximumDrawCommands,recording->commandCount);



 frameTimes[frame]=nativeNs-beforeNs; ++completedFrames;if(spring.records.contact3C)++mainContactFrames;if(s.bodies.vehicle838&&secondarySpring.records.contact3C)++secondaryContactFrames;
 }
 if(!noSupport)f::require(mainContactFrames>0&&secondaryContactFrames>0,"real support from source grid");
 if(noSupport)f::require(mainContactFrames==0&&secondaryContactFrames==0,"actual deregistration control generates no support");
 f::require(peakDrive>.9f&&(balanced?peakDriveDifference<.0001f:peakDriveDifference>.1f)&&brakeFrames>=30,"throttle differential steering and braking exercised");
 std::sort(frameTimes,frameTimes+160);
 const auto actual=s;const auto actualSpring=spring;const auto actualSecondarySpring=secondarySpring;
 std::cout<<J{{"submitted_draw_frames",submittedDrawFrames},{"maximum_draw_commands",maximumDrawCommands},{"submission_backend","headless command recorder"},{"prepared_draw_frames",preparedDrawFrames},{"maximum_draw_vertices",maximumDrawVertices},{"oracle_comparisons_in_frame_loop",false},{"collector_calls",collectedCalls},{"peak_collected",peakCollected},{"collection_domain","persistent authored9-zone retained37 registry, source global first-zone link"},{"candidate_access","borrowed indexed main/secondary owners"},{"registration_order","explicit supplied cold order"},{"registered_candidates",registeredCount},{"registration_capacity",registry.perBlock},{"registration_storage_bytes",sizeof registry+sizeof registeredCandidates+sizeof registeredMovement+sizeof registeredIndices+sizeof registrationBlocks+sizeof registrationView+sizeof registrationCursor},{"effect_commands",effectCommands},{"effect_starts",effectStarts},{"effect_positions",effectPositions},{"effect_stops",effectStops},{"chariot_reset_bounds_radius",resetModelBounds.radius198},{"chariot_reset_bounds",true},{"source_lifecycle_reset",true},{"source_body_resets",true},{"grid_rank",4u},{"start_position",J::array({pose.v[12],pose.v[13],pose.v[14]})},{"control_scenario",balanced?"balanced":"differential"},{"peak_drive",peakDrive},{"peak_drive_difference",peakDriveDifference},{"brake_frames",brakeFrames},{"final_velocity",J::array({m.components.root.body.velocity.v[0],m.components.root.body.velocity.v[1],m.components.root.body.velocity.v[2]})},{"no_support_control",noSupport},{"final_position",J::array({m.components.root.body.position.v[0],m.components.root.body.position.v[1],m.components.root.body.position.v[2]})},{"timed_native_calls",nativeCalls},{"native_total_ns",nativeNs},{"native_frame_median_ns",frameTimes[80]},{"native_frame_p95_ns",frameTimes[151]},{"timing_scope","host native call segments only; excludes cold loading, reference checks, rendering and presentation"},{"completed_frames",completedFrames},{"main_contact_frames",mainContactFrames},{"secondary_contact_frames",secondaryContactFrames},{"main_contacts",actualSpring.records.generatedCount84},{"secondary_contacts",actualSecondarySpring.records.generatedCount84},{"movement_hit",movementResult.hasAnyHit},{"vehicle7a0",movementFlag},{"checks",f::checks},{"hot_allocations",0},{"phase",static_cast<unsigned>(s.phase)}}.dump()<<'\n';
 return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
