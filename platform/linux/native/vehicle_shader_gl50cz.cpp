#include "vehicle_shader_gl50cz.h"
#include "../../../src/assets/native_asset48b.h"
#include <cstddef>
#include <type_traits>

namespace rr::platform {
namespace {
constexpr std::uint32_t vertexShader = 0x8B31, fragmentShader = 0x8B30;
constexpr std::uint32_t compileStatus = 0x8B81, linkStatus = 0x8B82;
static_assert(std::is_standard_layout<assets::Vertex48B>::value && sizeof(float) == 4 &&
    sizeof(assets::Vertex48B) == 24 && offsetof(assets::Vertex48B, position) == 0 &&
    offsetof(assets::Vertex48B, uv) == 12 && offsetof(assets::Vertex48B, color) == 20,
    "48B viewer / 50CV GL attribute layout");
}

// Default diagnostic behavior from platform/windows/native/main48b.cpp, with
// explicit opt-in native source-normal lighting for the window renderer.
// This is not a recovered original-game material program or PS2 transform.
const char* vehicleGlVertexSource50CZ() noexcept {
    return R"GLSL(#version 150 core
in vec3 rrPosition;
in vec2 rrUv;
in vec4 rrColor;
in vec3 rrWorldNormal;
uniform mat4 worldViewProjection;
uniform int useUvTransform;
uniform vec4 uvScaleOffset;
out vec2 viewerUv;
out vec4 viewerColor;
out vec3 viewerWorldPosition;
out vec3 viewerWorldNormal;
void main() {
    gl_Position = vec4(rrPosition, 1.0) * worldViewProjection;
    viewerUv = useUvTransform != 0 ? rrUv * uvScaleOffset.xy + uvScaleOffset.zw : rrUv;
    viewerColor = rrColor;
    viewerWorldPosition = rrPosition;
    viewerWorldNormal = rrWorldNormal;
}
)GLSL";
}

const char* vehicleGlFragmentSource50CZ() noexcept {
    return R"GLSL(#version 150 core
in vec2 viewerUv;
in vec4 viewerColor;
in vec3 viewerWorldPosition;
in vec3 viewerWorldNormal;
uniform int useTexture;
uniform sampler2D image;
uniform int effectMode;
uniform float alphaScale;
uniform int suppressDraw;
uniform int useLighting;
out vec4 viewerOutput;
void main() {
    // Evaluate derivatives before any discard. Source normals retain their
    // authored signs; only the zero-normal fallback follows triangle winding.
    vec3 plane = cross(dFdx(viewerWorldPosition), dFdy(viewerWorldPosition));
    float planeLength = dot(plane, plane);
    vec3 normal = vec3(0.0, 1.0, 0.0);
    float authoredLength = dot(viewerWorldNormal, viewerWorldNormal);
    if (authoredLength > 0.000001) normal = viewerWorldNormal * inversesqrt(authoredLength);
    else if (planeLength > 0.0) {
        normal = plane * inversesqrt(planeLength);
        if (!gl_FrontFacing) normal = -normal;
    }
    if (suppressDraw != 0) discard;
    vec4 texel = useTexture != 0 ? texture(image, viewerUv) : vec4(1.0);
    vec3 color = useTexture != 0 || effectMode != 0 ?
        texel.rgb * viewerColor.rgb : vec3(0.68, 0.72, 0.78);
    float alpha = 1.0;
    if (effectMode == 1) alpha = clamp(texel.a * viewerColor.a * alphaScale, 0.0, 1.0);
    if (effectMode == 2) color *= alphaScale;
    if (useLighting != 0) {
        vec3 lightDirection = normalize(vec3(0.35, 0.85, 0.30));
        float illumination = 0.32 + 0.16 * max(normal.y, 0.0) +
            0.52 * max(dot(normal, lightDirection), 0.0);
        color *= illumination;
    }
    viewerOutput = vec4(color, alpha);
}
)GLSL";
}

VehicleGlShader50CZ::VehicleGlShader50CZ(const VehicleGlShaderFunctions50CZ& gl) noexcept
    : gl_(gl) {}

bool VehicleGlShader50CZ::noGlError(VehicleGlShaderDiagnostic50CZ& diagnostic,
    bool duringCleanup) noexcept {
    const auto error = gl_.getError();
    if (!error) return true;
    auto& first = duringCleanup ? diagnostic.cleanupGlError : diagnostic.glError;
    if (!first) first = error;
    // Consume residual errors before any cleanup so they cannot be mistaken
    // for deletion failures. Never spin forever on a lost/broken context.
    for (unsigned i = 0; i < 16; ++i)
        if (!gl_.getError()) return false;
    diagnostic.errorStateUnclear = true;
    return false;
}

bool VehicleGlShader50CZ::compile(std::uint32_t type, const char* source,
    std::uint32_t& shader, VehicleGlShaderDiagnostic50CZ& diagnostic) noexcept {
    shader = gl_.createShader(type);
    if (!noGlError(diagnostic) || !shader) return false;
    gl_.shaderSource(shader, 1, &source, nullptr);
    if (!noGlError(diagnostic)) return false;
    gl_.compileShader(shader);
    if (!noGlError(diagnostic)) return false;
    std::int32_t compiled = 0;
    gl_.getShaderiv(shader, compileStatus, &compiled);
    if (!noGlError(diagnostic)) return false;
    if (compiled) return true;
    gl_.getShaderInfoLog(shader, static_cast<std::int32_t>(sizeof(diagnostic.log)),
        nullptr, diagnostic.log);
    diagnostic.log[sizeof(diagnostic.log) - 1] = '\0';
    noGlError(diagnostic);
    return false;
}

void VehicleGlShader50CZ::cleanup(VehicleGlShaderDiagnostic50CZ& diagnostic) noexcept {
    ready_ = false;
    uniforms_ = {};
    // Program first, releasing its attachments, then both separately owned
    // shaders. Keep names whose deletion reported an error; never lose them.
    if (program_ && !diagnostic.errorStateUnclear) {
        gl_.deleteProgram(program_);
        if (noGlError(diagnostic, true)) program_ = 0;
    }
    if (vertex_ && !diagnostic.errorStateUnclear) {
        gl_.deleteShader(vertex_);
        if (noGlError(diagnostic, true)) vertex_ = 0;
    }
    if (fragment_ && !diagnostic.errorStateUnclear) {
        gl_.deleteShader(fragment_);
        if (noGlError(diagnostic, true)) fragment_ = 0;
    }
    diagnostic.cleanupIncomplete = hasResources();
}

bool VehicleGlShader50CZ::initialize(VehicleGlShaderDiagnostic50CZ& diagnostic) noexcept {
    diagnostic = {};
    if (hasResources()) {
        diagnostic.stage = VehicleGlShaderStage50CZ::alreadyOwned;
        return false; // Preserve an existing successful program unchanged.
    }
    if (!gl_.createShader || !gl_.shaderSource || !gl_.compileShader ||
        !gl_.getShaderiv || !gl_.getShaderInfoLog || !gl_.deleteShader ||
        !gl_.createProgram || !gl_.attachShader || !gl_.bindAttribLocation ||
        !gl_.bindFragDataLocation || !gl_.linkProgram || !gl_.getProgramiv ||
        !gl_.getProgramInfoLog || !gl_.getUniformLocation || !gl_.deleteProgram || !gl_.getError) {
        diagnostic.stage = VehicleGlShaderStage50CZ::entryPoints;
        return false;
    }
    diagnostic.stage = VehicleGlShaderStage50CZ::entryError;
    if (!noGlError(diagnostic)) return false;
    const auto fail = [&]() noexcept { cleanup(diagnostic); return false; };
    diagnostic.stage = VehicleGlShaderStage50CZ::vertex;
    if (!compile(vertexShader, vehicleGlVertexSource50CZ(), vertex_, diagnostic)) return fail();
    diagnostic.stage = VehicleGlShaderStage50CZ::fragment;
    if (!compile(fragmentShader, vehicleGlFragmentSource50CZ(), fragment_, diagnostic)) return fail();
    diagnostic.stage = VehicleGlShaderStage50CZ::link;
    program_ = gl_.createProgram();
    if (!noGlError(diagnostic) || !program_) return fail();
    gl_.attachShader(program_, vertex_);
    if (!noGlError(diagnostic)) return fail();
    gl_.attachShader(program_, fragment_);
    if (!noGlError(diagnostic)) return fail();
    gl_.bindAttribLocation(program_, vehicleGlPositionAttribute50CZ, "rrPosition");
    if (!noGlError(diagnostic)) return fail();
    gl_.bindAttribLocation(program_, vehicleGlUvAttribute50CZ, "rrUv");
    if (!noGlError(diagnostic)) return fail();
    gl_.bindAttribLocation(program_, vehicleGlColorAttribute50CZ, "rrColor");
    if (!noGlError(diagnostic)) return fail();
    gl_.bindAttribLocation(program_, vehicleGlNormalAttribute50CZ, "rrWorldNormal");
    if (!noGlError(diagnostic)) return fail();
    gl_.bindFragDataLocation(program_, vehicleGlColorOutput50CZ, "viewerOutput");
    if (!noGlError(diagnostic)) return fail();
    gl_.linkProgram(program_);
    if (!noGlError(diagnostic)) return fail();
    std::int32_t linked = 0;
    gl_.getProgramiv(program_, linkStatus, &linked);
    if (!noGlError(diagnostic)) return fail();
    if (!linked) {
        gl_.getProgramInfoLog(program_, static_cast<std::int32_t>(sizeof(diagnostic.log)),
            nullptr, diagnostic.log);
        diagnostic.log[sizeof(diagnostic.log) - 1] = '\0';
        noGlError(diagnostic);
        return fail();
    }
    diagnostic.stage = VehicleGlShaderStage50CZ::uniforms;
    uniforms_.worldViewProjection = gl_.getUniformLocation(program_, "worldViewProjection");
    if (!noGlError(diagnostic) || uniforms_.worldViewProjection < 0) return fail();
    uniforms_.useTexture = gl_.getUniformLocation(program_, "useTexture");
    if (!noGlError(diagnostic) || uniforms_.useTexture < 0) return fail();
    uniforms_.image = gl_.getUniformLocation(program_, "image");
    if (!noGlError(diagnostic) || uniforms_.image < 0) return fail();
    uniforms_.effectMode = gl_.getUniformLocation(program_, "effectMode");
    uniforms_.alphaScale = gl_.getUniformLocation(program_, "alphaScale");
    uniforms_.suppressDraw = gl_.getUniformLocation(program_, "suppressDraw");
    uniforms_.useUvTransform = gl_.getUniformLocation(program_, "useUvTransform");
    uniforms_.uvScaleOffset = gl_.getUniformLocation(program_, "uvScaleOffset");
    uniforms_.useLighting = gl_.getUniformLocation(program_, "useLighting");
    if (!noGlError(diagnostic) || uniforms_.effectMode < 0 || uniforms_.alphaScale < 0 ||
        uniforms_.suppressDraw < 0 || uniforms_.useUvTransform < 0 || uniforms_.uvScaleOffset < 0 ||
        uniforms_.useLighting < 0)
        return fail();
    ready_ = true;
    diagnostic.stage = VehicleGlShaderStage50CZ::none;
    return true;
}

bool VehicleGlShader50CZ::release(VehicleGlShaderDiagnostic50CZ& diagnostic) noexcept {
    diagnostic = {};
    ready_ = false;
    uniforms_ = {};
    if (!hasResources()) return true;
    diagnostic.stage = VehicleGlShaderStage50CZ::entryError;
    if (!noGlError(diagnostic)) {
        diagnostic.cleanupIncomplete = true;
        return false;
    }
    diagnostic.stage = VehicleGlShaderStage50CZ::cleanup;
    cleanup(diagnostic);
    if (diagnostic.cleanupIncomplete) return false;
    diagnostic.stage = VehicleGlShaderStage50CZ::none;
    return true;
}
}
