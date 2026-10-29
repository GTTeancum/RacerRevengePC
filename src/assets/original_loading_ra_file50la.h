#pragma once
#include "original_standard_texture_resource50la.h"
#include <memory>
#include <thread>
namespace rr::assets {
// Exact loose LD_RA file lifetime. This is neither an archive bank capability
// nor a Course/Fonts Config grant. The same resource can outlive131040 while
// its actual source1C association keeps the closed device/member registered.
class OriginalLoadingRaFile50LA final {
    friend class rr::game::OriginalLoadingScreen40Mode0LA;
    friend class OriginalStandardTextureResolver50LA;
    using D=OriginalStandardTextureDiagnostic50LA;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<const OriginalLoadingTextureSource50LA> source_;
    std::weak_ptr<OriginalLoadingRaFile50LA> self_;
    const std::thread::id thread_=std::this_thread::get_id();
    struct Storage;
    std::unique_ptr<Storage> storage_;
    std::int32_t device_=-1,member_=-1;
    std::size_t sourceIndex_=static_cast<std::size_t>(-1);
    bool attempted_=false,acquired_=false,registered_=false,nativeClosed_=false;
    OriginalLoadingRaFile50LA()noexcept;
    static std::shared_ptr<OriginalLoadingRaFile50LA> admitNative(
        std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<const OriginalLoadingTextureSource50LA>,D&)noexcept;
    bool begin132820(std::size_t&,OriginalStandardTextureResolver50LA::UiSelection&,
        std::shared_ptr<OriginalLoadingPstFile50LA>&,
        std::shared_ptr<OriginalStandardTextureResource50LA>& cached,D&)noexcept;
    bool finishLoad24E094(const std::shared_ptr<OriginalLoadingPstFile50LA>&,
        const std::shared_ptr<OriginalStandardTextureResource50LA>&,D&)noexcept;
    bool abortPendingNative(const std::shared_ptr<OriginalLoadingPstFile50LA>&,D&)noexcept;
    bool closeNative(D&)noexcept;
public:
    ~OriginalLoadingRaFile50LA()noexcept;
    OriginalLoadingRaFile50LA(const OriginalLoadingRaFile50LA&)=delete;
    OriginalLoadingRaFile50LA& operator=(const OriginalLoadingRaFile50LA&)=delete;
};
}
