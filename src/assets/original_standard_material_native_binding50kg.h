#pragma once
#include "original_ui_archive50kg.h"
namespace rr::platform {class OriginalUiGarageCanonicalSubmission50KG;}
namespace rr::assets {
class OriginalStandardMaterialTable50LA;
// Actual canonical pass-lifetime HOST binding history. Cold/bound differ even
// when the same texture token was constructed. This replaces device addresses
// with immutable authenticated native image provenance, not guest residency.
// Only the real table's renderer-accepted transaction can publish a value.
class OriginalStandardMaterialNativeBinding50KG final {
public:
    bool bound() const noexcept {return member_!=nullptr;}
    bool equivalent(const OriginalStandardMaterialNativeBinding50KG& b)const noexcept {
        return member_==b.member_&&pstOffset_==b.pstOffset_&&pstBytes_==b.pstBytes_&&
            storedMip_==b.storedMip_&&header_==b.header_&&payload_==b.payload_&&
            width_==b.width_&&height_==b.height_&&kind_==b.kind_;
    }
private:
    friend class OriginalStandardMaterialTable50LA;
    friend class rr::platform::OriginalUiGarageCanonicalSubmission50KG;
    const OriginalUiMemberIdentity50KG* member_=nullptr;
    std::size_t pstOffset_=0,pstBytes_=0,storedMip_=0,header_=0,payload_=0;
    std::uint32_t width_=0,height_=0,kind_=0;
};
}
