#pragma once
// Common rewrite (area E, W0 round 2 R8/R5 notes): the ONE environment-map UV
// producer of the race's track, rr::platform::VehicleWindowEnvironmentUv50DS, for
// every course. Replaces the RC-only rc_crystal50RC owner (its 4 crystal PSG pins,
// 79 placements, 20596 UVs, 44 selections and the RC ambient value pin are gone).
//
// Original: pass program EnvironmentMapCompressed (33AB78..33AD58) projects each
// vertex of an environment-mapped surface from its RAW quantized PSG position and
// normal (rawPosition*qPosition - camera in the model's source frame; normal
// rawNormal*qNormal), with the VU row27 ambient as vertex colour (33ABB8/33AC08).
// Which surfaces: exactly the static-world spans whose material pass the one
// material rule (material_alpha50gx_fixture::selectMaSa via
// single_pass_world50LM::Owner::alpha) marks sourceEnvironmentMap - data, never a
// course or material name. A track with no such pass has no producer (make()
// returns null, nothing is drawn by the environment route).
//
// Storage: one range per environment-mapped surface placement (the base and
// environment passes share the mesh vertices), sized once from the counted spans;
// raw vertices copied once from each distinct PSG member (bounded by the decoder's
// own vertex limit); every write bounds-checked. Host memory only.
#include "common_track50cr.h"
#include "single_pass_world50lm.h"
#include "../../src/assets/import_scene48d.h"
#include "../../src/game/original_track_environment50rc.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "../../src/game/camera_view49f.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace track_environment_uv50CR {
namespace a=rr::assets;namespace g=rr::game;namespace p=rr::platform;
namespace math=g::original_track_environment50RC;

class Owner final : public p::VehicleWindowEnvironmentUv50DS {
    struct Source {                       // one distinct environment-mapped PSG surface
        std::string member;std::uint32_t surface=0;
        std::vector<math::Vertex> vertices;
        float qPosition=0,qNormal=0;
        bool worldSpace=false;            // the PSG's own world_space flag (every placement of it)
    };
    struct Placement {                    // one placed environment-mapped surface
        std::size_t source=0;std::array<float,16> matrix{};
    };
    std::shared_ptr<const single_pass_world50LM::Owner> world_;
    std::shared_ptr<const g::original_gc_gameplay_lighting_candidate::Owner> lights_;
    std::vector<Source> sources_;
    std::vector<Placement> placements_;   // parallel to ranges_
    std::vector<Range> ranges_;
    std::vector<std::uint8_t> environmentSpan_;  // per world span: 1 = environment-mapped pass
    std::size_t uvCount_=0;
    std::thread::id thread_=std::this_thread::get_id();
    bool attempted_=false,ready_=false;
    static void need(bool v,const std::string& why){if(!v)throw std::runtime_error(why);}
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    // 05.08 results, after 05.13 (the ambient comes from the lighting owner): reads
    // the Track's static world, lights and archive. Returns false (error) on a
    // malformed environment source; `authored` false = the track has no
    // environment-mapped pass (no producer is needed).
    bool prepare(const common_track50CR::Track& track,const a::OriginalUiMaterialTemplates50KG& templates,
        const a::OriginalUiMaterialRuntime50KG& runtime,bool& authored,std::string& error)noexcept{
        authored=false;
        if(attempted_||thread_!=std::this_thread::get_id()){error="environment UV producer is one attempt";return false;}
        attempted_=true;
        try{
            const auto& world=track.roles.staticWorld;
            need(world&&world->loaded()&&world->mesh()&&world->spans()&&world->selections(),
                "environment UV: static world (05.07/05.08) not loaded");
            need(bool(track.roles.lights),"environment UV: lighting owner (05.13) absent");
            const auto& spans=*world->spans();const auto& selections=*world->selections();
            // The one material rule per selection: which passes are environment-mapped.
            std::vector<std::uint8_t> environment(selections.size(),0);
            for(std::size_t i=0;i<selections.size();++i){material_alpha50gx_fixture::Policy policy;
                need(world->alpha(i,templates,runtime,policy),"environment UV: material pass outside the one material rule");
                environment[i]=policy.sourceEnvironmentMap?1u:0u;}
            environmentSpan_.assign(spans.size(),0);
            // One range per distinct environment-mapped vertex range, ascending.
            std::vector<std::size_t> picked;
            for(std::size_t i=0;i<spans.size();++i){const auto& s=spans[i];
                need(s.materialSelection<selections.size(),"environment UV: span material index");
                if(!environment[s.materialSelection])continue;
                environmentSpan_[i]=1;
                bool same=false;for(auto k:picked)if(spans[k].vertexStart==s.vertexStart){
                    need(spans[k].vertexCount==s.vertexCount,"environment UV: passes of one surface disagree on vertices");same=true;}
                if(!same)picked.push_back(i);
            }
            if(picked.empty()){error.clear();return true;}   // nothing authored
            std::sort(picked.begin(),picked.end(),[&](std::size_t x,std::size_t y){return spans[x].vertexStart<spans[y].vertexStart;});
            ranges_.reserve(picked.size());placements_.reserve(picked.size());
            const auto& instances=track.documents.world.instances;
            for(std::size_t n=0;n<picked.size();++n){const auto& s=spans[picked[n]];
                need(!n||s.vertexStart>=ranges_.back().vertexStart+ranges_.back().vertexCount,"environment UV: overlapping ranges");
                need(s.worldInstanceIndex<instances.size(),"environment UV: span world instance");
                const auto& instance=instances[s.worldInstanceIndex];
                // One distinct raw source per (PSG member, surface).
                std::size_t k=0;while(k<sources_.size()&&!(sources_[k].member==instance.geometry_member&&sources_[k].surface==s.surfaceIndex))++k;
                if(k==sources_.size()){
                    Source src;src.member=instance.geometry_member;src.surface=s.surfaceIndex;
                    common_track50CR::Bytes bytes;common_track50CR::Diagnostic md;
                    need(track.archive.member(src.member,bytes,8u*1024u*1024u,md),"environment UV: PSG member "+src.member+" "+md.reason);
                    a::SceneLimits48D limits;limits.allow_signed32_quantized_positions=true;
                    a::Scene48D probe;a::DecodeError48C de;
                    need(a::decodeScene48D(bytes,probe,de,nullptr,limits),"environment UV: PSG decode "+src.member+" "+de.code);
                    a::Scene48D scene;a::SceneRawCapture48D raw;
                    const a::SceneRawCaptureLimits48D rawLimits{probe.vertices_all_lods,probe.surfaces_all_lods,
                        std::size_t(probe.vertices_all_lods)*sizeof(a::SceneRawVertex48D)+
                        std::size_t(probe.surfaces_all_lods)*sizeof(a::SceneRawSurface48D)};
                    need(a::decodeSceneWithRaw48D(bytes,scene,raw,rawLimits,de,nullptr,limits),"environment UV: raw PSG capture "+src.member+" "+de.code);
                    // PORT STAND-IN (flagged): the native producer admits the single-node,
                    // zero-origin quantization frame the stock environment models author;
                    // a hierarchy or a nonzero quantization origin (quantization[4..6]) is refused.
                    need(scene.nodes.size()==1&&scene.quantization[4]==0&&scene.quantization[5]==0&&scene.quantization[6]==0,
                        "environment UV: PSG frame outside the native producer (PORT STAND-IN): "+src.member);
                    const a::SceneRawSurface48D* surface=nullptr;
                    for(const auto& r:raw.surfaces)if(r.lod==0&&r.surface==s.surfaceIndex)surface=&r;
                    need(surface&&surface->vertexCount==s.vertexCount&&
                        std::size_t(surface->vertexStart)+surface->vertexCount<=raw.vertices.size(),
                        "environment UV: raw surface does not match its mesh span: "+src.member);
                    src.qPosition=static_cast<float>(scene.quantization[0]);src.qNormal=static_cast<float>(scene.quantization[1]);
                    need(std::isfinite(src.qPosition)&&src.qPosition>0&&std::isfinite(src.qNormal)&&src.qNormal>0,
                        "environment UV: quantization scales");
                    src.vertices.reserve(surface->vertexCount);
                    for(std::uint32_t v=0;v<surface->vertexCount;++v){const auto& rv=raw.vertices[surface->vertexStart+v];
                        src.vertices.push_back({rv.position,rv.normal});}
                    src.worldSpace=scene.world_space;
                    sources_.push_back(std::move(src));
                }
                // C9: the flag belongs to the cached source, so a second placement of a
                // world-space model also takes the identity matrix.
                const bool worldSpace=sources_[k].worldSpace;
                need(sources_[k].vertices.size()==s.vertexCount,"environment UV: placement vertex count");
                Placement placement;placement.source=k;
                for(unsigned m=0;m<16;++m){
                    const double value=worldSpace?((m%5)==0?1.0:0.0):instance.matrix[m];
                    placement.matrix[m]=static_cast<float>(value);
                    need(std::isfinite(value)&&std::isfinite(placement.matrix[m]),"environment UV: placement binary32 boundary");}
                ranges_.push_back({s.vertexStart,s.vertexCount});placements_.push_back(placement);
                uvCount_+=s.vertexCount;
            }
            need(uvCount_<=a::max_mesh_vertices,"environment UV: UV count above the mesh vertex limit");
            std::array<float,4> ambient{};
            need(track.roles.lights->currentSourceGlobalAmbient50(ambient),"environment UV: global ambient unavailable");
            world_=world;lights_=track.roles.lights;authored=true;ready_=true;
            std::cerr<<"environment_uv course="<<track.code()<<" ranges="<<ranges_.size()<<" sources="<<sources_.size()
                     <<" uvs="<<uvCount_<<'\n';
            error.clear();return true;
        }catch(const std::exception& e){try{error=e.what();}catch(...){}ranges_.clear();placements_.clear();sources_.clear();return false;}
         catch(...){error="environment UV exception";return false;}
    }
    bool ready()const noexcept override{return ready_&&thread_==std::this_thread::get_id()&&world_&&world_->loaded()&&lights_;}
    const Range* ranges()const noexcept override{return ranges_.data();}
    std::size_t rangeCount()const noexcept override{return ranges_.size();}
    std::size_t uvCount()const noexcept override{return uvCount_;}
    // Cold: the renderer's descriptor is this world's mesh, and its environment
    // spans are exactly the environment passes this producer was built from.
    bool validate(const p::VehicleWindowTrack50DS& track)const noexcept override{
        if(!ready()||track.mesh!=world_->mesh()||!track.spans||!world_->spans())return false;
        const auto& spans=*world_->spans();
        if(track.spanCount!=spans.size()||environmentSpan_.size()!=spans.size())return false;
        for(std::size_t i=0;i<spans.size();++i){const auto& src=spans[i];const auto& dst=track.spans[i];
            if(dst.vertexStart!=src.vertexStart||dst.vertexCount!=src.vertexCount||
               dst.sourceEnvironmentMap!=(environmentSpan_[i]!=0))return false;
            if(dst.sourceEnvironmentMap){
                bool found=false;for(const auto& r:ranges_)if(r.vertexStart==src.vertexStart&&r.vertexCount==src.vertexCount)found=true;
                if(!found)return false;}
        }
        return true;
    }
    // Per frame: one UV per range vertex (range order) and the current ambient row.
    bool project(const g::CameraViewOutput49F& camera,std::array<float,2>* output,std::size_t capacity,
        std::array<std::uint8_t,4>& ambient)const noexcept override{
        if(!ready()||!output||capacity!=uvCount_||!math::detail::ready())return false;
        std::array<float,4> row{};std::array<std::uint8_t,4> nextAmbient{};
        if(!lights_->currentSourceGlobalAmbient50(row)||!math::projectAmbient(row,nextAmbient))return false;
        const math::Vec3 position{{camera.position.x,camera.position.y,camera.position.z}};
        std::size_t at=0;
        for(std::size_t r=0;r<ranges_.size();++r){const auto& placement=placements_[r];
            if(placement.source>=sources_.size())return false;
            const auto& src=sources_[placement.source];const auto& m=placement.matrix;
            math::Vec3 delta{},local{};
            for(unsigned k=0;k<3;++k){delta[k]=math::detail::sub(position[k],m[12+k]);if(!std::isfinite(delta[k]))return false;}
            // Camera into the model's source frame (placement rotation rows), the
            // same binary32 order as the projection boundary it replaces.
            for(unsigned k=0;k<3;++k){const float x=math::detail::mul(delta[0],m[k*4]);
                const float y=math::detail::mul(delta[1],m[k*4+1]);const float z=math::detail::mul(delta[2],m[k*4+2]);
                local[k]=k==2?math::detail::add(math::detail::add(z,x),y):k==1?
                    math::detail::add(math::detail::add(y,x),z):math::detail::add(math::detail::add(x,y),z);
                if(!std::isfinite(local[k]))return false;}
            const math::Inputs inputs{local,src.qPosition,src.qNormal};
            for(const auto& vertex:src.vertices){if(at>=capacity||!math::projectUv(vertex,inputs,output[at]))return false;++at;}
        }
        if(at!=capacity)return false;ambient=nextAmbient;return true;
    }
};

// The track's producer, or null when no pass is environment-mapped (or on a
// refusal: `error` then says why and the caller refuses the race load).
inline std::shared_ptr<const p::VehicleWindowEnvironmentUv50DS> make(const common_track50CR::Track& track,
    const a::OriginalUiMaterialTemplates50KG& templates,const a::OriginalUiMaterialRuntime50KG& runtime,
    std::string& error){
    auto owner=std::make_shared<Owner>();bool authored=false;
    if(!owner->prepare(track,templates,runtime,authored,error)||!authored)return nullptr;
    return owner;
}
} // namespace track_environment_uv50CR
