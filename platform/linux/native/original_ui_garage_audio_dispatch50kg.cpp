#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_main_session50kg.h"
#include <cstring>
#include <exception>
namespace rr::platform {namespace {
using D=assets::OriginalStandardMaterialDiagnostic50LA;
using S=assets::OriginalStandardMaterialStatus50LA;
bool audioFail(D& d,S status,std::uint32_t pc)noexcept {d={};d.status=status;d.sourcePC=pc;return false;}
}
OriginalUiGarageAudioFields50KG::~OriginalUiGarageAudioFields50KG()noexcept {
    if((ambient3B4&&ambient3B4->live)||(soultree3B8&&soultree3B8->live)||
       (collision3BC&&collision3BC->live)||(driveOn3C0&&driveOn3C0->live))std::terminate();
}
bool OriginalUiGaragePodScene50KG::loadAudioConfig1B4720(D& d)noexcept {
    d={};if(configFailure_||audioConfigAttempted_||loadStage_!=LoadStage::toyClosed||
       !currentPrefix()||!nativeApplication_||!configResources_||config34_||config38_)
        return audioFail(d,S::phase,0x1B4720);
    audioConfigAttempted_=true;
    try{
        config38_=std::unique_ptr<OriginalUiGarageSceneConfig50KG>(new OriginalUiGarageSceneConfig50KG());
        auto provider=configResources_->domain_->archiveProvider3E1A70();
        assets::OriginalUiPageDiagnostic50KG text;
        using R=OriginalUiGarageSceneConfig50KG::OpenResult;
        auto opened=config38_->open1577D0(provider,assets_->archive(),path3C_+base140_+".aub",text);
        if(opened==R::absent){
            opened=config38_->open1577D0(provider,assets_->archive(),path3C_+base140_+".aud",text);
            if(opened==R::opened&&nativeApplication_->sceneConfigExportGlobals.exportTob434318!=0){
                configFailure_=true;return audioFail(d,S::phase,0x159580);
            }
        }
        if(opened!=R::opened){
            if(opened==R::absent){config38_->close1573B0();config38_.reset();}
            configFailure_=true;return audioFail(d,opened==R::allocation?S::allocation:S::source,
                text.sourcePC?text.sourcePC:0x1B47B8);
        }
        loadStage_=LoadStage::audioOpen;return true;
    }catch(...){configFailure_=true;return audioFail(d,S::allocation,0x1B4794);}
}
bool OriginalUiGaragePodScene50KG::readAudioDispatch2728E0(D& d)noexcept {
    d={};const auto* file=config38_?config38_->parsed():nullptr;
    if(configFailure_||audioDispatchAttempted_||loadStage_!=LoadStage::audioOpen||
       !currentPrefix()||!file||!sceneDispatch_||!sceneDispatch_->sceneComplete)
        return audioFail(d,S::phase,0x2728E0);
    audioDispatchAttempted_=true;
    const auto fail=[&](S status,std::uint32_t pc){configFailure_=true;return audioFail(d,status,pc);};
    try{
        using Fields=OriginalUiGarageAudioFields50KG;
        audioFields_=std::unique_ptr<Fields>(new Fields());auto& out=*audioFields_;
        const auto* sounds=file->section("Sounds");if(!sounds)return fail(S::source,0x2793C4);
        assets::OriginalUiPageDiagnostic50KG text;
        // All four are real counted list constructions, including zero-count
        // Ambient/Soultree lists which take the source readfalse/destructor path.
        const auto makeList=[&](std::unique_ptr<Fields::List>& owner,const char* key,
            std::uint32_t ctor,std::uint32_t read,std::int32_t required)->bool {
            owner=std::make_unique<Fields::List>();
            auto result=grant_->constructPrefix(owner->entity,K::entity141300);
            if(!result.okay())return fail(S::phase,result.sourcePC?result.sourcePC:ctor);
            owner->live=true;
            if(!assets::originalUiInteger50KG(*sounds,key,0,owner->count30,text,read))
                return fail(S::source,text.sourcePC);
            if(owner->count30!=required)return fail(S::source,read);
            if(!owner->count30){
                result=grant_->retireEmptyPrefix(owner->entity,K::entity141300,nullptr);
                if(!result.okay())return fail(S::phase,result.sourcePC);
                owner->live=false;owner.reset();return true;
            }
            owner->rows34=std::make_unique<Fields::Descriptor[]>(owner->count30);return true;
        };
        if(!makeList(out.ambient3B4,"NumAmbientSounds",0x2792E0,0x2793D8,0)||
           !makeList(out.soultree3B8,"NumSoultreeSounds",0x279850,0x279948,0))return false;
        const auto readDescriptor=[&](const assets::OriginalUiDatSection50KG& row,
            Fields::Descriptor& descriptor,bool drive)->bool {
            const auto* resource=row.find("SoundResourceFile");
            if(!resource||resource->text.size()+4>=260)return fail(S::source,0x2787E0);
            descriptor.soundResource00=resource->text+".wav";
            std::array<float,3> position{};
            if(!assets::originalUiTriple50KG(row,"Position",position,position,text,0x27881C))
                return fail(S::source,text.sourcePC);
            descriptor.position110={position[0],position[1],position[2],1};
            const std::array<const char*,5> switches{{"Looping","OneShot","ResetWhenInaudible","Force2D","Stereo"}};
            const std::array<std::int32_t*,5> destinations{{&descriptor.looping128,&descriptor.oneShot12C,
                &descriptor.resetWhenInaudible130,&descriptor.force2D134,&descriptor.stereo138}};
            for(std::size_t i=0;i<switches.size();++i){
                bool enabled=false;
                if(!assets::originalUiBoolean50KG(row,switches[i],false,enabled,text,0x278834+static_cast<std::uint32_t>(i)*24))
                    return fail(S::source,text.sourcePC);
                *destinations[i]=enabled?1:0;
            }
            if(!assets::originalUiNumber50KG(row,"OneShotDistance",20,descriptor.oneShotDistance13C,text,0x2788B0)||
               !assets::originalUiNumber50KG(row,"RandomTriggerPercent",100,descriptor.randomTriggerPercent140,text,0x2788CC)||
               !assets::originalUiNumber50KG(row,"MaxDistance",0,descriptor.maxDistance120,text,0x2788E4)||
               !assets::originalUiNumber50KG(row,"MinDistance",0,descriptor.minDistance124,text,0x2788FC)||
               !assets::originalUiInteger50KG(row,"Priority",10,descriptor.priority1C0,text,0x278914))
                return fail(S::source,text.sourcePC);
            std::memcpy(descriptor.coldParameters150.data()+0x10,descriptor.position110.data(),16);
            std::memcpy(descriptor.coldParameters150.data()+0x58,&descriptor.minDistance124,4);
            std::memcpy(descriptor.coldParameters150.data()+0x5C,&descriptor.maxDistance120,4);
            descriptor.driveOn=drive;
            if(drive){
                const std::array<const char*,5> keys{{"MaterialSampleSpeed","MaterialMinVolume","MaterialMinSpeed",
                    "MaterialMaxSpeed","MaterialResonantFreqSpeed"}};
                const std::array<std::int32_t*,5> values{{&descriptor.sampleSpeed1D0,&descriptor.minVolume1D4,
                    &descriptor.minSpeed1D8,&descriptor.maxSpeed1DC,&descriptor.resonantSpeed1E0}};
                for(std::size_t i=0;i<keys.size();++i)
                    if(!assets::originalUiInteger50KG(row,keys[i],0,*values[i],text,0x2789DC+static_cast<std::uint32_t>(i)*24))
                        return fail(S::source,text.sourcePC);
                if(!assets::originalUiNumber50KG(row,"MaterialFreqSlope",0,descriptor.frequencySlope1E4,text,0x278A54))
                    return fail(S::source,text.sourcePC);
            }
            descriptor.parsed=true;return true;
        };
        if(!makeList(out.collision3BC,"NumMaterialCollisionSounds",0x279AA0,0x279BA0,8))return false;
        for(std::int32_t i=0;i<out.collision3BC->count30;++i){
            const auto* row=file->section("MaterialCollisionSound"+std::to_string(i+1));
            if(!row)return fail(S::source,0x279C1C);
            if(!readDescriptor(*row,out.collision3BC->rows34[i],false))return false;
        }
        out.collision3BC->complete=true;
        if(!makeList(out.driveOn3C0,"NumMaterialDriveOnSounds",0x279D90,0x279E90,8))return false;
        for(std::int32_t i=0;i<out.driveOn3C0->count30;++i){
            const auto* row=file->section("MaterialDriveOnSound"+std::to_string(i+1));
            if(!row)return fail(S::source,0x279F04);
            if(!readDescriptor(*row,out.driveOn3C0->rows34[i],true))return false;
        }
        out.driveOn3C0->complete=true;
        //271600 slotF0 still executes: two authored materials, no matching
        // CollisionObjectsMaterialN sections. Each count defaults0, so neither
        // OBJ reassignment nor the nested default-material rewrite is reached.
        if(!assets::originalUiInteger50KG(*sounds,"NumMaterials",0,out.materialsCount,text,0x271654)||
           !assets::originalUiInteger50KG(*sounds,"DefaultMaterial",0,out.defaultMaterial,text,0x27166C))
            return fail(S::source,text.sourcePC);
        if(out.materialsCount!=2||out.defaultMaterial!=1)return fail(S::source,0x271674);
        for(std::int32_t i=1;i<=out.materialsCount;++i){
            if(const auto* row=file->section("CollisionObjectsMaterial"+std::to_string(i)))
                if(!assets::originalUiInteger50KG(*row,"NumCollisionObjects",0,out.collisionObjectsPerMaterial[i-1],text,0x2716AC))
                    return fail(S::source,text.sourcePC);
            if(out.collisionObjectsPerMaterial[i-1]!=0)return fail(S::phase,0x2716BC);
        }
        out.complete=true;loadStage_=LoadStage::audioDescriptors;return true;
    }catch(...){return fail(S::allocation,0x271470);}
}
bool OriginalUiGaragePodScene50KG::closeAudioConfig272930(D& d)noexcept {
    d={};if(configFailure_||loadStage_!=LoadStage::audioDescriptors||!audioFields_||
       !audioFields_->complete||!config38_||!config38_->parsed()||config34_)
        return audioFail(d,S::phase,0x272930);
    config38_->close1573B0();config38_.reset();retiredAudioConfig38_=true;
    //272954 writes34, leaving source38 dangling. Native ownership retires the
    // actual object once and records that stale-source state without a UAF.
    loadStage_=LoadStage::audioClosed;return true;
}
bool OriginalUiGaragePodScene50KG::closeAudioFieldsPartial271470(D& d)noexcept {
    d={};if(!audioFields_)return true;auto& out=*audioFields_;if(out.closed)return true;
    if(!grant_||!grant_->current())return audioFail(d,S::phase,0x26FE60);
    const std::array<OriginalUiGarageAudioFields50KG::List*,4> lists{{out.ambient3B4.get(),out.soultree3B8.get(),
        out.collision3BC.get(),out.driveOn3C0.get()}};
    for(const auto* list:lists)if(list&&list->live&&
       (list->entity.parent1C||list->entity.firstChild18||list->entity.reference00!=1||
        list->entity.previous10||list->entity.next14||list->entity.descendants24))
        return audioFail(d,S::phase,0x142590);
    for(auto* list:lists)if(list&&list->live){
        list->rows34.reset();
        const auto result=grant_->retireEmptyPrefix(list->entity,K::entity141300,nullptr);
        if(!result.okay())return audioFail(d,S::phase,result.sourcePC);
        list->live=false;
    }
    out.closed=true;out.complete=false;configFailure_=true;return true;
}
bool OriginalUiGaragePodScene50KG::retirePageMaterialSoundList(
    const OriginalUiGaragePageOwner50KG& page,std::size_t ordinal,D& d)noexcept {
    d={};auto fail=[&](std::uint32_t pc){d.status=S::phase;d.sourcePC=pc;return false;};
    if(ordinal<1||ordinal>2||!grant_||!grant_->current()||!audioFields_||audioFields_->closed||
       !audioFields_->complete)return fail(0x142590);
    auto* list=ordinal==1?audioFields_->collision3BC.get():audioFields_->driveOn3C0.get();
    if(!list)return fail(ordinal==1?0x279AE0:0x279DD0);
    if(list->retired)return true;
    if(!list->live||!list->complete||list->count30!=8)return fail(0x142590);
    if(!list->retiring){
        if(!list->rows34)return fail(ordinal==1?0x279B08:0x279DF8);
        for(std::size_t i=0;i<8;++i)if(!list->rows34[i].parsed||list->rows34[i].driveOn!=(ordinal==2))return fail(0x142590);
        const auto begun=grant_->beginGaragePageAudioList142590(page,*this,ordinal,list->entity);
        if(!begun.okay())return fail(begun.sourcePC);
        list->retiring=true;
    }
    //Actual279AE0/279DD0 free their descriptor array before Entity1413E0.
    //These retained rows are cold configuration, not live Sound/voice owners.
    //Keep count30 and the dying Entity's source observations; reset only the
    //native strong payload. No stop, fake audio drain, or sample deletion.
    list->rows34.reset();
    const auto finished=grant_->finishGaragePageAudioList1413E0(page,*this,ordinal,list->entity);
    if(!finished.okay())return fail(finished.sourcePC);
    list->live=false;list->retiring=false;list->retired=true;return true;
}
bool OriginalUiGaragePodScene50KG::retireFailedPageMaterialSoundList(
    const OriginalUiGaragePageOwner50KG& page,std::size_t ordinal,D& d)noexcept {
    d={};auto fail=[&](std::uint32_t pc){d.status=S::phase;d.sourcePC=pc;return false;};
    // Native terminal cleanup for the exact failed six-child attachment, not
    // a fictional late Page attachment or source successful-load completion.
    if(ordinal<1||ordinal>2||!grant_||!grant_->current()||
       !page.partialSceneAttachmentFailureCurrent()||
       page.failedSceneRetirement_!=OriginalUiGaragePageOwner50KG::FailedSceneRetirement::detachedOwnersRetired||
       page.scene4C0_!=this||page.sceneBacking_.get()!=this||page.grant_!=grant_||
       page.grant_.owner_before(grant_)||grant_.owner_before(page.grant_)||
       !constructed_||!sceneLive_||!groupLive_||retirementStage_!=RetirementStage::none||
       !audioFields_||audioFields_->closed||!audioFields_->complete)
        return fail(0x142590);
    auto* list=ordinal==1?audioFields_->collision3BC.get():audioFields_->driveOn3C0.get();
    if(!list||!list->complete||list->count30!=8)return fail(ordinal==1?0x279AE0:0x279DD0);
    const auto& entity=list->entity;
    if(entity.kind!=K::entity141300||entity.sessionGeneration!=grant_->generation_||
       entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity))||
       entity.parent1C||entity.previous10||entity.next14||entity.firstChild18||
       entity.mask20||entity.descendants24)return fail(0x142590);
    if(list->retired){
        if(list->live||list->retiring||list->rows34||entity.reference00)return fail(0x1413E0);
        return true;
    }
    if(!list->live)return fail(0x142590);
    // The actual failed Page cursor retires these otherwise detached owners
    // tail-first: driveOn3C0, then collision3BC, before its Search continuation.
    if(ordinal==1&&(!audioFields_->driveOn3C0||!audioFields_->driveOn3C0->retired||
       audioFields_->driveOn3C0->live||audioFields_->driveOn3C0->retiring||
       audioFields_->driveOn3C0->rows34||audioFields_->driveOn3C0->entity.reference00))
        return fail(0x279DD0);
    if(!list->retiring){
        if(entity.reference00!=1||!list->rows34)return fail(ordinal==1?0x279B08:0x279DF8);
        for(std::size_t i=0;i<8;++i)
            if(!list->rows34[i].parsed||list->rows34[i].driveOn!=(ordinal==2))return fail(0x142590);
        const auto begun=grant_->beginGarageFailedAudioList142590(page,*this,ordinal,list->entity);
        if(!begun.okay())return fail(begun.sourcePC);
        list->retiring=true;
    }else if(entity.reference00)return fail(0x104970);
    // Actual279AE0/279DD0 body, in the same order as the normal Page path:
    // source ref0 first, descriptor destruction next, canonical1413E0 last.
    // These are configuration rows, not Sound/sample/voice ownership.
    list->rows34.reset();
    const auto finished=grant_->finishGarageFailedAudioList1413E0(page,*this,ordinal,list->entity);
    if(!finished.okay())return fail(finished.sourcePC);
    list->live=false;list->retiring=false;list->retired=true;
    // Keep count30, List.complete, audioFields.complete and all source words.
    // Only actual finishSceneRetirement2701CC later closes the aggregate.
    return true;
}
bool OriginalUiGaragePodScene50KG::retireLateBorrowMaterialSoundList(
    const OriginalUiGaragePageOwner50KG& page,std::size_t ordinal,D& d)noexcept {
    d={};auto fail=[&](std::uint32_t pc){d.status=S::phase;d.sourcePC=pc;return false;};
    // Distinct GUI77 failed1B4D24: all ten initial children are now actually
    // retired, but these two lists were never late-attached to the Page.
    if(ordinal<1||ordinal>2||!grant_||!grant_->current()||
       !page.lateSceneBorrowFailureCurrent()||
       page.sceneInitialRetiredCount_!=page.scenePageChildren_.size()||
       page.scene4C0_!=this||page.sceneBacking_.get()!=this||page.grant_!=grant_||
       page.grant_.owner_before(grant_)||grant_.owner_before(page.grant_)||
       !constructed_||!sceneLive_||!groupLive_||retirementStage_!=RetirementStage::none||
       !audioFields_||audioFields_->closed||!audioFields_->complete)
        return fail(0x142590);
    auto* list=ordinal==1?audioFields_->collision3BC.get():audioFields_->driveOn3C0.get();
    if(!list||!list->complete||list->count30!=8)return fail(ordinal==1?0x279AE0:0x279DD0);
    const auto& entity=list->entity;
    if(entity.kind!=K::entity141300||entity.sessionGeneration!=grant_->generation_||
       entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity))||
       entity.parent1C||entity.previous10||entity.next14||entity.firstChild18||
       entity.mask20||entity.descendants24)return fail(0x142590);
    if(list->retired){
        if(list->live||list->retiring||list->rows34||entity.reference00)return fail(0x1413E0);
        return true;
    }
    if(!list->live)return fail(0x142590);
    // The actual failed Page cursor retires these otherwise detached owners
    // tail-first: driveOn3C0, then collision3BC, before Router/Page ref0.
    if(ordinal==1&&(!audioFields_->driveOn3C0||!audioFields_->driveOn3C0->retired||
       audioFields_->driveOn3C0->live||audioFields_->driveOn3C0->retiring||
       audioFields_->driveOn3C0->rows34||audioFields_->driveOn3C0->entity.reference00))
        return fail(0x279DD0);
    if(!list->retiring){
        if(entity.reference00!=1||!list->rows34)return fail(ordinal==1?0x279B08:0x279DF8);
        for(std::size_t i=0;i<8;++i)
            if(!list->rows34[i].parsed||list->rows34[i].driveOn!=(ordinal==2))return fail(0x142590);
        const auto begun=grant_->beginGarageLateBorrowAudioList142590(page,*this,ordinal,list->entity);
        if(!begun.okay())return fail(begun.sourcePC);
        list->retiring=true;
    }else if(entity.reference00)return fail(0x104970);
    // Actual279AE0/279DD0 body, in the same order as the normal Page path:
    // source ref0 first, descriptor destruction next, canonical1413E0 last.
    // These are configuration rows, not Sound/sample/voice ownership.
    list->rows34.reset();
    const auto finished=grant_->finishGarageLateBorrowAudioList1413E0(page,*this,ordinal,list->entity);
    if(!finished.okay())return fail(finished.sourcePC);
    list->live=false;list->retiring=false;list->retired=true;
    // Keep count30, List.complete, audioFields.complete and all source words.
    // Only actual finishSceneRetirement2701CC later closes the aggregate.
    return true;
}
OriginalUiGaragePodScene50KG::C* OriginalUiGaragePodScene50KG::pageEntity3BC()noexcept {
    if(configFailure_||loadStage_<LoadStage::audioDescriptors||!currentPrefix()||!audioFields_||!audioFields_->complete||audioFields_->closed||
       !audioFields_->collision3BC||!audioFields_->collision3BC->live||
       !audioFields_->collision3BC->complete||audioFields_->collision3BC->count30!=8||
       !audioFields_->collision3BC->rows34)return nullptr;
    const auto& entity=audioFields_->collision3BC->entity;
    if(entity.kind!=K::entity141300||entity.reference00!=1||entity.sessionGeneration!=grant_->generation_||
       entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity)))return nullptr;
    return &audioFields_->collision3BC->entity;
}
OriginalUiGaragePodScene50KG::C* OriginalUiGaragePodScene50KG::pageEntity3C0()noexcept {
    if(configFailure_||loadStage_<LoadStage::audioDescriptors||!currentPrefix()||!audioFields_||!audioFields_->complete||audioFields_->closed||
       !audioFields_->driveOn3C0||!audioFields_->driveOn3C0->live||
       !audioFields_->driveOn3C0->complete||audioFields_->driveOn3C0->count30!=8||
       !audioFields_->driveOn3C0->rows34)return nullptr;
    const auto& entity=audioFields_->driveOn3C0->entity;
    if(entity.kind!=K::entity141300||entity.reference00!=1||entity.sessionGeneration!=grant_->generation_||
       entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity)))return nullptr;
    return &audioFields_->driveOn3C0->entity;
}
}
