// Private actual vC8 1B4890->1B5090. Missing Buoys retains the last
// ParticleEmitter4 section; the actual integer getter searches those six fields.
    bool sourceBuoysAttempted_=false,sourceBuoysMissing_=false;
    bool sourceBuoyCountAttempted_=false,sourceBuoyCountReturned_=false;
    bool sourceBuoysMissingCurrent()const noexcept {
        return sourceBuoysAttempted_&&sourceBuoysMissing_&&sourceObjectDirectoryCurrent()&&
            sourceParticleDescriptorNextSelection_==5&&sourceParticleDescriptorSelected_&&*sourceParticleDescriptorSelected_==4&&
            sectionAttempts_==sourceParticleDescriptorBaseAttempt_+6&&sectionReturned_&&
            source_->selected20==6&&source_->field4C8==1378&&source_->block4C4==block_.get()&&
            block_&&sourceSectionPayloadCurrent(302)&&std::strcmp(source_->name28.data(),"ParticleEmitter4")==0;
    }
    bool selectSourceBuoys157D00(bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(sourceBuoysAttempted_||sourceParticleDescriptorNextSelection_!=5||!sourceParticleDescriptorSelected_||
           *sourceParticleDescriptorSelected_!=4||!sourceParticleDescriptorConfigCurrent())return false;
        sourceBuoysAttempted_=true;++sectionAttempts_;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=713)return false;
            const auto* candidate=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(candidate,0,1024))return false;
            result=std::strcmp("buoys",candidate);return true;
        };
        std::uint32_t low=0,high=712,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x157BE8;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}
            else {sourcePC_=0x157C1C;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(found){sourcePC_=0x157E90;return false;} // Positive section is not this bounded cohort.
        sourceBuoysMissing_=true;sourcePC_=0x157DD8;return true;
    }
    bool readSourceBuoyCount158B30(std::optional<std::int32_t>& actualWorld538,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x158B30;
        if(sourceBuoyCountAttempted_||actualWorld538||!sourceBuoysMissingCurrent())return false;
        sourceBuoyCountAttempted_=true;
        // 158B48 -> 158920 -> 158934/158240, over actual retained six-field cache.
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=source_->selected20)return false;
            const auto* row=block_.get()+std::size_t(i)*43;
            if(!std::memchr(row,0,35))return false;
            result=std::strcmp("numberofbuoys",reinterpret_cast<const char*>(row));return true;
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
        sourcePC_=0x158B60;actualWorld538=0; // Genuine caller a2=0 default store, not a native initial value.
        sourceBuoyCountReturned_=true;sourcePC_=0x158B78;return true; // actual getter v0=0
    }
