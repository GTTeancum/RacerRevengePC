// Private real TOB Terrain selections. Tuples qualify actual reads, never replace payload.
    struct SourceTerrainSection {const char* name;const char* lower;std::uint32_t ordinal,offset,bytes,count;};
    inline static constexpr SourceTerrainSection sourceTerrainSections_[]={
        {"TerrainZone1","terrainzone1",690,91477,90,2},
        {"TerrainZone2","terrainzone2",691,91571,231,5},
        {"TerrainZone3","terrainzone3",692,91806,231,5},
        {"TerrainZone4","terrainzone4",693,92041,231,5},
        {"TerrainZone5","terrainzone5",694,92276,231,5},
        {"TerrainZone6","terrainzone6",695,92511,231,5},
        {"TerrainZone7","terrainzone7",696,92746,231,5},
        {"TerrainZone8","terrainzone8",697,92981,231,5},
        {"TerrainZoneCollision","terrainzonecollision",698,93216,45,1},
        {"ZoneCollisionObject1","zonecollisionobject1",704,94478,116,2},
        {"ZoneCollisionObject2","zonecollisionobject2",705,94598,116,2},
        {"ZoneCollisionObject3","zonecollisionobject3",706,94718,116,2},
        {"ZoneCollisionObject4","zonecollisionobject4",707,94838,116,2},
        {"ZoneCollisionObject5","zonecollisionobject5",708,94958,116,2},
        {"ZoneCollisionObject6","zonecollisionobject6",709,95078,116,2},
        {"ZoneCollisionObject7","zonecollisionobject7",710,95198,116,2},
        {"ZoneCollisionObject8","zonecollisionobject8",711,95318,116,2},
        {"ZoneCollisionObject9","zonecollisionobject9",712,95438,116,2},
    };
    std::uint32_t sourceTerrainNextSelection_=0,sourceTerrainBaseAttempt_=0;
    std::optional<std::uint32_t> sourceTerrainSelected_;
    bool sourceTerrainConfigCurrent()const noexcept {
        if(!sourceTerrainSelected_||*sourceTerrainSelected_>=18)return false;
        const auto& expected=sourceTerrainSections_[*sourceTerrainSelected_];
        return sourceObjectDirectoryCurrent()&&sectionReturned_&&sourceTerrainNextSelection_&&
            sectionAttempts_==sourceTerrainBaseAttempt_+sourceTerrainNextSelection_&&
            source_->selected20==expected.count&&source_->field4C8==(*sourceTerrainSelected_?231u:135u)&&
            source_->block4C4==block_.get()&&block_&&sourceSectionPayloadCurrent(expected.bytes)&&
            std::strcmp(source_->name28.data(),expected.name)==0;
    }
    bool selectSourceTerrainSection157D00(std::uint32_t selected,bool& returned,D& d)noexcept {
        d={};returned=false;sourcePC_=0x157D1C;
        if(selected>=18||selected!=sourceTerrainNextSelection_)return false;
        if(!selected){
            if(objectSectionPhase_!=ObjectSectionPhase::animationsMissing||objectPodReads_!=2||
               (sectionAttempts_!=2&&sectionAttempts_!=3)||!sourceObjectPodCurrent())return false;
            sourceTerrainBaseAttempt_=sectionAttempts_;
        }else if(!sourceTerrainConfigCurrent())return false;
        if(!sourceObjectDirectoryCurrent()||sectionAttempts_!=sourceTerrainBaseAttempt_+selected)return false;
        ++sourceTerrainNextSelection_;++sectionAttempts_;
        const auto& expected=sourceTerrainSections_[selected];
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=713)return false;const auto* candidate=reinterpret_cast<const char*>(names_.get()+std::size_t(i)*1024);
            if(!std::memchr(candidate,0,1024))return false;result=std::strcmp(expected.lower,candidate);return true;
        };
        std::uint32_t low=0,high=712,mid=high/2;int compared=0;bool found=false;
        for(;;){sourcePC_=0x157B8C;if(!compare(mid,compared))return false;
            if(!compared){found=true;break;}if(compared>=0)low=mid;else high=mid;
            mid=low+(high-low)/2;if(high-low<2)break;}
        if(!found){sourcePC_=0x157BE8;if(!compare(low,compared))return false;
            if(!compared){mid=low;found=true;}
            else {sourcePC_=0x157C1C;if(!compare(high,compared))return false;if(!compared){mid=high;found=true;}}}
        if(!found){sourcePC_=0x157DD8;return true;} // Actual0; caller must bound before unjoined missing-section continuation, never grant later selection.
        if(mid!=expected.ordinal)return false;
        sectionReturned_=false;const auto length=std::strlen(expected.name);
        std::memcpy(source_->name28.data(),expected.name,length);source_->name28[length]=0;sourcePC_=0x157E9C;
        const auto offset=little(index_.get()+std::size_t(mid)*8),bytes=little(index_.get()+std::size_t(mid)*8+4);
        blockSectionBytes_=bytes;blockPayloadReturned_=false; //Actual index length, not4C8 read size.
        if(offset!=expected.offset||bytes!=expected.bytes){sourcePC_=0x157EB0;return false;}
        // Same native lowering of actual realloc: source capacity first, NULL publication
        // unconditional, old backing retained separately only for native withdrawal.
        if(source_->field4C8<bytes){
            const auto oldBytes=source_->field4C8;source_->field4C8=bytes;sourcePC_=0x157ECC;
            std::unique_ptr<std::uint8_t[]> next(new(std::nothrow) std::uint8_t[bytes]);
            if(next){std::memcpy(next.get(),block_.get(),oldBytes);block_=std::move(next);}
            else {lostBlock_=std::move(block_);lostBlockKnownBytes_=blockKnownBytes_;blockKnownBytes_=0;}
            source_->block4C4=block_.get();sourcePC_=0x157EE0;
        }
        sourcePC_=0x157EF4;
        if(!file_->seek12F640(*this,source_->file00,offset,d)){sourcePC_=0x157F08;return false;}
        std::array<std::uint8_t,4> count{};std::size_t done=0;
        if(!readExact(count.data(),4,done,0x157F28,0x157F30,d))return false;
        source_->selected20=little(count.data());sourcePC_=0x157F38;
        if(!block_){sourcePC_=0x157F4C;return false;}
        if(!readSourceSectionPayload157F4C(d))return false;
        if(source_->selected20!=expected.count)return false;
        sourceTerrainSelected_=selected;
        sectionReturned_=true;returned=true;sourcePC_=0x15821C;return true;
    }
    bool readSourceTerrainField158240(const char* lower,std::string_view& value,bool& found,
        std::uint32_t& reachedCopyPC,D& d)noexcept {
        d={};value={};found=false;reachedCopyPC=0;sourcePC_=0x158280;
        if(!sourceTerrainConfigCurrent()||!lower)return false;
        bool admitted=false;
        if(*sourceTerrainSelected_<8){
            static constexpr const char* keys[]={"surfacefriction","surfacedrag","surfacetraction","generatedust","generatedirt"};
            for(const auto* key:keys)if(std::strcmp(lower,key)==0){admitted=true;break;}
        }else if(*sourceTerrainSelected_==8)admitted=std::strcmp(lower,"numcollisionobjects")==0;
        else admitted=std::strcmp(lower,"obj")==0||std::strcmp(lower,"terrainzone")==0;
        if(!admitted)return false;
        const auto compare=[&](std::uint32_t i,int& result)noexcept {
            if(i>=source_->selected20)return false;const auto* row=block_.get()+std::size_t(i)*43;
            if(!std::memchr(row,0,35))return false;result=std::strcmp(lower,reinterpret_cast<const char*>(row));return true;
        };
        const auto capture=[&](std::uint32_t i)noexcept {
            const auto offset=little(block_.get()+std::size_t(i)*43+35);if(offset>=sourceSelectedKnownBytes())return false;
            const auto* begin=reinterpret_cast<const char*>(block_.get()+offset);
            const auto* end=static_cast<const char*>(std::memchr(begin,0,sourceSelectedKnownBytes()-offset));if(!end)return false;
            reachedCopyPC=sourcePC_==0x1583F8?0x158474:sourcePC_==0x1584F0?0x158560:0x158620;
            fieldCopyPC_=reachedCopyPC;value=std::string_view(begin,std::size_t(end-begin));found=true;return true;
        };
        std::uint32_t low=0,high=source_->selected20-1,mid=high/2;int compared=0;
        for(;;){sourcePC_=0x1583F8;if(!compare(mid,compared))return false;if(!compared)return capture(mid);
            if(compared>=0)low=mid;else high=mid;mid=low+(high-low)/2;if(high-low<2)break;}
        sourcePC_=0x1584F0;if(!compare(low,compared))return false;if(!compared)return capture(low);
        sourcePC_=0x1585B0;if(!compare(high,compared))return false;if(!compared)return capture(high);
        sourcePC_=0x158640;return true;
    }
