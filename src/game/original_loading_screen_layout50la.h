#pragma once
#include "original_loading_interrupt3_custody50la.h"
#include "../../platform/linux/native/original_loading_lsi_config50la.h"
#include <array>
#include <optional>
#include <cstring>
#include <string>
#include <memory>
#include <new>
namespace rr::game {
class OriginalLoadingScreen40Mode0LA;
// Native attempt custody is distinct from the nullable original malloc(0x18).
// Only a real allocation receives2CCC60 stores/global publication. No Entity.
class OriginalLoadingScreenLayout50LA final {
    friend class OriginalLoadingScreen40Mode0LA;
    using Config=rr::platform::OriginalLoadingLsiConfig50LA;
    struct SourceAllocation18 {std::array<std::uint8_t,24> identityBacking;};
    OriginalLoadingScreen40Mode0LA* const screen_;
    const std::thread::id thread_=std::this_thread::get_id();
    std::unique_ptr<SourceAllocation18> sourceAllocation_;
    const void* sourceIdentity_=nullptr;
    std::unique_ptr<Config> config_;
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> archive_;
    OriginalLoadingInterrupt3Custody50LA interrupt_;
    bool sourceAllocationAttempted_=false,constructorReturned_=false,loaderAttempted_=false,nativeClosed_=false;
    std::uint8_t loaderDrainPhase_=0;
    bool directoryFormatted_=false,bankCallReached_=false;
    std::string bankPath_;
    // Pointer fields are source-known only after the real ctor; raw host
    // zero backing in a NULL attempt is never evidence of original stores.
    const void* frames10_=nullptr;
    const void* gs14_=nullptr;
    std::uint8_t constructorStores_=0;
    explicit OriginalLoadingScreenLayout50LA(OriginalLoadingScreen40Mode0LA& screen)noexcept:screen_(&screen){}
    bool currentNative()const noexcept;
    static bool admitNative(OriginalLoadingScreen40Mode0LA&,std::unique_ptr<OriginalLoadingScreenLayout50LA>&)noexcept;
    bool allocateAndConstruct1CC610(std::uint32_t&)noexcept;
    bool load2CCDA0(std::uint32_t&)noexcept;
    bool preflightNativeClose()const noexcept;
    bool closeNative()noexcept;
#include "original_loading_lsi_animations50la.inl"
public:
    ~OriginalLoadingScreenLayout50LA()noexcept {if(sourceAllocationAttempted_&&!nativeClosed_)std::terminate();}
    OriginalLoadingScreenLayout50LA(const OriginalLoadingScreenLayout50LA&)=delete;
    OriginalLoadingScreenLayout50LA& operator=(const OriginalLoadingScreenLayout50LA&)=delete;
};
}
