#pragma once
#include "original_ui_archive50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rr::platform {class OriginalUiGarageSceneConfig50KG;}
namespace rr::assets {
// Native admission bounds, not constants claimed to exist in the original.
constexpr std::size_t originalUiDatByteLimit50KG = 256 * 1024;
constexpr std::size_t originalUiControlLimit50KG = 256;
constexpr std::size_t originalUiGroupLimit50KG = 64;
constexpr std::size_t originalUiWidgetLimit50KG = 1024;
constexpr std::size_t originalUiMembershipLimit50KG = 4096;
struct OriginalUiDatValue50KG {
    std::string key, text;
    std::size_t keyOffset = 0, valueOffset = 0;
    bool sourceValuePresent = true; // Foo= is absent; Foo="" is present empty.
};
struct OriginalUiDatSection50KG {
    std::string name;
    std::size_t sourceOffset = 0;
    std::vector<OriginalUiDatValue50KG> values;
    const OriginalUiDatValue50KG* find(const std::string& key) const noexcept;
};
enum class OriginalUiPageError50KG : std::uint8_t {
    none, missingArchive, missingMember, unsupportedEncoding, capacity,
    textSyntax, duplicateSection, duplicateKey, missingSection, missingKey,
    integerSyntax, numberSyntax, reference, unsupportedValue, allocation,
    booleanSyntax
};
struct OriginalUiPageDiagnostic50KG {
    OriginalUiPageError50KG error = OriginalUiPageError50KG::none;
    std::size_t byteOffset = 0;
    std::uint32_t sourcePC = 0;
};
enum class OriginalUiRepeatedKeyPolicy50KG : std::uint8_t { reject,preserveInOrder };
class OriginalUiDat50KG {
public:
    const OriginalUiDatSection50KG* section(const std::string& name) const noexcept;
    const OriginalUiDatValue50KG* find(const std::string& section,
        const std::string& key) const noexcept;
    const OriginalUiMemberIdentity50KG* identity() const noexcept { return identity_; }
    const std::vector<OriginalUiDatSection50KG>& sections() const noexcept { return sections_; }
private:
    friend class rr::platform::OriginalUiGarageSceneConfig50KG;
    // Private authenticated read buffer entry, used by the retained native
    // Scene Config file owner. Never a public raw-byte parser admission.
    static bool parseRetainedMember(const OriginalUiMemberView50KG&,OriginalUiDat50KG&,
        OriginalUiPageDiagnostic50KG&,OriginalUiRepeatedKeyPolicy50KG) noexcept;
    const OriginalUiMemberIdentity50KG* identity_ = nullptr;
    std::vector<OriginalUiDatSection50KG> sections_;
    friend bool readOriginalUiDat50KG(const OriginalUiArchive50KG&, const std::string&,
        OriginalUiDat50KG&, OriginalUiPageDiagnostic50KG&,OriginalUiRepeatedKeyPolicy50KG) noexcept;
};
// Only authenticated original archive members enter this reader. Source
// 0x159440 removes whitespace outside quotes and brackets, without requiring
// a closing quote. 0x158824/0x158858 remove leading/trailing quotes independently.
// CRLF is normalized before value extraction, including opening-only quotes.
// Semicolons begin comments at cleaned line start or after a section's closing
// bracket; value semicolons remain value bytes.
// Unknown fields and repeated values (when explicitly requested) retain order.
// find() returns the first matching present value, as source 0x158648 does.
// ASCII LF/CRLF with optional final NUL; malformed brackets, duplicate
// sections and binary BPB fail closed. Atomic cold operations, no IO/imports.
bool readOriginalUiDat50KG(const OriginalUiArchive50KG& archive,
    const std::string& originalMemberName, OriginalUiDat50KG& output,
    OriginalUiPageDiagnostic50KG& diagnostic,
    OriginalUiRepeatedKeyPolicy50KG repeatedKeys=OriginalUiRepeatedKeyPolicy50KG::reject) noexcept;

struct OriginalUiControl50KG {
    bool present = false; // Original controls may have no matching section.
    std::vector<std::uint16_t> widgets; // Original WidgetID: one-based.
};
struct OriginalUiGroup50KG {
    std::vector<std::int16_t> controls; // Original ControlID: zero-based; -1 = hole.
    std::vector<std::uint16_t> associations; // ControlIDs, NOT group IDs.
    bool keepEnabled = true;
};
struct OriginalUiPage50KG {
    const OriginalUiMemberIdentity50KG* identity = nullptr;
    std::uint16_t widgetCount = 0;
    std::int32_t pageNo = 0;
    // Source 1D6DEC / 2A5FE0 stores this metadata at page +0x13C.
    // The inspected page/manager leaves never use it to create a membership
    // list. It must not select leading widget IDs or imply Control 0.
    std::int32_t staticWidgetCount = 0;
    std::string background = "*INVALID*";
    bool useVideoAsTexture = false;
    // These vectors include entry zero: the source loops 0..count inclusive.
    std::vector<OriginalUiControl50KG> controls;
    std::vector<OriginalUiGroup50KG> groups;
    // PodUIPageController::Create 0x1D63B0; not generic host-layout defaults.
    std::int32_t openSize = 224;
    float openTime = 1.0f;
    std::string start = "Closed";
    bool hasPopup = false;
    // Confirm, Format, FormatSuccess, FormatFail, Yes, No, AskFormat,
    // NoMemcard, FullMemcard, in source 0x1D6598..0x1D666C order.
    std::array<std::int32_t, 9> popupWidgets{{-1,-1,-1,-1,-1,-1,-1,-1,-1}};
    std::array<float, 3> backgroundColor{{0.0f,255.0f,0.0f}};
};
// Membership schema: 0x1D6DEC..0x1D7138 / 0x1D7180 and generic 0x2A5EA0.
// Widget bounds come from the actual companion Widgets.Dat General section.
// Absent group sections retain zero memberships and KeepEnabled=true.
// Invalid references and unsafe transition divisors reject.
bool readOriginalUiPage50KG(const OriginalUiDat50KG& page,
    const OriginalUiDat50KG& widgets, OriginalUiPage50KG& output,
    OriginalUiPageDiagnostic50KG& diagnostic) noexcept;
struct OriginalUiPageIndex50KG {
    const OriginalUiMemberIdentity50KG* identity = nullptr;
    std::vector<std::string> pageFiles; // Original Page 1..TotalPages order.
};
bool readOriginalUiPageIndex50KG(const OriginalUiDat50KG& ui,
    OriginalUiPageIndex50KG& output, OriginalUiPageDiagnostic50KG& diagnostic) noexcept;

// Shared bounded value readers. Missing key writes the supplied original
// default. Invalid authored values preserve output and return a source offset.
// Integer values follow source base-0 conversion within signed32, allowing
// surrounding whitespace and a trailing semicolon annotation. Other suffixes
// reject, and the stored DAT value text is never altered by a typed reader.
bool originalUiInteger50KG(const OriginalUiDatSection50KG&, const std::string& key,
    std::int32_t originalDefault, std::int32_t& output,
    OriginalUiPageDiagnostic50KG&, std::uint32_t sourcePC) noexcept;
// Source 0x158B30 -> 0x158920: case-insensitive T/F and Y/N first characters,
// exact ON/OFF, then the same bounded base-0 integer conversion above. Missing
// values use the supplied default. Only boolean fields use this reader; the
// generic integer reader does not accept the source's alphabetic bool tokens.
bool originalUiBoolean50KG(const OriginalUiDatSection50KG&, const std::string& key,
    bool originalDefault, bool& output,
    OriginalUiPageDiagnostic50KG&, std::uint32_t sourcePC) noexcept;
bool originalUiNumber50KG(const OriginalUiDatSection50KG&, const std::string& key,
    float originalDefault, float& output, OriginalUiPageDiagnostic50KG&,
    std::uint32_t sourcePC) noexcept;
bool originalUiTriple50KG(const OriginalUiDatSection50KG&,const std::string& key,
    const std::array<float,3>& originalDefault,std::array<float,3>& output,
    OriginalUiPageDiagnostic50KG&,std::uint32_t sourcePC) noexcept;
}
