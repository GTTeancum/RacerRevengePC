#pragma once
namespace original_cable_selection50LA { class Binding; }
#include "original_pod_cable_allocation50la.h"
#include <array>
#include <cstdint>

namespace native_racer50HW { class Cables; }
namespace rr::game {

// Persistent scalar projection of the SAME roster PodCables254 owner. The
// native Cables owner embeds this outside racer-bank swaps. It is deliberately
// not a completed173EB0 allocation,224720 callback, or selection capability.
// No detached endpoint/strip arrays are invented from the connected mesh.
class OriginalPodCableSelectionState50LA final {
    friend class ::original_cable_selection50LA::Binding;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
public:
    OriginalPodCableSelectionState50LA(const OriginalPodCableSelectionState50LA&)=delete;
    OriginalPodCableSelectionState50LA& operator=(const OriginalPodCableSelectionState50LA&)=delete;
    OriginalPodCableSelectionState50LA(OriginalPodCableSelectionState50LA&&)=delete;
    OriginalPodCableSelectionState50LA& operator=(OriginalPodCableSelectionState50LA&&)=delete;

    const std::array<float,4>& position40()const noexcept{return position40_;}
    std::uint32_t membership50()const noexcept{return membership50_;}

private:
    friend class ::native_racer50HW::Cables;
    OriginalPodCableSelectionState50LA()=default;
    //173CE4..E8: SQC2 vf0 writes all four lanes; this is not zero4.
    std::array<float,4> position40_{{0.f,0.f,0.f,1.f}};
    //173D1C: null until the existing genuine174360 identity projection writes
    // this exact field. Reset216AC8 clears this SAME location. The current
    // visibility writer does not write position40 or complete174360.
    std::uint32_t membership50_=0;
    // Optional inactive173EB0 prefix, created only through the real Cables
    // resource join. No allocation/setup is implied by ordinary mesh loading.
    std::shared_ptr<OriginalPodCableAllocation50LA> allocation_;
    // No position mutator is exposed: full174360 must be reached only after
    //224720's real endpoint and174370 writes, under its actual graph/scene
    // lifetime. Existing connected-cable generation is not that authorization.
};
}
