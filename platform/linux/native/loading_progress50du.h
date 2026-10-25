#pragma once

// Host-only cooperative cold-load observation. No source readiness, game clock,
// gameplay input or rendering capability is published by this interface. It is
// dormant outside the root's explicitly enabled normal player loading scope.
// CPU loaders may pulse from noexcept code; cancellation is latched and only
// acted upon at a caller's existing cleanup-protected boundary.
namespace rr::platform::loading_progress50DU {
struct Observer {
    void* context=nullptr;
    void (*pulse)(void*,const char*)noexcept=nullptr;
    void (*finish)(void*)noexcept=nullptr;
};
inline thread_local Observer observer{};
inline void pulse(const char* stage=nullptr)noexcept {
    if(observer.pulse)observer.pulse(observer.context,stage);
}
inline void finish()noexcept {
    if(observer.finish)observer.finish(observer.context);
}
}
