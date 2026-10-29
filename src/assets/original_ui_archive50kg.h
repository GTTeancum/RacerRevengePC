#pragma once
#include "import_decode48c.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <utility>

namespace rr::assets {
class OriginalUiMaterialTemplates50KG;
class OriginalStandardArchiveProvider50LA;
class OriginalUiEncodedArchiveView50KG;
struct OriginalUiMaterialTemplateStorage50KG;
// NEW reconstruction: identities transcribed from the retained Full48F manifest.
// These IDs identify exact US-disc originals, not arbitrary files.
// Existing archive ordinals are preserved; authenticated Garage resources append.
enum class OriginalUiArchiveId50KG : std::uint8_t {
    appStart = 0, localization = 1, overlays = 2, fonts = 3, mainMenu = 4,
    backgrounds = 5, controllerPopup = 6, course = 7, rider = 8, options = 9, podTextures = 10, podPhysics = 11, tournament = 12, hallOfFame = 13,
    garage = 14, garagePod19 = 15,
    garagePod01 = 16, garagePod02 = 17, garagePod03 = 18, garagePod04 = 19, garagePod07 = 20, garagePod08 = 21, garagePod09 = 22, garagePod10 = 23, garagePod11 = 24, garagePod12 = 25, garagePod14 = 26, garagePod15 = 27, garagePod16 = 28, garagePod17 = 29, garagePod18 = 30, garagePod20 = 31, garagePod21 = 32, garagePod22 = 33, garagePod23 = 34, garagePod24 = 35, garagePod25 = 36, garagePod26 = 37, loadingIcon = 38, trackRa = 39, sourcePod01 = 40
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
// Exact original selected-career Garage archive; unsupported identities stay absent.
const OriginalUiArchiveIdentity50KG* originalUiGaragePodArchive50KG(std::uint32_t career) noexcept;
int originalUiGaragePodCareer50KG(OriginalUiArchiveId50KG) noexcept;
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
// Its optional immutable APPSTART template backing is published once by the
// serial material loader. It never changes these authenticated bytes.
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
    // Same authentication and single publication as load. On failure input is
    // unchanged. On success its allocation transfers to this immutable owner;
    // callers relinquish mutable aliases. No second encoded allocation is made.
    bool loadOwned(OriginalUiArchiveId50KG, std::vector<std::uint8_t>&&,
        OriginalUiArchiveDiagnostic50KG&, const DecodeHooks48C* = nullptr) noexcept;
    bool loaded() const noexcept;
    const OriginalUiArchiveIdentity50KG* identity() const noexcept;
    std::size_t memberCount() const noexcept;
    OriginalUiMemberView50KG member(std::size_t ordinal) const noexcept;
    // ASCII case-insensitive; slash and backslash are equivalent. No basename
    // fallback, prefix matching, cross-archive lookup or temporary allocation.
    OriginalUiMemberView50KG find(const std::string& originalName) const noexcept;
private:
    friend class OriginalUiMaterialTemplates50KG;
    friend class OriginalUiEncodedArchiveView50KG;
    bool loadImpl(OriginalUiArchiveId50KG, const std::vector<std::uint8_t>&,
        std::vector<std::uint8_t>* transfer,
        OriginalUiArchiveDiagnostic50KG&, const DecodeHooks48C*) noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
    // An application retaining this SAME APPSTART archive retains the SAME
    // DefaultMT/ScreenMT objects across page, frontend and race lifetimes.
    // Template handles also pin this backing; no reference back to the archive
    // is stored in it, so these two ownership directions do not form a cycle.
    mutable std::shared_ptr<const OriginalUiMaterialTemplateStorage50KG> materialTemplates_;
};
// Immutable original-file input, not decoded member data or manager readiness.
// Normally minted by a real mounted UI archive in the existing provider.
// The private actual Application17CAB8 issuer alone may mint a pre-mount view
// from its authenticated APPSTART owner and exact fresh provider.
// It pins that SAME archive, not a duplicate encoded buffer. providerIdentity is
// comparison-only provenance and must never be dereferenced through this view.
class OriginalUiEncodedArchiveView50KG final {
    friend class OriginalStandardArchiveProvider50LA;
    // Only the fresh actual Application issuer may read authenticated original
    // encoded input before its first native mount, for source17CAB8 registration.
    friend class OriginalStandardTextureResolver50LA;
    const OriginalStandardArchiveProvider50LA* provider_=nullptr;
    std::shared_ptr<const OriginalUiArchive50KG> archive_;
    OriginalUiEncodedArchiveView50KG(const OriginalStandardArchiveProvider50LA* provider,
        std::shared_ptr<const OriginalUiArchive50KG> archive) noexcept
        :provider_(provider),archive_(std::move(archive)){}
public:
    OriginalUiEncodedArchiveView50KG() noexcept=default;
    const OriginalStandardArchiveProvider50LA* providerIdentity() const noexcept {return provider_;}
    const OriginalUiArchiveIdentity50KG* identity() const noexcept;
    const std::uint8_t* data() const noexcept;
    std::size_t size() const noexcept;
    std::size_t blockCount() const noexcept;
    bool block(std::size_t, ResourceBlock48C&) const noexcept;
};
}

