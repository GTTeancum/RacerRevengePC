#pragma once
#include <array>
#include <cstdint>
#include <memory>
namespace rr::platform {
class VehicleWindowGl50DS;
// Native capability for the recovered state4 direct video command. Expiry is
// independent of the renderer address: stale copies never dereference it.
class OriginalUiDirectVideoCapability50KG final {
public:
    bool valid() const noexcept { return !lifetime_.expired(); }
private:
    friend class VehicleWindowGl50DS;
    std::weak_ptr<const std::uint8_t> lifetime_;
    const VehicleWindowGl50DS* renderer_=nullptr;
};
// Not a material or a synthetic PSM. This exact queue command is appended by
// 25E610 -> 244A40 before the subsequent 24B7B0 material-group drain.
// Broader rectangles/colors/video modes are deliberately not admitted here.
struct OriginalUiDirectVideo50KG {
    std::uint64_t test=0,prim=0,alpha=0;
    std::array<std::uint8_t,4> rgba{};
    std::uint32_t width=0,height=0,entryPC=0,producerPC=0,submitPC=0;
};
inline bool validOriginalUiDirectVideo50KG(const OriginalUiDirectVideo50KG& c) noexcept {
    return c.test==0x31001ull&&c.prim==0x46ull&&c.alpha==0x8000000009ull&&
        c.rgba==std::array<std::uint8_t,4>{0,0,0,0}&&c.width==640&&c.height==448&&
        c.entryPC==0x25B1A0&&c.producerPC==0x25E610&&c.submitPC==0x244A40;
}
}
