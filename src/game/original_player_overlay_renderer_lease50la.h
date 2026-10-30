#pragma once
#include <memory>
#include <cstdint>
#include "original_application_animation_clock50kg.h"
namespace original_main_entry50KG {struct Application;class RaceDisplayLifetime50LA;}
namespace rr::game {
class OriginalPlayerConstructorPrefix50LA;class OriginalPodAIDamageOverlay50LA;
class OriginalAIDamageMaterialRow50LA;
// Private native initialized-context drain adaptation. This pins the ONE App
// Display/Video, not a source25A9C0 call receipt or frame40 history. No issuer
// outside the still-unreached conditional Player occurrence may request it.
class OriginalPlayerOverlayRendererLease50LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalPlayerConstructorPrefix50LA;
    std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> display_;
    const ::original_main_entry50KG::Application* issuer_=nullptr;
    std::uint64_t generation_=0;
    OriginalApplicationAnimationClockView50KG applicationLifetime_;
    OriginalPlayerOverlayRendererLease50LA(
        std::shared_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA>,
        const ::original_main_entry50KG::Application*)noexcept;
    bool current(bool acquiring)const noexcept;
    bool bindMaterial(OriginalAIDamageMaterialRow50LA&)const noexcept;
    bool constructArrays(OriginalPodAIDamageOverlay50LA&,std::uint32_t&)noexcept;
    bool cancelArrays(OriginalPodAIDamageOverlay50LA&)noexcept;
public:
    ~OriginalPlayerOverlayRendererLease50LA()noexcept;
    OriginalPlayerOverlayRendererLease50LA(const OriginalPlayerOverlayRendererLease50LA&)=delete;
    OriginalPlayerOverlayRendererLease50LA& operator=(const OriginalPlayerOverlayRendererLease50LA&)=delete;
};
}
