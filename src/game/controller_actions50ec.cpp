#include "controller_actions50ec.h"

namespace rr::game {
bool bindControllerPlayerActions50EC(ControllerIngress50CX& ingress,
    std::uint32_t deviceIndex,EventRecordSources48Z& records,
    JoystickApplicationBridge48Z& application,PlayerEventChildBinding49A& player,
    ManagerEventCallbacks48Z& callbacks) noexcept {
    if(deviceIndex>=8u||!player.player||!player.environment||!player.bindings)return false;
    records.devices[deviceIndex]={ingress.buttons().records,32u};
    player.environment->held.devices[deviceIndex]=&ingress.buttons();
    application.childCallback=playerEventChild49A;
    application.childContext=&player;
    callbacks={&application,joystickReleaseBridge48Z,joystickPressBridge48Z};
    return true;
}

ControllerActions50EC::ControllerActions50EC(ControllerIngress50CX& ingress,
    InputEventQueue48Z& queue,std::uint32_t deviceIndex) noexcept
    :ingress_(ingress),queue_(queue),deviceIndex_(deviceIndex) {}

bool ControllerActions50EC::queueUnchanged() const noexcept {
    return queue_.events==leasedEvents_&&queue_.capacity==leasedCapacity_&&
           queue_.count==leasedCount_;
}
void ControllerActions50EC::rememberQueue() noexcept {
    leasedEvents_=queue_.events;
    leasedCapacity_=queue_.capacity;
    leasedCount_=queue_.count;
}

ControllerActionStatus50EC ControllerActions50EC::samplePacket(bool ready,
    bool readSucceeded,const DevicePacket48Y& packet,
    const DeviceButtonDescriptors48Y& descriptors,std::uint32_t activeCount) noexcept {
    if(phase_!=ControllerActionPhase50EC::Idle)return ControllerActionStatus50EC::WrongPhase;
    if(deviceIndex_>=8u)return ControllerActionStatus50EC::UnsupportedDeviceIndex;
    if(!ingress_.samplePacket(ready,readSucceeded,packet,descriptors,activeCount,deviceIndex_))
        return ControllerActionStatus50EC::InvalidSample;
    last_={ControllerActionStatus50EC::Ok,false,{EventDispatchStatus48Z::Complete,0,0}};
    phase_=ControllerActionPhase50EC::PendingAppend;
    return ControllerActionStatus50EC::Ok;
}

ControllerActionStatus50EC ControllerActions50EC::appendSample() noexcept {
    if(phase_!=ControllerActionPhase50EC::PendingAppend)return ControllerActionStatus50EC::WrongPhase;
    if(!appendDeviceTransitions48Z(ingress_.deviceSample(),queue_))
        return ControllerActionStatus50EC::QueueRejected;
    rememberQueue();
    phase_=ControllerActionPhase50EC::Queued;
    return ControllerActionStatus50EC::Ok;
}

ControllerActionResult50EC ControllerActions50EC::dispatch(const EventContext48Z& context,
    const EventRecordSources48Z& records,const ManagerEventCallbacks48Z& callbacks) noexcept {
    // A rejected extra attempt must not replace the first attempt's diagnostics.
    const auto reject=[](ControllerActionStatus50EC status) noexcept {
        return ControllerActionResult50EC{status,false,{EventDispatchStatus48Z::Complete,0,0}};
    };
    if(phase_!=ControllerActionPhase50EC::Queued)return reject(ControllerActionStatus50EC::WrongPhase);
    if(!queueUnchanged())return reject(ControllerActionStatus50EC::QueueChanged);
    const auto& selected=records.devices[deviceIndex_]; // Admitted by samplePacket.
    if(selected.records!=ingress_.buttons().records||selected.count!=32u)
        return reject(ControllerActionStatus50EC::MissingLiveDeviceRecords);
    phase_=ControllerActionPhase50EC::Dispatching;
    const auto result=dispatchInputEvents48Z(queue_,context,records,callbacks);
    const bool complete=result.status==EventDispatchStatus48Z::Complete;
    last_={complete?ControllerActionStatus50EC::Ok:ControllerActionStatus50EC::DispatchUnavailable,
           true,result};
    rememberQueue();
    phase_=complete?ControllerActionPhase50EC::Complete:ControllerActionPhase50EC::Blocked;
    return last_;
}

ControllerActionStatus50EC ControllerActions50EC::acknowledgeComplete() noexcept {
    if(phase_!=ControllerActionPhase50EC::Complete)return ControllerActionStatus50EC::WrongPhase;
    if(!queueUnchanged())return ControllerActionStatus50EC::QueueChanged;
    resetInputEventCount48Z(queue_);
    phase_=ControllerActionPhase50EC::Idle;
    return ControllerActionStatus50EC::Ok;
}

ControllerActionStatus50EC ControllerActions50EC::acceptExternalRecovery() noexcept {
    if(phase_!=ControllerActionPhase50EC::Blocked&&phase_!=ControllerActionPhase50EC::Complete)
        return ControllerActionStatus50EC::WrongPhase;
    if(queue_.count!=0)return ControllerActionStatus50EC::QueueRejected;
    phase_=ControllerActionPhase50EC::Idle;
    return ControllerActionStatus50EC::Ok;
}
}
