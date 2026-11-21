#pragma once
// The one hand-off from a trigger to the race cameras' shake (original Revenge
// CameraShakeTriggerAction: 1B5990 fields, enter 1B2780 -> 1ACEA0 -> 177720, which writes
// the shake timer/magnitude/decaying/delay of every registered race camera; exit has no
// effect). Used by the common trigger actions (course_trigger_actions50ct.h, ActionKind::
// CameraShake) and by the N64 AnimationPlay hazard's Shake keys (PORT EXTENSION, N64 data).
//
// PORT STAND-IN (flagged): the original writes the camera owners inside the racer's trigger
// query; the port's query cannot reach the camera session, so the enter is recorded here and
// applied by the world step of the same frame (before the camera update) through the camera
// session's own applyAuthoredShakeEnter. Several enters before that step: the last one wins,
// as the original's overwrite of the same camera fields. Host memory only.
#include "original_race_camera_owner.h"
#include <cstdint>

namespace rr::game {
struct CameraShakeRelay50CS {
    // Bound by the race session once its camera session exists; unbound before it goes.
    void* context = nullptr;
    bool (*apply)(void*, const OriginalCameraShakeAction&) noexcept = nullptr;
    OriginalCameraShakeAction pending{};
    bool hasPending = false;
    std::uint64_t requested = 0, applied = 0;

    void bind(void* c, bool (*f)(void*, const OriginalCameraShakeAction&) noexcept) noexcept { context = c; apply = f; }
    void unbind() noexcept { context = nullptr; apply = nullptr; hasPending = false; }
    void request(const OriginalCameraShakeAction& action) noexcept { pending = action; hasPending = true; ++requested; }
    // Once per world step. A refused apply keeps the request for the next step.
    void flush() noexcept {
        if (!hasPending || !apply || !context) return;
        if (apply(context, pending)) { hasPending = false; ++applied; }
    }
    void raceReset() noexcept { hasPending = false; }
};
inline CameraShakeRelay50CS cameraShakeRelay50CS;
} // namespace rr::game
