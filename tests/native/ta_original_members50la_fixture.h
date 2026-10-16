#pragma once
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/original_standard_model_resource50la.h"
#include "../../src/assets/original_standard_material_source50la.h"
#include "../../src/assets/original_standard_texture_resource50la.h"

// Cold source-member adapter for two WHOLE original TA models. The caller must
// first authenticate the exact TA.RES and asset-selection.json with the existing
// track fixture pins. This adapter performs no IO and mints no Application,
// class-registry, model-prefix, material-reference or loaded-texture authority.
// Its readers run only when a future native caller invokes prepare().
namespace ta_original_members50la_fixture {
namespace a=rr::assets;
namespace shared=pod_materials50ed_fixture::detail;
using Bytes=std::vector<std::uint8_t>;
using Json=nlohmann::json;

struct Target {
    const char* modelMember;
    const char* modelSha;
    const char* sceneKey;
    const char* materialName;
    const char* materialMember;
    const char* materialSha;
    const char* materialKey;
    std::uint32_t materialOffset;
    const char* textureMember;
    const char* textureSha;
    const char* textureKey;
    std::uint32_t textureOffset;
    std::uint32_t worldInstance,vertices,triangles;
    bool worldSpace;
};
inline constexpr std::array<Target,2> targets{{
    {"geometry\\ta_bigrocka.psg",
     "f66b3bb55b7fadd4c855c42376d0862f2de1824ec932d49428ace548e4eeaaaa",
     "e0bc97e80c701b4fc620b8458f029c471ef81518163558ab51fe3d6908e085ec",
     "ta_rock2","textures\\ta_rock2.psm",
     "831e0d8474fbc95f84bad538c6681408965056868759360de86475fe814563b0",
     "269c14352b6505cf0a109bb6660de38ece192973767adb5ca3db9b39c91f75bf",9314304u,
     "textures\\ta_rock2.pst",
     "74e54a341dd316f67185dc49ce52b3ff62f778aad0f3276b6eb452528c30cb27",
     "067539ffb7368421cb274927f195048fea75d511cdbe85df6f24ff6e7808f222",10715136u,
     0u,73u,41u,false},
    {"geometry\\zone01groundstrips16.psg",
     "34135fefe81be7a98797d64cb3068feebdd7ee063cd5ccd34c42fb635dc23e4e",
     "dbfb0d3142aae1ad77e39d0aa3209826e5d624da4e9751255139561fcf831149",
     "ta_sand8","textures\\ta_sand8.psm",
     "d31a59956f5792c58740a72905c229cabb9f6e4c06e36b7f174a55c77cb3953a",
     "2c47cd9b4d487f3941c679c7afb77f0a8aeac2152548cc1eade2ab4d81d7cb13",9560064u,
     "textures\\ta_sand8.pst",
     "d43ee9e14e4bf4033248b298b4ff55f428394c77cf562a7b90f2ed22e849f63a",
     "e1311f3ad10f19eacdf3db4101de85b75b4f35fba164c17ab55c6e283cfe1e00",11599872u,
     309u,53u,25u,true}
}};

// Selection paths use the fixture's slash/case-normalized comparison domain.
// Original resource/provider names below retain their distinct exact canonical
// backslash spelling. A basename is never an accepted target identity.
inline int targetIndex(const std::string& member) {
    const auto key=shared::normalized(member);
    for(std::size_t i=0;i<targets.size();++i)
        if(key==shared::normalized(targets[i].modelMember))return static_cast<int>(i);
    return -1;
}

namespace detail {
inline const a::ResourceEntry48C& entry(const a::ResArchive48C& archive,const char* member) {
    const a::ResourceEntry48C* result=nullptr;
    const auto wanted=shared::normalized(member);
    for(const auto& candidate:archive.entries)if(shared::normalized(candidate.name)==wanted){
        shared::need(!result,"TA original member is ambiguous");result=&candidate;
    }
    shared::need(result&&result->name==member,"TA original member canonical spelling mismatch");
    shared::need(result->bytes&&result->bytes<=8u*1024u*1024u&&
        std::uint64_t(result->logical_offset)+result->bytes<=archive.decoded.size(),
        "TA original member directory extent mismatch");
    return *result;
}
inline Bytes extract(const a::ResArchive48C& archive,const char* member,const char* digest) {
    const auto& source=entry(archive,member);Bytes bytes;a::DecodeError48C error;
    const bool okay=a::extractResMember48C(archive,member,bytes,error,source.bytes);
    shared::need(okay,"TA exact original member extraction failed: "+error.code+
        " offset="+std::to_string(error.offset)+" detail="+error.detail);
    shared::need(bytes.size()==source.bytes&&shared::sha256(bytes)==digest,
        "TA exact original member hash/extent mismatch");
    return bytes;
}
inline const Json& unique(const Json& rows,const char* field,const char* value) {
    shared::need(rows.is_array(),"TA original selection row array missing");
    const Json* result=nullptr;
    for(const auto& row:rows)if(row.at("container")=="TRACKS/TA.RES"&&row.at(field)==value){
        shared::need(!result,"TA original selected row is ambiguous");result=&row;
    }
    shared::need(result,"TA original selected row is missing");return *result;
}
inline const Json& resolution(const a::ResArchive48C& archive,const Json& rows,
    const Target& target,bool texture) {
    const auto& row=unique(rows,"name",target.materialName);
    const auto* member=texture?target.textureMember:target.materialMember;
    const auto* digest=texture?target.textureSha:target.materialSha;
    const auto* key=texture?target.textureKey:target.materialKey;
    const auto offset=texture?target.textureOffset:target.materialOffset;
    const std::uint32_t bytes=texture?88412u:261u;
    const auto* status=texture?"converted_texture":"converted_material";
    const auto& selected=row.at("selected");const auto& candidates=row.at("candidates");
    shared::need(row.at("status")=="resolved"&&row.at("scope")=="local"&&
        row.at("local_candidates")==1&&row.at("expected_status")==status&&
        row.at("requested_member")==shared::normalized(member)&&row.at("key")==key&&
        candidates.is_array()&&candidates.size()==1&&candidates.at(0)==selected,
        "TA original local member selection mismatch");
    shared::need(selected.at("container")=="TRACKS/TA.RES"&&selected.at("member")==member&&
        selected.at("key")==key&&selected.at("source_sha256")==digest&&
        selected.at("status")==status&&selected.at("source_bytes")==bytes&&
        selected.at("logical_offset")==offset,"TA original selected member identity mismatch");
    const auto& directory=entry(archive,member);
    shared::need(directory.bytes==bytes&&directory.logical_offset==offset,
        "TA original selection differs from authenticated archive directory");
    bool requested=false;
    for(const auto& requester:row.at("requested_by")){
        const bool matches=texture?
            requester.at("material_key")==target.materialKey&&
                requester.at("material_name")==target.materialName&&requester.at("pass")==1&&
                requester.at("slot")==1&&requester.at("program")=="PreLitCompressed":
            requester.at("scene_key")==target.sceneKey&&requester.at("scene_member")==target.modelMember;
        if(matches){shared::need(!requested,"TA original requester is ambiguous");requested=true;}
    }
    shared::need(requested,"TA original material/texture requester missing");
    return row;
}
}

class Sources final {
    bool prepared_=false;
    std::array<std::shared_ptr<const a::OriginalStandardModelResource50LA>,2> models_{};
    std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials_;
    std::vector<a::OriginalStandardTextureSourceInput50LA> textures_;
public:
    Sources()=default;
    Sources(const Sources&)=delete;
    Sources& operator=(const Sources&)=delete;
    bool prepared()const noexcept{return prepared_;}
    static int targetIndex(const std::string& member){return ta_original_members50la_fixture::targetIndex(member);}
    std::shared_ptr<const a::OriginalStandardModelResource50LA> model(std::size_t index)const noexcept {
        return prepared_&&index<models_.size()?models_[index]:nullptr;
    }
    const std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>>& materials()const noexcept{return materials_;}
    const std::vector<a::OriginalStandardTextureSourceInput50LA>& textures()const noexcept{return textures_;}

    // Transactional cold preparation. The already-authenticated selection
    // identifies original source members, never substitutes converted RRTEX,
    // Material48E or RRSCENE bytes for their actual PST/PSM/PSG payloads.
    bool prepare(const a::ResArchive48C& archive,const Json& selection,std::string& error)noexcept {
        const char* stage="input";
        const char* member="";
        try{
            error.clear();shared::need(!prepared_,"TA original sources already prepared");
            shared::need(selection.at("schema")=="RR_VISUAL_SELECTION49G_V1"&&
                selection.at("status")=="complete"&&archive.entries.size()==383,
                "TA original source authentication prerequisite mismatch");
            std::array<std::shared_ptr<const a::OriginalStandardModelResource50LA>,2> models{};
            std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials;
            std::vector<a::OriginalStandardTextureSourceInput50LA> textures;
            materials.reserve(targets.size());textures.reserve(targets.size());
            for(std::size_t i=0;i<targets.size();++i){
                const auto& target=targets[i];
                stage="scene_selection";member=target.modelMember;
                const auto& row=detail::unique(selection.at("scene_requests"),"member",target.modelMember);
                const auto& native=row.at("native");
                shared::need(row.at("manifest_member")==target.modelMember&&row.at("key")==target.sceneKey&&
                    row.at("status")=="converted_geometry"&&row.at("source_sha256")==target.modelSha&&
                    native.at("source_sha256")==target.modelSha&&native.at("key")==target.sceneKey&&
                    native.at("converter")=="rr-native-scene-48D-v1"&&
                    native.at("nodes")==1&&native.at("lods")==1&&native.at("surfaces_all_lods")==1&&
                    native.at("vertices")==target.vertices&&native.at("triangles")==target.triangles&&
                    native.at("world_space")==target.worldSpace&&
                    native.at("materials")==Json::array({target.materialName})&&
                    row.at("world_instance_indices")==Json::array({target.worldInstance}),
                    "TA original whole-model selection mismatch");
                bool instanceFound=false;
                for(const auto& instance:selection.at("visual_instances"))
                    if(instance.at("world_instance_index")==target.worldInstance){
                        shared::need(!instanceFound&&instance.at("container")=="TRACKS/TA.RES"&&
                            instance.at("geometry_member")==target.modelMember&&instance.at("key")==target.sceneKey&&
                            instance.at("status")=="converted_geometry","TA original model instance mismatch");
                        instanceFound=true;
                    }
                shared::need(instanceFound,"TA original model instance absent");
                // The retained scene row has no source_bytes field. Its exact
                // original extent is the authenticated archive directory's,
                // and the separate pinned digest authenticates the whole PSG.
                stage="model_extract";
                const auto modelBytes=detail::extract(archive,target.modelMember,target.modelSha);
                auto model=std::make_shared<a::OriginalStandardModelResource50LA>();
                a::OriginalStandardResourceDiagnostic50LA modelDiagnostic;
                stage="model_prepare";
                const bool modelOkay=model->prepare(target.modelMember,modelBytes,modelDiagnostic);
                shared::need(modelOkay,"TA original PSG owner rejected status="+
                    std::to_string(static_cast<unsigned>(modelDiagnostic.status))+
                    " sourcePC="+std::to_string(modelDiagnostic.sourcePC)+
                    " hierarchy_code="+std::to_string(static_cast<unsigned>(modelDiagnostic.hierarchy.code))+
                    " hierarchy_offset="+std::to_string(modelDiagnostic.hierarchy.byteOffset)+
                    " geometry_code="+modelDiagnostic.geometry.code+
                    " geometry_offset="+std::to_string(modelDiagnostic.geometry.offset)+
                    " geometry_detail="+modelDiagnostic.geometry.detail);
                stage="model_closure";
                const auto* scene=model->scene();
                shared::need(model->nodeCount()==1&&scene&&scene->nodes.size()==1&&
                    scene->materials==std::vector<std::string>{target.materialName}&&
                    scene->lods.size()==1&&scene->lods[0].surfaces.size()==1&&
                    scene->surfaces_all_lods==1&&scene->world_space==target.worldSpace&&
                    scene->lods[0].surfaces[0].material_index==0&&
                    scene->lods[0].surfaces[0].vertices.size()==target.vertices&&
                    scene->lods[0].surfaces[0].indices.size()==target.triangles*3u,
                    "TA original PSG whole-model closure rejected");
                models[i]=std::move(model);
                stage="material_resolution";member=target.materialMember;
                detail::resolution(archive,selection.at("material_resolution"),target,false);
                stage="material_extract";
                const auto materialBytes=detail::extract(archive,target.materialMember,target.materialSha);
                auto material=std::make_shared<a::OriginalStandardMaterialSource50LA>();
                a::OriginalUiPageDiagnostic50KG materialDiagnostic;
                stage="material_prepare";
                const bool materialOkay=material->prepare(target.materialMember,materialBytes,materialDiagnostic);
                shared::need(materialOkay,"TA original whole PSM source rejected error="+
                    std::to_string(static_cast<unsigned>(materialDiagnostic.error))+
                    " offset="+std::to_string(materialDiagnostic.byteOffset)+
                    " sourcePC="+std::to_string(materialDiagnostic.sourcePC));
                materials.push_back(std::move(material));
                stage="texture_resolution";member=target.textureMember;
                detail::resolution(archive,selection.at("texture_resolution"),target,true);
                stage="texture_extract";
                textures.push_back({target.textureMember,
                    detail::extract(archive,target.textureMember,target.textureSha)});
            }
            models_=std::move(models);materials_=std::move(materials);textures_=std::move(textures);
            prepared_=true;return true;
        }catch(const std::exception& failure){
            try{error=std::string("stage=")+stage+" member="+member+": "+failure.what();}catch(...){}
            return false;
        }catch(...){
            try{error=std::string("stage=")+stage+" member="+member+": TA original source preparation failed";}catch(...){}
            return false;
        }
    }
};
}
