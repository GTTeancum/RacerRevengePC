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
// Free pieces (explosion pieces, one-piece collections, released members) run
// the original 2CB8F0 -> 166E40 piece frame on their own D0 (stepFreeOriginal):
// the 166790 swept flight while 34 && 38, otherwise the 166F8C nonflight
// solver shared with the groups (E0 detector, 2964C0, 167C40, 166C60), the
// 11C airborne timer and 167640 settling. STAND-IN fallback (logged), only
// for a piece without a usable E0 or whose original frame is refused: the
// native box-support segment test against the track's static mesh candidates
// (front faces only, as 1082B0) with restitution and Coulomb friction.
//
// Wreck groups (2CA020/2CA190/2CA480/2CA620/2CA840/2CA2F0): for 226750 and the
// 227370/227A40 limp, each collection with two or more pieces is one body
// (the primary's) carrying its attached members at their authored offsets;
// members break off by BreakRate/BreakScale on the shared 308030 stream.
// Stage F5: a group's body runs the original 2CA840 nonflight solver
// (native_wreck_group_contact50pd.h): per-piece E0 detectors, 167250 forces,
// the 2964C0 root and member DC responses on the root body, 1C0 retention,
// 167C40 integration, 166C60 movement and 167640 settling; the break test reads
// the member's own DC.3C and the primary's C0; 2CA620 sets the full diagonal
// inertia (14E910 inverse). STAND-IN fallback, only for a group whose pieces
// lack a usable E0 or whose original query/response is refused (logged): the
// earlier native probe with a lever-arm response and an isotropic inertia.
// A piece below every track collision bound can never land again; it is
// retired (no longer simulated or drawn) instead of falling forever.
#include "native_wreck_debris50pd.h"
#include "native_wreck_group_contact50pd.h"
#include "../../src/game/collision_tree48i.h"
#include "../../src/game/original_death_camera50kg.h"
#include "../../src/game/static_type2_producer49x.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <iostream>
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
    Vec inertiaDiag{1.f,1.f,1.f};  // 166400 box diagonal (inertia is its mean)
    std::uint32_t contacts=0,lastContactStep=0;
    bool settled=false,retired=false;
    bool effectsStarted=false; // piece smoke/fire already started (limp pieces)
    // Wreck group (2CA020): root = the group's primary piece (its body is the
    // group body); attached = a member riding the root (body disabled).
    std::int32_t group=-1;
    bool groupRoot=false,attached=false;
    // 2CA2F0 released this step: its own frame (and its 160/164 damage
    // emitters) start from the next step.
    std::uint64_t releasedStep=0;
    // The piece's E0 (m.COL type2 rays) for the original group detector.
    const rr::game::Type2ResourceView49X* e0=nullptr;
    // Free piece on the original 166E40 frame (its own D0 in Sim::bodies at
    // the same live index). 34/38 flight enable/airborne (22C6A0/22CA60),
    // 11C airborne time, 120 flight delay (165AFC), 110/114/118 the 22C310
    // Restitution, FrictionTorqueScale and 166400 1/|extent|.
    bool original=false;
    std::uint32_t flight34=0,flight38=0;
    float airborne11C=0.f,flightDelay120=.5f;
    float restitution110=0.f,frictionTorqueScale114=1.f,inverseExtent118=1.f;
};
// Host-side wreck group, field order of 2CA020/2CA190 (group +30..+B0).
// One place defines this layout; storage is reserved, frames never allocate.
inline constexpr std::size_t maxGroupMembers=15,maxGroups=40;
struct Group {
    bool active=false;
    std::size_t racer=0,collection=0;
    std::size_t root=0;                                   // live index of the primary (40)
    std::uint32_t registered30=0,capacity34=0,attached38=0;
    std::array<std::size_t,maxGroupMembers> member{};     // 44[] live indices
    std::array<Vec,maxGroupMembers> offset48{};           // piece.50 - primary.50 (unrotated)
    std::array<float,maxGroupMembers> breakRateA4{},breakScaleA8{};
    std::array<bool,maxGroupMembers> attachedA0{},latchB0{},touched{};
    float savedMass4C=0.f;Vec savedInertia60{};           // the primary's own mass / inertia diagonal
    float mass26C=0.f;Vec inertiaDiag{};float inertiaMean=1.f; // 2CA620 aggregate
    Vec center1E0{};                                      // root-local centre of mass offset
    Vec peakC0{};                                         // per-axis peak impulse velocity change this frame
    Vec peakRaw{};                                        // same, every approaching contact (diagnostic)
    std::uint64_t draws=0,tests=0,breaks=0;
    // F5: the original solver's root body (piece 0's D0 carrying the group).
    bool original=false;
    f5::Body body{};
    rr::game::BodyMatrix48G savedInertiaMatrix60{};       // 2CA190: the primary's own inertia 100 (4x4)
    std::array<std::uint32_t,maxGroupMembers> positiveMember{};
    std::uint32_t positiveRoot=0;
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
    std::array<Group,maxGroups> groups{};
    std::size_t groupCount=0;
    std::uint64_t groupRefusals=0,rngDraws=0;
    // Free pieces on the original 166E40 frame: one D0 per live index
    // (bodies[i] belongs to pieces[i] only; reserved, never allocated per frame).
    std::array<f5::Body,512> bodies{};
    std::uint64_t pieceFlightSteps=0,pieceNonflightSteps=0,pieceFlightHits=0,pieceRefusals=0;
};
inline Sim& sim() noexcept { static Sim owner; return owner; }
// Capacity is reserved at race activation: frames never allocate.
inline constexpr std::size_t maxLivePieces=512;
static_assert(maxLivePieces<=std::tuple_size_v<decltype(Sim::bodies)>);
inline void resetCameraView() noexcept;
inline void resetSim() {
    resetCameraView();
    auto& s=sim();s.pieces.clear();s.pieces.reserve(maxLivePieces);
    s.steps=0;s.contacts=0;s.queries=0;s.invalid=0;s.capacity=0;s.unsupported=0;s.retired=0;s.summarized=false;
    s.activeCollections={};
    s.groups={};s.groupCount=0;s.groupRefusals=0;s.rngDraws=0;
    s.pieceFlightSteps=s.pieceNonflightSteps=s.pieceFlightHits=s.pieceRefusals=0;
    f5::stats()=f5::Stats{};
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
    live.inertiaDiag=mean>1e-4f?Vec{scale*(y*y+z*z),scale*(x*x+z*z),scale*(x*x+y*y)}:Vec{live.mass,live.mass,live.mass};
    live.drag=tuning?tuning->drag:0.f;live.rateDamping=tuning?tuning->rateDamping:0.f;
    live.radius=std::max(.5f,.5f*std::max(x,std::max(y,z)));
}

inline Vec cross(Vec a,Vec b){return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
inline Vec rotate(Quat q,Vec v){float m[16];toMatrix(q,{},m);
    return {m[0]*v.x+m[4]*v.y+m[8]*v.z,m[1]*v.x+m[5]*v.y+m[9]*v.z,m[2]*v.x+m[6]*v.y+m[10]*v.z};}
inline bool finite(Vec v){return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}

// 2CA620: the group body's mass, centre and inertia from the primary's saved
// values plus every attached member. Source order: 1E0 starts at the saved 50
// (zero), += each attached offset; mass += m_i; per axis I += I_i + o*(m*o)
// (diagonal only, as written); 1E0 *= 1/(30 + 1) with the REGISTERED count.
// Native: the body's rotational response uses the mean of that diagonal
// (isotropic, as tunePiece does for one piece).
inline bool recomputeGroup2CA620(Sim& s,Group& g) noexcept {
    float mass=g.savedMass4C;Vec inertia=g.savedInertia60;Vec center{};
    for(std::size_t i=0;i<g.capacity34;++i){
        if(!g.attachedA0[i]||g.member[i]>=s.pieces.size())continue;
        const auto& m=s.pieces[g.member[i]];const Vec o=g.offset48[i];
        center=center+o;mass+=m.mass;
        inertia={inertia.x+m.inertiaDiag.x+o.x*(m.mass*o.x),inertia.y+m.inertiaDiag.y+o.y*(m.mass*o.y),
                 inertia.z+m.inertiaDiag.z+o.z*(m.mass*o.z)};
    }
    const float divide=1.f/static_cast<float>(g.registered30+1u);center=center*divide;
    const float mean=(inertia.x+inertia.y+inertia.z)/3.f;
    if(!std::isfinite(mass)||!(mass>0.f)||!finite(center)||!finite(inertia)||!(mean>0.f)){
        ++s.groupRefusals;std::cerr<<"debris_group_refused racer="<<g.racer<<" c="<<g.collection+1<<" reason=2CA620_nonfinite\n";
        return false;}
    if(g.original){
        rr::game::BodyMatrix48G full=g.savedInertiaMatrix60;full.v[0]=inertia.x;full.v[5]=inertia.y;full.v[10]=inertia.z;
        const rr::game::Vector3_48C c{{center.x,center.y,center.z}};
        if(!f5::applyMassInertia(g.body,mass,full,c)){
            ++s.groupRefusals;std::cerr<<"debris_group_refused racer="<<g.racer<<" c="<<g.collection+1<<" reason=2CA620_inverse\n";
            return false;}
    }
    g.mass26C=mass;g.inertiaDiag=inertia;g.inertiaMean=mean;g.center1E0=center;
    return true;
}
// The piece's own E0 (m.COL type2 rays), or null when it has none usable.
inline const rr::game::Type2ResourceView49X* pieceE0(const LivePiece& p) noexcept {
    if(p.racer>=cold().racers.size())return nullptr;
    const auto& racer=cold().racers[p.racer];
    if(!racer.inventory||p.index>=racer.pieces.size())return nullptr;
    const auto& piece=racer.pieces[p.index];
    const auto* a=racer.inventory->pieceAssets(piece.collection,piece.piece);
    if(!a)return nullptr;const auto& v=a->queryE0();
    return v.rays&&v.rayCount&&v.rayCount<=f5::maxRays?&v:nullptr;
}
// One piece's D0/DC fields as the 22C6A0 activation leaves them: pose, the
// given world velocity 70 and LOCAL angular velocity B0, the 1653C0 limits,
// mass, the 295A40/22C310 DC tuning and the 22C310 drag/rate damping.
inline bool initOriginalBody(f5::Body& b,const LivePiece& p,Vec velocity,Vec localB0,
    const rr::game::OriginalWreckTuning50LA* tuning) noexcept {
    b=f5::Body{};
    toMatrix(p.orientation,p.position,b.root.scene.v);
    if(!rr::game::matrixToQuaternion48G(b.root.scene,b.root.body.orientation))return false;
    // D0.60 = the model origin (read by 167250's water test before the first
    // 167C40). STAND-IN: its value after 22C6A0 is unproven; the activation
    // pose's origin is used.
    b.root.body.position={{p.position.x,p.position.y,p.position.z}};
    // Fresh E8 state: vf0 vectors (W = 1), selected fraction 1.
    for(auto* v:{&b.e8.worldCenter,&b.e8.queryCenter,&b.e8.result00,&b.e8.result10,&b.e8.selectedNormal,
        &b.e8.selectedPoint,&b.e8.selectedCorrection})v->v[3]=1.f;
    b.root.body.velocity={{velocity.x,velocity.y,velocity.z}};
    b.root.body.angularVelocity={{localB0.x,localB0.y,localB0.z}};
    auto& q=b.parameters;
    q.linearAccelerationLimitSquared=std::bit_cast<float>(0x4963d240u);q.angularAccelerationLimitSquared=std::bit_cast<float>(0x48742400u);
    // 22C208 overwrites the 1653C0 default in each collection piece D0+27C.
    q.linearVelocityLimitSquared=std::bit_cast<float>(0x49034acfu);q.angularVelocityLimitSquared=std::bit_cast<float>(0x441c4000u);
    for(unsigned i=0;i<3;++i){q.translationEnabled[i]=true;q.rotationEnabled[i]=true;}
    q.mass=p.mass;q.inverseMass=1.f/p.mass;
    f5::tuneDc(b,tuning?tuning->friction:0.f);
    b.dragEC=b.baseDrag100=tuning?tuning->drag:0.f;b.rateDampingF0=b.baseRateDamping104=tuning?tuning->rateDamping:0.f;
    return true;
}
// F5: bind the group's original root body (2CA020 primary D0, 2CA480 member
// DC.30 := root D0). Every piece needs its own E0; otherwise the group keeps
// the native stand-in contact (logged).
inline void setupOriginalBody(Sim& s,Group& g) noexcept {
    const auto& racer=cold().racers[g.racer];
    const char* reason=nullptr;
    if(!f5::world().bound)reason="query_world_unbound";
    auto& root=s.pieces[g.root];
    if(!reason&&!(root.e0=pieceE0(root)))reason="root_e0";
    for(std::size_t i=0;!reason&&i<g.capacity34;++i)if(!(s.pieces[g.member[i]].e0=pieceE0(s.pieces[g.member[i]])))reason="member_e0";
    if(reason){std::cerr<<"debris_group_solver slot="<<g.racer<<" c="<<g.collection+1<<" solver=native_standin reason="<<reason<<'\n';return;}
    const auto* tuning=g.collection<racer.tuning.size()?&racer.tuning[g.collection]:nullptr;
    auto& b=g.body;
    // 340+B0 (local), as 226750/227370 copy it.
    if(!initOriginalBody(b,root,root.velocity,root.angular,tuning)){
        std::cerr<<"debris_group_solver slot="<<g.racer<<" c="<<g.collection+1<<" solver=native_standin reason=orientation\n";return;}
    auto& q=b.parameters;
    for(float& v:g.savedInertiaMatrix60.v)v=0.f;
    g.savedInertiaMatrix60.v[0]=root.inertiaDiag.x;g.savedInertiaMatrix60.v[5]=root.inertiaDiag.y;g.savedInertiaMatrix60.v[10]=root.inertiaDiag.z;g.savedInertiaMatrix60.v[15]=1.f;
    g.original=true;
    if(!recomputeGroup2CA620(s,g)){g.original=false;return;}
    std::cerr<<"debris_group_solver slot="<<g.racer<<" c="<<g.collection+1<<" solver=original root_rays="<<root.e0->rayCount
        <<" inv_inertia="<<q.inverseInertiaColumns[0].v[0]<<','<<q.inverseInertiaColumns[1].v[1]<<','<<q.inverseInertiaColumns[2].v[2]
        <<" friction="<<b.frictionByMaterial[0]<<'\n';
}
// A free piece (explosion piece, one-piece collection, released member) on
// its own original D0 for the 166E40 frame. velocity is world (70), localB0
// the body-local angular velocity (B0). The 166400 mass and diagonal inertia
// (inverse by 14E910) with the constructor's zero centre 1E0. Without a usable
// E0 or query world the piece keeps the native box probe (STAND-IN, logged).
inline bool setupPieceBody(Sim& s,std::size_t live,Vec velocity,Vec localB0,const char* route) noexcept {
    if(live>=s.pieces.size()||live>=s.bodies.size()){++s.pieceRefusals;
        std::cerr<<"debris_piece_solver live="<<live<<" solver=native_standin reason=body_capacity\n";return false;}
    auto& p=s.pieces[live];p.original=false;
    const auto& racer=cold().racers[p.racer];const auto& piece=racer.pieces[p.index];
    const auto* tuning=piece.collection<racer.tuning.size()?&racer.tuning[piece.collection]:nullptr;
    const char* reason=nullptr;
    if(!f5::world().bound)reason="query_world_unbound";
    else if(!(p.e0=pieceE0(p)))reason="piece_e0";
    auto& b=s.bodies[live];
    if(!reason&&!initOriginalBody(b,p,velocity,localB0,tuning))reason="orientation";
    if(!reason){
        rr::game::BodyMatrix48G inertia{};inertia.v[0]=p.inertiaDiag.x;inertia.v[5]=p.inertiaDiag.y;inertia.v[10]=p.inertiaDiag.z;inertia.v[15]=1.f;
        if(!f5::applyMassInertia(b,p.mass,inertia,rr::game::Vector3_48C{{0.f,0.f,0.f}}))reason="inverse_14E910";
    }
    if(reason){++s.pieceRefusals;
        std::cerr<<"debris_piece_solver slot="<<p.racer<<" piece="<<p.index<<" route="<<route<<" solver=native_standin reason="<<reason<<'\n';
        return false;}
    // 166400 1664B8..16652C: 118 = 1/sqrt(x*x + y*y + z*z) of the full model
    // extents (default 1 when zero). STAND-IN extents: the decoded LOD0 mesh
    // bounds (the same bounds tunePiece uses), not the 28D670 node bounds.
    {const auto& bb=piece.mesh.bounds;
     const float x=bb.maximum[0]-bb.minimum[0],y=bb.maximum[1]-bb.minimum[1],z=bb.maximum[2]-bb.minimum[2];
     volatile float xx=x*x,yy=y*y,zz=z*z,xy=xx+yy,sum=xy+zz;const float extent=std::sqrt(static_cast<float>(sum));
     p.inverseExtent118=extent>0.f?1.f/extent:1.f;}
    p.restitution110=tuning?tuning->restitution:0.f;p.frictionTorqueScale114=tuning?tuning->frictionTorqueScale:1.f;
    p.airborne11C=0.f;p.flightDelay120=.5f;p.settled=false;p.contactTime=0.f;
    p.original=true;
    std::cerr<<"debris_piece_solver slot="<<p.racer<<" piece="<<p.index<<" route="<<route<<" solver=original rays="<<p.e0->rayCount
        <<" flight="<<p.flight34<<p.flight38<<" e110="<<p.restitution110<<" k114="<<p.frictionTorqueScale114
        <<" inv118="<<p.inverseExtent118<<'\n';
    return true;
}
// 22C2B0 -> 2CC060 -> 2CA020 bind, 22C310 -> 2CA190 register, then (22C6A0
// tail) 2CA480 attach every member and 2CA620. The collection's pieces are
// live indices [first, first+count) in source order, piece 0 the primary.
// A collection whose loaded pieces differ from its authored list is never
// grouped (a missing piece would change its root or a member): refused, logged.
inline std::int32_t bindGroup(Sim& s,std::size_t slot,std::size_t collection,std::size_t first,std::size_t count,
    std::size_t authored) {
    if(count<2)return -1;                       // one-piece collection: capacity 0, no group frame
    if(count!=authored||count-1>maxGroupMembers||s.groupCount>=maxGroups||first+count>s.pieces.size()){
        ++s.groupRefusals;
        std::cerr<<"debris_group_refused racer="<<slot<<" c="<<collection+1<<" loaded="<<count<<" authored="<<authored
            <<" groups="<<s.groupCount<<'\n';
        return -1;
    }
    const auto& racer=cold().racers[slot];
    auto& g=s.groups[s.groupCount];g=Group{};
    g.racer=slot;g.collection=collection;g.root=first;g.capacity34=static_cast<std::uint32_t>(count-1);
    auto& root=s.pieces[first];
    g.savedMass4C=root.mass;g.savedInertia60=root.inertiaDiag;
    const auto& a0=racer.pieces[root.index].authored50;
    for(std::size_t k=1;k<count;++k){                // 2CA190: the primary is skipped
        auto& m=s.pieces[first+k];const auto& piece=racer.pieces[m.index];
        const std::size_t i=k-1;
        g.member[i]=first+k;g.breakRateA4[i]=piece.breakRate;g.breakScaleA8[i]=piece.breakScale;
        g.offset48[i]={piece.authored50[0]-a0[0],piece.authored50[1]-a0[1],piece.authored50[2]-a0[2]};
        ++g.registered30;
        g.attachedA0[i]=true;                        // 2CA480
        m.attached=true;m.group=static_cast<std::int32_t>(s.groupCount);m.velocity={};m.angular={};
    }
    root.groupRoot=true;root.group=static_cast<std::int32_t>(s.groupCount);
    if(!recomputeGroup2CA620(s,g)){
        for(std::size_t k=0;k<count;++k){auto& p=s.pieces[first+k];p.group=-1;p.groupRoot=p.attached=false;}
        g=Group{};return -1;
    }
    g.active=true;
    setupOriginalBody(s,g);
    std::cerr<<"debris_group slot="<<slot<<" c="<<collection+1<<" root="<<root.index<<" members="<<g.registered30
        <<" mass="<<g.mass26C<<" com="<<g.center1E0.x<<','<<g.center1E0.y<<','<<g.center1E0.z<<" inertia="<<g.inertiaMean;
    for(std::size_t i=0;i<g.capacity34;++i)std::cerr<<" [m"<<i<<" rate="<<g.breakRateA4[i]<<" scale="<<g.breakScaleA8[i]
        <<" off="<<g.offset48[i].x<<','<<g.offset48[i].y<<','<<g.offset48[i].z<<']';
    std::cerr<<'\n';
    return static_cast<std::int32_t>(s.groupCount++);
}
inline std::size_t authoredPieces(std::size_t slot,std::size_t collection) noexcept {
    const auto& racer=cold().racers[slot];
    if(!racer.inventory)return 0;
    const auto& authoring=racer.inventory->authoring();
    return collection<authoring.collections.size()?authoring.collections[collection].pieces.size():0;
}

// 22C6A0 -> 120070(E0, zone, mask 2) for every new piece [first, end): the
// shadow zone membership starts in the destroyed racer's primary zone. Zone 0
// (unknown) or a refusal keeps the storage-order stand-in for that piece.
inline void attachZones(Sim& s,std::size_t first,rr::game::CollectionRef50BX zone) noexcept {
    std::size_t attached=0,refused=0;
    for(std::size_t i=first;i<s.pieces.size();++i){
        const auto* e0=pieceE0(s.pieces[i]);if(!e0||!zone){++refused;continue;}
        rr::game::BodyMatrix48G world{};toMatrix(s.pieces[i].orientation,s.pieces[i].position,world.v);
        (zones::attach(i,zone,*e0,world)?attached:refused)++;
    }
    std::cerr<<"debris_zone_walk attach zone="<<zone<<" pieces="<<attached<<" standin="<<refused
        <<" bound="<<zones::walk().bound<<'\n';
}
// Spawns every loaded piece of the racer. bodyWorld is the pod's body288 world
// matrix; velocity/angular are its rigid-body velocities (340+70/+B0),
// worldAngularA0/bodyPosition its 340+A0 world rate and body position.
// Returns pieces spawned.
inline std::size_t spawn(std::size_t slot,bool explode,const float* bodyWorld,
    const float* velocity,const float* angular,const float* worldAngularA0,const float* containerWorld,
    const std::array<const float*,5>& collectionModels,
    std::uint32_t& sharedRaw,const float* cockpitVelocity=nullptr,const float* cockpitAngular=nullptr,
    std::size_t* skippedActive=nullptr,rr::game::CollectionRef50BX primaryZone=0) {
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
        if(piece.collection>=collectionModels.size()||!collectionModels[piece.collection])continue;
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
        }else if(piece.piece==0){
            // After 22C6A0, 226750 moves the primary onto the live model:
            // 1B4 (or 194), 1BC, 1B8, 1C4, 1C0 in collection order.
            const auto* model=collectionModels[piece.collection];
            live.position={model[12],model[13],model[14]};live.spawn=live.position;
            live.orientation=fromBasis(model);
            const Vec r=live.position-Vec{containerWorld[12],containerWorld[13],containerWorld[14]};
            const Vec w{worldAngularA0[0],worldAngularA0[1],worldAngularA0[2]};
            const Vec spin{w.y*r.z-w.z*r.y,w.z*r.x-w.x*r.z,w.x*r.y-w.y*r.x};
            live.velocity=Vec{velocity[0],velocity[1],velocity[2]}+spin;
            live.angular=Vec{angular[0],angular[1],angular[2]};
            // 226AD8..226B34: after a limp (states1/2) the cockpit collection
            // 374[0] takes the physical cockpit 35C's own 70/B0 instead.
            if(piece.collection==0u&&model==containerWorld){
                live.velocity={velocity[0],velocity[1],velocity[2]}; // 226C68
            }else if(cockpitVelocity&&cockpitAngular&&piece.collection==0u){
                live.velocity=Vec{cockpitVelocity[0],cockpitVelocity[1],cockpitVelocity[2]};
                live.angular=Vec{cockpitAngular[0],cockpitAngular[1],cockpitAngular[2]};
            }
        }
        // 22C6A0 (22C9B0..22CA00): every piece 34 = 38 = 1, then for a
        // collection of two or more pieces piece[0] 34 = 38 = 0; 22CA60
        // (22CAEC..22CB00) then sets piece[0].34 = 1 again (38 stays 0).
        const bool primaryOfGroup=piece.piece==0&&authoredPieces(slot,piece.collection)>=2;
        live.flight34=primaryOfGroup&&!explode?0u:1u;live.flight38=primaryOfGroup?0u:1u;
        if(s.pieces.size()>=maxLivePieces)break;
        s.pieces.push_back(live);++spawned;
    }
    // 226750 -> 22C6A0 tail per collection: 2CA480 + 2CA620. 2261B0 -> 22CA60
    // instead detaches every piece and deactivates the group (22CAD8), so an
    // explosion has no groups.
    const std::size_t first=s.pieces.size()-spawned;
    if(!explode){
        for(std::size_t i=first;i<s.pieces.size();){
            const auto collection=racer.pieces[s.pieces[i].index].collection;std::size_t n=0;
            while(i+n<s.pieces.size()&&racer.pieces[s.pieces[i+n].index].collection==collection)++n;
            bindGroup(s,slot,collection,i,n,authoredPieces(slot,collection));
            i+=n;
        }
    }
    attachZones(s,first,primaryZone);
    // Every piece outside a group runs its own 2CB8F0 -> 166E40 frame.
    for(std::size_t i=first;i<s.pieces.size();++i){
        const auto& p=s.pieces[i];if(p.attached||p.groupRoot)continue;
        setupPieceBody(s,i,p.velocity,p.angular,explode?"explode":"wreck");
    }
    return spawned;
}

// 227370/227A40 rows 11..20: activate the lost side's collections in place.
// Per collection (in 374 order: 374[1]+374[3] left, 374[2]+374[4] right):
// piece 0 takes the lost engine model's world pose (28ADD0/28A090) and the
// pod's point velocity at that engine, 70 + A0 x (engine - container194),
// with rate B0 + (0,0, sign*7.0*u) from ONE shared 308030 draw (227684).
// The collection's other pieces stand at their 22C6A0 places as attached
// group members (2CA480); from the next step they ride piece 0.
struct LimpCollection { std::size_t collection=0; const float* engineWorld=nullptr; float sign=1.f; };
inline std::size_t spawnLimp(std::size_t slot,const LimpCollection* list,std::size_t count,const float* bodyWorld,
    const float* velocity,const float* angular,const float* worldAngularA0,const float* container,
    std::uint32_t& sharedRaw,std::size_t& firstSpawned,rr::game::CollectionRef50BX primaryZone=0) {
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
        bool first=true;const std::size_t collectionFirst=s.pieces.size();
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
            // 22C6A0 flags (see spawn): piece[0] of a 2+ piece collection 0/0.
            const bool primaryOfGroup=piece.piece==0&&authoredPieces(slot,entry.collection)>=2;
            live.flight34=live.flight38=primaryOfGroup?0u:1u;
            s.pieces.push_back(live);++spawned;first=false;
        }
        bindGroup(s,slot,entry.collection,collectionFirst,s.pieces.size()-collectionFirst,authoredPieces(slot,entry.collection));
        attachZones(s,collectionFirst,primaryZone);
        for(std::size_t i=collectionFirst;i<s.pieces.size();++i){
            const auto& p=s.pieces[i];if(p.attached||p.groupRoot)continue;
            setupPieceBody(s,i,p.velocity,p.angular,"limp");
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

// excludeCategories: candidates whose category matches are skipped (the
// 166C60 E8 filter is mode 1 over {PHYSICSBODY, WATER}, 22C214..22C258).
inline bool segmentHit(Sim& s,Vec from,Vec to,rr::game::CollisionContact48I& hit,std::uint32_t excludeCategories=0u) noexcept {
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
        if(c.filter.categoryA4&excludeCategories)continue;
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

// Native box-support contact probe for one body at its pose (shared by free
// pieces, group roots and attached members). Fills the hit plane and the
// support distance; returns whether the box touches it this step.
struct Probe { bool touching=false; Vec normal{0.f,1.f,0.f},point{}; float need=0.f,above=0.f,below=0.f; };
inline Probe probeBody(Sim& s,const Piece& piece,Vec position,Quat orientation,Vec moved) noexcept {
    Probe r;
    r.below=support(piece,orientation,{0.f,-1.f,0.f});
    r.above=support(piece,orientation,{0.f,1.f,0.f});
    const float skin=.5f+.25f*r.below;
    constexpr float contactSlop=.5f;
    rr::game::CollisionContact48I hit{};
    if(segmentHit(s,position+Vec{0.f,r.above,0.f},moved-Vec{0.f,r.below+skin,0.f},hit)){
        Vec normal{hit.normal.v[0],hit.normal.v[1],hit.normal.v[2]};
        const float n=std::sqrt(dot(normal,normal));r.normal=n>0.f?normal*(1.f/n):Vec{0,1,0};
        r.point={hit.point.v[0],hit.point.v[1],hit.point.v[2]};
        r.need=support(piece,orientation,r.normal*-1.f);
        r.touching=dot(moved-r.point,r.normal)<r.need+contactSlop;
    }
    return r;
}

// 167640 settling shared by free pieces and group roots.
inline void settle(Sim& s,LivePiece& p,const rr::game::OriginalWreckTuning50LA* tuning,bool touching,
    Vec oldVelocity,Vec oldAngular,float dt) noexcept {
    constexpr float window=2.f,finalDrag=50.f,finalRate=200.f; // 22C5xx piece defaults (130/10C/108)
    constexpr float stopSpeed=10.f,stopSpin=.5f,stopAcceleration=10.f,stopAngularAcceleration=1.f; // 134..140
    // The original's spring contact stays set while a piece rests or skids;
    // these impulse contacts can skip steps on a slope, so a contact within
    // the last 0.25 s still counts as contact.
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
}
inline Quat integrate(Quat q,Vec w,float dt) noexcept {
    // q += 0.5 * (0,w) * q * dt.
    const Quat dq=multiply({w.x,w.y,w.z,0.f},q);
    return normalized({q.x+.5f*dq.x*dt,q.y+.5f*dq.y*dt,q.z+.5f*dq.z*dt,q.w+.5f*dq.w*dt});
}

// One free (ungrouped or released) piece.
inline void stepFree(Sim& s,LivePiece& p,float dt) noexcept {
    const auto& racer=cold().racers[p.racer];
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
    const Vec moved=p.position+p.velocity*dt;
    const Probe c=probeBody(s,piece,p.position,p.orientation,moved);
    if(c.touching){
        ++s.contacts;++p.contacts;p.lastContactStep=static_cast<std::uint32_t>(s.steps);
        const float into=dot(p.velocity,c.normal);
        // Within the slop counts as touching (settling); only a move that
        // would penetrate gets the contact response, so nothing hovers.
        if(into<0.f&&dot(moved-c.point,c.normal)<c.need){
            // Resting contact: an approach slower than two steps of gravity
            // is absorbed (no micro-bounce), larger impacts use Restitution.
            const float restingSpeed=2.f*std::fabs(racer.gravityAcceleration)*dt;
            const float restitution=-into<=restingSpeed?0.f:tuning?std::clamp(tuning->restitution,0.f,1.f):.3f;
            const float friction=tuning?std::max(tuning->friction,0.f):.5f;
            const Vec normalPart=c.normal*into,tangent=p.velocity-normalPart;
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
        const float gap=dot(next-c.point,c.normal);
        p.position=gap<c.need?next+c.normal*(c.need-gap):next;
    }else p.position=moved;
    settle(s,p,tuning,c.touching,oldVelocity,oldAngular,dt);
    p.orientation=integrate(p.orientation,p.angular,dt);
    if(!std::isfinite(p.position.x)||!std::isfinite(p.position.y)||!std::isfinite(p.position.z)||
       p.position.y+c.above<s.floor){
        p.retired=true;++s.retired;p.velocity={};p.angular={};
    }
}

// 2CA840 nonflight frame of one group: the root body carries the primary and
// every attached member. Forces (167250) act on the aggregate mass; the root
// and then each attached member are probed at their own poses and every
// response acts on the root body, with its lever arm about the centre 1E0.
// Native contact (as for free pieces), NOT the original DC spring/impulse
// solver (166E40/2964C0): the per-axis peak C0 counts only impacts faster
// than the resting threshold, standing in for 1C0's impulse part.
inline void stepGroupBody(Sim& s,Group& g,float dt) noexcept {
    auto& root=s.pieces[g.root];
    const auto& racer=cold().racers[root.racer];
    const auto* tuning=g.collection<racer.tuning.size()?&racer.tuning[g.collection]:nullptr;
    g.touched={};g.peakC0={};g.peakRaw={};
    g.attached38=0;for(std::size_t i=0;i<g.capacity34;++i)g.attached38+=g.attachedA0[i];
    if(root.settled||root.retired)return;                  // root disabled (204 = 0): 2CA8F0 skip
    const float M=g.mass26C,I=g.inertiaMean;
    const Vec oldVelocity=root.velocity,oldAngular=root.angular;
    Vec v=root.velocity,w=root.angular;
    const Vec center=root.position+rotate(root.orientation,g.center1E0);
    v.y-=std::fabs(racer.gravityAcceleration)*dt;
    {const float speed=std::sqrt(dot(v,v));v=v*(1.f/(1.f+std::max(root.drag,0.f)/M*speed*dt));}
    w=w*(1.f/(1.f+std::max(root.rateDamping,0.f)/I*dt));
    const float restingSpeed=2.f*std::fabs(racer.gravityAcceleration)*dt;
    const float restitutionAuthored=tuning?std::clamp(tuning->restitution,0.f,1.f):.3f;
    const float friction=tuning?std::max(tuning->friction,0.f):.5f;
    Vec shift{};bool touching=false;
    auto respond=[&](const LivePiece& body,std::int32_t member){
        const auto& piece=racer.pieces[body.index];
        const Vec arm=body.position-center;
        const Vec moved=body.position+(v+cross(w,arm))*dt;
        const Probe c=probeBody(s,piece,body.position,body.orientation,moved);
        if(!c.touching)return;
        touching=true;++s.contacts;
        if(member>=0)g.touched[static_cast<std::size_t>(member)]=true; // member DC.3C
        const Vec pointVelocity=v+cross(w,arm);
        const float into=dot(pointVelocity,c.normal);
        if(into<0.f&&dot(moved-c.point,c.normal)<c.need){
            const float restitution=-into<=restingSpeed?0.f:restitutionAuthored;
            const Vec r=body.position-c.normal*c.need-center;   // the box's support point
            const Vec rn=cross(r,c.normal);
            const float kn=1.f/M+dot(rn,rn)/I;
            const float jn=(1.f+restitution)*-into/kn;
            Vec dv=c.normal*(jn/M),dw=rn*(jn/I);
            const Vec tangent=pointVelocity-c.normal*into;
            const float tangentSpeed=std::sqrt(dot(tangent,tangent));
            if(tangentSpeed>0.f){
                const Vec t=tangent*(1.f/tangentSpeed);const Vec rt=cross(r,t);
                const float kt=1.f/M+dot(rt,rt)/I;const float jt=std::min(friction*jn,tangentSpeed/kt);
                dv=dv-t*(jt/M);dw=dw-rt*(jt/I);
            }
            v=v+dv;w=w+dw;
            g.peakRaw={std::max(g.peakRaw.x,std::fabs(dv.x)),std::max(g.peakRaw.y,std::fabs(dv.y)),std::max(g.peakRaw.z,std::fabs(dv.z))};
            if(-into>restingSpeed)
                g.peakC0={std::max(g.peakC0.x,std::fabs(dv.x)),std::max(g.peakC0.y,std::fabs(dv.y)),std::max(g.peakC0.z,std::fabs(dv.z))};
            // Same native ground spin loss per contact step as a free piece.
            w=w*std::max(0.f,1.f-std::min(1.f,friction*.5f));
        }
        const Vec next=body.position+(v+cross(w,arm))*dt+shift;
        const float gap=dot(next-c.point,c.normal);
        if(gap<c.need)shift=shift+c.normal*(c.need-gap);
    };
    respond(root,-1);
    for(std::size_t i=0;i<g.capacity34;++i)
        if(g.attachedA0[i]&&g.member[i]<s.pieces.size())respond(s.pieces[g.member[i]],static_cast<std::int32_t>(i));
    if(touching)root.lastContactStep=static_cast<std::uint32_t>(s.steps);
    if(touching)++root.contacts;
    // Integrate about the centre, then place the primary's origin.
    const Vec nextCenter=center+v*dt+shift;
    root.orientation=integrate(root.orientation,w,dt);
    root.position=nextCenter-rotate(root.orientation,g.center1E0);
    root.velocity=v;root.angular=w;
    settle(s,root,tuning,touching,oldVelocity,oldAngular,dt);
    const float above=support(racer.pieces[root.index],root.orientation,{0.f,1.f,0.f});
    if(!finite(root.position)||root.position.y+above<s.floor){
        root.retired=true;++s.retired;root.velocity={};root.angular={};
        for(std::size_t i=0;i<g.capacity34;++i)if(g.attachedA0[i]&&g.member[i]<s.pieces.size()){
            auto& m=s.pieces[g.member[i]];m.retired=true;++s.retired;}
    }
}

// F5: 2CA840 nonflight frame of a group on the original solver.
inline std::uint32_t wreckIdentity(const Group& g,std::size_t slotInGroup) noexcept {
    return 0xf5d00000u|(static_cast<std::uint32_t>(g.racer&7u)<<12)|(static_cast<std::uint32_t>(g.collection&15u)<<8)|
        static_cast<std::uint32_t>(slotInGroup&0xffu);
}
inline void refuseOriginal(Sim& s,Group& g,const char* stage) noexcept {
    ++s.groupRefusals;g.original=false;
    std::cerr<<"debris_group_solver slot="<<g.racer<<" c="<<g.collection+1<<" solver=native_standin reason="<<stage
        <<" step="<<s.steps<<'\n';
}
inline void syncRootPose(LivePiece& root,const f5::Body& b) noexcept {
    root.orientation=fromBasis(b.root.scene.v);
    root.position={b.root.scene.v[12],b.root.scene.v[13],b.root.scene.v[14]};
    root.velocity={b.root.body.velocity.v[0],b.root.body.velocity.v[1],b.root.body.velocity.v[2]};
    const auto& a=b.root.worldAngularVelocityA0.v;root.angular={a[0],a[1],a[2]};
}
// 166C60 movement correction of a D0's model origin (one E8 type3 query,
// previous80 -> current90; a hit moves the origin to point + 1.0 x normal and
// 80 := corrected, a miss 80 := current; velocity untouched). The query is the
// original type3 kernel (f5::queryMove). A refused query keeps the position
// (counted, logged once). The extra zero-length guard is native (a zero
// segment cannot hit). Returns whether it corrected.
inline bool movement166C60(Sim&,f5::Body& b,std::uint32_t identity) noexcept {
    auto& tr=b.root.scene.v;const Vec current{tr[12],tr[13],tr[14]};
    if(b.previous80[0]==0.f&&b.previous80[1]==0.f&&b.previous80[2]==0.f)b.previous80={current.x,current.y,current.z,1.f};
    const Vec previous{b.previous80[0],b.previous80[1],b.previous80[2]};
    const Vec d=current-previous;
    rr::game::Vector3_48C point{},normal{};
    const int hit=dot(d,d)>0.f?f5::queryMove({{previous.x,previous.y,previous.z}},{{current.x,current.y,current.z}},
        identity,b.e8,point,normal):0;
    if(hit==1){
        volatile float cx=normal.v[0]*1.f,cy=normal.v[1]*1.f,cz=normal.v[2]*1.f; // 106790: 433B60 = 1.0 x normal
        const Vec corrected{point.v[0]+cx,point.v[1]+cy,point.v[2]+cz};
        tr[12]=corrected.x;tr[13]=corrected.y;tr[14]=corrected.z;b.previous80={corrected.x,corrected.y,corrected.z,1.f};
        return true;
    }
    b.previous80={current.x,current.y,current.z,1.f};return false;
}
inline void stepGroupOriginal(Sim& s,Group& g,float dt) noexcept {
    auto& root=s.pieces[g.root];auto& b=g.body;auto& ws=f5::workspace();
    const auto& racer=cold().racers[root.racer];
    g.touched={};g.attached38=0;g.positiveRoot=0;g.positiveMember={};
    if(root.settled||root.retired||!b.enabled204)return;   // 2CA8F0: root disabled
    if(g.capacity34+1u>f5::maxDcs){refuseOriginal(s,g,"dc_capacity");return;}
    b.measured124=0.f;b.measured128=0.f;
    // D8 detectors: the root, then every attached member at its attached pose.
    std::array<rr::game::BodyMatrix48G,maxGroupMembers> memberScene{};
    f5::freshDc(ws.dc[0]);
    if(!f5::detect(*root.e0,b.root.scene,wreckIdentity(g,0),ws.dc[0],s.track,g.root)){refuseOriginal(s,g,"root_detector");return;}
    for(std::size_t i=0;i<g.capacity34;++i){
        if(!g.attachedA0[i]||g.member[i]>=s.pieces.size())continue;
        const auto& m=s.pieces[g.member[i]];toMatrix(m.orientation,m.position,memberScene[i].v);
        f5::freshDc(ws.dc[i+1]);
        if(!m.e0||!f5::detect(*m.e0,memberScene[i],wreckIdentity(g,i+1),ws.dc[i+1],s.track,g.member[i])){refuseOriginal(s,g,"member_detector");return;}
        ++g.attached38;
    }
    b.impulseC0={};                                         // 2CAA78: 4441F0 XYZ = 0
    const float maximum=std::bit_cast<float>(0x3c888889u);  // 444230 = 1/60
    float elapsed=0.f,step=rr::game::detail::scalarMin48O(dt,maximum);
    while(elapsed<dt){
        if(!(step>0.f)){refuseOriginal(s,g,"substep");return;}
        if(!f5::forces(b,b.parameters.mass,racer.gravityAcceleration)){refuseOriginal(s,g,"forces_167250");return;}
        if(!f5::respond(b,ws.dc[0],b.root.scene)){refuseOriginal(s,g,"root_2964C0");return;}
        g.positiveRoot+=static_cast<std::uint32_t>(ws.dc[0].positive);
        for(std::size_t i=0;i<g.capacity34;++i){
            if(!g.attachedA0[i]||g.member[i]>=s.pieces.size())continue;
            if(!f5::respond(b,ws.dc[i+1],memberScene[i])){refuseOriginal(s,g,"member_2964C0");return;}
            g.positiveMember[i]+=static_cast<std::uint32_t>(ws.dc[i+1].positive);
        }
        f5::retain(b);
        if(!f5::integrate(b,step)){refuseOriginal(s,g,"integrate_167C40");return;}
        volatile float next=elapsed+step;elapsed=next;
        volatile float left=dt-elapsed;step=rr::game::detail::scalarMin48O(maximum,left);
    }
    if(movement166C60(s,b,wreckIdentity(g,0)|0x00080000u))
        std::cerr<<"debris_group_movement slot="<<g.racer<<" c="<<g.collection+1<<" step="<<s.steps<<" corrected\n";
    if(!f5::settle(b,ws.dc[0].state,dt)){refuseOriginal(s,g,"settle_167640");return;}
    // Diagnostic (stand-in item f, log only): once per second while the root
    // is still enabled, the first 167640 stop test that fails (speed < 10,
    // spin < 0.5, |124| < 10, |128| < 1, then DC.3C) and the root DC's record
    // materials (friction applies on material 0 only).
    if(b.enabled204&&s.steps%60==0){
        const auto& w=b.root.body.angularVelocity.v;const float spin=std::sqrt(w[0]*w[0]+w[1]*w[1]+w[2]*w[2]);
        const char* gate=!(b.cachedSpeed34<b.stopSpeed134)?"speed":!(spin<b.stopAngularSpeed138)?"spin":
            !(b.measured124<b.stopLinearAcceleration13C)?"a124":!(b.measured128<b.stopAngularAcceleration140)?"a128":
            !ws.dc[0].state.contact3C?"contact":"none";
        std::uint32_t m0=0,other=0;
        for(std::int32_t r=0;r<ws.dc[0].state.generatedCount84&&r<std::int32_t(ws.dc[0].records.size());++r)
            (ws.dc[0].records[std::size_t(r)].material4C==0?m0:other)++;
        std::cerr<<"debris_rest_gate slot="<<g.racer<<" c="<<g.collection+1<<" step="<<s.steps<<" fails="<<gate
            <<" speed="<<b.cachedSpeed34<<" spin="<<spin<<" a124="<<b.measured124<<" a128="<<b.measured128
            <<" dc3c="<<ws.dc[0].state.contact3C<<" t12c="<<b.contactTime12C<<" records_m0="<<m0<<" records_other="<<other<<'\n';
    }
    for(std::size_t i=0;i<g.capacity34;++i)if(g.attachedA0[i])g.touched[i]=ws.dc[i+1].state.contact3C!=0u; // member DC.3C
    syncRootPose(root,b);
    if(ws.dc[0].state.contact3C){root.lastContactStep=static_cast<std::uint32_t>(s.steps);++root.contacts;++s.contacts;}
    if(!b.enabled204){root.settled=true;root.velocity={};root.angular={};}
    const float above=support(racer.pieces[root.index],root.orientation,{0.f,1.f,0.f});
    if(!finite(root.position)||root.position.y+above<s.floor){
        root.retired=true;++s.retired;root.velocity={};root.angular={};
        for(std::size_t i=0;i<g.capacity34;++i)if(g.attachedA0[i]&&g.member[i]<s.pieces.size()){
            auto& m=s.pieces[g.member[i]];m.retired=true;++s.retired;}
    }
}

inline std::uint32_t pieceIdentity(const LivePiece& p) noexcept {
    return 0xf5e00000u|(static_cast<std::uint32_t>(p.racer&7u)<<12)|static_cast<std::uint32_t>(p.index&0xfffu);
}
// A free piece whose original frame was refused continues on the native box
// probe from its current original state (logged).
inline void refusePiece(Sim& s,LivePiece& p,const f5::Body& b,const char* stage) noexcept {
    ++s.pieceRefusals;p.original=false;
    syncRootPose(p,b);
    std::cerr<<"debris_piece_solver slot="<<p.racer<<" piece="<<p.index<<" solver=native_standin reason="<<stage
        <<" step="<<s.steps<<'\n';
}
// 166790 flight: 167250 forces, ONE full-dt 167C40 integration, DC.3C = 0, then
// the repeated E8 movement query from the pre-integration origin to the current
// one. Each hit sets DC.3C and A0 = normal; |v.n| < 50 (433B58) clears 38;
// while 38 stays set the velocity is reflected with Restitution (110), the
// spin becomes normalize(n x (v' - p)) in body-local, scaled by 118 x (114 x
// speed34); the origin is mirrored to current + 2 max((hit - current).n,
// 0.025) n. The E8 query is the original type3 kernel (f5::queryMove, its
// own stand-ins listed there). STAND-INS: 28BBB0 is the rotation transpose;
// the source loop has no cap, this one stops after 16 hits (logged); a
// refused query ends the loop as a miss (counted, logged once).
inline bool pieceFlight166790(Sim& s,LivePiece& p,f5::Body& b,rr::game::SpringRecordState48N& dc,float dt) noexcept {
    const auto& racer=cold().racers[p.racer];
    auto& tr=b.root.scene.v;const Vec before{tr[12],tr[13],tr[14]};
    if(!f5::forces(b,b.parameters.mass,racer.gravityAcceleration))return false;
    if(!f5::integrate(b,dt))return false;
    dc.contact3C=0;
    for(unsigned loop=0;;++loop){
        const Vec current{tr[12],tr[13],tr[14]};const Vec d=current-before;const float cur[3]={current.x,current.y,current.z};
        rr::game::Vector3_48C hitPoint{},hitNormal{};
        const int found=dot(d,d)>0.f?f5::queryMove({{before.x,before.y,before.z}},{{current.x,current.y,current.z}},
            pieceIdentity(p)|0x00080000u,b.e8,hitPoint,hitNormal):0;
        if(found!=1)return true;                                     // miss, or refused (counted, logged once)
        if(loop>=16){std::cerr<<"debris_piece_flight_cap slot="<<p.racer<<" piece="<<p.index<<" step="<<s.steps<<'\n';return true;}
        ++s.pieceFlightHits;dc.contact3C=1;
        f5::detail::Math m;
        const f5::V3 n=hitNormal;
        auto& v=b.root.body.velocity;
        const float normalSpeed=std::fabs(m.dot(n,v));
        if(normalSpeed<50.f){
            p.flight38=0;
            std::cerr<<"debris_piece_flight slot="<<p.racer<<" piece="<<p.index<<" step="<<s.steps<<" landed normal_speed="<<normalSpeed
                <<" y="<<current.y<<'\n';
        }
        if(p.flight38){
            const float along=m.dot(v,n);                                   // 106E70(v, n)
            f5::V3 projected{};for(unsigned i=0;i<3;++i)projected.v[i]=m.mul(along,n.v[i]);
            const float rebound=-m.add(1.f,p.restitution110);
            for(unsigned i=0;i<3;++i){b.impulseC0.v[i]=m.mul(rebound,projected.v[i]);v.v[i]=m.add(v.v[i],b.impulseC0.v[i]);}
            f5::V3 tangent{};for(unsigned i=0;i<3;++i)tangent.v[i]=m.sub(v.v[i],projected.v[i]); // v' - p (source order)
            const f5::V3 spin=m.normalize(m.cross(n,tangent));
            const f5::V3 local=m.inverseDirection(b.root.scene,spin);
            const float scale=m.mul(p.inverseExtent118,m.mul(p.frictionTorqueScale114,b.cachedSpeed34));
            for(unsigned i=0;i<3;++i)b.root.body.angularVelocity.v[i]=m.mul(local.v[i],scale);
        }
        f5::V3 delta{};for(unsigned i=0;i<3;++i)delta.v[i]=m.sub(hitPoint.v[i],cur[i]);
        const float distance=rr::game::detail::scalarMax48O(m.dot(delta,n),std::bit_cast<float>(0x3ccccccdu)); // 433B50
        const float twice=m.mul(2.f,distance);
        for(unsigned i=0;i<3;++i)tr[12+i]=m.add(cur[i],m.mul(twice,n.v[i]));
        if(!m.valid)return false;
    }
}
// 2CB8F0 -> 166E40 for one free piece: 124/128 = 0; disabled bodies skip;
// 34 && 38 take the 166790 flight; otherwise 166F8C: the piece's own D8/E0
// detector, substeps of at most 1/60 (433B68) of 167250 forces, its own DC
// 2964C0 response, the per-axis |1C0| retention into C0, 167C40 and the
// 124/128 maxima, then 166C60 and, when 34 is set, the 11C airborne timer
// (reset on DC.3C, 38 = 1 once 11C > 120). 167640 settling closes both.
inline void stepFreeOriginal(Sim& s,LivePiece& p,f5::Body& b,float dt) noexcept {
    if(!b.enabled204){p.settled=true;return;}
    const auto& racer=cold().racers[p.racer];
    b.measured124=0.f;b.measured128=0.f;
    auto& dc=f5::workspace().dc[0];
    if(p.flight34&&p.flight38){
        ++s.pieceFlightSteps;
        if(!pieceFlight166790(s,p,b,dc.state,dt)){refusePiece(s,p,b,"flight_166790");return;}
    }else{
        ++s.pieceNonflightSteps;
        f5::freshDc(dc);
        const std::uint32_t identity=pieceIdentity(p);
        if(!p.e0||!f5::detect(*p.e0,b.root.scene,identity,dc,s.track,static_cast<std::size_t>(&p-s.pieces.data()))){refusePiece(s,p,b,"detector_295630");return;}
        b.impulseC0={};                                              // 166FC4: 433B10
        const float maximum=std::bit_cast<float>(0x3c888889u);      // 433B68 = 1/60
        float elapsed=0.f,step=rr::game::detail::scalarMin48O(dt,maximum);
        while(elapsed<dt){
            if(!(step>0.f)){refusePiece(s,p,b,"substep");return;}
            if(!f5::forces(b,b.parameters.mass,racer.gravityAcceleration)){refusePiece(s,p,b,"forces_167250");return;}
            if(!f5::respond(b,dc,b.root.scene)){refusePiece(s,p,b,"response_2964C0");return;}
            f5::retain(b);
            if(!f5::integrate(b,step)){refusePiece(s,p,b,"integrate_167C40");return;}
            volatile float next=elapsed+step;elapsed=next;
            volatile float left=dt-elapsed;step=rr::game::detail::scalarMin48O(maximum,left);
        }
        movement166C60(s,b,identity|0x00080000u);
        if(p.flight34){                                              // 1671C8..167214
            if(dc.state.contact3C)p.airborne11C=0.f;
            else{volatile float t=p.airborne11C+dt;p.airborne11C=t;}
            if(!(p.airborne11C<=p.flightDelay120)){
                p.flight38=1;
                std::cerr<<"debris_piece_flight slot="<<p.racer<<" piece="<<p.index<<" step="<<s.steps<<" airborne t="<<p.airborne11C<<'\n';
            }
        }
    }
    if(!f5::settle(b,dc.state,dt,p.flight34,p.flight38)){refusePiece(s,p,b,"settle_167640");return;}
    syncRootPose(p,b);
    if(dc.state.contact3C){p.lastContactStep=static_cast<std::uint32_t>(s.steps);++p.contacts;++s.contacts;}
    if(!b.enabled204){
        p.settled=true;p.velocity={};p.angular={};
        std::cerr<<"debris_piece_settled slot="<<p.racer<<" piece="<<p.index<<" step="<<s.steps
            <<" pos="<<p.position.x<<','<<p.position.y<<','<<p.position.z<<" moved="<<std::sqrt(dot(p.position-p.spawn,p.position-p.spawn))<<'\n';
    }
    // STAND-IN retire (as stepFree): below every track collision bound.
    const float above=support(racer.pieces[p.index],p.orientation,{0.f,1.f,0.f});
    if(!finite(p.position)||p.position.y+above<s.floor){p.retired=true;++s.retired;p.velocity={};p.angular={};}
}

// 2CAEE8..2CB340: member i breaks off, in source order. Each break sees the
// root's velocity and mass as changed by the earlier breaks this frame.
inline void breakMember(Sim& s,Group& g,std::size_t i,std::uint32_t& sharedRaw,std::uint64_t frame) noexcept {
    auto& root=s.pieces[g.root];auto& m=s.pieces[g.member[i]];
    const Vec off=g.offset48[i];
    // 2CAF44..2CAF58: the VU0 outer product of the root's B0 with the
    // UNROTATED authored offset (source quirk, copied as written).
    const auto& B0=g.body.root.body.angularVelocity.v;const auto& V70=g.body.root.body.velocity.v;
    const Vec rootW=g.original?Vec{B0[0],B0[1],B0[2]}:root.angular;
    const Vec rootV=g.original?Vec{V70[0],V70[1],V70[2]}:root.velocity;
    const Vec vp=cross(rootW,off)+rootV;
    const float speed=g.original?g.body.cachedSpeed34:std::sqrt(dot(root.velocity,root.velocity)); // root 34
    const float u1=drawUnit(sharedRaw),u2=drawUnit(sharedRaw),u3=drawUnit(sharedRaw);
    const float scaleJ=speed*.5f*g.breakScaleA8[i];                          // 444238 = 0.5
    const Vec J{scaleJ*u1,scaleJ*u2,scaleJ*u3};
    const float rootInverse=g.original?g.body.parameters.inverseMass:1.f/g.mass26C,memberInverse=1.f/m.mass; // 270s (root: current aggregate)
    const float k=1.f/(rootInverse+memberInverse),fm=k*memberInverse,fr=rootInverse*k;
    const Vec rootBefore=rootV;const float massBefore=g.mass26C;
    m.velocity=vp+J*fm;const Vec rootAfter=rootV-J*fr;root.velocity=rootAfter;
    const float u4=drawUnit(sharedRaw),u5=drawUnit(sharedRaw),u6=drawUnit(sharedRaw);
    const float scaleS=25.f*g.breakScaleA8[i];                               // 444240 = 25.0
    const Vec S{scaleS*u4,scaleS*u5,scaleS*u6};
    const Vec memberB0=rootW+S*fm,rootB0=rootW-S*fr;
    if(g.original){
        // The member's B0 is local to its attached pose (the root's rotation).
        // Its own D0 starts from that pose for its 166E40 frame; if that is
        // refused, the native free piece integrates the world rate.
        m.angular=rotate(m.orientation,memberB0);
        auto& rb=g.body.root.body;rb.velocity={{rootAfter.x,rootAfter.y,rootAfter.z}};rb.angularVelocity={{rootB0.x,rootB0.y,rootB0.z}};
        root.angular=rotate(root.orientation,rootB0);
    }else{m.angular=memberB0;root.angular=rootB0;}
    s.rngDraws+=6;g.draws+=6;++g.breaks;
    // 2CB308 member body enabled; 2CA2F0 detach: own contact, own frame from
    // the next frame (mask20 |= 2), A0[i] = 0, then 2CA620. The member keeps
    // 34 = 38 = 1 from 22C6A0: it starts on the 166790 flight path.
    m.attached=false;m.group=-1;m.releasedStep=s.steps;m.settled=false;m.contactTime=0.f;
    m.flight34=m.flight38=1u;
    if(g.original)setupPieceBody(s,g.member[i],m.velocity,memberB0,"release");
    g.attachedA0[i]=false;
    recomputeGroup2CA620(s,g);
    const Vec jm=(m.velocity-vp)*m.mass,jr=(rootBefore-root.velocity)*massBefore;
    const Vec gap=jm-jr;const float jLength=std::sqrt(dot(J,J));
    std::cerr<<"debris_break slot="<<g.racer<<" c="<<g.collection+1<<" i="<<i<<" step="<<frame
        <<" v_point="<<vp.x<<','<<vp.y<<','<<vp.z<<" v_member="<<m.velocity.x<<','<<m.velocity.y<<','<<m.velocity.z
        <<" v_root="<<rootBefore.x<<','<<rootBefore.y<<','<<rootBefore.z<<"->"<<root.velocity.x<<','<<root.velocity.y<<','<<root.velocity.z
        <<" w_member="<<m.angular.x<<','<<m.angular.y<<','<<m.angular.z
        <<" mass="<<massBefore<<"->"<<g.mass26C<<" com="<<g.center1E0.x<<','<<g.center1E0.y<<','<<g.center1E0.z
        <<" draws="<<u1<<','<<u2<<','<<u3<<','<<u4<<','<<u5<<','<<u6
        <<" momentum_err="<<(jLength>0.f?std::sqrt(dot(gap,gap))/(jLength*m.mass):0.f)<<'\n';
}

// 2CADDC..2CB3E4: the member loop, once per group frame. A member touching
// something this frame breaks with probability min(0.02 |C0|, 1) x BreakRate
// (one shared 308030 draw per touching attached member); otherwise it takes
// the root's rotation and root + R x offset48 (2CB348..2CB3CC).
inline void stepGroupMembers(Sim& s,Group& g,std::uint32_t& sharedRaw) noexcept {
    auto& root=s.pieces[g.root];
    if(root.settled||root.retired)return;
    const Vec c0=g.original?Vec{g.body.impulseC0.v[0],g.body.impulseC0.v[1],g.body.impulseC0.v[2]}:g.peakC0;
    const float peak=std::sqrt(dot(c0,c0));
    const float scaled=std::min(std::bit_cast<float>(0x3ca3d70au)*peak,1.f);
    for(std::size_t i=0;i<g.capacity34;++i){
        if(!g.attachedA0[i]||g.member[i]>=s.pieces.size())continue;
        g.latchB0[i]=false;
        if(g.touched[i]){
            const float u=drawUnit(sharedRaw);++s.rngDraws;++g.draws;++g.tests;
            const float p=scaled*g.breakRateA4[i];
            if(u<p)g.latchB0[i]=true;
            std::cerr<<"debris_break_test slot="<<g.racer<<" c="<<g.collection+1<<" i="<<i<<" step="<<s.steps
                <<" solver="<<(g.original?"original":"native")<<" peak_dv_raw="<<std::sqrt(dot(g.peakRaw,g.peakRaw))
                <<" peak_dv_impact="<<peak<<" s="<<scaled
                <<" u="<<u<<" p="<<p<<" break="<<g.latchB0[i]<<'\n';
        }
        if(g.latchB0[i]){breakMember(s,g,i,sharedRaw,s.steps);continue;}
        auto& m=s.pieces[g.member[i]];
        m.orientation=root.orientation;m.position=root.position+rotate(root.orientation,g.offset48[i]);
        m.velocity=root.velocity+cross(root.angular,rotate(root.orientation,g.offset48[i]));m.angular=root.angular;
    }
}

// One native step for every live piece. Free pieces (their own 2CB8F0
// frames) run first, then each group's frame, as the collection's child
// list orders pieces before the group. sharedRaw is the race's 308030 stream.
inline void step(float dt,std::uint32_t& sharedRaw) noexcept {
    auto& s=sim();if(s.pieces.empty())return;++s.steps;
    const auto drawsBefore=s.rngDraws;std::uint64_t tests=0,breaks=0;
    for(std::size_t i=0;i<s.pieces.size();++i){
        auto& p=s.pieces[i];
        if(p.settled||p.retired||p.attached||p.groupRoot)continue;
        if(p.releasedStep==s.steps)continue;                  // released this step: from the next
        if(p.original&&i<s.bodies.size())stepFreeOriginal(s,p,s.bodies[i],dt);
        else stepFree(s,p,dt);
    }
    for(std::size_t gi=0;gi<s.groupCount;++gi){
        auto& g=s.groups[gi];if(!g.active||g.root>=s.pieces.size())continue;
        const auto t0=g.tests,b0=g.breaks;
        if(g.original)stepGroupOriginal(s,g,dt);
        if(!g.original)stepGroupBody(s,g,dt);
        stepGroupMembers(s,g,sharedRaw);
        tests+=g.tests-t0;breaks+=g.breaks-b0;
        if(s.steps%15==1&&!s.pieces[g.root].settled){
            const auto& root=s.pieces[g.root];float offsetError=0.f,rotationDot=1.f;std::uint32_t attached=0;
            for(std::size_t i=0;i<g.capacity34;++i){if(!g.attachedA0[i])continue;++attached;
                const auto& m=s.pieces[g.member[i]];const Vec want=root.position+rotate(root.orientation,g.offset48[i]);
                const Vec e=m.position-want;offsetError=std::max(offsetError,std::sqrt(dot(e,e)));
                const float d=std::fabs(m.orientation.x*root.orientation.x+m.orientation.y*root.orientation.y+
                    m.orientation.z*root.orientation.z+m.orientation.w*root.orientation.w);
                rotationDot=std::min(rotationDot,d);}
            std::cerr<<"debris_group_pose slot="<<g.racer<<" c="<<g.collection+1<<" step="<<s.steps
                <<" root="<<root.position.x<<','<<root.position.y<<','<<root.position.z
                <<" v="<<root.velocity.x<<','<<root.velocity.y<<','<<root.velocity.z
                <<" w="<<root.angular.x<<','<<root.angular.y<<','<<root.angular.z<<" attached="<<attached
                <<" max_offset_err="<<offsetError<<" min_rot_dot="<<rotationDot<<" mass="<<g.mass26C
                <<" solver="<<(g.original?"original":"native")<<" pos_root="<<g.positiveRoot<<" pos_members=";
            for(std::size_t i=0;i<g.capacity34;++i)std::cerr<<(i?",":"")<<g.positiveMember[i];
            const auto& c=g.body.impulseC0.v;
            std::cerr<<" c0="<<std::sqrt(c[0]*c[0]+c[1]*c[1]+c[2]*c[2])<<" settle_t="<<g.body.contactTime12C<<" speed34="<<g.body.cachedSpeed34<<" a124="<<g.body.measured124<<" a128="<<g.body.measured128
                <<" dc3c="<<f5::workspace().dc[0].state.contact3C<<'\n';
        }
    }
    for(std::size_t i=0;i<s.pieces.size()&&i<zones::maxPieces;++i)
        if(s.pieces[i].retired&&zones::membership(i))zones::remove(i);              // 120290 at retirement
    if(s.steps%300==0){const auto& st=f5::stats();const auto& zw=zones::walk();
        std::cerr<<"debris_zone_walk step="<<s.steps<<" bound="<<zw.bound<<" attaches="<<zw.attaches<<" attach_refused="<<zw.attachRefused
            <<" updates="<<zw.updates<<" update_refused="<<zw.updateRefused<<" walks="<<zw.walks<<" visits="<<zw.visits
            <<" pass2="<<zw.pass2<<" query_objects="<<zw.queryObjects<<" refused="<<zw.refused<<" standin="<<zw.standinWalks
            <<" sampled_duplicates="<<zw.duplicates<<" sampled_storage_only="<<zw.storageOnly<<" sampled_walk_only="<<zw.walkOnly<<'\n';const auto& mq=f5::moveQuery();
        std::cerr<<"debris_f5_stats step="<<s.steps<<" e0_queries="<<st.queries<<" e0_refused="<<st.refused<<" records="<<st.records
            <<" saturated="<<st.saturated<<" e8_calls="<<mq.calls<<" e8_hits="<<mq.hits<<" e8_refused="<<mq.failures
            <<" water_queries="<<st.waterQueries<<" water_wet="<<st.waterWet<<" water_entries="<<st.waterEntries
            <<" water_refused="<<st.waterRefused<<" trigger_reach="<<st.triggerReach<<" piece_flight="<<s.pieceFlightSteps
            <<" piece_nonflight="<<s.pieceNonflightSteps<<" piece_refusals="<<s.pieceRefusals<<'\n';}
    if(s.rngDraws!=drawsBefore)
        std::cerr<<"debris_rng step="<<s.steps<<" draws="<<s.rngDraws-drawsBefore<<" expected="<<tests+6*breaks<<'\n';
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
// 229F80 -> 228110 at a Results replay start or loop: the racers in mask are
// restored and their wreck leaves the world (the replay re-runs it at the
// recorded moment). Pieces of racers kept wrecked (retired before the current
// recording) stay. Frames never allocate: the removed pieces are retired in
// place (zone memberships released), or the whole sim is cleared when no
// other racer has pieces. Returns the pieces removed.
inline std::size_t removeRacers(std::uint32_t mask) noexcept {
    auto& s=sim();std::size_t removed=0;bool kept=false;
    const auto listed=[&](std::size_t racer){return racer<32u&&((mask>>racer)&1u);};
    for(std::size_t i=0;i<s.pieces.size();++i){
        auto& p=s.pieces[i];
        if(!listed(p.racer)){kept=true;continue;}
        if(i<zones::maxPieces&&zones::membership(i))zones::remove(i);
        if(!p.retired){p.retired=true;++removed;}
    }
    for(std::size_t gi=0;gi<s.groupCount&&gi<s.groups.size();++gi)if(listed(s.groups[gi].racer))s.groups[gi].active=false;
    for(std::size_t r=0;r<s.activeCollections.size();++r)if(listed(r))s.activeCollections[r]=0;
    if(listed(cameraView().racer))resetCameraView();
    if(!kept)resetSim();
    return removed;
}

} // namespace native_wreck_debris50PD
