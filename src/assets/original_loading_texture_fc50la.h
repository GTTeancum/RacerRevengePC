#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <memory>
#include <new>
#include <optional>
#include <thread>
namespace rr::assets {
class OriginalStandardTextureResource50LA;
class OriginalLoadingTextureFc50LA;
// One process assertion history for every represented257750 producer. The
// existing World CUB producer must call this same leaf at its actual interval;
// sharing assertion state does not share its resource/provenance authority.
class OriginalTextureBufferAssertions50LA final {
    friend class OriginalStandardTextureResource50LA;
    friend class OriginalLoadingTextureFc50LA;
    struct State {std::atomic<bool> historyKnown{true};std::atomic<std::uint8_t> guard8A64{0};std::atomic<std::int32_t> assertion442CD0{0};};
    static State& state()noexcept {static State s;return s;}
    static bool historyCurrent()noexcept {return state().historyKnown.load();}
    static void invalidateNativeProjection()noexcept {state().historyKnown.store(false);}
    static bool reach257750(bool sourcePointerIsNull)noexcept {
        auto& s=state();if(!s.historyKnown.load())return false;
        if(!s.guard8A64.load(std::memory_order_acquire)){
            s.assertion442CD0.store(1,std::memory_order_relaxed);s.guard8A64.store(1,std::memory_order_release);
        }
        return !s.assertion442CD0.load()||sourcePointerIsNull;
    }
};
// Ordinary authenticated source PSTkind0 FC backing, distinct from World
// CubFacet and Layout's83-qword GS packet. Only the actual Texture owner may
// construct it in sourcepalette→257750→mip order. The resource branch must
// supply its distinct real Fonts or LoadingIcon authority; this backing grants
// neither archive identity. No transfer/submission API.
class OriginalLoadingTextureFc50LA final {
    friend class OriginalStandardTextureResource50LA;
    struct AlignedDelete {void operator()(std::uint8_t* p)const noexcept {
        if(p)::operator delete[](p,std::align_val_t{64});
    }};
    struct Buffer2592D0 {
        std::unique_ptr<std::uint8_t[],AlignedDelete> backing;
        const void* source00=nullptr;
        bool source00Published=false,uncachedAlias00=false,allocationNull00=false;
        std::optional<std::size_t> cursor04;
        std::optional<std::uint32_t> capacity08,field0C,field10,field14,field18,field1C;
        bool returned=false;
    };
    const std::thread::id thread_=std::this_thread::get_id();
    std::unique_ptr<Buffer2592D0> pending_;
    const Buffer2592D0* sourceFC_=nullptr;
    bool nullConstructorReached_=false,constructAttempted_=false,store2577F8Reached_=false;
    bool constructorReturned_=false,nativeClosed_=false,sourceRetired_=false;
    OriginalLoadingTextureFc50LA()noexcept=default;
    bool publishNull257740()noexcept {
        if(thread_!=std::this_thread::get_id()||nullConstructorReached_||constructAttempted_||nativeClosed_)return false;
        sourceFC_=nullptr;nullConstructorReached_=true;return true;
    }
    bool construct18_257750(std::uint32_t& pc)noexcept {
        pc=0x257750;
        if(thread_!=std::this_thread::get_id()||!nullConstructorReached_||constructAttempted_||nativeClosed_||sourceRetired_)return false;
        constructAttempted_=true;
        if(!OriginalTextureBufferAssertions50LA::historyCurrent()){pc=0x257750;return false;}
        if(!OriginalTextureBufferAssertions50LA::reach257750(sourceFC_==nullptr)){pc=0x2577A4;return false;}
        pc=0x2577D8;pending_.reset(new(std::nothrow) Buffer2592D0);
        if(!pending_){
            // Actual mallocNULL skips2592D0, then still publishesNULL and
            // returns normally. It is NOT an early synthetic allocation stop.
            sourceFC_=nullptr;store2577F8Reached_=true;constructorReturned_=true;pc=0x257810;return true;
        }
        auto& b=*pending_;pc=0x259314;
        // Request18:ceil(18/8)=3, bytes3<<7=384, sourcecapacity3<<3=24.
        try {b.backing.reset(static_cast<std::uint8_t*>(::operator new[](384,std::align_val_t{64})));}
        catch(...){
            b.source00Published=true;b.uncachedAlias00=true;b.allocationNull00=true;
            // Original OR20000000 produces that nonNULL invalid guest alias.
            // Native semantic state retains it; never dereference a fake host
            // address. No sourceFC publication, as constructor has NOT returned.
            pc=0x259348;return false;
        }
        b.source00=b.backing.get();b.source00Published=true;b.uncachedAlias00=true; //259338.
        std::memset(b.backing.get(),0,384); //259348 beforecapacity publication.
        b.capacity08=24; //259358 delay store.
        b.cursor04=0;b.field0C=0;b.field10=0;b.field14=0;b.field18=0;b.field1C=0; //2593F0.
        b.returned=true;
        sourceFC_=&b;store2577F8Reached_=true;constructorReturned_=true;pc=0x257810;return true;
    }
    bool sourceReturnedCurrent()const noexcept {
        if(thread_!=std::this_thread::get_id()||nativeClosed_||sourceRetired_||!nullConstructorReached_||
           !constructAttempted_||!store2577F8Reached_||!constructorReturned_)return false;
        if(!sourceFC_)return !pending_; // Exact returned outerNULL branch.
        if(sourceFC_!=pending_.get())return false;
        const auto& b=*pending_;
        return b.returned&&b.backing&&b.source00Published&&b.source00==b.backing.get()&&
            b.uncachedAlias00&&!b.allocationNull00&&b.capacity08==24&&b.cursor04==0&&
            b.field0C==0&&b.field10==0&&b.field14==0&&b.field18==0&&b.field1C==0;
    }
    bool preflightNativeClose()const noexcept {
        return thread_==std::this_thread::get_id();
        // No route exposes/borrows/submits the owned allocation. Source250900
        // and259170 must be separately implemented before this can change.
    }
    bool closeNative()noexcept {
        if(!preflightNativeClose())return false;if(nativeClosed_)return true;
        pending_.reset();nativeClosed_=true;return true; // Preserve sourceFC history.
    }
    bool retire257820MinusOne(std::uint32_t& pc)noexcept {
        pc=0x257820;
        if(thread_!=std::this_thread::get_id()||nativeClosed_||sourceRetired_||!nullConstructorReached_||
           (constructAttempted_&&!constructorReturned_))return false;
        if(sourceFC_&&!sourceReturnedCurrent())return false;
        // Actual257848→259380(+1) freesbuffer then outer. Caller a1=-1
        // prevents freeing embedded TextureFC itself. No sourceFC=NULL store.
        pending_.reset();sourceRetired_=true;pc=0x257878;return true;
    }
public:
    ~OriginalLoadingTextureFc50LA()noexcept {if(pending_&&!nativeClosed_&&!sourceRetired_)std::terminate();}
    OriginalLoadingTextureFc50LA(const OriginalLoadingTextureFc50LA&)=delete;
    OriginalLoadingTextureFc50LA& operator=(const OriginalLoadingTextureFc50LA&)=delete;
};
}
