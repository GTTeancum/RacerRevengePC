    struct SourceRegistration27C290 {
        OriginalSourcePhysicsObject50LA* owner=nullptr;
        const void* occurrence=nullptr;Model member=nullptr;
        std::int32_t zone=0,count=0,usedBefore=0,capacity=0,usedReload=0;
        Model* row=nullptr;
        std::uint32_t usedAfter=0,returnV0=0,pc=0;
        bool entered=false,countRead=false,usedRead=false,capacityRead=false,rowRead=false;
        bool memberStored=false,usedReloaded=false,usedStored=false,returned=false;
        bool pinned=false,nativeWithdrawn=false;
    };
    //Stable native occurrence addresses; not extra source allocation or guest rows.
    std::array<SourceRegistration27C290,1261> sourceRegistrations_{};
    std::size_t sourceRegistrationCount_=0;
    bool sourceRegistrationsClosing_=false;
    static std::int32_t sourceSignedWordNative(std::uint32_t word)noexcept {
        std::int32_t value;std::memcpy(&value,&word,sizeof value);return value;
    }
    bool registerSource27C290(const OriginalWreckComponent50LA& world,const void* occurrence,
        OriginalSourcePhysicsObject50LA& owner,std::int32_t zone,
        std::optional<std::uint32_t>& registrationIndex,bool& returned,
        std::uint32_t& returnV0,std::uint32_t& pc)noexcept {
        registrationIndex.reset();returned=false;returnV0=0;pc=0x27c290u;
        if(!occurrence||closed_||!sourcePrepared_||sourceRegistrationsClosing_||
           sourceWorld_!=&world||sourceWorldGeneration_!=world.sessionGeneration||!world.reference00||
           sourceThread_!=std::this_thread::get_id()||sourceRegistrationCount_==sourceRegistrations_.size()||
           !sourceCurrentNative()||owner.nativeWithdrawn_||!owner.object_||owner.physicsIndexBorrower_||
           !owner.preflightNative()||!owner.parentAttachment_.returned||
           owner.parentAttachment_.updateOccurrence!=occurrence||owner.parentAttachment_.updateReturnPC!=0x16677cu||
           owner.generation_!=world.sessionGeneration||owner.publishedIdentity_!=&owner.object_->entity)
            return false; //Native refusal, not original returned0.
        if(sourceRegistrationCount_&&!sourceRegistrations_[sourceRegistrationCount_-1].returned)return false;
        for(std::size_t i=0;i<sourceRegistrationCount_;++i)
            if(sourceRegistrations_[i].occurrence==occurrence||sourceRegistrations_[i].owner==&owner)return false;
        const auto index=sourceRegistrationCount_++;registrationIndex=static_cast<std::uint32_t>(index);
        auto& h=sourceRegistrations_[index];h.entered=true;h.owner=&owner;h.occurrence=occurrence;
        h.member=&owner.object_->entity;h.zone=zone;
        const auto at=[&](std::uint32_t address){pc=h.pc=sourcePC_=address;};
        const auto finish=[&](std::uint32_t value){h.returnV0=value;h.returned=true;
            returnV0=value;returned=true;at(0x27c32cu);return true;};
        at(0x27c290u);if(zone<0)return finish(0); //27C294 delay clears V0.
        at(0x27c298u);h.count=count00_;h.countRead=true;
        at(0x27c2a0u);if(zone>=h.count)return finish(0);
        at(0x27c2b4u);h.usedBefore=sourceSignedWordNative(used08_[zone]);h.usedRead=true;
        at(0x27c2b8u);if(h.usedBefore<0)return finish(0);
        at(0x27c2c8u);h.capacity=sourceSignedWordNative(capacity04_[zone]);h.capacityRead=true;
        at(0x27c2d0u);if(h.usedBefore>=h.capacity)return finish(0);
        at(0x27c2ecu);h.row=models0C_[zone];h.rowRead=true;
        at(0x27c2f0u);if(!h.row)return finish(0);
        at(0x27c308u);
        const auto& allocation=sourceCapacity_[zone];
        //Whole native slot custody precedes the first guest store. An incomplete
        //allocation/zeroing receipt cannot turn a native refusal into returned0.
        if(!allocation.returned||allocation.returnV0!=1u||!allocation.published||
           allocation.owner.get()!=h.row||static_cast<std::uint32_t>(h.usedBefore)>=allocation.zeroed||
           h.row[h.usedBefore]||owner.physicsIndexBorrower_)return false;
        owner.physicsIndexBorrower_=&h;h.pinned=true;
        h.row[h.usedBefore]=h.member;h.memberStored=true; //Actual27C308.
        at(0x27c318u);h.usedReload=sourceSignedWordNative(used08_[zone]);h.usedReloaded=true;
        at(0x27c31cu);h.usedAfter=static_cast<std::uint32_t>(h.usedReload)+1u;
        at(0x27c320u);used08_[zone]=h.usedAfter;h.usedStored=true;
        return finish(1); //JR27C324/delay27C328, not wrapper V0.
    }
    bool sourceRegistrationHistoryNative(std::array<std::uint32_t,10>& expectedUsed)const noexcept {
        expectedUsed={};
        if(sourceRegistrationCount_>sourceRegistrations_.size()||
           (sourceRegistrationCount_&&!sourcePrepared_))return false;
        bool anyWithdrawn=false;
        for(std::size_t i=0;i<sourceRegistrationCount_;++i){const auto& h=sourceRegistrations_[i];
            if(!h.entered||!h.owner||!h.occurrence||!h.member||h.returnV0>1u||
               (h.returned?h.pc!=0x27c32cu:h.pc!=0x27c308u)||
               (!h.returned&&i+1u!=sourceRegistrationCount_)||
               h.pinned!=h.memberStored||h.usedReloaded!=h.memberStored||h.usedStored!=h.memberStored||
               (h.memberStored&&(!h.returned||h.returnV0!=1u)))return false;
            for(std::size_t j=0;j<i;++j)if(sourceRegistrations_[j].occurrence==h.occurrence||
                sourceRegistrations_[j].owner==h.owner)return false;
            anyWithdrawn|=h.nativeWithdrawn;
            if(h.zone<0){
                if(h.countRead||h.usedRead||h.capacityRead||h.rowRead||h.memberStored||!h.returned||h.returnV0)return false;
            }else {
                if(!h.countRead||h.count!=count00_)return false;
                if(h.zone>=h.count){
                    if(h.usedRead||h.capacityRead||h.rowRead||h.memberStored||!h.returned||h.returnV0)return false;
                }else {
                    if(h.zone>=10||!h.usedRead||h.usedBefore!=static_cast<std::int32_t>(expectedUsed[h.zone]))return false;
                    if(h.usedBefore<0){
                        if(h.capacityRead||h.rowRead||h.memberStored||!h.returned||h.returnV0)return false;
                    }else {
                        if(!h.capacityRead||h.capacity!=sourceSignedWordNative(capacity04_[h.zone]))return false;
                        if(h.usedBefore>=h.capacity){
                            if(h.rowRead||h.memberStored||!h.returned||h.returnV0)return false;
                        }else {
                            if(!h.rowRead||h.row!=models0C_[h.zone])return false;
                            if(!h.row){if(h.memberStored||!h.returned||h.returnV0)return false;}
                            else if(!h.memberStored){if(h.returned||h.returnV0)return false;}
                        }
                    }
                }
            }
            if(!h.countRead&&h.count)return false;
            if(!h.usedRead&&h.usedBefore)return false;
            if(!h.capacityRead&&h.capacity)return false;
            if(!h.rowRead&&h.row)return false;
            if(!h.memberStored){if(h.usedReload||h.usedAfter||h.returnV0)return false;continue;}
            if(h.zone<0||h.zone>=10||!h.rowRead||!h.row||h.usedBefore<0||h.usedBefore>=h.capacity||
               h.usedReload!=h.usedBefore||h.usedAfter!=static_cast<std::uint32_t>(h.usedBefore)+1u)return false;
            const auto& allocation=sourceCapacity_[h.zone];
            if(!allocation.returned||allocation.returnV0!=1u||!allocation.published||
               allocation.owner.get()!=h.row||static_cast<std::uint32_t>(h.usedBefore)>=allocation.zeroed||
               h.row[h.usedBefore]!=(h.nativeWithdrawn?nullptr:h.member))return false;
            if(!h.nativeWithdrawn){
                if(h.owner->nativeWithdrawn_||!h.owner->object_||h.owner->physicsIndexBorrower_!=&h||
                   h.member!=&h.owner->object_->entity||!h.owner->preflightNative()||
                   !h.owner->parentAttachment_.returned||h.owner->parentAttachment_.updateOccurrence!=h.occurrence)
                    return false;
            } //After unpin, owner/member remain opaque historical identities only.
            ++expectedUsed[h.zone];
        }
        return sourceRegistrationsClosing_==anyWithdrawn;
    }
    bool withdrawSourceRegistrationNative(std::uint32_t index,const void* occurrence,
        OriginalSourcePhysicsObject50LA& owner)noexcept {
        if(index>=sourceRegistrationCount_||!occurrence)return false;
        auto& h=sourceRegistrations_[index];
        if(h.occurrence!=occurrence||h.owner!=&owner)return false;
        if(h.nativeWithdrawn)return true;
        if(closed_||!sourceCurrentNative())return false;
        if(h.memberStored){
            if(owner.physicsIndexBorrower_!=&h||!h.row||h.usedBefore<0||h.row[h.usedBefore]!=h.member)return false;
            h.row[h.usedBefore]=nullptr;owner.physicsIndexBorrower_=nullptr;
        }
        h.nativeWithdrawn=true;sourceRegistrationsClosing_=true;
        return true; //Native borrow cancellation, NOT27C330/reset or used08 decrement.
    }
