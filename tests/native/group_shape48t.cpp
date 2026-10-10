// Original135CF0 tag0 differential projection, atomic span guards and finite
// arithmetic boundaries. Direct11F680-only rows are explicitly outside this API.
#include "group_shape_fixture48t.h"
#include <cfenv>
#include <chrono>
#include <cmath>
#include <fstream>
#include <limits>
using namespace shape_test48t;
namespace {
using Status=GroupShapeStatus48T;
struct Fixture {
 GroupShapeSource48T source{};GroupShapeGeometry48T geo{};Vector3_48C translation{};
 GroupShapeState48T initial{},destination{};GroupShapeSphere48T sphere{};
 const Vector3_48C* translationInput=&translation;GroupShapeState48T* output=&destination;GroupShapeSphere48T* sphereOutput=&sphere;
 std::array<unsigned char,37> tailCanary{};
 explicit Fixture(const Json& row){
  const auto& words=row.at("shape_words");matrix(source.local40,words,16);geo=geometry(row.at("shape_geometry_words"));source.geometry=row.value("geometry_present",true)?&geo:nullptr;initial=state(words);
  for(unsigned k=0;k<3;++k){volatile float v=static_cast<float>(row.at("position")[k].get<std::uint16_t>())*fp(row.at("group_scale").get<std::uint32_t>());translation.v[k]=v;}
  std::memset(static_cast<void*>(&destination),0xa5,sizeof(destination));std::memset(static_cast<void*>(&sphere),0x6d,sizeof(sphere));tailCanary.fill(0xc7);
 }
 std::vector<Snapshot> snapshots(bool outputs)const{std::vector<Snapshot> s;s.emplace_back(&source,sizeof(source));s.emplace_back(&geo,sizeof(geo));s.emplace_back(&translation,sizeof(translation));s.emplace_back(&initial,sizeof(initial));s.emplace_back(tailCanary.data(),tailCanary.size());if(outputs){s.emplace_back(&destination,sizeof(destination));s.emplace_back(&sphere,sizeof(sphere));}return s;}
 Status call(){const auto n=allocations();watching=true;const auto result=setGroupShape0Bounded48T(source,*translationInput,initial,*output,*sphereOutput);watching=false;check(allocations()==n,"no ordinary/array/aligned allocation inside helper");return result;}
 void reject(Status wanted){const auto saved=snapshots(true);check(call()==wanted,"expected atomic helper rejection status");for(const auto& s:saved)s.verify();++safety;}
};
bool supported(const Json& row){return row.at("entry")=="group_member"&&row.at("member_words")[34].get<std::uint32_t>()==0&&row.value("shape_present",true);}
Status wanted(const Json& row){if(!row.value("geometry_present",true))return Status::MissingGeometry;if(row.value("native_status",std::string("Complete"))=="NonFinite")return Status::NonFinite;return Status::Complete;}
void compare(const Fixture& f,const Json& row){
 auto shape=row.at("shape_words").get<std::vector<std::uint32_t>>();project(shape,f.destination);check(shape==row.at("expected").at("shape_words").get<std::vector<std::uint32_t>>(),"all72 shape words match original, including every matrix lane and preserved words");
 auto member=row.at("member_words").get<std::vector<std::uint32_t>>();for(unsigned k=0;k<4;++k)member[24+k]=bits(f.sphere.center.v[k]);member[32]=bits(f.sphere.radius);check(member==row.at("expected").at("member_words").get<std::vector<std::uint32_t>>(),"all64 member words match original, including sphere W and untouched words");
 check(row.at("shape_geometry_words")==row.at("expected").at("shape_geometry_words"),"all9 geometry words preserved by original");
 for(unsigned k=0;k<16;++k)check(bits(f.destination.previousC0.v[k])==bits(f.destination.world80.v[k]),"previous cache sees prewritten new XYZ, preserves all old basis and W lanes");
}
void guards(const Json& base){
 auto test=[&](const char* name,auto mutate,Status status=Status::InvalidInput){label=name;Fixture f(base);mutate(f);f.reject(status);};
 test("null geometry",[](Fixture& f){f.source.geometry=nullptr;},Status::MissingGeometry);
 test("geometry misalignment",[](Fixture& f){f.source.geometry=reinterpret_cast<const GroupShapeGeometry48T*>(reinterpret_cast<std::uintptr_t>(&f.geo)+1);});
 test("geometry address wrap",[](Fixture& f){f.source.geometry=reinterpret_cast<const GroupShapeGeometry48T*>(std::numeric_limits<std::uintptr_t>::max()&~(alignof(GroupShapeGeometry48T)-1));});
 for(unsigned output=0;output<2;++output)for(unsigned input=0;input<5;++input){label="helper full-object overlap "+std::to_string(output)+"/"+std::to_string(input);Fixture f(base);void* p=input==0?static_cast<void*>(&f.source):input==1?static_cast<void*>(&f.translation):input==2?static_cast<void*>(&f.initial):input==3?static_cast<void*>(&f.geo):output==0?static_cast<void*>(&f.sphere):static_cast<void*>(&f.destination);if(output==0)f.output=static_cast<GroupShapeState48T*>(p);else f.sphereOutput=static_cast<GroupShapeSphere48T*>(p);f.reject(Status::InvalidInput);++overlaps;}
 // Every arithmetic input lane is reached; zero multipliers do not hide NaNs.
 for(unsigned lane=0;lane<16;++lane){label="local40 nonfinite lane "+std::to_string(lane);Fixture f(base);f.source.local40.v[lane]=fp(0x7fc12345);f.reject(Status::NonFinite);}
 for(unsigned lane=0;lane<16;++lane)if(lane<12||lane==15){label="world80 reached nonfinite lane "+std::to_string(lane);Fixture f(base);f.initial.world80.v[lane]=fp(0x7f800000);f.reject(Status::NonFinite);}
 for(unsigned lane=0;lane<3;++lane){label="translation nonfinite "+std::to_string(lane);Fixture f(base);f.translation.v[lane]=fp(0xff800000);f.reject(Status::NonFinite);}
 for(unsigned field=0;field<2;++field)for(unsigned lane=0;lane<3;++lane){label="geometry nonfinite "+std::to_string(field)+"/"+std::to_string(lane);Fixture f(base);(field?f.geo.localHalf:f.geo.localCenter).v[lane]=fp(0x7fc12345);f.reject(Status::NonFinite);}
 test("finite product overflow",[](Fixture& f){f.initial.world80.v[0]=std::numeric_limits<float>::max();f.source.local40.v[0]=2;},Status::NonFinite);
 test("finite half radius overflow",[](Fixture& f){f.geo.localHalf.v[2]=std::numeric_limits<float>::max();},Status::NonFinite);
 // Overwritten raw state is deliberately not part of the numeric domain.
 {label="unconsumed old matrices and overwritten XYZ may contain NaNs";Fixture f(base);for(auto& x:f.initial.composed00.v)x=fp(0x7fa12345);for(auto& x:f.initial.previousC0.v)x=fp(0xff800000);for(unsigned k=0;k<3;++k)f.initial.world80.v[12+k]=fp(0x7fc23456);const auto saved=f.snapshots(false);check(f.call()==Status::Complete,"unconsumed prior values stay lazy");compare(f,base);for(const auto& s:saved)s.verify();++safety;}
 // Borrowed immutable objects may share actual storage.
 {label="read-only translation/geometry subobject alias";Fixture f(base);f.translationInput=&f.geo.localCenter;const auto saved=f.snapshots(false);check(f.call()==Status::Complete,"real read-only subobject aliases accepted");for(const auto& s:saved)s.verify();++safety;}
}
}
int main(int argc,char** argv){try{
 if(argc<2){std::cerr<<"usage: group_shape48t helper-fixtures.json [report.json] [--differential-only]\n";return 2;}
 std::fesetround(FE_TONEAREST);std::ifstream stream(argv[1]);Json packet;stream>>packet;if(packet.at("format")!="initialized-group-shape48t-v1")throw std::runtime_error("unexpected helper fixture format");
 const Json* base=nullptr;std::size_t originalOnly=0,boundaries=0;
 for(const auto& row:packet.at("cases")){label=row.at("name");if(!supported(row)){++originalOnly;continue;}Fixture f(row);const auto expected=wanted(row);if(expected!=Status::Complete){f.reject(expected);++boundaries;continue;}const auto saved=f.snapshots(false);const auto got=f.call();check(got==Status::Complete,"source-executed supported group0 helper returns Complete");if(got==Status::Complete)compare(f,row);for(const auto& s:saved)s.verify();++differential;if(!base)base=&row;}
 if(!base)throw std::runtime_error("no supported group0 source-executed helper case");
 const bool only=argc>3&&std::string(argv[3])=="--differential-only";Json perf=nullptr;
 if(!only){guards(*base);Fixture f(*base);label="serial helper benchmark";constexpr unsigned n=25000;const auto start=std::chrono::steady_clock::now();const auto before=allocations();watching=true;for(unsigned i=0;i<n;++i)if(setGroupShape0Bounded48T(f.source,f.translation,f.initial,f.destination,f.sphere)!=Status::Complete)++failures;watching=false;check(allocations()==before,"allocation-free benchmark");perf={{"iterations",n},{"mean_ns_per_call",std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-start).count()/n},{"parallelism",1},{"runtime_dynamic_workspace_bytes",0}};}
 auto report=accounting();report["fixture_cases"]=packet.at("cases").size();report["source_boundary_cases"]=boundaries;report["original_only_outside_native_group_entry"]=originalOnly;report["benchmark"]=perf;report["scope"]="exact group135CF0 tag0 finite binary32 projection; direct11F680-only proofs excluded; bounded serial native host evidence";
 if(argc>2){std::ofstream out(argv[2]);out<<report.dump(2)<<'\n';}std::cout<<report.dump()<<'\n';return failures?1:0;
 }catch(const std::exception& e){watching=false;std::cerr<<label<<": harness error: "<<e.what()<<'\n';return 2;}}
