#pragma once
// Cold actual-Pod01 EventN owner plus the source-proven nonstream negative
// response. The explicitly disabled native provider is host policy. It never
// opens a device, loads sample bytes, or reports successful playback.
#include "pod_materials50ed_fixture.h"
#include "original_player_identity50ik_fixture.h"
#include "../../src/assets/import_decode48c.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <string>
#include <string_view>

namespace lap_sound50GH {
inline constexpr std::size_t eventCount = 6;
inline constexpr std::size_t podArchiveBytes = 349110;
inline constexpr std::size_t eventMemberBytes = 462;
inline constexpr const char* podArchiveSha256 =
    "08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3";
inline constexpr const char* eventMemberSha256 =
    "49856d31023a3c57d4792df3675ef83928b82d25332e1b0d9b7f9fd80f0e5dfd";
inline constexpr const char* eventMember = "audio\\p_event.ini";
enum class ProviderResult : std::int32_t { Unavailable = -1 };

struct EventAuthoring {
    std::uint32_t key = 0;
    std::array<char, 48> filename{};
    bool stereo = false, loop = false, stream = false, streamAuthored = false;
};

// Field projection, NOT a guest layout. Only source-proven constructor fields
// are represented. Unlisted fields, parent pointers and audio hardware state
// are not guessed. These are actual native sound objects, not guest addresses.
struct SoundFields {
    std::uint32_t flags10 = 0, bits20 = 0, word24 = 0;
    std::int32_t sample28 = -1, channel2C = -1;
    std::uint32_t word38 = 100, word3C = 0, word40 = 4096;
    std::uint32_t word44 = 100, word48 = 0, word4C = 4096, word50 = 4096;
    std::uint32_t bits54 = 0x3f800000, bits58 = 0x3f800000, bits5C = 0x3e800000;
    std::uint32_t word7C = 0, word80 = 0;
    std::int32_t resourceIndexA0 = -1;
    std::uint32_t stream1A8 = 0, stereo1AC = 0, loop1B0 = 0, word1B4 = 0;
    std::int32_t streamIndex1B8 = -1;
};
struct Sound {
    EventAuthoring authored{};
    SoundFields fields{};
};
struct EventEntry {
    std::uint32_t key0 = 0;
    Sound* object4 = nullptr;
};

// The original row+4 first contains a sound owner and later a sample handle.
// Keep those alternatives typed: never cast a host pointer to a guest integer.
enum class ResourceValueKind : std::uint8_t { SampleResult, RegisteredOwner };
struct ResourceValue {
    ResourceValueKind kind = ResourceValueKind::SampleResult;
    Sound* registeredOwner = nullptr;
    ProviderResult sample = ProviderResult::Unavailable;
};
struct ResourceRow {
    std::array<char, 48> name0{};
    ResourceValue value4{};
    std::uint32_t referenceCount8 = 0;
};
struct ColdReceipt {
    std::size_t archiveBytes = 0, memberBytes = 0;
    std::uint32_t parsedEvents = 0, insertedRows = 0, initializedSounds = 0;
    std::uint32_t providerRequests = 0, sample28Stores = 0, row4Publications = 0;
    std::uint32_t referenceClears = 0, playbackCalls = 0;
    std::array<std::int32_t, eventCount> resolvedIndices{};
    // Source initialization increments each row, then stores its sound owner.
    // 26D9F0 resolves that owner, publishes sample28, and clears the refcount.
    std::array<bool, eventCount> ownerRegisteredBeforeResolution{};
    std::uint32_t registerPC = 0x2332c0, providerPC = 0x268930;
    std::uint32_t sampleStorePC = 0x2333a8, publicationPC = 0x26da70;
    std::uint32_t referenceClearPC = 0x299110;
};

struct Owner;
struct DispatchReceipt;
class DisabledAudioProvider;
bool prepareDisabledFromPod01(Owner&, DisabledAudioProvider&, std::string&,
    const std::filesystem::path&,std::uint32_t);
DispatchReceipt dispatch(Owner&, std::uint32_t, std::uint32_t, std::uint32_t) noexcept;

// Explicitly instantiate this class to choose silent native-client operation.
// It owns a real, bounded name registry. Unavailable is the actual result of
// this provider's resolve operation, not an assertion about device availability.
class DisabledAudioProvider {
public:
    DisabledAudioProvider() = default;
    DisabledAudioProvider(const DisabledAudioProvider&) = delete;
    DisabledAudioProvider& operator=(const DisabledAudioProvider&) = delete;
    DisabledAudioProvider(DisabledAudioProvider&&) = delete;
    DisabledAudioProvider& operator=(DisabledAudioProvider&&) = delete;
    const std::array<ResourceRow, eventCount>& rows() const noexcept { return rows_; }
    std::uint32_t size() const noexcept { return count_; }
    bool prepared() const noexcept { return boundOwner_ != nullptr; }
    // Cold registration keeps this noncopyable owner at its stable address.
    // Snapshot only fixed storage and bindings, never the enclosing object.
    template<class Bank> void registerRacerState(Bank& bank) {
        bank.owner(rows_);
        bank.owner(count_);
        bank.owner(boundOwner_);
    }
private:
    std::array<ResourceRow, eventCount> rows_{};
    std::uint32_t count_ = 0;
    Owner* boundOwner_ = nullptr;
    // Caller validates the authenticated nonstream request before this exact
    // 268930 boundary. No callback, sample decoder, I/O or hardware is hidden.
    ProviderResult resolve(const EventAuthoring&) const noexcept {
        return ProviderResult::Unavailable;
    }
    friend bool prepareDisabledFromPod01(Owner&, DisabledAudioProvider&,
        std::string&, const std::filesystem::path&,std::uint32_t);
    friend DispatchReceipt dispatch(Owner&, std::uint32_t, std::uint32_t,
        std::uint32_t) noexcept;
};

struct Owner {
    std::array<Sound, eventCount> sounds{};
    std::array<EventEntry, eventCount> entries200{};
    Sound* selectedOwner1F0 = nullptr;
    ColdReceipt receipt{};
    Owner() = default;
    Owner(const Owner&) = delete;
    Owner& operator=(const Owner&) = delete;
    Owner(Owner&&) = delete;
    Owner& operator=(Owner&&) = delete;
    bool prepared() const noexcept { return provider_ != nullptr; }
    // Every slot restores these fields into the same live execution arena;
    // entries and provider bindings keep their original stable addresses.
    template<class Bank> void registerRacerState(Bank& bank) {
        bank.owner(sounds);
        bank.owner(entries200);
        bank.owner(selectedOwner1F0);
        bank.owner(receipt);
        bank.owner(provider_);
    }
private:
    DisabledAudioProvider* provider_ = nullptr;
    friend bool prepareDisabledFromPod01(Owner&, DisabledAudioProvider&,
        std::string&, const std::filesystem::path&,std::uint32_t);
    friend DispatchReceipt dispatch(Owner&, std::uint32_t, std::uint32_t,
        std::uint32_t) noexcept;
};

namespace detail {
namespace shared = pod_materials50ed_fixture::detail;
inline std::string_view trim(std::string_view value) noexcept {
    while (!value.empty() && (value.front() == ' ' || value.front() == '\t' || value.front() == '\r'))
        value.remove_prefix(1);
    while (!value.empty() && (value.back() == ' ' || value.back() == '\t' || value.back() == '\r'))
        value.remove_suffix(1);
    return value;
}
inline std::array<EventAuthoring, eventCount> parse(const std::vector<std::uint8_t>& bytes) {
    std::array<EventAuthoring, eventCount> out{};
    std::array<std::uint8_t, eventCount> seen{};
    std::string_view text(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    std::uint32_t section = 0, sections = 0;
    bool emitter = false, countSeen = false;
    for (std::size_t at = 0; at < text.size();) {
        auto end = text.find('\n', at);
        if (end == std::string_view::npos) end = text.size();
        auto line = trim(text.substr(at, end - at)); at = end + 1;
        if (line.empty()) continue;
        if (line.front() == '[') {
            if (line == "[ResponseEmitter]") {
                shared::need(!emitter && !sections, "50GH repeated/out-of-order emitter section");
                emitter = true; section = 0;
            } else {
                shared::need(emitter && countSeen && line.size() == 8 &&
                    line.substr(0, 6) == "[Event" && line[7] == ']' &&
                    line[6] >= '1' && line[6] <= '6', "50GH unsupported EventN section");
                section = static_cast<std::uint32_t>(line[6] - '0');
                shared::need(section == sections + 1, "50GH EventN source order mismatch");
                ++sections; out[section - 1].key = section;
            }
            continue;
        }
        const auto equals = line.find('=');
        shared::need(equals != std::string_view::npos, "50GH malformed event property");
        const auto key = trim(line.substr(0, equals)), value = trim(line.substr(equals + 1));
        if (!section) {
            shared::need(emitter && !countSeen && key == "NumEvents" && value == "6",
                "50GH actual six-event declaration required");
            countSeen = true; continue;
        }
        auto& event = out[section - 1]; auto& mask = seen[section - 1];
        std::uint8_t bit = 0;
        if (key == "FileName") {
            bit = 1;
            shared::need(!value.empty() && value.size() < event.filename.size() &&
                value.substr(0, 9) == "SFX\\Race\\" && value.size() > 13 &&
                value.substr(value.size() - 4) == ".vag", "50GH unsupported source sound name");
            std::memcpy(event.filename.data(), value.data(), value.size());
        } else {
            shared::need(value == "0" || value == "1", "50GH invalid authored sound flag");
            const bool flag = value == "1";
            if (key == "IsStereo") { bit = 2; event.stereo = flag; }
            else if (key == "IsLoop") { bit = 4; event.loop = flag; }
            else if (key == "IsStream") { bit = 8; event.stream = flag; event.streamAuthored = true; }
            else shared::need(false, "50GH unsupported sound property");
        }
        shared::need(!(mask & bit), "50GH duplicate sound property"); mask |= bit;
    }
    shared::need(countSeen && sections == eventCount, "50GH incomplete actual event table");
    for (std::size_t i = 0; i < eventCount; ++i) {
        shared::need(seen[i] == 7 && !out[i].stream && !out[i].streamAuthored &&
            !out[i].stereo && out[i].loop == (i >= 2), "50GH unsupported authored event flags");
        for (std::size_t j = 0; j < i; ++j)
            shared::need(out[j].filename != out[i].filename, "50GH duplicate resource name");
    }
    return out;
}
inline std::int32_t findResource(const std::array<ResourceRow, eventCount>& rows,
    std::uint32_t count, const std::array<char, 48>& filename) noexcept {
    for (std::uint32_t i = 0; i < count; ++i)
        if (rows[i].name0 == filename) return static_cast<std::int32_t>(i);
    return -1;
}
} // namespace detail

// Cold only; both arguments must be fresh and unpublished. They must then live
// at stable, disjoint addresses for every dispatch. File read/hash/decode/parse
// finishes before native owners are changed. No saved guest state is consumed.
inline bool prepareDisabledFromPod01(Owner& out, DisabledAudioProvider& provider,
    std::string& error, const std::filesystem::path& archivePath =
        "work/native/project/disc/PODS/POD01/POD01.RES",std::uint32_t playerRacer60=1) {
    namespace a = rr::assets;
    using namespace detail;
    try {
        shared::need(!out.prepared() && !provider.prepared() && !provider.count_,
            "50GH requires a fresh owner and explicit disabled provider");
        const auto* identity=original_player_identity50IK::find(playerRacer60);
        shared::need(identity,"50GH selected original player identity unsupported");
        const auto archiveSize=identity->archiveBytes;
        const auto archiveDigest=identity->archiveSha256;
        shared::Inputs input; input.capacity = archiveSize;
        const auto archiveBytes = input.read(archivePath, archiveSize);
        shared::need(archiveBytes.size()==archiveSize&&shared::sha256(archiveBytes)==archiveDigest,
            "50GH selected original archive identity mismatch");
        a::ResArchive48C archive; a::DecodeError48C decode;
        a::DecodeLimits48C limits;
        limits.resource_file_bytes = archiveSize;
        limits.resource_decoded_bytes = 16u * 1024u * 1024u;
        limits.resource_entries = 512; limits.resource_name_bytes = 256;
        shared::need(a::decodeResource48C(archiveBytes, archive, decode, nullptr, limits),
            "50GH selected original decode failed: " + decode.code);
        std::vector<std::uint8_t> member;
        shared::need(a::extractResMember48C(archive, eventMember, member, decode, eventMemberBytes),
            "50GH actual p_event.ini extraction failed: " + decode.code);
        shared::need(member.size() == eventMemberBytes && shared::sha256(member) == eventMemberSha256,
            "50GH original event member identity mismatch");
        const auto authored = parse(member);
        // Authenticate and parse first; the remaining fixed-size work cannot
        // fail or allocate. Native indices result from real insertion/lookup.
        out.receipt = {}; out.receipt.archiveBytes = archiveBytes.size();
        out.receipt.memberBytes = member.size(); out.receipt.parsedEvents = eventCount;
        out.selectedOwner1F0 = nullptr;
        for (std::uint32_t i = 0; i < eventCount; ++i) {
            auto& row = provider.rows_[provider.count_]; row = {};
            row.name0 = authored[i].filename;
            // Host registry policy supplies the unloaded -1 sentinel to
            // 298900's row insertion. This is not a completed provider call:
            // 233110/26DB00 next replaces it with a typed registered owner,
            // and the only resolve invocation is the 2333A0 boundary below.
            ++provider.count_; ++out.receipt.insertedRows;
        }
        for (std::uint32_t i = 0; i < eventCount; ++i) {
            auto& sound = out.sounds[i]; sound = {}; sound.authored = authored[i];
            sound.fields.flags10 = 1u | (authored[i].loop ? 0x80u : 0u);
            sound.fields.loop1B0 = authored[i].loop ? 1u : 0u;
            // Actual source name lookup, not a caller-supplied index or a
            // negative index coerced into a no-resource answer.
            const auto index = findResource(provider.rows_, provider.count_, authored[i].filename);
            sound.fields.resourceIndexA0 = index;
            auto& row = provider.rows_[static_cast<std::size_t>(index)];
            ++row.referenceCount8; // 26DAF0 -> 298A30.
            row.value4.kind = ResourceValueKind::RegisteredOwner;
            row.value4.registeredOwner = &sound; // 26DB00 -> 298F80.
            out.entries200[i] = {authored[i].key, &sound};
            out.receipt.resolvedIndices[i] = index; ++out.receipt.initializedSounds;
        }
        // 26D9F0 resolves each referenced owner through 2332F0/268930 BEFORE
        // publishing its sample28 into row+4 and clearing row+8. The explicit
        // disabled provider replaces only the provider boundary, not this order.
        for (std::uint32_t i = 0; i < provider.count_; ++i) {
            auto& row = provider.rows_[i]; auto& sound = *row.value4.registeredOwner;
            out.receipt.ownerRegisteredBeforeResolution[i] =
                row.value4.kind == ResourceValueKind::RegisteredOwner && row.referenceCount8 == 1;
            const auto result = provider.resolve(sound.authored); ++out.receipt.providerRequests;
            sound.fields.sample28 = static_cast<std::int32_t>(result); ++out.receipt.sample28Stores;
            row.value4 = {ResourceValueKind::SampleResult, nullptr, result};
            ++out.receipt.row4Publications;
            row.referenceCount8 = 0; ++out.receipt.referenceClears;
        }
        provider.boundOwner_ = &out; out.provider_ = &provider; error.clear(); return true;
    } catch (const std::exception& exception) {
        try { error = exception.what(); } catch (...) {} return false;
    } catch (...) {
        try { error = "Unexpected 50GH cold owner preparation failure"; } catch (...) {} return false;
    }
}

enum class DispatchStatus : std::uint8_t {
    CompletedUnavailable, Unprepared, UnsupportedRequest, InvalidEventTable,
    InvalidResourceRow, UnsupportedSoundState
};
struct DispatchReceipt {
    DispatchStatus status = DispatchStatus::Unprepared;
    std::int32_t originalV0 = 0;
    ProviderResult providerResult = ProviderResult::Unavailable;
    std::uint32_t requestedKey = 0, selectedEventKey = 0, orderedEntriesVisited = 0;
    std::int32_t resourceIndex = -1;
    bool ownerSelectionStored = false, sound28Stored = false;
    Sound* previousOwner1F0 = nullptr;
    Sound* selectedOwner1F0 = nullptr;
    SoundFields before{}, after{};
    std::uint32_t playbackCalls = 0;
    std::uint32_t lookupPC = 0x292360, entryLookupPC = 0x294350;
    std::uint32_t ownerStorePC = 0x292810, soundMethodPC = 0x233410;
    std::uint32_t resourceReadPC = 0x298e98, soundStorePC = 0x233ec4;
    std::uint32_t negativeReturnPC = 0x233450;
    bool completed() const noexcept { return status == DispatchStatus::CompletedUnavailable; }
};

// Hot path: bounded, allocation-free, no I/O and no hardware/provider calls.
// Only the lap callback's actual key1 and caller arguments (0,0) are admitted.
// Guard failures change no source owner/sound/resource fields. Complete means
// a REAL source negative return, never successful sound playback. Only a
// completed receipt can acknowledge the 50GG OwnerSelection292360 boundary.
inline DispatchReceipt dispatch(Owner& owner, std::uint32_t key = 1,
    std::uint32_t flags = 0, std::uint32_t argumentA3 = 0) noexcept {
    DispatchReceipt receipt{}; receipt.requestedKey = key;
    auto* provider = owner.provider_;
    if (!provider || provider->boundOwner_ != &owner || provider->count_ != eventCount)
        return receipt;
    if (key != 1 || flags || argumentA3) {
        receipt.status = DispatchStatus::UnsupportedRequest; return receipt;
    }
    // Validate the complete actual table before selection; absent backing is
    // an error, never a manufactured no-match that acknowledges the callback.
    for (std::uint32_t i = 0; i < eventCount; ++i) {
        const auto& entry = owner.entries200[i]; const auto& sound = owner.sounds[i];
        if (entry.key0 != i + 1 || entry.object4 != &sound || sound.authored.key != entry.key0 ||
            sound.authored.stereo || sound.authored.stream || sound.authored.streamAuthored ||
            sound.authored.loop != (i >= 2)) {
            receipt.status = DispatchStatus::InvalidEventTable; return receipt;
        }
        const auto index = sound.fields.resourceIndexA0;
        if (index < 0 || static_cast<std::uint32_t>(index) >= provider->count_ ||
            static_cast<std::uint32_t>(index) != i) {
            receipt.status = DispatchStatus::InvalidResourceRow; return receipt;
        }
        const auto& row = provider->rows_[static_cast<std::size_t>(index)];
        if (row.name0 != sound.authored.filename || row.referenceCount8 != 0 ||
            row.value4.kind != ResourceValueKind::SampleResult || row.value4.registeredOwner ||
            row.value4.sample != ProviderResult::Unavailable) {
            receipt.status = DispatchStatus::InvalidResourceRow; return receipt;
        }
    }
    Sound* selected = nullptr;
    for (const auto& entry : owner.entries200) {
        ++receipt.orderedEntriesVisited;
        if (entry.key0 == key) { selected = entry.object4; receipt.selectedEventKey = entry.key0; break; }
    }
    if (!selected) { receipt.status = DispatchStatus::InvalidEventTable; return receipt; }
    if (selected->fields.sample28 >= 0 || selected->fields.stream1A8 ||
        selected->fields.stereo1AC || selected->fields.loop1B0 || selected->fields.flags10 != 1) {
        receipt.status = DispatchStatus::UnsupportedSoundState; return receipt;
    }
    receipt.before = selected->fields;
    receipt.resourceIndex = selected->fields.resourceIndexA0;
    receipt.previousOwner1F0 = owner.selectedOwner1F0;
    receipt.selectedOwner1F0 = selected;
    // Source 292810 occurs before the selected concrete virtual+1C call.
    owner.selectedOwner1F0 = selected; receipt.ownerSelectionStored = true;
    // 233410 calls233E90 first. Negative28/nonstream reaches26DB10->298E60;
    // read a VALID row+4 and preserve its actual negative provider result.
    const auto& row = provider->rows_[static_cast<std::size_t>(receipt.resourceIndex)];
    receipt.providerResult = row.value4.sample;
    selected->fields.sample28 = static_cast<std::int32_t>(receipt.providerResult);
    receipt.sound28Stored = true;
    // 233E90 skips metadata; 233410's negative nonstream branch returns0 at
    // 233450, BEFORE channel2C/word24/word7C or any playback call can change.
    receipt.after = selected->fields;
    receipt.originalV0 = 0; receipt.status = DispatchStatus::CompletedUnavailable;
    return receipt;
}
} // namespace lap_sound50GH
