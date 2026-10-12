#include "vehicle_window_gl50ds.h"
#include "game_host50er.h"

namespace rr::platform {
bool VehicleWindowGl50DS::completeGameplayDisplay(GameHost50ER& host,
    GameHostDiagnostic50ER& hostDiagnostic,VehicleWindowDiagnostic50DS& diagnostic) noexcept {
    hostDiagnostic={};
    // Concrete host bindings stay in this optional translation unit. Ordinary
    // renderer-only targets do not acquire a GameHost/window/EGL dependency.
    const auto checkHost=[](GameHost50ER& actual,bool window,std::int32_t width,
        std::int32_t height,VehicleWindowDiagnostic50DS& d) noexcept {
        if(!actual.created() || !actual.current() ||
            actual.mode()!=(window?GameHostMode50ER::Window:GameHostMode50ER::HeadlessLive)){
            d.error=VehicleWindowError50DS::context;
            d.operation="gameplay display requires exact live GameHost mode and context";
            return false;
        }
        if(window && (actual.info().minimized || actual.info().drawableWidth!=width ||
            actual.info().drawableHeight!=height)){
            d.error=VehicleWindowError50DS::presentation;
            d.operation="window dimensions changed before gameplay display completion";
            return false;
        }
        return true;
    };
    const auto swapHost=[](GameHost50ER& actual,GameHostDiagnostic50ER& d) noexcept {
        return actual.swapWindow(d);
    };
    return completeGameplayDisplayImpl(host,hostDiagnostic,checkHost,swapHost,diagnostic);
}
} // namespace rr::platform
