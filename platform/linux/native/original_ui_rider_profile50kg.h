// New reconstruction after rollback; source-only, unverified.
#pragma once
#include "original_ui_main_session50kg.h"
#include "../../../src/game/original_ui_course_selection50kg.h"
#include "../../../src/game/original_ui_rider_selection50kg.h"
#include "../../../src/game/original_ui_rider_stat_projection50kg.h"
#include "../../../src/game/original_ui_route_decisions50kg.h"

namespace rr::platform {
class OriginalUiFrontend50KG;
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
// This represents the Rider-visible diagonal of the original27x27 career/pod
// matrix, plus its reset metadata. No original save import, off-diagonal game
// progression, save IO, or profile reset popup is claimed. No public readiness
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
    bool initialized() const noexcept;
    const OriginalUiMainProfile50KG* commonProfile() const noexcept;
    OriginalUiRiderProfileStamp50KG stamp() const noexcept;
    bool selectionSnapshot(game::OriginalUiRiderSelectionProfile50KG&) const noexcept;
    bool statSnapshot(std::uint32_t selectedRider,float panelY4DC,
        game::OriginalUiRiderStatInput50KG&) const noexcept;
    bool colors(std::uint32_t rider,OriginalUiRiderProfileColors50KG&) const noexcept;
    bool coursePlanetColors(std::uint32_t planet,OriginalUiCourseProfileColors50KG&)const noexcept;
    bool courseSnapshot(game::OriginalUiCourseRememberedProfile50KG&)const noexcept;
    bool riderConfirmSnapshot(std::uint8_t& packed1125D,std::uint32_t& comparison100C0)const noexcept;
private:
    friend class OriginalUiCourseSession50KG;
    bool commitRiderConfirmToCourse(const game::OriginalUiRouteDecision50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    bool commitCourseEntry(const game::OriginalUiCourseSelectionData50KG&,const game::OriginalUiCourseEntryPrefix50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
    bool commitCourseMove(const game::OriginalUiCourseSelectionData50KG&,const game::OriginalUiNavigation50KG& before,
        bool forward,const game::OriginalUiCourseMove50KG&,OriginalUiRiderProfileStamp50KG)noexcept;
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
