#pragma once
// Cold per-original-pod visual ownership. No GL calls, guest execution, cloned
// Pod01 meshes/materials, synthetic palettes, or per-frame allocations.
#include "native_racer_rig50hw_fixture.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../src/assets/import_texture48e.h"
#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace native_racer50HW {
namespace visual_detail {
namespace shared=pod_materials50ed_fixture::detail;
using Bytes=std::vector<std::uint8_t>;
using Json=nlohmann::json;
// Shared cold name provenance. The player may keep its existing visual owner
// while obtaining the same authenticated original display name as opponents.
inline std::string verifiedDisplayName(shared::Inputs& input,const std::filesystem::path& root,
    const Json& racer,const std::string& podId) {
    shared::need(racer.at("display_name_verified")==true,"Original UI racer name unavailable");
    const auto name=racer.at("display_name").get<std::string>();
    const auto ui=input.read(root/"name-provenance/UIACCEPT.INI",65536);
    shared::need(shared::sha256(ui)=="245b0bd530e6797637ac72bbf2de8f101f3b1da280e00e8fc24e4b4930d79f3b",
        "Original UI selection name source identity differs");
    const auto& nameSource=racer.at("display_name_source");
    const auto offset=nameSource.at("ui_comment_byte_offset").get<std::size_t>();
    shared::need(offset<ui.size(),"Original UI name source offset");
    const std::string uiText(ui.begin(),ui.end());const auto end=uiText.find('\n',offset);
    const auto comment=rig_detail::trim(uiText.substr(offset,end==std::string::npos?end:end-offset));
    shared::need(!comment.empty()&&comment.front()==';'&&rig_detail::trim(comment.substr(1))==name,
        "Original UI display-name comment differs");
    const auto speech=input.read(root/"name-provenance"/(podId+"-TAUNTS.INI"),65536);
    shared::need(shared::sha256(speech)==nameSource.at("pod_speech_sha256").get<std::string>(),
        "Original pod speech identity differs");
    const std::string speechText(speech.begin(),speech.end());
    const auto speechPrefix=rig_detail::lower(nameSource.at("speech_prefix").get<std::string>());
    shared::need(!speechPrefix.empty()&&rig_detail::lower(speechText).find(
        "speech/"+rig_detail::lower(podId)+"/"+speechPrefix)!=std::string::npos,
        "Original pod speech/name association differs");
    return name;
}
inline const char* role(unsigned component) {
    static const char* const roles[]={"Chariot","LeftEngine","RightEngine"};
    return roles[component];
}
inline std::string field(const a::MaterialFields48E& fields,const char* name) {
    for(const auto& item:fields.authored)
        if(rig_detail::lower(item.name)==rig_detail::lower(name))return item.value;
    return {};
}
inline std::string passField(const a::MaterialFields48E& fields,const char* name) {
    auto value=field(fields,name);
    if(value.empty())value=field(fields,(std::string(name)+"_1").c_str());
    return value;
}
inline Json fieldsJson(const a::MaterialFields48E& fields) {
    Json result={{"authored",Json::array()},{"unspecified",Json::array()}};
    for(const auto* list:{&fields.authored,&fields.unspecified})
        for(const auto& item:*list)
            result[list==&fields.authored?"authored":"unspecified"].push_back(
                {{"name",item.name},{"value",item.value},{"source_offset",item.source_offset}});
    return result;
}
inline void verifyMaterial(const a::Material48E& material,const Json& native) {
    shared::need(fieldsJson(material.general)==native.at("general") &&
        material.passes.size()==native.at("passes").size(),"Original material decoded metadata mismatch");
    for(std::size_t i=0;i<material.passes.size();++i) {
        const auto& pass=material.passes[i];const auto& record=native.at("passes").at(i);
        shared::need(record.at("pass")==pass.pass && record.at("program")==pass.microprogram &&
            record.at("fields")==fieldsJson(pass.fields) &&
            record.at("texture_count_defaulted")==pass.texture_count_defaulted &&
            record.at("textures").size()==pass.textures.size(),"Original shader pass metadata mismatch");
        for(std::size_t j=0;j<pass.textures.size();++j)
            shared::need(record.at("textures").at(j).at("slot")==pass.textures[j].slot &&
                record.at("textures").at(j).at("name")==pass.textures[j].name,
                "Original material texture slot metadata mismatch");
    }
}
inline rr::platform::VehicleWindowMaterialBinding50DS policy(
    const a::MaterialPass48E& pass,std::uint32_t textureIndex,bool flame) {
    using namespace rr::platform;
    VehicleWindowMaterialBinding50DS out{VehicleWindowMaterialMode50DS::requiredTexture,textureIndex};
    const auto depth=passField(pass.fields,"zWriteDisabled");
    shared::need(depth.empty()||depth=="0"||depth=="1","Unsupported original pod depth field");
    out.depthWrite=depth!="1";
    const auto blending=passField(pass.fields,"alphaBlendingOnOff");
    shared::need(blending.empty()||blending=="0"||blending=="1","Unsupported original pod blend field");
    if(flame) {
        // Existing supported host additive projection, now admitted from this
        // pod's own exact PSM fields rather than a material-index/name guess.
        const auto d=passField(pass.fields,"inputAlphaD").empty()?
            passField(pass.fields,"inputColorD"):passField(pass.fields,"inputAlphaD");
        shared::need(blending=="1" && passField(pass.fields,"inputColorA")=="0" &&
            passField(pass.fields,"inputColorB")=="2" && passField(pass.fields,"inputAlphaC")=="2" &&
            d=="1" && passField(pass.fields,"fixedAlphaValue")=="255" && depth=="1",
            "Unsupported original flame blending equation");
        out.blend=VehicleWindowBlend50DS::additive;out.fixedBlendAlpha=true;
        out.alphaScale=255.f/128.f;
    } else if(blending=="1") {
        const auto d=passField(pass.fields,"inputAlphaD").empty()?
            passField(pass.fields,"inputColorD"):passField(pass.fields,"inputAlphaD");
        shared::need(passField(pass.fields,"inputColorA")=="0" &&
            passField(pass.fields,"inputColorB")=="1" && passField(pass.fields,"inputAlphaC")=="0" && d=="1",
            "Unsupported original pod source-alpha equation");
        out.blend=VehicleWindowBlend50DS::sourceAlpha;
        out.alphaScale=(255.f/128.f)*(255.f/128.f);
    }
    return out;
}
} // namespace visual_detail

inline std::string loadVerifiedDisplayName(const std::filesystem::path& visualSelectionRoot,
    const std::string& podId,const std::string& resourceSha256) {
    namespace d=visual_detail;namespace s=visual_detail::shared;
    s::need(podId.size()==5&&podId.substr(0,3)=="POD"&&
        podId[3]>='0'&&podId[3]<='9'&&podId[4]>='0'&&podId[4]<='9'&&s::hash(resourceSha256),
        "Original name resource identity rejected");
    s::need(!std::filesystem::is_symlink(visualSelectionRoot),"Original visual root symlink rejected");
    const auto root=std::filesystem::canonical(visualSelectionRoot);
    s::Inputs input;input.root=root;input.capacity=8u*1024u*1024u;
    const auto bytes=input.read(root/"roster-visual-selection.json",4u*1024u*1024u);
    const auto catalog=d::Json::parse(bytes.begin(),bytes.end());
    s::need(catalog.at("schema")=="RR_ORIGINAL_ROSTER_VISUAL_V1"&&
        catalog.at("source_archive_sha256")=="60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c"&&
        catalog.at("source_generation")=="3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39",
        "Original visual source catalog identity");
    const d::Json* selected=nullptr;
    for(const auto& row:catalog.at("racers"))if(row.at("pod_id").get<std::string>()==podId){
        s::need(!selected,"Duplicate original pod catalog row");selected=&row;
    }
    s::need(selected&&selected->at("container_sha256").get<std::string>()==resourceSha256,
        "Original display name and resource differ");
    return d::verifiedDisplayName(input,root,*selected,podId);
}

// Construct one at its final heap address after Rig cold initialization. This
// owner copies the exact source-node/palette mappings into 50CR; Rig itself need
// not be consulted during draw preparation. All public data below is immutable
// after construction except the two explicit 50CR staging banks. Never move the
// owner, resize its vectors, alter banks, or destroy it while a batch/renderer
// borrows it. GPU ownership remains external: upload materials through a distinct
// 50EB owner and close the renderer before closing that GPU publication.
class Visual final {
public:
    a::VehicleDrawAssets50CR drawAssets;
    a::MaterialTextureAssets50EA materials;
    std::array<std::vector<pod_materials50ed_fixture::Selection>,3> textures;
    std::vector<pod_materials50ed_fixture::AuthoredSlot> authoredSlots;
    std::vector<pod_materials50ed_fixture::TextureSelection> textureSelections;
    std::string podId,displayName;

    Visual(const std::filesystem::path& repository,
           const std::filesystem::path& visualSelectionRoot,const Rig& rig) {
        namespace d=visual_detail;namespace s=visual_detail::shared;
        const auto requested=visualSelectionRoot.is_absolute()?visualSelectionRoot:repository/visualSelectionRoot;
        s::need(!std::filesystem::is_symlink(requested),"Original visual root symlink rejected");
        const auto root=std::filesystem::canonical(requested);
        s::Inputs input;input.root=root;input.capacity=64u*1024u*1024u;
        s::need(!std::filesystem::is_symlink(root/"cacheobjects"),"Original cache directory symlink rejected");
        const auto catalogBytes=input.read(root/"roster-visual-selection.json",4u*1024u*1024u);
        const auto catalog=d::Json::parse(catalogBytes.begin(),catalogBytes.end());
        s::need(catalog.at("schema")=="RR_ORIGINAL_ROSTER_VISUAL_V1" &&
            catalog.at("source_archive_sha256")=="60c041bd1acd58f7e19cea50b0c2f6d3ae1083f18d82bcbb591b3182d115aa0c" &&
            catalog.at("source_generation")=="3cf500075d2f31e3930d5e74896a3068ca596fba3f47475ba9c4643bcbf60b39",
            "Original visual source catalog identity");
        const auto& files=catalog.at("files");s::need(files.is_object()&&files.size()<=4096,"Original object table bound");
        for(auto it=files.begin();it!=files.end();++it) {
            s::need(s::safeObjectPath(it.key())&&s::hash(it.value().at("sha256").get<std::string>()),"Unsafe original object identity");
            auto metadata=it.value();metadata["extracted"]=true;
            s::need(input.objects.emplace(it.key(),std::move(metadata)).second,"Duplicate original object");
        }
        const d::Json* selected=nullptr;
        for(const auto& row:catalog.at("racers"))if(row.at("pod_id")==rig.podId) {
            s::need(!selected,"Duplicate original pod catalog row");selected=&row;
        }
        s::need(selected && selected->at("container_sha256")==rig.manifest.at("source_container_sha256"),
            "Original rig and visual resource differ");
        const auto& racer=*selected;podId=rig.podId;
        const std::uint64_t textureIdentityBase=
            (std::uint64_t((podId[3]-'0')*10+(podId[4]-'0'))+1u)<<32;
        displayName=d::verifiedDisplayName(input,root,racer,podId);
        std::map<std::string,const d::Json*> originals;
        for(const auto& member:rig.manifest.at("members"))
            s::need(originals.emplace(rig_detail::lower(member.at("source_member").get<std::string>()),&member).second,
                "Duplicate original imported member");
        auto original=[&](const d::Json& row)->d::Bytes {
            const auto found=originals.find(rig_detail::lower(row.at("member").get<std::string>()));
            s::need(found!=originals.end() && found->second->at("source_sha256")==row.at("source_sha256"),
                "Visual source member differs from own original resource");
            const auto path=std::filesystem::path(found->second->at("source_file").get<std::string>());
            s::need(path==path.filename()&&!path.is_absolute(),"Original imported member path");
            auto bytes=input.read(rig.directory/path,8u*1024u*1024u);
            s::need(bytes.size()==found->second->at("source_bytes").get<std::size_t>() &&
                s::sha256(bytes)==row.at("source_sha256").get<std::string>(),"Original visual member content identity");
            return bytes;
        };
        a::VehicleDrawAssetsInput50CR drawInput{};
        std::array<d::Bytes,3> sceneBytes;
        std::array<std::array<std::uint8_t,a::maxModelNodes49O>,3> palettes{};
        constexpr unsigned mapped[]={Rig::chariot,Rig::left,Rig::right};
        const auto& components=racer.at("components");s::need(components.size()==3,"Original three-component catalog");
        for(unsigned c=0;c<3;++c) {
            const auto& row=components.at(c);const unsigned model=mapped[c];
            s::need(row.at("role")==d::role(c),"Original component order differs");
            const auto& record=input.record(row.at("native").at("key").get<std::string>());
            s::need(record==row.at("native") && record.at("converter")=="rr-native-scene-48D-v1" &&
                record.at("source_sha256")==s::sha256(rig.psg[model]),"Original component PSG provenance differs");
            sceneBytes[c]=input.selected(row.at("rrscene").get<std::string>(),a::max_scene_bytes48d);
            s::need(s::sha256(sceneBytes[c])==record.at("scene_sha256").get<std::string>(),"Original component scene hash differs");
            // Authenticate the conversion against this retained source, not
            // merely against internally consistent replacement cache metadata.
            a::Scene48D freshlyDecoded;a::DecodeError48C sceneError{};
            s::need(a::decodeScene48D(rig.psg[model],freshlyDecoded,sceneError),
                "Original PSG native scene decode rejected: "+sceneError.code);
            d::Bytes freshlyEncoded;std::string sceneProblem;
            s::need(a::encodeSceneAsset48D(freshlyDecoded,freshlyEncoded,sceneProblem)&&
                freshlyEncoded==sceneBytes[c],"Original PSG/cache scene conversion differs");
            const auto count=rig.hierarchy[model].nodeCount;const auto padded=(count+3u)&~3u;
            const auto& psg=rig.psg[model];s::need(count&&count<=a::maxModelNodes49O&&psg.size()>=12u+padded,
                "Original PSG palette suffix bounds");
            const auto suffix=psg.size()-(12u+padded);
            s::need(s::u32(psg.data()+suffix)==1 && s::u32(psg.data()+suffix+4)==padded &&
                s::u32(psg.data()+suffix+8+padded)==count,"Original selected PSG palette framing");
            for(unsigned n=0;n<count;++n)palettes[c][n]=psg[suffix+8+n];
            drawInput.component[c]={&sceneBytes[c],rig.sourceNodes[model],count,palettes[c].data(),count};
        }
        a::VehicleDrawAssetDiagnostic50CR drawDiagnostic{};
        s::need(drawAssets.load(drawInput,drawDiagnostic),"Original per-pod draw asset binding rejected: "+drawDiagnostic.parserMessage);
        for(const auto& row:racer.at("materials")) {
            const auto& native=input.record(row.at("native").at("key").get<std::string>());
            s::need(native==row.at("native")&&native.at("converter")=="rr-native-material-48E-v1",
                "Original material record identity differs");
            const auto source=original(row);a::Material48E decoded;a::DecodeError48C error{};
            s::need(a::decodeMaterial48E(source,decoded,error),"Original material source decode rejected: "+error.code);
            d::verifyMaterial(decoded,native);
            const auto name=rig_detail::lower(std::filesystem::path(rig_detail::lower(row.at("member").get<std::string>())).stem().string());
            s::need(materialRecords_.emplace(name,native).second && originalMaterials_.emplace(name,std::move(decoded)).second,
                "Duplicate original material association");
        }
        std::map<std::string,const d::Json*> textureRows;
        for(const auto& row:racer.at("textures"))
            s::need(textureRows.emplace(row.at("native").at("key").get<std::string>(),&row).second,
                "Duplicate original texture key");
        std::map<std::string,std::uint32_t> textureIndices;
        std::vector<d::Bytes> textureBytes;
        std::vector<a::MaterialTextureSlotInput50EA> slots;
        std::size_t rgbaBytes=0,sourceBytes=0;
        for(unsigned c=0;c<3;++c) {
            const auto* scene=drawAssets.scene(static_cast<g::VehicleDrawComponent50CM>(c));
            s::need(scene&&scene->materials.size()<=4096,"Original component materials bound");
            textures[c].reserve(scene->materials.size());windowBindings_[c].reserve(scene->materials.size());
            for(std::uint32_t mi=0;mi<scene->materials.size();++mi) {
                const auto materialName=rig_detail::lower(scene->materials[mi]);
                const auto found=originalMaterials_.find(materialName);
                s::need(found!=originalMaterials_.end(),"Original scene material association absent");
                const auto& material=found->second;const auto& record=materialRecords_.at(materialName);
                s::need(!material.passes.empty()&&material.passes.front().pass==1,
                    "Original displayed pass1 absent");
                const auto& displayed=material.passes.front();
                const bool flame=rig_detail::lower(displayed.microprogram)=="diffusetextranscompressed" &&
                    rig_detail::lower(d::field(displayed.fields,"VertexAnimation"))=="unique";
                if(flame)s::need(c>0,"Original flame material unexpectedly attached to chariot");
                const d::Json* displayedBinding=nullptr;
                for(const auto& pass:material.passes)for(const auto& map:pass.textures) {
                    const d::Json* binding=nullptr;
                    for(const auto& candidate:racer.at("texture_bindings"))
                        if(rig_detail::lower(candidate.at("material_member").get<std::string>())=="textures/"+materialName+".psm" &&
                           candidate.at("pass_index")==pass.pass && candidate.at("slot")==map.slot) {
                            s::need(!binding,"Duplicate original texture resolution");binding=&candidate;
                        }
                    s::need(binding && rig_detail::lower(binding->at("member").get<std::string>())==
                        "textures/"+rig_detail::lower(map.name)+".pst","Original texture slot resolution absent");
                    pod_materials50ed_fixture::AuthoredSlot authored;authored.component=c;authored.materialIndex=mi;
                    authored.pass=pass.pass;authored.slot=map.slot;authored.materialName=scene->materials[mi];
                    authored.materialKey=record.at("key").get<std::string>();authored.program=pass.microprogram;authored.textureName=map.name;
                    authored.selectedForDisplay=pass.pass==1&&map.slot==1;
                    authored.resolvedInSelection=binding->at("status")=="resolved";
                    if(authored.resolvedInSelection)authored.textureKey=binding->at("texture_key").get<std::string>();
                    else authored.unresolvedReason=binding->value("reason",std::string("original_slot_unresolved"));
                    if(authored.selectedForDisplay)displayedBinding=binding;
                    authoredSlots.push_back(std::move(authored));
                }
                // Only authored pass1/map1 is displayed in this existing host
                // shader. Other passes/specular maps remain explicit metadata;
                // no substitute texture, Anakin record, or untextured fallback.
                s::need(displayedBinding&&displayedBinding->at("status")=="resolved",
                    "Required original base texture unavailable: "+materialName);
                const auto key=displayedBinding->at("texture_key").get<std::string>();
                auto known=textureIndices.find(key);
                if(known==textureIndices.end()) {
                    const auto t=textureRows.find(key);s::need(t!=textureRows.end(),"Original texture row missing");
                    const auto& row=*t->second;const auto& native=input.record(key);
                    s::need(native==row.at("native")&&native.at("converter")=="rr-native-texture-48E-v1",
                        "Original texture native record mismatch");
                    const auto member=originals.find(rig_detail::lower(row.at("member").get<std::string>()));
                    s::need(member!=originals.end()&&member->second->at("source_sha256")==row.at("source_sha256")&&
                        native.at("source_sha256")==row.at("source_sha256"),"Texture belongs to another original resource");
                    const auto& mips=row.at("mips");s::need(mips.is_array()&&!mips.empty()&&mips.size()<=7,"Original retained mip bound");
                    const auto& mip=mips.back();const auto width=s::number(mip,"width",1024),height=s::number(mip,"height",1024);
                    s::need(width&&height,"Original texture dimensions");
                    const auto path=mip.at("path").get<std::string>();
                    auto bytes=input.selected(path,static_cast<std::size_t>(width*height*4+16));
                    s::need(bytes.size()==width*height*4+16 && s::sha256(bytes)==mip.at("sha256").get<std::string>(),"Original texture mip identity differs");
                    const auto originalPst=original(row);a::Texture48E decodedTexture;a::DecodeError48C textureError{};
                    s::need(a::decodeTexture48E(originalPst,decodedTexture,textureError)&&
                        decodedTexture.base_rrtex==bytes,"Original PST/cache base texture differs");
                    const auto index=static_cast<std::uint32_t>(textureBytes.size());
                    s::need(index<32,"Original pod texture bank capacity");
                    pod_materials50ed_fixture::TextureSelection meta;meta.identity=textureIdentityBase+index+1;meta.bankIndex=index;
                    meta.mipIndex=static_cast<std::uint32_t>(s::number(mip,"index",6));
                    meta.width=static_cast<std::uint32_t>(width);meta.height=static_cast<std::uint32_t>(height);
                    meta.key=key;meta.path=path;meta.sha256=mip.at("sha256").get<std::string>();
                    textureSelections.push_back(std::move(meta));textureBytes.push_back(std::move(bytes));
                    samplers_.push_back({rr::platform::MaterialTextureFilter50EB::linear,rr::platform::MaterialTextureFilter50EB::linear,
                        rr::platform::MaterialTextureWrap50EB::repeat,rr::platform::MaterialTextureWrap50EB::repeat});
                    s::add(rgbaBytes,static_cast<std::size_t>(width*height*4),16u*1024u*1024u,"Original RGBA bank budget");
                    s::add(sourceBytes,static_cast<std::size_t>(width*height*4+16),16u*1024u*1024u,"Original texture source budget");
                    known=textureIndices.emplace(key,index).first;
                }
                const auto textureIndex=known->second;
                pod_materials50ed_fixture::Selection choice;choice.materialIndex=mi;choice.pass=choice.slot=1;
                choice.assetIdentity=c+1;choice.bankSlot=static_cast<std::uint32_t>(slots.size());
                choice.textureIndex=textureIndex;choice.textureIdentity=textureSelections[textureIndex].identity;
                choice.mipIndex=textureSelections[textureIndex].mipIndex;choice.materialName=scene->materials[mi];
                choice.materialKey=record.at("key").get<std::string>();choice.program=displayed.microprogram;
                choice.textureName=displayed.textures.front().name;choice.textureKey=key;
                choice.requirement=a::MaterialTextureRequirement50EA::required;
                slots.push_back({{c+1,mi,1,1},choice.textureIdentity,choice.requirement});
                textures[c].push_back(choice);windowBindings_[c].push_back(d::policy(displayed,textureIndex,flame));
                if(flame) {
                    flameMaterials_[c].push_back(mi);
                    s::need(d::field(displayed.fields,"wrapModeVertical")=="1",
                        "Original flame vertical clamp field unsupported");
                    samplers_[textureIndex].t=rr::platform::MaterialTextureWrap50EB::clampToEdge;
                }
                componentMaterials_[c].push_back(&material);
            }
        }
        std::vector<a::MaterialTextureAssetInput50EA> textureInput;textureInput.reserve(textureBytes.size());
        for(std::uint32_t i=0;i<textureBytes.size();++i)textureInput.push_back({textureSelections[i].identity,&textureBytes[i]});
        a::MaterialTextureAssetsInput50EA materialInput{textureInput.data(),static_cast<std::uint32_t>(textureInput.size()),
            slots.data(),static_cast<std::uint32_t>(slots.size()),
            {static_cast<std::uint32_t>(textureInput.size()),static_cast<std::uint32_t>(slots.size()),sourceBytes,rgbaBytes,1024}};
        a::MaterialTextureAssetDiagnostic50EA diagnostic{};
        s::need(materials.load(materialInput,diagnostic),"Original per-pod texture bank rejected: "+diagnostic.parserMessage);
    }
    Visual(const Visual&)=delete;Visual& operator=(const Visual&)=delete;
    Visual(Visual&&)=delete;Visual& operator=(Visual&&)=delete;
    const std::map<std::string,visual_detail::Json>& materialRecords()const noexcept{return materialRecords_;}
    const std::map<std::string,a::Material48E>& originalMaterials()const noexcept{return originalMaterials_;}
    const std::vector<const a::Material48E*>& componentMaterials(unsigned c)const {
        visual_detail::shared::need(c<3,"Original component material request");return componentMaterials_[c];
    }
    const std::vector<std::uint32_t>& flameMaterialIndices(unsigned c)const {
        visual_detail::shared::need(c<3,"Original flame component request");return flameMaterials_[c];
    }
    const std::vector<rr::platform::MaterialTextureSampler50EB>& samplers()const noexcept{return samplers_;}
    rr::platform::VehicleWindowMaterials50DS windowMaterials(const rr::platform::MaterialTextureGl50EB* gpu)const {
        visual_detail::shared::need(gpu&&gpu->initialized()&&gpu->textureCount()==materials.textureCount(),
            "Original per-pod texture publication unavailable");
        for(std::uint32_t i=0;i<materials.textureCount();++i)
            visual_detail::shared::need(gpu->asset(i)&&materials.texture(i)&&
                gpu->asset(i)->identity==materials.texture(i)->identity,
                "Original texture publication belongs to another racer");
        rr::platform::VehicleWindowMaterials50DS result{};result.textures=gpu;
        for(unsigned c=0;c<3;++c)result.component[c]={windowBindings_[c].data(),static_cast<std::uint32_t>(windowBindings_[c].size())};
        return result;
    }
private:
    std::map<std::string,visual_detail::Json> materialRecords_;
    std::map<std::string,a::Material48E> originalMaterials_;
    std::array<std::vector<const a::Material48E*>,3> componentMaterials_;
    std::array<std::vector<std::uint32_t>,3> flameMaterials_;
    std::array<std::vector<rr::platform::VehicleWindowMaterialBinding50DS>,3> windowBindings_;
    std::vector<rr::platform::MaterialTextureSampler50EB> samplers_;
};
} // namespace native_racer50HW
