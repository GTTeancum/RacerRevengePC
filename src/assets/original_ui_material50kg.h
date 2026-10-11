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
    authoredPsm, mainLogoMissingPsmFallback, mainPromptMissingPsmFallback,
    optionsMenuPromptMissingPsmFallback, optionsGamePromptMissingPsmFallback, riderStatMissingPsmFallback, riderPortraitMissingPsmFallback,
    riderNameBlurMissingPsmFallback, riderBackgroundMissingPsmFallback, riderPromptMissingPsmFallback
};
struct OriginalUiMaterialProvenance50KG {
    OriginalUiMaterialOrigin50KG origin = OriginalUiMaterialOrigin50KG::authoredPsm;
    const OriginalUiArchiveIdentity50KG* archive = nullptr;
    // Only the bounded no-PSM branch uses these fields. A real texture member
    // does not become a fabricated PSM identity or authored shader section.
    const OriginalUiMemberIdentity50KG* fallbackTexture = nullptr;
    const OriginalUiMemberIdentity50KG* fallbackWidgetSource = nullptr;
    const OriginalUiMemberIdentity50KG* fallbackPageSource = nullptr;
    std::uint32_t sourceCallerPC = 0;
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

// Shared page constructor1D682C/68DC/6994/6A4C creates these real button
// prompts from literal names through ScreenTexture2DF0B0. Their exact PSTs
// live in authenticated OVERLAYS; absent PSM invokes the SAME2475C0 builder.
// Main entry74 clears the mask; openedB0 sets X only, closingB4 clears it.
// Source caller/page/texture identities remain explicit; no general fallback.
enum class OriginalUiMainPrompt50KG : std::uint8_t { cross,triangle,square,circle };
bool readOriginalUiMainPromptFallbackMaterial50KG(
    const OriginalUiArchive50KG& mainArchive,const OriginalUiArchive50KG& overlays,
    OriginalUiMainPrompt50KG,const OriginalUiMaterialTemplates50KG&,
    OriginalUiTemplate50KG,const OriginalUiMaterialRuntime50KG&,
    OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Same inherited page constructor1D63B0, actually called by OptionsMenu1F8E54.
// Only screen12/page1's live cross/triangle prompts are admitted, with their
// own Options page provenance. No Main archive/identity is fabricated.
enum class OriginalUiOptionsMenuPrompt50KG : std::uint8_t { cross,triangle };
bool readOriginalUiOptionsMenuPromptFallbackMaterial50KG(
    const OriginalUiArchive50KG& options,const OriginalUiArchive50KG& overlays,
    OriginalUiOptionsMenuPrompt50KG,const OriginalUiMaterialTemplates50KG&,
    OriginalUiTemplate50KG,const OriginalUiMaterialRuntime50KG&,
    OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Actual Game Options page4 create1F4724 calls the shared page constructor.
// Its square uses the Options-local PST, with no local/shared PSM. Cross and
// triangle remain the already-authenticated shared menu prompt owners.
bool readOriginalUiGameOptionsPromptFallbackMaterial50KG(
    const OriginalUiArchive50KG& options,const OriginalUiArchive50KG& overlays,
    OriginalUiMainPrompt50KG,const OriginalUiMaterialTemplates50KG&,
    OriginalUiTemplate50KG,const OriginalUiMaterialRuntime50KG&,
    OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Exact Rider1E89D8/1E89F0/1E8A38 sprite constructors, ScreenMT owner.
// Only these original missing-PSM textures are admitted; no arbitrary basename.
enum class OriginalUiRiderStatTexture50KG : std::uint8_t { podstat,podstat2,notch };
bool readOriginalUiRiderStatFallbackMaterial50KG(const OriginalUiArchive50KG& rider,
    OriginalUiRiderStatTexture50KG,const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Actual1E9418..1E9448 %02d_flrg ScreenTexture construction. Admits only
// original1..26 excluding5,6,13; each genuine original portrait is distinct.
bool readOriginalUiRiderPortraitFallbackMaterial50KG(const OriginalUiArchive50KG& rider,
    std::uint8_t riderIndex,const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Rider Icon88 is the authenticated nameblur widget, through2AC6B0.
// Shared page prompts use OVERLAYS cross/triangle and Rider-local square/circle.
// Backgrounds are actual UIBKG %02d_bkg constructors1E8DC0, never portraits.
// Each bounded missing-PSM reader retains distinct source provenance and the
// genuine common ScreenMT constructor. No general fallback entry is exposed.
bool readOriginalUiRiderNameBlurFallbackMaterial50KG(const OriginalUiArchive50KG& rider,
    const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;
bool readOriginalUiRiderBackgroundFallbackMaterial50KG(const OriginalUiArchive50KG& rider,
    const OriginalUiArchive50KG& backgrounds,std::uint8_t riderIndex,
    const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;
bool readOriginalUiRiderPromptFallbackMaterial50KG(const OriginalUiArchive50KG& rider,
    const OriginalUiArchive50KG& overlays,OriginalUiMainPrompt50KG,
    const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;

enum class OriginalUiMaterialBlend50KG : std::uint8_t {
    opaque, sourceAlpha, additive, destinationAlphaAdditive, fixed255Additive,
    destinationSourceAlphaAttenuation
};
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
    // Native blended admission additionally requires the exact joint alpha
    // bound documented below; this cold policy alone has no vertex packets.
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
    // Per-vertex ceiling only. For native fixed-function blending the draw
    // owner must ALSO enforce vertexAlpha * exactAtlasMaximumAlpha <=16384.
    // Full-byte original texture alpha is retained, including values above128.
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
// Authenticated Main-logo/shared-prompt missing-PSM provenance is supported. A zero-map
// descriptor returns untexturedAdapterRequired without inspecting an atlas;
// this texture-taking API must not fabricate a white texture for NoTexture.
// Any authored animation still needs its own original state/frame owner.
bool prepareOriginalUiNativeMaterial50KG(const OriginalUiMaterialPass50KG&,
    const Texture48E& exactAtlas, OriginalUiNativeMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;
// Separate original 3D programs. These do not become ScreenObject passes.
enum class OriginalUiLitProgram50KM : std::uint8_t { diffuse, diffuseCompressed, specularMapCompressed };
struct OriginalUiLitMaterial50KM {
    OriginalUiNativeMaterial50KG raster{};
    OriginalUiLitProgram50KM program = OriginalUiLitProgram50KM::diffuseCompressed;
    bool fogEnabled = false, alphaTestGreaterZero = false;
};
// Actual authored, DefaultMT-associated one-map Diffuse/Compressed only.
// Requires original normal/light/projection producers and a real fog-byte
// stream when enabled. This CPU admission is not those owners or GPU readiness.
// Kind2/IPUM, multiple mips, destination-alpha factors and unknown tests reject.
bool prepareOriginalUiLitMaterial50KM(const OriginalUiMaterialPass50KG&,
    const Texture48E& exactTexture, OriginalUiLitMaterial50KM&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;
class OriginalUiScreenAssets50KG;
enum class OriginalUiNativeIpumImagePolicy50KM : std::uint8_t {
    unavailable, highestResolutionBaseOnly
};
struct OriginalUiRider19IpumMaterial50KM {
    OriginalUiLitMaterial50KM material{};
    OriginalUiNativeIpumImagePolicy50KM imagePolicy=OriginalUiNativeIpumImagePolicy50KM::unavailable;
    const OriginalUiScreenAssets50KG* images=nullptr;
    std::uint32_t imageIndex=0;
    std::uint64_t imageIdentity=0;
    std::uint32_t sourceStoredMipIndex=0,sourceMipCount=0;
};
// Separate exact Rider19c_1 entry. The ordinary Texture48E overload continues
// to reject kind2. This consumes the immutable real image-bank owner, canonical
// source provenance and typed original-data cache color policy. Native policy
// chooses the highest-resolution original level0, honestly publishing only
// that image on GPU while preserving all original mip bytes/provenance on CPU.
// It does NOT assert historical source residencyF4 or emulate memory pressure.
// Requires actual authored19c_1 DiffuseCompressed, MXL0/equal source texel
// filters, opaque RGB and alpha-test OFF, matching the actual original pass.
// Its one-map bind249818..24983C patches material TEXA to TA0/TA1=255,AEM1
// before packet submission24B39C, agreeing with the typed cache alpha policy.
// A downstream alpha consumer still requires the separate sealed source-RGBA
// frame lifetime; the ordinary RGB-only presentation promise is unchanged.
// The images owner must outlive this result and the renderer's publication.
bool prepareOriginalUiRider19IpumMaterial50KM(const OriginalUiMaterialPass50KG&,
    const OriginalUiScreenAssets50KG&,std::uint32_t imageIndex,
    OriginalUiRider19IpumMaterial50KM&,OriginalUiMaterialDiagnostic50KG&) noexcept;

enum class OriginalUiNativeIndexedImagePolicy50KM : std::uint8_t {
    unavailable, highestResolutionBaseOnly
};
struct OriginalUiRider19BodyMaterial50KM {
    OriginalUiLitMaterial50KM material{};
    OriginalUiNativeIndexedImagePolicy50KM imagePolicy=OriginalUiNativeIndexedImagePolicy50KM::unavailable;
    const OriginalUiScreenAssets50KG* images=nullptr;
    std::uint32_t imageIndex=0;
    std::uint64_t imageIdentity=0;
    std::uint32_t sourceStoredMipIndex=0,sourceMipCount=0;
};
// Exact Rider19_1/19_2 first DiffuseCompressed passes and original UIPODTEX
// indexed images only. The ordinary one-mip overload remains unchanged. This
// explicit native policy binds the actual bank's largest original mip, keeps
// all five source mips/palette on CPU, and does not claim historical residentF4.
// MXL0, equal original texel filters, opaque RGB and alpha-test OFF are required.
// Original alpha bytes remain intact. The bodies' later destination-alpha
// specular passes still need a source framebuffer-alpha owner; this admission
// cannot establish the existing presentation-only alpha-disposability promise.
bool prepareOriginalUiRider19BodyMaterial50KM(const OriginalUiMaterialPass50KG&,
    const OriginalUiScreenAssets50KG&,std::uint32_t imageIndex,
    OriginalUiRider19BodyMaterial50KM&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Exact original pod_wndw DiffuseCompressed passes1 and3. Both retain their
// source-alpha blend, alpha test and depth write. The distinct specular pass2
// is not replaced or omitted. Native policy publishes the real64x64 original
// base image while keeping all three indexed mips and palette bytes on CPU.
struct OriginalUiRiderWindowMaterial50KM {
    OriginalUiLitMaterial50KM material{};
    OriginalUiNativeIndexedImagePolicy50KM imagePolicy=OriginalUiNativeIndexedImagePolicy50KM::unavailable;
    const OriginalUiScreenAssets50KG* images=nullptr;
    std::uint32_t imageIndex=0;
    std::uint64_t imageIdentity=0;
    std::uint32_t sourceStoredMipIndex=0,sourceMipCount=0,sourcePass=0;
};
bool prepareOriginalUiRiderWindowMaterial50KM(const OriginalUiMaterialPass50KG&,
    const OriginalUiScreenAssets50KG&,std::uint32_t imageIndex,
    OriginalUiRiderWindowMaterial50KM&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Exact original19_shdw/20_shdw pass1 and each PSM's own authenticated128x128
// indexed image. The legacy19 API name remains; no other shadow is admitted.
// ALPHA=(2,1,0,1) subtracts Cd*As/128 from Cd; it does not blend shadow RGB.
// Preserve source fog, alpha GREATER0, reverse-Z GEQUAL and depth writes.
// The ordinary indexed/lit entry stays closed to this blend. Only the sealed
// source-RGBA renderer admits this result, retaining source-alpha replacement.
struct OriginalUiRider19ShadowMaterial50KM {
    OriginalUiLitMaterial50KM material{};
    const OriginalUiScreenAssets50KG* images=nullptr;
    std::uint32_t imageIndex=0;
    std::uint64_t imageIdentity=0;
};
bool prepareOriginalUiRider19ShadowMaterial50KM(const OriginalUiMaterialPass50KG&,
    const OriginalUiScreenAssets50KG&,std::uint32_t imageIndex,
    OriginalUiRider19ShadowMaterial50KM&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Successful result names the exact published one-map specmap1 image and the
// unchanged source pass. Only original19_1,19_2,20_1,20_2 and pod_wndw pass2
// are admitted under this legacy19 API name. No dormant character pass2 or
// unreferenced20_4 pass is admitted; body20 requires its real fog/ATE/GEQUAL.
// Destination-alpha and FIX255 policies are accepted solely by the separate
// sealed source-RGBA frame route; ordinary RGB-only span conversion rejects.
struct OriginalUiRider19SpecularMaterial50KM {
    OriginalUiLitMaterial50KM material{};
    const OriginalUiScreenAssets50KG* images=nullptr;
    std::uint32_t imageIndex=0;
    std::uint64_t imageIdentity=0;
};
bool prepareOriginalUiRider19SpecularMaterial50KM(const OriginalUiMaterialPass50KG&,
    const OriginalUiScreenAssets50KG&,std::uint32_t imageIndex,
    OriginalUiRider19SpecularMaterial50KM&,OriginalUiMaterialDiagnostic50KG&) noexcept;
}
