// One focused cold-construction/binding check. Default build is native only.
// Opt-in baseline build contains the separately retained reference adapters;
// they are never an include/link dependency of the interactive executable.
#ifdef RR_STARTUP50FL_COMPARE_BASELINE
#include "../../evidence/frameBridge49W/frame/frame_fixture.h"
#include "../../evidence/frameBridge49W/connected/authored_inputs49w.h"
#endif
#include "runtime_startup50fl_fixture.h"
#include <chrono>
#include <iostream>
#include <type_traits>
namespace native=runtime_startup50fl;
using namespace rr::game;
static unsigned startupChecks=0;
static void check(bool ok,const char* message){++startupChecks;if(!ok)throw std::runtime_error(message);}
template<class T>static void same(const T& a,const T& b,const char* message){
    static_assert(std::is_trivially_copyable_v<T>);
    if(std::memcmp(&a,&b,sizeof(T))){
        const auto* x=reinterpret_cast<const unsigned char*>(&a);
        const auto* y=reinterpret_cast<const unsigned char*>(&b);
        for(std::size_t i=0;i<sizeof(T);++i)if(x[i]!=y[i]){
            std::cerr<<message<<" byte "<<i<<" native="<<unsigned(x[i])<<" baseline="<<unsigned(y[i])<<'\n';break;
        }
        check(false,message);
    }++startupChecks;
}
static FreshPodGeometryResult49O geometry(const native::Fixture& owner){
    FreshPodGeometrySources49O sources{};sources.chariot=owner.assets.hierarchy[2];sources.rightEngine=owner.assets.hierarchy[1];
    sources.forward=owner.assets.motions[4];sources.back=owner.assets.motions[5];rr::assets::MotionBindingDiagnostic49O bindingDiagnostic{};
    check(rr::assets::readMotionNodeBindings49O(owner.assets.hierarchy[2],reinterpret_cast<const char*>(owner.assets.vut[2].data()),owner.assets.vut[2].size(),sources.chariotBindings,bindingDiagnostic),"geometry bindings");
    FreshPodGeometryWorkspace49O workspace{};FreshPodGeometryResult49O result{};FreshPodGeometryDiagnostic49O diagnostic{};
    check(buildFreshPodGeometry49O(sources,workspace,result,diagnostic),"native fresh geometry");return result;
}
int main(){try{
    check(std::fesetround(FE_TONEAREST)==0,"rounding");
    const auto started=std::chrono::steady_clock::now();native::Fixture owner(0);
    const auto constructorMicros=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-started).count();
    check(owner.bindings.animation.handRecords==&owner.handRecords,"owned IK records");
    check(owner.assets.hierarchy[2].nodeCount==18&&owner.state.phase==VehicleFramePhase49W::Ready,"native cold owner");
#ifdef RR_STARTUP50FL_COMPARE_BASELINE
    const auto baselineStarted=std::chrono::steady_clock::now();frame49w_test::Fixture baseline(0);
    const auto baselineMicros=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-baselineStarted).count();
    same(owner.state,baseline.state,"constructor state");same(owner.parameters,baseline.parameters,"constructor parameters");
    same(owner.selectors,baseline.selectors,"constructor selectors");same(owner.secondaryInputs,baseline.secondaryInputs,"constructor secondary inputs");
    same(owner.handRecords,*baseline.bindings.animation.handRecords,"computed IK records");
    auto normalized=baseline.bindings;
    normalized.animation.chariotRows=owner.bindings.animation.chariotRows;normalized.animation.leftRows=owner.bindings.animation.leftRows;normalized.animation.rightRows=owner.bindings.animation.rightRows;
    normalized.animation.brakeLeft28C=owner.bindings.animation.brakeLeft28C;normalized.animation.brakeRight290=owner.bindings.animation.brakeRight290;
    normalized.animation.forward280=owner.bindings.animation.forward280;normalized.animation.back284=owner.bindings.animation.back284;
    normalized.animation.left2A0=owner.bindings.animation.left2A0;normalized.animation.right29C=owner.bindings.animation.right29C;normalized.animation.handRecords=&owner.handRecords;
    same(owner.bindings,normalized,"constructor bindings (owner pointers normalized)");
    for(unsigned i=0;i<3;++i)same(owner.assets.rows[i],baseline.assets.rows[i],"authored motion rows");
#endif
    const auto fresh=geometry(owner);const auto phy=native::readBytes("work/impactAuthoring49Y/Pod01.phy");
    const AuthoredSelectionContext49N selection{AuthoredClass49N::A,false,true,false,false,{}};
    const auto bound=native::bind(phy,fresh,selection,owner.parameters,owner.state,owner.selectors,owner.secondaryInputs);
    check(bound.selectedClass==AuthoredClass49N::A&&bound.usedAuthoredClassValues,"authored class selection");
    check(owner.parameters.bodies.main.body.mass>0&&owner.parameters.bodies.secondary.body.mass>0,"bound native masses");
#ifdef RR_STARTUP50FL_COMPARE_BASELINE
    const auto retained=frame49w_authored::bind(phy,fresh,selection,baseline.parameters,baseline.state,baseline.selectors,baseline.secondaryInputs);
    same(bound,retained.binding,"authored binding");same(owner.state,baseline.state,"bound state");
    same(owner.parameters,baseline.parameters,"bound parameters");same(owner.selectors,baseline.selectors,"bound selectors");same(owner.secondaryInputs,baseline.secondaryInputs,"bound secondary inputs");
    std::cout<<"retained baseline constructor_us="<<baselineMicros<<" binder_reference_instructions="<<(retained.parameterInstructions+retained.geometryInstructions+retained.jointInstructions)<<'\n';
#endif
    std::cout<<"runtime_startup50fl PASS "<<startupChecks<<" focused checks; native_constructor_us="<<constructorMicros<<" native_owner_bytes="<<sizeof(owner)<<"; no game or device launch\n";
    return 0;
}catch(const std::exception& error){std::cerr<<"runtime_startup50fl FAIL "<<startupChecks<<": "<<error.what()<<'\n';return 1;}}
