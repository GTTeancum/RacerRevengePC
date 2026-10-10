#include <string>
#include "../../src/assets/chariot_root_authoring49u.h"
#include "../../src/assets/vehicle_visual_authoring49r.h"
#include "../../evidence/chariotRoot49U/authoring/authoring_oracle49u.h"
#include "../../evidence/chariotRoot49U/authoring/pod01_chariot_fixture49u.h"
#include "../../evidence/vehicleVisual49R/authoring/pod01_visual_fixture49r.h"
#include "../../evidence/authoredGeometry49O/initialization/geometry_oracle_bridge49o.h"
#include <cfenv>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <new>

static std::uint64_t allocations=0;
[[gnu::noinline]] void* operator new(std::size_t n){if(void* p=std::malloc(n?n:1)){++allocations;return p;}throw std::bad_alloc();}
[[gnu::noinline]] void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void operator delete(void* p) noexcept {std::free(p);}
[[gnu::noinline]] void operator delete[](void* p) noexcept {std::free(p);}
[[gnu::noinline]] void operator delete(void* p,std::size_t) noexcept {std::free(p);}
[[gnu::noinline]] void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
using namespace rr::assets;
using namespace rr::game;
namespace O=chariot_authoring49u_oracle;
namespace F=chariot_authoring49u_fixture;
using K=ChariotRootAuthoredKey49U;
using E=ChariotRootAuthoringError49U;
using U=std::uint32_t;
static std::uint64_t checks=0,calls=0,rejections=0,cases=0,sensitivities=0,instructions=0,geometryInstructions=0;
static std::array<std::uint64_t,chariot_authoring49u_words::total_words> hits{};
static void require(bool b,const char* s){++checks;if(!b)throw std::runtime_error(s);}
static U bits(float f){return O::bits(f);}
static float fp(U w){return O::fp(w);}
static std::string file(const std::string& p){std::ifstream f(p,std::ios::binary);require(bool(f),"fixture open");return {std::istreambuf_iterator<char>(f),{}};}
static void merge(const O::Machine& m){instructions+=m.steps;for(std::size_t i=0;i<hits.size();++i)hits[i]+=m.hits[i];}
static constexpr U vehicle=0x1000;
static const float ChariotRootParameters49U::* fields[]={
 &ChariotRootParameters49U::cableForceA6C,&ChariotRootParameters49U::radiusForceA70,
 &ChariotRootParameters49U::collisionXYAA0,&ChariotRootParameters49U::collisionZAA4,
 &ChariotRootParameters49U::groundGainAA8,&ChariotRootParameters49U::groundTransitionGainAAC,
 &ChariotRootParameters49U::collisionBoundA98,&ChariotRootParameters49U::groundBoundA9C,
 &ChariotRootParameters49U::returnGainA7C,&ChariotRootParameters49U::restoringA74,
 &ChariotRootParameters49U::slideRestoringA78,&ChariotRootParameters49U::linearDampingA80,
 &ChariotRootParameters49U::angularDampingA84,&ChariotRootParameters49U::yawInputRangeA88,
 &ChariotRootParameters49U::yawAngleA8C,nullptr,
 &ChariotRootParameters49U::minAmplitudeA4C,&ChariotRootParameters49U::maxAmplitudeA50,
 &ChariotRootParameters49U::minTimeA3C,&ChariotRootParameters49U::maxTimeA40,
 &ChariotRootParameters49U::orientationScaleA60};
static_assert(std::size(fields)==chariotRootAuthoredKeyCount49U);
static bool read(const std::string& s,ChariotRootAuthoredValues49U& v,ChariotRootAuthoringDiagnostic49U& d,ChariotRootAuthoredProvenance49U* p=nullptr){
 const auto a=allocations;const bool ok=readChariotRootAuthoring49U(s.data(),s.size(),v,d,p);require(allocations==a,"reader allocation");++calls;return ok;
}
static bool bind(const ChariotRootAuthoredValues49U& v,const FreshPodGeometryResult49O& g,const VehicleVisualParameters49R& e,ChariotRootParameters49U& p,ChariotRootAuthoringDiagnostic49U& d,float* a90=nullptr){
 const auto a=allocations;const bool ok=bindFreshChariotRootParameters49U(v,g,e,p,d,a90);require(allocations==a,"binder allocation");++calls;return ok;
}
static void sourceConstructor(O::Machine& m){
 m.call(0x3c9f70,0);m.write(O::Machine::stack+0xac,vehicle);
 m.interval(0x20f700,0x20f7f0);m.interval(0x20fb60,0x20fc68);
 m.interval(0x20ff4c,0x20ff58);m.interval(0x20f694,0x20f6b0);
}
static void constructor(){
 O::Machine m;sourceConstructor(m);ChariotRootState49U s{};ChariotRootShared49U shared{};float phase=123;
 std::memset(&s,0xa5,sizeof s);std::memset(&shared,0xa5,sizeof shared);
 const auto a=allocations;initializeChariotRootConstructorRecords49U(s,phase);initializeChariotRootSharedColdRecords49U(shared);
 require(allocations==a,"constructor allocation");calls+=2;
 require(s.vehicle838==m.read(vehicle+0x838)&&s.vehicle83C==m.read(vehicle+0x83c)&&s.vehicle840==m.read(vehicle+0x840),"constructor mode words");
 require(bits(s.previous85C)==m.read(vehicle+0x85c)&&bits(phase)==m.read(vehicle+0x864),"constructor history words");
 const auto equal=[&](const VisualVector49R& v,U p){for(unsigned i=0;i<4;++i)require(bits(v.v[i])==m.read(p+4*i),"constructor vector words");};
 equal(s.response.position,vehicle+0x870);equal(s.response.linearVelocity,vehicle+0x880);equal(s.response.angularVelocity,vehicle+0x890);equal(shared.up434650,0x434650);
 for(unsigned i=0;i<4;++i)require(bits(s.response.orientation.v[i])==m.read(vehicle+0x8a0+4*i),"constructor quaternion words");
 require(bits(s.response.position.v[0])==0x80000000u&&bits(s.response.position.v[1])==0x80000000u,"negative zero constructor witness");
 require(shared.speed434750.guard==0&&shared.speed434750.word==0&&shared.yaw434758.guard==0&&shared.yaw434758.word==0,"cold BSS fields");merge(m);
}
struct Assets {std::string chariot,right,left,forward,back,vut;};
static Assets assets(const std::string& root){const auto p=root+"/evidence/authoredGeometry49O/asset-reader/fixtures/";return {file(p+"52-chariot.psg"),file(p+"43-engrt1.psg"),file(p+"48-englt1.psg"),file(p+"56-01_forwd.mot"),file(p+"57-01_back.mot"),file(p+"53-chariot.vut")};}
static PsgHierarchyView49O psg(const std::string& s){PsgHierarchyView49O v{};GeometryDiagnostic49O d{};require(readPsgHierarchy49O(reinterpret_cast<const std::uint8_t*>(s.data()),s.size(),v,d),"PSG read");return v;}
static FreshPodGeometryResult49O geometry(const Assets& a){
 FreshPodGeometrySources49O s{};s.chariot=psg(a.chariot);s.rightEngine=psg(a.right);GeometryDiagnostic49O gd{};MotionBindingDiagnostic49O bd{};
 require(readMot49O(reinterpret_cast<const std::uint8_t*>(a.forward.data()),a.forward.size(),s.forward,gd),"forward MOT");
 require(readMot49O(reinterpret_cast<const std::uint8_t*>(a.back.data()),a.back.size(),s.back,gd),"back MOT");
 require(readMotionNodeBindings49O(s.chariot,a.vut.data(),a.vut.size(),s.chariotBindings,bd),"source VUT binding");
 FreshPodGeometryWorkspace49O w{};FreshPodGeometryResult49O out{};FreshPodGeometryDiagnostic49O d{};
 const auto count=allocations;require(buildFreshPodGeometry49O(s,w,out,d),"actual fresh49O geometry");require(count==allocations,"fresh geometry allocated");
 auto expected=initialization49o_fixture::execute(out.sourceInputs);geometryInstructions+=expected.instructions;
 require(bits(out.parameters.length854)==bits(expected.binding.length854),"actual49O length vs original words");return out;
}
static VehicleVisualParameters49R visual(const std::string& phy,const Assets& a){VisualAuthoredValues49R raw{};VisualAuthoringDiagnostic49R d{};VehicleVisualParameters49R p{};
 require(readVehicleVisualAuthoring49R(phy.data(),phy.size(),raw,d),"actual49R reader");require(bindFreshVehicleVisualParameters49R(raw,psg(a.right),psg(a.left),p,d),"actual49R binding");return p;}
static ChariotRootParameters49U compare(const ChariotRootAuthoredValues49U& raw,const FreshPodGeometryResult49O& geometry,const VehicleVisualParameters49R& visual,bool exact=true){
 ChariotRootParameters49U p{};ChariotRootAuthoringDiagnostic49U d{};float a90=123;require(bind(raw,geometry,visual,p,d,&a90),"finite binder rejection");require(d.error==E::none,"success diagnostic");
 O::Machine m;sourceConstructor(m);
 for(const auto& v:visual_authoring49r_fixture::all)m.parsed[std::string(v.section)+"/"+v.key]=v.word;
 for(unsigned i=0;i<chariotRootAuthoredKeyCount49U;++i)m.parsed[std::string("Pod Animation/")+F::keys[i]]=bits(raw.values[i]);
 m.g[20][0]=0x7000;m.g[21][0]=vehicle;m.interval(0x2154d8,0x215900);
 for(unsigned i=0;i<chariotRootAuthoredKeyCount49U;++i){const float n=fields[i]?p.*fields[i]:a90;require(bits(n)==m.read(vehicle+F::destinations[i]),"source scalar destination or conversion mismatch");bool found=false;
  for(const auto& call:m.boundaries){if(call.target==0x158a70&&call.key==F::keys[i]){require(call.destination==vehicle+F::destinations[i],"original lookup destination");found=true;}}
  require(found,"source key not consumed");}
 require(bits(p.orientationGainA94)==m.read(vehicle+0xa94),"A94 original constructor, not unused authored gain");
 require(bits(p.restLength854)==bits(geometry.parameters.length854),"49O radius projected incorrectly");
 require(bits(p.effectiveSpeedSquaredA54)==bits(visual.effectiveSpeedSquaredA54),"49R effective denominator reprocessed");
 if(exact)require(bits(p.effectiveSpeedSquaredA54)==m.read(vehicle+0xa54),"actual source A54 denominator");
 for(const auto& call:m.boundaries)require(call.key!="ChariotOrientationGain","unconsumed authored gain loaded");
 merge(m);++cases;return p;
}
static std::string replace(std::string s,const char* key,const std::string& token){const auto at=s.find(std::string("\n")+key+" =");require(at!=std::string::npos,"token replacement key");const auto begin=s.find('=',at)+1,end=s.find_first_of("\r\n",begin);s.replace(begin,end-begin,token);return s;}
static void rejectText(const std::string& text,PodPhysicsError49N error){
 ChariotRootAuthoredValues49U output;std::memset(&output,0xa5,sizeof output);const auto old=output;
 ChariotRootAuthoredProvenance49U provenance;std::memset(&provenance,0x5a,sizeof provenance);const auto before=provenance;ChariotRootAuthoringDiagnostic49U d{};
 require(!read(text,output,d,&provenance),"invalid text accepted");require(d.text.code==error,"text error code");require(!std::memcmp(&output,&old,sizeof old)&&!std::memcmp(&provenance,&before,sizeof before),"reader partial commit");++rejections;
}
static ChariotRootAuthoredValues49U textCases(const std::string& phy,const FreshPodGeometryResult49O& g,const VehicleVisualParameters49R& v){
 ChariotRootAuthoredValues49U actual{};ChariotRootAuthoredProvenance49U provenance{};ChariotRootAuthoringDiagnostic49U d{};require(read(phy,actual,d,&provenance),"actual Pod01 read");
 std::string minimal="; original selected tokens\r\n[pOd aNiMaTiOn]\r\n";
 for(unsigned i=0;i<chariotRootAuthoredKeyCount49U;++i){const visual_authoring49r_fixture::Value* e=nullptr;for(const auto& row:visual_authoring49r_fixture::all)if(!std::strcmp(row.key,F::keys[i]))e=&row;
  require(e!=nullptr,"original token absent");require(bits(actual.values[i])==e->word,"inherited49N source scalar mismatch");const auto& t=provenance.tokens[i];require(t.keyOffset==e->keyOffset&&t.valueOffset==e->valueOffset&&t.valueBytes==e->valueBytes,"PHY byte provenance");require(phy.substr(t.valueOffset,t.valueBytes)==e->token,"token roundtrip");minimal+=std::string(F::keys[i])+" = \t"+e->token+" \r\n";}
 ChariotRootAuthoredValues49U round{};require(read(minimal,round,d),"minimal mixed-case reader");require(!std::memcmp(&round,&actual,sizeof actual),"roundtrip mismatch");
 const auto baseline=compare(actual,g,v);
 for(unsigned i=0;i<chariotRootAuthoredKeyCount49U;++i){auto text=replace(minimal,F::keys[i],"1.25");ChariotRootAuthoredValues49U changed{};require(read(text,changed,d),"mutated token read");require(bits(changed.values[i])==0x3fa00000u,"edited token ignored");for(unsigned j=0;j<chariotRootAuthoredKeyCount49U;++j)if(j!=i)require(bits(changed.values[j])==bits(actual.values[j]),"edited token changed other key");
  const auto result=compare(changed,g,v);if(fields[i])require(bits(result.*fields[i])!=bits(baseline.*fields[i]),"edited field projection ignored");++sensitivities;
  auto missing=minimal;const auto at=missing.find(std::string("\n")+F::keys[i]+" =")+1;missing.erase(at,missing.find('\n',at)+1-at);rejectText(missing,PodPhysicsError49N::missing_key);}
 for(const char* s:{"NaN","inf","1e3",".5","1.","1f","1;bad","0x1","1 2","1000000000","0.0000001","--1"})rejectText(replace(minimal,"CableForce",s),PodPhysicsError49N::numeric_syntax);
 rejectText(minimal+"cableforce=1\n",PodPhysicsError49N::duplicate_key);rejectText(minimal+"[POD ANIMATION]\n",PodPhysicsError49N::duplicate_section);
 rejectText("a=1\n"+minimal,PodPhysicsError49N::syntax);rejectText(std::string(32769,'a'),PodPhysicsError49N::text_limit);rejectText(";"+std::string(255,'a')+"\n"+minimal,PodPhysicsError49N::line_limit);
 auto bad=minimal;bad[0]='\0';rejectText(bad,PodPhysicsError49N::invalid_character);bad[0]=char(0xff);rejectText(bad,PodPhysicsError49N::invalid_character);
 std::string sections=minimal;for(unsigned i=0;i<32;++i)sections+="[Unused"+std::to_string(i)+"]\n";rejectText(sections,PodPhysicsError49N::section_limit);
 std::string many=minimal;for(unsigned i=0;i<492;++i)many+="Unused"+std::to_string(i)+"=abc\n";rejectText(many,PodPhysicsError49N::key_limit);
 require(!readChariotRootAuthoring49U(nullptr,1,round,d),"null input accepted");require(!readChariotRootAuthoring49U("",0,round,d),"empty input accepted");
 const auto ignored=replace(phy,"ChariotOrientationGain","999999999");require(read(ignored,round,d),"ignored value rejected");require(!std::memcmp(&round,&actual,sizeof actual),"ignored gain consumed");compare(round,g,v);++sensitivities;
 for(int mode:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){require(std::fesetround(mode)==0,"set rounding");require(read(phy,round,d),"integer scalar parser host rounding");require(!std::memcmp(&round,&actual,sizeof actual),"host mode changed scalar words");}require(std::fesetround(FE_TONEAREST)==0,"restore rounding");return actual;
}
static void rejectBind(const ChariotRootAuthoredValues49U& raw,const FreshPodGeometryResult49O& g,const VehicleVisualParameters49R& v,E error){
 ChariotRootParameters49U p;std::memset(&p,0xa5,sizeof p);const auto before=p;float a90=fp(0x7fc12345u);ChariotRootAuthoringDiagnostic49U d{};
 require(!bind(raw,g,v,p,d,&a90),"invalid binding accepted");require(d.error==error,"binding rejection code");require(!std::memcmp(&p,&before,sizeof p)&&bits(a90)==0x7fc12345u,"binding partial commit");++rejections;
}
static void bindingCases(const ChariotRootAuthoredValues49U& actual,const FreshPodGeometryResult49O& g,const VehicleVisualParameters49R& v){
 U rng=0x49u;for(unsigned c=0;c<256;++c){auto raw=actual;for(float& f:raw.values){rng=rng*1664525u+1013904223u;f=float(int(rng%2000001)-1000000)/128;}compare(raw,g,v);}
 for(unsigned i=0;i<chariotRootAuthoredKeyCount49U;++i)for(U word:{0x7f800000u,0xff800000u,0x7fc12345u}){auto raw=actual;raw.values[i]=fp(word);rejectBind(raw,g,v,E::nonfinite);}
 for(U word:{0x7f800000u,0xff800000u,0x7fc12345u,0xbf800000u}){auto bad=g;bad.parameters.length854=fp(word);rejectBind(actual,bad,v,E::geometry);}
 for(U word:{0x7f800000u,0xff800000u,0x7fc12345u}){auto bad=v;bad.effectiveSpeedSquaredA54=fp(word);rejectBind(actual,g,bad,E::nonfinite);}
 for(int mode:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){require(std::fesetround(mode)==0,"set binder rounding");rejectBind(actual,g,v,E::rounding_mode);}require(std::fesetround(FE_TONEAREST)==0,"restore binder rounding");
 auto raw=actual;for(float& f:raw.values)f=fp(0x80000000u);compare(raw,g,v);
 auto b=g;b.parameters.length854=0;compare(actual,b,v);b.parameters.length854=100;compare(actual,b,v,false);
 auto visual=v;visual.effectiveSpeedSquaredA54=17;const auto p=compare(actual,g,visual,false);require(bits(p.effectiveSpeedSquaredA54)==bits(17.f),"already converted denominator changed");++sensitivities;
 raw=actual;raw.values[13]=std::numeric_limits<float>::max();raw.values[14]=-std::numeric_limits<float>::max();raw.values[15]=std::numeric_limits<float>::max();compare(raw,g,v);
}
int main(int argc,char** argv){try{
 require(argc==2||argc==3,"usage: chariot_root_authoring49u root [coverage.json]");require(std::fesetround(FE_TONEAREST)==0,"nearest unavailable");const std::string root=argv[1];const auto phy=file(root+"/evidence/authoredInitialization49N/authoring-source/Pod01.phy");
 const auto a=assets(root);const auto g=geometry(a);require(bits(g.parameters.length854)==0x41a64687u,"actual854 word");const auto v=visual(phy,a);constructor();const auto raw=textCases(phy,g,v);bindingCases(raw,g,v);
 auto altered=a;const U z=0x41200000u;for(unsigned i=0;i<4;++i)altered.right[196+i]=char(z>>(8*i));const auto changed=geometry(altered);require(bits(changed.parameters.length854)!=bits(g.parameters.length854),"authored nullcg change ineffective");compare(raw,changed,v);++sensitivities;
 std::size_t covered=0;for(auto n:hits)covered+=n!=0;require(covered==hits.size(),"unexecuted authoring or constructor original word");
 if(argc==3){std::ofstream o(argv[2]);require(bool(o),"coverage output");o<<"{\"blocks\":[";bool first=true;for(const auto& b:chariot_authoring49u_words::blocks){if(!first)o<<',';first=false;o<<"{\"start\":"<<b.start<<",\"hits\":[";for(unsigned i=0;i<b.count;++i){if(i)o<<',';o<<hits[b.index+i];}o<<"]}";}o<<"]}\n";}
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"native_calls\":"<<calls<<",\"native_allocations\":0,\"binder_cases\":"<<cases<<",\"rejections\":"<<rejections<<",\"sensitivity_cases\":"<<sensitivities<<",\"original_instructions\":"<<instructions<<",\"geometry_original_instructions\":"<<geometryInstructions<<",\"original_covered_words\":"<<covered<<",\"length854\":\"41a64687\",\"orientationGainA94\":\"c3960000\"}\n";return 0;
 }catch(const std::exception& e){std::cerr<<"chariot_root_authoring49u: "<<e.what()<<'\n';return 1;}}
