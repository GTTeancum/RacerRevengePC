#include "original_camera_racer_history.h"
#include <cfenv>
#include <cstring>

namespace rr::game {
BasePoseHistoryState50Y projectOriginalBaseHistoryConstructor(
    const BasePoseHistoryState50Y& prior,
    const std::array<std::uint32_t,4>& actual434630) noexcept {
    auto next=prior;
    for(auto& entry:next.historyD50){
        for(unsigned lane=0;lane<4;++lane)entry[lane]=actual434630[lane];
        entry[4]=0;
    }
    next.currentFD0=0;next.nextFD4=19;return next;
}
namespace {
float fp(std::uint32_t word) noexcept {
    float value;std::memcpy(&value,&word,sizeof value);return value;
}
std::uint32_t bits(float value) noexcept {
    std::uint32_t word;std::memcpy(&word,&value,sizeof word);return word;
}
struct Arithmetic {
    bool okay=true;
    bool admit(float value) noexcept {
        const auto word=bits(value),exponent=word&0x7f800000u;
        const bool valid=exponent!=0x7f800000u&&
            (exponent!=0u||(word&0x007fffffu)==0u);
        okay=okay&&valid;return valid;
    }
    float add(float left,float right) noexcept {
        if(!admit(left)||!admit(right))return 0.f;
        volatile float result=left+right;admit(result);return result;
    }
    float sub(float left,float right) noexcept {
        if(!admit(left)||!admit(right))return 0.f;
        volatile float result=left-right;admit(result);return result;
    }
    float mul(float left,float right) noexcept {
        if(!admit(left)||!admit(right))return 0.f;
        volatile float result=left*right;admit(result);return result;
    }
    float div(float left,float right) noexcept {
        if(!admit(left)||!admit(right))return 0.f;
        if(right==0.f){okay=false;return 0.f;}
        volatile float result=left/right;admit(result);return result;
    }
};
OriginalCameraRacerHistoryResult failure(OriginalCameraRacerHistoryStatus status) noexcept {
    OriginalCameraRacerHistoryResult result{};result.status=status;return result;
}
}
OriginalCameraRacerHistoryResult readOriginalCameraRacerHistory(
    const BasePoseHistoryState50Y& history,float cameraDistance04) noexcept {
    using Status=OriginalCameraRacerHistoryStatus;
    if(history.currentFD0>=20u)return failure(Status::InvalidInput);
    if(std::fegetround()!=FE_TONEAREST)return failure(Status::UnsupportedArithmetic);
    Arithmetic arithmetic;
    if(!arithmetic.admit(cameraDistance04))return failure(Status::UnsupportedArithmetic);
    std::uint32_t previousPoint[4]={0u,0u,0u,0x3f800000u}; //17AE34 vf0
    std::uint32_t slot=history.currentFD0,traversed=0;
    float preceding=0.f;
    for(;;){
        if(traversed>=20u)return failure(Status::InsufficientHistory);
        const float distance=fp(history.historyD50[slot][4]);
        const float candidate=arithmetic.add(preceding,distance); //17AEA0
        if(!arithmetic.okay)return failure(Status::UnsupportedArithmetic);
        if(!(candidate<cameraDistance04))break; //17AEA4, strict
        preceding=arithmetic.add(preceding,distance); //17AE68 repeats the add
        if(!arithmetic.okay)return failure(Status::UnsupportedArithmetic);
        std::memcpy(previousPoint,history.historyD50[slot],sizeof previousPoint);
        slot=slot+1u<20u?slot+1u:0u;
        ++traversed;
    }
    const float remainder=arithmetic.sub(cameraDistance04,preceding); //17AF3C
    const float fraction=arithmetic.div(remainder,fp(history.historyD50[slot][4])); //17AF58
    if(!arithmetic.okay)return failure(Status::UnsupportedArithmetic);
    float delta[3],scaled[3];
    for(unsigned axis=0;axis<3;++axis)
        delta[axis]=arithmetic.sub(fp(history.historyD50[slot][axis]),fp(previousPoint[axis]));
    for(unsigned axis=0;axis<3;++axis)
        scaled[axis]=arithmetic.mul(fraction,delta[axis]); //17AF8C..98
    OriginalCameraRacerHistoryResult result{};
    for(unsigned axis=0;axis<3;++axis)
        result.point180[axis]=arithmetic.add(fp(previousPoint[axis]),scaled[axis]);
    if(!arithmetic.okay)return failure(Status::UnsupportedArithmetic);
    result.point180[3]=1.f; //17AFD4; source106790 also sets intermediate W=1
    result.selectedSlot=slot;result.traversedSlots=traversed;
    result.precedingDistance=preceding;result.fraction=fraction;
    result.status=Status::Complete;return result;
}
}
