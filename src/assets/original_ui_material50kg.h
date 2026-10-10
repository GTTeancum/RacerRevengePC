#pragma once
#include "original_ui_pages50kg.h"
#include "import_texture48e.h"
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace rr::assets {
enum class OriginalUiGsField50KG : std::uint8_t {
    textureColorComponent, textureFunction, sourceAlphaValueWhenAis0,
    sourceAlphaValueWhenAis1, methodOfExpandingTextureAlpha,
    inputColorA, inputColorB, inputAlphaC, inputColorD, fixedAlphaValue,
    wrapModeHorizontal, wrapModeVertical, minClampParameterInUDirection,
    maxClampParameterInUDirection, minClampParameterInVDirection,
    maxClampParameterInVDirection, colorClampingMethod, alphaCorrectionValue,
    alphaBlendingControlInUnitsOfPixels, gouraudShadingOnOff, textureMappingOnOff,
    foggingOnOff, alphaBlendingOnOff, onePassAntiAliasingOnOff,
    textureCoordinatesSTQorUV, environmentContext0or1, fragmentValueControlFixedOrNot,
    alphaTestOnOff, alphaTestMethod, alphaReferenceValue,
    processingMethodWhenFailedInAlphaTest, destinationAlphaTestOnOff,
    destinationAlphaTestMode, depthTestMethod, zWriteDisabled, count
};
constexpr std::size_t originalUiGsFieldCount50KG = static_cast<std::size_t>(OriginalUiGsField50KG::count);
const char* originalUiGsFieldName50KG(OriginalUiGsField50KG) noexcept;
struct OriginalUiMaterialInteger50KG {
    std::int32_t value = 0;
    // Null member denotes an original executable constructor/pass default.
    const OriginalUiMemberIdentity50KG* member = nullptr;
    std::size_t byteOffset = 0;
    std::uint32_t sourcePC = 0;
};
struct OriginalUiGsState50KG {
    std::array<OriginalUiMaterialInteger50KG, originalUiGsFieldCount50KG> fields{};
    const OriginalUiMaterialInteger50KG& operator[](OriginalUiGsField50KG field) const noexcept {
        return fields[static_cast<std::size_t>(field)];
    }
};
enum class OriginalUiTemplate50KG : std::uint8_t { unresolved, defaultMt, screenMt, nullOwnerScreenFallback };
enum class OriginalUiMaterialError50KG : std::uint8_t {
    none, alreadyLoaded, archive, templateOwner, document, section, value,
    passCount, textureCount, textureName, profile, unsupportedField,
    unsupportedValue, texture, allocation, fallbackSource, untexturedAdapterRequired
};
struct OriginalUiMaterialDiagnostic50KG {
    OriginalUiMaterialError50KG error = OriginalUiMaterialError50KG::none;
    std::uint32_t pass = 0, sourcePC = 0;
    std::string field;
    OriginalUiPageDiagnostic50KG document;
};
struct OriginalUiMaterialTemplateState50KG {
    const OriginalUiMemberIdentity50KG* identity = nullptr;
    std::string microprogram;
    OriginalUiGsState50KG gs;
};
// Only authenticated APPSTART can construct the real DefaultMT and ScreenMT
// owners. Both start with original 24A8E0, then apply their own file overrides.
// 3833E8 is DefaultMT; 3833F0 is ScreenMT (not DefaultMT).
class OriginalUiMaterialTemplates50KG final {
public:
    bool load(const OriginalUiArchive50KG& appStart, OriginalUiMaterialDiagnostic50KG&) noexcept;
    bool loaded() const noexcept { return loaded_; }
    const OriginalUiMaterialTemplateState50KG* get(OriginalUiTemplate50KG) const noexcept;
private:
    bool loaded_ = false;
    OriginalUiMaterialTemplateState50KG defaults_, screen_;
};
enum class OriginalUiMaterialOrigin50KG : std::uint8_t {
    authoredPsm, mainLogoMissingPsmFallback
};
struct OriginalUiMaterialProvenance50KG {
    OriginalUiMaterialOrigin50KG origin = OriginalUiMaterialOrigin50KG::authoredPsm;
    const OriginalUiArchiveIdentity50KG* archive = nullptr;
    // Only the bounded no-PSM branch uses these fields. A real texture member
    // does not become a fabricated PSM identity or authored shader section.
    const OriginalUiMemberIdentity50KG* fallbackTexture = nullptr;
    const OriginalUiMemberIdentity50KG* fallbackWidgetSource = nullptr;
    std::string registeredName, attemptedPsmMember;
    std::uint32_t creationPC = 0;
};
struct OriginalUiMaterialPass50KG {
    const OriginalUiMemberIdentity50KG* identity = nullptr;
    const OriginalUiMemberIdentity50KG* inheritedTemplate = nullptr;
    OriginalUiMaterialProvenance50KG provenance;
    std::uint32_t pass = 0;
    std::string microprogram, textureName, shaderPassGroup;
    // Zero-map authored passes retain an empty textureName and never perform
    // the texture-name fallback or map lookup. They need an untextured draw owner.
    std::int32_t enabled = 1, textureCount = 1, textureCoordinateIndex = 0;
    OriginalUiGsState50KG gs;
    // TEX1 values are pass-local source defaults, not inherited template fields.
    // 247F38..247FE8: LCM=0,MXL=0,MMAG=1,MMIN=5,L=0,K=-75.
    std::int32_t lodCalculationMethod = 0, maximumMipLevel = 0;
    std::int32_t magFilter = 1, minFilter = 5, lodL = 0, lodK = -75;
    // Full authored section preserves first-present duplicates and fields
    // owned elsewhere (animation/groups), and allows strict shader admission.
    OriginalUiDatSection50KG authored;
};
struct OriginalUiMaterial50KG {
    const OriginalUiMemberIdentity50KG* identity = nullptr;
    OriginalUiMaterialProvenance50KG provenance;
    std::vector<OriginalUiMaterialPass50KG> passes;
};
struct OriginalUiMaterialRuntime50KG {
    // Pass construction reads original global 435670. A missing owner must
    // not be silently interpreted as normal mode.
    bool overrideKnown = false;
    bool override435670 = false;
};
// Explicit selection represents the actual material registry associated owner
// (14D5E0). Only a source-known null may choose nullOwnerScreenFallback.
// ScreenTexture 2DF10C and ScreenText 2DFAA0 explicitly supply screenMt.
// Bounded zero/one-map numbered-pass path 2476A0. Original 247444..2474BC
// selects this builder for zero and one maps; two-map 2480D0 is rejected.
bool readOriginalUiMaterial50KG(const OriginalUiArchive50KG&, const std::string& member,
    const OriginalUiMaterialTemplates50KG&, OriginalUiTemplate50KG associatedOwner,
    const OriginalUiMaterialRuntime50KG&,
    OriginalUiMaterial50KG&, OriginalUiMaterialDiagnostic50KG&) noexcept;

// Source 29B630..29B6A4 -> vtable3D2110+18 -> 2475C0: the original missing
// PSM branch, narrowly admitted for authenticated Main Icon35/mmlogo only.
// Confirms the original widget reference, actual mmlogo.pst, and absent PSM.
// ScreenTexture explicitly associates ScreenMT (2DF10C -> 299D38), so only
// screenMt is accepted here; unknown/null owners are not silently substituted.
// identity remains null, authored stays empty, and provenance records the
// original assets and constructor PCs. No PSM text is synthesized.
bool readOriginalUiMainLogoFallbackMaterial50KG(const OriginalUiArchive50KG& mainArchive,
    const OriginalUiMaterialTemplates50KG&, OriginalUiTemplate50KG associatedOwner,
    const OriginalUiMaterialRuntime50KG&, OriginalUiMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;

enum class OriginalUiMaterialBlend50KG : std::uint8_t { opaque, sourceAlpha, additive };
enum class OriginalUiMaterialFilter50KG : std::uint8_t { nearest, linear };
enum class OriginalUiMaterialWrap50KG : std::uint8_t { repeat, clampToEdge };
struct OriginalUiNativeMaterial50KG {
    OriginalUiMaterialBlend50KG blend = OriginalUiMaterialBlend50KG::opaque;
    OriginalUiMaterialFilter50KG minification{}, magnification{};
    OriginalUiMaterialWrap50KG u{}, v{};
    // Original TEST/ZBUF state. Platform must bind the proper depth owner:
    // 0=never,1=always,2=GEQUAL,3=GREATER, with original reverse-Z conversion.
    std::uint8_t depthMethod = 0, environmentContext = 0;
    bool depthWrite = false;
    // Raw RGBA8 texture and raw original packet vertex bytes, no recoloring.
    float colorScale = 255.f / 128.f;
    float storedAlphaScale = 255.f / 128.f;
    float blendAlphaScale = (255.f / 128.f) * (255.f / 128.f);
    // With fragment alpha in blend-factor units, source framebuffer alpha
    // requires separate-alpha CONSTANT_ALPHA/ZERO with constant 128/255.
    float alphaStoreFromBlendFactor = 128.f / 255.f;
    bool clampModulatedColorBeforeBlend = true;
    bool requiresSeparateAlphaStore = true;
    // CPU policy is not a certification of the existing color-only UI target.
    // Current draw's joint blendFunc/disabled-depth cannot silently satisfy it.
    bool requiresOriginalDepthOwner = true;
    std::uint8_t maximumVertexAlpha = 128;
};
// Separate type: no texture atlas, MODULATE operation, or sampler exists for
// the admitted zero-map/TME=0 path. Raw vertex channels reach the GS directly.
struct OriginalUiNativeUntexturedMaterial50KG {
    OriginalUiMaterialBlend50KG blend = OriginalUiMaterialBlend50KG::opaque;
    std::uint8_t depthMethod = 0, environmentContext = 0;
    bool depthWrite = false;
    float colorScale = 1.f, storedAlphaScale = 1.f;
    float blendAlphaScale = 255.f / 128.f;
    float alphaStoreFromBlendFactor = 128.f / 255.f;
    bool requiresSeparateAlphaStore = false;
    bool requiresOriginalDepthOwner = true;
    std::uint8_t maximumVertexAlpha = 128;
};
// Only authenticated authored zero-map ScreenObject passes with TME=0 and
// the supported native GS state subset. EnableFillColor/color overrides are
// outside this bounded policy. No atlas is accepted or fabricated. The caller
// must provide the actual untextured native span/depth/alpha-store owner.
bool prepareOriginalUiNativeUntexturedMaterial50KG(const OriginalUiMaterialPass50KG&,
    OriginalUiNativeUntexturedMaterial50KG&, OriginalUiMaterialDiagnostic50KG&) noexcept;
// Strict typed policy for single-level indexed-RGBA ScreenObject MODULATE.
// Consumes the exact selected CPU atlas; rejects tests/fog/DAT/PABE/FBA/etc.
// Returns explicit source depth and alpha requirements, never GPU readiness.
// Authenticated Main-logo missing-PSM provenance is supported. A zero-map
// descriptor returns untexturedAdapterRequired without inspecting an atlas;
// this texture-taking API must not fabricate a white texture for NoTexture.
// Any authored animation still needs its own original state/frame owner.
bool prepareOriginalUiNativeMaterial50KG(const OriginalUiMaterialPass50KG&,
    const Texture48E& exactAtlas, OriginalUiNativeMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;
}
