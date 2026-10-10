#include "material_texture_gl50eb.h"
#include <exception>
#include <limits>
#include <new>
#include <thread>
#include <utility>

namespace rr::platform {
namespace {
using U = std::uint32_t;
using I = std::int32_t;
constexpr U texture2D = 0x0DE1, rgba = 0x1908, rgba8 = 0x8058, unsignedByte = 0x1401;
constexpr U minFilter = 0x2801, magFilter = 0x2800, wrapS = 0x2802, wrapT = 0x2803;
constexpr U baseLevel = 0x813C, maximumLevel = 0x813D;
constexpr U pixelUnpackBuffer = 0x88EC, unpackAlignment = 0x0CF5;
constexpr U unpackRowLength = 0x0CF2, unpackSkipRows = 0x0CF3, unpackSkipPixels = 0x0CF4;
constexpr U maxTextureSize = 0x0D33;
constexpr U majorVersion = 0x821B, minorVersion = 0x821C, contextProfileMask = 0x9126;
constexpr I coreProfileBit = 1;
constexpr U textureWidth = 0x1000, textureHeight = 0x1001, textureInternalFormat = 0x1003;

bool fail(MaterialTextureGlDiagnostic50EB& d, MaterialTextureGlError50EB error,
    const char* operation) noexcept {
    if (d.error == MaterialTextureGlError50EB::none) { d.error = error; d.operation = operation; }
    return false;
}
I filter(MaterialTextureFilter50EB value) noexcept {
    switch (value) {
    case MaterialTextureFilter50EB::nearest: return 0x2600;
    case MaterialTextureFilter50EB::linear: return 0x2601;
    default: return 0;
    }
}
I wrap(MaterialTextureWrap50EB value) noexcept {
    switch (value) {
    case MaterialTextureWrap50EB::repeat: return 0x2901;
    case MaterialTextureWrap50EB::clampToEdge: return 0x812F;
    default: return 0;
    }
}
bool validate(const MaterialTextureGlRequest50EB& request,
    const assets::MaterialTextureAssets50EA& bank, std::size_t& total,
    MaterialTextureGlDiagnostic50EB& d) noexcept {
    if (!request.isCurrent)
        return fail(d, MaterialTextureGlError50EB::request, "exact-current-context predicate required");
#define REQUIRE(member, name) if (!request.gl.member) \
    return fail(d, MaterialTextureGlError50EB::entryPoint, name)
    REQUIRE(genTextures, "glGenTextures"); REQUIRE(deleteTextures, "glDeleteTextures");
    REQUIRE(bindTexture, "glBindTexture"); REQUIRE(texParameteri, "glTexParameteri");
    REQUIRE(texImage2D, "glTexImage2D"); REQUIRE(getTexLevelParameteriv, "glGetTexLevelParameteriv");
    REQUIRE(bindBuffer, "glBindBuffer"); REQUIRE(pixelStorei, "glPixelStorei");
    REQUIRE(getIntegerv, "glGetIntegerv"); REQUIRE(getError, "glGetError");
#undef REQUIRE
    if (!bank.loaded() || bank.textureCount() > assets::maxMaterialTextureAssets50EA)
        return fail(d, MaterialTextureGlError50EB::assets, "loaded bounded 50EA bank required");
    if (request.samplerCount != bank.textureCount() ||
        (request.samplerCount && !request.samplers))
        return fail(d, MaterialTextureGlError50EB::request, "one explicit sampler per bank texture required");
    total = 0;
    for (U t = 0; t < bank.textureCount(); ++t) {
        d.texture = t;
        const auto* asset = bank.texture(t);
        if (!asset)
            return fail(d, MaterialTextureGlError50EB::assets, "missing immutable bank texture");
        const auto& texture = asset->texture;
        if (!texture.width || !texture.height ||
            texture.width > assets::max_texture_dimension || texture.height > assets::max_texture_dimension ||
            texture.width > static_cast<U>(std::numeric_limits<I>::max()) ||
            texture.height > static_cast<U>(std::numeric_limits<I>::max()))
            return fail(d, MaterialTextureGlError50EB::assets, "invalid RGBA8 dimensions");
        const std::uint64_t bytes = 4ull * texture.width * texture.height;
        if (bytes > std::numeric_limits<std::size_t>::max() || bytes != texture.rgba.size())
            return fail(d, MaterialTextureGlError50EB::assets, "RGBA8 byte extent mismatch");
        if (total > request.maximumGpuBytes || bytes > request.maximumGpuBytes - total ||
            total > assets::maxMaterialTextureRGBABytes50EA ||
            bytes > assets::maxMaterialTextureRGBABytes50EA - total)
            return fail(d, MaterialTextureGlError50EB::budget, "RGBA8 upload exceeds explicit GPU byte budget");
        total += static_cast<std::size_t>(bytes);
        const auto& sampler = request.samplers[t];
        if (!filter(sampler.minification) || !filter(sampler.magnification) || !wrap(sampler.s) || !wrap(sampler.t))
            return fail(d, MaterialTextureGlError50EB::sampler, "explicit nonmip filters and wrap modes required");
    }
    d.texture = assets::unresolvedMaterialTexture50EA;
    return total == bank.rgbaByteCount() ||
        fail(d, MaterialTextureGlError50EB::assets, "immutable bank RGBA total mismatch");
}
}

struct MaterialTextureGl50EB::Storage {
    // One fixed, bounded allocation. No RGBA copy or per-texture CPU allocation.
    MaterialTextureGlAsset50EB textures[assets::maxMaterialTextureAssets50EA]{};
    MaterialTextureGlFunctions50EB gl{};
    void* context = nullptr;
    bool (*isCurrent)(void*) noexcept = nullptr;
    const std::thread::id thread = std::this_thread::get_id();
    U count = 0;
    std::size_t bytes = 0;
    bool initialized = false;

    bool current(MaterialTextureGlDiagnostic50EB& d) const noexcept {
        if (thread != std::this_thread::get_id())
            return fail(d, MaterialTextureGlError50EB::wrongThread, "texture owner requires creating thread");
        return (isCurrent && isCurrent(context)) ||
            fail(d, MaterialTextureGlError50EB::notCurrent, "caller texture context is not current");
    }
    bool hasNames() const noexcept {
        for (U t = 0; t < count; ++t) if (textures[t].name) return true;
        return false;
    }
    bool check(MaterialTextureGlDiagnostic50EB& d, const char* operation) const noexcept {
        const U error = gl.getError();
        if (!error) return true;
        if (!d.glError) d.glError = error;
        return fail(d, MaterialTextureGlError50EB::gl, operation);
    }
    bool upload(const assets::MaterialTextureAssets50EA& bank,
        const MaterialTextureSampler50EB* samplers, MaterialTextureGlDiagnostic50EB& d) noexcept {
        if (!current(d)) return false;
        // A valid empty/all-unresolved bank publishes without issuing GL calls.
        if (!count) { initialized = true; return true; }
        if (!check(d, "before cold texture upload")) return false;
        // Core profile excludes legacy pixel-transfer maps/scales/bias that
        // could otherwise alter the supplied bytes during texture upload.
        I major = 0, minor = 0, profile = 0, maximumDimension = 0;
        gl.getIntegerv(majorVersion, &major);
        gl.getIntegerv(minorVersion, &minor);
        gl.getIntegerv(contextProfileMask, &profile);
        if (!check(d, "query desktop core GL version/profile")) return false;
        if (major < 3 || (major == 3 && minor < 2) || !(profile & coreProfileBit))
            return fail(d, MaterialTextureGlError50EB::limits, "desktop core GL 3.2 or newer required");
        gl.getIntegerv(maxTextureSize, &maximumDimension);
        if (!check(d, "glGetIntegerv(GL_MAX_TEXTURE_SIZE)")) return false;
        if (maximumDimension <= 0)
            return fail(d, MaterialTextureGlError50EB::limits, "invalid GL maximum texture dimension");
        for (U t = 0; t < count; ++t) {
            d.texture = t;
            const auto& input = *bank.texture(t);
            if (input.texture.width > static_cast<U>(maximumDimension) ||
                input.texture.height > static_cast<U>(maximumDimension))
                return fail(d, MaterialTextureGlError50EB::limits, "texture exceeds GL maximum dimension");
            textures[t].identity = input.identity;
        }
        d.texture = assets::unresolvedMaterialTexture50EA;
        // Client pointers must never be interpreted as offsets into a PBO, and
        // caller row/skip state must never change the actual source byte range.
        gl.bindBuffer(pixelUnpackBuffer, 0);
        gl.pixelStorei(unpackAlignment, 1);
        gl.pixelStorei(unpackRowLength, 0);
        gl.pixelStorei(unpackSkipRows, 0);
        gl.pixelStorei(unpackSkipPixels, 0);
        if (!check(d, "prepare tightly packed client RGBA8 unpack")) return false;
        for (U t = 0; t < count; ++t) {
            d.texture = t;
            auto& owned = textures[t];
            const auto& input = bank.texture(t)->texture;
            const auto& sampler = samplers[t];
            // Store the returned name before checking errors so partial
            // generation is also covered by the ordinary cleanup path.
            gl.genTextures(1, &owned.name);
            if (!check(d, "glGenTextures")) return false;
            if (!owned.name)
                return fail(d, MaterialTextureGlError50EB::allocation, "zero generated texture name");
            gl.bindTexture(texture2D, owned.name);
            if (!check(d, "glBindTexture(GL_TEXTURE_2D)")) return false;
            gl.texParameteri(texture2D, minFilter, filter(sampler.minification));
            gl.texParameteri(texture2D, magFilter, filter(sampler.magnification));
            gl.texParameteri(texture2D, wrapS, wrap(sampler.s));
            gl.texParameteri(texture2D, wrapT, wrap(sampler.t));
            gl.texParameteri(texture2D, baseLevel, 0);
            gl.texParameteri(texture2D, maximumLevel, 0);
            if (!check(d, "apply explicit level-zero texture sampling")) return false;
            gl.texImage2D(texture2D, 0, static_cast<I>(rgba8), static_cast<I>(input.width),
                static_cast<I>(input.height), 0, rgba, unsignedByte, input.rgba.data());
            if (!check(d, "upload actual level-zero RGBA8 bytes")) return false;
            I width = 0, height = 0, format = 0;
            gl.getTexLevelParameteriv(texture2D, 0, textureWidth, &width);
            gl.getTexLevelParameteriv(texture2D, 0, textureHeight, &height);
            gl.getTexLevelParameteriv(texture2D, 0, textureInternalFormat, &format);
            if (!check(d, "verify level-zero RGBA8 allocation")) return false;
            if (width != static_cast<I>(input.width) || height != static_cast<I>(input.height) ||
                format != static_cast<I>(rgba8))
                return fail(d, MaterialTextureGlError50EB::storage, "GL texture size/format mismatch");
        }
        d.texture = assets::unresolvedMaterialTexture50EA;
        gl.bindTexture(texture2D, 0);
        if (!check(d, "clear cold-upload GL_TEXTURE_2D binding")) return false;
        initialized = true;
        return true;
    }
    bool cleanup(MaterialTextureGlDiagnostic50EB& d) noexcept {
        initialized = false;
        if (thread != std::this_thread::get_id()) {
            d.cleanupIncomplete = true;
            return fail(d, MaterialTextureGlError50EB::wrongThread, "texture cleanup requires creating thread");
        }
        if (!hasNames()) { d.cleanupIncomplete = false; return true; }
        d.cleanupIncomplete = true;
        if (!current(d)) return false;
        // A bounded drain prevents old failures from being attributed to a
        // deletion. If it cannot settle, preserve every name for recovery.
        bool clean = false;
        for (unsigned n = 0; n < 32; ++n) {
            const U error = gl.getError();
            if (!error) { clean = true; break; }
            if (!d.cleanupGlError) d.cleanupGlError = error;
        }
        if (!clean) {
            d.errorStateUnclear = true;
            return fail(d, MaterialTextureGlError50EB::cleanup, "GL error state will not clear for texture cleanup");
        }
        for (U t = 0; t < count; ++t) {
            auto& name = textures[t].name;
            if (!name) continue;
            d.cleanupTexture = t;
            gl.deleteTextures(1, &name);
            const U error = gl.getError();
            if (error) {
                if (!d.cleanupGlError) d.cleanupGlError = error;
                // The context owner must not generate/reuse names before a
                // retry: this deletion's acceptance is not established.
                return fail(d, MaterialTextureGlError50EB::cleanup, "GL texture deletion failed");
            }
            name = 0;
        }
        d.cleanupTexture = assets::unresolvedMaterialTexture50EA;
        d.cleanupIncomplete = false;
        return !d.cleanupGlError ||
            fail(d, MaterialTextureGlError50EB::cleanup, "texture cleanup completed after GL errors");
    }
};

MaterialTextureGl50EB::MaterialTextureGl50EB() noexcept = default;
MaterialTextureGl50EB::~MaterialTextureGl50EB() noexcept {
    MaterialTextureGlDiagnostic50EB diagnostic{};
    close(diagnostic);
    if (diagnostic.cleanupIncomplete) std::terminate();
}
bool MaterialTextureGl50EB::create(const MaterialTextureGlRequest50EB& request,
    const assets::MaterialTextureAssets50EA& bank, MaterialTextureGlDiagnostic50EB& d) noexcept {
    d = {};
    if (storage_)
        return fail(d, MaterialTextureGlError50EB::alreadyOwned, "texture owner already owns a bank");
    if (lastPublication_ == std::numeric_limits<std::uint64_t>::max())
        return fail(d, MaterialTextureGlError50EB::limits, "texture publication token exhausted");
    std::size_t bytes = 0;
    if (!validate(request, bank, bytes, d)) return false;
    std::unique_ptr<Storage> next(new (std::nothrow) Storage);
    if (!next)
        return fail(d, MaterialTextureGlError50EB::allocation, "allocate bounded texture owner metadata");
    next->gl = request.gl;
    next->context = request.context;
    next->isCurrent = request.isCurrent;
    next->count = bank.textureCount();
    next->bytes = bytes;
    if (!next->upload(bank, request.samplers, d)) {
        next->cleanup(d);
        if (d.cleanupIncomplete) storage_ = std::move(next);
        return false;
    }
    storage_ = std::move(next);
    ++lastPublication_;
    return true;
}
bool MaterialTextureGl50EB::close(MaterialTextureGlDiagnostic50EB& d) noexcept {
    d = {};
    if (!storage_) return true;
    const bool result = storage_->cleanup(d);
    if (!d.cleanupIncomplete) storage_.reset();
    return result;
}
bool MaterialTextureGl50EB::initialized() const noexcept { return storage_ && storage_->initialized; }
bool MaterialTextureGl50EB::ownsResources() const noexcept { return bool(storage_); }
std::uint32_t MaterialTextureGl50EB::textureCount() const noexcept {
    return initialized() ? storage_->count : 0;
}
std::size_t MaterialTextureGl50EB::gpuByteCount() const noexcept {
    return initialized() ? storage_->bytes : 0;
}
std::uint64_t MaterialTextureGl50EB::publicationToken() const noexcept {
    return initialized() ? lastPublication_ : 0;
}
bool MaterialTextureGl50EB::validBorrow(void* context, bool (*isCurrent)(void*) noexcept,
    std::uint64_t token) const noexcept {
    return token && token == publicationToken() && storage_->context == context &&
        storage_->isCurrent == isCurrent && storage_->thread == std::this_thread::get_id() &&
        isCurrent && isCurrent(context);
}
const MaterialTextureGlAsset50EB* MaterialTextureGl50EB::asset(U index) const noexcept {
    return initialized() && index < storage_->count ? &storage_->textures[index] : nullptr;
}
std::uint32_t MaterialTextureGl50EB::texture(U index) const noexcept {
    const auto* result = asset(index);
    return result ? result->name : 0;
}
}
