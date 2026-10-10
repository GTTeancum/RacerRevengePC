#pragma once
#include "import_decode48c.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rr { namespace assets {
constexpr std::uint32_t collision_leaf48g = 0x80000000u;
constexpr std::size_t max_collision_asset_bytes48g = 16u * 1024u * 1024u;
struct CollisionLimits48G {
    std::size_t input_bytes = 8u * 1024u * 1024u;
    std::uint32_t vertices = 65535, nodes = 32767, faces = 32768;
};
struct CollisionNode48G {
    float volume = 0;
    std::array<float, 3> center{}, half{};
    // Native tags: high bit denotes a face; otherwise a node index. Root is node 0.
    std::array<std::uint32_t, 2> children{};
    std::uint32_t source_offset = 0;
};
struct CollisionFace48G {
    // Authored equation dot(normal, local_position) + d = 0. Never repaired.
    // Original PS2 loader subsequently quantizes normals; these remain source precision.
    float d = 0;
    std::array<float, 3> normal{};
    std::array<std::uint32_t, 3> vertices{};
    std::uint32_t source_offset = 0;
};
struct Collision48G {
    std::uint32_t version = 1, type = 0, reverse_mode = 0, quantized = 0, normals_present = 0;
    std::array<float, 16> matrix{}; // Row-vector affine COL local transform.
    std::uint32_t vertices_offset = 0, normals_flag_offset = 0, nodes_offset = 0, faces_offset = 0;
    // quantized==1 positions retain the original query-prefix projection:
    // signed16 -> binary32 -> *bits38000100 -> *32767, rounded per operation.
    // Original shorts remain in original_bytes and are checked on serialization.
    // Quantized optional normals are still unsupported; float normals unchanged.
    std::vector<std::array<float, 3>> vertices, vertex_normals;
    std::vector<CollisionNode48G> nodes;
    std::vector<CollisionFace48G> faces;
    std::vector<std::uint8_t> original_bytes;
    // COL v1 contains no material IDs or per-face flags. No defaults are fabricated.
};
bool decodeCollision48G(const std::vector<std::uint8_t>& input, Collision48G& output,
                       DecodeError48C& error, const DecodeHooks48C* hooks = nullptr,
                       const CollisionLimits48G& limits = {});
// Original126450: ordered scene index followed by a headerless125890 tree.
// The tree is a typed body, NOT a standalone original COL0 file: its offsets
// are absolute in the compound source, and tree.original_bytes stays empty.
// The existing COL0/RRCOL001 serializers intentionally do not admit these bodies.
struct CollisionCompoundMember50IJ {
    std::uint32_t scene_node_index=0, source_offset=0, body_offset=0, source_end=0;
    Collision48G tree;
};
struct CollisionCompound50IJ {
    std::uint32_t version=1,type=1,reverse_mode=0;
    std::vector<CollisionCompoundMember50IJ> members;
    std::vector<std::uint8_t> original_bytes;
};
struct CollisionCompoundLimits50IJ {
    CollisionLimits48G tree{};
    std::uint32_t members=80; // caller policy; absolute implementation ceiling1024
};
// Direct original-byte cold decode. Full original provenance is retained once;
// member ordering, indices, tree topology and authored planes are never repaired.
// Failure leaves output unchanged. No guest execution or generated geometry.
bool decodeCollisionCompound50IJ(const std::vector<std::uint8_t>& input,
    CollisionCompound50IJ& output, DecodeError48C& error,
    const DecodeHooks48C* hooks=nullptr, const CollisionCompoundLimits50IJ& limits={});
// Canonical source serializer checks typed fields, source offsets, and complete raw provenance.
bool encodeOriginalCollision48G(const Collision48G& collision, std::vector<std::uint8_t>& output,
                               std::string& error);
// Native RRCOL001 cache: explicit LE typed records, u32 indices/tags, complete provenance.
// Parsing this cache never invokes the original COL decoder.
bool encodeCollisionAsset48G(const Collision48G& collision, std::vector<std::uint8_t>& output,
                            std::string& error);
bool parseCollisionAsset48G(const std::vector<std::uint8_t>& input, Collision48G& output,
                           std::string& error);
// Collision creator 27B178 differs from geometry: normalize Up and Look independently,
// then right = Up x Look. Scalar double policy, not PS2 VU rounding equivalence.
bool collisionPlacement48G(const std::array<double, 3>& position,
                          const std::array<double, 3>& look, const std::array<double, 3>& up,
                          std::array<double, 16>& output, std::string& error);
// Original 11F680 type0 composes COL local x placement (row-vector convention).
bool collisionWorldMatrix48G(const Collision48G& collision,
                             const std::array<double, 16>& placement,
                             std::array<double, 16>& output, std::string& error);
} }
