#pragma once
#include "../game/flame_bindings50v.h"
#include "original_standard_material_parameters50la.h"
#include <type_traits>
namespace rr::assets {
class OriginalStandardMaterialAnimationInstance50LA;
// The native material binder admits at most sixteen references per model.
// POD09 retains nine per engine; POD01 and the other selected rigs retain eight.
constexpr std::uint32_t podFlameReferencesPerModel50W=16;
constexpr std::uint32_t podFlameModelCapacity50W=4;
constexpr std::uint32_t podFlameRecordCapacity50W=2+podFlameModelCapacity50W;
constexpr std::uint32_t podFlameReferenceCapacity50W=podFlameModelCapacity50W*podFlameReferencesPerModel50W;
struct FlameUniformRows50W {float rows[3][4];};
struct PodFlameOwners50W {
 rr::game::FlameModel50V models[4];
 rr::game::FlameMaterial50V materials[3];
 rr::game::FlamePass50V passes[3];
 rr::game::FlameInstance50V instances[podFlameModelCapacity50W];
 std::uint32_t references[podFlameReferenceCapacity50W];
 rr::game::FlameRecord50V records[podFlameRecordCapacity50W];
 FlameUniformRows50W uniforms[podFlameRecordCapacity50W]; // two immutable templates, then per-model instances
 std::uint32_t modelCount=2; // left/right, then optional bottom-left/bottom-right
 // When an actual constructed material/model clone is joined, that clone is
 // the sole mutable row/stamp owner for this model. The bank copies only this
 // stable borrowed pointer; no clone may retain a pointer into banked arrays.
 // Null preserves the existing native50W path for other model families.
 OriginalStandardMaterialAnimationInstance50LA* canonicalInstances50LA[podFlameModelCapacity50W]{};
 // SAME originating pass's immutable249420 seed payload. Collector merging
 // does not replace this with the representative shader's parameters. The
 // actual pass owns both this vector and the clone above through model close.
 const OriginalStandardMaterialParameters50LA* canonicalParameters50LA[podFlameModelCapacity50W]{};
};
static_assert(std::is_trivially_copyable_v<PodFlameOwners50W>,"racer bank owns only scalar/value state and stable borrows");
inline rr::game::FlameViews50V flameViews50W(const PodFlameOwners50W&o) noexcept {return {o.materials,3,o.passes,3,o.instances,o.modelCount,o.references,podFlameReferenceCapacity50W};}
}
