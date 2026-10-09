#include "../../src/game/model_motion49o.h"
#include "../../evidence/authoredGeometry49O/motion/motion_machine.h"
#include <cstdlib>
#include <cfenv>
#include <new>
#include <iostream>
#include <sstream>
#include <limits>
using namespace rr::game;
using motion49o::U;
static std::uint64_t allocations=0,checks=0,cases=0,calls=0,steps=0;
static std::map<U,std::uint64_t> totalHits,totalTaken,totalNotTaken;
[[gnu::noinline]] void* operator new(std::size_t n){void*p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();++allocations;return p;}
[[gnu::noinline]] void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void operator delete(void*p) noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p) noexcept{std::free(p);}
[[gnu::noinline]] void operator delete(void*p,std::size_t) noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p,std::size_t) noexcept{std::free(p);}
void require(bool b,const char* s){++checks;if(!b)throw std::runtime_error(s);}
void compare(const motion49o::Machine& original,rr::assets::MotionNodeBindings49O bindings,float fraction,float blend){
 auto m=original;rr::assets::MotionView49O a{},b{};rr::assets::GeometryDiagnostic49O gd{};
 require(rr::assets::readMot49O(&m.mem[0x510000],2284,a,gd),"first reader");require(rr::assets::readMot49O(&m.mem[0x520000],2284,b,gd),"second reader");
 ModelLocalMatrix49O local[18];for(U i=0;i<18;++i)for(U j=0;j<16;++j)local[i].m[j]=motion49o::fp(m.read(0x504000+i*0x200+0xc0+j*4));
 for(U row=0;row<18;++row)m.write(0x503000+row*64+0x34,0x504000+bindings.rows[row].sceneNode*0x200);
 ModelMotionDiagnostic49O d{};auto before=allocations;++calls;bool ok=sampleModelMotionBlend49O(a,b,bindings,fraction,blend,local,18,d);auto after=allocations;
 require(ok,"native admitted call");require(after==before,"native allocated");require(d.error==ModelMotionError49O::none,"native diagnostic");
 m.g[4][0]=0x500000;m.g[5][0]=0x501000;m.g[6][0]=0x502000;m.f[12]=fraction;m.f[13]=blend;m.run(0x155e70);++cases;steps+=m.steps;for(auto [pc,n]:m.hits)totalHits[pc]+=n;for(auto [pc,n]:m.taken)totalTaken[pc]+=n;for(auto [pc,n]:m.notTaken)totalNotTaken[pc]+=n;
 for(U i=0;i<18;++i)for(U j=0;j<16;++j){++checks;U w=m.read(0x504000+i*0x200+0xc0+j*4),n=motion49o::bits(local[i].m[j]);if(w!=n){std::ostringstream s;s<<"matrix mismatch node="<<i<<" lane="<<j<<" fraction="<<fraction<<" blend="<<blend<<" source="<<std::hex<<w<<" native="<<n;throw std::runtime_error(s.str());}}
}
int main(int argc,char**argv){try{
 const std::string dir=argc>1?argv[1]:"evidence/authoredGeometry49O/motion";
 motion49o::Machine base((dir+"/fixture.bin").c_str());base.bytes(0x3debc0,(dir+"/trig/sin-table.bin").c_str());base.bytes(0x3defc0,(dir+"/trig/cos-table.bin").c_str());base.bytes(0x3e1dd0,(dir+"/trig/acos-table.bin").c_str());
 rr::assets::MotionNodeBindings49O bindings{};bindings.count=18;for(unsigned i=0;i<18;++i)bindings.rows[i].sceneNode=i;
 std::vector<float> fractions={0.f,.001f,.125f,.25f,.3333333f,.49999997f,.5f,.50000006f,.6666667f,.75f,.99999994f,1.f};U random=0x49a55a31;
 for(U i=0;i<200;++i){random=random*1664525+1013904223;fractions.push_back(float(random>>8)/16777216.f);}
 for(float f:fractions)for(float b:{0.f,.125f,.5f,.875f,1.f})compare(base,bindings,f,b);
 // Independent source-byte perturbations cover both quaternion hemispheres
 // and all ByteAxis angle bins; they are tests, not invented authored defaults.
 for(U seed=0;seed<128;++seed){auto varied=base;U rng=seed+0x49f00du;
  for(U motion:{0x510000u,0x520000u})for(U track=0;track<8;++track)for(U key=0;key<21;++key){U at=motion+884+track*176+key*8+4;for(U byte=0;byte<4;++byte){rng=rng*1664525u+1013904223u;varied.mem[at+byte]=static_cast<unsigned char>(rng>>24);}}
  compare(varied,bindings,.5f,float(seed%5)*.25f);
 }
 // Source binding permutations and intentional many-row-to-one-node mapping.
 for(U shift=1;shift<18;++shift){auto perm=bindings;for(U i=0;i<18;++i)perm.rows[i].sceneNode=(i+shift)%18;compare(base,perm,.5f,0.f);}
 auto same=bindings;for(auto& row:same.rows)row.sceneNode=5;compare(base,same,.5f,0.f);
 // Flags are not read on155E70, even when every source flag differs.
 auto flags=bindings;for(auto& row:flags.rows){row.translationFlag38=true;row.listedFlag3c=true;}compare(base,flags,.5f,0.f);
 // Late rejection must not leak already-computed rotations into caller locals.
 rr::assets::MotionView49O first{},second{};rr::assets::GeometryDiagnostic49O gd{};
 require(rr::assets::readMot49O(&base.mem[0x510000],2284,first,gd),"rejection first reader");require(rr::assets::readMot49O(&base.mem[0x520000],2284,second,gd),"rejection second reader");
 ModelLocalMatrix49O original[18],out[18];for(U i=0;i<18;++i)for(U j=0;j<16;++j)original[i].m[j]=motion49o::fp(base.read(0x504000+i*0x200+0xc0+j*4));
 auto reject=[&](rr::assets::MotionView49O a,rr::assets::MotionView49O b,rr::assets::MotionNodeBindings49O map,float f,float w,U count){std::memcpy(out,original,sizeof(out));ModelMotionDiagnostic49O d{};auto before=allocations;++calls;bool ok=sampleModelMotionBlend49O(a,b,map,f,w,out,count,d);auto after=allocations;require(!ok,"rejection accepted");require(d.error!=ModelMotionError49O::none,"rejection diagnostic");require(std::memcmp(out,original,sizeof(out))==0,"rejection changed locals");require(after==before,"rejection allocated");};
 auto bad=second;bad.position[0].rowIndex=2;reject(first,bad,bindings,.5f,0,18);
 bad=second;bad.rotationCount=0;reject(first,bad,bindings,.5f,0,18);
 auto badMap=bindings;badMap.rows[1].sceneNode=18;reject(first,second,badMap,.5f,0,18);
 reject(first,second,bindings,-.1f,0,18);reject(first,second,bindings,1.1f,0,18);reject(first,second,bindings,.5f,1.1f,18);reject(first,second,bindings,.5f,0,0);reject(first,second,bindings,.5f,0,65);
 bad=first;bad.duration=std::numeric_limits<float>::infinity();reject(bad,second,bindings,.5f,0,18);
 bad=first;bad.duration=first.duration*4;reject(bad,second,bindings,.5f,0,18);
 bad=first;bad.position[0].keyCount=1;reject(bad,second,bindings,.5f,0,18);
 // Finite source keys can overflow an intermediate; failure stays atomic.
 auto hugeFirst=base.mem,hugeSecond=base.mem;U plus=0x7f7fffff,minus=0xff7fffff;
 for(U k=0;k<21;++k){std::memcpy(&hugeFirst[0x510000+36+k*40+4],&plus,4);std::memcpy(&hugeSecond[0x520000+36+k*40+4],&minus,4);}
 rr::assets::MotionView49O largeA{},largeB{};
 require(rr::assets::readMot49O(&hugeFirst[0x510000],2284,largeA,gd),"finite large first");require(rr::assets::readMot49O(&hugeSecond[0x520000],2284,largeB,gd),"finite large second");reject(largeA,largeB,bindings,.5f,0,18);
 require(std::fesetround(FE_DOWNWARD)==0,"set rounding");reject(first,second,bindings,.5f,0,18);require(std::fesetround(FE_TONEAREST)==0,"restore rounding");
 std::ofstream coverage(dir+"/coverage.tsv");require(bool(coverage),"coverage output");
 for(auto [a,z]:blocks)for(U pc=a;pc<z;pc+=4)coverage<<std::hex<<pc<<"\t"<<std::dec<<totalHits[pc]<<"\t"<<totalTaken[pc]<<"\t"<<totalNotTaken[pc]<<"\n";
 printf("{\"cases\":%llu,\"checks\":%llu,\"native_calls\":%llu,\"native_allocations\":0,\"original_instructions\":%llu,\"all_match\":true}\n",(unsigned long long)cases,(unsigned long long)checks,(unsigned long long)calls,(unsigned long long)steps);
 return 0;}catch(std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
