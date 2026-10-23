#pragma once
#include "original_application_animation_clock50kg.h"
#include "original_ui_rider_render_context50kg.h"
#include "original_ui_rider_visibility50kg.h"
#include "original_ui_lit_geometry50km.h"
#include <memory>

namespace rr::platform {
class OriginalUiGarageSession50KG;
class OriginalUiGarageDrawContribution50KG;
}
namespace rr::game {
class OriginalUiLightManagerLifetime50KG;
// Final authenticated UIGarage/garage.scn Ambient,Directional population,
// after Scene272A48 resets the earlier UI lights. No Rider color rewrite.
struct OriginalUiGarageLighting50KG {
    const assets::OriginalUiMemberIdentity50KG* uiIdentity=nullptr;
    const assets::OriginalUiMemberIdentity50KG* sceneIdentity=nullptr;
    std::array<float,4> ambient50{},ambientSlot60{},directional3C{};
    std::array<float,3> directionalLook78{};
};
enum class OriginalUiGarageCameraError50KG : std::uint8_t {
    none,alreadyBound,archive,page,fog,lights,arithmetic,clock,phase,allocation
};
struct OriginalUiGarageCameraDiagnostic50KG {
    OriginalUiGarageCameraError50KG error=OriginalUiGarageCameraError50KG::none;
    assets::OriginalUiPageDiagnostic50KG parse{};
    std::uint32_t sourcePC=0;
};
class OriginalUiGarageCameraOwner50KG;
// One immutable-to-consumers issuance. It is NOT a freely constructible camera
// capability. Consumers retain the owner, check isCurrent(), and finish their
// actual camera borrows before the next issuance. Fully copied geometry does
// not borrow this storage; retiring it is NOT renderer/source-lease completion.
class OriginalUiGarageCameraFrame50KG final {
public:
    const OriginalUiRiderRenderContext50KG& renderContext() const noexcept {return render_;}
    const OriginalUiRiderCamera50KG& cameraAfterFog() const noexcept {return render_.cameraAfterFog;}
    const OriginalUiRiderClipCamera50KG& clip() const noexcept {return clip_;}
    const OriginalUiRiderMembershipCamera50KG& membership() const noexcept {return membership_;}
    const OriginalUiGarageLighting50KG& lighting() const noexcept {return lighting_;}
    const OriginalUiLitContext50KM& context() const noexcept {return context_;}
    std::uint64_t serial() const noexcept {return serial_;}
    std::uint32_t applicationCurrent230() const noexcept {return current230_;}
private:
    friend class OriginalUiGarageCameraOwner50KG;
    OriginalUiGarageCameraFrame50KG() noexcept=default;
    OriginalUiGarageCameraFrame50KG(const OriginalUiGarageCameraFrame50KG&)=default;
    OriginalUiGarageCameraFrame50KG& operator=(const OriginalUiGarageCameraFrame50KG&)=default;
    OriginalUiRiderRenderContext50KG render_{};
    OriginalUiRiderClipCamera50KG clip_{};
    OriginalUiRiderMembershipCamera50KG membership_{};
    OriginalUiGarageLighting50KG lighting_{};
    OriginalUiLitContext50KM context_{};
    std::uint64_t serial_=0;
    std::uint32_t current230_=0,pause11C_=0,pending120_=0;
};
// Genuine Garage camera and final SCN fog/light arithmetic. The UI light
// population is reset by Scene272A48->148B60 before SCN registration273AA0.
// Source1FB310 supplies max4000/min4000/3 and gap64, not the UI fog bounds.
// Cold bind issues no drawable frame. Session must retain the real shared
// light manager, application clock, observer and prelude/render lifecycle.
// Current frames are private-issued and borrowed until actual retirement.
class OriginalUiGarageCameraOwner50KG final {
public:
    OriginalUiGarageCameraOwner50KG() noexcept=default;
    OriginalUiGarageCameraOwner50KG(const OriginalUiGarageCameraOwner50KG&)=delete;
    OriginalUiGarageCameraOwner50KG& operator=(const OriginalUiGarageCameraOwner50KG&)=delete;
    bool bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        OriginalUiGarageCameraDiagnostic50KG&) noexcept;
    const assets::OriginalUiArchive50KG* archive() const noexcept {return archive_.get();}
    // Authored, unmodified current population for the real shared light manager
    // to install at manager activation. This getter alone installs no lights.
    const OriginalUiGarageLighting50KG* authoredLighting() const noexcept {
        return archive_?&lighting_:nullptr;
    }
    const OriginalUiGarageCameraFrame50KG* frame() const noexcept;
    bool isCurrent(const OriginalUiGarageCameraFrame50KG&) const noexcept;
private:
    friend class rr::platform::OriginalUiGarageSession50KG;
    friend class rr::platform::OriginalUiGarageDrawContribution50KG;
    // Never accepts a caller readiness flag, arbitrary matrix, serial or clock
    // scalar. Session supplies its SAME live Application clock and shared light
    // manager. Current registry rows and manager generation are checked, not
    // inferred from authored colors or Session readiness.
    bool prepareFrame(const OriginalApplicationAnimationClockView50KG&,
        std::shared_ptr<const OriginalUiLightManagerLifetime50KG>,
        OriginalUiGarageCameraDiagnostic50KG&) noexcept;
    bool retireFrame(const OriginalUiGarageCameraFrame50KG&) noexcept;
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive_;
    OriginalUiRiderCamera50KG authored_{};
    OriginalUiRiderRenderContext50KG fog_{};
    OriginalUiGarageLighting50KG lighting_{};
    OriginalApplicationAnimationClockView50KG clock_{};
    std::shared_ptr<const OriginalUiLightManagerLifetime50KG> lightManager_;
    OriginalUiGarageCameraFrame50KG frame_{};
    std::uint64_t serial_=0,lightGeneration_=0;
    bool issued_=false;
};
}
