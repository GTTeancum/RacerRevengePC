#pragma once
// ============================================================================
// PC EXTENSION, NOT ORIGINAL BEHAVIOUR: rigid chariot mount from pod data.
//
// Original Revenge has no rigid chariot. Every pod's chariot (drawn root
// r+1B4) is published each frame by 21A660 from one of two movers:
//  * physical (r+838): the towed second body 35C (node r+19C) on the
//    PodSphere rope; mass, springs and rope stiffness are code constants
//    (218A3C..2190A0, 4346C8..434708), no PHY key reaches them;
//  * virtual (r+83C): the [Pod Animation] spring model in the pod frame
//    (container r+194), whose only centring force aims at the direction of
//    travel, not the pod's nose (21ABF4..21B07C);
//  * or a blend of both for duration848.
// So a craft that is one fixed airframe cannot be expressed in a stock PHY.
//
// This module adds ONE pod-file key, asked for on EVERY pod (no pod names,
// no course names), with the original's own key rule (158920: asked for by
// name, absent key -> the default, here 0):
//
//     [Pod Animation]
//     ChariotRigidMount = 1
//
// The original reader never asks for this key and no stock PHY carries it
// (23 stock PHYs checked, NOTES.md section 2), so every stock pod stays at
// the default and this module does nothing for it: no graph, body or log
// work at all, bit-for-bit the previous behaviour.
//
// When a pod's key is nonzero, for the human and the AI alike, and only while
// that racer is intact (vehicleState40 == 0), after the unchanged 21A660 has
// run (its mode selector, noise draws and spring state are untouched):
//  * the drawn chariot root r+1B4 is put at its authored rest pose on the pod
//    frame: container r+194 world (banked, see below), translated by
//    targetRelative2F0 (chariot nullcg minus main origin, 2125D8..2126F4).
//    The engine roots are published from the same container world in the same
//    stage, so chariot and engines move as one piece;
//
// Second key, same rule (absent = 0 = off), independent of the first:
//
//     [Pod Animation]
//     RigidBankFactor = 28
//
// While the racer is intact, the drawn airframe (both engine roots and, when
// held, the chariot) is turned about the pod frame's nose-to-tail axis through
// the pod origin by factor x turn rate (rad per 1/60 s), into the turn: the
// Racer Arcade rule for one-piece craft (work/scratch/racer-arcade/notes/
// nev-rigid.md 7.3). Picture only: physics bodies, the towed body's held pose
// and collision are not banked.
//  * while the towed body is the active mover (r+838 != 0) it is put on that
//    same pose with the original's own activation sequence (21BF00..21BF3C:
//    pose copy, 167A50 reset, main velocities), so its track contacts happen
//    where the cockpit is drawn and a later limp/wreck starts from there.
// Everything that reads r+1B4 follows: first-person camera, cables, pod draw,
// shadows, GC lights, water spray, limp/wreck seeding.
//
// Rigid mount ends by itself when the pod is no longer intact: states 1/2
// (human one-side loss), 3/4 (wreck) keep the original path.
//
// One place defines the rest pose (restPose()) and the per-slot table.
// Host-side state only. The graph writes go through the 49P setters on a
// scratch copy of the racer's own forest; every index is bounds-checked and
// every operand checked finite BEFORE anything is committed, and a hold that
// cannot be done is refused whole (nothing written, counted, logged once).
// ============================================================================
#include "../assets/original_phy_cursor50la.h"
#include "scene_graph49p.h"
#include "secondary_activation49v.h"
#include "vehicle_frame49w.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace rr::game {
// ---- THE ONE DEFAULT. --------------------------------------------------------
// Converted content behaves canonically to its source game through its own
// data: on for pods that ask for it. RR_CHARIOT_RIGID_MOUNT=0 switches the
// extension off globally (every pod then runs the original chariot).
inline constexpr bool chariotRigidMountDefaultEnabled50RM=true;

struct ChariotRigidMountSwitches50RM {
    bool enabled=chariotRigidMountDefaultEnabled50RM;
    bool poseLog=false;             // TEST ONLY: pose line for every racer
    std::array<bool,8> forced{};    // TEST ONLY: by race slot, never by pod
    std::optional<float> forcedBank; // TEST ONLY: RigidBankFactor for every slot
};
inline const ChariotRigidMountSwitches50RM& chariotRigidMountSwitches50RM() {
    static const ChariotRigidMountSwitches50RM switches=[]{
        ChariotRigidMountSwitches50RM out{};
        if(const char* v=std::getenv("RR_CHARIOT_RIGID_MOUNT")){
            if(!std::strcmp(v,"0"))out.enabled=false;else if(!std::strcmp(v,"1"))out.enabled=true;
        }
        // TEST ONLY: RR_TEST_CHARIOT_POSE_LOG=1 prints the once-per-second
        // chariot_pose line for all racers, rigid or not (stock comparison).
        if(const char* v=std::getenv("RR_TEST_CHARIOT_POSE_LOG"))out.poseLog=!std::strcmp(v,"1");
        // TEST ONLY: RR_TEST_FORCE_RIGID_MOUNT="all" or "0,3,7": treat these
        // RACE SLOTS as if their PHY carried ChariotRigidMount = 1, so a stock
        // pod can show the hold in muted logs-only runs. Implies enabled.
        if(const char* v=std::getenv("RR_TEST_FORCE_RIGID_MOUNT")){
            const std::string text(v);bool any=false,bad=false;
            if(text=="all"){out.forced.fill(true);any=true;}
            else for(std::size_t at=0;at<=text.size();){
                const auto comma=text.find(',',at);
                const auto item=text.substr(at,comma==std::string::npos?std::string::npos:comma-at);
                if(item.size()==1&&item[0]>='0'&&item[0]<='7'){out.forced[static_cast<std::size_t>(item[0]-'0')]=true;any=true;}
                else bad=true;
                if(comma==std::string::npos)break;
                at=comma+1;
            }
            if(bad){out.forced={};std::cerr<<"chariot_rigid_mount test_force rejected value="<<text<<'\n';}
            else if(any)out.enabled=true;
        }
        // TEST ONLY: RR_TEST_FORCE_RIGID_BANK=<factor> treats every race slot
        // as if its PHY carried RigidBankFactor = <factor> (stock pods carry none).
        if(const char* v=std::getenv("RR_TEST_FORCE_RIGID_BANK")){
            char* end=nullptr;const float factor=std::strtof(v,&end);
            if(end&&end!=v&&!*end&&std::isfinite(factor)&&factor>=0.f&&factor<=1000.f)out.forcedBank=factor;
            else std::cerr<<"chariot_rigid_bank test_force rejected value="<<v<<'\n';
        }
        return out;
    }();
    return switches;
}

struct NativeChariotRigidMount50RM {
    bool loaded=false;
    bool keyPresent=false;
    std::int32_t authored=0;       // the PHY value as read by 158920
    bool forced=false;             // test switch for this race slot
    bool active=false;             // (authored != 0 || forced) && enabled
    // RigidBankFactor ([Pod Animation], number; absent = 0 = off).
    bool bankKeyPresent=false;
    float bankFactor=0.f;          // applied factor (authored or test-forced); 0 = off
    // Host-side turn-rate state (drawn pod frame heading, previous frame).
    bool havePreviousForward=false;
    float previousForward[3]{};
    float turnRate=0.f;            // smoothed, rad/s toward the pod frame's right
    // Once-per-second log window (race time from the racer's own frame factor).
    float logClock=0.f,maxRawOffset=0.f,maxRawTilt=0.f;
    std::uint32_t frames=0,holds=0,refusals=0;
    bool refusalLogged=false;
};
// The one table. Filled at cold race activation from each racer's own PHY.
inline std::array<NativeChariotRigidMount50RM,8> nativeChariotRigidMounts50RM{};
inline void resetNativeChariotRigidMounts50RM() noexcept { nativeChariotRigidMounts50RM={}; }

// Refuses (false, slot left unloaded and inactive) when the slot is outside
// the table or the value is not something 158920 accepts. A missing section
// or key is the default 0, exactly as the original treats any unasked key.
inline bool loadNativeChariotRigidMount50RM(std::size_t slot,const std::vector<std::uint8_t>& phy) noexcept {
    if(slot>=nativeChariotRigidMounts50RM.size())return false;
    auto& entry=nativeChariotRigidMounts50RM[slot];entry={};
    std::string_view selected;
    rr::assets::OriginalPhyCursor50LA cursor(phy,selected);
    bool present=false;
    if(!cursor.select157D00("Pod Animation",present))return false;
    std::optional<std::int32_t> value;bool written=false;
    if(present&&!cursor.integer158920("ChariotRigidMount",value,written))return false;
    const auto& switches=chariotRigidMountSwitches50RM();
    entry.keyPresent=written&&value.has_value();
    entry.authored=entry.keyPresent?*value:0;
    entry.forced=switches.forced[slot];
    entry.active=switches.enabled&&(entry.authored!=0||entry.forced);
    std::optional<float> bank;
    if(present&&!cursor.float158A70("RigidBankFactor",bank))return false;
    if(bank&&(!std::isfinite(*bank)||*bank<0.f||*bank>1000.f))return false; // refused: outside the bounded factor
    entry.bankKeyPresent=bank.has_value();
    const float factor=switches.forcedBank?*switches.forcedBank:(bank?*bank:0.f);
    entry.bankFactor=switches.enabled?factor:0.f;
    entry.loaded=true;
    return true;
}

enum class ChariotRigidMountResult50RM : std::uint32_t { inactive, measured, held, refused };

namespace chariot_rigid_mount_detail50RM {
inline bool finite16(const BodyMatrix48G& m) noexcept {
    for(float lane:m.v)if(!std::isfinite(lane))return false;
    return true;
}
inline bool nodeIndex(SceneNodeIndex49P i,std::uint32_t count) noexcept {
    const auto n=static_cast<std::int32_t>(i);
    return n>=0&&static_cast<std::uint32_t>(n)<count;
}
// THE ONE REST POSE: the pod frame's own rotation, translated by the authored
// chariot offset. Same row convention as the cold placement (1AA27C suffix):
// rows 0/4/8 are right/up/forward, 12 is translation.
inline bool restPose(const BodyMatrix48G& container,const PodSphereVector49L& target,BodyMatrix48G& out) noexcept {
    if(!finite16(container))return false;
    for(unsigned i=0;i<3;++i)if(!std::isfinite(target.v[i]))return false;
    BodyMatrix48G rest=container;
    for(unsigned lane=0;lane<3;++lane){
        const float value=container.v[12+lane]+container.v[lane]*target.v[0]+
            container.v[4+lane]*target.v[1]+container.v[8+lane]*target.v[2];
        if(!std::isfinite(value))return false;
        rest.v[12+lane]=value;
    }
    out=rest;return true;
}
// A drawn part's world matrix moved with the craft from the pod frame to the
// banked frame: its axes and its offset from the pod origin keep the same
// coordinates in the frame they hang from (orthonormal pod axes).
inline bool withCraft(const BodyMatrix48G& pod,const BodyMatrix48G& banked,const BodyMatrix48G& in,BodyMatrix48G& out) noexcept {
    if(!finite16(in))return false;
    BodyMatrix48G moved=in;
    for(unsigned row=0;row<4;++row){
        float d[3];
        for(unsigned i=0;i<3;++i)d[i]=row<3?in.v[4*row+i]:in.v[12+i]-pod.v[12+i];
        float c[3];
        for(unsigned k=0;k<3;++k)c[k]=d[0]*pod.v[4*k]+d[1]*pod.v[4*k+1]+d[2]*pod.v[4*k+2];
        for(unsigned i=0;i<3;++i)
            moved.v[4*row+i]=(row<3?0.f:pod.v[12+i])+c[0]*banked.v[i]+c[1]*banked.v[4+i]+c[2]*banked.v[8+i];
    }
    if(!finite16(moved))return false;
    out=moved;return true;
}
// THE ONE BANKED FRAME: the pod frame turned about its own forward
// (nose-to-tail) axis by `radians`, same origin; positive tips the top toward
// the pod frame's right axis. The craft leans as one piece about the pod
// frame origin, so the authored chariot offset turns too.
inline bool bankedFrame(const BodyMatrix48G& container,float radians,BodyMatrix48G& out) noexcept {
    if(!finite16(container)||!std::isfinite(radians))return false;
    const float c=std::cos(radians),s=std::sin(radians);
    BodyMatrix48G banked=container;
    for(unsigned i=0;i<3;++i){
        banked.v[i]=c*container.v[i]-s*container.v[4+i];
        banked.v[4+i]=c*container.v[4+i]+s*container.v[i];
    }
    if(!finite16(banked))return false;
    out=banked;return true;
}
// Chariot pose relative to the rest pose, in pod-frame axes: offset in feet
// and the larger of the forward/up axis angles in degrees.
inline bool deviation(const BodyMatrix48G& container,const BodyMatrix48G& rest,const BodyMatrix48G& chariot,
        float offset[3],float& tiltDegrees) noexcept {
    if(!finite16(chariot))return false;
    float delta[3];for(unsigned i=0;i<3;++i)delta[i]=chariot.v[12+i]-rest.v[12+i];
    for(unsigned axis=0;axis<3;++axis){
        offset[axis]=delta[0]*container.v[4*axis]+delta[1]*container.v[4*axis+1]+delta[2]*container.v[4*axis+2];
        if(!std::isfinite(offset[axis]))return false;
    }
    float worst=0.f;
    for(unsigned axis=1;axis<3;++axis){
        float dot=0.f,a=0.f,b=0.f;
        for(unsigned i=0;i<3;++i){
            dot+=chariot.v[4*axis+i]*container.v[4*axis+i];
            a+=chariot.v[4*axis+i]*chariot.v[4*axis+i];b+=container.v[4*axis+i]*container.v[4*axis+i];
        }
        const float scale=std::sqrt(a*b);
        if(!(scale>0.f)||!std::isfinite(scale))return false;
        float cosine=dot/scale;if(cosine>1.f)cosine=1.f;if(cosine<-1.f)cosine=-1.f;
        const float degrees=std::acos(cosine)*57.29578f;
        if(degrees>worst)worst=degrees;
    }
    tiltDegrees=worst;return true;
}
}

// Call once per racer per frame, directly after graphicsVehicleFrame49W has
// committed (21A660 returned), with that racer's own records. Returns `held`
// when the racer's graph was changed: the caller must then republish the
// frame's scene mirrors exactly as the stage itself does (its project()).
// `inactive` did no work at all. `refused` wrote nothing.
inline ChariotRigidMountResult50RM afterChariotLifecycle50RM(std::size_t slot,VehicleFrameState49W& s,
        const VehicleFrameBindings49W& b,const SecondaryActivationGlobals49V& activation,
        const PodSphereVector49L& targetRelative2F0) noexcept {
    using namespace chariot_rigid_mount_detail50RM;
    using R=ChariotRigidMountResult50RM;
    if(slot>=nativeChariotRigidMounts50RM.size())return R::inactive;
    auto& entry=nativeChariotRigidMounts50RM[slot];
    const auto& switches=chariotRigidMountSwitches50RM();
    const bool banking=entry.bankFactor>0.f;
    if(!entry.loaded||(!entry.active&&!banking&&!switches.poseLog))return R::inactive;
    const bool intact=s.bodies.main.vehicleState40==0u;
    const bool hold=entry.active&&intact;
    const bool bankNow=banking&&intact;
    const auto refuse=[&](const char* reason){
        ++entry.refusals;
        if(!entry.refusalLogged){entry.refusalLogged=true;
            std::cerr<<"chariot_rigid_mount refused slot="<<slot<<" reason="<<reason<<'\n';}
        return R::refused;
    };
    const auto& l=b.lifecycle;
    const std::uint32_t count=s.sceneNodeCount;
    if(!count||count>vehicleFrameNodeCount49W)return refuse("node_count");
    for(const auto index:{l.main190,l.container194,l.physical19C,l.model1B4})
        if(!nodeIndex(index,count))return refuse("node_index");
    if(std::fegetround()!=FE_TONEAREST)return refuse("rounding_mode");
    // All graph work happens on a scratch copy of this racer's own forest.
    SceneNode49P scratch[vehicleFrameNodeCount49W];
    std::memcpy(scratch,s.nodes,count*sizeof(SceneNode49P));
    const SceneGraph49P g{scratch,count};
    const auto at=[&](SceneNodeIndex49P i)->SceneNode49P&{return scratch[static_cast<std::uint32_t>(i)];};
    if(at(l.model1B4).parent180!=SceneNodeIndex49P::none||at(l.physical19C).parent180!=SceneNodeIndex49P::none)
        return refuse("root_topology");
    const auto& v=b.visual;
    if(!nodeIndex(v.right1B8,count)||!nodeIndex(v.left1BC,count))return refuse("engine_index");
    if(!refreshSceneWorld49P(g,l.container194)||!refreshSceneWorld49P(g,l.model1B4)||
       !refreshSceneWorld49P(g,v.right1B8)||!refreshSceneWorld49P(g,v.left1BC))return refuse("refresh");
    const BodyMatrix48G pod=at(l.container194).world100;
    if(!finite16(pod))return refuse("pod_frame");
    // ---- RigidBankFactor (PORT EXTENSION, arcade rule, nev-rigid.md 7.3/7.4) --
    // The drawn pod frame (container 194) follows the main body and the ground
    // only; it has no turn bank. The arcade rolls the whole airframe about the
    // pod origin by 28 x yaw rate (rad per 1/60 s), into the turn, with the
    // rate smoothed 0.5 per 1/60 s frame and no clamp. Here: the rate is the
    // drawn pod frame's own heading change toward its right axis per second
    // (same for player and AI), smoothed by 0.5^(dt*60) so the lean is the
    // same at any frame rate; bank = factor x rate / 60.
    // PORT STAND-IN (flagged): the arcade's rate is its NPC steering integrator
    // (torque/400 per frame); Revenge has none, so the measured heading rate of
    // the drawn frame stands in for it.
    const float dt=s.bodies.main.frameFactor;
    float bank=0.f;
    if(bankNow&&std::isfinite(dt)&&dt>0.f){
        float measured=0.f;
        if(entry.havePreviousForward){
            float toward=0.f;
            for(unsigned i=0;i<3;++i)toward+=(pod.v[8+i]-entry.previousForward[i])*pod.v[i];
            measured=toward/dt;
        }
        const float keep=std::pow(0.5f,dt*60.f);
        const float rate=keep*entry.turnRate+(1.f-keep)*measured;
        if(std::isfinite(rate))entry.turnRate=rate;
        bank=entry.bankFactor*entry.turnRate/60.f;
        if(!std::isfinite(bank))return refuse("bank");
    }else if(!bankNow){entry.turnRate=0.f;}
    for(unsigned i=0;i<3;++i)entry.previousForward[i]=pod.v[8+i];
    entry.havePreviousForward=bankNow;
    BodyMatrix48G container=pod;
    if(bank!=0.f&&!bankedFrame(pod,bank,container))return refuse("bank");
    BodyMatrix48G rest{},bodyRest{};
    if(!restPose(container,targetRelative2F0,rest))return refuse("rest_pose");
    // Picture only: the towed physics body keeps the unbanked rest pose.
    if(!restPose(pod,targetRelative2F0,bodyRest))return refuse("rest_pose");
    // What the original movers produced this frame, before any hold.
    float rawOffset[3]{};float rawTilt=0.f;
    if(!deviation(container,rest,at(l.model1B4).world100,rawOffset,rawTilt))return refuse("measure");
    const std::uint32_t mode838=s.bodies.vehicle838,mode83C=s.lifecycle.vehicle83C;
    R result=R::measured;
    float heldOffset[3]={rawOffset[0],rawOffset[1],rawOffset[2]};float heldTilt=rawTilt;
    const bool bankEngines=bank!=0.f;
    if(hold||bankEngines){
        const bool body=hold&&mode838!=0u;
        // Engine roots are parentless and rebuilt every frame by 21C120 from
        // their own response records times the pod frame, so turning them here
        // changes only this frame's picture, never the next frame's springs.
        if(bankEngines)for(const auto engine:{v.right1B8,v.left1BC}){
            if(at(engine).parent180!=SceneNodeIndex49P::none)return refuse("engine_topology");
            BodyMatrix48G moved{};
            if(!withCraft(pod,container,at(engine).world100,moved)||!setLocalMatrix49P(g,engine,moved)||
               !refreshSceneWorld49P(g,engine))return refuse("set_engine");
        }
        if(hold&&(!setLocalMatrix49P(g,l.model1B4,rest)||!refreshSceneWorld49P(g,l.model1B4)))return refuse("set_model");
        if(body&&(!setLocalMatrix49P(g,l.physical19C,bodyRest)||!refreshSceneWorld49P(g,l.physical19C)))
            return refuse("set_body");
        if(!deviation(container,rest,at(l.model1B4).world100,heldOffset,heldTilt))return refuse("verify");
        // Commit: nothing above touched the racer's records.
        std::memcpy(s.nodes,scratch,count*sizeof(SceneNode49P));
        if(body){
            // 21BF08..21BF3C on the rest pose: the towed body's scene view is
            // node 19C itself; 167A50 reset; inherit the main body's velocities.
            const auto& node=s.nodes[static_cast<std::uint32_t>(l.physical19C)];
            s.bodies.secondary.root.scene=node.world100;
            s.bodies.secondary.sceneFlags190=(s.bodies.secondary.sceneFlags190&0xffffff00u)|node.sceneFlags190;
            resetSecondaryActivation49V(s.bodies.secondary,s.secondarySupplement,activation);
            copySecondaryMainVelocities49V(s.bodies.main.components.root,s.bodies.mainSphere.velocity70W,
                s.bodies.mainSphere.angularVelocityB0W,s.bodies.secondary);
        }
        if(hold)++entry.holds;
        result=R::held; // graph changed: caller republishes the stage mirrors
    }
    // ---- once-per-second logs-only line (race time, not wall time) ----------
    const float rawDistance=std::sqrt(rawOffset[0]*rawOffset[0]+rawOffset[1]*rawOffset[1]+rawOffset[2]*rawOffset[2]);
    if(rawDistance>entry.maxRawOffset)entry.maxRawOffset=rawDistance;
    if(rawTilt>entry.maxRawTilt)entry.maxRawTilt=rawTilt;
    ++entry.frames;
    if(std::isfinite(dt)&&dt>0.f)entry.logClock+=dt;
    if(entry.logClock>=1.f){
        std::cerr<<"chariot_pose slot="<<slot<<" ai="<<(s.fields.vehicle58!=0u)<<" key="<<entry.authored
            <<" forced="<<entry.forced<<" rigid="<<entry.active<<" state40="<<s.bodies.main.vehicleState40
            <<" held="<<(result==R::held&&hold)<<" banked="<<(result==R::held&&bankEngines)<<" mode838="<<mode838<<" mode83C="<<mode83C
            <<" speed="<<s.bodies.main.cachedSpeed34
            <<" pod_roll_deg="<<std::asin(std::fmax(-1.f,std::fmin(1.f,pod.v[1])))*57.29578f
            <<" bank_factor="<<entry.bankFactor<<" turn_rate_dps="<<entry.turnRate*57.29578f
            <<" bank_deg="<<bank*57.29578f
            <<" raw_off="<<rawOffset[0]<<','<<rawOffset[1]<<','<<rawOffset[2]<<" raw_tilt_deg="<<rawTilt
            <<" off="<<heldOffset[0]<<','<<heldOffset[1]<<','<<heldOffset[2]<<" tilt_deg="<<heldTilt
            <<" max_raw_off_1s="<<entry.maxRawOffset<<" max_raw_tilt_1s="<<entry.maxRawTilt
            <<" frames="<<entry.frames<<" holds="<<entry.holds<<" refusals="<<entry.refusals<<'\n';
        entry.logClock-=1.f;if(entry.logClock>=1.f)entry.logClock=0.f;
        entry.maxRawOffset=entry.maxRawTilt=0.f;entry.frames=0;entry.holds=0;
    }
    return result;
}
} // namespace rr::game
