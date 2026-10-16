#pragma once
#include "original_ui_pages50kg.h"
#include "import_texture48e.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rr::assets {
class OriginalUiScreenAssets50KG;
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
// 3833E8 is DefaultMT; 3833F0 is ScreenMT (not DefaultMT). Each handle pins
// immutable backing shared with its actual APPSTART archive, so independent
// page/resource handles no longer create separate template object addresses.
// The application must retain the SAME archive through frontend/race restarts.
class OriginalUiMaterialTemplates50KG final {
public:
    // Serial cold call, like archive.load. A second load on this handle fails;
    // a new handle for the same archive borrows its existing template backing.
    bool load(const OriginalUiArchive50KG& appStart, OriginalUiMaterialDiagnostic50KG&) noexcept;
    bool loaded() const noexcept { return bool(storage_); }
    const OriginalUiMaterialTemplateState50KG* get(OriginalUiTemplate50KG) const noexcept;
private:
    std::shared_ptr<const OriginalUiMaterialTemplateStorage50KG> storage_;
};
enum class OriginalUiMaterialOrigin50KG : std::uint8_t {
    authoredPsm, mainLogoMissingPsmFallback, mainPromptMissingPsmFallback,
    optionsMenuPromptMissingPsmFallback, optionsGamePromptMissingPsmFallback, riderStatMissingPsmFallback, riderPortraitMissingPsmFallback,
    riderNameBlurMissingPsmFallback, riderBackgroundMissingPsmFallback, riderPromptMissingPsmFallback,
    courseNameBlurMissingPsmFallback, coursePromptMissingPsmFallback,
    controlsNameBlurMissingPsmFallback, controlsFontMissingPsmFallback, controlsPromptMissingPsmFallback, raceHudFontMissingPsmFallback, tournamentTrophyMissingPsmFallback, tournamentNotchMissingPsmFallback, tournamentPortraitMissingPsmFallback,
    speedBoostTextureMissingPsmFallback, speedBoostTriangleMissingPsmFallback,
    damageTextureMissingPsmFallback, pauseBackdropMissingPsmFallback
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
    // Distinct24A674 alternate-material branch. Missing remains unknown.
    // Only the actual Application startup owner supplies a known value.
    bool callbackOverrideKnown = false;
    std::uint32_t callbackOverride435678 = 0;
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

bool readOriginalUiCourseNameBlurFallbackMaterial50KG(const OriginalUiArchive50KG& course,
    const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;
bool readOriginalUiCoursePromptFallbackMaterial50KG(const OriginalUiArchive50KG& course,
    const OriginalUiArchive50KG& overlays,OriginalUiMainPrompt50KG,
    const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Actual Controls Icon131 / Overlay_Text129 / inherited circle prompt.
// Only these authenticated absent-PSM resources enter the shared2475C0 leaf.
enum class OriginalUiControlsFallback50KG : std::uint8_t {nameBlur,smallFont,circle};
bool readOriginalUiControlsFallbackMaterial50KG(const OriginalUiArchive50KG& options,
    const OriginalUiArchive50KG& fonts,OriginalUiControlsFallback50KG,
    const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;

// Source1A8DD4 ->29CA40 ->2DFD90/2DF8E0, exact in-game16 font.
// The authenticated OVERLAYS member has no PSM; ScreenMT is explicit.
bool readOriginalRaceHudFontFallbackMaterial50KG(const OriginalUiArchive50KG& overlays,
    const OriginalUiMaterialTemplates50KG&,const OriginalUiMaterialRuntime50KG&,
    OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;

enum class OriginalUiMaterialBlend50KG : std::uint8_t {
    opaque, sourceAlpha, additive, destinationAlphaAdditive, fixed255Additive,
    destinationSourceAlphaAttenuation, fixed64Additive
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
// The two authenticated OVERLAYS gleam ScreenTextures require actual stored
// framebuffer alpha. The clear pass is ATST=NEVER/AFAIL=FB_ONLY; the bright
// pass consumes destination alpha. This typed CPU policy does not admit a
// generic ScreenObject exception or certify a framebuffer/depth/group owner.
// clearFramebufferOnly preserves the source failed-test depth-write mask;
// callers must keep the unchanged raster.depthWrite source value separately.
struct OriginalRaceHudGleamMaterial50KG {
    OriginalUiNativeMaterial50KG raster{};
    bool clearFramebufferOnly=false;
};
bool prepareOriginalRaceHudGleamMaterial50KG(const OriginalUiMaterialPass50KG&,
    const OriginalUiScreenAssets50KG&,std::uint32_t imageIndex,
    OriginalRaceHudGleamMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;
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
// Legacy19 API retained for a finite canonical original IPUM set:19c_1,
// 04c_1/2,10c_1/2,12c_1,15c_1/2. Each pass must name its own authenticated
// original PST and per-source typed cache provenance. Actual1..5 source mips
// remain on CPU; native presentation publishes only the largest original mip.
// Missing evidence fails closed; no19 replacement or hardware parity claim.
// First DiffuseCompressed pass only; all actual IPUM passes are opaque and
// alpha-test-off, with source-backed lit raster/sampler validation.
// Actual authored additional passes still require their own admitted owners.
// The images owner must outlive this result and renderer publication.
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
// Legacy19 API retained for original body _1/_2 identities of riders
// 2,4,10,11,12,15,19,20. Each requires its own canonical UIPODTEX PST.
// Retains actual1..5 indexed mips on CPU and publishes the largest original.
// Existing19 dimensions/mip/state contract remains; added first diffuse
// passes must satisfy the common source-backed lit policy, without substitutes.
// Later specular passes need the separate source framebuffer-alpha owner.
bool prepareOriginalUiRider19BodyMaterial50KM(const OriginalUiMaterialPass50KG&,
    const OriginalUiScreenAssets50KG&,std::uint32_t imageIndex,
    OriginalUiRider19BodyMaterial50KM&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Dedicated canonical17_3 pass1 cutout policy. Keeps all four original
// UIPODTEX kind1 mips and raw palette alpha; publishes only source level0
// (last stored128x128). ATE1/ATST6/REF0, opaque RGB, fog and depth writes
// remain source-owned. No generic multilevel or opaque-body exception.
// The immutable images owner must outlive this result and publication.
struct OriginalUiRider17CutoutMaterial50KM {
    OriginalUiLitMaterial50KM material{};
    OriginalUiNativeIndexedImagePolicy50KM imagePolicy=OriginalUiNativeIndexedImagePolicy50KM::unavailable;
    const OriginalUiScreenAssets50KG* images=nullptr;
    std::uint32_t imageIndex=0;
    std::uint64_t imageIdentity=0;
    std::uint32_t sourceStoredMipIndex=0,sourceMipCount=0;
};
bool prepareOriginalUiRider17CutoutMaterial50KM(const OriginalUiMaterialPass50KG&,
    const OriginalUiScreenAssets50KG&,std::uint32_t imageIndex,
    OriginalUiRider17CutoutMaterial50KM&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Exact pod_wndw and12c_4 DiffuseCompressed passes1/3. Both retain their
// source-alpha blend, alpha test and depth write. The distinct specular pass2
// is not replaced or omitted. Native policy publishes the real64x64 original
// base image (12c_4:128x128/four mips), retaining indexed mips/palette on CPU.
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

// Exact original _shdw pass1 for riders2,4,10,11,12,15,19,20, each with
// its own authenticated128x128 indexed image. Legacy19 API name retained.
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

// Authenticated controller body second pass. Native TGA-to1555 RGB5
// base image and sampled alpha are pinned to specmap3 and TA0/TA1/AEM.
struct OriginalUiControlsSpecularMaterial50KG {
    OriginalUiLitMaterial50KM material{};
    const OriginalUiScreenAssets50KG* images=nullptr;
    std::uint32_t imageIndex=0xffffffffu;
    std::uint64_t imageIdentity=0;
};
bool prepareOriginalUiControlsSpecularMaterial50KG(const OriginalUiMaterialPass50KG&,
    const OriginalUiScreenAssets50KG&,std::uint32_t,
    OriginalUiControlsSpecularMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;

// Exact published specmap1 and unchanged original pass2: finite body _1/_2
// identities of riders2,4,10,11,12,15,19,20 plus pod_wndw. Legacy19 API kept.
// Actual12c_4 pass2 additionally requires FIX64 and depth writes; dormant
// character passes and unreferenced20_4 remain excluded. Existing
//19/20/window contracts stay; additions require common validated specular state.
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

// Exact Speed/Boost1895B0 family, from authenticated OVERLAYS absence.
// The last image belongs to the distinct18A7C0->2E1AE0 triangle caller.
// This is immutable material provenance only, not a live canonical row,
// primitive reference, population, renderer capability or active HUD.
enum class OriginalSpeedBoostImage50LA : std::uint8_t {
    panel,segment1,segment2,segment3,segment4,segment5,segment6,segment7,segment8,triangle9
};
bool readOriginalSpeedBoostFallbackMaterial50LA(
    const OriginalUiArchive50KG&,OriginalSpeedBoostImage50LA,
    const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;

// Exact Tournament1EEC40 championship ScreenTextures. No PSM is authored for
// these six names; original2DF0B0/29B638 reaches2475C0 with ScreenMT.
// This is not a generic missing-material fallback.
enum class OriginalUiTournamentTrophy50KG : std::uint8_t {
    goldIcon,silverIcon,bronzeIcon,goldAward,silverAward,bronzeAward
};
bool readOriginalUiTournamentTrophyFallbackMaterial50KG(
    const OriginalUiArchive50KG&,OriginalUiTournamentTrophy50KG,
    const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;

// Exact Tournament notch construction1F1424; same bounded2475C0 ScreenMT
// path, separately identified from Rider notch and Tournament trophies.
bool readOriginalUiTournamentNotchFallbackMaterial50KG(
    const OriginalUiArchive50KG&,const OriginalUiMaterialTemplates50KG&,
    OriginalUiTemplate50KG,const OriginalUiMaterialRuntime50KG&,
    OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;
// Exact Tournament selected-career portrait at1EF034, absent original PSM.
bool readOriginalUiTournamentPortraitFallbackMaterial50KG(
    const OriginalUiArchive50KG&,std::uint8_t career,
    const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;

}

namespace rr::assets {
// Original shared Pause loader1B8810: results1 at1B8840, results2 first at1B898C.
// index0/1 only; second results2 primitive retains the same canonical material.
bool readOriginalPauseBackdropFallbackMaterial50LA(const OriginalUiArchive50KG&,
    std::uint8_t index,const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;
// Exact Damage first loader18C748; per-primitive callers remain on references.
bool readOriginalDamageFallbackMaterial50LA(const OriginalUiArchive50KG&,
    const OriginalUiMaterialTemplates50KG&,OriginalUiTemplate50KG,
    const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,OriginalUiMaterialDiagnostic50KG&) noexcept;
}
