#pragma once
#include "ai_driving50iv.h"
#include "ai_state_prefix50iv.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <limits>

namespace rr::game {
// Process-lifetime originals, borrowed by every racer. Do not construct one
// per call/racer or reset it with16E830. The source tests the guard as nonzero.
struct AiAttackWideGlobals16EDD0 {
    std::uint8_t guardGP854C=0;
    float alpha433CB8=0.f;
};

namespace ai_attack_wide_detail50LH {
using V=VisualVector49R;
using Math=ai_prefix_detail50IV::Math;

inline V read(Math& f,const float* a) noexcept {
    return {{f.read(a[0]),f.read(a[1]),f.read(a[2]),1.f}};
}
inline V sub(Math& f,const V& a,const V& b) noexcept {
    return {{f.sub(a.v[0],b.v[0]),f.sub(a.v[1],b.v[1]),
        f.sub(a.v[2],b.v[2]),1.f}};
}
inline V add(Math& f,const V& a,const V& b) noexcept {
    return {{f.add(a.v[0],b.v[0]),f.add(a.v[1],b.v[1]),
        f.add(a.v[2],b.v[2]),1.f}};
}
inline V scale(Math& f,const V& a,float k) noexcept {
    return {{f.mul(k,a.v[0]),f.mul(k,a.v[1]),f.mul(k,a.v[2]),1.f}};
}
inline float dot106E70(Math& f,const V& a,const V& b) noexcept {
    // VMUL.xyz; VADDw.x(0,0)=1; VADDAy.x; VMADDz.x.
    const float x=f.mul(a.v[0],b.v[0]),y=f.mul(a.v[1],b.v[1]),
        z=f.mul(a.v[2],b.v[2]);
    const float one=f.add(0.f,1.f),xy=f.add(x,y);
    return f.add(xy,f.mul(one,z));
}
inline float scalarLength(Math& f,const V& a) noexcept {
    //16F0A0/16F220/16F5E0/16F8E8: MUL X/X; MUL Y/Y;
    // ADDA X2,Y2; MADD Z,Z; SQRT.S. Separate binary32 operations.
    const float x=f.mul(a.v[0],a.v[0]),y=f.mul(a.v[1],a.v[1]);
    const float xy=f.add(x,y),n=f.add(xy,f.mul(a.v[2],a.v[2]));
    if(!f.valid||n<0.f){f.valid=false;return 0.f;}
    volatile float value=static_cast<float>(std::sqrt(static_cast<double>(n)));
    return f.read(value);
}
inline V normalizeInline(Math& f,const V& a) noexcept {
    //16EE44 and16F9C0 use VRSQRT, including max-finite Q for zero.
    // No epsilon, alternate direction, or ordinary-driving fallback.
    const float n=dot106E70(f,a,a);
    float q=std::numeric_limits<float>::max();
    if(n!=0.f){
        if(!f.valid||n<0.f){f.valid=false;return {};}
        volatile float root=std::sqrt(n);
        q=f.div(1.f,f.read(root));
    }
    return scale(f,a,q);
}
inline V cross(Math& f,const V& a,const V& b) noexcept {
    // VOPMULA.xyz then VOPMSUB.xyz. All three source lanes execute.
    const float x=f.mul(a.v[1],b.v[2]),y=f.mul(a.v[2],b.v[0]),
        z=f.mul(a.v[0],b.v[1]);
    return {{f.sub(x,f.mul(b.v[1],a.v[2])),
        f.sub(y,f.mul(b.v[2],a.v[0])),
        f.sub(z,f.mul(b.v[0],a.v[1])),0.f}};
}
inline float segmentDistance(Math& f,const V& p,const V& a,const V& b) noexcept {
    const V segment=sub(f,b,a),relative=sub(f,p,a);
    const float numerator=dot106E70(f,segment,relative);
    const float denominator=dot106E70(f,segment,segment);
    const float ratio=f.div(numerator,denominator);
    const float t=detail::scalarMax48O(detail::scalarMin48O(ratio,1.f),0.f);
    // The original scales all lanes before adding the endpoint and then
    // subtracts all lanes before its scalar length, including Y throughout.
    const V scaled=scale(f,segment,t),projection=add(f,scaled,a);
    return scalarLength(f,sub(f,p,projection));
}
inline float distanceXZSquared(Math& f,const V& p,const V& q) noexcept {
    const float x=f.sub(p.v[0],q.v[0]),z=f.sub(p.v[2],q.v[2]);
    const float xx=f.mul(x,x);return f.add(xx,f.mul(z,z));
}
} // namespace ai_attack_wide_detail50LH

// Complete native16EDD0..16FB44, reached by1711E0 mode3 when the current
// authored left54+right58 exceeds its433CC8 threshold. The caller owns that
// dispatch; this leaf never substitutes ordinary steering or performs speed.
//
// target is the actual1008 manager identity's current observation. Required
// additional observation fields: vehicleC84, up190[3], velocity70[3]. The
// original also copies forward190 through28A530, but never consumes that copy.
// position190/up190 are LOCAL scene190 F0/D0, not world pose or bodyScene288.
// selfPosition190/forward190/upY must have the same canonical LOCAL meaning.
//
// No dt is consumed by16EDD0 even though its source caller passes f12. Alpha
// is the actual lazy433CB8 owner, initialized to0x3DCCCCCD, never dt-scaled.
// field104C is the canonical mutable racer field; actions are the real controls.
// Geometry/owners and other records are stable, backed and mutually disjoint.
//
// Failure can retain a source-committed prefix: the lazy global stores and
//104C store occur before self-node/Next0 geometry and172E60. The caller must
// stop this frame and report d; it must NOT retry the whole leaf or call an
// ordinary-steering fallback. Strict finite normal-or-zero binary32 subset,
// FE_TONEAREST, no fast-math/contraction and gradual underflow are required.
// Native true means completed; original16FB3C returns integer0 unconditionally.
// This source-only proposal has not been compiled or runtime validated.
template<class Navigation>
inline bool aiAttackWideSteering16EDD0Shared50MB(
    const Navigation& source_track,const TrackNavigationProgress50EU& progress,
    const AiObservedRacer50IV& target,const float selfPosition190[3],
    const float selfForward190[3],float selfUpY,float selfCachedSpeed34,
    float selfEngineDifference5C,float selfAngularA4,std::uint32_t selfRacer60,
    AiAttackWideGlobals16EDD0& globals,float& field104C,
    PlayerReadResult48X& actions,AiDrivingDiagnostic50IV& d) noexcept {
    // MB admission is sealed at the shared entry, before source effects.
    if(!navigation_consumer_detail50MB::aiReady(source_track))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x16ee10u);

    const auto& track=navigation_consumer_detail50MB::read(source_track);
    using namespace ai_attack_wide_detail50LH;
    using ai_driving_detail50IV::fail;
    d={};
    if(std::fegetround()!=FE_TONEAREST)
        return fail(d,AiDrivingError50IV::Arithmetic,0x16edd0u);
    if(!target.bound||!selfPosition190||!selfForward190||!track.gatesPrepared||
       !track.track.count||track.track.count>navigation_consumer_detail50MB::capacity(track))
        return fail(d,AiDrivingError50IV::Owner,0x16ee10u);

    Math f;
    const V targetPosition=read(f,target.position190);
    const V targetUp=read(f,target.up190);
    const V velocity=normalizeInline(f,read(f,target.velocity70));
    const V selfPosition=read(f,selfPosition190);
    const V side=cross(f,targetUp,velocity); //16EE8C: up cross normalized velocity.
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x16eea0u);
    const std::uint32_t targetCurrent=target.vehicleC80,targetPrevious=target.vehicleC84;
    if(targetCurrent>=track.track.count||targetPrevious>=track.track.count)
        return fail(d,AiDrivingError50IV::Owner,0x16eee8u,
            targetCurrent>=track.track.count?targetCurrent:targetPrevious);
    const V targetCurrentPosition=read(f,track.track.nodes[targetCurrent].position);
    const V targetPreviousPosition=read(f,track.track.nodes[targetPrevious].position);
    const auto& currentGate=track.nodes[targetCurrent];
    const auto& previousGate=track.nodes[targetPrevious];

    //16EF38..16F234: two longitudinal edge segments, not either node's
    //cross-track gate and not a centerline/nearest-node approximation.
    const float edge10Distance=segmentDistance(f,targetPosition,
        read(f,currentGate.gate10),read(f,previousGate.gate10));
    const float edge20Distance=segmentDistance(f,targetPosition,
        read(f,currentGate.gate20),read(f,previousGate.gate20));
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x16f234u,targetCurrent);
    float sign=1.f;
    if(!(edge10Distance<40.f)){
        if(edge20Distance<40.f)sign=-1.f;
        else{
            //16F27C..16F474: each candidate is (target +/-40*side)+3*velocity.
            // Keep both signed multiplications, addition grouping and XZ-only
            // squared comparison. A strict negative-candidate win chooses-1;
            // a tie chooses+1. No random side choice occurs.
            const V negativeVelocity=scale(f,velocity,3.f);
            const V negativeSigned=scale(f,side,-1.f);
            const V negativeSide=scale(f,negativeSigned,40.f);
            const V negativeBase=add(f,targetPosition,negativeSide);
            const V negative=add(f,negativeBase,negativeVelocity);
            const V positiveVelocity=scale(f,velocity,3.f);
            const V positiveSigned=scale(f,side,1.f);
            const V positiveSide=scale(f,positiveSigned,40.f);
            const V positiveBase=add(f,targetPosition,positiveSide);
            const V positive=add(f,positiveBase,positiveVelocity);
            const float negativeDistance=distanceXZSquared(f,selfPosition,negative);
            const float positiveDistance=distanceXZSquared(f,selfPosition,positive);
            if(negativeDistance<positiveDistance)sign=-1.f;
            if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x16f474u,targetCurrent);
        }
    }

    //16F478..16F6E0: unsigned3D distance to target's C80/C84 center segment,
    //then the cross-Y sign and the CURRENT target node's authored side width.
    float signedDisplacement=segmentDistance(f,targetPosition,
        targetCurrentPosition,targetPreviousPosition);
    const V centerDirection=sub(f,targetCurrentPosition,targetPreviousPosition);
    const V fromPrevious=sub(f,targetPosition,targetPreviousPosition);
    const V targetSide=cross(f,centerDirection,fromPrevious);
    bool outsideWidth=false;
    if(targetSide.v[1]<0.f){
        outsideWidth=f.read(currentGate.left54)<signedDisplacement;
        signedDisplacement=f.mul(signedDisplacement,-1.f);
    }else outsideWidth=f.read(currentGate.right58)<signedDisplacement;
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x16f6e4u,targetCurrent);

    //16F6E4..16F750: these are actual stores, retained on any later failure.
    if(!globals.guardGP854C){
        globals.alpha433CB8=ai_speed_detail50IR::word(0x3dcccccdu); //16F6FC
        globals.guardGP854C=1u;                                 //16F704
    }
    const float old=f.read(field104C);
    float filtered=signedDisplacement;
    if(old!=-1.f){
        const float alpha=f.read(globals.alpha433CB8);
        const float inverse=f.sub(1.f,alpha),retained=f.mul(old,inverse);
        filtered=f.add(retained,f.mul(signedDisplacement,alpha));
    }
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x16f750u);
    field104C=filtered; //16F72C sentinel path or16F750 filtering path.

    //16F754..16F978: self C80 unless3D gate distance<cachedSpeed/8;
    //then the literal authored Next0. No171A90 route chooser or RNG call.
    std::uint32_t selected=progress.reset.vehicleC80;
    if(selected>=track.track.count)
        return fail(d,AiDrivingError50IV::Owner,0x16f75cu,selected);
    V center=read(f,track.track.nodes[selected].position);
    V gate10=read(f,track.nodes[selected].gate10);
    V gate20=read(f,track.nodes[selected].gate20);
    const float selfDistance=segmentDistance(f,selfPosition,gate10,gate20);
    const float horizon=f.div(selfCachedSpeed34,8.f);
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x16f928u,selected);
    if(selfDistance<horizon){
        const auto& node=track.track.nodes[selected];
        if(!node.nextCount)return fail(d,AiDrivingError50IV::Owner,0x2a26c0u,selected);
        selected=node.next[0];
        if(selected>=track.track.count)
            return fail(d,AiDrivingError50IV::Owner,0x16f954u,selected);
        center=read(f,track.track.nodes[selected].position);
        gate10=read(f,track.nodes[selected].gate10); //Source copies both gates.
        gate20=read(f,track.nodes[selected].gate20);
    }
    const V lateral=normalizeInline(f,sub(f,gate20,center));
    const float amount=f.add(f.mul(40.f,sign),field104C);
    V aim=add(f,center,scale(f,lateral,amount)); //16F9E8..16FA5C always executes.
    if(!outsideWidth){
        // Preserve the original's duplicate candidate arithmetic. Its first
        //candidate is overwritten on BOTH branches of16FA58.
        const float repeatedAmount=f.add(f.mul(40.f,sign),field104C);
        aim=add(f,center,scale(f,lateral,repeatedAmount));
    }else aim=center;
    const V direction=sub(f,aim,selfPosition);
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x16fb1cu,selected);

    //172E60 owns steering/raw0 and clears raw1 on its authentic success path.
    //No throttle, B9C, route-choice, mode1050 or target1008 write occurs here.
    // Admit its consumed supplied scalars at the actual call boundary, after
    //the source104C store. Forward Y is not consumed by172DF0's angle wrapper.
    f.read(selfForward190[0]);f.read(selfForward190[2]);f.read(selfUpY);
    f.read(selfEngineDifference5C);f.read(selfAngularA4);
    if(!f.valid)return fail(d,AiDrivingError50IV::Arithmetic,0x172e60u);
    return steerAi50IV(direction.v,selfForward190,selfUpY,selfCachedSpeed34,
        selfEngineDifference5C,selfAngularA4,selfRacer60,actions,d);
}
inline bool aiAttackWideSteering16EDD0(
    const TrackNavigation50EU& track,const TrackNavigationProgress50EU& progress,
    const AiObservedRacer50IV& target,const float selfPosition190[3],
    const float selfForward190[3],float selfUpY,float selfCachedSpeed34,
    float selfEngineDifference5C,float selfAngularA4,std::uint32_t selfRacer60,
    AiAttackWideGlobals16EDD0& globals,float& field104C,
    PlayerReadResult48X& actions,AiDrivingDiagnostic50IV& d) noexcept {
    return aiAttackWideSteering16EDD0Shared50MB(track,progress,target,selfPosition190,selfForward190,selfUpY,selfCachedSpeed34,selfEngineDifference5C,selfAngularA4,selfRacer60,globals,field104C,actions,d);
}
inline bool aiAttackWideSteering16EDD0(
    const NavigationView50CR& track,const TrackNavigationProgress50EU& progress,
    const AiObservedRacer50IV& target,const float selfPosition190[3],
    const float selfForward190[3],float selfUpY,float selfCachedSpeed34,
    float selfEngineDifference5C,float selfAngularA4,std::uint32_t selfRacer60,
    AiAttackWideGlobals16EDD0& globals,float& field104C,
    PlayerReadResult48X& actions,AiDrivingDiagnostic50IV& d) noexcept {
    if(!navigation_consumer_detail50MB::aiReady(track))
        return ai_driving_detail50IV::fail(d,AiDrivingError50IV::Owner,0x16ee10u);
    return aiAttackWideSteering16EDD0Shared50MB(track,progress,target,selfPosition190,selfForward190,selfUpY,selfCachedSpeed34,selfEngineDifference5C,selfAngularA4,selfRacer60,globals,field104C,actions,d);
}

} // namespace rr::game
