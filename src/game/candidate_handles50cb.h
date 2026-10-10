#pragma once
#include <cstddef>
#include <cstdint>
namespace rr::game {
struct CandidateHandle50CB { std::uint32_t identity=0;std::size_t index=0; };
enum class CandidateHandleStatus50CB { Complete,InvalidInput,InsufficientCapacity,UnknownIdentity,DuplicateIdentity };
// Resolve the ordered merged identity list to owner indices. Repeated output
// identities are retained; duplicate owner identities are rejected as ambiguous.
// Owners are cold-built stable handles sorted by strictly increasing identity, not a spatial index or candidate source.
// Linear table validation and logarithmic lookup; never sorts the output.
// Uses caller scratch; success publishes all indices, failure preserves output.
// All declared spans are backed, aligned and disjoint except readonly spans.
CandidateHandleStatus50CB resolveCandidateHandles50CB(
 const std::uint32_t* identities,std::size_t count,
 const CandidateHandle50CB* owners,std::size_t ownerCount,
 std::size_t* scratch,std::size_t scratchCapacity,
 std::size_t* destination,std::size_t destinationCapacity)noexcept;
}
