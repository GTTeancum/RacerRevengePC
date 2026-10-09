#include "event_dispatch48z.h"
#include <limits>

namespace rr::game {
static_assert(sizeof(InputEvent48Z)==20,"Original event has five words");
namespace {
bool validQueue(const InputEventQueue48Z& q) noexcept {
    return q.count<=0||(q.events&&static_cast<std::uint32_t>(q.count)<=q.capacity);
}
DeviceButtonRecord48Y* resolve(std::uint32_t selectedCategory,const InputEvent48Z& event,
                              const EventContext48Z& context,const EventRecordSources48Z& sources) noexcept {
    EventRecordSpan48Z span{};
    if(selectedCategory==0){if(!context.present)return nullptr;span=context.records;}
    else if(selectedCategory==1)span=sources.auxiliary;
    else {if(event.deviceId>=8)return nullptr;span=sources.devices[event.deviceId];}
    if(!span.records||event.code>=span.count)return nullptr;
    return &span.records[event.code];
}
}
void resetInputEventCount48Z(InputEventQueue48Z& q) noexcept {q.count=0;}
bool appendDeviceTransitions48Z(const DeviceButtonSample48Y& batch,InputEventQueue48Z& q) noexcept {
    if(batch.transitionCount>32||q.count<0||!validQueue(q))return false;
    const auto count=static_cast<std::uint32_t>(q.count);
    if(count>q.capacity||batch.transitionCount>q.capacity-count||
       batch.transitionCount>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())-count||
       (batch.transitionCount&&!q.events))return false;
    for(unsigned i=0;i<batch.transitionCount;++i){const auto& e=batch.transitions[i];q.events[count+i]={e.buttonCode,e.kind,e.deviceId,e.held,e.contextMask};}
    q.count=static_cast<std::int32_t>(count+batch.transitionCount);return true;
}

EventDispatchResult48Z dispatchInputEvents48Z(InputEventQueue48Z& q,const EventContext48Z& context,
                                            const EventRecordSources48Z& sources,
                                            const ManagerEventCallbacks48Z& callbacks) noexcept {
    EventDispatchResult48Z result{EventDispatchStatus48Z::Complete,0,0};
    if(!validQueue(q)){result.status=EventDispatchStatus48Z::InvalidQueue;return result;}
    auto* const storage=q.events;const auto capacity=q.capacity;
    while(q.count>0&&result.eventsCompleted<static_cast<std::uint32_t>(q.count)) {
        auto& event=storage[result.eventsCompleted];
        event.contextMask=context.present?context.mask1898:0u;
        const auto category=event.category;
        if(category>2){result.status=EventDispatchStatus48Z::UnsupportedCategory;return result;}
        auto invoke=[&](ManagerEventCallback48Z callback) noexcept {
            auto* record=resolve(category,event,context,sources);
            if(!record){result.status=EventDispatchStatus48Z::MissingRecord;return false;}
            if(!callback){result.status=EventDispatchStatus48Z::MissingCallback;return false;}
            ++result.callbacksInvoked;
            const auto outcome=callback(callbacks.context,event,*record);
            if(q.events!=storage||q.capacity!=capacity){result.status=EventDispatchStatus48Z::StorageChanged;return false;}
            if(!validQueue(q)){result.status=EventDispatchStatus48Z::InvalidQueue;return false;}
            if(!outcome.available){result.status=EventDispatchStatus48Z::CallbackUnavailable;return false;}
            return true; // originalReturn is deliberately ignored by the manager.
        };
        if(event.transition==0u&&!invoke(callbacks.release))return result;
        // Original reload after the release callback: may now dispatch a press
        // for the same event. Category stays selected; code/device are fresh.
        if(event.transition==1u&&!invoke(callbacks.press))return result;
        ++result.eventsCompleted;
    }
    return result;
}

EventRouteResult48Z routeChildInputEvent48Z(const EventChildRouter48Z& router,bool press,
                                          InputEvent48Z& event,DeviceButtonRecord48Y& record,
                                          EventChildCallback48Z callback,void* callbackContext,
                                          std::uint32_t budget) noexcept {
    EventRouteResult48Z result{EventRouteStatus48Z::Complete,false,0,0};
    if(!(router.flags24&(press?0x400u:0x200u)))return result;
    auto child=router.firstChild;
    auto* const storage=router.children;const auto count=router.childCount;
    while(child!=noEventChild48Z) {
        if(result.visits==budget){result.status=EventRouteStatus48Z::VisitBudgetExhausted;return result;}
        if(!storage||child>=count){result.status=EventRouteStatus48Z::InvalidNode;return result;}
        ++result.visits;
        const auto flags=storage[child].flags29;
        if((flags&1u)&&!(flags&8u)) {
            if(!callback){result.status=EventRouteStatus48Z::MissingCallback;return result;}
            ++result.callbacksInvoked;
            const auto outcome=callback(callbackContext,child,press,event,record);
            if(!outcome.available){result.status=EventRouteStatus48Z::CallbackUnavailable;return result;}
            if(outcome.originalReturn!=0u){result.handled=true;return result;}
            if(router.children!=storage||router.childCount!=count){result.status=EventRouteStatus48Z::StorageChanged;return result;}
        }
        child=storage[child].next; // Deliberately after callback, never a snapshot.
    }
    return result;
}

namespace {
EventCallbackResult48Z bridge(void* opaque,bool press,InputEvent48Z& event,DeviceButtonRecord48Y& record) noexcept {
    if(!opaque)return {false,0};
    auto& app=*static_cast<JoystickApplicationBridge48Z*>(opaque);
    app.lastRoute={EventRouteStatus48Z::Complete,false,0,0};
    if(event.category!=2){app.lastRoute.status=EventRouteStatus48Z::UnsupportedCategory;return {false,0};}
    if(app.rootFlags228&8u){app.lastRoute.handled=true;return {true,1};}
    if(press&&(app.rootFlags228&4u)){app.lastRoute.status=EventRouteStatus48Z::UnsupportedMode;return {false,0};}
    app.lastRoute=routeChildInputEvent48Z(app.root,press,event,record,app.childCallback,app.childContext,app.visitBudget);
    if(app.lastRoute.status!=EventRouteStatus48Z::Complete)return {false,0};
    // PSXGame checks this bit again after an unhandled base press. A descendant
    // may have changed it; do not silently skip that unimplemented special path.
    if(press&&!app.lastRoute.handled&&(app.rootFlags228&4u)){app.lastRoute.status=EventRouteStatus48Z::UnsupportedMode;return {false,0};}
    return {true,app.lastRoute.handled?1u:0u};
}
}
EventCallbackResult48Z joystickReleaseBridge48Z(void* p,InputEvent48Z& e,DeviceButtonRecord48Y& r) noexcept {return bridge(p,false,e,r);}
EventCallbackResult48Z joystickPressBridge48Z(void* p,InputEvent48Z& e,DeviceButtonRecord48Y& r) noexcept {return bridge(p,true,e,r);}
}
