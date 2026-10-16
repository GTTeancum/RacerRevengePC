#pragma once
#include "original_gameplay_blur_leaf.h"
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace rr::game {

// SOURCE-ONLY, UNWIRED CANDIDATE. This owns the closed camera control/effect
// fields, not the unrecovered Follow pose/query implementation. No raw guest
// addresses, virtual calls, callbacks, allocation, renderer or AI control writes.
// Requires the separately supplied original_gameplay_blur_leaf.h candidate.
// Original ELF SHA256:
// c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30

enum class CameraRacerClass : std::uint8_t { unavailable, player, ai };
enum class CameraBootstrap : std::uint8_t {
    original_single_player, // Original1A6C54 forces one Player before opponents.
    native_all_ai           // Explicit current-native bootstrap variation.
};

struct RaceCameraBinding {
    // Stable native keys supplied by the actual scene/racer/body owners. These
    // are NOT source pointers. Their lifetime and uniqueness are caller-owned.
    std::uint64_t sessionGeneration = 0;
    std::uint64_t sceneKey = 0, racerKey = 0, mainBodyKey = 0;
    std::uint32_t managerIndex = 0;
    CameraRacerClass racerClass = CameraRacerClass::unavailable;
    CameraBootstrap bootstrap = CameraBootstrap::original_single_player;
};

struct OriginalRaceCameraOwner {
    RaceCameraBinding binding{};
    bool bound = false;
    //174A30 constructor fields. Mode4 is the cold intro mode, NOT Follow0.
    std::int32_t mode44 = 4;
    std::uint8_t flags62 = 0;
    // Only source63 bits0..4 are represented; constructor leaves its high
    // three bits untouched and this closed slice never reads them.
    std::uint8_t flags63Low5 = 4;
    float cooldown8C = 0.f;
    float cutRemaining90 = 0.f;
    float shakeRemaining94 = -1.f, shakeDuration98 = 0.f;
    float shakeMagnitude9C = 0.f, shakeDelayA0 = 0.f;
    float specialDelayA4 = 1.f;
    bool shakeDecayingA9 = false;
};

// Closed1A95B0 ->174A30/174DF0 ->1A73D8 ->1750A0 identity/state projection.
//1A73DC passes manager index0;17511C stores the exact common racer owner;
//1752A0 binds TV to that racer's340 main body. Camera is a separate race owner.
// This does not run intro resources, registration, mode-setting177AB0, scene
// attachment or the Follow constructor. It must not be called "camera ready".
// In ALL8, preserve genuine AI class and all existing input/control ownership.
inline bool bindFreshOriginalRaceCamera(OriginalRaceCameraOwner& destination,
    const RaceCameraBinding& binding) noexcept {
    if ((binding.bootstrap != CameraBootstrap::original_single_player &&
         binding.bootstrap != CameraBootstrap::native_all_ai) ||
        !binding.sessionGeneration || !binding.sceneKey || !binding.racerKey ||
        !binding.mainBodyKey || binding.managerIndex != 0 ||
        (binding.bootstrap == CameraBootstrap::original_single_player &&
         binding.racerClass != CameraRacerClass::player) ||
        (binding.bootstrap == CameraBootstrap::native_all_ai &&
         binding.racerClass != CameraRacerClass::ai)) return false;
    OriginalRaceCameraOwner next{};
    next.binding = binding;
    next.bound = true;
    destination = next;
    return true;
}

//1B5990 authored CameraShakeTriggerAction fields/defaults. Defaults do not
// imply that a trigger exists or was entered. An actual loaded action and
// genuine enter event are indispensable; exit has no effect (1B2784).
struct OriginalCameraShakeAction {
    float magnitude = 0.f; // Magnitude -> action10C, default0.
    float time = 0.f;      // ShakeTime -> action110, default0.
    std::int32_t decaying = 1; // Decaying -> action114, default1.
    float delay = 0.f;     // Delay -> action118, default0.
};

// Complete closed scalar1B2780 ->1ACEA0 ->177720 action projection. It broadcasts
// to every original registered race camera, regardless of the entering racer.
// The race wrapper reverses the two float arguments: duration=time, amplitude=
// magnitude. Decaying uses its low bit, not generic nonzero truth.
// The caller supplies the actual race camera order/count (source1B0&3, max3).
// No blur request is issued until the later camera update consumes the timer.
inline bool applyOriginalCameraShakeEnter(bool entering,
    const OriginalCameraShakeAction& action, OriginalRaceCameraOwner* cameras,
    std::size_t count) noexcept {
    if (!entering) return true;
    if (!count) return true; // Source zero-camera broadcast is an empty loop.
    if (count > 3 || !cameras) return false;
    for (std::size_t i = 0; i < count; ++i) if (!cameras[i].bound) return false;
    for (std::size_t i = 0; i < count; ++i) {
        auto& c = cameras[i];
        c.shakeDuration98 = action.time;
        c.shakeRemaining94 = action.time;
        c.shakeMagnitude9C = action.magnitude;
        c.shakeDecayingA9 = (static_cast<std::uint32_t>(action.decaying) & 1u) != 0;
        c.shakeDelayA0 = action.delay;
    }
    return true;
}

// Exact fields written by17BB60 and13E530. Bind to the same actual child-camera
// owners as controller64 and6C; initial values come from their own construction.
// This record is not a new shake generator or a replacement for those owners.
struct OriginalCameraShakeChannels {
    float followMagnitude174, followFactor170;
    float attachedMagnitude2C, attachedFactor28;
};

struct StableRaceCameraFrame {
    std::uint64_t sessionGeneration = 0, racerKey = 0, mainBodyKey = 0;
    float elapsed = 0.f; // Actual original prepared update delta.
    std::uint8_t raceStateFlags58 = 0, raceFlags1B0 = 0;
    std::int32_t racerState40 = 0, racerCD4 = 0;
    float raceTime94 = 0.f; // Only consumed for the reached source bit0 branch.
};

enum class RaceCameraControlResult : std::uint8_t {
    advanced_needs_follow_pose, // This closed control slice completed.
    unavailable_binding,
    requires_other_original_camera_path,
    outside_native_numeric_domain
};
struct RaceCameraControlReceipt {
    RaceCameraControlResult result = RaceCameraControlResult::unavailable_binding;
    bool requestedBlur = false;
    std::int32_t requestedStrength = 0;
    bool skippedShakeTail = false;
    // Original115CB0 still must invoke the selected child-camera update, then
    // scene/matrix update. This receipt never claims that work was performed.
};

namespace original_race_camera_detail {
inline bool scalar(float x) noexcept {
    return std::isfinite(x) &&
        (x == 0.f || std::fabs(x) >= std::numeric_limits<float>::min());
}
inline float sub(float a, float b) noexcept {volatile float r = a-b; return r;}
inline float div(float a, float b) noexcept {volatile float r = a/b; return r;}
inline float mul(float a, float b) noexcept {volatile float r = a*b; return r;}
inline bool strength(float value, std::int32_t& result) noexcept {
    if (!scalar(value) || value < -2147483648.f || value >= 2147483648.f)
        return false;
    result = static_cast<std::int32_t>(value); // Original CVT.W.S truncation.
    return true;
}
// Shared source175D90..175F4C prefix, used by both the ordinary controller
// and the genuine ending controller. Callers supply staged copies and perform
// the actual binding/mode/frame admission before calling this numerical leaf.
inline bool advanceCommonPrefix175D70(OriginalRaceCameraOwner& next,
    OriginalCameraShakeChannels& shake,GameplayBlurPending& requests,float elapsed,
    bool& attemptedBlur,std::int32_t& stagedStrength) noexcept {
    if (next.cooldown8C > 0.f) {
        next.cooldown8C = sub(next.cooldown8C,elapsed);
        if (!scalar(next.cooldown8C)) return false;
    }
    // Cut timer always has priority; the alternate timer/delay freezes beneath it.
    if (next.cutRemaining90 >= 0.f) {
        const float ratio = div(next.cutRemaining90,2.f);
        const float value = mul(100.f,ratio);
        if (!scalar(ratio) || !strength(value,stagedStrength)) return false;
        attemptedBlur = true;
        requestGameplayBlur(requests,stagedStrength,0,0);
        if (next.cutRemaining90 == 0.f) next.cutRemaining90 = -1.f;
        else {
            const float remaining = sub(next.cutRemaining90,elapsed);
            if (!scalar(remaining)) return false;
            next.cutRemaining90 = remaining <= 0.f ? 0.f : remaining;
        }
    } else {
        if (!scalar(next.shakeRemaining94)) return false;
        if (next.shakeRemaining94 >= 0.f) {
            if (!scalar(next.shakeDelayA0)) return false;
            if (next.shakeDelayA0 > 0.f) {
                next.shakeDelayA0 = sub(next.shakeDelayA0,elapsed);
                if (!scalar(next.shakeDelayA0)) return false;
            } else {
                stagedStrength = 75;
                if (next.shakeDecayingA9) {
                    if (!scalar(next.shakeDuration98) || next.shakeDuration98 == 0.f)
                        return false;
                    const float ratio = div(next.shakeRemaining94,next.shakeDuration98);
                    const float value = mul(80.f,ratio);
                    if (!scalar(ratio) || !strength(value,stagedStrength)) return false;
                }
                attemptedBlur = true;
                requestGameplayBlur(requests,stagedStrength,0,0);
                if (next.shakeRemaining94 == 0.f) {
                    next.shakeRemaining94 = -1.f;
                    next.shakeDuration98 = 0.f;
                    if (next.mode44 == 0) {
                        shake.followMagnitude174 = 0.f;
                        shake.followFactor170 = 0.f;
                    } else if (next.mode44 == 5) {
                        shake.attachedMagnitude2C = 0.f;
                        shake.attachedFactor28 = 0.f;
                    }
                } else {
                    const float remaining = sub(next.shakeRemaining94,elapsed);
                    if (!scalar(remaining)) return false;
                    next.shakeRemaining94 = remaining <= 0.f ? 0.f : remaining;
                }
            }
        }
    }
    return true;
}
}

// Original175D70's stable ordinary mode0/5 control path, excluding the final
// selected-camera pose call115CB0. All reached prefix requests, delay behavior,
// terminal writes and camera-shake setters are represented in source order.
// The source-owned mode must already have been established by actual lifecycle;
// this API deliberately has no "pretend Follow" or "originalOwnerAvailable"
// switch. Cold mode4 rejects and cannot activate effects by default.
//
// Admission excludes mode changes, TV/replay, racer states outside0/1/2 and
// the positive-time special bit0 branch. Source states1/2 retain mode0/5; only
// states3/4 select death child68 at176748. A nonzero CD4 is kept verbatim and
// correctly prevents the separate manager-ended finish-orbit branch. ALL8 may
// use the closed common-base camera
// consumer with explicit native_all_ai binding; this does not convert AI to
// Player or authorize Player1090/1094 effects. Nonfinite/subnormal reached
// arithmetic, negative dt and non-nearest host rounding reject. Atomic rejection
// is a native policy, not original rollback semantics. Compile strict binary32,
// no contraction/fast math. Live disjoint backed records are preconditions.
inline RaceCameraControlReceipt advanceStableOriginalRaceCamera(
    OriginalRaceCameraOwner& owner, OriginalCameraShakeChannels& childShake,
    const StableRaceCameraFrame& frame, GameplayBlurPending& pending) noexcept {
    namespace d = original_race_camera_detail;
    RaceCameraControlReceipt receipt;
    if (!owner.bound || owner.binding.sessionGeneration != frame.sessionGeneration ||
        owner.binding.racerKey != frame.racerKey ||
        owner.binding.mainBodyKey != frame.mainBodyKey) return receipt;
    receipt.result = RaceCameraControlResult::requires_other_original_camera_path;
    if ((owner.mode44 != 0 && owner.mode44 != 5) ||
        frame.racerState40 < 0 || frame.racerState40 > 2 ||
        (frame.raceFlags1B0 & 0x10u)) return receipt;
    const bool special = (frame.raceStateFlags58 & 1u) && frame.racerCD4 == 0;
    receipt.result = RaceCameraControlResult::outside_native_numeric_domain;
    if (std::fegetround() != FE_TONEAREST || !d::scalar(frame.elapsed) ||
        frame.elapsed < 0.f || !d::scalar(owner.cooldown8C) ||
        !d::scalar(owner.cutRemaining90)) return receipt;
    bool skipShakeTail = false;
    if (special) {
        if (!d::scalar(frame.raceTime94)) return receipt;
        if (frame.raceTime94 > 0.f) {
            receipt.result = RaceCameraControlResult::requires_other_original_camera_path;
            return receipt;
        }
        skipShakeTail = true; //17606C jumps directly to176D9C.
    }
    auto next = owner;
    auto shake = childShake;
    auto requests = pending;
    bool attemptedBlur = false;
    std::int32_t stagedStrength = 0;
    if (!d::advanceCommonPrefix175D70(next,shake,requests,frame.elapsed,
        attemptedBlur,stagedStrength)) return receipt;
    //175F50..17601C: state0 clears both bits, state1 sets bit3 only,
    //state2 sets bit4 only. Retirement does not alias or clear these fields.
    const auto stateBits = frame.racerState40 == 1 ? 8u :
        frame.racerState40 == 2 ? 16u : 0u;
    next.flags63Low5 = static_cast<std::uint8_t>((next.flags63Low5 & ~0x18u) | stateBits);
    if (!skipShakeTail) {
        if (!d::scalar(next.shakeDelayA0)) return receipt;
        if (next.shakeDelayA0 <= 0.f) {
            if (!d::scalar(next.shakeRemaining94)) return receipt;
            if (next.shakeRemaining94 >= 0.f) {
                if (!d::scalar(next.shakeMagnitude9C)) return receipt;
                float factor = 100.f;
                if (next.shakeDecayingA9) {
                    if (!d::scalar(next.shakeDuration98) || next.shakeDuration98 == 0.f)
                        return receipt;
                    factor = d::div(next.shakeRemaining94,next.shakeDuration98);
                    if (!d::scalar(factor)) return receipt;
                }
                if (next.mode44 == 0) {
                    shake.followMagnitude174 = next.shakeMagnitude9C;
                    shake.followFactor170 = factor;
                } else {
                    const float magnitude = d::div(next.shakeMagnitude9C,2.5f);
                    if (!d::scalar(magnitude)) return receipt;
                    shake.attachedMagnitude2C = magnitude;
                    shake.attachedFactor28 = factor;
                }
            }
        }
    }
    next.flags62 = static_cast<std::uint8_t>(next.flags62 & ~0x80u);
    owner = next;
    childShake = shake;
    pending = requests;
    receipt.requestedBlur = attemptedBlur;
    receipt.requestedStrength = stagedStrength;
    receipt.skippedShakeTail = skipShakeTail;
    receipt.result = RaceCameraControlResult::advanced_needs_follow_pose;
    return receipt;
}

} // namespace rr::game
