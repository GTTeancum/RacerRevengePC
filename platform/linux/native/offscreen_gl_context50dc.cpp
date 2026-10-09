#include "offscreen_gl_context50dc.h"
#include <cstring>
#include <dlfcn.h>
#include <exception>
#include <new>

#if !defined(__linux__)
#error "OffscreenGlContext50DC supports the Linux EGL ABI only"
#endif

namespace rr::platform {
namespace {
// Narrow Linux ABI declarations from the public Khronos EGL 1.4/1.5 API.
// No private driver structures or generated GL loader are used.
using Int = std::int32_t;
using UInt = std::uint32_t;
using Handle = void*;
using Proc = OffscreenGlProc50DC;
constexpr Int none = 0x3038, extensions = 0x3055, clientApis = 0x308D;
constexpr UInt openGlApi = 0x30A2;
constexpr Int surfaceType = 0x3033, pbufferBit = 0x0001;
constexpr Int renderableType = 0x3040, conformant = 0x3042, openGlBit = 0x0008;
constexpr Int colorBufferType = 0x303F, rgbBuffer = 0x308E;
constexpr Int redSize = 0x3024, greenSize = 0x3023, blueSize = 0x3022;
constexpr Int alphaSize = 0x3021, depthSize = 0x3025, stencilSize = 0x3026;
constexpr Int sampleBuffers = 0x3032, samples = 0x3031;
constexpr Int width = 0x3057, height = 0x3056, largestPbuffer = 0x3058;
constexpr Int contextMajor = 0x3098, contextMinor = 0x30FB;
constexpr Int contextProfile = 0x30FD, contextClientType = 0x3097;
constexpr Int coreProfile = 1, compatibilityProfile = 2;
constexpr Int drawSurface = 0x3059, readSurface = 0x305A;
constexpr UInt surfacelessMesa = 0x31DD;
constexpr UInt glMajorVersion = 0x821B, glMinorVersion = 0x821C, glProfileMask = 0x9126;
static_assert(sizeof(int) == sizeof(Int) && sizeof(unsigned int) == sizeof(UInt),
    "EGL/GL Linux scalar ABI");

bool fail(OffscreenGlDiagnostic50DC& d, OffscreenGlError50DC error,
    const char* operation, UInt eglError = 0, UInt glError = 0) noexcept {
    d.error = error; d.operation = operation; d.eglError = eglError; d.glError = glError;
    return false;
}
bool token(const char* list, const char* wanted) noexcept {
    if (!list) return false;
    const auto n = std::strlen(wanted);
    for (const char* p = list; (p = std::strstr(p, wanted)) != nullptr; p += n)
        if ((p == list || p[-1] == ' ') && (p[n] == '\0' || p[n] == ' ')) return true;
    return false;
}
bool valid(const OffscreenGlRequest50DC& r) noexcept {
    return (r.platform == OffscreenGlPlatform50DC::MesaSurfaceless ||
        r.platform == OffscreenGlPlatform50DC::DefaultDisplay) &&
        (r.profile == OffscreenGlProfile50DC::Core ||
         r.profile == OffscreenGlProfile50DC::Compatibility) &&
        r.width > 0 && r.height > 0 && r.major >= 3 && r.minor >= 0 &&
        (r.major > 3 || r.minor >= 2) && r.redBits >= 0 && r.greenBits >= 0 &&
        r.blueBits >= 0 && r.alphaBits >= 0 && r.depthBits >= 0 &&
        r.stencilBits >= 0 && r.samples >= 0;
}
}

struct OffscreenGlContext50DC::State {
    Handle eglLibrary = nullptr, glLibrary = nullptr;
    Handle display = nullptr, surface = nullptr, context = nullptr;
    bool initialized = false, apiBound = false, threadStateOwned = false, ready = false;
    UInt priorApi = 0;
    Int (*getError)() = nullptr;
    Handle (*getDisplay)(void*) = nullptr;
    UInt (*initialize)(Handle, Int*, Int*) = nullptr;
    UInt (*terminate)(Handle) = nullptr;
    const char* (*queryString)(Handle, Int) = nullptr;
    UInt (*bindApi)(UInt) = nullptr;
    UInt (*queryApi)() = nullptr;
    UInt (*releaseThread)() = nullptr;
    UInt (*chooseConfig)(Handle, const Int*, Handle*, Int, Int*) = nullptr;
    UInt (*getConfigAttrib)(Handle, Handle, Int, Int*) = nullptr;
    Handle (*createPbufferSurface)(Handle, Handle, const Int*) = nullptr;
    UInt (*querySurface)(Handle, Handle, Int, Int*) = nullptr;
    UInt (*destroySurface)(Handle, Handle) = nullptr;
    Handle (*createContext)(Handle, Handle, Handle, const Int*) = nullptr;
    UInt (*queryContext)(Handle, Handle, Int, Int*) = nullptr;
    UInt (*destroyContext)(Handle, Handle) = nullptr;
    UInt (*makeCurrent)(Handle, Handle, Handle, Handle) = nullptr;
    Handle (*getCurrentContext)() = nullptr;
    Handle (*getCurrentDisplay)() = nullptr;
    Handle (*getCurrentSurface)(Int) = nullptr;
    Proc (*getProcAddress)(const char*) = nullptr;

    bool load(OffscreenGlDiagnostic50DC& d) noexcept {
        eglLibrary = dlopen("libEGL.so.1", RTLD_NOW | RTLD_LOCAL);
        if (!eglLibrary) return fail(d, OffscreenGlError50DC::LibraryUnavailable,
            "dlopen(libEGL.so.1)");
#define RR_EGL50DC(member, symbol) \
        member = reinterpret_cast<decltype(member)>(dlsym(eglLibrary, symbol)); \
        if (!member) return fail(d, OffscreenGlError50DC::MissingEglEntryPoint, symbol)
        RR_EGL50DC(getError, "eglGetError");
        RR_EGL50DC(getDisplay, "eglGetDisplay");
        RR_EGL50DC(initialize, "eglInitialize");
        RR_EGL50DC(terminate, "eglTerminate");
        RR_EGL50DC(queryString, "eglQueryString");
        RR_EGL50DC(bindApi, "eglBindAPI");
        RR_EGL50DC(queryApi, "eglQueryAPI");
        RR_EGL50DC(releaseThread, "eglReleaseThread");
        RR_EGL50DC(chooseConfig, "eglChooseConfig");
        RR_EGL50DC(getConfigAttrib, "eglGetConfigAttrib");
        RR_EGL50DC(createPbufferSurface, "eglCreatePbufferSurface");
        RR_EGL50DC(querySurface, "eglQuerySurface");
        RR_EGL50DC(destroySurface, "eglDestroySurface");
        RR_EGL50DC(createContext, "eglCreateContext");
        RR_EGL50DC(queryContext, "eglQueryContext");
        RR_EGL50DC(destroyContext, "eglDestroyContext");
        RR_EGL50DC(makeCurrent, "eglMakeCurrent");
        RR_EGL50DC(getCurrentContext, "eglGetCurrentContext");
        RR_EGL50DC(getCurrentDisplay, "eglGetCurrentDisplay");
        RR_EGL50DC(getCurrentSurface, "eglGetCurrentSurface");
        RR_EGL50DC(getProcAddress, "eglGetProcAddress");
#undef RR_EGL50DC
        return true;
    }
    Proc lookup(const char* name) const noexcept {
        auto p = getProcAddress(name);
        // EGL 1.4 need not expose core GL symbols through eglGetProcAddress.
        if (!p && glLibrary) p = reinterpret_cast<Proc>(dlsym(glLibrary, name));
        return p;
    }
};

OffscreenGlContext50DC::OffscreenGlContext50DC() noexcept
    : thread_(std::this_thread::get_id()) {}
OffscreenGlContext50DC::~OffscreenGlContext50DC() noexcept {
    OffscreenGlDiagnostic50DC d;
    if (!ownerThread() || !closeOwned(d)) std::terminate();
}
bool OffscreenGlContext50DC::ownerThread() const noexcept {
    return thread_ == std::this_thread::get_id();
}
bool OffscreenGlContext50DC::created() const noexcept {
    return ownerThread() && state_ && state_->ready;
}
bool OffscreenGlContext50DC::ownsResources() const noexcept {
    return ownerThread() && state_;
}
bool OffscreenGlContext50DC::current() const noexcept {
    if (!created()) return false;
    const auto& s = *state_;
    return s.queryApi() == openGlApi && s.getCurrentContext() == s.context &&
        s.getCurrentDisplay() == s.display && s.getCurrentSurface(drawSurface) == s.surface &&
        s.getCurrentSurface(readSurface) == s.surface;
}
Proc OffscreenGlContext50DC::procAddress(const char* name) const noexcept {
    if (!current() || !name || name[0] != 'g' || name[1] != 'l' || !name[2]) return nullptr;
    return state_->lookup(name);
}

bool OffscreenGlContext50DC::create(const OffscreenGlRequest50DC& r,
    OffscreenGlDiagnostic50DC& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, OffscreenGlError50DC::WrongThread, "create");
    if (state_) return fail(d, OffscreenGlError50DC::AlreadyOwned, "create");
    if (!valid(r)) return fail(d, OffscreenGlError50DC::InvalidRequest, "create");
    state_ = new (std::nothrow) State;
    if (!state_) return fail(d, OffscreenGlError50DC::OutOfMemory, "create");
    auto& s = *state_;
    // Every failure funnels through the same ownership-aware partial teardown.
    const auto cleanup = [&]() noexcept {
        OffscreenGlDiagnostic50DC c;
        if (!closeOwned(c)) {
            d.cleanupError = c.error; d.cleanupOperation = c.operation;
            d.cleanupEglError = c.eglError;
        }
        return false;
    };
    const auto eglFail = [&](OffscreenGlError50DC code, const char* operation) noexcept {
        fail(d, code, operation, static_cast<UInt>(s.getError()));
        return cleanup();
    };
    if (!s.load(d)) return cleanup();
    if (s.getCurrentContext()) {
        fail(d, OffscreenGlError50DC::ForeignCurrentContext, "create/preexisting context");
        return cleanup();
    }
    s.priorApi = s.queryApi();
    if (!s.bindApi(openGlApi)) return eglFail(OffscreenGlError50DC::UnsupportedOpenGl, "eglBindAPI");
    s.apiBound = true;
    // eglGetCurrentContext is selected by the currently bound client API.
    if (s.getCurrentContext()) {
        fail(d, OffscreenGlError50DC::ForeignCurrentContext, "create/preexisting OpenGL context");
        return cleanup();
    }
    s.threadStateOwned = true;
    if (r.platform == OffscreenGlPlatform50DC::MesaSurfaceless) {
        const char* clientExtensions = s.queryString(nullptr, extensions);
        if (!token(clientExtensions, "EGL_MESA_platform_surfaceless"))
            return eglFail(OffscreenGlError50DC::PlatformUnavailable, "EGL_MESA_platform_surfaceless");
        using GetPlatform = Handle (*)(UInt, void*, const std::intptr_t*);
        using GetPlatformExt = Handle (*)(UInt, void*, const Int*);
        auto getPlatform = reinterpret_cast<GetPlatform>(dlsym(s.eglLibrary, "eglGetPlatformDisplay"));
        if (getPlatform) s.display = getPlatform(surfacelessMesa, nullptr, nullptr);
        else if (token(clientExtensions, "EGL_EXT_platform_base")) {
            auto getPlatformExt = reinterpret_cast<GetPlatformExt>(s.getProcAddress("eglGetPlatformDisplayEXT"));
            if (getPlatformExt) s.display = getPlatformExt(surfacelessMesa, nullptr, nullptr);
        }
        if (!s.display) return eglFail(OffscreenGlError50DC::PlatformUnavailable, "eglGetPlatformDisplay(surfaceless)");
    } else s.display = s.getDisplay(nullptr);
    if (!s.display) return eglFail(OffscreenGlError50DC::DisplayFailure, "eglGetDisplay");
    if (!s.initialize(s.display, &info_.eglMajor, &info_.eglMinor))
        return eglFail(OffscreenGlError50DC::DisplayFailure, "eglInitialize");
    s.initialized = true;
    if (info_.eglMajor < 1 || (info_.eglMajor == 1 && info_.eglMinor < 4)) {
        fail(d, OffscreenGlError50DC::UnsupportedEgl, "EGL 1.4 minimum");
        return cleanup();
    }
    if (!token(s.queryString(s.display, clientApis), "OpenGL"))
        return eglFail(OffscreenGlError50DC::UnsupportedOpenGl, "EGL_CLIENT_APIS/OpenGL");
    const bool egl15 = info_.eglMajor > 1 || info_.eglMinor >= 5;
    if (!egl15 && !token(s.queryString(s.display, extensions), "EGL_KHR_create_context"))
        return eglFail(OffscreenGlError50DC::UnsupportedContextRequest, "EGL_KHR_create_context");
    const Int attributes[] = {
        surfaceType, pbufferBit, renderableType, openGlBit, conformant, openGlBit,
        colorBufferType, rgbBuffer, redSize, r.redBits, greenSize, r.greenBits,
        blueSize, r.blueBits, alphaSize, r.alphaBits, depthSize, r.depthBits,
        stencilSize, r.stencilBits, sampleBuffers, r.samples ? 1 : 0, samples, r.samples, none
    };
    Handle config = nullptr;
    Int count = 0;
    if (!s.chooseConfig(s.display, attributes, &config, 1, &count))
        return eglFail(OffscreenGlError50DC::NoMatchingConfig, "eglChooseConfig");
    if (!count || !config) {
        fail(d, OffscreenGlError50DC::NoMatchingConfig, "eglChooseConfig/no match");
        return cleanup();
    }
    Int buffers = 0;
    const Int keys[] = { redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize, samples, sampleBuffers };
    Int* values[] = { &info_.redBits, &info_.greenBits, &info_.blueBits, &info_.alphaBits,
        &info_.depthBits, &info_.stencilBits, &info_.samples, &buffers };
    const Int minima[] = { r.redBits, r.greenBits, r.blueBits, r.alphaBits,
        r.depthBits, r.stencilBits, r.samples, r.samples ? 1 : 0 };
    for (unsigned i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        if (!s.getConfigAttrib(s.display, config, keys[i], values[i]))
            return eglFail(OffscreenGlError50DC::NoMatchingConfig, "eglGetConfigAttrib");
        if (*values[i] < minima[i]) {
            fail(d, OffscreenGlError50DC::RequirementsNotMet, "framebuffer minimum sizes");
            return cleanup();
        }
    }
    if (!r.samples && (buffers != 0 || info_.samples != 0)) {
        fail(d, OffscreenGlError50DC::RequirementsNotMet, "non-multisampled framebuffer");
        return cleanup();
    }
    const Int surfaceAttributes[] = { width, r.width, height, r.height, largestPbuffer, 0, none };
    s.surface = s.createPbufferSurface(s.display, config, surfaceAttributes);
    if (!s.surface) return eglFail(OffscreenGlError50DC::SurfaceFailure, "eglCreatePbufferSurface");
    if (!s.querySurface(s.display, s.surface, width, &info_.width) ||
        !s.querySurface(s.display, s.surface, height, &info_.height))
        return eglFail(OffscreenGlError50DC::SurfaceFailure, "eglQuerySurface");
    if (info_.width != r.width || info_.height != r.height) {
        fail(d, OffscreenGlError50DC::RequirementsNotMet, "exact pbuffer dimensions");
        return cleanup();
    }
    const Int requestedProfile = r.profile == OffscreenGlProfile50DC::Core ? coreProfile : compatibilityProfile;
    const Int contextAttributes[] = { contextMajor, r.major, contextMinor, r.minor,
        contextProfile, requestedProfile, none };
    s.context = s.createContext(s.display, config, nullptr, contextAttributes);
    if (!s.context) return eglFail(OffscreenGlError50DC::ContextFailure, "eglCreateContext");
    if (!s.makeCurrent(s.display, s.surface, s.surface, s.context))
        return eglFail(OffscreenGlError50DC::ContextFailure, "eglMakeCurrent");
    Int api = 0;
    if (!s.queryContext(s.display, s.context, contextClientType, &api))
        return eglFail(OffscreenGlError50DC::ContextFailure, "eglQueryContext/client type");
    if (static_cast<UInt>(api) != openGlApi) {
        fail(d, OffscreenGlError50DC::RequirementsNotMet, "desktop OpenGL client API");
        return cleanup();
    }
    // One-time symbol-loader fallback only. Never change EGL_PLATFORM, driver
    // or other environment settings, or request an alternate rendering API.
    // The installed EGL implementation can itself be a software renderer;
    // successful context creation does not establish hardware acceleration.
    s.glLibrary = dlopen("libOpenGL.so.0", RTLD_NOW | RTLD_LOCAL);
    if (!s.glLibrary) s.glLibrary = dlopen("libGL.so.1", RTLD_NOW | RTLD_LOCAL);
    using GetInteger = void (*)(UInt, Int*);
    using GetGlError = UInt (*)();
    auto getInteger = reinterpret_cast<GetInteger>(s.lookup("glGetIntegerv"));
    auto getGlError = reinterpret_cast<GetGlError>(s.lookup("glGetError"));
    if (!getInteger || !getGlError) {
        fail(d, OffscreenGlError50DC::MissingGlEntryPoint,
            !getInteger ? "glGetIntegerv" : "glGetError");
        return cleanup();
    }
    UInt glError = getGlError();
    if (glError) {
        fail(d, OffscreenGlError50DC::GlFailure, "new context GL error", 0, glError);
        return cleanup();
    }
    Int profile = 0;
    getInteger(glMajorVersion, &info_.glMajor);
    getInteger(glMinorVersion, &info_.glMinor);
    getInteger(glProfileMask, &profile);
    glError = getGlError();
    if (glError) {
        fail(d, OffscreenGlError50DC::GlFailure, "GL version/profile queries", 0, glError);
        return cleanup();
    }
    if (info_.glMajor < r.major || (info_.glMajor == r.major && info_.glMinor < r.minor) ||
        profile != requestedProfile) {
        fail(d, OffscreenGlError50DC::RequirementsNotMet, "GL version/profile");
        return cleanup();
    }
    info_.profile = r.profile;
    s.ready = true;
    return true;
}

bool OffscreenGlContext50DC::bind(OffscreenGlDiagnostic50DC& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, OffscreenGlError50DC::WrongThread, "bind");
    if (!created()) return fail(d, OffscreenGlError50DC::NotCreated, "bind");
    auto& s = *state_;
    if (s.queryApi() != openGlApi) return fail(d, OffscreenGlError50DC::ClientApiChanged, "bind");
    const auto active = s.getCurrentContext();
    if (active && active != s.context)
        return fail(d, OffscreenGlError50DC::ForeignCurrentContext, "bind");
    if (!s.makeCurrent(s.display, s.surface, s.surface, s.context))
        return fail(d, OffscreenGlError50DC::ContextFailure, "eglMakeCurrent", s.getError());
    return current() || fail(d, OffscreenGlError50DC::ContextFailure, "current context verification");
}
bool OffscreenGlContext50DC::release(OffscreenGlDiagnostic50DC& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, OffscreenGlError50DC::WrongThread, "release");
    if (!created()) return fail(d, OffscreenGlError50DC::NotCreated, "release");
    auto& s = *state_;
    if (s.queryApi() != openGlApi) return fail(d, OffscreenGlError50DC::ClientApiChanged, "release");
    const auto active = s.getCurrentContext();
    if (!active) return true;
    if (active != s.context) return fail(d, OffscreenGlError50DC::ForeignCurrentContext, "release");
    if (!s.makeCurrent(s.display, nullptr, nullptr, nullptr))
        return fail(d, OffscreenGlError50DC::ReleaseFailure, "eglMakeCurrent/unbind", s.getError());
    return true;
}
bool OffscreenGlContext50DC::close(OffscreenGlDiagnostic50DC& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, OffscreenGlError50DC::WrongThread, "close");
    return closeOwned(d);
}
bool OffscreenGlContext50DC::closeOwned(OffscreenGlDiagnostic50DC& d) noexcept {
    if (!state_) return true;
    auto& s = *state_;
    s.ready = false;
    // Until eglInitialize succeeds there are no display resources to release.
    // A missing function during initial symbol loading is safe here too.
    if (s.initialized) {
        if (s.queryApi() != openGlApi)
            return fail(d, OffscreenGlError50DC::ClientApiChanged, "close");
        const auto active = s.getCurrentContext();
        if (active && active != s.context)
            return fail(d, OffscreenGlError50DC::ForeignCurrentContext, "close");
        if (active && !s.makeCurrent(s.display, nullptr, nullptr, nullptr))
            return fail(d, OffscreenGlError50DC::ReleaseFailure, "eglMakeCurrent/unbind", s.getError());
        if (s.context) {
            if (!s.destroyContext(s.display, s.context))
                return fail(d, OffscreenGlError50DC::DestroyFailure, "eglDestroyContext", s.getError());
            s.context = nullptr;
        }
        if (s.surface) {
            if (!s.destroySurface(s.display, s.surface))
                return fail(d, OffscreenGlError50DC::DestroyFailure, "eglDestroySurface", s.getError());
            s.surface = nullptr;
        }
        if (!s.terminate(s.display))
            return fail(d, OffscreenGlError50DC::DestroyFailure, "eglTerminate", s.getError());
        s.initialized = false;
        s.display = nullptr;
    }
    if (s.apiBound) {
        // Release the dedicated thread's EGL state before unloading libraries.
        // Merely unbinding/terminating a display need not free thread-local EGL
        // resources. A rejected preexisting context was never ours: preserve
        // that thread's prior API selection instead of releasing its state.
        if (s.threadStateOwned) {
            if (!s.releaseThread())
                return fail(d, OffscreenGlError50DC::DestroyFailure, "eglReleaseThread", s.getError());
            s.threadStateOwned = false;
        } else if (!s.bindApi(s.priorApi))
            return fail(d, OffscreenGlError50DC::DestroyFailure, "eglBindAPI/restore", s.getError());
        s.apiBound = false;
    }
    if (s.glLibrary) {
        if (dlclose(s.glLibrary)) return fail(d, OffscreenGlError50DC::LibraryCloseFailure, "dlclose(GL)");
        s.glLibrary = nullptr;
    }
    if (s.eglLibrary) {
        if (dlclose(s.eglLibrary)) return fail(d, OffscreenGlError50DC::LibraryCloseFailure, "dlclose(EGL)");
        s.eglLibrary = nullptr;
    }
    delete state_;
    state_ = nullptr;
    info_ = {};
    return true;
}
}
