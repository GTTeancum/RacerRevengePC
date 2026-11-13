// Included after the real secondary scene getter; retain the same collected list.
// Stage S: secondaryRowLive is this racer's own 364 row (frame_physics, one accessor).
require50DU(mbSourceTriggers&&mbCollisionWorld&&mbCollisionWorld->ready()&&
    mbCollisionWorld->storage()==&trackStorage&&
    &mbCollisionWorld->world()==&liveWorld&&
    mbCollisionWorld->candidates()==candidates.data()&&
    mbCollisionWorld->candidateCount()==candidates.size(),
    "MB source secondary query lost its actual world/action owners");
SourceOuterResultLease50MB mbOuterResult;
SourceType2QueryOwners50MB mbQueryOwners{secondaryQuery,secondaryFilter,si,secondarySpring.records,
    secondaryRecords,60,secondaryResult,sip.data(),sim.data(),sip.size(),
    movementCandidates.data(),movementCandidates.size(),collisionNormalMode,
    mbSourceTriggers->resultBinding(),mbOuterResult,{},mbSourceTriggers->group(),mbSourceTriggers->hooks()};
mb_source_trigger50MB::QueryBorrow mbBorrow{s,x.bindings,x.parameters,navigationProgress,
    nullptr,nullptr,s.fields.vehicle58==1u?originalAi.racer60:playerRacer60};
const auto mbReceipt=mbSourceTriggers->query(actualRacerSlot,true,mbBorrow,secondaryAdmission,
    secondaryRowLive,mbQueryOwners,registeredIndices,registeredCount,sw,sd);
addCounter50DU(nativeCalls);
if(mbReceipt.live.prefixCommitted)
    addCounter50DU(secondaryCrossedPortals,static_cast<std::uint64_t>(mbReceipt.live.prefix.crossedPortals));
if(mbReceipt.status!=SourceType2QueryStatus50MB::Complete){
    recoveryPaused=true;
    std::cerr<<"MB source secondary query stopped: status="<<static_cast<unsigned>(mbReceipt.status)
        <<" candidate="<<mbReceipt.ordinal<<" action_pc="<<std::hex<<mbReceipt.contact.action.sourcePC
        <<std::dec<<"; actual prefixes retained; terminal owner cleanup required.\n";
    return false;
}
// No C80/reset action for a secondary identity, no old whole-list postcommit.
