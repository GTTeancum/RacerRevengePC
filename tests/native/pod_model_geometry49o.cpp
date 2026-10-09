#include <string>
#include "../../src/game/pod_model_geometry49o.h"
#include "../../evidence/authoredGeometry49O/motion/motion_machine.h"
#include "../../evidence/authoredGeometry49O/bounds/bounds_oracle.h"
#include "../../evidence/authoredGeometry49O/initialization/geometry_oracle_bridge49o.h"
#include "../../evidence/authoredGeometry49O/bindings/bindings_cases49o.h"
#include <cfenv>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <sstream>

using namespace rr::game;
using namespace rr::assets;
using U=std::uint32_t;
using Bytes=std::vector<std::uint8_t>;
using Matrix=std::array<U,16>;
namespace bo=bounds49o_oracle;
static std::uint64_t allocations=0,checks=0,nativeCalls=0,rejections=0;
static std::uint64_t motionInstructions=0,boundsInstructions=0,bindingInstructions=0;
static std::map<U,std::uint64_t> motionHits,motionTaken,motionNotTaken;
static std::array<std::uint64_t,bounds49o_words::total_words> boundsHits{},boundsTaken{},boundsNotTaken{};
static std::array<std::uint64_t,initialization49o_words::total_words> bindingHits{},bindingTaken{},bindingNotTaken{};
[[gnu::noinline]] void* operator new(std::size_t n){void* p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();++allocations;return p;}
[[gnu::noinline]] void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void operator delete(void* p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void* p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete(void* p,std::size_t)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
static void require(bool value,const char* message){++checks;if(!value)throw std::runtime_error(message);}
static U bits(float value){U word;std::memcpy(&word,&value,4);return word;}
static float fp(U word){float value;std::memcpy(&value,&word,4);return value;}
static U read(const Bytes& b,std::size_t at){require(at+4<=b.size(),"bounded raw word");return U(b[at])|(U(b[at+1])<<8)|(U(b[at+2])<<16)|(U(b[at+3])<<24);}
static void write(Bytes& b,std::size_t at,U word){require(at+4<=b.size(),"bounded raw mutation");for(unsigned j=0;j<4;++j)b[at+j]=std::uint8_t(word>>(j*8));}
static void change(Bytes& b,std::size_t at,float value){write(b,at,bits(value));}
static void equalWord(float actual,U expected,const std::string& field){++checks;if(bits(actual)!=expected){std::ostringstream s;s<<field<<": expected "<<std::hex<<expected<<", got "<<bits(actual);throw std::runtime_error(s.str());}}
static Bytes load(const std::string& path){std::ifstream f(path,std::ios::binary);require(bool(f),"fixture missing");return {(std::istreambuf_iterator<char>(f)),{}};}
struct Assets {Bytes chariot,right,forward,back,vut;};
static Assets loadAssets(const std::string& directory){return {load(directory+"/52-chariot.psg"),load(directory+"/43-engrt1.psg"),load(directory+"/56-01_forwd.mot"),load(directory+"/57-01_back.mot"),load(directory+"/53-chariot.vut")};}
static FreshPodGeometrySources49O views(const Assets& a){
 FreshPodGeometrySources49O s{};GeometryDiagnostic49O gd{};MotionBindingDiagnostic49O bd{};
 require(readPsgHierarchy49O(a.chariot.data(),a.chariot.size(),s.chariot,gd),"chariot reader");
 require(readPsgHierarchy49O(a.right.data(),a.right.size(),s.rightEngine,gd),"engine reader");
 require(readMot49O(a.forward.data(),a.forward.size(),s.forward,gd),"forward reader");
 require(readMot49O(a.back.data(),a.back.size(),s.back,gd),"back reader");
 require(readMotionNodeBindings49O(s.chariot,reinterpret_cast<const char*>(a.vut.data()),a.vut.size(),s.chariotBindings,bd),"binding reader");
 // Frozen expected rows are outputs of original152EA0/153218; no native row
 // map is passed to the reference evaluator. The actual18 identity is proved.
 const auto& proven=binding49o_cases::cases[0];require(s.chariotBindings.count==proven.count,"source row count");
 for(unsigned i=0;i<proven.count;++i){const auto& row=s.chariotBindings.rows[i];const auto& e=proven.expected[i];require(row.sceneNode==e.node,"original row association");require(!std::strcmp(row.name,e.name),"original row name");require(row.translationFlag38==bool(e.translation),"original translation flag");require(row.listedFlag3c==bool(e.listed),"original listed flag");}
 return s;
}
static void collect(const bo::Machine& m){boundsInstructions+=m.steps;for(unsigned i=0;i<boundsHits.size();++i){boundsHits[i]+=m.hits[i];boundsTaken[i]+=m.taken[i];boundsNotTaken[i]+=m.notTaken[i];}}
static void collect(const initialization49o_fixture::Result& r){bindingInstructions+=r.instructions;for(unsigned i=0;i<bindingHits.size();++i){bindingHits[i]+=r.hits[i];bindingTaken[i]+=r.taken[i];bindingNotTaken[i]+=r.notTaken[i];}}
static void collect(const motion49o::Machine& m){motionInstructions+=m.steps;for(auto [pc,n]:m.hits)motionHits[pc]+=n;for(auto [pc,n]:m.taken)motionTaken[pc]+=n;for(auto [pc,n]:m.notTaken)motionNotTaken[pc]+=n;}
constexpr U root=0x1000,nodeBase=0x2000,nodeStride=0x200,pointers=0xb000;
static void rootFixture(bo::Machine& m){for(unsigned j=0;j<16;++j)m.write(root+0xc0+4*j,(j%5==0)?0x3f800000u:0);m.write(root+0x190,0x80);}
struct Bounds {
 std::array<Matrix,18> world{};
 std::array<U,18> sceneFlags{};
 std::array<U,4> minimum{},maximum{},center{},extent{};
 U radius=0,rootFlags=0;bool seeded=false;
};
// The fixture marshals bytes only. All geometry arithmetic is executed by the
// extracted original bounds/world words, including each standalone node box.
static Bounds originalBounds(const Bytes& source,const std::array<Matrix,18>& local,int selected=-1){
 auto m=std::make_unique<bo::Machine>();rootFixture(*m);m->write(root+0x1a0,18);m->write(root+0x1b8,pointers);
 for(unsigned i=0;i<18;++i){const U node=nodeBase+nodeStride*i,at=12+224*i;const auto parent=static_cast<std::int32_t>(read(source,at+220));
  m->write(pointers+4*i,node);m->write(node+0x180,parent<0?root:nodeBase+nodeStride*parent);
  m->write(node+0x190,selected<0||selected==int(i)?0xa4:0xa0);m->write(node+0x194,read(source,at+216));
  for(unsigned j=0;j<16;++j)m->write(node+0xc0+4*j,local[i][j]);
  for(unsigned j=0;j<3;++j){m->write(node+0x140+4*j,read(source,at+192+4*j));m->write(node+0x150+4*j,read(source,at+204+4*j));}
 }
 m->call(0x26f4b0,root);Bounds b{};
 for(unsigned j=0;j<4;++j){b.minimum[j]=m->read(bo::Machine::stack-0xf0+0x40+4*j);b.maximum[j]=m->read(bo::Machine::stack-0xf0+0x50+4*j);b.center[j]=m->read(root+0x160+4*j);b.extent[j]=m->read(root+0x170+4*j);}
 b.radius=m->read(root+0x198);b.rootFlags=m->read(root+0x190);b.seeded=bool(m->read(bo::Machine::stack-0xf0+0xe8));
 for(unsigned i=0;i<18;++i){const U node=nodeBase+nodeStride*i;b.sceneFlags[i]=m->read(node+0x190);require(!m->reads[(node+0x194)/4],"original read stored PSG sphere");for(unsigned j=0;j<16;++j)b.world[i][j]=m->read(node+0x100+4*j);}
 collect(*m);return b;
}
static Matrix originalRightWorld(const Bytes& source){
 auto m=std::make_unique<bo::Machine>();rootFixture(*m);const U node=nodeBase;
 m->write(node+0x180,root);m->write(node+0x190,0xa4);
 for(unsigned j=0;j<16;++j)m->write(node+0xc0+4*j,read(source,12+128+4*j));
 m->call(0x2897b0,node);Matrix result{};for(unsigned j=0;j<16;++j)result[j]=m->read(node+0x100+4*j);
 require(m->hits[m->index(0x2897b0)]>0,"right world original path missing");collect(*m);return result;
}
struct Reference {
 std::array<Matrix,18> local{};
 Matrix rightWorld{};Bounds bounds{};
 AuthoredGeometryInputs49O inputs{};
 initialization49o_fixture::Result initialization{};
};
static Reference original(const Assets& a,const motion49o::Machine& base){
 require(read(a.chariot,8)==18,"closed original node layout");require(a.forward.size()==2284&&a.back.size()==2284,"closed original motion layout");
 auto m=base;std::memcpy(m.mem.data()+0x510000,a.forward.data(),a.forward.size());std::memcpy(m.mem.data()+0x520000,a.back.data(),a.back.size());
 // Supplied locals replace every initialized fixture local, so perturbations
 // cannot accidentally compare against a frozen baseline animation result.
 for(unsigned i=0;i<18;++i){const U node=0x504000+0x200*i;for(unsigned j=0;j<16;++j)m.write(node+0xc0+4*j,read(a.chariot,12+224*i+128+4*j));
  const auto parent=static_cast<std::int32_t>(read(a.chariot,12+224*i+220));m.write(node+0x180,parent<0?0:0x504000+0x200*parent);
  m.write(0x503000+64*i+0x34,0x504000+0x200*binding49o_cases::cases[0].expected[i].node);
 }
 Reference r{};for(unsigned j=0;j<3;++j)r.inputs.preAnimationChariotNullCgLocal.v[j]=fp(read(a.chariot,12+128+4*(12+j)));
 r.rightWorld=originalRightWorld(a.right);r.inputs.preAnimationRightEngineNullCgWorldZ=fp(r.rightWorld[14]);
 m.g[4][0]=0x500000;m.g[5][0]=0x501000;m.g[6][0]=0x502000;m.f[12]=.5f;m.f[13]=0;m.run(0x155e70);
 for(unsigned i=0;i<18;++i)for(unsigned j=0;j<16;++j)r.local[i][j]=m.read(0x504000+0x200*i+0xc0+4*j);
 require(m.rotations.size()==8&&m.positions.size()==1,"actual original channel calls");collect(m);
 r.bounds=originalBounds(a.chariot,r.local);r.inputs.postMotionRadius198=fp(r.bounds.radius);
 r.initialization=initialization49o_fixture::execute(r.inputs);collect(r.initialization);return r;
}
static void compareBounds(const ModelBoundsResult49O& n,const Bounds& o,const std::string& label){
 for(unsigned j=0;j<4;++j){equalWord(n.minimum[j],o.minimum[j],label+" minimum");equalWord(n.maximum[j],o.maximum[j],label+" maximum");equalWord(n.center160[j],o.center[j],label+" center");equalWord(n.halfExtent170[j],o.extent[j],label+" extent");}
 equalWord(n.radius198,o.radius,label+" radius");require(n.rootSceneFlags190==o.rootFlags,"root flags");require(n.seeded==o.seeded,"root seed flag");
}
struct CaseResult {std::string label;FreshPodGeometryResult49O value{};};
static std::vector<CaseResult> caseResults;
static FreshPodGeometryResult49O differential(const Assets& a,const motion49o::Machine& base,const std::string& label,Reference* saved=nullptr){
 const auto expected=original(a,base);const auto source=views(a);FreshPodGeometryWorkspace49O work{};FreshPodGeometryResult49O result{};FreshPodGeometryDiagnostic49O diagnostic{};
 const auto before=allocations;++nativeCalls;const bool ok=buildFreshPodGeometry49O(source,work,result,diagnostic);const auto after=allocations;
 require(ok,(label+" native builder rejected").c_str());require(before==after,"builder allocated");require(diagnostic.error==FreshPodGeometryError49O::none,"builder diagnostic");
 for(unsigned i=0;i<18;++i){for(unsigned j=0;j<16;++j){const auto location=label+" node="+std::to_string(i)+" lane="+std::to_string(j);equalWord(work.local[i].m[j],expected.local[i][j],location+" local");equalWord(work.nodes[i].local.v[j],expected.local[i][j],location+" bounds local");equalWord(work.bounds.world[i].v[j],expected.bounds.world[i][j],location+" world");}
  require(work.nodes[i].sceneFlags190==0xa4,"native fresh scene flags");require(expected.bounds.sceneFlags[i]==0xa5,"original refreshed scene flags");
  require(work.nodes[i].parent==static_cast<std::int32_t>(read(a.chariot,12+224*i+220)),"source parent");
  for(unsigned j=0;j<3;++j){equalWord(work.nodes[i].center140.v[j],read(a.chariot,12+224*i+192+4*j),"authored center marshaling");equalWord(work.nodes[i].halfExtent150.v[j],read(a.chariot,12+224*i+204+4*j),"authored extent marshaling");}
 }
 compareBounds(result.postMotionBounds,expected.bounds,label);
 static_assert(sizeof(AuthoredGeometryInputs49O)==5*4);static_assert(sizeof(AuthoredGeometryBinding49O)==49*4);
 require(!std::memcmp(&result.sourceInputs,&expected.inputs,sizeof expected.inputs),"all pre/post source input words");
 require(!std::memcmp(&result.parameters,&expected.initialization.binding,sizeof result.parameters),"all49 numeric binding words");
 caseResults.push_back({label,result});if(saved)*saved=expected;return result;
}
static void requireDifferentRadius(const FreshPodGeometryResult49O& a,const FreshPodGeometryResult49O& b){require(bits(a.postMotionBounds.radius198)!=bits(b.postMotionBounds.radius198),(caseResults.back().label+" variation did not distinguish live radius").c_str());require(bits(a.parameters.inertia100.v[0])!=bits(b.parameters.inertia100.v[0]),(caseResults.back().label+" variation did not propagate to inertia").c_str());}
static void sameResult(const FreshPodGeometryResult49O& a,const FreshPodGeometryResult49O& b){
 require(!std::memcmp(&a.sourceInputs,&b.sourceInputs,sizeof a.sourceInputs),"dead stored radius changed source inputs");require(!std::memcmp(&a.parameters,&b.parameters,sizeof a.parameters),"dead stored radius changed binding");
 for(unsigned j=0;j<4;++j){equalWord(a.postMotionBounds.minimum[j],bits(b.postMotionBounds.minimum[j]),"dead radius minimum");equalWord(a.postMotionBounds.maximum[j],bits(b.postMotionBounds.maximum[j]),"dead radius maximum");equalWord(a.postMotionBounds.center160[j],bits(b.postMotionBounds.center160[j]),"dead radius center");equalWord(a.postMotionBounds.halfExtent170[j],bits(b.postMotionBounds.halfExtent170[j]),"dead radius extent");}
 equalWord(a.postMotionBounds.radius198,bits(b.postMotionBounds.radius198),"dead radius aggregate");require(a.postMotionBounds.rootSceneFlags190==b.postMotionBounds.rootSceneFlags190&&a.postMotionBounds.seeded==b.postMotionBounds.seeded,"dead radius flags");
}
static void admission(const Assets& a,const FreshPodGeometryResult49O& sentinel){
 auto source=views(a);FreshPodGeometryWorkspace49O work{};FreshPodGeometryDiagnostic49O diagnostic{};
 auto reject=[&](const FreshPodGeometrySources49O& s,FreshPodGeometryError49O error){auto result=sentinel;const auto before=allocations;++nativeCalls;++rejections;const bool ok=buildFreshPodGeometry49O(s,work,result,diagnostic);const auto after=allocations;require(!ok,"unsupported scalar accepted");require(diagnostic.error==error,"wrong scalar rejection stage");require(!std::memcmp(&result,&sentinel,sizeof result),"builder failure not atomic");require(before==after,"rejected builder allocated");};
 auto bad=source;bad.chariot.nodeCount=0;reject(bad,FreshPodGeometryError49O::asset);bad=source;bad.chariot.nodeCount=65;reject(bad,FreshPodGeometryError49O::asset);bad=source;bad.rightEngine.nodeCount=0;reject(bad,FreshPodGeometryError49O::asset);bad=source;bad.rightEngine.nodeCount=65;reject(bad,FreshPodGeometryError49O::asset);
 bad=source;bad.chariotBindings.count=17;reject(bad,FreshPodGeometryError49O::asset);bad=source;bad.chariotBindings.rows[1].sceneNode=18;reject(bad,FreshPodGeometryError49O::motion);bad=source;bad.back.position[0].rowIndex=2;reject(bad,FreshPodGeometryError49O::motion);
 bad=source;bad.back.rotationCount=0;reject(bad,FreshPodGeometryError49O::motion);bad=source;bad.forward.duration=std::numeric_limits<float>::infinity();reject(bad,FreshPodGeometryError49O::motion);bad=source;bad.forward.duration*=4;reject(bad,FreshPodGeometryError49O::motion);
 // Parsed finite bytes can still overflow a later admitted numeric stage.
 auto huge=a;change(huge.chariot,12+204,std::numeric_limits<float>::max());auto hugeViews=views(huge);reject(hugeViews,FreshPodGeometryError49O::bounds);
 huge=a;change(huge.right,12+128+14*4,std::numeric_limits<float>::max());hugeViews=views(huge);reject(hugeViews,FreshPodGeometryError49O::parameters);
 // A different valid PSG name exercises nullcg admission without malformed
 // backing buffers. Row map is independently provided by caller at this edge.
 auto named=a;named.right[12]='x';GeometryDiagnostic49O gd{};bad=source;require(readPsgHierarchy49O(named.right.data(),named.right.size(),bad.rightEngine,gd),"alternate valid node name");reject(bad,FreshPodGeometryError49O::nullcg);
 for(int mode:{FE_UPWARD,FE_DOWNWARD,FE_TOWARDZERO}){require(!std::fesetround(mode),"set rounding");reject(source,FreshPodGeometryError49O::bounds);require(!std::fesetround(FE_TONEAREST),"restore rounding");}
}
static void words(std::ostream& out,const U* values,unsigned n){out<<'[';for(unsigned i=0;i<n;++i){if(i)out<<',';out<<'"'<<std::hex<<std::setfill('0')<<std::setw(8)<<values[i]<<'"';}out<<std::dec<<']';}
static void coverage(const std::string& directory){
 std::ofstream out(directory+"/coverage.json");require(bool(out),"coverage output");out<<"{\"motion\":[";bool first=true;
 for(auto [start,end]:blocks)for(U pc=start;pc<end;pc+=4){if(!first)out<<',';first=false;out<<"{\"pc\":"<<pc<<",\"hits\":"<<motionHits[pc]<<",\"taken\":"<<motionTaken[pc]<<",\"not_taken\":"<<motionNotTaken[pc]<<'}';}
 out<<"],\"bounds\":[";first=true;for(const auto& block:bounds49o_words::blocks)for(unsigned j=0;j<block.count;++j){const auto i=block.index+j;if(!first)out<<',';first=false;out<<"{\"pc\":"<<block.start+4*j<<",\"hits\":"<<boundsHits[i]<<",\"taken\":"<<boundsTaken[i]<<",\"not_taken\":"<<boundsNotTaken[i]<<'}';}
 out<<"],\"binding\":[";first=true;for(const auto& block:initialization49o_words::blocks)for(unsigned j=0;j<block.count;++j){const auto i=block.index+j;if(!first)out<<',';first=false;out<<"{\"pc\":"<<block.start+4*j<<",\"hits\":"<<bindingHits[i]<<",\"taken\":"<<bindingTaken[i]<<",\"not_taken\":"<<bindingNotTaken[i]<<'}';}out<<"]}\n";
}
int main(int argc,char** argv){try{
 require(!std::fesetround(FE_TONEAREST),"nearest rounding");const std::string evidence=argc>1?argv[1]:"evidence/authoredGeometry49O",output=argc>2?argv[2]:"";
 const auto assets=loadAssets(evidence+"/asset-reader/fixtures");motion49o::Machine base((evidence+"/motion/fixture.bin").c_str());base.bytes(0x3debc0,(evidence+"/motion/trig/sin-table.bin").c_str());base.bytes(0x3defc0,(evidence+"/motion/trig/cos-table.bin").c_str());base.bytes(0x3e1dd0,(evidence+"/motion/trig/acos-table.bin").c_str());
 Reference baselineReference{};const auto baseline=differential(assets,base,"actual-pod01",&baselineReference);
 // Establish actual motion happened before any equality-to-bind assertion.
 std::array<Matrix,18> bindLocal{};unsigned changed=0;for(unsigned i=0;i<18;++i)for(unsigned j=0;j<16;++j){bindLocal[i][j]=read(assets.chariot,12+224*i+128+4*j);changed+=bindLocal[i][j]!=baselineReference.local[i][j];}require(changed>0,"actual motion was skipped");
 const auto bindBounds=originalBounds(assets.chariot,bindLocal);std::array<Bounds,18> leaves{};
 for(unsigned i=0;i<18;++i)leaves[i]=originalBounds(assets.chariot,baselineReference.local,int(i));
 for(unsigned i=0;i<18;++i)for(unsigned j=0;j<3;++j){require(fp(leaves[i].minimum[j])>=fp(leaves[0].minimum[j])&&fp(leaves[i].maximum[j])<=fp(leaves[0].maximum[j]),"post-motion child not contained in source root box");require(baselineReference.bounds.minimum[j]==leaves[0].minimum[j]&&baselineReference.bounds.maximum[j]==leaves[0].maximum[j],"post-motion extrema not root box");}
 require(baselineReference.bounds.radius==0x40d082c6,"post-motion radius golden");require(bindBounds.radius==baselineReference.bounds.radius,"independently computed bind/live equality");
 auto varied=assets;for(auto* raw:{&varied.chariot,&varied.right})for(unsigned i=0;i<read(*raw,8);++i)change(*raw,12+224*i+216,99999.f+float(i));const auto dead=differential(varied,base,"all-stored-radii-finite-change");sameResult(dead,baseline);
 // Expand each eligible authored half-extent, including dummies. Every edit is
 // a scalar in original bytes, independently parsed and marshaled to both sides.
 for(unsigned node=0;node<18;++node)for(unsigned axis=0;axis<3;++axis){varied=assets;change(varied.chariot,12+224*node+204+4*axis,32.f+float(node));const auto result=differential(varied,base,"box-extent-"+std::to_string(node)+"-"+std::to_string(axis));requireDifferentRadius(result,baseline);}
 for(unsigned node=0;node<18;++node)for(unsigned axis=0;axis<3;++axis){varied=assets;const auto at=12+224*node+192+4*axis;change(varied.chariot,at,fp(read(varied.chariot,at))+64.f);const auto result=differential(varied,base,"box-center-"+std::to_string(node)+"-"+std::to_string(axis));if(node)requireDifferentRadius(result,baseline);}
 for(unsigned axis=0;axis<3;++axis)for(float value:{-17.25f,-0.0f,0.125f,31.5f}){varied=assets;change(varied.chariot,12+128+4*(12+axis),value);differential(varied,base,"chariot-nullcg-"+std::to_string(axis)+"-"+std::to_string(bits(value)));}
 for(float value:{-21.25f,-0.0f,0.0f,0.125f,31.5f}){varied=assets;change(varied.right,12+128+14*4,value);const auto result=differential(varied,base,"right-nullcg-"+std::to_string(bits(value)));if(bits(value)==0x80000000u)require(bits(result.sourceInputs.preAnimationRightEngineNullCgWorldZ)==0,"right world signed-zero operation was bypassed");}
 // These nodes have neither a position nor a rotation channel in either MOT.
 for(unsigned node:{0u,2u,3u,4u,5u,6u,7u,8u,16u,17u})for(unsigned lane:{0u,1u,4u,10u,12u,13u,14u}){varied=assets;const auto at=12+224*node+128+4*lane;change(varied.chariot,at,fp(read(varied.chariot,at))+(lane>=12?32.f:.25f));differential(varied,base,"untouched-local-"+std::to_string(node)+"-"+std::to_string(lane));}
 // Fixed channel descriptors/layout; vary only admitted raw scalar/ByteAxis
 // key data. Forward fraction.5 selects the actual middle interval. Back is
 // the blend0 endpoint; Forward is still decoded and original endpoint
 // quaternion operations are retained rather than algebraically canceled.
 for(unsigned variant=0;variant<32;++variant){varied=assets;U rng=0x49c049u+variant;for(auto* motion:{&varied.forward,&varied.back})for(unsigned track=0;track<8;++track)for(unsigned key=0;key<21;++key)for(unsigned byte=0;byte<4;++byte){rng=rng*1664525u+1013904223u;(*motion)[884+176*track+8*key+4+byte]=std::uint8_t(rng>>24);}differential(varied,base,"byteaxis-keys-"+std::to_string(variant));}
 for(unsigned axis=0;axis<3;++axis)for(unsigned member=0;member<2;++member){varied=assets;auto& motion=member?varied.back:varied.forward;for(unsigned key=0;key<21;++key){const auto at=36+40*key+4+4*axis;change(motion,at,fp(read(motion,at))+64.f);}const auto result=differential(varied,base,"bezier-position-"+std::to_string(member)+"-"+std::to_string(axis));if(member)requireDifferentRadius(result,baseline);}
 admission(assets,baseline);
 if(!output.empty()){
 coverage(output);
 std::ofstream proof(output+"/baseline.json");require(bool(proof),"baseline output");proof<<"{\"actual_motion_changed_local_words\":"<<changed<<",\"live_radius\":\"40d082c6\",\"bind_radius_equals_live_after_containment\":true,\"right_engine_world\":";words(proof,baselineReference.rightWorld.data(),16);proof<<",\"nodes\":[";
 for(unsigned i=0;i<18;++i){if(i)proof<<',';proof<<"{\"index\":"<<i<<",\"local\":";words(proof,baselineReference.local[i].data(),16);proof<<",\"world\":";words(proof,baselineReference.bounds.world[i].data(),16);proof<<",\"minimum\":";words(proof,leaves[i].minimum.data(),4);proof<<",\"maximum\":";words(proof,leaves[i].maximum.data(),4);proof<<",\"contained_in_source_node0\":true}";}proof<<"]}\n";
 std::ofstream summaries(output+"/cases.json");require(bool(summaries),"case output");summaries<<"[";for(unsigned i=0;i<caseResults.size();++i){if(i)summaries<<',';const auto& c=caseResults[i];U source[5],binding[49];std::memcpy(source,&c.value.sourceInputs,sizeof source);std::memcpy(binding,&c.value.parameters,sizeof binding);summaries<<"{\"name\":\""<<c.label<<"\",\"source_inputs\":";words(summaries,source,5);summaries<<",\"binding\":";words(summaries,binding,49);summaries<<'}';}summaries<<"]\n";
 }
 std::cout<<"{\"status\":\"pass\",\"cases\":"<<caseResults.size()<<",\"checks\":"<<checks<<",\"scalar_rejections\":"<<rejections<<",\"native_calls\":"<<nativeCalls<<",\"native_allocations\":0,\"motion_instructions\":"<<motionInstructions<<",\"bounds_instructions\":"<<boundsInstructions<<",\"binding_instructions\":"<<bindingInstructions<<",\"actual_motion_changed_local_words\":"<<changed<<",\"all_words_match\":true}\n";
 return 0;
 }catch(const std::exception& e){std::cerr<<"FAIL PodModelGeometry49O: "<<e.what()<<'\n';return 1;}}
