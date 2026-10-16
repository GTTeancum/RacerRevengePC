#include "original_ui_tournament_input50kg.h"

namespace rr::game {
OriginalUiTournamentInputPlan50KG planOriginalUiTournamentInput50KG(
    std::uint32_t controller,std::uint32_t event,
    std::uint8_t popupFlags29,std::uint32_t managerState170) noexcept {
    OriginalUiTournamentInputPlan50KG plan;
    if((popupFlags29&1u)||controller!=0)return plan;
    std::uint8_t screen=0;
    switch(event){
    case 4:
        screen=6;plan.requestCallPC=0x1F2374;
        plan.sound=1;plan.soundPC=0x1F2394;plan.transportPC=0x1F23AC;break;
    case 6:
        screen=5;plan.requestCallPC=0x1F23C8;
        plan.sound=5;plan.soundPC=0x1F23E8;plan.transportPC=0x1F2400;break;
    case 5:
        screen=13;plan.requestCallPC=0x1F241C;
        plan.sound=1;plan.soundPC=0x1F243C;plan.transportPC=0x1F2454;break;
    default:return plan;
    }
    plan.status=OriginalUiTournamentInputStatus50KG::proposed;
    // 1D6154: request helper is a no-op in manager state2. The caller's
    // following sound and transport calls are outside this helper's guard.
    if(managerState170!=2){
        plan.request.kind=OriginalUiRouteKind50KG::changeScreen;
        plan.request.sourceScreen=9;plan.request.sourcePage=1;
        plan.request.targetScreen=screen;plan.request.targetPage=1;
        plan.request.sourcePC=plan.requestCallPC;
    }
    return plan;
}
}
