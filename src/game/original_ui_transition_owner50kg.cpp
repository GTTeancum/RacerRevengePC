#include "original_ui_transition_owner50kg.h"

namespace rr::game {
namespace {
using Status=OriginalUiTransitionStatus50KG;
using Life=OriginalUiTransitionLifecycle50KG;
using Phase=OriginalUiTransitionPhase50KG;
using Admission=OriginalUiTransitionAdmission50KG;
using Completion=OriginalUiTransitionCompletion50KG;
using Notice=OriginalUiTransitionNotificationKind50KG;

// Reentrant mutation from any callback fails closed. Read-only snapshots are
// allowed and reflect the original order of state writes and notifications.
struct CallbackScope {
    bool& active;
    explicit CallbackScope(bool& flag) noexcept:active(flag) { active=true; }
    ~CallbackScope() { active=false; }
};
Status identity(std::uint32_t screen,std::uint32_t page,
    const OriginalUiScreenFactory50KG*& screenFactory,
    const OriginalUiPageFactory50KG*& pageFactory) noexcept {
    screenFactory=findOriginalUiScreenFactory50KG(screen);
    if (!screenFactory || !page || page>255) return Status::invalidRoute;
    pageFactory=findOriginalUiBoundedPageFactory50KG(screen,page);
    return pageFactory?Status::ok:Status::unsupportedDestination;
}
bool mainShape(const OriginalUiMainDecision50KG& d) noexcept {
    using Action=OriginalUiMainAction50KG;
    if (d.selectedSlot>=1 && d.selectedSlot<=3)
        return d.action==Action::changeScreen && d.targetScreen==5 && d.routeArgument==1 &&
            d.originalGameMode==static_cast<std::int8_t>(d.selectedSlot-1) &&
            d.originalPlayerBits==(d.selectedSlot==2?0x40:0x20);
    if (d.selectedSlot==4)
        return d.action==Action::changeScreen && d.targetScreen==12 && d.routeArgument==1 &&
            d.originalGameMode==-1 && d.originalPlayerBits==0;
    if (d.selectedSlot==5)
        return d.action==Action::changePage && d.targetScreen==-1 && d.routeArgument==5 &&
            d.originalGameMode==-1 && d.originalPlayerBits==0;
    return false;
}
}

Status OriginalUiTransitionOwner50KG::bind(std::uint32_t screen,std::uint32_t page,
    const OriginalUiTransitionCallbacks50KG& callbacks) noexcept {
    if (inCallback_) return Status::busy;
    if (state_.bound) return Status::alreadyBound;
    const auto admittedCallbacks=callbacks;
    const OriginalUiScreenFactory50KG* screenFactory=nullptr;
    const OriginalUiPageFactory50KG* pageFactory=nullptr;
    const auto status=identity(screen,page,screenFactory,pageFactory);
    if (status!=Status::ok) return status;
    if (!admittedCallbacks.admitDestination || !admittedCallbacks.notify || !admittedCallbacks.pollCompletion)
        return Status::missingCallback;
    {
        CallbackScope guard(inCallback_);
        if (!admittedCallbacks.admitDestination(admittedCallbacks.context,
            *screenFactory,*pageFactory,Admission::initialPage))
            return Status::unsupportedDestination;
    }
    OriginalUiTransitionSnapshot50KG next;
    next.bound=true;
    next.current={static_cast<std::uint8_t>(screen),static_cast<std::uint8_t>(page)};
    next.requested=next.current;
    callbacks_=admittedCallbacks;state_=next;
    return Status::ok;
}

Status OriginalUiTransitionOwner50KG::ready() const noexcept {
    if (inCallback_) return Status::busy;
    if (!state_.bound) return Status::notBound;
    if (state_.lifecycle!=Life::idle || state_.phase!=Phase::none || state_.applicationRequestQueued)
        return Status::busy;
    return Status::ok;
}
Status OriginalUiTransitionOwner50KG::admit(std::uint32_t screen,std::uint32_t page,
    Admission kind) noexcept {
    const OriginalUiScreenFactory50KG* screenFactory=nullptr;
    const OriginalUiPageFactory50KG* pageFactory=nullptr;
    const auto status=identity(screen,page,screenFactory,pageFactory);
    if (status!=Status::ok) return status;
    if (kind==Admission::screen && !callbacks_.queueApplication) return Status::missingCallback;
    CallbackScope guard(inCallback_);
    return callbacks_.admitDestination(callbacks_.context,*screenFactory,*pageFactory,kind)?
        Status::ok:Status::unsupportedDestination;
}
void OriginalUiTransitionOwner50KG::setRequest(std::uint32_t screen,std::uint32_t page,
    Life lifecycle) noexcept {
    // Request helpers write page, then screen, then lifecycle; no exit begins
    // until the next explicit tick. Admission has already validated both IDs.
    state_.requested.page=static_cast<std::uint8_t>(page);
    state_.requested.screen=static_cast<std::uint8_t>(screen);
    state_.lifecycle=lifecycle;
}
Status OriginalUiTransitionOwner50KG::requestScreen(std::uint32_t screen,std::uint32_t page) noexcept {
    auto status=ready();if (status!=Status::ok) return status;
    status=admit(screen,page,Admission::screen);if (status!=Status::ok) return status;
    setRequest(screen,page,Life::requestScreenExit);return Status::ok;
}
Status OriginalUiTransitionOwner50KG::requestPage(std::uint32_t page) noexcept {
    auto status=ready();if (status!=Status::ok) return status;
    status=admit(state_.current.screen,page,Admission::page);if (status!=Status::ok) return status;
    setRequest(state_.current.screen,page,Life::requestPageExit);return Status::ok;
}
Status OriginalUiTransitionOwner50KG::requestCourseLaunch() noexcept {
    const auto status=ready();if (status!=Status::ok) return status;
    if (state_.current.screen!=6 || state_.current.page!=1) return Status::unsupportedLaunch;
    if (!callbacks_.admitCourseLaunch || !callbacks_.queueApplication) return Status::missingCallback;
    {
        CallbackScope guard(inCallback_);
        if (!callbacks_.admitCourseLaunch(callbacks_.context)) return Status::unsupportedLaunch;
    }
    // Course 0x1DED58 writes state 8, with no replacement screen/page request.
    state_.lifecycle=Life::requestCourseExit;return Status::ok;
}
Status OriginalUiTransitionOwner50KG::requestMain(const OriginalUiMainDecision50KG& decision) noexcept {
    auto status=ready();if (status!=Status::ok) return status;
    if (state_.current.screen!=4 || state_.current.page!=1 || !mainShape(decision))
        return Status::invalidDecision;
    const bool sameScreen=decision.action==OriginalUiMainAction50KG::changePage;
    const auto screen=sameScreen?std::uint32_t(state_.current.screen):
        static_cast<std::uint32_t>(decision.targetScreen);
    const auto page=static_cast<std::uint32_t>(decision.routeArgument);
    if (decision.selectedSlot<=3 && !callbacks_.commitMainSelection) return Status::missingCallback;
    status=admit(screen,page,sameScreen?Admission::page:Admission::screen);
    if (status!=Status::ok) return status;
    if (decision.selectedSlot<=3) {
        CallbackScope guard(inCallback_);
        if (!callbacks_.commitMainSelection(callbacks_.context,decision)) return Status::selectionNotAccepted;
    }
    setRequest(screen,page,sameScreen?Life::requestPageExit:Life::requestScreenExit);
    return Status::ok;
}

Status OriginalUiTransitionOwner50KG::requestRouteDecision(const OriginalUiRouteDecision50KG& suppliedDecision) noexcept {
    auto status=ready();if (status!=Status::ok) return status;
    // Callbacks may release/reuse their caller-owned pending decision. Retain
    // one complete bounded snapshot through admission, commit and publication.
    const OriginalUiRouteDecision50KG decision=suppliedDecision;
    using Kind=OriginalUiRouteKind50KG;
    const auto& request=decision.request;
    if (!decision.admitted || !decision.profileProposalComplete || decision.blockers ||
        decision.action!=OriginalUiRouteAction50KG::request ||
        decision.profileChanges.count>decision.profileChanges.values.size() ||
        request.sourceScreen!=state_.current.screen || request.sourcePage!=state_.current.page)
        return Status::invalidDecision;
    if (request.kind==Kind::courseLaunch) return Status::unsupportedLaunch;
    if (request.kind!=Kind::changeScreen && request.kind!=Kind::changePage)
        return Status::invalidDecision;
    const bool sameScreen=request.kind==Kind::changePage;
    if (sameScreen && request.targetScreen!=state_.current.screen) return Status::invalidDecision;
    if (!callbacks_.commitRouteSelection) return Status::missingCallback;
    status=admit(request.targetScreen,request.targetPage,sameScreen?Admission::page:Admission::screen);
    if (status!=Status::ok) return status;
    {
        CallbackScope guard(inCallback_);
        if (!callbacks_.commitRouteSelection(callbacks_.context,decision))
            return Status::selectionNotAccepted;
    }
    setRequest(request.targetScreen,request.targetPage,
        sameScreen?Life::requestPageExit:Life::requestScreenExit);
    return Status::ok;
}

void OriginalUiTransitionOwner50KG::notify(Notice kind,OriginalUiTransitionRoute50KG route,
    std::uint32_t pc) noexcept {
    const OriginalUiTransitionNotification50KG event{kind,route,state_.lifecycle,state_.phase,pc};
    CallbackScope guard(inCallback_);callbacks_.notify(callbacks_.context,event);
}
Completion OriginalUiTransitionOwner50KG::poll() noexcept {
    CallbackScope guard(inCallback_);
    return callbacks_.pollCompletion(callbacks_.context,state_.current,state_.phase);
}
Status OriginalUiTransitionOwner50KG::tick() noexcept {
    if (inCallback_) return Status::busy;
    if (!state_.bound) return Status::notBound;
    if (state_.applicationRequestQueued) return Status::noChange;
    switch (state_.lifecycle) {
    case Life::idle: return Status::noChange;
    case Life::requestScreenExit:
        state_.lifecycle=Life::waitScreenExit;state_.phase=Phase::exit;
        notify(Notice::beginTransition7C,state_.current,0x2A3DFC);
        return Status::ok;
    case Life::requestPageExit:
        state_.lifecycle=Life::waitPageTransition;state_.phase=Phase::exit;
        notify(Notice::beginTransition7C,state_.current,0x2A3DD0);
        return Status::ok;
    case Life::requestCourseExit:
        state_.lifecycle=Life::waitCourseExit;state_.phase=Phase::exit;
        notify(Notice::beginTransition7C,state_.current,0x2A3E28);
        // Unlike screen/page exits, course notifies this child immediately.
        notify(Notice::pageAuxiliary1C,state_.current,0x2A3E40);
        return Status::ok;
    case Life::waitScreenExit:
    case Life::waitPageTransition:
    case Life::waitCourseExit: {
        const auto completed=poll();
        if (completed==Completion::pending) return Status::pending;
        if (completed!=Completion::complete) return Status::callbackUnavailable;
        if (state_.lifecycle==Life::waitScreenExit) {
            notify(Notice::pageAuxiliary1C,state_.current,0x2A3E8C);
            state_.lifecycle=Life::queueScreen;
        } else if (state_.lifecycle==Life::waitCourseExit) {
            state_.lifecycle=Life::queueCourse;
        } else if (state_.phase==Phase::exit) {
            notify(Notice::pageAuxiliary1C,state_.current,0x2A3F0C);
            state_.lifecycle=Life::selectRequestedPage;
        } else {
            notify(Notice::pageAuxiliary24,state_.current,0x2A3EE0);
            state_.lifecycle=Life::idle;
        }
        state_.phase=Phase::none;
        return Status::ok;
    }
    case Life::selectRequestedPage:
        // 0x2A4020: old page+0x94, new page+0x90, then current index/object.
        notify(Notice::previousPage94,state_.current,0x2A4050);
        notify(Notice::requestedPage90,state_.requested,0x2A4070);
        state_.current=state_.requested;
        notify(Notice::pageEntry74,state_.current,0x2A3CEC);
        notify(Notice::managerEntry84,state_.current,0x2A3D0C);
        state_.lifecycle=Life::waitPageTransition;state_.phase=Phase::entry;
        notify(Notice::beginTransition7C,state_.current,0x2A3D30);
        notify(Notice::pageAuxiliary1C,state_.current,0x2A3D48);
        return Status::ok;
    case Life::queueScreen:
    case Life::queueCourse: {
        const bool course=state_.lifecycle==Life::queueCourse;
        const OriginalUiApplicationRequest50KG request{
            course?9u:2u,course?0u:std::uint32_t(state_.requested.screen),
            course?0u:std::uint32_t(state_.requested.page),course?0x2A3F98u:0x2A3F70u};
        bool accepted=false;
        {
            CallbackScope guard(inCallback_);
            accepted=callbacks_.queueApplication(callbacks_.context,request);
        }
        if (!accepted) return Status::queueNotAccepted;
        state_.applicationRequestQueued=true;
        return Status::queued;
    }
    }
    return Status::callbackUnavailable; // No writable state escapes this owner.
}
} // namespace rr::game
