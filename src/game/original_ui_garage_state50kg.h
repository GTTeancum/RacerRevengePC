#pragma once
#include "original_ui_route_decisions50kg.h"
#include <array>
#include <cstdint>

namespace rr::game {
// Isolated Garage numeric/input proposal. This is neither a page owner nor a
// route-admission token. The caller must obtain these values coherently from
// the actual selected career/pod diagonal; a public input cannot confer that.
struct OriginalUiGarageProfileNumbers50KG {
    std::array<float,6> base0180{},current1920{},maximum0408{};
    std::int32_t unspent2194=0;
};
struct OriginalUiGarageState50KG {
    bool known=false;
    // Constructor1FA0B0 sets0. Entry1FAAF0 preserves this retained selection.
    std::uint32_t selected4E4=0;
    std::array<std::int32_t,6> base{},current4E8{},maximum{},delta500{};
    std::int32_t available34B0=0;
};
//188D10(base),188D70(current),188DE0(maximum): separate binary32 MUL/ADD,
// truncation toward zero, then upper cap20. No invented lower clamp. Returns
// false only outside finite, representable conversion domain; leaves out alone.
bool convertOriginalUiGarageStat50KG(float,std::int32_t& out) noexcept;
// Bounded unchanged-state admission is deliberately SEPARATE from conversion:
// nonnegative authored values/cash and base <= current <= maximum notch counts.
// Prior earned upgrades are retained. Entry initializes zero pending deltas;
// the legacy Fresh-named row/input helpers also support staged purchases.
// Does not write profile/cash/progression or reset the retained selected row.
bool enterOriginalUiGarageFreshState50KG(const OriginalUiGarageProfileNumbers50KG&,
    OriginalUiGarageState50KG&) noexcept;
struct OriginalUiGarageRowNumbers50KG {
    std::int32_t base=0,current=0,maximum=0,nextPrice=0,upgradeNameOrdinal=0;
    bool maximumReached=false,showUpgradeDescription=false;
};
bool describeOriginalUiGarageFreshRow50KG(const OriginalUiGarageState50KG&,
    std::uint32_t row,OriginalUiGarageRowNumbers50KG&) noexcept;

enum class OriginalUiGarageInputStatus50KG : std::uint8_t {
    ignored,proposed,unavailableState,unavailableApply,unavailableModal
};
enum class OriginalUiGarageInputIntent50KG : std::uint8_t {
    none,previousStat,nextStat,minimumRejected,maximumRejected,
    insufficientFunds,acceptUnchanged,back,shortenClosingWait,
    incrementPending,decrementPending,restoreOwnedPending,requestPurchase,confirmPurchase,cancelPurchase
};
struct OriginalUiGarageInputContext50KG {
    std::uint32_t controller=0,event=0,managerState170=0;
    // Actual shared dialog choice and Garage information-overlay state.
    // Nonzero popup choices remain closed; information-only input has the
    // recovered1FE74C timer/bypass suffix, separate from fresh navigation.
    std::uint32_t dialogChoice3D8=0,informationOverlay4DC=0;
};
struct OriginalUiGarageInputPlan50KG {
    OriginalUiGarageInputStatus50KG status=OriginalUiGarageInputStatus50KG::ignored;
    OriginalUiGarageInputIntent50KG intent=OriginalUiGarageInputIntent50KG::none;
    OriginalUiGarageState50KG next{};
    OriginalUiRouteRequest50KG request{};
    std::uint32_t branchPC=0,stateWritePC=0,requestCallPC=0;
    std::uint32_t sound=0,soundPC=0,transportPC=0;
    //1D5320(Watto owner33B0, action). Action0 is valid, so presence is explicit.
    bool hasWattoAction=false;
    std::uint32_t wattoAction=0,wattoPC=0;
    // Source selection-group call precedes sound. These fields describe the
    // required real owner operation; this leaf never synthesizes a widget.
    std::uint32_t selectionGroupCallPC=0,selectionGroupFunction=0;
};
// No external effects or profile writes. Pending purchases use full prices;
// pending sales of owned notches use the original half-price refund/restore.
// Edits stay within base..maximum and the nonnegative native cash domain. Modal result2 proposes the private retained apply owner;
// result3 only clears the dialog, preserving edits. Caller authenticates ownership.
// All plans are proposals requiring the concrete source/destination owners.
OriginalUiGarageInputPlan50KG planOriginalUiGarageFreshInput50KG(
    const OriginalUiGarageState50KG&,const OriginalUiGarageInputContext50KG&) noexcept;
}
