// Reconstructed after workspace replacement; requires fresh validation.
#include "static_type2_producer49x.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <new>
#include <string>
#include <limits>
#include <chrono>
using namespace rr::game;
using Json=nlohmann::json;
namespace {bool tracking=false;std::size_t allocations=0,checks=0;}
[[gnu::noinline]] void* operator new(std::size_t n){if(tracking)++allocations;if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void operator delete(void* p) noexcept {std::free(p);}
[[gnu::noinline]] void operator delete[](void* p) noexcept {std::free(p);}
[[gnu::noinline]] void operator delete(void* p,std::size_t) noexcept {std::free(p);}
[[gnu::noinline]] void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
namespace {
void check(bool value,const std::string& name){++checks;if(!value)throw std::runtime_error(name);}
std::uint32_t bits(float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;}
float fp(std::uint32_t u){float f;std::memcpy(&f,&u,4);return f;}
void words(void* p,const Json& row){for(std::size_t i=0;i<row.size();++i){const auto u=row[i].get<std::uint32_t>();std::memcpy(static_cast<char*>(p)+i*4,&u,4);}}
void compareWords(const void* p,const Json& row,const std::string& name){for(std::size_t i=0;i<row.size();++i){std::uint32_t u;std::memcpy(&u,static_cast<const char*>(p)+i*4,4);const auto want=row[i].get<std::uint32_t>();check(u==want,name+" word"+std::to_string(i)+" got"+std::to_string(u)+" want"+std::to_string(want));}}
BodyMatrix48G matrix(const Json& j){BodyMatrix48G m{};for(unsigned i=0;i<16;++i)m.v[i]=j[i].get<float>();return m;}
CollisionSphere48J sphere(const Json& j){return {{{j[0].get<float>(),j[1].get<float>(),j[2].get<float>()}},j[3].get<float>()};}
CollisionFilter48J filter(const Json& j){CollisionFilter48J f;f.identity=j.at("identity");f.categoryA4=j.at("category");f.acceptMaskB8=j.at("accept_mask");f.flagsF8=j.at("flags");f.flags31=j.at("flags31");f.categoryModeD4=j.at("category_mode");f.categoryMasks=j.at("category_masks").get<std::vector<std::uint32_t>>();for(unsigned i=0;i<2;++i){f.identityLists[i].mode=j["identity_lists"][i]["mode"];f.identityLists[i].identities=j["identity_lists"][i]["ids"].get<std::vector<std::uint32_t>>();}return f;}
struct Shape {std::vector<Type2Node49X> nodes;std::vector<Type2Ray49X> rays;Type2ResourceView49X view;};
std::size_t depth(const std::vector<Type2Node49X>& nodes,std::uint16_t tag){if(tag&0x8000)return 0;return 1+std::max(depth(nodes,nodes.at(tag).children[0]),depth(nodes,nodes.at(tag).children[1]));}
Shape shape(const Json& j){Shape s;for(const auto& n:j["nodes"]){Type2Node49X x;x.volume=n["volume"];x.radius=n["radius"];x.center=n["center"].get<std::array<float,3>>();x.half=n["half"].get<std::array<float,3>>();x.children[0]=n["children"][0];x.children[1]=n["children"][1];s.nodes.push_back(x);}s.rays.resize(j["rays"].size());for(std::size_t i=0;i<s.rays.size();++i)words(&s.rays[i],j["rays"][i]);s.view.localMatrix.v[0]=s.view.localMatrix.v[5]=s.view.localMatrix.v[10]=s.view.localMatrix.v[15]=1.f;s.view.nodes=s.nodes.data();s.view.nodeCount=s.nodes.size();s.view.rays=s.rays.data();s.view.rayCount=s.rays.size();s.view.root=j["root"];s.view.maxDepth=depth(s.nodes,s.view.root);return s;}
struct Mesh {rr::assets::CollisionMesh48I mesh;std::vector<float> radii;std::size_t maxDepth=0;};
Mesh mesh(const Json& j){Mesh m;for(unsigned i:{0u,5u,10u,15u})m.mesh.local_matrix[i]=1.f;m.mesh.vertices=j["vertices"].get<std::vector<std::array<float,3>>>();m.mesh.vertex_normals=j["normals"].get<std::vector<std::array<float,3>>>();for(const auto& n:j["nodes"]){rr::assets::CollisionMeshNode48I x;x.volume=n["volume"];x.center=n["center"].get<std::array<float,3>>();x.half=n["half"].get<std::array<float,3>>();for(unsigned k=0;k<2;++k){auto tag=n["children"][k].get<std::uint32_t>();x.children[k]=(tag&0x7fff)|((tag&0x8000)?rr::assets::collision_mesh_leaf48i:0);}m.mesh.nodes.push_back(x);}for(const auto& f:j["faces"]){rr::assets::CollisionMeshFace48I x;x.d=f["d"];x.normal=f["normal"].get<std::array<std::int16_t,3>>();x.vertices=f["vertices"].get<std::array<std::uint16_t,3>>();m.mesh.faces.push_back(x);}m.radii.resize(m.mesh.nodes.size());std::vector<std::uint32_t> topology(m.mesh.nodes.size()+m.mesh.faces.size());check(prepareStaticTargetRadii49X(m.mesh,m.radii.data(),m.radii.size(),{topology.data(),topology.size()},m.maxDepth)==Type2ResourceStatus49X::Complete,"prepare mesh");for(std::size_t i=0;i<m.radii.size();++i)check(bits(m.radii[i])==bits(j["nodes"][i]["radius"].get<float>()),"original target radius");return m;}
std::string unhex(const std::string& hex){std::string out;for(std::size_t i=0;i<hex.size();i+=2)out.push_back(static_cast<char>(std::stoul(hex.substr(i,2),nullptr,16)));return out;}
struct Buffers {
 std::vector<SpringCumulativePair48N> ip,wp,dp;std::vector<Type2PairMetadata49X> im,wm,dm;
 std::vector<SpringRecord48N> ir,wr,dr;std::vector<TerrainCandidateEffects48J> we,de;
 std::vector<Type2WorkItem49X> stack;std::vector<Type2Visit49X> trace;
 Type2StaticInitial49X initial;Type2StaticWorkspace49X workspace;Type2StaticDestination49X destination;
 Buffers(const Json& j,std::size_t candidates,std::size_t stackCount,std::size_t traceCount){
  const auto slots=j["endpoints"].size();ip.resize(slots);wp.resize(slots);dp.resize(slots);im.resize(slots);wm.resize(slots);dm.resize(slots);
  for(std::size_t i=0;i<slots;++i){words(&ip[i].first,j["endpoints"][i]);words(&ip[i].normal,j["normals"][i]);words(&ip[i].surfacePoint,j["surfaces"][i]);im[i].fraction=fp(j["fractions"][i]);im[i].worldConverted=j["transformed"][i];im[i].queryTag=j["pair_tags"][i];}
  ir.resize(j["records"].size());wr.resize(ir.size());dr.resize(ir.size());for(std::size_t i=0;i<ir.size();++i)words(&ir[i],j["records"][i]);
  we.resize(candidates);de.resize(candidates);stack.resize(stackCount);trace.resize(traceCount);
  words(&initial.selected,j["selected"]);initial.selected.fraction=fp(j["selected_fraction"]);const auto flag=j["selected_flags"].get<std::uint32_t>();std::memcpy(&initial.selected.flags,&flag,4);words(&initial.records,j["spring"]);
  initial.pairs=ip.data();initial.metadata=im.data();initial.pairSlots=slots;initial.recordSlots=ir.data();initial.recordCount=ir.size();
  workspace={wp.data(),wp.size(),wm.data(),wm.size(),wr.data(),wr.size(),we.data(),we.size(),stack.data(),stack.size(),trace.data(),trace.size()};
  destination={dp.data(),dp.size(),dm.data(),dm.size(),dr.data(),dr.size(),de.data(),de.size()};
 }
};
void expected(const Json& e,const Type2StaticResult49X& result,const Buffers& b,const std::vector<std::size_t>& order,const std::string& name){
 const auto& p=e.at("pair");compareWords(&result.selected,p["selected"],name+" selected");check(bits(result.selected.fraction)==p["selected_fraction"],name+" fraction");compareWords(&result.selected.flags,Json::array({p["selected_flags"]}),name+" flags");
 check(result.pairCount==p["count"],name+" count");check(result.selected.candidateIdentity==p["selected_ids"][0]&&result.selected.face==p["selected_ids"][1]&&result.selected.queryTag==p["selected_ids"][2],name+" selected ownership");
 for(std::size_t i=0;i<b.dp.size();++i){compareWords(&b.dp[i].first,p["endpoints"][i],name+" endpoints"+std::to_string(i));compareWords(&b.dp[i].normal,p["normals"][i],name+" normal"+std::to_string(i));compareWords(&b.dp[i].surfacePoint,p["surfaces"][i],name+" surface"+std::to_string(i));check(bits(b.dm[i].fraction)==p["fractions"][i],name+" pairfraction");check(b.dm[i].worldConverted==p["transformed"][i],name+" worldflag");check(b.dm[i].queryTag==p["pair_tags"][i],name+" pairtag");check(b.dm[i].candidateIdentity==p["pair_owners"][i][0]&&b.dm[i].face==p["pair_owners"][i][1],name+" pair ownership");}
 compareWords(&result.records,e["spring_state"],name+" recordstate");for(std::size_t i=0;i<b.dr.size();++i)compareWords(&b.dr[i],e["records"][i],name+" record"+std::to_string(i));
 check(result.queryFlagsF8==e["query_flags"],name+" query flags");check(result.currentCandidateIdentity==e["current_candidate"],name+" current");check(result.hasGeometryHit==(e["returned"].get<int>()!=0),name+" hit");
 for(std::size_t i=0;i<order.size();++i){const auto& want=e["candidates"][order[i]];check(b.de[i].identity==want["identity"],name+" effectidentity");check(b.de[i].flagsF8==want["flags"],name+" effectflags");compareWords(b.de[i].copied40.data(),want["copied40"],name+" copied40");compareWords(b.de[i].copied50.data(),want["copied50"],name+" copied50");}
 check(result.traceUsed==e["trace"].size(),name+" trace count");for(std::size_t i=0;i<result.traceUsed;++i){const auto& want=e["trace"][i];const auto& got=b.trace[i];check(got.candidateIdentity==want[0]&&got.queryTag==want[1]&&got.targetTag==want[2],name+" visitorder"+std::to_string(i));if((got.queryTag&0x8000)&&(got.targetTag&0x8000))check((got.outcome==1)==(want[3].get<int>()!=0),name+" leafreturn");}
 if(e.contains("visits")){check(result.traceUsed==e["visits"].size(),name+" source visits");for(std::size_t i=0;i<result.traceUsed;++i)check(b.trace[i].outcome==e["visits"][i][3],name+" split/prune"+std::to_string(i));}
 check(result.leafHits==e["leaf_hits"].size(),name+" leaf hits");check(result.callbacks==e["callback_calls"].size(),name+" callbacks");
}
void negativeChecks(const Type2StaticQuery49X& query,const std::vector<Type2StaticCandidate49X>& candidates,Buffers& b){
 const auto savedP=b.dp;const auto savedM=b.dm;const auto savedR=b.dr;const auto savedE=b.de;
 Type2StaticResult49X original{};original.selected.fraction=-123.f;original.pairCount=98765;
 auto trial=[&](const std::string& name,const Type2StaticQuery49X& q,const Type2StaticInitial49X& initial,const Type2StaticWorkspace49X& w,const Type2StaticDestination49X& d,Type2StaticStatus49X want){
  Type2StaticResult49X result=original;tracking=true;const auto got=produceStaticType2Contacts49X(q,candidates.data(),candidates.size(),initial,w,d,result);tracking=false;
  check(got==want,name+" status");check(allocations==0,name+" allocations");check(std::memcmp(&result,&original,sizeof(result))==0,name+" result atomic");
  check(std::memcmp(b.dp.data(),savedP.data(),b.dp.size()*sizeof(b.dp[0]))==0,name+" pair atomic");check(std::memcmp(b.dm.data(),savedM.data(),b.dm.size()*sizeof(b.dm[0]))==0,name+" metadata atomic");check(std::memcmp(b.dr.data(),savedR.data(),b.dr.size()*sizeof(b.dr[0]))==0,name+" records atomic");check(std::memcmp(b.de.data(),savedE.data(),b.de.size()*sizeof(b.de[0]))==0,name+" effects atomic");
 };
 using S=Type2StaticStatus49X;
 for(unsigned i=0;i<9;++i){auto w=b.workspace;auto d=b.destination;switch(i){case 0:--w.pairCapacity;break;case 1:--w.metadataCapacity;break;case 2:--w.recordCapacity;break;case 3:--w.effectCapacity;break;case 4:--w.stackCapacity;break;case 5:--d.pairCapacity;break;case 6:--d.metadataCapacity;break;case 7:--d.recordCapacity;break;case 8:--d.effectCapacity;break;}trial("capacity"+std::to_string(i),query,b.initial,w,d,S::InsufficientCapacity);}
 {auto w=b.workspace;w.traceCapacity=1;trial("trace exhaustion",query,b.initial,w,b.destination,S::InsufficientCapacity);}
 {auto w=b.workspace;w.pairs=b.dp.data();trial("scratch destination alias",query,b.initial,w,b.destination,S::InvalidInput);}
 {auto w=b.workspace;w.metadata=reinterpret_cast<Type2PairMetadata49X*>(b.wp.data());w.metadataCapacity=1;trial("scratch scratch alias",query,b.initial,w,b.destination,S::InvalidInput);}
 {auto d=b.destination;d.metadata=reinterpret_cast<Type2PairMetadata49X*>(b.dp.data());d.metadataCapacity=1;trial("destination destination alias",query,b.initial,b.workspace,d,S::InvalidInput);}
 {auto w=b.workspace;w.pairs=const_cast<SpringCumulativePair48N*>(b.initial.pairs);trial("scratch input alias",query,b.initial,w,b.destination,S::InvalidInput);}
 {auto w=b.workspace;w.trace=reinterpret_cast<Type2Visit49X*>(const_cast<Type2Ray49X*>(query.resource->rays));w.traceCapacity=1;trial("nested ray alias",query,b.initial,w,b.destination,S::InvalidInput);}
 {auto w=b.workspace;w.trace=reinterpret_cast<Type2Visit49X*>(const_cast<Type2Node49X*>(query.resource->nodes));w.traceCapacity=1;trial("nested node alias",query,b.initial,w,b.destination,S::InvalidInput);}
 {auto w=b.workspace;w.trace=reinterpret_cast<Type2Visit49X*>(const_cast<float*>(candidates[0].nodeRadii));w.traceCapacity=1;trial("nested radii alias",query,b.initial,w,b.destination,S::InvalidInput);}
 {auto w=b.workspace;w.trace=reinterpret_cast<Type2Visit49X*>(const_cast<rr::assets::CollisionMeshNode48I*>(candidates[0].candidate.collision.geometry.mesh->nodes.data()));w.traceCapacity=1;trial("nested target alias",query,b.initial,w,b.destination,S::InvalidInput);}
 {auto w=b.workspace;w.pairs=nullptr;trial("null scratch",query,b.initial,w,b.destination,S::InvalidInput);}
 {auto w=b.workspace;w.pairCapacity=std::numeric_limits<std::size_t>::max();trial("overflow extent",query,b.initial,w,b.destination,S::InvalidInput);}
 {auto w=b.workspace;w.pairs=reinterpret_cast<SpringCumulativePair48N*>(reinterpret_cast<char*>(w.pairs)+1);trial("unaligned scratch",query,b.initial,w,b.destination,S::InvalidInput);}
 {auto q=query;q.worldMatrix.v[0]=std::numeric_limits<float>::infinity();trial("nonfinite matrix",q,b.initial,b.workspace,b.destination,S::InvalidInput);}
 {auto q=query;q.worldMatrix.v[3]=1;trial("nonaffine matrix",q,b.initial,b.workspace,b.destination,S::InvalidInput);}
 {auto q=query;q.resource=nullptr;trial("null resource",q,b.initial,b.workspace,b.destination,S::InvalidInput);}
 {auto q=query;q.dynamicMask=4;q.skipMask=0;trial("dynamic callback",q,b.initial,b.workspace,b.destination,S::UnsupportedScope);}
 {auto initial=b.initial;initial.records.generatedCount84=-1;trial("negative record count",query,initial,b.workspace,b.destination,S::InvalidInput);}
}
}
int main(int argc,char** argv){try{
 check(argc==2,"fixture path required");std::ifstream f(argv[1]);Json corpus;f>>corpus;
 std::vector<Shape> shapes;for(const auto& j:corpus["shapes"])shapes.push_back(shape(j));std::vector<Mesh> meshes;for(const auto& j:corpus["meshes"])meshes.push_back(mesh(j));
 std::size_t maxStack=0,maxPairs=0,totalLeafHits=0,totalVisits=0;double elapsed=0;
 for(const auto& test:corpus["cases"]){
  const auto name=test["name"].get<std::string>();const auto& qj=test["query"];auto& shape=shapes[qj["shape"].get<std::size_t>()];auto qfilter=filter(qj["filter"]);
  Type2StaticQuery49X query;query.resource=&shape.view;query.worldMatrix=matrix(qj["world"]);query.filter=&qfilter;query.sphere=sphere(qj["sphere"]);query.collisionTypeTag=qj["collision_tag"];query.groupTypeTag=qj["group_tag"];query.callbackEnabled=qj["callback"];query.queryScene.value=qj["scene_id"];words(query.copied40.data(),qj["copied40"]);words(query.copied50.data(),qj["copied50"]);query.skipMask=test["masks"][0];query.dynamicMask=test["masks"][1];query.materialMask=test["masks"][2];query.resetRecordsBefore=test["reset_before"];query.initialQueryCurrentIdentity=qj.value("initial_current_candidate",0u);query.initialCurrentCategory=4;
  check(qj["mode"]==0,name+" mode0 corpus");
  const auto order=test["order"].get<std::vector<std::size_t>>();std::vector<std::string> names(test["candidates"].size());for(std::size_t i=0;i<names.size();++i)names[i]=unhex(test["candidates"][i]["name_hex"]);
  std::vector<Type2StaticCandidate49X> candidates;candidates.reserve(order.size());std::size_t stackCount=1;
  for(auto index:order){const auto& cj=test["candidates"][index];auto& m=meshes[cj["mesh"].get<std::size_t>()];candidates.emplace_back();auto& c=candidates.back();c.nodeRadii=m.radii.data();c.nodeRadiusCount=m.radii.size();c.maxDepth=m.maxDepth;c.candidate.hasMaterialScene=cj["has_scene"];c.candidate.sceneName=names[index].data();c.candidate.sceneNameBytes=names[index].size();auto& g=c.candidate.collision;g.geometry={&m.mesh,matrix(cj["world"]),cj["id"].get<std::uint32_t>()};g.filter=filter(cj["filter"]);g.sphere=sphere(cj["sphere"]);g.typeTag=cj["type_tag"];g.hasResponseCallbackE8=cj.value("response_callback",false);words(g.copied40.data(),cj["copied40"]);words(g.copied50.data(),cj["copied50"]);stackCount=std::max(stackCount,1+shape.view.maxDepth+m.maxDepth);}
  Buffers b(test["initial"],candidates.size(),stackCount,std::max<std::size_t>(4096,test["expected"]["trace"].size()*4+64));Type2StaticResult49X result{};
  const auto start=std::chrono::steady_clock::now();tracking=true;const auto status=produceStaticType2Contacts49X(query,candidates.data(),candidates.size(),b.initial,b.workspace,b.destination,result);tracking=false;elapsed+=std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-start).count();
  check(status==Type2StaticStatus49X::Complete,name+" complete status "+std::to_string(static_cast<int>(status)));check(allocations==0,name+" hot allocations");expected(test["expected"],result,b,order,name);
  if(test["reset_after"].get<bool>()){auto after=result.records;resetSpringRecords48N(after);compareWords(&after,test["expected"]["after_reset"]["spring_state"],name+" afterreset");for(std::size_t i=0;i<b.dr.size();++i)compareWords(&b.dr[i],test["expected"]["after_reset"]["records"][i],name+" afterresetrecords");}
  if(name=="two-ray-smoke")negativeChecks(query,candidates,b);
  maxStack=std::max(maxStack,result.stackHighWater);maxPairs=std::max(maxPairs,result.pairCount);totalLeafHits+=result.leafHits;totalVisits+=result.visits;
 }
 std::cout<<"{\"cases\":"<<corpus["cases"].size()<<",\"checks\":"<<checks<<",\"hot_allocations\":"<<allocations<<",\"leaf_hits\":"<<totalLeafHits<<",\"visits\":"<<totalVisits<<",\"maximum_stack_items\":"<<maxStack<<",\"maximum_pairs\":"<<maxPairs<<",\"sizeof_work_item\":"<<sizeof(Type2WorkItem49X)<<",\"sizeof_pair\":"<<sizeof(SpringCumulativePair48N)<<",\"sizeof_pair_metadata\":"<<sizeof(Type2PairMetadata49X)<<",\"aggregate_host_us\":"<<elapsed<<"}\n";return 0;
}catch(const std::exception& e){tracking=false;std::cerr<<e.what()<<'\n';return 1;}}
