#pragma once
#include <cstdint>
namespace rr::game {class OriginalSourcePhysicsContact50LA;}
namespace rr::assets {
//Private reached296190/295E00 adapters; not decoded collision authority.
class OriginalSourceContactPointMath50LA final {
    friend class rr::game::OriginalSourcePhysicsContact50LA;
    static bool packed296244(std::uint16_t word,std::uint32_t& outputBits)noexcept;
    static bool equal295E54(std::uint32_t left,std::uint32_t right,bool& equal)noexcept;
};
}
