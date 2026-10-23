#pragma once
#include <cstdint>
namespace original_main_entry50KG { struct Video; class RaceDisplayLifetime50LA; }
namespace rr::platform { class OriginalLocalizationRendererLease50KG; }
namespace rr::game {
// Constructor-produced storage of the SAME real Video443A60. It is not a
// native presentation counter. The missing25A9C0 mode/frame-call owner must
// establish continuity before a current-session consumer may read frame40.
class OriginalVideoServiceFields50KG final {
    friend struct ::original_main_entry50KG::Video;
    friend class rr::platform::OriginalLocalizationRendererLease50KG;
    OriginalVideoServiceFields50KG()noexcept=default;
    std::uint32_t frame40_=0; // Actual259E9C constructor write.
    // This source suffix is not wired to swaps, native renders or glFinish.
    // It can only join a subsequently recovered actual25A9C0 call owner.
    void increment25AF84()noexcept { frame40_+=std::uint32_t{1}; }
    // No current native owner covers mode25A7B0's TWO frame calls, callbacks
    // at261E80, suspended84/busy exits and the final25AF84 producer. Keeping
    // constructor storage does not produce that missing history. There is no
    // caller-set ready flag or factory for a fabricated continuity token.
    bool currentFrame40(std::uint32_t&)const noexcept { return false; }
public:
    OriginalVideoServiceFields50KG(const OriginalVideoServiceFields50KG&)=delete;
    OriginalVideoServiceFields50KG& operator=(const OriginalVideoServiceFields50KG&)=delete;
};
// Constructor-produced storage of the SAME real renderer442F70. Native GL
// errors are NOT source242EE0 hardware-recovery results and may not write C4.
class OriginalRendererServiceFields50KG final {
    friend class ::original_main_entry50KG::RaceDisplayLifetime50LA;
    friend class rr::platform::OriginalLocalizationRendererLease50KG;
    OriginalRendererServiceFields50KG()noexcept=default;
    std::uint32_t errorC4_=0; // Actual243B94 constructor write.
public:
    OriginalRendererServiceFields50KG(const OriginalRendererServiceFields50KG&)=delete;
    OriginalRendererServiceFields50KG& operator=(const OriginalRendererServiceFields50KG&)=delete;
};
}
