#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <span>

namespace rr::game {
// A four-byte automatic destination reused by a Water reader loop: stack+5C
// at2BAD48 (LH consumes only the low half), or stack+7C at299964 (LW consumes
// all four bytes). Neither stack word is initialized on function entry.
// Zeroed bookkeeping bytes are not known source bytes. Short transfers retain
// the previously written suffix; EOF preserves the entire previous value.
// The caller authenticates the read and commits its actual copied prefix even
// on helper refusal. This value helper proves no file return or cursor state.
class OriginalWaterReadScratch50LA final {
    std::array<std::uint8_t,4> bytes_{};
    std::uint8_t mask_=0;
public:
    bool copyPrefix(std::span<const std::uint8_t> copied)noexcept {
        if(copied.size()>bytes_.size())return false;
        for(std::size_t i=0;i<copied.size();++i){bytes_[i]=copied[i];mask_|=std::uint8_t(1u<<i);}
        return true;
    }
    std::optional<std::int32_t> signedHalfword()const noexcept {
        if((mask_&3u)!=3u)return std::nullopt;
        const auto value=std::uint32_t(bytes_[0])|(std::uint32_t(bytes_[1])<<8u);
        return value<0x8000u?std::int32_t(value):std::int32_t(value)-65536;
    }
    std::optional<std::uint32_t> word()const noexcept {
        if(mask_!=15u)return std::nullopt;
        std::uint32_t value=0;
        for(unsigned i=0;i<4u;++i)value|=std::uint32_t(bytes_[i])<<(8u*i);
        return value;
    }
    std::optional<std::uint8_t> byte(std::size_t offset)const noexcept {
        if(offset>=4u||!(mask_&(1u<<offset)))return std::nullopt;
        return bytes_[offset];
    }
};
} //namespace rr::game
