#pragma once
#include <array>
#include <cstdint>
namespace rr::game {
// Process BSS plus the actual3CAE20 static constructor at3CAE7C. These are
// retained by Application, never reset on Garage entry. Concrete290F10 owns
// the lazy assertion and wrapping serial writes. This record alone does not
// prove every legacy native emitter is joined to the process-global owner.
struct OriginalGarageEmitterShared50KG final {
    std::uint8_t guard8C5C=0;
    std::uint32_t assertion434C0=0,serial434B8=0;
    const std::array<float,4> origin43470{{0.f,0.f,0.f,1.f}};
};
}
