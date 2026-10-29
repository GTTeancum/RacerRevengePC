#include "original_ui_garage_manager_search50kg.h"
namespace rr::platform {
OriginalUiGarageManagerSearch50KG::~OriginalUiGarageManagerSearch50KG()noexcept {
    if(bound_&&(!payloadReleased_||(grant_&&grant_->stillBorrowingManagerSearchOwner(this))))std::terminate();
    if(selectedBorrow_||nodeLive_||camera_||cameraObserver_)std::terminate();
}
bool OriginalUiGarageManagerSearch50KG::emptyFields()const noexcept {
    return !field30_&&!field3C_&&!field40_&&!field44_&&!field48_&&!fieldA0_&&!node_.firstChild18;
}
bool OriginalUiGarageManagerSearch50KG::current()const noexcept {
    if(!complete_||!bound_||!nodeLive_||nodeRetiring_||nodeRetired_||payloadReleased_||
       !grant_||!grant_->current()||grant_->application()!=application_||grant_->issuer()!=issuer_||
       !selected_||!selectedBorrow_||!selected_->managerSearchPredecessorCurrent(this,grant_)||
       selected_->model()->scene().modelEntity8()!=selectedEntity_||!list_||manager2A0_!=list_.get()||
       first34_!=list_.get()||second38_!=list_.get()||!emptyFields()||
       grant_->managerSearchNode2A4(this)!=&node_||node_.reference00!=1||
       (node_.next14&&(node_.next14->previous10!=&node_||node_.next14->parent1C!=grant_->manager()))||
       node_.parent1C!=grant_->manager()||node_.previous10!=selectedEntity_||node_.mask20!=2u)return false;
    return list_->managerHistoryCurrent(this,issuer_,application_)&&list_->complete_&&
        list_->managerAliases_==2&&!list_->managerReleasedAliases_&&list_->reference00_==3&&list_->usage10_==2;
}
OriginalUiGarageManagerSearch50KG::Observation OriginalUiGarageManagerSearch50KG::inspect()const noexcept {
    Observation o;o.node2A4=bound_&&grant_?grant_->managerSearchNode2A4(this):nullptr;
    o.selectedPredecessor=selectedEntity_;o.list2A0=manager2A0_;o.first34=first34_;o.second38=second38_;
    if(list_)o.list=list_->snapshot();
    o.attempted=attempted_;o.nodeLive=nodeLive_;
    o.nodeRetired=nodeRetired_;o.payloadReleased=payloadReleased_;o.complete=current();return o;
}
bool OriginalUiGarageManagerSearch50KG::prepare1CCD14(std::shared_ptr<Grant> grant,
    std::shared_ptr<OriginalUiGarageSelectedModel50KG> selected,Diagnostic& d)noexcept {
    d={};if(attempted_||bound_||payloadReleased_)return fail(d,Status::phase,0x1CCD14);
    if(!grant||!grant->current()||!grant->issuer()||!grant->application()||!selected||!selected->current())
        return fail(d,Status::input,0x1CCD14);
    auto* predecessor=selected->borrowManagerSearchPredecessor(this,grant);
    if(!predecessor)return fail(d,Status::selected,0x1D1E38);
    auto step=grant->bindManagerSearchPayload(this,*predecessor);
    if(!step.okay()){
        if(!selected->releaseManagerSearchPredecessor(this,grant))std::terminate();
        return fail(d,Status::scope,step.sourcePC);
    }
    attempted_=true;bound_=true;selectedBorrow_=true;grant_=std::move(grant);selected_=std::move(selected);
    selectedEntity_=predecessor;application_=grant_->application();issuer_=grant_->issuer();
    try{list_=std::unique_ptr<List>(new List(*issuer_,application_));}
    catch(...){return fail(d,Status::allocation,0x1CCD14);}
    list_->managerOwner_=this;List::D ld;
    const bool built=list_->construct27E260(issuer_,application_,ld);
    // The actual constructor retains its produced partial fields too. Manager
    // source2A0 publication precedes ALL node construction/alias operations.
    manager2A0_=list_.get();
    if(!built)return fail(d,Status::list,ld.sourcePC);
    step=grant_->constructManagerSearchNode(this,node_);
    if(!step.okay())return fail(d,Status::scope,step.sourcePC);
    nodeLive_=true;
    // Scope owns the actual2A4 store between constructor and source attachment.
    // These cold scalar fields are the explicit27F120 constructor zero prefix.
    if(!emptyFields()||grant_->managerSearchNode2A4(this)!=&node_)return fail(d,Status::phase,0x27F140);
    if(!list_->assignManagerAlias(this,issuer_,application_,1,first34_,ld))return fail(d,Status::list,ld.sourcePC);
    if(!list_->assignManagerAlias(this,issuer_,application_,2,second38_,ld))return fail(d,Status::list,ld.sourcePC);
    complete_=true;d={Status::complete,0x1CCD78};return true;
}
bool OriginalUiGarageManagerSearch50KG::retireNode27F180(Diagnostic& d)noexcept {
    d={};if(nodeRetired_)return true;
    if(camera_||cameraObserver_||!bound_||!grant_||!grant_->current()||payloadReleased_||!selected_||!selectedBorrow_||
       !selected_->managerSearchPredecessorCurrent(this,grant_)||!emptyFields())return fail(d,Status::phase,0x27F180);
    // Validate the exact produced alias prefix before unlink/ref0. Dying
    // fields remain nonnull after their releases, so released history matters.
    if(list_){
        if(manager2A0_!=list_.get()||!list_->managerHistoryCurrent(this,issuer_,application_)||
           first34_!=(list_->managerAliases_>=1?list_.get():nullptr)||
           second38_!=(list_->managerAliases_>=2?list_.get():nullptr)||
           (!nodeRetiring_&&list_->managerReleasedAliases_)||
           (!nodeLive_&&list_->managerAliases_)||
           (nodeLive_&&(!list_->complete_||!list_->selected1C_)))return fail(d,Status::list,0x27F180);
    }else if(manager2A0_||first34_||second38_||nodeLive_)return fail(d,Status::list,0x27F180);
    if(nodeLive_){
        if(!nodeRetiring_){
            const auto preflight=grant_->preflightManagerSearchNodeRetirement(this,node_);
            if(!preflight.okay())return fail(d,Status::scope,preflight.sourcePC);
            const auto begin=grant_->beginManagerSearchNodeRetirement(this,node_);
            if(!begin.okay())return fail(d,Status::scope,begin.sourcePC);
            nodeRetiring_=true;complete_=false;
        }
        List::D ld;
        //30 wrappers and3C are really absent. Drop34 then38 after ref0/unlink;
        // dying pointers and source usage10 remain unchanged.
        if(first34_&&list_->managerReleasedAliases_<1&&
           !list_->releaseManagerAlias(this,issuer_,application_,1,first34_,ld))return fail(d,Status::list,ld.sourcePC);
        if(second38_&&list_->managerReleasedAliases_<2&&
           !list_->releaseManagerAlias(this,issuer_,application_,2,second38_,ld))return fail(d,Status::list,ld.sourcePC);
        // A0 is genuinely null; Entity count decrement follows these payloads.
        const auto finish=grant_->finishManagerSearchNodeRetirement(this,node_);
        if(!finish.okay())return fail(d,Status::scope,finish.sourcePC);
        nodeLive_=false;nodeRetiring_=false;
    }else {
        const auto release=grant_->releaseUnconstructedSearchPredecessor(this);
        if(!release.okay())return fail(d,Status::scope,release.sourcePC);
    }
    if(!selected_->releaseManagerSearchPredecessor(this,grant_))return fail(d,Status::selected,0x27F23C);
    selectedBorrow_=false;selectedEntity_=nullptr;selected_.reset();nodeRetired_=true;complete_=false;
    d={Status::complete,0x27F23C};return true;
}
bool OriginalUiGarageManagerSearch50KG::retireManagerList1CCADC(Diagnostic& d)noexcept {
    d={};if(payloadReleased_)return true;
    if(!bound_||!nodeRetired_||nodeLive_||selectedBorrow_||!grant_||
       !grant_->managerPayloadRetirementCurrent(this))return fail(d,Status::phase,0x1CCADC);
    if(list_){List::D ld;
        if(manager2A0_!=list_.get()||!list_->releaseManagerPayload(this,issuer_,application_,ld))return fail(d,Status::list,ld.sourcePC);
    }else if(manager2A0_)return fail(d,Status::phase,0x1CCADC);
    const auto note=grant_->noteManagerSearchPayloadReleased(this);
    if(!note.okay())std::terminate(); // Prevalidated same-thread typed pending owner.
    payloadReleased_=true;d={Status::complete,0x1CCADC};return true;
}
bool OriginalUiGarageManagerSearch50KG::cameraInputCurrent(
    const std::shared_ptr<game::OriginalFrontendCameraCold50LA>& camera)const noexcept {
    using Camera=game::OriginalFrontendCameraCold50LA;
    return camera&&camera->nativeRegistrationIdentityCurrent()&&
        camera->issuer_==issuer_&&camera->entities_==application_&&camera->entityConstructed_&&
        camera->fieldsComplete_&&camera->enrolled_&&
        (camera->phase_==Camera::Phase::viewportInitialized2361C0||camera->phase_==Camera::Phase::poseSet114F90)&&
        camera->entity_.kind==game::OriginalWreckComponentKind50LA::camera236180&&
        camera->entity_.reference00==1&&camera->entity_.sessionGeneration==camera->generation_&&
        camera->entity_.objectKey==camera->key_;
}
bool OriginalUiGarageManagerSearch50KG::cameraRegistrationCurrent()const noexcept {
    if(!current()||!cameraInputCurrent(camera_)||!cameraObserver_)return false;
    const auto& o=*cameraObserver_;auto* shared=grant_->observerShared();
    return o.sharedConstructed&&o.added&&o.complete&&shared&&shared->owns(&o)&&
        !o.next00&&o.child04==&node_&&o.method08==list_.get()&&!o.zero0C&&!o.zero10&&
        camera_->fields_.registration34==&o&&camera_->fields_.parent38==&node_&&
        list_->managerCameraCurrent(this,issuer_,application_,camera_->entity_,o.slot14);
}
bool OriginalUiGarageManagerSearch50KG::registerCamera114770(
    std::shared_ptr<game::OriginalFrontendCameraCold50LA> camera,Diagnostic& d)noexcept {
    d={};if(!current()||!cameraInputCurrent(camera))return fail(d,Status::phase,0x114770);
    if(camera_&&camera_!=camera)return fail(d,Status::input,0x114770);
    if(cameraObserver_&&cameraObserver_->complete){
        if(!cameraRegistrationCurrent())return fail(d,Status::phase,0x1148B4);
        d={Status::complete,0x1148B4};return true;
    }
    auto* shared=grant_->observerShared();if(!shared)return fail(d,Status::scope,0x280890);
    if(!cameraObserver_){
        if(camera->phase_!=game::OriginalFrontendCameraCold50LA::Phase::viewportInitialized2361C0)
            return fail(d,Status::phase,0x114770);
        if(camera->fields_.registration34||camera->fields_.parent38)return fail(d,Status::phase,0x1147A8);
        try{cameraObserver_=std::make_unique<CameraObserver>();}catch(...){return fail(d,Status::allocation,0x114790);}
        camera_=std::move(camera);
    }
    auto& o=*cameraObserver_;
    if(!o.sharedConstructed){
        if(camera_->fields_.registration34||camera_->fields_.parent38||o.added||o.child04||o.method08||o.slot14)
            return fail(d,Status::phase,0x280890);
        if(!shared->construct280890(&o))return fail(d,Status::allocation,0x280890);
        o.sharedConstructed=true;
        camera_->fields_.registration34=&o; //1147A8, only after actual constructor.
    }
    if(!game::OriginalCameraObserverAssertions50KG::allocation1147E4(camera_->fields_.registration34!=nullptr))
        return fail(d,Status::phase,0x1147E4);
    if(!shared->owns(&o)||camera_->fields_.registration34!=&o||o.next00||o.zero0C||o.zero10)
        return fail(d,Status::phase,0x114818);
    if(camera_->fields_.parent38&&camera_->fields_.parent38!=&node_)return fail(d,Status::phase,0x11481C);
    camera_->fields_.parent38=&node_; //11481C before2809C0, not an Entity attach.
    if(!o.added){
        if(o.child04||o.method08||o.slot14)return fail(d,Status::phase,0x280A2C);
        List::D ld;
        //27F590(mode2) selects38, which is this exact same manager List.
        if(!list_->insertManagerCamera(this,issuer_,application_,camera_->entity_,o.slot14,ld))
            return fail(d,Status::list,ld.sourcePC);
        o.child04=&node_;o.method08=list_.get();o.next00=nullptr;
        o.previousPosition20=camera_->fields_.position180;o.added=true;
    }
    //280BF0 sees the actual node30==null and returns at280D08. There are no
    // portal-link allocations or secondary observer records in this owner.
    if(field30_)return fail(d,Status::phase,0x280C34);
    o.complete=true;
    if(!game::OriginalCameraObserverAssertions50KG::membership114884(o.child04!=nullptr))
        return fail(d,Status::phase,0x114884);
    if(!cameraRegistrationCurrent())return fail(d,Status::phase,0x1148B4);
    d={Status::complete,0x1148B4};return true;
}
bool OriginalUiGarageManagerSearch50KG::unregisterCamera1148D0(
    const std::shared_ptr<game::OriginalFrontendCameraCold50LA>& camera,Diagnostic& d)noexcept {
    d={};if(!camera||camera!=camera_||!cameraRegistrationCurrent())return fail(d,Status::phase,0x1148D0);
    auto* shared=grant_->observerShared();auto& o=*cameraObserver_;
    //281490 removes the exact stored slot. No portal links exist. The source
    //1148D0 does not clear parent38, so native retirement does not invent it.
    List::D ld;if(!list_->removeManagerCamera(this,issuer_,application_,camera_->entity_,o.slot14,ld))
        return fail(d,Status::list,ld.sourcePC);
    if(!shared->release280920(&o))std::terminate(); // Identity preflight above.
    camera_->fields_.registration34=nullptr; //11490C follows observer destruction.
    cameraObserver_.reset();camera_.reset();d={Status::complete,0x114910};return true;
}

}
