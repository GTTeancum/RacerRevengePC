#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
#include "vehicle_secondary_contact50j.h"
#include "../../src/assets/spring_authoring50f.h"
namespace f=frame49w_test;
using namespace rr::game;
struct Storage50J {
 SpringGeneratedRecord48K projected[60]{};SpringRecord48N preparedRecords[60]{};
 std::uint8_t flags[60]{},preparedFlags[60]{};SpringModelBody48K scratch{},prepared{};
 StaticResponseWorkspace50E response(){return {projected,60,{&scratch,1,flags,60},{&prepared,1,preparedFlags,60}};}
 VehicleStaticResponseWorkspace50G workspace(VehicleFrameWorkspace49W&w){return {response(),preparedRecords,60,&w};}
};
int main(){try{
 std::fesetround(FE_TONEAREST);f::Fixture x(4);x.beginFrame({128,128,128,128},.016f,60);
 f::require(beginVehicleSubstep49W(x.state,x.bindings,x.selectors,x.workspace,x.diagnostic),"begin native substep");
 PostWorld49W world{};world.point=x.state.bodies.vehicle838?WorldCut49W::First358Secondary36CAnd219420:WorldCut49W::First358And219420;
 world.effects=contactEffects49W(x.state);world.applicationGateByte58=2;
 f::require(produceVehicleForces49W(x.state,x.bindings,x.parameters,world,x.workspace,x.diagnostic),"current native forces");
 Post354_49W noMain{};noMain.effects=contactEffects49W(x.state);noMain.vehicle838=1;
 f::require(respondVehicle354_49W(x.state,x.bindings,x.parameters,noMain,x.secondaryInputs,x.workspace,x.diagnostic),"secondary force prefix");
 const auto base=x.state;unsigned cases=0,rejections=0;
 for(unsigned n=0;n<8;++n){
  x.state=base;x.state.bodies.vehicle838=1;x.state.bodies.secondary.root.body.velocity.v[1]=-5;
  SpringGeneratedParameters48K settings{};settings.impact54=true;settings.impact.disableLeverArms5C=false;settings.impact.correctPenetration64=true;settings.impact.normalForceScale98=1.f;settings.impact.positionCorrectionScaleCC=.025f;
  SpringResponseParameters48L response{};
  StaticSpringState50G spring{};spring.records.pending38=1;spring.records.generatedCount84=1;spring.records.generatedCapacity88=60;
  spring.worldCenter110={{8,9,10}};spring.contactVelocity120={{11,12,13}};
  SpringRecord48N records[60];std::memset(records,0xa5,sizeof records);auto&record=records[0];
  record.localPoint10={{0,0,0,7}};record.normal20={{0,1,0,8}};
  const auto&matrix=x.state.bodies.secondary.root.scene;
  record.surfacePoint30={{matrix.v[12],matrix.v[13]+(n<4?1.f:-1.f),matrix.v[14],9}};
  record.scene40.value=n&2?0:77;record.other44.value=0;record.material4C=0;
  const auto before=x.state;auto expected=before;auto expectedSpring=spring;SpringRecord48N expectedRecords[60];std::memcpy(expectedRecords,records,sizeof records);
  Storage50J refStorage;SpringModelBody48K body{};body.identity=1;body.body.state=expected.bodies.secondary.root;body.body.parameters=x.parameters.bodies.secondary.body;body.body.localCenter=x.parameters.bodies.secondary.centerOffset;
  auto effective=settings;effective.impact.worldCenter110=spring.worldCenter110;effective.impact.contactVelocity120=spring.contactVelocity120;
  StaticResponseScene50E scene{{77},body.body.state.scene};StaticResponseResult50E result{};auto referenceWorkspace=refStorage.response();
  f::require(respondStaticGenerated50E(effective,response,spring.records,body,scene,records,60,referenceWorkspace,expectedRecords,60,result)==StaticResponseStatus50E::Complete,"reference source-verified response");
  expectedSpring.records=result.records;expectedSpring.worldCenter110=result.worldCenter110;expectedSpring.contactVelocity120=result.contactVelocity120;
  Post354_49W post{};post.effects=contactEffects49W(expected);auto&m=post.effects.secondary;const auto&r=result.primary.body.state;
  m.velocity70=r.body.velocity;m.angularVelocityB0=r.body.angularVelocity;m.loads=r.loads;m.positionCorrection1A0=r.positionCorrection1a0;m.rotationCorrection1B0=r.rotationCorrection1b0;m.accumulator1C0=r.accumulator1c0;m.accumulator1D0=r.accumulator1d0;
  post.response3C=result.records.contact3C;post.response44=result.records.other44;post.vehicle838=expected.bodies.vehicle838;
  VehicleFrameWorkspace49W referenceFrame{};VehicleFrameDiagnostic49W referenceDiagnostic{};
  f::require(respondVehicle368_49W(expected,x.bindings,x.parameters,post.effects,referenceFrame,referenceDiagnostic),"reference native stage");
  Storage50J storage;auto w=storage.workspace(x.workspace);
  x.hot([&]{return respondSecondaryStaticVehicle50J(x.state,x.bindings,x.parameters,settings,response,{77},1,spring,records,60,w,x.diagnostic)==VehicleStaticResponseStatus50G::Complete;},"connected main response");
  f::words(&x.state,&expected,sizeof expected,"canonical frame exact composition");f::words(&spring,&expectedSpring,sizeof spring,"persistent spring exact");f::words(records,expectedRecords,sizeof records,"persistent record bytes exact");
  f::require(x.state.phase==VehicleFramePhase49W::MainIntegration,"native response stage order");++cases;
  // The response succeeds, but a later frame failure must roll back BOTH owners.
  for(unsigned bad=0;bad<5;++bad){x.state=before;x.state.bodies.vehicle838=1;auto broken=x.parameters;auto localSettings=settings;auto localW=w;auto identity=1u;
   if(bad==0)broken.bodies.secondary.body.linearAccelerationLimitSquared=std::numeric_limits<float>::quiet_NaN();
   if(bad==1)x.state.phase=VehicleFramePhase49W::Ready;
   if(bad==2)localW.preparedRecords=records;
   if(bad==3)localSettings.pointContacts48=true;
   if(bad==4)identity=0;
   records[0].flags50=0xa1;spring.records.pending38=1;
   spring.worldCenter110={{99,98,97}};spring.contactVelocity120={{-99,-98,-97}};
   const auto saved=x.state;const auto savedSpring=spring;SpringRecord48N savedRecords[60];std::memcpy(savedRecords,records,sizeof records);
   const auto allocationBefore=::allocations;
   const auto status=respondSecondaryStaticVehicle50J(x.state,x.bindings,broken,localSettings,response,{77},identity,spring,records,60,localW,x.diagnostic);
   f::require(status!=VehicleStaticResponseStatus50G::Complete,"invalid stage rejected");if(bad==0)f::require(status==VehicleStaticResponseStatus50G::FrameRejected,"late native stage rejected");
   if(bad==0&&n<4)f::require((storage.preparedRecords[0].flags50&2u)&&!(records[0].flags50&2u),"reached response effects not committed after frame rejection");
   f::words(&x.state,&saved,sizeof saved,"frame rollback");f::words(&spring,&savedSpring,sizeof spring,"spring rollback");f::words(records,savedRecords,sizeof records,"records rollback");f::require(::allocations==allocationBefore,"rejection no allocations");++rejections;
  }
 }
 std::cout<<"{\"cases\":"<<cases<<",\"rejections\":"<<rejections<<",\"checks\":"<<f::checks<<",\"hot_allocations\":"<<0<<"}\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
