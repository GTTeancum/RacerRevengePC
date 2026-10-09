#pragma once
#include "offscreen_gl_context50dc.h"
#include "vehicle_draw_gl50cv.h"
#include "vehicle_shader_gl50cz.h"
#include "../../../src/assets/vehicle_draw_assets50cr.h"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace rr::platform {
enum class VehicleOffscreenMode50DD : std::uint8_t {
    unspecified, viewerNeutralUntextured
};
struct VehicleOffscreenRequest50DD {
    OffscreenGlRequest50DC context{};
    // Mandatory explicit opt-in to the existing viewer's constant neutral
    // shader branch. Textured rendering is not implemented or substituted.
    VehicleOffscreenMode50DD mode = VehicleOffscreenMode50DD::unspecified;
};
struct VehicleOffscreenFrame50DD {
    // Required, 16 finite floats in GL column-major storage. The shader uses
    // vec4(worldPosition, 1) * matrix; supply that complete row-vector matrix
    // in column-major storage, including the caller's GL clip-depth conversion.
    // No camera, root/model transform, aspect ratio or transpose is inferred.
    const float* worldViewProjection = nullptr;
    // Required, four finite values in [0,1]. Copied before submission begins.
    const float* clearRgba = nullptr;
};
enum class VehicleOffscreenError50DD : std::uint8_t {
    none, alreadyOwned, request, assets, allocation, context, entryPoint,
    shader, limits, gl, indices, layout, framebuffer, notInitialized,
    notCurrent, frame, submission, noImage, readback, cleanup
};
struct VehicleOffscreenDiagnostic50DD {
    VehicleOffscreenError50DD error = VehicleOffscreenError50DD::none;
    // Static string, never owned or allocated. First failure wins.
    const char* operation = nullptr;
    std::uint32_t component = 3, glError = 0, cleanupGlError = 0, framebufferStatus = 0;
    bool cleanupIncomplete = false;
    OffscreenGlDiagnostic50DC context{};
    OffscreenGlDiagnostic50DC cleanupContext{};
    VehicleGlShaderDiagnostic50CZ shader{};
    game::VehicleDrawSubmitDiagnostic50CS submission{};
};

// Small, exclusive-context Linux diagnostic renderer. Cold creation owns GL
// objects; frames/readback allocate no application heap storage. No window,
// event loop, original-game shaders, texture loader, presentation or FPS claim.
//
// The loaded 50CR owner must outlive this nonmovable renderer and stay unchanged.
// Create/use/close/destroy on one thread. Do not use the renderer's implicitly
// current EGL/GL context externally, change its state, or share its objects.
// The context is intentionally not exposed, preserving allocation/layout/index
// provenance checked once during creation. The renderer does not restore other
// applications' GL state. Call close explicitly to observe cleanup diagnostics.
// Inherits 50DC's dedicated thread/display-lifetime requirement and terminate-on-
// wrong-thread/unresolved-teardown destructor contract. Inputs, output arrays and
// diagnostics must be separate live storage; callbacks must never reenter.
class VehicleOffscreenGl50DD final {
public:
    VehicleOffscreenGl50DD() noexcept;
    ~VehicleOffscreenGl50DD();
    VehicleOffscreenGl50DD(const VehicleOffscreenGl50DD&) = delete;
    VehicleOffscreenGl50DD& operator=(const VehicleOffscreenGl50DD&) = delete;
    VehicleOffscreenGl50DD(VehicleOffscreenGl50DD&&) = delete;
    VehicleOffscreenGl50DD& operator=(VehicleOffscreenGl50DD&&) = delete;

    // Resolution comes only from request.context.width/height. Private targets
    // are single-sample RGBA8/depth24 regardless of pbuffer format preferences.
    // Failure cleans partial initialization; cleanup failure retains ownership
    // for close(), and an already-owned instance never silently recreates.
    bool create(const VehicleOffscreenRequest50DD&, const assets::VehicleDrawAssets50CR&,
        VehicleOffscreenDiagnostic50DD&) noexcept;
    // Accept only a successful unchanged 50CO batch using these exact prepared
    // meshes. Original material indices must belong to each actual 50CR scene,
    // even in explicit neutral mode. Every rejection keeps the prior committed
    // target/bank. Success is command acceptance, not GPU completion: no glFinish
    // is issued. Context/device loss can make an older image unreadable.
    bool submit(const game::VehicleDrawBatch50CO&, const VehicleOffscreenFrame50DD&,
        VehicleOffscreenDiagnostic50DD&) noexcept;
    // Read the most recent successful commit, RGBA unsigned bytes, tightly
    // packed, bottom row first (GL orientation), width*height*4 bytes. Caller
    // supplies live disjoint storage. Failure can partially modify that output
    // but cannot change the committed GPU image. No readback before first commit.
    bool readRgba(std::uint8_t* destination, std::size_t capacity,
        VehicleOffscreenDiagnostic50DD&) noexcept;
    bool close(VehicleOffscreenDiagnostic50DD&) noexcept;
    bool initialized() const noexcept;
    bool hasImage() const noexcept;
    std::size_t rgbaBytes() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
