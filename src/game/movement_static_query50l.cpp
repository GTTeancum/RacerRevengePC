#include "movement_static_query50l.h"
#include "movement_callback50l.h"
#include <limits>
namespace rr::game {
MovementQueryOutcome50L queryMovementStaticCandidates50L(
 MovementQueryAdmission50L admission,const Type3QueryInput48P&input,const Type3QuerySource48P&source,
 const Type3Candidate48P*candidates,std::size_t count,const Type3QueryWorkspace48P&workspace,
 const Type3QueryBuffers48P&destination,Type3QueryResult48P&result,std::uint32_t flag) noexcept {
 using S=Type3QueryStatus48P;MovementQueryOutcome50L outcome{S::UnsupportedScope,flag};
 if(!admission.callbackIs20EE10||!admission.completeCollectedStaticOrder||
    !(input.filter.flagsF8&0x02u)||input.pairCount!=1||input.normalMode!=0||input.callbackEnabled||
    input.resetSpring||input.entry!=Type3QueryEntry48P::LimitedList)return outcome;
 const auto address=reinterpret_cast<std::uintptr_t>(candidates);
 if(count&&(!candidates||address%alignof(Type3Candidate48P)||count>std::numeric_limits<std::size_t>::max()/sizeof(*candidates)||
    count*sizeof(*candidates)>std::numeric_limits<std::uintptr_t>::max()-address))return {S::InvalidInput,flag};
 // Whole-domain static admission; even filtered dynamic entries are not admitted.
 for(std::size_t i=0;i<count;++i)if(candidates[i].collision.filter.categoryA4&input.dynamicMask)return outcome;
 outcome.status=queryStaticType3Bounded48P(input,source,candidates,count,workspace,destination,result);
 if(outcome.status==S::Complete&&result.hasAnyHit){
  // The branch is source-proven static and cannot fail or consume body data.
  (void)applyMovementCallback50L(0,input.dynamicMask,nullptr,nullptr,outcome.vehicle7A0);
 }
 return outcome;
}
}
