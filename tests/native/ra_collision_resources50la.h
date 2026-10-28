#pragma once
#include "ra_static_candidates50la.h"
#include "../../src/assets/original_standard_collision_type0_resource50la.h"
#include "../../src/game/original_wreck_components50la.h"
#include <limits>
namespace ra_pod_scene50LA {class Owner;}
namespace ra_collision_resources50LA {
namespace a=rr::assets;namespace g=rr::game;
// Canonical file/backing admission on the SAME existing candidate owner.
// Cold binding acquires no source ref60 or counted collision Entity.
class Owner final {
    friend class ::ra_pod_scene50LA::Owner;
    using Resource=a::OriginalStandardCollisionType0Resource50LA;
    struct Backing final : a::OriginalCollisionType0Backing50LA {
        std::shared_ptr<const ra_static_candidates50LA::Owner> owner;
        std::size_t index;
        Backing(std::shared_ptr<const ra_static_candidates50LA::Owner> p,std::size_t i)
            :owner(std::move(p)),index(i){}
        const a::CollisionMesh48I& mesh()const noexcept override {
            return owner->source_->sourcesDecoded()->collisions[index].mesh;
        }
        const std::vector<float>& radii()const noexcept override{return owner->radii_[index].values;}
    };
    struct Shape {
        // Source model110 is genuinely NULL on the RA Standard COL route.
        g::OriginalWreckComponent50LA* query=nullptr;
        std::shared_ptr<Resource> resource124;
        bool attempted=false;
    };
    struct Row {
        a::OriginalStandardModelFileId50LA file;
        std::uint32_t vertices=0,packed=0;
        std::shared_ptr<const Backing> backing;
        Shape shape;
    };
    std::shared_ptr<ra_static_candidates50LA::Owner> candidates_;
    std::shared_ptr<a::OriginalStandardTextureResolver50LA> resolver_;
    std::vector<Row> rows_;
    bool attempted_=false,prepared_=false;
    static std::uint32_t word(const std::vector<std::uint8_t>& b,std::size_t n)noexcept {
        return std::uint32_t(b[n])|(std::uint32_t(b[n+1])<<8)|
            (std::uint32_t(b[n+2])<<16)|(std::uint32_t(b[n+3])<<24);
    }
    Owner()=default;
    bool prepare(std::shared_ptr<ra_static_candidates50LA::Owner> candidates,
        std::shared_ptr<a::OriginalStandardTextureResolver50LA> resolver,
        a::OriginalStandardTextureDiagnostic50LA& d)noexcept {
        if(attempted_||!candidates||!candidates->prepared_||!resolver||!resolver->prepared())return false;
        attempted_=true;candidates_=std::move(candidates);resolver_=std::move(resolver);
        const auto& source=*candidates_->source_->sourcesDecoded();
        if(source.collisions.size()!=178||candidates_->radii_.size()!=candidates_->candidates_.size()||
           candidates_->candidates_.size()<178||candidates_->bindings_.size()<178)return false;
        try {
            rows_.resize(178);
            for(std::size_t i=0;i<178;++i){const auto& original=source.collisions[i];
                const auto& candidate=candidates_->candidates_[i];const auto& radius=candidates_->radii_[i];
                const auto& binding=candidates_->bindings_[i];auto& row=rows_[i];
                if(binding.originalOwner!=i||binding.candidate!=&candidate||binding.animated||binding.trigger||
                   candidate.candidate.collision.geometry.mesh!=&original.mesh||
                   candidate.nodeRadii!=radius.values.data()||candidate.nodeRadiusCount!=radius.values.size()||
                   radius.values.size()!=original.mesh.nodes.size()||original.mesh.reverse_mode)return false;
                std::vector<std::uint8_t> raw;a::DecodeError48C decode;
                if(!a::extractResMember48C(source.archive,original.member,raw,decode)||raw.size()<84||
                   word(raw,68)!=original.mesh.vertices.size()||word(raw,72)!=original.mesh.nodes.size()||
                   word(raw,76)!=original.mesh.faces.size()||
                   (word(raw,80)&&!original.mesh.vertex_normals.empty()))return false;
                //The resolver's existing sealed provider must independently
                //authenticate exact mounted member bytes. No synthetic file ID.
                if(!resolver_->bindModelMember(original.member,a::OriginalStandardModelFileKind50LA::colType0,
                    raw,row.file,d))return false;
                row.vertices=word(raw,68);row.packed=word(raw,80);
                row.backing=std::make_shared<Backing>(candidates_,i);
            }
        }catch(...){return false;}
        prepared_=true;return true;
    }
    // Private future27B040 join only AFTER its actual counted11B190 prefix.
    // Decoded readiness alone can never manufacture a source shape retain.
    bool load11B920(std::size_t index,g::OriginalWreckComponent50LA& actualQuery,
        a::OriginalStandardTextureDiagnostic50LA& d)noexcept {
        if(!prepared_||index>=rows_.size()||actualQuery.reference00!=1||
           actualQuery.kind!=g::OriginalWreckComponentKind50LA::collision11B190||
           !actualQuery.sessionGeneration||!actualQuery.objectKey)return false;
        auto& row=rows_[index];auto& shape=row.shape;
        if(shape.attempted)return false;
        a::OriginalStandardModelFileView50LA file;
        if(!resolver_->modelFile(row.file,file,d)||file.kind!=a::OriginalStandardModelFileKind50LA::colType0)return false;
        auto resource=file.collision125014;
        if(resource&&(!resource->live()||resource->reference60_==std::numeric_limits<std::int32_t>::max()))return false;
        shape.attempted=true;shape.query=&actualQuery;
        if(resource){shape.resource124=std::move(resource);++shape.resource124->reference60_;return true;}
        try {resource=std::make_shared<Resource>();}catch(...){return false;}
        resource->resolver_=resolver_;resource->file_=row.file;resource->borrowedBacking_=row.backing;
        resource->vertexCount04_=row.vertices;resource->packed18_=row.packed;resource->root1C_=0;
        resource->constructed_=true;resource->reference60_=1;
        shape.resource124=std::move(resource);
        return resolver_->publishCollision1326A0(row.file,shape.resource124,d);
    }
    // Exact same-object checks for the reached World loader and its partial
    // retirement. Never promotes cold prepared() into source11B920 success.
    bool attemptedBy11B920(std::size_t index,const g::OriginalWreckComponent50LA& query)const noexcept {
        return index<rows_.size()&&rows_[index].shape.attempted&&rows_[index].shape.query==&query;
    }
    bool loadedBy11B920(std::size_t index,const g::OriginalWreckComponent50LA& query)const noexcept {
        if(!attemptedBy11B920(index,query))return false;
        const auto& shape=rows_[index].shape;
        return shape.resource124&&shape.resource124->live()&&
            shape.resource124->resolver_==resolver_&&
            // A cached first provider may own a different decoded allocation.
            // Compare the sealed resolver/file identity, not mesh pointers.
            shape.resource124->file_.resolver==rows_[index].file.resolver&&
            shape.resource124->file_.row==rows_[index].file.row;
    }
    bool canRelease11B600(std::size_t index,const g::OriginalWreckComponent50LA& query)const noexcept {
        if(!attemptedBy11B920(index,query))return false;
        const auto& resource=rows_[index].shape.resource124;
        return !resource||(resource->live()&&resource->resolver_==resolver_);
    }
    bool release11B600(std::size_t index,const g::OriginalWreckComponent50LA& query)noexcept {
        return canRelease11B600(index,query)&&release11B600(index);
    }
    bool release11B600(std::size_t index)noexcept {
        if(index>=rows_.size())return false;auto& shape=rows_[index].shape;
        if(!shape.attempted)return false;
        if(shape.resource124){auto& resource=*shape.resource124;
            if(!resource.live()||resource.resolver_!=resolver_)return false;
            resolver_->clearCollision1327D0(&resource); //Before decrement, even for positive remaining refs.
            if(!--resource.reference60_){resource.releaseArrays11AEE0();resource.retired_=true;}
            shape.resource124.reset();
        }
        shape.query=nullptr;return true;
    }
    bool canCloseUnloadedBacking()const noexcept {
        for(const auto& row:rows_)if(row.shape.query||row.shape.resource124)return false;
        return true;
    }
    bool closeUnloadedBacking()noexcept {
        if(!canCloseUnloadedBacking())return false;
        std::vector<Row>().swap(rows_);candidates_.reset();resolver_.reset();prepared_=false;return true;
    }
public:
    ~Owner(){if(!closeUnloadedBacking())std::terminate();}
};
}
