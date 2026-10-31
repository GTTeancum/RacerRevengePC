#pragma once
#include "original_standard_texture_resource50la.h"
#include "collision_mesh48i.h"
#include <exception>
#include <array>
#include <memory>
#include <optional>

namespace ra_collision_resources50LA { class Owner; }
namespace rr::game { class OriginalWreckType0Owners50LA; }
namespace rr::platform { class OriginalUiGarageSceneCollision50KG; }
namespace rr::assets {
class OriginalSourceCollisionFile50LA;
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
    friend class rr::platform::OriginalUiGarageSceneCollision50KG;
    friend class ::ra_collision_resources50LA::Owner;
    friend class OriginalSourceCollisionFile50LA;
    friend class OriginalStandardTextureResolver50LA;
    // Private raw source125014 facet, independent of semantic/prewarmed mesh.
    // Type0 allocates this control object BEFORE issuing nullable raw0x70.
    // Host pointer lanes have typed aliases; raw guest-address bytes remain unknown.
    struct SourceFacet {
        const rr::game::OriginalWreckType0Owners50LA* owner=nullptr;
        const OriginalSourceCollisionFile50LA* file=nullptr;
        bool attempted=false;
        std::unique_ptr<std::array<std::uint8_t,0x70>> raw;
        std::array<std::uint8_t,0x70> bytes{}; // Retained store history after native raw retirement.
        std::array<std::uint8_t,0x70> known{}; // One 0/1 byte per source byte.
        std::optional<const void*> nodes08,faces0C,vertices10,normals14;
        // Bits0..7:00,08,0C,04,10,14,18,1C stores; bits8..23:
        // sixteen matrix words20..5C in original order; bit24:ref60.
        std::uint32_t storeMask=0;
        bool refInitialized=false,nativeWithdrawn=false;
        // Durable original store history; native compensation never rewrites it.
        std::uint32_t referenceBits60=0;
    } source_;
    static constexpr std::uint32_t sourceAllStores1250D4=0x01ffffffu;
    const OriginalSourceCollisionFile50LA* sourceAssociationBorrower_=nullptr;
    const void* sourceResultBorrower_=nullptr; //Exact unpublished/published11BBC0 result occurrence.
    const void* sourceDirectTransformBorrower_=nullptr; //Exact E4 getter/transform native lease; no source retain.
    const void* sourceUpdateBorrower_=nullptr; //Distinct per-COL post-placement E4 or result-backed E0 getter/update custody.
    const void* sourceContactBorrower_=nullptr; //Exact296190 point-reader/record occurrence.
    bool sourceAssociationReady1326A0(const OriginalSourceCollisionFile50LA* file,
        const rr::game::OriginalWreckType0Owners50LA* owner)const noexcept {
        // Deliberately no File.current()/owner recursion. The caller proves
        // their live custody independently before reaching original1326A0.
        const auto& s=source_;
        if(!file||!owner||!s.attempted||s.file!=file||s.owner!=owner||
           s.nativeWithdrawn||!s.raw||s.storeMask!=sourceAllStores1250D4||
           !s.refInitialized||s.referenceBits60!=1u||reference60_!=1||
           constructed_||retired_||borrowedBacking_||
           !s.nodes08||!s.faces0C||!s.vertices10||!s.normals14||
           (sourceAssociationBorrower_&&sourceAssociationBorrower_!=file))return false;
        for(std::size_t i=0;i<s.known.size();++i)
            if(s.known[i]>1u||(s.known[i]&&s.bytes[i]!=(*s.raw)[i]))return false;
        // Numeric type/ref are genuinely known. Copied matrix bytes can remain
        // unknown after source short reads; completed stores do not invent them.
        for(std::size_t i=0;i<4;++i){
            if(s.known[i]!=1u||(*s.raw)[i]!=0u||s.known[0x60+i]!=1u||
               (*s.raw)[0x60+i]!=(i==0?1u:0u))return false;
        }
        for(std::size_t i=8;i<0x18;++i)if(s.known[i])return false;
        for(std::size_t i=0x1e;i<0x20;++i)if(s.known[i])return false;
        for(std::size_t i=0x64;i<0x70;++i)if(s.known[i])return false;
        return true;
    }
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
    ~OriginalStandardCollisionType0Resource50LA(){
        // Native cancellation must explicitly compensate a reached source ref
        // only after revoking association, preserving referenceBits60 history.
        if(reference60_!=0||sourceAssociationBorrower_||sourceResultBorrower_||sourceDirectTransformBorrower_||sourceUpdateBorrower_||sourceContactBorrower_||
           (source_.attempted&&!source_.nativeWithdrawn))std::terminate();
    }
    bool live()const noexcept{return !source_.attempted&&constructed_&&!retired_&&reference60_>0;}
    std::int32_t sourceReference60()const noexcept{return reference60_;}
    OriginalStandardModelFileId50LA fileId()const noexcept{return file_;}
    const CollisionMesh48I& mesh()const noexcept{return borrowedBacking_?borrowedBacking_->mesh():mesh_;}
    const std::vector<float>& radii()const noexcept{return borrowedBacking_?borrowedBacking_->radii():radii_;}
    std::uint32_t vertexCount04()const noexcept{return vertexCount04_;}
    std::uint32_t packed18()const noexcept{return packed18_;}
    std::uint16_t root1C()const noexcept{return root1C_;}
};
}
