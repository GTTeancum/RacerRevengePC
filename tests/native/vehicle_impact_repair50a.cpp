#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
#include <array>
#include <cstddef>
using namespace frame49w_test;
namespace {
using Fields=VehicleFrameFields49W;using Supplement=VehicleImpactSupplement49Z;
constexpr std::size_t F=offsetof(State,fields),Z=offsetof(State,impactSupplement);
constexpr std::size_t offsets[]={F+offsetof(Fields,damage7D0),F+offsetof(Fields,damage7D4),F+offsetof(Fields,damage7D8),F+offsetof(Fields,damage7DC),Z+offsetof(Supplement,repairTimer7E4),Z+offsetof(Supplement,environmentalD38),Z+offsetof(Supplement,environmentalD3C)};
void install(State&s,const U*raw){for(unsigned i=0;i<7;++i)std::memcpy(reinterpret_cast<char*>(&s)+offsets[i],raw+i,4);}
void compare(const State&s,const U*raw){for(unsigned i=0;i<7;++i){U word;std::memcpy(&word,reinterpret_cast<const char*>(&s)+offsets[i],4);if(word!=raw[i]){std::fprintf(stderr,"repair word%u native=%08x source=%08x\n",i,word,raw[i]);throw std::runtime_error("repair source projection");}}}
unsigned rows=0,applied=0,skipped=0,crossPairs=0,rejected=0,phaseChecks=0;
void noBypass(Fixture&f){State before;std::memcpy(&before,&f.state,sizeof before);const auto n=allocations;require(!refreshMainVehicleFrame49W(f.state,f.bindings,cut(PreservingCut49W::BaseAfterGraphicsThroughBA0ResetBeforeChildren),f.workspace,f.diagnostic),"repair aftermath bypassed");require(n==allocations,"repair bypass allocation");words(&f.state,&before,sizeof before,"repair bypass atomicity");++phaseChecks;}
void fixtures(const char*path,Fixture&f,const State&entry,const VehicleFrameBindings49W&binding){
 std::ifstream file(path,std::ios::binary);U count,width;file.read(reinterpret_cast<char*>(&count),4);file.read(reinterpret_cast<char*>(&width),4);require(bool(file)&&width==19,"repair fixture header");
 for(unsigned row=0;row<count;++row){std::array<U,19> r{};file.read(reinterpret_cast<char*>(r.data()),sizeof r);require(bool(file),"repair fixture row");if((r[1]&3)!=3)continue;
  f.state=entry;f.bindings=binding;f.bindings.visual.extra1C0Present=(r[1]&4)!=0;f.bindings.visual.extra1C4Present=(r[1]&8)!=0;install(f.state,r.data()+4);compare(f.state,r.data()+4);
  f.state.bodies.main.controls.player.flagsB9C=std::uint8_t(r[0]);std::memcpy(&f.state.bodies.main.frameFactor,r.data()+2,4);std::memcpy(&f.state.fields.repairCF4,r.data()+3,4);
  State expected;std::memcpy(&expected,&f.state,sizeof expected);install(expected,r.data()+12);expected.phase=VehicleFramePhase49W::PostImpactRepair;
  const auto n=allocations;require(repairVehicleImpacts50A(f.state,f.bindings,f.workspace,f.diagnostic),"source repair rejected");require(n==allocations,"repair frame allocation");compare(f.state,r.data()+12);words(&f.state,&expected,sizeof expected,"repair sole-owner full-state projection");
  if(r[11])++skipped;else ++applied;
  if((r[0]&2)&&r[8]==0&&r[6]!=r[5]&&r[14]==r[13])++crossPairs;
  noBypass(f);auto phase=f.state.phase;f.state.phase=VehicleFramePhase49W::PostGraphics;noBypass(f);require(f.diagnostic.sourceStage==0x2288b4,"repair active legacy guard");f.state.phase=phase;
  State after;std::memcpy(&after,&f.state,sizeof after);require(!repairVehicleImpacts50A(f.state,f.bindings,f.workspace,f.diagnostic)&&f.diagnostic.error==VehicleFrameError49W::Phase,"repair replay phase rejected");words(&f.state,&after,sizeof after,"repair replay atomicity");++phaseChecks;++rows;
 }
}
}
int main(int argc,char**argv){try{
 require(argc<=2,"usage: vehicle_impact_repair50a [source-fixtures]");std::fesetround(FE_TONEAREST);Fixture f(80);
 FrameReader49K read{};read.packet={128,128,128,128};read.preparedFactor=.016f;read.actions={nullptr,noAction};read.codes=f.actionCodes;
 f.hot([&]{return readVehicleFrame49W(f.state,f.bindings,read,f.workspace,f.diagnostic);},"repair frame reader");PostPlayerAt217DE0_49K post{};post.player=f.state.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=true;
 f.hot([&]{return bindVehicleFrame49W(f.state,f.bindings,f.parameters,cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,f.workspace,f.diagnostic);},"repair frame binding");
 f.hot([&]{return graphicsVehicleFrame49W(f.state,f.bindings,f.parameters,cut(PreservingCut49W::Base2176E0BeforeGraphics),f.workspace,f.diagnostic);},"repair frame graphics");
 // The real ordinary mask3 prefix and actual enabled manager consumer reach repair.
 // Additional component masks below are isolated authenticated boundary cases;
 // upstream49R currently rejects optional extra models and is not claimed here.
 {const auto before=f.state;require(!repairVehicleImpacts50A(f.state,f.bindings,f.workspace,f.diagnostic)&&f.diagnostic.error==VehicleFrameError49W::Phase,"repair before consumer accepted");words(&f.state,&before,sizeof before,"early repair phase atomicity");++phaseChecks;}
 ImpactConsumerInput49Z consumer{};consumer.managerFlags58=2;consumer.profile34=2;consumer.finalDamageThreshold64=1;f.state.impactSupplement.partnerId=0xffffffffu;VehicleImpactEvents49Z events{};f.hot([&]{return consumeVehicleImpacts49Z(f.state,f.bindings,consumer,events,f.workspace,f.diagnostic);},"repair actual enabled manager consumer");require(events.count==0,"zero-impact consumer emitted controller event");const auto entry=f.state;const auto binding=f.bindings;
 fixtures(argc==2?argv[1]:"work/local50A/impact-aftermath/repair-fixtures.bin",f,entry,binding);require(rows==2569&&applied==2536&&skipped==33&&crossPairs==655,"admitted original repair coverage");
 fixtures("evidence/frameImpact49Y/repair-copy-only.bin",f,entry,binding);require(rows==2593&&applied==2548&&skipped==45,"raw source repair copy coverage");
 // Late failure: first pair can already repair in scratch, then consuming the
 // second pair's signaling NaN must roll back every frame byte.
 for(unsigned i=0;i<3;++i){f.state=entry;f.bindings=binding;f.bindings.visual.extra1C0Present=true;f.bindings.visual.extra1C4Present=true;f.state.bodies.main.controls.player.flagsB9C=2;f.state.fields.repairCF4=.125f;f.state.fields.damage7D0=.4f;f.state.fields.damage7D4=.4f;f.state.fields.damage7D8=.6f;f.state.fields.damage7DC=.5f;f.state.impactSupplement.repairTimer7E4=0;
  if(i==0){const U snan=0x7f800001;std::memcpy(&f.state.fields.damage7DC,&snan,4);}if(i==1){const U x=0x00800001,y=0x00800000;std::memcpy(&f.state.impactSupplement.repairTimer7E4,&x,4);std::memcpy(&f.state.bodies.main.frameFactor,&y,4);}if(i==2){const U a=0x7f7fffff,b=0xff7fffff;std::memcpy(&f.state.fields.damage7D0,&a,4);std::memcpy(&f.state.fields.damage7D4,&b,4);}
  State before;std::memcpy(&before,&f.state,sizeof before);const auto n=allocations;require(!repairVehicleImpacts50A(f.state,f.bindings,f.workspace,f.diagnostic)&&f.diagnostic.sourceStage==0x224370,"repair arithmetic accepted");require(n==allocations,"repair reject allocation");words(&f.state,&before,sizeof before,"repair complete atomic rollback");++rejected;
 }
 std::printf("PASS vehicle_impact_repair50a rows=%u applied=%u skipped=%u crossPairs=%u rejects=%u phaseChecks=%u\n",rows,applied,skipped,crossPairs,rejected,phaseChecks);return 0;
 }catch(const std::exception&e){std::fprintf(stderr,"FAIL vehicle_impact_repair50a row=%u: %s\n",rows,e.what());return 1;}}
