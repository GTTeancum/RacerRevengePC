#include <string>
#include "../../evidence/chariotLifecycle49V/lifecycle/source/lifecycle_fixture.h"
#include <filesystem>
#include <new>
#include <cfenv>
#include <limits>
static std::uint64_t allocationCount=0;
[[gnu::noinline]] void* operator new(std::size_t n){void*p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();++allocationCount;return p;}
[[gnu::noinline]] void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void operator delete(void*p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete(void*p,std::size_t)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
using namespace chariot49v_test;
std::vector<std::uint8_t> snapshot(const Fixture&f){std::vector<std::uint8_t> words;auto add=[&](const void*p,std::size_t n){const auto*b=static_cast<const std::uint8_t*>(p);words.insert(words.end(),b,b+n);};add(f.nodes.data(),f.nodes.size()*sizeof(SceneNode49P));for(auto p:{std::pair<const void*,std::size_t>{&f.state,sizeof f.state},{&f.shared,sizeof f.shared},{&f.main,sizeof f.main},{&f.mainLanes,sizeof f.mainLanes},{&f.secondary,sizeof f.secondary},{&f.supplement,sizeof f.supplement},{&f.physical,sizeof f.physical},{&f.noise,sizeof f.noise}})add(p.first,p.second);return words;}
void reject(Fixture&f,U expectedStage=0){const auto before=snapshot(f);ChariotLifecycleWorkspace49V w{};ChariotLifecycleDiagnostic49V d{};bool ok=updateChariotLifecycle49V(f.input,f.parameters,f.bindings,{f.nodes.data(),U(f.nodes.size())},f.main,f.mainLanes,f.secondary,f.supplement,f.globals,f.physical,f.state,f.shared,f.zero,f.noise,w,d);require(!ok&&d.error!=ChariotLifecycleError49V::none,"expected lifecycle rejection");require(snapshot(f)==before,"lifecycle rejection changed committed state");if(expectedStage)same(d.sourceStage,expectedStage,"late failure stage");}
int main(int argc,char**argv){try{
 if(argc>1)std::filesystem::current_path(argv[1]);
 if(argc==4){chariot49v_test::mutationPc=std::stoul(argv[2],nullptr,16);chariot49v_test::mutationWord=std::stoul(argv[3],nullptr,16);}
 allocationCounter=&allocationCount;
 for(bool cold:{false,true}){Fixture f;if(!cold)f.readyNoise();f.run();for(unsigned frame=0;frame<12;++frame)f.run();}
 // Original matrices seed the response. Exercise its live branches and the
 // full return with zero delta, zero/reversed ranges, all gates and warm words.
 for(U scenario=0;scenario<88;++scenario){Fixture f;f.readyNoise();
  if(scenario%4==0){f.state.response.linearVelocity={{2,3,4,fp(0x7fc12345)}};f.state.response.angularVelocity={{5,6,7,fp(0x80000000)}};}
  if(scenario%5==0){f.nodes[3].localC0=matrix(.91f,2,6,-10);f.nodes[1].localC0=matrix(-.26f,.4f,-.2f,.7f);}
  f.run();
  f.input.aeroD8=scenario&1;f.main.components.probe348.field10C=(scenario&2)?0:3;f.state.vehicle840=(scenario&4)?1:0;
  f.input.vehicleCFC=scenario&8;f.input.vehicleD00=scenario&16;f.input.objectD34_10=(scenario&32)?.1f:.7f;
  f.main.components.root.body.angularVelocity.v[1]=(float(int(scenario%9)-4))*.4f;
  f.input.frameFactor=scenario%7==0?0.0f:.013f;
  f.main.cachedSpeed34=fp(0x4312aa7f)+float(scenario%6)*70;
  f.input.impactBA0={{float(int(scenario%5)-2)*20,-30,40,fp(0x7fc12345)}};
  std::copy_n(std::array<float,4>{.2f,(scenario&1)?20.f:-20.f,.1f,fp(0x7fc23456)}.data(),4,f.main.components.probe348.vector1D0);
  if(scenario%8==0){f.parameters.minAmplitudeA4C=.4f;f.parameters.maxAmplitudeA50=.02f;f.parameters.minTimeA3C=1.8f;f.parameters.maxTimeA40=.2f;}
  if(scenario%11==0)f.parameters.effectiveSpeedSquaredA54=0;
  if(scenario%13==0)f.parameters.yawInputRangeA88=0;
  if(scenario%17==0){f.shared.speed434750={0xff,0};f.shared.yaw434758={0xa5,bits(.3f)};}
  if(scenario%19==0){f.parameters.orientationScaleA60=0;f.parameters.minAmplitudeA4C=0;f.parameters.maxAmplitudeA50=0;}
  if(scenario%23==0)f.main.components.root.localVelocity80={{0,0,0}};
  if(scenario%29==0){f.state.response.orientation={{0,0,0,0}};f.parameters.minAmplitudeA4C=f.parameters.maxAmplitudeA50=0;}
  if(scenario%31==0){f.state.response.position={{0,0,0,fp(0x7fc45678)}};f.nodes[2].localC0=matrix();f.nodes[2].sceneFlags190=0;}
  f.state.response.linearVelocity.v[3]=fp(0x7fc56789);f.state.response.angularVelocity.v[3]=fp(0x7fc6789a);
  for(U i=0;i<3;++i)f.run();
 }
 for(U scenario=0;scenario<8;++scenario){Fixture f;f.readyNoise();f.main.cachedSpeed34=300;f.shared.speed434750={0x81,bits((scenario&1)?-10.f:100.f)};f.parameters.yawAngleA8C=(scenario&2)?-.9f:.3f;
  if(scenario&4){f.parameters.effectiveSpeedSquaredA54=-10000;f.parameters.minAmplitudeA4C=.4f;f.parameters.maxAmplitudeA50=.02f;f.parameters.minTimeA3C=1.8f;f.parameters.maxTimeA40=.2f;}
  else f.parameters.effectiveSpeedSquaredA54=-10000;
  f.run();f.run();}
 // Independently initialized vehicles deliberately interleave one shared lazy
 // context and one noise/RNG object. Their actual caller clocks remain distinct.
 {Fixture a,z;a.readyNoise();z.readyNoise();a.run();z.run();ChariotRootShared49U common=a.shared;CoupledNoiseContext49M sharedNoise{};sharedNoise.sharedRandomState=0x76543210;for(U i=0;i<24;++i){auto&f=i&1?z:a;f.shared=common;f.noise=sharedNoise;f.run();common=f.shared;sharedNoise=f.noise;}require(sharedNoise.noise.initialized,"interleaved cold noise");}
 // Every mode, source threshold equality, strict expiry, zero delta and speed
 // reversal are exercised persistently through the real original return.
 for(bool cold:{false,true}){
  Fixture f;if(!cold)f.readyNoise();f.physical=9;f.main.cachedSpeed34=100;
  f.secondary.root.body.position={{-13,14,-15}};f.state.previous85C=3;f.run();f.run();
  require(f.physical==9&&!f.state.vehicle83C,"stable physical selector preservation");
  f.main.cachedSpeed34=fp(0x4312aa7f);f.run();require(!f.physical&&f.state.vehicle83C==1,"equal speed must seed virtual");
  f.main.cachedSpeed34=std::nextafter(fp(0x4312aa7f),0.0f);f.run();
  require(f.physical==1&&f.state.vehicle83C==1&&bits(f.state.elapsed84C)==0,"reverse retains virtual and resets timer");
  same(bits(f.secondary.cachedSpeed34),0,"reverse cached speed remains reset");
  same(bits(f.secondary.velocity70W),bits(f.mainLanes.velocity70W),"full main70W copy");
  const auto world=f.nodes[3].world100;require(!(f.nodes[3].sceneFlags190&1),"reverse physical cache must remain dirty");
  f.input.frameFactor=.25f;
  for(U i=0;i<4;++i){f.main.cachedSpeed34=i&1?200:100;f.run();require(f.physical&&f.state.vehicle83C,"blend must retain modes through equality");require(!std::memcmp(&world,&f.nodes[3].world100,sizeof world),"blend eagerly refreshed physical world");}
  f.main.cachedSpeed34=200;f.input.frameFactor=0;f.run();require(f.physical&&f.state.vehicle83C,"zero-delta equality expires blend");
  f.input.frameFactor=.125f;f.run();require(!f.physical&&f.state.vehicle83C==1,"expired high-speed blend must reseed virtual");
  f.main.cachedSpeed34=100;f.run();for(U i=0;i<10;++i)f.run();require(f.physical&&!f.state.vehicle83C,"expired low blend must become physical");
  f.main.cachedSpeed34=200;f.run();f.main.cachedSpeed34=100;f.run();f.run();
 }
 for(float elapsed:{-.25f,-0.0f,0.f,.125f,.5f,.875f,1.f,1.25f})for(float speed:{100.f,fp(0x4312aa7f),200.f}){
  Fixture f;f.readyNoise();f.physical=7;f.state.vehicle83C=3;f.state.elapsed84C=elapsed;f.main.cachedSpeed34=speed;f.input.frameFactor=.125f;
  f.nodes[2].localC0=matrix(.24f,3,2,-20);f.state.response.position={{-2,.3f,-20,1}};
  f.nodes[3].localC0=matrix(.3f,70,80,90);f.nodes[3].world100=matrix(.7f,40,50,60);
  f.nodes[3].sceneFlags190=0x80;f.synchronizeSecondaryScene();f.run();f.run();
 }
 // Reversed/cold speed globals do not eliminate the response prefix or its
 // six calls. Nonzero selectors remain their original words until source writes.
 for(U i=0;i<12;++i){Fixture f;f.readyNoise();f.physical=0;f.state.vehicle83C=0x8123;f.main.cachedSpeed34=i%2?50:-30;
  f.shared.speed434750={0xb3,bits(i%3?100.f:-20.f)};f.input.frameFactor=i%4?0.015f:0.f;
  f.parameters.minAmplitudeA4C=f.parameters.maxAmplitudeA50=0;f.run();f.run();}
 // A late timer overflow occurs after staged response, noise, basis rebuild and
 // graph publication. All externally visible records must still roll back.
 {Fixture f;f.readyNoise();f.run();f.physical=1;f.state.elapsed84C=std::numeric_limits<float>::max();
  f.state.duration848=std::numeric_limits<float>::max();f.input.frameFactor=std::numeric_limits<float>::max();
  f.parameters={};f.parameters.restLength854=20;f.parameters.yawInputRangeA88=.5f;f.input.aeroD8=1;f.state.response.linearVelocity=f.zero;f.state.response.angularVelocity=f.zero;reject(f,0x21be98);}
 // Call-only rejection checks preserve complete committed records.
 for(U scenario=0;scenario<18;++scenario){Fixture f;f.readyNoise();f.run();switch(scenario){case 0:f.state.duration848=0;break;case 1:f.input.vehicle58=1;break;case 2:f.main.vehicleState40=1;break;case 3:f.state.duration848=-1;break;case 4:f.state.vehicle83C=0;break;case 5:f.input.frameFactor=-1;break;case 6:f.parameters.restLength854=std::numeric_limits<float>::infinity();break;case 7:f.secondary.time864=0x1p31f;break;case 8:f.bindings.hidden198=f.bindings.main190;break;case 9:f.nodes[7].parent180=SceneNodeIndex49P::none;break;case 10:f.shared.speed434750.word=0x7fc00000;break;
 case 11:f.state.duration848=std::numeric_limits<float>::infinity();break;
 case 12:f.secondary.scenePresent=false;break;
 case 13:f.secondary.sceneFlags190^=1;break;
 case 14:f.secondary.root.scene.v[0]+=1;break;
 case 15:f.physical=7;f.state.elapsed84C=std::numeric_limits<float>::quiet_NaN();break;
 case 16:f.physical=7;f.nodes[3].world100.v[5]=std::numeric_limits<float>::infinity();f.synchronizeSecondaryScene();break;
 case 17:f.input.phase=ChariotLifecyclePhase49V::laterPhysics;break;}reject(f);}
 {Fixture f;f.readyNoise();f.run();const int mode=std::fegetround();std::fesetround(FE_UPWARD);reject(f);std::fesetround(mode);}
 if(const char* p=std::getenv("RR_CHARIOT_LIFECYCLE49V_COVERAGE")){std::ofstream out(p);if(!out)throw std::runtime_error("coverage output");for(const auto&b:chariot49v_words::blocks)for(U i=0;i<b.count;++i){const auto k=b.index+i;out<<std::hex<<b.start+4*i<<'\t'<<std::dec<<hits[k]<<'\t'<<taken[k]<<'\t'<<notTaken[k]<<'\n';}}
 std::cout<<"{\"passed\":true,\"cases\":"<<cases<<",\"checks\":"<<checks<<",\"native_calls\":"<<nativeCalls<<",\"full_returns\":"<<fullReturns<<",\"source_instructions\":"<<instructions<<",\"noise_calls\":"<<sourceNoiseCalls<<",\"rng_calls\":"<<sourceRngCalls<<",\"reverse_calls\":"<<reverseCalls<<",\"blend_calls\":"<<blendCalls<<",\"body_bytes_compared\":"<<bodyBytesCompared<<",\"native_allocations\":"<<nativeAllocations<<"}\n";
 return 0;}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
