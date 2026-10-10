#pragma once
#include "original_ui_draw50kg.h"
#include "../../../src/game/original_ui_text50kg.h"
#include <array>
#include <cmath>

namespace rr::platform {
// Explicit source widget local-to-canvas transform. Source owners fill every
// coefficient, including source scale/rotation/translation; there is no implicit
// centering, font substitution, color, justification, or target-size heuristic.
struct OriginalUiTransform50KG {
    float xx = 1.f, xy = 0.f, x = 0.f;
    float yx = 0.f, yy = 1.f, y = 0.f;
};
// Cold-owned bounded storage. Keep off the live frame stack and do not copy it.
// CPU/GPU banks remain alive and immutable through submission. The canvas
// also remains alive and unmodified while submitOriginalUi borrows draw().
class OriginalUiCanvas50KG final {
public:
    OriginalUiCanvas50KG() noexcept = default;
    OriginalUiCanvas50KG(const OriginalUiCanvas50KG&) = delete;
    OriginalUiCanvas50KG& operator=(const OriginalUiCanvas50KG&) = delete;
    bool begin(const assets::MaterialTextureAssets50EA& cpu, const MaterialTextureGl50EB& gpu,
        float canvasWidth, float canvasHeight, const std::array<float,4>& clear,
        bool retainRaceImage) noexcept {
        if (!cpu.loaded() || !gpu.initialized() || cpu.textureCount() != gpu.textureCount() ||
            !std::isfinite(canvasWidth) || !std::isfinite(canvasHeight) || canvasWidth <= 0 || canvasHeight <= 0)
            return false;
        for (float value : clear) if (!std::isfinite(value) || value < 0 || value > 1) return false;
        cpu_ = &cpu; gpu_ = &gpu; draw_ = {};
        draw_.textures = &gpu; draw_.texturePublication = gpu.publicationToken();
        draw_.vertices = vertices_.data(); draw_.spans = spans_.data();
        draw_.canvasWidth = canvasWidth; draw_.canvasHeight = canvasHeight;
        draw_.retainRaceImage = retainRaceImage;
        for (unsigned i=0;i<4;++i) draw_.clearRgba[i] = clear[i];
        return true;
    }
    bool appendText(const game::OriginalUiTextLayout50KG& layout, std::uint32_t textureIndex,
        const OriginalUiTransform50KG& transform, const std::array<std::uint8_t,4>& rgba,
        OriginalUiBlend50KG blend, float alphaScale, float colorScale = 1.f) noexcept {
        if (!cpu_ || !gpu_ || layout.count > game::originalUiTextCapacity50KG ||
            !room(layout.count) || !material(textureIndex, blend, alphaScale, colorScale)) return false;
        const auto* original = cpu_->texture(textureIndex);
        // Layout must have used this exact original atlas, not a same-sized
        // replacement. Renderer publication later checks current context/thread.
        if (!original || layout.atlas != &original->texture) return false;
        const float matrix[] = {transform.xx,transform.xy,transform.x,transform.yx,transform.yy,transform.y};
        for (float value : matrix) if (!std::isfinite(value)) return false;
        // Validate every transformed coordinate before publishing any append.
        for (unsigned q=0;q<layout.count;++q) for (const auto& v : layout.quads[q].vertices) {
            const float x = transform.xx*v.x + transform.xy*v.y + transform.x;
            const float y = transform.yx*v.x + transform.yy*v.y + transform.y;
            if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(v.u) || !std::isfinite(v.v)) return false;
        }
        if (!layout.count) return true;
        for (unsigned q=0;q<layout.count;++q) for (unsigned corner=0;corner<4;++corner) {
            const auto& v = layout.quads[q].vertices[corner];
            auto& out = vertices_[(draw_.quadCount+q)*4+corner];
            out.position[0] = transform.xx*v.x + transform.xy*v.y + transform.x;
            out.position[1] = transform.yx*v.x + transform.yy*v.y + transform.y;
            out.position[2] = 0;
            out.uv[0] = v.u; out.uv[1] = v.v;
            for (unsigned c=0;c<4;++c) out.color[c] = rgba[c];
        }
        appendSpan(layout.count,textureIndex,blend,alphaScale,colorScale); return true;
    }
    // Real authored ScreenObject sprite corners, with caller-applied source
    // transform and color. No generic rectangles or generated background art.
    bool appendSprite(const std::array<assets::Vertex48B,4>& corners, std::uint32_t textureIndex,
        OriginalUiBlend50KG blend, float alphaScale, float colorScale = 1.f) noexcept {
        if (!cpu_ || !gpu_ || !room(1) || !material(textureIndex,blend,alphaScale,colorScale)) return false;
        for (const auto& vertex : corners) {
            for (float value : vertex.position) if (!std::isfinite(value)) return false;
            for (float value : vertex.uv) if (!std::isfinite(value)) return false;
            if (vertex.position[2] != 0) return false;
        }
        for (unsigned i=0;i<4;++i) vertices_[draw_.quadCount*4+i]=corners[i];
        appendSpan(1,textureIndex,blend,alphaScale,colorScale); return true;
    }
    const OriginalUiDraw50KG& draw() const noexcept { return draw_; }
private:
    bool room(std::uint32_t count) const noexcept {
        return count <= originalUiMaximumQuads50KG-draw_.quadCount &&
            (!count || draw_.spanCount < originalUiMaximumSpans50KG);
    }
    bool material(std::uint32_t index, OriginalUiBlend50KG blend, float alpha, float color) const noexcept {
        if (!std::isfinite(color) || color<0 || color>8 ||
            gpu_->publicationToken()!=draw_.texturePublication || !std::isfinite(alpha) || alpha<0 || alpha>8 ||
            (blend!=OriginalUiBlend50KG::opaque && blend!=OriginalUiBlend50KG::sourceAlpha && blend!=OriginalUiBlend50KG::additive) ||
            (blend==OriginalUiBlend50KG::opaque && alpha!=1)) return false;
        const auto* cpu=cpu_->texture(index); const auto* gpu=gpu_->asset(index);
        return cpu && gpu && gpu->name && cpu->identity==gpu->identity;
    }
    void appendSpan(std::uint32_t count,std::uint32_t texture,OriginalUiBlend50KG blend,float alpha,float color) noexcept {
        spans_[draw_.spanCount++]={draw_.quadCount,count,texture,blend,alpha,color};
        draw_.quadCount+=count;
    }
    std::array<assets::Vertex48B,originalUiMaximumQuads50KG*4> vertices_{};
    std::array<OriginalUiSpan50KG,originalUiMaximumSpans50KG> spans_{};
    const assets::MaterialTextureAssets50EA* cpu_=nullptr;
    const MaterialTextureGl50EB* gpu_=nullptr;
    OriginalUiDraw50KG draw_{};
};
}
