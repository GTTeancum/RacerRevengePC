#include "../../src/game/advancement_selectors49k.h"
#include "../../tools/gameplay/advancement_numeric_oracle49k.h"
#include <cfenv>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
static std::uint64_t allocations=0;
void* operator new(std::size_t n){if(void* p=std::malloc(n?n:1)){++allocations;return p;}throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept{std::free(p);}
void operator delete[](void* p) noexcept{std::free(p);}
void operator delete(void* p,std::size_t) noexcept{std::free(p);}
void operator delete[](void* p,std::size_t) noexcept{std::free(p);}
using namespace rr::game;namespace original=advancement49k_oracle;using original::U;using original::bits;using original::fp;using M=original::Machine;
static std::uint64_t checks=0,cases=0,hotCalls=0,hotAllocations=0,sensitive=0;static const char* phase="start";
static void need(bool b,const char* why){++checks;if(!b)throw std::runtime_error(why);}
static void eq(float a,float b){need(bits(a)==bits(b),"binary32 differs from source words");}
template<class Call>static auto hot(Call call){const auto before=allocations;auto result=call();hotAllocations+=allocations-before;++hotCalls;return result;}
static constexpr U VEH=0x10000,PROBE=0x20000,BODY=0x30000,AERO=0x40000,GRAV=0x50000,ENGINE=0x60000,SCENE=0x70000,CONTACT=0x80000;
static M base(){M m;m.r[19][0]=VEH;m.put(VEH+0x348,PROBE);m.put(VEH+0x340,BODY);m.put(VEH+0x350,AERO);m.put(VEH+0x344,GRAV);m.put(VEH+0x34c,ENGINE);m.put(VEH+0x354,CONTACT);return m;}
static void schedule(float factor,float scale){++cases;M m;m.r[1][0]=0x430000;m.putf(0x434188,scale);m.f[20]=factor;m.run(0x1a8384,{0x1a83b4});Schedule49K out{};need(hot([&]{return selectSchedule49K(factor,scale,out);}),"schedule rejected admitted fixture");need(out.count==m.r[18][0],"schedule count differs");eq(out.factorPerSubstep,m.f[20]);}
static float gravity(const GravitySelection49K& s){++cases;M m=base();m.put(PROBE+0x104,s.incoming348_104);m.putf(AERO+0xec,s.brake350_EC);m.putf(BODY+0x34,s.cachedSpeed340_34);m.putf(VEH+0x378,s.vehicle378);m.putf(VEH+0x37c,s.vehicle37C);m.putf(0x434730,s.effective434730);m.putf(0x434738,s.effective434738);m.putf(m.r[29][0]+0x74,s.sceneUpY);
 U end=m.run(0x21990c,{0x219964,0x219a7c,0x219a80});if(end==0x219964)m.run(0x2199c0,{0x219a7c});
 // The inactive path reaches9A7C; its source load of a0 is independently run.
 if(end!=0x219a80){U target=0;need(!m.execute(0x219a7c,target),"gravity target load");}
 m.r[4][0]=GRAV;m.run(0x1143e0);float out=123;need(hot([&]{return selectGravity49K(s,out);}),"gravity rejected admitted fixture");eq(out,m.readf(GRAV+0x38));need(m.stores.size()==1&&m.stores[0].pc==0x1143e4,"gravity setter delay store");return out;}
static void flags(U state,unsigned byte,unsigned first){++cases;M m=base();m.r[18][0]=first;m.put(VEH+0x64,state);m.put(VEH+0xb9c,byte);m.run(0x219a90,{0x219ad4,0x219c24});const auto out=hot([&]{return selectEngineFlags49K(state,static_cast<std::uint8_t>(byte));});need(out.engine34==m.read(ENGINE+0x34)&&out.engine38==m.read(ENGINE+0x38),"engine flags differ");need(m.stores.size()==2&&m.stores[0].pc==0x219ab8&&m.stores[1].pc==0x219ad0,"engine flag store order/delay slot");}
static constexpr U offsets[]={0x104,0x108,0x10c,0x110,0x114,0x118,0x11c,0x12c,0x1e8};
static std::array<U,9> scalars(const DisabledProbeState49K& s){return {s.field104,s.field108,s.field10C,s.field110,s.field114,s.field118,s.field11C,s.field12C,s.field1E8};}
static void putState(M& m,const DisabledProbeState49K& s){auto a=scalars(s);for(unsigned i=0;i<9;++i)m.put(PROBE+offsets[i],a[i]);for(unsigned i=0;i<4;++i){m.putf(PROBE+0x150+i*4,s.point150[i]);m.putf(PROBE+0x1d0+i*4,s.vector1D0[i]);}}
static void equalState(const M& m,const DisabledProbeState49K& s){auto a=scalars(s);for(unsigned i=0;i<9;++i)need(m.read(PROBE+offsets[i])==a[i],"probe scalar differs");for(unsigned i=0;i<4;++i){eq(s.point150[i],m.readf(PROBE+0x150+i*4));eq(s.vector1D0[i],m.readf(PROBE+0x1d0+i*4));}}
static DisabledProbeState49K prepare(const DisabledProbeState49K& s,const float (&local)[4],const BodyMatrix48G& matrix,const float (&clear)[4]){++cases;M m=base();m.r[4][0]=PROBE;putState(m,s);m.put(PROBE+0x128,BODY);m.put(BODY+0x288,SCENE);m.put(PROBE+0x120,0);m.put(PROBE+0x124,0);m.put(PROBE+0x100,0);for(unsigned i=0;i<4;++i){m.putf(PROBE+0x140+i*4,local[i]);m.putf(0x434260+i*4,clear[i]);}for(unsigned i=0;i<16;++i)m.putf(SCENE+0x100+i*4,matrix.v[i]);m.run(0x1b1800);DisabledProbeState49K out{};need(hot([&]{return prepareDisabledProbe49K(s,local,matrix,clear,out);}),"prepare rejected admitted");equalState(m,out);
 const U wanted[]={0x108,0x110,0x114,0x118,0x1e8,0x12c,0x1d0,0x1d4,0x1d8,0x1dc,0x150,0x154,0x158,0x15c};unsigned j=0;for(const auto& st:m.stores)if(st.address>=PROBE&&st.address<PROBE+0x300){need(j<14&&st.address==PROBE+wanted[j++],"prepare component-store order");}need(j==14,"prepare missing stores");return out;}
static DisabledProbeResponse49K response(const DisabledProbeState49K& s,float angle,float strength,float global,const Vector3_48C& axis,const Vector3_48C& torque){++cases;M m=base();m.r[16][0]=PROBE;putState(m,s);m.put(PROBE+0x128,BODY);m.putf(PROBE+0x204,strength);m.putf(BODY+0xe8,angle);m.putf(0x4342c8,global);for(unsigned i=0;i<3;++i){m.putf(0x434290+i*4,axis.v[i]);m.putf(BODY+0x190+i*4,torque.v[i]);}const U sp=m.r[29][0];for(unsigned i=0;i<4;++i){m.put(sp+0x20+i*4,i==0?0xdead0000:0);m.put(sp+0x10+i*4,0);}m.putf(sp,0);m.run(0x1b1aac,{0x1b239c});m.run(0x1b23c0);DisabledProbeResponse49K out{};need(hot([&]{return respondDisabledProbe49K(s,angle,strength,global,axis,torque,out);}),"response rejected admitted");equalState(m,out.state);for(unsigned i=0;i<3;++i)eq(out.torque190.v[i],m.readf(BODY+0x190+i*4));std::vector<U> seq;for(const auto& st:m.stores)if((st.address>=PROBE&&st.address<PROBE+0x300)||(st.address>=BODY&&st.address<BODY+0x300))seq.push_back(st.address);const std::vector<U> wanted=s.field11C?std::vector<U>{PROBE+0x104,PROBE+0x11c,PROBE+0x10c,BODY+0x190,BODY+0x194,BODY+0x198}:std::vector<U>{PROBE+0x104,PROBE+0x10c,BODY+0x190,BODY+0x194,BODY+0x198};need(seq==wanted,"response store order");return out;}
static void late(U a,U b,U c){++cases;M m=base();m.put(PROBE+0x104,a);m.put(CONTACT+0x3c,b);m.put(CONTACT+0x44,c);m.run(0x219ea4,{0x219ec0});const auto vehicle380=hot([&]{return selectVehicle38049K(c);});need(vehicle380==m.read(VEH+0x380),"vehicle380 differs from source");m.stores.clear();m.run(0x219f90,{0x219fd0});const auto r=hot([&]{return selectLateFeedback49K(a,b,c);});need(r.engine3C==m.read(ENGINE+0x3c)&&r.aeroE0==m.read(AERO+0xe0)&&r.aeroD0==m.read(AERO+0xd0)&&r.aeroD4==m.read(AERO+0xd4),"late feedback differs");need(m.stores.size()==4&&m.stores[0].pc==0x219f9c&&m.stores[1].pc==0x219fac&&m.stores[2].pc==0x219fbc&&m.stores[3].pc==0x219fcc,"feedback source store order");}
static U randWord(U& x){x^=x<<13;x^=x>>17;x^=x<<5;return x;}
static float small(U& x){return static_cast<float>(int(randWord(x)%20001)-10000)/257.0f;}
int main(int argc,char** argv){try{
 need(std::fesetround(FE_TONEAREST)==0,"rounding mode");
 phase="schedule";for(float factor:{0.0f,-0.0f,0.001f,0.016f,0.033333335f,fp(0x3d75c28f)})for(float scale:{0.0f,-0.0f,1.0f,16.0f,30.0f,50.0f,60.0f})schedule(factor,scale);
 for(U boundary:{0x3f800000u,0x40000000u,0x40400000u})for(int d:{-1,0,1})schedule(0.03125f,fp(boundary+U(d))*32.0f);
 phase="gravity";for(U active:{0u,1u,0xffffffffu})for(float brake:{-1.0f,-0.0f,0.0f,0.5f})for(float speed:{-3.0f,fp(0x4212aa7e),fp(0x4212aa7f),fp(0x4212aa80),200.0f})for(float up:{-2.0f,-0.0f,0.2f,fp(0x3f34fdf4),1.0f,3.0f})for(float baseValue:{30.0f,fp(0x43c13334),600.0f})for(float threshold:{-0.7f,0.0f,fp(0x3f34fdf4)})gravity({active,brake,speed,up,baseValue,17.0f,fp(0x43c13334),threshold});
 phase="engine flags";for(U state:{0u,1u,2u,3u,0x80000000u,0xffffffffu})for(unsigned byte=0;byte<256;++byte)for(unsigned first=0;first<2;++first)flags(state,byte,first);
 phase="probe";U seed=0x49abcdef;for(unsigned i=0;i<600;++i){DisabledProbeState49K s{randWord(seed),randWord(seed),randWord(seed),randWord(seed),randWord(seed),randWord(seed),i%2?randWord(seed):0u,randWord(seed),randWord(seed),{1,2,3,4},{5,6,7,8}};BodyMatrix48G matrix{};for(float& f:matrix.v)f=small(seed);float local[4]={small(seed),small(seed),small(seed),small(seed)},clear[4]={small(seed),small(seed),small(seed),small(seed)};s=prepare(s,local,matrix,clear);Vector3_48C axis{{small(seed),small(seed),small(seed)}},torque{{small(seed),small(seed),small(seed)}};response(s,small(seed),small(seed),i%2?fp(0x3e99999a):small(seed),axis,torque);late(randWord(seed),randWord(seed),randWord(seed));}
 late(0,0,0);late(1,0,1);late(0xffffffffu,0xffffffffu,0xffffffffu);
 phase="post";for(U value:{0u,1u,0xffffffffu}){M m;m.r[4][0]=BODY;m.put(BODY+0x20c,value);m.run(0x165420);U out=value;const auto before=allocations;clearMainBodyPost20C49K(out);hotAllocations+=allocations-before;++hotCalls;need(out==m.read(BODY+0x20c)&&out==0,"post callback");need(m.stores.size()==1&&m.stores[0].pc==0x165424,"post delay-slot store");}
 phase="safe scalar/state sensitivity";Schedule49K s1{},s2{};need(selectSchedule49K(.03125f,31.0f,s1)&&selectSchedule49K(.03125f,32.0f,s2)&&s1.count!=s2.count,"schedule boundary sensitivity");++sensitive;GravitySelection49K g{0,0,80,.25f,10,20,fp(0x43c13334),fp(0x3f34fdf4)};const float incoming=gravity(g);g.incoming348_104=1;need(bits(incoming)!=bits(gravity(g)),"incoming104 sensitivity");++sensitive;g.brake350_EC=1;g.cachedSpeed340_34=2;need(gravity(g)==0,"brake/cache sensitivity");++sensitive;
 DisabledProbeState49K p{1,0,0,0,0,0,7,0,0,{0,0,0,1},{0,0,0,1}};const Vector3_48C axis{{1,2,3}},torque{{3,4,5}};auto a=response(p,1,2,.3f,axis,torque),b=response(p,2,2,.3f,axis,torque);need(bits(a.torque190.v[0])!=bits(b.torque190.v[0])&&a.state.field104==0&&a.state.field10C==1&&a.state.field11C==0,"airborne cache/state sensitivity");++sensitive;
 phase="bounded scalar rejections";Schedule49K sentinel{7,9},old=sentinel;for(auto bad:std::initializer_list<std::array<float,2>>{{-.001f,60},{.061f,60},{.03125f,128},{.03f,-1},{std::numeric_limits<float>::infinity(),1}}){need(!hot([&]{return selectSchedule49K(bad[0],bad[1],sentinel);}),"out-of-domain scheduler admitted");need(sentinel.count==old.count&&bits(sentinel.factorPerSubstep)==bits(old.factorPerSubstep),"schedule rejection wrote output");}
 p.field110=1;DisabledProbeResponse49K responseSentinel{p,torque};need(!hot([&]{return respondDisabledProbe49K(p,1,2,.3f,axis,torque,responseSentinel);}),"enabled-hit response admitted");need(responseSentinel.state.field104==1,"response rejection wrote state");
 phase="rounding-mode admission";
 for(int mode:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){
  need(std::fesetround(mode)==0,"set nonnearest mode");
  Schedule49K sc{3,7};float grav=9;DisabledProbeState49K stateOut=p;DisabledProbeResponse49K responseOut{p,torque};
  BodyMatrix48G matrix{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};float local[4]={1,2,3,1},clear[4]={0,0,0,1};p.field110=0;
  need(!hot([&]{return selectSchedule49K(.03f,60,sc);})&&sc.count==3&&sc.factorPerSubstep==7,"schedule rounding admission");
  need(!hot([&]{return selectGravity49K(g,grav);})&&grav==9,"gravity rounding admission");
  need(!hot([&]{return prepareDisabledProbe49K(p,local,matrix,clear,stateOut);})&&stateOut.field104==p.field104,"prepare rounding admission");
  need(!hot([&]{return respondDisabledProbe49K(p,1,2,.3f,axis,torque,responseOut);})&&responseOut.state.field104==p.field104,"response rounding admission");
  auto e=selectEngineFlags49K(2,2);auto l=selectLateFeedback49K(7,8,9);U post=1;clearMainBodyPost20C49K(post);
  need(e.engine34==1&&e.engine38==1&&l.engine3C==7&&l.aeroD4==9&&post==0,"integer leaves depend on rounding mode");
 }
 need(std::fesetround(FE_TONEAREST)==0,"restore nearest mode");
 phase="additional safe scalar sensitivities";
 need(selectEngineFlags49K(1,0).engine34!=selectEngineFlags49K(3,0).engine34,"boost state sensitivity");++sensitive;
 need(selectEngineFlags49K(0,2).engine38!=selectEngineFlags49K(0,0).engine38,"repair bit sensitivity");++sensitive;
 need(selectLateFeedback49K(7,8,9).aeroE0!=selectLateFeedback49K(0,8,9).aeroE0&&selectLateFeedback49K(7,8,9).aeroD4!=selectLateFeedback49K(7,8,10).aeroD4,"late feedback raw state sensitivity");++sensitive;
 BodyMatrix48G sensitivityMatrix{{1,0,0,0,0,1,0,0,0,0,1,0,10,20,30,1}};float local[4]={1,2,3,99},clear[4]={0,0,0,1};
 auto pa=prepare(p,local,sensitivityMatrix,clear);local[0]=2;clear[2]=4;auto pb=prepare(p,local,sensitivityMatrix,clear);
 need(bits(pa.point150[0])!=bits(pb.point150[0])&&pa.point150[3]==1&&pb.point150[3]==1&&bits(pa.vector1D0[2])!=bits(pb.vector1D0[2]),"preparation point/global sensitivity");++sensitive;
 phase="coverage";unsigned reached=0;for(auto n:original::coverage)if(n)++reached;need(reached==original::wordCount,"selected source coverage incomplete");need(hotAllocations==0,"successful hot calls allocated");
 if(argc>1){std::ofstream f(std::string(argv[1])+"-coverage.json");f<<"{\n  \"selected_words\": "<<original::wordCount<<",\n  \"reached_words\": "<<reached<<",\n  \"pcs\": [\n";for(unsigned i=0;i<original::wordCount;++i){f<<"    {\"pc\":\"0x"<<std::hex<<advancement49k_source::words[i].pc<<std::dec<<"\",\"executions\":"<<original::coverage[i]<<",\"taken\":"<<original::taken[i]<<",\"not_taken\":"<<original::notTaken[i]<<"}"<<(i+1==original::wordCount?"\n":",\n");}f<<"  ]\n}\n";}
 std::cout<<"{\"status\":\"passed\",\"cases\":"<<cases<<",\"checks\":"<<checks<<",\"hot_calls\":"<<hotCalls<<",\"hot_allocations\":"<<hotAllocations<<",\"selected_words\":"<<original::wordCount<<",\"reached_words\":"<<reached<<",\"interpreted_instructions\":"<<original::instructionCount<<",\"scalar_state_sensitivity_checks\":"<<sensitive<<"}\n";
 return 0;
 }catch(const std::exception& e){std::cerr<<"49K numeric failure in "<<phase<<": "<<e.what()<<"\n";return 1;}}
