#include "original_ui_garage_state50kg.h"
#include <cmath>
#include <limits>

namespace rr::game {
namespace {
bool zeroDelta(const OriginalUiGarageState50KG& s) noexcept {
    for(const auto d:s.delta500)if(d)return false;
    return true;
}
bool validEditable(const OriginalUiGarageState50KG& s) noexcept {
    if(!s.known||s.selected4E4>=6||s.available34B0<0)return false;
    for(unsigned i=0;i<6;++i)
        if(s.base[i]<0||s.current4E8[i]<s.base[i]||
           s.maximum[i]<s.current4E8[i]||s.maximum[i]>20||s.delta500[i]<s.base[i]-s.current4E8[i]||
           s.delta500[i]>s.maximum[i]-s.current4E8[i])return false;
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
    if(state.selected4E4>=6||p.unspent2194<0)return false;
    OriginalUiGarageState50KG s;s.selected4E4=state.selected4E4;
    for(unsigned i=0;i<6;++i){
        if(!std::isfinite(p.base0180[i])||!std::isfinite(p.current1920[i])||
           !std::isfinite(p.maximum0408[i])||p.base0180[i]<0||p.maximum0408[i]<p.base0180[i]||
           p.current1920[i]<0||
           !convertOriginalUiGarageStat50KG(p.base0180[i],s.base[i])||
           !convertOriginalUiGarageStat50KG(p.current1920[i],s.current4E8[i])||
           !convertOriginalUiGarageStat50KG(p.maximum0408[i],s.maximum[i]))return false;
    }
    //1FAB8C/1FABA8..BC/1FABF0: counts, zero deltas, copied actual cash.
    s.available34B0=p.unspent2194;s.known=true;
    if(!validEditable(s))return false;
    state=s;return true;
}
bool describeOriginalUiGarageFreshRow50KG(const OriginalUiGarageState50KG& s,
    std::uint32_t row,OriginalUiGarageRowNumbers50KG& out) noexcept {
    if(row>=6||!validEditable(s))return false;
    OriginalUiGarageRowNumbers50KG d;
    d.base=s.base[row];d.current=s.current4E8[row]+s.delta500[row];d.maximum=s.maximum[row];
    //1FD918..92C: multiplier50*(current+delta+1)+addend300, for every row.
    //At max the original displays localized210 "MAX", not a numeric cost.
    d.maximumReached=d.current==d.maximum;
    d.nextPrice=d.maximumReached?0:50*(d.current+1)+300;
    const auto upgraded=d.current-d.base;
    // Preserve literal1FCCC4/1FCCCC condition, even though it differs from the
    // max-cost condition. Previously earned upgrades select their real ordinal.
    d.showUpgradeDescription=upgraded!=d.maximum&&upgraded<14;
    d.upgradeNameOrdinal=d.showUpgradeDescription?upgraded+1:0;
    out=d;return true;
}
OriginalUiGarageInputPlan50KG planOriginalUiGarageFreshInput50KG(
    const OriginalUiGarageState50KG& state,const OriginalUiGarageInputContext50KG& c) noexcept {
    OriginalUiGarageInputPlan50KG p;p.next=state;
    // Shared result is observed before controller/event, as at1FE6D4.
    if(c.dialogChoice3D8){
        if(c.dialogChoice3D8==1)return p;
        if(c.dialogChoice3D8==3){
            p.status=OriginalUiGarageInputStatus50KG::proposed;
            p.intent=OriginalUiGarageInputIntent50KG::cancelPurchase;
            p.branchPC=0x1FE73C;return p;
        }
        if(c.dialogChoice3D8!=2||!validEditable(state)||zeroDelta(state)){
            p.status=OriginalUiGarageInputStatus50KG::unavailableModal;
            p.branchPC=0x1FE6D4;return p;
        }
        p.status=OriginalUiGarageInputStatus50KG::proposed;
        p.intent=OriginalUiGarageInputIntent50KG::confirmPurchase;
        p.branchPC=0x1FE6F4;p.requestCallPC=0x1FE70C;
        p.hasWattoAction=true;p.wattoAction=0;p.wattoPC=0x1FE718;
        if(c.managerState170!=2){
            p.request.kind=OriginalUiRouteKind50KG::changeScreen;
            p.request.sourceScreen=13;p.request.sourcePage=1;
            p.request.targetScreen=9;p.request.targetPage=1;p.request.sourcePC=p.requestCallPC;
        }
        return p;
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
    if(!validEditable(state)){
        p.status=OriginalUiGarageInputStatus50KG::unavailableState;return p;
    }
    using I=OriginalUiGarageInputIntent50KG;
    p.status=OriginalUiGarageInputStatus50KG::proposed;
    const auto row=state.selected4E4;
    switch(c.event){
    case 0:{
        const auto total=state.current4E8[row]+state.delta500[row];
        if(total>state.base[row]){
            //1FE344 full refund of pending buys;1FE374 half-price sale of
            // already-owned notches. Signed division truncates toward zero.
            const auto full=50*total+300;
            const auto refund=state.delta500[row]>0?full:full/2;
            const auto cash=std::int64_t(state.available34B0)+refund;
            if(cash>std::numeric_limits<std::int32_t>::max()){
                p.status=OriginalUiGarageInputStatus50KG::unavailableState;return p;}
            p.intent=I::decrementPending;p.branchPC=0x1FE32C;
            p.next.available34B0=static_cast<std::int32_t>(cash);--p.next.delta500[row];
            p.stateWritePC=state.delta500[row]>0?0x1FE370:0x1FE3AC;
            p.sound=9;p.soundPC=0x1FE3C8;p.transportPC=0x1FE3E0;
        }else{
            p.intent=I::minimumRejected;p.branchPC=0x1FE28C;
            p.next.delta500[row]=state.base[row]-state.current4E8[row];
            p.stateWritePC=0x1FE2F4;p.sound=7;p.soundPC=0x1FE304;p.transportPC=0x1FE31C;
        }
        break;}
    case 1:{
        p.branchPC=0x1FE404;
        const auto total=state.current4E8[row]+state.delta500[row];
        if(total>=state.maximum[row]){
            p.intent=I::maximumRejected;
            p.stateWritePC=0x1FE444;p.sound=7;p.soundPC=0x1FE460;p.transportPC=0x1FE478;
        }else if(state.delta500[row]<0){
            //1FE488..5B0: restoring a pending sale is NOT a new purchase and
            // has no Watto response. Preserve the source's distinct precheck.
            const auto checkedPrice=(50*total+300)/2;
            if(state.available34B0<checkedPrice){
                p.intent=I::insufficientFunds;p.branchPC=0x1FE4C8;
                p.sound=7;p.soundPC=0x1FE4E8;p.transportPC=0x1FE500;
            }else{
                const auto price=(50*(total+1)+300)/2;
                if(state.available34B0<price){
                    // Source can enter negative cash here. That domain is not
                    // admitted by the native persisted profile; do not partly
                    // mutate or replace either original price calculation.
                    p.status=OriginalUiGarageInputStatus50KG::unavailableState;return p;}
                p.intent=I::restoreOwnedPending;p.branchPC=0x1FE510;
                ++p.next.delta500[row];p.next.available34B0-=price;
                p.stateWritePC=0x1FE524;p.sound=10;p.soundPC=0x1FE58C;p.transportPC=0x1FE5A4;
            }
        }else{
            const auto price=50*(total+1)+300;
            if(state.available34B0>=price){
                p.intent=I::incrementPending;p.hasWattoAction=true;p.wattoAction=2;p.wattoPC=0x1FE638;
                ++p.next.delta500[row];p.next.available34B0-=price;
                p.stateWritePC=0x1FE660;p.sound=10;p.soundPC=0x1FE6AC;p.transportPC=0x1FE6C4;
            }else{
                p.intent=I::insufficientFunds;p.hasWattoAction=true;p.wattoAction=1;p.wattoPC=0x1FE5EC;
                p.sound=7;p.soundPC=0x1FE60C;p.transportPC=0x1FE624;
            }
        }
        break;}
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
        if(!zeroDelta(state)){
            p.intent=I::requestPurchase;p.branchPC=0x1FDF18;
            p.sound=1;p.soundPC=0x1FE120;p.transportPC=0x1FE138;break;
        }
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
