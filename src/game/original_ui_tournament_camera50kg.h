#pragma once
#include "original_application_animation_clock50kg.h"
#include "original_ui_rider_render_context50kg.h"
#include "original_ui_rider_visibility50kg.h"
#include "original_ui_lit_geometry50km.h"
#include <memory>

namespace rr::platform {
class OriginalUiTournamentSession50KG;
class OriginalUiTournamentDrawContribution50KG;
}
namespace rr::game {
class OriginalUiLightManagerLifetime50KG;
// Actual UITourn/ui.dat population, in Ambient,Directional registration order.
// Tournament has no Rider capture/locked-color rewrite or ARGB truncation.
struct OriginalUiTournamentLighting50KG {
    const assets::OriginalUiMemberIdentity50KG* uiIdentity=nullptr;
    std::array<float,4> ambient50{},ambientSlot60{},directional3C{};
    std::array<float,3> directionalLook78{};
};
enum class OriginalUiTournamentCameraError50KG : std::uint8_t {
    none,alreadyBound,archive,page,fog,lights,arithmetic,clock,phase,allocation
};
struct OriginalUiTournamentCameraDiagnostic50KG {
    OriginalUiTournamentCameraError50KG error=OriginalUiTournamentCameraError50KG::none;
    assets::OriginalUiPageDiagnostic50KG parse{};
    std::uint32_t sourcePC=0;
};
class OriginalUiTournamentCameraOwner50KG;
// One immutable-to-consumers issuance. It is NOT a freely constructible camera
// capability. Consumers retain the owner, check isCurrent(), and finish their
// actual camera borrows before the next issuance. Fully copied geometry does
// not borrow this storage; retiring it is NOT renderer/source-lease completion.
class OriginalUiTournamentCameraFrame50KG final {
public:
    const OriginalUiRiderRenderContext50KG& renderContext() const noexcept {return render_;}
    const OriginalUiRiderCamera50KG& cameraAfterFog() const noexcept {return render_.cameraAfterFog;}
    const OriginalUiRiderClipCamera50KG& clip() const noexcept {return clip_;}
    const OriginalUiRiderMembershipCamera50KG& membership() const noexcept {return membership_;}
    const OriginalUiTournamentLighting50KG& lighting() const noexcept {return lighting_;}
    const OriginalUiLitContext50KM& context() const noexcept {return context_;}
    std::uint64_t serial() const noexcept {return serial_;}
    std::uint32_t applicationCurrent230() const noexcept {return current230_;}
private:
    friend class OriginalUiTournamentCameraOwner50KG;
    OriginalUiTournamentCameraFrame50KG() noexcept=default;
    OriginalUiTournamentCameraFrame50KG(const OriginalUiTournamentCameraFrame50KG&)=default;
    OriginalUiTournamentCameraFrame50KG& operator=(const OriginalUiTournamentCameraFrame50KG&)=default;
    OriginalUiRiderRenderContext50KG render_{};
    OriginalUiRiderClipCamera50KG clip_{};
    OriginalUiRiderMembershipCamera50KG membership_{};
    OriginalUiTournamentLighting50KG lighting_{};
    OriginalUiLitContext50KM context_{};
    std::uint64_t serial_=0;
    std::uint32_t current230_=0,pause11C_=0,pending120_=0;
};
// Scoped native semantic owner, NOT a claim to execute2361C0 or preserve its
// early uninitialized bounds. The reached normal authored pose is refreshed
// before fog13E9C0/13EA30 rebuilds near/far and237000 rewrites ALL of290.
// Therefore no early unknown290 word reaches this owner's selection/draw.
// Numeric Camera/RenderContext types have historical Rider names only; their
// identities remain UITourn/standing.dat and UITourn/ui.dat. No Rider loader,
// Rider live registration, identity relaxation or fabricated raw camera is used.
//
// Session retains the actual Application and selected camera, normal debug
// gates, live manager lights and phase order. Its single-active-Session native
// dispatch replaces guest observer-array mechanics, not their ownership effects.
// CPU preparation touches no GL state. The native effects of pre-child244D60(0)
// are supplied by sealed source submission: real target/state setup, ordered
// draw and restoration on the retained renderer context. No guest prelude
// receipt or readiness boolean is issued here. Private prepareFrame is legal
// only during admitted Session frame preparation after the normal source update.
// The collector must keep it alive and call retireFrame ONLY after all camera,
// selected-model and binder borrows have actually retired. Its CPU drain may
// do so after complete geometry/context copies and pod endFrame; real GPU,
// source-renderer, material and image leases remain independently outstanding.
// A failed begin with no camera borrowers may return the issuance directly.
// Never use camera retirement to fake renderer completion or reset a failure.
class OriginalUiTournamentCameraOwner50KG final {
public:
    OriginalUiTournamentCameraOwner50KG() noexcept=default;
    OriginalUiTournamentCameraOwner50KG(const OriginalUiTournamentCameraOwner50KG&)=delete;
    OriginalUiTournamentCameraOwner50KG& operator=(const OriginalUiTournamentCameraOwner50KG&)=delete;
    bool bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        OriginalUiTournamentCameraDiagnostic50KG&) noexcept;
    const assets::OriginalUiArchive50KG* archive() const noexcept {return archive_.get();}
    // Authored, unmodified current population for the real shared light manager
    // to install at manager activation. This getter alone installs no lights.
    const OriginalUiTournamentLighting50KG* authoredLighting() const noexcept {
        return archive_?&lighting_:nullptr;
    }
    const OriginalUiTournamentCameraFrame50KG* frame() const noexcept;
    bool isCurrent(const OriginalUiTournamentCameraFrame50KG&) const noexcept;
private:
    friend class rr::platform::OriginalUiTournamentSession50KG;
    friend class rr::platform::OriginalUiTournamentDrawContribution50KG;
    // Never accepts a caller readiness flag, arbitrary matrix, serial or clock
    // scalar. Session supplies its SAME live Application clock and shared light
    // manager. Current registry rows and manager generation are checked, not
    // inferred from authored colors or Session readiness.
    bool prepareFrame(const OriginalApplicationAnimationClockView50KG&,
        std::shared_ptr<const OriginalUiLightManagerLifetime50KG>,
        OriginalUiTournamentCameraDiagnostic50KG&) noexcept;
    bool retireFrame(const OriginalUiTournamentCameraFrame50KG&) noexcept;
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive_;
    OriginalUiRiderCamera50KG authored_{};
    OriginalUiRiderRenderContext50KG fog_{};
    OriginalUiTournamentLighting50KG lighting_{};
    OriginalApplicationAnimationClockView50KG clock_{};
    std::shared_ptr<const OriginalUiLightManagerLifetime50KG> lightManager_;
    OriginalUiTournamentCameraFrame50KG frame_{};
    std::uint64_t serial_=0,lightGeneration_=0;
    bool issued_=false;
};
}
