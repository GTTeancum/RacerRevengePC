#pragma once
#include "original_native_collector_fifo50la.h"
#include <array>
#include <memory>
#include <thread>
namespace rr::game {class OriginalLoadingScreen40Mode0LA;}
namespace original_main_entry50KG {
// Native exclusive custody of the real fresh 1CAAA4/1CAAB4 allocation whose
// return the source discards. Only the already-nonnull443848 branch is admitted.
// This is NOT adoption of that registered FIFO, nor Screen1CC580 retirement.
class OriginalLoadingCollector50LA final {
    friend class rr::game::OriginalLoadingScreen40Mode0LA;
    struct Pool105620 final {
        // Actual separate 0x38 pool08, constructed with (0x1C,0x1000).
        // No slabs are allocated by105620;1058F0 sees null14/null1C.
        std::uint64_t references00=1; //1048F0/104900
        std::uint32_t vtable08=0x3cc9e0u;
        std::uint32_t capacity10=(0x1000u-0x1cu)/0x1cu;
        void* chain14=nullptr;void* cursor18=nullptr;void* spare1C=nullptr;
        std::uint32_t used20=capacity10,recordBytes24=0x1cu,blockBytes28=0x1000u;
        std::uint32_t field2C=0,field30=0;
        // +0C and +34 are constructor-unknown, deliberately not invented.
    };
    const Application* application_=nullptr;
    const std::thread::id ownerThread_=std::this_thread::get_id();
    // Same concrete FIFO type as443848, but no producer can obtain this pointer.
    std::shared_ptr<NativeCollectorFifo50LA> fresh_;
    std::unique_ptr<Pool105620> pool08_;
    std::array<void*,4096> slots0C_{}; //29C28C..298, no accepted leaf aliases.
    std::uint32_t count00_=0;
    bool complete_=false,closed_=false;
    explicit OriginalLoadingCollector50LA(const Application& app):application_(&app){}
    static bool construct1CAAB4(const Application& app,
        std::unique_ptr<OriginalLoadingCollector50LA>& out,std::uint32_t& pc)noexcept {
        pc=0x1caaa4u;
        // Private Screen caller must check its immutable owner thread BEFORE
        // reading Application slots. No arbitrary caller-supplied certification.
        if(out)return false;
        const auto registered=app.nativeCollectorFifoOwner_;
        if(!registered||registered->ownerThread_!=std::this_thread::get_id())return false;
        if(registered->application_!=&app||registered->closed_||registered->draining_||
           !app.entitiesOwner_||!app.entitiesOwner_->application10()||
           !app.standardMaterialDomainOwner_||!app.standardMaterialDomainOwner_->prepared())return false;
        const auto table=app.standardMaterialDomainOwner_->materials382B38();
        if(!table||registered->table_!=table)return false;
        try {
            out.reset(new OriginalLoadingCollector50LA(app));
            out->fresh_=std::shared_ptr<NativeCollectorFifo50LA>(new NativeCollectorFifo50LA(app,table));
            pc=0x29c2a0u;
            out->pool08_.reset(new Pool105620);
            // Source29C2D4 branches over global store: leave the exact real
            // registered owner untouched. No14D440 bind/receipt or table alias
            // belongs to this fresh constructor. Null-global is NOT admitted.
            pc=0x29c2d4u;
            if(app.nativeCollectorFifoOwner_!=registered)return false;
            out->complete_=true;pc=0x24b718u;return true;
        }catch(...){return false;} // Retain any partial owner for native unwind.
    }
    bool complete()const noexcept {
        return ownerThread_==std::this_thread::get_id()&&complete_&&!closed_;
    }
    bool retireNativeExclusive()noexcept {
        if(ownerThread_!=std::this_thread::get_id())return false;
        if(closed_)return true;
        if(!application_||count00_)return false;
        for(const auto* p:slots0C_)if(p)return false;
        if(fresh_&&(fresh_.use_count()!=1||application_->nativeCollectorFifoOwner_.get()==fresh_.get()||
           fresh_->application_!=application_||fresh_->draining_||!fresh_->records_.empty()||
           !fresh_->leaves_.empty()||fresh_->garageRecordCount_||fresh_->uiConstructor1B73BC_))return false;
        if(pool08_&&(pool08_->references00!=1||pool08_->chain14||pool08_->cursor18||pool08_->spare1C))return false;
        // NATIVE exclusive unwind, not a reached1CC580 call.24B748 sees null4010;
        //29C330 releases08 via104970 ->1056B0 ->1058F0 (both chains null).
        //29C354 must NOT clear foreign443848; count00=0 skips all leaf deletes.
        // No canonical-table retire call: this object never bound that registry.
        pool08_.reset();
        if(fresh_){fresh_->closed_=true;fresh_->table_.reset();fresh_.reset();}
        complete_=false;closed_=true;application_=nullptr;return true;
    }
public:
    OriginalLoadingCollector50LA(const OriginalLoadingCollector50LA&)=delete;
    OriginalLoadingCollector50LA& operator=(const OriginalLoadingCollector50LA&)=delete;
    ~OriginalLoadingCollector50LA()noexcept {if(!retireNativeExclusive())std::terminate();}
};
}
