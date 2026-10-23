#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_static_models50kg.h"
#include "original_ui_garage_actors50gn.h"
#include "original_ui_main_session50kg.h"
namespace rr::platform {namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool configFail(D& d,S status,std::uint32_t pc)noexcept {d={};d.status=status;d.sourcePC=pc;return false;}
}
bool OriginalUiGaragePodScene50KG::beginLoad1B2AB0(
    std::shared_ptr<OriginalUiMainApplication50KG> application,
    std::shared_ptr<const OriginalUiMainProfile50KG> profile,
    std::shared_ptr<OriginalUiGarageModelResources50KG> resources,D& d)noexcept {
    d={};if(!currentPrefix()||loadStage_!=LoadStage::empty||!savedModelDirectory1FB1B4_||
       !nativeApplication_||application!=nativeApplication_||config34_||staticModels_||
       actorOwner3C8_||visibility_||!application||!application->known||!profile||!profile->known||!resources||
       resources->issuer_!=grant_->issuer()||resources->generation_!=grant_->generation_||
       resources->application_!=grant_->application()||
       resources->application_.owner_before(grant_->application())||grant_->application().owner_before(resources->application_)||
       !resources->domain_||
       !resources->domain_->archiveProvider3E1A70())return configFail(d,S::phase,0x1B2AB0);
    try {
        //1FB178..1FB1CC already captured the actual local and wrote this
        //same retained Application namespace, before material1FB1F0. Do not
        //repeat or invent that earlier source write here at slot30.
        if(application->sceneConfigExportGlobals.namespacePrefix36B640!="ui\\UIGarage\\")
            return configFail(d,S::phase,0x1FB1CC);
        nativeApplication_=std::move(application);nativeProfile_=std::move(profile);configResources_=std::move(resources);
        //270550 on a genuinely fresh Scene. SoundGroup4D8 and500/524 survive.
        path3C_.clear();base140_.clear();sceneName244_.clear();
        numPortals358_=numZones34C_=useEnvironment35C_=0;
        portals3A0_=nullptr;physicsManager4B8_=nullptr; //270594/2705FC.
        water400_=nullptr;waterRenderer404_=nullptr;ecosystem408_=nullptr; //2705D0/D4/D8.
        //Actual27061C..62C reset of all FIVE fog interpolation source fields.
        //A fresh native Scene has no prior allocation to silently discard.
        for(const auto& row:fogInterpolation4DC_)if(row)return configFail(d,S::phase,0x27061C);
        fogInterpolationReset27061C_=true;
        loadStage_=LoadStage::reset;return true;
    }catch(...){configFailure_=true;return configFail(d,S::allocation,0x1FB1CC);}
}
bool OriginalUiGaragePodScene50KG::loadSceneConfig1B3880(D& d)noexcept {
    d={};if(configFailure_||sceneConfigAttempted_||loadStage_!=LoadStage::reset||!currentPrefix()||
       !nativeApplication_||!configResources_||config34_)return configFail(d,S::phase,0x1B3880);
    sceneConfigAttempted_=true;
    try {
        path3C_=nativeApplication_->sceneConfigExportGlobals.namespacePrefix36B640;
        if(path3C_!="ui\\UIGarage\\")return configFail(d,S::source,0x1B38DC);
        base140_="garage";
        config34_=std::unique_ptr<OriginalUiGarageSceneConfig50KG>(new OriginalUiGarageSceneConfig50KG());
        auto provider=configResources_->domain_->archiveProvider3E1A70();
        assets::OriginalUiPageDiagnostic50KG text;
        using Result=OriginalUiGarageSceneConfig50KG::OpenResult;
        auto result=config34_->open1577D0(provider,assets_->archive(),path3C_+base140_+".scb",text);
        if(result==Result::absent){
            result=config34_->open1577D0(provider,assets_->archive(),path3C_+base140_+".scn",text);
            if(result==Result::opened&&nativeApplication_->sceneConfigExportGlobals.exportScb434310!=0){
                configFailure_=true;return configFail(d,S::phase,0x159580); //No output/export authority.
            }
        }
        if(result!=Result::opened){
            if(result==Result::absent){config34_->close1573B0();config34_.reset();}
            configFailure_=true;return configFail(d,result==Result::allocation?S::allocation:S::source,
                text.sourcePC?text.sourcePC:0x1B39C0);
        }
        loadStage_=LoadStage::sceneOpen;return true;
    }catch(...){configFailure_=true;return configFail(d,S::allocation,0x1B3984);}
}
const assets::OriginalUiDat50KG* OriginalUiGaragePodScene50KG::liveSceneConfig34()const noexcept {
    if(configFailure_||!currentPrefix()||!config34_||loadStage_<LoadStage::sceneOpen||
       loadStage_>LoadStage::sceneTail)return nullptr;
    return config34_->parsed();
}
const assets::OriginalUiDat50KG* OriginalUiGaragePodScene50KG::liveToyConfig34()const noexcept {
    if(configFailure_||!currentPrefix()||!config34_||loadStage_<LoadStage::toyOpen||
       loadStage_>LoadStage::toyTail)return nullptr;
    return config34_->parsed();
}
bool OriginalUiGaragePodScene50KG::readSceneInfo270900Prefix(D& d)noexcept {
    d={};const auto* file=liveSceneConfig34();
    if(sceneInfoAttempted_||loadStage_!=LoadStage::sceneOpen||!file)return configFail(d,S::phase,0x270900);
    sceneInfoAttempted_=true;
    const auto* section=file->section("SceneInfo");assets::OriginalUiPageDiagnostic50KG text;
    if(!section)return configFail(d,S::source,0x2709A4);
    const auto* name=section->find("SceneName");
    try {
        if(!name||!name->sourceValuePresent||name->text.size()>=260)return configFail(d,S::source,0x2709BC);
        sceneName244_=name->text;
        if(!assets::originalUiInteger50KG(*section,"NumPortals",0,numPortals358_,text,0x2709D4)||
           !assets::originalUiInteger50KG(*section,"NumZones",1,numZones34C_,text,0x270A00)||
           !assets::originalUiInteger50KG(*section,"UseEnvironmentNode",0,useEnvironment35C_,text,0x270A2C))
            return configFail(d,S::source,text.sourcePC);
        // Native bounded Garage scope. Non-Garage branches need their actual
        // portal/environment-node owners, not skipped records or empty zones.
        if(sceneName244_!="Garage"||numPortals358_!=0||numZones34C_!=1||useEnvironment35C_!=0)
            return configFail(d,S::source,0x270A48);
        loadStage_=LoadStage::sceneInfo;return true;
    }catch(...){return configFail(d,S::allocation,0x2709BC);}
}
bool OriginalUiGaragePodScene50KG::closeSceneConfig271C10(D& d)noexcept {
    d={};if(loadStage_!=LoadStage::sceneTail||!liveSceneConfig34())return configFail(d,S::phase,0x271C10);
    config34_->close1573B0();config34_.reset();loadStage_=LoadStage::sceneClosed;return true;
}
bool OriginalUiGaragePodScene50KG::loadToyConfig1B4330(D& d)noexcept {
    d={};if(configFailure_||toyConfigAttempted_||loadStage_!=LoadStage::sceneClosed||config34_||
       !currentPrefix()||!nativeApplication_||!configResources_)return configFail(d,S::phase,0x1B4330);
    toyConfigAttempted_=true;
    try {
        config34_=std::unique_ptr<OriginalUiGarageSceneConfig50KG>(new OriginalUiGarageSceneConfig50KG());
        auto provider=configResources_->domain_->archiveProvider3E1A70();
        assets::OriginalUiPageDiagnostic50KG text;
        using Result=OriginalUiGarageSceneConfig50KG::OpenResult;
        auto result=config34_->open1577D0(provider,assets_->archive(),path3C_+base140_+".tob",text);
        if(result==Result::absent){
            result=config34_->open1577D0(provider,assets_->archive(),path3C_+base140_+".toy",text);
            if(result==Result::opened&&nativeApplication_->sceneConfigExportGlobals.exportTob434318!=0){
                configFailure_=true;return configFail(d,S::phase,0x159580);
            }
        }
        if(result!=Result::opened){
            if(result==Result::absent){config34_->close1573B0();config34_.reset();}
            configFailure_=true;return configFail(d,result==Result::allocation?S::allocation:S::source,
                text.sourcePC?text.sourcePC:0x1B43E8);
        }
        const auto* file=config34_->parsed();if(!file)return configFail(d,S::phase,0x1577D0);
        if(const auto* pod=file->section("PodRacer")){
            std::int32_t dust=0,ipu=3;
            if(!assets::originalUiInteger50KG(*pod,"TrackEmitsDust",0,dust,text,0x1B44B0)||
               !assets::originalUiInteger50KG(*pod,"IPUCacheSize",3,ipu,text,0x1B44C8))
                return configFail(d,S::source,text.sourcePC);
            trackEmitsDust51C_=dust!=0;mode524_=static_cast<std::uint32_t>(ipu);
        }
        //1B44D0 follows actual Application2DC profile1125C, not playback
        // readiness. Rain is queried only when bits5..6 equal1.
        if(!nativeProfile_||!nativeProfile_->known)return configFail(d,S::phase,0x1B44D0);
        if(((nativeProfile_->playerBits1125C>>5)&3u)==1u){
            if(const auto* rain=file->section("Rain")){
                std::int32_t active=0;
                if(!assets::originalUiInteger50KG(*rain,"RainActive",0,active,text,0x1B4524))
                    return configFail(d,S::source,text.sourcePC);
                if(active)return configFail(d,S::phase,0x2D0290); //Required real Rain owner not supplied.
            }
        }
        if(const auto* animations=file->section("321_Anims")){
            if(!assets::originalUiInteger50KG(*animations,"Num321Anims",0,num321Anims528_,text,0x1B45BC))
                return configFail(d,S::source,text.sourcePC);
            if(num321Anims528_!=0)return configFail(d,S::phase,0x1B45C8);
        }
        loadStage_=LoadStage::toyOpen;return true;
    }catch(...){configFailure_=true;return configFail(d,S::allocation,0x1B43AC);}
}
bool OriginalUiGaragePodScene50KG::closeToyConfig272800(D& d)noexcept {
    d={};if(loadStage_!=LoadStage::toyTail||!liveToyConfig34())return configFail(d,S::phase,0x272800);
    config34_->close1573B0();config34_.reset();loadStage_=LoadStage::toyClosed;return true;
}
void OriginalUiGaragePodScene50KG::closeConfig34ForRetirement1573B0()noexcept {
    if(config34_){config34_->close1573B0();config34_.reset();}
    if(config38_){config38_->close1573B0();config38_.reset();retiredAudioConfig38_=true;}
    configFailure_=true; //Retirement cannot re-enter a load or issue readiness.
}
OriginalUiGaragePodScene50KG::C* OriginalUiGaragePodScene50KG::pageEntity3A8()noexcept {
    if(configFailure_||!currentPrefix()||!sceneSetup_||!sceneSetup_->complete_||sceneSetup_->closed_||
       !sceneSetup_->lights3A8_||!sceneSetup_->lights3A8_->entityLive)return nullptr;
    auto& entity=sceneSetup_->lights3A8_->entity;
    if(entity.kind!=K::entity141300||entity.reference00!=1||entity.sessionGeneration!=grant_->generation_||
       entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity))||
       entity.parent1C||entity.previous10||entity.next14||entity.firstChild18||entity.descendants24)return nullptr;
    return &entity;
}
OriginalUiGaragePodScene50KG::C* OriginalUiGaragePodScene50KG::pageEntity3CC(std::size_t zone)noexcept {
    if(configFailure_||!currentPrefix()||zone!=0||!sceneDispatch_||!sceneDispatch_->sceneComplete||
       sceneDispatch_->closed||!sceneDispatch_->collisionLists3CC||!sceneDispatch_->collisionZone)return nullptr;
    auto& list=*sceneDispatch_->collisionZone;auto& entity=list.entity;
    if(!list.live||!list.complete||list.count30!=30||!list.records34||
       sceneDispatch_->collisionLists3CC[0]!=&list||entity.kind!=K::entity141300||entity.reference00!=1||
       entity.sessionGeneration!=grant_->generation_||
       entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity))||
       entity.parent1C||entity.previous10||entity.next14||entity.firstChild18||entity.descendants24)return nullptr;
    for(std::size_t i=0;i<list.records.size();++i)
        if(!list.records[i]||list.records34[i]!=list.records[i].get()||!list.records[i]->temporary08)return nullptr;
    return &entity;
}
}
