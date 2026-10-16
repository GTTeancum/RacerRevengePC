#pragma once
#include "original_wreck_assets50la.h"
#include "original_wreck_visibility50la.h"
#include "original_wreck_components50la.h"
#include "original_wreck_splash50la.h"
#include "original_wreck_explosion50la.h"
#include "original_gc_pod_dust50kg.h"
#include "original_gc_pod_sparks50kg.h"
#include "../assets/effect_cold50bj.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rr::platform {class VehicleWindowGl50DS;struct VehicleWindowDiagnostic50DS;}
namespace rr::game {
struct OriginalRendererDispatchFields;
// Actual session resource-context dependency. These are borrowed original
// member bytes and the SAME scene manager/process random owners as other race
// effects. This record never creates an independent pool, seed or clock.
struct OriginalWreckEffectResourceContext50LA {
    OriginalGcParticleManager50KG* particleManager=nullptr;
    const std::vector<std::uint8_t>* podWreckDustEmt=nullptr;
    const std::vector<std::uint8_t>* podWreckSplashEmt=nullptr;
    const std::vector<std::uint8_t>* podWreckSparksEmt=nullptr;
    const std::vector<std::uint8_t>* podExplosionEmt=nullptr;
    std::uint32_t* sharedRawRandom=nullptr;
    SharedRandomOwner50ID* sharedRandomOwner=nullptr;
    HostMonotonicClock50ID* hostClock=nullptr;
    OriginalGcPodSparksGlobals50KG* sparksGlobals=nullptr;
    OriginalWreckExplosionGlobals50LA* explosionGlobals=nullptr;
};
enum class OriginalWreckEffectSlot50LA : std::uint8_t {dust168,splash16C,sparksC0,explosionC4};
struct OriginalWreckEffectComponentBinding50LA {
    // Mutable view of the actual external parent forest. Every existing
    // ancestor/sibling must be included; future unconstructed slots must not.
    OriginalWreckComponentDomain50LA* domain=nullptr;
    OriginalWreckComponent50LA** domainStorage=nullptr;
    std::size_t domainCapacity=0;
    OriginalWreckComponent50LA* particleParent=nullptr;
    std::uint32_t* actualObjectCount423F60=nullptr;
    const std::uint32_t* actualAttachmentLock423F58=nullptr;
    std::array<std::uint64_t,4> componentKeys{};
    std::array<std::uint32_t,4> controlOwnerIds{};
};
struct OriginalWreckEffectRendererBinding50LA {
    // Address of the actual observer19C->child04's A4 field binding, updated
    // by its real renderer owner when that child changes. This is not a copy
    // of a visible boolean. Missing binding stops the reached attached gate.
    const std::uint16_t* const* actualChild04TagA4Field=nullptr;
    const std::uint16_t* actualCurrentTag8BE0=nullptr;
};
struct OriginalWreckEffectsReceipt50LA {
    OriginalGcSprayStatus50KG status=OriginalGcSprayStatus50KG::unprepared;
    std::uint32_t sourcePC=0;
    std::size_t constructedChildren=0;
    bool sourcePrefixCommitted=false;
    bool okay()const noexcept{return status==OriginalGcSprayStatus50KG::complete;}
};
// Four actual owners for the authenticated POD02 one-piece/count174=0 route:
// piece168 dust, piece16C splash, groupC0 sparks, groupC4 explosion. Nonzero
// damage-emitter arrays160/164 remain an explicit pre-construction boundary.
class OriginalWreckEffects50LA final {
public:
    OriginalWreckEffects50LA()=default;
    OriginalWreckEffects50LA(const OriginalWreckEffects50LA&)=delete;
    OriginalWreckEffects50LA& operator=(const OriginalWreckEffects50LA&)=delete;
    // Cold parse/allocation only, before source constructors publish anything.
    OriginalWreckEffectsReceipt50LA prepareCold(const OriginalWreckEffectResourceContext50LA&,
        assets::OriginalUiPageDiagnostic50KG&) noexcept;
    OriginalWreckEffectsReceipt50LA constructPiece(const OriginalWreckEffectComponentBinding50LA&,
        std::uint32_t actualCount174) noexcept;
    OriginalWreckEffectsReceipt50LA constructGroup(const OriginalWreckEffectComponentBinding50LA&,
        OriginalWreckAssets50LA&,OriginalWreckVisibility50LA&,
        const OriginalWreckEffectRendererBinding50LA&) noexcept;
    OriginalWreckComponent50LA& component(OriginalWreckEffectSlot50LA s) noexcept {
        return components_[static_cast<unsigned>(s)];
    }
    EffectOwner50BE* control(OriginalWreckEffectSlot50LA s) noexcept;
    bool pieceConstructed()const noexcept{return constructedChildren_>=2;}
    bool groupConstructed()const noexcept{return constructedChildren_==4;}
    std::size_t constructedChildren()const noexcept{return constructedChildren_;}
    std::size_t initializedChildren()const noexcept{return initializedChildren_;}
    bool coherent()const noexcept;
    OriginalWreckEffectsReceipt50LA start80(OriginalWreckEffectSlot50LA) noexcept;
    OriginalWreckEffectsReceipt50LA stop84(OriginalWreckEffectSlot50LA) noexcept;
    OriginalWreckEffectsReceipt50LA reset88(OriginalWreckEffectSlot50LA) noexcept;
    //2CC9C0 prefix: sparks88/84, explosion88/84, primary splash84. The
    // caller then runs real group141520. Count1740 piece2CBEB0 performs no
    // effect work at all; it must not invent dust/splash cleanup calls.
    OriginalWreckEffectsReceipt50LA deactivateGroupEffects() noexcept;
    // Actual141F4C renderer20(0) prelude followed by the concrete child38.
    // Borrows the same application renderer fields/window as the parent; a
    // queued callback/DMA path remains an explicit source boundary. A blocked
    // prelude has not run common and may be resumed without replaying effects.
    OriginalGcSprayReceipt50KG advanceRegisteredChild(
        OriginalWreckComponent50LA& actualChild,float elapsedSeconds,bool paused,
        const OriginalRendererDispatchFields&,platform::VehicleWindowGl50DS&,
        platform::VehicleWindowDiagnostic50DS&) noexcept;
    // Concrete source virtual38 dispatch. The actual external parent's141ED0
    // walker must first perform its real renderer442F70 virtual20(0), then
    // call this for the exact selected child; no renderer-complete flag or
    // fake callback is accepted here. This method never traverses or silently
    // skips unknown external siblings and is NOT a replacement parent walker.
    // It runs common exactly once; only returned-zero common reaches derived.
    OriginalGcSprayReceipt50KG advanceRegisteredChildAfterParentDispatch(
        OriginalWreckComponent50LA& actualChild,float elapsedSeconds,bool paused) noexcept;
private:
    OriginalWreckEffectResourceContext50LA context_{};
    OriginalWreckEffectComponentBinding50LA binding_{};
    OriginalWreckEffectRendererBinding50LA renderer_{};
    OriginalWreckAssets50LA* modelAssets_=nullptr;
    OriginalWreckVisibility50LA* modelVisibility_=nullptr;
    SceneGraph49P groupAttachmentGraph_{}; // stable descriptor of the actual model graph
    std::array<OriginalGcCommonAttachment50KG,2> groupAttachments_{};
    OriginalGcPodDustDefinition50KG dustDefinition_{};
    OriginalWreckSplashDefinition50LA splashDefinition_{};
    OriginalGcPodSparksDefinition50KG sparksDefinition_{};
    OriginalWreckExplosionDefinition50LA explosionDefinition_{};
    OriginalGcPodDustRuntime50KG dust_{};
    OriginalWreckSplashRuntime50LA splash_{};
    OriginalGcPodSparksRuntime50KG sparks_{};
    OriginalWreckExplosionRuntime50LA explosion_{};
    std::array<OriginalWreckComponent50LA,4> components_{};
    std::array<EffectOwner50BE,4> owners_{};
    std::array<float,4> delays1DC_{};
    std::size_t initializedChildren_=0,constructedChildren_=0;
    bool preparationAttempted_=false,prepared_=false,constructionFailed_=false,runtimeFailed_=false;
    OriginalGcSprayReceipt50KG stoppedAt_{};
    OriginalWreckEffectsReceipt50LA result(OriginalGcSprayStatus50KG,std::uint32_t)const noexcept;
    bool binding(const OriginalWreckEffectComponentBinding50LA&,bool same)const noexcept;
    OriginalWreckEffectsReceipt50LA constructOne(unsigned) noexcept;
    const OriginalGcCommonEmitterTiming50KG& timing(unsigned)const noexcept;
    bool rendererGate(OriginalWreckExplosionAttachmentGate50LA&)const noexcept;
};
} // namespace rr::game
