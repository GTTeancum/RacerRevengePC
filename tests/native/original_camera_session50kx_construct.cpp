// Existing Session special member outlined intact; no new owner or traits.
#include "original_camera_session50kx.h"

namespace original_camera_session50KX {
Session::Session(ActualOwners actual) noexcept:actual_(actual){}
} // namespace original_camera_session50KX

#include "ra_fresh_execution_arena50la.h"
namespace original_camera_session50KX {
Session::Session(ActualOwners actual,
    const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>& fresh,
    std::shared_ptr<const OriginalRacerWorldBorrow50LA> world) noexcept
    :actual_(actual),constructorArena_(fresh),constructorWorld_(std::move(world)),constructorOnly_(true){}
bool Session::constructorArenaMatches(
    const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>& fresh,
    const Bank& bank,const VehicleFrameState49W& frame,const VehicleFrameBindings49W& bindings,
    const AiRuntimeState50IV& ai,const BasePoseHistoryState50Y& history,
    const std::array<std::unique_ptr<native_racer50HW::Rig>,8>& rigs) noexcept {
    return fresh&&fresh->matches(bank,frame,bindings,ai,history,rigs);
}
bool Session::admitConstructorOnly(
    const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>& fresh,
    std::uint64_t generation,DisplayInputs& display) noexcept {
    if(!onOwnerThread()||!fresh||!generation||!constructorOnly_||generation_||readsBound_||
       displayInputs_||coldBound_||closed_||abandoned_||nativeRaOwnedPreparationAttempted_||
       !constructorWorld_||!constructorWorld_->current()||constructorWorld_->generation()!=generation||
       nativeTaIssuance_||coldController_.started||actual_.followQueries.get()||actual_.actualCables||
       frameStage_!=FrameStage::idle||!actual_.actualRigs||
       !fresh->matches(actual_.bank,actual_.arena,actual_.bindings,actual_.ai,
           actual_.poseHistory,*actual_.actualRigs)||!actual_.applicationEntities||
       !actual_.applicationMaterials||!actual_.actualMaterialDirectory36B970)return false;
    // Publish native lifetime before any fallible read-handle admission. This
    // is not a source Session constructor nor camera/countdown readiness.
    constructorOnly_=true;constructorArena_=fresh;generation_=generation;displayInputs_=&display;
    try {
        auto& b=actual_.bank;
        reads_.model1B4=b.bindRead(actual_.bindings.lifecycle.model1B4);
        reads_.rightModel1B8=b.bindRead(actual_.bindings.visual.right1B8);
        reads_.leftModel1BC=b.bindRead(actual_.bindings.visual.left1BC);
        reads_.bottomRightModel1C0=b.bindRead(actual_.bindings.visual.bottomRight1C0);
        reads_.bottomLeftModel1C4=b.bindRead(actual_.bindings.visual.bottomLeft1C4);
        reads_.bottomRightPresent=b.bindRead(actual_.bindings.visual.extra1C0Present);
        reads_.bottomLeftPresent=b.bindRead(actual_.bindings.visual.extra1C4Present);
        for(std::size_t i=0;i<b.count();++i){
            if(!b.active(i)||!actual_.racer60[i])return false;
            identity_[i].managerIndex=static_cast<std::uint32_t>(i);
            identity_[i].pod60=actual_.racer60[i];
        }
        // readsBound_ deliberately remains false: AI/timing/phase and all
        // gameplay read handles have NOT been admitted or initialized.
        return true;
    }catch(...){frameStage_=FrameStage::failed;return false;}
}
bool Session::constructConstructorOnlyPrefixes() noexcept {
    const auto fresh=constructorArena_.lock();
    if(!onOwnerThread()||!constructorOnly_||constructorPrefixAttempted_||!fresh||
       closed_||abandoned_||frameStage_!=FrameStage::idle||actual_.actualCables||!actual_.actualRigs||
       !fresh->matches(actual_.bank,actual_.arena,actual_.bindings,actual_.ai,
           actual_.poseHistory,*actual_.actualRigs))return false;
    constructorPrefixAttempted_=true;
    //Retain the real wrapper arguments before entering any210770 prefix.
    if(!nativeWreckOwner_||!nativeWreckOwner_->bindFreshSourceIncomingZones(*this)){
        fail(Status::sourcePath,0x210770u);return false;
    }
    if(!constructRacerEntityPrefixes()||!constructRacerModelPrefixes()){
        fail(Status::sourcePath,result_.entityCold.sourcePC);return false;
    }
    constructorPrefixComplete_=true;
    // Only these represented Entity/model prefixes returned. No2146 return,
    // prepared348, source camera, controller, Follow, visibility or grid grant.
    set(Status::complete,result_.animationCold.sourcePC);return true;
}
Result Session::closeConstructorOnlyNative(bool abandon) noexcept {
    if(!onOwnerThread()||!constructorOnly_)return set(Status::owner);
    if(closed_)return set(abandon?Status::abandoned:Status::complete);
    const auto fresh=constructorArena_.lock();
    if(!fresh||!actual_.actualRigs||!fresh->matches(actual_.bank,actual_.arena,
       actual_.bindings,actual_.ai,actual_.poseHistory,*actual_.actualRigs)||
       actual_.followQueries.get()||actual_.actualCables||coldBound_||coldController_.started||
       spatial_.registration34.attached||pendingDisplayWork())return set(Status::owner);
    // Refusal retains all owners. Once cleanup starts no constructor replay.
    frameStage_=FrameStage::failed;abandoned_=abandoned_||abandon;
    if(nativeWreckOwner_&&!nativeWreckOwner_->withdrawFreshSourceIncomingZones(*this))
        return set(Status::owner,0x210770u);
    if(nativeWreckOwner_&&!nativeWreckOwner_->retireFreshSourceWreckConstructors(*this))
        return set(Status::owner,0x22bf50u);
    if(!releaseRacerEntityPrefixes())return set(Status::renderer,result_.entityCleanup.sourcePC);
    if(nativeWreckOwner_&&!nativeWreckOwner_->retireFreshSourceIncomingZones(*this))
        return set(Status::owner,0x210770u);
    // All actual Racer/model cleanup has succeeded. Only now may the World
    // borrower counter fall; partial cleanup retains this exact strong lease.
    constructorWorld_.reset();closed_=true;
    return set(abandon?Status::abandoned:Status::complete,0); //Native disposal, not1148D0.
}
} // namespace original_camera_session50KX
