// NEW unverified source reconstruction: read-only Confirm preflight.
#include "original_ui_rider_dynamic_owner50kg.h"
#include <cmath>
#include <string>
#include <utility>
namespace rr::game {
namespace {
using K=OriginalUiRiderWidgetKind50KG;using O=OriginalUiControlOperation50KG;
using E=OriginalUiRiderDynamicError50KG;
bool idOk(std::uint16_t id) noexcept{return id>0&&id<120;}
bool dynamic(K k) noexcept{return k==K::pod||k==K::character;}
bool sameState(const OriginalUiRiderWidgetState50KG& a,const OriginalUiRiderWidgetState50KG& b) noexcept {
    return a.kind==b.kind&&a.selected==b.selected&&a.shown==b.shown&&a.onTop==b.onTop;
}
}
struct OriginalUiRiderDynamicOwner50KG::Storage {
    struct Entry {
        OriginalUiRiderWidgetState50KG state;
        bool enableSelect=false,disableSelect=false,enableDeselect=false,disableDeselect=false;
        bool everLoaded=false,registered=false,detachComplete=false;
        std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG> characterAssets;
        std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> podAssets;
        OriginalUiRiderCharacterScene50KG character;OriginalUiRiderPodScene50KG pod;
    };
    std::array<Entry,120> entries;
    // Exact bind origin, comparison only. Existing source-widget lifetime is
    // required; projection never dereferences this possibly stale identity.
    const OriginalUiRiderWidgets50KG* widgets=nullptr;
    std::array<std::uint16_t,32> pods{},characters{};
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    ModelMotionSharedState49T* motion=nullptr;ModelMode0IkShared50KG* ik=nullptr;
    SourceLibmState49T* libm=nullptr;CoupledNoiseContext49M* rng=nullptr;
    OriginalUiRiderDynamicBackend50KG backend;
    std::uint32_t requestedPreviewRiders=0;
    bool coldPrepared=false,poisoned=false,inCallback=false,drained=false;
};
OriginalUiRiderDynamicOwner50KG::OriginalUiRiderDynamicOwner50KG() noexcept=default;
OriginalUiRiderDynamicOwner50KG::~OriginalUiRiderDynamicOwner50KG(){
    if(!storage_)return;
    // The session owns explicit fallible cleanup and keeps the backend alive.
    // Never invoke detach for the first time from a destructor. On misuse,
    // preserve every graph/token together instead of silently destroying them.
    for(const auto& e:storage_->entries)if(e.registered||e.character.requiresModelCleanup()||e.pod.requiresModelCleanup()){
        (void)storage_.release();return;
    }
}
bool OriginalUiRiderDynamicOwner50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const OriginalUiRiderWidgets50KG& widgets,ModelMotionSharedState49T& motion,ModelMode0IkShared50KG& ik,
    SourceLibmState49T& libm,CoupledNoiseContext49M& rng,const OriginalUiRiderDynamicBackend50KG& backend,
    OriginalUiRiderDynamicDiagnostic50KG& d) noexcept {
    d={};const auto fail=[&](E e){d.error=e;diagnostic_=d;return false;};
    if(storage_)return fail(E::alreadyBound);
    if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::rider)||
        !widgets.selectionData()||widgets.poisoned()||!backend.preflight||!backend.attach||
        !backend.drainAndDetach||!backend.setShown||!backend.materialLifetime||!backend.graphFootprint)return fail(E::input);
    try {
        assets::OriginalUiDat50KG dat;
        if(!assets::readOriginalUiDat50KG(*archive,"UI/UIRider/Widgets.Dat",dat,d.parse,
            assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(E::parse);
        auto next=std::make_unique<Storage>();next->archive=std::move(archive);
        next->motion=&motion;next->ik=&ik;next->libm=&libm;next->rng=&rng;next->backend=backend;
        for(std::uint16_t id=1;id<120;++id){
            auto& e=next->entries[id];e.state=widgets.state(id);if(!dynamic(e.state.kind))continue;
            d.widget=id;const auto* section=dat.section("Widget "+std::to_string(id));
            if(!section||!assets::originalUiBoolean50KG(*section,"EnableOnSelect",false,e.enableSelect,d.parse,0x2A7848)||
                !assets::originalUiBoolean50KG(*section,"DisableOnSelect",false,e.disableSelect,d.parse,0x2A7860)||
                !assets::originalUiBoolean50KG(*section,"EnableOnDeSelect",false,e.enableDeselect,d.parse,0x2A7878)||
                !assets::originalUiBoolean50KG(*section,"DisableOnDeSelect",false,e.disableDeselect,d.parse,0x2A7890))return fail(E::parse);
            // Dynamic model graph state starts uncreated. Bind must happen at
            // actual manager cold construction, before selection publication.
            if(e.state.selected||e.state.shown)return fail(E::input);
        }
        // Original Group1 order and PodIndex identities from authenticated rider.dat.
        // Bind metadata for all23 controls, including hidden traversal intermediates.
        // These maps confer no descriptor ownership or preview readiness.
        constexpr std::uint16_t controls[]{16,17,10,8,2,4,9,12,3,5,7,19,20,13,6,14,21,15,1,11,22,18,23};
        constexpr std::uint16_t riders[]{19,20,12,10,2,4,11,15,3,7,9,22,23,16,8,17,24,18,1,14,25,21,26};
        constexpr std::size_t count=sizeof(controls)/sizeof(controls[0]);
        const auto* selection=widgets.selectionData();const auto* page=selection->page();
        if(!page||page->groups.size()<=1||page->groups[1].controls.size()!=count)return fail(E::input);
        std::array<bool,120> usedWidgets{};
        for(std::size_t slot=0;slot<count;++slot){
            const auto control=page->groups[1].controls[slot];
            if(control!=controls[slot]||static_cast<std::size_t>(control)>=page->controls.size())return fail(E::input);
            const auto rider=selection->controlRider(static_cast<std::uint16_t>(control));
            if(rider!=riders[slot]||next->pods[rider]||next->characters[rider])return fail(E::input);
            const auto& ids=page->controls[control].widgets;
            if(ids.size()!=4)return fail(E::input);
            constexpr K kinds[]{K::pod,K::text,K::text,K::character};
            for(std::size_t i=0;i<4;++i){
                if(!idOk(ids[i])||usedWidgets[ids[i]]||next->entries[ids[i]].state.kind!=kinds[i])return fail(E::input);
                usedWidgets[ids[i]]=true;
            }
            // Widget references come from the source control, never rider arithmetic.
            next->pods[rider]=ids[0];next->characters[rider]=ids[3];
        }
        next->widgets=&widgets;storage_=std::move(next);diagnostic_={};d={};return true;
    }catch(...){return fail(E::allocation);}
}
OriginalUiRiderDynamicHooks50KG OriginalUiRiderDynamicOwner50KG::hooks() noexcept {
    return {this,[](void* p,const OriginalUiRiderWidgetCallback50KG* c,std::size_t n) noexcept {
        return static_cast<OriginalUiRiderDynamicOwner50KG*>(p)->preflight(c,n);
    },[](void* p,const OriginalUiRiderWidgetCallback50KG& c,OriginalUiRiderWidgetState50KG& s) noexcept {
        return static_cast<OriginalUiRiderDynamicOwner50KG*>(p)->apply(c,s);
    }};
}
const OriginalUiRiderDynamicDiagnostic50KG& OriginalUiRiderDynamicOwner50KG::diagnostic()const noexcept{return diagnostic_;}
bool OriginalUiRiderDynamicOwner50KG::poisoned()const noexcept{return storage_&&storage_->poisoned;}
std::uint16_t OriginalUiRiderDynamicOwner50KG::podWidget(std::uint32_t rider)const noexcept {
    return storage_&&rider<storage_->pods.size()?storage_->pods[rider]:0;
}
std::uint16_t OriginalUiRiderDynamicOwner50KG::characterWidget(std::uint32_t rider)const noexcept {
    return storage_&&rider<storage_->characters.size()?storage_->characters[rider]:0;
}
OriginalUiRiderWidgetState50KG OriginalUiRiderDynamicOwner50KG::state(std::uint16_t id)const noexcept {
    return storage_&&idOk(id)?storage_->entries[id].state:OriginalUiRiderWidgetState50KG{};
}
bool OriginalUiRiderDynamicOwner50KG::nativeProjectionHealthy()const noexcept {
    if(!storage_||!storage_->coldPrepared||storage_->poisoned||storage_->drained||storage_->inCallback||
       diagnostic_.cleanupPending||diagnostic_.cleanupPhase!=OriginalUiRiderDynamicCleanupPhase50KG::none||
       diagnostic_.model.cleanupPending||diagnostic_.cleanupModel.cleanupPending)return false;
    // Read the actual model/registration entries. Cleanup tokens on a healthy
    // registered graph are normal; unregistered pending or detached-but-still-
    // registered graphs are partial cleanup and cannot issue a projection.
    for(const auto& e:storage_->entries){
        const bool characterPending=e.character.requiresModelCleanup();
        const bool podPending=e.pod.requiresModelCleanup();
        const bool characterAttached=e.character.attached(),podLoaded=e.pod.loaded();
        if(e.state.kind==K::character){
            if(podPending||podLoaded||e.podAssets)return false;
            if(e.characterAssets&&!e.characterAssets->loaded())return false;
            if(e.registered){
                if(!e.everLoaded||e.detachComplete||!e.characterAssets||!characterPending||!characterAttached)return false;
            }else if(characterPending||characterAttached)return false;
        }else if(e.state.kind==K::pod){
            if(characterPending||characterAttached||e.characterAssets)return false;
            if(e.podAssets&&(!e.podAssets->loaded()||!e.podAssets->config()))return false;
            if(e.registered){
                if(!e.everLoaded||e.detachComplete||!e.podAssets||!podPending||!podLoaded)return false;
            }else if(podPending||podLoaded)return false;
        }else if(e.registered||characterPending||podPending||characterAttached||podLoaded||
                 e.characterAssets||e.podAssets)return false;
    }
    return true;
}
bool OriginalUiRiderDynamicOwner50KG::nativeProjectionForWidgets(
    const OriginalUiRiderWidgets50KG& widgets)const noexcept {
    return storage_&&storage_->widgets==&widgets&&nativeProjectionHealthy();
}
bool OriginalUiRiderDynamicOwner50KG::nativeProjectionSnapshot(std::uint16_t id,
    OriginalUiRiderWidgetState50KG& output,bool& registered)const noexcept {
    if(!nativeProjectionHealthy()||!idOk(id))return false;
    const auto& e=storage_->entries[id];if(!dynamic(e.state.kind))return false;
    output=e.state;registered=e.registered;return true;
}
bool OriginalUiRiderDynamicOwner50KG::projectNativeDescriptorOne(
    const OriginalUiRiderWidgetCallback50KG& c,OriginalUiRiderWidgetState50KG& state,
    bool& registered)const noexcept {
    if(!storage_||!idOk(c.widget))return false;
    const auto& e=storage_->entries[c.widget];
    if(!dynamic(e.state.kind)||state.kind!=e.state.kind||state.onTop!=e.state.onTop)return false;
    auto next=state;bool nextRegistered=registered;
    const auto load=[&]() noexcept {
        // Immutable actual descriptors only; no graph, ledger or RNG operation.
        if(e.state.kind==K::character){if(!e.characterAssets||!e.characterAssets->loaded())return false;}
        else if(!e.podAssets||!e.podAssets->loaded()||!e.podAssets->config())return false;
        nextRegistered=true;return true;
    };
    const auto genericState=[&](bool selected) noexcept {
        next.selected=selected;
        const bool enable=selected?e.enableSelect:e.enableDeselect;
        const bool disable=selected?e.disableSelect:e.disableDeselect;
        // As in generic(): both set explicitly suppress both effects.
        if(enable!=disable)next.shown=enable;
    };
    switch(c.operation){
    case O::select:
        if(e.state.kind==K::character){if(!load())return false;genericState(true);}
        else{genericState(true);if(!load())return false;genericState(true);next.shown=true;}
        break;
    case O::deselect:
        if(e.state.kind==K::character){nextRegistered=false;genericState(false);}
        else{
            genericState(false);
            if(e.podAssets){
                const auto* config=e.podAssets->config();if(!config)return false;
                if(config->cleanFromMemory)nextRegistered=false;
            }
            genericState(false);next.shown=false;
        }
        break;
    case O::clearSelection:
        // Character+88 loads but does NOT imply shown. Pod+88 only clears select.
        if(e.state.kind==K::character&&!load())return false;
        next.selected=false;break;
    case O::setEnabled:next.selected=c.value;next.shown=c.value;break;
    case O::show:case O::reset:next.shown=true;break;
    case O::hide:next.shown=false;break;
    default:return false;
    }
    state=next;registered=nextRegistered;return true;
}
bool OriginalUiRiderDynamicOwner50KG::projectNativeOne(
    const OriginalUiRiderWidgetCallback50KG& c,OriginalUiRiderWidgetState50KG& state,
    bool& registered)const noexcept {
    return nativeProjectionHealthy()&&projectNativeDescriptorOne(c,state,registered);
}
bool OriginalUiRiderDynamicOwner50KG::matchesNativeProjection(std::uint16_t id,
    const OriginalUiRiderWidgetState50KG& expected,bool registered)const noexcept {
    if(!nativeProjectionHealthy()||!idOk(id))return false;
    const auto& e=storage_->entries[id];
    return dynamic(e.state.kind)&&sameState(e.state,expected)&&e.registered==registered;
}
bool OriginalUiRiderDynamicOwner50KG::previewRequested(std::uint32_t rider)const noexcept {
    return storage_&&storage_->coldPrepared&&originalUiRiderRequested50KG(storage_->requestedPreviewRiders,rider);
}
bool OriginalUiRiderDynamicOwner50KG::prepareCold(OriginalUiRiderColdPopulation50KG population,
    OriginalUiRiderDynamicDiagnostic50KG& d,OriginalUiRiderPreviewSet50KG previewSet,
    std::uint32_t requestedPreviewRiders) noexcept {
    d={};const auto fail=[&](E e){d.error=e;diagnostic_=d;return false;};
    std::uint32_t requested=0;
    if(!originalUiRiderPreviewRequest50KG(previewSet,requestedPreviewRiders,requested))return fail(E::input);
    if(!storage_||storage_->coldPrepared||storage_->poisoned||storage_->drained||storage_->inCallback||
       (population!=OriginalUiRiderColdPopulation50KG::selected19&&
        population!=OriginalUiRiderColdPopulation50KG::rider19And20&&
        population!=OriginalUiRiderColdPopulation50KG::defaultUnlocked&&
        population!=OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasgano&&
        population!=OriginalUiRiderColdPopulation50KG::defaultUnlockedAndCleggPreview&&
        population!=OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasganoAndCleggPreview))return fail(E::input);
    if(previewSet==OriginalUiRiderPreviewSet50KG::gasgano7&&
       population!=OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasgano&&
       population!=OriginalUiRiderColdPopulation50KG::defaultUnlockedAndGasganoAndCleggPreview)return fail(E::input);
    auto& s=*storage_;
    for(const auto& e:s.entries)if(e.everLoaded||e.registered||e.characterAssets||e.podAssets||
       (dynamic(e.state.kind)&&(e.state.selected||e.state.shown)))return fail(E::input);
    try {
        std::array<std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG>,120> characters{};
        std::array<std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG>,120> pods{};
        auto podIds=s.pods,characterIds=s.characters;
        for(const auto rider:originalUiRiderRoster50KG){
            if(!originalUiRiderColdMember50KG(population,requested,rider))continue;
            const auto pod=podIds[rider],character=characterIds[rider];
            if(!pod||!character||pods[pod]||characters[character])return fail(E::input);
            d.widget=pod;
            auto p=std::make_shared<assets::OriginalUiRiderPodAssets50KG>();
            if(!p->load(s.archive,pod,d.pod))return fail(E::cold);
            if(p->widgetId()!=pod||p->widgetSource()!=s.archive->find("UI/UIRider/Widgets.Dat").identity||
               !p->modelSource().identity)return fail(E::cold);
            d.widget=character;
            auto c=std::make_shared<assets::OriginalUiRiderCharacterAssets50KG>();
            if(!c->load(s.archive,character,d.character))return fail(E::cold);
            if(c->widgetId()!=character||!c->baseName()||
               c->widgetSource()!=s.archive->find("UI/UIRider/Widgets.Dat").identity||
               c->mcfSource()!=s.archive->find("UI/Anim/"+*c->baseName()+".mcf").identity)return fail(E::cold);
            pods[pod]=std::move(p);characters[character]=std::move(c);
        }
        // These are immutable files/geometry/motions only. No graph, entry
        // sample, attachment, IK, visibility, binder clock or source row exists.
        for(std::uint16_t id=1;id<120;++id){s.entries[id].characterAssets=std::move(characters[id]);
            s.entries[id].podAssets=std::move(pods[id]);}
        s.pods=podIds;s.characters=characterIds;
        s.requestedPreviewRiders=requested;s.coldPrepared=true;diagnostic_={};d={};return true;
    }catch(...){return fail(E::allocation);}
}
bool OriginalUiRiderDynamicOwner50KG::preflight(const OriginalUiRiderWidgetCallback50KG* calls,std::size_t count) noexcept {
    diagnostic_={};if(!storage_||!storage_->coldPrepared||storage_->poisoned||storage_->drained||storage_->inCallback||
        (count&&!calls)||count>32768){diagnostic_.error=E::input;return false;}
    auto& s=*storage_;s.inCallback=true;bool ok=true;
    // Resource-read-only after private preparation. A missing cold owner
    // rejects before callbacks; a live preflight never widens the population.
    for(std::size_t i=0;i<count&&ok;++i){const auto& c=calls[i];diagnostic_.widget=c.widget;
        if(!idOk(c.widget)){ok=false;break;}const auto& e=s.entries[c.widget];
        if(!dynamic(e.state.kind))continue;
        const bool needs=c.operation==O::select||(e.state.kind==K::character&&c.operation==O::clearSelection);
        // Only a retained cold descriptor and sealed backend pair can acquire a graph.
        if(needs&&((e.state.kind==K::character&&!e.characterAssets)||(e.state.kind==K::pod&&!e.podAssets))){
            diagnostic_.error=E::cold;ok=false;
        }
    }
    if(ok&&!s.backend.preflight(s.backend.context,*this,calls,count)){diagnostic_.error=E::backend;ok=false;}
    s.inCallback=false;if(!ok&&diagnostic_.error==E::none)diagnostic_.error=E::input;
    return ok;
}
bool OriginalUiRiderDynamicOwner50KG::shown(std::uint16_t id,bool value) noexcept {
    auto& s=*storage_;if(!s.backend.setShown(s.backend.context,id,value)){diagnostic_.error=E::backend;return false;}
    s.entries[id].state.shown=value;return true;
}
bool OriginalUiRiderDynamicOwner50KG::generic(std::uint16_t id,bool selected) noexcept {
    auto& e=storage_->entries[id];e.state.selected=selected;
    const bool enable=selected?e.enableSelect:e.enableDeselect;
    const bool disable=selected?e.disableSelect:e.disableDeselect;
    if(enable&&disable)return true; // Source explicitly suppresses both.
    if(enable&&!shown(id,true))return false;
    return !disable||shown(id,false);
}
bool OriginalUiRiderDynamicOwner50KG::loadModel(std::uint16_t id) noexcept {
    auto& s=*storage_;auto& e=s.entries[id];
    if(!dynamic(e.state.kind)||(!e.characterAssets&&!e.podAssets)){diagnostic_.error=E::input;return false;}
    if(e.registered)return true;
    auto* ledger=s.backend.materialLifetime(s.backend.context);
    const auto* footprint=s.backend.graphFootprint(s.backend.context,id);
    if(!ledger||!footprint){diagnostic_.error=E::backend;return false;}
    e.detachComplete=false;
    bool ok=false;
    if(e.state.kind==K::character){
        if(!e.characterAssets){diagnostic_.error=E::cold;return false;}
        ok=e.everLoaded?e.character.reload(*s.motion,*s.ik,*ledger,*footprint,diagnostic_.model):
            e.character.beginLoad(e.characterAssets,*s.motion,*s.ik,*ledger,*footprint,diagnostic_.model);
    }else{
        if(!e.podAssets){diagnostic_.error=E::cold;return false;}
        ok=e.everLoaded?e.pod.reload(*ledger,*footprint,diagnostic_.model):
            e.pod.load(e.podAssets,*ledger,*footprint,diagnostic_.model);
    }
    if(!ok){diagnostic_.error=E::model;unloadModel(id);return false;}
    e.everLoaded=true;e.registered=true; // Cleanup owns even a partial attach.
    if(!s.backend.attach(s.backend.context,id,*this)){diagnostic_.error=E::backend;unloadModel(id);return false;}
    if(e.state.kind==K::character&&!e.character.finishAttachment(*s.libm,diagnostic_.model)){
        diagnostic_.error=E::model;unloadModel(id);return false;
    }
    return true;
}
bool OriginalUiRiderDynamicOwner50KG::unloadModel(std::uint16_t id) noexcept {
    auto& s=*storage_;auto& e=s.entries[id];
    const bool pending=e.character.requiresModelCleanup()||e.pod.requiresModelCleanup();
    if(!e.registered&&!pending)return true;
    if(!e.detachComplete){
        // Backend verifies the actual source/renderer completion entitlement
        // BEFORE removing borrows/membership/helpers. False cannot clear our
        // registered state or authorize graph destruction.
        if(!s.backend.drainAndDetach(s.backend.context,id,*this)){
            diagnostic_.cleanupPending=true;
            diagnostic_.cleanupWidget=id;diagnostic_.cleanupPhase=OriginalUiRiderDynamicCleanupPhase50KG::detach;
            if(diagnostic_.error==E::none)diagnostic_.error=E::backend;
            s.poisoned=true;return false;
        }
        e.detachComplete=true;
    }
    OriginalUiRiderModelDiagnostic50KG cleanup;
    const bool cleaned=e.state.kind==K::character?e.character.modelDestroyedAfterDetach(cleanup):
        e.pod.modelDestroyedAfterDetach(cleanup);
    if(!cleaned){
        diagnostic_.cleanupPending=true;diagnostic_.cleanupModel=cleanup;
        diagnostic_.cleanupWidget=id;diagnostic_.cleanupPhase=OriginalUiRiderDynamicCleanupPhase50KG::graphRelease;
        if(diagnostic_.error==E::none)diagnostic_.error=E::model;
        s.poisoned=true;return false;
    }
    e.registered=false;return true;
}
bool OriginalUiRiderDynamicOwner50KG::apply(const OriginalUiRiderWidgetCallback50KG& c,
    OriginalUiRiderWidgetState50KG& actual) noexcept {
    diagnostic_={};diagnostic_.widget=c.widget;diagnostic_.sourcePC=c.sourcePC;
    if(!storage_||storage_->poisoned||storage_->drained||storage_->inCallback||!idOk(c.widget)){
        diagnostic_.error=E::poisoned;return false;
    }
    auto& s=*storage_;auto& e=s.entries[c.widget];
    if(!dynamic(e.state.kind)||e.state.kind!=actual.kind||e.state.onTop!=actual.onTop||
        e.state.selected!=actual.selected||e.state.shown!=actual.shown){diagnostic_.error=E::input;s.poisoned=true;return false;}
    // Compute only copied semantic expectations. Do not use projection-health
    // admission to strengthen existing real callback/cleanup preconditions, and
    // do not skip any original effect even if an expectation cannot be formed.
    auto expected=e.state;bool expectedRegistered=e.registered;
    const bool expectedValid=projectNativeDescriptorOne(c,expected,expectedRegistered);
    s.inCallback=true;bool ok=true;
    switch(c.operation){
    case O::select:
        if(e.state.kind==K::character){
            ok=loadModel(c.widget)&&generic(c.widget,true);if(ok)e.character.selected();
        }else{
            // Normal Rider delay250 is source constructor0, not2500ms.
            ok=generic(c.widget,true)&&loadModel(c.widget)&&generic(c.widget,true);
            if(ok){e.pod.selected();ok=shown(c.widget,true);}
        }
        break;
    case O::deselect:
        if(e.state.kind==K::character)ok=unloadModel(c.widget)&&generic(c.widget,false);
        else{
            ok=generic(c.widget,false);
            if(ok){if(e.podAssets&&e.podAssets->config()->cleanFromMemory)ok=unloadModel(c.widget);
                if(ok)ok=generic(c.widget,false);}
            if(ok){e.pod.deselected();ok=shown(c.widget,false);}
        }
        break;
    case O::clearSelection:
        // Character+88=1D2650 really LOADS, unlike pod2A79D0.
        if(e.state.kind==K::character)ok=loadModel(c.widget);
        if(ok)e.state.selected=false;
        break;
    case O::setEnabled:e.state.selected=c.value;ok=shown(c.widget,c.value);break;
    case O::show:case O::reset:ok=shown(c.widget,true);break;
    case O::hide:ok=shown(c.widget,false);break;
    default:ok=false;diagnostic_.error=E::input;break;
    }
    s.inCallback=false;
    if(!ok){s.poisoned=true;if(diagnostic_.error==E::none)diagnostic_.error=E::backend;return false;}
    // This is a check AFTER successful real hooks/load/unload/selection. A
    // mismatch is a retained source fault, never synthetic callback completion
    // or rollback of already-consumed material, motion, IK or RNG state.
    if(!expectedValid||!sameState(e.state,expected)||e.registered!=expectedRegistered){
        diagnostic_.error=E::input;s.poisoned=true;return false;
    }
    actual=e.state;return true;
}
bool OriginalUiRiderDynamicOwner50KG::drain() noexcept {
    if(!storage_||storage_->drained)return true;auto& s=*storage_;
    if(s.inCallback){diagnostic_.cleanupPending=true;
        diagnostic_.cleanupPhase=OriginalUiRiderDynamicCleanupPhase50KG::callbackActive;s.poisoned=true;return false;}
    // Explicit teardown, never retry a partly executed selection batch.
    // A completed detach phase is retained so a graph-release failure cannot
    // replay membership/helper destruction. Unregistered tokens count too.
    for(std::uint16_t id=1;id<120;++id)if(!unloadModel(id))return false;
    diagnostic_.cleanupPending=false;diagnostic_.cleanupWidget=0;
    diagnostic_.cleanupPhase=OriginalUiRiderDynamicCleanupPhase50KG::none;s.drained=true;return true;
}
bool OriginalUiRiderDynamicOwner50KG::advanceWidget(std::uint16_t id,float dt,bool ancestorsShown) noexcept {
    diagnostic_={};diagnostic_.widget=id;
    if(!storage_||storage_->poisoned||storage_->drained||storage_->inCallback||!idOk(id)||!std::isfinite(dt)||dt<0){diagnostic_.error=E::input;return false;}
    auto& s=*storage_;auto& e=s.entries[id];if(!dynamic(e.state.kind)){diagnostic_.error=E::input;return false;}
    if(!ancestorsShown||!e.state.shown||!e.registered)return true;
    const bool ok=e.state.kind==K::character?e.character.advance(dt,*s.rng,*s.motion,*s.ik,diagnostic_.model):e.pod.advance(dt,diagnostic_.model);
    if(!ok){diagnostic_.error=E::model;s.poisoned=true;}return ok;
}
bool OriginalUiRiderDynamicOwner50KG::preflightConfirmCharacter(std::uint16_t id,
    OriginalUiRiderDynamicDiagnostic50KG& d) const noexcept {
    d={};d.widget=id;
    if(!storage_||storage_->poisoned||storage_->drained||storage_->inCallback||!idOk(id)||
       storage_->entries[id].state.kind!=K::character){d.error=E::input;return false;}
    const auto& e=storage_->entries[id];if(!e.registered)return true;
    if(!e.character.preflightConfirm(d.model)){d.error=E::model;return false;}return true;
}
bool OriginalUiRiderDynamicOwner50KG::confirmCharacter(std::uint16_t id) noexcept {
    diagnostic_={};diagnostic_.widget=id;
    if(!storage_||storage_->poisoned||storage_->drained||!idOk(id)||storage_->entries[id].state.kind!=K::character){diagnostic_.error=E::input;return false;}
    auto& e=storage_->entries[id];if(!e.registered)return true; // Actual1D269C unloaded branch.
    if(!e.character.confirm(diagnostic_.model)){diagnostic_.error=E::model;storage_->poisoned=true;return false;}return true;
}
bool OriginalUiRiderDynamicOwner50KG::sourceCharacterReadiness(std::uint16_t id)const noexcept {
    if(!storage_||storage_->poisoned||storage_->drained||!idOk(id)||storage_->entries[id].state.kind!=K::character)return false;
    const auto& e=storage_->entries[id];return !e.registered||e.character.sourceReadiness();
}
OriginalUiRiderCharacterScene50KG* OriginalUiRiderDynamicOwner50KG::characterScene(std::uint16_t id) noexcept {
    return storage_&&idOk(id)&&storage_->entries[id].state.kind==K::character?&storage_->entries[id].character:nullptr;
}
OriginalUiRiderPodScene50KG* OriginalUiRiderDynamicOwner50KG::podScene(std::uint16_t id) noexcept {
    return storage_&&idOk(id)&&storage_->entries[id].state.kind==K::pod?&storage_->entries[id].pod:nullptr;
}
std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG> OriginalUiRiderDynamicOwner50KG::characterAssets(std::uint16_t id)const noexcept {
    return storage_&&idOk(id)?storage_->entries[id].characterAssets:nullptr;
}
std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> OriginalUiRiderDynamicOwner50KG::podAssets(std::uint16_t id)const noexcept {
    return storage_&&idOk(id)?storage_->entries[id].podAssets:nullptr;
}
}
