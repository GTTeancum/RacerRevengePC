#include "original_video_blocking_native.h"

namespace rr::platform {
OriginalVideoBlockingNativeBackend::OriginalVideoBlockingNativeBackend(
    VehicleWindowGl50DS& renderer,GameHost50ER& host,OriginalVideoNativeUse use,
    NativeVideoPacketOverlayPolicies50LA blendPolicies) noexcept :
    renderer_(renderer),host_(host),use_(use),blendPolicies_(blendPolicies),window_(host.hasWindow()) {
    // These two private bindings are the concrete host implementation, not a
    // plugin/no-op completion interface. Only users of this optional backend
    // need this translation unit and GameHost50ER's actual window/EGL owners.
    checkHost_=[](GameHost50ER& actual,bool window,std::int32_t w,std::int32_t h,
        VehicleWindowDiagnostic50DS& d) noexcept {
        if(!actual.created() || !actual.current() ||
           actual.mode()!=(window?GameHostMode50ER::Window:GameHostMode50ER::HeadlessLive)){
            d.error=VehicleWindowError50DS::context;d.operation="display fade requires its exact live GameHost50ER mode/context";
            return false;
        }
        if(w && (actual.info().drawableWidth!=w || actual.info().drawableHeight!=h)){
            d.error=VehicleWindowError50DS::presentation;d.operation="display fade requires unchanged actual host drawable dimensions";
            return false;
        }
        return true;
    };
    swapHost_=[](GameHost50ER& actual,GameHostDiagnostic50ER& d) noexcept {
        return actual.swapWindow(d);
    };
}
}
