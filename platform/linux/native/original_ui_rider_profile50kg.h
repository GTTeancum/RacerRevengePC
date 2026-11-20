// New reconstruction after rollback; source-only, unverified.
#pragma once
#include "original_persisted_profile_name50kg.h"
#include "native_beta_file50kg.h"
#include "original_ui_main_session50kg.h"
#include "../../../src/game/original_ui_course_selection50kg.h"
#include "../../../src/game/original_ui_course_mode2_entry50kg.h"
#include "../../../src/game/original_ui_rider_selection50kg.h"
#include "../../../src/game/original_ui_rider_stat_projection50kg.h"
#include "../../../src/game/original_ui_route_decisions50kg.h"
#include "../../../src/game/original_tournament_result50kg.h"
#include "../../../src/game/lap_timing50id.h"
#include "../../../src/game/original_ui_hall_records50kg.h"
#include "../../../src/game/original_race_record_insertion50kg.h"
#include "../../../src/game/original_race_record_qualification50kg.h"
#include "../../../src/game/original_race_rival_qualification50kg.h"
#include "../../../src/game/original_unlock_checks50pd.h"
#include "../../../src/game/original_race_ack_route50pd.h"

namespace original_results_roster50LA { class Read; }
namespace original_main_entry50KG { struct Application; }
namespace original_racer_manager_entity { class Owner; }
namespace ra_cold_aggregate50LA { class Owner; }
namespace rr::game { struct OriginalUiGarageState50KG; struct ControlPresets49D; struct OriginalUiGameSettingsProfile50KG; class OriginalResults10450LA; class OriginalRaceConstructor50LA; struct LapRecordQueryResult50ID; }

namespace rr::platform {
class OriginalUiFrontend50KG;
class OriginalUiMainAudio50KG;
class OriginalSoundMode50KG;
class OriginalSaveNativeReadStage50KG;
class OriginalSaveProfileOwner50KG;
class OriginalUiTournamentSession50KG;
class OriginalUiHallOfFameSession50KG;
struct OriginalUiRiderProfileDiagnostic50KG {
    enum class Error:std::uint8_t {none,alreadyInitialized,commonProfile,archive,member,parse,range,allocation} error=Error::none;
    std::uint32_t sourcePC=0,rider=0;
    assets::OriginalUiPageDiagnostic50KG parse{};
};
struct OriginalUiRiderProfileColors50KG {
    std::uint32_t accent17FA00=0,secondary17FA10=0;
};
struct OriginalUiCourseProfileColors50KG {
    std::uint32_t primary79C=0,secondary7A0=0;
};
struct OriginalUiRiderProfileStamp50KG {
    std::uint64_t generation=0;
    std::uint32_t mode34=0;
};
// Actual application-lifetime fresh-profile subset. Construct once alongside
// the application's sole Main/Options profiles, BEFORE opening any UI page.
// 17F970 orders1870B0 authored abilities,17FFB0 career reset,1864B0 settings /
// 186EF0 unlock reset / remembered fields. The Main subset is shared by identity,
// so Main's later mode34 changes are observed directly, never copied stale.
//
// This retains the original27x27 career/pod matrix and its source result
// writeback. The private career reset owns only the profile mutation, never
// shared-popup admission, result custody, original save IO or persistence.
// A result commit remains private and requires a genuinely armed tournament
// launch; current missing tournament lifecycle guards stay closed. No public readiness
// flag or arbitrary observation setter can manufacture an initialized owner.
class OriginalUiRiderProfileOwner50KG final {
public:
    OriginalUiRiderProfileOwner50KG() noexcept;
    ~OriginalUiRiderProfileOwner50KG();
    OriginalUiRiderProfileOwner50KG(const OriginalUiRiderProfileOwner50KG&)=delete;
    OriginalUiRiderProfileOwner50KG& operator=(const OriginalUiRiderProfileOwner50KG&)=delete;
    // Explicit NEW-profile application operation, never a missing-save fallback
    // or page-entry reset. Both inputs must already be actual retained owners.
    // Failure preserves this uninitialized owner and both supplied owners.
    bool initializeAtApplicationStartup(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG> podPhysics,
        OriginalUiRiderProfileDiagnostic50KG&) noexcept;
    // Explicit process-local capture fixture, NOT186EF0 or an earned award.
    // Only rider7 availability and mode0 remembered selection differ. No saved
    // profile, career result, progression, or arbitrary rider setter exists.
    bool initializeGasganoTestProfileAtApplicationStartup(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG> podPhysics,
        OriginalUiRiderProfileDiagnostic50KG&) noexcept;
    // Actual persisted names, not racer localization IDs3C. Copies are owned
    // and survive later profile changes. Failure preserves out.
    bool careerName219D(std::uint32_t career,OriginalPersistedProfileName50KG& out)const noexcept;
    bool playerName10F68(std::uint32_t player,OriginalPersistedProfileName50KG& out)const noexcept;
    bool gasganoTestProfile() const noexcept;
    // Explicit native test override, not the retail cheat or earned progression.
    // Only visible, nonfresh riders with existing native Course owners:3/8/22.
    // Changes one startup unlock and mode0 remembered rider; no track/save changes.
    bool initializeVisibleRiderTestProfileAtApplicationStartup(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG> podPhysics,std::uint32_t rider,
        OriginalUiRiderProfileDiagnostic50KG&) noexcept;
    std::uint32_t visibleRiderTestProfile() const noexcept;
    // Explicit process-local Mawhonic/Brightlands fixture: rider9 available,
    // mode0 remembers9, and original course prefix availability is5. No earned
    // medal/progression or saved state is manufactured; RA runtime stays gated.
    bool initializeMawhonicRylothTestProfileAtApplicationStartup(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG> podPhysics,
        OriginalUiRiderProfileDiagnostic50KG&) noexcept;
    bool mawhonicRylothTestProfile() const noexcept;
    // Explicit process-local Wan16/MB test profile; no earned progression or save IO.
    // This profile does not grant native racer/course capability.
    bool initializeWanMbTestProfileAtApplicationStartup(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,OriginalUiRiderProfileDiagnostic50KG&) noexcept;
    bool wanMbTestProfile() const noexcept;
    // Separate process-local Gasgano7/SC provenance; never the legacy Gasgano
    // unattended SA fixture. Prefix9/remembered9 are not race capability.
    bool initializeGasganoScTestProfileAtApplicationStartup(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,OriginalUiRiderProfileDiagnostic50KG&) noexcept;
    bool gasganoScTestProfile() const noexcept;
    // Distinct process-local Tzidik24/TC provenance, not gameplay readiness.
    bool initializeTzidikTcTestProfileAtApplicationStartup(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,OriginalUiRiderProfileDiagnostic50KG&) noexcept;
    bool tzidikTcTestProfile() const noexcept;
    // Separate Scorch17/RC test provenance, not runtime course readiness.
    bool initializeScorchRcTestProfileAtApplicationStartup(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,OriginalUiRiderProfileDiagnostic50KG&) noexcept;
    bool scorchRcTestProfile() const noexcept;
    // Separate Sebulba18/TB process-local provenance; no runtime readiness.
    bool initializeSebulbaTbTestProfileAtApplicationStartup(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,OriginalUiRiderProfileDiagnostic50KG&) noexcept;
    bool sebulbaTbTestProfile() const noexcept;
    bool initializeKraidGbTestProfileAtApplicationStartup(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,OriginalUiRiderProfileDiagnostic50KG&) noexcept;
    bool kraidGbTestProfile() const noexcept;
    bool initializeYoungAnakinMcTestProfileAtApplicationStartup(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,OriginalUiRiderProfileDiagnostic50KG&,
        std::uint32_t mcRider=1) noexcept;
    // Native beta progress is a separate versioned user file, never a PS2 card.
    // Load is one-shot at application startup, before any frontend owns snapshots.
    enum class BetaPersistenceStatus {notStarted,ready,loaded,saved,disabledFixture,
        unavailable,locked,invalid,readError,writeError,durabilityUnknown};
    BetaPersistenceStatus betaPersistenceStatus() const noexcept;
    bool saveNativeBetaProgress() noexcept;
    // PORT STAND-IN (DLC decision D7 b): a free-play race ends on the native
    // Results panel, before the retail end dispatch, so no 1AB5BC / 1AC440 /
    // save moment runs in that flow for any pod. For a latched free-play
    // add-on run only, this applies 1ABFC0 against the merged Hall (stock +
    // DLC rows), submits a qualifying record to the DLC side file and writes
    // the side file at once (one dlc_record line either way). Any other run:
    // false, nothing read, changed or logged. Never writes the stock save.
    bool commitAddonFreePlayRecordAtNativeResults(
        const game::OriginalRaceRecordQualificationRacerObservation50KG& row0) noexcept;
    bool reloadNativeBetaProgressFromMenu() noexcept;
    void bindNativeBetaAutosave(std::shared_ptr<const game::OriginalUiGameSettingsProfile50KG>) noexcept;
    bool youngAnakinMcTestProfile() const noexcept;
    // Source17DE80 loads actual localized names before1801D0. One explicit
    // startup population, never a menu-entry reset or missing-save fallback.
    bool initializeHallRecordsAtApplicationStartup(const assets::OriginalUiEnglish50KG&,
        OriginalUiRiderProfileDiagnostic50KG&) noexcept;
    bool hallRecordsReady() const noexcept;
    // Typed browse-mode views do not widen the existing racer mode0..2 gate.
    bool hallSnapshot(game::OriginalUiHallProfile50KG&) const noexcept;
    bool hallRecordSnapshot(std::uint32_t course,game::OriginalUiHallCourseRecords50KG&) const noexcept;
    bool hallPlanetColors(std::uint32_t planet,OriginalUiCourseProfileColors50KG&) const noexcept;
    bool initialized() const noexcept;
    const OriginalUiMainProfile50KG* commonProfile() const noexcept;
    OriginalUiRiderProfileStamp50KG stamp() const noexcept;
    bool selectionSnapshot(game::OriginalUiRiderSelectionProfile50KG&) const noexcept;
    struct RiderSelectionFacts {
        std::uint32_t rider=0,unlock10FAC=0,hidden730=0;
    };
    // Read actual retained profile facts for an authored rider. Hidden is
    // informational: original Confirm tests unlock10FAC, not hidden730.
    // These facts do not certify any native model, renderer or race owner.
    bool riderSelectionFacts(std::uint32_t rider,RiderSelectionFacts&) const noexcept;
    // Native mode0 route-continuity guard, not an added original Course
    // instruction: packed, remembered and comparison identity must still
    // agree with the unlocked Rider selection committed by the source route.
    bool courseRiderSelectionSnapshot(RiderSelectionFacts&) const noexcept;
    bool statSnapshot(std::uint32_t selectedRider,float panelY4DC,
        game::OriginalUiRiderStatInput50KG&) const noexcept;
    bool colors(std::uint32_t rider,OriginalUiRiderProfileColors50KG&) const noexcept;
    bool coursePlanetColors(std::uint32_t planet,OriginalUiCourseProfileColors50KG&)const noexcept;
    bool courseSnapshot(game::OriginalUiCourseRememberedProfile50KG&)const noexcept;
    struct CourseRaceSelection {
        std::uint32_t course=0,planet=0,rider=0,subtype=0,laps=0,entrants=0;
        std::uint16_t packed1125C=0;
        // Original frontend mode, independent of the native loader mode.
        std::uint32_t mode34=0;
    };
    bool courseRaceSnapshot(CourseRaceSelection&)const noexcept;
    bool riderConfirmSnapshot(std::uint8_t& packed1125D,std::uint32_t& comparison100C0)const noexcept;
    struct TournamentCareerSnapshot {
        std::uint32_t career100C0=0,earned2190=0,unspent2194=0,kills2198=0;
        std::uint8_t progress219C=0,availability11018=0;
        // Actual profile100BC bit0, tested by Tournament entry1EEA64/68.
        // Observation only: a clean value never certifies popup ownership.
        bool cardFlag100BC=false;
        bool nativeBetaStorage=false; // Native save UI, not original memory-card popup.
        std::array<std::uint8_t,13> medals1938{};
        std::array<std::int16_t,13> remainingPrizes1946{};
        OriginalUiRiderProfileStamp50KG stamp{};
    };
    bool tournamentCareerSnapshot(std::uint32_t career,TournamentCareerSnapshot&)const noexcept;
    struct GarageProfileSnapshot {
        TournamentCareerSnapshot career{};
        std::array<float,6> base0180{},maximum0408{},current1920{};
        OriginalUiRiderProfileColors50KG colors{};
    };
    // Coherent read of the selected career's actual diagonal ability row and
    // original authored bounds. No upgrade, cash, dirty-bit or save mutation.
    // This native observation is not Garage resource/route admission.
    bool selectedGarageProfileSnapshot(GarageProfileSnapshot&)const noexcept;
    // Coherent read of the actual selected career and Course lanes. No caller
    // supplied career/progress or independent snapshots confer selection.
    bool selectedTournamentCourseSnapshot(TournamentCareerSnapshot&,
        game::OriginalUiCourseRememberedProfile50KG&) const noexcept;
    struct TournamentCourseRecordSnapshot {
        std::uint32_t slot=0,course100C4=0;
        std::int32_t constructorPurse100C8=0,par100E8=0;
        std::array<std::int32_t,7> killPercent100CC{};
        OriginalUiRiderProfileStamp50KG stamp{};
    };
    // Actual next course uses the retained original order and constructor prizes.
    // par100E8=-1 means unrecovered PAR; renderer must omit that annotation.
    // Actual remaining purse remains in the selected career/pod snapshot.
    bool selectedTournamentCourseRecordSnapshot(TournamentCourseRecordSnapshot&) const noexcept;
    bool tournamentPodCareerSnapshot(std::uint32_t career,std::uint32_t pod,
        game::OriginalTournamentPodCareer50KG&)const noexcept;
    struct TournamentTextMetadataSnapshot {
        std::uint32_t career100C0=0;
        std::int32_t racerName3C=-1,courseTitle1127C=-1;
        std::int8_t planetTitle11281=-1; // Source1EF3F0 uses signed LB.
        std::uint8_t progressIndex=0,course100C4=0;
        OriginalUiRiderProfileStamp50KG stamp{};
    };
    // Observes retained constructor localization IDs and actual course order,
    // not caller-supplied labels. Progress13 preserves its completed identity
    // while the source-local course-order index clamps to12 (1EF230..244).
    // The standings owner selects the separate completed localization branch.
    bool tournamentTextMetadataSnapshot(std::uint32_t career,
        TournamentTextMetadataSnapshot&) const noexcept;
    bool armTournamentResultForCommittedRace(const CourseRaceSelection&,OriginalUiRiderProfileStamp50KG,
        const game::LapTimingManager50ID&,OriginalUiMainApplication50KG&)noexcept;
    bool commitTournamentRaceResult(const game::LapTimingManager50ID&,
        const game::OriginalTournamentResultRacer50KG*,std::size_t,std::uint32_t humanCount,
        const CourseRaceSelection&,OriginalUiRiderProfileStamp50KG,
        OriginalUiMainApplication50KG&,game::OriginalTournamentResultReceipt50KG&)noexcept;
    // Restart/leave must call this before destroying its real timing manager.
    void cancelTournamentResultForRace(const game::LapTimingManager50ID&) noexcept;
    // Read-only facts for 187E30 and its grants: unlock words, 7E8, the
    // selected course rival, diagonal medals, Hall defaults (7EC) and live
    // records (102F0), gallery tier. Read after the 1875C0/187D80 commit.
    bool unlockFacts187E30(game::OriginalUnlockFacts50PD&)const noexcept;
    // 209C2C..209CC4: rivals of course orders 4..12 (signed byte
    // P+11280+12*byte(P+100C4+40*o)) then pods 3,1,14,25,21,26. Bit p = pod p.
    bool unlockSpritePods209BE0(std::uint32_t& mask)const noexcept;
    // Read-only facts for the 1AB5BC destination rows (mode34, 187B10 order,
    // 102CC, 7E4 bit0, M[C][C][12], 187E30). No store, route or save.
    bool raceAcknowledgementFacts1AB5BC(game::OriginalRaceAckFacts50PD&)const noexcept;
private:
    friend class ::ra_cold_aggregate50LA::Owner;
    //Actual1B44E8 synchronous byte read from THIS profile's same common owner.
    //No snapshot/route readiness, mutation, reset or caller-provided source bits.
    bool readWorldProfileByte1B44E8(const OriginalUiMainProfile50KG&,std::uint8_t&)const noexcept;
    //Fresh caller occurrence; shares storage/identity mechanics, not an earlier read receipt.
    bool readWorldProfileByte1AA2F4(const OriginalUiMainProfile50KG&,std::uint8_t&)const noexcept;
    //Exact fresh course byte at1AA354; does not read unrelated snapshot lanes.
    bool readWorldCourseByte1AA354(const OriginalUiMainProfile50KG&,std::uint8_t&)const noexcept;
    bool readSharedWorldProfileByteNative(const OriginalUiMainProfile50KG&,std::uint8_t&)const noexcept;
    friend class rr::game::OriginalResults10450LA;
    friend class ::original_results_roster50LA::Read;
    // Reached DONE strcpy only. Preserves unknown suffix; no dirty/save/ack.
    bool storeResultsName1CA528(std::uint32_t resultsMode,std::string_view,
        OriginalUiRiderProfileStamp50KG expected)noexcept;
    // Reached literal NO TIME branch only: actual11054 bit0, no save/award.
    bool storeResultsNoTime1CA4DC(OriginalUiRiderProfileStamp50KG expected)noexcept;
    // Reached only after Results1C2148 read actual mode34 as zero.
    bool resultsSubtype11258(std::uint32_t&)const noexcept;
    bool resultsRecordBit7E4(bool&)const noexcept;
    friend class rr::game::OriginalRaceConstructor50LA;
    // Actual live profile3C[vehicle60] read for Results1C3050..3064. No
    // tournament/selected-career proxy and no caller-provided localization ID.
    bool resultsRacerName3C(std::uint32_t,std::int32_t&)const noexcept;
    //187330 reads the ACTUAL constructor-owned11278 descriptor selected by
    //packed1125F low4. Private synchronous observation, not load admission.
    bool selectedTrackName187330(std::array<char,3>&)const noexcept;
    // Private immutable metadata/read snapshot for the actual Results owner.
    // Selected1125F search187B10, NOT progress219C/next-course selection.
    // No supplied rank/count, award mutation or career advancement authority.
    struct ResultsCareerPageFields {
        std::uint32_t order=0;
        std::uint8_t priorMedal1938=0;
        std::int16_t remaining1946=0;
        std::int32_t title1127C=-1,par100E8=0;
        std::array<std::int32_t,8> placePercent102D0{},killPercent100CC{};
        OriginalUiRiderProfileStamp50KG stamp{};
    };
    bool resultsCareerPageFields1C4CD8(ResultsCareerPageFields&)const noexcept;
    // Reached readonly leaves for the career message body, not next-course
    // menu projections or permission to advance/commit the career.
    bool resultsCareerProgress219C(std::uint8_t&)const noexcept;
    bool resultsSelectedOrder187B10(std::uint32_t&)const noexcept;
    bool resultsNextCourseTitle187480(std::int32_t&)const noexcept;
    // Concrete1C9B10 reads of THIS retained profile; no public row/Hall input.
    bool resultsNameCourse1C9C80(std::uint32_t&)const noexcept;
    bool resultsFirstName1C9CA4(OriginalPersistedProfileName50KG&)const noexcept;
    game::LapRecordQueryResult50ID resultsLapQuery1861A0(std::uint32_t,float)const noexcept;
    game::LapRecordQueryResult50ID resultsRaceQuery186280(std::uint32_t,float)const noexcept;
    game::LapRecordQueryResult50ID resultsKnockoutQuery1863B0(std::uint32_t,std::uint32_t)const noexcept;
    friend struct ::original_main_entry50KG::Application;
    friend class ::original_racer_manager_entity::Owner;
    // Pure, private preparation from the SAME profile's Hall/config/names.
    // No current caller. Row0/race1B0 are observations, not authority: a future
    // once-only1AB520 continuation must establish the actual retained manager,
    // completed timing and earned-CD0 custody before calling, then preserve
    // original mutation/dirty/aural/Hall order. This method never commits.
    bool prepareSingleHumanRecordInsertion1AC440(
        const game::OriginalRaceRecordRacerObservation50KG& row0,std::uint8_t race1B0,
        std::optional<game::OriginalRaceRecordInsertionStage50KG>&,
        game::OriginalRaceRecordInsertionStage50KG::Diagnostic&)const noexcept;
    //Same preparation-only boundary for actual manager rows0,1, in that order.
    //No multiplayer launch/result admission is created by accepting two values.
    bool prepareTwoHumanRecordInsertion1AC440(
        const std::array<game::OriginalRaceRecordRacerObservation50KG,2>& rows,
        std::uint8_t race1B0,std::optional<game::OriginalRaceRecordInsertionStage50KG>&,
        game::OriginalRaceRecordInsertionStage50KG::Diagnostic&)const noexcept;
    //Readonly1ABFC0 preparation only. Its private1AB520 caller must use
    //the actual synchronous same-Race manager observation join,
    //not public snapshots or a copied display row as earned-result authority.
    bool prepareRaceRecordQualification1ABFC0(std::uint32_t selectorA1,std::optional<std::uint8_t> race1B0Low2,
        const game::OriginalRaceRecordQualificationRacerObservation50KG* rows,
        std::size_t rowCount,std::optional<game::OriginalRaceRecordQualification50KG>&,
        game::OriginalRaceRecordQualification50KG::Diagnostic&)const noexcept;
    //Readonly1ABE70 using this exact Profile's selected descriptor/unlocks.
    //No7E8 write here. The private1AB520 continuation preserves preceding
    //1ABFC0 ->7E4 even if this later operation cannot run; no live trigger.
    bool prepareRaceRivalQualification1ABE70(
        const game::OriginalRaceRivalRacerObservation50KG*,std::size_t availableRows,
        std::int32_t actualManagerCount30,std::optional<game::OriginalRaceRivalQualification50KG>&,
        game::OriginalRaceRivalQualification50KG::Diagnostic&)const noexcept;
    //Only the actual Race's private source continuation calls these immediate
    //query-result stores. They confer no earned credit/Hall/save authority.
    //Each checks same Profile preimage generation and returns its resulting
    //native mutation generation (unchanged stores do not bump it), so the
    //suspended caller cannot replay or reject itself.
    bool storeEndRecordBit1AB598(const game::OriginalRaceRecordQualification50KG&,
        OriginalUiRiderProfileStamp50KG,OriginalUiRiderProfileStamp50KG&)noexcept;
    bool storeEndRivalWord1AB5A8(const game::OriginalRaceRivalQualification50KG&,
        OriginalUiRiderProfileStamp50KG,OriginalUiRiderProfileStamp50KG&)noexcept;
    // Bounded Hall ancestry prerequisite. Never installs a row or marks it
    // earned. No live caller exists; the genuine1AB520 mutation admission is
    // still required before any future row/rank/provenance store interval.
    bool prepareHallRecordProvenance1AC440(const game::OriginalRaceRecordInsertionStage50KG&,
        std::unique_ptr<OriginalSaveProfileOwner50KG>&)const noexcept;
    bool hallRecordProvenancePreparationCurrent1AC440(
        const game::OriginalRaceRecordInsertionStage50KG&,
        const OriginalSaveProfileOwner50KG&)const noexcept;
    bool recordInsertionIdentityCurrent(const game::OriginalRaceRecordInsertionStage50KG&)const noexcept;
    // Uncalled private store leaf, NOT admission. Future genuine same-Race
    // Results/end continuation must establish earned owner/once-only custody
    // before calling. Native failure-atomic preflight; no dirty/save/route.
    // The prepared container retains the displaced descendant for release
    // AFTER the nonfailing store interval; no copied unrelated state installs.
    bool commitSingleHumanHallRecords1AC440(const game::OriginalRaceRecordInsertionStage50KG&,
        OriginalSaveProfileOwner50KG& prepared,OriginalUiRiderProfileStamp50KG expected,
        OriginalUiRiderProfileStamp50KG& after)noexcept;
    // 1AB5BC record row (1AB680 / 1AB838): the same single-human preparation,
    // provenance and store as above in one synchronous call, for callers that
    // cannot own the prepared save container. row0 must be this race's own
    // manager row0 read immediately before; failure changes nothing.
    bool commitSingleHumanRaceRecords1AC440(const game::OriginalRaceRecordRacerObservation50KG& row0,
        std::uint8_t race1B0,OriginalUiRiderProfileStamp50KG expected,OriginalUiRiderProfileStamp50KG& after,
        game::OriginalRaceRecordInsertionStage50KG::Diagnostic&)noexcept;
    // Actual Results1CA6E4 bit store, not persistence or a save acknowledgement.
    bool markResultsHallDirty1CA6E4(OriginalUiRiderProfileStamp50KG expected)noexcept;
    bool commitRacerManager1AD520(const CourseRaceSelection& expected,CourseRaceSelection& actual)noexcept;
    //Private current1ADF80 Profile2DC read. No snapshot/selected-ID receipt.
    //Human0 reads1125D>>3;human1 reads1125E&31;others are uninitialized
    //in original s1 and must not receive a fabricated value.
    bool readHumanPodSelection1ADFF4(const OriginalUiMainProfile50KG& actualCommon,
        std::uint32_t humanIndex,std::uint32_t& selected)const noexcept;
    // Actual Application2DC owner only. Called at the future admitted race
    // constructor boundary, never on a page proposal or resource prewarm.
    // Represents only proven7E4 bit0; upper seven bits stay unrepresented.
    bool beginPodRace1A66CC(const OriginalUiMainProfile50KG& actualCommon)noexcept;
    bool initializeStartupProfile(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,std::uint32_t testRider,
        OriginalUiRiderProfileDiagnostic50KG&,bool gasganoSc=false,bool tzidikTc=false,bool scorchRc=false,bool sebulbaTb=false,bool kraidGb=false,bool youngAnakinMc=false,std::uint32_t mcRider=1) noexcept;
    friend class OriginalUiUnlockSession50PD;
    friend class OriginalUiWinSession50PD;
    // 20B480: grant ONE check (187F40/187FF0/188050/1880D0/188150/1881D0/
    // 1882B0, or 188440(P,N) for the gallery). Exact preimage stamp only.
    bool grantUnlock20B480(std::size_t check,std::uint32_t galleryTier,
        OriginalUiRiderProfileStamp50KG expected,OriginalUiRiderProfileStamp50KG& after)noexcept;
    // 1AB878/1AB8DC: non-tournament acknowledgement 100BC bit0 store. The
    // tournament row's 1AB648 store is part of commitTournamentRaceResult.
    bool markAcknowledgementSavePending1AB648(OriginalUiRiderProfileStamp50KG expected,
        OriginalUiRiderProfileStamp50KG& after)noexcept;
    friend class OriginalUiHallOfFameSession50KG;
    bool hallSnapshotForMode(std::uint32_t prospectiveMode,game::OriginalUiHallProfile50KG&) const noexcept;
    bool commitHallPlanet(std::uint32_t planet,OriginalUiRiderProfileStamp50KG) noexcept;
    bool commitHallBack(std::uint16_t selectedSlot,const game::OriginalUiRouteDecision50KG&,
        OriginalUiRiderProfileStamp50KG) noexcept;
    bool commitHallRender(OriginalUiRiderProfileStamp50KG) noexcept;
    // Hall page reads in its own live mode: mode5 (Options) or mode2 (the
    // tournament race-return Hall, entered by 1AB5BC's 17DE10(6,11,1) with
    // no mode write). Mode5 results equal hallRecordSnapshot/hallPlanetColors.
    bool hallBrowseRecordSnapshot(std::uint32_t course,game::OriginalUiHallCourseRecords50KG&) const noexcept;
    bool hallBrowsePlanetColors(std::uint32_t planet,OriginalUiCourseProfileColors50KG&) const noexcept;
    friend class OriginalUiTournamentSession50KG;
    friend class OriginalUiCourseSession50KG;
    bool commitRiderConfirmToCourse(const game::OriginalUiRouteDecision50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    friend class OriginalUiTournamentCareerResources50KG;
    // Same exact selection transaction validation, read-only before admission.
    // This observes original authored/unlock facts, never race readiness.
    bool canPrepareRiderConfirmToTournament(const game::OriginalUiRouteDecision50KG&,OriginalUiRiderProfileStamp50KG)const noexcept;
    bool canCommitRiderConfirmToTournament(const game::OriginalUiRouteDecision50KG&,OriginalUiRiderProfileStamp50KG)const noexcept;
    bool commitRiderConfirmToTournament(const game::OriginalUiRouteDecision50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    // Concrete frontend only: call AFTER an admitted genuine mode2 cold race
    // launch commits its exact original selected profile fields and the SAME
    // real manager reaches active bit1 (before ended bit0). Arming never
    // unlocks/mutates progress. Existing mode0-only launch cannot arm this.
    bool commitCourseEntry(const game::OriginalUiCourseSelectionData50KG&,const game::OriginalUiCourseEntryPrefix50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    bool commitCourseMode2Entry(const game::OriginalUiCourseSelectionData50KG&,
        const game::OriginalUiCourseMode2EntryPlan50KG&,OriginalUiRiderProfileStamp50KG) noexcept;
    bool commitCourseMove(const game::OriginalUiCourseSelectionData50KG&,const game::OriginalUiNavigation50KG& before,
        bool forward,const game::OriginalUiCourseMove50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    bool commitCourseLaunch(const game::OriginalUiRouteDecision50KG&,const game::OriginalUiCourseContext50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    bool commitCourseBack(const game::OriginalUiRouteDecision50KG&,const game::OriginalUiCourseContext50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    friend class OriginalUiGaragePageOwner50KG;
    // Private retained Garage confirmation only; no public numeric mutation API.
    bool canApplyGaragePurchase1FEB90(const GarageProfileSnapshot&,
        const game::OriginalUiGarageState50KG&) const noexcept;
    bool applyGaragePurchase1FEB90(const GarageProfileSnapshot&,
        const game::OriginalUiGarageState50KG&,GarageProfileSnapshot&) noexcept;
    void autosaveGaragePurchase() noexcept;
    friend class OriginalUiFrontend50KG;
    friend class OriginalUiRiderSession50KG;
    // Genuine Rider1EAEC8/result2 caller only. The session must retain the
    // actual selected4C4 and popup result through this synchronous commit;
    // numeric selection/stamp observations alone confer no mutation authority.
    // Preflight is read-only. Commit stages all values before a nonfailing
    // store interval; failure preserves the profile, name and generation.
    // No selected-race fields, unlocks, dirty bit, save IO or autosave changes.
    bool canResetRiderCareer1800E0(std::uint32_t selectedRider,
        OriginalUiRiderProfileStamp50KG expected) const noexcept;
    bool resetRiderCareer1800E0(std::uint32_t selectedRider,
        OriginalUiRiderProfileStamp50KG expected) noexcept;
    // Read-only cold resource planning from a genuine Main proposal. Main's
    // live mode4 is preserved; only this returned snapshot uses the proposed
    // mode. Actual Rider entry and every profile commit still read live mode.
    bool selectionSnapshotForMode(std::uint32_t prospectiveMode,
        game::OriginalUiRiderSelectionProfile50KG&) const noexcept;
    // One all-owner nonfailing store interval, called only after the actual
    // Frontend/Options exclusion. Prepared ownership receives displaced shared
    // ancestry and must outlive that exclusion. No separately callable stores.
    bool installNativeReadOwners(const std::shared_ptr<const OriginalSaveNativeReadStage50KG>&,
        OriginalSaveProfileOwner50KG& prepared,OriginalUiMainProfile50KG&,
        OriginalUiRiderProfileStamp50KG,game::OriginalUiGameSettingsProfile50KG&,
        game::ControlPresets49D&,OriginalUiMainAudio50KG&,OriginalSoundMode50KG&)noexcept;
    // Existing-owner facet for the future private load coordinator. It uses
    // THIS sole Rider bank/common pointer and generation, not supplied careers.
    // No mutation, storage IO, live import or completed-load capability.
    bool prepareNativeReadMetadata(const std::shared_ptr<const OriginalSaveNativeReadStage50KG>&,
        const OriginalUiMainProfile50KG& actualCommon,OriginalUiRiderProfileStamp50KG,
        std::unique_ptr<OriginalSaveProfileOwner50KG>& out)const noexcept;
    // Concrete frontend commit boundary only. Public snapshots/stamps cannot
    // authorize mutation. No public adapter may forward into this method.
    // Only exact already-admitted Back decisions: one remembered-rider write,
    // page1 -> Main4/1 or page2 -> Rider5/1. Caller still owns current source
    // selection/lifecycle validation and manager admission. No partial mutation
    // on rejection; expected stamp must be from that same source snapshot.
    bool commitBack(const game::OriginalUiRouteDecision50KG&,
        OriginalUiRiderProfileStamp50KG expected) noexcept;
    void loadNativeBetaProgressAtStartup() noexcept;
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
