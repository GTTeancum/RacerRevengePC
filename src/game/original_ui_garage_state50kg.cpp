#include "original_ui_garage_state50kg.h"
#include <cmath>
#include <limits>

namespace rr::game {
namespace {
bool zeroDelta(const OriginalUiGarageState50KG& s) noexcept {
    for(const auto d:s.delta500)if(d)return false;
    return true;
}
bool validFresh(const OriginalUiGarageState50KG& s) noexcept {
    if(!s.known||s.selected4E4>=6||s.available34B0!=0||!zeroDelta(s))return false;
    for(unsigned i=0;i<6;++i)
        if(s.base[i]<0||s.current4E8[i]!=s.base[i]||
           s.maximum[i]<s.current4E8[i]||s.maximum[i]>20)return false;
    return true;
}
}
bool convertOriginalUiGarageStat50KG(float value,std::int32_t& out) noexcept {
    if(!std::isfinite(value))return false;
    volatile float product=20.f*value;
    volatile float rounded=product+.5f;
    const float result=rounded;
    if(!std::isfinite(result)||double(result)<double(std::numeric_limits<std::int32_t>::min())||
       double(result)>double(std::numeric_limits<std::int32_t>::max()))return false;
    auto count=static_cast<std::int32_t>(result);
    if(count>20)count=20;
    out=count;return true;
}
bool enterOriginalUiGarageFreshState50KG(const OriginalUiGarageProfileNumbers50KG& p,
    OriginalUiGarageState50KG& state) noexcept {
    if(state.selected4E4>=6||p.unspent2194!=0)return false;
    OriginalUiGarageState50KG s;s.selected4E4=state.selected4E4;
    for(unsigned i=0;i<6;++i){
        if(!std::isfinite(p.base0180[i])||!std::isfinite(p.current1920[i])||
           !std::isfinite(p.maximum0408[i])||p.base0180[i]<0||p.maximum0408[i]<p.base0180[i]||
           p.current1920[i]!=p.base0180[i]||
           !convertOriginalUiGarageStat50KG(p.base0180[i],s.base[i])||
           !convertOriginalUiGarageStat50KG(p.current1920[i],s.current4E8[i])||
           !convertOriginalUiGarageStat50KG(p.maximum0408[i],s.maximum[i]))return false;
    }
    //1FAB8C/1FABA8..BC/1FABF0: counts, zero deltas, copied actual cash.
    s.available34B0=p.unspent2194;s.known=true;
    if(!validFresh(s))return false;
    state=s;return true;
}
bool describeOriginalUiGarageFreshRow50KG(const OriginalUiGarageState50KG& s,
    std::uint32_t row,OriginalUiGarageRowNumbers50KG& out) noexcept {
    if(row>=6||!validFresh(s))return false;
    OriginalUiGarageRowNumbers50KG d;
    d.base=s.base[row];d.current=s.current4E8[row];d.maximum=s.maximum[row];
    //1FD918..92C: multiplier50*(current+delta+1)+addend300, for every row.
    //At max the original displays localized210 "MAX", not a numeric cost.
    d.maximumReached=d.current==d.maximum;
    d.nextPrice=d.maximumReached?0:50*(d.current+1)+300;
    const auto upgraded=d.current-d.base;
    // Preserve literal1FCCC4/1FCCCC condition, even though it differs from the
    // max-cost condition. In this fresh scope upgraded==0 and ordinal==1.
    d.showUpgradeDescription=upgraded!=d.maximum&&upgraded<14;
    d.upgradeNameOrdinal=d.showUpgradeDescription?upgraded+1:0;
    out=d;return true;
}
OriginalUiGarageInputPlan50KG planOriginalUiGarageFreshInput50KG(
    const OriginalUiGarageState50KG& state,const OriginalUiGarageInputContext50KG& c) noexcept {
    OriginalUiGarageInputPlan50KG p;p.next=state;
    // Original checks these BEFORE controller/event. Popup2 enters1FEB90
    // and popup3 clears its real result; neither is an information-only call.
    if(c.dialogChoice3D8){
        p.status=OriginalUiGarageInputStatus50KG::unavailableModal;
        p.branchPC=0x1FE6D4;return p;
    }
    if(c.informationOverlay4DC){
        p.branchPC=0x1FE74C;
        if(!c.controller&&(c.event==4||c.event==6)){
            p.status=OriginalUiGarageInputStatus50KG::proposed;
            p.intent=OriginalUiGarageInputIntent50KG::shortenClosingWait;
            p.stateWritePC=0x1FE788;
        }
        return p; //Other events/controllers have no source effects.
    }
    if(c.controller||c.event>=7||c.event==5)return p;
    if(c.event==4&&!zeroDelta(state)){
        p.status=OriginalUiGarageInputStatus50KG::unavailableApply;
        p.branchPC=0x1FDF18;return p;
    }
    if(!validFresh(state)){
        p.status=OriginalUiGarageInputStatus50KG::unavailableState;return p;
    }
    using I=OriginalUiGarageInputIntent50KG;
    p.status=OriginalUiGarageInputStatus50KG::proposed;
    const auto row=state.selected4E4;
    switch(c.event){
    case 0:
        p.intent=I::minimumRejected;p.branchPC=0x1FE28C;
        p.next.delta500[row]=state.base[row]-state.current4E8[row];
        p.stateWritePC=0x1FE2F4;p.sound=7;p.soundPC=0x1FE304;p.transportPC=0x1FE31C;break;
    case 1:
        p.branchPC=0x1FE404;
        if(state.current4E8[row]>=state.maximum[row]){
            p.intent=I::maximumRejected;p.next.delta500[row]=state.maximum[row]-state.current4E8[row];
            p.stateWritePC=0x1FE444;p.sound=7;p.soundPC=0x1FE460;p.transportPC=0x1FE478;
        }else{
            //Cash0 can never fund the positive next price in the admitted
            //0..20 count domain.1FE5DC branches to1FE5E8 before either write.
            p.intent=I::insufficientFunds;p.hasWattoAction=true;p.wattoAction=1;p.wattoPC=0x1FE5EC;
            p.sound=7;p.soundPC=0x1FE60C;p.transportPC=0x1FE624;
        }
        break;
    case 2:
        p.intent=I::previousStat;p.branchPC=0x1FE1AC;
        p.next.selected4E4=row?row-1:5;p.stateWritePC=row?0x1FE1B4:0x1FE1C8;
        p.selectionGroupCallPC=0x1FE1D4;p.selectionGroupFunction=0x2A51B0;
        p.sound=3;p.soundPC=0x1FE1F4;p.transportPC=0x1FE20C;break;
    case 3:
        p.intent=I::nextStat;p.branchPC=0x1FE21C;
        p.next.selected4E4=row==5?0:row+1;p.stateWritePC=row==5?0x1FE238:0x1FE224;
        p.selectionGroupCallPC=0x1FE244;p.selectionGroupFunction=0x2A50F0;
        p.sound=4;p.soundPC=0x1FE264;p.transportPC=0x1FE27C;break;
    case 4:
        p.intent=I::acceptUnchanged;p.branchPC=0x1FDF18;p.requestCallPC=0x1FE0F4;
        p.hasWattoAction=true;p.wattoAction=3;p.wattoPC=0x1FE100;
        p.sound=1;p.soundPC=0x1FE120;p.transportPC=0x1FE138;break;
    case 6:
        p.intent=I::back;p.branchPC=0x1FE148;p.requestCallPC=0x1FE158;
        p.hasWattoAction=true;p.wattoAction=3;p.wattoPC=0x1FE164;
        p.sound=5;p.soundPC=0x1FE184;p.transportPC=0x1FE19C;break;
    default:break;
    }
    if(p.requestCallPC&&c.managerState170!=2){
        p.request.kind=OriginalUiRouteKind50KG::changeScreen;
        p.request.sourceScreen=13;p.request.sourcePage=1;
        p.request.targetScreen=9;p.request.targetPage=1;p.request.sourcePC=p.requestCallPC;
    }
    return p;
}
}
