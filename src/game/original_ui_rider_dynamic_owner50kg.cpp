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
}
struct OriginalUiRiderDynamicOwner50KG::Storage {
    struct Entry {
        OriginalUiRiderWidgetState50KG state;
        bool enableSelect=false,disableSelect=false,enableDeselect=false,disableDeselect=false;
        bool everLoaded=false,registered=false;
        std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG> characterAssets;
        std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> podAssets;
        OriginalUiRiderCharacterScene50KG character;OriginalUiRiderPodScene50KG pod;
    };
    std::array<Entry,120> entries;
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    ModelMotionSharedState49T* motion=nullptr;ModelMode0IkShared50KG* ik=nullptr;
    SourceLibmState49T* libm=nullptr;CoupledNoiseContext49M* rng=nullptr;
    OriginalUiRiderDynamicBackend50KG backend;
    bool coldPrepared=false,poisoned=false,inCallback=false,drained=false;
};
OriginalUiRiderDynamicOwner50KG::OriginalUiRiderDynamicOwner50KG() noexcept=default;
OriginalUiRiderDynamicOwner50KG::~OriginalUiRiderDynamicOwner50KG(){drain();}
bool OriginalUiRiderDynamicOwner50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const OriginalUiRiderWidgets50KG& widgets,ModelMotionSharedState49T& motion,ModelMode0IkShared50KG& ik,
    SourceLibmState49T& libm,CoupledNoiseContext49M& rng,const OriginalUiRiderDynamicBackend50KG& backend,
    OriginalUiRiderDynamicDiagnostic50KG& d) noexcept {
    d={};const auto fail=[&](E e){d.error=e;diagnostic_=d;return false;};
    if(storage_)return fail(E::alreadyBound);
    if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::rider)||
        !widgets.selectionData()||widgets.poisoned()||!backend.preflight||!backend.attach||
        !backend.drainAndDetach||!backend.setShown)return fail(E::input);
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
        storage_=std::move(next);diagnostic_={};d={};return true;
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
OriginalUiRiderWidgetState50KG OriginalUiRiderDynamicOwner50KG::state(std::uint16_t id)const noexcept {
    return storage_&&idOk(id)?storage_->entries[id].state:OriginalUiRiderWidgetState50KG{};
}
bool OriginalUiRiderDynamicOwner50KG::prepareCold(OriginalUiRiderColdPopulation50KG population,
    OriginalUiRiderDynamicDiagnostic50KG& d) noexcept {
    d={};const auto fail=[&](E e){d.error=e;diagnostic_=d;return false;};
    if(!storage_||storage_->coldPrepared||storage_->poisoned||storage_->drained||storage_->inCallback||
       (population!=OriginalUiRiderColdPopulation50KG::selected19&&
        population!=OriginalUiRiderColdPopulation50KG::rider19And20))return fail(E::input);
    auto& s=*storage_;
    for(const auto& e:s.entries)if(e.everLoaded||e.registered||e.characterAssets||e.podAssets||
       (dynamic(e.state.kind)&&(e.state.selected||e.state.shown)))return fail(E::input);
    try {
        std::array<std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG>,120> characters{};
        std::array<std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG>,120> pods{};
        const bool include20=population==OriginalUiRiderColdPopulation50KG::rider19And20;
        // Original Control16/17 membership is verified by the private session.
        // This order fixes only cold bank remapping, never source callbacks.
        for(const std::uint16_t id:{27,28,53,54}){
            if(!include20&&(id==28||id==54))continue;d.widget=id;
            const auto kind=s.entries[id].state.kind;
            if(id==27||id==28){
                if(kind!=K::pod)return fail(E::input);
                auto a=std::make_shared<assets::OriginalUiRiderPodAssets50KG>();
                if(!a->load(s.archive,id,d.pod))return fail(E::cold);
                if(a->widgetId()!=id||a->widgetSource()!=s.archive->find("UI/UIRider/Widgets.Dat").identity||
                   a->modelSource().identity!=s.archive->find(id==27?"UI/Geometry/19.psg":"UI/Geometry/20.psg").identity)
                    return fail(E::cold);
                pods[id]=std::move(a);
            }else{
                if(kind!=K::character)return fail(E::input);
                auto a=std::make_shared<assets::OriginalUiRiderCharacterAssets50KG>();
                if(!a->load(s.archive,id,d.character))return fail(E::cold);
                if(a->widgetId()!=id||!a->baseName()||*a->baseName()!=(id==53?"19c":"20c")||
                   a->widgetSource()!=s.archive->find("UI/UIRider/Widgets.Dat").identity||
                   a->mcfSource()!=s.archive->find(id==53?"UI/Anim/19c.mcf":"UI/Anim/20c.mcf").identity)
                    return fail(E::cold);
                characters[id]=std::move(a);
            }
        }
        // These are immutable files/geometry/motions only. No graph, entry
        // sample, attachment, IK, visibility, binder clock or source row exists.
        for(std::uint16_t id=1;id<120;++id){s.entries[id].characterAssets=std::move(characters[id]);
            s.entries[id].podAssets=std::move(pods[id]);}
        s.coldPrepared=true;diagnostic_={};d={};return true;
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
    auto& s=*storage_;auto& e=s.entries[id];if(e.registered)return true;
    bool ok=false;
    if(e.state.kind==K::character){
        if(!e.characterAssets){diagnostic_.error=E::cold;return false;}
        ok=e.everLoaded?e.character.reload(*s.motion,*s.ik,diagnostic_.model):
            e.character.beginLoad(e.characterAssets,*s.motion,*s.ik,diagnostic_.model);
    }else{
        if(!e.podAssets){diagnostic_.error=E::cold;return false;}
        ok=e.everLoaded?e.pod.reload(diagnostic_.model):e.pod.load(e.podAssets,diagnostic_.model);
    }
    if(!ok){diagnostic_.error=E::model;return false;}
    e.everLoaded=true;e.registered=true; // Cleanup owns even a partial attach.
    if(!s.backend.attach(s.backend.context,id,*this)){diagnostic_.error=E::backend;unloadModel(id);return false;}
    if(e.state.kind==K::character&&!e.character.finishAttachment(*s.libm,diagnostic_.model)){
        diagnostic_.error=E::model;unloadModel(id);return false;
    }
    return true;
}
void OriginalUiRiderDynamicOwner50KG::unloadModel(std::uint16_t id) noexcept {
    auto& s=*storage_;auto& e=s.entries[id];if(!e.registered)return;
    s.backend.drainAndDetach(s.backend.context,id,*this);e.registered=false;
    if(e.state.kind==K::character)e.character.modelDestroyedAfterDetach();
    else e.pod.modelDestroyedAfterDetach();
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
        if(e.state.kind==K::character){unloadModel(c.widget);ok=generic(c.widget,false);}
        else{
            ok=generic(c.widget,false);
            if(ok){if(e.podAssets&&e.podAssets->config()->cleanFromMemory)unloadModel(c.widget);
                ok=generic(c.widget,false);}
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
    actual=e.state;return true;
}
void OriginalUiRiderDynamicOwner50KG::drain() noexcept {
    if(!storage_||storage_->drained)return;auto& s=*storage_;
    // Explicit teardown, never retry a partly executed selection batch.
    for(std::uint16_t id=1;id<120;++id)unloadModel(id);
    s.drained=true;
}
bool OriginalUiRiderDynamicOwner50KG::advanceWidget(std::uint16_t id,float dt,bool ancestorsShown) noexcept {
    diagnostic_={};diagnostic_.widget=id;
    if(!storage_||storage_->poisoned||storage_->drained||storage_->inCallback||!idOk(id)||!std::isfinite(dt)||dt<0){diagnostic_.error=E::input;return false;}
    auto& s=*storage_;auto& e=s.entries[id];if(!dynamic(e.state.kind)){diagnostic_.error=E::input;return false;}
    if(!ancestorsShown||!e.state.shown||!e.registered)return true;
    const bool ok=e.state.kind==K::character?e.character.advance(dt,*s.rng,*s.motion,*s.ik,diagnostic_.model):e.pod.advance(dt,diagnostic_.model);
    if(!ok){diagnostic_.error=E::model;s.poisoned=true;}return ok;
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
