#pragma once
#include <array>
#include <cstdint>

namespace rr::platform::keyboard_bindings {
enum Family {Standard, Twin, Both};
struct Action {Family family; const wchar_t* name; std::uint16_t original;};
#define RR_WIDE2(v) L##v
#define RR_WIDE(v) RR_WIDE2(v)
#define RR_BIND(f,id,k,label) {f,RR_WIDE(#id),k},
inline constexpr Action actions[]={
#include "keyboard_actions.inc"
};
#undef RR_BIND
#undef RR_WIDE
#undef RR_WIDE2

struct Layout {
    std::array<std::uint16_t,512> keys{};
    bool custom=false;
    Layout(){for(std::uint16_t i=0;i<keys.size();++i)keys[i]=i;}
};
inline Layout layouts[2];
inline std::uint16_t physical(bool twin,std::uint16_t canonical) noexcept {
    return canonical<512?layouts[twin].keys[canonical]:canonical;
}
// Project a snapshot from physical keys to original gameplay actions. Read all
// sources from the original snapshot so swaps and cleared bindings are independent.
inline const std::uint8_t* drivingKeys(bool twin,const std::array<std::uint8_t,512>& input,
    std::array<std::uint8_t,512>& scratch) noexcept {
    const auto& layout=layouts[twin];
    if(!layout.custom)return input.data();
    scratch=input;
    for(const auto& a:actions)if(a.family==Both||a.family==(twin?Twin:Standard)){
        const auto key=layout.keys[a.original];scratch[a.original]=key?input[key]:0;
    }
    return scratch.data();
}
}
