#pragma once
#include <cstdint>
#include <memory>
#include <thread>
#include <string_view>
namespace original_main_entry50KG {struct Application;}
namespace rr::assets {class OriginalStandardTextureResolver50LA;}
namespace rr::game {class OriginalApplicationStartup17CCF0Source50LA;}
namespace rr::platform {
class OriginalApplicationAural50KG;
// Same actual interface+40, never a library created from immutable authoring.
// No current source17CCF0 issuer exists. Both private loader bodies remain
// UNCALLED; future issuance must return232EA0 before entering268690.
class OriginalStreamLibrarySource50LA final {
    friend class OriginalApplicationAural50KG;
    friend class rr::game::OriginalApplicationStartup17CCF0Source50LA;
    using Aural=OriginalApplicationAural50KG;
    using Resolver=assets::OriginalStandardTextureResolver50LA;
    struct Diagnostic {std::uint32_t sourcePC=0;const char* reason=nullptr;};
    const ::original_main_entry50KG::Application* const issuer_;
    const Aural* const aural_;
    const void* const interfaceA0_;
    const std::thread::id thread_=std::this_thread::get_id();
    // Historical exact2665F0 stores, not a current-library readiness query.
    // Only the genuine future startup load can consume/update these fields.
    std::uint32_t loadedDFC_;
    struct Catalog;
    std::unique_ptr<Catalog> catalog98A0_,audioDirectory98A4_;
    struct CatalogLoad;
    std::unique_ptr<CatalogLoad> audioDirectoryLoad_;
    struct Storage;
    std::unique_ptr<Storage> load_;
    bool attempted_=false,audioDirectoryAttempted_=false,nativeClosed_=false;
    OriginalStreamLibrarySource50LA(const Aural&,const void* actualInterface)noexcept;
    bool begin232EA0(std::shared_ptr<Resolver> actualResolver,Diagnostic&)noexcept;
    bool begin268690(std::shared_ptr<Resolver> actualResolver,Diagnostic&)noexcept;
    bool loadCatalog2985B0(Catalog&,CatalogLoad&,bool audioDirectory,std::string_view filename,Diagnostic&)noexcept;
    bool openConfig1577D0(Diagnostic&)noexcept;
    bool format308C90(bool song,std::uint32_t ordinal,Diagnostic&)noexcept;
    bool finish268690(std::uint32_t callPC,std::uint32_t sourceResult,Diagnostic&)noexcept;
    bool metadata268790(Diagnostic&)noexcept;
    bool closeNative()noexcept;
    static bool stop(Diagnostic& d,std::uint32_t pc,const char* why)noexcept {
        d={pc,why};return false;
    }
public:
    ~OriginalStreamLibrarySource50LA()noexcept;
    OriginalStreamLibrarySource50LA(const OriginalStreamLibrarySource50LA&)=delete;
    OriginalStreamLibrarySource50LA& operator=(const OriginalStreamLibrarySource50LA&)=delete;
};
}
