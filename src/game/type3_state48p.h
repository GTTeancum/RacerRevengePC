#pragma once
#include "body_scene48g.h"
#include "spring_records48n.h"

namespace rr::game {
// Native structure-of-arrays state. Candidate-local scratch has only XYZ;
// its original unused W lanes depend on vector-register history and are not
// represented as guest-register state. All copied game-facing Vec4 lanes remain.
struct Type3RaySource48P {
    const Vector3_48C* localStarts=nullptr;
    const Vector3_48C* localDirections=nullptr;
    const SpringVector4_48N* worldStarts=nullptr;
    const SpringVector4_48N* worldDirections=nullptr;
    const SpringVector4_48N* normals=nullptr;
    const SpringVector4_48N* points=nullptr;
    const std::uint32_t* hits=nullptr;
    const float* fractions=nullptr;
    std::size_t capacity=0;
    const float* lengths=nullptr;std::size_t lengthCapacity=0;
};
struct Type3RayBuffers48P {
    Vector3_48C* localStarts=nullptr;
    Vector3_48C* localDirections=nullptr;
    SpringVector4_48N* worldStarts=nullptr;
    SpringVector4_48N* worldDirections=nullptr;
    SpringVector4_48N* normals=nullptr;
    SpringVector4_48N* points=nullptr;
    std::uint32_t* hits=nullptr;
    float* fractions=nullptr;
    std::size_t capacity=0;
    float* lengths=nullptr;std::size_t lengthCapacity=0;
};
struct Type3QueryState48P {
    Vector3_48C localCenter{};
    SpringVector4_48N worldCenter{},queryCenter{};
    float radius=0,queryRadius=0;
    SpringVector4_48N result00{},result10{},selectedNormal{},selectedPoint{},selectedCorrection{};
    std::uint32_t hitCount=0,opaque78=0,opaque7c=0;
    float selectedFraction=1.f;
};
}
