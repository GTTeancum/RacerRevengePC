// New reconstruction after rollback; source-only, unverified.
#pragma once
#include "original_ui_transition_owner50kg.h"
#include "../assets/original_ui_pages50kg.h"
#include <array>
#include <cstdint>
#include <memory>

namespace rr::game {
// Source page+0x4B4, not the manager's lifecycle or transition phase.
enum class OriginalUiApertureState50KG : std::uint8_t {
    open=0,opening=1,closed=2,closing=3,unbound=255
};
enum class OriginalUiApertureStatus50KG : std::uint8_t {
    ok,noChange,notBound,alreadyBound,busy,invalidArchive,parseFailure,
    missingPage,invalidConfig,missingDependency,unavailable,invalidPhase,
    missingUpdateDelta,invalidDelta,unsupportedArithmetic,allocation
};
struct OriginalUiApertureDiagnostic50KG {
    OriginalUiApertureStatus50KG status=OriginalUiApertureStatus50KG::ok;
    assets::OriginalUiPageDiagnostic50KG parse{};
    std::uint32_t sourcePC=0;
};
struct OriginalUiApertureConfig50KG {
    // A default-constructed descriptor is NOT an admitted source configuration.
    // bind reads authenticated UIBase/Page 1/PageFile, then its PageInfo.
    const assets::OriginalUiMemberIdentity50KG* indexIdentity=nullptr;
    const assets::OriginalUiMemberIdentity50KG* pageIdentity=nullptr;
    std::int32_t openSize274=0;
    float time4B0=0;
    bool startClosed=false;
    // Missing or empty unquoted Key= uses the original parser default. An
    // authored value equal to that default still has its authored flag set.
    bool openSizeAuthored=false,timeAuthored=false,startAuthored=false;
};
struct OriginalUiApertureSnapshot50KG {
    bool bound=false,hasUpdateDelta=false,geometryPrepared=false;
    OriginalUiApertureState50KG state=OriginalUiApertureState50KG::unbound;
    std::int32_t top284=0,bottom28C=0;
    float speed4B8=0,savedDelta298=0;
    bool inputEnabled268=false;
};
struct OriginalUiApertureVertex50KG { float x=0,y=0; };
struct OriginalUiApertureRectangle50KG {
    // Original primitive vertex order: TL, BL, TR, BR. Do not send directly
    // to the native renderer's TL,TR,BR,BL API; its permutation is 0,2,3,1.
    std::array<OriginalUiApertureVertex50KG,4> vertices{};
    std::int32_t left=0,top=0,right=0,bottom=0;
    std::uint32_t sourceObjectOffset=0;
    bool dirty3C=false;
};
struct OriginalUiApertureGeometry50KG {
    // Source 1D7980..1D79A0 submission order: +0x424, then +0x428.
    std::array<OriginalUiApertureRectangle50KG,2> rectangles{};
    // 1D66F4 literal and 1D6754/58 per-vertex alpha. Full material/depth,
    // RGB packet conversion, pass routing and GPU submission stay external.
    const char* sourceMaterial="NoTexture";
    std::uint8_t sourceVertexAlpha=63;
};
enum class OriginalUiApertureHookKind50KG : std::uint8_t {
    openingAC,openedB0,closingB4,closingThresholdB8,closedBC,
    screenExitObject440 // Source 2E0170 call before the closed clamp in life3.
};
struct OriginalUiApertureHook50KG {
    OriginalUiApertureHookKind50KG kind=OriginalUiApertureHookKind50KG::openingAC;
    std::uint32_t callPC=0,targetPC=0;
    OriginalUiApertureSnapshot50KG observed{};
};
// Source CVT.W.S at 1D75E8/1D76B8 is signed truncation toward zero, proved
// by Sony's EE instruction manual and hardwired FCR31 rounding bits. Finite
// binary32 values in [-2147483648,2147483648) are admitted. Out-of-range source
// saturation is deliberately outside this helper; failure preserves output.
// This conversion is independent of host rounding mode and does not prove
// that upstream host multiply/divide is universally bit-identical to EE.
bool convertOriginalUiApertureCvtWS50KG(float,std::int32_t&) noexcept;
struct OriginalUiApertureCallbacks50KG {
    // Borrowed native owners must outlive this object. All calls are serial,
    // synchronous and noexcept. Callbacks must not mutate/reenter the owner.
    void* context=nullptr;
    // Certifies real Main hook side-effect owners, the +0x440 native object,
    // both original primitives and their real render-pass submission owner.
    // A trace-only sink or asset-presence check is insufficient certification.
    // This does NOT certify the rest of Main's page/input/manager capabilities.
    bool (*available)(void*) noexcept=nullptr;
    // Synchronous native implementation of the exact hook in the event. PCs
    // are provenance only and are NEVER interpreted as executable addresses.
    void (*hook)(void*,const OriginalUiApertureHook50KG&) noexcept=nullptr;
    // Source primitive preparation/submission is infallible after available.
    // ordinal 0 is submitted before ordinal 1; no sorting or batching change.
    // Borrowed rectangle is valid only for the duration of this callback.
    void (*submitOverlay)(void*,std::uint8_t ordinal,
        const OriginalUiApertureRectangle50KG&) noexcept=nullptr;

    // Numeric policy is implemented here, not certified by a caller callback:
    // finite/in-range host binary32 operations under FE_TONEAREST, followed
    // by verified signed truncation. No universal bit-identical EE claim.
};

// Staged bounded Main/shared aperture owner. It is NOT a full Main page,
// route-admission callback, event-handler tree, renderer or frontend switch.
// Rider's panel/preview and Course's entry gate are intentionally excluded.
class OriginalUiApertureOwner50KG final {
public:
    OriginalUiApertureOwner50KG() noexcept=default;
    OriginalUiApertureOwner50KG(const OriginalUiApertureOwner50KG&)=delete;
    OriginalUiApertureOwner50KG& operator=(const OriginalUiApertureOwner50KG&)=delete;
    OriginalUiApertureOwner50KG(OriginalUiApertureOwner50KG&&)=delete;
    OriginalUiApertureOwner50KG& operator=(OriginalUiApertureOwner50KG&&)=delete;

    // Cold all-or-nothing bind. Archive must genuinely own authenticated
    // UIMAIN bytes; no fabricated descriptor or default-only binding exists.
    // Uses the exact Page 1 PageFile, never filename-based route inference.
    // No asset decode, file IO, widget creation or hooks occur here. A failed
    // bind is retryable. Parsed config/default provenance remains inspectable.
    OriginalUiApertureStatus50KG bindMain(
        std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const OriginalUiApertureCallbacks50KG&,OriginalUiApertureDiagnostic50KG&) noexcept;
    // Explicit bounded opening-only admission. available certifies AC/B0 and
    // both primitives, not the source +440 exit ScreenText. Every exit request
    // and exit completion query is unavailable before any mutation. Existing
    // bindMain retains its stricter full-hook contract unchanged.
    OriginalUiApertureStatus50KG bindMainEntryOnly(
        std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const OriginalUiApertureCallbacks50KG&,OriginalUiApertureDiagnostic50KG&) noexcept;
    // Same shared numeric1D7550 owner, separately authenticated Options page1.
    // Hooks target actual1F9D20/1F9E20/1F9ED0/1F9FE0, never Main callbacks.
    // Full bind requires real +440 exit-object effects; entry-only rejects exit.
    OriginalUiApertureStatus50KG bindOptionsMenu(
        std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const OriginalUiApertureCallbacks50KG&,OriginalUiApertureDiagnostic50KG&) noexcept;
    // Actual page4/options.dat, vtable3D0D50; no alias of menu.dat state.
    OriginalUiApertureStatus50KG bindGameOptions(
        std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const OriginalUiApertureCallbacks50KG&,OriginalUiApertureDiagnostic50KG&) noexcept;
    // Actual config.dat shared by factory pages2/3, with distinct lifetimes.
    OriginalUiApertureStatus50KG bindControls(
        std::shared_ptr<const assets::OriginalUiArchive50KG>,std::uint8_t factoryPage,
        const OriginalUiApertureCallbacks50KG&,OriginalUiApertureDiagnostic50KG&) noexcept;
    OriginalUiApertureStatus50KG bindOptionsMenuEntryOnly(
        std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const OriginalUiApertureCallbacks50KG&,OriginalUiApertureDiagnostic50KG&) noexcept;
    // Authenticated Rider factory page1/page2, both owning the original rider.dat.
    // This binds only shared aperture arithmetic. The real Rider controller
    // defers EXIT beginTransition7C until panel140/readiness1ECFE0 permits it.
    // available must own actual Rider AC/B0/B4/B8 and shared +440 effects.
    OriginalUiApertureStatus50KG bindRider(
        std::shared_ptr<const assets::OriginalUiArchive50KG>,std::uint8_t factoryPage,
        const OriginalUiApertureCallbacks50KG&,OriginalUiApertureDiagnostic50KG&) noexcept;
    OriginalUiApertureStatus50KG saveRiderUpdateDelta38(float actualDelta) noexcept;
    // Genuine Course6/1 owner. Entry opens only at normal render1DEB20.
    OriginalUiApertureStatus50KG bindCourse(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const OriginalUiApertureCallbacks50KG&,OriginalUiApertureDiagnostic50KG&)noexcept;
    OriginalUiApertureStatus50KG bindCourseEntryOnly(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const OriginalUiApertureCallbacks50KG&,OriginalUiApertureDiagnostic50KG&)noexcept;
    OriginalUiApertureStatus50KG saveCourseUpdateDelta38(float actualDelta)noexcept;
    OriginalUiApertureStatus50KG beginCourseEntryAtRender48(OriginalUiTransitionPhase50KG actualManagerPhase170)noexcept;
    // Tournament1F2E30 entry is a no-op; render1F2DF8 begins opening.
    // This is the aperture owner, not a completed Tournament screen.
    OriginalUiApertureStatus50KG bindTournament(
        std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const OriginalUiApertureCallbacks50KG&,OriginalUiApertureDiagnostic50KG&) noexcept;
    OriginalUiApertureStatus50KG saveTournamentUpdateDelta38(float actualDelta) noexcept;
    // Call at entry74 reset point1EEAB8/1EEAD0 after actual popup setup.
    // Resets only4060; the session owns timer4064 and remaining entry effects.
    OriginalUiApertureStatus50KG resetTournamentExitRequestedAtEntry74() noexcept;
    // Render1F2DB8 also follows active popup when actual4060 is nonzero.
    OriginalUiApertureStatus50KG beginTournamentEntryAtRender48(
        OriginalUiTransitionPhase50KG actualManagerPhase170,std::uint8_t actualPopupFlags29) noexcept;
    bool tournamentExitRequested4060() const noexcept {return tournament_&&tournamentExitRequested4060_;}
    OriginalUiApertureSnapshot50KG snapshot() const noexcept { return state_; }
    const OriginalUiApertureConfig50KG* config() const noexcept {
        return state_.bound?&config_:nullptr;
    }
    const OriginalUiApertureGeometry50KG* geometry() const noexcept {
        return state_.geometryPrepared?&geometry_:nullptr;
    }

    // Main's shared +0x7C begin. Entry does not clear input268; exit clears it
    // before +0xB4. Repeating the same target is source-idempotent. Reverse
    // begins retain live edges rather than resetting either endpoint.
    OriginalUiApertureStatus50KG beginTransition7C(OriginalUiTransitionPhase50KG) noexcept;
    // Call ONLY at the real Main entity-update tail (1E63F8), before its child
    // update 2A6BB0. It saves actual dt and never changes aperture geometry.
    // This is not the entire Main update and does not traverse children.
    OriginalUiApertureStatus50KG saveMainUpdateDelta38(float actualDelta) noexcept;
    // Options actual+38=1F9A70 saves dt at1F9A74 before jumping to2A6BB0.
    OriginalUiApertureStatus50KG saveOptionsMenuUpdateDelta38(float actualDelta) noexcept;
    OriginalUiApertureStatus50KG saveGameOptionsUpdateDelta38(float actualDelta) noexcept;
    OriginalUiApertureStatus50KG saveControlsUpdateDelta38(float actualDelta) noexcept;
    // Call ONLY at the real Main render's 1E6688 -> 1D7550 point. Uses the
    // saved delta, never a new elapsed timer. Every real render may use that
    // same saved sample, as source does; no synthetic per-tick deduplication.
    // Endpoint checks precede steps, so crossing stays opening/closing until
    // a subsequent real render observes it. Close crossing invokes B8 first.
    // No implicit entity update or manager tick; no GPU-fence interpretation.
    OriginalUiApertureStatus50KG render48(OriginalUiTransitionLifecycle50KG) noexcept;
    // Source phase predicate; Tournament also records pending exit4060.
    // Wrong route or lost dependency is
    // unavailable. Unknown/none phase is pending. Repeated polls never move.
    OriginalUiTransitionCompletion50KG pollCompletion(
        OriginalUiTransitionRoute50KG,OriginalUiTransitionPhase50KG) const noexcept;

private:
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive_;
    OriginalUiApertureConfig50KG config_{};
    OriginalUiApertureSnapshot50KG state_{};
    OriginalUiApertureGeometry50KG geometry_{};
    OriginalUiApertureCallbacks50KG callbacks_{};
    mutable bool inOperation_=false;
    bool entryOnly_=false;
    bool optionsMenu_=false,rider_=false,course_=false,tournament_=false;
    mutable bool tournamentExitRequested4060_=false;
    std::uint8_t optionsPage_=1;
    OriginalUiApertureStatus50KG bindPageImpl(
        std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const OriginalUiApertureCallbacks50KG&,bool entryOnly,bool optionsMenu,std::uint8_t page,
        OriginalUiApertureDiagnostic50KG&,bool rider=false,bool course=false,bool tournament=false) noexcept;
    OriginalUiApertureStatus50KG saveUpdateDelta38(float actualDelta) noexcept;
    bool available() const noexcept;
    void emit(OriginalUiApertureHookKind50KG,std::uint32_t callPC,
        std::uint32_t targetPC) noexcept;
};
} // namespace rr::game

