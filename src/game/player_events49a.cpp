#include "player_events49a.h"
#include <cmath>
#include <cstring>

namespace rr::game {
ManagerHeldResult49A queryManagerHeld49A(const ManagerHeldSources49A& sources,std::int32_t code,
                                      std::uint32_t category,std::uint32_t mask,
                                      std::uint32_t sourceIndex) noexcept {
    if(category==2u) {
        if(sourceIndex>=8u)return {ManagerHeldStatus49A::UnsupportedDeviceIndex,false};
        const auto* bank=sources.devices[sourceIndex];
        if(!bank)return {ManagerHeldStatus49A::Complete,false};
        bool matched=false;
        const DeviceQueryContext48Y context{sources.currentContext.present,sources.rootFlags228,
                                           sources.context518,sources.context1898};
        if(!queryDeviceHeld48Y(*bank,code,mask,context,matched,nullptr))
            return {ManagerHeldStatus49A::UnsupportedDeviceCode,false};
        return {ManagerHeldStatus49A::Complete,matched};
    }
    if(category>1u)return {ManagerHeldStatus49A::Complete,false};
    const auto& source=category==0u?sources.currentContext:sources.auxiliary;
    if(!source.present)return {ManagerHeldStatus49A::Complete,false};
    if(!source.query)return {ManagerHeldStatus49A::MissingSourceBinding,false};
    const auto result=source.query(source.context,code,mask);
    if(!result.available)return {ManagerHeldStatus49A::SourceUnavailable,false};
    return {ManagerHeldStatus49A::Complete,result.originalReturn!=0u};
}

PlayerEventResult49A handlePlayerPress49A(PlayerEventState49A& player,PlayerEventEnvironment49A& env,
                                        const PlayerEventBindings49A& bindings,
                                        InputEvent48Z& event,DeviceButtonRecord48Y& record) noexcept {
    PlayerEventResult49A result{PlayerEventStatus49A::Complete,false,0,0,
                               ManagerHeldStatus49A::Complete,{EventRouteStatus48Z::Complete,false,0,0}};
    if(!env.actionRows||player.indexFFC>=env.actionRowCount){result.status=PlayerEventStatus49A::InvalidActionRow;return result;}
    auto query=[&](std::int32_t code,std::uint32_t category,std::uint32_t mask,bool& matched) noexcept {
        const auto q=queryManagerHeld49A(env.held,code,category,mask,player.source5C);
        result.lastQuery=q.status;
        if(q.status!=ManagerHeldStatus49A::Complete){result.status=PlayerEventStatus49A::QueryUnavailable;return false;}
        ++result.queriesCompleted;matched=q.matched;return true;
    };
    auto effect=[&](PlayerEffectKind49A kind,float scalar=0.0f,std::uint32_t integer=0u) noexcept {
        if(!bindings.effects.apply){result.status=PlayerEventStatus49A::MissingEffectBinding;return false;}
        ++result.effectsInvoked;
        if(!bindings.effects.apply(bindings.effects.context,{kind,scalar,integer})) {
            result.status=PlayerEventStatus49A::EffectUnavailable;return false;
        }
        return true;
    };
    bool matched=false;
    if(!query(env.actionRows[player.indexFFC].action6AC,2u,0x3fu,matched))return result;
    if(matched&&!(env.flags29&4u)&&(env.gate58&2u)&&!(env.gate58&1u)) {
        if(!std::isfinite(player.value1098)){result.status=PlayerEventStatus49A::UnsupportedNumeric;return result;}
        if(player.value1098<0.0f) {
            if(!effect(PlayerEffectKind49A::Function2255B0))return result;
            player.value1094=2.5f;player.value1090=-1.0f;
        }
    }
    if(!query(0x1a,0u,0x80u,matched))return result;
    if(matched&&player.field40==0u) {
        if(env.lazyFlag85F4==0u){env.lazyWord4340A0=2u;env.lazyFlag85F4=1u;}
        const auto kind=env.lazyWord4340A0==0u?PlayerEffectKind49A::Virtual94:
                        env.lazyWord4340A0==1u?PlayerEffectKind49A::Virtual90:
                        env.lazyWord4340A0==2u?PlayerEffectKind49A::Virtual8C:PlayerEffectKind49A::Virtual88;
        if(!effect(kind))return result;
    }
    if(!query(0x12,0u,0x80u,matched))return result;
    if(matched&&player.source5C<=1u) {
        const auto selected=static_cast<std::uint32_t>(env.selection1125F&15u);
        if(!env.tuningRows||selected>=env.tuningRowCount){result.status=PlayerEventStatus49A::InvalidTuningRow;return result;}
        const auto& row=env.tuningRows[selected];
        if(!std::isfinite(row.value102F0)){result.status=PlayerEventStatus49A::UnsupportedNumeric;return result;}
        std::uint32_t deltaBits=0x3c23d70au;float delta;std::memcpy(&delta,&deltaBits,sizeof delta);
        volatile float left=row.value102F0,right=delta;
        const float adjusted=left-right;
        if(!std::isfinite(adjusted)){result.status=PlayerEventStatus49A::UnsupportedNumeric;return result;}
        player.valueCB4=row.value102F0;
        player.valueCB0=adjusted;
        player.countCD0=row.word10388+1u;
        if(!effect(PlayerEffectKind49A::Function22B170,player.valueCB0))return result;
        if(!effect(PlayerEffectKind49A::Function22B370,player.valueCB8))return result;
        if(!effect(PlayerEffectKind49A::Function22B570))return result;
        if(!effect(PlayerEffectKind49A::Function22B760))return result;
        if(!effect(PlayerEffectKind49A::Function22BA60,0.0f,12u))return result;
    }
    result.childRoute=routeChildInputEvent48Z(bindings.children,true,event,record,
                                            bindings.childCallback,bindings.childContext,
                                            bindings.childVisitBudget);
    if(result.childRoute.status!=EventRouteStatus48Z::Complete){result.status=PlayerEventStatus49A::ChildRouteUnavailable;return result;}
    result.handled=result.childRoute.handled;return result;
}
EventCallbackResult48Z playerEventChild49A(void* opaque,std::uint32_t child,bool press,
                                         InputEvent48Z& event,DeviceButtonRecord48Y& record) noexcept {
    if(!opaque)return {false,0};
    auto& binding=*static_cast<PlayerEventChildBinding49A*>(opaque);
    if(child!=binding.childIndex||!binding.bindings)return {false,0};
    if(press) {
        if(!binding.player||!binding.environment)return {false,0};
        binding.lastResult=handlePlayerPress49A(*binding.player,*binding.environment,*binding.bindings,event,record);
    } else {
        const auto& b=*binding.bindings;
        const auto route=routeChildInputEvent48Z(b.children,false,event,record,b.childCallback,b.childContext,b.childVisitBudget);
        binding.lastResult={route.status==EventRouteStatus48Z::Complete?PlayerEventStatus49A::Complete:
                            PlayerEventStatus49A::ChildRouteUnavailable,route.handled,0,0,ManagerHeldStatus49A::Complete,route};
    }
    return {binding.lastResult.status==PlayerEventStatus49A::Complete,binding.lastResult.handled?1u:0u};
}
}
