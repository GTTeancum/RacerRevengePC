// Cold stage bodies retain their original order and borrowed execution arena.
#include "native_last_failure50pd.h"
#include "terrain_interactive_cold_owners50du.h"
#include "original_ui_main_entry50kg.h"
#include "runtime_startup50fl_fixture.h"
#include "terrain_interactive_storage50du.h"
#include "native_racer_bank50hn_fixture.h"
#include "native_racer_observation50iz_fixture.h"
#include "native_ai_session50iv_fixture.h"
#include "native_racer_rig50hw_fixture.h"
#include "original_racer_visual50hw_fixture.h"
#include "native_racer_cables50hw_fixture.h"
#include "native_racer_model_lifetime50la_fixture.h"
#include "ra_native_session50la.h"
#include "original_consumed_ra_stage50du.h"
#include "original_position_stats_prestage50la.h"
#include "original_pause_cold_driver50la.h"
#include "gb_animation_binding50gb.h"
#include "mc_animation_binding50mc.h"
#include "mb_collision_world50mb.h"
#include "sc_runtime_ground50la.h"
#include "terrain_probe_authoring50fq_fixture.h"
#include "live_terrain_probe50fr_fixture.h"
#include "undertrack_owners50ew_fixture.h"
#include "original_ai_physical_recovery.h"
#include "original_human_physical_recovery.h"
#include "lap_drive50gj_fixture.h"
#include "checkpoint_recovery50fi_fixture.h"
#include "opponent_engine_audio50hx_fixture.h"
#include "../../src/game/dynamic_impact_events50iw.h"
#include "../../src/game/original_shared_rng50iu.h"
#include "../../src/game/original_human_reset_timers50lc.h"
#include "../../src/game/host_course_driver50gk.h"
#include "../../src/game/controller_actions50ec.h"
#include "../../src/game/controller_ingress50cx.h"
#include "../../src/game/reset_effects50bk.h"
#include "../../src/game/pod_scene_rebase50fb.h"
#include "../../src/game/wrong_way_progress50go.h"
#include "../../src/game/pod_live_query50iz.h"
#include "../../platform/linux/native/keyboard_packet50dr.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace interactive_cold50DU {
namespace {
void require50DU(bool ok,const char* message) {
    if(!ok){native_last_failure50PD::record(message);std::cerr<<"require_failed "<<(message?message:"")<<std::endl;throw std::runtime_error(message);}
}
template<class Operation>
decltype(auto) coldRacerStage50DU(const std::string& podId,const char* stage,
    Operation&& operation) {
    try { return operation(); }
    catch(const std::exception& error) {
        std::throw_with_nested(std::runtime_error(
            "Original racer "+podId+" ["+stage+"]: "+error.what()));
    }
    catch(...) {
        std::throw_with_nested(std::runtime_error(
            "Original racer "+podId+" ["+stage+"]: non-standard exception"));
    }
}
constexpr std::uint32_t raceRacerCapacity=rr::game::defaultHostRaceRacers50HT;
} // namespace

void OwnersContext::run() {
  contactBindings[0]={racerCollisions[0].get(),settings,response};
  for(std::size_t racer=0;racer<raceRacerCount;++racer){
   racerContactIds[racer]=static_cast<std::uint32_t>(racer+1);
   // Empty physical-root material0 is explicit HOST policy for every racer.
   racerContactMaterials[racer]=native_racer_contact50HW::emptyPhysicalRootMaterial;
  }
#include "native_racer_owners50hn.inc"
  for(auto& membership:gcAnimatedMembership)if(membership){
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  for(auto& membership:gaAnimatedMembership)if(membership){
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  for(auto& membership:gaTriggerMembership)if(membership){
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  for(auto& membership:scTriggerMembership)if(membership){
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  for(auto& membership:maTriggerMembership)if(membership){
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  for(auto& membership:saTriggerMembership)if(membership){
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  for(auto& membership:gcTriggerMembership)if(membership){
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  // Actual authored trigger groups need stable, distinct main/secondary
  // identities. Only these scalar IDs are banked; shared reciprocal contacts,
  // Gate fired/audio state and action counters never roll back on arena visits.
  if(gcTriggerActions||isGaTrack||isMaSaTrack||isRaTrack||isMbTrack||isScTrack||isTcTrack||isRcTrack||isTbTrack||isGbTrack||isMcTrack){racerBank.owner(qfilter.identity);racerBank.owner(secondaryFilter.identity);}
  if(isRaTrack)require50DU(raWorld->registerRuntimeQueryWorldOwners(racerBank,raDiagnostic),
      raDiagnostic.failure?raDiagnostic.failure:"RA world registration rejected");
  if(isMbTrack)for(std::size_t i=0;i<3;++i){
   auto* membership=mbCollisionWorld->triggerMembership(i);
   require50DU(membership,"MB actual trigger membership backing missing");
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  if(isTcTrack)for(std::size_t i=0;i<tc_trigger_authoring50TC::count;++i){
   auto* membership=tcWorldBorrow->triggerMembership(i);
   require50DU(membership,"TC actual trigger membership backing missing");
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  if(isRcTrack)for(std::size_t i=0;i<rc_trigger_authoring::count;++i){
   auto* membership=rcWorldBorrow->triggerMembership(i);
   require50DU(membership,"RC actual trigger membership backing missing");
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  if(isTbTrack)for(std::size_t i=0;i<tb_trigger_authoring50TB::count;++i){
   auto* membership=tbWorldBorrow->triggerMembership(i);
   require50DU(membership,"TB actual trigger membership backing missing");
   racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
   racerBank.elements(membership->entries.data(),membership->entries.size());
  }
  if(isMcTrack){
   for(std::size_t i=0;i<21;++i){auto* membership=mcWorldBorrow->triggerMembership(i);
    require50DU(membership,"MC actual trigger membership backing missing");
    racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
    racerBank.elements(membership->entries.data(),membership->entries.size());}
   const bool enrolled=mcAnimationBinding->enrollBank(racerBank,mcAnimationBindingDiagnostic);
   require50DU(enrolled,mcAnimationBindingDiagnostic.message.empty()?"MC real complete Bank animation enrollment rejected":mcAnimationBindingDiagnostic.message.c_str());
  }else if(isGbTrack){
   for(std::size_t i=0;i<18;++i){auto* membership=gbWorldBorrow->triggerMembership(i);
    require50DU(membership,"GB actual trigger membership backing missing");
    racerBank.owner(membership->state);racerBank.elements(membership->links.data(),membership->links.size());
    racerBank.elements(membership->entries.data(),membership->entries.size());}
   const bool enrolled=gbAnimationBinding->enrollBank(racerBank,gbAnimationBindingDiagnostic);
   require50DU(enrolled,gbAnimationBindingDiagnostic.message.empty()?"GB real complete Bank animation enrollment rejected":gbAnimationBindingDiagnostic.message.c_str());
  }
  racerBank.seal(raceRacerCount);
  if(isMcTrack){const bool activated=mcAnimationBinding->activateSealedBank(mcAnimationBindingDiagnostic);
   require50DU(activated,mcAnimationBindingDiagnostic.message.empty()?"MC actual sealed eight-peer animation backing rejected":mcAnimationBindingDiagnostic.message.c_str());}else if(isGbTrack){const bool activated=gbAnimationBinding->activateSealedBank(gbAnimationBindingDiagnostic);
   require50DU(activated,gbAnimationBindingDiagnostic.message.empty()?"GB actual sealed eight-peer animation backing rejected":gbAnimationBindingDiagnostic.message.c_str());}
  if(originalAiEnabled){
   // Handles bind registered locations only. Actual values are read afresh
   // after every native step; no per-world peer cache or arena swap is used.
   racerFrameReads.bind(racerBank,s);
   originalAiReads.bind(racerBank,x.bindings,navigationProgress,
       lapContinuity.state.vehicleCC8,lapContinuity.timing.wordCD4,
       originalAi,originalAiReceipt,x.diagnostic);
  }
  if(isRaTrack){
   require50DU(originalMainApplication.enabled,"RA requires the actual original Application");
   openNativeVideo();
   originalMainApplication.appStart(originalMainApplication.assetRoot);
   originalMainApplication.enterGameplayMaterialDirectory17D7A0();
   const auto raMaterialContext=nativeVideo->standardMaterialConstructionContext(
       nativeVideo,originalMainApplication);
   // Stage every HUD source/reference before RA captures its real provider
   // revision and performs the single combined final material population.
   require50DU(originalMainApplication.stageSpeedBoostNativeCold50LA(
       originalMainApplication.assetRoot,sessionIndex+1,sourceSpeedBoost),"RA Speed/Boost source staging failed");
   require50DU(originalMainApplication.stageTimeStatsNativeCold50LA(
       originalMainApplication.assetRoot,sessionIndex+1,sourceTimeStats,true),"RA TimeStats source staging failed");
   if(allAiEnabled||humanCourseEnabled)require50DU(originalMainApplication.stageDamageNativeCold50LA(
       originalMainApplication.assetRoot,sessionIndex+1,sourceDamage),"RA Damage source staging failed");
   require50DU(sourcePositionStatsPrestage.stage(originalMainApplication,
       originalMainApplication.assetRoot,raMaterialContext,sourcePositionStatsPrestageDiagnostic),
       sourcePositionStatsPrestageDiagnostic.failure?sourcePositionStatsPrestageDiagnostic.failure:
       "RA PositionStats native source preload rejected");
   // Cold-only human loading: never changes spectator live Pause/audio/input
   // policy. The same consumed RA Tournament owner already exists for intro.
   const bool humanResultsCold=canonicalTournamentGrid&&humanCourseEnabled&&!allAiEnabled&&
       bool(consumedRaStage.race());
   if((sourceConsumedPauseActivation||humanResultsCold)&&consumedRaStage.race())
    require50DU(originalMainApplication.prewarmPauseCold50LA(sourcePause),
       "actual Pause canonical prewarm rejected");
   {const bool mounted=raWorld->mountRetainedRuntimeRosterArchives(originalMainApplication,raDiagnostic);
    if(!mounted)std::cerr<<"ra_roster_mount_rejected failure="<<(raDiagnostic.failure?raDiagnostic.failure:"")
        <<" slot="<<raDiagnostic.rosterSlot<<" mount_status="<<static_cast<unsigned>(raDiagnostic.rosterMount.status)
        <<" mount="<<raDiagnostic.rosterMount.mount<<" ordinal="<<raDiagnostic.rosterMount.ordinal<<'\n';
    require50DU(mounted,raDiagnostic.failure?raDiagnostic.failure:"RA roster archive mounting rejected");}
   std::array<std::vector<std::uint8_t>,6> crowdImages;
   if(!runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty())
    require50DU(raWorld->decodeCrowdImages(crowdImages,raDiagnostic),
        raDiagnostic.failure?raDiagnostic.failure:"direct original RA crowd decode rejected");
   else for(std::size_t i=0;i<crowdImages.size();++i)crowdImages[i]=runtime_startup50fl::readBytes(
       "evidence/ra-crowd-original/crowd-frame"+std::to_string(i)+"-mip4.rrtex");
   raSession=std::make_unique<ra_native_session50LA::Join>(raWorld,originalMainApplication,racerBank,sessionIndex+1);
   const ra_cold_roster50LA::Arena arena{x,
       {boostParameters,boostCoolingCF8,settings,response,secondarySettings,secondaryResponse},
       selection,originalAi,originalAiSession,navigationProgress,lapContinuity,poseHistory,
       rebaseModels,rebaseModelCount,rebaseWorkspace,rebaseBounds,rebaseDiagnostic,namesForEffects,
       nativeEffects,nativeEffectCount,resetEffectBindings,resetEffectState,resetEffectWorkspace,
       originalRecoveryModels,terrainBindings,coldFlame,nativeOrdinaryLaunch11258,
       originalAiHumanCount,&authored,&authoredFinishSteering10A4,
       humanCourseEnabled&&!allAiEnabled?
           ra_cold_roster50LA::NativePlayerLoadCaller50LA(racerBank,controller,controllerActions,
               controllerEvents,applicationEvents,playerEventChild,playerEvents,playerEnvironment,
               authored,authoredFinishSteering10A4,actualRacer60ForResults,nativeHumanAudio,
               nativeHumanSpeechCounts,speechState):ra_cold_roster50LA::NativePlayerLoadCaller50LA{}};
   const bool raSessionReady=humanResultsCold?
       raSession->constructHumanTournamentCold(arena,raFreshProbe,{endpoints,endpointScenes,movementSource},
           raMaterialContext,crowdImages,raSessionDiagnostic,nativeRosterOwners,
           *consumedRaStage.race(),sourcePause,actualRacer60ForResults):
       raSession->construct(arena,raFreshProbe,{endpoints,endpointScenes,movementSource},
           raMaterialContext,crowdImages,raSessionDiagnostic,nativeRosterOwners,
           consumedRaStage.race().get(),sourceConsumedPauseActivation&&consumedRaStage.race()?&sourcePause:nullptr,
           canonicalTournamentGrid);
   if(!raSessionReady){
    auto text=[](const char* p){return p?p:"";};
    std::cerr<<"RA session failure operation="<<text(raSessionDiagnostic.operation)
      <<" aggregate="<<text(raSessionDiagnostic.aggregate.failure)
      <<" world="<<text(raSessionDiagnostic.aggregate.world.failure)
      <<" static_models="<<text(raSessionDiagnostic.aggregate.staticModels.failure)
      <<" roster="<<text(raSessionDiagnostic.roster.failure)
      <<" roster_stage="<<text(raSessionDiagnostic.roster.stage)
      <<" slot="<<raSessionDiagnostic.roster.slot
      <<" detail="<<raSessionDiagnostic.roster.detail
      <<" results="<<text(raSessionDiagnostic.results.operation)
      <<" results_pc="<<std::hex<<raSessionDiagnostic.results.sourcePC<<std::dec
      <<" results_status="<<static_cast<unsigned>(raSessionDiagnostic.results.status)<<'\n';
    const auto& material=raSessionDiagnostic.aggregate.world.material;
    rr::assets::OriginalStandardMaterialRowView50LA materialRow;
    rr::assets::OriginalStandardTextureRowView50LA textureRow;
    const auto failureDomain=originalMainApplication.standardMaterialDomain();
    const bool hasMaterial=failureDomain->materials382B38()->row(material.row,materialRow);
    const bool hasTexture=material.texture.row>=0&&material.texture.row<=32767&&
        failureDomain->textures383400()->row(static_cast<std::int16_t>(material.texture.row),textureRow);
    std::cerr<<"ra_population_diagnostic status="<<static_cast<unsigned>(material.status)
        <<" pc="<<material.sourcePC<<" row="<<material.row
        <<" name="<<(hasMaterial&&materialRow.name00?*materialRow.name00:std::string{})
        <<" field="<<text(material.field)<<" pass="<<material.pass
        <<" texture_status="<<static_cast<unsigned>(material.texture.status)
        <<" texture_pc="<<material.texture.sourcePC<<" texture_row="<<material.texture.row
        <<" texture_name="<<(hasTexture&&textureRow.name00?*textureRow.name00:std::string{})
        <<" texture_offset="<<material.texture.byteOffset<<'\n';
    require50DU(false,raSessionDiagnostic.failure?raSessionDiagnostic.failure:"RA actual cold session construction failed");
   }
   require50DU(originalMainApplication.finishTimeStatsNativeCold50LA(sourceTimeStats),
       "RA TimeStats final shared material verification failed");
   if(sourceDamage.owner)require50DU(originalMainApplication.finishDamageNativeCold50LA(sourceDamage),
       "RA Damage final shared material verification failed");
   require50DU(sourcePositionStatsPrestage.verify(sourcePositionStatsPrestageDiagnostic),
       "RA PositionStats preloaded canonical sources changed during construction");
   require50DU(originalMainApplication.finishSpeedBoostNativeCold50LA(sourceSpeedBoost),
       "RA Speed/Boost final shared material verification failed");
   if(humanCourseEnabled)racerBank.visit(0,[&](){initializeOriginalHumanResetTimers50LC(playerEvents);});
   contactBindings=nativeRosterOwners->contacts;
   for(std::size_t slot=0;slot<racerBank.count();++slot)racerNames[slot]=racerVisuals[slot]->displayName;
  }
  // Alternate camera selections and every all-AI slot use their actual
  // original-resource cold path before any tick. Only the legacy mixed POD01
  // selection retains the fixture startup. No live identity is flipped.
  if(isRcTrack)requireMbRuntimeNavigation50MB();
  if(!isRaTrack)for(std::size_t racer=playerUsesRosterRig?0u:1u;racer<racerBank.count();++racer)racerBank.visit(racer,[&](){
   const auto& rig=*racerRigs[racer];const auto& collision=*racerCollisions[racer];
   coldRacerStage50DU(rig.podId,"rig installation",[&](){
    require50DU(completedFrames==0&&m.frameFactor==0.f,"original racer installation requires fresh state");
    if(nativeModelDomain){
     nativeRightModels[racer]=std::make_unique<native_racer_model50LA::Owner>();
     nativeRightModels[racer]->prepareEarly(rig,racer,sessionIndex+1,racerBank,x,
         originalMainApplication.entities(),nativeModelDomain,originalMainApplication.motionLibrary4333C0(),
         originalMainApplication.modelDirectory36B860());
     nativeLeftModels[racer]=std::make_unique<native_racer_model50LA::Owner>();
     nativeLeftModels[racer]->prepareEarly(rig,racer,sessionIndex+1,racerBank,x,
         originalMainApplication.entities(),nativeModelDomain,originalMainApplication.motionLibrary4333C0(),
         originalMainApplication.modelDirectory36B860(),native_racer50HW::Rig::left);
     if(rig.modelCount==5){
      nativeBottomRightModels[racer]=std::make_unique<native_racer_model50LA::Owner>();
      nativeBottomRightModels[racer]->prepareEarly(rig,racer,sessionIndex+1,racerBank,x,
          originalMainApplication.entities(),nativeModelDomain,originalMainApplication.motionLibrary4333C0(),
          originalMainApplication.modelDirectory36B860(),native_racer50HW::Rig::bottomRight);
      nativeBottomLeftModels[racer]=std::make_unique<native_racer_model50LA::Owner>();
      nativeBottomLeftModels[racer]->prepareEarly(rig,racer,sessionIndex+1,racerBank,x,
          originalMainApplication.entities(),nativeModelDomain,originalMainApplication.motionLibrary4333C0(),
          originalMainApplication.modelDirectory36B860(),native_racer50HW::Rig::bottomLeft);
     }
     nativeChariotModels[racer]=std::make_unique<native_racer_model50LA::Owner>();
     nativeChariotModels[racer]->prepareEarly(rig,racer,sessionIndex+1,racerBank,x,
         originalMainApplication.entities(),nativeModelDomain,originalMainApplication.motionLibrary4333C0(),
         originalMainApplication.modelDirectory36B860(),native_racer50HW::Rig::chariot);
     nativeRightModels[racer]->bindSibling(*nativeLeftModels[racer]);
     nativeChariotModels[racer]->bindChariotSiblings(*nativeRightModels[racer],*nativeLeftModels[racer],
         nativeBottomRightModels[racer].get(),nativeBottomLeftModels[racer].get());
     rig.install(s,x.bindings,x.handRecords,nativeRightModels[racer].get(),nativeLeftModels[racer].get(),
         nativeChariotModels[racer].get(),nativeBottomRightModels[racer].get(),nativeBottomLeftModels[racer].get(),isGbTrack||isMcTrack);
    }else if(isGbTrack||isMcTrack)rig.install(s,x.bindings,x.handRecords,nullptr,nullptr,nullptr,nullptr,nullptr,true);
    else rig.install(s,x.bindings,x.handRecords);
   });
   auto racerSelection=selection;
   // The actual human preset/configuration belongs to slot0. AI construction
   // uses its genuine mode58 branch and independent cold binding.
   if(!allAiEnabled){
    if(racer!=0)racerSelection.configurationEntryNonzero=true;
    x.configurationEntryNonzero=racerSelection.configurationEntryNonzero;
   }
   if(originalAiOwnsSlot(racer))racerSelection.mode58Nonzero=true;
   const auto receipt=coldRacerStage50DU(rig.podId,"handling bind",[&](){
    return native_racer50HW::bindColdHandling(rig.phy,rig.hierarchy[1],
        rig.hierarchy[0],rig.geometry,collision.mainTarget().source,racerSelection,x,
        {boostParameters,boostCoolingCF8,settings,response,secondarySettings,secondaryResponse},
        rig.modelCount==5?&rig.hierarchy[native_racer50HW::Rig::bottomRight]:nullptr,
        rig.modelCount==5?&rig.hierarchy[native_racer50HW::Rig::bottomLeft]:nullptr,
        nativeChariotModels[racer].get());
   });
   if(originalAiOwnsSlot(racer)){
    rr::assets::ResArchive48C source;rr::assets::DecodeError48C error;std::vector<std::uint8_t> profileText;
    const auto sourceBytes=runtime_startup50fl::readBytes(rig.resourceFile);
    std::string lowerPod=rig.podId;for(char&c:lowerPod)c=static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    const std::string member="Pods\\"+lowerPod+"\\"+lowerPod+".ai";
    require50DU(rr::assets::decodeResource48C(sourceBytes,source,error)&&
        rr::assets::extractResMember48C(source,member.c_str(),profileText,error),"selected original racer .ai member unavailable");
    AiProfile50IU profile{};
    require50DU(rr::assets::readAiProfile50IU(reinterpret_cast<const char*>(profileText.data()),profileText.size(),
        profile,originalAiAuthoringDiagnostic),"selected original AI profile rejected");
    // The immutable fresh selected course supplies creation class/record pod.
    // TA/SA/MA/GA fresh orders0..3 select class1, overwriting RepairTime1064
    // to2 AFTER the real pod load; GC ID10/order10/class3 preserves it.
    const auto& createSelection=originalAiCreateSelection;
    require50DU(createSelection.bound&&applyAiCreateClass50IU(createSelection.actualRaceClass,profile),
        "selected original AI create class unavailable");
    const auto* identity=original_racer_selection50IJ::findByPodId(rig.podId.c_str());
    require50DU(identity,"original AI racer60 identity unavailable");
    AiCreateIdentity50IU createIdentity{};
    require50DU(bindAiCreateIdentity50IU(static_cast<std::uint32_t>(racer-originalAiHumanCount),originalAiHumanCount,
        static_cast<std::int32_t>(identity->racer60),createSelection.freshCourseRecordPod11280,createIdentity),
        "original AI create identity unavailable");
    initializeAiRuntime50IV(createIdentity.ordinalFE0,identity->racer60,
        static_cast<std::uint32_t>(createSelection.freshCourseRecordPod11280),
        profile,receipt.authored.engineDifference5C,originalAi);
    require50DU(originalAi.fieldFE4==createIdentity.courseRecordMatchFE4,
        "selected course-record identity differs from original AI FE4");
    originalAiSession.packingReferences[racer]=-1;
    if(isMcTrack){
     requireMbRuntimeNavigation50MB();
     AiColdTrackBindingsView50MC tracks{};
     require50DU(bindAiColdTracks50IU(mcMapView,mcAiView,mcMapMetadata,
         mcAiMetadata,navigationProgress,tracks),"original MC AI cold track owners unavailable");
    }else if(isGbTrack){
     requireMbRuntimeNavigation50MB();
     AiColdTrackBindingsView50GB tracks{};
     require50DU(bindAiColdTracks50IU(gbMapView,gbAiView,gbMapMetadata,
         gbAiMetadata,navigationProgress,tracks),"original GB AI cold track owners unavailable");
    }else if(isTbTrack){
     requireMbRuntimeNavigation50MB();
     AiColdTrackBindingsView50TB tracks{};
     require50DU(bindAiColdTracks50IU(tbMapView,tbAiView,tbMapMetadata,
         tbAiMetadata,navigationProgress,tracks),"original TB AI cold track owners unavailable");
    }else if(isTcTrack){
     requireMbRuntimeNavigation50MB();
     AiColdTrackBindingsView50TC tracks{};
     require50DU(bindAiColdTracks50IU(tcMapView,tcAiView,tcMapMetadata,
         tcAiMetadata,navigationProgress,tracks),"original TC AI cold track owners unavailable");
    }else if(isMbTrack){
     AiColdTrackBindingsView50MB tracks{};
     require50DU(bindAiColdTracks50IU(mbMapView,mbAiView,mbMapMetadata,
         mbAiMetadata,navigationProgress,tracks),"original MB AI cold track owners unavailable");
    }else{
     AiColdTrackBindings50IU tracks{};
     require50DU(bindAiColdTracks50IU(*navigationMap,*navigationAI,selectedOriginalAiMapMetadata,
         selectedOriginalAiMetadata,navigationProgress,tracks),"original AI cold track owners unavailable");
    }
    s.fields.vehicle58=1u;s.fields.nativeAiOwnersBound50IV=true;s.fields.applicationByte58=0u;
    x.parameters.rates.x=4.f;
    initializeLapTimingFields50ID(lapContinuity.timing,dynamicContactEnabled?dynamicMusicProfile.mode11258:2u);lapContinuity.timingInitialized=true;
   }
   coldRacerStage50DU(rig.podId,"handling root placement",[&](){
    native_racer50HW::placeColdHandlingRoots(x);
   });
   coldRacerStage50DU(rig.podId,"model rebase",[&](){
    rebaseModelCount=rig.modelCount;
    for(unsigned component=0;component<rebaseModelCount;++component)rebaseModels[component]=rig.rebaseModels[component];
    require50DU(rebaseColdPodScene50FB({s.nodes,s.sceneNodeCount},rebaseModels,rebaseModelCount,
        rebaseWorkspace,rebaseBounds,rebaseDiagnostic),"original racer cold model rebase failed");
   });
   coldRacerStage50DU(rig.podId,"effect name binding",[&](){rig.buildEffectNames(s,namesForEffects);});
   coldRacerStage50DU(rig.podId,"effect owner construction",[&](){bindColdNativeEffects();});
   query.resource=&collision.main();secondaryQuery.resource=&collision.secondary();
   if(gcTriggerActions||isGaTrack||isMaSaTrack||isMbTrack||isScTrack||isTcTrack||isRcTrack||isTbTrack||isGbTrack||isMcTrack){qfilter.identity=0x30000000u+static_cast<std::uint32_t>(racer*2);
    secondaryFilter.identity=qfilter.identity+1u;}
   terrain_probe_authoring50fq_fixture::Owner ownProbe{};std::string probeError;
   coldRacerStage50DU(rig.podId,"terrain probe authoring",[&](){
    const bool probeReady=terrain_probe_authoring50fq_fixture::prepare(ownProbe,query,x.parameters,
        rig.phy,terrainBindings,probeError,collision);
    require50DU(probeReady,probeError.empty()?"original racer probe authoring failed":probeError.c_str());
   });
   for(unsigned axis=0;axis<3;++axis)x.parameters.bodies.main.probeLocal140[axis]=ownProbe.lowerEndpoint140.v[axis];
   x.parameters.bodies.main.probeLocal140[3]=1.f;
   coldRacerStage50DU(rig.podId,"terrain probe installation",[&](){
    const bool probeInstalled=terrain_probe_authoring50fq_fixture::installProbeCold(terrainOwner,
        terrainLiveOwner,ownProbe,probeError);
    require50DU(probeInstalled,probeError.empty()?"original racer cold probe installation failed":probeError.c_str());
   });
   if(originalRecoveryEnabled){
    std::string recoveryError;
    require50DU(undertrack_owners50ew_fixture::bindSelectedCold(recoveryQueryOwners,
        collision,query,secondaryQuery,x.parameters,rig.phy,recoveryError),
        recoveryError.empty()?"selected original recovery query owner rejected":recoveryError.c_str());
    qfilter.identityLists[0].identities={secondaryFilter.identity,recoveryQueryOwners.podFilter.identity,
        movement.filter.identity,terrainOwner.metadata.filter.identity};
    secondaryFilter.identityLists[0].identities={qfilter.identity,recoveryQueryOwners.podFilter.identity,movement.filter.identity};
    require50DU(native_original_ai_recovery::bindColdModels(s,x.bindings,rebaseModels,rebaseModelCount,
        originalRecoveryModels),"actual selected reset model-bound owners rejected");
    x.bindings.podLiveQuery50IZ=&recoveryPodLive;
   }
   // This slot has never advanced. Replace inherited enabled-probe construction
   // with captured pre-enable construction, then use its own native constructor.
   s.terrainProbe=freshTerrainProbe;s.bodies.main.components.probe348=freshMainProbe;
   s.bodies.main.domain.probe100=freshProbe100;
   coldRacerStage50DU(rig.podId,"terrain probe enable",[&](){
    require50DU(enableVehicleTerrain50FP(s,x.bindings,terrainOwner.initial,
        terrainOwner.follow,x.workspace,x.diagnostic),"enable original racer terrain probe");
   });
   coldRacerStage50DU(rig.podId,"visual construction",[&](){
    racerVisuals[racer]=std::make_unique<native_racer50HW::Visual>(".",
        runtime_asset_paths50LB::resolve("evidence/originalRoster50HW/original-eight-visuals"),rig);
   });
   coldRacerStage50DU(rig.podId,"cable construction",[&](){
    racerCables[racer]=std::make_unique<native_racer50HW::Cables>(rig,s,x.bindings);
   });
   coldRacerStage50DU(rig.podId,"flame construction",[&](){
    coldFlame=native_racer50HW::buildColdFlames(rig,racerVisuals[racer]->drawAssets,x.bindings);
   });
   racerNames[racer]=racerVisuals[racer]->displayName;
   contactBindings[racer]={&collision,settings,response};
   if(!racer){
    // Human finish controls retain this racer's selected tuning as well.
    authored=receipt.authored;
    authoredFinishSteering10A4=x.parameters.bodies.main.aero.steering100;
    // Recovery's cold scene carries the human lift admission. Terrain owners
    // and mesh data remain unchanged; opponent grid queries pass their own lift.
    if(isRcTrack)require50DU(rcCollisionWorld->bindHumanLift(x.parameters.visual.verticalLimit7EC),"RC actual selected/class-applied human lift rejected");
    if(isMcTrack)require50DU(mcCollisionWorld->bindHumanLift(x.parameters.visual.verticalLimit7EC),"MC actual selected/class-applied human lift rejected");
    if(isGbTrack)require50DU(gbCollisionWorld->bindHumanLift(x.parameters.visual.verticalLimit7EC),"GB actual selected/class-applied human lift rejected");
    if(isTbTrack)require50DU(tbCollisionWorld->bindHumanLift(x.parameters.visual.verticalLimit7EC),"TB actual selected/class-applied human lift rejected");
    else if(isTcTrack)require50DU(tcCollisionWorld->bindHumanLift(x.parameters.visual.verticalLimit7EC),"TC actual selected/class-applied human lift rejected");
    else if(isMbTrack)require50DU(mbCollisionWorld->setHumanLift(x.parameters.visual.verticalLimit7EC),"MB genuine human lift update rejected");
    else if(isScTrack)require50DU(scGround->bindHumanLift(*scColdSources,x.parameters.visual.verticalLimit7EC),
        "SC actual selected/class-applied human lift rejected");
    else legacyGround.lift=x.parameters.visual.verticalLimit7EC;
   }
   std::cerr<<"ORIGINAL RACER "<<racer<<": "<<racerNames[racer]<<" ["<<rig.podId
       <<"] nodes="<<s.sceneNodeCount<<" mass="<<x.parameters.bodies.main.body.mass
       <<" lift="<<receipt.liftHeight<<" main_rays="<<collision.main().rayCount
       <<" secondary_rays="<<collision.secondary().rayCount
       <<" track="<<authoredTrackName<<" class="<<static_cast<unsigned>(receipt.authored.selectedClass)
       <<" class_values="<<receipt.authored.usedAuthoredClassValues
       <<" max_thrust="<<x.parameters.bodies.main.engine.maxThrust
       <<" top_speed_body="<<x.parameters.bodies.main.engine.theoreticalTopSpeed
       <<" steering100="<<x.parameters.bodies.main.aero.steering100
       <<" repair="<<s.fields.repairCF4<<" cooling="<<boostCoolingCF8<<'\n';
  });
  // Keep exact original names in fixed cold storage for allocation-free HUD publication.
  for(std::size_t racer=0;racer<raceRacerCount;++racer){
   require50DU(!racerNames[racer].empty()&&racerNames[racer].size()<=platform::maxVehicleWindowRacerName50DS,
       "original racer display name exceeds fixed HUD capacity");
   std::memcpy(racerHudNames[racer].data(),racerNames[racer].c_str(),racerNames[racer].size()+1);
  }
}
} // namespace interactive_cold50DU
