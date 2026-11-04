#pragma once
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../src/game/original_application_animation_clock50kg.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <thread>

namespace original_main_entry50KG {
struct Application;
class RaceDisplayLifetime50LA;
enum class NativeVideoPacketInvocationState50LA : std::uint8_t {
    absent,pending,completed,faulted,cancelled,retired
};
// Numeric observations only. No API accepts these as completion, source-call,
// context or lifetime authority. FINISH is the native packet prefix, not CSR.
struct NativeVideoPacketInvocationInfo50LA {
    NativeVideoPacketInvocationState50LA state=NativeVideoPacketInvocationState50LA::absent;
    std::uint64_t invocation=0,displayGeneration=0;
    bool retained=false,clearAccepted=false;
    rr::platform::NativeVideoPacketFinishInfo50LA finish{}; //Last actual observation; may predate an admission failure.
};
// One opt-in native clear -> FINISH execution, owned by its actual Application.
// This is NOT source25CAD0/2596A0/25A9C0 control flow. Source Video supplies
// immutable numeric artifacts; no source bytes, pending PC or events change.
class NativeVideoPacketInvocation50LA final {
    friend struct Application;
    using Diagnostic=rr::platform::VehicleWindowDiagnostic50DS;
    using Info=NativeVideoPacketInvocationInfo50LA;
    using State=NativeVideoPacketInvocationState50LA;
    using Policies=rr::platform::OriginalVideoPacketCpuRenderer50LA::NativePolicies;
    NativeVideoPacketInvocation50LA(const Application&,std::shared_ptr<RaceDisplayLifetime50LA>,
        std::unique_ptr<const rr::game::OriginalVideoClearPacket25CAD0Native50LA>,
        std::unique_ptr<const rr::game::OriginalVideoFinishPacket25AB2CNative50LA>,std::uint64_t)noexcept;
    static bool begin(const Application&,std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t,
        Policies,std::size_t,Info&,Diagnostic&)noexcept;
    bool identityCurrent()const noexcept;
    bool poll(Info&,Diagnostic&)noexcept;
    bool cancel(Info&,Diagnostic&)noexcept;
    bool retire(Info&,Diagnostic&)noexcept;
    bool releaseCustody(Diagnostic&)noexcept;
    void observe(Info&)const noexcept;
    static bool reject(Diagnostic&,const char*)noexcept;
    const Application* const application_;
    const std::thread::id thread_=std::this_thread::get_id();
    const rr::game::OriginalApplicationAnimationClockView50KG lifetime_;
    std::shared_ptr<RaceDisplayLifetime50LA> display_;
    std::unique_ptr<const rr::game::OriginalVideoClearPacket25CAD0Native50LA> clear_;
    std::unique_ptr<const rr::game::OriginalVideoFinishPacket25AB2CNative50LA> finish_;
    const std::uint64_t invocation_,displayGeneration_;
    State state_=State::faulted;
    rr::platform::NativeVideoPacketFinishInfo50LA finishInfo_{};
    bool pinned_=false,custody_=false,clearAccepted_=false,busy_=false;
public:
    ~NativeVideoPacketInvocation50LA()noexcept;
    NativeVideoPacketInvocation50LA(const NativeVideoPacketInvocation50LA&)=delete;
    NativeVideoPacketInvocation50LA& operator=(const NativeVideoPacketInvocation50LA&)=delete;
};
}
