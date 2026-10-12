#pragma once
#include "authored_runtime_ground50cp_fixture.h"
#include "live_terrain_probe50fr_fixture.h"
#include "candidate_handles50cb.h"
#include "static_type2_producer49x.h"
#include "collection_projection50cf.h"
#include "vehicle_candidates50ca.h"
#include "zone_registration_events50cq.h"
#include "type3_query48p.h"
#include "../../src/assets/material_audio50hi.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

// Cold ownership only. This replaces size assumptions, not original resources
// or authoring: every candidate must point at one actual loaded COL owner.
// No geometry, filters, opaque identities, spawn, route or query state is made
// up here. The caller constructs those owners before preparing this layer.
namespace track_runtime_storage50IG {
using namespace rr::game;
namespace a=rr::assets;
using Ground=authored_runtime_ground50cp_fixture::ColdScene;
// Share the cold loader's one-block ceiling, including all three dynamic
// references. Neither layer may admit 1024 static owners plus live queries.
inline constexpr std::size_t hardTerrainOwners=authored_runtime_ground50cp_fixture::hardCollisionOwners50IG;
inline constexpr std::size_t runtimeSearchSlotLimit=authored_runtime_ground50cp_fixture::runtimeSearchSlotLimit50IG;
inline constexpr std::size_t runtimeDynamicReferenceReserve=authored_runtime_ground50cp_fixture::runtimeDynamicReferenceReserve50IG;
static_assert(runtimeDynamicReferenceReserve==3&&hardTerrainOwners+runtimeDynamicReferenceReserve==runtimeSearchSlotLimit);
inline constexpr std::size_t hardZones=256,hardPortals=4096;
inline constexpr std::size_t sourceSelectionCapacity=CandidateMergeCapacity48S;
static_assert(sourceSelectionCapacity==64);

struct Limits {
    std::size_t terrainOwners=hardTerrainOwners,zones=hardZones,portals=hardPortals;
    // Requested native element payload, including reserved filter elements.
    // Excludes allocator bookkeeping and independently owned mesh/radii bytes.
    std::size_t coldPayloadBytes=32u*1024u*1024u;
    // Only the explicitly selected original-camera route reserves its actual
    // sphere24/ray190 records. Legacy callers retain the exact three refs.
    bool originalCameraQueries=false;
};
struct Binding {
    // Caller order is authoritative (TA may retain its existing corpus order).
    // Every original loaded owner must occur exactly once, with no duplicates.
    std::size_t originalOwner=0;
    Type2StaticCandidate49X* candidate=nullptr;
    // Additive original TOB/model owner. Not a fake SCB/static geometry row.
    // Caller authenticates the actual source resource and constructor fields.
    bool animated=false;
    std::uint32_t authoredZone=0;
    std::uint8_t materialA9=0;
    ZoneMethod50CL method=ZoneMethod50CL::First;
    bool trigger=false; // authenticated additive TOB Gate sphere, no SCB index
};
struct Counts {
    std::size_t terrain=0,zones=0,portals=0,methods=0,blocks=0,objects=0;
    std::size_t portalLinks=0,membership=0,selected=0,events=0;
    CollectionRef50BX mainQuery=0,secondaryQuery=0,terrainQuery=0;
    CollectionRef50BX cameraSphere=0,cameraRay=0;
};
struct Diagnostic {
    std::string message;
    std::size_t candidate=0,zone=0,requestedPayloadBytes=0;
};

// Runtime mutation can change elements, never backing address or capacity.
// This cannot be resized, copied or moved after construction.
template<class T> class FixedBuffer {
    std::unique_ptr<T[]> values_;
    std::size_t count_=0;
public:
    explicit FixedBuffer(std::size_t count):
        values_(count?std::make_unique<T[]>(count):nullptr),count_(count){}
    FixedBuffer(const FixedBuffer&)=delete;
    FixedBuffer& operator=(const FixedBuffer&)=delete;
    FixedBuffer(FixedBuffer&&)=delete;
    FixedBuffer& operator=(FixedBuffer&&)=delete;
    T* data() noexcept{return values_.get();}
    const T* data()const noexcept{return values_.get();}
    std::size_t size()const noexcept{return count_;}
    T& operator[](std::size_t i) noexcept{return values_[i];}
    const T& operator[](std::size_t i)const noexcept{return values_[i];}
    T* begin() noexcept{return data();}
    T* end() noexcept{return count_?data()+count_:data();}
};

struct Membership {
    FixedBuffer<CollectionZoneLink50BX> links;
    FixedBuffer<LiveZoneEntry50EN> entries;
    LiveZoneMembership50EN state{};
    explicit Membership(std::size_t count):links(count),entries(count){}
    LiveZoneStorage50EN storage() noexcept{return {links.data(),entries.data(),links.size()};}
};

// Own this at a stable address for the complete session. Backing may be exposed
// to the existing serial frame APIs, but never replace descriptors while a
// query, cursor, racer-bank visit or registration operation is outstanding.
struct Storage {
    const Counts count;
    FixedBuffer<Type2StaticCandidate49X*> candidates;
    FixedBuffer<std::uint32_t> candidateZones;
    FixedBuffer<ZoneMethod50CL> candidateMethods;
    FixedBuffer<ContactMaterial50HI> contactMaterials;
    FixedBuffer<CandidateHandle50CB> handles;
    FixedBuffer<Type3Candidate48P> movement,registeredMovement;
    FixedBuffer<TerrainCandidate48J*> terrainTargets;
    FixedBuffer<TerrainCandidate48J> terrainSelected;
    FixedBuffer<TerrainCandidateEffects48J> terrainScratchEffects,terrainEffects;
    FixedBuffer<std::size_t> registeredIndices,probeIndices,resolveScratch;
    std::size_t registeredCount=0,probeCount=0;

    SearchStorage50BS movementRegistry{};
    SearchListBlock50BR movementBlocks[2]{};
    SearchListInput50BR movementView{};
    SearchListCursor50BR movementCursor{};
    FixedBuffer<SearchStorage50BS> registries,membershipScratchRegistries;
    FixedBuffer<ZoneRegistration50CJ> registrations;
    FixedBuffer<SearchListBlock50BR> blocks;
    FixedBuffer<CollectionTree50BX> trees;
    FixedBuffer<CollectionZone50BX> zones;
    ZoneLifecycle50CL lifecycle{};
    ZoneRegistrationScratch50CJ registrationScratch{};
    FixedBuffer<ZoneRegistrationCommand50CQ> events;
    ZoneRegistrationQueue50CQ queue{};

    FixedBuffer<LiveZonePortalInput50EN> authoredPortals;
    FixedBuffer<CollectionRef50BX> portalHeads;
    FixedBuffer<LiveZonePortalLink50EN> portalLinks,portalScratchLinks;
    FixedBuffer<PortalGeometry50EO> portalGeometry;
    LiveZoneWorld50EN world{};
    Membership mainMembership,secondaryMembership,terrainMembership;
    FixedBuffer<CollectionZoneLink50BX> membershipScratchLinks;
    FixedBuffer<LiveZoneEntry50EN> membershipScratchEntries;
    LiveZoneWorkspace50EN membershipWork{};

    FixedBuffer<CollectionObject50BX> objects;
    FixedBuffer<CollectionObjectState50BX> source,scratch,first,second;
    FixedBuffer<CollectionTreeState50BX> treeSource,treeScratch,treeFirst,treeSecond;
    FixedBuffer<std::uint32_t> scratchIds,firstIds,secondIds,mergeScratch,mergedIds;
    CandidateCollectionInput50BX collectionInput{};

    explicit Storage(Counts n):count(n),candidates(n.terrain),candidateZones(n.terrain),candidateMethods(n.terrain),
        contactMaterials(n.terrain),handles(n.terrain),movement(n.terrain),
        registeredMovement(n.terrain),terrainTargets(n.terrain),terrainSelected(n.selected),
        terrainScratchEffects(n.selected),terrainEffects(n.selected),
        registeredIndices(n.selected),probeIndices(n.selected),resolveScratch(sourceSelectionCapacity),
        registries(n.methods),membershipScratchRegistries(n.methods),registrations(n.objects),
        blocks(n.blocks),trees(n.methods),zones(n.zones),events(n.events),
        authoredPortals(n.portals),portalHeads(n.zones),portalLinks(n.portalLinks),
        portalScratchLinks(n.portalLinks),portalGeometry(n.portals),
        mainMembership(n.membership),secondaryMembership(n.membership),terrainMembership(n.membership),
        membershipScratchLinks(n.membership),membershipScratchEntries(n.membership),
        objects(n.objects),source(n.objects),scratch(n.objects),first(n.objects),second(n.objects),
        treeSource(n.methods),treeScratch(n.methods),treeFirst(n.methods),treeSecond(n.methods),
        scratchIds(sourceSelectionCapacity),firstIds(sourceSelectionCapacity),secondIds(sourceSelectionCapacity),
        mergeScratch(sourceSelectionCapacity),mergedIds(sourceSelectionCapacity){}
    Storage(const Storage&)=delete;
    Storage& operator=(const Storage&)=delete;
    Storage(Storage&&)=delete;
    Storage& operator=(Storage&&)=delete;

    CandidateCollectionBuffers50BX collectionBuffer(CollectionObjectState50BX* o,
        CollectionTreeState50BX* t,std::uint32_t* ids)const noexcept {
        CandidateCollectionBuffers50BX b{};b.objects=o;b.objectCapacity=count.objects;
        b.trees=t;b.treeCapacity=count.methods;b.identities=ids;
        b.identityCapacity=sourceSelectionCapacity;return b;
    }
    CandidateCollectionState50BX initialCollection()const noexcept {
        CandidateCollectionState50BX s{};s.objects=source.data();s.objectCapacity=count.objects;
        s.trees=treeSource.data();s.treeCapacity=count.methods;return s;
    }
    VehicleCandidateWorkspace50CA collectionWorkspace() noexcept {
        VehicleCandidateWorkspace50CA w{};
        w.collector.buffers=collectionBuffer(scratch.data(),treeScratch.data(),scratchIds.data());
        w.first=collectionBuffer(first.data(),treeFirst.data(),firstIds.data());
        w.second=collectionBuffer(second.data(),treeSecond.data(),secondIds.data());
        w.mergeScratch={mergeScratch.data(),mergeScratch.size()};
        w.merged={mergedIds.data(),mergedIds.size()};return w;
    }
    // Mirrors the current late movement registry iteration without assigning
    // vector-owning candidate objects (which could allocate after cold setup).
    bool gatherMovement() noexcept {
        if(prepareSearchView50BT(movementRegistry,movementBlocks,2,movementView)!=
            SearchStorageStatus50BS::Complete)return false;
        resetSearchList50BR(movementCursor,true);registeredCount=0;
        for(;;){
            std::uint32_t id=0;
            if(nextSearchList50BR(movementView,movementCursor,id)!=SearchListStatus50BR::Complete)return false;
            if(!id)return true;
            if((id&3u)||id/4u<1u||id/4u>count.terrain||registeredCount>=count.terrain)return false;
            const std::size_t index=id/4u-1u;
            const auto& input=movement[index];auto& output=registeredMovement[registeredCount];
            if(!live_terrain_probe50fr::detail::copyCandidate(input.collision,output.collision))return false;
            output.shapeType=input.shapeType;output.compound=input.compound;output.hasMaterialScene=input.hasMaterialScene;
            output.sceneName=input.sceneName;output.sceneNameBytes=input.sceneNameBytes;
            registeredIndices[registeredCount++]=index;
        }
    }
    // Replace the corresponding old world-storage registration block in
    // native_racer_owners50hn.inc. Do not register this Storage/FixedBuffer or
    // any std::vector object itself. External candidate owner lanes and the
    // actual vehicle/query state remain the caller's existing registrations.
    template<class Bank> void registerMutableOwners(Bank& bank) {
        auto elements=[&](auto& a){bank.elements(a.data(),a.size());};
        for(auto& c:movement){bank.owner(c.collision.filter.flagsF8);
            bank.owner(c.collision.copied40);bank.owner(c.collision.copied50);}
        bank.owner(lifecycle);elements(registries);elements(registrations);
        elements(blocks);elements(trees);elements(zones);bank.owner(queue);elements(events);
        elements(portalHeads);elements(portalLinks);elements(portalGeometry);
        for(auto* m:{&mainMembership,&secondaryMembership,&terrainMembership}){
            bank.owner(m->state);elements(m->links);elements(m->entries);}
        elements(objects);elements(source);elements(treeSource);bank.owner(collectionInput);
        elements(registeredIndices);bank.owner(registeredCount);elements(probeIndices);bank.owner(probeCount);
    }
};

namespace detail {
inline void need(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
struct Budget {
    std::size_t capacity=0,used=0;
    template<class T> void add(std::size_t count,std::size_t repeats=1){
        need(!repeats||count<=std::numeric_limits<std::size_t>::max()/repeats,"track storage extent overflow");
        count*=repeats;
        need(count<=std::numeric_limits<std::size_t>::max()/sizeof(T),"track storage byte overflow");
        const auto bytes=count*sizeof(T);
        need(used<=capacity&&bytes<=capacity-used,"track storage requested payload exceeds cold budget");used+=bytes;
    }
};
inline void account(Budget& b,const Counts& n){
    b.add<Storage>(1);b.add<Type2StaticCandidate49X*>(n.terrain);
    b.add<std::uint32_t>(n.terrain);b.add<ZoneMethod50CL>(n.terrain);b.add<ContactMaterial50HI>(n.terrain);
    b.add<CandidateHandle50CB>(n.terrain);b.add<Type3Candidate48P>(n.terrain,2);
    b.add<TerrainCandidate48J*>(n.terrain);b.add<TerrainCandidate48J>(n.selected);
    b.add<TerrainCandidateEffects48J>(n.selected,2);b.add<std::size_t>(n.selected,2);
    b.add<std::size_t>(sourceSelectionCapacity);b.add<SearchStorage50BS>(n.methods,2);
    b.add<ZoneRegistration50CJ>(n.objects);b.add<SearchListBlock50BR>(n.blocks);
    b.add<CollectionTree50BX>(n.methods);b.add<CollectionZone50BX>(n.zones);
    b.add<ZoneRegistrationCommand50CQ>(n.events);b.add<LiveZonePortalInput50EN>(n.portals);
    b.add<CollectionRef50BX>(n.zones);b.add<LiveZonePortalLink50EN>(n.portalLinks,2);
    b.add<PortalGeometry50EO>(n.portals);b.add<CollectionZoneLink50BX>(n.membership,4);
    b.add<LiveZoneEntry50EN>(n.membership,4);b.add<CollectionObject50BX>(n.objects);
    b.add<CollectionObjectState50BX>(n.objects,4);b.add<CollectionTreeState50BX>(n.methods,4);
    b.add<std::uint32_t>(sourceSelectionCapacity,5);
}
inline bool vectorField(const a::ParameterBlock48F& block,const char* key,Vector3_48C& out){
    const std::string* value=nullptr;
    for(const auto& f:block.fields)if(f.name==key){if(value)return false;value=&f.value;}
    if(!value)return false;
    std::size_t begin=0;
    for(unsigned axis=0;axis<3;++axis){
        auto end=value->find(',',begin);
        if(axis<2&&end==std::string::npos)return false;
        if(axis==2&&end!=std::string::npos)return false;
        if(end==std::string::npos)end=value->size();
        auto a=begin,z=end;while(a<z&&(*value)[a]==' ')++a;while(z>a&&(*value)[z-1]==' ')--z;
        if(!rr::assets::parseTrackNumber50AD(value->data()+a,z-a,out.v[axis]))return false;
        begin=end+1;
    }
    return begin==value->size()+1;
}
inline void reserveFilter(CollisionFilter48J& f,const std::array<std::size_t,3>& maximum){
    f.categoryMasks.reserve(maximum[0]);
    f.identityLists[0].identities.reserve(maximum[1]);
    f.identityLists[1].identities.reserve(maximum[2]);
}
} // namespace detail

class Owner {
    std::unique_ptr<Storage> storage_;
public:
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    Storage* get() noexcept{return storage_.get();}
    const Storage* get()const noexcept{return storage_.get();}
    // Transactional publication. A failure leaves this owner unprepared and
    // every original/candidate input untouched. No live retry/resizing exists.
    // Caller keeps Ground, candidates, names, filters and radii at their stable
    // addresses longer than this owner; its hot helpers never discover assets.
    bool prepare(const Ground& ground,const Binding* bindings,std::size_t count,
        const Limits& limits,Diagnostic& diagnostic) noexcept {
        using namespace detail;
        try {
            diagnostic={};need(!storage_,"track storage already prepared");
            need(limits.terrainOwners&&limits.terrainOwners<=hardTerrainOwners&&
                limits.zones&&limits.zones<=hardZones&&limits.portals<=hardPortals&&
                limits.coldPayloadBytes&&limits.coldPayloadBytes<=32u*1024u*1024u,
                "track storage limits may only reduce hard ceilings");
            need(bindings&&count&&count>=ground.owners.size()&&count<=limits.terrainOwners,
                "track storage requires every original loaded COL owner exactly once");
            Counts n{};n.terrain=count;n.zones=ground.world.zones.size();n.portals=ground.world.portals.size();
            need(n.zones&&n.zones<=limits.zones&&n.portals<=limits.portals,"original world exceeds storage bounds");
            n.methods=n.zones*2;n.blocks=n.zones*4;
            const std::size_t references=runtimeDynamicReferenceReserve+(limits.originalCameraQueries?2u:0u);
            need(count<=runtimeSearchSlotLimit-references,
                "original camera query pair exceeds the one-block SearchList ceiling");
            n.objects=count+references;n.portalLinks=n.portals*2;
            n.membership=n.portals+1;n.selected=std::max(count,sourceSelectionCapacity);
            n.events=std::max<std::size_t>(128,n.objects);
            n.mainQuery=static_cast<CollectionRef50BX>(count+1);
            n.secondaryQuery=static_cast<CollectionRef50BX>(count+2);
            n.terrainQuery=static_cast<CollectionRef50BX>(count+3);
            if(limits.originalCameraQueries){
                n.cameraSphere=static_cast<CollectionRef50BX>(count+4);
                n.cameraRay=static_cast<CollectionRef50BX>(count+5);
            }
            Budget budget{limits.coldPayloadBytes};account(budget,n);
            std::array<std::size_t,3> filterMaximum{};
            for(std::size_t z=0;z<n.zones;++z){
                diagnostic.zone=z;const auto choice=a::readZoneSearchSelection50CN(ground.world,z);
                need(ground.world.zones[z].zone==z+1&&choice.status==a::ZoneSearchReadStatus50CN::Complete&&
                    choice.selection.collisionStatic118==a::ZoneSearchKind50CN::List&&
                    choice.selection.collisionDynamic11C==a::ZoneSearchKind50CN::List,
                    "original zone requires unsupported search method; no List substitution");
            }
            std::size_t staticCount=0;
            for(std::size_t i=0;i<count;++i){
                diagnostic.candidate=i;const auto& binding=bindings[i];
                need(binding.candidate,"invalid original collider binding");
                for(std::size_t j=0;j<i;++j)need((binding.animated||binding.trigger||bindings[j].animated||bindings[j].trigger||bindings[j].originalOwner!=binding.originalOwner)&&
                    bindings[j].candidate!=binding.candidate,"duplicate original collider binding");
                const auto& c=binding.candidate->candidate.collision;
                if(binding.trigger){
                    need(!binding.animated&&binding.originalOwner==std::numeric_limits<std::size_t>::max()&&
                        binding.authoredZone&&binding.authoredZone<=n.zones&&binding.method==ZoneMethod50CL::First&&
                        binding.candidate->shapeType==5&&(bool(binding.candidate->gate)!=bool(binding.candidate->nativeTriggers))&&c.shapeType==5&&
                        !c.geometry.mesh&&!c.compound&&c.hasResponseCallbackE8&&c.filter.categoryA4==8u&&
                        c.filter.identity&&c.geometry.id==c.filter.identity&&c.ownerIdentity9C,
                        "original Gate sphere binding is incomplete");
                }else if(binding.animated){
                    need(binding.originalOwner==std::numeric_limits<std::size_t>::max()&&binding.authoredZone&&binding.authoredZone<=n.zones&&
                        binding.candidate->shapeType==1&&binding.candidate->compound&&c.shapeType==1&&c.compound==binding.candidate->compound&&
                        c.filter.identity&&c.geometry.id==c.filter.identity&&c.ownerIdentity9C,
                        "original animated compound binding is incomplete");
                }else{
                need(binding.originalOwner<ground.owners.size(),"invalid original SCB collider index");++staticCount;
                const auto& original=ground.owners[binding.originalOwner];
                need(original.zone&&original.zone<=n.zones&&c.geometry.mesh==&original.mesh&&
                    !std::memcmp(c.geometry.worldMatrix.v,original.instance.world.v,sizeof original.instance.world.v)&&
                    c.filter.categoryA4==original.instance.category&&c.filter.identity&&
                    !std::memcmp(c.sphere.center.v,original.instance.sphere.center.v,sizeof c.sphere.center.v)&&
                    !std::memcmp(&c.sphere.radius,&original.instance.sphere.radius,sizeof(float)),
                    "candidate does not borrow its original prepared COL placement/category/sphere");
                }
                const auto& f=c.filter;
                const std::size_t lengths[]{f.categoryMasks.size(),f.identityLists[0].identities.size(),f.identityLists[1].identities.size()};
                for(unsigned k=0;k<3;++k){budget.add<std::uint32_t>(lengths[k]);filterMaximum[k]=std::max(filterMaximum[k],lengths[k]);}
            }
            need(staticCount==ground.owners.size(),"not every original SCB collider is bound exactly once");
            for(auto maximum:filterMaximum){budget.add<std::uint32_t>(maximum,n.terrain);budget.add<std::uint32_t>(maximum,n.selected);}
            diagnostic.requestedPayloadBytes=budget.used;
            auto next=std::make_unique<Storage>(n);
            for(std::size_t i=0;i<count;++i){
                diagnostic.candidate=i;auto& c=*bindings[i].candidate;
                const auto& binding=bindings[i];
                next->candidates[i]=&c;next->candidateZones[i]=(binding.animated||binding.trigger)?binding.authoredZone:ground.owners[binding.originalOwner].zone;
                next->candidateMethods[i]=binding.method;
                next->terrainTargets[i]=&c.candidate.collision;
                next->handles[i]={c.candidate.collision.filter.identity,i};
                std::string error;
                if(binding.animated||binding.trigger)next->contactMaterials[i]={c.candidate.collision.filter.identity,binding.materialA9};
                else {const auto& original=ground.owners[binding.originalOwner];
                    need(a::readTrackContactMaterial50HI(ground.world,original.zone,original.ordinal,
                        c.candidate.collision.filter.identity,next->contactMaterials[i],error),
                        "original contact MaterialID mapping rejected");}

                auto& movement=next->movement[i];movement.collision=c.candidate.collision;
                movement.shapeType=c.shapeType;movement.compound=c.compound;movement.hasMaterialScene=c.candidate.hasMaterialScene;
                movement.sceneName=c.candidate.sceneName;movement.sceneNameBytes=c.candidate.sceneNameBytes;
                reserveFilter(next->registeredMovement[i].collision.filter,filterMaximum);
                const auto p=projectCollectionObject50CF(c.candidate.collision.filter,
                    c.candidate.collision.sphere,c.candidate.collision.typeTag,c.shapeType,0);
                need(p.valid,"original candidate collector projection rejected");
                next->objects[i]=p.object;next->source[i]=p.state;
            }
            std::sort(next->handles.begin(),next->handles.end(),[](const auto& x,const auto& y){return x.identity<y.identity;});
            for(std::size_t i=1;i<count;++i)need(next->handles[i-1].identity!=next->handles[i].identity,
                "duplicate original candidate identity");
            need(live_terrain_probe50fr::reserveSelected(next->terrainSelected.data(),n.selected,
                next->terrainTargets.data(),count),"reserve real terrain filter copies");
            need(initializeColdSearchStorage50BU(next->movementRegistry,runtimeSearchSlotLimit)==SearchStorageStatus50BS::Complete,
                "movement SearchList initialization failed");
            std::uint32_t slot=0;
            need(adoptFirstSearchBlock50BU(next->movementRegistry,slot)==SearchStorageStatus50BS::Complete,
                "movement SearchList first block unavailable");
            for(std::size_t i=0;i<count;++i){
                need(selectSearchSlot50BS(next->movementRegistry,0,slot)==SearchStorageStatus50BS::Complete&&slot==i+1&&
                    insertSearchSlot50BS(next->movementRegistry,slot,static_cast<std::uint32_t>((i+1)*4))==SearchStorageStatus50BS::Complete,
                    "original ordered movement registry rejected");
            }
            const ZoneLifecycleStorage50CL zones{next->registries.data(),n.methods,
                next->registrations.data(),n.objects,{next->trees.data(),n.methods,
                next->zones.data(),n.zones,next->blocks.data(),n.blocks}};
            need(initializeZoneLifecycle50CL(next->lifecycle,zones,n.zones,n.objects,runtimeSearchSlotLimit,
                next->registrationScratch)==SearchStorageStatus50BS::Complete,"track zone lifecycle initialization failed");
            next->queue={next->events.data(),n.events,0,0};
            for(std::size_t i=0;i<count;++i)if(!bindings[i].animated&&!bindings[i].trigger)
                need(enqueueZoneRegistration50CQ(next->queue,{ZoneRegistrationOperation50CQ::Set,
                    static_cast<CollectionRef50BX>(i+1),next->candidateZones[i],next->candidateMethods[i]})==ZoneQueueStatus50CQ::Complete,
                    "cold original collider registration queue full");
            const auto drained=drainZoneRegistrations50CQ(next->queue,next->lifecycle,next->registrationScratch,n.events);
            need(drained.status==ZoneQueueStatus50CQ::Complete&&!drained.remaining,"cold original collider registration rejected");
            for(std::size_t i=0;i<n.portals;++i){
                const auto& p=ground.world.portals[i];need(p.parameter_block<ground.world.parameters.blocks.size(),"invalid original portal block");
                auto& v=next->authoredPortals[i];v.zoneA=p.zones[0];v.zoneB=p.zones[1];
                const auto& block=ground.world.parameters.blocks[p.parameter_block];
                for(unsigned k=0;k<4;++k)need(vectorField(block,("vertex"+std::to_string(k+1)).c_str(),v.vertices[k]),
                    "original portal vertex text rejected");
            }
            const LiveZoneWorldStorage50EN portals{next->portalHeads.data(),n.zones,
                next->portalLinks.data(),n.portalLinks,next->portalGeometry.data(),n.portals};
            need(prepareLiveZoneWorld50EN(next->authoredPortals.data(),n.portals,n.zones,portals,next->world)==
                LiveZoneStatus50EN::Complete,"original portal world preparation rejected");
            next->membershipWork={next->membershipScratchRegistries.data(),n.methods,
                next->membershipScratchLinks.data(),next->membershipScratchEntries.data(),n.membership,
                next->portalScratchLinks.data(),n.portalLinks,{}};
            auto& input=next->collectionInput;input.objects=next->objects.data();
            // The optional pair does not exist in opponent-bank worlds. Its
            // concrete bank0 preparation alone expands this BANKED active
            // domain; backing and lifecycle capacities already include it.
            input.objectCount=n.terrain+runtimeDynamicReferenceReserve;
            input.trees=next->trees.data();input.treeCount=n.methods;input.zones=next->zones.data();input.zoneCount=n.zones;
            // Query owner, bounds, actual E0 membership and type tags remain
            // explicitly unbound until the caller has prepared each real query.
            storage_=std::move(next);diagnostic.message.clear();return true;
        }catch(const std::exception& e){try{diagnostic.message=e.what();}catch(...){}return false;}
        catch(...){try{diagnostic.message="Unexpected cold track-storage failure";}catch(...){}return false;}
    }
};
} // namespace track_runtime_storage50IG
