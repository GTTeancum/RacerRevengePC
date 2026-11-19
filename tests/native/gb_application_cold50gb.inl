// Included inside the actual root; all retained owners precede their Bank,
// query, camera and renderer borrowers. No direct-entry or generated pack path.
 std::shared_ptr<const gb_cold_sources50LA::Owner> gbColdSources;
 std::shared_ptr<original_gc_gameplay_lighting_candidate::Owner> gbLightOwner;
 std::shared_ptr<gb_original_world50LM::Owner> gbOriginalWorld;
 std::shared_ptr<gb_trigger_authoring50GB::Owner> gbTriggerAuthoring;
 std::shared_ptr<gb_collision_world50GB::Owner> gbCollisionWorld;
 std::optional<gb_collision_world50GB::Owner::Borrow> gbWorldBorrow;
 std::shared_ptr<gb_navigation50GB::Owner> gbNavigationOwner;
 gb_navigation50GB::Owner emptyGbNavigation;
 if(isGbTrack)gbNavigationOwner=std::make_shared<gb_navigation50GB::Owner>();
 auto& gbNavigation=gbNavigationOwner?*gbNavigationOwner:emptyGbNavigation;
 gb_navigation50GB::Diagnostic gbNavigationDiagnostic;
 std::shared_ptr<gb_trigger_actions50GB::Owner> gbTriggerActions;
 std::shared_ptr<gb_particle_authoring50GB::Owner> gbParticleAuthoring;
 std::shared_ptr<gb_particle_attachments50GB::Owner> gbParticleAttachments;
 std::shared_ptr<OriginalTrackParticleCold50LA> gbTrackParticles;
 std::shared_ptr<gb_particle_runtime50GB::Owner> gbParticleRuntime;
 std::shared_ptr<gb_particle_visual50GB::Window> gbParticleWindow;
 std::shared_ptr<gb_particle_render50GB::Owner> gbParticleRender;
 std::shared_ptr<gb_track_fog50GB::Owner> gbFogOwner;
 gb_particle_runtime50GB::Diagnostic gbParticleRuntimeDiagnostic;
 std::unique_ptr<gb_query50GB::Binding> gbQueryBinding;
 std::size_t gbExternalPayloadReservation=0,gbAdditionalPayloadReserved=0;
 const auto reserveGbEarly=[&](std::size_t bytes){
  const auto retainedSource=gbColdSources?gbColdSources->retainedPayloadBytes():0u;
  require50DU(retainedSource<=trackStorageLimits.coldPayloadBytes&&
      bytes<=trackStorageLimits.coldPayloadBytes-retainedSource&&
      gbExternalPayloadReservation<=trackStorageLimits.coldPayloadBytes-retainedSource-bytes,
      "GB early source plus shared descriptor reservation overflow");gbExternalPayloadReservation+=bytes;
 };
 #include "gb_application_prefix_retirement50gb.inl"
 if(isGbTrack)interactive_course_cold50DU::GbColdPrefix{
  .gbOriginalDisc=gbOriginalDisc,
  .directOriginalHuman=directOriginalHuman,
  .originalAiEnabled=originalAiEnabled,
  .nativeOriginalFleet=nativeOriginalFleet,
  .originalMainApplication=originalMainApplication,
  .trackSession=trackSession,
  .s=s,
  .x=x,
  .raceRacerCount=raceRacerCount,
  .sessionIndex=sessionIndex,
  .nativeWaterGlobals=nativeWaterGlobals,
  .trackStorageLimits=trackStorageLimits,
  .gbColdSources=gbColdSources,
  .gbLightOwner=gbLightOwner,
  .gbNavigation=gbNavigation,
  .gbNavigationDiagnostic=gbNavigationDiagnostic,
  .gbTriggerAuthoring=gbTriggerAuthoring,
  .trackAnimations=trackAnimations,
  .trackAnimationBinding=trackAnimationBinding,
  .trackAnimationBindingDiagnostic=trackAnimationBindingDiagnostic,
  .gbOriginalWorld=gbOriginalWorld,
  .gbParticleAuthoring=gbParticleAuthoring,
  .gbExternalPayloadReservation=gbExternalPayloadReservation,
  .gbParticleAttachments=gbParticleAttachments,
  .gbTrackParticles=gbTrackParticles,
  .gbParticleRuntime=gbParticleRuntime,
  .gbParticleRuntimeDiagnostic=gbParticleRuntimeDiagnostic,
  .gbTriggerActions=gbTriggerActions,
  .gbCollisionWorld=gbCollisionWorld,
  .gbFogOwner=gbFogOwner,
  .gbPodSound=gbPodSound,
  .prepareGbPodSound=prepareGbPodSound,
  .reserveGbEarly=interactive_frame50DU::BorrowedCall<void(std::size_t)>(reserveGbEarly),
 }.run();
