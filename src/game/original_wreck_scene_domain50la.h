#pragma once
#include "original_wreck_assets50la.h"
#include "original_wreck_physical50la.h"
#include "candidate_collection50bx.h"
#include "original_wreck_type2_owners50la.h"

namespace rr::game {
class OriginalWreckQueries50LA;
struct OriginalWreckSceneView50LA {
    OriginalWreckAssets50LA* assets=nullptr;
    OriginalWreckPhysical50LA* physical=nullptr;
    SceneGraph49P graph{};
    SceneNodeIndex49P model=SceneNodeIndex49P::none;
};
// Native identity authority for actual1253F0 model/shape publications. Only the
// real query owner can publish. Query/POINT producers and response resolve the
// SAME entry. This owns no model, query, Entity, registration or source lifetime.
// An external shared domain MUST outlive every borrowing query owner.
class OriginalWreckSceneDomain50LA final {
    friend class OriginalWreckQueries50LA;
    struct Handle {std::size_t slot=static_cast<std::size_t>(-1);std::uint64_t generation=0;};
    struct Entry {
        bool live=false;
        SpringSceneId48N identity{};
        OriginalWreckAssets50LA* assets=nullptr;
        OriginalWreckPhysical50LA* physical=nullptr;
        const OriginalWreckComponent50LA* modelComponent=nullptr;
        const OriginalWreckComponent50LA* queryComponent=nullptr;
        const CollectionObject50BX* queryObject=nullptr;
        std::uint64_t modelKey=0,queryKey=0;
        std::uint32_t queryIdentity=0;
        const SceneNode49P* nodes=nullptr;
        std::uint32_t nodeCount=0;
        SceneNodeIndex49P model=SceneNodeIndex49P::none;
    };
    std::unique_ptr<Entry[]> entries_;
    std::uint64_t generation_=0;
    std::size_t capacity_=0,used_=0,live_=0;
    bool closed_=false;
    bool coherent(const Entry& e)const noexcept {
        if(!e.live||closed_||!e.assets||!e.assets->prepared()||!e.physical||
           !e.modelComponent||!e.queryComponent||!e.queryObject)return false;
        return e.modelComponent->kind==OriginalWreckComponentKind50LA::model2403A0&&
            e.queryComponent->kind==OriginalWreckComponentKind50LA::collision11B190&&
            e.modelComponent->reference00&&e.queryComponent->reference00&&
            e.modelComponent->sessionGeneration==generation_&&e.queryComponent->sessionGeneration==generation_&&
            e.modelComponent->objectKey==e.modelKey&&e.queryComponent->objectKey==e.queryKey&&
            e.queryObject->identity==e.queryIdentity&&e.queryObject->shapeTag==2u&&
            e.queryObject->hasShape&&e.physical->modelSceneDomain==this&&e.physical->bodyConstructorFieldsApplied&&
            e.physical->model288==e.model&&e.physical->modelGraphNodes==e.nodes&&
            e.physical->modelGraphCount==e.nodeCount&&e.assets->model14C()==e.model&&
            e.assets->modelGraphIdentity()==e.nodes&&e.assets->modelGraphNodeCount()==e.nodeCount;
    }
    // Native admission before canonical publication; no source field is written.
    bool preparePublication(SpringSceneId48N id,OriginalWreckAssets50LA& assets,
        OriginalWreckPhysical50LA& physical,const OriginalWreckComponent50LA& model,
        const OriginalWreckComponent50LA& query,std::uint32_t queryIdentity,Entry& out)const noexcept {
        if(!prepared()||used_==capacity_||!id.value||!queryIdentity||!assets.prepared()||
           model.kind!=OriginalWreckComponentKind50LA::model2403A0||
           query.kind!=OriginalWreckComponentKind50LA::collision11B190||!model.reference00||!query.reference00||
           !model.objectKey||!query.objectKey||model.objectKey==query.objectKey||
           model.sessionGeneration!=generation_||query.sessionGeneration!=generation_||
           !physical.bodyConstructorFieldsApplied||physical.modelSceneDomain||physical.model288!=assets.model14C()||
           physical.modelGraphNodes!=assets.modelGraphIdentity()||
           physical.modelGraphCount!=assets.modelGraphNodeCount())return false;
        for(std::size_t i=0;i<used_;++i){const auto& e=entries_[i];
            // Tombstones are never reused: neither an old ID nor occurrence can
            // silently acquire another meaning within the same session.
            if(e.identity.value==id.value||e.physical==&physical||e.nodes==assets.modelGraphIdentity()||
               e.modelKey==model.objectKey||e.queryKey==query.objectKey||
               e.modelKey==query.objectKey||e.queryKey==model.objectKey||e.queryIdentity==queryIdentity)return false;
        }
        out={};out.identity=id;out.assets=&assets;out.physical=&physical;
        out.modelComponent=&model;out.queryComponent=&query;out.modelKey=model.objectKey;out.queryKey=query.objectKey;
        out.queryIdentity=queryIdentity;out.nodes=assets.modelGraphIdentity();out.nodeCount=assets.modelGraphNodeCount();
        out.model=assets.model14C();return true;
    }
    // Opaque native handle allocated only for this actual model/query pair at
    // the1253F0 publication attempt. Never a truncated guest/host pointer or
    // cold planned component identity. Existing public caller IDs unchanged.
    SpringSceneId48N nativeIdentityForPublication(OriginalWreckAssets50LA& assets,
        OriginalWreckPhysical50LA& physical,const OriginalWreckComponent50LA& model,
        const OriginalWreckComponent50LA& query,std::uint32_t queryIdentity)const noexcept {
        if(!prepared()||used_>=capacity_)return {};
        const SpringSceneId48N id{static_cast<std::uint32_t>(used_+1u)};
        Entry checked;
        return preparePublication(id,assets,physical,model,query,queryIdentity,checked)?id:SpringSceneId48N{};
    }
    Handle publish1253F0(Entry entry,const CollectionObject50BX& canonicalQuery)noexcept {
        // Called serially ONLY after preparePublication and canonical E0 store.
        entry.queryObject=&canonicalQuery;entry.live=true;entry.physical->modelSceneDomain=this;
        const Handle result{used_,generation_};entries_[used_++]=entry;++live_;return result;
    }
    SpringSceneId48N identity(Handle h)const noexcept {
        return h.generation==generation_&&h.slot<used_&&coherent(entries_[h.slot])?
            entries_[h.slot].identity:SpringSceneId48N{};
    }
    bool canRetireShape11B600(Handle h,const OriginalWreckComponent50LA& query)const noexcept {
        return h.generation==generation_&&h.slot<used_&&coherent(entries_[h.slot])&&
            entries_[h.slot].queryComponent==&query&&query.reference00==1u;
    }
    bool retireShape11B600(Handle h,const OriginalWreckType2Owners50LA& allocations,
        const OriginalWreckComponent50LA& query)noexcept {
        if(h.generation!=generation_||h.slot>=used_||!allocations.retired()||query.reference00)return false;
        auto& e=entries_[h.slot];
        if(!e.live||e.queryComponent!=&query||query.objectKey!=e.queryKey||
           query.sessionGeneration!=generation_||!e.queryObject||e.queryObject->hasShape||
           e.queryObject->identity!=e.queryIdentity||!e.physical||e.physical->modelSceneDomain!=this)return false;
        // The source shape containing model44 has actually died. Preserve the
        // historical identity/key/occurrence tombstone; never erase its row or
        // make it available to another model. End the native model borrow now.
        e.physical->modelSceneDomain=nullptr;e.live=false;--live_;return true;
    }
    void revokeNative(Handle h)noexcept {
        // Token-only invalidation: NEVER dereference possibly already destroyed
        // component/model/query borrows. This is not source11B4A0 retirement.
        if(h.generation==generation_&&h.slot<used_&&entries_[h.slot].live){entries_[h.slot].live=false;--live_;}
    }
public:
    OriginalWreckSceneDomain50LA()=default;
    OriginalWreckSceneDomain50LA(const OriginalWreckSceneDomain50LA&)=delete;
    OriginalWreckSceneDomain50LA& operator=(const OriginalWreckSceneDomain50LA&)=delete;
    bool prepareCold(std::uint64_t sessionGeneration,std::size_t actualPieceCapacity)noexcept {
        if(generation_||closed_||!sessionGeneration||!actualPieceCapacity||actualPieceCapacity>512u)return false;
        try {entries_=std::make_unique<Entry[]>(actualPieceCapacity);}catch(const std::bad_alloc&){return false;}
        generation_=sessionGeneration;capacity_=actualPieceCapacity;return true;
    }
    bool prepared()const noexcept{return generation_&&entries_&&!closed_;}
    std::uint64_t sessionGeneration()const noexcept{return generation_;}
    bool closeAfterNativeRevocation()noexcept {
        if(!prepared()||live_)return false;closed_=true;return true;
    }
    bool publishedFor(const OriginalWreckPhysical50LA& physical,std::uint64_t generation)const noexcept {
        if(!prepared()||generation!=generation_||physical.modelSceneDomain!=this)return false;
        for(std::size_t i=0;i<used_;++i)if(entries_[i].physical==&physical)
            return coherent(entries_[i]);
        return false;
    }
    bool resolve(SpringSceneId48N id,std::uint64_t generation,OriginalWreckSceneView50LA& out)const noexcept {
        if(!prepared()||generation!=generation_||!id.value)return false;
        for(std::size_t i=0;i<used_;++i)if(entries_[i].identity.value==id.value){
            const auto& e=entries_[i];if(!coherent(e))return false;
            out={e.assets,e.physical,e.assets->graph(),e.model};return true;
        }
        return false;
    }
};
} // namespace rr::game
