#pragma once
#include "candidate_collection50bx.h"
#include "candidate_merge48s.h"
namespace rr::game {
// Explicit caller-owned staging. All storage is exclusive for the duration of
// the call. On failure stages may change and MUST NOT be published to the live
// world. On success second contains final world state and merged holds IDs.
struct VehicleCandidateWorkspace50CA {
    CandidateCollectionWorkspace50BX collector{};
    CandidateCollectionBuffers50BX first{},second{};
    CandidateMergeBuffer48S mergeScratch{},merged{};
};
struct VehicleCandidateResult50CA {
    enum class Phase { Admission, First, Second, Merge, Complete } phase=Phase::Admission;
    CandidateCollectionStatus50BX collectionStatus=CandidateCollectionStatus50BX::InvalidInput;
    CandidateMergeStatus48S mergeStatus=CandidateMergeStatus48S::InvalidInput;
    CandidateCollectionResult50BX first{},second{};
    std::int32_t count=0;
};
// Original219AD4..219BD0: same query, method2 then method1, ordered merge64.
// Caller supplies the correctly prepared query matrix/cache and world snapshot.
// No live zone construction, registration or frame-phase substitution occurs.
// Allocation-free. The returned result owns no storage; publish workspace.second
// state and workspace.merged IDs only when phase==Complete. Initial state and
// all source tables stay unchanged; full cross-stage alias checks precede writes.
VehicleCandidateResult50CA collectVehicleCandidates50CA(
    const CandidateCollectionInput50BX&,const CandidateCollectionState50BX&,
    const VehicleCandidateWorkspace50CA&) noexcept;
}
