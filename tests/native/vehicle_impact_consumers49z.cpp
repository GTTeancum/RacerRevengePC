#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
#include <array>
#include "../../evidence/frameImpact49Y/consumer_dispatch49z.h"
#include <cstddef>
#include <limits>
using namespace frame49w_test;
namespace {
using Fields=VehicleFrameFields49W;using Supplement=VehicleImpactSupplement49Z;
constexpr std::size_t F=offsetof(State,fields),Z=offsetof(State,impactSupplement);
// Order comes from original source offsets D08..D24,820..830,7D0..7DC,
// 7E0,7E8,CFC,D00,D04, partner{00,04,08,0C,10},D38,D3C.
constexpr std::size_t offsets[]={
 F+offsetof(Fields,impactDirectionalD08)+0,F+offsetof(Fields,impactDirectionalD08)+4,F+offsetof(Fields,impactDirectionalD08)+8,F+offsetof(Fields,impactDirectionalD08)+12,
 F+offsetof(Fields,impactDirectionalD08)+16,F+offsetof(Fields,impactDirectionalD08)+20,F+offsetof(Fields,impactDirectionalD08)+24,F+offsetof(Fields,impactDirectionalD08)+28,
 Z+offsetof(Supplement,timers820)+0,Z+offsetof(Supplement,timers820)+4,Z+offsetof(Supplement,timers820)+8,Z+offsetof(Supplement,timers820)+12,Z+offsetof(Supplement,timers820)+16,
 F+offsetof(Fields,damage7D0),F+offsetof(Fields,damage7D4),F+offsetof(Fields,damage7D8),F+offsetof(Fields,damage7DC),
 Z+offsetof(Supplement,cooldown7E0),Z+offsetof(Supplement,periodic7E8),F+offsetof(Fields,vehicleCFC),F+offsetof(Fields,vehicleD00),Z+offsetof(Supplement,fieldD04),
 Z+offsetof(Supplement,partnerId),Z+offsetof(Supplement,partnerState),Z+offsetof(Supplement,partnerTimer08),Z+offsetof(Supplement,partnerAmount0C),F+offsetof(Fields,objectD34_10),Z+offsetof(Supplement,environmentalD38),Z+offsetof(Supplement,environmentalD3C)};
static_assert(std::size(offsets)==29);
void install(State&s,const U*words){for(unsigned i=0;i<29;++i)std::memcpy(reinterpret_cast<char*>(&s)+offsets[i],words+i,4);}
void projection(const State&s,const U*expected){for(unsigned i=0;i<29;++i){U got;std::memcpy(&got,reinterpret_cast<const char*>(&s)+offsets[i],4);if(got!=expected[i]){std::fprintf(stderr,"projection word %u native=%08x source=%08x\n",i,got,expected[i]);throw std::runtime_error("canonical consumer word differs");}}}
unsigned rows=0,applied=0,skipped=0,rejects=0,eventPairs=0,phaseChecks=0,managerSkips=0,leafSkips=0;
struct EventBox {U before;VehicleImpactEvents49Z events;U after;};
void readGraphics(Fixture&f){
 FrameReader49K read{};read.packet={128,128,128,128};read.preparedFactor=.016f;read.actions={nullptr,noAction};read.codes=f.actionCodes;
 f.hot([&]{return readVehicleFrame49W(f.state,f.bindings,read,f.workspace,f.diagnostic);},"consumer frame reader");
 PostPlayerAt217DE0_49K post{};post.player=f.state.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=true;
 f.hot([&]{return bindVehicleFrame49W(f.state,f.bindings,f.parameters,cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,f.workspace,f.diagnostic);},"consumer frame binding");
 f.hot([&]{return graphicsVehicleFrame49W(f.state,f.bindings,f.parameters,cut(PreservingCut49W::Base2176E0BeforeGraphics),f.workspace,f.diagnostic);},"consumer frame graphics");
}
void noBypass(Fixture&f){const auto before=f.state;auto n=allocations;require(!refreshMainVehicleFrame49W(f.state,f.bindings,cut(PreservingCut49W::BaseAfterGraphicsThroughBA0ResetBeforeChildren),f.workspace,f.diagnostic),"consumer aftermath bypassed");require(allocations==n,"no-bypass allocation");words(&f.state,&before,sizeof before,"no-bypass state atomicity");++phaseChecks;}
void fixtures(const char*path,Fixture&f,const std::array<State,4>&entry,const std::array<VehicleFrameBindings49W,4>&bindings){
 std::ifstream file(path,std::ios::binary);U count,width;file.read(reinterpret_cast<char*>(&count),4);file.read(reinterpret_cast<char*>(&width),4);require(bool(file)&&width==86,"consumer source fixture header");
 for(unsigned row=0;row<count;++row){std::array<U,86> r{};file.read(reinterpret_cast<char*>(r.data()),sizeof r);require(bool(file),"consumer source fixture row");
  // The frame admits ordinary3C/40/58=0 with actual left/right components.
  if(r[2]||r[3]||r[4]||(r[7]&3)!=3)continue;
  const unsigned mask=(r[7]>>2)&3;f.state=entry[mask];f.bindings=bindings[mask];install(f.state,r.data()+14);projection(f.state,r.data()+14);
  std::memcpy(&f.state.bodies.main.frameFactor,r.data()+11,4);f.state.fields.impactFlagsCD8=std::uint8_t(r[1]);f.state.impactConsumerPending49Y=1;
  ImpactConsumerInput49Z input;std::memcpy(&input,r.data(),sizeof input);
  // These duplicate members must never replace their canonical state/bindings.
  const U poison=0x7f800001;std::memcpy(&input.factor,&poison,4);input.flagsCD8=r[1]^1u;input.vehicle3C=0x12345678;input.vehicle40=3;input.vehicle58=1;input.componentPresence=(~r[7])&15u;
  std::array<U,14> inputBefore;std::memcpy(inputBefore.data(),&input,sizeof input);State before;std::memcpy(&before,&f.state,sizeof before);State expected;std::memcpy(&expected,&before,sizeof expected);
  EventBox box;std::memset(&box,0xa5,sizeof box);const auto boxBefore=box;
  const auto n=allocations;const bool ok=consumeVehicleImpacts49Z(f.state,f.bindings,input,box.events,f.workspace,f.diagnostic);require(n==allocations,"consumer frame allocation");words(&input,&inputBefore,sizeof input,"consumer input immutable");require(box.before==boxBefore.before&&box.after==boxBefore.after,"consumer bounded event storage");
  if(r[43]==2){require(!ok&&f.diagnostic.error==VehicleFrameError49W::Admission&&f.diagnostic.sourceStage==0x222cec,"source dynamic partner rejection");words(&f.state,&before,sizeof before,"consumer whole-frame rejected atomicity");words(&box,&boxBefore,sizeof box,"consumer rejected events atomicity");++rejects;}
  else {require(ok,"source consumer success rejected");install(expected,r.data()+44);const bool managerRuns=(r[0]&2)!=0;const U exitPC=managerRuns?consumer_dispatch49z::leafSkipExit:consumer_dispatch49z::managerSkipExit;require(exitPC==(managerRuns?0x2288bc:0x228b40),"authenticated dispatch exit PC");expected.phase=managerRuns?VehicleFramePhase49W::PostImpactConsumers:VehicleFramePhase49W::AfterImpactBlock;expected.impactConsumerPending49Y=0;expected.impactConsumersActive49Z=1;projection(f.state,r.data()+44);words(&f.state,&expected,sizeof expected,"consumer full frame sole-owner projection");require(box.events.count==r[73]&&box.events.count<=2,"source controller event count");words(box.events.values,r.data()+74,box.events.count*sizeof(ImpactConsumerEvent49Z),"source controller event words and order");if(box.events.count)++eventPairs;if(r[43]==1)++skipped;else ++applied;
   if(!managerRuns){++managerSkips;const auto beforeRepair=f.state;const auto n=allocations;require(!repairVehicleImpacts50A(f.state,f.bindings,f.workspace,f.diagnostic)&&f.diagnostic.error==VehicleFrameError49W::Phase,"repair executed after manager block skip");require(n==allocations,"skip repair reject allocation");words(&f.state,&beforeRepair,sizeof beforeRepair,"AfterImpactBlock repair atomicity");++phaseChecks;}else if(r[43]==1)++leafSkips;
   noBypass(f);
   // Even an incorrectly retagged caller cannot use the legacy preserving cut
   // after an actual consumer has established active ownership.
   {const auto phase=f.state.phase;f.state.phase=VehicleFramePhase49W::PostGraphics;noBypass(f);require(f.diagnostic.sourceStage==0x2288b4,"active consumer legacy guard");f.state.phase=phase;}
   const auto after=f.state;const auto eventsAfter=box;require(!consumeVehicleImpacts49Z(f.state,f.bindings,input,box.events,f.workspace,f.diagnostic)&&f.diagnostic.error==VehicleFrameError49W::Phase,"consumer replay phase rejected");words(&f.state,&after,sizeof after,"consumer replay state atomicity");words(&box,&eventsAfter,sizeof box,"consumer replay events atomicity");++phaseChecks;
  }
  ++rows;
 }
}
}
int main(int argc,char**argv){try{
 static_assert(sizeof(ImpactConsumerInput49Z)==56&&sizeof(ImpactConsumerEvent49Z)==24);require(argc<=2,"usage: vehicle_impact_consumers49z [original-fixtures]");std::fesetround(FE_TONEAREST);Fixture f(80);const auto initial=f.state;const auto originalBindings=f.bindings;
 std::array<State,4> entry;std::array<VehicleFrameBindings49W,4> bindings;
 readGraphics(f);const auto postGraphics=f.state;
 // Current upstream graphics admits mask3 only. Other masks are source-owned
 // postgraphics boundary fixtures for this consumer API, not upstream coverage.
 for(unsigned i=0;i<4;++i){entry[i]=postGraphics;bindings[i]=originalBindings;bindings[i].visual.extra1C0Present=(i&1)!=0;bindings[i].visual.extra1C4Present=(i&2)!=0;}
 fixtures(argc==2?argv[1]:"work/local49Z/impact-consumers/fixtures.bin",f,entry,bindings);
 require(rows==844&&applied==793&&skipped==30&&rejects==21&&eventPairs==378,"admitted original fixture coverage");
 fixtures("evidence/frameImpact49Y/consumer-copy-only.bin",f,entry,bindings);require(rows==868&&skipped==54&&managerSkips==28&&leafSkips==26,"raw original distinct skip exit coverage");
 // A consumed invalid timer can fail after earlier projected fields were updated
 // inside the leaf. Neither the frame nor controller output may commit.
 f.state=entry[3];f.bindings=bindings[3];f.state.impactSupplement.partnerId=0xffffffffu;f.state.impactSupplement.timers820[0]=1;const U snan=0x7f800001;std::memcpy(&f.state.impactSupplement.timers820[4],&snan,4);ImpactConsumerInput49Z in{};in.managerFlags58=2;in.profile34=2;in.finalDamageThreshold64=1;
 {const auto before=f.state;EventBox events;std::memset(&events,0x59,sizeof events);const auto eb=events;const auto n=allocations;require(!consumeVehicleImpacts49Z(f.state,f.bindings,in,events.events,f.workspace,f.diagnostic)&&f.diagnostic.sourceStage==0x222b20,"late consumer arithmetic rejection");require(n==allocations,"late reject allocation");words(&f.state,&before,sizeof before,"late consumer atomic state");words(&events,&eb,sizeof eb,"late consumer atomic events");++rejects;}
 std::printf("PASS vehicle_impact_consumers49z sourceRows=%u applied=%u skipped=%u rejects=%u eventPairs=%u phaseChecks=%u managerExit228B40=%u leafSkipExit2288BC=%u\n",rows,applied,skipped,rejects,eventPairs,phaseChecks,managerSkips,leafSkips);return 0;
 }catch(const std::exception&e){std::fprintf(stderr,"FAIL vehicle_impact_consumers49z row=%u: %s\n",rows,e.what());return 1;}}

