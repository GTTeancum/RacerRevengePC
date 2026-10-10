// Production-only timed executable. All fixture reads, comparisons, reset,
// hashes and output occur outside the single timer pair per144-frame history.
#include "native_frame49w.h"
#include <cfenv>
#include <chrono>
#include <memory>
#include <new>
#include <vector>
namespace b=budget49w;
#ifdef PROBE49W
static bool counting=false;static std::uint64_t allocated=0;
void*operator new(std::size_t n){if(counting)++allocated;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void*operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void*p)noexcept{std::free(p);}void operator delete[](void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);}void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
#endif
struct Fixture {
    b::Immutable data{};
    b::VehicleFrameState49W initial{};
    std::array<rr::assets::MotionView49O,6> motions{};
    std::array<std::vector<std::uint8_t>,6> mot;
    std::array<b::Expected,b::frames> expected[2]{};
    std::size_t motionBytes=0;
};
struct Reader {
    std::FILE*f=nullptr;
    explicit Reader(const char*path):f(std::fopen(path,"rb")){b::need(f!=nullptr,"fixture open");}
    void bytes(void*p,std::size_t n){b::need(std::fread(p,1,n,f)==n,"fixture exact read");}
    template<class T>void get(T&x){bytes(&x,sizeof x);}
    b::U word(){b::U u;get(u);return u;}
    void close(){b::need(std::fgetc(f)==EOF,"no trailing fixture");b::need(std::fclose(f)==0,"fixture close");}
};
static void read(const char*path,Fixture&f){
    Reader r(path);char magic[8];r.bytes(magic,8);b::need(!std::memcmp(magic,"RR49WB01",8),"fixture magic");
    b::need(r.word()==0x01020304&&r.word()==sizeof(b::Immutable)&&r.word()==sizeof(b::VehicleFrameState49W)&&r.word()==sizeof(b::Expected)&&r.word()==b::frames,"host ABI/endian/extent");
    r.get(f.data);r.get(f.initial);
    rr::assets::GeometryDiagnostic49O d{};
    for(unsigned i=0;i<6;++i){auto&mot=f.mot[i];const auto n=r.word();b::need(n<=rr::assets::maxMotBytes49O,"bounded MOT");mot.resize(n);r.bytes(mot.data(),n);f.motionBytes+=n;b::need(rr::assets::readMot49O(mot.data(),n,f.motions[i],d),"actual native MOT admission");}
    for(auto&scenario:f.expected)for(auto&frame:scenario)r.get(frame);r.close();b::bindPointers(f.data,f.motions.data());
}
static void same(const b::VehicleFrameState49W&a,const b::VehicleFrameState49W&e,b::U frame,const char*phase){
    const auto actual=b::normalized(a),expected=b::normalized(e);
    if(std::memcmp(&actual,&expected,sizeof actual)){const auto*p=reinterpret_cast<const unsigned char*>(&actual),*q=reinterpret_cast<const unsigned char*>(&expected);std::size_t at=0;while(at<sizeof actual&&p[at]==q[at])++at;std::fprintf(stderr,"frame=%u phase=%s byte=%zu actual=%u expected=%u\n",frame,phase,at,unsigned(p[at]),unsigned(q[at]));b::need(false,"exact snapshot parity");}
}
static b::H immutableHash(const Fixture&f){b::H h=b::hash(f.data);for(unsigned i=0;i<6;++i){h^=b::hash(f.motions[i]);h^=b::hashBytes(f.mot[i].data(),f.mot[i].size());}return h;}
static void sizes(){
#define S(t) std::printf(#t " %zu %zu\n",sizeof(t),alignof(t))
    using namespace rr::game;using namespace rr::assets;
    S(VehicleFrameState49W);S(VehicleFrameWorkspace49W);S(VehicleFrameDiagnostic49W);S(VehicleFrameParameters49W);S(VehicleFrameBindings49W);
    S(CoupledBodyState49M);S(SecondaryActivationSupplement49V);S(GraphBodyExtraLanes49W);S(VehicleAnimationState49T);S(VehicleAnimationShared49T);S(VehicleVisualState49R);S(VisualShared49R);S(ChariotLifecycleState49V);S(ChariotLifecycleShared49V);S(CoupledNoiseContext49M);S(PodShapeState49W);S(VehicleFrameFields49W);S(VehicleFramePhase49W);
    S(VehicleAnimationWorkspace49T);S(VehicleVisualWorkspace49R);S(ChariotLifecycleWorkspace49V);S(GraphBodyWorkspace49W);S(FrameTimingState49I);S(FrameElapsed49I);S(FrameReader49K);S(PostPlayerAt217DE0_49K);S(PostWorld49W);S(Post354_49W);S(ContactEffects49W);S(NoHitQuery49W);S(SecondaryInputs49M);S(SubstepSelectionContext49K);S(PlayerActionCodes48X);S(SceneNode49P);S(MotionView49O);S(MotionNodeBindings49O);S(ChariotIkPair49T);S(PodShapeSource49W);
    S(b::Immutable);S(b::Running);S(b::Expected);S(Fixture);S(std::vector<std::uint8_t>);
#undef S
}
int main(int argc,char**argv){
    if(argc==2&&!std::strcmp(argv[1],"--sizes")){sizes();return 0;}
    b::need(argc==4,"bench fixture histories synthetic-support|no-support");b::need(std::fesetround(FE_TONEAREST)==0,"nearest");
    const unsigned histories=unsigned(std::strtoul(argv[2],nullptr,10));b::need(histories<=10000,"bounded histories");
    const bool support=!std::strcmp(argv[3],"synthetic-support");b::need(support||!std::strcmp(argv[3],"no-support"),"scenario");const unsigned selected=support?0:1;
    auto fixture=std::make_unique<Fixture>();read(argv[1],*fixture);const auto&f=*fixture;
    auto owner=std::make_unique<b::Running>();auto&r=*owner;const auto immutable=immutableHash(f);
    b::U steps=0,physicalSteps=0,transitions=0;
    for(unsigned scenario=0;scenario<2;++scenario){b::restart(r,f.initial);
        for(b::U frame=0;frame<b::frames;++frame){const auto&e=f.expected[scenario][frame];
            same(r.state,frame?f.expected[scenario][frame-1].after:f.initial,frame,"before");const b::U beforePhysical=r.state.bodies.vehicle838,beforeVirtual=r.state.lifecycle.vehicle83C;
            b::need(b::graphics(r,f.data,frame),"preflight graphics admitted");same(r.state,e.graphics,frame,"graphics");
            b::U step=0;do{b::need(step<e.substeps&&step<4,"expected substep extent");b::Support supplied{};b::need(b::substep(r,f.data,frame,scenario==0,&supplied),"preflight substep admitted");b::need(b::equal(supplied,e.support[step]),"exact support words");if(scenario==selected){++steps;physicalSteps+=supplied.physical!=0;}++step;}while(r.state.phase!=b::VehicleFramePhase49W::Ready);
            b::need(step==e.substeps,"all expected substeps");same(r.state,e.after,frame,"after");
            if(scenario==selected)transitions+=beforePhysical!=r.state.bodies.vehicle838||beforeVirtual!=r.state.lifecycle.vehicle83C;
        }
    }
    b::need(immutableHash(f)==immutable,"immutable descriptors/MOT/IK preflight");
    b::H nanoseconds=0,checksum=0;bool success=true;
    for(unsigned history=0;history<histories;++history){b::restart(r,f.initial);
#ifdef PROBE49W
        counting=true;
#else
        const auto start=std::chrono::steady_clock::now();
#endif
        for(b::U frame=0;frame<b::frames;++frame)success=b::advance(r,f.data,frame,support)&&success;
#ifdef PROBE49W
        counting=false;
#else
        nanoseconds+=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-start).count();
#endif
        b::need(success,"timed native history admitted");same(r.state,f.expected[selected].back().after,143,"timed-final");checksum+=b::hash(r.state);
    }
    b::need(immutableHash(f)==immutable,"immutable descriptors/MOT/IK histories");
#ifdef PROBE49W
    b::need(allocated==0,"hot ordinary new allocation");const b::H count=allocated;const unsigned counter=1;
#else
    const b::H count=0;const unsigned counter=0;
#endif
    std::printf("scenario=%s histories=%u frames=%u nanoseconds=%llu checksum=%llu final_state=%llu cpp_new_allocations=%llu allocation_counter_enabled=%u retained_motion_bytes=%zu preflight_frames=288 preflight_phase_snapshots=864 validated_substeps=%u validated_physical_substeps=%u validated_transitions=%u\n",argv[3],histories,histories*b::frames,(unsigned long long)nanoseconds,(unsigned long long)checksum,(unsigned long long)b::hash(f.expected[selected].back().after),(unsigned long long)count,counter,f.motionBytes,steps,physicalSteps,transitions);
}
