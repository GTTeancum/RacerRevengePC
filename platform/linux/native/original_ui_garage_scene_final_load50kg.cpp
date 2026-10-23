#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_emitters50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "original_ui_garage_static_models50kg.h"
#include "original_ui_garage_actors50gn.h"
#include "original_ui_main_session50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <algorithm>
#include <cstring>
namespace rr::platform {namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool finalLoadStop(D& d,S status,std::uint32_t pc)noexcept {
    d={};d.status=status;d.sourcePC=pc;return false;
}
}
bool OriginalUiGaragePodScene50KG::captureModelDirectory1FB178(D& d)noexcept {
    d={};
    if(!currentPrefix()||loadStage_!=LoadStage::empty||loadDirectorySaveAttempted_||
       savedModelDirectory1FB1B4_||finalLoadStage_!=FinalLoadStage::none)
        return finalLoadStop(d,S::phase,0x1FB178);
    const auto* host=grant_->issuer();
    if(!host||host->entities()!=grant_->application()||!host->state||!host->state->known)
        return finalLoadStop(d,S::phase,0x1FB178);
    loadDirectorySaveAttempted_=true;
    try {
        const auto& actual=host->modelDirectory36B860();
        //Source260-byte local, min(strlen,259), memcpy followed by NUL.
        savedModelDirectory1FB1B4_=actual.substr(0,std::min<std::size_t>(std::strlen(actual.c_str()),259));
        //The next actual source write1FB1CC precedes the Root material
        //directory write1FB1F0 and slot30. Retain the real producer owner.
        nativeApplication_=host->state;
        nativeApplication_->sceneConfigExportGlobals.namespacePrefix36B640="ui\\UIGarage\\";
        return true;
    }catch(...){sourceConstructionFailed_=true;return finalLoadStop(d,S::allocation,0x1FB1A8);}
}
bool OriginalUiGaragePodScene50KG::sceneLoadTailPrerequisitesCurrent()const noexcept {
    if(!currentPrefix()||sourceConstructionFailed_||configFailure_||
       loadStage_!=LoadStage::audioClosed||populationStage_!=PopulationStage::emitters||
       config34_||config38_||!retiredAudioConfig38_||!savedModelDirectory1FB1B4_||
       !sceneSetup_||sceneSetup_->closed_||!sceneSetup_->complete_||!sceneSetup_->startData348_||
       !sceneSetup_->environment3A4_||!sceneDispatch_||sceneDispatch_->closed||
       !sceneDispatch_->sceneComplete||!sceneDispatch_->terrainComplete||
       !toyFields_||toyFields_->closed_||!toyFields_->complete_||
       !audioFields_||audioFields_->closed||!audioFields_->complete||
       !staticModels_||!staticModels_->populationCurrent()||!actorOwner3C8_||
       !actorOwner3C8_->scenePopulationCurrent2742F0(*this)||!actorOverrideScan1B40B0_||
       !sceneParticles_||!sceneParticles_->prepared||!sceneParticles_->entityLive||
       !sceneParticles_->numeric||!sceneParticles_->numeric->prepared()||
       !sceneEmitters_||!sceneEmitters_->complete||sceneEmitters_->failed||
       sceneEmitters_->retired||sceneEmitters_->completed!=7||!toyFields_->particles3D8_)return false;
    const auto& list=*toyFields_->particles3D8_;
    if(!list.live||list.count30!=7||!list.records34||list.entity.reference00!=1||
       list.entity.parent1C||list.entity.previous10||list.entity.next14||
       list.entity.descendants24!=0x402u)return false;
    const C* previous=nullptr;const C* child=list.entity.firstChild18;
    for(std::size_t i=0;i<7;++i){
        const auto& owner=sceneEmitters_->rows[i];const auto* wrapper=list.records34[i];
        if(!owner||!owner->entityLive||!owner->numericConstructed||!owner->loaded||
           !owner->attached||owner->retiring||owner->retired||child!=&owner->entity||
           owner->entity.parent1C!=&list.entity||owner->entity.previous10!=previous||
           owner->entity.reference00!=1||owner->entity.mask20!=0x402u||owner->entity.firstChild18||
           !wrapper||wrapper!=list.records[i].get()||wrapper->temp04||!wrapper->live00||
           wrapper->live00[0]!=&owner->entity||owner->manager70!=&sceneParticles_->entity)return false;
        previous=child;child=child->next14;
    }
    return child==nullptr;
}
bool OriginalUiGaragePodScene50KG::populateWater274570(D& d)noexcept {
    d={};
    if(finalLoadStage_!=FinalLoadStage::soultreeAudio||!sceneLoadTailPrerequisitesCurrent())
        return finalLoadStop(d,S::phase,0x274570);
    //Page1FB450 supplies(a1=0,a2=1,a3=0). The actual Garage environment
    //exists, so the first null3A4 shortcut is not used to impersonate success.
    const auto* water=sceneSetup_->environment3A4_->water110.get();
    if(!water)return finalLoadStop(d,S::phase,0x2745B0);
    if(std::strcmp(water,"none"))return finalLoadStop(d,S::source,0x2745D4);
    if(water400_)return finalLoadStop(d,S::phase,0x2745C8);
    //Actual2745C8 early return. No terrain table/resource/tree/body work ran.
    finalLoadStage_=FinalLoadStage::water;return true;
}
bool OriginalUiGaragePodScene50KG::populateWaterRenderer2747C0(D& d)noexcept {
    d={};
    if(finalLoadStage_!=FinalLoadStage::water||!sceneLoadTailPrerequisitesCurrent())
        return finalLoadStop(d,S::phase,0x2747C0);
    const auto* water=sceneSetup_->environment3A4_->water110.get();
    if(!water)return finalLoadStop(d,S::phase,0x2747EC);
    if(std::strcmp(water,"none"))return finalLoadStop(d,S::source,0x274804);
    if(water400_||waterRenderer404_)return finalLoadStop(d,S::phase,0x2747FC);
    //Page1FB468 supplies a1=0. Source2747FC returns before any renderer
    //allocation, LightManager borrow or Water child attachment is reached.
    finalLoadStage_=FinalLoadStage::waterRenderer;return true;
}
bool OriginalUiGaragePodScene50KG::dispatchEcosystem1FB47C(D& d)noexcept {
    d={};
    if(finalLoadStage_!=FinalLoadStage::waterRenderer||!sceneLoadTailPrerequisitesCurrent())
        return finalLoadStop(d,S::phase,0x1FB47C);
    const auto* name=sceneSetup_->environment3A4_->ecosystem318.get();
    if(!name)return finalLoadStop(d,S::phase,0x1FB480);
    if(std::strcmp(name,"none"))return finalLoadStop(d,S::source,0x1FB49C);
    if(ecosystem408_)return finalLoadStop(d,S::phase,0x1FB488);
    //Actual Page condition skips slot138 entirely. DO NOT call2748F0 with
    //'none': that function's own branch constructs a default ecosystem path!
    finalLoadStage_=FinalLoadStage::ecosystem;return true;
}
bool OriginalUiGaragePodScene50KG::finishSceneLoad1FB594(std::string& actualMaterialDirectory,D& d)noexcept {
    d={};
    if(finalLoadStage_!=FinalLoadStage::ecosystem||!sceneLoadTailPrerequisitesCurrent())
        return finalLoadStop(d,S::phase,0x1FB520);
    const auto* host=grant_->issuer();
    if(!host||&actualMaterialDirectory!=&host->materialDirectory36B970()||
       host->state!=nativeApplication_||host->entities()!=grant_->application())
        return finalLoadStop(d,S::phase,0x1FB540);
    try {
        //The saved local is genuine1FB178..1FB1B4 data, not an assumed UI path.
        host->modelDirectory36B860()=*savedModelDirectory1FB1B4_; //1FB54C/560.
        actualMaterialDirectory="ui\\textures\\"; //Literal3B0198 at1FB580/584/588.
        finalLoadStage_=FinalLoadStage::namespaceRestored;
        std::string error;
        if(!configureReverb235520(error)){
            sourceConstructionFailed_=true;return finalLoadStop(d,S::phase,0x1FB594);
        }
        //This proves only the exact successful source return at1FB5A8.
        //It does not manufacture a wet mixer, Page parent, draw or scheduler.
        finalLoadStage_=FinalLoadStage::complete;return true;
    }catch(...){sourceConstructionFailed_=true;return finalLoadStop(d,S::allocation,0x1FB54C);}
}
bool OriginalUiGaragePodScene50KG::sourceLoadReturned1FB59C()const noexcept {
    //A completed source call remains completed after Page takes the genuine
    //child borrows. Do not reapply detached-custody predicates to those rows.
    return finalLoadStage_==FinalLoadStage::complete&&savedModelDirectory1FB1B4_&&
        !sourceConstructionFailed_&&!configFailure_&&reverbAttempted_&&constructed_&&sceneLive_&&
        retirementStage_==RetirementStage::none&&grant_&&grant_->current()&&
        grant_->podSceneOwnerCurrent(this,podScene_)&&podScene_.reference00==1;
    //Later native wave publication / queue consumption cannot erase this
    //actual prior235520 call. Page admission uses its own attachment ledger.
}
// Include inside namespace rr::platform in the Scene final-load translation
// unit. The parent owns FinalLoadStage and sceneLoadTailPrerequisitesCurrent.
// No Entity allocation, fake source vtable, AudioFields mutation or sample
// registration belongs to these three authored Garage branches.
bool OriginalUiGaragePodScene50KG::populateTriggerList2DCB30(
    OriginalUiGarageToyFields50KG::TriggerList& list,D& d)noexcept {
    d={};
    const auto fail=[&](assets::OriginalStandardMaterialStatus50LA status,std::uint32_t pc){
        d.status=status;d.sourcePC=pc;return false;
    };
    using S=assets::OriginalStandardMaterialStatus50LA;
    if(!sceneLoadTailPrerequisitesCurrent()||finalLoadStage_!=FinalLoadStage::none||
       !toyFields_||!toyFields_->complete_||toyFields_->closed_||
       toyFields_->triggers3D0_.get()!=&list||!list.live)
        return fail(S::phase,0x2DCB30);
    const auto& entity=list.entity;
    if(entity.kind!=K::entity141300||entity.reference00!=1||
       entity.sessionGeneration!=grant_->generation_||
       entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity))||
       entity.parent1C||entity.previous10||entity.next14||entity.firstChild18||
       entity.mask20||entity.descendants24)
        return fail(S::phase,0x2DCB44);
    // The existing live 2DC430 reader admits a genuinely retained list when
    // [Triggers] exists with count0. Missing [Triggers] instead retires it.
    // Do not collapse those distinct outcomes into pointer-null/count-null.
    if(list.count30<0)return fail(S::source,0x2DC4B0);
    if(list.count30>0){
        // Positive rows require actual Trigger records, their virtual+0C
        // constructors, and Entity141960(mask0x402), in ascending source
        // row order. C* records are not that descriptor/owner model.
        return fail(S::source,0x2DCB54);
    }
    if(list.records34)return fail(S::phase,0x2DCB54);
    // Enter the real virtual+7C target even for count0: its first action is
    // the 2DCC40 loop test. No row constructor/attachment executes, and its
    // actual 2DCC60 return is1. This is not an allocated empty substitute.
    return true;
}

bool OriginalUiGaragePodScene50KG::populateDynamicObjects274E90(D& d)noexcept {
    d={};
    if(!sceneLoadTailPrerequisitesCurrent()||finalLoadStage_!=FinalLoadStage::none||
       !toyFields_||!toyFields_->complete_||toyFields_->closed_){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x274E90;return false;
    }
    //274E98 passes this Scene as a1. The call is conditional on pointer3D0,
    //NOT on count30, so a real retained zero-row list still gets its call.
    if(toyFields_->triggers3D0_&&
       !populateTriggerList2DCB30(*toyFields_->triggers3D0_,d))return false;
    finalLoadStage_=FinalLoadStage::dynamicObjects;return true;
}

bool OriginalUiGaragePodScene50KG::populateAmbientList2794B0(
    OriginalUiGarageAudioFields50KG::List& list,D& d)noexcept {
    d={};
    const auto fail=[&](assets::OriginalStandardMaterialStatus50LA status,std::uint32_t pc){
        d.status=status;d.sourcePC=pc;return false;
    };
    using S=assets::OriginalStandardMaterialStatus50LA;
    if(!sceneLoadTailPrerequisitesCurrent()||finalLoadStage_!=FinalLoadStage::dynamicObjects||
       !audioFields_||!audioFields_->complete||audioFields_->closed||
       audioFields_->ambient3B4.get()!=&list||!list.live)
        return fail(S::phase,0x2794B0);
    const auto& entity=list.entity;
    if(entity.kind!=K::entity141300||entity.reference00!=1||
       entity.sessionGeneration!=grant_->generation_||
       entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity))||
       entity.parent1C||entity.previous10||entity.next14||entity.firstChild18||
       entity.mask20||entity.descendants24)
        return fail(S::phase,0x2794D0);
    if(list.count30<0)return fail(S::source,0x2793E8);
    if(list.count30>0){
        // First source row2794E4 needs the actual12-byte Ambient wrapper,
        // derived291E70 Sound Entity and its291F20/291FA0 configuration,
        // then descriptor destruction and list attachment141960(mask2).
        // Existing AudioFields has cold descriptors, not these live owners.
        return fail(S::source,0x2794E4);
    }
    //2793F4 returns readfalse for count0;2714BC retires the actual Entity
    //and2714D0 nulls3B4. A nonnull zero-row owner cannot be produced by the
    //retained successful reader. Never promote a fabricated empty list.
    return fail(S::phase,0x2714D0);
}

bool OriginalUiGaragePodScene50KG::populateAmbientAudio275140(D& d)noexcept {
    d={};
    if(!sceneLoadTailPrerequisitesCurrent()||finalLoadStage_!=FinalLoadStage::dynamicObjects||
       !audioFields_||!audioFields_->complete||audioFields_->closed){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x275140;return false;
    }
    //27516C passes the actual group4D8; the common prerequisite validates
    //that group's native owner. There is no new group/reference on absence.
    if(audioFields_->ambient3B4&&
       !populateAmbientList2794B0(*audioFields_->ambient3B4,d))return false;
    finalLoadStage_=FinalLoadStage::ambientAudio;return true;
}

bool OriginalUiGaragePodScene50KG::populateSoultreeList279A20(
    OriginalUiGarageAudioFields50KG::List& list,D& d)noexcept {
    d={};
    const auto fail=[&](assets::OriginalStandardMaterialStatus50LA status,std::uint32_t pc){
        d.status=status;d.sourcePC=pc;return false;
    };
    using S=assets::OriginalStandardMaterialStatus50LA;
    if(!sceneLoadTailPrerequisitesCurrent()||finalLoadStage_!=FinalLoadStage::ambientAudio||
       !audioFields_||!audioFields_->complete||audioFields_->closed||
       audioFields_->soultree3B8.get()!=&list||!list.live)
        return fail(S::phase,0x279A20);
    const auto& entity=list.entity;
    if(entity.kind!=K::entity141300||entity.reference00!=1||
       entity.sessionGeneration!=grant_->generation_||
       entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity))||
       entity.parent1C||entity.previous10||entity.next14||entity.firstChild18||
       entity.mask20||entity.descendants24)
        return fail(S::phase,0x279A38);
    if(list.count30<0)return fail(S::source,0x279958);
    if(list.count30>0){
        //279A4C visits12-byte source wrappers in ascending order, calling
        //each actual virtual+0C (278E80) with a1=null and a2=group4D8.
        //It needs103E20 sound construction and descriptor consumption,
        //not a borrowed actor row or a cold descriptor marked successful.
        return fail(S::source,0x279A4C);
    }
    //279964 returns readfalse for count0;271514 retires the real list and
    //271528 nulls3B8. Do not invent a successful empty Soultree owner.
    return fail(S::phase,0x271528);
}

bool OriginalUiGaragePodScene50KG::populateSoultreeAudio275180(D& d)noexcept {
    d={};
    if(!sceneLoadTailPrerequisitesCurrent()||finalLoadStage_!=FinalLoadStage::ambientAudio||
       !audioFields_||!audioFields_->complete||audioFields_->closed){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x275180;return false;
    }
    //The real pointer branch always dispatches list+78, even if a malformed
    //owner says count0. The target then rejects that impossible provenance.
    if(audioFields_->soultree3B8&&
       !populateSoultreeList279A20(*audioFields_->soultree3B8,d))return false;
    finalLoadStage_=FinalLoadStage::soultreeAudio;return true;
}

}
