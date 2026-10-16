#pragma once
#include "original_standard_texture_resource50la.h"
#include "collision_mesh48i.h"
#include <exception>

namespace rr::game { class OriginalWreckType0Owners50LA; }
namespace rr::assets {
// Actual shared125014 resource lifetime, represented as host semantic arrays.
// The retained decoded source is immutable input, not these owned arrays.
// Native nodes/faces are NOT the original36/20-byte records; radii are separate.
// Only12559C/11AEE0 change signed ref60. shared_ptr merely pins host storage.
class OriginalStandardCollisionType0Resource50LA final {
    friend class rr::game::OriginalWreckType0Owners50LA;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver_;
    OriginalStandardModelFileId50LA file_{};
    CollisionMesh48I mesh_;
    std::vector<float> radii_;
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
    const CollisionMesh48I& mesh()const noexcept{return mesh_;}
    const std::vector<float>& radii()const noexcept{return radii_;}
    std::uint32_t vertexCount04()const noexcept{return vertexCount04_;}
    std::uint32_t packed18()const noexcept{return packed18_;}
    std::uint16_t root1C()const noexcept{return root1C_;}
};
}
