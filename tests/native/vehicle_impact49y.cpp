#include "../../evidence/frameImpact49Y/impact_join49y_fixture.h"
#include <limits>
#include <set>
using namespace impact_join49y;
static unsigned joinedCases=0, rejected=0, matrixControls=0, sourceCalls=0;
static std::set<U> covered;
static std::ofstream proof;
static void tuning(Fixture&f){f.parameters.impactTuning={{1050,787.5f,440,175,50},{14,12,8,5,.5f}};f.parameters.impactTuningBound=true;f.state.fields.defenseCF0=.8f;}
static Result joined(Fixture&f,bool callExpected){
 const auto before=f.state;const auto source=original(f,before,-7);const auto alternate=original(f,before,31);
 require(source.calls.count(0x223030)==unsigned(callExpected),"source conditional impact dispatch");words(source.maximaAfterLeaf.data(),alternate.maximaAfterLeaf.data(),32,"scratch W maxima independence");words(source.ba0AfterLeaf.data(),alternate.ba0AfterLeaf.data(),16,"scratch W BA0 independence");words(&source.state.bodies,&alternate.state.bodies,sizeof source.state.bodies,"scratch W body independence");words(source.state.nodes,alternate.state.nodes,sizeof source.state.nodes,"scratch W graph independence");
 words(source.impulseAfterLeaf.data(),&before.bodies.main.components.root.accumulator1c0,12,"source impact preserves accumulator XYZ");words(source.impulseAfterLeaf.data()+3,&before.mainExtra.accumulator1C0W,4,"source impact preserves accumulator W");
 if(callExpected){auto a=std::find(source.trace.begin(),source.trace.end(),0x223030),b=std::find(source.trace.begin(),source.trace.end(),0x167c40),c=std::find(source.trace.begin(),source.trace.end(),0x165420);require(a<b&&b<c,"impact/main/post source order");++sourceCalls;}
 const bool applied=callExpected&&!(before.fields.impactFlagsCD8&1);const U pending=before.impactConsumerPending49Y||applied;
 f.hot([&]{return integrateVehicleMain49W(f.state,f.bindings,f.parameters,f.workspace,f.diagnostic);},"joined impact/main");f.compareBody(source.state,false);words(f.state.fields.impactDirectionalD08,source.maximaAfterLeaf.data(),32,"joined source maxima");words(&f.state.fields.impactBA0,source.ba0AfterLeaf.data(),16,"joined source BA0");require(f.state.impactConsumerPending49Y==pending,"impact pending coverage marker");finish(f,before,source.state);
 for(U pc:source.trace)covered.insert(pc);
 if(proof){proof<<"scratch-inputs c0e00000 41f80000 outputs "<<std::hex<<source.actualVf9WAfterLeaf<<' '<<alternate.actualVf9WAfterLeaf<<std::dec<<"\n";proof<<"case "<<joinedCases<<" calls223030 "<<callExpected<<" flagsCD8 "<<unsigned(before.fields.impactFlagsCD8)<<"\n";auto dump=[&](const char*name,const void*data,unsigned count){proof<<name;for(unsigned i=0;i<count;++i){U word;std::memcpy(&word,(const char*)data+4*i,4);proof<<' '<<std::hex<<word;}proof<<std::dec<<'\n';};dump("canonicalSentinel",&before.visualShared.zero434630,4);dump("alternateMaxima",alternate.maximaAfterLeaf.data(),8);dump("alternateBA0",alternate.ba0AfterLeaf.data(),4);dump("input1C0",&before.bodies.main.components.root.accumulator1c0,3);dump("sourceMaxima",source.maximaAfterLeaf.data(),8);dump("sourceBA0",source.ba0AfterLeaf.data(),4);dump("sourcePreWorld",&before.nodes[50].world100,16);dump("sourcePostWorld",&source.state.nodes[50].world100,16);proof<<"trace";for(U pc:source.trace)proof<<' '<<std::hex<<pc;proof<<std::dec<<"\nstores\n";for(auto&v:source.stores)proof<<std::hex<<v.pc<<' '<<v.address<<' '<<v.before<<' '<<v.after<<'\n';proof<<std::dec;}
 ++joinedCases;return source;
}
static void rejectImpact(Fixture&f,U stage){const auto before=f.state;const auto n=allocations;require(!integrateVehicleMain49W(f.state,f.bindings,f.parameters,f.workspace,f.diagnostic),"unsupported impact accepted");require(n==allocations,"impact reject allocation");words(&before,&f.state,sizeof before,"whole frame transaction rollback");require(f.diagnostic.sourceStage==stage,"impact rejection source stage");++rejected;}
int main(int argc,char**argv){try{
 require(argc<=2,"usage: vehicle_impact49y [proof-log]");if(argc==2){proof.open(argv[1]);require(bool(proof),"proof log open");}
 std::fesetround(FE_TONEAREST);Fixture f(80);tuning(f);f.beginFrame({128,128,128,128},.05f,60);const auto start=f.state;const auto parameters=f.parameters;
 for(unsigned i=0;i<80;++i){f.state=start;f.parameters=parameters;tuning(f);std::array<float,3> impulse{};
  if(i>=2)impulse[(i-2)%3]=(i&1)?-800.f:1200.f;else if(i==1)impulse={-0.f,0.f,-0.f};
  if(i>=12){const unsigned j=(i-12)%15;float value=parameters.impactTuning.thresholds00[j/3];if(j%3==0)value=std::nextafter(value,0.f);if(j%3==2)value=std::nextafter(value,2000.f);impulse={value*((i&1)?-1.f:1.f),i%4==0?value*.3f:0.f,i%5==0?value*.9f:0.f};f.state.fields.defenseCF0=float(i%5)*.25f;for(unsigned k=0;k<8;++k)f.state.fields.impactDirectionalD08[k]=float((i+k)%4)*.1f;}
  if(i==9)impulse={1100,0,900};if(i==11)impulse={0,0,0};
  prefix(f,impulse,i%2==0);
  if(i==8){f.state.fields.impactFlagsCD8=1;f.state.bodies.main.components.body20C=7;f.parameters.impactTuningBound=false;f.state.fields.defenseCF0=std::numeric_limits<float>::quiet_NaN();for(float&x:f.parameters.impactTuning.thresholds00)x=std::numeric_limits<float>::quiet_NaN();}
  if(i==11)f.state.visualShared.zero434630.v[0]=1;
  if(i==2)f.state.visualShared.zero434630.v[1]=std::numeric_limits<float>::quiet_NaN();
  if(i<2){f.parameters.impactTuningBound=false;f.state.bodies.main.components.body20C=17;}
  if(i==9){f.state.nodes[50].localC0=c::v::matrix(.7f,900,800,700);project(f.state,f.bindings);auto wrong=f.state;wrong.nodes[50].world100=wrong.nodes[50].localC0;project(wrong,f.bindings);auto actual=original(f,f.state,0),bad=original(f,wrong,0);require(std::memcmp(actual.maximaAfterLeaf.data(),bad.maximaAfterLeaf.data(),32)!=0,"wrong pre-matrix negative control");++matrixControls;
   frame49w_order::TailInput tail{};std::copy_n(actual.state.fields.impactBA0.v,4,tail.impactBA0.begin());std::copy_n(actual.state.nodes[50].world100.v,16,tail.cachedWorld.begin());auto correctTail=frame49w_order::finishNoHit(tail);std::copy_n(f.state.nodes[50].world100.v,16,tail.cachedWorld.begin());auto staleTail=frame49w_order::finishNoHit(tail);require(std::memcmp(correctTail.bb0.data(),staleTail.bb0.data(),12)!=0,"stale late-matrix negative control");++matrixControls;}
  if(i==10){const U snan=0x7f812345;std::memcpy(&f.state.fields.impactBA0.v[3],&snan,4);}
  f.parameters.bodies.main.effectiveImpactZero434630={{impulse[0],impulse[1],impulse[2]}};joined(f,i>=2);
 }
 // Four consecutive original substeps share evolving maxima, BA0, matrices,
 // velocities and integration history. The last no-impact step must preserve them.
 f.state=start;f.parameters=parameters;tuning(f);unsigned step=0;do{prefix(f,step==0?std::array<float,3>{1100,200,-700}:(step==1?std::array<float,3>{-300,900,600}:std::array<float,3>{0,0,0}));joined(f,step<2);++step;}while(f.state.phase!=VehicleFramePhase49W::Ready);require(step==start.bodies.main.schedule.count&&step>=3,"persistent scheduled steps");
 // Advance the next frame only as far as the real missing consumer. The old
 // preserving cut must not discard pending impact maxima or reset BA0.
 FrameReader49K read{};read.packet={128,128,128,128};read.preparedFactor=.016f;read.actions={nullptr,noAction};read.codes=f.actionCodes;
 f.hot([&]{return readVehicleFrame49W(f.state,f.bindings,read,f.workspace,f.diagnostic);},"pending next reader");
 PostPlayerAt217DE0_49K post{};post.player=f.state.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=true;
 f.hot([&]{return bindVehicleFrame49W(f.state,f.bindings,f.parameters,cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,f.workspace,f.diagnostic);},"pending next binding");
 f.hot([&]{return graphicsVehicleFrame49W(f.state,f.bindings,f.parameters,cut(PreservingCut49W::Base2176E0BeforeGraphics),f.workspace,f.diagnostic);},"pending next graphics");
 {const auto before=f.state;const auto n=allocations;require(!refreshMainVehicleFrame49W(f.state,f.bindings,cut(PreservingCut49W::BaseAfterGraphicsThroughBA0ResetBeforeChildren),f.workspace,f.diagnostic),"unrecovered consumer silently preserved");require(allocations==n&&f.diagnostic.sourceStage==0x2288b4,"missing consumer boundary");words(&before,&f.state,sizeof before,"missing consumer atomicity");++rejected;}
 // No production mutation: unsupported inputs must preserve every canonical byte,
 // including already-computed impact changes when a later body operation fails.
 for(unsigned i=0;i<6;++i){f.state=start;f.parameters=parameters;tuning(f);prefix(f,{1200,0,0});U stage=0x223030;if(i==0)f.parameters.impactTuningBound=false;if(i==1)f.state.bodies.main.components.body20C=1;if(i==2)f.state.bodies.main.components.root.accumulator1c0.v[0]=std::numeric_limits<float>::max();if(i==3)f.state.bodies.main.components.root.accumulator1c0.v[0]=std::numeric_limits<float>::denorm_min();if(i==4)f.state.fields.defenseCF0=std::numeric_limits<float>::quiet_NaN();if(i==5){f.parameters.bodies.main.body.inverseMass=std::numeric_limits<float>::quiet_NaN();stage=0x219e80;}rejectImpact(f,stage);}
 std::printf("PASS vehicle_impact49y cases=%u rejects=%u sourceCalls=%u coveredWords=%zu matrixControls=%u nativeCalls=%llu\n",joinedCases,rejected,sourceCalls,covered.size(),matrixControls,(unsigned long long)nativeCalls);return 0;
 }catch(const std::exception&e){std::fprintf(stderr,"FAIL vehicle_impact49y case=%u rejects=%u: %s\n",joinedCases,rejected,e.what());return 1;}}


