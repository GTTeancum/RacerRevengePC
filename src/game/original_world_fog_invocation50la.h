#pragma once
#include <memory>
#include <utility>
#include <thread>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <exception>
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {
//Private actual1AA178 custody, issued only after the genuine source light
//loop returns273C24. Known NULL Environment is retained, not fabricated.
//Native light readiness cannot issue this token.
class OriginalWorldFogInvocation50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    using Release=bool(*)(const void*,const OriginalWorldFogInvocation50LA&)noexcept;
    std::shared_ptr<const void> world_;
    const void* environment_=nullptr;const void* application_=nullptr;
    std::uint64_t generation_=0;std::size_t* borrowers_=nullptr;Release release_=nullptr;
    const std::thread::id thread_=std::this_thread::get_id();
    mutable bool attempted_=false;mutable std::uint32_t pc_=0x1aa178;
    OriginalWorldFogInvocation50LA(std::shared_ptr<const void> world,const void* environment,
        const void* application,std::uint64_t generation,std::size_t& borrowers,Release release)
        :world_(std::move(world)),environment_(environment),application_(application),generation_(generation),
         borrowers_(&borrowers),release_(release){
        if(!world_||!application_||!generation_||!release_||
           borrowers==std::numeric_limits<std::size_t>::max())std::terminate();
        ++borrowers;
    }
public:
    OriginalWorldFogInvocation50LA(const OriginalWorldFogInvocation50LA&)=delete;
    OriginalWorldFogInvocation50LA& operator=(const OriginalWorldFogInvocation50LA&)=delete;
    ~OriginalWorldFogInvocation50LA(){
        if(thread_!=std::this_thread::get_id()||!borrowers_||!*borrowers_||
           !release_||!release_(world_.get(),*this))std::terminate();
        --*borrowers_;
    }
};
}
