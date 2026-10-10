// Source-only acceptance cases. This file has NOT been compiled or executed.
// The central validation owner decides when to add/run it in an allowed slot.
#include "../../src/game/original_racer_material50km.h"
#include <cstdint>
#include <cstring>
#include <limits>

namespace light = rr::game::original_racer_lighting50KM;
namespace {
unsigned failed = 0;
void check(bool result) { if (!result) ++failed; }
bool same(const light::LightMatrices& a, const light::LightMatrices& b) {
    return std::memcmp(&a, &b, sizeof(a)) == 0;
}
}

int main() {
    using light::Status;
    light::SmallDirectionalRegistry registry;
    registry.count = 2;
    registry.registered[0].type = 4;
    registry.registered[1].type = 2;
    registry.registered[1].direction78 = {{0.f, -1.f, 0.f}};
    registry.registered[1].diffuse3C = {{85.f, 85.f, 80.f}};
    registry.registered[1].specular4C = {{85.f, 85.f, 80.f}};
    registry.selectedZoneAmbient = {{40.f, 40.f, 45.f, 64.f}};
    light::LightMatrices matrices;
    check(light::projectSmallDirectionalRegistry(registry, matrices) == Status::complete);
    check(matrices.directions[1][0] == 1.f);
    check(matrices.diffuse[0] == light::Vec4{{85.f, 85.f, 80.f, 0.f}});
    check(matrices.diffuse[3] == registry.selectedZoneAmbient);
    check(matrices.specular[3] == light::Vec4{});
    const auto saved = matrices;
    registry.registered[1].type = 3;
    check(light::projectSmallDirectionalRegistry(registry, matrices) == Status::unsupportedRegistry);
    check(same(matrices, saved));
    registry.count = 5;
    check(light::projectSmallDirectionalRegistry(registry, matrices) == Status::unsupportedRegistry);
    check(same(matrices, saved));
    registry.count = 4;
    for (auto& value : registry.registered) value.type = 2;
    check(light::projectSmallDirectionalRegistry(registry, matrices) == Status::unsupportedRegistry);
    check(same(matrices, saved));

    light::LightMatrices negative;
    negative.diffuse[0] = {{-20.f, -40.f, -80.f, 7.f}};
    negative.diffuse[1] = {{100.f, 100.f, 100.f, 8.f}};
    negative.diffuse[2] = {{-10.f, 0.f, -10.f, 9.f}};
    negative.diffuse[3] = {{40.f, 40.f, 45.f, 64.f}};
    negative.specular[0] = {{1.f, 2.f, 3.f, 4.f}};
    check(light::foldSelectedNegativeLights(negative, 1, matrices) == Status::complete);
    check(matrices.diffuse[0] == light::Vec4{{0.f, 0.f, 0.f, 7.f}});
    check(matrices.diffuse[1] == light::Vec4{{77.5f, 70.f, 32.5f, 8.f}});
    check(matrices.diffuse[2] == light::Vec4{{0.f, 0.f, 0.f, 9.f}});
    check(matrices.diffuse[3] == light::Vec4{{25.f, 20.f, 0.f, 64.f}});
    check(matrices.specular == negative.specular);
    check(light::foldSelectedNegativeLights(negative, -1, matrices) == Status::complete);
    check(matrices.diffuse[1] == negative.diffuse[1]);
    const auto folded = matrices;
    check(light::foldSelectedNegativeLights(negative, 3, matrices) == Status::input);
    check(same(matrices, folded));
    negative.diffuse[0][0] = -std::numeric_limits<float>::max();
    negative.diffuse[2][0] = negative.diffuse[0][0];
    check(light::foldSelectedNegativeLights(negative, 1, matrices) == Status::arithmetic);
    check(same(matrices, folded));

    light::DiffuseProgramInputs diffuse;
    for (unsigned r = 0; r < 3; ++r)
        for (unsigned c = 0; c < 3; ++c) diffuse.normalAxes[r][c] = saved.directions[r][c];
    for (unsigned r = 0; r < 4; ++r)
        for (unsigned c = 0; c < 3; ++c) diffuse.colorRows[r][c] = saved.diffuse[r][c];
    light::DiffuseVertexRgb rgb;
    check(light::projectDiffuseCompressedRgb(diffuse, {{0, 1, 0}}, rgb) == Status::complete);
    check(rgb.rgb == std::array<std::uint8_t, 3>{{125, 125, 125}});
    check(light::projectDiffuseCompressedRgb(diffuse, {{0, -1, 0}}, rgb) == Status::complete);
    check(rgb.rgb == std::array<std::uint8_t, 3>{{40, 40, 45}});
    diffuse.colorRows[0] = {{1000.f, 1000.f, 1000.f}};
    check(light::projectDiffuseCompressedRgb(diffuse, {{0, 1, 0}}, rgb) == Status::complete);
    check(rgb.rgb == std::array<std::uint8_t, 3>{{255, 255, 255}});
    // Proves the documented native early-bias arithmetic rather than taking
    // ambient+all lights first and rounding only at the end (which gives 1).
    diffuse = {};
    diffuse.normalAxes[0] = {{1.f, 1.f, 1.f}};
    for (unsigned r = 0; r < 3; ++r) diffuse.colorRows[r] = {{0.4f, 0.4f, 0.4f}};
    check(light::projectDiffuseCompressedRgb(diffuse, {{1, 0, 0}}, rgb) == Status::complete);
    check(rgb.rgb == std::array<std::uint8_t, 3>{{0, 0, 0}});
    const auto savedRgb = rgb;
    diffuse.colorRows[3][0] = -2.f;
    check(light::projectDiffuseCompressedRgb(diffuse, {{0, 0, 0}}, rgb) == Status::arithmetic);
    check(rgb.rgb == savedRgb.rgb);
    diffuse.colorRows[3][0] = std::numeric_limits<float>::quiet_NaN();
    check(light::projectDiffuseCompressedRgb(diffuse, {{0, 0, 0}}, rgb) == Status::input);
    check(rgb.rgb == savedRgb.rgb);

    light::SpecularMapDualInputs specular;
    specular.cameraNormalAxes[0] = {{1.f, 0.f}};
    specular.cameraNormalAxes[1] = {{0.f, 1.f}};
    std::array<float, 2> uv{};
    check(light::projectSpecularMapDualUv(specular, {{1, -1, 0}}, uv) == Status::complete);
    check(uv == std::array<float, 2>{{1.f, 0.f}});
    check(light::projectSpecularMapDualUv(specular, {{0, 0, 1}}, uv) == Status::complete);
    check(uv == std::array<float, 2>{{0.5f, 0.5f}});
    check(light::projectSpecularMapDualUv(specular, {{2, -2, 0}}, uv) == Status::complete);
    check(uv == std::array<float, 2>{{1.5f, -0.5f}}); // No invented clamp.
    const auto savedUv = uv;
    specular.cameraNormalAxes[0][0] = std::numeric_limits<float>::infinity();
    check(light::projectSpecularMapDualUv(specular, {{1, 0, 0}}, uv) == Status::input);
    check(uv == savedUv);
    return failed ? 1 : 0;
}
