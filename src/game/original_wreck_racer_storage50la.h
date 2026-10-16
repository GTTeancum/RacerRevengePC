#pragma once
#include "original_wreck_asset_inventory50la.h"
#include "original_wreck_forces50la.h"
#include "original_wreck_collection_tuning50la.h"
#include "original_wreck_shared_response50la.h"
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
    std::unique_ptr<std::uint8_t[]> sceneObjectFlags03_;
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
    const OriginalWreckComponent50LA* componentBacking()const noexcept{return components_.get();}
};
class OriginalWreckCollectionStorage50LA final {
    friend class OriginalWreckRacerStorage50LA;
    std::size_t ordinal_=0,count_=0;
    std::uint32_t sourceCount30_=0;
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
    std::size_t ordinal()const noexcept{return ordinal_;}
    std::size_t plannedPieceCount()const noexcept{return count_;}
    const OriginalWreckPieceStorage50LA* piece(std::size_t i)const noexcept {
        return i<count_?ordered38_[i]:nullptr;
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
// Keys cover its retained component storage, including later unconstructed rows.
struct OriginalWreckConstructorPrefixBinding50LA {
    std::uint64_t sessionGeneration=0,racerKey=0,collectionKey=0;
    const std::uint64_t* pieceComponentKeys=nullptr;
    std::size_t pieceComponentKeyCount=0;
    std::uint32_t* actualObjectCount423F60=nullptr;
    const std::uint32_t* actualAttachmentLock423F58=nullptr;
    const OriginalWreckConstructorGlobals50LA* constructorGlobals=nullptr;
    const SpringVector4_48N* actualVector4431C0=nullptr;
    const std::uint8_t* soulTreeObjectType=nullptr;
    const WaterHeightProvider50JC* const* actualWaterB0Field=nullptr;
    const WaterHeightGlobals50JC* actualWaterGlobals=nullptr;
};
enum class OriginalWreckConstructorPrefixStatus50LA : std::uint8_t {
    binding,capacity,allocation,components,physical,publishedLifetimeRequired,stopped
};
struct OriginalWreckConstructorPrefixResult50LA {
    OriginalWreckConstructorPrefixStatus50LA status=OriginalWreckConstructorPrefixStatus50LA::binding;
    std::uint32_t sourcePC=0x22bfd0u;
    std::size_t initializedComponents=0;
    bool sourcePrefixCommitted=false;
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
        bool constructorAttempted=false;
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
        if(!inventory->complete())return fail(S::inventory);
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
                    piece.components_=std::make_unique<OriginalWreckComponent50LA[]>(piece.componentCount_);
                    piece.sceneObjectFlags03_=std::make_unique<std::uint8_t[]>(piece.sceneCount_);
                    if(!piece.queryOwner_.bindCold(*assets,piece.physical_,piece.piece_,piece.components_[0],sessionGeneration))
                        return fail(S::binding);
                    next->totalComponents+=piece.componentCount_;++next->totalPieces;
                    collection.ordered38_[p]=&piece;
                }
                next->ordered374[c]=&collection;
            }
            // All allocations/identity checks finish before ownership transfer.
            // No original bytes, model transforms or runtime globals changed.
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
                    &piece.components_[body+2u],&piece.components_[body+5u],&piece.components_[body+6u]}};
                out.query=piece.queries_.prepareCold(piece.queryOwner_,world,piece.components_[1],queries);
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
    // to reach another piece/collection. This real prefix constructs model,
    // D0 and gravity, then stops BEFORE E0 construction and11BBC0 world-row publication.
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
           binding.pieceComponentKeyCount!=piece.componentCount_||!binding.actualObjectCount423F60||
           !binding.actualAttachmentLock423F58||!binding.constructorGlobals||!binding.actualVector4431C0||
           !binding.soulTreeObjectType||*binding.soulTreeObjectType==0xffu||!vectors||
           !binding.actualWaterB0Field||(*binding.actualWaterB0Field&&!binding.actualWaterGlobals))return out;
        const auto body=1u+piece.sceneCount_;
        // Domain includes collection plus the initialized piece prefix only.
        if(body+3u>maxOriginalWreckComponents50LA){out.status=S::capacity;return out;}
        for(std::size_t i=0;i<piece.componentCount_;++i){
            const auto key=binding.pieceComponentKeys[i];
            if(!key||key==binding.collectionKey||key==storage_->racerKey)return out;
            for(std::size_t j=0;j<i;++j)if(key==binding.pieceComponentKeys[j])return out;
        }
        storage_->constructorAttempted=true;
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
        auto append=[&](std::size_t child,std::uint32_t mask){
            return appendOriginalWreckComponent50LA(domain,piece.components_[0],piece.components_[child],mask,
                *binding.actualAttachmentLock423F58).status==OriginalWreckComponentStatus50LA::complete;
        };
        if(!initialize(collection.collectionComponent_,K::collection22BF00,binding.collectionKey))
            return finish(S::components,0x22bf0cu);
        collection.sourceCount30_=static_cast<std::uint32_t>(collection.count_);
        if(!initialize(piece.components_[0],K::piece2CB4B0,binding.pieceComponentKeys[0]))
            return finish(S::components,0x1659ccu);
        initializeOriginalWreckPieceFields50LA(piece.piece_,*binding.constructorGlobals);
        piece.effects_.dustActive178=0;piece.effects_.dustTime17C=0.f;
        std::memcpy(piece.piece_.authored50.v,vectors->position434830.data(),16);
        std::memcpy(piece.piece_.look60.v,vectors->look434860.data(),16);
        std::memcpy(piece.piece_.up70.v,vectors->up434850.data(),16);
        piece.actualWaterB0_=*binding.actualWaterB0Field;piece.actualWaterGlobals_=binding.actualWaterGlobals;
        for(std::size_t row=0;row<piece.sceneCount_;++row){
            if(!initialize(piece.components_[1u+row],row?K::scene2893A0:K::model2403A0,
                binding.pieceComponentKeys[1u+row]))return finish(S::components,0x289418u);
            piece.components_[1u+row].category28=*binding.soulTreeObjectType;
            piece.sceneObjectFlags03_[row]=static_cast<std::uint8_t>((piece.sceneObjectFlags03_[row]&~7u)|(row?0u:3u));
        }
        // PSG parent/child rows are not Entity component children.
        if(!append(1u,0u))return finish(S::components,0x165cf0u);
        std::memcpy(piece.piece_.authored50.v,piece.assets_->authored50().data(),16);
        auto graph=piece.assets_->graph();
        if(!initialize(piece.components_[body],K::body167920,binding.pieceComponentKeys[body])||
           !initializeOriginalWreckBody50LA(piece.physical_,piece.components_[body],*binding.constructorGlobals)||
           !bindOriginalWreckBodyModel50LA(piece.physical_,graph,piece.assets_->model14C()))
            return finish(S::physical,0x165f10u);
        if(!append(body,0xffffffffu))return finish(S::components,0x165f3cu);
        if(!initialize(piece.components_[body+1u],K::gravity282670,binding.pieceComponentKeys[body+1u])||
           !initializeOriginalWreckGravity50LA(piece.gravity_,piece.physical_,piece.components_[body+1u],
                *binding.actualVector4431C0))return finish(S::physical,0x165f58u);
        if(!append(body+1u,0u))return finish(S::components,0x165f94u);
        return finish(S::publishedLifetimeRequired,0x166018u);
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
