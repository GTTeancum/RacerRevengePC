#include <string>
#include "../../src/game/model_motion49q.h"
#include "../../evidence/engineAnimation49Q/single-motion/motion_machine.h"
#include <algorithm>
#include <cfenv>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <sstream>
using namespace rr::game;
namespace original=motion49q;
namespace assets=rr::assets;
using U=original::U;using I=SceneNodeIndex49P;using N=SceneNode49P;
static std::uint64_t allocations=0,checks=0,cases=0,calls=0,instructions=0,flagsCompared=0;
static std::map<U,std::uint64_t> hits,taken,notTaken;
static std::array<std::array<U,3>,20000> nativeFlags{},nativeTracks{};
static unsigned flagCount=0,trackCount=0;
#ifndef RR_MODEL49Q_TRACE_REQUIRED
#define RR_MODEL49Q_TRACE_REQUIRED 0
#endif
void rrScene49PFlagStore(U node,std::uint8_t before,std::uint8_t after) noexcept {
 if(flagCount>=nativeFlags.size())std::abort();
 nativeFlags[flagCount++]={node,before,after};
}
void rrModel49QTrack(U channel,U track,U node) noexcept {
 if(trackCount>=nativeTracks.size())std::abort();
 nativeTracks[trackCount++]={channel,track,node};
}
[[gnu::noinline]] void* operator new(std::size_t n){void*p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();++allocations;return p;}
[[gnu::noinline]] void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void operator delete(void*p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete(void*p,std::size_t)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
static void need(bool v,const char*s){++checks;if(!v)throw std::runtime_error(s);}
static U word(float f){return original::bits(f);}static float fp(U w){return original::fp(w);}
static U pointer(I i){return i==I::none?0:0x600000+static_cast<U>(i)*0x200;}
static I ix(U i){return static_cast<I>(i);}
static std::vector<std::uint8_t> bytes(const std::string&p){std::ifstream in(p,std::ios::binary);need(bool(in),"missing asset");return {std::istreambuf_iterator<char>(in),{}};}
static void poke(std::vector<std::uint8_t>&b,std::size_t at,U w){need(at+4<=b.size(),"poke range");std::memcpy(b.data()+at,&w,4);}
struct Asset {
 std::vector<std::uint8_t> data,psg,vut;
 assets::MotionNodeBindings49O bindings{};assets::PsgHierarchyView49O hierarchy{};
 Asset(const std::string&dir,const char*mot,const char*psgName,const char*vutName):data(bytes(dir+mot)),psg(bytes(dir+psgName)),vut(bytes(dir+vutName)){
  assets::GeometryDiagnostic49O gd{};assets::MotionBindingDiagnostic49O bd{};
  need(assets::readPsgHierarchy49O(psg.data(),psg.size(),hierarchy,gd),"PSG reader");
  need(assets::readMotionNodeBindings49O(hierarchy,reinterpret_cast<const char*>(vut.data()),vut.size(),bindings,bd),"source binding reader");
 }
};
static U mutationPc=0,mutationWord=0;
static void compare(const original::Machine& base,const Asset&asset,float fraction,U seed,U shape=0,U mapping=0,const std::vector<std::uint8_t>*overrideData=nullptr){
 auto m=base;const auto&raw=overrideData?*overrideData:asset.data;assets::MotionView49O motion{};assets::GeometryDiagnostic49O gd{};
 need(assets::readMot49O(raw.data(),raw.size(),motion,gd),"MOT reader");
 const U count=shape?64:asset.hierarchy.nodeCount;
 std::array<N,64> nodes{},initial{};std::memset(nodes.data(),0x5a,sizeof nodes);
 auto random=[&](){seed=seed*1664525u+1013904223u;return seed;};
 for(U i=0;i<64;++i){auto&n=nodes[i];n.parent180=n.firstChild184=n.nextSibling188=I::none;n.sceneFlags190=random()&255;
  for(U j=0;j<16;++j){n.localC0.v[j]=float(std::int32_t(random()%1025)-512)/16;n.world100.v[j]=float(std::int32_t(random()%1025)-512)/16;}
  n.localC0.v[15]=(i&1)?fp(0x80000000):-.75f;
 }
 auto link=[&](U child,U parent){nodes[child].parent180=ix(parent);auto*p=&nodes[parent].firstChild184;while(*p!=I::none)p=&nodes[static_cast<U>(*p)].nextSibling188;*p=ix(child);};
 for(U i=0;i<count;++i){if(!shape){assets::PsgNode49O node{};need(assets::readPsgNode49O(asset.hierarchy,i,node),"PSG node");if(node.parent>=0)link(i,U(node.parent));}
  else if(i&&!(shape==4&&i%7==0))link(i,shape==1?i-1:shape==2?0:random()%i);
 }
 auto bindings=asset.bindings;
 if(mapping==1){for(U i=0;i<bindings.count;++i)bindings.rows[i].sceneNode=(bindings.rows[i].sceneNode+7)%count;}
 if(mapping==2){for(U i=0;i<bindings.count;++i)bindings.rows[i].sceneNode=count-1;}
 if(mapping==3){for(U i=0;i<bindings.count;++i)bindings.rows[i].sceneNode=0;}
 if(mapping==4){for(U i=0;i<bindings.count;++i)bindings.rows[i].sceneNode=random()%count;}
 for(U i=0;i<bindings.count;++i){bindings.rows[i].translationFlag38=bool(random()&1);bindings.rows[i].listedFlag3c=bool(random()&1);}
 // Reverse every graph index without changing topology or logical row targets.
 if(seed&1){auto old=nodes;auto remap=[&](I i){return i==I::none?i:ix(count-1-U(i));};
  for(U i=0;i<count;++i){nodes[count-1-i]=old[i];auto&n=nodes[count-1-i];n.parent180=remap(n.parent180);n.firstChild184=remap(n.firstChild184);n.nextSibling188=remap(n.nextSibling188);}
  for(U i=0;i<bindings.count;++i)bindings.rows[i].sceneNode=count-1-bindings.rows[i].sceneNode;
 }
 need(validateSceneGraph49P({nodes.data(),count}),"fixture topology");std::memcpy(initial.data(),nodes.data(),sizeof nodes);
 for(U i=0;i<count;++i){U p=pointer(ix(i));std::memset(&m.mem[p],0xa5,0x200);
  for(U j=0;j<16;++j){m.write(p+0xc0+j*4,word(nodes[i].localC0.v[j]));m.write(p+0x100+j*4,word(nodes[i].world100.v[j]));}
  m.write(p+0x180,pointer(nodes[i].parent180));m.write(p+0x184,pointer(nodes[i].firstChild184));m.write(p+0x188,pointer(nodes[i].nextSibling188));m.write(p+0x190,0xa5a5a500u|nodes[i].sceneFlags190);
 }
 constexpr U model=0x500000,record=0x510000,data=0x540000,rows=0x520000;
 std::memcpy(&m.mem[data],raw.data(),raw.size());m.write(model+0x1ec,rows);m.write(model+0x18,0xdeadbeef);
 for(U i=0;i<bindings.count;++i){m.write(rows+i*64+0x34,pointer(ix(bindings.rows[i].sceneNode)));m.mem[rows+i*64+0x38]=bindings.rows[i].translationFlag38;m.mem[rows+i*64+0x3c]=bindings.rows[i].listedFlag3c;}
 m.write(record+0xa4,record+0x200);m.write(record+0xa8,record+0x400);m.write(record+0xac,motion.positionCount);m.write(record+0xb0,motion.rotationCount);m.write(record+0xd0,word(motion.duration));
 for(U channel=0;channel<2;++channel){const U n=channel?motion.rotationCount:motion.positionCount;for(U i=0;i<n;++i){const auto&t=channel?motion.rotation[i]:motion.position[i];U p=record+0x1000+channel*0x1000+i*0x20;
  m.write(record+(channel?0x400:0x200)+4*i,p);m.write(p,t.rowIndex);m.write(p+4,1);m.write(p+8,t.keyCount);m.write(p+12,t.keyCount);m.write(p+16,channel?0x3cd410:0x3cd470);m.write(p+20,data+t.keyDataOffset);
 }}
 m.stores.clear();if(mutationPc)m.substitutions[mutationPc]=mutationWord;
 ModelMotionState49Q state{0xdeadbeef};ModelMotionWorkspace49Q workspace{};ModelMotionDiagnostic49Q d{};
 flagCount=trackCount=0;const auto before=allocations;++calls;
 const bool ok=sampleModelMotion49Q(motion,bindings,fraction,{nodes.data(),count},state,workspace,d);
 need(allocations==before,"native allocated");need(ok,"valid native sample rejected");need(d.error==ModelMotionError49Q::none,"success diagnostic");
 m.g[4][0]=model;m.g[5][0]=record;m.g[6][0]=0;m.g[7][0]=0;m.f[12]=fraction;m.run(0x1559f0);
 need(state.model18==m.read(model+0x18),"model18 mismatch");
 for(U i=0;i<count;++i){const U p=pointer(ix(i));N expected;std::memcpy(&expected,&initial[i],sizeof expected);
  for(U j=0;j<16;++j){expected.localC0.v[j]=fp(m.read(p+0xc0+j*4));expected.world100.v[j]=fp(m.read(p+0x100+j*4));}
  expected.sceneFlags190=m.mem[p+0x190];
  if(std::memcmp(&expected,&nodes[i],sizeof(N))){std::ostringstream e;e<<"full graph mismatch case="<<cases<<" node="<<i<<" fraction="<<fraction;for(U j=0;j<16;++j)if(word(expected.localC0.v[j])!=word(nodes[i].localC0.v[j]))e<<" lane="<<j<<" source="<<std::hex<<word(expected.localC0.v[j])<<" native="<<word(nodes[i].localC0.v[j]);throw std::runtime_error(e.str());}++checks;
  need(std::memcmp(&nodes[i].world100,&initial[i].world100,sizeof(BodyMatrix48G))==0,"world cache refreshed");
  need((m.read(p+0x190)&0xffffff00u)==0xa5a5a500u,"source adjacent flag bytes");
  for(U at=0;at<0x200;at+=4){if((at>=0xc0&&at<0x140)||at==0x190)continue;const U want=at==0x180?pointer(initial[i].parent180):at==0x184?pointer(initial[i].firstChild184):at==0x188?pointer(initial[i].nextSibling188):0xa5a5a5a5u;need(m.read(p+at)==want,"source untouched field changed");}
 }
 for(U i=count;i<64;++i)need(std::memcmp(&nodes[i],&initial[i],sizeof(N))==0,"node outside graph changed");
 if(RR_MODEL49Q_TRACE_REQUIRED){U k=0;for(const auto&s:m.stores)if(s.address>=0x600000&&s.address<0x600000+count*0x200&&(s.address-0x600000)%0x200==0x190){need(k<flagCount,"native missing flag store");need(nativeFlags[k++]==std::array<U,3>{(s.address-0x600190)/0x200,s.before&255,s.after&255},"flag store order/value mismatch");}
  need(k==flagCount,"native extra flag store");flagsCompared+=k;need(trackCount==m.trackCalls.size(),"track count mismatch");for(U i=0;i<trackCount;++i){const auto&t=m.trackCalls[i];need(nativeTracks[i]==std::array<U,3>{t[0],t[2],(t[1]-0x600000)/0x200},"track order mismatch");}
 }
 ++cases;instructions+=m.steps;for(auto[pc,n]:m.hits)hits[pc]+=n;for(auto[pc,n]:m.taken)taken[pc]+=n;for(auto[pc,n]:m.notTaken)notTaken[pc]+=n;
}
static void admission(const Asset& asset){
 assets::MotionView49O v{};assets::GeometryDiagnostic49O gd{};need(assets::readMot49O(asset.data.data(),asset.data.size(),v,gd),"reject fixture read");
 std::array<N,64> nodes{};for(auto&n:nodes){n.parent180=n.firstChild184=n.nextSibling188=I::none;for(U j=0;j<16;++j){n.localC0.v[j]=float(j);n.world100.v[j]=-float(j);}}
 ModelMotionWorkspace49Q workspace{};
 auto reject=[&](assets::MotionView49O view,assets::MotionNodeBindings49O bindings,float fraction,U count){auto saved=nodes;ModelMotionState49Q state{0x10203040};ModelMotionDiagnostic49Q d{};flagCount=trackCount=0;const auto before=allocations;++calls;
  const bool ok=sampleModelMotion49Q(view,bindings,fraction,{nodes.data(),count},state,workspace,d);need(!ok,"invalid domain accepted");need(allocations==before,"rejection allocated");need(d.error!=ModelMotionError49Q::none,"rejection diagnostic");need(state.model18==0x10203040,"rejection changed model18");need(std::memcmp(nodes.data(),saved.data(),sizeof nodes)==0,"rejection changed graph");};
 auto map=asset.bindings;reject(v,map,-.01f,64);reject(v,map,1.01f,64);reject(v,map,fp(0x7fc00000),64);reject(v,map,fp(0x7f800000),64);reject(v,map,.5f,0);reject(v,map,.5f,65);
 auto bad=v;bad.data=nullptr;reject(bad,map,.5f,64);bad=v;bad.duration=fp(0x7f800000);reject(bad,map,.5f,64);bad=v;bad.duration*=4;reject(bad,map,.5f,64);bad=v;bad.rotationCount=65;reject(bad,map,.5f,64);
 auto bm=map;bm.count=0;reject(v,bm,.5f,64);bm=map;bm.count=65;reject(v,bm,.5f,64);bm=map;bm.rows[v.position[0].rowIndex].sceneNode=64;reject(v,bm,.5f,64);
 bad=v;bad.position[0].keyCount=1;reject(bad,map,.5f,64);bad=v;bad.position[0].rowIndex=64;reject(bad,map,.5f,64);
 nodes[0].parent180=ix(0);reject(v,map,.5f,64);nodes[0].parent180=I::none;nodes[0].firstChild184=ix(1);reject(v,map,.5f,64);nodes[0].firstChild184=I::none;
 nodes[20].world100.v[7]=fp(0x7fc00001);reject(v,map,.5f,64);nodes[20].world100.v[7]=0;nodes[0].localC0.v[15]=fp(0x7f800000);reject(v,map,.5f,64);nodes[0].localC0.v[15]=0;
 need(std::fesetround(FE_DOWNWARD)==0,"set rounding");reject(v,map,.5f,64);need(std::fesetround(FE_TONEAREST)==0,"restore rounding");
 auto huge=asset.data;for(U k=0;k<v.position[0].keyCount;++k){poke(huge,v.position[0].keyDataOffset+k*40+4,k&1?0xff7fffff:0x7f7fffff);poke(huge,v.position[0].keyDataOffset+k*40+28,0x7f7fffff);}
 assets::MotionView49O hv{};need(assets::readMot49O(huge.data(),huge.size(),hv,gd),"finite large reader");reject(hv,map,.001f,64);
 // Reuse workspace after late failure; empty-channel source still clears model18.
 std::vector<std::uint8_t> empty(28);poke(empty,0,0xfffffffe);poke(empty,4,2);poke(empty,8,word(1));poke(empty,16,1);assets::MotionView49O ev{};need(assets::readMot49O(empty.data(),empty.size(),ev,gd),"empty channels reader");auto saved=nodes;ModelMotionState49Q state{123};ModelMotionDiagnostic49Q d{};const auto before=allocations;need(sampleModelMotion49Q(ev,map,1,{nodes.data(),64},state,workspace,d),"empty channels rejected");need(allocations==before,"empty native allocated");need(state.model18==0,"empty model18 not cleared");need(std::memcmp(nodes.data(),saved.data(),sizeof nodes)==0,"empty graph changed");
}
static void coverage(){if(const char*p=std::getenv("RR_MODEL49Q_COVERAGE")){std::ofstream out(p);need(bool(out),"coverage file");out<<"{\"ordered_store_tracing\":"<<(RR_MODEL49Q_TRACE_REQUIRED?"true":"false")<<",\"cases\":"<<cases<<",\"checks\":"<<checks<<",\"native_calls\":"<<calls<<",\"original_instructions\":"<<instructions<<",\"flag_stores_compared\":"<<flagsCompared<<",\"native_allocations\":0,\"pcs\":[";bool first=true;for(auto[a,z]:blocks)for(U p=a;p<z;p+=4){if(!first)out<<',';first=false;out<<"{\"pc\":"<<p<<",\"hits\":"<<hits[p]<<",\"taken\":"<<taken[p]<<",\"not_taken\":"<<notTaken[p]<<'}';}out<<"]}\n";}}
int main(int argc,char**argv){try{
 const std::string root=argc>1?argv[1]:".";if(argc==4){mutationPc=std::stoul(argv[2],nullptr,16);mutationWord=std::stoul(argv[3],nullptr,16);}const auto old=root+"/evidence/authoredGeometry49O/motion/";const auto dir=root+"/evidence/authoredGeometry49O/asset-reader/fixtures/";
 original::Machine base((old+"fixture.bin").c_str());base.bytes(0x3debc0,(old+"trig/sin-table.bin").c_str());base.bytes(0x3defc0,(old+"trig/cos-table.bin").c_str());base.bytes(0x3e1dd0,(old+"trig/acos-table.bin").c_str());base.stores.clear();
 const char*mot[]={"45-engrt1.mot","50-englt1.mot","54-01_brklt.mot","55-01_brkrt.mot","56-01_forwd.mot","57-01_back.mot"};
 for(U a=0;a<6;++a){Asset asset(dir,mot[a],a<2?(a?"48-englt1.psg":"43-engrt1.psg"):"52-chariot.psg",a<2?(a?"49-englt1.vut":"44-engrt1.vut"):"53-chariot.vut");
  const float f[]={0,.001f,.125f,.25f,.3333333f,.49999997f,.5f,.50000006f,.6666667f,.75f,.99999994f,1};
  for(float fraction:f)for(U variant=0;variant<5;++variant)compare(base,asset,fraction,0x493300+variant*7+a,variant,variant);
  U rng=0x494900+a;for(U n=0;n<64;++n){rng=rng*1664525+1013904223;compare(base,asset,float(rng>>8)/16777216.f,rng,n%5,n%5);}
  //Independent admitted byte perturbations exercise all angle/axis classes.
  for(U n=0;n<32;++n){auto changed=asset.data;assets::MotionView49O v{};assets::GeometryDiagnostic49O d{};need(assets::readMot49O(changed.data(),changed.size(),v,d),"perturb read");for(U t=0;t<v.rotationCount;++t)for(U k=0;k<v.rotation[t].keyCount;++k)for(U b=4;b<8;++b){rng=rng*1664525+1013904223;changed[v.rotation[t].keyDataOffset+k*8+b]=rng>>24;}compare(base,asset,.375f,rng,n%5,n%5,&changed);}
  // Valid original-format subsets isolate setter gates masked by earlier rotations.
  {auto positionOnly=asset.data;assets::MotionView49O v{};assets::GeometryDiagnostic49O d{};need(assets::readMot49O(positionOnly.data(),positionOnly.size(),v,d),"position-only seed");
   if(v.rotationCount){positionOnly.resize(v.rotation[0].keyDataOffset-8);poke(positionOnly,24,0);for(U n=0;n<16;++n)compare(base,asset,float(n)/15,0x4900+n,n%5,n%5,&positionOnly);}
   if(v.positionCount){auto signedZero=asset.data;const auto&t=v.position[0];for(U lane=0;lane<3;++lane){poke(signedZero,t.keyDataOffset+(t.keyCount-1)*40+4+lane*4,0x80000000u);poke(signedZero,t.keyDataOffset+(t.keyCount-2)*40+4+lane*4,word(-1));}compare(base,asset,1,191,2,0,&signedZero);}
   std::vector<std::uint8_t> empty(28);poke(empty,0,0xfffffffe);poke(empty,4,2);poke(empty,8,word(1));poke(empty,16,1);compare(base,asset,.5f,99,0,0,&empty);
  }
  if(a==4)admission(asset);
 }
 coverage();std::cout<<"PASS ModelMotion49Q: "<<cases<<" cases, "<<checks<<" checks, "<<calls<<" native calls, "<<instructions<<" original instructions, "<<flagsCompared<<" ordered flag stores, zero native allocations\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FAIL ModelMotion49Q: "<<e.what()<<'\n';return 1;}}
