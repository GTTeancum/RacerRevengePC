#pragma once
// One application RNG/noise authority with explicit, non-reentrant execution
// leases. Native adaptation: a frame's embedded noise is transactional backing
// ONLY during its lease. Kernel workspace.next.noise is never exported directly.
#include "native_racer_frame50jq_fixture.h"
#include "native_ai_session50iv_fixture.h"
#include <cstdint>
#include <exception>
#include <limits>
#include <stdexcept>

namespace native_racer50JQ {
class SharedNoise;
inline SharedNoise& sharedNoise();
class SharedNoise final {
    friend SharedNoise& sharedNoise();
    CoupledNoiseContext49M* active_=nullptr;
    std::uint64_t sequence_=0,activeIdentity_=0;
    bool frameActive_=false;
    SharedNoise()=default;
    CoupledNoiseContext49M& authority() noexcept {
        return native_ai50IV::applicationNoise().state;
    }
    std::uint64_t begin(CoupledNoiseContext49M* frameBacking) {
        if(active_||sequence_==std::numeric_limits<std::uint64_t>::max())
            throw std::runtime_error("nested or exhausted canonical shared-noise lease");
        auto& application=native_ai50IV::applicationNoise();
        if(!application.initialized){
            initializeOriginalSharedNoise50IU(application.state);application.initialized=true;
        }
        if(frameBacking){*frameBacking=authority();active_=frameBacking;frameActive_=true;}
        else{active_=&authority();frameActive_=false;}
        activeIdentity_=++sequence_;return activeIdentity_;
    }
    CoupledNoiseContext49M& current(std::uint64_t identity,
        const CoupledNoiseContext49M* expected) {
        if(!identity||identity!=activeIdentity_||!active_||active_!=expected)
            throw std::runtime_error("expired or foreign canonical shared-noise lease");
        return *active_;
    }
    void end(std::uint64_t identity,const CoupledNoiseContext49M* expected) noexcept {
        // Nonmovable leases are the only callers and have exclusive ownership.
        // No fallible consumer runs after this guard. A mismatch is corruption,
        // never permission to overwrite a newer application's random chronology.
        if(!identity||identity!=activeIdentity_||!active_||active_!=expected)std::terminate();
        if(frameActive_)authority()=*active_;
        active_=nullptr;activeIdentity_=0;frameActive_=false;
    }
public:
    SharedNoise(const SharedNoise&)=delete;
    SharedNoise& operator=(const SharedNoise&)=delete;
    SharedNoise(SharedNoise&&)=delete;
    SharedNoise& operator=(SharedNoise&&)=delete;
    bool leased()const noexcept{return active_!=nullptr;}
    bool frameLeased()const noexcept{return frameActive_;}
    // Bank may borrow this stable bool solely to reject physical observations
    // inside an in-flight frame. Application-only leases do not mutate frames.
    const bool* frameReadBarrier()const noexcept{return &frameActive_;}

    class FrameLease final {
        SharedNoise* owner_=nullptr;
        Context* context_=nullptr;
        std::uint64_t identity_=0;
    public:
        FrameLease(SharedNoise& owner,Context& context)
            :owner_(&owner),context_(&context) {
            identity_=owner.begin(&context.state.noise);
        }
        FrameLease(const FrameLease&)=delete;
        FrameLease& operator=(const FrameLease&)=delete;
        FrameLease(FrameLease&&)=delete;
        FrameLease& operator=(FrameLease&&)=delete;
        ~FrameLease(){close();}
        // Idempotent explicit release supports the initial slot0 cold prefix.
        // This publishes reached committed frame state even after an exception.
        void close()noexcept {
            if(owner_){owner_->end(identity_,&context_->state.noise);owner_=nullptr;}
        }
        bool active()const noexcept{return owner_!=nullptr;}
        std::uint64_t identity()const noexcept{return owner_?identity_:0;}
        Context& context() {
            if(!owner_)throw std::runtime_error("closed canonical frame lease");
            owner_->current(identity_,&context_->state.noise);return *context_;
        }
        CoupledNoiseContext49M& noise() {
            if(!owner_)throw std::runtime_error("closed canonical frame-noise lease");
            return owner_->current(identity_,&context_->state.noise);
        }
    };
    class ApplicationLease final {
        SharedNoise* owner_=nullptr;
        CoupledNoiseContext49M* noise_=nullptr;
        std::uint64_t identity_=0;
    public:
        explicit ApplicationLease(SharedNoise& owner):owner_(&owner) {
            identity_=owner.begin(nullptr);noise_=owner.active_;
        }
        ApplicationLease(const ApplicationLease&)=delete;
        ApplicationLease& operator=(const ApplicationLease&)=delete;
        ApplicationLease(ApplicationLease&&)=delete;
        ApplicationLease& operator=(ApplicationLease&&)=delete;
        ~ApplicationLease(){close();}
        void close()noexcept {if(owner_){owner_->end(identity_,noise_);owner_=nullptr;}}
        bool active()const noexcept{return owner_!=nullptr;}
        std::uint64_t identity()const noexcept{return owner_?identity_:0;}
        CoupledNoiseContext49M& noise() {
            if(!owner_)throw std::runtime_error("closed canonical application-noise lease");
            return owner_->current(identity_,noise_);
        }
    };
};
inline SharedNoise& sharedNoise(){static SharedNoise owner;return owner;}
} // namespace native_racer50JQ
