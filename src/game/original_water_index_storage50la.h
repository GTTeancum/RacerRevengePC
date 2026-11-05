#pragma once
#include <array>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <new>
#include <optional>

namespace rr::game {
//Native backing for Data+20. The separate mask is bookkeeping, never a
//zero-fill of the raw index array. Its last slot is not read by2BAC28..38.
class OriginalWaterIndexStorage50LA final {
    struct Free {void operator()(void* p)const noexcept {std::free(p);}};
    std::unique_ptr<void,Free> bytes_;
    std::unique_ptr<std::uint8_t[]> known_;
    std::uint32_t count_=0;
    bool attempted_=false;
public:
    enum class Allocation {none,capacity,metadata,payload,ready,alreadyAttempted};
    Allocation prepare(std::uint32_t count)noexcept {
        if(attempted_)return Allocation::alreadyAttempted;
        attempted_=true;
        if(count>16384u)return Allocation::capacity;
        count_=count;const auto extent=std::size_t(count)*4u;
        if(extent){known_.reset(new(std::nothrow) std::uint8_t[extent]{});if(!known_)return Allocation::metadata;}
        bytes_.reset(std::malloc(extent)); //Including actual malloc(0).
        return bytes_?Allocation::ready:Allocation::payload;
    }
    std::uint32_t count()const noexcept{return count_;}
    std::uint32_t readCount()const noexcept{return count_?count_-1u:0u;}
    bool ready()const noexcept{return bool(bytes_);}
    const void* identity()const noexcept{return bytes_.get();}
    bool copyPrefix(std::uint32_t ordinal,const std::array<std::uint8_t,4>& source,std::size_t copied)noexcept {
        if(!ready()||ordinal>=readCount()||copied>4u)return false;
        const auto offset=std::size_t(ordinal)*4u;
        for(std::size_t i=0;i<4u;++i)if(known_[offset+i])return false;
        auto* bytes=static_cast<std::uint8_t*>(bytes_.get());
        for(std::size_t i=0;i<copied;++i){bytes[offset+i]=source[i];known_[offset+i]=255u;}
        return true;
    }
    bool matches(std::uint32_t ordinal,const std::array<std::uint8_t,4>& source,std::size_t copied)const noexcept {
        if(!ready()||ordinal>=count_||copied>4u)return false;
        const auto offset=std::size_t(ordinal)*4u;
        const auto* bytes=static_cast<const std::uint8_t*>(bytes_.get());
        for(std::size_t i=0;i<4u;++i){
            if(known_[offset+i]!=(i<copied?255u:0u))return false;
            if(i<copied&&bytes[offset+i]!=source[i])return false;
        }
        return true;
    }
    std::optional<std::uint32_t> knownWord(std::uint32_t ordinal)const noexcept {
        if(!ready()||ordinal>=count_)return std::nullopt;
        const auto offset=std::size_t(ordinal)*4u;
        for(std::size_t i=0;i<4u;++i)if(known_[offset+i]!=255u)return std::nullopt;
        const auto* bytes=static_cast<const std::uint8_t*>(bytes_.get());std::uint32_t word=0;
        for(std::size_t i=0;i<4u;++i)word|=std::uint32_t(bytes[offset+i])<<(i*8u);
        return word;
    }
};
} //namespace rr::game
