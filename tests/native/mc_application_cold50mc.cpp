// Exact original MC cold executable block; no owner relocation.
#include "mc_application_cold50mc.h"
#include "original_ui_main_entry50kg.h"
#include "mc_cold_sources50la.h"
#include "mc_original_world50lm.h"
#include "mc_trigger_authoring50mc.h"
#include "mc_collision_world50mc.h"
#include "mc_navigation50mc.h"
#include "mc_trigger_actions50mc.h"
#include "mc_animation_source50mc.h"
#include "mc_animation_runtime50mc.h"
#include "mc_animation_binding50mc.h"
#include "mc_particle_attachments50mc.h"
#include "mc_particle_runtime50mc.h"
#include "mc_track_fog50mc.h"
#include "mc_query50mc.h"
#include "native_ta_particle_cold50la_fixture.h"
#include "runtime_startup50fl_fixture.h"
#include "track_session_assets50ig_fixture.h"
#include "native_gc_water_session50kg_fixture.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "mc_trigger_geometry50mc.h"
#include "mc_particle_source50mc.h"
#include "mc_particle_cold50mc.h"
#include <stdexcept>

namespace interactive_course_cold50DU {
using namespace rr::game;
namespace {
void require50DU(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
}

void McColdPrefix::run() const {
 auto& mcOriginalDisc=this->mcOriginalDisc;
 auto& directOriginalHuman=this->directOriginalHuman;
 auto& originalAiEnabled=this->originalAiEnabled;
 auto& nativeOriginalFleet=this->nativeOriginalFleet;
 auto& originalMainApplication=this->originalMainApplication;
 auto& trackSession=this->trackSession;
 auto& s=this->s;
 auto& x=this->x;
 auto& raceRacerCount=this->raceRacerCount;
 auto& sessionIndex=this->sessionIndex;
 auto& nativeWaterGlobals=this->nativeWaterGlobals;
 auto& trackStorageLimits=this->trackStorageLimits;
 auto& mcColdSources=this->mcColdSources;
 auto& mcLightOwner=this->mcLightOwner;
 auto& mcNavigation=this->mcNavigation;
 auto& mcNavigationDiagnostic=this->mcNavigationDiagnostic;
 auto& mcTriggerAuthoring=this->mcTriggerAuthoring;
 auto& mcTriggerGeometry=this->mcTriggerGeometry;
 auto& mcAnimationSource=this->mcAnimationSource;
 auto& mcAnimationRuntime=this->mcAnimationRuntime;
 auto& mcAnimationDiagnostic=this->mcAnimationDiagnostic;
 auto& mcAnimationBinding=this->mcAnimationBinding;
 auto& mcAnimationBindingDiagnostic=this->mcAnimationBindingDiagnostic;
 auto& mcOriginalWorld=this->mcOriginalWorld;
 auto& mcParticleSource=this->mcParticleSource;
 auto& mcExternalPayloadReservation=this->mcExternalPayloadReservation;
 auto& mcParticleAttachments=this->mcParticleAttachments;
 auto& mcTrackParticles=this->mcTrackParticles;
 auto& mcParticleRuntime=this->mcParticleRuntime;
 auto& mcParticleRuntimeDiagnostic=this->mcParticleRuntimeDiagnostic;
 auto& mcTriggerActions=this->mcTriggerActions;
 auto& mcCollisionWorld=this->mcCollisionWorld;
 auto& mcFogOwner=this->mcFogOwner;
 const auto& reserveMcEarly=this->reserveMcEarly;
{
  require50DU(mcOriginalDisc&&directOriginalHuman&&originalAiEnabled&&nativeOriginalFleet&&originalMainApplication.state,
    "MC requires actual Main process and one human/seven original AI");
  originalMainApplication.appStart(originalMainApplication.assetRoot);
  const auto domain=originalMainApplication.standardMaterialDomain();
  const auto provider=domain?domain->archiveProvider3E1A70():nullptr;
  require50DU(bool(provider),"MC actual Main canonical source provider absent");
  rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic reusedDiagnostic;
  require50DU(mc_cold_sources50LA::Owner::reuseCanonicalSource(*provider,mcColdSources,reusedDiagnostic),"MC same-process source reuse rejected");
  if(!mcColdSources){
   auto source=std::make_shared<mc_cold_sources50LA::Owner>();mc_cold_sources50LA::Diagnostic d;
   {const bool mcPrepared=(source->decodeOwned(runtime_startup50fl::readBytes(trackSession->archive),s.animationShared.libm,
    std::min<std::size_t>(trackStorageLimits.coldPayloadBytes,32u*1024u*1024u),d));require50DU(mcPrepared,d.message.c_str());}mcColdSources=std::move(source);
  }
  reserveMcEarly(sizeof(mc_track_fog50MC::Owner));
  reserveMcEarly(mc_trigger_authoring50MC::Owner::maximumAdditionalPayloadBytes());
  reserveMcEarly(mc_animation_source50MC::Owner::maximumAdditionalPayloadBytes());
  reserveMcEarly(mc_animation_runtime50MC::Owner::requiredPayloadBytes());
  reserveMcEarly(mc_animation_binding50MC::Binding::requiredPayloadBytes());
  reserveMcEarly(mc_query50MC::Binding::requiredPayloadBytes());
  mcLightOwner=std::make_shared<original_gc_gameplay_lighting_candidate::Owner>();
  original_gc_gameplay_lighting_candidate::Diagnostic lightDiagnostic;
  require50DU(mcLightOwner->prepareMcNative(mcColdSources,raceRacerCount,lightDiagnostic),"MC original28-zone/29-record lighting rejected");
  reserveMcEarly(mcLightOwner->mcRetainedPayloadBytes());
  require50DU(mcNavigation.prepare(mcColdSources,mcNavigationDiagnostic),"MC exact173/173 source navigation rejected");
  std::string error;mcTriggerAuthoring=std::make_shared<mc_trigger_authoring50MC::Owner>();
  {const bool mcPrepared=(mcTriggerAuthoring->decode(mcColdSources,mc_trigger_authoring50MC::Owner::maximumAdditionalPayloadBytes(),error));require50DU(mcPrepared,error.c_str());}
  mcTriggerGeometry=std::make_shared<mc_trigger_geometry50MC::Owner>();
  {const bool mcPrepared=(mcTriggerGeometry->prepare(mcTriggerAuthoring,error));require50DU(mcPrepared,error.c_str());}
  reserveMcEarly(sizeof(*mcTriggerGeometry));
  mcAnimationSource=std::make_shared<mc_animation_source50MC::Owner>();mc_animation_source50MC::Diagnostic animationSourceDiagnostic;
  {const bool mcPrepared=(mcAnimationSource->decode(mcColdSources,mc_animation_source50MC::Owner::maximumAdditionalPayloadBytes(),animationSourceDiagnostic));require50DU(mcPrepared,animationSourceDiagnostic.message.c_str());}
  // Decode returned: only measured source/scene capacities remain alive.
  const auto animationPeak=mc_animation_source50MC::Owner::maximumAdditionalPayloadBytes();
  const auto animationRetained=mcAnimationSource->retainedPayloadBytes();
  require50DU(animationRetained<=animationPeak&&mcExternalPayloadReservation>=animationPeak-animationRetained,"MC animation scratch release accounting");
  mcExternalPayloadReservation-=animationPeak-animationRetained;
  mcAnimationRuntime=std::make_shared<mc_animation_runtime50MC::Owner>();
  {const bool mcPrepared=(mcAnimationRuntime->beginPrepare(mcAnimationSource,s.animationShared.motion,
    mc_animation_runtime50MC::Owner::requiredPayloadBytes(),mcAnimationDiagnostic));require50DU(mcPrepared,mcAnimationDiagnostic.message.c_str());}
  mcAnimationBinding=std::make_unique<mc_animation_binding50MC::Binding>();
  {const bool mcPrepared=(mcAnimationBinding->beginVisual(mcAnimationRuntime,mcLightOwner,mcAnimationBindingDiagnostic));require50DU(mcPrepared,mcAnimationBindingDiagnostic.message.c_str());}
  mcOriginalWorld=std::make_shared<mc_original_world50LM::Owner>();mc_original_world50LM::Diagnostic worldDiagnostic;
  {const bool mcPrepared=(mcOriginalWorld->load(mcColdSources,worldDiagnostic));require50DU(mcPrepared,worldDiagnostic.message.c_str());}
  const auto mountReservation=mcColdSources->canonicalMountReservationBytes();reserveMcEarly(mountReservation);
  rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic mounted;rr::assets::OriginalStandardTextureDiagnostic50LA textureDiagnostic;
  require50DU(mcColdSources->mountCanonicalArchive(*provider,mountReservation,mounted)&&provider->sealNativeMounts(mounted)&&
   domain->resolver3E1A70()->bindProvider3E1A70(provider,textureDiagnostic),"MC canonical original archive mount rejected");
  // Source-specific original40 descriptor bank, distinct real Main manager.
  reserveMcEarly(mc_particle_source50MC::Owner::maximumAdditionalPayloadBytes()+128u);
  reserveMcEarly(mc_particle_attachments50MC::Owner::maximumAdditionalPayloadBytes()+128u);
  mcParticleSource=std::make_shared<mc_particle_source50MC::Owner>();mc_particle_source50MC::Diagnostic particleSourceDiagnostic;
  {const bool mcPrepared=(mcParticleSource->decode(mcColdSources,mc_particle_source50MC::Owner::maximumAdditionalPayloadBytes(),particleSourceDiagnostic));require50DU(mcPrepared,particleSourceDiagnostic.message.c_str());}
  // This decoder phase has returned and destroyed its actual temporary owners.
  mcExternalPayloadReservation-=mc_particle_source50MC::Owner::maximumAdditionalPayloadBytes()-mcParticleSource->retainedPayloadBytes();
  mcParticleAttachments=std::make_shared<mc_particle_attachments50MC::Owner>();mc_particle_attachments50MC::Diagnostic attachmentsDiagnostic;
  {const bool mcPrepared=(mcParticleAttachments->prepare(mcParticleSource,mcOriginalWorld,mc_particle_attachments50MC::Owner::maximumAdditionalPayloadBytes(),attachmentsDiagnostic));require50DU(mcPrepared,attachmentsDiagnostic.message.c_str());}
  // This decoder phase has returned and destroyed its actual temporary owners.
  mcExternalPayloadReservation-=mc_particle_attachments50MC::Owner::maximumAdditionalPayloadBytes()-sizeof(mc_particle_attachments50MC::Owner);
  constexpr std::size_t mcRegistryQuota=2u*originalGcParticleCapacity50KG*sizeof(OriginalGcParticleEmitterState50KG);
  reserveMcEarly(sizeof(OriginalTrackParticleCold50LA)+OriginalGcParticleManager50KG::emitterRegistryReservationMetadataBytes()+128u);
  reserveMcEarly(mcRegistryQuota+mc_particle_runtime50MC::Owner::retainedPayloadBytes()+128u);
  reserveMcEarly(4u*(10770u+5022u+132124u));
  mcTrackParticles=std::make_shared<OriginalTrackParticleCold50LA>();OriginalTrackParticleColdDiagnostic50LA particleColdDiagnostic;
  require50DU(native_ta_particle_cold50LA::prepareMc(*mcTrackParticles,*trackSession,mcColdSources,originalMainApplication,domain->materials382B38(),sessionIndex+1,particleColdDiagnostic),"MC actual original canonical particle pool rejected");
  mcParticleRuntime=std::make_shared<mc_particle_runtime50MC::Owner>();
  {const bool mcPrepared=(mcParticleRuntime->prepare(mcParticleSource,mcParticleAttachments,mcTrackParticles,originalMainApplication.state,nativeWaterGlobals,
    originalMainApplication.animationClock(),sessionIndex+1,mcRegistryQuota,mcRegistryQuota+mc_particle_runtime50MC::Owner::retainedPayloadBytes(),mcParticleRuntimeDiagnostic));require50DU(mcPrepared,mcParticleRuntimeDiagnostic.message.c_str());}
  mcTriggerActions=std::make_shared<mc_trigger_actions50MC::Owner>();reserveMcEarly(sizeof(*mcTriggerActions));
  require50DU(mcTriggerActions->prepare(mcTriggerAuthoring,mcAnimationRuntime,mcTriggerGeometry),"MC real LostAI/Animation action destinations rejected");
  mcCollisionWorld=std::make_shared<mc_collision_world50MC::Owner>();
  {const bool mcPrepared=(mcCollisionWorld->prepareGround(mcColdSources,{4,16,32,64},x.assets.phy,trackSession->gridRank,
    mcExternalPayloadReservation,trackStorageLimits,error));require50DU(mcPrepared,error.c_str());}
  mcFogOwner=std::make_shared<mc_track_fog50MC::Owner>();{const bool mcPrepared=(mcFogOwner->prepare(mcColdSources,error));require50DU(mcPrepared,error.c_str());}
 }
}
} // namespace interactive_course_cold50DU
