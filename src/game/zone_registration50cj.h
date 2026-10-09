#pragma once
#include "search_cold50bu.h"
namespace rr::game {
struct ZoneRegistration50CJ {
 SearchStorage50BS* owner=nullptr;
 std::uint32_t slot=0,encodedObject=0;
};
struct ZoneRegistrationScratch50CJ {SearchStorage50BS previous{},next{};};
// Explicit prepared-owner transfer, not a policy for choosing the current zone.
// Caller chooses nextOwner (null means unregister). Identity is native Ref<<2.
// Existing record is either empty or names its exact live slot. On success the
// old/new registries and record commit together. Failure preserves all three.
// Scratch must be caller-owned and disjoint. No heap allocation or list growth
// beyond the existing bounded first-block publication. Views must be rebuilt.
SearchStorageStatus50BS movePreparedRegistration50CJ(ZoneRegistration50CJ&,
 SearchStorage50BS*nextOwner,std::uint32_t encodedObject,
 ZoneRegistrationScratch50CJ&)noexcept;
}
