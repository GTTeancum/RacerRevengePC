#pragma once
#include <cstdint>
namespace native_racer50HW {class Rig;}
namespace original_cable_selection50LA {class Binding;}
namespace rr::game {
// Canonical banked scalar, not a constant supplied to a cable callback.
// Fresh native storage is unbound until the real cold racer projection.
// Unknown/unrepresented writers must stop at their existing lifecycle boundary;
// there is deliberately no public setter or diagnostic-to-write adapter.
class OriginalRacerField48_50LA final {
    friend class ::native_racer50HW::Rig;
    friend class ::original_cable_selection50LA::Binding;
    std::uint32_t value_=0;
    bool constructed_=false;
    bool construct20F6B4()noexcept {
        if(constructed_)return false;
        value_=0;constructed_=true;return true;
    }
public:
    bool sourceConstructed()const noexcept{return constructed_;}
};
}
