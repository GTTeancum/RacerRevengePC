#pragma once
#include "track_runtime_storage50ig_fixture.h"
#include "shared_query_world50jq_fixture.h"
#include "fallback_quadtree50jk_fixture.h"
#include "../../src/game/vegetation_mesh50jj.h"
#include "../../src/game/vegetation_world50jl.h"
#include "../../src/game/vegetation_raw_dispatch50jl.h"
#include "../../src/game/ai_reset_physics50je.h"
#include "../../src/game/group_query48u.h"
#include "../../src/game/material_audio50hi.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <memory>
#include <utility>
#include <vector>

namespace extended_fallback_world50JM {
using namespace rr::game;
using Prefix=track_runtime_storage50IG::Storage;
struct Bindings {
    std::uint32_t waterIdentity=0,ecosystemIdentity=0,solidFoliageCategory=0;
    const void* waterOwner=nullptr;
    const std::uint8_t* collisionTag=nullptr;const std::uint8_t* searchableTag=nullptr;
    const std::uint8_t* vegetationTag=nullptr;
    bool actualGcEnvironmentAndLoadOrderBound=false,nativeColdQueryPoolAdmission=false;
    // Optional canonical multi-racer namespace; never a selected-slot switch.
    shared_query_world50JQ::Owner* sharedQueries=nullptr;
};
// A serial full121920 transaction borrows the same actual tree as50CA/50IZ.
// Caller configures the query/zone/bounds fields for actual7A4 and runs the
// incremental iterator and Type3 cursor without an implicit final close.
struct Full121920Borrow50JN {
    CandidateCollectionInput50BX input{};
    CandidateCollectionState50BX initial{};
    CandidateCollectionWorkspace50BX workspace{};
    CandidateCollectionBuffers50BX destination{};
    GroupQueryInput48U groups{};
    GroupQueryState48U dispatchInitial{};
    const GroupQueryRef48U* identityOrder=nullptr;std::size_t identityOrderCount=0;
};
class Owner final {
    struct Phase {
        std::vector<CollectionObjectState50BX> objects;
        std::vector<CollectionTreeState50BX> methods;
        CollectionPoolState50BX pool{};
        std::vector<CollectionPoolBlockState50BX> blocks;
        std::vector<CollectionPoolEntry50BX> entries;
        std::array<GroupShapeState48T,5> shapes{};
        std::array<std::uint32_t,64> ids{};
        void size(std::size_t n,std::size_t m,std::size_t b,std::size_t e){objects.resize(n);methods.resize(m);blocks.resize(b);entries.resize(e);}
        CandidateCollectionBuffers50BX buffer(){return {objects.data(),objects.size(),methods.data(),methods.size(),
            &pool,1,blocks.data(),blocks.size(),entries.data(),entries.size(),ids.data(),ids.size(),shapes.data(),shapes.size()};}
        CandidateCollectionState50BX state()const{return {objects.data(),objects.size(),methods.data(),methods.size(),
            &pool,1,blocks.data(),blocks.size(),entries.data(),entries.size(),shapes.data(),shapes.size()};}
    };
    struct Data {
        Prefix* prefix=nullptr;std::size_t prefixCount=0,firstPrototype=0;
        shared_query_world50JQ::Owner* sharedQueries=nullptr;
        std::vector<CollectionObject50BX> objects;std::vector<CollectionTree50BX> methods;
        std::vector<CollectionZone50BX> zones;std::array<CollectionGroup50BX,7> groups{};
        std::array<CollectionRef50BX,7> memberRefs{};
        std::array<VegetationCollisionMesh50JJ,5> meshes;
        std::array<VegetationPrototype50JL,5> prototypes;
        // Actual137210 ->11B190 cold A9 store11B424, one row per shared
        // definition-owned CollisionObject. Raw Vegetation IDs are not contacts.
        std::array<ContactMaterial50HI,5> prototypeContactMaterials{};
        std::vector<Type2StaticCandidate49X> candidates;
        std::vector<VegetationPrototype50JL*> prototypesByObject;
        std::vector<Type3Candidate48P> type3Candidates;
        std::vector<GroupQueryObject48U> type3Objects;
        std::array<GroupQueryGroup48U,7> type3Groups{};
        std::array<GroupQueryShape48U,5> type3Shapes{};
        std::vector<GroupQueryObjectState48U> type3Initial;
        std::vector<CollectionRef50BX> identityOrder,blockOrder;
        std::array<GroupShapeSource48T,5> shapeSources{};
        std::array<VegetationShapeState50JL,5> rawInitial{},rawScratch{},rawDestination{};
        std::vector<ResetGroundGeometry50IZ> geometry;
        std::vector<std::uint8_t*> flags;
        std::vector<CollectionObjectState50BX*> canonicalObjects;
        std::vector<CollectionTreeState50BX*> canonicalMethods;
        fallback_quadtree50JK::Owner tree;
        Phase initial,scratch,first,second;
        std::vector<CollectionTraversalFrame50BX> traversal;
        std::array<std::uint32_t,64> mergeScratch{},merged{};
        std::array<std::size_t,64> mergedIndices{},secondIndices{};
        std::vector<std::uint32_t> meshStack;
        CollectionRef50BX fallbackZone=0;
        CandidateCollectionInput50BX input{};AiResetGroundExtension50JM extension{};
    };
    std::unique_ptr<Data> data_;
    static bool fail(std::string& e,const char* why){e=why;return false;}
    static bool copyColdFilter(const CollectionObject50BX& object,CollisionFilter48J& out){
        const auto& f=object.filter;
        const auto valid=[](const CollectionWords50BX& words){return words.count>=0&&
            static_cast<std::size_t>(words.count)<=words.capacity&&(!words.count||words.values);};
        if(!valid(f.identities[0])||!valid(f.identities[1])||!valid(f.categories))return false;
        out.identity=object.identity;out.categoryA4=f.categoryA4;out.acceptMaskB8=f.acceptMaskB8;out.flags31=f.flags31;
        for(unsigned i=0;i<2;++i){out.identityLists[i].mode=f.identities[i].mode;
            out.identityLists[i].identities.clear();if(f.identities[i].count)
                out.identityLists[i].identities.assign(f.identities[i].values,f.identities[i].values+f.identities[i].count);}
        out.categoryModeD4=f.categories.mode;out.categoryMasks.clear();
        if(f.categories.count)out.categoryMasks.assign(f.categories.values,f.categories.values+f.categories.count);
        return true;
    }
    static void syncPrototype(VegetationPrototype50JL& p){
        auto& c=p.candidate.candidate.collision;c.geometry.worldMatrix=p.shape.composed00;
        for(unsigned j=0;j<3;++j)c.sphere.center.v[j]=p.collection.cachedCenter.v[j];
        c.sphere.radius=p.collection.cachedRadius;c.filter.flagsF8=p.collection.flagsF8;
    }
    static void copyCandidateView(Type2StaticCandidate49X& out,const Type2StaticCandidate49X& actual){
        // Vector-owning filter lists are immutable cold bindings; never copied
        // or allocated in the hot path. Only these source mutable lanes change.
        out.candidate.collision.geometry.worldMatrix=actual.candidate.collision.geometry.worldMatrix;
        out.candidate.collision.sphere=actual.candidate.collision.sphere;
        out.candidate.collision.filter.flagsF8=actual.candidate.collision.filter.flagsF8;
        out.candidate.collision.copied40=actual.candidate.collision.copied40;
        out.candidate.collision.copied50=actual.candidate.collision.copied50;
    }
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    bool prepare(Prefix& prefix,const AiResetPhysicsOwners50JE& prefixGround,
        const gc_vegetation50JH::Owner& vegetation,const Bindings& b,std::string& error){
        if(data_||!vegetation.ready()||prefix.canonicalShared50JQ!=(b.sharedQueries!=nullptr)||prefix.count.objects!=prefix.count.terrain+5||
           prefix.collectionInput.groupCount||prefix.collectionInput.nodeCount||prefix.collectionInput.poolCount||
           prefix.collectionInput.shape0Count||prefixGround.groundWorld.objectCount!=prefix.count.objects||
           prefixGround.collisionFlagsCount!=prefix.count.objects||!prefixGround.collisionFlagsF8||!prefixGround.groundWorld.geometry||
           !b.collisionTag||!b.searchableTag||!b.vegetationTag||
           *b.collisionTag==*b.searchableTag||*b.collisionTag==*b.vegetationTag||*b.searchableTag==*b.vegetationTag||
           !b.solidFoliageCategory||(b.solidFoliageCategory&(b.solidFoliageCategory-1u)))
            return fail(error,"unbound extended-world prefix or source registry");
        if(b.sharedQueries&&(!b.sharedQueries->ready()||b.sharedQueries->prefix()!=&prefix||
           !b.sharedQueries->refresh()))return fail(error,"unbound canonical multi-racer query world");
        auto d=std::make_unique<Data>();d->prefix=&prefix;d->prefixCount=prefix.count.objects;d->sharedQueries=b.sharedQueries;
        const auto population=vegetation.instances().size()+2;
        d->firstPrototype=d->prefixCount+population;
        const auto total=d->sharedQueries?d->sharedQueries->layout()->objectCount:d->firstPrototype+5;
        if(d->sharedQueries&&(d->sharedQueries->layout()->prefixObjects!=d->prefixCount||
            d->sharedQueries->layout()->firstExtraRef!=d->firstPrototype+6))
            return fail(error,"changed legacy fallback/prototype reference range");
        d->objects.resize(total);d->candidates.resize(total);d->prototypesByObject.resize(total,nullptr);
        d->geometry.resize(total);d->flags.resize(total);d->canonicalObjects.resize(total);
        d->methods.resize(prefix.count.methods+1);d->zones.resize(prefix.count.zones+1);
        d->canonicalMethods.resize(d->methods.size());d->fallbackZone=static_cast<CollectionRef50BX>(d->zones.size());
        for(std::size_t i=0;i<d->prefixCount;++i){
            d->objects[i]=prefix.objects[i];d->canonicalObjects[i]=&prefix.source[i];
            d->flags[i]=prefixGround.collisionFlagsF8[i];d->geometry[i]=prefixGround.groundWorld.geometry[i];
            if(!d->flags[i])return fail(error,"unbound actual prefix flags");
            if(i<prefix.count.terrain){if(!prefix.candidates[i])return fail(error,"missing actual prefix candidate");d->candidates[i]=*prefix.candidates[i];}
            else{auto& c=d->candidates[i];c.shapeType=d->objects[i].shapeTag;c.candidate.collision.typeTag=d->objects[i].typeTag;
                c.candidate.collision.filter.identity=d->objects[i].identity;} // non-dispatchable dynamic slots
        }
        std::size_t proto=0;
        for(std::size_t g=0;g<vegetation.definitions().size();++g){
            const auto& def=vegetation.definitions()[g];
            if(g>=d->groups.size()||def.collisions.size()>1)return fail(error,"outside authenticated GC group scope");
            for(const auto& record:def.collisions){
                if(proto>=d->prototypes.size())return fail(error,"too many shared prototypes");
                auto& mesh=d->meshes[proto];auto& p=d->prototypes[proto];
                if(buildVegetationCollisionMesh50JJ(record.input,mesh)!=VegetationMeshStatus50JJ::Complete)
                    return fail(error,"source procedural prototype construction rejected");
                const auto index=d->firstPrototype+proto;auto& c=p.candidate;auto& collision=c.candidate.collision;
                collision.geometry={&mesh.mesh,record.input.local40,record.nativeIdentity};
                collision.filter.identity=record.nativeIdentity;collision.filter.categoryA4=b.solidFoliageCategory;
                d->prototypeContactMaterials[proto]={record.nativeIdentity,0u}; //11B424
                collision.filter.flagsF8=4;collision.filter.flags31=3;collision.typeTag=*b.collisionTag;
                collision.copied40[3]=collision.copied50[3]=1.f;c.shapeType=0;
                c.nodeRadii=mesh.nodeRadii.data();c.nodeRadiusCount=mesh.nodeRadii.size();
                //31-node binary tree has at most31 internal levels; exact depth
                // is derived without arithmetic changes to source-owned radii.
                std::array<std::pair<std::uint32_t,std::size_t>,63> stack{};std::size_t used=1;stack[0]={0,1};
                while(used){const auto current=stack[--used];c.maxDepth=std::max(c.maxDepth,current.second);
                    for(auto child:mesh.mesh.nodes[current.first].children)if(!(child&rr::assets::collision_mesh_leaf48i)){
                        if(used==stack.size()||child>=mesh.mesh.nodes.size())return fail(error,"invalid prototype topology");stack[used++]={child,current.second+1};}}
                for(unsigned j=0;j<3;++j){p.geometry.localCenter.v[j]=mesh.mesh.nodes[0].center[j];p.geometry.localHalf.v[j]=mesh.mesh.nodes[0].half[j];}
                p.source={record.input.local40,&p.geometry};p.shape={record.input.local40,record.input.world80,record.input.world80};
                p.collection.flagsF8=4;d->shapeSources[proto]=p.source;
                auto& object=d->objects[index];object.identity=record.nativeIdentity;object.typeTag=*b.collisionTag;
                object.shapeTag=0;object.shape0Ref=static_cast<CollectionRef50BX>(proto+1);
                object.filter.categoryA4=b.solidFoliageCategory;object.filter.flags31=3;
                d->candidates[index]=c;d->prototypesByObject[index]=&p;d->canonicalObjects[index]=&p.collection;
                d->flags[index]=&collision.filter.flagsF8;d->geometry[index].shape0=&collision.geometry;
                d->memberRefs[g]=static_cast<CollectionRef50BX>(index+1);++proto;
            }
            if(!vegetation.collectionDefinition(g,def.collisions.empty()?nullptr:&d->memberRefs[g],def.collisions.size(),d->groups[g]))
                return fail(error,"invalid actual group binding");
        }
        if(proto!=5||vegetation.definitions().size()!=7||vegetation.instances().size()!=2974)
            return fail(error,"outside authenticated GC complete population");
        const auto waterRef=static_cast<CollectionRef50BX>(d->prefixCount+1);
        const auto vegFirst=waterRef+1,ecoRef=static_cast<CollectionRef50BX>(d->firstPrototype);
        fallback_quadtree50JK::Constructor ctor{std::bit_cast<float>(0x80000000u),std::bit_cast<float>(0x80000000u),
            30720.f,30720.f,96.f,static_cast<CollectionRef50BX>(d->methods.size()),b.actualGcEnvironmentAndLoadOrderBound,b.nativeColdQueryPoolAdmission};
        fallback_quadtree50JK::Population populationBindings{{waterRef,b.waterIdentity,b.waterOwner,b.searchableTag},
            {ecoRef,b.ecosystemIdentity,&vegetation,b.searchableTag},vegFirst,b.vegetationTag,b.actualGcEnvironmentAndLoadOrderBound};
        if(!d->tree.prepare(ctor,populationBindings,vegetation,error))return false;
        for(std::size_t i=0;i<population;++i){const auto& entry=(*d->tree.population())[i];const auto index=entry.object-1;
            auto& o=d->objects[index];o.identity=entry.identity;o.typeTag=*entry.actualTypeTag;o.hasShape=false;
            if(i&&i+1<population&&!vegetation.collectionInstance(i-1,vegetation.instances()[i-1].definition+1,o))return fail(error,"raw instance projection");
            d->canonicalObjects[index]=d->tree.canonicalObjectState(i);d->flags[index]=&d->canonicalObjects[index]->flagsF8;
            d->candidates[index].candidate.collision.filter.identity=o.identity;d->candidates[index].candidate.collision.typeTag=o.typeTag;
        }
        if(d->sharedQueries)for(std::size_t i=0;i<total;++i){const auto ref=static_cast<CollectionRef50BX>(i+1);
            if(auto* actual=d->sharedQueries->object(ref)){
                auto* candidate=d->sharedQueries->candidate(ref);auto* state=d->sharedQueries->state(ref);auto* flags=d->sharedQueries->flags(ref);
                if(!candidate||!state||!flags)return fail(error,"unbound canonical query row");
                d->objects[i]=*actual;d->candidates[i]=*candidate;d->canonicalObjects[i]=state;d->flags[i]=flags;
                d->geometry[i]={};if(d->objects[i].shapeTag==0)d->geometry[i].shape0=&candidate->candidate.collision.geometry;
            }
        }
        for(std::size_t i=0;i<total;++i)for(std::size_t j=0;j<i;++j)
            if(!d->objects[i].identity||d->objects[i].identity==d->objects[j].identity)return fail(error,"duplicate extended-world identity");
        for(std::size_t i=0;i<prefix.count.methods;++i)d->canonicalMethods[i]=&prefix.treeSource[i];
        d->canonicalMethods.back()=d->tree.canonicalTreeState();d->methods.back()=*d->tree.tree();
        d->zones.back()={static_cast<CollectionRef50BX>(d->methods.size()),static_cast<CollectionRef50BX>(d->methods.size())};
        const auto n=d->tree.counts();for(auto* p:{&d->initial,&d->scratch,&d->first,&d->second})p->size(total,d->methods.size(),n.poolBlocks,n.poolEntries);
        d->traversal.resize(d->tree.maximumQueryStackDepth());d->meshStack.resize(std::max<std::size_t>(64,total));
        d->extension={&prefix.lifecycle,d->methods.data(),d->zones.data(),total,d->methods.size(),d->zones.size(),&d->fallbackZone,d->sharedQueries!=nullptr};
        d->type3Candidates.resize(total);d->type3Objects.resize(total);d->type3Initial.resize(total);
        for(std::size_t i=0;i<total;++i){
            const auto& c=d->candidates[i];auto& t=d->type3Candidates[i];
            t.collision=c.candidate.collision;t.shapeType=d->objects[i].shapeTag;t.compound=c.compound;
            t.hasMaterialScene=c.candidate.hasMaterialScene;t.sceneName=c.candidate.sceneName;t.sceneNameBytes=c.candidate.sceneNameBytes;
            if(!copyColdFilter(d->objects[i],t.collision.filter))return fail(error,"invalid actual cold Type3 filter spans");
            const auto& o=d->objects[i];auto& g=d->type3Objects[i];g.candidate=&t;g.group=o.group;g.shape0=o.shape0Ref;
            std::copy_n(o.position,3,g.position);g.scale10=o.scale10;g.scale11=o.scale11;g.hasShape=o.hasShape;
        }
        for(std::size_t i=0;i<7;++i){const auto& g=d->groups[i];
            d->type3Groups[i]={g.members,g.memberCapacity,g.memberCount,g.base180,g.other184,g.base18C,g.other194};}
        for(std::size_t i=0;i<5;++i)d->type3Shapes[i]={d->prototypes[i].source.local40,&d->meshes[i].mesh};
        d->identityOrder.resize(total);for(std::size_t i=0;i<total;++i)d->identityOrder[i]=static_cast<CollectionRef50BX>(i+1);
        std::sort(d->identityOrder.begin(),d->identityOrder.end(),[&](auto a,auto b){return d->objects[a-1].identity<d->objects[b-1].identity;});
        d->blockOrder.resize(n.poolBlocks);for(std::size_t i=0;i<n.poolBlocks;++i)d->blockOrder[i]=static_cast<CollectionRef50BX>(i+1);
        std::sort(d->blockOrder.begin(),d->blockOrder.end(),[&](auto a,auto b){
            const auto& x=d->tree.poolBlocks()[a-1];const auto& y=d->tree.poolBlocks()[b-1];
            const auto ax=x.entryCount?x.firstEntry:0,by=y.entryCount?y.firstEntry:0;
            return ax<by||(ax==by&&a<b);});
        data_=std::move(d);error.clear();return begin();
    }
    bool begin(const CandidateCollectionInput50BX* actualQuery=nullptr){
        if(!data_||!data_->tree.ready())return false;auto& d=*data_;auto& prefix=*d.prefix;
        if(d.sharedQueries&&!d.sharedQueries->refresh())return false;
        for(std::size_t i=0;i<d.prefixCount;++i){d.objects[i]=prefix.objects[i];if(i<prefix.count.terrain)copyCandidateView(d.candidates[i],*prefix.candidates[i]);}
        if(d.sharedQueries)for(std::size_t i=0;i<d.objects.size();++i){const auto ref=static_cast<CollectionRef50BX>(i+1);
            if(auto* actual=d.sharedQueries->object(ref)){d.objects[i]=*actual;copyCandidateView(d.candidates[i],*d.sharedQueries->candidate(ref));}}
        for(std::size_t i=0;i<d.objects.size();++i){d.initial.objects[i]=*d.canonicalObjects[i];d.initial.objects[i].flagsF8=*d.flags[i];}
        for(std::size_t i=0;i<d.methods.size();++i)d.initial.methods[i]=*d.canonicalMethods[i];
        for(std::size_t i=0;i<prefix.count.methods;++i)d.methods[i]=prefix.trees[i];
        for(std::size_t i=0;i<prefix.count.zones;++i)d.zones[i]=prefix.zones[i];
        d.initial.pool=*d.tree.canonicalPoolState();
        std::copy_n(d.tree.canonicalPoolBlockStates(),d.initial.blocks.size(),d.initial.blocks.data());
        std::copy_n(d.tree.canonicalPoolEntries(),d.initial.entries.size(),d.initial.entries.data());
        for(std::size_t i=0;i<5;++i){auto& p=d.prototypes[i];d.initial.shapes[i]=p.shape;
            d.rawInitial[i]={p.shape,p.candidate.candidate.collision.sphere};copyCandidateView(d.candidates[d.firstPrototype+i],p.candidate);}
        const auto n=d.tree.counts();d.input=actualQuery?*actualQuery:prefix.collectionInput;
        d.input.objects=d.objects.data();d.input.objectCount=d.objects.size();d.input.groups=d.groups.data();d.input.groupCount=d.groups.size();
        d.input.trees=d.methods.data();d.input.treeCount=d.methods.size();d.input.zones=d.zones.data();d.input.zoneCount=d.zones.size();
        d.input.nodes=d.tree.nodes();d.input.nodeCount=n.nodes;d.input.itemBlocks=d.tree.items();d.input.itemBlockCount=n.itemBlocks;
        d.input.pools=d.tree.pool();d.input.poolCount=1;d.input.poolBlocks=d.tree.poolBlocks();d.input.poolBlockCount=n.poolBlocks;d.input.poolEntryCount=n.poolEntries;
        d.input.fallbackZone=d.fallbackZone;d.input.initialActiveTree=*d.tree.canonicalActiveTree442E18();d.input.groupPositionScale=.46875f;
        d.input.shape0Sources=d.shapeSources.data();d.input.shape0Count=5;
        for(std::size_t i=0;i<d.objects.size();++i){
            auto& t=d.type3Candidates[i].collision;const auto& c=d.candidates[i].candidate.collision;
            if(t.filter.identity!=d.objects[i].identity)return false; // immutable cold identity admission
            t.geometry.worldMatrix=c.geometry.worldMatrix;t.sphere=c.sphere;
            t.filter.flagsF8=*d.flags[i];t.copied40=c.copied40;t.copied50=c.copied50;
            auto& state=d.type3Initial[i];state.center=d.initial.objects[i].cachedCenter;
            state.radius=d.initial.objects[i].cachedRadius;state.flagsF8=*d.flags[i];
            state.copied40=t.copied40;state.copied50=t.copied50;
        }
        return true;
    }
    VehicleCandidateResult50CA collect(const CandidateCollectionInput50BX& actualQuery){
        if(data_&&data_->sharedQueries&&!data_->sharedQueries->admitsCollection(actualQuery))return {};
        return collectImpl(&actualQuery);
    }
    VehicleCandidateResult50CA collect(const shared_query_world50JQ::RacerView& racer){
        if(!data_||!data_->sharedQueries||data_->sharedQueries->racer(racer.slot)!=&racer)return {};
        return collect(racer.collectionInput);
    }
    VehicleCandidateResult50CA collect(){return data_&&data_->sharedQueries?VehicleCandidateResult50CA{}:collectImpl(nullptr);}
private:
    VehicleCandidateResult50CA collectImpl(const CandidateCollectionInput50BX* actualQuery){
        VehicleCandidateResult50CA failed{};if(!begin(actualQuery)||!data_->tree.admit(fallback_quadtree50JK::Consumer::Collection122350))return failed;
        auto& d=*data_;CandidateCollectionWorkspace50BX scratch{d.scratch.buffer(),d.traversal.data(),d.traversal.size(),
            d.identityOrder.data(),d.identityOrder.size(),d.blockOrder.data(),d.blockOrder.size()};
        VehicleCandidateWorkspace50CA stages{scratch,d.first.buffer(),d.second.buffer(),{d.mergeScratch.data(),64},{d.merged.data(),64}};
        auto result=collectVehicleCandidates50CA(d.input,d.initial.state(),stages);
        if(result.phase!=VehicleCandidateResult50CA::Phase::Complete)return result;
        for(std::size_t i=0;i<d.objects.size();++i){*d.canonicalObjects[i]=d.second.objects[i];*d.flags[i]=d.second.objects[i].flagsF8;}
        for(std::size_t i=0;i<d.methods.size();++i)*d.canonicalMethods[i]=d.second.methods[i];
        *d.tree.canonicalPoolState()=d.second.pool;
        std::copy(d.second.blocks.begin(),d.second.blocks.end(),d.tree.canonicalPoolBlockStates());
        std::copy(d.second.entries.begin(),d.second.entries.end(),d.tree.canonicalPoolEntries());
        *d.tree.canonicalActiveTree442E18()=result.second.activeTree;
        for(std::size_t i=0;i<5;++i){d.prototypes[i].shape=d.second.shapes[i];syncPrototype(d.prototypes[i]);}
        for(std::size_t k=0;k<static_cast<std::size_t>(result.count);++k)if(!resolve(d.merged[k],d.mergedIndices[k])||
            (!d.sharedQueries&&d.mergedIndices[k]>=d.prefix->count.terrain&&d.mergedIndices[k]<d.prefixCount))return failed;
        for(std::size_t k=0;k<static_cast<std::size_t>(result.second.count);++k)if(!resolve(d.second.ids[k],d.secondIndices[k])||
            (!d.sharedQueries&&d.secondIndices[k]>=d.prefix->count.terrain&&d.secondIndices[k]<d.prefixCount))return failed;
        begin();return result;
    }
public:
    bool resolve(std::uint32_t identity,std::size_t& index)const noexcept{
        if(!data_)return false;for(std::size_t i=0;i<data_->objects.size();++i)if(data_->objects[i].identity==identity){index=i;return true;}return false;
    }
    // Immutable constructor scalar projections share the canonical prototype
    // lifetime. No AUB static-SCB assignment or admitted shape/cache writer
    // targets these five A9 bytes. Identity lookup remains strict.
    const ContactMaterial50HI* prototypeContactMaterials()const noexcept{
        return data_&&data_->tree.ready()?data_->prototypeContactMaterials.data():nullptr;
    }
    std::size_t prototypeContactMaterialCount()const noexcept{
        return data_&&data_->tree.ready()?data_->prototypeContactMaterials.size():0u;
    }
    const Type2StaticCandidate49X* candidates()const noexcept{return data_?data_->candidates.data():nullptr;}
    std::size_t candidateCount()const noexcept{return data_?data_->candidates.size():0;}
    const std::size_t* mergedIndices()const noexcept{return data_?data_->mergedIndices.data():nullptr;}
    const std::size_t* secondIndices()const noexcept{return data_?data_->secondIndices.data():nullptr;}
    VegetationExpansion50JL expansion()noexcept{if(!data_)return {};auto& d=*data_;return {d.objects.data(),d.objects.size(),d.groups.data(),d.groups.size(),.46875f,
        d.shapeSources.data(),d.rawInitial.data(),d.rawScratch.data(),d.rawDestination.data(),5};}
    bool commitRaw(const TerrainCandidateEffects48J* effects,std::size_t count){
        if(!data_||(count&&!effects))return false;auto& d=*data_;
        for(std::size_t i=0;i<count;++i){std::size_t index=0;
            if(!resolve(effects[i].identity,index)||(!d.sharedQueries&&index>=d.prefix->count.terrain&&index<d.prefixCount))return false;}
        for(std::size_t i=0;i<count;++i){std::size_t index=0;resolve(effects[i].identity,index);
            if(index>=d.prefixCount&&index<d.firstPrototype)continue;
            if(d.sharedQueries&&d.sharedQueries->object(static_cast<CollectionRef50BX>(index+1))){
                if(!d.sharedQueries->commitEffects(static_cast<CollectionRef50BX>(index+1),effects[i]))return false;continue;}
            Type2StaticCandidate49X* c=index<d.prefixCount?(index<d.prefix->count.terrain?d.prefix->candidates[index]:nullptr):(index<d.firstPrototype+5?&d.prototypes[index-d.firstPrototype].candidate:nullptr);
            if(!c)return false;auto& actual=c->candidate.collision;actual.filter.flagsF8=effects[i].flagsF8;
            actual.copied40=effects[i].copied40;actual.copied50=effects[i].copied50;d.canonicalObjects[index]->flagsF8=effects[i].flagsF8;
        }
        for(std::size_t i=0;i<5;++i){auto& p=d.prototypes[i];p.shape=d.rawDestination[i].matrices;
            p.candidate.candidate.collision.sphere=d.rawDestination[i].sphere;
            p.candidate.candidate.collision.geometry.worldMatrix=p.shape.composed00;
            for(unsigned j=0;j<3;++j)p.collection.cachedCenter.v[j]=d.rawDestination[i].sphere.center.v[j];
            p.collection.cachedCenter.v[3]=1.f;p.collection.cachedRadius=d.rawDestination[i].sphere.radius;}
        return begin();
    }
    bool borrowFull121920(const shared_query_world50JQ::RacerView& racer,Full121920Borrow50JN& out){
        if(!data_||!data_->sharedQueries||data_->sharedQueries->racer(racer.slot)!=&racer)return false;
        return borrowFullImpl(out,racer.count.movementQuery,racer.movementMembership,&racer.collectionInput);
    }
    bool borrowFull121920(Full121920Borrow50JN& out){
        if(!data_||data_->sharedQueries)return false;
        return borrowFullImpl(out,data_->prefix->count.movementQuery,data_->prefix->movementMembership,nullptr);
    }
private:
    bool borrowFullImpl(Full121920Borrow50JN& out,CollectionRef50BX motion,
        const track_runtime_storage50IG::Membership& actualMembership,const CandidateCollectionInput50BX* actualQuery){
        if(!data_||!data_->tree.admit(fallback_quadtree50JK::Consumer::FullType3Query121920)||!begin(actualQuery))return false;
        auto& d=*data_;const auto& membership=actualMembership.state;
        if(!membership.attached||membership.object!=motion||membership.lastShapeTag!=3)return false;
        out.input=d.input;out.input.query=motion;
        out.input.zoneLinks=membership.storage.links;out.input.zoneLinkCount=membership.storage.capacity;out.input.zoneHead=1;
        // Full121920 derives1202E0 bounds from its actual Type3 source, not
        // from the membership result (which owns no endpoint input).
        out.input.bounds={};out.input.type2Resource=nullptr;
        out.initial=d.initial.state();
        out.workspace={d.scratch.buffer(),d.traversal.data(),d.traversal.size(),
            d.identityOrder.data(),d.identityOrder.size(),d.blockOrder.data(),d.blockOrder.size()};out.destination=d.first.buffer();
        out.groups={d.type3Objects.data(),d.type3Objects.size(),d.type3Groups.data(),7,d.type3Shapes.data(),5,
            nullptr,0,nullptr,0,.46875f,motion,GroupQueryCallback48U::None};
        out.dispatchInitial={d.type3Initial.data(),d.type3Initial.size(),d.initial.shapes.data(),5,nullptr,0};
        out.identityOrder=d.identityOrder.data();out.identityOrderCount=d.identityOrder.size();
        return true;
    }
public:
    // Called only after the full iterator and incremental Type3 cursor report
    // Complete. Their destination arrays are proposals until this commit.
    // Collection shape rows are deliberately ignored: dispatch is authoritative.
    bool commitFull121920(const shared_query_world50JQ::RacerView& racer,CollectionRef50BX activeTree,
        const GroupQueryState48U& dispatch,Type3QueryInput48P& actualMovement){
        if(!data_||!data_->sharedQueries||data_->sharedQueries->racer(racer.slot)!=&racer)return false;
        return commitFullImpl(activeTree,dispatch,actualMovement,racer.count.movementQuery);
    }
    bool commitFull121920(CollectionRef50BX activeTree,const GroupQueryState48U& dispatch,
        Type3QueryInput48P& actualMovement){
        if(!data_||data_->sharedQueries)return false;
        return commitFullImpl(activeTree,dispatch,actualMovement,data_->prefix->count.movementQuery);
    }
private:
    bool commitFullImpl(CollectionRef50BX activeTree,const GroupQueryState48U& dispatch,
        Type3QueryInput48P& actualMovement,CollectionRef50BX motion){
        if(!data_||!data_->tree.ready())return false;auto& d=*data_;
        if(!motion||motion>d.objects.size()||activeTree>d.methods.size()||!dispatch.objects||
           dispatch.objectCapacity!=d.objects.size()||!dispatch.shapes||dispatch.shapeCapacity!=5||
           dispatch.shapes45||dispatch.shape45Capacity||actualMovement.filter.identity!=d.objects[motion-1].identity||
           d.flags[motion-1]!=&actualMovement.filter.flagsF8)return false;
        // Other query owners have no copied-vector target adapter. If a future
        // admitted consumer changes those lanes, require its real binding.
        for(std::size_t i=d.prefix->count.terrain;!d.sharedQueries&&i<d.prefixCount;++i)if(i!=motion-1&&
            (dispatch.objects[i].copied40!=d.type3Initial[i].copied40||dispatch.objects[i].copied50!=d.type3Initial[i].copied50))return false;
        if(d.sharedQueries)for(std::size_t i=0;i<d.objects.size();++i){const auto ref=static_cast<CollectionRef50BX>(i+1);
            if(d.sharedQueries->object(ref)&&!d.sharedQueries->admitsDispatch(ref,dispatch.objects[i],motion))return false;}
        for(std::size_t i=0;i<d.objects.size();++i){
            *d.canonicalObjects[i]=d.first.objects[i];const auto& state=dispatch.objects[i];
            auto& actual=*d.canonicalObjects[i];actual.flagsF8=state.flagsF8;actual.cachedCenter=state.center;actual.cachedRadius=state.radius;
            *d.flags[i]=state.flagsF8;
            if(d.sharedQueries&&d.sharedQueries->object(static_cast<CollectionRef50BX>(i+1)))
                d.sharedQueries->commitDispatch(static_cast<CollectionRef50BX>(i+1),state,motion);
            Type2StaticCandidate49X* c=i<d.prefix->count.terrain?d.prefix->candidates[i]:
                (i>=d.firstPrototype&&i<d.firstPrototype+5?&d.prototypes[i-d.firstPrototype].candidate:nullptr);
            if(c){auto& target=c->candidate.collision;target.filter.flagsF8=state.flagsF8;
                target.copied40=state.copied40;target.copied50=state.copied50;
                std::copy_n(state.center.v,3,target.sphere.center.v);target.sphere.radius=state.radius;}
        }
        for(std::size_t i=0;i<d.methods.size();++i)*d.canonicalMethods[i]=d.first.methods[i];
        *d.tree.canonicalPoolState()=d.first.pool;
        std::copy(d.first.blocks.begin(),d.first.blocks.end(),d.tree.canonicalPoolBlockStates());
        std::copy(d.first.entries.begin(),d.first.entries.end(),d.tree.canonicalPoolEntries());
        *d.tree.canonicalActiveTree442E18()=activeTree;
        for(std::size_t i=0;i<5;++i){auto& p=d.prototypes[i];p.shape=dispatch.shapes[i];
            p.candidate.candidate.collision.geometry.worldMatrix=p.shape.composed00;}
        actualMovement.copied40=dispatch.objects[motion-1].copied40;actualMovement.copied50=dispatch.objects[motion-1].copied50;
        return begin();
    }
public:
    bool bindGround(AiResetPhysicsOwners50JE& out){
        if(!begin()||!data_->tree.admit(fallback_quadtree50JK::Consumer::Ground124CB0StaticOnly))return false;
        auto& d=*data_;const auto n=d.tree.counts();out.groundWorld={d.objects.data(),d.geometry.data(),d.objects.size(),d.methods.data(),d.methods.size(),d.zones.data(),d.zones.size(),
            d.tree.nodes(),n.nodes,d.tree.items(),n.itemBlocks,d.tree.pool(),1,d.tree.poolBlocks(),n.poolBlocks,n.poolEntries};
        out.groundMutable={d.initial.objects.data(),d.objects.size(),d.initial.methods.data(),d.methods.size(),d.tree.canonicalPoolState(),1,
            d.tree.canonicalPoolBlockStates(),n.poolBlocks,d.tree.canonicalPoolEntries(),n.poolEntries,d.canonicalObjects.data(),d.objects.size(),d.canonicalMethods.data(),d.methods.size(),d.tree.canonicalActiveTree442E18()};
        out.groundWorkspace={d.scratch.objects.data(),d.objects.size(),d.scratch.methods.data(),d.methods.size(),d.meshStack.data(),d.meshStack.size(),
            &d.scratch.pool,1,d.scratch.blocks.data(),n.poolBlocks,d.scratch.entries.data(),n.poolEntries,d.traversal.data(),d.traversal.size()};
        out.collisionFlagsF8=d.flags.data();out.collisionFlagsCount=d.flags.size();out.globalFallbackZone443108=&d.fallbackZone;out.groundExtension=&d.extension;return true;
    }
};
} // namespace extended_fallback_world50JM
