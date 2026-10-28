#pragma once
#include <cstdint>
#include <memory>
namespace rr::game {class OriginalRaceF850LA;struct OriginalRaceF8Diagnostic50LA;}
namespace original_main_entry50KG {
struct Application;class RaceDisplayLifetime50LA;class NativeCollectorFifo50LA;
// F8-specific native lifetime on the SAME process Display. This is neither a
// source render invocation nor a submission/acknowledgment capability.
// The FIFO shared owner is NOT a FIFO retirement counter. The admitted close
// route retires F8 before the FIFO, and every service rechecks its actual state.
// Future queue producers must join FIFO retire24B720 admission before activation.
class RaceF8RendererLease50LA final {
    friend struct Application;
    friend class rr::game::OriginalRaceF850LA;
    std::shared_ptr<RaceDisplayLifetime50LA> display_;
    std::shared_ptr<NativeCollectorFifo50LA> fifo_;
    const Application* issuer_=nullptr;
    rr::game::OriginalRaceF850LA* source_=nullptr;
    std::uint64_t displayGeneration_=0;
    bool pinned_=false;
    std::uint8_t retirementCursor_=0,drainPhase_=0;
    RaceF8RendererLease50LA(std::shared_ptr<RaceDisplayLifetime50LA>,
        std::shared_ptr<NativeCollectorFifo50LA>,const Application&,rr::game::OriginalRaceF850LA&)noexcept;
    bool identityCurrent()const noexcept;
    // Actual141DD8 renderer20(0) leaf only; no child48 call or completion.
    // Uncalled until the genuine gameplay mode5/index traversal is recovered.
    bool serviceBeforeSlot48(rr::game::OriginalRaceF8Diagnostic50LA&)noexcept;
    bool drainResource(std::uint32_t,std::uint32_t,rr::game::OriginalRaceF8Diagnostic50LA&)noexcept;
    bool closeResources198510(rr::game::OriginalRaceF850LA&,rr::game::OriginalRaceF8Diagnostic50LA&)noexcept;
public:
    RaceF8RendererLease50LA(const RaceF8RendererLease50LA&)=delete;
    RaceF8RendererLease50LA& operator=(const RaceF8RendererLease50LA&)=delete;
    ~RaceF8RendererLease50LA()noexcept;
};
}
