#pragma once
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Native caller-owned identities, never addresses into an emulated machine.
// Zero denotes no scene override / no other body respectively. This append
// seam transports identities without resolving them or inventing validity rules.
struct SpringSceneId48N { std::uint32_t value; };
struct SpringBodyId48N { std::uint32_t value; };
struct SpringVector4_48N { float v[4]; };
struct SpringRecord48N {
    SpringVector4_48N field00,localPoint10,normal20,surfacePoint30;
    SpringSceneId48N scene40;
    SpringBodyId48N other44;
    float margin48;
    std::uint32_t material4C;
    std::uint8_t flags50;
    std::uint8_t preserved51[15];
};
struct SpringRecordState48N {
    std::uint32_t pending38,contact3C,upward40,other44,point74,generated78;
    std::uint32_t currentCandidate130,pointCount7C;
    std::int32_t generatedCount84,generatedCapacity88;
    std::uint32_t lastMaterialD4;
};
struct SpringRecordAppend48N {
    SpringVector4_48N field00,localPoint10,normal20,surfacePoint30;
    SpringSceneId48N scene40;
    SpringBodyId48N other44;
    std::uint32_t material4C;
};
enum class SpringRecordStatus48N { Appended,Saturated,InvalidInput,InsufficientCapacity };
// Original2963E0: copy four complete vec4 bit patterns and the three named
// scalar fields, increment count84, and update lastMaterialD4. margin48,
// flags50 and preserved51 are NOT initialized. Existing slots and every other
// state field remain unchanged. Repeated calls append in order; no deduplication.
// Saturated is the original signed count84 >= capacity88 no-append result.
// Native safety contract: both signed metadata values must be nonnegative;
// backing storage must cover the FULL logical capacity, including saturated
// calls. A smaller workspace is InsufficientCapacity, never partial success.
// The declared storage range must be representable, suitably aligned, and
// disjoint from input and state; input and state must also be disjoint.
// Null storage is allowed only for zero storageCapacity. Non-finite payloads,
// signed zero and unusual fourth lanes are copied without float arithmetic.
// Every result except Appended leaves input, state and all slots unchanged.
// Fixed stack use, no allocation, no global state, no guest-memory dependency.
SpringRecordStatus48N appendSpringRecord48N(const SpringRecordAppend48N&,
    SpringRecord48N* records,std::size_t storageCapacity,SpringRecordState48N&) noexcept;
// Original296490: set pending38=1; clear generatedCount84/currentCandidate130,
// contact3C/point74/generated78/upward40/other44. Preserve pointCount7C,
// generatedCapacity88,lastMaterialD4, and every record slot (not accessed).
void resetSpringRecords48N(SpringRecordState48N&) noexcept;
}
