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
 f::require(argc==2,"verified query fixture required");std::fesetround(FE_TONEAREST);
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
 f::Fixture x(4);auto&s=x.state;auto&m=s.bodies.main;
 const auto pose=cold::matrix(qj["world"]);
 f::require(setLocalMatrix49P({s.nodes,53},x.bindings.lifecycle.main190,pose)&&refreshSceneWorld49P({s.nodes,53},x.bindings.lifecycle.main190),"supplied initial pose");f::project(s,x.bindings);
 FreshPodGeometrySources49O sources{};sources.chariot=x.assets.hierarchy[2];sources.rightEngine=x.assets.hierarchy[1];sources.forward=x.assets.motions[4];sources.back=x.assets.motions[5];rr::assets::MotionBindingDiagnostic49O bindingDiagnostic{};
 f::require(rr::assets::readMotionNodeBindings49O(x.assets.hierarchy[2],reinterpret_cast<const char*>(x.assets.vut[2].data()),x.assets.vut[2].size(),sources.chariotBindings,bindingDiagnostic),"actual motion bindings");
 FreshPodGeometryWorkspace49O geometryWorkspace{};FreshPodGeometryResult49O geometry{};FreshPodGeometryDiagnostic49O geometryDiagnostic{};
 f::require(buildFreshPodGeometry49O(sources,geometryWorkspace,geometry,geometryDiagnostic),"actual pod geometry");
 const auto phy=c::e::readBytes("work/impactAuthoring49Y/Pod01.phy");const AuthoredSelectionContext49N selection{AuthoredClass49N::A,false,true,false,false,{}};
 const auto authored=frame49w_authored::bind(phy,geometry,selection,x.parameters,s,x.selectors,x.secondaryInputs);
 auto secondaryPose=pose;for(unsigned i=0;i<3;++i)secondaryPose.v[12+i]+=x.secondaryInputs.targetRelative2F0.v[i];
 f::require(setLocalMatrix49P({s.nodes,53},x.bindings.lifecycle.physical19C,secondaryPose)&&refreshSceneWorld49P({s.nodes,53},x.bindings.lifecycle.physical19C),"supplied secondary initial pose");
 f::require(setLocalMatrix49P({s.nodes,53},x.bindings.lifecycle.model1B4,secondaryPose),"supplied initial displayed pose");f::project(s,x.bindings);
 for(auto*body:{&m.components.root,&s.bodies.secondary.root}){
  f::require(matrixToQuaternion48G(body->scene,body->body.orientation),"initial orientation");body->body.velocity={};body->body.angularVelocity={};body->body.linearAcceleration={};body->body.angularAcceleration={};
  body->body.previousPositionDerivative={};body->body.previousLinearAcceleration={};body->body.previousAngularAcceleration={};body->body.previousOrientationDerivative={};
 }
 m.components.root.body.velocity={{0,-1,4}}; // Explicit low-speed state: the original lifecycle activates the secondary body.
 m.components.root.body.previousPositionDerivative=m.components.root.body.velocity;
 s.bodies.vehicle838=0;s.lifecycle.vehicle83C=1;s.fields.applicationByte58=2;m.controls.player.flagsB9C=0x80;
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
 auto commitCandidateEffects=[&](const std::vector<TerrainCandidateEffects48J>&changed){
  if(changed.size()!=candidates.size())return false;
  for(std::size_t i=0;i<candidates.size();++i)if(changed[i].identity!=candidates[i].candidate.collision.filter.identity)return false;
  for(std::size_t i=0;i<candidates.size();++i){auto&primary=candidates[i].candidate.collision;auto&movementView=movementCandidates[i].collision;primary.filter.flagsF8=changed[i].flagsF8;primary.copied40=changed[i].copied40;primary.copied50=changed[i].copied50;movementView.filter.flagsF8=changed[i].flagsF8;movementView.copied40=changed[i].copied40;movementView.copied50=changed[i].copied50;}
  return true;
 };
 x.beginFrame({128,128,128,128},.016f,60);f::require(s.bodies.vehicle838==1,"original lifecycle activates secondary body");
 x.hot([&]{return beginVehicleSubstep49W(s,x.bindings,x.selectors,x.workspace,x.diagnostic);},"native selector");
 MainStaticDetectorAdmission50H admission{true,true,true,false,false,false,true,true,2,0,x.bindings.lifecycle.main190};
 x.hot([&]{return queryMainStaticVehicle50H(s,x.bindings,admission,query,candidates.data(),candidates.size(),initial,workspace,destination,queryResult)==Type2StaticStatus49X::Complete;},"actual terrain query");
 x.hot([&]{return commitCandidateEffects(effects);},"commit main query candidate effects");qfilter.flagsF8=queryResult.queryFlagsF8;query.initialQueryCurrentIdentity=queryResult.currentCandidateIdentity;initial.selected=queryResult.selected;
 f::require(queryResult.hasGeometryHit&&queryResult.records.generatedCount84>0,"actual contact generated");spring.records=queryResult.records;std::memcpy(records,queryRecords,sizeof records);

 MainStaticDetectorAdmission50H secondaryAdmission=admission;secondaryAdmission.queryScene=x.bindings.lifecycle.physical19C;
 x.hot([&]{return prepareSecondaryQueryScene50J(s,x.bindings,secondaryAdmission,x.workspace)==SecondaryQuerySceneStatus50J::Ready;},"early original secondary scene getter");
 x.hot([&]{return querySecondaryStaticVehicle50J(s,x.bindings,secondaryAdmission,secondaryQuery,candidates.data(),candidates.size(),si,sw,sd,secondaryResult)==Type2StaticStatus49X::Complete;},"actual secondary terrain query");
 x.hot([&]{return commitCandidateEffects(sde);},"commit secondary query candidate effects");secondaryFilter.flagsF8=secondaryResult.queryFlagsF8;secondaryQuery.initialQueryCurrentIdentity=secondaryResult.currentCandidateIdentity;si.selected=secondaryResult.selected;
 secondarySpring.records=secondaryResult.records;std::memcpy(secondaryRecords,secondaryQueryRecords,sizeof secondaryRecords);
 f::require(secondaryResult.hasGeometryHit&&secondaryResult.records.generatedCount84>0,"actual secondary contacts generated");
 const auto queryStage=s;
 PostWorld49W world{};world.point=WorldCut49W::First358Secondary36CAnd219420;world.effects=contactEffects49W(s);world.applicationGateByte58=2;
 x.hot([&]{return produceVehicleForces49W(s,x.bindings,x.parameters,world,x.workspace,x.diagnostic);},"native main forces");
 const auto beforeResponse=s;ResponseStorage50K responseStorage;auto responseWorkspace=responseStorage.view(x.workspace);
 x.hot([&]{return respondMainStaticVehicle50G(s,x.bindings,x.parameters,settings,response,x.secondaryInputs,query.queryScene,1,spring,records,40,responseWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual main contact response");
 f::require(spring.records.contact3C&&s.phase==VehicleFramePhase49W::Post368,"positive persistent contact");
 f::require(std::memcmp(&beforeResponse.bodies.main.components.root,&s.bodies.main.components.root,sizeof s.bodies.main.components.root)!=0,"response changes canonical body");

 ResponseStorage50K secondaryStorage;auto secondaryWorkspace=secondaryStorage.view(x.workspace);
 // Independent body execution after the separately verified response leaf.
 auto originalSecondaryInput=s;SpringModelBody48K originalBody{};originalBody.identity=2;originalBody.body.state=s.bodies.secondary.root;originalBody.body.parameters=x.parameters.bodies.secondary.body;originalBody.body.localCenter=x.parameters.bodies.secondary.centerOffset;
 auto originalSettings=secondarySettings;originalSettings.impact.worldCenter110=secondarySpring.worldCenter110;originalSettings.impact.contactVelocity120=secondarySpring.contactVelocity120;
 StaticResponseScene50E originalScene{{78},originalBody.body.state.scene};StaticResponseResult50E originalResponse{};SpringRecord48N originalRecords[60]{};ResponseStorage50K originalStorage;auto ow=originalStorage.view(x.workspace);
 f::require(respondStaticGenerated50E(originalSettings,secondaryResponse,secondarySpring.records,originalBody,originalScene,secondaryRecords,60,ow.response,originalRecords,60,originalResponse)==StaticResponseStatus50E::Complete,"independent secondary response preparation");
 originalSecondaryInput.bodies.secondary.root=originalResponse.primary.body.state;
 const auto originalSecondary=x.sourceBody(originalSecondaryInput,true);
 x.hot([&]{return respondSecondaryStaticVehicle50J(s,x.bindings,x.parameters,secondarySettings,secondaryResponse,{78},2,secondarySpring,secondaryRecords,60,secondaryWorkspace,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"actual secondary contact and integration");
 x.compareBody(originalSecondary,true);f::require(secondarySpring.records.contact3C&&s.phase==VehicleFramePhase49W::MainIntegration,"positive secondary contact");
 const auto beforeIntegration=s;const auto original=impact_join49y::original(x,s,-7);
 x.hot([&]{return integrateVehicleMain49W(s,x.bindings,x.parameters,x.workspace,x.diagnostic);},"contact-driven body integration");x.compareBody(original.state,false);
 f::words(s.fields.impactDirectionalD08,original.maximaAfterLeaf.data(),32,"original impact maxima");f::words(&s.fields.impactBA0,original.ba0AfterLeaf.data(),16,"original impact BA0");
 f::require(s.phase==VehicleFramePhase49W::Query219120,"real late-query boundary");
 std::memcpy(&endpoints[0],s.bodies.main.priorPoint7B0,16);std::memcpy(&endpoints[1],s.bodies.main.currentPoint7C0,16);
 x.hot([&]{const auto outcome=queryMovementStaticCandidates50L({true,true},movement,movementSource,movementCandidates.data(),movementCandidates.size(),movementWorkspace,movementOutput,movementResult,movementFlag);movementFlag=outcome.vehicle7A0;return outcome.status==Type3QueryStatus48P::Complete;},"actual movement segment against terrain");
 x.hot([&]{return commitCandidateEffects(movementEffects);},"commit movement query candidate effects");movement.filter.flagsF8=movementResult.queryFlags;movement.initial=movementResult.state;movement.initialQueryCurrent=movementResult.currentCandidate;movementInitial=movementDestination;
 MovementQueryCompletion50M completion{};completion.resultFlagsF8=movementResult.queryFlags;std::memcpy(&completion.selectedPoint,&movementResult.state.selectedPoint,16);completion.pushDistance434710=10;completion.representedStatePreserved=completion.omittedBB0WNotConsumed=true;
 x.hot([&]{return finishVehicleMovementQuery50M(s,x.bindings,x.parameters,completion,x.workspace,x.diagnostic);},"native actual movement result and frame tail");
 f::require(s.phase==VehicleFramePhase49W::Ready,"one-substep diagnostic frame complete");

 const auto actual=s;const auto actualSpring=spring;const auto actualSecondarySpring=secondarySpring;
 std::cout<<J{{"main_contacts",actualSpring.records.generatedCount84},{"secondary_contacts",actualSecondarySpring.records.generatedCount84},{"movement_hit",movementResult.hasAnyHit},{"vehicle7a0",movementFlag},{"checks",f::checks},{"hot_allocations",0},{"phase",static_cast<unsigned>(s.phase)}}.dump()<<'\n';
 return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
