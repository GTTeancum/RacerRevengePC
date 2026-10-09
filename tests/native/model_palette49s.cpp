#include <string>
#include "../../src/game/model_palette49s.h"
#include "../../evidence/modelPalette49S/source/palette_oracle49s.h"
#include <algorithm>
#include <array>
#include <cfenv>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <random>
#include <vector>
using namespace rr::game;
namespace original=palette49s_oracle;
using original::U;using original::bits;using original::fp;
using N=SceneNode49P;using M=BodyMatrix48G;using I=SceneNodeIndex49P;
#ifndef RR_PALETTE49S_TRACE_REQUIRED
#define RR_PALETTE49S_TRACE_REQUIRED 0
#endif
static constexpr bool traced=RR_PALETTE49S_TRACE_REQUIRED!=0;
static std::uint64_t checks=0,fixtures=0,operations=0,instructions=0,getterCalls=0,flagStores=0,allocations=0;
static std::array<std::uint64_t,palette49s_words::total_words> hits{},taken{},notTaken{};
static std::array<U,65> nativeGetters{};static unsigned nativeGetterCount=0;
struct Flag{U node;std::uint8_t before,after;};static std::array<Flag,64> nativeFlags{};static unsigned nativeFlagCount=0;
void rrPalette49SGetter(U index) noexcept {if(nativeGetterCount==nativeGetters.size())std::abort();nativeGetters[nativeGetterCount++]=index;}
void rrScene49PFlagStore(U index,std::uint8_t before,std::uint8_t after) noexcept {if(nativeFlagCount==nativeFlags.size())std::abort();nativeFlags[nativeFlagCount++]={index,before,after};}
[[gnu::noinline]] void* operator new(std::size_t n){void*p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();++allocations;return p;}
[[gnu::noinline]] void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void operator delete(void*p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete(void*p,std::size_t)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
static void need(bool v,const char*m){++checks;if(!v)throw std::runtime_error(m);}
static I ix(unsigned i){return static_cast<I>(i);}static U pointer(I i){return i==I::none?0:0x1000+static_cast<U>(i)*0x200;}
static M identity(){M m{};for(unsigned i=0;i<4;++i)m.v[5*i]=1;return m;}
static M randomMatrix(std::mt19937&r){M m{};for(float&v:m.v)v=float(std::int32_t(r()%129)-64)/64.f;return m;}
static U mutationPc=0,mutationWord=0;
struct Fixture {
 std::array<N,64> nodes{},initial{};std::array<I,64> source{};std::array<std::uint8_t,64> ids{};
 ModelPaletteInput49S input{};ModelPalette49S output{};ModelPaletteWorkspace49S workspace{};ModelPaletteDiagnostic49S diagnostic{};
 unsigned count;std::unique_ptr<original::Machine> o=std::make_unique<original::Machine>();
 explicit Fixture(unsigned n):count(n){
  std::memset(nodes.data(),0x5a,sizeof nodes);std::memset(&output,0xa5,sizeof output);
  for(unsigned i=0;i<64;++i){nodes[i].localC0=identity();nodes[i].world100=identity();nodes[i].world100.v[12]=-17.f;nodes[i].parent180=nodes[i].firstChild184=nodes[i].nextSibling188=I::none;nodes[i].sceneFlags190=0xa4;source[i]=ix(i%n);ids[i]=i%n;}
  input.phase=ModelPalettePhase49S::selectedRenderPreparation;input.suppliedState=ModelPaletteSceneState49S::completeCurrentScene;input.modelFlags191=1;input.modelRoot=ix(0);input.sourceNodes=source.data();input.sourceNodeCount=n;input.paletteIds=ids.data();input.paletteCount=n;
 }
 void link(unsigned child,unsigned parent){nodes[child].parent180=ix(parent);auto*p=&nodes[parent].firstChild184;while(*p!=I::none)p=&nodes[static_cast<unsigned>(*p)].nextSibling188;*p=ix(child);}
 void seed(){
  need(validateSceneGraph49P({nodes.data(),count}),"fixture topology");initial=nodes;
  for(unsigned i=0;i<count;++i){U p=pointer(ix(i));for(unsigned j=0;j<16;++j){o->store(p+0xc0+j*4,nodes[i].localC0.v[j]);o->store(p+0x100+j*4,nodes[i].world100.v[j]);}o->write(p+0x180,pointer(nodes[i].parent180));o->write(p+0x184,pointer(nodes[i].firstChild184));o->write(p+0x188,pointer(nodes[i].nextSibling188));o->write(p+0x190,0xa5a5a500u|nodes[i].sceneFlags190);}
  if(mutationPc)o->substitutions[mutationPc]=mutationWord;
  ++fixtures;
 }
 void compare(){
  for(unsigned i=0;i<count;++i){const U p=pointer(ix(i));N expected=initial[i];for(unsigned j=0;j<16;++j){expected.localC0.v[j]=fp(o->read(p+0xc0+4*j));expected.world100.v[j]=fp(o->read(p+0x100+4*j));}expected.sceneFlags190=o->read(p+0x190)&255;
   need(std::memcmp(&nodes[i],&expected,sizeof(N))==0,"full graph matrix/local/cache/flags/links mismatch");
   for(unsigned at=0;at<0x200;at+=4){if((at>=0xc0&&at<0x140)||at==0x190)continue;const U expectedWord=at==0x180?pointer(initial[i].parent180):at==0x184?pointer(initial[i].firstChild184):at==0x188?pointer(initial[i].nextSibling188):at==0x1b8&&ix(i)==input.modelRoot?0x9400:0xa5a5a5a5u;need(o->read(p+at)==expectedWord,"original unrelated scene field mismatch");}
   need((o->read(p+0x190)&0xffffff00u)==0xa5a5a500u,"original adjacent flag byte mismatch");
  }
  for(unsigned i=count;i<64;++i)need(std::memcmp(&nodes[i],&initial[i],sizeof(N))==0,"inactive node mismatch");
  if(traced){need(nativeGetterCount==o->explicitGetters.size(),"getter count mismatch");for(unsigned i=0;i<nativeGetterCount;++i)need(pointer(ix(nativeGetters[i]))==o->explicitGetters[i],"getter order mismatch");
   unsigned fi=0;for(const auto&s:o->stores)if(s.address>=0x1000&&s.address<0x1000+count*0x200&&(s.address-0x1000)%0x200==0x190){need(fi<nativeFlagCount,"flag store count mismatch");const auto&f=nativeFlags[fi++];need(pointer(ix(f.node))+0x190==s.address&&f.before==(s.oldWord&255)&&f.after==(s.newWord&255),"flag store order/value mismatch");}need(fi==nativeFlagCount,"extra flag store mismatch");flagStores+=fi;
  }getterCalls+=o->explicitGetters.size();
 }
 void run(){
  constexpr U table=0x9400,idAddress=0x9500,idPointer=0x9600,countAddress=0x9700,desc=0x9800,packet=0xa000;
  const auto initialOutput=output;const auto initialSource=source;const auto initialIds=ids;const auto initialInput=input;
  for(unsigned i=0;i<input.sourceNodeCount;++i)o->write(table+4*i,pointer(source[i]));
  for(unsigned i=0;i<64;i+=4){U word=0;for(unsigned k=0;k<4;++k)word|=U(ids[i+k])<<(8*k);o->write(idAddress+i,word);}
  o->write(pointer(input.modelRoot)+0x1b8,table);o->write(desc+4,countAddress);o->write(desc+8,idPointer);o->write(countAddress,input.paletteCount);o->write(idPointer,idAddress);
  for(unsigned i=0;i<0x50*(input.paletteCount+1);i+=4)o->write(packet+i,0xa5a5a5a5);
  o->trace.clear();o->stores.clear();o->explicitGetters.clear();nativeGetterCount=nativeFlagCount=0;
  o->g[16][0]=pointer(input.modelRoot);o->runUntil(0x2412e4,0x2412fc);
  o->g[16][0]=pointer(input.modelRoot);o->g[17][0]=0;o->g[23][0]=desc;o->g[21][0]=packet;o->runUntil(0x2414f8,0x2415a4);
  const auto before=allocations;const bool result=projectModelPalette49S({nodes.data(),count},input,output,workspace,diagnostic);
  need(allocations==before,"hot allocation mismatch");need(result&&diagnostic.error==ModelPaletteError49S::none,"valid palette projection rejected");need(output.entryCount==input.paletteCount,"palette count mismatch");
  for(unsigned j=0;j<16;++j)need(bits(output.rootWorld.v[j])==o->read(packet+j*4),"root world mismatch");
  for(unsigned i=0;i<input.paletteCount;++i){for(unsigned j=0;j<16;++j)need(bits(output.nodeWorld[i].v[j])==o->read(packet+0x50*(i+1)+4*j),"palette matrix mismatch");for(unsigned j=0;j<4;++j)need(o->read(packet+0x50*(i+1)+64+4*j)==0xa5a5a5a5,"packet command lane mismatch");}
  for(unsigned i=input.paletteCount;i<64;++i)need(std::memcmp(&output.nodeWorld[i],&initialOutput.nodeWorld[i],sizeof(M))==0,"unused output slot mismatch");
  need(std::memcmp(source.data(),initialSource.data(),sizeof source)==0&&std::memcmp(ids.data(),initialIds.data(),sizeof ids)==0,"input arrays changed");need(std::memcmp(&input,&initialInput,sizeof input)==0,"input descriptor changed");compare();++operations;
 }
 ~Fixture(){instructions+=o->steps;for(unsigned i=0;i<hits.size();++i){hits[i]+=o->hits[i];taken[i]+=o->taken[i];notTaken[i]+=o->notTaken[i];}}
};
static void cases(){
 std::mt19937 rng(490053);
 // All model and source flag-byte combinations, with stale ready caches and
 // independent local matrices. Include source-node table permutation.
 for(unsigned flags=0;flags<256;++flags){Fixture f(6);f.link(1,0);f.link(2,1);f.link(3,1);f.link(4,2);f.link(5,0);
  for(unsigned i=0;i<f.count;++i){f.nodes[i].localC0=randomMatrix(rng);f.nodes[i].world100=randomMatrix(rng);f.nodes[i].sceneFlags190=static_cast<std::uint8_t>((flags+i*31)&255);}std::swap(f.source[1],f.source[4]);f.ids={3,1,5,2,1,0};f.seed();f.run();f.run();
 }
 // Full bounded sizes, deep/wide forests, duplicate IDs, arbitrary source
 // table order and non-parent-first graph storage.
 for(unsigned trial=0;trial<256;++trial){const unsigned n=1+trial%64;Fixture f(n);std::vector<unsigned> order(n);for(unsigned i=0;i<n;++i)order[i]=i;std::shuffle(order.begin(),order.end(),rng);
  for(unsigned i=1;i<n;++i)if(i%11!=0)f.link(order[i],order[trial%3==0?i-1:trial%3==1?0:rng()%i]);
  f.input.modelRoot=ix(order[trial%n]);
  for(unsigned i=0;i<n;++i){f.source[i]=ix(order[(i+1)%n]);f.ids[i]=static_cast<std::uint8_t>(rng()%n);f.nodes[i].localC0=randomMatrix(rng);f.nodes[i].world100=randomMatrix(rng);f.nodes[i].sceneFlags190=rng()&255;}f.seed();f.run();
  // Real persistent local write with source-consistent invalidation supplied
  // identically to both sides; next projection owns only lazy world effects.
  for(unsigned i=0;i<n;++i){f.nodes[i].localC0=randomMatrix(rng);f.nodes[i].sceneFlags190&=0xfe;for(unsigned j=0;j<16;++j)f.o->store(pointer(ix(i))+0xc0+4*j,f.nodes[i].localC0.v[j]);f.o->write(pointer(ix(i))+0x190,0xa5a5a500u|f.nodes[i].sceneFlags190);}f.initial=f.nodes;f.run();
 }
 // All64 entries may repeat one source index; requested getter count remains64.
 {Fixture f(64);for(unsigned i=1;i<64;++i)f.link(i,i-1);f.ids.fill(63);f.seed();f.run();}
 // Source table may contain duplicate graph references. This is an admitted
 // numeric association, not a claim that the original loader creates aliases.
 {Fixture f(8);for(unsigned i=1;i<8;++i)f.link(i,0);f.source.fill(ix(7));f.seed();f.run();}
 // Copy-only paths retain every word, including nonfinite payloads. No blanket
 // finiteness scan may reject ready caches or dirty root copy-only matrices.
 for(unsigned ready=0;ready<2;++ready){Fixture f(2);for(unsigned i=0;i<2;++i){f.nodes[i].sceneFlags190=0x80u|ready;for(unsigned j=0;j<16;++j){f.nodes[i].localC0.v[j]=fp(0x7fc00001+j+i*32);f.nodes[i].world100.v[j]=fp(0xff800000+j+i*32);}}f.seed();f.run();}
 // Deliberately ready descendants of a dirty parent are source-admitted. They
 // keep cached worlds until actually dirtied by a represented producer.
 {Fixture f(4);f.link(1,0);f.link(2,1);f.link(3,0);f.nodes[1].sceneFlags190=1;f.nodes[2].sceneFlags190=1;f.input.paletteCount=1;f.ids[0]=2;f.seed();f.run();}
}
static void authored(const char*name){
 const std::string path=std::string("evidence/authoredGeometry49O/asset-reader/fixtures/")+name;std::ifstream in(path,std::ios::binary);std::vector<char>b((std::istreambuf_iterator<char>(in)),{});need(b.size()>12,"PSG input missing");auto word=[&](unsigned at){U v;std::memcpy(&v,b.data()+at,4);return v;};const unsigned n=word(8);need(n==14||n==18,"unexpected PSG node count");
 for(unsigned variant=0;variant<8;++variant){Fixture f(n+2);f.input.sourceNodeCount=f.input.paletteCount=n;
  for(unsigned i=0;i<n;++i){f.source[i]=ix(i+1);f.ids[i]=i;const int parent=static_cast<std::int32_t>(word(12+224*i+220));f.link(i+1,parent<0?0:static_cast<unsigned>(parent)+1);for(unsigned j=0;j<16;++j)f.nodes[i+1].localC0.v[j]=fp(word(12+224*i+128+j*4));f.nodes[i+1].sceneFlags190=(variant&1)?0xa5:0xa4;}
  f.nodes[0].sceneFlags190=(variant&2)?0x41:0x40;f.nodes[0].localC0=identity();f.nodes[0].localC0.v[0]=0;f.nodes[0].localC0.v[1]=1;f.nodes[0].localC0.v[4]=-1;f.nodes[0].localC0.v[5]=0;f.nodes[0].localC0.v[12]=12.25f;f.nodes[0].localC0.v[13]=-8.5f;f.nodes[0].localC0.v[14]=31.75f;
  if(variant&4)std::reverse(f.ids.begin(),f.ids.begin()+n);
  f.seed();f.run();f.run();
 }
}
static void admission(){
 Fixture f(4);f.link(1,0);f.link(2,1);f.link(3,0);f.seed();
 auto reject=[&](ModelPaletteError49S error){auto before=f.nodes;const auto out=f.output;const auto alloc=allocations;nativeGetterCount=nativeFlagCount=0;need(!projectModelPalette49S({f.nodes.data(),f.count},f.input,f.output,f.workspace,f.diagnostic),"invalid input accepted");need(f.diagnostic.error==error,"rejection diagnostic mismatch");need(std::memcmp(before.data(),f.nodes.data(),sizeof before)==0&&std::memcmp(&out,&f.output,sizeof out)==0,"transactional rejection mismatch");need(alloc==allocations,"rejection allocation mismatch");};
 const auto valid=f.input;
 f.input.phase=ModelPalettePhase49S::unspecified;reject(ModelPaletteError49S::unsupportedContext);f.input=valid;
 f.input.suppliedState=ModelPaletteSceneState49S::localMatricesOnly;reject(ModelPaletteError49S::unsupportedContext);f.input=valid;
 f.input.modelFlags191=0;reject(ModelPaletteError49S::unsupportedContext);f.input=valid;
 need(std::fesetround(FE_DOWNWARD)==0,"rounding set");reject(ModelPaletteError49S::unsupportedContext);need(std::fesetround(FE_TONEAREST)==0,"rounding restore");
 f.count=0;reject(ModelPaletteError49S::invalidGraph);f.count=65;reject(ModelPaletteError49S::invalidGraph);f.count=4;
 f.input.modelRoot=I::none;reject(ModelPaletteError49S::invalidRoot);f.input=valid;f.input.modelRoot=ix(4);reject(ModelPaletteError49S::invalidRoot);f.input=valid;
 f.input.sourceNodes=nullptr;reject(ModelPaletteError49S::invalidSourceTable);f.input=valid;f.input.sourceNodeCount=0;reject(ModelPaletteError49S::invalidSourceTable);f.input=valid;f.input.sourceNodeCount=65;reject(ModelPaletteError49S::invalidSourceTable);f.input=valid;
 f.source[3]=I::none;reject(ModelPaletteError49S::invalidSourceTable);f.source[3]=ix(3);
 f.input.paletteIds=nullptr;reject(ModelPaletteError49S::invalidPalette);f.input=valid;f.input.paletteCount=0;reject(ModelPaletteError49S::invalidPalette);f.input=valid;f.input.paletteCount=65;reject(ModelPaletteError49S::invalidPalette);f.input=valid;f.ids[3]=4;reject(ModelPaletteError49S::invalidPalette);f.ids[3]=3;
 auto saved=f.nodes;f.nodes[0].firstChild184=I::none;reject(ModelPaletteError49S::invalidGraph);f.nodes=saved;
 // A later palette entry rejects after earlier dirty caches were successfully
 // staged. Graph/output must remain entirely unchanged, even at late failure.
 f.nodes[2].localC0.v[0]=fp(0x7f800000);reject(ModelPaletteError49S::sceneArithmetic);f.nodes=saved;
 f.nodes[1].localC0.v[0]=std::numeric_limits<float>::max();f.nodes[2].localC0.v[0]=2.f;reject(ModelPaletteError49S::sceneArithmetic);f.nodes=saved;
}
static void coverage(){const char*path=std::getenv("RR_PALETTE49S_COVERAGE");if(!path)return;std::ofstream out(path);need(bool(out),"coverage file");out<<"{\"fixtures\":"<<fixtures<<",\"operations\":"<<operations<<",\"checks\":"<<checks<<",\"instructions\":"<<instructions<<",\"getter_calls\":"<<getterCalls<<",\"ordered_trace\":"<<(traced?"true":"false")<<",\"flag_stores\":"<<flagStores<<",\"hot_allocations\":0,\"pcs\":[";bool first=true;for(const auto&b:palette49s_words::blocks)for(unsigned i=0;i<b.count;++i){if(!first)out<<',';first=false;const auto k=b.index+i;out<<"{\"pc\":"<<b.start+4*i<<",\"word\":"<<b.words[i]<<",\"hits\":"<<hits[k]<<",\"taken\":"<<taken[k]<<",\"not_taken\":"<<notTaken[k]<<'}';}out<<"]}\n";}
int main(int argc,char**argv){try{need(std::fesetround(FE_TONEAREST)==0,"rounding");if(argc==3){mutationPc=std::stoul(argv[1],nullptr,16);mutationWord=std::stoul(argv[2],nullptr,16);}cases();authored("43-engrt1.psg");authored("48-englt1.psg");authored("52-chariot.psg");admission();coverage();std::cout<<"PASS ModelPalette49S: "<<fixtures<<" fixtures, "<<operations<<" projections, "<<checks<<" checks, "<<instructions<<" original instructions, "<<getterCalls<<" getters, "<<flagStores<<" ordered flag stores, zero hot allocations; "<<(traced?"ordered tracing on":"ordered tracing off")<<'\n';return 0;}catch(const std::exception&e){std::cerr<<"FAIL ModelPalette49S: "<<e.what()<<'\n';return 1;}}
