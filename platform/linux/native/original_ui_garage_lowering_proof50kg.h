#pragma once
namespace rr::platform {
// Host-only currentness proof for ONE queued Surface's triangle loop. That loop
// is a pure numeric pass over immutable retained source bytes: its lowering and
// raw Surface are fully re-authenticated (current()) immediately before the
// first triangle and again after the last, and nothing between them can mutate
// the model, geometry, archive or packet revision. Per-triangle reads inside the
// bracket therefore reuse the bracket's proof instead of repeating the complete
// model/archive identity check for every triangle. Not source state.
class OriginalUiGarageLoweringProof50KG final {
public:
    static inline thread_local const void* provenLowering=nullptr;
    static inline thread_local const void* provenRaw=nullptr;
    OriginalUiGarageLoweringProof50KG(const void* lowering,const void* raw)noexcept
        :previousLowering_(provenLowering),previousRaw_(provenRaw){provenLowering=lowering;provenRaw=raw;}
    ~OriginalUiGarageLoweringProof50KG(){provenLowering=previousLowering_;provenRaw=previousRaw_;}
    OriginalUiGarageLoweringProof50KG(const OriginalUiGarageLoweringProof50KG&)=delete;
    OriginalUiGarageLoweringProof50KG& operator=(const OriginalUiGarageLoweringProof50KG&)=delete;
private:
    const void* previousLowering_;
    const void* previousRaw_;
};
}
