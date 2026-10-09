#include "../../src/game/chariot_projection49p.h"
#include "../../src/assets/model_geometry49o.h"
#include "../../evidence/sceneProjection49P/source/scene_oracle.h"
#include "../../evidence/authoredGeometry49O/bounds/pod01_live_matrices.h"
#include <algorithm>
#include <array>
#include <cfenv>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <new>
#include <sstream>
#include <vector>

using namespace rr::game;
namespace original=scene49p_oracle;
using original::U;using original::bits;using original::fp;
using M=BodyMatrix48G;using N=SceneNode49P;using I=SceneNodeIndex49P;
constexpr unsigned count=22,mainRoot=0,container=1,secondaryRoot=2,chariotRoot=3,nodeBase=4;
static std::uint64_t allocations=0,checks=0,hotCalls=0,instructions=0,comparedStates=0;
static std::uint64_t phaseDigest=1469598103934665603ull;
static unsigned currentScenario=0,currentFrame=0,currentStep=0;
static const char* currentStage="loading";
[[gnu::noinline]] void* operator new(std::size_t n){void* p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();++allocations;return p;}
[[gnu::noinline]] void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void operator delete(void*p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete(void*p,std::size_t)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
static void need(bool b,const char* message){++checks;if(!b)throw std::runtime_error(message);}
static I ix(unsigned i){return static_cast<I>(i);}
static U pointer(I i){return i==I::none?0:0x1000+static_cast<U>(i)*0x200;}
static M identity(){M m{};for(unsigned i=0;i<4;++i)m.v[5*i]=1;return m;}
static void hashWord(std::uint64_t& h,U w){for(unsigned b=0;b<4;++b){h^=(w>>(b*8))&255u;h*=1099511628211ull;}}
static std::string hex(std::uint64_t x){std::ostringstream o;o<<std::hex<<std::setfill('0')<<std::setw(16)<<x;return o.str();}
static bool equal(const M&a,const M&b){return std::memcmp(&a,&b,sizeof a)==0;}
static std::vector<std::uint8_t> readBytes(const char* path){std::ifstream f(path,std::ios::binary);need(bool(f),"input missing");return std::vector<std::uint8_t>((std::istreambuf_iterator<char>(f)),{});}
struct Cursor {
 const std::vector<std::uint8_t>& bytes;std::size_t at=8;
 U word(){need(at+4<=bytes.size(),"compact fixture truncated");U w=0;for(unsigned j=0;j<4;++j)w|=U(bytes[at++])<<(8*j);return w;}
 M matrix(){M m{};for(float& f:m.v)f=fp(word());return m;}
};
struct Step{M main,secondary;};
struct Frame{U index,later838;std::vector<Step> steps;};
struct Scenario{std::array<float,4> target;std::vector<Frame> frames;};
static std::vector<Scenario> readTrace(const char* path){
 const auto bytes=readBytes(path);need(bytes.size()>=12&&std::memcmp(bytes.data(),"RR49P001",8)==0,"compact fixture header");Cursor c{bytes};const U n=c.word();need(n==14,"scenario count");std::vector<Scenario> scenarios(n);
 for(auto&s:scenarios){const U frames=c.word();need(frames==120,"frame count");for(auto&x:s.target)x=fp(c.word());s.frames.resize(frames);for(auto&f:s.frames){f.index=c.word();f.later838=c.word();f.steps.resize(c.word());need(!f.steps.empty(),"empty supplied physical frame");for(auto&step:f.steps){step.main=c.matrix();step.secondary=c.matrix();}}}
 need(c.at==bytes.size(),"compact fixture trailing bytes");return scenarios;
}
struct Hierarchy {
 std::array<M,18> bind{};std::array<std::int32_t,18> parent{};
 explicit Hierarchy(const char*path){const auto bytes=readBytes(path);rr::assets::PsgHierarchyView49O view{};rr::assets::GeometryDiagnostic49O d{};need(rr::assets::readPsgHierarchy49O(bytes.data(),bytes.size(),view,d),"actual Pod01 PSG admission");need(view.nodeCount==18,"actual Pod01 node count");
  for(unsigned i=0;i<18;++i){rr::assets::PsgNode49O n{};need(rr::assets::readPsgNode49O(view,i,n),"actual Pod01 node");std::memcpy(bind[i].v,n.local,sizeof n.local);parent[i]=n.parent;if(i==0)need(std::strcmp(n.name,"nullcg")==0,"actual nullcg identity");}
  // Explicit post-first-base node condition supplied by the fixture. Neither
  //217440 nor its other side effects are executed here.
  bind[0].v[12]=bind[0].v[13]=bind[0].v[14]=0;
 }
};
struct Fixture {
 std::array<N,count> nodes{};
 std::unique_ptr<original::Machine> oracle=std::make_unique<original::Machine>();
 SceneGraph49P graph(){return {nodes.data(),count};}
 void link(unsigned child,unsigned parent){nodes[child].parent180=ix(parent);auto*p=&nodes[parent].firstChild184;while(*p!=I::none)p=&nodes[static_cast<unsigned>(*p)].nextSibling188;*p=ix(child);}
 Fixture(const Hierarchy&hierarchy,const Step&seed){
  for(auto&n:nodes){n.localC0=identity();n.world100={};n.parent180=n.firstChild184=n.nextSibling188=I::none;n.sceneFlags190=0x84;}
  nodes[mainRoot].localC0=seed.main;nodes[secondaryRoot].localC0=seed.secondary;
  link(container,mainRoot);
  for(unsigned i=0;i<18;++i){nodes[nodeBase+i].localC0=hierarchy.bind[i];link(nodeBase+i,hierarchy.parent[i]<0?chariotRoot:nodeBase+unsigned(hierarchy.parent[i]));}
  nodes[chariotRoot].sceneFlags190&=0xbfu;nodes[nodeBase].sceneFlags190&=0xdfu;
  need(validateSceneGraph49P(graph()),"fixed actual-source forest");for(unsigned i=0;i<count;++i)supply(i);compare();
 }
 void supply(unsigned i){const U p=pointer(ix(i));const auto&n=nodes[i];for(unsigned j=0;j<16;++j){oracle->store(p+0xc0+4*j,n.localC0.v[j]);oracle->store(p+0x100+4*j,n.world100.v[j]);}oracle->write(p+0x180,pointer(n.parent180));oracle->write(p+0x184,pointer(n.firstChild184));oracle->write(p+0x188,pointer(n.nextSibling188));oracle->write(p+0x190,0x6b4d2100u|n.sceneFlags190);}
 std::uint64_t digest(bool expected)const{std::uint64_t h=1469598103934665603ull;for(unsigned i=0;i<count;++i){const U p=pointer(ix(i));const auto&n=nodes[i];for(unsigned j=0;j<16;++j){hashWord(h,expected?oracle->read(p+0xc0+4*j):bits(n.localC0.v[j]));hashWord(h,expected?oracle->read(p+0x100+4*j):bits(n.world100.v[j]));}hashWord(h,expected?oracle->read(p+0x190)&255u:n.sceneFlags190);hashWord(h,pointer(n.parent180));hashWord(h,pointer(n.firstChild184));hashWord(h,pointer(n.nextSibling188));}return h;}
 void compare(){for(unsigned i=0;i<count;++i){const U p=pointer(ix(i));const auto&n=nodes[i];for(unsigned j=0;j<16;++j){need(bits(n.localC0.v[j])==oracle->read(p+0xc0+4*j),"local16 original-word mismatch");need(bits(n.world100.v[j])==oracle->read(p+0x100+4*j),"world16 original-word mismatch");}need(n.sceneFlags190==(oracle->read(p+0x190)&255u),"flags original-word mismatch");need((oracle->read(p+0x190)&0xffffff00u)==0x6b4d2100u,"adjacent flag bytes changed");need(pointer(n.parent180)==oracle->read(p+0x180),"parent link changed");need(pointer(n.firstChild184)==oracle->read(p+0x184),"first child changed");need(pointer(n.nextSibling188)==oracle->read(p+0x188),"sibling link changed");}
  const auto h=digest(true);need(digest(false)==h,"canonical scene digest mismatch");hashWord(phaseDigest,U(h));hashWord(phaseDigest,U(h>>32));++comparedStates;
 }
 template<class F>void hot(F f){const auto before=allocations;const bool ok=f();need(allocations==before,"native scene call allocated");need(ok,"admitted native scene call rejected");++hotCalls;}
 enum Op{SetRoot,SetLocal,World};
 void operation(Op op,unsigned index,const M&m=identity()){
  constexpr U input=0x9000;for(unsigned j=0;j<16;++j)oracle->store(input+4*j,m.v[j]);oracle->stores.clear();oracle->trace.clear();const U p=pointer(ix(index));
  if(op==SetRoot)oracle->call(0x289b20,p,0,input);else if(op==SetLocal)oracle->call(0x28b490,p,input);else oracle->call(0x2897b0,p);
  hot([&](){if(op==SetRoot)return setParentlessWorldMatrix49P(graph(),ix(index),m);if(op==SetLocal)return setLocalMatrix49P(graph(),ix(index),m);return refreshSceneWorld49P(graph(),ix(index));});compare();
 }
 void chariot(){
  constexpr U vehicle=0x8000;oracle->write(vehicle+0x838,1);oracle->write(vehicle+0x83c,0);oracle->write(vehicle+0x19c,pointer(ix(secondaryRoot)));oracle->write(vehicle+0x1b4,pointer(ix(chariotRoot)));oracle->write(vehicle+0x58,0x12345678);
  oracle->g[16][0]=vehicle;oracle->g[29][0]=original::Machine::stack;oracle->g[31][0]=original::Machine::stop;oracle->v[0]={0,0,0,1};oracle->callSteps=0;oracle->stores.clear();oracle->trace.clear();U p=0x21b9a4;
  while(p!=0x21beb8){U next=0,unused=0;if(oracle->execute(p,next)){need(!oracle->execute(p+4,unused),"branch in dispatch delay");p=next;}else p+=4;}
  ChariotProjectionInput49P input{};input.phase=ChariotProjectionPhase49P::beforePhysicalCopy21B9A4;input.suppliedState=ChariotSuppliedState49P::completeSceneStateThroughChariotPrefix;input.vehicle838=1;input.vehicle83C=0;input.secondaryPhysical19C=ix(secondaryRoot);input.chariotModel1B4=ix(chariotRoot);
  hot([&](){return projectChariotPhysicalBranch49P(graph(),input);});compare();
 }
 void suppliedAnimation(const Hierarchy& hierarchy,unsigned frame){
  // A full external state boundary, not an animation implementation. Every
  // third frame supplies changed current locals and dirty flags, retaining the
  // explicitly supplied cached worlds. Other frames retain the entire state.
  if(frame%3)return;
  for(unsigned i=0;i<18;++i){auto&n=nodes[nodeBase+i];if((frame/3)%2)for(unsigned j=0;j<16;++j)n.localC0.v[j]=fp(bounds49o_fixture::liveLocal[i][j]);else n.localC0=hierarchy.bind[i];if(i==0)n.localC0.v[12]=n.localC0.v[13]=n.localC0.v[14]=0;n.sceneFlags190=static_cast<std::uint8_t>((n.sceneFlags190|0x80u)&0xfeu);if(i==0)n.sceneFlags190&=0xdfu;supply(nodeBase+i);}compare();
 }
 void suppliedContainer(unsigned frame){
  // Current194 orientation/translation is explicit post-engine input. The two
  // source scene operations below materialize its supplied local/ready/world
  // snapshot. They do not run21C120's visual response or28A090 orientation.
  M m=identity();if(frame%4==1){m.v[0]=0;m.v[1]=1;m.v[4]=-1;m.v[5]=0;}else if(frame%4==2){m.v[0]=-1;m.v[5]=-1;}else if(frame%4==3){m.v[0]=0;m.v[1]=-1;m.v[4]=1;m.v[5]=0;}
  m.v[12]=float(int(frame%7)-3)*.125f;m.v[13]=.25f;m.v[14]=float(frame%5)*-.0625f;
  operation(SetLocal,container,m);operation(World,container);
 }
 ~Fixture(){instructions+=oracle->steps;}
};
struct NullBuffer:std::streambuf{int overflow(int c)override{return traits_type::not_eof(c);}};
struct Controls{std::uint64_t postPhysicsFrames=0,postPhysicsSteps=0,eagerFrames=0,offsetFrames=0,cachedWorldFrames=0;};
static unsigned differences(const std::array<N,count>&a,const std::array<N,count>&b){unsigned result=0;for(unsigned i=0;i<count;++i)if(!equal(a[i].localC0,b[i].localC0)||!equal(a[i].world100,b[i].world100)||a[i].sceneFlags190!=b[i].sceneFlags190)++result;return result;}
static void jsonMatrix(std::ostream&o,const M&m){o<<'[';for(unsigned j=0;j<16;++j){if(j)o<<',';o<<bits(m.v[j]);}o<<']';}
static void golden(std::ostream&o,const Fixture& f,unsigned scenario,unsigned frame,std::uint64_t copy,std::uint64_t palette,std::uint64_t physical){
 o<<scenario<<' '<<frame<<' '<<hex(copy)<<' '<<hex(palette)<<' '<<hex(physical)<<'\n';
 if(frame==119){o<<"final-leaf-world16";for(unsigned j=0;j<16;++j)o<<' '<<std::hex<<std::setfill('0')<<std::setw(8)<<f.oracle->read(pointer(ix(count-1))+0x100+4*j);o<<std::dec<<'\n';}
}
int main(int argc,char**argv){try{
 need(argc>=4&&argc<=6,"usage: trace.bin hierarchy.psg goldens.txt [output.json [--capture-goldens]]");need(std::fesetround(FE_TONEAREST)==0,"rounding");const auto scenarios=readTrace(argv[1]);const Hierarchy hierarchy(argv[2]);NullBuffer discardBuffer;std::ostream discard(&discardBuffer);std::ofstream fileOutput;if(argc>=5){fileOutput.open(argv[4]);need(bool(fileOutput),"trace output");}std::ostream& output=argc>=5?fileOutput:discard;std::ostringstream goldenWords;Controls controls{};std::uint64_t frames=0,substeps=0,copies=0,lazyInvalidations=0,retainedChariotSteps=0,laterGateZero=0;
 output<<"{\"kind\":\"49P stateful supplied-physical-scene/prephysics-Chariot projection\",\"all_graph_records_compared\":22,\"body_admission_unchanged\":true,\"scenarios\":[";
 for(unsigned si=0;si<scenarios.size();++si){currentScenario=si;const auto&s=scenarios[si];Fixture f(hierarchy,s.frames.front().steps.front());if(si)output<<',';output<<"{\"index\":"<<si<<",\"frames\":[";
  for(const auto&frame:s.frames){currentFrame=frame.index;currentStep=0;++frames;if(!frame.later838)++laterGateZero;if(frame.index)output<<',';
   currentStage="supplied postanimation/container boundary";f.suppliedAnimation(hierarchy,frame.index);f.suppliedContainer(frame.index);
   const M prePhysicsSecondary=f.nodes[secondaryRoot].localC0;const M priorCache=f.nodes[secondaryRoot].world100;
   currentStage="pre21B9A4 physical-mode copy";f.chariot();++copies;need(equal(f.nodes[chariotRoot].localC0,prePhysicsSecondary),"Chariot did not copy prephysics secondary local");const auto copyDigest=f.digest(true);
   // Deliberately wrong alternative operations are applied to ordinary copies
   // of these fixed records. No alias, storage capacity or ownership tests.
   auto eager=f.nodes;f.hot([&](){for(unsigned i=0;i<count;++i)if(!refreshSceneWorld49P({eager.data(),count},ix(i)))return false;return true;});if(differences(eager,f.nodes))++controls.eagerFrames;
   auto offset=f.nodes;SceneDisplacement49P d{{s.target[0],s.target[1],s.target[2],s.target[3]}};f.hot([&](){return addParentlessTranslation49P({offset.data(),count},ix(chariotRoot),d);});if(!equal(offset[chariotRoot].localC0,f.nodes[chariotRoot].localC0))++controls.offsetFrames;
   auto cached=f.nodes;f.hot([&](){return setLocalMatrix49P({cached.data(),count},ix(chariotRoot),priorCache);});if(!equal(cached[chariotRoot].localC0,f.nodes[chariotRoot].localC0))++controls.cachedWorldFrames;
   currentStage="explicit current palette getter requests";for(unsigned i=0;i<18;++i)f.operation(Fixture::World,nodeBase+(frame.index%2?17-i:i));f.operation(Fixture::World,nodeBase+17);const auto paletteDigest=f.digest(true);const M sampledPalette=f.nodes[count-1].world100;
   auto wrongPost=f.nodes;const auto savedChariot=f.nodes[chariotRoot];const M childCachedBeforePhysics=f.nodes[container].world100;
   currentStage="later supplied physical-scene sequence";
   for(const auto&step:frame.steps){++substeps;f.operation(Fixture::SetRoot,mainRoot,step.main);f.operation(Fixture::SetRoot,secondaryRoot,step.secondary);
    need(!(f.nodes[container].sceneFlags190&1u),"later main root failed child194 invalidation");need(equal(f.nodes[container].world100,childCachedBeforePhysics),"child194 world eagerly regenerated");++lazyInvalidations;
    need(equal(f.nodes[chariotRoot].localC0,savedChariot.localC0)&&equal(f.nodes[chariotRoot].world100,savedChariot.world100)&&f.nodes[chariotRoot].sceneFlags190==savedChariot.sceneFlags190,"Chariot copy repeated after physical substep");need(equal(f.nodes[count-1].world100,sampledPalette),"palette changed without getter/local change");++retainedChariotSteps;
    f.hot([&](){return setLocalMatrix49P({wrongPost.data(),count},ix(chariotRoot),step.secondary);});if(!equal(wrongPost[chariotRoot].localC0,f.nodes[chariotRoot].localC0))++controls.postPhysicsSteps;++currentStep;
   }
   if(!equal(wrongPost[chariotRoot].localC0,f.nodes[chariotRoot].localC0))++controls.postPhysicsFrames;
   // Explicit reads at their own later boundary. A secondary read every third
   // frame supplies both stale-cache and current-cache cases to the next base.
   currentStage="explicit later physical-root getters";if(frame.index%3==0)f.operation(Fixture::World,secondaryRoot);if(frame.index%4==0)f.operation(Fixture::World,mainRoot);const auto physicalDigest=f.digest(true);
   golden(goldenWords,f,si,frame.index,copyDigest,paletteDigest,physicalDigest);
   output<<"{\"frame\":"<<frame.index<<",\"supplied_graphics838\":1,\"later_physics838\":"<<frame.later838<<",\"substeps\":"<<frame.steps.size()<<",\"copy_state_fnv64\":\""<<hex(copyDigest)<<"\",\"palette_state_fnv64\":\""<<hex(paletteDigest)<<"\",\"later_state_fnv64\":\""<<hex(physicalDigest)<<"\",\"chariot_local_bits\":";jsonMatrix(output,f.nodes[chariotRoot].localC0);output<<",\"explicit_leaf_palette_bits\":";jsonMatrix(output,sampledPalette);output<<'}';
  }
  output<<"]}";
 }
 need(frames==1680&&substeps==3780&&copies==1680,"replay counts");need(lazyInvalidations==3780&&retainedChariotSteps==3780,"phase retention counts");need(controls.postPhysicsFrames==1515&&controls.postPhysicsSteps==3475,"wrong postphysics copy failed sensitivity");need(controls.eagerFrames==1680&&controls.offsetFrames==1680&&controls.cachedWorldFrames==1066,"wrong eager/offset/cache controls failed sensitivity");
 currentStage="exact independent-word goldens";const bool capture=argc==6&&std::string(argv[5])=="--capture-goldens";if(capture){std::ofstream file(argv[3]);need(bool(file),"golden output");file<<goldenWords.str();}else{std::ifstream file(argv[3]);need(bool(file),"goldens missing");const std::string expected((std::istreambuf_iterator<char>(file)),{});need(expected==goldenWords.str(),"exact phase/leaf goldens changed");}
 output<<"],\"summary\":{\"passed\":true,\"scenarios\":14,\"frames\":"<<frames<<",\"substeps\":"<<substeps<<",\"prephysics_copies\":"<<copies<<",\"compared_states\":"<<comparedStates<<",\"checks\":"<<checks<<",\"native_hot_transitions\":"<<hotCalls<<",\"hot_allocations\":0,\"original_instructions\":"<<instructions<<",\"all_state_fnv64\":\""<<hex(phaseDigest)<<"\",\"child194_lazy_invalidations\":"<<lazyInvalidations<<",\"chariot_retained_later_substeps\":"<<retainedChariotSteps<<",\"later_physics838_zero_frames\":"<<laterGateZero<<",\"wrong_postphysics_copy_changed_frames\":"<<controls.postPhysicsFrames<<",\"wrong_postphysics_copy_changed_substeps\":"<<controls.postPhysicsSteps<<",\"wrong_eager_world_changed_frames\":"<<controls.eagerFrames<<",\"wrong_added2F0_changed_frames\":"<<controls.offsetFrames<<",\"wrong_cached_world_copy_changed_frames\":"<<controls.cachedWorldFrames<<"}}\n";
 std::cout<<"PASS SceneFrameDiagnostic49P: "<<frames<<" frames, "<<substeps<<" supplied substeps, "<<copies<<" prephysics copies, "<<comparedStates<<" complete scene states, "<<checks<<" checks, "<<instructions<<" original instructions, zero native hot allocations\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FAIL SceneFrameDiagnostic49P scenario="<<currentScenario<<" frame="<<currentFrame<<" step="<<currentStep<<" stage="<<currentStage<<": "<<e.what()<<'\n';return 1;}}
