#pragma once
// The same pass -> native span dispatch as the Rider Select composer
// (original_ui_rider_draw_contribution50kg.cpp material()), for the UIUnlock
// pod/pilot/binder passes. Every branch calls the existing public preparers.
#include "original_ui_material_adapter50kg.h"
#include "original_ui_lit_material_adapter50km.h"
#include "../../../src/assets/original_ui_material50kg.h"
#include "../../../src/assets/original_ui_screen_assets50kg.h"
#include <string>

namespace rr::platform {
namespace unlock_model_material50pd {
inline char fold(char c) noexcept {return c=='\\'?'/':c>='A'&&c<='Z'?char(c+32):c;}
inline bool same(const std::string& a,const char* b) noexcept {
    std::size_t i=0;for(;i<a.size()&&b[i];++i)if(fold(a[i])!=fold(b[i]))return false;
    return i==a.size()&&!b[i];
}
}
inline bool convertOriginalUiUnlockModelPass50PD(const assets::OriginalUiMaterialPass50KG& pass,
    const assets::OriginalUiScreenAssets50KG& images,std::uint32_t index,std::uint32_t quadCount,OriginalUiMaterialSpan50KG& output,
    assets::OriginalUiMaterialDiagnostic50KG& detail) noexcept {
    using unlock_model_material50pd::same;
    const auto* textures=images.textures();if(!textures)return false;
    if(pass.textureCount==0){assets::OriginalUiNativeUntexturedMaterial50KG policy{};
        return assets::prepareOriginalUiNativeUntexturedMaterial50KG(pass,policy,detail)&&
            convertOriginalUiUntexturedMaterialSpan50KG(policy,{0,quadCount,quadCount},output);}
    const auto* texture=images.decodedTexture(index);if(!texture)return false;
    if(!quadCount)return false;const OriginalUiMaterialSpanBinding50KG range{0,quadCount,quadCount,index,textures->textureCount()};
    if(same(pass.microprogram,"ScreenObject")){assets::OriginalUiNativeMaterial50KG policy{};
        return assets::prepareOriginalUiNativeMaterial50KG(pass,*texture,policy,detail)&&convertOriginalUiMaterialSpan50KG(policy,range,output);}
    if(same(pass.textureName,"02_shdw")||same(pass.textureName,"03_shdw")||same(pass.textureName,"04_shdw")||
       same(pass.textureName,"07_shdw")||same(pass.textureName,"10_shdw")||same(pass.textureName,"11_shdw")||
       same(pass.textureName,"12_shdw")||same(pass.textureName,"15_shdw")||
       same(pass.textureName,"19_shdw")||same(pass.textureName,"20_shdw")||
       same(pass.textureName,"09_shdw")||same(pass.textureName,"23_shdw")||same(pass.textureName,"24_shdw")||
       same(pass.textureName,"01_shdw")||same(pass.textureName,"08_shdw")||same(pass.textureName,"14_shdw")||same(pass.textureName,"16_shdw")||same(pass.textureName,"17_shdw")||
       same(pass.textureName,"18_shdw")||same(pass.textureName,"21_shdw")||same(pass.textureName,"22_shdw")||same(pass.textureName,"25_shdw")){
        assets::OriginalUiRider19ShadowMaterial50KM policy{};
        return assets::prepareOriginalUiRider19ShadowMaterial50KM(pass,images,index,policy,detail)&&
            convertOriginalUiRider19ShadowMaterialSpan50KM(policy,images,range,output);}
    if(same(pass.microprogram,"SpecularMapCompressed")){assets::OriginalUiRider19SpecularMaterial50KM policy{};
        return assets::prepareOriginalUiRider19SpecularMaterial50KM(pass,images,index,policy,detail)&&
            convertOriginalUiRider19SpecularMaterialSpan50KM(policy,images,range,output);}
    if(pass.gs[assets::OriginalUiGsField50KG::onePassAntiAliasingOnOff].value==1){assets::OriginalUiRider19IpumMaterial50KM policy{};
        return assets::prepareOriginalUiAa1IpumMaterial50KM(pass,images,index,policy,detail)&&
            convertOriginalUiRider19IpumMaterialSpan50KM(policy,images,range,output);}
    if(texture->kind==2){assets::OriginalUiRider19IpumMaterial50KM policy{};
        return assets::prepareOriginalUiRider19IpumMaterial50KM(pass,images,index,policy,detail)&&
            convertOriginalUiRider19IpumMaterialSpan50KM(policy,images,range,output);}
    if(same(pass.textureName,"17_3")){assets::OriginalUiRider17CutoutMaterial50KM policy{};
        return assets::prepareOriginalUiRider17CutoutMaterial50KM(pass,images,index,policy,detail)&&
            convertOriginalUiRider17CutoutMaterialSpan50KM(policy,images,range,output);}
    if(texture->mips.size()>1){
        if(same(pass.textureName,"pod_wndw")||same(pass.textureName,"12c_4")){assets::OriginalUiRiderWindowMaterial50KM policy{};
            return assets::prepareOriginalUiRiderWindowMaterial50KM(pass,images,index,policy,detail)&&
                convertOriginalUiRiderWindowMaterialSpan50KM(policy,images,range,output);}
        assets::OriginalUiRider19BodyMaterial50KM policy{};
        return assets::prepareOriginalUiRider19BodyMaterial50KM(pass,images,index,policy,detail)&&
            convertOriginalUiRider19BodyMaterialSpan50KM(policy,images,range,output);
    }
    assets::OriginalUiLitMaterial50KM policy{};
    return assets::prepareOriginalUiLitMaterial50KM(pass,*texture,policy,detail)&&convertOriginalUiLitMaterialSpan50KM(policy,range,output);
}
}
