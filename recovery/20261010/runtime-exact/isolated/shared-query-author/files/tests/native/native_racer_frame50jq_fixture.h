#pragma once
// Explicit native storage adaptation for the bounded all-original-AI lane.
// These are permanent typed racer records. They are never restored from Bank.
// Slot0 borrows the one pre-existing development Fixture; the other slots own
// records only, borrowing the same immutable Assets and serial scratch workspace.
#include "runtime_startup50fl_fixture.h"
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_collision50hw_fixture.h"
#include <array>
#include <cstddef>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace native_racer50JQ {
using namespace rr::game;
struct SceneIdentity {
    std::uint32_t value=0;
    SceneNodeIndex49P node=SceneNodeIndex49P::none;
};
struct PhysicalAuxiliary;
struct Context {
    VehicleFrameState49W& state;
    VehicleFrameParameters49W& parameters;
    VehicleFrameBindings49W& bindings;
    ChariotIkPair49T& handRecords;
    SecondaryInputs49M& secondaryInputs;
    SubstepSelectionContext49K& selectors;
    unsigned& frame;
    VehicleFrameDiagnostic49W& diagnostic;
    PlayerActionCodes48X& actionCodes;
    bool& configurationEntryNonzero;
    runtime_startup50fl::Assets& assets; // Immutable source bytes, shared for life.
    VehicleFrameWorkspace49W& workspace; // Throwaway scratch, serial lease only.
    const std::size_t slot;
    // Real podboxf owner, including triangle mesh/radii/depth, not just bounds.
    const native_racer_collision50HW::Target* actualPod694Target=nullptr;
    SceneIdentity mainScene{},secondaryScene{}; // Opaque record40 IDs, not graph indices.
    PhysicalAuxiliary* physical=nullptr; // Bound once at final cold auxiliary setup.

    Context(const Context&)=delete;
    Context& operator=(const Context&)=delete;
    Context(Context&&)=delete;
    Context& operator=(Context&&)=delete;
    Context(VehicleFrameState49W& s,VehicleFrameParameters49W& p,
        VehicleFrameBindings49W& b,ChariotIkPair49T& h,SecondaryInputs49M& secondary,
        SubstepSelectionContext49K& select,unsigned& f,VehicleFrameDiagnostic49W& d,
        PlayerActionCodes48X& actions,bool& configured,runtime_startup50fl::Assets& a,
        VehicleFrameWorkspace49W& w,std::size_t index) noexcept
        :state(s),parameters(p),bindings(b),handRecords(h),secondaryInputs(secondary),
         selectors(select),frame(f),diagnostic(d),actionCodes(actions),
         configurationEntryNonzero(configured),assets(a),workspace(w),slot(index) {}

    // Called after this slot's actual rig.install and authored handling bind.
    // Exact source words are compared; no recomputation or substitute mesh.
    void bindPod694(const native_racer_collision50HW::Owner& source) {
        if(actualPod694Target||!source.prepared()||state.phase!=VehicleFramePhase49W::Ready||
           state.bodies.main.frameFactor!=0.f||bindings.animation.handRecords!=&handRecords)
            throw std::runtime_error("canonical racer pod694 requires its fresh prepared owner");
        const auto& target=source.mainTarget();
        if(std::memcmp(target.source.local40.v,parameters.shape.local40.v,sizeof parameters.shape.local40.v)||
           std::memcmp(target.source.rootCenter.v,parameters.shape.rootCenter.v,sizeof parameters.shape.rootCenter.v)||
           std::memcmp(target.source.rootHalf.v,parameters.shape.rootHalf.v,sizeof parameters.shape.rootHalf.v))
            throw std::runtime_error("canonical racer pod694 source differs from authored parameters");
        actualPod694Target=&target;
    }
};

class Fleet final {
    struct Records {
        VehicleFrameState49W state{};
        VehicleFrameParameters49W parameters{};
        VehicleFrameBindings49W bindings{};
        ChariotIkPair49T handRecords{};
        SecondaryInputs49M secondaryInputs{};
        SubstepSelectionContext49K selectors{};
        unsigned frame=0;
        VehicleFrameDiagnostic49W diagnostic{};
        PlayerActionCodes48X actionCodes{};
        bool configurationEntryNonzero=false;
    };
    std::array<std::unique_ptr<Records>,native_racer50HN::maximumRacers> owned_{};
    std::array<std::unique_ptr<Context>,native_racer50HN::maximumRacers> contexts_{};
    runtime_startup50fl::Fixture* baseline_=nullptr;
    std::size_t count_=0;
    bool registered_=false;

    // One-time baseline clone, before the first racer factory/step. Frame noise
    // is deliberately excluded; it has no per-racer cold value or authority.
    // The only retained frame pointers are the unbound water auxiliary record;
    // every source binding is installed later by this racer's real cold path.
    static void cloneFrame(const VehicleFrameState49W& from,VehicleFrameState49W& to) noexcept {
        static_assert(std::is_standard_layout_v<VehicleFrameState49W>);
        static_assert(std::is_trivially_copyable_v<VehicleFrameState49W>);
        constexpr auto first=offsetof(VehicleFrameState49W,noise);
        constexpr auto end=first+sizeof(CoupledNoiseContext49M);
        auto* destination=reinterpret_cast<unsigned char*>(&to);
        const auto* source=reinterpret_cast<const unsigned char*>(&from);
        std::memcpy(destination,source,first);
        std::memcpy(destination+end,source+end,sizeof(VehicleFrameState49W)-end);
    }
    template<class T,class F> void registerRecord(native_racer50HN::Bank& bank,F field) {
        std::array<const void*,native_racer50HN::maximumRacers> addresses{};
        for(std::size_t i=0;i<count_;++i)addresses[i]=&field(*contexts_[i]);
        bank.canonicalReadFamily(addresses,count_,sizeof(T));
    }
public:
    Fleet()=default;
    Fleet(const Fleet&)=delete;
    Fleet& operator=(const Fleet&)=delete;
    Fleet(Fleet&&)=delete;
    Fleet& operator=(Fleet&&)=delete;
    // Expose the actual selected records immediately, before initial shared
    // setup/RNG. Other slots do not exist until the final baseline is ready.
    void attachSlotZero(runtime_startup50fl::Fixture& baseline) {
        if(count_||baseline_)throw std::runtime_error("canonical slot0 already attached");
        auto context=std::make_unique<Context>(baseline.state,baseline.parameters,baseline.bindings,
            baseline.handRecords,baseline.secondaryInputs,baseline.selectors,baseline.frame,
            baseline.diagnostic,baseline.actionCodes,baseline.configurationEntryNonzero,
            baseline.assets,baseline.workspace,0);
        contexts_[0]=std::move(context);baseline_=&baseline;count_=1;
    }
    // Allocate all permanent storage before any new racer is constructed.
    // This deliberately preserves the established caller-supplied baseline;
    // rig/handling/probe factories must still run separately for every slot.
    void finalizeBaseline(std::size_t count) {
        if(!baseline_||count_!=1)throw std::runtime_error("canonical baseline finalization phase");
        auto& baseline=*baseline_;
        const auto& auxiliary=baseline.state.terrainProbe.auxiliary50JC;
        if(count<2||count>native_racer50HN::maximumRacers||baseline.frame||
           baseline.state.phase!=VehicleFramePhase49W::Ready||
           baseline.state.bodies.main.frameFactor!=0.f||
           baseline.bindings.animation.handRecords!=&baseline.handRecords||
           auxiliary.phase!=TerrainAuxiliaryPhase50JC::Unbound||
           auxiliary.bindings.waterQuery||auxiliary.bindings.waterGlobals||
           auxiliary.bindings.effective434260||auxiliary.bindings.actualTrack210770AndSetup21207CBound)
            throw std::runtime_error("canonical racer fleet requires the unadvanced pre-factory baseline");
        // Local arrays make allocation failure leave the selected owner intact.
        decltype(owned_) owned;
        decltype(contexts_) contexts;
        for(std::size_t i=1;i<count;++i){
            owned[i]=std::make_unique<Records>();auto& r=*owned[i];
            cloneFrame(baseline.state,r.state);
            // The selected baseline already owns its attached694. A different
            // fresh11B190 owner has E0=0 from11B488 until its own120230 attach.
            // This is cold ownership rebinding, never a live detach/reset.
            r.state.shape.registrationE0=0;
            r.parameters=baseline.parameters;r.bindings=baseline.bindings;
            r.handRecords=baseline.handRecords;r.secondaryInputs=baseline.secondaryInputs;
            r.selectors=baseline.selectors;r.frame=baseline.frame;r.diagnostic=baseline.diagnostic;
            r.actionCodes=baseline.actionCodes;r.configurationEntryNonzero=baseline.configurationEntryNonzero;
            r.bindings.animation.handRecords=&r.handRecords;
            r.bindings.podLiveQuery50IZ=nullptr;
            contexts[i]=std::make_unique<Context>(r.state,r.parameters,r.bindings,r.handRecords,
                r.secondaryInputs,r.selectors,r.frame,r.diagnostic,r.actionCodes,
                r.configurationEntryNonzero,baseline.assets,baseline.workspace,i);
        }
        // Each slot must bind its own permanent accepted-world context. Keeping
        // the old arena pointer even in slot0 would mask a missing caller join.
        baseline.bindings.podLiveQuery50IZ=nullptr;
        // Keep Context0 itself stable: early provider bindings may name it.
        for(std::size_t i=1;i<count;++i){owned_[i]=std::move(owned[i]);contexts_[i]=std::move(contexts[i]);}
        count_=count;
    }
    std::size_t count()const noexcept{return count_;}
    Context& context(std::size_t slot) {
        if(slot>=count_)throw std::runtime_error("canonical racer context index");
        return *contexts_[slot];
    }
    const Context& context(std::size_t slot)const {
        if(slot>=count_)throw std::runtime_error("canonical racer context index");
        return *contexts_[slot];
    }
    // The session caller owns the global scene-ID namespace. This validates
    // all racer pairs without equating IDs to graph indices or CollectionRefs.
    // Call after real rig installation; caller also checks non-racer collisions.
    void bindSceneIdentities(std::size_t slot,std::uint32_t mainId,std::uint32_t secondaryId) {
        auto& c=context(slot);
        const auto main=c.bindings.lifecycle.main190,secondary=c.bindings.lifecycle.physical19C;
        const auto valid=[&](SceneNodeIndex49P node){
            const auto n=static_cast<std::int32_t>(node);
            return n>=0&&static_cast<std::uint32_t>(n)<c.state.sceneNodeCount;
        };
        if(!mainId||!secondaryId||mainId==secondaryId||c.mainScene.value||c.secondaryScene.value||
           !valid(main)||!valid(secondary)||main==secondary||
           c.bindings.animation.bodyScene288!=main||c.state.phase!=VehicleFramePhase49W::Ready||
           c.state.bodies.main.frameFactor!=0.f)
            throw std::runtime_error("canonical racer scene identity binding");
        for(std::size_t i=0;i<count_;++i){const auto& other=*contexts_[i];
            if(mainId==other.mainScene.value||mainId==other.secondaryScene.value||
               secondaryId==other.mainScene.value||secondaryId==other.secondaryScene.value)
                throw std::runtime_error("canonical racer scene identities overlap");
        }
        c.mainScene={mainId,main};c.secondaryScene={secondaryId,secondary};
    }
    // ReadHandle compatibility only. Bank stores addresses, never any byte of
    // these physical owners. Noise is neither observed nor saved/restored.
    void registerBankReads(native_racer50HN::Bank& bank) {
        if(!count_||registered_)throw std::runtime_error("canonical racer read registration");
        constexpr auto first=offsetof(VehicleFrameState49W,noise);
        constexpr auto end=first+sizeof(CoupledNoiseContext49M);
        std::array<const void*,native_racer50HN::maximumRacers> addresses{};
        for(std::size_t i=0;i<count_;++i)addresses[i]=&contexts_[i]->state;
        bank.canonicalReadFamily(addresses,count_,first);
        for(std::size_t i=0;i<count_;++i)
            addresses[i]=reinterpret_cast<const unsigned char*>(&contexts_[i]->state)+end;
        bank.canonicalReadFamily(addresses,count_,sizeof(VehicleFrameState49W)-end);
        registerRecord<VehicleFrameParameters49W>(bank,[](Context& c)->auto&{return c.parameters;});
        registerRecord<VehicleFrameBindings49W>(bank,[](Context& c)->auto&{return c.bindings;});
        registerRecord<SecondaryInputs49M>(bank,[](Context& c)->auto&{return c.secondaryInputs;});
        registerRecord<ChariotIkPair49T>(bank,[](Context& c)->auto&{return c.handRecords;});
        registerRecord<SubstepSelectionContext49K>(bank,[](Context& c)->auto&{return c.selectors;});
        registerRecord<unsigned>(bank,[](Context& c)->auto&{return c.frame;});
        registerRecord<VehicleFrameDiagnostic49W>(bank,[](Context& c)->auto&{return c.diagnostic;});
        registered_=true;
    }
};
} // namespace native_racer50JQ
