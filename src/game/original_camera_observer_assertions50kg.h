#pragma once
#include <array>
#include <atomic>
#include <cstdint>
namespace rr::game {
// Source114770's process-static8118/3E00A0 and811C/3E00A8 records.
// Owner-neutral scalar effects only: no Camera/observer/collection authority.
class OriginalCameraObserverAssertions50KG final {
    struct Records {
        std::array<std::atomic<std::uint8_t>,2> guard{};
        std::array<std::atomic<std::int32_t>,2> word{};
    };
    static Records& records()noexcept {static Records value;return value;}
    static bool apply(std::size_t i,bool condition)noexcept {
        auto& r=records();
        if(!r.guard[i].load(std::memory_order_acquire)){
            r.word[i].store(1,std::memory_order_relaxed);r.guard[i].store(1,std::memory_order_release);
        }
        return !r.word[i].load(std::memory_order_relaxed)||condition;
    }
public:
    static bool allocation1147E4(bool nonnull)noexcept {return apply(0,nonnull);}
    static bool membership114884(bool childNonNull)noexcept {return apply(1,childNonNull);}
};
}
