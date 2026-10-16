#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rr::assets {
// Native semantic249390/249420 values, in the retained fresh382B78 registry
// order. destination is that registry row's scalar value, never a guest/host
// address. No VIF/GS words or execution-ready program payload are retained.
// An absent authored key creates no entry and has no invented float default.
struct OriginalStandardMaterialParameter50LA {
    std::int16_t row382B78=-1;
    std::int32_t destination=0;
    std::array<float,4> value{};
    std::size_t byteOffset=0;
    std::uint32_t sourcePC=0x249508;
};
using OriginalStandardMaterialParameters50LA=std::vector<OriginalStandardMaterialParameter50LA>;
}
