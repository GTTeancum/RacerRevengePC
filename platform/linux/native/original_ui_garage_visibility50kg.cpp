#include "original_ui_garage_visibility50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "../../../src/game/original_ui_garage_model_view50kg.h"
#include "../../../src/game/camera_view49f.h"
#include "../../../src/game/original_zone_scissor50la.h"
#include <cfenv>
#include <cmath>
#include "original_ui_garage_actors50gn.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_scene_lighting50kg.h"
#include "original_ui_garage_scene_collision50kg.h"
#include "../../../src/game/original_visibility_quadtree50la.h"
#include "original_ui_garage_static_models50kg.h"
#include "original_ui_garage_selected_model50kg.h"
#include "original_ui_garage_watto_owner50kg.h"
#include "../../../src/game/original_garage_sphere_bounds50gn.h"
#include <array>
#include <cstring>
#include <limits>
#include <optional>
#include <vector>

namespace rr::platform {
namespace g=game;
namespace {
constexpr std::size_t staticCount=assets::originalUiGarageStaticModelCount50KG;
constexpr std::size_t actorCount=assets::originalUiGarageActorCount50KG;
constexpr std::size_t selectedSlot=staticCount+actorCount;
constexpr std::size_t wattoSlot=selectedSlot+1u;
constexpr std::size_t modelCount=wattoSlot+1u;
//42 real model ordinals remain unchanged. Camera43 has its own typed native
//owner/receipt; the lifecycle backing gains ONE identity, not another model.
constexpr std::size_t cameraObject=modelCount+1u;
constexpr std::size_t identityCount=modelCount+1u;
using C=g::OriginalWreckComponent50LA;
using K=g::OriginalWreckComponentKind50LA;
bool typeName(const g::OriginalEntityTypeRegistry50LA& types,std::uint8_t id,const char* name)noexcept {
    const auto* found=types.name(id);return found&&std::strncmp(found->data(),name,found->size())==0;
}
}
struct OriginalUiGarageVisibility50KG::Storage {
    // Four actual references, not aliases to one empty or populated registry.
    //27E9D0 owns a real30-element selection allocation. It is not populated by
    //cold membership and provides no evidence of27EB70 collector execution.
    struct List {
        std::uint64_t reference00=0;
        std::uint32_t selector10=0;
        std::uint8_t type2C=0xffu,type2D=0xffu;
        std::vector<const C*> selected18;
        std::uint32_t selectedCount18=0;
        std::uint64_t selectionPublication=0;
        g::SearchStorage50BS* registry=nullptr;
        // selected1CIdentity is comparison-only after its allocation is freed. It is
        // not a second selection buffer and must never be dereferenced after retirement.
        const C* const* selected1CIdentity=nullptr;
        //27E260 never initializes44/48 or flags16. Preserve that distinction:
        //2805F0's real27E850 call writes the two cursor words and bit0 only.
        std::optional<std::int32_t> iterator44,iterator48;
        std::uint8_t iteratorFlags16Value=0,iteratorFlags16Known=0;
        std::uint32_t ancestryTable08=0;
        bool sourcePayloadRetired27E2C0=false;
        
        bool sceneReference=false,zoneReference=false;
    };
    struct DefaultZoneObserver final {
        //280890 explicitly initializes00..14 and VF0 at20.18/1C are
        //unwritten source words, not invented constructor zeroes.
        const void* next00=nullptr;
        const C* child04=nullptr;
        const void* method08=nullptr;
        std::uint32_t field0C=0,field10=0,field14=0;
        std::array<std::optional<std::uint32_t>,2> untouched18To1C;
        std::array<std::uint32_t,4> vector20{{0,0,0,0x3f800000u}};
        bool sharedConstructed=false,published=false;
    };
    std::unique_ptr<DefaultZoneObserver> defaultObserver443148;
    const void* retiredDefaultObserverIdentity=nullptr;
    bool defaultObserverRetired270144=false;
    struct NodePayload {
        //Actual27F120 zero stores. Portals/exclusions remain constructor-null;
        //the real280230 callback owns a distinct typed native A0 allocation.
        const void* portalHead30=nullptr;
        std::unique_ptr<std::uint32_t[]> exclusions3C;
        std::uint32_t exclusionCount40=0;
        const void* dataA0=nullptr;
        std::shared_ptr<g::OriginalZoneScissor50LA> scissorA0;
        bool linksEmpty()const noexcept{return !portalHead30&&!exclusions3C&&!exclusionCount40;}
        bool supportedLive()const noexcept{return linksEmpty()&&dataA0==scissorA0.get();}
        bool empty()const noexcept{return !portalHead30&&!exclusions3C&&!exclusionCount40&&!dataA0;}
    };
    enum class NodeRetirement {live,lastReference,linksReleased,firstReleased,secondReleased,dataReleased,complete};
    //50CL requires the two method registries to be contiguous. They remain
    //distinct1024-word List stores, but their native allocation is released
    //after the two adjacent source List destructors, not kept as tombstones.
    struct RegistryAllocation {
        std::unique_ptr<std::array<g::SearchStorage50BS,2>> allocation;
        void allocate(){allocation=std::make_unique<std::array<g::SearchStorage50BS,2>>();}
        g::SearchStorage50BS* data()noexcept{return allocation?allocation->data():nullptr;}
        const g::SearchStorage50BS* data()const noexcept{return allocation?allocation->data():nullptr;}
        std::size_t size()const noexcept{return allocation?2u:0u;}
        g::SearchStorage50BS& operator[](std::size_t i)noexcept{return (*allocation)[i];}
        const g::SearchStorage50BS& operator[](std::size_t i)const noexcept{return (*allocation)[i];}
        void reset()noexcept{allocation.reset();}
    };
    struct Domain {
        NodePayload payload;
        std::uint32_t index48=0; //27F14C, then the real one-zone ordinal0.
        std::array<float,4> viewport4C{};
        std::array<float,16> matrix60{};
        std::uint16_t epochA4=0,epochA6=0;
        // Same-publication2805F0 continuation, separate from selected18.
        std::uint64_t finalCallbackPublication=0;
        std::uint8_t finalCallbackStep=0;

        C entity;
        std::array<List,2> lists;
        List* first34=nullptr;List* second38=nullptr;
        RegistryAllocation registries;
        std::array<g::ZoneRegistration50CJ,identityCount> registrations{};
        std::array<g::CollectionTree50BX,2> methods{};
        std::array<g::CollectionZone50BX,1> zones{};
        std::array<g::SearchListBlock50BR,4> blocks{};
        std::array<g::CollectionRef50BX,1> portalHeads{};
        g::ZoneLifecycle50CL lifecycle{};
        g::LiveZoneWorld50EN world{};
        std::uint32_t environmentNodeVisible44=0; //27F158;2741C0 produces it.
        bool entityLive=false;
    };
    struct Observer {
        std::shared_ptr<R> model;
        const OriginalUiGarageWattoOwner50KG* wattoOwner=nullptr;
        std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG> wattoAssets;
        g::CollectionRef50BX object=0;
        g::ZoneMethod50CL method=g::ZoneMethod50CL::First;
        Domain* child04=nullptr;List* method08=nullptr;
        std::array<g::CollectionZoneLink50BX,1> links{};
        std::array<g::LiveZoneEntry50EN,1> entries{};
        g::LiveZoneMembership50EN membership{};
        bool sharedConstructed=false,sourceRetired28976C=false;
        //Native lifetime only. No source04/08/19C store accompanies expiry.
        bool targetNodeExpired=false,targetListExpired=false;
    };
    struct CameraObserver {
        const void* next00=nullptr;const C* child04=nullptr;List* method08=nullptr;
        std::uint32_t zero0C=0,zero10=0,slot14=0;
        std::array<std::uint32_t,4> previousPosition20{};
        bool sharedConstructed=false,added=false,complete=false;
    };
    const OriginalUiGaragePageOwner50KG* attachedPage=nullptr;
    const OriginalUiGaragePageOwner50KG* cameraPage=nullptr;
    std::shared_ptr<g::OriginalFrontendCameraCold50LA> camera;
    std::unique_ptr<CameraObserver> cameraObserver;
    bool cameraObserverRetired=false;
    std::shared_ptr<Grant> grant;
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets;
    C scene4BC;
    //Separate source Scene pointer arrays, not aliases to inline Domains.
    std::unique_ptr<C*[]> primaryNodes4C0,secondaryNodes4C4;
    std::size_t nodePointerExtent=0;
    std::uint8_t nodePointerReleaseStage=0;
    NodePayload environmentPayload;
    std::array<NodeRetirement,3> nodeRetirement{};
    const OriginalUiGaragePageOwner50KG* pageSearchRetirementStarted=nullptr;
    const OriginalUiGaragePageOwner50KG* pageSearchRetirementOwner=nullptr;
    const OriginalUiGaragePageOwner50KG* failedPageSearchRetirementStarted=nullptr;
    const OriginalUiGaragePageOwner50KG* failedPageSearchRetirementOwner=nullptr;
    bool pageRootRetiring=false;

    C environmentNode443108;
    std::unique_ptr<g::OriginalVisibilityQuadTree50LA> environmentTree443A98;
    g::OriginalVisibilityQuadTree50LA* environmentFirst34=nullptr;
    g::OriginalVisibilityQuadTree50LA* environmentSecond38=nullptr;
    std::uint32_t environmentNode44=0,environmentNode48=0;
    bool environmentAttempted=false,environmentPrepared=false,environmentLive=false;
    std::optional<std::uint16_t> environmentEpochA4;
    bool environmentGlobalPublished=false,environmentGlobalCleared=false;
    OriginalUiGarageSelectedModel50KG* selectedOwner=nullptr;
    OriginalUiGarageStaticModels50KG* staticOwner=nullptr;
    OriginalUiGarageSceneCollision50KG* collisionOwner=nullptr;
    Domain primary4C0,secondary4C4;
    // Each source Scene pointer array is a distinct native allocation. These are
    // the constructor's original references, NOT an additional retain per array.
    std::unique_ptr<List*[]> sceneLists4C8,sceneLists4CC,sceneLists4D0,sceneLists4D4;
    bool sceneListSlotsInitialized272EC8=false;
    bool sceneListPayloadsRetired2700E8=false;
    std::array<std::unique_ptr<Observer>,modelCount> observers;
    std::array<const void*,2> retiredExternalObserverIdentity{};
    std::array<const R*,2> retiredExternalModelIdentity{};
    std::array<g::SearchStorage50BS,2> scratchRegistries{};
    std::array<g::CollectionZoneLink50BX,1> scratchLinks{};
    std::array<g::LiveZoneEntry50EN,1> scratchEntries{};
    g::LiveZoneWorkspace50EN workspace{};
    enum class SearchPhase {none,listBegin,row,clip,lod,demand,count,append,drain,environment,environmentReset,environmentQuery,environmentDrain,returned};
    SearchPhase searchPhase=SearchPhase::none;
    enum class RenderPhase {none,scissor,publish,list,modelPrepare,modelCollect,modelChildren,environment,returned};
    RenderPhase renderPhase=RenderPhase::none;
    std::uint64_t renderPublication=0;
    std::size_t renderList=0,renderRow=0;
    std::shared_ptr<R> renderModel;
    std::shared_ptr<g::OriginalZoneScissor50LA> pendingScissorA0;
    std::uint64_t searchPublication=0;
    std::size_t searchList=0,searchRow=0,searchModel=0,searchSurface=0;
    g::garage_model_view50KG::Selection searchSelection;
    g::garage_model_view50KG::Lod searchLod;
    OriginalUiGarageCameraFrame50KG::Fields searchCamera;
    std::uint32_t searchTerminalPC=0;
    bool selectedStateCurrent(const List& list,bool dying=false)const noexcept {
        if(list.selected18.empty()||list.selected18.size()%30||list.selectedCount18>list.selected18.size()||
           list.selected1CIdentity!=list.selected18.data())return false;
        if(!list.selectionPublication){
            if(list.selected18.size()!=30||list.selectedCount18)return false;
            for(const auto* row:list.selected18)if(row)return false;
            return true;
        }
        if(!searchPublication||list.selectionPublication>searchPublication)return false;
        for(std::size_t i=0;i<list.selectedCount18;++i){
            const auto* row=list.selected18[i];bool found=false;
            for(const auto& observer:observers)if(observer&&observer->model&&
                observer->model->scene().modelEntity8()==row){
                found=true;if(dying&&(!observer->sourceRetired28976C||row->reference00))return false;break;
            }
            if(!found)return false;
        }
        return true;
    }
    bool sceneLive=false,prepared=false,closing=false;
    Storage()noexcept {
        workspace.registries=scratchRegistries.data();workspace.registryCapacity=scratchRegistries.size();
        workspace.links=scratchLinks.data();workspace.entries=scratchEntries.data();workspace.membershipCapacity=1;
    }
    bool domainCurrent(const Domain& domain,const g::OriginalEntityTypeRegistry50LA& types)const noexcept {
        if(!domain.entityLive||domain.entity.kind!=K::searchNode27F120||domain.entity.reference00!=1||
           domain.entity.parent1C!=&scene4BC||domain.entity.firstChild18||
           domain.entity.sessionGeneration!=scene4BC.sessionGeneration||
           domain.entity.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&domain.entity))||
           domain.first34!=&domain.lists[0]||domain.second38!=&domain.lists[1]||
           domain.lifecycle.zoneCount!=1||domain.lifecycle.objectCount!=identityCount||domain.lifecycle.perBlock!=1024||
           domain.lifecycle.storage.registries!=domain.registries.data()||
           domain.lifecycle.storage.registrations!=domain.registrations.data()||
           domain.lifecycle.storage.views.methods!=domain.methods.data()||
           domain.lifecycle.storage.views.zones!=domain.zones.data()||
           domain.lifecycle.storage.views.blocks!=domain.blocks.data()||
           domain.world.zoneCount!=1||domain.world.portalCount||domain.world.portalLinkCount||
           domain.world.portalHeads!=domain.portalHeads.data()||domain.portalHeads[0])return false;
        for(unsigned i=0;i<2;++i){const auto& list=domain.lists[i];
            if(list.reference00!=2||!list.sceneReference||!list.zoneReference||list.selector10!=i+1u||
               list.registry!=&domain.registries[i]||!selectedStateCurrent(list)||
               !typeName(types,list.type2C,"SoultreeObject")||!typeName(types,list.type2D,"SearchableGameObject")||
               domain.methods[i].method!=g::CollectionTree50BX::Method::SearchList)return false;
        }
        return true;
    }
};
// Merge inside rr::platform AFTER Visibility::Storage and with
// original_ui_garage_scene_collision50kg.h included. Storage has one pointer:
// OriginalUiGarageSceneCollision50KG* collisionOwner=nullptr;
bool OriginalUiGarageVisibility50KG::beginCollisionPopulation2743B0(
    OriginalUiGarageSceneCollision50KG& owner,D& d)noexcept {
    d={};if(!current()||storage_->collisionOwner||!sourceSceneOwner_||
       !owner.belongsTo(*sourceSceneOwner_,storage_->grant,storage_->assets)||
       owner.visibility_.get()!=this||!owner.attempted_||owner.complete_||
       !storage_->staticOwner||owner.zone3C4_!=&storage_->staticOwner->list3C4_)
        return R::fail(d,S::phase,0x2743B0);
    for(const auto& registration:storage_->secondary4C4.registrations)
        if(registration.owner||registration.slot||registration.encodedObject)
            return R::fail(d,S::phase,0x120070);
    storage_->collisionOwner=&owner;return true;
}
bool OriginalUiGarageVisibility50KG::releaseCollisionTemporary120200(
    OriginalUiGarageSceneCollision50KG& owner,std::size_t ordinal,D& d)noexcept {
    d={};if(!storage_||storage_->collisionOwner!=&owner||ordinal>=owner.count||
       !owner.rows_[ordinal].query)return R::fail(d,S::phase,0x120200);
    auto& q=*owner.rows_[ordinal].query;
    if(!q.sharedObserver)return true;
    auto* shared=storage_->grant->observerShared();
    if(!shared||!shared->release280920(q.transientObserver50.data()))return R::fail(d,S::phase,0x280920);
    q.sharedObserver=false;return true;
}
bool OriginalUiGarageVisibility50KG::attachCollision1B58A8(
    OriginalUiGarageSceneCollision50KG& owner,std::size_t ordinal,D& d)noexcept {
    d={};if(!current()||storage_->collisionOwner!=&owner||ordinal>=owner.count||
       ordinal!=owner.completed_||!owner.belongsTo(*sourceSceneOwner_,storage_->grant,storage_->assets)||
       !owner.rows_[ordinal].query)return R::fail(d,S::phase,0x1B58A8);
    auto& q=*owner.rows_[ordinal].query;auto& s=*storage_;
    if(!q.loaded||!q.resource124||!q.resource124->live()||!q.entityLive||
       q.entity8.parent1C!=owner.zone3C4_||q.sharedObserver||q.membership.attached||
       q.shape.registrationE0||!(q.flagsF8&8u)||q.child04||q.method08)
        return R::fail(d,S::phase,0x120070);
    auto* shared=s.grant->observerShared();
    if(!shared||!shared->construct280890(q.transientObserver50.data()))
        return R::fail(d,S::allocation,0x120094);
    q.sharedObserver=true;
    //1200A8 selects this STACK object because F8.bit3 is set by11B570(13).
    // Do not publish a persistent queryE0 or count it among visual observers.
    g::LiveZoneBounds50EN bounds{};
    bounds.bounds.shapeTag=0;bounds.bounds.shapeWorld=q.shape.composed00;
    bounds.bounds.cachedCenter=q.shape.center60;bounds.bounds.cachedRadius=q.shape.radius80;
    bounds.type0Resource=&q.source;
    q.membership.storage={q.links.data(),q.entries.data(),q.links.size()};
    const auto backing=q.membership.storage;
    const auto result=g::attachLiveZoneMembership50EN(s.secondary4C4.lifecycle,s.secondary4C4.world,
        q.membership,backing,static_cast<g::CollectionRef50BX>(ordinal+1u),1,
        g::ZoneMethod50CL::First,0.f,bounds,s.workspace);
    if(result.status!=g::LiveZoneStatus50EN::Complete)return R::fail(d,S::source,0x2809C0);
    q.child04=&s.secondary4C4.entity;q.method08=&s.secondary4C4.lists[0];
    // Retain a native registry receipt, NOT the temporary source observer.
    //280920 only destroys the transient observer, not the registered list row.
    return releaseCollisionTemporary120200(owner,ordinal,d);
}
bool OriginalUiGarageVisibility50KG::collisionMembershipCurrent(
    const OriginalUiGarageSceneCollision50KG& owner,std::size_t ordinal)const noexcept {
    if(!current()||storage_->collisionOwner!=&owner||ordinal>=owner.count||
       !owner.rows_[ordinal].query)return false;
    const auto& q=*owner.rows_[ordinal].query;const auto& s=*storage_;
    const auto& registration=s.secondary4C4.registrations[ordinal];
    return !q.sharedObserver&&q.shape.registrationE0==0&&q.membership.attached&&
        q.membership.object==ordinal+1u&&q.membership.method==g::ZoneMethod50CL::First&&
        q.membership.revision==1&&q.membership.lastShapeTag==0&&
        q.membership.storage.links==q.links.data()&&q.membership.storage.entries==q.entries.data()&&
        q.membership.storage.capacity==1&&q.child04==&s.secondary4C4.entity&&
        q.method08==&s.secondary4C4.lists[0]&&registration.owner==&s.secondary4C4.registries[0]&&registration.slot&&
        registration.encodedObject==static_cast<std::uint32_t>((ordinal+1u)<<2);
}
bool OriginalUiGarageVisibility50KG::endCollisionPopulationRetirement(
    OriginalUiGarageSceneCollision50KG& owner,D& d)noexcept {
    d={};if(!storage_||!storage_->collisionOwner)return true;
    if(storage_->collisionOwner!=&owner)return R::fail(d,S::phase,0x26FF6C);
    for(const auto& row:owner.rows_)if(row.query){const auto& q=*row.query;
        if(q.entityLive||q.sharedObserver)return R::fail(d,S::phase,0x11B4A0);
        // Static collisions intentionally have no sourceE0. Their persistent
        // SearchList rows are released by the real later List destructor, not
        // by inventing an11B4A0->281490 call. Positive teardown remains closed
        // until that exact owner integrates its final allocation release.
        if(q.membership.attached)return R::fail(d,S::phase,0x26FF6C);
    }
    storage_->collisionOwner=nullptr;return true;
}

// Include INSIDE rr::platform, AFTER OriginalUiGarageVisibility50KG::Storage.
// The existing Visibility .cpp already includes the typed Page, Pod, Static,
// Collision and Camera owners. No detached cleanup method is replaced here.

bool OriginalUiGarageVisibility50KG::allocateSceneSearchListArrays272E38(D& d)noexcept {
    d={};if(!storage_||closed_||storage_->prepared||storage_->closing||
       storage_->sceneLists4C8||storage_->sceneLists4CC||storage_->sceneLists4D0||
       storage_->sceneLists4D4||storage_->sceneListSlotsInitialized272EC8||
       (sourceSceneOwner_&&sourceSceneOwner_->numZones34C_!=1))
        return R::fail(d,S::phase,0x272E38);
    auto& s=*storage_;std::uint32_t pc=0x272E3C;
    try {
        // Authenticated Garage has one zone. Native zero initialization is
        // safe backing, not source slot initialization until272EC8..272EEC.
        s.sceneLists4C8=std::make_unique<Storage::List*[]>(1); //272E44.
        pc=0x272E4C;s.sceneLists4CC=std::make_unique<Storage::List*[]>(1); //272E54.
        pc=0x272E5C;s.sceneLists4D0=std::make_unique<Storage::List*[]>(1); //272E64.
        pc=0x272E6C;s.sceneLists4D4=std::make_unique<Storage::List*[]>(1); //272E74.
    }catch(...){return R::fail(d,S::allocation,pc);}
    return true;
}

bool OriginalUiGarageVisibility50KG::sceneSearchListArraysCurrent272E38()const noexcept {
    if(!storage_||closed_)return false;
    const auto& s=*storage_;
    if(!s.sceneListSlotsInitialized272EC8||s.sceneListPayloadsRetired2700E8||
       !s.sceneLists4C8||!s.sceneLists4CC||!s.sceneLists4D0||!s.sceneLists4D4||
       s.sceneLists4C8[0]!=&s.primary4C0.lists[0]||
       s.sceneLists4CC[0]!=&s.secondary4C4.lists[0]||
       s.sceneLists4D0[0]!=&s.primary4C0.lists[1]||
       s.sceneLists4D4[0]!=&s.secondary4C4.lists[1])return false;
    const std::array<const void*,4> arrays{{s.sceneLists4C8.get(),s.sceneLists4CC.get(),
        s.sceneLists4D0.get(),s.sceneLists4D4.get()}};
    for(std::size_t i=0;i<arrays.size();++i)for(std::size_t j=0;j<i;++j)
        if(arrays[i]==arrays[j])return false;
    for(const auto* domain:{&s.primary4C0,&s.secondary4C4})for(const auto& list:domain->lists)
        if(list.sourcePayloadRetired27E2C0||list.ancestryTable08!=0x3D2D30u||
           !s.selectedStateCurrent(list))return false;
    return true;
}

bool OriginalUiGarageVisibility50KG::pageSearchReferentsDead27F180(
    const OriginalUiGaragePageOwner50KG& page,D& d)const noexcept {
    return pageSearchReferentsDeadBody27F180(page,false,d);
}
bool OriginalUiGarageVisibility50KG::pageSearchReferentsDeadBody27F180(
    const OriginalUiGaragePageOwner50KG& page,bool failedPrefix,D& d)const noexcept {
    d={};if(!storage_||closed_||!sourceSceneOwner_||
       sourceSceneOwner_->visibility_.get()!=this||page.scene4C0_!=sourceSceneOwner_||
       !storage_->grant||!storage_->grant->current()||page.grant_!=storage_->grant||
       sourceSceneOwner_->grant_!=storage_->grant||sourceSceneOwner_->assets_!=storage_->assets||
       storage_->attachedPage!=&page||
       (failedPrefix?(!page.partialSceneAttachmentFailureCurrent()||storage_->cameraPage||
          storage_->pageSearchRetirementStarted||storage_->pageSearchRetirementOwner):
          (storage_->cameraPage!=&page||storage_->failedPageSearchRetirementStarted||storage_->failedPageSearchRetirementOwner))||
       !sceneSearchListArraysCurrent272E38())return R::fail(d,S::phase,0x27F180);
    const auto& s=*storage_;const auto* models=s.staticOwner;const auto* collision=s.collisionOwner;
    auto* shared=s.grant->observerShared();
    if(!shared||!shared->coherent()||!models||sourceSceneOwner_->staticModels_.get()!=models||
       models->sceneOwner_!=sourceSceneOwner_||models->grant_!=s.grant||models->assets_!=s.assets||
       models->visibility_.get()!=this||models->pageRetirementOwner_!=&page||
       models->pageRetirement_!=OriginalUiGarageStaticModels50KG::PageRetirement::complete||
       !models->closed_||models->listLive_||models->retiring_||models->entries34_||
       models->list3C4_.reference00||models->list3C4_.firstChild18||
       !collision||sourceSceneOwner_->sceneCollision_.get()!=collision||
       collision->scene_!=sourceSceneOwner_||collision->grant_!=s.grant||
       collision->visibility_.get()!=this||collision->zone3C4_!=&models->list3C4_||
       (failedPrefix?(!models->failedPageRetirement_||!collision->failedRawListRetired27BB80(page)):
          (models->failedPageRetirement_||collision->rawListRetirementPage_!=&page||!collision->rawListRetired27BB80()))||
       !collision->pageQueriesRetired_||!collision->complete_||collision->closed_||
       !collision->visibilityBound_||collision->completed_!=staticCount)
        return R::fail(d,S::phase,0x27F180);

    const auto empty=[](const g::ZoneRegistration50CJ& row)noexcept {
        return !row.owner&&!row.slot&&!row.encodedObject;
    };
    const auto same=[](const g::ZoneRegistration50CJ& a,const g::ZoneRegistration50CJ& b)noexcept {
        return a.owner==b.owner&&a.slot==b.slot&&a.encodedObject==b.encodedObject;
    };
    // Read the canonical existing50CL owners, never a detached registry copy.
    // Every physical used slot is either on its bounded free chain or names
    // exactly one actual registration below. No unknown live referent can hide
    // behind a correct count or a valid selected-vector shape.
    for(const auto* domain:{&s.primary4C0,&s.secondary4C4}){
        const auto& life=domain->lifecycle;const auto& backing=life.storage;
        if(life.zoneCount!=1||life.objectCount!=identityCount||life.perBlock!=1024||
           backing.registries!=domain->registries.data()||backing.registryCapacity!=2||
           backing.registrations!=domain->registrations.data()||backing.registrationCapacity!=identityCount||
           backing.views.methods!=domain->methods.data()||backing.views.methodCapacity!=2||
           backing.views.zones!=domain->zones.data()||backing.views.zoneCapacity!=1||
           backing.views.blocks!=domain->blocks.data()||backing.views.blockCapacity!=4||
           domain->world.zoneCount!=1||domain->world.portalHeads!=domain->portalHeads.data()||
           domain->portalHeads[0]||domain->world.portalCount||domain->world.portalLinkCount||
           domain->zones[0].firstMethod!=1||domain->zones[0].secondMethod!=2)
            return R::fail(d,S::phase,0x27E2F0);
        for(unsigned method=0;method<2;++method){
            const auto& registry=domain->registries[method];
            const auto& list=domain->lists[method];const auto& view=domain->methods[method];
            if(list.registry!=&registry||list.selector10!=method+1u||
               registry.perBlock!=1024||registry.blockCount>1||registry.lastUsed>1024||
               (!registry.blockCount&&(registry.lastUsed!=1024||registry.freeHead))||
               view.method!=g::CollectionTree50BX::Method::SearchList||
               view.searchList.blockCount!=static_cast<std::int32_t>(registry.blockCount)||
               view.searchList.perBlock30!=1024||
               view.searchList.lastUsed38!=static_cast<std::int32_t>(registry.lastUsed))
                return R::fail(d,S::phase,0x27E2F0);
            if(!s.selectedStateCurrent(list))
                return R::fail(d,S::phase,0x27EAA4);
            //27EA70 frees the nonowning selected-pointer array, not its models.
            //Page models must already be dead. Only the exact manager-owned
            //Selected/Watto rows may outlive this primary First list; their full
            //live Observer/registration proof below still precedes every free.
            for(std::size_t selected=0;selected<list.selectedCount18;++selected){
                const auto* selectedRow=list.selected18[selected];bool lifetimeCurrent=false;
                if(!selectedRow)return R::fail(d,S::phase,0x27EAA4);
                for(std::size_t slot=0;slot<modelCount;++slot){
                    const auto& held=s.observers[slot];
                    if(!held||!held->model||held->model->scene().modelEntity8()!=selectedRow)continue;
                    if(slot<selectedSlot)
                        lifetimeCurrent=held->sourceRetired28976C&&!selectedRow->reference00;
                    else if(!failedPrefix&&domain==&s.primary4C0&&method==0&&
                            (slot==selectedSlot||slot==wattoSlot)&&externalObserverCurrent(slot,false)&&
                            selectedRow->reference00==1)
                        lifetimeCurrent=slot==selectedSlot?
                            page.selected4C4_==selectedRow:page.watto33B0_==held->wattoOwner;
                    break;
                }
                if(!lifetimeCurrent)return R::fail(d,S::phase,0x27EAA4);
            }
            if(registry.blockCount){
                const auto* blocks=domain->blocks.data()+method*2u;
                if(view.searchList.blocks!=blocks||view.searchList.blockCapacity!=2||
                   blocks[0].words!=registry.words||blocks[0].capacity!=1024||
                   blocks[1].words||blocks[1].capacity)return R::fail(d,S::phase,0x27E310);
            }else if(view.searchList.blocks||view.searchList.blockCapacity)
                return R::fail(d,S::phase,0x27E2F0);
            const std::uint32_t used=registry.blockCount?registry.lastUsed:0;
            std::array<bool,1024> freed{};auto next=registry.freeHead;
            for(std::size_t visits=0;next;++visits){
                if(visits>=used||next>used||freed[next-1u]||(registry.words[next-1u]&3u)!=2u)
                    return R::fail(d,S::phase,0x27E310);
                freed[next-1u]=true;next=registry.words[next-1u]>>2u;
            }
            for(std::uint32_t slot=1;slot<=used;++slot){
                if(freed[slot-1u])continue;
                const auto encoded=registry.words[slot-1u];const auto object=encoded>>2u;
                if(!encoded||(encoded&3u)||!object||object>identityCount)
                    return R::fail(d,S::phase,0x27E310);
                const auto& row=domain->registrations[object-1u];
                if(row.owner!=&registry||row.slot!=slot||row.encodedObject!=encoded)
                    return R::fail(d,S::phase,0x27E310);
            }
        }
        for(std::size_t i=0;i<identityCount;++i){const auto& row=domain->registrations[i];
            if(!row.owner){if(!empty(row))return R::fail(d,S::phase,0x27E310);continue;}
            if((row.owner!=&domain->registries[0]&&row.owner!=&domain->registries[1])||
               !row.slot||row.slot>row.owner->lastUsed||!row.owner->blockCount||
               row.encodedObject!=static_cast<std::uint32_t>((i+1u)<<2u)||
               row.owner->words[row.slot-1u]!=row.encodedObject)
                return R::fail(d,S::phase,0x27E310);
        }
    }

    for(std::size_t i=0;i<modelCount;++i){
        const auto& held=s.observers[i];const auto& row=s.primary4C0.registrations[i];
        // Static's genuine normal destructor retains every original row.
        // Other slots may be absent, or retain the SAME genuine normal model
        // destructor receipt when their actual Page callback is implemented.
        if(!held){
            if(i<staticCount||!empty(row)||
               (i==selectedSlot&&(page.selected4C4_||s.selectedOwner))||
               (i==wattoSlot&&s.retiredExternalObserverIdentity[1]))
                return R::fail(d,S::phase,0x28976C);
            continue;
        }
        const auto& observer=*held;
        if(i>=selectedSlot){
            if(!externalObserverCurrent(i,false)||
               (i==selectedSlot&&page.selected4C4_!=observer.model->scene_.modelEntity8())||
               (i==wattoSlot&&page.watto33B0_!=observer.wattoOwner))
                return R::fail(d,S::phase,0x27F180);
            if(observer.targetNodeExpired!=(s.nodeRetirement[0]==Storage::NodeRetirement::complete)||
               observer.targetListExpired||observer.method!=g::ZoneMethod50CL::First||
               row.owner!=&s.primary4C0.registries[0]||!row.slot||
               !observer.membership.attached||observer.membership.object!=observer.object||
               observer.membership.method!=observer.method||!observer.membership.revision||
               observer.membership.storage.links!=observer.links.data()||
               observer.membership.storage.entries!=observer.entries.data()||observer.membership.storage.capacity!=1||
               observer.links[0].next||observer.links[0].zone!=1||!observer.entries[0].used||
               observer.entries[0].portal||!same(observer.entries[0].registration,row))
                return R::fail(d,S::phase,0x27E310);
            continue;
        }
        if(!observer.model||!observer.sourceRetired28976C||observer.sharedConstructed||
           shared->owns(held.get())||observer.child04!=&s.primary4C0||
           observer.object!=i+1u||
           (observer.method!=g::ZoneMethod50CL::First&&observer.method!=g::ZoneMethod50CL::Second))
            return R::fail(d,S::phase,0x280920);
        const auto& model=*observer.model;const auto* actualEntity=model.scene_.modelEntity8();
        if(!actualEntity)return R::fail(d,S::phase,0x289778);
        const auto& entity=*actualEntity;
        const auto method=static_cast<unsigned>(observer.method);
        if(!model.closed_||model.complete_||model.resourceHeld_||
           model.pageRetirement_!=R::PageRetirement::complete||
           !model.scene_.referenceReleased_||!model.scene_.garageSceneChildrenRetired_||
           !model.scene_.released_||!model.scene_.model_.released||entity.reference00||
           entity.firstChild18||model.scene_.model_.observer19C!=held.get()||
           model.pageObserverIdentity_!=held.get()||model.generation_!=s.grant->generation_||
           model.application_!=s.grant->application()||observer.method08!=&s.primary4C0.lists[method]||
           row.owner!=&s.primary4C0.registries[method]||!row.slot||
           !observer.membership.attached||observer.membership.object!=observer.object||
           observer.membership.method!=observer.method||!observer.membership.revision||
           observer.membership.storage.links!=observer.links.data()||
           observer.membership.storage.entries!=observer.entries.data()||observer.membership.storage.capacity!=1||
           observer.links[0].next||observer.links[0].zone!=1||!observer.entries[0].used||
           observer.entries[0].portal||!same(observer.entries[0].registration,row))
            return R::fail(d,S::phase,0x289778);
        if(i<staticCount&&(models->models_[i]!=observer.model||models->enrolled_[i]||models->attached_[i]))
            return R::fail(d,S::phase,0x27B650);
    }

    using Camera=g::OriginalFrontendCameraCold50LA;
    if(failedPrefix){
        //This entry never reached114770 against Scene4BC: no Camera row or
        //Observer may be invented. The actual earlier manager Camera is dead.
        if(s.cameraPage||s.camera||s.cameraObserver||s.cameraObserverRetired||
           s.selectedOwner||page.selected4C4_||s.observers[selectedSlot]||s.observers[wattoSlot]||
           s.retiredExternalObserverIdentity[0]||s.retiredExternalObserverIdentity[1]||
           !empty(s.primary4C0.registrations[cameraObject-1u])||
           !page.failedPartialCameraRetiredCurrent())return R::fail(d,S::phase,0x1155F4);
    }else {
    if(!s.camera||s.camera!=page.sharedCamera_||!s.cameraObserverRetired||s.cameraObserver||
       s.camera->phase_!=Camera::Phase::released||s.camera->enrolled_||s.camera->entity_.reference00||
       s.camera->entity_.firstChild18||!s.camera->onAffinity()||!s.camera->actualEntitiesAlive()||
       s.camera->issuer_!=s.grant->issuer()||s.camera->entities_!=s.grant->application()||
       !s.camera->fields_.registration34||shared->owns(s.camera->fields_.registration34)||
       s.camera->fields_.parent38!=&s.primary4C0.entity)
        return R::fail(d,S::phase,0x1155F4);
    const auto& cameraRow=s.primary4C0.registrations[cameraObject-1u];
    if(cameraRow.owner!=&s.primary4C0.registries[1]||!cameraRow.slot||
       cameraRow.encodedObject!=static_cast<std::uint32_t>(cameraObject<<2u))
        return R::fail(d,S::phase,0x1155F4);
    }

    for(std::size_t i=0;i<staticCount;++i){
        const auto& actual=collision->rows_[i];const auto* q=actual.query.get();
        const auto& row=s.secondary4C4.registrations[i];
        if(!actual.attempted||!actual.completed||actual.wrapper||!actual.wrapperIdentity||!q||
           !q->retired||q->retiring||q->entityLive||q->entity8.reference00||q->entity8.firstChild18||
           q->sharedObserver||shared->owns(q->transientObserver50.data())||q->resource124||q->loading||
           q->shape8C||q->loaded||q->shape.registrationE0||q->model110)
            return R::fail(d,S::phase,0x11B558);
        for(const auto& filter:q->filters)if(filter.data||filter.count)
            return R::fail(d,S::phase,0x11B4E8);
        if(q->providerAbsent){
            if(q->membership.attached||!empty(row)||q->child04||q->method08)
                return R::fail(d,S::phase,0x1B574C);
            continue;
        }
        if(!(q->flagsF8&8u)||q->entity8.parent1C!=&models->list3C4_||
           !q->membership.attached||q->membership.object!=i+1u||
           q->membership.method!=g::ZoneMethod50CL::First||q->membership.revision!=1||
           q->membership.lastShapeTag!=0||q->membership.storage.links!=q->links.data()||
           q->membership.storage.entries!=q->entries.data()||q->membership.storage.capacity!=1||
           q->child04!=&s.secondary4C4.entity||q->method08!=&s.secondary4C4.lists[0]||
           row.owner!=&s.secondary4C4.registries[0]||!row.slot||q->links[0].next||
           q->links[0].zone!=1||!q->entries[0].used||q->entries[0].portal||
           !same(q->entries[0].registration,row))return R::fail(d,S::phase,0x11B558);
    }
    for(std::size_t i=staticCount;i<identityCount;++i)
        if(!empty(s.secondary4C4.registrations[i]))return R::fail(d,S::phase,0x27E310);
    return true;
}

bool OriginalUiGarageVisibility50KG::retirePageSceneSearchLists26FF6C(
    OriginalUiGaragePodScene50KG& scene,D& d)noexcept {
    d={};if(!storage_||sourceSceneOwner_!=&scene||scene.visibility_.get()!=this||
       scene.grant_!=storage_->grant||scene.assets_!=storage_->assets||scene.numZones34C_!=1)
        return R::fail(d,S::phase,0x26FF6C);
    auto& s=*storage_;
    // Identity precedes idempotence. Source34C remains one after these frees.
    if(s.sceneListPayloadsRetired2700E8)
        return sceneSearchListsRetired2700E8(scene)?true:R::fail(d,S::phase,0x2700E8);
    const auto* failedPage=scene.failedPageRetirementOwner_;
    const auto* sourcePage=failedPage?failedPage:s.pageSearchRetirementOwner;
    if(closed_||!scene.searchListPayloadRetirementCurrent26FF6C()||
       !sourceSearchEntitiesRetiredForPayload()||!sourcePage||sourcePage!=s.attachedPage||
       (failedPage?(s.cameraPage||s.failedPageSearchRetirementOwner!=sourcePage):sourcePage!=s.cameraPage)||
       !s.closing||s.prepared||s.sceneLive||s.scene4BC.reference00||s.scene4BC.firstChild18||
       !s.environmentGlobalCleared||s.environmentLive||s.environmentPrepared)
        return R::fail(d,S::phase,0x26FF6C);
    if(!pageSearchReferentsDeadBody27F180(*sourcePage,failedPage!=nullptr,d))return false;
    for(const auto* domain:{&s.primary4C0,&s.secondary4C4}){
        if(domain->entityLive||domain->entity.reference00||domain->entity.firstChild18||
           domain->first34!=&domain->lists[0]||domain->second38!=&domain->lists[1])
            return R::fail(d,S::phase,0x27F180);
        for(const auto& list:domain->lists)
            if(list.reference00!=1||!list.sceneReference||list.zoneReference||list.sourcePayloadRetired27E2C0)
                return R::fail(d,S::phase,0x104970);
    }
    //The exact external selected/Watto Observers remain alive. All other
    //referents are actual retired Page children, never unknown live rows.
    //This complete typed preflight precedes the first source mutation.
    // 3D2D30+10 dispatches104970; ref1->0 dispatches3D2D30+8 ==27E2C0.
    // Four different source allocations each carry their original ref, in
    // actual destructor order4C8,4D0,4CC,4D4, NOT constructor array order.
    for(auto* array:{&s.sceneLists4C8,&s.sceneLists4D0,&s.sceneLists4CC,&s.sceneLists4D4}){
        auto& list=*(*array)[0];
        --list.reference00;list.sceneReference=false; //10497C/80.
        list.ancestryTable08=0x3D2D30u; //27E2EC.
        //27E308..27E344 ends this real List's raw storage lifetime. The
        //two contiguous50CL registries share only a native allocation: the
        //next adjacent destructor cannot access this expired List, and the
        //allocation is actually freed as soon as both List lifetimes end.
        //No remove281490, row clearing, free-chain rewrite or count reset.
        if(&list==&s.primary4C0.lists[0])for(std::size_t i=selectedSlot;i<=wattoSlot;++i)
            if(s.observers[i])s.observers[i]->targetListExpired=true;
        list.ancestryTable08=0x3D2D80u; //27EA94, base selection destructor.
        std::vector<const C*>().swap(list.selected18); //27EAA4 frees actual selection.
        //Keep selected1CIdentity and registry as comparison-only stale fields.
        list.ancestryTable08=0x3CC980u; //10493C, reference-base destructor.
        list.sourcePayloadRetired27E2C0=true; //27E364/2E8B00: source object lifetime ended.
        array->reset(); //2E8AE0; ONLY4C8/4D0/4CC/4D4 source pointer is nulled.
        if(&list==&s.primary4C0.lists[1])s.primary4C0.registries.reset();
        if(&list==&s.secondary4C4.lists[1])s.secondary4C4.registries.reset();
    }
    //Queries, Page-owned models and Camera were destroyed by their actual Page
    //callbacks. Keep their native tombstone backing and stale source fields;
    //this final payload release merely ends the collision owner's native pin.
    s.collisionOwner->visibilityBound_=false;
    s.collisionOwner->complete_=false;s.collisionOwner->closed_=true;
    //Collection access ends now, but the actual global default Observer
    //remains alive until270138. This owner is not fully closed yet.
    //Workspace copies are native scratch, not retained source rows.
    s.scratchRegistries={};s.scratchLinks={};s.scratchEntries={};
    s.sceneListPayloadsRetired2700E8=true;return true;
}

bool OriginalUiGarageVisibility50KG::sceneSearchListsRetired2700E8(
    const OriginalUiGaragePodScene50KG& scene)const noexcept {
    if(!storage_||sourceSceneOwner_!=&scene||scene.visibility_.get()!=this||
       scene.grant_!=storage_->grant||scene.assets_!=storage_->assets||scene.numZones34C_!=1||
       !searchEntitiesRetired_)return false;
    const auto& s=*storage_;
    const auto* failedPage=scene.failedPageRetirementOwner_;
    const auto* sourcePage=failedPage?failedPage:s.pageSearchRetirementOwner;
    if(!s.sceneListPayloadsRetired2700E8||!s.sceneListSlotsInitialized272EC8||
       !sourcePage||sourcePage!=s.attachedPage||
       (failedPage?(s.cameraPage||s.failedPageSearchRetirementOwner!=sourcePage||
           !sourceSearchEntitiesRetiredForPayload()):sourcePage!=s.cameraPage)||
       s.sceneLive||s.scene4BC.reference00||
       s.scene4BC.firstChild18||s.sceneLists4C8||s.sceneLists4D0||s.sceneLists4CC||s.sceneLists4D4||
       s.primary4C0.registries.data()||s.secondary4C4.registries.data()||
       !s.collisionOwner||!s.collisionOwner->closed_||s.collisionOwner->complete_||
       s.collisionOwner->visibilityBound_)return false;
    for(const auto* domain:{&s.primary4C0,&s.secondary4C4}){
        if(domain->entityLive||domain->entity.reference00||domain->entity.firstChild18||
           domain->first34!=&domain->lists[0]||domain->second38!=&domain->lists[1])return false;
        for(const auto& list:domain->lists)
            if(list.reference00||list.zoneReference||list.sceneReference||
               !list.sourcePayloadRetired27E2C0||list.ancestryTable08!=0x3CC980u||
               !list.selected18.empty()||!list.selected1CIdentity)return false;
    }
    return true;
}


//Actual24186C tail. Source Scene2893A0 rows are a separate graphical
//forest, not invented Entity children. Walk the real Entity links/masks and
//stop at a genuinely reached renderer+20 callback if one is introduced.
bool OriginalUiGarageVisibility50KG::finishModelChildren24186C(const R& model,D& d)const noexcept {
    const auto* root=model.scene().modelEntity8();
    if(!root||!model.current())return R::fail(d,S::phase,0x24186cu);
    if(!(root->descendants24&8u)){d.sourcePC=0x141e00u;return true;}
    const C* previous=nullptr;std::size_t seen=0;
    for(auto* child=root->firstChild18;child;child=child->next14){
        if(++seen>model.scene().sourceRowCount()||child->parent1C!=root||child->previous10!=previous||
           child->sessionGeneration!=root->sessionGeneration||!child->reference00)
            return R::fail(d,S::phase,0x141d98u);
        bool actual=false;
        for(std::uint32_t row=0;row<model.scene().sourceRowCount();++row){
            const auto* source=model.scene().sourceRow(row);
            if(source&&(&source->entity()==child)){actual=true;break;}
        }
        if(!actual)return R::fail(d,S::phase,0x141d98u);
        if((child->flags29&1u)&&!(child->flags29&8u)&&(child->mask20&8u))
            return R::fail(d,S::phase,0x141dd8u); //Actual renderer+20, never an omitted draw.
        previous=child;
    }
    d.sourcePC=0x141e00u;return true;
}
bool OriginalUiGarageVisibility50KG::renderCameraSearch114E00(
    OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageCameraFrame50KG& frame,D& d)noexcept {
    d={};OriginalUiGarageCameraFrame50KG::ModelRenderInputs inputs;
    std::uint64_t publication=0;std::uint32_t pending=0;
    if(!frame.renderInvocation114E00Current(page,*this)||
       !frame.readRenderInputs114E00(inputs,publication,pending)||!current())
        return R::fail(d,S::phase,pending?pending:0x114e00u);
    auto& s=*storage_;auto& domain=s.primary4C0;const auto app=s.grant->application();using Phase=Storage::RenderPhase;
    if(!app||!publication||s.attachedPage!=&page||s.cameraPage!=&page||
       s.searchPublication!=publication||s.searchPhase!=Storage::SearchPhase::returned||
       domain.epochA4!=app->portalAdmission8BE0_||!domain.payload.supportedLive()||
       domain.world.portalCount||domain.world.portalLinkCount||domain.portalHeads[0]||
       app->diagnostic14_||app->portalDrawDepth443118_||app->portalDrawDebug443110_)
        return R::fail(d,S::phase,0x27fee0u);
    if(s.renderPublication!=publication){
        if((s.renderPhase!=Phase::none&&s.renderPhase!=Phase::returned)||app->portalDepth443120_)
            return R::fail(d,S::phase,0x27fef8u);
        //27FEF8..27FF30. Preserve these writes on any later native failure.
        ++app->portalDepth443120_;
        app->portalDispatch8BE4_=static_cast<std::uint16_t>(app->portalDispatch8BE4_+1u);
        domain.epochA6=app->portalDispatch8BE4_;
        s.renderPublication=publication;s.renderPhase=Phase::scissor;s.renderList=s.renderRow=0;
    }
    if(s.renderPhase==Phase::returned){d.sourcePC=0x28022cu;return true;}
    if(app->portalDepth443120_!=1u||domain.epochA6!=app->portalDispatch8BE4_)
        return R::fail(d,S::phase,0x27ff30u);
    if(s.renderPhase==Phase::scissor){
        if(std::fegetround()!=FE_TONEAREST||!inputs.viewportWidth1E8||!inputs.viewportHeight1EC||
           inputs.viewportX1E0>2047u||inputs.viewportY1E4>2047u||
           inputs.viewportWidth1E8>2047u||inputs.viewportHeight1EC>2047u)
            return R::fail(d,S::arithmetic,0x28036cu);
        try {
            if(!domain.payload.scissorA0&&!s.pendingScissorA0){
                auto owner=std::shared_ptr<g::OriginalZoneScissor50LA>(new g::OriginalZoneScissor50LA(domain.index48));
                owner->garageOwner_=this;s.pendingScissorA0=std::move(owner);
            }
        }catch(...){return R::fail(d,S::allocation,0x280250u);}
        auto& out=*(domain.payload.scissorA0?domain.payload.scissorA0:s.pendingScissorA0);
        if(out.garageOwner_!=this||out.garageRetired_||out.revision_==std::numeric_limits<std::uint64_t>::max())
            return R::fail(d,S::phase,0x28036cu);
        std::array<std::uint32_t,4> coordinates;
        for(unsigned i=0;i<4;++i){
            const auto b=domain.viewport4C[i<2?i:i==2?3:2];
            if(!std::isfinite(b))return R::fail(d,S::arithmetic,0x28036cu);
            volatile float shifted=i<2?1.f+b:1.f-b;
            volatile float half=shifted/2.f;
            volatile float extent=half*static_cast<float>(i<2?inputs.viewportWidth1E8:inputs.viewportHeight1EC);
            volatile float pixel=extent+static_cast<float>(i<2?inputs.viewportX1E0:inputs.viewportY1E4);
            //Actual CVT.W.S under nearest mode, not truncation. The bounded
            //native target excludes packed11-bit overflow, not required draws.
            const float rounded=std::nearbyint(pixel);
            if(!std::isfinite(rounded)||rounded<0.f||rounded>2047.f)
                return R::fail(d,S::arithmetic,0x2805b4u);
            coordinates[i]=static_cast<std::uint32_t>(rounded);
        }
        out.endpoints_=coordinates;++out.revision_;
        if(!domain.payload.scissorA0)domain.payload.scissorA0=std::move(s.pendingScissorA0);
        domain.payload.dataA0=&out; //27FF40, after genuine280230 return.
        s.renderPhase=Phase::publish;
    }
    if(s.renderPhase==Phase::publish){
        //The actual portal30 chain is empty, so280118 is now reached. This
        //calls the same real FIFO4014 producer, not a second scissor slot.
        if(!frame.publishModelScissor280118(*this,domain.payload.scissorA0,d))return false;
        s.renderPhase=Phase::list;
    }
    for(;;){
        if(s.renderPhase==Phase::environment)break;
        if(s.renderPhase==Phase::list){
            if(s.renderList>=2u){s.renderPhase=Phase::environment;break;}
            auto& list=domain.lists[s.renderList];
            if(!s.selectedStateCurrent(list)||list.selectionPublication!=publication)
                return R::fail(d,S::phase,0x27eca0u);
            if(s.renderRow>=list.selectedCount18){++s.renderList;s.renderRow=0;continue;}
            s.renderPhase=Phase::modelPrepare;
        }
        if(s.renderPhase==Phase::modelPrepare){
            bool selected=false;
            if(!prepareSelectedModelPackets241280(page,frame,s.renderList,s.renderRow,s.renderModel,selected,d))return false;
            if(!selected){s.renderModel.reset();++s.renderRow;s.renderPhase=Phase::list;continue;}
            s.renderPhase=Phase::modelCollect;
        }
        if(s.renderPhase==Phase::modelCollect){
            if(!s.renderModel)return R::fail(d,S::phase,0x2416a4u);
            if(!frame.collectGarageModel241280(*s.renderModel,d))return false;
            s.renderPhase=Phase::modelChildren;
        }
        if(s.renderPhase==Phase::modelChildren){
            if(!s.renderModel||!finishModelChildren24186C(*s.renderModel,d))return false;
            s.renderModel.reset();++s.renderRow;s.renderPhase=Phase::list;
        }
    }
    if(s.renderPhase==Phase::environment){
        if(s.environmentEpochA4&&*s.environmentEpochA4==app->portalAdmission8BE0_){
            //2B00D0 ->27ECA0. Only the actual previous query's zero selected18
            //branch is supported; its allocated buffer and live owners persist.
            if(!s.environmentTree443A98||!s.environmentTree443A98->emptyGarageQueryStateCurrent()||
               s.environmentTree443A98->selectedCount18_)
                return R::fail(d,S::phase,0x2b00d0u);
        }
        --app->portalDepth443120_;s.renderPhase=Phase::returned;d.sourcePC=0x28022cu;return true;
    }
    return R::fail(d,S::phase,0x27fee0u);
}

bool OriginalUiGarageVisibility50KG::skipFireObserver15E1B4(
    const OriginalUiGaragePodScene50KG& scene,const R& model,std::uint32_t sourceRow,
    const g::OriginalRacerSceneObject50LA& object,bool& skipped,D& d)const noexcept {
    d={};skipped=false;
    if(!current()||sourceSceneOwner_!=&scene||scene.visibility_.get()!=this||
       !storage_->attachedPage||!scene.actorOwner3C8_||!model.current()||
       model.application_!=storage_->grant->application()||model.generation_!=storage_->grant->generation_||
       model.scene().sourceRowObject(sourceRow)!=&object||!object.constructed||object.released)
        return R::fail(d,S::phase,0x15E198);
    const auto& s=*storage_;const auto app=s.grant->application();
    //This producer admits only genuine retained actor attachments. Authenticate
    //their owning model without using its root observer as the child's field.
    const Storage::Observer* held=nullptr;
    for(std::size_t i=0;i<actorCount;++i){const auto& candidate=s.observers[staticCount+i];
        if(candidate&&candidate->model.get()==&model){
            if(!pageActorObserverCurrent(*s.attachedPage,*scene.actorOwner3C8_,i,model))
                return R::fail(d,S::phase,0x15E198);
            held=candidate.get();break;
        }
    }
    if(!held)return R::fail(d,S::phase,0x15E198);
    const auto* actual=object.observer19C; //15E198: real field, every visit.
    if(!actual)return true; //Separate pd_fire Plane01 really has null19C.
    if(actual!=held||model.scene().model()!=&object||held->targetNodeExpired||held->targetListExpired||
       held->child04!=&s.primary4C0||!s.primary4C0.entityLive||
       held->method08!=&s.primary4C0.lists[1]||held->links[0].zone!=1u)
        return R::fail(d,S::phase,0x15E1A4);
    //Both are the original unsigned16-bit fields. Same owner-thread visit,
    //no observer mutation between the source reload and comparison/return.
    skipped=held->child04->epochA4!=app->portalAdmission8BE0_;
    return true;
}

bool OriginalUiGarageVisibility50KG::modelLightZone14A110(
    const OriginalUiLightManagerOwner50KG& manager,const R& model,std::uint32_t& zone)const noexcept {
    if(!current()||!storage_||!sourceSceneOwner_||!sourceSceneOwner_->sceneLighting_||
       sourceSceneOwner_->sceneLighting_->manager_.get()!=&manager||!model.current())return false;
    const auto& s=*storage_;
    for(const auto& item:s.observers){
        if(!item||item->model.get()!=&model)continue;
        const auto& observer=*item;
        if(!observer.sharedConstructed||observer.sourceRetired28976C||observer.targetNodeExpired||
           observer.targetListExpired||observer.child04!=&s.primary4C0||
           model.scene().model()->observer19C!=&observer||!observer.membership.attached||
           observer.links[0].zone!=1u||!s.primary4C0.entityLive)return false;
        zone=observer.child04->index48; //Actual retained27F14C/2729E0 row, not a caller override.
        return true;
    }
    return false;
}
bool OriginalUiGarageVisibility50KG::prepareSelectedModelPackets241280(
    OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageCameraFrame50KG& frame,
    std::size_t sourceList,std::size_t sourceRow,std::shared_ptr<R>& actualModel,bool& selected,D& d)noexcept {
    d={};actualModel.reset();selected=false;
    OriginalUiGarageCameraFrame50KG::ModelRenderInputs inputs;std::uint64_t publication=0;std::uint32_t pending=0;
    if(!frame.renderInvocation114E00Current(page,*this)||
       !frame.readRenderInputs114E00(inputs,publication,pending)||!storage_||!current())
        return R::fail(d,S::phase,pending?pending:0x114e00u);
    auto& s=*storage_;
    if(s.attachedPage!=&page||s.cameraPage!=&page||s.searchPublication!=publication||
       s.searchPhase!=Storage::SearchPhase::returned||sourceList>=2u)
        return R::fail(d,S::phase,0x27eca0u);
    auto& list=s.primary4C0.lists[sourceList];
    if(!s.selectedStateCurrent(list)||list.selectionPublication!=publication||
       sourceRow>=list.selectedCount18)return R::fail(d,S::phase,0x27ecd0u);
    const auto* entity=list.selected18[sourceRow];
    for(const auto& observer:s.observers){
        if(observer&&observer->model&&observer->model->scene().modelEntity8()==entity){
            if(observer->sourceRetired28976C||!observer->sharedConstructed||!observer->model->current())
                return R::fail(d,S::phase,0x2412acu);
            actualModel=observer->model;break;
        }
    }
    if(!actualModel)return R::fail(d,S::phase,0x2412acu);
    auto& model=*actualModel;
    if(!(model.scene_.model_.flags191&1u)){d.sourcePC=0x241878u;return true;}
    if(model.nativeGarageFifoPins_)return R::fail(d,S::phase,0x241280u);
    game::original_racer_lighting50KM::LightMatrices lights;const auto* lightInput=&lights;
    if(model.scene_.fields().flags1B3&1u){
        auto* lighting=sourceSceneOwner_?sourceSceneOwner_->sceneLighting_.get():nullptr;
        if(!lighting||!lighting->manager_)return R::fail(d,S::phase,0x14a0a0u);
        OriginalUiLightManagerOwner50KG::Diagnostic ld;
        if(!lighting->manager_->selectGarageModel14A0A0(*this,model,lights,ld))return R::fail(d,S::phase,ld.sourcePC);
    }else lightInput=nullptr;
    const auto result=model.packets_.prepareGaragePackets241280(model.graph(),lightInput,
        inputs.fogStartB0,inputs.secondMapDistance382FA0);
    if(!result.okay())return R::fail(d,S::phase,result.sourcePC);
    selected=true;d.sourcePC=result.sourcePC;return true;
}

OriginalUiGarageVisibility50KG::OriginalUiGarageVisibility50KG()noexcept=default;
OriginalUiGarageVisibility50KG::~OriginalUiGarageVisibility50KG(){
    if(storage_){
        if(!closed_)std::terminate();
        for(const auto& observer:storage_->observers)
            if(observer&&observer->sharedConstructed)std::terminate();
    }
}
game::OriginalWreckComponent50LA* OriginalUiGarageVisibility50KG::pageEntity4BC()noexcept {
    return current()&&!storage_->attachedPage?&storage_->scene4BC:nullptr;
}
bool OriginalUiGarageVisibility50KG::attachPage4BC1B48D4(
    OriginalUiGaragePageOwner50KG& page,OriginalUiGaragePodScene50KG& scene,D& d)noexcept {
    d={};if(!current()||storage_->attachedPage||sourceSceneOwner_!=&scene||
       page.scene4C0_!=&scene||page.scenePageAttachCursor_||!page.scenePageBorrowCaptured_||
       page.scenePageChildren_[0]!=&storage_->scene4BC)return R::fail(d,S::phase,0x1B48D4);
    const auto result=storage_->grant->attachGarageScenePageChild1B48A0(page,scene,0,storage_->scene4BC,0xffffffffu);
    if(!result.okay())return R::fail(d,S::phase,result.sourcePC);
    storage_->attachedPage=&page;return true;
}
bool OriginalUiGarageVisibility50KG::registerPageCamera1FB608(
    OriginalUiGaragePageOwner50KG& page,std::shared_ptr<g::OriginalFrontendCameraCold50LA> camera,D& d)noexcept {
    using Camera=g::OriginalFrontendCameraCold50LA;
    d={};if(!storage_||closed_||!sourceSceneOwner_||!page.sceneAttachmentPrefixCurrent1B48A0()||
       page.scene4C0_!=sourceSceneOwner_||sourceSceneOwner_->visibility_.get()!=this||
       !camera||page.sharedCamera_!=camera||page.cameraHandoffPhase_!=1||
       !camera->nativeRegistrationIdentityCurrent()||camera->issuer_!=storage_->grant->issuer()||
       camera->entities_!=storage_->grant->application()||camera->phase_!=Camera::Phase::poseSet114F90)
        return R::fail(d,S::phase,0x1FB608);
    auto& s=*storage_;auto& domain=s.primary4C0;
    if(s.cameraObserverRetired||(s.camera&&s.camera!=camera)||
       (s.cameraPage&&s.cameraPage!=&page)||!domain.entityLive||
       domain.second38!=&domain.lists[1]||domain.world.portalCount||domain.world.portalLinkCount||
       domain.portalHeads[0])return R::fail(d,S::phase,0x2809C0);
    if(s.cameraObserver&&s.cameraObserver->complete)return pageCameraRegistrationCurrent(page,*camera);
    auto* shared=s.grant->observerShared();if(!shared)return R::fail(d,S::phase,0x280890);
    if(!s.cameraObserver){
        if(camera->fields_.registration34)return R::fail(d,S::phase,0x1147A8);
        try{s.cameraObserver=std::make_unique<Storage::CameraObserver>();}
        catch(...){return R::fail(d,S::allocation,0x114790);}
        s.camera=std::move(camera);s.cameraPage=&page;
    }
    auto& o=*s.cameraObserver;
    if(!o.sharedConstructed){
        if(s.camera->fields_.registration34||o.added||o.child04||o.method08||o.slot14)
            return R::fail(d,S::phase,0x280890);
        if(!shared->construct280890(&o))return R::fail(d,S::allocation,0x280890);
        o.sharedConstructed=true;s.camera->fields_.registration34=&o; //1147A8.
    }
    if(!shared->owns(&o)||s.camera->fields_.registration34!=&o||o.next00||o.zero0C||o.zero10)
        return R::fail(d,S::phase,0x114818);
    //1148D0 left the old manager38 unchanged. This source store legitimately
    //replaces it with the actual primary zone, before2809C0.
    s.camera->fields_.parent38=&domain.entity;
    if(!o.added){
        auto& row=domain.registrations[cameraObject-1u];
        if(row.owner||row.slot||row.encodedObject)return R::fail(d,S::phase,0x280A2C);
        if(g::updateZoneRegistration50CL(domain.lifecycle,cameraObject,1,g::ZoneMethod50CL::Second,
           s.workspace.transfer)!=g::SearchStorageStatus50BS::Complete)return R::fail(d,S::source,0x280A2C);
        o.child04=&domain.entity;o.method08=domain.second38;o.slot14=row.slot;
        o.previousPosition20=s.camera->fields_.position180;o.added=true;
    }
    //No portals are authored and the actual zone's link head is null. The
    //source280BF0 continuation therefore allocates no secondary observers.
    o.complete=true;
    if(!pageCameraRegistrationCurrent(page,*s.camera))return R::fail(d,S::phase,0x1148B4);
    return true;
}
bool OriginalUiGarageVisibility50KG::pageCameraRegistrationCurrent(
    const OriginalUiGaragePageOwner50KG& page,const g::OriginalFrontendCameraCold50LA& camera)const noexcept {
    using Camera=g::OriginalFrontendCameraCold50LA;
    if(!storage_||closed_||storage_->cameraPage!=&page||storage_->camera.get()!=&camera||
       storage_->cameraObserverRetired||!storage_->cameraObserver||
       !camera.nativeRegistrationIdentityCurrent()||camera.issuer_!=storage_->grant->issuer()||
       camera.entities_!=storage_->grant->application()||
       (camera.phase_!=Camera::Phase::poseSet114F90&&camera.phase_!=Camera::Phase::pageAttached1FB620))return false;
    const auto& s=*storage_;const auto& o=*s.cameraObserver;const auto& domain=s.primary4C0;
    const auto& row=domain.registrations[cameraObject-1u];auto* shared=s.grant->observerShared();
    return shared&&shared->owns(&o)&&o.sharedConstructed&&o.added&&o.complete&&!o.next00&&
        !o.zero0C&&!o.zero10&&o.child04==&domain.entity&&o.method08==domain.second38&&
        o.method08==&domain.lists[1]&&o.slot14&&o.slot14==row.slot&&
        row.owner==&domain.registries[1]&&row.encodedObject==(cameraObject<<2)&&
        row.slot<=1024&&row.owner->words[row.slot-1]==row.encodedObject&&
        camera.fields_.registration34==&o&&camera.fields_.parent38==&domain.entity;
}
bool OriginalUiGarageVisibility50KG::updatePageCameraObserver114C44(
    const OriginalUiGaragePageOwner50KG& page,g::OriginalFrontendCameraCold50LA& camera,D& d)noexcept {
    d={};if(!pageCameraRegistrationCurrent(page,camera)||storage_->attachedPage!=&page||
       !sourceSceneOwner_||page.scene4C0_!=sourceSceneOwner_||sourceSceneOwner_->visibility_.get()!=this||
       !page.updatePublication_||page.updateFailed_||page.updateReturned_||
       page.updateChild_!=&camera.entity_||page.updatedPageChildren_!=11||
       camera.updatePublication_!=page.updatePublication_||camera.updateSourcePC_!=0x114C44||
       camera.updateFailed_||camera.updateReturned_||!camera.fields_.guardProjection290Known)
        return R::fail(d,S::phase,0x114C44);
    auto& s=*storage_;auto& domain=s.primary4C0;auto& observer=*s.cameraObserver;
    if(!domain.entityLive||domain.world.zoneCount!=1||domain.world.portalCount||
       domain.world.portalLinkCount||domain.portalHeads[0]||observer.next00||
       domain.second38!=&domain.lists[1]||domain.lists[1].ancestryTable08!=0x3D2D30u)
        return R::fail(d,S::source,0x280D88);
    //280D94->281380 uses the real SearchList SECOND row.27E3B0/27E3C0
    //returns its nonnull previous handle;27E9C0 is a literal no-op. Neither
    //the zero radius nor3E0060 extent is read on this no-portal same-row path.
    //Do not insert/remove a row, rebuild an observer, or clear source34/38.
    auto& row=domain.registrations[cameraObject-1u];std::uint32_t selected=0;
    if(row.owner!=&domain.registries[1]||
       g::selectSearchSlot50BS(*row.owner,observer.slot14,selected)!=g::SearchStorageStatus50BS::Complete||
       selected!=observer.slot14||selected!=row.slot)
        return R::fail(d,S::source,0x2813C8);
    observer.slot14=selected; //281460, same live slot.
    observer.previousPosition20=camera.fields_.position180; //280FE8 after281380.
    camera.fields_.parent38=observer.child04; //114C54 after the actual update.
    return true;
}
bool OriginalUiGarageVisibility50KG::retirePageCameraObserver1155F4(
    const OriginalUiGaragePageOwner50KG& page,g::OriginalFrontendCameraCold50LA& camera,D& d)noexcept {
    d={};if(!storage_||closed_||storage_->cameraPage!=&page||storage_->camera.get()!=&camera||
       camera.phase_!=g::OriginalFrontendCameraCold50LA::Phase::pageRetiring1155C0||
       !camera.onAffinity()||!camera.actualEntitiesAlive()||camera.enrolled_||camera.entity_.reference00)
        return R::fail(d,S::phase,0x1155F4);
    auto& s=*storage_;if(s.cameraObserverRetired)return true;
    if(!s.cameraObserver)return R::fail(d,S::phase,0x1155E8);
    auto& o=*s.cameraObserver;const auto& row=s.primary4C0.registrations[cameraObject-1u];
    auto* shared=s.grant->observerShared();
    if(!shared||!shared->owns(&o)||!o.sharedConstructed||!o.added||!o.complete||
       camera.fields_.registration34!=&o||camera.fields_.parent38!=&s.primary4C0.entity||
       o.method08!=s.primary4C0.second38||row.owner!=&s.primary4C0.registries[1]||
       row.slot!=o.slot14||row.encodedObject!=(cameraObject<<2)||!row.slot||row.slot>1024||
       row.owner->words[row.slot-1]!=row.encodedObject)return R::fail(d,S::phase,0x1155F4);
    if(!shared->release280920(&o))return R::fail(d,S::phase,0x280920);
    //1155C0 does NOT call281490, and does NOT clear Camera34/38. Keep those
    //raw stale source observations and the exact real list row. Native Camera
    //backing remains pinned by this owner until the genuine List final free.
    s.cameraObserver.reset();s.cameraObserverRetired=true;return true;
}
bool OriginalUiGarageVisibility50KG::prepareCameraSearch114D28(
    OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageCameraFrame50KG& frame,D& d)noexcept {
    namespace view=g::garage_model_view50KG;
    d={};std::uint64_t publication=0;
    if(!frame.searchInvocation114D28Current(page,*this)||!frame.readSearchPublication114D28(publication)||
       !publication||!current()||storage_->attachedPage!=&page||!storage_->camera||
       !pageCameraRegistrationCurrent(page,*storage_->camera))return R::fail(d,S::phase,0x114D28);
    auto& s=*storage_;auto& domain=s.primary4C0;auto* shared=s.grant->observerShared();
    const auto app=s.grant->application();
    if(!app||!shared||!shared->coherent()||!app->portalEnabled36ADA8_||app->diagnostic14_||
       !domain.payload.supportedLive()||domain.world.zoneCount!=1||
       domain.world.portalCount||domain.world.portalLinkCount||domain.portalHeads[0]||
       domain.first34!=&domain.lists[0]||domain.second38!=&domain.lists[1])return R::fail(d,S::source,0x27F650);
    if(s.searchTerminalPC)return R::fail(d,S::demandTerminal,s.searchTerminalPC);
    if(s.searchPublication!=publication){
        if(app->portalDepth443120_||(s.renderPhase!=Storage::RenderPhase::none&&s.renderPhase!=Storage::RenderPhase::returned)||
           (s.searchPhase!=Storage::SearchPhase::none&&s.searchPhase!=Storage::SearchPhase::returned))
            return R::fail(d,S::phase,0x114D28);
        OriginalUiGarageCameraFrame50KG::Fields camera;std::uint32_t pc=0;
        if(!frame.read(camera,pc))return R::fail(d,S::phase,pc?pc:0x114D28);
        g::CameraViewOutput49F currentCamera;currentCamera.view=camera.viewC0;
        currentCamera.sceneAfter.fovDegrees=camera.fov1D0;
        currentCamera.sceneAfter.aspect1F8=camera.aspect1F8;
        currentCamera.sceneAfter.nearPlane=camera.near1FC;
        currentCamera.sceneAfter.farPlane=camera.far200;
        g::PortalZoneViewOutput49F numeric;
        const std::array<float,4> bounds{{-1.f,1.f,-1.f,1.f}};
        if(!g::composePortalZoneView280720(currentCamera,bounds,numeric))return R::fail(d,S::arithmetic,0x280720);
        //The real source27F650 prefix. Native numeric admission precedes stores;
        //after this point a failed continuation keeps this same invocation.
        app->portalAdmission8BE0_=static_cast<std::uint16_t>(app->portalAdmission8BE0_+1u);
        domain.epochA4=app->portalAdmission8BE0_;domain.viewport4C=bounds;
        app->portalDebug443100_=0; //27F7BC, same process owner used by RA.
        domain.matrix60=numeric.matrix60; //280700, after the reached empty portal loop.
        app->portalDispatch8BE4_=static_cast<std::uint16_t>(app->portalDispatch8BE4_+1u);
        domain.epochA6=app->portalDispatch8BE4_;
        s.searchCamera=camera;s.searchPublication=publication;s.searchList=0;
        s.searchPhase=Storage::SearchPhase::listBegin;
    }
    if(s.searchPhase==Storage::SearchPhase::returned)return true;
    while(s.searchList<2){
        auto& list=domain.lists[s.searchList];auto& registry=domain.registries[s.searchList];
        if(list.registry!=&registry||registry.perBlock!=1024||registry.blockCount>1||
           registry.lastUsed>1024||list.ancestryTable08!=0x3D2D30u)return R::fail(d,S::source,0x27E880);
        if(s.searchPhase==Storage::SearchPhase::listBegin){
            //A true source3C-null list skips27EAF0 altogether. Not an invented
            //empty collector: absence must match the actual retained registry.
            if(!registry.blockCount){
                if(registry.lastUsed!=1024||registry.freeHead)return R::fail(d,S::source,0x27E898);
                ++s.searchList;continue;
            }
            list.selectedCount18=0;list.selectionPublication=publication; //27EB38 ->27F050.
            s.searchRow=0;s.searchPhase=Storage::SearchPhase::row;
        }
        while(s.searchRow<registry.lastUsed){
            const auto encoded=registry.words[s.searchRow];
            if(s.searchPhase==Storage::SearchPhase::row){
                if(encoded&2u){++s.searchRow;continue;} //Actual dead/free source row.
                const auto object=encoded>>2u;
                if(!object||object>identityCount)return R::fail(d,S::source,0x27EB58);
                const auto& registration=domain.registrations[object-1u];
                if(registration.owner!=&registry||registration.slot!=s.searchRow+1u||
                   registration.encodedObject!=encoded)return R::fail(d,S::source,0x27EB48);
                if(object==cameraObject){
                    const auto& fields=s.camera->fields_;
                    if((fields.flags33&1u)&&!(fields.flags33&4u)&&
                       (fields.type32==list.type2C||fields.type32==list.type2D))
                        return R::fail(d,S::phase,0x27EC18); //No recursive invented Camera callback.
                    ++s.searchRow;continue;
                }
                s.searchModel=object-1u;
                const auto& held=s.observers[s.searchModel];
                if(!held||!held->model||!held->model->current()||held->sourceRetired28976C||
                   held->child04!=&domain||held->method08!=&list||held->object!=object||
                   !shared->owns(held.get())||held->model->scene_.model_.observer19C!=held.get())
                    return R::fail(d,S::phase,0x27EB80);
                auto& model=*held->model;auto& body=model.scene_.model_;
                const auto* entity=model.scene_.modelEntity8();
                if(body.type02!=list.type2C||!entity||entity->reference00!=1)
                    return R::fail(d,S::source,0x27EB80);
                //27EB60/78/98 skip these rows BEFORE26EFC0, preserving stale
                //flags191 as the original does. No counter or selected append.
                if(!(body.flags03&1u)||(body.flags03&4u)||!(entity->flags29&1u)){
                    ++s.searchRow;continue;
                }
                auto graph=model.graph();
                if(!g::refreshSceneWorld49P(graph,body.sceneIndex))return R::fail(d,S::phase,0x2897B0);
                s.searchPhase=Storage::SearchPhase::clip;
            }
            auto& model=*s.observers[s.searchModel]->model;auto& body=model.scene_.model_;
            auto& fields=model.scene_.fields_;const auto graph=model.graph();
            const auto index=static_cast<std::uint32_t>(body.sceneIndex);
            if(index>=graph.count)return R::fail(d,S::source,0x240C00);
            if(s.searchPhase==Storage::SearchPhase::clip){
                g::OriginalRacerAggregateBounds50LA bounds;
                bounds.center160=body.bounds160;bounds.halfExtent170=body.bounds170;
                //240C00 consumes these two vectors, not optional radius198.
                const g::OriginalUiRiderVisibilityPolicy50KG policy{(app->useMembership3833D8_!=0),(app->guardInside4354F0_!=0)};
                if(!view::select240C00(graph.nodes[index].world100,bounds,domain.matrix60,
                    s.searchCamera.combined100,s.searchCamera.guardCombined2D0,policy,
                    body.flags03,body.flags191,fields.flags1B2,s.searchSelection))return R::fail(d,S::arithmetic,0x240C00);
                fields.flags1B2=s.searchSelection.flags1B2;body.flags191=s.searchSelection.flags191;
                s.searchPhase=s.searchSelection.selected?Storage::SearchPhase::lod:Storage::SearchPhase::count;
            }
            if(s.searchPhase==Storage::SearchPhase::lod){
                const auto* geometry=model.geometry();const auto* payload=geometry?geometry->payload():nullptr;
                const auto* source=payload?payload->scene():nullptr;const auto* scale=geometry?geometry->scale14():nullptr;
                const auto radius=body.nodeCount1A0==1?body.radius194:body.radius198;
                if(!source||!scale||!radius||!geometry->holdsModelReference(&model.scene_))return R::fail(d,S::phase,0x26F090);
                view::LodInput input;input.actualCenter=body.nodeCount1A0==1?body.bounds140:body.bounds160;
                for(unsigned axis=0;axis<3;++axis){input.actualLocalF0[axis]=graph.nodes[index].localC0.v[12+axis];input.actualCamera180[axis]=s.searchCamera.position180[axis];}
                input.actualRadius=*radius;input.actualFocal1D8=s.searchCamera.focal1D8;
                input.actualGeometryScale14=*scale;input.actualLodScale383360=app->modelLod383360_;
                input.actualRadiusScale383368=app->projectedRadius383368_;
                if(!view::selectLod26F070(*source,input,s.searchLod))return R::fail(d,S::arithmetic,0x26F070);
                fields.distance1BC=s.searchLod.distance1BC;fields.field1B0=s.searchLod.selected1B0;
                s.searchSurface=0;s.searchPhase=Storage::SearchPhase::demand;
            }
            if(s.searchPhase==Storage::SearchPhase::demand){
                const auto* payload=model.geometry()->payload();const auto* source=payload->scene();
                const auto* table=model.materials_.table382B38();
                if(!table||!source||fields.field1B0>=source->lods.size()||!fields.materials1C4||
                   fields.materialCount1C0<0)return R::fail(d,S::phase,0x26F2A0);
                const auto extent=source->lods[fields.field1B0].surfaces.size();
                if(extent>static_cast<std::size_t>(fields.materialCount1C0))return R::fail(d,S::source,0x26F2AC);
                while(s.searchSurface<extent){
                    if(!table->demandSurface246810(*payload,fields.field1B0,s.searchSurface,
                        fields.materials1C4[s.searchSurface],s.searchLod.projectedRadiusF21,s.searchLod.scaleF20,d)){
                        if(d.sourceEffectsCommitted)s.searchTerminalPC=d.sourcePC?d.sourcePC:0x26F2B8;
                        return false;
                    }
                    ++s.searchSurface;
                }
                s.searchPhase=Storage::SearchPhase::count;
            }
            if(s.searchPhase==Storage::SearchPhase::count){
                if(!frame.incrementModelCount26F050())return R::fail(d,S::phase,0x26F050);
                s.searchPhase=Storage::SearchPhase::append;
            }
            if(s.searchPhase==Storage::SearchPhase::append){
                if(s.searchSelection.selected){
                    if(list.selectedCount18==list.selected18.size()){
                        try{list.selected18.resize(list.selected18.size()+30,nullptr);}
                        catch(...){return R::fail(d,S::allocation,0x27EEA0);}
                        list.selected1CIdentity=list.selected18.data();
                    }
                    list.selected18[list.selectedCount18++]=model.scene_.modelEntity8();
                }
                ++s.searchRow;s.searchPhase=Storage::SearchPhase::row;
            }
        }
        s.searchPhase=Storage::SearchPhase::drain;
        VehicleWindowDiagnostic50DS renderer;std::uint32_t pending=0;
        if(!frame.drainSearchBlock27EC68(renderer,pending))return R::fail(d,S::video,pending?pending:0x27EC68);
        ++s.searchList;s.searchPhase=Storage::SearchPhase::listBegin;
    }
    if(s.searchPhase==Storage::SearchPhase::listBegin)s.searchPhase=Storage::SearchPhase::environment;
    if(domain.environmentNodeVisible44){
        auto* tree=s.environmentTree443A98.get();const auto app=s.grant->application();
        if(!environmentCurrent272FC0()||!tree||!app||app->environment443108_!=&s.environmentNode443108)
            return R::fail(d,S::phase,0x27F8C8);
        if(s.searchPhase==Storage::SearchPhase::environment){
            //A fresh native node has no prior visit. We retain that fact rather
            //than inventing zero for source27F120's unwritten A4 allocation bytes.
            //Once stamped, the source16-bit equality and wrap are retained.
            if(s.environmentEpochA4&&*s.environmentEpochA4==domain.epochA4){
                s.searchPhase=Storage::SearchPhase::returned;return true;
            }
            s.environmentEpochA4=domain.epochA4; //27F8F0, before tree's actual+44.
            s.searchPhase=Storage::SearchPhase::environmentReset;
        }
        if(s.searchPhase==Storage::SearchPhase::environmentReset){
            if(!app->treeSearchFilterInitialized3D6698_){
                app->treeSearchFilter443AA8_=-1;app->treeSearchFilterInitialized3D6698_=true;
            }
            std::uint32_t pc=0;
            if(!tree->reset265C60(pc))return R::fail(d,S::phase,pc);
            s.searchPhase=Storage::SearchPhase::environmentQuery;
        }
        if(s.searchPhase==Storage::SearchPhase::environmentQuery){
            std::uint32_t pc=0;
            if(!tree->queryGarageEmpty2AF1D0(s.searchCamera.position180[0],s.searchCamera.position180[2],publication,pc))
                return R::fail(d,S::phase,pc);
            //2B0154 reaches27EAF0 even for actual count0. Its real selected18
            //clear and renderer block drain are not skipped by empty geometry.
            tree->selectedCount18_=0;s.searchPhase=Storage::SearchPhase::environmentDrain;
        }
        if(s.searchPhase==Storage::SearchPhase::environmentDrain){
            VehicleWindowDiagnostic50DS renderer;std::uint32_t pending=0;
            if(!frame.drainSearchBlock27EC68(renderer,pending))return R::fail(d,S::video,pending?pending:0x27EC68);
        }
    }
    s.searchPhase=Storage::SearchPhase::returned;d.status=S::complete;return true;
}

// SearchNode3D2DD0+4C. This callback closes the actual SearchList iterator,
// not its selected array. Source280670 deliberately reloads FIRST34 for the
// distinct-second branch; do not "fix" that into a second38 callback.
bool OriginalUiGarageVisibility50KG::finishSearchNode2805F0(
    OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageCameraFrame50KG& frame,
    C& node,D& d)noexcept {
    d={};std::uint64_t publication=0;
    if(!frame.readFinalSearchInvocation2805F0(page,*this,node,publication)||
       !publication||!current()||!storage_||storage_->attachedPage!=&page)
        return R::fail(d,S::phase,0x2805F0);
    auto& s=*storage_;
    Storage::Domain* domain=nullptr;
    if(&node==&s.primary4C0.entity)domain=&s.primary4C0;
    else if(&node==&s.secondary4C4.entity)domain=&s.secondary4C4;
    if(!domain||!domain->entityLive||node.parent1C!=&s.scene4BC||
       domain->first34!=&domain->lists[0]||domain->second38!=&domain->lists[1])
        return R::fail(d,S::phase,0x280600);
    // A positive source30 chain recursively visits nodes whose A4 matches
    // Application8BE0. This authored Scene has real null portal ownership;
    // neither a guessed empty list nor a missing epoch producer is accepted.
    if(domain->payload.portalHead30||domain->world.portalCount||
       domain->world.portalLinkCount||domain->portalHeads[0])
        return R::fail(d,S::source,0x28060C);
    if(domain->finalCallbackPublication!=publication){
        if(domain->finalCallbackStep&&domain->finalCallbackStep!=3)
            return R::fail(d,S::phase,0x2805F0);
        domain->finalCallbackPublication=publication;domain->finalCallbackStep=1;
    }
    const auto closeFirst27E850=[&]()noexcept {
        auto* first=domain->first34;
        if(!first)return true; //Actual28064C test, never manufactured.
        if(first!=&domain->lists[0]||first->ancestryTable08!=0x3D2D30u||
           !first->reference00||first->sourcePayloadRetired27E2C0||
           first->registry!=&domain->registries[0]||!s.selectedStateCurrent(*first))
            return false;
        first->iterator44=0;first->iterator48=0; //27E850/54.
        first->iteratorFlags16Value&=0xfeu;
        first->iteratorFlags16Known|=1u; //Only bit0 is written/known here.
        return true;
    };
    if(domain->finalCallbackStep==1){
        if(!closeFirst27E850())return R::fail(d,S::phase,0x28065C);
        domain->finalCallbackStep=2;d.sourceEffectsCommitted=true;
    }
    if(domain->finalCallbackStep==2){
        if(domain->second38&&domain->second38!=domain->first34){
            //Exact280670/67C: the original receiver is first34 AGAIN.
            if(!domain->first34||!closeFirst27E850())return R::fail(d,S::phase,0x280684);
            d.sourceEffectsCommitted=true;
        }
        domain->finalCallbackStep=3;
    }
    if(domain->finalCallbackStep!=3)return R::fail(d,S::phase,0x28068C);
    d.sourcePC=0x28069C;return true;
}

bool OriginalUiGarageVisibility50KG::current()const noexcept {
    if(!storage_||closed_||!sceneSearchListArraysCurrent272E38())return false;
    const auto& s=*storage_;
    if(sourceSceneOwner_&&!defaultZoneObserverCurrent272F90())return false;
    if(s.nodePointerExtent!=1||!s.primaryNodes4C0||!s.secondaryNodes4C4||s.nodePointerReleaseStage||
       s.primaryNodes4C0[0]!=&s.primary4C0.entity||s.secondaryNodes4C4[0]!=&s.secondary4C4.entity||
       !s.prepared||s.closing||!s.grant||!s.grant->current()||!s.assets||!s.assets->loaded()||!s.sceneLive||
       s.scene4BC.reference00!=1||s.scene4BC.kind!=K::entity141300||
       (!s.attachedPage&&(s.scene4BC.parent1C||s.scene4BC.previous10||s.scene4BC.next14))||
       (s.attachedPage&&(!sourceSceneOwner_||!s.grant->garageScenePageChildCurrent(
           *s.attachedPage,*sourceSceneOwner_,0,s.scene4BC)))||
       s.scene4BC.sessionGeneration!=s.grant->generation_||
       s.scene4BC.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&s.scene4BC))||
       s.scene4BC.firstChild18!=&s.primary4C0.entity||
       s.primary4C0.entity.previous10||s.primary4C0.entity.next14!=&s.secondary4C4.entity||
       s.secondary4C4.entity.previous10!=&s.primary4C0.entity||
       s.secondary4C4.entity.next14!=(s.environmentLive?&s.environmentNode443108:nullptr)||
       s.primary4C0.entity.mask20!=0x400u||s.secondary4C4.entity.mask20)return false;
    if(s.environmentAttempted&&!environmentCurrent272FC0())return false;
    const auto app=s.grant->application();
    return app&&s.domainCurrent(s.primary4C0,app->types383408())&&s.domainCurrent(s.secondary4C4,app->types383408());
}
bool OriginalUiGarageVisibility50KG::constructDefaultZoneObserver272F44(D& d)noexcept {
    d={};if(!storage_||!sourceSceneOwner_||closed_||!storage_->grant||!storage_->grant->current()||
       sourceSceneOwner_->visibility_.get()!=this||sourceSceneOwner_->grant_!=storage_->grant||
       sourceSceneOwner_->useEnvironment35C_!=0||!storage_->sceneLive||storage_->prepared||
       !storage_->sceneLists4C8||!sceneSearchListArraysCurrent272E38()||
       !storage_->secondaryNodes4C4||storage_->secondaryNodes4C4[0]!=&storage_->secondary4C4.entity||
       storage_->secondary4C4.entity.reference00!=1||storage_->defaultObserver443148||
       storage_->defaultObserverRetired270144)return R::fail(d,S::phase,0x272F44);
    auto& s=*storage_;const auto app=s.grant->application();auto* shared=s.grant->observerShared();
    //Actual fresh Application globals, not asset emptiness. A foreign retained
    //tree/default observer is unsupported, never overwritten or guessed absent.
    if(!app||!shared||app->defaultZoneObserver443148_||app->visibility443A98_||
       app->environment443108_||s.environmentAttempted)return R::fail(d,S::phase,0x272F28);
    try{s.defaultObserver443148=std::make_unique<Storage::DefaultZoneObserver>();}
    catch(...){return R::fail(d,S::allocation,0x272F44);}
    auto& observer=*s.defaultObserver443148;
    if(!shared->construct280890(&observer))return R::fail(d,S::allocation,0x272F58);
    observer.sharedConstructed=true;
    app->defaultZoneObserver443148_=&observer;observer.published=true; //272F64.
    observer.child04=s.secondaryNodes4C4[0]; //272F80; no registration or retain.
    observer.method08=app->visibility443A98_; //272F90, actual NULL captured once.
    return defaultZoneObserverCurrent272F90();
}
bool OriginalUiGarageVisibility50KG::defaultZoneObserverCurrent272F90()const noexcept {
    if(!storage_||!sourceSceneOwner_||closed_||!storage_->grant||!storage_->grant->current())return false;
    const auto& s=*storage_;const auto* o=s.defaultObserver443148.get();
    const auto app=s.grant->application();const auto* shared=s.grant->observerShared();
    return app&&shared&&o&&!s.defaultObserverRetired270144&&!s.retiredDefaultObserverIdentity&&
        o->sharedConstructed&&o->published&&shared->owns(o)&&app->defaultZoneObserver443148_==o&&
        !o->next00&&o->child04==&s.secondary4C4.entity&&!o->method08&&
        !o->field0C&&!o->field10&&!o->field14&&!o->untouched18To1C[0]&&!o->untouched18To1C[1]&&
        o->vector20==std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}};
}
bool OriginalUiGarageVisibility50KG::retireDefaultZoneObserver270138(
    OriginalUiGaragePodScene50KG& scene,D& d)noexcept {
    d={};if(!storage_||sourceSceneOwner_!=&scene||scene.visibility_.get()!=this||
       !scene.scenePayloadRetirementCurrent()||
       scene.retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::lightManagerZonesReleased||
       !sceneSearchListsRetired2700E8(scene))return R::fail(d,S::phase,0x270138);
    auto& s=*storage_;const auto app=s.grant->application();auto* shared=s.grant->observerShared();
    auto* const o=s.defaultObserver443148.get();
    //Nodes/tree are already dead. Compare their retained identities only;
    //280920 neither traverses04/08 nor unregisters a row nor releases a tree.
    if(!app||!shared||!shared->coherent()||!o||s.defaultObserverRetired270144||
       !o->sharedConstructed||!o->published||app->defaultZoneObserver443148_!=o||!shared->owns(o)||
       o->next00||o->child04!=&s.secondary4C4.entity||o->method08||o->field0C||o->field10||o->field14||
       o->untouched18To1C[0]||o->untouched18To1C[1]||
       o->vector20!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})
        return R::fail(d,S::phase,0x280920);
    if(!shared->release280920(o))return R::fail(d,S::phase,0x280920);
    o->sharedConstructed=false;s.retiredDefaultObserverIdentity=o;
    s.defaultObserver443148.reset(); //Actual280920(a1=1) allocation release.
    app->defaultZoneObserver443148_=nullptr; //270144, AFTER actual release.
    s.defaultObserverRetired270144=true;closed_=true;return true;
}
bool OriginalUiGarageVisibility50KG::defaultZoneObserverRetired270144(
    const OriginalUiGaragePodScene50KG& scene)const noexcept {
    if(!storage_||sourceSceneOwner_!=&scene||scene.visibility_.get()!=this||
       !sceneSearchListsRetired2700E8(scene))return false;
    const auto& s=*storage_;const auto app=s.grant?s.grant->application():nullptr;
    const auto* shared=s.grant?s.grant->observerShared():nullptr;
    return app&&shared&&s.defaultObserverRetired270144&&s.retiredDefaultObserverIdentity&&
        !s.defaultObserver443148&&!app->defaultZoneObserver443148_&&
        !shared->owns(s.retiredDefaultObserverIdentity);
}
bool OriginalUiGarageVisibility50KG::environmentCurrent272FC0()const noexcept {
    if(!storage_)return false;
    const auto& s=*storage_;
    const auto app=s.grant?s.grant->application():nullptr;
    const auto* tree=s.environmentTree443A98.get();const auto& node=s.environmentNode443108;
    return app&&s.environmentPrepared&&s.environmentLive&&s.environmentGlobalPublished&&
        !s.environmentGlobalCleared&&tree&&tree->application_==app.get()&&tree->configured_&&
        tree->reference00_==2&&tree->published_&&app->visibility443A98_==tree&&
        app->environment443108_==&node&&s.environmentFirst34==tree&&s.environmentSecond38==tree&&
        tree->requestedCapacity8C0_==5000&&node.kind==K::searchNode27F120&&node.reference00==1&&
        node.sessionGeneration==s.scene4BC.sessionGeneration&&node.objectKey==
        static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&node))&&
        node.parent1C==&s.scene4BC&&node.previous10==&s.secondary4C4.entity&&!node.next14&&
        !node.firstChild18&&!node.mask20&&!node.descendants24&&!s.environmentNode44&&!s.environmentNode48;
}
bool OriginalUiGarageVisibility50KG::prepareEnvironment272FC0(
    const g::original_world_environment50KG::SpatialBounds& bounds,D& d)noexcept {
    d={};if(!current()||!sourceSceneOwner_||storage_->environmentAttempted)
        return R::fail(d,S::phase,0x272FC0);
    auto& s=*storage_;const auto app=s.grant->application();
    // Actual global identity decides the source branch. A foreign retained
    // tree/node is not an empty-world substitute or an adoptable proof token.
    if(!app||app->visibility443A98_||app->environment443108_)
        return R::fail(d,S::phase,0x272FD4);
    s.environmentAttempted=true;
    try {s.environmentTree443A98.reset(new g::OriginalVisibilityQuadTree50LA);}
    catch(...){return R::fail(d,S::allocation,0x272FE4);}
    auto& tree=*s.environmentTree443A98;std::uint32_t pc=0;
    if(!tree.construct(*app,bounds,pc))return R::fail(d,S::source,pc);
    const auto step=s.grant->constructPrefix(s.environmentNode443108,K::searchNode27F120);
    if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
    s.environmentLive=true;
    app->environment443108_=&s.environmentNode443108;s.environmentGlobalPublished=true; //2730F8
    s.environmentNode48=0; //273104, separately reached after27F120.
    const auto attached=s.grant->attach(s.scene4BC,s.environmentNode443108,0);
    if(!attached.okay())return R::fail(d,S::phase,attached.sourcePC);
    const auto bound=s.grant->bindGarageEnvironment2730F8(*this,*sourceSceneOwner_,s.environmentNode443108);
    if(!bound.okay())return R::fail(d,S::phase,bound.sourcePC);
    //27F420 and27F490 both retain the SAME real collection owner. These
    //references are additional to, never aliases replacing, the four Lists.
    s.environmentFirst34=&tree;++tree.reference00_;
    s.environmentSecond38=&tree;++tree.reference00_;
    --tree.reference00_; //27315C drops the constructor's local reference.
    s.environmentPrepared=true;return environmentCurrent272FC0();
}
bool OriginalUiGarageVisibility50KG::clearEnvironmentGlobal26FF54(D& d)noexcept {
    d={};if(!storage_)return true;auto& s=*storage_;
    if(s.environmentGlobalCleared)return true;
    if(!searchEntitiesRetired_||s.environmentLive||s.environmentFirst34||s.environmentSecond38||
       (s.environmentTree443A98&&s.environmentTree443A98->reference00_))
        return R::fail(d,S::phase,0x26FF54);
    const auto app=s.grant?s.grant->application():nullptr;
    if(!app)return R::fail(d,S::phase,0x26FF54);
    if(s.environmentGlobalPublished){
        if(app->environment443108_!=&s.environmentNode443108)return R::fail(d,S::phase,0x26FF54);
        app->environment443108_=nullptr;
    }
    s.environmentGlobalCleared=true;return true;
}
bool OriginalUiGarageVisibility50KG::nativeTerminalRenderQuiescent(
    const OriginalUiGaragePageOwner50KG& page,std::uint64_t publication)const noexcept {
    if(!publication||!current()||!storage_||!sourceSceneOwner_||page.scene4C0_!=sourceSceneOwner_||
       storage_->attachedPage!=&page||storage_->cameraPage!=&page||!storage_->camera||
       !pageCameraRegistrationCurrent(page,*storage_->camera))return false;
    const auto& s=*storage_;const auto app=s.grant->application();
    if(!app||app->portalDepth443120_||app->portalDrawDepth443118_||s.renderModel||s.pendingScissorA0||
       s.searchTerminalPC||
       (s.searchPhase!=Storage::SearchPhase::none&&s.searchPhase!=Storage::SearchPhase::returned)||
       (s.renderPhase!=Storage::RenderPhase::none&&s.renderPhase!=Storage::RenderPhase::returned)||
       s.searchPublication>publication||s.renderPublication>publication||
       !s.environmentTree443A98||!s.environmentTree443A98->emptyGarageQueryStateCurrent())return false;
    for(const auto* domain:{&s.primary4C0,&s.secondary4C4}){
        if(!domain->payload.supportedLive()||
           (domain->finalCallbackStep&&domain->finalCallbackStep!=3))return false;
        for(const auto& list:domain->lists){
            if(!s.selectedStateCurrent(list))return false;
            // Actual Search destruction precedes these two external model
            // destructors. Its existing dying-selected-array proof requires
            // ref0 for every selected row; do not clear or reinterpret one.
            for(std::size_t i=0;i<list.selectedCount18;++i)
                for(std::size_t external=selectedSlot;external<modelCount;++external){
                    const auto& held=s.observers[external];
                    if(held&&held->model&&held->model->scene_.modelEntity8()==list.selected18[i])return false;
                }
        }
    }
    return true;
}
bool OriginalUiGarageVisibility50KG::matchesColdConstruction(
    const std::shared_ptr<Grant>& grant,
    const std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>& assets)const noexcept {
    if(!current()||!grant||!assets||storage_->searchPublication)return false;
    const auto& s=*storage_;
    return s.grant.get()==grant.get()&&!s.grant.owner_before(grant)&&!grant.owner_before(s.grant)&&
        s.assets.get()==assets.get()&&!s.assets.owner_before(assets)&&!assets.owner_before(s.assets);
}
bool OriginalUiGarageVisibility50KG::matchesRetainedSceneOwnership(
    const std::shared_ptr<Grant>& grant,
    const std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>& assets,
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGaragePodScene50KG& scene)const noexcept {
    // Only a retained identity observation. Caller-specific Camera, timing,
    // Search/render phases and completion predicates remain at their callers.
    // In particular a real nonzero Search publication is not cold corruption.
    if(!current()||!grant||!assets||sourceSceneOwner_!=&scene||storage_->attachedPage!=&page)
        return false;
    const auto& s=*storage_;
    return s.grant.get()==grant.get()&&!s.grant.owner_before(grant)&&!grant.owner_before(s.grant)&&
        s.assets.get()==assets.get()&&!s.assets.owner_before(assets)&&!assets.owner_before(s.assets);
}
bool OriginalUiGarageVisibility50KG::prepare2729E0(std::shared_ptr<Grant> grant,
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> assets,D& d)noexcept {
    d={};if(attempted_||closed_)return R::fail(d,S::alreadyPrepared,0x2729E0);
    if(!grant||!grant->current()||!assets||!assets->loaded()||!assets->sceneDat()||!assets->archive()||
       assets->archive()->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::garage))
        return R::fail(d,S::input,0x2729E0);
    const auto* search=assets->sceneDat()->find("StaticModels","SearchType");
    if(!search||search->text!="List")return R::fail(d,S::source,0x275F74);
    // The authenticated Garage has the single implicit StaticModels zone and
    // no authored portal section. Do not replace a general world with this.
    if(assets->sceneDat()->section("Zones")||assets->sceneDat()->section("Portals"))
        return R::fail(d,S::source,0x272B3C);
    attempted_=true;
    try {storage_=std::make_unique<Storage>();}catch(...){return R::fail(d,S::allocation,0x2729FC);}
    auto& s=*storage_;s.grant=std::move(grant);s.assets=std::move(assets);
    auto step=s.grant->constructPrefix(s.scene4BC,K::entity141300);
    if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
    s.sceneLive=true;
    s.nodePointerExtent=1; //Authentic implicit Garage zone count34C.
    try {s.primaryNodes4C0=std::make_unique<C*[]>(s.nodePointerExtent);}
    catch(...){return R::fail(d,S::allocation,0x272A24);}
    try {s.secondaryNodes4C4=std::make_unique<C*[]>(s.nodePointerExtent);}
    catch(...){return R::fail(d,S::allocation,0x272A34);}
    for(auto* domain:{&s.primary4C0,&s.secondary4C4}){
        step=s.grant->constructPrefix(domain->entity,K::searchNode27F120);
        if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
        domain->entityLive=true;
        if(domain==&s.primary4C0)s.primaryNodes4C0[0]=&domain->entity; //272A8C.
        else s.secondaryNodes4C4[0]=&domain->entity; //272AC0.
    }
    //272E3C..272E74 allocates four distinct original List-pointer arrays.
    if(!allocateSceneSearchListArrays272E38(d))return false;
    // Source272E94/272EB0, with genuine retained Entity linkage and masks.
    step=s.grant->attach(s.scene4BC,s.primary4C0.entity,0x400u);
    if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
    step=s.grant->attach(s.scene4BC,s.secondary4C4.entity,0u);
    if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
    if(sourceSceneOwner_){
        step=s.grant->bindGarageVisibility272E94(*this,*sourceSceneOwner_,s.scene4BC,
            s.primary4C0.entity,s.secondary4C4.entity);
        if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
    }
    //272EC8/D4/E0/EC: actual one-zone slot initialization, after attach.
    s.sceneLists4C8[0]=nullptr;s.sceneLists4CC[0]=nullptr;
    s.sceneLists4D0[0]=nullptr;s.sceneLists4D4[0]=nullptr;
    s.sceneListSlotsInitialized272EC8=true;
    for(auto* domain:{&s.primary4C0,&s.secondary4C4}){
        try {domain->registries.allocate();}catch(...){return R::fail(d,S::allocation,0x27E260);}
        const g::ZoneLifecycleStorage50CL backing{domain->registries.data(),domain->registries.size(),
            domain->registrations.data(),domain->registrations.size(),
            {domain->methods.data(),domain->methods.size(),domain->zones.data(),domain->zones.size(),
             domain->blocks.data(),domain->blocks.size()}};
        if(g::initializeZoneLifecycle50CL(domain->lifecycle,backing,1,identityCount,1024,s.workspace.transfer)!=g::SearchStorageStatus50BS::Complete)
            return R::fail(d,S::phase,0x27E260);
        const g::LiveZoneWorldStorage50EN portalStorage{domain->portalHeads.data(),1,nullptr,0,nullptr,0};
        if(g::prepareLiveZoneWorld50EN(nullptr,0,1,portalStorage,domain->world)!=g::LiveZoneStatus50EN::Complete)
            return R::fail(d,S::phase,0x272B3C);
    }
    //2731D8 primary34,273400 secondary34,273668 primary38,273890 secondary38.
    for(unsigned method=0;method<2;++method)for(auto* domain:{&s.primary4C0,&s.secondary4C4}){
        auto& list=domain->lists[method];
        list.reference00=1;list.sceneReference=true; //1048F0, not Entity141300.
        try{list.selected18.resize(30,nullptr);}catch(...){return R::fail(d,S::allocation,0x27ED40);}
        auto app=s.grant->application();if(!app)return R::fail(d,S::phase,0x27EA20);
        auto id=app->types383408().findOrAppend2A31E0("SoultreeObject");
        if(!id.okay())return R::fail(d,S::source,id.sourcePC);
        list.type2C=id.type;
        id=app->types383408().findOrAppend2A31E0("SearchableGameObject");
        if(!id.okay())return R::fail(d,S::source,id.sourcePC);
        list.type2D=id.type;list.registry=&domain->registries[method];
        list.selector10=method+1u;
        list.selected1CIdentity=list.selected18.data();
        list.ancestryTable08=0x3D2D30u; //27E284, actual SearchList dispatch.
        auto& sourceArray=method?
            (domain==&s.primary4C0?s.sceneLists4D0:s.sceneLists4D4):
            (domain==&s.primary4C0?s.sceneLists4C8:s.sceneLists4CC);
        sourceArray[0]=&list; //2731EC/273414/27367C/2738A4; no extra retain.
        (method?domain->second38:domain->first34)=&list;
        ++list.reference00;list.zoneReference=true; //27F420/27F490 ->104960.
    }
    //Authored live Info35C is0.272F28 skips272FC0 here; only later particle
    //1B4E20 creates the environment tree. The default observer captures the
    //actual global NOW and must not be patched when that later tree appears.
    if(sourceSceneOwner_&&!constructDefaultZoneObserver272F44(d))return false;
    s.prepared=true;return current()?true:R::fail(d,S::phase,0x272F90);
}
const g::OriginalWreckComponent50LA* OriginalUiGarageVisibility50KG::primaryZone4C0()const noexcept {
    return current()?&storage_->primary4C0.entity:nullptr;
}
const g::OriginalWreckComponent50LA* OriginalUiGarageVisibility50KG::secondaryZone4C4()const noexcept {
    return current()?&storage_->secondary4C4.entity:nullptr;
}
bool OriginalUiGarageVisibility50KG::modelMatches(const R& resource)const noexcept {
    if(!current()||!resource.current()||resource.generation_!=storage_->grant->generation_||
       resource.application_!=storage_->grant->application())return false;
    const auto app=storage_->grant->application();const auto& scene=resource.scene_;
    const auto* model=scene.model();const auto* entity=scene.modelEntity8();
    return model&&entity&&entity->reference00==1&&entity->sessionGeneration==resource.generation_&&
        scene.binding_.actualObjectCount423F60==&app->objectCount423F60()&&
        scene.binding_.actualAttachmentLock423F58==&app->attachmentLock423F58()&&
        scene.binding_.actualTypes383408==&app->types383408();
}
bool OriginalUiGarageVisibility50KG::makeBounds28E230(R& resource,g::LiveZoneBounds50EN& out,D& d)noexcept {
    if(!modelMatches(resource))return R::fail(d,S::phase,0x28E230);
    auto& scene=resource.scene_;auto& model=scene.model_;const auto graph=resource.graph();
    const auto index=static_cast<std::int32_t>(model.sceneIndex);
    if(!g::validateSceneGraph49P(graph)||index<0||static_cast<std::uint32_t>(index)>=graph.count)
        return R::fail(d,S::source,0x28E230);
    auto& root=graph.nodes[index];const auto flags=root.sceneFlags190;
    const bool recompute=(flags&0x80u)&&((flags&8u)||!(flags&0x40u));
    if(flags&0x10u){
        if(resource.mode_!=R::RootMode::separate||!scene.rowsLoaded_||scene.referenceReleased_||
           !scene.rowTable_||scene.fields_.sourceRows1B8!=scene.rowTable_.get()||
           !model.nodeCount1A0||model.nodeCount1A0!=scene.binding_.sourceRowCount||
           model.nodeCount1A0>g::maxModelBoundsNodes49O)return R::fail(d,S::phase,0x26F4C0);
        if(recompute){
            std::array<g::OriginalGarageSphereRow50GN,g::maxModelBoundsNodes49O> rows{};
            const g::OriginalGarageSphereRow50GN actualRoot{model.sceneIndex,model.bounds140,model.radius194};
            for(std::uint32_t i=0;i<model.nodeCount1A0;++i){
                const auto* row=scene.sourceRow(i);
                if(!row||scene.fields_.sourceRows1B8[i]!=row||!row->constructed||row->released)
                    return R::fail(d,S::phase,0x26F560);
                rows[i]={row->sceneIndex,row->bounds140,row->radius194};
            }
            auto* shared=storage_->grant->sphereShared();
            if(!shared)return R::fail(d,S::phase,0x26F4D4);
            g::OriginalGarageSphereResult50GN result;g::OriginalGarageSphereDiagnostic50GN diagnostic;
            if(!g::computeOriginalGarageSphereBounds26F4B050GN(graph,actualRoot,rows.data(),model.nodeCount1A0,
                *shared,result,diagnostic))return R::fail(d,S::source,diagnostic.sourcePC);
            model.bounds160=result.center160;model.radius198=result.radius198;
            for(unsigned axis=0;axis<3;++axis)model.bounds170[axis]=result.radius198;
            root.sceneFlags190=result.rootSceneFlags190; // Preserve170.W and80.
        }
    }else {
        const auto result=resource.mode_==R::RootMode::direct?scene.publishDirectRootBounds28E110(graph):
            scene.publishAggregate28E110(graph);
        if(!result.okay())return R::fail(d,S::source,result.sourcePC);
    }
    //28E388 independently performs the actual lazy-world request even when
    //the earlier80 gate skipped it. No bind-pose world replaces sampled locals.
    if(!model.radius198||!g::refreshSceneWorld49P(graph,model.sceneIndex))
        return R::fail(d,S::source,0x28E398);
    out={};out.bounds.shapeTag=1;out.bounds.shapeWorld=root.world100;
    std::memcpy(out.bounds.localCenter.v,model.bounds160.data(),sizeof out.bounds.localCenter.v);
    std::memcpy(out.bounds.localHalf.v,model.bounds170.data(),sizeof out.bounds.localHalf.v);
    out.bounds.cachedRadius=*model.radius198;
    g::CollectionBoundsResult48S bounds;
    if(g::prepareCollectionBounds48S(out.bounds,bounds)!=g::CollectionBoundsStatus48S::Complete)
        return R::fail(d,S::source,0x28E410);
    out.bounds.cachedCenter=bounds.center;return true;
}
bool OriginalUiGarageVisibility50KG::attach(std::size_t slot,std::shared_ptr<R> resource,
    g::ZoneMethod50CL method,D& d)noexcept {
    d={};if(slot>=modelCount||!resource||!modelMatches(*resource)||
       (method!=g::ZoneMethod50CL::First&&method!=g::ZoneMethod50CL::Second))
        return R::fail(d,S::input,0x28E8A0,"garage.model.observer.inputs");
    auto& s=*storage_;
    if(s.observers[slot]||resource->scene_.model_.observer19C)return R::fail(d,S::phase,0x28E8B8,"garage.model.observer.alreadyOwned");
    for(const auto& observer:s.observers)if(observer&&observer->model.get()==resource.get())
        return R::fail(d,S::phase,0x28E8B8,"garage.model.observer.duplicateModel");
    auto* shared=s.grant->observerShared();if(!shared)return R::fail(d,S::phase,0x280890,"garage.model.observer.sharedOwner");
    try {
        auto next=std::make_unique<Storage::Observer>();next->model=std::move(resource);
        next->object=static_cast<g::CollectionRef50BX>(slot+1u);next->method=method;
        next->membership.storage={next->links.data(),next->entries.data(),1};
        if(!shared->construct280890(next.get()))return R::fail(d,S::allocation,0x280890,"garage.model.observer.construct280890");
        next->sharedConstructed=true;s.observers[slot]=std::move(next);
    }catch(...){return R::fail(d,S::allocation,0x28E8D0,"garage.model.observer.allocation");}
    auto& observer=*s.observers[slot];auto& model=*observer.model;
    model.scene_.model_.observer19C=&observer; //28E8E8 precedes bounds and attach.
    g::LiveZoneBounds50EN bounds;
    if(!makeBounds28E230(model,bounds,d))return R::failureStage(d,"garage.model.observer.bounds28E230"); // Own the actual partial prefix.
    const auto backing=observer.membership.storage;
    const auto result=g::attachLiveZoneMembership50EN(s.primary4C0.lifecycle,s.primary4C0.world,
        observer.membership,backing,observer.object,1,method,0.f,bounds,s.workspace);
    if(result.status!=g::LiveZoneStatus50EN::Complete)return R::fail(d,S::source,0x2809C0,"garage.model.observer.attach2809C0");
    observer.child04=&s.primary4C0;
    observer.method08=&s.primary4C0.lists[static_cast<unsigned>(method)];
    return true;
}
bool OriginalUiGarageVisibility50KG::beginStaticPopulation2741C0(OriginalUiGarageStaticModels50KG& owner,D& d)noexcept {
    d={};if(!current()||storage_->staticOwner||!sourceSceneOwner_||
       !owner.belongsTo(*sourceSceneOwner_,storage_->grant,storage_->assets)||
       !owner.descriptorIdentityCurrent()||!owner.populationAttempted_||owner.visibility_.get()!=this)
        return R::fail(d,S::phase,0x2741C0);
    for(const auto& observer:storage_->observers)if(observer)return R::fail(d,S::phase,0x2741C0);
    storage_->staticOwner=&owner;
    storage_->primary4C0.environmentNodeVisible44=owner.environmentNodeVisible_?1u:0u;
    return true;
}
bool OriginalUiGarageVisibility50KG::attachStatic27AE0C(OriginalUiGarageStaticModels50KG& owner,
    std::size_t ordinal,std::shared_ptr<R> model,D& d)noexcept {
    d={};if(!current()||storage_->staticOwner!=&owner||ordinal>=staticCount||
       ordinal!=owner.populated_||!owner.descriptorIdentityCurrent()||!owner.enrolled_[ordinal]||
       owner.models_[ordinal]!=model||!model||!storage_->assets->staticModel(ordinal)||
       model->source()!=storage_->assets->staticModel(ordinal)->model.get()||model->mode_!=R::RootMode::direct)
        return R::fail(d,S::input,0x27AE0C);
    const auto* entity=model->scene_.modelEntity8();
    if(!entity||entity->parent1C||entity->previous10||entity->next14||entity->mask20)
        return R::fail(d,S::phase,0x27AE0C);
    return attach(ordinal,std::move(model),g::ZoneMethod50CL::First,d);
}
bool OriginalUiGarageVisibility50KG::staticMembershipCurrent(const OriginalUiGarageStaticModels50KG& owner,
    std::size_t ordinal,const R& model)const noexcept {
    if(!current()||storage_->staticOwner!=&owner||ordinal>=staticCount||!modelMatches(model))return false;
    const auto& held=storage_->observers[ordinal];auto* shared=storage_->grant->observerShared();
    return held&&held->model.get()==&model&&model.scene_.model_.observer19C==held.get()&&
        held->sharedConstructed&&shared&&shared->owns(held.get())&&held->membership.attached&&
        held->object==ordinal+1u&&held->method==g::ZoneMethod50CL::First&&
        held->child04==&storage_->primary4C0&&held->method08==&storage_->primary4C0.lists[0];
}
bool OriginalUiGarageVisibility50KG::endStaticPopulationRetirement(OriginalUiGarageStaticModels50KG& owner,D& d)noexcept {
    d={};if(!storage_||!storage_->staticOwner)return true;
    if(storage_->staticOwner!=&owner||owner.listLive_||owner.retiring_)
        return R::fail(d,S::phase,0x27B700);
    for(std::size_t i=0;i<staticCount;++i)if(storage_->observers[i])return R::fail(d,S::phase,0x28EA80);
    storage_->staticOwner=nullptr;return true;
}
bool OriginalUiGarageVisibility50KG::liveOrExpiredWattoObserverCurrent(
    const OriginalUiGarageWattoOwner50KG& owner)const noexcept {
    return storage_&&storage_->observers[wattoSlot]&&storage_->observers[wattoSlot]->wattoOwner==&owner&&
        (externalObserverCurrent(wattoSlot,false)||externalObserverCurrent(wattoSlot,true));
}
bool OriginalUiGarageVisibility50KG::externalSearchTargetExpired(
    const OriginalUiGaragePageOwner50KG* failedLoadedPage,
    const OriginalUiGaragePageOwner50KG* failedWattoBindingPage)const noexcept {
    if(!storage_||!closed_||!searchEntitiesRetired_||(failedLoadedPage&&failedWattoBindingPage))return false;
    const auto& s=*storage_;
    //No Page, Scene, dead List, or stale registry pointer is dereferenced here.
    return s.grant&&s.grant->current()&&s.attachedPage&&
        (failedWattoBindingPage?
            (s.attachedPage==failedWattoBindingPage&&s.grant->failedWattoBindingGaragePageRetired1413E0(*failedWattoBindingPage)):
         failedLoadedPage?
            (s.attachedPage==failedLoadedPage&&s.grant->failedLoadedHideGaragePageRetired1413E0(*failedLoadedPage)):
            s.grant->garageManagerPageRetired1413E0(s.attachedPage))&&
        s.pageSearchRetirementOwner==s.attachedPage&&s.defaultObserverRetired270144&&
        !s.defaultObserver443148&&s.sceneListPayloadsRetired2700E8&&
        !s.primary4C0.registries.data()&&!s.secondary4C4.registries.data()&&
        !s.sceneLists4C8&&!s.sceneLists4D0&&!s.sceneLists4CC&&!s.sceneLists4D4&&
        s.nodePointerReleaseStage==2&&!s.primaryNodes4C0&&!s.secondaryNodes4C4;
}
bool OriginalUiGarageVisibility50KG::externalObserverCurrent(std::size_t slot,bool expired,
    const OriginalUiGaragePageOwner50KG* failedLoadedPage,
    const OriginalUiGaragePageOwner50KG* failedWattoBindingPage)const noexcept {
    if(!storage_||(slot!=selectedSlot&&slot!=wattoSlot)||
       (failedLoadedPage&&failedWattoBindingPage)||
       ((failedLoadedPage||failedWattoBindingPage)&&(!expired||slot!=selectedSlot||
        !externalSearchTargetExpired(failedLoadedPage,failedWattoBindingPage))))return false;
    const auto& s=*storage_;const auto& held=s.observers[slot];
    auto* shared=s.grant?s.grant->observerShared():nullptr;
    if(!s.grant||!s.grant->current()||!shared||!shared->coherent()||!held||!held->model||
       !held->sharedConstructed||held->sourceRetired28976C||!shared->owns(held.get())||
       s.retiredExternalObserverIdentity[slot-selectedSlot]||
       held->object!=slot+1u||held->method!=g::ZoneMethod50CL::First||
       held->child04!=&s.primary4C0||held->method08!=&s.primary4C0.lists[0])return false;
    const auto& model=*held->model;const auto* entity=model.scene_.modelEntity8();
    bool modelLifetimeCurrent=model.current();
    if(!modelLifetimeCurrent){
        //240450/26E4C0/26E598 intentionally end live resource readiness before
        //28976C. Authenticate the exact destruction owner separately for each
        //completed Page origin, then share only its real ref0/resource cursor.
        bool retirementOwnerCurrent=false;
        if(failedWattoBindingPage){
            const auto* owner=s.selectedOwner;
            retirementOwnerCurrent=owner&&!owner->failedLoadedPageManagerRetirementOwner_&&
                owner->failedWattoBindingManagerRetirementOwner_==failedWattoBindingPage&&
                owner->managerRetiring_&&owner->model_.get()==&model;
        }else if(failedLoadedPage){
            const auto* owner=s.selectedOwner;
            retirementOwnerCurrent=owner&&!owner->failedWattoBindingManagerRetirementOwner_&&
                owner->failedLoadedPageManagerRetirementOwner_==failedLoadedPage&&
                owner->managerRetiring_&&owner->model_.get()==&model;
        }else if(expired&&externalSearchTargetExpired()){
            if(slot==selectedSlot){
                const auto* owner=s.selectedOwner;
                retirementOwnerCurrent=owner&&!owner->failedLoadedPageManagerRetirementOwner_&&
                    !owner->failedWattoBindingManagerRetirementOwner_&&owner->managerRetiring_&&owner->model_.get()==&model;
            }else retirementOwnerCurrent=held->wattoOwner&&s.attachedPage&&
                held->wattoOwner->normalModelObserverRetirementCurrent(*this,model,s.attachedPage);
        }
        const auto stage=model.pageRetirement_;
        modelLifetimeCurrent=retirementOwnerCurrent&&entity&&!entity->reference00&&
            model.referenceReleased142590()&&model.complete_&&!model.closed_&&!model.scene_.released_&&
            !model.nativeGarageFifoPins_&&model.pageObserverIdentity_==held.get()&&
            stage>=R::PageRetirement::packetsReleased&&stage<=R::PageRetirement::childrenReleased&&
            model.packets_.released()&&model.context_&&model.context_->live()&&
            model.domain_&&model.domain_->prepared()&&
            (stage<R::PageRetirement::materialsReleased||model.materials_.released26E4C0())&&
            (stage<R::PageRetirement::geometryReleased?
                (model.resourceHeld_&&model.geometry_&&model.geometry_->holdsModelReference(&model.scene_)):
                !model.resourceHeld_)&&
            (stage<R::PageRetirement::childrenReleased||model.scene_.garageSceneChildrenRetired_);
    }
    if(!entity||!modelLifetimeCurrent||model.generation_!=s.grant->generation_||
       model.application_!=s.grant->application()||model.scene_.model_.observer19C!=held.get()||
       entity->kind!=K::model2403A0||entity->sessionGeneration!=s.grant->generation_||
       entity->objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(entity))||
       entity->reference00!=(model.referenceReleased142590()?0u:1u))return false;
    if(slot==selectedSlot){
        const auto* owner=s.selectedOwner;
        if(!owner||owner->grant_!=s.grant||owner->assets_!=s.assets||owner->model_!=held->model||
           owner->visibility_.get()!=this||owner->closed_||entity->parent1C!=s.grant->manager()||
           entity->mask20!=2u||held->wattoOwner||held->wattoAssets)return false;
    }else {
        OriginalUiGarageWattoOwner50KG::VisibilityBinding b;
        if(!held->wattoOwner||!held->wattoOwner->visibilityBinding(b)||b.grant!=s.grant||
           b.sceneAssets!=s.assets||b.model!=held->model||b.visibility.get()!=this||
           b.assets!=held->wattoAssets||b.primary4C0!=&s.primary4C0.entity||
           !b.widget||b.widget->parent1C!=s.grant->manager()||b.widget->reference00!=1||
           !b.animation220||b.animation220->parent1C!=b.widget||b.animation220->reference00!=1||
           entity->parent1C!=b.animation220||entity->mask20||
           b.animationModel04!=model.scene_.model())return false;
    }
    if(expired)return held->targetNodeExpired&&held->targetListExpired&&externalSearchTargetExpired(failedLoadedPage,failedWattoBindingPage);
    return !closed_&&!held->targetListExpired&&s.primary4C0.registries.data();
}
bool OriginalUiGarageVisibility50KG::externalObserverRetirementCurrent(std::size_t slot,const R& model,
    const OriginalUiGaragePageOwner50KG* failedLoadedPage,
    const OriginalUiGaragePageOwner50KG* failedWattoBindingPage)const noexcept {
    if(!externalSearchTargetExpired(failedLoadedPage,failedWattoBindingPage)||(slot!=selectedSlot&&slot!=wattoSlot)||
       (failedWattoBindingPage&&(slot!=selectedSlot||!storage_->selectedOwner||
        storage_->selectedOwner->failedLoadedPageManagerRetirementOwner_||
        storage_->selectedOwner->failedWattoBindingManagerRetirementOwner_!=failedWattoBindingPage||
        storage_->selectedOwner->model_.get()!=&model))||
       (failedLoadedPage&&(slot!=selectedSlot||!storage_->selectedOwner||
        storage_->selectedOwner->failedWattoBindingManagerRetirementOwner_||
        storage_->selectedOwner->failedLoadedPageManagerRetirementOwner_!=failedLoadedPage||
        storage_->selectedOwner->model_.get()!=&model)))return false;
    const auto& s=*storage_;const auto* shared=s.grant->observerShared();
    const auto identity=s.retiredExternalObserverIdentity[slot-selectedSlot];
    return shared&&shared->coherent()&&identity&&!s.observers[slot]&&!shared->owns(identity)&&
        s.retiredExternalModelIdentity[slot-selectedSlot]==&model&&
        model.application_==s.grant->application()&&model.generation_==s.grant->generation_&&
        model.pageObserverIdentity_==identity&&model.scene_.model_.observer19C==identity&&
        model.referenceReleased142590()&&model.scene_.garageSceneChildrenRetired_&&
        (model.pageRetirement_==R::PageRetirement::observerReleased||
         model.pageRetirement_==R::PageRetirement::complete);
}
bool OriginalUiGarageVisibility50KG::preflightSelectedObserver28976C(
    const OriginalUiGarageSelectedModel50KG& owner)const noexcept {
    return storage_&&storage_->selectedOwner==&owner&&owner.model_&&
        (externalObserverCurrent(selectedSlot,true)||
         externalObserverRetirementCurrent(selectedSlot,*owner.model_));
}
bool OriginalUiGarageVisibility50KG::preflightFailedLoadedSelectedObserver28976C(
    const OriginalUiGarageSelectedModel50KG& owner,const OriginalUiGaragePageOwner50KG& page)const noexcept {
    return storage_&&storage_->selectedOwner==&owner&&storage_->attachedPage==&page&&owner.model_&&!owner.failedWattoBindingManagerRetirementOwner_&&
        owner.visibility_.get()==this&&owner.grant_==storage_->grant&&
        (!owner.failedLoadedPageManagerRetirementOwner_||owner.failedLoadedPageManagerRetirementOwner_==&page)&&
        (externalObserverCurrent(selectedSlot,true,&page)||
         externalObserverRetirementCurrent(selectedSlot,*owner.model_,&page));
}
bool OriginalUiGarageVisibility50KG::preflightFailedWattoBindingSelectedObserver28976C(
    const OriginalUiGarageSelectedModel50KG& owner,const OriginalUiGaragePageOwner50KG& page)const noexcept {
    return storage_&&storage_->selectedOwner==&owner&&storage_->attachedPage==&page&&owner.model_&&!owner.failedLoadedPageManagerRetirementOwner_&&
        owner.visibility_.get()==this&&owner.grant_==storage_->grant&&
        (!owner.failedWattoBindingManagerRetirementOwner_||owner.failedWattoBindingManagerRetirementOwner_==&page)&&
        (externalObserverCurrent(selectedSlot,true,nullptr,&page)||
         externalObserverRetirementCurrent(selectedSlot,*owner.model_,nullptr,&page));
}
bool OriginalUiGarageVisibility50KG::preflightWattoObserver28976C(
    const OriginalUiGarageWattoOwner50KG& owner)const noexcept {
    OriginalUiGarageWattoOwner50KG::VisibilityBinding b;
    if(!storage_||!owner.visibilityBinding(b)||!b.model||b.visibility.get()!=this)return false;
    if(storage_->observers[wattoSlot]&&storage_->observers[wattoSlot]->wattoOwner!=&owner)return false;
    return externalObserverCurrent(wattoSlot,true)||externalObserverRetirementCurrent(wattoSlot,*b.model);
}
bool OriginalUiGarageVisibility50KG::retireExternalObserver28976C(std::size_t slot,R& model,D& d,
    const OriginalUiGaragePageOwner50KG* failedLoadedPage,
    const OriginalUiGaragePageOwner50KG* failedWattoBindingPage)noexcept {
    d={};if(!externalObserverCurrent(slot,true,failedLoadedPage,failedWattoBindingPage)||storage_->observers[slot]->model.get()!=&model||
       model.pageRetirement_!=R::PageRetirement::childrenReleased||!model.referenceReleased142590()||
       !model.scene_.garageSceneChildrenRetired_||model.scene_.released_||
       model.pageObserverIdentity_!=storage_->observers[slot].get())return R::fail(d,S::phase,0x28976C);
    auto& held=storage_->observers[slot];auto* shared=storage_->grant->observerShared();
    if(!shared->release280920(held.get()))return R::fail(d,S::phase,0x280920);
    held->sharedConstructed=false;held->sourceRetired28976C=true;
    storage_->retiredExternalObserverIdentity[slot-selectedSlot]=held.get();
    storage_->retiredExternalModelIdentity[slot-selectedSlot]=&model;
    held.reset(); //Actual Observer allocation free280998. No19C/04/08 writes.
    model.pageRetirement_=R::PageRetirement::observerReleased;return true;
}
bool OriginalUiGarageVisibility50KG::retireSelectedObserver28976C(OriginalUiGarageSelectedModel50KG& owner,D& d)noexcept {
    d={};if(!storage_||storage_->selectedOwner!=&owner||!owner.model_)
        return R::fail(d,S::phase,0x28976C);
    return retireExternalObserver28976C(selectedSlot,*owner.model_,d);
}
bool OriginalUiGarageVisibility50KG::retireFailedLoadedSelectedObserver28976C(
    OriginalUiGarageSelectedModel50KG& owner,const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!storage_||storage_->selectedOwner!=&owner||!owner.model_||owner.failedWattoBindingManagerRetirementOwner_||
       owner.failedLoadedPageManagerRetirementOwner_!=&page||!owner.managerRetiring_||
       owner.visibility_.get()!=this)return R::fail(d,S::phase,0x28976C);
    return retireExternalObserver28976C(selectedSlot,*owner.model_,d,&page);
}
bool OriginalUiGarageVisibility50KG::retireFailedWattoBindingSelectedObserver28976C(
    OriginalUiGarageSelectedModel50KG& owner,const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    d={};if(!storage_||storage_->selectedOwner!=&owner||!owner.model_||owner.failedLoadedPageManagerRetirementOwner_||
       owner.failedWattoBindingManagerRetirementOwner_!=&page||!owner.managerRetiring_||
       owner.visibility_.get()!=this)return R::fail(d,S::phase,0x28976C);
    return retireExternalObserver28976C(selectedSlot,*owner.model_,d,nullptr,&page);
}
bool OriginalUiGarageVisibility50KG::retireWattoObserver28976C(OriginalUiGarageWattoOwner50KG& owner,D& d)noexcept {
    d={};OriginalUiGarageWattoOwner50KG::VisibilityBinding b;
    if(!owner.visibilityBinding(b)||!b.model||!storage_||!storage_->observers[wattoSlot]||
       storage_->observers[wattoSlot]->wattoOwner!=&owner)return R::fail(d,S::phase,0x28976C);
    return retireExternalObserver28976C(wattoSlot,*b.model,d);
}
bool OriginalUiGarageSelectedModel50KG::closeManager142590(D& d)noexcept {
    d={};if(failedLoadedPageManagerRetirementOwner_||failedWattoBindingManagerRetirementOwner_)return R::fail(d,S::phase,0x142590);
    return closeAttachedManager142590(d,nullptr);
}
bool OriginalUiGarageSelectedModel50KG::preflightFailedLoadedPageManager142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)const noexcept {
    d={};if(failedWattoBindingManagerRetirementOwner_||!grant_||!grant_->current()||!grant_->failedLoadedHideGaragePageRetired1413E0(page)||
       !model_||managerSearchBorrower_||
       (failedLoadedPageManagerRetirementOwner_&&failedLoadedPageManagerRetirementOwner_!=&page))
        return R::fail(d,S::phase,0x142590);
    if(closed_){
        if(failedLoadedPageManagerRetirementOwner_!=&page||model_->pageRetirement_!=R::PageRetirement::complete)
            return R::fail(d,S::phase,0x1413E0);
        return true;
    }
    if(!visibility_||!visibility_->preflightFailedLoadedSelectedObserver28976C(*this,page))
        return R::fail(d,S::phase,0x28976C);
    if(managerRetiring_){
        if(failedLoadedPageManagerRetirementOwner_!=&page||!model_->referenceReleased142590())
            return R::fail(d,S::phase,0x142590);
        return true;
    }
    const auto* entity=model_->scene_.modelEntity8();
    if(!current()||!entity||entity->next14||entity->firstChild18||
       model_->pageRetirement_!=R::PageRetirement::none||
       !model_->canRetireResources(model_->scene_.model()->observer19C))
        return R::fail(d,S::phase,0x142590);
    return true;
}
bool OriginalUiGarageSelectedModel50KG::preflightFailedWattoBindingManager142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)const noexcept {
    d={};if(failedLoadedPageManagerRetirementOwner_||!grant_||!grant_->current()||!grant_->failedWattoBindingGaragePageRetired1413E0(page)||
       !model_||managerSearchBorrower_||
       (failedWattoBindingManagerRetirementOwner_&&failedWattoBindingManagerRetirementOwner_!=&page))
        return R::fail(d,S::phase,0x142590);
    if(closed_){
        if(failedWattoBindingManagerRetirementOwner_!=&page||model_->pageRetirement_!=R::PageRetirement::complete)
            return R::fail(d,S::phase,0x1413E0);
        return true;
    }
    if(!visibility_||!visibility_->preflightFailedWattoBindingSelectedObserver28976C(*this,page))
        return R::fail(d,S::phase,0x28976C);
    if(managerRetiring_){
        if(failedWattoBindingManagerRetirementOwner_!=&page||!model_->referenceReleased142590())
            return R::fail(d,S::phase,0x142590);
        return true;
    }
    const auto* entity=model_->scene_.modelEntity8();
    if(!current()||!entity||entity->next14||entity->firstChild18||
       model_->pageRetirement_!=R::PageRetirement::none||
       !model_->canRetireResources(model_->scene_.model()->observer19C))
        return R::fail(d,S::phase,0x142590);
    return true;
}
bool OriginalUiGarageSelectedModel50KG::closeFailedLoadedPageManager142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    if(!preflightFailedLoadedPageManager142590(page,d))return false;
    if(closed_)return true;
    //Native invocation receipt only, after genuine completed failed Page and
    //same retained Observer/model preflight. No normal Page receipt is minted.
    failedLoadedPageManagerRetirementOwner_=&page;
    return closeAttachedManager142590(d,&page);
}
bool OriginalUiGarageSelectedModel50KG::closeFailedWattoBindingManager142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    if(!preflightFailedWattoBindingManager142590(page,d))return false;
    if(closed_)return true;
    //Native invocation receipt only, after genuine completed failed Page and
    //same retained Observer/model preflight. No normal Page receipt is minted.
    failedWattoBindingManagerRetirementOwner_=&page;
    return closeAttachedManager142590(d,nullptr,&page);
}
bool OriginalUiGarageSelectedModel50KG::closeAttachedManager142590(D& d,
    const OriginalUiGaragePageOwner50KG* failedLoadedPage,
    const OriginalUiGaragePageOwner50KG* failedWattoBindingPage)noexcept {
    d={};if(closed_)return true;
    if(!grant_||!grant_->current()||(failedLoadedPage&&failedWattoBindingPage)||
       (failedWattoBindingPage?
        (failedLoadedPageManagerRetirementOwner_||failedWattoBindingManagerRetirementOwner_!=failedWattoBindingPage||
         !grant_->failedWattoBindingGaragePageRetired1413E0(*failedWattoBindingPage)):
        failedLoadedPage?
        (failedWattoBindingManagerRetirementOwner_||failedLoadedPageManagerRetirementOwner_!=failedLoadedPage||!grant_->failedLoadedHideGaragePageRetired1413E0(*failedLoadedPage)):
        !grant_->garageManagerPageRetired1413E0())||
       !model_||managerSearchBorrower_)return R::fail(d,S::phase,0x142590);
    const auto* sourceModel=model_->scene_.model();
    if(!sourceModel)return R::fail(d,S::phase,0x28976C);
    if(visibility_){
        if(failedWattoBindingPage?!visibility_->preflightFailedWattoBindingSelectedObserver28976C(*this,*failedWattoBindingPage):
            failedLoadedPage?!visibility_->preflightFailedLoadedSelectedObserver28976C(*this,*failedLoadedPage):
            !visibility_->preflightSelectedObserver28976C(*this))return R::fail(d,S::phase,0x28976C);
    }else if(sourceModel->observer19C)return R::fail(d,S::phase,0x28976C);
    const auto* entity=model_->scene_.modelEntity8();
    if(!managerRetiring_){
        if(!current()||entity->next14||entity->firstChild18||
           model_->pageRetirement_!=R::PageRetirement::none||
           !model_->canRetireResources(model_->scene_.model()->observer19C))
            return R::fail(d,S::phase,0x142590);
        const auto begin=grant_->retireModel(model_->scene_);
        if(!begin.okay())return R::fail(d,S::phase,begin.sourcePC);
        managerRetiring_=true;
    }
    if(model_->pageRetirement_<=R::PageRetirement::payloadsReleased&&
       !model_->retirePagePayloads240450(d))return false;
    if(model_->pageRetirement_==R::PageRetirement::payloadsReleased&&
       !model_->retirePageSceneChildren2896D0(d))return false;
    if(model_->pageRetirement_==R::PageRetirement::childrenReleased){
        if(model_->pageObserverIdentity_){
            if(!visibility_||(failedWattoBindingPage?
                !visibility_->retireFailedWattoBindingSelectedObserver28976C(*this,*failedWattoBindingPage,d):failedLoadedPage?
                !visibility_->retireFailedLoadedSelectedObserver28976C(*this,*failedLoadedPage,d):
                !visibility_->retireSelectedObserver28976C(*this,d)))return false;
        }else model_->pageRetirement_=R::PageRetirement::observerReleased; //Actual null19C branch289760.
    }
    if(model_->pageRetirement_==R::PageRetirement::observerReleased&&
       !model_->finishPageSceneBase1413E0(d))return false;
    if(model_->pageRetirement_!=R::PageRetirement::complete)return R::fail(d,S::phase,0x1413E0);
    enrolled_=false;attached_=false;placed_=false;closed_=true;managerRetiring_=false;
    visibility_.reset();return true;
}
bool OriginalUiGarageVisibility50KG::attachSelected1FB67C(OriginalUiGarageSelectedModel50KG& selected,D& d)noexcept {
    d={};if(!current()||!selected.current()||selected.grant_!=storage_->grant||selected.assets_!=storage_->assets||
       !selected.model_||selected.model_->source()!=storage_->assets->selectedPod()->model.get())
        return R::fail(d,S::input,0x1FB67C,"garage.selected.visibility.inputs");
    if(storage_->selectedOwner&&storage_->selectedOwner!=&selected)return R::fail(d,S::phase,0x1FB67C,"garage.selected.visibility.owner");
    if(sourceSceneOwner_){
        if(sourceSceneOwner_->visibility_.get()!=this||
           (selected.visibility_&&selected.visibility_.get()!=this))return R::fail(d,S::phase,0x1FB67C,"garage.selected.visibility.sceneBorrow");
        //Native custody survives the Scene payload. It does not retain any
        //source Entity/List reference or extend its counted lifetime.
        selected.visibility_=sourceSceneOwner_->visibility_;
    }
    storage_->selectedOwner=&selected;
    return attach(selectedSlot,selected.model_,g::ZoneMethod50CL::First,d);
}
bool OriginalUiGarageVisibility50KG::attachActor2D9350(std::size_t ordinal,std::shared_ptr<R> model,D& d)noexcept {
    d={};if(!current()||ordinal>=actorCount||!model||!storage_->assets->actor(ordinal)||
       model->source()!=storage_->assets->actor(ordinal)->model.get()||model->mode_!=R::RootMode::separate||
       storage_->assets->actor(ordinal)->startingZone!=1||storage_->assets->actor(ordinal)->useCollision)
        return R::fail(d,S::input,0x2D9350);
    if(storage_->staticOwner&&!storage_->staticOwner->populationCurrent())return R::fail(d,S::phase,0x274160);
    const auto* entity=model->scene_.modelEntity8();
    if(!entity||!entity->parent1C||entity->parent1C->kind!=K::character11A750||entity->mask20)
        return R::fail(d,S::phase,0x2D9350);
    // Actor owner additionally enforces library, ToyList, initial motion and
    //sound order. This owner cannot invent those completed predecessor stages.
    for(std::size_t i=0;i<ordinal;++i){const auto& previous=storage_->observers[staticCount+i];
        if(!previous||!previous->membership.attached||previous->membership.revision<2)
            return R::fail(d,S::phase,0x2D9418);
    }
    return attach(staticCount+ordinal,std::move(model),g::ZoneMethod50CL::Second,d);
}
bool OriginalUiGarageVisibility50KG::update(std::size_t slot,R& model,D& d)noexcept {
    d={};if(slot>=modelCount||!modelMatches(model))return R::fail(d,S::input,0x28EA20);
    auto& s=*storage_;const auto& held=s.observers[slot];
    if(!held)return model.scene_.model_.observer19C?R::fail(d,S::phase,0x28EA2C):true;
    auto& observer=*held;auto* shared=s.grant->observerShared();
    if(observer.model.get()!=&model||model.scene_.model_.observer19C!=&observer||!shared||!shared->owns(&observer)||
       !observer.membership.attached||observer.child04!=&s.primary4C0||
       observer.method08!=&s.primary4C0.lists[static_cast<unsigned>(observer.method)])
        return R::fail(d,S::phase,0x28EA2C);
    g::LiveZoneBounds50EN bounds;if(!makeBounds28E230(model,bounds,d))return false;
    const auto result=g::updateLiveZoneMembership50EN(s.primary4C0.lifecycle,s.primary4C0.world,
        observer.membership,bounds,s.workspace);
    return result.status==g::LiveZoneStatus50EN::Complete?true:R::fail(d,S::source,0x280D40);
}
bool OriginalUiGarageVisibility50KG::updateActor28EA20(std::size_t ordinal,R& model,D& d)noexcept {
    if(ordinal>=actorCount){d={};return R::fail(d,S::input,0x28EA20);}
    return update(staticCount+ordinal,model,d);
}
bool OriginalUiGarageVisibility50KG::remove(std::size_t slot,R& model,D& d)noexcept {
    d={};if(slot>=modelCount||!modelMatches(model))return R::fail(d,S::input,0x28EA80);
    auto& s=*storage_;auto& held=s.observers[slot];
    if(!held)return model.scene_.model_.observer19C?R::fail(d,S::phase,0x28EA90):true;
    auto& observer=*held;auto* shared=s.grant->observerShared();
    if(observer.model.get()!=&model||model.scene_.model_.observer19C!=&observer||!observer.sharedConstructed||
       !shared||!shared->owns(&observer))return R::fail(d,S::phase,0x280920);
    if(observer.membership.attached){
        const auto result=g::removeLiveZoneMembership50EN(s.primary4C0.lifecycle,s.primary4C0.world,
            observer.membership,s.workspace);
        if(result.status!=g::LiveZoneStatus50EN::Complete)return R::fail(d,S::source,0x281490);
    }
    const auto& registration=s.primary4C0.registrations[slot];
    if(registration.owner||registration.slot||registration.encodedObject)return R::fail(d,S::phase,0x281490);
    if(!shared->release280920(&observer))std::terminate();
    observer.sharedConstructed=false;
    // Source280920 destruction precedes clearing19C. No stale public pointer
    //is observable between these operations in this serial cold owner.
    held.reset();model.scene_.model_.observer19C=nullptr;return true;
}
bool OriginalUiGarageVisibility50KG::removeActor28EA80(std::size_t ordinal,R& model,D& d)noexcept {
    if(ordinal>=actorCount){d={};return R::fail(d,S::input,0x28EA80);}
    return remove(staticCount+ordinal,model,d);
}
bool OriginalUiGarageVisibility50KG::pageStaticObserverCurrent(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageStaticModels50KG& owner,
    std::size_t ordinal,const R& model)const noexcept {
    if(!current()||ordinal>=staticCount||storage_->attachedPage!=&page||
       storage_->staticOwner!=&owner||sourceSceneOwner_!=owner.sceneOwner_||
       !sourceSceneOwner_||owner.models_[ordinal].get()!=&model||
       !storage_->grant->garagePageStaticListRetirementCurrent(page,*sourceSceneOwner_,owner.list3C4_))
        return false;
    const auto& s=*storage_;const auto& held=s.observers[ordinal];
    const auto& row=s.primary4C0.registrations[ordinal];auto* shared=s.grant->observerShared();
    return held&&held->model.get()==&model&&model.scene_.model_.observer19C==held.get()&&
        held->sharedConstructed&&!held->sourceRetired28976C&&shared&&shared->owns(held.get())&&
        held->membership.attached&&held->object==ordinal+1u&&held->method==g::ZoneMethod50CL::First&&
        held->membership.object==held->object&&held->membership.method==held->method&&
        held->membership.storage.links==held->links.data()&&
        held->membership.storage.entries==held->entries.data()&&held->membership.storage.capacity==1&&
        held->child04==&s.primary4C0&&held->method08==&s.primary4C0.lists[0]&&
        row.owner==&s.primary4C0.registries[0]&&row.slot&&row.slot<=1024&&
        row.encodedObject==static_cast<std::uint32_t>((ordinal+1u)<<2)&&
        row.owner->words[row.slot-1]==row.encodedObject;
}
bool OriginalUiGarageVisibility50KG::preflightPageStaticObserver28976C(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageStaticModels50KG& owner,
    std::size_t ordinal,const R& model)const noexcept {
    return pageStaticObserverCurrent(page,owner,ordinal,model)&&
        model.scene_.modelEntity8()&&model.scene_.modelEntity8()->reference00==1&&!model.scene_.referenceReleased_&&
        model.pageRetirement_==R::PageRetirement::none&&model.current();
}
bool OriginalUiGarageVisibility50KG::retirePageStaticObserver28976C(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageStaticModels50KG& owner,
    std::size_t ordinal,R& model,D& d)noexcept {
    d={};if(!pageStaticObserverCurrent(page,owner,ordinal,model)||
       model.pageRetirement_!=R::PageRetirement::childrenReleased||
       !model.scene_.referenceReleased_||!model.scene_.garageSceneChildrenRetired_||
       !model.scene_.modelEntity8()||model.scene_.modelEntity8()->reference00||model.scene_.released_||
       model.pageObserverIdentity_!=storage_->observers[ordinal].get())
        return R::fail(d,S::phase,0x28976C);
    auto& observer=*storage_->observers[ordinal];auto* shared=storage_->grant->observerShared();
    if(!shared->release280920(&observer))return R::fail(d,S::phase,0x280920);
    observer.sharedConstructed=false;observer.sourceRetired28976C=true;
    //2896D0 calls280920 only: no281490, no28EA80 and no19C clear. Retain
    //native comparison backing and the actual dead SearchList row until the
    //later List destructor. This is not an alive observer or a reusable row.
    model.pageRetirement_=R::PageRetirement::observerReleased;return true;
}
bool OriginalUiGarageVisibility50KG::pageActorObserverCurrent(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageActors50GN& owner,
    std::size_t ordinal,const R& model)const noexcept {
    if(!current()||ordinal>=actorCount||storage_->attachedPage!=&page||
       !sourceSceneOwner_||sourceSceneOwner_->actorOwner3C8_!=&owner||
       sourceSceneOwner_->visibility_.get()!=this||
       !owner.pageActorModelRetirementCurrent(page,ordinal,model))return false;
    const auto slot=staticCount+ordinal;
    const auto& s=*storage_;const auto& held=s.observers[slot];
    const auto& row=s.primary4C0.registrations[slot];auto* shared=s.grant->observerShared();
    return held&&held->model.get()==&model&&model.scene_.model_.observer19C==held.get()&&
        held->sharedConstructed&&!held->sourceRetired28976C&&shared&&shared->owns(held.get())&&
        held->membership.attached&&held->object==slot+1u&&held->method==g::ZoneMethod50CL::Second&&
        held->membership.object==held->object&&held->membership.method==held->method&&
        held->membership.storage.links==held->links.data()&&
        held->membership.storage.entries==held->entries.data()&&held->membership.storage.capacity==1&&
        held->child04==&s.primary4C0&&held->method08==&s.primary4C0.lists[1]&&
        row.owner==&s.primary4C0.registries[1]&&row.slot&&row.slot<=1024&&
        row.encodedObject==static_cast<std::uint32_t>((slot+1u)<<2)&&
        row.owner->words[row.slot-1]==row.encodedObject;
}
bool OriginalUiGarageVisibility50KG::preflightPageActorObserver28976C(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageActors50GN& owner,
    std::size_t ordinal,const R& model)const noexcept {
    return pageActorObserverCurrent(page,owner,ordinal,model)&&
        model.scene().modelEntity8()->reference00==1&&!model.scene_.referenceReleased_&&
        model.pageRetirement_==R::PageRetirement::none&&model.current();
}
bool OriginalUiGarageVisibility50KG::retirePageActorObserver28976C(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGarageActors50GN& owner,
    std::size_t ordinal,R& model,D& d)noexcept {
    d={};if(!pageActorObserverCurrent(page,owner,ordinal,model)||
       model.pageRetirement_!=R::PageRetirement::childrenReleased||
       !model.scene_.referenceReleased_||!model.scene_.garageSceneChildrenRetired_||
       model.scene().modelEntity8()->reference00||model.scene_.released_||
       model.pageObserverIdentity_!=storage_->observers[staticCount+ordinal].get())
        return R::fail(d,S::phase,0x28976C);
    auto& observer=*storage_->observers[staticCount+ordinal];auto* shared=storage_->grant->observerShared();
    if(!shared->release280920(&observer))return R::fail(d,S::phase,0x280920);
    observer.sharedConstructed=false;observer.sourceRetired28976C=true;
    //Actual28976C destroys only the Observer. No28EA80/281490 occurs here:
    //method2 Search registration, membership and model19C remain tombstones
    //until the later genuine SearchList destructor, exactly as static rows do.
    model.pageRetirement_=R::PageRetirement::observerReleased;return true;
}
bool OriginalUiGarageVisibility50KG::removeStatic28EA80(std::size_t ordinal,R& model,D& d)noexcept {
    if(ordinal>=staticCount){d={};return R::fail(d,S::input,0x28EA80);}
    return remove(ordinal,model,d);
}
bool OriginalUiGarageVisibility50KG::removeSelected28EA80(OriginalUiGarageSelectedModel50KG& selected,D& d)noexcept {
    d={};if(!current()||!selected.model_||selected.grant_!=storage_->grant)return R::fail(d,S::input,0x28EA80);
    if(!remove(selectedSlot,*selected.model_,d))return false;
    selected.visibility_.reset();return true;
}
bool OriginalUiGarageVisibility50KG::wattoModelMatches(const OriginalUiGarageWattoOwner50KG& owner,
    std::shared_ptr<R>& model,std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG>& assets,D& d)const noexcept {
    OriginalUiGarageWattoOwner50KG::VisibilityBinding b;
    //The original entry/removal contract remains cold. A live update has a
    //different actual caller/publication proof, never a relaxed cold guard.
    if(!current()||!owner.visibilityBinding(b)||!matchesColdConstruction(b.grant,b.sceneAssets))
        return R::fail(d,S::input,0x1D4800);
    return wattoModelIdentityMatches(owner,model,assets,d);
}
bool OriginalUiGarageVisibility50KG::wattoModelIdentityMatches(const OriginalUiGarageWattoOwner50KG& owner,
    std::shared_ptr<R>& model,std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG>& assets,D& d)const noexcept {
    OriginalUiGarageWattoOwner50KG::VisibilityBinding b;
    if(!current()||!owner.visibilityBinding(b)||b.visibility.get()!=this||
       !b.grant||!b.sceneAssets||storage_->grant.get()!=b.grant.get()||
       storage_->grant.owner_before(b.grant)||b.grant.owner_before(storage_->grant)||
       storage_->assets.get()!=b.sceneAssets.get()||storage_->assets.owner_before(b.sceneAssets)||
       b.sceneAssets.owner_before(storage_->assets)||!b.model||!modelMatches(*b.model)||
       !b.assets||!b.assets->loaded()||!b.assets->geometrySource()||!b.assets->hierarchy()||
       b.primary4C0!=&storage_->primary4C0.entity||b.model->mode_!=R::RootMode::separate)
        return R::fail(d,S::input,0x1D4800);
    const auto* source=b.model->source();
    if(!source||!source->payload||source->archive!=storage_->assets->archive()||
       source->source.identity!=b.assets->geometrySource()||
       source->hierarchy.data!=b.assets->hierarchy()->data||
       source->hierarchy.bytes!=b.assets->hierarchy()->bytes||source->payload->nodeCount()!=32u)
        return R::fail(d,S::source,0x1D4818);
    const auto* entity=b.model->scene_.modelEntity8();const auto* actualModel=b.model->scene_.model();
    if(!b.widget||b.widget->kind!=K::wattoWidget1D4320||b.widget->reference00!=1||
       b.widget->sessionGeneration!=b.grant->generation_||
       b.widget->objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(b.widget))||
       !b.animation220||b.animation220->kind!=K::animation1536F0||b.animation220->reference00!=1||
       b.animation220->sessionGeneration!=b.grant->generation_||
       b.animation220->objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(b.animation220))||
       b.animation220->parent1C!=b.widget||b.animation220->mask20!=2u||
       !entity||entity->parent1C!=b.animation220||entity->mask20||b.animationModel04!=actualModel)
        return R::fail(d,S::phase,0x1D4818);
    const auto graph=b.model->graph();
    const auto index=actualModel?static_cast<std::int32_t>(actualModel->sceneIndex):-1;
    if(!g::validateSceneGraph49P(graph)||index<0||static_cast<std::uint32_t>(index)>=graph.count||
       !(graph.nodes[index].sceneFlags190&8u)||(graph.nodes[index].sceneFlags190&0x10u))
        return R::fail(d,S::phase,0x1D4730);
    model=std::move(b.model);assets=std::move(b.assets);return true;
}
bool OriginalUiGarageVisibility50KG::attachWatto1D4800(OriginalUiGarageWattoOwner50KG& owner,D& d)noexcept {
    d={};std::shared_ptr<R> model;std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG> assets;
    if(!wattoModelMatches(owner,model,assets,d))return false;
    const auto& prior=storage_->observers[wattoSlot];
    if(prior&&(prior->wattoOwner!=&owner||prior->wattoAssets!=assets))return R::fail(d,S::phase,0x1D481C);
    if(!remove(wattoSlot,*model,d))return false; // Actual source1D481C, including null19C.
    const bool complete=attach(wattoSlot,model,g::ZoneMethod50CL::First,d);
    // Retain the exact caller identity for a real partial source observer too.
    auto& held=storage_->observers[wattoSlot];
    if(held&&held->model==model){held->wattoOwner=&owner;held->wattoAssets=std::move(assets);}
    return complete;
}
bool OriginalUiGarageVisibility50KG::updateWatto28EA20(OriginalUiGarageWattoOwner50KG& owner,D& d)noexcept {
    d={};const OriginalUiGaragePageOwner50KG* page=nullptr;std::uint64_t publication=0;
    if(!owner.observerUpdateCall1D4DD0(*this,page,publication)||!publication||!current())
        return R::fail(d,S::phase,0x1D4DD0);
    const auto& s=*storage_;const auto app=s.grant->application();
    if(!page||s.attachedPage!=page||s.cameraPage!=page||!s.camera||
       !pageCameraRegistrationCurrent(*page,*s.camera)||s.searchTerminalPC||!app||
       app->portalDepth443120_||app->portalDrawDepth443118_)
        return R::fail(d,S::phase,0x1D4DD0);
    if(!s.searchPublication){
        //First live update precedes the first Camera114D28. There is no
        //prior Search/renderer publication to fabricate or require.
        if(s.searchPhase!=Storage::SearchPhase::none||s.renderPublication||
           s.renderPhase!=Storage::RenderPhase::none)return R::fail(d,S::phase,0x1D4DD0);
    }else if(s.searchPublication>=publication||s.renderPublication!=s.searchPublication||
             s.searchPhase!=Storage::SearchPhase::returned||s.renderPhase!=Storage::RenderPhase::returned)
        return R::fail(d,S::phase,0x1D4DD0);
    //Only the genuine older completed query/render may survive into this
    //update. Keep its selected rows/epochs intact until the later Camera call.
    std::shared_ptr<R> model;std::shared_ptr<const assets::OriginalUiGarageWattoAssets50KG> assets;
    if(!wattoModelIdentityMatches(owner,model,assets,d))return false;
    const auto& held=s.observers[wattoSlot];
    if(!held||held->wattoOwner!=&owner||held->wattoAssets!=assets||
       !liveOrExpiredWattoObserverCurrent(owner))return R::fail(d,S::phase,0x28EA2C);
    return update(wattoSlot,*model,d);
}
bool OriginalUiGarageVisibility50KG::removeWatto28EA80(OriginalUiGarageWattoOwner50KG& owner,D& d)noexcept {
    d={};OriginalUiGarageWattoOwner50KG::VisibilityBinding b;
    // Exact retained ownership suffices for unwind. Do not demand repaired
    // poses, finite bounds, a committed zone or completed widget stages.
    if(!current()||!owner.visibilityBinding(b)||b.visibility.get()!=this||
       !matchesColdConstruction(b.grant,b.sceneAssets)||!b.model||
       b.primary4C0!=&storage_->primary4C0.entity)
        return R::fail(d,S::input,0x28EA80);
    const auto& held=storage_->observers[wattoSlot];
    if(held&&(held->wattoOwner!=&owner||held->wattoAssets!=b.assets))return R::fail(d,S::phase,0x280920);
    return remove(wattoSlot,*b.model,d);
}
bool OriginalUiGarageVisibility50KG::normalSearchEntitiesRetired142590()const noexcept {
    if(!storage_||!sourceSceneOwner_||!searchEntitiesRetired_)return false;
    const auto& s=*storage_;
    if(!s.pageSearchRetirementOwner||s.pageSearchRetirementOwner!=s.attachedPage||
       s.pageSearchRetirementStarted!=s.attachedPage||s.sceneLive||s.prepared||!s.closing||
       s.pageRootRetiring||!s.grant||!s.grant->current()||
       !s.grant->garageVisibilitySourceRetired(*sourceSceneOwner_,*this,s.scene4BC))return false;
    for(const auto phase:s.nodeRetirement)if(phase!=Storage::NodeRetirement::complete)return false;
    return !s.primary4C0.entityLive&&!s.secondary4C4.entityLive&&!s.environmentLive;
}
bool OriginalUiGarageVisibility50KG::failedPageSearchEntitiesRetired142590(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    if(!storage_||!sourceSceneOwner_||!searchEntitiesRetired_||!page.partialSceneAttachmentFailureCurrent())return false;
    const auto& s=*storage_;
    if(s.failedPageSearchRetirementOwner!=&page||s.failedPageSearchRetirementStarted!=&page||
       s.attachedPage!=&page||s.pageSearchRetirementOwner||s.pageSearchRetirementStarted||
       s.cameraPage||s.camera||s.cameraObserver||s.cameraObserverRetired||
       s.sceneLive||s.prepared||!s.closing||s.pageRootRetiring||!s.grant||!s.grant->current()||
       !s.grant->garageVisibilitySourceRetired(*sourceSceneOwner_,*this,s.scene4BC))return false;
    for(const auto phase:s.nodeRetirement)if(phase!=Storage::NodeRetirement::complete)return false;
    return !s.primary4C0.entityLive&&!s.secondary4C4.entityLive&&!s.environmentLive;
}
bool OriginalUiGarageVisibility50KG::retirePageSearchEntities142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    return retirePageSearchEntitiesBody142590(page,false,d);
}
bool OriginalUiGarageVisibility50KG::retireFailedPageSearchEntities142590(
    const OriginalUiGaragePageOwner50KG& page,D& d)noexcept {
    if(!page.partialSceneAttachmentFailureCurrent())return R::fail(d,S::phase,0x27F180);
    return retirePageSearchEntitiesBody142590(page,true,d);
}
bool OriginalUiGarageVisibility50KG::retirePageSearchEntitiesBody142590(
    const OriginalUiGaragePageOwner50KG& page,bool failedPrefix,D& d)noexcept {
    d={};if(!storage_||!sourceSceneOwner_||closed_)return R::fail(d,S::phase,0x142590);
    auto& s=*storage_;using Stage=Storage::NodeRetirement;
    if(failedPrefix?(s.pageSearchRetirementStarted||s.pageSearchRetirementOwner):
        (s.failedPageSearchRetirementStarted||s.failedPageSearchRetirementOwner))return R::fail(d,S::phase,0x27F180);
    auto*& started=failedPrefix?s.failedPageSearchRetirementStarted:s.pageSearchRetirementStarted;
    auto*& owner=failedPrefix?s.failedPageSearchRetirementOwner:s.pageSearchRetirementOwner;
    if(owner)
        return owner==&page&&(failedPrefix?failedPageSearchEntitiesRetired142590(page):normalSearchEntitiesRetired142590())?
            true:R::fail(d,S::phase,0x1413E0);
    if(!s.grant||!s.grant->current()||s.attachedPage!=&page||
       !sourceSceneOwner_->sourceLoadReturned1FB59C()||
       (started&&started!=&page))
        return R::fail(d,S::phase,0x142590);
    if(!started){
        if(!current()||!s.environmentPrepared||!s.environmentTree443A98||
           !s.environmentTree443A98->emptyGarageQueryStateCurrent()||
           !s.primary4C0.payload.supportedLive()||!s.secondary4C4.payload.supportedLive()||!s.environmentPayload.supportedLive()||
           (s.renderPhase!=Storage::RenderPhase::none&&s.renderPhase!=Storage::RenderPhase::returned)||
           !s.grant->garagePageVisibilityRetirementCurrent(page,*sourceSceneOwner_,*this,s.scene4BC))
            return R::fail(d,S::phase,0x27F180);
        if(!pageSearchReferentsDeadBody27F180(page,failedPrefix,d))return false;
        started=&page;s.closing=true;
    }
    for(std::size_t end=3;end>0;--end){const auto ordinal=end-1;
        auto& stage=s.nodeRetirement[ordinal];if(stage==Stage::complete)continue;
        auto* domain=ordinal==2?nullptr:ordinal==1?&s.secondary4C4:&s.primary4C0;
        auto& c=domain?domain->entity:s.environmentNode443108;
        auto& payload=domain?domain->payload:s.environmentPayload;
        if(stage==Stage::live){
            if(!payload.supportedLive())return R::fail(d,S::phase,0x27F1AC);
            if(domain){
                if(!domain->entityLive||domain->first34!=&domain->lists[0]||
                   domain->second38!=&domain->lists[1])return R::fail(d,S::phase,0x27F1EC);
                for(const auto& list:domain->lists)
                    if(list.reference00!=2||!list.zoneReference||!list.sceneReference)
                        return R::fail(d,S::phase,0x104970);
            }else {
                const auto* tree=s.environmentTree443A98.get();
                if(!s.environmentLive||!tree||!tree->emptyGarageQueryStateCurrent()||tree->reference00_!=2||
                   s.environmentFirst34!=tree||s.environmentSecond38!=tree)
                    return R::fail(d,S::phase,0x27F1EC);
            }
            const auto step=s.grant->beginGaragePageSearchNode142590(page,*sourceSceneOwner_,*this,ordinal,c);
            if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
            stage=Stage::lastReference;
        }
        if(!s.grant->garagePageSearchNodeBaseCurrent(page,*sourceSceneOwner_,*this,ordinal,c))
            return R::fail(d,S::phase,0x27F180);
        if(stage==Stage::lastReference){
            //27F1AC/1D8 reached constructor-null portal30/exclusions3C.
            if(!payload.supportedLive())return R::fail(d,S::phase,0x27F1AC);
            stage=Stage::linksReleased;
        }
        if(stage==Stage::linksReleased){
            if(domain){auto& list=domain->lists[0];
                if(list.reference00!=2||!list.zoneReference||!list.sceneReference)
                    return R::fail(d,S::phase,0x27F200);
                --list.reference00;list.zoneReference=false;
            }else {
                auto& tree=*s.environmentTree443A98;
                if(tree.reference00_!=2)return R::fail(d,S::phase,0x27F200);
                --tree.reference00_;
            }
            stage=Stage::firstReleased;
        }
        if(stage==Stage::firstReleased){
            if(domain){auto& list=domain->lists[1];
                if(list.reference00!=2||!list.zoneReference||!list.sceneReference)
                    return R::fail(d,S::phase,0x27F21C);
                --list.reference00;list.zoneReference=false;
            }else if(!s.environmentTree443A98->closeGarageEmptyQuery2AF060())
                return R::fail(d,S::phase,0x2AF060);
            stage=Stage::secondReleased;
        }
        if(stage==Stage::secondReleased){
            if(!payload.supportedLive())return R::fail(d,S::phase,0x27F230);
            if(payload.scissorA0){
                if(payload.scissorA0->garageOwner_!=this||payload.scissorA0->garageRetired_)
                    return R::fail(d,S::phase,0x27F230);
                //Source free does not null A0. Native4014 may retain comparison
                //backing, but this precise lifetime is no longer readable.
                payload.scissorA0->garageRetired_=true;payload.scissorA0.reset();
            }
            stage=Stage::dataReleased;
        }
        if(stage==Stage::dataReleased){
            const auto step=s.grant->finishGaragePageSearchNode1413E0(page,*sourceSceneOwner_,*this,ordinal,c);
            if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
            if(domain){
                domain->entityLive=false;
                if(domain==&s.primary4C0)for(std::size_t i=selectedSlot;i<=wattoSlot;++i)
                    if(s.observers[i])s.observers[i]->targetNodeExpired=true;
            }else {s.environmentLive=false;s.environmentPrepared=false;}
            stage=Stage::complete;
        }
        //No source27F180 store clears dying34/38 or Entity sibling links.
    }
    if(!s.pageRootRetiring){
        const auto step=s.grant->beginGaragePageVisibility142590(page,*sourceSceneOwner_,*this,s.scene4BC);
        if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
        s.pageRootRetiring=true;
    }
    const auto step=s.grant->finishGaragePageVisibility1413E0(page,*sourceSceneOwner_,*this,s.scene4BC);
    if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
    s.pageRootRetiring=false;s.sceneLive=false;s.prepared=false;
    searchEntitiesRetired_=true;owner=&page;
    return (failedPrefix?failedPageSearchEntitiesRetired142590(page):normalSearchEntitiesRetired142590())?true:R::fail(d,S::phase,0x1413E0);
}
bool OriginalUiGarageVisibility50KG::sourceSearchEntitiesRetiredForPayload()const noexcept {
    if(!sourceSceneOwner_)return false;
    const auto* failedPage=sourceSceneOwner_->failedPageRetirementOwner_;
    // Separate completed destructor history, never a normal Camera receipt.
    return failedPage?failedPageSearchEntitiesRetired142590(*failedPage):normalSearchEntitiesRetired142590();
}
bool OriginalUiGarageVisibility50KG::retirePrimaryNodePointers26FF14(
    OriginalUiGaragePodScene50KG& scene,D& d)noexcept {
    d={};if(sourceSceneOwner_!=&scene||!sourceSearchEntitiesRetiredForPayload()||
       !scene.scenePayloadRetirementCurrent()||scene.visibility_.get()!=this||
       scene.retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::instanceMasterReleased)
        return R::fail(d,S::phase,0x26FF14);
    auto& s=*storage_;
    if(s.nodePointerReleaseStage||s.nodePointerExtent!=1||!s.primaryNodes4C0||
       s.primaryNodes4C0[0]!=&s.primary4C0.entity||!s.secondaryNodes4C4||
       s.secondaryNodes4C4[0]!=&s.secondary4C4.entity)return R::fail(d,S::phase,0x26FF14);
    s.primaryNodes4C0.reset();s.nodePointerReleaseStage=1;return true;
}
bool OriginalUiGarageVisibility50KG::retireSecondaryNodePointers26FF2C(
    OriginalUiGaragePodScene50KG& scene,D& d)noexcept {
    d={};if(sourceSceneOwner_!=&scene||!sourceSearchEntitiesRetiredForPayload()||
       !scene.scenePayloadRetirementCurrent()||scene.visibility_.get()!=this||
       scene.retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::primarySearchPointersReleased)
        return R::fail(d,S::phase,0x26FF2C);
    auto& s=*storage_;
    if(s.nodePointerReleaseStage!=1||s.primaryNodes4C0||!s.secondaryNodes4C4||
       s.secondaryNodes4C4[0]!=&s.secondary4C4.entity)return R::fail(d,S::phase,0x26FF2C);
    s.secondaryNodes4C4.reset();s.nodePointerReleaseStage=2;return true;
}
bool OriginalUiGarageVisibility50KG::sourceNodePointersReleased26FF34()const noexcept {
    return sourceSearchEntitiesRetiredForPayload()&&storage_->nodePointerReleaseStage==2&&
        !storage_->primaryNodes4C0&&!storage_->secondaryNodes4C4;
}
bool OriginalUiGarageVisibility50KG::clearSourceEnvironmentGlobal26FF54(
    OriginalUiGaragePodScene50KG& scene,D& d)noexcept {
    d={};if(sourceSceneOwner_!=&scene||!sourceNodePointersReleased26FF34()||
       !scene.scenePayloadRetirementCurrent()||scene.visibility_.get()!=this||
       scene.retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::collisionPointersReleased)
        return R::fail(d,S::phase,0x26FF54);
    auto& s=*storage_;const auto app=s.grant->application();
    if(!app||s.environmentGlobalCleared||!s.environmentGlobalPublished||s.environmentLive||
       !s.environmentTree443A98||s.environmentTree443A98->reference00_||
       app->visibility443A98_||app->environment443108_!=&s.environmentNode443108)
        return R::fail(d,S::phase,0x26FF54);
    app->environment443108_=nullptr;s.environmentGlobalCleared=true;return true;
}
bool OriginalUiGarageVisibility50KG::sourceEnvironmentGlobalCleared26FF54()const noexcept {
    if(!sourceNodePointersReleased26FF34()||!storage_->environmentGlobalCleared)return false;
    const auto app=storage_->grant->application();return app&&!app->environment443108_;
}
bool OriginalUiGarageVisibility50KG::close26FD90(D& d)noexcept {
    d={};if(closed_)return true;
    // Historical detached native cleanup cannot skip the joined Scene's
    // intervening Page/config/group lifetime obligations.
    if(sourceSceneOwner_)return R::fail(d,S::phase,0x1FA170);
    return retireSearchEntities142590(d)&&clearEnvironmentGlobal26FF54(d)&&releaseSceneLists26FF6C(d);
}
bool OriginalUiGarageVisibility50KG::retireSearchEntities142590(D& d)noexcept {
    d={};if(searchEntitiesRetired_)return true;
    if(!storage_){searchEntitiesRetired_=true;return true;}
    auto& s=*storage_;
    if(s.staticOwner)return R::fail(d,S::phase,0x27B650);
    if(s.collisionOwner)return R::fail(d,S::phase,0x26FF6C);
    if(!s.grant||!s.grant->current()||s.scene4BC.parent1C||s.scene4BC.previous10||s.scene4BC.next14)
        return R::fail(d,S::phase,0x26FD90);
    for(const auto& observer:s.observers)if(observer)return R::fail(d,S::phase,0x28EA80);
    for(const auto* domain:{&s.primary4C0,&s.secondary4C4})for(const auto& registration:domain->registrations)
        if(registration.owner||registration.slot||registration.encodedObject)return R::fail(d,S::phase,0x281490);
    for(const auto* domain:{&s.primary4C0,&s.secondary4C4}){
        if(domain->entityLive&&(domain->entity.reference00!=1||domain->entity.firstChild18||
           (domain->entity.parent1C&&domain->entity.parent1C!=&s.scene4BC)))return R::fail(d,S::phase,0x27F180);
        for(const auto& list:domain->lists)
            if(list.reference00!=std::uint64_t(list.sceneReference)+std::uint64_t(list.zoneReference))
                return R::fail(d,S::phase,0x104970);
    }
    s.closing=true;
    // The environment SearchNode was appended last. Its actual two tree
    //references retire in34/38 order before the Entity count is removed.
    if(s.environmentAttempted){
        auto* tree=s.environmentTree443A98.get();
        if(tree&&!tree->constructorStateOnly())return R::fail(d,S::phase,0x27F180);
        for(auto** slot:{&s.environmentFirst34,&s.environmentSecond38})if(*slot){
            if(*slot!=tree||!tree->reference00_)return R::fail(d,S::phase,0x104970);
            if(tree->reference00_==1){if(!tree->closeConstructor())return R::fail(d,S::phase,0x2AF060);}
            else --tree->reference00_;
            *slot=nullptr;
        }
        // Partial-constructor failure may still own the local reference.
        if(tree&&tree->reference00_&&!tree->closeConstructor())return R::fail(d,S::phase,0x2AF060);
        if(s.environmentLive){
            const auto step=s.grant->retireEmptyPrefix(s.environmentNode443108,K::searchNode27F120,
                s.environmentNode443108.parent1C);
            if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
            s.environmentLive=false;
        }
        s.environmentPrepared=false;
    }
    // Exact27F180 list34/38 release precedes that Entity's1413E0. Reverse
    //native prefix unwind repairs sibling links without fake detach/reparent.
    for(auto* domain:{&s.secondary4C4,&s.primary4C0}){
        if(!domain->entityLive)continue;
        for(auto& list:domain->lists)if(list.zoneReference){--list.reference00;list.zoneReference=false;}
        domain->first34=domain->second38=nullptr;
        const auto step=s.grant->retireEmptyPrefix(domain->entity,K::searchNode27F120,domain->entity.parent1C);
        if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
        domain->entityLive=false;
    }
    if(s.sceneLive){
        const auto step=s.grant->retireEmptyPrefix(s.scene4BC,K::entity141300,nullptr);
        if(!step.okay())return R::fail(d,S::phase,step.sourcePC);
        s.sceneLive=false;
    }
    s.prepared=false;searchEntitiesRetired_=true;return true;
}
bool OriginalUiGarageVisibility50KG::releaseSceneLists26FF6C(D& d)noexcept {
    d={};
    if(storage_&&storage_->environmentGlobalPublished&&!storage_->environmentGlobalCleared)
        return R::fail(d,S::phase,0x26FF54);
    if(closed_)return true;
    if(!searchEntitiesRetired_||sourceSceneOwner_)return R::fail(d,S::phase,0x26FF6C);
    if(!storage_){closed_=true;return true;}
    auto& s=*storage_;
    for(const auto* domain:{&s.primary4C0,&s.secondary4C4})for(const auto& list:domain->lists)
        if(list.zoneReference||list.reference00!=std::uint64_t(list.sceneReference))
            return R::fail(d,S::phase,0x104970);
    //Detached component compatibility only, never the joined Page/Scene path.
    //New native arrays still release their actual backing when this component
    //closes; this does NOT issue the source-normal Page retirement receipt.
    s.primaryNodes4C0.reset();s.secondaryNodes4C4.reset();s.nodePointerReleaseStage=2;
    for(auto* domain:{&s.primary4C0,&s.secondary4C4})for(unsigned method=0;method<2;++method){
        auto& list=domain->lists[method];
        if(list.sceneReference){--list.reference00;list.sceneReference=false;}
        if(list.reference00)std::terminate();
        std::vector<const C*>().swap(list.selected18);list.registry=nullptr;
        auto& array=method?
            (domain==&s.primary4C0?s.sceneLists4D0:s.sceneLists4D4):
            (domain==&s.primary4C0?s.sceneLists4C8:s.sceneLists4CC);
        array.reset();
    }
    s.primary4C0.registries.reset();s.secondary4C4.registries.reset();
    s.prepared=false;closed_=true;return true;
}
}
