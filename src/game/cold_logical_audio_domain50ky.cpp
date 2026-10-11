#include "cold_logical_audio_domain50ky.h"
#include <limits>
#include <new>
#include <utility>

namespace rr::game {
struct ColdAudioDomainIdentity50KY final {};
namespace {
using Status = ColdAudioStatus50KY;
bool fail(ColdAudioDiagnostic50KY& diagnostic, Status status,
    SourceAudioResourceIndex50KY resource = {}, ColdAudioGroupId50KY group = {}) noexcept {
    diagnostic = {status, resource, group};
    return false;
}
}

struct ColdLogicalAudioDomain50KY::Storage {
    std::shared_ptr<const assets::OriginalAudioDirectory50KX> directory;
    std::shared_ptr<const ColdAudioDomainIdentity50KY> identity;
    std::array<ColdAudioResourceRow50KY, assets::originalAudioDirectoryRows50KX> rows{};
    std::array<std::unique_ptr<ColdLogicalGroup50KY>, coldAudioGroupSlots50KY> groups{};
    std::array<std::uint64_t, coldAudioGroupSlots50KY> generations{};
    std::array<bool, coldAudioGroupSlots50KY> occupied{};
    std::size_t groupCount = 0, soundCount = 0;
};

ColdLogicalSound50KY::~ColdLogicalSound50KY() = default;
SourceAudioResourceIndex50KY ColdLogicalSound50KY::resource() const noexcept {
    return {binding_->sourceResourceIndex};
}
ColdLogicalGroup50KY::~ColdLogicalGroup50KY() = default;
const ColdLogicalSound50KY* ColdLogicalGroup50KY::soundAt(std::size_t ordinal) const noexcept {
    return ordinal < count_ ? sounds_[ordinal].get() : nullptr;
}
ColdLogicalAudioDomain50KY::ColdLogicalAudioDomain50KY() noexcept = default;
ColdLogicalAudioDomain50KY::~ColdLogicalAudioDomain50KY() { close(); }

bool ColdLogicalAudioDomain50KY::prepare(
    std::shared_ptr<const assets::OriginalAudioDirectory50KX> directory,
    ColdAudioDiagnostic50KY& diagnostic) noexcept {
    diagnostic = {};
    if (state_ == ColdAudioDomainState50KY::closed) return fail(diagnostic, Status::domainClosed);
    if (state_ == ColdAudioDomainState50KY::initialized) return fail(diagnostic, Status::alreadyInitialized);
    if (!directory || !directory->prepared()) return fail(diagnostic, Status::directoryNotPrepared);
    if (directory->size() != assets::originalAudioDirectoryRows50KX)
        return fail(diagnostic, Status::directoryShape);
    // Validate the real immutable owner, never trust a supplied span, digest,
    // occupancy snapshot or count as a substitute for directory preparation.
    for (std::size_t i = 0; i < directory->size(); ++i) {
        const auto* row = directory->row(i);
        if (!row || row->sourceIndex != i || directory->find(row->sourceName) != row)
            return fail(diagnostic, Status::directoryShape, {static_cast<std::uint32_t>(i)});
    }
    try {
        auto next = std::make_unique<Storage>();
        next->identity = std::make_shared<const ColdAudioDomainIdentity50KY>();
        next->directory = std::move(directory);
        next->occupied[0] = next->occupied[1] = true; //232AA0 reserved masters.
        for (std::size_t i = 0; i < next->rows.size(); ++i) {
            next->rows[i].resource = {static_cast<std::uint32_t>(i)};
            next->rows[i].directory = next->directory->row(i);
        }
        storage_ = std::move(next);
        state_ = ColdAudioDomainState50KY::initialized;
        return true;
    } catch (const std::bad_alloc&) { return fail(diagnostic, Status::allocation); }
    catch (...) { return fail(diagnostic, Status::unexpectedFailure); }
}

bool ColdLogicalAudioDomain50KY::requireInitialized(ColdAudioDiagnostic50KY& diagnostic) const noexcept {
    if (state_ == ColdAudioDomainState50KY::closed) return fail(diagnostic, Status::domainClosed);
    if (state_ != ColdAudioDomainState50KY::initialized) return fail(diagnostic, Status::notInitialized);
    return true;
}
ColdAudioOwnedCounts50KY ColdLogicalAudioDomain50KY::ownedCounts() const noexcept {
    return storage_ ? ColdAudioOwnedCounts50KY{storage_->groupCount,
        storage_->soundCount, storage_->groupCount} : ColdAudioOwnedCounts50KY{};
}
const assets::OriginalAudioDirectory50KX* ColdLogicalAudioDomain50KY::directory() const noexcept {
    return storage_ ? storage_->directory.get() : nullptr;
}
std::size_t ColdLogicalAudioDomain50KY::rowCount() const noexcept {
    return storage_ ? storage_->rows.size() : 0;
}
const ColdAudioResourceRow50KY* ColdLogicalAudioDomain50KY::row(SourceAudioResourceIndex50KY index) const noexcept {
    return storage_ && index.value < storage_->rows.size() ? &storage_->rows[index.value] : nullptr;
}

bool ColdLogicalAudioDomain50KY::createDetachedGroup(ColdAudioGroupHandle50KY& output,
    ColdAudioDiagnostic50KY& diagnostic) noexcept {
    diagnostic = {};
    if (!requireInitialized(diagnostic)) return false;
    std::size_t id = coldAudioFirstDynamicGroup50KY;
    for (; id < storage_->occupied.size() && storage_->occupied[id]; ++id) {}
    if (id == storage_->occupied.size()) return fail(diagnostic, Status::groupIdsExhausted);
    const ColdAudioGroupId50KY groupId{static_cast<std::uint8_t>(id)};
    if (storage_->generations[id] == std::numeric_limits<std::uint64_t>::max())
        return fail(diagnostic, Status::identityExhausted, {}, groupId);
    try {
        auto group = std::unique_ptr<ColdLogicalGroup50KY>(new ColdLogicalGroup50KY());
        group->sounds_ = std::make_unique<std::unique_ptr<ColdLogicalSound50KY>[]>(
            coldAudioInitialSoundCapacity50KY);
        group->capacity_ = coldAudioInitialSoundCapacity50KY;
        // All allocations precede ID claim/publication. An unclaimed failed
        // construction never indexes occupancy using source constructor2/-1.
        group->id_ = groupId;
        group->fields_.volume38 = 100;
        ColdAudioGroupHandle50KY handle;
        handle.domain_ = storage_->identity;
        handle.slot_ = groupId.value;
        handle.generation_ = storage_->generations[id] + 1u;
        storage_->groups[id] = std::move(group);
        storage_->occupied[id] = true;
        storage_->generations[id] = handle.generation_;
        ++storage_->groupCount;
        output = std::move(handle);
        return true;
    } catch (const std::bad_alloc&) { return fail(diagnostic, Status::allocation, {}, groupId); }
    catch (...) { return fail(diagnostic, Status::unexpectedFailure, {}, groupId); }
}

const ColdLogicalGroup50KY* ColdLogicalAudioDomain50KY::checkedGroup(
    const ColdAudioGroupHandle50KY& handle, ColdAudioDiagnostic50KY& diagnostic) const noexcept {
    if (!requireInitialized(diagnostic)) return nullptr;
    if (handle.slot_ < coldAudioFirstDynamicGroup50KY || handle.slot_ >= coldAudioGroupSlots50KY ||
        !handle.generation_) {
        fail(diagnostic, Status::invalidGroupHandle);
        return nullptr;
    }
    const ColdAudioGroupId50KY id{handle.slot_};
    const auto identity = handle.domain_.lock();
    if (!identity) {
        fail(diagnostic, Status::staleGroupHandle, {}, id);
        return nullptr;
    }
    if (identity != storage_->identity) {
        fail(diagnostic, Status::wrongDomain, {}, id);
        return nullptr;
    }
    if (!storage_->occupied[handle.slot_] || !storage_->groups[handle.slot_] ||
        storage_->generations[handle.slot_] != handle.generation_) {
        fail(diagnostic, Status::staleGroupHandle, {}, id);
        return nullptr;
    }
    return storage_->groups[handle.slot_].get();
}
const ColdLogicalGroup50KY* ColdLogicalAudioDomain50KY::inspectGroup(
    const ColdAudioGroupHandle50KY& handle, ColdAudioDiagnostic50KY& diagnostic) const noexcept {
    diagnostic = {};
    return checkedGroup(handle, diagnostic);
}
const ColdLogicalSound50KY* ColdLogicalAudioDomain50KY::inspectSound(
    const ColdAudioSoundHandle50KY& handle, ColdAudioDiagnostic50KY& diagnostic) const noexcept {
    diagnostic = {};
    const auto* group = checkedGroup(handle.group_, diagnostic);
    if (!group) return nullptr;
    const auto* sound = group->soundAt(handle.ordinal_);
    if (!sound) fail(diagnostic, Status::invalidSoundHandle, {}, group->id());
    return sound;
}

bool ColdLogicalAudioDomain50KY::createGcSound(const ColdAudioGroupHandle50KY& handle,
    std::shared_ptr<const assets::GcMaterialResourceBindings50KX> bindings,
    GcMaterialSoundKind50KY kind, std::uint32_t sourceKey,
    ColdAudioSoundHandle50KY& output, ColdAudioDiagnostic50KY& diagnostic) noexcept {
    diagnostic = {};
    if (!checkedGroup(handle, diagnostic)) return false;
    auto& group = *storage_->groups[handle.slot_];
    if (!bindings || !bindings->prepared())
        return fail(diagnostic, Status::bindingsNotPrepared, {}, group.id());
    const assets::GcMaterialResourceBinding50KX* binding = nullptr;
    switch (kind) {
    case GcMaterialSoundKind50KY::collision: binding = bindings->collision(sourceKey); break;
    case GcMaterialSoundKind50KY::drive: binding = bindings->drive(sourceKey); break;
    default: return fail(diagnostic, Status::unsupportedMaterialKind, {}, group.id());
    }
    if (!binding) return fail(diagnostic, Status::invalidMaterialKey, {}, group.id());
    const SourceAudioResourceIndex50KY resource{binding->sourceResourceIndex};
    const auto* directoryRow = storage_->directory->row(resource.value);
    if (!directoryRow || storage_->directory->find(binding->sourceRegistryName) != directoryRow ||
        binding->sourceKey != sourceKey || binding->sourceFactoryFilename.empty())
        return fail(diagnostic, Status::missingResource, resource, group.id());
    auto& row = storage_->rows[resource.value];
    // Source26D9F0 selects signed32 count>0. Reject before that signed boundary,
    // not merely before unsigned wrap, and do not mutate the last owner.
    if (row.registrationCount >= static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))
        return fail(diagnostic, Status::registrationCountOverflow, resource, group.id());
    constexpr auto maxSize = std::numeric_limits<std::size_t>::max();
    if (group.count_ == maxSize || storage_->soundCount == maxSize)
        return fail(diagnostic, Status::storageCapacityOverflow, resource, group.id());
    const bool grow = group.count_ == group.capacity_;
    if (grow && (group.capacity_ > maxSize - coldAudioSoundGrowth50KY ||
        group.capacity_ + coldAudioSoundGrowth50KY > maxSize / sizeof(std::unique_ptr<ColdLogicalSound50KY>)))
        return fail(diagnostic, Status::storageCapacityOverflow, resource, group.id());
    try {
        std::unique_ptr<std::unique_ptr<ColdLogicalSound50KY>[]> expanded;
        if (grow) expanded = std::make_unique<std::unique_ptr<ColdLogicalSound50KY>[]>(
            group.capacity_ + coldAudioSoundGrowth50KY);
        auto sound = std::unique_ptr<ColdLogicalSound50KY>(new ColdLogicalSound50KY());
        sound->group_ = &group;
        sound->bindings_ = std::move(bindings);
        sound->binding_ = binding;
        sound->kind_ = kind;
        // All potentially throwing work finished. Moving owning pointers does
        // not relocate groups/sounds and cannot invalidate borrowed objects.
        if (grow) {
            for (std::size_t i = 0; i < group.count_; ++i) expanded[i] = std::move(group.sounds_[i]);
            group.sounds_ = std::move(expanded);
            group.capacity_ += coldAudioSoundGrowth50KY;
        }
        ColdAudioSoundHandle50KY soundHandle;
        soundHandle.group_ = handle;
        soundHandle.ordinal_ = group.count_;
        auto* actualSound = sound.get();
        // Source232ED0/235270 insertion owns the initial64-bit reference and
        // precedes233110 registration. No extra retain or deduplicated sound.
        group.sounds_[group.count_++] = std::move(sound);
        actualSound->fields_.flags10 = kind == GcMaterialSoundKind50KY::drive ? 0x81u : 1u;
        actualSound->fields_.loop1B0 = kind == GcMaterialSoundKind50KY::drive ? 1u : 0u;
        actualSound->fields_.resourceIndexA0 = static_cast<std::int32_t>(resource.value);
        ++row.registrationCount; //298A30 before298F80, not a sound retain.
        row.representative = actualSound;
        row.tag = ColdAudioRowTag50KY::pendingRegistration;
        ++storage_->soundCount;
        output = std::move(soundHandle);
        return true;
    } catch (const std::bad_alloc&) { return fail(diagnostic, Status::allocation, resource, group.id()); }
    catch (...) { return fail(diagnostic, Status::unexpectedFailure, resource, group.id()); }
}

bool ColdLogicalAudioDomain50KY::resolvePending(ColdAudioDiagnostic50KY& diagnostic) const noexcept {
    diagnostic = {};
    if (!requireInitialized(diagnostic)) return false;
    // This is an unavailable capability, not an attempted provider load and
    // not a negative source sample result. Counts/representatives stay intact.
    return fail(diagnostic, Status::sampleOwnerUnavailable);
}
void ColdLogicalAudioDomain50KY::clearRows() noexcept {
    for (auto& row : storage_->rows) {
        row.tag = ColdAudioRowTag50KY::cleared;
        row.representative = nullptr;
        row.registrationCount = 0;
    }
}
bool ColdLogicalAudioDomain50KY::clearRegistrations(ColdAudioDiagnostic50KY& diagnostic) noexcept {
    diagnostic = {};
    if (!requireInitialized(diagnostic)) return false;
    clearRows();
    return true;
}
void ColdLogicalAudioDomain50KY::destroyGroupAt(std::size_t id) noexcept {
    auto& group = *storage_->groups[id];
    //235150 returns the claimed ID before ordered cleanup. This is only the
    // host detached initial-reference lifetime, never142590 or a global guard.
    storage_->occupied[id] = false;
    // No callbacks or list mutations can occur here. The original snapshot is
    // therefore unnecessary; visit the actual owned list in insertion order.
    for (std::size_t i = 0; i < group.count_; ++i) {
        auto& sound = group.sounds_[i];
        // All admitted channels remain-1; source233670's stop is an early
        // return. Consume this group's sole reference, with no transport call.
        sound->fields_.referenceCount0 = 0;
        sound.reset();
    }
    storage_->soundCount -= group.count_;
    group.count_ = 0;
    group.sounds_.reset();
    group.capacity_ = 0;
    group.fields_.referenceCount0 = 0;
    storage_->groups[id].reset();
    --storage_->groupCount;
}
bool ColdLogicalAudioDomain50KY::destroyDetachedGroup(const ColdAudioGroupHandle50KY& handle,
    ColdAudioDiagnostic50KY& diagnostic) noexcept {
    diagnostic = {};
    const auto* group = checkedGroup(handle, diagnostic);
    if (!group) return false;
    for (const auto& row : storage_->rows) {
        if (row.tag == ColdAudioRowTag50KY::pendingRegistration && row.representative &&
            &row.representative->group() == group)
            return fail(diagnostic, Status::pendingRepresentativeBorrow, row.resource, group->id());
    }
    // Historical registration counts and representatives in other groups are
    // deliberately untouched, even when this deletes earlier same-row sounds.
    destroyGroupAt(handle.slot_);
    return true;
}
void ColdLogicalAudioDomain50KY::close() noexcept {
    state_ = ColdAudioDomainState50KY::closed;
    if (!storage_) return;
    clearRows(); // Expire row borrows before any sound can be destroyed.
    for (std::size_t i = coldAudioFirstDynamicGroup50KY; i < storage_->groups.size(); ++i)
        if (storage_->groups[i]) destroyGroupAt(i);
    storage_.reset(); // Releases lifetime token, rows and retained directory.
}
}
