#pragma once
namespace ra_cold_aggregate50LA {
bool Owner::continueSourceStandardSuccess27AC64(ra_static_model_cold50LA::Owner::StandardWrapper& w,std::uint32_t& pc)noexcept {
    const auto at=[&](std::uint32_t p){pc=sourceTrackPC_=sourcePostParticleModelsPC_=p;};
    //Reached only after exact returned Entity+8 from genuine source
    //geometry/import/cache/fileclose/rowtable/240740 packet suffix.
    //Native prepared Model/resourcesComplete flags cannot satisfy this gate.
    if(!sourceStandardCallbackEntered_||sourceStandardWrapperA0_!=&w||!sourceStaticOwner_||
       !worldApplication_||!w.sourceModelCallbackEntered||!w.sourceModel||!w.descriptor10||
       !w.descriptor10->sourceFields||w.sourceCallbackReturnV0||w.sourceDescriptorDisposed)return false;
    auto& f=*w.sourceModel;auto& scene=f.scene;auto& p=f.placement;auto& d=*w.descriptor10->sourceFields;
    if(p.entered||f.loaderReturnV0||f.loaderReturnedEntity!=&scene.model_.entity()||!f.lookupReturn||!*f.lookupReturn||
       f.nativeRetired||f.nativeAliasWithdrawn||f.sourceOuterDeleted||f.sourceReferenceDropped||
       !f.counted||!f.enrolled||!w.sourceModelPublished||w.sourceModelCleared||w.model8!=&scene||
       !scene.sourceConstructor_.modelReturned||scene.released_||scene.referenceReleased_||
       !scene.rowsLoaded_||!scene.fields_.geometry1B4||!scene.fields_.sourceRows1B8||
       scene.fields_.sourceRows1B8!=scene.rowTable_.get()||!d.loaded||!w.sourceModelNameFormatted||
       !worldApplication_->entityAuthority_.canRetire(scene.model_.entity())||!f.graphCount||
       f.graphCount>a::maxModelNodes49O||scene.model_.sceneIndex!=static_cast<g::SceneNodeIndex49P>(0))return false;
    p.entered=true;at(0x27ac74);if(!d.shadow148)return false;p.shadow148=*d.shadow148;
    p.priorFlags1B3=scene.fields_.flags1B3;
    at(0x27ac94);scene.fields_.flags1B3=static_cast<std::uint8_t>((p.priorFlags1B3&~0x10u)|
        ((static_cast<std::uint32_t>(*p.shadow148)&1u)<<4));p.shadowStored=true;
    //s0=128 at27AC70 is unequal4, so443048's entire lazy assertion is skipped.
    at(0x27ad04);
    if(!d.name180Known||d.name180Known>d.name180.size()||d.name180[d.name180Known-1])return false;
    std::size_t length=0;while(length<d.name180Known&&d.name180[length])++length;
    if(length==d.name180Known)return false;if(length>127)length=127;
    at(0x27ad34);
    try{scene.model_.name38.assign(d.name180.data(),length);}catch(...){return false;}
    std::memcpy(p.copiedName.data(),d.name180.data(),length);
    at(0x27ad44);p.copiedName[length]=0;p.nameLength=length;p.nameStored=true;
    at(0x27ad48);
    if(!g::OriginalStandardPlacementGlobals50LA::parent27AD48(p.parentGuard))return false;
    //Original assertion reads parent180 and compares its SceneObject identity
    //with THIS model, not the embeddedEntity address. Native indices encode
    //this exact same-owner relationship; never fabricate a guest pointer.
    at(0x27ad84);p.parentNotSelf=f.nodes[0].parent180!=scene.model_.sceneIndex;
    at(0x27ad8c);p.parentChecked=true;
    if(p.parentGuard.wordValue&&!p.parentNotSelf)return false; //Unjoined actual diagnostic/break.
    at(0x27adcc);at(0x28ab90);if(!(d.vectorKnown&1u))return false;
    p.position=d.position110;g::SceneDisplacement49P translation;
    std::memcpy(translation.v,p.position.data(),sizeof translation.v);
    //Reviewed copy-only XYZ leaf preserves W and all other13 local words;
    //native graph/rounding admission is not a source success/failure branch.
    if(!g::setLocalTranslation49P(f.graph(),scene.model_.sceneIndex,translation))return false;
    p.positionReturned=true;
    at(0x27adec);at(0x289ecc);p.orientationEntered=true;
    if(!g::OriginalStandardPlacementGlobals50LA::orientation289ECC(p.orientationGuard))return false;
    if((d.vectorKnown&6u)!=6u){at(0x289f5c);return false;}
    p.look=d.look120;p.up=d.up130;
    std::array<float,3> position{},look{},up{};
    for(unsigned lane=0;lane<3;++lane){
        std::memcpy(&position[lane],&f.nodes[0].localC0.v[12+lane],4);
        std::memcpy(&look[lane],&p.look[lane],4);std::memcpy(&up[lane],&p.up[lane],4);
    }
    //a3=1,t0=0,t1=0: right=up×look, correctedLook=right×up;
    //normalize both, reconstruct right. Existing finite/FE_TONEAREST native
    //adapter, not VU execution or early resource/observer readiness.
    at(0x289f98);g::BodyMatrix48G oriented;
    if(!g::originalUiUserRotatePose50KG(position,look,up,oriented))return false;
    at(0x28a02c);if(!g::setLocalMatrix49P(f.graph(),scene.model_.sceneIndex,oriented))return false;
    p.oriented=oriented;p.orientationReturned=true;at(0x28a084);at(0x27adf4);
    return true; //ONLY placement prefix.28E230 center/half and28E8A0 observer remain unjoined.
}
}
