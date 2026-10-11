#pragma once
#include "original_ui_game_options_controller50kg.h"

namespace rr::game {
// Exact source1F66F0 stores followed by1F5158. This describes their order; it
// cannot commit a profile, attest a PCM consumer, save a file or admit a route.
inline bool describeOriginalUiGameOptionsAccept50KG(const OriginalUiGameSettings50KG& local,
    std::uint16_t group,OriginalUiRouteDecision50KG& out) noexcept {
    if(group<1||group>8||local.soundMode>2)return false;
    for(auto volume:local.volume)if(volume>100)return false;
    using Field=OriginalUiProfileField50KG;
    using Write=OriginalUiProfileWrite50KG;
    OriginalUiRouteDecision50KG next;
    next.action=OriginalUiRouteAction50KG::unavailable;
    next.request={OriginalUiRouteKind50KG::changePage,12,4,12,1,0x1F5168};
    next.blockers=static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::destinationOwner);
    next.selectedSlot=group;next.inputPC=0x1F50F0;next.profileProposalComplete=true;
    next.profileChanges.count=9;
    next.profileChanges.values[0]={Field::settingsFlags11050,Write::maskedByte,local.autoSave?1u:0u,1u,0x1F672C};
    next.profileChanges.values[1]={Field::soundMode1101C,Write::word,local.soundMode,0xFFFFFFFFu,0x1F6744};
    next.profileChanges.values[2]={Field::sfxVolume11020,Write::word,local.volume[0],0xFFFFFFFFu,0x1F6760};
    next.profileChanges.values[3]={Field::musicVolume11024,Write::word,local.volume[1],0xFFFFFFFFu,0x1F677C};
    next.profileChanges.values[4]={Field::voiceVolume11028,Write::word,local.volume[2],0xFFFFFFFFu,0x1F6798};
    next.profileChanges.values[5]={Field::announcerVolume1102C,Write::word,local.volume[3],0xFFFFFFFFu,0x1F67B4};
    next.profileChanges.values[6]={Field::settingsFlags11050,Write::maskedByte,local.verticalSplit?2u:0u,2u,0x1F67EC};
    next.profileChanges.values[7]={Field::settingsFlags11050,Write::maskedByte,local.vibration?4u:0u,4u,0x1F6824};
    // Actual188740 belongs BETWEEN entries7 and8. Never execute this array as
    // a generic patch list, which would put remembered-group before audio.
    next.profileChanges.values[8]={Field::optionsGroup6E8,Write::word,group,0xFFFFFFFFu,0x1F5158};
    out=next;return true;
}
inline bool originalUiGameOptionsAcceptShape50KG(const OriginalUiRouteDecision50KG& decision) noexcept {
    const auto& route=decision.request;
    if(route.kind!=OriginalUiRouteKind50KG::changePage||route.sourceScreen!=12||route.sourcePage!=4||
        route.targetScreen!=12||route.targetPage!=1||route.sourcePC!=0x1F5168||decision.inputPC!=0x1F50F0||
        !decision.profileProposalComplete||decision.selectedRider!=-1||decision.profileChanges.count!=9)return false;
    const auto& writes=decision.profileChanges.values;
    OriginalUiGameSettings50KG local;
    local.autoSave=writes[0].value!=0;local.soundMode=writes[1].value;
    for(std::size_t i=0;i<4;++i)local.volume[i]=writes[i+2].value;
    local.verticalSplit=writes[6].value!=0;local.vibration=writes[7].value!=0;
    OriginalUiRouteDecision50KG exact;
    if(!describeOriginalUiGameOptionsAccept50KG(local,decision.selectedSlot,exact))return false;
    for(std::size_t i=0;i<9;++i){const auto& actual=writes[i];const auto& expected=exact.profileChanges.values[i];
        if(actual.field!=expected.field||actual.operation!=expected.operation||actual.value!=expected.value||
            actual.replaceMask!=expected.replaceMask||actual.sourcePC!=expected.sourcePC)return false;
    }
    return true;
}
inline bool sameOriginalUiGameSettings50KG(const OriginalUiGameSettings50KG& a,
    const OriginalUiGameSettings50KG& b) noexcept {
    return a.autoSave==b.autoSave&&a.vibration==b.vibration&&a.verticalSplit==b.verticalSplit&&
        a.soundMode==b.soundMode&&a.volume==b.volume;
}
inline bool sameOriginalUiGameSettingsProfile50KG(const OriginalUiGameSettingsProfile50KG& a,
    const OriginalUiGameSettingsProfile50KG& b) noexcept {
    return a.known==b.known&&a.savedGroup6E8==b.savedGroup6E8&&a.soundMode1101C==b.soundMode1101C&&
        a.volume11020==b.volume11020&&a.flags11050==b.flags11050;
}
} // namespace rr::game
