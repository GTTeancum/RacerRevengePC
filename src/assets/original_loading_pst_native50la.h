#pragma once
#include <atomic>
#include <cstdint>
#include <thread>
namespace rr::assets {
class OriginalStandardTextureResource50LA;
// One process history, NEVER reset by bank/screen/resource lifetime. Generic
// source projections must invalidate before their unrepresented allocator
// interval; BSS initialization is not permission to forget previous calls.
class OriginalTextureAllocationHistory50LA final {
    friend class OriginalStandardTextureResource50LA;
    struct State {
        const std::thread::id thread=std::this_thread::get_id();
        std::atomic<bool> known{true},assertionsKnown{true};std::uint32_t bytes435938=0; // Original BSS initializer.
        std::atomic<std::uint8_t> version8974{0},allocation8994{0},width8984{0};
        std::atomic<std::uint32_t> version435980{0},allocation4359C0{0},width4359A0{0};
    };
    static State& state()noexcept {static State s;return s;}
    static void invalidateCounterNativeProjection()noexcept {state().known=false;}
    static void invalidateVersionNativeProjection()noexcept {state().assertionsKnown=false;}
    static void invalidateNativeCounterCleanup()noexcept {state().known=false;}
    static bool assertionHistoryCurrent()noexcept {return state().assertionsKnown.load()&&state().thread==std::this_thread::get_id();}
    static bool add2505E8(std::uint32_t bytes)noexcept {
        auto& s=state();if(!s.known||s.thread!=std::this_thread::get_id())return false;
        s.bytes435938+=bytes;return true; // Source32-bit wrap, not size_t sum.
    }
    static bool version24E900(std::uint32_t version)noexcept {
        auto& s=state();if(!s.version8974.load()){s.version435980.store(1);s.version8974.store(1);}
        return !s.version435980.load()||version==1;
    }
    static bool width24F26C(std::int32_t width)noexcept {
        auto& s=state();if(!s.width8984.load()){s.width4359A0.store(1);s.width8984.store(1);}
        return !s.width4359A0.load()||width>=16;
    }
    static bool allocation250758(bool allocated)noexcept {
        if(allocated)return true; // SourceNULL-only assertion interval.
        auto& s=state();if(!s.allocation8994.load()){s.allocation4359C0.store(1);s.allocation8994.store(1);}
        return !s.allocation4359C0.load();
    }
};
}
