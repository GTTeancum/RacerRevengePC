#pragma once
#include "../../../src/game/vehicle_draw_submit50cs.h"
#include <cstddef>
#include <cstdint>

namespace rr::platform {
// Linux OpenGL 3.2-style entry points, supplied by the context owner. These
// signatures use GL's scalar widths without including or linking a GL loader.
// Every entry is required; functions and hooks must not throw or reenter.
struct VehicleGlFunctions50CV {
    void (*bindBuffer)(std::uint32_t target, std::uint32_t buffer) = nullptr;
    void (*bufferSubData)(std::uint32_t target, std::intptr_t offset,
        std::intptr_t bytes, const void* data) = nullptr;
    void (*bindVertexArray)(std::uint32_t array) = nullptr;
    void (*drawElementsBaseVertex)(std::uint32_t mode, std::int32_t count,
        std::uint32_t type, const void* byteOffset, std::int32_t baseVertex) = nullptr;
    std::uint8_t (*isBuffer)(std::uint32_t buffer) = nullptr;
    std::uint8_t (*isVertexArray)(std::uint32_t array) = nullptr;
    std::uint32_t (*getError)() = nullptr;
};

struct VehicleGlBank50CV {
    // Six distinct VBOs and six distinct VAOs across both banks, created once.
    // Each VAO references its component's VBO: float3 position at 0, float2 UV
    // at 12, normalized unsigned-byte RGBA at 20, stride 24, base offset zero.
    std::uint32_t vertexBuffer[3]{}, vertexArray[3]{}, vertexCapacity[3]{};
};
struct VehicleGlIndices50CV {
    // The exact immutable array used to initialize buffer, retained unchanged
    // for the adapter's lifetime. This pointer must match the 50CM submission.
    const std::uint32_t* data = nullptr;
    std::uint32_t count = 0, buffer = 0;
};
struct VehicleGlResources50CV {
    VehicleGlBank50CV bank[2]{};
    VehicleGlIndices50CV indices[3]{}; // Immutable, shared by both banks.
};

struct VehicleGlHooks50CV {
    void* context = nullptr;
    // Required, read-only: verify the correct live/current GL context FIRST,
    // actual allocation sizes and upload capability, exact retained GPU index
    // contents/provenance, both VAO layouts/buffer associations, and explicitly
    // selected shader/camera/render state and unpublished target readiness.
    // Names/capacity claims alone are not sufficient. Returning true promises
    // that all following GL calls, including error queries, are safe here.
    bool (*ready)(void*, const VehicleGlResources50CV&, std::uint8_t inactiveBank,
        const game::VehicleDrawBatch50CO&, const game::VehicleDrawPlan50CS&) noexcept = nullptr;
    // Required, read-only: resolve this component's ORIGINAL material index.
    // No fallback material/texture is selected by this adapter.
    bool (*materialReady)(void*, game::VehicleDrawComponent50CM,
        std::uint32_t materialIndex) noexcept = nullptr;
    // Must establish an UNPUBLISHED target, explicit camera and device state,
    // and ensure inactive resources are no longer in use before any upload.
    // Safe to abort even if begin returns false. No visible-target drawing.
    bool (*begin)(void*, std::uint8_t inactiveBank) noexcept = nullptr;
    // Bind resolved shader/textures/material uniforms. Preserve the current
    // VAO, EBO, render target and caller's world-space view/projection contract.
    bool (*bindMaterial)(void*, game::VehicleDrawComponent50CM,
        std::uint32_t materialIndex) noexcept = nullptr;
    // The ONLY publication point. Verify all remaining errors/readiness before
    // publishing. False MUST publish nothing and remain abortable; true must
    // require no further fallible step to preserve the transaction contract.
    bool (*commit)(void*, std::uint8_t inactiveBank) noexcept = nullptr;
    // Discard pending work, restore caller state as needed, preserve the last
    // committed image and resources. Must also handle failed begin/commit.
    void (*abort)(void*, std::uint8_t inactiveBank) noexcept = nullptr;
};

// No allocation, resource creation/destruction, index upload, default state,
// shaders, transforms, window integration or presentation policy. Descriptors
// are copied once; GL objects and immutable indices remain caller-owned.
// Invoke the returned callbacks only via submitVehicleDrawBatch50CS. All work
// is synchronous and single-threaded on the caller's current GL context.
class VehicleGlBackend50CV {
public:
    // initialPublishedBank is explicitly selected by the caller (0 or 1).
    // Its opposite is the first writable bank. Recreate the adapter if the
    // resources/context change; do not mutate resources behind its back.
    VehicleGlBackend50CV(const VehicleGlFunctions50CV&, const VehicleGlResources50CV&,
        const VehicleGlHooks50CV&, std::uint8_t initialPublishedBank) noexcept;
    VehicleGlBackend50CV(const VehicleGlBackend50CV&) = delete;
    VehicleGlBackend50CV& operator=(const VehicleGlBackend50CV&) = delete;
    game::VehicleDrawBackend50CS backend() noexcept;
    std::uint8_t publishedBank() const noexcept { return publishedBank_; }
    // First GL error observed in the latest backend preflight/submission; zero
    // does not establish success (a required hook/structural check may fail).
    std::uint32_t glError() const noexcept { return glError_; }
private:
    static bool preflight(void*, const game::VehicleDrawBatch50CO&,
        const game::VehicleDrawPlan50CS&) noexcept;
    static bool begin(void*, const game::VehicleDrawBatch50CO&,
        const game::VehicleDrawPlan50CS&) noexcept;
    static bool upload(void*, game::VehicleDrawComponent50CM,
        const assets::Vertex48B*, std::uint32_t) noexcept;
    static bool bind(void*, game::VehicleDrawComponent50CM,
        const std::uint32_t*, std::uint32_t) noexcept;
    static bool draw(void*, game::VehicleDrawComponent50CM, std::uint32_t,
        const assets::SceneSpan48D&, std::int32_t) noexcept;
    static bool commit(void*) noexcept;
    static void abort(void*) noexcept;
    bool noGlError() noexcept;

    VehicleGlFunctions50CV gl_;
    VehicleGlResources50CV resources_;
    VehicleGlHooks50CV hooks_;
    std::uint32_t vertexCount_[3]{}, spanCount_[3]{};
    std::uint32_t uploaded_ = 0, nextComponent_ = 0, nextSpan_ = 0;
    std::uint32_t boundComponent_ = 3, glError_ = 0;
    std::uint8_t publishedBank_ = 0, pendingBank_ = 1;
    bool active_ = false;
};
}
