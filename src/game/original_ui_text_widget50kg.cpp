#include "original_ui_text_widget50kg.h"
#include <cmath>
#include <limits>

namespace rr::game {
namespace {
bool rawAlphaAdmitted(std::int32_t alpha) noexcept {
    return alpha>=std::numeric_limits<std::int16_t>::min() &&
        alpha<=std::numeric_limits<std::int16_t>::max();
}
bool sourceKindAdmitted(assets::OriginalUiTextSource50KG source) noexcept {
    switch (source) {
    case assets::OriginalUiTextSource50KG::localized:
    case assets::OriginalUiTextSource50KG::authoredLines:
    case assets::OriginalUiTextSource50KG::authoredText:
    case assets::OriginalUiTextSource50KG::dynamic: return true;
    }
    return false;
}
std::int32_t lowAlphaByte(std::int16_t alpha) noexcept {
    // Source 29DBC0 and 29D084 use lbu, even though draw stores signed16.
    return static_cast<std::uint16_t>(alpha)&255u;
}
}

void OriginalUiTextWidget50KG::setupStatic(OriginalUiTextLineState50KG& line,
    const OriginalUiTextLineSetup50KG& setup) noexcept {
    if (line.locked) return; // 29DA40 +CC gate precedes every write.
    line.locked=setup.lock;
    line.overlayEnabled=true;
    line.blinkOn=true;
    line.indefinite=true; // f13<0 leaves +84 unchanged (29DA64..29DA80).
    line.alphaTarget=setup.alpha;
    line.alphaCurrent=static_cast<float>(setup.alpha);
    line.alphaDirty=true;
    line.scaleTarget=setup.scale;
    line.scaleCurrent=setup.scale;
    line.scaleDirty=true;
    line.rotationTarget=setup.rotation;
    line.rotationCurrent=setup.rotation;
    line.rotationDirty=true;
    line.secondPhaseDelay=setup.secondPhaseDelay;
    line.secondPhaseAlpha=setup.alpha;
    line.secondPhaseScale=setup.scale;
    line.secondPhaseRotation=setup.rotation;
    // No writes to alphaDuration/alphaElapsed/alphaFrom: the no-second-phase
    // branch 29DAF0 skips the interpolation-duration stores at 29DACC.
}

void OriginalUiTextWidget50KG::setColor(OriginalUiTextLineState50KG& line,
    std::uint32_t color) noexcept {
    if (line.locked) return; // 29DA10 -> 29DCB0.
    line.color=color;
    line.colorDirty=true;
}

void OriginalUiTextWidget50KG::setAlpha(OriginalUiTextLineState50KG& line,
    std::int32_t alpha,float duration) noexcept {
    if (line.locked) return; // 29DBC0.
    line.alphaDuration=duration;
    line.alphaElapsed=0;
    line.alphaTarget=alpha<0 ? 0 : (alpha>255 ? 255 : alpha);
    line.alphaFrom=lowAlphaByte(line.lastRenderedAlpha);
    // Source setter does not write current +174 or dirty +184 here.
}

void OriginalUiTextWidget50KG::stop(OriginalUiTextLineState50KG& line) noexcept {
    // 29DB10 does not have the +CC gate.
    line.overlayEnabled=false;
    line.indefinite=false;
    line.holdRemaining=-1;
    line.locked=false;
}

OriginalUiTextWidgetStatus50KG OriginalUiTextWidget50KG::bind(
    const assets::OriginalUiOverlayText50KG& descriptor) noexcept {
    if (descriptor_) return OriginalUiTextWidgetStatus50KG::alreadyBound;
    if (!descriptor.identity || descriptor.fontName.empty() ||
        descriptor.materialName.empty() ||
        descriptor.lines.size()>originalUiTextWidgetLineLimit50KG ||
        !rawAlphaAdmitted(descriptor.alpha) ||
        !sourceKindAdmitted(descriptor.textSource) ||
        static_cast<unsigned>(descriptor.justification)>4u)
        return OriginalUiTextWidgetStatus50KG::invalidDescriptor;
    for (const auto& text:descriptor.lines) {
        if (text.size()>255 || text.find('\0')!=std::string::npos)
            return OriginalUiTextWidgetStatus50KG::invalidDescriptor;
    }
    // All validation precedes writes. The immutable owner pointer publishes
    // last, after every line is initialized. No callbacks allocate memory.
    const bool split=descriptor.textSource==assets::OriginalUiTextSource50KG::localized ||
        (descriptor.textSource==assets::OriginalUiTextSource50KG::authoredText &&
         descriptor.usesExplicitHeight);
    const OriginalUiTextLineSetup50KG initial{};
    for (std::size_t i=0;i<descriptor.lines.size();++i) {
        auto& line=lines_[i];
        if (split) {
            // 2A9AD0: authored Alpha/Depth/Justification/Color/Position,
            // then 29DA40(255). Setup overwrites target/current alpha.
            setAlpha(line,descriptor.alpha,0);
            setColor(line,descriptor.color);
            setupStatic(line,initial);
        } else {
            // Direct Text, LineN and dynamic lines: 29DA40(255) first,
            // then the authored alpha setter. Current stays 255 until update.
            setupStatic(line,initial);
            setColor(line,descriptor.color);
            setAlpha(line,descriptor.alpha,0);
        }
    }
    count_=descriptor.lines.size();
    selected_=false;
    disabled_=false;
    shown_=false; // Manager 2A44E0's recursive initial hide.
    descriptor_=&descriptor;
    return OriginalUiTextWidgetStatus50KG::ok;
}

void OriginalUiTextWidget50KG::applyVisibility(bool show,bool hide) noexcept {
    if (show!=hide) shown_=show; // Both true, like both false, do nothing.
}

void OriginalUiTextWidget50KG::select() noexcept {
    selected_=true; // 2AA2A0. Disabled status is intentionally not a guard.
    applyVisibility(descriptor_->enableOnSelect,descriptor_->disableOnSelect);
    if (descriptor_->hasHighlightColor) {
        for (std::size_t i=0;i<count_;++i) setColor(lines_[i],descriptor_->highlightColor);
    }
    OriginalUiTextLineSetup50KG setup;
    setup.alpha=descriptor_->alpha; // Raw authored alpha, not setter-clamped.
    for (std::size_t i=0;i<count_;++i) setupStatic(lines_[i],setup);
}

void OriginalUiTextWidget50KG::deselect() noexcept {
    selected_=false; // 2AA3E0.
    applyVisibility(descriptor_->enableOnDeselect,descriptor_->disableOnDeselect);
    if (descriptor_->hasHighlightColor) {
        for (std::size_t i=0;i<count_;++i) setColor(lines_[i],descriptor_->color);
    } else {
        for (std::size_t i=0;i<count_;++i) stop(lines_[i]);
    }
}

void OriginalUiTextWidget50KG::enable() noexcept {
    disabled_=false; // 2AA4F0 replays the selected/deselected callback.
    if (selected_) select();
    else deselect();
}

void OriginalUiTextWidget50KG::disable() noexcept {
    disabled_=true; // 2AA540 preserves selection and hierarchy visibility.
    if (descriptor_->hasDisabledColor) {
        for (std::size_t i=0;i<count_;++i) setColor(lines_[i],descriptor_->disabledColor);
    } else {
        for (std::size_t i=0;i<count_;++i) stop(lines_[i]);
    }
}

void OriginalUiTextWidget50KG::clearSelection() noexcept {
    selected_=false; // 2AA600 has neither deselect visibility nor stop.
    if (descriptor_->hasHighlightColor) {
        for (std::size_t i=0;i<count_;++i) setColor(lines_[i],descriptor_->color);
    }
}

OriginalUiTextWidgetStatus50KG OriginalUiTextWidget50KG::apply(
    OriginalUiControlOperation50KG operation,bool value) noexcept {
    if (!descriptor_) return OriginalUiTextWidgetStatus50KG::notBound;
    switch (operation) {
    case OriginalUiControlOperation50KG::show:
    case OriginalUiControlOperation50KG::reset: shown_=true; break;
    case OriginalUiControlOperation50KG::hide: shown_=false; break;
    case OriginalUiControlOperation50KG::select: select(); break;
    case OriginalUiControlOperation50KG::deselect: deselect(); break;
    case OriginalUiControlOperation50KG::clearSelection: clearSelection(); break;
    case OriginalUiControlOperation50KG::setEnabled:
        if (value) enable(); else disable();
        break;
    default: return OriginalUiTextWidgetStatus50KG::invalidOperation;
    }
    return OriginalUiTextWidgetStatus50KG::ok;
}

OriginalUiTextWidgetStatus50KG OriginalUiTextWidget50KG::setControlEnabled(bool enabled) noexcept {
    if (!descriptor_) return OriginalUiTextWidgetStatus50KG::notBound;
    selected_=enabled;
    shown_=enabled;
    return OriginalUiTextWidgetStatus50KG::ok;
}

OriginalUiTextWidgetStatus50KG OriginalUiTextWidget50KG::setupLine(std::size_t index,
    const OriginalUiTextLineSetup50KG& setup) noexcept {
    if (!descriptor_) return OriginalUiTextWidgetStatus50KG::notBound;
    if (index>=count_) return OriginalUiTextWidgetStatus50KG::invalidLine;
    if (lines_[index].locked) return OriginalUiTextWidgetStatus50KG::lockedOut;
    if (!rawAlphaAdmitted(setup.alpha) || !std::isfinite(setup.blinkPeriod) ||
        !std::isfinite(setup.holdDuration) || !std::isfinite(setup.scale) ||
        !std::isfinite(setup.rotation) || !std::isfinite(setup.secondPhaseDelay))
        return OriginalUiTextWidgetStatus50KG::invalidInput;
    if (setup.blinkPeriod!=0 || setup.holdDuration>=0 || setup.scale!=1 ||
        setup.rotation!=0 || setup.secondPhaseDelay!=-1)
        return OriginalUiTextWidgetStatus50KG::unsupportedAnimation;
    setupStatic(lines_[index],setup);
    return OriginalUiTextWidgetStatus50KG::ok;
}

OriginalUiTextWidgetStatus50KG OriginalUiTextWidget50KG::setLineColor(
    std::size_t index,std::uint32_t color) noexcept {
    if (!descriptor_) return OriginalUiTextWidgetStatus50KG::notBound;
    if (index>=count_) return OriginalUiTextWidgetStatus50KG::invalidLine;
    if (lines_[index].locked) return OriginalUiTextWidgetStatus50KG::lockedOut;
    setColor(lines_[index],color);
    return OriginalUiTextWidgetStatus50KG::ok;
}

OriginalUiTextWidgetStatus50KG OriginalUiTextWidget50KG::setLineAlpha(
    std::size_t index,std::int32_t alpha,float duration) noexcept {
    if (!descriptor_) return OriginalUiTextWidgetStatus50KG::notBound;
    if (index>=count_) return OriginalUiTextWidgetStatus50KG::invalidLine;
    if (lines_[index].locked) return OriginalUiTextWidgetStatus50KG::lockedOut;
    if (!std::isfinite(duration)) return OriginalUiTextWidgetStatus50KG::invalidInput;
    if (duration>0) return OriginalUiTextWidgetStatus50KG::unsupportedAnimation;
    setAlpha(lines_[index],alpha,duration);
    return OriginalUiTextWidgetStatus50KG::ok;
}

OriginalUiTextWidgetStatus50KG OriginalUiTextWidget50KG::stopLine(std::size_t index) noexcept {
    if (!descriptor_) return OriginalUiTextWidgetStatus50KG::notBound;
    if (index>=count_) return OriginalUiTextWidgetStatus50KG::invalidLine;
    stop(lines_[index]);
    return OriginalUiTextWidgetStatus50KG::ok;
}

OriginalUiTextWidgetStatus50KG OriginalUiTextWidget50KG::updateStatic(
    bool ancestorsShown) noexcept {
    if (!descriptor_) return OriginalUiTextWidgetStatus50KG::notBound;
    if (!shown_ || !ancestorsShown) return OriginalUiTextWidgetStatus50KG::hidden;
    for (std::size_t i=0;i<count_;++i) {
        auto& line=lines_[i];
        line.colorDirty=false;
        line.alphaDirty=false;
        line.scaleDirty=false;
        line.rotationDirty=false;
        // The admitted enabled path has +84=-1,+88=1,+7C=0. Source
        // 29CEFC..29CF68 neither stops nor toggles it, nor clears its lock.
        // The zero-period countdown is unobservable and is not accumulated.
        if (!line.overlayEnabled) line.locked=false; // 29CF64.
        // 29D080 compares the target to ScreenText's LOW BYTE, not the current
        // float. It can intentionally leave a different current float intact.
        // Every admitted duration<=0 takes 29D0A8 ->29DCC0, without advancing
        // alphaElapsed or consulting an invented interpolation clock.
        if (line.alphaTarget!=lowAlphaByte(line.lastRenderedAlpha)) {
            line.alphaCurrent=static_cast<float>(line.alphaTarget);
            line.alphaDirty=true;
        }
        if (line.scaleTarget!=line.lastRenderedScale) {
            line.scaleCurrent=line.scaleTarget;
            line.scaleDirty=true;
        }
        if (line.rotationTarget!=line.lastRenderedRotation) {
            line.rotationCurrent=line.rotationTarget;
            line.rotationDirty=true;
        }
    }
    return OriginalUiTextWidgetStatus50KG::ok;
}

OriginalUiTextWidgetStatus50KG OriginalUiTextWidget50KG::projectLine(std::size_t index,
    OriginalUiTextLineProjection50KG& output,bool ancestorsShown) noexcept {
    if (!descriptor_) return OriginalUiTextWidgetStatus50KG::notBound;
    if (index>=count_) return OriginalUiTextWidgetStatus50KG::invalidLine;
    auto& line=lines_[index];
    if (!shown_ || !ancestorsShown || !line.overlayEnabled || !line.blinkOn)
        return OriginalUiTextWidgetStatus50KG::hidden;
    OriginalUiTextLineProjection50KG projected;
    projected.text=&descriptor_->lines[index];
    projected.sourceColor=line.color;
    // All reachable current values are exact signed16 integers in binary32.
    projected.sourceAlpha=static_cast<std::int16_t>(line.alphaCurrent);
    projected.packetColor=originalUiTextPacketColor50KG(projected.sourceColor,
        projected.sourceAlpha);
    projected.scale=line.scaleCurrent;
    projected.rotation=line.rotationCurrent;
    // 29D464..29D5C0 updates the source ScreenText packet state at draw time.
    line.lastRenderedColor=projected.sourceColor;
    line.lastRenderedAlpha=projected.sourceAlpha;
    line.lastRenderedScale=projected.scale;
    line.lastRenderedRotation=projected.rotation;
    output=projected;
    return OriginalUiTextWidgetStatus50KG::ok;
}
}
