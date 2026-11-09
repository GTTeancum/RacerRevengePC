#pragma once
// Authenticated UIUnlock widget manager (UI/UIUnlock/Widgets.Dat, 53 widgets).
// Same callback/visibility model as the Hall bank. Widgets 9..38 (User_Rotate
// pods and Soultree_Character pilots) keep their real callback state here;
// their 3D drawing belongs to a separate model owner (not in this bank).
#include "original_ui_options_menu_controller50kg.h"
#include "original_ui_icon_layout50kg.h"
#include "original_ui_text_widget50kg.h"
#include "original_ui_text_position50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

namespace rr::game {
inline constexpr std::size_t originalUiUnlockWidgetStorageCount50PD=54; // exact53 + sentinel
enum class OriginalUiUnlockWidgetKind50PD:std::uint8_t {absent,empty,text,icon,userRotate,character};
enum class OriginalUiUnlockWidgetError50PD:std::uint8_t {none,notBound,alreadyBound,archive,parse,population,input,callback,text,icon,missingResource,hidden,capacity,allocation};
struct OriginalUiUnlockWidgetDiagnostic50PD {
    OriginalUiUnlockWidgetError50PD error=OriginalUiUnlockWidgetError50PD::none;
    assets::OriginalUiPageDiagnostic50KG parse{};
    std::uint16_t widget=0;std::size_t callback=0;std::uint32_t sourcePC=0;
};
struct OriginalUiUnlockWidgetState50PD {
    OriginalUiUnlockWidgetKind50PD kind=OriginalUiUnlockWidgetKind50PD::absent;
    bool selected=false,shown=false,onTop=false;
};
struct OriginalUiUnlockTextProjection50PD {
    const assets::OriginalUiOverlayText50KG* descriptor=nullptr;
    const std::string* text=nullptr; // descriptor line, or the live dynamic line
    OriginalUiTextPosition50KG position{};
    OriginalUiTextLineProjection50KG line{};
};
struct OriginalUiUnlockIconProjection50PD {
    const assets::OriginalUiIcon50KG* descriptor=nullptr;
    const assets::OriginalUiScreenAssets50KG* images=nullptr;
    std::uint32_t imageIndex=0;OriginalUiIconLayout50KG layout{};
};
class OriginalUiUnlockWidgets50PD final {
public:
    OriginalUiUnlockWidgets50PD() noexcept;~OriginalUiUnlockWidgets50PD();
    OriginalUiUnlockWidgets50PD(const OriginalUiUnlockWidgets50PD&)=delete;
    OriginalUiUnlockWidgets50PD& operator=(const OriginalUiUnlockWidgets50PD&)=delete;
    bool bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,const assets::OriginalUiEnglish50KG*,
        OriginalUiUnlockWidgetDiagnostic50PD&) noexcept;
    const assets::OriginalUiDat50KG* widgetsDat() const noexcept;
    const std::vector<std::uint16_t>* order(bool onTop) const noexcept;
    OriginalUiUnlockWidgetState50PD state(std::uint16_t) const noexcept;
    const assets::OriginalUiOverlayText50KG* textDescriptor(std::uint16_t) const noexcept;
    const assets::OriginalUiIcon50KG* iconDescriptor(std::uint16_t) const noexcept;
    const OriginalUiTextWidget50KG* textWidget(std::uint16_t) const noexcept;
    bool dynamicText(std::uint16_t) const noexcept;
    const std::vector<std::string>* dynamicLines(std::uint16_t) const noexcept;
    bool preflight(const OriginalUiOptionsMenuCallback50KG*,std::size_t,OriginalUiUnlockWidgetDiagnostic50PD&) const noexcept;
    bool apply(const OriginalUiOptionsMenuCallback50KG*,std::size_t,OriginalUiUnlockWidgetDiagnostic50PD&) noexcept;
    // Text 2AA7F0 / Icon 2ACE50 colour setters.
    bool setWidgetColor(std::uint16_t,std::uint32_t,OriginalUiUnlockWidgetDiagnostic50PD&) noexcept;
    // 2A9FF0 -> 2A9DE0: split at '@', 29D940 + 29DA40(255,indefinite) per
    // piece, remaining lines emptied. Pieces beyond the dynamic line count
    // (source assert 3D5) or longer than TextDynamicLineLength are refused.
    bool preflightDynamicText(std::uint16_t,const std::string&,std::vector<std::string>&,
        OriginalUiUnlockWidgetDiagnostic50PD&) const noexcept;
    bool setDynamicText(std::uint16_t,const std::string&,OriginalUiUnlockWidgetDiagnostic50PD&) noexcept;
    bool updateText(std::uint16_t,bool,OriginalUiUnlockWidgetDiagnostic50PD&) noexcept;
    bool projectText(std::uint16_t,std::size_t,float,std::int32_t,std::int32_t,bool,
        OriginalUiUnlockTextProjection50PD&,OriginalUiUnlockWidgetDiagnostic50PD&) noexcept;
    bool prepareIcon(std::uint16_t,std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,std::uint32_t,
        std::int32_t,OriginalUiUnlockWidgetDiagnostic50PD&) noexcept;
    bool projectIcon(std::uint16_t,bool,OriginalUiUnlockIconProjection50PD&,OriginalUiUnlockWidgetDiagnostic50PD&) const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
