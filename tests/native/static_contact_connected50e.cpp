// Connected numerical diagnostic using the actual type2/TA differential inputs.
// Supplied test body and response parameters are explicit, not asset tuning.
#include "static_type2_producer49x.h"
#include "static_contact_response50e.h"
#include "../../src/assets/spring_authoring50f.h"
using namespace rr::game;
Type2StaticStatus49X connectedQuery50E(const Type2StaticQuery49X&,
    const Type2StaticCandidate49X*,std::size_t,const Type2StaticInitial49X&,
    const Type2StaticWorkspace49X&,const Type2StaticDestination49X&,Type2StaticResult49X&) noexcept;
#define produceStaticType2Contacts49X connectedQuery50E
#define main retained_type2_main
#include "static_type2_producer49x.cpp"
#undef main
#undef produceStaticType2Contacts49X
namespace {std::size_t responseCalls=0,positiveContacts=0,changedBodies=0,authoredMainCalls=0;
rr::assets::SpringCollisionAuthoring50F authoredCollision{};bool haveAuthoring=false;}
Type2StaticStatus49X connectedQuery50E(const Type2StaticQuery49X&q,
    const Type2StaticCandidate49X*c,std::size_t n,const Type2StaticInitial49X&i,
    const Type2StaticWorkspace49X&w,const Type2StaticDestination49X&d,Type2StaticResult49X&r) noexcept {
    const auto status=produceStaticType2Contacts49X(q,c,n,i,w,d,r);
    if(status!=Type2StaticStatus49X::Complete)return status;
    // All per-frame buffers below are fixed test-owned storage, never resized.
    constexpr std::size_t capacity=64;
    if(i.recordCount>capacity)std::abort();
    SpringGeneratedRecord48K projected[capacity]{};std::uint8_t flags[capacity]{},nextFlags[capacity]{};
    SpringRecord48N records[capacity]{};SpringModelBody48K body{},scratch{},prepared{};
    body.identity=1;std::memcpy(&body.body.state.scene,&q.worldMatrix,sizeof q.worldMatrix);
    body.body.parameters.mass=1;body.body.parameters.inverseMass=1;
    for(unsigned k=0;k<3;++k)body.body.parameters.inverseInertiaColumns[k].v[k]=1;
    body.body.state.body.velocity={{0,-1,0}};
    body.body.state.body.linearAcceleration={{0,-9.81f,0}};body.body.state.loads.force={{0,-9.81f,0}};
    SpringGeneratedParameters48K parameters{};parameters.impact54=true;
    parameters.impact.disableLeverArms5C=true;parameters.impact.correctPenetration64=true;
    parameters.impact.normalForceScale98=.97f;parameters.impact.positionCorrectionScaleCC=.2f;
    StaticResponseScene50E scene{q.queryScene,q.worldMatrix};SpringResponseParameters48L response{};
    if(haveAuthoring&&q.resource->rayCount==24){
        parameters.friction50=true;
        if(!rr::assets::applySpringCollision50F(authoredCollision,body.body.parameters.mass,parameters,response))std::abort();
        ++authoredMainCalls;
    }
    StaticResponseWorkspace50E work{projected,capacity,{&scratch,1,flags,capacity},{&prepared,1,nextFlags,capacity}};
    StaticResponseResult50E result{};
    const auto responseStatus=respondStaticGenerated50E(parameters,response,r.records,body,scene,d.records,i.recordCount,work,records,capacity,result);
    if(responseStatus!=StaticResponseStatus50E::Complete)std::abort();
    ++responseCalls;positiveContacts+=result.records.contact3C!=0;
    changedBodies+=std::memcmp(&body.body.state,&result.primary.body.state,sizeof body.body.state)!=0;
    for(std::size_t k=0;k<i.recordCount;++k){SpringRecord48N expected;std::memcpy(&expected,&d.records[k],sizeof expected);expected.flags50=records[k].flags50;if(std::memcmp(&expected,&records[k],sizeof expected))std::abort();}
    return status;
}
int main(int argc,char**argv){
    if(argc==3){std::ifstream input(argv[2],std::ios::binary);std::string text((std::istreambuf_iterator<char>(input)),{});
        rr::assets::PodPhysicsDiagnostic49N diagnostic{};
        if(!rr::assets::readSpringCollision50F(text.data(),text.size(),authoredCollision,diagnostic))return 2;
        haveAuthoring=true;
    }
    const int result=retained_type2_main(argc==3?2:argc,argv);
    std::cout<<"{\"connected_response_calls\":"<<responseCalls<<",\"positive_contacts\":"<<positiveContacts<<",\"changed_bodies\":"<<changedBodies<<",\"authored_main_calls\":"<<authoredMainCalls<<",\"hot_allocations\":"<<allocations<<"}\n";
    return result||!positiveContacts||!changedBodies||allocations?1:0;
}
