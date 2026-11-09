#pragma once
// UIWin (screen 7, PodUICircuitWin, page 1 PodUIPageCircuitWin 2076A0).
// Native, source-ordered page logic only: no GPU, no audio device, no profile
// writes. Every value comes from the authenticated UIWIN.RES members or from
// the caller's real profile facts. Unknown source states fail closed.
#include "original_ui_aperture_owner50kg.h"
#include "original_ui_text_widget50kg.h"
#include "original_ui_icon_layout50kg.h"
#include "original_unlock_checks50pd.h"
#include "original_ui_route_decisions50kg.h"
#include "../assets/original_ui_overlay_text50kg.h"
#include "../assets/original_ui_icon50kg.h"
#include "../assets/original_ui_localization50kg.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

namespace rr::game {
inline constexpr std::uint8_t originalUiWinScreen50PD=7,originalUiWinPageId50PD=1;
// Widgets.Dat [General] NumberOfWidgets=14, plus unused slot 0.
inline constexpr std::size_t originalUiWinWidgetStorage50PD=15;
// Source widget numbers (Widgets.Dat), fixed by win.dat Control 0..3.
inline constexpr std::uint16_t originalUiWinContinue50PD=1,originalUiWinTitle50PD=3,
    originalUiWinCongratulations50PD=4,originalUiWinMessage50PD=5;
inline constexpr std::array<std::uint16_t,3> originalUiWinOverlayIcons50PD{{6,7,8}};
// Control 1/2/3 = place 1/2/3: {trophy, star}.
inline constexpr std::array<std::array<std::uint16_t,2>,3> originalUiWinPlaceModels50PD{{{{9,10}},{{11,12}},{{13,14}}}};

enum class OriginalUiWinWidgetKind50PD:std::uint8_t {absent,empty,text,icon,rotate};
enum class OriginalUiWinPageError50PD:std::uint8_t {
    none,notBound,alreadyBound,archive,parse,population,input,callback,text,icon,
    profile,circuit,localization,aperture,arithmetic,hidden,allocation
};
struct OriginalUiWinPageDiagnostic50PD {
    OriginalUiWinPageError50PD error=OriginalUiWinPageError50PD::none;
    assets::OriginalUiPageDiagnostic50KG parse{};
    std::uint16_t widget=0;std::size_t callback=0;std::uint32_t sourcePC=0;
};
// Widgets.Dat User_Rotate (1D31D0 reader, 1D38B0 update). Velocity is in
// revolutions per second (1D38B0 multiplies by 2pi 0x40C90FDB).
struct OriginalUiWinRotateConfig50PD {
    std::string slt,stem;
    std::array<float,3> position{},look{},up{},velocity{};
    std::array<bool,3> rotate{};
    bool lit=false,startPaused=true,cleanFromMemory=false,dynamicLoad=false;
    bool enableOnSelect=false,disableOnSelect=false,enableOnDeselect=false,disableOnDeselect=false;
};
struct OriginalUiWinWidgetState50PD {
    OriginalUiWinWidgetKind50PD kind=OriginalUiWinWidgetKind50PD::absent;
    bool selected=false,shown=false;
    // User_Rotate DynamicLoad: the model exists only while selected.
    bool loaded=false;
};
// Circuit derived exactly as entry 2084C0: 187B10 order+1 is 4/8/13, the
// place is M[C][C][3/7/12]. Any other order asserts in the original
// ("No Tier was completed..."); place outside 1..3 asserts in 2A6BC0.
struct OriginalUiWinCircuit50PD {std::uint8_t circuit=0,place=0;};
bool resolveOriginalUiWinCircuit50PD(const OriginalUnlockFacts50PD&,
    OriginalUiWinCircuit50PD&,std::uint32_t& failPC) noexcept;
// Render 208AA0: sprintf("%s %s@%s", loc[160], loc[place], loc[circuit]).
// Place text 145/146/147, circuit text 161/162/163. Original defect kept:
// 2nd place in circuit 3 reads loc[147] (0x20946C, offset 0x498).
bool buildOriginalUiWinMessage50PD(const assets::OriginalUiEnglish50KG&,
    OriginalUiWinCircuit50PD,std::string& combined,std::uint32_t& failPC) noexcept;

enum class OriginalUiWinPhase50PD:std::uint8_t {
    requestedPage90,previousPage94,entry74,openingAC,openedB0,closingB4,closingThresholdB8,closedBC
};
enum class OriginalUiWinMusic50PD:std::uint8_t {none,song10Once,song8Once,fade1s};
struct OriginalUiWinCallback50PD {
    std::uint16_t widgetId=0;
    OriginalUiControlOperation50KG operation=OriginalUiControlOperation50KG::hide;
    bool value=false;
    // true = nonvirtual UIWidget::SetEnabled 2A7A00 (not the text +80/+84).
    bool controlEnable=false;
    std::uint32_t sourcePC=0;
};
// Pure ordered proposal. The session preflights every effect before applying.
struct OriginalUiWinPlan50PD {
    std::array<OriginalUiWinCallback50PD,64> callbacks{};
    std::size_t callbackCount=0;
    OriginalUiNavigation50KG navigation{};
    bool writesPromptMask42C=false;std::uint32_t promptMask42C=0;
    bool activateCamera=false,clearScene3D8=false,applyBackground430=false;
    bool writesCircuit650=false;OriginalUiWinCircuit50PD circuit{};
    OriginalUiWinMusic50PD music=OriginalUiWinMusic50PD::none;
    std::uint32_t musicPC=0;
};
// Input 2089D0. Only event4 on controller0 acts; it requests screen 10 when
// 187E30 is pending, else 13 (vtable+A0 = 1D6130), then UI sound 1.
struct OriginalUiWinInputResult50PD {
    bool pageAccepted=false,routeRequested=false;
    OriginalUiRouteDecision50KG route{};
    std::uint32_t soundId=0,soundPC=0,flushPC=0;
};
class OriginalUiWinPage50PD final {
public:
    OriginalUiWinPage50PD() noexcept;~OriginalUiWinPage50PD();
    OriginalUiWinPage50PD(const OriginalUiWinPage50PD&)=delete;OriginalUiWinPage50PD& operator=(const OriginalUiWinPage50PD&)=delete;
    // Authenticated UIWIN.RES only: ui.dat Page 1 -> win.dat, Widgets.Dat.
    bool bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,const assets::OriginalUiEnglish50KG*,
        OriginalUiWinPageDiagnostic50PD&) noexcept;
    bool bound()const noexcept;
    const assets::OriginalUiPage50KG* page()const noexcept;
    const assets::OriginalUiDat50KG* widgetsDat()const noexcept;
    const assets::OriginalUiDat50KG* uiDat()const noexcept;
    const assets::OriginalUiDat50KG* pageDat()const noexcept;
    const std::vector<std::uint16_t>* order()const noexcept; // ascending ordinary widgets
    OriginalUiWinWidgetState50PD state(std::uint16_t)const noexcept;
    const assets::OriginalUiOverlayText50KG* textDescriptor(std::uint16_t)const noexcept;
    const assets::OriginalUiIcon50KG* iconDescriptor(std::uint16_t)const noexcept;
    const OriginalUiWinRotateConfig50PD* rotateConfig(std::uint16_t)const noexcept;
    const OriginalUiTextWidget50KG* textWidget(std::uint16_t)const noexcept;
    const OriginalUiNavigation50KG* navigation()const noexcept;
    std::uint32_t promptMask42C()const noexcept;
    OriginalUiWinCircuit50PD circuit()const noexcept; // page+650 and its place
    bool entered()const noexcept;
    // Selected group-1 slot as 2A6FA0 reads it at render time.
    std::uint16_t selectedPlace()const noexcept;

    bool phase(OriginalUiWinPhase50PD,const OriginalUiWinCircuit50PD* entryCircuit,
        OriginalUiWinPlan50PD&,OriginalUiWinPageDiagnostic50PD&)const noexcept;
    bool preflight(const OriginalUiWinPlan50PD&,OriginalUiWinPageDiagnostic50PD&)const noexcept;
    bool apply(const OriginalUiWinPlan50PD&,OriginalUiWinPageDiagnostic50PD&) noexcept;
    bool input(std::uint8_t controller,std::uint8_t originalInput,bool unlockPending187E30,
        OriginalUiWinInputResult50PD&,OriginalUiWinPageDiagnostic50PD&)const noexcept;

    // Text owner calls used by the page render 208AA0.
    bool setWidgetColor(std::uint16_t,std::uint32_t,OriginalUiWinPageDiagnostic50PD&) noexcept;
    // SetText 2A9FF0 -> 2A9DE0: '@' splits, preserving a trailing empty line;
    // 29D940 per line, 29DA40 setup only on lines that received text.
    bool setDynamicText(std::uint16_t,const std::string&,OriginalUiWinPageDiagnostic50PD&) noexcept;
    const std::vector<std::string>* dynamicLines(std::uint16_t)const noexcept;
    std::uint64_t dynamicRevision(std::uint16_t)const noexcept;
    bool updateText(std::uint16_t,bool ancestorsShown,OriginalUiWinPageDiagnostic50PD&) noexcept;
    bool projectText(std::uint16_t,std::size_t line,float fontHeight,std::int32_t width,std::int32_t height,
        bool ancestorsShown,OriginalUiTextPosition50KG&,OriginalUiTextLineProjection50KG&,
        OriginalUiWinPageDiagnostic50PD&) noexcept;
    // Icon 2AC590 binding: real atlas dimensions of the material's first image.
    bool prepareIcon(std::uint16_t,const assets::Texture48B& atlas,std::int32_t videoDepthMaximum,
        OriginalUiWinPageDiagnostic50PD&) noexcept;
    const OriginalUiIconLayout50KG* iconLayout(std::uint16_t)const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
