// GPU vertex path algebra versus the CPU source functions. Builds the CPU
// inputs exactly as prepareSourceColors / prepareOriginalRacerSpecular50LA /
// projectDrawPositions do, and evaluates the vertex shader's formulas (same
// uniform layout bindRacerSkin50LA uploads) in C++ float arithmetic.
// Diffuse RGBA may differ by one step (ties, rounding order); positions and
// map-2 UV must agree to float tolerance.
#include "../../src/game/original_racer_material50km.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
using namespace rr::game::original_racer_lighting50KM;
namespace {
unsigned seed = 7;
float unit() { seed = seed * 1664525u + 1013904223u; return float(seed >> 8) / float(1u << 24); }
float range(float a, float b) { return a + (b - a) * unit(); }
void need(bool ok, const char* why) { if (!ok) { std::fprintf(stderr, "FAILED: %s\n", why); std::exit(1); } }
}
int main() {
    need(selection_detail::arithmeticReady(), "arithmetic mode");
    int worstColor = 0; double worstPosition = 0, worstUv = 0; unsigned checked = 0;
    for (unsigned trial = 0; trial < 20000; ++trial) {
        // Bone: random rotation-ish affine, row-vector BodyMatrix48G layout.
        float m[16]{};
        for (unsigned k = 0; k < 3; ++k) for (unsigned a = 0; a < 3; ++a) m[k * 4 + a] = range(-1.2f, 1.2f);
        for (unsigned a = 0; a < 3; ++a) m[12 + a] = range(-500.f, 500.f);
        m[15] = 1.f;
        const float normalQ = range(0.005f, 0.02f);
        float direction[4][4]{}, diffuse[4][4]{};
        for (unsigned r = 0; r < 3; ++r) for (unsigned l = 0; l < 3; ++l) direction[r][l] = range(-1.f, 1.f);
        for (unsigned r = 0; r < 4; ++r) for (unsigned l = 0; l < 4; ++l) diffuse[r][l] = range(0.f, 90.f);
        CompressedNormal n{};
        for (auto& v : n) v = static_cast<std::int8_t>(int(range(-127.f, 127.f)));
        const float local[3] = {range(-3.f, 3.f), range(-3.f, 3.f), range(-3.f, 3.f)};

        // CPU: prepareSourceColors' scaled rows and per-bone normalAxes.
        float scaled[3][3];
        for (unsigned axis = 0; axis < 3; ++axis) for (unsigned lane = 0; lane < 3; ++lane)
            scaled[axis][lane] = material_detail::mul(direction[axis][lane], normalQ);
        DiffuseRgbaProgramInputs inputs{};
        for (unsigned r = 0; r < 4; ++r) for (unsigned l = 0; l < 4; ++l) inputs.colorRows[r][l] = diffuse[r][l];
        for (unsigned axis = 0; axis < 3; ++axis)
            for (unsigned lane = 0; lane < 3; ++lane) {
                float value = material_detail::mul(scaled[0][lane], m[axis * 4]);
                for (unsigned c = 1; c < 3; ++c)
                    value = material_detail::add(value, material_detail::mul(scaled[c][lane], m[axis * 4 + c]));
                inputs.normalAxes[axis][lane] = material_detail::add(value, material_detail::mul(0.f, m[axis * 4 + 3]));
            }
        DiffuseVertexRgba cpu{};
        if (material_detail::projectDiffuseCompressedRgbaFiniteInputs(inputs, n, cpu) != Status::complete) continue;

        // Shader: rows r_axis = (m[axis], m[4+axis], m[8+axis], m[12+axis]);
        // lightDirections[light][axis] = scaled[axis][light].
        float rows[3][4];
        for (unsigned axis = 0; axis < 3; ++axis) {
            rows[axis][0] = m[axis]; rows[axis][1] = m[4 + axis]; rows[axis][2] = m[8 + axis]; rows[axis][3] = m[12 + axis];
        }
        float lightDirections[3][3];
        for (unsigned light = 0; light < 3; ++light) for (unsigned axis = 0; axis < 3; ++axis)
            lightDirections[light][axis] = scaled[axis][light];
        const float nf[3] = {float(n[0]), float(n[1]), float(n[2])};
        float mm[3];
        for (unsigned c = 0; c < 3; ++c) mm[c] = rows[c][0] * nf[0] + rows[c][1] * nf[1] + rows[c][2] * nf[2];
        for (unsigned lane = 0; lane < 4; ++lane) {
            // Shader: roundEven after ambient and after each light term.
            float packed = std::nearbyint(diffuse[3][lane]);
            for (unsigned light = 0; light < 3; ++light) {
                const float amount = std::max(lightDirections[light][0] * mm[0] + lightDirections[light][1] * mm[1] +
                    lightDirections[light][2] * mm[2], 0.f);
                packed = std::nearbyint(packed + diffuse[light][lane] * amount);
            }
            const int gpu = int(std::clamp(packed, 0.f, 255.f));
            worstColor = std::max(worstColor, std::abs(gpu - int(cpu.rgba[lane])));
        }

        // Positions: projectDrawPositions vs shader dot(row, (local, 1)).
        for (unsigned axis = 0; axis < 3; ++axis) {
            double sum = 0;
            for (unsigned k = 0; k < 3; ++k) sum += double(local[k]) * double(m[k * 4 + axis]);
            const float cpuPosition = float(sum + double(m[12 + axis]));
            const float gpuPosition = rows[axis][0] * local[0] + rows[axis][1] * local[1] + rows[axis][2] * local[2] + rows[axis][3];
            worstPosition = std::max(worstPosition, double(std::abs(cpuPosition - gpuPosition)) / (1.0 + std::abs(cpuPosition)));
        }

        // Map-2 UV: CPU leaf vs shader 0.5 * (axes . n) + 0.5.
        SpecularMapDualInputs specular{};
        for (unsigned axis = 0; axis < 3; ++axis) for (unsigned lane = 0; lane < 2; ++lane)
            specular.cameraNormalAxes[axis][lane] = range(-0.02f, 0.02f);
        std::array<float, 2> cpuUv{};
        if (material_detail::projectSpecularMapDualUvFiniteInputs(specular, n, cpuUv) != Status::complete) continue;
        for (unsigned lane = 0; lane < 2; ++lane) {
            const float projected = specular.cameraNormalAxes[0][lane] * nf[0] + specular.cameraNormalAxes[1][lane] * nf[1] +
                specular.cameraNormalAxes[2][lane] * nf[2];
            worstUv = std::max(worstUv, double(std::abs(projected * 0.5f + 0.5f - cpuUv[lane])));
        }
        ++checked;
    }
    std::printf("checked=%u worst diffuse step=%d worst relative position=%.3g worst map-2 uv=%.3g\n",
        checked, worstColor, worstPosition, worstUv);
    need(checked > 10000, "enough admitted cases");
    need(worstColor <= 1, "diffuse within one step");
    need(worstPosition < 1e-5, "positions agree");
    need(worstUv < 1e-5, "map-2 UV agrees");
    std::printf("gpu_skin_math_check passed\n");
}
