#pragma once
#include "../../../src/assets/material_texture_assets50ea.h"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace rr::platform {
// Linux desktop core GL >= 3.2 signatures. Resolve these in the context owner; no
// GL headers, loader/link dependency, context creation or global proc table.
// Every entry is required, synchronous, nonthrowing and non-reentrant.
struct MaterialTextureGlFunctions50EB {
    void (*genTextures)(std::int32_t, std::uint32_t*) = nullptr;
    void (*deleteTextures)(std::int32_t, const std::uint32_t*) = nullptr;
    void (*bindTexture)(std::uint32_t target, std::uint32_t texture) = nullptr;
    void (*texParameteri)(std::uint32_t target, std::uint32_t pname, std::int32_t) = nullptr;
    void (*texImage2D)(std::uint32_t target, std::int32_t level,
        std::int32_t internalFormat, std::int32_t width, std::int32_t height,
        std::int32_t border, std::uint32_t format, std::uint32_t type, const void*) = nullptr;
    void (*getTexLevelParameteriv)(std::uint32_t target, std::int32_t level,
        std::uint32_t pname, std::int32_t*) = nullptr;
    void (*bindBuffer)(std::uint32_t target, std::uint32_t buffer) = nullptr;
    void (*pixelStorei)(std::uint32_t pname, std::int32_t) = nullptr;
    void (*getIntegerv)(std::uint32_t pname, std::int32_t*) = nullptr;
    std::uint32_t (*getError)() = nullptr;
};
enum class MaterialTextureFilter50EB : std::uint8_t { unspecified, nearest, linear };
enum class MaterialTextureWrap50EB : std::uint8_t { unspecified, repeat, clampToEdge };
struct MaterialTextureSampler50EB {
    // Explicit caller policy, never claimed as recovered original-game state.
    MaterialTextureFilter50EB minification = MaterialTextureFilter50EB::unspecified;
    MaterialTextureFilter50EB magnification = MaterialTextureFilter50EB::unspecified;
    MaterialTextureWrap50EB s = MaterialTextureWrap50EB::unspecified;
    MaterialTextureWrap50EB t = MaterialTextureWrap50EB::unspecified;
};
struct MaterialTextureGlRequest50EB {
    // Borrowed until close. Check this exact live/current context (and its
    // intended drawable, if any) without GL calls. Never rebind/release it.
    void* context = nullptr;
    bool (*isCurrent)(void*) noexcept = nullptr;
    MaterialTextureGlFunctions50EB gl{};
    // Exact bank-index order; one fully specified sampler for every texture.
    // Borrowed during create only. Zero textures require samplerCount == 0.
    const MaterialTextureSampler50EB* samplers = nullptr;
    std::uint32_t samplerCount = 0;
    // Explicit ceiling for sum(width * height * 4), including unreferenced
    // real bank textures. Zero admits only an empty texture bank. GL driver
    // metadata/padding are implementation-dependent and outside this count.
    std::size_t maximumGpuBytes = 0;
};
struct MaterialTextureGlAsset50EB {
    std::uint64_t identity = 0; // Exact 50EA caller identity, including zero.
    std::uint32_t name = 0;    // Borrowed GLuint; never delete or mutate it.
};
enum class MaterialTextureGlError50EB : std::uint8_t {
    none, alreadyOwned, request, assets, budget, sampler, allocation, entryPoint,
    limits, gl, storage, wrongThread, notCurrent, cleanup
};
struct MaterialTextureGlDiagnostic50EB {
    MaterialTextureGlError50EB error = MaterialTextureGlError50EB::none;
    const char* operation = nullptr; // Static; first failure wins.
    std::uint32_t texture = assets::unresolvedMaterialTexture50EA;
    std::uint32_t cleanupTexture = assets::unresolvedMaterialTexture50EA;
    std::uint32_t glError = 0, cleanupGlError = 0;
    bool errorStateUnclear = false, cleanupIncomplete = false;
};

// Cold, load-once GPU owner for one immutable 50EA texture table. Uploads exact
// RGBA8 bytes once at level zero; no generated pixels, mipmaps, slot fallback,
// channel interpretation, shader/draw calls, or renderer integration. All CPU
// inputs and byte budgets validate before allocation or any GL call. The GL
// maximum dimension is checked for the whole bank before generating a name.
// One bounded cold metadata allocation; no allocation/GL work in lookup.
//
// Caller owns one unshared desktop core GL context and serial creating-thread use.
// Keep callback storage/context alive until successful cleanup. The CPU bank
// and sampler array are borrowed only during create, including failure paths.
// Caller may bind/sample published names but must not delete, redefine or
// change their parameters, attach them to framebuffer/image targets, or
// mutate them elsewhere.
//
// State is NOT restored. Upload uses the already active texture unit, leaves
// its TEXTURE_2D binding zero on success, and sets PIXEL_UNPACK_BUFFER to zero,
// UNPACK_ALIGNMENT to 1, ROW_LENGTH/SKIP_ROWS/SKIP_PIXELS to zero. Failure can
// leave partial changes to those states. Active unit and all other state are
// untouched, except GL error consumption and deletion's implicit unbinding of
// owned textures. An external sampler object can override the chosen texture
// parameters; the eventual draw caller owns its sampler/unit bindings.
class MaterialTextureGl50EB final {
public:
    MaterialTextureGl50EB() noexcept;
    // Explicit close before host/context destruction is required. Unresolved
    // cleanup on destruction terminates rather than forgetting owned names.
    ~MaterialTextureGl50EB() noexcept;
    MaterialTextureGl50EB(const MaterialTextureGl50EB&) = delete;
    MaterialTextureGl50EB& operator=(const MaterialTextureGl50EB&) = delete;
    MaterialTextureGl50EB(MaterialTextureGl50EB&&) = delete;
    MaterialTextureGl50EB& operator=(MaterialTextureGl50EB&&) = delete;

    // Inputs and diagnostics must be distinct live storage. Retry after a
    // fully cleaned-up failure; an existing publication is never replaced.
    bool create(const MaterialTextureGlRequest50EB&, const assets::MaterialTextureAssets50EA&,
        MaterialTextureGlDiagnostic50EB&) noexcept;
    // Any close attempt ends publication. A failure retains unresolved names
    // for retry; never create/reuse texture names between failed close calls.
    // cleanupIncomplete == false means ownership ended even if stale GL errors
    // made the return value false. Empty close is idempotent and requires no GL.
    bool close(MaterialTextureGlDiagnostic50EB&) noexcept;
    bool initialized() const noexcept;
    bool ownsResources() const noexcept; // Includes a published empty bank.
    std::uint32_t textureCount() const noexcept;
    std::size_t gpuByteCount() const noexcept;
    // Nonzero only for the current published bank. Each successful create on
    // this live owner has a distinct token, including close/recreate cycles.
    std::uint64_t publicationToken() const noexcept;
    // Read-only, allocation-free, and GL-free borrower validation. Requires
    // this exact callback/context pair, the creating thread, the same live
    // publication, and the callback's current-context check. Keep this owner
    // alive; never close/recreate it while borrowed rendering is in progress.
    bool validBorrow(void* context, bool (*isCurrent)(void*) noexcept,
        std::uint64_t publicationToken) const noexcept;
    // Exact original bank index; no remapping or identity-derived GL names.
    // Null/zero for unpublished, invalid, or unresolved-sentinel indexes.
    // Returned metadata pointer/name is borrowed until the next close attempt.
    const MaterialTextureGlAsset50EB* asset(std::uint32_t bankIndex) const noexcept;
    std::uint32_t texture(std::uint32_t bankIndex) const noexcept;

private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
    std::uint64_t lastPublication_ = 0;
};
}
