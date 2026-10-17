#pragma once
#include <algorithm>
#include "ra_static_world50la.h"
#include "ra_source_camera_ownership50la.h"
#include "ra_static_diffuse_stream50la.h"

namespace ra_static_lighting50LA {
namespace g=rr::game;
namespace light=g::original_gc_gameplay_lighting_candidate;
namespace numeric=g::original_racer_lighting50KM;
// Value projection only, NOT a renderer/admission receipt. The optional CPU
// stream below uses an explicit native policy without model overrides.
struct Snapshot {
    std::size_t surface=0,placement=0,worldInstance=0;
    std::uint32_t authoredZone=0;
    std::uint64_t cameraGeneration=0,cameraPublication=0,cameraFrame=0;
    bool authoredUseLighting=false;
    // Original SCB Position, retained separately from geometry root bypass.
    std::array<double,3> authoredSelectionAnchor{};
    std::array<float,3> nativeSelectionAnchor{};
    g::BodyMatrix48G visual{};
    numeric::LightMatrices lights{};
    numeric::DiffuseProgramInputs normalInputs{};
};
// Private aggregate adapter. SourceCameraPublicationLease pins the real bridge
// lifetime; it does not lock simulation, attach a camera, or invent a draw.
// Every read revalidates the actual session stage/serial and live idle Bank.
// A later publication, terminal camera state or retirement makes it stale.
class Binding final {
    friend class ra_cold_aggregate50LA::Owner;
    struct SourceCameraPublicationLease {
        std::shared_ptr<ra_source_camera50LA::OwnershipBridge> owner;
        ra_source_camera50LA::OwnershipBridge::StaticLightPublication identity;
        bool current()const noexcept {
            ra_source_camera50LA::OwnershipBridge::StaticLightPublication now;
            return owner&&owner->readStaticLightPublication(now)&&
                now.session==identity.session&&now.generation==identity.generation&&
                now.publication==identity.publication&&now.frame==identity.frame;
        }
    } camera_;
    std::shared_ptr<ra_static_world50LA::Owner> world_;
    std::shared_ptr<light::Owner> light_;
    std::vector<Snapshot> snapshots_;
    struct RecoveredNormalSurface {
        const rr::assets::Scene48D* scene=nullptr;
        const rr::assets::SceneSurface48D* surface=nullptr;
        std::size_t first=0,count=0;
    };
    // Same-publication results from prepare's existing source validation, not
    // dynamic lighting or a cache surviving replacement of this Binding.
    std::vector<RecoveredNormalSurface> recoveredNormalSurfaces_;
    std::vector<numeric::CompressedNormal> recoveredNormals_;
    std::unique_ptr<StaticDiffuseStream> diffuseStream_;
    Binding()=default;
    bool current()const noexcept {
        return camera_.current()&&world_&&world_->drawPlanCurrent()&&
            light_&&light_->raStaticCurrent()&&camera_.owner->camera_&&
            camera_.owner->camera_->lighting==light_;
    }
    bool prepare(std::shared_ptr<ra_static_world50LA::Owner> world,
        std::shared_ptr<light::Owner> lighting,
        std::shared_ptr<ra_source_camera50LA::OwnershipBridge> camera,
        light::Diagnostic& d) {
        world_=std::move(world);light_=std::move(lighting);camera_.owner=std::move(camera);
        if(!camera_.owner||!camera_.owner->readStaticLightPublication(camera_.identity)||!current())return false;
        // The completed immutable draw plan counted these exact lit rows.
        // Reserve exact metadata and lit-vertex demand, not all-world vertices.
        snapshots_.reserve(world_->drawRequirements_.staticLightingSurfaces);
        recoveredNormalSurfaces_.reserve(world_->drawRequirements_.staticLightingSurfaces);
        recoveredNormals_.reserve(world_->drawRequirements_.staticLightingVertices);
        // Native static policy: select at the authored SCB Position, from the
        // authored one-based zone. import_world48f::placement stores Position
        // directly in matrix[12..14]. PSG world-space bypass only changes the
        // visual transform; it must not erase this independent spatial anchor.
        for(const auto& plan:world_->surfaceDrawPlan_){
            const auto& raster=world_->rasterInventory_[plan.raster];
            if(!raster.requiresNativeStaticLighting)continue;
            if(raster.program!=ra_static_world50LA::Owner::StaticProgram::diffuseCompressed||
               plan.placement>=world_->placements_.size()||!world_->placements_[plan.placement])return false;
            const auto& placement=*world_->placements_[plan.placement];
            const auto* scene=placement.payload?placement.payload->scene():nullptr;
            if(!scene||plan.worldInstance!=placement.worldInstance||!placement.zone||
               plan.sourceLod>=scene->lods.size()||plan.sourceSurface>=scene->lods[plan.sourceLod].surfaces.size())return false;
            // This fresh binding is unpublished until every row validates.
            snapshots_.emplace_back();auto& row=snapshots_.back();
            row.surface=plan.surface;row.placement=plan.placement;
            row.worldInstance=placement.worldInstance;row.authoredZone=placement.zone;
            row.authoredUseLighting=placement.sourceUseLighting;
            row.cameraGeneration=camera_.identity.generation;
            row.cameraPublication=camera_.identity.publication;row.cameraFrame=camera_.identity.frame;
            for(unsigned i=0;i<16;++i){
                row.visual.v[i]=static_cast<float>(placement.visualMatrix[i]);
                if(!std::isfinite(row.visual.v[i]))return false;
            }
            for(unsigned axis=0;axis<3;++axis){
                row.authoredSelectionAnchor[axis]=placement.authoredMatrix[12+axis];
                row.nativeSelectionAnchor[axis]=static_cast<float>(row.authoredSelectionAnchor[axis]);
                if(!std::isfinite(row.authoredSelectionAnchor[axis])||
                   !std::isfinite(row.nativeSelectionAnchor[axis]))return false;
            }
            if(!light_->selectRaStatic(placement.zone,row.nativeSelectionAnchor,row.lights,d))return false;
            const double normalQ=scene->quantization[1];
            if(!std::isfinite(normalQ)||static_cast<double>(static_cast<float>(normalQ))!=normalQ||
               light::prepareDiffuseNormalAxes(row.visual,static_cast<float>(normalQ),row.lights,row.normalInputs)!=numeric::Status::complete)return false;
            // Verify the original compressed normals remain recoverable; never
            // replace source normals or force white prelit color streams.
            const auto& surface=scene->lods[plan.sourceLod].surfaces[plan.sourceSurface];
            RecoveredNormalSurface recovered{scene,&surface,recoveredNormals_.size(),surface.vertices.size()};
            for(const auto& vertex:surface.vertices){numeric::CompressedNormal normal;
                if(!light::recoverCompressedNormal(vertex.normal_local,normalQ,normal))return false;
                recoveredNormals_.push_back(normal);
            }
            recoveredNormalSurfaces_.push_back(recovered);
        }
        return snapshots_.size()==world_->drawRequirements_.staticLightingSurfaces&&
            recoveredNormals_.size()==world_->drawRequirements_.staticLightingVertices&&current();
    }
    bool read(std::size_t surface,Snapshot& out)const noexcept {
        if(!current())return false;
        for(const auto& row:snapshots_)if(row.surface==surface){out=row;return true;}
        return false;
    }
    bool diffuseRasterCurrent(std::size_t index)const noexcept {
        if(!current()||index>=world_->rasterInventory_.size())return false;
        const auto& row=world_->rasterInventory_[index];
        if(!row.requiresNativeStaticLighting||row.dynamicCrowdMap||row.imageCount!=1||
           row.program!=ra_static_world50LA::Owner::StaticProgram::diffuseCompressed||
           row.family>=world_->imageFamilies_.size())return false;
        // The immutable full draw inventory admits DiffuseCompressed only for
        // the authenticated black.psm family; do not add a name-only admission.
        const rr::assets::OriginalStandardMaterialObject50LA* material=nullptr;
        rr::assets::OriginalStandardMaterialDiagnostic50LA d;
        rr::assets::OriginalStandardMaterialPassView50LA pass;
        const auto& table=world_->domain_->materials382B38();
        return table->resolve298E60(row.materialRow,material,d)&&material&&
            material->passCount48()&&*material->passCount48()==1&&material->pass4C(0,pass)&&
            pass.program18&&*pass.program18==row.programToken&&
            pass.flags21&&*pass.flags21==row.passFlags&&
            pass.fillRgba2C&&*pass.fillRgba2C==row.fillRgba&&
            pass.group22&&*pass.group22==row.group;
    }
    bool prepareDiffuseStream(StaticDiffuseDiagnostic& d)noexcept {
        d={};diffuseStream_.reset();
        auto reject=[&](StaticDiffuseStatus s){d.status=s;return false;};
        if(!current())return reject(StaticDiffuseStatus::stale);
        try {
            std::unique_ptr<StaticDiffuseStream> next(new StaticDiffuseStream);
            next->surfaces_.reserve(snapshots_.size());
            std::size_t selected=0;
            for(const auto& plan:world_->surfaceDrawPlan_){
                d.surface=plan.surface;d.vertex=0;
                if(plan.raster>=world_->rasterInventory_.size())return reject(StaticDiffuseStatus::material);
                const auto& raster=world_->rasterInventory_[plan.raster];
                if(!raster.requiresNativeStaticLighting){
                    if(raster.program!=ra_static_world50LA::Owner::StaticProgram::prelitCompressed)
                        return reject(StaticDiffuseStatus::material);
                    continue; // Original prelit data stays in its separate source bank.
                }
                if(!diffuseRasterCurrent(plan.raster))return reject(StaticDiffuseStatus::material);
                if(raster.passFlags&4u)return reject(StaticDiffuseStatus::fillUnsupported);
                if(selected>=snapshots_.size()||selected>=recoveredNormalSurfaces_.size())return reject(StaticDiffuseStatus::source);
                const auto& recovered=recoveredNormalSurfaces_[selected];
                const auto& snapshot=snapshots_[selected++];
                if(snapshot.surface!=plan.surface||snapshot.placement!=plan.placement||
                   snapshot.worldInstance!=plan.worldInstance||plan.placement>=world_->placements_.size()||
                   !world_->placements_[plan.placement])return reject(StaticDiffuseStatus::source);
                const auto& placement=*world_->placements_[plan.placement];
                const auto* scene=placement.payload?placement.payload->scene():nullptr;
                if(!scene||plan.sourceLod>=scene->lods.size()||
                   plan.sourceSurface>=scene->lods[plan.sourceLod].surfaces.size())return reject(StaticDiffuseStatus::source);
                const auto& surface=scene->lods[plan.sourceLod].surfaces[plan.sourceSurface];
                const auto* material=placement.materials.handle1C4(surface.material_index);
                if(!material||*material!=raster.materialRow||surface.material_index!=plan.materialSlot||
                   surface.vertices.size()!=plan.vertexCount||surface.indices.size()!=plan.indexCount)
                    return reject(StaticDiffuseStatus::source);
                if(recovered.scene!=scene||recovered.surface!=&surface||recovered.count!=surface.vertices.size()||
                   recovered.first>recoveredNormals_.size()||recovered.count>recoveredNormals_.size()-recovered.first)
                    return reject(StaticDiffuseStatus::source);
                next->surfaces_.emplace_back();auto& output=next->surfaces_.back();
                auto& id=output.identity;id.surface=plan.surface;id.placement=plan.placement;
                id.worldInstance=plan.worldInstance;id.raster=plan.raster;id.materialRow=raster.materialRow;
                id.programToken=raster.programToken;id.vertexStart=plan.vertexStart;id.vertexCount=plan.vertexCount;
                id.indexStart=plan.indexStart;id.indexCount=plan.indexCount;
                id.cameraGeneration=snapshot.cameraGeneration;id.cameraPublication=snapshot.cameraPublication;
                id.cameraFrame=snapshot.cameraFrame;output.vertices.reserve(plan.vertexCount);
                numeric::DiffuseRgbaProgramInputs input;
                input.normalAxes=snapshot.normalInputs.normalAxes;
                // Approved native policy: preserve ALL selected lanes. No
                // model ambient/alpha overrides, fill substitution or constant
                // alpha. Selection already performed the signed-light fold.
                input.colorRows=snapshot.lights.diffuse;
                for(std::size_t vertex=0;vertex<recovered.count;++vertex){
                    const auto& normal=recoveredNormals_[recovered.first+vertex];
                    numeric::DiffuseVertexRgba rgba;
                    // The first successful vertex admits the unchanged surface inputs.
                    // This reserved, callback-free loop cannot change their FP mode.
                    const auto projected=vertex==0?
                        numeric::projectDiffuseCompressedRgba(input,normal,rgba):
                        numeric::material_detail::projectDiffuseCompressedRgbaFiniteInputs(input,normal,rgba);
                    if(projected!=numeric::Status::complete)
                        return reject(StaticDiffuseStatus::arithmetic);
                    output.vertices.push_back(rgba);++d.vertex;
                }
            }
            if(selected!=snapshots_.size()||selected!=recoveredNormalSurfaces_.size()||
               selected!=world_->drawRequirements_.staticLightingSurfaces)
                return reject(StaticDiffuseStatus::source);
            if(!current())return reject(StaticDiffuseStatus::stale);
            diffuseStream_=std::move(next);return true;
        }catch(...){return reject(StaticDiffuseStatus::allocation);}
    }
    // Renderer-only synchronous surface scope via the aggregate. The caller
    // must not mutate or reenter publication/material owners during the visit.
    template<class Visitor>
    bool visitDiffuseSurface(std::size_t surface,Visitor&& visitor)const noexcept {
        if(!current()||!diffuseStream_)return false;
        // The scan is read-only and callback-free. Reuse only the immediately
        // preceding successful raster check, never a result from another visit.
        bool havePrevious=false;std::size_t previousRaster=0;
        for(const auto& row:diffuseStream_->surfaces_){
            const auto raster=row.identity.raster;
            if(havePrevious&&raster==previousRaster)continue;
            if(!diffuseRasterCurrent(raster)||
               (world_->rasterInventory_[raster].passFlags&4u))return false;
            previousRaster=raster;havePrevious=true;
        }
        // prepareDiffuseStream appends a filtered subset of the source plan's
        // strictly increasing surface IDs; the published stream never reorders.
        const auto found=std::lower_bound(diffuseStream_->surfaces_.begin(),diffuseStream_->surfaces_.end(),
            surface,[](const auto& row,std::size_t id){return row.identity.surface<id;});
        if(found==diffuseStream_->surfaces_.end()||found->identity.surface!=surface)return false;
        if(!visitor(found->identity,found->vertices))return false;
        return current();
    }
    bool readDiffuseVertex(std::size_t surface,std::size_t vertex,
        StaticDiffuseSurface& identity,numeric::DiffuseVertexRgba& rgba)const noexcept {
        if(!current()||!diffuseStream_)return false;
        // A changed/unsupported material invalidates the whole publication,
        // including reads targeting a different otherwise-valid surface.
        for(const auto& row:diffuseStream_->surfaces_)
            if(!diffuseRasterCurrent(row.identity.raster)||
               (world_->rasterInventory_[row.identity.raster].passFlags&4u))return false;
        for(const auto& row:diffuseStream_->surfaces_)if(row.identity.surface==surface){
            if(vertex>=row.vertices.size())return false;
            identity=row.identity;rgba=row.vertices[vertex];return true;
        }
        return false;
    }
public:
    ~Binding()=default;
    Binding(const Binding&)=delete;Binding& operator=(const Binding&)=delete;
};
}
