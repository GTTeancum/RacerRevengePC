#include "spring_model48k.h"
#include "json.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <string>
#include <vector>

// Test-only allocation instrumentation; production has no allocator dependency.
static bool watchAllocations=false;
static std::size_t observedAllocations=0;
#if defined(_MSC_VER)
#define RR_TEST_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define RR_TEST_NOINLINE __attribute__((noinline))
#else
#define RR_TEST_NOINLINE
#endif
RR_TEST_NOINLINE void* operator new(std::size_t size){
    if(watchAllocations)++observedAllocations;
    if(void* p=std::malloc(size?size:1))return p;
    throw std::bad_alloc();
}
RR_TEST_NOINLINE void* operator new[](std::size_t size){return ::operator new(size);}
RR_TEST_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
RR_TEST_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}

using namespace rr::game;
using Json=nlohmann::json;
namespace {
unsigned checks=0,failures=0;
std::string label;
void check(bool value,const char* detail){
    ++checks;if(!value){++failures;if(failures<30)std::cerr<<label<<": "<<detail<<'\n';}
}
std::uint32_t bits(float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;}
float fp(std::uint32_t u){float f;std::memcpy(&f,&u,4);return f;}
Vector3_48C vec(const float* f){return {{f[0],f[1],f[2]}};}
std::vector<float> floats(const Json& a){std::vector<float> result;for(const auto& v:a)result.push_back(fp(v.get<std::uint32_t>()));return result;}
SpringGeneratedState48K state(const Json& a){
    return {a.at(0).get<std::uint32_t>(),a.at(1).get<std::uint32_t>(),a.at(2).get<std::uint32_t>(),
            a.at(3).get<std::uint32_t>(),a.at(4).get<std::uint32_t>(),a.at(5).get<std::uint32_t>(),a.at(6).get<std::uint32_t>()};
}
std::array<std::uint32_t,7> stateWords(const SpringGeneratedState48K& s){return {s.pending38,s.contact3C,s.upward40,s.other44,s.point74,s.generated78,s.field130};}
SpringModelBody48K body(const Json& j){
    const auto f=floats(j.at("input"));if(f.size()!=60)throw std::runtime_error("body fixture width");
    SpringModelBody48K result{};result.identity=j.at("identity").get<std::uint32_t>();auto& b=result.body;
    std::copy_n(f.data(),16,b.state.scene.v);b.localCenter=vec(f.data()+16);
    b.parameters.mass=f[19];b.parameters.inverseMass=f[20];
    for(unsigned i=0;i<3;++i)b.parameters.inverseInertiaColumns[i]=vec(f.data()+21+3*i);
    b.state.body.velocity=vec(f.data()+30);b.state.body.angularVelocity=vec(f.data()+33);
    b.state.body.linearAcceleration=vec(f.data()+36);b.state.body.angularAcceleration=vec(f.data()+39);
    b.state.loads={vec(f.data()+42),vec(f.data()+45)};
    b.state.positionCorrection1a0=vec(f.data()+48);b.state.rotationCorrection1b0=vec(f.data()+51);
    b.state.accumulator1c0=vec(f.data()+54);b.state.accumulator1d0=vec(f.data()+57);
    // Distinct native-only values must survive the numerical response untouched.
    b.state.body.position={{123,234,345}};b.state.body.orientation={{1,2,3,4}};
    b.state.body.previousPositionDerivative={{5,6,7}};b.state.body.previousLinearAcceleration={{8,9,10}};
    b.state.body.previousAngularAcceleration={{11,12,13}};b.state.body.previousOrientationDerivative={{14,15,16,17}};
    b.state.body.orientationDerivative={{18,19,20,21}};b.state.previousVelocity90={{22,23,24}};
    b.state.localVelocity80={{25,26,27}};b.state.worldAngularVelocityA0={{28,29,30}};b.state.callbackState238=0x12345678;
    return result;
}
struct Fixture {
    SpringGeneratedParameters48K parameters{};SpringGeneratedState48K initial{};
    std::vector<SpringModelBody48K> bodies;std::vector<SpringGeneratedRecord48K> records;
};
Fixture parse(const Json& j){
    Fixture f;const auto& p=j.at("parameters");const auto a=floats(p.at("impact"));if(a.size()!=21)throw std::runtime_error("impact fixture width");
    f.parameters.impact={vec(a.data()),vec(a.data()+3),vec(a.data()+6),a[9],a[10],a[11],a[12],a[13],a[14],a[15],a[16],a[17]!=0,a[18]!=0,a[19]!=0,a[20]!=0};
    f.parameters.frictionTorqueScaleC4=fp(p.at("frictionTorque").get<std::uint32_t>());
    const auto material=floats(p.at("frictionByMaterial"));if(material.size()!=10)throw std::runtime_error("material fixture width");
    std::copy(material.begin(),material.end(),f.parameters.frictionByMaterial.begin());
    const auto& t=p.at("toggles");f.parameters.pointContacts48=t.at(0).get<bool>();f.parameters.springForce4C=t.at(1).get<bool>();
    f.parameters.friction50=t.at(2).get<bool>();f.parameters.impact54=t.at(3).get<bool>();f.parameters.otherSpring58=t.at(4).get<bool>();f.parameters.secondary68=t.at(5).get<bool>();
    f.initial=state(j.at("state"));for(const auto& b:j.at("bodies"))f.bodies.push_back(body(b));
    for(const auto& r:j.at("records")){
        SpringGeneratedRecord48K record{};const auto v=floats(r.at("vectors")),m=floats(r.at("scene"));
        if(v.size()!=9||m.size()!=16)throw std::runtime_error("record fixture width");
        record.localPoint10=vec(v.data());record.normal20=vec(v.data()+3);record.surfacePoint30=vec(v.data()+6);
        std::copy(m.begin(),m.end(),record.scene40.v);record.hasScene40=r.at("hasScene").get<bool>();
        record.otherBodyIdentity44=r.at("other").get<std::uint32_t>();record.material4C=r.at("material").get<std::uint32_t>();record.flags50=r.at("flags").get<std::uint8_t>();f.records.push_back(record);
    }
    return f;
}
std::array<std::uint32_t,24> outputWords(const SpringImpactBody48K& b){
    std::array<std::uint32_t,24> out{};const Vector3_48C* v[]={&b.state.body.velocity,&b.state.body.angularVelocity,&b.state.loads.force,&b.state.loads.torque,&b.state.positionCorrection1a0,&b.state.rotationCorrection1b0,&b.state.accumulator1c0,&b.state.accumulator1d0};
    for(unsigned i=0;i<8;++i)for(unsigned k=0;k<3;++k)out[i*3+k]=bits(v[i]->v[k]);
    return out;
}
void checkUnchanged(const SpringModelBody48K& before,SpringModelBody48K after){
    after.body.state.body.velocity=before.body.state.body.velocity;after.body.state.body.angularVelocity=before.body.state.body.angularVelocity;
    after.body.state.loads=before.body.state.loads;after.body.state.positionCorrection1a0=before.body.state.positionCorrection1a0;
    after.body.state.rotationCorrection1b0=before.body.state.rotationCorrection1b0;after.body.state.accumulator1c0=before.body.state.accumulator1c0;after.body.state.accumulator1d0=before.body.state.accumulator1d0;
    // Field-wise scalar/vector portions avoid relying on host struct padding.
    check(after.identity==before.identity,"body identity unchanged");
    check(std::memcmp(&after.body.state.body,&before.body.state.body,sizeof(BodyState48C))==0,"body histories unchanged");
    check(std::memcmp(after.body.state.scene.v,before.body.state.scene.v,64)==0,"scene unchanged");
    check(std::memcmp(&after.body.state.previousVelocity90,&before.body.state.previousVelocity90,36)==0,"root velocity caches unchanged");
    check(after.body.state.callbackState238==before.body.state.callbackState238,"callback state unchanged");
    check(std::memcmp(&after.body.parameters,&before.body.parameters,offsetof(BodyParameters48C,rotationEnabled)+sizeof(after.body.parameters.rotationEnabled))==0,"parameters unchanged");
    check(std::memcmp(&after.body.localCenter,&before.body.localCenter,12)==0,"local center unchanged");
}
struct Storage {
    std::vector<SpringModelBody48K> scratch,out;
    std::vector<std::uint8_t> scratchFlags,outFlags;
    SpringGeneratedResult48K result{};
    explicit Storage(const Fixture& f):scratch(f.bodies.size()+1),out(f.bodies.size()+1),scratchFlags(f.records.size()+1,0xA5),outFlags(f.records.size()+1,0x5A){out.back().identity=0xDEADBEEF;scratch.back().identity=0xCAFEBABE;result.state.field130=0xAABBCCDD;}
    SpringGeneratedBuffers48K temp(){return {scratch.data(),scratch.size()-1,scratchFlags.data(),scratchFlags.size()-1};}
    SpringGeneratedBuffers48K dest(){return {out.data(),out.size()-1,outFlags.data(),outFlags.size()-1};}
};
SpringGeneratedStatus48K run(const Fixture& f,Storage& s){
    const auto old=observedAllocations;watchAllocations=true;
    const auto status=updateGeneratedSpringContacts48K(f.parameters,f.initial,f.bodies.data(),f.bodies.size(),f.records.data(),f.records.size(),s.temp(),s.dest(),s.result);
    watchAllocations=false;check(observedAllocations==old,"zero runtime allocations");return status;
}
void differential(const Json& j){
    label=j.at("name").get<std::string>();const auto f=parse(j);Storage s(f);check(run(f,s)==SpringGeneratedStatus48K::Complete,"complete");
    const auto& e=j.at("expected");check(stateWords(s.result.state)==stateWords(state(e.at("state"))),"spring state bits");
    check(s.result.bodyCount==f.bodies.size()&&s.result.recordCount==f.records.size(),"result extents");
    for(unsigned k=0;k<3;++k){check(bits(s.result.worldCenter110.v[k])==e.at("center").at(k).get<std::uint32_t>(),"center bits");check(bits(s.result.contactVelocity120.v[k])==e.at("velocity").at(k).get<std::uint32_t>(),"velocity bits");}
    for(std::size_t i=0;i<f.bodies.size();++i){const auto actual=outputWords(s.out[i].body);for(unsigned k=0;k<24;++k){const auto expected=j.at("bodies").at(i).at("expected").at(k).get<std::uint32_t>();if(actual[k]!=expected&&failures<20)std::cerr<<label<<" body "<<i<<" word "<<k<<" got "<<std::hex<<actual[k]<<" expected "<<expected<<std::dec<<'\n';check(actual[k]==expected,"body output bits");}checkUnchanged(f.bodies[i],s.out[i]);}
    for(std::size_t i=0;i<f.records.size();++i)check(s.outFlags[i]==e.at("flags").at(i).get<std::uint8_t>(),"all record flags");
    check(s.out.back().identity==0xDEADBEEF&&s.scratch.back().identity==0xCAFEBABE&&s.outFlags.back()==0x5A&&s.scratchFlags.back()==0xA5,"buffer canaries");
    // The output body array can be the persistent input body array.
    auto inPlace=f;Storage alias(inPlace);auto destination=alias.dest();destination.bodies=inPlace.bodies.data();
    watchAllocations=true;const auto status=updateGeneratedSpringContacts48K(inPlace.parameters,inPlace.initial,inPlace.bodies.data(),inPlace.bodies.size(),inPlace.records.data(),inPlace.records.size(),alias.temp(),destination,alias.result);watchAllocations=false;
    check(status==SpringGeneratedStatus48K::Complete,"in-place complete");
    for(std::size_t i=0;i<f.bodies.size();++i)check(outputWords(inPlace.bodies[i].body)==outputWords(s.out[i].body),"in-place exact");
}
Fixture simple(){
    Fixture f;f.bodies.resize(2);f.bodies[0].identity=1;f.bodies[1].identity=2;
    for(auto& b:f.bodies){for(unsigned i=0;i<4;++i)b.body.state.scene.v[i*5]=1;b.body.parameters.mass=1;b.body.parameters.inverseMass=1;for(unsigned i=0;i<3;++i)b.body.parameters.inverseInertiaColumns[i].v[i]=1;}
    f.bodies[0].body.state.body.velocity={{2,-5,1}};f.parameters.impact54=true;f.parameters.friction50=true;f.parameters.impact.angularImpulseScaleB0=1;f.parameters.impact.normalForceScale98=1;f.parameters.impact.positionCorrectionScaleCC=.1f;
    f.parameters.frictionByMaterial.fill(.1f);f.parameters.frictionTorqueScaleC4=1;f.records.resize(2);
    for(auto& r:f.records){r.localPoint10={{1,0,0}};r.normal20={{0,1,0}};r.surfacePoint30={{1,.5f,0}};r.flags50=0xAD;}
    f.initial={1,0,0,0,7,0,99};return f;
}
void rejected(const Fixture& f,SpringGeneratedStatus48K expected){
    Storage s(f);const auto old=s.out;const auto oldFlags=s.outFlags;const auto meta=s.result;check(run(f,s)==expected,"rejected status");check(std::memcmp(s.out.data(),old.data(),old.size()*sizeof(old[0]))==0&&s.outFlags==oldFlags&&std::memcmp(&s.result,&meta,sizeof(meta))==0,"transactional failure");
}
void safety(){
    label="adversarial";const auto base=simple();
    for(unsigned k=0;k<4;++k){auto f=base;bool* fields[]={&f.parameters.pointContacts48,&f.parameters.springForce4C,&f.parameters.otherSpring58,&f.parameters.secondary68};*fields[k]=true;rejected(f,SpringGeneratedStatus48K::UnsupportedScope);}
    auto f=base;f.records[1].normal20.v[1]=std::numeric_limits<float>::quiet_NaN();rejected(f,SpringGeneratedStatus48K::InvalidInput);
    f=base;f.records[1].material4C=10;rejected(f,SpringGeneratedStatus48K::InvalidInput);
    f=base;f.records[1].otherBodyIdentity44=3;rejected(f,SpringGeneratedStatus48K::InvalidInput);
    f=base;f.records[1].otherBodyIdentity44=1;rejected(f,SpringGeneratedStatus48K::InvalidInput);
    f=base;f.bodies[1].identity=1;rejected(f,SpringGeneratedStatus48K::InvalidInput);
    f=base;f.bodies[0].identity=0;rejected(f,SpringGeneratedStatus48K::InvalidInput);
    f=base;f.records[1].surfacePoint30.v[1]=std::numeric_limits<float>::max();f.records[1].normal20.v[1]=std::numeric_limits<float>::max();rejected(f,SpringGeneratedStatus48K::InvalidInput);
    f=base;f.records[1].surfacePoint30.v[1]=-1;f.records[1].material4C=999;f.records[1].otherBodyIdentity44=999;{Storage s(f);check(run(f,s)==SpringGeneratedStatus48K::Complete,"skipped contact does not consume material/body identity");check(s.outFlags[1]==(f.records[1].flags50&~2u),"miss clears only bit2");}
    f=base;f.parameters.friction50=false;f.records[1].material4C=999;{Storage s(f);check(run(f,s)==SpringGeneratedStatus48K::Complete,"disabled friction does not consume material");}
    f=base;f.records.clear();f.parameters.impact.worldCenter110={{12,13,14}};f.parameters.impact.contactVelocity120={{15,16,17}};{Storage s(f);auto tmp=s.temp(),dst=s.dest();tmp.recordFlags50=dst.recordFlags50=nullptr;check(updateGeneratedSpringContacts48K(f.parameters,f.initial,f.bodies.data(),f.bodies.size(),nullptr,0,tmp,dst,s.result)==SpringGeneratedStatus48K::Complete,"zero records null flag buffers");check(s.result.worldCenter110.v[0]==12&&s.result.contactVelocity120.v[0]==15&&s.result.state.pending38==0&&s.result.state.point74==7,"zero-record cache and sticky state");}
    // Exact original upward threshold; other-body contacts use the other flag.
    for(int offset=-1;offset<=1;++offset)for(bool other:{false,true}){
        f=base;f.parameters.impact54=f.parameters.friction50=false;f.records.resize(1);
        f.records[0].normal20={{.6f,fp(static_cast<std::uint32_t>(static_cast<int>(0x3f4ccccd)+offset)),0}};
        f.records[0].otherBodyIdentity44=other?2:0;Storage edge(f);
        check(run(f,edge)==SpringGeneratedStatus48K::Complete,"threshold complete");
        check(edge.result.state.upward40==static_cast<unsigned>(!other&&offset>0),"strict upward threshold");
        check(edge.result.state.other44==static_cast<unsigned>(other),"other contact flag excludes upward");
    }
    for(float zero:{0.f,-0.f}){
        f=base;f.records.resize(1);f.records[0].surfacePoint30=f.records[0].localPoint10;f.records[0].normal20={{0,zero,0}};
        f.parameters.impact.contactVelocity120={{12,13,14}};Storage edge(f);
        check(run(f,edge)==SpringGeneratedStatus48K::Complete&&edge.result.contactVelocity120.v[0]==12&&edge.result.state.contact3C==f.initial.contact3C,"signed-zero depth leaves caches/sticky flags");
    }
    f=base;f.records.resize(1);f.records[0].scene40.v[0]=std::numeric_limits<float>::quiet_NaN();f.parameters.frictionByMaterial[9]=std::numeric_limits<float>::quiet_NaN();
    {Storage edge(f);check(run(f,edge)==SpringGeneratedStatus48K::Complete,"unused scene/material NaN is not consumed");}
    f=base;Storage s(f);const auto meta=s.result;auto tmp=s.temp(),dst=s.dest();tmp.bodyCapacity=0;check(updateGeneratedSpringContacts48K(f.parameters,f.initial,f.bodies.data(),f.bodies.size(),f.records.data(),f.records.size(),tmp,dst,s.result)==SpringGeneratedStatus48K::InsufficientCapacity,"body capacity");
    tmp=s.temp();dst.recordCapacity=0;check(updateGeneratedSpringContacts48K(f.parameters,f.initial,f.bodies.data(),f.bodies.size(),f.records.data(),f.records.size(),tmp,dst,s.result)==SpringGeneratedStatus48K::InsufficientCapacity,"flag capacity");
    dst=s.dest();tmp.bodies=f.bodies.data();check(updateGeneratedSpringContacts48K(f.parameters,f.initial,f.bodies.data(),f.bodies.size(),f.records.data(),f.records.size(),tmp,dst,s.result)==SpringGeneratedStatus48K::InvalidInput,"scratch/input alias");
    tmp=s.temp();tmp.recordFlags50=dst.recordFlags50;check(updateGeneratedSpringContacts48K(f.parameters,f.initial,f.bodies.data(),f.bodies.size(),f.records.data(),f.records.size(),tmp,dst,s.result)==SpringGeneratedStatus48K::InvalidInput,"scratch/output alias");
    tmp=s.temp();tmp.recordFlags50=reinterpret_cast<std::uint8_t*>(&tmp);check(updateGeneratedSpringContacts48K(f.parameters,f.initial,f.bodies.data(),f.bodies.size(),f.records.data(),f.records.size(),tmp,dst,s.result)==SpringGeneratedStatus48K::InvalidInput,"scratch descriptor overlap");
    tmp=s.temp();dst.recordFlags50=reinterpret_cast<std::uint8_t*>(&dst);check(updateGeneratedSpringContacts48K(f.parameters,f.initial,f.bodies.data(),f.bodies.size(),f.records.data(),f.records.size(),tmp,dst,s.result)==SpringGeneratedStatus48K::InvalidInput,"output descriptor overlap");dst=s.dest();
    tmp=s.temp();tmp.bodies=nullptr;check(updateGeneratedSpringContacts48K(f.parameters,f.initial,f.bodies.data(),f.bodies.size(),f.records.data(),f.records.size(),tmp,dst,s.result)==SpringGeneratedStatus48K::InvalidInput,"null scratch");
    tmp=s.temp();tmp.bodyCapacity=dst.bodyCapacity=std::numeric_limits<std::size_t>::max();check(updateGeneratedSpringContacts48K(f.parameters,f.initial,f.bodies.data(),std::numeric_limits<std::size_t>::max(),f.records.data(),f.records.size(),tmp,dst,s.result)==SpringGeneratedStatus48K::InvalidInput,"overflow extent rejected before access");check(std::memcmp(&s.result,&meta,sizeof(meta))==0,"buffer errors preserve metadata");
}
void benchmark(){
    label="allocation/cost";const auto f=simple();Storage s(f);std::array<double,9> sample{};
    constexpr unsigned loops=10000;unsigned completed=0;const auto before=observedAllocations;
    for(auto& value:sample){watchAllocations=true;const auto start=std::chrono::steady_clock::now();for(unsigned i=0;i<loops;++i)completed+=updateGeneratedSpringContacts48K(f.parameters,f.initial,f.bodies.data(),f.bodies.size(),f.records.data(),f.records.size(),s.temp(),s.dest(),s.result)==SpringGeneratedStatus48K::Complete;const auto end=std::chrono::steady_clock::now();watchAllocations=false;value=std::chrono::duration<double,std::nano>(end-start).count()/loops;}
    check(completed==sample.size()*loops,"benchmark completion");check(observedAllocations==before,"benchmark zero allocations");std::sort(sample.begin(),sample.end());
    const auto bufferBytes=2*(f.bodies.size()*sizeof(SpringModelBody48K)+f.records.size());
    std::cout<<"coordinator_cost_host_ns median="<<sample[4]<<" max_batch_mean="<<sample[8]<<" contacts_per_call="<<f.records.size()<<" bodies="<<f.bodies.size()<<" caller_output_plus_scratch_bytes="<<bufferBytes<<" result_metadata_bytes="<<sizeof(SpringGeneratedResult48K)<<" body_bytes="<<sizeof(SpringModelBody48K)<<" record_bytes="<<sizeof(SpringGeneratedRecord48K)<<" runtime_allocations="<<(observedAllocations-before)<<" (microbenchmark; no game or Xbox FPS claim)\n";
}
}
int main(int argc,char** argv){
    try{
        if(argc!=2)return 2;
        std::ifstream in(std::filesystem::path(argv[1])/"evidence/springOrchestration48K/coordinator-native-fixtures.json");if(!in)throw std::runtime_error("missing coordinator-native-fixtures.json; run tools/gameplay/spring_model_fixtures48k.py");
        Json fixture;in>>fixture;const auto& cases=fixture.at("cases");if(cases.size()<4)throw std::runtime_error("missing corrected saved cases");
        for(const auto& j:cases)differential(j);
        safety();benchmark();
        std::cout<<"spring_model48K cases="<<cases.size()<<" checks="<<checks<<" failures="<<failures<<" runtime_allocations="<<observedAllocations<<'\n';return failures?1:0;
    }catch(const std::exception& e){watchAllocations=false;std::cerr<<e.what()<<'\n';return 2;}
}
