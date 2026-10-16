#pragma once
#include "original_wreck_components50la.h"

namespace rr::game {
namespace original_entity_chain_detail50LA {
using namespace original_wreck_component_detail50LA;
// Validate the complete borrowed topology before any source store. No copied
// nodes, reference acquisition, activity changes or dispatch completion.
template<class Domain>
inline bool walk(const Domain& d,const C* first,const C* parent,
                 bool (&seen)[maxOriginalWreckComponents50LA]) noexcept {
    const C* previous=nullptr;
    for(auto* p=first;p;p=p->next14){
        if(!contains(d,p))return false;
        std::size_t i=0;while(d.objects[i]!=p)++i;
        if(seen[i]||p->parent1C!=parent||p->previous10!=previous)return false;
        seen[i]=true;
        if(!walk(d,p->firstChild18,p,seen))return false;
        previous=p;
    }
    return true;
}
template<class Domain>
inline bool forest(const Domain& d) noexcept {
    if(!domain(d))return false;
    bool seen[maxOriginalWreckComponents50LA]{};
    for(std::size_t i=0;i<d.count;++i){
        const auto* p=d.objects[i];
        if(!p->parent1C&&!p->previous10&&!walk(d,p,nullptr,seen))return false;
    }
    for(std::size_t i=0;i<d.count;++i)if(!seen[i])return false;
    return true;
}
inline bool incoming(const C& head,const C& destination) noexcept {
    if(head.parent1C||head.previous10)return false;
    // The destination cannot lie in any incoming sibling's subtree.
    for(auto* p=&destination;p;p=p->parent1C)
        for(auto* q=&head;q;q=q->next14)if(p==q)return false;
    return true;
}
inline void propagate(C& head) noexcept {
    auto* below=&head;
    for(auto* p=head.parent1C;p;p=p->parent1C){
        p->descendants24|=below->descendants24|below->mask20;
        below=p;
    }
}
//141770: recompute each reached child subtree, returning own mask OR descendants.
inline std::uint32_t recompute(C& root) noexcept {
    std::uint32_t mask=0;
    for(auto* p=root.firstChild18;p;p=p->next14)mask|=recompute(*p);
    root.descendants24=mask;
    return mask|root.mask20;
}
}

namespace original_entity_chain_detail50LA {
//141860 appends an incoming sibling chain after the tail reached from anchor.
// It is NOT child attachment. Source sets only the incoming HEAD's mask20;
// the remaining siblings retain their masks. Parent may genuinely be null
// (race load builds its chain before Application10 attachment).
template<class Domain>
inline OriginalWreckComponentResult50LA appendOriginalWreckSiblingChain14186050LA(
    const Domain& d,OriginalWreckComponent50LA& anchor,
    OriginalWreckComponent50LA& head,std::uint32_t sourceMask,
    std::uint32_t actualAttachmentLock423F58) noexcept {
    using namespace original_entity_chain_detail50LA;
    using S=OriginalWreckComponentStatus50LA;
    if(actualAttachmentLock423F58)return {S::attachmentLocked,0x14187cu};
    if(!domain(d)||!contains(d,&anchor)||!contains(d,&head))return {S::owner,0x141884u};
    if(!forest(d)||!incoming(head,anchor))return {S::topology,0x141898u};
    auto* tail=&anchor;while(tail->next14)tail=tail->next14;
    tail->next14=&head;head.previous10=tail;
    for(auto* p=&head;p;p=p->next14)p->parent1C=anchor.parent1C;
    head.mask20=sourceMask;propagate(head);
    return {S::complete,0x141948u};
}

//141960's actual chain splice. The older singleton helper remains unchanged.
// Parent assignment visits every incoming sibling (1419D0..1419E4), but the
// mask store and ancestor propagation start only at head (1419F0..141A54).
template<class Domain>
inline OriginalWreckComponentResult50LA attachOriginalWreckChildChain14196050LA(
    const Domain& d,OriginalWreckComponent50LA& parent,
    OriginalWreckComponent50LA& head,std::uint32_t sourceMask,
    std::uint32_t actualAttachmentLock423F58) noexcept {
    using namespace original_entity_chain_detail50LA;
    using S=OriginalWreckComponentStatus50LA;
    if(actualAttachmentLock423F58)return {S::attachmentLocked,0x14197cu};
    if(!domain(d)||!contains(d,&parent)||!contains(d,&head))return {S::owner,0x141984u};
    if(!forest(d)||!incoming(head,parent))return {S::topology,0x14198cu};
    auto* tail=parent.firstChild18;
    if(tail){while(tail->next14)tail=tail->next14;tail->next14=&head;head.previous10=tail;}
    else {parent.firstChild18=&head;head.previous10=nullptr;}
    for(auto* p=&head;p;p=p->next14)p->parent1C=&parent;
    head.mask20=sourceMask;propagate(head);
    return {S::complete,0x141a58u};
}

//141A70 removes exactly one node, leaving its children, mask20 and reference00
// intact. Recompute the original top ancestor's complete surviving child tree,
// rather than subtracting bits shared with another sibling. A parentless node
// recomputes its own child tree, exactly as141AF8..141B78 do.
template<class Domain>
inline OriginalWreckComponentResult50LA detachOriginalWreckComponent141A7050LA(
    const Domain& d,OriginalWreckComponent50LA& node,
    std::uint32_t actualAttachmentLock423F58) noexcept {
    using namespace original_entity_chain_detail50LA;
    using S=OriginalWreckComponentStatus50LA;
    if(actualAttachmentLock423F58)return {S::attachmentLocked,0x141aa0u};
    if(!domain(d)||!contains(d,&node))return {S::owner,0x141aa8u};
    if(!forest(d))return {S::topology,0x141aa8u};
    if(node.previous10)node.previous10->next14=node.next14;
    if(node.next14)node.next14->previous10=node.previous10;
    if(node.parent1C&&node.parent1C->firstChild18==&node)node.parent1C->firstChild18=node.next14;
    auto* root=&node;while(root->parent1C)root=root->parent1C;
    recompute(*root);
    node.parent1C=nullptr;node.next14=nullptr;node.previous10=nullptr;
    return {S::complete,0x141b88u};
}
}
inline OriginalWreckComponentResult50LA appendOriginalWreckSiblingChain14186050LA(
    const OriginalWreckComponentDomain50LA& d,OriginalWreckComponent50LA& anchor,
    OriginalWreckComponent50LA& head,std::uint32_t mask,std::uint32_t lock) noexcept {
    return original_entity_chain_detail50LA::appendOriginalWreckSiblingChain14186050LA(d,anchor,head,mask,lock);
}
inline OriginalWreckComponentResult50LA attachOriginalWreckChildChain14196050LA(
    const OriginalWreckComponentDomain50LA& d,OriginalWreckComponent50LA& anchor,
    OriginalWreckComponent50LA& head,std::uint32_t mask,std::uint32_t lock) noexcept {
    return original_entity_chain_detail50LA::attachOriginalWreckChildChain14196050LA(d,anchor,head,mask,lock);
}
inline OriginalWreckComponentResult50LA detachOriginalWreckComponent141A7050LA(
    const OriginalWreckComponentDomain50LA& d,OriginalWreckComponent50LA& node,
    std::uint32_t lock) noexcept {
    return original_entity_chain_detail50LA::detachOriginalWreckComponent141A7050LA(d,node,lock);
}
}
