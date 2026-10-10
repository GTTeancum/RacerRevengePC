#pragma once
#include "original_ui_navigation50kg.h"
#include <array>
#include <memory>

namespace rr::game {
// NEW source reconstruction. This is the authenticated Rider selection/model
// layer, NOT a destination admission, renderer, or completed page controller.
// Existing route_decisions50kg owns confirm/back profile proposals.
enum class OriginalUiRiderSelectionStatus50KG : std::uint8_t {
    ok,noChange,alreadyLoaded,invalidArchive,invalidPage,invalidPlayer,
    invalidSelection,invalidProfile,missingProfile,noEligibleRider,
    unsupportedInput,capacity,parseFailure,allocation
};
struct OriginalUiRiderSelectionDiagnostic50KG {
    OriginalUiRiderSelectionStatus50KG status=OriginalUiRiderSelectionStatus50KG::ok;
    assets::OriginalUiPageDiagnostic50KG parse{};
    std::uint32_t sourcePC=0;
};
class OriginalUiRiderSelectionData50KG final {
public:
    OriginalUiRiderSelectionData50KG()=default;
    OriginalUiRiderSelectionData50KG(const OriginalUiRiderSelectionData50KG&)=delete;
    OriginalUiRiderSelectionData50KG& operator=(const OriginalUiRiderSelectionData50KG&)=delete;
    // Keeps the actual authenticated archive and exact shared rider.dat alive.
    // Both factory page1 and page2 use this PageNo1 file; player is separate.
    bool load(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        OriginalUiRiderSelectionDiagnostic50KG&) noexcept;
    const assets::OriginalUiPage50KG* page() const noexcept { return archive_?&page_:nullptr; }
    std::int32_t controlRider(std::uint16_t control) const noexcept {
        return archive_ && control<page_.controls.size()?riders_[control]:-1;
    }
private:
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive_;
    assets::OriginalUiPage50KG page_{};
    std::array<std::int32_t,assets::originalUiControlLimit50KG> riders_{};
};
struct OriginalUiRiderProfileRow50KG {
    bool ownerReady=false;
    std::uint32_t unlock10FAC=0,hidden730=0;
    // Source byte at profile+0x1938+rider*(0x894+0x50), NOT simply rider*0x894.
    bool careerByteOwnerReady=false;
    std::uint8_t career1938=0;
};
struct OriginalUiRiderSelectionProfile50KG {
    bool ownerReady=false;
    std::uint32_t mode34=0;
    std::uint32_t remembered6F0=0,remembered6F4=0,remembered6F8=0,remembered6FC=0;
    std::array<OriginalUiRiderProfileRow50KG,32> riders{};
};
struct OriginalUiRiderWidgetEnable50KG {
    std::uint16_t widget=0; // Original one-based widget index.
    bool enable=false;
    std::uint32_t sourcePC=0;
};
struct OriginalUiRiderAppearance50KG {
    // Exact ordered 2A7A00 calls. Absence means preserve the prior widget state.
    std::array<OriginalUiRiderWidgetEnable50KG,6> widgets{};
    std::size_t count=0;
    std::uint32_t promptMask42C=0;
    // Before widget calls, apply actual saved lighting520/cameraTint530 when
    // unlocked; otherwise actual locked lighting500/cameraTint510. These are
    // references to the page's real captured colors, never replacement art.
    bool useSavedColors=false;
};
struct OriginalUiRiderSelectionPlan50KG {
    // Immutable proposal: consumer must atomically admit all real visual and
    // profile dependencies, then replay callbacks in source order. Merely
    // assigning nextNavigation would discard dynamic-load/preview effects.
    OriginalUiNavigation50KG nextNavigation{};
    OriginalUiCommands50KG navigationCommands{};
    OriginalUiRiderAppearance50KG appearance{};
    std::uint32_t rider4C4=0;
    std::uint16_t slot4C8=0;
    std::uint8_t player4C0=0;
    std::size_t visitedControls=0;
    bool captureAndRestorePodTransform=false;
    // Moving calls 2A5A00 on the final selected control after appearance, then
    // restores its first widget's +0x60 transform and sets page4EC=3. The
    // frame-3 render work, sound2 and transport267BE0 are separate real owners.
    bool resetSelectedControl=false;
    std::uint32_t deferred4EC=0,sourcePC=0;
};
// 1E9C50 entry's remembered-rider search: last matching slot wins. No unlock
// filtering. Unknown mode/no matching rider rejects, never falls back to slot1.
// Proposal includes the later openingAC group-entry commands (1E9FD4/1E9FEC),
// NOT the other entry/opening side effects. Caller must keep their real order.
OriginalUiRiderSelectionStatus50KG planOriginalUiRiderEntry50KG(
    const OriginalUiRiderSelectionData50KG&,const OriginalUiNavigation50KG&,
    std::uint8_t player,const OriginalUiRiderSelectionProfile50KG&,
    OriginalUiRiderSelectionPlan50KG&) noexcept;
// Actual event IDs:0=>previous (1EB370/2A51B0),1=>next (1EB1F0/2A50F0).
// Wrong controller/popup/4E0 block produces noChange with empty output.
// Failed calls preserve output and input state. Every intermediate skipped
// control's select/deselect commands are retained, not collapsed away.
OriginalUiRiderSelectionStatus50KG planOriginalUiRiderMove50KG(
    const OriginalUiRiderSelectionData50KG&,const OriginalUiNavigation50KG&,
    std::uint8_t player,std::uint8_t controller,std::uint8_t originalEvent,
    bool popupActive,bool blocked4E0,const OriginalUiRiderSelectionProfile50KG&,
    OriginalUiRiderSelectionPlan50KG&) noexcept;
// 1EAFB0's exact unlock-dependent widget visibility/prompt projection.
OriginalUiRiderSelectionStatus50KG projectOriginalUiRiderAppearance50KG(
    std::uint32_t rider,const OriginalUiRiderSelectionProfile50KG&,
    OriginalUiRiderAppearance50KG&) noexcept;
} // namespace rr::game
