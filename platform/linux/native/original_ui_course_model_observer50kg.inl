// Actual authenticated Course Soultree cohort per-row model19C -> Course manager2A4/List2A0 method1. No Garage grant.
    using CourseSearchList=OriginalUiSearchListCold50KG;
    using CourseObserverShared=game::OriginalGarageObserverShared50KG;
    struct CourseModelObserver {
        const void* next00=nullptr;const C* child04=nullptr;CourseSearchList* method08=nullptr;
        std::uint32_t field0C=0,field10=0,slot14=0;
        std::array<std::optional<std::uint32_t>,2> untouched18To1C;
        std::array<std::uint32_t,4> position20{0,0,0,0x3f800000u};
        game::OriginalRacerModelSceneOwner50LA* scene=nullptr;
        const C* entity=nullptr;std::uint32_t selectedSlot=0;
        bool sharedConstructed=false,published=false,selected=false,inserted=false,removed=false,complete=false;
    };
    // Stable native per-source-row custody. Empty native slots do not establish
    // source19C; only each real model ctor and28E8E8 publication do that.
    struct CourseObserverCustody {
        std::unique_ptr<CourseModelObserver> observer;
        game::OriginalRacerModelSceneOwner50LA* scene=nullptr;
        const void* retiredIdentity=nullptr;
        game::OriginalRacerModelSceneOwner50LA* retiredScene=nullptr;
        game::OriginalRacerModelSceneOwner50LA* receiptScene=nullptr;
        const C* receiptEntity=nullptr;
        std::uint64_t receiptGeneration=0,receiptKey=0;
        bool attempted=false,fieldPublished=false,retired=false;
    };
    std::array<CourseObserverCustody,84> courseObserverRows_;
    static bool courseObserverOrdinalAdmitted(std::size_t i)noexcept {
        return courseSoultreeOrdinalAdmitted(i); // Exact authored cohort, never a range/capacity grant.
    }
    bool preflightAllCourseModelObserversNative()const noexcept {
        // Read-only global gate before any row release. Actual removal remains
        // inside reverse per-model payload/children/observer/root retirement.
        for(std::size_t i=0;i<courseObserverRows_.size();++i){const auto& state=courseObserverRows_[i];
            if(!courseObserverOrdinalAdmitted(i)){
                if(state.observer||state.scene||state.retiredIdentity||state.retiredScene||state.receiptScene||
                   state.receiptEntity||state.receiptGeneration||state.receiptKey||state.attempted||state.fieldPublished||state.retired)return false;
                continue;
            }
            if(!preflightCourseModelObserverNative(i))return false;
        }
        return true;
    }
    bool courseModelObserverBounds28E230(std::size_t i,game::OriginalRacerModelSceneOwner50LA& scene,
        const game::SceneGraph49P& graph,game::CollectionBoundsResult48S& out,Diagnostic& d)noexcept {
        const auto fail=[&](std::uint32_t pc)noexcept {d.sourcePC=pc;return false;};
        if(!courseObserverOrdinalAdmitted(i)||!courseSoultreeRows_[i]||courseSoultreeRows_[i]->model.scene.get()!=&scene)
            return fail(0x28E230);
        const auto* current=scene.model();if(!current||!game::validateSceneGraph49P(graph))return fail(0x28E230);
        const auto index=static_cast<std::int32_t>(current->sceneIndex);
        if(index<0||std::uint32_t(index)>=graph.count)return fail(0x28E230);
        auto& model=scene.model_;auto& root=graph.nodes[index];
        // This actual cold two-row Course model has no admitted sphere flag
        // producer. If one appears, do not replace26F4B0 with box math.
        if(root.sceneFlags190&0x10u)return fail(0x26F4C0);
        const auto bounds=scene.publishAggregate28E110(graph);
        if(!bounds.okay())return fail(bounds.sourcePC);
        //28E388 independently requests the lazy world after bounds publication.
        if(!model.radius198||!game::refreshSceneWorld49P(graph,model.sceneIndex))return fail(0x28E398);
        game::CollectionBoundsInput48S input;input.shapeTag=1;input.shapeWorld=root.world100;
        std::memcpy(input.localCenter.v,model.bounds160.data(),sizeof input.localCenter.v);
        std::memcpy(input.localHalf.v,model.bounds170.data(),sizeof input.localHalf.v);
        input.cachedRadius=*model.radius198;
        if(game::prepareCollectionBounds48S(input,out)!=game::CollectionBoundsStatus48S::Complete)return fail(0x28E410);
        return true;
    }
    bool attachCourseModelObserver28E8A0(std::size_t i,Diagnostic& d)noexcept {
        d.sourcePC=0x1CD36C;
        if(!courseObserverOrdinalAdmitted(i)||courseWidgetOrdinal_!=i)return false;
        auto& state=courseObserverRows_[i];
        if(thread_!=std::this_thread::get_id()||terminal_||state.attempted||!construction_||
           !courseSoultreeRows_[i]||!courseSoultreeRows_[i]->model.scene||!construction_->registrationCurrent()||
           !construction_->searchCustodyCurrent()||construction_->searchOwner_!=this||
           construction_->searchAliases_!=2||!construction_->searchNodeAttached_||
           !preflightAllCourseModelObserversNative())return false;
        auto& m=courseSoultreeRows_[i]->model;auto& scene=*m.scene;const auto* model=scene.model();
        const auto* entity=scene.modelEntity8();
        if(!model||!entity||entity->reference00!=1||model->observer19C||
           construction_->courseSoultreeRows_[i].model!=&scene||construction_->courseSoultreeRows_[i].modelRetiring)return false;
        auto* shared=construction_->observerSharedForCourse(this);if(!shared)return false;
        state.attempted=true;state.scene=&scene;
        const auto fail=[&](std::uint32_t pc)noexcept {d.sourcePC=pc;return false;};
        state.observer.reset(new(std::nothrow) CourseModelObserver); // Actual malloc30 at28E8D0.
        if(!state.observer){
            scene.model_.observer19C=nullptr;state.fieldPublished=true; //28E8E8 NULL store.
            CourseObserverShared::assertion(0,false); //8C40/443400 first-use before allocation assertion.
            return fail(0x28E924);
        }
        auto& o=*state.observer;o.scene=&scene;o.entity=entity;
        if(!shared->construct280890(&o))return fail(0x280890);
        o.sharedConstructed=true;scene.model_.observer19C=&o;o.published=true;state.fieldPublished=true;
        if(!CourseObserverShared::assertion(0,scene.model_.observer19C!=nullptr))return fail(0x28E924);
        game::CollectionBoundsResult48S bounds;
        if(!courseModelObserverBounds28E230(i,scene,m.graph(),bounds,d))return false;
        //2809C0 process assertions bracket real27F590 and List selection.
        if(!CourseObserverShared::assertion(1,o.child04==nullptr))return fail(0x280A40);
        auto* list=construction_->searchNode34_; // Actual27F590(method1); same shared2A0.
        if(!CourseObserverShared::assertion(2,list!=nullptr))return fail(0x280AB8);
        if(list!=construction_->searchList_.get()||!list->courseHistoryCurrent(this,issuer_,scope_->application_))
            return fail(0x27F590);
        CourseSearchList::D ld;
        if(!list->selectCourseModelSlot(this,issuer_,scope_->application_,i,*entity,o.selectedSlot,ld))return fail(ld.sourcePC);
        o.selected=true;
        if(!CourseObserverShared::assertion(3,o.selectedSlot!=0))return fail(0x280B40);
        if(!list->insertCourseModelSelected(this,issuer_,scope_->application_,i,*entity,o.selectedSlot,ld))return fail(ld.sourcePC);
        o.inserted=true;o.selected=false;
        o.child04=&construction_->searchNode_;o.method08=list;o.slot14=o.selectedSlot;o.next00=nullptr;
        for(std::size_t lane=0;lane<4;++lane)o.position20[lane]=std::bit_cast<std::uint32_t>(bounds.center.v[lane]);
        //280BF0 sees this actual Node30NULL and returns without secondary links.
        if(construction_->searchNode30_)return fail(0x280C34);
        o.complete=true;
        if(!CourseObserverShared::assertion(4,o.child04!=nullptr))return fail(0x28E9CC);
        d.sourcePC=0x1CD374;return true;
    }
    bool preflightCourseModelObserverNative(std::size_t i)const noexcept {
        if(!courseObserverOrdinalAdmitted(i))return false;const auto& state=courseObserverRows_[i];
        if(!state.observer){
            if(construction_&&construction_->searchList_){const auto& list=*construction_->searchList_;
                if(list.courseMembers_[i]||list.courseSlots_[i]||list.coursePendingOrdinal_==i)return false;
            }
            // Completed rows may already have freed native model backing.
            // Retired identities are comparison-only and never dereferenced.
            if(!courseSoultreeRows_[i]||!courseSoultreeRows_[i]->model.scene||!courseSoultreeRows_[i]->model.scene->model()){
                if(!state.retired)return !state.attempted&&!state.fieldPublished&&!state.scene&&!state.retiredIdentity&&
                    !state.retiredScene&&!state.receiptScene&&!state.receiptEntity&&!state.receiptGeneration&&!state.receiptKey;
                return (!state.scene||state.retiredScene==state.scene)&&
                    (!state.receiptScene||state.retiredScene==state.receiptScene)&&
                    (!state.retiredIdentity||(state.attempted&&state.fieldPublished&&state.retiredScene));
            }
            const auto& scene=*courseSoultreeRows_[i]->model.scene;const auto* identity=scene.model()->observer19C;
            return state.retired?courseModelObserverRetired(i,scene,identity):identity==nullptr;
        }
        if(!construction_||!construction_->registrationCurrent()||construction_->searchOwner_!=this||
           !courseSoultreeRows_[i]||!courseSoultreeRows_[i]->model.scene)return false;
        const auto& o=*state.observer;auto& scene=*courseSoultreeRows_[i]->model.scene;
        if(construction_->courseSoultreeRows_[i].model!=&scene)return false;
        auto* shared=construction_->observerSharedForCourse(this);
        const bool liveReference=o.entity&&o.entity->reference00==1&&!scene.referenceReleased_;
        const bool retiringReference=o.entity&&o.entity->reference00==0&&
            state.receiptScene==&scene&&state.receiptEntity==o.entity&&
            state.receiptGeneration==o.entity->sessionGeneration&&state.receiptKey==o.entity->objectKey&&
            construction_->courseSoultreeRows_[i].model==&scene&&construction_->courseSoultreeRows_[i].modelRetiring&&
            scene.referenceReleased_&&!scene.released_;
        // Read-only custody survives payload/child retirement refusals after
        //142590. Actual observer removal below separately gates children/table.
        if(o.scene!=&scene||o.entity!=scene.modelEntity8()||(!liveReference&&!retiringReference)||
           !shared||(o.sharedConstructed&&(!shared->owns(&o)||shared->ownerHasLinks(&o)))||o.next00||o.field0C||o.field10||
           (o.published?scene.model_.observer19C!=&o:scene.model_.observer19C!=nullptr))return false;
        const auto* list=construction_->searchList_.get();if(!list)return false;
        if(o.inserted)return o.child04==&construction_->searchNode_&&o.method08==list&&
            list->courseModelCurrent(this,issuer_,scope_->application_,i,*o.entity,o.slot14);
        if(o.removed)return o.child04==&construction_->searchNode_&&o.method08==list&&o.slot14&&
            !list->courseMembers_[i]&&!list->courseSlots_[i]&&list->courseStorageCurrent();
        return !o.child04&&!o.method08&&!o.slot14&&
            (!o.selected||(list->coursePendingOrdinal_==i&&list->coursePendingSlot_==o.selectedSlot));
    }
    bool preflightCourseModelObserverRetirement(std::size_t i,game::OriginalRacerModelSceneOwner50LA& scene)noexcept {
        if(!courseObserverOrdinalAdmitted(i))return false;auto& state=courseObserverRows_[i];
        // Acquire only while the actual registered model still owns reference1.
        // This receipt does not release it or allow another ref0 object through.
        if(thread_!=std::this_thread::get_id()||!construction_||!courseSoultreeRows_[i]||
           courseSoultreeRows_[i]->model.scene.get()!=&scene||construction_->courseSoultreeRows_[i].model!=&scene||
           construction_->courseSoultreeRows_[i].modelRetiring||scene.referenceReleased_||scene.released_||
           !scene.modelEntity8()||scene.modelEntity8()->reference00!=1||
           !preflightCourseModelObserverNative(i))return false;
        const auto* identity=scene.model()->observer19C;
        if(!courseModelObserverCustodyCurrent(i,scene,identity))return false;
        if(state.receiptScene&&state.receiptScene!=&scene)return false;
        state.receiptScene=&scene;state.receiptEntity=scene.modelEntity8();
        state.receiptGeneration=scene.modelEntity8()->sessionGeneration;
        state.receiptKey=scene.modelEntity8()->objectKey;return true;
    }
    bool abortCourseModelObserverNative(std::size_t i)noexcept {
        if(!courseObserverOrdinalAdmitted(i))return false;auto& state=courseObserverRows_[i];
        if(thread_!=std::this_thread::get_id()||!preflightCourseModelObserverNative(i))return false;
        if(!state.observer){state.retiredScene=state.scene?state.scene:state.receiptScene;
            state.retired=true;return true;}
        auto& o=*state.observer;
        if(!o.scene||state.receiptScene!=o.scene||!construction_->courseSoultreeRows_[i].modelRetiring||
           construction_->courseSoultreeRows_[i].model!=o.scene||!o.scene->referenceReleased_||
           !o.scene->garageSceneChildrenRetired_||!o.scene->rowTableFreed_||o.scene->rowTable_||
           o.entity->reference00||o.scene->released_)return false;
        auto* shared=construction_->observerSharedForCourse(this);
        auto* list=construction_->searchList_.get();
        if(o.inserted){
            if(!CourseObserverShared::assertion(5,o.method08!=nullptr))return false;
            CourseSearchList::D ld;
            if(!list->removeCourseModel(this,issuer_,scope_->application_,i,*o.entity,o.slot14,ld))return false;
            o.inserted=false;o.removed=true; // Native release ledger; source04/08/14 are not zeroed.
        }else if(o.selected){
            if(!list->cancelCourseSelectionNative(i,o.selectedSlot))return false;o.selected=false;
        }
        if(o.sharedConstructed){if(!shared->release280920(&o))return false;o.sharedConstructed=false;}
        state.retiredIdentity=o.published?&o:nullptr;state.retiredScene=o.scene;
        state.observer.reset();state.retired=true;
        return true; // Historical model19C deliberately retained, never a source clear.
    }
    bool courseModelObserverCustodyCurrent(std::size_t i,const game::OriginalRacerModelSceneOwner50LA& scene,const void* identity)const noexcept {
        if(!courseObserverOrdinalAdmitted(i))return false;const auto& state=courseObserverRows_[i];
        if(!courseSoultreeRows_[i]||courseSoultreeRows_[i]->model.scene.get()!=&scene||!scene.model()||scene.model()->observer19C!=identity)return false;
        if(state.retired)return courseModelObserverRetired(i,scene,identity);
        if(!state.observer)return identity==nullptr;
        return state.observer->scene==&scene&&
            (state.observer->published?identity==state.observer.get():identity==nullptr)&&
            preflightCourseModelObserverNative(i);
    }
    bool courseModelObserverRetired(std::size_t i,const game::OriginalRacerModelSceneOwner50LA& scene,const void* identity)const noexcept {
        if(!courseObserverOrdinalAdmitted(i)||!courseSoultreeRows_[i]||courseSoultreeRows_[i]->model.scene.get()!=&scene)return false;
        const auto& state=courseObserverRows_[i];
        return state.retired&&!state.observer&&state.retiredScene==&scene&&
            state.retiredIdentity==identity&&scene.model()&&scene.model()->observer19C==identity;
    }
