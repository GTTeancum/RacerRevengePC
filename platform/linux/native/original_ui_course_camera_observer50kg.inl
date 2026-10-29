// Private exact Page camera114770 -> SAME Course node2A4/List2A0 method2.
// No Garage construction grant, model ordinal, second List or Entity attach.
    struct CourseCameraObserver {
        const void* next00=nullptr;const C* child04=nullptr;CourseSearchList* method08=nullptr;
        std::uint32_t zero0C=0,zero10=0,slot14=0;
        std::array<std::optional<std::uint32_t>,2> untouched18To1C;
        std::array<std::uint32_t,4> position20{0,0,0,0x3f800000u};
        std::uint32_t selectedSlot=0;
        bool sharedConstructed=false,published=false,parentPublished=false;
        bool selected=false,inserted=false,removed=false,complete=false;
    };
    std::unique_ptr<CourseCameraObserver> courseCameraObserver_;
    bool courseCameraObserverAttempted_=false,courseCameraObserver34Published_=false,courseCameraObserverRetired_=false;
    bool courseCameraObserverParent38Published_=false;
    const void* courseCameraObserverRetiredIdentity_=nullptr;
    bool courseCameraObserverOwnerCurrent(const CourseCamera& camera)const noexcept {
        return thread_==std::this_thread::get_id()&&issuer_&&scope_&&construction_&&coursePageCamera_.get()==&camera&&
            camera.issuer_==issuer_&&camera.entities_==scope_->application_&&camera.onAffinity()&&camera.actualEntitiesAlive()&&
            camera.entityConstructed_&&camera.fieldsComplete_&&camera.entity_.reference00==1&&
            construction_->registrationCurrent()&&construction_->searchCustodyCurrent()&&
            construction_->searchOwner_==this&&construction_->searchAliases_==2&&construction_->searchNodeAttached_&&
            construction_->sourceNode2A4_==&construction_->searchNode_;
    }
    bool bindCourseCameraObserver114770(CourseCamera& camera,std::uint32_t& pc)noexcept {
        pc=0x114770;
        if(!courseCameraObserverOwnerCurrent(camera)||courseCameraObserverAttempted_||courseCameraObserver_||
           camera.phase_!=CourseCamera::Phase::viewportInitialized2361C0||camera.fields_.registration34||camera.fields_.parent38)return false;
        auto* shared=construction_->observerSharedForCourse(this);if(!shared)return false;
        courseCameraObserverAttempted_=true;
        courseCameraObserver_.reset(new(std::nothrow) CourseCameraObserver); //Actual malloc30 at114790.
        if(!courseCameraObserver_){
            camera.fields_.registration34=nullptr;courseCameraObserver34Published_=true; //1147A8 NULL publication.
            game::OriginalCameraObserverAssertions50KG::allocation1147E4(false);pc=0x1147E4;return false;
        }
        auto& o=*courseCameraObserver_;
        if(!shared->construct280890(&o)){pc=0x280890;return false;}
        o.sharedConstructed=true;camera.fields_.registration34=&o;o.published=true;courseCameraObserver34Published_=true;
        if(!game::OriginalCameraObserverAssertions50KG::allocation1147E4(camera.fields_.registration34!=nullptr)){pc=0x1147E4;return false;}
        camera.fields_.parent38=&construction_->searchNode_;o.parentPublished=true;courseCameraObserverParent38Published_=true; //11481C, BEFORE2809C0.
        if(!CourseObserverShared::assertion(1,o.child04==nullptr)){pc=0x280A40;return false;}
        auto* list=construction_->searchNode38_; //Actual27F590(a2=2); same2A0 alias.
        if(!CourseObserverShared::assertion(2,list!=nullptr)){pc=0x280AB8;return false;}
        if(list!=construction_->searchList_.get()||!list->courseHistoryCurrent(this,issuer_,scope_->application_)){
            pc=0x27F590;return false;
        }
        CourseSearchList::D ld;
        if(!list->selectCourseCameraSlot(this,issuer_,scope_->application_,camera.entity_,o.selectedSlot,ld)){pc=ld.sourcePC;return false;}
        o.selected=true;
        if(!CourseObserverShared::assertion(3,o.selectedSlot!=0)){pc=0x280B40;return false;}
        if(!list->insertCourseCameraSelected(this,issuer_,scope_->application_,camera.entity_,o.selectedSlot,ld)){pc=ld.sourcePC;return false;}
        o.inserted=true;o.selected=false;
        o.child04=&construction_->searchNode_;o.method08=list;o.slot14=o.selectedSlot;o.next00=nullptr;
        o.position20=camera.fields_.position180;
        if(construction_->searchNode30_){pc=0x280C34;return false;} //No unsupported portal secondary-link producer.
        o.complete=true;
        if(!game::OriginalCameraObserverAssertions50KG::membership114884(o.child04!=nullptr)){pc=0x114884;return false;}
        pc=0x1148B4;return true;
    }
    bool preflightCourseCameraObserverNative(const CourseCamera& camera)const noexcept {
        if(!courseCameraObserverOwnerCurrent(camera))return false;
        const auto* list=construction_->searchList_.get();
        if(!list||!list->courseHistoryCurrent(this,issuer_,scope_->application_))return false;
        if(!courseCameraObserver_){
            if(list->courseCameraMember_||list->courseCameraSlot_||list->courseCameraPendingMember_||list->courseCameraPendingSlot_)return false;
            if(camera.fields_.registration34)return false;
            if(courseCameraObserverRetired_)return courseCameraObserverAttempted_&&
                (courseCameraObserverParent38Published_?camera.fields_.parent38==&construction_->searchNode_:camera.fields_.parent38==nullptr);
            return !camera.fields_.parent38&&!courseCameraObserverRetiredIdentity_;
        }
        const auto& o=*courseCameraObserver_;auto* shared=construction_->observerSharedForCourse(this);
        if(!courseCameraObserverAttempted_||courseCameraObserverRetired_||!shared||
           (o.sharedConstructed?(!shared->owns(&o)||shared->ownerHasLinks(&o)):shared->owns(&o))||
           o.next00||o.zero0C||o.zero10||o.parentPublished!=courseCameraObserverParent38Published_||
           o.published!=courseCameraObserver34Published_||(o.published&&!o.sharedConstructed)||
           (o.published?camera.fields_.registration34!=&o:camera.fields_.registration34!=nullptr)||
           (o.parentPublished?camera.fields_.parent38!=&construction_->searchNode_:camera.fields_.parent38!=nullptr))return false;
        if(o.inserted)return !o.removed&&o.child04==&construction_->searchNode_&&o.method08==list&&
            list->courseCameraCurrent(this,issuer_,scope_->application_,camera.entity_,o.slot14);
        if(o.removed)return o.child04==&construction_->searchNode_&&o.method08==list&&o.slot14&&
            !list->courseCameraMember_&&!list->courseCameraSlot_&&!list->courseCameraPendingMember_&&!list->courseCameraPendingSlot_;
        return !o.child04&&!o.method08&&!o.slot14&&!list->courseCameraMember_&&!list->courseCameraSlot_&&
            (o.selected?(list->courseCameraPendingMember_==&camera.entity_&&list->courseCameraPendingSlot_==o.selectedSlot):
                (!list->courseCameraPendingMember_&&!list->courseCameraPendingSlot_));
    }
    bool abortCourseCameraObserverNative(CourseCamera& camera,std::uint32_t& pc)noexcept {
        pc=0;
        if(!preflightCourseCameraObserverNative(camera))return false;
        if(!courseCameraObserver_){courseCameraObserverRetired_=courseCameraObserverAttempted_;return true;}
        auto& o=*courseCameraObserver_;auto* shared=construction_->observerSharedForCourse(this);
        auto& list=*construction_->searchList_;
        if(o.inserted){
            if(!CourseObserverShared::assertion(5,o.method08!=nullptr)){pc=0x2814E4;return false;}
            CourseSearchList::D ld;
            if(!list.removeCourseCamera(this,issuer_,scope_->application_,camera.entity_,o.slot14,ld)){pc=ld.sourcePC;return false;}
            o.inserted=false;o.removed=true;
        }
        if(o.selected){if(!list.cancelCourseCameraSelectionNative(camera.entity_,o.selectedSlot))return false;o.selected=false;}
        if(o.sharedConstructed){if(!shared->release280920(&o))return false;o.sharedConstructed=false;}
        // Source11490C clears34 only after actual280920 has returned. Unentered
        // constructor failure has no published field to clear. parent38 remains
        // its genuine reached historical pointer, never reset for cold admission.
        if(o.published)camera.fields_.registration34=nullptr;
        courseCameraObserverRetiredIdentity_=&o;courseCameraObserver_.reset();courseCameraObserverRetired_=true;
        return true; //Native cleanup receipt; no source Camera destructor return.
    }
