#pragma once
// Process-wide copy of the last native failure reason (require50DU). Read by
// the Windows terminate handler so a failure that ends in std::terminate
// (strict owner guards during unwinding) is still reported, not a silent
// close. Fixed-size copy: never keeps a pointer to the caller's text.
#include <cstddef>
#include <cstring>

namespace native_last_failure50PD {
inline char* buffer()noexcept{static char text[512]{};return text;}
inline void record(const char* message)noexcept{
    if(!message)return;
    auto* out=buffer();std::size_t i=0;
    for(;i+1<512u&&message[i];++i)out[i]=message[i];
    out[i]=0;
}
inline const char* last()noexcept{return buffer()[0]?buffer():nullptr;}
}
