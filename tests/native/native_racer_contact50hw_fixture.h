#pragma once
// Heterogeneous extension of the HOST-ORDERED50HT main-body contact seam.
// Each PRIMARY's original type2 rays query the OTHER's original type0 mesh.
// Source ray/tree/record ordering and the294BB0 processed-body gate remain;
// ordered pairs are supplied verbatim by the host, with no sorting/deduplication.
// This is still a world-frame host join, not recovered all-body scheduling,
// collision-damage ownership, or secondary/chariot response scheduling.
#include "native_racer_collision50hw_fixture.h"
#include "native_racer_contact50ht_fixture.h"
#include <algorithm>
#include <limits>

namespace native_racer_contact50HW {
using namespace rr::game;
using native_racer_contact50HT::Pair;
using native_racer_contact50HT::Body;
using native_racer_contact50HT::Receipt;
using native_racer_contact50HT::Status;
using native_racer_contact50HT::ProcessedList;
inline constexpr auto maxRacers=native_racer_contact50HT::maxRacers;
inline constexpr auto emptyPhysicalRootMaterial=native_racer_contact50HT::emptyPhysicalRootMaterial;
struct Binding {
    const native_racer_collision50HW::Owner* resources=nullptr;
    SpringGeneratedParameters48K settings{};
    SpringResponseParameters48L response{};
};
class Owner {
public:
    Owner()=default;
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    // Cold only. All resource owners must remain alive and immutable. Parameter
    // copies are per-racer; no borrowed pointer into a changing bank arena is kept.
    bool prepare(const Binding* binding,std::size_t count,std::string& error) {
        if(ready_||!binding||count<2||count>maxRacers)
            return fail(error,"invalid fresh heterogeneous contact binding");
        std::size_t rayCapacity=0,queryDepth=0,targetDepth=0;
        for(std::size_t i=0;i<count;++i){
            if(!binding[i].resources||!binding[i].resources->prepared()||binding[i].settings.pointContacts48)
                return fail(error,"unprepared per-racer contact resources/settings");
            const auto& resource=binding[i].resources->main();
            rayCapacity=std::max(rayCapacity,resource.rayCount);
            queryDepth=std::max(queryDepth,resource.maxDepth);
            targetDepth=std::max(targetDepth,binding[i].resources->mainTarget().maxDepth);
        }
        // Resource decoder maxima bound both sums; no runtime resizing occurs.
        emptyPairs_.resize(rayCapacity);pairs_.resize(rayCapacity);scratchPairs_.resize(rayCapacity);
        emptyMetadata_.resize(rayCapacity);metadata_.resize(rayCapacity);scratchMetadata_.resize(rayCapacity);
        stack_.resize(1+queryDepth+targetDepth);
        for(std::size_t i=0;i<count;++i){resources_[i]=binding[i].resources;
            settings_[i]=binding[i].settings;response_[i]=binding[i].response;}
        queryFilter_.flagsF8=2;queryFilter_.flags31=3;
        queryFilter_.categoryA4=2;queryFilter_.acceptMaskB8=2;
        auto& c=candidate_.candidate.collision;
        c.filter.flagsF8=4;c.filter.flags31=3;c.filter.categoryA4=2;c.typeTag=1;
        count_=count;ready_=true;error.clear();return true;
    }
    std::size_t count()const noexcept{return count_;}
    std::size_t rays()const noexcept{return pairs_.size();}
    std::size_t stackCapacity()const noexcept{return stack_.size();}
    // Hot, no allocations. Input and output must be separate arrays of at least
    // count live Body objects, neither owned by this Owner; receipt is separate.
    // Both body results commit together only after every pair succeeds. Pair
    // order is consumed verbatim. Reverse pairs are suppressed ONLY after an
    // actual prior geometry hit, exactly as the source dynamic callback gates.
    // Each pair's one-candidate query preserves all ray/tree/pair ordering.
    // Each directed pair uses its PRIMARY's own authored spring parameters.
    // The OTHER supplies its own physical parameters, target mesh and material.
    Status run(const Body* input,std::size_t count,const Pair* order,std::size_t pairCount,
               Body* output,Receipt& receipt) noexcept {
        if(!ready_)return Status::Unprepared;
        if(!input||!output||count!=count_||count<2||count>maxRacers||
           pairCount>maxRacers*(maxRacers-1)||(pairCount&&!order))
            return Status::InvalidInput;
        if(!disjoint(input,count,order,pairCount,output,receipt))return Status::InvalidInput;
        for(std::size_t i=0;i<count;++i){
            if(!input[i].physical.identity||input[i].material>=settings_[i].frictionByMaterial.size())return Status::InvalidInput;
            for(std::size_t j=0;j<i;++j)if(input[i].physical.identity==input[j].physical.identity)return Status::InvalidInput;
        }
        for(std::size_t i=0;i<pairCount;++i)
            if(order[i].primary>=count||order[i].other>=count||order[i].primary==order[i].other)return Status::InvalidInput;
        std::array<Body,maxRacers> staged{};
        std::memcpy(staged.data(),input,count*sizeof(Body));
        std::array<ProcessedList,maxRacers> processed{};Receipt result{};
        result.normalModeWrite.bound=true; // prepared main690 query owners, including zero-pair pass
        for(std::size_t p=0;p<pairCount;++p){
            const auto a=order[p].primary,b=order[p].other;
            auto& primary=staged[a].physical;auto& other=staged[b].physical;
            const auto& resource=resources_[a]->main();
            const auto& targetResource=resources_[b]->mainTarget();
            const auto& settings=settings_[a];const auto& response=response_[a];
            ++result.testedPairs;
            QuerySpherePrepareInput48O sphereInput{};sphereInput.preparedScene=primary.body.state.scene;
            sphereInput.hasPairTree=true;
            const auto& root=resource.nodes[resource.root];
            for(unsigned k=0;k<3;++k){sphereInput.treeCenter.v[k]=root.center[k];sphereInput.treeHalf.v[k]=root.half[k];}
            QuerySpherePrepareOutput48O sphere{};
            if(prepareQuerySphere48O(sphereInput,sphere)!=QuerySpherePrepareStatus48O::Ready)return Status::GeometryRejected;
            PodShapeState49W target{};target.registrationE0=0;
            if(!transformPodShape49W(targetResource.source,other.body.state.scene,target))return Status::GeometryRejected;
            auto& c=candidate_.candidate.collision;
            queryFilter_.identity=primary.identity;c.filter.identity=other.identity;
            c.geometry.id=other.identity;c.geometry.worldMatrix=target.composed00;
            c.geometry.mesh=&targetResource.mesh;
            candidate_.nodeRadii=targetResource.radii.data();
            candidate_.nodeRadiusCount=targetResource.radii.size();
            candidate_.maxDepth=targetResource.maxDepth;
            for(unsigned k=0;k<3;++k)c.sphere.center.v[k]=target.center60.v[k];c.sphere.radius=target.radius80;
            Type2StaticQuery49X query{};query.resource=&resource;query.filter=&queryFilter_;
            query.worldMatrix=sphere.world;query.sphere.radius=sphere.radius;
            for(unsigned k=0;k<3;++k)query.sphere.center.v[k]=sphere.center.v[k];
            query.queryScene={primary.identity};query.collisionTypeTag=1;query.groupTypeTag=2;
            query.callbackEnabled=false;
            query.normalMode54=0u;query.normalMode54Bound=true; // authentic main690 constructor21219C/11BBEC
            Type2StaticInitial49X initial{};initial.pairs=emptyPairs_.data();initial.metadata=emptyMetadata_.data();
            initial.pairSlots=resource.rayCount;
            Type2StaticWorkspace49X workspace{scratchPairs_.data(),scratchPairs_.size(),scratchMetadata_.data(),scratchMetadata_.size(),
                nullptr,0,&scratchEffect_,1,stack_.data(),stack_.size(),nullptr,0};
            Type2StaticDestination49X destination{pairs_.data(),pairs_.size(),metadata_.data(),metadata_.size(),nullptr,0,&effect_,1};
            Type2StaticResult49X geometry{};
            if(produceStaticType2Contacts49X(query,&candidate_,1,initial,workspace,destination,geometry)!=Type2StaticStatus49X::Complete)
                return Status::GeometryRejected;
            if(!geometry.normalModeWrite.bound)return Status::GeometryRejected;
            result.normalModeWrite.bound=true;
            if(geometry.normalModeWrite.reached)result.normalModeWrite=geometry.normalModeWrite;
            if(!geometry.hasGeometryHit)continue;
            ++result.geometryPairs;
            // 294C34 checks PRIMARY's +238 list. A prior accepted query by the
            // other body registered itself there. No broadphase-only suppression.
            if(processed[a].contains(other.identity)){++result.suppressedPairs;continue;}
            if(!processed[b].append(primary.identity)||
               applyBodyContactList48R({other.identity},result.contacts[a])==BodyContactListStatus48R::InvalidInput||
               applyBodyContactList48R({primary.identity},result.contacts[b])==BodyContactListStatus48R::InvalidInput)
                return Status::InvalidInput;
            // 294E1C..294E70 ->2963E0: direct cumulative-pair append in order,
            // same query scene, dynamic other body, no geometric normalization.
            // Main354 source21229C/2122A0 supplies40 to295C20;295C2C stores+88.
            SpringRecordState48N state{};state.generatedCapacity88=40;resetSpringRecords48N(state);
            for(std::size_t i=0;i<geometry.pairCount;++i){
                SpringRecordAppend48N append{};const auto& pair=pairs_[i];
                append.field00=pair.first;append.localPoint10=pair.second;
                append.normal20=pair.normal;append.surfacePoint30=pair.surfacePoint;
                append.scene40={primary.identity};append.other44={other.identity};append.material4C=staged[b].material;
                const auto written=appendSpringRecord48N(append,records_.data(),records_.size(),state);
                if(written!=SpringRecordStatus48N::Appended&&written!=SpringRecordStatus48N::Saturated)return Status::ResponseRejected;
            }
            const auto n=static_cast<std::size_t>(state.generatedCount84);
            for(std::size_t i=0;i<n;++i){const auto& in=records_[i];auto& out=projected_[i];out={};
                std::memcpy(out.localPoint10.v,in.localPoint10.v,sizeof(Vector3_48C));
                std::memcpy(out.normal20.v,in.normal20.v,sizeof(Vector3_48C));
                std::memcpy(out.surfacePoint30.v,in.surfacePoint30.v,sizeof(Vector3_48C));
                out.hasScene40=true;out.scene40=primary.body.state.scene;
                out.otherBodyIdentity44=other.identity;out.material4C=in.material4C;out.flags50=in.flags50;
            }
            SpringModelBody48K bodies[2]{primary,other};SpringGeneratedResult48K reacted{};
            const SpringGeneratedState48K generated{state.pending38,state.contact3C,state.upward40,state.other44,
                state.point74,state.generated78,other.identity};
            if(updateSpringContacts48L(settings,response,generated,bodies,2,nullptr,0,projected_.data(),n,
                {scratchBodies_.data(),2,scratchFlags_.data(),40},{preparedBodies_.data(),2,preparedFlags_.data(),40},
                reacted)!=SpringGeneratedStatus48K::Complete)return Status::ResponseRejected;
            primary=preparedBodies_[0];other=preparedBodies_[1];
            for(std::size_t i=0;i<n;++i){records_[i].flags50=preparedFlags_[i];if(preparedFlags_[i]&2)++result.activeRecords;}
            result.generatedRecords+=n;++result.respondedPairs;
        }
        std::memcpy(output,staged.data(),count*sizeof(Body));receipt=result;return Status::Complete;
    }
private:
    static bool fail(std::string& e,const char* why){e=why;return false;}
    struct Range {std::uintptr_t begin=0,end=0;};
    template<class T> static bool range(const T* p,std::size_t n,Range& out)noexcept {
        out={};if(!n)return true;if(!p||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
        const auto begin=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);
        if(begin%alignof(T)||bytes>std::numeric_limits<std::uintptr_t>::max()-begin)return false;
        out={begin,begin+bytes};return true;
    }
    static bool overlap(Range a,Range b)noexcept{return a.begin<b.end&&b.begin<a.end;}
    bool disjoint(const Body* input,std::size_t count,const Pair* order,std::size_t pairCount,
                  Body* output,Receipt& receipt)const noexcept {
        Range in,sequence,out,result,owner;
        if(!range(input,count,in)||!range(order,pairCount,sequence)||!range(output,count,out)||
           !range(&receipt,1,result)||!range(this,1,owner))return false;
        for(const auto r:{in,sequence,owner})if(overlap(out,r)||overlap(result,r))return false;
        return !overlap(out,result);
    }
    bool ready_=false;
    std::size_t count_=0;
    std::array<const native_racer_collision50HW::Owner*,maxRacers> resources_{};
    std::array<SpringGeneratedParameters48K,maxRacers> settings_{};
    std::array<SpringResponseParameters48L,maxRacers> response_{};
    CollisionFilter48J queryFilter_{};
    Type2StaticCandidate49X candidate_{};
    std::vector<SpringCumulativePair48N> emptyPairs_,pairs_,scratchPairs_;
    std::vector<Type2PairMetadata49X> emptyMetadata_,metadata_,scratchMetadata_;
    std::vector<Type2WorkItem49X> stack_;
    TerrainCandidateEffects48J effect_{},scratchEffect_{};
    std::array<SpringRecord48N,40> records_{};
    std::array<SpringGeneratedRecord48K,40> projected_{};
    std::array<SpringModelBody48K,2> scratchBodies_{},preparedBodies_{};
    std::array<std::uint8_t,40> scratchFlags_{},preparedFlags_{};
};
}
