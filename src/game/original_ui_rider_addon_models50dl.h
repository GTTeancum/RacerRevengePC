#pragma once
// Add-on pod menu models for the free-play Rider screen (DLC menu-art step A).
// PORT EXTENSION, NOT ORIGINAL BEHAVIOUR: the original Rider page has 23 authored
// rider controls and no add-on content.
//
// The ONE place that defines the add-on model table: one row per admitted
// add-on pod, in registry (menu) order, at most RegistryLimits::pods rows.
// Rows are created once while the Rider session is still private (cold) and
// never grow afterwards. A row is never a stock rider's widget or slot.
//
// Each row holds the pod's own menu archive (UI/<STEM>_UI.RES, opened through
// dlc50DL::host and decoded within RegistryLimits::uiArchive*/uiEntries, members
// by name only), its pod model (ui.rider_model), the materials and textures that
// model needs, and its User_Rotate graph. PORT STAND-IN: the placement (position,
// look, up, rotation speed) is a read-only copy of one authored stock Rider pod
// widget, because an add-on package has no Rider widget of its own.
// A row that cannot be prepared stays in the table with ready=false and a reason;
// it draws nothing.
#include "original_ui_rider_model_scene50kg.h"
#include "../assets/original_ui_rider_model_materials50kg.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rr::game {

class OriginalUiRiderAddonModels50DL final {
public:
    static constexpr std::uint32_t noOwner=0xffffffffu;
    struct Row {
        unsigned number=0;                    // registry add-on number (32..99)
        std::string key;                      // "<package-id>/<item-id>"
        std::shared_ptr<const assets::OriginalUiArchive50KG> archive;  // identity addonUi
        std::shared_ptr<const assets::OriginalUiRiderPodAssets50KG> pod;
        std::shared_ptr<const assets::OriginalUiRiderModelMaterials50KG> materials;
        std::unique_ptr<OriginalUiRiderPodScene50KG> scene;
        std::uint32_t imageOwner=noOwner;     // bank owner index, set by the Rider backend seal
        bool ready=false;
        std::string refused;                  // why the row draws nothing ("" when ready)
        // Menu-art step B: the rider's character (ui.character: <stem>c psg/mcf/vut
        // and the six motions) with its character-page materials. The graph is
        // built when the add-on is highlighted and destroyed when it leaves, as
        // the original loads a selected character and unloads a deselected one.
        std::shared_ptr<const assets::OriginalUiRiderCharacterAssets50KG> characterAssets;
        std::shared_ptr<const assets::OriginalUiRiderModelMaterials50KG> characterMaterials;
        std::unique_ptr<OriginalUiRiderCharacterScene50KG> character;
        std::uint32_t characterImageOwner=noOwner;
        std::uint16_t characterLayout=0;      // widget the character assets were placed with
        bool characterLayoutFixed=false;      // from ui.layout_slot (never re-read)
        std::string mcf;
        bool characterReady=false;
        std::string characterRefused;
        // Menu-art step C: the portrait (ui.portraits) and the background
        // (ui.background) from the add-on archive, one image owner (index 0 =
        // portrait, 1 = background when present). Materials are the original
        // constructions: portrait = ScreenTexture missing-PSM fallback, background
        // = its own PSM, as the stock NN_flrg / NN_bkg rows.
        std::shared_ptr<assets::OriginalUiScreenAssets50KG> pageImages;
        assets::OriginalUiMaterial50KG portraitMaterial,backgroundMaterial;
        std::string portraitName,backgroundName;  // registered "ui/textures/<leaf>"
        std::uint32_t portraitImage=noOwner,backgroundImage=noOwner,pageImageOwner=noOwner;
        bool portraitReady=false,backgroundReady=false;
        std::string pageRefused;
    };
    struct Input {
        std::shared_ptr<const assets::OriginalUiArchive50KG> stockRider,appStart,podTextures;
        assets::OriginalUiMaterialRuntime50KG materialRuntime{};
        const assets::OriginalUiIpumTextureCache50KG* ipumCache=nullptr;
        std::uint16_t layoutWidget=0;         // authored stock User_Rotate pod widget (placement copy)
        std::uint16_t characterLayoutWidget=0;// authored stock Soultree_Character widget (placement copy)
        // Authored Rider widgets per stock rider (0 = none), for ui.layout_slot.
        std::array<std::uint16_t,32> podWidgets{},characterWidgets{};
    };
    OriginalUiRiderAddonModels50DL() noexcept=default;
    OriginalUiRiderAddonModels50DL(const OriginalUiRiderAddonModels50DL&)=delete;
    OriginalUiRiderAddonModels50DL& operator=(const OriginalUiRiderAddonModels50DL&)=delete;
    // Once, cold. One row per admitted add-on pod; logs one dlc_rider_model_prepare
    // line per row. False only for misuse (second call, bad input, allocation).
    bool load(const Input&) noexcept;
    bool loaded() const noexcept {return loaded_;}
    std::size_t size() const noexcept {return rows_.size();}
    const Row* row(std::size_t index) const noexcept {return index<rows_.size()?&rows_[index]:nullptr;}
    Row* row(std::size_t index) noexcept {return index<rows_.size()?&rows_[index]:nullptr;}
    // The row of a registry add-on number; nullptr for any other number.
    const Row* byNumber(unsigned number) const noexcept;
    Row* byNumber(unsigned number) noexcept;
    // Backend seal only: record the bank owner of row `index` (bounds-checked).
    bool setImageOwner(std::size_t index,std::uint32_t owner) noexcept;
    bool setCharacterImageOwner(std::size_t index,std::uint32_t owner) noexcept;
    bool setPageImageOwner(std::size_t index,std::uint32_t owner) noexcept;
    // Build the row's character graph (entry motion sampled, attached, IK bound)
    // with the same shared motion/IK/libm owners the stock characters use, or
    // destroy it. start refuses a row without a ready character or a live graph.
    // characterLayoutWidget: the authored stock Soultree_Character widget that
    // places the character for this highlight (used when the row has no
    // ui.layout_slot; the assets are re-read with it when it differs).
    bool startCharacter(std::size_t index,std::uint16_t characterLayoutWidget,ModelMotionSharedState49T&,ModelMode0IkShared50KG&,
        SourceLibmState49T&,OriginalUiRiderModelDiagnostic50KG&) noexcept;
    void stopCharacter(std::size_t index) noexcept;
    OriginalUiRiderCharacterScene50KG* liveCharacter(std::size_t index) noexcept;
    // Destroys every graph (no lifetime rows are held). Idempotent.
    void drain() noexcept;
private:
    std::vector<Row> rows_;
    std::shared_ptr<const assets::OriginalUiArchive50KG> stockRider_;
    bool loaded_=false;
};

} // namespace rr::game
