// Source-only candidate. Not compiled, linked, or run.
// Focused canonical B9D bit0 coverage using the existing native cold fixture.
// No guest/original-word adapter, stored state image, gameplay driver, or
// optional-probe reference JSON is included. The fixture's development inputs
// remain explicit component inputs, not a recovered full game constructor.
#include "runtime_startup50fl_fixture.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <new>
#include <type_traits>

namespace {
bool watchAllocations=false;
std::size_t hotAllocations=0,checks=0,nativeCalls=0;

void check(bool okay,const char* message) {
    ++checks;
    if(!okay)throw std::runtime_error(message);
}
template<class T> void copy(T& destination,const T& source) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);
    std::memcpy(&destination,&source,sizeof(T));
}
template<class T> void same(const T& actual,const T& expected,const char* message) {
    static_assert(std::is_trivially_copyable_v<T>);
    check(std::memcmp(&actual,&expected,sizeof(T))==0,message);
}
template<class F> bool hot(F&& operation) {
    const auto before=hotAllocations;
    struct Guard {
        Guard() { watchAllocations=true; }
        ~Guard() { watchAllocations=false; }
    };
    bool okay;
    {
        Guard guard;
        ++nativeCalls;
        okay=operation();
    }
    check(hotAllocations==before,"native stage allocated");
    return okay;
}
rr::game::PlayerActionResult48X noAction(void*,std::int32_t,std::uint32_t) noexcept {
    return rr::game::PlayerActionResult48X::NoMatch;
}
}

// Match the existing native component tests' ordinary C++ allocation watch.
// Construction, snapshots and diagnostic output are outside hot calls.
void* operator new(std::size_t bytes) {
    if(watchAllocations)++hotAllocations;
    if(void* value=std::malloc(bytes?bytes:1))return value;
    throw std::bad_alloc();
}
void* operator new[](std::size_t bytes) { return ::operator new(bytes); }
void operator delete(void* value) noexcept { std::free(value); }
void operator delete[](void* value) noexcept { std::free(value); }
void operator delete(void* value,std::size_t) noexcept { std::free(value); }
void operator delete[](void* value,std::size_t) noexcept { std::free(value); }

int main(int argc,char** argv) { try {
    if(argc!=2) {
        std::cerr<<"usage: vehicle_b9d_native_check NATIVE_ASSET_ROOT\n";
        return 2;
    }
    using namespace rr::game;
    namespace startup=runtime_startup50fl;
    check(std::fesetround(FE_TONEAREST)==0,"rounding mode");
    auto owner=std::make_unique<startup::Fixture>(0,std::filesystem::path(argv[1]));
    auto& state=owner->state;
    check(state.phase==VehicleFramePhase49W::Ready,"native fixture starts Ready");
    check(!state.fields.flagB9D0,"native cold owner clears represented B9D bit0");
    // Existing explicit no-partner/no-action component context, also used by
    // checkB9DCheckpointJoin in portal_recovery50fv_check.cpp.
    state.impactSupplement.partnerId=0xffffffffu;
    FrameReader49K reader{};
    reader.packet={128,128,128,128};
    reader.preparedFactor=.016f;
    reader.codes=owner->actionCodes;
    reader.actions={nullptr,noAction};

    auto cold=std::make_unique<VehicleFrameState49W>();
    auto ready=std::make_unique<VehicleFrameState49W>();
    auto expected=std::make_unique<VehicleFrameState49W>();
    copy(*cold,state);
    unsigned readerCases=0,tailCases=0;
    for(unsigned bit=0;bit<2;++bit)for(unsigned byte=0;byte<256;++byte) {
        copy(state,*cold);
        // Controlled input-reader cases, not asserted gameplay-produced flags.
        state.fields.applicationByte58=0;
        state.fields.flagB9D0=bit!=0;
        state.bodies.main.controls.player.flagsB9C=static_cast<std::uint8_t>(byte);
        check(hot([&] {
            return readVehicleFrame49W(state,owner->bindings,reader,
                owner->workspace,owner->diagnostic);
        }),"native B9C reader");
        check(state.phase==VehicleFramePhase49W::PostPlayer,"reader phase reached");
        // Independently stated gate-byte-zero contract from readPlayerInput48X.
        check(state.bodies.main.controls.player.flagsB9C==
            static_cast<std::uint8_t>(byte&0xf8u),"reader rewrites only B9C low bits");
        check(state.fields.flagB9D0==(bit!=0),"B9C reader preserves independent B9D");
        ++readerCases;
    }

    // Reach the tail through the existing native prefix and its two explicit
    // preserving-stage component assumptions. startup::cut supplies
    // {point,true,true}; it does not execute the omitted original source stages.
    // No phase is assigned to live input, and no body substep is run.
    copy(state,*cold);
    check(hot([&] {
        return readVehicleFrame49W(state,owner->bindings,reader,
            owner->workspace,owner->diagnostic);
    }),"native prefix reader");
    PostPlayerAt217DE0_49K post{};
    post.player=state.bodies.main.controls.player;
    post.bindingContext.configurationEntryNonzero=true;
    check(hot([&] {
        return bindVehicleFrame49W(state,owner->bindings,owner->parameters,
            startup::cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,
            owner->workspace,owner->diagnostic);
    }),"native prefix binding");
    check(hot([&] {
        return graphicsVehicleFrame49W(state,owner->bindings,owner->parameters,
            startup::cut(PreservingCut49W::Base2176E0BeforeGraphics),
            owner->workspace,owner->diagnostic);
    }),"native prefix graphics");
    ImpactConsumerInput49Z consumer{};
    consumer.managerFlags58=2;consumer.profile34=2;consumer.finalDamageThreshold64=1;
    VehicleImpactEvents49Z events{};
    check(hot([&] {
        return consumeVehicleImpacts49Z(state,owner->bindings,consumer,events,
            owner->workspace,owner->diagnostic);
    }),"native prefix impact consumers");
    check(events.count==0&&events.partners.count==0,"no unconsumed impact events");
    check(hot([&] {
        return repairVehicleImpacts50A(state,owner->bindings,
            owner->workspace,owner->diagnostic);
    }),"native prefix repair");
    ImpactEffectsInput50B effects{};
    effects.models[0].node=owner->bindings.visual.left1BC;
    effects.models[1].node=owner->bindings.visual.right1B8;
    effects.models[2].node=effects.models[3].node=SceneNodeIndex49P::none;
    ImpactEffectsWorkspace50B effectWorkspace{};
    std::size_t effectCount=0;
    check(hot([&] {
        return effectsVehicleImpacts50O(state,owner->bindings,effects,
            effectWorkspace,nullptr,0,effectCount,owner->workspace,owner->diagnostic);
    }),"native prefix absent-effect-owner branch");
    check(effectCount==0,"no unconsumed backend effect events");
    OptionalProbeOwners50P absent{};
    OptionalProbeResult50P optional{};
    check(hot([&] {
        return optionalProbeVehicleImpacts50P(state,owner->bindings,absent,optional,
            owner->workspace,owner->diagnostic);
    }),"native prefix absent optional owner");
    check(optional.count==0,"no optional-probe event requests");
    check(state.phase==VehicleFramePhase49W::PostOptionalProbeEffects50P,
        "canonical tail phase genuinely reached");
    check(!state.fields.flagB9D0,"native prefix preserves cold-clear B9D");
    copy(*ready,state);

    for(unsigned bit=0;bit<2;++bit)for(unsigned byte=0;byte<256;++byte) {
        copy(state,*ready);
        // Controlled healthy damage isolates the documented B9D gate. These
        // chosen flags are not evidence that an original collision/reset ran.
        state.fields.damage7D0=state.fields.damage7D4=0.f;
        state.fields.damage7D8=state.fields.damage7DC=0.f;
        state.fields.flagB9D0=bit!=0;
        state.bodies.main.controls.player.flagsB9C=static_cast<std::uint8_t>(byte);
        copy(*expected,state);
        ImpactTransitionResult50C decision{};
        const bool okay=hot([&] {
            return decideVehicleImpacts50P(state,owner->bindings,0x3f800000u,
                decision,owner->workspace,owner->diagnostic);
        });
        if(bit) {
            check(!okay&&decision.status==ImpactTransitionStatus50C::unsupportedTransition&&
                decision.sourcePC==0x225d10u&&decision.vtableSlot==0x88u&&
                decision.originalTarget==0x2261b0u,"B9D selects genuine destruction boundary");
            check(owner->diagnostic.error==VehicleFrameError49W::ExternalCut&&
                owner->diagnostic.sourceStage==0x225d10u,"unsupported B9D diagnostic");
        } else {
            check(okay&&decision.status==ImpactTransitionStatus50C::noTransition&&
                decision.sourcePC==0x2261a4u&&decision.vtableSlot==0&&
                decision.originalTarget==0,"B9C cannot substitute for clear B9D");
            // Only the expected comparison object is changed; never live input.
            expected->phase=VehicleFramePhase49W::AfterImpactBlock;
        }
        same(state,*expected,"tail preserves canonical frame except successful phase");
        ++tailCases;
    }
    check(readerCases==512&&tailCases==512,"complete B9C/B9D cross products");
    std::cout<<"{\"b9d_reader_cases\":"<<readerCases
        <<",\"b9d_tail_cases\":"<<tailCases<<",\"checks\":"<<checks
        <<",\"native_calls\":"<<nativeCalls<<",\"hot_allocations\":"<<hotAllocations
        <<",\"guest_execution\":false,\"body_substeps\":0}\n";
    return 0;
} catch(const std::exception& error) {
    std::cerr<<"vehicle_b9d_native_check: "<<error.what()<<'\n';
    return 1;
} }
