#include "../../src/game/static_impact49y.h"
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <limits>
#include <new>
#include <cfenv>
#include <array>
#include <cstddef>
namespace { unsigned checks=0,failures=0,allocations=0;bool hot=false;
void check(bool yes,const char* label,unsigned n=0){++checks;if(!yes){++failures;if(failures<20)std::printf("FAIL %s case=%u\n",label,n);}}
float fp(std::uint32_t w){float f;std::memcpy(&f,&w,4);return f;}
std::uint32_t bits(const float& f){std::uint32_t w;std::memcpy(&w,&f,4);return w;}
void rawPut(float& destination,std::uint32_t word){std::memcpy(&destination,&word,4);}
template<class T> std::array<unsigned char,sizeof(T)> snapshot(const T& object){std::array<unsigned char,sizeof(T)> bytes;std::memcpy(bytes.data(),&object,sizeof object);return bytes;}
void copyOnlyWords(){
 using namespace rr::game;using S=StaticImpactStatus49Y;
 // Never construct, return, or compare these payloads through a float value.
 // In particular an IA32 x87 FLD/FSTP can quiet an SNaN in a test helper.
 constexpr std::uint32_t patterns[]={0x7f800001u,0xff800123u,0x7fa54321u,0x7fc12345u,0xffc54321u,0x7f800000u,0xff800000u,0x00000000u,0x80000000u,0x00000001u,0x007fffffu,0x80000001u};
 for(unsigned n=0;n<sizeof(patterns)/sizeof(patterns[0]);++n){
  StaticImpactInput49Y in{};in.sceneFlags190=1;in.readyMainScene190={{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};in.accumulatedImpact1C0[0]=1;
  for(float& value:in.tuning.severities14)value=1;
  StaticImpactState49Y state{},out{};rawPut(state.impactBA0[3],patterns[n]);rawPut(in.accumulatedImpact1C0[3],patterns[n]);
  auto* objectBytes=reinterpret_cast<unsigned char*>(&in);
  for(std::size_t p=offsetof(StaticImpactInput49Y,sceneFlags190)+sizeof(in.sceneFlags190);p<offsetof(StaticImpactInput49Y,readyMainScene190);++p)objectBytes[p]=0xa5;
  check(bits(state.impactBA0[3])==patterns[n]&&bits(in.accumulatedImpact1C0[3])==patterns[n],"active raw payload precall",n);
  const auto inputBytes=snapshot(in);const auto stateBytes=snapshot(state);float w=0;
  hot=true;const auto status=recordStaticImpact49Y(in,state,out,w);hot=false;
  check(status==S::applied,"copy-only exceptional active status",n);
  check(bits(out.impactBA0[3])==patterns[n],"active BA0W exact exceptional payload",n);
  check(std::memcmp(inputBytes.data(),&in,sizeof in)==0&&std::memcmp(stateBytes.data(),&state,sizeof state)==0,"active immutable raw bytes and padding",n);
  StaticImpactState49Y alias;std::memcpy(&alias,&state,sizeof alias);
  check(bits(alias.impactBA0[3])==patterns[n],"active alias raw payload precall",n);
  hot=true;const auto aliasStatus=recordStaticImpact49Y(in,alias,alias,w);hot=false;
  check(aliasStatus==S::applied&&std::memcmp(&alias,&out,sizeof out)==0,"active in-place opaque payload",n);
  for(unsigned gate=0;gate<3;++gate){
   StaticImpactInput49Y skipped{};skipped.flagsCD8=gate==0?1:0;skipped.vehicle40=gate==1?3:(gate==2?4:0);skipped.vehicle58=1;skipped.bodyContactPartnerCount20C=17;skipped.sceneFlags190=0;
   auto poison=[&](float& word){rawPut(word,patterns[n]);check(bits(word)==patterns[n],"skipped raw payload precall",n);};
   for(float& word:skipped.readyMainScene190.v)poison(word);
   for(float& word:skipped.accumulatedImpact1C0)poison(word);
   poison(skipped.reductionCF0);for(float& word:skipped.tuning.thresholds00)poison(word);for(float& word:skipped.tuning.severities14)poison(word);poison(skipped.priorVf9W);
   StaticImpactState49Y old{},result;for(float& word:old.directionalD08)poison(word);for(float& word:old.impactBA0)poison(word);
   std::memset(&result,0xa5,sizeof result);const auto skippedBytes=snapshot(skipped);const auto oldBytes=snapshot(old);float next;rawPut(next,0xdeadbeefu);
   hot=true;const auto skipStatus=recordStaticImpact49Y(skipped,old,result,next);hot=false;
   check(skipStatus==S::skipped,"all early gates skip poisoned arithmetic",gate);
   check(std::memcmp(&result,oldBytes.data(),sizeof result)==0,"skipped state raw opaque words",n);
   check(bits(next)==patterns[n],"skipped priorW raw opaque word",n);
   check(std::memcmp(&skipped,skippedBytes.data(),sizeof skipped)==0&&std::memcmp(&old,oldBytes.data(),sizeof old)==0,"skipped immutable byte snapshots",n);
   std::memcpy(&result,&old,sizeof result);check(std::memcmp(&result,oldBytes.data(),sizeof result)==0,"skipped alias raw precall",n);
   hot=true;const auto inPlaceStatus=recordStaticImpact49Y(skipped,result,result,next);hot=false;
   check(inPlaceStatus==S::skipped&&std::memcmp(&result,oldBytes.data(),sizeof result)==0&&bits(next)==patterns[n],"skipped in-place opaque state and scratch",n);
  }
 }
}
}
void* operator new(std::size_t n){if(hot)++allocations;if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
int main(int argc,char** argv){
 using namespace rr::game;using S=StaticImpactStatus49Y;
 if(argc!=2){std::fprintf(stderr,"usage: static_impact49y fixtures.bin\n");return 2;}
 std::ifstream file(argv[1],std::ios::binary);std::uint32_t header[2]{};file.read(reinterpret_cast<char*>(header),8);
 if(!file||header[0]!=2048||header[1]!=62)return 2;
 std::vector<std::uint32_t> words(std::size_t(header[0])*header[1]);file.read(reinterpret_cast<char*>(words.data()),std::streamsize(words.size()*4));if(!file||file.peek()!=EOF)return 2;
 for(unsigned c=0;c<header[0];++c){
  const auto* r=words.data()+c*62;StaticImpactInput49Y in{};StaticImpactState49Y before{},out{};
  in.vehicle40=r[0];in.vehicle58=r[1];in.bodyContactPartnerCount20C=r[2];in.flagsCD8=std::uint8_t(r[3]);in.sceneFlags190=std::uint8_t(r[4]);
  unsigned at=5;for(auto& x:in.readyMainScene190.v)x=fp(r[at++]);for(auto& x:in.accumulatedImpact1C0)x=fp(r[at++]);in.reductionCF0=fp(r[at++]);
  for(auto& x:in.tuning.thresholds00)x=fp(r[at++]);for(auto& x:in.tuning.severities14)x=fp(r[at++]);in.priorVf9W=fp(r[at++]);
  for(auto& x:before.directionalD08)x=fp(r[at++]);for(auto& x:before.impactBA0)x=fp(r[at++]);
  const auto savedIn=snapshot(in);const auto savedState=snapshot(before);float next=fp(0x7fc54321);hot=true;const auto status=recordStaticImpact49Y(in,before,out,next);hot=false;
  check(status==((in.flagsCD8&1)||in.vehicle40==3||in.vehicle40==4?S::skipped:S::applied),"status",c);
  for(const float& x:out.directionalD08)check(bits(x)==r[at++],"directional bits",c);
  for(const float& x:out.impactBA0)check(bits(x)==r[at++],"BA0 bits",c);
  check(bits(next)==r[at++],"getter scratchW",c);
  check(std::memcmp(&in,savedIn.data(),sizeof in)==0,"input preserved including1C0",c);
  check(std::memcmp(&before,savedState.data(),sizeof before)==0,"separate state preserved",c);
  auto alias=before;float aliasW=0;hot=true;const auto aliasStatus=recordStaticImpact49Y(in,alias,alias,aliasW);hot=false;
  check(aliasStatus==status&&std::memcmp(&alias,&out,sizeof out)==0&&bits(aliasW)==bits(next),"in-place state",c);
 }
 StaticImpactInput49Y in{};in.sceneFlags190=1;in.readyMainScene190={{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
 StaticImpactState49Y s{};for(float& x:s.directionalD08)x=-0.0f;s.impactBA0[3]=fp(0x7fc12345);
 auto reject=[&](S expected){StaticImpactState49Y out;std::memset(&out,0xa5,sizeof out);auto copy=out;float w=42;hot=true;auto result=recordStaticImpact49Y(in,s,out,w);hot=false;check(result==expected,"rejection kind");check(std::memcmp(&copy,&out,sizeof out)==0&&w==42,"rejection atomic");};
 in.vehicle58=1;reject(S::unsupported);in.vehicle58=0;in.bodyContactPartnerCount20C=1;reject(S::unsupported);in.bodyContactPartnerCount20C=0;
 in.sceneFlags190=0;reject(S::unsupported);in.sceneFlags190=1;
 in.reductionCF0=std::numeric_limits<float>::quiet_NaN();reject(S::arithmetic);in.reductionCF0=0;
 in.readyMainScene190.v[12]=std::numeric_limits<float>::infinity();reject(S::arithmetic);in.readyMainScene190.v[12]=0;
 in.accumulatedImpact1C0[0]=std::numeric_limits<float>::max();reject(S::arithmetic);in.accumulatedImpact1C0[0]=0;
 in.accumulatedImpact1C0[0]=fp(1);reject(S::arithmetic);in.accumulatedImpact1C0[0]=0;
 // Normal input2^-64 has a subnormal square2^-128: finite-only admission is
 // insufficient. The scalar and VU source roots have their own flush behavior.
 in.accumulatedImpact1C0[0]=fp(0x1f800000u);reject(S::arithmetic);in.accumulatedImpact1C0[0]=0;
 in.reductionCF0=fp(0x007fffffu);reject(S::arithmetic);in.reductionCF0=0;
 std::fesetround(FE_DOWNWARD);reject(S::arithmetic);std::fesetround(FE_TONEAREST);
 // Entry gate precedes unsupported scope and arithmetic, and consumes no1C0.
 in.flagsCD8=1;in.vehicle58=1;in.sceneFlags190=0;in.accumulatedImpact1C0[0]=std::numeric_limits<float>::infinity();
 StaticImpactState49Y out{};float w=0;check(recordStaticImpact49Y(in,s,out,w)==S::skipped&&std::memcmp(&out,&s,sizeof s)==0,"early gate preserves copy bits");
 copyOnlyWords();
 check(allocations==0,"zero hot allocations");
 std::printf("static-impact49Y: %u checks, %u failures, %u hot allocations, %u-bit pointers\n",checks,failures,allocations,unsigned(sizeof(void*)*8));
 return failures?1:0;
}
