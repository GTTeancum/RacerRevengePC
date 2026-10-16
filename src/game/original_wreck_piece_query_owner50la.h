#pragma once
#include "original_wreck_assets50la.h"
#include "original_wreck_physical50la.h"

namespace rr::game {
class OriginalOnePieceWreckOwner50LA;
class OriginalWreckRacerStorage50LA;
// Stable typed identity of ONE actual piece, shared by its E0/E4/E8 source9C
// fields. It borrows canonical storage; it owns no extra group or physics body.
// Cold binding is not source construction, activation or callback completion.
class OriginalWreckPieceQueryOwner50LA final {
    friend class OriginalOnePieceWreckOwner50LA;
    friend class OriginalWreckRacerStorage50LA;
    OriginalWreckAssets50LA* assets_=nullptr;
    OriginalWreckPhysical50LA* physical_=nullptr;
    OriginalWreckPieceFields50LA* fields_=nullptr;
    OriginalWreckComponent50LA* component_=nullptr;
    std::uint64_t generation_=0;
public:
    OriginalWreckPieceQueryOwner50LA()=default;
    OriginalWreckPieceQueryOwner50LA(const OriginalWreckPieceQueryOwner50LA&)=delete;
    OriginalWreckPieceQueryOwner50LA& operator=(const OriginalWreckPieceQueryOwner50LA&)=delete;
    OriginalWreckPieceQueryOwner50LA(OriginalWreckPieceQueryOwner50LA&&)=delete;
    OriginalWreckPieceQueryOwner50LA& operator=(OriginalWreckPieceQueryOwner50LA&&)=delete;
private:
    bool bindCold(OriginalWreckAssets50LA& assets,OriginalWreckPhysical50LA& physical,
        OriginalWreckPieceFields50LA& fields,OriginalWreckComponent50LA& actualPieceStorage,
        std::uint64_t sessionGeneration)noexcept {
        if(assets_||!assets.prepared()||!sessionGeneration)return false;
        assets_=&assets;physical_=&physical;fields_=&fields;component_=&actualPieceStorage;
        generation_=sessionGeneration;return true;
    }
public:
    bool boundCold()const noexcept{return assets_&&physical_&&fields_&&component_&&generation_;}
    bool coherent()const noexcept {
        return boundCold()&&assets_->prepared()&&component_->kind==OriginalWreckComponentKind50LA::piece2CB4B0&&
            component_->reference00&&component_->objectKey&&component_->sessionGeneration==generation_&&
            physical_->bodyConstructorFieldsApplied&&physical_->model288==assets_->model14C()&&
            physical_->modelGraphNodes==assets_->modelGraphIdentity()&&
            physical_->modelGraphCount==assets_->modelGraphNodeCount();
    }
    // Require boundCold() first. All borrows retain the lifetime of the actual
    // owning piece/inventory; no setter can substitute another occurrence.
    OriginalWreckAssets50LA& assets()noexcept{return *assets_;}
    const OriginalWreckAssets50LA& assets()const noexcept{return *assets_;}
    OriginalWreckPhysical50LA& physical()noexcept{return *physical_;}
    const OriginalWreckPhysical50LA& physical()const noexcept{return *physical_;}
    OriginalWreckPieceFields50LA& fields()noexcept{return *fields_;}
    const OriginalWreckPieceFields50LA& fields()const noexcept{return *fields_;}
    OriginalWreckComponent50LA& component()noexcept{return *component_;}
    const OriginalWreckComponent50LA& component()const noexcept{return *component_;}
    std::uint64_t sessionGeneration()const noexcept{return generation_;}
};
} // namespace rr::game
