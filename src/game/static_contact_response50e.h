#pragma once
#include "spring_records48n.h"
#include "spring_model48k.h"

namespace rr::game {
struct StaticResponseScene50E {
    SpringSceneId48N identity;
    BodyMatrix48G world; // Explicit current scene; no scene refresh is inferred.
};
struct StaticResponseWorkspace50E {
    SpringGeneratedRecord48K* projected=nullptr;
    std::size_t projectedCapacity=0;
    SpringGeneratedBuffers48K scratch{},prepared{};
};
struct StaticResponseResult50E {
    SpringRecordState48N records;
    SpringModelBody48K primary;
    Vector3_48C worldCenter110,contactVelocity120;
};
enum class StaticResponseStatus50E { Complete,InvalidInput,UnsupportedScope,InsufficientCapacity };
// Projects original 0x60-byte static generated records to the released 2964C0
// numerical response, in their existing order. No contact creation, defaults,
// provider/point contacts, dynamic bodies, registration or scene update.
// Enabled point contacts and nonstatic records are explicit scope boundaries.
// A nonzero scene40 must match the supplied current scene identity; zero uses
// the primary body's current scene, as in the original generated-record loop.
// Parameters and primary state are explicit runtime inputs, not guessed tuning.
// Complete copies all recordCount slots verbatim except reached flags50, and
// commits result atomically. Extra destination tails remain untouched. Errors
// change workspace only. Full declared writable spans must be mutually disjoint
// and disjoint from inputs/descriptors. No allocation, recursion or guest code.
StaticResponseStatus50E respondStaticGenerated50E(
    const SpringGeneratedParameters48K&,const SpringResponseParameters48L&,
    const SpringRecordState48N&,const SpringModelBody48K&,
    const StaticResponseScene50E&,const SpringRecord48N*,std::size_t recordCount,
    const StaticResponseWorkspace50E&,SpringRecord48N* destination,
    std::size_t destinationCapacity,StaticResponseResult50E&) noexcept;
}
