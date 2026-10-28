#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
namespace original_main_entry50KG { struct Application; class RaceF8RendererLease50LA; class NativeF8BarPrefix50LA; }
namespace rr::assets {
class OriginalStandardMaterialDomain50LA;
class OriginalStandardMaterialContext50LA;
class OriginalApplicationFonts50LA;
class OriginalPauseSourceData50LA;
}
namespace rr::game {
class OriginalApplicationEntities50LA;
class OriginalRaceConstructor50LA;
struct OriginalWreckComponent50LA;
struct OriginalCameraViewportFields;
struct OriginalRaceF8Diagnostic50LA {
    std::uint32_t sourcePC=0;
    std::size_t object=0,retainedPrimitives=0;
    const char* operation=nullptr;
    bool sourceEffectsCommitted=false;
};
// Same counted Race+F8 cold object:1983D0/1985F0/198510. No public load,
// draw, input, public state setter, or end-flow readiness certificate.
// Private CPU callbacks do not attach a live scheduler or authorize submission.
class OriginalRaceF850LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class ::original_main_entry50KG::RaceF8RendererLease50LA;
    friend class ::original_main_entry50KG::NativeF8BarPrefix50LA;
    friend class OriginalRaceConstructor50LA;
    struct Impl;
    std::unique_ptr<Impl> impl_;
    OriginalRaceF850LA(std::shared_ptr<OriginalApplicationEntities50LA>,std::uint64_t);
    bool constructed()const noexcept;
    bool load1985F0(std::shared_ptr<assets::OriginalStandardMaterialDomain50LA>,
        std::shared_ptr<const assets::OriginalStandardMaterialContext50LA>,
        std::shared_ptr<assets::OriginalApplicationFonts50LA>,
        std::shared_ptr<const assets::OriginalPauseSourceData50LA>,
        std::string& actualDirectory36B970,const std::string& fontDirectory36B420,
        std::uint32_t actualVideoMaximum60,OriginalRaceF8Diagnostic50LA&)noexcept;
    bool initialVisibility1A9170()noexcept;
    // Original leaf, always operating on this owner's actual low5/transition.
    // Only the existing cold caller is wired; later live callers are not admitted.
    bool visibility199770(std::int32_t shown,std::int32_t immediate,
        std::int32_t cross,std::int32_t square,std::int32_t localizedCross)noexcept;
    // Private Application bridge supplies its reached140CE4 factor/publication.
    // Empty-child subset only; no caller-supplied public delta or fake recursion.
    bool update199320(float actual23C,std::uint64_t publication,OriginalRaceF8Diagnostic50LA&)noexcept;
    bool demand199350(OriginalRaceF8Diagnostic50LA&)noexcept;
    // Private CPU-only198CF0. The Application obtains these four words from
    // its actual globally selected, retained gameplay Camera via115990.
    bool layout198CF0(const std::array<std::int32_t,4>&,OriginalRaceF8Diagnostic50LA&)noexcept;
    // CPU2E2DD0/2E2ED0 only; no2E21D0 assertion/gate,246760, FIFO or draw
    // completion. The native bar-prefix adapter is not original mode5 admission.
    bool preparePolygon2E2DD0(std::size_t,const OriginalCameraViewportFields&,
        std::uint32_t actualVideoMaximum60,OriginalRaceF8Diagnostic50LA&)noexcept;
    bool retireRendererResources198510(OriginalRaceF8Diagnostic50LA&)noexcept;
    bool loadedCold()const noexcept;
    bool belongsTo(const std::shared_ptr<OriginalApplicationEntities50LA>&,std::uint64_t)const noexcept;
    OriginalWreckComponent50LA* entity()noexcept;
    bool attachedMask(std::uint32_t)noexcept;
    bool nativeBarBorrowed()const noexcept; //Native pre-detach exclusion only.
    bool closeDetached(OriginalRaceF8Diagnostic50LA&)noexcept;
public:
    OriginalRaceF850LA(const OriginalRaceF850LA&)=delete;
    OriginalRaceF850LA& operator=(const OriginalRaceF850LA&)=delete;
    ~OriginalRaceF850LA()noexcept;
};
}
