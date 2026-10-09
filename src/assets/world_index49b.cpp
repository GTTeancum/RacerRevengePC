#include "world_index49b.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <map>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace rr { namespace assets {
static_assert(sizeof(double) == 8 && std::numeric_limits<double>::is_iec559,
    "World index requires IEEE binary64");
static_assert(sizeof(WorldPlacement49B) == 216, "Unexpected placement layout");
static_assert(sizeof(WorldPortalRecord49B) == 112, "Unexpected portal layout");
static_assert(sizeof(WorldZoneRecord49B) == 20, "Unexpected zone layout");
static_assert(sizeof(WorldResourcePair49B) == 16, "Unexpected reference layout");
static_assert(std::is_trivially_copyable<WorldPlacement49B>::value &&
    std::is_trivially_copyable<WorldPortalRecord49B>::value &&
    std::is_trivially_copyable<WorldZoneRecord49B>::value &&
    std::is_trivially_copyable<WorldResourcePair49B>::value, "Copy requires plain records");

namespace {
constexpr std::size_t maxTextBytes = 32u * 1024u * 1024u;
constexpr std::size_t maxPayloadBytes = 64u * 1024u * 1024u;
void need(bool condition, const char* error) {
    if (!condition) throw std::runtime_error(error);
}
std::size_t payload(const WorldIndexRequirements49B& r) noexcept {
    // Every producer checks fixed hard ceilings before calling this routine.
    return r.zones * sizeof(WorldZoneRecord49B) + r.placements * sizeof(WorldPlacement49B) +
        r.portals * sizeof(WorldPortalRecord49B) + r.resource_pairs * sizeof(WorldResourcePair49B) +
        r.text_bytes + r.incident_portal_ids * sizeof(std::uint32_t);
}
void checkSourceLimits(const World48F& source, const WorldIndexLimits49B& limits) {
    const auto& w = limits.world;
    need(source.parameters.original_bytes.size() <= std::min(w.input_bytes, std::size_t(2u * 1024u * 1024u)), "world_index.source_byte_limit");
    need(source.parameters.blocks.size() <= std::min(w.blocks, 8192u), "world_index.block_limit");
    for (const auto& b : source.parameters.blocks)
        need(b.fields.size() <= std::min(w.fields_per_block, 1024u), "world_index.field_limit");
    need(!source.zones.empty() && source.zones.size() <= std::min(w.zones, 256u), "world_index.zone_limit");
    need(source.portals.size() <= std::min(w.portals, 4096u), "world_index.portal_limit");
    need(source.instances.size() <= std::min(w.instances, 100000u), "world_index.placement_limit");
    for (const auto& z : source.zones)
        need(z.models <= std::min(w.instances_per_zone, 10000u), "world_index.zone_placement_limit");
}
void swapProvenance(WorldIndexProvenance49B& a, WorldIndexProvenance49B& b) noexcept {
    std::swap(a.scene_info_block, b.scene_info_block);
    a.zone_parameter_blocks.swap(b.zone_parameter_blocks);
    a.portal_parameter_blocks.swap(b.portal_parameter_blocks);
    a.placement_parameter_blocks.swap(b.placement_parameter_blocks);
    a.placement_source_block_offsets.swap(b.placement_source_block_offsets);
}
struct AddressRange { std::uintptr_t begin = 0, end = 0; };
bool overlaps(const AddressRange& a, const AddressRange& b) noexcept {
    return a.begin < b.end && b.begin < a.end;
}
template<class T> bool range(WorldIndexSpan49B<T> span, AddressRange& out,
    WorldIndexCopyError49B& error) noexcept {
    if (span.size == 0) { out = {}; return true; }
    if (!span.data) { error = WorldIndexCopyError49B::null_storage; return false; }
    const auto address = reinterpret_cast<std::uintptr_t>(span.data);
    if (address % alignof(T) != 0) { error = WorldIndexCopyError49B::misaligned_storage; return false; }
    if (span.size > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
        error = WorldIndexCopyError49B::invalid_extent; return false;
    }
    const auto bytes = span.size * sizeof(T);
    if (bytes > std::numeric_limits<std::uintptr_t>::max() - address) {
        error = WorldIndexCopyError49B::invalid_extent; return false;
    }
    out = {address, address + bytes}; return true;
}
template<class T> AddressRange objectRange(const T& object) noexcept {
    const auto address = reinterpret_cast<std::uintptr_t>(&object);
    return {address, address + sizeof(T)};
}
bool sourceValid(const WorldIndexView49B& v) noexcept {
    const auto r = v.requirements();
    if (!r.zones || r.zones > 256 || r.placements > 100000 || r.portals > 4096 ||
        r.resource_pairs > r.placements || r.text_bytes > maxTextBytes ||
        r.incident_portal_ids != r.portals * 2 || r.payload_bytes > maxPayloadBytes) return false;
    std::size_t placementNext = 0, incidentNext = 0;
    for (std::size_t i = 0; i < r.zones; ++i) {
        const auto& z = v.zones()[i];
        if (z.zone != i + 1 || z.placement_count > 10000 || z.first_placement != placementNext ||
            z.placement_count > r.placements - placementNext || z.first_incident_portal != incidentNext ||
            z.incident_portal_count > r.incident_portal_ids - incidentNext) return false;
        std::uint32_t previous = 0;
        for (std::size_t j = 0; j < z.incident_portal_count; ++j) {
            const auto id = v.incidentPortalIds()[incidentNext + j];
            if (id >= r.portals || (j && id <= previous)) return false;
            const auto& p = v.portals()[id];
            if (p.zones[0] != z.zone && p.zones[1] != z.zone) return false;
            previous = id;
        }
        for (std::size_t j = 0; j < z.placement_count; ++j) {
            const auto& p = v.placements()[placementNext + j];
            if (p.zone != z.zone || p.ordinal != j + 1 || p.resource_pair >= r.resource_pairs || p.use_lighting > 1) return false;
            for (double x : p.position) if (!std::isfinite(x)) return false;
            for (double x : p.look) if (!std::isfinite(x)) return false;
            for (double x : p.up) if (!std::isfinite(x)) return false;
            for (double x : p.matrix) if (!std::isfinite(x)) return false;
            if (p.matrix[3] != 0 || p.matrix[7] != 0 || p.matrix[11] != 0 || p.matrix[15] != 1) return false;
            for (unsigned k = 0; k < 3; ++k) if (p.matrix[12 + k] != p.position[k]) return false;
        }
        placementNext += z.placement_count; incidentNext += z.incident_portal_count;
    }
    if (placementNext != r.placements || incidentNext != r.incident_portal_ids) return false;
    for (std::size_t i = 0; i < r.portals; ++i) {
        const auto& p = v.portals()[i];
        if (p.reserved || p.ordinal != i + 1 || !p.zones[0] || !p.zones[1] || p.zones[0] > r.zones ||
            p.zones[1] > r.zones || p.zones[0] == p.zones[1]) return false;
        for (const auto& vertex : p.vertices) for (double x : vertex) if (!std::isfinite(x)) return false;
    }
    std::size_t textNext = 0;
    for (const auto& p : v.resourcePairs()) {
        if (p.geometry_offset != textNext || !p.geometry_bytes || p.geometry_bytes > 511 || p.geometry_bytes > r.text_bytes - textNext) return false;
        textNext += p.geometry_bytes;
        if (p.collision_offset != textNext || !p.collision_bytes || p.collision_bytes > 511 || p.collision_bytes > r.text_bytes - textNext) return false;
        textNext += p.collision_bytes;
    }
    if (textNext != r.text_bytes) return false;
    for (unsigned char c : v.text()) if (c < 32 || c >= 127) return false;
    std::size_t nextResource = 0;
    for (const auto& p : v.placements()) {
        if (p.resource_pair > nextResource) return false;
        if (p.resource_pair == nextResource) ++nextResource;
    }
    return nextResource == r.resource_pairs;
}
template<class T> void copySpan(WorldIndexSpan49B<const T> source, WorldIndexSpan49B<T> destination) noexcept {
    if (source.size) std::memcpy(destination.data, source.data, source.size * sizeof(T));
}
} // namespace

struct WorldIndexAccess49B {
    static WorldIndexView49B view(const OwnedWorldIndex49B& o) noexcept {
        WorldIndexView49B v;
        v.zones_ = {o.zones_.data(), o.zones_.size()};
        v.placements_ = {o.placements_.data(), o.placements_.size()};
        v.portals_ = {o.portals_.data(), o.portals_.size()};
        v.resources_ = {o.resources_.data(), o.resources_.size()};
        v.text_ = {o.text_.data(), o.text_.size()};
        v.incidents_ = {o.incidents_.data(), o.incidents_.size()};
        return v;
    }
    static WorldIndexView49B view(const WorldIndexStorage49B& s, const WorldIndexRequirements49B& r) noexcept {
        WorldIndexView49B v;
        v.zones_ = {s.zones.data, r.zones}; v.placements_ = {s.placements.data, r.placements};
        v.portals_ = {s.portals.data, r.portals}; v.resources_ = {s.resource_pairs.data, r.resource_pairs};
        v.text_ = {s.text.data, r.text_bytes}; v.incidents_ = {s.incident_portal_ids.data, r.incident_portal_ids};
        return v;
    }
    static void project(const World48F& source, OwnedWorldIndex49B& output,
        WorldIndexProvenance49B* provenance, const WorldIndexLimits49B& limits) {
        checkSourceLimits(source, limits);
        using Key = std::pair<std::string_view, std::string_view>;
        std::map<Key, std::uint32_t> interned;
        std::vector<Key> keys;
        std::vector<std::uint32_t> resourceIds;
        resourceIds.reserve(source.instances.size());
        std::size_t textBytes = 0;
        for (const auto& p : source.instances) {
            const Key key{p.geometry_member, p.collision_member};
            const auto found = interned.find(key);
            if (found != interned.end()) { resourceIds.push_back(found->second); continue; }
            need(keys.size() < std::min(limits.resource_pairs, 100000u), "world_index.resource_limit");
            const auto bytes = key.first.size() + key.second.size();
            const auto textLimit = std::min(limits.text_bytes, maxTextBytes);
            need(textBytes <= textLimit && bytes <= textLimit - textBytes, "world_index.text_limit");
            const auto id = static_cast<std::uint32_t>(keys.size());
            interned.emplace(key, id); keys.push_back(key); resourceIds.push_back(id); textBytes += bytes;
        }
        WorldIndexRequirements49B r;
        r.zones = source.zones.size(); r.placements = source.instances.size(); r.portals = source.portals.size();
        r.resource_pairs = keys.size(); r.text_bytes = textBytes; r.incident_portal_ids = r.portals * 2;
        r.payload_bytes = payload(r);
        need(r.payload_bytes <= std::min(limits.payload_bytes, maxPayloadBytes), "world_index.payload_limit");
        OwnedWorldIndex49B built;
        built.zones_.resize(r.zones); built.placements_.resize(r.placements); built.portals_.resize(r.portals);
        built.resources_.resize(r.resource_pairs); built.text_.resize(r.text_bytes); built.incidents_.resize(r.incident_portal_ids);
        WorldIndexProvenance49B audit;
        if (provenance) {
            audit.scene_info_block = source.scene_info_block;
            audit.zone_parameter_blocks.resize(r.zones); audit.portal_parameter_blocks.resize(r.portals);
            audit.placement_parameter_blocks.resize(r.placements); audit.placement_source_block_offsets.resize(r.placements);
        }
        std::uint32_t first = 0;
        for (std::size_t i = 0; i < r.zones; ++i) {
            const auto& s = source.zones[i];
            built.zones_[i] = {s.zone, first, s.models, 0, 0}; first += s.models;
            if (provenance) audit.zone_parameter_blocks[i] = s.parameter_block;
        }
        for (std::size_t i = 0; i < r.placements; ++i) {
            const auto& s = source.instances[i]; auto& d = built.placements_[i];
            std::memcpy(d.position.data(), s.position.data(), sizeof(d.position));
            std::memcpy(d.look.data(), s.look.data(), sizeof(d.look));
            std::memcpy(d.up.data(), s.up.data(), sizeof(d.up));
            std::memcpy(d.matrix.data(), s.matrix.data(), sizeof(d.matrix));
            d.zone = s.zone; d.ordinal = s.ordinal; d.resource_pair = resourceIds[i]; d.use_lighting = s.use_lighting ? 1u : 0u;
            if (provenance) { audit.placement_parameter_blocks[i] = s.parameter_block;
                audit.placement_source_block_offsets[i] = static_cast<std::uint32_t>(s.source_block_offset); }
        }
        for (std::size_t i = 0; i < r.portals; ++i) {
            const auto& s = source.portals[i]; auto& d = built.portals_[i];
            std::memcpy(d.vertices.data(), s.vertices.data(), sizeof(d.vertices));
            d.ordinal = static_cast<std::uint32_t>(i + 1); d.zones = s.zones;
            ++built.zones_[d.zones[0] - 1].incident_portal_count;
            ++built.zones_[d.zones[1] - 1].incident_portal_count;
            if (provenance) audit.portal_parameter_blocks[i] = s.parameter_block;
        }
        std::array<std::uint32_t, 256> cursor{};
        first = 0;
        for (std::size_t i = 0; i < r.zones; ++i) {
            auto& z = built.zones_[i]; z.first_incident_portal = first; cursor[i] = first; first += z.incident_portal_count;
        }
        // Scan once in original portal order; both endpoint lists inherit it.
        for (std::size_t i = 0; i < r.portals; ++i)
            for (auto zone : built.portals_[i].zones) built.incidents_[cursor[zone - 1]++] = static_cast<std::uint32_t>(i);
        std::uint32_t offset = 0;
        for (std::size_t i = 0; i < keys.size(); ++i) {
            const auto& key = keys[i]; auto& d = built.resources_[i];
            d.geometry_offset = offset; d.geometry_bytes = static_cast<std::uint32_t>(key.first.size());
            std::memcpy(built.text_.data() + offset, key.first.data(), key.first.size()); offset += d.geometry_bytes;
            d.collision_offset = offset; d.collision_bytes = static_cast<std::uint32_t>(key.second.size());
            std::memcpy(built.text_.data() + offset, key.second.data(), key.second.size()); offset += d.collision_bytes;
        }
        // Everything that can allocate/fail has completed before either commit.
        output.swap(built);
        if (provenance) swapProvenance(*provenance, audit);
    }
};

WorldIndexRequirements49B WorldIndexView49B::requirements() const noexcept {
    WorldIndexRequirements49B r;
    r.zones = zones_.size; r.placements = placements_.size; r.portals = portals_.size;
    r.resource_pairs = resources_.size; r.text_bytes = text_.size; r.incident_portal_ids = incidents_.size;
    r.payload_bytes = payload(r); return r;
}
const WorldZoneRecord49B* WorldIndexView49B::zone(std::uint32_t number) const noexcept {
    return number && number <= zones_.size ? zones_.data + (number - 1) : nullptr;
}
const WorldPlacement49B* WorldIndexView49B::placement(std::uint32_t number, std::uint32_t ordinal) const noexcept {
    const auto* z = zone(number);
    return z && ordinal && ordinal <= z->placement_count ? placements_.data + z->first_placement + (ordinal - 1) : nullptr;
}
const WorldPlacement49B* WorldIndexView49B::placementById(std::uint32_t id) const noexcept {
    return id < placements_.size ? placements_.data + id : nullptr;
}
const WorldPortalRecord49B* WorldIndexView49B::portalById(std::uint32_t id) const noexcept {
    return id < portals_.size ? portals_.data + id : nullptr;
}
WorldIndexSpan49B<const WorldPlacement49B> WorldIndexView49B::zonePlacements(std::uint32_t number) const noexcept {
    const auto* z = zone(number);
    return z && z->placement_count ? WorldIndexSpan49B<const WorldPlacement49B>{placements_.data + z->first_placement, z->placement_count} : WorldIndexSpan49B<const WorldPlacement49B>{};
}
WorldIndexSpan49B<const std::uint32_t> WorldIndexView49B::zoneIncidentPortals(std::uint32_t number) const noexcept {
    const auto* z = zone(number);
    return z && z->incident_portal_count ? WorldIndexSpan49B<const std::uint32_t>{incidents_.data + z->first_incident_portal, z->incident_portal_count} : WorldIndexSpan49B<const std::uint32_t>{};
}
bool WorldIndexView49B::resourceText(std::uint32_t id, WorldResourceText49B& output) const noexcept {
    if (id >= resources_.size) return false;
    const auto& r = resources_[id];
    output = {{text_.data + r.geometry_offset, r.geometry_bytes}, {text_.data + r.collision_offset, r.collision_bytes}};
    return true;
}
WorldIndexView49B OwnedWorldIndex49B::view() const noexcept { return WorldIndexAccess49B::view(*this); }
std::size_t OwnedWorldIndex49B::allocatedPayloadBytes() const noexcept {
    return zones_.capacity() * sizeof(WorldZoneRecord49B) + placements_.capacity() * sizeof(WorldPlacement49B) +
        portals_.capacity() * sizeof(WorldPortalRecord49B) + resources_.capacity() * sizeof(WorldResourcePair49B) +
        text_.capacity() + incidents_.capacity() * sizeof(std::uint32_t);
}
void OwnedWorldIndex49B::swap(OwnedWorldIndex49B& other) noexcept {
    zones_.swap(other.zones_); placements_.swap(other.placements_); portals_.swap(other.portals_);
    resources_.swap(other.resources_); text_.swap(other.text_); incidents_.swap(other.incidents_);
}
bool buildWorldIndex49B(const World48F& source, OwnedWorldIndex49B& output, std::string& error,
    WorldIndexProvenance49B* provenance, const WorldIndexLimits49B& limits) {
    error.clear();
    try {
        checkSourceLimits(source, limits);
        // The public 48F encoder provides its complete provenance + semantic
        // validator. The temporary unchanged-format bytes are load-time cost.
        std::vector<std::uint8_t> validated;
        if (!encodeWorldAsset48F(source, validated, error)) return false;
        need(validated.size() <= std::min(limits.asset_bytes, max_world_asset_bytes48f), "world_index.asset_byte_limit");
        // Release validation serialization before allocating resident staging.
        std::vector<std::uint8_t>().swap(validated);
        WorldIndexAccess49B::project(source, output, provenance, limits); return true;
    } catch (const std::bad_alloc&) { error = "world_index.allocation"; }
      catch (const std::exception& e) { error = e.what(); }
    return false;
}
bool loadWorldIndex49B(const std::vector<std::uint8_t>& input, OwnedWorldIndex49B& output, std::string& error,
    WorldIndexProvenance49B* provenance, const WorldIndexLimits49B& limits) {
    error.clear();
    try {
        need(input.size() <= std::min(limits.asset_bytes, max_world_asset_bytes48f), "world_index.asset_byte_limit");
        World48F source;
        if (!parseWorldAsset48F(input, source, error)) return false;
        WorldIndexAccess49B::project(source, output, provenance, limits); return true;
    } catch (const std::bad_alloc&) { error = "world_index.allocation"; }
      catch (const std::exception& e) { error = e.what(); }
    return false;
}
bool copyWorldIndex49B(const WorldIndexView49B& source, const WorldIndexStorage49B& storage,
    WorldIndexView49B& output, WorldIndexCopyError49B& error) noexcept {
    error = WorldIndexCopyError49B::none;
    AddressRange s[6], d[6];
    if (!range(source.zones(), s[0], error) || !range(source.placements(), s[1], error) ||
        !range(source.portals(), s[2], error) || !range(source.resourcePairs(), s[3], error) ||
        !range(source.text(), s[4], error) || !range(source.incidentPortalIds(), s[5], error)) {
        error = WorldIndexCopyError49B::invalid_source; return false;
    }
    if (!range(storage.zones, d[0], error) || !range(storage.placements, d[1], error) ||
        !range(storage.portals, d[2], error) || !range(storage.resource_pairs, d[3], error) ||
        !range(storage.text, d[4], error) || !range(storage.incident_portal_ids, d[5], error)) return false;
    const AddressRange controls[] = {objectRange(source), objectRange(storage), objectRange(output), objectRange(error)};
    for (unsigned i = 0; i < 6; ++i) {
        for (unsigned j = 0; j < 6; ++j) {
            if (overlaps(d[i], s[j]) || (i != j && (overlaps(d[i], d[j]) || overlaps(s[i], s[j])))) {
                error = WorldIndexCopyError49B::overlapping_storage; return false;
            }
        }
        for (const auto& c : controls) if (overlaps(d[i], c) || overlaps(s[i], c)) {
            error = WorldIndexCopyError49B::overlapping_storage; return false;
        }
    }
    const auto r = source.requirements();
    if (storage.zones.size < r.zones || storage.placements.size < r.placements || storage.portals.size < r.portals ||
        storage.resource_pairs.size < r.resource_pairs || storage.text.size < r.text_bytes || storage.incident_portal_ids.size < r.incident_portal_ids) {
        error = WorldIndexCopyError49B::insufficient_storage; return false;
    }
    if (!sourceValid(source)) { error = WorldIndexCopyError49B::invalid_source; return false; }
    copySpan(source.zones(), storage.zones); copySpan(source.placements(), storage.placements);
    copySpan(source.portals(), storage.portals); copySpan(source.resourcePairs(), storage.resource_pairs);
    copySpan(source.text(), storage.text); copySpan(source.incidentPortalIds(), storage.incident_portal_ids);
    output = WorldIndexAccess49B::view(storage, r); return true;
}
} }
