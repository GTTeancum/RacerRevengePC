// Genuine initialized original122350 comparison, transactional guards and
// bounded serial measurements. Test-only allocations occur outside the call.
#include "candidate_collection_fixture50bx.h"
#include <cfenv>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <new>
#include <type_traits>
#if defined(__linux__)
#include <sys/mman.h>
#include <unistd.h>
#endif
#if defined(_MSC_VER)
#define RR_NOINLINE __declspec(noinline)
#else
#define RR_NOINLINE __attribute__((noinline))
#endif
using namespace collection_test50bx;
static bool watching=false;
static std::size_t allocationKinds[4]{};
static void* allocate(std::size_t n,unsigned kind,std::size_t alignment=0){if(watching)++allocationKinds[kind];void* p=alignment?std::aligned_alloc(alignment,((n?n:1)+alignment-1)/alignment*alignment):std::malloc(n?n:1);if(p)return p;throw std::bad_alloc();}
RR_NOINLINE void* operator new(std::size_t n){return allocate(n,0);}
RR_NOINLINE void* operator new[](std::size_t n){return allocate(n,1);}
RR_NOINLINE void* operator new(std::size_t n,std::align_val_t a){return allocate(n,2,static_cast<std::size_t>(a));}
RR_NOINLINE void* operator new[](std::size_t n,std::align_val_t a){return allocate(n,3,static_cast<std::size_t>(a));}
RR_NOINLINE void operator delete(void* p)noexcept{std::free(p);}
RR_NOINLINE void operator delete[](void* p)noexcept{std::free(p);}
RR_NOINLINE void operator delete(void* p,std::size_t)noexcept{std::free(p);}
RR_NOINLINE void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
RR_NOINLINE void operator delete(void* p,std::align_val_t)noexcept{std::free(p);}
RR_NOINLINE void operator delete[](void* p,std::align_val_t)noexcept{std::free(p);}
RR_NOINLINE void operator delete(void* p,std::size_t,std::align_val_t)noexcept{std::free(p);}
RR_NOINLINE void operator delete[](void* p,std::size_t,std::align_val_t)noexcept{std::free(p);}
namespace {
using Status=CandidateCollectionStatus50BX;
std::size_t checks=0,failures=0,differential=0,boundaries=0,safety=0,overlapChecks=0,lazyChecks=0;
std::string label;
void check(bool ok,const char* why){++checks;if(!ok){++failures;if(failures<70)std::cerr<<label<<": "<<why<<'\n';}}
std::size_t allocations(){std::size_t n=0;for(auto v:allocationKinds)n+=v;return n;}
Status call(Fixture& f){const auto before=allocations();watching=true;const auto s=collectCandidatesBounded50BX(f.in,f.initial,f.work,f.destination,*f.output);watching=false;check(allocations()==before,"no ordinary/array/aligned allocation inside call");return s;}
const char* statusName(Status s){switch(s){
#define S(x) case Status::x:return #x;
S(Complete) S(InvalidInput) S(InsufficientCapacity) S(NonFinite) S(UnsupportedBounds) S(UnsupportedMethod) S(MissingMethod) S(AllocatorBoundary) S(UnsupportedMemberShape) S(InvalidTopology)
#undef S
}return "invalid-status";}
Status status(const std::string& s){for(unsigned k=0;k<=static_cast<unsigned>(Status::InvalidTopology);++k)if(s==statusName(static_cast<Status>(k)))return static_cast<Status>(k);throw std::runtime_error("unknown fixture status");}
struct Snapshot {
 const void* ptr;std::vector<unsigned char> bytes;
 Snapshot(const void* p,std::size_t n):ptr(p),bytes(n){if(n)std::memcpy(bytes.data(),p,n);}
 void verify()const{check(bytes.empty()||!std::memcmp(ptr,bytes.data(),bytes.size()),"entire source/destination/result byte span preserved");}
};
template<class T>void snap(std::vector<Snapshot>& s,const std::vector<T>& a){s.emplace_back(a.data(),a.size()*sizeof(T));}
void snapStorage(std::vector<Snapshot>& s,const Storage& a){snap(s,a.objects);snap(s,a.trees);snap(s,a.pools);snap(s,a.blocks);snap(s,a.entries);snap(s,a.ids);}
std::vector<Snapshot> snapshots(const Fixture& f,bool destination){std::vector<Snapshot> s;
 s.emplace_back(&f.in,sizeof(f.in));s.emplace_back(&f.initial,sizeof(f.initial));s.emplace_back(&f.work,sizeof(f.work));s.emplace_back(&f.destination,sizeof(f.destination));
 snap(s,f.objects);snap(s,f.groups);snap(s,f.nodes);snap(s,f.itemBlocks);snap(s,f.trees);snap(s,f.zones);snap(s,f.zoneLinks);snap(s,f.pools);snap(s,f.poolBlocks);snap(s,f.endpoints);snap(s,f.endpointScenes);snap(s,f.scenes);
 for(const auto& a:f.lists){for(const auto& v:a){snap(s,v);}}for(const auto& a:f.members){snap(s,a);}snapStorage(s,f.source);
 if(destination){snapStorage(s,f.final);s.emplace_back(&f.result,sizeof(f.result));}return s;}
void reject(Fixture& f,Status wanted){const auto s=snapshots(f,true);const auto got=call(f);if(got!=wanted&&failures<70)std::cerr<<label<<": wanted "<<statusName(wanted)<<", got "<<statusName(got)<<'\n';check(got==wanted,"expected transactional rejection status");for(const auto& x:s)x.verify();++safety;}
template<class T>void stateEqual(const std::vector<T>& actual,const std::vector<T>& expected,std::size_t n){for(std::size_t k=0;k<n;++k)check(!std::memcmp(&actual[k],&expected[k],sizeof(T)),"bitwise complete represented state including preserved padding");}
template<class T>void tail(const std::vector<T>& a,std::size_t from,unsigned char canary){const auto* bytes=reinterpret_cast<const unsigned char*>(a.data());for(std::size_t k=from*sizeof(T);k<a.size()*sizeof(T);++k)check(bytes[k]==canary,"full inactive state capacity tail preserved");}
void compare(Fixture& f,const Json& j){const auto& e=j.at("expected");Storage expected=f.source;f.loadState(expected,e);
 check(f.result.count==si(e.at("count")),"source executed count");check(f.result.currentIdentity==e.at("query").at("current").get<std::uint32_t>(),"current identity differs from group output identity");check(f.result.activeTree==f.tree(e.at("last_tree")),"last active tree state");
 stateEqual(f.final.objects,expected.objects,f.in.objectCount);stateEqual(f.final.trees,expected.trees,f.in.treeCount);stateEqual(f.final.pools,expected.pools,f.in.poolCount);stateEqual(f.final.blocks,expected.blocks,f.in.poolBlockCount);stateEqual(f.final.entries,expected.entries,f.in.poolEntryCount);
 stateEqual(f.scratch.objects,expected.objects,f.in.objectCount);stateEqual(f.scratch.trees,expected.trees,f.in.treeCount);stateEqual(f.scratch.pools,expected.pools,f.in.poolCount);stateEqual(f.scratch.blocks,expected.blocks,f.in.poolBlockCount);stateEqual(f.scratch.entries,expected.entries,f.in.poolEntryCount);
 for(std::size_t k=0;k<e.at("output").size();++k)check(f.final.ids[k]==e.at("output")[k].get<std::uint32_t>(),"all source output bits, original order and inactive output tail");
 for(std::size_t k=0;k<f.scratch.ids.size();++k)check(f.scratch.ids[k]==(k<static_cast<std::size_t>(f.result.count)?f.final.ids[k]:0x6d6d6d6du),"scratch output reached prefix and full remaining tail");
 for(std::size_t k=e.at("output").size();k<f.final.ids.size();++k)check(f.final.ids[k]==0xa5a5a5a5u,"extra physical output tail untouched");
 for(const auto* storage:{&f.scratch,&f.final}){const auto canary=storage==&f.scratch?0x6d:0xa5;tail(storage->objects,f.in.objectCount,canary);tail(storage->trees,f.in.treeCount,canary);tail(storage->pools,f.in.poolCount,canary);tail(storage->blocks,f.in.poolBlockCount,canary);tail(storage->entries,f.in.poolEntryCount,canary);}
 // Rebuild raw shape snapshots only by projecting the five represented values.
 // All sixty-four words are checked; nonprojected bytes remain original input.
 for(std::size_t k=0;k<j.at("candidates").size();++k){const auto& c=j.at("candidates")[k];const auto& before=j.at("initial_state").at("candidates")[k].at("shape_words");if(before.is_null())continue;auto words=before.get<std::vector<std::uint32_t>>();const auto r=f.object(c.at("id"));const auto& st=f.final.objects[r-1];const auto tag=f.objects[r-1].shapeTag;if(tag==4||tag==5){const unsigned p=tag==4?12:16,q=tag==4?26:22;for(unsigned h=0;h<3;++h)words[p+h]=bits(st.memberTranslation.v[h]);words[q]=bits(st.memberScale18C);words[q+1]=bits(st.memberScale180);}check(words==e.at("candidates")[k].at("shape_words").get<std::vector<std::uint32_t>>(),"complete64-word original member payload projection");}
}
struct Slot {std::string name;void* pointer;std::size_t count,size,alignment;std::function<void(void*,std::size_t)> set;};
template<class T>void writable(std::vector<Slot>& v,const std::string& name,T*& p,std::size_t& n){v.push_back({name,p,n,sizeof(T),alignof(T),[&p,&n](void* q,std::size_t c){p=static_cast<T*>(q);n=c;}});}
std::vector<Slot> writes(Fixture& f){std::vector<Slot> v;for(unsigned k=0;k<2;++k){auto& b=k?f.destination:f.work.buffers;const std::string p=k?"destination.":"scratch.";writable(v,p+"objects",b.objects,b.objectCapacity);writable(v,p+"trees",b.trees,b.treeCapacity);writable(v,p+"pools",b.pools,b.poolCapacity);writable(v,p+"blocks",b.poolBlocks,b.poolBlockCapacity);writable(v,p+"entries",b.poolEntries,b.poolEntryCapacity);writable(v,p+"identities",b.identities,b.identityCapacity);}writable(v,"stack",f.work.stack,f.work.stackCapacity);v.push_back({"result",f.output,1,sizeof(*f.output),alignof(CandidateCollectionResult50BX),[&f](void* q,std::size_t){f.output=static_cast<CandidateCollectionResult50BX*>(q);}});return v;}
template<class T>void readable(std::vector<Slot>& v,const std::string& name,const T*& p,std::size_t& n){v.push_back({name,const_cast<T*>(p),n,sizeof(T),alignof(T),[&p,&n](void* q,std::size_t c){p=static_cast<const T*>(q);n=c;}});}
std::vector<Slot> reads(Fixture& f){std::vector<Slot> v;
#define READ(n,p,c) readable(v,n,p,c)
 READ("objects",f.in.objects,f.in.objectCount);READ("groups",f.in.groups,f.in.groupCount);READ("nodes",f.in.nodes,f.in.nodeCount);READ("itemBlocks",f.in.itemBlocks,f.in.itemBlockCount);READ("trees",f.in.trees,f.in.treeCount);READ("zones",f.in.zones,f.in.zoneCount);READ("zoneLinks",f.in.zoneLinks,f.in.zoneLinkCount);READ("pools",f.in.pools,f.in.poolCount);READ("poolBlocks",f.in.poolBlocks,f.in.poolBlockCount);
 READ("initial.objects",f.initial.objects,f.initial.objectCapacity);READ("initial.trees",f.initial.trees,f.initial.treeCapacity);READ("initial.pools",f.initial.pools,f.initial.poolCapacity);READ("initial.blocks",f.initial.poolBlocks,f.initial.poolBlockCapacity);READ("initial.entries",f.initial.poolEntries,f.initial.poolEntryCapacity);
 READ("bounds.endpoints",f.in.bounds.endpoints,f.in.bounds.endpointCount);READ("bounds.metadata",f.in.bounds.endpointScenes,f.in.bounds.endpointSceneCount);READ("bounds.scenes",f.in.bounds.scenes,f.in.bounds.sceneCount);
 for(std::size_t k=0;k<f.objects.size();++k){auto& x=f.objects[k].filter;for(unsigned h=0;h<3;++h){auto& l=h==2?x.categories:x.identities[h];READ("filter"+std::to_string(k)+"."+std::to_string(h),l.values,l.capacity);}}
 for(std::size_t k=0;k<f.groups.size();++k)READ("members"+std::to_string(k),f.groups[k].members,f.groups[k].memberCapacity);
#undef READ
 return v;}
void guards(const Json& base,const Json& fresh){
 Fixture model(base);const auto nw=writes(model).size(),nr=reads(model).size();
 // Every pair of full declared mutable spans, including scratch vs destination,
 // each destination table, traversal stack and caller result.
 for(std::size_t a=0;a<nw;++a)for(std::size_t b=0;b<nw;++b)if(a!=b){Fixture f(base);auto w=writes(f);label="write overlap "+w[a].name+"/"+w[b].name;w[a].set(w[b].pointer,1);reject(f,Status::InvalidInput);++overlapChecks;}
 // Every mutable span against every actual read-only table/nested span. Use the
 // far end when large enough to establish overlaps solely in declared tails.
 for(std::size_t a=0;a<nw;++a)for(std::size_t b=0;b<nr;++b){Fixture f(base);auto w=writes(f);auto r=reads(f);if(!r[b].count)continue;const auto bytes=r[b].count*r[b].size;std::size_t off=bytes>=w[a].size?bytes-w[a].size:0;off-=off%w[a].alignment;label="read overlap "+w[a].name+"/"+r[b].name;w[a].set(static_cast<unsigned char*>(r[b].pointer)+off,1);reject(f,Status::InvalidInput);++overlapChecks;}
 for(std::size_t a=0;a<nw;++a)for(unsigned d=0;d<4;++d){Fixture f(base);auto w=writes(f);void* p=d==0?static_cast<void*>(&f.in):d==1?static_cast<void*>(&f.initial):d==2?static_cast<void*>(&f.work):static_cast<void*>(&f.destination);label="descriptor overlap "+w[a].name+"/"+std::to_string(d);w[a].set(p,1);reject(f,Status::InvalidInput);++overlapChecks;}
 // Result is an actual C++ reference; never create a null or misaligned result
 // reference merely to test a check after undefined reference construction.
 for(std::size_t a=0;a+1<nw;++a)for(unsigned mode=0;mode<4;++mode){Fixture f(base);auto w=writes(f);auto& x=w[a];label="write range "+x.name+"/"+std::to_string(mode);if(mode==0)x.set(nullptr,1);if(mode==1)x.set(static_cast<unsigned char*>(x.pointer)+1,1);if(mode==2)x.set(x.pointer,std::numeric_limits<std::size_t>::max());if(mode==3)x.set(reinterpret_cast<void*>(std::numeric_limits<std::uintptr_t>::max()&~(x.alignment-1)),2);reject(f,Status::InvalidInput);}
 for(std::size_t a=0;a<nr;++a)for(unsigned mode=0;mode<4;++mode){Fixture f(base);auto r=reads(f);auto& x=r[a];label="read range "+x.name+"/"+std::to_string(mode);if(mode==0)x.set(nullptr,1);if(mode==1)x.set(static_cast<unsigned char*>(x.pointer)+1,1);if(mode==2)x.set(x.pointer,std::numeric_limits<std::size_t>::max());if(mode==3)x.set(reinterpret_cast<void*>(std::numeric_limits<std::uintptr_t>::max()&~(x.alignment-1)),2);reject(f,Status::InvalidInput);}
 auto test=[&](const char* name,auto change,Status s=Status::InvalidTopology,const Json* row=nullptr){label=name;Fixture f(row?*row:base);change(f);reject(f,s);};
 for(unsigned a=0;a<5;++a)for(unsigned set=0;set<3;++set){label="short mutable/source state "+std::to_string(set)+"/"+std::to_string(a);Fixture f(base);if(set==2){auto r=reads(f);r[9+a].set(r[9+a].pointer,0);}else{auto w=writes(f);w[set*6+a].set(w[set*6+a].pointer,0);}reject(f,Status::InsufficientCapacity);}
 test("query missing",[](Fixture& f){f.in.query=0;});test("query range",[](Fixture& f){f.in.query=0xffffffffu;});test("explicit zone head missing",[](Fixture& f){f.in.zoneHead=0;});test("zone ref range",[](Fixture& f){f.zoneLinks[0].zone=0xffffffffu;});test("method ref range",[](Fixture& f){f.zones[0].firstMethod=0xffffffffu;});
 test("missing selected method",[](Fixture& f){f.in.methodMask=0;},Status::MissingMethod);test("unknown selected method",[](Fixture& f){f.trees[0].method=CollectionTree50BX::Method::Unknown;},Status::UnsupportedMethod);
 test("pool entry count cannot fit a reference",[](Fixture& f){f.in.poolEntryCount=std::numeric_limits<std::size_t>::max();},Status::InvalidInput);
 test("null object identity",[](Fixture& f){f.objects[1].identity=0;},Status::InvalidInput);
 test("distinct pool chunk entry ranges identical",[](Fixture& f){f.poolBlocks.push_back(f.poolBlocks[0]);f.in.poolBlocks=f.poolBlocks.data();f.in.poolBlockCount=f.poolBlocks.size();},Status::InvalidInput);
 test("distinct pool chunk entry range tails overlap",[](Fixture& f){auto b=f.poolBlocks[0];++b.firstEntry;--b.entryCount;f.poolBlocks.push_back(b);f.in.poolBlocks=f.poolBlocks.data();f.in.poolBlockCount=f.poolBlocks.size();},Status::InvalidInput);
 test("duplicate output identity distinct objects",[](Fixture& f){f.objects[1].identity=f.objects[0].identity;},Status::InvalidInput);
 test("group descriptor out of range",[](Fixture& f){f.objects[f.object(101)-1].group=0xffffffffu;});
 test("fresh root ref range",[](Fixture& f){f.trees[0].root=0xffffffffu;},Status::InvalidTopology,&fresh);
 test("next item reference late failure",[](Fixture& f){f.itemBlocks[0].objects[1]=0xffffffffu;});test("item block cycle",[](Fixture& f){f.itemBlocks[0].next=1;});test("pool entry cycle",[](Fixture& f){f.source.entries[0].next=1;});test("pool link node range",[](Fixture& f){f.source.entries[0].node=0xffffffffu;});test("slot negative",[](Fixture& f){f.trees[0].originX=1000;f.source.trees[0].currentHeadA8=1;f.source.trees[0].slotAC=-1;});
 test("zone link cycle",[](Fixture& f){f.zoneLinks[0].next=1;});test("fresh node cycle",[](Fixture& f){f.nodes[0].children[0]=1;},Status::InvalidTopology,&fresh);test("fresh node range",[](Fixture& f){f.nodes[0].children[0]=0xffffffffu;},Status::InvalidTopology,&fresh);test("fresh stack shortage",[](Fixture& f){f.work.stackCapacity=0;},Status::InsufficientCapacity,&fresh);
 test("fresh pool range",[](Fixture& f){f.trees[0].pool=0xffffffffu;},Status::InvalidTopology,&fresh);test("fresh pool chunk range",[](Fixture& f){f.source.pools[0].activeLast18=0xffffffffu;},Status::InvalidTopology,&fresh);test("fresh entry extent range",[](Fixture& f){f.poolBlocks[0].firstEntry=0xffffffffu;},Status::InvalidInput,&fresh);test("pool free-list selfcycle before early capacity break",[](Fixture& f){f.in.nominalCapacity=0;f.source.pools[0].used20=f.pools[0].perBlockCapacity;f.source.pools[0].freeEntry2C=1;f.source.entries[0].next=1;},Status::InvalidTopology,&fresh);
 test("pool active block cycle",[](Fixture& f){f.source.blocks[0].next=1;},Status::InvalidTopology,&fresh);
 test("pool spare block cycle",[](Fixture& f){f.source.pools[0].activeFirst14=0;f.source.pools[0].activeLast18=0;f.source.pools[0].unusedFirst1C=1;f.source.blocks[0].next=1;},Status::InvalidTopology,&fresh);
 test("pool active/spare same physical chunk",[](Fixture& f){f.source.pools[0].unusedFirst1C=1;},Status::InvalidTopology,&fresh);
 test("pool active last mismatch",[](Fixture& f){f.source.pools[0].activeLast18=0;},Status::InvalidTopology,&fresh);
 test("pool free chain two-cycle",[](Fixture& f){f.source.pools[0].used20=f.pools[0].perBlockCapacity;f.source.pools[0].freeEntry2C=1;f.source.entries[0].next=2;f.source.entries[1].next=1;},Status::InvalidTopology,&fresh);
 test("pool free entry future sequential overlap",[](Fixture& f){f.source.pools[0].freeEntry2C=1;f.source.entries[0].next=0;},Status::InvalidTopology,&fresh);
 test("pool free entry ref range",[](Fixture& f){f.source.pools[0].freeEntry2C=0xffffffffu;},Status::InvalidTopology,&fresh);
 test("pool unused block ref range",[](Fixture& f){f.source.pools[0].unusedFirst1C=0xffffffffu;},Status::InvalidTopology,&fresh);
 test("pool active first ref range",[](Fixture& f){f.source.pools[0].activeFirst14=0xffffffffu;},Status::InvalidTopology,&fresh);
 test("pool negative used",[](Fixture& f){f.source.pools[0].used20=-1;},Status::InvalidTopology,&fresh);
 test("pool negative capacity",[](Fixture& f){f.pools[0].perBlockCapacity=-1;},Status::InvalidTopology,&fresh);
 test("fresh chunk entry count short",[](Fixture& f){f.poolBlocks[0].entryCount=0;},Status::InvalidTopology,&fresh);
 test("late physical scratch output shortage",[](Fixture& f){f.work.buffers.identityCapacity=1;},Status::InsufficientCapacity);test("late physical destination output shortage",[](Fixture& f){f.destination.identityCapacity=1;},Status::InsufficientCapacity);
 test("late group member ref",[](Fixture& f){f.members[0][1]=0xffffffffu;});test("late group member shape",[](Fixture& f){f.objects[f.members[0][1]-1].hasShape=false;},Status::UnsupportedMemberShape);test("group member count backing",[](Fixture& f){f.groups[0].memberCount=100;},Status::InsufficientCapacity);
 test("late group arithmetic nonfinite",[](Fixture& f){f.groups[0].base180=0;},Status::NonFinite);test("group position nonfinite",[](Fixture& f){f.in.groupPositionScale=std::numeric_limits<float>::infinity();},Status::NonFinite);
 test("bounds unsupported",[](Fixture& f){f.in.bounds.shapeTag=7;},Status::UnsupportedBounds);test("bounds nonfinite",[](Fixture& f){f.in.bounds.localCenter.v[0]=std::numeric_limits<float>::quiet_NaN();},Status::NonFinite);test("open nonfinite",[](Fixture& f){f.trees[0].scale=std::numeric_limits<float>::infinity();},Status::NonFinite);test("open invalid quantum",[](Fixture& f){f.trees[0].quantum=0;},Status::InvalidInput);
}
void laziness(const Json& base){
 auto good=[&](const char* name,auto mutate){label=name;Fixture f(base);mutate(f);const auto s=snapshots(f,false);check(call(f)==Status::Complete,"unreached invalid numeric/count data is not consumed");for(const auto& a:s)a.verify();++lazyChecks;};
 good("disabled shape and method laziness",[](Fixture& f){f.source.objects[0].flagsF8=0x20;f.in.bounds.shapeTag=0xffffffffu;f.in.zoneHead=0;f.trees[0].method=CollectionTree50BX::Method::Unknown;});
 good("skipSphere ignores all cached XYZ/radius NaNs",[](Fixture& f){f.in.skipSphere=true;for(auto& s:f.source.objects){for(auto& x:s.cachedCenter.v)x=fp(0x7fc12345);s.cachedRadius=fp(0x7fc56789);}});
 good("unreached later method remains unknown",[](Fixture& f){f.in.nominalCapacity=0;f.zones[0].firstMethod=0xffffffffu;});
 good("zero physical output for nominal zero",[](Fixture& f){f.in.nominalCapacity=0;f.work.buffers.identities=nullptr;f.work.buffers.identityCapacity=0;f.destination.identities=nullptr;f.destination.identityCapacity=0;});
 good("nonpositive identity counts ignore absent backing",[](Fixture& f){for(auto& o:f.objects)for(auto& l:o.filter.identities){l.count=-1;l.values=nullptr;l.capacity=0;}});
 good("candidate flags gate lazy oversized identity count",[](Fixture& f){for(std::size_t k=1;k<f.objects.size();++k){f.source.objects[k].flagsF8=0;f.objects[k].filter.identities[0].count=0x7fffffff;}});
 good("sphere rejection skips filter count",[](Fixture& f){f.in.skipSphere=false;for(std::size_t k=1;k<f.objects.size();++k){f.source.objects[k].cachedCenter.v[0]=999;f.objects[k].filter.identities[0].count=0x7fffffff;}});
 {label="reached query identity backing failure";Fixture f(base);f.objects[0].filter.identities[0].count=0x7fffffff;reject(f,Status::InsufficientCapacity);}
 {label="reached candidate identity backing failure";Fixture f(base);f.objects[1].filter.identities[1].count=0x7fffffff;reject(f,Status::InsufficientCapacity);}
 {label="reached category backing failure";Fixture f(base);f.objects[1].filter.categories.count=0x7fffffff;reject(f,Status::InsufficientCapacity);}
 {label="late sphere numeric failure";Fixture f(base);f.in.skipSphere=false;f.source.objects[2].cachedRadius=fp(0x7fc12345);reject(f,Status::NonFinite);}
 // The result and every mutable table tail must stay intact even when the
 // first object was already appended and the second fails a lazy filter read.
 {label="late second candidate filter failure";Fixture f(base);f.objects[2].filter.identities[0].count=0x7fffffff;reject(f,Status::InsufficientCapacity);}
#if defined(__linux__)
 const auto page=static_cast<std::size_t>(::sysconf(_SC_PAGESIZE));void* protectedPage=::mmap(nullptr,page,PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);if(protectedPage==MAP_FAILED)throw std::runtime_error("protected lazy read mmap failed");
 for(unsigned gate=0;gate<8;++gate){label="protected filter backing lazy gate "+std::to_string(gate);Fixture f(base);f.itemBlocks[0].objects[0]=gate==0?f.in.query:f.object(1);for(unsigned k=1;k<4;++k)f.itemBlocks[0].objects[k]=0;
  auto& q=f.objects[0].filter;auto& c=f.objects[f.object(1)-1].filter;const auto* absent=static_cast<const std::uint32_t*>(protectedPage);
  if(gate==0){q.identities[0]={absent,1,1,1};}
  if(gate==1){f.source.objects[f.object(1)-1].flagsF8=0;c.identities[0]={absent,1,1,1};}
  if(gate==2){c.flags31=0;c.identities[0]={absent,1,1,1};}
  if(gate==3){q.acceptMaskB8=0;c.identities[0]={absent,1,1,1};}
  if(gate==4){q.identities[0]={absent,1,-1,0};c.identities[0]={absent,1,0,0};}
  if(gate==5){q.categories={absent,1,0,1};c.categories={absent,1,-1,1};}
  if(gate==6){f.in.skipSphere=false;f.source.objects[f.object(1)-1].cachedCenter.v[0]=99;c.identities[0]={absent,1,1,1};}
  if(gate==7){q.identities[0]={f.lists[0][0].data(),f.lists[0][0].size(),1,1};f.lists[0][0][0]=1;c.identities[0]={absent,1,1,1};}
  const auto saved=snapshots(f,false);check(call(f)==Status::Complete,"protected never-reached backing not read");for(const auto& x:saved)x.verify();++lazyChecks;
 }
 // Read-only nested filter spans are permitted to alias each other.
 {label="shared readonly filter spans";Fixture f(base);f.objects[1].filter.identities[0]=f.objects[0].filter.identities[0];const auto saved=snapshots(f,false);check(call(f)==Status::Complete,"read-only shared spans accepted");for(const auto& x:saved)x.verify();++lazyChecks;}
 if(::munmap(protectedPage,page))throw std::runtime_error("protected lazy read munmap failed");
#endif
}
std::size_t stateBytes(const Fixture& f){return f.in.objectCount*sizeof(CollectionObjectState50BX)+f.in.treeCount*sizeof(CollectionTreeState50BX)+f.in.poolCount*sizeof(CollectionPoolState50BX)+f.in.poolBlockCount*sizeof(CollectionPoolBlockState50BX)+f.in.poolEntryCount*sizeof(CollectionPoolEntry50BX);}
Json benchmark(const Json& row,unsigned iterations){Fixture f(row);label="serial benchmark "+row.at("name").get<std::string>();const auto before=allocations();const auto started=std::chrono::steady_clock::now();watching=true;for(unsigned k=0;k<iterations;++k)if(collectCandidatesBounded50BX(f.in,f.initial,f.work,f.destination,f.result)!=Status::Complete)++failures;watching=false;const auto elapsed=std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-started).count();check(allocations()==before,"benchmark call-only allocation count");return {{"case",row.at("name")},{"iterations",iterations},{"mean_ns_per_call",elapsed/iterations},{"parallelism",1},{"objects",f.in.objectCount},{"nodes",f.in.nodeCount},{"pool_entries",f.in.poolEntryCount},{"source_state_bytes",stateBytes(f)},{"committed_state_bytes",stateBytes(f)},{"scratch_state_bytes",stateBytes(f)},{"logical_scratch_output_bytes",f.work.buffers.identityCapacity*4},{"declared_traversal_stack_bytes",f.work.stackCapacity*sizeof(CollectionTraversalFrame50BX)},{"declared_stack_frames",f.work.stackCapacity},{"allocation_calls",allocations()-before}};}
}
int main(int argc,char** argv){
 try{
  if(argc<2){std::cerr<<"usage: candidate_collection50bx native-fixtures.json [report.json] [--differential-only]\n";return 2;}
  std::fesetround(FE_TONEAREST);std::ifstream stream(argv[1]);Json packet;stream>>packet;if(packet.at("format")!="initialized-candidate-collection48s-v1")throw std::runtime_error("unexpected fixture format");
  const Json *base=nullptr,*group=nullptr,*fresh=nullptr,*large=nullptr;
  for(const auto& row:packet.at("cases")){
   label=row.at("name");if(label=="basic-mode2")base=&row;if(label=="group-before-direct")group=&row;if(label=="uncached-whole-subtree-order")fresh=&row;if(label=="nominal64-input72")large=&row;
   Fixture f(row);const auto wanted=status(row.at("native_status"));if(wanted!=Status::Complete){reject(f,wanted);++boundaries;continue;}
   const auto saved=snapshots(f,false);const auto got=call(f);if(got!=wanted&&failures<70)std::cerr<<label<<": got "<<statusName(got)<<'\n';check(got==wanted,"genuine-return native complete");if(got==Status::Complete)compare(f,row);for(const auto& s:saved)s.verify();++differential;
  }
  if(!base||!group||!fresh||!large)throw std::runtime_error("required source proof rows absent");
  for(const auto& row:packet.at("cases"))if(row.at("name")=="group-member-is-query"){label="query/member shape canonical alias";Fixture f(row);f.objects[f.in.query-1].shapeTag=5;f.objects[f.in.query-1].hasShape=false;const auto saved=snapshots(f,false);check(call(f)==Status::Complete,"query shape derives from already prepared bounds identity");compare(f,row);for(const auto& x:saved)x.verify();++lazyChecks;}
  for(const auto& row:packet.at("cases"))if(row.at("name")=="50BX-SearchList-0"){
   label="SearchList nested read/write alias";{Fixture f(row);f.searchBlocks[0][0].words=f.destination.identities;reject(f,Status::InvalidInput);}
   label="SearchList terminal descriptor capacity";{Fixture f(row);f.trees[0].searchList.blockCapacity=0;reject(f,Status::InsufficientCapacity);}
   label="SearchList invalid object reference";{Fixture f(row);f.searchWords[0][0][0]=static_cast<std::uint32_t>((f.in.objectCount+1)*4);reject(f,Status::InvalidTopology);}
   label="SearchList malformed live reference tag";{Fixture f(row);f.searchWords[0][0][0]=5;reject(f,Status::InvalidTopology);}
  }
  for(const auto& row:packet.at("cases"))if(row.at("name")=="50BZ-Type2-0"){
   label="type2 resource missing remains explicit unsupported";{Fixture f(row);f.in.type2Resource=nullptr;reject(f,Status::UnsupportedBounds);}
   label="type2 root outside declared resource";{Fixture f(row);f.type2View.root=static_cast<std::uint16_t>(f.type2View.nodeCount);reject(f,Status::InvalidInput);}
   label="type2 nested read/write alias before scratch copy";{Fixture f(row);f.type2View.nodes=reinterpret_cast<const Type2Node49X*>(f.work.buffers.objects);reject(f,Status::InvalidInput);}
  }
  Json performance=Json::array();const bool only=argc>3&&std::string(argv[3])=="--differential-only";
  if(!only){guards(*group,*fresh);laziness(*base);for(const auto* row:{base,group,fresh,large})performance.push_back(benchmark(*row,2000));}
  Json report={{"checks",checks},{"failures",failures},{"fixture_cases",packet.at("cases").size()},{"differential_cases",differential},{"source_boundary_cases",boundaries},{"native_safety_cases",safety},{"exhaustive_declared_span_overlap_cases",overlapChecks},{"lazy_read_cases",lazyChecks},{"observed_allocations",allocations()},{"ordinary_new",allocationKinds[0]},{"array_new",allocationKinds[1]},{"aligned_new",allocationKinds[2]},{"aligned_array_new",allocationKinds[3]},{"benchmarks",performance},{"struct_bytes",{{"object_state",sizeof(CollectionObjectState50BX)},{"tree_state",sizeof(CollectionTreeState50BX)},{"pool_state",sizeof(CollectionPoolState50BX)},{"pool_block_state",sizeof(CollectionPoolBlockState50BX)},{"pool_entry",sizeof(CollectionPoolEntry50BX)},{"traversal_frame",sizeof(CollectionTraversalFrame50BX)},{"result",sizeof(CandidateCollectionResult50BX)}}},{"scope","explicit synthetic worlds; original instruction executions; compact full bitwise state; serial bounded Linux host benchmark, no game FPS claim"}};
  if(argc>2){std::ofstream out(argv[2]);out<<report.dump(2)<<'\n';}std::cout<<report.dump()<<'\n';return failures?1:0;
 }catch(const std::exception& e){watching=false;std::cerr<<label<<": harness error: "<<e.what()<<'\n';return 2;}
}
