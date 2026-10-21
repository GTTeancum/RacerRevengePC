#pragma once
#include "original_ui_material_adapter50kg.h"
#include "../../../src/assets/original_ui_tournament_page50kg.h"
#include "../../../src/assets/original_ui_rider_pod_assets50kg.h"
#include "../../../src/assets/original_ui_image_bank50kg.h"
#include "../../../src/assets/original_ui_movement_control50kg.h"
#include "../../../src/game/original_ui_icon_layout50kg.h"
#include "../../../src/game/original_ui_text_geometry50kg.h"
#include "../../../src/game/original_ui_exit_text50kg.h"
#include <array>
#include <memory>

namespace rr::platform {
inline constexpr std::size_t originalUiTournamentWidgetSlots50KG=64; // 63 authored IDs plus sentinel.
inline constexpr std::size_t originalUiTournamentWidgetMaterialCount50KG=12;
enum class OriginalUiTournamentWidgetKind50KG : std::uint8_t { absent,empty,text,icon,movement,userRotate };
enum class OriginalUiTournamentWidgetResourceError50KG : std::uint8_t {
    none,alreadyLoaded,notLoaded,owner,page,population,descriptor,material,image,registry,sampler,geometry,allocation
};
struct OriginalUiTournamentWidgetResourceDiagnostic50KG {
    OriginalUiTournamentWidgetResourceError50KG error=OriginalUiTournamentWidgetResourceError50KG::none;
    std::uint16_t widget=0;std::uint32_t sourcePC=0,materialIndex=0,imageIndex=0;
    assets::OriginalUiTournamentPageDiagnostic50KG page{};
    assets::OriginalUiPageDiagnostic50KG descriptor{};
    assets::OriginalUiMaterialDiagnostic50KG material{};
    assets::OriginalUiScreenAssetDiagnostic50KG image{};
    assets::OriginalUiImageBankDiagnostic50KG bank{};
    game::OriginalUiTextGeometryDiagnostic50KG text{};
};
struct OriginalUiTournamentWidgetVideo50KG {
    std::shared_ptr<const void> lifetime;
    std::int32_t width=0,height=0,colorBits=0,packetZMaximum=0;
    std::uint32_t zBufferFormat=0;
};
struct OriginalUiTournamentWidgetColdInput50KG {
    std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,tournament,fonts,controllerPopup,overlays;
    const assets::OriginalUiEnglish50KG* english=nullptr;
    assets::OriginalUiMaterialRuntime50KG runtime{};
    OriginalUiTournamentWidgetVideo50KG video{};
};
struct OriginalUiTournamentPodDescriptor50KG {
    const assets::OriginalUiMemberIdentity50KG* identity=nullptr;
    std::uint16_t widget=0;std::uint8_t career=0;
    std::string slt;
    assets::OriginalUiRiderPodConfig50KG config{};
    bool enableOnSelect=false,disableOnSelect=false,enableOnDeselect=false,disableOnDeselect=false;
    std::array<bool,2> wantsJoystickInput{{false,false}};
    // Genuine borrowed PSG bytes, not a constructed/selected/drawable model.
    assets::OriginalUiMemberView50KG modelSource{};
};
struct OriginalUiTournamentWidgetMaterial50KG {
    std::string registeredName;
    assets::OriginalUiMaterial50KG source;
    std::uint32_t image=assets::unresolvedMaterialTexture50EA;
    assets::OriginalUiNativeMaterial50KG policy{};
    assets::OriginalUiNativeUntexturedMaterial50KG untextured{};
    MaterialTextureSampler50EB sampler{};
    // Exact first Mat_Stbl token; not canonical live row ownership or a collector receipt.
    std::uint32_t registryToken=0;
    const assets::OriginalUiMemberIdentity50KG* registryIdentity=nullptr;
};
struct OriginalUiTournamentPopupTexture50KG {
    std::uint32_t objectOffset=0,constructorPC=0,materialIndex=0,imageIndex=0;
    game::OriginalUiOverlayQuad50KG initialQuad{};
    // Source1FF020 creates all seven, even when the popup is then hidden.
    // Multiple occurrences remain distinct references to one material row.
};
struct OriginalUiTournamentPopupTextClone50KG {
    std::uint16_t sourceWidget=0;
    const assets::OriginalUiMemberIdentity50KG* materialIdentity=nullptr;
    const assets::OriginalUiFont50KG* font=nullptr;
    const assets::Texture48B* atlas=nullptr;
    std::string text;
    float x10=0,y14=0;
    assets::OriginalUiTextJustification50KG justification50=assets::OriginalUiTextJustification50KG::left;
    std::uint32_t color54=0;
    std::uint8_t capacity5B=0,characterCount5C=0,allocatedPackets5D=0,activePackets5E=0;
};
// Retained cold resource population for all 63 genuine UITourn widget slots.
// Empty5/12/19/62 remain null factory slots, including the Control2 reference
// to62. All dynamic texts retain their original capacity and EMPTY initial
// state; no profile/name/next-course text is invented. User_Rotate descriptors
// retain authentic model bytes but do not eagerly construct 23 dynamic models.
// This owner supplies no visibility, callback acceptance, canonical material
// references, complete-frame seal, popup enable state or route admission.
class OriginalUiTournamentWidgetResources50KG final {
public:
    OriginalUiTournamentWidgetResources50KG() noexcept;
    ~OriginalUiTournamentWidgetResources50KG();
    OriginalUiTournamentWidgetResources50KG(const OriginalUiTournamentWidgetResources50KG&)=delete;
    OriginalUiTournamentWidgetResources50KG& operator=(const OriginalUiTournamentWidgetResources50KG&)=delete;
    bool load(const OriginalUiTournamentWidgetColdInput50KG&,OriginalUiTournamentWidgetResourceDiagnostic50KG&) noexcept;
    bool loaded() const noexcept;
    const assets::OriginalUiTournamentPageData50KG* pageData() const noexcept;
    const assets::OriginalUiPage50KG* page() const noexcept;
    const assets::OriginalUiDat50KG* widgetsDat() const noexcept;
    OriginalUiTournamentWidgetKind50KG kind(std::uint16_t) const noexcept;
    bool hasWidget(std::uint16_t) const noexcept;
    // Source manager attachment order; not a complete manager traversal receipt.
    const std::vector<std::uint16_t>* order(bool onTop) const noexcept;
    const assets::OriginalUiOverlayText50KG* text(std::uint16_t) const noexcept;
    const assets::OriginalUiIcon50KG* icon(std::uint16_t) const noexcept;
    const OriginalUiTournamentPodDescriptor50KG* pod(std::uint16_t) const noexcept;
    const assets::OriginalUiMovementControl50KG* movement() const noexcept;
    const game::OriginalUiIconLayout50KG* iconLayout(std::uint16_t) const noexcept;
    // Null for dynamic widgets: a real SetText owner must provide their strings.
    const game::OriginalUiTextGeometry50KG* staticTextGeometry(std::uint16_t) const noexcept;
    bool prepareDynamicText(std::uint16_t,const std::vector<std::string>&,
        game::OriginalUiTextGeometry50KG&,OriginalUiTournamentWidgetResourceDiagnostic50KG&) const noexcept;
    const std::array<OriginalUiTournamentWidgetMaterial50KG,originalUiTournamentWidgetMaterialCount50KG>* materials() const noexcept;
    std::uint32_t widgetMaterial(std::uint16_t) const noexcept;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images() const noexcept;
    const assets::OriginalUiImageBankInput50KG* bankInput() const noexcept;
    const assets::OriginalUiImageBank50KG* sourceImageBank() const noexcept;
    const std::vector<MaterialTextureSampler50EB>* samplers() const noexcept;
    game::OriginalUiExitText50KG* exitText() noexcept;
    // Actual1FF020/2037D0 copied ScreenText resources, not reparented widgets.
    // Caller retains this owner through every clone/geometry/atlas borrow.
    const std::array<std::vector<OriginalUiTournamentPopupTextClone50KG>,9>* popupClones438() const noexcept;
    const std::array<OriginalUiTournamentPopupTexture50KG,7>* popupTextures438() const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
