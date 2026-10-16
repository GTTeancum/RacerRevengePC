#pragma once
#include "original_wreck_assets50la.h"
#include "../assets/original_standard_collision_type0_resource50la.h"
#include <limits>

namespace rr::game {
class OriginalWreckQueries50LA;
// ONE11B190 query's shape8C, with the genuine shared resource124. Source90
// remains NULL on the11B920/type0 branch. The existing numeric PodShapeState
// is kept separately; it does not own or retain shared array storage.
class OriginalWreckType0Owners50LA final {
    friend class OriginalWreckQueries50LA;
    using Resource=rr::assets::OriginalStandardCollisionType0Resource50LA;
    struct Shape {
        SpringSceneId48N model110{}; //borrow, no independent model retain
        std::shared_ptr<Resource> resource124;
    };
    std::unique_ptr<Shape> shape8C_;
    //Native ownership staging for125890's arrays before125014 exists. Its
    //ref60 remains zero until the actual resource constructor store1250D4.
    std::shared_ptr<Resource> loading_;
    bool attempted_=false,loaded_=false,retired_=false;
    std::uint32_t boundary_=0;
    static std::uint32_t word(const std::vector<std::uint8_t>& b,std::size_t at)noexcept {
        return std::uint32_t(b[at])|(std::uint32_t(b[at+1])<<8)|
            (std::uint32_t(b[at+2])<<16)|(std::uint32_t(b[at+3])<<24);
    }
    static void freeArrays11AEE0(Resource& r)noexcept {
        //11AF24/3C/5C/7C. Shared RA-backed resources retire their actual
        //borrowed view; immutable native cache pins may remain afterwards.
        r.releaseArrays11AEE0();
    }
    bool load11B920(const OriginalWreckAssets50LA& assets,SpringSceneId48N model,
        const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        using Kind=rr::assets::OriginalStandardModelFileKind50LA;
        if(attempted_||retired_||!assets.prepared()||!model.value||!resolver)return false;
        const auto id=assets.targetFileIdentity();const auto& raw=assets.originalMembers()[3];
        const auto& target=assets.targetE4();rr::assets::OriginalStandardModelFileView50LA file;
        if(id.resolver!=resolver.get()||!resolver->modelFile(id,file,diagnostic)||
           file.kind!=Kind::colType0||!file.bytes||*file.bytes!=raw||raw.size()<84u||
           !file.member||
           target.mesh.nodes.empty()||target.mesh.faces.empty()||target.mesh.vertices.empty()||
           target.radii.size()!=target.mesh.nodes.size()||target.mesh.reverse_mode!=0u||
           word(raw,68)!=target.mesh.vertices.size()||word(raw,72)!=target.mesh.nodes.size()||
           word(raw,76)!=target.mesh.faces.size()||
           (word(raw,80)&&!target.mesh.vertex_normals.empty()))return false;
        auto resource=file.collision125014;
        if(resource&&(!resource->live()||resource->reference60_==std::numeric_limits<std::int32_t>::max()))return false;
        attempted_=true;boundary_=resource?0x125500u:0x124ff0u;
        try {shape8C_=std::make_unique<Shape>();}catch(...){return false;}
        shape8C_->model110=model;
        if(resource){
            //125594 followed by12559C/5A0. Native lookup alone did not retain.
            shape8C_->resource124=std::move(resource);++shape8C_->resource124->reference60_;
            loaded_=true;boundary_=0x1255a4u;return true;
        }
        boundary_=0x125890u;
        try {
            //Host bookkeeping allocation is not an emulated retail0x70 block.
            loading_=std::make_shared<Resource>();loading_->resolver_=resolver;loading_->file_=id;
            auto& r=*loading_;r.mesh_.local_matrix=target.mesh.local_matrix;r.mesh_.reverse_mode=target.mesh.reverse_mode;
            r.vertexCount04_=word(raw,68);r.packed18_=word(raw,80);r.root1C_=0;
            boundary_=r.packed18_?0x125ac4u:0x125bb0u;r.mesh_.vertices=target.mesh.vertices;
            boundary_=r.packed18_?0x125cd4u:0x125db0u;r.mesh_.vertex_normals=target.mesh.vertex_normals;
            boundary_=0x126038u;r.mesh_.nodes=target.mesh.nodes;r.radii_=target.radii;
            boundary_=0x12625cu;r.mesh_.faces=target.mesh.faces;
        }catch(...){return false;}
        //Only now does resource124 exist in source terms. Arrays retain the
        //same native addresses on warm sharing; no per-query mesh clones.
        auto& r=*loading_;r.constructed_=true;r.reference60_=1;
        shape8C_->resource124=std::move(loading_);boundary_=0x1250e4u;
        if(!resolver->publishCollision1326A0(id,shape8C_->resource124,diagnostic))return false;
        loaded_=true;boundary_=0x1250ecu;return true;
    }
    bool release11B600()noexcept {
        if(!attempted_||retired_)return false;
        if(shape8C_&&shape8C_->resource124){
            auto& r=*shape8C_->resource124;if(!r.live()||!r.resolver_)return false;
            r.resolver_->clearCollision1327D0(&r); //11B650, even for nonfinal release
            --r.reference60_; //11AEFC..11AF04
            if(!r.reference60_){freeArrays11AEE0(r);r.retired_=true;}
            //At positive refs source shape124 remains stale until shape8C
            //is freed. Native pins can now end because this shape ends too.
        }else if(loading_){freeArrays11AEE0(*loading_);loading_.reset();} //source124 NULL branch
        shape8C_.reset();loaded_=false;retired_=true;boundary_=0x11b918u;return true;
    }
public:
    OriginalWreckType0Owners50LA()=default;
    OriginalWreckType0Owners50LA(const OriginalWreckType0Owners50LA&)=delete;
    OriginalWreckType0Owners50LA& operator=(const OriginalWreckType0Owners50LA&)=delete;
    ~OriginalWreckType0Owners50LA(){if(shape8C_||loading_)std::terminate();}
    bool attempted()const noexcept{return attempted_;}
    bool loaded()const noexcept{return loaded_;}
    bool retired()const noexcept{return retired_;}
    std::uint32_t sourceBoundary()const noexcept{return boundary_;}
    const Resource* resource()const noexcept{return shape8C_?shape8C_->resource124.get():nullptr;}
};
}
