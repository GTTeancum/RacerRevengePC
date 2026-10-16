#pragma once
#include "original_ui_pages50kg.h"
#include <memory>

namespace rr::assets {
// Native PSM text owner for an already resolved ORIGINAL member. Like the
// Standard PSG owner, this does not authenticate arbitrary caller bytes. The
// actual application resource resolver must authenticate/select the member;
// neither this type nor a filename is an authentication or loaded-material token.
// No UI archive/member identity is fabricated for gameplay resources.
class OriginalStandardMaterialSource50LA final {
public:
    OriginalStandardMaterialSource50LA() noexcept;
    ~OriginalStandardMaterialSource50LA();
    OriginalStandardMaterialSource50LA(const OriginalStandardMaterialSource50LA&)=delete;
    OriginalStandardMaterialSource50LA& operator=(const OriginalStandardMaterialSource50LA&)=delete;
    // Copies exact decompressed PSM bytes and their resolved member name.
    // Native text parsing only; no IO, texture decoding, callbacks or GPU work.
    bool prepare(const std::string& originalMember,const std::vector<std::uint8_t>&,
        OriginalUiPageDiagnostic50KG&) noexcept;
    bool prepared() const noexcept;
    const std::string* member() const noexcept;
    const std::vector<std::uint8_t>* originalBytes() const noexcept;
    const OriginalUiDatSection50KG* section(const std::string&) const noexcept;
    const std::vector<OriginalUiDatSection50KG>* sections() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
