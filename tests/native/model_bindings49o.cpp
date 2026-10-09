#include "../../src/assets/model_bindings49o.h"
#include "../../evidence/authoredGeometry49O/bindings/bindings_cases49o.h"
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>
#include <stdexcept>
#include <string>
using namespace rr::assets;
static std::uint64_t checks=0,allocations=0,calls=0;
void* operator new(std::size_t n){++allocations;if(void*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void*p)noexcept{std::free(p);}void operator delete[](void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);}void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
static void check(bool b,const char*m){++checks;if(!b)throw std::runtime_error(m);}
using Bytes=std::array<std::uint8_t,12+224*64>;
static void word(Bytes&b,std::size_t p,std::uint32_t w){for(unsigned i=0;i<4;++i)b[p+i]=std::uint8_t(w>>(8*i));}
static Bytes hierarchy(const binding49o_cases::Case&c){Bytes b{};word(b,0,0x00677370);word(b,4,3);word(b,8,c.count);
 for(unsigned i=0;i<c.count;++i){auto p=12+224*i;std::memcpy(b.data()+p,c.names[i],std::strlen(c.names[i]));for(unsigned j=0;j<4;++j)word(b,p+128+j*20,0x3f800000);word(b,p+220,std::uint32_t(c.parents[i]));}return b;}
static void compare(const MotionNodeBindings49O&b,const binding49o_cases::Case&c){check(b.count==c.count,"row count");
 for(unsigned i=0;i<c.count;++i){const auto&r=b.rows[i];const auto&e=c.expected[i];check(std::strcmp(r.name,e.name)==0,"source row name");check(r.sceneNode==e.node,"source scene index");check(r.translationFlag38==bool(e.translation),"source translation flag");check(r.listedFlag3c==bool(e.listed),"source listed flag");}}
static bool call(const PsgHierarchyView49O&v,const char*t,std::size_t n,MotionNodeBindings49O&o,MotionBindingDiagnostic49O&d){const auto a=allocations;const bool b=readMotionNodeBindings49O(v,t,n,o,d);check(allocations==a,"binding allocated");++calls;return b;}
int main(){try{
 for(const auto&c:binding49o_cases::cases){auto b=hierarchy(c);PsgHierarchyView49O v{};GeometryDiagnostic49O pd{};check(readPsgHierarchy49O(b.data(),12+224*c.count,v,pd),"PSG fixture");MotionNodeBindings49O out{};MotionBindingDiagnostic49O d{};
  check(call(v,c.vut,std::strlen(c.vut),out,d),c.label);compare(out,c);
  // LF versus CRLF is text transport only. Source rows and flags stay exact.
  std::string cr;for(const char*p=c.vut;*p;++p){if(*p=='\r')continue;if(*p=='\n')cr+='\r';cr+=*p;}check(call(v,cr.data(),cr.size(),out,d),"CRLF");compare(out,c);
 }
 auto b=hierarchy(binding49o_cases::cases[0]);PsgHierarchyView49O v{};GeometryDiagnostic49O pd{};check(readPsgHierarchy49O(b.data(),12+18*224,v,pd),"chariot prefix");MotionNodeBindings49O out{};MotionBindingDiagnostic49O d{};
 const char* bad[]={"","\n","nullcg","nullcg=","=nullcg","nullcg=nullcg,","nullcg=nullcg,T,x","nullcg =nullcg","nullcg= nullcg","missing=thing","nullcg=x\rthing","nullcg=x\t","nullcg=x\n\n"};
 for(const auto*s:bad)check(!call(v,s,std::strlen(s),out,d),"invalid binding text admitted");
 check(!call(v,nullptr,1,out,d),"null text");std::string huge(16385,'x');check(!call(v,huge.data(),huge.size(),out,d),"text limit");
 std::string longName="nullcg="+std::string(50,'x');check(!call(v,longName.data(),longName.size(),out,d),"name limit");
 const char duplicate[]="nullcg=brake1\nbrake1=winner,T\n";check(call(v,duplicate,sizeof duplicate-1,out,d),"sequential rename");check(std::strcmp(out.rows[0].name,"WINNER")==0&&out.rows[0].translationFlag38,"first matching current row");check(std::strcmp(out.rows[2].name,"BRAKE1")==0,"second duplicate untouched");
 std::cout<<"model_bindings49o cases=128 checks="<<checks<<" zero_allocation_calls="<<calls<<" original_instructions=1585790\n";
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
