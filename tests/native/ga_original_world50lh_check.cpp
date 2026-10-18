// Proposed cold source check. Reads authentic GA.RES and APPSTART.RES only;
// no window, GL, generated asset input, geometry/image output or guest code.
// The author has not compiled or run this C++ test. Parent owns execution.
#include "ga_original_world50lh.h"
#include "ga_atm_animation_fixture.h"
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

namespace t=ga_original_world50LH;
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
    need(argc==2,"usage: ga_original_world50lh_check ORIGINAL_DISC_DIRECTORY");
    need(std::fesetround(FE_TONEAREST)==0,"Source rounding mode");
    const auto root=std::filesystem::path(argv[1]);const auto path=root/"TRACKS/GA.RES";
    const auto original=read(path);a::ResArchive48C archive;a::DecodeError48C error;
    a::DecodeLimits48C limits;limits.resource_file_bytes=t::archiveBytes;
    limits.resource_decoded_bytes=watchtower_source_fixture::decodedBytes;limits.resource_entries=970;
    need(a::decodeResource48C(original,archive,error,nullptr,limits),"Original GA archive decode");
    const auto scb=member(archive,"tracks\\ga.scb",error);a::World48F world;
    need(a::decodeWorld48F(scb,world,error),"Original GA SCB decode");
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
    need(prepared,"Original GA direct preparation");
    const auto* mesh=owner.mesh();const auto* bank=owner.bank();const auto* spans=owner.spans();
    const auto* selections=owner.selections();const auto* normals=owner.normals();const auto* projection=owner.projectionUvs();
    const auto* materials=owner.materials();const auto* textures=owner.textures();const auto* pinned=owner.independentlyPinnedImages();
    need(mesh&&bank&&spans&&selections&&normals&&projection&&materials&&textures&&pinned&&owner.archive()&&bank->loaded(),
        "Complete GA native publication");
    need(d.scenes==298&&d.placements==563&&d.collisionOnly==190&&d.surfaces==993&&d.materials==308&&
        d.selections==542&&d.secondPasses==248&&mesh->vertices.size()==144521&&mesh->indices.size()==258507&&
        spans->size()==1241&&selections->size()==542&&materials->size()==308&&textures->size()==79&&pinned->size()==79&&
        normals->size()==mesh->vertices.size()&&projection->size()==mesh->vertices.size()&&
        bank->textureCount()==79&&bank->slotCount()==542&&bank->rgbaByteCount()==4673536&&
        d.independentlyPinnedImages==5&&d.runtimeIdentityImages==74,"Exact GA static source closure");
    need(t::shared::sha256(serializeMesh(*mesh))=="b3085dd555b2987b4aaa07aa3fe06600a99195b55bc886c5d35cc5a95ba45bb0",
        "Native mesh differs from independent original-source arithmetic");
    need(t::shared::sha256(serializeProjection(*projection))=="1f6c12a9a67250626816ee10b1b707ea78da08648af1b47000759d115cbdd6ab",
        "Native projected UVs differ from independent original-source arithmetic");

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
    need(vertices==144521&&indices==258507&&surfaces==993&&secondPasses==248&&projectedVertices==22208&&scenes.size()==298,
        "Complete GA geometric/pass/projection coverage");

    // Load the actual independently authenticated APPSTART templates. The
    // cold test explicitly selects known normal runtime policy; no live Video
    // or canonical racer/world model is manufactured by this CPU check.
    a::OriginalUiArchive50KG app;a::OriginalUiArchiveDiagnostic50KG archiveDiagnostic;
    need(app.load(a::OriginalUiArchiveId50KG::appStart,read(root/"APPSTART.RES"),archiveDiagnostic),"Original APPSTART admission");
    a::OriginalUiMaterialTemplates50KG templates;a::OriginalUiMaterialDiagnostic50KG materialDiagnostic;
    need(templates.load(app,materialDiagnostic),"Original DefaultMT/ScreenMT templates");
    a::OriginalUiMaterialRuntime50KG runtime;runtime.overrideKnown=true;runtime.callbackOverrideKnown=true;
    using F=a::OriginalUiGsField50KG;const auto* defaults=templates.get(a::OriginalUiTemplate50KG::defaultMt);
    need(defaults&&defaults->gs[F::alphaTestOnOff].value==1,"Expected original DefaultMT alpha test");
    std::size_t singleSelections=0,projectedSelections=0,animatedSelections=0,flagSelections=0;
    for(std::size_t i=0;i<selections->size();++i){
        const auto& selection=selections->at(i);
        need(selection.track==track_source_fixture::Track::ga&&selection.materialIndex<materials->size()&&selection.bankSlot==i,
            "Selection lost original GA ownership");
        const auto psm=member(*owner.archive(),"textures\\"+selection.materialName+".psm",error);a::Material48E decoded;
        need(a::decodeMaterial48E(psm,decoded,error)&&selection.pass&&selection.pass<=decoded.passes.size(),"Fresh original PSM pass decode");
        const auto& pass=decoded.passes[selection.pass-1];const auto* slot=bank->slot(selection.bankSlot);
        need(selection.materialKey==t::shared::sha256(std::string("rr-native-material-48E-v1\n")+t::shared::sha256(psm))&&
            selection.program==pass.microprogram&&selection.textureName==pass.textures.at(0).name&&
            selection.sourcePassFields==t::detail::fields(pass.fields)&&selection.passCount==decoded.passes.size()&&
            slot&&slot->key.assetIdentity==selection.assetIdentity&&slot->key.materialIndex==selection.materialIndex&&
            slot->key.pass==selection.pass&&slot->key.slot==1&&slot->textureIndex==selection.textureIndex,
            "Original PSM/pass/bank-slot provenance");
        if(selection.projectedPrelit){
            ++projectedSelections;need(selection.texGenMatrix==track::detail::projection(t::detail::fields(pass.fields)),"Original two-pass matrix source");
        }else ++singleSelections;
        alpha::Policy policy;need(owner.alpha(i,templates,runtime,policy),"Original GA alpha policy rejected");
        need(policy.depthEqual,"Original GA reversed-depth equality policy");
        if(selection.projectedPrelit)need(policy.unlit&&policy.sourcePrelit&&!policy.sourceAlphaTest&&
            policy.blend==(selection.pass==1?alpha::Blend::opaque:alpha::Blend::sourceAlpha),"Original GA projected two-pass alpha policy");
        if(selection.staticAnimationFrame){
            ++animatedSelections;const auto name=t::detail::key(selection.textureName);const auto* pin=t::detail::sourceImage(selection.decodedSourceMember);
            need(pin&&t::detail::initialFrame(pass.fields)==pin->initialFrame,"Original ATM material start association");
            if(name=="S_FLAGA"||name=="S_FLAGB"||name=="S_FLAGC"){
                ++flagSelections;need(t::detail::field(pass.fields,"alphaTestOnOff").empty()&&policy.sourceAlphaTest&&
                    policy.sourceAlphaReference==defaults->gs[F::alphaReferenceValue].value,"GA flags lost actual DefaultMT alpha inheritance");
            }
        }
    }
    need(singleSelections==74&&projectedSelections==468&&animatedSelections==4&&flagSelections==3,"Original GA unique material pass/ATM closure");
    alpha::Policy sentinel;sentinel.alphaScale=17.25f;const auto policyBefore=snapshot(sentinel);
    a::OriginalUiMaterialTemplates50KG missingTemplates;
    need(!owner.alpha(0,missingTemplates,runtime,sentinel)&&snapshot(sentinel)==policyBefore,"Missing DefaultMT changed alpha policy");
    auto unknownRuntime=runtime;unknownRuntime.overrideKnown=false;
    need(!owner.alpha(0,templates,unknownRuntime,sentinel)&&snapshot(sentinel)==policyBefore,"Unknown runtime mode changed alpha policy");
    unknownRuntime=runtime;unknownRuntime.override435670=true;
    need(!owner.alpha(0,templates,unknownRuntime,sentinel)&&snapshot(sentinel)==policyBefore,"Unjoined runtime override changed alpha policy");
    need(!owner.alpha(selections->size(),templates,runtime,sentinel)&&snapshot(sentinel)==policyBefore,"Out-of-range alpha selection changed policy");

    std::size_t independentCount=0;
    for(std::size_t i=0;i<textures->size();++i){
        const auto& selected=textures->at(i);const auto* live=bank->texture(i);need(live&&selected.bankIndex==i&&selected.identity==live->identity,"Image bank identity/order");
        a::TextureMip48E level;level.width=live->texture.width;level.height=live->texture.height;level.rgba=live->texture.rgba;t::Bytes image;
        need(a::encodeTextureLevel48E(level,image,error)&&t::shared::sha256(image)==selected.sha256,"Recorded decoded runtime image identity");
        bool hasPin=false;for(const auto& pin:source_texture50fc_fixture::detail::gaEntries)
            if(t::detail::key(pin.member)==t::detail::key(selected.path)){hasPin=true;need(selected.sha256==pin.outputSha256,"Existing independent static output pin");}
        need(pinned->at(i)==(hasPin?1:0),"Computed image digest mislabeled as an independent pin");independentCount+=hasPin;
    }
    need(independentCount==5,"GA independently pinned live image count");

    // All nine already pinned exceptional images, including separate dynamic
    // source pixels. This does not register or activate any dynamic objects.
    std::size_t exceptional=0,exceptionalAtm=0,exceptionalRgba=0;
    for(const auto& pin:source_texture50fc_fixture::detail::gaEntries){
        const auto source=member(*owner.archive(),pin.member,error);t::Bytes image;std::uint32_t mip=99;std::string why;
        need(t::decodeSourceTexture(pin.member,source,image,mip,why)&&mip==pin.mipIndex&&
            image.size()==std::size_t(pin.width)*pin.height*4+16&&t::shared::sha256(image)==pin.outputSha256,
            "Original exceptional GA pixel callback: "+why);
        ++exceptional;exceptionalAtm+=t::detail::key(pin.member).ends_with(".ATM");exceptionalRgba+=image.size()-16;
        const auto prior=image;const auto oldMip=mip;auto corrupt=source;corrupt.back()^=1;
        need(!t::decodeSourceTexture(pin.member,corrupt,image,mip,why)&&image==prior&&mip==oldMip&&!why.empty(),"Corrupt exceptional source replaced pixels");
        need(!t::decodeSourceTexture("TEXTURES/NOT_AUTHORED.PST",source,image,mip,why)&&image==prior&&mip==oldMip&&!why.empty(),"Unknown source replaced pixels");
        need(!t::decodeSourceTexture(nullptr,source,image,mip,why)&&image==prior&&mip==oldMip&&!why.empty(),"Null source replaced pixels");
    }
    need(exceptional==9&&exceptionalAtm==4&&exceptionalRgba==491520,"Nine exact exceptional GA source/image pins");

    ga_atm_animation_fixture::Owner atm;ga_atm_animation_fixture::Diagnostic ad;
    const bool animated=atm.load(path,owner,ad); // The generic direct-static-owner overload.
    if(!animated)std::cerr<<"ATM animation="<<ad.animation<<" frame="<<ad.frame<<" message="<<ad.message<<" decode="<<ad.source.code<<'\n';
    need(animated&&atm.bank()&&atm.animations()&&atm.selectedFrames()&&atm.bank()->textureCount()==35&&
        atm.bank()->rgbaByteCount()==2293760&&atm.animations()->size()==57&&atm.selectedFrames()->size()==57,
        "Original GA 35-frame/57-placement ATM closure");
    const std::array<const char*,4> names{{"ARROWRED","S_FLAGA","S_FLAGB","S_FLAGC"}};
    const std::array<std::uint32_t,4> starts{{0,1,5,3}},firstFrames{{0,5,15,25}},counts{{5,10,10,10}},placed{{20,5,22,10}};
    const std::array<float,4> rates{{5.f,14.f,12.f,15.f}};std::array<std::uint32_t,4> actualPlaced{};
    std::set<std::pair<std::uint32_t,std::uint32_t>> uniqueInstances;std::set<std::uint32_t> uniqueSpans;
    for(std::size_t i=0;i<atm.animations()->size();++i){
        const auto& animation=atm.animations()->at(i);need(animation.sourceSpanIndex<spans->size()&&animation.materialSelection<selections->size(),"ATM span association range");
        const auto& selection=selections->at(animation.materialSelection);const auto& span=spans->at(animation.sourceSpanIndex);
        const auto name=t::detail::key(selection.textureName);std::size_t kind=0;while(kind<names.size()&&name!=names[kind])++kind;
        need(kind<names.size(),"Unknown original GA ATM attachment");++actualPlaced[kind];
        need(animation.initialFrame==starts[kind]&&animation.firstTextureIndex==firstFrames[kind]&&animation.frameCount==counts[kind]&&
            animation.framesPerSecond==rates[kind]&&atm.selectedFrames()->at(i)==starts[kind]&&
            span.materialSelection==animation.materialSelection&&span.worldInstanceIndex==animation.worldInstanceIndex&&
            span.sourcePass==1&&span.sourcePassCount==1&&!span.projectionPass&&animation.sourceTextureIndex==selection.textureIndex&&
            uniqueInstances.emplace(animation.worldInstanceIndex,animation.materialSelection).second&&uniqueSpans.insert(animation.sourceSpanIndex).second,
            "Original GA ATM start/rate/unique placement ownership");
        const auto* start=atm.bank()->texture(animation.firstTextureIndex+animation.initialFrame);const auto* staticImage=bank->texture(animation.sourceTextureIndex);
        need(start&&staticImage&&start->texture.width==staticImage->texture.width&&start->texture.height==staticImage->texture.height&&
            start->texture.rgba==staticImage->texture.rgba,"ATM source start image differs from direct static image");
    }
    need(actualPlaced==placed,"Original GA ATM per-source placement counts");
    const auto initialFrames=*atm.selectedFrames();need(atm.advance(1000)&&*atm.selectedFrames()==initialFrames,"First source timestamp changed authored starts");
    need(atm.advance(1100),"Original GA ATM source timestamp advance");const auto advancedFrames=*atm.selectedFrames();bool anyAdvanced=false;
    for(std::size_t i=0;i<advancedFrames.size();++i){need(advancedFrames[i]<atm.animations()->at(i).frameCount,"ATM selected frame bounds");anyAdvanced|=advancedFrames[i]!=initialFrames[i];}
    need(anyAdvanced&&atm.advance(1100)&&*atm.selectedFrames()==advancedFrames,"Repeated source timestamp advanced twice");
    need(std::fesetround(FE_DOWNWARD)==0,"ATM negative rounding setup");const bool badAdvance=atm.advance(1200);
    need(std::fesetround(FE_TONEAREST)==0,"ATM restore rounding");need(!badAdvance&&*atm.selectedFrames()==advancedFrames,"Failed ATM update changed frames");
    need(atm.advance(1200),"ATM retry after rejected update");atm.reset();need(*atm.selectedFrames()==initialFrames,"Restart lost 0/1/5/3 source starts");
    need(atm.advance(50000)&&*atm.selectedFrames()==initialFrames&&atm.advance(50000)&&*atm.selectedFrames()==initialFrames,
        "First post-reset/repeated timestamp consumed an unowned clock gap");
    const auto* firstTexture=bank->texture(0);const auto* atmBank=atm.bank();
    need(!owner.load(original,world,d)&&owner.mesh()==mesh&&owner.bank()==bank&&bank->texture(0)==firstTexture,
        "Reload replaced published original GA world");
    need(!atm.load(path,owner,ad)&&atm.bank()==atmBank&&*atm.selectedFrames()==initialFrames,
        "Reload replaced published original GA ATM state");
    std::cout<<"original GA: 298 PSGs; 563 visual + 190 collision-only placements; 308 PSMs / 542 passes; "
        <<"993 surfaces / 1241 spans / 248 second passes; 144521 vertices / 258507 indices; "
        <<"79 images (5 independent pins + 74 runtime identities); all 9 exceptional outputs; "
        <<"35 ATM frames / 57 placements, starts 0/1/5/3; provenance negatives passed; no generated inputs or GUI\n";
    return 0;
}catch(const std::exception& error){std::fesetround(FE_TONEAREST);std::cerr<<error.what()<<'\n';return 1;}}
