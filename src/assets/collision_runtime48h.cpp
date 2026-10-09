#include "collision_runtime48h.h"
#include <cmath>
#include <cstring>
#include <limits>
namespace rr::assets {
bool packCollisionFaceNormal48H(const std::array<float,3>&input,std::array<std::int16_t,3>&output) noexcept {
    std::array<std::int16_t,3> result{};
    for(unsigned i=0;i<3;++i){
        if(!std::isfinite(input[i]))return false;
        volatile float scaled=32767.0f*input[i];
        volatile float biased=.5f+scaled;
        if(!std::isfinite(biased))return false;
        const auto integer=biased>=2147483648.0f?std::numeric_limits<std::int32_t>::max():
            (biased<=-2147483648.0f?std::numeric_limits<std::int32_t>::min():static_cast<std::int32_t>(biased));
        const auto low=static_cast<std::uint16_t>(static_cast<std::uint32_t>(integer)&65535u);
        std::memcpy(&result[i],&low,sizeof(low));
    }
    output=result;return true;
}
}
