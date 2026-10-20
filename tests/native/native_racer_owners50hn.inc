// Include once in runSession50DU immediately after advanceFrame is defined and
// before any player input/frame. Requires native_racer50HN::Bank racerBank.
// Only persistent live owners are stored. All original pointers keep their
// stable execution-arena addresses. No dynamic object metadata is copied.
// Every span below is disjoint; Bank rejects accidental overlap.
//
// Canonical frame includes both rigid bodies, complete graph/animation/visual,
// filter histories, joint, ground follow, damage, own serial RNG and source libm.
if(originalAiEnabled)registerRacerFrameExceptSharedNoise50IU(racerBank,s);
else racerBank.owner(s);
racerBank.owner(originalAi);
racerBank.owner(originalAiReceipt);
if(originalRecoveryEnabled){
 racerBank.owner(originalRecovery);racerBank.owner(originalRecoveryModels);
 racerBank.owner(originalHumanRecovery);racerBank.owner(humanBaseUndertrack);
 recoveryQueryOwners.registerMutableOwners(racerBank);
 racerBank.contents(qfilter.identityLists[0].identities);
 racerBank.contents(secondaryFilter.identityLists[0].identities);
 racerBank.owner(endpoints);racerBank.owner(endpointScenes);
 racerBank.owner(movement.copied40);racerBank.owner(movement.copied50);
}
//50HW: a roster slot owns its authored tuning and source binding descriptors,
// not just its live bodies. Borrowed PSG/MOT/type2 arrays remain in permanent
// per-archetype cold owners; restoring these POD views never moves that data.
// Configuration-sensitive input follows its actual racer; a human Controls
// selection must not leak into another existing control owner.
racerBank.owner(x.configurationEntryNonzero);
racerBank.owner(x.actionCodes);
racerBank.owner(x.parameters);
racerBank.owner(x.bindings);
racerBank.owner(x.secondaryInputs);
racerBank.owner(x.handRecords);
racerBank.owner(x.selectors);
racerBank.owner(x.frame);
racerBank.owner(x.diagnostic);
racerBank.owner(boostState);
racerBank.owner(boostDiagnostic);
// Source boost pitch/onset state is independent per racer. It only emits to
// the shared mixer after an accepted world commit, never during a bank visit.
racerBank.owner(opponentBoostAudio);
racerBank.owner(boostParameters);
racerBank.owner(boostCoolingCF8);
racerBank.owner(settings);
racerBank.owner(response);
racerBank.owner(secondarySettings);
racerBank.owner(secondaryResponse);
racerBank.owner(clock);
racerBank.owner(baseTail);
racerBank.owner(baseGlobals);
racerBank.owner(poseHistory);
racerBank.owner(coldFlame);
racerBank.owner(namesForEffects);
racerBank.owner(nativeEffects);
racerBank.owner(nativeEffectCount);
racerBank.owner(resetEffectBindings);
// Cold placement/geometry views follow the selected original forest. The
// pointed-to source bytes are immutable; sourceNodes refers either to this
// registered stable array or to the selected permanent Rig owner.
racerBank.owner(rebaseNodes);
racerBank.owner(rebaseModels);
racerBank.owner(rebaseModelCount);
// Actual source navigation, accepted lap crossings and wrong-way history.
racerBank.owner(navigationProgress);
racerBank.owner(navigationCA0);
racerBank.owner(navigationDiagnostic);
racerBank.owner(lapContinuity);
//50ID mode1 timing, cursor and known-null original HUD fields live within
//lapContinuity above. Shared mode1 config/manager/FIFO/dispatch, session lazy
//selector state and application clock remain outside this bank; do not register them here.
racerBank.owner(lapOwner.pending);
racerBank.owner(lapOwner.diagnostic);
racerBank.owner(lapOwner.completion);
racerBank.owner(lapOwner.completionResult);
// These owners are noncopyable; register their complete fixed member state.
lapOwner.sound.registerRacerState(racerBank);
lapOwner.provider.registerRacerState(racerBank);
racerBank.owner(lapOwner.audioContext);
racerBank.owner(lapOwner.audioDispatch);
racerBank.owner(lapOwner.audibleStarts);
racerBank.owner(lapOwner.markers);
racerBank.owner(lapOwner.completedCallbacks);
racerBank.owner(lapOwner.unavailableSounds);
racerBank.owner(wrongWayContinuation);
racerBank.owner(wrongWayReceipt);
racerBank.owner(wrongWayDiagnostic);
racerBank.owner(courseState);
racerBank.owner(courseDiagnostic);
racerBank.owner(courseOutput);
racerBank.owner(courseTuning);
racerBank.owner(autoDriveEnabled);
racerBank.owner(autoStopReported);
// Complete native device/action ownership, switched only after acknowledgement.
// These owners are noncopyable; register their fixed member state. Borrowed
// providers/references stay bound to the same permanent execution arena.
controller.registerRacerState(racerBank);
controllerActions.registerRacerState(racerBank);
racerBank.owner(controllerEventStorage);
racerBank.owner(controllerEvents);
racerBank.owner(playerEvents);
racerBank.owner(playerEnvironment);
racerBank.owner(applicationEvents);
racerBank.owner(playerEventChild);
racerBank.owner(unavailableEffect);
racerBank.owner(packet);
racerBank.owner(packetLive);
racerBank.owner(keyboardDiagnostic);
// Source query caches and history. Collision filter vectors are immutable;
// native queries mutate only flagsF8, selected state and copied40/copied50.
racerBank.owner(query);
racerBank.owner(qfilter.flagsF8);
racerBank.owner(secondaryQuery);
racerBank.owner(secondaryFilter.flagsF8);
racerBank.owner(spring);
racerBank.owner(secondarySpring);
racerBank.owner(initial);
racerBank.owner(si);
racerBank.owner(records);
racerBank.owner(secondaryRecords);
racerBank.owner(queryResult);
racerBank.owner(secondaryResult);
racerBank.contents(initialPairs);
racerBank.contents(initialMetadata);
racerBank.contents(sip);
racerBank.contents(sim);
racerBank.owner(movement.filter.flagsF8);
racerBank.owner(movement.initial);
racerBank.owner(movement.initialQueryCurrent);
racerBank.owner(movementInitial);
racerBank.owner(movementResult);
racerBank.owner(movementFlag);
racerBank.owner(terrainOwner.metadata.filter.flagsF8);
// The terrain metadata's vector-backed category/identity lists are immutable
// and shared only when identical. Numeric probe authoring is racer-specific.
// Never memcpy terrainOwner itself: its metadata contains std::vectors.
racerBank.owner(terrainOwner.initial);
racerBank.owner(terrainOwner.follow);
racerBank.owner(terrainOwner.lowerEndpoint140);
racerBank.owner(terrainOwner.lift1F0);
racerBank.owner(terrainOwner.receipt);
racerBank.owner(terrainOwner.prepared);
// resource.rays must point to the SAME live arena terrainLiveOwner.pair for
// every slot. Rebuild this cold owner in place when installing an archetype;
// copying an owner whose resource points into a temporary is invalid.
racerBank.owner(terrainLiveOwner);
if(!isRaTrack&&!isMbTrack)for(auto& candidate:candidates){
 racerBank.owner(candidate.candidate.collision.filter.flagsF8);
 racerBank.owner(candidate.candidate.collision.copied40);
 racerBank.owner(candidate.candidate.collision.copied50);
}
// Registers actual backing elements and mutable lanes, never a FixedBuffer,
// vector descriptor or the Storage owner. No duplicate old array spans remain.
if(!isRaTrack)trackStorage.registerMutableOwners(racerBank);
// A native source cut belongs only to its racer; never restart the human's
// session because an opponent reaches an unsupported source boundary.
racerBank.owner(recoveryPaused);
racerBank.owner(checkpointRequested);
racerBank.owner(checkpointOutput);
// Speech RNG/timers stay independent; the opponent visit suppresses audio I/O.
racerBank.owner(speechState);
racerBank.owner(pendingSpeech);
// Per-vehicle telemetry, not application rendering/output counters.
racerBank.owner(completedFrames);
racerBank.owner(mainContactFrames);
racerBank.owner(secondaryContactFrames);
racerBank.owner(repairActiveTicks);
racerBank.owner(nativeCalls);
racerBank.owner(effectCommands);
racerBank.owner(controllerPresses);
racerBank.owner(controllerReleases);
racerBank.owner(mainCrossedPortals);
racerBank.owner(secondaryCrossedPortals);
racerBank.owner(terrainQueries);
racerBank.owner(terrainHitFrames);
racerBank.owner(terrainSupportedFrames);
racerBank.owner(collectedCalls);
racerBank.owner(peakCollected);
racerBank.owner(navigationUpdates);
racerBank.owner(navigationGateSkips);
racerBank.owner(navigationCA0Copies);
racerBank.owner(navigationAISequential);
racerBank.owner(navigationMapSequential);
racerBank.owner(navigationAIScan);
racerBank.owner(navigationMapScan);
racerBank.owner(wrongWayUpdates);
racerBank.owner(wrongWayActiveTicks);
racerBank.owner(speechEvents);
racerBank.owner(speechCues);
racerBank.owner(announcerRequestsUnjoined);
