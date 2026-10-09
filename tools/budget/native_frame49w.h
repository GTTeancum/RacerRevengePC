#pragma once
// Native-only replay of the exact declared cuts in export_vehicle_frame49w.
// No state replacement occurs inside a 144-frame history. Caller snapshots,
// fixtures and checks remain outside measured intervals.
#include "game/vehicle_frame49w.h"
#include "game/frame_timing49i.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>
namespace budget49w {
using namespace rr::game;
using U=std::uint32_t; using H=std::uint64_t;
constexpr U frames=144;
constexpr H basis=1469598103934665603ull;
inline void need(bool good,const char* why){if(!good){std::fprintf(stderr,"FAIL budget49W: %s\n",why);std::exit(2);}}
template<class T> T normalized(const T& x){static_assert(std::is_trivially_copyable<T>::value);T y=x;__builtin_clear_padding(&y);return y;}
inline H hashBytes(const void* data,std::size_t count){H h=basis;const auto*p=static_cast<const unsigned char*>(data);for(std::size_t i=0;i<count;++i){h^=p[i];h*=1099511628211ull;}return h;}
template<class T> H hash(const T& x){const auto y=normalized(x);return hashBytes(&y,sizeof y);}
template<class T> bool equal(const T& a,const T& b){const auto x=normalized(a),y=normalized(b);return std::memcmp(&x,&y,sizeof x)==0;}
struct Immutable {
    VehicleFrameParameters49W parameters{};
    VehicleFrameBindings49W bindings{}; // Stored with pointers null, rebound once.
    SecondaryInputs49M secondary{};
    SubstepSelectionContext49K selectors{};
    PlayerActionCodes48X codes{};
    bool configurationEntryNonzero=false;
    rr::assets::MotionNodeBindings49O rows[3]{};
    ChariotIkPair49T ik{};
};
inline void clearPointers(VehicleFrameBindings49W& b){auto&a=b.animation;a.chariotRows=a.leftRows=a.rightRows=nullptr;a.brakeLeft28C=a.brakeRight290=a.forward280=a.back284=a.left2A0=a.right29C=nullptr;a.handRecords=nullptr;}
inline void bindPointers(Immutable& d,const rr::assets::MotionView49O* motions){auto&a=d.bindings.animation;a.leftRows=&d.rows[0];a.rightRows=&d.rows[1];a.chariotRows=&d.rows[2];a.left2A0=&motions[0];a.right29C=&motions[1];a.brakeLeft28C=&motions[2];a.brakeRight290=&motions[3];a.forward280=&motions[4];a.back284=&motions[5];a.handRecords=&d.ik;}
struct Running {
    VehicleFrameState49W state{};
    VehicleFrameWorkspace49W workspace{};
    VehicleFrameDiagnostic49W diagnostic{};
    FrameTimingState49I clock{1000,1000,0,0,7,0,0};
    U timestamp=1000;
};
struct Support {U mainBits=0,secondaryBits=0,physical=0;};
struct Expected {
    VehicleFrameState49W graphics{},after{};
    Support support[4]{};
    U substeps=0;
};
inline U bits(float x){U y;std::memcpy(&y,&x,4);return y;}
inline PreservingStage49W cut(PreservingCut49W p){return {p,true,true};}
inline PlayerActionResult48X noAction(void*,std::int32_t,std::uint32_t)noexcept{return PlayerActionResult48X::NoMatch;}
inline DualShockAxisBytes48X packet(U frame){if(frame<32)return {128,80,128,48};if(frame<48)return {136,128,120,128};return {128,255,128,255};}
inline bool graphics(Running& r,const Immutable& d,U frame){
    auto&s=r.state;auto&w=r.workspace;auto&err=r.diagnostic;const auto&b=d.bindings;const auto&p=d.parameters;
    r.timestamp+=(frame%3?17:16);FrameElapsed49I elapsed{};
    if(!beginFrameTiming49I(r.clock,r.timestamp,0,elapsed))return false;finishFrameTiming49I(r.clock);
    FrameReader49K read{};read.packet=packet(frame);read.preparedFactor=elapsed.elapsedSeconds;read.actions={nullptr,noAction};read.codes=d.codes;read.context.previousFlagsB9C=s.bodies.main.controls.player.flagsB9C;
    if(!readVehicleFrame49W(s,b,read,w,err))return false;
    PostPlayerAt217DE0_49K post{};post.player=s.bodies.main.controls.player;post.bindingContext.configurationEntryNonzero=d.configurationEntryNonzero;
    return bindVehicleFrame49W(s,b,p,cut(PreservingCut49W::PlayerAndBaseBefore217DE0),post,w,err)
        &&graphicsVehicleFrame49W(s,b,p,cut(PreservingCut49W::Base2176E0BeforeGraphics),w,err)
        &&refreshMainVehicleFrame49W(s,b,cut(PreservingCut49W::BaseAfterGraphicsThroughBA0ResetBeforeChildren),w,err)
        &&refreshSecondaryVehicleFrame49W(s,b,cut(PreservingCut49W::OtherChildrenBetween340And35C),w,err)
        &&scheduleVehicleFrame49W(s,b,cut(PreservingCut49W::OtherChildrenAfter35C),60,w,err);
}
inline bool substep(Running&r,const Immutable&d,U frame,bool enabled,Support* observed=nullptr){
    auto&s=r.state;auto&w=r.workspace;auto&err=r.diagnostic;const auto&b=d.bindings;const auto&p=d.parameters;
    if(!beginVehicleSubstep49W(s,b,d.selectors,w,err))return false;
    auto&m=s.bodies.main;PostWorld49W world{};
    world.point=m.substepIndex==0?(s.bodies.vehicle838?WorldCut49W::First358Secondary36CAnd219420:WorldCut49W::First358And219420):(m.vehicle380?WorldCut49W::LaterRequired358And219420:WorldCut49W::LaterSkipped);
    world.effects=contactEffects49W(s);world.applicationGateByte58=2;
    if(!produceVehicleForces49W(s,b,p,world,w,err))return false;
    Post354_49W response{contactEffects49W(s),0,frame%2,s.bodies.vehicle838};
    volatile float mainSupport=enabled?p.bodies.main.body.mass*m.selectedGravity:0.0f;
    if(enabled){volatile float fy=response.effects.main.loads.force.v[1]+mainSupport;response.effects.main.loads.force.v[1]=fy;}
    float secondarySupport=0;
    if(enabled&&s.bodies.vehicle838){volatile float value=p.bodies.secondary.body.mass*s.fields.gravity37C;secondarySupport=value;volatile float sy=response.effects.secondary.loads.force.v[1]+secondarySupport;response.effects.secondary.loads.force.v[1]=sy;}
    if(observed)*observed={bits(mainSupport),bits(secondarySupport),s.bodies.vehicle838};
    if(!respondVehicle354_49W(s,b,p,response,d.secondary,w,err))return false;
    if(s.phase==VehicleFramePhase49W::Post368){const auto e=contactEffects49W(s);if(!respondVehicle368_49W(s,b,p,e,w,err))return false;}
    return integrateVehicleMain49W(s,b,p,w,err)&&finishVehicleQuery49W(s,b,p,{0,true,true},w,err);
}
inline bool finish(Running&r,const Immutable&d,U frame,bool support){do{if(!substep(r,d,frame,support))return false;}while(r.state.phase!=VehicleFramePhase49W::Ready);return true;}
inline bool advance(Running&r,const Immutable&d,U frame,bool support){return graphics(r,d,frame)&&finish(r,d,frame,support);}
inline void restart(Running&r,const VehicleFrameState49W&initial){r.state=initial;r.clock={1000,1000,0,0,7,0,0};r.timestamp=1000;}
}
