#pragma once
#include "original_standard_material_table50la.h"

namespace rr::assets {
enum class OriginalUiTournamentFallback50KG : std::uint8_t {
    goldIcon,silverIcon,bronzeIcon,goldAward,silverAward,bronzeAward,notch,portrait
};
// One real ScreenTexture54 primitive material00 reference. The authored Tournament outline and finite
// proven missing-PSM branches and medstar ATM prototype are admitted. This is not full2DF0B0:
// population14D370/29E340, geometry and reciprocal dimensions remain external.
class OriginalUiScreenMaterialReference50KG final {
public:
    OriginalUiScreenMaterialReference50KG() noexcept=default;
    ~OriginalUiScreenMaterialReference50KG();
    OriginalUiScreenMaterialReference50KG(const OriginalUiScreenMaterialReference50KG&)=delete;
    OriginalUiScreenMaterialReference50KG& operator=(const OriginalUiScreenMaterialReference50KG&)=delete;
    // actualDirectory36B970 must be the actual sampled caller directory, not
    // inferred from an archive filename. Only ui\textures[\] is admitted.
    // Each outline0..12 owns a separate retain, even though all share one row.
    bool acquireTournamentOutline1EF884(std::uint8_t outline,
        std::shared_ptr<const OriginalUiArchive50KG> tournament,
        std::shared_ptr<const OriginalUiMaterialApplication50LA> application,
        std::shared_ptr<OriginalStandardMaterialTable50LA> materials,
        std::shared_ptr<OriginalStandardTextureTable50LA> textures,
        const char* actualDirectory36B970,std::size_t capacity,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    // Bounded source constructors only. objectIndex: course0..12 for trophy
    //icons, trophy slot0..2 for awards, notch0..119, zero for portrait.
    // Portrait career must be authored; other kinds require career=0.
    // Caller owns actual profile selection and constructor chronology.
    bool acquireTournamentFallback2DF0B0(OriginalUiTournamentFallback50KG,
        std::uint32_t objectIndex,std::uint8_t career,
        std::shared_ptr<const OriginalUiArchive50KG> tournament,
        std::shared_ptr<const OriginalUiMaterialApplication50LA> application,
        std::shared_ptr<OriginalStandardMaterialTable50LA> materials,
        std::shared_ptr<OriginalStandardTextureTable50LA> textures,
        const char* actualDirectory36B970,std::size_t capacity,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    //Original1EFEDC ScreenTextureFA0 retains the genuine medstar row.
    bool acquireTournamentMarker1EFEDC(
        std::shared_ptr<const OriginalUiArchive50KG> tournament,
        std::shared_ptr<const OriginalUiMaterialApplication50LA> application,
        std::shared_ptr<OriginalStandardMaterialTable50LA> materials,
        std::shared_ptr<OriginalStandardTextureTable50LA> textures,
        const char* actualDirectory36B970,std::size_t capacity,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    //Original1F2624 resolves this actual aggregate then floors only its10
    //at1F264C. No child forwarding, frame selection or residency is implied.
    bool raiseTournamentMarkerDemand1F2624(OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    //Caller forwards clock.applicationAnimation from the SAME retained
    //Application. Authoritative reads use the sealed material context; the
    //forwarded view is checked for consistent values, never identity by value.
    //Schedule only genuine traversal; no legacy advanceAnimations pause gate.
    bool serviceTournamentMarker24A740(const rr::game::OriginalApplicationAnimationClockView50KG&,
        OriginalStandardMaterialDiagnostic50LA&) noexcept;
    //Current prototype phase only, not an animation service or render receipt.
    //Caller must retire the borrowed child view before releasing this reference.
    bool prepareTournamentMarkerFrame24B278(const OriginalStandardMaterialCollectorReceipt50LA&,
        OriginalStandardMaterialMapBindingView50LA&,OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    bool started() const noexcept{return started_;}
    bool retained() const noexcept{return retained_;}
    std::int32_t material00() const noexcept{return material00_;}
    std::uint32_t objectOffset() const noexcept{return objectOffset_;}
    const OriginalStandardMaterialTable50LA* table382B38() const noexcept{return materials_.get();}
    // Source1F2508->2DF8A0->direct1F2530 store through SAME live registries.
    // Fails for unpopulated rows, changed application/template/table identity,
    // unexpected source bytes/pass, missing PSM or animated texture paths.
    // Caller/session must enforce current profile generation and actual phase.
    bool raiseTournamentOutlineDemand1F2508(OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    // Finite fallback counterpart. Resolves SAME actual canonical resources;
    // no ready flag, ATM substitute, or public texture setter. Session still
    // owns profile generation, visible-trophy/count gates and traversal order.
    bool raiseTournamentFallbackDemand1F24B0(OriginalStandardMaterialDiagnostic50LA&) const noexcept;
    // Only after actual renderer retirement. Destruction invokes this same
    // release; shared_ptr lifetime does not replace the source numeric ref.
    void release299AC0() noexcept;
private:
    std::shared_ptr<const OriginalUiArchive50KG> tournament_;
    std::shared_ptr<const OriginalUiMaterialApplication50LA> application_;
    std::shared_ptr<OriginalStandardMaterialTable50LA> materials_;
    std::shared_ptr<OriginalStandardTextureTable50LA> textures_;
    std::int32_t material00_=-1;
    std::uint32_t objectOffset_=0,fallbackDemandStorePC_=0;
    bool missingPsm_=false,marker_=false;
    bool started_=false,retained_=false;
};
}
