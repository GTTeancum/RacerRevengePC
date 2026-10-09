#pragma once
#include <cstddef>
#include <cstdint>

namespace rr::game {
inline constexpr std::size_t CandidateMergeCapacity48S=64;
struct CandidateMergeList48S {
    std::int32_t count=0;
    const std::uint32_t* identities=nullptr;
    std::size_t backingCount=0;
};
struct CandidateMergeInput48S {
    CandidateMergeList48S first{},second{};
};
struct CandidateMergeBuffer48S {
    std::uint32_t* identities=nullptr;
    std::size_t capacity=0;
};
struct CandidateMergeResult48S { std::int32_t count=0; };
enum class CandidateMergeStatus48S {
    Complete,InvalidInput,InsufficientBacking,InsufficientCapacity
};
// Isolated original219810 interval [219B1C,219BD0): reset output count,
// copy first (+58C/+590), then second (+488/+48C), at most64 raw32 words.
// Zero/repeated identities are preserved. Signed count<=0 consumes no IDs.
// Original still iterates and rereads each source count after saturation; this
// immutable, nonvolatile, callback-free projection collapses those count-only
// iterations. It does not run122350, detectors, or the full219810 caller.
//
// Let a=min(max(first.count,0),64), b=min(max(second.count,0),64-a).
// Only first[0..a),second[0..b) are read and need initialized elements. Their
// backingCount must cover those prefixes, NOT their entire logical counts.
// All declared spans must nevertheless name real caller-owned storage. Each
// nonempty span requires nonnull, aligned, representable storage. Read-only
// input spans may alias each other. Full declared writable spans (including
// unused tails) and result must not overlap any input span or descriptor;
// writable spans and result must also be mutually disjoint.
// Both scratch/destination capacities must cover a+b; neither must be64.
// Complete replaces destination[0..a+b) and result.count; destination and
// scratch tails remain untouched. Every error preserves all destination,
// result, AND scratch bytes. Original partial writes are not exposed on error.
// Allocation-free, at most64 input reads,128 list-word writes and one count
// write; constant stack,
// caller-owned scratch, no guest-pointer dereferences or retained state.
CandidateMergeStatus48S mergeCandidates48S(const CandidateMergeInput48S&,
    const CandidateMergeBuffer48S& scratch,const CandidateMergeBuffer48S& destination,
    CandidateMergeResult48S&) noexcept;
}
