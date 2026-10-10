#pragma once
#include "../assets/tv_camera49e.h"
#include <cstdint>

namespace rr::game {
// Numerical fields read/written by original2D30D0; names retain source offsets.
// This is not the full camera object or an ownership wrapper for its filter.
// Known base/TV initializer fields match their original values. The hold/filter
// zero defaults are native placeholders, not proof of their enclosing setup;
// the initial negative-current selection overwrites both before consuming them.
struct TVCameraSelectorState49E {
    std::int32_t current40=-1,previous44=0,stationary80=0;
    float hold90=0;
    assets::TVCameraVector49E positionA0;
    float fieldC0=0,fieldC4=0,fieldC8=0;
    std::uint32_t wordCC=0;
    std::int32_t sourceD0=-1;
    float filterCurrent00=0;
};
// Caller-owned counterpart of the original C-library random state. Initial
// static ELF state is1. Sharing/seeding order remains a scheduler responsibility.
struct TVCameraRandom49E {std::uint32_t raw=1;};
void seedTVCameraRandom49E(TVCameraRandom49E&,std::uint32_t seed) noexcept;
std::uint32_t nextTVCameraRandom49E(TVCameraRandom49E&) noexcept;

struct TVCameraSelection49E {
    assets::TVCameraVector49E position;
    bool changed=false;
};
// Original2D30B0 resets exactly current40=0 and hold90=bits3F19999A. It does not
// choose nearest, reset previous/filter/other fields, or consume random state.
void resetTVCameraSelector49E(TVCameraSelectorState49E&) noexcept;

// Original2D30D0..2D33FC, with closed308030 random leaf. A negative current
// searches all cameras; otherwise only its ordered links compete with current.
// A current-vs-link tie chooses the link, while link-order ties retain the first.
// If desired differs and pre-decrement hold>0, subtract elapsed but do not switch
// that call. Switching resets hold to0.6; unchanged desired preserves hold.
// Each successful call copies the selected position. Changes alone reset the
// five source fields, set filterCurrent00 to MaxFOV and consume one random word
// to compute the exact strict Stationary test. SwitchEvery is NOT consumed here.
//
// Native admission: loaded nonempty view, current negative or valid, finite
// player XYZ and elapsed; every reached camera XYZ/arithmetic result finite and
// squared distance strictly below FLT_MAX. Only consumed hold/MaxFOV/Stationary
// values are checked. Negative elapsed is not silently clamped. Position W is
// copied bitwise and is not used for distance. Unsupported data leaves state,
// random and output unchanged (stronger native failure contract than original).
// Live disjoint typed view/state/random/output objects are caller preconditions.
// All code is serial, bounded by the loaded data, and allocation-free.
//
// Numeric proof uses strict host binary32 instruction steps: dx²,dy², their
// rounded accumulator sum, then rounded dz² plus accumulator. This is not proof
// of every PS2 silicon MADD/denormal/exceptional-value detail. No camera matrix,
// full2D3400 update, world queries, transport or scheduler is supplied here.
bool selectTVCamera49E(const assets::TVCameraView49E&,TVCameraSelectorState49E&,
    const assets::TVCameraVector49E& playerPosition,float elapsed,
    TVCameraRandom49E&,TVCameraSelection49E& output) noexcept;
}
