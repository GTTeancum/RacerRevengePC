#pragma once
#include "device_held48y.h"
#include <cstdint>

namespace rr::game {
struct InputEvent48Z {std::uint32_t code,category,deviceId,transition,contextMask;};
struct InputEventQueue48Z {InputEvent48Z* events;std::uint32_t capacity;std::int32_t count;};
struct EventRecordSpan48Z {DeviceButtonRecord48Y* records;std::uint32_t count;};
struct EventContext48Z {bool present;std::uint32_t mask1898;EventRecordSpan48Z records;};
struct EventRecordSources48Z {EventRecordSpan48Z auxiliary;EventRecordSpan48Z devices[8];};
// available is a native support/error boundary. originalReturn is the original
// callback result, which manager dispatch ignores (including nonzero values).
struct EventCallbackResult48Z {bool available;std::uint32_t originalReturn;};
using ManagerEventCallback48Z=EventCallbackResult48Z (*)(void*,InputEvent48Z&,DeviceButtonRecord48Y&) noexcept;
struct ManagerEventCallbacks48Z {void* context;ManagerEventCallback48Z release,press;};
enum class EventDispatchStatus48Z {Complete,InvalidQueue,UnsupportedCategory,MissingRecord,
                                  MissingCallback,CallbackUnavailable,StorageChanged};
struct EventDispatchResult48Z {EventDispatchStatus48Z status;std::uint32_t eventsCompleted,callbacksInvoked;};
void resetInputEventCount48Z(InputEventQueue48Z&) noexcept;
// Bounded adapter from48Y's ordered batch. Failure does not mutate the queue.
// Capacity is caller-supplied native storage, not an inferred original queue size.
bool appendDeviceTransitions48Z(const DeviceButtonSample48Y&,InputEventQueue48Z&) noexcept;

// Original supplied-event loop12815C..128430. Tags each event from the live
// context, selects category once, then release and possibly press after live
// reloads. Callback mutation may grow/shrink count, retarget code/device and
// modify records/context. Callback return does not stop the queue. Storage must
// remain alive/stable; replacements are reported before subsequent access.
// Errors preserve completed effects and report partial progress: external
// callback mutations cannot be rolled back. No transport/update/rebuild implied.
EventDispatchResult48Z dispatchInputEvents48Z(InputEventQueue48Z&,const EventContext48Z&,
                                            const EventRecordSources48Z&,
                                            const ManagerEventCallbacks48Z&) noexcept;

inline constexpr std::uint32_t noEventChild48Z=0xffffffffu;
struct EventChild48Z {std::uint32_t next;std::uint8_t flags29;};
struct EventChildRouter48Z {
    std::uint32_t flags24,firstChild;
    EventChild48Z* children;
    std::uint32_t childCount;
};
using EventChildCallback48Z=EventCallbackResult48Z (*)(void*,std::uint32_t child,bool press,
                                                    InputEvent48Z&,DeviceButtonRecord48Y&) noexcept;
enum class EventRouteStatus48Z {Complete,InvalidNode,MissingCallback,CallbackUnavailable,
                               VisitBudgetExhausted,StorageChanged,UnsupportedCategory,UnsupportedMode};
struct EventRouteResult48Z {EventRouteStatus48Z status;bool handled;std::uint32_t visits,callbacksInvoked;};
// Original1422A0/142350 one-router contracts. Parent gate and first child loaded
// once; child flags and next links are live. Stop first nonzero child return.
// Caller visitBudget bounds even callback-created cycles. It is a native domain
// gate, not an invented original limit. This routine uses no recursive stack;
// concrete descendant handlers remain explicit supplied bindings.
EventRouteResult48Z routeChildInputEvent48Z(const EventChildRouter48Z&,bool press,
                                          InputEvent48Z&,DeviceButtonRecord48Y&,
                                          EventChildCallback48Z,void* callbackContext,
                                          std::uint32_t visitBudget) noexcept;
struct JoystickApplicationBridge48Z {
    std::uint8_t rootFlags228;
    EventChildRouter48Z root;
    EventChildCallback48Z childCallback;
    void* childContext;
    std::uint32_t visitBudget;
    EventRouteResult48Z lastRoute;
};
// Constructor/vtable-proven category2 path through Game/PSXGame/PodGame.
// Root bit3 consumes without forwarding. Normal press requires bit2 clear;
// special bit2 paths and non-category2 PodGame behavior remain unsupported.
// Child effects are not fabricated. Manager still ignores handled results.
EventCallbackResult48Z joystickReleaseBridge48Z(void*,InputEvent48Z&,DeviceButtonRecord48Y&) noexcept;
EventCallbackResult48Z joystickPressBridge48Z(void*,InputEvent48Z&,DeviceButtonRecord48Y&) noexcept;
}
