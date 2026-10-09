#pragma once
#include "../game/flame_bindings50v.h"
namespace rr::assets {
// The native material binder admits at most sixteen references per model.
// POD09 retains nine per engine; POD01 and the other selected rigs retain eight.
constexpr std::uint32_t podFlameReferencesPerModel50W=16;
constexpr std::uint32_t podFlameReferenceCapacity50W=2*podFlameReferencesPerModel50W;
struct FlameUniformRows50W {float rows[3][4];};
struct PodFlameOwners50W {
 rr::game::FlameModel50V models[4];
 rr::game::FlameMaterial50V materials[3];
 rr::game::FlamePass50V passes[3];
 rr::game::FlameInstance50V instances[2];
 std::uint32_t references[podFlameReferenceCapacity50W];
 rr::game::FlameRecord50V records[4];
 FlameUniformRows50W uniforms[4]; // left/right immutable templates, then private instances
};
inline rr::game::FlameViews50V flameViews50W(const PodFlameOwners50W&o) noexcept {return {o.materials,3,o.passes,3,o.instances,2,o.references,podFlameReferenceCapacity50W};}
}
