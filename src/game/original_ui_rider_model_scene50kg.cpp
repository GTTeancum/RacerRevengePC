// NEW unverified source reconstruction: read-only Confirm preflight.
#include "original_ui_rider_model_scene50kg.h"
#include <cstring>
#include <new>
#include <utility>
namespace rr::game {
namespace {
using I=SceneNodeIndex49P;
using D=OriginalUiRiderModelDiagnostic50KG;
using E=OriginalUiRiderModelError50KG;
bool fail(D& d,E e,std::uint32_t pc) noexcept {d.error=e;d.sourcePC=pc;return false;}
struct MaterialDomain {
    enum class Phase : std::uint8_t {absent,live,invalidated};
    OriginalUiRiderMaterialLifetime50KG* ledger=nullptr;
    OriginalUiRiderMaterialFootprint50KG footprint{};
    OriginalUiRiderMaterialOwnerToken50KG token{};
    Phase phase=Phase::absent;
    bool pending()const noexcept{return phase!=Phase::absent;}
    bool active()const noexcept{return phase==Phase::live&&ledger&&ledger->owns(token);}
    bool adopt(OriginalUiRiderMaterialLifetime50KG& owner,
        const OriginalUiRiderMaterialFootprint50KG& source,D& d) noexcept {
        if(pending())return fail(d,E::phase,0x26E7D8);
        OriginalUiRiderMaterialMutation50KG mutation;
        if(!owner.acquire(source,owner.sequence(),token,mutation,d.lifetime))return fail(d,E::lifetime,0x26E7D8);
        ledger=&owner;footprint=source;phase=Phase::live;return true;
    }
    bool beforeInvalidation(D& d)const noexcept {
        if(!pending())return true;
        if(!ledger||!ledger->owns(token)){
            d.lifetime.error=OriginalUiRiderMaterialLifetimeError50KG::invariant;
            d.cleanupPending=true;
            return d.error==E::none?fail(d,E::lifetime,0x26E7D8):false;
        }
        if(ledger->canRelease(token,ledger->sequence(),d.lifetime))return true;
        d.cleanupPending=true;return d.error==E::none?fail(d,E::lifetime,0x26E7D8):false;
    }
    void invalidated()noexcept{if(pending())phase=Phase::invalidated;}
    bool release(D& d)noexcept {
        if(!pending())return true;
        OriginalUiRiderMaterialMutation50KG mutation;
        if(phase!=Phase::invalidated||!ledger||
           !ledger->release(token,ledger->sequence(),mutation,d.lifetime)){
            d.cleanupPending=true;return d.error==E::none?fail(d,E::lifetime,0x26E7D8):false;
        }
        // release consumes only the ledger's owner; it never clears our copy.
        token={};footprint={};ledger=nullptr;phase=Phase::absent;
        d.cleanupPending=false;return true;
    }
};
struct ModelGraph {
    std::array<SceneNode49P,maxSceneNodes49P> nodes{};
    std::array<I,assets::maxModelNodes49O> sources{};
    std::array<std::array<char,128>,assets::maxModelNodes49O> names{};
    std::array<std::uint8_t,assets::maxModelNodes49O> palette{};
    std::uint32_t count=0;
    SceneGraph49P view() noexcept {return {nodes.data(),count?count+1:0};}
    bool build(const assets::PsgHierarchyView49O& psg,const std::array<float,3>& position,
        const std::array<float,3>& look,const std::array<float,3>& up,bool character) noexcept {
        if (!psg.nodeCount||psg.nodeCount>assets::maxModelNodes49O||psg.nodeCount+1>maxSceneNodes49P)
            return false;
        count=psg.nodeCount;
        //2893A0 starts local identity, world zero and dirty80. Root pose below
        // replaces local through native289EC0-equivalent branch. PSG source
        // loader supplies its original local matrices and flagsA4.
        auto& root=nodes[0];root={};
        root.sceneFlags190=character?0x88:0x80; // character1D23DC adds08
        if (!originalUiUserRotatePose50KG(position,look,up,root.localC0)) return false;
        for (std::uint32_t n=0;n<count;++n) {
            assets::PsgNode49O source{};if (!assets::readPsgNode49O(psg,n,source)) return false;
            if (n==0?source.parent!=-1:source.parent<0) return false;
            const auto index=n+1;
            const auto parent=source.parent<0?0u:static_cast<std::uint32_t>(source.parent)+1;
            auto& node=nodes[index];node={};node.sceneFlags190=0xa4;
            std::memcpy(node.localC0.v,source.local,sizeof source.local);
            node.parent180=static_cast<I>(parent);
            auto& first=nodes[parent].firstChild184;
            if (first==I::none) first=static_cast<I>(index);
            else {
                auto tail=static_cast<std::uint32_t>(first);
                while (nodes[tail].nextSibling188!=I::none)
                    tail=static_cast<std::uint32_t>(nodes[tail].nextSibling188);
                nodes[tail].nextSibling188=static_cast<I>(index);
            }
            sources[n]=static_cast<I>(index);palette[n]=static_cast<std::uint8_t>(n);
            std::memcpy(names[n].data(),source.name,sizeof source.name);
        }
        return validateSceneGraph49P(view());
    }
    static bool equal(const char* a,const char* b) noexcept {
        for (;*a&&*b;++a,++b) {
            const auto fold=[](char c){return c>='A'&&c<='Z'?char(c+32):c;};
            if (fold(*a)!=fold(*b)) return false;
        }
        return *a==*b;
    }
    I findFrom(I start,const char* name) const noexcept {
        if (start==I::none) return I::none;
        const auto index=static_cast<std::uint32_t>(start);
        if (index&&equal(names[index-1].data(),name)) return start;
        //28C590: this node, next sibling subtree, then first child subtree.
        const auto sibling=findFrom(nodes[index].nextSibling188,name);
        return sibling!=I::none?sibling:findFrom(nodes[index].firstChild184,name);
    }
    I find(const char* name) const noexcept {return findFrom(static_cast<I>(0),name);}
    bool project(std::uint8_t selected,ModelPalette49S& output,D& d) noexcept {
        ModelPaletteInput49S input{};input.phase=ModelPalettePhase49S::selectedRenderPreparation;
        input.suppliedState=ModelPaletteSceneState49S::completeCurrentScene;
        input.modelFlags191=selected;input.modelRoot=static_cast<I>(0);
        input.sourceNodes=sources.data();input.sourceNodeCount=count;
        input.paletteIds=palette.data();input.paletteCount=count;
        ModelPaletteWorkspace49S scratch{};
        return projectModelPalette49S(view(),input,output,scratch,d.palette)||fail(d,E::palette,0x2412E4);
    }
};
}
struct OriginalUiRiderCharacterScene50KG::Storage {
    std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG> sourceAssets;
    MaterialDomain material;
    ModelGraph model;
    assets::MotionNodeBindings49O bindings{};
    OriginalUiRiderCharacter50KG lifecycle;
    std::array<ChariotIkChain49T,2> chains{};
    ModelMode0IkContext50KG context{};
    ModelPlaybackIk50KG ik{};
    bool attached=false,destroyed=false;
    bool destroy(D& d)noexcept {
        if(!material.beforeInvalidation(d))return false;
        // This runs only for an unpublished candidate or AFTER actual detach.
        // Clear every mutable graph/IK/playback reference before its domain.
        if(!destroyed){lifecycle.modelDestroyed();attached=false;destroyed=true;
            model={};ik={};context={};chains={};}
        material.invalidated();return material.release(d);
    }
};
OriginalUiRiderCharacterScene50KG::OriginalUiRiderCharacterScene50KG() noexcept=default;
OriginalUiRiderCharacterScene50KG::~OriginalUiRiderCharacterScene50KG(){
    // Explicit owner teardown must precede destruction. Preserve an invariant
    // failure instead of dropping graph storage while its token remains live.
    if(requiresModelCleanup())(void)storage_.release();
}
bool OriginalUiRiderCharacterScene50KG::beginLoad(
    std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG> assets,
    ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ikShared,
    OriginalUiRiderMaterialLifetime50KG& ledger,const OriginalUiRiderMaterialFootprint50KG& footprint,D& d) noexcept {
    d={};if (storage_) return fail(d,E::alreadyPrepared,0x1D22A0);
    if (!assets||!assets->loaded()||!assets->hierarchy()||!assets->bindings()||
        !assets->position()||!assets->look()||!assets->up()) return fail(d,E::asset,0x1D22A0);
    if((assets->widgetId()!=53&&assets->widgetId()!=54)||footprint.kind()!=OriginalUiRiderMaterialOwnerKind50KG::characterGraph||
       footprint.widget()!=assets->widgetId()||footprint.source()!=assets->mcfSource())return fail(d,E::asset,0x1D22A0);
    if(!ledger.canAcquire(footprint,ledger.sequence(),d.lifetime))return fail(d,E::lifetime,0x1D22A0);
    std::unique_ptr<Storage> next;
    const auto reject=[&](E error,std::uint32_t pc)noexcept {
        fail(d,error,pc);
        if(next&&!next->destroy(d))storage_=std::move(next);
        return false;
    };
    try {
        next=std::make_unique<Storage>();next->sourceAssets=std::move(assets);
        if (!next->model.build(*next->sourceAssets->hierarchy(),*next->sourceAssets->position(),
            *next->sourceAssets->look(),*next->sourceAssets->up(),true)) return reject(E::graph,0x1D2438);
        // The complete graph's projected mesh/geometry/wrapper domain starts
        // here, before binding or the entry sample can fail.
        if(!next->material.adopt(ledger,footprint,d))return reject(E::lifetime,0x26E7D8);
        next->bindings=*next->sourceAssets->bindings();
        for (std::uint32_t row=0;row<next->bindings.count;++row) {
            auto& index=next->bindings.rows[row].sceneNode;
            if (index>=next->model.count) return reject(E::binding,0x153218);
            ++index;
        }
        if (!next->lifecycle.bind(*next->sourceAssets)) return reject(E::binding,0x1D244C);
        ModelPlaybackWorkspace50KG workspace{};
        if (!next->lifecycle.startEntry(next->bindings,next->model.view(),shared,ikShared,workspace,d.playback))
            return reject(E::playback,0x1D2478);
        storage_=std::move(next);return true;
    } catch (...) {return reject(E::allocation,0x1D22A0);}
}
bool OriginalUiRiderCharacterScene50KG::finishAttachment(SourceLibmState49T& libm,D& d) noexcept {
    d={};if (!storage_||storage_->attached||storage_->destroyed||!storage_->material.active()) return fail(d,E::phase,0x1D24A8);
    auto& s=*storage_;
    if (!s.lifecycle.captureAfterAttachment(s.bindings,s.model.view(),d.playback)) return fail(d,E::playback,0x1D24A8);
    std::array<I,2> endpoints{};std::uint32_t count=0;
    for (const char* name:{"l_IK","r_IK"}) {
        const auto endpoint=s.model.find(name);if (endpoint==I::none) continue;
        ChariotIkWorkspace49T scratch{};
        if (!initializeModelMode0Ik50KG(endpoint,s.model.view(),s.chains[count],libm,scratch,d.ik))
            return fail(d,E::ik,0x1568C8);
        endpoints[count++]=endpoint;
    }
    if (!bindModelPlaybackIk50KG(endpoints.data(),count,s.bindings,s.model.view(),s.ik,d.playback))
        return fail(d,E::ik,0x156620);
    s.context={s.chains.data(),count};s.ik.solve=modelMode0IkCallback50KG;
    s.ik.context=&s.context;s.ik.enabled204=true;
    if (!s.lifecycle.modelAttached()) return fail(d,E::phase,0x1D251C);
    s.attached=true;return true;
}
bool OriginalUiRiderCharacterScene50KG::modelDestroyedAfterDetach(D& d) noexcept {
    d={};return !storage_||storage_->destroy(d);
}
bool OriginalUiRiderCharacterScene50KG::requiresModelCleanup() const noexcept {
    return storage_&&(!storage_->destroyed||storage_->material.pending());
}
bool OriginalUiRiderCharacterScene50KG::reload(ModelMotionSharedState49T& shared,
    ModelMode0IkShared50KG& ikShared,OriginalUiRiderMaterialLifetime50KG& ledger,
    const OriginalUiRiderMaterialFootprint50KG& footprint,D& d) noexcept {
    d={};if (!storage_||!storage_->destroyed||storage_->material.pending()) return fail(d,E::phase,0x1D22A0);
    auto& s=*storage_;
    if((s.sourceAssets->widgetId()!=53&&s.sourceAssets->widgetId()!=54)||footprint.kind()!=OriginalUiRiderMaterialOwnerKind50KG::characterGraph||
       footprint.widget()!=s.sourceAssets->widgetId()||footprint.source()!=s.sourceAssets->mcfSource())return fail(d,E::asset,0x1D22A0);
    if(!ledger.canAcquire(footprint,ledger.sequence(),d.lifetime))return fail(d,E::lifetime,0x1D22A0);
    s.destroyed=false;
    const auto reject=[&](E error,std::uint32_t pc)noexcept {fail(d,error,pc);s.destroy(d);return false;};
    if (!s.model.build(*s.sourceAssets->hierarchy(),*s.sourceAssets->position(),*s.sourceAssets->look(),*s.sourceAssets->up(),true))
        return reject(E::graph,0x1D2438);
    if(!s.material.adopt(ledger,footprint,d))return reject(E::lifetime,0x26E7D8);
    ModelPlaybackWorkspace50KG scratch{};
    if (!s.lifecycle.startEntry(s.bindings,s.model.view(),shared,ikShared,scratch,d.playback))
        return reject(E::playback,0x1D2478);
    return true;
}
bool OriginalUiRiderCharacterScene50KG::attached() const noexcept {return storage_&&storage_->attached&&storage_->material.active();}
void OriginalUiRiderCharacterScene50KG::selected() noexcept {if (storage_) storage_->lifecycle.selected();}
bool OriginalUiRiderCharacterScene50KG::preflightConfirm(D& d) const noexcept {
    d={};if(!attached())return fail(d,E::phase,0x1D2690);
    // SceneGraph49P is mutable by type; use actual copied native nodes so the
    // preflight cannot modify live matrices even if a shared leaf evolves.
    auto staged=storage_->model;
    return storage_->lifecycle.preflightConfirm(storage_->bindings,staged.view(),d.playback)||
        fail(d,E::playback,0x1D2690);
}
bool OriginalUiRiderCharacterScene50KG::confirm(D& d) noexcept {
    d={};if (!attached()) return fail(d,E::phase,0x1D2690);
    return storage_->lifecycle.confirm(storage_->bindings,storage_->model.view(),d.playback)||fail(d,E::playback,0x1D2690);
}
bool OriginalUiRiderCharacterScene50KG::advance(float dt,CoupledNoiseContext49M& random,
    ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ikShared,D& d) noexcept {
    d={};if (!attached()) return fail(d,E::phase,0x1D2730);
    ModelPlaybackWorkspace50KG scratch{};auto& s=*storage_;
    return s.lifecycle.advance(dt,s.bindings,s.model.view(),s.ik,random,shared,ikShared,scratch,d.playback)||
        fail(d,E::playback,0x1D2774);
}
bool OriginalUiRiderCharacterScene50KG::projectPalette(std::uint8_t flags,ModelPalette49S& output,D& d) noexcept {
    d={};if (!attached()) return fail(d,E::phase,0x2412E4);return storage_->model.project(flags,output,d);
}
bool OriginalUiRiderCharacterScene50KG::sourceReadiness() const noexcept {
    return attached()&&storage_->lifecycle.sourceReadiness1D2710();
}
const assets::Scene48D* OriginalUiRiderCharacterScene50KG::scene() const noexcept {return storage_?storage_->sourceAssets->scene():nullptr;}
SceneGraph49P OriginalUiRiderCharacterScene50KG::graph() noexcept {return storage_?storage_->model.view():SceneGraph49P{nullptr,0};}
const OriginalUiRiderCharacter50KG* OriginalUiRiderCharacterScene50KG::lifecycle() const noexcept {return storage_?&storage_->lifecycle:nullptr;}
struct OriginalUiRiderPodScene50KG::Storage {
    std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> sourceAssets;
    MaterialDomain material;
    ModelGraph model;
    OriginalUiUserRotateParameters50KG rotation{};
    std::array<std::array<I,2>,2> binders{};
    bool paused=true,destroyed=false;
    bool destroy(D& d)noexcept {
        if(!material.beforeInvalidation(d))return false;
        if(!destroyed){model={};destroyed=true;paused=true;
            for(auto& pair:binders)pair={{I::none,I::none}};}
        material.invalidated();return material.release(d);
    }
};
OriginalUiRiderPodScene50KG::OriginalUiRiderPodScene50KG() noexcept=default;
OriginalUiRiderPodScene50KG::~OriginalUiRiderPodScene50KG(){
    // Never perform first fallible detach/release from a destructor.
    if(requiresModelCleanup())(void)storage_.release();
}
bool OriginalUiRiderPodScene50KG::load(std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> assets,
    OriginalUiRiderMaterialLifetime50KG& ledger,const OriginalUiRiderMaterialFootprint50KG& footprint,D& d) noexcept {
    d={};if (storage_) return fail(d,E::alreadyPrepared,0x1D3590);
    if (!assets||!assets->loaded()||!assets->hierarchy()||!assets->config()) return fail(d,E::asset,0x1D3590);
    if((assets->widgetId()!=27&&assets->widgetId()!=28)||footprint.kind()!=OriginalUiRiderMaterialOwnerKind50KG::podGraph||
       footprint.widget()!=assets->widgetId()||footprint.source()!=assets->modelSource().identity)return fail(d,E::asset,0x1D3590);
    if(!ledger.canAcquire(footprint,ledger.sequence(),d.lifetime))return fail(d,E::lifetime,0x1D3590);
    std::unique_ptr<Storage> next;
    const auto reject=[&](E error,std::uint32_t pc)noexcept {
        fail(d,error,pc);
        if(next&&!next->destroy(d))storage_=std::move(next);
        return false;
    };
    try {
        next=std::make_unique<Storage>();next->sourceAssets=std::move(assets);
        const auto& c=*next->sourceAssets->config();
        if (!next->model.build(*next->sourceAssets->hierarchy(),c.position,c.look,c.up,false)) return reject(E::graph,0x1D3678);
        if(!next->material.adopt(ledger,footprint,d))return reject(E::lifetime,0x26E7D8);
        next->rotation={c.rotate,c.velocity,c.position};next->paused=c.startPaused;
        next->binders={{{{next->model.find("binderleft1"),next->model.find("binderright1")}},
            {{next->model.find("binderleft2"),next->model.find("binderright2")}}}};
        storage_=std::move(next);return true;
    } catch (...) {return reject(E::allocation,0x1D3590);}
}
bool OriginalUiRiderPodScene50KG::modelDestroyedAfterDetach(D& d) noexcept {
    d={};return !storage_||storage_->destroy(d);
}
bool OriginalUiRiderPodScene50KG::requiresModelCleanup() const noexcept {
    return storage_&&(!storage_->destroyed||storage_->material.pending());
}
bool OriginalUiRiderPodScene50KG::reload(OriginalUiRiderMaterialLifetime50KG& ledger,
    const OriginalUiRiderMaterialFootprint50KG& footprint,D& d) noexcept {
    d={};if (!storage_||!storage_->destroyed||storage_->material.pending()) return fail(d,E::phase,0x1D3590);
    auto& s=*storage_;const auto& c=*s.sourceAssets->config();
    if((s.sourceAssets->widgetId()!=27&&s.sourceAssets->widgetId()!=28)||footprint.kind()!=OriginalUiRiderMaterialOwnerKind50KG::podGraph||
       footprint.widget()!=s.sourceAssets->widgetId()||footprint.source()!=s.sourceAssets->modelSource().identity)return fail(d,E::asset,0x1D3590);
    if(!ledger.canAcquire(footprint,ledger.sequence(),d.lifetime))return fail(d,E::lifetime,0x1D3590);
    s.destroyed=false;
    const auto reject=[&](E error,std::uint32_t pc)noexcept {fail(d,error,pc);s.destroy(d);return false;};
    if (!s.model.build(*s.sourceAssets->hierarchy(),c.position,c.look,c.up,false)) return reject(E::graph,0x1D3678);
    if(!s.material.adopt(ledger,footprint,d))return reject(E::lifetime,0x26E7D8);
    s.binders={{{{s.model.find("binderleft1"),s.model.find("binderright1")}},
        {{s.model.find("binderleft2"),s.model.find("binderright2")}}}};
    return true;
}
bool OriginalUiRiderPodScene50KG::loaded() const noexcept {return storage_&&!storage_->destroyed&&storage_->material.active();}
bool OriginalUiRiderPodScene50KG::captureDirectionalOrientation(
    OriginalUiRiderPodOrientation50KG& output,D& d) const noexcept {
    d={};if (!loaded()) return fail(d,E::phase,0x28A530);
    return captureOriginalUiRiderPodOrientation50KG(storage_->model.view(),static_cast<I>(0),output)||
        fail(d,E::graph,0x28A530);
}
bool OriginalUiRiderPodScene50KG::restoreDirectionalOrientation(
    const OriginalUiRiderPodOrientation50KG& orientation,D& d) noexcept {
    d={};if (!loaded()) return fail(d,E::phase,0x289EC0);
    return restoreOriginalUiRiderPodOrientation50KG(storage_->model.view(),static_cast<I>(0),orientation)||
        fail(d,E::pose,0x289EC0);
}
void OriginalUiRiderPodScene50KG::selected() noexcept {if (storage_) storage_->paused=false;}
void OriginalUiRiderPodScene50KG::deselected() noexcept {if (storage_) storage_->paused=true;}
bool OriginalUiRiderPodScene50KG::advance(float dt,D& d) noexcept {
    d={};if (!loaded()) return fail(d,E::phase,0x1D38B0);
    auto& s=*storage_;
    return advanceOriginalUiUserRotate50KG(s.rotation,true,s.paused,dt,s.model.view(),static_cast<I>(0))||
        fail(d,E::pose,0x1D3B0C);
}
bool OriginalUiRiderPodScene50KG::projectPalette(std::uint8_t flags,ModelPalette49S& output,D& d) noexcept {
    d={};if (!loaded()) return fail(d,E::phase,0x2412E4);return storage_->model.project(flags,output,d);
}
bool OriginalUiRiderPodScene50KG::binderPresent(std::size_t pair) const noexcept {
    return loaded()&&pair<2&&storage_->binders[pair][0]!=I::none&&storage_->binders[pair][1]!=I::none;
}
bool OriginalUiRiderPodScene50KG::projectBinder(std::size_t pair,const std::array<float,3>& camera,
    OriginalUiRiderBinderQuad50KG& output,D& d) noexcept {
    d={};if (!binderPresent(pair)) return fail(d,E::binder,0x1D3F90);
    auto& s=*storage_;return projectOriginalUiRiderBinder50KG(s.model.view(),s.binders[pair][0],
        s.binders[pair][1],camera,output)||fail(d,E::binder,0x1D3F90);
}
const assets::Scene48D* OriginalUiRiderPodScene50KG::scene() const noexcept {return storage_?storage_->sourceAssets->scene():nullptr;}
SceneGraph49P OriginalUiRiderPodScene50KG::graph() noexcept {return storage_?storage_->model.view():SceneGraph49P{nullptr,0};}
}
