#pragma once
#include <cstdint>

namespace rr::game {
// Original profile+1101C, NOT the engine's different outputMode+9900 enum.
enum class OriginalSoundMode50KG : std::uint32_t { mono=0,stereo=1,surround=2 };
struct OriginalSoundModeFields50KG {
    OriginalSoundMode50KG profile1101C=OriginalSoundMode50KG::stereo;
    std::uint32_t monoE38=0,surroundE34=0,outputMode9900=1,reverseAngle9918=0;
};
inline bool projectOriginalSoundMode18874050KG(std::uint32_t profile1101C,
    OriginalSoundModeFields50KG& out) noexcept {
    if(profile1101C>2)return false;
    OriginalSoundModeFields50KG next;
    next.profile1101C=static_cast<OriginalSoundMode50KG>(profile1101C);
    next.monoE38=profile1101C==0?1u:0u;
    next.surroundE34=profile1101C==2?1u:0u;
    next.outputMode9900=profile1101C==0?0u:1u;
    //188740 ALWAYS supplies0 or1 to232B60, never its separate reverse mode2.
    next.reverseAngle9918=0;out=next;return true;
}
inline bool validOriginalSoundModeFields50KG(const OriginalSoundModeFields50KG& value) noexcept {
    OriginalSoundModeFields50KG exact;
    return projectOriginalSoundMode18874050KG(static_cast<std::uint32_t>(value.profile1101C),exact)&&
        value.monoE38==exact.monoE38&&value.surroundE34==exact.surroundE34&&
        value.outputMode9900==exact.outputMode9900&&value.reverseAngle9918==exact.reverseAngle9918;
}
// This is the engine-angle input to26CBA0, after233950/2338C0 orientation.
// -1,0,360 mean no positional panning in this combined-controls operation.
// Use only a real source angle. A host pan scalar cannot supply front/rear.
struct OriginalSoundRouting50KG {
    std::int32_t leftPercent=100,rightPercent=100;
    // Exact source packet marker, not an assumed PCM sign.0 is ordinary;
    //1 is source surround-enabled;76/82 are the source L/R selectors.
    std::uint32_t phaseSelector=0;
    bool positional=false;
};
// Exact bounded integer gain projection from26CC14..26CE7C. The original
// multiply/magic-divide operations truncate positive products by90 or89.
// It returns a source plan even when a native phase consumer is unavailable.
inline bool projectOriginalSoundRouting26CBA050KG(const OriginalSoundModeFields50KG& mode,
    std::int32_t engineAngle,std::int32_t volume,OriginalSoundRouting50KG& out) noexcept {
    if(!validOriginalSoundModeFields50KG(mode)||engineAngle< -1||engineAngle>360||
        volume<0||volume>100)return false;
    OriginalSoundRouting50KG next;
    next.leftPercent=volume;next.rightPercent=volume;
    if(mode.monoE38||engineAngle==-1||engineAngle==0||engineAngle==360){out=next;return true;}
    next.positional=true;next.phaseSelector=mode.surroundE34?1u:0u;
    if(engineAngle<180){
        if(engineAngle<=90)next.leftPercent=volume*(90-engineAngle)/90;
        else if(mode.surroundE34){
            next.leftPercent=volume*(engineAngle-90)/90;next.phaseSelector=76;
        }else next.leftPercent=volume*(engineAngle-91)/89;
    }else if(engineAngle>180){
        if(engineAngle>=270)next.rightPercent=volume*(engineAngle-270)/90;
        else if(mode.surroundE34){
            next.rightPercent=volume*(270-engineAngle)/90;next.phaseSelector=82;
        }else next.rightPercent=volume*(269-engineAngle)/89;
    }else if(mode.surroundE34)next.phaseSelector=82;
    out=next;return true;
}
// Original233950 orientation has a359 adjustment, NOT modulo360. Preserve
//0 and the endpoint behavior; do not replace this with a host azimuth guess.
inline bool projectOriginalSoundAngle23395050KG(const OriginalSoundModeFields50KG& mode,
    std::int32_t sourceAngle,std::int32_t& engineAngle) noexcept {
    if(!validOriginalSoundModeFields50KG(mode)||sourceAngle< -360||sourceAngle>360)return false;
    auto next=mode.reverseAngle9918?sourceAngle:-sourceAngle;
    if(next<0)next+=359;
    engineAngle=next;return true;
}
} // namespace rr::game
