#include "vehicle_offscreen_gl50dd.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <new>
#include <utility>

namespace rr::platform {
namespace {
using U = std::uint32_t;
using I = std::int32_t;
constexpr U arrayBuffer = 0x8892, elementBuffer = 0x8893, pixelPackBuffer = 0x88EB;
constexpr U bufferSize = 0x8764, bufferUsage = 0x8765, dynamicDraw = 0x88E8, staticDraw = 0x88E4;
constexpr U framebuffer = 0x8D40, readFramebuffer = 0x8CA8, framebufferComplete = 0x8CD5;
constexpr U renderbuffer = 0x8D41, colorAttachment = 0x8CE0, depthAttachment = 0x8D00;
constexpr U rgba8 = 0x8058, depth24 = 0x81A6, rgba = 0x1908, unsignedByte = 0x1401;
constexpr U floatType = 0x1406;
constexpr game::VehicleDrawComponent50CM components[] = {
    game::VehicleDrawComponent50CM::chariot, game::VehicleDrawComponent50CM::leftEngine,
    game::VehicleDrawComponent50CM::rightEngine
};
bool fail(VehicleOffscreenDiagnostic50DD& d, VehicleOffscreenError50DD error,
    const char* operation) noexcept {
    if (d.error == VehicleOffscreenError50DD::none) { d.error = error; d.operation = operation; }
    return false;
}
U componentIndex(game::VehicleDrawComponent50CM component) noexcept {
    for (U c = 0; c < 3; ++c) if (components[c] == component) return c;
    return 3;
}

// Only the functions needed by this diagnostic owner. No GL SDK or global
// loader state; every entry is resolved against the private current context.
struct OwnerGl {
    void (*genBuffers)(I, U*) = nullptr;
    void (*deleteBuffers)(I, const U*) = nullptr;
    void (*bufferData)(U, std::intptr_t, const void*, U) = nullptr;
    void (*getBufferParameteri64v)(U, U, std::int64_t*) = nullptr;
    void (*getBufferParameteriv)(U, U, I*) = nullptr;
    void (*getBufferSubData)(U, std::intptr_t, std::intptr_t, void*) = nullptr;
    void (*genVertexArrays)(I, U*) = nullptr;
    void (*deleteVertexArrays)(I, const U*) = nullptr;
    void (*enableVertexAttribArray)(U) = nullptr;
    void (*vertexAttribPointer)(U, I, U, std::uint8_t, I, const void*) = nullptr;
    void (*getVertexAttribiv)(U, U, I*) = nullptr;
    void (*getVertexAttribPointerv)(U, U, void**) = nullptr;
    void (*genFramebuffers)(I, U*) = nullptr;
    void (*deleteFramebuffers)(I, const U*) = nullptr;
    void (*bindFramebuffer)(U, U) = nullptr;
    U (*checkFramebufferStatus)(U) = nullptr;
    void (*framebufferRenderbuffer)(U, U, U, U) = nullptr;
    void (*genRenderbuffers)(I, U*) = nullptr;
    void (*deleteRenderbuffers)(I, const U*) = nullptr;
    void (*bindRenderbuffer)(U, U) = nullptr;
    void (*renderbufferStorage)(U, U, I, I) = nullptr;
    void (*getRenderbufferParameteriv)(U, U, I*) = nullptr;
    void (*getIntegerv)(U, I*) = nullptr;
    void (*useProgram)(U) = nullptr;
    void (*uniformMatrix4fv)(I, I, std::uint8_t, const float*) = nullptr;
    void (*uniform1i)(I, I) = nullptr;
    void (*viewport)(I, I, I, I) = nullptr;
    void (*enable)(U) = nullptr;
    void (*disable)(U) = nullptr;
    void (*depthFunc)(U) = nullptr;
    void (*depthMask)(std::uint8_t) = nullptr;
    void (*depthRange)(double, double) = nullptr;
    void (*clearDepth)(double) = nullptr;
    void (*colorMask)(std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t) = nullptr;
    void (*polygonMode)(U, U) = nullptr;
    void (*clearColor)(float, float, float, float) = nullptr;
    void (*clear)(U) = nullptr;
    void (*drawBuffer)(U) = nullptr;
    void (*readBuffer)(U) = nullptr;
    void (*pixelStorei)(U, I) = nullptr;
    void (*readPixels)(I, I, I, I, U, U, void*) = nullptr;
};
}

struct VehicleOffscreenGl50DD::Storage {
    OffscreenGlContext50DC context;
    VehicleGlFunctions50CV drawGl{};
    VehicleGlShaderFunctions50CZ shaderGl{};
    OwnerGl gl{};
    VehicleGlResources50CV resources{};
    std::unique_ptr<VehicleGlShader50CZ> shader;
    std::unique_ptr<VehicleGlBackend50CV> adapter;
    const std::thread::id thread = std::this_thread::get_id();
    const game::VehicleDrawMesh50CM* meshes[3]{};
    const assets::Scene48D* scenes[3]{};
    U targets[2]{}, colors[2]{}, depths[2]{};
    I width = 0, height = 0;
    std::size_t imageBytes = 0;
    float matrix[16]{}, clearRgba[4]{};
    VehicleOffscreenDiagnostic50DD* diagnostic = nullptr; // Synchronous call only.
    std::uint8_t published = 0;
    bool initialized = false, publishedImage = false, active = false, frameSet = false;

    ~Storage() { VehicleOffscreenDiagnostic50DD d{}; cleanup(d); }

    bool check(const char* operation) noexcept {
        const U error = drawGl.getError();
        if (!error) return true;
        if (!diagnostic->glError) diagnostic->glError = error;
        return fail(*diagnostic, VehicleOffscreenError50DD::gl, operation);
    }
    template<class F> bool load(F& pointer, const char* name) noexcept {
        pointer = reinterpret_cast<F>(context.procAddress(name));
        return pointer || fail(*diagnostic, VehicleOffscreenError50DD::entryPoint, name);
    }
    bool loadFunctions() noexcept {
#define LOAD(group, member, suffix) if (!load(group.member, "gl" #suffix)) return false
        LOAD(drawGl, bindBuffer, BindBuffer); LOAD(drawGl, bufferSubData, BufferSubData);
        LOAD(drawGl, bindVertexArray, BindVertexArray);
        LOAD(drawGl, drawElementsBaseVertex, DrawElementsBaseVertex);
        LOAD(drawGl, isBuffer, IsBuffer); LOAD(drawGl, isVertexArray, IsVertexArray);
        LOAD(drawGl, getError, GetError);
        LOAD(shaderGl, createShader, CreateShader); LOAD(shaderGl, shaderSource, ShaderSource);
        LOAD(shaderGl, compileShader, CompileShader); LOAD(shaderGl, getShaderiv, GetShaderiv);
        LOAD(shaderGl, getShaderInfoLog, GetShaderInfoLog); LOAD(shaderGl, deleteShader, DeleteShader);
        LOAD(shaderGl, createProgram, CreateProgram); LOAD(shaderGl, attachShader, AttachShader);
        LOAD(shaderGl, bindAttribLocation, BindAttribLocation);
        LOAD(shaderGl, bindFragDataLocation, BindFragDataLocation);
        LOAD(shaderGl, linkProgram, LinkProgram); LOAD(shaderGl, getProgramiv, GetProgramiv);
        LOAD(shaderGl, getProgramInfoLog, GetProgramInfoLog);
        LOAD(shaderGl, getUniformLocation, GetUniformLocation);
        LOAD(shaderGl, deleteProgram, DeleteProgram); shaderGl.getError = drawGl.getError;
        LOAD(gl, genBuffers, GenBuffers); LOAD(gl, deleteBuffers, DeleteBuffers);
        LOAD(gl, bufferData, BufferData); LOAD(gl, getBufferParameteri64v, GetBufferParameteri64v);
        LOAD(gl, getBufferParameteriv, GetBufferParameteriv); LOAD(gl, getBufferSubData, GetBufferSubData);
        LOAD(gl, genVertexArrays, GenVertexArrays); LOAD(gl, deleteVertexArrays, DeleteVertexArrays);
        LOAD(gl, enableVertexAttribArray, EnableVertexAttribArray);
        LOAD(gl, vertexAttribPointer, VertexAttribPointer); LOAD(gl, getVertexAttribiv, GetVertexAttribiv);
        LOAD(gl, getVertexAttribPointerv, GetVertexAttribPointerv);
        LOAD(gl, genFramebuffers, GenFramebuffers); LOAD(gl, deleteFramebuffers, DeleteFramebuffers);
        LOAD(gl, bindFramebuffer, BindFramebuffer); LOAD(gl, checkFramebufferStatus, CheckFramebufferStatus);
        LOAD(gl, framebufferRenderbuffer, FramebufferRenderbuffer);
        LOAD(gl, genRenderbuffers, GenRenderbuffers); LOAD(gl, deleteRenderbuffers, DeleteRenderbuffers);
        LOAD(gl, bindRenderbuffer, BindRenderbuffer); LOAD(gl, renderbufferStorage, RenderbufferStorage);
        LOAD(gl, getRenderbufferParameteriv, GetRenderbufferParameteriv);
        LOAD(gl, getIntegerv, GetIntegerv); LOAD(gl, useProgram, UseProgram);
        LOAD(gl, uniformMatrix4fv, UniformMatrix4fv); LOAD(gl, uniform1i, Uniform1i);
        LOAD(gl, viewport, Viewport); LOAD(gl, enable, Enable); LOAD(gl, disable, Disable);
        LOAD(gl, depthFunc, DepthFunc); LOAD(gl, depthMask, DepthMask);
        LOAD(gl, depthRange, DepthRange); LOAD(gl, clearDepth, ClearDepth);
        LOAD(gl, colorMask, ColorMask); LOAD(gl, polygonMode, PolygonMode);
        LOAD(gl, clearColor, ClearColor); LOAD(gl, clear, Clear);
        LOAD(gl, drawBuffer, DrawBuffer); LOAD(gl, readBuffer, ReadBuffer);
        LOAD(gl, pixelStorei, PixelStorei); LOAD(gl, readPixels, ReadPixels);
#undef LOAD
        return true;
    }
    bool verifyBuffer(U target, std::size_t bytes, U usage) noexcept {
        std::int64_t actualBytes = 0;
        I actualUsage = 0;
        gl.getBufferParameteri64v(target, bufferSize, &actualBytes);
        gl.getBufferParameteriv(target, bufferUsage, &actualUsage);
        if (!check("verify buffer allocation")) return false;
        return (actualBytes == static_cast<std::int64_t>(bytes) && actualUsage == static_cast<I>(usage)) ||
            fail(*diagnostic, VehicleOffscreenError50DD::allocation, "buffer size/usage mismatch");
    }
    bool verifyLayout(U vertexBuffer, U indices) noexcept {
        constexpr I sizes[] = {3, 2, 4};
        constexpr U types[] = {floatType, floatType, unsignedByte};
        constexpr std::uintptr_t offsets[] = {0, 12, 20};
        constexpr U properties[] = {0x8622, 0x8623, 0x8624, 0x8625, 0x886A, 0x889F, 0x88FD};
        // enabled, size, stride, type, normalized, VBO association, integer.
        for (U a = 0; a < 3; ++a) {
            const I expected[] = {1, sizes[a], 24, static_cast<I>(types[a]), a == 2 ? 1 : 0,
                static_cast<I>(vertexBuffer), 0};
            for (U p = 0; p < 7; ++p) {
                I value = 0;
                gl.getVertexAttribiv(a, properties[p], &value);
                if (!check("glGetVertexAttribiv")) return false;
                if (value != expected[p]) return fail(*diagnostic, VehicleOffscreenError50DD::layout,
                    "vertex attribute layout mismatch");
            }
            void* pointer = nullptr;
            gl.getVertexAttribPointerv(a, 0x8645, &pointer);
            if (!check("glGetVertexAttribPointerv")) return false;
            if (reinterpret_cast<std::uintptr_t>(pointer) != offsets[a])
                return fail(*diagnostic, VehicleOffscreenError50DD::layout, "vertex attribute offset mismatch");
        }
        I element = 0;
        gl.getIntegerv(0x8895, &element);
        if (!check("verify VAO element buffer")) return false;
        return static_cast<U>(element) == indices ||
            fail(*diagnostic, VehicleOffscreenError50DD::layout, "VAO element association mismatch");
    }
    bool createGeometry() noexcept {
        for (U c = 0; c < 3; ++c) {
            diagnostic->component = c;
            const auto& mesh = *meshes[c];
            auto& index = resources.indices[c];
            index.data = mesh.indices; index.count = mesh.indexCount;
            const std::size_t indexBytes = std::size_t(index.count) * sizeof(U);
            gl.genBuffers(1, &index.buffer);
            // ARRAY_BUFFER can initialize an EBO without changing VAO state.
            drawGl.bindBuffer(arrayBuffer, index.buffer);
            gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(indexBytes), index.data, staticDraw);
            if (!check("create immutable index buffer")) return false;
            if (!index.buffer) return fail(*diagnostic, VehicleOffscreenError50DD::allocation, "zero index buffer name");
            if (!verifyBuffer(arrayBuffer, indexBytes, staticDraw)) return false;
            // Cold, exact GPU-side contents check; only 1 KiB stack scratch.
            U words[256];
            for (U start = 0; start < index.count;) {
                const U count = std::min<U>(256, index.count - start);
                gl.getBufferSubData(arrayBuffer, std::intptr_t(start) * sizeof(U),
                    std::intptr_t(count) * sizeof(U), words);
                if (!check("verify retained index bytes")) return false;
                if (std::memcmp(words, index.data + start, std::size_t(count) * sizeof(U)))
                    return fail(*diagnostic, VehicleOffscreenError50DD::indices, "GPU index contents mismatch");
                start += count;
            }
            for (auto& bank : resources.bank) {
                bank.vertexCapacity[c] = mesh.vertexCount;
                gl.genBuffers(1, &bank.vertexBuffer[c]);
                drawGl.bindBuffer(arrayBuffer, bank.vertexBuffer[c]);
                const std::size_t bytes = std::size_t(mesh.vertexCount) * sizeof(assets::Vertex48B);
                gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bytes), nullptr, dynamicDraw);
                if (!check("allocate inactive vertex bank")) return false;
                if (!bank.vertexBuffer[c]) return fail(*diagnostic, VehicleOffscreenError50DD::allocation, "zero vertex buffer name");
                if (!verifyBuffer(arrayBuffer, bytes, dynamicDraw)) return false;
                gl.genVertexArrays(1, &bank.vertexArray[c]);
                drawGl.bindVertexArray(bank.vertexArray[c]);
                drawGl.bindBuffer(elementBuffer, index.buffer);
                constexpr I sizes[] = {3, 2, 4};
                constexpr std::uintptr_t offsets[] = {0, 12, 20};
                for (U a = 0; a < 3; ++a) {
                    gl.enableVertexAttribArray(a);
                    gl.vertexAttribPointer(a, sizes[a], a == 2 ? unsignedByte : floatType,
                        a == 2 ? 1 : 0, 24, reinterpret_cast<const void*>(offsets[a]));
                }
                if (!check("create Vertex48B VAO")) return false;
                if (!bank.vertexArray[c]) return fail(*diagnostic, VehicleOffscreenError50DD::allocation, "zero vertex array name");
                if (!verifyLayout(bank.vertexBuffer[c], index.buffer)) return false;
            }
        }
        diagnostic->component = 3;
        return true;
    }
    bool createAttachment(U& name, U format, U attachment) noexcept {
        gl.genRenderbuffers(1, &name);
        gl.bindRenderbuffer(renderbuffer, name);
        gl.renderbufferStorage(renderbuffer, format, width, height);
        if (!check("allocate offscreen renderbuffer")) return false;
        if (!name) return fail(*diagnostic, VehicleOffscreenError50DD::allocation, "zero renderbuffer name");
        I actualWidth = 0, actualHeight = 0, actualFormat = 0, actualSamples = 0;
        gl.getRenderbufferParameteriv(renderbuffer, 0x8D42, &actualWidth);
        gl.getRenderbufferParameteriv(renderbuffer, 0x8D43, &actualHeight);
        gl.getRenderbufferParameteriv(renderbuffer, 0x8D44, &actualFormat);
        gl.getRenderbufferParameteriv(renderbuffer, 0x8CAB, &actualSamples);
        if (!check("verify offscreen renderbuffer")) return false;
        if (actualWidth != width || actualHeight != height || actualFormat != static_cast<I>(format) || actualSamples)
            return fail(*diagnostic, VehicleOffscreenError50DD::framebuffer, "renderbuffer storage mismatch");
        gl.framebufferRenderbuffer(framebuffer, attachment, renderbuffer, name);
        return check("attach offscreen renderbuffer");
    }
    bool createTargets() noexcept {
        I maximum = 0, viewportLimits[2]{};
        gl.getIntegerv(0x84E8, &maximum); // GL_MAX_RENDERBUFFER_SIZE
        gl.getIntegerv(0x0D3A, viewportLimits); // GL_MAX_VIEWPORT_DIMS
        if (!check("query target limits")) return false;
        if (width > maximum || height > maximum || width > viewportLimits[0] || height > viewportLimits[1])
            return fail(*diagnostic, VehicleOffscreenError50DD::limits, "explicit resolution exceeds GL limits");
        for (U b = 0; b < 2; ++b) {
            gl.genFramebuffers(1, &targets[b]);
            gl.bindFramebuffer(framebuffer, targets[b]);
            if (!check("create unpublished framebuffer")) return false;
            if (!targets[b]) return fail(*diagnostic, VehicleOffscreenError50DD::allocation, "zero framebuffer name");
            if (!createAttachment(colors[b], rgba8, colorAttachment) ||
                !createAttachment(depths[b], depth24, depthAttachment)) return false;
            gl.drawBuffer(colorAttachment); gl.readBuffer(colorAttachment);
            const U status = gl.checkFramebufferStatus(framebuffer);
            if (!check("check unpublished framebuffer")) return false;
            if (status != framebufferComplete) {
                diagnostic->framebufferStatus = status;
                return fail(*diagnostic, VehicleOffscreenError50DD::framebuffer, "incomplete unpublished framebuffer");
            }
        }
        return true;
    }
    bool initialize(const VehicleOffscreenRequest50DD& request,
        const assets::VehicleDrawAssets50CR& owner) noexcept {
        width = request.context.width; height = request.context.height;
        imageBytes = std::size_t(width) * std::size_t(height) * 4;
        for (U c = 0; c < 3; ++c) {
            diagnostic->component = c;
            meshes[c] = owner.preparedMesh(components[c]); scenes[c] = owner.scene(components[c]);
            if (!meshes[c] || !scenes[c] || !meshes[c]->indices || !meshes[c]->spans ||
                !meshes[c]->vertexCount || !meshes[c]->indexCount || !meshes[c]->spanCount ||
                meshes[c]->vertexCount > game::maxRigidVertices49S ||
                meshes[c]->indexCount > assets::max_mesh_indices ||
                meshes[c]->spanCount > game::maxVehicleDrawSpans50CM)
                return fail(*diagnostic, VehicleOffscreenError50DD::assets, "invalid prepared 50CR mesh");
            for (U s = 0; s < meshes[c]->spanCount; ++s)
                if (meshes[c]->spans[s].material_index >= scenes[c]->materials.size())
                    return fail(*diagnostic, VehicleOffscreenError50DD::assets, "original material index outside loaded scene");
        }
        diagnostic->component = 3;
        if (!context.create(request.context, diagnostic->context))
            return fail(*diagnostic, VehicleOffscreenError50DD::context, "create private 50DC context");
        if (!loadFunctions() || !check("initial GL error state")) return false;
        shader.reset(new (std::nothrow) VehicleGlShader50CZ(shaderGl));
        if (!shader) return fail(*diagnostic, VehicleOffscreenError50DD::allocation, "allocate shader owner");
        if (!shader->initialize(diagnostic->shader))
            return fail(*diagnostic, VehicleOffscreenError50DD::shader, "initialize 50CZ viewer shader");
        if (!createGeometry() || !createTargets()) return false;
        VehicleGlHooks50CV hooks{};
        hooks.context = this; hooks.ready = ready; hooks.materialReady = materialReady;
        hooks.begin = begin; hooks.bindMaterial = bindMaterial; hooks.commit = commit; hooks.abort = abort;
        adapter.reset(new (std::nothrow) VehicleGlBackend50CV(drawGl, resources, hooks, published));
        if (!adapter) return fail(*diagnostic, VehicleOffscreenError50DD::allocation, "allocate 50CV adapter");
        initialized = true;
        return true;
    }
    static bool ready(void* context, const VehicleGlResources50CV& resources, std::uint8_t bank,
        const game::VehicleDrawBatch50CO& batch, const game::VehicleDrawPlan50CS&) noexcept {
        auto& s = *static_cast<Storage*>(context);
        // Context identity before even glGetError. Private, unshared ownership
        // preserves the verified sizes/layouts/index bytes between submissions.
        if (!s.context.current()) return fail(*s.diagnostic, VehicleOffscreenError50DD::notCurrent, "private context not current");
        if (!s.initialized || s.active || !s.frameSet || bank != (s.published ^ 1u) ||
            !s.shader->program() || !s.targets[bank]) return false;
        for (U c = 0; c < 3; ++c) {
            const auto& mesh = *s.meshes[c];
            const auto& b = batch.component[c];
            if (b.indices != mesh.indices || b.indexCount != mesh.indexCount ||
                b.spans != mesh.spans || b.spanCount != mesh.spanCount || b.vertexCount != mesh.vertexCount ||
                resources.indices[c].buffer != s.resources.indices[c].buffer)
                return fail(*s.diagnostic, VehicleOffscreenError50DD::assets, "batch is not the retained prepared mesh");
        }
        return s.check("before vehicle preflight");
    }
    static bool materialReady(void* context, game::VehicleDrawComponent50CM component, U material) noexcept {
        auto& s = *static_cast<Storage*>(context);
        const U c = componentIndex(component);
        if (c >= 3 || material >= s.scenes[c]->materials.size())
            return fail(*s.diagnostic, VehicleOffscreenError50DD::assets, "material is outside loaded component scene");
        return true;
    }
    static bool begin(void* context, std::uint8_t bank) noexcept {
        auto& s = *static_cast<Storage*>(context);
        s.active = true;
        // This sole context orders target reuse and buffer updates after their
        // prior consumers, including work from an aborted submission. No mapping,
        // unsynchronized uploads or other-context access bypass GL dependencies.
        // Do not add an unconditional finish: acceptance is not GPU completion.
        s.gl.bindFramebuffer(framebuffer, s.targets[bank]);
        s.gl.drawBuffer(colorAttachment); s.gl.readBuffer(colorAttachment);
        s.gl.viewport(0, 0, s.width, s.height);
        // Existing inspection viewer: solid, two-sided, opaque, depth LESS.
        // Disable desktop GL features that could otherwise alter these pixels.
        constexpr U disabled[] = {0x0BE2, 0x0B44, 0x0C11, 0x0B90, 0x8037, 0x0B20,
            0x8F9D, 0x0BD0, 0x809D, 0x80A0, 0x809E, 0x8C89, 0x8DB9, 0x864F};
        for (U capability : disabled) s.gl.disable(capability);
        s.gl.enable(0x0B71); s.gl.depthFunc(0x0201); s.gl.depthMask(1);
        s.gl.depthRange(0.0, 1.0); s.gl.clearDepth(1.0);
        s.gl.colorMask(1, 1, 1, 1); s.gl.polygonMode(0x0408, 0x1B02);
        s.gl.useProgram(s.shader->program());
        const auto& uniforms = s.shader->uniforms();
        s.gl.uniformMatrix4fv(uniforms.worldViewProjection, 1, 0, s.matrix);
        s.gl.uniform1i(uniforms.useTexture, 0); s.gl.uniform1i(uniforms.image, 0);
        s.gl.clearColor(s.clearRgba[0], s.clearRgba[1], s.clearRgba[2], s.clearRgba[3]);
        s.gl.clear(0x00004000 | 0x00000100);
        return s.check("begin explicit neutral frame");
    }
    static bool bindMaterial(void* context, game::VehicleDrawComponent50CM component, U material) noexcept {
        if (!materialReady(context, component, material)) return false;
        auto& s = *static_cast<Storage*>(context);
        s.gl.uniform1i(s.shader->uniforms().useTexture, 0);
        return s.check("bind explicit viewer neutral mode");
    }
    static bool commit(void* context, std::uint8_t bank) noexcept {
        auto& s = *static_cast<Storage*>(context);
        if (!s.active || bank != (s.published ^ 1u)) return false;
        if (!s.check("accept unpublished vehicle frame")) return false;
        // CPU-only publication after the last fallible call. No swaps, copies,
        // GL error queries or user callbacks may be added after this point.
        s.published = bank; s.publishedImage = true; s.active = false;
        return true;
    }
    static void abort(void* context, std::uint8_t) noexcept {
        auto& s = *static_cast<Storage*>(context);
        // Leave the abandoned target private. No GL call is needed to preserve
        // the other target. Same-context ordering protects subsequent reuse.
        s.active = false;
    }
    bool cleanup(VehicleOffscreenDiagnostic50DD& d) noexcept {
        if (thread != std::this_thread::get_id()) {
            d.cleanupContext.error = OffscreenGlError50DC::WrongThread;
            d.cleanupContext.operation = "50DD cleanup"; d.cleanupIncomplete = true;
            return fail(d, VehicleOffscreenError50DD::cleanup, "cleanup requires creating thread");
        }
        initialized = false; active = frameSet = false;
        adapter.reset();
        if (context.created()) {
            OffscreenGlDiagnostic50DC bindDiagnostic{};
            if (!context.current() && !context.bind(bindDiagnostic)) {
                d.cleanupContext = bindDiagnostic; d.cleanupIncomplete = true;
                return fail(d, VehicleOffscreenError50DD::cleanup, "bind context for GL cleanup");
            }
            // Drain stale submission errors before deleting. Context destruction
            // below is the final owner of every GL object, including deletions
            // that fail on a lost context. No function is used before loading.
            bool clean = true;
            if (drawGl.getError) {
                clean = false;
                for (unsigned i = 0; i < 32; ++i) {
                    const U error = drawGl.getError();
                    if (!error) { clean = true; break; }
                    if (!d.cleanupGlError) d.cleanupGlError = error;
                }
            }
            if (clean) {
                if (gl.useProgram) gl.useProgram(0);
                if (drawGl.bindVertexArray) drawGl.bindVertexArray(0);
                if (gl.bindFramebuffer) gl.bindFramebuffer(framebuffer, 0);
                if (shader) {
                    VehicleGlShaderDiagnostic50CZ releaseDiagnostic{};
                    if (!shader->release(releaseDiagnostic) && !d.cleanupGlError)
                        d.cleanupGlError = releaseDiagnostic.glError ? releaseDiagnostic.glError : releaseDiagnostic.cleanupGlError;
                }
                for (const auto& bank : resources.bank) {
                    if (gl.deleteVertexArrays) gl.deleteVertexArrays(3, bank.vertexArray);
                    if (gl.deleteBuffers) gl.deleteBuffers(3, bank.vertexBuffer);
                }
                for (const auto& index : resources.indices)
                    if (gl.deleteBuffers) gl.deleteBuffers(1, &index.buffer);
                if (gl.deleteFramebuffers) gl.deleteFramebuffers(2, targets);
                if (gl.deleteRenderbuffers) {
                    gl.deleteRenderbuffers(2, colors); gl.deleteRenderbuffers(2, depths);
                }
                if (drawGl.getError) {
                    const U error = drawGl.getError();
                    if (!d.cleanupGlError) d.cleanupGlError = error;
                }
            }
        }
        OffscreenGlDiagnostic50DC closeDiagnostic{};
        if (!context.close(closeDiagnostic)) {
            d.cleanupContext = closeDiagnostic; d.cleanupIncomplete = true;
            return fail(d, VehicleOffscreenError50DD::cleanup, "close private context");
        }
        // No share group survives context close, so even unresolved shader or
        // GL deletion names are now destroyed. Never call GL through them again.
        shader.reset(); resources = {}; drawGl = {}; shaderGl = {}; gl = {};
        for (U b = 0; b < 2; ++b) targets[b] = colors[b] = depths[b] = 0;
        publishedImage = false; imageBytes = 0; d.cleanupIncomplete = false;
        if (d.cleanupGlError) return fail(d, VehicleOffscreenError50DD::cleanup, "GL cleanup reported an error; context closed");
        return true;
    }
};

VehicleOffscreenGl50DD::VehicleOffscreenGl50DD() noexcept = default;
VehicleOffscreenGl50DD::~VehicleOffscreenGl50DD() = default;

bool VehicleOffscreenGl50DD::create(const VehicleOffscreenRequest50DD& request,
    const assets::VehicleDrawAssets50CR& assets, VehicleOffscreenDiagnostic50DD& d) noexcept {
    d = {};
    if (storage_) return fail(d, VehicleOffscreenError50DD::alreadyOwned, "renderer already owns resources");
    if (request.mode != VehicleOffscreenMode50DD::viewerNeutralUntextured ||
        request.context.width <= 0 || request.context.height <= 0 ||
        std::uint64_t(request.context.width) * std::uint64_t(request.context.height) >
            std::numeric_limits<std::size_t>::max() / 4)
        return fail(d, VehicleOffscreenError50DD::request, "explicit neutral mode and valid resolution required");
    if (!assets.loaded()) return fail(d, VehicleOffscreenError50DD::assets, "50CR assets are not loaded");
    std::unique_ptr<Storage> next(new (std::nothrow) Storage);
    if (!next) return fail(d, VehicleOffscreenError50DD::allocation, "allocate offscreen owner");
    next->diagnostic = &d;
    const bool initialized = next->initialize(request, assets);
    next->diagnostic = nullptr;
    if (!initialized) {
        next->cleanup(d);
        if (next->context.ownsResources()) storage_ = std::move(next);
        return false;
    }
    storage_ = std::move(next);
    return true;
}

bool VehicleOffscreenGl50DD::submit(const game::VehicleDrawBatch50CO& batch,
    const VehicleOffscreenFrame50DD& frame, VehicleOffscreenDiagnostic50DD& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleOffscreenError50DD::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if (!s.context.current()) return fail(d, VehicleOffscreenError50DD::notCurrent, "private context not current");
    if (!frame.worldViewProjection || !frame.clearRgba)
        return fail(d, VehicleOffscreenError50DD::frame, "explicit matrix and clear color required");
    for (U i = 0; i < 16; ++i)
        if (!std::isfinite(frame.worldViewProjection[i]))
            return fail(d, VehicleOffscreenError50DD::frame, "nonfinite world-to-clip matrix");
    for (U i = 0; i < 4; ++i)
        if (!std::isfinite(frame.clearRgba[i]) || frame.clearRgba[i] < 0 || frame.clearRgba[i] > 1)
            return fail(d, VehicleOffscreenError50DD::frame, "clear color outside finite [0,1]");
    std::memcpy(s.matrix, frame.worldViewProjection, sizeof(s.matrix));
    std::memcpy(s.clearRgba, frame.clearRgba, sizeof(s.clearRgba));
    s.frameSet = true; s.diagnostic = &d;
    const bool accepted = game::submitVehicleDrawBatch50CS(batch, s.adapter->backend(), d.submission);
    if (!accepted) {
        // Structural 50CS rejection occurs before adapter preflight resets its
        // error. Do not attribute a previous call's GL error to this rejection.
        if (!d.glError && d.submission.error >= game::VehicleDrawSubmitError50CS::backendPreflight)
            d.glError = s.adapter->glError();
        fail(d, VehicleOffscreenError50DD::submission, "50CS/50CV submission rejected");
    }
    s.frameSet = false; s.diagnostic = nullptr;
    return accepted;
}

bool VehicleOffscreenGl50DD::readRgba(std::uint8_t* destination, std::size_t capacity,
    VehicleOffscreenDiagnostic50DD& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleOffscreenError50DD::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if (!s.context.current()) return fail(d, VehicleOffscreenError50DD::notCurrent, "private context not current");
    if (!s.publishedImage) return fail(d, VehicleOffscreenError50DD::noImage, "no committed vehicle frame");
    if (!destination || capacity < s.imageBytes)
        return fail(d, VehicleOffscreenError50DD::readback, "RGBA destination too small or null");
    s.diagnostic = &d;
    bool result = s.check("before RGBA readback");
    if (result) {
        s.gl.bindFramebuffer(readFramebuffer, s.targets[s.published]);
        s.gl.readBuffer(colorAttachment);
        s.drawGl.bindBuffer(pixelPackBuffer, 0);
        s.gl.pixelStorei(0x0D05, 1); // PACK_ALIGNMENT
        s.gl.pixelStorei(0x0D02, 0); // PACK_ROW_LENGTH
        s.gl.pixelStorei(0x0D03, 0); // PACK_SKIP_ROWS
        s.gl.pixelStorei(0x0D04, 0); // PACK_SKIP_PIXELS
        s.gl.pixelStorei(0x0D00, 0); // PACK_SWAP_BYTES
        s.gl.pixelStorei(0x0D01, 0); // PACK_LSB_FIRST
        result = s.check("configure RGBA readback");
        if (result) {
            s.gl.readPixels(0, 0, s.width, s.height, rgba, unsignedByte, destination);
            result = s.check("glReadPixels committed RGBA");
        }
    }
    s.diagnostic = nullptr;
    return result;
}

bool VehicleOffscreenGl50DD::close(VehicleOffscreenDiagnostic50DD& d) noexcept {
    d = {};
    if (!storage_) return true;
    const bool result = storage_->cleanup(d);
    if (!d.cleanupIncomplete) storage_.reset();
    return result;
}
bool VehicleOffscreenGl50DD::initialized() const noexcept { return storage_ && storage_->initialized; }
bool VehicleOffscreenGl50DD::hasImage() const noexcept { return initialized() && storage_->publishedImage; }
std::size_t VehicleOffscreenGl50DD::rgbaBytes() const noexcept { return initialized() ? storage_->imageBytes : 0; }
}
