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
#include "track_animation_binding50an.h"
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
#include <iostream>
#include <stdexcept>

namespace interactive_course_cold50DU {
using namespace rr::game;
namespace {
void require50DU(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
// Count the course TOB authors in [Block] Field (absent block or field = 0),
// as the original's owner loops read NumberOfAnimations / NumberOfEmitters.
// Plain decimal text only; anything else is refused, never guessed.
std::uint32_t authoredCount(const rr::assets::ParameterDocument48F& p,const char* block,const char* field){
 const rr::assets::ParameterBlock48F* found=nullptr;
 for(const auto& b:p.blocks)if(b.name==block){require50DU(!found,"GB duplicate TOB owner block");found=&b;}
 if(!found)return 0;
 const std::string* value=nullptr;
 for(const auto& f:found->fields)if(f.name==field){require50DU(!value,"GB duplicate TOB owner count");value=&f.value;}
 if(!value)return 0;
 require50DU(!value->empty()&&value->size()<=4,"GB TOB owner count text");
 std::uint32_t n=0;for(const char c:*value){require50DU(c>='0'&&c<='9',"GB TOB owner count text");n=n*10u+std::uint32_t(c-'0');}
 return n;
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
 auto& trackAnimations=this->trackAnimations;
 auto& trackAnimationBinding=this->trackAnimationBinding;
 auto& trackAnimationBindingDiagnostic=this->trackAnimationBindingDiagnostic;
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
  reserveGbEarly(gb_trigger_actions50GB::Owner::maximumAdditionalPayloadBytes());
  reserveGbEarly(original_gc_gameplay_lighting_candidate::Owner::gbTimedLightStorageReservationBytes());
  gbLightOwner=std::make_shared<original_gc_gameplay_lighting_candidate::Owner>();
  // Track animations: the course TOB's own [Animations] through the common player
  // (track_animation_binding50an.h loadForLane; absent = no runtime); the rows plan
  // the lighting owner's one visual-zone lane before its prepare.
  try{trackAnimations=track_animation_binding50AN::loadForLane(trackSession->archive,trackSession->world->code,
      gbColdSources->world().zones.size(),*gbLightOwner,s.animationShared.libm,s.animationShared.motion);}
  catch(const std::exception& e){require50DU(false,e.what());}
  reserveGbEarly(track_animation_binding50AN::lanePayloadBytes(trackAnimations.get(),gbColdSources->world().portals.size()));
  original_gc_gameplay_lighting_candidate::Diagnostic lightDiagnostic;
  require50DU(gbLightOwner->prepareGbNative(gbColdSources->world().parameters.original_bytes,raceRacerCount,lightDiagnostic),
      "GB genuine same-source timed lighting rejected");
  require50DU(gbNavigation.prepare(gbColdSources,gbNavigationDiagnostic),"GB authentic213/257 navigation source rejected");
  // Triggers: the course TOB's own set through the common reader into the
  // capacity store (0 authored = no trigger state; stock 18).
  gbTriggerAuthoring=std::make_shared<gb_trigger_authoring50GB::Owner>();std::string error;
  require50DU(gbTriggerAuthoring->decode(gbColdSources,error),error.empty()?"GB course triggers rejected":error.c_str());
  // Every remaining course owner exists exactly when the data authors it
  // (absent = none). The present owners are GB's stock-shaped ones.
  const auto& tob=gbColdSources->triggerParameters();
  const auto animations=trackAnimations?trackAnimations->rowCount():0u;
  const auto emitters=authoredCount(tob,"particleemitters","numberofemitters");
  const bool stockTob=gbTriggerAuthoring->stockTob();
  // [ParticleEmitters] is read by the common emitter reader inside
  // gb_particle_authoring50gb.h; that owner refuses (PORT STAND-IN, flagged)
  // a layout the native GB emitter chain does not implement.
  std::cerr<<"gb_course_owner owner=animations present="<<(animations?1:0)<<" count="<<animations<<" stock="<<(stockTob?1:0)<<'\n';
  std::cerr<<"gb_course_owner owner=emitters present="<<(emitters?1:0)<<" count="<<emitters<<" stock="<<(stockTob?1:0)<<'\n';
  if(trackAnimations){
   trackAnimationBinding=std::make_unique<track_animation_binding50AN::Binding>();
   require50DU(trackAnimationBinding->beginVisual(trackAnimations,gbLightOwner,trackAnimationBindingDiagnostic),
       trackAnimationBindingDiagnostic.message.empty()?"track animation visual prefix rejected":trackAnimationBindingDiagnostic.message.c_str());
  }
  gbOriginalWorld=std::make_shared<gb_original_world50LM::Owner>();gb_original_world50LM::Diagnostic worldDiagnostic;
  require50DU(gbOriginalWorld->load(gbColdSources,worldDiagnostic),worldDiagnostic.message.empty()?"GB original static world rejected":worldDiagnostic.message.c_str());
  if(emitters){
   gbParticleAuthoring=std::make_shared<gb_particle_authoring50GB::Owner>();gb_particle_authoring50GB::Diagnostic particleAuthoringDiagnostic;
   require50DU(gbParticleAuthoring->decode(gbColdSources,trackStorageLimits.coldPayloadBytes-gbExternalPayloadReservation-gbColdSources->retainedPayloadBytes(),
       particleAuthoringDiagnostic),particleAuthoringDiagnostic.message.empty()?"GB emitter authoring rejected":particleAuthoringDiagnostic.message.c_str());
   reserveGbEarly(gbParticleAuthoring->retainedPayloadBytes());
   reserveGbEarly(gb_particle_attachments50GB::Owner::maximumAdditionalPayloadBytes());
   gbParticleAttachments=std::make_shared<gb_particle_attachments50GB::Owner>();gb_particle_attachments50GB::Diagnostic attachmentsDiagnostic;
   require50DU(gbParticleAttachments->prepare(gbParticleAuthoring,gbOriginalWorld,
       gb_particle_attachments50GB::Owner::maximumAdditionalPayloadBytes(),attachmentsDiagnostic),"GB actual55 attachment graphs rejected");
  }
  const auto mountReservation=gbColdSources->canonicalMountReservationBytes();reserveGbEarly(mountReservation);
  rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic mounted;
  rr::assets::OriginalStandardTextureDiagnostic50LA textureDiagnostic;
  require50DU(gbColdSources->mountCanonicalArchive(*provider,mountReservation,mounted,
      native_course_archive_release50PD::holder().begin(provider,domain->resolver3E1A70()))&&provider->sealNativeMounts(mounted)&&
      domain->resolver3E1A70()->bindProvider3E1A70(provider,textureDiagnostic),"GB canonical original archive mount rejected");
  // The quota is an allotment, not an assumed std::vector growth factor.
  constexpr std::size_t registryQuota=2u*originalGcParticleCapacity50KG*sizeof(OriginalGcParticleEmitterState50KG);
  reserveGbEarly(sizeof(OriginalTrackParticleCold50LA)+OriginalGcParticleManager50KG::emitterRegistryReservationMetadataBytes());
  if(emitters)reserveGbEarly(registryQuota+gb_particle_runtime50GB::Owner::retainedPayloadBytes());
  // Original PRT/PSM/PST extraction, hash and decoder phase scratch. Same
  // conservative four-copy reservation policy as the established TB route,
  // charged from the SCB-named members' own sizes (stock demoprt:
  // 9385 + 5022 + 132124 bytes; prepareGb refuses a missing member).
  {std::size_t particleBytes=0;const std::string* particleName=nullptr;
   for(const auto& b:gbColdSources->world().parameters.blocks)if(b.name=="environment")
    for(const auto& f:b.fields)if(f.name=="particletexturemapname")particleName=&f.value;
   if(particleName){auto stem=*particleName;const auto dot=stem.find_last_of('.');if(dot!=std::string::npos)stem.resize(dot);
    for(const char* extension:{".prt",".psm",".pst"})for(const auto& e:gbColdSources->archive().entries())
     if(rr::assets::OriginalStandardArchiveProvider50LA::sameName(e.name,"particle\\"+stem+extension)){
      require50DU(e.bytes<=8u*1024u*1024u,"GB particle member exceeds the shared member cap");particleBytes+=e.bytes;}}
   reserveGbEarly(4u*particleBytes);}
  const std::size_t liveTriggers=gbTriggerAuthoring->liveCount();
  const std::size_t liveAnimations=track_animation_world50AN::Lane::collisionRowCount(trackAnimations.get());
  require50DU(gb_collision_world50GB::Owner::requiredPreStoragePayloadBytes(*gbColdSources,
      gbExternalPayloadReservation,liveTriggers,liveAnimations)<trackStorageLimits.coldPayloadBytes,
      "GB complete pre-Storage source/ground/particle reservation exceeds existing allowance");
  gbTrackParticles=std::make_shared<OriginalTrackParticleCold50LA>();OriginalTrackParticleColdDiagnostic50LA particleColdDiagnostic;
  require50DU(native_ta_particle_cold50LA::prepareGb(*gbTrackParticles,*trackSession,gbColdSources,originalMainApplication,
      domain->materials382B38(),sessionIndex+1,particleColdDiagnostic),"GB actual original particle pool rejected");
  if(emitters){
   gbParticleRuntime=std::make_shared<gb_particle_runtime50GB::Owner>();
   require50DU(gbParticleRuntime->prepare(gbParticleAuthoring,gbParticleAttachments,gbTrackParticles,
       originalMainApplication.state,nativeWaterGlobals,originalMainApplication.animationClock(),sessionIndex+1,registryQuota,
       registryQuota+gb_particle_runtime50GB::Owner::retainedPayloadBytes(),gbParticleRuntimeDiagnostic),
       "GB genuine shared Main particle runtime staging rejected");
  }
  // PodSound (HUGEDOOR, World4D8) and the trigger actions that use it are
  // attached after the race and its counted world exist (world stage), as in
  // the original: 1A6B5C groups -> 1A6BC0 world (26FCF8 World4D8) -> sounds.
  gbCollisionWorld=std::make_shared<gb_collision_world50GB::Owner>();
  require50DU(gbCollisionWorld->prepareGround(gbColdSources,{4,16,32,64},x.assets.phy,trackSession->gridRank,
      gbExternalPayloadReservation,trackStorageLimits,liveTriggers,liveAnimations,error),error.empty()?"GB placed Ground rejected":error.c_str());
  gbFogOwner=std::make_shared<gb_track_fog50GB::Owner>();require50DU(gbFogOwner->prepare(gbColdSources,error),error.c_str());
 }
}
} // namespace interactive_course_cold50DU
