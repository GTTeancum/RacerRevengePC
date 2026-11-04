#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_config_process_globals50la.h"
namespace rr::platform { class OriginalStreamLibrarySource50LA; }
namespace rr::assets {
// Private adapter for the same-backend AudioStr232EA0/Stream268690 issuer. Constructor custody is
// supplied by the same-App/Aural owner, never by a path or metadata match.
class OriginalStreamLibraryFile50LA final {
    friend class rr::platform::OriginalStreamLibrarySource50LA;
    friend class OriginalStandardTextureResolver50LA;
    using D=OriginalStandardTextureDiagnostic50LA;
    using R=OriginalStandardTextureResolver50LA;
    const original_main_entry50KG::Application* const issuer_;
    const rr::platform::OriginalStreamLibrarySource50LA* const loader_;
    const std::thread::id thread_=std::this_thread::get_id();
    std::shared_ptr<R> resolver_;
    std::shared_ptr<const OriginalUiArchive50KG> archive_;
    const OriginalStandardArchiveProvider50LA* provider_=nullptr;
    const void* occurrence_=nullptr;
    std::size_t mount_=static_cast<std::size_t>(-1);
    std::uint64_t generation_=0;
    std::int32_t handle_=-1,winner_=-1;
    enum class Kind { audioDirectory,streamNames,streamParameters };
    enum class Stage { fresh,audioAttempted,audioOpen,audioClosed,txtAttempted,txtOpen,txtClosed,jukAttempted,jukOpen,jukClosed };
    Stage stage_=Stage::fresh;
    bool acquired_=false,blocked_=false,nativeClosed_=false;
    bool flushAttempted_=false,flushReturned_=false,flushHandleCall_=false;
    OriginalStreamLibraryFile50LA(const original_main_entry50KG::Application*,
        const rr::platform::OriginalStreamLibrarySource50LA*)noexcept;
    // Fresh admits either catalog298680; JUK1578D0 requires this root's TXT close.
    // Audio close releases native custody; later Stream uses a fresh File owner.
    // bool means translated call returned; output is untouched on a bound.
    bool begin131230(std::shared_ptr<R>,const void* exactOccurrence,Kind kind,std::int64_t& returned,D&)noexcept;
    bool current(const void* exactOccurrence,std::int64_t handle)const noexcept;
    bool status12EC90(const void*,std::int64_t,std::uint64_t& modified,std::uint64_t& size,D&)noexcept;
    bool read12FB30(const void*,std::int64_t,void*,std::size_t requested,std::size_t& copied,D&)noexcept;
    bool seekSet12F640(const void*,std::int64_t,std::size_t position,D&)noexcept;
    bool flush12F290(const void*,std::int64_t,std::int32_t& returned,D&)noexcept;
    bool close131040(const void*,std::int64_t,bool flush,D&)noexcept;
    bool closeNative(D&)noexcept;
public:
    ~OriginalStreamLibraryFile50LA()noexcept;
    OriginalStreamLibraryFile50LA(const OriginalStreamLibraryFile50LA&)=delete;
    OriginalStreamLibraryFile50LA& operator=(const OriginalStreamLibraryFile50LA&)=delete;
};
}
