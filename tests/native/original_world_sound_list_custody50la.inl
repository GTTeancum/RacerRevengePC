// Included by the Application-aware World TU. Friendship is deliberately held
// by this existing base owner, not inherited by the cold aggregate. These
// private leaves expose no authority/view and leave descriptor/order custody
// with the exact Ambient/SoulTree/MaterialSounds callers.
namespace ra_pod_scene50LA {
bool Owner::reserveNativeSoundListCustody()noexcept {
    return worldApplication_->objectCount423F60()!=std::numeric_limits<std::uint32_t>::max()&&
        worldApplication_->entityAuthority_.reserveAdditional(1);
}
bool Owner::canEnrollNativeSoundListCustody(const Entity& entity)const noexcept {
    return worldApplication_->entityAuthority_.canEnroll(entity,key(&entity));
}
void Owner::constructNativeSoundListCustody(Entity& entity)noexcept {
    if(g::initializeOriginalWreckComponent50LA(entity,g::OriginalWreckComponentKind50LA::entity141300,
       worldEntity_.sessionGeneration,key(&entity),true,worldApplication_->objectCount423F60()).status!=
       g::OriginalWreckComponentStatus50LA::complete||!worldApplication_->entityAuthority_.enroll(entity))
        std::terminate();
}
bool Owner::canRetireNativeSoundListCustody(const Entity& entity)const noexcept {
    return worldApplication_->entityAuthority_.canRetire(entity);
}
void Owner::retireNativeSoundListCustody(Entity& entity)noexcept {
    if(!worldApplication_->entityAuthority_.retire(entity))std::terminate();
    // Caller performs its existing reference/count decrements immediately next.
}
}
