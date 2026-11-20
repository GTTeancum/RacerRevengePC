namespace ra_pod_scene50LA {
bool Owner::nativeEcosystemNone1AA410(const rr::assets::World48F& source,std::uint32_t& pc)const noexcept {
    pc=0x1aa408;
    if(!lighting_||!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    const auto* name=lighting_->nativeRaEcosystemSelector1AA410(this,source,visibilityRows_,pc);
    if(!name||std::strcmp(name,"none"))return false;
    //1AA418 skips +138 entirely. No source408 write occurs on this branch.
    //Native no-Ecosystem admission uses actual fresh-constructor publication,
    //not a second independent absence authority or inferred loaded-null.
    if(!worldReset270550Complete_||!worldEcosystem408_||*worldEcosystem408_)return false;
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
// Common rewrite (R3): removed - the RA-only 1A9EE0 entry. The one track load is common_track50CR::Load (common_track_load50cr.cpp), run before the camera session for every course.
