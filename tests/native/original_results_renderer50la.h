#pragma once
#include <cstdint>
#include <memory>
namespace rr::game {class OriginalResults10450LA;struct OriginalResults104Diagnostic50LA;}
namespace original_main_entry50KG {
struct Application;class RaceDisplayLifetime50LA;class NativeCollectorFifo50LA;
// Results-specific ownership of the actual Display/FIFO through source frees.
// No Frame, source mode5, last-camera or draw authority is supplied by a lease.
class ResultsRendererLease50LA final {
    friend struct Application;
    friend class NativeResultsFrame50LA;
    friend class rr::game::OriginalResults10450LA;
    std::shared_ptr<RaceDisplayLifetime50LA> display_;
    std::shared_ptr<NativeCollectorFifo50LA> fifo_;
    const Application* issuer_=nullptr;
    rr::game::OriginalResults10450LA* source_=nullptr;
    std::uint64_t displayGeneration_=0;
    bool pinned_=false;
    unsigned drainPhase_=0;
    ResultsRendererLease50LA(std::shared_ptr<RaceDisplayLifetime50LA>,
        std::shared_ptr<NativeCollectorFifo50LA>,const Application&,rr::game::OriginalResults10450LA&)noexcept;
    bool identityCurrent()const noexcept;
    bool drainResource(std::uint32_t,std::uint32_t,rr::game::OriginalResults104Diagnostic50LA&)noexcept;
    void resourceFreed()noexcept;
    bool releaseAfterResources(rr::game::OriginalResults104Diagnostic50LA&)noexcept;
public:
    ResultsRendererLease50LA(const ResultsRendererLease50LA&)=delete;
    ResultsRendererLease50LA& operator=(const ResultsRendererLease50LA&)=delete;
    ~ResultsRendererLease50LA()noexcept;
};
}
