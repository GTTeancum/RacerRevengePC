#pragma once
#include "ai_speed_braking50ir.h"

namespace rr::game {
// Source172B50..172C4C, called only by16EAB0 after the same172380 steering
// owner used while racing. It never calls172C50/173180 or chooses a speed.
// Source94 is the actual shared race end clock. Preserve B5C/B60 steering
// outputs; source clears only B64/B68 and the low three B9C bits.
inline bool applyOriginalAiEndingSpeed172B50(float cachedSpeed34,
    float raceCountdown94,PlayerReadResult48X& controls,
    AiSpeedDiagnostic50IR& diagnostic) noexcept {
    namespace d=ai_speed_detail50IR;
    diagnostic={};
    if(std::fegetround()!=FE_TONEAREST)
        return d::fail(diagnostic,AiSpeedError50IR::RoundingMode,0x172b50u);
    if(!d::admitted(cachedSpeed34)||!d::admitted(raceCountdown94))
        return d::fail(diagnostic,AiSpeedError50IR::Input,0x172b64u);
    d::Math math;const float speed=math.mul(d::word(0x3f2e8bc1u),cachedSpeed34);
    if(!math.valid)return d::fail(diagnostic,AiSpeedError50IR::Arithmetic,0x172b70u);
    auto next=controls;
    if(speed>160.f&&raceCountdown94>1.f){next.actionB7C=0.f;next.actionB80=1.f;}
    else if(raceCountdown94>1.25f){next.actionB7C=1.f;next.actionB80=1.f;}
    else {next.actionB7C=0.f;next.actionB80=0.f;}
    next.raw.values[2]=0.f;next.raw.values[3]=0.f;
    next.flagsB9C=static_cast<std::uint8_t>(next.flagsB9C&~7u);
    controls=next;return true;
}
} // namespace rr::game
