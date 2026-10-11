// Prospective native source-only regression. NOT compiled or executed here.
// Arguments: retained authenticated APPSTART.RES and original extracted GC AUB.
// No PCM, provider, device, playback, gameplay, guest, PS2, IOP or VU execution.
#include "../../src/game/cold_logical_audio_domain50ky.h"
#include <array>
#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <new>
#include <type_traits>
#include <vector>

// Prospective allocator-failure instrumentation belongs to this executable,
// never the production API. Enable only around one domain operation; ordinary
// fixture/authentication allocations occur before the injected failure.
namespace allocation_probe {
std::ptrdiff_t remaining = -1;
struct Scope {
    explicit Scope(std::ptrdiff_t count) noexcept { remaining = count; }
    ~Scope() { remaining = -1; }
};
}
void* operator new(std::size_t bytes) {
    if (allocation_probe::remaining == 0) throw std::bad_alloc();
    if (allocation_probe::remaining > 0) --allocation_probe::remaining;
    if (void* memory = std::malloc(bytes ? bytes : 1)) return memory;
    throw std::bad_alloc();
}
void* operator new[](std::size_t bytes) { return ::operator new(bytes); }
void operator delete(void* memory) noexcept { std::free(memory); }
void operator delete[](void* memory) noexcept { std::free(memory); }
void operator delete(void* memory, std::size_t) noexcept { std::free(memory); }
void operator delete[](void* memory, std::size_t) noexcept { std::free(memory); }

namespace a = rr::assets;
namespace g = rr::game;
namespace {
using Status = g::ColdAudioStatus50KY;
using Kind = g::GcMaterialSoundKind50KY;
using Tag = g::ColdAudioRowTag50KY;
using Domain = g::ColdLogicalAudioDomain50KY;
using GroupHandle = g::ColdAudioGroupHandle50KY;
using SoundHandle = g::ColdAudioSoundHandle50KY;
using DirectoryOwner = std::shared_ptr<const a::OriginalAudioDirectory50KX>;
using BindingOwner = std::shared_ptr<const a::GcMaterialResourceBindings50KX>;
std::size_t checks = 0;
void require(bool value, const char* label) {
    ++checks;
    if (!value) { std::cerr << label << '\n'; std::exit(1); }
}
std::vector<std::uint8_t> read(const char* path) {
    std::ifstream input(path, std::ios::binary);
    require(bool(input), "retained original input opens");
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}
struct Inputs { DirectoryOwner directory; BindingOwner bindings; };
Inputs prepareInputs(const std::vector<std::uint8_t>& app, const std::vector<std::uint8_t>& gc) {
    a::OriginalUiArchive50KG archive;
    a::OriginalUiArchiveDiagnostic50KG archiveDiagnostic;
    a::AudioDirectoryDiagnostic50KX assetDiagnostic;
    auto directory = std::make_shared<a::OriginalAudioDirectory50KX>();
    auto bindings = std::make_shared<a::GcMaterialResourceBindings50KX>();
    require(archive.load(a::OriginalUiArchiveId50KG::appStart, app, archiveDiagnostic),
        "real APPSTART owner authenticates exact original bytes");
    require(directory->prepare(archive, assetDiagnostic), "real267-row directory prepares");
    require(bindings->prepare(*directory, gc, assetDiagnostic), "real24-row GC binding prepares");
    return {directory, bindings};
}
GroupHandle createGroup(Domain& domain) {
    GroupHandle handle;
    g::ColdAudioDiagnostic50KY diagnostic;
    require(domain.createDetachedGroup(handle, diagnostic) && diagnostic.status == Status::none,
        "actual detached group created");
    return handle;
}
SoundHandle createSound(Domain& domain, const GroupHandle& group,
    const BindingOwner& bindings, Kind kind, std::uint32_t key) {
    SoundHandle handle;
    g::ColdAudioDiagnostic50KY diagnostic;
    require(domain.createGcSound(group, bindings, kind, key, handle, diagnostic) &&
        diagnostic.status == Status::none, "distinct actual GC sound created and registered");
    return handle;
}
void checkFields(const g::ColdLogicalSound50KY& sound, Kind kind, std::uint32_t resource) {
    const auto& f = sound.fields();
    require(f.referenceCount0 == 1 && f.flags10 == (kind == Kind::drive ? 0x81u : 1u) &&
        f.word14 == 0 && f.word18 == 0, "64-bit sole reference and proven base fields");
    require(f.durationBits20 == 0 && f.elapsedBits24 == 0 && f.sample28 == -1 && f.channel2C == -1,
        "cold zeros and sentinels never imply actual sample or voice");
    require(f.volume38 == 100 && f.pan3C == 0 && f.pitch40 == 4096 && f.effective44 == 100 &&
        f.effective48 == 0 && f.effectivePitch4C == 4096 && f.basePitch50 == 4096,
        "constructor gain/pan/pitch projection, not provider metadata");
    require(f.multiplierBits54 == 0x3f800000u && f.multiplierBits58 == 0x3f800000u &&
        f.multiplierBits5C == 0x3e800000u &&
        f.positionBits60 == std::array<std::uint32_t, 4>{{0, 0, 0, 0x3f800000}} &&
        f.vectorBits90 == std::array<std::uint32_t, 4>{{0, 0, 0, 0x3f800000}},
        "proven binary32 constants and both initialized vectors");
    require(f.word70 == 0 && f.word74 == 0 && f.word78 == 0 && f.word7C == 0 && f.word80 == 0 &&
        f.resourceIndexA0 == static_cast<std::int32_t>(resource) && sound.resource().value == resource,
        "known words and actual global resource ordinal");
    require(f.stream1A8 == 0 && f.stereo1AC == 0 && f.loop1B0 == (kind == Kind::drive ? 1u : 0u) &&
        f.word1B4 == 0 && f.streamIndex1B8 == -1, "exact bounded nonstream material flag domain");
}
void checkEmptyRows(const Domain& domain, Tag expected) {
    require(domain.rowCount() == 267, "mutable registry keeps all267 original rows");
    for (std::uint32_t i = 0; i < 267; ++i) {
        const auto* row = domain.row({i});
        require(row && row->resource.value == i && row->directory == domain.directory()->row(i) &&
            row->tag == expected && !row->representative && row->registrationCount == 0,
            "empty/cleared registry row has no fabricated sample0");
    }
    require(!domain.row({267}) && !domain.row({}), "out-of-range and invalid resource identities reject");
}

void registrationAndClear(const Inputs& inputs) {
    Domain domain;
    g::ColdAudioDiagnostic50KY d;
    GroupHandle invalid, group;
    SoundHandle soundOutput;
    require(domain.state() == g::ColdAudioDomainState50KY::empty && !domain.coldInitialized() &&
        !domain.rowCount() && !domain.directory(), "fresh domain has no ready backend claim");
    require(!domain.createDetachedGroup(group, d) && d.status == Status::notInitialized,
        "groups require real directory-backed initialization");
    require(!domain.prepare({}, d) && d.status == Status::directoryNotPrepared,
        "null owner cannot authorize directory rows");
    auto emptyDirectory = std::make_shared<a::OriginalAudioDirectory50KX>();
    require(!domain.prepare(emptyDirectory, d) && d.status == Status::directoryNotPrepared,
        "unprepared real owner cannot authorize directory rows");
    require(domain.prepare(inputs.directory, d) && domain.coldInitialized() &&
        domain.directory() == inputs.directory.get(), "prepared directory is actually retained");
    checkEmptyRows(domain, Tag::unreferenced);
    group = createGroup(domain);
    const auto* actualGroup = domain.inspectGroup(group, d);
    require(actualGroup && actualGroup->id().value == 2 && actualGroup->soundCount() == 0 &&
        actualGroup->soundCapacity() == 8, "first isolated dynamic ID2 with real initial8-slot storage");
    const auto& gf = actualGroup->fields();
    require(gf.referenceCount0 == 1 && !gf.previous10 && !gf.next14 && !gf.firstChild18 && !gf.parent1C &&
        gf.mask20 == 0 && gf.mask24 == 0 && gf.byte28 == 0xff &&
        gf.flags29.mask == 0x0f && gf.flags29.value == 3 &&
        gf.flags30.mask == 1 && gf.flags30.value == 1 && gf.volume38 == 100 && gf.state44 == 0,
        "partial detached GameObject fields preserve unknown upper flag bits as unknown");
    require(sizeof(gf.referenceCount0) == 8 && sizeof(gf.volume38) == 8,
        "source group refcount and volume are64-bit");
    require(!domain.createGcSound(invalid, inputs.bindings, Kind::collision, 1, soundOutput, d) &&
        d.status == Status::invalidGroupHandle, "caller cannot fabricate group identity");
    require(!domain.createGcSound(group, {}, Kind::collision, 1, soundOutput, d) &&
        d.status == Status::bindingsNotPrepared, "fabricated/null binding owner rejected");
    auto emptyBindings = std::make_shared<a::GcMaterialResourceBindings50KX>();
    require(!domain.createGcSound(group, emptyBindings, Kind::collision, 1, soundOutput, d) &&
        d.status == Status::bindingsNotPrepared, "unprepared actual binding owner rejected");
    require(!domain.createGcSound(group, inputs.bindings, static_cast<Kind>(255), 1, soundOutput, d) &&
        d.status == Status::unsupportedMaterialKind, "unsupported flags cannot enter through forged kind");
    require(!domain.createGcSound(group, inputs.bindings, Kind::collision, 0, soundOutput, d) &&
        d.status == Status::invalidMaterialKey && actualGroup->soundCount() == 0,
        "invalid key does not construct or register a sound");
    require(!domain.createGcSound(group, inputs.bindings, Kind::drive, 13, soundOutput, d) &&
        d.status == Status::invalidMaterialKey && actualGroup->soundCount() == 0,
        "out-of-range authored kind/key cannot select a fallback sound");

    std::array<SoundHandle, 24> handles{};
    std::array<const g::ColdLogicalSound50KY*, 24> sounds{};
    for (std::size_t kind = 0; kind < 2; ++kind) {
        for (std::uint32_t key = 1; key <= 12; ++key) {
            const auto offset = kind * 12 + key - 1;
            const auto materialKind = kind ? Kind::drive : Kind::collision;
            handles[offset] = createSound(domain, group, inputs.bindings, materialKind, key);
            sounds[offset] = domain.inspectSound(handles[offset], d);
            const auto* binding = kind ? inputs.bindings->drive(key) : inputs.bindings->collision(key);
            require(sounds[offset] && &sounds[offset]->group() == actualGroup &&
                &sounds[offset]->binding() == binding &&
                sounds[offset]->factoryFilename() == binding->sourceFactoryFilename,
                "sound owns real group relation and retained authentic material binding");
            checkFields(*sounds[offset], materialKind, binding->sourceResourceIndex);
            for (std::size_t previous = 0; previous < offset; ++previous)
                require(sounds[previous] != sounds[offset] &&
                    domain.inspectSound(handles[previous], d) == sounds[previous],
                    "every sound is distinct and stable across pointer-array growth");
        }
    }
    require(domain.inspectGroup(group, d) == actualGroup && actualGroup->soundCount() == 24 &&
        actualGroup->soundCapacity() == 24 && !actualGroup->soundAt(24),
        "group identity stable with source8+16 storage capacity");
    const auto* pod = domain.row({109});
    require(pod && pod->tag == Tag::pendingRegistration && pod->registrationCount == 4 &&
        pod->representative == sounds[3] && sounds[0] != sounds[3] &&
        sounds[0]->fields().sample28 == -1 && sounds[3]->fields().sample28 == -1,
        "duplicate resource keeps four owned sounds and the latest actual representative");
    std::size_t pendingRows = 0, registrations = 0;
    std::array<g::ColdAudioResourceRow50KY, 267> before{};
    for (std::uint32_t i = 0; i < 267; ++i) {
        const auto& row = *domain.row({i});
        before[i] = row;
        if (row.tag == Tag::pendingRegistration) {
            ++pendingRows;
            registrations += row.registrationCount;
            require(row.representative && row.representative->resource().value == i &&
                row.directory->sourceIndex == i, "pending demand inspectable in true directory order");
        }
    }
    require(pendingRows == 21 && registrations == 24 && domain.ownedCounts().sounds == 24 &&
        domain.ownedCounts().groups == 1 && domain.ownedCounts().gameObjectContribution == 1,
        "24 sounds21 resource demands and one owned GameObject contribution");
    require(!domain.resolvePending(d) && d.status == Status::sampleOwnerUnavailable,
        "unimplemented provider is a capability refusal, not a synthesized sample result");
    require(!domain.prepare(inputs.directory, d) && d.status == Status::alreadyInitialized,
        "repeated initialization cannot reset live group occupancy");
    require(!domain.destroyDetachedGroup(group, d) && d.status == Status::pendingRepresentativeBorrow,
        "pending representative guards group teardown");
    for (std::uint32_t i = 0; i < 267; ++i) {
        const auto* row = domain.row({i});
        require(row->tag == before[i].tag && row->representative == before[i].representative &&
            row->registrationCount == before[i].registrationCount && row->directory == before[i].directory,
            "failed provider/reprepare/teardown leaves every registration unchanged");
    }
    require(domain.clearRegistrations(d), "explicit clear-only registry reset succeeds");
    checkEmptyRows(domain, Tag::cleared);
    require(domain.row({109}) == pod && domain.inspectGroup(group, d) == actualGroup &&
        actualGroup->soundCount() == 24 && domain.ownedCounts().sounds == 24,
        "clear-only reset preserves stable rows groups and all owned sounds");
    for (std::size_t i = 0; i < sounds.size(); ++i) {
        require(domain.inspectSound(handles[i], d) == sounds[i], "clear does not invalidate logical sound identity");
        checkFields(*sounds[i], i < 12 ? Kind::collision : Kind::drive, sounds[i]->resource().value);
    }
    const auto afterClear = createSound(domain, group, inputs.bindings, Kind::collision, 1);
    require(domain.row({109})->registrationCount == 1 && domain.row({109})->tag == Tag::pendingRegistration &&
        domain.row({109})->representative == domain.inspectSound(afterClear, d) &&
        domain.inspectSound(handles[0], d) == sounds[0] && actualGroup->soundCount() == 25,
        "new registration after clear counts only new work while earlier owned sounds survive");
    require(!domain.destroyDetachedGroup(group, d) && d.status == Status::pendingRepresentativeBorrow &&
        domain.clearRegistrations(d), "new pending work requires another explicit clear before teardown");
    require(domain.destroyDetachedGroup(group, d) && domain.ownedCounts().groups == 0 &&
        domain.ownedCounts().sounds == 0 && domain.ownedCounts().gameObjectContribution == 0,
        "explicit cleared group teardown consumes actual owned storage");
    require(!domain.inspectGroup(group, d) && d.status == Status::staleGroupHandle &&
        !domain.inspectSound(handles[0], d) && d.status == Status::staleGroupHandle,
        "group and sound handles expire on group destruction");
    const auto replacement = createGroup(domain);
    require(domain.inspectGroup(replacement, d)->id().value == 2 && !domain.inspectGroup(group, d) &&
        d.status == Status::staleGroupHandle, "reused source ID has a new private generation");
    domain.close();
    domain.close();
    require(domain.state() == g::ColdAudioDomainState50KY::closed && !domain.rowCount() &&
        !domain.directory() && !domain.ownedCounts().sounds, "idempotent close releases all cold ownership");
    require(!domain.prepare(inputs.directory, d) && d.status == Status::domainClosed,
        "closed domain cannot be reopened with stale namespace handles");
}

void historicalRegistrationAndCapacity(const Inputs& inputs) {
    Domain domain, other;
    g::ColdAudioDiagnostic50KY d;
    require(domain.prepare(inputs.directory, d) && other.prepare(inputs.directory, d), "independent test domains prepare");
    auto first = createGroup(domain);
    auto second = createGroup(domain);
    auto firstSound = createSound(domain, first, inputs.bindings, Kind::collision, 1);
    createSound(domain, first, inputs.bindings, Kind::collision, 2);
    const auto last = createSound(domain, second, inputs.bindings, Kind::collision, 3);
    const auto* lastSound = domain.inspectSound(last, d);
    require(!other.inspectGroup(first, d) && d.status == Status::wrongDomain &&
        !other.inspectSound(firstSound, d) && d.status == Status::wrongDomain,
        "same manifest does not alias independently owned namespaces");
    require(!other.destroyDetachedGroup(first, d) && d.status == Status::wrongDomain,
        "wrong-domain destructive operation rejects");
    SoundHandle output;
    require(!other.createGcSound(first, inputs.bindings, Kind::collision, 1, output, d) &&
        d.status == Status::wrongDomain, "foreign group cannot receive actual sounds");
    require(domain.row({109})->registrationCount == 3 && domain.row({109})->representative == lastSound,
        "latest group sound replaces representative without merging earlier sounds");
    require(domain.destroyDetachedGroup(first, d) && domain.ownedCounts().sounds == 1 &&
        domain.row({109})->registrationCount == 3 && domain.row({109})->representative == lastSound,
        "deleting only earlier nonrepresentatives neither decrements nor retargets pending row");
    require(!domain.inspectSound(firstSound, d) && d.status == Status::staleGroupHandle,
        "earlier sound handle expires independently of remaining pending representative");
    require(!domain.destroyDetachedGroup(second, d) && d.status == Status::pendingRepresentativeBorrow &&
        d.resource.value == 109, "exact pending borrow blocks its owning group");
    require(domain.clearRegistrations(d) && domain.destroyDetachedGroup(second, d),
        "explicit clear admits last representative group destruction");

    std::array<GroupHandle, 23> groups{};
    for (std::size_t i = 0; i < groups.size(); ++i) {
        groups[i] = createGroup(domain);
        require(domain.inspectGroup(groups[i], d)->id().value == i + 2, "first free dynamic group ID claimed in order");
    }
    GroupHandle unchanged = groups[0];
    const auto* initial = domain.inspectGroup(unchanged, d);
    require(!domain.createDetachedGroup(unchanged, d) && d.status == Status::groupIdsExhausted &&
        domain.ownedCounts().groups == 23 && domain.inspectGroup(unchanged, d) == initial,
        "25-slot namespace reserves0/1, admits23 groups, and exhaustion preserves output");
    const auto removed = groups[5];
    require(domain.destroyDetachedGroup(removed, d), "empty group returns its claimed ID");
    const auto reused = createGroup(domain);
    require(domain.inspectGroup(reused, d)->id().value == 7 && !domain.inspectGroup(removed, d) &&
        d.status == Status::staleGroupHandle, "first hole is reused without stale generation alias");
}

void fullEightMaterialFamilies(const Inputs& inputs) {
    Domain domain;
    g::ColdAudioDiagnostic50KY d;
    require(domain.prepare(inputs.directory, d), "192-sound domain prepares");
    const auto group = createGroup(domain);
    std::array<SoundHandle, 192> handles{};
    std::array<const g::ColdLogicalSound50KY*, 192> addresses{};
    std::size_t at = 0;
    for (std::size_t family = 0; family < 8; ++family)
        for (std::size_t kind = 0; kind < 2; ++kind)
            for (std::uint32_t key = 1; key <= 12; ++key) {
                handles[at] = createSound(domain, group, inputs.bindings, kind ? Kind::drive : Kind::collision, key);
                addresses[at] = domain.inspectSound(handles[at], d);
                for (std::size_t earlier = 0; earlier < at; ++earlier)
                    require(addresses[at] != addresses[earlier], "192 registrations own192 distinct logical sounds");
                ++at;
            }
    require(domain.ownedCounts().sounds == 192 && domain.ownedCounts().gameObjectContribution == 1 &&
        domain.inspectGroup(group, d)->soundCapacity() == 200 &&
        domain.row({109})->registrationCount == 32 && domain.row({109})->representative == addresses[171],
        "eight source families retain all sounds with32 pod collisions and last genuine representative");
    std::size_t pending = 0, count = 0;
    for (std::uint32_t i = 0; i < 267; ++i) {
        const auto* row = domain.row({i});
        if (row->tag == Tag::pendingRegistration) { ++pending; count += row->registrationCount; }
    }
    require(pending == 21 && count == 192, "21 pending resources do not collapse192 sounds");
    for (std::size_t i = 0; i < addresses.size(); ++i)
        require(domain.inspectSound(handles[i], d) == addresses[i], "all192 borrowed sound addresses stay stable");
    // Destructor close clears these still-pending rows before ordered release.
}

void addressReuseAndOwnership(const std::vector<std::uint8_t>& app,
    const std::vector<std::uint8_t>& gc, const Inputs& sharedInputs) {
    alignas(Domain) unsigned char storage[sizeof(Domain)];
    g::ColdAudioDiagnostic50KY d;
    auto* first = new (storage) Domain();
    require(first->prepare(sharedInputs.directory, d), "first same-address domain prepares");
    const auto oldGroup = createGroup(*first);
    const auto oldSound = createSound(*first, oldGroup, sharedInputs.bindings, Kind::collision, 1);
    first->~Domain();
    auto* second = new (storage) Domain();
    require(second->prepare(sharedInputs.directory, d), "second same-address domain prepares");
    createGroup(*second);
    require(!second->inspectGroup(oldGroup, d) && d.status == Status::staleGroupHandle &&
        !second->inspectSound(oldSound, d) && d.status == Status::staleGroupHandle,
        "domain address reuse cannot revive expired token ownership");
    second->~Domain();

    auto ownInputs = prepareInputs(app, gc);
    std::weak_ptr<const a::OriginalAudioDirectory50KX> directoryLife = ownInputs.directory;
    std::weak_ptr<const a::GcMaterialResourceBindings50KX> bindingLife = ownInputs.bindings;
    Domain domain;
    require(domain.prepare(ownInputs.directory, d), "retention-test domain prepares");
    const auto group = createGroup(domain);
    const auto sound = createSound(domain, group, ownInputs.bindings, Kind::collision, 1);
    ownInputs.directory.reset();
    ownInputs.bindings.reset();
    require(!directoryLife.expired() && !bindingLife.expired() &&
        domain.inspectSound(sound, d)->factoryFilename() == "sfx\\race\\pod_coll.wav" &&
        domain.row({109})->directory->sourceName == "audio\\sfx\\race\\pod_coll.vag",
        "actual immutable owners retained after caller releases all owning references");
    require(domain.clearRegistrations(d) && domain.destroyDetachedGroup(group, d) &&
        bindingLife.expired() && !directoryLife.expired(), "sound destruction releases its genuine binding owner");
    domain.close();
    require(directoryLife.expired(), "close releases genuine retained directory after clearing row borrows");

    auto pendingInputs = prepareInputs(app, gc);
    std::weak_ptr<const a::OriginalAudioDirectory50KX> pendingDirectoryLife = pendingInputs.directory;
    std::weak_ptr<const a::GcMaterialResourceBindings50KX> pendingBindingLife = pendingInputs.bindings;
    Domain pendingDomain;
    require(pendingDomain.prepare(pendingInputs.directory, d), "close-with-pending domain prepares");
    const auto pendingGroup = createGroup(pendingDomain);
    createSound(pendingDomain, pendingGroup, pendingInputs.bindings, Kind::drive, 1);
    pendingInputs.directory.reset();
    pendingInputs.bindings.reset();
    pendingDomain.close();
    require(pendingDirectoryLife.expired() && pendingBindingLife.expired() &&
        pendingDomain.ownedCounts().groups == 0 && pendingDomain.ownedCounts().sounds == 0,
        "direct close safely clears live pending borrows before releasing every retained owner");
}

void allocationFailures(const Inputs& inputs) {
    g::ColdAudioDiagnostic50KY d;
    std::size_t prepareFailures = 0, groupFailures = 0;
    bool preparedOnce = false, groupOnce = false;
    for (std::ptrdiff_t failAfter = 0; failAfter < 16 && !preparedOnce; ++failAfter) {
        Domain domain;
        bool success;
        { allocation_probe::Scope injected(failAfter); success = domain.prepare(inputs.directory, d); }
        if (success) { preparedOnce = true; continue; }
        ++prepareFailures;
        require(d.status == Status::allocation && domain.state() == g::ColdAudioDomainState50KY::empty &&
            !domain.rowCount() && !domain.directory(), "every preparation allocation failure publishes nothing");
        require(domain.prepare(inputs.directory, d), "failed preparation stays retryable");
    }
    require(preparedOnce && prepareFailures >= 2, "prospective injection covers storage and lifetime token allocation");
    for (std::ptrdiff_t failAfter = 0; failAfter < 16 && !groupOnce; ++failAfter) {
        Domain domain;
        require(domain.prepare(inputs.directory, d), "group-failure test domain prepares");
        GroupHandle group;
        bool success;
        { allocation_probe::Scope injected(failAfter); success = domain.createDetachedGroup(group, d); }
        if (success) { groupOnce = true; continue; }
        ++groupFailures;
        require(d.status == Status::allocation && !domain.ownedCounts().groups &&
            !domain.ownedCounts().gameObjectContribution, "group/object-array failure claims no ID or object contribution");
        group = createGroup(domain);
        require(domain.inspectGroup(group, d)->id().value == 2, "failed group construction did not consume first free ID");
    }
    require(groupOnce && groupFailures >= 2, "prospective injection covers group and initial sound-array allocation");
    for (const bool growth : {false, true}) {
        bool soundOnce = false;
        std::size_t failures = 0;
        for (std::ptrdiff_t failAfter = 0; failAfter < 16 && !soundOnce; ++failAfter) {
            Domain domain;
            require(domain.prepare(inputs.directory, d), "sound-failure test domain prepares");
            const auto group = createGroup(domain);
            SoundHandle output;
            if (growth) for (unsigned i = 0; i < 8; ++i)
                output = createSound(domain, group, inputs.bindings, Kind::collision, 1);
            const auto* oldOutput = growth ? domain.inspectSound(output, d) : nullptr;
            const auto* actualGroup = domain.inspectGroup(group, d);
            const auto* oldRepresentative = domain.row({109})->representative;
            const auto oldCount = domain.row({109})->registrationCount;
            bool success;
            { allocation_probe::Scope injected(failAfter);
              success = domain.createGcSound(group, inputs.bindings, Kind::collision, 2, output, d); }
            if (success) {
                soundOnce = true;
                require(domain.row({109})->registrationCount == oldCount + 1 &&
                    domain.row({109})->representative == domain.inspectSound(output, d),
                    "first successful allocation sequence publishes exactly once");
                continue;
            }
            ++failures;
            require(d.status == Status::allocation && actualGroup->soundCapacity() == 8 &&
                actualGroup->soundCount() == (growth ? 8u : 0u) &&
                domain.ownedCounts().sounds == (growth ? 8u : 0u) &&
                domain.row({109})->registrationCount == oldCount &&
                domain.row({109})->representative == oldRepresentative &&
                domain.row({109})->tag == (growth ? Tag::pendingRegistration : Tag::unreferenced),
                "sound/growth allocation failure preserves capacity storage tags counts and representative");
            if (growth) require(domain.inspectSound(output, d) == oldOutput,
                "failed sound admission preserves caller's previous valid output handle");
        }
        require(soundOnce && failures >= (growth ? 2u : 1u),
            "prospective injection covers both expanded pointer-array and sound allocation");
    }
}
}

static_assert(!std::is_copy_constructible_v<Domain> && !std::is_move_constructible_v<Domain>);
static_assert(!std::is_default_constructible_v<g::ColdLogicalGroup50KY>);
static_assert(!std::is_default_constructible_v<g::ColdLogicalSound50KY>);
static_assert(!std::is_copy_constructible_v<g::ColdLogicalGroup50KY>);
static_assert(!std::is_copy_constructible_v<g::ColdLogicalSound50KY>);
int main(int argc, char** argv) {
    require(argc == 3, "provide retained APPSTART.RES and original extracted GC AUB paths");
    const auto app = read(argv[1]);
    const auto gc = read(argv[2]);
    const auto inputs = prepareInputs(app, gc);
    registrationAndClear(inputs);
    historicalRegistrationAndCapacity(inputs);
    fullEightMaterialFamilies(inputs);
    addressReuseAndOwnership(app, gc, inputs);
    allocationFailures(inputs);
    std::cout << "cold_logical_audio_domain50ky: " << checks << " checks; cold pending registration only\n";
}
