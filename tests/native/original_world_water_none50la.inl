namespace ra_pod_scene50LA {
bool Owner::acceptNativeWaterNone274570(const rr::assets::World48F& source,
    std::uint8_t bits,std::uint8_t course,std::uint32_t& pc)noexcept {
    pc=0x274570;
    if(!worldApplication_||worldApplication_->attachmentLock423F58()||!lighting_||
       !nativeSky3F8_||!nativeSky3F8_->owner||!nativeSky3F8_->owner->tailComplete||course>15u)return false;
    const auto* name=lighting_->nativeRaWaterSelector274570(this,source,visibilityRows_,pc);
    if(!name||std::strcmp(name,"none"))return false;
    if(!worldReset270550Complete_||!worldWater3FC_||*worldWater3FC_||
       !worldWater400_||*worldWater400_)return false;
    NativeWaterNone274570 next;next.environment110=name;next.profile1125C=bits;
    next.course1125FLow4=course;next.argumentA2=(bits>>5u)&3u;
    next.argumentA3=(course==6u&&next.argumentA2==1u)?0u:3u;
    if(nativeWaterNone274570_){const auto& old=*nativeWaterNone274570_;
        if(old.environment110!=next.environment110||old.profile1125C!=bits||
           old.course1125FLow4!=course||old.argumentA2!=next.argumentA2||
           old.argumentA3!=next.argumentA3||old.returned2745CC!=1u)return false;
    }else nativeWaterNone274570_=next;
    //2745C0 equal branch reaches2745CC return1 before parameter-grid or
    //provider/allocation/400 code. No native fixture Absence is consulted.
    pc=0x1aa3c0;return true;
}
bool Owner::acceptNativeWaterNone2747C0(const rr::assets::World48F& source,
    std::uint8_t bits,std::uint32_t& pc)noexcept {
    pc=0x2747c0;
    if(!worldApplication_||worldApplication_->attachmentLock423F58()||!lighting_||!nativeWaterNone274570_)return false;
    auto& receipt=*nativeWaterNone274570_;
    const auto* name=lighting_->nativeRaWaterSelector274570(this,source,visibilityRows_,pc);
    if(!name||name!=receipt.environment110||std::strcmp(name,"none")||receipt.returned2745CC!=1u)return false;
    if(!worldWater400_||*worldWater400_||!worldWater404_||*worldWater404_)return false;
    const auto a2=std::uint32_t((bits>>5u)&3u);
    if(receipt.returned274800){
        if(*receipt.returned274800!=1u||!receipt.profile134_1125C||
           *receipt.profile134_1125C!=bits||!receipt.argument134A2||*receipt.argument134A2!=a2)return false;
    }else{
        receipt.profile134_1125C=bits;receipt.argument134A2=a2;
        pc=0x274800;receipt.returned274800=1u;
    }
    //2747EC/2747FC exact same-none branch; no access to400 or404.
    pc=0x1aa400;return true;
}
}
namespace ra_cold_aggregate50LA {
bool Owner::continueNativeWater274570(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa33c;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeSceneInfo270900Current(app,pc)||!source_||!source_->sourcesDecoded()||
       !app.riderProfile||!worldApplication_||worldApplication_->attachmentLock423F58()||
       !nativeTrackLoadBorrowsAdmitted())return false;
    const auto* profile=app.riderProfile->commonProfile();
    rr::platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection selected;
    if(!profile||!profile->known||!app.riderProfile->courseRaceSnapshot(selected)||
       selected.course>15u||std::uint8_t(selected.packed1125C)!=profile->playerBits1125C)return false;
    //Existing readonly observation reads actual1125F low4 and1125C, not
    //remembered course preference, selection count, or a fabricated mode.
    //Race8C is an unused operand on this exact early branch; no invented
    //value/read receipt is introduced merely to fill a source argument slot.
    if(!acceptNativeWaterNone274570(source_->sourcesDecoded()->world,
       profile->playerBits1125C,static_cast<std::uint8_t>(selected.course),pc))return false;
    //1AA3C0 rereads the SAME actual Profile independently for slot134.
    const auto* nextProfile=app.riderProfile->commonProfile();
    if(!nextProfile||!nextProfile->known)return false;
    if(!acceptNativeWaterNone2747C0(source_->sourcesDecoded()->world,nextProfile->playerBits1125C,pc))return false;
    pc=0x1aa400;return false; //BEFORE actual400 read/Race70 store; value remains unknown.
}
}
