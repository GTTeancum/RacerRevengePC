#include "original_camera_scene_owner.h"
#include "original_camera_observer_assertions50kg.h"
#include <cmath>
#include <cstring>
#include <limits>

//2809C0/280D40/281490 transaction core retains the reviewed50EN source-order
//SearchList/portal primitive composition. Camera points bypass collision bounds.
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
    Span writes[64]{};std::size_t count=0;
    template<class T> bool write(T* p,std::size_t n) noexcept {
        Span s;if(count==64||!span(p,n,s))return false;
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
    LiveZoneWorkspace50EN& work,const SpringVector4_48N* point,
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
    return g.read(point,point?1u:0u);

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
bool preparePoint(const SpringVector4_48N& point,LiveZoneMembership50EN& staged,
    LiveZoneResult50EN& result) noexcept {
    if(!std::isfinite(point.v[0])||!std::isfinite(point.v[1])||!std::isfinite(point.v[2])){
        result.status=Status::NonFinite;return false;}
    if(staged.revision==std::numeric_limits<std::uint64_t>::max()){
        result.status=Status::TraversalBoundary;return false;}
    //114770/114C44 pass point180 directly, without1202E0. Source3E0060
    //is(0,0,0,1), and radius argument is exact positive0.
    staged.bounds.center=point;staged.bounds.cachedCenter=point;
    staged.bounds.half={{0,0,0}};staged.bounds.cachedRadius=0;
    ++staged.revision;return true;
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
bool ownerLayout(ResetVisibilityWorld50EY& space,OriginalCameraSceneOwner& owner,
    LiveZoneWorkspace50EN& work,const SpringVector4_48N* point=nullptr,
    const SpringVector4_48N* saved=nullptr,
    const OriginalCameraSceneSpan* extraWrites=nullptr,std::size_t writeCount=0,
    const OriginalCameraSceneSpan* extraReads=nullptr,std::size_t readCount=0) noexcept {
    const auto& life=space.lifecycle;const auto& world=space.portals;
    if(!owner.sessionGeneration||!owner.sceneKey||!owner.object30||
        owner.object30>life.objectCount||!owner.storage.capacity)return false;
    Guard g;
    if(!g.write(&space,1)||!g.write(&owner,1)||!g.write(&work,1)||
       !g.write(life.storage.registries,life.storage.registryCapacity)||
       !g.write(life.storage.registrations,life.storage.registrationCapacity)||
       !g.write(life.storage.views.methods,life.storage.views.methodCapacity)||
       !g.write(life.storage.views.zones,life.storage.views.zoneCapacity)||
       !g.write(life.storage.views.blocks,life.storage.views.blockCapacity)||
       !g.write(owner.storage.links,owner.storage.capacity)||
       !g.write(owner.storage.entries,owner.storage.capacity)||
       !g.write(work.registries,work.registryCapacity)||
       !g.write(work.links,work.membershipCapacity)||!g.write(work.entries,work.membershipCapacity)||
       !g.write(world.portalLinks,world.portalLinkCount)||
       !g.write(work.portalLinks,work.portalLinkCapacity))return false;
    if((writeCount&&!extraWrites)||(readCount&&!extraReads))return false;
    for(std::size_t i=0;i<writeCount;++i)
        if(!g.write(static_cast<const unsigned char*>(extraWrites[i].data),extraWrites[i].bytes))return false;
    if(!g.read(world.portalHeads,world.zoneCount)||!g.read(world.portals,world.portalCount)||
       !g.read(point,point?1u:0u)||!g.read(saved,saved?1u:0u))return false;
    for(std::size_t i=0;i<readCount;++i)
        if(!g.read(static_cast<const unsigned char*>(extraReads[i].data),extraReads[i].bytes))return false;
    if(!layout(space.lifecycle,space.portals,owner.registration34,owner.storage,work,point))return false;
    const auto& m=owner.registration34;
    if(!m.attached)return !m.object&&emptyRecord(life.storage.registrations[owner.object30-1u]);
    return owner.allocationSerial&&m.object==owner.object30&&m.method==ZoneMethod50CL::Second&&
        m.storage.links==owner.storage.links&&m.storage.entries==owner.storage.entries&&
        m.storage.capacity==owner.storage.capacity&&owner.parent38==m.storage.links[0].zone;
}
} // namespace

bool admitOriginalCameraSceneStorage(ResetVisibilityWorld50EY& space,OriginalCameraSceneOwner& owner,
    LiveZoneWorkspace50EN& work,const OriginalCameraSceneSpan* writes,std::size_t writeCount,
    const OriginalCameraSceneSpan* reads,std::size_t readCount) noexcept {
    return ownerLayout(space,owner,work,nullptr,nullptr,writes,writeCount,reads,readCount);
}
bool bindFreshOriginalCameraScene(OriginalCameraSceneOwner& out,
    std::uint64_t session,std::uint64_t scene,CollectionRef50BX object,
    const LiveZoneStorage50EN& storage) noexcept {
    if(out.sessionGeneration||out.sceneKey||out.object30||out.registration34.attached||
       !session||!scene||!object||object>(std::numeric_limits<CollectionRef50BX>::max()>>2u)||
       !storage.capacity||!storage.links||!storage.entries)return false;
    OriginalCameraSceneOwner next;next.sessionGeneration=session;next.sceneKey=scene;
    next.object30=object;next.storage=storage;
    next.searchableType32=0xffu; //114428
    next.searchableFlags33Low3=0; //114448/45C/470, all three cleared.
    next.searchableConstructed1143F0=true;
    out=next;return true;
}
LiveZoneResult50EN attachOriginalCameraScene114770(ResetVisibilityWorld50EY& space,
    OriginalCameraSceneOwner& owner,CollectionRef50BX zone,
    const SpringVector4_48N& position,LiveZoneWorkspace50EN& work) noexcept {
    LiveZoneResult50EN rejected;
    if(!ownerLayout(space,owner,work,&position)||owner.registration34.attached||
       !zone||zone>space.lifecycle.zoneCount||owner.allocationSerial==std::numeric_limits<std::uint64_t>::max())return rejected;
    if(!worldTopology(space.portals)){rejected.status=Status::InvalidTopology;return rejected;}
    Transaction t{space.lifecycle,space.portals,work};
    if(!t.begin(owner.registration34,owner.storage,true))return t.result;
    t.staged.object=owner.object30;t.staged.method=ZoneMethod50CL::Second;t.staged.attached=true;
    if(!preparePoint(position,t.staged,t.result))return t.result;
    work.links[0]={0,zone};work.entries[0].used=true;
    if(!t.move(0,zone)||!t.addOverlaps(0))return t.result;
    // This adapter's retained backing and scratch transaction are native
    // preadmission, NOT source malloc-null/partial-prefix equivalence. Preserve
    // its no-publication failure policy above. Once admitted, the actual
    //114770 success uses the SAME process assertion records as Page Cameras.
    if(!OriginalCameraObserverAssertions50KG::allocation1147E4(t.staged.storage.entries!=nullptr)){
        t.result.status=Status::InvalidTopology;return t.result;
    }
    t.publish(owner.registration34);owner.position180=position;owner.parent38=zone;++owner.allocationSerial;
    if(!OriginalCameraObserverAssertions50KG::membership114884(owner.registration34.storage.links[0].zone!=0))
        t.result.status=Status::InvalidTopology;
    return t.result;
}
LiveZoneResult50EN attachOriginalCameraSceneFromRacer175224(ResetVisibilityWorld50EY& space,
    OriginalCameraSceneOwner& owner,const LiveZoneMembership50EN& racer,
    LiveZoneWorkspace50EN& work) noexcept {
    LiveZoneResult50EN rejected;
    if(reinterpret_cast<std::uintptr_t>(racer.storage.links)%alignof(CollectionZoneLink50BX)||
       reinterpret_cast<std::uintptr_t>(racer.storage.entries)%alignof(LiveZoneEntry50EN)||
       racer.storage.capacity>std::numeric_limits<std::size_t>::max()/sizeof(LiveZoneEntry50EN)||
       racer.storage.capacity>std::numeric_limits<std::size_t>::max()/sizeof(CollectionZoneLink50BX))return rejected;
    const OriginalCameraSceneSpan reads[]={{&racer,sizeof racer},
        {racer.storage.links,racer.storage.capacity*sizeof(CollectionZoneLink50BX)},
        {racer.storage.entries,racer.storage.capacity*sizeof(LiveZoneEntry50EN)}};
    if(!ownerLayout(space,owner,work,nullptr,nullptr,nullptr,0,reads,3)||
       !racer.attached||racer.object==owner.object30||racer.method!=ZoneMethod50CL::Second||
       !racer.storage.capacity||!racer.storage.links||!racer.storage.entries||
       !worldTopology(space.portals)||!liveTopology(space.lifecycle,space.portals,racer))return rejected;
    const auto position=racer.bounds.center;const auto zone=racer.storage.links[0].zone;
    if(owner.registration34.attached)
        return rebindOriginalCameraScene177AB0(space,owner,zone,position,position,work);
    return attachOriginalCameraScene114770(space,owner,zone,position,work);
}
LiveZoneResult50EN removeOriginalCameraScene1148D0(ResetVisibilityWorld50EY& space,
    OriginalCameraSceneOwner& owner,LiveZoneWorkspace50EN& work) noexcept {
    LiveZoneResult50EN rejected;
    if(!ownerLayout(space,owner,work))return rejected;
    return removeLiveZoneMembership50EN(space.lifecycle,space.portals,owner.registration34,work);
}
LiveZoneResult50EN rebindOriginalCameraScene177AB0(ResetVisibilityWorld50EY& space,
    OriginalCameraSceneOwner& owner,CollectionRef50BX zone,
    const SpringVector4_48N& position,const SpringVector4_48N& saved,
    LiveZoneWorkspace50EN& work) noexcept {
    LiveZoneResult50EN rejected;
    if(!ownerLayout(space,owner,work,&position,&saved)||!owner.registration34.attached||
       !zone||zone>space.lifecycle.zoneCount||owner.allocationSerial==std::numeric_limits<std::uint64_t>::max())return rejected;
    if(!std::isfinite(saved.v[0])||!std::isfinite(saved.v[1])||!std::isfinite(saved.v[2])){
        rejected.status=Status::NonFinite;return rejected;}
    if(!worldTopology(space.portals)||!liveTopology(space.lifecycle,space.portals,owner.registration34)){
        rejected.status=Status::InvalidTopology;return rejected;}
    Transaction t{space.lifecycle,space.portals,work};
    if(!t.begin(owner.registration34,owner.storage,false))return t.result;
    //1148D0/281490 removes primary then linked secondaries. The subsequent
    //280890 allocation is represented by a fresh registration generation.
    if(!t.move(0,0)||!t.clearSecondaries())return t.result;
    work.links[0]={0,zone};work.entries[0]={};work.entries[0].used=true;
    if(!preparePoint(position,t.staged,t.result)||!t.move(0,zone)||!t.addOverlaps(0))return t.result;
    //1781F8 copies racer690->E0+20 only AFTER all registration calls return.
    t.staged.bounds.center=saved;t.staged.bounds.cachedCenter=saved;
    t.publish(owner.registration34);owner.position180=position;owner.parent38=zone;++owner.allocationSerial;
    return t.result;
}
LiveZoneResult50EN updateOriginalCameraScene114C44(ResetVisibilityWorld50EY& space,
    OriginalCameraSceneOwner& owner,const SpringVector4_48N& position,
    LiveZoneWorkspace50EN& work) noexcept {
    LiveZoneResult50EN rejected;
    if(!ownerLayout(space,owner,work,&position))return rejected;
    if(!owner.registration34.attached){owner.position180=position;rejected.status=Status::Complete;return rejected;}
    if(!worldTopology(space.portals)||!liveTopology(space.lifecycle,space.portals,owner.registration34)){
        rejected.status=Status::InvalidTopology;return rejected;}
    Transaction t{space.lifecycle,space.portals,work};
    if(!t.begin(owner.registration34,owner.storage,false))return t.result;
    const auto oldCenter=center(owner.registration34.bounds);
    if(!preparePoint(position,t.staged,t.result))return t.result;
    const auto now=center(t.staged.bounds);auto primary=work.links[0].zone;CollectionRef50BX previous=0;
    auto edge=space.portals.portalHeads[primary-1u];
    if(!edge){if(!t.move(0,primary))return t.result;}
    else {
        const auto& world=space.portals;
        if(world.portalLinkCount==std::numeric_limits<std::size_t>::max()||
           world.zoneCount==std::numeric_limits<std::size_t>::max()||
           world.portalLinkCount+1u>std::numeric_limits<std::size_t>::max()/(world.zoneCount+1u)){
            t.result.status=Status::TraversalBoundary;return t.result;}
        const auto limit=(world.portalLinkCount+1u)*(world.zoneCount+1u);std::size_t visits=0;
        while(edge){
            if(++visits>limit){t.result.status=Status::TraversalBoundary;return t.result;}
            const auto& p=work.portalLinks[edge-1u];bool crossed=false;
            if(p.zone!=previous){t.result.geometryStatus=crossPortal230BD0_50EO(world.portals[p.portal-1u],oldCenter,now,crossed);
                if(t.result.geometryStatus!=PortalGeometryStatus50EO::Complete){t.result.status=Status::GeometryRejected;return t.result;}}
            if(crossed){previous=primary;primary=p.zone;edge=world.portalHeads[primary-1u];++t.result.crossedPortals;}
            else edge=p.next;
        }
        if(primary!=work.links[0].zone){
            if(!t.move(0,0)||!t.clearSecondaries())return t.result;
            work.links[0].zone=primary;if(!t.move(0,primary)||!t.addOverlaps(0))return t.result;
        }else if(!t.move(0,primary)||!t.updateOverlaps(oldCenter))return t.result;
    }
    t.publish(owner.registration34);owner.position180=position;owner.parent38=primary;return t.result;
}
} // namespace rr::game
