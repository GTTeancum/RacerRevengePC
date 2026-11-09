#include "original_ui_unlock_models50pd.h"
#include <cstdio>
#include <utility>

namespace rr::game {
namespace {
using E=OriginalUiUnlockModelError50PD;using D=OriginalUiUnlockModelDiagnostic50PD;using O=OriginalUiControlOperation50KG;
// Failure receipts carry the source line to the log (diagnostic only).
bool failAt(int line,D& d,E e,std::uint32_t pc=0) noexcept {d.error=e;d.sourcePC=pc;
    std::fprintf(stderr,"unlock_models failure line=%d error=%u pc=%x\n",line,unsigned(e),pc);return false;}
#define fail(...) failAt(__LINE__,__VA_ARGS__)
bool podWidget(std::uint16_t id) noexcept {return id>=9&&id<=23;}
bool characterWidget(std::uint16_t id) noexcept {return id>=24&&id<=38;}
}
std::uint32_t originalUiUnlockCandidatePods50PD(const OriginalUnlockFacts50PD& f) noexcept {
    const auto checks=evaluateOriginalUnlocks187E30(f);std::uint32_t mask=0;
    for(std::size_t i=0;i<7;++i)if(checks.pending[i]){const auto pod=originalUnlockPod50PD(f,i);if(pod>=0&&pod<27)mask|=1u<<pod;}
    if(f.unlock10FAC[26]==0)mask|=1u<<26;
    return mask;
}
struct OriginalUiUnlockModels50PD::Storage {
    OriginalUiUnlockModelInput50PD input;
    std::vector<OriginalUiUnlockColdModel50PD> cold;
    std::shared_ptr<assets::OriginalUiRiderBinderAssets50KG> binderAssets;
    OriginalUiRiderMaterialFootprint50KG binderFootprint;
    OriginalUiRiderMaterialLifetime50KG lifetime;  // outlives every scene/helper below
    OriginalUiRiderVisibilityOwner50KG visibility;
    OriginalUiUnlockModelShared50PD shared{};
    struct Entry {
        const OriginalUiUnlockColdModel50PD* cold=nullptr;
        OriginalUiRiderPodScene50KG pod;OriginalUiRiderCharacterScene50KG character;
        std::array<std::unique_ptr<OriginalUiRiderBinderAnimation50KG>,2> binders;
        OriginalUiRiderVisibilityHandle50KG membership{};
        bool registered=false,everLoaded=false,shown=false;
    };
    std::array<Entry,39> entries; // widgets 9..38
    bool live=false,poisoned=false,drained=false;
    bool acknowledge(D& d) noexcept {return lifetime.acknowledgeThrough(lifetime.sequence(),d.lifetime)||fail(d,E::lifetime,0x14D440);}
    bool load(std::uint16_t id,D& d) noexcept {
        auto& e=entries[id];if(e.registered)return true;if(!e.cold)return fail(d,E::input,0x1D3590);
        const bool pilot=characterWidget(id);bool ok=false;
        if(pilot)ok=e.everLoaded?e.character.reload(*shared.motion,*shared.ik,lifetime,e.cold->characterFootprint,d.model):
            e.character.beginLoad(e.cold->characterAssets,*shared.motion,*shared.ik,lifetime,e.cold->characterFootprint,d.model); // 1D22A0
        else ok=e.everLoaded?e.pod.reload(lifetime,e.cold->podFootprint,d.model):
            e.pod.load(e.cold->podAssets,lifetime,e.cold->podFootprint,d.model);                                         // 1D3590
        if(!ok){unload(id,d);return fail(d,E::model,pilot?0x1D22A0:0x1D3590);}
        e.everLoaded=true;e.registered=true;
        if(!pilot){ // backend attach: binder helpers 1D3E10, then 141960 registration
            for(std::size_t pair=0;pair<2;++pair)if(e.pod.binderPresent(pair)){
                e.binders[pair]=std::make_unique<OriginalUiRiderBinderAnimation50KG>();
                if(!e.binders[pair]->initialize(binderAssets,lifetime,binderFootprint,d.lifetime)){unload(id,d);return fail(d,E::binder,0x1D3E10);}
            }
            if(!visibility.registerModel(e.cold->podAssets,e.pod,e.shown,e.membership,d.visibility)){unload(id,d);return fail(d,E::visibility,0x1D35F4);}
        }else{
            if(!visibility.registerModel(e.cold->characterAssets,e.character,e.shown,e.membership,d.visibility)){unload(id,d);return fail(d,E::visibility,0x1D24A8);}
            if(!e.character.finishAttachment(*shared.libm,d.model)){unload(id,d);return fail(d,E::model,0x1D24A8);}
        }
        return acknowledge(d);
    }
    bool unload(std::uint16_t id,D& d) noexcept {
        auto& e=entries[id];
        const bool pending=e.character.requiresModelCleanup()||e.pod.requiresModelCleanup();
        if(!e.registered&&!pending)return true;
        if(visibility.contains(e.membership)&&!visibility.removeModel(e.membership)){poisoned=true;return fail(d,E::visibility,0x280A00);}
        e.membership={};
        for(int pair=1;pair>=0;--pair)if(e.binders[pair]){
            if(!e.binders[pair]->destroy(d.lifetime)){poisoned=true;return fail(d,E::binder,0x150590);}e.binders[pair].reset();}
        OriginalUiRiderModelDiagnostic50KG cleanup;
        const bool cleaned=characterWidget(id)?e.character.modelDestroyedAfterDetach(cleanup):e.pod.modelDestroyedAfterDetach(cleanup);
        if(!cleaned){d.model=cleanup;poisoned=true;return fail(d,E::model,0x1D36F0);}
        e.registered=false;return acknowledge(d);
    }
    bool shownState(std::uint16_t id,bool value) noexcept {
        auto& e=entries[id];e.shown=value;return !e.registered||visibility.setShown(e.membership,value);
    }
};
OriginalUiUnlockModels50PD::OriginalUiUnlockModels50PD() noexcept=default;
OriginalUiUnlockModels50PD::~OriginalUiUnlockModels50PD(){
    // A live graph/helper is never silently released from a destructor.
    if(storage_&&storage_->live&&!storage_->drained)(void)storage_.release();
}
bool OriginalUiUnlockModels50PD::prepareCold(const OriginalUiUnlockModelInput50PD& in,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyBound);
    const auto authentic=[](const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,assets::OriginalUiArchiveId50KG id){
        return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);};
    if(!authentic(in.unlock,assets::OriginalUiArchiveId50KG::unlock)||!authentic(in.appStart,assets::OriginalUiArchiveId50KG::appStart)||
       !authentic(in.podTextures,assets::OriginalUiArchiveId50KG::podTextures)||!in.page||!in.page->page()||!in.runtime.overrideKnown||
       (in.podMask>>27))return fail(d,E::input);
    try {
        auto s=std::make_unique<Storage>();s->input=in;
        if(!s->lifetime.bind(in.appStart,d.lifetime))return fail(d,E::lifetime,0x26E640);
        if(!s->visibility.bindUnlock(in.unlock,d.visibility))return fail(d,E::visibility,0x1CCD44);
        auto binder=std::make_shared<assets::OriginalUiRiderBinderAssets50KG>();
        if(!binder->loadUnlock(in.unlock,in.appStart,in.runtime,d.binder))return fail(d,E::binder,0x1D3E10);
        if(!OriginalUiRiderMaterialFootprint50KG::unlockBinder(*in.appStart,*binder,s->binderFootprint,d.lifetime))return fail(d,E::footprint,0x150450);
        s->binderAssets=std::move(binder);
        s->cold.reserve(27);
        for(std::int32_t pod=0;pod<27;++pod){
            if(!(in.podMask&(1u<<pod)))continue;
            const auto control=in.page->podControl(pod);
            if(control<0)return fail(d,E::input,0x20AFD4);   // no group-2 control: never displayable
            const auto& widgets=in.page->page()->controls[std::size_t(control)].widgets;
            if(widgets.size()!=3||!podWidget(widgets[0])||!characterWidget(widgets[2]))return fail(d,E::input,0x20A850);
            OriginalUiUnlockColdModel50PD c;c.pod=pod;c.podWidget=widgets[0];c.characterWidget=widgets[2];d.widget=c.podWidget;
            auto podAssets=std::make_shared<assets::OriginalUiRiderPodAssets50KG>();
            if(!podAssets->loadUnlock(in.unlock,c.podWidget,d.pod))return fail(d,E::pod,0x1D31D0);
            auto podMaterials=std::make_shared<assets::OriginalUiRiderModelMaterials50KG>();
            if(!podMaterials->loadUnlockPod(podAssets,in.appStart,in.podTextures,in.runtime,in.ipumCache,d.material))return fail(d,E::material,0x26E640);
            if(!OriginalUiRiderMaterialFootprint50KG::unlockPod(*in.appStart,*podAssets,c.podFootprint,d.lifetime))return fail(d,E::footprint,0x26E640);
            d.widget=c.characterWidget;
            auto characterAssets=std::make_shared<assets::OriginalUiRiderCharacterAssets50KG>();
            if(!characterAssets->loadUnlock(in.unlock,c.characterWidget,d.character))return fail(d,E::character,0x1D2120);
            auto characterMaterials=std::make_shared<assets::OriginalUiRiderModelMaterials50KG>();
            if(!characterMaterials->loadUnlockCharacter(characterAssets,in.unlock,in.appStart,in.runtime,in.ipumCache,d.material))return fail(d,E::material,0x26E640);
            if(!OriginalUiRiderMaterialFootprint50KG::unlockCharacter(*in.appStart,*characterAssets,c.characterFootprint,d.lifetime))return fail(d,E::footprint,0x1538B0);
            c.podAssets=std::move(podAssets);c.podMaterials=std::move(podMaterials);
            c.characterAssets=std::move(characterAssets);c.characterMaterials=std::move(characterMaterials);
            s->cold.push_back(std::move(c));
        }
        for(const auto& c:s->cold){s->entries[c.podWidget].cold=&c;s->entries[c.characterWidget].cold=&c;}
        storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,E::allocation);}
}
const std::vector<OriginalUiUnlockColdModel50PD>* OriginalUiUnlockModels50PD::cold() const noexcept {return storage_?&storage_->cold:nullptr;}
std::shared_ptr<const assets::OriginalUiRiderBinderAssets50KG> OriginalUiUnlockModels50PD::binderAssets() const noexcept {return storage_?storage_->binderAssets:nullptr;}
bool OriginalUiUnlockModels50PD::dynamicWidget(std::uint16_t id) const noexcept {return podWidget(id)||characterWidget(id);}
bool OriginalUiUnlockModels50PD::bindLive(const OriginalUiUnlockModelShared50PD& shared,D& d) noexcept {
    d={};if(!storage_)return fail(d,E::notBound);if(storage_->live)return fail(d,E::alreadyBound);
    if(!shared.motion||!shared.ik||!shared.libm||!shared.rng)return fail(d,E::input);
    storage_->shared=shared;storage_->live=true;return true;
}
bool OriginalUiUnlockModels50PD::apply(const OriginalUiOptionsMenuCallback50KG& c,bool selected,bool shown,D& d) noexcept {
    d={};d.widget=c.widgetId;d.sourcePC=c.sourcePC;
    if(!storage_||!storage_->live||storage_->poisoned||storage_->drained||!dynamicWidget(c.widgetId))return fail(d,E::phase);
    auto& s=*storage_;const auto id=c.widgetId;auto& e=s.entries[id];const bool pilot=characterWidget(id);
    bool ok=true;
    switch(c.operation){
    case O::select:
        // Pilot 1D2550 loads then selects; pod 1D33C0 loads, unpauses, shows.
        // A widget whose pod is outside the prepared candidates cannot be shown.
        if(!e.cold)return fail(d,E::input,pilot?0x1D22A0:0x1D3590);
        e.shown=shown;ok=s.load(id,d);
        if(ok){if(pilot)e.character.selected();else{e.pod.selected();ok=s.shownState(id,shown);}}
        break;
    case O::deselect:
        if(pilot)ok=s.unload(id,d);
        else{if(e.registered&&e.cold&&e.cold->podAssets->config()->cleanFromMemory)ok=s.unload(id,d);
            if(ok){e.pod.deselected();ok=s.shownState(id,shown);}}
        break;
    case O::clearSelection:
        if(pilot&&e.cold)ok=s.load(id,d); // character +88 = 1D2650 really loads
        break;
    case O::setEnabled:case O::show:case O::reset:case O::hide:ok=s.shownState(id,shown);break;
    }
    if(!ok){s.poisoned=true;if(d.error==E::none)d.error=E::model;return false;}
    (void)selected;return true;
}
bool OriginalUiUnlockModels50PD::advance(float dt,bool ancestorsShown,D& d) noexcept {
    d={};if(!storage_||!storage_->live||storage_->poisoned||storage_->drained||!std::isfinite(dt)||dt<0)return fail(d,E::phase,0x2A6BB0);
    auto& s=*storage_;if(!ancestorsShown)return true;
    for(std::uint16_t id=9;id<=38;++id){auto& e=s.entries[id];if(!e.registered||!e.shown)continue;d.widget=id;
        const bool ok=characterWidget(id)?e.character.advance(dt,*s.shared.rng,*s.shared.motion,*s.shared.ik,d.model):e.pod.advance(dt,d.model);
        if(!ok){s.poisoned=true;return fail(d,E::model,characterWidget(id)?0x1D2730:0x1D38B0);}}
    return true;
}
bool OriginalUiUnlockModels50PD::beginFrame(const OriginalUiUnlockCameraFrame50PD& frame,D& d) noexcept {
    d={};if(!storage_||!storage_->live||storage_->poisoned)return fail(d,E::phase,0x27F650);
    return storage_->visibility.prepareFrame(frame.render.cameraAfterFog,{},d.visibility)||fail(d,E::visibility,0x27F650);
}
bool OriginalUiUnlockModels50PD::loaded(std::uint16_t id) const noexcept {
    return storage_&&dynamicWidget(id)&&storage_->entries[id].registered&&storage_->entries[id].shown;
}
bool OriginalUiUnlockModels50PD::prepare(std::uint16_t id,const OriginalUiUnlockCameraFrame50PD& frame,
    OriginalUiUnlockModelPacket50PD& out,D& d) noexcept {
    d={};d.widget=id;if(!storage_||!storage_->live||storage_->poisoned||!loaded(id))return fail(d,E::phase,0x26EFC0);
    auto& s=*storage_;auto& e=s.entries[id];OriginalUiUnlockModelPacket50PD next;
    if(!s.visibility.selectModel(e.membership,next.selection,d.visibility)||!s.visibility.isCurrentSelection(next.selection))
        return fail(d,E::visibility,0x26EFC0);
    if(!next.selection.selected()){out=next;return true;}
    const auto& render=frame.render;
    if(characterWidget(id)){
        const auto& a=*e.cold->characterAssets;
        if(!e.character.attached()||e.character.scene()!=a.scene())return fail(d,E::model,0x2412E4);
        next.packet.context=frame.characterContext;
        if(!e.character.projectPalette(next.selection.sourceModelFlags191,next.packet.palette,d.model))return fail(d,E::packet,0x2412E4);
        if(!prepareOriginalUiRiderModelFog50KG(*a.hierarchy(),e.character.graph(),render.cameraAfterFog.position,render.fogStart,next.packet.fog))
            return fail(d,E::packet,0x26F070);
        next.packet.scene=a.scene();next.materials=e.cold->characterMaterials.get();next.characterAssets=&a;
    }else{
        const auto& a=*e.cold->podAssets;
        if(!e.pod.loaded()||e.pod.scene()!=a.scene())return fail(d,E::model,0x2412E4);
        next.packet.context=frame.podContext;
        if(!e.pod.projectPalette(next.selection.sourceModelFlags191,next.packet.palette,d.model))return fail(d,E::packet,0x2412E4);
        if(!prepareOriginalUiRiderModelFog50KG(*a.hierarchy(),e.pod.graph(),render.cameraAfterFog.position,render.fogStart,next.packet.fog))
            return fail(d,E::packet,0x26F070);
        next.packet.scene=a.scene();next.materials=e.cold->podMaterials.get();
    }
    next.packet.sourceModelFlags191=next.selection.sourceModelFlags191;
    if(!s.visibility.recordPacketFog(next.selection,next.packet.fog.computeRadialFog1B2))return fail(d,E::visibility,0x24136C);
    next.emitsGeometry=true;out=std::move(next);return true;
}
bool OriginalUiUnlockModels50PD::binderPresent(std::uint16_t id,std::size_t pair) const noexcept {
    return storage_&&podWidget(id)&&pair<2&&storage_->entries[id].registered&&bool(storage_->entries[id].binders[pair]);
}
bool OriginalUiUnlockModels50PD::projectBinder(std::uint16_t id,std::size_t pair,const OriginalUiUnlockCameraFrame50PD& frame,
    std::uint32_t milliseconds,bool animate,OriginalUiUnlockBinderPacket50PD& out,D& d) noexcept {
    d={};d.widget=id;if(!binderPresent(id,pair)||!loaded(id)||storage_->poisoned)return fail(d,E::phase,0x1D3F90);
    auto& e=storage_->entries[id];OriginalUiRiderBinderQuad50KG source;OriginalUiUnlockBinderPacket50PD next;
    if(!e.pod.projectBinder(pair,frame.render.cameraAfterFog.binderCameraColumn,source,d.model))return fail(d,E::model,0x1D3F90);
    if(!(animate?e.binders[pair]->advanceAfterGeometry(milliseconds,next.image):e.binders[pair]->current(next.image)))return fail(d,E::binder,0x1D4240);
    if(projectOriginalUiRiderBinderPacket50KM(source,frame.podContext,frame.lighting.ambient50,next.geometry)!=OriginalUiLitGeometryStatus50KM::complete)
        return fail(d,E::model,0x1D4240);
    out=next;return true;
}
bool OriginalUiUnlockModels50PD::drain(D& d) noexcept {
    d={};if(!storage_||storage_->drained)return true;auto& s=*storage_;
    for(std::uint16_t id=9;id<=38;++id)if(!s.unload(id,d))return false;
    s.drained=true;return true;
}
}
