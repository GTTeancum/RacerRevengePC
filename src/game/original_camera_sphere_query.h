#pragma once
#include "../assets/collision_mesh48i.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Typed projection of the camera shape5's explicit world-space center and its
// stored radius50/radiusSquared54. The caller performs source shape preparation.
struct OriginalCameraSphereQuery {
    std::array<float,3> worldCenter{};
    float radius50=0.f,radiusSquared54=0.f;
};
// Camera11C0B0 owns a result container of capacity3. Selection108E90 stores at
// most one contact. Preserve count/distance across candidates within one query;
// reset only at the original121920 query boundary. XYZ are the live consumers.
struct OriginalCameraSphereContact {
    std::uint32_t count=0;
    float planeDistance=0.f;
    std::array<float,3> worldCenter{},worldNormal{},worldPoint{};
    std::uint32_t face=0; // native receipt, not an original stored field
};
struct OriginalCameraSphereWorkspace {
    std::uint32_t* tags=nullptr;
    std::size_t capacity=0;
};
enum class OriginalCameraSphereStatus {
    Hit,Miss,InvalidInput,UnsupportedVertexStorage,UnsupportedArithmetic,
    InsufficientWorkspace
};
// Source124270 shape0 ->10A4E0(selection1) ->108E90 ->109F10, followed by
// 124318..1243D4's world transform. No registration, collection, mask filtering,
// callback, type1 compound coarse gate or implicit owner admission occurs here.
// mesh must be the decoded original float-vertex (reverse_mode0) rooted tree;
// world must be its actual prepared source matrix, not a general inverse.
// Caller-owned workspace is reusable and may change on any return; it must be
// disjoint from all inputs/output. Other outputs change only on Hit. prior and
// output may be the same object. No allocation, retained pointer or guest code.
// FE_TONEAREST, ordinary finite affine binary32/no contraction domain. Singular
// edge division/nonfinite arithmetic is surfaced, never converted into a miss.
OriginalCameraSphereStatus queryOriginalCameraSphereMesh(
    const OriginalCameraSphereQuery& query,const assets::CollisionMesh48I& mesh,
    const std::array<float,16>& world,const OriginalCameraSphereContact& prior,
    const OriginalCameraSphereWorkspace& workspace,
    OriginalCameraSphereContact& output) noexcept;
}

namespace rr::game {
struct CompoundCollisionView50IJ;
// Source1243E4..124544: actual owner-local coarse box, then ordered members.
// Member active flags are not read. Stop on the first member returning Hit;
// prior selection remains authoritative. The complete shape0 contract applies.
OriginalCameraSphereStatus queryOriginalCameraSphereCompound(
    const OriginalCameraSphereQuery&,const CompoundCollisionView50IJ&,
    const OriginalCameraSphereContact&,const OriginalCameraSphereWorkspace&,
    OriginalCameraSphereContact&) noexcept;
}
