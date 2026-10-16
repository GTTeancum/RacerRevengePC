#pragma once
#include "original_race_display_lifetime50la.h"
#include "ra_source_camera_ownership50la.h"
#include "native_racer_cables50hw_fixture.h"
#include "../../src/game/original_pod_cable_allocation50la.h"
#include "../../src/game/original_zone_scissor50la.h"
#include <list>
#include <thread>
#include <limits>
#include <vector>
namespace original_main_entry50KG {
// Inactive native443848 semantic FIFO. The actual Application owns exactly
//one active process-slot instance. Source race teardown clears that slot; a
//later genuine constructor may create its successor. No synthetic frame/drain.
class NativeCollectorFifo50LA final {
    friend struct Application;
    friend class ::original_cable_selection50LA::Binding;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    friend class rr::platform::VehicleWindowGl50DS;
    using Table=rr::assets::OriginalStandardMaterialTable50LA;
    using Binding=rr::assets::OriginalCableEmissionBinding50LA;
    using Allocation=rr::game::OriginalPodCableAllocation50LA;
    using Bridge=ra_source_camera50LA::OwnershipBridge;
    using Visibility=rr::game::original_gc_gameplay_lighting_candidate::CameraVisibilityLease;
    using Zone=rr::game::OriginalZoneScissorPublication50LA;
    using Pin=RaceDisplayLifetime50LA::CableInspection;
    enum class Status {owner,allocation,scene,stale,acceptedCpuOnly,loweringRequired};
    struct Result {Status status=Status::owner;std::uint32_t sourcePC=0;bool sourceEffectsCommitted=false;};
    struct Record {
        Record* next=nullptr;
        std::shared_ptr<Bridge> camera; // Must outlive allocation destruction.
        std::shared_ptr<Allocation> allocation;
        std::uint32_t strip=0;
        native_racer50HN::Bank* bank=nullptr;
        native_racer50HW::Cables* cables=nullptr;
        std::shared_ptr<rr::game::OriginalZoneScissor50LA> scissor18;
        std::unique_ptr<Binding> material;
        std::unique_ptr<Pin> display;
        std::uint32_t epoch=0;
        // Actual common/attributes stay inside allocation; origin parameters
        // and animation stay in material. No snapshots replace mutable owners.
    };
    struct Leaf {Record* head=nullptr;Record* tail=nullptr;std::uint32_t epoch=0;};
    const Application* application_=nullptr;
    std::shared_ptr<Table> table_;
    rr::assets::OriginalStandardMaterialCollectorReceipt50LA registry_;
    std::uint32_t epoch04_=1; // Actual29C2C4/C8 fresh constructor, NOT cold generation.
    Zone scissor4014_; // Actual slot; value_ starts null at24B704.
    std::list<Record> records_; // Stable native nodes, source insertion order.
    std::vector<Leaf> leaves_; // Actual registration ordinals; redirects share final leaf.
    bool draining_=false,closed_=false;
    const std::thread::id ownerThread_=std::this_thread::get_id();
    NativeCollectorFifo50LA(const Application& app,std::shared_ptr<Table> table)
        :application_(&app),table_(std::move(table)),scissor4014_(app.entitiesOwner_.get()){}
    static std::shared_ptr<NativeCollectorFifo50LA> actualApplication(Pin& pin,
        const std::shared_ptr<Table>& table) {
        if(!pin.owner_)return {};
        const auto& display=*pin.owner_;const auto* app=display.application_;
        if(!app||!display.canRetain()||!app->standardMaterialDomainOwner_||
           app->standardMaterialDomainOwner_->materials382B38()!=table)return {};
        if(!app->nativeCollectorFifoOwner_)
            app->nativeCollectorFifoOwner_=std::shared_ptr<NativeCollectorFifo50LA>(new NativeCollectorFifo50LA(*app,table));
        const auto& fifo=app->nativeCollectorFifoOwner_;
        return !fifo->closed_&&fifo->application_==app&&fifo->table_==table?fifo:nullptr;
    }
    const rr::game::OriginalApplicationEntities50LA* applicationEntities()const noexcept {
        return application_?application_->entitiesOwner_.get():nullptr;
    }
    Result accept24B9B0(std::shared_ptr<Allocation> allocation,std::uint32_t strip,
        std::shared_ptr<Bridge> camera,native_racer50HW::Cables& cables,std::unique_ptr<Binding> binding,
        std::unique_ptr<Pin> display,const Zone& zone,const Visibility& visibility)noexcept {
        if(closed_||draining_||!allocation||!camera||!binding||!binding->context_||!display||!display->owner_||
           camera->closed_||camera->nativeFifoPins_==std::numeric_limits<std::uint32_t>::max()||
           display->owner_->application_!=application_||!display->owner_->canRetain()||
           binding->context_->videoIdentity()!=display->owner_->video_.get()||
           !table_->coldCollectorCurrent(binding->registry_)||allocation->closed_||
           strip>=allocation->ownedStrips_||!allocation->strips30_[strip]||
           allocation->fifoPins_==std::numeric_limits<std::uint32_t>::max())return {Status::owner,0x24b9b0u};
        if(&zone!=&scissor4014_||zone.owner_!=visibility.owner_||!zone.value_||
           zone.application_!=application_->entitiesOwner_.get()||
           zone.cameraGeneration_!=visibility.generation_||!visibility.racerVisibility7F0()||
           camera->freshLighting_.get()!=zone.owner_)return {Status::scene,0x24b9f8u};
        // Same externally owned Bank and persistent Cables, not bridge-only
        // lifetime. Their destructor-entry guards precede all backing release.
        auto* bank=const_cast<native_racer50HN::Bank*>(camera->bankIdentity_);
        const auto bankPin=camera->bankLifetime_.lock();
        if(!bank||!bankPin||bankPin->identity!=bank||bank->raWorldLifetime_!=bankPin||
           bank->nativeFifoPins_==std::numeric_limits<std::uint32_t>::max()||
           cables.nativeFifoPins_==std::numeric_limits<std::uint32_t>::max()||
           cables.selectionState_.allocation_!=allocation)
            return {Status::owner,0x24b9b0u};
        const auto& primitive=allocation->strips30_[strip]->primitive30_;
        if(!primitive||!primitive->reference.retained_||primitive->reference.released_||
           primitive->reference.table_!=table_||
           primitive->reference.material00_!=binding->origin_.materialRow)
            return {Status::stale,0x24b9b0u};
        const auto leaf=binding->representative_.registration;
        const auto count=table_->coldCollectorCount(binding->registry_);
        if(leaf<0||static_cast<std::size_t>(leaf)>=count)return {Status::stale,0x29c160u};
        if(!records_.empty()&&!table_->coldCollectorCurrent(registry_))return {Status::stale,0x29c160u};
        bool tablePinned=false;
        try {
            if(records_.empty()) {leaves_.assign(count,Leaf{});registry_=binding->registry_;}
            if(leaves_.size()!=count||!table_->pinNativeCollector(binding->registry_))return {Status::stale,0x29c150u};
            tablePinned=true;
            records_.emplace_back(); // Native allocation; no logical source append on failure.
            auto& record=records_.back();auto& destination=leaves_[static_cast<std::size_t>(leaf)];
            ++allocation->fifoPins_;++camera->nativeFifoPins_;
            ++bank->nativeFifoPins_;++cables.nativeFifoPins_;
            record.bank=bank;record.cables=&cables;
            record.allocation=std::move(allocation);record.strip=strip;
            record.camera=std::move(camera);record.material=std::move(binding);record.display=std::move(display);
            record.epoch=epoch04_;
            //29C218/21C then epoch/head/tail stores, followed by24B9E4..24BA24.
            if(destination.epoch!=epoch04_){destination.epoch=epoch04_;destination.head=&record;destination.tail=&record;}
            else {destination.tail->next=&record;destination.tail=&record;}
            record.scissor18=scissor4014_.value_;
            return {Status::acceptedCpuOnly,0x24ba28u,true};
        }catch(...){if(tablePinned)table_->unpinNativeCollector();return {Status::allocation,0x105720u};}
    }
    bool retire24B720(const Application&,rr::assets::OriginalStandardMaterialDiagnostic50LA&)noexcept;
    Result drain24B7B0()noexcept {
        if(closed_)return {Status::owner,0x24b7b0u};
        // Even an empty source drain performs renderer/global work before
        //105990 and epoch increment. No glFinish/empty-queue substitute.
        // The real lowering has no implementation/receipt producer yet.
        return {Status::loweringRequired,0x24b870u};
    }
    // Sealed eventual backend completion, not a caller callback/Boolean. The
    // real VehicleWindow implementation is the only possible issuer, and no
    // issuer is implemented here. A mere context finish cannot create it.
    class CompletedLowering final {
        friend class rr::platform::VehicleWindowGl50DS;
        friend class NativeCollectorFifo50LA;
        const NativeCollectorFifo50LA* owner_;
        std::uint32_t epoch_;
        std::size_t count_;
        CompletedLowering(const NativeCollectorFifo50LA& owner)
            :owner_(&owner),epoch_(owner.epoch04_),count_(owner.records_.size()){}
    };
    bool commitActualDrain(const CompletedLowering& done)noexcept {
        if(closed_||!draining_||done.owner_!=this||done.epoch_!=epoch04_||done.count_!=records_.size())return false;
        //105990 belongs AFTER all actual source drain work. Resources survive
        //every unsupported/failed drain; this is its only successful release.
        for(auto& record:records_){
            if(!record.allocation->fifoPins_)std::terminate();
            --record.allocation->fifoPins_;
            if(!record.camera->nativeFifoPins_)std::terminate();
            --record.camera->nativeFifoPins_;
            if(!record.bank->nativeFifoPins_||!record.cables->nativeFifoPins_)std::terminate();
            --record.bank->nativeFifoPins_;--record.cables->nativeFifoPins_;
            table_->unpinNativeCollector();
        }
        records_.clear();leaves_.clear();++epoch04_; //24B954 uint32 wrap, not host frame.
        draining_=false;return true;
    }
public:
    NativeCollectorFifo50LA(const NativeCollectorFifo50LA&)=delete;
    NativeCollectorFifo50LA& operator=(const NativeCollectorFifo50LA&)=delete;
    ~NativeCollectorFifo50LA()noexcept {
        if(!records_.empty())std::terminate(); // No implicit failed-queue discard.
    }
};
inline bool NativeCollectorFifo50LA::retire24B720(const Application& app,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
    using S=rr::assets::OriginalStandardMaterialStatus50LA;
    d={};
    if(application_!=&app||app.nativeCollectorFifoOwner_.get()!=this||closed_||draining_||
       ownerThread_!=std::this_thread::get_id()||!records_.empty()||
       !table_||!app.standardMaterialDomainOwner_||
       app.standardMaterialDomainOwner_->materials382B38()!=table_||
       !table_->canRetireNativeCollector29C300()){
        d.status=S::phase;d.sourcePC=0x1a6948u;return false;
    }
    // No native4010 lowering allocation exists in this implementation. Future
    // real lowering must retire its actual backing BEFORE the base destructor.
    // Allocator08 has no live accepted records; no drain/105990/epoch advance
    // is synthesized. Every rejection above preserves all dependent lifetimes.
    closed_=true;
    //29C354/360 clears443848 iff this is the actual registered instance.
    // Application entry retains this storage through registration cleanup.
    app.nativeCollectorFifoOwner_.reset();
    table_->retireNativeCollector29C300();
    leaves_.clear();registry_={};
    scissor4014_.value_.reset();scissor4014_.owner_=nullptr;
    scissor4014_.cameraGeneration_=0;
    table_.reset();
    d.sourceEffectsCommitted=true;d.sourcePC=0x24b778u;return true;
}
inline bool Application::retireNativeCollector1A6948(
    rr::assets::OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
    d={};const auto fifo=nativeCollectorFifoOwner_;
    if(!fifo)return true; //Actual1A6930/38 null branch: no destructor reached.
    return fifo->retire24B720(*this,d);
}
}

namespace ra_source_camera50LA {
inline bool OwnershipBridge::drawZonePrefix114E00(const ZoneDrawEntry114E00& entry,Diagnostic& d)noexcept {
    d={};StaticFogPublication id;ra_static_fog50LA::CameraObservation observation;
    if(entry.owner_!=this||!entry.visibility_||!entry.collector_||
       !readStaticFogPublication(id,observation)||!camera_||!camera_->session){
        d.message="actual114D50 draw entry and retained camera required";return false;
    }
    auto& session=*camera_->session;auto& fifo=*entry.collector_;
    // The actual factory-associated Application, canonical materials and same
    // selected SourceCamera own this store. No supplied replacement4014 slot.
    if(!session.actual_.applicationEntities||!session.actual_.applicationMaterials||
       !fifo.application_||fifo.closed_||fifo.draining_||
       fifo.applicationEntities()!=session.actual_.applicationEntities.get()||
       fifo.table_!=session.actual_.applicationMaterials||
       &session.actual_.visibility!=freshLighting_.get()||
       session.display_.rendererCurrentScene0!=&session.viewport_||
       !session.spatial_.registration34.attached||!session.spatial_.parent38||
       !session.visibilityKeys_.matches(*freshLighting_,*entry.visibility_,session.generation_,session.key(session.scene_))){
        d.message="draw entry no longer matches actual Application/camera/collector";return false;
    }
    g::original_gc_gameplay_lighting_candidate::Diagnostic detail;
    const bool complete=freshLighting_->drawRaZonePrefix27FEE0(*entry.visibility_,
        *session.actual_.applicationEntities,session.spatial_.parent38,session.viewport_,fifo.scissor4014_,detail);
    d.camera.sourcePC=detail.sourcePC;
    d.message="private zone draw prefix only; camera drain and child tail remain unjoined";
    return complete;
}
}
