#pragma once
#include "vehicle_draw50cm.h"
#include "original_racer_diffuse_camera50la.h"
#include "original_racer_model_packets50la.h"
#include "diffuse_compressed_fog_numeric50la.h"
#include <vector>
#include <algorithm>
namespace rr::game {
namespace original_racer_diffuse50LA {
namespace fog=::diffuse_compressed_fog_numeric50LA;
struct Surface {
    std::uint32_t ordinal=0,vertexStart=0;
    std::vector<fog::RawVertex> vertices;
    std::vector<fog::VifGroup> groups;
    std::vector<fog::StripSpan> strips;
};
struct Geometry {
    const assets::Scene48D* scene=nullptr;
    const std::uint32_t* indices=nullptr;
    std::uint32_t vertexCount=0;
    std::vector<Surface> surfaces;
};
struct Workspace {
    std::array<fog::Matrix,fog::maxPalette> matrices{};
    std::array<fog::Preamble,fog::maxPalette> preambles{};
    std::array<std::uint8_t,fog::maxVertices> bytes{};
    std::array<float,fog::maxVertices> pending{};
};
// Cold-only reconstruction from exact retained semantic data. Decode division
// is accepted only if the integer exactly re-expands to the source value.
inline bool prepareCold(const VehicleDrawMesh50CM& mesh,const std::uint8_t* singleModes,
    std::size_t materialCount,Geometry& output)noexcept {
    if(!mesh.semanticScene||!singleModes||!materialCount||!mesh.spans||!mesh.indices||
       !mesh.vertexCount||mesh.vertexCount>fog::maxVertices)return false;
    const auto& scene=*mesh.semanticScene;
    if(scene.world_space||!scene.identity_matrix_palette||scene.nodes.size()<2||
       scene.nodes.size()>fog::maxPalette||scene.lods.empty()||
       scene.lods[0].surfaces.size()!=mesh.spanCount||scene.materials.size()!=materialCount)return false;
    const double q=scene.quantization[0];
    if(!std::isfinite(q)||q<=0||double(float(q))!=q||scene.quantization[4]!=0||
       scene.quantization[5]!=0||scene.quantization[6]!=0)return false;
    Geometry next;next.scene=&scene;next.indices=mesh.indices;next.vertexCount=mesh.vertexCount;
    try {
        std::size_t vertexOffset=0,indexOffset=0;
        for(std::uint32_t ordinal=0;ordinal<mesh.spanCount;++ordinal){
            const auto& span=mesh.spans[ordinal];const auto& source=scene.lods[0].surfaces[ordinal];
            if(span.material_index>=materialCount||source.material_index!=span.material_index||
               span.vertex_start!=vertexOffset||span.index_start!=indexOffset||
               span.vertex_count!=source.vertices.size()||span.index_count!=source.indices.size()||
               vertexOffset>mesh.vertexCount||source.vertices.size()>mesh.vertexCount-vertexOffset||
               indexOffset>mesh.indexCount||source.indices.size()>mesh.indexCount-indexOffset)return false;
            for(std::size_t i=0;i<source.indices.size();++i)
                if(source.indices[i]>=source.vertices.size()||mesh.indices[indexOffset+i]!=vertexOffset+source.indices[i])return false;
            vertexOffset+=source.vertices.size();indexOffset+=source.indices.size();
            if(!singleModes[span.material_index])continue;
            if(source.material_index!=span.material_index||source.vertices.size()!=span.vertex_count||
               span.vertex_start>mesh.vertexCount||span.vertex_count>mesh.vertexCount-span.vertex_start||
               !source.groups||source.strips.empty())return false;
            Surface row;row.ordinal=ordinal;row.vertexStart=span.vertex_start;
            row.vertices.resize(source.vertices.size());
            for(std::size_t v=0;v<source.vertices.size();++v){
                const auto& vertex=source.vertices[v];if(vertex.bone>=scene.nodes.size())return false;
                for(unsigned c=0;c<3;++c){
                    const double value=vertex.position_local[c]/q;
                    if(!std::isfinite(value)||std::trunc(value)!=value||value< -8388608||value>8388607||
                       value*q!=vertex.position_local[c])return false;
                    row.vertices[v].xyz[c]=static_cast<std::int32_t>(value);
                }
                row.vertices[v].signedW=16*(static_cast<std::int32_t>(vertex.bone)+1);
            }
            std::size_t strip=0,base=0;
            for(std::uint32_t group=0;group<source.groups;++group){
                std::size_t end=0;
                while(strip<source.strips.size()&&source.strips[strip].group==group){
                    const auto& sourceStrip=source.strips[strip++];
                    if(sourceStrip.start!=end||sourceStrip.end<sourceStrip.start+3||sourceStrip.end>16||
                       sourceStrip.vertex_start!=base+sourceStrip.start||base+sourceStrip.end>row.vertices.size())return false;
                    row.strips.push_back({group,sourceStrip.start,sourceStrip.end,sourceStrip.vertex_start,false});
                    row.vertices[base+sourceStrip.start].signedW*=-1;
                    row.vertices[base+sourceStrip.start+1].signedW*=-1;
                    end=sourceStrip.end;
                }
                if(end<3)return false;row.groups.push_back({base,end});base+=end;
            }
            if(strip!=source.strips.size()||base!=row.vertices.size())return false;
            next.surfaces.push_back(std::move(row));
        }
        if(vertexOffset!=mesh.vertexCount||indexOffset!=mesh.indexCount)return false;
    }catch(...){return false;}
    if(next.surfaces.empty())return false;output=std::move(next);return true;
}
inline bool prepareFrame(const Geometry& geometry,const VehicleDrawMesh50CM& mesh,
    const VehicleDrawSubmission50CM& body,const OriginalRacerModelPackets50LA& owner,
    const OriginalRacerModelDistanceFlags50LA& view,const OriginalRacerDiffuseClip50LA& clip,
    const OriginalRacerDiffuseCameraSnapshot50LA& camera,Workspace& workspace,
    float* output,std::size_t capacity)noexcept {
    if(!output||capacity<geometry.vertexCount||geometry.scene!=mesh.semanticScene||
       geometry.indices!=mesh.indices||geometry.vertexCount!=mesh.vertexCount||
       body.sourceGeometryModel50LA!=&owner||!body.sourceGeometryRevision50LA||!body.sourceSpecularGeometry50LA)return false;
    const auto* packet=owner.packet(0);
    if(!packet||packet->extraMatrixCount!=2||packet->sourcePaletteRows.size()!=geometry.scene->nodes.size()||
       packet->matrixSlots.size()!=3+packet->sourcePaletteRows.size())return false;
    for(std::size_t i=0;i<packet->sourcePaletteRows.size();++i){
        if(packet->sourcePaletteRows[i]!=i)return false;
        std::memcpy(workspace.matrices[i].data(),packet->matrixSlots[3+i].v,sizeof(BodyMatrix48G));
    }
    std::fill_n(workspace.pending.data(),geometry.vertexCount,255.f);
    volatile float denominator=camera.fog.start34-camera.fog.end38;
    if(!std::isfinite(denominator)||denominator==0)return false;
    volatile float slope=255.f/denominator;
    if(!std::isfinite(slope))return false;
    for(const auto& surface:geometry.surfaces){
        fog::Packet in;in.palette=workspace.matrices.data();in.paletteCount=packet->sourcePaletteRows.size();
        in.vertices=surface.vertices.data();in.vertexCount=surface.vertices.size();
        in.groups=surface.groups.data();in.groupCount=surface.groups.size();
        in.strips=surface.strips.data();in.stripCount=surface.strips.size();
        in.camera180={{camera.camera.position.x,camera.camera.position.y,camera.camera.position.z}};
        in.positionScale=packet->scale40[0];in.fogEnd33Z=camera.fog.end38;in.fogSlope33W=slope;
        in.sourceFlags41X=static_cast<std::uint16_t>((view.flags20&2u)|(clip.clip04?4u:0u)|owner.compressionFlag40());
        fog::Workspace scratch{workspace.preambles.data(),workspace.preambles.size(),workspace.bytes.data(),workspace.bytes.size()};
        if(fog::evaluate(in,scratch,fog::FloatOutput{workspace.pending.data()+surface.vertexStart,
            workspace.pending.size()-surface.vertexStart})!=fog::Status::complete)return false;
    }
    std::copy_n(workspace.pending.data(),geometry.vertexCount,output);return true;
}
}
}
