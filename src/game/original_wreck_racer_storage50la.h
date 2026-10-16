#pragma once
#include "original_wreck_asset_inventory50la.h"
#include "original_wreck_forces50la.h"
#include "original_wreck_collection_tuning50la.h"
#include "original_wreck_shared_response50la.h"
#include "original_racer_model_packets50la.h"
#include "reset_visibility50ey.h"
#include <limits>
#include <exception>

namespace rr::game {
class OriginalWreckRacerStorage50LA;
// Native backing for ONE actual authored occurrence. Its body/gravity/Entity
// storage is not initialized by any source constructor merely by allocation.
// In particular it is not a disguised one-piece collection/group owner.
class OriginalWreckPieceStorage50LA final {
    friend class OriginalWreckRacerStorage50LA;
    OriginalWreckAssets50LA* assets_=nullptr; // owned by the adopted inventory
    std::size_t collectionOrdinal_=0,pieceOrdinal_=0,sceneCount_=0,componentCount_=0;
    OriginalWreckPhysical50LA physical_;
    OriginalWreckPieceFields50LA piece_;
    OriginalWreckGravity50LA gravity_;
    //165EC8/165ED0 retain separate native zone namespaces, consumed by
    //166750 visibility28E8A0 and166730/166740 collision120070.
    ResetVisibilityWorld50EY* visibilityWorldB4_=nullptr;
    LiveZoneWorld50EN* collisionWorldB8_=nullptr;
    ZoneLifecycle50CL* collisionLifecycleB8_=nullptr;
    CollectionRef50BX visibilityZoneB4_=0,collisionZoneB8_=0;
    bool setupZonesBound_=false;
    // Model/PSG Entity identity belongs ONLY to this canonical owner. Source
    // resource facets stay empty until their genuine load phases are reached.
    OriginalRacerModelSceneOwner50LA modelOwner_;
    rr::assets::OriginalStandardMaterialReferences50LA modelMaterials_;
    std::shared_ptr<rr::assets::OriginalStandardModelGeometry50LA> modelGeometry_;
    OriginalRacerModelPackets50LA modelPackets_;
    std::unique_ptr<OriginalWreckComponent50LA*[]> componentSlots_;
    OriginalWreckEffectFields50LA effects_;
    const WaterHeightProvider50JC* actualWaterB0_=nullptr;
    const WaterHeightGlobals50JC* actualWaterGlobals_=nullptr;
    std::unique_ptr<OriginalWreckComponent50LA[]> components_;
    // One receiver/query owner per actual occurrence. Declaration order keeps
    // these borrows alive through query-domain token revocation on destruction.
    OriginalWreckPieceQueryOwner50LA queryOwner_;
    OriginalWreckQueries50LA queries_;
public:
    OriginalWreckPieceStorage50LA()=default;
    OriginalWreckPieceStorage50LA(const OriginalWreckPieceStorage50LA&)=delete;
    OriginalWreckPieceStorage50LA& operator=(const OriginalWreckPieceStorage50LA&)=delete;
    OriginalWreckPieceStorage50LA(OriginalWreckPieceStorage50LA&&)=delete;
    OriginalWreckPieceStorage50LA& operator=(OriginalWreckPieceStorage50LA&&)=delete;
    std::size_t collectionOrdinal()const noexcept{return collectionOrdinal_;}
    std::size_t pieceOrdinal()const noexcept{return pieceOrdinal_;}
    const OriginalWreckAssets50LA& assets()const noexcept{return *assets_;}
    std::size_t sceneCount()const noexcept{return sceneCount_;}
    std::size_t componentCapacity()const noexcept{return componentCount_;}
    const OriginalWreckPieceQueryOwner50LA& queryReceiverBacking()const noexcept{return queryOwner_;}
    // Read-only cold inspection. No public constructor-completion flag or
    // mutable body/query/camera view exists at this stage.
    const OriginalWreckPhysical50LA& physicalBacking()const noexcept{return physical_;}
    const OriginalWreckComponent50LA* componentAt(std::size_t index)const noexcept {
        return index<componentCount_?componentSlots_[index]:nullptr;
    }
};
class OriginalWreckCollectionStorage50LA final {
    friend class OriginalWreckRacerStorage50LA;
    std::size_t ordinal_=0,count_=0;
    std::uint32_t sourceCount30_=0;
    std::size_t sourceSlots38_=0;
    OriginalWreckComponent50LA collectionComponent_,groupComponent_;
    std::unique_ptr<OriginalWreckPieceStorage50LA[]> pieces_;
    // Genuine planned pointer order for collection38, not yet a source
    // publication. Each slot names its distinct stable occurrence owner.
    std::unique_ptr<OriginalWreckPieceStorage50LA*[]> ordered38_;
    // Destroy group backing before every piece/component it may later borrow.
    OriginalWreckGroupArrays50LA group_;
public:
    OriginalWreckCollectionStorage50LA()=default;
    OriginalWreckCollectionStorage50LA(const OriginalWreckCollectionStorage50LA&)=delete;
    OriginalWreckCollectionStorage50LA& operator=(const OriginalWreckCollectionStorage50LA&)=delete;
    OriginalWreckCollectionStorage50LA(OriginalWreckCollectionStorage50LA&&)=delete;
    OriginalWreckCollectionStorage50LA& operator=(OriginalWreckCollectionStorage50LA&&)=delete;
    const OriginalWreckGroupArrays50LA& groupBacking()const noexcept{return group_;}
    const OriginalWreckComponent50LA& collectionComponentBacking()const noexcept{return collectionComponent_;}
    std::size_t ordinal()const noexcept{return ordinal_;}
    std::size_t plannedPieceCount()const noexcept{return count_;}
    //22C16C publishes each source array slot before its piece setup; Entity
    //append22C1B8 is a distinct later phase. This extent is not activation.
    std::size_t reachedSourcePieceSlotCount()const noexcept{return sourceSlots38_;}
    const OriginalWreckPieceStorage50LA* piece(std::size_t i)const noexcept {
        return ordered38_&&i<count_?ordered38_[i]:nullptr;
    }
    //2CC060's future primary is piece0; capacity is count-1. This describes
    //authoring only, not source group40/34 or aggregate construction completion.
    const OriginalWreckPieceStorage50LA* plannedPrimary()const noexcept{return piece(0);}
    std::size_t plannedNonprimaryCount()const noexcept{return count_?count_-1u:0u;}
};
enum class OriginalWreckRacerStorageStatus50LA : std::uint8_t {coldBacking,binding,inventory,capacity,allocation};
struct OriginalWreckRacerStorageResult50LA {
    OriginalWreckRacerStorageStatus50LA status=OriginalWreckRacerStorageStatus50LA::binding;
    std::size_t collectionOrdinal=0,pieceOrdinal=0,collections=0,pieces=0,componentCapacity=0;
};
// Slots supplied by the actual canonical world allocator, in collection-major,
// piece-major order. Each triple retains the source E0/E4/E8 order.
struct OriginalWreckPieceQuerySlots50LA {
    std::array<CollectionRef50BX,3> refs{};
    std::array<std::uint32_t,3> identities{};
};
enum class OriginalWreckOrderedQueryStatus50LA : std::uint8_t {
    coldPrepared,binding,capacity,allocation,queryPreparation
};
struct OriginalWreckOrderedQueryResult50LA {
    OriginalWreckOrderedQueryStatus50LA status=OriginalWreckOrderedQueryStatus50LA::binding;
    std::size_t collectionOrdinal=0,pieceOrdinal=0,preparedPieces=0;
    std::uint32_t sourcePC=0x165e90u;
    OriginalWreckQueryResult50LA query{};
};
// Genuine application identities/globals for the first reached source piece.
// Keys are the actual native addresses of collection and eight non-model
// components (piece,D0,D4,E0,DC,D8,E4,E8). Model/PSG keys belong to their owner.
struct OriginalWreckConstructorPrefixBinding50LA {
    std::uint64_t sessionGeneration=0,racerKey=0,collectionKey=0;
    const std::uint64_t* pieceComponentKeys=nullptr;
    std::size_t pieceComponentKeyCount=0;
    // These same application fields are retained through source retirement.
    std::uint32_t* actualObjectCount423F60=nullptr;
    const std::uint32_t* actualAttachmentLock423F58=nullptr;
    const OriginalWreckConstructorGlobals50LA* constructorGlobals=nullptr;
    const SpringVector4_48N* actualVector4431C0=nullptr;
    OriginalEntityTypeRegistry50LA* actualTypes383408=nullptr;
    const WaterHeightProvider50JC* const* actualWaterB0Field=nullptr;
    const WaterHeightGlobals50JC* actualWaterGlobals=nullptr;
};
// Actual source zone-field borrows, not inferred host/current-camera zones.
// B4/B8 select distinct visibility/collision namespaces; equal numeric refs
// never make those worlds interchangeable.
struct OriginalWreckSetupZones50LA {
    ResetVisibilityWorld50EY* visibility4C0=nullptr;
    const CollectionRef50BX* visibilityZoneField=nullptr;
    LiveZoneWorld50EN* collision4C4=nullptr;
    ZoneLifecycle50CL* collisionLifecycle=nullptr;
    const CollectionRef50BX* collisionZoneField=nullptr;
};
enum class OriginalWreckConstructorPrefixStatus50LA : std::uint8_t {
    binding,capacity,allocation,components,physical,publishedLifetimeRequired,stopped,modelLoadRequired,warmModelRequired,modelAppendRequired,geometryAdmission,pieceSetupRequired,queryPublicationRequired
};
struct OriginalWreckConstructorPrefixResult50LA {
    OriginalWreckConstructorPrefixStatus50LA status=OriginalWreckConstructorPrefixStatus50LA::binding;
    std::uint32_t sourcePC=0x22bfd0u;
    std::size_t initializedComponents=0;
    bool sourcePrefixCommitted=false;
    // Existing typed geometry loader is transactional. This diagnostic is
    // not a claim that its internal source phase was committed.
    std::uint32_t nativeLoaderDiagnosticPC=0;
};
enum class OriginalWreckPrefixRetirementStatus50LA : std::uint8_t {
    binding,attachmentLocked,topology,modelRetirementRequired,pieceRetirementRequired,modelResource,prefixRetired
};
struct OriginalWreckPrefixRetirementResult50LA {
    OriginalWreckPrefixRetirementStatus50LA status=OriginalWreckPrefixRetirementStatus50LA::binding;
    std::uint32_t sourcePC=0x142590u;
    std::size_t referencesReleased=0,objectsDestroyed=0;
};
// Own the complete original inventory and the nested native arrays together.
// The inventory outlives every borrower by declaration/destruction order.
// This class cannot publish an active racer374/death-camera view. It has no
// gameplay destructor callbacks. Its bounded constructor stops before any
// query publication; source retirement is not inferred from native storage release.
class OriginalWreckRacerStorage50LA final {
    struct Storage {
        std::unique_ptr<OriginalWreckAssetInventory50LA> inventory; // dies LAST
        std::uint64_t sessionGeneration=0,racerKey=0;
        std::size_t count=0,totalPieces=0,totalComponents=0;
        // Declared before collections so every borrowing query dies first.
        OriginalWreckSceneDomain50LA modelScenes;
        bool queryPreparationAttempted=false,queriesColdPrepared=false,queryBorrowsReleased=false;
        std::size_t preparedQueries=0;
        bool modelRetirementAttempted=false,pieceCollectionRetirementAttempted=false;
        bool e0ConstructorAttempted=false,e0PrefixRetired=false;
        bool constructorAttempted=false,retirementAttempted=false,freshModelLoadAttempted=false,modelAppendAttempted=false,pieceSetupAttempted=false;
        const OriginalWreckConstructorGlobals50LA* actualConstructorGlobals=nullptr;
        const SpringVector4_48N* actualGravityVector4431C0=nullptr;
        std::uint32_t* actualObjectCount423F60=nullptr;
        const std::uint32_t* actualAttachmentLock423F58=nullptr;
        OriginalWreckPrefixRetirementResult50LA retirementResult{};
        std::array<OriginalWreckComponent50LA*,maxOriginalWreckComponents50LA> prefixComponents{};
        OriginalWreckComponentDomain50LA prefixDomain{};
        OriginalWreckConstructorPrefixResult50LA prefixResult{};
        std::unique_ptr<OriginalWreckCollectionStorage50LA[]> collections;
        std::unique_ptr<OriginalWreckCollectionStorage50LA*[]> ordered374;
        ~Storage(){
            // Runs before collections die. Cold allocation is freely releasable;
            // reached source references require real104970/slot08 retirement.
            for(std::size_t i=0;i<prefixDomain.count;++i)
                if(!prefixComponents[i]||prefixComponents[i]->reference00)std::terminate();
        }
    };
    std::unique_ptr<Storage> storage_;
public:
    OriginalWreckRacerStorage50LA()=default;
    OriginalWreckRacerStorage50LA(const OriginalWreckRacerStorage50LA&)=delete;
    OriginalWreckRacerStorage50LA& operator=(const OriginalWreckRacerStorage50LA&)=delete;
    OriginalWreckRacerStorage50LA(OriginalWreckRacerStorage50LA&&)=delete;
    OriginalWreckRacerStorage50LA& operator=(OriginalWreckRacerStorage50LA&&)=delete;

    // Native cold transaction only. On failure, caller retains inventory and
    // all its exact asset addresses; on success it transfers exclusive ownership.
    // Existing nonowning asset borrows remain stable and must obey this owner's
    // lifetime. Identity values must be those of the actual retained racer.
    OriginalWreckRacerStorageResult50LA adoptCompleteInventory(
        std::unique_ptr<OriginalWreckAssetInventory50LA>& inventory,
        std::uint64_t sessionGeneration,std::uint64_t racerKey)noexcept {
        using S=OriginalWreckRacerStorageStatus50LA;
        OriginalWreckRacerStorageResult50LA r;
        auto fail=[&](S status){r.status=status;return r;};
        if(storage_||!inventory||!sessionGeneration||!racerKey)return fail(S::binding);
        if(!inventory->complete()||!inventory->modelSourcesBound())return fail(S::inventory);
        const auto& authoring=inventory->authoring();const auto& planned=inventory->collections();
        if(planned.empty()||planned.size()!=authoring.collections.size())return fail(S::inventory);
        try {
            auto next=std::make_unique<Storage>();next->count=planned.size();
            next->sessionGeneration=sessionGeneration;next->racerKey=racerKey;
            next->collections=std::make_unique<OriginalWreckCollectionStorage50LA[]>(next->count);
            next->ordered374=std::make_unique<OriginalWreckCollectionStorage50LA*[]>(next->count);
            for(std::size_t c=0;c<next->count;++c){
                r.collectionOrdinal=c+1u;r.pieceOrdinal=0;
                const auto& source=authoring.collections[c];auto& collection=next->collections[c];
                if(source.ordinal!=c+1u||planned[c].ordinal!=source.ordinal||source.pieces.empty()||
                   source.pieces.size()!=planned[c].pieces.size())return fail(S::inventory);
                collection.ordinal_=source.ordinal;collection.count_=source.pieces.size();
                const auto group=collection.group_.prepareCold(collection.count_-1u);
                if(group.status!=OriginalWreckGroupArrayStatus50LA::coldBacking)
                    return fail(group.status==OriginalWreckGroupArrayStatus50LA::allocation?S::allocation:S::inventory);
                collection.pieces_=std::make_unique<OriginalWreckPieceStorage50LA[]>(collection.count_);
                collection.ordered38_=std::make_unique<OriginalWreckPieceStorage50LA*[]>(collection.count_);
                if(next->totalComponents>std::numeric_limits<std::size_t>::max()-2u)return fail(S::capacity);
                next->totalComponents+=2u; // collection and group backing, still unconstructed
                for(std::size_t p=0;p<collection.count_;++p){
                    r.pieceOrdinal=p+1u;
                    auto* assets=inventory->pieceAssets(c,p);const auto& occurrence=planned[c].pieces[p];
                    if(!assets||!assets->prepared()||source.pieces[p].ordinal!=p+1u||
                       occurrence.collectionOrdinal!=c+1u||occurrence.pieceOrdinal!=p+1u||
                       assets->selected().ordinal!=c+1u||assets->selectedPiece().ordinal!=p+1u||
                       assets->selectedPiece().stem!=source.pieces[p].stem)return fail(S::inventory);
                    auto& piece=collection.pieces_[p];piece.assets_=assets;
                    piece.collectionOrdinal_=c+1u;piece.pieceOrdinal_=p+1u;
                    piece.sceneCount_=assets->graph().count;
                    if(!piece.sceneCount_||piece.sceneCount_>std::numeric_limits<std::size_t>::max()-8u)
                        return fail(S::capacity);
                    // piece + model/PSG entities + D0,D4,E0,DC,D8,E4,E8.
                    // PSG entities remain a separate hierarchy, not fabricated
                    // Entity children. This is capacity only; no141960 is called.
                    piece.componentCount_=piece.sceneCount_+8u;
                    if(next->totalComponents>std::numeric_limits<std::size_t>::max()-piece.componentCount_)
                        return fail(S::capacity);
                    // Eight actual non-model components: piece,D0,D4,E0,DC,D8,E4,E8.
                    // No dormant duplicate Model/PSG Entity row is allocated.
                    piece.components_=std::make_unique<OriginalWreckComponent50LA[]>(8u);
                    piece.componentSlots_=std::make_unique<OriginalWreckComponent50LA*[]>(piece.componentCount_);
                    piece.componentSlots_[0]=&piece.components_[0];
                    piece.componentSlots_[1]=piece.modelOwner_.wreckModelStorage();
                    for(std::size_t slot=0;slot<7u;++slot)
                        piece.componentSlots_[1u+piece.sceneCount_+slot]=&piece.components_[1u+slot];
                    if(assets->storage_->canonicalGraphBacking)return fail(S::inventory);
                    if(!piece.queryOwner_.bindCold(*assets,piece.physical_,piece.piece_,piece.components_[0],sessionGeneration))
                        return fail(S::binding);
                    next->totalComponents+=piece.componentCount_;++next->totalPieces;
                    collection.ordered38_[p]=&piece;
                }
                next->ordered374[c]=&collection;
            }
            // All allocations/identity checks finish before ownership transfer.
            // No original bytes, model transforms or runtime globals changed.
            // Exclusive cold adoption restages this SAME graph allocation as
            // backing before any canonical model/query construction. No caller
            // can select this route after published source objects exist.
            for(std::size_t c=0;c<next->count;++c)for(std::size_t p=0;p<next->collections[c].count_;++p){
                auto& asset=*next->collections[c].pieces_[p].assets_;
                for(auto& node:asset.storage_->nodes)node=SceneNode49P{};
                asset.storage_->cachedBoundsReady=false;asset.storage_->canonicalGraphBacking=true;
            }
            next->inventory=std::move(inventory);storage_=std::move(next);
            return {S::coldBacking,0,0,storage_->count,storage_->totalPieces,storage_->totalComponents};
        }catch(const std::bad_alloc&){return fail(S::allocation);}
    }
    // Reserve/stage the actual full inventory without running11B190/1253F0,
    // appending children, or publishing any canonical world definition. The
    // common world has no per-piece slots or external model domain; those are
    // supplied below by the actual slot allocator and this retained owner.
    // A reached failure retains its prepared prefix and is terminal. Its world
    // must outlive this owner or pass releaseUnpublishedQueries below.
    OriginalWreckOrderedQueryResult50LA prepareOrderedQueriesCold(
        const OriginalWreckQueryWorld50LA& commonWorld,
        const OriginalWreckPieceQuerySlots50LA* slots,std::size_t count)noexcept {
        using S=OriginalWreckOrderedQueryStatus50LA;OriginalWreckOrderedQueryResult50LA out;
        if(!storage_||storage_->queryPreparationAttempted||!slots||count!=storage_->totalPieces||
           commonWorld.modelScenes||commonWorld.groupPieceCapacity!=1u)return out;
        for(unsigned i=0;i<3;++i)if(commonWorld.refs[i]||commonWorld.identities[i])return out;
        if(!count||count>512u){out.status=S::capacity;return out;}
        // Check all inter-piece collisions BEFORE the first owner is staged.
        // prepareCold additionally authenticates actual world registrations,
        // existing identities, asset shapes and static type2 capacity.
        for(std::size_t p=0;p<count;++p)for(unsigned i=0;i<3;++i){
            if(!slots[p].refs[i]||slots[p].refs[i]>commonWorld.objectCount||!slots[p].identities[i])return out;
            for(std::size_t q=0;q<=p;++q)for(unsigned j=0;j<3;++j){
                if(q==p&&j>=i)break;
                if(slots[p].refs[i]==slots[q].refs[j]||slots[p].identities[i]==slots[q].identities[j])return out;
            }
        }
        storage_->queryPreparationAttempted=true;
        if(!storage_->modelScenes.prepareCold(storage_->sessionGeneration,count)){
            out.status=S::allocation;out.sourcePC=0x11bbc0u;return out;
        }
        std::size_t ordinal=0;
        for(std::size_t c=0;c<storage_->count;++c){
            auto& collection=storage_->collections[c];out.collectionOrdinal=c+1u;
            for(std::size_t p=0;p<collection.count_;++p,++ordinal){
                out.pieceOrdinal=p+1u;auto& piece=collection.pieces_[p];
                auto world=commonWorld;world.refs=slots[ordinal].refs;world.identities=slots[ordinal].identities;
                world.groupPieceCapacity=collection.count_;world.modelScenes=&storage_->modelScenes;
                // Model/PSG rows start at1. They are separate hierarchy rows,
                // not piece Entity children. The seven following components
                // retain D0,D4,E0,DC,D8,E4,E8 constructor/append order.
                const auto body=1u+piece.sceneCount_;
                const std::array<OriginalWreckComponent50LA*,3> queries{{
                    piece.componentSlots_[body+2u],piece.componentSlots_[body+5u],piece.componentSlots_[body+6u]}};
                out.query=piece.queries_.prepareBackingCold(piece.queryOwner_,world,*piece.componentSlots_[1],queries);
                out.sourcePC=out.query.sourcePC;
                if(out.query.status!=OriginalWreckQueryStatus50LA::complete){
                    out.status=S::queryPreparation;out.preparedPieces=storage_->preparedQueries;return out;
                }
                ++storage_->preparedQueries;
            }
        }
        storage_->queriesColdPrepared=true;out.status=S::coldPrepared;
        out.collectionOrdinal=0;out.pieceOrdinal=0;out.preparedPieces=storage_->preparedQueries;return out;
    }
    bool orderedQueriesColdPrepared()const noexcept {
        return storage_&&storage_->queriesColdPrepared&&!storage_->queryBorrowsReleased;
    }
    // Native unpublished-world borrow release only. Validate the entire prefix
    // first. No source unregister/destructor/effect receipt is manufactured.
    bool releaseUnpublishedQueries(const LiveZoneWorld50EN& actualWorld)noexcept {
        if(!storage_||!storage_->queryPreparationAttempted||storage_->queryBorrowsReleased)return false;
        for(std::size_t c=0;c<storage_->count;++c)for(std::size_t p=0;p<storage_->collections[c].count_;++p){
            if(storage_->collections[c].pieces_[p].setupZonesBound_)return false;
            const auto& queries=storage_->collections[c].pieces_[p].queries_;
            if(queries.prepared()&&!queries.canReleaseUnpublishedWorld(actualWorld))return false;
        }
        for(std::size_t c=0;c<storage_->count;++c)for(std::size_t p=0;p<storage_->collections[c].count_;++p){
            auto& queries=storage_->collections[c].pieces_[p].queries_;
            if(queries.prepared()&&!queries.releaseUnpublishedWorld(actualWorld))return false;
        }
        if(storage_->modelScenes.prepared()&&!storage_->modelScenes.closeAfterNativeRevocation())return false;
        storage_->queryBorrowsReleased=true;storage_->queriesColdPrepared=false;return true;
    }
    // Original ordered construction may not skip piece1's unresolved suffix
    // to reach another piece/collection. This canonical prefix reaches fresh
    // PSG hierarchy loading, then stops BEFORE material/resource/packet load.
    // A warm cache hit stops before source geometry acquisition instead.
    OriginalWreckConstructorPrefixResult50LA constructFirstOrderedPrefix(
        const OriginalWreckConstructorPrefixBinding50LA& binding)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckConstructorPrefixResult50LA out;
        if(!orderedQueriesColdPrepared()||!storage_->count)return out;
        if(storage_->constructorAttempted){out=storage_->prefixResult;out.status=S::stopped;return out;}
        auto& collection=storage_->collections[0];auto& piece=collection.pieces_[0];
        const auto* vectors=storage_->inventory->constructorVectors();
        if(binding.sessionGeneration!=storage_->sessionGeneration||binding.racerKey!=storage_->racerKey||
           !binding.collectionKey||binding.collectionKey==storage_->racerKey||!binding.pieceComponentKeys||
           binding.pieceComponentKeyCount!=8u||!binding.actualObjectCount423F60||
           !binding.actualAttachmentLock423F58||!binding.constructorGlobals||!binding.actualVector4431C0||
           !binding.actualTypes383408||!vectors||
           !binding.actualWaterB0Field||(*binding.actualWaterB0Field&&!binding.actualWaterGlobals))return out;
        const auto body=1u+piece.sceneCount_;
        // Domain includes collection plus the initialized piece prefix only.
        if(body+3u>maxOriginalWreckComponents50LA){out.status=S::capacity;return out;}
        if(binding.collectionKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&collection.collectionComponent_))||
           storage_->racerKey==static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(piece.componentSlots_[1])))return out;
        for(std::size_t i=0;i<8u;++i){
            const auto key=binding.pieceComponentKeys[i];
            if(key!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&piece.components_[i])))return out;
            if(!key||key==binding.collectionKey||key==storage_->racerKey)return out;
            for(std::size_t j=0;j<i;++j)if(key==binding.pieceComponentKeys[j])return out;
        }
        storage_->constructorAttempted=true;
        storage_->actualConstructorGlobals=binding.constructorGlobals;
        storage_->actualGravityVector4431C0=binding.actualVector4431C0;
        storage_->actualObjectCount423F60=binding.actualObjectCount423F60;
        storage_->actualAttachmentLock423F58=binding.actualAttachmentLock423F58;
        auto& domain=storage_->prefixDomain;
        domain={storage_->sessionGeneration,storage_->prefixComponents.data(),0};
        auto finish=[&](S status,std::uint32_t pc){
            storage_->prefixResult={status,pc,domain.count,domain.count!=0};return storage_->prefixResult;
        };
        auto initialize=[&](OriginalWreckComponent50LA& component,K kind,std::uint64_t key){
            const auto result=initializeOriginalWreckComponent50LA(component,kind,storage_->sessionGeneration,
                key,true,*binding.actualObjectCount423F60);
            if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
            storage_->prefixComponents[domain.count++]=&component;return true;
        };
        if(!initialize(collection.collectionComponent_,K::collection22BF00,binding.collectionKey))
            return finish(S::components,0x22bf0cu);
        collection.sourceCount30_=static_cast<std::uint32_t>(collection.count_);
        if(!initialize(piece.components_[0],K::piece2CB4B0,binding.pieceComponentKeys[0]))
            return finish(S::components,0x1659ccu);
        initializeOriginalWreckPieceFields50LA(piece.piece_,*binding.constructorGlobals);
        piece.effects_.dustActive178=0;piece.effects_.dustTime17C=0.f;
        collection.sourceSlots38_=1; //22C16C, before setup2CB5A0/165C30.
        std::memcpy(piece.piece_.authored50.v,vectors->position434830.data(),16);
        std::memcpy(piece.piece_.look60.v,vectors->look434860.data(),16);
        std::memcpy(piece.piece_.up70.v,vectors->up434850.data(),16);
        piece.actualWaterB0_=*binding.actualWaterB0Field;piece.actualWaterGlobals_=binding.actualWaterGlobals;
        auto graph=piece.assets_->graph();
        std::array<SceneNodeIndex49P,rr::assets::maxModelNodes49O> rows{};
        for(std::size_t row=0;row<piece.sceneCount_;++row)rows[row]=static_cast<SceneNodeIndex49P>(row);
        OriginalRacerModelSceneBinding50LA modelBinding{storage_->sessionGeneration,
            binding.actualObjectCount423F60,binding.actualAttachmentLock423F58,binding.actualTypes383408,
            piece.assets_->model14C(),rows.data(),static_cast<std::uint32_t>(piece.sceneCount_)};
        auto collectRows=[&](){
            for(std::size_t row=0;row<piece.sceneCount_;++row){
                auto* actual=piece.modelOwner_.wreckRowStorage(static_cast<std::uint32_t>(row));
                if(!actual||!actual->reference00)continue;
                piece.componentSlots_[1u+row]=actual;
                bool present=false;for(std::size_t i=0;i<domain.count;++i)present=present||domain.objects[i]==actual;
                if(!present)storage_->prefixComponents[domain.count++]=actual;
            }
        };
        auto modelResult=piece.modelOwner_.constructDirectRoot2403A0(modelBinding,graph,true);
        collectRows();if(!modelResult.okay())return finish(S::components,modelResult.sourcePC);
        modelResult=piece.modelOwner_.prepareDirectRoot26E934(graph,*piece.assets_->standardModelPayload()->member());
        if(!modelResult.okay())return finish(S::components,modelResult.sourcePC);
        const auto file=piece.assets_->modelFileIdentity();
        rr::assets::OriginalStandardModelFileView50LA source;
        rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
        const auto* resolver=storage_->inventory->modelResolverIdentity();
        if(!resolver||!resolver->modelFile(file,source,diagnostic)||!source.member||!source.bytes||
           *source.bytes!=*piece.assets_->standardModelPayload()->originalBytes())
            return finish(S::modelLoadRequired,0x26e9acu);
        // No source geometry retain/clone is faked. Even the childless warm
        // continuation remains closed until its actual prototype is bound.
        if(source.geometry240110)return finish(S::warmModelRequired,0x26e9f0u);
        modelResult=piece.modelOwner_.loadDirectRootSourceRows28CAC0(graph,piece.assets_->hierarchy());
        collectRows();if(!modelResult.okay())return finish(S::components,modelResult.sourcePC);
        // Next source phase26EE1C is real material acquisition. Model append,
        // authored-position extraction/reset, D0 and D4 follow packet loading.
        return finish(S::modelLoadRequired,0x26ee1cu);
    }
    // Continue ONLY the fresh first occurrence already stopped at26EE1C.
    // The actual application domain and36B970 prefix are caller-owned inputs;
    // inventory binding pins both resolver and material-table identities.
    OriginalWreckConstructorPrefixResult50LA loadFirstFreshModel26EE1C(
        std::uint64_t sessionGeneration,std::uint64_t racerKey,
        rr::assets::OriginalStandardMaterialDomain50LA& application,
        const rr::assets::OriginalStandardMaterialPath50LA& path)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!storage_||!storage_->constructorAttempted||storage_->freshModelLoadAttempted||
           sessionGeneration!=storage_->sessionGeneration||racerKey!=storage_->racerKey||
           storage_->prefixResult.status!=S::modelLoadRequired||storage_->prefixResult.sourcePC!=0x26ee1cu||
           !application.prepared()||!path.actualPrefix36B970||
           application.resolver3E1A70().get()!=storage_->inventory->modelResolverIdentity()||
           application.materials382B38().get()!=storage_->inventory->modelMaterialsIdentity())return rejected;
        auto& piece=storage_->collections[0].pieces_[0];
        if(!piece.modelOwner_.loadedFreshRows28CAC0()||piece.modelMaterials_.started()||piece.modelGeometry_||
           piece.modelOwner_.fields().geometry1B4||piece.modelOwner_.fields().sourceRows1B8||
           piece.modelOwner_.fields().packets1F0)return rejected;
        storage_->freshModelLoadAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){
            storage_->prefixResult={status,pc,storage_->prefixDomain.count,true};return storage_->prefixResult;
        };
        auto& fields=piece.modelOwner_.fields();
        const auto& payload=piece.assets_->standardModelPayload();
        // This native suspension boundary must still describe the same fresh
        // cache miss. A changed association cannot switch this loaded hierarchy
        // onto the source warm-clone branch or acquire extra material refs.
        rr::assets::OriginalStandardModelFileView50LA source;
        rr::assets::OriginalStandardTextureDiagnostic50LA sourceDiagnostic;
        if(!application.resolver3E1A70()->modelFile(piece.assets_->modelFileIdentity(),source,sourceDiagnostic)||
           !source.bytes||!source.member||source.geometry240110||
           *source.bytes!=*payload->originalBytes())return finish(S::stopped,0x26ee1cu);
        rr::assets::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
        const bool acquired=piece.modelMaterials_.acquire26E640(*payload,application.materials382B38(),path,materialDiagnostic);
        // Preserve26E670/26E684 even if the first lookup fails. Allocation
        // identity does not authorize reading an unwritten material slot.
        if(piece.modelMaterials_.started()){
            fields.materialCount1C0=static_cast<std::int32_t>(*piece.modelMaterials_.count1C0());
            fields.materials1C4=piece.modelMaterials_.allocationBase1C4();
        }
        if(!acquired)return finish(S::modelLoadRequired,materialDiagnostic.sourcePC);
        auto modelResult=piece.modelOwner_.loadQuantization240570(*payload->scene());
        if(!modelResult.okay())return finish(S::modelLoadRequired,modelResult.sourcePC);
        try{piece.modelGeometry_=std::make_shared<rr::assets::OriginalStandardModelGeometry50LA>();}
        catch(...){return finish(S::allocation,0x26ee38u);}
        rr::assets::OriginalStandardResourceDiagnostic50LA resourceDiagnostic;
        // Existing typed geometry owner includes240210 payload construction
        // and prototype26EE74. Its successful source reference is real, not
        // shared_ptr::use_count; its internal unsupported-input failure stays
        // a terminal bounded loader failure, never a completed resource.
        if(!piece.modelGeometry_->construct240110(payload,application.resolver3E1A70(),
                piece.assets_->modelFileIdentity(),&piece.modelOwner_,resourceDiagnostic)){
            finish(S::geometryAdmission,0x26ee38u);
            storage_->prefixResult.nativeLoaderDiagnosticPC=resourceDiagnostic.sourcePC;
            return storage_->prefixResult;
        }
        fields.geometry1B4=piece.modelGeometry_.get();
        rr::assets::OriginalStandardTextureDiagnostic50LA cacheDiagnostic;
        if(!application.resolver3E1A70()->publishGeometry1326A0(piece.assets_->modelFileIdentity(),
                piece.modelGeometry_,cacheDiagnostic))return finish(S::modelLoadRequired,cacheDiagnostic.sourcePC);
        // Source26EE90 closes its transient reader. The retained immutable
        // native payload has no open reader or guest allocation to reproduce.
        modelResult=piece.modelOwner_.allocateSourceRowTable26EE9C();
        if(!modelResult.okay())return finish(S::modelLoadRequired,modelResult.sourcePC);
        const auto packets=piece.modelPackets_.construct240740(piece.modelOwner_,*piece.modelGeometry_,false);
        if(!packets.okay())return finish(S::modelLoadRequired,packets.sourcePC);
        //165CD4 returns before165CF0 appends this exact model. Body/gravity,
        // initial query histories, effects and publication remain unreached.
        return finish(S::modelAppendRequired,0x165cf0u);
    }
    //165CF0..165D8C: append the actual loaded model, then consume its
    // current local translation. No inventory preview substitutes for28B180.
    OriginalWreckConstructorPrefixResult50LA appendFirstModel165CF0(
        std::uint64_t sessionGeneration,std::uint64_t racerKey)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!storage_||storage_->modelAppendAttempted||
           sessionGeneration!=storage_->sessionGeneration||racerKey!=storage_->racerKey||
           storage_->prefixResult.status!=S::modelAppendRequired||storage_->prefixResult.sourcePC!=0x165cf0u||
           !storage_->actualAttachmentLock423F58)return rejected;
        auto& piece=storage_->collections[0].pieces_[0];
        const auto* vectors=storage_->inventory->constructorVectors();
        if(!vectors||!piece.modelOwner_.fields().packets1F0||
           !piece.modelOwner_.fields().geometry1B4||!piece.modelMaterials_.complete())return rejected;
        storage_->modelAppendAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){
            storage_->prefixResult={status,pc,storage_->prefixDomain.count,true};return storage_->prefixResult;
        };
        const auto appended=appendOriginalWreckComponent50LA(storage_->prefixDomain,
            *piece.componentSlots_[0],*piece.componentSlots_[1],0u,*storage_->actualAttachmentLock423F58);
        if(appended.status!=OriginalWreckComponentStatus50LA::complete)
            return finish(S::components,appended.sourcePC);
        auto graph=piece.assets_->graph();const auto model=piece.assets_->model14C();
        // Source compares X, then Y, then Z to433B10. W never participates.
        bool sentinel=true;
        for(unsigned lane=0;lane<3u&&sentinel;++lane)
            sentinel=piece.piece_.authored50.v[lane]==vectors->zero433B10[lane];
        if(sentinel){
            //28B180 copies local F0/F4/F8 only, without refreshing world100.
            const auto& local=graph.nodes[static_cast<std::int32_t>(model)].localC0;
            std::memcpy(piece.piece_.authored50.v,local.v+12,3u*sizeof(float));
        }
        SceneDisplacement49P zero;
        std::memcpy(zero.v,vectors->zero433B10.data(),sizeof zero.v);
        //28AB90 preserves localFC/basis and performs actual28C4F0/289A20
        // invalidation. The authored50 copy and append survive a rejection.
        if(!setLocalTranslation49P(graph,model,zero))return finish(S::physical,0x165d8cu);
        //165DA8 calls165E90. The separate setup continuation admits actual
        // B4/B8 zone/world identities before entering D0/gravity.
        return finish(S::pieceSetupRequired,0x165e90u);
    }
    // Reached165E90 setup: retain its actual zone inputs, then construct and
    // append D0/D4 in source order. No zone membership is published here.
    OriginalWreckConstructorPrefixResult50LA setupFirstBody165E90(
        std::uint64_t sessionGeneration,std::uint64_t racerKey,
        const OriginalWreckSetupZones50LA& zones)noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!storage_||storage_->pieceSetupAttempted||!orderedQueriesColdPrepared()||
           sessionGeneration!=storage_->sessionGeneration||racerKey!=storage_->racerKey||
           storage_->prefixResult.status!=S::pieceSetupRequired||storage_->prefixResult.sourcePC!=0x165e90u||
           !storage_->actualConstructorGlobals||!storage_->actualGravityVector4431C0||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||
           !zones.visibility4C0||!zones.visibilityZoneField||!zones.collision4C4||
           !zones.collisionLifecycle||!zones.collisionZoneField)return rejected;
        auto& piece=storage_->collections[0].pieces_[0];
        const auto& queryWorld=piece.queries_.worldBinding();
        if(zones.collision4C4!=queryWorld.world||zones.collisionLifecycle!=queryWorld.lifecycle||
           &zones.visibility4C0->lifecycle==zones.collisionLifecycle||
           &zones.visibility4C0->portals==zones.collision4C4||
           !*zones.visibilityZoneField||*zones.visibilityZoneField>zones.visibility4C0->lifecycle.zoneCount||
           *zones.visibilityZoneField>zones.visibility4C0->portals.zoneCount||
           !*zones.collisionZoneField||*zones.collisionZoneField>zones.collisionLifecycle->zoneCount||
           *zones.collisionZoneField>zones.collision4C4->zoneCount||piece.setupZonesBound_||
           storage_->prefixDomain.count+2u>maxOriginalWreckComponents50LA)return rejected;
        storage_->pieceSetupAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){
            storage_->prefixResult={status,pc,storage_->prefixDomain.count,true};return storage_->prefixResult;
        };
        //141C20 at165EBC is the literal identity return, with no side effect.
        piece.visibilityWorldB4_=zones.visibility4C0;piece.visibilityZoneB4_=*zones.visibilityZoneField;
        piece.collisionWorldB8_=zones.collision4C4;piece.collisionLifecycleB8_=zones.collisionLifecycle;
        piece.collisionZoneB8_=*zones.collisionZoneField;piece.setupZonesBound_=true;
        const auto body=1u+piece.sceneCount_;
        auto initialize=[&](OriginalWreckComponent50LA& component,K kind){
            const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&component));
            const auto result=initializeOriginalWreckComponent50LA(component,kind,storage_->sessionGeneration,
                key,true,*storage_->actualObjectCount423F60);
            if(result.status!=OriginalWreckComponentStatus50LA::complete)return false;
            storage_->prefixComponents[storage_->prefixDomain.count++]=&component;return true;
        };
        auto append=[&](OriginalWreckComponent50LA& component,std::uint32_t mask){
            return appendOriginalWreckComponent50LA(storage_->prefixDomain,*piece.componentSlots_[0],component,
                mask,*storage_->actualAttachmentLock423F58).status==OriginalWreckComponentStatus50LA::complete;
        };
        auto& d0=*piece.componentSlots_[body];auto& d4=*piece.componentSlots_[body+1u];
        if(!initialize(d0,K::body167920)||
           !initializeOriginalWreckBody50LA(piece.physical_,d0,*storage_->actualConstructorGlobals))
            return finish(S::physical,0x165ee0u);
        //D0 slot30 is141C20;1652E0 binds the SAME model cache without a
        //forced refresh, followed by actual320=1 and1653C0(0).
        if(!bindOriginalWreckBodyModel50LA(piece.physical_,piece.assets_->graph(),piece.assets_->model14C()))
            return finish(S::physical,0x165f10u);
        if(!append(d0,0xffffffffu))return finish(S::components,0x165f3cu);
        if(!initialize(d4,K::gravity282670)||
           !initializeOriginalWreckGravity50LA(piece.gravity_,piece.physical_,d4,*storage_->actualGravityVector4431C0))
            return finish(S::physical,0x165f58u);
        if(!append(d4,0u))return finish(S::components,0x165f94u);
        //Subsequent source filename scratch consumes the already retained
        //authentic COL member. Stop before E0 allocation/construction166018.
        return finish(S::publishedLifetimeRequired,0x166018u);
    }
    // Actual E0 Entity and11B190/11B570 setup, stopping before11BBC0.
    // No model-domain row or source shape allocation is claimed here.
    OriginalWreckConstructorPrefixResult50LA constructFirstE0To11BBC0()noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckConstructorPrefixResult50LA rejected;
        if(!storage_||storage_->e0ConstructorAttempted||storage_->retirementAttempted||
           storage_->prefixResult.status!=S::publishedLifetimeRequired||storage_->prefixResult.sourcePC!=0x166018u||
           !storage_->actualObjectCount423F60||!orderedQueriesColdPrepared())return rejected;
        auto& piece=storage_->collections[0].pieces_[0];
        if(!piece.queries_.canConstructE0Prefix11B190()||
           storage_->prefixDomain.count==maxOriginalWreckComponents50LA)return rejected;
        auto& e0=*piece.componentSlots_[3u+piece.sceneCount_];
        storage_->e0ConstructorAttempted=true;
        auto finish=[&](S status,std::uint32_t pc){
            storage_->prefixResult={status,pc,storage_->prefixDomain.count,true};return storage_->prefixResult;
        };
        const auto initialized=initializeOriginalWreckComponent50LA(e0,K::collision11B190,
            storage_->sessionGeneration,static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&e0)),
            true,*storage_->actualObjectCount423F60);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)return finish(S::components,initialized.sourcePC);
        storage_->prefixComponents[storage_->prefixDomain.count++]=&e0;
        if(!piece.queries_.constructE0Prefix11B190(e0))return finish(S::components,0x11b190u);
        return finish(S::queryPublicationRequired,0x166074u);
    }
    // Unlinked E0 has not reached append166098. Its real104970->11B4A0
    // retirement must precede the earlier D4/D0/model disposal cursor.
    bool retireFirstUnpublishedE0()noexcept {
        using S=OriginalWreckConstructorPrefixStatus50LA;
        if(!storage_||!storage_->e0ConstructorAttempted)return false;
        if(storage_->e0PrefixRetired)return true;
        if(storage_->retirementAttempted||storage_->prefixResult.status!=S::queryPublicationRequired||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||
           *storage_->actualAttachmentLock423F58)return false;
        auto& piece=storage_->collections[0].pieces_[0];auto& e0=*piece.componentSlots_[3u+piece.sceneCount_];
        if(e0.reference00!=1u||e0.parent1C||e0.firstChild18||e0.previous10||e0.next14||
           !piece.queries_.canRetireE0Prefix11B4A0(e0))return false;
        --e0.reference00;
        if(!piece.queries_.retireE0Prefix11B4A0(e0,*storage_->actualObjectCount423F60))std::terminate();
        storage_->e0PrefixRetired=true;return true;
    }
    // Reached142590 child suffix of the complete pre-E0 constructor prefix.
    // This does not pretend the unjoined model destructor or enclosing piece
    // has retired. Original sibling/reference/count writes survive the stop.
    OriginalWreckPrefixRetirementResult50LA retireFirstOrderedPrefixChildren()noexcept {
        using S=OriginalWreckPrefixRetirementStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckPrefixRetirementResult50LA out;
        if(!storage_||!storage_->constructorAttempted||
           (storage_->prefixResult.status!=OriginalWreckConstructorPrefixStatus50LA::publishedLifetimeRequired&&
            !(storage_->prefixResult.status==OriginalWreckConstructorPrefixStatus50LA::queryPublicationRequired&&storage_->e0PrefixRetired))||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58)return out;
        if(storage_->retirementAttempted)return storage_->retirementResult;
        if(*storage_->actualAttachmentLock423F58){out.status=S::attachmentLocked;out.sourcePC=0x1425a8u;return out;}
        auto& piece=storage_->collections[0].pieces_[0];const auto body=1u+piece.sceneCount_;
        auto& root=*piece.componentSlots_[0];auto& model=*piece.componentSlots_[1];
        auto& d0=*piece.componentSlots_[body];auto& gravity=*piece.componentSlots_[body+1u];
        // No generic source destructor is admitted: only this exact private,
        // never-published constructor route has known NULL body28C and the
        // one-target gravity allocation represented by gravity_.
        std::array<OriginalWreckComponent50LA*,maxOriginalWreckComponents50LA> liveObjects{};
        OriginalWreckComponentDomain50LA liveDomain{storage_->sessionGeneration,liveObjects.data(),0};
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i){
            auto* object=storage_->prefixComponents[i];
            if(storage_->e0PrefixRetired&&object==piece.componentSlots_[body+2u]){
                if(object->reference00){out.status=S::topology;return out;}
                continue;
            }
            liveObjects[liveDomain.count++]=object;
        }
        if(!original_wreck_component_detail50LA::domain(liveDomain)||
           root.parent1C||root.previous10||root.next14||root.firstChild18!=&model||
           model.parent1C!=&root||model.previous10||model.next14!=&d0||
           d0.parent1C!=&root||d0.previous10!=&model||d0.next14!=&gravity||d0.firstChild18||
           gravity.parent1C!=&root||gravity.previous10!=&d0||gravity.next14||gravity.firstChild18||
           d0.kind!=K::body167920||gravity.kind!=K::gravity282670||
           piece.physical_.linkedCount268||piece.gravity_.target50!=&piece.physical_||
           piece.gravity_.count30!=1u||piece.gravity_.capacity34!=1u||
           piece.queries_.modelSceneIdentity().value){out.status=S::topology;return out;}
        storage_->retirementAttempted=true;
        if(storage_->e0PrefixRetired){out.referencesReleased=1u;out.objectsDestroyed=1u;}
        //1424E0 tail recursion first severs predecessor.next and self.next.
        //142590 then repairs adjacent links and104970 decrements exactly once.
        auto releaseLeaf=[&](OriginalWreckComponent50LA& leaf){
            if(leaf.previous10)leaf.previous10->next14=nullptr;
            leaf.next14=nullptr;
            if(leaf.parent1C&&leaf.parent1C->firstChild18==&leaf)
                leaf.parent1C->firstChild18=leaf.next14;
            --leaf.reference00;++out.referencesReleased;
            if(!leaf.reference00){
                //2826D0 frees only its pointer-array backing, not the borrowed
                //body.1679D0->165260 sees actual constructor28C=NULL. Neither
                //route clears canonical numeric fields or stale source links.
                // Their final1413E0 transitions the type and decrements423F60.
                if(leaf.kind==K::body167920)leaf.kind=K::body164ED0;
                // The one-target native gravity slot is embedded, so source
                // pointer-array disposal needs no host heap allocation/free.
                leaf.kind=K::entity141300;--*storage_->actualObjectCount423F60;
                ++out.objectsDestroyed;
            }
        };
        releaseLeaf(gravity);releaseLeaf(d0);
        // The separate loaded-model continuation owns the next actual
        // slot10/slot08 and PSG/cache phases. Piece/collection stay live.
        out.status=S::modelRetirementRequired;out.sourcePC=0x1425fcu;
        storage_->retirementResult=out;return out;
    }
    // Continue tail-first142590 only after D4/D0 really retired. This is
    // the actual loaded model240450/26E460/2896D0 path, not null-resource cleanup.
    OriginalWreckPrefixRetirementResult50LA retireFirstLoadedModel()noexcept {
        using S=OriginalWreckPrefixRetirementStatus50LA;
        OriginalWreckPrefixRetirementResult50LA rejected;
        if(!storage_||!storage_->retirementAttempted)return rejected;
        if(storage_->modelRetirementAttempted)return storage_->retirementResult;
        if(storage_->retirementResult.status!=S::modelRetirementRequired||
           !storage_->actualAttachmentLock423F58||!storage_->actualObjectCount423F60)return rejected;
        auto& piece=storage_->collections[0].pieces_[0];const auto body=1u+piece.sceneCount_;
        auto& root=*piece.componentSlots_[0];auto& model=*piece.componentSlots_[1];
        if(piece.componentSlots_[body]->reference00||piece.componentSlots_[body+1u]->reference00||
           root.firstChild18!=&model||model.parent1C!=&root||model.previous10||model.next14||
           piece.queries_.modelSceneIdentity().value||!piece.modelGeometry_||
           !piece.modelGeometry_->constructed240110()||!piece.modelMaterials_.complete()||
           !piece.modelPackets_.constructed240740())return rejected;
        auto graph=piece.assets_->graph();
        // The canonical model owner adds its own Entity once. Retired D0/D4
        // and owned PSG rows do not belong in its live outside Entity domain.
        std::array<OriginalWreckComponent50LA*,2> outsideObjects{{
            &storage_->collections[0].collectionComponent_,&root}};
        const OriginalWreckComponentDomain50LA outside{storage_->sessionGeneration,outsideObjects.data(),outsideObjects.size()};
        auto scene=piece.modelOwner_.preflightScene2896D0(graph);
        if(!scene.okay()){rejected.sourcePC=scene.sourcePC;return rejected;}
        scene=piece.modelOwner_.preflightModelReference142590(outside);
        if(!scene.okay()){rejected.sourcePC=scene.sourcePC;return rejected;}
        if(!piece.modelPackets_.preflight240450().okay()||!piece.modelMaterials_.canRelease26E4C0()||
           !piece.modelGeometry_->canReleaseForModel(&piece.modelOwner_))return rejected;
        storage_->modelRetirementAttempted=true;
        auto out=storage_->retirementResult;
        auto finish=[&](S status,std::uint32_t pc){
            out.status=status;out.sourcePC=pc;storage_->retirementResult=out;return out;
        };
        scene=piece.modelOwner_.releaseModelReference142590(outside);
        if(!scene.okay())return finish(S::topology,scene.sourcePC);
        ++out.referencesReleased;
        const auto packets=piece.modelPackets_.release240450();
        if(!packets.okay())return finish(S::modelResource,packets.sourcePC);
        rr::assets::OriginalStandardMaterialDiagnostic50LA material;
        if(!piece.modelMaterials_.release26E4C0(material))return finish(S::modelResource,material.sourcePC);
        rr::assets::OriginalStandardResourceDiagnostic50LA geometry;
        if(!piece.modelGeometry_->release26E598(&piece.modelOwner_,piece.modelMaterials_,geometry))
            return finish(S::modelResource,geometry.sourcePC);
        scene=piece.modelOwner_.freeSourceRowTable26E5CC();
        if(!scene.okay())return finish(S::modelResource,scene.sourcePC);
        if(!piece.modelMaterials_.freeAllocation26E5EC())return finish(S::modelResource,0x26e5ecu);
        scene=piece.modelOwner_.releaseSceneBase2896D0(graph);
        if(!scene.okay())return finish(S::topology,scene.sourcePC);
        // Actual2896D0 performs these child reference/count transitions using
        // PSG sibling order; these totals only report the completed operations.
        out.referencesReleased+=piece.sceneCount_-1u;out.objectsDestroyed+=piece.sceneCount_;
        // PieceB4/B8 still borrow their worlds. Model retirement is not the
        // enclosing2CB500/165BD0/1413E0 lifetime boundary; do not clear them.
        return finish(S::pieceRetirementRequired,0x2cb500u);
    }
    // Dispose the actual unlinked first piece and collection after their
    // children. Does not unregister or cancel never-published query backing.
    OriginalWreckPrefixRetirementResult50LA retireFirstPieceAndCollection(
        const LiveZoneWorld50EN& actualCollisionWorld)noexcept {
        using S=OriginalWreckPrefixRetirementStatus50LA;using K=OriginalWreckComponentKind50LA;
        OriginalWreckPrefixRetirementResult50LA rejected;
        if(!storage_||!storage_->modelRetirementAttempted)return rejected;
        if(storage_->pieceCollectionRetirementAttempted)return storage_->retirementResult;
        if(storage_->retirementResult.status!=S::pieceRetirementRequired||
           !storage_->actualObjectCount423F60||!storage_->actualAttachmentLock423F58||
           *storage_->actualAttachmentLock423F58||!orderedQueriesColdPrepared())return rejected;
        auto& collection=storage_->collections[0];auto& piece=collection.pieces_[0];
        auto& object=*piece.componentSlots_[0];auto& container=collection.collectionComponent_;
        if(!piece.modelOwner_.released()||!piece.modelPackets_.released()||
           !piece.modelMaterials_.released26E4C0()||!piece.modelGeometry_||
           piece.modelGeometry_->constructed240110()||!piece.setupZonesBound_||
           piece.collisionWorldB8_!=&actualCollisionWorld||
           object.reference00!=1u||container.reference00!=1u||
           object.kind!=K::piece2CB4B0||container.kind!=K::collection22BF00||
           object.firstChild18||object.parent1C||object.previous10||object.next14||
           container.firstChild18||container.parent1C||container.previous10||container.next14||
           !collection.ordered38_||collection.sourceSlots38_!=1u||
           collection.ordered38_[0]!=&piece||collection.groupComponent_.reference00)return rejected;
        // No other source constructor may have been reached. The retained
        // source-prefix pointer list is an observation, never a ref-zero writer.
        for(std::size_t i=0;i<storage_->prefixDomain.count;++i){
            const auto* component=storage_->prefixComponents[i];
            if(!component||(component!=&object&&component!=&container&&component->reference00))return rejected;
        }
        // Refuse pending publication/registration/response borrows before any
        // lifetime ends. The later cancellation repeats its own admission.
        for(std::size_t c=0;c<storage_->count;++c)for(std::size_t p=0;p<storage_->collections[c].count_;++p){
            const auto& query=storage_->collections[c].pieces_[p].queries_;
            if(!query.prepared()||!query.canReleaseUnpublishedWorld(actualCollisionWorld))return rejected;
        }
        storage_->pieceCollectionRetirementAttempted=true;
        auto out=storage_->retirementResult;
        //104970 reaches zero once. Actual2CB4B0 initialized160/164/170
        // NULL, and this private pre-E0 path never reached their allocators.
        //2CB500 therefore skips all three frees, then165BD0 reaches1413E0.
        --object.reference00;++out.referencesReleased;
        object.kind=K::entity141300;--*storage_->actualObjectCount423F60;++out.objectsDestroyed;
        // Only now has the source piece lifetime ended. End native borrows,
        // not source B4/B8 stores. Numeric zone values remain historical.
        piece.visibilityWorldB4_=nullptr;piece.collisionWorldB8_=nullptr;
        piece.collisionLifecycleB8_=nullptr;piece.setupZonesBound_=false;
        piece.actualWaterB0_=nullptr;piece.actualWaterGlobals_=nullptr;
        // The piece was never appended at22C1B8, so container child18 is
        // genuinely empty.22BF50 frees38 only; it never releases pointees.
        --container.reference00;++out.referencesReleased;
        collection.ordered38_.reset(); //22BF84 native pointer-array disposal
        container.kind=K::entity141300;--*storage_->actualObjectCount423F60;++out.objectsDestroyed;
        // Source count30/slot history and native occurrence backing survive;
        // query storage still borrows it until explicit cold cancellation.
        out.status=S::prefixRetired;out.sourcePC=0x22bf90u;
        storage_->retirementResult=out;return out;
    }
    bool coldBackingPrepared()const noexcept{return bool(storage_);}
    std::uint64_t sessionGeneration()const noexcept{return storage_?storage_->sessionGeneration:0;}
    std::uint64_t racerKey()const noexcept{return storage_?storage_->racerKey:0;}
    std::size_t plannedCollectionCount()const noexcept{return storage_?storage_->count:0;}
    const OriginalWreckCollectionStorage50LA* collection(std::size_t i)const noexcept {
        return storage_&&i<storage_->count?storage_->ordered374[i]:nullptr;
    }
};
} // namespace rr::game
