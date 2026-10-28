// Actual MC root cold prefix. Kept out of root selection until all downstream
// query/particle/renderer joins compile together. Original media only.
 std::shared_ptr<const mc_cold_sources50LA::Owner> mcColdSources;
 std::shared_ptr<original_gc_gameplay_lighting_candidate::Owner> mcLightOwner;
 std::shared_ptr<mc_original_world50LM::Owner> mcOriginalWorld;
 std::shared_ptr<mc_trigger_authoring50MC::Owner> mcTriggerAuthoring;
 std::shared_ptr<mc_trigger_geometry50MC::Owner> mcTriggerGeometry;
 std::shared_ptr<mc_trigger_actions50MC::Owner> mcTriggerActions;
 std::shared_ptr<mc_collision_world50MC::Owner> mcCollisionWorld;
 std::optional<mc_collision_world50MC::Owner::Borrow> mcWorldBorrow;
 std::shared_ptr<mc_navigation50MC::Owner> mcNavigationOwner;
 mc_navigation50MC::Owner emptyMcNavigation;
 if(isMcTrack)mcNavigationOwner=std::make_shared<mc_navigation50MC::Owner>();
 auto& mcNavigation=mcNavigationOwner?*mcNavigationOwner:emptyMcNavigation;
 mc_navigation50MC::Diagnostic mcNavigationDiagnostic;
 std::shared_ptr<mc_animation_source50MC::Owner> mcAnimationSource;
 std::shared_ptr<mc_animation_runtime50MC::Owner> mcAnimationRuntime;
 std::unique_ptr<mc_animation_binding50MC::Binding> mcAnimationBinding;
 std::unique_ptr<mc_query50MC::Binding> mcQueryBinding;
 mc_animation_runtime50MC::Diagnostic mcAnimationDiagnostic;
 mc_animation_binding50MC::Diagnostic mcAnimationBindingDiagnostic;
 std::shared_ptr<mc_track_fog50MC::Owner> mcFogOwner;
 std::shared_ptr<mc_particle_source50MC::Owner> mcParticleSource;
 std::shared_ptr<mc_particle_attachments50MC::Owner> mcParticleAttachments;
 std::shared_ptr<OriginalTrackParticleCold50LA> mcTrackParticles;
 std::shared_ptr<mc_particle_runtime50MC::Owner> mcParticleRuntime;
 std::shared_ptr<mc_particle_visual50MC::Window> mcParticleWindow;
 std::shared_ptr<mc_particle_render50MC::Owner> mcParticleRender;
 mc_particle_runtime50MC::Diagnostic mcParticleRuntimeDiagnostic;

 std::size_t mcExternalPayloadReservation=0,mcAdditionalPayloadReserved=0;
 const auto reserveMcEarly=[&](std::size_t bytes){
  const auto retained=mcColdSources?mcColdSources->retainedPayloadBytes():0u;
  require50DU(retained<=trackStorageLimits.coldPayloadBytes&&bytes<=trackStorageLimits.coldPayloadBytes-retained&&
    mcExternalPayloadReservation<=trackStorageLimits.coldPayloadBytes-retained-bytes,
    "MC exact source plus descriptor reservation overflow");mcExternalPayloadReservation+=bytes;
 };
 #include "mc_application_prefix_retirement50mc.inl"
 if(isMcTrack)interactive_course_cold50DU::McColdPrefix{
  .mcOriginalDisc=mcOriginalDisc,
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
  .mcColdSources=mcColdSources,
  .mcLightOwner=mcLightOwner,
  .mcNavigation=mcNavigation,
  .mcNavigationDiagnostic=mcNavigationDiagnostic,
  .mcTriggerAuthoring=mcTriggerAuthoring,
  .mcTriggerGeometry=mcTriggerGeometry,
  .mcAnimationSource=mcAnimationSource,
  .mcAnimationRuntime=mcAnimationRuntime,
  .mcAnimationDiagnostic=mcAnimationDiagnostic,
  .mcAnimationBinding=mcAnimationBinding,
  .mcAnimationBindingDiagnostic=mcAnimationBindingDiagnostic,
  .mcOriginalWorld=mcOriginalWorld,
  .mcParticleSource=mcParticleSource,
  .mcExternalPayloadReservation=mcExternalPayloadReservation,
  .mcParticleAttachments=mcParticleAttachments,
  .mcTrackParticles=mcTrackParticles,
  .mcParticleRuntime=mcParticleRuntime,
  .mcParticleRuntimeDiagnostic=mcParticleRuntimeDiagnostic,
  .mcTriggerActions=mcTriggerActions,
  .mcCollisionWorld=mcCollisionWorld,
  .mcFogOwner=mcFogOwner,
  .reserveMcEarly=interactive_frame50DU::BorrowedCall<void(std::size_t)>(reserveMcEarly),
 }.run();
