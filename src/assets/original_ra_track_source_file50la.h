#pragma once
#include "original_standard_texture_resource50la.h"
#include "original_standard_archive_provider50la.h"
#include <memory>
#include <array>
#include <string_view>
#include <string>
#include <thread>
namespace rr::platform {class OriginalWorldSourceConfig50LA;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::assets {
// Private native custody for the genuine TRACKS/RA bank in the existing
// FileManager. It neither replaces Course's bank root nor owns another manager.
class OriginalRaTrackSourceFile50LA final {
    friend class OriginalStandardTextureResolver50LA;
    friend class rr::platform::OriginalWorldSourceConfig50LA;
    friend class ::ra_cold_aggregate50LA::Owner;
    using D=OriginalStandardTextureDiagnostic50LA;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<const OriginalUiArchive50KG> archive_;
    const std::thread::id thread_=std::this_thread::get_id();
    const rr::platform::OriginalWorldSourceConfig50LA* occurrence_=nullptr;
    std::int64_t handle_=-1;
    bool attempted_=false,acquired_=false,joined_=false,nativeClosed_=false;
    std::size_t nativeMissingLookupBorrowers_=0; //Typed132820 attempts, not source file opens.
    explicit OriginalRaTrackSourceFile50LA()noexcept=default;
    bool mount12CEA0(std::shared_ptr<OriginalStandardTextureResolver50LA>,
        std::shared_ptr<OriginalStandardArchiveProvider50LA>,
        std::shared_ptr<const OriginalUiArchive50KG>,D&)noexcept;
    bool enterArchiveOnly132A20(D&)noexcept;
    bool resizeCache132E40(D&)noexcept; // Actual fixed bank name / 0x280000 call.
    // True means the source call returned (including genuine absent handle -1).
    // False is a bounded failure before caller source00 publication.
    // Retain the exact attempted Config occurrence before fallible search/open.
    // False with handle -1 still requires abortOccurrenceNative before deletion.
    bool open131230(const rr::platform::OriginalWorldSourceConfig50LA&,
        std::string_view,std::int64_t& source00,D&)noexcept;
    bool current(const rr::platform::OriginalWorldSourceConfig50LA&,
        std::int64_t source00)const noexcept;
    bool read12FB30(const rr::platform::OriginalWorldSourceConfig50LA&,
        std::int64_t source00,void*,std::size_t,std::size_t& done,D&)noexcept;
    // Real same-occurrence line result. Unknown tails are never initialized
    // as source bytes. A bounded return cannot be interpreted as EOF.
    struct Line12EE90 {
        std::array<char,1024> bytes;
        std::array<bool,1024> known{};
        bool present=false;
        std::size_t position=0; // Native post-line cursor, not12F520's return.
        Line12EE90()noexcept=default;
        Line12EE90(const Line12EE90&)=delete; // Never copy indeterminate tail bytes.
        Line12EE90& operator=(const Line12EE90&)=delete;
    };
    bool read12EE90(const rr::platform::OriginalWorldSourceConfig50LA&,
        std::int64_t source00,Line12EE90&,D&)noexcept;
    bool tell12F520(const rr::platform::OriginalWorldSourceConfig50LA&,
        std::int64_t source00,std::int64_t& sourceV0,D&)noexcept;
    bool seek12F640(const rr::platform::OriginalWorldSourceConfig50LA&,
        std::int64_t source00,std::size_t position,D&)noexcept;
    // Genuine flush12F290/no-op13D9E0 then numeric archive close131040.
    // Does not mutate Config.source00; caller performs only its reached stores.
    bool close131040(const rr::platform::OriginalWorldSourceConfig50LA&,
        std::int64_t source00,bool flush12F290,D&)noexcept;
    // Native-only exact attempted/open occurrence withdrawal; no Config
    // destructor/close receipt and no invented source00 store on failed open.
    bool abortOccurrenceNative(const rr::platform::OriginalWorldSourceConfig50LA&,D&)noexcept;
    bool closeNative(D&)noexcept;
public:
    ~OriginalRaTrackSourceFile50LA();
    OriginalRaTrackSourceFile50LA(const OriginalRaTrackSourceFile50LA&)=delete;
    OriginalRaTrackSourceFile50LA& operator=(const OriginalRaTrackSourceFile50LA&)=delete;
};
}
