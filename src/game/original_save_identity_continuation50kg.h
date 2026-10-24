#pragma once
#include "shared_random_owner50id.h"
#include <cstdint>

namespace rr::game {
// The two original code sites share the SAME application seed/range/RNG owners.
// This continuation requests calls; it never runs a clock, seeds/draws an RNG,
// opens a file or stores profile191C. No current application invokes it.
class OriginalSaveIdentityContinuation50KG final {
public:
    enum class Purpose:std::uint8_t {profileConstructor17DF80,dirtySave1869B0};
    enum class Call:std::uint8_t {none,clock101A50,seed308020,draw308030};
    enum class Status:std::uint8_t {pending,complete,unsupportedNumeric,sourceRangeAssertion,invalidState};
    struct Request {
        Call call=Call::none;
        std::uint32_t sequence=0,sourcePC=0,argument=0;
    };
    struct Result {
        Status status=Status::invalidState;
        Request request{};
        std::uint32_t sourcePC=0,identity191C=0;
    };
    explicit OriginalSaveIdentityContinuation50KG(Purpose) noexcept;
    OriginalSaveIdentityContinuation50KG(const OriginalSaveIdentityContinuation50KG&)=delete;
    OriginalSaveIdentityContinuation50KG& operator=(const OriginalSaveIdentityContinuation50KG&)=delete;
    // Shared cache writes follow original instruction order. Caller must use
    // the ACTUAL one shared owner and serialize the complete source operation,
    // including outstanding requested calls, without intervening consumers.
    // Pending repeats return the same request, never perform a second draw.
    Result advance(SharedRandomOwner50ID&) noexcept;
    // Matching sequence/call/PC/argument required. These are protocol checks,
    // NOT proof of execution: future private driver must supply genuine returns.
    // Seed's ignored return is accepted only after the actual seed call returns.
    bool acknowledge(const Request&,std::uint32_t actualReturn) noexcept;
private:
    enum class Phase:std::uint8_t {entry,afterClock,afterSeed,afterDraw,done,failed};
    Result issue(Call,std::uint32_t pc,Phase,std::uint32_t argument=0) noexcept;
    Result terminal(Status,std::uint32_t pc) noexcept;
    Purpose purpose_;
    Phase phase_=Phase::entry;
    SharedRandomOwner50ID* shared_=nullptr;
    Request pending_{};
    std::uint32_t serial_=0,lastReturn_=0,identity_=0;
    Status failure_=Status::invalidState;
    std::uint32_t failurePC_=0;
};
}
