#include "../../src/assets/vehicle_visual_authoring49r.h"
#include "../../evidence/vehicleVisual49R/authoring/authoring_oracle49r.h"
#include "../../evidence/vehicleVisual49R/authoring/pod01_visual_fixture49r.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <vector>

static std::uint64_t allocations=0;
void* operator new(std::size_t n){if(void* p=std::malloc(n?n:1)){++allocations;return p;}throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete[](void* p) noexcept {std::free(p);}
void operator delete(void* p,std::size_t) noexcept {std::free(p);}
void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
using namespace rr::assets;
using namespace rr::game;
namespace O=visual_authoring49r_oracle;
namespace F=visual_authoring49r_fixture;
using U=std::uint32_t;
using K=VisualAuthoredKey49R;
using E=VisualAuthoringError49R;
static std::uint64_t checks=0,calls=0,rejections=0,originalInstructions=0,binderCases=0,roundTrips=0,sensitivities=0,worldGetterCases=0;
static std::array<std::uint64_t,visual_authoring49r_words::total_words> hits{};
static void require(bool yes,const char* message){++checks;if(!yes)throw std::runtime_error(message);}
static U bits(float value){return O::bits(value);}
static float fp(U value){return O::fp(value);}
static void merge(const O::Machine& m){originalInstructions+=m.steps;for(std::size_t i=0;i<hits.size();++i)hits[i]+=m.hits[i];}
static std::string file(const std::string& name){std::ifstream f(name,std::ios::binary);require(bool(f),"fixture open failed");return {std::istreambuf_iterator<char>(f),{}};}
static bool read(const std::string& text,VisualAuthoredValues49R& out,VisualAuthoringDiagnostic49R& d,VisualAuthoredProvenance49R* p=nullptr){
 const auto before=allocations;const bool ok=readVehicleVisualAuthoring49R(text.data(),text.size(),out,d,p);require(allocations==before,"reader allocated");++calls;return ok;
}
static bool bind(const VisualAuthoredValues49R& values,const PsgHierarchyView49O& right,const PsgHierarchyView49O& left,VehicleVisualParameters49R& out,VisualAuthoringDiagnostic49R& d){
 const auto before=allocations;const bool ok=bindFreshVehicleVisualParameters49R(values,right,left,out,d);require(allocations==before,"binder allocated");++calls;return ok;
}
static const float VehicleVisualParameters49R::* fields[]={
 &VehicleVisualParameters49R::impact9F0,&VehicleVisualParameters49R::groundImpact9F4,&VehicleVisualParameters49R::separation9F8,&VehicleVisualParameters49R::thrust9FC,
 &VehicleVisualParameters49R::liftA00,&VehicleVisualParameters49R::dragA04,&VehicleVisualParameters49R::springA08,&VehicleVisualParameters49R::dampingA0C,&VehicleVisualParameters49R::binderSpringA10,
 &VehicleVisualParameters49R::yawA14,&VehicleVisualParameters49R::slideYawA18,&VehicleVisualParameters49R::slidePitchA1C,&VehicleVisualParameters49R::pitchA20,&VehicleVisualParameters49R::rollA24,
 &VehicleVisualParameters49R::angleReturnA28,&VehicleVisualParameters49R::angularDampingA2C,&VehicleVisualParameters49R::binderTorqueA30,
 &VehicleVisualParameters49R::minTimeA34,&VehicleVisualParameters49R::maxTimeA38,&VehicleVisualParameters49R::minAmplitudeA44,&VehicleVisualParameters49R::maxAmplitudeA48,
 &VehicleVisualParameters49R::effectiveSpeedSquaredA54,&VehicleVisualParameters49R::damageTranslationA58,&VehicleVisualParameters49R::damageOrientationA5C,
 &VehicleVisualParameters49R::orientationScaleA64,&VehicleVisualParameters49R::effectiveInputSpeedA68,&VehicleVisualParameters49R::verticalLimit7EC};
static_assert(std::size(fields)==visualAuthoredKeyCount49R);
static U sourceVehicle=0x1000;
static void initializeSource(O::Machine& m){
 m.call(0x3c9f70,0); // Source vector static initializer including all stores.
 m.write(O::Machine::stack+0xac,sourceVehicle);m.g[1][0]=0x430000;
 m.interval(0x20fba0,0x20fbc0);m.interval(0x20fc68,0x20fd48);
 m.g[1][0]=0x430000;m.interval(0x210000,0x210010);
 m.g[1][0]=0x430000;m.interval(0x210060,0x210070);
}
static void constructor(){
 O::Machine m;initializeSource(m);VehicleVisualState49R s{};VisualShared49R shared{};
 const auto before=allocations;initializeVehicleVisualConstructorRecords49R(s);initializeVehicleVisualSharedColdRecords49R(shared);
 require(allocations==before,"constructor records allocated");calls+=2;
 const auto vector=[&](const VisualVector49R& v,U source){for(unsigned i=0;i<4;++i)require(bits(v.v[i])==m.read(source+4*i),"constructor vector word");};
 vector(s.right.position,sourceVehicle+0x910);vector(s.right.linearVelocity,sourceVehicle+0x920);vector(s.right.angularVelocity,sourceVehicle+0x930);
 vector(s.left.position,sourceVehicle+0x8c0);vector(s.left.linearVelocity,sourceVehicle+0x8d0);vector(s.left.angularVelocity,sourceVehicle+0x8e0);
 for(unsigned i=0;i<4;++i){require(bits(s.right.orientation.v[i])==m.read(sourceVehicle+0x940+4*i),"right constructor quaternion");require(bits(s.left.orientation.v[i])==m.read(sourceVehicle+0x8f0+4*i),"left constructor quaternion");}
 vector(s.upC20,sourceVehicle+0xc20);vector(s.publishedUpAF0,sourceVehicle+0xaf0);vector(shared.zero434630,0x434630);
 require(bits(s.phase8B0)==m.read(sourceVehicle+0x8b0)&&bits(s.phase900)==m.read(sourceVehicle+0x900)&&bits(s.phase950)==m.read(sourceVehicle+0x950)&&bits(s.phase9A0)==m.read(sourceVehicle+0x9a0),"constructor clocks");
 for(const auto& g:shared.globals)require(g.guard==0&&g.word==0,"graphics cold globals");
 require(shared.orientationSentinel4433D8.guard==0&&shared.orientationSentinel4433D8.word==0,"graphics cold sentinel");
 require(bits(s.right.position.v[3])==0x3f800000&&bits(s.left.angularVelocity.v[3])==0x3f800000,"constructor W mutation control");merge(m);
}
static void scalarSource(O::Machine& m,const VisualAuthoredValues49R& raw){
 for(const auto& r:F::all)m.parsed[std::string(r.section)+"/"+r.key]=r.word;
 for(std::size_t i=0;i<visualAuthoredKeyCount49R;++i)m.parsed[std::string(i==26?"Repulsor":"Pod Animation")+"/"+F::keys[i]]=bits(raw.values[i]);
 m.g[20][0]=0x7000;m.g[21][0]=sourceVehicle;
 m.interval(0x2154d8,0x215900);m.interval(0x214894,0x2148b8);
}
static std::array<U,4> sourceOffset(O::Machine& m,const PsgHierarchyView49O& view,bool right,U inheritedW=0x3f800000){
 PsgNode49O node{};require(readPsgNode49O(view,0,node),"oracle PSG node");
 const U parent=O::Machine::sceneA,child=O::Machine::sceneB,destination=sourceVehicle+(right?0x2c0:0x2b0);
 for(unsigned i=0;i<16;++i){m.store(parent+0xc0+4*i,i==0||i==5||i==10||i==15?1:0);m.store(child+0xc0+4*i,node.local[i]);}
 m.write(parent+0x180,0);m.write(parent+0x190,0x80);m.write(child+0x180,parent);m.write(child+0x190,0xa4);
 m.write(destination+12,inheritedW);m.call(0x28b1a0,child,0,destination); // Real getter, including lazy original world product.
 require(m.read(destination+12)==inheritedW,"getter altered caller W");
 if(right){m.g[22][0]=sourceVehicle;m.interval(0x210cf0,0x210d08);}else{m.g[22][0]=sourceVehicle;m.interval(0x210fb4,0x210fbc);}
 ++worldGetterCases;return {m.read(destination),m.read(destination+4),m.read(destination+8),m.read(destination+12)};
}
static VehicleVisualParameters49R compare(const VisualAuthoredValues49R& raw,const PsgHierarchyView49O& right,const PsgHierarchyView49O& left){
 VehicleVisualParameters49R p{};VisualAuthoringDiagnostic49R d{};require(bind(raw,right,left,p,d),"finite binding rejected");require(d.error==E::none,"binding success diagnostic");
 O::Machine m;scalarSource(m,raw);
 for(std::size_t i=0;i<visualAuthoredKeyCount49R;++i)require(bits(p.*fields[i])==m.read(sourceVehicle+F::destinations[i]),"authored source destination mismatch");
 const auto r=sourceOffset(m,right,true),l=sourceOffset(m,left,false);
 for(unsigned i=0;i<4;++i){require(bits(p.rightBase2C0.v[i])==r[i],"right source world/Z clear mismatch");require(bits(p.leftBase2B0.v[i])==l[i],"left source world/Z clear mismatch");}
 for(std::size_t i=0;i<visualAuthoredKeyCount49R;++i){bool seen=false;for(const auto& b:m.boundaries)if(b.target==0x158a70&&b.key==F::keys[i]){require(b.destination==sourceVehicle+F::destinations[i],"source lookup mapped to wrong field");seen=true;}require(seen,"mapped scalar not consumed by original loader");}
 merge(m);++binderCases;return p;
}
static void rejectText(const std::string& text,PodPhysicsError49N expected){
 VisualAuthoredValues49R out;std::memset(&out,0xa5,sizeof(out));const auto old=out;
 VisualAuthoredProvenance49R audit;std::memset(&audit,0x5a,sizeof(audit));const auto oldAudit=audit;VisualAuthoringDiagnostic49R d{};
 require(!read(text,out,d,&audit),"unsupported PHY accepted");require(d.text.code==expected,"wrong reader rejection code");
 require(std::memcmp(&out,&old,sizeof(out))==0&&std::memcmp(&audit,&oldAudit,sizeof(audit))==0,"failed reader committed output");++rejections;
}
static std::string replacement(std::string s,const char* key,const std::string& token){
 const auto at=s.find(std::string("\n  ")+key+" =");require(at!=std::string::npos,"replacement missing key");const auto begin=s.find('=',at)+1;const auto end=s.find_first_of("\r\n",begin);s.replace(begin,end-begin,token);return s;
}
static void textCases(const std::string& phy,VisualAuthoredValues49R& actual,const PsgHierarchyView49O& right,const PsgHierarchyView49O& left){
 VisualAuthoredProvenance49R audit{};VisualAuthoringDiagnostic49R d{};require(read(phy,actual,d,&audit),"actual Pod01 reader");
 std::string rebuilt="; selected original tokens with source provenance\r\n[pOd aNiMaTiOn]\r\n";
 for(std::size_t i=0;i<visualAuthoredKeyCount49R;++i){const F::Value* expected=nullptr;for(const auto& r:F::all)if(std::strcmp(r.key,F::keys[i])==0)expected=&r;
  require(expected!=nullptr,"missing actual original token");require(bits(actual.values[i])==expected->word,"49N scalar conversion original word");
  const auto& t=audit.tokens[i];require(t.keyOffset==expected->keyOffset&&t.valueOffset==expected->valueOffset&&t.valueBytes==expected->valueBytes,"actual byte provenance");
  require(phy.substr(t.valueOffset,t.valueBytes)==expected->token,"actual raw token roundtrip");
  if(i==26)rebuilt+="[rEpUlSoR]\r\n";
  rebuilt+="  "+std::string(F::keys[i])+" = \t"+phy.substr(t.valueOffset,t.valueBytes)+" \r\n";
 }
 VisualAuthoredValues49R round{};require(read(rebuilt,round,d),"rebuilt selected PHY");require(std::memcmp(&round,&actual,sizeof round)==0,"authored actual-token roundtrip mismatch");++roundTrips;
 const auto baseline=compare(actual,right,left);
 O::Machine intermediate;for(const auto& r:F::all)intermediate.parsed[std::string(r.section)+"/"+r.key]=r.word;
 intermediate.g[20][0]=0x7000;intermediate.g[21][0]=sourceVehicle;intermediate.interval(0x2154d8,0x2158ec);
 require(intermediate.read(sourceVehicle+0xa68)==0x4392aa7fu,"original actual A68 intermediate");
 intermediate.interval(0x2158ec,0x2158f0);require(bits(intermediate.f[0])==0x4412aa7fu,"first original A54 product");
 intermediate.interval(0x2158f0,0x2158f4);require(bits(intermediate.f[0])==0x48652a66u,"second original A54 product");
 intermediate.interval(0x2158f4,0x2158f8);require(bits(intermediate.f[0])==0x48a80dd5u,"third original A54 product");merge(intermediate);
 require(bits(baseline.effectiveSpeedSquaredA54)==0x48a80dd5u,"actual A54 effective word");require(bits(baseline.effectiveInputSpeedA68)==0x4392aa7fu,"actual A68 effective word");require(bits(baseline.verticalLimit7EC)==0x40a00000u,"actual Repulsor.MaxLiftHeight");
 require(bits(baseline.rightBase2C0.v[0])==0x409d9f95u&&bits(baseline.leftBase2B0.v[0])==0xc09d9f95u,"actual nullcg X");
 require(bits(baseline.rightBase2C0.v[2])==0&&bits(baseline.leftBase2B0.v[2])==0,"actual Z clear");
 for(std::size_t i=0;i<visualAuthoredKeyCount49R;++i){
  auto edited=replacement(rebuilt,F::keys[i],"1.25");VisualAuthoredValues49R raw{};require(read(edited,raw,d),"actual token sensitivity parse");
  require(bits(raw.values[i])==0x3fa00000u&&bits(raw.values[i])!=bits(actual.values[i]),"edited original bytes ignored");
  for(std::size_t j=0;j<visualAuthoredKeyCount49R;++j)if(j!=i)require(bits(raw.values[j])==bits(actual.values[j]),"token changed another raw field");
  const auto p=compare(raw,right,left);require(bits(p.*fields[i])!=bits(baseline.*fields[i]),"field ignored actual token mutation");++sensitivities;
  auto missing=rebuilt;const auto at=missing.find(std::string("\n  ")+F::keys[i]+" =")+1;const auto begin=missing.rfind('\n',at)+1;missing.erase(begin,missing.find('\n',at)+1-begin);rejectText(missing,PodPhysicsError49N::missing_key);
 }
 for(const char* s:{"NaN","nan","inf","Infinity","1e3","1E-3",".5","1.","1f","1;bad","0x1","1 2","1000000000","0.0000001","--1"})rejectText(replacement(rebuilt,"ImpactForce",s),PodPhysicsError49N::numeric_syntax);
 rejectText(rebuilt+"MaxLiftHeight=1\n",PodPhysicsError49N::duplicate_key);
 rejectText(rebuilt+"[repulsor]\nMaxLiftHeight=1\n",PodPhysicsError49N::duplicate_section);
 rejectText("a=1\n"+rebuilt,PodPhysicsError49N::syntax);rejectText(std::string(32769,'a'),PodPhysicsError49N::text_limit);
 rejectText(";"+std::string(255,'a')+"\n"+rebuilt,PodPhysicsError49N::line_limit);
 auto bad=rebuilt;bad[0]='\0';rejectText(bad,PodPhysicsError49N::invalid_character);bad[0]=char(0xff);rejectText(bad,PodPhysicsError49N::invalid_character);
 std::string sections=rebuilt;for(unsigned i=0;i<31;++i)sections+="[Unused"+std::to_string(i)+"]\n";rejectText(sections,PodPhysicsError49N::section_limit);
 std::string many=rebuilt;for(unsigned i=0;i<486;++i)many+="Unused"+std::to_string(i)+"=abc\n";rejectText(many,PodPhysicsError49N::key_limit);
 VisualAuthoredValues49R none{};require(!readVehicleVisualAuthoring49R(nullptr,1,none,d),"null text admitted");require(!readVehicleVisualAuthoring49R("",0,none,d),"empty text admitted");
 for(int mode:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){require(std::fesetround(mode)==0,"set rounding mode");VisualAuthoredValues49R r{};require(read(phy,r,d),"integer parser rejected host rounding mode");require(std::memcmp(&r,&actual,sizeof r)==0,"host rounding changed original scalar conversion");}
 require(std::fesetround(FE_TONEAREST)==0,"restore rounding mode");
}
static void putWord(std::string& data,std::size_t at,U word){require(at+4<=data.size(),"fixture write range");for(unsigned i=0;i<4;++i)data[at+i]=char(word>>(8*i));}
static PsgHierarchyView49O view(const std::string& bytes){PsgHierarchyView49O v{};GeometryDiagnostic49O d{};require(readPsgHierarchy49O(reinterpret_cast<const std::uint8_t*>(bytes.data()),bytes.size(),v,d),"PSG view rejected");return v;}
static void binderReject(const VisualAuthoredValues49R& raw,const PsgHierarchyView49O& right,const PsgHierarchyView49O& left,E expected){
 VehicleVisualParameters49R p;std::memset(&p,0x5a,sizeof p);const auto prior=p;VisualAuthoringDiagnostic49R d{};
 require(!bind(raw,right,left,p,d),"bad binder input accepted");require(d.error==expected,"wrong binder error");require(std::memcmp(&p,&prior,sizeof p)==0,"failed binder committed output");++rejections;
}
static void parameterCases(const VisualAuthoredValues49R& actual,const PsgHierarchyView49O& right,const PsgHierarchyView49O& left,const std::string& rightBytes){
 for(float rawSpeed:{0.f,-0.f,1.f,-1.f,.01f,.1f,3.f,17.f,300.f,400.f,401.f,12345.25f,999999.f}){
  auto raw=actual;raw.values[21]=rawSpeed;raw.values[25]=-rawSpeed;compare(raw,right,left);
 }
 U rng=0x49ab1234u;
 for(unsigned trial=0;trial<256;++trial){auto raw=actual;for(auto& x:raw.values){rng=rng*1664525u+1013904223u;x=float(int(rng%2000001)-1000000)/128;}compare(raw,right,left);}
 for(std::size_t i=0;i<visualAuthoredKeyCount49R;++i)for(U word:{0x7f800000u,0xff800000u,0x7fc12345u}){auto raw=actual;raw.values[i]=fp(word);binderReject(raw,right,left,E::nonfinite);}
 for(std::size_t i:{std::size_t(21),std::size_t(25)}){auto raw=actual;raw.values[i]=std::numeric_limits<float>::max();binderReject(raw,right,left,E::arithmetic);}
 for(int mode:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){require(std::fesetround(mode)==0,"rounding mode set");binderReject(actual,right,left,E::rounding_mode);}require(std::fesetround(FE_TONEAREST)==0,"rounding mode restore");
 auto invalid=right;invalid.nodeCount=0;binderReject(actual,invalid,left,E::hierarchy);
 std::string renamed=rightBytes;renamed[12]='x';const auto named=view(renamed);binderReject(actual,named,left,E::nullcg);
 // Source local-to-world arithmetic turns this local -0 into +0; a direct
 // local XYZ copy is observably wrong even under an identity model root.
 auto changed=rightBytes;putWord(changed,12+128+4*12,0x80000000u);const auto signedZero=view(changed);
 auto p=compare(actual,signedZero,left);require(bits(p.rightBase2C0.v[0])==0,"identity world signed-zero source order");
 putWord(changed,12+128+4*12,0x41480000u);putWord(changed,12+128+4*13,0xc0200000u);putWord(changed,12+128+4*14,0x42f60000u);
 p=compare(actual,view(changed),left);require(bits(p.rightBase2C0.v[0])==0x41480000u&&bits(p.rightBase2C0.v[1])==0xc0200000u&&bits(p.rightBase2C0.v[2])==0,"actual PSG offset sensitivity");sensitivities+=2;
 O::Machine m;const auto unusual=sourceOffset(m,right,true,0x80000000u);require(unusual[3]==0x80000000u,"getter did not preserve supplied W");merge(m);
 // Mutation witnesses are source-vs-wrong-formula comparisons, never goldens.
 const float k=fp(0x3fbbbb84u);bool squaredWitness=false;for(unsigned i=1;i<4096&&!squaredWitness;++i){const float x=float(i)/7;auto raw=actual;raw.values[21]=x;const auto p0=compare(raw,right,left);const float first=O::mul(k,x);squaredWitness=bits(O::mul(first,first))!=bits(p0.effectiveSpeedSquaredA54);}require(squaredWitness,"A54 multiply-order mutation lacked witness");
}
int main(int argc,char** argv){try{
 require(argc==2||argc==3,"usage: vehicle_visual_authoring49r source-root [coverage.json]");require(std::fesetround(FE_TONEAREST)==0,"nearest-even rounding unavailable");const std::string root=argv[1];
 const auto phy=file(root+"/evidence/authoredInitialization49N/authoring-source/Pod01.phy");
 const auto rightBytes=file(root+"/evidence/authoredGeometry49O/asset-reader/fixtures/43-engrt1.psg"),leftBytes=file(root+"/evidence/authoredGeometry49O/asset-reader/fixtures/48-englt1.psg");
 const auto right=view(rightBytes),left=view(leftBytes);constructor();VisualAuthoredValues49R actual{};textCases(phy,actual,right,left);parameterCases(actual,right,left,rightBytes);
 std::size_t covered=0;for(const auto n:hits)covered+=n!=0;
 if(argc==3){std::ofstream report(argv[2]);require(bool(report),"coverage output open");report<<"{\"scope\":\"Original authoring stores/postprocessing, constructor records, and fresh nullcg world getter; original scalar lookup is an explicit proved-conversion boundary. Unexecuted original getter alternatives are outside this fresh context.\",\"blocks\":[";
  bool firstBlock=true;for(const auto& block:visual_authoring49r_words::blocks){if(!firstBlock)report<<',';firstBlock=false;report<<"{\"start\":"<<block.start<<",\"end_exclusive\":"<<block.start+4*block.count<<",\"hits\":[";for(std::size_t i=0;i<block.count;++i){if(i)report<<',';report<<hits[block.index+i];}report<<"]}";}report<<"]}\n";}

 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"native_calls\":"<<calls<<",\"native_allocations\":0,\"binder_cases\":"<<binderCases<<",\"roundtrips\":"<<roundTrips<<",\"sensitivity_cases\":"<<sensitivities<<",\"rejections\":"<<rejections<<",\"world_getter_calls\":"<<worldGetterCases<<",\"original_instructions\":"<<originalInstructions<<",\"original_covered_words\":"<<covered<<",\"A54\":\"48a80dd5\",\"A68\":\"4392aa7f\",\"7EC\":\"40a00000\"}\n";return 0;
 }catch(const std::exception& e){std::cerr<<"vehicle_visual_authoring49r: "<<e.what()<<'\n';return 1;}}
