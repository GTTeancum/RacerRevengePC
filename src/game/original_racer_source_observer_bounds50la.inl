#pragma once
namespace rr::game {
inline OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::produceSourceObserverBounds28E230(
    const SceneGraph49P& graph,std::array<std::uint32_t,4>& center,
    std::array<std::uint32_t,4>& half,SourceObserverBounds28E230& h)noexcept {
    if(h.entered||sourceBounds_.entered||sourceWarmGeometry_.attempted||&center==&half||
       !preflightSourceRenderableReturn26E820Native()||graph.nodes!=sourceConstructor_.canonicalNodes||
       graph.count!=binding_.sourceRowCount||!graph.count||graph.count>maxSceneNodes49P||
       !validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex)||
       at(graph,model_.sceneIndex).parent180!=none)return {S::phase,0x28e230u};
    h.entered=true;h.owner=this;h.canonicalNodes=graph.nodes;h.count=graph.count;
    h.root=static_cast<std::uint32_t>(model_.sceneIndex);
    for(std::uint32_t i=0;i<h.count;++i)h.before[i]=h.after[i]=graph.nodes[i];
    std::memcpy(h.bounds160Before.data(),model_.bounds160.data(),16);
    std::memcpy(h.bounds170Before.data(),model_.bounds170.data(),16);
    if(model_.radius198){std::uint32_t bits;std::memcpy(&bits,&*model_.radius198,4);h.radiusBefore=bits;}
    h.centerBefore=center;h.halfBefore=half;
    //Typed temporary backing for the existing reviewed float-output service;
    //raw copies preserve every partial output lane, including caller half.W.
    std::array<float,4> actualCenter{},actualHalf{};
    std::memcpy(actualCenter.data(),center.data(),16);std::memcpy(actualHalf.data(),half.data(),16);
    const auto r=produceSourceDirectBounds28E230(graph,actualCenter,actualHalf);
    std::memcpy(center.data(),actualCenter.data(),16);std::memcpy(half.data(),actualHalf.data(),16);
    h.centerAfter=center;h.halfAfter=half;h.pc=r.sourcePC;h.returned=r.okay();
    for(std::uint32_t i=0;i<h.count;++i)h.after[i]=graph.nodes[i];
    return r;
}
inline bool OriginalRacerModelSceneOwner50LA::sourceObserverBoundsHistoryNative(
    const SceneGraph49P& graph,const SourceObserverBounds28E230& h)const noexcept {
    if(!h.entered)return !h.owner&&!h.canonicalNodes&&!h.count&&!h.root&&!h.pc&&!h.returned;
    if(h.owner!=this||h.canonicalNodes!=graph.nodes||h.count!=graph.count||!h.count||h.count>maxSceneNodes49P||
       h.root>=h.count||h.root!=static_cast<std::uint32_t>(model_.sceneIndex)||
       !validateSceneGraph49P(graph)||h.before[h.root].parent180!=none)return false;
    const auto& s=sourceBounds_;
    if(!s.entered){if(h.returned||h.pc!=0x28e230u)return false;}
    else if(!preflightSourceBoundsHistoryNative()||s.warmZeroCount||s.rowCount!=h.count||
       s.flagsAtEntry!=h.before[h.root].sceneFlags190||s.halfWBefore!=h.halfBefore[3]||
       h.pc!=s.pc||h.returned!=s.returned||
       (s.aggregateStoreMask!=0u&&s.aggregateStoreMask!=7u)||
       s.aggregateReturned!=(s.aggregateStoreMask==7u))return false;
    std::array<std::uint8_t,maxSceneNodes49P> flags{};
    std::array<bool,maxSceneNodes49P> refreshed{};
    for(std::uint32_t i=0;i<h.count;++i){
        const auto& b=h.before[i];const auto& a=h.after[i];const auto& c=graph.nodes[i];
        if(b.parent180!=a.parent180||b.firstChild184!=a.firstChild184||b.nextSibling188!=a.nextSibling188||
           c.parent180!=a.parent180||c.firstChild184!=a.firstChild184||c.nextSibling188!=a.nextSibling188||
           std::memcmp(b.localC0.v,a.localC0.v,64)||std::memcmp(c.localC0.v,a.localC0.v,64)||
           std::memcmp(c.world100.v,a.world100.v,64)||c.sceneFlags190!=a.sceneFlags190)return false;
        flags[i]=b.sceneFlags190;
    }
    //Replay only readiness/ancestor visitation, not numeric arithmetic. The
    //immutable after words were captured from actual successful refresh calls.
    const auto refresh=[&](I index)->void {
        while(index!=none){const auto i=static_cast<std::uint32_t>(index);
            if(flags[i]&1u)break;flags[i]|=1u;refreshed[i]=true;index=h.before[i].parent180;}
    };
    if(s.entered){
        if(s.rootRefreshed)refresh(static_cast<I>(h.root));
        for(std::uint32_t row=0;row<h.count;++row){
            const auto n=s.rowRefreshes[row];if(n>4u||(!s.recompute&&n))return false;
            if(n){const auto* object=sourceRowObject(row);
                if(!object||!inRange(graph,object->sceneIndex)||row>s.row||
                   !(h.before[static_cast<std::uint32_t>(object->sceneIndex)].sceneFlags190&4u))return false;
                for(unsigned call=0;call<n;++call)refresh(object->sceneIndex);}
        }
        if(s.aggregateReturned){float radius;std::memcpy(&radius,&s.radius198,4);
            if(radius>0.f)flags[h.root]|=0x40u;}
        if(s.finalWorldRefreshed)refresh(static_cast<I>(h.root));
    }
    for(std::uint32_t i=0;i<h.count;++i){
        if(h.after[i].sceneFlags190!=flags[i])return false;
        if(!refreshed[i]&&std::memcmp(h.before[i].world100.v,h.after[i].world100.v,64))return false;
        if(refreshed[i]&&h.before[i].parent180==none&&
           std::memcmp(h.after[i].world100.v,h.before[i].localC0.v,64))return false;
    }
    const auto& center160=s.entered&&(s.aggregateStoreMask&1u)?s.aggregate160:h.bounds160Before;
    const auto& half170=s.entered&&(s.aggregateStoreMask&2u)?s.aggregate170:h.bounds170Before;
    if(std::memcmp(model_.bounds160.data(),center160.data(),16)||
       std::memcmp(model_.bounds170.data(),half170.data(),16))return false;
    const auto radius=s.entered&&(s.aggregateStoreMask&4u)?std::optional<std::uint32_t>(s.radius198):h.radiusBefore;
    if(bool(radius)!=bool(model_.radius198))return false;
    if(radius){std::uint32_t bits;std::memcpy(&bits,&*model_.radius198,4);if(bits!=*radius)return false;}
    if(h.centerAfter!=(s.entered&&s.centerStored?s.center:h.centerBefore))return false;
    for(unsigned lane=0;lane<4;++lane){const auto expected=s.entered&&lane<3&&(s.halfStoreMask&(1u<<lane))?
        s.half[lane]:h.halfBefore[lane];if(h.halfAfter[lane]!=expected)return false;}
    return true;
}
}
