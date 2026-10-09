#pragma once
#include "type3_query48p.h"
namespace rr::game {
struct MovementQueryAdmission50L {
 bool callbackIs20EE10=false;
 bool completeCollectedStaticOrder=false;
};
struct MovementQueryOutcome50L {
 Type3QueryStatus48P status=Type3QueryStatus48P::InvalidInput;
 std::uint32_t vehicle7A0=0;
};
// Candidate-stage projection of active121920(a1=0), one world-space movement
// ray, mode0, callback20EE10, static flat candidates. The caller supplies the
// complete source-ordered collection output (both passes if required). This
// does not discover a world or implement registration, query70/84 cache writes,
// or the219120 hit-position/observer tail. No implicit no-hit is supplied.
//
// The type3 kernel owns its full-span transaction contract. The callback result
// is returned by value for the caller's sole7A0 owner; on failure it is the input
// value. Static20EE10 only writes1, so applying it once after any hit is exactly
// equivalent to its ordered static invocations. Dynamic categories, candidate
// response callbacks, group expansion, and spring callbacks are not substituted.
MovementQueryOutcome50L queryMovementStaticCandidates50L(
 MovementQueryAdmission50L,const Type3QueryInput48P&,const Type3QuerySource48P&,
 const Type3Candidate48P*,std::size_t,const Type3QueryWorkspace48P&,
 const Type3QueryBuffers48P&,Type3QueryResult48P&,std::uint32_t vehicle7A0) noexcept;
}
