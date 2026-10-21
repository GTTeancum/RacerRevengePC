#pragma once
#include "mc_shield_uv_fixture.h"
#include "mc_original_world50lm.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
namespace mc_shield_render50MC {
// Cold renderer join only. Does not infer a 26EF10/29B050 model-instance
// registration from VertexAnimation=unique. Existing owner resolves the real
// prototype/explicit-instance policy on the actual render service callback.
inline bool bind(const mc_shield_uv_fixture::Owner& uv,const mc_original_world50LM::Owner& world,
 std::uint32_t index,rr::platform::VehicleWindowTrackSpan50DS& output)noexcept {
 if(!uv.loaded()||!uv.rows(index)||!world.loaded()||!world.spans()||!world.selections()||
    index>=world.spans()->size()||!uv.bindings())return false;
 const auto& source=(*world.spans())[index];const mc_shield_uv_fixture::Binding* match=nullptr;
 for(const auto& b:*uv.bindings())if(b.sourceSpanIndex==index){if(match)return false;match=&b;}
 if(!match||match->worldInstanceIndex!=source.worldInstanceIndex||match->materialSelection!=source.materialSelection||
    source.materialSelection>=world.selections()->size())return false;
 const auto& selected=(*world.selections())[source.materialSelection];
 if(match->sourceMaterialIndex!=selected.materialIndex||selected.track!=track_source_fixture::Track::mc||
    selected.assetIdentity!=0x4d43||selected.materialName!="mc_shield"||selected.program!="diffuseTextransCompressed"||
    selected.textureName!="mc_shield"||selected.pass!=1||selected.passCount!=1||selected.slot!=1||
    source.textureIndex!=selected.textureIndex||output.indexStart!=source.indexStart||output.indexCount!=source.indexCount||
    output.vertexStart!=source.vertexStart||output.vertexCount!=source.vertexCount||output.sourcePass!=1||output.sourcePassCount!=1||
    output.projectionPass||output.sourceUvAnimation||output.sourceUvPolicy!=rr::platform::NativeTrackUvPolicy50DS::legacySinglePass)return false;
 const auto& material=output.material;
 if(material.mode!=rr::platform::VehicleWindowMaterialMode50DS::requiredTexture||material.textureIndex!=source.textureIndex||
    material.blend!=rr::platform::VehicleWindowBlend50DS::additive||material.depthWrite||!material.depthEqual||
    material.alphaScale!=(255.f/128.f)*(255.f/128.f)||material.fixedBlendAlpha||material.unlit||material.sourcePrelit||
    material.sourceDiffuseCompressed50LA||material.sourceSpecularPair50LA||!output.sourceAlphaTest||
    output.sourceAlphaReference||output.discardZeroAlpha||!output.sourceFogKnown||!output.sourceFogEnabled)return false;
 output.sourceUvAnimation=true;return true;
}
}
