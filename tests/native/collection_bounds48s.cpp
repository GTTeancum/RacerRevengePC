// Test-only independent original1202E0 fixture comparison and native span guards.
#include "collection_bounds48s.h"
#include "json.hpp"
#include <array>
#include <cfenv>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <string>
#include <vector>
#if defined(_MSC_VER)
#define RR_NOINLINE __declspec(noinline)
#else
#define RR_NOINLINE __attribute__((noinline))
#endif
using namespace rr::game;
using Json=nlohmann::json;
static bool watching=false;
static std::size_t allocations=0;
RR_NOINLINE void* operator new(std::size_t n){if(watching)++allocations;if(void* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
RR_NOINLINE void* operator new[](std::size_t n){return ::operator new(n);}
RR_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
RR_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
RR_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
RR_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
namespace {
using Status=CollectionBoundsStatus48S;
std::size_t checks=0,failures=0,safety=0,differential=0,boundaries=0;std::string label;
void check(bool ok,const char* why){++checks;if(!ok){++failures;if(failures<40)std::cerr<<label<<": "<<why<<'\n';}}
float fp(std::uint32_t w){float f;std::memcpy(&f,&w,4);return f;}
std::uint32_t bits(float f){std::uint32_t w;std::memcpy(&w,&f,4);return w;}
template<class T>void raw(const Json& j,T& v){for(unsigned i=0;i<std::size(v);++i)v[i]=fp(j.at(i).template get<std::uint32_t>());}
struct Snapshot {
 const void* p;std::vector<unsigned char> data;
 Snapshot(const void* a,std::size_t n):p(a),data(n){if(n)std::memcpy(data.data(),a,n);}
 void verify() const {check(data.empty()||!std::memcmp(p,data.data(),data.size()),"source or transactional byte span unchanged");}
};
struct Fixture {
 CollectionBoundsInput48S in{};CollectionBoundsResult48S out{};
 std::vector<SpringVector4_48N> endpoints;std::vector<std::uint32_t> indices;std::vector<BodyMatrix48G> scenes;
 explicit Fixture(const Json& j){
  in.shapeTag=j.at("shape_tag");raw(j.at("world"),in.shapeWorld.v);raw(j.at("local_center"),in.localCenter.v);raw(j.at("local_half"),in.localHalf.v);raw(j.at("cached_center"),in.cachedCenter.v);in.cachedRadius=fp(j.at("cached_radius"));in.pairCount=j.at("pair_count");
  for(auto& e:j.at("endpoints")){SpringVector4_48N v;raw(e,v.v);endpoints.push_back(v);}
  if(!j.at("endpoint_scenes").is_null())indices=j.at("endpoint_scenes").get<decltype(indices)>();
  for(auto& e:j.at("scenes")){BodyMatrix48G v;raw(e,v.v);scenes.push_back(v);}
  in.endpoints=endpoints.data();in.endpointCount=endpoints.size();in.endpointScenes=indices.empty()?nullptr:indices.data();in.endpointSceneCount=indices.size();in.scenes=scenes.data();in.sceneCount=scenes.size();
  std::memset(static_cast<void*>(&out),0xa5,sizeof(out));
 }
 std::vector<Snapshot> snapshots(bool result) const {
  std::vector<Snapshot> s;s.emplace_back(&in,sizeof(in));s.emplace_back(endpoints.data(),endpoints.size()*sizeof(endpoints[0]));s.emplace_back(indices.data(),indices.size()*4);s.emplace_back(scenes.data(),scenes.size()*sizeof(scenes[0]));if(result)s.emplace_back(&out,sizeof(out));return s;
 }
 Status call(){watching=true;const auto result=prepareCollectionBounds48S(in,out);watching=false;return result;}
 void reject(Status wanted=Status::InvalidInput){const auto saved=snapshots(true);const auto before=allocations;check(call()==wanted,"expected rejection status");for(const auto& s:saved)s.verify();check(before==allocations,"allocation-free rejection");++safety;}
};
RR_NOINLINE CollectionBoundsResult48S* resultAt(void* p){return static_cast<CollectionBoundsResult48S*>(p);}
Status status(const std::string& s){if(s=="Complete")return Status::Complete;if(s=="InvalidInput")return Status::InvalidInput;if(s=="NonFinite")return Status::NonFinite;return Status::UnsupportedBounds;}
void compare(const Fixture& f,const Json& j){const auto& e=j.at("expected");for(unsigned k=0;k<4;++k){check(bits(f.out.center.v[k])==e.at("center")[k].get<std::uint32_t>(),"returned center bit equality");check(bits(f.out.cachedCenter.v[k])==e.at("cached_center")[k].get<std::uint32_t>(),"cached center bit equality");}for(unsigned k=0;k<3;++k)check(bits(f.out.half.v[k])==e.at("half")[k].get<std::uint32_t>(),"returned half bit equality");check(bits(f.out.cachedRadius)==e.at("cached_radius").get<std::uint32_t>(),"cached radius bit equality");}
void guards(const Json& base){
 auto test=[&](const char* name,auto mutate,Status expected=Status::InvalidInput){label=name;Fixture f(base);mutate(f);f.reject(expected);};
 test("null endpoint",[](Fixture& f){f.in.endpoints=nullptr;});
 test("unaligned endpoint",[](Fixture& f){f.in.endpoints=reinterpret_cast<const SpringVector4_48N*>(reinterpret_cast<std::uintptr_t>(f.in.endpoints)+1);});
 test("endpoint short",[](Fixture& f){f.in.endpointCount=1;},Status::InsufficientCapacity);
 test("endpoint multiplication overflow",[](Fixture& f){f.in.endpointCount=std::numeric_limits<std::size_t>::max();});
 test("endpoint address wrap",[](Fixture& f){f.in.endpoints=reinterpret_cast<const SpringVector4_48N*>(std::numeric_limits<std::uintptr_t>::max()-3);});
 test("metadata null full span",[](Fixture& f){f.in.endpointSceneCount=2;});
 test("metadata unaligned",[](Fixture& f){f.indices={0,0};f.in.endpointScenes=reinterpret_cast<const std::uint32_t*>(reinterpret_cast<std::uintptr_t>(f.indices.data())+1);f.in.endpointSceneCount=2;});
 test("metadata short",[](Fixture& f){f.indices={0};f.in.endpointScenes=f.indices.data();f.in.endpointSceneCount=1;},Status::InsufficientCapacity);
 test("metadata multiplication overflow",[](Fixture& f){f.in.endpointScenes=reinterpret_cast<const std::uint32_t*>(0x1000);f.in.endpointSceneCount=std::numeric_limits<std::size_t>::max();});
 test("metadata address wrap",[](Fixture& f){f.in.endpointScenes=reinterpret_cast<const std::uint32_t*>(std::numeric_limits<std::uintptr_t>::max()-3);f.in.endpointSceneCount=2;});
 test("scene null full span",[](Fixture& f){f.in.sceneCount=1;});
 test("scene unaligned",[](Fixture& f){f.scenes.resize(1);f.in.scenes=reinterpret_cast<const BodyMatrix48G*>(reinterpret_cast<std::uintptr_t>(f.scenes.data())+1);f.in.sceneCount=1;});
 test("scene multiplication overflow",[](Fixture& f){f.in.scenes=reinterpret_cast<const BodyMatrix48G*>(0x1000);f.in.sceneCount=std::numeric_limits<std::size_t>::max();});
 test("scene address wrap",[](Fixture& f){f.in.scenes=reinterpret_cast<const BodyMatrix48G*>(std::numeric_limits<std::uintptr_t>::max()-3);f.in.sceneCount=1;});
 test("scene index out of bounds",[](Fixture& f){f.indices={1,0};f.in.endpointScenes=f.indices.data();f.in.endpointSceneCount=2;});
 test("scene index UINT_MAX",[](Fixture& f){f.indices={0,0xffffffff};f.in.endpointScenes=f.indices.data();f.in.endpointSceneCount=2;});
 test("zero count",[](Fixture& f){f.in.pairCount=0;});
 test("negative count",[](Fixture& f){f.in.pairCount=-1;});
 test("signed maximum count insufficient span",[](Fixture& f){f.in.pairCount=std::numeric_limits<std::int32_t>::max();},Status::InsufficientCapacity);
 test("late numeric failure",[](Fixture& f){f.endpoints[1].v[2]=std::numeric_limits<float>::quiet_NaN();},Status::NonFinite);
 test("alias endpoint/result full span",[](Fixture& f){f.in.endpoints=reinterpret_cast<const SpringVector4_48N*>(&f.out);f.in.endpointCount=2;});
 test("alias metadata/result",[](Fixture& f){f.in.endpointScenes=reinterpret_cast<const std::uint32_t*>(&f.out);f.in.endpointSceneCount=2;});
 test("alias scene/result",[](Fixture& f){f.in.scenes=reinterpret_cast<const BodyMatrix48G*>(&f.out);f.in.sceneCount=1;});
 {label="alias input/result";Fixture f(base);const auto saved=f.snapshots(true);watching=true;const auto s=prepareCollectionBounds48S(f.in,*resultAt(&f.in));watching=false;check(s==Status::InvalidInput,"input/result alias rejected");for(auto& x:saved)x.verify();++safety;}
 // The overlap occurs only in an inactive declared tail. Validation may not
 // narrow spans to consumed endpoints or metadata, even with tags1/6.
 test("inactive full-span tail overlaps result",[](Fixture& f){const auto a=reinterpret_cast<std::uintptr_t>(&f.in);const auto b=reinterpret_cast<std::uintptr_t>(&f.out);f.in.endpoints=reinterpret_cast<const SpringVector4_48N*>(a);f.in.endpointCount=(b+sizeof(f.out)-a+15)/16;f.in.shapeTag=1;});
 {label="valid scene/descriptor read-only alias";Fixture f(base);f.in.scenes=&f.in.shapeWorld;f.in.sceneCount=1;f.indices={1,1};f.in.endpointScenes=f.indices.data();f.in.endpointSceneCount=2;const auto saved=f.snapshots(false);check(f.call()==Status::Complete,"read-only scene/descriptor alias accepted");for(auto& s:saved)s.verify();++safety;}

}
}
int main(int argc,char** argv){
 if(argc<2){std::cerr<<"usage: collection_bounds48s fixtures.json [summary.json]\n";return 2;}
 std::fesetround(FE_TONEAREST);std::ifstream stream(argv[1]);Json fixture;stream>>fixture;
 for(const auto& c:fixture.at("cases")){
  label=c.at("name");Fixture f(c);const auto wanted=status(c.at("native_status"));
  if(wanted!=Status::Complete){f.reject(wanted);++boundaries;continue;}
  const auto saved=f.snapshots(false);const auto before=allocations;
  check(f.call()==Status::Complete,"original-success native complete");compare(f,c);for(const auto& s:saved)s.verify();check(allocations==before,"allocation-free success");++differential;
 }
 const Json* base=nullptr;for(const auto& c:fixture.at("cases"))if(c.at("name")=="tag3-optional-null-metadata")base=&c;
 if(!base)return 3;
 guards(*base);
 Fixture perf(*base);constexpr unsigned iterations=25000;const auto started=std::chrono::steady_clock::now();watching=true;
 for(unsigned i=0;i<iterations;++i)if(prepareCollectionBounds48S(perf.in,perf.out)!=Status::Complete)++failures;
 watching=false;const double ns=std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-started).count()/iterations;
 Json report={{"checks",checks},{"failures",failures},{"differential_cases",differential},{"boundary_cases",boundaries},{"safety_cases",safety},{"observed_allocations",allocations},{"performance_iterations",iterations},{"mean_ns_per_call",ns},{"parallelism",1},{"runtime_dynamic_workspace_bytes",0},{"model","ordinary separately rounded finite binary32"}};
 if(argc>2){std::ofstream out(argv[2]);out<<report.dump(2)<<'\n';}std::cout<<report.dump()<<'\n';return failures?1:0;
}
