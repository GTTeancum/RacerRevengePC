// Proposed cold source check. Reads authentic TA.RES only;
// no window, GL, generated asset input, geometry/image output or guest code.
// The author has not compiled or run this C++ test. Parent owns execution.
#include "ta_original_world50le.h"

#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <stdexcept>

namespace t=ta_original_world50LE;
namespace a=rr::assets;
namespace track=track_materials50fa_fixture;
namespace alpha=material_alpha50gx_fixture;
namespace {
void need(bool okay,const std::string& why){if(!okay)throw std::runtime_error(why);}
t::Bytes read(const std::filesystem::path& path){
    std::ifstream input(path,std::ios::binary|std::ios::ate);
    need(bool(input),"Original archive unavailable: "+path.string());
    const auto size=input.tellg();need(size>0&&size<=64*1024*1024,"Original archive read bound");
    t::Bytes bytes(static_cast<std::size_t>(size));input.seekg(0);
    input.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
    need(bool(input)&&input.peek()==std::char_traits<char>::eof(),"Exact original archive read");return bytes;
}
t::Bytes member(const a::ResArchive48C& archive,const std::string& requested,a::DecodeError48C& error){
    const auto name=t::detail::key(requested);const a::ResourceEntry48C* found=nullptr;
    for(const auto& entry:archive.entries)if(t::detail::key(entry.name)==name){need(!found,"Ambiguous source member");found=&entry;}
    need(found,"Missing original member "+requested);t::Bytes bytes;
    need(a::extractResMember48C(archive,found->name,bytes,error,found->bytes)&&bytes.size()==found->bytes,
        "Original member extraction "+requested);return bytes;
}
bool sameFloat(float x,float y){return !std::memcmp(&x,&y,sizeof(float));}
void put32(t::Bytes& bytes,std::uint32_t word){for(unsigned i=0;i<4;++i)bytes.push_back(static_cast<std::uint8_t>(word>>(8*i)));}
void putFloat(t::Bytes& bytes,float value){std::uint32_t word=0;std::memcpy(&word,&value,sizeof(word));put32(bytes,word);}
t::Bytes serializeMesh(const a::Mesh48B& mesh){
    t::Bytes out{'R','R','M','E','S','H','0','1'};put32(out,static_cast<std::uint32_t>(mesh.vertices.size()));put32(out,static_cast<std::uint32_t>(mesh.indices.size()));
    out.reserve(16+24*mesh.vertices.size()+4*mesh.indices.size());
    for(const auto& v:mesh.vertices){for(float x:v.position)putFloat(out,x);for(float x:v.uv)putFloat(out,x);for(auto x:v.color)out.push_back(x);}
    for(auto i:mesh.indices)put32(out,i);return out;
}
t::Bytes serializeProjection(const std::vector<a::TrackProjectedUv>& rows){
    t::Bytes out;out.reserve(rows.size()*16);for(const auto& row:rows)for(float x:row.uv)putFloat(out,x);return out;
}
template<class T> auto snapshot(const T& value){std::array<unsigned char,sizeof(T)> bytes{};std::memcpy(bytes.data(),&value,sizeof(T));return bytes;}
}

int main(int argc,char** argv){try{
    need(argc==2,"usage: ta_original_world50le_check ORIGINAL_DISC_DIRECTORY");
    need(std::fesetround(FE_TONEAREST)==0,"Source rounding mode");
    const auto root=std::filesystem::path(argv[1]);const auto path=root/"TRACKS/TA.RES";
    const auto original=read(path);a::ResArchive48C archive;a::DecodeError48C error;
    a::DecodeLimits48C limits;limits.resource_file_bytes=t::archiveBytes;
    limits.resource_decoded_bytes=13099008;limits.resource_entries=383;
    need(a::decodeResource48C(original,archive,error,nullptr,limits),"Original TA archive decode");
    const auto scb=member(archive,"tracks\\ta.scb",error);a::World48F world;
    need(a::decodeWorld48F(scb,world,error),"Original TA SCB decode");
    t::Owner owner;t::Diagnostic d;
    const auto empty=[&](){return !owner.loaded()&&!owner.mesh()&&!owner.normals()&&!owner.bank()&&
        !owner.spans()&&!owner.selections()&&!owner.textures()&&!owner.materials()&&!owner.projectionUvs()&&
        !owner.independentlyPinnedImages()&&!owner.archive();};
    const auto reject=[&](const t::Bytes& bytes,const a::World48F& candidate,const char* why){
        need(!owner.load(bytes,candidate,d)&&empty(),why);
    };
    auto damaged=original;damaged.back()^=1;reject(damaged,world,"Damaged archive published a world");
    damaged.pop_back();reject(damaged,world,"Truncated archive published a world");
    auto foreign=world;foreign.parameters.original_bytes.back()^=1;
    reject(original,foreign,"Different original SCB buffer published a world");
    foreign=world;foreign.instances.front().matrix[12]+=1;
    reject(original,foreign,"Changed typed transform published a world");

    // Regression for a semantically consistent forged parsed document whose
    // retained original byte vector still claims the exact authenticated SCB.
    foreign=world;auto& changed=foreign.parameters.blocks.at(foreign.instances.front().parameter_block);
    bool changedPosition=false;for(auto& field:changed.fields)if(field.name=="position"){
        field.value=field.value=="0,0,0"?"1,0,0":"0,0,0";changedPosition=true;
    }
    need(changedPosition,"Missing original model position field");t::Bytes changedScb;
    need(a::encodeParameters48F(foreign.parameters,changedScb,error)&&changedScb!=scb,"Forge parameter regression setup");
    need(a::decodeWorld48F(changedScb,foreign,error),"Forge typed world regression setup");
    foreign.parameters.original_bytes=scb;
    need(a::validateWorldSemantics48F(foreign,error),"Forged parsed/typed world should expose old admission gap");
    reject(original,foreign,"Changed parsed fields plus matching typed semantics published a world");
    foreign=world;std::swap(foreign.instances[0],foreign.instances[1]);
    need(a::validateWorldSemantics48F(foreign,error),"Instance permutation regression setup");
    reject(original,foreign,"Permuted typed instance order published a world");
    foreign=world;foreign.instances.front().zone=2;
    reject(original,foreign,"Changed instance zone published a world");
    foreign=world;++foreign.instances.front().ordinal;
    reject(original,foreign,"Changed instance ordinal published a world");
    foreign=world;++foreign.instances.front().source_block_offset;
    reject(original,foreign,"Changed instance source offset published a world");
    foreign=world;std::swap(foreign.zones[0],foreign.zones[1]);
    reject(original,foreign,"Permuted typed zone order published a world");
    foreign=world;std::swap(foreign.portals[0],foreign.portals[1]);
    reject(original,foreign,"Permuted typed portal order published a world");
    foreign=world;++foreign.parameters.blocks.front().name_offset;
    reject(original,foreign,"Changed parsed block provenance published a world");
    foreign=world;++foreign.parameters.blocks.front().fields.front().entry_offset;
    reject(original,foreign,"Changed parsed field provenance published a world");
    need(std::fesetround(FE_DOWNWARD)==0,"Negative rounding setup");
    const bool wrongRounding=owner.load(original,world,d);
    need(std::fesetround(FE_TONEAREST)==0,"Restore source rounding");
    need(!wrongRounding&&empty(),"Wrong rounding mode published a world");

    const bool prepared=owner.load(original,world,d);
    if(!prepared)std::cerr<<"stage="<<d.stage<<" member="<<d.member<<" message="<<d.message
        <<" decode="<<d.decode.code<<" bank="<<static_cast<unsigned>(d.bank.error)<<'\n';
    need(prepared,"Original TA direct preparation");
    const auto* mesh=owner.mesh();const auto* bank=owner.bank();const auto* spans=owner.spans();
    const auto* selections=owner.selections();const auto* normals=owner.normals();const auto* projection=owner.projectionUvs();
    const auto* materials=owner.materials();const auto* textures=owner.textures();const auto* pinned=owner.independentlyPinnedImages();
    need(mesh&&bank&&spans&&selections&&normals&&projection&&materials&&textures&&pinned&&owner.archive()&&bank->loaded(),
        "Complete TA native publication");
    need(d.scenes==228&&d.placements==284&&d.collisionOnly==34&&d.surfaces==353&&d.materials==29&&
        d.selections==29&&d.secondPasses==0&&mesh->vertices.size()==110196&&mesh->indices.size()==209514&&
        spans->size()==353&&selections->size()==29&&materials->size()==29&&textures->size()==29&&pinned->size()==29&&
        normals->size()==mesh->vertices.size()&&projection->size()==mesh->vertices.size()&&
        bank->textureCount()==29&&bank->slotCount()==29&&bank->rgbaByteCount()==4038656&&
        d.independentlyPinnedImages==29&&d.runtimeIdentityImages==0,"Exact TA static source closure");
    // Re-read each original PSG for the complete source span/normal/projection
    // cross-check. Never trust a flattened world coordinate as a TexGen input.
    std::map<std::string,a::Scene48D> scenes;std::size_t vertices=0,indices=0,surfaces=0,secondPasses=0,projectedVertices=0;
    constexpr std::array<double,16> identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
    for(std::size_t si=0;si<spans->size();){
        const auto& first=spans->at(si);
        need(first.sourcePass==1&&(first.sourcePassCount==1||first.sourcePassCount==2)&&
            first.vertexStart==vertices&&first.indexStart==indices&&first.vertexCount&&first.indexCount&&
            first.worldInstanceIndex<world.instances.size()&&first.materialSelection<selections->size(),
            "Original first-pass span order/extent");
        const auto& instance=world.instances[first.worldInstanceIndex];const auto key=t::detail::key(instance.geometry_member);
        auto found=scenes.find(key);if(found==scenes.end()){
            a::Scene48D scene;need(a::decodeScene48D(member(*owner.archive(),instance.geometry_member,error),scene,error),"Fresh original PSG decode");
            found=scenes.emplace(key,std::move(scene)).first;
        }
        const auto& scene=found->second;need(first.surfaceIndex<scene.lods.at(0).surfaces.size(),"Original surface index");
        const auto& surface=scene.lods.at(0).surfaces[first.surfaceIndex];
        const auto& firstSelection=selections->at(first.materialSelection);
        need(surface.vertices.size()==first.vertexCount&&surface.indices.size()==first.indexCount&&
            firstSelection.materialName==scene.materials.at(surface.material_index)&&
            first.hasOriginalColors==surface.has_original_colors,"Original surface/material association");
        std::array<const track::Selection*,2> selected{{nullptr,nullptr}};
        for(std::size_t pi=0;pi<first.sourcePassCount;++pi){
            need(si+pi<spans->size(),"Missing adjacent original second pass");const auto& span=spans->at(si+pi);
            need(span.sourcePass==pi+1&&span.sourcePassCount==first.sourcePassCount&&span.vertexStart==first.vertexStart&&
                span.vertexCount==first.vertexCount&&span.indexStart==first.indexStart&&span.indexCount==first.indexCount&&
                span.worldInstanceIndex==first.worldInstanceIndex&&span.surfaceIndex==first.surfaceIndex&&
                span.hasOriginalColors==first.hasOriginalColors&&span.materialSelection<selections->size(),
                "Second-pass adjacency/shared-geometry ownership");
            const auto& selection=selections->at(span.materialSelection);selected[pi]=&selection;
            need(selection.pass==span.sourcePass&&selection.passCount==span.sourcePassCount&&selection.slot==1&&
                selection.materialIndex==firstSelection.materialIndex&&selection.materialKey==firstSelection.materialKey&&
                span.textureIndex==selection.textureIndex&&bank->texture(span.textureIndex)&&
                span.projectionPass==(first.sourcePassCount==2?pi+1:0)&&
                selection.projectedPrelit==(first.sourcePassCount==2),"Original pass/projection/material continuity");
        }
        const auto& matrix=scene.world_space?identity:instance.matrix;a::WorldNormalTransform50HK normalTransform;
        need(a::prepareWorldNormalTransform50HK(matrix,normalTransform),"Fresh original normal transform");
        for(std::size_t vi=0;vi<surface.vertices.size();++vi){
            const auto& source=surface.vertices[vi];const auto index=first.vertexStart+vi;const auto& live=mesh->vertices[index];
            for(unsigned axis=0;axis<3;++axis){double value=0;
                for(unsigned k=0;k<3;++k)value+=source.position_local[k]*matrix[k*4+axis];value+=matrix[12+axis];
                need(sameFloat(live.position[axis],static_cast<float>(value)),"Original placed vertex mismatch");
            }
            for(unsigned axis=0;axis<2;++axis)need(sameFloat(live.uv[axis],static_cast<float>(source.uv[axis])),"Original source UV mismatch");
            for(unsigned axis=0;axis<4;++axis)need(live.color[axis]==(surface.has_original_colors?source.color[axis]:255),"Original source color mismatch");
            a::WorldNormal50HK normal;need(a::transformPreparedWorldNormal50HK(source.normal_local,normalTransform,normal),"Fresh source normal conversion");
            for(unsigned axis=0;axis<3;++axis)need(sameFloat(normals->at(index).xyz[axis],normal.xyz[axis]),"Original normal sidecar mismatch");
            for(unsigned pi=0;pi<2;++pi)for(unsigned axis=0;axis<2;++axis){
                float expected=static_cast<float>(source.uv[axis]);
                if(selected[pi]&&selected[pi]->projectedPrelit){double value=0;
                    for(unsigned k=0;k<3;++k)value+=(source.position_local[k]-scene.quantization[4+k])*selected[pi]->texGenMatrix[k*4+axis];
                    value+=selected[pi]->texGenMatrix[12+axis];expected=static_cast<float>(value);
                }
                need(sameFloat(projection->at(index).uv[pi*2+axis],expected),"Original pre-world-transform projected UV pair mismatch");
            }
        }
        for(std::size_t ii=0;ii<surface.indices.size();++ii)need(mesh->indices[first.indexStart+ii]==first.vertexStart+surface.indices[ii],"Original index order mismatch");
        vertices+=first.vertexCount;indices+=first.indexCount;++surfaces;
        if(first.sourcePassCount==2){++secondPasses;projectedVertices+=first.vertexCount;}
        si+=first.sourcePassCount;
    }
    need(vertices==110196&&indices==209514&&surfaces==353&&secondPasses==0&&projectedVertices==0&&scenes.size()==228,
        "Complete TA geometric/pass/projection coverage");

    for(std::size_t i=0;i<selections->size();++i){
        const auto& selection=selections->at(i);
        need(selection.track==track_source_fixture::Track::ta&&selection.materialIndex<materials->size()&&selection.bankSlot==i,
            "Selection lost original TA ownership");
        const auto psm=member(*owner.archive(),"textures\\"+selection.materialName+".psm",error);a::Material48E decoded;
        need(a::decodeMaterial48E(psm,decoded,error)&&decoded.passes.size()==1,"Fresh original one-pass PSM decode");
        const auto& pass=decoded.passes[0];const auto* slot=bank->slot(selection.bankSlot);
        need(selection.materialKey==t::shared::sha256(std::string("rr-native-material-48E-v1\n")+t::shared::sha256(psm))&&
            selection.program==pass.microprogram&&selection.textureName==pass.textures.at(0).name&&
            selection.sourcePassFields==t::detail::fields(pass.fields)&&selection.pass==1&&selection.passCount==1&&
            !selection.projectedPrelit&&slot&&slot->key.assetIdentity==selection.assetIdentity&&
            slot->key.materialIndex==selection.materialIndex&&slot->key.pass==1&&slot->key.slot==1&&
            slot->textureIndex==selection.textureIndex,"Original PSM/pass/bank-slot provenance");
        alpha::Policy policy;need(alpha::select(selection,policy),"Existing TA alpha policy rejected");
    }
    for(std::size_t i=0;i<textures->size();++i){
        const auto& texture=textures->at(i);const auto source=member(*owner.archive(),texture.path,error);
        t::Bytes image;std::uint32_t mip=99;std::string why;
        need(t::decodeSourceTexture(texture.path.c_str(),source,image,mip,why)&&mip==texture.mipIndex&&
            t::shared::sha256(image)==texture.sha256&&pinned->at(i)==1,"Exact independently pinned TA image");
        const auto prior=image;const auto oldMip=mip;auto corrupt=source;corrupt.back()^=1;
        need(!t::decodeSourceTexture(texture.path.c_str(),corrupt,image,mip,why)&&image==prior&&mip==oldMip,
            "Corrupt source replaced pixels");
    }
    // Decode all eight existing exceptional output pins from their exact source;
    // this does not register dynamic objects.
    for(const auto& pin:source_texture50fc_fixture::detail::entries){
        t::Bytes image;std::uint32_t mip=99;std::string why;
        need(t::decodeSourceTexture(pin.member,member(*owner.archive(),pin.member,error),image,mip,why)&&
            mip==pin.mipIndex&&t::shared::sha256(image)==pin.outputSha256,"Existing exceptional TA output pin");
    }
    const auto* firstTexture=bank->texture(0);
    need(!owner.load(original,world,d)&&owner.mesh()==mesh&&owner.bank()==bank&&bank->texture(0)==firstTexture,
        "Reload replaced published original TA world");
    std::cout<<"original TA: 228 PSGs; 284 visual + 34 collision-only placements; 29 one-pass PSMs; "
        <<"353 spans; 110196 vertices / 209514 indices; 29 independently pinned images / 4038656 RGBA bytes; "
        <<"all 8 exceptional outputs; original normals and provenance negatives; no generated inputs or GUI\n";
    return 0;
}catch(const std::exception& error){std::fesetround(FE_TONEAREST);std::cerr<<error.what()<<'\n';return 1;}}
