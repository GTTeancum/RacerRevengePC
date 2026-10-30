#pragma once
namespace rr::game {
inline OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::produceSourceDirectBounds28E230(
    const SceneGraph49P& graph,std::array<float,4>& center,std::array<float,4>& half)noexcept {
    auto& h=sourceBounds_;
    const bool warm=sourceWarmGeometry_.attempted;
    const bool sourceReady=warm?preflightSourceWarmRenderable26E820Native():
        (sourceDirectRows_.returned&&!sourceDirectRows_.nativeScratchWithdrawn&&
         sourceGeometry_.geometryCallReturned&&!sourceGeometry_.nativeWithdrawn);
    if(h.entered||&center==&half||!sourceConstructor_.modelReturned||
       graph.nodes!=sourceConstructor_.canonicalNodes||!binding_.sessionGeneration||
       model_.entity8.sessionGeneration!=binding_.sessionGeneration||
       !sourceReady||
       !constructed2403A0()||!rowsLoaded_||referenceReleased_||released_||
       rootMode_!=RootMode::direct||
       (warm?!(warmChildlessRows_||warmHierarchyRows_):(warmChildlessRows_||warmHierarchyRows_))||!rowTable_||
       fields_.sourceRows1B8!=rowTable_.get()||!fields_.geometry1B4||
       (warm?(model_.nodeCount1A0!=0||!binding_.sourceRowCount||graph.count!=binding_.sourceRowCount):
         (model_.nodeCount1A0!=binding_.sourceRowCount||!model_.nodeCount1A0||graph.count!=model_.nodeCount1A0))||
       model_.nodeCount1A0>maxModelBoundsNodes49O||
       !validateSceneGraph49P(graph)||!inRange(graph,model_.sceneIndex)||
       sourceRowObject(0)!=&model_||rowTable_[0]!=&model_||
       at(graph,model_.sceneIndex).parent180!=none)return {S::phase,0x28e230u};
    //Native output aliases are required disjoint from the retained source
    //bounds rows. This is admission, not a source pointer comparison.
    //Native alias admission covers the retained root even when original1A0
    //is zero; it is not an extra source iteration or transform.
    for(std::uint32_t i=0;i<binding_.sourceRowCount;++i){const auto* row=sourceRowObject(i);
        if(!row)return {S::sourceRowTable,0x28e230u};
        for(const auto* box:{&row->bounds140,&row->bounds150,&row->bounds160,&row->bounds170})
            if(&center==box||&half==box)return {S::phase,0x28e230u};
    }
    h.entered=true;h.warmZeroCount=warm;h.rowCount=model_.nodeCount1A0;
    const auto stop=[&](S status,std::uint32_t pc){h.pc=pc;return R{status,pc};};
    auto& root=at(graph,model_.sceneIndex);h.flagsAtEntry=root.sceneFlags190;
    std::memcpy(&h.halfWBefore,&half[3],4);
    h.recompute=(h.flagsAtEntry&0x80u)&&((h.flagsAtEntry&8u)||!(h.flagsAtEntry&0x40u));
    if(h.recompute){
        h.pc=0x28e280;if(sourceConstructor_.vtable1A8!=0x3d1f40)return stop(S::phase,h.pc);
        h.pc=0x26f4c0;if(h.flagsAtEntry&0x10u)return stop(S::phase,h.pc); //Separate sphere route.
        h.pc=0x26f5e4;if(!OriginalStandardPlacementGlobals50LA::bounds26F5E4(h.profileGuard))return stop(S::phase,h.pc);
        //Actual lazy writes occur in source order, not an all-or-nothing
        //precomputed graph adoption. Later admission refusal retains them.
        h.pc=0x26f608;if(!refreshSceneWorld49P(graph,model_.sceneIndex))return stop(S::graph,h.pc);
        h.rootRefreshed=true;
        if(!(root.sceneFlags190&4u))return stop(S::phase,0x26f628u); //Existing direct adapter requires own seed.
        ModelBoundsContext49O context{};context.rootWorld100=root.world100;context.rootSceneFlags190=root.sceneFlags190;
        for(unsigned axis=0;axis<3;++axis){context.rootCenter140.v[axis]=model_.bounds140[axis];
            context.rootHalfExtent150.v[axis]=model_.bounds150[axis];}
        std::array<ModelBoundsNode49O,maxModelBoundsNodes49O> rows{};
        for(h.row=0;h.row<h.rowCount;++h.row){
            h.pc=0x26f6e0;const auto* row=sourceRowObject(h.row);
            if(!row||rowTable_[h.row]!=row||!row->constructed||row->released||!inRange(graph,row->sceneIndex))
                return stop(S::sourceRowTable,h.pc);
            const auto parent=sourceParents_[h.row];auto& node=at(graph,row->sceneIndex);
            if(h.row==0){if(parent!=-1||node.parent180!=none)return stop(S::hierarchy,0x28daf0u);}
            else if(parent<0||parent>=static_cast<std::int32_t>(h.row)||
                !sourceRowObject(static_cast<std::uint32_t>(parent))||
                node.parent180!=sourceRowObject(static_cast<std::uint32_t>(parent))->sceneIndex)
                return stop(S::hierarchy,0x28daf0u);
            rows[h.row].local=node.localC0;rows[h.row].parent=parent;rows[h.row].sceneFlags190=node.sceneFlags190;
            for(unsigned axis=0;axis<3;++axis){rows[h.row].center140.v[axis]=row->bounds140[axis];
                rows[h.row].halfExtent150.v[axis]=row->bounds150[axis];}
            if(node.sceneFlags190&4u)for(unsigned getter=0;getter<4;++getter){
                h.pc=0x28db9c;if(!refreshSceneWorld49P(graph,row->sceneIndex))return stop(S::graph,h.pc);
                ++h.rowRefreshes[h.row];
            }
        }
        ModelBoundsWorkspace49O workspace{};ModelBoundsResult49O aggregate{};
        h.pc=0x26f718;
        const bool calculated=h.warmZeroCount?
            computeModelBounds49O(nullptr,0,context,workspace,aggregate):
            computeDirectRootModelBounds50KG(rows.data(),h.rowCount,context,workspace,aggregate);
        if(!calculated)return stop(S::graph,h.pc);
        //The accepted math helper eagerly regenerates row worlds. Its values
        //are not authority to replace a different already-ready source cache.
        for(std::uint32_t i=0;i<h.rowCount;++i)if(rows[i].sceneFlags190&4u)
            if(std::memcmp(workspace.world[i].v,at(graph,sourceRowObject(i)->sceneIndex).world100.v,sizeof workspace.world[i].v))
                return stop(S::graph,0x26f718u);
        //Native finite/current-transform admission precedes these source
        //publications; no claim of PS2 exceptional-mode/bitwise parity.
        h.pc=0x26f780;std::memcpy(model_.bounds160.data(),aggregate.center160,16);
        std::memcpy(h.aggregate160.data(),aggregate.center160,16);h.aggregateStoreMask|=1u;
        h.pc=0x26f7e8;std::memcpy(model_.bounds170.data(),aggregate.halfExtent170,16);
        std::memcpy(h.aggregate170.data(),aggregate.halfExtent170,16);h.aggregateStoreMask|=2u;
        h.pc=0x26f828;model_.radius198=aggregate.radius198;
        std::memcpy(&h.radius198,&aggregate.radius198,4);h.aggregateStoreMask|=4u;
        if(aggregate.radius198>0.f){h.pc=0x26f840;root.sceneFlags190=std::uint8_t(root.sceneFlags190|0x40u);}
        h.aggregateReturned=true;h.pc=0x26f854;
    }else if((h.flagsAtEntry&0x80u)&&!(h.flagsAtEntry&1u)){
        h.pc=0x28e2a4;if(!refreshSceneWorld49P(graph,model_.sceneIndex))return stop(S::graph,h.pc);
        h.rootRefreshed=true;
    }
    //This fresh direct source owner has no other genuine aggregate producer.
    //Native default/preloaded160/170, or a radius optional by itself, is not
    //proof of cached source bounds on the dirty-gate skip branch.
    if(!h.aggregateReturned)return stop(S::phase,0x28e330u);
    h.pc=0x28e388;if(!(root.sceneFlags190&1u)){
        if(!refreshSceneWorld49P(graph,model_.sceneIndex))return stop(S::graph,h.pc);h.finalWorldRefreshed=true;
    }
    const auto& world=root.world100;
    if(world.v[3]!=0.f||world.v[7]!=0.f||world.v[11]!=0.f||world.v[15]!=1.f)return stop(S::graph,0x28e410u);
    CollectionBoundsInput48S input{};input.shapeTag=1;input.shapeWorld=world;
    for(unsigned axis=0;axis<4;++axis){input.localCenter.v[axis]=model_.bounds160[axis];input.localHalf.v[axis]=model_.bounds170[axis];}
    CollectionBoundsResult48S output{};
    if(prepareCollectionBounds48S(input,output)!=CollectionBoundsStatus48S::Complete)return stop(S::graph,0x28e410u);
    h.pc=0x28e43c;std::memcpy(center.data(),output.center.v,16);std::memcpy(h.center.data(),output.center.v,16);h.centerStored=true;
    constexpr std::uint32_t stores[3]={0x28e524,0x28e56c,0x28e5b0};
    for(unsigned axis=0;axis<3;++axis){h.pc=stores[axis];std::memcpy(&half[axis],&output.half.v[axis],4);
        std::memcpy(&h.half[axis],&output.half.v[axis],4);h.halfStoreMask|=std::uint8_t(1u<<axis);}
    h.returned=true;h.pc=0x28e5c8;return {S::complete,h.pc}; //No observer/Entity attachment issuer.
}
inline bool OriginalRacerModelSceneOwner50LA::preflightSourceBoundsHistoryNative()const noexcept {
    const auto& h=sourceBounds_;if(!h.entered)return !h.profileGuard.entered&&!h.aggregateReturned&&!h.returned;
    if((h.warmZeroCount?h.rowCount!=0:!h.rowCount)||h.rowCount>maxModelBoundsNodes49O||h.row>h.rowCount||
       h.recompute!=bool((h.flagsAtEntry&0x80u)&&((h.flagsAtEntry&8u)||!(h.flagsAtEntry&0x40u)))||
       !OriginalStandardPlacementGlobals50LA::boundsCurrent(h.profileGuard)||
       h.aggregateStoreMask>7u||h.halfStoreMask>7u)return false;
    for(auto count:h.rowRefreshes)if(count>4)return false;
    if(h.aggregateReturned&&(!h.recompute||!h.rootRefreshed||!h.profileGuard.entered||h.aggregateStoreMask!=7u||h.row!=h.rowCount))return false;
    if(h.centerStored&&!h.aggregateReturned)return false;
    if(h.halfStoreMask&&!h.centerStored)return false;
    return !h.returned||(h.centerStored&&h.halfStoreMask==7u&&h.pc==0x28e5c8u);
}
}
