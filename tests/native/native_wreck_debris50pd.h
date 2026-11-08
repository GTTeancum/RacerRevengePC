#pragma once
// Cold debris (wreckage) assets for every racer of a race, on every track.
//
// Original racer constructor 2129A4..212B48 loads PHY [Wreckage] NumberOfParts
// and PartN collections; 22BF00/22BFD0/22C310 construct each collection's
// ordered pieces (PSG model, m.COL/f.COL queries) and tuning. This owner uses
// the existing authenticated inventory/loader (original_wreck_asset_inventory
// 50la, proven on RA) for each racer's retained RES, then keeps each loaded
// piece's LOD0 mesh (original bind matrices applied, piece-local space), its
// spans/material names, authored placement and physics authoring for the
// native debris simulation and renderer (docs/pod-destruction-20261108.md).
#include "original_wreck_rig_inventory50la.h"
#include "../../src/game/original_wreck_constructor_vectors50la.h"
#include "../../src/assets/native_asset48b.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/material_texture_assets50ea.h"
#include <map>
#include <array>
#include <cfenv>
#include <memory>
#include <string>
#include <vector>

namespace native_wreck_debris50PD {

struct Piece {
    std::size_t collection=0,piece=0;          // zero-based source order
    rr::assets::Mesh48B mesh;                  // piece-local, original bind matrices applied
    std::vector<rr::assets::SceneSpan48D> spans;
    std::vector<std::string> materials;
    std::array<float,4> authored50{},look60{},up70{};
    float weight=0.f,inertia=0.f,breakRate=0.f,breakScale=0.f;
    bool emitSmoke=false;
    // Per scene material: debris texture bank index, or unresolved.
    std::vector<std::uint32_t> materialTexture;
};
struct Racer {
    std::string podId;
    std::unique_ptr<rr::game::OriginalWreckAssetInventory50LA> inventory;
    std::vector<Piece> pieces;
    std::vector<rr::game::OriginalWreckTuning50LA> tuning; // per collection
    float gravityAcceleration=0.f;
    std::size_t authoredPieces=0;
    bool complete=false;
    std::string error;
    // Base (pass1/slot1) textures of the pieces' materials, decoded like the
    // pod's own (original_racer_visual50hw_fixture loadOriginalDisc).
    std::vector<std::vector<std::uint8_t>> textureBytes;
    std::vector<std::string> textureNames;
    std::unique_ptr<rr::assets::MaterialTextureAssets50EA> textures;
    std::size_t unresolvedMaterials=0;
};
struct Cold {
    // Owned initialized source projection (3CA020..3CA09C); the inventory
    // retains a pointer, so it lives as long as every racer below.
    rr::game::OriginalWreckConstructorVectors50LA vectors=rr::game::initializedOriginalWreckAssetVectors50LA();
    std::array<Racer,8> racers;
};
inline Cold& cold() noexcept { static Cold owner; return owner; }

// Same explicit host opaque-W endpoint words as the racer collision owners
// (original_disc_query_bootstrap50LE): COL authors endpoint XYZ only.
inline rr::game::OriginalWreckInventoryEndpointPolicy50LA endpointPolicy() noexcept {
    return {nullptr,[](void*,const rr::game::OriginalWreckCollectionAuthoring50LA&,
        const rr::game::OriginalWreckPieceAuthoring50LA&,std::size_t index,
        rr::game::Type2OpaqueEndpointWords49X& out) noexcept {
        if(index>=32768)return false;
        out={0x80000000u+static_cast<std::uint32_t>(index),0x7fc10000u+static_cast<std::uint32_t>(index)};
        return true;
    }};
}

inline std::string lowerName(std::string value) {
    for(char& c:value){if(c>='A'&&c<='Z')c=static_cast<char>(c-'A'+'a');if(c=='/')c='\\';}
    return value;
}
// Decodes every piece material's base texture into one bank for the racer.
inline void loadTextures(Racer& racer,const native_racer50HW::Rig& rig) {
    std::map<std::string,std::uint32_t> byName;
    std::size_t rgbaBytes=0,sourceBytes=0;
    for(auto& piece:racer.pieces){
        piece.materialTexture.assign(piece.materials.size(),rr::assets::unresolvedMaterialTexture50EA);
        for(std::size_t mi=0;mi<piece.materials.size();++mi){
            const auto name=lowerName(piece.materials[mi]);
            const auto* psm=rig.originalEntry("textures/"+name+".psm");
            if(!psm){++racer.unresolvedMaterials;continue;}
            rr::assets::Material48E material;rr::assets::DecodeError48C error;
            if(!rr::assets::decodeMaterial48E(rig.originalMember(psm->name),material,error)||material.passes.empty()){
                ++racer.unresolvedMaterials;continue;}
            std::string textureName;
            for(const auto& map:material.passes.front().textures)if(map.slot==1){textureName=lowerName(map.name);break;}
            if(textureName.empty()){++racer.unresolvedMaterials;continue;}
            auto known=byName.find(textureName);
            if(known==byName.end()){
                const auto* pst=rig.originalEntry("textures/"+textureName+".pst");
                if(!pst){++racer.unresolvedMaterials;continue;}
                rr::assets::Texture48E texture;
                if(!rr::assets::decodeTexture48E(rig.originalMember(pst->name),texture,error)||texture.mips.empty()){
                    ++racer.unresolvedMaterials;continue;}
                const auto& mip=texture.mips.back();
                rgbaBytes+=static_cast<std::size_t>(mip.width)*mip.height*4u;sourceBytes+=texture.base_rrtex.size();
                const auto index=static_cast<std::uint32_t>(racer.textureBytes.size());
                racer.textureBytes.push_back(std::move(texture.base_rrtex));racer.textureNames.push_back(textureName);
                known=byName.emplace(textureName,index).first;
            }
            piece.materialTexture[mi]=known->second;
        }
    }
    if(racer.textureBytes.empty())return;
    std::vector<rr::assets::MaterialTextureAssetInput50EA> inputs;std::vector<rr::assets::MaterialTextureSlotInput50EA> slots;
    for(std::uint32_t i=0;i<racer.textureBytes.size();++i){
        inputs.push_back({i+1u,&racer.textureBytes[i]});
        rr::assets::MaterialTextureSlotInput50EA slot{};slot.key={1u,i,1u,1u};slot.textureIdentity=i+1u;
        slot.requirement=rr::assets::MaterialTextureRequirement50EA::required;slots.push_back(slot);
    }
    rr::assets::MaterialTextureAssetsInput50EA input{inputs.data(),static_cast<std::uint32_t>(inputs.size()),
        slots.data(),static_cast<std::uint32_t>(slots.size()),
        {static_cast<std::uint32_t>(inputs.size()),static_cast<std::uint32_t>(slots.size()),sourceBytes,rgbaBytes,1024u}};
    auto bank=std::make_unique<rr::assets::MaterialTextureAssets50EA>();
    rr::assets::MaterialTextureAssetDiagnostic50EA diagnostic;
    if(bank->load(input,diagnostic))racer.textures=std::move(bank);
    else{racer.error=racer.error.empty()?"debris texture bank: "+diagnostic.parserMessage:racer.error;
        for(auto& piece:racer.pieces)piece.materialTexture.assign(piece.materials.size(),rr::assets::unresolvedMaterialTexture50EA);}
}

// Loads every piece of one racer in source order. Pieces are loaded until the
// first one the existing decoders reject (logged with its ordinals); earlier
// pieces stay usable. Returns false only when the inventory itself fails.
inline bool loadRacer(std::size_t slot,const native_racer50HW::Rig& rig,
    const std::vector<std::uint8_t>& retainedRes,std::string& log) {
    auto& owner=cold();
    if(slot>=owner.racers.size()){log="slot outside the race field";return false;}
    auto& racer=owner.racers[slot];
    racer=Racer{};racer.podId=rig.podId;
    racer.inventory=std::make_unique<rr::game::OriginalWreckAssetInventory50LA>();
    using S=rr::game::OriginalWreckInventoryStatus50LA;
    std::string error;
    if(original_wreck_rig50LA::inventoryRetainedRig(rig,retainedRes,owner.vectors,*racer.inventory,error).status!=S::inventoried){
        racer.error=error.empty()?"inventory rejected":error;log=racer.error;return false;
    }
    const auto& authoring=racer.inventory->authoring();
    racer.gravityAcceleration=authoring.gravityAcceleration;
    for(const auto& collection:authoring.collections){
        racer.tuning.push_back(collection.tuning);racer.authoredPieces+=collection.pieces.size();
    }
    const auto policy=endpointPolicy();
    while(!racer.inventory->complete()){
        const auto loaded=racer.inventory->loadNextCold(policy,error);
        if(loaded.status!=S::pieceLoaded&&loaded.status!=S::complete){
            racer.error="collection "+std::to_string(loaded.collectionOrdinal)+" piece "+
                std::to_string(loaded.pieceOrdinal)+": "+(error.empty()?"rejected":error);
            break;
        }
        if(loaded.status==S::complete)break;
    }
    racer.complete=racer.inventory->complete();
    const auto& collections=racer.inventory->collections();
    for(std::size_t c=0;c<collections.size();++c)
        for(std::size_t p=0;p<collections[c].pieces.size();++p){
            const auto* assets=racer.inventory->pieceAssets(c,p);
            if(!assets)continue;
            Piece piece;piece.collection=c;piece.piece=p;
            const auto& scene=assets->scene();
            std::string meshError;
            if(scene.neutral_lod0.empty()||
               !rr::assets::parseMesh48B(scene.neutral_lod0.data(),scene.neutral_lod0.size(),piece.mesh,meshError)){
                if(racer.error.empty())racer.error="collection "+std::to_string(c+1)+" piece "+std::to_string(p+1)+
                    " mesh: "+(meshError.empty()?"empty":meshError);
                continue;
            }
            piece.spans=scene.spans_lod0;piece.materials=scene.materials;
            piece.authored50=assets->authored50();piece.look60=assets->look60();piece.up70=assets->up70();
            if(c<authoring.collections.size()&&p<authoring.collections[c].pieces.size()){
                const auto& a=authoring.collections[c].pieces[p];
                piece.weight=a.weight;piece.inertia=a.inertia;piece.breakRate=a.breakRate;
                piece.breakScale=a.breakScale;piece.emitSmoke=a.emitSmoke;
            }
            racer.pieces.push_back(std::move(piece));
        }
    loadTextures(racer,rig);
    log="pieces="+std::to_string(racer.pieces.size())+"/"+std::to_string(racer.authoredPieces)+
        " collections="+std::to_string(authoring.collections.size())+" complete="+(racer.complete?"1":"0")+
        " textures="+std::to_string(racer.textureBytes.size())+" unresolved_materials="+
        std::to_string(racer.unresolvedMaterials)+" gravity="+std::to_string(racer.gravityAcceleration);
    for(std::size_t c=0;c<racer.tuning.size();++c){const auto& t=racer.tuning[c];
        log+=" [c"+std::to_string(c+1)+" drag="+std::to_string(t.drag)+" rate="+std::to_string(t.rateDamping)+
            " rest="+std::to_string(t.restitution)+" fric="+std::to_string(t.friction)+"]";}
    for(const auto& piece:racer.pieces)log+=" w"+std::to_string(piece.collection+1)+"."+std::to_string(piece.piece+1)+"="+
        std::to_string(piece.weight)+"/"+std::to_string(piece.inertia)+(piece.emitSmoke?"/smoke":"");
    if(!racer.error.empty())log+=" first_error=\""+racer.error+"\"";
    return true;
}

} // namespace native_wreck_debris50PD
