#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <thread>

namespace rr::platform {
inline constexpr std::size_t windowScancodeCount50DQ = 512;
inline constexpr std::size_t windowEventBudget50DQ = 128;
enum class WindowGlProfile50DQ { Unspecified, Core, Compatibility };
struct WindowHostRequest50DQ {
    const char* title = nullptr; // Borrowed only during create; nonempty UTF-8.
    std::int32_t width = 0, height = 0; // Logical window dimensions.
    std::int32_t major = 0, minor = 0; // Minimum desktop GL version, >= 3.2.
    WindowGlProfile50DQ profile = WindowGlProfile50DQ::Unspecified;
    std::int32_t redBits = 8, greenBits = 8, blueBits = 8, alphaBits = 8;
    std::int32_t depthBits = 24, stencilBits = 8;
    std::int32_t samples = 0; // Zero explicitly forbids multisampling.
    std::int32_t swapInterval = 1; // Exactly -1 (adaptive), 0 or 1; no fallback.
    bool resizable = true;
    // Opt-in actual SDL GameController transport; absent devices stay absent.
    // Keyboard-only windows do not initialize or load this subsystem.
    bool gameControllers = false;
};
struct WindowHostInfo50DQ {
    std::int32_t width = 0, height = 0;
    std::int32_t drawableWidth = 0, drawableHeight = 0; // GL pixel dimensions.
    std::int32_t major = 0, minor = 0;
    WindowGlProfile50DQ profile = WindowGlProfile50DQ::Unspecified;
    std::int32_t redBits = 0, greenBits = 0, blueBits = 0, alphaBits = 0;
    std::int32_t depthBits = 0, stencilBits = 0, samples = 0, swapInterval = 0;
    bool focused = false, minimized = false;
};
struct WindowGameController50DQ {
    // Actual opened SDL instance identity; stable slots do not shift on unplug.
    bool connected=false;
    bool rumbleSupported=false; // Actual SDL_GameControllerHasRumble result.
    std::int32_t instanceId=-1;
    // SDL2 LEFTX,LEFTY,RIGHTX,RIGHTY,TRIGGERLEFT,TRIGGERRIGHT.
    std::array<std::int16_t,6> axes{};
    // SDL2 A through DPAD_RIGHT. No guessed raw joystick button numbering.
    std::array<std::uint8_t,15> buttons{},pressed{},released{}; // Drained genuine SDL button edges.
};
enum class ControllerRumbleStatus50KZ { Submitted, NoDevice, ChangedDevice, Unsupported };
struct ControllerRumbleReceipt50KZ {
    ControllerRumbleStatus50KZ status=ControllerRumbleStatus50KZ::NoDevice;
    std::int32_t instanceId=-1;
    std::uint8_t smallMotor=0,largeMotor=0;
};
// Native SDL slot observation, not original pad-port identity or page readiness.
enum class WindowControllerPresence50DQ { Unknown, Absent, Present };
struct WindowInput50DQ {
    // Raw SDL2 physical scancode indices, each value 0 or 1. No game bindings.
    // Edges reset each poll; both can be set if a press and release were drained.
    std::array<std::uint8_t, windowScancodeCount50DQ> down{}, pressed{}, released{};
    std::array<WindowGameController50DQ,2> controllers{};
    bool gameControllersEnabled=false;
    WindowControllerPresence50DQ secondControllerPresence=WindowControllerPresence50DQ::Unknown;
    std::uint32_t controllerOpenFailures=0; // This poll; failed opens never report connected.
    std::uint32_t eventsProcessed = 0;
    bool quitRequested = false; // Latched until close/recreate, including titlebar close.
    bool resized = false; // Logical or drawable dimensions changed, or resize event.
    bool focused = false, minimized = false;
    // An inactive interval occurred in this poll, even if focus/restore returned
    // before its end. Consumers can release/disarm without missing that edge.
    bool activityLost = false;
    // Conservative: the entire budget was consumed; more events MAY remain.
    // Key state reflects drained events, so a caller may defer simulation here.
    bool eventBudgetExhausted = false;
};
enum class WindowHostError50DQ {
    None, WrongThread, InvalidRequest, AlreadyOwned, NotCreated, OutOfMemory,
    LibraryUnavailable, MissingEntryPoint, UnsupportedSdl, ForeignSdlOwner,
    ForeignCurrentContext, VideoFailure, AttributeFailure, WindowFailure,
    ContextFailure, GlFailure, RequirementsNotMet, SwapIntervalFailure, NotCurrent,
    EventFailure, ReleaseFailure, LibraryCloseFailure, ControllerFailure
};
struct WindowHostDiagnostic50DQ {
    WindowHostError50DQ error = WindowHostError50DQ::None;
    const char* operation = nullptr; // Static string.
    std::array<char, 256> detail{}; // Copied SDL/loader error; survives cleanup.
    std::uint32_t glError = 0;
    WindowHostError50DQ cleanupError = WindowHostError50DQ::None;
    const char* cleanupOperation = nullptr;
    std::array<char, 256> cleanupDetail{};
};
using WindowGlProc50DQ = void (*)();

// Linux/Windows x64 SDL2 window + desktop GL owner. Construct/use/destroy on the process's
// main thread. Requires exclusive SDL subsystem/event-queue ownership for its
// lifetime, and a dedicated rendering thread with no foreign GL/EGL/GLX owner.
// Does not render, select a camera, advance gameplay, map keys, pace simulation,
// set a viewport, or install/configure any library/driver/display.
class WindowHost50DQ final {
public:
    WindowHost50DQ() noexcept;
    // As in 50DC, wrong-thread destruction or unresolved teardown terminates.
    // Call close explicitly first to inspect and resolve a cleanup diagnostic.
    ~WindowHost50DQ() noexcept;
    WindowHost50DQ(const WindowHost50DQ&) = delete;
    WindowHost50DQ& operator=(const WindowHost50DQ&) = delete;
    WindowHost50DQ(WindowHost50DQ&&) = delete;
    WindowHost50DQ& operator=(WindowHost50DQ&&) = delete;

    bool create(const WindowHostRequest50DQ&, WindowHostDiagnostic50DQ&) noexcept;
    bool bind(WindowHostDiagnostic50DQ&) noexcept;
    bool release(WindowHostDiagnostic50DQ&) noexcept;
    // Pumps OS events once and drains at most windowEventBudget50DQ events.
    // No application heap allocation. SDL/backend pumping itself is not given a
    // hard time bound. Ignored SDL-owned drop/text payloads are freed correctly.
    bool poll(WindowInput50DQ&, WindowHostDiagnostic50DQ&) noexcept;
    // Submission only: SDL's void swap API cannot prove presentation/completion.
    bool swap(WindowHostDiagnostic50DQ&) noexcept;
    bool setSwapInterval(std::int32_t,WindowHostDiagnostic50DQ&) noexcept;
    // Nonmodal native status only; never changes the original menu artwork.
    bool setTitle(const char*,WindowHostDiagnostic50DQ&) noexcept;
    // Sends only to the actual opened SDL instance. A missing/disconnected/
    // non-rumble device is an explicit receipt, never a claimed submission.
    // Source motor bytes map small(0/1) to high(0/65535), large(0..255) to
    // low(byte*257). A100ms renewable safety lease prevents stuck motors if
    // the host stops polling; source duration timers remain owned by gameplay.
    bool rumbleController50KZ(std::uint32_t slot,std::int32_t expectedInstance,
        std::uint8_t small,std::uint8_t large,ControllerRumbleReceipt50KZ&,
        WindowHostDiagnostic50DQ&) noexcept;
    // A failed best-effort final motor stop is reported in cleanupError while
    // resource teardown continues; it does not turn destruction into terminate.
    bool close(WindowHostDiagnostic50DQ&) noexcept;
    bool created() const noexcept;
    std::uint32_t nativeWindowId50DU()const noexcept;
    bool ownsResources() const noexcept;
    bool current() const noexcept;
    WindowGlProc50DQ procAddress(const char* name) const noexcept;
    const WindowHostInfo50DQ& info() const noexcept { return info_; }
private:
    struct State;
    bool ownerThread() const noexcept;
    bool closeOwned(WindowHostDiagnostic50DQ&) noexcept;
    void refreshWindowInfo() noexcept;
    const std::thread::id thread_;
    State* state_ = nullptr;
    WindowHostInfo50DQ info_{};
};
}
