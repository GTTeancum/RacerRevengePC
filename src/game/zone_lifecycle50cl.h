#pragma once
#include "zone_registration50cj.h"
#include "zone_search_views50cg.h"

namespace rr::game {
enum class ZoneMethod50CL : std::uint8_t { First=0,Second=1 };

// Caller-owned, stable backing. Registry order is zone1.first, zone1.second,
// zone2.first, zone2.second, ... . Registrations are indexed by object Ref-1.
// Active prefixes require 2*zoneCount registries/methods, zoneCount zones,
// objectCount registrations, and 4*zoneCount block descriptors. Extra capacity
// is protected from aliasing but is never initialized or written.
struct ZoneLifecycleStorage50CL {
    SearchStorage50BS* registries=nullptr;
    std::size_t registryCapacity=0;
    ZoneRegistration50CJ* registrations=nullptr;
    std::size_t registrationCapacity=0;
    ZoneSearchViews50CG views{};
};
struct ZoneLifecycle50CL {
    ZoneLifecycleStorage50CL storage{};
    std::size_t zoneCount=0,objectCount=0;
    std::uint32_t perBlock=0; // Zero denotes a fresh, unpublished adapter.
};

// Fresh-owner initialization only; initialize a value-initialized state once.
// All methods start cold/empty and share the explicit perBlock bound (1..1024).
// Backing words and unused descriptor tails remain opaque until first use.
// Uses 50BU initialization and 50CG empty-method projection. Failure leaves
// state and every backing array unchanged; caller-owned scratch may change.
SearchStorageStatus50BS initializeZoneLifecycle50CL(ZoneLifecycle50CL& state,
    const ZoneLifecycleStorage50CL& storage,std::size_t zoneCount,
    std::size_t objectCount,std::uint32_t perBlock,
    ZoneRegistrationScratch50CJ& scratch) noexcept;

// Register a previously empty object, retain its current slot, or transfer it
// to the explicitly chosen destination. Both refs are 1-based; method is exact.
// Encoded list identity is objectRef<<2, never an arbitrary object identity.
// objectCount and objectRef cannot exceed UINT32_MAX>>2.
// Successful transfers retain 50CJ source order and LIFO free-slot behavior.
// A full method with no free slot returns GrowthBoundary; no second-block
// allocation, eviction, repartitioning, or destination selection is performed.
SearchStorageStatus50BS updateZoneRegistration50CL(ZoneLifecycle50CL& state,
    CollectionRef50BX objectRef,CollectionRef50BX zoneRef,ZoneMethod50CL method,
    ZoneRegistrationScratch50CJ& scratch) noexcept;

// Removal is idempotent for an already empty, valid record. Physical list tails
// and the first block remain in place; 50CJ links the freed slot at freeHead.
SearchStorageStatus50BS removeZoneRegistration50CL(ZoneLifecycle50CL& state,
    CollectionRef50BX objectRef,ZoneRegistrationScratch50CJ& scratch) noexcept;

// Contract for every operation:
// - Full backing capacities, state, and scratch are aligned, representable,
//   mutually disjoint writable storage with caller-managed lifetimes.
// - The adapter exclusively owns its active registries, records, and views.
//   Do not edit them, move them, or use a copied state as a second owner.
// - Calls are serial and occur between collections, with no open list cursor.
// - On failure, registries, records, published views, and state are unchanged.
//   On success, changed method views are refreshed before the function returns;
//   this is a serial transaction, not a thread-safe/atomic publication scheme.
// - Zone refs and fixed method/descriptor addresses retain caller order. Zone
//   links, global/fallback heads, object geometry, and collector state remain
//   caller-owned. This adapter does not infer spatial ownership or portal rules.
}
