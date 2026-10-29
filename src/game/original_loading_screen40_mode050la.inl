// Included once after the actual Application declaration. Source-only bounded
// resource stage; intentionally no startup, manager activation or race caller.
#include "original_loading_screen40_mode050la.h"
#include "../../tests/native/original_loading_collector50la.h"
#include "../../tests/native/original_race_display_lifetime50la.h"
#include "original_camera_start_effects.h"
#include "original_loading_screen_layout50la.inl"
namespace rr::game {
OriginalLoadingScreen40Mode0LA::~OriginalLoadingScreen40Mode0LA()noexcept {
    if(phase_!=Phase::retired)std::terminate();
}
OriginalLoadingScreen40Mode0LA::OriginalLoadingScreen40Mode0LA(const App* app,
    std::shared_ptr<OriginalApplicationEntities50LA> entities,std::shared_ptr<Domain> domain,
    std::shared_ptr<const Context> context)noexcept
    :issuer_(app),entities_(std::move(entities)),domain_(std::move(domain)),context_(std::move(context)){}
bool OriginalLoadingScreen40Mode0LA::current()const noexcept {
    return entities_&&entities_->nativeOwnerThread_==std::this_thread::get_id()&&issuer_&&
        issuer_->entitiesOwner_==entities_&&issuer_->standardMaterialDomainOwner_==domain_&&
        issuer_->materialContextOwner_==context_&&entities_->application10()&&
        !entities_->attachmentLock423F58()&&domain_&&domain_->prepared()&&context_&&context_->live()&&
        context_->materialApplication()==issuer_->materialApplication;
}
bool OriginalLoadingScreen40Mode0LA::ownsLoadingTextOccurrence(const Text* text,std::size_t i)const noexcept {
    return current()&&!retiring_&&phase_==Phase::loadingTexts&&i==nextText_&&i<texts_.size()&&
        text&&texts_[i].get()==text;
}
std::string& OriginalLoadingScreen40Mode0LA::actualMaterialDirectory()const noexcept {
    return issuer_->materialDirectory36B970_;
}
const std::string& OriginalLoadingScreen40Mode0LA::actualFontDirectory()const noexcept {
    return issuer_->fontDirectory36B420_;
}
bool OriginalLoadingScreen40Mode0LA::stageRaResources1CAA20(std::uint32_t& pc)noexcept {
    pc=stopPC_;if(!current()||attempted_||retiring_||phase_!=Phase::fresh)return false;
    attempted_=true;
    const auto stop=[&](std::uint32_t at)noexcept {stopPC_=pc=at;phase_=Phase::failed;return false;};
    try {
        auto& authority=entities_->entityAuthority_;
        const auto identity=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&entity_));
        if(entities_->objectCount423F60_==std::numeric_limits<std::uint32_t>::max()||
           !authority.reserveAdditional(1)||!authority.canEnroll(entity_,identity))return stop(0x141300);
        const auto made=initializeOriginalWreckComponent50LA(entity_,OriginalWreckComponentKind50LA::entity141300,
            static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(entities_.get())),identity,true,
            entities_->objectCount423F60_);
        if(made.status!=OriginalWreckComponentStatus50LA::complete||!authority.enroll(entity_))std::terminate();
        enrolled_=true;phase_=Phase::entity;vtable_=0x3cf520u;
        //1CA958..968 constructor-null texture34/layout38/progress3C. The
        //eleven Text slots remain unpublished until their own allocations.
        progress3C_=0;layout38_=nullptr;mode30_=0; //1CAA3C, bounded mode0 only.
        phase_=Phase::texture;stopPC_=pc=0x1caa58;
        if(issuer_->loadingTextureDirectory36BCA0_!="UI\\Textures\\")return stop(0x1caa58);
        looseRa_=rr::assets::OriginalLoadingTextureSource50LA::readRa(issuer_->assetRoot);
        if(!looseRa_)return stop(0x24daf8);
        const auto resolver=domain_->resolver3E1A70();
        if(!resolver)return stop(0x24daf8);
        // Native string/owner admission does not publish sourceTexture34 or
        // replace the actual loose-file132820/12DCB0 numeric producer.
        const std::string raName=issuer_->loadingTextureDirectory36BCA0_+"LD_RA.pst";
        std::size_t row=0;Resolver::UiSelection source;
        rr::assets::OriginalStandardTextureDiagnostic50LA textureDiagnostic;
        raFile_=rr::assets::OriginalLoadingRaFile50LA::admitNative(resolver,looseRa_,textureDiagnostic);
        if(!raFile_)return stop(textureDiagnostic.sourcePC?textureDiagnostic.sourcePC:0x24DAF8);
        std::shared_ptr<Texture> cached;
        stopPC_=pc=0x24DAF8;
        if(!raFile_->begin132820(row,source,raAttempt_,cached,textureDiagnostic))return stop(textureDiagnostic.sourcePC);
        if(cached){
            // Actual1CAA64/68 passes retainCached=1, whose call is24DCE4.
            // This owner currently has only cold exclusive backing retirement;
            // stop BEFORE acquiring a shared reference rather than later freeing
            // another owner's kernel or retaining an unretirable shared grant.
            return stop(0x24DCE4);
        }else{
            if(!raAttempt_)return stop(0x24DAF8); // Exact native attempt contract.

            stopPC_=pc=0x24DEE8;
            // Allocation refusal here retains the already reached file attempt.
            // No failed native control-block allocation is a sourceNULL34 store.
            raPendingTexture_=Texture::allocate();
            if(!resolver->bindLoadingPstResource(*raAttempt_,*raPendingTexture_,textureDiagnostic))
                return stop(textureDiagnostic.sourcePC);
            raPendingTexture_->loadingPstFile_=raAttempt_;
            if(!raPendingTexture_->construct24DA30(resolver,row,raName,context_,textureDiagnostic,
                static_cast<std::size_t>(-1),&source))return stop(textureDiagnostic.sourcePC);
            if(!raFile_->finishLoad24E094(raAttempt_,raPendingTexture_,textureDiagnostic))
                return stop(textureDiagnostic.sourcePC);
            texture34_=std::move(raPendingTexture_); //1CAA94 after1326A0+131040+actual return.
        }
        phase_=Phase::collector;stopPC_=pc=0x1caab4;
        if(!::original_main_entry50KG::OriginalLoadingCollector50LA::construct1CAAB4(*issuer_,collector_,pc))
            return stop(pc);
        phase_=Phase::fonts;stopPC_=pc=0x1caae4;
        fontArchive_=::original_main_entry50KG::readArchive(issuer_->assetRoot,rr::assets::OriginalUiArchiveId50KG::fonts);
        const auto provider=domain_->archiveProvider3E1A70();
        rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic archiveDiagnostic;
        if(!provider||!provider->mountAuthenticatedUi(fontArchive_,archiveDiagnostic)||
           !provider->sealNativeMounts(archiveDiagnostic))return stop(0x12cea0);
        // Typed transport wrapper admission is native; it is not a retail
        // Font allocation or a completed FileManager registration receipt.
        fontSource_.reset(new rr::assets::OriginalLoadingFontSource50LA);
        if(!fontSource_->mount1CAAE4(resolver,provider,fontArchive_,fontBank_,textureDiagnostic))
            return stop(textureDiagnostic.sourcePC);
        fonts_=issuer_->fonts328();
        rr::assets::OriginalApplicationFontDiagnostic50LA fontDiagnostic;
        if(!fonts_->loadLoadingMode0_1CAB00(fontArchive_,fontSource_,actualMaterialDirectory(),actualFontDirectory(),fontDiagnostic))
            return stop(fontDiagnostic.sourcePC);
        phase_=Phase::loadingTexts;
        constexpr std::uint32_t calls[11]={0x1cab80,0x1cabe4,0x1cac78,0x1cac94,0x1cacb0,
            0x1caccc,0x1cace8,0x1cad04,0x1cad20,0x1cad3c,0x1cad58};
        for(;nextText_<texts_.size();++nextText_){
            if(nextText_<2){
                stopPC_=pc=nextText_?0x1cabb4u:0x1cab50u;
                texts_[nextText_].reset(new Text);
            }else if(nextText_==2){
                //1CABF4..1CAC60: all nine allocations precede their loads,
                //in interleaved48/54/60,4C/58/64,50/5C/68 source order.
                for(std::size_t group=0;group<3;++group){
                    constexpr std::uint32_t allocations[3]={0x1cabf4,0x1cac14,0x1cac34};
                    for(std::size_t column=0;column<3;++column){
                        stopPC_=pc=allocations[column];
                        texts_[2+group+3*column].reset(new Text);
                    }
                }
            }
            stopPC_=pc=calls[nextText_];
            Text::Diagnostic textDiagnostic;
            if(!texts_[nextText_]->loadLoadingSource2DFD90(*this,nextText_,textDiagnostic))return stop(textDiagnostic.sourcePC);
        }
        phase_=Phase::population;stopPC_=pc=0x1cad64;
        rr::assets::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
        if(!domain_->populate(context_,materialDiagnostic))return stop(materialDiagnostic.sourcePC);
        phase_=Phase::bankClose;stopPC_=pc=0x1cad94;
        if(!fontSource_->retireBank1CAD94(fontBank_,textureDiagnostic))return stop(textureDiagnostic.sourcePC);
        //141C20 is the separate inert Entity leaf, not LSI construction/render.
        phase_=Phase::resources;stopPC_=pc=0x1cada8;return true;
    }catch(...){return stop(stopPC_);}
}
bool OriginalLoadingScreen40Mode0LA::stageLsiPreamble1CC580(std::uint32_t& pc)noexcept {
    pc=0x1CC580;
    if(!current()||retiring_||lsiPreambleAttempted_||phase_!=Phase::resources||mode30_!=0)return false;
    const auto display=issuer_->raceDisplayOwner50LA_.lock();
    rr::platform::VehicleWindowDiagnostic50DS renderer;
    if(!display||display->application_!=issuer_||!display->video_||!display->video_->originalMain||
       !display->frontendFirstViewportCurrent50KG(renderer))return false;
    // Host storage/lifetime admission precedes the first source renderer call.
    // This does not assert a reached source17C130 dispatch or force manager3C.
    std::string directory;
    try {directory="UI\\Textures\\";}catch(...){return false;}
    lsiDisplay_=display;display->pin();lsiDisplayPinned_=true;
    lsiPreambleAttempted_=true;phase_=Phase::lsiPreamble;
    const auto stop=[&](std::uint32_t at)noexcept {stopPC_=pc=at;phase_=Phase::failed;return false;};
    std::int32_t returned=0;
    const auto drained=serviceOriginalRenderer244D60(display->dispatch_,true,display->renderer_,renderer,returned,
        OriginalCameraDrainPolicy::initializedContext); //1CC5A4(a1=1).
    if(drained.status!=OriginalCameraStartStatus::complete)return stop(drained.sourcePC);
    lsiDrainPhase_=1;
    if(!display->renderer_.finishInitializedContextWork(renderer)||!renderer.contextWorkCompleted)return stop(0x245090);
    lsiDrainPhase_=2; // Actual unconditional second virtual call1CC5BC.
    // Actual Video259E34 vtable3D22C0 +50 is25B9D0: only four bytes.
    // Do NOT reset fade3E8, frame3C, sourceMode44 or other End-view fields.
    for(auto& view:display->video_->sourceEndViews_)view.overlay494=0;
    lsiVideo494ClearReached_=true;
    if(issuer_->uiResourceDirectory36BB90_)issuer_->uiResourceDirectory36BB90_->swap(directory);
    else issuer_->uiResourceDirectory36BB90_.emplace(std::move(directory));
    lsiDirectoryWriteReached_=true; //1CC604/608/614 exact mode0 string store.
    // The full directory includes the actual1CC614 call delay-slot byte.
    phase_=Phase::lsiAllocationBoundary;stopPC_=pc=0x1CC610;
    return stageLsiLayout1CC610(pc);
}
const std::optional<std::string>& OriginalLoadingScreen40Mode0LA::actualLoadingUiDirectory()const noexcept {
    return issuer_->uiResourceDirectory36BB90_;
}
std::shared_ptr<const OriginalLoadingScreen40Mode0LA::Archive> OriginalLoadingScreen40Mode0LA::readLoadingIconNative()const {
    return ::original_main_entry50KG::readArchive(issuer_->assetRoot,rr::assets::OriginalUiArchiveId50KG::loadingIcon);
}
bool OriginalLoadingScreen40Mode0LA::stageLsiLayout1CC610(std::uint32_t& pc)noexcept {
    pc=0x1CC610;
    if(!current()||retiring_||phase_!=Phase::lsiAllocationBoundary||layoutAttempt_)return false;
    const auto stop=[&](std::uint32_t at)noexcept {stopPC_=pc=at;phase_=Phase::failed;return false;};
    if(!OriginalLoadingScreenLayout50LA::admitNative(*this,layoutAttempt_))return stop(pc);
    if(!layoutAttempt_->allocateAndConstruct1CC610(pc))return stop(pc);
    layout38_=layoutAttempt_->sourceIdentity_; //1CC628, INCLUDING actualmallocNULL.
    phase_=Phase::lsiLayout;pc=0x1CC634;
    if(!layoutAttempt_->load2CCDA0(pc))return stop(pc);
    return stop(0x1CC63C); // No unsupported loader return/Screen suffix admission.
}
bool OriginalLoadingScreen40Mode0LA::retireRaNative()noexcept {
    if(raPendingTexture_&&texture34_)return false;
    auto& resource=raPendingTexture_?raPendingTexture_:texture34_;
    // Complete every refusal gate before closing an attempt or freeing native
    // palette/FC/mip backing. A live source table keeps its association until
    // the same Resource destructor reaches1327D0. A lost source table instead
    // requires the exact separately-ledgered native old-backing withdrawal.
    if(resource&&(resource.use_count()!=1||!resource->preflightLoadingPstNative()))return false;
    rr::assets::OriginalStandardTextureDiagnostic50LA d;
    if(raAttempt_){
        if(!raFile_||!raFile_->abortPendingNative(raAttempt_,d))return false;
    }
    if(resource){
        if(resource->loadingPstFile_&&!resource->closeLoadingPstNative(d))return false;
        if(!resource->release104970())return false;
        resource.reset(); // Source clear or exact native withdrawal precedes owner disposal.
    }
    raAttempt_.reset();
    if(raFile_){if(!raFile_->closeNative(d))return false;raFile_.reset();}
    return true;
}
bool OriginalLoadingScreen40Mode0LA::retireNativeExclusive()noexcept {
    if(phase_==Phase::retired)return true;
    if(!current()||(lsiDisplayPinned_&&!lsiDisplay_))return false;
    retiring_=true;
    // Newer Layout attempt may own a real Config/bank even on source38NULL.
    // Refusal retains the attempt and the shared Display/Application owners.
    if(layoutAttempt_){if(!layoutAttempt_->closeNative())return false;layoutAttempt_.reset();}
    // A failed cold Font still owns its real CEL and a pointer into this exact
    // Application material-directory string. Refusal retains all Screen/App
    // custody; never free that pending Font or bank before the exact abort.
    if(fontSource_&&fonts_&&!fonts_->abortLoadingCelNative(fontSource_))return false;
    // Explicit native partial unwind. Do not replay reached renderer work or
    // claim source global-collector retirement, font unload or LSI destructor.
    // Preserve Text call order1CC74C/758 and1CC768..7AC, without claiming
    // unreached282570 font unloads or completed loading callbacks.
    constexpr std::size_t order[11]={0,1,2,5,8,3,6,9,4,7,10};
    for(const auto i:order){auto& text=texts_[i];
        if(text){Text::Diagnostic d;if(!text->retire2E1340(d))return false;text.reset();}
    }
    if(fontSource_){rr::assets::OriginalStandardTextureDiagnostic50LA d;
        if(!fontSource_->closeNative(fontBank_,d))return false;
        fontSource_.reset();
    }
    if(!retireRaNative())return false;
    if(collector_){if(!collector_->retireNativeExclusive())return false;collector_.reset();}
    if(enrolled_){
        if(entity_.reference00!=1||entity_.parent1C||entity_.firstChild18||entity_.previous10||entity_.next14||
           !entities_->entityAuthority_.canRetire(entity_)||!entities_->objectCount423F60_)return false;
        if(!entities_->entityAuthority_.retire(entity_))std::terminate();
        --entity_.reference00;--entities_->objectCount423F60_;enrolled_=false;
    }
    // Native partial bank withdrawal above is not a source1CAD94 receipt.
    // Published Application Fonts survive; source row/capacity history is not
    // reset to constructor values by native cleanup.
    fonts_.reset();fontArchive_.reset();looseRa_.reset();
    if(lsiDisplayPinned_){if(!lsiDisplay_)return false;lsiDisplay_->unpin();lsiDisplayPinned_=false;}
    lsiDisplay_.reset();phase_=Phase::retired;return true;
}
}
