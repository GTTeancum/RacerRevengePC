#pragma once
#include "../../../src/game/original_save_profile_stage50kg.h"
#include "../../../src/game/original_ui_hall_records50kg.h"
#include "../../../src/game/original_race_record_insertion50kg.h"
#include "original_persisted_profile_name50kg.h"
#include <memory>
#include <optional>

namespace rr::platform {
class OriginalUiRiderProfileOwner50KG;
class OriginalSaveNativeReadStage50KG;
// Original application globals, NOT per-profile/service observations. Source
// data382BF0/382BF8 starts-3;433E98 is BSS0. Not instantiated by this proposal.
// Any future service must retain ONE actual application instance, never reset
// these on page entry, load, provider replacement or profile reconstruction.
struct OriginalSaveStatusGlobals50KG {
    std::int32_t previous382BF0=-3,current382BF8=-3,change433E98=0;
};
class OriginalSaveProfileOwner50KG final {
public:
    // Cold constructor-only knowledge. Does not imply a storage device exists,
    // has been probed, supports native IO, or has an authenticated selected path.
    OriginalSaveProfileOwner50KG() noexcept;
    ~OriginalSaveProfileOwner50KG()=default;
    enum class HallOrigin:std::uint8_t {none,localizedFreshRecords,originalPayload,nativeBeta};
    struct KnownFlags100BC {
        std::uint8_t value=0,knownMask=0;
        // Unknown bits are not defined by value. No whole-byte export exists.
    };
    struct Observation {
        std::uint32_t selectedServiceSlot=99,seenCard3C8=0;
        std::uint32_t cache1914=0,directoryCache1918=0;
        std::optional<std::uint32_t> identity191C;
        KnownFlags100BC flags{};
        HallOrigin hallOrigin=HallOrigin::none;
        // Native latest Hall descendant revision; root/load origin above is
        // preserved. This observation is not an earned-result receipt.
        std::uint64_t hallRecordEpoch=0;
    };
    Observation observation() const noexcept;
    bool dirty100BCBit0() const noexcept {return (cardFlags_.value&1u)!=0;}
    bool loaded100BCBit1() const noexcept {return (cardFlags_.value&2u)!=0;}
private:
    friend class OriginalUiRiderProfileOwner50KG;
    OriginalSaveProfileOwner50KG(const OriginalSaveProfileOwner50KG&)=default;
    OriginalSaveProfileOwner50KG& operator=(const OriginalSaveProfileOwner50KG&)=delete;
    //17FFB0/180098..B0 and1864CC..F4 write empty C strings only.
    void recordFreshNames() noexcept;
    bool namesReady() const noexcept;
    bool persistedName(bool career,std::uint32_t index,
        OriginalPersistedProfileName50KG& out)const noexcept;
    bool namesInitialized_=false;
    std::array<OriginalPersistedProfileName50KG,27> careerNames_{};
    std::array<OriginalPersistedProfileName50KG,2> playerNames_{};
    // Only the actual private Rider result commit calls this, after187D80.
    void markDirty1AB648() noexcept;
    // Only the existing authenticated fresh Hall initializer calls this.
    // Retain an immutable exact initializer root before live publication.
    // Allocation failure leaves this metadata unchanged.
    bool recordFreshHall(const game::OriginalUiHallRecords50KG&) noexcept;
    // Private pure preparation for the future full transaction. NOT a complete
    // storage-read receipt: selected slot/seen-card/caches intentionally remain
    // unchanged because their real preceding service observations are missing.
    // Identity comes ONLY from the retained actual full-read/projection join,
    // never an independently supplied envelope or synthetic seed.
    // Failure leaves out unchanged. No current live caller exists.
    bool preparePayloadMetadata(std::shared_ptr<const OriginalSaveNativeReadStage50KG>,
        std::unique_ptr<OriginalSaveProfileOwner50KG>& out) const noexcept;
    // Concrete provenance check for actual current Hall data, not a caller bit.
    bool hallRecordsCurrent(const game::OriginalUiHallRecords50KG&,
        const std::shared_ptr<const game::OriginalHallProfileIdentity50KG>&,
        std::uint64_t profileGeneration)const noexcept;
    // Private metadata preparation ONLY. No current caller installs this into
    // live state. Same-profile identity is checked by the concrete Rider owner;
    // generation/currentness and exact preimage are checked again here. Stage
    // success is NOT earned admission. No dirty/aural/save/rank writes occur.
    bool prepareHallRecordDescendant(const game::OriginalUiHallRecords50KG&,
        const game::OriginalRaceRecordInsertionStage50KG&,
        const std::shared_ptr<const game::OriginalHallProfileIdentity50KG>&,
        std::uint64_t,std::unique_ptr<OriginalSaveProfileOwner50KG>&)const noexcept;
    // Revalidate immediately BEFORE a future genuine owner store interval.
    // This checks data/provenance only, never the missing earned authority.
    // A future installer must swap ONLY hallRecordDescendant_, not restore the
    // copied card flags/names/other metadata from the preparation container.
    bool hallRecordPreparationMatches(const OriginalSaveProfileOwner50KG&,
        const game::OriginalUiHallRecords50KG&,
        const game::OriginalRaceRecordInsertionStage50KG&,
        const std::shared_ptr<const game::OriginalHallProfileIdentity50KG>&,
        std::uint64_t)const noexcept;
    bool sameHallRoot(const OriginalSaveProfileOwner50KG&)const noexcept;
    // BOUNDED audit backing: one latest13-course after-image, never a chain.
    // The existing original-load/native-beta root owners remain immutable.
    struct HallRecordDescendant {
        game::OriginalUiHallRecords50KG records;
        std::shared_ptr<const game::OriginalHallProfileIdentity50KG> profileIdentity;
        std::uint64_t epoch=0,generationBefore=0,generationAfter=0;
        HallOrigin origin=HallOrigin::none;
        const assets::OriginalUiMemberIdentity50KG* names=nullptr;
        const assets::OriginalUiMemberIdentity50KG* text=nullptr;
        std::shared_ptr<const game::OriginalSavePayloadEnvelope50KG> payload;
        std::shared_ptr<const OriginalSaveNativeReadStage50KG> read;
        std::shared_ptr<const game::OriginalUiHallRecords50KG> nativeBeta;
        std::shared_ptr<const game::OriginalUiHallRecords50KG> fresh;
    };
    std::shared_ptr<const HallRecordDescendant> hallRecordDescendant_;
    //23AE90: slot99,seen0;17E300/308: caches0. Each is source constructor state,
    // not a present/missing-card answer or result of a native directory probe.
    std::uint32_t selectedServiceSlot_=99,seenCard3C8_=0;
    std::uint32_t cache1914_=0,directoryCache1918_=0;
    //17E180..298 needs actual shared clock/RNG constructor lifecycle. Its native
    // join is not yet present: keep UNKNOWN, never insert zero/random default.
    std::optional<std::uint32_t> identity191C_;
    //17E2AC..2E4 clear only bits0/1; upper six are unproven and stay unknown.
    KnownFlags100BC cardFlags_{0,3};
    HallOrigin hallOrigin_=HallOrigin::none;
    const assets::OriginalUiMemberIdentity50KG* localizedNames_=nullptr;
    const assets::OriginalUiMemberIdentity50KG* localizedText_=nullptr;
    std::shared_ptr<const game::OriginalSavePayloadEnvelope50KG> payloadRecords_;
    std::shared_ptr<const OriginalSaveNativeReadStage50KG> importedRead_;
    std::shared_ptr<const game::OriginalUiHallRecords50KG> nativeBetaHall_;
    std::shared_ptr<const game::OriginalUiHallRecords50KG> freshHall_;
};
}
