#pragma once
#include <cstdint>
#include <limits>
#include <exception>
#include <thread>
namespace rr::platform {
class OriginalUiFrontend50KG;
class VehicleWindowGl50DS;
// Native serialized-operation custody, NOT a reconstructed game global.
// Deliberately thread-wide: unbound texture callers need not supply canonical
// context pointers. No active route acquires this private Options reservation.
class OriginalOptionsMutationInterlock50KG final {
    friend class OriginalUiFrontend50KG;
    friend class VehicleWindowGl50DS;
    inline static thread_local const OriginalOptionsMutationInterlock50KG* held_=nullptr;
    inline static thread_local std::uint64_t operations_=0;
    const std::thread::id thread_=std::this_thread::get_id();
    const void* context_=nullptr;
    bool acquired_=false,retired_=false;
    OriginalOptionsMutationInterlock50KG()noexcept=default;
    bool acquire(const void* context)noexcept {
        if(thread_!=std::this_thread::get_id()||!context||acquired_||retired_||held_||operations_)return false;
        context_=context;held_=this;acquired_=true;return true;
    }
    bool current(const void* context)const noexcept {
        return thread_==std::this_thread::get_id()&&acquired_&&!retired_&&context_==context&&held_==this;
    }
    bool release(const void* context)noexcept {
        if(!current(context)||operations_)return false;
        held_=nullptr;acquired_=false;retired_=true;return true;
    }
    static bool permitted(const OriginalOptionsMutationInterlock50KG* owner)noexcept {
        return !held_||held_==owner;
    }
public:
    OriginalOptionsMutationInterlock50KG(const OriginalOptionsMutationInterlock50KG&)=delete;
    ~OriginalOptionsMutationInterlock50KG()noexcept {
        if(thread_!=std::this_thread::get_id()||acquired_)std::terminate();
    }
    static bool ordinaryAllowed()noexcept{return !held_;}
    class Operation final {
        friend class VehicleWindowGl50DS;
        bool admitted_=false;
        const std::thread::id thread_=std::this_thread::get_id();
        explicit Operation(const OriginalOptionsMutationInterlock50KG& owner)noexcept {
            if(!owner.current(owner.context_)||operations_)return;
            ++operations_;admitted_=true;
        }
    public:
        Operation()noexcept {
            if(held_||operations_==std::numeric_limits<std::uint64_t>::max())return;
            ++operations_;admitted_=true;
        }
        Operation(const Operation&)=delete;
        bool admitted()const noexcept{return admitted_;}
        ~Operation()noexcept {
            if(thread_!=std::this_thread::get_id())std::terminate();
            if(admitted_){if(!operations_)std::terminate();--operations_;}
        }
    };
};
}
