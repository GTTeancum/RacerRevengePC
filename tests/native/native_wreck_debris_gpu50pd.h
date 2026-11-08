#pragma once
// GPU side of the PC-native debris: one texture bank per racer (its pieces'
// base textures) and one combined piece-local mesh for the renderer's debris
// pass. Prepared with the race renderer request, closed with the other race
// texture banks while the context is current.
#include "native_wreck_debris50pd.h"
#include "native_wreck_effects50pd.h"
#include "runtime_startup50fl_fixture.h"
#include <filesystem>
#include "../../platform/linux/native/material_texture_gl50eb.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <array>
#include <cstdio>
#include <string>
#include <vector>

namespace native_wreck_debris50PD {

struct Gpu {
    std::array<rr::platform::MaterialTextureGl50EB,8> banks;
    rr::assets::Mesh48B mesh;
    std::vector<rr::platform::VehicleWindowDebrisSpan50DS> spans;
    std::vector<rr::platform::VehicleWindowDebrisPiece50DS> pieces;
    // Renderer piece index of each racer's first loaded piece.
    std::array<std::uint32_t,8> firstPiece{},pieceCount{};
    bool prepared=false;
};
inline Gpu& gpu() noexcept { static Gpu owner; return owner; }
// Wreck particle atlas: the selected track's particle\demoprt.pst (and its
// PRT for the effects' groups/frames), decoded at cold time, uploaded with
// the race texture banks and closed with them.
struct EffectsAtlas {
    std::vector<std::uint8_t> bytes;
    std::unique_ptr<rr::assets::MaterialTextureAssets50EA> assets;
    rr::platform::MaterialTextureGl50EB bank;
    float width=0.f,height=0.f;
};
inline EffectsAtlas& effectsAtlas() noexcept { static EffectsAtlas owner; return owner; }
template<class Rig>
inline bool loadEffects(const std::filesystem::path& trackArchive,const Rig& rig,std::string& log) {
    auto& a=effectsAtlas();a.bytes.clear();a.assets.reset();a.width=a.height=0.f;
    try {
        const auto bytes=runtime_startup50fl::readBytes(trackArchive);
        rr::assets::ResArchive48C archive;rr::assets::DecodeError48C error;rr::assets::DecodeLimits48C limits;
        limits.resource_file_bytes=bytes.size();limits.resource_decoded_bytes=64u*1024u*1024u;limits.resource_entries=4096;
        std::vector<std::uint8_t> prt,pst;
        if(!rr::assets::decodeResource48C(bytes,archive,error,nullptr,limits)||
           !rr::assets::extractResMember48C(archive,"particle\\demoprt.prt",prt,error)||
           !rr::assets::extractResMember48C(archive,"particle\\demoprt.pst",pst,error)){
            log="track particle atlas unavailable: "+error.code;return false;}
        rr::assets::Texture48E texture;
        if(!rr::assets::decodeTexture48E(pst,texture,error)||texture.mips.empty()){
            log="track particle atlas texture rejected: "+error.code;return false;}
        const auto& mip=texture.mips.back();a.width=float(mip.width);a.height=float(mip.height);
        const auto rgbaBytes=std::size_t(mip.width)*mip.height*4u;
        a.bytes=std::move(texture.base_rrtex);
        rr::assets::MaterialTextureAssetInput50EA input{1u,&a.bytes};
        rr::assets::MaterialTextureSlotInput50EA slot{};slot.key={1u,0u,1u,1u};slot.textureIdentity=1u;
        slot.requirement=rr::assets::MaterialTextureRequirement50EA::required;
        rr::assets::MaterialTextureAssetsInput50EA all{&input,1u,&slot,1u,{1u,1u,a.bytes.size(),rgbaBytes,1024u}};
        auto bank=std::make_unique<rr::assets::MaterialTextureAssets50EA>();
        rr::assets::MaterialTextureAssetDiagnostic50EA diagnostic;
        if(!bank->load(all,diagnostic)){log="track particle atlas bank rejected: "+diagnostic.parserMessage;return false;}
        a.assets=std::move(bank);
        std::string effects;
        const bool ready=native_wreck_effects50PD::loadCold(rig,prt,a.width,a.height,effects);
        log="atlas="+std::to_string(a.width)+"x"+std::to_string(a.height)+" "+effects;
        return ready;
    }catch(const std::exception& e){log=std::string("track particle atlas: ")+e.what();return false;}
}
inline bool prepareEffectsGpu(const rr::platform::MaterialTextureGlRequest50EB& base,
    rr::platform::MaterialTextureGlDiagnostic50EB& diagnostic) {
    auto& a=effectsAtlas();
    if(!a.assets||a.bank.ownsResources())return true;
    std::vector<rr::platform::MaterialTextureSampler50EB> samplers(1,
        {rr::platform::MaterialTextureFilter50EB::linear,rr::platform::MaterialTextureFilter50EB::linear,
         rr::platform::MaterialTextureWrap50EB::clampToEdge,rr::platform::MaterialTextureWrap50EB::clampToEdge});
    auto request=base;request.samplers=samplers.data();request.samplerCount=1u;request.maximumGpuBytes=4u*1024u*1024u;
    return a.bank.create(request,*a.assets,diagnostic);
}

inline bool prepareGpu(const rr::platform::MaterialTextureGlRequest50EB& base,
    rr::platform::MaterialTextureGlDiagnostic50EB& diagnostic,std::string& log) {
    auto& g=gpu();
    g.mesh={};g.spans.clear();g.pieces.clear();g.firstPiece={};g.pieceCount={};g.prepared=false;
    auto& c=cold();
    std::size_t textured=0,untextured=0;
    for(std::size_t r=0;r<c.racers.size();++r){
        auto& racer=c.racers[r];
        const bool banked=racer.textures&&!racer.textureBytes.empty();
        if(banked&&!g.banks[r].ownsResources()){
            std::vector<rr::platform::MaterialTextureSampler50EB> samplers(racer.textureBytes.size(),
                {rr::platform::MaterialTextureFilter50EB::linear,rr::platform::MaterialTextureFilter50EB::linear,
                 rr::platform::MaterialTextureWrap50EB::repeat,rr::platform::MaterialTextureWrap50EB::repeat});
            auto request=base;request.samplers=samplers.data();
            request.samplerCount=static_cast<std::uint32_t>(samplers.size());request.maximumGpuBytes=8u*1024u*1024u;
            if(!g.banks[r].create(request,*racer.textures,diagnostic)){
                log="debris texture upload failed for slot "+std::to_string(r);return false;
            }
        }
        g.firstPiece[r]=static_cast<std::uint32_t>(g.pieces.size());
        for(const auto& piece:racer.pieces){
            const auto baseVertex=static_cast<std::uint32_t>(g.mesh.vertices.size());
            const auto baseIndex=static_cast<std::uint32_t>(g.mesh.indices.size());
            g.mesh.vertices.insert(g.mesh.vertices.end(),piece.mesh.vertices.begin(),piece.mesh.vertices.end());
            for(const auto index:piece.mesh.indices)g.mesh.indices.push_back(baseVertex+index);
            rr::platform::VehicleWindowDebrisPiece50DS entry{static_cast<std::uint32_t>(g.spans.size()),0};
            auto addSpan=[&](std::uint32_t start,std::uint32_t count,std::uint32_t material){
                std::uint32_t texture=0;
                if(banked&&material<piece.materialTexture.size()&&
                   piece.materialTexture[material]!=rr::assets::unresolvedMaterialTexture50EA)
                    texture=g.banks[r].texture(piece.materialTexture[material]);
                (texture?textured:untextured)+=1;
                g.spans.push_back({baseIndex+start,count,texture});++entry.spanCount;
            };
            if(piece.spans.empty())addSpan(0,static_cast<std::uint32_t>(piece.mesh.indices.size()),0);
            else for(const auto& span:piece.spans){
                if(!span.index_count||span.index_start+span.index_count>piece.mesh.indices.size())continue;
                addSpan(span.index_start,span.index_count,span.material_index);
            }
            if(!entry.spanCount)addSpan(0,static_cast<std::uint32_t>(piece.mesh.indices.size()),0);
            g.pieces.push_back(entry);
        }
        g.pieceCount[r]=static_cast<std::uint32_t>(g.pieces.size())-g.firstPiece[r];
    }
    g.prepared=!g.pieces.empty();
    log="pieces="+std::to_string(g.pieces.size())+" vertices="+std::to_string(g.mesh.vertices.size())+
        " spans_textured="+std::to_string(textured)+" spans_untextured="+std::to_string(untextured);
    return true;
}
inline void bindRequest(rr::platform::VehicleWindowRequest50DS& request) noexcept {
    auto& g=gpu();
    auto& atlas=effectsAtlas();
    request.effects.texture=atlas.bank.ownsResources()?atlas.bank.texture(0):0u;
    if(!g.prepared){request.debris={};return;}
    request.debris.mesh=&g.mesh;
    request.debris.spans=g.spans.data();request.debris.spanCount=static_cast<std::uint32_t>(g.spans.size());
    request.debris.pieces=g.pieces.data();request.debris.pieceCount=static_cast<std::uint32_t>(g.pieces.size());
}
inline bool closeGpu(rr::platform::MaterialTextureGlDiagnostic50EB& diagnostic) noexcept {
    auto& g=gpu();bool clean=true;
    for(auto& bank:g.banks)if(bank.ownsResources()&&!bank.close(diagnostic))clean=false;
    if(effectsAtlas().bank.ownsResources()&&!effectsAtlas().bank.close(diagnostic))clean=false;
    g.prepared=false;
    return clean;
}

} // namespace native_wreck_debris50PD
