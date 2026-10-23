#pragma once
#include "original_ui_draw50kg.h"
#include <array>
#include "original_ui_direct_video50kg.h"
#include "../../../src/assets/original_ui_material50kg.h"

namespace rr::platform {
class OriginalUiRiderSession50KG;
class OriginalUiGarageCanonicalSubmission50KG;
// NEW reconstruction, not recovered historical bytes. No implicit material
// defaults: caller joins authenticated material, packet-depth and pass owners.
// Each span must first pass prepareOriginalUiNativeMaterial50KG (textured) or
// prepareOriginalUiNativeUntexturedMaterial50KG (zero-map vertex color).
// The caller must preserve its blend/depth policy and any exact sampler,
// resolve pass-group/animation enablement, and exclude every rejected source
// state. This renderer cannot infer those owners from a texture-bank identity.
enum class OriginalUiDepth50KG : std::uint8_t { unspecified, never, always, greaterEqual, greater };
enum class OriginalUiCoverage50KG : std::uint8_t { aa0, fourSampleAa1 };
struct OriginalUiNativeSpan50KG {
    OriginalUiCoverage50KG coverage = OriginalUiCoverage50KG::aa0;
    OriginalUiDepth50KG depth = OriginalUiDepth50KG::unspecified;
    bool depthWrite = false;
    // Exact source policy per textured span, even when images repeat. The renderer
    // verifies actual immutable texture parameters before target writes;
    // conflicts reject instead of inheriting the first image's sampler.
    // Untextured spans must leave every sampler field unspecified. No texture
    // is bound as a surrogate and no irrelevant sampler policy is invented.
    MaterialTextureSampler50EB sampler{};
    // Only explicit source 3D program modes consume these fields. The
    // material owner supplies FGE and the admitted GREATER/AREF0/KEEP test.
    bool sourceFogEnabled = false;
    bool sourceAlphaTestGreaterZero = false;
    // Opt in per actual source packet, not by image or stream presence.
    // Raw339600/366430 STQ keeps its separately rounded Q; W remains the
    // original geometry value. Requires a sealed source-RGBA frame.
    bool sourcePacketStq = false;
};
struct OriginalUiNativeDraw50KG {
    OriginalUiDraw50KG draw{};
    const OriginalUiNativeSpan50KG* states = nullptr;
    std::uint32_t stateCount = 0; // Exactly one per ordered draw span.
    // Ordinary submitOriginalUi requires this proof; the sealed source-RGBA
    // entry instead requires false and owns the real source alpha separately.
    // Must explicitly prove no destination-alpha test/factor or downstream
    // consumer expects source framebuffer alpha. This presentation target is
    // cleared to alpha 1 and RGB-only writes preserve that value. RGBA readback
    // is presentation data, NOT source framebuffer-alpha recovery.
    bool opaquePresentationAlphaOnly = false;
    // Source fog byte /256, parallel to all draw vertices. Required for Main
    // particles or a fog-enabled Diffuse span. Main admits 0..254, Diffuse
    // 0..255. Unused slots may be 0; per-span policy controls consumption.
    // This is a separate native stream, never a fabricated texture channel.
    const float* particleFog = nullptr;
    std::uint32_t particleFogCount = 0;
    float particleFogColor[3] = {0.f,0.f,0.f}; // authored source RGB /255
    // Original pre-divide projected W, parallel to all vertices. Required for
    // Diffuse/Compressed/SpecularMapCompressed spans; consumed only by those modes. Ordinary screen
    // and particle spans retain W=1. Preserves source perspective ST/Q while
    // packet RGB and fog remain screen-linear. No reconstruction from depth.
    const float* sourceRasterW = nullptr;
    std::uint32_t sourceRasterWCount = 0;
    // Raw original packet S,T,Q, parallel to ALL vertex slots when present.
    // Only sourcePacketStq spans consume it. Other slots are neutral {0,0,1}.
    // Never reconstruct Q from W or multiply already divided native UV by Q.
    // The producer copies packet lanes and reorders them with its geometry.
    const std::array<float,3>* sourcePacketStq = nullptr;
    std::uint32_t sourcePacketStqCount = 0;
    // Vertex position[2] is the original integer packet Z in [0,16777215].
    // The vertex shader normalizes it once into native D24 reverse depth.
    // XY stays on the 640x448 source canvas. UI corners are TL,TR,BR,BL;
    // particle corners BL,TL,TR,BR preserve the source strip's BL--TR diagonal.
    // texturedModulate: colorScale=255/128, blended alphaScale=(255/128)^2.
    // untexturedVertexColor: colorScale=1, blended alphaScale=255/128.
    // Opaque spans retain alphaScale=1. The RGB-only route does not store source
    // alpha; the sealed source route independently computes/replaces raw bytes.
    // Both routes retain raw vertex alpha <=128 as a bounded native admission.
    // RGB-only blended spans additionally require each vertex alpha times the
    // real texture's maximum alpha <=16384. Full-byte atlases (including actual
    // Main alpha255) are valid with source packet alpha63/64. This joint bound
    // prevents native blend-factor clamping from hiding a source factor >1.
};
// Owned source material values from the final nonempty canonical drain leaf.
// Inactive FIX and all clamp-region limits survive native enum conversion.
// This value alone is NOT a submission/completion or closed-route certificate.
struct OriginalUiSourceTerminal50KN {
    std::uint32_t canonicalToken=0,pass=0,leaf=0;
    std::uint64_t registryGeneration=0,bankGeneration=0;
    std::array<std::int32_t,assets::originalUiGsFieldCount50KG> gs{};
    // Construction locations; gs itself additionally includes the actual
    // retained binding's texture-alpha mutation, if present (249818..24983C).
    std::array<std::uint32_t,assets::originalUiGsFieldCount50KG> constructionFieldSourcePC{};
    std::array<std::size_t,assets::originalUiGsFieldCount50KG> constructionFieldByteOffset{};
    std::int32_t enabled=0,textureCount=0,textureCoordinateIndex=0;
    std::int32_t lodCalculationMethod=0,maximumMipLevel=0,magFilter=0,minFilter=0,lodL=0,lodK=0;
};
class OriginalUiRiderDrawContribution50KG;
class OriginalUiOptionsDrawContribution50KG;
class OriginalUiHallDrawContribution50KG;
class VehicleWindowGl50DS;
// A closed native ownership interface, not a caller-created certificate. Only
// the concrete full-frame composers can construct it. Each retains its
// own material-history transaction; renderer ownership and GPU retirement are
// identical regardless of the active original UI destination.
class OriginalUiSourceOwner50KM {
public:
    OriginalUiSourceOwner50KM(const OriginalUiSourceOwner50KM&)=delete;
    OriginalUiSourceOwner50KM& operator=(const OriginalUiSourceOwner50KM&)=delete;
protected:
    virtual ~OriginalUiSourceOwner50KM() noexcept=default;
private:
    friend class OriginalUiRiderDrawContribution50KG;
    friend class OriginalUiOptionsDrawContribution50KG;
    friend class OriginalUiMainDraw50KG;
    friend class OriginalUiHallDrawContribution50KG;
    friend class OriginalUiTournamentDrawContribution50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class VehicleWindowGl50DS;
    OriginalUiSourceOwner50KM() noexcept=default;
    virtual bool beginSourceSubmission(std::uint64_t,const VehicleWindowGl50DS&) const noexcept=0;
    virtual void finishSourceSubmission(std::uint64_t,bool sourceAccepted,bool sourceWorkStarted) const noexcept=0;
};
// Sealed by a concrete complete Rider, Options or Hall frame composer, after every real
// widget/model/material pass has joined its source order and combined bank.
// Source target is fresh PSMCT32/context0/mask0,640x448,clear alpha128,reverseD24.
// No retained race image, unhandled group target, later source sampler or other
// source-alpha consumer may survive this frame. The renderer supplies the real
// source allocation/clear and retains its alpha until a subsequent fresh clear;
// opaque display/readback uses a separate presentation surface. The live owner
// must retire through VehicleWindowGl50DS before destination destruction; a
// failed retirement retains that lifetime obligation for an explicit retry.
class OriginalUiSourceFrame50KM final {
public:
    ~OriginalUiSourceFrame50KM() noexcept=default;
private:
    friend class OriginalUiRiderDrawContribution50KG;
    friend class OriginalUiOptionsDrawContribution50KG;
    friend class OriginalUiMainDraw50KG;
    friend class OriginalUiHallDrawContribution50KG;
    friend class OriginalUiTournamentDrawContribution50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class VehicleWindowGl50DS;
    OriginalUiSourceFrame50KM() noexcept=default;
    OriginalUiSourceFrame50KM(const OriginalUiSourceFrame50KM&)=delete;
    OriginalUiSourceFrame50KM& operator=(const OriginalUiSourceFrame50KM&)=delete;
    const OriginalUiNativeDraw50KG* native_=nullptr;
    const OriginalUiSourceOwner50KM* owner_=nullptr;
    // Optional typed canonical pass transaction, never a frame-sealing owner.
    OriginalUiGarageCanonicalSubmission50KG* garageCanonical_=nullptr;
    bool attachGarageCanonical(OriginalUiGarageCanonicalSubmission50KG&)noexcept;
    // Nonzero, increasing for this live composer; written only after a whole
    // frame has assembled successfully. Borrowed arrays and owner remain live.
    std::uint64_t frameGeneration_=0;
    OriginalUiSourceTerminal50KN terminal_{};
    bool directVideoPresent_=false;
    OriginalUiDirectVideo50KG directVideo_{};
    OriginalUiDirectVideoCapability50KG directVideoCapability_{};
    const OriginalUiRiderSession50KG* riderSession_=nullptr;
    const MaterialTextureGl50EB* riderTextures_=nullptr;
    std::uint64_t riderResidencySerial_=0;
};
}
