#pragma once
#include "import_decode48c.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rr::assets {
// These IDs identify exact retained US-disc originals, not arbitrary files.
enum class OriginalUiArchiveId50KG : std::uint8_t {
    appStart, localization, overlays, fonts, mainMenu, backgrounds, controllerPopup
};
struct OriginalUiArchiveIdentity50KG {
    OriginalUiArchiveId50KG id;
    const char* discPath;
    std::size_t bytes;
    const char* sha256;
    std::size_t firstMember, memberCount;
};
struct OriginalUiMemberIdentity50KG {
    const char* name;
    std::uint32_t logicalOffset, bytes;
    const char* sha256;
};
const OriginalUiArchiveIdentity50KG* originalUiArchiveIdentity50KG(
    OriginalUiArchiveId50KG) noexcept;
// Ordinal is local to this archive, retaining its original archive order.
const OriginalUiMemberIdentity50KG* originalUiMemberIdentity50KG(
    OriginalUiArchiveId50KG, std::size_t ordinal) noexcept;

struct OriginalUiMemberView50KG {
    const OriginalUiMemberIdentity50KG* identity = nullptr;
    const std::uint8_t* data = nullptr;
    std::size_t size = 0;
};
enum class OriginalUiArchiveStatus50KG : std::uint8_t {
    none, alreadyLoaded, unknownArchive, archiveIdentity, resource,
    memberTable, memberIdentity, cancelled, allocation, unexpectedFailure
};
struct OriginalUiArchiveDiagnostic50KG {
    OriginalUiArchiveStatus50KG status = OriginalUiArchiveStatus50KG::none;
    std::size_t member = static_cast<std::size_t>(-1);
    DecodeError48C resource;
};

// Cold load-once, immutable owner of authenticated original logical bytes.
// No file I/O, cache writing, format conversion, substitute art or GL calls.
// No archive member name is ever used as an output filesystem path.
// Borrowed views remain valid until this owner is destroyed. Failed loads
// leave an empty owner retryable; reload does not replace a published owner.
class OriginalUiArchive50KG final {
public:
    OriginalUiArchive50KG() noexcept;
    ~OriginalUiArchive50KG();
    OriginalUiArchive50KG(const OriginalUiArchive50KG&) = delete;
    OriginalUiArchive50KG& operator=(const OriginalUiArchive50KG&) = delete;
    OriginalUiArchive50KG(OriginalUiArchive50KG&&) = delete;
    OriginalUiArchive50KG& operator=(OriginalUiArchive50KG&&) = delete;

    // Serial cold call. Input, diagnostic and hook storage must be disjoint.
    // Hooks obey DecodeHooks48C: no throw, mutation or reentry. Authentication
    // is completed before publication; resource callbacks can report progress
    // before authentication is complete and therefore never mean success.
    bool load(OriginalUiArchiveId50KG, const std::vector<std::uint8_t>&,
        OriginalUiArchiveDiagnostic50KG&, const DecodeHooks48C* = nullptr) noexcept;
    bool loaded() const noexcept;
    const OriginalUiArchiveIdentity50KG* identity() const noexcept;
    std::size_t memberCount() const noexcept;
    OriginalUiMemberView50KG member(std::size_t ordinal) const noexcept;
    // ASCII case-insensitive; slash and backslash are equivalent. No basename
    // fallback, prefix matching, cross-archive lookup or temporary allocation.
    OriginalUiMemberView50KG find(const std::string& originalName) const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
