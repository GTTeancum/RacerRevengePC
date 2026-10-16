#pragma once
#include "import_scene48d.h"
#include "import_texture48e.h"
#include "pod_flame_runtime50w.h"
namespace rr::assets {
enum class PodFlameError50W {None,Models,Materials,Shader,Animation,Rows,Arithmetic};
// Cold authored projection for two to four authentic engine models. Model order
// is left/right, then optional bottom-left/bottom-right. Each decoded material
// pointer must be the original PSM named by that scene's material slot.
// One unique animated flame and four authored references are retained per model.
// Same-side models share their genuine material prototype only: each model owns
// a distinct private animation record, frame stamp and uniform row set.
// Templates retain indices 0/1; model instances are 2+model, preserving 2/3
// for legacy left/right clients. Static references retain their source order.
// Cold parsing may allocate. Hot binding is bounded and allocation-free.
// Failure leaves output unchanged; no guest VIF packet or render state is made.
struct PodFlameModelInput50W {
 const Scene48D* scene=nullptr;
 const std::vector<const Material48E*>* materials=nullptr;
 std::uint32_t modelId=0;
};
bool buildPodFlameOwners50W(const PodFlameModelInput50W*,std::uint32_t modelCount,
 PodFlameOwners50W&,PodFlameError50W&);
// Compatibility adapter for existing two-model callers.
bool buildPodFlameOwners50W(const Scene48D&left,const Scene48D&right,
 const std::vector<const Material48E*>&leftMaterials,
 const std::vector<const Material48E*>&rightMaterials,
 std::uint32_t leftModelId,std::uint32_t rightModelId,PodFlameOwners50W&,PodFlameError50W&);
// Compatibility admission for the original retained POD01 material names/layout.
bool buildPodFlameOwners50W(const Scene48D&left,const Scene48D&right,
 const Material48E&shared,const Material48E&leftFlame,const Material48E&rightFlame,
 std::uint32_t leftModelId,std::uint32_t rightModelId,PodFlameOwners50W&,PodFlameError50W&);
}
