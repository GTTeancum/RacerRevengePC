//Private actual2D9C94 selection on the SAME713-name TOB occurrence.
    bool sourceToyAnimationsAttempted_=false,sourceToyAnimationsMissing_=false;
    bool sourceToyAnimationsMissingCurrent()const noexcept {
        return sourceToyAnimationsAttempted_&&sourceToyAnimationsMissing_&&sourceObjectDirectoryCurrent()&&
            sourceTerrainNextSelection_==18&&sourceTerrainSelected_&&*sourceTerrainSelected_==17&&
            sectionAttempts_==sourceTerrainBaseAttempt_+19&&sectionReturned_&&
            source_->selected20==2&&source_->field4C8==231&&source_->block4C4==block_.get()&&
            block_&&sourceSectionPayloadCurrent(116)&&std::strcmp(source_->name28.data(),"ZoneCollisionObject9")==0;
    }
    bool selectSourceToyAnimations157D00(bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(sourceToyAnimationsAttempted_||sourceTerrainNextSelection_!=18||!sourceTerrainSelected_||
           *sourceTerrainSelected_!=17||!sourceTerrainConfigCurrent())return false;
        sourceToyAnimationsAttempted_=true;++sectionAttempts_;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=713)return false;const auto* candidate=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(candidate,0,1024))return false;result=std::strcmp("animations",candidate);return true;
        };
        std::uint32_t low=0,high=712,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x157BE8;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}
            else {sourcePC_=0x157C1C;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(found){sourcePC_=0x157E90;return false;} //Unexpected positive: no name/cache publication or toy parser grant.
        //Actual missing search never publishes name/count/cache or changes File cursor.
        sourceToyAnimationsMissing_=true;sourcePC_=0x157DD8;return true;
    }
