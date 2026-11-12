#pragma once
// PC-native debris simulation for destroyed racers (docs/pod-destruction-20261108.md).
//
// Spawn follows the original destroy handlers: 2261B0 (explode) launches each
// collection through 22CA60, whose 22CB9C tail adds the pod's velocity and
// angular velocity to six shared-308030 draws scaled by the lazy 434870
// (20.0, angular) and 434878 (293.333, linear) words; 226750 (wreck in place)
// activates the collections at the pod (22C6A0) without the launch tail.
// Placement per 22C6A0: piece position = pod world translation + pod rotation
// x authored offset (PHY position50), orientation = the pod's.
//
// The per-frame motion uses the original force and settling laws: 167250
// gravity (-mass x [Wreckage] Gravity x 32.2 on Y), quadratic air drag
// (-Drag x speed^2 along the velocity) and rate damping (-RateDamping x
// angular velocity) with mass = Weight / 32.2 and the 166400 box inertia;
// 167640 settling (contact time toward 2 x 2.0 s, drag lerped to 50 and rate
// damping to 200 over the second window, stop below speed 10, spin 0.5,
// linear acceleration 10 and angular acceleration 1 while in contact).
// Ground contact is a native box-support segment test against the track's
// static mesh candidates (front faces only, as 1082B0) with restitution and
// Coulomb friction, not the original 166E40/2CA840 spring/impulse solver.
// A piece below every track collision bound can never land again; it is
// retired (no longer simulated or drawn) instead of falling forever.
#include "native_wreck_debris50pd.h"
#include "../../src/game/collision_tree48i.h"
#include "../../src/game/original_death_camera50kg.h"
#include "../../src/game/static_type2_producer49x.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <span>
#include <vector>

namespace native_wreck_debris50PD {

struct Vec { float x=0,y=0,z=0; };
inline Vec operator+(Vec a,Vec b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline Vec operator-(Vec a,Vec b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline Vec operator*(Vec a,float s){return {a.x*s,a.y*s,a.z*s};}
inline float dot(Vec a,Vec b){return a.x*b.x+a.y*b.y+a.z*b.z;}
struct Quat { float x=0,y=0,z=0,w=1; };
inline Quat multiply(Quat a,Quat b){
    return {a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
            a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};
}
inline Quat normalized(Quat q){
    const float n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
    return n>0.f?Quat{q.x/n,q.y/n,q.z/n,q.w/n}:Quat{};
}
// Rotation part of a BodyMatrix48G-style 4x4 (basis columns at 0/4/8).
inline Quat fromBasis(const float* m){
    const float m00=m[0],m11=m[5],m22=m[10];
    const float trace=m00+m11+m22;Quat q;
    if(trace>0.f){const float s=std::sqrt(trace+1.f)*2.f;q.w=.25f*s;q.x=(m[6]-m[9])/s;q.y=(m[8]-m[2])/s;q.z=(m[1]-m[4])/s;}
    else if(m00>m11&&m00>m22){const float s=std::sqrt(1.f+m00-m11-m22)*2.f;q.w=(m[6]-m[9])/s;q.x=.25f*s;q.y=(m[4]+m[1])/s;q.z=(m[8]+m[2])/s;}
    else if(m11>m22){const float s=std::sqrt(1.f+m11-m00-m22)*2.f;q.w=(m[8]-m[2])/s;q.x=(m[4]+m[1])/s;q.y=.25f*s;q.z=(m[9]+m[6])/s;}
    else{const float s=std::sqrt(1.f+m22-m00-m11)*2.f;q.w=(m[1]-m[4])/s;q.x=(m[8]+m[2])/s;q.y=(m[9]+m[6])/s;q.z=.25f*s;}
    return normalized(q);
}
inline void toMatrix(Quat q,Vec p,float out[16]){
    const float xx=q.x*q.x,yy=q.y*q.y,zz=q.z*q.z,xy=q.x*q.y,xz=q.x*q.z,yz=q.y*q.z,wx=q.w*q.x,wy=q.w*q.y,wz=q.w*q.z;
    out[0]=1-2*(yy+zz);out[1]=2*(xy+wz);out[2]=2*(xz-wy);out[3]=0;
    out[4]=2*(xy-wz);out[5]=1-2*(xx+zz);out[6]=2*(yz+wx);out[7]=0;
    out[8]=2*(xz+wy);out[9]=2*(yz-wx);out[10]=1-2*(xx+yy);out[11]=0;
    out[12]=p.x;out[13]=p.y;out[14]=p.z;out[15]=1;
}

struct LivePiece {
    std::size_t racer=0,index=0;  // cold racer slot and its piece index
    Vec position,velocity,angular,spawn;
    Quat orientation;
    float radius=1.f,mass=1.f,inertia=1.f,drag=0.f,rateDamping=0.f,contactTime=0.f;
    std::uint32_t contacts=0,lastContactStep=0;
    bool settled=false,retired=false;
    bool effectsStarted=false; // piece smoke/fire already started (limp pieces)
};
struct Sim {
    std::vector<LivePiece> pieces;
    std::span<rr::game::Type2StaticCandidate49X> track;
    std::array<std::uint32_t,256> stack{};
    std::uint64_t steps=0,contacts=0;
    // Query outcomes (diagnostics): misses are ordinary, the rest are reported.
    std::uint64_t queries=0,invalid=0,capacity=0,unsupported=0;
    std::uint32_t retired=0;
    // Track collision summary, computed once per bound track.
    bool summarized=false;
    float floor=0.f,ceiling=0.f;
    std::uint32_t meshCandidates=0,callbackCandidates=0,compoundCandidates=0,otherCandidates=0;
    // Per racer: [Wreckage] collections already activated by 227370/227A40
    // (flags29 bit0). 2261B0 (22659C..) and 226750 (2269CC..) skip them.
    std::array<std::uint32_t,8> activeCollections{};
};
inline Sim& sim() noexcept { static Sim owner; return owner; }
// Capacity is reserved at race activation: frames never allocate.
inline constexpr std::size_t maxLivePieces=512;
inline void resetCameraView() noexcept;
inline void resetSim() {
    resetCameraView();
    auto& s=sim();s.pieces.clear();s.pieces.reserve(maxLivePieces);
    s.steps=0;s.contacts=0;s.queries=0;s.invalid=0;s.capacity=0;s.unsupported=0;s.retired=0;s.summarized=false;
    s.activeCollections={};
}
// Lowest/highest point any static mesh candidate's bounding sphere reaches.
inline void summarizeTrack(Sim& s) noexcept {
    if(s.summarized)return;
    s.summarized=true;s.floor=1e30f;s.ceiling=-1e30f;
    s.meshCandidates=s.callbackCandidates=s.compoundCandidates=s.otherCandidates=0;
    for(const auto& candidate:s.track){
        const auto& c=candidate.candidate.collision;
        if(c.hasResponseCallbackE8){++s.callbackCandidates;continue;}
        if(candidate.shapeType==1){++s.compoundCandidates;continue;}
        if(!c.geometry.mesh||candidate.shapeType!=0){++s.otherCandidates;continue;}
        ++s.meshCandidates;
        s.floor=std::min(s.floor,c.sphere.center.v[1]-c.sphere.radius);
        s.ceiling=std::max(s.ceiling,c.sphere.center.v[1]+c.sphere.radius);
    }
    if(!s.meshCandidates){s.floor=-1e30f;s.ceiling=1e30f;}
}

inline float drawUnit(std::uint32_t& raw) noexcept {
    raw=raw*0x41c64e6du+0x3039u; //308030: advances raw32, returns low31
    volatile float converted=static_cast<float>(static_cast<std::int32_t>(raw&0x7fffffffu));
    volatile float unit=std::bit_cast<float>(0x30000000u)*converted;
    return unit;
}
inline float signedUnit(std::uint32_t& raw) noexcept {
    const float unit=drawUnit(raw);volatile float twice=2.f*unit;return twice-1.f;
}

// 166400 mass/inertia for the piece's unanimated model bounds (full extents).
inline void tunePiece(LivePiece& live,const Piece& piece,const rr::game::OriginalWreckTuning50LA* tuning) noexcept {
    const auto& b=piece.mesh.bounds;
    const float x=b.maximum[0]-b.minimum[0],y=b.maximum[1]-b.minimum[1],z=b.maximum[2]-b.minimum[2];
    const float gravityUnit=std::bit_cast<float>(0x4200cccdu);
    live.mass=piece.weight>0.f?piece.weight/gravityUnit:1.f;
    const float scale=(piece.inertia>0.f?piece.inertia:1.f)*live.mass/12.f;
    const float mean=scale*((y*y+z*z)+(x*x+z*z)+(x*x+y*y))/3.f;
    live.inertia=mean>1e-4f?mean:live.mass;
    live.drag=tuning?tuning->drag:0.f;live.rateDamping=tuning?tuning->rateDamping:0.f;
    live.radius=std::max(.5f,.5f*std::max(x,std::max(y,z)));
}

// Spawns every loaded piece of the racer. bodyWorld is the pod's body288 world
// matrix; velocity/angular are its rigid-body velocities (340+70/+B0),
// worldAngularA0/bodyPosition its 340+A0 world rate and body position.
// Returns pieces spawned.
inline std::size_t spawn(std::size_t slot,bool explode,const float* bodyWorld,
    const float* velocity,const float* angular,const float* worldAngularA0,const float* bodyPosition,
    std::uint32_t& sharedRaw,const float* cockpitVelocity=nullptr,const float* cockpitAngular=nullptr,
    std::size_t* skippedActive=nullptr) {
    auto& c=cold();if(slot>=c.racers.size())return 0;
    auto& s=sim();summarizeTrack(s);
    const auto& racer=c.racers[slot];
    const Quat orientation=fromBasis(bodyWorld);
    const Vec origin{bodyWorld[12],bodyWorld[13],bodyWorld[14]};
    const float angularScale=std::bit_cast<float>(0x41a00000u),velocityScale=std::bit_cast<float>(0x4392aa7fu);
    std::size_t spawned=0;
    for(std::size_t i=0;i<racer.pieces.size();++i){
        const auto& piece=racer.pieces[i];
        if(slot<s.activeCollections.size()&&piece.collection<32u&&((s.activeCollections[slot]>>piece.collection)&1u)){
            if(skippedActive)++*skippedActive;continue;}
        LivePiece live;live.racer=slot;live.index=i;live.orientation=orientation;
        const auto& a=piece.authored50;
        const Vec offset{a[0]*bodyWorld[0]+a[1]*bodyWorld[4]+a[2]*bodyWorld[8],
                         a[0]*bodyWorld[1]+a[1]*bodyWorld[5]+a[2]*bodyWorld[9],
                         a[0]*bodyWorld[2]+a[1]*bodyWorld[6]+a[2]*bodyWorld[10]};
        live.position=origin+offset;live.spawn=live.position;
        tunePiece(live,piece,piece.collection<racer.tuning.size()?&racer.tuning[piece.collection]:nullptr);
        if(explode){
            // 22CB9C..22CDE4 order: angular X/Y/Z signed, then linear X signed, Y unit, Z signed.
            const Vec jitterAngular{signedUnit(sharedRaw)*angularScale,signedUnit(sharedRaw)*angularScale,signedUnit(sharedRaw)*angularScale};
            const float lx=signedUnit(sharedRaw),ly=drawUnit(sharedRaw),lz=signedUnit(sharedRaw);
            live.velocity=Vec{velocity[0],velocity[1],velocity[2]}+Vec{lx,ly,lz}*velocityScale;
            live.angular=Vec{angular[0],angular[1],angular[2]}+jitterAngular;
        }else{
            // 226750 (226A70..226BAC): rigid-body point velocity at the piece,
            // 70 + (A0 x r) with r = piece - body position; rate = 340+B0.
            // Native: the source writes this to each collection's FIRST piece
            // and the rest ride its group until break-off (2CA480/2CA620, not
            // simulated); every piece takes its own point velocity here, which
            // is what a rigid group carries before it breaks.
            const Vec r{live.position.x-bodyPosition[0],live.position.y-bodyPosition[1],live.position.z-bodyPosition[2]};
            const Vec w{worldAngularA0[0],worldAngularA0[1],worldAngularA0[2]};
            const Vec spin{w.y*r.z-w.z*r.y,w.z*r.x-w.x*r.z,w.x*r.y-w.y*r.x};
            live.velocity=Vec{velocity[0],velocity[1],velocity[2]}+spin;
            live.angular=Vec{angular[0],angular[1],angular[2]};
            // 226AD8..226B34: after a limp (states1/2) the cockpit collection
            // 374[0] takes the physical cockpit 35C's own 70/B0 instead.
            if(cockpitVelocity&&cockpitAngular&&piece.collection==0u){
                live.velocity=Vec{cockpitVelocity[0],cockpitVelocity[1],cockpitVelocity[2]};
                live.angular=Vec{cockpitAngular[0],cockpitAngular[1],cockpitAngular[2]};
            }
        }
        if(s.pieces.size()>=maxLivePieces)break;
        s.pieces.push_back(live);++spawned;
    }
    return spawned;
}

// 227370/227A40 rows 11..20: activate the lost side's collections in place.
// Per collection (in 374 order: 374[1]+374[3] left, 374[2]+374[4] right):
// piece 0 takes the lost engine model's world pose (28ADD0/28A090) and the
// pod's point velocity at that engine, 70 + A0 x (engine - container194),
// with rate B0 + (0,0, sign*7.0*u) from ONE shared 308030 draw (227684).
// Native: the collection's other pieces stand at their authored places with
// the same point velocity until the group break-off simulation exists.
struct LimpCollection { std::size_t collection=0; const float* engineWorld=nullptr; float sign=1.f; };
inline std::size_t spawnLimp(std::size_t slot,const LimpCollection* list,std::size_t count,const float* bodyWorld,
    const float* velocity,const float* angular,const float* worldAngularA0,const float* container,
    std::uint32_t& sharedRaw,std::size_t& firstSpawned) {
    auto& c=cold();firstSpawned=sim().pieces.size();if(slot>=c.racers.size())return 0;
    auto& s=sim();summarizeTrack(s);
    const auto& racer=c.racers[slot];
    const float kick=std::bit_cast<float>(0x40e00000u);
    std::size_t spawned=0;
    for(std::size_t k=0;k<count;++k){
        const auto& entry=list[k];
        if(entry.collection>=racer.tuning.size()||entry.collection>4u||!entry.engineWorld)continue;
        const float u=drawUnit(sharedRaw);volatile float spinZ=entry.sign*kick*u;
        const Vec origin{entry.engineWorld[12],entry.engineWorld[13],entry.engineWorld[14]};
        const Vec r{origin.x-container[0],origin.y-container[1],origin.z-container[2]};
        const Vec w{worldAngularA0[0],worldAngularA0[1],worldAngularA0[2]};
        const Vec point=Vec{velocity[0],velocity[1],velocity[2]}+Vec{w.y*r.z-w.z*r.y,w.z*r.x-w.x*r.z,w.x*r.y-w.y*r.x};
        const Vec rate=Vec{angular[0],angular[1],angular[2]}+Vec{0.f,0.f,static_cast<float>(spinZ)};
        bool first=true;
        for(std::size_t i=0;i<racer.pieces.size();++i){
            const auto& piece=racer.pieces[i];if(piece.collection!=entry.collection)continue;
            if(s.pieces.size()>=maxLivePieces)return spawned;
            LivePiece live;live.racer=slot;live.index=i;
            if(first){live.orientation=fromBasis(entry.engineWorld);live.position=origin;}
            else{
                live.orientation=fromBasis(bodyWorld);const auto& a=piece.authored50;
                live.position=Vec{bodyWorld[12]+a[0]*bodyWorld[0]+a[1]*bodyWorld[4]+a[2]*bodyWorld[8],
                    bodyWorld[13]+a[0]*bodyWorld[1]+a[1]*bodyWorld[5]+a[2]*bodyWorld[9],
                    bodyWorld[14]+a[0]*bodyWorld[2]+a[1]*bodyWorld[6]+a[2]*bodyWorld[10]};
            }
            live.spawn=live.position;
            tunePiece(live,piece,&racer.tuning[entry.collection]);
            live.velocity=point;live.angular=first?rate:Vec{angular[0],angular[1],angular[2]};
            s.pieces.push_back(live);++spawned;first=false;
        }
        s.activeCollections[slot]|=1u<<entry.collection;
    }
    return spawned;
}

// Distance from the piece origin to its farthest model-bounds corner along
// direction (world): how far the oriented box reaches that way.
inline float support(const Piece& piece,Quat q,Vec direction) noexcept {
    float m[16];toMatrix(q,{},m);
    const auto& b=piece.mesh.bounds;float reach=0.f;
    for(unsigned axis=0;axis<3;++axis){
        const float k=m[axis*4]*direction.x+m[axis*4+1]*direction.y+m[axis*4+2]*direction.z;
        reach+=std::max(b.minimum[axis]*k,b.maximum[axis]*k);
    }
    return std::max(reach,.25f);
}

inline bool segmentHit(Sim& s,Vec from,Vec to,rr::game::CollisionContact48I& hit) noexcept {
    if(s.track.empty())return false;
    const Vec mid=(from+to)*.5f;const Vec d=to-from;
    const float reach=.5f*std::sqrt(dot(d,d));
    rr::game::LocalSegment48I segment{};
    segment.first={{from.x,from.y,from.z}};segment.second={{to.x,to.y,to.z}};
    for(unsigned k=0;k<16;++k)segment.worldMatrix.v[k]=(k%5==0)?1.f:0.f;
    bool found=false;float best=2.f;
    rr::game::CollisionWorkspace48M workspace{s.stack.data(),s.stack.size(),nullptr,0};
    for(const auto& candidate:s.track){
        const auto& c=candidate.candidate.collision;
        if(c.hasResponseCallbackE8||!c.geometry.mesh||candidate.shapeType!=0)continue; // triggers carry callbacks
        const Vec center{c.sphere.center.v[0],c.sphere.center.v[1],c.sphere.center.v[2]};
        const Vec gap=center-mid;const float limit=c.sphere.radius+reach;
        if(dot(gap,gap)>limit*limit)continue;
        rr::game::CollisionResult48I result{};std::size_t traceUsed=0;++s.queries;
        const auto status=rr::game::queryOrderedCollisionBounded48M(segment,&c.geometry,1,workspace,result,traceUsed);
        using Q=rr::game::CollisionQueryStatus48I;
        if(status==Q::InvalidInput)++s.invalid;
        else if(status==Q::InsufficientCapacity)++s.capacity;
        else if(status==Q::UnsupportedInterpolationBoundary)++s.unsupported;
        else if(status==Q::Hit&&result.selected.fraction<best){best=result.selected.fraction;hit=result.selected;found=true;}
    }
    return found;
}

// One native step for every live piece.
inline void step(float dt) noexcept {
    auto& s=sim();if(s.pieces.empty())return;++s.steps;
    auto& c=cold();
    constexpr float window=2.f,finalDrag=50.f,finalRate=200.f; // 22C5xx piece defaults (130/10C/108)
    constexpr float stopSpeed=10.f,stopSpin=.5f,stopAcceleration=10.f,stopAngularAcceleration=1.f; // 134..140
    for(auto& p:s.pieces){
        if(p.settled||p.retired)continue;
        const auto& racer=c.racers[p.racer];
        const auto& piece=racer.pieces[p.index];
        const auto* tuning=piece.collection<racer.tuning.size()?&racer.tuning[piece.collection]:nullptr;
        const Vec oldVelocity=p.velocity,oldAngular=p.angular;
        // 167250: gravity, then quadratic drag and rate damping (implicit so a
        // large settling drag never reverses the motion).
        p.velocity.y-=std::fabs(racer.gravityAcceleration)*dt;
        {const float speed=std::sqrt(dot(p.velocity,p.velocity));
         p.velocity=p.velocity*(1.f/(1.f+std::max(p.drag,0.f)/p.mass*speed*dt));}
        p.angular=p.angular*(1.f/(1.f+std::max(p.rateDamping,0.f)/p.inertia*dt));
        // Box-support probe: from the top of the piece to below its lowest point
        // after this step's motion (plus a contact skin, so a box resting on a
        // slope keeps seeing the plane under its centre every step).
        const float below=support(piece,p.orientation,{0.f,-1.f,0.f});
        const float above=support(piece,p.orientation,{0.f,1.f,0.f});
        const float skin=.5f+.25f*below;
        constexpr float contactSlop=.5f;
        const Vec moved=p.position+p.velocity*dt;
        rr::game::CollisionContact48I hit{};
        bool touching=false;
        Vec normal{0.f,1.f,0.f},point{};float need=0.f;
        if(segmentHit(s,p.position+Vec{0.f,above,0.f},moved-Vec{0.f,below+skin,0.f},hit)){
            normal={hit.normal.v[0],hit.normal.v[1],hit.normal.v[2]};
            const float n=std::sqrt(dot(normal,normal));normal=n>0.f?normal*(1.f/n):Vec{0,1,0};
            point={hit.point.v[0],hit.point.v[1],hit.point.v[2]};
            need=support(piece,p.orientation,normal*-1.f);
            touching=dot(moved-point,normal)<need+contactSlop;
        }
        if(touching){
            ++s.contacts;++p.contacts;p.lastContactStep=static_cast<std::uint32_t>(s.steps);
            const float into=dot(p.velocity,normal);
            // Within the slop counts as touching (settling); only a move that
            // would penetrate gets the contact response, so nothing hovers.
            if(into<0.f&&dot(moved-point,normal)<need){
                // Resting contact: an approach slower than two steps of gravity
                // is absorbed (no micro-bounce), larger impacts use Restitution.
                const float restingSpeed=2.f*std::fabs(racer.gravityAcceleration)*dt;
                const float restitution=-into<=restingSpeed?0.f:tuning?std::clamp(tuning->restitution,0.f,1.f):.3f;
                const float friction=tuning?std::max(tuning->friction,0.f):.5f;
                const Vec normalPart=normal*into,tangent=p.velocity-normalPart;
                const float tangentSpeed=std::sqrt(dot(tangent,tangent));
                const float impulse=(1.f+restitution)*-into;
                const float slow=tangentSpeed>0.f?std::max(0.f,1.f-friction*impulse/tangentSpeed):0.f;
                p.velocity=normalPart*(-restitution)+tangent*slow;
                // Ground friction torque: a tumbling piece loses spin on every contact
                // step (native; spin otherwise re-feeds small bounces on slopes).
                p.angular=p.angular*std::max(0.f,1.f-std::min(1.f,friction*.5f));
            }
            // Move with the post-contact velocity, then push out only along the
            // normal by the remaining penetration (no downhill creep at rest).
            const Vec next=p.position+p.velocity*dt;
            const float gap=dot(next-point,normal);
            p.position=gap<need?next+normal*(need-gap):next;
        }else p.position=moved;
        // 167640 settling. The original's spring contact stays set while a
        // piece rests or skids; these impulse contacts can skip steps on a
        // slope, so a contact within the last 0.25 s still counts as contact.
        const bool inContact=touching||(p.lastContactStep&&float(s.steps-p.lastContactStep)*dt<=.25f);
        p.contactTime=inContact?std::min(p.contactTime+dt,2.f*window):std::max(p.contactTime-dt,0.f);
        {const float fraction=std::clamp((p.contactTime-window)/window,0.f,1.f);
         const float baseDrag=tuning?tuning->drag:0.f,baseRate=tuning?tuning->rateDamping:0.f;
         p.drag=baseDrag+(finalDrag-baseDrag)*fraction;p.rateDamping=baseRate+(finalRate-baseRate)*fraction;}
        const float speed=std::sqrt(dot(p.velocity,p.velocity)),spin=std::sqrt(dot(p.angular,p.angular));
        const Vec dv=p.velocity-oldVelocity,dw=p.angular-oldAngular;
        const float acceleration=std::sqrt(dot(dv,dv))/dt,angularAcceleration=std::sqrt(dot(dw,dw))/dt;
        if(inContact&&speed<stopSpeed&&spin<stopSpin&&acceleration<stopAcceleration&&angularAcceleration<stopAngularAcceleration){
            p.settled=true;p.velocity={};p.angular={};
        }
        // Orientation: q += 0.5 * (0,w) * q * dt.
        const Quat spinQ{p.angular.x,p.angular.y,p.angular.z,0.f};
        const Quat dq=multiply(spinQ,p.orientation);
        p.orientation=normalized({p.orientation.x+.5f*dq.x*dt,p.orientation.y+.5f*dq.y*dt,
            p.orientation.z+.5f*dq.z*dt,p.orientation.w+.5f*dq.w*dt});
        if(!std::isfinite(p.position.x)||!std::isfinite(p.position.y)||!std::isfinite(p.position.z)||
           p.position.y+above<s.floor){
            p.retired=true;++s.retired;p.velocity={};p.angular={};
        }
    }
}

// Renderer instances for every live piece (capped by the renderer capacity).
inline std::uint32_t instances(std::array<rr::platform::VehicleWindowDebrisInstance50DS,
    rr::platform::maxVehicleWindowDebrisInstances50DS>& out,const std::array<std::uint32_t,8>& firstPiece,
    const std::array<std::uint32_t,8>& pieceCount) noexcept {
    std::uint32_t count=0;
    for(const auto& p:sim().pieces){
        if(count>=out.size())break;
        if(p.retired||p.racer>=firstPiece.size()||p.index>=pieceCount[p.racer])continue;
        auto& instance=out[count++];instance.piece=firstPiece[p.racer]+static_cast<std::uint32_t>(p.index);
        toMatrix(p.orientation,p.position,instance.world);
    }
    return count;
}

// Death-camera debris view (22D4B0 reads each active piece's model localC0
// translation). One root scene node per live piece of the camera's racer,
// grouped by authored collection like racer370/374; rebound to the camera
// Session's death identity. Retired pieces stay listed at their last pose.
struct CameraView {
    static constexpr std::size_t maxPieces=16,maxCollections=8;
    std::array<rr::game::SceneNode49P,maxPieces> nodes{};
    std::array<std::uint8_t,maxPieces> flags{};
    std::array<rr::game::OriginalDeathPiece50KG,maxPieces> pieces{};
    std::array<rr::game::OriginalDeathPieceCollection50KG,maxCollections> collections{};
    std::array<std::size_t,maxPieces> live{};
    rr::game::SceneGraph49P graph{nullptr,0};
    rr::game::OriginalDeathCameraDebrisView50KG view{};
    std::size_t racer=8,count=0;
};
inline CameraView& cameraView() noexcept { static CameraView owner; return owner; }
inline void resetCameraView() noexcept {
    auto& v=cameraView();v.view={};v.count=0;v.racer=8;v.graph={nullptr,0};v.flags={};
}
inline void updateCameraView() noexcept {
    auto& v=cameraView();const auto& s=sim();
    for(std::size_t k=0;k<v.count;++k){
        if(v.live[k]>=s.pieces.size())continue;
        const auto& p=s.pieces[v.live[k]];
        toMatrix(p.orientation,p.position,v.nodes[k].localC0.v);v.nodes[k].world100=v.nodes[k].localC0;
    }
}
inline bool cameraViewBound(std::size_t slot) noexcept { const auto& v=cameraView();return v.racer==slot&&v.count; }
inline bool bindCameraView(std::size_t slot,std::uint64_t sessionGeneration,std::uint64_t racerKey) noexcept {
    auto& v=cameraView();const auto& s=sim();const auto& c=cold();
    resetCameraView();if(slot>=c.racers.size())return false;
    const auto& racer=c.racers[slot];std::size_t collections=0;
    for(std::size_t collection=0;collection<CameraView::maxCollections;++collection){
        const std::size_t first=v.count;
        for(std::size_t i=0;i<s.pieces.size()&&v.count<CameraView::maxPieces;++i){
            const auto& p=s.pieces[i];
            if(p.racer!=slot||p.index>=racer.pieces.size()||racer.pieces[p.index].collection!=collection)continue;
            v.live[v.count++]=i;
        }
        if(v.count>first)v.collections[collections++]={v.pieces.data()+first,v.count-first};
    }
    for(std::size_t k=0;k<v.count;++k){
        v.nodes[k]=rr::game::SceneNode49P{};v.nodes[k].sceneFlags190=0;v.flags[k]=1u;
        v.pieces[k]={&v.flags[k],&v.graph,static_cast<rr::game::SceneNodeIndex49P>(static_cast<std::int32_t>(k))};
    }
    v.graph={v.nodes.data(),static_cast<std::uint32_t>(v.count)};
    updateCameraView();
    if(!v.count)return false;
    v.racer=slot;v.view={sessionGeneration,racerKey,v.collections.data(),collections};
    return true;
}

} // namespace native_wreck_debris50PD
