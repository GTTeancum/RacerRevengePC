#pragma once
#include "window_host50dq.h"
#include "offscreen_gl_context50dc.h" // Shared request/diagnostic types; no Windows EGL owner.
#include <array>
#include <cstddef>
#include <cstdint>
#include <thread>

namespace rr::platform {
enum class GameHostMode50ER : std::uint8_t { Unspecified, Window, HeadlessLive };
inline constexpr std::size_t gameHostInputByteBudget50ER = 1024;
inline constexpr std::size_t gameHostCommandCapacity50ER = 64;
struct GameHostRequest50ER {
    GameHostMode50ER mode = GameHostMode50ER::Unspecified;
    // Only the explicitly selected request is used. There is no fallback from
    // a failed window to headless, or from a failed EGL platform to another.
    WindowHostRequest50DQ window{};
    OffscreenGlRequest50DC headless{};
};
struct GameHostInfo50ER {
    std::int32_t width = 0, height = 0, drawableWidth = 0, drawableHeight = 0;
    // In HeadlessLive, focused means its explicit stdin control stream remains
    // connected. It does not assert that a window or OS focus exists.
    bool focused = false, minimized = false, inputConnected = false;
    std::int32_t swapInterval=0; // Actual queried native window setting; headless stays0.
};
struct GameHostInput50ER {
    WindowInput50DQ keys{};
    // Per-poll request only. The client must latch this until a successful
    // real game render commit can be captured. Multiple requests coalesce.
    bool captureRequested = false;
    // Host-only full-session restart request: F5 press or stdin restart.
    // Per-poll, never a native vehicle reposition action; quit takes priority.
    bool restartRequested = false;
    // Host session navigation only, never native vehicle action-button bits.
    // Window Escape is consumed by the client as a fresh toggle edge. These
    // flags expose explicit stdin requests; window close/EOF still means quit.
    bool pauseRequested = false, resumeRequested = false, menuRequested = false;
    // Host-only checkpoint reset request: non-repeated Backspace press or
    // stdin checkpoint. Per-poll, coalesced, never a native action-button bit.
    // Quit/Escape/menu takes priority; the client owns Ready/ground admission.
    bool checkpointRequested = false;
    // EOF/read error is a successful terminal input snapshot: all keys are
    // released and quitRequested is latched. Nonzero inputError is errno.
    bool inputEnded = false;
    int inputError = 0;
    std::uint32_t bytesRead = 0, commandsAccepted = 0, commandsRejected = 0;
};
enum class GameHostError50ER : std::uint8_t {
    None, WrongThread, InvalidMode, AlreadyOwned, NotCreated, NotWindow,
    Window, Headless, StdinFlags, UnsupportedMode
};
struct GameHostDiagnostic50ER {
    GameHostError50ER error = GameHostError50ER::None;
    const char* operation = nullptr;
    int systemError = 0;
    WindowHostDiagnostic50DQ window{};
    OffscreenGlDiagnostic50DC headless{};
    GameHostError50ER cleanupError = GameHostError50ER::None;
    const char* cleanupOperation = nullptr;
    int cleanupSystemError = 0;
    WindowHostDiagnostic50DQ cleanupWindow{};
    OffscreenGlDiagnostic50DC cleanupHeadless{};
};
using GameHostGlProc50ER = void (*)();

// One existing GL owner: native Window on Linux/Windows x64, or explicitly
// selected live stdin + offscreen EGL on Linux only. Windows HeadlessLive
// requests fail with UnsupportedMode before acquiring any resources.
// This facade never renders, advances physics, synthesizes sampled native
// input, captures images, or claims a headless render was a window swap.
// Construct/use/destroy on one thread (the main thread for Window mode).
// HeadlessLive exclusively borrows stdin and its open-file-description flags;
// no concurrent reader, flag mutation, close, or descriptor replacement is
// supported. It enables O_NONBLOCK and restores original flags on close.
// It does not change terminal mode; a terminal sends commands after Enter.
class GameHost50ER final {
public:
    GameHost50ER() noexcept;
    ~GameHost50ER() noexcept;
    GameHost50ER(const GameHost50ER&) = delete;
    GameHost50ER& operator=(const GameHost50ER&) = delete;
    GameHost50ER(GameHost50ER&&) = delete;
    GameHost50ER& operator=(GameHost50ER&&) = delete;

    bool create(const GameHostRequest50ER&, GameHostDiagnostic50ER&) noexcept;
    bool bind(GameHostDiagnostic50ER&) noexcept;
    bool release(GameHostDiagnostic50ER&) noexcept;
    // HeadlessLive: at most one nonblocking read and 1024 bytes per call;
    // 63 command bytes plus terminator, fixed storage, no heap allocation.
    // Commands are ASCII, newline-delimited and case-insensitive:
    // down KEY | up KEY | release | capture | restart | checkpoint | quit
    // pause | resume | menu
    // KEY is W, S, Up, Down, R, Space, Shift (Left Shift), Escape, Enter.
    // down/up set raw SDL physical scancodes; Enter selects the host menu row.
    // Invalid/overlong lines are rejected whole without changing key state.
    // A partial line survives until newline; EOF discards it and quits safely.
    // Like 50DQ, down is the latest drained state. A down/up pair in one poll
    // is NOT turned into a native button pulse or queued for a later tick.
    bool poll(GameHostInput50ER&, GameHostDiagnostic50ER&) noexcept;
    // Window mode only. HeadlessLive explicitly rejects this operation.
    // Renderer submit/commit is separate and never performed by this facade.
    bool swapWindow(GameHostDiagnostic50ER&) noexcept;
    bool setSwapInterval(std::int32_t,GameHostDiagnostic50ER&) noexcept;
    bool close(GameHostDiagnostic50ER&) noexcept;
    bool created() const noexcept;
    bool ownsResources() const noexcept;
    bool current() const noexcept;
    GameHostGlProc50ER procAddress(const char*) const noexcept;
    GameHostMode50ER mode() const noexcept { return mode_; }
    bool hasWindow() const noexcept { return mode_ == GameHostMode50ER::Window; }
    const GameHostInfo50ER& info() const noexcept { return info_; }
private:
    bool ownerThread() const noexcept;
    bool closeOwned(GameHostDiagnostic50ER&) noexcept;
    void refreshInfo() noexcept;
#if defined(__linux__)
    void releaseKeys(WindowInput50DQ&) noexcept;
    void consumeCommand(GameHostInput50ER&) noexcept;
    void resetInput() noexcept;
#endif
    const std::thread::id thread_;
    WindowHost50DQ window_;
#if defined(__linux__)
    OffscreenGlContext50DC headless_;
#endif
    GameHostMode50ER mode_ = GameHostMode50ER::Unspecified;
    GameHostInfo50ER info_{};
    bool ready_ = false;
#if defined(__linux__)
    bool restoreStdinFlags_ = false;
    int originalStdinFlags_ = 0;
    std::array<std::uint8_t, windowScancodeCount50DQ> down_{};
    std::array<char, gameHostCommandCapacity50ER> command_{};
    std::size_t commandLength_ = 0;
    bool rejectCommand_ = false, quitRequested_ = false, inputEnded_ = false;
    int inputError_ = 0;
#endif
};
}
