#pragma once
#include <array>
#include <cstdlib>
#include <cwchar>
namespace rr::platform::controller_bindings {
template<std::size_t N> inline std::array<int,N> identity() noexcept {
    std::array<int,N> result{};for(std::size_t i=0;i<N;++i)result[i]=static_cast<int>(i);return result;
}
struct Layout {
    std::array<int,16> buttons=identity<16>();
    std::array<int,4> axes=identity<4>();
};
inline Layout layouts[2][8];
// Each target uses a unique source, or -1 to leave it unbound. Pause stays on Start.
template<std::size_t N> bool parse(const wchar_t* text,std::array<int,N>& out) noexcept {
    if(!*text)return true;
    std::array<int,N> parsed{};std::array<bool,N> used{};
    for(std::size_t i=0;i<N;++i){
        wchar_t* end=nullptr;const long value=std::wcstol(text,&end,10);
        if(end==text||value< -1||value>=static_cast<long>(N)||*end!=(i+1==N?L'\0':L',')||
            (N==16&&((value==9)!=(i==9)))||(value>=0&&used[value]))return false;
        parsed[i]=static_cast<int>(value);if(value>=0)used[value]=true;
        text=end+1;
    }
    out=parsed;return true;
}
}
