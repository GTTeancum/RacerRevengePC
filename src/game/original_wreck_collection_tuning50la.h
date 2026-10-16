#pragma once
#include "original_wreck_asset_inventory50la.h"
#include "original_wreck_group_arrays50la.h"
#include "original_wreck_tuning50la.h"

namespace rr::game {
enum class OriginalWreckCollectionTuningStatus50LA : std::uint8_t {
    complete,binding,pieceArithmetic,registration,rootRefresh
};
struct OriginalWreckCollectionTuningResult50LA {
    OriginalWreckCollectionTuningStatus50LA status=OriginalWreckCollectionTuningStatus50LA::binding;
    std::uint32_t sourcePC=0;
    std::size_t pieceOrdinal=0,piecesCompleted=0;
    bool currentPieceTuned=false;
};
// First22C310 pass over ONE real collection, using its retained inventory
// occurrences in original22C410..22C5A8 order. These are actual piece/body/
// Entity borrows, never one-piece substitute groups. All must remain stable.
// Source-valid prefix writes survive a later rejection; stop rather than
// replaying a partially completed call. No activation/effect/query receipt.
inline OriginalWreckCollectionTuningResult50LA tuneOriginalWreckCollection22C310_50LA(
    const OriginalWreckAssetInventory50LA& inventory,std::size_t collectionIndex,
    const OriginalWreckGroupPiece50LA* orderedPieces,std::size_t count,
    OriginalWreckGroupArrays50LA& group)noexcept {
    using S=OriginalWreckCollectionTuningStatus50LA;
    OriginalWreckCollectionTuningResult50LA out;
    auto fail=[&](S status,std::uint32_t pc){out.status=status;out.sourcePC=pc;return out;};
    if(!inventory.complete()||!orderedPieces||!count||count>512u||
       collectionIndex>=inventory.collections().size()||!group.primaryFieldsBound()||
       group.allocationCapacity()!=count-1u||group.registeredMemberCount30())return fail(S::binding,0x22c310u);
    const auto& authored=inventory.authoring().collections[collectionIndex];
    const auto* primary=group.primary();
    if(authored.pieces.size()!=count||primary->piece!=orderedPieces[0].piece||
       primary->physical!=orderedPieces[0].physical||primary->component!=orderedPieces[0].component)
        return fail(S::binding,0x22c310u);
    // Validate complete ordered ownership before the first numeric source write.
    for(std::size_t i=0;i<count;++i){
        const auto& p=orderedPieces[i];const auto* assets=inventory.pieceAssets(collectionIndex,i);
        if(!assets||!assets->prepared()||assets->selected().ordinal!=authored.ordinal||
           assets->selectedPiece().ordinal!=i+1u||!p.piece||!p.physical||!p.component||
           p.component->kind!=OriginalWreckComponentKind50LA::piece2CB4B0||
           !p.component->reference00||!p.component->objectKey||
           p.component->sessionGeneration!=primary->component->sessionGeneration||
           !p.physical->bodyConstructorFieldsApplied||!p.physical->contactConstructorFieldsApplied||
           p.physical->contactBody30!=&p.physical->body||p.physical->model288!=assets->model14C()||
           p.physical->modelGraphNodes!=assets->modelGraphIdentity()||
           p.physical->modelGraphCount!=assets->modelGraphNodeCount()){
            out.pieceOrdinal=i+1u;return fail(S::binding,0x22c410u);
        }
        for(std::size_t j=0;j<i;++j)if(p.piece==orderedPieces[j].piece||
           p.physical==orderedPieces[j].physical||p.component==orderedPieces[j].component||
           p.component->objectKey==orderedPieces[j].component->objectKey){
            out.pieceOrdinal=i+1u;return fail(S::binding,0x22c410u);
        }
    }
    for(std::size_t i=0;i<count;++i){
        out.pieceOrdinal=i+1u;out.currentPieceTuned=false;
        const auto& p=orderedPieces[i];const auto& assets=*inventory.pieceAssets(collectionIndex,i);
        if(!tuneOriginalWreckPiece22C310_50LA(assets,*p.physical,*p.piece))
            return fail(S::pieceArithmetic,0x22c534u);
        out.currentPieceTuned=true;
        const auto& source=assets.selectedPiece();
        const auto registered=group.registerMember2CA190(p,source.breakRate,source.breakScale);
        if(registered.status!=(i?OriginalWreckGroupArrayStatus50LA::memberStored:
                               OriginalWreckGroupArrayStatus50LA::primarySkipped))
            return fail(S::registration,registered.sourcePC);
        ++out.piecesCompleted;
    }
    const auto refresh=group.refreshSavedRoot22C5B0();
    if(refresh.status!=OriginalWreckGroupArrayStatus50LA::rootRefreshed)
        return fail(S::rootRefresh,refresh.sourcePC);
    out.status=S::complete;out.sourcePC=0x22c608u;return out;
}
} // namespace rr::game
