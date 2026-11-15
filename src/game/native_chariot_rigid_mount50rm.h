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
//    frame: container r+194 world, translated by targetRelative2F0 (chariot
//    nullcg minus main origin, 2125D8..2126F4), no extra rotation. The engine
//    roots are published from the same container world in the same stage, so
//    chariot and engines move as one piece;
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
    if(!entry.loaded||(!entry.active&&!switches.poseLog))return R::inactive;
    const bool intact=s.bodies.main.vehicleState40==0u;
    const bool hold=entry.active&&intact;
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
    if(!refreshSceneWorld49P(g,l.container194)||!refreshSceneWorld49P(g,l.model1B4))return refuse("refresh");
    const BodyMatrix48G container=at(l.container194).world100;
    BodyMatrix48G rest{};
    if(!restPose(container,targetRelative2F0,rest))return refuse("rest_pose");
    // What the original movers produced this frame, before any hold.
    float rawOffset[3]{};float rawTilt=0.f;
    if(!deviation(container,rest,at(l.model1B4).world100,rawOffset,rawTilt))return refuse("measure");
    const std::uint32_t mode838=s.bodies.vehicle838,mode83C=s.lifecycle.vehicle83C;
    R result=R::measured;
    float heldOffset[3]={rawOffset[0],rawOffset[1],rawOffset[2]};float heldTilt=rawTilt;
    if(hold){
        const bool body=mode838!=0u;
        if(!setLocalMatrix49P(g,l.model1B4,rest)||!refreshSceneWorld49P(g,l.model1B4))return refuse("set_model");
        if(body&&(!setLocalMatrix49P(g,l.physical19C,rest)||!refreshSceneWorld49P(g,l.physical19C)))
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
        ++entry.holds;result=R::held;
    }
    // ---- once-per-second logs-only line (race time, not wall time) ----------
    const float rawDistance=std::sqrt(rawOffset[0]*rawOffset[0]+rawOffset[1]*rawOffset[1]+rawOffset[2]*rawOffset[2]);
    if(rawDistance>entry.maxRawOffset)entry.maxRawOffset=rawDistance;
    if(rawTilt>entry.maxRawTilt)entry.maxRawTilt=rawTilt;
    ++entry.frames;
    const float dt=s.bodies.main.frameFactor;
    if(std::isfinite(dt)&&dt>0.f)entry.logClock+=dt;
    if(entry.logClock>=1.f){
        std::cerr<<"chariot_pose slot="<<slot<<" ai="<<(s.fields.vehicle58!=0u)<<" key="<<entry.authored
            <<" forced="<<entry.forced<<" rigid="<<entry.active<<" state40="<<s.bodies.main.vehicleState40
            <<" held="<<(result==R::held)<<" mode838="<<mode838<<" mode83C="<<mode83C
            <<" speed="<<s.bodies.main.cachedSpeed34
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
