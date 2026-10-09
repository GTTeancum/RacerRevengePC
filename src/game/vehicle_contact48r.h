#pragma once
#include "body_contact_list48r.h"
#include "body_scene48g.h"
#include "spring_records48n.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {
struct VehicleContactScene48R {
    BodyMatrix48G world{},local{};
    std::uint32_t flags=1; // Cache flag190 bit0; other bits are preserved.
    bool parentPresent=false;
};
enum class VehicleContactCallback48R { RigidBody165430,Unsupported };
struct VehicleContactBody48R {
    BodyContactIdentity48R identity{};
    VehicleContactCallback48R callback=VehicleContactCallback48R::RigidBody165430;
    BodyContactListState48R contacts{};
};
struct VehicleContactCandidate48R {
    std::uint32_t identity=0,shapeTag=1;
    BodyMatrix48G shapeWorld{};
    SpringVector4_48N localCenter{},localHalf{},cachedCenter{};
    float cachedRadius=0; // Original candidate+80 is NOT read by219420.
    std::uint32_t category=0,bodyIndex=0;
};
struct VehicleContactStatics48R {
    // Original gp-7894/gp-7890 bytes and434718/434720 raw words. Zero flags
    // lazily initialize the corresponding word to0.5/1 in every prologue.
    // An already initialized second word is copied without interpreting it.
    std::uint8_t flags[2]{};
    std::uint32_t words[2]{};
};
struct VehicleContactInput48R {
    std::int32_t candidateCount=0;
    std::uint32_t positionSceneIndex=0,localSceneIndex=0,primaryBodyIndex=0;
    std::uint32_t categoryMask=0;
    float vehicleRadius=0; // Original vehicle364->80, not candidate.cachedRadius.
    SpringRecordState48N spring{};
    VehicleContactStatics48R statics{};
};
struct VehicleContactSource48R {
    const VehicleContactScene48R* scenes=nullptr;std::size_t sceneCount=0;
    const VehicleContactBody48R* bodies=nullptr;std::size_t bodyCount=0;
    const VehicleContactCandidate48R* candidates=nullptr;std::size_t candidateCount=0;
    const std::uint32_t* candidateOrder=nullptr;std::size_t candidateOrderCount=0;
    const SpringRecord48N* records=nullptr;std::size_t recordCount=0;
};
struct VehicleContactBuffers48R {
    VehicleContactScene48R* scenes=nullptr;std::size_t sceneCapacity=0;
    VehicleContactBody48R* bodies=nullptr;std::size_t bodyCapacity=0;
    SpringRecord48N* records=nullptr;std::size_t recordCapacity=0;
};
struct VehicleContactResult48R {
    SpringRecordState48N spring{};
    VehicleContactStatics48R statics{};
};
enum class VehicleContactStatus48R {
    Complete,UnsupportedBounds,UnsupportedSceneParent,UnsupportedBodyCallback,
    InvalidInput,InsufficientCapacity
};
// Bounded original219420, including original1202E0 tags1/6,28B1A0,28A700,
//106E70,106790,28BD50, known body+98 ->165430 and2963E0. Complete means
// natural handler completion; the original has no semantic boolean return.
// Scene indices express aliasing: use the SAME index for a shared scene. Body
// indices likewise identify shared mutable objects; distinct body entries must
// have distinct identity values. Reached body identities must be nonzero (the
// SpringBodyId48N zero value means no body). Candidate order is supplied, not
// enumerated, sorted or deduplicated. Repeated candidates and self bodies work.
// Signed candidateCount<=0 still executes both static initializers and scene
// position/axes preparation. Other fields are consumed only on reached paths.
// Prepared caches ignore parent presence. Dirty parentless caches copy all16
// local words; dirty parented scenes, other bounds tags and unknown reached
// callbacks report their precise unsupported boundary without invented effects.
//
// Every source array, including inactive tails, is projected in full on Complete;
// excess destination tails remain untouched. All full declared writable spans
// (including scratch, result and inline contact lists) must be mutually disjoint
// and disjoint from every input array and descriptor. Read-only spans may alias.
// Nonzero spans require aligned, representable caller-backed storage. No nested
// pointers occur inside the scene/body/candidate/record elements. Every non-
// Complete status preserves ALL destination arrays and the entire result;
// scratch contents are unspecified on failure. Unsupported statuses do not
// commit an original prefix. Reached append requires full initialized backing
// for spring.generatedCapacity88, even when already saturated.
//
// Strict separately rounded finite binary32 arithmetic, no contraction or
// reassociation; caller rounding mode must be round-to-nearest. Other host or
// physical EE/VU exceptional modes are not certified. Consumed numeric XYZ
// lanes/intermediates must be finite; opaque
// W lanes, inactive fields and preserved record bytes remain raw bit patterns.
// Allocation-free, serial, constant stack; caller-owned variable capacities,
// no guest pointers, containers, retained references or global state.
VehicleContactStatus48R runVehicleContacts48R(const VehicleContactInput48R&,
    const VehicleContactSource48R&,const VehicleContactBuffers48R& scratch,
    const VehicleContactBuffers48R& destination,VehicleContactResult48R&) noexcept;
}
