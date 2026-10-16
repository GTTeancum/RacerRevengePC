#pragma once
#include "original_wreck_response50la.h"
#include "original_wreck_group_arrays50la.h"
#include "original_wreck_asset_inventory50la.h"

namespace rr::game {
namespace original_wreck_response_detail50LA {
// The optional domain is the SAME privately published query-shape identity
// authority consumed by POINT/294BB0 producers, never a response-only map.
struct GroupContactContext {
    OriginalWreckPhysical50LA& contactOwner;
    OriginalWreckPhysical50LA& bodyOwner;
    OriginalWreckAssetInventory50LA& inventory;
    std::size_t collection;
    const SpringVector4_48N& actualZero4435A0;
    const OriginalWreckSceneDomain50LA* scenes=nullptr;
    std::uint64_t generation=0;
    VisualLazyWord49R* pointSentinel=nullptr;
    VisualLazyWord49R* generatedSentinel=nullptr;
    OriginalWreckAssets50LA* bodyAssets=nullptr;
    OriginalWreckAssets50LA* contactAssets=nullptr;
    OriginalWreckPhysical50LA& contact()noexcept{return contactOwner;}
    OriginalWreckPhysical50LA& body()noexcept{return bodyOwner;}
    bool bound()const noexcept{return contactOwner.contactConstructorFieldsApplied&&
        contactOwner.contactBody30==&bodyOwner.body;}
    OriginalWreckAssets50LA* occurrence(const OriginalWreckPhysical50LA& p)noexcept {
        if(!inventory.complete()||collection>=inventory.collections().size())return nullptr;
        OriginalWreckAssets50LA* match=nullptr;
        for(std::size_t i=0;i<inventory.collections()[collection].pieces.size();++i){
            auto* a=inventory.pieceAssets(collection,i);
            if(a&&a->prepared()&&p.model288==a->model14C()&&
               p.modelGraphNodes==a->modelGraphIdentity()&&p.modelGraphCount==a->modelGraphNodeCount()){
                if(match)return nullptr;match=a;
            }
        }
        return match;
    }
    bool ready()noexcept {
        if(!contactOwner.bodyConstructorFieldsApplied||!bodyOwner.bodyConstructorFieldsApplied)return false;
        contactAssets=occurrence(contactOwner);bodyAssets=occurrence(bodyOwner);
        return contactAssets&&bodyAssets;
    }
    SceneGraph49P graph()noexcept{return bodyAssets->graph();}
    const SpringVector4_48N* zero()const noexcept{return &actualZero4435A0;}
    bool sentinels(VisualLazyWord49R& a,VisualLazyWord49R& b)noexcept {
        Span child{},target{},x{},y{};
        if(!span(&contactOwner,1,child)||!span(&bodyOwner,1,target)||!span(&a,1,x)||!span(&b,1,y))return false;
        return (&contactOwner==&bodyOwner||!overlap(child,target))&&!overlap(x,y)&&
            !overlap(child,x)&&!overlap(child,y)&&!overlap(target,x)&&!overlap(target,y);
    }
    bool storageBound(const SceneGraph49P& g,VisualLazyWord49R& a,VisualLazyWord49R& b)noexcept {
        Span ranges[7];std::size_t count=0;
        if(!span(&contactOwner,1,ranges[count++]))return false;
        if(&contactOwner!=&bodyOwner&&!span(&bodyOwner,1,ranges[count++]))return false;
        if(!span(g.nodes,g.count,ranges[count++]))return false;
        if(contactAssets!=bodyAssets){const auto childGraph=contactAssets->graph();
            if(!span(childGraph.nodes,childGraph.count,ranges[count++]))return false;}
        if(!span(&a,1,ranges[count++])||!span(&b,1,ranges[count++])||
           !span(&actualZero4435A0,1,ranges[count++]))return false;
        for(std::size_t i=0;i<count;++i)for(std::size_t j=0;j<i;++j)
            if(overlap(ranges[i],ranges[j]))return false;
        pointSentinel=&a;generatedSentinel=&b;return true;
    }
    bool recordFrame(SpringSceneId48N id,SceneGraph49P& g,SceneNodeIndex49P& model)noexcept {
        if(!id.value){g=bodyAssets->graph();model=bodyOwner.model288;return true;} //2968CC
        OriginalWreckSceneView50LA view;
        if(!scenes||scenes!=contactOwner.modelSceneDomain||!scenes->publishedFor(contactOwner,generation)||
           !scenes->resolve(id,generation,view)||!view.physical||
           occurrence(*view.physical)!=view.assets||!pointSentinel||!generatedSentinel)return false;
        Span scene{},ranges[5];
        if(!span(view.graph.nodes,view.graph.count,scene)||!span(&contactOwner,1,ranges[0])||
           !span(&bodyOwner,1,ranges[1])||!span(pointSentinel,1,ranges[2])||
           !span(generatedSentinel,1,ranges[3])||!span(&actualZero4435A0,1,ranges[4]))return false;
        for(const auto& range:ranges)if(overlap(scene,range))return false;
        g=view.graph;model=view.model;return true; //296884, no refresh or matrix copy
    }
};
}
// Bounded static generated-contact response for a real group member. Resolve
// the ONLY allowed target from its canonical DC.30: own D0 or group primary D0.
// Existing state/records are consumed in place. This neither binds/attaches a
// member nor enables bodies, creates records, or certifies query/effect lifetime.
// No-record completion does not read inventory, graph, or scene identities.
// Nonzero scene40 requires the actual query/producer domain; null, stale or
// unknown bindings stop at296884. Earlier source prefix writes remain committed.
inline OriginalWreckResponseResult50LA advanceOriginalWreckGroupContact2964C0_50LA(
    OriginalWreckGroupArrays50LA& group,std::size_t memberSlot,
    OriginalWreckAssetInventory50LA& inventory,std::size_t collectionIndex,
    const SpringVector4_48N& actualZero4435A0,VisualLazyWord49R& pointSentinel4435E0,
    VisualLazyWord49R& generatedSentinel4435E8,
    const OriginalWreckSceneDomain50LA* actualProducerScenes=nullptr)noexcept {
    using S=OriginalWreckResponseStatus50LA;
    const auto* member=group.member(memberSlot);const auto* primary=group.primary();
    if(!member||!primary||!member->piece||!member->physical||!member->component||
       !primary->piece||!primary->physical||!primary->component||
       member->component->kind!=OriginalWreckComponentKind50LA::piece2CB4B0||
       primary->component->kind!=OriginalWreckComponentKind50LA::piece2CB4B0||
       !member->component->reference00||!primary->component->reference00||
       !member->component->objectKey||!primary->component->objectKey||
       !member->component->sessionGeneration||
       member->component->sessionGeneration!=primary->component->sessionGeneration)
        return {S::owner,0x2964c0u};
    auto& child=*member->physical;
    OriginalWreckPhysical50LA* target=nullptr;
    if(child.contactBody30==&child.body)target=&child;
    else if(child.contactBody30==&primary->physical->body)target=primary->physical;
    if(!target)return {S::owner,0x2964c0u};
    original_wreck_response_detail50LA::GroupContactContext context{
        child,*target,inventory,collectionIndex,actualZero4435A0,actualProducerScenes,
        member->component->sessionGeneration};
    return original_wreck_response_detail50LA::advanceContact2964C0(
        context,pointSentinel4435E0,generatedSentinel4435E8);
}
} // namespace rr::game
