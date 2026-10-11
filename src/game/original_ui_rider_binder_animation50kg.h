#pragma once
#include "../assets/original_ui_rider_binder_assets50kg.h"
#include "original_ui_texture_animation50kg.h"
#include "original_ui_rider_material_lifetime50kg.h"
#include <memory>

namespace rr::game {
struct OriginalUiRiderBinderFrame50KG {
    // Exact Diffuse pass and selected original frame; no native/GPU admission.
    const assets::OriginalUiMaterialPass50KG* pass=nullptr;
    const assets::OriginalUiScreenAssets50KG* images=nullptr;
    const assets::Texture48E* decoded=nullptr;
    std::uint32_t imageIndex=0;
    std::size_t originalPstOffset=0,originalPstBytes=0;
};
// One noncopyable instance per created original binderleftN/rightN helper.
// Cold assets may be shared, "unique" temporal state MUST NOT be shared.
class OriginalUiRiderBinderAnimation50KG final {
public:
    OriginalUiRiderBinderAnimation50KG() noexcept;
    ~OriginalUiRiderBinderAnimation50KG();
    OriginalUiRiderBinderAnimation50KG(const OriginalUiRiderBinderAnimation50KG&)=delete;
    OriginalUiRiderBinderAnimation50KG& operator=(const OriginalUiRiderBinderAnimation50KG&)=delete;
    bool initialize(std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG>,
        OriginalUiRiderMaterialLifetime50KG&,const OriginalUiRiderMaterialFootprint50KG&,
        OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    // Owner verifies actual completion/detach entitlement before this call.
    // Failure preserves the exact helper and token for explicit cleanup.
    bool destroy(OriginalUiRiderMaterialLifetimeDiagnostic50KG&) noexcept;
    // Call exactly at1D4240 after original geometry preparation, only when the
    // original material handle is valid. Clock is source application millis;
    // no independent wall clock, delta-time counter, render-time or audio time.
    // Duplicate timestamp and initial sentinel handled by shared original leaf.
    bool advanceAfterGeometry(std::uint32_t sourceMilliseconds,OriginalUiRiderBinderFrame50KG&) noexcept;
    bool current(OriginalUiRiderBinderFrame50KG&)const noexcept;
    bool initialized()const noexcept;
private:
    bool bind(std::uint32_t,OriginalUiRiderBinderFrame50KG&)const noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
