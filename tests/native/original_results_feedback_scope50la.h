#pragma once
#include <cstdint>
#include <memory>
#include <thread>

namespace interactive_continuation50DU { struct Context; }
namespace native_racer50HN { class Bank; }
namespace original_main_entry50KG { struct Application; }
namespace rr::game {
class OriginalRaceConstructor50LA;
class OriginalResults10450LA;
struct ControllerFeedbackState50KZ;
}

namespace original_results_feedback50LA {
class Scope;
// Native same-thread borrow of the two ACTUAL setup-owned device objects.
// Not a device allocator, renderer lease, input receipt or guest Application
// layout. Only Scope can mint it; only the actual Results owner can consume it.
class Registration final {
    friend class Scope;
    friend class rr::game::OriginalResults10450LA;
    const std::thread::id thread_;
    const original_main_entry50KG::Application* const application_;
    const std::weak_ptr<rr::game::OriginalRaceConstructor50LA> race_;
    const std::uint64_t generation_;
    const native_racer50HN::Bank* const bank_;
    rr::game::ControllerFeedbackState50KZ (*devices_)[2];
    bool active_=true;
    Registration(const original_main_entry50KG::Application&,
        const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>&,
        const native_racer50HN::Bank&,
        rr::game::ControllerFeedbackState50KZ (&)[2])noexcept;
    // Caller has already authenticated the successful-load Operation, exact
    // current Bank and its ACTUAL arena humanCount. No public count/array API.
    bool clearHumans1AF770(const rr::game::OriginalRaceConstructor50LA&,
        const native_racer50HN::Bank*,std::uint32_t actualHumanCount)noexcept;
public:
    Registration(const Registration&)=delete;
    Registration& operator=(const Registration&)=delete;
    ~Registration()=default;
};

// Nonmoving lexical guard, privately constructed by the real Continuation
// factory. Setup's device array outlives that entire synchronous run. This
// guard is declared AFTER the Bank/RA/camera owners and revoked BEFORE the
// first fallible close operation; destruction also revokes on every unwind.
class Scope final {
    friend struct ::interactive_continuation50DU::Context;
    std::shared_ptr<Registration> registration_;
    explicit Scope(::interactive_continuation50DU::Context&,
        const native_racer50HN::Bank&)noexcept;
    // The common race loads Results104 during publication, after this
    // Scope's construction; it binds once that load has completed.
    void bindAfterPublication(::interactive_continuation50DU::Context&,
        const native_racer50HN::Bank&)noexcept;
    void attach(::interactive_continuation50DU::Context&,const native_racer50HN::Bank&)noexcept;
    void revoke()noexcept;
public:
    Scope(const Scope&)=delete;
    Scope& operator=(const Scope&)=delete;
    Scope(Scope&&)=delete;
    Scope& operator=(Scope&&)=delete;
    ~Scope()noexcept;
};
}
