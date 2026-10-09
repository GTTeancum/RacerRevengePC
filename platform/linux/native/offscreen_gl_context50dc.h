#pragma once
#include <cstdint>
#include <thread>

namespace rr::platform {
// No implicit display/platform, dimensions, version or profile selection.
enum class OffscreenGlPlatform50DC { Unspecified, MesaSurfaceless, DefaultDisplay };
enum class OffscreenGlProfile50DC { Unspecified, Core, Compatibility };
struct OffscreenGlRequest50DC {
    OffscreenGlPlatform50DC platform = OffscreenGlPlatform50DC::Unspecified;
    std::int32_t width = 0, height = 0;
    std::int32_t major = 0, minor = 0; // Minimum desktop GL version, >= 3.2.
    OffscreenGlProfile50DC profile = OffscreenGlProfile50DC::Unspecified;
    std::int32_t redBits = 0, greenBits = 0, blueBits = 0, alphaBits = 0;
    std::int32_t depthBits = 0, stencilBits = 0; // Minimum component sizes.
    // Zero forbids multisampling; positive values request at least this many.
    std::int32_t samples = 0;
};
struct OffscreenGlInfo50DC {
    std::int32_t eglMajor = 0, eglMinor = 0, glMajor = 0, glMinor = 0;
    OffscreenGlProfile50DC profile = OffscreenGlProfile50DC::Unspecified;
    std::int32_t width = 0, height = 0;
    std::int32_t redBits = 0, greenBits = 0, blueBits = 0, alphaBits = 0;
    std::int32_t depthBits = 0, stencilBits = 0, samples = 0;
};
enum class OffscreenGlError50DC {
    None, WrongThread, InvalidRequest, AlreadyOwned, NotCreated, OutOfMemory,
    LibraryUnavailable, MissingEglEntryPoint, ForeignCurrentContext,
    ClientApiChanged, PlatformUnavailable, DisplayFailure, UnsupportedEgl,
    UnsupportedOpenGl, UnsupportedContextRequest, NoMatchingConfig,
    SurfaceFailure, ContextFailure, MissingGlEntryPoint, GlFailure,
    RequirementsNotMet, ReleaseFailure, DestroyFailure, LibraryCloseFailure
};
struct OffscreenGlDiagnostic50DC {
    OffscreenGlError50DC error = OffscreenGlError50DC::None;
    const char* operation = nullptr; // Static string, also identifies missing entry points.
    std::uint32_t eglError = 0, glError = 0; // Zero means not queried, or no GL error.
    // A failed create retains ownership if cleanup itself fails. Call close again
    // after resolving the cause; do not assume a failed create owns nothing.
    OffscreenGlError50DC cleanupError = OffscreenGlError50DC::None;
    const char* cleanupOperation = nullptr;
    std::uint32_t cleanupEglError = 0;
};
using OffscreenGlProc50DC = void (*)();

// Linux-only, owning pbuffer + desktop OpenGL context. No windows, swap/present,
// rendering, default shaders/materials, per-frame allocation or shared contexts.
// Construct/use/destroy on one thread. Noncopyable and nonmovable. The caller
// must dedicate this thread's rendering API and this EGL display's ENTIRE
// initialized lifetime to this owner; EGL display initialization is not a
// private/refcounted resource. Do not overlap another EGL owner on that display,
// or a GLX/other rendering context on this thread. External API switching and
// raw context rebinding while resources are owned are unsupported. Closing an
// acquired owner releases EGL thread state and resets its client API to default.
class OffscreenGlContext50DC {
public:
    OffscreenGlContext50DC() noexcept;
    // close() first when a teardown diagnostic is needed. Destruction on a
    // different thread, or teardown failure, terminates rather than silently
    // leaking ownership/unloading code still referenced by a live context.
    ~OffscreenGlContext50DC() noexcept;
    OffscreenGlContext50DC(const OffscreenGlContext50DC&) = delete;
    OffscreenGlContext50DC& operator=(const OffscreenGlContext50DC&) = delete;
    OffscreenGlContext50DC(OffscreenGlContext50DC&&) = delete;
    OffscreenGlContext50DC& operator=(OffscreenGlContext50DC&&) = delete;

    bool create(const OffscreenGlRequest50DC&, OffscreenGlDiagnostic50DC&) noexcept;
    bool bind(OffscreenGlDiagnostic50DC&) noexcept;
    bool release(OffscreenGlDiagnostic50DC&) noexcept;
    bool close(OffscreenGlDiagnostic50DC&) noexcept;
    bool current() const noexcept;
    bool created() const noexcept;
    bool ownsResources() const noexcept;
    // Exact current-context check first. Cast returned pointers to their actual
    // Linux GL signature; reject nulls. No dlopen/allocation in this operation.
    OffscreenGlProc50DC procAddress(const char* name) const noexcept;
    const OffscreenGlInfo50DC& info() const noexcept { return info_; }
private:
    struct State;
    bool ownerThread() const noexcept;
    bool closeOwned(OffscreenGlDiagnostic50DC&) noexcept;
    const std::thread::id thread_;
    State* state_ = nullptr;
    OffscreenGlInfo50DC info_{};
};
}
