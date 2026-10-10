#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rr { namespace assets {
enum class DecodeKind48C { none, malformed, unsupported, limit, cancelled };
struct DecodeError48C {
    DecodeKind48C kind = DecodeKind48C::none;
    std::string code;
    std::size_t offset = 0;
    std::string detail;
};
struct DecodeLimits48C {
    std::size_t resource_file_bytes = 64u * 1024u * 1024u;
    std::size_t resource_decoded_bytes = 128u * 1024u * 1024u;
    std::uint32_t resource_entries = 10000;
    std::uint32_t resource_name_bytes = 1024;
    std::size_t mesh_file_bytes = 8u * 1024u * 1024u;
};
// Optional synchronous callbacks on the caller's thread. They must not throw,
// mutate input/output vectors, or reenter this decode. No platform/UI is called.
struct DecodeHooks48C {
    void* context = nullptr;
    bool (*cancelled)(void*) = nullptr;
    void (*progress)(void*, const char* stage, std::uint32_t completed, std::uint32_t total) = nullptr;
};
struct ResourceEntry48C {
    std::string name;
    std::uint32_t logical_offset = 0;
    std::uint32_t bytes = 0;
};
struct ResourceBlock48C {
    std::uint32_t compressed_offset = 0;
    std::uint32_t compressed_bytes = 0;
};
struct ResArchive48C {
    std::vector<ResourceEntry48C> entries;
    std::vector<ResourceBlock48C> blocks;
    std::vector<std::uint8_t> decoded;
};
struct PsgMesh48C {
    std::vector<std::uint8_t> rrmesh;
    std::string object_name, material;
    bool world_space = false;
    std::uint32_t vertices = 0, triangles = 0, strips = 0, groups = 0;
    std::uint32_t positive_normal_alignment = 0, degenerate_normal_alignment = 0;
    std::array<double, 3> bounds_min{}, bounds_max{};
    std::array<double, 7> quantization{};
    double max_bounds_error = 0;
    double uv_size_uninterpreted = 0;
};

// Frozen 48B subsets. Outputs remain unchanged on failure or cancellation.
// Limits can reduce, never increase, the fixed hard ceilings shown above.
// Resource names are archive metadata, not safe filesystem paths. The caller
// must not write members under untrusted names without its own path policy.
bool decodeResource48C(const std::vector<std::uint8_t>& input, ResArchive48C& output,
    DecodeError48C& error, const DecodeHooks48C* hooks = nullptr,
    const DecodeLimits48C& limits = DecodeLimits48C{});
bool extractResMember48C(const ResArchive48C& archive, const std::string& name,
    std::vector<std::uint8_t>& output, DecodeError48C& error,
    std::size_t maxBytes = 8u * 1024u * 1024u);
bool decodeMesh48C(const std::vector<std::uint8_t>& input, PsgMesh48C& output,
    DecodeError48C& error, const DecodeHooks48C* hooks = nullptr,
    const DecodeLimits48C& limits = DecodeLimits48C{});
} }
