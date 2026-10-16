#pragma once
#include "original_standard_texture_resource50la.h"
#include "collision_mesh48i.h"
#include <exception>

namespace ra_collision_resources50LA { class Owner; }
namespace rr::game { class OriginalWreckType0Owners50LA; }
namespace rr::assets {
// Typed immutable native prewarming lease. Only authenticated owners can bind
// it into a resource; retaining cache bytes is separate from source ref60.
class OriginalCollisionType0Backing50LA {
public:
    virtual ~OriginalCollisionType0Backing50LA()=default;
    virtual const CollisionMesh48I& mesh()const noexcept=0;
    virtual const std::vector<float>& radii()const noexcept=0;
};
// Actual shared125014 resource lifetime, represented as host semantic arrays.
// Wreck resources own their arrays; authenticated RA resources may retain a
// typed immutable native cache lease. Both expose the same source ref60.
// Native nodes/faces are NOT the original36/20-byte records; radii are separate.
// Only12559C/11AEE0 change signed ref60. shared_ptr merely pins host storage.
class OriginalStandardCollisionType0Resource50LA final {
    friend class rr::game::OriginalWreckType0Owners50LA;
    friend class ::ra_collision_resources50LA::Owner;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    OriginalStandardModelFileId50LA file_{};
    CollisionMesh48I mesh_;
    std::vector<float> radii_;
    std::shared_ptr<const OriginalCollisionType0Backing50LA> borrowedBacking_;
    void releaseArrays11AEE0()noexcept {
        // Native immutable cache storage may outlive the source resource.
        // Retire this resource view without clearing somebody else's arrays.
        borrowedBacking_.reset();
        decltype(mesh_.nodes){}.swap(mesh_.nodes);decltype(radii_){}.swap(radii_);
        decltype(mesh_.faces){}.swap(mesh_.faces);decltype(mesh_.vertices){}.swap(mesh_.vertices);
        decltype(mesh_.vertex_normals){}.swap(mesh_.vertex_normals);
    }
    std::int32_t reference60_=0;
    std::uint32_t vertexCount04_=0,packed18_=0;
    std::uint16_t root1C_=0;
    bool constructed_=false,retired_=false;
public:
    OriginalStandardCollisionType0Resource50LA()=default;
    OriginalStandardCollisionType0Resource50LA(const OriginalStandardCollisionType0Resource50LA&)=delete;
    OriginalStandardCollisionType0Resource50LA& operator=(const OriginalStandardCollisionType0Resource50LA&)=delete;
    ~OriginalStandardCollisionType0Resource50LA(){if(reference60_!=0)std::terminate();}
    bool live()const noexcept{return constructed_&&!retired_&&reference60_>0;}
    std::int32_t sourceReference60()const noexcept{return reference60_;}
    OriginalStandardModelFileId50LA fileId()const noexcept{return file_;}
    const CollisionMesh48I& mesh()const noexcept{return borrowedBacking_?borrowedBacking_->mesh():mesh_;}
    const std::vector<float>& radii()const noexcept{return borrowedBacking_?borrowedBacking_->radii():radii_;}
    std::uint32_t vertexCount04()const noexcept{return vertexCount04_;}
    std::uint32_t packed18()const noexcept{return packed18_;}
    std::uint16_t root1C()const noexcept{return root1C_;}
};
}
