#pragma once
// Cold selected-original-pod ownership for the actual694/7A4 recovery join.
// No archive default, duplicate348, guest execution, or live resource loading.
#include "undertrack_reset50ew.h"
#include "type2_bounds50by.h"
#include "native_racer_collision50hw_fixture.h"
#include "../../src/game/type3_query48p.h"
#include "../../src/game/terrain_prepare48m.h"
#include "../../src/assets/pod_physics49n.h"
#include <array>
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

namespace undertrack_owners50ew_fixture {
using namespace rr::game;
namespace assets=rr::assets;
struct SourceGlobals {
    // Actual application category-registry bindings. The caller must bind the
    // source3E0718/3B14B0 values, not infer the extra category from an owner ID.
    // Source3E0720 is the existing main690 materialMask registry binding.
    std::uint32_t podCategory3E0718=0,extraPodCategory3B14B0=0;
    std::uint32_t movementCategory3E0720=0;
    std::uint8_t collisionTypeTag=0,reservedPodF8=0,podFlags31=0;
    std::uint32_t identity[2]{}; // actual694 and the EXISTING live7A4 identity.
    std::uint32_t terrainIdentity348=0;
};
struct ColdReceipt {
    std::size_t podArchiveBytes=0,podBoxFBytes=0,podBoxMBytes=0;
    float vehicle324=0,maxLiftHeight=0;
    SpringVector4_48N body1E0{},probe130{},probe140{};
    std::uint32_t sourcePodLoad=0x212238,sourceMovementConstructor=0x212444;
};
// All borrowed records are the existing execution-arena owners. In particular,
// movementSource must already consume these exact endpoint/metadata arrays;
// terrain348/resource/metadata/membership are the live terrain query's owners.
struct BorrowedQueries {
    Type2StaticQuery49X* main690=nullptr;
    LiveZoneMembership50EN* mainMembership=nullptr;
    LiveZoneStorage50EN mainStorage{};
    CollectionRef50BX mainRef=0;
    Type3QueryInput48P* movement7A4=nullptr;
    SpringVector4_48N* movementEndpoints=nullptr; // exactly two entries
    std::uint32_t* movementMetadata=nullptr; // exactly two entries
    const Type3QuerySource48P* movementSource=nullptr;
    TerrainPrepareState48M* terrain348=nullptr;
    const Type2ResourceView49X* terrainResource=nullptr;
    TerrainQuery48J* terrainMetadata=nullptr;
    LiveZoneMembership50EN* terrainMembership=nullptr;
    LiveZoneStorage50EN terrainStorage{};
    CollectionRef50BX terrainRef=0;
};
struct Owners {
    // Canonical type0 geometry remains parameters.shape / selected.mainTarget.
    // Only the previously absent694 filter and registry backing are owned here.
    CollisionFilter48J podFilter{};
    //11B450/11B45C source sentinel. Camera queries may mutate the SAME target
    //copies andF8; no camera-private replacement is authoritative.
    std::array<float,4> podCopied40{{0.f,0.f,0.f,1.f}},podCopied50{{0.f,0.f,0.f,1.f}};
    LiveZoneMembership50EN memberships[2]{}; //694,7A4; never secondary spring.
    std::vector<CollectionZoneLink50BX> links[2];
    std::vector<LiveZoneEntry50EN> entries[2];
    //11C710 constructs the actual type3 tag. This is only its collection bounds
    // cache (1207C8..120884), not another type3 ray/query/result owner.
    std::uint32_t movementTag=3;
    CollisionSphere48J movementSphere{};
    SourceGlobals globals{};
    ColdReceipt receipt{};
    const native_racer_collision50HW::Owner* selectedSource=nullptr;
    bool storagePrepared=false,sourcePrepared=false;
    Owners()=default;
    Owners(const Owners&)=delete;Owners& operator=(const Owners&)=delete;
    Owners(Owners&&)=delete;Owners& operator=(Owners&&)=delete;
    LiveZoneStorage50EN storage(unsigned i) noexcept {
        return i<2?LiveZoneStorage50EN{links[i].data(),entries[i].data(),links[i].size()}:LiveZoneStorage50EN{};
    }
    // Call before sealing the racer bank. Vector objects and immutable filters
    // are never copied. These are the exact mutable elements/lanes, disjoint
    // from the caller's existing main/secondary/terrain/movement registrations.
    template<class Bank> void registerMutableOwners(Bank& bank) {
        bank.owner(podFilter.flagsF8);bank.owner(podCopied40);bank.owner(podCopied50);
        bank.contents(podFilter.identityLists[0].identities);
        bank.owner(movementTag);bank.owner(movementSphere);
        bank.owner(receipt);bank.owner(selectedSource);bank.owner(sourcePrepared);
        for(unsigned i=0;i<2;++i){bank.owner(memberships[i]);
            bank.elements(links[i].data(),links[i].size());
            bank.elements(entries[i].data(),entries[i].size());}
    }
    UndertrackQueryOwners50EW binding(const BorrowedQueries& q,
        const std::array<CollectionRef50BX,2>& addedRefs) noexcept {
        UndertrackQueryOwners50EW out{};
        if(!storagePrepared||!sourcePrepared||!selectedSource||!selectedSource->prepared()||
           !q.main690||q.main690->resource!=&selectedSource->main()||!q.mainMembership||
           !q.mainStorage.links||!q.mainStorage.entries||!q.mainStorage.capacity||!q.mainRef||
           !q.movement7A4||q.movement7A4->filter.identity!=globals.identity[1]||
           q.movement7A4->pairCount!=1||!q.movementEndpoints||!q.movementMetadata||
           !q.movementSource||q.movementSource->endpoints!=q.movementEndpoints||
           q.movementSource->endpointCount!=2||q.movementSource->sceneMetadata!=q.movementMetadata||
           q.movementSource->sceneMetadataCount!=2||
           !q.terrain348||!q.terrainResource||!q.terrainMetadata||!q.terrainMembership||
           !q.terrainStorage.links||!q.terrainStorage.entries||!q.terrainStorage.capacity||
           !q.terrainRef||q.terrainResource->nodeCount||q.terrainResource->rayCount!=1||
           q.terrainResource->root!=type2Leaf49X||!q.terrainResource->rays||
           q.terrainMetadata->filter.identity==globals.identity[0]||
           q.terrainMetadata->filter.identity==globals.identity[1]||
           q.terrainMetadata->collisionTypeTag!=globals.collisionTypeTag)return out;
        const CollectionRef50BX refs[]={q.mainRef,addedRefs[0],addedRefs[1],q.terrainRef};
        LiveZoneMembership50EN* const members[]={q.mainMembership,&memberships[0],&memberships[1],q.terrainMembership};
        for(unsigned i=0;i<4;++i){if(!refs[i])return out;
            for(unsigned j=0;j<i;++j)if(refs[i]==refs[j]||members[i]==members[j])return out;}
        // Verify that the live348 query uses its same resource endpoint XYZ.
        // No source geometry or canonical140 is
        // rewritten to make a different pod pass this join.
        for(unsigned axis=0;axis<3;++axis){
            if(std::bit_cast<std::uint32_t>(q.terrainResource->rays[0].first.v[axis])!=
               std::bit_cast<std::uint32_t>(q.terrain348->segment.first.v[axis])||
               std::bit_cast<std::uint32_t>(q.terrainResource->rays[0].second.v[axis])!=
               std::bit_cast<std::uint32_t>(q.terrain348->segment.second.v[axis]))return out;
        }
        out.query[0]={q.mainMembership,q.mainStorage,q.mainRef,q.main690};
        out.query[1]={&memberships[0],storage(0),addedRefs[0]};
        out.query[2]={&memberships[1],storage(1),addedRefs[1],nullptr,
            q.movementEndpoints,q.movementMetadata,&movementTag,&movementSphere};
        auto& terrain=out.query[3];terrain.membership=q.terrainMembership;
        terrain.storage=q.terrainStorage;terrain.object=q.terrainRef;
        terrain.terrain=q.terrain348;terrain.terrainResource=q.terrainResource;
        return out;
    }
};
namespace detail {
inline bool fail(std::string& error,const char* message){error=message;return false;}
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
    return seen||fail(error,"selected PHY lacks Repulsor MaxLiftHeight");
}
inline float add(float a,float b) noexcept {volatile float v=a+b;return v;}
inline float sub(float a,float b) noexcept {volatile float v=a-b;return v;}
inline float mul(float a,float b) noexcept {volatile float v=a*b;return v;}
inline BodyMatrix48G identity() noexcept {BodyMatrix48G m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1.f;return m;}
inline bool oneBit(std::uint32_t x) noexcept {return x&&!(x&(x-1));}
}

// Cold arena storage only, before Bank::seal. No selected source has been bound
// yet. Each fresh bank slot later binds its own already-authenticated RES owner.
// This function may leave an unpublished object partly prepared on allocation
// failure; never resize an arena after any runtime reference has escaped.
inline bool prepareStorage(Owners& out,const SourceGlobals& globals,
    std::size_t membershipCapacity,std::string& error) {
    if(out.storagePrepared||out.sourcePrepared||!membershipCapacity||membershipCapacity>65536||
       !globals.identity[0]||!globals.identity[1]||!globals.terrainIdentity348||
       globals.identity[0]==globals.identity[1]||globals.identity[0]==globals.terrainIdentity348||
       globals.identity[1]==globals.terrainIdentity348||
       !detail::oneBit(globals.podCategory3E0718)||!detail::oneBit(globals.movementCategory3E0720)||
       globals.podCategory3E0718==globals.movementCategory3E0720)
        return detail::fail(error,"invalid fresh actual694/7A4 storage bindings");
    out.globals=globals;auto& f=out.podFilter;f={};f.identity=globals.identity[0];
    f.flags31=static_cast<std::uint8_t>((globals.podFlags31&0xf0u)|3u);f.flagsF8=static_cast<std::uint8_t>((globals.reservedPodF8&0xc0u)|4u);
    // Original11B570 mode5: target bit2 enabled, querying bit1 disabled.
    f.categoryA4=globals.podCategory3E0718|globals.extraPodCategory3B14B0;
    f.acceptMaskB8=0xffffffffu;f.categoryModeD4=1;
    f.identityLists[0].mode=f.identityLists[1].mode=1;
    //212984/212990/21299C append actual690,7A4,348 in source order.
    // Main identity is filled from the selected live owner before any query.
    f.identityLists[0].identities.resize(3);
    f.identityLists[0].identities[1]=globals.identity[1];
    f.identityLists[0].identities[2]=globals.terrainIdentity348;
    for(unsigned i=0;i<2;++i){out.links[i].resize(membershipCapacity);out.entries[i].resize(membershipCapacity);}
    out.storagePrepared=true;error.clear();return true;
}

// Selected PHY and collision owner must come from the caller's authenticated
// original roster. Its four raw COL members have ALREADY been decoded once by
// native_racer_collision50HW::Owner; compare exact owner identity and the full
// canonical type0 projection instead of re-reading a path or using the legacy
// Pod01-only decodePodShapeSource49W extent ceiling. No shape, body1E0, lift,
// graphics forest, terrain348, or live movement owner is changed here.
// Call once in each fresh bank slot, after its own handling/probe authoring and
// before any tick. A live reset never calls this function.
inline bool bindSelectedCold(Owners& out,const native_racer_collision50HW::Owner& selected,
    const Type2StaticQuery49X& main690,const Type2StaticQuery49X& secondarySpring,
    const VehicleFrameParameters49W& parameters,const std::vector<std::uint8_t>& phy,
    std::string& error) {
    if(!out.storagePrepared||out.sourcePrepared||std::fegetround()!=FE_TONEAREST||!selected.prepared()||
       selected.binding().archivePath.empty()||selected.binding().memberPrefix.empty()||
       main690.resource!=&selected.main()||secondarySpring.resource!=&selected.secondary()||
       !main690.filter||!secondarySpring.filter||!main690.filter->identity||!secondarySpring.filter->identity||
       !main690.queryScene.value||!secondarySpring.queryScene.value||
       main690.resource==secondarySpring.resource||main690.filter==secondarySpring.filter||
       main690.filter->identity==secondarySpring.filter->identity||
       main690.collisionTypeTag!=out.globals.collisionTypeTag||
       main690.materialMask!=out.globals.movementCategory3E0720)
        return detail::fail(error,"selected recovery source is not the actual main/secondary collision owner");
    for(const auto id:out.globals.identity)
        if(id==main690.filter->identity||id==secondarySpring.filter->identity)
            return detail::fail(error,"actual694/7A4 identity aliases a spring query");
    const auto& source=selected.mainTarget().source;
    if(std::memcmp(source.local40.v,parameters.shape.local40.v,sizeof source.local40.v)||
       std::memcmp(source.rootCenter.v,parameters.shape.rootCenter.v,sizeof source.rootCenter.v)||
       std::memcmp(source.rootHalf.v,parameters.shape.rootHalf.v,sizeof source.rootHalf.v))
        return detail::fail(error,"canonical694 shape differs from selected original podboxf");
    ColdReceipt receipt{};const auto& loaded=selected.receipt();
    receipt.podArchiveBytes=loaded.archiveBytes;receipt.podBoxMBytes=loaded.memberBytes[0];
    receipt.podBoxFBytes=loaded.memberBytes[2];
    if(!receipt.podArchiveBytes||!receipt.podBoxFBytes||!receipt.podBoxMBytes)
        return detail::fail(error,"selected original collision raw-member receipt is missing");
    if(!detail::lift(phy,receipt.maxLiftHeight,error))return false;
    if(!std::isfinite(receipt.maxLiftHeight)||
       std::bit_cast<std::uint32_t>(receipt.maxLiftHeight)!=std::bit_cast<std::uint32_t>(parameters.visual.verticalLimit7EC))
        return detail::fail(error,"canonical7EC differs from selected Repulsor authoring");
    Type2BoundsInput50BY input{};input.resource=&selected.main();input.preparedWorld=detail::identity();
    CollectionBoundsResult48S bounds{};
    if(prepareType2Bounds50BY(input,bounds)!=CollectionBoundsStatus48S::Complete)
        return detail::fail(error,"selected source2121C8 podboxm bounds rejected");
    receipt.vehicle324=bounds.half.v[1];
    const float offset[3]={0,receipt.vehicle324,0},up[3]={0,1,0};
    const float fiveLift=detail::mul(5.f,receipt.maxLiftHeight);
    for(unsigned i=0;i<3;++i){receipt.body1E0.v[i]=parameters.bodies.main.centerOffset.v[i];
        receipt.probe130.v[i]=detail::add(receipt.body1E0.v[i],offset[i]);
        receipt.probe140.v[i]=detail::sub(detail::sub(receipt.probe130.v[i],detail::mul(2.f,offset[i])),detail::mul(fiveLift,up[i]));
        if(!std::isfinite(receipt.body1E0.v[i])||!std::isfinite(receipt.probe130.v[i])||!std::isfinite(receipt.probe140.v[i])||
           std::bit_cast<std::uint32_t>(receipt.probe140.v[i])!=std::bit_cast<std::uint32_t>(parameters.bodies.main.probeLocal140[i]))
            return detail::fail(error,"canonical348 endpoint differs from selected body1E0/MaxLiftHeight");}
    receipt.body1E0.v[3]=receipt.probe130.v[3]=receipt.probe140.v[3]=1.f;
    out.podFilter.identityLists[0].identities[0]=main690.filter->identity;
    out.receipt=receipt;out.selectedSource=&selected;out.sourcePrepared=true;error.clear();return true;
}

inline bool prepare(Owners& out,const native_racer_collision50HW::Owner& selected,
    const Type2StaticQuery49X& main690,const Type2StaticQuery49X& secondarySpring,
    const VehicleFrameParameters49W& parameters,const std::vector<std::uint8_t>& phy,
    const SourceGlobals& globals,std::size_t membershipCapacity,std::string& error) {
    return prepareStorage(out,globals,membershipCapacity,error)&&
        bindSelectedCold(out,selected,main690,secondarySpring,parameters,phy,error);
}
}

// Cold completion of the two previously missing query registrations, after
// the actual selected rig's final grid and7F0 preparation. Existing690/364/348
// retain their own source owners. This is cold native owner assembly, never a
// live reset, phase spoof, frame advance or claim that a player reset ran.
namespace undertrack_owners50ew_fixture {
inline bool attachFreshQueries(Owners& owner,VehicleFrameState49W& frame,
    const VehicleFrameBindings49W& bindings,const VehicleFrameParameters49W& parameters,
    const BorrowedQueries& borrowed,const std::array<CollectionRef50BX,2>& refs,
    ZoneLifecycle50CL& life,LiveZoneWorld50EN& world,LiveZoneWorkspace50EN& workspace,
    CollectionRef50BX actualInitialC34) noexcept {
    if(frame.phase!=VehicleFramePhase49W::Ready||frame.fields.vehicle58!=1u||
       !frame.fields.nativeAiOwnersBound50IV||frame.bodies.main.frameFactor!=0.f||
       frame.shape.registrationE0||!actualInitialC34||actualInitialC34>life.zoneCount||
       !owner.sourcePrepared||owner.memberships[0].attached||owner.memberships[1].attached)return false;
    const auto queries=owner.binding(borrowed,refs);
    if(!queries.query[0].membership||!queries.query[0].membership->attached||
       !queries.query[3].membership||!queries.query[3].membership->attached)return false;
    const auto main=static_cast<std::int32_t>(bindings.lifecycle.main190);
    if(main<0||static_cast<std::uint32_t>(main)>=frame.sceneNodeCount||
       !(frame.nodes[main].sceneFlags190&1u))return false;
    const auto& matrix=frame.nodes[main].world100;
    SpringVector4_48N pair[2]{};std::uint32_t metadata[2]{};
    for(unsigned axis=0;axis<3;++axis){
        volatile float x=matrix.v[axis]*parameters.bodies.main.centerOffset.v[0];
        volatile float y=matrix.v[4+axis]*parameters.bodies.main.centerOffset.v[1];
        volatile float xy=x+y;
        volatile float z=matrix.v[8+axis]*parameters.bodies.main.centerOffset.v[2];
        volatile float xyz=xy+z;volatile float point=xyz+matrix.v[12+axis];
        if(!std::isfinite(point))return false;pair[0].v[axis]=pair[1].v[axis]=point;
    }
    pair[0].v[3]=pair[1].v[3]=1.f;
    LiveZoneBounds50EN movement{};movement.bounds.shapeTag=3;movement.bounds.pairCount=1;
    movement.bounds.endpoints=pair;movement.bounds.endpointCount=2;
    movement.bounds.endpointScenes=metadata;movement.bounds.endpointSceneCount=2;
    if(attachLiveZoneMembership50EN(life,world,owner.memberships[1],owner.storage(1),refs[1],
        actualInitialC34,ZoneMethod50CL::Second,0.f,movement,workspace).status!=LiveZoneStatus50EN::Complete)return false;
    std::memcpy(borrowed.movementEndpoints,pair,sizeof pair);
    std::memcpy(borrowed.movementMetadata,metadata,sizeof metadata);owner.movementTag=3;
    for(unsigned axis=0;axis<3;++axis)owner.movementSphere.center.v[axis]=owner.memberships[1].bounds.cachedCenter.v[axis];
    owner.movementSphere.radius=owner.memberships[1].bounds.cachedRadius;
    PodShapeState49W prepared=frame.shape;
    if(!transformPodShape49W(parameters.shape,matrix,prepared))return false;
    LiveZoneBounds50EN pod{};pod.bounds.shapeTag=0;pod.bounds.shapeWorld=prepared.composed00;
    pod.bounds.cachedCenter=prepared.center60;pod.bounds.cachedRadius=prepared.radius80;pod.type0Resource=&parameters.shape;
    if(attachLiveZoneMembership50EN(life,world,owner.memberships[0],owner.storage(0),refs[0],
        actualInitialC34,ZoneMethod50CL::Second,0.f,pod,workspace).status!=LiveZoneStatus50EN::Complete)return false;
    prepared.registrationE0=refs[0];frame.shape=prepared;return true;
}
}
