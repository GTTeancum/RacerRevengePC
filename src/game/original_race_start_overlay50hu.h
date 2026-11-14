#pragma once
// Original PodRaceStartOverlay (class name 3A9160, type info 3A9198, vtable
// 3CE720): the 3 / 2 / 1 / GO lights drawn during the race-start countdown.
// One owner type for every course and every race (human and all-AI): the race
// load 1A8A40 builds it unconditionally at 1A8C40 (race+D4) and attaches it at
// 1A8C78. Native race-lifetime owner; no guest memory is read or written.
//
// Source map (SLUS_202.68):
//   196F20 constructor: age340 = 0.75, once-flags 344/348/34C/350 = 0.
//   197040 load (vtable+30): eight ScreenTextures 2DF0B0 at +40,+A0,+100,+160,
//          +1C0,+220,+280,+2E0 from 321_1, 321_1h, 321_2, 321_2h, 321_3,
//          321_3h, 321_go, 321_goh (strings 3A9010..3A9048). No PSM exists for
//          any of them; 2DF10C associates ScreenMT (2475C0 fallback).
//   1976B0 attach: +30 = race, +354 = race+8C view mode, then 1976E0(this,1).
//   1976E0 layout: per-texture top-left positions plus the view-mode offset.
//   198190 update (vtable+38): while raceB4 flags58 bit1 (racing), age -= dt.
//   198270 layout tick: while age >= 0, 1976E0(this, Application+268 + 1).
//   1982C0 draw (vtable): second view only with two race views; while
//          age >= 0, flags58 bit2 (counting) and race+29 bit2 clear: 197C10.
//   197C10 draw state: "3" lit while countdown34 < 3.0, "2" < 2.0, "1" < 1.0,
//          "GO" lit once flags58 bit1; lit alpha = trunc(255*f), f = age/0.75
//          (1 while age > 0.75), lit sprites rise by 100*(1-f) in view mode 0.
//          First lit frame of each plays race sound 1A4B80 event 3/2/1/4
//          (197DA4 / 197EE0 / 19801C / 198150), once each (344/348/34C/350).
//   1983A0 reset (called from the in-place restart 1AC8E0 at 1ACDF0).
#include "../assets/original_ui_archive50kg.h"
#include "../assets/original_ui_material50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include <array>
#include <cstdint>
#include <memory>

namespace rr::game {
struct OriginalRaceStartOverlayDiagnostic50HU {
    enum class Error:std::uint8_t {none,alreadyLoaded,archive,material,images,state,allocation} error=Error::none;
    std::uint32_t sourcePC=0;
    std::size_t texture=0;
    assets::OriginalUiMaterialDiagnostic50KG material;
    assets::OriginalUiScreenAssetDiagnostic50KG images;
};
// Values the overlay reads through race+30 and the Application. The caller
// copies them from their single port owners; none is invented here.
struct OriginalRaceStartOverlayInputs50HU {
    float countdown34=0;              // raceB4 +34 (1AFD80 / 1AFA84 clock)
    std::uint8_t flags58=0;           // raceB4 +58: bit1 racing, bit2 counting
    std::uint8_t raceFlags1B0=0;      // race28C +1B0: low two bits = view count
    std::uint8_t raceEntityFlags29=0; // race +29: bit2 suppresses the draw
    std::uint32_t drawnView268=0;     // Application +268: view being drawn
};
// One 2DF550 ScreenTexture quad, in the 640x448 source canvas. Top-left x/y,
// size from the texture, full UV, PS2 packet colour (RGB 255>>1, alpha a>>2).
struct OriginalRaceStartOverlaySprite50HU {
    float x=0,y=0,width=0,height=0;
    std::array<std::uint8_t,4> color{{127,127,127,63}};
    std::uint8_t texture=0;
};
struct OriginalRaceStartOverlayDraw50HU {
    static constexpr std::size_t maximumSprites=4;
    std::array<OriginalRaceStartOverlaySprite50HU,maximumSprites> sprites{};
    std::uint32_t count=0;
    // 1A4B80 events requested by THIS draw, in source order (3,2,1,4).
    std::array<std::uint32_t,4> sounds{};
    std::uint32_t soundCount=0;
    // Which digit/GO is lit this draw: 0 none, 3/2/1, 4 = GO.
    std::uint32_t lit=0;
    std::uint64_t serial=0;
    bool drawn=false;
};
class OriginalRaceStartOverlay50HU final {
public:
    static constexpr std::size_t textureCount=8;
    OriginalRaceStartOverlay50HU() noexcept; // 196F20 field initialization
    ~OriginalRaceStartOverlay50HU();
    OriginalRaceStartOverlay50HU(const OriginalRaceStartOverlay50HU&)=delete;
    OriginalRaceStartOverlay50HU& operator=(const OriginalRaceStartOverlay50HU&)=delete;
    // 197040: authenticated APPSTART (ScreenMT template) and OVERLAYS only.
    bool load197040(std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,
        std::shared_ptr<const assets::OriginalUiArchive50KG> overlays,
        const assets::OriginalUiMaterialRuntime50KG&,OriginalRaceStartOverlayDiagnostic50HU&) noexcept;
    bool loaded() const noexcept;
    // 1976B0: viewMode is the race's +8C. Repeated attach is refused.
    bool attach1976B0(std::uint32_t viewMode8C) noexcept;
    bool attached() const noexcept;
    // 1983A0: age 0.75, all four once-flags cleared.
    void reset1983A0() noexcept;
    // 198190: once per race step, after the lap manager step 1AF9C0.
    bool update198190(const OriginalRaceStartOverlayInputs50HU&,float dt) noexcept;
    // 198270 + 1982C0 + 197C10 for one drawn view. Fills nativeDraw(); the
    // caller plays nativeDraw().sounds through the race sound owner (1A4B80).
    bool draw198300(const OriginalRaceStartOverlayInputs50HU&) noexcept;
    const OriginalRaceStartOverlayDraw50HU& nativeDraw() const noexcept;
    float age340() const noexcept;
    const assets::OriginalUiScreenAssets50KG* images() const noexcept;
    const assets::OriginalUiMaterial50KG* material(std::size_t) const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
