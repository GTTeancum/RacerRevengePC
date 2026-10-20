// New reconstruction after rollback; source-only, unverified.
#pragma once
#include "original_ui_main_session50kg.h"
#include "../../../src/game/original_ui_course_selection50kg.h"
#include "../../../src/game/original_ui_rider_selection50kg.h"
#include "../../../src/game/original_ui_rider_stat_projection50kg.h"
#include "../../../src/game/original_ui_route_decisions50kg.h"
#include "../../../src/game/original_tournament_result50kg.h"
#include "../../../src/game/lap_timing50id.h"

namespace original_main_entry50KG { struct Application; }

namespace rr::platform {
class OriginalUiFrontend50KG;
class OriginalUiTournamentSession50KG;
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
    bool initialized() const noexcept;
    const OriginalUiMainProfile50KG* commonProfile() const noexcept;
    OriginalUiRiderProfileStamp50KG stamp() const noexcept;
    bool selectionSnapshot(game::OriginalUiRiderSelectionProfile50KG&) const noexcept;
    bool statSnapshot(std::uint32_t selectedRider,float panelY4DC,
        game::OriginalUiRiderStatInput50KG&) const noexcept;
    bool colors(std::uint32_t rider,OriginalUiRiderProfileColors50KG&) const noexcept;
    bool coursePlanetColors(std::uint32_t planet,OriginalUiCourseProfileColors50KG&)const noexcept;
    bool courseSnapshot(game::OriginalUiCourseRememberedProfile50KG&)const noexcept;
    struct CourseRaceSelection {
        std::uint32_t course=0,planet=0,rider=0,subtype=0,laps=0,entrants=0;
        std::uint16_t packed1125C=0;
    };
    bool courseRaceSnapshot(CourseRaceSelection&)const noexcept;
    bool riderConfirmSnapshot(std::uint8_t& packed1125D,std::uint32_t& comparison100C0)const noexcept;
    struct TournamentCareerSnapshot {
        std::uint32_t career100C0=0,earned2190=0,unspent2194=0,kills2198=0;
        std::uint8_t progress219C=0,availability11018=0;
        std::array<std::uint8_t,13> medals1938{};
        std::array<std::int16_t,13> remainingPrizes1946{};
        OriginalUiRiderProfileStamp50KG stamp{};
    };
    bool tournamentCareerSnapshot(std::uint32_t career,TournamentCareerSnapshot&)const noexcept;
    bool tournamentPodCareerSnapshot(std::uint32_t career,std::uint32_t pod,
        game::OriginalTournamentPodCareer50KG&)const noexcept;
private:
    friend struct ::original_main_entry50KG::Application;
    // Actual Application2DC owner only. Called at the future admitted race
    // constructor boundary, never on a page proposal or resource prewarm.
    // Represents only proven7E4 bit0; upper seven bits stay unrepresented.
    bool beginPodRace1A66CC(const OriginalUiMainProfile50KG& actualCommon)noexcept;
    bool initializeStartupProfile(std::shared_ptr<OriginalUiMainProfile50KG>,
        std::shared_ptr<const assets::OriginalUiArchive50KG>,std::uint32_t testRider,
        OriginalUiRiderProfileDiagnostic50KG&,bool gasganoSc=false,bool tzidikTc=false,bool scorchRc=false,bool sebulbaTb=false) noexcept;
    friend class OriginalUiTournamentSession50KG;
    friend class OriginalUiCourseSession50KG;
    bool commitRiderConfirmToCourse(const game::OriginalUiRouteDecision50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
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
