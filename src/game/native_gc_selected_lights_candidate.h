#pragma once
#include "original_racer_light_selection50km.h"
#include "vehicle_draw50cm.h"
#include <array>
#include <cstdint>

namespace rr::game {
namespace original_gc_gameplay_lighting_candidate { class Owner; }
// Immutable-by-interface copied publication for the EXPLICIT native host-light
// policy, not original generated RGBA or cache parity. Only the real GC owner
// can publish it from a live draw lease. No owner/arena/lease pointer is borrowed.
// A stopped racer's retained completed geometry must retain this SAME snapshot.
class NativeGcSelectedLightsCandidate final {
    friend class original_gc_gameplay_lighting_candidate::Owner;
    std::array<original_racer_lighting50KM::LightMatrices,maxVehicleDrawComponents50CM> matrices_{};
    std::uintptr_t ownerIdentity_=0;
    std::uint64_t graphicsSequence_=0,publication_=0;
    std::uint32_t racerSlot_=0,componentCount_=0;
public:
    NativeGcSelectedLightsCandidate() noexcept=default;
    bool ready() const noexcept {return ownerIdentity_&&graphicsSequence_&&publication_&&validVehicleDrawComponentCount50CM(componentCount_);}
    std::uintptr_t ownerIdentity() const noexcept {return ownerIdentity_;}
    std::uint64_t graphicsSequence() const noexcept {return graphicsSequence_;}
    std::uint64_t publication() const noexcept {return publication_;}
    std::uint32_t racerSlot() const noexcept {return racerSlot_;}
    std::uint32_t componentCount() const noexcept {return componentCount_;}
    const original_racer_lighting50KM::LightMatrices* component(unsigned index) const noexcept {
        return ready()&&index<componentCount_?&matrices_[index]:nullptr;
    }
};
}
