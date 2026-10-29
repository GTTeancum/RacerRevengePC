#pragma once
#include "original_ui_archive50kg.h"
#include <cstddef>
#include <cstdint>
#include <memory>
namespace rr::assets {
class OriginalLoadingFontSource50LA;
class OriginalLoadingRaFile50LA;
class OriginalStandardTextureResolver50LA;
class OriginalStandardTextureResource50LA;
class OriginalStandardTextureTable50LA;
// Native load-attempt custody is admitted BEFORE132820, not a prematurely
// constructed source Resource. Actual24DEE8 allocation binds the resource later.
// The exact Table pending slot retains this object even if that allocation fails.
class OriginalLoadingPstFile50LA final {
    friend class OriginalStandardTextureResolver50LA;
    friend class OriginalLoadingRaFile50LA;
    friend class OriginalStandardTextureResource50LA;
    friend class OriginalStandardTextureTable50LA;
    std::shared_ptr<OriginalLoadingFontSource50LA> root_;
    std::shared_ptr<OriginalLoadingRaFile50LA> raRoot_; // Mutually exclusive exact loose-file authority.
    const void* fontEntry_=nullptr;
    OriginalUiMemberView50KG expected_{};
    std::size_t sourceIndex_=static_cast<std::size_t>(-1);
    std::int32_t row_=-1,handle_=-1;
    std::uint64_t generation_=0;
    OriginalStandardTextureResource50LA* boundResource_=nullptr;
    std::weak_ptr<OriginalStandardTextureResource50LA> hit_;
    bool searchReached_=false,returned132820_=false,hitReturned_=false;
    bool associated_=false,closed_=false,nativeAborted_=false,associationCleared_=false;
    bool sourceBankRetiredAssociation_=false; // Exact force1 row clearing, not native withdrawal.
    bool nativeAssociationWithdrawn_=false; // Lost public table: no source1327D0 receipt.
    OriginalLoadingPstFile50LA()noexcept=default;
public:
    ~OriginalLoadingPstFile50LA()noexcept=default; // Owner must explicitly close retained handles.
    OriginalLoadingPstFile50LA(const OriginalLoadingPstFile50LA&)=delete;
    OriginalLoadingPstFile50LA& operator=(const OriginalLoadingPstFile50LA&)=delete;
};
}
