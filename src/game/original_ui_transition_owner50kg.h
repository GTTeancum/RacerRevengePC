#pragma once
#include "original_ui_navigation50kg.h"
#include "original_ui_route_registry50kg.h"
#include <cstdint>

namespace rr::game {
struct OriginalUiTransitionRoute50KG {
    std::uint8_t screen=0,page=0; // Original IDs; page is one-based.
};

// Original manager+0x16C values. State 2 (initial/resumed entry) is outside
// this owner: bind adopts an already-entered native page, never enters it.
enum class OriginalUiTransitionLifecycle50KG : std::uint8_t {
    idle=0,selectRequestedPage=1,waitScreenExit=3,waitPageTransition=4,
    waitCourseExit=5,requestScreenExit=6,requestPageExit=7,
    requestCourseExit=8,queueScreen=9,queueCourse=10
};
// Original manager+0x170, also the current-page virtual+0x7C argument.
enum class OriginalUiTransitionPhase50KG : std::uint8_t { none=0,entry=1,exit=2 };
enum class OriginalUiTransitionAdmission50KG : std::uint8_t { initialPage,screen,page };
enum class OriginalUiTransitionStatus50KG : std::uint8_t {
    ok,noChange,pending,queued,notBound,alreadyBound,busy,invalidRoute,
    unsupportedDestination,missingCallback,invalidDecision,
    selectionNotAccepted,unsupportedLaunch,callbackUnavailable,queueNotAccepted
};
enum class OriginalUiTransitionCompletion50KG : std::uint8_t { pending,complete,unavailable };

// Native callbacks corresponding to the source virtual-call sites. Their
// names/offsets identify evidence, not executable guest vtables. Auxiliary
// child semantics are deliberately not renamed to guessed audio/animation.
enum class OriginalUiTransitionNotificationKind50KG : std::uint8_t {
    previousPage94, // Old page virtual+0x94, in manager 0x2A4020.
    requestedPage90, // New page virtual+0x90, before publishing current page.
    pageEntry74, // New current page virtual+0x74.
    managerEntry84, // Manager virtual+0x84(state=1,1,0,0).
    beginTransition7C, // Current page virtual+0x7C(phase).
    pageAuxiliary1C, // Current page+0x134 child's virtual+0x1C.
    pageAuxiliary24 // Current page+0x134 child's virtual+0x24.
};
struct OriginalUiTransitionNotification50KG {
    OriginalUiTransitionNotificationKind50KG kind=OriginalUiTransitionNotificationKind50KG::beginTransition7C;
    OriginalUiTransitionRoute50KG route{};
    OriginalUiTransitionLifecycle50KG lifecycle=OriginalUiTransitionLifecycle50KG::idle;
    OriginalUiTransitionPhase50KG phase=OriginalUiTransitionPhase50KG::none;
    std::uint32_t sourcePC=0;
};

// Original 0x17DE10 arguments. These are application queue flags, NEVER
// RR_RACE_MODE values. A queue consumer remains a separate native owner.
struct OriginalUiApplicationRequest50KG {
    std::uint32_t flags=0,screen=0,page=0,sourcePC=0;
};
struct OriginalUiTransitionCallbacks50KG {
    // Borrowed native context must outlive this owner and admitted transitions.
    // All callbacks are synchronous, nonthrowing and serialized on its caller.
    void* context=nullptr;

    // Read-only, fail closed. Registry identity alone grants no capability.
    // True certifies the real assets, native page controller and every notified
    // callback needed by that route exist and remain available until settled.
    bool (*admitDestination)(void*,const OriginalUiScreenFactory50KG&,
        const OriginalUiPageFactory50KG&,OriginalUiTransitionAdmission50KG) noexcept=nullptr;
    void (*notify)(void*,const OriginalUiTransitionNotification50KG&) noexcept=nullptr;
    OriginalUiTransitionCompletion50KG (*pollCompletion)(void*,
        OriginalUiTransitionRoute50KG,OriginalUiTransitionPhase50KG) noexcept=nullptr;

    // Read-only admission of the fully committed COURSE configuration and
    // native loading/race ownership. Must independently validate original
    // subtype11258, control-derived track and riders. Not a main-mode cast.
    bool (*admitCourseLaunch)(void*) noexcept=nullptr;

    // Main adapter only: atomically validate/commit the exact original mode34
    // (retaining previous mode at +0x38 when changed) and player bits while
    // preserving other packed bits, to the caller's sole profile owner. False MUST leave
    // that profile unchanged. Called only after full route admission, only
    // for slots 1..3, and before manager request state changes. No save write
    // or gameplay launch. Keeping this absent disables those main requests.
    bool (*commitMainSelection)(void*,const OriginalUiMainDecision50KG&) noexcept=nullptr;

    // Enqueue only. True accepts exactly once; false MUST have no effects and
    // allows a later tick to retry. Defer consumption until after tick returns:
    // this callback must not switch/destroy managers or launch gameplay inline.
    bool (*queueApplication)(void*,const OriginalUiApplicationRequest50KG&) noexcept=nullptr;
};
struct OriginalUiTransitionSnapshot50KG {
    bool bound=false;
    OriginalUiTransitionRoute50KG current{},requested{};
    OriginalUiTransitionLifecycle50KG lifecycle=OriginalUiTransitionLifecycle50KG::idle;
    OriginalUiTransitionPhase50KG phase=OriginalUiTransitionPhase50KG::none;
    // Native once-only handoff marker; source lifecycle remains 9 or 10.
    bool applicationRequestQueued=false;
};

// Allocation-free, one manager lifecycle owner. No page factories, assets,
// timers, transition durations, guest addresses or frontend activation here.
// Copy/move are disabled so callbacks cannot acquire a second manager state.
class OriginalUiTransitionOwner50KG final {
public:
    OriginalUiTransitionOwner50KG() noexcept=default;
    OriginalUiTransitionOwner50KG(const OriginalUiTransitionOwner50KG&)=delete;
    OriginalUiTransitionOwner50KG& operator=(const OriginalUiTransitionOwner50KG&)=delete;
    OriginalUiTransitionOwner50KG(OriginalUiTransitionOwner50KG&&)=delete;
    OriginalUiTransitionOwner50KG& operator=(OriginalUiTransitionOwner50KG&&)=delete;

    // Cold, load-once adoption of an already-entered original bounded page.
    // Failure leaves the owner unbound and invokes no lifecycle notification.
    OriginalUiTransitionStatus50KG bind(std::uint32_t screen,std::uint32_t page,
        const OriginalUiTransitionCallbacks50KG&) noexcept;
    OriginalUiTransitionSnapshot50KG snapshot() const noexcept { return state_; }

    // All requests reject unsupported routes before mutation or notification.
    // One in-flight request only: the stricter native admission also rejects
    // retargeting during entry/pending queue, not just source transition 2.
    // These generic methods own no profile writes. The caller must preserve
    // the appropriate original page-handler transaction around submission.
    OriginalUiTransitionStatus50KG requestScreen(std::uint32_t screen,std::uint32_t page) noexcept;
    OriginalUiTransitionStatus50KG requestPage(std::uint32_t page) noexcept;
    // Only valid while idle on original screen 6/page 1. Does not perform the
    // course profile commit, clear global UI+0x474/+0x478, or run PodLauncher.
    OriginalUiTransitionStatus50KG requestCourseLaunch() noexcept;
    // Validates exact accepted shapes from dispatchOriginalUiMain50KG; no
    // unavailable/popup decision can become a request. Slots 1..3 additionally
    // require atomic commitMainSelection; options/gallery write no profile.
    OriginalUiTransitionStatus50KG requestMain(const OriginalUiMainDecision50KG&) noexcept;

    // Advances at most one source switch case. Only caller completion can
    // open exit/entry gates. pending/unavailable never imply completion.
    // Queue acceptance terminates this owner; further ticks are noChange and
    // further requests are busy. The application consumes its deferred queue.
    // No implicit retry of notifications, rendering, entity update or teardown.
    OriginalUiTransitionStatus50KG tick() noexcept;

private:
    OriginalUiTransitionSnapshot50KG state_{};
    OriginalUiTransitionCallbacks50KG callbacks_{};
    bool inCallback_=false;

    OriginalUiTransitionStatus50KG ready() const noexcept;
    OriginalUiTransitionStatus50KG admit(std::uint32_t screen,std::uint32_t page,
        OriginalUiTransitionAdmission50KG) noexcept;
    void setRequest(std::uint32_t screen,std::uint32_t page,
        OriginalUiTransitionLifecycle50KG) noexcept;
    void notify(OriginalUiTransitionNotificationKind50KG,
        OriginalUiTransitionRoute50KG,std::uint32_t pc) noexcept;
    OriginalUiTransitionCompletion50KG poll() noexcept;
};
} // namespace rr::game
