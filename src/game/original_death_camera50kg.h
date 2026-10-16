#pragma once
#include "original_follow_camera.h"
#include "original_death_camera_query.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {
using OriginalDeathVector50KG=OriginalFollowVector;
struct OriginalDeathCameraIdentity50KG {
    std::uint64_t sessionGeneration=0,racerKey=0,mainBodyKey=0,death68Key=0;
    SceneNodeIndex49P body288=SceneNodeIndex49P::none;
};
// This is the actual racer340/body288 binding, never racer194 or a host pose.
// probe118 is the real racer348 field read only for state3 at22DA80.
struct OriginalDeathCameraSubject50KG {
    OriginalDeathCameraIdentity50KG identity{};
    const SceneGraph49P* graph=nullptr;
    std::int32_t racerState40=0;
    std::uint32_t probe118=0;
    OriginalDeathVector50KG bodyVelocity70{0,0,0,1};
};
struct OriginalDeathPiece50KG {
    const std::uint8_t* flags29=nullptr;
    const SceneGraph49P* graph=nullptr;
    SceneNodeIndex49P model14C=SceneNodeIndex49P::none;
};
struct OriginalDeathPieceCollection50KG {
    const OriginalDeathPiece50KG* pieces38=nullptr;
    std::size_t count30=0;
};
// Borrow the genuine racer370/374 owner order, including inactive pieces.
// No flattened substitute positions, camera-generated debris or synthetic count.
struct OriginalDeathCameraDebrisView50KG {
    std::uint64_t sessionGeneration=0,racerKey=0;
    const OriginalDeathPieceCollection50KG* collections374=nullptr;
    std::size_t count370=0;
};
struct OriginalDeathCameraGlobals50KG {
    // Actual3CA0A0 globals and22EFF0/22F160/230224/230300 lazy statics.
    OriginalDeathVector50KG zero434880{0,0,0,1},up4348A0{0,1,0,1};
    bool lazy87FC=false,lazy8800=false,lazy8804=false,lazy8808=false;
    float displacement4348C0=0,height4348C8=0,targetHeight4348D0=0,lockDistance4348D8=0;
};
struct OriginalDeathCamera50KG {
    OriginalDeathCameraIdentity50KG identity{};
    bool prepared=false,active=false;
    std::uint64_t revision=0;
    // Cold/config source writes only. field70 and unrelated preserved fields
    // are initialized native backing; prepare does not claim a retail write.
    float fov0C=80,fov10=0,minFov14=0,distance18=0,distance1C=0;
    float bodyScale20=0,targetHeight24=3;
    bool started50=false,terrain60Present=false;
    float fov64=0,distance68=0,clearance6C=0,field70=0,scale74=1,field78=75,radius7C=100;
    OriginalDeathVector50KG initial80{0,0,0,1},velocity90{0,0,0,1},lookA0{0,0,0,1},positionB0{0,0,0,1};
    OriginalDeathVector50KG previousC0{0,0,0,1},fieldD0{0,0,0,1},targetE0{0,0,0,1};
    OriginalDeathVector50KG fieldF0{0,0,0,1},field100{0,0,0,1},debris110{0,0,0,1};
    OriginalFollowFilter speed128{},fov12C{};
    std::array<OriginalFollowFilter,3> direction130{},debris13C{};
    std::int32_t mode148=2;
    std::uint8_t flags14CLow2=0;
};
enum class OriginalDeathCameraStatus50KG : std::uint8_t {
    complete,owner,graph,arithmetic,query,terrainStub,sourcePath,capacity
};
struct OriginalDeathCameraResult50KG {
    OriginalDeathCameraStatus50KG status=OriginalDeathCameraStatus50KG::complete;
    std::uint32_t sourcePC=0;
    std::size_t debrisAccepted=0,queriesCommitted=0;
    bool prefixCommitted=false,poseCommitted=false;
};
// Genuine174EF4/174F34 cold/config projection. No child registration, query
// construction or activation is claimed. Unwritten native priors are retained.
OriginalDeathCameraResult50KG prepareOriginalDeathCamera50KG(OriginalDeathCamera50KG&,
    const OriginalDeathCameraIdentity50KG&,const OriginalDeathCameraGlobals50KG&) noexcept;
//22D87C/286D20 prefix, before any query creation/attachment. Keeps this prefix
// when a later query fails. Call once for an actual177AB0 mode1 transition.
OriginalDeathCameraResult50KG beginOriginalDeathCamera22D810(OriginalDeathCamera50KG&,
    const OriginalDeathCameraSubject50KG&) noexcept;
// State writes of22D810 AFTER the actual single-query cold/attachment sequence.
// The native query owner must already be attached to the actual racer690 primary
// zone by attachOriginalDeathCameraQuery120. This function performs the final
//120230 refresh before286F90 clears started50;286F90
// ignores both the previous0x60-byte activation packet and the t1 argument.
OriginalDeathCameraResult50KG selectOriginalDeathCamera22D810(OriginalDeathCamera50KG&,
    const OriginalDeathCameraSubject50KG&,OriginalDeathCameraQueryOwner&) noexcept;
// Genuine22DC90 child update for the mode1 entry reached from racer states3/4.
// Includes22D4B0,2301E0,22E260 modes2/3/4,22F560,22E090 and output masks.
// Other death submodes remain explicit sourcePath boundaries. A finite query
// budget is native admission, never a forced successful source loop exit.
// Prefix/query side effects survive failure: terminate, do not replay a frame.
OriginalDeathCameraResult50KG advanceOriginalDeathCamera22DC90(OriginalDeathCamera50KG&,
    const OriginalDeathCameraSubject50KG&,const OriginalDeathCameraDebrisView50KG&,
    float elapsed,OriginalDeathCameraGlobals50KG&,OriginalDeathCameraQueryOwner&,
    TVCameraUpdateOutput49F&,std::size_t queryBudget=128) noexcept;
}
