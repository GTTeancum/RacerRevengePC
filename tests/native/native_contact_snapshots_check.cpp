// End-to-end equivalence of the 50HW world contact adapter before and after
// SnapshotAccess. The reference is the committed full-Bank::visit adapter,
// renamed by the check build (native_racer_contact_bank50hw_reference.h). Both
// run the SAME original POD01 collision geometry on two byte-identical banks,
// laid out exactly as the game registers them (shared noise excluded), with
// real eight-racer contacts. Every racer's complete registered and shared
// bytes, the status, receipt and admission diagnostic must match exactly.
// Argument: original POD01.RES path.
#include "native_racer_contact_bank50hw_reference.h"
#include "native_racer_contact_bank50hw_fixture.h"
#include "../../src/game/original_shared_rng50iu.h"
#include <cfenv>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>
#include <vector>
using namespace rr::game;
namespace c=native_racer_contact50HW;
namespace {
std::size_t checks=0;
void need(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAILED: %s\n",why);std::exit(1);}}
BodyMatrix48G pose(float x,float z){BodyMatrix48G m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1;m.v[12]=x;m.v[14]=z;return m;}
constexpr std::size_t racers=8;
constexpr std::uint32_t nodeCount=6;
std::uint32_t identity(std::size_t slot){return 100u+static_cast<std::uint32_t>(slot);}

struct World {
    VehicleFrameState49W state{};VehicleFrameBindings49W bindings{};VehicleFrameParameters49W parameters{};
    std::array<std::uint32_t,4096> unrelated{};
    native_racer50HN::Bank bank;
};
struct Scenario {
    std::array<float,racers> x{},z{},vx{};
    std::array<bool,racers> ai{};
    std::array<std::int32_t,racers> count20C{};
    std::size_t notReady=racers,sceneMismatch=racers,nodeMismatch=racers;
};
// Places one complete, independent racer into each bank slot.
std::unique_ptr<World> build(const Scenario& s,const SpringModelBody48K& template_){
    auto w=std::make_unique<World>();
    registerRacerFrameExceptSharedNoise50IU(w->bank,w->state);
    w->bank.owner(w->bindings);w->bank.owner(w->parameters);w->bank.owner(w->unrelated);
    w->bank.seal(racers);
    // Shared (unregistered) noise bytes: identical in both worlds, never moved.
    auto* noise=reinterpret_cast<unsigned char*>(&w->state.noise);
    for(std::size_t i=0;i<sizeof w->state.noise;++i)noise[i]=static_cast<unsigned char>(i*7+3);
    for(std::size_t slot=0;slot<racers;++slot)w->bank.visit(slot,[&](){
        auto& st=w->state;auto& b=w->bindings;auto& p=w->parameters;
        st.phase=slot==s.notReady?VehicleFramePhase49W::PostPlayer:VehicleFramePhase49W::Ready;
        st.sceneNodeCount=nodeCount;b.sceneNodeCount=slot==s.nodeMismatch?nodeCount-1:nodeCount;
        const auto node=static_cast<std::uint32_t>(1+slot%(nodeCount-1));
        b.lifecycle.main190=static_cast<decltype(b.lifecycle.main190)>(node);
        auto& root=st.bodies.main.components.root;
        root=template_.body.state;root.scene=pose(s.x[slot],s.z[slot]);
        root.body.orientation.v[0]=1;root.body.velocity={{s.vx[slot],0,0}};
        root.body.angularVelocity={{0,.25f*static_cast<float>(slot),0}};
        for(std::uint32_t n=0;n<nodeCount;++n)st.nodes[n].world100=pose(-1,static_cast<float>(n));
        st.nodes[node].world100=root.scene;
        if(slot==s.sceneMismatch)st.nodes[node].world100.v[13]+=1;
        p.bodies.main.body=template_.body.parameters;
        p.bodies.main.body.mass=100.f+static_cast<float>(slot);p.bodies.main.body.inverseMass=1.f/p.bodies.main.body.mass;
        p.bodies.main.centerOffset={{0,.01f*static_cast<float>(slot),0}};
        st.fields.vehicle58=s.ai[slot]?1u:0u;st.fields.dynamicOwnersBound50IW=true;
        std::memcpy(&st.bodies.main.components.body20C,&s.count20C[slot],sizeof s.count20C[slot]);
        for(std::size_t k=0;k<BodyContactCapacity48R;++k)
            st.mainContactPartners210[k].value=static_cast<std::uint32_t>(k<static_cast<std::size_t>(std::max(0,s.count20C[slot]))?
                identity((slot+1+k)%racers):0xdead0000u+slot*16+k);
        // Bytes outside the contact commit that must survive untouched.
        st.bodies.secondary.root.scene=pose(123.f+static_cast<float>(slot),456);
        w->unrelated.fill(static_cast<std::uint32_t>(slot*0x01010101u+17));
    });
    return w;
}
// Complete visible bytes of every slot, exactly as Bank::visit presents them.
std::vector<unsigned char> image(World& w){
    std::vector<unsigned char> out;
    for(std::size_t slot=0;slot<racers;++slot)w.bank.visit(slot,[&](){
        const auto add=[&](const auto& v){const auto* b=reinterpret_cast<const unsigned char*>(&v);out.insert(out.end(),b,b+sizeof v);};
        add(w.state);add(w.bindings);add(w.parameters);add(w.unrelated);
    });
    return out;
}
struct Result {
    c::Status status{};c::Receipt receipt{};c::AiContactBoundary50IV boundary{};bool threw=false;
};
bool sameReceipt(const c::Receipt& a,const c::Receipt& b){
    return a.testedPairs==b.testedPairs&&a.geometryPairs==b.geometryPairs&&a.suppressedPairs==b.suppressedPairs&&
        a.respondedPairs==b.respondedPairs&&a.generatedRecords==b.generatedRecords&&a.activeRecords==b.activeRecords&&
        a.normalModeWrite.bound==b.normalModeWrite.bound&&
        !std::memcmp(a.contacts.data(),b.contacts.data(),sizeof a.contacts);
}
using Adapter=std::function<c::Status(World&,c::Owner&,const c::Pair*,std::size_t,c::Receipt&,c::AiContactBoundary50IV*,
    bool,const native_dynamic_contact50IW::Registry*)>;
std::array<std::uint32_t,racers> identities(){std::array<std::uint32_t,racers> r{};for(std::size_t i=0;i<racers;++i)r[i]=identity(i);return r;}
const std::array<std::uint32_t,racers> ids=identities(),materials{};
Adapter reference=[](World& w,c::Owner& o,const c::Pair* order,std::size_t n,c::Receipt& r,c::AiContactBoundary50IV* d,
    bool joined,const native_dynamic_contact50IW::Registry* reg){
    return c::applyAtWorldBoundaryReference50HW(w.bank,o,w.state,w.bindings,w.parameters,ids,materials,order,n,r,d,joined,reg);};
Adapter current=[](World& w,c::Owner& o,const c::Pair* order,std::size_t n,c::Receipt& r,c::AiContactBoundary50IV* d,
    bool joined,const native_dynamic_contact50IW::Registry* reg){
    return c::applyAtWorldBoundary(w.bank,o,w.state,w.bindings,w.parameters,ids,materials,order,n,r,d,joined,reg);};
Result run(const Adapter& adapter,World& w,c::Owner& o,const std::vector<c::Pair>& order,bool joined,
    const native_dynamic_contact50IW::Registry* reg,bool insideVisit){
    Result r;
    const auto call=[&](){r.status=adapter(w,o,order.data(),order.size(),r.receipt,&r.boundary,joined,reg);};
    try{if(insideVisit)w.bank.visit(1,[&](){call();});else call();}catch(const std::runtime_error&){r.threw=true;}
    return r;
}
// Runs both adapters on identical fresh worlds; returns the shared result.
Result compare(const char* name,const Scenario& s,const SpringModelBody48K& t,c::Owner& o,
    const std::vector<c::Pair>& order,bool joined,const native_dynamic_contact50IW::Registry* reg,bool insideVisit=false){
    auto a=build(s,t),b=build(s,t);
    const auto before=image(*a);need(before==image(*b),"identical starting worlds");
    const auto ra=run(reference,*a,o,order,joined,reg,insideVisit);
    const auto rb=run(current,*b,o,order,joined,reg,insideVisit);
    const auto ia=image(*a),ib=image(*b);
    need(ra.threw==rb.threw,"same exception behavior");
    need(ra.threw||ra.status==rb.status,"same status");
    need(ra.threw||sameReceipt(ra.receipt,rb.receipt),"same receipt");
    need(ra.boundary.unsupported==rb.boundary.unsupported&&ra.boundary.racerSlot==rb.boundary.racerSlot&&
         ra.boundary.sourcePC==rb.boundary.sourcePC&&ra.boundary.contactCount20C==rb.boundary.contactCount20C,"same admission diagnostic");
    need(ia==ib,"identical bytes in every racer after the pass");
    const bool changed=ia!=before;
    need(ra.threw||ra.status==c::Status::Complete||!changed,"rejected pass leaves every racer unchanged");
    std::printf("%-34s status=%u threw=%d responded=%zu geometry=%zu changed=%d\n",name,
        static_cast<unsigned>(ra.status),int(ra.threw),ra.receipt.respondedPairs,ra.receipt.geometryPairs,int(changed));
    return ra;
}
std::vector<c::Pair> allPairs(){std::vector<c::Pair> r;for(std::size_t a=0;a<racers;++a)for(std::size_t b=0;b<racers;++b)if(a!=b)r.push_back({a,b});return r;}
}
int main(int argc,char** argv){
    need(argc==2,"usage: check POD01.RES");
    need(std::fesetround(FE_TONEAREST)==0,"rounding");
    native_racer_collision50HW::Owner shapes;std::string error;
    native_racer_collision50HW::EndpointPolicy endpoint{};
    endpoint.words=[](void*,native_racer_collision50HW::QueryBody,std::size_t,Type2OpaqueEndpointWords49X& w){w={0x3f800000u,0x3f800000u};return true;};
    need(shapes.prepare({argv[1],"Pods\\Pod01\\"},endpoint,error),error.c_str());
    std::array<c::Binding,racers> bindings{};
    for(auto& b:bindings){
        b.resources=&shapes;b.settings.impact54=true;b.settings.friction50=true;b.settings.impact.disableLeverArms5C=true;
        b.settings.impact.correctPenetration64=true;b.settings.impact.normalForceScale98=.97f;
        b.settings.impact.positionCorrectionScaleCC=.2f;b.response.maxSpeed94=10000;
    }
    c::Owner owner;need(owner.prepare(bindings.data(),racers,error),error.c_str());
    SpringModelBody48K body{};body.body.state.body.orientation.v[0]=1;
    body.body.parameters.mass=100;body.body.parameters.inverseMass=.01f;
    for(unsigned k=0;k<3;++k)body.body.parameters.inverseInertiaColumns[k].v[k]=.001f;
    // Find a real geometry hit between two authored POD01 shapes.
    float hitX=0,hitZ=0;bool found=false;
    {
        std::array<c::Body,racers> in{},out{};
        for(std::size_t i=0;i<racers;++i){in[i].physical=body;in[i].physical.identity=identity(i);
            in[i].physical.body.state.scene=pose(1000.f*static_cast<float>(i),0);}
        const c::Pair pair[]={{0,1},{1,0}};
        for(float x=2;x<100&&!found;x+=2)for(float z=-80;z<=80&&!found;z+=4){
            in[1].physical.body.state.scene=pose(x,z);in[0].physical.body.state.body.velocity={{100,0,0}};
            c::Receipt r{};need(owner.run(in.data(),racers,pair,2,out.data(),r)==c::Status::Complete,"probe run");
            if(r.respondedPairs){found=true;hitX=x;hitZ=z;}
        }
    }
    need(found,"authored POD01 shapes produce a real contact");
    std::printf("contact placement x=%g z=%g\n",hitX,hitZ);
    // Four colliding pairs, far apart from each other.
    Scenario base{};
    for(std::size_t i=0;i<racers;++i){
        const float group=1000.f*static_cast<float>(i/2);
        base.x[i]=group+(i%2?hitX:0.f);base.z[i]=i%2?hitZ:0.f;base.vx[i]=i%2?0.f:100.f;
    }
    const auto order=allPairs();
    auto r=compare("legacy, eight humans, four hits",base,body,owner,order,false,nullptr);
    need(r.status==c::Status::Complete&&r.receipt.respondedPairs>=4,"real responses committed");
    auto apart=base;for(std::size_t i=0;i<racers;++i){apart.x[i]=1000.f*static_cast<float>(i);apart.z[i]=0;}
    r=compare("legacy, no contacts",apart,body,owner,order,false,nullptr);
    need(r.status==c::Status::Complete&&!r.receipt.geometryPairs,"no-contact pass");
    auto ai=base;ai.ai[3]=true;
    r=compare("legacy, AI contact rejected",ai,body,owner,order,false,nullptr);
    need(r.status==c::Status::ResponseRejected&&r.boundary.unsupported,"AI boundary");
    // Joined dynamic-contact mode, with the retained registry the game uses.
    native_dynamic_contact50IW::Registry registry;
    std::array<native_dynamic_contact50IW::ManagerBinding,racers> manager{};
    for(std::size_t i=0;i<racers;++i)manager[i]={i,{identity(i)}};
    need(registry.prepare(manager.data(),racers,1,error),error.c_str());
    auto joined=base;for(std::size_t i=0;i<racers;++i){joined.ai[i]=i!=0;joined.count20C[i]=static_cast<std::int32_t>(i%3);}
    r=compare("joined, AI fleet, lists appended",joined,body,owner,order,true,&registry);
    need(r.status==c::Status::Complete&&r.receipt.respondedPairs>=4,"joined responses committed");
    auto negative=joined;negative.count20C[5]=-1;
    r=compare("joined, negative 20C rejected",negative,body,owner,order,true,&registry);
    need(r.status==c::Status::ResponseRejected,"negative count rejected");
    auto full=joined;full.count20C[2]=static_cast<std::int32_t>(BodyContactCapacity48R);
    compare("joined, full 210 list",full,body,owner,order,true,&registry);
    auto notReady=base;notReady.notReady=4;
    r=compare("participant not Ready",notReady,body,owner,order,false,nullptr);
    need(r.status==c::Status::InvalidInput,"non-Ready rejected");
    auto scene=base;scene.sceneMismatch=6;
    r=compare("scene/root mismatch",scene,body,owner,order,false,nullptr);
    need(r.status==c::Status::InvalidInput,"scene mismatch rejected");
    auto nodes=base;nodes.nodeMismatch=7;
    r=compare("scene node count mismatch",nodes,body,owner,order,false,nullptr);
    need(r.status==c::Status::InvalidInput,"node count rejected");
    // A stopped racer that the caller excludes from pairs is still validated
    // for scene counts but not phase, and is never committed.
    auto excluded=base;excluded.notReady=7;std::vector<c::Pair> six;
    for(const auto& p:order)if(p.primary!=7&&p.other!=7)six.push_back(p);
    r=compare("non-participant not Ready",excluded,body,owner,six,false,nullptr);
    need(r.status==c::Status::Complete,"excluded racer ignored");
    r=compare("called inside a racer visit",base,body,owner,order,false,nullptr,true);
    need(r.threw,"nested lease rejected");
    // Cost of the adapter itself on the four-hit world (repeat passes keep
    // accumulating real responses; equality is not re-checked here).
    for(const auto* label:{"reference","snapshot"}){
        auto w=build(base,body);const auto& adapter=label[0]=='r'?reference:current;
        constexpr int passes=200;c::Receipt receipt{};
        const auto start=std::chrono::steady_clock::now();
        for(int i=0;i<passes;++i)need(adapter(*w,owner,order.data(),order.size(),receipt,nullptr,false,nullptr)==c::Status::Complete,"timed pass");
        const auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-start).count();
        std::printf("timing %-9s %.1f us/pass\n",label,static_cast<double>(ns)/passes/1000.0);
    }
    std::printf("contact_snapshots_check passed checks=%zu full_snapshot_bytes=%zu\n",checks,build(base,body)->bank.bytesPerRacer());
}
