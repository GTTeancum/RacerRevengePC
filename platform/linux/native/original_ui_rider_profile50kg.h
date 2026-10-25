// New reconstruction after rollback; source-only, unverified.
#pragma once
#include "original_persisted_profile_name50kg.h"
#include "original_ui_main_session50kg.h"
#include "../../../src/game/original_ui_course_selection50kg.h"
#include "../../../src/game/original_ui_course_mode2_entry50kg.h"
#include "../../../src/game/original_ui_rider_selection50kg.h"
#include "../../../src/game/original_ui_rider_stat_projection50kg.h"
#include "../../../src/game/original_ui_route_decisions50kg.h"
#include "../../../src/game/original_tournament_result50kg.h"
#include "../../../src/game/lap_timing50id.h"
#include "../../../src/game/original_ui_hall_records50kg.h"

namespace original_main_entry50KG { struct Application; }
namespace original_racer_manager_entity { class Owner; }
namespace rr::game { struct ControlPresets49D; struct OriginalUiGameSettingsProfile50KG; }

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
// writeback. No original save import, save IO, or profile reset popup is claimed.
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
    // Only the authenticated first constructor row is represented here.
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
    // not caller-supplied labels. Only the uncompleted0..12 next-course branch
    // is represented; progress13 has its own source text path and rejects.
    bool tournamentTextMetadataSnapshot(std::uint32_t career,
        TournamentTextMetadataSnapshot&) const noexcept;
private:
    friend struct ::original_main_entry50KG::Application;
    friend class ::original_racer_manager_entity::Owner;
    bool commitRacerManager1AD520(const CourseRaceSelection& expected,CourseRaceSelection& actual)noexcept;
    // Actual Application2DC owner only. Called at the future admitted race
    // constructor boundary, never on a page proposal or resource prewarm.
    // Represents only proven7E4 bit0; upper seven bits stay unrepresented.
    bool beginPodRace1A66CC(const OriginalUiMainProfile50KG& actualCommon)noexcept;
    bool initializeStartupProfile(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,std::uint32_t testRider,
        OriginalUiRiderProfileDiagnostic50KG&,bool gasganoSc=false,bool tzidikTc=false,bool scorchRc=false,bool sebulbaTb=false,bool kraidGb=false,bool youngAnakinMc=false,std::uint32_t mcRider=1) noexcept;
    friend class OriginalUiHallOfFameSession50KG;
    bool hallSnapshotForMode(std::uint32_t prospectiveMode,game::OriginalUiHallProfile50KG&) const noexcept;
    bool commitHallPlanet(std::uint32_t planet,OriginalUiRiderProfileStamp50KG) noexcept;
    bool commitHallBack(std::uint16_t selectedSlot,const game::OriginalUiRouteDecision50KG&,
        OriginalUiRiderProfileStamp50KG) noexcept;
    bool commitHallRender(OriginalUiRiderProfileStamp50KG) noexcept;
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
    bool armTournamentResultForCommittedRace(const CourseRaceSelection&,OriginalUiRiderProfileStamp50KG,
        const game::LapTimingManager50ID&,OriginalUiMainApplication50KG&)noexcept;
    bool commitTournamentRaceResult(const game::LapTimingManager50ID&,
        const game::OriginalTournamentResultRacer50KG*,std::size_t,std::uint32_t humanCount,
        const CourseRaceSelection&,OriginalUiRiderProfileStamp50KG,
        OriginalUiMainApplication50KG&,game::OriginalTournamentResultReceipt50KG&)noexcept;
    bool commitCourseEntry(const game::OriginalUiCourseSelectionData50KG&,const game::OriginalUiCourseEntryPrefix50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    bool commitCourseMode2Entry(const game::OriginalUiCourseSelectionData50KG&,
        const game::OriginalUiCourseMode2EntryPlan50KG&,OriginalUiRiderProfileStamp50KG) noexcept;
    bool commitCourseMove(const game::OriginalUiCourseSelectionData50KG&,const game::OriginalUiNavigation50KG& before,
        bool forward,const game::OriginalUiCourseMove50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    bool commitCourseLaunch(const game::OriginalUiRouteDecision50KG&,const game::OriginalUiCourseContext50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    bool commitCourseBack(const game::OriginalUiRouteDecision50KG&,const game::OriginalUiCourseContext50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    friend class OriginalUiFrontend50KG;
    friend class OriginalUiRiderSession50KG;
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
    struct Storage;std::unique_ptr<Storage> storage_;
};
}
