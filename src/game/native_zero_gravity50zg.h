#pragma once
// ============================================================================
// PC ENHANCEMENT, NOT ORIGINAL BEHAVIOUR: N64 Racer zero gravity (Oovo IV) as
// a pod-physics mode of the one shared racer step (player and AI).
// Spec: work\scratch\racer-n64\notes\n64-zero-g-physics.md.
// Design: work\scratch\zero-g\NOTES.md.
//
// Data: the TerrainZone row key ZeroGravity (0 none, 1 on, 2 off, 3 both),
// read by the common reader (native_surface_traction50st.h), plus the course's
// own racing line (its map and AI .spl, read with the port's original 2A1260
// reader so node numbers match the live progress words) and an optional new
// per-node key LineHeight (ft of the N64 racing line above the node, default
// 0). Every stock course has no ZeroGravity value, so nothing here is built
// and every racer stays Normal: the original path runs unchanged.
//
// Per racer, host-side: mode Normal / Flight (N64 ZON) / Recover (N64 ZOFF),
// the spline-follow lookahead, the zero-g pitch and roll rates. The physics
// step publishes one ZeroGravityStep50ZG (native_zero_gravity_step50zg.h) for
// the frame code; nothing writes game or guest memory except through the
// frame's own scene setters, inside that racer's own frame state.
//
// Frame rate: the N64 per-frame constants (forward blend 0.2, roll decay 0.98,
// lookahead step 0.01) use the reference rate f0 below (UNSETTLED, as the ice
// note: 30). Everything else is per second already.
// ============================================================================
#include "native_surface_traction50st.h"
#include "native_zero_gravity_step50zg.h"
#include "n64_floor_table50nh.h" // PORT EXTENSION: N64 floor table (.nft) as the table source
#include "undertrack_reposition50et.h"
#include "body_scene48g.h"
#include "../assets/collision_mesh48i.h"
#include "../assets/track_start50ad.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace rr::game {
// ---- N64 constants (n64-zero-g-physics.md sections 2, 3, 5) -----------------
inline constexpr float zeroGReferenceFrameRate50ZG=30.f;  // f0 = 30, settled: designers' reference
inline constexpr float zeroGForwardBlendPerFrame50ZG=0.2f; // 0x8006B0B0
inline constexpr float zeroGRollDecayPerFrame50ZG=0.98f;   // 0x800AD5E0
inline constexpr float zeroGRollLimitDegrees50ZG=180.f;    // 0x800AD5D0 / 5D8
inline constexpr float zeroGTurnRollGain50ZG=-2.f;         // r += -2 turnRateTarget dt
inline constexpr float zeroGTiltRollGain50ZG=1000.f;       // r += 1000 tilt dt
inline constexpr float zeroGRotationScale50ZG=1.5f;        // yaw/pitch x1.5 (0x80017824 caller)
inline constexpr float zeroGLookAheadMin50ZG=0.01f;        // 0x800AD5BC
inline constexpr float zeroGLookAheadMax50ZG=2.0f;
inline constexpr float zeroGLookAheadStepPerFrame50ZG=0.01f; // 0x800AD5C4
inline constexpr float zeroGLookAheadNearSquared50ZG=18000.f; // 0x800AD5C8 (134 ft)
inline constexpr float zeroGLookAheadFarSquared50ZG=90000.f;  // 0x800AD5CC (300 ft)
inline constexpr float zeroGLookAheadInitial50ZG=0.5f;       // UNSETTLED: N64 shares +0x104 with the autopilot
inline constexpr float zeroGLandingDistance50ZG=12.f;        // 0x4140 (0x8006C648..67C)
inline constexpr float zeroGRecoverRollGain50ZG=1.25f;       // 10 x 0.125 (0x8006B0D8, 0x80065E54)
inline constexpr float zeroGPitchDeadZone50ZG=0.05f;         // 0x800AD794 / 798
inline constexpr float zeroGPitchStickScale50ZG=0.8f;
inline constexpr float zeroGPitchTargetScale50ZG=0.5f;
// Settled (n64-zero-g-physics.md section 9): Revenge pods are not N64 racers,
// so the N64 medians are used; a finished pod uses the N64 finished values.
// The N64 AI pace system is not ported (Revenge's own AI pacing stays).
inline constexpr float zeroGReferenceMaxTurnRate50ZG=90.f;   // +0x74, deg/s
inline constexpr float zeroGReferenceTurnResponse50ZG=250.f; // +0x70, deg/s per s
inline constexpr float zeroGFinishedMaxTurnRate50ZG=400.f;
inline constexpr float zeroGFinishedTurnResponse50ZG=1500.f;
// Tilt (N64): target -1/0/+1 from the dedicated buttons, forced 0 below speed
// 200, ramps at 3.2/s; released and |tilt| < 0.1 halves per N64 frame.
inline constexpr float zeroGTiltMinimumSpeed50ZG=200.f;
inline constexpr float zeroGTiltRampPerSecond50ZG=3.2f;
inline constexpr float zeroGTiltReleaseHalvingBelow50ZG=0.1f;
// THE ONE tilt-input mapping point (user decision 2026-11-16). PC ENHANCEMENT,
// NOT ORIGINAL: Revenge has no tilt control. The player's lean channel is the
// right stick X of the default control preset (packet byte 4; the original
// reader 1A1A20 conditions it into channel 2, which no race consumer of preset
// 0 reads) and, on the keyboard, A (left) / D (right) written to that same
// byte. lean > 0 = right. Digital like the N64 tilt buttons: |lean| >= 0.5
// selects -1/+1. Sign: leaning right rolls the same way as steering right
// (roll += -2 x turn, so right = tilt -1). AI never leans. Only read in flight.
inline constexpr float zeroGLeanThreshold50ZG=0.5f;
inline constexpr float zeroGTiltInputTarget50ZG(bool human,float lean)noexcept{
    return !human||!(lean==lean)?0.f:lean>=zeroGLeanThreshold50ZG?-1.f:lean<=-zeroGLeanThreshold50ZG?1.f:0.f;}
inline constexpr float zeroGBakeRayLength50ZG=10000.f;       // N64 down ray reach
inline constexpr std::uint32_t zeroGSamplesPerLink50ZG=10;   // swrSpline_BakeSampleTrackMesh density
inline constexpr float zeroGLineHeightLimit50ZG=5000.f;
inline constexpr float zeroGPi50ZG=3.14159265358979f;

// ---- Racing-line table: the ONE place its layout is defined. ----------------
struct ZeroGravityLineSample50ZG {
    std::uint32_t owner=noSurfaceOwner50ST; // static owner index under the line
    float floorY=0.f;
    float normal[3]{0.f,1.f,0.f};
};
struct ZeroGravityLineGraph50ZG {
    struct Node {float line[3]{};std::uint32_t next[maxUndertrackNext50ET]{};std::uint32_t nextCount=0;std::uint32_t sampleBase=0;};
    std::vector<Node> nodes;
    std::vector<ZeroGravityLineSample50ZG> samples; // node.sampleBase + nextSlot*10 + k
    std::size_t hits=0;
    bool ready()const noexcept {return !nodes.empty();}
};
enum class ZeroGravityGraph50ZG : std::uint8_t {Map=0,Ai=1};

struct ZeroGravityRacer50ZG {
    ZeroGravityMode50ZG mode=ZeroGravityMode50ZG::Normal;
    float lookAhead=zeroGLookAheadInitial50ZG; // spline parameter ahead of the cursor (+0x104)
    float pitchTarget=0.f,pitchRate=0.f;       // +0x224, +0x220 (deg/s, before x1.5)
    float rollRate=0.f;                        // +0x21C (deg/s)
    float tilt=0.f;                            // N64 tilt state (-1..1)
    std::uint32_t lineOwner=noSurfaceOwner50ST;
    std::uint64_t steps=0,modeStep=0;
    // Flight summary (logs only).
    float minSpeed=0.f,maxAbsRoll=0.f,maxAbsPitch=0.f,minAhead=0.f,maxAhead=0.f;
    std::uint64_t flightSteps=0,steerPinnedSteps=0; // |steer| >= 0.999 in Flight (logs only)
    float maxLineOffset=0.f,maxUpSlip=0.f;          // ft off the line, deg of travel off the nose along up (logs only)
    bool linkMissingLogged=false;
};
struct ZeroGravityStepInput50ZG {
    bool human=false;              // vehicle +58 == 0 (the original's own branch)
    std::uint32_t from=0,to=0;     // progress words: human C8C->C88 (map), AI C84->C80 (AI)
    float position[3]{};           // main190 world translation
    float steer=0.f;               // the racer's own steering channel (raw values[0])
    float pitchStick=0.f;          // human: bound stick Y (aeroAux50); AI: ignored
    float lean=0.f;                // human: zero-g lean channel (-1..1, + = right); 0 when unbound
    float speed=0.f;               // cached speed (+34), gates the tilt
    bool finished=false;           // vehicle +CD8 bit0
    float dt=0.f;
    // Logs only (flight samples): world velocity and the main190 basis rows
    // (right, up, forward). Absent (haveMotion false) -> no slip figures.
    bool haveMotion=false;
    float velocity[3]{},right[3]{},up[3]{},forward[3]{};
};
// Flight sample period in racer steps (logs only, about 0.5 s at 62.5 Hz).
inline constexpr std::uint64_t zeroGFlightSampleSteps50ZG=31;

class ZeroGravity50ZG {
public:
    // Cold, once per course load, after the surface rows are loaded. owners =
    // the same static collision owners the reader numbered; each has .mesh and
    // .instance.world. Builds nothing (and stays inert) unless the course has
    // a ZeroGravity "on" owner and the switch is on.
    template<class Owners>
    void loadCourse(const std::vector<std::uint8_t>& archiveBytes,const char* mapMember,const char* aiMember,
        const Owners& owners,const SurfaceRules50ST& rules,const char* floorMember=nullptr)noexcept {
        resetCourse();
        if(!rules.zeroGravityEnabled()||!rules.courseHasZeroGravity())return;
        try {
            if(owners.size()!=rules.ownerCount()){
                std::cerr<<"zero_gravity_load refused owners="<<owners.size()<<" rows="<<rules.ownerCount()<<'\n';return;}
            // World-space triangles of every static owner, once (host memory).
            std::vector<Mesh> meshes(owners.size());
            for(std::size_t k=0;k<owners.size();++k)meshes[k]=worldMesh(owners[k].mesh,owners[k].instance.world);
            assets::ResArchive48C archive;assets::DecodeError48C error;
            if(!assets::decodeResource48C(archiveBytes,archive,error)){std::cerr<<"zero_gravity_load refused archive\n";return;}
            bool ok=true;
            ok=readGraph(archive,mapMember,meshes,graphs_[0],"map")&&ok;
            ok=readGraph(archive,aiMember,meshes,graphs_[1],"ai")&&ok;
            if(!ok){resetCourse();std::cerr<<"zero_gravity_load failed; course stays original\n";return;}
            // PORT EXTENSION, NOT ORIGINAL (n64_floor_table50nh.h): the track's own N64
            // floor table replaces the line sampling's owner per sample when present;
            // absent or refused = the line sampling below (the log line says which).
            floor_.load(archive,floorMember,graphs_,rules);
            loaded_=true;
            std::cerr<<"zero_gravity_load map_nodes="<<graphs_[0].nodes.size()<<" map_samples="<<graphs_[0].samples.size()
                <<" map_hits="<<graphs_[0].hits<<" ai_nodes="<<graphs_[1].nodes.size()<<" ai_samples="<<graphs_[1].samples.size()
                <<" ai_hits="<<graphs_[1].hits<<'\n';
        }catch(...){resetCourse();std::cerr<<"zero_gravity_load failed; course stays original\n";}
    }
    bool active()const noexcept {return loaded_;}
    // N64 floor table bound to the line graphs (empty when the course has no .nft).
    const N64FloorTableOwner50NH& floorTable()const noexcept {return floor_;}
    // The player's lean channel is bound only in the default control preset
    // (set once per race session by the input binding; host state, no guest memory).
    void setHumanLeanBound(bool bound)noexcept {humanLeanBound_=bound;}
    bool humanLeanBound()const noexcept {return humanLeanBound_;}
    float tilt(std::size_t slot)const noexcept {return slot<zeroGravitySlots50ZG?racers_[slot].tilt:0.f;}
    ZeroGravityMode50ZG mode(std::size_t slot)const noexcept {
        return slot<zeroGravitySlots50ZG?racers_[slot].mode:ZeroGravityMode50ZG::Normal;}
    std::uint32_t lineOwner(std::size_t slot)const noexcept {
        return slot<zeroGravitySlots50ZG?racers_[slot].lineOwner:noSurfaceOwner50ST;}

    // Line owner the surface rule must use for the step being run.
    std::uint32_t stepLineOwner(std::size_t slot)const noexcept {
        if(slot>=zeroGravitySlots50ZG)return noSurfaceOwner50ST;
        return pendingValid_[slot]?pending_[slot].lineOwner:racers_[slot].lineOwner;}

    // BEFORE the racer's physics step. Returns the context for this step
    // (mode Normal when inactive: the caller then changes nothing). Computes
    // the host-side rates in the N64 order (control: pitch target; move step:
    // slew and roll accumulation) into a PENDING copy that afterStep commits,
    // so a step the frame abandons and re-runs cannot advance them twice.
    ZeroGravityStep50ZG prepareStep(std::size_t slot,const ZeroGravityStepInput50ZG& in)noexcept {
        ZeroGravityStep50ZG out{};
        if(!loaded_||slot>=zeroGravitySlots50ZG||!std::isfinite(in.dt)||!(in.dt>0.f)||in.dt>0.25f)return out;
        pending_[slot]=racers_[slot];pendingValid_[slot]=true;
        auto& r=pending_[slot];
        const auto& g=graphs_[in.human?0:1];
        // Floor under the line at this racer's progress (needed in every mode:
        // Flight reads its surface there; Recover rights itself to it when the
        // probe sees no ground).
        Cursor c{};
        if(!cursor(g,in.from,in.to,in.position,c)){
            r.lineOwner=noSurfaceOwner50ST;
            if(!r.linkMissingLogged&&r.mode!=ZeroGravityMode50ZG::Normal){r.linkMissingLogged=true;
                std::cerr<<"zero_gravity line_link_missing slot="<<slot<<" from="<<in.from<<" to="<<in.to<<'\n';}
        }else{
            floor_.apply(slot,in.human?0u:1u,c.from,c.slot,c.t,c.sample,floorSample_[slot]); // .nft when bound
            r.lineOwner=c.sample?c.sample->owner:noSurfaceOwner50ST;}
        if(r.mode==ZeroGravityMode50ZG::Normal||!c.valid)return out;
        const float frames=zeroGReferenceFrameRate50ZG*in.dt;
        // f0 = 30 is the designers' reference (the N64 used measured frame
        // time, unclamped). Finished pods use the N64 finished rates.
        const float maxTurnRate=in.finished?zeroGFinishedMaxTurnRate50ZG:zeroGReferenceMaxTurnRate50ZG;
        const float turnResponse=in.finished?zeroGFinishedTurnResponse50ZG:zeroGReferenceTurnResponse50ZG;
        // Lookahead: keep the cursor-to-lookahead distance in 134..300 ft by
        // stepping the parameter 0.01 per N64 frame (0x8006AF48 step 1).
        float ahead[3]{};lineAt(g,c,r.lookAhead,ahead);
        const float dx=ahead[0]-c.point[0],dy=ahead[1]-c.point[1],dz=ahead[2]-c.point[2];
        const float d2=dx*dx+dy*dy+dz*dz;
        if(d2<zeroGLookAheadNearSquared50ZG)r.lookAhead+=zeroGLookAheadStepPerFrame50ZG*frames;
        else if(d2>zeroGLookAheadFarSquared50ZG)r.lookAhead-=zeroGLookAheadStepPerFrame50ZG*frames;
        r.lookAhead=std::clamp(r.lookAhead,zeroGLookAheadMin50ZG,zeroGLookAheadMax50ZG);
        lineAt(g,c,r.lookAhead,out.lookAhead);
        if(r.mode==ZeroGravityMode50ZG::Flight){
            // Human-only pitch (N64 0x8006F244..314): only the player's control
            // read writes the target; AI targets stay 0.
            r.pitchTarget=0.f;
            if(in.human&&std::isfinite(in.pitchStick)){
                const float p=std::clamp(zeroGPitchStickScale50ZG*in.pitchStick,-zeroGPitchStickScale50ZG,zeroGPitchStickScale50ZG);
                if(std::fabs(p)>=zeroGPitchDeadZone50ZG){const float q=p/zeroGPitchStickScale50ZG;
                    r.pitchTarget=(p<0.f?1.f:-1.f)*zeroGPitchStickScale50ZG*q*q*maxTurnRate*zeroGPitchTargetScale50ZG;}
            }
            // Slew at turnResponse x pace per second (0x8006B884..92C), only in ZON.
            const float slew=turnResponse*in.dt; // pace 1 (N64 AI pace not ported)
            r.pitchRate+=std::clamp(r.pitchTarget-r.pitchRate,-slew,slew);
            // Roll (0x8006B0F0..1B8): r += (-2 turnRateTarget + 1000 tilt) dt,
            // clamp 180, then x0.98 per N64 frame. turnRateTarget = steer x
            // maxTurnRate x pace. Tilt from the one mapping point (lean input).
            const float turnTarget=std::clamp(std::isfinite(in.steer)?in.steer:0.f,-1.f,1.f)*maxTurnRate;
            {   // Tilt state (N64): target from the one mapping point, 0 below
                // speed 200; ramp 3.2/s; released and small: halves per frame.
                const float speed=std::isfinite(in.speed)?in.speed:0.f;
                const float target=speed<zeroGTiltMinimumSpeed50ZG?0.f:std::clamp(zeroGTiltInputTarget50ZG(in.human,in.lean),-1.f,1.f);
                if(target==0.f&&std::fabs(r.tilt)<zeroGTiltReleaseHalvingBelow50ZG)r.tilt*=std::pow(0.5f,frames);
                else{const float ramp=zeroGTiltRampPerSecond50ZG*in.dt;r.tilt+=std::clamp(target-r.tilt,-ramp,ramp);}
                if(!std::isfinite(r.tilt))r.tilt=0.f;
            }
            const float tilt=r.tilt;
            r.rollRate=std::clamp(r.rollRate+(zeroGTurnRollGain50ZG*turnTarget+zeroGTiltRollGain50ZG*tilt)*in.dt,
                -zeroGRollLimitDegrees50ZG,zeroGRollLimitDegrees50ZG)*std::pow(zeroGRollDecayPerFrame50ZG,frames);
            out.flightRollRateDegrees=r.rollRate;
        }else{
            // ZOFF: pitch rate is held (not slewed: N64 quirk), roll comes
            // from the bank error (frame side); floor normal for that error
            // when the probe sees no ground.
            out.recoverRollGain=zeroGRecoverRollGain50ZG;
            if(c.sample){out.haveSurfaceNormal=true;for(unsigned i=0;i<3;++i)out.surfaceNormal[i]=c.sample->normal[i];}
        }
        out.mode=r.mode;out.dt=in.dt;
        out.forwardBlend=1.f-std::pow(1.f-zeroGForwardBlendPerFrame50ZG,frames);
        out.pitchRateDegrees=r.pitchRate*zeroGRotationScale50ZG;
        // Summary (logs only).
        const float aheadDistance=std::sqrt(std::max(0.f,
            (out.lookAhead[0]-c.point[0])*(out.lookAhead[0]-c.point[0])+(out.lookAhead[1]-c.point[1])*(out.lookAhead[1]-c.point[1])+
            (out.lookAhead[2]-c.point[2])*(out.lookAhead[2]-c.point[2])));
        if(r.mode==ZeroGravityMode50ZG::Flight){
            r.maxAbsRoll=std::max(r.maxAbsRoll,std::fabs(r.rollRate));r.maxAbsPitch=std::max(r.maxAbsPitch,std::fabs(r.pitchRate));
            r.minAhead=r.minAhead==0.f?aheadDistance:std::min(r.minAhead,aheadDistance);r.maxAhead=std::max(r.maxAhead,aheadDistance);
            ++r.flightSteps;if(std::isfinite(in.steer)&&std::fabs(in.steer)>=0.999f)++r.steerPinnedSteps;
            // Where the racer is against the line and how far its travel is
            // off its nose (logs only; settles why a pod leaves the line).
            // line_dy = world height above the line point, line_dx = sideways
            // off the line in the horizontal plane (+ = right of the link).
            const auto& a=g.nodes[c.from].line;const auto& b=g.nodes[c.to].line;
            float hx=b[0]-a[0],hz=b[2]-a[2];const float hl=std::sqrt(hx*hx+hz*hz);
            const float ox=in.position[0]-c.point[0],oy=in.position[1]-c.point[1],oz=in.position[2]-c.point[2];
            float dx=0.f;if(hl>1e-3f){hx/=hl;hz/=hl;dx=ox*hz-oz*hx;} // right = up x forward
            const float offset=std::sqrt(ox*ox+oy*oy+oz*oz);
            float upSlip=0.f,sideSlip=0.f;bool slip=false;
            if(in.haveMotion){
                const float* v=in.velocity;const float speed=std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
                if(speed>1.f&&std::isfinite(speed)){slip=true;
                    const float rad=180.f/zeroGPi50ZG;
                    upSlip=std::asin(std::clamp((v[0]*in.up[0]+v[1]*in.up[1]+v[2]*in.up[2])/speed,-1.f,1.f))*rad;
                    sideSlip=std::asin(std::clamp((v[0]*in.right[0]+v[1]*in.right[1]+v[2]*in.right[2])/speed,-1.f,1.f))*rad;}
            }
            if(std::isfinite(offset))r.maxLineOffset=std::max(r.maxLineOffset,offset);
            if(slip&&std::isfinite(upSlip))r.maxUpSlip=std::max(r.maxUpSlip,std::fabs(upSlip));
            if(r.flightSteps%zeroGFlightSampleSteps50ZG==1u){
                std::cerr<<"zero_gravity sample slot="<<slot<<" step="<<r.steps<<" flight_step="<<r.flightSteps
                    <<" link="<<c.from<<"->"<<c.to<<" t="<<c.t<<" line_dy="<<oy<<" line_dx="<<dx<<" line_ft="<<offset
                    <<" ahead_ft="<<aheadDistance<<" speed="<<in.speed<<" steer="<<in.steer<<" roll_rate="<<r.rollRate;
                if(slip)std::cerr<<" up_slip_deg="<<upSlip<<" side_slip_deg="<<sideSlip<<" pod_up_y="<<in.up[1]<<" nose_y="<<in.forward[1];
                std::cerr<<'\n';
            }
        }
        return out;
    }
    // AFTER a COMPLETED racer step, after the surface rule (N64
    // swrRace_UpdateSurfaceTag, second per-frame pass). Commits the pending
    // rates (plus the Recover roll rate the frame derived from the bank error),
    // then runs the latch. groundDistance = this step's probe distance (ft,
    // +inf when the probe saw nothing). done = the step context as the frame
    // left it.
    void afterStep(std::size_t slot,const PreparedTerrainState48J& terrain,float groundDistance,
        float measuredSpeed,const SurfaceRules50ST& rules,const ZeroGravityStep50ZG& done)noexcept {
        if(!loaded_||slot>=zeroGravitySlots50ZG)return;
        if(pendingValid_[slot]){racers_[slot]=pending_[slot];pendingValid_[slot]=false;}
        auto& r=racers_[slot];++r.steps;
        if(done.active&&done.mode==ZeroGravityMode50ZG::Recover&&done.basisWritten)
            r.rollRate=done.recoverRollGain*done.bankErrorDegrees;
        const auto before=r.mode;
        if(before==ZeroGravityMode50ZG::Flight&&std::isfinite(measuredSpeed))
            r.minSpeed=r.minSpeed==0.f?measuredSpeed:std::min(r.minSpeed,measuredSpeed);
        // ZOFF ends when the ground is within 12 ft (contact step, before the tag pass).
        if(before==ZeroGravityMode50ZG::Recover&&groundDistance<zeroGLandingDistance50ZG){
            set(slot,ZeroGravityMode50ZG::Normal,"land",r.lineOwner,rules,groundDistance);}
        // Surface: the line's floor in ZON, the racer's own probe list otherwise.
        std::uint32_t owner=noSurfaceOwner50ST;std::uint8_t bits=0;
        if(before==ZeroGravityMode50ZG::Flight){owner=r.lineOwner;bits=rules.ownerZeroGravity(owner);}
        else bits=rules.collectedZeroGravity(terrain,owner);
        // ZeroGravity is the raw N64 bitfield (1 ZOn, 2 ZOff, 3 both). N64
        // UpdateSurfaceTag order (0x80066C5C, then 0x80066C78): ZOn sets ZON
        // first, then ZOff clears it against the just-updated state, so a face
        // flagged 3 enters and leaves flight in the same step (ends in Recover).
        if((bits&zeroGravityOnBit50ST)&&r.mode!=ZeroGravityMode50ZG::Flight)
            set(slot,ZeroGravityMode50ZG::Flight,"enter",owner,rules,groundDistance);
        if((bits&zeroGravityOffBit50ST)&&r.mode==ZeroGravityMode50ZG::Flight)
            set(slot,ZeroGravityMode50ZG::Recover,"exit",owner,rules,groundDistance);
        zeroGravityPublishedMode50ZG[slot]=r.mode;
    }
    // Any track reset (228BBC under-track and 16EAA0 AI reset both reach
    // 2255B0). N64 swrRace_ResetToSpline: clears ZOFF, keeps ZON, zeroes both
    // pitch rates (0x8007057C, 0x80070668). Gravity returns by mode.
    void onTrackReset(std::size_t slot)noexcept {
        if(!loaded_||slot>=zeroGravitySlots50ZG)return;
        auto& r=racers_[slot];const auto before=r.mode;
        if(r.mode==ZeroGravityMode50ZG::Recover){r.mode=ZeroGravityMode50ZG::Normal;r.modeStep=r.steps;}
        r.pitchTarget=0.f;r.pitchRate=0.f;zeroGravityPublishedMode50ZG[slot]=r.mode;
        std::cerr<<"zero_gravity reset slot="<<slot<<" step="<<r.steps<<" mode_from="<<name(before)
            <<" mode_to="<<name(r.mode)<<'\n';
    }
    void resetRacers()noexcept {racers_.fill({});pending_.fill({});pendingValid_.fill(false);
        zeroGravityPublishedMode50ZG.fill(ZeroGravityMode50ZG::Normal);}

private:
    struct Tri {float a[3],b[3],c[3];};
    struct Mesh {std::vector<Tri> tris;float lo[3]{},hi[3]{};bool any=false;};
    struct Cursor {bool valid=false;std::uint32_t from=0,to=0,slot=0;float t=0.f;float point[3]{};
        const ZeroGravityLineSample50ZG* sample=nullptr;};

    static const char* name(ZeroGravityMode50ZG m)noexcept {
        return m==ZeroGravityMode50ZG::Flight?"flight":m==ZeroGravityMode50ZG::Recover?"recover":"normal";}
    void set(std::size_t slot,ZeroGravityMode50ZG to,const char* event,std::uint32_t owner,
        const SurfaceRules50ST& rules,float groundDistance)noexcept {
        auto& r=racers_[slot];
        std::cerr<<"zero_gravity "<<event<<" slot="<<slot<<" step="<<r.steps<<" mode_from="<<name(r.mode)
            <<" mode_to="<<name(to)<<" owner="<<rules.ownerName(owner)<<" mode_steps="<<(r.steps-r.modeStep);
        if(std::isfinite(groundDistance))std::cerr<<" ground_distance="<<groundDistance;
        if(r.mode==ZeroGravityMode50ZG::Flight)
            std::cerr<<" min_speed="<<r.minSpeed<<" max_roll_rate="<<r.maxAbsRoll<<" max_pitch_rate="<<r.maxAbsPitch
                <<" lookahead_ft="<<r.minAhead<<".."<<r.maxAhead
                <<" steer_pinned_pct="<<(r.flightSteps?100.0*static_cast<double>(r.steerPinnedSteps)/static_cast<double>(r.flightSteps):0.0)
                <<" max_line_ft="<<r.maxLineOffset<<" max_up_slip_deg="<<r.maxUpSlip;
        std::cerr<<'\n';
        if(to==ZeroGravityMode50ZG::Flight){r.minSpeed=0.f;r.maxAbsRoll=0.f;r.maxAbsPitch=0.f;r.minAhead=0.f;r.maxAhead=0.f;
            r.flightSteps=0;r.steerPinnedSteps=0;r.maxLineOffset=0.f;r.maxUpSlip=0.f;}
        r.mode=to;r.modeStep=r.steps;r.linkMissingLogged=false;
    }
    static void transform(const BodyMatrix48G& m,const std::array<float,3>& p,float out[3])noexcept {
        for(unsigned i=0;i<3;++i)out[i]=m.v[i]*p[0]+m.v[4+i]*p[1]+m.v[8+i]*p[2]+m.v[12+i];
    }
    // FLAG (check on first run, logged as node_floor_*): the instance world
    // is taken as the COL's full placement (TrackCollider50AO composes it).
    static Mesh worldMesh(const assets::CollisionMesh48I& mesh,const BodyMatrix48G& world){
        Mesh out;out.tris.reserve(mesh.faces.size());
        for(const auto& f:mesh.faces){
            if(f.vertices[0]>=mesh.vertices.size()||f.vertices[1]>=mesh.vertices.size()||f.vertices[2]>=mesh.vertices.size())continue;
            Tri t;transform(world,mesh.vertices[f.vertices[0]],t.a);transform(world,mesh.vertices[f.vertices[1]],t.b);
            transform(world,mesh.vertices[f.vertices[2]],t.c);
            for(const float* p:{t.a,t.b,t.c})for(unsigned i=0;i<3;++i){
                if(!out.any||p[i]<out.lo[i])out.lo[i]=p[i];if(!out.any||p[i]>out.hi[i])out.hi[i]=p[i];}
            out.any=true;out.tris.push_back(t);
        }
        return out;
    }
    // Straight-down ray from (x,y0,z) over every static owner: highest face
    // at or below y0 within the N64 reach. Host-side bake, like the N64's own
    // load-time bake; no Revenge query state is touched.
    static bool downRay(const std::vector<Mesh>& meshes,float x,float y0,float z,ZeroGravityLineSample50ZG& out){
        bool hit=false;float best=-std::numeric_limits<float>::infinity();
        for(std::size_t k=0;k<meshes.size();++k){const auto& m=meshes[k];
            if(!m.any||x<m.lo[0]||x>m.hi[0]||z<m.lo[2]||z>m.hi[2]||m.lo[1]>y0||m.hi[1]<y0-zeroGBakeRayLength50ZG)continue;
            for(const auto& t:m.tris){
                const float e1x=t.b[0]-t.a[0],e1z=t.b[2]-t.a[2],e2x=t.c[0]-t.a[0],e2z=t.c[2]-t.a[2];
                const float det=e1x*e2z-e2x*e1z;if(std::fabs(det)<1e-9f)continue; // vertical face
                const float px=x-t.a[0],pz=z-t.a[2];
                const float u=(px*e2z-e2x*pz)/det,v=(e1x*pz-px*e1z)/det;
                if(u<0.f||v<0.f||u+v>1.f)continue;
                const float y=t.a[1]+u*(t.b[1]-t.a[1])+v*(t.c[1]-t.a[1]);
                if(y>y0||y<y0-zeroGBakeRayLength50ZG||!(y>best))continue;
                float n[3]{(t.b[1]-t.a[1])*(t.c[2]-t.a[2])-(t.b[2]-t.a[2])*(t.c[1]-t.a[1]),
                    (t.b[2]-t.a[2])*(t.c[0]-t.a[0])-(t.b[0]-t.a[0])*(t.c[2]-t.a[2]),
                    (t.b[0]-t.a[0])*(t.c[1]-t.a[1])-(t.b[1]-t.a[1])*(t.c[0]-t.a[0])};
                const float len=std::sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);if(!(len>0.f))continue;
                const float s=(n[1]<0.f?-1.f:1.f)/len;
                best=y;hit=true;out.owner=static_cast<std::uint32_t>(k);out.floorY=y;
                for(unsigned i=0;i<3;++i)out.normal[i]=n[i]*s;
            }
        }
        return hit;
    }
    static std::string lower(std::string s){for(auto& ch:s)if(ch>='A'&&ch<='Z')ch=static_cast<char>(ch-'A'+'a');return s;}
    // Optional per-node LineHeight (new key, default 0, refused outside
    // [0,5000]). The original 2A1260 reader and the port's readers ignore
    // unknown keys (only Position / NumNextNodes / Next* are read).
    static void lineHeights(const std::string& text,std::vector<float>& out){
        long node=-1;std::size_t at=0;
        while(at<text.size()){const auto end=std::min(text.find('\n',at),text.size());
            std::string line=text.substr(at,end-at);at=end+1;
            while(!line.empty()&&(line.back()=='\r'||line.back()==' '||line.back()=='\t'))line.pop_back();
            std::size_t b=0;while(b<line.size()&&(line[b]==' '||line[b]=='\t'))++b;line=line.substr(b);
            if(line.empty()||line[0]==';')continue;
            if(line[0]=='['){const auto l=lower(line);node=-1;
                if(l.rfind("[node",0)==0&&l.size()>6&&l.back()==']')node=std::strtol(l.substr(5,l.size()-6).c_str(),nullptr,10);
                continue;}
            const auto eq=line.find('=');if(eq==std::string::npos||node<0||static_cast<std::size_t>(node)>=out.size())continue;
            std::string key=lower(line.substr(0,eq));while(!key.empty()&&(key.back()==' '||key.back()=='\t'))key.pop_back();
            if(key!="lineheight")continue;
            std::string value=line.substr(eq+1);std::size_t vb=0;while(vb<value.size()&&(value[vb]==' '||value[vb]=='\t'))++vb;
            value=value.substr(vb);float x=0.f;
            if(assets::parseTrackNumber50AD(value.data(),value.size(),x)&&std::isfinite(x)&&x>=0.f&&x<=zeroGLineHeightLimit50ZG)
                out[static_cast<std::size_t>(node)]=x;
            else std::cerr<<"zero_gravity value_refused node="<<node<<" lineheight="<<value<<'\n';
        }
    }
    static bool readGraph(const assets::ResArchive48C& archive,const char* member,const std::vector<Mesh>& meshes,
        ZeroGravityLineGraph50ZG& g,const char* label){
        if(!member)return false;std::vector<std::uint8_t> bytes;assets::DecodeError48C error;
        if(!assets::extractResMember48C(archive,member,bytes,error)){std::cerr<<"zero_gravity_load refused "<<label<<" member\n";return false;}
        auto scratch=std::make_unique<UndertrackReadScratch50ET>();auto track=std::make_unique<UndertrackTrack50ET>();
        UndertrackDiagnostic50ET d{};
        if(!readUndertrackTrack50ET(reinterpret_cast<const char*>(bytes.data()),bytes.size(),*scratch,*track,d)){
            std::cerr<<"zero_gravity_load refused "<<label<<" spline pc="<<std::hex<<d.sourcePC<<std::dec<<'\n';return false;}
        std::vector<float> heights(track->count,0.f);
        lineHeights(std::string(bytes.begin(),bytes.end()),heights);
        g.nodes.assign(track->count,{});std::uint32_t base=0;
        for(std::uint32_t i=0;i<track->count;++i){auto& n=g.nodes[i];const auto& s=track->nodes[i];
            n.line[0]=s.position[0];n.line[1]=s.position[1]+heights[i];n.line[2]=s.position[2];
            n.nextCount=std::min<std::uint32_t>(s.nextCount,maxUndertrackNext50ET);
            for(std::uint32_t j=0;j<n.nextCount;++j)n.next[j]=s.next[j]<track->count?s.next[j]:i;
            n.sampleBase=base;base+=n.nextCount*zeroGSamplesPerLink50ZG;}
        g.samples.assign(base,{});
        std::vector<float> nodeFloor;std::size_t lineAboveFloor=0;
        for(std::uint32_t i=0;i<track->count;++i){const auto& n=g.nodes[i];
            for(std::uint32_t j=0;j<n.nextCount;++j){const auto& to=g.nodes[n.next[j]];
                for(std::uint32_t k=0;k<zeroGSamplesPerLink50ZG;++k){
                    const float t=(static_cast<float>(k)+0.5f)/static_cast<float>(zeroGSamplesPerLink50ZG);
                    float p[3];for(unsigned a=0;a<3;++a)p[a]=n.line[a]+t*(to.line[a]-n.line[a]);
                    auto& sample=g.samples[n.sampleBase+j*zeroGSamplesPerLink50ZG+k];
                    if(downRay(meshes,p[0],p[1]+1.f,p[2],sample)){++g.hits;if(p[1]-sample.floorY>20.f)++lineAboveFloor;}
                }}
            // Self-check of the bake against the data: nodes sit on the floor
            // (converter rule; stock nodes near it), so a ray from node+1 ft
            // should land within a few ft of the node.
            ZeroGravityLineSample50ZG at{};
            const auto& s=track->nodes[i];
            if(downRay(meshes,s.position[0],s.position[1]+1.f,s.position[2],at))nodeFloor.push_back(std::fabs(s.position[1]-at.floorY));
        }
        std::sort(nodeFloor.begin(),nodeFloor.end());
        std::cerr<<"zero_gravity_line graph="<<label<<" nodes="<<g.nodes.size()<<" samples="<<g.samples.size()
            <<" hits="<<g.hits<<" line_over_20ft="<<lineAboveFloor<<" node_floor_hits="<<nodeFloor.size()
            <<" node_floor_median_ft="<<(nodeFloor.empty()?-1.f:nodeFloor[nodeFloor.size()/2])<<'\n';
        return true;
    }
    // Cursor on link from->to (the racer's own progress words), projected.
    static bool cursor(const ZeroGravityLineGraph50ZG& g,std::uint32_t from,std::uint32_t to,const float pos[3],Cursor& c)noexcept {
        if(!g.ready()||from>=g.nodes.size()||to>=g.nodes.size())return false;
        const auto& a=g.nodes[from];std::uint32_t slot=maxUndertrackNext50ET;
        for(std::uint32_t j=0;j<a.nextCount;++j)if(a.next[j]==to){slot=j;break;}
        if(slot==maxUndertrackNext50ET)return false;
        const auto& b=g.nodes[to];float ab[3],ap[3],len2=0.f,dot=0.f;
        for(unsigned i=0;i<3;++i){ab[i]=b.line[i]-a.line[i];ap[i]=pos[i]-a.line[i];len2+=ab[i]*ab[i];dot+=ab[i]*ap[i];}
        const float t=len2>0.f?std::clamp(dot/len2,0.f,1.f):0.f;
        c.valid=true;c.from=from;c.to=to;c.slot=slot;c.t=t;
        for(unsigned i=0;i<3;++i)c.point[i]=a.line[i]+t*ab[i];
        const auto k=std::min<std::uint32_t>(static_cast<std::uint32_t>(t*zeroGSamplesPerLink50ZG),zeroGSamplesPerLink50ZG-1);
        const std::size_t index=static_cast<std::size_t>(a.sampleBase)+slot*zeroGSamplesPerLink50ZG+k;
        c.sample=index<g.samples.size()&&g.samples[index].owner!=noSurfaceOwner50ST?&g.samples[index]:nullptr;
        return true;
    }
    // Point at spline parameter cursor+ahead: whole links follow Next0 (the
    // ordered first link) past the cursor's own link. FLAG: at a fork inside
    // a zero-g section this can look down the other branch for <= 2 links.
    static void lineAt(const ZeroGravityLineGraph50ZG& g,const Cursor& c,float ahead,float out[3])noexcept {
        std::uint32_t from=c.from,to=c.to;float t=c.t+std::max(0.f,ahead);
        for(unsigned guard=0;t>=1.f&&guard<8;++guard){
            const auto& n=g.nodes[to];if(!n.nextCount){t=1.f;break;}
            from=to;to=n.next[0];t-=1.f;
        }
        t=std::min(t,1.f);const auto& a=g.nodes[from];const auto& b=g.nodes[to];
        for(unsigned i=0;i<3;++i)out[i]=a.line[i]+t*(b.line[i]-a.line[i]);
    }
    void resetCourse()noexcept {loaded_=false;for(auto& g:graphs_)g=ZeroGravityLineGraph50ZG{};floor_.reset();resetRacers();}

    bool loaded_=false,humanLeanBound_=false;
    std::array<ZeroGravityLineGraph50ZG,2> graphs_{};
    std::array<ZeroGravityRacer50ZG,zeroGravitySlots50ZG> racers_{},pending_{};
    std::array<bool,zeroGravitySlots50ZG> pendingValid_{};
    N64FloorTableOwner50NH floor_;                                   // .nft table source (PORT EXTENSION)
    std::array<ZeroGravityLineSample50ZG,zeroGravitySlots50ZG> floorSample_{}; // per-slot sample the cursor points at
};
inline ZeroGravity50ZG zeroGravity50ZG;
} // namespace rr::game
