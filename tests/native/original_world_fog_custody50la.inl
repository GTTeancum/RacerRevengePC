// Included by the actual World producer TU after ra_pod_scene50la.h.
// No independent TU, runtime callbacks, renderer publication or numeric replay.
namespace ra_pod_scene50LA {
bool Owner::canRetireNativeFogCustody()const noexcept {
    if(!nativeFog3EC_&&!nativeFogOff3F4_&&!nativeFogOn3F0_)return true;
    if(!nativeFogAttempted_||nativeFogRetired_||!sceneApplication_||sceneApplication_->released_||
       sceneApplication_->attachmentLock423F58()||!sceneGeneration_)return false;
    std::uint32_t counted=0;
    const auto valid=[&](const NativeFogEntity& owner,std::uint32_t constructor){
        if(!owner.counted)return !owner.published&&!owner.constructorPC&&!owner.entity.reference00&&
            owner.entity.kind==g::OriginalWreckComponentKind50LA::unbound&&!owner.entity.objectKey&&
            !owner.entity.sessionGeneration&&!owner.entity.parent1C&&!owner.entity.firstChild18;
        const auto& e=owner.entity;++counted;
        return owner.published&&owner.constructorPC==constructor&&e.reference00==1u&&
            e.kind==g::OriginalWreckComponentKind50LA::entity141300&&
            e.sessionGeneration==sceneGeneration_&&e.objectKey==key(&e)&&
            !e.previous10&&!e.next14&&!e.firstChild18&&!e.parent1C&&!e.mask20&&!e.descendants24&&
            e.category28==0xffu&&(e.flags29&15u)==3u&&!e.nativeFlags29&&
            sceneApplication_->entityAuthority_.canRetire(e);
    };
    if(nativeFog3EC_){const auto& fog=*nativeFog3EC_;
        if(!valid(fog,0x13e6b0)||fog.field44||fog.field48!=1u||fog.field4C)return false;
        if(fog.buffer5C){const auto& b=*fog.buffer5C;
            if(!fog.counted||!fog.bufferAttempted||!nativeFogOff3F4_||!nativeFogOn3F0_||
               !nativeFogOff3F4_->counted||!nativeFogOn3F0_->counted||
               b.field20||b.field24||b.field28||b.field2C!=-1)return false;
            for(const auto& lane:b.unknown00To1C)if(lane)return false;
        }
    }
    if(nativeFogOff3F4_&&(!nativeFog3EC_||!nativeFog3EC_->published||
       !valid(*nativeFogOff3F4_,0x13f850)))return false;
    if(nativeFogOn3F0_&&(!nativeFogOff3F4_||!nativeFogOff3F4_->published||
       !valid(*nativeFogOn3F0_,0x13f980)))return false;
    if(nativeFogOff3F4_&&nativeFogOff3F4_->initialized&&
       (!nativeFogOff3F4_->counted||!nativeFog3EC_->buffer5C))return false;
    if(nativeFogOn3F0_&&nativeFogOn3F0_->borrowedFog30&&
       (!nativeFogOn3F0_->counted||!nativeFogOff3F4_->initialized||
        *nativeFogOn3F0_->borrowedFog30!=nativeFog3EC_.get()))return false;
    return counted<=sceneApplication_->objectCount423F60();
}
bool Owner::nativeFogCustodyCurrent(const g::OriginalRaceFogFields* actual)const noexcept {
    // Compare actual borrow identity only. No read of already-native numeric or
    // dirty60 lanes, no claim that a source constructor initialized those lanes.
    return actual&&!nativeFogRetired_&&nativeFog3EC_&&nativeFog3EC_->actualFields==actual&&
        canRetireNativeFogCustody();
}
bool Owner::constructNativeFog273CAC(const g::OriginalRaceFogFields& actual,std::uint32_t& pc)noexcept {
    pc=0x273cac;
    if(nativeFogFailed_){pc=nativeFogFailurePC_;return false;}
    if(nativeFogRetired_||!sceneApplication_||sceneApplication_->released_||!sceneGeneration_||
       sceneApplication_->attachmentLock423F58())return false;
    if(nativeFogAttempted_){
        if(!nativeFogCustodyCurrent(&actual)||!nativeFogOn3F0_||!nativeFogOn3F0_->published)return false;
        pc=0x273d08;return true; //Actual completed constructor prefix, no recount.
    }
    nativeFogAttempted_=true;
    const auto failed=[&](){nativeFogFailed_=true;nativeFogFailurePC_=pc;return false;};
    const auto count=[&](NativeFogEntity& owner,std::uint32_t ctor,std::uint32_t publication){
        auto& authority=sceneApplication_->entityAuthority_;auto& total=sceneApplication_->objectCount423F60();
        if(total==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
           !authority.canEnroll(owner.entity,key(&owner.entity)))return false;
        pc=ctor;
        if(g::initializeOriginalWreckComponent50LA(owner.entity,g::OriginalWreckComponentKind50LA::entity141300,
           sceneGeneration_,key(&owner.entity),true,total).status!=g::OriginalWreckComponentStatus50LA::complete||
           !authority.enroll(owner.entity))std::terminate();
        owner.counted=true;owner.constructorPC=ctor;
        pc=publication;owner.published=true;return true;
    };
    try {
        nativeFog3EC_=std::make_unique<NativeFog13E6B0>();nativeFog3EC_->actualFields=&actual;
        if(!count(*nativeFog3EC_,0x13e6b0,0x273cd0))return failed();
        pc=0x273ccc;nativeFogOff3F4_=std::make_unique<NativeFogOff13F850>();
        if(!count(*nativeFogOff3F4_,0x13f850,0x273cf0))return failed();
        pc=0x273cec;nativeFogOn3F0_=std::make_unique<NativeFogOn13F980>();
        if(!count(*nativeFogOn3F0_,0x13f980,0x273d08))return failed();
        if(!canRetireNativeFogCustody())return failed();
        return true;
    }catch(...){return failed();}
}
bool Owner::allocateNativeFogBuffer13E780(std::uint32_t& pc)noexcept {
    pc=0x13e780;if(nativeFogFailed_){pc=nativeFogFailurePC_;return false;}
    if(!canRetireNativeFogCustody()||nativeFogRetired_||!nativeFog3EC_||!nativeFogOn3F0_||
       !nativeFogOn3F0_->published)return false;
    auto& fog=*nativeFog3EC_;
    if(fog.buffer5C){pc=0x13e830;return true;}
    if(fog.bufferAttempted)return false;
    fog.bufferAttempted=true;
    // Source50/54/58 already belong to actual native CameraBinding fields.
    // This creates missing5C custody ONLY; no false stores or renderer receipt.
    try {pc=0x13e7d0;fog.buffer5C=std::make_unique<NativeFogBuffer133F80>();}
    catch(...){nativeFogFailed_=true;nativeFogFailurePC_=pc;return false;}
    pc=0x13e830;return true;
}
bool Owner::initializeNativeFogOff13F890(std::uint32_t& pc)noexcept {
    pc=0x273e18;
    if(nativeFogFailed_||!canRetireNativeFogCustody()||!nativeFog3EC_||!nativeFog3EC_->buffer5C||
       !nativeFogOff3F4_||!nativeFogOff3F4_->published)return false;
    // Source13F890 calls literal return-this141C20. This records THAT local
    // service only; caller must first join13E830 actual renderer publication.
    nativeFogOff3F4_->initialized=true;pc=0x13f8b0;return true;
}
bool Owner::bindNativeFogOn13F9C0(std::uint32_t& pc)noexcept {
    pc=0x273ec8;
    if(nativeFogFailed_||!canRetireNativeFogCustody()||!nativeFogOff3F4_||!nativeFogOff3F4_->initialized||
       !nativeFogOn3F0_||!nativeFogOn3F0_->published)return false;
    //13F9D4 return-this141C20 then13F9DC raw pointer store. No retain.
    nativeFogOn3F0_->borrowedFog30=nativeFog3EC_.get();pc=0x13f9dc;return true;
}
bool Owner::retireNativeFogCustody(std::uint32_t& pc)noexcept {
    pc=0x13fa70;if(!canRetireNativeFogCustody())return false;
    const auto retire=[&](NativeFogEntity& owner){
        if(!owner.counted)return;
        if(!sceneApplication_->entityAuthority_.retire(owner.entity))std::terminate();
        --owner.entity.reference00;--sceneApplication_->objectCount423F60();owner.counted=false;
    };
    // Explicit native partial-load ordering: kill borrower before borrowedFog.
    // Not a receipt for unrepresented full retail World destruction order.
    if(nativeFogOn3F0_){nativeFogOn3F0_->borrowedFog30.reset();retire(*nativeFogOn3F0_);nativeFogOn3F0_.reset();}
    pc=0x13fad0;if(nativeFogOff3F4_){retire(*nativeFogOff3F4_);nativeFogOff3F4_.reset();}
    pc=0x13e728;if(nativeFog3EC_){
        //133FA0 has null20 on this reached constructor-only buffer; no backing
        //allocation/free305728 was ever reached. Buffer allocation freed first.
        nativeFog3EC_->buffer5C.reset();nativeFog3EC_->actualFields=nullptr;
        pc=0x13e740;retire(*nativeFog3EC_);nativeFog3EC_.reset();
    }
    nativeFogRetired_=nativeFogAttempted_;pc=0x13e774;return true;
}
}
