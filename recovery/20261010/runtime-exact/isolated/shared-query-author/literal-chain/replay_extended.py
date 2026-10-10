from pathlib import Path
root=Path('/workspace/shared/recovery-20261010/runtime-exact/isolated/shared-query-author')
p=root/'files/tests/native/extended_fallback_world50jm_fixture.h'
s=(root/'literal-chain/tests/native/extended_fallback_world50jm_fixture.h').read_text()
# Literal replacement payloads retained from the original authoring calls.
s=s.replace('#include "track_runtime_storage50ig_fixture.h"','#include "track_runtime_storage50ig_fixture.h"\n#include "shared_query_world50jq_fixture.h"')
s=s.replace('bool actualGcEnvironmentAndLoadOrderBound=false,nativeColdQueryPoolAdmission=false;','bool actualGcEnvironmentAndLoadOrderBound=false,nativeColdQueryPoolAdmission=false;\n    // Optional canonical multi-racer namespace; never a selected-slot switch.\n    shared_query_world50JQ::Owner* sharedQueries=nullptr;')
s=s.replace('Prefix* prefix=nullptr;std::size_t prefixCount=0,firstPrototype=0;','Prefix* prefix=nullptr;std::size_t prefixCount=0,firstPrototype=0;\n        shared_query_world50JQ::Owner* sharedQueries=nullptr;')
s=s.replace('auto d=std::make_unique<Data>();d->prefix=&prefix;d->prefixCount=prefix.count.objects;','''if(b.sharedQueries&&(!b.sharedQueries->ready()||b.sharedQueries->prefix()!=&prefix||
           !b.sharedQueries->refresh()))return fail(error,"unbound canonical multi-racer query world");
        auto d=std::make_unique<Data>();d->prefix=&prefix;d->prefixCount=prefix.count.objects;d->sharedQueries=b.sharedQueries;''')
s=s.replace('d->firstPrototype=d->prefixCount+population;const auto total=d->firstPrototype+5;','''d->firstPrototype=d->prefixCount+population;
        const auto total=d->sharedQueries?d->sharedQueries->layout()->objectCount:d->firstPrototype+5;
        if(d->sharedQueries&&(d->sharedQueries->layout()->prefixObjects!=d->prefixCount||
            d->sharedQueries->layout()->firstExtraRef!=d->firstPrototype+6))
            return fail(error,"changed legacy fallback/prototype reference range");''')
needle='        for(std::size_t i=0;i<total;++i)for(std::size_t j=0;j<i;++j)'
s=s.replace(needle,'''        if(d->sharedQueries)for(std::size_t i=0;i<total;++i){const auto ref=static_cast<CollectionRef50BX>(i+1);
            if(auto* actual=d->sharedQueries->object(ref)){
                auto* candidate=d->sharedQueries->candidate(ref);auto* state=d->sharedQueries->state(ref);auto* flags=d->sharedQueries->flags(ref);
                if(!candidate||!state||!flags)return fail(error,"unbound canonical query row");
                d->objects[i]=*actual;d->candidates[i]=*candidate;d->canonicalObjects[i]=state;d->flags[i]=flags;
                d->geometry[i]={};if(d->objects[i].shapeTag==0)d->geometry[i].shape0=&candidate->candidate.collision.geometry;
            }
        }
'''+needle)
s=s.replace('d->extension={&prefix.lifecycle,d->methods.data(),d->zones.data(),total,d->methods.size(),d->zones.size(),&d->fallbackZone};','d->extension={&prefix.lifecycle,d->methods.data(),d->zones.data(),total,d->methods.size(),d->zones.size(),&d->fallbackZone,d->sharedQueries!=nullptr};')
s=s.replace('    bool begin(){\n        if(!data_||!data_->tree.ready())return false;auto& d=*data_;auto& prefix=*d.prefix;','''    bool begin(const CandidateCollectionInput50BX* actualQuery=nullptr){
        if(!data_||!data_->tree.ready())return false;auto& d=*data_;auto& prefix=*d.prefix;
        if(d.sharedQueries&&!d.sharedQueries->refresh())return false;''')
s=s.replace('        for(std::size_t i=0;i<d.objects.size();++i){d.initial.objects[i]=*d.canonicalObjects[i];d.initial.objects[i].flagsF8=*d.flags[i];}', '''        if(d.sharedQueries)for(std::size_t i=0;i<d.objects.size();++i){const auto ref=static_cast<CollectionRef50BX>(i+1);
            if(auto* actual=d.sharedQueries->object(ref)){d.objects[i]=*actual;copyCandidateView(d.candidates[i],*d.sharedQueries->candidate(ref));}}
        for(std::size_t i=0;i<d.objects.size();++i){d.initial.objects[i]=*d.canonicalObjects[i];d.initial.objects[i].flagsF8=*d.flags[i];}''')
s=s.replace('const auto n=d.tree.counts();d.input=prefix.collectionInput;','const auto n=d.tree.counts();d.input=actualQuery?*actualQuery:prefix.collectionInput;')
s=s.replace('    VehicleCandidateResult50CA collect(){\n        VehicleCandidateResult50CA failed{};if(!begin()||!data_->tree.admit', '''    VehicleCandidateResult50CA collect(const CandidateCollectionInput50BX& actualQuery){return collectImpl(&actualQuery);}
    VehicleCandidateResult50CA collect(){return collectImpl(nullptr);}
private:
    VehicleCandidateResult50CA collectImpl(const CandidateCollectionInput50BX* actualQuery){
        VehicleCandidateResult50CA failed{};if(!begin(actualQuery)||!data_->tree.admit''')
s=s.replace('(d.mergedIndices[k]>=d.prefix->count.terrain&&d.mergedIndices[k]<d.prefixCount))','(!d.sharedQueries&&d.mergedIndices[k]>=d.prefix->count.terrain&&d.mergedIndices[k]<d.prefixCount))')
s=s.replace('(d.secondIndices[k]>=d.prefix->count.terrain&&d.secondIndices[k]<d.prefixCount))','(!d.sharedQueries&&d.secondIndices[k]>=d.prefix->count.terrain&&d.secondIndices[k]<d.prefixCount))')
s=s.replace('    bool resolve(std::uint32_t identity', 'public:\n    bool resolve(std::uint32_t identity')
s=s.replace('if(!resolve(effects[i].identity,index)||(index>=d.prefix->count.terrain&&index<d.prefixCount))return false;','if(!resolve(effects[i].identity,index)||(!d.sharedQueries&&index>=d.prefix->count.terrain&&index<d.prefixCount))return false;')
s=s.replace('            if(index>=d.prefixCount&&index<d.firstPrototype)continue;\n            Type2StaticCandidate49X* c=', '''            if(index>=d.prefixCount&&index<d.firstPrototype)continue;
            if(d.sharedQueries&&d.sharedQueries->object(static_cast<CollectionRef50BX>(index+1))){
                if(!d.sharedQueries->commitEffects(static_cast<CollectionRef50BX>(index+1),effects[i]))return false;continue;}
            Type2StaticCandidate49X* c=''')
s=s.replace(':&d.prototypes[index-d.firstPrototype].candidate;',':(index<d.firstPrototype+5?&d.prototypes[index-d.firstPrototype].candidate:nullptr);')
s=s.replace('    bool borrowFull121920(Full121920Borrow50JN& out){\n        if(!data_', '''    bool borrowFull121920(const shared_query_world50JQ::RacerView& racer,Full121920Borrow50JN& out){
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
        if(!data_''')
s=s.replace('Consumer::FullType3Query121920)||!begin())return false;\n        auto& d=*data_;const auto& membership=d.prefix->movementMembership.state;\n        if(!membership.attached||membership.object!=d.prefix->count.movementQuery||membership.lastShapeTag!=3)return false;\n        out.input=d.input;out.input.query=d.prefix->count.movementQuery;', '''Consumer::FullType3Query121920)||!begin(actualQuery))return false;
        auto& d=*data_;const auto& membership=actualMembership.state;
        if(!membership.attached||membership.object!=motion||membership.lastShapeTag!=3)return false;
        out.input=d.input;out.input.query=motion;''')
s=s.replace('nullptr,0,nullptr,0,.46875f,d.prefix->count.movementQuery,GroupQueryCallback48U::None};','nullptr,0,nullptr,0,.46875f,motion,GroupQueryCallback48U::None};')
s=s.replace('    // Called only after the full iterator', '''public:
    // Called only after the full iterator''')
s=s.replace('''    bool commitFull121920(CollectionRef50BX activeTree,const GroupQueryState48U& dispatch,
        Type3QueryInput48P& actualMovement){
        if(!data_||!data_->tree.ready())return false;auto& d=*data_;const auto motion=d.prefix->count.movementQuery;
        if(!motion||motion>d.prefixCount''','''    bool commitFull121920(const shared_query_world50JQ::RacerView& racer,CollectionRef50BX activeTree,
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
        if(!motion||motion>d.objects.size()''')
s=s.replace('for(std::size_t i=d.prefix->count.terrain;i<d.prefixCount;++i)if(i!=motion-1&&','for(std::size_t i=d.prefix->count.terrain;!d.sharedQueries&&i<d.prefixCount;++i)if(i!=motion-1&&')
s=s.replace('''        for(std::size_t i=0;i<d.objects.size();++i){
            *d.canonicalObjects[i]=d.first.objects[i];const auto& state=dispatch.objects[i];''','''        if(d.sharedQueries)for(std::size_t i=0;i<d.objects.size();++i){const auto ref=static_cast<CollectionRef50BX>(i+1);
            if(d.sharedQueries->object(ref)&&!d.sharedQueries->admitsDispatch(ref,dispatch.objects[i],motion))return false;}
        for(std::size_t i=0;i<d.objects.size();++i){
            *d.canonicalObjects[i]=d.first.objects[i];const auto& state=dispatch.objects[i];''')
s=s.replace('(i>=d.firstPrototype?&d.prototypes[i-d.firstPrototype].candidate:nullptr);','(i>=d.firstPrototype&&i<d.firstPrototype+5?&d.prototypes[i-d.firstPrototype].candidate:nullptr);')
s=s.replace('''            *d.flags[i]=state.flagsF8;
            Type2StaticCandidate49X* c''','''            *d.flags[i]=state.flagsF8;
            if(d.sharedQueries&&d.sharedQueries->object(static_cast<CollectionRef50BX>(i+1)))
                d.sharedQueries->commitDispatch(static_cast<CollectionRef50BX>(i+1),state,motion);
            Type2StaticCandidate49X* c''')
s=s.replace('    bool bindGround(AiResetPhysicsOwners50JE& out){','public:\n    bool bindGround(AiResetPhysicsOwners50JE& out){')
# Later retained literal edit call.
s=s.replace('if(data_||!vegetation.ready()||prefix.count.objects!=prefix.count.terrain+5||','if(data_||!vegetation.ready()||prefix.canonicalShared50JQ!=(b.sharedQueries!=nullptr)||prefix.count.objects!=prefix.count.terrain+5||')
s=s.replace('VehicleCandidateResult50CA collect(){return collectImpl(nullptr);}','VehicleCandidateResult50CA collect(){return data_&&data_->sharedQueries?VehicleCandidateResult50CA{}:collectImpl(nullptr);}')
s=s.replace('''    VehicleCandidateResult50CA collect(const CandidateCollectionInput50BX& actualQuery){return collectImpl(&actualQuery);}''','''    VehicleCandidateResult50CA collect(const CandidateCollectionInput50BX& actualQuery){
        if(data_&&data_->sharedQueries&&!data_->sharedQueries->admitsCollection(actualQuery))return {};
        return collectImpl(&actualQuery);
    }
    VehicleCandidateResult50CA collect(const shared_query_world50JQ::RacerView& racer){
        if(!data_||!data_->sharedQueries||data_->sharedQueries->racer(racer.slot)!=&racer)return {};
        return collect(racer.collectionInput);
    }''')
p.write_text(s)
