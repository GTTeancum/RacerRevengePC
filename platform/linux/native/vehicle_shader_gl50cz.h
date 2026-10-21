#pragma once
#include <cstdint>

namespace rr::platform {
// Desktop OpenGL 3.2 / GLSL 150 entry points supplied by the context owner.
// No GL headers, loader, link dependency, context creation or capability probe.
// All entries are required and must be synchronous, nonthrowing, non-reentrant.
struct VehicleGlShaderFunctions50CZ {
    std::uint32_t (*createShader)(std::uint32_t type) = nullptr;
    void (*shaderSource)(std::uint32_t shader, std::int32_t count,
        const char* const* strings, const std::int32_t* lengths) = nullptr;
    void (*compileShader)(std::uint32_t shader) = nullptr;
    void (*getShaderiv)(std::uint32_t shader, std::uint32_t pname, std::int32_t*) = nullptr;
    void (*getShaderInfoLog)(std::uint32_t shader, std::int32_t capacity,
        std::int32_t* length, char*) = nullptr;
    void (*deleteShader)(std::uint32_t shader) = nullptr;
    std::uint32_t (*createProgram)() = nullptr;
    void (*attachShader)(std::uint32_t program, std::uint32_t shader) = nullptr;
    void (*bindAttribLocation)(std::uint32_t program, std::uint32_t index, const char*) = nullptr;
    void (*bindFragDataLocation)(std::uint32_t program, std::uint32_t index, const char*) = nullptr;
    void (*linkProgram)(std::uint32_t program) = nullptr;
    void (*getProgramiv)(std::uint32_t program, std::uint32_t pname, std::int32_t*) = nullptr;
    void (*getProgramInfoLog)(std::uint32_t program, std::int32_t capacity,
        std::int32_t* length, char*) = nullptr;
    std::int32_t (*getUniformLocation)(std::uint32_t program, const char*) = nullptr;
    void (*deleteProgram)(std::uint32_t program) = nullptr;
    std::uint32_t (*getError)() = nullptr;
};

// Vertex48B: stride 24; float3 at byte 0, float2 at 12, normalized unsigned-byte
// RGBA at 20. Configure ordinary vertex attributes, not integer attributes.
constexpr std::uint32_t vehicleGlPositionAttribute50CZ = 0;
constexpr std::uint32_t vehicleGlUvAttribute50CZ = 1;
constexpr std::uint32_t vehicleGlColorAttribute50CZ = 2;
constexpr std::uint32_t vehicleGlNormalAttribute50CZ = 3;
constexpr std::uint32_t vehicleGlSpecularUvAttribute50LA = 4;
// Separate float scalar in source byte units [0,255], not a normalized color.
constexpr std::uint32_t vehicleGlSourceFogAttribute50LA = 5;
constexpr std::uint32_t vehicleGlColorOutput50CZ = 0;

struct VehicleGlShaderUniforms50CZ {
    // mat4; shader evaluates vec4(worldPosition, 1) * worldViewProjection.
    // Caller supplies its complete GL clip-space transform, including any
    // needed transpose/depth conversion. No model/root or viewport is added.
    std::int32_t worldViewProjection = -1;
    // int; explicitly write 0 (viewer neutral color) or 1 (sample * vertex RGB).
    std::int32_t useTexture = -1;
    // sampler2D; explicitly write the caller-selected texture-unit INDEX.
    std::int32_t image = -1;
    // Defaults (all zero at GL link) preserve the original opaque shader.
    // effectMode: 0 opaque, 1 source-alpha, 2 fixed-alpha additive, 3 source RGB.
    std::int32_t effectMode = -1, alphaScale = -1, suppressDraw = -1;
    // UV scale/offset is ignored unless useUvTransform is explicitly nonzero.
    std::int32_t useUvTransform = -1, uvScaleOffset = -1;
    // Opt-in host directional lighting; zero retains diagnostic/effect RGB.
    // Authored world normals are float3 attribute 3 in a separate stream.
    std::int32_t useLighting = -1;
    // Explicit native GC selected-light policy, independently disabled for
    // every nonadmitted material. RGB coefficients are source colors / 128;
    // direction XYZ is the selected world-space column, already toward light.
    // Padded vec4 uniforms use the existing uniform4f entry point only.
    std::int32_t useNativeGcSelectedLights = -1, nativeGcAmbient = -1;
    std::int32_t nativeGcDiffuse[3] = {-1, -1, -1};
    std::int32_t nativeGcDirection[3] = {-1, -1, -1};
    // Explicit base-map original packed-RGB MODULATE; bypasses host lighting.
    // Generated color uses its own noperspective varying. Legacy unpaired
    // alpha stays 1 unless the distinct single-map RGBA capability is enabled.
    std::int32_t useSourceDiffuseCompressed50LA = -1;
    // 0 preserves legacy opaque alpha, 1 writes paired base RGBA, 2 performs
    // the ordered source map-2 equation using the current destination byte A.
    std::int32_t sourcePairPass50LA = -1, sourcePairFog50LA = -1;
    std::int32_t sourcePairDestination50LA = -1, sourcePairAlphaTest50LA = -1;
    // Independent static single-map RGBA path. Fog RGB is vec4.xyz in byte units (padding W=0);
    // the scalar byte fog attribute is interpolated screen-linearly. All four
    // locations are required even when this opt-in capability is disabled.
    std::int32_t sourceSingleDiffuseRgba50LA = -1, sourceSingleFogColor50LA = -1;
    std::int32_t sourceSingleFogEnabled50LA = -1, sourceSingleAlphaTest50LA = -1;
};

enum class VehicleGlShaderStage50CZ : std::uint8_t {
    none, entryPoints, alreadyOwned, entryError, vertex, fragment, link, uniforms, cleanup
};
struct VehicleGlShaderDiagnostic50CZ {
    VehicleGlShaderStage50CZ stage = VehicleGlShaderStage50CZ::none;
    std::uint32_t glError = 0, cleanupGlError = 0;
    // A bounded error drain did not reach GL_NO_ERROR. Stop GL work and let the
    // context owner recover; cleanup has not forgotten any still-owned names.
    bool errorStateUnclear = false, cleanupIncomplete = false;
    char log[512]{}; // Bounded, NUL-terminated compile/link log, possibly truncated.
};

const char* vehicleGlVertexSource50CZ() noexcept;
const char* vehicleGlFragmentSource50CZ() noexcept;

// Cold-only ownership of one program and its two attached shader objects.
// initialize/release require the SAME live/current GL share group, with a clean
// GL error state and no concurrent GL work. Caller verifies that BEFORE entry.
// No method binds the program, writes uniforms or chooses textures/state.
class VehicleGlShader50CZ {
public:
    explicit VehicleGlShader50CZ(const VehicleGlShaderFunctions50CZ&) noexcept;
    VehicleGlShader50CZ(const VehicleGlShader50CZ&) = delete;
    VehicleGlShader50CZ& operator=(const VehicleGlShader50CZ&) = delete;
    // Deliberately no implicit GL cleanup: call release while the context is
    // current, before destruction. Never delete/relink the borrowed names.
    ~VehicleGlShader50CZ() = default;
    // coveragePayload is private native AA1-demand plumbing: same linked
    // position/source-byte kernel, with pass3 writing an AA0 scratch payload.
    // Default false retains the existing single-sample program route.
    bool initialize(VehicleGlShaderDiagnostic50CZ&, bool coveragePayload = false) noexcept;
    // First unbind this program in every context using it. Deletion may be
    // deferred by GL otherwise. False retains any unresolved ownership; keep
    // this object alive and recover the context before attempting release again.
    bool release(VehicleGlShaderDiagnostic50CZ&) noexcept;
    std::uint32_t program() const noexcept { return ready_ ? program_ : 0; }
    const VehicleGlShaderUniforms50CZ& uniforms() const noexcept { return uniforms_; }
    bool hasResources() const noexcept { return program_ || vertex_ || fragment_; }
private:
    bool noGlError(VehicleGlShaderDiagnostic50CZ&, bool cleanup = false) noexcept;
    bool compile(std::uint32_t type, const char* source, std::uint32_t&,
        VehicleGlShaderDiagnostic50CZ&, bool coveragePayload = false) noexcept;
    void cleanup(VehicleGlShaderDiagnostic50CZ&) noexcept;
    VehicleGlShaderFunctions50CZ gl_;
    VehicleGlShaderUniforms50CZ uniforms_{};
    std::uint32_t program_ = 0, vertex_ = 0, fragment_ = 0;
    bool ready_ = false;
};
}
