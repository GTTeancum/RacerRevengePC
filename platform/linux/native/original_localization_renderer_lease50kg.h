#pragma once
#include "../../../src/game/original_application_animation_clock50kg.h"
#include <atomic>
#include <cstdint>
#include <memory>
#include <thread>
namespace original_main_entry50KG { struct Application; class RaceDisplayLifetime50LA; }
namespace rr::assets { class OriginalApplicationLocalization50KG; }
namespace rr::game { class OriginalApplicationEntities50LA; }
namespace rr::platform {
enum class OriginalLocalizationRendererStatus50KG : std::uint8_t {
    staged, complete, authority, dictionaryPhase, displayOwner, allocation,
    nativeInvocationEnvelope, sourceVideoProducerPending, sourceProcessProducerPending, sourceCallExclusivityPending,
    nativeContextPending, callbackPending, queuePending, nativeTransferPending, nativeErrorStateUnsupported,
    nativeCompletionFault, cancelled, retired
};
struct OriginalLocalizationRendererDiagnostic50KG {
    OriginalLocalizationRendererStatus50KG status=OriginalLocalizationRendererStatus50KG::authority;
    std::uint32_t sourcePC=0;
    std::uint64_t invocation=0;
    std::uint8_t completedSourceCalls=0;
    bool sourceEffectsCommitted=false;
};
// One private actual14C5A0 invocation. This is a retained native service
// transaction, not selected English, a source caller, or a completion receipt.
// Dedicated frontend execution is a synchronous native no-error adaptation:
// real ordered context completion, same retained C4==0, no callbacks/queues.
// It neither produces nor reads an invented source Video40/process history.
class OriginalLocalizationRendererLease50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class rr::assets::OriginalApplicationLocalization50KG;
    using Application=::original_main_entry50KG::Application;
    using Display=::original_main_entry50KG::RaceDisplayLifetime50LA;
    using Dictionary=rr::assets::OriginalApplicationLocalization50KG;
    using Authority=rr::game::OriginalApplicationEntities50LA;
    enum class Phase : std::uint8_t { beforeFirst, beforeSecond, servicesComplete, faulted, cancelled, retired };
    OriginalLocalizationRendererLease50KG(const Application&,std::shared_ptr<Authority>,
        std::shared_ptr<Dictionary>,std::shared_ptr<Display>,std::uint64_t)noexcept;
    static bool stage(const Application&,std::shared_ptr<OriginalLocalizationRendererLease50KG>&,
        OriginalLocalizationRendererDiagnostic50KG&)noexcept;
    bool cleanupIdentityCurrent()const noexcept;
    bool current()const noexcept;
    // Private eligibility of THIS actual completed invocation, not an
    // Observation/boolean receipt supplied by a caller. No service is replayed.
    bool permitsEnglishSelection14C5F0(const Application&,const Dictionary&)const noexcept;
    bool advance(const Application&,OriginalLocalizationRendererDiagnostic50KG&)noexcept;
    bool cancel(const Application&,OriginalLocalizationRendererDiagnostic50KG&)noexcept;
    // One process record, never reset by Application/dictionary retirement.
    // The operations below are private source leaves, not service receipts.
    struct ProcessState {
        std::atomic<std::uint8_t> guard88F4{0},guard88F8{0};
        std::atomic<std::uint32_t> lastErrorFrame4355E0{0},busy443DC8{0};
        std::atomic<std::int32_t> assertion4355E8{0};
        std::atomic<std::uint32_t> recoveryScale382FA8{6};
        bool beginIdle242F08()noexcept;
        bool finishNoError2439FC(std::uint32_t,std::uint32_t&)noexcept;
    };
    static ProcessState& processState()noexcept;
    static bool processContinuityCurrent()noexcept;
    static bool sourceCallExclusiveCurrent()noexcept;
    bool service242EE0(OriginalLocalizationRendererDiagnostic50KG&,std::uint32_t,bool)noexcept;
    bool finish(OriginalLocalizationRendererDiagnostic50KG&,OriginalLocalizationRendererStatus50KG,
        std::uint32_t)const noexcept;
    const Application* issuer_=nullptr;
    std::shared_ptr<Authority> authority_;
    std::shared_ptr<Dictionary> dictionary_;
    std::shared_ptr<Display> display_;
    std::weak_ptr<OriginalLocalizationRendererLease50KG> issued_;
    rr::game::OriginalApplicationAnimationClockView50KG lifetime_;
    const std::thread::id thread_=std::this_thread::get_id();
    const std::uint64_t invocation_,displayGeneration_;
    Phase phase_=Phase::beforeFirst;
    std::uint8_t completedCalls_=0;
    bool sourceEffects_=false,pinned_=false,everFaulted_=false;
    mutable std::uint32_t diagnosticPC_=0x14C5D0;
public:
    struct Observation {
        std::uint64_t invocation=0,displayGeneration=0;
        std::uint8_t completedSourceCalls=0;
        bool retained=false,sourceEffectsCommitted=false,servicesComplete=false;
        bool cancelled=false,faulted=false,sourceVideoContinuity=false;
        std::uint32_t pendingSourcePC=0;
    };
    ~OriginalLocalizationRendererLease50KG()noexcept;
    OriginalLocalizationRendererLease50KG(const OriginalLocalizationRendererLease50KG&)=delete;
    OriginalLocalizationRendererLease50KG& operator=(const OriginalLocalizationRendererLease50KG&)=delete;
    bool inspect(Observation&)const noexcept;
};
}
