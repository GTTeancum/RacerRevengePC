#pragma once
namespace rr::game {
inline OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::orientSourceNullRelative28A090(
    const SceneGraph49P& graph,const std::uint32_t* look,const std::uint32_t* lookKnown,
    const std::uint32_t* up,const std::uint32_t* upKnown,SourceOrientation28A090& h)noexcept {
    if(h.entered||!look||!lookKnown||!up||!upKnown||!preflightSourceRenderableReturn26E820Native()||
       graph.nodes!=sourceConstructor_.canonicalNodes||graph.count!=binding_.sourceRowCount||
       !graph.count||graph.count>maxSceneNodes49P||!validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex))
        return {S::phase,0x28a090u};
    h.entered=true;h.owner=this;h.canonicalNodes=graph.nodes;h.count=graph.count;
    h.root=static_cast<std::uint32_t>(model_.sceneIndex);
    for(std::uint32_t i=0;i<h.count;++i)h.before[i]=h.after[i]=graph.nodes[i];
    h.pc=0x28a0a0u;
    if(!OriginalStandardPlacementGlobals50LA::enterBasis28A090(h.guard))return {S::phase,h.pc};
    h.guardReturned=true;
    auto& root=graph.nodes[h.root];
    //28A0C8 VF0 initializes W;28A0CC..E4 saves actual localXYZ BEFORE
    //reading actual Base vectors. No guard or snapshot is rolled back.
    std::memcpy(h.savedXYZ.data(),root.localC0.v+12,12);h.scratchInitialized=true;
    h.pc=0x28a138u;
    for(unsigned lane=0;lane<4;++lane)if(lookKnown[lane]==0xffffffffu){
        h.look[lane]=look[lane];h.lookKnown|=std::uint8_t(1u<<lane);}
    if(h.lookKnown!=15u)return {S::phase,h.pc};
    h.pc=0x28a14cu;
    for(unsigned lane=0;lane<4;++lane)if(upKnown[lane]==0xffffffffu){
        h.up[lane]=up[lane];h.upKnown|=std::uint8_t(1u<<lane);}
    if(h.upKnown!=15u)return {S::phase,h.pc};
    h.inputsCopied=true;
    //Native numeric-domain admission is explicitly NOT a source branch.
    //The facade reuses only the reviewed instruction arithmetic, never the
    //whole visual-container caller, graph refresh or prepared Wreck pose.
    if(!detail::sourceParentlessBasis28A090(h.look,h.up,h.scratch,h.pc))return {S::phase,h.pc};
    h.numericReturned=true;
    //28A164 loaded saved stack30 into vf4: matrix translation XYZ is the
    //saved actual XYZ already at28A214, not a temporary zero translation.
    for(unsigned lane=0;lane<3;++lane)h.scratch[12+lane]=h.savedXYZ[lane];
    h.pc=0x28a468u;h.parentRead=true;
    if(root.parent180!=none){h.pc=0x28a474u;return {S::graph,h.pc};}
    //Eight paired scalar stores; each pair stores first word at28A4AC,
    //second at28A4B0. Record all16 writes, not an atomic matrix replacement.
    for(unsigned pair=0;pair<8;++pair)for(unsigned half=0;half<2;++half){
        const unsigned lane=2*pair+half;h.pc=half?0x28a4b0u:0x28a4acu;
        std::memcpy(root.localC0.v+lane,h.scratch.data()+lane,4);
        h.matrixMask|=std::uint16_t(1u<<lane);h.after[h.root]=root;}
    constexpr std::uint32_t stores[3]={0x28a4c8u,0x28a4d0u,0x28a4dcu};
    for(unsigned lane=0;lane<3;++lane){h.pc=stores[lane];
        std::memcpy(root.localC0.v+12+lane,h.savedXYZ.data()+lane,4);
        h.xyzMask|=std::uint8_t(1u<<lane);h.after[h.root]=root;}
    h.pc=0x28c50cu;root.sceneFlags190=std::uint8_t(root.sceneFlags190|0x80u);
    h.rootMarked=true;h.after[h.root]=root;
    //Parentless28C4F0 has no ancestor stores. The final copy-loop v0 is
    //translationW=0x3F800000, preserved by this helper;28A4F0 masks
    //its low bit and therefore contributes zero to bit0.
    if(root.sceneFlags190&1u){
        h.pc=0x28a4fcu;root.sceneFlags190=std::uint8_t(root.sceneFlags190&~1u);
        h.rootCleared=true;h.after[h.root]=root;
        const auto invalidate=[&](auto&& self,I index)->void {
            auto& node=at(graph,index);
            if(node.nextSibling188!=none)self(self,node.nextSibling188);
            if(!(node.sceneFlags190&1u))return;
            node.sceneFlags190=std::uint8_t(node.sceneFlags190&~1u);
            const auto i=static_cast<std::uint32_t>(index);
            h.invalidationOrder[h.invalidationCount++]=i;h.after[i]=node;
            if(node.firstChild184!=none)self(self,node.firstChild184);
        };
        if(root.firstChild184!=none){h.pc=0x28a50cu;invalidate(invalidate,root.firstChild184);}
    }
    h.returned=true;h.pc=0x28a528u;return {S::complete,h.pc};
}
inline bool OriginalRacerModelSceneOwner50LA::sourceOrientationHistoryNative(
    const SceneGraph49P& graph,const SourceOrientation28A090& h,
    const SourcePostPoseWorld28B550* laterWorld,const SourceObserverBounds28E230* laterBounds)const noexcept {
    if(laterBounds&&!laterWorld)return false;
    if(laterWorld&&(!h.returned||!laterWorld->entered||!sourcePostPoseWorldHistoryNative(graph,*laterWorld,laterBounds)))return false;
    if(!h.entered)return !h.owner&&!h.canonicalNodes&&!h.count&&!h.pc&&!h.returned&&!h.guard.entered;
    if(h.owner!=this||h.canonicalNodes!=graph.nodes||h.count!=graph.count||!h.count||h.count>maxSceneNodes49P||
       h.root>=h.count||h.root!=static_cast<std::uint32_t>(model_.sceneIndex)||
       !OriginalStandardPlacementGlobals50LA::basisCurrent(h.guard)||h.invalidationCount>h.count)return false;
    if(h.guardReturned&&!h.guard.entered)return false;
    if(!h.guardReturned&&(h.pc!=0x28a0a0u||h.scratchInitialized||h.lookKnown||h.upKnown||h.inputsCopied||
       h.numericReturned||h.parentRead||h.matrixMask||h.xyzMask||h.rootMarked||h.rootCleared||h.returned))return false;
    if(h.guardReturned&&!h.scratchInitialized)return false;
    if(h.scratchInitialized&&(h.savedXYZ[3]!=0x3f800000u||
       std::memcmp(h.savedXYZ.data(),h.before[h.root].localC0.v+12,12)))return false;
    if(h.lookKnown>15u||h.upKnown>15u||(h.upKnown&&h.lookKnown!=15u))return false;
    if(h.inputsCopied!=(h.lookKnown==15u&&h.upKnown==15u))return false;
    if(h.numericReturned&&(!h.inputsCopied||!h.parentRead||h.scratch[3]||h.scratch[7]||h.scratch[11]||
       h.scratch[15]!=0x3f800000u||std::memcmp(h.scratch.data()+12,h.savedXYZ.data(),12)))return false;
    if(h.parentRead&&!h.numericReturned)return false;
    if(!h.numericReturned&&h.scratch!=std::array<std::uint32_t,16>{})return false;
    //No fallible operation lies among the parentless writes: admitted calls
    //finish all stores and dirty effects. Partial native refusals occur only
    //before them, retaining guard/inputs and leaving graph bytes unchanged.
    if(h.returned){
        if(!h.numericReturned||h.before[h.root].parent180!=none||h.matrixMask!=0xffffu||h.xyzMask!=7u||
           !h.rootMarked||h.rootCleared!=bool(h.before[h.root].sceneFlags190&1u)||h.pc!=0x28a528u)return false;
    }else{
        if(h.matrixMask||h.xyzMask||h.rootMarked||h.rootCleared||h.invalidationCount)return false;
        if(h.guardReturned){
            if(h.lookKnown!=15u){if(h.pc!=0x28a138u||h.upKnown||h.inputsCopied||h.numericReturned)return false;}
            else if(h.upKnown!=15u){if(h.pc!=0x28a14cu||h.inputsCopied||h.numericReturned)return false;}
            else if(h.numericReturned){if(h.pc!=0x28a474u||h.before[h.root].parent180==none)return false;}
            else if(h.pc!=0x28a1a8u&&h.pc!=0x28a1c4u&&h.pc!=0x28a1e4u&&
                    h.pc!=0x28a1ecu&&h.pc!=0x28a1fcu)return false;
            //Numeric-refusal PC is retained from the narrow facade; it is
            //not a source return, graph mutation or parent conversion proof.
        }
    }
    std::array<std::uint8_t,maxSceneNodes49P> flags{};
    for(std::uint32_t i=0;i<h.count;++i)flags[i]=h.before[i].sceneFlags190;
    if(h.rootMarked)flags[h.root]|=0x80u;
    if(h.rootCleared)flags[h.root]&=std::uint8_t(~1u);
    if(!validateSceneGraph49P(graph))return false;
    for(std::uint32_t i=0;i<h.count;++i){const auto& b=h.before[i];const auto& a=h.after[i];const auto& c=laterWorld?laterWorld->before[i]:graph.nodes[i];
        if(b.parent180!=a.parent180||b.firstChild184!=a.firstChild184||b.nextSibling188!=a.nextSibling188||
           c.parent180!=b.parent180||c.firstChild184!=b.firstChild184||c.nextSibling188!=b.nextSibling188)return false;}
    unsigned event=0;bool valid=true;
    const auto replay=[&](auto&& self,I index)->void {
        if(index==none)return;const auto i=static_cast<std::uint32_t>(index);
        if(i>=h.count){valid=false;return;}
        const auto& node=h.before[i];
        if(node.nextSibling188!=none)self(self,node.nextSibling188);
        if(!(flags[i]&1u))return;
        if(event>=h.invalidationCount||h.invalidationOrder[event]!=i){valid=false;return;}
        ++event;flags[i]&=std::uint8_t(~1u);
        if(node.firstChild184!=none)self(self,node.firstChild184);
    };
    if(h.rootCleared)replay(replay,h.before[h.root].firstChild184);
    if(!valid||event!=h.invalidationCount)return false;
    for(std::uint32_t i=0;i<h.count;++i){const auto& b=h.before[i];const auto& a=h.after[i];const auto& c=laterWorld?laterWorld->before[i]:graph.nodes[i];
        if(std::memcmp(a.world100.v,b.world100.v,64)||std::memcmp(c.world100.v,b.world100.v,64)||
           a.sceneFlags190!=flags[i]||c.sceneFlags190!=flags[i])return false;
        for(unsigned lane=0;lane<16;++lane){std::uint32_t before,after,current;
            std::memcpy(&before,b.localC0.v+lane,4);std::memcpy(&after,a.localC0.v+lane,4);std::memcpy(&current,c.localC0.v+lane,4);
            const auto expected=i==h.root&&h.returned?h.scratch[lane]:before;
            if(after!=expected||current!=expected)return false;}
    }
    return true;
}
}
