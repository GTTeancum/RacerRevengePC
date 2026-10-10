#pragma once
#include "original_ui_options_menu_controller50kg.h"

namespace rr::game {
// Actual screen12/page4/options.dat. This is an edit buffer, not persistence.
struct OriginalUiGameSettings50KG {
    bool autoSave=false,vibration=false,verticalSplit=false;
    std::uint32_t soundMode=0;
    std::array<std::uint32_t,4> volume{}; // SFX, music, voice, announcer
};
struct OriginalUiGameSettingsProfile50KG {
    bool known=false;
    std::uint32_t savedGroup6E8=0,soundMode1101C=0;
    std::array<std::uint32_t,4> volume11020{};
    std::uint8_t flags11050=0;
};
// Original reset fields1865B4..18666C, preserving unrelated flag bits. This
// does not set known or claim 188740 audio application/save completion.
void resetOriginalUiGameSettingsProfileFields50KG(OriginalUiGameSettingsProfile50KG&) noexcept;
bool validOriginalUiGameSettingsProfile50KG(const OriginalUiGameSettingsProfile50KG&) noexcept;
struct OriginalUiGameOptionsState50KG {
    OriginalUiNavigation50KG navigation{};
    OriginalUiGameSettings50KG local{};
    bool loaded=false;
};
enum class OriginalUiGameOptionsIntent50KG : std::uint8_t {none,accept,cancel};
struct OriginalUiGameOptionsPlan50KG {
    OriginalUiGameOptionsState50KG state{};
    std::array<OriginalUiOptionsMenuCallback50KG,256> callbacks{};
    std::size_t callbackCount=0;
    bool writesPageInputFlags42C=false;
    std::uint32_t pageInputFlags42C=0;
    bool pageAccepted=false,soundRequested=false,flushRequested=false;
    std::uint8_t soundId=0;
    std::uint32_t soundPC=0,flushPC=0;
    // Intent remains uncommitted. Both paths remember the actual attached group
    // then request page1/lifecycle7; Accept first needs the genuine 188740 sound
    // mode consumer and shared profile writes. No readiness flag can bypass it.
    OriginalUiGameOptionsIntent50KG intent=OriginalUiGameOptionsIntent50KG::none;
    std::uint16_t rememberedGroup6E8=0;
    std::uint32_t rememberedGroupPC=0,requestPC=0;
};
class OriginalUiGameOptionsController50KG final {
public:
    OriginalUiGameOptionsController50KG() noexcept;
    ~OriginalUiGameOptionsController50KG();
    OriginalUiGameOptionsController50KG(const OriginalUiGameOptionsController50KG&)=delete;
    OriginalUiGameOptionsController50KG& operator=(const OriginalUiGameOptionsController50KG&)=delete;
    OriginalUiOptionsMenuStatus50KG bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,assets::OriginalUiPageDiagnostic50KG&) noexcept;
    const assets::OriginalUiPage50KG* page() const noexcept;
    const assets::OriginalUiDat50KG* widgetsDat() const noexcept;
    bool initialState(OriginalUiGameOptionsState50KG&) const noexcept;
    OriginalUiOptionsMenuStatus50KG phase(const OriginalUiGameOptionsState50KG&,
        OriginalUiOptionsMenuPhase50KG,const OriginalUiGameSettingsProfile50KG&,OriginalUiGameOptionsPlan50KG&) const noexcept;
    OriginalUiOptionsMenuStatus50KG input(const OriginalUiGameOptionsState50KG&,
        std::uint8_t controller,assets::OriginalUiMovementInput50KG,
        bool containerReached,bool pageInputEnabled268,OriginalUiGameOptionsPlan50KG&) const noexcept;
    // Value widget actually used by Render1F5F40 for selection-frame placement.
    std::uint16_t selectedValueWidget(const OriginalUiGameOptionsState50KG&) const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
