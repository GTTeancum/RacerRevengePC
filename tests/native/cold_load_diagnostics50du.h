#pragma once
#include <chrono>
#include <cstdio>
#include <exception>
#include <cstdint>

// Cold-load observations only. No game clock, allocation counter, source RNG,
// context ownership, or input state is changed. All labels are string literals.
// Output follows stderr into the native entry point's single process log.
class ColdLoadDiagnostics50DU final {
    using Clock = std::chrono::steady_clock;
    bool enabled_;
    const char* track_;
    std::uint32_t rider_;
    std::uint64_t session_;
    const char* stage_=nullptr;
    Clock::time_point origin_=Clock::now(), started_=origin_;
    int exceptions_=std::uncaught_exceptions();
    void report(const char* phase,const char* reason) const noexcept {
        if(!enabled_||!stage_)return;
        const auto now=Clock::now();
        const auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(now-origin_).count();
        const auto duration=std::chrono::duration_cast<std::chrono::milliseconds>(now-started_).count();
        std::fprintf(stderr,"cold_load phase=%s stage=%s track=%s rider=%u session=%llu elapsed_ms=%lld stage_ms=%lld reason=%s\n",
            phase,stage_,track_?track_:"unknown",static_cast<unsigned>(rider_),
            static_cast<unsigned long long>(session_),static_cast<long long>(elapsed),
            static_cast<long long>(duration),reason);
        std::fflush(stderr);
    }
public:
    ColdLoadDiagnostics50DU(bool enabled,const char* track,std::uint32_t rider,std::uint64_t session) noexcept
        : enabled_(enabled),track_(track),rider_(rider),session_(session) {}
    ColdLoadDiagnostics50DU(const ColdLoadDiagnostics50DU&)=delete;
    ColdLoadDiagnostics50DU& operator=(const ColdLoadDiagnostics50DU&)=delete;
    void begin(const char* stage) noexcept {
        if(!enabled_)return;
        report("END","boundary_completed");
        stage_=stage;started_=Clock::now();
        report("BEGIN","entered");
    }
    void complete() noexcept {
        report("END","input_loop_ready");stage_=nullptr;
    }
    ~ColdLoadDiagnostics50DU() noexcept {
        // Never call an unfinished stage successful. SEH, abort, termination,
        // or process kills may bypass this destructor; absence is not a verdict.
        report("INCOMPLETE",std::uncaught_exceptions()>exceptions_?
            "cpp_exception_unwinding":"returned_before_input_loop");
    }
};
