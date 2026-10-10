#pragma once
#include "original_ui_archive50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace rr::assets {
constexpr std::size_t originalUiLocalizedStringCount50KG=467;
struct OriginalUiLocalizedEntry50KG {
    std::string name,text; // Original bytes, not UTF-8 conversion.
    std::int16_t sourceMaximumLength=0;
    std::size_t nameOffset=0,textOffset=0;
};
enum class OriginalUiLocalizationError50KG : std::uint8_t {
    none,missingArchive,missingMember,header,count,bounds,unterminated,
    duplicateName,capacity,allocation
};
struct OriginalUiLocalizationDiagnostic50KG {
    OriginalUiLocalizationError50KG error=OriginalUiLocalizationError50KG::none;
    std::size_t byteOffset=0,entry=0;
    std::uint32_t sourcePC=0;
};
class OriginalUiEnglish50KG {
public:
    bool loaded() const noexcept { return indexIdentity_ && textIdentity_; }
    const OriginalUiLocalizedEntry50KG* find(const std::string& name) const noexcept;
    const OriginalUiLocalizedEntry50KG* at(std::size_t index) const noexcept;
    const OriginalUiMemberIdentity50KG* indexIdentity() const noexcept { return indexIdentity_; }
    const OriginalUiMemberIdentity50KG* textIdentity() const noexcept { return textIdentity_; }
private:
    const OriginalUiMemberIdentity50KG *indexIdentity_=nullptr,*textIdentity_=nullptr;
    std::array<OriginalUiLocalizedEntry50KG,originalUiLocalizedStringCount50KG> entries_{};
    friend bool readOriginalUiEnglish50KG(const OriginalUiArchive50KG&,
        OriginalUiEnglish50KG&,OriginalUiLocalizationDiagnostic50KG&) noexcept;
};
// Source GameText 0x14BC30 / 0x14C5A0. UI.loc has count i16@26,
// names offset u32@4 and 8-byte metadata-table offset u32@8. Names are
// sequential NUL strings; metadata +4 is an i16 maximum length. Language
// BIN is 467 sequential NUL strings starting at byte zero, without a header.
// TextID comparison is case-sensitive strcmp (0x2D3AB0 -> 0x3090E8).
// Authenticated loc\english\UI.loc and loc\english\L_UI_ENG.bin only.
// Atomic cold parsing; no resource imports, guest execution or guessed labels.
bool readOriginalUiEnglish50KG(const OriginalUiArchive50KG&,
    OriginalUiEnglish50KG&,OriginalUiLocalizationDiagnostic50KG&) noexcept;
}
