#pragma once
#include "original_camera_start_effects.h"

namespace rr::game {
// Typed, once-only projection of174E2C..174EC0 into the actual controller's
// retained cold cut-count. Track3E4 stays immutable in authenticated authoring.
// This is not a caller-provided "skip intro" receipt and does not activate any
// child. A positive effective count requires the real controller74 collection,
//175510 resource loading,177750 audio80 and1756D0 cut lifecycle.
class OriginalCameraStartPrelude final {
    bool prepared_=false;
    std::int32_t sourceSceneCutCount3E4_=-1,controllerCutCountA8_=-1;
public:
    bool prepared() const noexcept {return prepared_;}
    std::int32_t sourceSceneCutCount3E4() const noexcept {return sourceSceneCutCount3E4_;}
    std::int32_t controllerCutCountA8() const noexcept {return controllerCutCountA8_;}
    // Borrow the actual selected profile34, launch11258 and process350 fields.
    //17C784 initializes350=0 ONCE at application construction. The source
    // command-line startup sets it to1 at17D104 iff source argc435378>=2;
    // native argv/environment options are not a substitute for that lifecycle.
    bool prepare174DF0(const OriginalCourseStartActions& authoring,
        const std::uint32_t& actualLaunchKind11258,
        const std::uint32_t& actualConfiguration34,
        const std::int32_t& actualApplication350) noexcept {
        if(prepared_||!authoring.authenticated())return false;
        const auto raw=authoring.sourceSceneCutCount3E4();
        if(raw<0)return false;
        //174E48 LH, ANDI31, then A8 bits4..8. Later consumers sign-extend
        // that five-bit field. Current authenticated courses have only0/3/4/5.
        const auto low5=static_cast<std::uint32_t>(raw)&31u;
        std::int32_t effective=low5<16u?static_cast<std::int32_t>(low5):
            static_cast<std::int32_t>(low5)-32;
        if(actualLaunchKind11258==2u||actualLaunchKind11258==3u||
           (actualLaunchKind11258==1u&&actualConfiguration34!=2u&&actualApplication350==0))
            effective=0;
        sourceSceneCutCount3E4_=raw;controllerCutCountA8_=effective;prepared_=true;
        return true;
    }
};
}
