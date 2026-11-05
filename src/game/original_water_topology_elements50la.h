#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <optional>

namespace rr::game {
// Native success backing for the 48*N+16 request at2BACEC, separate from
// allocator history. No original new return, guest address or owner authority.
// 2E8990 writes stride/count into the first eight cookie bytes;133F80 writes
// only element+20/+24/+28=0 and+2C=-1. All other bytes remain uninitialized.
class OriginalWaterTopologyElements50LA final {
    struct Free {void operator()(void* p)const noexcept {std::free(p);}};
    std::unique_ptr<void,Free> payload_;
    std::uint32_t count_=0;
    bool attempted_=false,constructed_=false;
    void store(std::size_t offset,std::uint32_t value)noexcept {
        auto* p=static_cast<std::uint8_t*>(payload_.get());
        for(unsigned i=0;i<4;++i)p[offset+i]=std::uint8_t(value>>(8u*i));
    }
public:
    OriginalWaterTopologyElements50LA()noexcept=default;
    OriginalWaterTopologyElements50LA(const OriginalWaterTopologyElements50LA&)=delete;
    OriginalWaterTopologyElements50LA& operator=(const OriginalWaterTopologyElements50LA&)=delete;
    enum class Allocation {ready,capacity,payload,alreadyAttempted};
    Allocation allocate(std::uint32_t count)noexcept {
        if(attempted_)return Allocation::alreadyAttempted;
        attempted_=true;
        if(count>16384u)return Allocation::capacity;
        count_=count;
        payload_.reset(std::malloc(std::size_t(count)*48u+16u));
        return payload_?Allocation::ready:Allocation::payload;
    }
    bool construct()noexcept {
        if(!payload_||constructed_)return false;
        store(0,48u);store(4,count_);
        for(std::uint32_t i=0;i<count_;++i){
            const auto at=16u+std::size_t(i)*48u;
            store(at+0x2cu,0xffffffffu);
            store(at+0x24u,0);store(at+0x28u,0);store(at+0x20u,0);
        }
        constructed_=true;return true;
    }
    std::uint32_t count()const noexcept{return count_;}
    std::size_t extent()const noexcept{return payload_?std::size_t(count_)*48u+16u:0u;}
    const void* allocationIdentity()const noexcept{return payload_.get();}
    const void* elementsIdentity()const noexcept {
        return constructed_?static_cast<const std::uint8_t*>(payload_.get())+16u:nullptr;
    }
    // Offset is relative to allocation, including cookie. Unknown bytes are
    // never read, even if the native allocator happened to return zeroed pages.
    std::optional<std::uint8_t> knownByte(std::size_t offset)const noexcept {
        if(!constructed_||offset>=extent())return std::nullopt;
        if(offset>=8u&&(offset<16u||(offset-16u)%48u<0x20u))return std::nullopt;
        return static_cast<const std::uint8_t*>(payload_.get())[offset];
    }
};
} //namespace rr::game
