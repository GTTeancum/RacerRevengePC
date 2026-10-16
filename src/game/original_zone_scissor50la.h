#pragma once
#include <array>
#include <cstdint>
#include <memory>
namespace original_main_entry50KG {class NativeCollectorFifo50LA;}
namespace rr::game::original_gc_gameplay_lighting_candidate {class Owner;}
namespace rr::game {
class OriginalApplicationEntities50LA;
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
// The actual Application collector owns this slot; only the retained zone
// traversal writes it AFTER recursive children reach280118. Numeric A0 or
// camera publication alone cannot publish it. This is not draw completion.
class OriginalZoneScissorPublication50LA final {
    friend class original_gc_gameplay_lighting_candidate::Owner;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    explicit OriginalZoneScissorPublication50LA(const OriginalApplicationEntities50LA* application)
        :application_(application){}
    OriginalZoneScissorPublication50LA(const OriginalZoneScissorPublication50LA&)=delete;
    OriginalZoneScissorPublication50LA& operator=(const OriginalZoneScissorPublication50LA&)=delete;
    const OriginalApplicationEntities50LA* application_=nullptr;
    const original_gc_gameplay_lighting_candidate::Owner* owner_=nullptr;
    std::shared_ptr<OriginalZoneScissor50LA> value_;
    std::uint64_t cameraGeneration_=0;
};
}
