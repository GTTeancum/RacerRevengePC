#pragma once
#include "runtime_asset_paths50lb.h"
#include "native_racer_contact_status50du.h"
// UNVERIFIED reconstruction from worker conversation after workspace rollback.
// HOST-ORDERED native main-body contact adapter, not original world scheduling.
// Actual Pod01 podboxm rays -> actual podboxf triangles; no bumper primitive.
// Each supplied pair is a separate one-candidate source query. Source tree/ray
// order, 294BB0 dynamic-body suppression and 2964C0 two-body response are kept.
// Terrain contacts are neither read nor reset. Secondary/chariot and damage
// ownership are deliberately outside this adapter.
#include "../../src/game/static_type2_producer49x.h"
#include "../../src/game/query_prepare48o.h"
#include "../../src/game/pod_shape49w.h"
#include "../../src/game/body_contact_list48r.h"
#include "../../src/game/spring_model48k.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/import_collision48g.h"
#include "../../src/assets/collision_mesh48i.h"
#include <array>
#include <bit>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace native_racer_contact50HT {
using namespace rr::game;
inline constexpr std::size_t maxRacers=8;
// Deterministic native policy authorized for the authenticated empty physical
// root name. There is no authored frX suffix; source reads unknown prefix
// padding here, so this is NOT a byte-equivalent recovery of dynamic material.
inline constexpr std::uint32_t emptyPhysicalRootMaterial=0;
struct Pair {std::size_t primary,other;};
struct Body {
    SpringModelBody48K physical{};
    // Caller binding; the current fresh-root host policy is explicitly zero.
    // A named root with an authored frX suffix must supply that material.
    std::uint32_t material=emptyPhysicalRootMaterial;
};
struct Receipt {
    std::size_t testedPairs=0,geometryPairs=0,suppressedPairs=0,respondedPairs=0;
    std::size_t generatedRecords=0,activeRecords=0;
    std::array<BodyContactListState48R,maxRacers> contacts{};
    CollisionNormalModeWrite50JA normalModeWrite{};
};
// Original3CE000+A0 ->1654C0 has the same bounded append/search algorithm as
// +98 ->165430, with count238/slots23C instead of20C/210. +A4 ->165540 searches
// every active slot. Native 0..10 admission avoids original out-of-bounds states.
struct ProcessedList {
    // Transaction-local scratch only. 1654C0 is the same algorithm as165430,
    // hence the reused storage shape; the names20C/210 are NOT another owner.
    // Canonical state is always the input/output RootBodyState48H's238/23C.
    BodyContactListState48R storage{};
    bool load(const RootBodyState48H& root) noexcept {
        std::memcpy(&storage.count20C,&root.callbackState238,sizeof(storage.count20C));
        std::memcpy(storage.slots210.data(),root.processedContactPartners23C,
            sizeof(root.processedContactPartners23C));
        return storage.count20C>=0&&
            storage.count20C<=static_cast<std::int32_t>(BodyContactCapacity48R);
    }
    void store(RootBodyState48H& root) const noexcept {
        std::memcpy(&root.callbackState238,&storage.count20C,sizeof(storage.count20C));
        std::memcpy(root.processedContactPartners23C,storage.slots210.data(),
            sizeof(root.processedContactPartners23C));
    }
    bool contains(std::uint32_t id)const noexcept {
        if(storage.count20C<0||storage.count20C>static_cast<std::int32_t>(BodyContactCapacity48R))return false;
        for(std::int32_t i=0;i<storage.count20C;++i)
            if(storage.slots210[static_cast<std::size_t>(i)].value==id)return true;
        return false;
    }
    bool append(std::uint32_t id)noexcept {
        return applyBodyContactList48R({id},storage)!=BodyContactListStatus48R::InvalidInput;
    }
};
class Owner {
public:
    Owner()=default;
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    // Cold only. Re-decodes real resources and compares caller's query geometry
    // byte-for-byte, retaining its explicit file-absent endpoint W words.
    bool prepare(const Type2ResourceView49X& mainResource,std::string& error,
                 const char* archivePath="work/native/project/disc/PODS/POD01/POD01.RES") {
        if(ready_)return fail(error,"contact owner already prepared");
        std::ifstream file(runtime_asset_paths50LB::resolve(archivePath),std::ios::binary);
        if(!file)return fail(error,"actual Pod01 archive missing");
        std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(file),{}},podM,podF,cache;
        rr::assets::ResArchive48C archive;rr::assets::DecodeError48C why;
        if(!rr::assets::decodeResource48C(bytes,archive,why)||
           !rr::assets::extractResMember48C(archive,"Pods\\Pod01\\podboxm.col",podM,why)||
           !rr::assets::extractResMember48C(archive,"Pods\\Pod01\\podboxf.col",podF,why))
            return fail(error,"actual Pod01 collision members missing");
        if(podM.size()<92||!mainResource.nodes||!mainResource.rays||
           u32(podM.data()+80)!=mainResource.rayCount||u32(podM.data()+84)!=mainResource.nodeCount)
            return fail(error,"main contact query is not Pod01 podboxm");
        queryNodes_.resize(mainResource.nodeCount);queryRays_.resize(mainResource.rayCount);
        std::vector<Type2OpaqueEndpointWords49X> opaque(mainResource.rayCount);
        std::vector<std::uint32_t> topology(mainResource.nodeCount+mainResource.rayCount);
        for(std::size_t i=0;i<opaque.size();++i)opaque[i]={
            std::bit_cast<std::uint32_t>(mainResource.rays[i].first.v[3]),
            std::bit_cast<std::uint32_t>(mainResource.rays[i].second.v[3])};
        if(decodeType2Resource49X(podM.data(),podM.size(),opaque.data(),opaque.size(),
            queryNodes_.data(),queryNodes_.size(),queryRays_.data(),queryRays_.size(),
            {topology.data(),topology.size()},resource_)!=Type2ResourceStatus49X::Complete||
            resource_.root!=mainResource.root||resource_.maxDepth!=mainResource.maxDepth||
            std::memcmp(resource_.localMatrix.v,mainResource.localMatrix.v,sizeof(BodyMatrix48G))||
            std::memcmp(queryNodes_.data(),mainResource.nodes,queryNodes_.size()*sizeof(Type2Node49X))||
            std::memcmp(queryRays_.data(),mainResource.rays,queryRays_.size()*sizeof(Type2Ray49X)))
            return fail(error,"main contact query differs from actual Pod01 geometry");
        rr::assets::Collision48G source;
        if(!rr::assets::decodeCollision48G(podF,source,why)||
           !rr::assets::encodeCollisionAsset48G(source,cache,error)||
           !rr::assets::loadCollisionMesh48I(cache,mesh_,error))
            return fail(error,"actual Pod01 podboxf mesh rejected");
        if(mesh_.vertices.size()!=16||mesh_.nodes.size()!=27||mesh_.faces.size()!=28)
            return fail(error,"unexpected Pod01 podboxf topology");
        radii_.resize(mesh_.nodes.size());topology.resize(mesh_.nodes.size()+mesh_.faces.size());
        if(prepareStaticTargetRadii49X(mesh_,radii_.data(),radii_.size(),
            {topology.data(),topology.size()},targetDepth_)!=Type2ResourceStatus49X::Complete)
            return fail(error,"Pod01 target tree rejected");
        std::memcpy(targetSource_.local40.v,mesh_.local_matrix.data(),sizeof(BodyMatrix48G));
        std::memcpy(targetSource_.rootCenter.v,mesh_.nodes[0].center.data(),sizeof(Vector3_48C));
        std::memcpy(targetSource_.rootHalf.v,mesh_.nodes[0].half.data(),sizeof(Vector3_48C));
        const auto n=resource_.rayCount;
        emptyPairs_.resize(n);pairs_.resize(n);scratchPairs_.resize(n);
        emptyMetadata_.resize(n);metadata_.resize(n);scratchMetadata_.resize(n);
        stack_.resize(1+resource_.maxDepth+targetDepth_);
        // Host collision namespace; all bodies get unique explicit identities.
        // These empty exclusion lists preserve the source filter semantics.
        queryFilter_.flagsF8=2;queryFilter_.flags31=3;
        queryFilter_.categoryA4=2;queryFilter_.acceptMaskB8=2;
        candidate_.candidate.collision.filter.flagsF8=4;
        candidate_.candidate.collision.filter.flags31=3;
        candidate_.candidate.collision.filter.categoryA4=2;
        candidate_.candidate.collision.typeTag=1;
        candidate_.candidate.collision.geometry.mesh=&mesh_;
        candidate_.nodeRadii=radii_.data();candidate_.nodeRadiusCount=radii_.size();
        candidate_.maxDepth=targetDepth_;
        ready_=true;error.clear();return true;
    }
    std::size_t rays()const noexcept{return resource_.rayCount;}
    const rr::assets::CollisionMesh48I& targetMesh()const noexcept{return mesh_;}
    // Hot, no allocations. Input and output must be separate arrays of at least
    // count live Body objects, neither owned by this Owner; receipt is separate.
    // Both body results commit together only after every pair succeeds. Pair
    // order is consumed verbatim. Reverse pairs are suppressed ONLY after an
    // actual prior geometry hit, exactly as the source dynamic callback gates.
    // Each pair's one-candidate query preserves all ray/tree/pair ordering.
    // Settings/response are the already-bound authored main spring parameters.
    Status run(const Body* input,std::size_t count,const Pair* order,std::size_t pairCount,
               const SpringGeneratedParameters48K& settings,const SpringResponseParameters48L& response,
               Body* output,Receipt& receipt) noexcept {
        if(!ready_)return Status::Unprepared;
        if(!input||!output||input==output||count<2||count>maxRacers||
           pairCount>maxRacers*(maxRacers-1)||(pairCount&&!order)||settings.pointContacts48)
            return Status::InvalidInput;
        for(std::size_t i=0;i<count;++i){
            if(!input[i].physical.identity||input[i].material>=settings.frictionByMaterial.size())return Status::InvalidInput;
            for(std::size_t j=0;j<i;++j)if(input[i].physical.identity==input[j].physical.identity)return Status::InvalidInput;
        }
        for(std::size_t i=0;i<pairCount;++i)
            if(order[i].primary>=count||order[i].other>=count||order[i].primary==order[i].other)return Status::InvalidInput;
        std::array<Body,maxRacers> staged{};
        std::memcpy(staged.data(),input,count*sizeof(Body));
        std::array<ProcessedList,maxRacers> processed{};Receipt result{};
        // Never reset here: only the genuine root callback1654B0 owns cadence.
        // Existing pending identities must participate in294C34 suppression.
        for(std::size_t i=0;i<count;++i)
            if(!processed[i].load(staged[i].physical.body.state))return Status::InvalidInput;
        result.normalModeWrite.bound=true; // prepared main690 query owners, including zero-pair pass
        for(std::size_t p=0;p<pairCount;++p){
            const auto a=order[p].primary,b=order[p].other;
            auto& primary=staged[a].physical;auto& other=staged[b].physical;
            ++result.testedPairs;
            QuerySpherePrepareInput48O sphereInput{};sphereInput.preparedScene=primary.body.state.scene;
            sphereInput.hasPairTree=true;
            const auto& root=resource_.nodes[resource_.root];
            for(unsigned k=0;k<3;++k){sphereInput.treeCenter.v[k]=root.center[k];sphereInput.treeHalf.v[k]=root.half[k];}
            QuerySpherePrepareOutput48O sphere{};
            if(prepareQuerySphere48O(sphereInput,sphere)!=QuerySpherePrepareStatus48O::Ready)return Status::GeometryRejected;
            PodShapeState49W target{};target.registrationE0=0;
            if(!transformPodShape49W(targetSource_,other.body.state.scene,target))return Status::GeometryRejected;
            auto& c=candidate_.candidate.collision;
            queryFilter_.identity=primary.identity;c.filter.identity=other.identity;
            c.geometry.id=other.identity;c.geometry.worldMatrix=target.composed00;
            for(unsigned k=0;k<3;++k)c.sphere.center.v[k]=target.center60.v[k];c.sphere.radius=target.radius80;
            Type2StaticQuery49X query{};query.resource=&resource_;query.filter=&queryFilter_;
            query.worldMatrix=sphere.world;query.sphere.radius=sphere.radius;
            for(unsigned k=0;k<3;++k)query.sphere.center.v[k]=sphere.center.v[k];
            query.queryScene={primary.identity};query.collisionTypeTag=1;query.groupTypeTag=2;
            query.callbackEnabled=false;
            query.normalMode54=0u;query.normalMode54Bound=true; // authentic main690 constructor21219C/11BBEC
            Type2StaticInitial49X initial{};initial.pairs=emptyPairs_.data();initial.metadata=emptyMetadata_.data();
            initial.pairSlots=resource_.rayCount;
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
        // Publish the actual asymmetric1654C0 callbacks with the responses.
        // A rejected pair leaves both canonical counts and every slot unchanged.
        for(std::size_t i=0;i<count;++i)processed[i].store(staged[i].physical.body.state);
        std::memcpy(output,staged.data(),count*sizeof(Body));receipt=result;return Status::Complete;
    }
private:
    static bool fail(std::string& e,const char* s){e=s;return false;}
    static std::uint32_t u32(const std::uint8_t* p)noexcept{return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);}
    bool ready_=false;
    Type2ResourceView49X resource_{};
    std::vector<Type2Node49X> queryNodes_;
    std::vector<Type2Ray49X> queryRays_;
    rr::assets::CollisionMesh48I mesh_;
    std::vector<float> radii_;
    std::size_t targetDepth_=0;
    PodShapeSource49W targetSource_{};
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
