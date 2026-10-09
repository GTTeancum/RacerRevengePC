#pragma once
// Cold original-pod collision ownership. COL XYZ, tree order, local matrices,
// and type0 faces come from the selected RES, never from the display mesh.
// Endpoint W words do NOT exist in COL. A caller must explicitly supply every
// word; this adapter neither recovers historical heap contents nor guesses W.
#include "../../src/game/type2_resource49x.h"
#include "../../src/game/pod_shape49w.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/import_collision48g.h"
#include <algorithm>
#include <array>
#include <cfenv>
#include <cstring>
#include <fstream>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace native_racer_collision50HW {
using namespace rr::game;
enum class QueryBody { Main, Secondary };
struct ArchiveBinding {
    std::string archivePath;
    // Exact original prefix, including trailing separator: "Pods\\Pod03\\".
    // This is archive metadata only, never a destination filesystem path.
    std::string memberPrefix;
};
struct EndpointPolicy {
    void* context=nullptr;
    // Invoked once per original ray, in resource order, during cold loading.
    // Return false if this caller cannot supply the requested opaque words.
    // The callback must not mutate this owner or throw.
    bool (*words)(void*,QueryBody,std::size_t,Type2OpaqueEndpointWords49X&)=nullptr;
};
struct Target {
    rr::assets::CollisionMesh48I mesh;
    std::vector<float> radii;
    std::size_t maxDepth=0;
    PodShapeSource49W source{};
};
struct ScratchRequirements {
    std::size_t mainPairSlots=0,secondaryPairSlots=0;
    std::size_t mainStack=0,secondaryStack=0;
};
struct Receipt {
    std::size_t archiveBytes=0;
    std::array<std::size_t,4> memberBytes{}; // podboxm,chariotm,podboxf,chariotf
    bool endpointWordsCallerSupplied=false;
};
class Owner {
public:
    Owner()=default;
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;
    Owner& operator=(Owner&&)=delete;

    // Fresh cold setup only. Failure does not publish partial storage. Once
    // prepared, all borrowed views stay immutable and stable for Owner's life.
    bool prepare(const ArchiveBinding& binding,const EndpointPolicy& endpoint,
                 std::string& error) {
        if(storage_||!endpoint.words||binding.archivePath.empty()||
           binding.memberPrefix.empty()||std::fegetround()!=FE_TONEAREST)
            return fail(error,"invalid fresh original-pod collision binding");
        std::ifstream file(binding.archivePath,std::ios::binary);
        if(!file)return fail(error,"original pod collision archive unavailable");
        std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(file),{}};
        rr::assets::ResArchive48C archive;rr::assets::DecodeError48C why;
        if(!rr::assets::decodeResource48C(bytes,archive,why))
            return fail(error,"original pod collision archive rejected");
        auto staged=std::make_unique<Storage>();
        staged->binding=binding;staged->receipt.archiveBytes=bytes.size();
        constexpr const char* members[]={"podboxm.col","chariotm.col","podboxf.col","chariotf.col"};
        for(std::size_t i=0;i<4;++i){
            if(!rr::assets::extractResMember48C(archive,binding.memberPrefix+members[i],staged->raw[i],why))
                return fail(error,"original pod collision member unavailable");
            staged->receipt.memberBytes[i]=staged->raw[i].size();
        }
        if(!decodeQuery(staged->raw[0],QueryBody::Main,endpoint,staged->query[0],error)||
           !decodeQuery(staged->raw[1],QueryBody::Secondary,endpoint,staged->query[1],error)||
           !decodeTarget(staged->raw[2],staged->target[0],error)||
           !decodeTarget(staged->raw[3],staged->target[1],error))return false;
        staged->receipt.endpointWordsCallerSupplied=true;
        storage_=std::move(staged);error.clear();return true;
    }
    bool prepared()const noexcept{return bool(storage_);}
    // Accessors require successful prepare; no per-frame allocation or lookup.
    const Type2ResourceView49X& main()const noexcept{return storage_->query[0].view;}
    const Type2ResourceView49X& secondary()const noexcept{return storage_->query[1].view;}
    const Target& mainTarget()const noexcept{return storage_->target[0];}
    const Target& secondaryTarget()const noexcept{return storage_->target[1];}
    const Receipt& receipt()const noexcept{return storage_->receipt;}
    const ArchiveBinding& binding()const noexcept{return storage_->binding;}
    // Exact maximum target depth must include every static/dynamic candidate
    // admitted by that query. Pair capacity is ray count, not hit count.
    bool requirements(std::size_t targetDepth,ScratchRequirements& out)const noexcept {
        if(!storage_)return false;
        const auto maximum=std::numeric_limits<std::size_t>::max();
        if(targetDepth==maximum||main().maxDepth>maximum-1-targetDepth||
           secondary().maxDepth>maximum-1-targetDepth)return false;
        out={main().rayCount,secondary().rayCount,
            1+main().maxDepth+targetDepth,1+secondary().maxDepth+targetDepth};
        return true;
    }
private:
    struct Query {
        std::vector<Type2Node49X> nodes;
        std::vector<Type2Ray49X> rays;
        Type2ResourceView49X view{};
    };
    struct Storage {
        ArchiveBinding binding;
        Receipt receipt;
        std::array<std::vector<std::uint8_t>,4> raw;
        std::array<Query,2> query;
        std::array<Target,2> target;
    };
    static bool fail(std::string& e,const char* why){e=why;return false;}
    static std::uint32_t u32(const std::uint8_t* p)noexcept {
        return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|
            (std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
    }
    static bool decodeQuery(const std::vector<std::uint8_t>& bytes,QueryBody body,
                            const EndpointPolicy& policy,Query& out,std::string& error) {
        if(bytes.size()<92)return fail(error,"short original pod type2 resource");
        const auto nr=u32(bytes.data()+80),nn=u32(bytes.data()+84),nl=u32(bytes.data()+88);
        if(!nn||nn>32767||nr!=nn+1||nr!=nl||nr>32768||
           bytes.size()!=92u+std::size_t(32)*nn+std::size_t(24)*nr)
            return fail(error,"invalid original pod type2 resource extents");
        out.nodes.resize(nn);out.rays.resize(nr);
        std::vector<Type2OpaqueEndpointWords49X> opaque(nr);
        for(std::size_t i=0;i<nr;++i)
            if(!policy.words(policy.context,body,i,opaque[i]))
                return fail(error,"caller endpoint-W policy does not cover original pod rays");
        std::vector<std::uint32_t> topology(std::size_t(nn)+nr);
        if(decodeType2Resource49X(bytes.data(),bytes.size(),opaque.data(),opaque.size(),
            out.nodes.data(),out.nodes.size(),out.rays.data(),out.rays.size(),
            {topology.data(),topology.size()},out.view)!=Type2ResourceStatus49X::Complete)
            return fail(error,"original pod type2 native decoding rejected");
        return true;
    }
    static bool decodeTarget(const std::vector<std::uint8_t>& bytes,Target& out,std::string& error) {
        rr::assets::Collision48G source;rr::assets::DecodeError48C why;
        std::vector<std::uint8_t> cache;
        if(!rr::assets::decodeCollision48G(bytes,source,why)||
           !rr::assets::encodeCollisionAsset48G(source,cache,error)||
           !rr::assets::loadCollisionMesh48I(cache,out.mesh,error)||out.mesh.nodes.empty())
            return fail(error,"original pod type0 native decoding rejected");
        out.radii.resize(out.mesh.nodes.size());
        std::vector<std::uint32_t> topology(out.mesh.nodes.size()+out.mesh.faces.size());
        if(prepareStaticTargetRadii49X(out.mesh,out.radii.data(),out.radii.size(),
            {topology.data(),topology.size()},out.maxDepth)!=Type2ResourceStatus49X::Complete)
            return fail(error,"original pod type0 target tree rejected");
        // Same projection as decodePodShapeSource49W, after the full48G/49X
        // validation. Its Pod01-only extent ceilings are not used as geometry.
        std::memcpy(out.source.local40.v,out.mesh.local_matrix.data(),sizeof(BodyMatrix48G));
        std::memcpy(out.source.rootCenter.v,out.mesh.nodes[0].center.data(),sizeof(Vector3_48C));
        std::memcpy(out.source.rootHalf.v,out.mesh.nodes[0].half.data(),sizeof(Vector3_48C));
        return true;
    }
    std::unique_ptr<Storage> storage_;
};
}
