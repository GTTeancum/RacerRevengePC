#pragma once
#include <cstdint>
#include <limits>
#include <thread>
#include <exception>
namespace rr::game {
class OriginalLoadingScreenLayout50LA;
// Native ownership of the exact LSI callback registration, NOT a PS2 kernel
// emulator or a VBlank source. No method dispatches2CCA70/2CEDB0. In particular
// having this lease never certifies2CE9C0 enable or a Screen activation.
class OriginalLoadingInterrupt3Custody50LA final {
    friend class OriginalLoadingScreenLayout50LA;
    struct Process {
        const std::thread::id ownerThread=std::this_thread::get_id();
        OriginalLoadingScreenLayout50LA* nativeLive=nullptr;
        const OriginalLoadingInterrupt3Custody50LA* nativeLease=nullptr;
        // Source constructor2CCC80 is the ONLY writer. Native close retains
        // this historical pointer; it is never dereferenced after retirement.
        const void* source4442F0=nullptr; // Actual source allocation, not native attempt wrapper.
        bool source4442F0Known=false;
        std::uint64_t generation=0;
    };
    static Process& process()noexcept {static Process value;return value;}
    const std::thread::id thread_=std::this_thread::get_id();
    OriginalLoadingScreenLayout50LA* owner_=nullptr;
    std::uint64_t generation_=0;
    const void* sourceIdentity_=nullptr;
    bool constructorPublished_=false,nativeRegistered_=false,nativeClosed_=false;
    bool registrationCallReached_=false,disableCallReached_=false,removeCallReached_=false;
    // Both original registration and disable return values are ignored. These
    // ledgers record calls and native custody, never invented kernel results.
    bool channelKnownDisabled_=false,sourceDestructorEntered_=false;
    OriginalLoadingInterrupt3Custody50LA()noexcept=default;
    static bool canAdmitNative()noexcept {
        const auto& p=process();
        return p.ownerThread==std::this_thread::get_id()&&!p.nativeLive&&!p.nativeLease&&p.generation!=std::numeric_limits<std::uint64_t>::max();
    }
    bool publishConstructor2CCC80(OriginalLoadingScreenLayout50LA& owner,const void* sourceIdentity)noexcept {
        if(thread_!=std::this_thread::get_id()||constructorPublished_||owner_||nativeClosed_||!sourceIdentity||!canAdmitNative())return false;
        auto& p=process();owner_=&owner;sourceIdentity_=sourceIdentity;generation_=++p.generation;
        p.nativeLive=&owner;p.nativeLease=this;
        p.source4442F0=sourceIdentity;p.source4442F0Known=true;
        constructorPublished_=true;return true;
    }
    bool current(const OriginalLoadingScreenLayout50LA& owner)const noexcept {
        const auto& p=process();
        return p.ownerThread==std::this_thread::get_id()&&thread_==std::this_thread::get_id()&&!nativeClosed_&&constructorPublished_&&owner_==&owner&&
            p.nativeLive==&owner&&p.nativeLease==this&&p.generation==generation_&&
            p.source4442F0Known&&sourceIdentity_&&p.source4442F0==sourceIdentity_;
    }
    bool register2CCED4(OriginalLoadingScreenLayout50LA& owner,std::uint32_t& pc)noexcept {
        pc=0x2CCED4;
        if(!current(owner)||sourceDestructorEntered_||registrationCallReached_||nativeRegistered_||removeCallReached_)return false;
        // Exact native registration metadata for source syscall16:
        // AddIntcHandler(channel3, handler2CCA70, next0). No handler-ID return
        // is invented: original code discards it. No delivery occurs here.
        registrationCallReached_=true;nativeRegistered_=true;return true;
    }
    bool disable2CCEDC(OriginalLoadingScreenLayout50LA& owner,std::uint32_t& pc)noexcept {
        pc=0x2CCEDC;
        if(!current(owner)||sourceDestructorEntered_||!registrationCallReached_||!nativeRegistered_||disableCallReached_||removeCallReached_)return false;
        disableCallReached_=true;channelKnownDisabled_=true;return true;
    }
    bool registrationDisabledCurrent(const OriginalLoadingScreenLayout50LA& owner)const noexcept {
        return current(owner)&&!sourceDestructorEntered_&&nativeRegistered_&&registrationCallReached_&&disableCallReached_&&
            channelKnownDisabled_&&!removeCallReached_;
    }
    bool sourceDisable2CCCAC(OriginalLoadingScreenLayout50LA& owner,std::uint32_t& pc)noexcept {
        pc=0x2CCCAC;if(!current(owner)||removeCallReached_)return false;
        sourceDestructorEntered_=true;
        // Actual destructor executes this even when the loader stopped before
        // registration. A disabled flag is not evidence registration succeeded.
        disableCallReached_=true;channelKnownDisabled_=true;return true;
    }
    bool sourceRemove2CCCB8(OriginalLoadingScreenLayout50LA& owner,std::uint32_t& pc)noexcept {
        pc=0x2CCCB8;if(!current(owner)||!sourceDestructorEntered_||!channelKnownDisabled_||removeCallReached_)return false;
        // Exact syscall17(channel3, handlerId0). Original return ignored.
        // Remove only THIS native binding; never remove another channel/owner.
        nativeRegistered_=false;removeCallReached_=true;return true;
    }
    bool preflightNativeClose(const OriginalLoadingScreenLayout50LA& owner)const noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(nativeClosed_)return owner_==&owner;
        if(!constructorPublished_)return !owner_&&!nativeRegistered_;
        return current(owner); // There is deliberately no callback-active path.
    }
    bool closeNative(OriginalLoadingScreenLayout50LA& owner)noexcept {
        if(!preflightNativeClose(owner))return false;if(nativeClosed_)return true;
        if(constructorPublished_){auto& p=process();p.nativeLive=nullptr;p.nativeLease=nullptr;}
        // Explicit native unwind, not source Disable/Remove execution or a
        // source4442F0=NULL store. All reached source call ledgers survive.
        owner_=&owner;nativeRegistered_=false;nativeClosed_=true;return true;
    }
    // Caller must stop BEFORE actual EnableIntc until a real renderer event
    // producer and callback-safe GS packet service are represented.
    bool enable2CE9C0(OriginalLoadingScreenLayout50LA&,std::uint32_t& pc)noexcept {
        pc=0x2CE9C0;return false;
    }
    ~OriginalLoadingInterrupt3Custody50LA()noexcept {
        if((constructorPublished_||nativeRegistered_)&&!nativeClosed_)std::terminate();
    }
    OriginalLoadingInterrupt3Custody50LA(const OriginalLoadingInterrupt3Custody50LA&)=delete;
    OriginalLoadingInterrupt3Custody50LA& operator=(const OriginalLoadingInterrupt3Custody50LA&)=delete;
};
}
