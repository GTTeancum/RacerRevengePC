// Included only in advanceFramePart's real PostWorld main-query branch.
// All locals below refer to the current authentic Bank visit, including slot0.
// Stage S: mainRowLive is this racer's own 690 row (frame_physics, one accessor).
require50DU(mbSourceTriggers&&mbCollisionWorld&&mbCollisionWorld->ready()&&
    mbCollisionWorld->storage()==&trackStorage&&
    &mbCollisionWorld->world()==&liveWorld&&
    mbCollisionWorld->candidates()==candidates.data()&&
    mbCollisionWorld->candidateCount()==candidates.size()&&
    mbAiView.runtimeValid()&&mbAiView.role()==NavigationRole50MB::Ai&&mbAiView.sameWorld(liveWorld),
    "MB source main query lost its actual world/navigation/action owners");
SourceOuterResultLease50MB mbOuterResult;
SourceType2QueryOwners50MB mbQueryOwners{query,qfilter,initial,spring.records,records,40,
    queryResult,initialPairs.data(),initialMetadata.data(),initialPairs.size(),
    movementCandidates.data(),movementCandidates.size(),collisionNormalMode,
    mbSourceTriggers->resultBinding(),mbOuterResult,{},mbSourceTriggers->group(),mbSourceTriggers->hooks()};
const auto mbRacer60=s.fields.vehicle58==1u?originalAi.racer60:playerRacer60;
const auto mbExecute=[&](native_source_trigger_reset50MB::Owners* physical){
    mb_source_trigger50MB::QueryBorrow borrow{s,x.bindings,x.parameters,navigationProgress,
        physical,physical?originalRecoveryWorkspace.get():nullptr,mbRacer60};
    return mbSourceTriggers->query(actualRacerSlot,false,borrow,admission,mainRowLive,
        mbQueryOwners,registeredIndices,registeredCount,workspace,destination);
};
SourceType2QueryReceipt50MB mbReceipt;
std::uint8_t* mbFlags22C=nullptr;std::uint32_t* mbField24C=nullptr;
// Borrow only the real PostWorld D40/D44 owners. Missing physical ownership is
// not synthesized: a reached Reset1 preflight rejects inside the reserved query.
// Human and secondary colliders do not run the main690 LostAI physical action.
if(sourceAiAudio&&originalRecoveryEnabled&&originalAiAudio&&originalResetGround&&
    originalRecoveryWorkspace&&actualRacerSlot<racerCables.size()&&racerCables[actualRacerSlot]&&
    originalAiAudio->sourceTriggerResetOwners50MB(actualRacerSlot,mbRacer60,s,mbFlags22C,mbField24C)){
    const auto mbQueries=recoveryQueries();
    const auto& relative=x.secondaryInputs.targetRelative2F0;
    const VisualVector49R mbTargetRelative{{relative.v[0],relative.v[1],relative.v[2],relative.v[3]}};
    native_source_trigger_reset50MB::Owners physical{gcLightOwner,racerCables[actualRacerSlot]->visibility50(),
        *mbFlags22C,*mbField24C,mbAiView,mbSourceTriggers->resultBinding(),mbOuterResult,
        actualHeight.sourceDomain,resetGlobals,resetLifecycle,mbTargetRelative,mbQueries,
        zoneLifecycle,liveWorld,membershipWork,*originalResetGround,originalRecoveryModels,
        resetEffectBindings,nativeEffects,nativeEffectCount};
    // Physical query[0] is recoveryQueries()' SAME canonical main690 query.
    // Its identity and parent result binding are checked again at the real E8.
    mbReceipt=mbExecute(&physical);
}else mbReceipt=mbExecute(nullptr);
addCounter50DU(nativeCalls);
if(mbReceipt.live.prefixCommitted)
    addCounter50DU(mainCrossedPortals,static_cast<std::uint64_t>(mbReceipt.live.prefix.crossedPortals));
if(mbReceipt.status!=SourceType2QueryStatus50MB::Complete){
    recoveryPaused=true;
    std::cerr<<"MB source main query stopped: status="<<static_cast<unsigned>(mbReceipt.status)
        <<" candidate="<<mbReceipt.ordinal<<" action_pc="<<std::hex<<mbReceipt.contact.action.sourcePC
        <<std::dec<<"; actual prefixes retained; terminal owner cleanup required.\n";
    return false;
}
// The driver already published canonical pairs/records/filter/current/selected,
// reached normal mode, candidate effects and movement mirrors synchronously.
// Never rewrite its reset-updated query from a saved prefix or scratch batch.
