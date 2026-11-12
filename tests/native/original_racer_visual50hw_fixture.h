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
           const std::filesystem::path& visualSelectionRoot,const Rig& rig);
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
        const a::OriginalStandardMaterialDomain50LA& domain);
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
        bool useOriginalBodyColors50LA=false);
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
        visual_detail::shared::need(componentCount==5,
            "Canonical bottom-left engine requires the pod's authored bottom-engine pair");
        bindCanonicalEngineBaseMap(model,geometry,references,domain,
            g::VehicleDrawComponent50CM::bottomLeftEngine,lowerModelIndex_[0],
            lowerPsg_[0].member,lowerPsg_[0].sha256,canonicalLowerBound_[0]);
    }
    bool canonicalBottomLeftBound()const noexcept{return canonicalLowerBound_[0];}
    void bindCanonicalBottomRight(const g::OriginalRacerModelSceneOwner50LA& model,
        const a::OriginalStandardModelGeometry50LA& geometry,
        const a::OriginalStandardMaterialReferences50LA& references,
        const a::OriginalStandardMaterialDomain50LA& domain) {
        visual_detail::shared::need(componentCount==5,
            "Canonical bottom-right engine requires the pod's authored bottom-engine pair");
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
        bool useOriginalBodyColors50LA=false);
    bool canonicalRightBound()const noexcept{return canonicalRightBound_;}
    // Per-frame source-color input points only at this immutable canonical
    // material selection and the actual completed selected-light publication.
    // The numeric draw producer copies publication identity into its batch;
    // the renderer checks that same identity before using generated RGB.
    bool canonicalRightSourceColors50LA(const g::NativeGcSelectedLightsCandidate& lights,
        std::uint32_t actualRacerSlot,g::VehicleDrawBatchSourceColors50LA& output)const noexcept;
    // One completed light publication and the actual component rows. Each
    // requests its own model bounds/root/normals for single-map fog or map2.
    // The optional lower pair is complete or absent; failure preserves output.
    bool hasCanonicalBodyColors50LA()const noexcept {return canonicalRightBound_&&!canonicalBodyColorModes50LA_.empty();}
    // A bound body material drawn through the source single-diffuse map program
    // (its fog comes from the gameplay fog preparation of the current camera).
    bool hasCanonicalSingleDiffuse50LA()const noexcept {
        if(!hasCanonicalBodyColors50LA())return false;
        for(const auto& component:windowBindings_)for(const auto& binding:component)
            if(binding.sourceSingleDiffuseRgba50LA)return true;
        return false;
    }
    bool canonicalEngineSourceColors50LA(const g::NativeGcSelectedLightsCandidate& lights,
        std::uint32_t actualRacerSlot,g::VehicleDrawBatchSourceColors50LA& output)const noexcept;
    rr::platform::VehicleWindowMaterials50DS windowMaterials(const rr::platform::MaterialTextureGl50EB* gpu,
        const rr::platform::MaterialTextureGl50EB* specularGpu=nullptr)const;
private:
    // No retained selection catalog or cached geometry is involved. Each native
    // owner is built directly from this Rig's authenticated original archive.
    // The existing canonical closure below still proves the actual model,
    // geometry, material references, textures and source-node identities before
    // any render publication. This is cold conversion only; no guest execution.
    void loadOriginalDisc(const Rig& rig);
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
