// Serial host-only validation scaling. Compile against the frozen48S baseline
// for the recorded48T audit; construction/printing allocations are outside calls.
#include "candidate_collection48s.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <new>
#include <string>
#include <vector>
using namespace rr::game;
static bool watching=false;
static std::size_t allocations=0;
static bool hinted=false;
void* operator new(std::size_t n){if(watching)++allocations;if(auto* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}
void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
void* operator new(std::size_t n,std::align_val_t a){if(watching)++allocations;const auto k=static_cast<std::size_t>(a);if(auto* p=std::aligned_alloc(k,((n?n:1)+k-1)/k*k))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n,std::align_val_t a){return ::operator new(n,a);}
void operator delete(void* p,std::align_val_t)noexcept{std::free(p);}
void operator delete[](void* p,std::align_val_t)noexcept{std::free(p);}
void operator delete(void* p,std::size_t,std::align_val_t)noexcept{std::free(p);}
void operator delete[](void* p,std::size_t,std::align_val_t)noexcept{std::free(p);}
struct State {
 std::vector<CollectionObjectState48S> objects;
 std::vector<CollectionTreeState48S> trees;
 std::vector<CollectionPoolState48S> pools;
 std::vector<CollectionPoolBlockState48S> blocks;
 std::vector<CollectionPoolEntry48S> entries;
 State(std::size_t n,std::size_t b,std::size_t e,bool active):objects(n),trees(active?1:0),pools(active?1:0),blocks(b),entries(e){}
 CandidateCollectionBuffers48S buffers(){return {objects.data(),objects.size(),trees.data(),trees.size(),pools.data(),pools.size(),blocks.data(),blocks.size(),entries.data(),entries.size(),nullptr,0};}
 CandidateCollectionState48S initial(){return {objects.data(),objects.size(),trees.data(),trees.size(),pools.data(),pools.size(),blocks.data(),blocks.size(),entries.data(),entries.size()};}
};
void measure(const char* family,std::size_t scale,bool active,bool manyBlocks){
 const std::size_t n=active?1:(manyBlocks?1:scale),s=active?scale:0,b=active?(manyBlocks?scale:1):(manyBlocks?scale:0),e=active?scale:b;
 std::vector<CollectionObject48S> objects(n);for(std::size_t k=0;k<n;++k)objects[k].identity=static_cast<std::uint32_t>(k+1);
 std::vector<CollectionPoolBlock48S> blocks(b);for(std::size_t k=0;k<b;++k)blocks[k]={static_cast<CollectionRef48S>(manyBlocks?k+1:1),manyBlocks?1:e};
 std::vector<CollectionNode48S> nodes(s);for(std::size_t k=0;k+1<s;++k)nodes[k].children[0]=static_cast<CollectionRef48S>(k+2);
 std::vector<CollectionTraversalFrame48S> stack(s);
 State initial(n,b,e,active),work(n,b,e,active),destination(n,b,e,active);
 initial.objects[0].flagsF8=active?2:0x20;
 CollectionTree48S tree{};tree.root=1;tree.pool=1;tree.extentX=tree.extentZ=32768;
 CollectionZone48S zone{1,0};CollectionZoneLink48S link{0,1};CollectionPool48S pool{static_cast<std::int32_t>(manyBlocks?1:s)};
 if(active){auto& p=initial.pools[0];if(manyBlocks){p.unusedFirst1C=1;p.used20=1;for(std::size_t k=0;k+1<b;++k)initial.blocks[k].next=static_cast<CollectionRef48S>(k+2);}else{p.activeFirst14=p.activeLast18=1;p.used20=0;}}
 CandidateCollectionInput48S in{};in.objects=objects.data();in.objectCount=n;in.poolBlocks=blocks.data();in.poolBlockCount=b;in.poolEntryCount=e;in.query=1;
 if(active){in.nodes=nodes.data();in.nodeCount=s;in.trees=&tree;in.treeCount=1;in.zones=&zone;in.zoneCount=1;in.zoneLinks=&link;in.zoneLinkCount=1;in.zoneHead=1;in.methodMask=1;in.pools=&pool;in.poolCount=1;in.bounds.localCenter={{16384,0,16384,1}};in.bounds.localHalf={{16384,0,16384,1}};for(auto k:{0,5,10,15})in.bounds.shapeWorld.v[k]=1;}
 auto state=initial.initial();CandidateCollectionWorkspace48S scratch{work.buffers(),stack.data(),stack.size()};auto dest=destination.buffers();CandidateCollectionResult48S result{};
 std::size_t hintBytes=0,hintStores=0;
#if defined(RR_VALIDATION_HINTS48T)
 std::vector<CollectionRef48S> identityOrder(n),blockOrder(b);
 for(std::size_t k=0;k<n;++k)identityOrder[k]=static_cast<CollectionRef48S>(k+1);
 for(std::size_t k=0;k<b;++k)blockOrder[k]=static_cast<CollectionRef48S>(k+1);
 if(hinted){scratch.identityOrder=identityOrder.data();scratch.identityOrderCount=n;scratch.blockOrder=blockOrder.data();scratch.blockOrderCount=b;hintBytes=(n+b)*sizeof(CollectionRef48S);hintStores=n+b;}
#endif
 const auto comparisons=active&&manyBlocks?(scale*scale*scale-scale)/6:scale*(scale-1)/2;
 const auto iterations=std::max<std::size_t>(3,std::min<std::size_t>(256,2000000/std::max<std::size_t>(1,comparisons)));
 unsigned failures=0;double samples[5]{};const auto before=allocations;
 for(unsigned sample=0;sample<5;++sample){const auto start=std::chrono::steady_clock::now();watching=true;for(std::size_t k=0;k<iterations;++k){if(collectCandidatesBounded48S(in,state,scratch,dest,result)!=CandidateCollectionStatus48S::Complete)++failures;if(result.count!=0)++failures;}watching=false;samples[sample]=std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-start).count()/iterations;}
 std::sort(samples,samples+5);
 std::cout<<family<<(hinted?"_hinted":"")<<','<<scale<<','<<n<<','<<b<<','<<e<<','<<s<<','<<iterations<<','<<samples[0]<<','<<samples[2]<<','<<samples[4]<<','<<comparisons<<','<<hintBytes<<','<<hintStores<<','<<allocations-before<<','<<failures<<'\n';
 if(failures||allocations!=before)std::exit(1);
}
int main(int argc,char** argv){
#if defined(RR_VALIDATION_HINTS48T)
 if(argc==2&&std::string(argv[1])=="--hints")hinted=true;
#else
 (void)argc;(void)argv;
#endif
 std::cout<<std::fixed<<std::setprecision(1);
 std::cout<<"family,scale,objects,blocks,entries,selected_nodes,iterations_per_sample,min_ns,median_ns,max_ns,baseline_pairwise_comparisons_per_call,hint_storage_bytes,hint_setup_refs_written,allocation_calls,failures\n";
 for(auto n:{1u,64u,256u,1024u,4096u,8192u})measure("disabled_identity",n,false,false);
 for(auto n:{1u,16u,64u,256u,1024u,4096u})measure("disabled_blocks",n,false,true);
 for(auto n:{8u,16u,32u,64u,128u,256u,512u})measure("fresh_single_chunk_chain",n,true,false);
 for(auto n:{8u,16u,32u,64u,128u,256u,512u})measure("fresh_spare_chunk_chain",n,true,true);
}
