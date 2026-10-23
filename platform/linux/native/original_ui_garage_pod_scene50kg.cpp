#include "original_ui_garage_audio_fields50kg.h"
#include "original_ui_garage_scene_emitters50kg.h"
#include "original_ui_garage_scene_particles50kg.h"
#include "original_ui_garage_scene_collision50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_toy_fields50kg.h"
#include "original_ui_garage_scene_lighting50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_setup50kg.h"
#include "original_ui_garage_scene_dispatch50kg.h"
#include "original_ui_garage_toy_fields50kg.h"
#include "original_ui_garage_static_models50kg.h"
#include "original_ui_main_audio50kg.h"
#include "original_ui_main_session50kg.h"
#include "original_ui_garage_actors50gn.h"
#include "original_audio_pending_waves50kg.h"
#include <cstring>
#include <exception>
namespace rr::platform {namespace {
constexpr std::array<const char*,4> sourceNames{"sfx\\race\\pdwalk01.vag","sfx\\race\\onfire01.vag",
        "sfx\\race\\exting01.vag","sfx\\race\\pdfall01.vag"};
constexpr std::array<const char*,4> directoryNames{"audio\\sfx\\race\\pdwalk01.vag","audio\\sfx\\race\\onfire01.vag",
    "audio\\sfx\\race\\exting01.vag","audio\\sfx\\race\\pdfall01.vag"};
bool sceneFailure(std::string& error,const char* message)noexcept {
    try{error=message;}catch(...){error.clear();}return false;
}
}
OriginalUiGaragePodScene50KG::OriginalUiGaragePodScene50KG()noexcept=default;
bool OriginalUiGaragePodScene50KG::borrowLatePageChildren1B4CA0(
    const OriginalUiGaragePageOwner50KG& page,std::array<C*,3>& children,D& d)noexcept {
    d={};children.fill(nullptr);
    auto stop=[&](std::uint32_t pc){d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=pc;return false;};
    if(!sourceLoadReturned1FB59C()||!currentPrefix()||page.scene4C0_!=this||page.sceneBacking_.get()!=this||
       page.grant_!=grant_||page.sceneLateBorrowCaptured_||page.sceneLateAttachCursor_||
       !page.sceneAttachmentPrefixCurrent1B48A0()||page.cameraHandoffPhase_!=3||
       !page.cameraRegistrationCurrent1FB608()||!toyFields_||!toyFields_->complete_||toyFields_->closed_||
       !audioFields_||!audioFields_->complete||audioFields_->closed||!sceneEmitters_||
       !sceneEmitters_->complete||sceneEmitters_->failed||sceneEmitters_->completed!=7||
       sceneEmitters_->retired||sceneEmitters_->listRetiring||sceneEmitters_->listRetired||
       sceneEmitters_->payloadReleased)return stop(0x1B4CA0);
    //These are outcomes of the retained constructor/readers, not invented
    //empty lists. Missing Triggers really retires3D0 at1B5070; zero-count
    //Ambient/Soultree readers really retire/null3B4/3B8. Positive branches
    //need their own owners and cannot be silently skipped here.
    if(toyFields_->triggers3D0_||water400_||ecosystem408_||field540_||field534_||
       audioFields_->ambient3B4||audioFields_->soultree3B8)return stop(0x1B4CC4);
    auto* list=toyFields_->particles3D8_.get();
    if(!list||!list->live||list->count30!=7||!list->records34||!sceneParticles_)return stop(0x1B4D24);
    for(std::size_t i=0;i<7;++i){const auto* wrapper=list->records[i].get();const auto* row=sceneEmitters_->rows[i].get();
        if(!wrapper||list->records34[i]!=wrapper||wrapper->temp04||!wrapper->live00||
           wrapper->unread08To10[1]!=1||!row||wrapper->live00[0]!=&row->entity||
           !row->entityLive||!row->numericConstructed||!row->loaded||!row->attached||row->retiring||row->retired||
           row->entity.reference00!=1||row->entity.parent1C!=&list->entity||
           row->manager70!=&sceneParticles_->entity)return stop(0x1B4D24);
    }
    std::array<C*,3> actual{{&list->entity,pageEntity3BC(),pageEntity3C0()}};
    for(std::size_t i=0;i<actual.size();++i){const auto* c=actual[i];
        if(!c||c->kind!=K::entity141300||c->reference00!=1||c->sessionGeneration!=grant_->generation_||
           c->objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(c))||
           c->parent1C||c->previous10||c->next14||c->mask20||(i&&c->firstChild18))return stop(0x1B4CA0);
    }
    children=actual;return true;
}
bool OriginalUiGaragePodScene50KG::borrowPageChildren1B48A0(
    const OriginalUiGaragePageOwner50KG& page,std::array<C*,10>& children,D& d)noexcept {
    d={};children.fill(nullptr);
    auto stop=[&](std::uint32_t pc){d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=pc;return false;};
    if(!sourceLoadReturned1FB59C()||!currentPrefix()||page.scene4C0_!=this||page.sceneBacking_.get()!=this||
       page.grant_!=grant_||page.scenePageBorrowCaptured_||page.scenePageAttachCursor_||
       !visibility_||!sceneSetup_||!sceneLighting_||!sceneParticles_||!sceneDispatch_||!toyFields_||
       !staticModels_||!sceneCollision_||!actorOwner3C8_||numZones34C_!=1||
       !attachmentReset270550Known_||!attachmentArrays26FD68Known_||!buoyNull1B2904Observed_||
       cubeModels3F8_||terrainModel3FC_||racerCount4AC_)
        return stop(0x1B48A0);
    //The genuine SCN/TOY owners establish these reached zero/"none" branches.
    //No optional positive owner can be silently omitted from141960 ordering.
    const auto* environment=sceneSetup_->environment3A4_.get();
    if(!environment||environment->cubeCount00||std::strcmp(environment->terrain0C.get(),"none")||
       std::strcmp(environment->water110.get(),"none")||std::strcmp(environment->ecosystem318.get(),"none")||
       !toyFields_->complete_||toyFields_->closed_||toyFields_->physicsCount4B0_||toyFields_->cameraCount3E4_||
       toyFields_->sunFlareCount4A8_||toyFields_->buoyCount538_||toyFields_->physics4B4_||
       toyFields_->sunFlares4A4_||toyFields_->buoys53C_)return stop(0x1B4904);
    //1B4B48 may observe a real positive Application28C. The independent
    //constructor4AC==0 excludes that loop; no raced-world absence is inferred.
    for(std::size_t i=0;i<12;++i)if(racerHelpers40C_[i]||racers43C_[i]||racerHelpers46C_[i])
        return stop(0x1B4B10);
    auto& lighting=*sceneLighting_;auto* const lights=sceneSetup_->lights3A8_.get();
    if(!lights||!lights->entityLive||lights->count30!=2||!lighting.identityCurrent()||!lighting.lightsComplete_||
       lighting.fogStage_!=OriginalUiGarageSceneLighting50KG::FogStage::complete||
       !lighting.fog3EC_||!lighting.reset3F4_||!lighting.proxy3F0_||
       lighting.proxy3F0_->fog30!=&lighting.fog3EC_->entity)return stop(0x1B48EC);
    for(std::size_t i=0;i<2;++i){const auto& light=lighting.lights_[i];
        if(!light||!lighting.lightIdentity(*light)||!light->configured||!light->registered||!light->attached||
           light->retiring||light->retired||light->entity.parent1C!=&lights->entity)return stop(0x1B48EC);}
    for(std::size_t i=0;i<3;++i)if(!lighting.fogConstructed_[i]||lighting.fogRetiring_[i]||lighting.fogRetired_[i])
        return stop(0x1B4928);
    if(!sceneCollision_->current2743B0(*this)||!sceneDispatch_->collisionZone||
       !sceneDispatch_->collisionZone->live||!sceneDispatch_->collisionLists3CC||
       sceneDispatch_->collisionLists3CC[0]!=sceneDispatch_->collisionZone.get())return stop(0x1B4A38);
    auto& particles=*sceneParticles_;
    if(!particles.entityLive||!particles.prepared||particles.retiring||particles.retired||!particles.numeric||
       !particles.auxiliary1C0||!particles.material150)return stop(0x1B4AE0);
    C* manager=nullptr;C* toys=nullptr;
    if(!actorOwner3C8_->borrowPageEntities1B4A68(*this,manager,toys))return stop(0x1B4A68);
    std::array<C*,10> found{{visibility_->pageEntity4BC(),&lights->entity,&lighting.fog3EC_->entity,
        &lighting.reset3F4_->entity,&lighting.proxy3F0_->entity,staticModels_->pageEntity3C4(),
        &sceneDispatch_->collisionZone->entity,manager,toys,&particles.entity}};
    constexpr std::array<K,10> kinds{{K::entity141300,K::entity141300,K::fog13E6B0,K::fogOff13F850,
        K::fogOn13F980,K::staticModels27B610,K::entity141300,K::animationManager101330,
        K::toyList2D9B40,K::particleManager160B10}};
    for(std::size_t i=0;i<found.size();++i){const auto* c=found[i];
        if(!c||c->kind!=kinds[i]||c->reference00!=1||c->parent1C||c->previous10||c->next14||c->mask20||
           c->sessionGeneration!=grant_->generation_||
           c->objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(c)))return stop(0x1B48A0);
        for(std::size_t j=0;j<i;++j)if(found[j]==c)return stop(0x1B48A0);
    }
    children=found;return true;
}
OriginalUiGaragePodScene50KG::~OriginalUiGaragePodScene50KG()noexcept {
    // Unjoined source teardown cannot discard an actual counted prefix.
    if(sceneLive_||groupLive_||group4D8_.count48_||config34_||config38_)std::terminate();
}
bool OriginalUiGaragePodScene50KG::construct1B2870(std::shared_ptr<Grant> grant,
    std::shared_ptr<const Assets> assets,std::shared_ptr<OriginalUiMainAudio50KG> main,
    OriginalUiGarageDroidAudio50GN& backend,std::shared_ptr<OriginalAudioPendingWaves50KG> pending,
    std::string& error)noexcept {
    if(attempted_||!grant||!grant->current()||!assets||!assets->loaded()||!main||
       !backend.sourceConstructionPrefixCurrent()||backend.sourceSceneGroup_||!pending||!pending->current()||
       pending->issuer_!=grant->issuer()||pending->authority_.lock()!=grant->application()||
       pending->authority_.lock().owner_before(grant->application())||
       grant->application().owner_before(pending->authority_.lock()))
        return sceneFailure(error,"PodScene requires one actual cold grant/audio prefix");
    attempted_=true;grant_=std::move(grant);assets_=std::move(assets);
    main_=std::move(main);backend_=&backend;pendingWaves_=std::move(pending);
    try {
        //26FC9C is a NEW counted Entity.270550 clears config34 and visibility
        //pointers, not ownership of the later immutable SceneAssets parser.
        auto step=grant_->constructGaragePodScene1FA308(this,podScene_);
        if(!step.okay())return sceneFailure(error,"PodScene Entity constructor refused; prefix retained");
        sceneLive_=true;mode30_=1;
        cubeModels3F8_.reset(); //2705C8, reached through base constructor slot78.
        terrainModel3FC_=nullptr; //2705CC.
        attachmentReset270550Known_=true;
        //Intentional native transactional preallocation: the existing shared
        //authority helper combines Entity construction and slot claim. Original
        //2350D0 constructs its Entity BEFORE list allocation; this preflight is
        //NOT an assertion of exact guest allocation-failure ordering. It still
        //retains the genuinely counted enclosing Scene on native failure.
        group4D8_.members4C_=std::make_unique<Sound*[]>(8);
        step=grant_->constructSceneSoundGroup2350D0(group4D8_.entity,group4D8_.slot40_);
        if(!step.okay())return sceneFailure(error,"Scene SoundGroup shared25-slot claim refused; prefix retained");
        groupLive_=true;group4D8_.manager5C=main_.get();group4D8_.capacity54_=8;group4D8_.growth50_=16;group4D8_.enabled_=true;
        group4D8_.volume38_=100;group4D8_.pan3C_=0;group4D8_.state44_=0;
        backend.sourceSceneGroup_=this;
        for(std::size_t i=0;i<12;++i){
            racerHelpers40C_[i]=nullptr; //26FD40.
            racers43C_[i]=nullptr; //26FD4C.
            racerHelpers46C_[i]=nullptr; //26FD60.
        }
        racerCount4AC_=0;attachmentArrays26FD68Known_=true; //26FD68.
        //1B28A4 follows the whole base Scene/group constructor. Preserve this
        //fallible allocation AFTER counted ownership, never discard/replay it.
        worldList500_=std::make_unique<C*[]>(10);
        capacity504_=10;growth508_=100000;count50C_=0;
        interpolation510_.reset();interpolation514_.reset();field518_=0;
        grant_->application()->worldRegistration434320_=0; //Actual shared1B28D0 store.
        mode524_=3;trackEmitsDust51C_=false;field534_=0;num321Anims528_=0;
        animationRows52C_.reset();animationRows530_.reset();
        //53C is represented by the one later ToyFields.buoys53C_ allocation
        //owner, absent here and initialized null before any source read.
        field540_=0;
        buoyNull1B2904Observed_=true; //Actual1B2904 zero; positive TOY path unsupported.
        constructed_=true;error.clear();return true;
    }catch(...){return sceneFailure(error,"PodScene allocation failed; exact counted prefix retained");}
}
bool OriginalUiGaragePodScene50KG::currentPrefix()const noexcept {
    if(!constructed_||!sceneLive_||!groupLive_||!grant_||!grant_->current()||!main_||
       !pendingWaves_||!pendingWaves_->current()||pendingWaves_->issuer_!=grant_->issuer()||
       !backend_||backend_->sourceSceneGroup_!=this||!backend_->transportValid()||
       !grant_->podSceneOwnerCurrent(this,podScene_)||
       !grant_->sceneSoundGroupCurrent(group4D8_.entity,group4D8_.slot40_)||
       podScene_.reference00!=1||podScene_.kind!=K::entity141300||
       podScene_.sessionGeneration!=grant_->generation_||
       podScene_.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&podScene_))||
       podScene_.mask20||podScene_.descendants24||podScene_.parent1C||podScene_.previous10||podScene_.next14||podScene_.firstChild18||
       group4D8_.entity.reference00!=1||group4D8_.entity.mask20||group4D8_.entity.descendants24||group4D8_.entity.parent1C||group4D8_.entity.previous10||
       group4D8_.entity.next14||group4D8_.entity.firstChild18||!group4D8_.members4C_||group4D8_.capacity54_!=8||group4D8_.growth50_!=16||
       !group4D8_.enabled_||group4D8_.manager5C!=main_.get()||group4D8_.volume38_!=100||group4D8_.pan3C_||group4D8_.state44_||!worldList500_||capacity504_!=10||
       growth508_!=100000||count50C_||mode30_!=1||mode524_!=3||group4D8_.count48_>group4D8_.sounds_.size())return false;
    for(std::size_t i=0;i<group4D8_.sounds_.size();++i){
        if(i<group4D8_.count48_){if(!sourceRowIdentityCurrent(*backend_,i,false))return false;}
        else if(group4D8_.sounds_[i])return false;
    }
    for(std::size_t i=group4D8_.count48_;i<group4D8_.capacity54_;++i)if(group4D8_.members4C_[i])return false;
    return true;
}
bool OriginalUiGaragePodScene50KG::prepareStaticDescriptors2710F0(
    std::shared_ptr<OriginalUiGarageModelResources50KG> resources,D& d)noexcept {
    d={};using S=assets::OriginalStandardMaterialStatus50LA;
    const auto fail=[&](S status,std::uint32_t pc){d.status=status;d.sourcePC=pc;return false;};
    if(staticDescriptorsAttempted_||visibilityAttempted_||!currentPrefix()||
       loadStage_!=LoadStage::sceneStructure||!liveSceneConfig34()||resources!=configResources_||
       numZones34C_!=1||staticZoneLists3C4_)
        return fail(S::phase,0x2710F0);
    staticDescriptorsAttempted_=true;
    try {
        //271124/12C precedes the first real27B610 list constructor. Native
        //zeroed pointer storage is private until that constructor publishes.
        staticZoneLists3C4_=std::make_unique<C*[]>(1);
        staticModels_=std::shared_ptr<OriginalUiGarageStaticModels50KG>(new OriginalUiGarageStaticModels50KG());
    }
    catch(...){return fail(S::allocation,0x271128);}
    if(!staticModels_->prepare2710F0(*this,grant_,assets_,std::move(resources),d))return false;
    loadStage_=LoadStage::statics;return true;
}
bool OriginalUiGaragePodScene50KG::populateStaticModels274160(D& d)noexcept {
    d={};using S=assets::OriginalStandardMaterialStatus50LA;
    const auto fail=[&](S status,std::uint32_t pc){d.status=status;d.sourcePC=pc;return false;};
    if(!currentPrefix()||populationStage_!=PopulationStage::instances||!staticModels_||!visibility_)
        return fail(S::phase,0x274160);
    if(!staticModels_->populate274160(visibility_,d))return false;
    populationStage_=PopulationStage::statics;return true;
}
bool OriginalUiGaragePodScene50KG::prepareVisibility2729E0(
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility,D& diagnostic)noexcept {
    diagnostic={};
    if(visibilityAttempted_||!currentPrefix()||loadStage_!=LoadStage::audioClosed||config34_||config38_||
       !visibility||visibility->attempted_||
       visibility->closed_||visibility->sourceSceneOwner_){
        diagnostic.status=assets::OriginalStandardMaterialStatus50LA::phase;diagnostic.sourcePC=0x2729E0;return false;
    }
    visibilityAttempted_=true;visibility_=std::move(visibility);
    //This is the DISTINCT4BC constructor plus its two search nodes. It does
    //not attach either4BC or this PodScene to a fabricated Page.
    visibility_->sourceSceneOwner_=this;
    if(!visibility_->prepare2729E0(grant_,assets_,diagnostic))return false;
    populationStage_=PopulationStage::visibility;return true;
}
bool OriginalUiGaragePodScene50KG::sourceRowIdentityCurrent(
    const OriginalUiGarageDroidAudio50GN& backend,std::size_t ordinal,bool registered)const noexcept {
    if(!constructed_||!groupLive_||backend_!=&backend||backend.sourceSceneGroup_!=this||
       !grant_||!grant_->sceneSoundGroupCurrent(group4D8_.entity,group4D8_.slot40_)||
       !pendingWaves_||!pendingWaves_->current()||ordinal>=group4D8_.count48_||
       ordinal>=group4D8_.sounds_.size()||!backend.assets_)return false;
    const auto* sound=group4D8_.sounds_[ordinal].get();
    const auto* resource=pendingWaves_->directory_->find(directoryNames[ordinal]);
    return resource&&sound&&group4D8_.members4C_[ordinal]==sound&&sound->reference00==1&&
       sound->ordinal==ordinal&&sound->group34==&group4D8_&&sound->manager30==main_.get()&&
       sound->authenticatedSource==&backend.assets_->clips[ordinal]&&sound->flags10==1&&
       !sound->stream1A8&&!sound->stereo1AC&&!sound->loop1B0&&
       (!registered||(sound->sourceRegistered&&sound->resourceA0&&*sound->resourceA0==resource->sourceIndex&&
         sound->sampleContinuationPending!=sound->nativeSample28.has_value()&&
         static_cast<bool>(sound->nativeSound)==sound->nativeSample28.has_value()&&
         std::strncmp(sound->originalNameA4.data(),sourceNames[ordinal],std::strlen(sourceNames[ordinal])+1)==0));
}
bool OriginalUiGaragePodScene50KG::appendSoundPrefix278F60(
    OriginalUiGarageDroidAudio50GN& backend,std::size_t actor,std::size_t ordinal,
    const assets::AudioClip50HC& source,std::string& error)noexcept {
    constexpr std::array<std::size_t,4> actors{0,5,6,9};
    // Exact authenticated Garage TOY Animation1/6/7/10 Sound1 values. These
    // original callback names, not uppercase native filesystem requests, are
    // copied back by232E60 AFTER233110 successfully registers the pending row.

    if(sourceConstructionFailed_||!currentPrefix()||&backend!=backend_||ordinal>=group4D8_.sounds_.size()||
       soundAttempted_[ordinal]||ordinal!=group4D8_.count48_||actor!=actors[ordinal]||!backend.assets_||
       &source!=&backend.assets_->clips[ordinal]||backend.rows_[ordinal].source||backend.rows_[ordinal].sound)
        return sceneFailure(error,"Scene Sound factory is not the next exact retained prefix");
    soundAttempted_[ordinal]=true;
    try {
        auto row=std::make_unique<Sound>();row->group34=&group4D8_;row->manager30=main_.get();
        row->ordinal=ordinal;row->authenticatedSource=&source;
        group4D8_.sounds_[ordinal]=std::move(row);
        if(!grant_->noteSceneSoundInsert235270(group4D8_.entity,group4D8_.slot40_)){
            sourceConstructionFailed_=true;
            return sceneFailure(error,"Scene Sound insertion refused; allocated source row retained");
        }
        group4D8_.members4C_[group4D8_.count48_++]=group4D8_.sounds_[ordinal].get();
        auto& sound=*group4D8_.sounds_[ordinal];sound.flags10=1;
        sound.sampleContinuationPending=true;
        if(!pendingWaves_->registerSceneSound2332A0(*this,ordinal,sourceNames[ordinal],error)){
            sourceConstructionFailed_=true;return false;
        }
        //2332C8 returns1 with sample28 still unresolved.232E60 copies A4;
        //278F90 can retain the callback BORROW. No native clip/gain/play occurs.
        const auto length=std::strlen(sourceNames[ordinal]);
        std::memcpy(sound.originalNameA4.data(),sourceNames[ordinal],length+1);
        sound.sourceRegistered=true;error.clear();return true;
    }catch(...){sourceConstructionFailed_=true;
        return sceneFailure(error,"Scene Sound native allocation failed; exact prefix retained");}
}
// Add inside namespace rr::platform in original_ui_garage_pod_scene50kg.cpp.
// The existing .cpp already includes original_ui_main_audio50kg.h.
bool OriginalUiGaragePodScene50KG::coldRoutingIdentityCurrent(
    const OriginalUiMainAudio50KG* main)const noexcept {
    // Pure structural source half. It remains usable during the exact atomic
    // native sample publication interval; do not call backend.current here.
    // Group4D8 is detached from Scene/Page. Page topology grants no audio lease.
    const auto& g=group4D8_;const auto& e=g.entity;
    if(!main||main_.get()!=main||!constructed_||!sceneLive_||!groupLive_||sourceConstructionFailed_||
       !grant_||!grant_->current()||!grant_->sceneSoundGroupCurrent(e,g.slot40_)||
       e.reference00!=1||e.kind!=K::soundGroup2350D0||e.sessionGeneration!=grant_->generation_||
       e.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e))||
       e.parent1C||e.previous10||e.next14||e.firstChild18||e.mask20||e.descendants24||
       g.manager5C!=main||!g.enabled_||g.slot40_<0||g.slot40_>=25||
       g.volume38_!=100||g.pan3C_||g.state44_||g.count48_!=4||g.capacity54_!=8||g.growth50_!=16||
       !g.members4C_||!backend_||backend_->sourceSceneGroup_!=this||backend_->nextActor_!=10||
       backend_->constructedSounds_!=4||backend_->retired_||backend_->poisoned_||backend_->paused_)return false;
    for(std::size_t i=0;i<g.sounds_.size();++i)
        if(!sourceRowIdentityCurrent(*backend_,i,true)||g.sounds_[i]->reference00!=1)return false;
    for(std::size_t i=g.count48_;i<g.capacity54_;++i)if(g.members4C_[i])return false;
    return true;
}
bool OriginalUiGaragePodScene50KG::configureReverb235520(std::string& error)noexcept {
    // ONLY the real Page1FB594 continuation calls this, after all preceding
    // Scene dispatches and the actual shared-global restoration1FB520..588.
    // Source Sound rows may still be pending until manager flush1D1B9C.
    if(finalLoadStage_!=FinalLoadStage::namespaceRestored||!savedModelDirectory1FB1B4_||
       !sceneLoadTailPrerequisitesCurrent()||reverbAttempted_||!coldRoutingIdentityCurrent(main_.get())){
        error="1FB594 Scene235520 is not the next exact source call";return false;
    }
    reverbAttempted_=true;
    const auto retained=nativeRoutingLifetime_.lock();
    if(!retained||retained.get()!=this){
        error="1FB594 has no Root-issued Page/Scene lifetime alias";return false;
    }
    // Actual append, not a Boolean stand-in. No wet output or Page-ready
    // claim follows. Accepted native custody keeps the same source counts.
    return main_->enqueueSceneReverb26D6C0(retained,error);
}
bool OriginalUiGaragePodScene50KG::routingEnqueueCurrent235520()const noexcept {
    return reverbAttempted_&&main_&&main_->queuedSceneReverbCurrent(*this);
}
// Apply inside namespace rr::platform in the coherent PodScene successor.
// Requires original_audio_pending_waves50kg.h and original_ui_main_audio50kg.h.
// The private declarations are in pod-scene-private-declarations.inc.
// REPLACES the predecessor's unconditional routing retirement refusal.
bool OriginalUiGaragePodScene50KG::preflightRoutingRetirement235150()const noexcept {
    return main_&&groupLive_&&pendingWaves_&&pendingWaves_->canRetire(*this)&&backend_&&
        backend_->preflightSourceSceneRetirement2353D0(*this)&&main_->mayRetireQueuedSceneGroup(*this);
}
bool OriginalUiGaragePodScene50KG::soundGroupRetirementEntered235150()const noexcept {
    // This phase is written only after the shared canonical registry helper
    // performed142590/104970/23518C. A negative live lookup alone is not proof.
    return soundGroupRetirement_==SoundGroupRetirement::registryReleased&&
        retirementStage_==RetirementStage::configClosed&&sceneLive_&&groupLive_&&
        grant_&&grant_->scenePodRetirementCurrent(podScene_)&&group4D8_.entity.reference00==0&&
        !grant_->sceneSoundGroupCurrent(group4D8_.entity,group4D8_.slot40_);
}
bool OriginalUiGaragePodScene50KG::soundGroupSourceRetired235150()const noexcept {
    const auto& g=group4D8_;
    if(soundGroupRetirement_!=SoundGroupRetirement::complete||
       retirementStage_<RetirementStage::groupReleased||groupLive_||!grant_||!grant_->current()||
       g.entity.reference00||g.count48_||g.members4C_||g.capacity54_||pendingWaves_||
       g.entity.kind!=K::soundGroup2350D0||g.entity.sessionGeneration!=grant_->generation_||
       g.entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&g.entity))||
       grant_->sceneSoundGroupCurrent(g.entity,g.slot40_))return false;
    for(const auto& sound:g.sounds_)if(sound)return false;
    return true;
}
bool OriginalUiGaragePodScene50KG::retiredSoundGroupRoutingIdentity(
    const OriginalUiMainAudio50KG* main)const noexcept {
    // Pure structural tombstone: exact counted Group finished, no source row
    // remains. Any stopped raw Mixer clip remains pinned by the retained native
    // backend until replacement/actual transport retirement. Other payloads may remain.
    // Do NOT require later zones/lights/visibility already absent at26FDD4.
    return main&&main_.get()==main&&soundGroupSourceRetired235150()&&backend_&&
        group4D8_.manager5C==main&&group4D8_.slot40_>=0&&group4D8_.slot40_<25&&
        !backend_->sourceSceneGroup_&&backend_->sourceSceneGroupReleased_&&
        backend_->retiredSceneTransportCurrent();
}
bool OriginalUiGaragePodScene50KG::preflightLateBorrowActorAudio(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    // Still the real live pre-flush Group. Child teardown is not Group235150,
    // sample resolution, accepted-command cancellation or normal rearm.
    return !nativeUnresolvedEntryRetiring_&&main_&&backend_&&pendingWaves_&&
        sceneLive_&&groupLive_&&retirementStage_==RetirementStage::none&&
        soundGroupRetirement_==SoundGroupRetirement::live&&
        main_->unresolvedLateBorrowActorAudioCurrent(page,*this)&&
        pendingWaves_->canAbortUnresolvedSceneNative(*this);
}
bool OriginalUiGaragePodScene50KG::preflightLoadedPageActorAudio(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    // Still the real live pre-flush Group. Child teardown is not Group235150,
    // sample resolution, accepted-command cancellation or normal rearm.
    return !nativeUnresolvedEntryRetiring_&&main_&&backend_&&pendingWaves_&&
        sceneLive_&&groupLive_&&retirementStage_==RetirementStage::none&&
        soundGroupRetirement_==SoundGroupRetirement::live&&
        main_->unresolvedLoadedPageActorAudioCurrent(page,*this)&&
        pendingWaves_->canAbortUnresolvedSceneNative(*this);
}
bool OriginalUiGaragePodScene50KG::preflightWattoBindingActorAudio(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    // Still the real live pre-flush Group. Child teardown is not Group235150,
    // sample resolution, accepted-command cancellation or normal rearm.
    return !nativeUnresolvedEntryRetiring_&&main_&&backend_&&pendingWaves_&&
        sceneLive_&&groupLive_&&retirementStage_==RetirementStage::none&&
        soundGroupRetirement_==SoundGroupRetirement::live&&
        main_->unresolvedWattoBindingActorAudioCurrent(page,*this)&&
        pendingWaves_->canAbortUnresolvedSceneNative(*this);
}
bool OriginalUiGaragePodScene50KG::canRetireUnresolvedSoundGroupForEntryFailure(
    const OriginalUiGarageSession50KG& entry,bool sceneRetiring)const noexcept {
    if(nativeUnresolvedEntryRetiring_||!main_||!backend_||!pendingWaves_||!grant_||
       !sceneLive_||!groupLive_||soundGroupRetirement_!=SoundGroupRetirement::live||
       !main_->unresolvedSceneEntryFailureCurrent(entry,*this,sceneRetiring)||
       !backend_->preflightUnresolvedSceneEntryRetirement(*this)||
       !pendingWaves_->canAbortUnresolvedSceneNative(*this)||
       !grant_->preflightWattoSoundGroupRetirement(group4D8_.entity,nullptr,group4D8_.slot40_).okay())return false;
    if(sceneRetiring&&(retirementStage_!=RetirementStage::configClosed||
       !grant_->scenePodRetirementCurrent(podScene_)||podScene_.reference00||
       actorOwner3C8_||worldList500_||count50C_||capacity504_||config34_||config38_||
       (audioConfigAttempted_&&!retiredAudioConfig38_)))return false;
    return true;
}
bool OriginalUiGaragePodScene50KG::canRetireUnresolvedSoundGroupForLateBorrowFailure(
    const OriginalUiGarageSession50KG& entry,bool sceneRetiring)const noexcept {
    if(nativeUnresolvedEntryRetiring_||!main_||!backend_||!pendingWaves_||!grant_||
       !sceneLive_||!groupLive_||soundGroupRetirement_!=SoundGroupRetirement::live||
       !main_->unresolvedLateBorrowEntryFailureCurrent(entry,*this,sceneRetiring)||
       !backend_->preflightUnresolvedSceneEntryRetirement(*this)||
       !pendingWaves_->canAbortUnresolvedSceneNative(*this)||
       !grant_->preflightWattoSoundGroupRetirement(group4D8_.entity,nullptr,group4D8_.slot40_).okay())return false;
    if(sceneRetiring&&(retirementStage_!=RetirementStage::configClosed||
       !grant_->scenePodRetirementCurrent(podScene_)||podScene_.reference00||
       actorOwner3C8_||worldList500_||count50C_||capacity504_||config34_||config38_||
       (audioConfigAttempted_&&!retiredAudioConfig38_)))return false;
    return true;
}
bool OriginalUiGaragePodScene50KG::canRetireUnresolvedSoundGroupForLoadedPageFailure(
    const OriginalUiGarageSession50KG& entry,bool sceneRetiring)const noexcept {
    if(nativeUnresolvedEntryRetiring_||!main_||!backend_||!pendingWaves_||!grant_||
       !sceneLive_||!groupLive_||soundGroupRetirement_!=SoundGroupRetirement::live||
       !main_->unresolvedLoadedPageEntryFailureCurrent(entry,*this,sceneRetiring)||
       !backend_->preflightUnresolvedSceneEntryRetirement(*this)||
       !pendingWaves_->canAbortUnresolvedSceneNative(*this)||
       !grant_->preflightWattoSoundGroupRetirement(group4D8_.entity,nullptr,group4D8_.slot40_).okay())return false;
    if(sceneRetiring&&(retirementStage_!=RetirementStage::configClosed||
       !grant_->scenePodRetirementCurrent(podScene_)||podScene_.reference00||
       actorOwner3C8_||worldList500_||count50C_||capacity504_||config34_||config38_||
       (audioConfigAttempted_&&!retiredAudioConfig38_)))return false;
    return true;
}
bool OriginalUiGaragePodScene50KG::canRetireUnresolvedSoundGroupForWattoBindingFailure(
    const OriginalUiGarageSession50KG& entry,bool sceneRetiring)const noexcept {
    if(nativeUnresolvedEntryRetiring_||!main_||!backend_||!pendingWaves_||!grant_||
       !sceneLive_||!groupLive_||soundGroupRetirement_!=SoundGroupRetirement::live||
       !main_->unresolvedWattoBindingEntryFailureCurrent(entry,*this,sceneRetiring)||
       !backend_->preflightUnresolvedSceneEntryRetirement(*this)||
       !pendingWaves_->canAbortUnresolvedSceneNative(*this)||
       !grant_->preflightWattoSoundGroupRetirement(group4D8_.entity,nullptr,group4D8_.slot40_).okay())return false;
    if(sceneRetiring&&(retirementStage_!=RetirementStage::configClosed||
       !grant_->scenePodRetirementCurrent(podScene_)||podScene_.reference00||
       actorOwner3C8_||worldList500_||count50C_||capacity504_||config34_||config38_||
       (audioConfigAttempted_&&!retiredAudioConfig38_)))return false;
    return true;
}
bool OriginalUiGaragePodScene50KG::retireUnresolvedSoundGroupForEntryFailure(
    const OriginalUiGarageSession50KG& entry,std::string& error)noexcept {
    return retireUnresolvedSoundGroupForFailure(entry,UnresolvedEntryFailure::partialAttachment,error);
}
bool OriginalUiGaragePodScene50KG::retireUnresolvedSoundGroupForLateBorrowFailure(
    const OriginalUiGarageSession50KG& entry,std::string& error)noexcept {
    return retireUnresolvedSoundGroupForFailure(entry,UnresolvedEntryFailure::lateBorrow,error);
}
bool OriginalUiGaragePodScene50KG::retireUnresolvedSoundGroupForLoadedPageFailure(
    const OriginalUiGarageSession50KG& entry,std::string& error)noexcept {
    return retireUnresolvedSoundGroupForFailure(entry,UnresolvedEntryFailure::loadedPageHide,error);
}
bool OriginalUiGaragePodScene50KG::retireUnresolvedSoundGroupForWattoBindingFailure(
    const OriginalUiGarageSession50KG& entry,std::string& error)noexcept {
    return retireUnresolvedSoundGroupForFailure(entry,UnresolvedEntryFailure::wattoBinding,error);
}
bool OriginalUiGaragePodScene50KG::retireUnresolvedSoundGroupForFailure(
    const OriginalUiGarageSession50KG& entry,UnresolvedEntryFailure kind,std::string& error)noexcept {
    // Only these private wrappers select an exact source failure family.
    // No family admits another one's Session or Page retirement receipt.
    const auto canRetire=[&]()noexcept {
        switch(kind){
        case UnresolvedEntryFailure::partialAttachment:
            return canRetireUnresolvedSoundGroupForEntryFailure(entry,true);
        case UnresolvedEntryFailure::lateBorrow:
            return canRetireUnresolvedSoundGroupForLateBorrowFailure(entry,true);
        case UnresolvedEntryFailure::loadedPageHide:
            return canRetireUnresolvedSoundGroupForLoadedPageFailure(entry,true);
        case UnresolvedEntryFailure::wattoBinding:
            return canRetireUnresolvedSoundGroupForWattoBindingFailure(entry,true);
        }
        return false;
    };
    if(!canRetire())
        return sceneFailure(error,"Native unresolved Scene abort lacks exact failed-entry/configClosed ownership");
    auto& g=group4D8_;
    std::unique_ptr<Sound*[]> snapshot;
    try{snapshot=std::make_unique<Sound*[]>(g.count48_);}catch(...){
        return sceneFailure(error,"Native unresolved Scene abort snapshot allocation failed before effects");}
    for(std::size_t i=0;i<g.count48_;++i)snapshot[i]=g.members4C_[i];
    // Command25 was accepted by real1FB594. Apply it through its existing
    // ordered native consumer, never discard it or pretend a sample flush ran.
    if(!main_->serviceColdGarageRouting(error)||!main_->nativeSceneWetLifetimeCurrent(*this))return false;
    if(!canRetire())
        return sceneFailure(error,"Native unresolved Scene abort identity changed after actual command25 application");
    if(!grant_->beginWattoSoundGroupRetirement(g.entity,nullptr,g.slot40_).okay())
        return sceneFailure(error,"235150 registry begin refused before unresolved Sound release");
    soundGroupRetirement_=SoundGroupRetirement::registryReleased;
    nativeUnresolvedEntryRetiring_=true;
    // Native terminal cancellation of dangling representative borrows, NOT
    // guest26D960/26D9F0. Pending retains positive registration history.
    pendingWaves_->abortUnresolvedSceneNativePreflighted(*this);
    for(std::size_t i=0;i<g.count48_;++i){auto* sound=snapshot[i];
        if(sound!=g.sounds_[i].get()||sound->reference00!=1||sound->channel2C!=-1||
           sound->flags10!=1||sound->nativeSound||sound->nativeSample28||
           !sound->sampleContinuationPending)std::terminate();
        //233670 is genuinely a no-op for channel−1;104970→233070(flags1)
        //does not unregister row+4, reset count8, unload samples or emit RPC.
        --sound->reference00;g.sounds_[i].reset();g.members4C_[i]=nullptr;
    }
    snapshot.reset();g.count48_=0;g.members4C_.reset();g.capacity54_=0;
    if(!grant_->finishWattoSoundGroupRetirement(g.entity).okay())std::terminate();
    groupLive_=false;
    if(!pendingWaves_->retireSceneBorrow(*this))std::terminate();
    pendingWaves_.reset();soundGroupRetirement_=SoundGroupRetirement::complete;
    retirementStage_=RetirementStage::groupReleased;
    if(!backend_->finishUnresolvedSceneEntryRetirement(*this))std::terminate();
    // Main's applied command and strong Page alias still belong to the later
    // actual Scene/Page/manager retirement. Neither is erased here.
    error.clear();return true;
}
bool OriginalUiGaragePodScene50KG::retireSoundGroup26FDD4(std::string& error)noexcept {
    // Only the real reached base Scene26FD80 body calls this after its
    // slot9C->271C10 Config close. Native partial-abort cleanup is not entry.
    if(!sceneLive_||!groupLive_||!grant_||!grant_->scenePodRetirementCurrent(podScene_)||
       retirementStage_!=RetirementStage::configClosed||
       soundGroupRetirement_!=SoundGroupRetirement::live||actorOwner3C8_||
       worldList500_||count50C_||capacity504_||config34_||config38_||
       (audioConfigAttempted_&&!retiredAudioConfig38_)||
       !preflightRoutingRetirement235150()){
        error="26FDD4 requires real PodScene last-ref, destroyed callback/world-list borrows, Config close and resolved cold audio";
        return false;
    }
    auto& g=group4D8_;
    if(!grant_->preflightWattoSoundGroupRetirement(g.entity,nullptr,g.slot40_).okay()){
        error="26FDD4 shared exact Scene Group retirement preflight refused";return false;
    }
    //2353D0 uses an insertion-order pointer snapshot, count4/growth16. The
    //native preallocation is intentionally before irreversible registry
    //mutation, not a claim of the guest's allocation-failure timing.
    if(g.count48_!=4||g.capacity54_!=8||g.growth50_!=16||!g.members4C_){
        error="2353D0 exact Scene Sound list shape changed";return false;
    }
    std::unique_ptr<Sound*[]> snapshot;
    try{snapshot=std::make_unique<Sound*[]>(g.count48_);}catch(...){
        error="2353D0 Scene Sound snapshot allocation failed before source effects";return false;
    }
    for(std::size_t i=0;i<g.count48_;++i){
        if(!sourceRowIdentityCurrent(*backend_,i,true)||g.members4C_[i]!=g.sounds_[i].get()){
            error="2353D0 actual Scene Sound snapshot identity changed";return false;
        }
        snapshot[i]=g.members4C_[i];
    }
    if(!grant_->beginWattoSoundGroupRetirement(g.entity,nullptr,g.slot40_).okay()){
        error="235150 shared registry begin refused before Sound release";return false;
    }
    soundGroupRetirement_=SoundGroupRetirement::registryReleased;
    // Registry slot is already clear at23518C. Stop233670 then release104970
    // each actual source Sound in order. These references are NOT Entities.
    for(std::size_t i=0;i<g.count48_;++i){auto* source=snapshot[i];
        if(source!=g.sounds_[i].get()||source->reference00!=1||
           !backend_->stopSourceSceneRow233670(*this,i))std::terminate();
        --source->reference00; //104970 ->233070(flags10=1), no extra stop.
        //Backend still pins its exact sample until all logical borrows close.
        g.sounds_[i].reset();g.members4C_[i]=nullptr;
    }
    snapshot.reset(); //235490: temporary list before original count clear.
    g.count48_=0;      //235498 ->235C40.
    g.members4C_.reset();g.capacity54_=0; //2351AC after all Sounds released.
    if(!grant_->finishWattoSoundGroupRetirement(g.entity).okay())std::terminate();
    groupLive_=false; //2351B8->102670->1413E0 real Group count release.
    if(!pendingWaves_->retireSceneBorrow(*this))std::terminate();
    pendingWaves_.reset();
    soundGroupRetirement_=SoundGroupRetirement::complete;
    retirementStage_=RetirementStage::groupReleased; //26FDDC source4D8 gone.
    if(!backend_->finishSourceSceneRetirement235150(*this))std::terminate();
    // DO NOT release Main command/Page alias here. Scene26FDE0 onward and
    // Page1FA170's later payload tails must genuinely complete first.
    error.clear();return true;
}

// Include original_ui_garage_page_owner50kg.h in PodScene.cpp.
// This begins ONLY the actual Page1FA1B4 call. It does not execute or skip
// PodScene1B2920's interpolation-array prelude, world-list or Scene payloads.
bool OriginalUiGaragePodScene50KG::beginRetirementAt1FA1B4(
    const OriginalUiGaragePageOwner50KG& page,std::string& error)noexcept {
    return beginRetirementBody1FA1B4(page,nullptr,PageRetirementOrigin::normal,error);
}
bool OriginalUiGaragePodScene50KG::beginFailedPartialRetirement1FA1B4(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageSession50KG& entry,
    std::string& error)noexcept {
    if(!page.partialSceneAttachmentFailureCurrent()||
       page.failedSceneRetirement_!=OriginalUiGaragePageOwner50KG::FailedSceneRetirement::payloadRetiring||
       page.failedAttachedRetiredCount_!=6||!visibility_||
       !visibility_->failedPageSearchEntitiesRetired142590(page)){
        error="Failed Scene entry requires actual count6 children, Router and Page last reference";return false;
    }
    return beginRetirementBody1FA1B4(page,&entry,PageRetirementOrigin::partialScene,error);
}
bool OriginalUiGaragePodScene50KG::beginFailedLateBorrowRetirement1FA1B4(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageSession50KG& entry,
    std::string& error)noexcept {
    using Page=OriginalUiGaragePageOwner50KG;
    if(!page.lateBorrowFailureOwnerCurrent()||
       page.failedLateBorrowRetirement_!=Page::FailedLateBorrowRetirement::payloadRetiring||
       page.sceneInitialRetiredCount_!=10||page.cameraRetirementStep_!=6||
       !page.grant_->garageLateBorrowAudioListsRetired(page,*this)){
        error="1FA1B4 requires the exact late-borrow Page/Camera/child/list return";return false;
    }
    return beginRetirementBody1FA1B4(page,&entry,PageRetirementOrigin::lateBorrow,error);
}
bool OriginalUiGaragePodScene50KG::beginFailedWattoBindingRetirement1FA1B4(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageSession50KG& entry,
    std::string& error)noexcept {
    using Page=OriginalUiGaragePageOwner50KG;
    if(!page.wattoBindingFailureOwnerCurrent()||
       page.failedWattoBindingRetirement_!=Page::FailedWattoBindingRetirement::payloadRetiring||
       page.sceneLateRetiredCount_!=3||page.sceneInitialRetiredCount_!=10||page.cameraRetirementStep_!=6||
       !page.managerAttached1D1C40_||!page.entityRefReleased_||!page.grant_->garagePageRetirementCurrent(page)){
        error="1FA1B4 requires the exact failed pre-bind Page/late/Camera/initial child return";return false;
    }
    return beginRetirementBody1FA1B4(page,&entry,PageRetirementOrigin::wattoBinding,error);
}
bool OriginalUiGaragePodScene50KG::beginFailedLoadedHideRetirement1FA1B4(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageSession50KG& entry,
    std::string& error)noexcept {
    using Page=OriginalUiGaragePageOwner50KG;
    if(!page.loadedHideFailureOwnerCurrent()||
       page.failedLoadedHideRetirement_!=Page::FailedLoadedHideRetirement::payloadRetiring||
       page.sceneLateRetiredCount_!=3||page.sceneInitialRetiredCount_!=10||page.cameraRetirementStep_!=6||
       !page.managerAttached1D1C40_||!page.entityRefReleased_||!page.grant_->garagePageRetirementCurrent(page)){
        error="1FA1B4 requires the exact loaded-hide Page/late/Camera/initial child return";return false;
    }
    return beginRetirementBody1FA1B4(page,&entry,PageRetirementOrigin::loadedHide,error);
}
bool OriginalUiGaragePodScene50KG::beginRetirementBody1FA1B4(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageSession50KG* failedEntry,
    PageRetirementOrigin origin,std::string& error)noexcept {
    using Page=OriginalUiGaragePageOwner50KG;
    const auto audioCurrent=[&]()noexcept {
        switch(origin){
        case PageRetirementOrigin::normal:return !failedEntry&&preflightRoutingRetirement235150();
        case PageRetirementOrigin::partialScene:return failedEntry&&canRetireUnresolvedSoundGroupForEntryFailure(*failedEntry,false);
        case PageRetirementOrigin::lateBorrow:return failedEntry&&canRetireUnresolvedSoundGroupForLateBorrowFailure(*failedEntry,false);
        case PageRetirementOrigin::loadedHide:return failedEntry&&canRetireUnresolvedSoundGroupForLoadedPageFailure(*failedEntry,false);
        case PageRetirementOrigin::wattoBinding:return failedEntry&&canRetireUnresolvedSoundGroupForWattoBindingFailure(*failedEntry,false);
        }
        return false;
    };
    if(!constructed_||!sceneLive_||!groupLive_||sourceConstructionFailed_||
       retirementStage_!=RetirementStage::none||actorOwner3C8_||
       page.sceneBacking_.get()!=this||page.scene4C0_!=this||page.sceneReferenceReleased_||
       page.stage_!=Page::Stage::pageRefReleased||page.pendingPC_!=0x1FA1B4||
       !page.entityRefReleased_||page.entityCountReleased_||page.entity_.reference00||page.entity_.firstChild18||
       !grant_||page.grant_!=grant_||page.grant_.owner_before(grant_)||grant_.owner_before(page.grant_)||
       failedPageRetirementOwner_||failedLateBorrowRetirementOwner_||failedLoadedHideRetirementOwner_||
       failedWattoBindingRetirementOwner_||!audioCurrent()){
        error="1FA1B4 requires actual Page last-ref/callback teardown and retained cold Scene";return false;
    }
    const auto preflight=grant_->preflightScenePodRetirement142590(*this,podScene_,nullptr);
    if(!preflight.okay()){
        error="1FA1B4 exact typed PodScene reference preflight refused";return false;
    }
    const auto begin=grant_->beginScenePodRetirement142590(*this,podScene_,nullptr);
    if(!begin.okay()){
        error="1FA1B4 exact typed PodScene last-reference refused";return false;
    }
    switch(origin){
    case PageRetirementOrigin::normal:break;
    case PageRetirementOrigin::partialScene:failedPageRetirementOwner_=&page;break;
    case PageRetirementOrigin::lateBorrow:failedLateBorrowRetirementOwner_=&page;break;
    case PageRetirementOrigin::loadedHide:failedLoadedHideRetirementOwner_=&page;break;
    case PageRetirementOrigin::wattoBinding:failedWattoBindingRetirementOwner_=&page;break;
    }
    retirementStage_=RetirementStage::lastReference;
    error.clear();return true;
}

bool OriginalUiGaragePodScene50KG::retirePodScenePrefix1B2920(D& d)noexcept {
    d={};using S=assets::OriginalStandardMaterialStatus50LA;
    const auto stop=[&](std::uint32_t pc){d.status=S::phase;d.sourcePC=pc;return false;};
    //Entered only after actual Page1FA1B4 last-reference dispatch, not a
    //detached component abort. The real Toy and Actor load phases establish
    //the bounded empty arrays; immutable authenticated assets do not.
    if(!sceneLive_||!grant_||!grant_->scenePodRetirementCurrent(podScene_)||
       sourceConstructionFailed_||actorOwner3C8_||!actorOverrideScan1B40B0_||
       configFailure_||loadStage_!=LoadStage::audioClosed||!toyFields_||
       toyFields_->closed_||!toyFields_->complete_)return stop(0x1B2920);
    if(retirementStage_==RetirementStage::worldListReleased)return true;
    if(retirementStage_==RetirementStage::lastReference){
        //1B2954: actual510 null skips the nested interpolation destructors.
        if(interpolation510_||count50C_)return stop(0x1B2954);
        retirementStage_=RetirementStage::interpolation510Released;
    }
    if(retirementStage_==RetirementStage::interpolation510Released){
        //1B29C0: actual514 null bypasses its array-header delete.
        if(interpolation514_)return stop(0x1B29C0);
        retirementStage_=RetirementStage::interpolation514Released;
    }
    if(retirementStage_==RetirementStage::interpolation514Released){
        //The sole native53C pointer belongs to the reached TOY field owner.
        //The supported live read was zero; nonzero row lifecycle is not
        //silently inferred from a null pointer or freed as an empty scene.
        if(toyFields_->buoyCount538_!=0||toyFields_->buoys53C_)return stop(0x1B29D4);
        toyFields_->buoys53C_.reset(); //Source branch is a genuine null no-op.
        retirementStage_=RetirementStage::buoysReleased;
    }
    if(retirementStage_==RetirementStage::buoysReleased){
        if(num321Anims528_!=0||animationRows52C_||animationRows530_)return stop(0x1B29E8);
        //1B29EC skips both inner rows and both table releases for count0.
        retirementStage_=RetirementStage::animationArraysReleased;
    }
    if(retirementStage_!=RetirementStage::animationArraysReleased)return stop(0x1B2A4C);
    if(!worldList500_||capacity504_!=10||growth508_!=100000||count50C_)return stop(0x1B5F90);
    //This is the actual reached allocation destructor1B5F90. Empty list
    //content never substitutes for deleting its real constructor allocation.
    worldList500_.reset();capacity504_=0; //Native allocation lifetime, not a guest504 store.
    retirementStage_=RetirementStage::worldListReleased;
    return true; //Next call is base Scene26FD80, not complete Scene retirement.
}
bool OriginalUiGaragePodScene50KG::retireConfig34At26FDB8(D& d)noexcept {
    d={};using S=assets::OriginalStandardMaterialStatus50LA;
    if(!sceneLive_||!grant_||!grant_->scenePodRetirementCurrent(podScene_)||
       retirementStage_!=RetirementStage::worldListReleased||worldList500_||count50C_||capacity504_||
       config38_||(audioConfigAttempted_&&!retiredAudioConfig38_)){
        d.status=S::phase;d.sourcePC=0x26FDB8;return false;
    }
    //The reached virtual9C calls271C10 even when34 was already closed by
    //the loader. It does not replay272930 on the retired Audio Config38.
    if(config34_){config34_->close1573B0();config34_.reset();}
    retirementStage_=RetirementStage::configClosed;return true;
}
bool OriginalUiGaragePodScene50KG::populateCollisionModels2743B0(D& d)noexcept {
    d={};using S=assets::OriginalStandardMaterialStatus50LA;
    if(!currentPrefix()||populationStage_!=PopulationStage::statics||sceneCollision_){
        d.status=S::phase;d.sourcePC=0x2743B0;return false;
    }
    try{sceneCollision_.reset(new OriginalUiGarageSceneCollision50KG());}
    catch(...){d.status=S::allocation;d.sourcePC=0x2743B0;return false;}
    if(!sceneCollision_->populate2743B0(*this,d))return false;
    populationStage_=PopulationStage::collisions;return true;
}
bool OriginalUiGaragePodScene50KG::closeCollisionPrefix2743B0(D& d)noexcept {
    d={};return !sceneCollision_||sceneCollision_->closePartial2743B0(d);
}
bool OriginalUiGaragePodScene50KG::finishActorPopulation2742F0(D& d)noexcept {
    d={};if(!currentPrefix()||populationStage_!=PopulationStage::collisions||!sceneCollision_||
       !sceneCollision_->current2743B0(*this)||!actorOwner3C8_||
       !actorOwner3C8_->scenePopulationCurrent2742F0(*this)){
        d.status=assets::OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x2742F0;return false;
    }
    if(!actorOwner3C8_->finishPodSceneActorScan1B40B0(*this,d))return false;
    populationStage_=PopulationStage::actors;return true;
}
}
