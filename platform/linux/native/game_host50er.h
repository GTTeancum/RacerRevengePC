#pragma once
#include "window_host50dq.h"
#include "original_results_host_input50la.h"
#include "offscreen_gl_context50dc.h" // Shared request/diagnostic types; no Windows EGL owner.
#include <array>
#include <cstddef>
#include <cstdint>
#include <thread>

namespace original_main_entry50KG { class RaceDisplayLifetime50LA; struct Application; }
namespace interactive_loop50DU { struct Context; }
namespace rr::platform {
class VehicleWindowGl50DS;
class OriginalUiSharedDialogs50KG;
class OriginalSpectatorMenuInput50LA;
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
    // Main-thread loading pump: consumes real native events and latches Close,
    // but publishes no frontend or gameplay input. The next ordinary poll is
    // release-to-arm so controls pressed during loading cannot leak through.
    bool pollLoading50DU(bool& quitRequested,GameHostDiagnostic50ER&)noexcept;
    // Inter-session native GL baseline only after the old Display has died.
    bool prepareRetainedPlayerSession50DU(GameHostDiagnostic50ER&)noexcept;
    // Window mode only. HeadlessLive explicitly rejects this operation.
    // Renderer submit/commit is separate and never performed by this facade.
    bool swapWindow(GameHostDiagnostic50ER&) noexcept;
    bool setSwapInterval(std::int32_t,GameHostDiagnostic50ER&) noexcept;
    bool setWindowTitle(const char*,GameHostDiagnostic50ER&) noexcept;
    bool rumbleController50KZ(std::uint32_t slot,std::int32_t expectedInstance,
        std::uint8_t small,std::uint8_t large,ControllerRumbleReceipt50KZ&,
        GameHostDiagnostic50ER&) noexcept;
    bool close(GameHostDiagnostic50ER&) noexcept;
    bool created() const noexcept;
    bool ownsResources() const noexcept;
    bool current() const noexcept;
    GameHostGlProc50ER procAddress(const char*) const noexcept;
    GameHostMode50ER mode() const noexcept { return mode_; }
    bool hasWindow() const noexcept { return mode_ == GameHostMode50ER::Window; }
    std::uint32_t nativeWindowId50DU()const noexcept{return window_.nativeWindowId50DU();}
    const GameHostInfo50ER& info() const noexcept { return info_; }
private:
    std::shared_ptr<OriginalSourcePadHostLease50LA> bindSourcePadNative(
        const original_main_entry50KG::Application&,const rr::game::OriginalApplicationInput04Source50LA&)noexcept;
    friend class ::original_main_entry50KG::RaceDisplayLifetime50LA;
    friend class OriginalUiSharedDialogs50KG;
    friend struct ::original_main_entry50KG::Application;
    friend struct ::interactive_loop50DU::Context;
    friend class rr::game::OriginalResults10450LA;
    friend class OriginalSpectatorMenuInput50LA;
    // Separate from Results retirement: the native ALL8 Pause branch runs
    // later in this same actual poll. Only its lexical producer may consume
    // this pointer and retain the stable event/record batch. No snapshot API.
    const GameHostInput50ER* spectatorPollDestination50LA_=nullptr;
    const OriginalSpectatorMenuInput50LA* spectatorBatchOwner50LA_=nullptr;
    OriginalResultsHostInput50LA resultsInput50LA_;
    // End the one fresh-poll dispatch window before existing native clients.
    // Availability remains current; retired edges can never be replayed later.
    GameHostInput50ER* resultsPollDestination50LA_=nullptr; //Synchronous poll-to-retire borrow only.
    // Explicit transfer of the producer's SAME events/records storage to a
    // suspended Results call. No copy is reissued as fresh input. Uncalled
    // until the complete fade/postfade continuation is integrated.
    const rr::game::OriginalResults10450LA* retainedResultsPollOwner50LA_=nullptr;
    std::uint64_t retainedResultsPollRevision50LA_=0;
    std::size_t retainedResultsPollEvent50LA_=0;
    // Written only at the actual Results dispatch claim/current-event cuts.
    // The retain helper cannot choose another issuer or fabricate an ordinal.
    const rr::game::OriginalResults10450LA* claimedResultsOwner50LA_=nullptr;
    const rr::game::InputEvent48Z* claimedResultsEvent50LA_=nullptr;
    bool retainedResultsPollCurrent50LA(const rr::game::OriginalResults10450LA&)const noexcept;
    bool retainResultsPoll50LA(const rr::game::OriginalResults10450LA&)noexcept;
    bool releaseResultsPoll50LA(const rr::game::OriginalResults10450LA&)noexcept;
    bool appendResultsHideSamePoll50LA(const rr::game::OriginalResults10450LA& owner)noexcept {
        return ownerThread()&&created()&&hasWindow()&&!retainedResultsPollOwner50LA_&&
            !claimedResultsOwner50LA_&&resultsPollDestination50LA_&&
            resultsInput50LA_.appendResultsHideSamePoll(owner);
    }
    bool retireResultsEdges50LA(GameHostInput50ER& actual)noexcept {
        if(!ownerThread())return false; //Immutable fence before mutable poll custody.
        // Handoff already retired these controls from this exact output.
        // Consume its synchronous pointer once, NOT the retained event batch.
        if(retainedResultsPollOwner50LA_){
            if(resultsPollDestination50LA_!=&actual)return false;
            resultsPollDestination50LA_=nullptr;return true;
        }
        if(resultsPollDestination50LA_!=&actual){
            resultsInput50LA_.edgesOpen_=false;resultsPollDestination50LA_=nullptr;return false;
        }
        const bool claimed=resultsInput50LA_.fixedUiCurrent()&&
            resultsInput50LA_.claimedRevision_==resultsInput50LA_.revision_;
        resultsInput50LA_.retireClaimedControls(actual.keys);
        claimedResultsOwner50LA_=nullptr;claimedResultsEvent50LA_=nullptr;
        resultsPollDestination50LA_=nullptr;return claimed;
    }
    // Native global UI470 consumes the actual successful input poll, not a
    // caller-supplied controller-presence assertion. This owner is independent
    // of any menu generation; close/failure invalidates the observation.
    std::uint64_t sharedDialogPollRevision50KG_=0;
    bool sharedDialogPollCurrent50KG_=false;
    bool sharedDialogPrimaryPresent50KG_=false;
    WindowControllerPresence50DQ sharedDialogSecondPresence50KG_=WindowControllerPresence50DQ::Unknown;
    void retainSharedDialogPoll50KG(const GameHostInput50ER&)noexcept;
    const VehicleWindowGl50DS* sessionDisplayRenderer50LA_=nullptr;
    std::uint32_t sessionDependentPins50LA_=0;
    bool sessionDisplayBlocksClose50LA() const noexcept;
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
    bool loadingInputBarrier50DU_=false;
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
