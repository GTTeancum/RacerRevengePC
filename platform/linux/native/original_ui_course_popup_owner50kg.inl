// Private Course popup custody. The actual Page loader must issue the counted
// object at its real1D6670 interval before any load/resource method can use it.
    using CoursePopup=OriginalUiCoursePopupPayload50KG;
    struct CoursePopupHolder {
        C entity;
        CoursePopup payload;
        bool counted=false,source438Published=false,retiring=false;
    };
    std::unique_ptr<CoursePopupHolder> coursePopup_;
    bool coursePopupAllocationAttempted_=false,coursePopup438StoreReached_=false,coursePopupHidden_=false;
    std::shared_ptr<const assets::OriginalUiArchive50KG> coursePopupArchive()const noexcept {
        return loaderConfig_?loaderConfig_->archive_:nullptr;
    }
    bool coursePopupOccurrenceCurrent(const CoursePopup& payload,const C& c,bool retiring)const noexcept {
        if(thread_!=std::this_thread::get_id()||!issuer_||!construction_||!coursePopup_||
           &coursePopup_->payload!=&payload||&coursePopup_->entity!=&c||
           !construction_->pageAcquired_||!construction_->registrationCurrent()||
           pagePayload_.root_!=issuer_||!pagePayload_.pinned_||!pagePayload_.displayCurrent())return false;
        if(!coursePopup_->counted){
            // Only native bound admission before the actual constructor. No
            // material/clone proof accepts this phase as source construction.
            return !retiring&&!coursePopup_->source438Published&&!coursePopup_->retiring&&
                payload.phase_==CoursePopup::Phase::bound&&!c.reference00&&
                !construction_->coursePagePopup_;
        }
        return construction_->coursePagePopup_==&c&&c.reference00==1&&
            coursePopup_->retiring==retiring&&
            (!coursePopup_->source438Published||pagePayload_.shared_.popup438==&c);
    }
    bool coursePopupManagerCurrent(const C& manager)const noexcept {
        return thread_==std::this_thread::get_id()&&issuer_&&scope_&&construction_&&coursePopup_&&
            coursePopup_->counted&&!coursePopup_->retiring&& &manager==&scope_->manager_&&
            construction_->registrationCurrent();
    }
    bool coursePopupTextLine(std::int32_t widgetId,std::size_t lineOrdinal,CoursePopup::SourceLine& output)const noexcept {
        output={};
        if(thread_!=std::this_thread::get_id()||widgetId<68||widgetId>76||!issuer_||!construction_||
           !construction_->registrationCurrent()||courseWidgetTableNativeReleased_||!sourceWidgetTable154_)return false;
        const auto i=std::size_t(widgetId-1);
        const auto& row=courseTextRows_[i];
        if(!courseWidgetSlotsKnown_[i]||!row.loaded||!row.constructed||!row.outer||
           sourceWidgetTable154_[i]!=&row.outer->entity||construction_->courseOverlayOuters_[i]!=&row.outer->entity||
           !construction_->courseOverlayAttached_[i]||row.arrayReleased||!row.lineArray||
           row.outer->lines60!=row.lineArray.get()||row.outer->lineCount168<=0||
           std::size_t(row.outer->lineCount168)!=courseStaticTextSourceRow(i)->expectedLines||
           lineOrdinal>=std::size_t(row.outer->lineCount168)||lineOrdinal>=row.lines.size())return false;
        const auto& line=row.lines[lineOrdinal];
        if(!line||!line->constructed||!line->loaded||!line->attached||!row.slotKnown[lineOrdinal]||
           row.lineArray[lineOrdinal]!=&line->entity||row.sourceLineSlots_[lineOrdinal]!=&line->entity||
           construction_->courseTextLines_[i][lineOrdinal]!=&line->entity||
           !construction_->courseTextLineAttached_[i][lineOrdinal]||!line->screenText30_||!line->font34_||
           line->sourceFont34_!=line->font34_.get()||line->screenRetired||line->fontReleased||
           !ownsCourseScreenTextOccurrence(line->screenText30_.get(),i)||!line->screenText30_->coldInputsCurrent())return false;
        output.outer=row.outer.get();output.line=line.get();output.outerEntity=&row.outer->entity;
        output.lineEntity=&line->entity;output.screen=line->screenText30_.get();output.font=line->font34_;
        output.row=i;output.lineOrdinal=lineOrdinal;return true;
    }
    bool constructLoadCoursePopup1D6670(const std::array<std::int32_t,9>& ids,Diagnostic& d)noexcept {
        d.sourcePC=0x1D6670;
        if(thread_!=std::this_thread::get_id()||!issuer_||!scope_||!construction_||
           !coursePageBaseAttempted_||!coursePageConfig_||!coursePageConfig_->selectedCurrent("PageInfo")||
           !construction_->pageAcquired_||!construction_->registrationCurrent()||coursePopupAllocationAttempted_||
           pagePayload_.phase_!=OriginalUiCoursePagePayload50KG::Phase::derived||
           ids!=std::array<std::int32_t,9>{{68,69,70,71,72,73,74,75,76}})return false;
        coursePopupAllocationAttempted_=true;
        try {coursePopup_.reset(new CoursePopupHolder);}catch(...){
            // Original allocation NULL skips1FEC80, but still publishes NULL438
            // before the unconditional1FF020 call's first object store.
            pagePayload_.shared_.popup438=nullptr;coursePopup438StoreReached_=true;
            d.sourcePC=0x1FF05C;return false;
        }
        // Native resource-owner admission is not a source constructor return.
        // Preserve the actual allocation on refusal without publishing438 yet.
        if(!coursePopup_->payload.bindNative(*this,coursePopup_->entity,issuer_->raceDisplayOwner50LA_.lock(),d.popup))return false;
        d.entity=construction_->constructCoursePopup1FEC80(coursePopup_->entity);
        if(!d.entity.okay()){d.sourcePC=d.entity.sourcePC;return false;}
        coursePopup_->counted=true;
        if(!coursePopup_->payload.construct1FEC80AfterCount(d.popup)){d.sourcePC=d.popup.sourcePC;return false;}
        pagePayload_.shared_.popup438=&coursePopup_->entity; // Actual1D668C after constructor return.
        coursePopup438StoreReached_=true;coursePopup_->source438Published=true;
        if(!coursePopup_->payload.load1FF020(scope_->manager_,ids,d.popup)){d.sourcePC=d.popup.sourcePC;return false;}
        auto& popup=coursePopup_->entity;
        // Actual v18=141520 writes the two flags separately, then dispatches
        // v28=141460. This exact detached popup has no counted Entity children;
        // embedded Quads and cloned ScreenTexts are not that child forest.
        if(!coursePopup_->payload.current()||popup.firstChild18||popup.parent1C||popup.previous10||popup.next14){
            d.sourcePC=0x1D66AC;return false;
        }
        popup.flags29=static_cast<std::uint8_t>(popup.flags29&~1u); //141538.
        popup.flags29=static_cast<std::uint8_t>(popup.flags29&~2u); //14154C.
        coursePopupHidden_=true;d.sourcePC=0x1D66B4;return true;
    }
    bool abortNativeCoursePopupOwner()noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        if(!coursePopup_)return true;
        if(coursePopup_->payload.phase_==CoursePopup::Phase::empty){
            if(coursePopup_->counted||coursePopup_->source438Published)return false;
            coursePopup_.reset();return true;
        }
        CoursePopup::Diagnostic d;
        if(!coursePopup_->payload.preflightNativeAbort(d))return false;
        if(coursePopup_->counted)coursePopup_->retiring=true;
        if(!coursePopup_->payload.nativeAbortResources(d))return false;
        if(coursePopup_->counted){
            if(!construction_||!construction_->abortNativeCoursePopup(coursePopup_->entity).okay())return false;
            coursePopup_->counted=false;
        }
        // Keep reached Page438 as historical source identity, never dereference
        // it after this native owner withdrawal. No retail destructor receipt.
        coursePopup_.reset();return true;
    }
