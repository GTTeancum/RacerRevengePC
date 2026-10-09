#include "float_unsigned50r.h"
namespace rr::game {
std::uint32_t sourceFloatUnsigned50R(std::uint32_t raw) noexcept {
 const auto exponent=(raw>>23)&255u;const auto fraction=raw&0x7fffffu;
 if(!exponent||(exponent==255u&&fraction)||(raw&0x80000000u))return 0;
 if(exponent<127u)return 0;
 if(exponent>=159u)return 0xffffffffu;
 const auto significand=0x800000u|fraction;
 return exponent>=150u?significand<<(exponent-150u):significand>>(150u-exponent);
}
}
