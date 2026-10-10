#include <string>
#include "../../evidence/chariotRoot49U/root/source/root_fixture.h"
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
using namespace chariot49u_test;
std::vector<U> snapshot(const Fixture&f){std::vector<U> words;auto scalar=[&](float x){words.push_back(bits(x));};for(const auto&n:f.nodes){for(float x:n.localC0.v)scalar(x);for(float x:n.world100.v)scalar(x);words.push_back(U(n.parent180));words.push_back(U(n.firstChild184));words.push_back(U(n.nextSibling188));words.push_back(n.sceneFlags190);}words.push_back(f.state.vehicle838);words.push_back(f.state.vehicle83C);words.push_back(f.state.vehicle840);scalar(f.state.previous85C);for(const auto*v:{f.state.response.position.v,f.state.response.linearVelocity.v,f.state.response.angularVelocity.v,f.state.response.orientation.v})for(U i=0;i<4;++i)scalar(v[i]);scalar(f.phase);words.push_back(f.shared.speed434750.guard);words.push_back(f.shared.speed434750.word);words.push_back(f.shared.yaw434758.guard);words.push_back(f.shared.yaw434758.word);for(float x:f.shared.up434650.v)scalar(x);words.push_back(f.noise.sharedRandomState);words.push_back(f.noise.noise.initialized);words.push_back(U(f.noise.noise.scratchCell));for(U x:f.noise.noise.permutation)words.push_back(x);for(float x:f.noise.noise.gradient1D)scalar(x);return words;}
void reject(Fixture&f){const auto before=snapshot(f);ChariotRootWorkspace49U w{};ChariotRootDiagnostic49U d{};bool ok=updateChariotRoot49U(f.input,f.parameters,f.bindings,{f.nodes.data(),U(f.nodes.size())},f.state,f.shared,f.zero,f.phase,f.noise,w,d);require(!ok&&d.error!=ChariotRootError49U::none,"expected root rejection");require(snapshot(f)==before,"root rejection changed committed state");}
int main(int argc,char**argv){try{
 if(argc>1)std::filesystem::current_path(argv[1]);
 if(argc==4){chariot49u_test::mutationPc=std::stoul(argv[2],nullptr,16);chariot49u_test::mutationWord=std::stoul(argv[3],nullptr,16);}
 allocationCounter=&allocationCount;
 for(bool cold:{false,true}){Fixture f;if(!cold)f.readyNoise();f.run();for(unsigned frame=0;frame<12;++frame)f.run();}
 // Original matrices seed the response. Exercise its live branches and the
 // full return with zero delta, zero/reversed ranges, all gates and warm words.
 for(U scenario=0;scenario<88;++scenario){Fixture f;f.readyNoise();
  if(scenario%4==0){f.state.response.linearVelocity={{2,3,4,fp(0x7fc12345)}};f.state.response.angularVelocity={{5,6,7,fp(0x80000000)}};}
  if(scenario%5==0){f.nodes[3].localC0=matrix(.91f,2,6,-10);f.nodes[1].localC0=matrix(-.26f,.4f,-.2f,.7f);}
  f.run();
  f.input.aeroD8=scenario&1;f.input.probe10C=(scenario&2)?0:3;f.state.vehicle840=(scenario&4)?1:0;
  f.input.vehicleCFC=scenario&8;f.input.vehicleD00=scenario&16;f.input.objectD34_10=(scenario&32)?.1f:.7f;
  f.input.bodyAngularVelocityYB4=(float(int(scenario%9)-4))*.4f;
  f.input.frameFactor=scenario%7==0?0.0f:.013f;
  f.input.cachedBodySpeed34=fp(0x4312aa7f)+float(scenario%6)*70;
  f.input.impactBA0={{float(int(scenario%5)-2)*20,-30,40,fp(0x7fc12345)}};
  f.input.probeVector1D0={{.2f,(scenario&1)?20.f:-20.f,.1f,fp(0x7fc23456)}};
  if(scenario%8==0){f.parameters.minAmplitudeA4C=.4f;f.parameters.maxAmplitudeA50=.02f;f.parameters.minTimeA3C=1.8f;f.parameters.maxTimeA40=.2f;}
  if(scenario%11==0)f.parameters.effectiveSpeedSquaredA54=0;
  if(scenario%13==0)f.parameters.yawInputRangeA88=0;
  if(scenario%17==0){f.shared.speed434750={0xff,0};f.shared.yaw434758={0xa5,bits(.3f)};}
  if(scenario%19==0){f.parameters.orientationScaleA60=0;f.parameters.minAmplitudeA4C=0;f.parameters.maxAmplitudeA50=0;}
  if(scenario%23==0)f.input.bodyVelocity80={{0,0,0,fp(0x7fc34567)}};
  else f.input.bodyVelocity80.v[3]=fp(0x7fc34567);
  if(scenario%29==0){f.state.response.orientation={{0,0,0,0}};f.parameters.minAmplitudeA4C=f.parameters.maxAmplitudeA50=0;}
  if(scenario%31==0){f.state.response.position={{0,0,0,fp(0x7fc45678)}};f.nodes[2].localC0=matrix();f.nodes[2].sceneFlags190=0;}
  f.state.response.linearVelocity.v[3]=fp(0x7fc56789);f.state.response.angularVelocity.v[3]=fp(0x7fc6789a);
  for(U i=0;i<3;++i)f.run();
 }
 for(U scenario=0;scenario<8;++scenario){Fixture f;f.readyNoise();f.input.cachedBodySpeed34=300;f.shared.speed434750={0x81,bits((scenario&1)?-10.f:100.f)};f.parameters.yawAngleA8C=(scenario&2)?-.9f:.3f;
  if(scenario&4){f.parameters.effectiveSpeedSquaredA54=-10000;f.parameters.minAmplitudeA4C=.4f;f.parameters.maxAmplitudeA50=.02f;f.parameters.minTimeA3C=1.8f;f.parameters.maxTimeA40=.2f;}
  else f.parameters.effectiveSpeedSquaredA54=-10000;
  f.run();f.run();}
 // Independently initialized vehicles deliberately interleave one shared lazy
 // context and one noise/RNG object. Their actual caller clocks remain distinct.
 {Fixture a,z;a.readyNoise();z.readyNoise();a.run();z.run();ChariotRootShared49U common=a.shared;CoupledNoiseContext49M sharedNoise{};sharedNoise.sharedRandomState=0x76543210;for(U i=0;i<24;++i){auto&f=i&1?z:a;f.shared=common;f.noise=sharedNoise;f.run();common=f.shared;sharedNoise=f.noise;}require(sharedNoise.noise.initialized,"interleaved cold noise");}
 // Call-only rejection checks preserve complete committed records.
 for(U scenario=0;scenario<11;++scenario){Fixture f;f.readyNoise();f.run();switch(scenario){case 0:f.input.cachedBodySpeed34=0;break;case 1:f.input.vehicle58=1;break;case 2:f.input.vehicle40=1;break;case 3:f.state.vehicle838=1;break;case 4:f.state.vehicle83C=0;break;case 5:f.input.frameFactor=-1;break;case 6:f.parameters.restLength854=std::numeric_limits<float>::infinity();break;case 7:f.phase=0x1p31f;break;case 8:f.bindings.hidden198=f.bindings.main190;break;case 9:f.nodes[7].parent180=SceneNodeIndex49P::none;break;case 10:f.shared.speed434750.word=0x7fc00000;break;}reject(f);}
 {Fixture f;f.readyNoise();f.run();const int mode=std::fegetround();std::fesetround(FE_UPWARD);reject(f);std::fesetround(mode);}
 if(const char* p=std::getenv("RR_CHARIOT_ROOT49U_COVERAGE")){std::ofstream out(p);if(!out)throw std::runtime_error("coverage output");for(const auto&b:chariot49u_words::blocks)for(U i=0;i<b.count;++i){const auto k=b.index+i;out<<std::hex<<b.start+4*i<<'\t'<<std::dec<<hits[k]<<'\t'<<taken[k]<<'\t'<<notTaken[k]<<'\n';}}
 std::cout<<"{\"passed\":true,\"cases\":"<<cases<<",\"checks\":"<<checks<<",\"native_calls\":"<<nativeCalls<<",\"full_returns\":"<<fullReturns<<",\"source_instructions\":"<<instructions<<",\"noise_calls\":"<<sourceNoiseCalls<<",\"rng_calls\":"<<sourceRngCalls<<",\"native_allocations\":"<<nativeAllocations<<"}\n";
 return 0;}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
