#pragma once
#include "original_ui_navigation50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Original input-event IDs, not host key codes. These leaves deliberately do
// not consume directional events: the corresponding page/navigation owner
// must handle them once, with its actual authored selection data.
enum class OriginalUiRouteInput50KG : std::uint8_t { activate=4, back=6 };
enum class OriginalUiRouteStatus50KG : std::uint8_t {
    ok, noChange, invalidPage, invalidGroup, invalidSlot, invalidController,
    invalidRider, invalidCourse, unsupportedInput, unavailable
};
enum class OriginalUiRouteKind50KG : std::uint8_t {
    none, changeScreen, changePage, courseLaunch
};
enum class OriginalUiRouteAction50KG : std::uint8_t {
    // controllerRequired reports the source gate only. Options has a popup;
    // Course merely refuses input. This value does not create either effect.
    none, request, controllerRequired, unavailable
};
enum class OriginalUiRouteBlocker50KG : std::uint32_t {
    none=0, sourceController=1u<<0, profileOwner=1u<<1,
    selectionOwner=1u<<2, destinationOwner=1u<<3,
    galleryAvailabilityOwner=1u<<4, galleryLocked=1u<<5,
    riderUnlockOwner=1u<<6, riderLocked=1u<<7,
    riderComparisonOwner=1u<<8, humanCount=1u<<9,
    controllerPresence=1u<<10, courseControlData=1u<<11,
    courseLaunchOwner=1u<<12, unsupportedMode34=1u<<13,
    selectedControl=1u<<14
};
struct OriginalUiRouteRequest50KG {
    OriginalUiRouteKind50KG kind=OriginalUiRouteKind50KG::none;
    std::uint8_t sourceScreen=0,sourcePage=0,targetScreen=0,targetPage=0;
    std::uint32_t sourcePC=0;
};

// Typed profile destinations. Numeric values are provenance offsets, NEVER
// native memory offsets or guest addresses. word writes replace all 32 bits;
// maskedByte/maskedHalfword preserve every bit outside replaceMask.
enum class OriginalUiProfileField50KG : std::uint32_t {
    mode34=0x34,previousMode38=0x38,optionsSlot6E4=0x6E4,
    riderMode0_6F0=0x6F0,rider1Mode1_6F4=0x6F4,rider2_6F8=0x6F8,
    riderOtherMode6FC=0x6FC,courseMode0_700=0x700,lapsMode0_704=0x704,
    entrantsMode0_708=0x708,subtypeMode0_70C=0x70C,
    courseMode1_710=0x710,entrantsMode1_714=0x714,lapsMode1_718=0x718,
    gallerySlot71C=0x71C,comparisonRider100C0=0x100C0,
    subtype11258=0x11258,packed1125C=0x1125C,rider1Bits1125D=0x1125D,
    rider2Bits1125E=0x1125E,trackBits1125F=0x1125F
};
enum class OriginalUiProfileWrite50KG : std::uint8_t { word,maskedByte,maskedHalfword };
struct OriginalUiProfileChange50KG {
    OriginalUiProfileField50KG field=OriginalUiProfileField50KG::mode34;
    OriginalUiProfileWrite50KG operation=OriginalUiProfileWrite50KG::word;
    std::uint32_t value=0,replaceMask=0xFFFFFFFFu,sourcePC=0;
};
constexpr std::size_t originalUiProfileChangeLimit50KG=12;
struct OriginalUiProfileChanges50KG {
    std::array<OriginalUiProfileChange50KG,originalUiProfileChangeLimit50KG> values{};
    std::size_t count=0;
};
struct OriginalUiRouteDecision50KG {
    OriginalUiRouteAction50KG action=OriginalUiRouteAction50KG::none;
    OriginalUiRouteRequest50KG request{};
    OriginalUiProfileChanges50KG profileChanges{};
    std::uint32_t blockers=0;
    std::uint16_t selectedSlot=0; // Original one-based menu/course slot.
    std::int32_t selectedRider=-1; // Original rider value, NOT slot minus one.
    std::uint32_t inputPC=0;
    // Complete means the supplied source facts determine the profile proposal.
    // Admitted means this leaf's independent native-owner gates also passed.
    // Neither flag means a profile commit or manager transition has occurred.
    bool profileProposalComplete=false,admitted=false;
};
constexpr bool hasOriginalUiRouteBlocker50KG(const OriginalUiRouteDecision50KG& d,
    OriginalUiRouteBlocker50KG blocker) noexcept {
    return (d.blockers&static_cast<std::uint32_t>(blocker))!=0;
}
struct OriginalUiRouteAdmissions50KG {
    // True only when the actual source-page controller, selection data and
    // profile owner exist. A registry match/asset name never grants admission.
    bool sourceControllerReady=false,selectionOwnerReady=false,profileOwnerReady=false;
    // Bit N admits original page N of a screen only after all native page,
    // widget/assets, unlock and callback owners for that destination are ready.
    // A missing bounded page-factory identity remains closed even with a set
    // bit (currently gallery screens1/2/3 and HOF11). The mask cannot create it.
    // All closed by default. Bit zero is unused; this is not RR_RACE_MODE.
    std::array<std::uint32_t,15> nativePageMasks{};
};
struct OriginalUiOptionsContext50KG {
    OriginalUiRouteAdmissions50KG admission{};
    bool popupActive=false,sourceInputBlocked=false,secondControllerPresent=false;
    std::uint32_t mode34=0; // Independent from the race subtype at +0x11258.
};
struct OriginalUiGalleryContext50KG {
    OriginalUiRouteAdmissions50KG admission{};
    bool sourceInputBlocked=false;
    // The original availability owner must derive this from profile+0x11018
    // and the original enable callbacks. Asset existence is insufficient.
    bool availabilityOwnerReady=false;
    std::uint8_t enabledSlotMask=0; // bits 0..2 represent original slots 1..3.
};
struct OriginalUiRiderComparisonRow50KG {
    bool ownerReady=false;
    std::uint32_t originalRider=0,word2190=0,word2194=0;
    // Source addresses each word at profile + rider*0x894 + word offset.
};
struct OriginalUiRiderContext50KG {
    OriginalUiRouteAdmissions50KG admission{};
    bool popupActive=false,sourceInputBlocked=false;
    std::uint8_t playerIndex=0; // Original constructor value: 0=>page1, 1=>page2.
    std::uint32_t selectedRider=0,mode34=0;
    std::uint16_t packed1125C=0;
    bool unlockOwnerReady=false;
    std::uint32_t selectedUnlockWord10FAC=0;
    std::uint32_t comparisonRider100C0=0;
    OriginalUiRiderComparisonRow50KG currentComparisonRow{},selectedComparisonRow{};
};
struct OriginalUiCourseContext50KG {
    OriginalUiRouteAdmissions50KG admission{};
    bool sourceInputBlocked=false,confirmEnabled42C=false;
    bool firstControllerPresent=false,secondControllerPresent=false;
    std::uint32_t mode34=0;
    std::uint16_t packed1125C=0;
    std::uint16_t courseSlot14E4=0,courseGroupSlotCount=0;
    std::uint32_t laps14EC=0,entrants14F0=0;
    std::uint16_t subtypeSlot14F4=0,subtypeGroupSlotCount=0;
    bool courseControlDataReady=false;
    std::uint32_t selectedControlWord44=0; // Never derive from courseSlot14E4.
};

// Confirm/back route leaves only. Inputs are immutable native snapshots; no
// guest execution, allocation, callbacks, save IO, transitions or gameplay.
// The menus read the original controller's attached group's one-based slot
// (0x2A6F60), not a guessed group number. Its page identity must be bound to the
// corresponding original screen/page by the source controller owner.
//
// Malformed/unsupported calls preserve output. noChange publishes an empty
// decision. unavailable publishes diagnostic/prospective fields ONLY. The
// consumer must require status==ok AND admitted AND profileProposalComplete,
// then transactionally admit the manager request and all profile changes with
// the same still-current source snapshot. No partial write on rejection.
OriginalUiRouteStatus50KG dispatchOriginalUiOptions50KG(const OriginalUiNavigation50KG&,
    std::uint8_t controller,OriginalUiRouteInput50KG,const OriginalUiOptionsContext50KG&,
    OriginalUiRouteDecision50KG&) noexcept;
OriginalUiRouteStatus50KG dispatchOriginalUiGallery50KG(const OriginalUiNavigation50KG&,
    std::uint8_t controller,OriginalUiRouteInput50KG,const OriginalUiGalleryContext50KG&,
    OriginalUiRouteDecision50KG&) noexcept;
OriginalUiRouteStatus50KG dispatchOriginalUiRider50KG(std::uint8_t controller,
    OriginalUiRouteInput50KG,const OriginalUiRiderContext50KG&,
    OriginalUiRouteDecision50KG&) noexcept;
// Course back routes and remembered fields are closed. Activate can describe
// source-proven profile changes, but always returns unavailable/courseLaunchOwner:
// the complete native course-commit/launch owner is not implemented here.
OriginalUiRouteStatus50KG dispatchOriginalUiCourse50KG(std::uint8_t controller,
    OriginalUiRouteInput50KG,const OriginalUiCourseContext50KG&,
    OriginalUiRouteDecision50KG&) noexcept;
} // namespace rr::game
