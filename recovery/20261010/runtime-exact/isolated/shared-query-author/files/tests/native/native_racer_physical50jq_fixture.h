#pragma once
// Direct numerical vehicle supplements outside VehicleFrameState49W. They
// retain permanent per-racer identity instead of byte-restored arena authority.
#include "native_racer_frame50jq_fixture.h"
#include "../../src/game/boost_controls50fk.h"
#include "../../src/game/spring_model48k.h"
#include "../../src/game/vehicle_base_tail50x.h"
#include "../../src/game/base_pose_history50y.h"
#include "../../src/game/pod_scene_rebase50fb.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>

namespace native_racer50JQ {
struct PhysicalAuxiliary {
    BoostState50FK& boostState;
    BoostParameters50FK& boostParameters;
    BoostDiagnostic50FK& boostDiagnostic;
    float& boostCoolingCF8;
    SpringGeneratedParameters48K& settings;
    SpringResponseParameters48L& response;
    SpringGeneratedParameters48K& secondarySettings;
    SpringResponseParameters48L& secondaryResponse;
    FrameTimingState49I& clock; // One actual application140AC0 owner.
    BaseTailState50X& baseTail;
    BaseTailGlobals50X& baseGlobals; // One shared application lazy owner.
    BasePoseHistoryState50Y& poseHistory;
    rr::assets::PodFlameOwners50W& coldFlame;
    SceneNodeIndex49P (&rebaseNodes)[3][18];
    PodSceneRebaseModel50FB (&rebaseModels)[3];
};
class PhysicalAuxiliaryFleet final {
    struct Records {
        BoostState50FK boostState{};
        BoostParameters50FK boostParameters{};
        BoostDiagnostic50FK boostDiagnostic{};
        float boostCoolingCF8=0;
        SpringGeneratedParameters48K settings{};
        SpringResponseParameters48L response{};
        SpringGeneratedParameters48K secondarySettings{};
        SpringResponseParameters48L secondaryResponse{};
        BaseTailState50X baseTail{};
        BasePoseHistoryState50Y poseHistory{};
        rr::assets::PodFlameOwners50W coldFlame{};
        SceneNodeIndex49P rebaseNodes[3][18]{};
        PodSceneRebaseModel50FB rebaseModels[3]{};
    };
    std::array<std::unique_ptr<Records>,native_racer50HN::maximumRacers> records_{};
    std::array<std::unique_ptr<PhysicalAuxiliary>,native_racer50HN::maximumRacers> views_{};
    std::size_t count_=0;
    bool registered_=false;
    template<class F> void registerField(native_racer50HN::Bank& bank,F field) {
        std::array<const void*,native_racer50HN::maximumRacers> addresses{};
        for(std::size_t i=0;i<count_;++i)addresses[i]=&field(*views_[i]);
        bank.canonicalReadFamily(addresses,count_,sizeof(field(*views_[0])));
    }
    static void copyRebase(const PhysicalAuxiliary& source,Records& out) {
        std::memcpy(out.rebaseNodes,source.rebaseNodes,sizeof out.rebaseNodes);
        // Preserve borrowed immutable rig mappings. Only an actual pointer
        // into the selected baseline's local mapping is rebound to our copy.
        const auto first=reinterpret_cast<std::uintptr_t>(source.rebaseNodes);
        constexpr auto bytes=sizeof out.rebaseNodes;
        if(bytes>std::numeric_limits<std::uintptr_t>::max()-first)
            throw std::runtime_error("canonical rebase source extent overflow");
        for(unsigned i=0;i<3;++i){
            out.rebaseModels[i]=source.rebaseModels[i];auto& model=out.rebaseModels[i];
            if(!model.sourceNodes||!model.sourceNodeCount)
                throw std::runtime_error("canonical rebase model source mapping unavailable");
            const auto pointer=reinterpret_cast<std::uintptr_t>(model.sourceNodes);
            if(pointer>=first&&pointer<first+bytes){
                const auto offset=pointer-first;
                if(offset%sizeof(SceneNodeIndex49P)||
                   model.sourceNodeCount>(bytes-offset)/sizeof(SceneNodeIndex49P)||
                   model.sourceNodeCount>18u-(offset/sizeof(SceneNodeIndex49P))%18u)
                    throw std::runtime_error("canonical rebase source mapping exceeds baseline backing");
                model.sourceNodes=reinterpret_cast<const SceneNodeIndex49P*>(
                    reinterpret_cast<const unsigned char*>(out.rebaseNodes)+offset);
            }
        }
    }
public:
    PhysicalAuxiliaryFleet()=default;
    PhysicalAuxiliaryFleet(const PhysicalAuxiliaryFleet&)=delete;
    PhysicalAuxiliaryFleet& operator=(const PhysicalAuxiliaryFleet&)=delete;
    PhysicalAuxiliaryFleet(PhysicalAuxiliaryFleet&&)=delete;
    PhysicalAuxiliaryFleet& operator=(PhysicalAuxiliaryFleet&&)=delete;
    // Exactly once, after Fleet baseline finalization and before factories.
    // All input references name permanent selected owners. Source arrays and
    // live resources must not be transient operation-local projections.
    void prepare(Fleet& frames,const PhysicalAuxiliary& source) {
        const auto count=frames.count();
        if(count_||count<2||count>native_racer50HN::maximumRacers)
            throw std::runtime_error("canonical physical auxiliary setup phase");
        for(std::size_t i=0;i<count;++i)
            if(frames.context(i).physical||frames.context(i).state.phase!=VehicleFramePhase49W::Ready||
               frames.context(i).state.bodies.main.frameFactor!=0.f)
                throw std::runtime_error("canonical physical auxiliary requires fresh frame");
        decltype(records_) records;decltype(views_) views;
        views[0]=std::make_unique<PhysicalAuxiliary>(source);
        for(std::size_t i=1;i<count;++i){
            records[i]=std::make_unique<Records>();auto& r=*records[i];
            r.boostState=source.boostState;r.boostParameters=source.boostParameters;
            r.boostDiagnostic=source.boostDiagnostic;r.boostCoolingCF8=source.boostCoolingCF8;
            r.settings=source.settings;r.response=source.response;
            r.secondarySettings=source.secondarySettings;r.secondaryResponse=source.secondaryResponse;
            r.baseTail=source.baseTail;r.poseHistory=source.poseHistory;
            r.coldFlame=source.coldFlame;copyRebase(source,r);
            views[i]=std::make_unique<PhysicalAuxiliary>(PhysicalAuxiliary{
                r.boostState,r.boostParameters,r.boostDiagnostic,r.boostCoolingCF8,
                r.settings,r.response,r.secondarySettings,r.secondaryResponse,
                source.clock,r.baseTail,source.baseGlobals,r.poseHistory,r.coldFlame,
                r.rebaseNodes,r.rebaseModels});
        }
        // Pointer publication follows all fallible allocation/admission work.
        records_=std::move(records);views_=std::move(views);count_=count;
        for(std::size_t i=0;i<count_;++i)frames.context(i).physical=views_[i].get();
    }
    std::size_t count()const noexcept{return count_;}
    PhysicalAuxiliary& context(std::size_t slot) {
        if(slot>=count_)throw std::runtime_error("canonical physical auxiliary slot");
        return *views_[slot];
    }
    void registerBankReads(native_racer50HN::Bank& bank) {
        if(!count_||registered_)throw std::runtime_error("canonical physical auxiliary read registration");
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.boostState;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.boostParameters;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.boostDiagnostic;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.boostCoolingCF8;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.settings;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.response;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.secondarySettings;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.secondaryResponse;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.baseTail;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.poseHistory;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.coldFlame;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.rebaseNodes;});
        registerField(bank,[](PhysicalAuxiliary& a)->auto&{return a.rebaseModels;});
        // clock/baseGlobals are application owners; no family or snapshot.
        registered_=true;
    }
};
} // namespace native_racer50JQ
