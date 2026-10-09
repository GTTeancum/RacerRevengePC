#include <string>
#include "../../evidence/chariotAnimation49T/flow/flow_fixture.h"
#include "../../third_party/native-json/json.hpp"
#include <cfenv>
#include <cstdlib>
#include <iostream>
#include <new>
#include <limits>
using namespace vehicle_animation49t_test;
using J=nlohmann::json;
static std::uint64_t allocations=0,nativeCalls=0,rejections=0;
[[gnu::noinline]] void* operator new(std::size_t n){void* p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();++allocations;return p;}
[[gnu::noinline]] void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void operator delete(void* p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void* p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete(void* p,std::size_t)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
#ifndef RR_VEHICLE_ANIMATION49T_TRACE_ACTIVE
#define RR_VEHICLE_ANIMATION49T_TRACE_ACTIVE 0
#endif
static constexpr U stages[]={0x21a190,0x21a1a8,0x21a298,0x21a318,0x21a618,0x21a620,0x21a654};
static U mutationPc=0,mutationWord=0;
static std::uint64_t initializationCases=0;
static void initializers(original::Machine& m){
 const U begin[]={0x2134c4,0x21350c,0x213554,0x21359c};
 const U end[]={0x2134f8,0x213540,0x213588,0x2135d0};
 for(float left:{0.f,fp(0x80000000),-.5f,.75f})for(float right:{0.f,fp(0x80000000),-1.5f,.25f}){
  VehicleAnimationState49T state{};state.poseB1C=.375f;state.blendB14=.875f;state.chariotModel.model18=123;
  m.init(vehicle+0xb04,bits(left));m.init(vehicle+0xb0c,bits(right));m.put64(17,vehicle);
  for(U i=0;i<4;++i){m.put64(2,filters+i*0x20);for(U p=begin[i];p<end[i];p+=4){U dst=0;require(!m.execute(p,dst),"initializer numeric range branch");}}
  auto before=allocations;require(initializeVehicleAnimationFilters49T(left,right,state),"native initializer");require(before==allocations,"initializer allocated");
  const AnimationFilter49Q* values[]={&state.leftB00,&state.rightB08,&state.poseB18,&state.blendB10};
  for(U i=0;i<4;++i){equal(bits(values[i]->value0),m.read(filters+i*0x20),"initial filter value");equal(bits(values[i]->time4),m.read(filters+i*0x20+4),"initial filter time");equal(bits(values[i]->alpha8),m.read(filters+i*0x20+8),"initial filter alpha");equal(m.read(filters+i*0x20+12),0x7f7fffff,"original unused upper limit");equal(m.read(filters+i*0x20+16),0xff7fffff,"original unused lower limit");}
  equal(bits(state.poseB1C),bits(.375f),"initializer preservesB1C");equal(bits(state.blendB14),bits(.875f),"initializer preservesB14");equal(state.chariotModel.model18,123,"initializer preserves model");++initializationCases;
 }
}
static void sourceRun(original::Machine& m,Graph& beforeIk,std::vector<U>& ordered){
 m.put64(31,original::Machine::stop);m.put64(29,original::Machine::stack);m.v[0]={0,0,0,1};m.callSteps=0;
 U p=0x21a000;
 while(p!=original::Machine::stop){
  if(std::find(std::begin(stages),std::end(stages),p)!=std::end(stages))ordered.push_back(p);
  if(p==0x21a620)graphFromOriginal(m,beforeIk);
  U target=0,unused=0;
  if(m.execute(p,target)){const bool delay=m.delay;if(delay)require(!m.execute(p+4,unused),"source delay branch");p=target;}else p+=4;
 }
}
static void runCase(Fixture& f,original::Machine& m,VehicleAnimationWorkspace49T& w){
 setup(m,f);if(mutationPc)m.substitutions[mutationPc]=mutationWord;
 const auto priorVisual=f.visual;const auto priorRecords=f.records;
 Graph preIk=f.nodes;std::vector<U> ordered;sourceRun(m,preIk,ordered);
#if RR_VEHICLE_ANIMATION49T_TRACE_ACTIVE
 vehicle_animation49t_trace::clear();
#endif
 VehicleAnimationDiagnostic49T d{};const auto before=allocations;++nativeCalls;
 const bool ok=advanceVehicleAnimation49T(f.input,f.bindings,{f.nodes.data(),U(f.nodes.size())},f.state,f.shared,f.visual,w,d);
 require(before==allocations,"native allocated");
 if(!ok){std::ostringstream s;s<<"native rejected error="<<U(d.error)<<" pc="<<std::hex<<d.sourceStage<<" motion="<<U(d.motion.error)<<" ik="<<U(d.ik.error)<<" ikpc="<<d.ik.sourceStage;throw std::runtime_error(s.str());}
 compare(f,m);require(!std::memcmp(&priorRecords,&f.records,sizeof f.records),"runtime changed IK coefficients");
 for(U i=0;i<9;++i){equal(priorVisual.globals[i].guard,f.visual.globals[i].guard,"unrelated visual guard");equal(priorVisual.globals[i].word,f.visual.globals[i].word,"unrelated visual word");}
 for(U i=0;i<4;++i)equal(bits(priorVisual.zero434630.v[i]),bits(f.visual.zero434630.v[i]),"shared visual zero");
 require(f.shared.libm.mode==1&&f.shared.libm.defaultMatherr&&f.shared.libm.coldNanRecord,"changed libm admission");
#if RR_VEHICLE_ANIMATION49T_TRACE_ACTIVE
 require(!vehicle_animation49t_trace::overflow&&ordered.size()==vehicle_animation49t_trace::count,"ordered stage count");
 for(U i=0;i<ordered.size();++i)equal(vehicle_animation49t_trace::events[i],ordered[i],"ordered source stage");
 if(std::find(ordered.begin(),ordered.end(),0x21a620)!=ordered.end()){
  equal(vehicle_animation49t_trace::beforeIkCount,U(preIk.size()),"pre-IK snapshot size");
  require(!std::memcmp(vehicle_animation49t_trace::beforeIk,preIk.data(),sizeof preIk),"pre-IK complete graph mismatch");
 }
#endif
 ++cases;
}
static void rejectCase(Fixture& f,VehicleAnimationWorkspace49T& w){
 const auto g=f.nodes;const auto s=f.state;const auto h=f.shared;const auto v=f.visual;
 VehicleAnimationDiagnostic49T d{};auto before=allocations;
 bool ok=advanceVehicleAnimation49T(f.input,f.bindings,{f.nodes.data(),U(f.nodes.size())},f.state,f.shared,f.visual,w,d);
 require(!ok&&d.error!=VehicleAnimationError49T::none,"invalid input accepted");require(before==allocations,"reject allocated");
 require(!std::memcmp(g.data(),f.nodes.data(),sizeof g)&&!std::memcmp(&s,&f.state,sizeof s)&&!std::memcmp(&h,&f.shared,sizeof h)&&!std::memcmp(&v,&f.visual,sizeof v),"rejection not atomic");++rejections;
}
int main(int argc,char**argv){try{
 require(argc==2||argc==4,"usage: vehicle_animation49t ROOT [mutationPC mutationWord]");
 require(std::fesetround(FE_TONEAREST)==0,"rounding mode");
 if(argc==4){mutationPc=U(std::stoul(argv[2],nullptr,16));mutationWord=U(std::stoul(argv[3],nullptr,16));}
 const std::filesystem::path root=argv[1];Assets assets(root);auto source=machine(root,assets);Fixture base(assets,*source);
 auto work=std::make_unique<VehicleAnimationWorkspace49T>();
 initializers(*source);
 // Persistent constructor coefficients remain unchanged when the complete
 // current scene is prepared and later animated.
 for(U i=0;i<base.nodes.size();++i){ik_fixture::call(*source,0x2897b0,node(i));}
 graphFromOriginal(*source,base.nodes);
 // Complete return, source skip gates, warm/cold globals, unclamped filter history.
 for(U c=0;c<128;++c){Fixture f(base);
  f.input.chariot191=(c&1)?1:0;f.input.chariot1B0=(c&2)?1:0;f.input.left191=(c&4)?1:0;f.input.right191=(c&8)?1:0;
  if(c&16)f.bindings.hands.left.target=Index::none;
  if(c&32)f.bindings.hands.right.target=Index::none;
  f.input.body340Present=!(c&64);f.input.animationDelta23C=(c%5==0)?0.f:(c%5==1?.3f:.016666668f);
  f.input.aeroE4=float(int(c%9)-3)/2;f.input.aeroE8=float(int(c%7)-2)/3;
  f.input.bodyAccelerationC0={{float(int(c%11)-5)*80,3,float(int(c%13)-6)*90,fp(0x80000000)}};
  if(c%3==0){f.shared.motion.interpolationGuard3D5D8C=0x80;f.shared.motion.pairGuard3D5DC4=0x80;f.shared.ik.sentinel443450.guard=0x80;f.shared.ik.sentinel443458.guard=0x80;f.visual.orientationSentinel4433D8.guard=0x80;}
  if(c%7==0){f.shared.enableGuard3D60F4=0;f.shared.enable434748=0;}
  if(c%11==0){f.shared.enableGuard3D60F4=0x80;f.shared.enable434748=0;}
  runCase(f,*source,*work);
 }
 // Source pointer-presence branches, including pair absence still reaching IK.
 for(U c=0;c<10;++c){Fixture f(base);
  if(c==0)f.bindings.chariot1B4=Index::none;
  if(c==1)f.bindings.brakeLeft28C=nullptr;
  if(c==2)f.bindings.brakeRight290=nullptr;
  if(c==3)f.bindings.forward280=nullptr;
  if(c==4)f.bindings.back284=nullptr;
  if(c==5)f.bindings.left2A0=nullptr;
  if(c==6)f.bindings.right29C=nullptr;
  if(c==7)f.bindings.left1BC=Index::none;
  if(c==8)f.bindings.right1B8=Index::none;
  if(c==9){f.input.body340Present=false;f.bindings.bodyScene288=Index::none;}
  runCase(f,*source,*work);
 }
 // Held/change/release controls and independently supplied acceleration at the
 // pre-physics boundary. Every graph/cache/filter/shared word persists.
 for(U scenario=0;scenario<6;++scenario){Fixture f(base);
  require(initializeVehicleAnimationFilters49T(float(int(scenario)-2)/2,float(int(scenario)-3)/2,f.state),"seed filters");
  for(U frame=0;frame<64;++frame){
   f.input.animationDelta23C=frame%9==0?0.f:frame%7==0?.125f:.016666668f;
   f.input.aeroE4=frame<16?1.f:frame<32?0.f:frame<48?.3f:-.5f;
   f.input.aeroE8=frame<8?0.f:frame<40?.8f:1.5f;
   f.input.bodyAccelerationC0={{float(int((frame+scenario*3)%13)-6)*90,0,float(int((frame+scenario)%11)-5)*110,1}};
   f.input.left191=frame%13!=4;f.input.right191=frame%11!=7;
   if(frame==3){f.nodes[50].localC0.v[12]=31;f.nodes[50].world100.v[12]=-47;f.nodes[50].world100.v[0]=0;f.nodes[50].world100.v[2]=-1;f.nodes[50].world100.v[8]=1;f.nodes[50].world100.v[10]=0;f.nodes[50].sceneFlags190|=1;}
   if(frame==4)f.nodes[50].sceneFlags190&=std::uint8_t(~1u);
   if(frame==17){f.nodes[31].sceneFlags190&=std::uint8_t(~1u);f.nodes[44].sceneFlags190|=1;}
   runCase(f,*source,*work);
  }
 }
 // Finite signed-zero inputs and initializer mirror preservation.
 for(float zero:{0.f,fp(0x80000000)}){Fixture f(base);f.input.bodyAccelerationC0={{zero,zero,zero,zero}};f.input.aeroE4=zero;f.input.aeroE8=zero;f.input.animationDelta23C=zero;runCase(f,*source,*work);}
 for(U c=0;c<12;++c){Fixture f(base);
  if(c==0)f.input.phase=VehicleAnimationPhase49T::laterPhysics;
  if(c==1)f.input.midRecordsAbsent=false;
  if(c==2)f.input.animationDelta23C=-1;
  if(c==3)f.state.leftB00.time4=0;
  if(c==4)f.state.poseB18.time4=0;
  if(c==5)f.bindings.bodyScene288=Index::none;
  if(c==6)f.bindings.left1BC=f.bindings.right1B8;
  if(c==7)f.bindings.chariotRows=nullptr;
  if(c==8)f.shared.libm.mode=0;
  if(c==9)f.records.left.mode10=0;
  if(c==10)f.input.bodyAccelerationC0.v[0]=std::numeric_limits<float>::infinity();
  if(c==11)f.nodes[48].localC0.v[0]=std::numeric_limits<float>::quiet_NaN();
  rejectCase(f,*work);
 }
 {Fixture f(base);require(std::fesetround(FE_DOWNWARD)==0,"set alternate rounding");rejectCase(f,*work);require(std::fesetround(FE_TONEAREST)==0,"restore rounding");}
 {Fixture f(base);f.state.poseB1C=.375f;f.state.blendB14=.875f;auto before=f.state;
  require(initializeVehicleAnimationFilters49T(-.5f,.75f,f.state),"initializer");equal(bits(f.state.poseB18.value0),bits(-.5f),"B18 seed comes fromB04");equal(bits(f.state.blendB10.value0),bits(.75f),"B10 seed comes fromB0C");equal(bits(f.state.poseB1C),bits(before.poseB1C),"B1C preserved");equal(bits(f.state.blendB14),bits(before.blendB14),"B14 preserved");}
 if(const char* output=std::getenv("RR_VEHICLE_ANIMATION49T_COVERAGE")){J pcs=J::array();for(U p=0x21a000;p<0x21a65c;p+=4)pcs.push_back({{"pc",p},{"hits",hits[p]},{"taken",taken[p]},{"not_taken",notTaken[p]}});std::ofstream f(output);require(bool(f),"coverage output");f<<J({{"pcs",pcs}}).dump(2)<<'\n';}
 std::cout<<J({{"cases",cases},{"checks",checks},{"initializer_cases",initializationCases},{"native_calls",nativeCalls},{"rejections",rejections},{"source_instructions",instructions},{"real_returns",fullReturns},{"source_ik_calls",ikCalls},{"source_substitutions",mutationPc?1:0},{"native_hot_allocations",0},{"stage_trace",bool(RR_VEHICLE_ANIMATION49T_TRACE_ACTIVE)}}).dump()<<'\n';
 return 0;
}catch(const std::exception& e){std::cerr<<"FAIL vehicle_animation49t case="<<cases<<": "<<e.what()<<'\n';return 1;}}
