#pragma once
// Cold-only real Pod01 owner construction for the50EW recovery join.
// No oracle, guest execution, current-position/grid replacement or hot loading.
#include "undertrack_reset50ew.h"
#include "type2_bounds50by.h"
#include "collection_projection50cf.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/import_collision48g.h"
#include "../../src/assets/pod_shape49w.h"
#include "../../src/assets/pod_physics49n.h"
#include <array>
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace undertrack_owners50ew_fixture {
using namespace rr::game;
namespace assets=rr::assets;
struct SourceGlobals {
    // Actual application category-registry results. Numbers are NOT inferred
    // from names, copied from a different query, or fetched from captured RAM.
    std::uint32_t podCategory3E0718=0,extraPodCategory3B14B0=0;
    std::uint32_t movementCategory3E0720=0,probeCategory4342A0=0;
    std::uint8_t collisionTypeTag=0;
    // Source11B190 preservesF8 bits6/7. Native identity is chosen by the caller,
    // while unrelated owner+31 flags and copied sentinel remain explicit.
    std::uint8_t reservedF8[3]{},flags31[3]{};
    std::uint32_t identity[3]{}; //694,7A4,348; distinct from existing690/secondary.
    float sentinel3E02C0[4]{};
};
struct ColdReceipt {
    std::size_t podArchiveBytes=0,podBoxFBytes=0,podBoxMBytes=0;
    float vehicle324=0,maxLiftHeight=0;
    SpringVector4_48N probe130{},probe140{};
    std::uint32_t sourceProbeCall=0x21494c,sourcePairConstructor=0x11c3f0;
    std::uint32_t sourcePodLoad=0x212238,sourceMovementConstructor=0x212444;
};
struct Owners {
    PodShapeSource49W podSource{};
    assets::CollisionMesh48I podMesh;
    Type2Ray49X probeRay{};
    Type2ResourceView49X probeResource{};
    CollisionFilter48J filters[3]; //694,7A4,348.
    Type2StaticQuery49X probeQuery{};
    SpringVector4_48N movementEndpoints[2]{};
    std::uint32_t movementMetadata[2]{},movementTag=3;
    CollisionSphere48J movementSphere{};
    LiveZoneMembership50EN memberships[3]{};
    std::vector<CollectionZoneLink50BX> links[3];
    std::vector<LiveZoneEntry50EN> entries[3];
    ColdReceipt receipt{};
    bool prepared=false;
    Owners()=default;
    Owners(const Owners&)=delete;Owners& operator=(const Owners&)=delete;
    Owners(Owners&&)=delete;Owners& operator=(Owners&&)=delete;
    LiveZoneStorage50EN storage(unsigned i) noexcept {
        return {links[i].data(),entries[i].data(),links[i].size()};
    }
    // The disabled-probe step consumes this same source140. Publish once during
    //cold assembly so its parameter and new resource agree, never as a live reset.
    bool bindProbeLocal140(VehicleFrameParameters49W& parameters) const noexcept {
        if(!prepared||std::bit_cast<std::uint32_t>(parameters.visual.verticalLimit7EC)!=
           std::bit_cast<std::uint32_t>(receipt.maxLiftHeight))return false;
        std::memcpy(parameters.bodies.main.probeLocal140,receipt.probe140.v,16);return true;
    }
    // No retained pointer escapes beforeprepare succeeds. Keep this object and
    // its vectors at stable addresses afterward; do not resize in live frames.
    UndertrackQueryOwners50EW binding(Type2StaticQuery49X& main690,
        LiveZoneMembership50EN& mainMembership,const LiveZoneStorage50EN& mainStorage,
        CollectionRef50BX mainRef,const std::array<CollectionRef50BX,3>& addedRefs) noexcept {
        UndertrackQueryOwners50EW out{};if(!prepared)return out;
        out.query[0]={&mainMembership,mainStorage,mainRef,&main690};
        out.query[1]={&memberships[0],storage(0),addedRefs[0]};
        out.query[2]={&memberships[1],storage(1),addedRefs[1],nullptr,
            movementEndpoints,movementMetadata,&movementTag,&movementSphere};
        out.query[3]={&memberships[2],storage(2),addedRefs[2],&probeQuery};
        return out;
    }
};
namespace detail {
inline bool fail(std::string& error,const char* message){error=message;return false;}
inline std::uint32_t u32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
}
inline bool read(const char* path,std::vector<std::uint8_t>& bytes,std::string& error){
    std::ifstream f(path,std::ios::binary);if(!f)return fail(error,"cannot read actual Pod01 archive");
    bytes.assign(std::istreambuf_iterator<char>(f),{});return true;
}
inline std::string_view trim(std::string_view s) noexcept {
    while(!s.empty()&&(s.front()==' '||s.front()=='\t'||s.front()=='\r'))s.remove_prefix(1);
    while(!s.empty()&&(s.back()==' '||s.back()=='\t'||s.back()=='\r'))s.remove_suffix(1);return s;
}
inline char lower(char c) noexcept {return c>='A'&&c<='Z'?char(c+32):c;}
inline bool equal(std::string_view a,std::string_view b) noexcept {
    if(a.size()!=b.size())return false;for(std::size_t i=0;i<a.size();++i)if(lower(a[i])!=lower(b[i]))return false;return true;
}
inline bool lift(const std::vector<std::uint8_t>& phy,float& value,std::string& error){
    bool selected=false,seen=false;std::string_view text(reinterpret_cast<const char*>(phy.data()),phy.size());
    for(std::size_t at=0;at<text.size();){auto end=text.find('\n',at);if(end==std::string_view::npos)end=text.size();
        auto line=trim(text.substr(at,end-at));at=end+1;
        if(line.empty()||line.front()==';')continue;
        if(line.front()=='['){if(line.size()<3||line.back()!=']')return fail(error,"invalid PHY section");
            selected=equal(trim(line.substr(1,line.size()-2)),"Repulsor");continue;}
        if(!selected)continue;const auto split=line.find('=');if(split==std::string_view::npos)continue;
        if(!equal(trim(line.substr(0,split)),"MaxLiftHeight"))continue;
        const auto token=trim(line.substr(split+1));
        if(seen||!assets::parsePodPhysicsNumber49N(token.data(),token.size(),value))return fail(error,"invalid or duplicate authored MaxLiftHeight");seen=true;
    }
    return seen||fail(error,"actual PHY lacks Repulsor MaxLiftHeight");
}
inline float add(float a,float b) noexcept {volatile float v=a+b;return v;}
inline float sub(float a,float b) noexcept {volatile float v=a-b;return v;}
inline float mul(float a,float b) noexcept {volatile float v=a*b;return v;}
inline BodyMatrix48G identity() noexcept {BodyMatrix48G m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1.f;return m;}
// Original113070's constructor-identity inverse/107140 endpoint conversion.
// Retain all XYZ multiply/add steps; the fresh shape's local matrix is identity.
inline SpringVector4_48N identityPoint(const SpringVector4_48N& p) noexcept {
    const auto m=identity();SpringVector4_48N out{};
    for(unsigned i=0;i<3;++i)out.v[i]=add(add(add(mul(m.v[i],p.v[0]),mul(m.v[4+i],p.v[1])),mul(m.v[8+i],p.v[2])),0.f);
    out.v[3]=1.f;return out;
}
}

// Cold file/geometry preparation only. Existing690 must reference actual
//podboxm data: the fresh decode is compared to it, including every node/ray XYZ.
// Existing opaque endpoint W words are retained solely for the comparison.
// Uses source freshly constructed190 identity at2121C8 to derive324; it does
//not read the now-moving vehicle pose. Body center1E0 is explicit current
//constructor/setup input, not a supplied spawn or a duplicate body authority.
// The actual694 COL is fully decoded into owned mesh storage and its source
//projection must match canonical parameters.shape. No physics state is changed.
// Membership arrays allocate here only; registry refs/attachment stay external.
// False can modify this unpublished fresh Owners object; never call on a live
//or already-prepared owner. All published runtime APIs remain allocation-free.
inline bool prepare(Owners& out,const Type2StaticQuery49X& main690,
    const VehicleFrameParameters49W& parameters,const std::vector<std::uint8_t>& phy,
    const SourceGlobals& globals,std::size_t membershipCapacity,std::string& error,
    const char* archivePath="work/native/project/disc/PODS/POD01/POD01.RES") {
    if(out.prepared||std::fegetround()!=FE_TONEAREST||!main690.resource||!main690.queryScene.value||
       !membershipCapacity||membershipCapacity>65536)return detail::fail(error,"invalid fresh owner setup");
    for(unsigned i=0;i<3;++i){if(!globals.identity[i])return detail::fail(error,"native owner identity is unbound");
        if(main690.filter&&globals.identity[i]==main690.filter->identity)return detail::fail(error,"new owner identity aliases690");
        for(unsigned j=0;j<i;++j)if(globals.identity[i]==globals.identity[j])return detail::fail(error,"new owner identities must be distinct");}
    std::vector<std::uint8_t> bytes,podF,podM;
    if(!detail::read(archivePath,bytes,error))return false;
    assets::ResArchive48C archive;assets::DecodeError48C decode;
    if(!assets::decodeResource48C(bytes,archive,decode)||
       !assets::extractResMember48C(archive,"Pods\\Pod01\\podboxf.col",podF,decode)||
       !assets::extractResMember48C(archive,"Pods\\Pod01\\podboxm.col",podM,decode))return detail::fail(error,"actual Pod01 query resources unavailable");
    if(podM.size()<92)return detail::fail(error,"short actual podboxm resource");
    const auto nr=detail::u32(podM.data()+80),nn=detail::u32(podM.data()+84);
    if(nr>32768||nn>32767||nr!=main690.resource->rayCount||nn!=main690.resource->nodeCount||
       (nr&&!main690.resource->rays)||(nn&&!main690.resource->nodes))return detail::fail(error,"690 resource does not match actual podboxm extent");
    std::vector<Type2Node49X> nodes(nn);std::vector<Type2Ray49X> rays(nr);
    std::vector<Type2OpaqueEndpointWords49X> opaque(nr);std::vector<std::uint32_t> topology(nn+nr);
    for(std::size_t i=0;i<nr;++i){opaque[i].first=std::bit_cast<std::uint32_t>(main690.resource->rays[i].first.v[3]);
        opaque[i].second=std::bit_cast<std::uint32_t>(main690.resource->rays[i].second.v[3]);}
    Type2ResourceView49X authentic{};
    if(decodeType2Resource49X(podM.data(),podM.size(),opaque.data(),opaque.size(),nodes.data(),nodes.size(),
       rays.data(),rays.size(),{topology.data(),topology.size()},authentic)!=Type2ResourceStatus49X::Complete)
        return detail::fail(error,"actual podboxm native decoding rejected");
    if(authentic.root!=main690.resource->root||authentic.maxDepth!=main690.resource->maxDepth||
       std::memcmp(authentic.localMatrix.v,main690.resource->localMatrix.v,sizeof(BodyMatrix48G))||
       std::memcmp(nodes.data(),main690.resource->nodes,nodes.size()*sizeof(Type2Node49X))||
       std::memcmp(rays.data(),main690.resource->rays,rays.size()*sizeof(Type2Ray49X)))
        return detail::fail(error,"690 is not the actual decoded Pod01 resource");
    Type2BoundsInput50BY input{};input.resource=&authentic;input.preparedWorld=detail::identity();
    CollectionBoundsResult48S bounds{};
    if(prepareType2Bounds50BY(input,bounds)!=CollectionBoundsStatus48S::Complete)return detail::fail(error,"source2121C8 podboxm bounds rejected");
    float lift=0;if(!detail::lift(phy,lift,error))return false;
    if(std::bit_cast<std::uint32_t>(lift)!=std::bit_cast<std::uint32_t>(parameters.visual.verticalLimit7EC))
        return detail::fail(error,"canonical7EC differs from actual Repulsor authoring");
    SpringVector4_48N offset{{0,bounds.half.v[1],0,1}},first{},second{};
    // Source3C9B90..3C9C0C initializer, independently distinct from434650.
    constexpr float up434280[3]={0,1,0};const float fiveLift=detail::mul(5.f,lift);
    for(unsigned i=0;i<3;++i){first.v[i]=detail::add(parameters.bodies.main.centerOffset.v[i],offset.v[i]);
        const auto doubled=detail::mul(2.f,offset.v[i]);const auto down=detail::mul(fiveLift,up434280[i]);
        second.v[i]=detail::sub(detail::sub(first.v[i],doubled),down);
        if(!std::isfinite(first.v[i])||!std::isfinite(second.v[i]))return detail::fail(error,"source348 endpoint arithmetic rejected");}
    first.v[3]=second.v[3]=1.f;
    out.probeRay.first=detail::identityPoint(first);out.probeRay.second=detail::identityPoint(second);
    out.probeResource={};out.probeResource.localMatrix=detail::identity();
    out.probeResource.rays=&out.probeRay;out.probeResource.rayCount=1;out.probeResource.root=type2Leaf49X;
    assets::Collision48G decoded;std::vector<std::uint8_t> cache;
    if(!assets::decodeCollision48G(podF,decoded,decode)||!assets::encodeCollisionAsset48G(decoded,cache,error)||
       !assets::loadCollisionMesh48I(cache,out.podMesh,error)||!assets::decodePodShapeSource49W(podF,out.podSource,decode))
        return detail::fail(error,"actual podboxf native decoding rejected");
    if(std::memcmp(&out.podSource,&parameters.shape,sizeof(PodShapeSource49W)))return detail::fail(error,"canonical694 resource differs from actual podboxf");
    for(unsigned i=0;i<3;++i){auto& filter=out.filters[i];filter={};filter.identity=globals.identity[i];
        filter.flags31=globals.flags31[i];filter.flagsF8=static_cast<std::uint8_t>(globals.reservedF8[i]&0xc0u);
        out.links[i].resize(membershipCapacity);out.entries[i].resize(membershipCapacity);}
    //11B570 modes5,3,7 respectively; query bit1 and target bit2 are distinct.
    out.filters[0].flagsF8|=4u;out.filters[0].categoryA4=globals.podCategory3E0718|globals.extraPodCategory3B14B0;
    out.filters[1].flagsF8|=2u;out.filters[1].categoryA4=globals.podCategory3E0718;
    out.filters[1].categoryModeD4=0;out.filters[1].categoryMasks.push_back(globals.movementCategory3E0720);
    out.filters[2].flagsF8|=6u;out.filters[2].categoryModeD4=0;out.filters[2].categoryMasks.push_back(globals.probeCategory4342A0);
    out.probeQuery={};out.probeQuery.resource=&out.probeResource;out.probeQuery.filter=&out.filters[2];
    out.probeQuery.queryScene=main690.queryScene;out.probeQuery.collisionTypeTag=globals.collisionTypeTag;
    std::memcpy(out.probeQuery.copied40.data(),globals.sentinel3E02C0,16);
    std::memcpy(out.probeQuery.copied50.data(),globals.sentinel3E02C0,16);
    out.probeQuery.callbackEnabled=true;
    // Source11C710 fresh pair allocation constructs VF0 endpoints. Reset will
    //replace them using the real main1E0 point; no live-start coordinates here.
    out.movementEndpoints[0]={{0,0,0,1}};out.movementEndpoints[1]={{0,0,0,1}};
    out.movementMetadata[0]=out.movementMetadata[1]=0;out.movementTag=3;
    out.receipt={bytes.size(),podF.size(),podM.size(),bounds.half.v[1],lift,first,second};
    out.prepared=true;error.clear();return true;
}
}
