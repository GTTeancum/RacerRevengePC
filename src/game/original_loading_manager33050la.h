#pragma once
#include "original_frontend_camera_cold50la.h"
#include "../../platform/linux/native/original_ui_search_list_cold50kg.h"

namespace rr::game {
// Actual Application custody. Genuine frontend entry may finish17BF90;
// native direct gameplay remains at its bounded before114A00 prefix.
// No live330 dispatch, loading screen40 or ready predicate.
class OriginalLoadingManager33050LA final {
    friend struct ::original_main_entry50KG::Application;
    friend class ::original_main_entry50KG::RaceDisplayLifetime50LA;
    using App=::original_main_entry50KG::Application;
    using Entities=OriginalApplicationEntities50LA;
    using C=OriginalWreckComponent50LA;
    using List=rr::platform::OriginalUiSearchListCold50KG;
    using Camera=OriginalFrontendCameraCold50LA;
    const App* const issuer_;
    const std::shared_ptr<Entities> entities_;
    C manager_,node_;
    std::uint32_t managerVtable_=0,nodeVtable_=0;
    std::uint32_t mode30_=0,pending34_=0,shown38_=0,field3C_=0,state50_=0;
    const void* screen40_=nullptr;
    std::optional<std::uint32_t> mode54_; //Unknown until actual17C590 store.
    // Other source-unwritten fields remain absent.
    const void* node30_=nullptr;List* node34_=nullptr;List* node38_=nullptr;
    const void* node48_=nullptr;const void* node3C_=nullptr;
    const void* node40_=nullptr;const void* node44_=nullptr;const void* nodeA0_=nullptr;
    std::unique_ptr<List> list_;List* manager4C_=nullptr;
    std::shared_ptr<Camera> camera_;Camera* manager44_=nullptr;
    bool managerConstructed_=false,slot330Published_=false,initializationAttempted_=false;
    bool nodeConstructed_=false,nodeAttached_=false,boundary114A00_=false;
    bool retiring_=false,retired_=false,managerListReleased_=false,nodeRetired_=false;
    std::uint8_t aliases_=0,releasedAliases_=0;
    // One actual frontend renderer invocation; failures are terminal/nonreplaying.
    bool viewportAttempted_=false,viewportCompleted_=false;
    bool suffixAttempted_=false,managerInitialized_=false,cameraAttached_=false,managerAttached_=false;
    std::uint8_t constructorCallbacks_=0;
    std::uint8_t viewportServicePhase_=0;
    std::weak_ptr<::original_main_entry50KG::RaceDisplayLifetime50LA> viewportDisplay_;
    std::uint32_t stopPC_=0x17cde4;
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    bool current()const noexcept {
        return issuer_&&entities_&&entities_->nativeOwnerThread_==std::this_thread::get_id()&&
            entities_->application10()&&!entities_->attachmentLock423F58();
    }
    bool constructEntity(C& c,OriginalWreckComponentKind50LA kind,bool active)noexcept {
        auto& authority=entities_->entityAuthority_;
        if(entities_->objectCount423F60_==std::numeric_limits<std::uint32_t>::max()||
           !authority.reserveAdditional(1)||!authority.canEnroll(c,key(&c)))return false;
        const auto r=initializeOriginalWreckComponent50LA(c,kind,key(entities_.get()),key(&c),active,
            entities_->objectCount423F60_);
        if(r.status!=OriginalWreckComponentStatus50LA::complete||!authority.enroll(c))std::terminate();
        return true;
    }
    OriginalLoadingManager33050LA(const App* app,std::shared_ptr<Entities> entities)noexcept
        :issuer_(app),entities_(std::move(entities)){}
    bool construct17CDF8()noexcept {
        if(!current()||managerConstructed_||retiring_||retired_)return false;
        if(!constructEntity(manager_,OriginalWreckComponentKind50LA::entity141300,false))return false;
        managerConstructed_=true;managerVtable_=0x3ce520u; //17CE08.
        return true; //The Application publishes this retained owner next, BEFORE17BF90.
    }
    bool prepare17BF90(std::uint32_t& pc)noexcept {
        pc=stopPC_;
        if(!current()||!managerConstructed_||!slot330Published_||initializationAttempted_||
           retiring_||retired_||manager_.reference00!=1||manager_.parent1C||manager_.firstChild18)return false;
        initializationAttempted_=true;
        pending34_=0;shown38_=0;field3C_=0;mode30_=0;state50_=0xffffffffu;screen40_=nullptr;
        stopPC_=pc=0x17bfbc;
        try {list_.reset(new List(*issuer_,entities_));}catch(...){return false;}
        list_->loadingManagerOwner_=this;
        List::D diagnostic;
        const bool listComplete=list_->construct27E260(issuer_,entities_,diagnostic);
        manager4C_=list_.get(); //17BFE0, retains even a rejected partial constructor.
        if(!listComplete){stopPC_=pc=diagnostic.sourcePC;return false;}
        stopPC_=pc=0x17bfec;
        if(!constructEntity(node_,OriginalWreckComponentKind50LA::searchNode27F120,
                            (manager_.flags29&1u)!=0))return false;
        nodeConstructed_=true;nodeVtable_=0x3d2dd0u;
        node30_=nullptr;node34_=nullptr;node38_=nullptr;node48_=nullptr;
        node3C_=nullptr;node40_=nullptr;node44_=nullptr;nodeA0_=nullptr; //27F140..15C.
        //17BFFC publishes SAME node48 before141960. No alias or Camera yet.
        stopPC_=pc=0x17c008;
        const auto attached=entities_->entityAuthority_.attachChildren(manager_,node_,0u,entities_->attachmentLock423F58());
        if(attached.status!=OriginalWreckComponentStatus50LA::complete)return false;
        nodeAttached_=true;
        //27F420/27F490 on genuine constructor-null old pointers: incoming
        //usage increment, actual field store, then reference retain. List4C
        //and both node aliases refer to the ONE real List, not garage grants.
        ++list_->usage10_;node34_=list_.get();++list_->reference00_;++aliases_;
        ++list_->usage10_;node38_=list_.get();++list_->reference00_;++aliases_;
        stopPC_=pc=0x17c028;
        auto cameraResult=Camera::construct(issuer_,entities_,camera_);
        manager44_=camera_.get(); //17C044, only the actual produced owner.
        if(!cameraResult.okay()){stopPC_=pc=cameraResult.sourcePC;return false;}
        stopPC_=pc=0x17c05c;
        cameraResult=camera_->stageViewportPrefix1149D4(issuer_,entities_);
        if(!cameraResult.okay()){stopPC_=pc=cameraResult.sourcePC;return false;}
        boundary114A00_=true;stopPC_=pc=0x114a00;
        //17C06C Camera attach and17CE2C manager attach are UNREACHED.
        //No renderer drain, callback, selection, screen40 or progress service.
        return true; //Only native prefix custody, never original17BF90 return.
    }
    bool prefixRetained()const noexcept {
        if(!current()||retiring_||retired_||!slot330Published_||!managerConstructed_||
           !nodeConstructed_||!nodeAttached_||aliases_!=2||releasedAliases_||
           manager_.reference00!=1||manager_.firstChild18!=&node_||
           node_.parent1C!=&manager_||node_.previous10||node_.reference00!=1||
           manager4C_!=list_.get()||!list_||list_->loadingManagerOwner_!=this||
           node34_!=list_.get()||node38_!=list_.get()||list_->reference00_!=3||list_->usage10_!=2||
           manager44_!=camera_.get()||!camera_)return false;
        if(suffixAttempted_){
            return managerInitialized_&&managerAttached_&&cameraAttached_&&viewportCompleted_&&
                constructorCallbacks_==2&&manager_.parent1C==entities_->application10()&&
                node_.next14==&camera_->entity_&&camera_->entity_.previous10==&node_&&
                camera_->entity_.parent1C==&manager_&&!camera_->entity_.next14&&
                !(manager_.flags29&3u)&&!(node_.flags29&3u)&&!(camera_->entity_.flags29&3u)&&
                camera_->phase_==Camera::Phase::viewportInitialized2361C0;
        }
        return !manager_.parent1C&&!node_.next14&&!cameraAttached_&&!managerAttached_&&
            ((!viewportAttempted_&&boundary114A00_&&camera_->phase_==Camera::Phase::beforeViewport114A00)||
             (viewportCompleted_&&viewportServicePhase_==2&&camera_->phase_==Camera::Phase::afterViewport114A00));
    }
    bool stageMode0Activity17C580(std::uint32_t& pc)noexcept {
        pc=0x17c580;
        if(!prefixRetained()||!managerInitialized_||mode54_||pending34_||shown38_||screen40_||
           node_.firstChild18||camera_->entity_.firstChild18)return false;
        mode54_=0u; //17C590, BEFORE actual virtual24/141630 activity.
        manager_.flags29=static_cast<std::uint8_t>(manager_.flags29|3u);
        // Exact constructor-owned two-child topology.141630 skips children
        // with bit3 set; no unknown virtual callback or new node is admitted.
        if(!(node_.flags29&8u))node_.flags29=static_cast<std::uint8_t>(node_.flags29|3u);
        if(!(camera_->entity_.flags29&8u))camera_->entity_.flags29=static_cast<std::uint8_t>(camera_->entity_.flags29|3u);
        pc=0x17c5b0;return true;
    }
    bool retireNativePrefix()noexcept {
        if(!current())return false;
        if(retired_)return true;
        retiring_=true;boundary114A00_=false;
        // Explicit native custody unwind, not a completed-source destructor.
        // Camera may now be selected/attached; Application first withdraws
        // only its actual selection borrow and rejects all outside borrowers.
        // No loading Camera observer, renderer resource or live update was installed.
        if(camera_){
            if(manager44_!=camera_.get()||camera_.use_count()!=1)return false;
            if(cameraAttached_){
                auto& e=camera_->entity_;
                if(e.parent1C!=&manager_||e.previous10!=&node_||e.next14||e.firstChild18||
                   e.reference00!=1||e.kind!=OriginalWreckComponentKind50LA::camera236180||
                   camera_->fields_.registration34||camera_->fields_.parent38||camera_->fields_.resource224||
                   !camera_->enrolled_||!entities_->entityAuthority_.canRetire(e))return false;
                const auto detached=entities_->entityAuthority_.detach(e,entities_->attachmentLock423F58());
                if(detached.status!=OriginalWreckComponentStatus50LA::complete)return false;
                cameraAttached_=false;
                // Explicit native shutdown of this exact attached-but-never-
                //dispatched loading Camera. No generic cold admission widened.
                if(!entities_->entityAuthority_.retire(e))std::terminate();
                camera_->enrolled_=false;--e.reference00;--entities_->objectCount423F60_;
                camera_->phase_=Camera::Phase::nativeUnpublishedAborted;
            }else if(!camera_->abortUnpublished236180(issuer_,entities_,suffixAttempted_).okay())return false;
            manager44_=nullptr;camera_.reset();
        }
        if(list_){
            if(manager4C_!=list_.get()||list_->loadingManagerOwner_!=this||list_->managerOwner_||
               list_->lightManagerOwner_||list_->storage_.blockCount||list_->selectedCount18_||
               list_->usage10_!=aliases_||releasedAliases_>aliases_||
               list_->reference00_!=(managerListReleased_?0u:1u)+aliases_-releasedAliases_)return false;
            //NATIVE SAFE UNWIND ONLY: drop this owner reference, retaining
            //the List until both node aliases are retired. Original17BF40
            //dispatches +08 DIRECT DELETE (27E2C0), NOT104970. We do not
            //invoke that completed-manager destructor on this partial graph.
            if(!managerListReleased_){--list_->reference00_;managerListReleased_=true;}
        }else if(manager4C_||(aliases_&&!nodeRetired_))return false;
        if(nodeConstructed_&&!nodeRetired_){
            if(node_.firstChild18||node30_||node48_||node3C_||node40_||node44_||nodeA0_||
               node34_!=(aliases_>=1?list_.get():nullptr)||node38_!=(aliases_>=2?list_.get():nullptr)||
               !entities_->entityAuthority_.canRetire(node_))return false;
            if(nodeAttached_){
                if(node_.parent1C!=&manager_||manager_.firstChild18!=&node_||node_.previous10||node_.next14)return false;
                const auto detached=entities_->entityAuthority_.detach(node_,entities_->attachmentLock423F58());
                if(detached.status!=OriginalWreckComponentStatus50LA::complete)return false;
                nodeAttached_=false;
            }
            if(!entities_->entityAuthority_.retire(node_))std::terminate();
            --node_.reference00_;nodeVtable_=0x3d2dd0u;
            //27F200/21C drops aliases in order; dying source pointers/usage
            //are not reset. Final release frees the actual selected backing.
            while(releasedAliases_<aliases_){--list_->reference00_;++releasedAliases_;}
            --entities_->objectCount423F60_;nodeRetired_=true;
        }
        if(list_){
            if(list_->reference00_)return false;
            list_->selected1C_.reset();list_->released_=true;list_->complete_=false;
            list_->loadingManagerOwner_=nullptr;manager4C_=nullptr;list_.reset();
            // The source dying aliases remain nonnull; no future read is admitted.
        }
        if(managerConstructed_){
            if(managerAttached_){
                if(manager_.parent1C!=entities_->application10())return false;
                const auto detached=entities_->entityAuthority_.detach(manager_,entities_->attachmentLock423F58());
                if(detached.status!=OriginalWreckComponentStatus50LA::complete)return false;
                managerAttached_=false;
            }
            if(manager_.firstChild18||manager_.parent1C||manager_.previous10||manager_.next14||
               !entities_->entityAuthority_.canRetire(manager_))return false;
            if(!entities_->entityAuthority_.retire(manager_))std::terminate();
            --manager_.reference00_;managerVtable_=0x3cd240u;
            --entities_->objectCount423F60_;managerVtable_=0x3cc980u;
        }
        slot330Published_=false;retired_=true;return true;
    }
public:
    ~OriginalLoadingManager33050LA(){if(managerConstructed_&&!retired_)std::terminate();}
    OriginalLoadingManager33050LA(const OriginalLoadingManager33050LA&)=delete;
    OriginalLoadingManager33050LA& operator=(const OriginalLoadingManager33050LA&)=delete;
};
}
