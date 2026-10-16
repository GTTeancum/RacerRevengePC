#pragma once
#include "original_ui_options_menu_controller50kg.h"
#include "control_presets49d.h"

namespace rr::game {
// One authenticated config.dat supplies factory pages12/2 and12/3. PageNo in
// that member remains2 for BOTH; the factory controller index distinguishes them.
struct OriginalUiControlsState50KG {
    OriginalUiNavigation50KG navigation{};
    std::uint8_t controllerIndex4DC=0;
    bool frontView4CC=true,modelBlocked4D0=false,loaded=false;
    std::uint32_t configuration4D4=0,preset4D8=0;
};
enum class OriginalUiControlsPhase50KG : std::uint8_t {
    requestedPage90,previousPage94,entry74,openingAC,openedB0,closingB4,closingThresholdB8
};
enum class OriginalUiControlsIntent50KG : std::uint8_t {none,accept,cancel};
enum class OriginalUiControlsEffectKind50KG : std::uint8_t {
    widget,localizedText,model,scene,previewPresets,drawBackground,renderAperture
};
enum class OriginalUiControlsModelAction50KG : std::uint8_t {
    //28E8A0 replaces Widget36.model+4 root's visibility observer using
    // manager2B4->4C0[0] and method filter2 (scene+38). Then1556C0 restores
    // each model transform from its load-time saved matrix.
    bindEntryModel,restoreEntryTransforms,
    // Exact names "controller" and "controller2", then154480 with binary32
    // bits3A83126F and a1=a2=0. This is an actual model update, not a timer.
    selectBackMotion,selectFrontMotion,advanceRotation001
};
enum class OriginalUiControlsSceneAction50KG : std::uint8_t {
    // Entry1F38DC: if borrowed camera4BC exists, +20 then app+27C/+44
    // with that camera and three zeros. Preserve real source owner identity.
    activateBorrowedCamera,
    // Entry1F3930: app+2D8->47C->3D8=0. Entry1F393C: global442F70->28=page430.
    clearSceneField3D8,applyPageSceneField430,
    // Actual index1 opening/closingThreshold stores app+2D8+474/+478.
    routeToSecondController,routeToFirstController
};
struct OriginalUiControlsEffect50KG {
    OriginalUiControlsEffectKind50KG kind=OriginalUiControlsEffectKind50KG::widget;
    std::uint16_t widgetId=0; // Authored one-based IDs, NOT pointer-array offsets.
    OriginalUiControlOperation50KG operation=OriginalUiControlOperation50KG::hide;
    bool value=false;
    std::uint16_t stringId=0;
    OriginalUiControlsModelAction50KG modelAction=OriginalUiControlsModelAction50KG::bindEntryModel;
    OriginalUiControlsSceneAction50KG sceneAction=OriginalUiControlsSceneAction50KG::activateBorrowedCamera;
    std::uint8_t ordinal=0;
    std::uint32_t sourcePC=0;
};
struct OriginalUiControlsPlan50KG {
    OriginalUiControlsState50KG state{};
    // Process this one ordered stream. Never process all callbacks and THEN
    // all labels: the actual render interleaves them and the preview commit.
    std::array<OriginalUiControlsEffect50KG,256> effects{};
    std::size_t effectCount=0;
    bool writesPageInputFlags42C=false;
    std::uint32_t pageInputFlags42C=0;
    bool pageAccepted=false,soundRequested=false,flushRequested=false;
    std::uint8_t soundId=0;
    std::uint32_t soundPC=0,flushPC=0;
    OriginalUiControlsIntent50KG intent=OriginalUiControlsIntent50KG::none;
    std::uint32_t requestPC=0;
    // Exact188C10 proposal. Preflight the complete effect stream and verify
    // canonical presets still match previewBefore. At previewPresets effect,
    // publish previewAfter to the SAME shared profile used by racing. A local
    // throwaway preview is not the source stateful operation.
    bool previewPerformed=false;
    ControlPresets49D previewBefore{},previewAfter{};
    ControlActionRow49D previewRow{};
};
class OriginalUiControlsController50KG final {
public:
    OriginalUiControlsController50KG() noexcept;
    ~OriginalUiControlsController50KG();
    OriginalUiControlsController50KG(const OriginalUiControlsController50KG&)=delete;
    OriginalUiControlsController50KG& operator=(const OriginalUiControlsController50KG&)=delete;
    OriginalUiOptionsMenuStatus50KG bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        std::uint8_t controllerIndex,assets::OriginalUiPageDiagnostic50KG&) noexcept;
    const assets::OriginalUiPage50KG* page() const noexcept;
    const assets::OriginalUiDat50KG* widgetsDat() const noexcept;
    bool initialState(OriginalUiControlsState50KG&) const noexcept;
    OriginalUiOptionsMenuStatus50KG phase(const OriginalUiControlsState50KG&,
        OriginalUiControlsPhase50KG,const ControlPresets49D&,OriginalUiControlsPlan50KG&) const noexcept;
    OriginalUiOptionsMenuStatus50KG input(const OriginalUiControlsState50KG&,
        std::uint8_t controller,assets::OriginalUiMovementInput50KG,
        bool containerReached,bool pageInputEnabled268,OriginalUiControlsPlan50KG&) const noexcept;
    // Call at actual page render1F3CE0, after ordinary child widget rendering.
    // animationWord10Nonzero is the real Widget36 ModelPlayback stopped10.
    // Original model constructor15376C initializes this to1; no InitialMotion
    // is authored, so initial stopped state is real constructor behavior.
    // Update1F3CD0 separately saves dt then traverses children; no preview there.
    OriginalUiOptionsMenuStatus50KG render(const OriginalUiControlsState50KG&,
        const ControlPresets49D&,bool modelObservationKnown,bool animationWord10Nonzero,
        OriginalUiControlsPlan50KG&) const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
// Route proposal only. Neither path remembers a group or writes6E4/6E8. Accept
// performs the typed49D selection BEFORE lifecycle7/page1 and sound1; Cancel
// performs no selection and plays5. No disk-save/default behavior is implied.
bool describeOriginalUiControlsReturn50KG(const OriginalUiControlsState50KG&,
    OriginalUiControlsIntent50KG,OriginalUiRouteDecision50KG&) noexcept;
bool originalUiControlsReturnShape50KG(const OriginalUiRouteDecision50KG&) noexcept;
bool applyOriginalUiControlsAccept50KG(const OriginalUiControlsState50KG&,
    const ControlPresets49D&,ControlPresets49D&) noexcept;
bool sameOriginalUiControlsPresets50KG(const ControlPresets49D&,const ControlPresets49D&) noexcept;
} // namespace rr::game
