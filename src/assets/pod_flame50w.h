#pragma once
#include "import_scene48d.h"
#include "import_texture48e.h"
#include "pod_flame_runtime50w.h"
namespace rr::assets {
enum class PodFlameError50W {None,Models,Materials,Shader,Animation,Rows,Arithmetic};
// Cold authored projection for the retained four-LOD, two-engine rigs. Each
// material pointer is the original decoded PSM named by that scene's material
// slot. Duplicate references may share a decoded material. Each side retains
// one unique animated flame material and four authored references to it; static
// references need not alternate with the flame (POD09 retains an extra 09_2).
// The complete material lists are projected in their original order. Uniform
// templates/instances always retain left/right identity at indices 0/1 and 2/3.
// Standard asset decode
// may allocate before this call; row parsing may allocate here. Hot binding does
// not allocate. Uniforms are native semantic float rows, never guest VIF packets.
// Only supported flame animation is represented; other renderer state stays in
// the original decoded material assets. Failure leaves output unchanged.
bool buildPodFlameOwners50W(const Scene48D&left,const Scene48D&right,
 const std::vector<const Material48E*>&leftMaterials,
 const std::vector<const Material48E*>&rightMaterials,
 std::uint32_t leftModelId,std::uint32_t rightModelId,PodFlameOwners50W&,PodFlameError50W&);
// Compatibility admission for the original retained POD01 material names/layout.
bool buildPodFlameOwners50W(const Scene48D&left,const Scene48D&right,
 const Material48E&shared,const Material48E&leftFlame,const Material48E&rightFlame,
 std::uint32_t leftModelId,std::uint32_t rightModelId,PodFlameOwners50W&,PodFlameError50W&);
}
