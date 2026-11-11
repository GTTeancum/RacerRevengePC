// Exact original GB cold executable block; no owner relocation.
#include "gb_application_cold50gb.h"
#include "original_ui_main_entry50kg.h"
#include "gb_cold_sources50la.h"
#include "native_course_archive_release50pd.h"
#include "gb_original_world50lm.h"
#include "gb_trigger_authoring50gb.h"
#include "gb_collision_world50gb.h"
#include "gb_navigation50gb.h"
#include "gb_trigger_actions50gb.h"
#include "gb_animation_source50gb.h"
#include "gb_animation_runtime50gb.h"
#include "gb_animation_binding50gb.h"
#include "gb_particle_attachments50gb.h"
#include "gb_particle_runtime50gb.h"
#include "gb_track_fog50gb.h"
#include "gb_query50gb.h"
#include "native_ta_particle_cold50la_fixture.h"
#include "runtime_startup50fl_fixture.h"
#include "track_session_assets50ig_fixture.h"
#include "native_gc_water_session50kg_fixture.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "gb_particle_authoring50gb.h"
#include "gb_pod_sound50gb.h"
#include <stdexcept>

namespace interactive_course_cold50DU {
using namespace rr::game;
namespace {
void require50DU(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
}

void GbColdPrefix::run() const {
 auto& gbOriginalDisc=this->gbOriginalDisc;
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
 auto& gbColdSources=this->gbColdSources;
 auto& gbLightOwner=this->gbLightOwner;
 auto& gbNavigation=this->gbNavigation;
 auto& gbNavigationDiagnostic=this->gbNavigationDiagnostic;
 auto& gbTriggerAuthoring=this->gbTriggerAuthoring;
 auto& gbAnimationSource=this->gbAnimationSource;
 auto& gbAnimationRuntime=this->gbAnimationRuntime;
 auto& gbAnimationDiagnostic=this->gbAnimationDiagnostic;
 auto& gbAnimationBinding=this->gbAnimationBinding;
 auto& gbAnimationBindingDiagnostic=this->gbAnimationBindingDiagnostic;
 auto& gbOriginalWorld=this->gbOriginalWorld;
 auto& gbParticleAuthoring=this->gbParticleAuthoring;
 auto& gbExternalPayloadReservation=this->gbExternalPayloadReservation;
 auto& gbParticleAttachments=this->gbParticleAttachments;
 auto& gbTrackParticles=this->gbTrackParticles;
 auto& gbParticleRuntime=this->gbParticleRuntime;
 auto& gbParticleRuntimeDiagnostic=this->gbParticleRuntimeDiagnostic;
 auto& gbTriggerActions=this->gbTriggerActions;
 auto& gbCollisionWorld=this->gbCollisionWorld;
 auto& gbFogOwner=this->gbFogOwner;
 auto& gbPodSound=this->gbPodSound;
 const auto& prepareGbPodSound=this->prepareGbPodSound;
 const auto& reserveGbEarly=this->reserveGbEarly;
{
  require50DU(gbOriginalDisc&&directOriginalHuman&&originalAiEnabled&&nativeOriginalFleet&&originalMainApplication.state,
      "GB requires the actual Main process and one human/seven original AI");
  originalMainApplication.appStart(originalMainApplication.assetRoot);
  const auto domain=originalMainApplication.standardMaterialDomain();
  const auto provider=domain?domain->archiveProvider3E1A70():nullptr;
  require50DU(bool(provider),"GB actual Main canonical source provider absent");
  rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic reusedDiagnostic;
  require50DU(gb_cold_sources50LA::Owner::reuseCanonicalSource(*provider,gbColdSources,reusedDiagnostic),
      "GB same-process source reuse rejected");
  if(!gbColdSources){
   auto source=std::make_shared<gb_cold_sources50LA::Owner>();gb_cold_sources50LA::Diagnostic diagnostic;
   const bool decoded=source->decodeOwned(runtime_startup50fl::readBytes(trackSession->archive),s.animationShared.libm,
       trackStorageLimits.coldPayloadBytes,diagnostic);
   require50DU(decoded,diagnostic.message.empty()?"GB original compact source rejected":diagnostic.message.c_str());gbColdSources=std::move(source);
  }
  reserveGbEarly(gb_query50GB::Binding::requiredPayloadBytes());
  reserveGbEarly(sizeof(gb_track_fog50GB::Owner));
  reserveGbEarly(gb_trigger_authoring50GB::Owner::maximumAdditionalPayloadBytes());
  reserveGbEarly(gb_animation_source50GB::Owner::maximumAdditionalPayloadBytes());
  reserveGbEarly(gb_animation_runtime50GB::Owner::requiredPayloadBytes());
  reserveGbEarly(gb_animation_binding50GB::Binding::requiredPayloadBytes());
  reserveGbEarly(gb_trigger_actions50GB::Owner::maximumAdditionalPayloadBytes());
  reserveGbEarly(original_gc_gameplay_lighting_candidate::Owner::gbAnimationStorageReservationBytes());
  reserveGbEarly(original_gc_gameplay_lighting_candidate::Owner::gbTimedLightStorageReservationBytes());
  gbLightOwner=std::make_shared<original_gc_gameplay_lighting_candidate::Owner>();
  original_gc_gameplay_lighting_candidate::Diagnostic lightDiagnostic;
  require50DU(gbLightOwner->prepareGbNative(gbColdSources->world().parameters.original_bytes,raceRacerCount,lightDiagnostic),
      "GB genuine same-source timed lighting rejected");
  require50DU(gbNavigation.prepare(gbColdSources,gbNavigationDiagnostic),"GB authentic213/257 navigation source rejected");
  gbTriggerAuthoring=std::make_shared<gb_trigger_authoring50GB::Owner>();std::string error;
  require50DU(gbTriggerAuthoring->decode(gbColdSources,error),error.empty()?"GB eighteen source triggers rejected":error.c_str());
  gbAnimationSource=std::make_shared<gb_animation_source50GB::Owner>();gb_animation_source50GB::Diagnostic animationSourceDiagnostic;
  require50DU(gbAnimationSource->decode(gbColdSources,s.animationShared.libm,
      gb_animation_source50GB::Owner::maximumAdditionalPayloadBytes(),animationSourceDiagnostic),"GB four authentic animation sources rejected");
  gbAnimationRuntime=std::make_shared<gb_animation_runtime50GB::Owner>();
  require50DU(gbAnimationRuntime->beginPrepare(gbAnimationSource,s.animationShared.motion,
      gb_animation_runtime50GB::Owner::requiredPayloadBytes(),gbAnimationDiagnostic),"GB four actual animation graphs rejected");
  gbAnimationBinding=std::make_unique<gb_animation_binding50GB::Binding>();
  require50DU(gbAnimationBinding->beginVisual(gbAnimationRuntime,gbLightOwner,gbAnimationBindingDiagnostic),
      "GB actual four-animation visual prefix rejected");
  gbOriginalWorld=std::make_shared<gb_original_world50LM::Owner>();gb_original_world50LM::Diagnostic worldDiagnostic;
  require50DU(gbOriginalWorld->load(gbColdSources,worldDiagnostic),worldDiagnostic.message.empty()?"GB original static world rejected":worldDiagnostic.message.c_str());
  gbParticleAuthoring=std::make_shared<gb_particle_authoring50GB::Owner>();gb_particle_authoring50GB::Diagnostic particleAuthoringDiagnostic;
  require50DU(gbParticleAuthoring->decode(gbColdSources,trackStorageLimits.coldPayloadBytes-gbExternalPayloadReservation-gbColdSources->retainedPayloadBytes(),
      particleAuthoringDiagnostic),"GB original twelve descriptors/67 emitter instances rejected");
  reserveGbEarly(gbParticleAuthoring->retainedPayloadBytes());
  reserveGbEarly(gb_particle_attachments50GB::Owner::maximumAdditionalPayloadBytes());
  gbParticleAttachments=std::make_shared<gb_particle_attachments50GB::Owner>();gb_particle_attachments50GB::Diagnostic attachmentsDiagnostic;
  require50DU(gbParticleAttachments->prepare(gbParticleAuthoring,gbOriginalWorld,
      gb_particle_attachments50GB::Owner::maximumAdditionalPayloadBytes(),attachmentsDiagnostic),"GB actual55 attachment graphs rejected");
  const auto mountReservation=gbColdSources->canonicalMountReservationBytes();reserveGbEarly(mountReservation);
  rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic mounted;
  rr::assets::OriginalStandardTextureDiagnostic50LA textureDiagnostic;
  require50DU(gbColdSources->mountCanonicalArchive(*provider,mountReservation,mounted,
      native_course_archive_release50PD::holder().begin(provider,domain->resolver3E1A70()))&&provider->sealNativeMounts(mounted)&&
      domain->resolver3E1A70()->bindProvider3E1A70(provider,textureDiagnostic),"GB canonical original archive mount rejected");
  // The quota is an allotment, not an assumed std::vector growth factor.
  constexpr std::size_t registryQuota=2u*originalGcParticleCapacity50KG*sizeof(OriginalGcParticleEmitterState50KG);
  reserveGbEarly(sizeof(OriginalTrackParticleCold50LA)+OriginalGcParticleManager50KG::emitterRegistryReservationMetadataBytes());
  reserveGbEarly(registryQuota+gb_particle_runtime50GB::Owner::retainedPayloadBytes());
  // Original PRT/PSM/PST extraction, hash and decoder phase scratch. Same
  // conservative four-copy reservation policy as the established TB route.
  reserveGbEarly(4u*(9385u+5022u+132124u));
  require50DU(gb_collision_world50GB::Owner::requiredPreStoragePayloadBytes(*gbColdSources,
      gbExternalPayloadReservation)<trackStorageLimits.coldPayloadBytes,
      "GB complete pre-Storage source/ground/particle reservation exceeds existing allowance");
  gbTrackParticles=std::make_shared<OriginalTrackParticleCold50LA>();OriginalTrackParticleColdDiagnostic50LA particleColdDiagnostic;
  require50DU(native_ta_particle_cold50LA::prepareGb(*gbTrackParticles,*trackSession,gbColdSources,originalMainApplication,
      domain->materials382B38(),sessionIndex+1,particleColdDiagnostic),"GB actual original particle pool rejected");
  gbParticleRuntime=std::make_shared<gb_particle_runtime50GB::Owner>();
  require50DU(gbParticleRuntime->prepare(gbParticleAuthoring,gbParticleAttachments,gbTrackParticles,
      originalMainApplication.state,nativeWaterGlobals,originalMainApplication.animationClock(),sessionIndex+1,registryQuota,
      registryQuota+gb_particle_runtime50GB::Owner::retainedPayloadBytes(),gbParticleRuntimeDiagnostic),
      "GB genuine shared Main particle runtime staging rejected");
  // PodSound (HUGEDOOR, World4D8) and the trigger actions that use it are
  // attached after the race and its counted world exist (world stage), as in
  // the original: 1A6B5C groups -> 1A6BC0 world (26FCF8 World4D8) -> sounds.
  gbCollisionWorld=std::make_shared<gb_collision_world50GB::Owner>();
  require50DU(gbCollisionWorld->prepareGround(gbColdSources,{4,16,32,64},x.assets.phy,trackSession->gridRank,
      gbExternalPayloadReservation,trackStorageLimits,error),error.empty()?"GB genuine615-placed Ground rejected":error.c_str());
  gbFogOwner=std::make_shared<gb_track_fog50GB::Owner>();require50DU(gbFogOwner->prepare(gbColdSources,error),error.c_str());
 }
}
} // namespace interactive_course_cold50DU
