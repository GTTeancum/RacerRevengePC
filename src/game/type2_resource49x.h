#pragma once
#include "body_scene48g.h"
#include "spring_records48n.h"
#include "../assets/collision_mesh48i.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
constexpr std::uint16_t type2Leaf49X = 0x8000u;
// Original36-byte runtime node, including radius from the PRE-final-quantized
// adjusted half vector. Children retain the original high-bit/u15 encoding.
struct Type2Node49X {
    float volume=0, radius=0;
    std::uint16_t children[2]{};
    std::array<float,3> center{}, half{};
};
struct Type2Ray49X { SpringVector4_48N first{}, second{}; };
// The file contains only XYZ. These are mandatory opaque native input words;
// neither initialization nor historical heap values are inferred from COL.
struct Type2OpaqueEndpointWords49X { std::uint32_t first, second; };
struct Type2ResourceView49X {
    BodyMatrix48G localMatrix{};
    const Type2Node49X* nodes=nullptr;
    std::size_t nodeCount=0;
    const Type2Ray49X* rays=nullptr;
    std::size_t rayCount=0;
    std::uint16_t root=0;
    std::size_t maxDepth=0; // maximum number of edges from root to a leaf
};
struct Type2ResourceWorkspace49X {
    std::uint32_t* topology=nullptr;
    std::size_t topologyCapacity=0; // at least nodeCount+leafCount elements
};
enum class Type2ResourceStatus49X { Complete, InvalidInput, InsufficientCapacity };
// Cold, allocation-free COL1/type2 decoder. Source:126640..126E34/1137D0.
// Accepts exact-sized resources with node0 root and every node/leaf reached
// exactly once. Retains matrix/volume/endpoints; center/half use source signed
// quantization and asymmetric +.5/truncation. Source radii use scalar RSQRT
// then reciprocal, NOT sqrt or final-half length. Requires FE_TONEAREST and
// separately rounded IEEE binary32 arithmetic (-ffp-contract=off).
// Every output and all destination tails are unchanged on errors. Scratch may
// change after disjointness admission. FULL declared writable capacities must
// be mutually disjoint and disjoint from all inputs and descriptors, including
// unused tails. Input ranges may overlap each other. No in-place conversion.
Type2ResourceStatus49X decodeType2Resource49X(const std::uint8_t* bytes,
    std::size_t byteCount,const Type2OpaqueEndpointWords49X* opaque,
    std::size_t opaqueCount,Type2Node49X* nodes,std::size_t nodeCapacity,
    Type2Ray49X* rays,std::size_t rayCapacity,const Type2ResourceWorkspace49X&,
    Type2ResourceView49X& output) noexcept;
// Validates a caller-constructed immutable view. Its declared maxDepth must
// match the recovered topology; maxDepthOut changes only on Complete. The
// query consumer may trust decode/validation only while all backing data and
// view metadata remain immutable. This does not authenticate asset identity.
Type2ResourceStatus49X validateType2Resource49X(const Type2ResourceView49X&,
    const Type2ResourceWorkspace49X&,std::size_t& maxDepthOut) noexcept;
// Cold radius adapter for the existing immutable type0 mesh. Validates finite
// affine matrix, reverse0, vertex/optional-normal/face fields and complete tree.
// Source1261B0..126200 uses authored half unchanged. radii and maxDepth are
// committed only after complete validation; full declared extents participate
// in the same disjointness contract. Workspace needs nodes.size()+faces.size().
Type2ResourceStatus49X prepareStaticTargetRadii49X(const assets::CollisionMesh48I&,
    float* radii,std::size_t radiusCapacity,const Type2ResourceWorkspace49X&,
    std::size_t& maxDepth) noexcept;
// Private reached type0 payload math. This reuses the retained bounded native
// scalar-EE projection, not whole-mesh validation or a source-data decoder.
// Requires FE_TONEAREST, separately rounded binary32 (-ffp-contract=off),
// finite normal-or-zero input words. Signed halves are accepted. Subnormal
// inputs/nonfinite intermediates are explicit unsupported native boundaries.
// Outputs remain unchanged on refusal; caller owns input knownness and PCs.
class SourceCollisionPayloadMath50LA final {
    friend class OriginalWreckType0Owners50LA;
    static bool radius1261B0(const std::array<std::uint32_t,3>&,std::uint32_t&)noexcept;
    static bool faceLane126320(std::uint32_t,std::uint16_t&)noexcept;
};

}
