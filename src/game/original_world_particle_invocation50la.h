#pragma once
#include <memory>
#include <cstddef>
#include <cstdint>
#include <thread>
#include <limits>
#include <exception>
#include <utility>
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {
// Private custody for the genuine ordered World1B4DC0 ->160E00 occurrence.
// Source lighting/fog, fresh visibility predecessor and actual World4A0
// publication must all complete first. Node availability, reset NULL and native
// prewarm cannot construct this token or supply its captured Environment.
class OriginalWorldParticleInvocation50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    using Release=bool(*)(const void*,const OriginalWorldParticleInvocation50LA&)noexcept;
    std::shared_ptr<const void> world_,particle_;
    const void* environment_=nullptr;
    const void* application_=nullptr;
    std::uint64_t worldGeneration_=0,particleGeneration_=0;
    std::size_t *worldBorrowers_=nullptr,*particleBorrowers_=nullptr;
    Release release_=nullptr;
    mutable bool configAttempted_=false;
    mutable std::uint32_t configPC_=0x160f64;
    const std::thread::id thread_=std::this_thread::get_id();
    OriginalWorldParticleInvocation50LA(std::shared_ptr<const void> world,
        std::shared_ptr<const void> particle,const void* environment,const void* app,
        std::uint64_t worldGeneration,std::uint64_t particleGeneration,
        std::size_t& worldBorrowers,std::size_t& particleBorrowers,Release release)
        :world_(std::move(world)),particle_(std::move(particle)),environment_(environment),
         application_(app),worldGeneration_(worldGeneration),particleGeneration_(particleGeneration),
         worldBorrowers_(&worldBorrowers),particleBorrowers_(&particleBorrowers),release_(release) {
        if(!world_||!particle_||!environment_||!application_||!worldGeneration_||!particleGeneration_||!release_||
           worldBorrowers==std::numeric_limits<std::size_t>::max()||particleBorrowers==std::numeric_limits<std::size_t>::max())std::terminate();
        ++worldBorrowers;++particleBorrowers;
    }
public:
    OriginalWorldParticleInvocation50LA(const OriginalWorldParticleInvocation50LA&)=delete;
    OriginalWorldParticleInvocation50LA& operator=(const OriginalWorldParticleInvocation50LA&)=delete;
    ~OriginalWorldParticleInvocation50LA(){
        if(thread_!=std::this_thread::get_id()||!worldBorrowers_||!*worldBorrowers_||
           !particleBorrowers_||!*particleBorrowers_||!release_||!release_(world_.get(),*this))std::terminate();
        // Actual abort callback precedes counter release and both strong pins.
        --*particleBorrowers_;--*worldBorrowers_;
    }
};
}
