#pragma once
#include "original_ui_route_decisions50kg.h"
#include <cstdint>

namespace rr::game {
// Complete source-only input leaf 1F2310 and its inherited request helper
// 1D6130. This is a proposal, NOT a session, admission receipt, or profile
// mutation. A native Tournament owner must read the actual popup/manager and
// retain its source bank until the outgoing frame completes. Never use this
// leaf to enable Main/Rider/Tournament in the absence of those concrete owners.
enum class OriginalUiTournamentInputStatus50KG : std::uint8_t { ignored, proposed };
struct OriginalUiTournamentInputPlan50KG {
    OriginalUiTournamentInputStatus50KG status=OriginalUiTournamentInputStatus50KG::ignored;
    OriginalUiRouteRequest50KG request{};
    // Original 292360 arguments: (audioOwner40, sound, 1, 0), followed by
    // 267BE0(application->object0->objectA0+40). Source calls audio even when
    // helper1D6130 suppresses the screen request because manager170 is 2.
    std::uint32_t sound=0, soundPC=0, transportPC=0;
    std::uint32_t requestCallPC=0;
};
// Source ignores every nonzero controller and every event except 4,5,6.
// popupFlags29 is the actual byte from the page's retained popup438 owner;
// managerState170 is the actual manager word. No profile writes occur here.
// Unsupported destinations (notably Garage13/1) remain proposals only.
OriginalUiTournamentInputPlan50KG planOriginalUiTournamentInput50KG(
    std::uint32_t controller,std::uint32_t originalEvent,
    std::uint8_t popupFlags29,std::uint32_t managerState170) noexcept;
}
