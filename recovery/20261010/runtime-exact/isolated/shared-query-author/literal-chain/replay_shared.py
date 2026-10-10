from pathlib import Path
root=Path('/workspace/shared/recovery-20261010/runtime-exact/isolated/shared-query-author')
p=root/'files/tests/native/shared_query_world50jq_fixture.h'
s=(root/'literal-chain/tests/native/shared_query_world50jq_fixture.h').read_text()
# Literal replacement payloads retained from the original authoring calls.
s=s.replace('#include "gc_vegetation50jh_fixture.h"','#include "gc_vegetation50jh_fixture.h"\n#include "../../src/game/group_query48u.h"\n#include <limits>')
s=s.replace('!terrainCount||racers<2','!terrainCount||terrainCount>track_runtime_storage50IG::hardTerrainOwners||racers<2')
s=s.replace('''    static std::array<CollisionFilter48J*,5> filters''','''    static bool sourceMatches(const PodShapeSource49W& a,const PodShapeSource49W& b)noexcept{
        return !std::memcmp(&a.local40,&b.local40,sizeof(a.local40))&&
            !std::memcmp(&a.rootCenter,&b.rootCenter,sizeof(a.rootCenter))&&
            !std::memcmp(&a.rootHalf,&b.rootHalf,sizeof(a.rootHalf));
    }
    struct Span {const void* p;std::size_t n;};
    static bool overlap(Span a,Span b)noexcept{
        if(!a.p||!b.p)return false;const auto x=reinterpret_cast<std::uintptr_t>(a.p),y=reinterpret_cast<std::uintptr_t>(b.p);
        return x<=y?y-x<a.n:x-y<b.n;
    }
    static std::array<Span,10> backing(const QueryBindings& b)noexcept{
        return {{{b.query,sizeof(*b.query)},{b.qfilter,sizeof(*b.qfilter)},
            {b.secondaryQuery,sizeof(*b.secondaryQuery)},{b.secondaryFilter,sizeof(*b.secondaryFilter)},
            {b.terrain,sizeof(*b.terrain)},{b.movement,sizeof(*b.movement)},
            {b.resetQueryAdded,sizeof(*b.resetQueryAdded)},{b.podShape,sizeof(*b.podShape)},
            {b.podCopied40,sizeof(*b.podCopied40)},{b.podCopied50,sizeof(*b.podCopied50)}}};
    }
    static std::array<CollisionFilter48J*,5> filters''')
s=s.replace('''        const Membership* members[]={&v.mainMembership''','''        if(tags[0]!=tags[1]||tags[0]!=tags[2]||tags[0]!=tags[3]||tags[0]!=tags[4])return false;
        const Membership* members[]={&v.mainMembership''',1)
s=s.replace('''            const auto projection=projectCollectionObject50CF(*fs[i],sphere,tags[i],shapes[i],0x3f800000u);
            if(!projection.valid)return false;*object=projection.object;
            state->flagsF8=fs[i]->flagsF8;state->cachedCenter=projection.state.cachedCenter;state->cachedRadius=projection.state.cachedRadius;''','''            // Broadphase remains the accepted120230 membership projection.
            // Narrowphase can retain a newer source shape/cache after a partial
            // operation; do not silently overwrite membership history from it.
            CollisionSphere48J registered{};
            std::copy_n(members[i]->state.bounds.cachedCenter.v,3,registered.center.v);
            registered.radius=members[i]->state.bounds.cachedRadius;
            const auto projection=projectCollectionObject50CF(*fs[i],registered,tags[i],shapes[i],
                std::bit_cast<std::uint32_t>(members[i]->state.bounds.cachedCenter.v[3]));
            if(!projection.valid)return false;*object=projection.object;
            state->flagsF8=fs[i]->flagsF8;
            if(members[i]->state.attached){state->cachedCenter=projection.state.cachedCenter;state->cachedRadius=projection.state.cachedRadius;}''')
s=s.replace('''           layout.prefixObjects!=prefix.count.objects||layout.racerCount<2||layout.racerCount>maxRacers||''','''           layout.terrain>track_runtime_storage50IG::hardTerrainOwners||layout.prefixObjects!=layout.terrain+5||
           layout.prefixObjects!=prefix.count.objects||layout.racerCount<2||layout.racerCount>maxRacers||
           layout.fallbackObjects!=2976||layout.prototypeObjects!=5||
           layout.firstExtraRef!=layout.prefixObjects+2982||
           layout.objectCount!=layout.firstExtraRef-1+5*(layout.racerCount-1)||
           layout.terrain+5*layout.racerCount>track_runtime_storage50IG::runtimeSearchSlotLimit||
           layout.objectCount>(std::numeric_limits<CollectionRef50BX>::max()>>2)||''')
s=s.replace('''           !b.actualPod694Target->maxDepth||std::memcmp(b.podSource,&b.actualPod694Target->source,sizeof(*b.podSource)))''','''           !b.actualPod694Target->maxDepth||!sourceMatches(*b.podSource,b.actualPod694Target->source)||
           b.podHasResponseCallbackE8||b.podShape->type88!=0||
           b.query->collisionTypeTag!=b.secondaryQuery->collisionTypeTag||b.query->collisionTypeTag!=b.terrain->collisionTypeTag||
           b.query->collisionTypeTag!=b.resetQueryAdded->collisionTypeTag||b.query->collisionTypeTag!=b.movement->collisionTypeTag)''')
s=s.replace('''        const Membership* members[]={&v.mainMembership,&v.secondaryMembership,&v.terrainMembership,&v.podMembership,&v.movementMembership};
        for(unsigned i=0;i<5;++i){if''','''        if(b.podShape->registrationE0!=refs.podQuery)return fail(error,"pod694 E0 does not name actual registration");
        const auto spans=backing(b);
        for(std::size_t i=0;i<spans.size();++i)for(std::size_t j=0;j<i;++j)
            if(overlap(spans[i],spans[j]))return fail(error,"overlapping racer numeric owners");
        for(std::size_t slot2=0;slot2<data_->layout.racerCount;++slot2)if(data_->racers[slot2].bound)
            for(auto own:spans)for(auto other:backing(data_->racers[slot2].bindings))
                if(overlap(own,other))return fail(error,"cross-racer numeric backing overlap");
        const Membership* members[]={&v.mainMembership,&v.secondaryMembership,&v.terrainMembership,&v.podMembership,&v.movementMembership};
        for(unsigned i=0;i<5;++i){if''')
s=s.replace('''    bool commitEffects(CollectionRef50BX ref,const TerrainCandidateEffects48J& effect)noexcept{''','''    bool admitsDispatch(CollectionRef50BX ref,const GroupQueryObjectState48U& value,CollectionRef50BX motion)noexcept{
        std::size_t slot=0;unsigned role=0;if(!locate(ref,slot,role)||!data_->racers[slot].bound)return false;
        if(ref==motion)return role==4;
        const auto* actual=state(ref);
        return !std::memcmp(&actual->cachedCenter,&value.center,sizeof(value.center))&&
            std::bit_cast<std::uint32_t>(actual->cachedRadius)==std::bit_cast<std::uint32_t>(value.radius);
    }
    // Full121920 may prepare its own7A4 sphere. Other query-owner cache writes
    // are outside this adapter and must pass admitsDispatch before any commit.
    void commitDispatch(CollectionRef50BX ref,const GroupQueryObjectState48U& value,CollectionRef50BX motion)noexcept{
        std::size_t slot=0;unsigned role=0;if(!locate(ref,slot,role))return;auto& r=data_->racers[slot];
        TerrainCandidateEffects48J effect{filters(r.bindings)[role]->identity,value.flagsF8,value.copied40,value.copied50};
        commitEffects(ref,effect);
        if(ref==motion&&role==4){std::copy_n(value.center.v,3,r.bindings.resetQueryAdded->movementSphere.center.v);
            r.bindings.resetQueryAdded->movementSphere.radius=value.radius;}
    }
    bool commitEffects(CollectionRef50BX ref,const TerrainCandidateEffects48J& effect)noexcept{''')
# Later retained literal edit call.
s=s.replace('!b.podCopied40||!b.podCopied50||b.podCopied40==b.podCopied50||!b.podOwner9C||!b.podBodyA0||!b.podFieldsSourceBound||','!b.podCopied40||!b.podCopied50||b.podCopied40==b.podCopied50||!b.podOwner9C||!b.podBodyA0||b.podOwner9C==b.podBodyA0||!b.podFieldsSourceBound||')
s=s.replace('''        for(std::size_t slot2=0;slot2<data_->layout.racerCount;++slot2)if(data_->racers[slot2].bound)
            for(auto own:spans)for(auto other:backing(data_->racers[slot2].bindings))
                if(overlap(own,other))return fail(error,"cross-racer numeric backing overlap");''','''        for(std::size_t slot2=0;slot2<data_->layout.racerCount;++slot2)if(data_->racers[slot2].bound){
            const auto& otherBinding=data_->racers[slot2].bindings;
            if(b.podOwner9C==otherBinding.podOwner9C||b.podBodyA0==otherBinding.podBodyA0||
               b.podOwner9C==otherBinding.podBodyA0||b.podBodyA0==otherBinding.podOwner9C)
                return fail(error,"cross-racer vehicle/body identity alias");
            for(auto own:spans)for(auto other:backing(otherBinding))
                if(overlap(own,other))return fail(error,"cross-racer numeric backing overlap");
        }''')
s=s.replace('''    bool admitsDispatch(CollectionRef50BX ref,''','''    bool admitsCollection(const CandidateCollectionInput50BX& input)const noexcept{
        std::size_t slot=0;unsigned role=0;
        return locate(input.query,slot,role)&&role==0&&data_->racers[slot].bound&&
            input.type2Resource==data_->racers[slot].bindings.query->resource;
    }
    bool admitsDispatch(CollectionRef50BX ref,''')
# Resource alias literal edits.
s=s.replace('    TerrainQuery48J* terrain=nullptr; // canonical terrainOwner.metadata','    TerrainQuery48J* terrain=nullptr; // canonical terrainOwner.metadata\n    const Type2ResourceView49X* terrainResource=nullptr; // actual stable live348.resource')
s=s.replace('!b.secondaryFilter||!b.terrain||!b.movement||','!b.secondaryFilter||!b.terrain||!b.terrainResource||!b.query->resource||!b.secondaryQuery->resource||!b.movement||')
s=s.replace('''        if(b.podShape->registrationE0!=refs.podQuery)return fail(error,"pod694 E0 does not name actual registration");''','''        if(b.podShape->registrationE0!=refs.podQuery)return fail(error,"pod694 E0 does not name actual registration");
        if(v.mainMembership.state.lastType2Resource!=b.query->resource||
           v.secondaryMembership.state.lastType2Resource!=b.secondaryQuery->resource||
           v.terrainMembership.state.lastType2Resource!=b.terrainResource)
            return fail(error,"query membership does not borrow its actual stable Type2 resource");''')
# Scene-identity literal edits.
s=s.replace('!b.secondaryFilter||!b.terrain||!b.terrainResource||!b.query->resource||!b.secondaryQuery->resource||!b.movement||','''!b.secondaryFilter||!b.terrain||!b.terrainResource||!b.query->resource||!b.secondaryQuery->resource||
           !b.query->queryScene.value||!b.secondaryQuery->queryScene.value||b.query->queryScene.value==b.secondaryQuery->queryScene.value||!b.movement||''')
s=s.replace('''            const auto& otherBinding=data_->racers[slot2].bindings;
            if(b.podOwner9C''','''            const auto& otherBinding=data_->racers[slot2].bindings;
            for(auto scene:{b.query->queryScene.value,b.secondaryQuery->queryScene.value})
                if(scene==otherBinding.query->queryScene.value||scene==otherBinding.secondaryQuery->queryScene.value)
                    return fail(error,"cross-racer query scene identity alias");
            if(b.podOwner9C''')
# Final collection-admission literal edit.
old='''        return locate(input.query,slot,role)&&role==0&&data_->racers[slot].bound&&
            input.type2Resource==data_->racers[slot].bindings.query->resource;'''
new='''        if(!locate(input.query,slot,role)||role!=0||!data_->racers[slot].bound)return false;
        const auto& r=data_->racers[slot];const auto& query=*r.bindings.query;const auto& member=r.view->mainMembership.state;
        return member.attached&&member.object==input.query&&member.lastShapeTag==2&&
            input.type2Resource==query.resource&&member.lastType2Resource==query.resource&&
            input.zoneLinks==member.storage.links&&input.zoneLinkCount==member.storage.capacity&&input.zoneHead==1&&
            input.initialCurrentIdentity==query.initialQueryCurrentIdentity&&input.directTypeTag==query.collisionTypeTag&&
            input.groupTypeTag==query.groupTypeTag&&input.bounds.shapeTag==2&&
            !std::memcmp(&input.bounds.shapeWorld,&member.lastShapeWorld,sizeof(member.lastShapeWorld))&&
            !std::memcmp(&input.bounds.cachedCenter,&member.bounds.cachedCenter,sizeof(member.bounds.cachedCenter))&&
            std::bit_cast<std::uint32_t>(input.bounds.cachedRadius)==std::bit_cast<std::uint32_t>(member.bounds.cachedRadius);'''
if old not in s: raise Exception('preimage')
s=s.replace(old,new);p.write_text(s)
