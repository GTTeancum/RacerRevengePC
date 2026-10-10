#pragma once
#include "original_racer_material50km.h"
#include "model_palette49s.h"
#include "original_ui_rider_binder50kg.h"
#include "../assets/import_scene48d.h"
#include "../assets/native_asset48b.h"

namespace rr::game {
using OriginalUiLitMatrix50KM = std::array<float,16>;
enum class OriginalUiLitGeometryStatus50KM : std::uint8_t {
    complete, input, unsupportedScene, unsupportedClipping, unsupportedFog, arithmetic
};
// Actual renderer-context1499A0, used by24BD7C. This is a different owner
// from model-specific149BB0/14A0A0 zone selection. Caller supplies current
// manager+40 existence/look/color and current manager+50, not authored colors.
original_racer_lighting50KM::Status projectOriginalUiGlobalLightRows50KM(
    bool hasDirectional40,const std::array<float,3>& look78,
    const std::array<float,4>& color3C,const std::array<float,4>& ambient50,
    original_racer_lighting50KM::LightMatrices&) noexcept;
struct OriginalUiLitContext50KM {
    OriginalUiLitMatrix50KM viewC0{},projection80{},clipProjection290{},screen140{};
    std::array<float,3> cameraPosition180{};
    original_racer_lighting50KM::LightMatrices lights{};
    float fogEnd33Z=0,fogSlope33W=0;
    std::array<float,2> canvasOffset{{1728,1824}};
};
struct OriginalUiLitPreamble50KM {
    OriginalUiLitMatrix50KM screen{},clip{};
    std::array<std::array<float,3>,3> normalAxes{};
    std::array<std::array<float,4>,4> colorRows{};
    std::array<float,3> localCamera{};
    float fogEndTimesSlope=0,fogSlope=0;
    bool compressed=false;
};
// Native semantic StripPreamble with original multiplication grouping. The
// supplied nodeWorld already includes its full parent chain. Source normal
// quantization precedes composition with its basis; never inverse-transpose or
// normalize a replacement world normal. Affine matrices only. Camera290 is an
// explicit required input: it must not be silently replaced by projection80.
OriginalUiLitGeometryStatus50KM prepareOriginalUiLitPreamble50KM(
    const BodyMatrix48G& nodeWorld,const OriginalUiLitContext50KM&,
    bool compressed,float positionQuantization,float normalQuantization,
    OriginalUiLitPreamble50KM&) noexcept;
struct OriginalUiLitVertex50KM {
    assets::Vertex48B vertex{};
    float rasterW=0,fogFactor=0;
};
// Position/normal are original raw quantized values for Compressed, original
// floats for Diffuse. UV is the source program's expanded value. RGB/alpha are
// calculated from the real lighting rows; imported diagnostic colors are unused.
// Only whole-inside vertices are admitted: partial clipping rejects. Plain
// Diffuse radial fog uses native sqrt, explicitly not guest EFU emulation.
// Compressed radial-fog palette branches remain unsupported pending their
// distinct scale/camera-row joins. No-fog branch still emits source F=255.
OriginalUiLitGeometryStatus50KM projectOriginalUiLitVertex50KM(
    const OriginalUiLitPreamble50KM&,const std::array<float,3>& position,
    const std::array<float,3>& normal,const std::array<float,2>& uv,
    bool computeRadialFog,OriginalUiLitVertex50KM&) noexcept;
// Actual SpecularMapCompressed33B8C0 normal-to-UV basis. In its multi-
// palette branch this is model/rootWorld * cameraC0, then normal quantization,
// independent of the selected node's position and diffuse-light palette rows.
// No inverse-transpose, normalization, reflection vector or UV flip is used.
using OriginalUiSpecularNormalAxes50KM = std::array<std::array<float,2>,3>;
OriginalUiLitGeometryStatus50KM prepareOriginalUiSpecularNormalAxes50KM(
    const BodyMatrix48G& sourceModelWorld,const OriginalUiLitMatrix50KM& viewC0,
    float normalQuantization,OriginalUiSpecularNormalAxes50KM&) noexcept;
OriginalUiLitGeometryStatus50KM projectOriginalUiSpecularMapCompressedVertex50KM(
    const OriginalUiLitPreamble50KM&,const OriginalUiSpecularNormalAxes50KM&,
    const std::array<float,3>& rawPosition,const std::array<float,3>& rawNormal,
    OriginalUiLitVertex50KM&) noexcept;
struct OriginalUiLitBinderPacket50KM {
    std::array<assets::Vertex48B,8> vertices{};
    std::array<float,8> rasterW{},fogFactors{};
};
// Exact binder instance23CB20 overrides context rows: identity model matrix,
// identity direction matrix, zero directional colors, current ambient50 in
// row27. Source normal(0,1,0), stream flags7, radial fog ON. Actual source strip
// becomes triangles(0,1,2),(2,1,3), each with one degenerate native slot half.
OriginalUiLitGeometryStatus50KM projectOriginalUiRiderBinderPacket50KM(
    const OriginalUiRiderBinderQuad50KG&,const OriginalUiLitContext50KM&,
    const std::array<float,4>& currentAmbient50,OriginalUiLitBinderPacket50KM&) noexcept;
inline constexpr std::uint32_t originalUiLitMaximumSurfaceVertices50KM=4096;
inline constexpr std::uint32_t originalUiLitMaximumTriangles50KM=8192;
struct OriginalUiLitGeometryWorkspace50KM {
    // Long-lived caller storage; do not create this on a hot frame stack.
    std::array<OriginalUiLitPreamble50KM,maxModelPaletteEntries49S> palette;
    std::array<OriginalUiLitVertex50KM,originalUiLitMaximumSurfaceVertices50KM> vertices;
};
struct OriginalUiLitSurfaceOutput50KM {
    // Distinct caller-owned buffers, capacity in four-vertex slots. Each real
    // source triangle becomes [a,b,c,c], retaining exact source triangle order
    // and making the fixed renderer's second triangle degenerate. No invented
    // quad topology, neutral mesh export or bind-matrix replacement is used.
    assets::Vertex48B* vertices=nullptr;
    float* rasterW=nullptr;
    float* fogFactors=nullptr;
    std::uint32_t slotCapacity=0,slotCount=0,materialIndex=0;
};
// Actual Scene48D semantic surface + current source-index ModelPalette49S.
// Bounded identity palette/local compressed scenes, exact quantized roundtrip,
// same source UV scale in both lanes. Caller chooses real LOD/pass/order and
// supplies the real model fog-computation bit independently of material FGE.
// On failure output bytes/counts are unchanged; caller scratch may change.
OriginalUiLitGeometryStatus50KM projectOriginalUiLitSceneSurface50KM(
    const assets::Scene48D&,std::uint32_t lod,std::uint32_t surface,
    const ModelPalette49S&,const OriginalUiLitContext50KM&,bool computeRadialFog,
    OriginalUiLitSurfaceOutput50KM&,OriginalUiLitGeometryWorkspace50KM&) noexcept;
// Separate original SpecularMapCompressed entry: same real per-node position
// and diffuse-light rows, but source model/root camera-normal UVs. The original
// program emits F=255 even when the model's radial-fog bit is set. This bounded
// entry requires the actual multi-entry identity palette (Rider pod has three).
// One-entry/single-palette UV ownership is deliberately not inferred here.
// Material pass ordering, destination-alpha/fixed255 blending and the native
// framebuffer-alpha owner remain separate obligations; this is geometry only.
OriginalUiLitGeometryStatus50KM projectOriginalUiSpecularMapCompressedSurface50KM(
    const assets::Scene48D&,std::uint32_t lod,std::uint32_t surface,
    const ModelPalette49S&,const OriginalUiLitContext50KM&,
    OriginalUiLitSurfaceOutput50KM&,OriginalUiLitGeometryWorkspace50KM&) noexcept;
}
