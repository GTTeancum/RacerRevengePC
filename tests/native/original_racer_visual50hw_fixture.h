#pragma once
// Cold per-original-pod visual ownership. No GL calls, guest execution, cloned
// Pod01 meshes/materials, synthetic palettes, or per-frame allocations.
#include "native_racer_rig50hw_fixture.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../src/assets/import_texture48e.h"
#include "../../src/assets/original_standard_material_domain50la.h"
#include "../../src/game/original_racer_model_scene50la.h"
#include "../../src/game/native_gc_selected_lights_candidate.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <limits>
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
    static const char* const roles[]={"Chariot","LeftEngine","RightEngine","BottomLeftEngine","BottomRightEngine"};
    return roles[component];
}
//Canonical132820 joins fold ASCII case only. Retained selection-path slash
//normalization is a separate domain and must not admit a source-name alias.
inline bool sameSourceMember(const std::string& left,const std::string& right) noexcept {
    if(left.size()!=right.size())return false;
    const auto fold=[](unsigned char value){return value>='A'&&value<='Z'?value+('a'-'A'):value;};
    for(std::size_t i=0;i<left.size();++i)
        if(fold(static_cast<unsigned char>(left[i]))!=fold(static_cast<unsigned char>(right[i])))return false;
    return true;
}
// Cold publication identities are opaque and never reused, including between
// two Visuals of the same pod with different actual TEXA/wrap projections.
// The high half is disjoint from the retained per-pod base-map identities.
// Authentication comes from actual resource/source/pixel checks below, never
// from this token or a texture filename. Exhaustion rejects before mutation.
inline std::uint64_t nextSpecularIdentity50LA() {
    static std::atomic<std::uint64_t> next{std::uint64_t{1}<<63};
    auto value=next.load(std::memory_order_relaxed);
    for(;;){
        shared::need(value<std::numeric_limits<std::uint64_t>::max(),
            "Canonical second-map publication identity exhausted");
        if(next.compare_exchange_weak(value,value+1,std::memory_order_relaxed))return value;
    }
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
// after construction except the two explicit 50CR staging banks and one cold
// canonical lowering per admitted component before publication. Never move the
// owner, resize its vectors, alter banks, or destroy it while a batch/renderer
// borrows it. GPU ownership remains external: upload materials through a distinct
// 50EB owner and close the renderer before closing that GPU publication.
class Visual final {
public:
    a::VehicleDrawAssets50CR drawAssets;
    a::MaterialTextureAssets50EA materials;
    std::array<std::vector<pod_materials50ed_fixture::Selection>,5> textures;
    std::vector<pod_materials50ed_fixture::AuthoredSlot> authoredSlots;
    std::vector<pod_materials50ed_fixture::TextureSelection> textureSelections;
    std::string podId,displayName;
    unsigned componentCount=3;

    Visual(const std::filesystem::path& repository,
           const std::filesystem::path& visualSelectionRoot,const Rig& rig) {
        namespace d=visual_detail;namespace s=visual_detail::shared;
        if(rig.directOriginal){loadOriginalDisc(rig);return;}
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
        chariotPsgMember_=rig.psgMembers[Rig::chariot];
        chariotPsgSha256_=s::sha256(rig.psg[Rig::chariot]);
        chariotModelIndex_=rig.roots[Rig::chariot];
        leftPsgMember_=rig.psgMembers[Rig::left];
        leftPsgSha256_=s::sha256(rig.psg[Rig::left]);
        leftModelIndex_=rig.roots[Rig::left];
        rightPsgMember_=rig.psgMembers[Rig::right];
        rightPsgSha256_=s::sha256(rig.psg[Rig::right]);
        rightModelIndex_=rig.roots[Rig::right];
        if(rig.modelCount==5){
            const unsigned lowerModels[]={Rig::bottomLeft,Rig::bottomRight};
            for(unsigned side=0;side<2;++side){
                const auto model=lowerModels[side];
                lowerPsg_[side]={rig.psgMembers[model],s::sha256(rig.psg[model])};
                lowerModelIndex_[side]=rig.roots[model];
            }
        }
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
        std::array<d::Bytes,5> sceneBytes;
        std::array<std::array<std::uint8_t,a::maxModelNodes49O>,5> palettes{};
        constexpr unsigned mapped[]={Rig::chariot,Rig::left,Rig::right,Rig::bottomLeft,Rig::bottomRight};
        componentCount=rig.modelCount;drawInput.componentCount=componentCount;
        const auto& components=racer.at("components");s::need(components.size()==componentCount,"Original authored component catalog");
        for(unsigned c=0;c<componentCount;++c) {
            const auto& row=components.at(c);const unsigned model=mapped[c];
            s::need(row.at("role")==d::role(c),"Original component order differs");
            const auto& record=input.record(row.at("native").at("key").get<std::string>());
            s::need(record==row.at("native") && record.at("converter")=="rr-native-scene-48D-v1" &&
                record.at("source_sha256")==s::sha256(rig.psg[model]),"Original component PSG provenance differs");
            sceneBytes[c]=input.selected(row.at("rrscene").get<std::string>(),a::max_scene_bytes48d);
            s::need(s::sha256(sceneBytes[c])==record.at("scene_sha256").get<std::string>(),"Original component scene hash differs");
            // The actual draw-asset owner authenticates this same original
            // PSG/cache conversion and retains the decoded group topology.
            // RRSCENE1 alone omits groups/strips; do not discard that source.
            const auto count=rig.hierarchy[model].nodeCount;const auto padded=(count+3u)&~3u;
            const auto& psg=rig.psg[model];s::need(count&&count<=a::maxModelNodes49O&&psg.size()>=12u+padded,
                "Original PSG palette suffix bounds");
            const auto suffix=psg.size()-(12u+padded);
            s::need(s::u32(psg.data()+suffix)==1 && s::u32(psg.data()+suffix+4)==padded &&
                s::u32(psg.data()+suffix+8+padded)==count,"Original selected PSG palette framing");
            for(unsigned n=0;n<count;++n)palettes[c][n]=psg[suffix+8+n];
            drawInput.component[c]={&sceneBytes[c],rig.sourceNodes[model],count,palettes[c].data(),count};
            drawInput.component[c].sourcePsgBytes=&rig.psg[model];
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
            const auto& originalMember=*originals.at(rig_detail::lower(row.at("member").get<std::string>()));
            s::need(materialSources_.emplace(name,SourceIdentity{originalMember.at("source_member").get<std::string>(),
                originalMember.at("source_sha256").get<std::string>()}).second,
                "Duplicate original material source identity");
        }
        std::map<std::string,const d::Json*> textureRows;
        for(const auto& row:racer.at("textures"))
            s::need(textureRows.emplace(row.at("native").at("key").get<std::string>(),&row).second,
                "Duplicate original texture key");
        std::map<std::string,std::uint32_t> textureIndices;
        std::vector<d::Bytes> textureBytes;
        std::vector<a::MaterialTextureSlotInput50EA> slots;
        std::size_t rgbaBytes=0,sourceBytes=0;
        for(unsigned c=0;c<componentCount;++c) {
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
                // Constructor publication is authored pass1/map1 only. The
                // separate canonical-engine source-pair opt-in below obtains
                // its real second resource from the actual domain. No other
                // pass, substitute texture or untextured fallback is selected.
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
                    textureSources_.push_back({member->second->at("source_member").get<std::string>(),
                        member->second->at("source_sha256").get<std::string>()});
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
        visual_detail::shared::need(c<componentCount,"Original component material request");return componentMaterials_[c];
    }
    const std::vector<std::uint32_t>& flameMaterialIndices(unsigned c)const {
        visual_detail::shared::need(c<componentCount,"Original flame component request");return flameMaterials_[c];
    }
    const std::vector<rr::platform::MaterialTextureSampler50EB>& samplers()const noexcept{return samplers_;}
    const a::MaterialTextureAssets50EA& canonicalEngineSpecularMaterials50LA()const noexcept {
        return canonicalEngineSpecularMaterials_;
    }
    const std::vector<rr::platform::MaterialTextureSampler50EB>& canonicalEngineSpecularSamplers50LA()const noexcept {
        return canonicalEngineSpecularSamplers_;
    }
    // Cold chariot consumer closure. The SAME actual model/geometry1B4,
    // references1C4 and shared382B38/383400 owners select the existing native
    // base-map bank. Authenticate this model's own Rig PSG root, source rows,
    // palette and all material ordinals before publishing copied descriptors.
    // Chariot body lighting and pass1/map1 display remain the existing host policy;
    // no source-color mask, second-map bank, specular output or flame is introduced.
    // Whole actual materials retain every authored pass/map, including the
    // extra pod_wndw/12c_4 passes; only pass1/map1 is displayed here.
    // Call after canonical population and before GPU/window publication.
    void bindCanonicalChariot(const g::OriginalRacerModelSceneOwner50LA& model,
        const a::OriginalStandardModelGeometry50LA& geometry,
        const a::OriginalStandardMaterialReferences50LA& references,
        const a::OriginalStandardMaterialDomain50LA& domain) {
        namespace s=visual_detail::shared;
        namespace p=rr::platform;
        constexpr unsigned component=static_cast<unsigned>(g::VehicleDrawComponent50CM::chariot);
        s::need(!canonicalChariotBound_&&!materialsPublished_&&domain.prepared()&&
            !domain.materials382B38()->populationTerminal()&&!domain.materials382B38()->demandTerminal()&&
            !domain.textures383400()->populationTerminal()&&model.constructed2403A0()&&
            model.loadedFreshRows28CAC0()&&model.loadedQuantization240570()&&model.model()&&
            model.model()->sceneIndex==chariotModelIndex_&&model.fields().geometry1B4==&geometry&&
            geometry.constructed240110()&&geometry.prototype1C()==&model&&references.complete()&&
            !references.released26E4C0()&&references.table382B38()==domain.materials382B38().get()&&
            references.count1C0()&&model.fields().materials1C4==references.handle1C4(0)&&
            model.fields().materialCount1C0==static_cast<std::int32_t>(*references.count1C0()),
            "Canonical chariot actual model/material ownership rejected");
        const auto* resource=geometry.payload();
        const auto* scene=drawAssets.scene(g::VehicleDrawComponent50CM::chariot);
        s::need(resource&&resource->member()&&resource->originalBytes()&&resource->scene()&&scene&&
            visual_detail::sameSourceMember(*resource->member(),chariotPsgMember_)&&
            s::sha256(*resource->originalBytes())==chariotPsgSha256_&&
            resource->scene()->materials==scene->materials&&
            *references.count1C0()==scene->materials.size()&&
            scene->materials.size()==windowBindings_[component].size()&&
            scene->materials.size()==textures[component].size()&&
            scene->materials.size()==componentMaterials_[component].size(),
            "Canonical chariot PSG/material ordinal identity differs");
        g::VehicleDrawBatchInput50CO nativeDraw;
        s::need(drawAssets.batchInput(0,g::ModelPalettePhase49S::selectedRenderPreparation,
                g::ModelPaletteSceneState49S::completeCurrentScene,nativeDraw)&&
            geometry.hasPalette20()&&geometry.paletteCount20()==1&&geometry.palette20(0)&&
            model.fields().sourceRows1B8&&model.sourceRowCount()==resource->nodeCount(),
            "Canonical chariot actual palette/source-row ownership missing");
        const auto& selected=nativeDraw.selection[component];
        const auto& palette=*geometry.palette20(0);
        s::need(selected.sourceNodeCount==model.sourceRowCount()&&palette.size()==model.sourceRowCount()&&
            selected.paletteCount==palette.size()&&selected.sourceNodes&&selected.paletteIds,
            "Canonical chariot native row/palette extent differs");
        for(std::uint32_t row=0;row<model.sourceRowCount();++row)
            s::need(model.sourceRow(row)&&model.fields().sourceRows1B8[row]==model.sourceRow(row)&&
                selected.sourceNodes[row]==model.sourceRow(row)->sceneIndex&&
                selected.paletteIds[row]==palette[row],
                "Canonical chariot draw does not use actual model source rows");
        s::need(!scene->lods.empty()&&resource->scene()->lods.size()==scene->lods.size(),
            "Canonical chariot LOD geometry differs");
        for(std::size_t lod=0;lod<scene->lods.size();++lod){
            s::need(resource->scene()->lods[lod].surfaces.size()==scene->lods[lod].surfaces.size(),
                "Canonical chariot surface extent differs");
            for(std::size_t surface=0;surface<scene->lods[lod].surfaces.size();++surface){
                const auto* material=geometry.surfaceMaterial00(lod,surface);
                s::need(material&&*material>=0&&static_cast<std::uint32_t>(*material)==
                    scene->lods[lod].surfaces[surface].material_index,
                    "Canonical chariot surface material ordinal differs");
            }
        }
        s::need(textureSources_.size()==textureSelections.size()&&
            textureSources_.size()==materials.textureCount(),"Canonical texture source identities missing");
        auto bindings=windowBindings_[component];
        auto selections=textures[component];
        auto samplers=samplers_;
        for(std::size_t ordinal=0;ordinal<scene->materials.size();++ordinal){
            const auto* retainedMaterial=componentMaterials_[component][ordinal];
            s::need(retainedMaterial&&!retainedMaterial->passes.empty()&&retainedMaterial->passes.size()<=4,
                "Canonical chariot authored material pass extent differs");
            const auto* handle=references.handle1C4(ordinal);
            const a::OriginalStandardMaterialObject50LA* material=nullptr;
            a::OriginalStandardMaterialDiagnostic50LA diagnostic;
            s::need(handle&&handle==model.fields().materials1C4+ordinal&&
                domain.materials382B38()->resolve298E60(*handle,material,diagnostic)&&
                material&&material->constructed29B550()&&material->row40()&&*material->row40()==*handle&&
                material->passCount48()&&*material->passCount48()==
                    static_cast<std::int32_t>(retainedMaterial->passes.size())&&material->source()&&
                material->source()->member()&&material->source()->originalBytes()&&
                material->textures383400()==domain.textures383400().get(),
                "Canonical chariot model1C4 does not resolve a complete actual material");
            const auto source=materialSources_.find(rig_detail::lower(scene->materials[ordinal]));
            s::need(source!=materialSources_.end()&&
                visual_detail::sameSourceMember(*material->source()->member(),source->second.member)&&
                s::sha256(*material->source()->originalBytes())==source->second.sha256,
                "Canonical chariot PSM differs from authenticated visual source");
            // Every pass/map keeps its genuine shared table/resource owner,
            // even when the existing host display policy does not draw it.
            // No additional native publication is made for those later maps.
            using Provider=a::OriginalStandardArchiveProvider50LA;
            const auto* provider=domain.archiveProvider3E1A70().get();
            s::need(provider&&provider->sealed()&&material->name00(),
                "Canonical chariot material has no sealed actual source domain");
            const auto slash=material->name00()->find_last_of('\\');
            const auto prefix=slash==std::string::npos?std::string{}:material->name00()->substr(0,slash+1);
            for(std::size_t pi=0;pi<retainedMaterial->passes.size();++pi){
                const auto& authored=retainedMaterial->passes[pi];
                a::OriginalStandardMaterialPassView50LA ownedPass;
                a::OriginalStandardProgramRowView50LA ownedProgram;
                s::need(authored.pass==pi+1&&material->pass4C(pi,ownedPass)&&
                    ownedPass.references04&&*ownedPass.references04>0&&ownedPass.program18&&
                    domain.programs382B70()->row(*ownedPass.program18,ownedProgram)&&ownedProgram.canonicalName&&
                    rig_detail::lower(ownedProgram.canonicalName)==rig_detail::lower(authored.microprogram)&&
                    ownedPass.mapCount20&&*ownedPass.mapCount20==authored.textures.size()&&
                    *ownedPass.mapCount20<=2&&ownedPass.texture1A&&ownedPass.state30,
                    "Canonical chariot complete authored pass/map ownership differs");
                for(std::size_t map=0;map<authored.textures.size();++map){
                    a::OriginalStandardTextureRowView50LA ownedTexture;
                    s::need(authored.textures[map].slot==map+1&&
                        domain.textures383400()->row((*ownedPass.texture1A)[map],ownedTexture)&&
                        ownedTexture.name00&&visual_detail::sameSourceMember(
                            *ownedTexture.name00,prefix+authored.textures[map].name)&&
                        ownedTexture.references08&&*ownedTexture.references08>0&&ownedTexture.object04&&
                        ownedTexture.object04->constructed24DA30()&&ownedTexture.object04->references00()&&
                        *ownedTexture.object04->references00()>0&&ownedTexture.object04->sourceMember()&&
                        ownedTexture.object04->originalBytes(),
                        "Canonical chariot authored map has no actual live texture resource");
                    Provider::MemberView sourceMember;
                    const auto& bytes=*ownedTexture.object04->originalBytes();
                    s::need(provider->lookup132820(*ownedTexture.object04->sourceMember(),sourceMember)==
                            Provider::Lookup::present&&sourceMember.member&&sourceMember.data&&
                        sourceMember.size==bytes.size()&&visual_detail::sameSourceMember(
                            *sourceMember.member,*ownedTexture.object04->sourceMember())&&
                        std::equal(bytes.begin(),bytes.end(),sourceMember.data),
                        "Canonical chariot map bytes differ from the same actual mounted source");
                }
            }
            a::OriginalStandardMaterialPassView50LA pass;
            s::need(material->pass4C(0,pass)&&pass.references04&&*pass.references04>0&&
                pass.program18&&pass.texture1A&&pass.uv1E&&pass.mapCount20&&
                *pass.mapCount20>=1&&*pass.mapCount20<=2&&pass.flags21&&(*pass.flags21&2u)&&
                pass.state30&&pass.state30->context(0),
                "Canonical chariot displayed pass/map/context is unavailable");
            a::OriginalStandardProgramRowView50LA program;
            s::need(domain.programs382B70()->row(*pass.program18,program)&&program.canonicalName&&
                rig_detail::lower(program.canonicalName)==rig_detail::lower(textures[component][ordinal].program),
                "Canonical chariot program differs from authenticated display selection");
            const auto programName=rig_detail::lower(program.canonicalName);
            s::need(programName=="diffusecompressed"||programName=="specularmapdualcompressed",
                "Canonical chariot program has no admitted host base-map projection");
            s::need(pass.state30->contextCount()==*pass.mapCount20&&(*pass.uv1E)[0]==0&&
                !pass.animation10,
                "Canonical chariot native base-map context/UV projection is unsupported");
            a::OriginalStandardTextureRowView50LA textureRow;
            const auto token=(*pass.texture1A)[0];
            s::need(domain.textures383400()->row(token,textureRow)&&textureRow.references08&&
                *textureRow.references08>0&&textureRow.object04&&
                textureRow.object04->constructed24DA30()&&textureRow.object04->sourceMember()&&
                textureRow.object04->originalBytes(),
                "Canonical chariot first-map token has no actual PST resource");
            const auto& texture=*textureRow.object04;
            a::OriginalStandardTextureLevelView50LA level;
            s::need(texture.level100(0,level)&&level.width104>0&&level.height110>0&&
                level.linearSourceIndices&&level.linearSourceBytes,
                "Canonical chariot native selected PST mip is unavailable");
            const auto textureSha=s::sha256(*texture.originalBytes());
            std::uint32_t bank=a::unresolvedMaterialTexture50EA;
            for(std::uint32_t i=0;i<textureSources_.size();++i){
                const auto& identity=textureSources_[i];const auto& retained=textureSelections[i];
                if(!visual_detail::sameSourceMember(identity.member,*texture.sourceMember())||
                   identity.sha256!=textureSha||retained.mipIndex!=level.sourceMipOrdinal||
                   retained.width!=static_cast<std::uint32_t>(level.width104)||
                   retained.height!=static_cast<std::uint32_t>(level.height110))continue;
                s::need(bank==a::unresolvedMaterialTexture50EA,
                    "Canonical chariot PST/mip maps to ambiguous native bank entries");
                bank=i;
            }
            s::need(bank!=a::unresolvedMaterialTexture50EA&&bank<samplers.size()&&
                materials.texture(bank)&&materials.texture(bank)->identity==textureSelections[bank].identity&&
                textureSelections[bank].key==textures[component][ordinal].textureKey,
                "Canonical chariot PST/mip has no authenticated native RGBA bank entry");
            const auto& state=*pass.state30->context(0);
            using Field=a::OriginalUiGsField50KG;
            const auto value=[&](Field field){return state.fields[static_cast<std::size_t>(field)].value;};
            const auto blend=value(Field::alphaBlendingOnOff),depth=value(Field::zWriteDisabled);
            const auto wrapU=value(Field::wrapModeHorizontal),wrapV=value(Field::wrapModeVertical);
            s::need((blend==0||blend==1)&&(depth==0||depth==1)&&
                (wrapU==0||wrapU==1)&&(wrapV==0||wrapV==1),
                "Canonical chariot context has unsupported native blend/depth/wrap policy");
            p::VehicleWindowMaterialBinding50DS binding{p::VehicleWindowMaterialMode50DS::requiredTexture,bank};
            binding.depthWrite=depth==0;
            if(blend){
                s::need(value(Field::inputColorA)==0&&value(Field::inputColorB)==1&&
                    value(Field::inputAlphaC)==0&&value(Field::inputColorD)==1,
                    "Canonical chariot body context has unsupported native source-alpha projection");
                binding.blend=p::VehicleWindowBlend50DS::sourceAlpha;
                binding.alphaScale=(255.f/128.f)*(255.f/128.f);
            }
            const auto& retainedBinding=windowBindings_[component][ordinal];
            s::need(binding.blend==retainedBinding.blend&&binding.depthWrite==retainedBinding.depthWrite&&
                binding.fixedBlendAlpha==retainedBinding.fixedBlendAlpha&&binding.alphaScale==retainedBinding.alphaScale,
                "Canonical chariot context changes the existing native base-map policy");
            const auto u=wrapU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat;
            const auto v=wrapV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat;
            s::need(samplers[bank].s==u&&samplers[bank].t==v,
                "Canonical chariot texture needs a distinct native sampler publication");
            samplers[bank].s=u;samplers[bank].t=v;bindings[ordinal]=binding;
            selections[ordinal].textureIndex=bank;
            selections[ordinal].textureIdentity=textureSelections[bank].identity;
            selections[ordinal].mipIndex=level.sourceMipOrdinal;
        }
        windowBindings_[component].swap(bindings);textures[component].swap(selections);samplers_.swap(samplers);
        canonicalChariotBound_=true;
    }
    bool canonicalChariotBound()const noexcept{return canonicalChariotBound_;}
private:
    // The existing left-engine base-map projection is also exact for POD02's
    // actual lower PSMs (02_1 plus flame02l/r). Only the authenticated model,
    // component and PSG identity change; lower lighting and map1 stay as-is.
    void bindCanonicalEngineBaseMap(const g::OriginalRacerModelSceneOwner50LA& model,
        const a::OriginalStandardModelGeometry50LA& geometry,
        const a::OriginalStandardMaterialReferences50LA& references,
        const a::OriginalStandardMaterialDomain50LA& domain,
        g::VehicleDrawComponent50CM drawComponent,g::SceneNodeIndex49P modelIndex,
        const std::string& psgMember,const std::string& psgSha256,bool& bound,
        bool useOriginalBodyColors50LA=false) {
        namespace s=visual_detail::shared;
        namespace p=rr::platform;
        const unsigned component=static_cast<unsigned>(drawComponent);
        s::need(!useOriginalBodyColors50LA||drawComponent==g::VehicleDrawComponent50CM::leftEngine,
            "Canonical engine base-color projection is restricted to the primary left engine");
        s::need(component<componentCount&&!bound&&!materialsPublished_&&domain.prepared()&&
            !domain.materials382B38()->populationTerminal()&&!domain.materials382B38()->demandTerminal()&&
            !domain.textures383400()->populationTerminal()&&model.constructed2403A0()&&
            model.loadedFreshRows28CAC0()&&model.loadedQuantization240570()&&model.model()&&
            model.model()->sceneIndex==modelIndex&&model.fields().geometry1B4==&geometry&&
            geometry.constructed240110()&&geometry.prototype1C()==&model&&references.complete()&&
            !references.released26E4C0()&&references.table382B38()==domain.materials382B38().get()&&
            references.count1C0()&&model.fields().materials1C4==references.handle1C4(0)&&
            model.fields().materialCount1C0==static_cast<std::int32_t>(*references.count1C0()),
            "Canonical engine base-map actual model/material ownership rejected");
        const auto* resource=geometry.payload();
        const auto* scene=drawAssets.scene(drawComponent);
        s::need(resource&&resource->member()&&resource->originalBytes()&&resource->scene()&&scene&&
            visual_detail::sameSourceMember(*resource->member(),psgMember)&&
            s::sha256(*resource->originalBytes())==psgSha256&&
            resource->scene()->materials==scene->materials&&
            *references.count1C0()==scene->materials.size()&&
            scene->materials.size()==windowBindings_[component].size()&&
            scene->materials.size()==textures[component].size(),
            "Canonical engine base-map PSG/material ordinal identity differs");
        g::VehicleDrawBatchInput50CO nativeDraw;
        s::need(drawAssets.batchInput(0,g::ModelPalettePhase49S::selectedRenderPreparation,
                g::ModelPaletteSceneState49S::completeCurrentScene,nativeDraw)&&
            geometry.hasPalette20()&&geometry.paletteCount20()==1&&geometry.palette20(0)&&
            model.fields().sourceRows1B8&&model.sourceRowCount()==resource->nodeCount(),
            "Canonical engine base-map actual palette/source-row ownership missing");
        const auto& selected=nativeDraw.selection[component];
        const auto& palette=*geometry.palette20(0);
        s::need(selected.sourceNodeCount==model.sourceRowCount()&&palette.size()==model.sourceRowCount()&&
            selected.paletteCount==palette.size()&&selected.sourceNodes&&selected.paletteIds,
            "Canonical engine base-map native row/palette extent differs");
        for(std::uint32_t row=0;row<model.sourceRowCount();++row)
            s::need(model.sourceRow(row)&&model.fields().sourceRows1B8[row]==model.sourceRow(row)&&
                selected.sourceNodes[row]==model.sourceRow(row)->sceneIndex&&
                selected.paletteIds[row]==palette[row],
                "Canonical engine base-map draw does not use actual model source rows");
        s::need(!scene->lods.empty()&&resource->scene()->lods.size()==scene->lods.size(),
            "Canonical engine base-map LOD geometry differs");
        for(std::size_t lod=0;lod<scene->lods.size();++lod){
            s::need(resource->scene()->lods[lod].surfaces.size()==scene->lods[lod].surfaces.size(),
                "Canonical engine base-map surface extent differs");
            for(std::size_t surface=0;surface<scene->lods[lod].surfaces.size();++surface){
                const auto* material=geometry.surfaceMaterial00(lod,surface);
                s::need(material&&*material>=0&&static_cast<std::uint32_t>(*material)==
                    scene->lods[lod].surfaces[surface].material_index,
                    "Canonical engine base-map surface material ordinal differs");
            }
        }
        s::need(textureSources_.size()==textureSelections.size()&&
            textureSources_.size()==materials.textureCount(),"Canonical texture source identities missing");
        auto bindings=windowBindings_[component];
        auto selections=textures[component];
        auto samplers=samplers_;
        std::vector<std::uint8_t> bodyColorModes;
        if(useOriginalBodyColors50LA)bodyColorModes.resize(scene->materials.size(),0);
        std::size_t bodyColorCount=0;
        for(std::size_t ordinal=0;ordinal<scene->materials.size();++ordinal){
            const auto* handle=references.handle1C4(ordinal);
            const a::OriginalStandardMaterialObject50LA* material=nullptr;
            a::OriginalStandardMaterialDiagnostic50LA diagnostic;
            s::need(handle&&handle==model.fields().materials1C4+ordinal&&
                domain.materials382B38()->resolve298E60(*handle,material,diagnostic)&&
                material&&material->constructed29B550()&&material->row40()&&*material->row40()==*handle&&
                material->passCount48()&&*material->passCount48()==1&&material->source()&&
                material->source()->member()&&material->source()->originalBytes()&&
                material->textures383400()==domain.textures383400().get(),
                "Canonical engine base-map model1C4 does not resolve a complete actual material");
            const auto source=materialSources_.find(rig_detail::lower(scene->materials[ordinal]));
            s::need(source!=materialSources_.end()&&
                visual_detail::sameSourceMember(*material->source()->member(),source->second.member)&&
                s::sha256(*material->source()->originalBytes())==source->second.sha256,
                "Canonical engine base-map PSM differs from authenticated visual source");
            a::OriginalStandardMaterialPassView50LA pass;
            s::need(material->pass4C(0,pass)&&pass.references04&&*pass.references04>0&&
                pass.program18&&pass.texture1A&&pass.uv1E&&pass.mapCount20&&
                *pass.mapCount20>=1&&*pass.mapCount20<=2&&pass.flags21&&(*pass.flags21&2u)&&
                pass.state30&&pass.state30->context(0),
                "Canonical engine base-map displayed pass/map/context is unavailable");
            a::OriginalStandardProgramRowView50LA program;
            s::need(domain.programs382B70()->row(*pass.program18,program)&&program.canonicalName&&
                rig_detail::lower(program.canonicalName)==rig_detail::lower(textures[component][ordinal].program),
                "Canonical engine base-map program differs from authenticated display selection");
            const auto programName=rig_detail::lower(program.canonicalName);
            const bool flame=programName=="diffusetextranscompressed";
            s::need(flame||programName=="specularmapdualcompressed",
                "Canonical engine base-map program has no admitted host base-map projection");
            s::need(!flame||*pass.mapCount20==1,
                "Canonical engine base-map flame requires its original single map");
            s::need(pass.state30->contextCount()>=1&&pass.state30->contextCount()<=2&&
                (!flame||pass.state30->contextCount()==1)&&(*pass.uv1E)[0]==0,
                "Canonical engine base-map context/UV projection is unsupported");
            a::OriginalStandardTextureRowView50LA textureRow;
            const auto token=(*pass.texture1A)[0];
            s::need(domain.textures383400()->row(token,textureRow)&&textureRow.references08&&
                *textureRow.references08>0&&textureRow.object04&&
                textureRow.object04->constructed24DA30()&&textureRow.object04->sourceMember()&&
                textureRow.object04->originalBytes(),
                "Canonical engine base-map first-map token has no actual PST resource");
            const auto& texture=*textureRow.object04;
            a::OriginalStandardTextureLevelView50LA level;
            s::need(texture.level100(0,level)&&level.width104>0&&level.height110>0&&
                level.linearSourceIndices&&level.linearSourceBytes,
                "Canonical engine base-map native selected PST mip is unavailable");
            const auto textureSha=s::sha256(*texture.originalBytes());
            std::uint32_t bank=a::unresolvedMaterialTexture50EA;
            for(std::uint32_t i=0;i<textureSources_.size();++i){
                const auto& identity=textureSources_[i];const auto& retained=textureSelections[i];
                if(!visual_detail::sameSourceMember(identity.member,*texture.sourceMember())||
                   identity.sha256!=textureSha||retained.mipIndex!=level.sourceMipOrdinal||
                   retained.width!=static_cast<std::uint32_t>(level.width104)||
                   retained.height!=static_cast<std::uint32_t>(level.height110))continue;
                s::need(bank==a::unresolvedMaterialTexture50EA,
                    "Canonical engine base-map PST/mip maps to ambiguous native bank entries");
                bank=i;
            }
            s::need(bank!=a::unresolvedMaterialTexture50EA&&bank<samplers.size()&&
                materials.texture(bank)&&materials.texture(bank)->identity==textureSelections[bank].identity&&
                textureSelections[bank].key==textures[component][ordinal].textureKey,
                "Canonical engine base-map PST/mip has no authenticated native RGBA bank entry");
            const auto& state=*pass.state30->context(0);
            using Field=a::OriginalUiGsField50KG;
            const auto value=[&](Field field){return state.fields[static_cast<std::size_t>(field)].value;};
            const auto blend=value(Field::alphaBlendingOnOff),depth=value(Field::zWriteDisabled);
            const auto wrapU=value(Field::wrapModeHorizontal),wrapV=value(Field::wrapModeVertical);
            s::need((blend==0||blend==1)&&(depth==0||depth==1)&&
                (wrapU==0||wrapU==1)&&(wrapV==0||wrapV==1),
                "Canonical engine base-map context has unsupported native blend/depth/wrap policy");
            p::VehicleWindowMaterialBinding50DS binding{p::VehicleWindowMaterialMode50DS::requiredTexture,bank};
            binding.depthWrite=depth==0;
            if(flame){
                s::need(blend==1&&value(Field::inputColorA)==0&&value(Field::inputColorB)==2&&
                    value(Field::inputAlphaC)==2&&value(Field::inputColorD)==1&&
                    value(Field::fixedAlphaValue)==255&&depth==1,
                    "Canonical engine flame context has unsupported native additive projection");
                binding.blend=p::VehicleWindowBlend50DS::additive;
                binding.fixedBlendAlpha=true;binding.alphaScale=255.f/128.f;
            }else if(blend){
                s::need(value(Field::inputColorA)==0&&value(Field::inputColorB)==1&&
                    value(Field::inputAlphaC)==0&&value(Field::inputColorD)==1,
                    "Canonical engine body context has unsupported native source-alpha projection");
                binding.blend=p::VehicleWindowBlend50DS::sourceAlpha;
                binding.alphaScale=(255.f/128.f)*(255.f/128.f);
            }
            if(useOriginalBodyColors50LA&&!flame){
                // The SAME actual left model's base context selects the
                // recovered per-node diffuse RGB and byte MODULATE route.
                // Its second map stays owned but unconsumed; no source-alpha,
                // pair fog/depth policy or right-model light rows are borrowed.
                s::need(programName=="specularmapdualcompressed"&&*pass.mapCount20==2&&
                    pass.state30->contextCount()==2&&!pass.animation10&&state.runtime435670==0&&
                    pass.parameterCount28&&*pass.parameterCount28==0&&
                    pass.parameterMask24&&!*pass.parameterMask24&&!pass.parameters34&&
                    value(Field::textureFunction)==0&&value(Field::textureColorComponent)==1&&
                    value(Field::textureMappingOnOff)==1&&value(Field::gouraudShadingOnOff)==1&&
                    value(Field::textureCoordinatesSTQorUV)==0&&
                    value(Field::colorClampingMethod)==1&&blend==0&&depth==0&&
                    value(Field::alphaTestOnOff)==0&&value(Field::destinationAlphaTestOnOff)==0,
                    "Canonical left-engine body has no admitted source RGB/MODULATE projection");
                binding.sourceDiffuseCompressed50LA=true;
                bodyColorModes[ordinal]=1;++bodyColorCount;
            }
            if(drawComponent==g::VehicleDrawComponent50CM::bottomLeftEngine||
               drawComponent==g::VehicleDrawComponent50CM::bottomRightEngine){
                const auto& retainedBinding=windowBindings_[component][ordinal];
                s::need(binding.blend==retainedBinding.blend&&binding.depthWrite==retainedBinding.depthWrite&&
                    binding.fixedBlendAlpha==retainedBinding.fixedBlendAlpha&&binding.alphaScale==retainedBinding.alphaScale,
                    "Canonical lower-engine context changes the existing native base-map policy");
            }
            const auto u=wrapU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat;
            const auto v=wrapV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat;
            s::need(samplers[bank].s==u&&samplers[bank].t==v,
                "Canonical engine base-map texture needs a distinct native sampler publication");
            samplers[bank].s=u;samplers[bank].t=v;bindings[ordinal]=binding;
            selections[ordinal].textureIndex=bank;
            selections[ordinal].textureIdentity=textureSelections[bank].identity;
            selections[ordinal].mipIndex=level.sourceMipOrdinal;
        }
        s::need(!useOriginalBodyColors50LA||bodyColorCount,
            "Canonical left-engine has no actual body-color material");
        windowBindings_[component].swap(bindings);textures[component].swap(selections);samplers_.swap(samplers);
        if(useOriginalBodyColors50LA)canonicalLeftBodyColorModes50LA_.swap(bodyColorModes);
        bound=true;
    }
public:
    // Cold engine consumers. All joins authenticate the SAME actual model,
    // geometry1B4, references1C4, shared382B38/383400 resources and source rows.
    // Primary left optionally consumes its own source-color/MODULATE base
    // projection. POD02 lower models keep their existing pass1/map1 host route.
    // No additional second-map bank or specular output is introduced.
    void bindCanonicalLeft(const g::OriginalRacerModelSceneOwner50LA& model,
        const a::OriginalStandardModelGeometry50LA& geometry,
        const a::OriginalStandardMaterialReferences50LA& references,
        const a::OriginalStandardMaterialDomain50LA& domain,
        bool useOriginalBodyColors50LA=false) {
        bindCanonicalEngineBaseMap(model,geometry,references,domain,
            g::VehicleDrawComponent50CM::leftEngine,leftModelIndex_,leftPsgMember_,leftPsgSha256_,canonicalLeftBound_,
            useOriginalBodyColors50LA);
    }
    bool canonicalLeftBound()const noexcept{return canonicalLeftBound_;}
    void bindCanonicalBottomLeft(const g::OriginalRacerModelSceneOwner50LA& model,
        const a::OriginalStandardModelGeometry50LA& geometry,
        const a::OriginalStandardMaterialReferences50LA& references,
        const a::OriginalStandardMaterialDomain50LA& domain) {
        visual_detail::shared::need(podId=="POD02"&&componentCount==5,
            "Canonical bottom-left engine requires the actual POD02 five-model family");
        bindCanonicalEngineBaseMap(model,geometry,references,domain,
            g::VehicleDrawComponent50CM::bottomLeftEngine,lowerModelIndex_[0],
            lowerPsg_[0].member,lowerPsg_[0].sha256,canonicalLowerBound_[0]);
    }
    bool canonicalBottomLeftBound()const noexcept{return canonicalLowerBound_[0];}
    void bindCanonicalBottomRight(const g::OriginalRacerModelSceneOwner50LA& model,
        const a::OriginalStandardModelGeometry50LA& geometry,
        const a::OriginalStandardMaterialReferences50LA& references,
        const a::OriginalStandardMaterialDomain50LA& domain) {
        visual_detail::shared::need(podId=="POD02"&&componentCount==5,
            "Canonical bottom-right engine requires the actual POD02 five-model family");
        bindCanonicalEngineBaseMap(model,geometry,references,domain,
            g::VehicleDrawComponent50CM::bottomRightEngine,lowerModelIndex_[1],
            lowerPsg_[1].member,lowerPsg_[1].sha256,canonicalLowerBound_[1]);
    }
    bool canonicalBottomRightBound()const noexcept{return canonicalLowerBound_[1];}
    // Cold owner borrows in draw-component order: left, right, lower-left,
    // lower-right. They are authenticated against this Visual before use and
    // never retained by the resulting immutable native publication.
    struct CanonicalEngineModelInput50LA {
        const g::OriginalRacerModelSceneOwner50LA* model=nullptr;
        const a::OriginalStandardModelGeometry50LA* geometry=nullptr;
        const a::OriginalStandardMaterialReferences50LA* references=nullptr;
    };
    // Prepare every actual engine before publishing one load-once map2 bank.
    // Cold source order remains right, left, optional lower-right, lower-left.
    // Each retains its own source identity, palette, bounds and component slots.
    void bindCanonicalEngineModels(const std::array<CanonicalEngineModelInput50LA,4>& models,
        const a::OriginalStandardMaterialDomain50LA& domain,
        bool useOriginalBodyColors50LA=false) {
        namespace s=visual_detail::shared;
        namespace p=rr::platform;
        s::need(componentCount==3u||(componentCount==5u&&podId=="POD02"),
            "Canonical optional engine pairs require the actual POD02 five-model family");
        const unsigned engineCount=componentCount==5u?4u:2u;
        for(unsigned engine=0;engine<4;++engine){
            const auto& input=models[engine];
            s::need(engine<engineCount?(input.model&&input.geometry&&input.references):
                (!input.model&&!input.geometry&&!input.references),
                "Canonical engine input extent differs from actual model family");
        }
        struct EngineDraft {
            std::vector<p::VehicleWindowMaterialBinding50DS> bindings;
            std::vector<pod_materials50ed_fixture::Selection> selections;
            std::vector<std::uint8_t> bodyColorModes;
        };
        std::array<EngineDraft,4> drafts;
        auto samplers=samplers_;
        std::size_t totalBodyColorCount=0;
        // Resource pointers and level borrows are cold-call-only dedup keys.
        // Only copied native RGBA, slot identities and samplers are published.
        struct SpecularProjection {
            const a::OriginalStandardTextureResource50LA* resource=nullptr;
            std::size_t level=0;
            std::array<std::int32_t,5> texaWrap{};
            std::uint64_t identity=0;
            visual_detail::Bytes bytes;
        };
        std::vector<SpecularProjection> specularProjections;
        std::vector<a::MaterialTextureSlotInput50EA> specularSlots;
        std::vector<p::MaterialTextureSampler50EB> specularSamplers;
        constexpr std::array<unsigned,4> coldOrder{{1,0,3,2}};
        for(unsigned visit=0;visit<engineCount;++visit){
            const unsigned engine=coldOrder[visit],component=engine+1u;
            const auto drawComponent=static_cast<g::VehicleDrawComponent50CM>(component);
            const auto& model=*models[engine].model;
            const auto& geometry=*models[engine].geometry;
            const auto& references=*models[engine].references;
            const auto modelIndex=engine==0?leftModelIndex_:engine==1?rightModelIndex_:lowerModelIndex_[engine-2u];
            const auto& psgMember=engine==0?leftPsgMember_:engine==1?rightPsgMember_:lowerPsg_[engine-2u].member;
            const auto& psgSha256=engine==0?leftPsgSha256_:engine==1?rightPsgSha256_:lowerPsg_[engine-2u].sha256;
            s::need(!canonicalRightBound_&&!canonicalLeftBound_&&!canonicalLowerBound_[0]&&!canonicalLowerBound_[1]&&
                !materialsPublished_&&!canonicalEngineSpecularMaterials_.loaded()&&domain.prepared()&&
                !domain.materials382B38()->populationTerminal()&&!domain.materials382B38()->demandTerminal()&&
                !domain.textures383400()->populationTerminal()&&model.constructed2403A0()&&
                model.loadedFreshRows28CAC0()&&model.loadedQuantization240570()&&model.model()&&
                model.model()->sceneIndex==modelIndex&&model.fields().geometry1B4==&geometry&&
                geometry.constructed240110()&&geometry.prototype1C()==&model&&references.complete()&&!references.released26E4C0()&&
                references.table382B38()==domain.materials382B38().get()&&references.count1C0()&&
                model.fields().materials1C4==references.handle1C4(0)&&
                model.fields().materialCount1C0==static_cast<std::int32_t>(*references.count1C0()),
                "Canonical engine actual model/material ownership rejected");
            const auto* resource=geometry.payload();
            const auto* scene=drawAssets.scene(drawComponent);
            s::need(resource&&resource->member()&&resource->originalBytes()&&resource->scene()&&scene&&
                visual_detail::sameSourceMember(*resource->member(),psgMember)&&
                s::sha256(*resource->originalBytes())==psgSha256&&
                resource->scene()->materials==scene->materials&&
                *references.count1C0()==scene->materials.size()&&
                scene->materials.size()==windowBindings_[component].size()&&
                scene->materials.size()==textures[component].size(),
                "Canonical engine PSG/material ordinal identity differs");
            //Borrow only immutable cold mapping descriptors, as opponent cold
            //storage preparation does. No batch is built, no matrix is read, and
            //these descriptor tags are not a receipt that graphics has run.
            g::VehicleDrawBatchInput50CO nativeDraw;
            s::need(drawAssets.batchInput(0,g::ModelPalettePhase49S::selectedRenderPreparation,
                    g::ModelPaletteSceneState49S::completeCurrentScene,nativeDraw)&&
                geometry.hasPalette20()&&geometry.paletteCount20()==1&&geometry.palette20(0)&&
                model.fields().sourceRows1B8&&model.sourceRowCount()==resource->nodeCount(),
                "Canonical engine actual palette/source-row ownership missing");
            const auto& selected=nativeDraw.selection[component];
            const auto& palette=*geometry.palette20(0);
            s::need(selected.sourceNodeCount==model.sourceRowCount()&&palette.size()==model.sourceRowCount()&&
                selected.paletteCount==palette.size()&&selected.sourceNodes&&selected.paletteIds,
                "Canonical engine native row/palette extent differs");
            for(std::uint32_t row=0;row<model.sourceRowCount();++row)
                s::need(model.sourceRow(row)&&model.fields().sourceRows1B8[row]==model.sourceRow(row)&&
                    selected.sourceNodes[row]==model.sourceRow(row)->sceneIndex&&
                    selected.paletteIds[row]==palette[row],
                    "Canonical engine draw does not use actual model source rows");
            s::need(!scene->lods.empty()&&resource->scene()->lods.size()==scene->lods.size(),
                "Canonical engine LOD geometry differs");
            for(std::size_t lod=0;lod<scene->lods.size();++lod){
                s::need(resource->scene()->lods[lod].surfaces.size()==scene->lods[lod].surfaces.size(),
                    "Canonical engine surface extent differs");
                for(std::size_t surface=0;surface<scene->lods[lod].surfaces.size();++surface){
                    const auto* material=geometry.surfaceMaterial00(lod,surface);
                    s::need(material&&*material>=0&&static_cast<std::uint32_t>(*material)==
                        scene->lods[lod].surfaces[surface].material_index,
                        "Canonical engine surface material ordinal differs");
                }
            }
            s::need(textureSources_.size()==textureSelections.size()&&
                textureSources_.size()==materials.textureCount(),"Canonical texture source identities missing");
            auto& bindings=drafts[engine].bindings;bindings=windowBindings_[component];
            auto& selections=drafts[engine].selections;selections=textures[component];
            auto& bodyColorModes=drafts[engine].bodyColorModes;
            bodyColorModes.resize(scene->materials.size(),0);
            std::size_t bodyColorCount=0;
            for(std::size_t ordinal=0;ordinal<scene->materials.size();++ordinal){
                const auto* handle=references.handle1C4(ordinal);
                const a::OriginalStandardMaterialObject50LA* material=nullptr;
                a::OriginalStandardMaterialDiagnostic50LA diagnostic;
                s::need(handle&&handle==model.fields().materials1C4+ordinal&&
                    domain.materials382B38()->resolve298E60(*handle,material,diagnostic)&&
                    material&&material->constructed29B550()&&material->row40()&&*material->row40()==*handle&&
                    material->passCount48()&&*material->passCount48()==1&&material->source()&&
                    material->source()->member()&&material->source()->originalBytes()&&
                    material->textures383400()==domain.textures383400().get(),
                    "Canonical engine model1C4 does not resolve a complete actual material");
                const auto source=materialSources_.find(rig_detail::lower(scene->materials[ordinal]));
                s::need(source!=materialSources_.end()&&
                    visual_detail::sameSourceMember(*material->source()->member(),source->second.member)&&
                    s::sha256(*material->source()->originalBytes())==source->second.sha256,
                    "Canonical engine PSM differs from authenticated visual source");
                a::OriginalStandardMaterialPassView50LA pass;
                s::need(material->pass4C(0,pass)&&pass.references04&&*pass.references04>0&&
                    pass.program18&&pass.texture1A&&pass.uv1E&&pass.mapCount20&&
                    *pass.mapCount20>=1&&*pass.mapCount20<=2&&pass.flags21&&(*pass.flags21&2u)&&
                    pass.state30&&pass.state30->context(0),
                    "Canonical engine displayed pass/map/context is unavailable");
                a::OriginalStandardProgramRowView50LA program;
                s::need(domain.programs382B70()->row(*pass.program18,program)&&program.canonicalName&&
                    rig_detail::lower(program.canonicalName)==rig_detail::lower(textures[component][ordinal].program),
                    "Canonical engine program differs from authenticated display selection");
                const auto programName=rig_detail::lower(program.canonicalName);
                const bool flame=programName=="diffusetextranscompressed";
                s::need(flame||programName=="specularmapdualcompressed",
                    "Canonical engine program has no admitted host base-map projection");
                s::need(!flame||*pass.mapCount20==1,
                    "Canonical engine flame requires its original single map");
                s::need(pass.state30->contextCount()>=1&&pass.state30->contextCount()<=2&&
                    (!flame||pass.state30->contextCount()==1)&&(*pass.uv1E)[0]==0,
                    "Canonical engine native base-map context/UV projection is unsupported");
                a::OriginalStandardTextureRowView50LA textureRow;
                const auto token=(*pass.texture1A)[0];
                a::OriginalStandardMaterialMapBindingView50LA firstMap;
                s::need(domain.materials382B38()->prepareColdRepresentativeMap249610(
                    domain.coldCollector14D440(),*handle,0,0,firstMap,diagnostic)&&
                    firstMap.preparation==a::OriginalStandardMaterialMapPreparation50LA::ready&&
                    firstMap.textureToken==token&&firstMap.texture,
                    "Canonical engine first-map representative is unavailable");
                s::need(domain.textures383400()->row(token,textureRow)&&textureRow.references08&&
                    *textureRow.references08>0&&textureRow.object04&&
                    textureRow.object04==firstMap.texture&&
                    textureRow.object04->constructed24DA30()&&textureRow.object04->sourceMember()&&
                    textureRow.object04->originalBytes(),
                    "Canonical engine first-map token has no actual PST resource");
                const auto& texture=*textureRow.object04;
                a::OriginalStandardTextureLevelView50LA level;
                s::need(texture.level100(0,level)&&level.width104>0&&level.height110>0&&
                    level.linearSourceIndices&&level.linearSourceBytes,
                    "Canonical engine native selected PST mip is unavailable");
                const auto textureSha=s::sha256(*texture.originalBytes());
                std::uint32_t bank=a::unresolvedMaterialTexture50EA;
                for(std::uint32_t i=0;i<textureSources_.size();++i){
                    const auto& identity=textureSources_[i];const auto& retained=textureSelections[i];
                    if(!visual_detail::sameSourceMember(identity.member,*texture.sourceMember())||
                       identity.sha256!=textureSha||retained.mipIndex!=level.sourceMipOrdinal||
                       retained.width!=static_cast<std::uint32_t>(level.width104)||
                       retained.height!=static_cast<std::uint32_t>(level.height110))continue;
                    s::need(bank==a::unresolvedMaterialTexture50EA,
                        "Canonical engine PST/mip maps to ambiguous native bank entries");
                    bank=i;
                }
                s::need(bank!=a::unresolvedMaterialTexture50EA&&bank<samplers.size()&&
                    materials.texture(bank)&&materials.texture(bank)->identity==textureSelections[bank].identity&&
                    textureSelections[bank].key==textures[component][ordinal].textureKey,
                    "Canonical engine PST/mip has no authenticated native RGBA bank entry");
                //Linear filtering, native depth comparison and lighting remain
                //the existing explicit host policy. The consumed source fields
                //below select only its admitted blend/depth-write/wrap subset.
                const auto& state=firstMap.contextAfter249610;
                using Field=a::OriginalUiGsField50KG;
                const auto value=[&](Field field){return state.fields[static_cast<std::size_t>(field)].value;};
                const auto blend=value(Field::alphaBlendingOnOff),depth=value(Field::zWriteDisabled);
                const auto wrapU=value(Field::wrapModeHorizontal),wrapV=value(Field::wrapModeVertical);
                s::need((blend==0||blend==1)&&(depth==0||depth==1)&&
                    (wrapU==0||wrapU==1)&&(wrapV==0||wrapV==1),
                    "Canonical engine context has unsupported native blend/depth/wrap policy");
                p::VehicleWindowMaterialBinding50DS binding{p::VehicleWindowMaterialMode50DS::requiredTexture,bank};
                binding.depthWrite=depth==0;
                if(flame){
                    s::need(blend==1&&value(Field::inputColorA)==0&&value(Field::inputColorB)==2&&
                        value(Field::inputAlphaC)==2&&value(Field::inputColorD)==1&&
                        value(Field::fixedAlphaValue)==255&&depth==1,
                        "Canonical flame context has unsupported native additive projection");
                    binding.blend=p::VehicleWindowBlend50DS::additive;
                    binding.fixedBlendAlpha=true;binding.alphaScale=255.f/128.f;
                }else if(blend){
                    s::need(value(Field::inputColorA)==0&&value(Field::inputColorB)==1&&
                        value(Field::inputAlphaC)==0&&value(Field::inputColorD)==1,
                        "Canonical body context has unsupported native source-alpha projection");
                    binding.blend=p::VehicleWindowBlend50DS::sourceAlpha;
                    binding.alphaScale=(255.f/128.f)*(255.f/128.f);
                }
                if(useOriginalBodyColors50LA&&!flame){
                    // The source pair is admitted together; no active parameter,
                    // animation or alternate one-node program path is substituted.
                    s::need(programName=="specularmapdualcompressed"&&*pass.mapCount20==2&&
                        scene->nodes.size()>1&&resource->scene()->nodes.size()==scene->nodes.size()&&
                        (*pass.flags21&3u)==3u&&(*pass.uv1E)[1]==0&&!pass.animation10&&
                        pass.state30->contextCount()==2&&pass.parameterCount28&&*pass.parameterCount28==0&&
                        pass.parameterMask24&&!*pass.parameterMask24&&!pass.parameters34&&
                        value(Field::textureFunction)==0&&value(Field::textureColorComponent)==1&&
                        value(Field::textureMappingOnOff)==1&&value(Field::gouraudShadingOnOff)==1&&
                        value(Field::textureCoordinatesSTQorUV)==0&&
                        value(Field::colorClampingMethod)==1&&blend==0&&depth==0&&
                        value(Field::alphaTestOnOff)==0&&value(Field::destinationAlphaTestOnOff)==0,
                        "Canonical engine body has no admitted source RGB/MODULATE projection");
                    a::OriginalStandardMaterialMapBindingView50LA secondMap;
                    s::need(domain.materials382B38()->prepareColdRepresentativeMap249610(
                        domain.coldCollector14D440(),*handle,0,1,secondMap,diagnostic)&&
                        secondMap.preparation==a::OriginalStandardMaterialMapPreparation50LA::ready&&
                        secondMap.textureToken==(*pass.texture1A)[1]&&secondMap.texture,
                        "Canonical engine second-map representative is unavailable");
                    const auto& second=secondMap.contextAfter249610;
                    const auto secondValue=[&](Field field){return second.fields[static_cast<std::size_t>(field)].value;};
                    for(std::size_t contextIndex=0;contextIndex<2;++contextIndex){
                        const auto& context=contextIndex?secondMap.contextAfter249610:firstMap.contextAfter249610;
                        const auto contextValue=[&](Field field){return context.fields[static_cast<std::size_t>(field)].value;};
                        s::need(context.runtime435670==0&&
                            contextValue(Field::environmentContext0or1)==static_cast<std::int32_t>(contextIndex)&&
                            contextValue(Field::depthTestMethod)==2&&contextValue(Field::textureFunction)==0&&
                            contextValue(Field::textureColorComponent)==1&&contextValue(Field::gouraudShadingOnOff)==1&&
                            contextValue(Field::textureMappingOnOff)==1&&contextValue(Field::textureCoordinatesSTQorUV)==0&&
                            contextValue(Field::colorClampingMethod)==1&&contextValue(Field::alphaCorrectionValue)==0&&
                            contextValue(Field::alphaBlendingControlInUnitsOfPixels)==0&&
                            contextValue(Field::onePassAntiAliasingOnOff)==0&&
                            contextValue(Field::fragmentValueControlFixedOrNot)==0&&
                            contextValue(Field::destinationAlphaTestOnOff)==0&&
                            (contextValue(Field::foggingOnOff)==0||contextValue(Field::foggingOnOff)==1),
                            "Canonical engine dual context has unsupported source pair state");
                    }
                    const auto alphaTest=secondValue(Field::alphaTestOnOff);
                    // Actual DefaultMT inherits ATE1/ATST6/AFAIL0 and supplies
                    // AREF0. A blank map2 field retains this GREATER0/KEEP test.
                    s::need((alphaTest==0||(alphaTest==1&&secondValue(Field::alphaTestMethod)==6&&
                            secondValue(Field::alphaReferenceValue)==0&&
                            secondValue(Field::processingMethodWhenFailedInAlphaTest)==0))&&
                        secondValue(Field::alphaBlendingOnOff)==1&&secondValue(Field::inputColorA)==0&&
                        secondValue(Field::inputColorB)==2&&secondValue(Field::inputAlphaC)==1&&
                        secondValue(Field::inputColorD)==1&&secondValue(Field::zWriteDisabled)==1,
                        "Canonical engine second context has unsupported source alpha/blend/depth state");
                    const auto ta0=secondValue(Field::sourceAlphaValueWhenAis0);
                    const auto ta1=secondValue(Field::sourceAlphaValueWhenAis1);
                    const auto aem=secondValue(Field::methodOfExpandingTextureAlpha);
                    const auto secondU=secondValue(Field::wrapModeHorizontal),secondV=secondValue(Field::wrapModeVertical);
                    s::need(ta0>=0&&ta0<=255&&ta1>=0&&ta1<=255&&(aem==0||aem==1)&&
                        (secondU==0||secondU==1)&&(secondV==0||secondV==1),
                        "Canonical engine second context TEXA/wrap is unsupported");
                    a::OriginalStandardTextureRowView50LA secondRow;
                    s::need(domain.textures383400()->row((*pass.texture1A)[1],secondRow)&&
                        secondRow.name00&&secondRow.references08&&*secondRow.references08>0&&secondRow.object04&&
                        secondRow.object04==secondMap.texture&&
                        secondRow.object04->constructed24DA30()&&secondRow.object04->references00()&&
                        *secondRow.object04->references00()>0&&secondRow.object04->name20()&&
                        secondRow.object04->sourceMember()&&secondRow.object04->originalBytes()&&material->name00(),
                        "Canonical engine second-map token has no actual live resource");
                    const auto& secondTexture=*secondRow.object04;
                    const auto& authored=componentMaterials_[component][ordinal]->passes.front();
                    const auto slash=material->name00()->find_last_of('\\');
                    const auto prefix=slash==std::string::npos?std::string{}:material->name00()->substr(0,slash+1);
                    s::need(authored.pass==1&&authored.textures.size()==2&&authored.textures[1].slot==2&&
                        visual_detail::sameSourceMember(*secondRow.name00,prefix+authored.textures[1].name)&&
                        visual_detail::sameSourceMember(*secondTexture.name20(),*secondRow.name00+".tga")&&
                        visual_detail::sameSourceMember(*secondTexture.sourceMember(),*secondTexture.name20()),
                        "Canonical second-map row/resource names differ from the actual PSM slot");
                    using Provider=a::OriginalStandardArchiveProvider50LA;
                    const auto* provider=domain.archiveProvider3E1A70().get();
                    Provider::MemberView sourceMember,priorMember;
                    const auto& sourceBytes=*secondTexture.originalBytes();
                    s::need(provider&&provider->sealed()&&
                        provider->lookup132820(*secondRow.name00+".pst",priorMember)==Provider::Lookup::absent&&
                        provider->lookup132820(*secondRow.name00+".ipu",priorMember)==Provider::Lookup::absent&&
                        provider->lookup132820(*secondTexture.sourceMember(),sourceMember)==Provider::Lookup::present&&
                        sourceMember.member&&sourceMember.data&&sourceMember.size==sourceBytes.size()&&
                        visual_detail::sameSourceMember(*sourceMember.member,*secondTexture.sourceMember())&&
                        std::equal(sourceBytes.begin(),sourceBytes.end(),sourceMember.data),
                        "Canonical second-map bytes differ from the same actual mounted source");
                    a::OriginalStandardTextureLevelView50LA secondLevel;
                    s::need(secondTexture.sourceFormat()&&
                        *secondTexture.sourceFormat()==a::OriginalStandardTextureSourceFormat50LA::tgaBgr24&&
                        secondTexture.formatAC()&&*secondTexture.formatAC()==1555&&
                        secondTexture.bitsPerPixelE8()&&*secondTexture.bitsPerPixelE8()==16&&
                        secondTexture.level100(0,secondLevel)&&secondLevel.width104==64&&secondLevel.height110==64&&
                        secondLevel.storage==a::OriginalStandardTextureLevelStorage50LA::tgaConverted1555&&
                        secondLevel.packed1555&&secondLevel.packed1555Words==4096,
                        "Canonical second-map largest native1555 level is unsupported");
                    const std::array<std::int32_t,5> texaWrap{{ta0,ta1,aem,secondU,secondV}};
                    std::uint32_t specularIndex=a::unresolvedMaterialTexture50EA;
                    for(std::uint32_t i=0;i<specularProjections.size();++i){
                        const auto& projection=specularProjections[i];
                        if(projection.resource==&secondTexture&&projection.level==0&&projection.texaWrap==texaWrap){
                            specularIndex=i;break;
                        }
                    }
                    if(specularIndex==a::unresolvedMaterialTexture50EA){
                        s::need(specularProjections.size()<32,"Canonical second-map native bank capacity");
                        a::TextureMip48E native;native.width=native.height=64;native.rgba.resize(4096u*4u);
                        for(std::size_t pixel=0;pixel<4096;++pixel){
                            const auto word=secondLevel.packed1555[pixel];
                            native.rgba[4*pixel]=static_cast<std::uint8_t>((word&31u)<<3);
                            native.rgba[4*pixel+1]=static_cast<std::uint8_t>(((word>>5)&31u)<<3);
                            native.rgba[4*pixel+2]=static_cast<std::uint8_t>(((word>>10)&31u)<<3);
                            native.rgba[4*pixel+3]=static_cast<std::uint8_t>((word&0x8000u)?ta1:((aem&&word==0)?0:ta0));
                        }
                        // The resource already owns the original row projection.
                        // No second flip, RGB5 replication, mip generation, source
                        // decoder or Controls/specmap3 identity enters this copy.
                        SpecularProjection projection;projection.resource=&secondTexture;
                        projection.texaWrap=texaWrap;projection.identity=visual_detail::nextSpecularIdentity50LA();
                        a::DecodeError48C encodeError;
                        s::need(a::encodeTextureLevel48E(native,projection.bytes,encodeError)&&
                            projection.bytes.size()==native.rgba.size()+16&&
                            std::equal(native.rgba.begin(),native.rgba.end(),projection.bytes.begin()+16),
                            "Canonical second-map native RGBA serialization differs");
                        specularIndex=static_cast<std::uint32_t>(specularProjections.size());
                        specularProjections.push_back(std::move(projection));
                        specularSamplers.push_back({p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
                            secondU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat,
                            secondV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat});
                    }
                    specularSlots.push_back({{component+1,static_cast<std::uint32_t>(ordinal),1,2},
                        specularProjections[specularIndex].identity,a::MaterialTextureRequirement50EA::required});
                    binding.sourceDiffuseCompressed50LA=true;
                    binding.sourceSpecularPair50LA=true;binding.specularTextureIndex50LA=specularIndex;
                    binding.sourceBaseFog50LA=value(Field::foggingOnOff)!=0;
                    binding.sourceSpecularFog50LA=secondValue(Field::foggingOnOff)!=0;
                    binding.sourceSpecularAlphaTest50LA=alphaTest!=0;binding.depthEqual=true;
                    bodyColorModes[ordinal]=1;++bodyColorCount;
                }
                if(engine>=2u){
                    const auto& retained=windowBindings_[component][ordinal];
                    s::need(binding.blend==retained.blend&&binding.depthWrite==retained.depthWrite&&
                        binding.fixedBlendAlpha==retained.fixedBlendAlpha&&binding.alphaScale==retained.alphaScale,
                        "Canonical lower-engine context changes the existing native base-map policy");
                }
                const auto u=wrapU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat;
                const auto v=wrapV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat;
                //One bank texture may also serve the left/chariot path. Preserve
                //that admitted shared sampling contract instead of changing peers.
                s::need(samplers[bank].s==u&&samplers[bank].t==v,
                    "Canonical engine texture needs a distinct native sampler publication");
                samplers[bank].s=u;samplers[bank].t=v;bindings[ordinal]=binding;
                selections[ordinal].textureIndex=bank;
                selections[ordinal].textureIdentity=textureSelections[bank].identity;
                selections[ordinal].mipIndex=level.sourceMipOrdinal;
            }
            s::need(!useOriginalBodyColors50LA||bodyColorCount,
                "Canonical engine has no actual body-color material");
            totalBodyColorCount+=bodyColorCount;
        }
        if(useOriginalBodyColors50LA){
            s::need(!specularProjections.empty()&&specularSlots.size()==totalBodyColorCount&&
                specularSamplers.size()==specularProjections.size(),"Canonical second-map publication is incomplete");
            std::vector<a::MaterialTextureAssetInput50EA> specularInput;
            specularInput.reserve(specularProjections.size());
            for(const auto& projection:specularProjections)
                specularInput.push_back({projection.identity,&projection.bytes});
            const auto count=static_cast<std::uint32_t>(specularInput.size());
            const auto slotCount=static_cast<std::uint32_t>(specularSlots.size());
            const a::MaterialTextureAssetsInput50EA input{specularInput.data(),count,specularSlots.data(),slotCount,
                {count,slotCount,std::size_t(count)*(4096u*4u+16u),std::size_t(count)*4096u*4u,64}};
            a::MaterialTextureAssetDiagnostic50EA diagnostic;
            // Final fallible operation. 50EA transactionally owns exactly the
            // serialized native pixels. Nothing below can resize or allocate.
            if(!canonicalEngineSpecularMaterials_.load(input,diagnostic))
                s::need(false,"Canonical second-map native bank rejected: "+diagnostic.parserMessage);
            canonicalEngineSpecularSamplers_.swap(specularSamplers);
        }
        for(unsigned engine=0;engine<engineCount;++engine){
            const unsigned component=engine+1u;
            windowBindings_[component].swap(drafts[engine].bindings);
            textures[component].swap(drafts[engine].selections);
            if(useOriginalBodyColors50LA){
                auto& modes=engine==0?canonicalLeftBodyColorModes50LA_:
                    engine==1?canonicalBodyColorModes50LA_:canonicalLowerBodyColorModes50LA_[engine-2u];
                modes.swap(drafts[engine].bodyColorModes);
            }
        }
        samplers_.swap(samplers);
        canonicalRightBound_=canonicalLeftBound_=true;
        canonicalLowerBound_[0]=canonicalLowerBound_[1]=engineCount==4u;
    }
    bool canonicalRightBound()const noexcept{return canonicalRightBound_;}
    // Per-frame source-color input points only at this immutable canonical
    // material selection and the actual completed selected-light publication.
    // The numeric draw producer copies publication identity into its batch;
    // the renderer checks that same identity before using generated RGB.
    bool canonicalRightSourceColors50LA(const g::NativeGcSelectedLightsCandidate& lights,
        std::uint32_t actualRacerSlot,g::VehicleDrawBatchSourceColors50LA& output)const noexcept {
        constexpr auto component=static_cast<unsigned>(g::VehicleDrawComponent50CM::rightEngine);
        if(!canonicalRightBound_||canonicalBodyColorModes50LA_.empty()||!lights.ready()||
           lights.racerSlot()!=actualRacerSlot||lights.componentCount()!=componentCount||
           !lights.component(component))return false;
        g::VehicleDrawBatchSourceColors50LA next;
        next.component[component]={canonicalBodyColorModes50LA_.data(),
            static_cast<std::uint32_t>(canonicalBodyColorModes50LA_.size()),&lights};
        next.component[component].prepareSpecular50LA=canonicalEngineSpecularMaterials_.loaded();
        output=next;return true;
    }
    // One completed light publication and the actual component rows. Each
    // requests its own model bounds/root/normals for the shared cold pair bank.
    // The optional lower pair is complete or absent; failure preserves output.
    bool canonicalEngineSourceColors50LA(const g::NativeGcSelectedLightsCandidate& lights,
        std::uint32_t actualRacerSlot,g::VehicleDrawBatchSourceColors50LA& output)const noexcept {
        constexpr auto left=static_cast<unsigned>(g::VehicleDrawComponent50CM::leftEngine);
        g::VehicleDrawBatchSourceColors50LA next;
        if(!canonicalRightSourceColors50LA(lights,actualRacerSlot,next)||!canonicalLeftBound_||
           canonicalLeftBodyColorModes50LA_.empty()||!lights.component(left))return false;
        next.component[left]={canonicalLeftBodyColorModes50LA_.data(),
            static_cast<std::uint32_t>(canonicalLeftBodyColorModes50LA_.size()),&lights};
        next.component[left].prepareSpecular50LA=canonicalEngineSpecularMaterials_.loaded();
        if(componentCount==5u){
            if(podId!="POD02"||!canonicalLowerBound_[0]||!canonicalLowerBound_[1])return false;
            for(unsigned lower=0;lower<2;++lower){
                const unsigned component=lower+3u;
                const auto& modes=canonicalLowerBodyColorModes50LA_[lower];
                if(modes.empty()||!lights.component(component))return false;
                next.component[component]={modes.data(),static_cast<std::uint32_t>(modes.size()),&lights};
                next.component[component].prepareSpecular50LA=canonicalEngineSpecularMaterials_.loaded();
            }
        }
        output=next;return true;
    }
    rr::platform::VehicleWindowMaterials50DS windowMaterials(const rr::platform::MaterialTextureGl50EB* gpu,
        const rr::platform::MaterialTextureGl50EB* specularGpu=nullptr)const {
        visual_detail::shared::need(gpu&&gpu->initialized()&&gpu->textureCount()==materials.textureCount(),
            "Original per-pod texture publication unavailable");
        for(std::uint32_t i=0;i<materials.textureCount();++i)
            visual_detail::shared::need(gpu->asset(i)&&materials.texture(i)&&
                gpu->asset(i)->identity==materials.texture(i)->identity,
                "Original texture publication belongs to another racer");
        if(canonicalEngineSpecularMaterials_.loaded()){
            visual_detail::shared::need(specularGpu&&specularGpu->initialized()&&
                specularGpu->textureCount()==canonicalEngineSpecularMaterials_.textureCount()&&
                canonicalEngineSpecularSamplers_.size()==canonicalEngineSpecularMaterials_.textureCount(),
                "Canonical second-map GPU publication unavailable");
            for(std::uint32_t i=0;i<canonicalEngineSpecularMaterials_.textureCount();++i)
                visual_detail::shared::need(specularGpu->asset(i)&&canonicalEngineSpecularMaterials_.texture(i)&&
                    specularGpu->asset(i)->identity==canonicalEngineSpecularMaterials_.texture(i)->identity,
                    "Canonical second-map GPU publication has a different resource/TEXA/wrap identity");
        }else visual_detail::shared::need(!specularGpu,"Unexpected canonical second-map GPU publication");
        rr::platform::VehicleWindowMaterials50DS result{};result.textures=gpu;result.componentCount=componentCount;
        result.specularTextures50LA=specularGpu;
        for(unsigned c=0;c<componentCount;++c)result.component[c]={windowBindings_[c].data(),static_cast<std::uint32_t>(windowBindings_[c].size())};
        materialsPublished_=true;
        return result;
    }
private:
    // No retained selection catalog or cached geometry is involved. Each native
    // owner is built directly from this Rig's authenticated original archive.
    // The existing canonical closure below still proves the actual model,
    // geometry, material references, textures and source-node identities before
    // any render publication. This is cold conversion only; no guest execution.
    void loadOriginalDisc(const Rig& rig) {
        namespace d=visual_detail;namespace s=visual_detail::shared;
        podId=rig.podId;displayName=rig.displayName;componentCount=rig.modelCount;
        s::need(!displayName.empty()&&(componentCount==3||componentCount==5),
            "Direct original pod identity/topology unavailable");
        chariotPsgMember_=rig.psgMembers[Rig::chariot];
        chariotPsgSha256_=s::sha256(rig.psg[Rig::chariot]);chariotModelIndex_=rig.roots[Rig::chariot];
        leftPsgMember_=rig.psgMembers[Rig::left];
        leftPsgSha256_=s::sha256(rig.psg[Rig::left]);leftModelIndex_=rig.roots[Rig::left];
        rightPsgMember_=rig.psgMembers[Rig::right];
        rightPsgSha256_=s::sha256(rig.psg[Rig::right]);rightModelIndex_=rig.roots[Rig::right];
        if(componentCount==5)for(unsigned side=0;side<2;++side){const auto model=side?Rig::bottomRight:Rig::bottomLeft;
            lowerPsg_[side]={rig.psgMembers[model],s::sha256(rig.psg[model])};lowerModelIndex_[side]=rig.roots[model];}
        const std::uint64_t textureIdentityBase=(std::uint64_t((podId[3]-'0')*10+(podId[4]-'0'))+1u)<<32;
        a::VehicleDrawAssetsInput50CR drawInput{};drawInput.componentCount=componentCount;
        std::array<d::Bytes,5> sceneBytes;
        std::array<std::array<std::uint8_t,a::maxModelNodes49O>,5> palettes{};
        constexpr unsigned mapped[]={Rig::chariot,Rig::left,Rig::right,Rig::bottomLeft,Rig::bottomRight};
        for(unsigned c=0;c<componentCount;++c){
            const auto model=mapped[c];const auto& psg=rig.psg[model];
            a::Scene48D scene;a::DecodeError48C error;std::string why;
            s::need(a::decodeScene48D(psg,scene,error),"Direct original PSG decode rejected: "+rig.psgMembers[model]+" "+error.code);
            s::need(a::encodeSceneAsset48D(scene,sceneBytes[c],why),"Direct native scene preparation rejected: "+why);
            const auto count=rig.hierarchy[model].nodeCount;const auto padded=(count+3u)&~3u;
            s::need(count&&count<=a::maxModelNodes49O&&psg.size()>=12u+padded,"Direct original PSG palette bounds");
            const auto suffix=psg.size()-(12u+padded);
            s::need(s::u32(psg.data()+suffix)==1&&s::u32(psg.data()+suffix+4)==padded&&
                s::u32(psg.data()+suffix+8+padded)==count,"Direct original PSG palette framing");
            for(unsigned n=0;n<count;++n)palettes[c][n]=psg[suffix+8+n];
            drawInput.component[c]={&sceneBytes[c],rig.sourceNodes[model],count,palettes[c].data(),count};
            drawInput.component[c].sourcePsgBytes=&psg;
        }
        a::VehicleDrawAssetDiagnostic50CR drawDiagnostic;
        s::need(drawAssets.load(drawInput,drawDiagnostic),"Direct original draw owner rejected: "+drawDiagnostic.parserMessage);
        std::map<std::string,std::uint32_t> textureIndices;
        std::vector<d::Bytes> textureBytes;std::vector<a::MaterialTextureSlotInput50EA> slots;
        std::size_t rgbaBytes=0,sourceBytes=0;
        for(unsigned c=0;c<componentCount;++c){
            const auto* scene=drawAssets.scene(static_cast<g::VehicleDrawComponent50CM>(c));
            s::need(scene&&scene->materials.size()<=4096,"Direct original component material bound");
            textures[c].reserve(scene->materials.size());windowBindings_[c].reserve(scene->materials.size());
            for(std::uint32_t mi=0;mi<scene->materials.size();++mi){
                const auto materialName=rig_detail::lower(scene->materials[mi]);
                auto found=originalMaterials_.find(materialName);
                if(found==originalMaterials_.end()){
                    const auto* entry=rig.originalEntry("textures/"+materialName+".psm");
                    s::need(entry,"Direct original scene PSM missing: "+materialName);
                    const auto bytes=rig.originalMember(entry->name);const auto digest=s::sha256(bytes);
                    a::Material48E material;a::DecodeError48C error;
                    s::need(a::decodeMaterial48E(bytes,material,error),"Direct original PSM decode rejected: "+entry->name+" "+error.code);
                    const auto key="disc:"+rig_detail::lower(entry->name)+":"+digest;
                    d::Json record={{"origin","original-disc-runtime"},{"key",key},{"source_member",entry->name},
                        {"source_sha256",digest},{"general",d::fieldsJson(material.general)},{"passes",d::Json::array()}};
                    for(const auto& pass:material.passes){d::Json part={{"pass",pass.pass},{"program",pass.microprogram},
                        {"texture_count_defaulted",pass.texture_count_defaulted},{"fields",d::fieldsJson(pass.fields)},
                        {"textures",d::Json::array()}};
                        for(const auto& map:pass.textures)part["textures"].push_back({{"slot",map.slot},{"name",map.name}});
                        record["passes"].push_back(std::move(part));}
                    materialSources_.emplace(materialName,SourceIdentity{entry->name,digest});
                    materialRecords_.emplace(materialName,std::move(record));
                    found=originalMaterials_.emplace(materialName,std::move(material)).first;
                }
                const auto& material=found->second;const auto& record=materialRecords_.at(materialName);
                s::need(!material.passes.empty()&&material.passes.front().pass==1,"Direct original pass1 absent");
                const auto& displayed=material.passes.front();
                const bool flame=rig_detail::lower(displayed.microprogram)=="diffusetextranscompressed"&&
                    rig_detail::lower(d::field(displayed.fields,"VertexAnimation"))=="unique";
                if(flame)s::need(c>0,"Direct original flame attached to chariot");
                const a::ResourceEntry48C* displayedTexture=nullptr;std::string displayedTextureName;
                for(const auto& pass:material.passes)for(const auto& map:pass.textures){
                    const auto* entry=rig.originalEntry("textures/"+rig_detail::lower(map.name)+".pst");
                    pod_materials50ed_fixture::AuthoredSlot authored;
                    authored.component=c;authored.materialIndex=mi;authored.pass=pass.pass;authored.slot=map.slot;
                    authored.materialName=scene->materials[mi];authored.materialKey=record.at("key").get<std::string>();
                    authored.program=pass.microprogram;authored.textureName=map.name;
                    authored.selectedForDisplay=pass.pass==1&&map.slot==1;authored.resolvedInSelection=entry!=nullptr;
                    if(entry)authored.textureKey="disc:"+rig_detail::lower(entry->name)+":"+s::sha256(rig.originalMember(entry->name));
                    else authored.unresolvedReason="missing_original_archive_member";
                    if(authored.selectedForDisplay){s::need(!displayedTexture,"Direct original duplicate base map");
                        displayedTexture=entry;displayedTextureName=map.name;}
                    authoredSlots.push_back(std::move(authored));
                }
                s::need(displayedTexture,"Direct original required base texture missing: "+materialName);
                const auto source=rig.originalMember(displayedTexture->name);const auto digest=s::sha256(source);
                const auto key="disc:"+rig_detail::lower(displayedTexture->name)+":"+digest;
                auto known=textureIndices.find(key);
                if(known==textureIndices.end()){
                    a::Texture48E texture;a::DecodeError48C error;
                    s::need(a::decodeTexture48E(source,texture,error),"Direct original PST decode rejected: "+displayedTexture->name+" "+error.code);
                    s::need(!texture.mips.empty()&&texture.mips.size()<=7,"Direct original PST mip bound");
                    const auto& mip=texture.mips.back();const auto index=static_cast<std::uint32_t>(textureBytes.size());
                    s::need(index<32&&mip.width&&mip.height&&mip.width<=1024&&mip.height<=1024,
                        "Direct original texture bank/dimension bound");
                    pod_materials50ed_fixture::TextureSelection meta;
                    meta.identity=textureIdentityBase+index+1;meta.bankIndex=index;meta.mipIndex=texture.mips.size()-1;
                    meta.width=mip.width;meta.height=mip.height;meta.key=key;
                    meta.path=displayedTexture->name;meta.sha256=s::sha256(texture.base_rrtex);
                    textureSelections.push_back(std::move(meta));textureBytes.push_back(std::move(texture.base_rrtex));
                    textureSources_.push_back({displayedTexture->name,digest});
                    samplers_.push_back({rr::platform::MaterialTextureFilter50EB::linear,rr::platform::MaterialTextureFilter50EB::linear,
                        rr::platform::MaterialTextureWrap50EB::repeat,rr::platform::MaterialTextureWrap50EB::repeat});
                    s::add(rgbaBytes,static_cast<std::size_t>(mip.width)*mip.height*4,16u*1024u*1024u,"Direct original RGBA budget");
                    s::add(sourceBytes,textureBytes.back().size(),16u*1024u*1024u,"Direct original texture source budget");
                    known=textureIndices.emplace(key,index).first;
                }
                const auto textureIndex=known->second;
                pod_materials50ed_fixture::Selection choice;choice.materialIndex=mi;choice.pass=choice.slot=1;
                choice.assetIdentity=c+1;choice.bankSlot=static_cast<std::uint32_t>(slots.size());choice.textureIndex=textureIndex;
                choice.textureIdentity=textureSelections[textureIndex].identity;choice.mipIndex=textureSelections[textureIndex].mipIndex;
                choice.materialName=scene->materials[mi];choice.materialKey=record.at("key").get<std::string>();
                choice.program=displayed.microprogram;choice.textureName=displayedTextureName;choice.textureKey=key;
                choice.requirement=a::MaterialTextureRequirement50EA::required;
                slots.push_back({{c+1,mi,1,1},choice.textureIdentity,choice.requirement});
                textures[c].push_back(std::move(choice));windowBindings_[c].push_back(d::policy(displayed,textureIndex,flame));
                if(flame){flameMaterials_[c].push_back(mi);s::need(d::field(displayed.fields,"wrapModeVertical")=="1",
                    "Direct original flame vertical clamp unsupported");samplers_[textureIndex].t=rr::platform::MaterialTextureWrap50EB::clampToEdge;}
                componentMaterials_[c].push_back(&material);
            }
        }
        std::vector<a::MaterialTextureAssetInput50EA> textureInput;textureInput.reserve(textureBytes.size());
        for(std::uint32_t i=0;i<textureBytes.size();++i)textureInput.push_back({textureSelections[i].identity,&textureBytes[i]});
        a::MaterialTextureAssetsInput50EA materialInput{textureInput.data(),static_cast<std::uint32_t>(textureInput.size()),
            slots.data(),static_cast<std::uint32_t>(slots.size()),
            {static_cast<std::uint32_t>(textureInput.size()),static_cast<std::uint32_t>(slots.size()),sourceBytes,rgbaBytes,1024}};
        a::MaterialTextureAssetDiagnostic50EA diagnostic;
        s::need(materials.load(materialInput,diagnostic),"Direct original texture owner rejected: "+diagnostic.parserMessage);
    }
    struct SourceIdentity {std::string member,sha256;};
    std::map<std::string,SourceIdentity> materialSources_;
    std::vector<SourceIdentity> textureSources_;
    std::string chariotPsgMember_,chariotPsgSha256_;
    g::SceneNodeIndex49P chariotModelIndex_=g::SceneNodeIndex49P::none;
    bool canonicalChariotBound_=false;
    std::string leftPsgMember_,leftPsgSha256_;
    g::SceneNodeIndex49P leftModelIndex_=g::SceneNodeIndex49P::none;
    bool canonicalLeftBound_=false;
    std::vector<std::uint8_t> canonicalLeftBodyColorModes50LA_;
    SourceIdentity lowerPsg_[2]; // source bottom-left1C4, bottom-right1C0
    g::SceneNodeIndex49P lowerModelIndex_[2]{g::SceneNodeIndex49P::none,g::SceneNodeIndex49P::none};
    bool canonicalLowerBound_[2]{};
    std::string rightPsgMember_,rightPsgSha256_;
    g::SceneNodeIndex49P rightModelIndex_=g::SceneNodeIndex49P::none;
    bool canonicalRightBound_=false;
    std::vector<std::uint8_t> canonicalBodyColorModes50LA_;
    std::array<std::vector<std::uint8_t>,2> canonicalLowerBodyColorModes50LA_;
    a::MaterialTextureAssets50EA canonicalEngineSpecularMaterials_;
    std::vector<rr::platform::MaterialTextureSampler50EB> canonicalEngineSpecularSamplers_;
    mutable bool materialsPublished_=false;
    std::map<std::string,visual_detail::Json> materialRecords_;
    std::map<std::string,a::Material48E> originalMaterials_;
    std::array<std::vector<const a::Material48E*>,5> componentMaterials_;
    std::array<std::vector<std::uint32_t>,5> flameMaterials_;
    std::array<std::vector<rr::platform::VehicleWindowMaterialBinding50DS>,5> windowBindings_;
    std::vector<rr::platform::MaterialTextureSampler50EB> samplers_;
};
} // namespace native_racer50HW
