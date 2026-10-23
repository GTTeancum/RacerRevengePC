#pragma once
#include "original_ui_screen_assets50kg.h"
namespace rr::assets {
struct OriginalUiImageBankInput50KG {
    // Pin authentic archives for every provenance member/CEL. No cross-archive
    // basename search: canonical catalog pointer equality resolves each member.
    std::vector<std::shared_ptr<const OriginalUiArchive50KG>> archives;
    // A binder owner's borrowed images may be passed via an aliasing shared_ptr
    // which pins its real binder owner. Input order becomes explicit remap order.
    std::vector<std::shared_ptr<const OriginalUiScreenAssets50KG>> owners;
    const OriginalUiIpumTextureCache50KG* ipumCache=nullptr;
};
struct OriginalUiImageBankMapping50KG {
    std::uint32_t combinedIndex=0;
    std::uint64_t sourceIdentity=0,combinedIdentity=0;
};
enum class OriginalUiImageBankError50KG : std::uint8_t {
    none,alreadyLoaded,input,provenance,identityConflict,capacity,decode,pixels,allocation
};
struct OriginalUiImageBankDiagnostic50KG {
    OriginalUiImageBankError50KG error=OriginalUiImageBankError50KG::none;
    std::uint32_t owner=0,image=0;
    OriginalUiScreenAssetDiagnostic50KG images;
};
// One genuine immutable ScreenAssets bank for an ordered mixed UI frame.
// Identical owner-local numeric IDs never identify images across owners.
// Deduplicates exact archive/member/PST-range/CEL tuples only; assigns canonical
// native IDs from global catalog ordinal, frame offset and CEL-presence bit.
// Every source range is re-decoded from pinned authenticated archive bytes and
// compared to each source owner's full decode, retaining IPUM/mips/provenance.
// No source program/material state is changed. This is CPU bank publication,
// not a GPU upload, ready destination, span remap or frame-lifetime drain.
class OriginalUiImageBank50KG final {
public:
    OriginalUiImageBank50KG() noexcept;~OriginalUiImageBank50KG();
    OriginalUiImageBank50KG(const OriginalUiImageBank50KG&)=delete;
    OriginalUiImageBank50KG& operator=(const OriginalUiImageBank50KG&)=delete;
    bool load(const OriginalUiImageBankInput50KG&,OriginalUiImageBankDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    std::shared_ptr<const OriginalUiScreenAssets50KG> images() const noexcept;
    const OriginalUiImageBankMapping50KG* mapping(std::uint32_t owner,std::uint32_t image) const noexcept;
    // Read-only exact member backing pinned by this combined bank. No new identity.
    OriginalUiMemberView50KG sourceMember(std::uint32_t combinedImageIndex)const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
