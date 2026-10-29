#pragma once
#include "original_camera_start_effects.h"

namespace original_camera_session50KX {class Session;}

namespace rr::game {
// Typed, once-only projection of174E2C..174EC0 into the actual controller's
// retained cold cut-count. Track3E4 stays immutable in authenticated authoring.
// This is not a caller-provided "skip intro" receipt and does not activate any
// child. A positive effective count requires the real controller74 collection,
//175510 resource loading,177750 audio80 and1756D0 cut lifecycle.
class OriginalCameraStartPrelude final {
    friend class ::original_camera_session50KX::Session;
    // Optional native custody of the SAME actual counted controller halfword.
    // Once bound, the standalone numeric projection is never authoritative.
    std::uint16_t* actualA8Value_=nullptr;
    std::uint16_t* actualA8Known_=nullptr;
    bool bindingAttempted_=false,bindingRevoked_=false,scenePublished_=false;
    bool bindControllerA8(std::uint16_t& value,std::uint16_t& known)noexcept {
        if(bindingAttempted_||prepared_||&value==&known)return false;
        bindingAttempted_=true;actualA8Value_=&value;actualA8Known_=&known;return true;
    }
    bool currentControllerA8(const std::uint16_t& value,const std::uint16_t& known)const noexcept {
        return bindingAttempted_&&!bindingRevoked_&&actualA8Value_==&value&&actualA8Known_==&known;
    }
    bool publishSceneCutCount174E58(std::int16_t actualWorld3E4)noexcept {
        if(!bindingAttempted_||bindingRevoked_||!actualA8Value_||!actualA8Known_||
           scenePublished_||prepared_)return false;
        //174E48 signed LH then ANDI31. Preserve ALL unrelated bits and their
        //known-mask, including unknown high lanes; no full-halfword invention.
        const auto low5=static_cast<std::uint16_t>(actualWorld3E4)&31u;
        *actualA8Value_=static_cast<std::uint16_t>((*actualA8Value_&0xfe0fu)|(low5<<4u));
        *actualA8Known_=static_cast<std::uint16_t>(*actualA8Known_|0x01f0u);
        sourceSceneCutCount3E4_=actualWorld3E4;scenePublished_=true;return true;
    }
    bool finishControllerCutCount174EC0(const std::uint32_t& actualLaunchKind11258,
        const std::uint32_t& actualConfiguration34,const std::int32_t& actualApplication350)noexcept {
        if(!bindingAttempted_||bindingRevoked_||!scenePublished_||prepared_||
           !actualA8Value_||!actualA8Known_||(*actualA8Known_&0x01f0u)!=0x01f0u)return false;
        //Caller reads the real profile/process operands only AFTER174E58.
        //The earlier scene prefix survives any caller failure before this call.
        if(actualLaunchKind11258==2u||actualLaunchKind11258==3u||
           (actualLaunchKind11258==1u&&actualConfiguration34!=2u&&actualApplication350==0))
            *actualA8Value_=static_cast<std::uint16_t>(*actualA8Value_&0xfe0fu); //174EC0
        prepared_=true;return true;
    }
    void withdrawControllerA8()noexcept {
        if(!bindingAttempted_)return;
        bindingRevoked_=true;actualA8Value_=nullptr;actualA8Known_=nullptr;
    }
    bool prepared_=false;
    std::int32_t sourceSceneCutCount3E4_=-1,controllerCutCountA8_=-1;
public:
    bool prepared() const noexcept {return prepared_&&!bindingRevoked_;}
    std::int32_t sourceSceneCutCount3E4() const noexcept {return sourceSceneCutCount3E4_;}
    std::int32_t controllerCutCountA8() const noexcept {
        if(!bindingAttempted_)return controllerCutCountA8_;
        if(bindingRevoked_||!prepared_||!scenePublished_||!actualA8Value_||!actualA8Known_||
           (*actualA8Known_&0x01f0u)!=0x01f0u)return -1;
        const auto low5=static_cast<std::uint32_t>(*actualA8Value_>>4u)&31u;
        return low5<16u?static_cast<std::int32_t>(low5):static_cast<std::int32_t>(low5)-32;
    }
    // Borrow the actual selected profile34, launch11258 and process350 fields.
    //17C784 initializes350=0 ONCE at application construction. The source
    // command-line startup sets it to1 at17D104 iff source argc435378>=2;
    // native argv/environment options are not a substitute for that lifecycle.
    bool prepare174DF0(const OriginalCourseStartActions& authoring,
        const std::uint32_t& actualLaunchKind11258,
        const std::uint32_t& actualConfiguration34,
        const std::int32_t& actualApplication350) noexcept {
        if(bindingAttempted_||prepared_||!authoring.authenticated())return false;
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
