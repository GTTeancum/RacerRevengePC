// Private actual vAC2718C0: missing PhysicsObjects still leaves the last
// Trigger2Action1 section selected. The following integer getter searches it.
    bool sourcePhysicsObjectsAttempted_=false,sourcePhysicsObjectsMissing_=false;
    bool sourcePhysicsObjectCountAttempted_=false,sourcePhysicsObjectCountReturned_=false;
    bool sourcePhysicsObjectsMissingCurrent()const noexcept {
        return sourcePhysicsObjectsAttempted_&&sourcePhysicsObjectsMissing_&&sourceObjectDirectoryCurrent()&&
            sourceTriggerNextSelection_==5&&sourceTriggerSelected_&&*sourceTriggerSelected_==4&&
            sectionAttempts_==sourceTriggerBaseAttempt_+6&&sectionReturned_&&
            source_->selected20==2&&source_->field4C8==478&&source_->block4C4==block_.get()&&
            block_&&sourceSectionPayloadCurrent(97)&&std::strcmp(source_->name28.data(),"Trigger2Action1")==0;
    }
    bool selectSourcePhysicsObjects157D00(bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(sourcePhysicsObjectsAttempted_||sourceTriggerNextSelection_!=5||!sourceTriggerSelected_||
           *sourceTriggerSelected_!=4||!sourceTriggerConfigCurrent())return false;
        sourcePhysicsObjectsAttempted_=true;++sectionAttempts_;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=713)return false;
            const auto* candidate=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(candidate,0,1024))return false;
            result=std::strcmp("physicsobjects",candidate);return true;
        };
        std::uint32_t low=0,high=712,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x157BE8;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}
            else {sourcePC_=0x157C1C;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(found){sourcePC_=0x157E90;return false;} // Positive section is not this bounded cohort.
        sourcePhysicsObjectsMissing_=true;sourcePC_=0x157DD8;return true;
    }
    bool readSourcePhysicsObjectCount158B30(std::optional<std::uint32_t>& actualStack14C,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x158B30;
        if(sourcePhysicsObjectCountAttempted_||actualStack14C||!sourcePhysicsObjectsMissingCurrent())return false;
        sourcePhysicsObjectCountAttempted_=true;
        // 158B48 -> 158920 -> 158934/158240, over actual retained two-field cache.
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=source_->selected20)return false;
            const auto* row=block_.get()+std::size_t(i)*43;
            if(!std::memchr(row,0,35))return false;
            result=std::strcmp("numberofphysicsobjects",reinterpret_cast<const char*>(row));return true;
        };
        std::uint32_t low=0,high=source_->selected20-1,mid=high/2;int compared=0;
        for(;;){sourcePC_=0x1583F8;if(!compare(mid,compared))return false;
            if(!compared){sourcePC_=0x158474;return false;}
            if(compared>=0)low=mid;else high=mid;mid=low+(high-low)/2;if(high-low<2)break;}
        sourcePC_=0x1584F0;if(!compare(low,compared))return false;
        if(!compared){sourcePC_=0x158560;return false;}
        sourcePC_=0x1585B0;if(!compare(high,compared))return false;
        if(!compared){sourcePC_=0x158620;return false;}
        sourcePC_=0x158640; // String getter0, then integer parser0 at158A54.
        sourcePC_=0x158B60;actualStack14C=0; // Genuine caller a2=0 default store, not a native initial value.
        sourcePhysicsObjectCountReturned_=true;sourcePC_=0x158B78;return true; // actual getter v0=0
    }
