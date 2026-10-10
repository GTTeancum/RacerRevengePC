#pragma once
#include "original_ui_navigation50kg.h"
#include "original_ui_text_position50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::game {
constexpr std::size_t originalUiTextWidgetLineLimit50KG=64;

enum class OriginalUiTextWidgetStatus50KG : std::uint8_t {
    ok, notBound, alreadyBound, invalidDescriptor, invalidLine, invalidInput,
    unsupportedAnimation, lockedOut, hidden, invalidOperation
};

// The admitted 29DA40 path is indefinite, unblinking and untransformed.
// Nonnegative hold, nonzero blink, scheduled second phase, or a non-unit
// transform returns unsupportedAnimation, rather than silently settling it.
// Negative hold sets +88 but DOES NOT replace the existing +84 hold value.
struct OriginalUiTextLineSetup50KG {
    std::int32_t alpha=255;
    bool lock=false;
    float blinkPeriod=0,holdDuration=-1,scale=1,rotation=0,secondPhaseDelay=-1;
};

// Fixed-address native projection of the source fields used by these leaves.
// Only const access is exposed, so an unsupported animation cannot be injected
// by changing fields behind the admission checks. No guest pointer is stored.
struct OriginalUiTextLineState50KG {
    bool locked=false,overlayEnabled=false,blinkOn=true,indefinite=false;
    float holdRemaining=-1;
    std::uint32_t color=0xffffffff,lastRenderedColor=0xffffffff;
    std::int32_t alphaTarget=255,alphaFrom=255,secondPhaseAlpha=255;
    float alphaCurrent=255,alphaDuration=0,alphaElapsed=0;
    std::int16_t lastRenderedAlpha=255;
    float scaleTarget=1,scaleCurrent=1,lastRenderedScale=1;
    float rotationTarget=0,rotationCurrent=0,lastRenderedRotation=0;
    float secondPhaseDelay=-1,secondPhaseScale=1,secondPhaseRotation=0;
    bool colorDirty=false,alphaDirty=false,scaleDirty=false,rotationDirty=false;
};

struct OriginalUiTextLineProjection50KG {
    const std::string* text=nullptr;
    std::uint32_t sourceColor=0xffffffff;
    std::int16_t sourceAlpha=255;
    std::array<std::uint8_t,4> packetColor{{127,127,127,63}};
    float scale=1,rotation=0;
};

class OriginalUiTextWidget50KG final {
public:
    OriginalUiTextWidget50KG() noexcept=default;
    OriginalUiTextWidget50KG(const OriginalUiTextWidget50KG&)=delete;
    OriginalUiTextWidget50KG& operator=(const OriginalUiTextWidget50KG&)=delete;
    OriginalUiTextWidget50KG(OriginalUiTextWidget50KG&&)=delete;
    OriginalUiTextWidget50KG& operator=(OriginalUiTextWidget50KG&&)=delete;

    // Cold, allocation-free, one-time publication. The descriptor, its strings
    // and authenticated archive identity owner must remain immutable and alive
    // until this widget and all borrowed projection pointers are discarded.
    // Native raw-alpha admission is signed16: every admitted integer is exact
    // in binary32 and source cvt.w.s -> signed16 needs no host rounding policy.
    OriginalUiTextWidgetStatus50KG bind(const assets::OriginalUiOverlayText50KG&) noexcept;
    const assets::OriginalUiOverlayText50KG* descriptor() const noexcept { return descriptor_; }
    std::size_t lineCount() const noexcept { return count_; }
    const OriginalUiTextLineState50KG* line(std::size_t index) const noexcept {
        return index<count_ ? &lines_[index] : nullptr;
    }
    bool selected() const noexcept { return selected_; }
    bool disabled() const noexcept { return disabled_; }
    bool shown() const noexcept { return shown_; }

    // Expand navigation commands through every authored membership in order,
    // including duplicates, then call this once for each matching widget.
    // value matters only for setEnabled. show/hide and reset do not reenable
    // stopped TextOverlays. reset is the inherited 2A79E0 show-only callback.
    OriginalUiTextWidgetStatus50KG apply(OriginalUiControlOperation50KG,
        bool value=false) noexcept;

    // Actual source line entry points. A locked line suppresses setup/color/
    // alpha BEFORE inspecting the requested values. stop is unconditional and
    // clears the lock. Setup does not erase the alpha interpolation setter.
    OriginalUiTextWidgetStatus50KG setupLine(std::size_t,
        const OriginalUiTextLineSetup50KG&) noexcept;
    OriginalUiTextWidgetStatus50KG setLineColor(std::size_t,std::uint32_t) noexcept;
    OriginalUiTextWidgetStatus50KG setLineAlpha(std::size_t,std::int32_t,
        float duration=0) noexcept;
    OriginalUiTextWidgetStatus50KG stopLine(std::size_t) noexcept;

    // Source update's supported zero/nonpositive-duration branch, not a time
    // fast-forward. Call at the ordinary update point. Hidden hierarchy skips
    // update; ancestor visibility is supplied by the owning page. Inactive
    // overlays still receive the source alpha comparison/setter projection.
    OriginalUiTextWidgetStatus50KG updateStatic(bool ancestorsShown=true) noexcept;

    // Models source draw's packet preparation, including last-rendered state.
    // Call only at the source draw point, after normal update if one occurred.
    // It does not perform an update implicitly. Hidden/stopped lines leave
    // output and packet state unchanged. An empty visible line still prepares
    // its source ScreenText state, but naturally emits no glyphs downstream.
    // This is CPU projection, not evidence that a renderer submitted a draw.
    OriginalUiTextWidgetStatus50KG projectLine(std::size_t,
        OriginalUiTextLineProjection50KG&,bool ancestorsShown=true) noexcept;

private:
    static void setupStatic(OriginalUiTextLineState50KG&,
        const OriginalUiTextLineSetup50KG&) noexcept;
    static void setColor(OriginalUiTextLineState50KG&,std::uint32_t) noexcept;
    static void setAlpha(OriginalUiTextLineState50KG&,std::int32_t,float) noexcept;
    static void stop(OriginalUiTextLineState50KG&) noexcept;
    void select() noexcept;
    void deselect() noexcept;
    void enable() noexcept;
    void disable() noexcept;
    void clearSelection() noexcept;
    void applyVisibility(bool show,bool hide) noexcept;

    const assets::OriginalUiOverlayText50KG* descriptor_=nullptr;
    std::array<OriginalUiTextLineState50KG,originalUiTextWidgetLineLimit50KG> lines_{};
    std::size_t count_=0;
    bool selected_=false,disabled_=false,shown_=false;
};
}
