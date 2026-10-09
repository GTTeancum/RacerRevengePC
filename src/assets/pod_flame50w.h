#pragma once
#include "import_scene48d.h"
#include "import_texture48e.h"
#include "pod_flame_runtime50w.h"
namespace rr::assets {
enum class PodFlameError50W {None,Models,Materials,Shader,Animation,Rows,Arithmetic};
// Cold authored projection for the retained Pod01 layout. Standard asset decode
// may allocate before this call; row parsing may allocate here. Hot binding does
// not allocate. Uniforms are native semantic float rows, never guest VIF packets.
// Only supported flame animation is represented; other renderer state stays in
// the original decoded material assets. Failure leaves output unchanged.
bool buildPodFlameOwners50W(const Scene48D&left,const Scene48D&right,
 const Material48E&shared,const Material48E&leftFlame,const Material48E&rightFlame,
 std::uint32_t leftModelId,std::uint32_t rightModelId,PodFlameOwners50W&,PodFlameError50W&);
}
