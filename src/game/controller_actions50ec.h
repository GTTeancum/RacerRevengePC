#pragma once
#include "controller_ingress50cx.h"
#include "player_events49a.h"

namespace rr::game {
enum class ControllerActionPhase50EC {
    Idle, PendingAppend, Queued, Dispatching, Complete, Blocked
};
enum class ControllerActionStatus50EC {
    Ok, WrongPhase, UnsupportedDeviceIndex, InvalidSample, QueueRejected,
    QueueChanged, MissingLiveDeviceRecords, DispatchUnavailable
};
struct ControllerActionResult50EC {
    ControllerActionStatus50EC status;
    bool dispatchAttempted;
    EventDispatchResult48Z dispatch;
};

// Explicit one-player application binding. The caller supplies the real root
// and player child topology, flags, budgets, state, environment and synchronous
// effect/held-source callbacks. No child or action is fabricated. Other eligible
// root children will report unavailable through playerEventChild49A; callers
// needing a multi-child dispatcher must supply their own manager callbacks.
// On false nothing changes. On true only the selected device-record/held slots,
// application's child callback/context and manager callbacks are replaced.
// All owners remain alive at stable addresses through every later dispatch.
bool bindControllerPlayerActions50EC(ControllerIngress50CX&,std::uint32_t deviceIndex,
    EventRecordSources48Z&,JoystickApplicationBridge48Z&,PlayerEventChildBinding49A&,
    ManagerEventCallbacks48Z&) noexcept;

// Serial one-device transport-to-action lease. Storage, ingress and queue are
// caller-owned; this object owns only lifecycle/diagnostic metadata. Poll through
// samplePacket here instead of sampling ingress directly. Never mutate ingress
// or the leased queue outside the documented callback/recovery boundaries.
// Queue events may precede this sample; they retain their order and are included
// in the one whole-queue dispatch. Such events must not already have been run.
class ControllerActions50EC {
public:
    ControllerActions50EC(ControllerIngress50CX&,InputEventQueue48Z&,
                         std::uint32_t deviceIndex) noexcept;
    ControllerActions50EC(const ControllerActions50EC&)=delete;
    ControllerActions50EC& operator=(const ControllerActions50EC&)=delete;
    ControllerActions50EC(ControllerActions50EC&&)=delete;
    ControllerActions50EC& operator=(ControllerActions50EC&&)=delete;

    // Cold registration only, before the bank is sealed. The ingress/queue
    // references stay bound to the same live owners, registered separately.
    // Keep every owned field, including acknowledged lease diagnostics.
    template<class Bank> void registerRacerState(Bank& bank) {
        bank.owner(deviceIndex_);
        bank.owner(phase_);
        bank.owner(leasedEvents_);
        bank.owner(leasedCapacity_);
        bank.owner(leasedCount_);
        bank.owner(last_);
    }

    // Idle only. A successful poll owns its complete batch until acknowledged;
    // even NotReady/ReadFailed require the append/dispatch/acknowledge sequence.
    // Inspect ingress.deviceSample().status independently of this native status.
    // Invalid device/sample preserves ingress, queue and this phase. No queue
    // capacity guess, synthetic release or host-to-packet mapping is introduced.
    ControllerActionStatus50EC samplePacket(bool deviceReady,bool readSucceeded,
        const DevicePacket48Y&,const DeviceButtonDescriptors48Y&,
        std::uint32_t activeCount) noexcept;

    // PendingAppend only. Calls the existing atomic48Z append: failure leaves
    // queue and sample intact for a capacity/storage repair and another attempt.
    // Success leases queue pointer/capacity/count until dispatch. The caller may
    // not append more polls/events or overwrite the batch behind this object.
    ControllerActionStatus50EC appendSample() noexcept;

    // Queued only. Requires the selected device span to reference the ingress's
    // LIVE32-record bank. Preflight rejection leaves Queued and has no effects.
    // Calls48Z exactly once with the actual supplied context/sources/callbacks;
    // its live callback mutation semantics and partial-effect failures apply.
    // No copying of player/environment/queue, action replay or deferred effects.
    // A failed native dispatch blocks automatic reuse, even at eventsCompleted0:
    // the current event may already have produced external effects.
    ControllerActionResult50EC dispatch(const EventContext48Z&,
        const EventRecordSources48Z&,const ManagerEventCallbacks48Z&) noexcept;

    // Complete only. Explicitly acknowledges the completed whole queue and
    // resets its count with48Z, then permits another sample. Rejects changed
    // queue metadata; event bytes are never erased. This is caller bookkeeping,
    // not a newly inferred original scheduler reset point.
    ControllerActionStatus50EC acknowledgeComplete() noexcept;

    // Blocked or Complete only. The caller must first resolve/retain every effect and
    // undispatched event externally and explicitly set queue.count=0. This merely
    // releases the latch; it neither repairs callbacks nor resets/replays data.
    // Pointer/capacity changes are permitted here after the recovery owner has
    // safely dealt with both original and replacement storage, if applicable.
    // Complete also permits recovery when an external owner changed metadata
    // before acknowledgeComplete. No unattempted batch can be released here.
    ControllerActionStatus50EC acceptExternalRecovery() noexcept;

    ControllerActionPhase50EC phase() const noexcept {return phase_;}
    std::uint32_t deviceIndex() const noexcept {return deviceIndex_;}
    const ControllerActionResult50EC& lastDispatch() const noexcept {return last_;}

private:
    bool queueUnchanged() const noexcept;
    void rememberQueue() noexcept;
    ControllerIngress50CX& ingress_;
    InputEventQueue48Z& queue_;
    std::uint32_t deviceIndex_;
    ControllerActionPhase50EC phase_=ControllerActionPhase50EC::Idle;
    InputEvent48Z* leasedEvents_=nullptr;
    std::uint32_t leasedCapacity_=0;
    std::int32_t leasedCount_=0;
    ControllerActionResult50EC last_{ControllerActionStatus50EC::Ok,false,
                                   {EventDispatchStatus48Z::Complete,0,0}};
};
}
