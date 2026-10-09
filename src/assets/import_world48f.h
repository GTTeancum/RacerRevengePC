#pragma once
#include "import_decode48c.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rr { namespace assets {
struct WorldLimits48F {
    std::size_t input_bytes = 2u * 1024u * 1024u;
    std::uint32_t blocks = 8192, fields_per_block = 1024;
    std::uint32_t zones = 256, portals = 4096, instances_per_zone = 10000, instances = 100000;
    // Representation-only opt-in: preserve duplicate sorted labels, without
    // selecting a winning value. Each consumer must reject duplicates it uses.
    // World semantic decoding never accepts this opt-in.
    bool retainDuplicateLabels = false;
};
struct ParameterField48F {
    std::string name, value;
    std::size_t entry_offset = 0, value_offset = 0;
    std::uint32_t value_bytes = 0;
};
struct ParameterBlock48F {
    std::string name;
    std::size_t name_offset = 0, table_entry_offset = 0, offset = 0;
    std::uint32_t size_without_count = 0;
    std::vector<ParameterField48F> fields; // Original sorted label-table order.
};
struct ParameterDocument48F {
    std::uint32_t version = 1;
    std::size_t header_table_offset = 0;
    std::vector<ParameterBlock48F> blocks; // Original sorted block order.
    std::vector<std::uint8_t> original_bytes; // Exact file including all raw blocks.
    bool preservesDuplicateLabels = false;
};
struct WorldZone48F {
    std::uint32_t zone = 0, models = 0, parameter_block = 0;
};
struct WorldPortal48F {
    std::string section;
    std::uint32_t parameter_block = 0;
    std::array<std::uint32_t, 2> zones{};
    std::array<std::array<double, 3>, 4> vertices{};
};
struct WorldInstance48F {
    std::string section, geometry_member, collision_member;
    std::uint32_t zone = 0, ordinal = 0, parameter_block = 0;
    std::size_t source_block_offset = 0;
    std::array<double, 3> position{}, look{}, up{}; // Decimal -> binary64 -> binary32 -> binary64.
    std::array<double, 16> matrix{}; // Row-vector placement; replaces saved PSG root transform.
    bool use_lighting = false; // Original flag; not a shader policy.
};
struct World48F {
    ParameterDocument48F parameters; // Includes every interpreted/uninterpreted block and field.
    std::uint32_t scene_info_block = 0;
    std::vector<WorldZone48F> zones;
    std::vector<WorldPortal48F> portals;
    std::vector<WorldInstance48F> instances;
};

// Portable C++17, transactional on error/cancel, no files/JSON/guest execution.
// Limits only reduce hard ceilings. Parameter decode supports the frozen48E
// BINARY1 labelled-block subset and verifies exact reversible serialization.
bool decodeParameters48F(const std::vector<std::uint8_t>& input, ParameterDocument48F& output,
    DecodeError48C& error, const DecodeHooks48C* hooks = nullptr,
    const WorldLimits48F& limits = WorldLimits48F{});
bool encodeParameters48F(const ParameterDocument48F& parameters, std::vector<std::uint8_t>& output,
    DecodeError48C& error);
bool decodeWorld48F(const std::vector<std::uint8_t>& input, World48F& output,
    DecodeError48C& error, const DecodeHooks48C* hooks = nullptr,
    const WorldLimits48F& limits = WorldLimits48F{});
// Reconcile cached typed semantics with retained parsed fields without reading
// the original binary format. Used by the neutral cache's invariant checks.
bool validateWorldSemantics48F(const World48F& world, DecodeError48C& error);
// Interpreted objects reference original fields through parameter_block indices.
// Matrix arithmetic matches frozen native scalar semantics, not bit-exact VU
// RSQRT. No visibility, geometry resolution, root-bind composition, collisions,
// animations, or material/lighting behavior is invented by this decoder.
} }
