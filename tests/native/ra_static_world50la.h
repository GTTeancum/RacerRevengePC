#pragma once
#include "ra_cold_sources50la.h"
#include "ra_crowd_image50la.h"
#include "ra_static_model_cold50la.h"
#include "ra_tga_source50la.h"
#include "../../src/assets/material_texture_assets50ea.h"
#include "../../src/assets/original_standard_ipum_framing50la.h"
#include "../../src/assets/original_standard_material_animation50la.h"
#include "../../src/assets/original_standard_material_domain50la.h"
#include "../../src/assets/original_standard_model_resource50la.h"
#include "../../src/assets/native_asset48b.h"
#include "../../tools/assets/original_roster/ra_compressed_scene_admission.h"
#include <array>
#include <cmath>
#include <cstring>
#include <exception>
#include <map>
#include <memory>
#include <set>
#include <utility>

namespace rr::platform {class MaterialTextureGl50EB;class VehicleWindowGl50DS;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_static_lighting50LA {class Binding;}
namespace ra_static_fog50LA {class Binding;}
namespace ra_static_world50LA {
namespace a=rr::assets;
namespace g=rr::game;
// One actual-source native static CPU world. No retail240110 trailer value,
// scene ancestry or draw/gameplay capability. A separate terminal native
// cold stage may populate canonical resources and own a CPU image bank.
class Owner final {
public:
    struct Vertex {
        std::array<float,3> worldPosition{};
        std::array<double,3> sourceNormal{};
        std::array<float,2> sourceUv{};
        // Meaningful ONLY if the enclosing surface hasOriginalColors.
        // Missing colors remain absent, never synthesized as white/prelit RGB.
        std::array<std::uint8_t,4> sourceColor{};
    };
    struct Surface {
        std::size_t placement=0,worldInstance=0;
        std::uint32_t sourceLod=0,sourceSurface=0,materialSlot=0;
        std::int32_t materialRow=-1;
        std::uint32_t vertexStart=0,vertexCount=0,indexStart=0,indexCount=0;
        bool hasOriginalColors=false,worldSpaceRootBypassed=false;
    };
    struct LodRange {
        std::size_t firstSurface=0,surfaceCount=0;
    };
    struct Placement {
        std::size_t worldInstance=0;
        std::uint32_t zone=0,ordinal=0;
        std::shared_ptr<const a::OriginalStandardModelResource50LA> payload;
        a::OriginalStandardMaterialReferences50LA materials;
        std::array<double,16> authoredMatrix{},visualMatrix{};
        std::size_t firstSurface=0,surfaceCount=0;
        // Indexed by the authentic source LOD; empty source LODs stay empty.
        // Surface and draw-plan indices share this contiguous range.
        std::vector<LodRange> lodRanges;
        bool sourceUseLighting=false;
    };
    struct CollisionOnly {
        std::size_t worldInstance=0;
        std::uint32_t zone=0,ordinal=0;
        std::string absentGeometry,presentCollision;
        bool sourceUseLighting=false;
    };
    struct FireSubset {
        std::size_t placement=0,worldInstance=0;
        std::vector<std::uint32_t> surfaces;
    };
    struct Diagnostic {
        const char* failure=nullptr;
        std::size_t payloads=0,widePayloads=0,placements=0,retainedPrefixes=0,collisionOnly=0,surface=0;
        a::DecodeError48C decode{};
        a::OriginalStandardResourceDiagnostic50LA resource{};
        a::OriginalStandardMaterialDiagnostic50LA material{};
        a::OriginalUiPageDiagnostic50KG text{};
        ra_crowd_image50LA::Verifier::Diagnostic crowd{};
        a::MaterialTextureAssetDiagnostic50EA images{};
    };
private:
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class ::ra_static_lighting50LA::Binding;
    friend class ::ra_static_fog50LA::Binding;
    friend class rr::platform::MaterialTextureGl50EB;
    friend class rr::platform::VehicleWindowGl50DS;
    // Private exact-publication leases. Publishing owners retain this actual
    // world strongly; source/CPU mip storage cannot retire beneath their views.
    const rr::platform::MaterialTextureGl50EB* imagePublication_=nullptr;
    const rr::platform::VehicleWindowGl50DS* rendererPublication_=nullptr;
    bool publishImage(const rr::platform::MaterialTextureGl50EB* image)noexcept {
        if(!image||imagePublication_||rendererPublication_||modelServiceTerminal_||!coldImagesCurrent())return false;
        imagePublication_=image;return true;
    }
    void retireImage(const rr::platform::MaterialTextureGl50EB* image)noexcept {
        if(imagePublication_!=image||rendererPublication_)std::terminate();imagePublication_=nullptr;
    }
    bool publishRenderer(const rr::platform::VehicleWindowGl50DS* renderer,
        const rr::platform::MaterialTextureGl50EB* image) noexcept {
        if(!renderer || !image || imagePublication_!=image || rendererPublication_ || !drawPlanCurrent())return false;
        rendererPublication_=renderer;return true;
    }
    void retireRenderer(const rr::platform::VehicleWindowGl50DS* renderer) noexcept {
        if(!renderer || rendererPublication_!=renderer)std::terminate();
        rendererPublication_=nullptr;
    }
    Owner()=default;
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const a::OriginalStandardMaterialContext50LA> context_;
    std::map<std::string,std::shared_ptr<const a::OriginalStandardModelResource50LA>> payloads_;
    std::map<std::string,std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> psms_;
    std::vector<std::unique_ptr<Placement>> placements_;
    std::unique_ptr<ra_static_model_cold50LA::Owner> modelPrefixes_;
    std::vector<CollisionOnly> collisionOnly_;
    std::vector<Vertex> vertices_;
    std::vector<std::uint32_t> indices_;
    std::vector<Surface> surfaces_;
    std::array<FireSubset,4> fires_{};
    std::array<bool,4> fireFound_{};
    std::uint64_t providerRevision_=0;
    bool attempted_=false,staged_=false,closed_=false;
    static std::string normalized(std::string s){return ra_compressed_scene::detail::normalized(std::move(s));}
    static bool reject(Diagnostic& d,const char* why)noexcept{d.failure=why;return false;}
    const a::ResourceEntry48C* sourceEntry(const std::string& name)const {
        const auto key=normalized(name);const auto* data=source_->sourcesDecoded();
        const a::ResourceEntry48C* found=nullptr;
        for(const auto& e:data->archive.entries)if(normalized(e.name)==key){if(found)return nullptr;found=&e;}
        return found;
    }
    bool member(const a::ResourceEntry48C& row,std::vector<std::uint8_t>& bytes,Diagnostic& d)const {
        const auto* data=source_->sourcesDecoded();const auto& provider=domain_->archiveProvider3E1A70();
        if(!data||!provider||!provider->sealed()||provider->revision()!=providerRevision_||
           !a::extractResMember48C(data->archive,row.name,bytes,d.decode)||bytes.size()!=row.bytes)return false;
        a::OriginalStandardArchiveProvider50LA::MemberView selected;
        a::OriginalStandardArchiveProvider50LA::MountView mount;
        return provider->lookup132820(row.name,selected)==a::OriginalStandardArchiveProvider50LA::Lookup::present&&
            provider->mount(selected.mount,mount)&&mount.archiveSha256&&*mount.archiveSha256==ra_cold_sources50LA::archiveSha&&
            mount.archiveBytes==4604561&&mount.memberCount==1010&&selected.data&&selected.member&&
            normalized(*selected.member)==normalized(row.name)&&selected.size==bytes.size()&&
            !std::memcmp(selected.data,bytes.data(),bytes.size());
    }
    bool retainPsmSources(const a::Scene48D& scene,Diagnostic& d){
        for(const auto& name:scene.materials){
            if(name.empty()||name.find_first_of("\\/.:\"")!=std::string::npos)return reject(d,"invalid actual material name");
            const auto memberName="textures\\"+name+".psm";const auto key=normalized(memberName);
            if(psms_.find(key)!=psms_.end())continue;
            const auto* row=sourceEntry(memberName);std::vector<std::uint8_t> bytes;
            if(!row||!member(*row,bytes,d))return reject(d,"actual same-provider PSM missing/different");
            auto psm=std::make_shared<a::OriginalStandardMaterialSource50LA>();
            if(!psm->prepare(row->name,bytes,d.text))return reject(d,"actual static PSM syntax rejected");
            psms_.emplace(key,std::move(psm));
        }
        return true;
    }
    bool lowerPlacement(Placement& out,std::size_t placement,Diagnostic& d){
        const auto& scene=*out.payload->scene();
        if(scene.nodes.size()!=1||scene.nodes[0].parent!=-1||scene.identity_matrix_palette||
           (scene.lods.size()!=1&&scene.lods.size()!=4))return reject(d,"unsupported original native static hierarchy/LOD");
        // Original single-node local roots are replaced by SCB placement;
        // world-space geometry bypasses that root at240B04/241678. Semantic
        // position_local already includes quantization scale AND origin.
        out.visualMatrix=scene.world_space?std::array<double,16>{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}}:out.authoredMatrix;
        for(double v:out.authoredMatrix)if(!std::isfinite(v))return reject(d,"nonfinite actual SCB matrix");
        if(out.authoredMatrix[3]!=0||out.authoredMatrix[7]!=0||out.authoredMatrix[11]!=0||out.authoredMatrix[15]!=1)
            return reject(d,"nonaffine actual SCB matrix");
        out.firstSurface=surfaces_.size();
        out.lodRanges.reserve(scene.lods.size());
        for(std::size_t li=0;li<scene.lods.size();++li){
            LodRange range;range.firstSurface=surfaces_.size();
            for(std::size_t si=0;si<scene.lods[li].surfaces.size();++si){
                d.surface=si;const auto& surface=scene.lods[li].surfaces[si];
                const auto* token=out.materials.handle1C4(surface.material_index);
                if(!token||surface.material_index>=scene.materials.size()||surface.vertices.empty()||surface.indices.empty()||
                   surface.indices.size()%3||surface.vertices.size()>a::max_mesh_vertices-vertices_.size()||
                   surface.indices.size()>a::max_mesh_indices-indices_.size()||surfaces_.size()>=a::max_mesh_indices/3)
                    return reject(d,"source surface/material or native geometry capacity rejected");
                Surface span;span.placement=placement;span.worldInstance=out.worldInstance;
                span.sourceLod=static_cast<std::uint32_t>(li);
                span.sourceSurface=static_cast<std::uint32_t>(si);span.materialSlot=surface.material_index;span.materialRow=*token;
                span.vertexStart=static_cast<std::uint32_t>(vertices_.size());span.vertexCount=static_cast<std::uint32_t>(surface.vertices.size());
                span.indexStart=static_cast<std::uint32_t>(indices_.size());span.indexCount=static_cast<std::uint32_t>(surface.indices.size());
                span.hasOriginalColors=surface.has_original_colors;span.worldSpaceRootBypassed=scene.world_space;
                for(const auto& v:surface.vertices){
                    if(v.bone!=0)return reject(d,"actual static vertex has unjoined bone");
                    Vertex vertex;
                    for(unsigned axis=0;axis<3;++axis){
                        double value=0;for(unsigned k=0;k<3;++k)value+=v.position_local[k]*out.visualMatrix[k*4+axis];
                        value+=out.visualMatrix[12+axis];vertex.worldPosition[axis]=static_cast<float>(value);
                        if(!std::isfinite(value)||!std::isfinite(vertex.worldPosition[axis])||!std::isfinite(v.normal_local[axis]))
                            return reject(d,"nonfinite source vertex/normal");
                        vertex.sourceNormal[axis]=v.normal_local[axis];
                    }
                    for(unsigned j=0;j<2;++j){vertex.sourceUv[j]=static_cast<float>(v.uv[j]);
                        if(!std::isfinite(v.uv[j])||!std::isfinite(vertex.sourceUv[j]))return reject(d,"nonfinite original UV");}
                    if(surface.has_original_colors)vertex.sourceColor=v.color;
                    vertices_.push_back(vertex);
                }
                for(auto index:surface.indices){if(index>=surface.vertices.size())return reject(d,"actual triangle index outside surface");
                    indices_.push_back(span.vertexStart+index);}
                surfaces_.push_back(span);++out.surfaceCount;++range.surfaceCount;
            }
            out.lodRanges.push_back(range);
        }
        return true;
    }
    bool prepare(std::shared_ptr<const ra_cold_sources50LA::Owner> source,
        std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const a::OriginalStandardMaterialContext50LA> context,
        const a::OriginalStandardMaterialPath50LA& path,Diagnostic& d)noexcept {
        d={};if(attempted_||closed_)return reject(d,"RA world stage already attempted");attempted_=true;
        try{
            const auto* data=source?source->sourcesDecoded():nullptr;
            if(!data||data->world.instances.size()!=1261||!domain||!domain->prepared()||!context||!context->live()||
               context->materialApplication()!=domain->application_||!path.actualPrefix36B970||*path.actualPrefix36B970!="textures\\"||
               !domain->archiveProvider3E1A70()||!domain->archiveProvider3E1A70()->sealed()||!domain->resolver3E1A70())
                return reject(d,"actual RA source/same Application/context/provider/path required");
            source_=std::move(source);domain_=std::move(domain);context_=std::move(context);
            providerRevision_=domain_->archiveProvider3E1A70()->revision();
            if(!domain_->resolver3E1A70()->bindProvider3E1A70(domain_->archiveProvider3E1A70(),d.material.texture))
                return reject(d,"same actual resolver/provider binding rejected");
            // Authenticate the fixed compressed-material closure in the SAME
            // mounted provider before any private widened geometry admission.
            for(const auto& pin:ra_compressed_scene::detail::compressedMaterials){
                const auto* row=sourceEntry(pin.name);std::vector<std::uint8_t> bytes;
                if(!row||!member(*row,bytes,d))return reject(d,"pinned compressed PSM provider identity differs");
                ra_compressed_scene::detail::requireIdentity(bytes,pin);
            }
            // Every present PSG, not merely selected visible models, is owned.
            for(const auto& row:data->archive.entries){const auto key=normalized(row.name);
                if(key.size()<4||key.substr(key.size()-4)!=".psg")continue;
                if(payloads_.size()>=710)return reject(d,"unexpected additional RA PSG");
                std::vector<std::uint8_t> bytes;if(!member(row,bytes,d))return reject(d,"same-provider original PSG differs");
                a::SceneLimits48D limits;
                if(const auto* pin=ra_compressed_scene::detail::findIdentity(ra_compressed_scene::detail::wideScenes,key)){
                    ra_compressed_scene::detail::requireIdentity(bytes,*pin);
                    ra_compressed_scene::detail::requireCompressedMaterials(data->archive,bytes);
                    limits.allow_signed32_quantized_positions=true;++d.widePayloads;
                }
                auto payload=std::make_shared<a::OriginalStandardModelResource50LA>();
                if(!payload->prepareOriginalLimits(row.name,bytes,limits,d.resource))return reject(d,"actual RA model decoder rejected");
                if(!payload->scene()||payload->nodeCount()!=1)return reject(d,"actual source hierarchy differs");
                if(!retainPsmSources(*payload->scene(),d))return false;
                if(!payloads_.emplace(key,std::move(payload)).second)return reject(d,"duplicate RA PSG identity");
                d.payloads=payloads_.size();
            }
            if(payloads_.size()!=710||d.widePayloads!=583||psms_.size()!=44)return reject(d,"complete RA source closure counts differ");
            std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials;
            for(const auto& psm:psms_)materials.push_back(psm.second);
            const std::vector<a::OriginalStandardTextureSourceInput50LA> noImages;
            if(!domain_->appendOriginalSources(materials,noImages,d.material))return reject(d,"actual PSM corpus append rejected");
            placements_.reserve(1085);collisionOnly_.reserve(176);std::set<std::string> usedPayloads;
            for(std::size_t wi=0;wi<data->world.instances.size();++wi){const auto& instance=data->world.instances[wi];
                const auto found=payloads_.find(normalized(instance.geometry_member));
                if(found==payloads_.end()){
                    if(sourceEntry(instance.geometry_member)||!sourceEntry(instance.collision_member)||collisionOnly_.size()>=176)
                        return reject(d,"missing visual is not an authenticated collision-only record");
                    a::OriginalStandardArchiveProvider50LA::MemberView other;
                    if(domain_->archiveProvider3E1A70()->lookup132820(instance.geometry_member,other)!=
                       a::OriginalStandardArchiveProvider50LA::Lookup::absent)return reject(d,"provider resolves supposedly absent PSG");
                    collisionOnly_.push_back({wi,instance.zone,instance.ordinal,instance.geometry_member,instance.collision_member,instance.use_lighting});
                    d.collisionOnly=collisionOnly_.size();continue;
                }
                if(placements_.size()>=1085)return reject(d,"unexpected additional RA visual placement");
                auto placement=std::unique_ptr<Placement>(new Placement);placement->worldInstance=wi;
                placement->zone=instance.zone;placement->ordinal=instance.ordinal;placement->payload=found->second;
                placement->authoredMatrix=instance.matrix;placement->sourceUseLighting=instance.use_lighting;
                placements_.push_back(std::move(placement));auto& out=*placements_.back();
                // Stable owner is installed before actual store-before-retain
                // material acquisition; failed prefixes remain until close.
                if(!out.materials.acquire26E640(*out.payload,domain_->materials382B38(),path,d.material))
                    return reject(d,"actual per-placement DefaultMT references rejected");
                ++d.retainedPrefixes;
                if(!lowerPlacement(out,placements_.size()-1,d))return false;
                usedPayloads.insert(found->first);d.placements=placements_.size();
                if(instance.zone==1&&instance.ordinal>=366&&instance.ordinal<=369){
                    const auto fire=instance.ordinal-366;const auto expected="geometry/firep0t0"+std::to_string(fire+1)+".psg";
                    if(fireFound_[fire]||found->first!=expected||instance.parameter_block>=data->world.parameters.blocks.size())
                        return reject(d,"actual fire subset source identity differs");
                    std::size_t names=0;for(const auto& field:data->world.parameters.blocks[instance.parameter_block].fields)
                        if(field.name=="slt"){if(field.value!="FireP0t0"+std::to_string(fire+1)+".slt")return reject(d,"actual fire SLT differs");++names;}
                    if(names!=1||!out.surfaceCount)return reject(d,"actual fire subset has no unique source/surface");
                    auto& subset=fires_[fire];subset.placement=placements_.size()-1;subset.worldInstance=wi;
                    for(std::size_t si=out.firstSurface;si<out.firstSurface+out.surfaceCount;++si)
                        subset.surfaces.push_back(static_cast<std::uint32_t>(si));
                    fireFound_[fire]=true;
                }
            }
            if(placements_.size()!=1085||collisionOnly_.size()!=176||usedPayloads.size()!=710||
               domain_->archiveProvider3E1A70()->revision()!=providerRevision_)return reject(d,"complete placed world closure differs");
            for(bool found:fireFound_)if(!found)return reject(d,"actual full-world fire subset incomplete");
            staged_=true;return true;
        }catch(...){return reject(d,"RA full-world cold failure; retained prefixes require close");}
    }
#include "ra_static_world_images50la.inl"
#include "ra_static_world_profiles50la.inl"
#include "ra_static_world_model_service50la.inl"
#include "ra_static_world_draw_plan50la.inl"
    bool constructModelPrefixes(std::shared_ptr<rr::game::OriginalApplicationEntities50LA> application,
        std::uint64_t generation,ra_static_model_cold50LA::Owner::Diagnostic& d)noexcept {
        d={};
        if(!staged_||closed_||modelPrefixes_||imagePublication_||rendererPublication_||
           !source_||!domain_||!domain_->prepared()||!context_||!context_->live()||
           !domain_->archiveProvider3E1A70()||
           domain_->archiveProvider3E1A70()->revision()!=providerRevision_){
            d.failure="same retained cold world required before model construction";return false;
        }
        try {
            std::vector<ra_static_model_cold50LA::Owner::Input> inputs;inputs.reserve(placements_.size());
            for(const auto& p:placements_){
                if(!p){d.failure="actual placement absent";return false;}
                inputs.push_back({p->worldInstance,p->payload,&p->materials});
            }
            // Keep partial constructor ownership installed, even on failure.
            modelPrefixes_.reset(new ra_static_model_cold50LA::Owner);
            return modelPrefixes_->prepare(source_,std::move(application),domain_->materials382B38(),generation,inputs,d);
        }catch(...){d.failure="static model backing allocation failed";return false;}
    }
    bool completeModelResourcesAndPlacement(ra_static_model_cold50LA::Owner::Diagnostic& d)noexcept {
        d={};
        if(!staged_||closed_||!modelPrefixes_||imagePublication_||rendererPublication_||
           !domain_||!domain_->prepared()||!context_||!context_->live()||
           !domain_->archiveProvider3E1A70()||domain_->archiveProvider3E1A70()->revision()!=providerRevision_){
            d.failure="same retained static resource world required";return false;
        }
        return modelPrefixes_->completeResourcesAndPlacement(domain_->resolver3E1A70(),d);
    }
    bool registerModelObservers(std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner> visibility,
        ra_static_model_cold50LA::Owner::Diagnostic& d)noexcept {
        if(!staged_||closed_||!modelPrefixes_||imagePublication_||rendererPublication_||
           !domain_||!domain_->prepared()||!context_||!context_->live()||
           !domain_->archiveProvider3E1A70()||domain_->archiveProvider3E1A70()->revision()!=providerRevision_){
            d={};d.failure="private current static world required";return false;}
        return modelPrefixes_->registerObserversAndParents(std::move(visibility),d);
    }
    bool holdsStaticVisibility(const g::original_gc_gameplay_lighting_candidate::Owner* owner)const noexcept {
        return modelPrefixes_&&modelPrefixes_->visibility_.get()==owner;
    }
public:
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner(){if(!close())std::terminate();}
    // Cold source views only. Separate image getters certify a CPU stage,
    // never a serviced animated map, GPU publication or render readiness.
    const std::vector<Vertex>* vertices()const noexcept{return staged_&&!closed_?&vertices_:nullptr;}
    const std::vector<std::uint32_t>* indices()const noexcept{return staged_&&!closed_?&indices_:nullptr;}
    const std::vector<Surface>* surfaces()const noexcept{return staged_&&!closed_?&surfaces_:nullptr;}
    const std::vector<CollisionOnly>* collisionOnly()const noexcept{return staged_&&!closed_?&collisionOnly_:nullptr;}
    const Placement* placement(std::size_t i)const noexcept{return staged_&&!closed_&&i<placements_.size()?placements_[i].get():nullptr;}
    const FireSubset* fireSubset(std::size_t i)const noexcept{return staged_&&!closed_&&i<4?&fires_[i]:nullptr;}
    const ra_static_model_cold50LA::Owner* coldModelPrefixes()const noexcept {
        return staged_&&!closed_?modelPrefixes_.get():nullptr;
    }
    bool canClose()const noexcept {
        if(imagePublication_||rendererPublication_)return false;
        if(modelPrefixes_&&!modelPrefixes_->canClose())return false;
        for(const auto& p:placements_)if(p&&!p->materials.canRelease26E4C0())return false;return true;
    }
    bool close()noexcept {
        if(closed_)return true;if(!canClose())return false;
        if(modelPrefixes_&&!modelPrefixes_->close())return false;
        modelPrefixes_.reset();
        drawPlanComplete_=false;surfaceDrawPlan_.clear();rasterInventory_.clear();
        for(auto& p:placements_)if(p){a::OriginalStandardMaterialDiagnostic50LA d;
            if(!p->materials.release26E4C0(d))return false;p.reset();}
        placements_.clear();
        // Canonical references release children before the retained six-set;
        // private image/renderer leases have already retired above.
        coldImages_.clear();imageFamilies_.clear();crowdImages_.reset();crowdCompressed_.reset();imagesComplete_=false;
        payloads_.clear();psms_.clear();context_.reset();domain_.reset();source_.reset();
        staged_=false;closed_=true;return true;
    }
};
}
