#pragma once
#include "../assets/original_audio_directory50kx.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>

namespace rr::game {
inline constexpr std::size_t coldAudioGroupSlots50KY = 25;
inline constexpr std::size_t coldAudioFirstDynamicGroup50KY = 2;
inline constexpr std::size_t coldAudioInitialSoundCapacity50KY = 8;
inline constexpr std::size_t coldAudioSoundGrowth50KY = 16;

// Source ordinals and dynamic group IDs are different identity domains. Neither
// is a provider sample, mixer voice, event group binding or guest address.
struct SourceAudioResourceIndex50KY { std::uint32_t value = UINT32_MAX; };
struct ColdAudioGroupId50KY { std::uint8_t value = UINT8_MAX; };
struct ColdAudioKnownBits8_50KY { std::uint8_t mask = 0, value = 0; };

enum class ColdAudioStatus50KY : std::uint8_t {
    none, alreadyInitialized, notInitialized, domainClosed,
    directoryNotPrepared, directoryShape, bindingsNotPrepared,
    unsupportedMaterialKind, invalidMaterialKey, missingResource,
    invalidGroupHandle, staleGroupHandle, wrongDomain, invalidSoundHandle,
    groupIdsExhausted, identityExhausted, registrationCountOverflow,
    storageCapacityOverflow, allocation, pendingRepresentativeBorrow,
    sampleOwnerUnavailable, unexpectedFailure
};
struct ColdAudioDiagnostic50KY {
    ColdAudioStatus50KY status = ColdAudioStatus50KY::none;
    SourceAudioResourceIndex50KY resource{};
    ColdAudioGroupId50KY group{};
};
enum class ColdAudioDomainState50KY : std::uint8_t { empty, initialized, closed };
enum class GcMaterialSoundKind50KY : std::uint8_t { collision, drive };

struct ColdAudioDomainIdentity50KY; // Private lifetime token, never caller-made.
class ColdLogicalAudioDomain50KY;
class ColdLogicalGroup50KY;
class ColdLogicalSound50KY;

// Value handles do not retain groups or sounds. Private construction, a weak
// lifetime token and a nonwrapping slot generation reject foreign/stale owners,
// including a different domain later constructed at the same memory address.
class ColdAudioGroupHandle50KY final {
public:
    ColdAudioGroupHandle50KY() noexcept = default;
private:
    std::weak_ptr<const ColdAudioDomainIdentity50KY> domain_;
    std::uint64_t generation_ = 0;
    std::uint8_t slot_ = UINT8_MAX;
    friend class ColdLogicalAudioDomain50KY;
};
class ColdAudioSoundHandle50KY final {
public:
    ColdAudioSoundHandle50KY() noexcept = default;
private:
    ColdAudioGroupHandle50KY group_;
    std::size_t ordinal_ = static_cast<std::size_t>(-1);
    friend class ColdLogicalAudioDomain50KY;
};

// Only proven cold values. This is not a complete GameObject or guest layout.
// Null-only link projections cannot be used to attach objects to a live tree.
struct ColdDetachedGroupFields50KY {
    std::uint64_t referenceCount0 = 1;
    std::nullptr_t previous10 = nullptr, next14 = nullptr;
    std::nullptr_t firstChild18 = nullptr, parent1C = nullptr;
    std::uint32_t mask20 = 0, mask24 = 0;
    std::uint8_t byte28 = 0xff;
    ColdAudioKnownBits8_50KY flags29{0x0f, 0x03};
    ColdAudioKnownBits8_50KY flags30{0x01, 0x01};
    std::int64_t volume38 = 0; // Construction0; admitted claimed group100.
    std::uint32_t state44 = 0;
};

// Exact proven constructor/initializer field projection, not a 0x1C0 image.
// The unlisted words are unknown. Pitches are cold constructor values, never
// source-positive sample metadata. The -1 sentinels cannot become sample IDs.
struct ColdLogicalSoundFields50KY {
    std::uint64_t referenceCount0 = 1;
    std::uint32_t flags10 = 0, word14 = 0, word18 = 0;
    std::uint32_t durationBits20 = 0, elapsedBits24 = 0;
    std::int32_t sample28 = -1, channel2C = -1;
    std::uint32_t volume38 = 100, pan3C = 0, pitch40 = 4096;
    std::uint32_t effective44 = 100, effective48 = 0, effectivePitch4C = 4096;
    std::uint32_t basePitch50 = 4096;
    std::uint32_t multiplierBits54 = 0x3f800000, multiplierBits58 = 0x3f800000;
    std::uint32_t multiplierBits5C = 0x3e800000;
    std::array<std::uint32_t, 4> positionBits60{{0, 0, 0, 0x3f800000}};
    std::uint32_t word70 = 0, word74 = 0, word78 = 0, word7C = 0, word80 = 0;
    std::array<std::uint32_t, 4> vectorBits90{{0, 0, 0, 0x3f800000}};
    std::int32_t resourceIndexA0 = -1;
    std::uint32_t stream1A8 = 0, stereo1AC = 0, loop1B0 = 0, word1B4 = 0;
    std::int32_t streamIndex1B8 = -1;
};

class ColdLogicalSound50KY final {
public:
    ~ColdLogicalSound50KY();
    ColdLogicalSound50KY(const ColdLogicalSound50KY&) = delete;
    ColdLogicalSound50KY& operator=(const ColdLogicalSound50KY&) = delete;
    ColdLogicalSound50KY(ColdLogicalSound50KY&&) = delete;
    ColdLogicalSound50KY& operator=(ColdLogicalSound50KY&&) = delete;
    // All object/field/group views expire with this sound's owning group or
    // domain. Binding/string views require their immutable binding owner to
    // remain alive (guaranteed here for at least this sound's lifetime).
    const ColdLogicalSoundFields50KY& fields() const noexcept { return fields_; }
    const ColdLogicalGroup50KY& group() const noexcept { return *group_; }
    SourceAudioResourceIndex50KY resource() const noexcept;
    GcMaterialSoundKind50KY materialKind() const noexcept { return kind_; }
    // All three names are distinct authenticated fields. No full source name
    // buffer initialization or actual interface+A0 binding is asserted.
    const assets::GcMaterialResourceBinding50KX& binding() const noexcept { return *binding_; }
    std::string_view factoryFilename() const noexcept { return binding_->sourceFactoryFilename; }
private:
    ColdLogicalSound50KY() noexcept = default;
    ColdLogicalSoundFields50KY fields_{};
    ColdLogicalGroup50KY* group_ = nullptr;
    std::shared_ptr<const assets::GcMaterialResourceBindings50KX> bindings_;
    const assets::GcMaterialResourceBinding50KX* binding_ = nullptr;
    GcMaterialSoundKind50KY kind_ = GcMaterialSoundKind50KY::collision;
    friend class ColdLogicalAudioDomain50KY;
};

class ColdLogicalGroup50KY final {
public:
    ~ColdLogicalGroup50KY();
    ColdLogicalGroup50KY(const ColdLogicalGroup50KY&) = delete;
    ColdLogicalGroup50KY& operator=(const ColdLogicalGroup50KY&) = delete;
    ColdLogicalGroup50KY(ColdLogicalGroup50KY&&) = delete;
    ColdLogicalGroup50KY& operator=(ColdLogicalGroup50KY&&) = delete;
    // These fields, soundAt views and all raw sound borrows expire at this
    // group's destruction or domain close. Use checked handles across calls.
    const ColdDetachedGroupFields50KY& fields() const noexcept { return fields_; }
    ColdAudioGroupId50KY id() const noexcept { return id_; }
    std::size_t soundCount() const noexcept { return count_; }
    std::size_t soundCapacity() const noexcept { return capacity_; }
    const ColdLogicalSound50KY* soundAt(std::size_t ordinal) const noexcept;
private:
    ColdLogicalGroup50KY() noexcept = default;
    ColdDetachedGroupFields50KY fields_{};
    ColdAudioGroupId50KY id_{}; // Unclaimed until real storage is ready.
    std::unique_ptr<std::unique_ptr<ColdLogicalSound50KY>[]> sounds_;
    std::size_t count_ = 0, capacity_ = 0;
    friend class ColdLogicalAudioDomain50KY;
};

// No Resolved alternative exists in this cold component. Adding one requires
// an actual sample owner and provenance-bearing result/metadata types.
enum class ColdAudioRowTag50KY : std::uint8_t { unreferenced, pendingRegistration, cleared };
struct ColdAudioResourceRow50KY {
    SourceAudioResourceIndex50KY resource{};
    const assets::OriginalAudioDirectoryRow50KX* directory = nullptr;
    ColdAudioRowTag50KY tag = ColdAudioRowTag50KY::unreferenced;
    const ColdLogicalSound50KY* representative = nullptr;
    // Historical registrations since the last clear; not an object refcount.
    std::uint32_t registrationCount = 0;
};
struct ColdAudioOwnedCounts50KY {
    std::size_t groups = 0, sounds = 0;
    std::size_t gameObjectContribution = 0; // These groups only; not global423F60.
};

// Single-threaded, non-reentrant, detached, never-played ownership domain.
// Retained immutable owners must have side-effect-free destruction; custom
// deleters must not call back into the domain. One instance is
// one prospective shared application-wide group namespace, never one per D44.
// Does not implement AuralScape, the global GameObject guard, event attachment,
// PCM loading, provider callbacks, sample publication, transport or playback.
class ColdLogicalAudioDomain50KY final {
public:
    ColdLogicalAudioDomain50KY() noexcept;
    ~ColdLogicalAudioDomain50KY();
    ColdLogicalAudioDomain50KY(const ColdLogicalAudioDomain50KY&) = delete;
    ColdLogicalAudioDomain50KY& operator=(const ColdLogicalAudioDomain50KY&) = delete;
    ColdLogicalAudioDomain50KY(ColdLogicalAudioDomain50KY&&) = delete;
    ColdLogicalAudioDomain50KY& operator=(ColdLogicalAudioDomain50KY&&) = delete;

    // Requires actual shared ownership, normally make_shared, not a no-op-
    // deleter lifetime assertion or unrelated aliasing control block. The
    // shared_ptr type cannot runtime-certify that ownership precondition.
    // Failed preparation remains empty/retryable. Initialized/closed owners
    // cannot replace their directory or reset a live namespace by preparing.
    bool prepare(std::shared_ptr<const assets::OriginalAudioDirectory50KX>,
        ColdAudioDiagnostic50KY&) noexcept;
    ColdAudioDomainState50KY state() const noexcept { return state_; }
    bool coldInitialized() const noexcept { return state_ == ColdAudioDomainState50KY::initialized; }
    ColdAudioOwnedCounts50KY ownedCounts() const noexcept;
    // These views expire at close. Row contents change on registration/clear.
    // Copying a row representative does not extend that sound's lifetime or
    // preserve a safe borrow after explicit clear followed by group teardown.
    const assets::OriginalAudioDirectory50KX* directory() const noexcept;
    std::size_t rowCount() const noexcept;
    const ColdAudioResourceRow50KY* row(SourceAudioResourceIndex50KY) const noexcept;

    // On failure, state, registrations and output handles are unchanged.
    bool createDetachedGroup(ColdAudioGroupHandle50KY&, ColdAudioDiagnostic50KY&) noexcept;
    // Selects flags1 or81 from the actual source factory kind; arbitrary flags
    // (especially streaming/stereo) and fabricated binding records are absent.
    // Bindings require the same actual-owning, side-effect-free shared_ptr
    // precondition as prepare; normally supply a make_shared owner.
    bool createGcSound(const ColdAudioGroupHandle50KY&,
        std::shared_ptr<const assets::GcMaterialResourceBindings50KX>,
        GcMaterialSoundKind50KY, std::uint32_t sourceKey,
        ColdAudioSoundHandle50KY&, ColdAudioDiagnostic50KY&) noexcept;
    // Returned borrows expire at their owning group's destruction or close.
    const ColdLogicalGroup50KY* inspectGroup(const ColdAudioGroupHandle50KY&,
        ColdAudioDiagnostic50KY&) const noexcept;
    const ColdLogicalSound50KY* inspectSound(const ColdAudioSoundHandle50KY&,
        ColdAudioDiagnostic50KY&) const noexcept;

    // Always refuses without mutating rows or synthesizing any sample result.
    bool resolvePending(ColdAudioDiagnostic50KY&) const noexcept;
    // Source26D960 clear-only semantics. Sounds/groups remain unchanged.
    bool clearRegistrations(ColdAudioDiagnostic50KY&) noexcept;
    // Host admission guard: refuses if a pending representative belongs to the
    // target group. Explicit clear is required first. Never retargets a row.
    bool destroyDetachedGroup(const ColdAudioGroupHandle50KY&, ColdAudioDiagnostic50KY&) noexcept;
    // Close admissions, clear borrowed row identities, return IDs, destroy
    // sounds in insertion order, then release directory/storage. Idempotent.
    void close() noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
    ColdAudioDomainState50KY state_ = ColdAudioDomainState50KY::empty;
    bool requireInitialized(ColdAudioDiagnostic50KY&) const noexcept;
    const ColdLogicalGroup50KY* checkedGroup(const ColdAudioGroupHandle50KY&,
        ColdAudioDiagnostic50KY&) const noexcept;
    void clearRows() noexcept;
    void destroyGroupAt(std::size_t) noexcept;
};
}
