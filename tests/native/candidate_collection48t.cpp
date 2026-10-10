// Additive48T differential/guard suite; the239-case48S suite is unchanged.
#include "candidate_collection_fixture48t.h"
#include <cfenv>
#include <chrono>
#include <fstream>
#include <functional>
#include <limits>
using namespace shape_test48t;
namespace {
using Fixture=collection_test48t::Fixture;
using Storage=collection_test48s::Storage;
using Status=CandidateCollectionStatus48S;
Status call(Fixture& f){const auto before=allocations();watching=true;const auto result=collectCandidatesBounded48S(f.in,f.initial,f.work,f.destination,*f.output);watching=false;check(allocations()==before,"no ordinary/array/aligned allocation in collector");return result;}
void snapStorage(std::vector<Snapshot>& s,const Storage& a){snap(s,a.objects);snap(s,a.trees);snap(s,a.pools);snap(s,a.blocks);snap(s,a.entries);snap(s,a.ids);}
std::vector<Snapshot> snapshots(const Fixture& f,bool destination){std::vector<Snapshot> s;s.emplace_back(&f.in,sizeof(f.in));s.emplace_back(&f.initial,sizeof(f.initial));s.emplace_back(&f.work,sizeof(f.work));s.emplace_back(&f.destination,sizeof(f.destination));snap(s,f.objects);snap(s,f.groups);snap(s,f.nodes);snap(s,f.itemBlocks);snap(s,f.trees);snap(s,f.zones);snap(s,f.zoneLinks);snap(s,f.pools);snap(s,f.poolBlocks);snap(s,f.endpoints);snap(s,f.endpointScenes);snap(s,f.scenes);for(const auto& a:f.lists)for(const auto& v:a)snap(s,v);for(const auto& a:f.members)snap(s,a);snapStorage(s,f.source);snap(s,f.shapeSources);snap(s,f.geometries);snap(s,f.sourceShapes);snap(s,f.identityOrder);snap(s,f.blockOrder);if(destination){snapStorage(s,f.final);snap(s,f.finalShapes);s.emplace_back(&f.result,sizeof(f.result));}return s;}
void reject(Fixture& f,Status wanted){const auto saved=snapshots(f,true);const auto got=call(f);if(got!=wanted&&failures<100)std::cerr<<label<<": wanted status "<<static_cast<unsigned>(wanted)<<", got "<<static_cast<unsigned>(got)<<'\n';check(got==wanted,"expected atomic collector rejection");for(const auto& s:saved)s.verify();++safety;}
template<class T>void equal(const std::vector<T>& got,const std::vector<T>& want,std::size_t n){for(std::size_t i=0;i<n;++i)check(!std::memcmp(&got[i],&want[i],sizeof(T)),"entire represented state and padding match source execution");}
void compare(Fixture& f,const Json& row){const auto& e=row.at("expected");Storage expected=f.source;f.loadState(expected,e);
 check(f.result.count==collection_test48s::si(e.at("count")),"source count including repeated group identities");check(f.result.currentIdentity==e.at("query").at("current").get<std::uint32_t>(),"source current identity");check(f.result.activeTree==f.tree(e.at("last_tree")),"source active tree");
 for(const auto* s:{&f.final,&f.scratch}){equal(s->objects,expected.objects,f.in.objectCount);equal(s->trees,expected.trees,f.in.treeCount);equal(s->pools,expected.pools,f.in.poolCount);equal(s->blocks,expected.blocks,f.in.poolBlockCount);equal(s->entries,expected.entries,f.in.poolEntryCount);const auto canary=s==&f.final?0xa5:0x6d;tail(s->objects,f.in.objectCount,canary);tail(s->trees,f.in.treeCount,canary);tail(s->pools,f.in.poolCount,canary);tail(s->blocks,f.in.poolBlockCount,canary);tail(s->entries,f.in.poolEntryCount,canary);}
 for(std::size_t i=0;i<e.at("output").size();++i)check(f.final.ids[i]==e.at("output")[i].get<std::uint32_t>(),"source output entire backed contents and inactive output tail");
 for(std::size_t i=e.at("output").size();i<f.final.ids.size();++i)check(f.final.ids[i]==0xa5a5a5a5u,"extra destination identity tail unchanged");
 for(std::size_t i=0;i<f.scratch.ids.size();++i)check(f.scratch.ids[i]==(i<static_cast<std::size_t>(f.result.count)?f.final.ids[i]:0x6d6d6d6du),"scratch identities only reached prefix changed");
 for(std::size_t i=0;i<row.at("candidates").size();++i){const auto& c=row.at("candidates")[i];const auto& raw=row.at("initial_state").at("candidates")[i].at("shape_words");if(raw.is_null())continue;auto words=raw.get<std::vector<std::uint32_t>>();const auto r=f.object(c.at("id"));const auto& object=f.objects[r-1];const auto& s=f.final.objects[r-1];if(object.shapeTag==0){check(object.shape0Ref!=0,"explicit shape0 identity binding");const auto ref=object.shape0Ref-1;project(words,f.finalShapes[ref]);check(!std::memcmp(&f.finalShapes[ref],&f.scratchShapes[ref],sizeof(GroupShapeState48T)),"scratch and committed shared shape state agree");}else if(object.shapeTag==4||object.shapeTag==5){const auto p=object.shapeTag==4?12u:16u,q=object.shapeTag==4?26u:22u;for(unsigned k=0;k<3;++k)words[p+k]=bits(s.memberTranslation.v[k]);words[q]=bits(s.memberScale18C);words[q+1]=bits(s.memberScale180);}
  check(words==e.at("candidates")[i].at("shape_words").get<std::vector<std::uint32_t>>(),"all72 original shape words including preserved metadata/W/alias views");if(c.contains("shape_geometry_words"))check(row.at("initial_state").at("candidates")[i].at("shape_geometry_words")==e.at("candidates")[i].at("shape_geometry_words"),"all9 geometry words preserved");
 }
 tail(f.finalShapes,f.in.shape0Count,0xa5);tail(f.scratchShapes,f.in.shape0Count,0x6d);
}
struct Slot {std::string name;void* pointer;std::size_t count,size,alignment;std::function<void(void*,std::size_t)> set;};
template<class T>void writable(std::vector<Slot>& v,const std::string& name,T*& p,std::size_t& n){v.push_back({name,p,n,sizeof(T),alignof(T),[&p,&n](void* q,std::size_t c){p=static_cast<T*>(q);n=c;}});}
template<class T>void readable(std::vector<Slot>& v,const std::string& name,const T*& p,std::size_t& n){v.push_back({name,const_cast<T*>(p),n,sizeof(T),alignof(T),[&p,&n](void* q,std::size_t c){p=static_cast<const T*>(q);n=c;}});}
std::vector<Slot> writes(Fixture& f){std::vector<Slot> v;for(unsigned i=0;i<2;++i){auto& b=i?f.destination:f.work.buffers;const std::string prefix=i?"destination.":"scratch.";writable(v,prefix+"objects",b.objects,b.objectCapacity);writable(v,prefix+"trees",b.trees,b.treeCapacity);writable(v,prefix+"pools",b.pools,b.poolCapacity);writable(v,prefix+"blocks",b.poolBlocks,b.poolBlockCapacity);writable(v,prefix+"entries",b.poolEntries,b.poolEntryCapacity);writable(v,prefix+"identities",b.identities,b.identityCapacity);writable(v,prefix+"shapes",b.shape0States,b.shape0Capacity);}writable(v,"stack",f.work.stack,f.work.stackCapacity);v.push_back({"result",f.output,1,sizeof(*f.output),alignof(CandidateCollectionResult48S),[&f](void* p,std::size_t){f.output=static_cast<CandidateCollectionResult48S*>(p);}});return v;}
std::vector<Slot> reads(Fixture& f){std::vector<Slot> v;
#define R(name,p,n) readable(v,name,p,n)
 R("shape sources",f.in.shape0Sources,f.in.shape0Count);R("initial shapes",f.initial.shape0States,f.initial.shape0Capacity);R("objects",f.in.objects,f.in.objectCount);R("groups",f.in.groups,f.in.groupCount);R("nodes",f.in.nodes,f.in.nodeCount);R("items",f.in.itemBlocks,f.in.itemBlockCount);R("trees",f.in.trees,f.in.treeCount);R("zones",f.in.zones,f.in.zoneCount);R("zone links",f.in.zoneLinks,f.in.zoneLinkCount);R("pools",f.in.pools,f.in.poolCount);R("blocks",f.in.poolBlocks,f.in.poolBlockCount);R("initial objects",f.initial.objects,f.initial.objectCapacity);R("initial trees",f.initial.trees,f.initial.treeCapacity);R("initial pools",f.initial.pools,f.initial.poolCapacity);R("initial blocks",f.initial.poolBlocks,f.initial.poolBlockCapacity);R("initial entries",f.initial.poolEntries,f.initial.poolEntryCapacity);R("endpoints",f.in.bounds.endpoints,f.in.bounds.endpointCount);R("endpoint metadata",f.in.bounds.endpointScenes,f.in.bounds.endpointSceneCount);R("scenes",f.in.bounds.scenes,f.in.bounds.sceneCount);
 for(auto& o:f.objects){for(auto& list:o.filter.identities)R("identity filter",list.values,list.capacity);R("category filter",o.filter.categories.values,o.filter.categories.capacity);}for(auto& g:f.groups)R("members",g.members,g.memberCapacity);
#undef R
 for(std::size_t i=0;i<f.shapeSources.size();++i){auto& p=f.shapeSources[i].geometry;v.push_back({"geometry"+std::to_string(i),const_cast<GroupShapeGeometry48T*>(p),p?1u:0u,sizeof(GroupShapeGeometry48T),alignof(GroupShapeGeometry48T),[&p](void* q,std::size_t){p=static_cast<const GroupShapeGeometry48T*>(q);}});}return v;}
void guards(const Json& base){Fixture model(base);const auto nw=writes(model).size(),nr=reads(model).size();
 // All48T spans against every inherited/new writable or immutable declared span.
 for(std::size_t a=0;a<nw;++a)for(std::size_t b=0;b<nw;++b)if(a!=b&&(a==6||a==13||b==6||b==13)){Fixture f(base);auto w=writes(f);label="new write overlap "+w[a].name+"/"+w[b].name;w[a].set(w[b].pointer,1);reject(f,Status::InvalidInput);++overlaps;}
 for(std::size_t a=0;a<nw;++a)for(std::size_t b=0;b<nr;++b)if(a==6||a==13||b<2||b>=nr-model.shapeSources.size()){Fixture f(base);auto w=writes(f);auto r=reads(f);if(!r[b].count)continue;const auto bytes=r[b].count*r[b].size;std::size_t offset=bytes>w[a].size?bytes-w[a].size:0;offset-=offset%w[a].alignment;label="new read/write overlap "+w[a].name+"/"+r[b].name;w[a].set(static_cast<unsigned char*>(r[b].pointer)+offset,1);reject(f,Status::InvalidInput);++overlaps;}
 for(unsigned a:{6u,13u})for(unsigned d=0;d<4;++d){Fixture f(base);auto w=writes(f);label="shape write/descriptor overlap "+std::to_string(a)+"/"+std::to_string(d);void* p=d==0?static_cast<void*>(&f.in):d==1?static_cast<void*>(&f.initial):d==2?static_cast<void*>(&f.work):static_cast<void*>(&f.destination);w[a].set(p,1);reject(f,Status::InvalidInput);++overlaps;}
 for(unsigned kind=0;kind<2;++kind)for(unsigned a=0;a<2;++a)for(unsigned mode=0;mode<4;++mode){Fixture f(base);auto slots=kind?reads(f):writes(f);auto& s=slots[kind?a:(a?13:6)];label="new declared span range "+s.name+"/"+std::to_string(mode);if(mode==0)s.set(nullptr,1);if(mode==1)s.set(static_cast<unsigned char*>(s.pointer)+1,s.count);if(mode==2)s.set(s.pointer,std::numeric_limits<std::size_t>::max());if(mode==3)s.set(reinterpret_cast<void*>(std::numeric_limits<std::uintptr_t>::max()&~(s.alignment-1)),2);reject(f,Status::InvalidInput);}
 auto test=[&](const char* name,auto mutate,Status status=Status::InvalidInput){label=name;Fixture f(base);mutate(f);reject(f,status);};
 test("short initial shape capacity",[](Fixture& f){f.initial.shape0Capacity=0;},Status::InsufficientCapacity);test("short scratch shape capacity",[](Fixture& f){f.work.buffers.shape0Capacity=0;},Status::InsufficientCapacity);test("short destination shape capacity",[](Fixture& f){f.destination.shape0Capacity=0;},Status::InsufficientCapacity);
 test("late member shape ref zero",[](Fixture& f){f.objects[f.object(2)-1].shape0Ref=0;},Status::UnsupportedMemberShape);test("late member shape ref out of range",[](Fixture& f){f.objects[f.object(2)-1].shape0Ref=0xffffffffu;},Status::InvalidTopology);test("late geometry null",[](Fixture& f){f.shapeSources[1].geometry=nullptr;},Status::UnsupportedMemberShape);test("late shape numeric error",[](Fixture& f){f.shapeSources[1].local40.v[7]=fp(0x7fc12345);},Status::NonFinite);test("late sphere numeric error",[](Fixture& f){f.geometries[1].localHalf.v[2]=fp(0x7f800000);},Status::NonFinite);
 test("geometry unaligned",[](Fixture& f){f.shapeSources[0].geometry=reinterpret_cast<const GroupShapeGeometry48T*>(reinterpret_cast<std::uintptr_t>(&f.geometries[0])+1);});test("geometry wrap",[](Fixture& f){f.shapeSources[0].geometry=reinterpret_cast<const GroupShapeGeometry48T*>(std::numeric_limits<std::uintptr_t>::max()&~(alignof(GroupShapeGeometry48T)-1));});
 test("late physical identity shortage after shape writes",[](Fixture& f){f.destination.identityCapacity=1;},Status::InsufficientCapacity);test("late scratch identity shortage after shape writes",[](Fixture& f){f.work.buffers.identityCapacity=1;},Status::InsufficientCapacity);test("shape0 query bounds still unsupported",[](Fixture& f){f.in.bounds.shapeTag=0;},Status::UnsupportedBounds);
 // Query descriptor metadata cannot override its already prepared bounds shape.
 {label="query member uses bounds shape authority";Fixture f(base);f.members[0][0]=f.in.query;f.objects[f.in.query-1].shapeTag=0;f.objects[f.in.query-1].shape0Ref=0xffffffffu;f.objects[f.in.query-1].hasShape=false;const auto saved=snapshots(f,false);check(call(f)==Status::Complete,"query ignores duplicate member shape metadata");check(!std::memcmp(&f.finalShapes[0],&f.sourceShapes[0],sizeof(GroupShapeState48T)),"unreached first member shape remains initial");for(const auto& s:saved)s.verify();++safety;}
 {label="inactive query shape0 ref cannot alias authoritative nonzero bounds";Fixture f(base);f.objects[f.in.query-1].shapeTag=0;f.objects[f.in.query-1].shape0Ref=1;f.objects[f.in.query-1].hasShape=false;const auto saved=snapshots(f,false);check(call(f)==Status::Complete,"query nonzero bounds tag makes shape0 metadata inactive");compare(f,base);check(!std::memcmp(&f.final.objects[f.in.query-1].cachedCenter,&f.source.objects[f.in.query-1].cachedCenter,sizeof(SpringVector4_48N)),"candidate shared ref never mutates authoritative query sphere");for(const auto& s:saved)s.verify();++safety;}
 for(unsigned gate=0;gate<4;++gate){label="unreached shape numeric laziness "+std::to_string(gate);Fixture f(base);for(auto& source:f.shapeSources)source.local40.v[0]=fp(0x7fc12345);if(gate==0)f.source.objects[f.in.query-1].flagsF8=0;if(gate==1)f.in.ignoreGroups=true;if(gate==2)f.in.nominalCapacity=0;if(gate==3)f.groups[0].memberCount=0;const auto saved=snapshots(f,false);check(call(f)==Status::Complete,"unreached shape arithmetic not consumed");for(const auto& s:saved)s.verify();++safety;}
}
void hintGuards(const Json& base){
 for(unsigned which=0;which<2;++which)for(unsigned fault=0;fault<10;++fault){Fixture f(base);f.hints(3);auto& p=which?f.work.blockOrder:f.work.identityOrder;auto& n=which?f.work.blockOrderCount:f.work.identityOrderCount;auto& a=which?f.blockOrder:f.identityOrder;label="validation hint malformed "+std::to_string(which)+"/"+std::to_string(fault);
  if(fault==0)p=nullptr;
  if(fault==1)p=reinterpret_cast<const CollectionRef48S*>(reinterpret_cast<std::uintptr_t>(p)+1);
  if(fault==2)n=std::numeric_limits<std::size_t>::max();
  if(fault==3)p=reinterpret_cast<const CollectionRef48S*>(std::numeric_limits<std::uintptr_t>::max()&~(alignof(CollectionRef48S)-1));
  if(fault==4)n=0;
  if(fault==5){a.push_back(1);p=a.data();++n;}
  if(fault==6)a[0]=0;
  if(fault==7)a[0]=0xffffffffu;
  if(fault==8){if(a.size()>1)a[1]=a[0];else a[0]=2;}
  if(fault==9){if(a.size()>1)std::swap(a[0],a[1]);else a[0]=2;}
  reject(f,Status::InvalidInput);
 }
 for(unsigned which=0;which<2;++which){Fixture model(base);model.hints(3);for(std::size_t a=0;a<writes(model).size();++a){Fixture f(base);f.hints(3);auto w=writes(f);const auto& refs=which?f.blockOrder:f.identityOrder;label="writable/hint fullspan overlap "+w[a].name+"/"+std::to_string(which);w[a].set(const_cast<CollectionRef48S*>(refs.data()+refs.size()-1),1);reject(f,Status::InvalidInput);++overlaps;}}
 auto test=[&](const char* name,auto mutate){label=name;Fixture f(base);f.hints(3);mutate(f);reject(f,Status::InvalidInput);};
 test("stale identity order after identity zero",[](Fixture& f){f.objects[f.identityOrder[0]-1].identity=0;});
 test("stale identity order after duplicate current identities",[](Fixture& f){f.objects[f.identityOrder[1]-1].identity=f.objects[f.identityOrder[0]-1].identity;});
 test("stale identity order after swapping current identities",[](Fixture& f){std::swap(f.objects[f.identityOrder[0]-1].identity,f.objects[f.identityOrder[1]-1].identity);});
 test("stale block range now out of backing",[](Fixture& f){f.poolBlocks[0].firstEntry=0xffffffffu;});
 // A hint is a checked ordering, not a generation token. A still-valid changed
 // identity must be accepted and read from the current descriptor.
 {label="current identity changed but sorted order still valid";Fixture f(base);f.hints(3);f.objects[f.identityOrder.back()-1].identity+=10000;const auto saved=snapshots(f,false);check(call(f)==Status::Complete,"valid current descriptors accepted without stale trust");for(const auto& s:saved)s.verify();++safety;}
 // Add backed empty block descriptors: same zero key is broken by ref, so an
 // exact sorted permutation still proves coverage and cannot omit empties.
 for(unsigned fault=0;fault<6;++fault){Fixture f(base);f.poolBlocks.resize(4);f.source.blocks.resize(8);f.scratch.blocks.resize(8);f.final.blocks.resize(8);f.in.poolBlocks=f.poolBlocks.data();f.in.poolBlockCount=4;f.initial.poolBlocks=f.source.blocks.data();f.initial.poolBlockCapacity=f.source.blocks.size();f.work.buffers.poolBlocks=f.scratch.blocks.data();f.work.buffers.poolBlockCapacity=f.scratch.blocks.size();f.destination.poolBlocks=f.final.blocks.data();f.destination.poolBlockCapacity=f.final.blocks.size();f.hints(3);label="empty and nonempty block hint permutation "+std::to_string(fault);
  if(fault==1)std::swap(f.blockOrder[0],f.blockOrder[1]);
  if(fault==2)f.blockOrder[1]=f.blockOrder[0];
  if(fault==3)f.poolBlocks[1]=f.poolBlocks[0];
  if(fault==4){f.poolBlocks[1]={2,1};f.hints(3);}
  if(fault==5){f.poolBlocks[1].firstEntry=0xffffffffu;}
  if(fault>0&&fault<5)reject(f,Status::InvalidInput);else{const auto saved=snapshots(f,false);check(call(f)==Status::Complete,"sorted empty blocks accepted, inactive firstEntry not a range");for(const auto& s:saved)s.verify();++safety;}
 }
 for(unsigned route=0;route<3;++route){Fixture f(base);f.source.trees[0].flags16=0;f.trees[0].root=0;
  if(route==0){f.hints(3);f.in.poolBlockCount=0;f.work.blockOrderCount=0;}
  if(route==1){f.poolBlocks.resize(3);for(auto& b:f.poolBlocks)b.entryCount=0;f.in.poolBlocks=f.poolBlocks.data();f.in.poolBlockCount=3;f.hints(3);}
  if(route==2){f.poolBlocks.resize(2);f.poolBlocks[0]={1,16};f.poolBlocks[1]={17,16};f.in.poolBlocks=f.poolBlocks.data();f.in.poolBlockCount=2;f.hints(3);}
  label="valid hint boundary route "+std::to_string(route);const auto saved=snapshots(f,false);check(call(f)==Status::Complete,"zero-table present-empty hint, all-empty blocks or touching intervals accepted");check(f.result.count==0,"empty root consumes no invalid inactive pool metadata");for(const auto& s:saved)s.verify();++safety;
 }
 test("current nested member span aliases destination despite valid hints",[](Fixture& f){f.groups[0].members=reinterpret_cast<const CollectionRef48S*>(f.finalShapes.data()+f.in.shape0Count);});
 test("current nested filter span aliases destination despite valid hints",[](Fixture& f){f.objects[0].filter.identities[0].values=reinterpret_cast<const std::uint32_t*>(f.final.objects.data()+f.in.objectCount);});
}
void poolTailGuards(const Json& base){
 for(unsigned hints=0;hints<2;++hints)for(unsigned route=0;route<11;++route){Fixture f(base);f.poolBlocks.resize(6);f.source.blocks.resize(10);f.scratch.blocks.resize(10);f.final.blocks.resize(10);shape_test48t::fill(f.source.blocks,0x35);shape_test48t::fill(f.scratch.blocks,0x6d);shape_test48t::fill(f.final.blocks,0xa5);for(unsigned i=0;i<6;++i)f.source.blocks[i].next=0;
  f.in.poolBlocks=f.poolBlocks.data();f.in.poolBlockCount=f.poolBlocks.size();f.initial.poolBlocks=f.source.blocks.data();f.initial.poolBlockCapacity=f.source.blocks.size();f.work.buffers.poolBlocks=f.scratch.blocks.data();f.work.buffers.poolBlockCapacity=f.scratch.blocks.size();f.destination.poolBlocks=f.final.blocks.data();f.destination.poolBlockCapacity=f.final.blocks.size();
  auto& p=f.source.pools[0];p.activeFirst14=1;p.activeLast18=2;p.unusedFirst1C=3;p.freeEntry2C=0;p.used20=0;f.source.blocks[0].next=2;f.source.blocks[2].next=4;f.source.trees[0].flags16=1;f.source.trees[0].cache[0]=123;f.trees[0].root=0;
  if(route==1){p.activeFirst14=p.activeLast18=p.unusedFirst1C=0;}
  if(route==2)p.unusedFirst1C=0;
  if(route==3)p.activeFirst14=p.activeLast18=0;
  if(route==4)p.unusedFirst1C=p.activeFirst14;
  if(route==5)f.source.blocks[2].next=2;
  if(route==6)p.activeLast18=1;
  if(route==7)f.source.blocks[1].next=1;
  if(route==8)f.source.blocks[3].next=3;
  if(route==9)f.source.blocks[1].next=0xffffffffu;
  if(route==10)f.source.blocks[3].next=0xffffffffu;
  if(hints)f.hints(3);
  label="validated pool-tail equivalence route "+std::to_string(route)+" hints "+std::to_string(hints);
  if(route>=4)reject(f,Status::InvalidTopology);else{const auto saved=snapshots(f,false);check(call(f)==Status::Complete,"acyclic disjoint pool chains retain accepted reset route");check(f.result.count==0,"empty-root reset emits no objects");check(f.final.pools[0].activeFirst14==0&&f.final.pools[0].activeLast18==0,"reset clears active chain endpoints");check(f.final.pools[0].unusedFirst1C==(route==1?0u:route==3?3u:1u),"reset retains exact active-before-unused order");if(route==0)check(f.final.blocks[1].next==3,"reset concatenates distinct chains");for(const auto& s:saved)s.verify();++safety;}
 }
}
Json benchmark(const Json& row){Fixture f(row);label="serial collector benchmark "+row.at("name").get<std::string>();constexpr unsigned n=2000;const auto start=std::chrono::steady_clock::now();const auto before=allocations();watching=true;for(unsigned i=0;i<n;++i)if(collectCandidatesBounded48S(f.in,f.initial,f.work,f.destination,f.result)!=Status::Complete)++failures;watching=false;check(before==allocations(),"allocation-free collector benchmark");return {{"case",row.at("name")},{"iterations",n},{"mean_ns_per_call",std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-start).count()/n},{"parallelism",1},{"shape_count",f.in.shape0Count},{"shape_source_bytes",f.in.shape0Count*sizeof(GroupShapeSource48T)},{"shape_state_used_bytes_per_buffer",f.in.shape0Count*sizeof(GroupShapeState48T)},{"declared_shape_scratch_bytes",f.work.buffers.shape0Capacity*sizeof(GroupShapeState48T)}};}
}
int main(int argc,char** argv){try{
 if(argc<2){std::cerr<<"usage: candidate_collection48t collector-fixtures.json [report.json] [--differential-only]\n";return 2;}std::fesetround(FE_TONEAREST);std::ifstream stream(argv[1]);Json packet;stream>>packet;if(packet.at("format")!="initialized-candidate-collection48t-v1")throw std::runtime_error("unexpected collector fixture format");
 const Json *base=nullptr,*shared=nullptr;std::size_t boundaries=0;for(const auto& row:packet.at("cases"))for(unsigned hintMode=0;hintMode<4;++hintMode){label=row.at("name");if(label=="group-shape0-complete")base=&row;if(label=="two-groups-shared-shape")shared=&row;Fixture f(row);f.hints(hintMode);if(row.at("native_status")!="Complete"){reject(f,Status::UnsupportedMemberShape);++boundaries;continue;}const auto saved=snapshots(f,false);const auto got=call(f);check(got==Status::Complete,"original complete native Complete");if(got==Status::Complete)compare(f,row);for(const auto& s:saved)s.verify();++differential;}
 if(!base||!shared)throw std::runtime_error("required new group0 fixture rows missing");
 Json perf=Json::array();const bool only=argc>3&&std::string(argv[3])=="--differential-only";if(!only){guards(*base);hintGuards(*base);poolTailGuards(*base);perf.push_back(benchmark(*base));perf.push_back(benchmark(*shared));}auto report=accounting();report["fixture_cases"]=packet.at("cases").size();report["validation_modes"]=4;report["source_boundary_cases"]=boundaries;report["benchmarks"]=perf;report["struct_bytes"]={{"shape_source",sizeof(GroupShapeSource48T)},{"shape_state",sizeof(GroupShapeState48T)},{"geometry",sizeof(GroupShapeGeometry48T)},{"sphere",sizeof(GroupShapeSphere48T)}};report["scope"]="explicit source-executed synthetic worlds; all72 shape words and member sphere/cache projection; serial caller-owned bounded host evidence";if(argc>2){std::ofstream out(argv[2]);out<<report.dump(2)<<'\n';}std::cout<<report.dump()<<'\n';return failures?1:0;
 }catch(const std::exception& e){watching=false;std::cerr<<label<<": harness error: "<<e.what()<<'\n';return 2;}}
