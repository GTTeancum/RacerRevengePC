#pragma once
#include <array>
#include <cstdint>
#include <memory>
namespace original_main_entry50KG {class NativeCollectorFifo50LA;}
namespace rr::game::original_gc_gameplay_lighting_candidate {class Owner;}
namespace rr::game {
// Numeric semantic owner of zone+A0 (27F120 null;280230 allocation/update).
// This is NOT a scene pointer or packet. Both source scissor contexts use
// the same four endpoints. Keep stable identity while the source updates it.
class OriginalZoneScissor50LA final {
    friend class original_gc_gameplay_lighting_candidate::Owner;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    std::array<std::uint32_t,4> endpoints_{}; // x0,x1,y0,y1, inclusive source fields.
    std::uint32_t zone_=0;
    std::uint64_t revision_=0;
    explicit OriginalZoneScissor50LA(std::uint32_t zone):zone_(zone){}
};
// Only the actual zone traversal may publish this AFTER recursive children
// reach280118. There is intentionally no issuer in the incomplete draw walk.
// A numeric A0 producer or a camera publication alone cannot mint it.
class OriginalZoneScissorPublication50LA final {
    friend class original_gc_gameplay_lighting_candidate::Owner;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    OriginalZoneScissorPublication50LA()=default;
    const original_gc_gameplay_lighting_candidate::Owner* owner_=nullptr;
    std::shared_ptr<OriginalZoneScissor50LA> value_;
    std::uint64_t cameraGeneration_=0;
};
}
