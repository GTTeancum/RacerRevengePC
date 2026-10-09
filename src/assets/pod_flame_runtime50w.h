#pragma once
#include "../game/flame_bindings50v.h"
namespace rr::assets {
struct FlameUniformRows50W {float rows[3][4];};
struct PodFlameOwners50W {
 rr::game::FlameModel50V models[4];
 rr::game::FlameMaterial50V materials[3];
 rr::game::FlamePass50V passes[3];
 rr::game::FlameInstance50V instances[2];
 std::uint32_t references[16];
 rr::game::FlameRecord50V records[4];
 FlameUniformRows50W uniforms[4]; // left/right immutable templates, then private instances
};
inline rr::game::FlameViews50V flameViews50W(const PodFlameOwners50W&o) noexcept {return {o.materials,3,o.passes,3,o.instances,2,o.references,16};}
}
