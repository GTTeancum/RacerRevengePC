#pragma once
#include "original_race_display_lifetime50la.h"
#include "ra_source_camera_ownership50la.h"
#include "native_racer_cables50hw_fixture.h"
#include "../../src/game/original_pod_cable_allocation50la.h"
#include "../../src/game/original_zone_scissor50la.h"
#include <list>
#include <limits>
#include <vector>
namespace original_main_entry50KG {
// Inactive native443848 semantic FIFO. The actual Application owns exactly
//one process instance. No source command words, synthetic frame or drain.
class NativeCollectorFifo50LA final {
    friend class ::original_cable_selection50LA::Binding;
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
    std::shared_ptr<rr::game::OriginalZoneScissor50LA> scissor4014_; //24B704 null.
    std::list<Record> records_; // Stable native nodes, source insertion order.
    std::vector<Leaf> leaves_; // Actual registration ordinals; redirects share final leaf.
    bool draining_=false;
    NativeCollectorFifo50LA(const Application& app,std::shared_ptr<Table> table)
        :application_(&app),table_(std::move(table)){}
    static std::shared_ptr<NativeCollectorFifo50LA> actualApplication(Pin& pin,
        const std::shared_ptr<Table>& table) {
        if(!pin.owner_)return {};
        const auto& display=*pin.owner_;const auto* app=display.application_;
        if(!app||!display.canRetain()||!app->standardMaterialDomainOwner_||
           app->standardMaterialDomainOwner_->materials382B38()!=table)return {};
        if(!app->nativeCollectorFifoOwner_)
            app->nativeCollectorFifoOwner_=std::shared_ptr<NativeCollectorFifo50LA>(new NativeCollectorFifo50LA(*app,table));
        const auto& fifo=app->nativeCollectorFifoOwner_;
        return fifo->application_==app&&fifo->table_==table?fifo:nullptr;
    }
    Result bindZone280118(const Zone& zone,const Visibility& visibility,
        const std::shared_ptr<Bridge>& camera)noexcept {
        if(draining_||!camera||!zone.owner_||zone.owner_!=visibility.owner_||
           camera->freshLighting_.get()!=zone.owner_||!visibility.racerVisibility7F0()||
           !zone.cameraGeneration_||zone.cameraGeneration_!=visibility.generation_||
           !zone.value_||!zone.value_->revision_)return {Status::scene,0x280118u};
        // Only a traversal-issued capability can reach this store. The current
        // incomplete27FEE0 walk has NO issuer; do not create one from bounds.
        scissor4014_=zone.value_;return {Status::acceptedCpuOnly,0x280118u,true};
    }
    Result accept24B9B0(std::shared_ptr<Allocation> allocation,std::uint32_t strip,
        std::shared_ptr<Bridge> camera,native_racer50HW::Cables& cables,std::unique_ptr<Binding> binding,
        std::unique_ptr<Pin> display,const Zone& zone,const Visibility& visibility)noexcept {
        if(draining_||!allocation||!camera||!binding||!binding->context_||!display||!display->owner_||
           camera->closed_||camera->nativeFifoPins_==std::numeric_limits<std::uint32_t>::max()||
           display->owner_->application_!=application_||!display->owner_->canRetain()||
           binding->context_->videoIdentity()!=display->owner_->video_.get()||
           !table_->coldCollectorCurrent(binding->registry_)||allocation->closed_||
           strip>=allocation->ownedStrips_||!allocation->strips30_[strip]||
           allocation->fifoPins_==std::numeric_limits<std::uint32_t>::max())return {Status::owner,0x24b9b0u};
        if(zone.owner_!=visibility.owner_||!zone.value_||scissor4014_!=zone.value_||
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
            record.scissor18=scissor4014_;
            return {Status::acceptedCpuOnly,0x24ba28u,true};
        }catch(...){if(tablePinned)table_->unpinNativeCollector();return {Status::allocation,0x105720u};}
    }
    Result drain24B7B0()noexcept {
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
        if(!draining_||done.owner_!=this||done.epoch_!=epoch04_||done.count_!=records_.size())return false;
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
}
