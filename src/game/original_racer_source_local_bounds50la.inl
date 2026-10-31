#pragma once
namespace rr::game {
inline OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::produceSourceLocalBounds28D670(
    const SceneGraph49P& graph,std::array<std::uint32_t,4>& actualCenter,
    std::array<std::uint32_t,4>& actualHalf,SourceLocalBounds28D670& h)noexcept {
    if(h.entered||&actualCenter==&actualHalf||!preflightSourceRenderableReturn26E820Native()||!sourceDirectRows_.returned||
       sourceDirectRows_.nativeScratchWithdrawn||rootMode_!=RootMode::direct||
       graph.nodes!=sourceConstructor_.canonicalNodes||graph.count!=binding_.sourceRowCount||
       !graph.count||graph.count>maxModelBoundsNodes49O||!validateSceneGraph49P(graph)||
       !inRange(graph,model_.sceneIndex)||at(graph,model_.sceneIndex).parent180!=none)
        return {S::phase,0x28d670u}; //Native domain: actual cold parentless source hierarchy.
    std::array<const O*,maxSceneNodes49P> objects{};
    for(std::uint32_t row=0;row<graph.count;++row){const auto* object=sourceRowObject(row);
        if(!object||!object->constructed||object->released||!inRange(graph,object->sceneIndex)||
           !rowTable_||rowTable_[row]!=object||sourceDirectRows_.readMask[row]!=63u)return {S::sourceRowTable,0x28d670u};
        const auto index=static_cast<std::uint32_t>(object->sceneIndex);
        if(objects[index])return {S::hierarchy,0x28d670u};objects[index]=object;
    }
    const auto root=static_cast<std::uint32_t>(model_.sceneIndex);
    if(objects[root]!=&model_)return {S::hierarchy,0x28d670u};
    std::array<std::uint32_t,maxSceneNodes49P> order{};std::uint32_t count=0;
    const auto visit=[&](auto&& self,I index)->void {
        order[count++]=static_cast<std::uint32_t>(index);
        for(auto child=at(graph,index).firstChild184;child!=none;child=at(graph,child).nextSibling188)self(self,child);
    };
    visit(visit,model_.sceneIndex);
    if(count!=graph.count)return {S::hierarchy,0x28d670u}; //No unrelated detached forest is silently adopted.
    h.owner=this;h.canonicalNodes=graph.nodes;h.count=count;h.root=root;h.objects=objects;h.order=order;h.entered=true;
    for(std::uint32_t i=0;i<count;++i){h.before[i]=h.after[i]=graph.nodes[i];
        h.center140[i]=objects[i]->bounds140;h.half150[i]=objects[i]->bounds150;}
    const auto stop=[&](S status,std::uint32_t pc){h.pc=pc;return R{status,pc};};
    //Root inline copy28D6D8..28D710 is copy-only; Entity.parent1C is irrelevant.
    auto& rootNode=graph.nodes[root];h.pc=0x28d698u;
    if(!(rootNode.sceneFlags190&1u)){
        for(unsigned pair=0;pair<8;++pair)for(unsigned lane=0;lane<2;++lane)
            std::memcpy(rootNode.world100.v+2*pair+lane,rootNode.localC0.v+2*pair+lane,4);
        rootNode.sceneFlags190=static_cast<std::uint8_t>(rootNode.sceneFlags190|1u);
        h.worldWritten[root]=true;h.after[root]=rootNode;
    }
    h.rootReady=true;h.pc=0x28d770u;
    Vector3_48C rootCenter{},rootHalf{};
    for(unsigned j=0;j<3;++j){rootCenter.v[j]=h.center140[root][j];rootHalf.v[j]=h.half150[root][j];}
    if(!seedLocalBounds28D670(rootCenter,rootHalf,(rootNode.sceneFlags190&4u)!=0,h.aggregate))return stop(S::graph,h.pc);
    h.seedReturned=true;h.visited=1;
    //Each actual2897B0 request refreshes dirty ancestors individually, oldest
    //first. A later numeric refusal cannot erase already reached ancestor stores.
    const auto refresh=[&](I index)->bool {
        std::array<I,maxSceneNodes49P> chain{};unsigned size=0;auto current=index;
        while(current!=none&&!(at(graph,current).sceneFlags190&1u)){chain[size++]=current;current=at(graph,current).parent180;}
        while(size){const auto item=chain[--size];
            if(!refreshSceneWorld49P(graph,item))return false;
            const auto i=static_cast<std::uint32_t>(item);h.worldWritten[i]=true;h.after[i]=graph.nodes[i];
        }
        return true;
    };
    constexpr std::uint32_t getterPC[4]={0x28db9cu,0x28dbc8u,0x28dbf4u,0x28dc20u};
    for(std::uint32_t ordinal=1;ordinal<count;++ordinal){const auto i=order[ordinal];
        h.pc=0x28daf0u;
        if(graph.nodes[i].sceneFlags190&4u){
            Vector3_48C center{},half{};for(unsigned j=0;j<3;++j){center.v[j]=h.center140[i][j];half.v[j]=h.half150[i][j];}
            auto& projection=h.projection[i];
            for(unsigned getter=0;getter<4;++getter){h.pc=getterPC[getter];
                if(!refresh(static_cast<I>(i)))return stop(S::graph,h.pc);++h.getterCount[i];
                h.pc=getterPC[getter]+8u;
                if(!captureLocalBounds28DAF0(graph.nodes[i].world100,center,half,getter,projection))return stop(S::graph,h.pc);
                h.projectionMask[i]|=std::uint8_t(1u<<getter);}
            h.pc=0x28dc38u;
            if(!mergeLocalBounds28DAF0(projection,rootNode.world100,h.aggregate))return stop(S::graph,h.pc);
        }
        h.visited=ordinal+1; //DFS184/188 order, not imported row enumeration or1A0 loop.
    }
    float output[4];h.pc=0x28d84cu;
    if(!finishLocalBounds28D670(h.aggregate,false,output))return stop(S::graph,h.pc);
    h.pc=0x28d898u;std::memcpy(actualCenter.data(),output,16);std::memcpy(h.center.data(),output,16);h.centerStored=true;
    h.pc=0x28d8c0u;
    if(!finishLocalBounds28D670(h.aggregate,true,output))return stop(S::graph,h.pc);
    h.pc=0x28d90cu;std::memcpy(actualHalf.data(),output,16);std::memcpy(h.half.data(),output,16);h.halfStored=true;
    h.returned=true;h.pc=0x28d93cu;return {S::complete,h.pc}; //JR/delay completed, control only.
}
inline bool OriginalRacerModelSceneOwner50LA::sourceLocalBoundsHistoryNative(
    const SceneGraph49P& graph,const SourceLocalBounds28D670& h,const SourceTranslation28ADD0* laterTranslation,
    const SourceOrientation28A090* laterOrientation,
    const SourcePostPoseWorld28B550* laterWorld,const SourceObserverBounds28E230* laterBounds)const noexcept {
    //Keep the original28D670 after image; only the authenticated sequence
    //28D670 -> translation.before/after -> orientation.before/after -> live
    //may account for later local/flag changes. Only the later getter can
    //change root world/ready state; its own before/after history proves those.
    if((laterOrientation&&!laterTranslation)||(laterWorld&&!laterOrientation)||(laterBounds&&!laterWorld))return false;
    if(laterTranslation&&(!laterTranslation->entered||
       !sourceTranslationHistoryNative(graph,*laterTranslation,laterOrientation,laterWorld,laterBounds)))return false;
    if(!h.entered)return !h.owner&&!h.canonicalNodes&&!h.count&&!h.pc&&!h.returned;
    if(h.owner!=this||h.canonicalNodes!=graph.nodes||h.count!=graph.count||!h.count||h.count>maxModelBoundsNodes49O||
       h.root>=h.count||h.order[0]!=h.root||h.objects[h.root]!=&model_||h.visited>h.count||!h.rootReady)return false;
    std::array<bool,maxSceneNodes49P> seen{};
    for(std::uint32_t ordinal=0;ordinal<h.count;++ordinal){const auto i=h.order[ordinal];if(i>=h.count||seen[i])return false;seen[i]=true;
        const auto getters=h.getterCount[i];if(getters>4)return false;
        const auto full=(1u<<getters)-1u;const auto partial=getters?((1u<<(getters-1u))-1u):0u;
        if(h.projectionMask[i]!=full&&h.projectionMask[i]!=partial)return false;
        const bool eligible=ordinal&&(h.before[i].sceneFlags190&4u);
        if(!eligible&&(getters||h.projectionMask[i]))return false;
        if(eligible&&ordinal<h.visited&&(getters!=4||h.projectionMask[i]!=15))return false;
        if(ordinal>h.visited&&(getters||h.projectionMask[i]))return false;
    }
    for(std::uint32_t i=0;i<h.count;++i){const auto& before=h.before[i];const auto& after=h.after[i];const auto& current=laterTranslation?laterTranslation->before[i]:graph.nodes[i];
        if(!h.objects[i]||before.parent180!=after.parent180||before.firstChild184!=after.firstChild184||
           before.nextSibling188!=after.nextSibling188||current.parent180!=after.parent180||current.firstChild184!=after.firstChild184||
           current.nextSibling188!=after.nextSibling188||std::memcmp(before.localC0.v,after.localC0.v,64)||
           std::memcmp(current.localC0.v,after.localC0.v,64)||std::memcmp(current.world100.v,after.world100.v,64)||
           current.sceneFlags190!=after.sceneFlags190)return false;
        if(h.worldWritten[i]){if((before.sceneFlags190&1u)||after.sceneFlags190!=std::uint8_t(before.sceneFlags190|1u))return false;}
        else if(before.sceneFlags190!=after.sceneFlags190||std::memcmp(before.world100.v,after.world100.v,64))return false;
    }
    if(h.seedReturned!=(h.visited!=0))return false;
    if(h.centerStored&&(!h.seedReturned||h.visited!=h.count))return false;
    if(h.halfStored&&!h.centerStored)return false;
    return !h.returned||(h.centerStored&&h.halfStored&&h.pc==0x28d93cu);
}
}
