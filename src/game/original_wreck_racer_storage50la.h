#pragma once
#include "original_wreck_asset_inventory50la.h"
#include "original_wreck_forces50la.h"
#include "original_wreck_collection_tuning50la.h"
#include "original_wreck_shared_response50la.h"
#include <limits>

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
    std::unique_ptr<OriginalWreckComponent50LA[]> components_;
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
    // Read-only cold inspection. No public constructor-completion flag or
    // mutable body/query/camera view exists at this stage.
    const OriginalWreckPhysical50LA& physicalBacking()const noexcept{return physical_;}
    const OriginalWreckComponent50LA* componentBacking()const noexcept{return components_.get();}
};
class OriginalWreckCollectionStorage50LA final {
    friend class OriginalWreckRacerStorage50LA;
    std::size_t ordinal_=0,count_=0;
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
// Own the complete original inventory and the nested native arrays together.
// The inventory outlives every borrower by declaration/destruction order.
// This class cannot publish an active racer374/death-camera view. It has no
// destructor callbacks because no source Entity/query/effect is constructed.
class OriginalWreckRacerStorage50LA final {
    struct Storage {
        std::unique_ptr<OriginalWreckAssetInventory50LA> inventory; // dies LAST
        std::uint64_t sessionGeneration=0,racerKey=0;
        std::size_t count=0,totalPieces=0,totalComponents=0;
        std::unique_ptr<OriginalWreckCollectionStorage50LA[]> collections;
        std::unique_ptr<OriginalWreckCollectionStorage50LA*[]> ordered374;
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
    bool coldBackingPrepared()const noexcept{return bool(storage_);}
    std::uint64_t sessionGeneration()const noexcept{return storage_?storage_->sessionGeneration:0;}
    std::uint64_t racerKey()const noexcept{return storage_?storage_->racerKey:0;}
    std::size_t plannedCollectionCount()const noexcept{return storage_?storage_->count:0;}
    const OriginalWreckCollectionStorage50LA* collection(std::size_t i)const noexcept {
        return storage_&&i<storage_->count?storage_->ordered374[i]:nullptr;
    }
};
} // namespace rr::game
