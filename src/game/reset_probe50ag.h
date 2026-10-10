#pragma once
#include <cstdint>
namespace rr::game {
enum class ResetProbePhase50AG : std::uint32_t { AwaitingQuery, Hit, Exhausted };
struct ResetProbe50AG {
 std::uint32_t staged[4]{},start[4]{},end[4]{},lift=0,attempt=0;
 ResetProbePhase50AG phase=ResetProbePhase50AG::AwaitingQuery;
};
// Original216E8C..216F60 numeric sequencing at explicit124CB0 call boundaries.
// Caller supplies BC0 AFTER earlier terrain/water adjustments and must execute
// the actual query for each request. This does not supply a miss or geometry,
// enumerate candidates, invoke scene callbacks, or complete vehicle reset.
// On exhausted attempts the original diagnostic call is left to the caller.
// XYZ/lift finite; opaque W copied verbatim. Serial, bounded, no allocation.
// Errors leave destination unchanged. State and request are caller owned.
bool beginResetProbe50AG(const std::uint32_t staged[4],std::uint32_t lift,
 ResetProbe50AG&) noexcept;
bool acceptResetProbe50AG(ResetProbe50AG&,bool hit,std::uint32_t hitY) noexcept;
}
