#pragma once
// Cold original-roster cable and flame adapters. Scene endpoints always come
// from the selected racer's own PSG; shared-looking source bytes are verified
// from that racer's retained RES members before being decoded.
#include "native_racer_rig50hw_fixture.h"
#include "source_cable_texture50fc_fixture.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/pod_flame50w.h"
#include "../../src/game/pod_cables50fd.h"
#include <map>

namespace native_racer50HW {
namespace visual_detail {
inline std::vector<std::uint8_t> original(const Rig& rig,const std::string& member){
    const auto wanted=rig_detail::lower(member);const J* selected=nullptr;
    for(const auto& row:rig.manifest.at("members"))
        if(rig_detail::lower(row.at("source_member").get<std::string>())==wanted){
            rig_detail::need(!selected,"Roster duplicate original visual member");selected=&row;}
    rig_detail::need(selected,"Roster missing original visual member: "+member);
    const std::filesystem::path path=selected->at("source_file").get<std::string>();
    rig_detail::need(!path.is_absolute()&&path==path.filename(),"Roster unsafe visual member path");
    auto bytes=f::readBytes(rig.directory/path);
    rig_detail::need(bytes.size()==selected->at("source_bytes").get<std::size_t>()&&
        pod_materials50ed_fixture::detail::sha256(bytes)==selected->at("source_sha256"),
        "Roster original visual member identity differs: "+member);
    return bytes;
}
inline a::Material48E material(const Rig& rig,const std::string& name){
    const auto bytes=original(rig,"textures/"+rig_detail::lower(name)+".psm");
    a::Material48E out;a::DecodeError48C diagnostic{};
    rig_detail::need(a::decodeMaterial48E(bytes,out,diagnostic),"Roster original visual material decode");
    return out;
}
}

class Cables final {
    a::MaterialTextureAssets50EA bank_;
    g::PodCableBindings50FD bindings_{};
public:
    Cables(const Rig& rig,const g::VehicleFrameState49W& state,
        const g::VehicleFrameBindings49W& frame){
        using rig_detail::need;
        need(g::validVehicleSceneCount49W(state,frame)&&state.sceneNodeCount==rig.nodeCount&&
            frame.lifecycle.model1B4==rig.roots[Rig::chariot]&&
            frame.lifecycle.left1BC==rig.roots[Rig::left]&&frame.lifecycle.right1B8==rig.roots[Rig::right],
            "Roster cable graph identity differs");
        for(unsigned model=0;model<3;++model)for(unsigned n=0;n<rig.hierarchy[model].nodeCount;++n){
            a::PsgNode49O source{};need(a::readPsgNode49O(rig.hierarchy[model],n,source),"Roster cable source node");
            const auto parent=source.parent<0?rig.roots[model]:rig.sourceNodes[model][source.parent];
            need(state.nodes[static_cast<unsigned>(rig.sourceNodes[model][n])].parent180==parent,
                "Roster cable source/current topology differs");
        }
        const char* ordinary[]={"chariotcableleft1","chariotcableright1"};
        const char* display[]={"drawcableleft1","drawcableright1"};
        const Index lifecycle[]={frame.lifecycle.leftCable1E0,frame.lifecycle.rightCable1E4};
        for(unsigned side=0;side<2;++side){
            const auto endpoint=rig.find(Rig::chariot,ordinary[side]);
            need(endpoint!=Index::none&&endpoint==lifecycle[side],"Roster cable lifecycle source binding");
            bindings_.chariot[side]=rig.find(Rig::chariot,display[side]);
            if(bindings_.chariot[side]==Index::none)bindings_.chariot[side]=endpoint;
        }
        bindings_.engine[0]=rig.find(Rig::left,"enginecableleft1");
        bindings_.engine[1]=rig.find(Rig::right,"enginecableright1");
        need(bindings_.engine[0]!=Index::none&&bindings_.engine[1]!=Index::none,"Roster cable engine anchors unavailable");
        const auto psm=visual_detail::original(rig,"textures/cable.psm");
        const auto pst=visual_detail::original(rig,"textures/cable.pst");
        // All eight actual RES members share these authenticated originals.
        // Decode this racer's exact bytes; do not read Pod01 as a fallback.
        need(pod_materials50ed_fixture::detail::sha256(psm)==
            "701039c97a0f4b74a65e90fdb1e386b40cbbb17bcedb2347196884234d0eee33",
            "Roster cable material differs from supported source");
        a::Material48E material;a::DecodeError48C decode{};
        need(a::decodeMaterial48E(psm,material,decode),"Roster cable material decode");
        need(material.passes.size()==1&&material.passes[0].pass==1&&
            material.passes[0].microprogram=="StripOptFogCode"&&
            material.passes[0].textures.size()==1&&material.passes[0].textures[0].slot==1&&
            material.passes[0].textures[0].name=="cable","Roster cable source shader/texture binding");
        std::vector<std::uint8_t> texture;std::string textureError;
        need(source_cable_texture50fc_fixture::decodeCableTexture50FC(pst,texture,textureError),
            textureError.empty()?"Roster cable original texture decode":textureError);
        constexpr std::uint64_t identity=0x50fd00000001ull;
        const a::MaterialTextureAssetInput50EA textureInput{identity,&texture};
        const a::MaterialTextureSlotInput50EA slot{{identity,0,1,1},identity,a::MaterialTextureRequirement50EA::required};
        const a::MaterialTextureAssetsInput50EA input{&textureInput,1,&slot,1,{1,1,64u*1024u,64u*1024u,256}};
        a::MaterialTextureAssetDiagnostic50EA diagnostic{};
        need(bank_.load(input,diagnostic),"Roster cable immutable texture bank");
    }
    Cables(const Cables&)=delete;Cables& operator=(const Cables&)=delete;
    Cables(Cables&&)=delete;Cables& operator=(Cables&&)=delete;
    const a::MaterialTextureAssets50EA* bank()const noexcept{return &bank_;}
    const g::PodCableBindings50FD& bindings()const noexcept{return bindings_;}
};

// Defined against the generic source-selected builder in pod_flame50w. The
// decoded material table is temporary; PodFlameOwners50W owns all retained POD
// uniforms/bindings and is safe to register in the racer snapshot bank.
inline a::PodFlameOwners50W buildColdFlames(const Rig& rig,
    const a::VehicleDrawAssets50CR& draw,const g::VehicleFrameBindings49W& bindings){
    const auto* left=draw.scene(g::VehicleDrawComponent50CM::leftEngine);
    const auto* right=draw.scene(g::VehicleDrawComponent50CM::rightEngine);
    rig_detail::need(left&&right&&bindings.visual.left1BC==rig.roots[Rig::left]&&
        bindings.visual.right1B8==rig.roots[Rig::right],"Roster flame model/source identity differs");
    std::map<std::string,a::Material48E> decoded;
    std::vector<const a::Material48E*> materials[2];
    const a::Scene48D* scenes[]={left,right};
    for(unsigned side=0;side<2;++side){
        materials[side].reserve(scenes[side]->materials.size());
        for(const auto& name:scenes[side]->materials){
            const auto identity=rig_detail::lower(name);
            auto found=decoded.find(identity);
            if(found==decoded.end())found=decoded.emplace(identity,visual_detail::material(rig,name)).first;
            materials[side].push_back(&found->second);
        }
    }
    a::PodFlameOwners50W result{};a::PodFlameError50W diagnostic{};
    rig_detail::need(a::buildPodFlameOwners50W(*left,*right,materials[0],materials[1],
        static_cast<unsigned>(bindings.visual.left1BC)+1,
        static_cast<unsigned>(bindings.visual.right1B8)+1,result,diagnostic),
        "Roster source-selected flame owners rejected");
    return result;
}
}
