#include "live_zone_membership50en.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game { namespace {
using Status=LiveZoneStatus50EN;
struct Span {std::uintptr_t first=0,last=0;};
template<class T> bool span(const T* p,std::size_t n,Span& out) noexcept {
    out={};if(!n)return true;
    const auto first=reinterpret_cast<std::uintptr_t>(p);
    if(!p||first%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto bytes=n*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-first)return false;
    out={first,first+bytes};return true;
}
bool overlap(Span a,Span b) noexcept {return a.first<b.last&&b.first<a.last;}
struct Guard {
    Span writes[20]{};std::size_t count=0;
    template<class T> bool write(T* p,std::size_t n) noexcept {
        Span s;if(count==20||!span(p,n,s))return false;
        for(std::size_t i=0;i<count;++i)if(overlap(s,writes[i]))return false;
        writes[count++]=s;return true;
    }
    template<class T> bool read(const T* p,std::size_t n) const noexcept {
        Span s;if(!span(p,n,s))return false;
        for(std::size_t i=0;i<count;++i)if(overlap(s,writes[i]))return false;
        return true;
    }
};
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
bool finite(const Vector3_48C& v) noexcept {
    return std::isfinite(v.v[0])&&std::isfinite(v.v[1])&&std::isfinite(v.v[2]);
}
Vector3_48C center(const CollectionBoundsResult48S& b) noexcept {
    return {{b.center.v[0],b.center.v[1],b.center.v[2]}};
}
bool sameRecord(const ZoneRegistration50CJ& a,const ZoneRegistration50CJ& b) noexcept {
    return a.owner==b.owner&&a.slot==b.slot&&a.encodedObject==b.encodedObject;
}
bool emptyRecord(const ZoneRegistration50CJ& r) noexcept {
    return !r.owner&&!r.slot&&!r.encodedObject;
}
bool registryIndex(const ZoneLifecycle50CL& life,const SearchStorage50BS* owner,
    std::size_t& index) noexcept {
    const auto first=reinterpret_cast<std::uintptr_t>(life.storage.registries);
    const auto p=reinterpret_cast<std::uintptr_t>(owner);
    if(!owner||p<first||(p-first)%sizeof(SearchStorage50BS))return false;
    index=(p-first)/sizeof(SearchStorage50BS);return index<life.zoneCount*2u;
}
std::size_t methodIndex(CollectionRef50BX zone,ZoneMethod50CL method) noexcept {
    return (static_cast<std::size_t>(zone)-1u)*2u+static_cast<unsigned>(method);
}
bool published(const ZoneLifecycle50CL& life,std::size_t i) noexcept {
    const auto& r=life.storage.registries[i];const auto& m=life.storage.views.methods[i];
    const auto& list=m.searchList;const auto& z=life.storage.views.zones[i/2u];
    if(r.perBlock!=life.perBlock||r.blockCount>1u||r.lastUsed>life.perBlock||
       (!r.blockCount&&r.lastUsed!=life.perBlock)||
       m.method!=CollectionTree50BX::Method::SearchList||
       z.firstMethod!=i/2u*2u+1u||z.secondMethod!=i/2u*2u+2u||
       list.blockCount!=static_cast<std::int32_t>(r.blockCount)||
       list.perBlock30!=static_cast<std::int32_t>(r.perBlock)||
       list.lastUsed38!=static_cast<std::int32_t>(r.lastUsed))return false;
    if(!r.blockCount)return !list.blocks&&!list.blockCapacity;
    const auto* blocks=life.storage.views.blocks+i*2u;
    return list.blocks==blocks&&list.blockCapacity==2u&&
        blocks[0].words==r.words&&blocks[0].capacity==r.perBlock&&
        !blocks[1].words&&!blocks[1].capacity;
}
void refresh(ZoneLifecycle50CL& life,std::size_t i) noexcept {
    const auto& r=life.storage.registries[i];CollectionTree50BX m{};
    m.method=CollectionTree50BX::Method::SearchList;
    m.searchList={nullptr,0,static_cast<std::int32_t>(r.blockCount),
        static_cast<std::int32_t>(r.perBlock),static_cast<std::int32_t>(r.lastUsed)};
    if(r.blockCount){auto* b=life.storage.views.blocks+i*2u;
        b[0]={r.words,r.perBlock};b[1]={};m.searchList.blocks=b;m.searchList.blockCapacity=2;}
    life.storage.views.methods[i]=m;
}
bool layout(ZoneLifecycle50CL& life,LiveZoneWorld50EN& world,
    LiveZoneMembership50EN& state,const LiveZoneStorage50EN& storage,
    LiveZoneWorkspace50EN& work,const LiveZoneBounds50EN* input,
    const LiveZoneStorage50EN* externalDescription=nullptr) noexcept {
    if(!life.perBlock||life.perBlock>1024u||!life.zoneCount||
       life.zoneCount>std::numeric_limits<CollectionRef50BX>::max()/2u||
       life.zoneCount>std::numeric_limits<std::size_t>::max()/4u||
       life.objectCount>(std::numeric_limits<CollectionRef50BX>::max()>>2u)||
       world.zoneCount!=life.zoneCount||
       life.zoneCount*2u>life.storage.registryCapacity||
       life.zoneCount*2u>life.storage.views.methodCapacity||
       life.zoneCount>life.storage.views.zoneCapacity||
       life.zoneCount*4u>life.storage.views.blockCapacity||
       life.objectCount>life.storage.registrationCapacity||
       !storage.capacity||storage.capacity>std::numeric_limits<CollectionRef50BX>::max()||
       work.registryCapacity<life.zoneCount*2u||work.membershipCapacity<storage.capacity||
       work.portalLinkCapacity<world.portalLinkCount||
       world.portalLinkCount>std::numeric_limits<CollectionRef50BX>::max()||
       world.portalCount>std::numeric_limits<CollectionRef50BX>::max())return false;
    Guard g;
    if(!g.write(&life,1)||!g.write(&state,1)||!g.write(&work,1)||
       !g.write(life.storage.registries,life.storage.registryCapacity)||
       !g.write(life.storage.registrations,life.storage.registrationCapacity)||
       !g.write(life.storage.views.methods,life.storage.views.methodCapacity)||
       !g.write(life.storage.views.zones,life.storage.views.zoneCapacity)||
       !g.write(life.storage.views.blocks,life.storage.views.blockCapacity)||
       !g.write(storage.links,storage.capacity)||!g.write(storage.entries,storage.capacity)||
       !g.write(work.registries,work.registryCapacity)||
       !g.write(work.links,work.membershipCapacity)||!g.write(work.entries,work.membershipCapacity)||
       !g.write(world.portalLinks,world.portalLinkCount)||
       !g.write(work.portalLinks,work.portalLinkCapacity)||
       !g.read(&world,1)||!g.read(world.portalHeads,world.zoneCount)||
       !g.read(world.portals,world.portalCount)||
       !g.read(externalDescription,externalDescription?1u:0u))return false;
    if(!input)return true;
    const auto& b=input->bounds;
    if(!g.read(input,1)||!g.read(b.endpoints,b.endpointCount)||
       !g.read(b.endpointScenes,b.endpointSceneCount)||!g.read(b.scenes,b.sceneCount)||
       !g.read(input->type0Resource,input->type0Resource?1u:0u)||
       !g.read(input->type2Resource,input->type2Resource?1u:0u))return false;
    return !input->type2Resource||
        (g.read(input->type2Resource->nodes,input->type2Resource->nodeCount)&&
         g.read(input->type2Resource->rays,input->type2Resource->rayCount));
}
bool worldTopology(const LiveZoneWorld50EN& world) noexcept {
    std::size_t total=0;
    for(std::size_t z=0;z<world.zoneCount;++z){
        auto edge=world.portalHeads[z];std::size_t visits=0;
        while(edge){
            if(edge>world.portalLinkCount||++visits>world.portalLinkCount)return false;
            const auto& e=world.portalLinks[edge-1u];
            if(!e.zone||e.zone>world.zoneCount||e.zone==z+1u||
               !e.portal||e.portal>world.portalCount||
               !std::isfinite(e.minimumDistanceC)||!std::isfinite(e.maximumDistance10))return false;
            edge=e.next;++total;if(total>world.portalLinkCount)return false;
        }
    }
    if(total!=world.portalLinkCount)return false;
    // Distinct incident lists must not share a physical mutable portal link.
    for(std::size_t i=0;i<world.portalLinkCount;++i){std::size_t occurrences=0;
        for(std::size_t z=0;z<world.zoneCount;++z)
            for(auto e=world.portalHeads[z];e;e=world.portalLinks[e-1u].next)
                occurrences+=e==i+1u;
        if(occurrences!=1u)return false;
    }
    return true;
}
bool liveTopology(const ZoneLifecycle50CL& life,const LiveZoneWorld50EN& world,
    const LiveZoneMembership50EN& state) noexcept {
    if(!state.attached||!state.object||state.object>life.objectCount||
       (state.method!=ZoneMethod50CL::First&&state.method!=ZoneMethod50CL::Second)||
       !finite(center(state.bounds)))return false;
    const auto& s=state.storage;
    if(!sameRecord(s.entries[0].registration,life.storage.registrations[state.object-1u]))return false;
    std::size_t count=0;
    for(CollectionRef50BX ref=1;ref;ref=s.links[ref-1u].next){
        if(ref>s.capacity||++count>s.capacity)return false;
        const auto& link=s.links[ref-1u];const auto& entry=s.entries[ref-1u];
        if(!entry.used||!link.zone||link.zone>world.zoneCount||
           (ref==1u?entry.portal!=0u:(!entry.portal||entry.portal>world.portalCount)))return false;
        std::size_t index=0;
        if(!registryIndex(life,entry.registration.owner,index)||
           index!=methodIndex(link.zone,state.method)||
           entry.registration.encodedObject!=(state.object<<2u)||
           !entry.registration.slot||entry.registration.slot>1024u||
           entry.registration.owner->words[entry.registration.slot-1u]!=(state.object<<2u))return false;
    }
    std::size_t used=0;
    for(std::size_t i=0;i<s.capacity;++i){
        if(s.entries[i].used)++used;
        else if(!emptyRecord(s.entries[i].registration)||s.links[i].next||s.links[i].zone)return false;
    }
    return count==used;
}
bool prepareBounds(const LiveZoneBounds50EN& input,LiveZoneMembership50EN& staged,
    LiveZoneResult50EN& result) noexcept {
    if(input.bounds.shapeTag==0){
        Type0BoundsInput50EW b{};b.resource=input.type0Resource;b.composed00=input.bounds.shapeWorld;
        std::memcpy(b.cachedCenterWords,input.bounds.cachedCenter.v,sizeof b.cachedCenterWords);
        std::memcpy(&b.cachedRadiusWord,&input.bounds.cachedRadius,sizeof b.cachedRadiusWord);
        result.boundsStatus=prepareType0Bounds50EW(b,staged.bounds);
    }else if(input.bounds.shapeTag==2){
        Type2BoundsInput50BY b{};b.resource=input.type2Resource;b.preparedWorld=input.bounds.shapeWorld;
        std::memcpy(b.cachedCenterWords,input.bounds.cachedCenter.v,sizeof b.cachedCenterWords);
        std::memcpy(&b.cachedRadiusWord,&input.bounds.cachedRadius,sizeof b.cachedRadiusWord);
        result.boundsStatus=prepareType2Bounds50BY(b,staged.bounds);
    }else result.boundsStatus=prepareCollectionBounds48S(input.bounds,staged.bounds);
    if(result.boundsStatus!=CollectionBoundsStatus48S::Complete){result.status=Status::BoundsRejected;return false;}
    if(staged.revision==std::numeric_limits<std::uint64_t>::max()){
        result.status=Status::TraversalBoundary;return false;}
    staged.lastShapeTag=input.bounds.shapeTag;
    staged.lastShapeWorld=input.bounds.shapeWorld;
    staged.lastType2Resource=input.bounds.shapeTag==2?input.type2Resource:nullptr;
    ++staged.revision;
    return true;
}
struct Transaction {
    ZoneLifecycle50CL& life;LiveZoneWorld50EN& world;LiveZoneWorkspace50EN& work;
    LiveZoneMembership50EN staged{};LiveZoneResult50EN result{};

    bool begin(const LiveZoneMembership50EN& source,const LiveZoneStorage50EN& storage,
        bool fresh) noexcept {
        staged=source;staged.storage=storage;
        for(std::size_t i=0;i<life.zoneCount*2u;++i){
            if(!published(life,i)){result.status=Status::InvalidTopology;return false;}
            std::uint32_t selected=0;
            const auto status=selectSearchSlot50BS(life.storage.registries[i],0,selected);
            if(status!=SearchStorageStatus50BS::Complete&&status!=SearchStorageStatus50BS::GrowthBoundary){
                result.status=Status::RegistryRejected;result.registryStatus=status;return false;}
            work.registries[i]=life.storage.registries[i];
        }
        for(std::size_t i=0;i<world.portalLinkCount;++i)work.portalLinks[i]=world.portalLinks[i];
        for(std::size_t i=0;i<storage.capacity;++i){
            work.links[i]=fresh?CollectionZoneLink50BX{}:storage.links[i];
            work.entries[i]=fresh?LiveZoneEntry50EN{}:storage.entries[i];
            auto& record=work.entries[i].registration;
            if(record.owner){std::size_t index=0;
                if(!registryIndex(life,record.owner,index)){result.status=Status::InvalidTopology;return false;}
                record.owner=work.registries+index;
            }
        }
        result.status=Status::Complete;return true;
    }
    bool move(std::size_t index,CollectionRef50BX zone) noexcept {
        auto* owner=zone?work.registries+methodIndex(zone,staged.method):nullptr;
        result.registryStatus=movePreparedRegistration50CJ(work.entries[index].registration,
            owner,staged.object<<2u,work.transfer);
        if(result.registryStatus!=SearchStorageStatus50BS::Complete){result.status=Status::RegistryRejected;return false;}
        return true;
    }
    bool removeSecondary(CollectionRef50BX ref) noexcept {
        if(!move(ref-1u,0))return false;
        work.links[ref-1u]={};work.entries[ref-1u]={};return true;
    }
    bool clearSecondaries() noexcept {
        auto ref=work.links[0].next;
        while(ref){const auto next=work.links[ref-1u].next;
            if(!removeSecondary(ref))return false;ref=next;}
        work.links[0].next=0;return true;
    }
    bool overlapPortal(CollectionRef50BX edge,float radius,bool& hit,float& distance) noexcept {
        const auto& p=work.portalLinks[edge-1u];
        result.geometryStatus=overlapPortal231150_50EO(world.portals[p.portal-1u],
            center(staged.bounds),radius,hit,distance);
        if(result.geometryStatus!=PortalGeometryStatus50EO::Complete){result.status=Status::GeometryRejected;return false;}
        return true;
    }
    bool insertSecondary(CollectionRef50BX edge,float distance) noexcept {
        auto& p=work.portalLinks[edge-1u];std::size_t slot=1;
        while(slot<staged.storage.capacity&&work.entries[slot].used)++slot;
        if(slot==staged.storage.capacity){result.status=Status::InsufficientCapacity;return false;}
        if(!move(slot,p.zone))return false;
        work.entries[slot].used=true;work.entries[slot].portal=p.portal;
        work.entries[slot].distance10=distance;
        work.links[slot]={work.links[0].next,p.zone};
        work.links[0].next=static_cast<CollectionRef50BX>(slot+1u);
        return true;
    }
    //280BF0: source-order portal scan, head-insert overlapping memberships.
    bool addOverlaps(float radius) noexcept {
        for(auto edge=world.portalHeads[work.links[0].zone-1u];edge;edge=work.portalLinks[edge-1u].next){
            bool hit=false;float distance=0;if(!overlapPortal(edge,radius,hit,distance))return false;
            if(hit){auto& p=work.portalLinks[edge-1u];p.minimumDistanceC=p.maximumDistance10=distance;
                if(!insertSecondary(edge,distance))return false;}
        }
        return true;
    }
    //281020: retain source head-update behavior; do not turn it into a set diff.
    bool updateOverlaps(const Vector3_48C& oldCenter) noexcept {
        const auto now=center(staged.bounds);float delta[3]{};
        for(unsigned i=0;i<3;++i)delta[i]=sub(now.v[i],oldCenter.v[i]);
        const float square=add(add(mul(delta[0],delta[0]),mul(delta[1],delta[1])),mul(delta[2],delta[2]));
        if(!std::isfinite(square)){result.status=Status::NonFinite;return false;}
        volatile float distanceMoved=static_cast<float>(std::sqrt(static_cast<double>(square)));
        for(auto edge=world.portalHeads[work.links[0].zone-1u];edge;edge=work.portalLinks[edge-1u].next){
            auto& p=work.portalLinks[edge-1u];
            p.minimumDistanceC=sub(p.minimumDistanceC,distanceMoved);
            p.maximumDistance10=add(p.maximumDistance10,distanceMoved);
            if(!std::isfinite(p.minimumDistanceC)||!std::isfinite(p.maximumDistance10)){
                result.status=Status::NonFinite;return false;}
            auto found=work.links[0].next;
            while(found&&work.links[found-1u].zone!=p.zone)found=work.links[found-1u].next;
            bool hit=false;float distance=0;
            if(!overlapPortal(edge,staged.bounds.cachedRadius,hit,distance))return false;
            if(hit){
                //281144 loads the HEAD, not the search result in s2. SearchList
                // bounds selection retains its existing handle;27E9C0 is no-op.
                const auto head=work.links[0].next;
                if(head){if(!move(head-1u,work.links[head-1u].zone))return false;}
                else {p.minimumDistanceC=p.maximumDistance10=distance;
                    if(!insertSecondary(edge,distance))return false;}
            }else if(found){
                //281320 writes head=found.next. Outside a head removal that
                // would strand other live records. Keep that domain explicit.
                if(found!=work.links[0].next){result.status=Status::UnsupportedLinkRemoval;return false;}
                p.minimumDistanceC=0;p.maximumDistance10=std::numeric_limits<float>::max();
                const auto next=work.links[found-1u].next;
                if(!removeSecondary(found))return false;work.links[0].next=next;
            }
        }
        return true;
    }
    void publish(LiveZoneMembership50EN& destination) noexcept {
        for(std::size_t i=0;i<life.zoneCount*2u;++i){life.storage.registries[i]=work.registries[i];refresh(life,i);}
        for(std::size_t i=0;i<world.portalLinkCount;++i)world.portalLinks[i]=work.portalLinks[i];
        for(std::size_t i=0;i<staged.storage.capacity;++i){
            auto entry=work.entries[i];if(entry.registration.owner){
                const auto index=static_cast<std::size_t>(entry.registration.owner-work.registries);
                entry.registration.owner=life.storage.registries+index;
            }
            staged.storage.entries[i]=entry;staged.storage.links[i]=work.links[i];
        }
        life.storage.registrations[staged.object-1u]=staged.storage.entries[0].registration;
        destination=staged;result.status=Status::Complete;
    }
};
} // namespace

LiveZoneStatus50EN prepareLiveZoneWorld50EN(const LiveZonePortalInput50EN* input,
    std::size_t count,std::size_t zones,const LiveZoneWorldStorage50EN& storage,
    LiveZoneWorld50EN& output) noexcept {
    if(!zones||zones>std::numeric_limits<CollectionRef50BX>::max()||
       count>std::numeric_limits<CollectionRef50BX>::max()/2u||
       count>std::numeric_limits<std::size_t>::max()/2u||
       zones>storage.zoneCapacity||count>storage.portalCapacity||
       count*2u>storage.portalLinkCapacity)return Status::InvalidInput;
    Guard guard;
    if(!guard.write(&output,1)||!guard.write(storage.portalHeads,storage.zoneCapacity)||
       !guard.write(storage.portalLinks,storage.portalLinkCapacity)||
       !guard.write(storage.portals,storage.portalCapacity)||
       !guard.read(input,count)||!guard.read(&storage,1))return Status::InvalidInput;
    for(std::size_t i=0;i<count;++i){const auto& p=input[i];PortalGeometry50EO prepared{};
        if(!p.zoneA||p.zoneA>zones||!p.zoneB||p.zoneB>zones||p.zoneA==p.zoneB)return Status::InvalidTopology;
        if(preparePortalGeometry50EO(p.vertices,prepared)!=PortalGeometryStatus50EO::Complete)return Status::GeometryRejected;
    }
    for(std::size_t i=0;i<zones;++i)storage.portalHeads[i]=0;
    for(std::size_t i=0;i<count;++i){
        // Preflight above makes these deterministic numeric calls infallible.
        preparePortalGeometry50EO(input[i].vertices,storage.portals[i]);
        const CollectionRef50BX a=input[i].zoneA,b=input[i].zoneB;
        const auto first=static_cast<CollectionRef50BX>(i*2u+1u),second=first+1u;
        const auto portal=static_cast<CollectionRef50BX>(i+1u);
        storage.portalLinks[first-1u]={storage.portalHeads[a-1u],b,portal,0,std::numeric_limits<float>::max()};
        storage.portalHeads[a-1u]=first;
        storage.portalLinks[second-1u]={storage.portalHeads[b-1u],a,portal,0,std::numeric_limits<float>::max()};
        storage.portalHeads[b-1u]=second;
    }
    output={storage.portalHeads,zones,storage.portalLinks,count*2u,storage.portals,count};
    return Status::Complete;
}

LiveZoneResult50EN attachLiveZoneMembership50EN(ZoneLifecycle50CL& life,
    LiveZoneWorld50EN& world,LiveZoneMembership50EN& state,const LiveZoneStorage50EN& storage,
    CollectionRef50BX object,CollectionRef50BX initialZone,ZoneMethod50CL method,
    float initialOverlapRadius,const LiveZoneBounds50EN& bounds,LiveZoneWorkspace50EN& work) noexcept {
    LiveZoneResult50EN rejected{};
    if(state.attached||state.object||!object||object>life.objectCount||
       !initialZone||initialZone>life.zoneCount||!std::isfinite(initialOverlapRadius)||initialOverlapRadius<0||
       (method!=ZoneMethod50CL::First&&method!=ZoneMethod50CL::Second)||
       !layout(life,world,state,storage,work,&bounds,&storage))return rejected;
    if(!worldTopology(world)||!emptyRecord(life.storage.registrations[object-1u])){
        rejected.status=Status::InvalidTopology;return rejected;}
    Transaction transaction{life,world,work};
    if(!transaction.begin(state,storage,true))return transaction.result;
    auto& staged=transaction.staged;staged.object=object;staged.method=method;staged.attached=true;
    if(!prepareBounds(bounds,staged,transaction.result))return transaction.result;
    work.links[0]={0,initialZone};work.entries[0].used=true;
    if(!transaction.move(0,initialZone)||!transaction.addOverlaps(initialOverlapRadius))return transaction.result;
    transaction.publish(state);return transaction.result;
}

LiveZoneResult50EN updateLiveZoneMembership50EN(ZoneLifecycle50CL& life,
    LiveZoneWorld50EN& world,LiveZoneMembership50EN& state,const LiveZoneBounds50EN& bounds,
    LiveZoneWorkspace50EN& work) noexcept {
    LiveZoneResult50EN rejected{};
    if(!state.attached){rejected.status=Status::Complete;return rejected;}
    if(!layout(life,world,state,state.storage,work,&bounds))return rejected;
    if(!worldTopology(world)||!liveTopology(life,world,state)){
        rejected.status=Status::InvalidTopology;return rejected;}
    Transaction transaction{life,world,work};
    if(!transaction.begin(state,state.storage,false))return transaction.result;
    const auto oldCenter=center(state.bounds);
    if(!prepareBounds(bounds,transaction.staged,transaction.result))return transaction.result;
    const auto newCenter=center(transaction.staged.bounds);
    auto primary=work.links[0].zone;CollectionRef50BX previous=0;
    auto edge=world.portalHeads[primary-1u];
    if(!edge){ //280D94 ->281380: no portal maintenance on this path.
        if(!transaction.move(0,primary))return transaction.result;
    }else {
        if(world.portalLinkCount==std::numeric_limits<std::size_t>::max()||
           world.zoneCount==std::numeric_limits<std::size_t>::max()||
           world.portalLinkCount+1u>std::numeric_limits<std::size_t>::max()/(world.zoneCount+1u)){
            transaction.result.status=Status::TraversalBoundary;return transaction.result;}
        const auto limit=(world.portalLinkCount+1u)*(world.zoneCount+1u);std::size_t visits=0;
        while(edge){
            if(++visits>limit){transaction.result.status=Status::TraversalBoundary;return transaction.result;}
            const auto& p=work.portalLinks[edge-1u];bool crossed=false;
            if(p.zone!=previous){
                transaction.result.geometryStatus=crossPortal230BD0_50EO(world.portals[p.portal-1u],oldCenter,newCenter,crossed);
                if(transaction.result.geometryStatus!=PortalGeometryStatus50EO::Complete){
                    transaction.result.status=Status::GeometryRejected;return transaction.result;}
            }
            if(crossed){previous=primary;primary=p.zone;edge=world.portalHeads[primary-1u];++transaction.result.crossedPortals;}
            else edge=p.next;
        }
        if(primary!=work.links[0].zone){
            //280E04 removes primary before walking old secondary links.
            if(!transaction.move(0,0)||!transaction.clearSecondaries())return transaction.result;
            work.links[0].zone=primary;
            if(!transaction.move(0,primary)||!transaction.addOverlaps(transaction.staged.bounds.cachedRadius))return transaction.result;
        }else {
            if(!transaction.move(0,primary)||!transaction.updateOverlaps(oldCenter))return transaction.result;
        }
    }
    transaction.publish(state);return transaction.result;
}

LiveZoneResult50EN removeLiveZoneMembership50EN(ZoneLifecycle50CL& life,
    LiveZoneWorld50EN& world,LiveZoneMembership50EN& state,LiveZoneWorkspace50EN& work) noexcept {
    LiveZoneResult50EN rejected{};
    if(!state.attached){rejected.status=Status::Complete;return rejected;}
    if(!layout(life,world,state,state.storage,work,nullptr))return rejected;
    if(!worldTopology(world)||!liveTopology(life,world,state)){
        rejected.status=Status::InvalidTopology;return rejected;}
    Transaction transaction{life,world,work};
    if(!transaction.begin(state,state.storage,false))return transaction.result;
    if(!transaction.move(0,0)||!transaction.clearSecondaries())return transaction.result;
    work.links[0]={};work.entries[0]={};transaction.staged.attached=false;
    transaction.publish(state);
    state.object=0; //1202C0: clear E0 only after all live removals publish.
    return transaction.result;
}
} // namespace rr::game
