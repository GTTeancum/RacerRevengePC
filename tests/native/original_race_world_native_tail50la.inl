namespace ra_pod_scene50LA {
bool Owner::nativeEcosystemNone1AA410(const rr::assets::World48F& source,std::uint32_t& pc)const noexcept {
    pc=0x1aa408;
    if(!lighting_||!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    const auto* name=lighting_->nativeRaEcosystemSelector1AA410(this,source,visibilityRows_,pc);
    if(!name||std::strcmp(name,"none"))return false;
    //1AA418 skips +138 entirely. Calling2748F0 withnone would still allocate
    //an Ecosystem: that is NOT this branch and no World408 is inferred.
    pc=0x1aa438;return true;
}
}
namespace ra_cold_aggregate50LA {
bool Owner::skipNativeEcosystem1AA410(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)const noexcept {
    if(!source_||!source_->sourcesDecoded()||!nativeSceneInfo270900Current(app,pc)||
       !nativeWaterNone274570_||!nativeWaterNone274570_->returned274800||
       *nativeWaterNone274570_->returned274800!=1u)return false;
    return nativeEcosystemNone1AA410(source_->sourcesDecoded()->world,pc);
}
}
namespace rr::game {
bool OriginalRaceConstructor50LA::continueNativeTrackAfterWater(
    const original_main_entry50KG::Application& app,std::uint32_t& pc)noexcept {
    pc=0x1aa400;
    using Step=NativeTrackLoadStep;
    if(resultsFeedbackThread_!=std::this_thread::get_id()||!counted_||rolledBack_||
       !application_||application_!=app.entities()||application_->attachmentLock423F58()||
       !nativeIntroWorldEntity()||!world74_||world74_->worldApplication_!=application_||
       world74_->worldEntity_.sessionGeneration!=entity_.sessionGeneration||
       (nativeTrackLoadStep_!=Step::aubRetired&&nativeTrackLoadStep_!=Step::nativeAdaptedReturn)||
       nativeWater70State_==NativeWater70State::withdrawn)return false;
    const auto* domain=world74_->readNativeAbsentWater400(pc);
    if(!domain||domain!=world74_->nativeWaterDomain_.get())return false;
    if(nativeWater70State_==NativeWater70State::published){
        const auto retained=nativeWater70Domain_.lock();
        if(!retained||retained.get()!=domain||!nativeWater70Object_)return false;
    }else{
        //Explicit native representation of1AA400/404: copy ACTUAL current
        //no-water selection under its constructor+Context authority. This is
        //NOT a guest400 null read or proof270550 ran on the adopted World.
        pc=0x1aa404;nativeWater70Domain_=world74_->nativeWaterDomain_;
        nativeWater70Object_.emplace(nullptr);nativeWater70State_=NativeWater70State::published;
    }
    if(!world74_->skipNativeEcosystem1AA410(app,pc))return false;
    //1AA41C supplies s0=0x104 even on taken default-none branch; assertion
    //s0==4 is impossible here. No skipped +138 resource or408 reset is invented.
    const auto* nul=static_cast<const char*>(std::memchr(savedModelDirectory1A9EE0_.data(),0,
        savedModelDirectory1A9EE0_.size()));
    if(!nul)return false;
    const auto length=static_cast<std::size_t>(nul-savedModelDirectory1A9EE0_.data());
    if(length>259u)return false;
    try {
        const std::string restored(savedModelDirectory1A9EE0_.data(),length);
        if(nativeTrackLoadStep_==Step::nativeAdaptedReturn){
            if(app.modelDirectory36B860()!=restored)return false;
        }else{
            //Actual retained1A9F68 preimage, not a fabricated entry directory.
            //Native string assignment is atomic relative to its allocation
            //failure, not a claim of bytewise guest memcpy failure behavior.
            pc=0x1aa4f0;app.modelDirectory36B860()=restored;
            nativeTrackLoadStep_=Step::nativeAdaptedReturn;
        }
    }catch(...){return false;} //Real70 copy/default skip retained; no source step replay.
    pc=0x1aa508;return true; //Represented native-adapted1A9EE0 only, not fullretailWorld/Race load.
}
}
