#pragma once
namespace rr::game {
inline OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::translateSourceNullRelative28ADD0(
    const SceneGraph49P& graph,const std::uint32_t* input,const std::uint32_t* known,SourceTranslation28ADD0& h)noexcept {
    if(h.entered||!input||!known||!preflightSourceRenderableReturn26E820Native()||
       graph.nodes!=sourceConstructor_.canonicalNodes||graph.count!=binding_.sourceRowCount||
       !graph.count||graph.count>maxSceneNodes49P||!validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex))
        return {S::phase,0x28add0u};
    h.entered=true;h.owner=this;h.canonicalNodes=graph.nodes;h.count=graph.count;
    h.root=static_cast<std::uint32_t>(model_.sceneIndex);
    for(std::uint32_t i=0;i<h.count;++i)h.before[i]=h.after[i]=graph.nodes[i];
    h.pc=0x28addcu;
    if(!OriginalStandardPlacementGlobals50LA::enterPosition28ADD0(h.guard))return {S::phase,h.pc};
    h.guardReturned=true;
    //Actual relative argument is NULL, not Entity.parent1C. No numeric kernel.
    h.pc=0x28aefcu;h.scratchInitialized=true;
    h.pc=0x28af00u;h.parentRead=true;
    auto& root=graph.nodes[h.root];
    if(root.parent180!=none){h.pc=0x28af0cu;return {S::graph,h.pc};}
    h.pc=0x28af20u;
    for(unsigned lane=0;lane<4;++lane)if(known[lane]==0xffffffffu){h.input[lane]=input[lane];h.inputKnown|=std::uint8_t(1u<<lane);}
    if(h.inputKnown!=15u)return {S::phase,h.pc}; //No unknown raw word is invented.
    h.scratch=h.input;h.inputCopied=true;
    constexpr std::uint32_t stores[3]={0x28af30u,0x28af38u,0x28af44u};
    for(unsigned lane=0;lane<3;++lane){h.pc=stores[lane];
        std::memcpy(root.localC0.v+12+lane,h.scratch.data()+lane,4);
        h.xyzMask|=std::uint8_t(1u<<lane);h.after[h.root]=root;}
    //28C4F0 always stores bit80, including the already-set case. This
    //qualified branch is parentless, so there are no ancestor stores.
    h.pc=0x28c50cu;root.sceneFlags190=std::uint8_t(root.sceneFlags190|0x80u);
    h.rootMarked=true;h.after[h.root]=root;
    if(root.sceneFlags190&1u){
        h.pc=0x28af68u;root.sceneFlags190=std::uint8_t(root.sceneFlags190&~1u);
        h.rootCleared=true;h.after[h.root]=root;
        //289A20: sibling recursion first even if this node is already dirty;
        //a dirty node skips its own children. Same source order as49P helper,
        //without that helper's unrelated FE_TONEAREST admission on raw copies.
        const auto invalidate=[&](auto&& self,I index)->void {
            auto& node=at(graph,index);
            if(node.nextSibling188!=none)self(self,node.nextSibling188);
            if(!(node.sceneFlags190&1u))return;
            node.sceneFlags190=std::uint8_t(node.sceneFlags190&~1u);
            const auto i=static_cast<std::uint32_t>(index);
            h.invalidationOrder[h.invalidationCount++]=i;h.after[i]=node;
            if(node.firstChild184!=none)self(self,node.firstChild184);
        };
        if(root.firstChild184!=none){h.pc=0x28af78u;invalidate(invalidate,root.firstChild184);}
    }
    h.returned=true;h.pc=0x28af90u;return {S::complete,h.pc};
}
inline bool OriginalRacerModelSceneOwner50LA::sourceTranslationHistoryNative(
    const SceneGraph49P& graph,const SourceTranslation28ADD0& h,const SourceOrientation28A090* laterOrientation,
    const SourcePostPoseWorld28B550* laterWorld,const SourceObserverBounds28E230* laterBounds)const noexcept {
    if((laterWorld&&!laterOrientation)||(laterBounds&&!laterWorld))return false;
    if(laterOrientation&&(!h.returned||!laterOrientation->entered||!sourceOrientationHistoryNative(graph,*laterOrientation,laterWorld,laterBounds)))return false;
    if(!h.entered)return !h.owner&&!h.canonicalNodes&&!h.count&&!h.pc&&!h.returned&&!h.guard.entered;
    if(h.owner!=this||h.canonicalNodes!=graph.nodes||h.count!=graph.count||!h.count||h.count>maxSceneNodes49P||
       h.root>=h.count||h.root!=static_cast<std::uint32_t>(model_.sceneIndex)||
       !OriginalStandardPlacementGlobals50LA::positionCurrent(h.guard)||h.invalidationCount>h.count)return false;
    if(h.guardReturned&&!h.guard.entered)return false;
    if(!h.guardReturned&&(h.pc!=0x28addcu||h.parentRead||h.scratchInitialized||h.inputCopied||h.inputKnown||h.xyzMask||h.rootMarked||h.rootCleared||h.returned))return false;
    if(h.guardReturned&&(!h.parentRead||!h.scratchInitialized))return false;
    if(h.inputCopied&&(h.inputKnown!=15u||h.scratch!=h.input||h.before[h.root].parent180!=none))return false;
    if(!h.inputCopied&&(h.scratch!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}}||h.xyzMask||h.rootMarked||h.rootCleared||h.returned))return false;
    if(h.xyzMask!=0u&&h.xyzMask!=1u&&h.xyzMask!=3u&&h.xyzMask!=7u)return false;
    if(h.rootMarked&&h.xyzMask!=7u)return false;
    if(h.rootCleared&&(!h.rootMarked||!(h.before[h.root].sceneFlags190&1u)))return false;
    if(h.returned&&(!h.inputCopied||h.xyzMask!=7u||!h.rootMarked||h.pc!=0x28af90u||
        h.rootCleared!=bool(h.before[h.root].sceneFlags190&1u)))return false;
    if(h.guardReturned&&!h.returned){
        if(h.before[h.root].parent180!=none){if(h.pc!=0x28af0cu||h.inputKnown||h.inputCopied)return false;}
        else if(h.pc!=0x28af20u||h.inputKnown==15u||h.inputCopied)return false;
    }
    std::array<std::uint8_t,maxSceneNodes49P> flags{};
    for(std::uint32_t i=0;i<h.count;++i)flags[i]=h.before[i].sceneFlags190;
    if(h.rootMarked)flags[h.root]|=0x80u;
    if(h.rootCleared)flags[h.root]&=std::uint8_t(~1u);
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
    //Current topology was authenticated before effects; check it again before
    //walking retained topology, so mutated links cannot make history recurse.
    if(!validateSceneGraph49P(graph))return false;
    for(std::uint32_t i=0;i<h.count;++i){const auto& b=h.before[i];const auto& a=h.after[i];const auto& c=laterOrientation?laterOrientation->before[i]:graph.nodes[i];
        if(b.parent180!=a.parent180||b.firstChild184!=a.firstChild184||b.nextSibling188!=a.nextSibling188||
           c.parent180!=b.parent180||c.firstChild184!=b.firstChild184||c.nextSibling188!=b.nextSibling188)return false;}
    if(h.rootCleared)replay(replay,h.before[h.root].firstChild184);
    if(!valid||event!=h.invalidationCount)return false;
    for(std::uint32_t i=0;i<h.count;++i){const auto& b=h.before[i];const auto& a=h.after[i];const auto& c=laterOrientation?laterOrientation->before[i]:graph.nodes[i];
        if(std::memcmp(a.world100.v,b.world100.v,64)||std::memcmp(c.world100.v,b.world100.v,64)||
           a.sceneFlags190!=flags[i]||c.sceneFlags190!=flags[i])return false;
        for(unsigned lane=0;lane<16;++lane){std::uint32_t before,after,current;
            std::memcpy(&before,b.localC0.v+lane,4);std::memcpy(&after,a.localC0.v+lane,4);std::memcpy(&current,c.localC0.v+lane,4);
            const auto expected=i==h.root&&lane>=12&&lane<15&&(h.xyzMask&(1u<<(lane-12)))?h.input[lane-12]:before;
            if(after!=expected||current!=expected)return false;}
    }
    return true;
}
}
