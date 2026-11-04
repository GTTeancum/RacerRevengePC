//Private member definition included inside ra_cold_aggregate50LA::Owner.
//No source occurrence is issued merely by defining this adapter. The bounded
//Model continuation supplies a synchronous issue(method,portal) only after its
//new bounds28E230 receipt is returned. Existing COL callbacks stay separate.
    template<class PhysicsState,class Issue>
    g::OriginalSourcePhysicsModel50LA::Result withSourcePhysicsModelPrimary2809C0(
        PhysicsState& h,Issue&& issue)noexcept {
        using Model=g::OriginalSourcePhysicsModel50LA;
        auto* const target=h.nestedModel.get();auto& config=h.configure166400;
        if(!sourceStandardWrapperA0_||&sourceStandardWrapperA0_->sourcePhysics!=&h||
           !config.directMembershipReturned||!config.resultMembershipReturned||!config.modelObserverEntered||
           !target||target->nativeWithdrawn_||!target->model_||!h.bodyOwner||!h.bodyOwner->object_||
           target->base_!=h.bodyOwner.get()||!target->preflightNative())return {Model::Status::owner,0x2809c0u};
        const auto& registration=target->modelObserver_;
        if(!registration.entered||registration.returned||registration.nativeWithdrawn||!registration.boundsReturned||
           !registration.constructorReturned||!registration.pointerStored||!registration.published||
           registration.updateOccurrence!=&config||registration.ownerIdentity!=&target->model_->model_||
           h.bodyOwner->object_->model14C!=registration.ownerIdentity||registration.mode!=2u||
           !h.bodyOwner->object_->visibilityB4||*h.bodyOwner->object_->visibilityB4!=registration.zone||
           target->model_->model_.observer19C!=registration.published)return {Model::Status::owner,0x2809c0u};
        const auto modelMethod=[&,target](const void* actualZone,const void* registrationOccurrence,
            const void* actualModel,NativeListMethodCustody*& method,std::optional<std::uint32_t>& slot,
            bool& selectionPending,std::uint32_t& methodPC,bool select)noexcept {
            //Native same-world custody proof for the actual27B45C ->165EC8
            //visibilityB4 argument. Never resolve it through collision4C4/B8.
            methodPC=select?0x280afcu:0x27f5b4u;
            if(!h.visibilityRead||!h.visibilityB4Stored||!h.visibilityZone||*h.visibilityZone>=10||
               !h.bodyOwner||!h.bodyOwner->object_||!h.bodyOwner->object_->visibilityB4||
               actualZone!=*h.bodyOwner->object_->visibilityB4||actualZone!=h.visibilityNode||
               !target||actualModel!=static_cast<const void*>(&target->model_->model_)||
               registrationOccurrence!=&target->modelObserver_||
               !sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceVisibility4C0_||
               *sourceVisibility4C0_!=sourceVisibilitySlots4C0_.get()||!sourceVisibilitySlots4C0_||
               *h.visibilityZone>=sourceVisibilityPublished_)return false;
            const auto zone=*h.visibilityZone;auto& node=sourceSearchNodes_[0][zone];
            if(node.withdrawn||!node.published||!node.owner||!node.ordinalKnown||
               sourceVisibilitySlots4C0_[zone]!=node.owner.get()||actualZone!=node.owner.get()||
               !preflightSourceSearchNodeNative(node,zone,false))return false;
            //Mode2 follows27F5B4, selecting visibility dynamic method kind2
            //(World4D0), NOT visibility static kind0/World4C8.
            if(!select)method=node.owner->nativeList38;
            if(method!=node.owner->nativeList38||!method||!nativeMethods4D0_||
               nativeMethods4D0_[zone]!=method||!sourceListMethodArrayCurrent(2,zone,method))return false;
            const auto& publication=sourceListMethods_[2][zone];
            if(!publication.arrayPublished||!publication.aliasPublished||!publication.aliasRetained||
               publication.withdrawn||publication.aliasNode!=node.owner.get()||
               !method->sourceMode_||method->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
               method->application_!=worldApplication_.get()||method->ordinal_!=20u+zone||
               method->usage10_!=2||method->reference00_!=2||
               !method->source_.constructorReturned||method->source_.vtable08!=0x3d2d30u||
               !sourceListEntriesCurrent(*method))return false;
            if(!select)return true;
            //Actual3D2D30+20=27E3B0 tailcalls27E3C0 with a3(NULL)
            //as requested slot. +24=27E570 is owned by Model insertion.
            return selectSourceObserverSlot27E3C0(*method,registrationOccurrence,actualModel,
                slot,selectionPending,methodPC,2u);
        };

        const auto modelPortal=[&,target](unsigned phase,const void* registration,const void* actualModel,
            const void* actualZone,const std::array<std::uint32_t,4>& center,const void* cursor,
            auto& edge,std::shared_ptr<const void>& graphPin,std::uint32_t& portalPC)noexcept {
            portalPC=phase==0?0x280c34u:phase==2?0x280c90u:phase==4?0x280cf8u:0x280c40u;
            if(phase>4||!target||registration!=&target->modelObserver_||
               actualModel!=static_cast<const void*>(&target->model_->model_)||!h.visibilityRead||!h.visibilityB4Stored||
               !h.visibilityZone||*h.visibilityZone>=10||!h.bodyOwner||!h.bodyOwner->object_||
               !h.bodyOwner->object_->visibilityB4||actualZone!=*h.bodyOwner->object_->visibilityB4||
               actualZone!=h.visibilityNode||center!=target->modelObserver_.center||
               !sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceVisibility4C0_||
               *sourceVisibility4C0_!=sourceVisibilitySlots4C0_.get()||!sourceVisibilitySlots4C0_||
               *h.visibilityZone>=sourceVisibilityPublished_)return false;
            const auto zone=*h.visibilityZone;auto& root=sourceSearchNodes_[0][zone];
            if(root.withdrawn||!root.published||!root.owner||actualZone!=root.owner.get()||
               sourceVisibilitySlots4C0_[zone]!=root.owner.get()||
               !preflightSourceSearchNodeNative(root,zone,false)||
               !sourcePortalWorldLinksCurrent(*root.owner,false,zone))return false;
            if(phase==0){
                if(graphPin||cursor)return false;
                for(const auto& old:sourcePhysicsModelPortalBorrows_)
                    if(old.registration==registration&&!old.token.expired())return false;
                //Allocate native custody before exposing the source head pointer.
                //Weak root records never keep a completed Model borrower alive.
                try{std::shared_ptr<const void> token=std::make_shared<std::uint8_t>(0);
                    sourcePhysicsModelPortalBorrows_.push_back({registration,token});graphPin=std::move(token);
                }catch(...){return false;}
                edge.next=root.owner->portalLinks30.get();return true;
            }
            bool pinned=false;
            for(const auto& borrow:sourcePhysicsModelPortalBorrows_)if(borrow.registration==registration){
                const auto live=borrow.token.lock();if(live&&live==graphPin){pinned=true;break;}}
            if(!pinned||!graphPin||!cursor)return false;
            std::size_t index=0;while(index<40&&sourcePortalWorldLinks_[index/4][index%4].published!=cursor)++index;
            //Even links are visibility namespace (stages0/2), never collision1/3.
            if(index==40||(index&1u))return false;
            auto& link=sourcePortalWorldLinks_[index/4][index%4];
            if(link.withdrawn||link.knownMask!=31||!link.published||link.from!=root.owner.get()||!link.to)return false;
            auto& raw=*link.published;const auto& portal=nativePortals_[index/4];
            if(!portal||!portal->sourceMode||raw.neighbor04!=&link.to->entity||
               raw.portal08!=&portal->entity||!portal->source.loadReturned||
               portal->source.normalKnown!=7||!portal->source.normalNormalized||portal->source.copiedVertices!=4)return false;
            for(unsigned i=0;i<4;++i)if(portal->source.edgeKnown[i]!=7||!portal->source.edgeNormalized[i])return false;
            if(phase==1){
                edge.entered=true;edge.edge=cursor;edge.edgeIndex=index;
                edge.neighbor=raw.neighbor04;edge.portal=raw.portal08;
                //Actual231150 uses center and f12=+0, not half/radius80.
                portalPC=0x231150u;
                const bool overlapOkay=g::source_portal_overlap231150::run(portal->source.vertices30,portal->source.normal70,
                    portal->source.edges80,center,0u,edge.overlap);
                edge.overlapReturned=edge.overlap.returned;edge.hit=edge.overlap.hit;edge.distance=edge.overlap.outputDistance;
                if(!overlapOkay){portalPC=edge.overlap.pc;return false;}
                portalPC=0x280c54u;if(!edge.hit)return true;
                if(!edge.distance)return false;
                portalPC=0x280c60u;link.storeDistance10Native(*edge.distance);edge.distanceStoreMask|=1u;
                portalPC=0x280c64u;link.storeDistance0CNative(*edge.distance);edge.distanceStoreMask|=2u;
            }else if(edge.edge!=cursor||edge.edgeIndex!=index||edge.neighbor!=raw.neighbor04||edge.portal!=raw.portal08||
                     !g::source_portal_overlap231150::historyCurrent(edge.overlap))return false;
            if(phase==4){edge.next=raw.next00.get();portalPC=0x280cf8u;return true;}
            if(phase==3&&!edge.hit)return true;
            //The source keeps a method cursor: each hit reads its usage10,
            //then resolves the neighbor method using that same mode2.
            portalPC=phase==1?0x280c6cu:portalPC;
            const auto* prior=edge.previousMethod;
            if(!prior||!prior->sourceMode_||prior->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
               prior->application_!=worldApplication_.get()||prior->usage10_!=2||prior->ordinal_<20||prior->ordinal_>=30||
               !sourceListMethodArrayCurrent(2,prior->ordinal_-20,prior))return false;
            std::size_t neighborZone=0;while(neighborZone<10&&sourceVisibilitySlots4C0_[neighborZone]!=link.to)++neighborZone;
            if(neighborZone==10||neighborZone>=sourceVisibilityPublished_)return false;
            auto& neighbor=sourceSearchNodes_[0][neighborZone];
            if(neighbor.withdrawn||!neighbor.published||neighbor.owner.get()!=link.to||
               !preflightSourceSearchNodeNative(neighbor,neighborZone,false))return false;
            if(phase==1){portalPC=0x27f5b4u;edge.method=link.to->nativeList38;edge.methodReturned=true;}
            auto* method=edge.method;
            if(!method||method!=link.to->nativeList38||!nativeMethods4D0_||nativeMethods4D0_[neighborZone]!=method||
               !sourceListMethodArrayCurrent(2,neighborZone,method)||method->usage10_!=2||method->reference00_!=2||
               !method->sourceMode_||method->world_!=static_cast<const ra_pod_scene50LA::Owner*>(this)||
               method->application_!=worldApplication_.get()||method->ordinal_!=20u+neighborZone||
               !method->source_.constructorReturned||method->source_.vtable08!=0x3d2d30u||
               !sourceListEntriesCurrent(*method))return false;
            const auto& publication=sourceListMethods_[2][neighborZone];
            if(!publication.arrayPublished||!publication.aliasPublished||!publication.aliasRetained||
               publication.withdrawn||publication.aliasNode!=link.to)return false;
            if(phase==2){portalPC=0x280c90u;
                if(!edge.overlap.returned||!edge.overlap.hit||edge.distanceStoreMask!=3u||!edge.methodReturned)return false;
                return selectSourceObserverSlot27E3C0(*method,&edge,actualModel,
                    edge.slot,edge.selectionPending,portalPC,2u);}
            return phase==1||phase==3;
        };

        return issue(modelMethod,modelPortal); //Synchronous only; no callback is retained.
    }
