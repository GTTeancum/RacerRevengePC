#pragma once
#include <cstdint>

// Explicit original loading-bar steps. Each call stands for one original
// 1CC900(loadscreen, delta) call and names that call's jal PC. Kept apart from
// loading_progress50du.h (same namespace and thread) so the step API does not
// change the pulse observer every CPU loader includes. Dormant unless the
// player loading session installs an observer.
namespace rr::platform::loading_progress50DU {
struct StepObserver {
    void* context=nullptr;
    void (*step)(void*,float,std::uint32_t)noexcept=nullptr;
};
inline thread_local StepObserver stepObserver{};
inline void step(float delta,std::uint32_t sourcePC)noexcept {
    if(stepObserver.step)stepObserver.step(stepObserver.context,delta,sourcePC);
}
}
