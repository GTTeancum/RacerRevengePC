#pragma once
#include "world_asset48f.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace rr { namespace assets {
// In-memory projection only. RRWORLD1 remains the authoritative cache format.
// Resource references are opaque authored metadata, never availability claims.
template<class T> struct WorldIndexSpan49B {
    T* data = nullptr;
    std::size_t size = 0;
    T* begin() const noexcept { return data; }
    T* end() const noexcept { return size ? data + size : data; }
    bool empty() const noexcept { return size == 0; }
    T& operator[](std::size_t i) const noexcept { return data[i]; }
};
struct WorldZoneRecord49B {
    std::uint32_t zone = 0; // Authored, one-based identity.
    std::uint32_t first_placement = 0, placement_count = 0;
    std::uint32_t first_incident_portal = 0, incident_portal_count = 0;
};
struct WorldPlacement49B {
    std::array<double, 3> position{}, look{}, up{};
    std::array<double, 16> matrix{};
    std::uint32_t zone = 0, ordinal = 0; // Authored, one-based identities.
    std::uint32_t resource_pair = 0; // Zero-based ID, first-appearance order.
    std::uint32_t use_lighting = 0; // Exact authored bit; no shading policy.
};
struct WorldPortalRecord49B {
    std::array<std::array<double, 3>, 4> vertices{};
    std::uint32_t ordinal = 0; // Authored portalN identity; ID is ordinal - 1.
    std::array<std::uint32_t, 2> zones{}; // Preserve authored endpoint order.
    std::uint32_t reserved = 0; // Explicit zero padding; identical 32/64-bit layout.
};
struct WorldResourcePair49B {
    std::uint32_t geometry_offset = 0, geometry_bytes = 0;
    std::uint32_t collision_offset = 0, collision_bytes = 0;
};
struct WorldResourceText49B {
    std::string_view geometry_member, collision_member; // Not NUL-terminated.
};
struct WorldIndexRequirements49B {
    std::size_t zones = 0, placements = 0, portals = 0, resource_pairs = 0;
    std::size_t text_bytes = 0, incident_portal_ids = 0;
    std::size_t payload_bytes = 0; // sizeof(record) * count plus raw text/IDs.
};
struct WorldIndexLimits49B {
    WorldLimits48F world{}; // Only reduces the existing 48F hard ceilings.
    std::size_t asset_bytes = max_world_asset_bytes48f;
    std::uint32_t resource_pairs = 100000;
    std::size_t text_bytes = 32u * 1024u * 1024u;
    std::size_t payload_bytes = 64u * 1024u * 1024u;
};
// Optional load-time audit data, not part of the resident projection. Indices
// match the resident arrays. Unknown fields remain in the original RRWORLD1;
// callers retaining a source snapshot must keep that original cache unchanged.
struct WorldIndexProvenance49B {
    std::uint32_t scene_info_block = 0;
    std::vector<std::uint32_t> zone_parameter_blocks, portal_parameter_blocks;
    std::vector<std::uint32_t> placement_parameter_blocks, placement_source_block_offsets;
};
struct WorldIndexStorage49B {
    WorldIndexSpan49B<WorldZoneRecord49B> zones;
    WorldIndexSpan49B<WorldPlacement49B> placements;
    WorldIndexSpan49B<WorldPortalRecord49B> portals;
    WorldIndexSpan49B<WorldResourcePair49B> resource_pairs;
    WorldIndexSpan49B<char> text;
    WorldIndexSpan49B<std::uint32_t> incident_portal_ids;
};
struct WorldIndexAccess49B;
class WorldIndexView49B {
public:
    WorldIndexView49B() noexcept = default;
    WorldIndexSpan49B<const WorldZoneRecord49B> zones() const noexcept { return zones_; }
    WorldIndexSpan49B<const WorldPlacement49B> placements() const noexcept { return placements_; }
    WorldIndexSpan49B<const WorldPortalRecord49B> portals() const noexcept { return portals_; }
    WorldIndexSpan49B<const WorldResourcePair49B> resourcePairs() const noexcept { return resources_; }
    WorldIndexSpan49B<const char> text() const noexcept { return text_; }
    WorldIndexSpan49B<const std::uint32_t> incidentPortalIds() const noexcept { return incidents_; }
    WorldIndexRequirements49B requirements() const noexcept;
    const WorldZoneRecord49B* zone(std::uint32_t zoneNumber) const noexcept;
    const WorldPlacement49B* placement(std::uint32_t zoneNumber, std::uint32_t ordinal) const noexcept;
    const WorldPlacement49B* placementById(std::uint32_t id) const noexcept;
    const WorldPortalRecord49B* portalById(std::uint32_t id) const noexcept;
    WorldIndexSpan49B<const WorldPlacement49B> zonePlacements(std::uint32_t zoneNumber) const noexcept;
    WorldIndexSpan49B<const std::uint32_t> zoneIncidentPortals(std::uint32_t zoneNumber) const noexcept;
    bool resourceText(std::uint32_t pairId, WorldResourceText49B& output) const noexcept;
private:
    friend struct WorldIndexAccess49B;
    WorldIndexSpan49B<const WorldZoneRecord49B> zones_;
    WorldIndexSpan49B<const WorldPlacement49B> placements_;
    WorldIndexSpan49B<const WorldPortalRecord49B> portals_;
    WorldIndexSpan49B<const WorldResourcePair49B> resources_;
    WorldIndexSpan49B<const char> text_;
    WorldIndexSpan49B<const std::uint32_t> incidents_;
};
class OwnedWorldIndex49B {
public:
    // Only const views escape. Replacement/destruction invalidates old views;
    // copied owners are independent. No pointers into World48F or input bytes.
    WorldIndexView49B view() const noexcept;
    std::size_t allocatedPayloadBytes() const noexcept; // Vector capacities, not allocator overhead.
    void swap(OwnedWorldIndex49B& other) noexcept;
private:
    friend struct WorldIndexAccess49B;
    std::vector<WorldZoneRecord49B> zones_;
    std::vector<WorldPlacement49B> placements_;
    std::vector<WorldPortalRecord49B> portals_;
    std::vector<WorldResourcePair49B> resources_;
    std::vector<char> text_;
    std::vector<std::uint32_t> incidents_;
};
// Load-time only, serial and transactional: output and optional provenance are
// unchanged on failure (error is the diagnostic output). Allocations include
// validation, resource interning, staging, and retained vector storage.
// Direct build reuses encodeWorldAsset48F's complete validator and discards its
// temporary RRWORLD1 bytes; load reuses parseWorldAsset48F and does not reencode.
bool buildWorldIndex49B(const World48F& source, OwnedWorldIndex49B& output,
    std::string& error, WorldIndexProvenance49B* provenance = nullptr,
    const WorldIndexLimits49B& limits = WorldIndexLimits49B{});
bool loadWorldIndex49B(const std::vector<std::uint8_t>& rrworld, OwnedWorldIndex49B& output,
    std::string& error, WorldIndexProvenance49B* provenance = nullptr,
    const WorldIndexLimits49B& limits = WorldIndexLimits49B{});

enum class WorldIndexCopyError49B {
    none, invalid_source, insufficient_storage, null_storage,
    invalid_extent, misaligned_storage, overlapping_storage
};
// Explicit caller-owned build/copy path. Query, requirements, view creation,
// and this copy never allocate and never call the importer. Copy is bounded,
// serial, and checks everything before writing: destination bytes and output
// view are unchanged on failure. A default/empty source is invalid.
//
// Each nonempty span must name live, correctly typed contiguous objects for its
// entire stated capacity. All six destination capacity ranges must be mutually
// disjoint, and disjoint from every source range, source view object, storage
// descriptor, output view object and error object. Source arrays must likewise
// not overlap control objects. The output view and error must be separate
// objects, and error must not alias source or destination/control memory (the
// ordinary C++ reference validity precondition). In-place copies are rejected.
// Pointers must be process-local ordinary object pointers representable by
// uintptr_t. This is not an arbitrary-address or foreign-memory interface.
//
// After success, do not mutate/reallocate/free caller storage while using its
// borrowed view or resource text. Source owner/world/bytes may then be freed.
// Invalid IDs return nullptr/empty spans; resourceText leaves output unchanged.
bool copyWorldIndex49B(const WorldIndexView49B& source, const WorldIndexStorage49B& storage,
    WorldIndexView49B& output, WorldIndexCopyError49B& error) noexcept;
} }
