#pragma once
#include "original_ui_aperture_owner50kg.h"
#include "original_ui_text50kg.h"
#include "../assets/original_ui_localization50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include <memory>
#include <string>

namespace rr::game {
enum class OriginalUiExitTextStatus50KG : std::uint8_t {
    ok,notPrepared,alreadyPrepared,unsupportedPage,archive,parse,localization,
    resource,textCapacity,geometry,invalidRenderBoundary,allocation
};
struct OriginalUiExitTextDiagnostic50KG {
    OriginalUiExitTextStatus50KG status=OriginalUiExitTextStatus50KG::ok;
    std::uint32_t sourcePC=0;
    assets::OriginalUiPageDiagnostic50KG parse{};
    OriginalUiTextDiagnostic50KG text{};
};
struct OriginalUiExitTextSnapshot50KG {
    bool prepared=false,packetPrepared=false;
    OriginalUiTransitionRoute50KG route{};
    // Source ScreenText+5B capacity10; +5C is the actual localized byte count.
    std::uint8_t capacity5B=0,characterCount5C=0,activePackets5E=0;
    float positionedX20=0,positionedY24=0;
    std::uint32_t color54=0;
    std::int16_t alpha58=0;
    std::uint64_t renderPreparations=0;
};
struct OriginalUiExitTextProjection50KG {
    const OriginalUiTextLayout50KG* glyphs=nullptr;
    const assets::OriginalUiScreenAssets50KG* images=nullptr;
    const assets::OriginalUiMemberIdentity50KG* materialIdentity=nullptr;
    std::uint32_t imageIndex=0;
    std::array<std::uint8_t,4> packetColor{};
    float assignedDepth=1;
    std::uint32_t callPC=0x1D75A4,renderPC=0x2E0170,submitPC=0x2E02B0;
};

// The real shared-page embedded ScreenText at +440. Its one string is read
// from authenticated English entry89, never supplied as a label or DAT widget.
// It owns cold setup, glyph geometry, and the RenderText packet-preparation
// state. Source2E0170 renders this object; it does NOT destroy/hide a page.
// Actual native collector/material admission and GPU submission stay separate.
class OriginalUiExitText50KG final {
public:
    OriginalUiExitText50KG() noexcept=default;
    OriginalUiExitText50KG(const OriginalUiExitText50KG&)=delete;
    OriginalUiExitText50KG& operator=(const OriginalUiExitText50KG&)=delete;
    OriginalUiExitText50KG(OriginalUiExitText50KG&&)=delete;
    OriginalUiExitText50KG& operator=(OriginalUiExitText50KG&&)=delete;
    // Verified shared-constructor users only: Main4/1, Options12/1,
    // Rider5/1 or5/2, Course6/1, Tournament9/1, Hall11/1, Garage13/1. This does not admit any of those destinations.
    // Follows actual ui.dat PageFile; no filename search or fabricated config.
    // Images pin the exact original denmrk16 CEL/PST decode. English is copied
    // synchronously. Failure is cold/atomic/retryable; no effects occur.
    OriginalUiExitTextStatus50KG prepare(OriginalUiTransitionRoute50KG,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const assets::OriginalUiEnglish50KG&,
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,std::uint32_t imageIndex,
        OriginalUiExitTextDiagnostic50KG&) noexcept;
    bool prepared() const noexcept { return state_.prepared; }
    OriginalUiExitTextSnapshot50KG snapshot() const noexcept { return state_; }
    const std::string* text() const noexcept { return prepared()?&text_:nullptr; }
    const assets::OriginalUiMemberIdentity50KG* pageIdentity() const noexcept { return pageIdentity_; }
    // Only the actual aperture's life3 close endpoint may reach this call.
    // Aperture supplies its pre-clamp closing state in the event. The native
    // collector must preflight its capacity/ordering before calling here,
    // then enqueue this packet before either aperture rectangle. No update,
    // clock, elapsed-time completion, timer or callback certification exists.
    OriginalUiExitTextStatus50KG projectRender2E0170(const OriginalUiApertureHook50KG&,
        OriginalUiExitTextProjection50KG&) noexcept;
private:
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive_;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images_;
    const assets::OriginalUiMemberIdentity50KG* pageIdentity_=nullptr;
    const assets::OriginalUiMemberIdentity50KG* materialIdentity_=nullptr;
    OriginalUiExitTextSnapshot50KG state_{};
    OriginalUiTextLayout50KG geometry_{};
    std::string text_;
    std::uint32_t imageIndex_=0;
};
}
