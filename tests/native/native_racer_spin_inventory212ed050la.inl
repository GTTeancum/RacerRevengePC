// Included inside native_racer_model50LA. This is the genuine native
//212E10 ->212ED0 inventory, not a complete source210770/Config receipt.
bool Owner::continueNativeSpin212ED0(g::OriginalRacerEntityCold50LA& actualRacer,
        std::uint32_t& pc)noexcept {
    if(nativeSpinAttempted_){pc=nativeSpinPC_;return false;}
    if(pc!=0x212e10u||failurePC_!=pc||actualRacer.nativePostEmberPC_!=pc||
       modelRole_!=Rig::chariot||!nativeHandlingReturned_||
       handlingRoute_!=HandlingRoute::legacy||!bank_||!fixture_||!arena_||!rig_||slot_!=0u||
       !application_||!application_->application10()||!finishedFor(actualRacer)||
       !actualRacer.prepared()||actualRacer.nativeWreckWithdrawn_||
       !actualRacer.nativePostEmberAttempted_||!actualRacer.nativeDispatch20F390Known_||
       actualRacer.racer.sessionGeneration!=generation_||actualRacer.racer.reference00!=1u||
       actualRacer.racer.kind!=g::OriginalWreckComponentKind50LA::racer27E130||
       actualRacer.actualCounter423F60()!=&application_->objectCount423F60()||
       actualRacer.actualLock423F58()!=&application_->attachmentLock423F58()||
       application_->attachmentLock423F58()||actualRacer.sourceSpinCountB54_||
       !actualRacer.sourceSpinB58_||*actualRacer.sourceSpinB58_||actualRacer.spinAllocationB58_||
       actualRacer.nativeSpinB58Capacity_||actualRacer.nativeSpinB58PublishedRows_||
       nativeSpinAllocationAttempted_||nativeSpinCursor2C_)return false;
    nativeSpinAttempted_=true;
    const auto at=[&](std::uint32_t address)noexcept {
        pc=nativeSpinPC_=failurePC_=actualRacer.nativePostEmberPC_=address;
    };
    at(0x212ed0u);
    at(0x212edcu);actualRacer.sourceSpinCountB54_=0;
    at(0x212ee0u); //The first source model read follows the unconditional B54 store.
    const bool bottom=rig_->modelCount==5u;
    if((rig_->modelCount!=3u&&!bottom)||!actualRacer.leftModel1BC||!actualRacer.rightModel1B8||
       bool(actualRacer.bottomLeftModel1C4)!=bottom||bool(actualRacer.bottomRightModel1C0)!=bottom)
        return false;
    const auto current=graph();
    if(current.nodes!=arena_->nodes||arena_->sceneNodeCount!=rig_->nodeCount||
       !current.nodes||!g::validateSceneGraph49P(current))return false;
    const auto ownerCurrent=[&](const Owner* owner,unsigned role,
        const g::OriginalRacerModelSceneOwner50LA* model)noexcept {
        if(!owner||!model||owner->closed_||owner->modelRole_!=role||owner->rig_!=rig_||owner->bank_!=bank_||
           owner->fixture_!=fixture_||owner->arena_!=arena_||owner->slot_!=slot_||
           owner->generation_!=generation_||owner->application_!=application_||
           owner->domain_!=domain_||!owner->finishedFor(actualRacer)||
           owner->modelScene()!=model||model->released()||!model->model()||
           model->model()->sceneIndex!=rig_->roots[role]||!model->sourceRowCount()||
           !model->rowTable_||model->rowTableFreed_||model->referenceReleased_||
           model->fields().sourceRows1B8!=model->rowTable_.get()||
           model->model()->nodeCount1A0!=model->sourceRowCount())return false;
        //Use the published canonical source-row table, not retained authored
        //name/index caches. Every traversed object is checked again on access.
        for(std::uint32_t row=0;row<model->sourceRowCount();++row)
            if(model->fields().sourceRows1B8[row]!=model->sourceRow(row))return false;
        return true;
    };
    if(!ownerCurrent(this,Rig::chariot,actualRacer.chariotModel1B4)||
       !ownerCurrent(peers_[Rig::left],Rig::left,actualRacer.leftModel1BC)||
       !ownerCurrent(peers_[Rig::right],Rig::right,actualRacer.rightModel1B8)||
       (bottom&&(!ownerCurrent(peers_[Rig::bottomLeft],Rig::bottomLeft,actualRacer.bottomLeftModel1C4)||
                 !ownerCurrent(peers_[Rig::bottomRight],Rig::bottomRight,actualRacer.bottomRightModel1C0))))
        return false;
    const std::array<const g::OriginalRacerModelSceneOwner50LA*,5> models{{
        actualRacer.leftModel1BC,actualRacer.rightModel1B8,actualRacer.bottomLeftModel1C4,
        actualRacer.bottomRightModel1C0,actualRacer.chariotModel1B4}};
    const auto objectFor=[&](g::SceneNodeIndex49P index)noexcept
            ->const g::OriginalRacerSceneObject50LA* {
        const g::OriginalRacerSceneObject50LA* found=nullptr;
        const auto accept=[&](const g::OriginalRacerSceneObject50LA* candidate)noexcept {
            if(!candidate||candidate->sceneIndex!=index)return true;
            if(found&&found!=candidate)return false; //No ambiguous index-to-object alias.
            found=candidate;return true;
        };
        for(const auto* model:models)if(model){
            if(model->released()||!accept(model->model()))return nullptr;
            for(std::uint32_t row=0;row<model->sourceRowCount();++row)
                if(!accept(model->fields().sourceRows1B8[row]))return nullptr;
        }
        if(!found||!found->constructed||found->released||
           found->entity().sessionGeneration!=generation_||!found->entity().reference00||
           !found->entity().objectKey)return nullptr;
        return found;
    };
    //212FD0 inlines two recursion levels. These seven sites preserve the
    //actual name-call/count-store/link-load PCs, including a failed prefix.
    struct CountSite final {
        std::uint32_t name,store,sibling,child;
        unsigned siblingSite,childSite;
    };
    constexpr std::array<CountSite,7> countSites{{
        {0x213000u,0x213018u,0x21301cu,0x21310cu,1u,4u},
        {0x213030u,0x213048u,0x21304cu,0x2130acu,2u,3u},
        {0x213060u,0x213078u,0x21307cu,0x213094u,0u,0u},
        {0x2130c0u,0x2130d8u,0x2130dcu,0x2130f4u,0u,0u},
        {0x213120u,0x213138u,0x21313cu,0x21319cu,5u,6u},
        {0x213150u,0x213168u,0x21316cu,0x213184u,0u,0u},
        {0x2131b0u,0x2131c8u,0x2131ccu,0x2131e4u,0u,0u}
    }};
    const auto count=[&](auto&& self,g::SceneNodeIndex49P index,unsigned site,
            std::size_t& remaining)->bool {
        if(index==g::SceneNodeIndex49P::none)return true;
        const auto& source=countSites[site];at(source.name);
        const auto n=static_cast<std::uint32_t>(index);
        if(!remaining||n>=current.count)return false;--remaining;
        const auto* object=objectFor(index);if(!object)return false;
        //309BA8 searches the actual NULL-terminated name38, case-sensitive.
        //No uppercasing here and no immutable source-file-name substitution.
        if(std::strstr(object->name38.c_str(),"SPIN")){
            at(source.store);++*actualRacer.sourceSpinCountB54_;
        }
        at(source.sibling);
        if(!self(self,current.nodes[n].nextSibling188,source.siblingSite,remaining))return false;
        at(source.child);
        return self(self,current.nodes[n].firstChild184,source.childSite,remaining);
    };
    constexpr std::array<std::uint32_t,4> countCalls{{0x212eecu,0x212f04u,0x212f1cu,0x212f34u}};
    for(unsigned root=0;root<4u;++root)if(models[root]){
        at(countCalls[root]);std::size_t remaining=current.count;
        if(!count(count,models[root]->model()->sceneIndex,0u,remaining))return false;
        at(0x213218u); //212FD0 returned, including its delay slot.
    }
    at(0x212f3cu);
    if(*actualRacer.sourceSpinCountB54_>0){
        //Each validated graph has at most80 nodes. Four independent traversals
        //therefore cannot overflow signed B54 or the source count*4 byte size.
        at(0x212f48u);nativeSpinAllocationAttempted_=true;
        const auto size=static_cast<std::size_t>(*actualRacer.sourceSpinCountB54_);
        try {
            //Default-initialized raw pointer elements are deliberately unknown.
            //Native pointer width is used, never a fabricated32-bit guest address.
            actualRacer.spinAllocationB58_.reset(new const g::OriginalRacerSceneObject50LA*[size]);
        }catch(...){return false;} //Host failure stops BEFORE any212F50 NULL store.
        at(0x212f50u);actualRacer.sourceSpinB58_=actualRacer.spinAllocationB58_.get();
        actualRacer.nativeSpinB58Capacity_=size;
        at(0x212f54u);nativeSpinCursor2C_=0u;
        //213220 inlines one recursion level, with the same self/188/184 walk.
        struct FillSite final {
            std::uint32_t name,store,increment,sibling,child;
            unsigned siblingSite,childSite;
        };
        constexpr std::array<FillSite,3> fillSites{{
            {0x21324cu,0x21326cu,0x213278u,0x21327cu,0x2132f0u,1u,2u},
            {0x213290u,0x2132b0u,0x2132bcu,0x2132c0u,0x2132d8u,0u,0u},
            {0x213304u,0x213324u,0x213330u,0x213334u,0x21334cu,0u,0u}
        }};
        const auto fill=[&](auto&& self,g::SceneNodeIndex49P index,unsigned site,
                std::size_t& remaining)->bool {
            if(index==g::SceneNodeIndex49P::none)return true;
            const auto& source=fillSites[site];at(source.name);
            const auto n=static_cast<std::uint32_t>(index);
            if(!remaining||n>=current.count)return false;--remaining;
            const auto* object=objectFor(index);if(!object)return false;
            if(std::strstr(object->name38.c_str(),"SPIN")){
                at(source.store);
                if(*actualRacer.sourceSpinB58_!=actualRacer.spinAllocationB58_.get()||
                   *nativeSpinCursor2C_!=actualRacer.nativeSpinB58PublishedRows_||
                   *nativeSpinCursor2C_>=actualRacer.nativeSpinB58Capacity_)return false;
                actualRacer.spinAllocationB58_[*nativeSpinCursor2C_]=object;
                ++actualRacer.nativeSpinB58PublishedRows_; //Metadata only for this actual store.
                at(source.increment);++*nativeSpinCursor2C_;
            }
            at(source.sibling);
            if(!self(self,current.nodes[n].nextSibling188,source.siblingSite,remaining))return false;
            at(source.child);
            return self(self,current.nodes[n].firstChild184,source.childSite,remaining);
        };
        constexpr std::array<std::uint32_t,4> fillCalls{{0x212f68u,0x212f80u,0x212f98u,0x212fb0u}};
        for(unsigned root=0;root<4u;++root)if(models[root]){
            at(fillCalls[root]);std::size_t remaining=current.count;
            if(!fill(fill,models[root]->model()->sceneIndex,0u,remaining))return false;
            at(0x21337cu); //213220 returned, including its delay slot.
        }
        at(0x212fb8u);
        if(actualRacer.nativeSpinB58PublishedRows_!=size||*nativeSpinCursor2C_!=size)return false;
    }
    //Zero count takes212F40->212FB8 without allocating or touching B58/stack2C.
    //No replay, rollback, model retain, or source pointer clearing on failure.
    at(0x212fc4u);nativeSpinReturned_=true;
    at(0x212e18u);return true;
}
