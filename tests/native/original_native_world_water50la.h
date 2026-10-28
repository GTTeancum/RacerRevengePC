#pragma once
#include <cassert>
#include <cstdint>
#include <memory>
#include <thread>
#include <utility>
namespace rr::game {class OriginalApplicationEntities50LA;struct OriginalWreckComponent50LA;}
namespace track_height_cold50JB {struct Owner;}
namespace interactive_setup50DU {struct Context;}
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace native_world_water50LA {
// Constructor-created native water service domain, NOT a guest World400 value.
// No terrain3FC inference and no source reset/front-end history receipt.
class Domain final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    using App=rr::game::OriginalApplicationEntities50LA;
    using Entity=rr::game::OriginalWreckComponent50LA;
    using Height=track_height_cold50JB::Owner;
    using Context=interactive_setup50DU::Context;
    using Predicate=bool(*)(const Context&,const Height&,const Domain&) noexcept;
    const App* application_=nullptr;
    const Entity* world_=nullptr;
    const Height* sourceHeight_=nullptr;
    std::uint64_t generation_=0;
    std::thread::id thread_;
    const Context* context_=nullptr;
    Predicate predicate_=nullptr;
    bool everBound_=false,revoked_=false;
    mutable bool reading_=false,currentnessLost_=false;
    std::uint64_t epoch_=0;
    Domain(const App& app,const Entity& world,std::uint64_t generation,
           const Height& height) noexcept
        :application_(&app),world_(&world),sourceHeight_(&height),generation_(generation),
         thread_(std::this_thread::get_id()){}
    bool identity(const App& app,const Entity& world,std::uint64_t generation)const noexcept {
        return !revoked_&&application_==&app&&world_==&world&&generation!=0&&generation_==generation&&
            sourceHeight_&&thread_==std::this_thread::get_id();
    }
    bool matches(const App& app,const Entity& world,std::uint64_t generation,
                 const Height& height)const noexcept {
        return identity(app,world,generation)&&!currentnessLost_&&sourceHeight_==&height;
    }
    bool canRetire(const App& app,const Entity& world,std::uint64_t generation)const noexcept {
        return identity(app,world,generation)&&!reading_;
    }
    bool readAbsent(const App& app,const Entity& world,std::uint64_t generation)const noexcept {
        if(!identity(app,world,generation)||currentnessLost_||reading_||!context_||!predicate_)return false;
        reading_=true;
        const bool result=predicate_(*context_,*sourceHeight_,*this);
        reading_=false;
        if(!result)currentnessLost_=true; // An observed epoch loss cannot later become absence again.
        return result;
    }
    void withdraw(std::uint64_t epoch) noexcept {
        if(epoch!=epoch_)return;
        assert(thread_==std::this_thread::get_id()&&!reading_);
        context_=nullptr;predicate_=nullptr;++epoch_;
    }
    void revoke() noexcept {
        // Caller must canRetire first; synchronous predicate never calls revoke.
        context_=nullptr;predicate_=nullptr;sourceHeight_=nullptr;
        application_=nullptr;world_=nullptr;generation_=0;revoked_=true;++epoch_;
    }
public:
    ~Domain()=default;
    Domain(const Domain&)=delete;
    Domain& operator=(const Domain&)=delete;
    class Lease final {
        friend class Domain;
        std::shared_ptr<Domain> domain_;
        std::uint64_t epoch_=0;
        Lease(std::shared_ptr<Domain> domain,std::uint64_t epoch) noexcept
            :domain_(std::move(domain)),epoch_(epoch){}
        void clear() noexcept {if(domain_)domain_->withdraw(epoch_);domain_.reset();epoch_=0;}
    public:
        Lease() noexcept=default;
        ~Lease(){clear();}
        Lease(const Lease&)=delete;
        Lease& operator=(const Lease&)=delete;
        Lease(Lease&& other) noexcept:domain_(std::move(other.domain_)),epoch_(other.epoch_){other.epoch_=0;}
        Lease& operator=(Lease&& other) noexcept {
            if(this!=&other){clear();domain_=std::move(other.domain_);epoch_=other.epoch_;other.epoch_=0;}
            return *this;
        }
        explicit operator bool()const noexcept {
            return domain_&&!domain_->revoked_&&!domain_->currentnessLost_&&domain_->epoch_==epoch_&&
                domain_->context_&&domain_->predicate_&&
                domain_->thread_==std::this_thread::get_id();
        }
    };
private:
    Lease bind(const std::shared_ptr<Domain>& self,const Context& context,
               const Height& height,Predicate predicate) noexcept {
        if(self.get()!=this||revoked_||everBound_||reading_||!application_||!world_||
           sourceHeight_!=&height||!predicate||thread_!=std::this_thread::get_id())return {};
        // Admission and every subsequent read consult the actual live owner slots.
        everBound_=true; // Entered admission is not replayable, including predicate failure.
        reading_=true;const bool current=predicate(context,height,*this);reading_=false;
        if(!current)return {};
        context_=&context;predicate_=predicate;++epoch_;
        return Lease(self,epoch_);
    }
};
}
