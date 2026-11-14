#pragma once
// One water surface owner for every course whose SCB Environment.WaterFile is
// not `none` (today GC and MC). It consumes the shared water runtime both
// course loaders already build (native_water_runtime50JC::Owner) and never
// branches on a course name. Maths: src/game/original_water_surface50ws.h.
// Recovery and open items: work/scratch/audit-render/water/PLAN.md.
#include "native_water_runtime50jc_fixture.h"
#include "runtime_startup50fl_fixture.h"
#include "../../src/game/original_water_surface50ws.h"
#include "../../src/game/ai_cold_owners50iu.h"
#include "../../src/game/camera_view49f.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/material_texture_assets50ea.h"
#include "../../platform/linux/native/material_texture_gl50eb.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace native_water_surface50WS {
using namespace rr::game;

// Height offset at (x, z) as the GS sampling pass would produce it: wake
// (2B1110..2B11E0 bucket read) plus the signature channels 0..3 with their
// frame lerps and ChannelScales (2B72B0), times 1/4096 (heightScaleC0). No
// base-displacement noise, swell or pool terms: the VU program has none.
// Froth layer bytes (sampling rectangle 2, layer2 .g/.b): the original chain,
// traced statically (PLAN.md section 7):
//  - targets: two 128x128 8888 render targets at provider+0x59C[0..1],
//    created at 2B2718..2B27F0 when renderFoam 383418 != 0; ping-pong index
//    provider+0x5C0 (set at 2B4CCC..2B4D00; target count provider+0x5A4).
//  - update 2B75A0 (from 2B4EE0, every frame): pass 1 redraws the previous
//    target shifted by the lattice-origin change (443CA0[i] - origin) with
//    colour 443D60 and the A+D word 0x82_00000068 (2B7650; ALPHA Cs*FIX/128
//    + Cd with FIX 0x82 if that word is ALPHA_1) (to 2B7B8C); pass 2 stamps one
//    sprite per entry of the splat list provider+0x490 (20-byte x, z, size)
//    within +-64 of the origin (2B7E74..2B8060), count provider+0x498.
//  - sampled: 2B5238..2B5568 draws the water plane into rectangle 2 with
//    the current target (2B3DC0); the VU reads it as frothA/frothB.
// No instruction in the ELF ever adds to the splat list: provider+0x498 is
// only cleared (2B2144 at setup, 2B807C after the last ping-pong frame) and
// MinForce 383500, FrothScale 443E20 and FrothStartSize 443E28 are written
// by the WTT readers (2B97DC/2B9840/2B9860, 2BA3A8/2BA40C/2BA42C) and read
// nowhere. Pass 1 only moves existing content, so the froth layer holds
// whatever the targets held when created: zero here. Pass B is therefore
// alpha-tested away exactly as in the original data flow. PENDING emulator
// check (item 7): whether any froth is visible on GC or MC.
struct Sampler {
    const WaterHeightProvider50JC* provider = nullptr;
    const WaterHeightGlobals50JC* globals = nullptr;
};
inline bool sample(const void* context, float x, float z, WaterSurfaceSample50WS& out) noexcept {
    const auto& c = *static_cast<const Sampler*>(context);
    if (!c.provider || !c.globals || !c.provider->heightScaleC0 || !c.provider->signaturesC4 ||
        !c.globals->wakeScale443B88) return false;
    using namespace water_height_detail50jc;
    const auto& p = *c.provider;
    Math m;
    const float wakeScale = m.read(*c.globals->wakeScale443B88, 0x2b1118u);
    float wake = 0.f;
    for (std::int32_t i = 0; i < p.count4C; ++i) {
        const auto& b = p.buckets40[i];
        if (!b.bound || p.dimensionB0 <= 0 || p.blocksPerRowB4 <= 0 || b.activeBuffer30 < 0 || b.activeBuffer30 > 1)
            return false;
        const auto bx = m.cvtWord(m.mul(b.inverseScale04, m.sub(x, b.originX10, 0x2b1148u), 0x2b1150u), 0x2b1158u);
        const auto bz = m.cvtWord(m.mul(b.inverseScale04, m.sub(z, b.originZ18, 0x2b114cu), 0x2b1154u), 0x2b115cu);
        if (m.badPC) return false;
        if (bx < 0 || bx >= p.dimensionB0 || bz < 0 || bz >= p.dimensionB0) continue;
        const auto group = std::uint64_t(std::uint32_t(bz)) * std::uint32_t(p.blocksPerRowB4) + std::uint32_t(bx / 8);
        const auto offset = group * 64u + std::uint32_t(bx % 8) * 2u;
        const auto& bytes = b.buffers38[b.activeBuffer30];
        if (!at(bytes, offset) || !at(bytes, offset + 1)) return false;
        const std::uint32_t word = std::uint32_t(bytes.data[offset]) | (std::uint32_t(bytes.data[offset + 1]) << 8u);
        const auto s16 = word < 0x8000u ? std::int32_t(word) : std::int32_t(word) - 65536;
        wake = m.add(wake, m.mul(wakeScale, m.integer(s16, 0x2b11d8u), 0x2b11dcu), 0x2b11e0u);
    }
    float signature = 0.f;
    std::uint32_t marker = 0;
    if (signatures(*p.signaturesC4, *c.globals, x, z, signature, marker, m).status != WaterHeightStatus50JC::Complete)
        return false;
    out.height = m.mul(*p.heightScaleC0, m.add(signature, wake, 0x2b12e8u), 0x2b12ecu);
    out.frothA = out.frothB = 0;
    return !m.badPC;
}

struct State {
    WaterRenderAuthoring50WS authoring{};
    WaterGrid50WS grid{};
    std::vector<rr::assets::Vertex48B> vertices;
    std::vector<WaterSurfacePoint50WS> scratch;
    WaterSurfaceCounters50WS counters{};
    // Bank index 0 = environment (TEX0 #1), 1 = froth (TEX0 #2) when present.
    std::vector<std::vector<std::uint8_t>> textureBytes;
    std::unique_ptr<rr::assets::MaterialTextureAssets50EA> assets;
    rr::platform::MaterialTextureGl50EB bank;
    bool cold = false, frothImage = false;
    std::uint64_t frames = 0, drawnFrames = 0, hiddenFrames = 0, failedFrames = 0;
    std::string log;
};
inline State& state() noexcept { static State owner; return owner; }

// Cold: WTT render keys, grid size by the original rule, the material's
// environment image split into TEX0 #1 and #2. Optional: a refusal only
// disables water drawing and is logged.
inline bool loadCold(const std::filesystem::path& trackArchive, const water_resources50JC::Owner& resources,
    const char* trackCode, std::uint32_t humanCount, std::string& log) {
    auto& s = state();
    if (s.bank.ownsResources()) { log = "water surface cold: GPU bank still open"; return false; }
    auto reset = [&s] {
        s.authoring = {}; s.grid = {}; s.vertices.clear(); s.scratch.clear(); s.counters = {};
        s.textureBytes.clear(); s.assets.reset(); s.cold = false; s.frothImage = false;
        s.frames = s.drawnFrames = s.hiddenFrames = s.failedFrames = 0;
    };
    reset();
    try {
        const auto bytes = runtime_startup50fl::readBytes(trackArchive);
        rr::assets::ResArchive48C archive; rr::assets::DecodeError48C error; rr::assets::DecodeLimits48C limits;
        limits.resource_file_bytes = bytes.size(); limits.resource_decoded_bytes = 64u * 1024u * 1024u;
        limits.resource_entries = 4096;
        if (!rr::assets::decodeResource48C(bytes, archive, error, nullptr, limits)) {
            log = "water archive rejected: " + error.code; return false; }
        auto member = [&](const std::string& name, std::vector<std::uint8_t>& out) {
            water_resources50JC::Member identity; std::string e;
            if (!water_resources50JC::member(archive, name, out, identity, e)) { log = e; return false; }
            return true;
        };
        // WTT: same member the physics loader authenticated (Environment.WaterFile).
        std::vector<std::uint8_t> wtt;
        if (!member(resources.waterMember.name, wtt)) return false;
        rr::assets::AiAuthoringDiagnostic50IU diagnostic;
        if (!readWaterRenderAuthoring50WS(reinterpret_cast<const char*>(wtt.data()), wtt.size(), s.authoring, diagnostic)) {
            log = "water render keys rejected pc=" + std::to_string(diagnostic.sourcePC) +
                " offset=" + std::to_string(diagnostic.textOffset);
            return false;
        }
        // Grid: Race+0x8C = 0 in the one-view race; track id from the course
        // descriptor table; human count from the session.
        rr::game::AiFreshCreateSelection50IU selection{};
        if (!trackCode || !rr::game::bindFreshAiCreateSelection50IU(trackCode, 2, selection)) {
            log = "water grid: course descriptor unavailable"; return false; }
        s.grid = selectWaterGrid50WS(0u, selection.selectedTrackId1125F, humanCount);
        if (!s.grid.bound || waterSurfaceVertexCapacity50WS(s.grid) / (2u * waterStripVertices50WS) >
            rr::platform::maxVehicleWindowWaterStrips50DS) { log = "water grid outside renderer capacity"; return false; }
        // Material: textures\<ShaderName>.psm, pass 1 texture 1 (microProgramName = Water).
        std::vector<std::uint8_t> psm, pst;
        if (!member(std::string("textures\\") + s.authoring.shaderName.data() + ".psm", psm)) return false;
        rr::assets::Material48E material;
        if (!rr::assets::decodeMaterial48E(psm, material, error) || material.passes.empty() ||
            material.passes[0].microprogram != "Water" || material.passes[0].textures.empty()) {
            log = "water material rejected: " + error.code; return false; }
        if (!member("textures\\" + material.passes[0].textures[0].name + ".pst", pst)) return false;
        rr::assets::Texture48E texture;
        if (!rr::assets::decodeTexture48E(pst, texture, error) || texture.mips.empty()) {
            log = "water environment texture rejected: " + error.code; return false; }
        // TEX0 #2 is TBP0 + half the allocation (383530 = size/2, 2B9140). For
        // a W x 2W image that is the lower half of the rows. PENDING emulator
        // check W-E1: confirm the GS shows the lower half as the froth image.
        const auto& base = texture.mips.back();
        std::vector<rr::assets::TextureMip48E> halves;
        if (base.height == 2u * base.width) {
            const std::size_t half = std::size_t(base.width) * (base.height / 2u) * 4u;
            for (unsigned h = 0; h < 2; ++h) {
                rr::assets::TextureMip48E mip; mip.width = base.width; mip.height = base.height / 2u;
                mip.rgba.assign(base.rgba.begin() + std::ptrdiff_t(h * half), base.rgba.begin() + std::ptrdiff_t((h + 1) * half));
                halves.push_back(std::move(mip));
            }
            s.frothImage = true;
        } else halves.push_back(base); // unknown layout: environment only, pass B disabled
        std::size_t rgba = 0;
        for (const auto& mip : halves) {
            s.textureBytes.emplace_back();
            if (!rr::assets::encodeTextureLevel48E(mip, s.textureBytes.back(), error)) {
                log = "water texture level rejected: " + error.code; return false; }
            rgba += mip.rgba.size();
        }
        std::vector<rr::assets::MaterialTextureAssetInput50EA> inputs;
        std::vector<rr::assets::MaterialTextureSlotInput50EA> slots;
        std::size_t source = 0;
        for (std::uint32_t i = 0; i < s.textureBytes.size(); ++i) {
            inputs.push_back({i + 1u, &s.textureBytes[i]});
            rr::assets::MaterialTextureSlotInput50EA slot{};
            slot.key = {i + 1u, 0u, 1u, i + 1u}; slot.textureIdentity = i + 1u;
            slot.requirement = rr::assets::MaterialTextureRequirement50EA::required;
            slots.push_back(slot); source += s.textureBytes[i].size();
        }
        const auto count = static_cast<std::uint32_t>(inputs.size());
        rr::assets::MaterialTextureAssetsInput50EA all{inputs.data(), count, slots.data(), count,
            {count, count, source, rgba, 1024u}};
        auto bank = std::make_unique<rr::assets::MaterialTextureAssets50EA>();
        rr::assets::MaterialTextureAssetDiagnostic50EA bankDiagnostic;
        if (!bank->load(all, bankDiagnostic)) { log = "water texture bank rejected: " + bankDiagnostic.parserMessage; return false; }
        s.assets = std::move(bank);
        s.vertices.reserve(waterSurfaceVertexCapacity50WS(s.grid));
        s.cold = true;
        log = "wtt=" + resources.waterMember.name + " grid_n=" + std::to_string(s.grid.n) +
            " bands=" + std::to_string(s.grid.bands) + " rows=" + std::to_string(s.grid.rows) +
            " columns=" + std::to_string(s.grid.columns) + " track_id=" + std::to_string(selection.selectedTrackId1125F) +
            " humans=" + std::to_string(humanCount) + " env=" + std::to_string(base.width) + "x" + std::to_string(base.height) +
            " froth_image=" + std::to_string(s.frothImage ? 1 : 0) +
            " opacity=" + std::to_string(s.authoring.baseOpacity) + ".." + std::to_string(s.authoring.maxOpacity) +
            " alpha_ref=" + std::to_string(s.authoring.alphaReference) +
            " tcs=" + std::to_string(s.authoring.texCoordScale) + " zones=" + std::to_string(s.authoring.zoneCount);
        return true;
    } catch (const std::exception& e) { log = std::string("water surface cold: ") + e.what(); reset(); return false; }
}
inline bool prepareGpu(const rr::platform::MaterialTextureGlRequest50EB& base,
    rr::platform::MaterialTextureGlDiagnostic50EB& diagnostic) {
    auto& s = state();
    if (!s.cold || !s.assets || s.bank.ownsResources()) return true;
    // Environment clamps (paraboloid ST stays in 0..1); froth repeats (world
    // XZ x TexCoordScale). PENDING emulator check W-E1: CLAMP/TEX1 values.
    std::vector<rr::platform::MaterialTextureSampler50EB> samplers;
    samplers.push_back({rr::platform::MaterialTextureFilter50EB::linear, rr::platform::MaterialTextureFilter50EB::linear,
        rr::platform::MaterialTextureWrap50EB::clampToEdge, rr::platform::MaterialTextureWrap50EB::clampToEdge});
    if (s.frothImage)
        samplers.push_back({rr::platform::MaterialTextureFilter50EB::linear, rr::platform::MaterialTextureFilter50EB::linear,
            rr::platform::MaterialTextureWrap50EB::repeat, rr::platform::MaterialTextureWrap50EB::repeat});
    auto request = base; request.samplers = samplers.data();
    request.samplerCount = static_cast<std::uint32_t>(samplers.size()); request.maximumGpuBytes = 1u * 1024u * 1024u;
    return s.bank.create(request, *s.assets, diagnostic);
}
inline void bindRequest(rr::platform::VehicleWindowRequest50DS& request) noexcept {
    auto& s = state();
    request.water = {};
    if (!s.cold || !s.bank.ownsResources()) return;
    request.water.environmentTexture = s.bank.texture(0);
    request.water.frothTexture = s.frothImage ? s.bank.texture(1) : 0u;
    request.water.maximumStrips = static_cast<std::uint32_t>(
        waterSurfaceVertexCapacity50WS(s.grid) / (2u * waterStripVertices50WS));
}
// Per frame, after the water runtime update completed and the camera view is
// published. Gate: SeesWater[camera zone] (2B9140 -> Water+0x4A0, 2B8B80).
inline void buildFrame(const native_water_runtime50JC::Owner& runtime, const CameraViewOutput49F& view,
    std::uint32_t cameraZone48, rr::platform::VehicleWindowFrame50DS& frame) noexcept {
    auto& s = state();
    frame.waterVertices = nullptr; frame.waterStrips = 0;
    if (!s.cold || !s.bank.ownsResources()) return;
    // Diagnostic switches (muted A/B runs only): RR_WATER_DRAW=0 submits no
    // water; RR_WATER_MISS_SCALE=x places plane-missing rays at x * far
    // (default 2: beyond the far plane, a flagged stand-in, see missDepthScale).
    static const bool drawOff = [] { const char* e = std::getenv("RR_WATER_DRAW"); return e && e[0] == '0' && !e[1]; }();
    static const float missScale = [] { const char* e = std::getenv("RR_WATER_MISS_SCALE");
        const float v = e ? static_cast<float>(std::atof(e)) : 2.f; return std::isfinite(v) && v > 0.f ? v : 2.f; }();
    if (drawOff) return;
    ++s.frames;
    const bool sees = s.authoring.seesWater48(cameraZone48);
    const auto* provider = runtime.provider();
    const auto* globals = runtime.globals();
    if (!sees || !provider || !globals || !provider->baseHeight3C) { ++s.hiddenFrames; }
    else {
        Sampler context{provider, globals};
        WaterSurfaceFrame50WS f{};
        const auto& scene = view.sceneAfter;
        f.camera.position[0] = view.position.x; f.camera.position[1] = view.position.y; f.camera.position[2] = view.position.z;
        f.camera.look[0] = scene.look.x; f.camera.look[1] = scene.look.y; f.camera.look[2] = scene.look.z;
        f.camera.up[0] = scene.up.x; f.camera.up[1] = scene.up.y; f.camera.up[2] = scene.up.z;
        f.camera.fovDegrees = scene.fovDegrees; f.camera.aspect1F8 = scene.aspect1F8;
        f.camera.nearPlane = scene.nearPlane; f.camera.farPlane = scene.farPlane;
        f.grid = s.grid; f.authoring = &s.authoring;
        f.baseHeight = *provider->baseHeight3C;
        f.frothPhase35C = runtime.frothPhase35C();
        f.sample = &sample; f.sampleContext = &context; f.missDepthScale = missScale;
        if (buildWaterSurface50WS(f, s.vertices, s.scratch, s.counters) &&
            s.counters.vertices == s.counters.strips * 2u * waterStripVertices50WS) {
            frame.waterVertices = s.vertices.data();
            frame.waterStrips = s.counters.strips;
            frame.waterAlphaReference = static_cast<std::uint32_t>(std::clamp(s.authoring.alphaReference, 0, 255));
            frame.waterDepthWrite = false; // PENDING emulator check W-E1 (ZBUF ZMSK)
            ++s.drawnFrames;
        } else ++s.failedFrames;
    }
    if (s.frames == 1 || s.frames % 625 == 0) {
        const auto& c = s.counters;
        std::fprintf(stderr,
            "native_water_surface frames=%llu drawn=%llu hidden=%llu failed=%llu zone=%u sees=%u strips=%u vertices=%u "
            "ray_misses=%u sample_failures=%u degenerate_normals=%u height=%.3f..%.3f env_alpha=%u..%u "
            "froth_vertices=%u froth_alpha_max=%u froth_source=original_none\n",
            static_cast<unsigned long long>(s.frames), static_cast<unsigned long long>(s.drawnFrames),
            static_cast<unsigned long long>(s.hiddenFrames), static_cast<unsigned long long>(s.failedFrames),
            cameraZone48, sees ? 1u : 0u, c.strips, c.vertices, c.rayMisses, c.sampleFailures, c.degenerateNormals,
            c.heightMin, c.heightMax, c.envAlphaMin, c.envAlphaMax, c.frothVertices, c.frothAlphaMax);
    }
}
inline bool closeGpu(rr::platform::MaterialTextureGlDiagnostic50EB& diagnostic) noexcept {
    auto& s = state();
    const bool clean = !s.bank.ownsResources() || s.bank.close(diagnostic);
    s.cold = false;
    return clean;
}
} // namespace native_water_surface50WS
