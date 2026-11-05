#pragma once
#include <cstdint>

namespace rr::game {
//Native storage admission only. Does not issue malloc, initialize memory,
//supply a source pointer, or establish a source allocator return.
struct OriginalWaterTopologyRequest50LA {
    enum class Admission {accepted,countCapacity,byteCapacity,requestMismatch};
    std::uint64_t fullBytes=0;
    std::uint32_t sourceRequest=0;
    Admission admission=Admission::requestMismatch;
};
constexpr OriginalWaterTopologyRequest50LA originalWaterTopologyRequest50LA(
    std::uint32_t count,std::uint32_t maxCount,std::uint32_t maxBytes)noexcept {
    using A=OriginalWaterTopologyRequest50LA::Admission;
    const auto full=std::uint64_t(count)*20ull;
    const auto request=std::uint32_t(std::uint32_t(count*5u)<<2u);
    const auto sum=std::uint32_t(request+19u);
    const auto sign=[](std::uint32_t v)constexpr noexcept {
        return v&0x80000000u?0xffffffff00000000ull|std::uint64_t(v):std::uint64_t(v);
    };
    const auto normalized=sign(sum)<31ull?16u:sum&0xfffffff0u;
    const auto admission=count>maxCount?A::countCapacity:full>maxBytes?A::byteCapacity:
        full!=request||(request&0x80000000u)||(sum&0x80000000u)||
        (normalized&0x80000000u)||normalized<request?A::requestMismatch:A::accepted;
    return {full,request,admission};
}
} //namespace rr::game
