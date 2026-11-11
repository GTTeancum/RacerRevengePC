// Authentic RA decode/authoring/query-storage prefix, original owners retained.
#include "ra_application_cold50la.h"
#include "original_ui_main_entry50kg.h"
#include "ra_cold_roster_operation50la.h"
#include "ra_cold_aggregate50la.h"
#include "original_consumed_ra_stage50du.h"
#include "../../src/game/original_racer_model_view50la.h"
#include "runtime_startup50fl_fixture.h"
#include "track_session_assets50ig_fixture.h"
#include "track_static_candidates50ig_fixture.h"
#include "undertrack_owners50ew_fixture.h"
#include "terrain_probe_authoring50fq_fixture.h"
#include "runtime_allocation_counter50fl.h"
#include <iostream>
#include <stdexcept>
#include <string>
namespace interactive_ra_cold50DU {
using namespace rr::game;
namespace {
void require50DU(bool ok, const char* message) {
    if(!ok){std::cerr<<"require_failed "<<(message?message:"")<<std::endl;throw std::runtime_error(message);}
}
}
void Context::run() const {
 auto& raWorld=this->raWorld;
 auto& raDiagnostic=this->raDiagnostic;
 auto& query=this->query;
 auto& qfilter=this->qfilter;
 auto& raRosterOwners=this->raRosterOwners;
 auto& originalAiHumanCount=this->originalAiHumanCount;
 auto& raceRacerCount=this->raceRacerCount;
 auto& s=this->s;
 auto& trackSession=this->trackSession;
 auto& consumedLaunch=this->consumedLaunch;
 auto& allAiEnabled=this->allAiEnabled;
 auto& sourceConsumedIntroActivation=this->sourceConsumedIntroActivation;
 auto& originalCameraEnabled=this->originalCameraEnabled;
 const auto& openNativeVideo=this->openNativeVideo;
 auto& originalMainApplication=this->originalMainApplication;
 auto& nativeVideo=this->nativeVideo;
 auto& sourceModelRenderGlobals=this->sourceModelRenderGlobals;
 auto& nativeOrdinaryLaunch11258=this->nativeOrdinaryLaunch11258;
 auto& consumedRaStage=this->consumedRaStage;
 auto& sessionIndex=this->sessionIndex;
 auto& playerRacer60=this->playerRacer60;
 auto& sourceConsumedPauseActivation=this->sourceConsumedPauseActivation;
 auto& recoveryQueryOwners=this->recoveryQueryOwners;
 auto& recoveryGlobals=this->recoveryGlobals;
 auto& terrainOwner=this->terrainOwner;
 auto& movement=this->movement;
 const auto& publishNativeRuntime=this->publishNativeRuntime;
{
  raWorld=std::make_shared<ra_cold_aggregate50LA::Owner>();
  track_static_candidates50IG::Bindings raNames{};
  raNames.firstIdentity=0x10000000u;
  raNames.collisionTypeTag=query.collisionTypeTag;raNames.groupTypeTag=query.groupTypeTag;
  raNames.preservedFlags31=static_cast<std::uint8_t>(qfilter.flags31&0xf0u);
  raNames.preservedFlagsF8=static_cast<std::uint8_t>(qfilter.flagsF8&0xc0u);
  std::string error;
  auto roster=ra_cold_roster50LA::Resources::adopt(raRosterOwners,error,originalAiHumanCount,raceRacerCount);
  require50DU(bool(roster),error.empty()?"RA actual eight-racer source adoption rejected":error.c_str());
  const bool raColdReady=raWorld->decodeSources(runtime_startup50fl::readBytes(trackSession->archive),
      s.animationShared.libm,{4,16,32,64},raDiagnostic)&&
      raWorld->prepareStaticCandidates(raNames,raDiagnostic)&&
      raWorld->prepareAuthoring(raDiagnostic)&&raWorld->prepareNativeTriggers(raDiagnostic)&&
      raWorld->prepareLighting(raceRacerCount,raDiagnostic);
  // Compiled real prefix initializer, intentionally not invoked until the
  // remaining slot30 load, scheduler/parent and retirement joins are reviewed.
  [[maybe_unused]] auto beginConsumedRaPrefix=[&](){
   if(!consumedLaunch||(!allAiEnabled&&!sourceConsumedIntroActivation)||!originalCameraEnabled)return false;
   openNativeVideo();
   originalMainApplication.appStart(originalMainApplication.assetRoot);
   originalMainApplication.enterGameplayMaterialDirectory17D7A0();
   const auto context=nativeVideo->standardMaterialConstructionContext(nativeVideo,originalMainApplication);
   const auto domain=originalMainApplication.standardMaterialDomain();
   if(!context||!domain||!domain->prepared())return false;
   if(!sourceModelRenderGlobals)sourceModelRenderGlobals=
       std::make_unique<OriginalRacerModelRenderGlobals50LA>(nativeOrdinaryLaunch11258);
   rr::assets::OriginalStandardMaterialDiagnostic50LA d;std::uint32_t pc=0;
   const bool ready=consumedRaStage.begin(*consumedLaunch,sessionIndex+1,playerRacer60,
       *sourceModelRenderGlobals,raWorld,d,pc);
   if(!ready)std::cerr<<"consumed_ra_prefix_rejected source_pc="<<pc
       <<" material_pc="<<d.sourcePC<<" material_status="<<static_cast<unsigned>(d.status)<<'\n';
   return ready;
  };
  if(raColdReady&&(sourceConsumedPauseActivation||sourceConsumedIntroActivation))require50DU(beginConsumedRaPrefix(),
      "actual consumed RA constructor prefix rejected");
  const bool raStorageReady=raColdReady&&
      raWorld->prepareRuntimeRosterQueryStorage(originalMainApplication.entities(),roster,&recoveryQueryOwners,recoveryGlobals,
          terrainOwner,movement,{},raDiagnostic);
  if(!raStorageReady){
   std::cerr<<"RA cold failure stage="<<unsigned(raDiagnostic.stage)
       <<" source="<<(raDiagnostic.source.failure?raDiagnostic.source.failure:"")
       <<" detail="<<raDiagnostic.source.message
       <<" trigger="<<(raDiagnostic.triggers.failure?raDiagnostic.triggers.failure:"")
       <<" query="<<(raDiagnostic.runtimeQueries.message?raDiagnostic.runtimeQueries.message:"")
       <<" sky="<<raDiagnostic.sky<<'\n';
   require50DU(false,raDiagnostic.failure?raDiagnostic.failure:"RA same-world cold reservation failed");
  }
  publishNativeRuntime();
 }
}
}
