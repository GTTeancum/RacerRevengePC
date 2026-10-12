#include "original_ui_route_decisions50kg.h"
#include "original_ui_route_registry50kg.h"

namespace rr::game {
namespace {
using Status=OriginalUiRouteStatus50KG;
using Kind=OriginalUiRouteKind50KG;
using Action=OriginalUiRouteAction50KG;
using Blocker=OriginalUiRouteBlocker50KG;
using Field=OriginalUiProfileField50KG;
using Write=OriginalUiProfileWrite50KG;
using Decision=OriginalUiRouteDecision50KG;
using Input=OriginalUiRouteInput50KG;
static_assert(originalUiProfileChangeLimit50KG>=8);

void block(Decision& d,Blocker value) noexcept {
    d.blockers|=static_cast<std::uint32_t>(value);
}
void word(Decision& d,Field field,std::uint32_t value,std::uint32_t pc) noexcept {
    // Each fixed branch below emits at most eight changes, from a fresh result.
    d.profileChanges.values[d.profileChanges.count++]={field,Write::word,value,0xFFFFFFFFu,pc};
}
void bits(Decision& d,Field field,Write operation,std::uint32_t mask,
    std::uint32_t value,std::uint32_t pc) noexcept {
    d.profileChanges.values[d.profileChanges.count++]={field,operation,value&mask,mask,pc};
}
void route(Decision& d,Kind kind,std::uint8_t screen,std::uint8_t page,
    std::uint32_t pc) noexcept {
    d.request.kind=kind;d.request.targetScreen=screen;d.request.targetPage=page;
    d.request.sourcePC=pc;
}
bool supportedInput(Input input) noexcept { return input==Input::activate || input==Input::back; }
Status ignore(Decision& output) noexcept { output={};return Status::noChange; }
bool profileFactsReady(const OriginalUiRouteAdmissions50KG& admission) noexcept {
    return admission.selectionOwnerReady && admission.profileOwnerReady;
}
Status finish(Decision& d,const OriginalUiRouteAdmissions50KG& admission,
    Decision& output) noexcept {
    if (!admission.sourceControllerReady) block(d,Blocker::sourceController);
    if (!admission.selectionOwnerReady) block(d,Blocker::selectionOwner);
    if (!admission.profileOwnerReady) block(d,Blocker::profileOwner);
    if (d.request.kind==Kind::changeScreen || d.request.kind==Kind::changePage) {
        const auto screen=d.request.targetScreen,page=d.request.targetPage;
        if (screen>=admission.nativePageMasks.size() || screen==8 || !page || page>=32 ||
            !findOriginalUiBoundedPageFactory50KG(screen,page) ||
            !(admission.nativePageMasks[screen]&(std::uint32_t{1}<<page)))
            block(d,Blocker::destinationOwner);
    }
    d.admitted=d.blockers==0 && d.profileProposalComplete && d.request.kind!=Kind::none;
    if (d.action!=Action::controllerRequired) d.action=d.admitted?Action::request:Action::unavailable;
    output=d;return d.admitted?Status::ok:Status::unavailable;
}
Status menuSelection(const OriginalUiNavigation50KG& nav,std::uint16_t maximum,
    std::uint16_t& slot,bool& selectable) noexcept {
    // Reuse the existing complete typed-page validation without mutating nav.
    OriginalUiNavigation50KG checked;
    if (!nav.page || !bindOriginalUiNavigation50KG(*nav.page,checked)) return Status::invalidPage;
    const auto groupId=nav.controllerGroup[0];
    if (groupId<0 || static_cast<std::size_t>(groupId)>=nav.page->groups.size())
        return Status::invalidGroup;
    const auto index=static_cast<std::size_t>(groupId);
    const auto& group=nav.page->groups[index];
    const auto selected=nav.groups[index].selectedSlot;
    if (!selected || selected>maximum || selected>group.controls.size()) return Status::invalidSlot;
    slot=selected;selectable=false;
    const auto control=group.controls[selected-1];
    if (control>=0) {
        const auto id=static_cast<std::size_t>(control);
        selectable=nav.groups[index].enabled && nav.page->controls[id].present &&
            nav.controls[id].enabled && !nav.controls[id].blocked;
    }
    return Status::ok;
}
Field rememberedRider(std::uint32_t mode) noexcept {
    return mode==0?Field::riderMode0_6F0:(mode==1?Field::rider1Mode1_6F4:Field::riderOtherMode6FC);
}
std::int64_t signedWord(std::uint32_t value) noexcept {
    return value<=0x7FFFFFFFu?static_cast<std::int64_t>(value):
        static_cast<std::int64_t>(value)-0x100000000LL;
}
bool riderComparisonReady(const OriginalUiRiderContext50KG& context) noexcept {
    return context.comparisonRider100C0<32 && context.currentComparisonRow.ownerReady &&
        context.selectedComparisonRow.ownerReady &&
        context.currentComparisonRow.originalRider==context.comparisonRider100C0 &&
        context.selectedComparisonRow.originalRider==context.selectedRider;
}
bool validCourseSelections(const OriginalUiCourseContext50KG& context) noexcept {
    // Bounded native admission, not claimed original maximum course/subtype IDs.
    if (!context.courseGroupSlotCount ||
        context.courseGroupSlotCount>assets::originalUiControlLimit50KG ||
        !context.courseSlot14E4 || context.courseSlot14E4>context.courseGroupSlotCount) return false;
    // The subtype group is consumed/remembered only by original mode0. Other
    // modes retain page14F4 for the source opponent-count branch without turning
    // it into an invented selection or a native race-mode enum.
    return context.mode34!=0 || (context.subtypeGroupSlotCount &&
        context.subtypeGroupSlotCount<=assets::originalUiControlLimit50KG &&
        context.subtypeSlot14F4 && context.subtypeSlot14F4<=context.subtypeGroupSlotCount);
}
void rememberCourse(Decision& d,const OriginalUiCourseContext50KG& context,bool back) noexcept {
    if (context.mode34==0) {
        word(d,Field::courseMode0_700,context.courseSlot14E4,back?0x1DEFD8:0x1DEDCC);
        word(d,Field::lapsMode0_704,context.laps14EC,back?0x1DEFEC:0x1DEDE0);
        word(d,Field::entrantsMode0_708,context.entrants14F0,back?0x1DF000:0x1DEDF4);
        word(d,Field::subtypeMode0_70C,context.subtypeSlot14F4,back?0x1DF010:0x1DEE08);
    } else if (context.mode34==1) {
        word(d,Field::courseMode1_710,context.courseSlot14E4,back?0x1DF054:0x1DEE70);
        word(d,Field::entrantsMode1_714,context.entrants14F0,back?0x1DF068:0x1DEE84);
        word(d,Field::lapsMode1_718,context.laps14EC,back?0x1DF078:0x1DEE98);
    }
}
} // namespace

Status dispatchOriginalUiOptions50KG(const OriginalUiNavigation50KG& nav,
    std::uint8_t controller,Input input,const OriginalUiOptionsContext50KG& context,
    Decision& output) noexcept {
    if (controller>=2) return Status::invalidController;
    if (controller!=0 || context.popupActive || context.sourceInputBlocked) return ignore(output);
    if (!supportedInput(input)) return Status::unsupportedInput;
    Decision d;d.inputPC=0x1F9640;d.request.sourceScreen=12;d.request.sourcePage=1;
    bool selectable=false;
    const auto status=menuSelection(nav,6,d.selectedSlot,selectable);
    if (status!=Status::ok) return status;
    d.profileProposalComplete=profileFactsReady(context.admission);
    if (input==Input::back) {
        word(d,Field::optionsSlot6E4,d.selectedSlot,0x1F9994);
        route(d,Kind::changeScreen,4,1,0x1F99A4);
    } else {
        if (!selectable) block(d,Blocker::selectedControl);
        switch (d.selectedSlot) {
        case 1:route(d,Kind::changePage,12,2,0x1F96FC);break;
        case 2:
            route(d,Kind::changePage,12,3,0x1F9744);
            if (!context.secondControllerPresent) {
                block(d,Blocker::controllerPresence);d.action=Action::controllerRequired;
            }
            break;
        case 3:route(d,Kind::changePage,12,4,0x1F9898);break;
        case 4:
            // 0x17DE50 changes both words only when mode34 actually changes.
            if (context.mode34!=5) {
                word(d,Field::previousMode38,context.mode34,0x17DE64);
                word(d,Field::mode34,5,0x17DE6C);
            }
            route(d,Kind::changeScreen,11,1,0x1F98C8);break;
        case 5:route(d,Kind::changePage,12,5,0x1F98F4);break;
        case 6:route(d,Kind::changePage,12,6,0x1F9910);break;
        default:return Status::invalidSlot;
        }
        word(d,Field::optionsSlot6E4,d.selectedSlot,0x1F9940);
    }
    return finish(d,context.admission,output);
}

Status dispatchOriginalUiGallery50KG(const OriginalUiNavigation50KG& nav,
    std::uint8_t controller,Input input,const OriginalUiGalleryContext50KG& context,
    Decision& output) noexcept {
    if (controller>=2) return Status::invalidController;
    if (controller!=0 || context.sourceInputBlocked) return ignore(output);
    if (!supportedInput(input)) return Status::unsupportedInput;
    Decision d;d.inputPC=0x20CEE0;d.request.sourceScreen=4;d.request.sourcePage=5;
    bool selectable=false;
    const auto status=menuSelection(nav,3,d.selectedSlot,selectable);
    if (status!=Status::ok) return status;
    d.profileProposalComplete=profileFactsReady(context.admission);
    if (input==Input::back) {
        route(d,Kind::changePage,4,1,0x20D014);
        word(d,Field::gallerySlot71C,d.selectedSlot,0x20D048);
    } else {
        if (!selectable) block(d,Blocker::selectedControl);
        if (!context.availabilityOwnerReady) block(d,Blocker::galleryAvailabilityOwner);
        else if (!(context.enabledSlotMask&(1u<<(d.selectedSlot-1)))) block(d,Blocker::galleryLocked);
        const auto pc=d.selectedSlot==1?0x20CF6Cu:(d.selectedSlot==2?0x20CF88u:0x20CFA4u);
        route(d,Kind::changeScreen,static_cast<std::uint8_t>(d.selectedSlot),1,pc);
        word(d,Field::gallerySlot71C,d.selectedSlot,0x20CFDC);
    }
    return finish(d,context.admission,output);
}

Status dispatchOriginalUiRider50KG(std::uint8_t controller,Input input,
    const OriginalUiRiderContext50KG& context,Decision& output) noexcept {
    if (controller>=2) return Status::invalidController;
    if (context.playerIndex>=2) return Status::invalidPage;
    if (controller!=context.playerIndex || context.popupActive || context.sourceInputBlocked)
        return ignore(output);
    if (!supportedInput(input)) return Status::unsupportedInput;
    // Original stores mask five bits, but its row lookups use the actual rider.
    // Refuse out-of-envelope values rather than aliasing rider32 to rider0.
    if (context.selectedRider>=32) return Status::invalidRider;
    Decision d;d.inputPC=0x1EA460;d.request.sourceScreen=5;
    d.request.sourcePage=static_cast<std::uint8_t>(context.playerIndex+1);
    d.selectedRider=static_cast<std::int32_t>(context.selectedRider);
    d.profileProposalComplete=profileFactsReady(context.admission);
    if (input==Input::back) {
        if (!context.playerIndex) {
            const auto pc=context.mode34==0?0x1EA7F0u:(context.mode34==1?0x1EA804u:0x1EA80Cu);
            word(d,rememberedRider(context.mode34),context.selectedRider,pc);
            route(d,Kind::changeScreen,4,1,0x1EA810);
        } else {
            word(d,Field::rider2_6F8,context.selectedRider,0x1EADBC);
            route(d,Kind::changePage,5,1,0x1EADC0);
        }
        return finish(d,context.admission,output);
    }
    if (!context.unlockOwnerReady) {
        block(d,Blocker::riderUnlockOwner);d.profileProposalComplete=false;
        return finish(d,context.admission,output);
    }
    if (!context.selectedUnlockWord10FAC) {
        block(d,Blocker::riderLocked);return finish(d,context.admission,output);
    }
    if (!context.playerIndex) {
        const auto pc=context.mode34==0?0x1EA538u:(context.mode34==1?0x1EA548u:0x1EA54Cu);
        word(d,rememberedRider(context.mode34),context.selectedRider,pc);
        bits(d,Field::rider1Bits1125D,Write::maskedByte,0xF8,context.selectedRider<<3,0x1EA58C);
        word(d,Field::comparisonRider100C0,context.selectedRider,0x1EA5A8);
        const auto humans=(context.packed1125C>>5)&3u;
        if (humans==1) {
            const bool tournament=context.mode34==2;
            route(d,Kind::changeScreen,tournament?9:6,1,tournament?0x1EA5E4:0x1EA604);
        } else if (humans==2) route(d,Kind::changePage,5,2,0x1EA630);
        else block(d,Blocker::humanCount); // Source has no route for values0/3.
    } else {
        bits(d,Field::rider2Bits1125E,Write::maskedByte,0x1F,context.selectedRider,0x1EAB7C);
        if (!riderComparisonReady(context)) {
            block(d,Blocker::riderComparisonOwner);d.profileProposalComplete=false;
        } else {
            // SUBU then SLT at0x1EABC4..1EABDC: wrap each subtraction in
            // 32 bits, then compare as signed. Equal values keep rider100C0.
            const std::uint32_t current=context.currentComparisonRow.word2190-context.currentComparisonRow.word2194;
            const std::uint32_t selected=context.selectedComparisonRow.word2190-context.selectedComparisonRow.word2194;
            if (signedWord(selected)<signedWord(current))
                word(d,Field::comparisonRider100C0,context.selectedRider,0x1EABEC);
        }
        word(d,Field::rider2_6F8,context.selectedRider,0x1EAD5C);
        route(d,Kind::changeScreen,6,1,0x1EAD60);
    }
    return finish(d,context.admission,output);
}

Status dispatchOriginalUiCourse50KG(std::uint8_t controller,Input input,
    const OriginalUiCourseContext50KG& context,Decision& output) noexcept {
    if (controller>=2) return Status::invalidController;
    if (controller!=0 || context.sourceInputBlocked) return ignore(output);
    if (!supportedInput(input)) return Status::unsupportedInput;
    if (input==Input::activate && !context.confirmEnabled42C) return ignore(output);
    if (!validCourseSelections(context)) return Status::invalidCourse;
    Decision d;d.inputPC=0x1DEB90;d.request.sourceScreen=6;d.request.sourcePage=1;
    d.selectedSlot=context.courseSlot14E4;
    d.profileProposalComplete=profileFactsReady(context.admission);
    if (context.mode34==1 && (!context.firstControllerPresent || !context.secondControllerPresent)) {
        block(d,Blocker::controllerPresence);d.action=Action::controllerRequired;
        return finish(d,context.admission,output);
    }
    if (input==Input::back) {
        rememberCourse(d,context,true);
        if (context.mode34==0) route(d,Kind::changeScreen,5,1,0x1DF020);
        else if (context.mode34==1) route(d,Kind::changeScreen,5,2,0x1DF088);
        else route(d,Kind::changeScreen,9,1,0x1DF0B4);
        return finish(d,context.admission,output);
    }
    route(d,Kind::courseLaunch,0,0,0x1DED58);
    // This is a prospective state8 request, never an admitted native launch.
    // A concrete frontend must commit the profile/global handles and native load owner.
    block(d,Blocker::courseLaunchOwner);
    if (!context.courseControlDataReady) {
        block(d,Blocker::courseControlData);d.profileProposalComplete=false;
        return finish(d,context.admission,output);
    }
    if (context.mode34>2) {
        block(d,Blocker::unsupportedMode34);d.profileProposalComplete=false;
        return finish(d,context.admission,output);
    }
    bits(d,Field::trackBits1125F,Write::maskedByte,0x0F,context.selectedControlWord44,0x187328);
    const auto humans=(context.packed1125C>>5)&3u;
    const bool usesSelectedEntrants=context.mode34==1 || context.subtypeSlot14F4==2;
    const std::uint32_t opponents=((usesSelectedEntrants?context.entrants14F0:8u)-humans)&15u;
    bits(d,Field::packed1125C,Write::maskedHalfword,0x780,opponents<<7,
        usesSelectedEntrants?0x1DED0C:0x1DED48);
    const std::uint32_t subtype=context.mode34==0?context.subtypeSlot14F4:1u;
    word(d,Field::subtype11258,subtype,
        context.mode34==0?0x1DED80:(context.mode34==1?0x1DEE20:0x1DEEB0));
    bits(d,Field::packed1125C,Write::maskedByte,0x1F,context.mode34==2?3u:context.laps14EC,
        context.mode34==0?0x1DEDB4:(context.mode34==1?0x1DEE58:0x1DEEE4));
    rememberCourse(d,context,false);
    return finish(d,context.admission,output);
}
} // namespace rr::game
