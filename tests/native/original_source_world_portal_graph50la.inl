#pragma once
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldPortalGraphNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourcePortalGraphPhase_==SourcePortalGraphPhase::unreached){
        if(sourcePortalGraphS0_||sourcePortalParameterFreeReached_||sourcePortalParameterNullStored_||sourcePortalCohortAdmitted_)return false;
        for(auto mask:sourcePortalCallerMask_)if(mask)return false;
        for(const auto& row:sourcePortalLinkArguments_)for(const auto& args:row)for(const auto& value:args)if(value)return false;
        for(const auto& row:sourcePortalLinkArrays_)for(const auto& value:row)if(value)return false;
        return true;
    }
    if(!sourcePortalGraphS0_||*sourcePortalGraphS0_>10||!sourcePortalCohortAdmitted_)return false;
    if(sourcePortalGraphPhase_==SourcePortalGraphPhase::nativeClosed){
        for(const auto& p:nativePortals_)if(p)return false;
        return true; //No historical Portal, parent, parameter or link pointer is dereferenced.
    }
    if(!sourceTopologyMode_||sourceTopologyWithdrawn_||!worldApplication_||
       !preflightSourcePortalOwnersNative(sourceTopologyContainer_.get())||
       !nativePortalParameters3A0_||!nativePortalParameters3A0_->sourceMode||
       !sourcePortal3A0_||nativePortalParameters3A0_->sourceReleased!=sourcePortalParameterFreeReached_)return false;
    const auto& role=*nativePortalParameters3A0_;
    if(sourcePortalParameterFreeReached_){
        if(role.sourceArray||*sourcePortalGraphS0_!=10)return false;
        //After actual free the pointer may be a historical dangling value until
        //the following real272C8C store. Never read its pointee.
        if(sourcePortalParameterNullStored_&&*sourcePortal3A0_)return false;
    }else if(*sourcePortal3A0_!=role.sourceArray.get()||!role.sourceArray||sourcePortalParameterNullStored_)return false;
    for(std::size_t i=0;i<10;++i){
        const auto mask=sourcePortalCallerMask_[i];if(mask&~63u)return false;
        if(i<*sourcePortalGraphS0_&&mask!=63u)return false;
        if(i>*sourcePortalGraphS0_&&mask)return false;
        const auto* p=nativePortals_[i].get();
        if(sourcePortalFrames_[i].withdrawn){if(p)return false;continue;}
        if((mask&2u)&&(!p||!p->sourceMode||!p->source.loadReturned))return false;
        if((mask&4u)&&(!p||!p->attached))return false;
        if((mask&8u)&&(!p||!p->source.connectA||!p->source.connectB))return false;
        if((mask&16u)&&(!p||p->source.worldLinksProduced!=4))return false;
        for(std::size_t stage=0;stage<4;++stage){const auto& args=sourcePortalLinkArguments_[i][stage];
            const auto& link=sourcePortalWorldLinks_[i][stage];
            const auto& array=sourcePortalLinkArrays_[i][stage];
            if(array&&(!(mask&4u)||*array!=(stage&1u?sourceCollisionSlots4C4_.get():sourceVisibilitySlots4C0_.get())))return false;
            if((args[0]||args[1])&&!array)return false;
            if(link.attempted&&(!args[0]||!args[1]||*args[0]!=link.from||*args[1]!=link.to))return false;
            if((args[0]||args[1])&&(!(mask&8u)||!p))return false;
        }
    }
    if(sourcePortalGraphPhase_==SourcePortalGraphPhase::returned)
        return *sourcePortalGraphS0_==10&&sourcePortalParameterFreeReached_&&sourcePortalParameterNullStored_;
    return true;
}
bool Owner::sourceWorldPortalGraphCounted(std::size_t& count)const noexcept {
    count=0;if(!preflightSourceWorldPortalGraphNative())return false;
    if(sourcePortalGraphPhase_==SourcePortalGraphPhase::unreached||sourcePortalGraphPhase_==SourcePortalGraphPhase::nativeClosed)return true;
    return sourcePortalOwnersCounted(count);
}
bool Owner::sourcePortalTailCurrent148D00(std::size_t ordinal,const g::OriginalWreckComponent50LA* parent,
    const g::OriginalWreckComponent50LA* portal,std::uint32_t a,std::uint32_t b)const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id()||ordinal>=10||a>=10||b>=10||a==b||
       sourcePortalGraphPhase_==SourcePortalGraphPhase::unreached||sourcePortalGraphPhase_==SourcePortalGraphPhase::nativeClosed||
       !sourcePortalGraphS0_||ordinal>*sourcePortalGraphS0_||!sourceTopologyMode_||sourceTopologyWithdrawn_||
       !sourceLoadRace_||!sourceLoadApplication_||sourceLoadRace_->nativeIntroWorldEntity()!=&worldEntity_||
       sourceLoadApplication_->entities()!=worldApplication_||parent!=sourceTopologyContainer_.get()||
       !sourceContainer4BC_||*sourceContainer4BC_!=parent||!parent||!portal||
       !preflightSourcePortalOwnersNative(parent))return false;
    const auto* p=nativePortals_[ordinal].get();
    return p&&p->sourceMode&&p->counted&&p->source.enrolled&&p->source.constructorReturned&&
        p->source.loadReturned&&p->attached&&&p->entity==portal&&portal->parent1C==parent&&
        p->source.worldLinksProduced==4&&p->source.connectA&&p->source.connectB&&
        *p->source.connectA==a&&*p->source.connectB==b&&(sourcePortalCallerMask_[ordinal]&31u)==31u;
}
bool Owner::sourceWorldPortalGraphInvocationCurrent()const noexcept {
    return nativeScbThread_==std::this_thread::get_id()&&!closed_&&!terminal_&&!nativeScbRevoked_&&
        sourceTopologyPhase_==SourceTopologyPhase::nodesReturned&&sourceTopologyPC_==0x272b28&&sourceTrackPC_==0x272b28&&
        sourceSearchNodesPhase_==SourceSearchNodesPhase::returned&&sourceSearchNodesPC_==0x272b28&&
        sourceTopologyMode_&&!sourceTopologyWithdrawn_&&sourceTopologyLightManager_&&
        sourceLoadRace_&&sourceLoadApplication_&&sourceLoadRace_->nativeIntroWorldEntity()==&worldEntity_&&
        sourceLoadApplication_->entities()==worldApplication_&&
        sourceLoadApplication_->sourceLightManager426F60Owner_==sourceTopologyLightManager_&&
        worldPortals358_&&*worldPortals358_==10&&sourcePortalsPhase_==SourcePortalsPhase::returned&&
        sourcePortalsConstructed_==10&&sourcePortalsLoaded_==10&&nativePortalParameters3A0_&&
        nativePortalParameters3A0_->sourceMode&&!nativePortalParameters3A0_->sourceReleased&&
        nativePortalParameters3A0_->sourceArray&&sourcePortal3A0_&&
        *sourcePortal3A0_==nativePortalParameters3A0_->sourceArray.get()&&
        preflightSourceWorldPortalsNative()&&preflightSourceWorldTopologyCountNative();
}
bool Owner::abortSourceWorldPortalGraphNative(std::uint32_t& pc)noexcept {
    if(sourceMethodArraysPhase_!=SourceMethodArraysPhase::unreached&&sourceMethodArraysPhase_!=SourceMethodArraysPhase::nativeClosed)return false;
    if(!preflightSourceWorldTopologyCountNative())return false;
    if(sourcePortalGraphPhase_==SourcePortalGraphPhase::unreached||sourcePortalGraphPhase_==SourcePortalGraphPhase::nativeClosed)return true;
    if(sourceTopologyLightManager_){rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
        if(!sourceTopologyLightManager_->withdrawSourceRaPortalLinksNative(*this,d)){pc=d.sourcePC;return false;}}
    if(!withdrawSourcePortalWorldLinksNative(pc)||!retireSourcePortalsNative(sourceTopologyContainer_.get(),pc))return false;
    sourcePortalGraphPhase_=SourcePortalGraphPhase::nativeClosed;return true;
}
}

//Evolving parent caller fragment; component declarations merged before sealing.
namespace ra_cold_aggregate50LA {
bool Owner::continueSourceWorldPortalGraph272B28(std::uint32_t& pc)noexcept {
    pc=sourcePortalGraphPC_;
    if(sourcePortalGraphPhase_!=SourcePortalGraphPhase::unreached||
       !sourceWorldPortalGraphInvocationCurrent()||!reserveSourcePortalCohortNative())return false;
    sourcePortalGraphPhase_=SourcePortalGraphPhase::entered;
    const auto at=[&](std::uint32_t p){pc=sourcePortalGraphPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourcePortalGraphPhase_=SourcePortalGraphPhase::bounded;return false;};
    at(0x272b28);if(!worldPortals358_||*worldPortals358_!=10)return stop(pc);
    at(0x272b38);sourcePortalGraphS0_=0;
    for(;;){
        at(0x272c68);if(!worldPortals358_)return stop(pc);const auto count=*worldPortals358_;
        at(0x272c6c);if(static_cast<std::int32_t>(*sourcePortalGraphS0_)>=static_cast<std::int32_t>(count))break;
        const auto i=static_cast<std::size_t>(*sourcePortalGraphS0_);if(i>=10)return stop(0x272b3c);
        if(!allocateSourcePortal230370(i,pc))return stop(pc);
        sourcePortalCallerMask_[i]|=1;
        //The real loader is reached even when malloc returned NULL. The child
        //records the first parameter-vector read before its nullable destination store.
        if(!loadSourcePortal230450(i,pc))return stop(pc);
        sourcePortalCallerMask_[i]|=2;
        at(0x272b78);if(!sourceContainer4BC_)return stop(pc);
        if(!attachSourcePortal141960(i,*sourceContainer4BC_,pc))return stop(pc);
        sourcePortalCallerMask_[i]|=4;
        at(0x272b88);if(!sourcePortal3A0_||!nativePortalParameters3A0_||nativePortalParameters3A0_->sourceReleased||
            *sourcePortal3A0_!=nativePortalParameters3A0_->sourceArray.get())return stop(pc);
        at(0x272b94);if(!sourceVisibility4C0_)return stop(pc);
        sourcePortalLinkArrays_[i][0]=*sourceVisibility4C0_; //Actual field load precedes connect reads.
        at(0x272ba0);if(!*sourcePortal3A0_||!nativePortals_[i])return stop(pc);
        const auto first=(*sourcePortal3A0_)[i].connect40;
        at(0x272ba4);const auto second=(*sourcePortal3A0_)[i].connect44;
        auto& p=*nativePortals_[i];
        at(0x272ba8);p.source.connectA=static_cast<std::uint32_t>(first)-1u;
        at(0x272bac);p.source.connectB=static_cast<std::uint32_t>(second)-1u;
        sourcePortalCallerMask_[i]|=8;
        for(std::size_t stage=0;stage<4;++stage){
            const bool collision=stage&1u;
            const auto from=stage<2?*p.source.connectA:*p.source.connectB;
            const auto to=stage<2?*p.source.connectB:*p.source.connectA;
            constexpr std::uint32_t fieldPC[]{0x272b94,0x272bd0,0x272bf4,0x272c18};
            constexpr std::uint32_t fromPC[]{0x272bb8,0x272be8,0x272c0c,0x272c30};
            constexpr std::uint32_t toPC[]{0x272bc4,0x272be4,0x272c08,0x272c2c};
            constexpr std::uint32_t callPC[]{0x272bc8,0x272bec,0x272c10,0x272c34};
            if(stage){at(fieldPC[stage]);const auto& published=collision?sourceCollision4C4_:sourceVisibility4C0_;
                if(!published)return stop(pc);sourcePortalLinkArrays_[i][stage]=*published;}
            //Stage0 consumes the genuine earlier272B94 loaded array pointer.
            if(!sourcePortalLinkArrays_[i][stage])return stop(fromPC[stage]);
            const auto* slots=*sourcePortalLinkArrays_[i][stage];const auto produced=collision?sourceCollisionPublished_:sourceVisibilityPublished_;
            const auto readArgument=[&](unsigned which,std::uint32_t index,std::uint32_t where){
                at(where);if(!slots||index>=10||index>=produced)return false;
                sourcePortalLinkArguments_[i][stage][which]=slots[index];return true;
            };
            //The first call loads A then B; the following three load their
            //neighbor before their head node. These are actual source loads.
            if(stage==0){if(!readArgument(0,from,fromPC[stage])||!readArgument(1,to,toPC[stage]))return stop(pc);}
            else if(!readArgument(1,to,toPC[stage])||!readArgument(0,from,fromPC[stage]))return stop(pc);
            at(callPC[stage]);
            if(!linkSourcePortalWorld27F3B0(i,stage,*p.source.connectA,*p.source.connectB,pc))return stop(pc);
        }
        sourcePortalCallerMask_[i]|=16;
        at(0x272c40);
        //Actual same-App process slot, not the retained native pointer substituted
        //for the global. Lost/displaced custody is a native bound, never a fake NULL branch.
        if(!sourceLoadApplication_||sourceLoadApplication_->sourceLightManager426F60Owner_!=sourceTopologyLightManager_||
           !sourceTopologyLightManager_)return stop(pc);
        rr::platform::OriginalUiLightManagerOwner50KG::Diagnostic d;
        at(0x272c54);if(!sourceTopologyLightManager_->linkSourceRaPortal148D00(*this,i,p.entity,
            *sourceTopologyContainer_,*p.source.connectA,*p.source.connectB,d))return stop(d.sourcePC?d.sourcePC:pc);
        sourcePortalCallerMask_[i]|=32;
        at(0x272c60);++*sourcePortalGraphS0_;
    }
    at(0x272c78);if(!sourcePortal3A0_||!nativePortalParameters3A0_||
       *sourcePortal3A0_!=nativePortalParameters3A0_->sourceArray.get())return stop(pc);
    auto& role=*nativePortalParameters3A0_;
    if(*sourcePortal3A0_){at(0x272c84);role.sourceArray.reset();sourcePortalParameterFreeReached_=true;role.sourceReleased=true;}
    at(0x272c8c);sourcePortal3A0_=nullptr;sourcePortalParameterNullStored_=true;
    sourcePortalGraphPhase_=SourcePortalGraphPhase::returned;at(0x272c90);return true;
}
}
