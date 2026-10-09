#pragma once
// Test-only relocation of source-executed synthetic worlds. No candidate answer
// is used to build topology: nodes, item blocks and pool lists come from inputs.
#include "candidate_collection48s.h"
#include "json.hpp"
#include <array>
#include <cstring>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
namespace collection_test48s {
using namespace rr::game;
using Json=nlohmann::json;
using Ref=CollectionRef48S;
inline float fp(std::uint32_t w){float f;std::memcpy(&f,&w,4);return f;}
inline std::uint32_t bits(float f){std::uint32_t w;std::memcpy(&w,&f,4);return w;}
inline std::int32_t si(const Json& j){auto w=j.get<std::uint32_t>();std::int32_t v;std::memcpy(&v,&w,4);return v;}
template<class T>inline void raw(const Json& j,T& v){for(std::size_t k=0;k<std::size(v);++k)v[k]=fp(j.at(k).template get<std::uint32_t>());}
template<class T>inline void fill(std::vector<T>& a,unsigned char c){if(!a.empty())std::memset(static_cast<void*>(a.data()),c,a.size()*sizeof(T));}
struct Storage {
 std::vector<CollectionObjectState48S> objects;
 std::vector<CollectionTreeState48S> trees;
 std::vector<CollectionPoolState48S> pools;
 std::vector<CollectionPoolBlockState48S> blocks;
 std::vector<CollectionPoolEntry48S> entries;
 std::vector<std::uint32_t> ids;
 CandidateCollectionBuffers48S buffers(){return {objects.data(),objects.size(),trees.data(),trees.size(),pools.data(),pools.size(),blocks.data(),blocks.size(),entries.data(),entries.size(),ids.data(),ids.size()};}
 CandidateCollectionState48S initial(){return {objects.data(),objects.size(),trees.data(),trees.size(),pools.data(),pools.size(),blocks.data(),blocks.size(),entries.data(),entries.size()};}
 void canary(unsigned char v){fill(objects,v);fill(trees,v);fill(pools,v);fill(blocks,v);fill(entries,v);fill(ids,v);}
};
struct Fixture {
 CandidateCollectionInput48S in{};CandidateCollectionState48S initial{};
 CandidateCollectionWorkspace48S work{};CandidateCollectionBuffers48S destination{};
 CandidateCollectionResult48S result{};CandidateCollectionResult48S* output=&result;
 std::vector<CollectionObject48S> objects;std::vector<CollectionGroup48S> groups;
 std::vector<CollectionNode48S> nodes;std::vector<CollectionItemBlock48S> itemBlocks;
 std::vector<CollectionTree48S> trees;std::vector<CollectionZone48S> zones;
 std::vector<CollectionZoneLink48S> zoneLinks;std::vector<CollectionPool48S> pools;
 std::vector<CollectionPoolBlock48S> poolBlocks;
 std::vector<std::array<std::vector<std::uint32_t>,3>> lists;
 std::vector<std::vector<Ref>> members;
 std::vector<SpringVector4_48N> endpoints;std::vector<std::uint32_t> endpointScenes;
 std::vector<BodyMatrix48G> scenes;std::vector<CollectionTraversalFrame48S> stack;
 Storage source,scratch,final;
 std::map<std::uint32_t,Ref> objectRefs,nodeRefs,blockRefs,entryRefs,chunkRefs,treeRefs;
 std::vector<std::uint32_t> objectIds;
 static Ref mapped(const std::map<std::uint32_t,Ref>& refs,std::uint32_t a){if(!a)return 0;auto it=refs.find(a);if(it==refs.end())throw std::runtime_error("unmapped fixture source address/identity: "+std::to_string(a));return it->second;}
 Ref object(std::uint32_t id)const{return mapped(objectRefs,id);}
 Ref node(std::uint32_t a)const{return mapped(nodeRefs,a);}
 Ref block(std::uint32_t a)const{return mapped(blockRefs,a);}
 Ref entry(std::uint32_t a)const{return mapped(entryRefs,a);}
 Ref chunk(std::uint32_t a)const{return mapped(chunkRefs,a);}
 Ref tree(std::uint32_t a)const{return a==0xfeed0000u?0:mapped(treeRefs,a);}
 void filter(Ref r,const Json& j){
  auto& f=objects[r-1].filter;f.categoryA4=j.at("category");f.acceptMaskB8=j.at("accept_mask");f.flags31=j.at("flags31");
  for(unsigned k=0;k<3;++k){
   auto& values=lists[r-1][k];CollectionWords48S& l=k==2?f.categories:f.identities[k];
   if(k<2){const auto& x=j.at("identity_lists")[k];values=x.at("ids").get<std::vector<std::uint32_t>>();l.mode=si(x.at("mode"));l.count=si(x.value("count",Json(values.size())));}
   else{values=j.at("category_masks").get<std::vector<std::uint32_t>>();l.mode=si(j.at("category_mode"));l.count=si(j.value("category_count",Json(values.size())));}
   // Include inactive backed canaries in every declared nested list span.
   values.resize(values.size()+3,0xe1000000u+k);l.values=values.data();l.capacity=values.size();
  }
 }
 void objectState(CollectionObjectState48S& s,const Json& j,const CollectionObject48S& def)const{
  s.epoch=j.at("epoch");s.flagsF8=j.value("flags",0);if(j.contains("center"))raw(j.at("center"),s.cachedCenter.v);if(j.contains("radius"))s.cachedRadius=fp(j.at("radius"));
  if(j.contains("shape_words")&&!j.at("shape_words").is_null()&&(def.shapeTag==4||def.shapeTag==5)){
   const auto& w=j.at("shape_words");const unsigned p=def.shapeTag==4?12:16,q=def.shapeTag==4?26:22;
   for(unsigned k=0;k<3;++k){s.memberTranslation.v[k]=fp(w[p+k]);}s.memberScale18C=fp(w[q]);s.memberScale180=fp(w[q+1]);
  }
 }
 void loadState(Storage& s,const Json& j)const{
  objectState(s.objects[object(1000)-1],j.at("query"),objects[object(1000)-1]);
  for(const auto& x:j.at("candidates"))objectState(s.objects[object(x.at("id"))-1],x,objects[object(x.at("id"))-1]);
  for(const auto& x:j.at("groups"))s.objects[object(x.at("id"))-1].epoch=x.at("epoch");
  for(std::size_t k=0;k<trees.size();++k){const auto& x=j.at("trees")[k];auto& v=s.trees[k];v.epoch=x.at("epoch");v.flags16=x.at("flags");v.blocked898=si(x.at("blocked"));for(unsigned h=0;h<4;++h)v.cache[h]=si(x.at("cache")[h]);v.auxiliaryB4=x.at("auxiliary");v.savedHeadA4=entry(x.at("head"));v.currentHeadA8=entry(x.at("next"));v.slotAC=si(x.at("slot"));v.currentBlockB0=block(x.at("block"));}
  for(std::size_t k=0;k<pools.size();++k){const auto& x=j.at("pools")[k];auto& v=s.pools[k];v.activeFirst14=chunk(x.at("active"));v.activeLast18=chunk(x.at("current"));v.unusedFirst1C=chunk(x.at("reserve"));v.used20=si(x.at("used"));v.freeEntry2C=entry(x.at("free"));v.bytesUsed34=x.at("bytes");}
  for(const auto& x:j.at("pool_blocks"))s.blocks[chunk(x.at("address"))-1].next=chunk(x.at("next"));
  for(const auto& a:j.at("pool_entries"))for(const auto& x:a)s.entries[entry(x.at("address"))-1]={entry(x.at("next")),node(x.at("node"))};
 }
 explicit Fixture(const Json& j){
  auto add=[&](std::uint32_t id){if(!objectRefs.count(id)){objectIds.push_back(id);objectRefs.emplace(id,static_cast<Ref>(objectIds.size()));}};
  add(1000);for(const auto& x:j.at("candidates"))add(x.at("id"));for(const auto& x:j.at("groups"))add(x.at("id"));
  objects.resize(objectIds.size());lists.resize(objects.size());for(std::size_t k=0;k<objects.size();++k)objects[k].identity=objectIds[k];
  in.query=object(1000);const auto& q=j.at("query");objects[in.query-1].typeTag=j.at("collision_tag");filter(in.query,q.at("filter"));
  for(const auto& x:j.at("candidates")){auto r=object(x.at("id"));auto& v=objects[r-1];v.typeTag=x.at("tag");v.shapeTag=x.value("shape_tag",1u);v.hasShape=x.value("shape_present",true);filter(r,x.at("filter"));}
  groups.resize(j.at("groups").size());members.resize(groups.size());
  for(std::size_t k=0;k<groups.size();++k){const auto& x=j.at("groups")[k];auto& o=objects[object(x.at("id"))-1];o.typeTag=x.value("tag",9u);o.group=static_cast<Ref>(k+1);for(unsigned h=0;h<3;++h)o.position[h]=x.at("position")[h];o.scale10=static_cast<std::int8_t>(x.at("width_byte").get<unsigned>());o.scale11=static_cast<std::int8_t>(x.at("height_byte").get<unsigned>());auto& g=groups[k];for(const auto& id:x.at("members"))members[k].push_back(object(id));g.memberCount=si(x.value("count",Json(members[k].size())));members[k].resize(members[k].size()+3,0);g.members=members[k].data();g.memberCapacity=members[k].size();g.base180=fp(x.at("dimensions")[0]);g.other184=fp(x.at("dimensions")[1]);g.base18C=fp(x.at("dimensions")[2]);g.other194=fp(x.at("dimensions")[3]);}
  const auto& layouts=j.at("spatial_layout");
  for(const auto& x:layouts){treeRefs.emplace(x.at("tree"),static_cast<Ref>(treeRefs.size()+1));for(const auto& n:x.at("nodes")){nodeRefs.emplace(n.at("address"),static_cast<Ref>(nodeRefs.size()+1));for(const auto& b:n.at("blocks"))blockRefs.emplace(b.at("address"),static_cast<Ref>(blockRefs.size()+1));}}
  for(const auto& b:j.at("initial_state").at("pool_blocks"))chunkRefs.emplace(b.at("address"),static_cast<Ref>(chunkRefs.size()+1));
  for(const auto& a:j.at("initial_state").at("pool_entries"))for(const auto& e:a)entryRefs.emplace(e.at("address"),static_cast<Ref>(entryRefs.size()+1));
  nodes.resize(nodeRefs.size());itemBlocks.resize(blockRefs.size());trees.resize(layouts.size());pools.resize(layouts.size());poolBlocks.resize(chunkRefs.size());
  for(std::size_t k=0;k<layouts.size();++k){const auto& x=layouts[k];const auto& t=j.at("trees")[k];auto& v=trees[k];v.pool=static_cast<Ref>(k+1);v.root=x.at("nodes").empty()?0:node(x.at("nodes")[0].at("address"));v.originX=fp(t.value("origin_x",bits(-100.f)));v.originZ=fp(t.value("origin_z",bits(-100.f)));v.extentX=fp(t.value("size_x",bits(65536.f)));v.extentZ=fp(t.value("size_z",bits(65536.f)));v.scale=fp(t.value("scale",bits(1.f)));v.quantum=si(t.value("quantum",Json(1024)));
   pools[k].perBlockCapacity=si(x.at("pool_capacity"));auto& pb=poolBlocks[chunk(x.at("chunk"))-1];pb.entryCount=x.at("pool_capacity");pb.firstEntry=pb.entryCount?entry(x.at("chunk").get<std::uint32_t>()+8):0;
   if(x.contains("extra_chunks"))for(const auto& extra:x.at("extra_chunks")){auto& eb=poolBlocks[chunk(extra.at("address"))-1];eb.entryCount=extra.at("entry_count");eb.firstEntry=eb.entryCount?entry(extra.at("address").get<std::uint32_t>()+8):0;}
   for(const auto& n:x.at("nodes")){auto& nn=nodes[node(n.at("address"))-1];for(std::size_t c=0;c<n.at("children").size();++c)if(!n.at("children")[c].is_null())nn.children[c]=node(x.at("nodes")[n.at("children")[c].get<std::size_t>()].at("address"));nn.firstBlock=n.at("blocks").empty()?0:block(n.at("blocks")[0].at("address"));for(const auto& b:n.at("blocks")){auto& bb=itemBlocks[block(b.at("address"))-1];bb.next=block(b.at("next"));for(unsigned h=0;h<4;++h)bb.objects[h]=object(b.at("objects")[h]);}}
  }
  std::map<std::size_t,Ref> zoneRefs;std::vector<Ref> linkedZones;for(std::size_t k=0;k<j.at("zones").size();++k){const auto sourceZone=j.contains("zone_aliases")?j.at("zone_aliases")[k].get<std::size_t>():k;auto found=zoneRefs.find(sourceZone);if(found==zoneRefs.end()){zoneRefs.emplace(sourceZone,static_cast<Ref>(zoneRefs.size()+1));}linkedZones.push_back(zoneRefs.at(sourceZone));}
  zones.resize(zoneRefs.size());zoneLinks.resize(linkedZones.size());for(std::size_t k=0;k<linkedZones.size();++k){const auto& z=j.at("zones")[k];zones[linkedZones[k]-1]={z[0].is_null()?0:z[0].get<Ref>()+1,z[1].is_null()?0:z[1].get<Ref>()+1};zoneLinks[k]={k+1<linkedZones.size()?static_cast<Ref>(k+2):0,linkedZones[k]};}
  in.zoneHead=zoneLinks.empty()?0:1;in.fallbackZone=j.at("fallback_zone").is_null()?0:linkedZones[j.at("fallback_zone").get<std::size_t>()];in.initialActiveTree=tree(j.at("initial_state").at("last_tree"));in.initialCurrentIdentity=j.at("initial_state").at("query").at("current");in.methodMask=j.at("modes")[0];in.laterMethodDiagnosticsEnabled=j.value("later_method_diagnostics",true);in.skipSphere=j.at("modes")[1].get<std::uint32_t>()!=0;in.ignoreGroups=j.at("modes")[2].get<std::uint32_t>()!=0;in.nominalCapacity=si(j.at("capacity"));in.directTypeTag=j.at("collision_tag");in.groupTypeTag=j.at("group_tag");in.groupPositionScale=fp(j.at("group_scale"));
  auto& bounds=in.bounds;bounds.shapeTag=q.at("shape_tag");raw(q.at("world"),bounds.shapeWorld.v);raw(q.at("local_center"),bounds.localCenter.v);raw(q.at("local_half"),bounds.localHalf.v);raw(q.at("center"),bounds.cachedCenter.v);bounds.cachedRadius=fp(q.at("radius"));bounds.pairCount=si(q.value("pair_count",Json(q.at("pairs").size())));for(const auto& p:q.at("pairs"))for(const auto& e:p){SpringVector4_48N v;raw(e,v.v);endpoints.push_back(v);}for(const auto& m:q.at("scenes")){if(m.is_null())endpointScenes.push_back(0);else{BodyMatrix48G v;raw(m,v.v);scenes.push_back(v);endpointScenes.push_back(static_cast<std::uint32_t>(scenes.size()));}}
  // Even tags1/6 declare valid borrowed endpoint/scene storage for span tests.
  if(scenes.empty()){scenes.resize(1);}bounds.endpoints=endpoints.data();bounds.endpointCount=endpoints.size();bounds.endpointScenes=endpointScenes.data();bounds.endpointSceneCount=endpointScenes.size();bounds.scenes=scenes.data();bounds.sceneCount=scenes.size();
  in.objects=objects.data();in.objectCount=objects.size();in.groups=groups.data();in.groupCount=groups.size();in.nodes=nodes.data();in.nodeCount=nodes.size();in.itemBlocks=itemBlocks.data();in.itemBlockCount=itemBlocks.size();in.trees=trees.data();in.treeCount=trees.size();in.zones=zones.data();in.zoneCount=zones.size();in.zoneLinks=zoneLinks.data();in.zoneLinkCount=zoneLinks.size();in.pools=pools.data();in.poolCount=pools.size();in.poolBlocks=poolBlocks.data();in.poolBlockCount=poolBlocks.size();in.poolEntryCount=entryRefs.size();
  for(auto* s:{&source,&scratch,&final}){s->objects.resize(objects.size()+4);s->trees.resize(trees.size()+4);s->pools.resize(pools.size()+4);s->blocks.resize(poolBlocks.size()+4);s->entries.resize(entryRefs.size()+4);s->ids.resize(j.at("initial_output").size()+4);s->canary(s==&source?0x35:s==&scratch?0x6d:0xa5);}
  // Initialize compact fields not represented for an object to explicit zero;
  // preserve raw padding and all capacity tails as separate canaries.
  for(std::size_t k=0;k<objects.size();++k)source.objects[k]=CollectionObjectState48S{};
  loadState(source,j.at("initial_state"));for(std::size_t k=0;k<j.at("initial_output").size();++k)final.ids[k]=j.at("initial_output")[k];
  initial=source.initial();stack.resize(nodes.size()+4);fill(stack,0xc3);work={scratch.buffers(),stack.data(),stack.size()};destination=final.buffers();std::memset(static_cast<void*>(&result),0xa7,sizeof(result));
 }
 Fixture(const Fixture&)=delete;Fixture& operator=(const Fixture&)=delete;
};
}
