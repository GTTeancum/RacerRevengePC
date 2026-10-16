#pragma once
#include "../../src/game/original_camera_start_effects.h"
#include <array>
#include <cstddef>
#include <cstdint>
namespace ra_static_fog50LA {
// Copied values, not renderer admission. An opaque private Binding owns the
// actual lifetime/revision checks; this public value cannot mint that binding.
struct CameraObservation {
    std::uint64_t generation=0,publication=0,frame=0,fogRevision=0;
    rr::game::OriginalRaceFogFields source;
    rr::game::CameraViewOutput49F camera;
    std::array<float,3> position{};
    std::array<float,16> screen{},guardProjection{};
};
struct PlacementObservation {
    // Native binary32 candidate only: compiler contraction, FTZ/DAZ and PS2
    // reciprocal/square-root differences are not certified by publication.
    std::size_t placement=0,worldInstance=0;
    std::array<float,3> center140{},localTranslation{};
    std::array<float,16> packetModel{};
    float radius194=0,positionScale=0,distanceCandidate=0;
    bool computeFogCandidate02=false,worldSpace=false;
    // No actual240C00 selected191/membership or26F070 callback was dispatched.
    // The values above are native evaluations, not source model field writes.
    static constexpr bool originalSelectedSchedulingJoined=false;
    static constexpr bool originalFogBit02Published=false;
};
}
