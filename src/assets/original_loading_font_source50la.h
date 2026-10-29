#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_standard_archive_provider50la.h"
#include <memory>
#include <string>
#include <thread>
namespace rr::game {class OriginalLoadingScreen40Mode0LA;}
namespace rr::assets {
class OriginalApplicationFonts50LA;
// Exact loading Screen's UIFONTS source bank and one currently open CEL.
// Namespace material/font leases do not supply these source file IDs. A cold
// pending Font Entry retains this owner until its actual file has been closed.
class OriginalLoadingFontSource50LA final {
    friend class rr::game::OriginalLoadingScreen40Mode0LA;
    friend class OriginalApplicationFonts50LA;
    friend class OriginalStandardTextureResolver50LA;
    using D=OriginalStandardTextureDiagnostic50LA;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<const OriginalUiArchive50KG> archive_;
    const std::thread::id thread_=std::this_thread::get_id();
    const void* celOccurrence_=nullptr; // Exact private Entry, never a handle ID.
    OriginalUiMemberView50KG cel_{};
    enum class Completion { unknown, registryHit, coldReturned };
    struct RegistryReceipt {
        Completion kind=Completion::unknown;
        std::weak_ptr<const void> entry;
        std::weak_ptr<OriginalStandardTextureResource50LA> resource;
    };
    std::array<RegistryReceipt,2> registryReceipts_{};
    std::array<const void*,2> sourceClosedCelEntries_{};
    std::array<OriginalUiMemberView50KG,2> sourceClosedCels_{};
    bool coldCelAttempted_=false;
    std::int32_t handle_=-1;
    bool attempted_=false,acquired_=false,joined_=false,sourceRetired_=false,nativeClosed_=false;
    explicit OriginalLoadingFontSource50LA()noexcept=default;
    bool mount1CAAE4(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,
        std::shared_ptr<const OriginalUiArchive50KG>,OriginalStandardTextureResolver50LA::LoadingFontsBankLease&,D&)noexcept;
    // Source-font kernel: actual opening and each read are interleaved with
    // reached glyph stores, not a retrospective completed-file observation.
    bool beginCel281890(const void* exactEntry,const std::string& name,
        OriginalUiMemberView50KG expected,D&)noexcept;
    bool readCel12FB30(const void* exactEntry,void*,std::size_t,std::size_t& done,D&)noexcept;
    bool eofCel12F3E0(const void* exactEntry,bool& atEnd,D&)noexcept;
    bool closeCel131040(const void* exactEntry,D&)noexcept;
    bool abortCelNative(const void* exactEntry,D&)noexcept;
    bool currentCel(const void* exactEntry)const noexcept;
    // Exact pending attempt custody includes failed opens with no acquired handle.
    // currentCel remains the stricter source-open proof.
    bool ownsCel(const void* exactEntry)const noexcept {return exactEntry&&celOccurrence_==exactEntry;}
    bool recordRegistryHit2822F0(const std::shared_ptr<const void>& exactEntry,const std::string& celName,OriginalUiMemberView50KG expected,D&)noexcept;
    bool recordColdRegistry2823EC(const std::shared_ptr<const void>& exactEntry,
        OriginalUiMemberView50KG cel,OriginalUiMemberView50KG pst,
        const OriginalStandardTextureResource50LA* resource,D&)noexcept;
    bool retireBank1CAD94(OriginalStandardTextureResolver50LA::LoadingFontsBankLease&,D&)noexcept;
    bool closeNative(OriginalStandardTextureResolver50LA::LoadingFontsBankLease&,D&)noexcept;
public:
    ~OriginalLoadingFontSource50LA()noexcept;
    OriginalLoadingFontSource50LA(const OriginalLoadingFontSource50LA&)=delete;
    OriginalLoadingFontSource50LA& operator=(const OriginalLoadingFontSource50LA&)=delete;
};
}
