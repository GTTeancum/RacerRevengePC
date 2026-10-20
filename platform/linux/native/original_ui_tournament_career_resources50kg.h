#pragma once
#include "original_ui_rider_profile50kg.h"
#include "original_ui_material_adapter50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include "../../../src/assets/original_ui_tournament_page50kg.h"
#include "../../../src/assets/original_ui_rider_pod_assets50kg.h"
#include "../../../src/assets/original_ui_screen_assets50kg.h"
#include "../../../src/assets/original_ui_screen_material_reference50kg.h"
#include "../../../src/game/original_ui_text50kg.h"
#include "../../../src/game/original_ui_main_layout50kg.h"

namespace original_main_entry50KG { struct Application; }
namespace rr::platform {
struct OriginalUiMainFrameClock50KG;
enum class OriginalUiTournamentCareerResourceError50KG : std::uint8_t {
    none,alreadyLoaded,owner,profile,unsupportedProgress,unsupportedMedal,
    page,pod,image,text,material,unsupportedStat,animation,allocation
};
struct OriginalUiTournamentCareerResourceDiagnostic50KG {
    OriginalUiTournamentCareerResourceError50KG error=OriginalUiTournamentCareerResourceError50KG::none;
    std::uint32_t sourcePC=0,courseOrder=0;
    assets::OriginalUiTournamentPageDiagnostic50KG page{};
    assets::OriginalUiRiderPodDiagnostic50KG pod{};
    assets::OriginalUiScreenAssetDiagnostic50KG image{};
    assets::OriginalUiMaterialDiagnostic50KG material{};
    game::OriginalUiTextDiagnostic50KG text{};
};
struct OriginalUiTournamentCareerMaterial50KG {
    std::string registeredName;
    assets::OriginalUiMaterial50KG source;
    assets::OriginalUiNativeMaterial50KG policy{};
    MaterialTextureSampler50EB sampler{};
};
struct OriginalUiTournamentMedalPacket50KG {
    std::uint32_t objectOffset=0,image=0;
    game::OriginalUiOverlayQuad50KG sprite{};
};
// Same source ScreenTexture packet shape; ownership remains with resources.
using OriginalUiTournamentStatPacket50KG=OriginalUiTournamentMedalPacket50KG;
struct OriginalUiTournamentNumber50KG {
    std::uint32_t objectOffset=0,formatPC=0;
    std::string text;
    game::OriginalUiTextLayout50KG glyphs{};
    std::uint32_t color54=0x00cccccc;
    std::uint8_t capacity5B=40,alpha58=255;
};
struct OriginalUiTournamentMedal50KG {
    std::uint32_t objectOffset=0;
    // Original image bank index;11 is the authored uncompleted medlgicn.
    std::uint32_t image=assets::unresolvedMaterialTexture50EA;
    std::uint8_t category=0;
};
// Coherent cold1EEC40 career resource owner. Consumes the real profile rather
// than caller-created win data. Pins page, pod geometry, fonts and medal images.
// Admits bounded materials and medal sprites; does not implement full-page
// animation, popup/save, registry submission or a complete Tournament session.
// A frontend must not interpret loaded/current as route/render readiness.
class OriginalUiTournamentCareerResources50KG final {
public:
    OriginalUiTournamentCareerResources50KG() noexcept;
    ~OriginalUiTournamentCareerResources50KG();
    OriginalUiTournamentCareerResources50KG(const OriginalUiTournamentCareerResources50KG&)=delete;
    OriginalUiTournamentCareerResources50KG& operator=(const OriginalUiTournamentCareerResources50KG&)=delete;
    bool load(std::shared_ptr<OriginalUiRiderProfileOwner50KG>,std::uint32_t selectedCareer,
        std::shared_ptr<const assets::OriginalUiArchive50KG> tournament,
        std::shared_ptr<const assets::OriginalUiArchive50KG> fonts,
        std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,
        std::shared_ptr<const assets::OriginalUiArchive50KG> backgrounds,
        const assets::OriginalUiMaterialRuntime50KG&,
        OriginalUiTournamentCareerResourceDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    bool current() const noexcept;
    const OriginalUiRiderProfileOwner50KG::TournamentCareerSnapshot* career() const noexcept;
    const assets::OriginalUiTournamentPageData50KG* page() const noexcept;
    const assets::OriginalUiRiderPodAssets50KG* pod() const noexcept;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images() const noexcept;
    // Exact material/policy/sampler for each of the24 image-bank entries.
    const std::array<OriginalUiTournamentCareerMaterial50KG,24>* materials() const noexcept;
    const assets::OriginalUiImageBankInput50KG* bankInput() const noexcept;
    // Exact medal subsection only. Actual popup29 bit0 suppresses it.
    // Borrowed pointers DO NOT retain this owner. Caller must retain it through
    // submission and actual renderer retirement. No frame seal is supplied.
    const std::vector<OriginalUiTournamentMedalPacket50KG>* medalPackets(
        std::uint8_t actualPopupFlags29) const noexcept;
    // Draw this subsection before medals, after portrait (1F2760..1F27A8).
    // Same borrowed-lifetime and actual popup requirements as medalPackets.
    const std::vector<OriginalUiTournamentStatPacket50KG>* statPackets(std::uint8_t actualPopupFlags29) const noexcept;
    // Original draw1F273C then1F2758, before stats. Borrowed exactly like
    // medalPackets: retain this owner through actual renderer retirement.
    const std::array<OriginalUiTournamentMedalPacket50KG,4>* backgroundPackets(std::uint8_t actualPopupFlags29) const noexcept;
    const OriginalUiTournamentMedalPacket50KG* portraitPacket(std::uint8_t actualPopupFlags29) const noexcept;
    //Legacy standalone numeric preparation only; never canonical ownership
    //or dispatch proof. Canonical callers use sampleCanonicalMarker below.
    // Copies a packet; its materialName still borrows this retained owner.
    // Call only in an accepted frame; this is not a complete frame transaction.
    bool sampleMarker(const OriginalUiMainFrameClock50KG&,std::uint8_t actualPopupFlags29,
        OriginalUiTournamentMedalPacket50KG&) noexcept;
    //At the genuine material service point only. Dispatches the retained
    //canonical prototype using applicationAnimation, then matches its actual
    //selected child extent to the immutable CPU image bank. No renderer receipt.
    //Any failure after actual service effects permanently latches this owner:
    //current() becomes false, retries return the original diagnostic without
    //service/projection, and output stays unchanged. No reset or rollback.
    //Retire renderer borrows, release canonical references, then destroy the
    //failed session/resource owner before a fresh construction is considered.
    bool canonicalMarkerTerminal() const noexcept;
    bool sampleCanonicalMarker(const OriginalUiMainFrameClock50KG&,
        assets::OriginalUiScreenMaterialReference50KG&,
        const assets::OriginalStandardMaterialCollectorReceipt50LA&,std::uint8_t actualPopupFlags29,
        OriginalUiTournamentMedalPacket50KG&,assets::OriginalStandardMaterialDiagnostic50LA&) noexcept;
    const std::array<std::uint32_t,6>* statCounts3D60() const noexcept;
    const std::array<OriginalUiTournamentNumber50KG,3>* numbers() const noexcept;
    const std::array<OriginalUiTournamentMedal50KG,13>* medals() const noexcept;
    // Actual source4050/4054/4058 flags, independently stored as0/1.
    const std::array<std::uint32_t,3>* trophyVisibility4050() const noexcept;
    // Host bank mapping for source ScreenTexturesE80/EE0/F40, not flags.
    const std::array<std::uint32_t,3>* trophyImages() const noexcept;
private:
    friend struct ::original_main_entry50KG::Application;
    bool sameProfileOwner(const OriginalUiRiderProfileOwner50KG&) const noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
