#include "test_results_input50pd.h"
#include "player_session_gl_reset50du.h"
#include "game_host50er.h"
#include "original_spectator_menu_input50la.h"
#include "vehicle_window_gl50ds.h"
#include <exception>

#if defined(__linux__)
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#elif defined(_WIN32) && (defined(_M_X64) || defined(__x86_64__))
// Windows x64 uses WindowHost50DQ's SDL2/native desktop OpenGL path only.
#else
#error "GameHost50ER supports Linux and windowed Windows x64 only"
#endif

namespace rr::platform {
namespace {
bool fail(GameHostDiagnostic50ER& d, GameHostError50ER error,
    const char* operation, int systemError = 0) noexcept {
    d.error = error;
    d.operation = operation;
    d.systemError = systemError;
    return false;
}
#if defined(__linux__)
struct Token { const char* bytes = nullptr; std::size_t count = 0; };
bool matches(Token token, const char* lower) noexcept {
    std::size_t i = 0;
    for (; i < token.count && lower[i]; ++i) {
        char c = token.bytes[i];
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + ('a' - 'A'));
        if (c != lower[i]) return false;
    }
    return i == token.count && !lower[i];
}
bool space(char c) noexcept { return c == ' ' || c == '\t' || c == '\r'; }
std::uint16_t scancode(Token key) noexcept {
    if (matches(key, "w")) return 26;
    if (matches(key, "s")) return 22;
    if (matches(key, "up")) return 82;
    if (matches(key, "down")) return 81;
    if (matches(key, "left")) return 80;
    if (matches(key, "right")) return 79;
    if (matches(key, "r")) return 21;
    if (matches(key, "space")) return 44;
    if (matches(key, "shift") || matches(key, "lshift") || matches(key, "leftshift"))
        return 225; // SDL physical Left Shift; same key as the windowed host.
    if (matches(key, "escape")) return 41;
    if (matches(key, "enter") || matches(key, "return")) return 40;
    return 0;
}
#endif
void copyCleanup(GameHostDiagnostic50ER& d, const GameHostDiagnostic50ER& c) noexcept {
    d.cleanupError = c.error;
    d.cleanupOperation = c.operation;
    d.cleanupSystemError = c.systemError;
    d.cleanupWindow = c.window;
    d.cleanupHeadless = c.headless;
}
}

GameHost50ER::GameHost50ER() noexcept : thread_(std::this_thread::get_id()) {}
GameHost50ER::~GameHost50ER() noexcept {
    GameHostDiagnostic50ER d;
    if (!ownerThread() || !closeOwned(d)) std::terminate();
}
bool GameHost50ER::sessionDisplayBlocksClose50LA() const noexcept {
    return sessionDependentPins50LA_ ||
        (sessionDisplayRenderer50LA_&&sessionDisplayRenderer50LA_->ownsResources());
}
bool GameHost50ER::ownerThread() const noexcept {
    return thread_ == std::this_thread::get_id();
}
bool GameHost50ER::created() const noexcept {
    if (!ownerThread() || !ready_) return false;
    if (mode_ == GameHostMode50ER::Window) return window_.created();
#if defined(__linux__)
    return mode_ == GameHostMode50ER::HeadlessLive && headless_.created();
#else
    return false;
#endif
}
bool GameHost50ER::ownsResources() const noexcept {
    return ownerThread() && (window_.ownsResources()
#if defined(__linux__)
        || headless_.ownsResources() || restoreStdinFlags_
#endif
    );
}
bool GameHost50ER::current() const noexcept {
    if (!created()) return false;
    if (mode_ == GameHostMode50ER::Window) return window_.current();
#if defined(__linux__)
    return headless_.current();
#else
    return false;
#endif
}
GameHostGlProc50ER GameHost50ER::procAddress(const char* name) const noexcept {
    if (!current()) return nullptr;
    if (mode_ == GameHostMode50ER::Window) return window_.procAddress(name);
#if defined(__linux__)
    return headless_.procAddress(name);
#else
    return nullptr;
#endif
}
void GameHost50ER::refreshInfo() noexcept {
    if (mode_ == GameHostMode50ER::Window) {
        const auto& source = window_.info();
        info_ = {source.width, source.height, source.drawableWidth, source.drawableHeight,
            source.focused, source.minimized, window_.created(),source.swapInterval};
#if defined(__linux__)
    } else if (mode_ == GameHostMode50ER::HeadlessLive) {
        const auto& source = headless_.info();
        const bool connected = ready_ && !inputEnded_ && !quitRequested_;
        info_ = {source.width, source.height, source.width, source.height,
            connected, false, connected};
#endif
    } else info_ = {};
}
#if defined(__linux__)
void GameHost50ER::resetInput() noexcept {
    down_.fill(0);
    command_.fill(0);
    commandLength_ = 0;
    rejectCommand_ = quitRequested_ = inputEnded_ = false;
    inputError_ = 0;
}

#endif

bool GameHost50ER::create(const GameHostRequest50ER& r, GameHostDiagnostic50ER& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, GameHostError50ER::WrongThread, "create");
    if(sessionDisplayBlocksClose50LA())return fail(d,GameHostError50ER::AlreadyOwned,"session renderer/context still retained");
    if (ready_ || ownsResources()) return fail(d, GameHostError50ER::AlreadyOwned, "create");
    if (r.mode != GameHostMode50ER::Window && r.mode != GameHostMode50ER::HeadlessLive)
        return fail(d, GameHostError50ER::InvalidMode, "create/explicit mode required");
#if defined(_WIN32)
    if (r.mode == GameHostMode50ER::HeadlessLive)
        return fail(d, GameHostError50ER::UnsupportedMode,
            "create/HeadlessLive is unsupported on Windows; select Window");
#endif
    mode_ = r.mode;
#if defined(__linux__)
    resetInput();
#endif
    const auto cleanup = [&]() noexcept {
        GameHostDiagnostic50ER c;
        if (!closeOwned(c)) copyCleanup(d, c);
        return false;
    };
    if (mode_ == GameHostMode50ER::Window) {
        if (!window_.create(r.window, d.window)) {
            fail(d, GameHostError50ER::Window, "WindowHost50DQ::create");
            return cleanup();
        }
    }
#if defined(__linux__)
    else {
        originalStdinFlags_ = ::fcntl(STDIN_FILENO, F_GETFL);
        if (originalStdinFlags_ < 0) {
            fail(d, GameHostError50ER::StdinFlags, "fcntl(stdin,F_GETFL)", errno);
            return cleanup();
        }
        if (!(originalStdinFlags_ & O_NONBLOCK)) {
            if (::fcntl(STDIN_FILENO, F_SETFL, originalStdinFlags_ | O_NONBLOCK) < 0) {
                fail(d, GameHostError50ER::StdinFlags, "fcntl(stdin,F_SETFL,O_NONBLOCK)", errno);
                return cleanup();
            }
            restoreStdinFlags_ = true;
        }
        if (!headless_.create(r.headless, d.headless)) {
            fail(d, GameHostError50ER::Headless, "OffscreenGlContext50DC::create");
            return cleanup();
        }
    }
#endif
    ready_ = true;
    refreshInfo();
    return true;
}
bool GameHost50ER::bind(GameHostDiagnostic50ER& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, GameHostError50ER::WrongThread, "bind");
    if (!created()) return fail(d, GameHostError50ER::NotCreated, "bind");
    if (mode_ == GameHostMode50ER::Window)
        return window_.bind(d.window) || fail(d, GameHostError50ER::Window, "WindowHost50DQ::bind");
#if defined(__linux__)
    return headless_.bind(d.headless) || fail(d, GameHostError50ER::Headless, "OffscreenGlContext50DC::bind");
#else
    return fail(d, GameHostError50ER::UnsupportedMode, "bind/HeadlessLive is unsupported on Windows");
#endif
}
bool GameHost50ER::release(GameHostDiagnostic50ER& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, GameHostError50ER::WrongThread, "release");
    if(sessionDisplayBlocksClose50LA())return fail(d,GameHostError50ER::AlreadyOwned,"session renderer/context still retained");
    if (!created()) return fail(d, GameHostError50ER::NotCreated, "release");
    if (mode_ == GameHostMode50ER::Window)
        return window_.release(d.window) || fail(d, GameHostError50ER::Window, "WindowHost50DQ::release");
#if defined(__linux__)
    return headless_.release(d.headless) || fail(d, GameHostError50ER::Headless, "OffscreenGlContext50DC::release");
#else
    return fail(d, GameHostError50ER::UnsupportedMode, "release/HeadlessLive is unsupported on Windows");
#endif
}
#if defined(__linux__)
void GameHost50ER::releaseKeys(WindowInput50DQ& keys) noexcept {
    for (std::size_t key = 0; key < down_.size(); ++key) {
        if (down_[key]) keys.released[key] = 1;
        down_[key] = 0;
    }
}
void GameHost50ER::consumeCommand(GameHostInput50ER& input) noexcept {
    Token tokens[3]{};
    std::size_t position = 0, count = 0;
    while (position < commandLength_) {
        while (position < commandLength_ && space(command_[position])) ++position;
        if (position == commandLength_) break;
        if (count == 3) { rejectCommand_ = true; break; }
        const auto begin = position;
        while (position < commandLength_ && !space(command_[position])) ++position;
        tokens[count++] = {command_.data() + begin, position - begin};
    }
    if (rejectCommand_) { ++input.commandsRejected; return; }
    if (!count) return;
    if (count == 1 && matches(tokens[0], "release")) releaseKeys(input.keys);
    else if (count == 1 && matches(tokens[0], "capture")) input.captureRequested = true;
    else if (count == 1 && matches(tokens[0], "restart")) {
        releaseKeys(input.keys);
        input.restartRequested = true;
    } else if (count == 1 && matches(tokens[0], "checkpoint")) {
        releaseKeys(input.keys);
        input.checkpointRequested = true;
    } else if (count == 1 && matches(tokens[0], "pause")) {
        releaseKeys(input.keys);
        input.pauseRequested = true;
    } else if (count == 1 && matches(tokens[0], "resume")) {
        releaseKeys(input.keys);
        input.resumeRequested = true;
    } else if (count == 1 && matches(tokens[0], "menu")) {
        releaseKeys(input.keys);
        input.menuRequested = true;
    } else if (count == 1 && matches(tokens[0], "quit")) {
        releaseKeys(input.keys);
        quitRequested_ = true;
    } else if (count == 2 && (matches(tokens[0], "down") || matches(tokens[0], "up"))) {
        const auto key = scancode(tokens[1]);
        if (!key) { ++input.commandsRejected; return; }
        const bool held = matches(tokens[0], "down");
        if (held && !down_[key]) input.keys.pressed[key] = 1;
        if (!held && down_[key]) input.keys.released[key] = 1;
        down_[key] = held ? 1 : 0;
    } else { ++input.commandsRejected; return; }
    ++input.commandsAccepted;
}
#endif

void GameHost50ER::retainSharedDialogPoll50KG(const GameHostInput50ER& input)noexcept {
    // Window mode means the actual SDL keyboard route exists. In explicit
    // headless mode this is the actual retained stdin stream. Focus loss is
    // not hardware removal; no gamepad is fabricated from keyboard activity.
    if(sharedDialogPollRevision50KG_==UINT64_MAX){sharedDialogPollCurrent50KG_=false;return;}
    ++sharedDialogPollRevision50KG_;
    sharedDialogPrimaryPresent50KG_=info_.inputConnected;
    sharedDialogSecondPresence50KG_=input.keys.secondControllerPresence;
    sharedDialogPollCurrent50KG_=true;
    spectatorPollDestination50LA_=&input;
}
bool GameHost50ER::retainedResultsPollCurrent50LA(const rr::game::OriginalResults10450LA& owner)const noexcept {
    return ownerThread()&&created()&&hasWindow()&&retainedResultsPollOwner50LA_==&owner&&
        retainedResultsPollRevision50LA_&&resultsInput50LA_.fixedUiCurrent()&&
        resultsInput50LA_.revision_==retainedResultsPollRevision50LA_&&
        resultsInput50LA_.claimedRevision_==retainedResultsPollRevision50LA_&&
        retainedResultsPollEvent50LA_<resultsInput50LA_.count_&&
        claimedResultsOwner50LA_==&owner&&
        claimedResultsEvent50LA_==&resultsInput50LA_.events_[retainedResultsPollEvent50LA_].event;
}
bool GameHost50ER::retainResultsPoll50LA(const rr::game::OriginalResults10450LA& owner)noexcept {
    if(!ownerThread()||!created()||!hasWindow()||retainedResultsPollOwner50LA_||
       !resultsPollDestination50LA_||!resultsInput50LA_.fixedUiCurrent()||
       resultsInput50LA_.edgesOpen_||resultsInput50LA_.claimedRevision_!=resultsInput50LA_.revision_||
       claimedResultsOwner50LA_!=&owner||!claimedResultsEvent50LA_)return false;
    std::size_t event=0;
    while(event<resultsInput50LA_.count_&&
          &resultsInput50LA_.events_[event].event!=claimedResultsEvent50LA_)++event;
    if(event==resultsInput50LA_.count_)return false;
    // Claim was issued by the real synchronous Results dispatcher. Retire
    // only its native UI controls now; events and records remain in place.
    resultsInput50LA_.retireClaimedControls(resultsPollDestination50LA_->keys);
    retainedResultsPollOwner50LA_=&owner;
    retainedResultsPollRevision50LA_=resultsInput50LA_.revision_;
    retainedResultsPollEvent50LA_=event;return true;
}
bool GameHost50ER::releaseResultsPoll50LA(const rr::game::OriginalResults10450LA& owner)noexcept {
    if(!ownerThread()||retainedResultsPollOwner50LA_!=&owner)return false;
    // Completion OR explicit whole-call cancellation only. The lexical
    // caller must first retire the real fade backend and its borrowed owners.
    // Releasing never claims that source effects completed or rolled back.
    retainedResultsPollOwner50LA_=nullptr;retainedResultsPollRevision50LA_=0;
    retainedResultsPollEvent50LA_=0;resultsPollDestination50LA_=nullptr;
    claimedResultsOwner50LA_=nullptr;claimedResultsEvent50LA_=nullptr;
    resultsInput50LA_.interrupt();loadingInputBarrier50DU_=true;return true;
}
bool GameHost50ER::poll(GameHostInput50ER& input, GameHostDiagnostic50ER& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, GameHostError50ER::WrongThread, "poll");
    if (!created()) return fail(d, GameHostError50ER::NotCreated, "poll");
    if(retainedResultsPollOwner50LA_)
        return fail(d,GameHostError50ER::AlreadyOwned,"Results retained source poll pending");
    if(spectatorBatchOwner50LA_)
        return fail(d,GameHostError50ER::AlreadyOwned,"Pause native input batch requires retirement");
    spectatorPollDestination50LA_=nullptr;
    sharedDialogPollCurrent50KG_=false;
    resultsInput50LA_.invalidate();resultsPollDestination50LA_=nullptr;
    claimedResultsOwner50LA_=nullptr;claimedResultsEvent50LA_=nullptr;
    input = {};
    if (mode_ == GameHostMode50ER::Window) {
        if (!window_.poll(input.keys, d.window)) {
            resultsInput50LA_.interrupt();resultsPollDestination50LA_=nullptr;
            return fail(d, GameHostError50ER::Window, "WindowHost50DQ::poll");
        }
        const bool resultsLoadingBarrier=loadingInputBarrier50DU_;
        if(loadingInputBarrier50DU_){
            bool held=input.keys.eventBudgetExhausted;
            for(const auto key:input.keys.down)held=held||key!=0;
            for(const auto key:input.keys.pressed)held=held||key!=0;
            for(const auto& controller:input.keys.controllers)if(controller.connected){
                for(const auto button:controller.buttons)held=held||button!=0;
                for(const auto button:controller.pressed)held=held||button!=0;
                for(const auto axis:controller.axes)held=held||axis>4096||axis< -4096;
            }
            loadingInputBarrier50DU_=held;
            input.keys.down.fill(0);input.keys.pressed.fill(0);input.keys.released.fill(0);
            for(auto& controller:input.keys.controllers){
                controller.axes.fill(0);controller.buttons.fill(0);
                controller.pressed.fill(0);controller.released.fill(0);
            }
        }
        input.restartRequested = input.keys.pressed[62] != 0; // SDL physical F5
        // 50DQ emits pressed only on a fresh, non-repeat key-down edge.
        // Never use down here: holding Backspace must not request another reset.
        input.checkpointRequested = input.keys.pressed[42] != 0 && // SDL Backspace
            !input.keys.quitRequested && !input.keys.pressed[41] && !input.keys.down[41];
        refreshInfo();
        retainSharedDialogPoll50KG(input);
        {auto& test=testResultsInput50PD(); //Unattended test only; inert otherwise.
         if(test.focus){input.keys.focused=true;input.keys.activityLost=false;}
         if(test.enter){input.keys.pressed[40]=1;input.keys.down[40]=1;test.enter=false;}}
        resultsInput50LA_.observeWindow(input.keys,info_.inputConnected,resultsLoadingBarrier);
        resultsPollDestination50LA_=&input;
        return true;
    }
#if defined(__linux__)
    if (!quitRequested_) {
        std::array<char, gameHostInputByteBudget50ER> bytes{};
        const auto received = ::read(STDIN_FILENO, bytes.data(), bytes.size());
        if (received > 0) {
            input.bytesRead = static_cast<std::uint32_t>(received);
            // Conservative, matching 50DQ: consuming the whole byte budget
            // means more input MAY remain. No unbounded drain or second read.
            input.keys.eventBudgetExhausted = static_cast<std::size_t>(received) == bytes.size();
            for (std::size_t i = 0; i < static_cast<std::size_t>(received); ++i) {
                const char c = bytes[i];
                if (c == '\n') {
                    consumeCommand(input);
                    commandLength_ = 0;
                    rejectCommand_ = false;
                    if (quitRequested_) break;
                } else if (!rejectCommand_) {
                    if ((c < ' ' && c != '\t' && c != '\r') ||
                        static_cast<unsigned char>(c) > 126 ||
                        commandLength_ + 1 >= command_.size()) rejectCommand_ = true;
                    else command_[commandLength_++] = c;
                }
            }
        } else if (received == 0 || (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)) {
            inputError_ = received < 0 ? errno : 0;
            inputEnded_ = quitRequested_ = true;
            if (commandLength_ || rejectCommand_) ++input.commandsRejected;
            commandLength_ = 0;
            rejectCommand_ = false;
            releaseKeys(input.keys);
        }
    }
    input.keys.down = down_;
    input.keys.eventsProcessed = input.commandsAccepted + input.commandsRejected;
    input.keys.quitRequested = quitRequested_;
    if (input.keys.quitRequested || input.keys.pressed[41] || input.keys.down[41] ||
        input.menuRequested || input.pauseRequested)
        input.checkpointRequested = false;
    input.inputEnded = inputEnded_;
    input.inputError = inputError_;
    refreshInfo();
    input.keys.focused = info_.focused;
    input.keys.minimized = info_.minimized;
    retainSharedDialogPoll50KG(input);
    return true;
#else
    return fail(d, GameHostError50ER::UnsupportedMode, "poll/HeadlessLive is unsupported on Windows");
#endif
}
bool GameHost50ER::prepareRetainedPlayerSession50DU(GameHostDiagnostic50ER& d)noexcept {
    d={};
    if(!ownerThread())return fail(d,GameHostError50ER::WrongThread,"preparePlayerSession");
    if(!created()||!current()||!hasWindow())
        return fail(d,GameHostError50ER::NotWindow,"preparePlayerSession/current window required");
    if(sessionDisplayRenderer50LA_||sessionDependentPins50LA_)
        return fail(d,GameHostError50ER::AlreadyOwned,"preparePlayerSession/previous display still retained");
    if(!setSwapInterval(0,d))return false;
    if(!resetPlayerSessionGl50DU(*this))
        return fail(d,GameHostError50ER::Window,"preparePlayerSession/GL baseline rejected");
    return true;
}
bool GameHost50ER::pollLoading50DU(bool& quitRequested,GameHostDiagnostic50ER& d)noexcept {
    d={};quitRequested=false;
    if(!ownerThread())return fail(d,GameHostError50ER::WrongThread,"pollLoading");
    if(!created())return fail(d,GameHostError50ER::NotCreated,"pollLoading");
    if(!hasWindow())return fail(d,GameHostError50ER::NotWindow,"pollLoading");
    if(spectatorBatchOwner50LA_&&
       (spectatorBatchOwner50LA_->phase_!=OriginalSpectatorMenuInput50LA::Phase::suspended||
        !spectatorBatchOwner50LA_->retainedForPauseReturn_||
        !spectatorBatchOwner50LA_->currentPoll()))
        return fail(d,GameHostError50ER::AlreadyOwned,"Pause input requires its actual retained continuation before loading poll");
    spectatorPollDestination50LA_=nullptr;
    if(retainedResultsPollOwner50LA_&&resultsPollDestination50LA_)
        return fail(d,GameHostError50ER::AlreadyOwned,"Results handoff requires synchronous output retirement first");
    sharedDialogPollCurrent50KG_=false;
    if(!retainedResultsPollOwner50LA_){
        resultsInput50LA_.interrupt();claimedResultsOwner50LA_=nullptr;claimedResultsEvent50LA_=nullptr;
    }
    resultsPollDestination50LA_=nullptr;
    WindowInput50DQ discarded;
    if(!window_.poll(discarded,d.window))
        return fail(d,GameHostError50ER::Window,"WindowHost50DQ::pollLoading");
    refreshInfo();loadingInputBarrier50DU_=true;
    quitRequested=discarded.quitRequested;
    return true;
}
bool GameHost50ER::swapWindow(GameHostDiagnostic50ER& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, GameHostError50ER::WrongThread, "swapWindow");
    if (!created()) return fail(d, GameHostError50ER::NotCreated, "swapWindow");
    if (mode_ != GameHostMode50ER::Window)
        return fail(d, GameHostError50ER::NotWindow, "swapWindow/headless has no window swap");
    return window_.swap(d.window) || fail(d, GameHostError50ER::Window, "WindowHost50DQ::swap");
}
bool GameHost50ER::setWindowTitle(const char* title,GameHostDiagnostic50ER& d) noexcept {
    d={};
    if(!ownerThread())return fail(d,GameHostError50ER::WrongThread,"setWindowTitle");
    if(!created()||mode_!=GameHostMode50ER::Window)return fail(d,GameHostError50ER::NotWindow,"setWindowTitle");
    return window_.setTitle(title,d.window)||fail(d,GameHostError50ER::Window,"WindowHost50DQ::setTitle");
}
bool GameHost50ER::setSwapInterval(std::int32_t interval,GameHostDiagnostic50ER& d) noexcept {
    d={};
    if(!ownerThread())return fail(d,GameHostError50ER::WrongThread,"setSwapInterval");
    if(!created()||mode_!=GameHostMode50ER::Window)return fail(d,GameHostError50ER::NotWindow,"setSwapInterval");
    const bool accepted=window_.setSwapInterval(interval,d.window);refreshInfo();
    return accepted||fail(d,GameHostError50ER::Window,"WindowHost50DQ::setSwapInterval");
}
bool GameHost50ER::rumbleController50KZ(std::uint32_t slot,std::int32_t instance,
    std::uint8_t small,std::uint8_t large,ControllerRumbleReceipt50KZ& receipt,
    GameHostDiagnostic50ER& d) noexcept {
    d={};receipt={};
    if(!ownerThread())return fail(d,GameHostError50ER::WrongThread,"rumbleController50KZ");
    if(!created())return fail(d,GameHostError50ER::NotCreated,"rumbleController50KZ");
    if(mode_!=GameHostMode50ER::Window)return true; // Genuine no physical host device.
    if(!window_.rumbleController50KZ(slot,instance,small,large,receipt,d.window))
        return fail(d,GameHostError50ER::Window,"WindowHost50DQ::rumbleController50KZ");
    return true;
}
std::shared_ptr<OriginalSourcePadHostLease50LA> GameHost50ER::bindSourcePadNative(
    const original_main_entry50KG::Application& app,const rr::game::OriginalApplicationInput04Source50LA& source)noexcept {
    if(!ownerThread()||!ready_||mode_!=GameHostMode50ER::Window)return {};
    return window_.bindSourcePadNative(app,source,*this);
}
bool GameHost50ER::close(GameHostDiagnostic50ER& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, GameHostError50ER::WrongThread, "close");
    return closeOwned(d);
}
bool GameHost50ER::closeOwned(GameHostDiagnostic50ER& d) noexcept {
    if(spectatorBatchOwner50LA_)
        return fail(d,GameHostError50ER::AlreadyOwned,"Pause native input batch requires retirement or owner abandonment");
    spectatorPollDestination50LA_=nullptr;
    window_.revokeSourcePadNative();
    if(retainedResultsPollOwner50LA_)
        return fail(d,GameHostError50ER::AlreadyOwned,"Results retained poll requires explicit continuation cancellation");
    //Also revoke native input custody before ANY close attempt, including
    //a renderer-pinned rejection; this does not invalidate that GL context.
    resultsInput50LA_.interrupt();resultsPollDestination50LA_=nullptr;
    claimedResultsOwner50LA_=nullptr;claimedResultsEvent50LA_=nullptr;
    // Also reached directly by the destructor: never invalidate a retained context.
    if(sessionDisplayBlocksClose50LA())return fail(d,GameHostError50ER::AlreadyOwned,"session renderer/context still retained");
    ready_ = false;
    loadingInputBarrier50DU_=false;
    sharedDialogPollCurrent50KG_=false;
    bool okay = true;
    if (window_.ownsResources() && !window_.close(d.window))
        okay = fail(d, GameHostError50ER::Window, "WindowHost50DQ::close");
#if defined(__linux__)
    if (headless_.ownsResources() && !headless_.close(d.headless))
        okay = fail(d, GameHostError50ER::Headless, "OffscreenGlContext50DC::close");
    if (restoreStdinFlags_) {
        if (::fcntl(STDIN_FILENO, F_SETFL, originalStdinFlags_) < 0) {
            if (okay) fail(d, GameHostError50ER::StdinFlags, "fcntl(stdin,restore flags)", errno);
            else {
                d.cleanupError = GameHostError50ER::StdinFlags;
                d.cleanupOperation = "fcntl(stdin,restore flags)";
                d.cleanupSystemError = errno;
            }
            okay = false;
        } else restoreStdinFlags_ = false;
    }
#endif
    if (!ownsResources()) {
        mode_ = GameHostMode50ER::Unspecified;
        info_ = {};
#if defined(__linux__)
        resetInput();
#endif
    }
    return okay;
}
}
