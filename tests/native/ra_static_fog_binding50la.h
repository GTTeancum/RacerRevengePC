#pragma once
#include "ra_static_world50la.h"
#include "ra_source_camera_ownership50la.h"
#include "../../src/game/original_racer_model_view50la.h"
#include <cmath>
#include <limits>

namespace ra_static_fog50LA {
// Read-only source operand binding. Deliberately independent of the separate
// unused fog-byte numeric candidate and of all renderer/material decoders.
class Binding final {
    friend class ra_cold_aggregate50LA::Owner;
    std::shared_ptr<ra_static_world50LA::Owner> world_;
    struct FogLease {
        std::shared_ptr<ra_source_camera50LA::OwnershipBridge> owner;
        ra_source_camera50LA::OwnershipBridge::StaticFogPublication issued;
        bool read(CameraObservation& out)const noexcept {
            ra_source_camera50LA::OwnershipBridge::StaticFogPublication now;
            CameraObservation next;
            if(!owner||!owner->readStaticFogPublication(now,next)||
               now.camera.session!=issued.camera.session||now.camera.generation!=issued.camera.generation||
               now.camera.publication!=issued.camera.publication||now.camera.frame!=issued.camera.frame||
               now.fog!=issued.fog||now.revision!=issued.revision)return false;
            out=next;return true;
        }
    } camera_;
    std::vector<PlacementObservation> placements_;
    Binding()=default;
    bool current(CameraObservation& out)const noexcept {
        return world_&&world_->drawPlanCurrent()&&camera_.read(out);
    }
    static bool exactFloat(double source,float& out)noexcept {
        if(!std::isfinite(source)||source>std::numeric_limits<float>::max()||
           source<-std::numeric_limits<float>::max())return false;
        const auto value=static_cast<float>(source);
        if(!std::isfinite(source)||!std::isfinite(value)||double(value)!=source)return false;
        out=value;return true;
    }
    static bool nativePlacementFloat(double source,float& out)noexcept {
        // World48F stores normalized placement bases as binary64. The actual
        // native static scene/GPU matrices consume their binary32 projection;
        // requiring an exact binary64 round trip rejects rotated scenery.
        if(!std::isfinite(source)||source>std::numeric_limits<float>::max()||
           source<-std::numeric_limits<float>::max())return false;
        out=static_cast<float>(source);return std::isfinite(out);
    }
    bool prepare(std::shared_ptr<ra_static_world50LA::Owner> world,
        std::shared_ptr<ra_source_camera50LA::OwnershipBridge> camera) {
        world_=std::move(world);camera_.owner=std::move(camera);CameraObservation observation;
        if(!camera_.owner||!camera_.owner->readStaticFogPublication(camera_.issued,observation)||
           !current(observation))return false;
        for(std::size_t i=0;i<world_->placements_.size();++i){
            const auto& p=*world_->placements_[i];const auto* scene=p.payload?p.payload->scene():nullptr;
            if(!scene||scene->nodes.size()!=1||scene->nodes[0].parent!=-1||scene->identity_matrix_palette)return false;
            PlacementObservation next;next.placement=i;next.worldInstance=p.worldInstance;
            next.worldSpace=scene->world_space;
            const auto& node=scene->nodes[0];
            //28CB88/28CC3C load source row0 center140/radius194; source1A0==1.
            for(unsigned a=0;a<3;++a){
                if(!exactFloat(node.stored_bounds[a],next.center140[a])||
                   !nativePlacementFloat(p.authoredMatrix[12+a],next.localTranslation[a]))return false;
            }
            if(!exactFloat(node.stored_bounds[6],next.radius194)||next.radius194<0||
               !exactFloat(scene->quantization[0],next.positionScale)||next.positionScale<=0)return false;
            if(scene->world_space){
                next.packetModel[0]=next.packetModel[5]=next.packetModel[10]=next.packetModel[15]=1;
                for(unsigned a=0;a<3;++a)if(!exactFloat(scene->quantization[4+a],next.packetModel[12+a]))return false;
            }else for(unsigned a=0;a<16;++a)if(!nativePlacementFloat(p.authoredMatrix[a],next.packetModel[a]))return false;
            // Identical26F0E0..26F190 numeric leaf; center140 replaces160 for
            // the proven single-node branch. No radius recomputation/AABB.
            std::array<float,4> center{{next.center140[0],next.center140[1],next.center140[2],0}};
            if(!rr::game::projectOriginalRacerDistance26F070(center,next.localTranslation,
                observation.position,next.distanceCandidate))return false;
            const auto far=rr::game::original_racer_model_view_detail50LA::add(next.distanceCandidate,next.radius194);
            if(!std::isfinite(far))return false;
            next.computeFogCandidate02=!(far<observation.source.start34);
            placements_.push_back(next);
        }
        return current(observation);
    }
    bool read(std::size_t placement,CameraObservation& camera,PlacementObservation& out)const noexcept {
        CameraObservation next;
        if(placement>=placements_.size()||!current(next))return false;
        camera=next;out=placements_[placement];return true;
    }
    static double decodedPosition(double raw,double q,double origin)noexcept {
        // Match the importer's separate double product and sum.
        volatile double product=raw*q;return product+origin;
    }
    bool rawPosition(std::size_t surface,std::size_t vertex,std::array<std::int32_t,3>& out)const noexcept {
        CameraObservation camera;
        if(!current(camera)||surface>=world_->surfaces_.size())return false;
        const auto& span=world_->surfaces_[surface];
        if(span.placement>=world_->placements_.size())return false;
        const auto& placement=*world_->placements_[span.placement];const auto* scene=placement.payload->scene();
        if(!scene||span.sourceLod>=scene->lods.size()||span.sourceSurface>=scene->lods[span.sourceLod].surfaces.size())return false;
        const auto& input=scene->lods[span.sourceLod].surfaces[span.sourceSurface];
        if(vertex>=input.vertices.size()||input.vertices[vertex].bone!=0)return false;
        const double q=scene->quantization[0];if(!std::isfinite(q)||q<=0)return false;
        std::array<std::int32_t,3> next{};
        for(unsigned a=0;a<3;++a){
            const double value=input.vertices[vertex].position_local[a],origin=scene->quantization[4+a];
            const double raw=(value-origin)/q;
            if(!std::isfinite(raw)||raw<-8388608.||raw>8388607.||std::trunc(raw)!=raw||decodedPosition(raw,q,origin)!=value)return false;
            // A round trip alone is insufficient when origin swallows quantized steps.
            // Positive q gives monotonic decoding; equal neighbors prove ambiguity.
            if((raw>-8388608.&&decodedPosition(raw-1.,q,origin)==value)||
               (raw<8388607.&&decodedPosition(raw+1.,q,origin)==value))return false;
            next[a]=static_cast<std::int32_t>(raw);
        }
        out=next;return true;
    }
};
}
