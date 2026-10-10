#pragma once
#include "native_ai_reset_queries50je_fixture.h"
#include "native_racer_collision50hw_fixture.h"
#include "gc_vegetation50jh_fixture.h"
#include <array>
#include <bit>
#include <cstring>
#include <memory>
#include <string>

// One canonical4C4 List world, with one genuine five-query set per racer.
// Original4C0 visibility remains the separately owned ai_reset_visibility50JE
// world. This schema does not register wreckage, invent dynamic callbacks or
// replace the actual fallback QuadTree with a second collection.
namespace shared_query_world50JQ {
using namespace rr::game;
using Storage=track_runtime_storage50IG::Storage;
using Counts=track_runtime_storage50IG::Counts;
using Membership=track_runtime_storage50IG::Membership;
inline constexpr std::size_t maxRacers=8,queriesPerRacer=5;
struct References {
    CollectionRef50BX mainQuery=0,secondaryQuery=0,terrainQuery=0,podQuery=0,movementQuery=0;
    CollectionRef50BX operator[](std::size_t i)const noexcept{
        const CollectionRef50BX r[]={mainQuery,secondaryQuery,terrainQuery,podQuery,movementQuery};
        return i<queriesPerRacer?r[i]:0;
    }
};
struct Layout {
    std::size_t terrain=0,prefixObjects=0,fallbackObjects=0,prototypeObjects=0,
        firstExtraRef=0,objectCount=0,racerCount=0;
    bool authoredGcBound=false;
    References refs(std::size_t slot)const noexcept{
        if(!authoredGcBound||slot>=racerCount)return {};
        const auto first=static_cast<CollectionRef50BX>(slot?firstExtraRef+(slot-1)*5:terrain+1);
        return {first,first+1,first+2,first+3,first+4};
    }
    // Bind the exact already-authenticated population. Ref holes belong to the
    // actual fallback/prototypes and are not extra List objects or empty trees.
    static bool fromGc(std::size_t terrainCount,std::size_t racers,
        const gc_vegetation50JH::Owner& vegetation,Layout& out)noexcept{
        if(!vegetation.ready()||vegetation.definitions().size()!=7||vegetation.instances().size()!=2974||
           !terrainCount||racers<2||racers>maxRacers||terrainCount+5*racers>track_runtime_storage50IG::runtimeSearchSlotLimit)return false;
        std::size_t prototypes=0;for(const auto& d:vegetation.definitions()){
            if(d.collisions.size()>1)return false;prototypes+=d.collisions.size();}
        if(prototypes!=5)return false;
        Layout next{};next.terrain=terrainCount;next.prefixObjects=terrainCount+5;
        next.fallbackObjects=vegetation.instances().size()+2;next.prototypeObjects=prototypes;
        next.firstExtraRef=next.prefixObjects+next.fallbackObjects+prototypes+1;
        next.objectCount=next.firstExtraRef-1+5*(racers-1);next.racerCount=racers;next.authoredGcBound=true;
        out=next;return true;
    }
};
struct QueryBindings {
    Type2StaticQuery49X* query=nullptr;
    CollisionFilter48J* qfilter=nullptr;
    Type2StaticQuery49X* secondaryQuery=nullptr;
    CollisionFilter48J* secondaryFilter=nullptr;
    TerrainQuery48J* terrain=nullptr; // canonical terrainOwner.metadata
    Type3QueryInput48P* movement=nullptr;
    native_ai_reset_queries50JE::AddedOwners* resetQueryAdded=nullptr;
    PodShapeState49W* podShape=nullptr; // canonical frame.shape
    const PodShapeSource49W* podSource=nullptr; // canonical parameters.shape
    const native_racer_collision50HW::Target* actualPod694Target=nullptr;
    std::array<float,4>* podCopied40=nullptr;
    std::array<float,4>* podCopied50=nullptr;
    // Actual21224C/212258 object identities, in the caller's unique session
    // namespace. CallbackE8 must follow its exact factory/runtime writer proof.
    std::uint32_t podOwner9C=0,podBodyA0=0;
    bool podFieldsSourceBound=false,podHasResponseCallbackE8=false;
};
struct RacerView {
    const std::size_t slot;
    const Counts count;
    track_runtime_storage50IG::FixedBuffer<Type2StaticCandidate49X*>& candidates;
    ZoneLifecycle50CL& lifecycle;
    LiveZoneWorld50EN& world;
    LiveZoneWorkspace50EN& membershipWork;
    Membership& mainMembership;
    Membership& secondaryMembership;
    Membership& terrainMembership;
    Membership& podMembership;
    Membership& movementMembership;
    CandidateCollectionInput50BX collectionInput{}; // explicit per-call source query descriptor
private:
    friend class Owner;
    Storage* prefix_=nullptr;
    std::array<CollectionObject50BX*,5> objects_{};
    std::array<CollectionObjectState50BX*,5> states_{};
    RacerView(std::size_t s,Counts c,Storage& p,Membership& a,Membership& b,Membership& t,Membership& pod,Membership& move):
        slot(s),count(c),candidates(p.candidates),lifecycle(p.lifecycle),world(p.world),membershipWork(p.membershipWork),
        mainMembership(a),secondaryMembership(b),terrainMembership(t),podMembership(pod),movementMembership(move),prefix_(&p){}
public:
    RacerView(const RacerView&)=delete;RacerView& operator=(const RacerView&)=delete;
    CollectionObject50BX* collectionObject(CollectionRef50BX ref)noexcept{
        const CollectionRef50BX refs[]={count.mainQuery,count.secondaryQuery,count.terrainQuery,count.podQuery,count.movementQuery};
        for(unsigned i=0;i<5;++i)if(ref==refs[i])return objects_[i];
        return ref&&ref<=count.terrain?prefix_->collectionObject(ref):nullptr;
    }
    CollectionObjectState50BX* collectionState(CollectionRef50BX ref)noexcept{
        const CollectionRef50BX refs[]={count.mainQuery,count.secondaryQuery,count.terrainQuery,count.podQuery,count.movementQuery};
        for(unsigned i=0;i<5;++i)if(ref==refs[i])return states_[i];
        return ref&&ref<=count.terrain?prefix_->collectionState(ref):nullptr;
    }
    const CollectionObject50BX* collectionObject(CollectionRef50BX ref)const noexcept{return const_cast<RacerView*>(this)->collectionObject(ref);}
    const CollectionObjectState50BX* collectionState(CollectionRef50BX ref)const noexcept{return const_cast<RacerView*>(this)->collectionState(ref);}
};
class Owner final {
    struct Extra {
        Membership main,secondary,terrain,pod,movement;
        std::array<CollectionObject50BX,5> objects{};
        std::array<CollectionObjectState50BX,5> states{};
        explicit Extra(std::size_t n):main(n),secondary(n),terrain(n),pod(n),movement(n){}
    };
    struct Racer {
        std::unique_ptr<Extra> extra;
        std::unique_ptr<RacerView> view;
        QueryBindings bindings{};
        std::array<Type2StaticCandidate49X,5> candidates{};
        bool bound=false;
    };
    struct Data {Storage* prefix=nullptr;Layout layout{};std::array<Racer,maxRacers> racers;};
    std::unique_ptr<Data> data_;
    static bool fail(std::string& e,const char* message){e=message;return false;}
    static std::array<CollisionFilter48J*,5> filters(const QueryBindings& b)noexcept{
        return {{b.qfilter,b.secondaryFilter,b.terrain?&b.terrain->filter:nullptr,
            b.resetQueryAdded?&b.resetQueryAdded->podFilter:nullptr,b.movement?&b.movement->filter:nullptr}};
    }
    static bool immutableFilterMatches(const CollisionFilter48J& a,const CollisionFilter48J& b)noexcept{
        if(a.identity!=b.identity||a.categoryA4!=b.categoryA4||a.acceptMaskB8!=b.acceptMaskB8||a.flags31!=b.flags31||
           a.categoryModeD4!=b.categoryModeD4||a.categoryMasks!=b.categoryMasks)return false;
        for(unsigned i=0;i<2;++i)if(a.identityLists[i].mode!=b.identityLists[i].mode||a.identityLists[i].identities!=b.identityLists[i].identities)return false;
        return true;
    }
    static void copied(const QueryBindings& b,unsigned i,std::array<float,4>*& a,std::array<float,4>*& z)noexcept{
        switch(i){case 0:a=&b.query->copied40;z=&b.query->copied50;break;
        case 1:a=&b.secondaryQuery->copied40;z=&b.secondaryQuery->copied50;break;
        case 2:a=&b.terrain->copied40;z=&b.terrain->copied50;break;
        case 3:a=b.podCopied40;z=b.podCopied50;break;
        default:a=&b.movement->copied40;z=&b.movement->copied50;break;}
    }
    bool refreshRacer(Racer& r)noexcept{
        if(!r.bound)return false;auto& v=*r.view;const auto& b=r.bindings;const auto fs=filters(b);
        const References refs{v.count.mainQuery,v.count.secondaryQuery,v.count.terrainQuery,v.count.podQuery,v.count.movementQuery};
        const std::uint32_t shapes[]={2,2,2,0,3};
        const std::uint8_t tags[]={b.query->collisionTypeTag,b.secondaryQuery->collisionTypeTag,b.terrain->collisionTypeTag,
            b.resetQueryAdded->collisionTypeTag,b.movement->collisionTypeTag};
        const Membership* members[]={&v.mainMembership,&v.secondaryMembership,&v.terrainMembership,&v.podMembership,&v.movementMembership};
        for(unsigned i=0;i<5;++i){auto* object=v.collectionObject(refs[i]);auto* state=v.collectionState(refs[i]);
            if(!object||!state||!immutableFilterMatches(r.candidates[i].candidate.collision.filter,*fs[i]))return false;
            auto& target=r.candidates[i].candidate.collision;CollisionSphere48J sphere=target.sphere;
            if(i==0)sphere=b.query->sphere;else if(i==1)sphere=b.secondaryQuery->sphere;
            else if(i==3){std::copy_n(b.podShape->center60.v,3,sphere.center.v);sphere.radius=b.podShape->radius80;}
            else if(i==4)sphere=b.resetQueryAdded->movementSphere;
            else if(members[i]->state.attached){std::copy_n(members[i]->state.bounds.cachedCenter.v,3,sphere.center.v);sphere.radius=members[i]->state.bounds.cachedRadius;}
            if(members[i]->state.attached&&(members[i]->state.object!=refs[i]||members[i]->state.lastShapeTag!=shapes[i]))return false;
            const auto projection=projectCollectionObject50CF(*fs[i],sphere,tags[i],shapes[i],0x3f800000u);
            if(!projection.valid)return false;*object=projection.object;
            state->flagsF8=fs[i]->flagsF8;state->cachedCenter=projection.state.cachedCenter;state->cachedRadius=projection.state.cachedRadius;
            target.filter.flagsF8=fs[i]->flagsF8;target.sphere=sphere;
            std::array<float,4>* a=nullptr;std::array<float,4>* z=nullptr;copied(b,i,a,z);target.copied40=*a;target.copied50=*z;
            if(i==3)target.geometry.worldMatrix=b.podShape->composed00;
        }
        return true;
    }
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    bool prepare(Storage& prefix,const Layout& layout,std::string& error){
        if(data_||prefix.canonicalShared50JQ||!layout.authoredGcBound||layout.terrain!=prefix.count.terrain||
           layout.prefixObjects!=prefix.count.objects||layout.racerCount<2||layout.racerCount>maxRacers||
           prefix.lifecycle.objectCount!=layout.objectCount||prefix.registrations.size()!=layout.objectCount)
            return fail(error,"shared query world requires its complete cold registration namespace");
        const auto r0=layout.refs(0);
        if(prefix.count.mainQuery!=r0.mainQuery||prefix.count.secondaryQuery!=r0.secondaryQuery||
           prefix.count.terrainQuery!=r0.terrainQuery||prefix.count.podQuery!=r0.podQuery||prefix.count.movementQuery!=r0.movementQuery)
            return fail(error,"shared query prefix references changed");
        auto d=std::make_unique<Data>();d->prefix=&prefix;d->layout=layout;
        for(std::size_t slot=0;slot<layout.racerCount;++slot){auto& r=d->racers[slot];const auto refs=layout.refs(slot);Counts c=prefix.count;
            c.objects=c.registrationObjects=layout.objectCount;c.mainQuery=refs.mainQuery;c.secondaryQuery=refs.secondaryQuery;
            c.terrainQuery=refs.terrainQuery;c.podQuery=refs.podQuery;c.movementQuery=refs.movementQuery;
            if(slot){r.extra=std::make_unique<Extra>(prefix.count.membership);auto& e=*r.extra;
                r.view=std::unique_ptr<RacerView>(new RacerView(slot,c,prefix,e.main,e.secondary,e.terrain,e.pod,e.movement));
                for(unsigned i=0;i<5;++i){r.view->objects_[i]=&e.objects[i];r.view->states_[i]=&e.states[i];}}
            else{r.view=std::unique_ptr<RacerView>(new RacerView(slot,c,prefix,prefix.mainMembership,prefix.secondaryMembership,
                prefix.terrainMembership,prefix.podMembership,prefix.movementMembership));
                for(unsigned i=0;i<5;++i){r.view->objects_[i]=prefix.collectionObject(refs[i]);r.view->states_[i]=prefix.collectionState(refs[i]);}}
            r.view->collectionInput=prefix.collectionInput;r.view->collectionInput.query=refs.mainQuery;
        }
        prefix.canonicalShared50JQ=true;data_=std::move(d);error.clear();return true;
    }
    const Layout* layout()const noexcept{return data_?&data_->layout:nullptr;}
    Storage* prefix()const noexcept{return data_?data_->prefix:nullptr;}
    RacerView* racer(std::size_t slot)noexcept{return data_&&slot<data_->layout.racerCount?data_->racers[slot].view.get():nullptr;}
    const RacerView* racer(std::size_t slot)const noexcept{return const_cast<Owner*>(this)->racer(slot);}
    bool bindRacer(std::size_t slot,const QueryBindings& b,std::string& error){
        if(!data_||slot>=data_->layout.racerCount||data_->racers[slot].bound||!b.query||!b.qfilter||!b.secondaryQuery||
           !b.secondaryFilter||!b.terrain||!b.movement||!b.resetQueryAdded||!b.podShape||!b.podSource||!b.actualPod694Target||
           !b.podCopied40||!b.podCopied50||b.podCopied40==b.podCopied50||!b.podOwner9C||!b.podBodyA0||!b.podFieldsSourceBound||
           !b.resetQueryAdded->admitsFilterReads(0x3fu,0x01u)||b.query->filter!=b.qfilter||b.secondaryQuery->filter!=b.secondaryFilter||
           b.actualPod694Target->mesh.nodes.empty()||b.actualPod694Target->radii.size()!=b.actualPod694Target->mesh.nodes.size()||
           !b.actualPod694Target->maxDepth||std::memcmp(b.podSource,&b.actualPod694Target->source,sizeof(*b.podSource)))
            return fail(error,"unbound actual per-racer query owners or pod geometry");
        auto& r=data_->racers[slot];auto& v=*r.view;const auto fs=filters(b);const auto refs=data_->layout.refs(slot);
        const Membership* members[]={&v.mainMembership,&v.secondaryMembership,&v.terrainMembership,&v.podMembership,&v.movementMembership};
        for(unsigned i=0;i<5;++i){if(!fs[i]||!fs[i]->identity||!members[i]->state.attached||members[i]->state.object!=refs[i])
                return fail(error,"query binding precedes actual source attachment");
            for(unsigned j=0;j<i;++j)if(fs[i]==fs[j]||fs[i]->identity==fs[j]->identity)return fail(error,"aliased racer query identity");
            for(std::size_t k=0;k<data_->prefix->count.terrain;++k)if(fs[i]->identity==data_->prefix->objects[k].identity)
                return fail(error,"query identity aliases actual terrain");
            for(std::size_t s=0;s<data_->layout.racerCount;++s)if(data_->racers[s].bound){const auto others=filters(data_->racers[s].bindings);
                for(auto* other:others)if(fs[i]==other||fs[i]->identity==other->identity)return fail(error,"cross-racer query identity/backing alias");}
        }
        // Cold copies reserve the immutable filter vectors once. Hot refresh
        // rejects an unrepresented later list mutation rather than allocating.
        const std::uint32_t shapes[]={2,2,2,0,3};
        for(unsigned i=0;i<5;++i){auto& c=r.candidates[i];c.shapeType=shapes[i];auto& t=c.candidate.collision;
            t.filter=*fs[i];t.geometry.id=fs[i]->identity;t.shapeType=shapes[i];
            t.typeTag=b.query->collisionTypeTag; // all five authenticated CollisionObject tokens
            const auto& membership=members[i]->state;std::copy_n(membership.bounds.cachedCenter.v,3,t.sphere.center.v);t.sphere.radius=membership.bounds.cachedRadius;
        }
        auto& pod=r.candidates[3];pod.candidate.collision.geometry.mesh=&b.actualPod694Target->mesh;
        pod.nodeRadii=b.actualPod694Target->radii.data();pod.nodeRadiusCount=b.actualPod694Target->radii.size();pod.maxDepth=b.actualPod694Target->maxDepth;
        pod.candidate.collision.ownerIdentity9C=b.podOwner9C;pod.candidate.collision.hasResponseCallbackE8=b.podHasResponseCallbackE8;
        r.bindings=b;r.bound=true;if(!refreshRacer(r)){r.bound=false;return fail(error,"actual query projection rejected");}
        error.clear();return true;
    }
    bool ready()const noexcept{if(!data_)return false;for(std::size_t s=0;s<data_->layout.racerCount;++s)if(!data_->racers[s].bound)return false;return true;}
    bool refresh()noexcept{if(!ready())return false;for(std::size_t s=0;s<data_->layout.racerCount;++s)if(!refreshRacer(data_->racers[s]))return false;return true;}
    bool locate(CollectionRef50BX ref,std::size_t& slot,unsigned& role)const noexcept{
        if(!data_)return false;for(std::size_t s=0;s<data_->layout.racerCount;++s){const auto refs=data_->layout.refs(s);
            for(unsigned i=0;i<5;++i)if(refs[i]==ref){slot=s;role=i;return true;}}return false;
    }
    CollectionObject50BX* object(CollectionRef50BX ref)noexcept{std::size_t s=0;unsigned i=0;return locate(ref,s,i)?data_->racers[s].view->collectionObject(ref):nullptr;}
    CollectionObjectState50BX* state(CollectionRef50BX ref)noexcept{std::size_t s=0;unsigned i=0;return locate(ref,s,i)?data_->racers[s].view->collectionState(ref):nullptr;}
    Type2StaticCandidate49X* candidate(CollectionRef50BX ref)noexcept{std::size_t s=0;unsigned i=0;return locate(ref,s,i)&&data_->racers[s].bound?&data_->racers[s].candidates[i]:nullptr;}
    std::uint8_t* flags(CollectionRef50BX ref)noexcept{std::size_t s=0;unsigned i=0;if(!locate(ref,s,i)||!data_->racers[s].bound)return nullptr;return &filters(data_->racers[s].bindings)[i]->flagsF8;}
    bool commitEffects(CollectionRef50BX ref,const TerrainCandidateEffects48J& effect)noexcept{
        std::size_t s=0;unsigned i=0;if(!locate(ref,s,i)||!data_->racers[s].bound)return false;auto& r=data_->racers[s];const auto fs=filters(r.bindings);
        if(effect.identity!=fs[i]->identity)return false;std::array<float,4>* a=nullptr;std::array<float,4>* z=nullptr;copied(r.bindings,i,a,z);
        fs[i]->flagsF8=effect.flagsF8;*a=effect.copied40;*z=effect.copied50;r.view->states_[i]->flagsF8=effect.flagsF8;return true;
    }
};
} //namespace shared_query_world50JQ
