#pragma once
#include "original_ui_pages50kg.h"
#include "original_ui_localization50kg.h"

namespace rr::assets {
enum class OriginalUiTextJustification50KG : std::uint8_t {
    left=0,centerHorizontal=1,centerVertical=2,centerBoth=3,right=4
};
enum class OriginalUiTextSource50KG : std::uint8_t {
    localized,authoredLines,authoredText,dynamic
};
struct OriginalUiOverlayText50KG {
    const OriginalUiMemberIdentity50KG* identity=nullptr;
    std::uint16_t widgetId=0;
    std::string fontName,materialName,textId;
    OriginalUiTextJustification50KG justification=OriginalUiTextJustification50KG::left;
    OriginalUiTextSource50KG textSource=OriginalUiTextSource50KG::authoredText;
    std::int32_t x=0,y=0,height=0,alpha=255,dynamicLineLength=0;
    float leadingFactor=1,depth=1;
    std::uint32_t color=0xff282828,highlightColor=0xffffffff,disabledColor=0xff808080;
    bool hasHighlightColor=false,hasDisabledColor=false;
    bool enableOnSelect=false,disableOnSelect=false,enableOnDeselect=false,disableOnDeselect=false;
    std::array<bool,2> wantsJoystickInput{{false,false}}; // Manager 0x2A44A0.
    // PodUIManager 1CD890, integer default 0: nonzero defers attachment.
    // Manager child order is ordinary widgets ascending, intervening original
    // setup/page children, then OnTop widgets descending. This is tree order,
    // not a material/depth/alpha override or a page membership designation.
    bool onTop=false;
    // Manager 0x2A44E0 recursively hides every newly created widget; initial
    // visible membership comes from page callbacks, not a synthesized flag.
    // Original one-byte character strings. TextID and Height-triggered Text
    // split at '@'; authored Line1..N do not. Dynamic lines start empty.
    std::vector<std::string> lines;
    bool usesExplicitHeight=false; // Only source SetText/split branch uses Height.
};
// Source Overlay_Text::Create 0x2A9000. Widget sections are "Widget N".
// Actual DAT data and optional authenticated English lookup are required.
// Missing required text/font references fail; no fallback labels are supplied.
// Color uses source CVT.W.S truncation toward zero then packed 0xFFRRGGBB. Native
// admission restricts RGB to [0,255], coordinates to safe signed16, 64 lines,
// and each glyph run to 255 bytes. These are bounds, not authored defaults.
// Alpha remains authored here; source TextOverlay setter clamps to 0..255.
bool readOriginalUiOverlayText50KG(const OriginalUiDat50KG& widgets,
    std::uint16_t widgetId,const OriginalUiEnglish50KG* english,
    OriginalUiOverlayText50KG&,OriginalUiPageDiagnostic50KG&) noexcept;
// Explicit source-prefix RGB adapter for the authenticated Garage Widgets.Dat
// only. Source158C00 tokenizes X/Y at comma and Z at newline, then302E10 reads
// a numeric prefix. Garage17..22 therefore use RGB104 despite the original
// fourth ",104" suffix. That suffix remains in the DAT; it is not alpha.
// The generic strict entry above and originalUiTriple50KG remain unchanged.
bool readOriginalUiGarageOverlayText50KG(const OriginalUiDat50KG& widgets,
    std::uint16_t widgetId,const OriginalUiEnglish50KG* english,
    OriginalUiOverlayText50KG&,OriginalUiPageDiagnostic50KG&) noexcept;
}
