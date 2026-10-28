// Existing Session special member outlined intact; no new owner or traits.
#include <exception>
#include "original_camera_session50kx.h"

namespace original_camera_session50KX {
Session::~Session(){
        invalidateSelectedGameplayCamera50LA(); //Native observation only; not115690.
        if(coldController_.entity.reference00)std::terminate();
        if(nativeTaCutChildPhase_==NativeTaCutChildPhase::constructed&&
           !abortUnpublishedTaCutChildNative())std::terminate();
    }
} // namespace original_camera_session50KX
