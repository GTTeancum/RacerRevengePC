#include "window_host50dq.h"
#include <cstdio>
#include <cstring>
#include "native_dynamic_library50win.h"
#include <exception>
#include <new>
#if defined(_WIN32)
#include <SDL_events.h>
#include <SDL_gamecontroller.h>
#include <SDL_version.h>
#endif

namespace rr::platform {
namespace {
using Int = std::int32_t;
using UInt = std::uint32_t;
#if defined(_WIN32)
// The Windows build supplies the official SDL2 headers. Validate every public
// event offset used by the common memcpy decoder against those declarations.
using SdlWindow = SDL_Window;
using SdlVersion = SDL_version;
using SdlEvent = SDL_Event;
using SdlGameController = SDL_GameController;
using SdlJoystick = SDL_Joystick;
static_assert(SDL_CONTROLLER_AXIS_LEFTX==0&&SDL_CONTROLLER_AXIS_LEFTY==1&&
    SDL_CONTROLLER_AXIS_RIGHTX==2&&SDL_CONTROLLER_AXIS_RIGHTY==3&&
    SDL_CONTROLLER_AXIS_TRIGGERLEFT==4&&SDL_CONTROLLER_AXIS_TRIGGERRIGHT==5&&
    SDL_CONTROLLER_BUTTON_A==0&&SDL_CONTROLLER_BUTTON_B==1&&SDL_CONTROLLER_BUTTON_X==2&&
    SDL_CONTROLLER_BUTTON_Y==3&&SDL_CONTROLLER_BUTTON_BACK==4&&SDL_CONTROLLER_BUTTON_GUIDE==5&&
    SDL_CONTROLLER_BUTTON_START==6&&SDL_CONTROLLER_BUTTON_LEFTSTICK==7&&SDL_CONTROLLER_BUTTON_RIGHTSTICK==8&&
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER==9&&SDL_CONTROLLER_BUTTON_RIGHTSHOULDER==10&&
    SDL_CONTROLLER_BUTTON_DPAD_UP==11&&SDL_CONTROLLER_BUTTON_DPAD_DOWN==12&&
    SDL_CONTROLLER_BUTTON_DPAD_LEFT==13&&SDL_CONTROLLER_BUTTON_DPAD_RIGHT==14,"SDL2 controller ABI");
static_assert(SDL_MAJOR_VERSION == 2, "SDL2 development headers required");
static_assert(sizeof(void*) == 8 && alignof(SdlEvent) == 8, "Windows x64 SDL2 ABI");
static_assert(offsetof(SDL_CommonEvent, type) == 0 &&
    offsetof(SDL_WindowEvent, windowID) == 8 && offsetof(SDL_WindowEvent, event) == 12 &&
    offsetof(SDL_KeyboardEvent, windowID) == 8 && offsetof(SDL_KeyboardEvent, repeat) == 13 &&
    offsetof(SDL_KeyboardEvent, keysym) + offsetof(SDL_Keysym, scancode) == 16 &&
    sizeof(SDL_Scancode) == 4 && offsetof(SDL_DropEvent, file) == 8 &&
    offsetof(SDL_TextEditingExtEvent, text) == 16 &&
    offsetof(SDL_ControllerButtonEvent, which)==8&&offsetof(SDL_ControllerButtonEvent, button)==12,
    "SDL2 event decoder offsets");
#else
struct SdlWindow;
struct SdlGameController;
struct SdlJoystick;
struct SdlVersion { std::uint8_t major, minor, patch; };
// Public SDL2 event ABI on ordinary 32/64-bit Linux. No SDL headers required.
struct alignas(8) SdlEvent { unsigned char bytes[56]; };
#endif
// Windows x86/ARM64 are rejected by native_dynamic_library50win.h; the GL
// procedure tables rely on the x64 ABI. Read foreign events through memcpy.
static_assert(sizeof(int) == 4 && (sizeof(void*) == 4 || sizeof(void*) == 8),
    "SDL2 host requires the standard 32/64-bit scalar/pointer ABI");
static_assert(sizeof(SdlEvent) == 56 && sizeof(SdlVersion) == 3, "SDL2 public ABI");
template<class T> T field(const SdlEvent& e, std::size_t offset) noexcept {
    T result{};
    std::memcpy(&result, reinterpret_cast<const unsigned char*>(&e) + offset, sizeof(result));
    return result;
}
constexpr UInt gameControllerInit = 0x2000;
constexpr UInt videoInit = 0x20, windowOpenGl = 0x2, windowShown = 0x4;
constexpr UInt windowResizable = 0x20, windowMinimized = 0x40;
constexpr UInt windowFocused = 0x200, windowHighDpi = 0x2000;
constexpr Int windowCentered = 0x2fff0000;
constexpr UInt quitEvent = 0x100, windowEvent = 0x200;
constexpr UInt keyDownEvent = 0x300, keyUpEvent = 0x301, textEditingExtEvent = 0x305;
constexpr UInt controllerButtonDownEvent=0x651,controllerButtonUpEvent=0x652;
constexpr UInt dropFileEvent = 0x1000, dropTextEvent = 0x1001;
constexpr Int glRed = 0, glGreen = 1, glBlue = 2, glAlpha = 3, glDoubleBuffer = 5;
constexpr Int glDepth = 6, glStencil = 7, glSampleBuffers = 13, glSamples = 14;
constexpr Int glMajor = 17, glMinor = 18, glFlags = 20, glProfile = 21, glShare = 22;
constexpr Int coreProfile = 1, compatibilityProfile = 2;
constexpr UInt glMajorVersion = 0x821b, glMinorVersion = 0x821c, glContextProfileMask = 0x9126;

void copyDetail(std::array<char, 256>& destination, const char* source) noexcept {
    destination.fill(0);
    if (!source) return;
    std::size_t n = 0;
    while (n + 1 < destination.size() && source[n]) {
        destination[n] = source[n];
        ++n;
    }
}
bool fail(WindowHostDiagnostic50DQ& d, WindowHostError50DQ error,
    const char* operation, const char* detail = nullptr) noexcept {
    d.error = error;
    d.operation = operation;
    copyDetail(d.detail, detail);
    std::fprintf(stderr,"native_window_failure error=%u operation=%s gl_error=%u detail=%s\n",
        static_cast<unsigned>(error),operation?operation:"unknown",d.glError,d.detail.data());
    return false;
}
bool valid(const WindowHostRequest50DQ& r) noexcept {
    return r.title && r.title[0] && r.width > 0 && r.height > 0 &&
        r.major >= 3 && r.minor >= 0 && (r.major > 3 || r.minor >= 2) &&
        (r.profile == WindowGlProfile50DQ::Core ||
         r.profile == WindowGlProfile50DQ::Compatibility) &&
        r.redBits >= 0 && r.greenBits >= 0 && r.blueBits >= 0 &&
        r.alphaBits >= 0 && r.depthBits >= 0 && r.stencilBits >= 0 &&
        r.samples >= 0 && r.swapInterval >= -1 && r.swapInterval <= 1;
}
}

struct WindowHost50DQ::State {
    native_library50win::Handle library = nullptr;
    SdlWindow* window = nullptr;
    void* context = nullptr;
    UInt windowId = 0;
    bool initAttempted = false, ready = false, quitRequested = false;
    std::array<std::uint8_t, windowScancodeCount50DQ> down{};
    bool controllersEnabled=false;
    SdlGameController* controllers[2]{};
    Int controllerIds[2]={-1,-1};
    bool controllerRumbling[2]{};
    Int (RR_SDL50WIN_CALL *numJoysticks)() = nullptr;
    Int (RR_SDL50WIN_CALL *isGameController)(Int) = nullptr;
    Int (RR_SDL50WIN_CALL *deviceInstanceId)(Int) = nullptr;
    SdlJoystick* (RR_SDL50WIN_CALL *controllerJoystick)(SdlGameController*) = nullptr;
    Int (RR_SDL50WIN_CALL *joystickInstanceId)(SdlJoystick*) = nullptr;
    SdlGameController* (RR_SDL50WIN_CALL *openController)(Int) = nullptr;
    void (RR_SDL50WIN_CALL *closeController)(SdlGameController*) = nullptr;
    Int (RR_SDL50WIN_CALL *controllerAttached)(SdlGameController*) = nullptr;
    std::int16_t (RR_SDL50WIN_CALL *controllerAxis)(SdlGameController*,Int) = nullptr;
    std::uint8_t (RR_SDL50WIN_CALL *controllerButton)(SdlGameController*,Int) = nullptr;
    Int (RR_SDL50WIN_CALL *controllerHasRumble)(SdlGameController*) = nullptr;
    Int (RR_SDL50WIN_CALL *controllerRumble)(SdlGameController*,std::uint16_t,std::uint16_t,UInt) = nullptr;
    Int (RR_SDL50WIN_CALL *init)(UInt) = nullptr;
    void (RR_SDL50WIN_CALL *quit)() = nullptr;
    UInt (RR_SDL50WIN_CALL *wasInit)(UInt) = nullptr;
    void (RR_SDL50WIN_CALL *getVersion)(SdlVersion*) = nullptr;
    const char* (RR_SDL50WIN_CALL *getError)() = nullptr;
    void (RR_SDL50WIN_CALL *freeMemory)(void*) = nullptr;
    SdlWindow* (RR_SDL50WIN_CALL *createWindow)(const char*, Int, Int, Int, Int, UInt) = nullptr;
    void (RR_SDL50WIN_CALL *destroyWindow)(SdlWindow*) = nullptr;
    UInt (RR_SDL50WIN_CALL *getWindowId)(SdlWindow*) = nullptr;
    UInt (RR_SDL50WIN_CALL *getWindowFlags)(SdlWindow*) = nullptr;
    void (RR_SDL50WIN_CALL *getWindowSize)(SdlWindow*, Int*, Int*) = nullptr;
    void (RR_SDL50WIN_CALL *getDrawableSize)(SdlWindow*, Int*, Int*) = nullptr;
    void (RR_SDL50WIN_CALL *resetAttributes)() = nullptr;
    Int (RR_SDL50WIN_CALL *setAttribute)(Int, Int) = nullptr;
    Int (RR_SDL50WIN_CALL *getAttribute)(Int, Int*) = nullptr;
    void* (RR_SDL50WIN_CALL *createContext)(SdlWindow*) = nullptr;
    void (RR_SDL50WIN_CALL *deleteContext)(void*) = nullptr;
    Int (RR_SDL50WIN_CALL *makeCurrent)(SdlWindow*, void*) = nullptr;
    void* (RR_SDL50WIN_CALL *getCurrentContext)() = nullptr;
    SdlWindow* (RR_SDL50WIN_CALL *getCurrentWindow)() = nullptr;
    void* (RR_SDL50WIN_CALL *getProcAddress)(const char*) = nullptr;
    Int (RR_SDL50WIN_CALL *setSwapInterval)(Int) = nullptr;
    Int (RR_SDL50WIN_CALL *getSwapInterval)() = nullptr;
    void (RR_SDL50WIN_CALL *swapWindow)(SdlWindow*) = nullptr;
    void (RR_SDL50WIN_CALL *pumpEvents)() = nullptr;
    Int (RR_SDL50WIN_CALL *peepEvents)(SdlEvent*, Int, Int, UInt, UInt) = nullptr;
    std::uint8_t (RR_SDL50WIN_CALL *eventState)(UInt, Int) = nullptr;
    void (RR_SDL50WIN_CALL *stopTextInput)() = nullptr;

#if defined(_WIN32)
    void (RR_SDL50WIN_CALL *setMainReady)() = nullptr;
#endif

    bool loadControllers(WindowHostDiagnostic50DQ& d) noexcept {
        char loaderDetail[256]{};
#define RR_SDL50DQ_CONTROLLER(member, entry) \
        member = reinterpret_cast<decltype(member)>(native_library50win::symbol(library, entry, loaderDetail)); \
        if (!member) return fail(d, WindowHostError50DQ::MissingEntryPoint, entry, loaderDetail)
        RR_SDL50DQ_CONTROLLER(numJoysticks,"SDL_NumJoysticks");
        RR_SDL50DQ_CONTROLLER(isGameController,"SDL_IsGameController");
        RR_SDL50DQ_CONTROLLER(deviceInstanceId,"SDL_JoystickGetDeviceInstanceID");
        RR_SDL50DQ_CONTROLLER(controllerJoystick,"SDL_GameControllerGetJoystick");
        RR_SDL50DQ_CONTROLLER(joystickInstanceId,"SDL_JoystickInstanceID");
        RR_SDL50DQ_CONTROLLER(openController,"SDL_GameControllerOpen");
        RR_SDL50DQ_CONTROLLER(closeController,"SDL_GameControllerClose");
        RR_SDL50DQ_CONTROLLER(controllerAttached,"SDL_GameControllerGetAttached");
        RR_SDL50DQ_CONTROLLER(controllerAxis,"SDL_GameControllerGetAxis");
        RR_SDL50DQ_CONTROLLER(controllerButton,"SDL_GameControllerGetButton");
        // Optional subsystem is explicitly SDL2.0.18+ when selected. Do not
        // guess rumble capability or manufacture support on an older library.
        RR_SDL50DQ_CONTROLLER(controllerHasRumble,"SDL_GameControllerHasRumble");
        RR_SDL50DQ_CONTROLLER(controllerRumble,"SDL_GameControllerRumble");
#undef RR_SDL50DQ_CONTROLLER
        return true;
    }
    bool sampleControllers(WindowInput50DQ& input,WindowHostDiagnostic50DQ& d) noexcept {
        input.gameControllersEnabled=controllersEnabled;
        if(!controllersEnabled)return true;
        for(unsigned slot=0;slot<2;++slot)if(controllers[slot]&&!controllerAttached(controllers[slot])){
            closeController(controllers[slot]);controllers[slot]=nullptr;controllerIds[slot]=-1;controllerRumbling[slot]=false;
        }
        const auto count=numJoysticks();
        if(count<0)return fail(d,WindowHostError50DQ::ControllerFailure,"SDL_NumJoysticks",getError());
        for(Int device=0;device<count;++device){
            unsigned slot=0;while(slot<2&&controllers[slot])++slot;if(slot==2)break;
            if(!isGameController(device))continue; // Unknown joystick layouts remain unsupported.
            const auto id=deviceInstanceId(device);if(id<0)continue; // Removed during enumeration.
            if(controllerIds[0]==id||controllerIds[1]==id)continue;
            auto* opened=openController(device);
            if(!opened){++input.controllerOpenFailures;continue;}
            auto* joystick=controllerJoystick(opened);
            if(!controllerAttached(opened)||!joystick||joystickInstanceId(joystick)!=id){closeController(opened);continue;}
            controllers[slot]=opened;controllerIds[slot]=id;
        }
        for(unsigned slot=0;slot<2;++slot){
            auto* controller=controllers[slot];if(!controller)continue;
            auto& snapshot=input.controllers[slot];
            for(Int axis=0;axis<6;++axis)snapshot.axes[static_cast<unsigned>(axis)]=controllerAxis(controller,axis);
            for(Int button=0;button<15;++button)snapshot.buttons[static_cast<unsigned>(button)]=controllerButton(controller,button);
            // Disconnect during a read publishes absent, never partially live.
            if(!controllerAttached(controller)){snapshot={};closeController(controller);controllers[slot]=nullptr;controllerIds[slot]=-1;controllerRumbling[slot]=false;continue;}
            snapshot.instanceId=controllerIds[slot];snapshot.connected=true;
            snapshot.rumbleSupported=controllerHasRumble(controller)!=0;
        }
        return true;
    }

    bool load(WindowHostDiagnostic50DQ& d) noexcept {
        char loaderDetail[256]{};
        library = native_library50win::openSdl2(loaderDetail);
        if (!library) return fail(d, WindowHostError50DQ::LibraryUnavailable,
            native_library50win::openOperation, loaderDetail);
#define RR_SDL50DQ(member, entry) \
        member = reinterpret_cast<decltype(member)>(native_library50win::symbol(library, entry, loaderDetail)); \
        if (!member) return fail(d, WindowHostError50DQ::MissingEntryPoint, entry, loaderDetail)
#if defined(_WIN32)
        RR_SDL50DQ(setMainReady, "SDL_SetMainReady");
#endif
        RR_SDL50DQ(init, "SDL_Init");
        RR_SDL50DQ(quit, "SDL_Quit");
        RR_SDL50DQ(wasInit, "SDL_WasInit");
        RR_SDL50DQ(getVersion, "SDL_GetVersion");
        RR_SDL50DQ(getError, "SDL_GetError");
        RR_SDL50DQ(freeMemory, "SDL_free");
        RR_SDL50DQ(createWindow, "SDL_CreateWindow");
        RR_SDL50DQ(destroyWindow, "SDL_DestroyWindow");
        RR_SDL50DQ(getWindowId, "SDL_GetWindowID");
        RR_SDL50DQ(getWindowFlags, "SDL_GetWindowFlags");
        RR_SDL50DQ(getWindowSize, "SDL_GetWindowSize");
        RR_SDL50DQ(getDrawableSize, "SDL_GL_GetDrawableSize");
        RR_SDL50DQ(resetAttributes, "SDL_GL_ResetAttributes");
        RR_SDL50DQ(setAttribute, "SDL_GL_SetAttribute");
        RR_SDL50DQ(getAttribute, "SDL_GL_GetAttribute");
        RR_SDL50DQ(createContext, "SDL_GL_CreateContext");
        RR_SDL50DQ(deleteContext, "SDL_GL_DeleteContext");
        RR_SDL50DQ(makeCurrent, "SDL_GL_MakeCurrent");
        RR_SDL50DQ(getCurrentContext, "SDL_GL_GetCurrentContext");
        RR_SDL50DQ(getCurrentWindow, "SDL_GL_GetCurrentWindow");
        RR_SDL50DQ(getProcAddress, "SDL_GL_GetProcAddress");
        RR_SDL50DQ(setSwapInterval, "SDL_GL_SetSwapInterval");
        RR_SDL50DQ(getSwapInterval, "SDL_GL_GetSwapInterval");
        RR_SDL50DQ(swapWindow, "SDL_GL_SwapWindow");
        RR_SDL50DQ(pumpEvents, "SDL_PumpEvents");
        RR_SDL50DQ(peepEvents, "SDL_PeepEvents");
        RR_SDL50DQ(eventState, "SDL_EventState");
        RR_SDL50DQ(stopTextInput, "SDL_StopTextInput");
#undef RR_SDL50DQ
        SdlVersion version{};
        getVersion(&version);
        if (version.major != 2) return fail(d, WindowHostError50DQ::UnsupportedSdl,
            "SDL_GetVersion/SDL2 required");
        return true;
    }
};

WindowHost50DQ::WindowHost50DQ() noexcept : thread_(std::this_thread::get_id()) {}
WindowHost50DQ::~WindowHost50DQ() noexcept {
    WindowHostDiagnostic50DQ d;
    if (!ownerThread() || !closeOwned(d)) std::terminate();
}
bool WindowHost50DQ::ownerThread() const noexcept {
    return thread_ == std::this_thread::get_id();
}
bool WindowHost50DQ::created() const noexcept { return ownerThread() && state_ && state_->ready; }
bool WindowHost50DQ::ownsResources() const noexcept { return ownerThread() && state_; }
bool WindowHost50DQ::current() const noexcept {
    return created() && state_->getCurrentContext() == state_->context &&
        state_->getCurrentWindow() == state_->window;
}
WindowGlProc50DQ WindowHost50DQ::procAddress(const char* name) const noexcept {
    if (!current() || !name || name[0] != 'g' || name[1] != 'l' || !name[2]) return nullptr;
    return reinterpret_cast<WindowGlProc50DQ>(state_->getProcAddress(name));
}
void WindowHost50DQ::refreshWindowInfo() noexcept {
    auto& s = *state_;
    s.getWindowSize(s.window, &info_.width, &info_.height);
    s.getDrawableSize(s.window, &info_.drawableWidth, &info_.drawableHeight);
    const UInt flags = s.getWindowFlags(s.window);
    info_.focused = (flags & windowFocused) != 0;
    info_.minimized = (flags & windowMinimized) != 0;
}

bool WindowHost50DQ::create(const WindowHostRequest50DQ& r, WindowHostDiagnostic50DQ& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, WindowHostError50DQ::WrongThread, "create");
    if (state_) return fail(d, WindowHostError50DQ::AlreadyOwned, "create");
    if (!valid(r)) return fail(d, WindowHostError50DQ::InvalidRequest, "create");
    state_ = new (std::nothrow) State;
    if (!state_) return fail(d, WindowHostError50DQ::OutOfMemory, "create");
    auto& s = *state_;
    const auto cleanup = [&]() noexcept {
        WindowHostDiagnostic50DQ c;
        if (!closeOwned(c)) {
            d.cleanupError = c.error;
            d.cleanupOperation = c.operation;
            d.cleanupDetail = c.detail;
        }
        return false;
    };
    const auto sdlFail = [&](WindowHostError50DQ error, const char* operation) noexcept {
        fail(d, error, operation, s.getError());
        return cleanup();
    };
    if (!s.load(d)) return cleanup();
    // Attributes, events, text input and SDL_Quit are process-global: never
    // quietly borrow SDL from another owner or consume its event queue.
    if (s.wasInit(0)) {
        fail(d, WindowHostError50DQ::ForeignSdlOwner, "SDL_WasInit/exclusive ownership required");
        return cleanup();
    }
    if (s.getCurrentContext()) {
        fail(d, WindowHostError50DQ::ForeignCurrentContext, "create/preexisting SDL GL context");
        return cleanup();
    }
#if defined(_WIN32)
    // This host owns main and dynamically loads SDL; it does not use SDL2main.
    s.setMainReady();
#endif
    if(r.gameControllers&&!s.loadControllers(d))return cleanup();
    s.initAttempted = true;
    if (s.init(videoInit|(r.gameControllers?gameControllerInit:0u)) != 0)
        return sdlFail(r.gameControllers?WindowHostError50DQ::ControllerFailure:WindowHostError50DQ::VideoFailure,
            r.gameControllers?"SDL_Init(video|gamecontroller)":"SDL_Init(video)");
    s.controllersEnabled=r.gameControllers;
    s.stopTextInput(); // Keyboard controls only; no IME/text-input consumer.
    // Disable heap-payload events before creating the window/pumping events.
    // SDL_Quit alone does not free application-owned payloads left in its queue.
    s.eventState(dropFileEvent, 0);
    s.eventState(dropTextEvent, 0);
    s.eventState(textEditingExtEvent, 0);
    s.resetAttributes();
    const Int requestedProfile = r.profile == WindowGlProfile50DQ::Core ? coreProfile : compatibilityProfile;
    const Int attributes[][2] = {
        {glRed, r.redBits}, {glGreen, r.greenBits}, {glBlue, r.blueBits}, {glAlpha, r.alphaBits},
        {glDepth, r.depthBits}, {glStencil, r.stencilBits}, {glDoubleBuffer, 1},
        {glSampleBuffers, r.samples ? 1 : 0}, {glSamples, r.samples},
        {glMajor, r.major}, {glMinor, r.minor}, {glProfile, requestedProfile}, {glFlags, 0}, {glShare, 0}
    };
    for (const auto& attribute : attributes)
        if (s.setAttribute(attribute[0], attribute[1]) != 0) {
            std::fprintf(stderr,"native_window_attribute_set id=%d requested=%d\n",attribute[0],attribute[1]);
            return sdlFail(WindowHostError50DQ::AttributeFailure, "SDL_GL_SetAttribute");
        }
    const UInt flags = windowOpenGl | windowShown | windowHighDpi | (r.resizable ? windowResizable : 0u);
    s.window = s.createWindow(r.title, windowCentered, windowCentered, r.width, r.height, flags);
    if (!s.window) return sdlFail(WindowHostError50DQ::WindowFailure, "SDL_CreateWindow");
    s.windowId = s.getWindowId(s.window);
    if (!s.windowId) return sdlFail(WindowHostError50DQ::WindowFailure, "SDL_GetWindowID");
    s.context = s.createContext(s.window);
    if (!s.context) return sdlFail(WindowHostError50DQ::ContextFailure, "SDL_GL_CreateContext");
    if (s.makeCurrent(s.window, s.context) != 0)
        return sdlFail(WindowHostError50DQ::ContextFailure, "SDL_GL_MakeCurrent");
    if (s.getCurrentContext() != s.context || s.getCurrentWindow() != s.window) {
        fail(d, WindowHostError50DQ::ContextFailure, "SDL_GL_GetCurrentContext/window mismatch");
        return cleanup();
    }
#if defined(__linux__)
    // GL 3.2 core section 6.1.11: absent default depth/stencil attachments
    // report OBJECT_TYPE=NONE; querying their component size is an error.
    // SDL2's size getter does not first distinguish that valid absent state.
    using LinuxGetInteger = void (*)(UInt, Int*);
    using LinuxGetGlError = UInt (*)();
    using LinuxGetAttachment = void (*)(UInt, UInt, UInt, Int*);
    const auto linuxGetInteger = reinterpret_cast<LinuxGetInteger>(s.getProcAddress("glGetIntegerv"));
    const auto linuxGetError = reinterpret_cast<LinuxGetGlError>(s.getProcAddress("glGetError"));
    const auto linuxGetAttachment = reinterpret_cast<LinuxGetAttachment>(
        s.getProcAddress("glGetFramebufferAttachmentParameteriv"));
    if (!linuxGetInteger || !linuxGetError || !linuxGetAttachment) {
        fail(d, WindowHostError50DQ::MissingEntryPoint, !linuxGetInteger ? "glGetIntegerv" :
            !linuxGetError ? "glGetError" : "glGetFramebufferAttachmentParameteriv");
        return cleanup();
    }
    const auto linuxCheckGlError = [&](const char* operation) noexcept {
        d.glError = linuxGetError();
        return !d.glError || fail(d, WindowHostError50DQ::GlFailure, operation);
    };
    if (!linuxCheckGlError("GL error before framebuffer queries")) return cleanup();
    constexpr UInt drawFramebufferBinding = 0x8ca6, drawFramebuffer = 0x8ca9;
    constexpr UInt attachmentObjectType = 0x8cd0, framebufferDefault = 0x8218;
    constexpr UInt depthAttachment = 0x1801, stencilAttachment = 0x1802;
    constexpr UInt attachmentDepthSize = 0x8216, attachmentStencilSize = 0x8217;
    Int drawBinding = -1;
    linuxGetInteger(drawFramebufferBinding, &drawBinding);
    if (!linuxCheckGlError("glGetIntegerv/GL_DRAW_FRAMEBUFFER_BINDING")) return cleanup();
    // Creation owns a fresh context. Do not silently change unexpected state.
    if (drawBinding != 0) {
        fail(d, WindowHostError50DQ::GlFailure, "initial draw framebuffer is not default");
        return cleanup();
    }
    const auto linuxGetDepthStencil = [&](Int key, Int& bits) noexcept {
        const bool depth = key == glDepth;
        const UInt attachment = depth ? depthAttachment : stencilAttachment;
        Int type = -1;
        linuxGetAttachment(drawFramebuffer, attachment, attachmentObjectType, &type);
        if (!linuxCheckGlError(depth ? "default depth attachment/object type" :
            "default stencil attachment/object type")) return false;
        if (type == 0) { // GL_NONE is an actual successful query, not a fallback.
            bits = 0;
            return true;
        }
        if (type != static_cast<Int>(framebufferDefault))
            return fail(d, WindowHostError50DQ::GlFailure,
                depth ? "unexpected default depth attachment type" :
                "unexpected default stencil attachment type");
        bits = -1;
        linuxGetAttachment(drawFramebuffer, attachment,
            depth ? attachmentDepthSize : attachmentStencilSize, &bits);
        if (!linuxCheckGlError(depth ? "default depth attachment/size" :
            "default stencil attachment/size")) return false;
        if (bits < 0) return fail(d, WindowHostError50DQ::GlFailure,
            depth ? "invalid default depth attachment size" :
            "invalid default stencil attachment size");
        return true;
    };
#endif
    Int actualProfile = 0, buffers = 0, doubleBuffered = 0;
    const Int keys[] = { glRed, glGreen, glBlue, glAlpha,
        glDepth, glStencil, glSamples, glSampleBuffers, glDoubleBuffer };
    Int* values[] = { &info_.redBits, &info_.greenBits,
        &info_.blueBits, &info_.alphaBits, &info_.depthBits, &info_.stencilBits, &info_.samples,
        &buffers, &doubleBuffered };
    for (std::size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
#if defined(__linux__)
        if (keys[i] == glDepth || keys[i] == glStencil) {
            if (!linuxGetDepthStencil(keys[i], *values[i])) return cleanup();
            continue;
        }
#endif
        if (s.getAttribute(keys[i], values[i]) != 0) {
            std::fprintf(stderr,"native_window_attribute_get id=%d index=%zu\n",keys[i],i);
            return sdlFail(WindowHostError50DQ::AttributeFailure, "SDL_GL_GetAttribute");
        }
    }
    // SDL_GL_GetAttribute returns the requested version/profile from SDL's
    // configuration, not necessarily the actual context. Query GL itself.
    using GetInteger = void (*)(UInt, Int*);
    using GetGlError = UInt (*)();
    const auto getInteger = reinterpret_cast<GetInteger>(s.getProcAddress("glGetIntegerv"));
    const auto getGlError = reinterpret_cast<GetGlError>(s.getProcAddress("glGetError"));
    if (!getInteger || !getGlError) {
        fail(d, WindowHostError50DQ::MissingEntryPoint, !getInteger ? "glGetIntegerv" : "glGetError");
        return cleanup();
    }
    d.glError = getGlError();
    if (d.glError) {
        fail(d, WindowHostError50DQ::GlFailure, "GL error before version query");
        return cleanup();
    }
    getInteger(glMajorVersion, &info_.major);
    getInteger(glMinorVersion, &info_.minor);
    getInteger(glContextProfileMask, &actualProfile);
    d.glError = getGlError();
    if (d.glError) {
        fail(d, WindowHostError50DQ::GlFailure, "glGetIntegerv/version/profile");
        return cleanup();
    }
    if (info_.major < r.major || (info_.major == r.major && info_.minor < r.minor) ||
        actualProfile != requestedProfile || !doubleBuffered || info_.redBits < r.redBits ||
        info_.greenBits < r.greenBits || info_.blueBits < r.blueBits || info_.alphaBits < r.alphaBits ||
        info_.depthBits < r.depthBits || info_.stencilBits < r.stencilBits ||
        (r.samples ? buffers < 1 || info_.samples < r.samples : buffers != 0 || info_.samples != 0)) {
        fail(d, WindowHostError50DQ::RequirementsNotMet, "desktop GL/framebuffer requirements");
        return cleanup();
    }
    info_.profile = r.profile;
#if defined(__linux__)
    // Software GLX can lack a swap-control setter while already reporting the
    // exact requested interval. Preserve that observed state; never substitute
    // another interval when a requested change is unsupported.
    if (s.getSwapInterval() != r.swapInterval && s.setSwapInterval(r.swapInterval) != 0)
        return sdlFail(WindowHostError50DQ::SwapIntervalFailure, "SDL_GL_SetSwapInterval");
#else
    if (s.setSwapInterval(r.swapInterval) != 0)
        return sdlFail(WindowHostError50DQ::SwapIntervalFailure, "SDL_GL_SetSwapInterval");
#endif
    info_.swapInterval = s.getSwapInterval();
    if (info_.swapInterval != r.swapInterval) {
        fail(d, WindowHostError50DQ::RequirementsNotMet, "SDL_GL_GetSwapInterval/exact interval");
        return cleanup();
    }
    refreshWindowInfo();
    s.ready = true;
    return true;
}

bool WindowHost50DQ::bind(WindowHostDiagnostic50DQ& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, WindowHostError50DQ::WrongThread, "bind");
    if (!created()) return fail(d, WindowHostError50DQ::NotCreated, "bind");
    auto& s = *state_;
    const auto active = s.getCurrentContext();
    if (active && (active != s.context || s.getCurrentWindow() != s.window))
        return fail(d, WindowHostError50DQ::ForeignCurrentContext, "bind");
    if (s.makeCurrent(s.window, s.context) != 0)
        return fail(d, WindowHostError50DQ::ContextFailure, "SDL_GL_MakeCurrent", s.getError());
    return true;
}
bool WindowHost50DQ::release(WindowHostDiagnostic50DQ& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, WindowHostError50DQ::WrongThread, "release");
    if (!created()) return fail(d, WindowHostError50DQ::NotCreated, "release");
    auto& s = *state_;
    const auto active = s.getCurrentContext();
    if (!active) return true;
    if (active != s.context || s.getCurrentWindow() != s.window)
        return fail(d, WindowHostError50DQ::ForeignCurrentContext, "release");
    if (s.makeCurrent(s.window, nullptr) != 0)
        return fail(d, WindowHostError50DQ::ReleaseFailure, "SDL_GL_MakeCurrent/unbind", s.getError());
    return true;
}

bool WindowHost50DQ::poll(WindowInput50DQ& input, WindowHostDiagnostic50DQ& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, WindowHostError50DQ::WrongThread, "poll");
    if (!created()) return fail(d, WindowHostError50DQ::NotCreated, "poll");
    auto& s = *state_;
    SdlEvent events[windowEventBudget50DQ]{};
    s.pumpEvents();
    const Int count = s.peepEvents(events, static_cast<Int>(windowEventBudget50DQ), 2, 0, 0xffff);
    if (count < 0) return fail(d, WindowHostError50DQ::EventFailure, "SDL_PeepEvents", s.getError());
    input = {};
    input.activityLost=!info_.focused||info_.minimized;
    input.eventsProcessed = static_cast<UInt>(count);
    input.eventBudgetExhausted = count == static_cast<Int>(windowEventBudget50DQ);
    const auto releaseKeys = [&]() noexcept {
        for (std::size_t key = 0; key < s.down.size(); ++key) {
            if (s.down[key]) input.released[key] = 1;
            s.down[key] = 0;
        }
    };
    for (Int i = 0; i < count; ++i) {
        const auto& event = events[i];
        const UInt type = field<UInt>(event, 0);
        // Diagnostic receipt only, restricted to this owned game window.
        // Reuse existing ABI fields before focus/scancode/repeat handling.
        if ((type == keyDownEvent || type == keyUpEvent) &&
            field<UInt>(event, 8) == s.windowId) {
            std::fprintf(stderr,
                "native_sdl_key_event type=%u scancode=%d window=%u matching_window=%u focused=%u repeat=%u\n",
                static_cast<unsigned>(type),static_cast<int>(field<Int>(event,16)),
                static_cast<unsigned>(field<UInt>(event,8)),
                static_cast<unsigned>(field<UInt>(event,8)==s.windowId),
                static_cast<unsigned>(info_.focused),
                static_cast<unsigned>(field<std::uint8_t>(event,13)));
        }
        if (type == quitEvent) s.quitRequested = true;
        else if (type == windowEvent && field<UInt>(event, 8) == s.windowId) {
            switch (field<std::uint8_t>(event, 12)) {
            case 5: case 6: input.resized = true; break; // RESIZED, SIZE_CHANGED
            case 7: input.activityLost=true; releaseKeys(); break; // MINIMIZED
            case 12: info_.focused = true; break; // FOCUS_GAINED
            case 13: input.activityLost=true; info_.focused = false; releaseKeys(); break; // FOCUS_LOST
            case 14: s.quitRequested = true; break; // CLOSE
            default: break;
            }
        } else if ((type == keyDownEvent || type == keyUpEvent) &&
            field<UInt>(event, 8) == s.windowId) {
            const Int scancode = field<Int>(event, 16);
            if (scancode <= 0 || scancode >= static_cast<Int>(s.down.size())) continue;
            const auto key = static_cast<std::size_t>(scancode);
            if (type == keyDownEvent && info_.focused) {
                if (!s.down[key] && !field<std::uint8_t>(event, 13)) input.pressed[key] = 1;
                s.down[key] = 1;
            } else if (type == keyUpEvent) {
                if (s.down[key]) input.released[key] = 1;
                s.down[key] = 0;
            }
        } else if (type == dropFileEvent || type == dropTextEvent) {
            s.freeMemory(field<void*>(event, 8));
        } else if (type == textEditingExtEvent) {
            constexpr std::size_t textPointerOffset = (12 + alignof(void*) - 1) & ~(alignof(void*) - 1);
            s.freeMemory(field<void*>(event, textPointerOffset));
        }
    }
    const Int width = info_.width, height = info_.height;
    const Int drawableWidth = info_.drawableWidth, drawableHeight = info_.drawableHeight;
    refreshWindowInfo();
    // Consult actual focus even if FOCUS_LOST remains behind this poll's budget.
    // This prevents a queued event flood from leaving movement stuck on blur.
    if (!info_.focused || info_.minimized) { input.activityLost=true; releaseKeys(); }
    input.resized = input.resized || width != info_.width || height != info_.height ||
        drawableWidth != info_.drawableWidth || drawableHeight != info_.drawableHeight;
    input.down = s.down;
    input.focused = info_.focused;
    input.minimized = info_.minimized;
    input.quitRequested = s.quitRequested;
    if(!s.sampleControllers(input,d))return false;
    if(input.activityLost||input.quitRequested)for(unsigned slot=0;slot<2;++slot)
        if(s.controllers[slot]&&s.controllerRumbling[slot]){
            ControllerRumbleReceipt50KZ stopped{};
            if(!rumbleController50KZ(slot,s.controllerIds[slot],0,0,stopped,d))return false;
        }
    // Preserve a real Start tap even when press and release share this poll.
    // Edges are accepted only for the actually opened, still-attached instance.
    // Conservatively suppress the batch if it crossed an inactive interval.
    for(Int i=0;!input.activityLost&&i<count;++i){const auto& event=events[i];const auto type=field<UInt>(event,0);
        if(type!=controllerButtonDownEvent&&type!=controllerButtonUpEvent)continue;
        const auto id=field<Int>(event,8);const auto button=field<std::uint8_t>(event,12);
        if(button>=15)continue;
        for(auto& controller:input.controllers)if(controller.connected&&controller.instanceId==id){
            if(type==controllerButtonDownEvent)controller.pressed[button]=1;
            else controller.released[button]=1;
        }
    }
    return true;
}
bool WindowHost50DQ::swap(WindowHostDiagnostic50DQ& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, WindowHostError50DQ::WrongThread, "swap");
    if (!created()) return fail(d, WindowHostError50DQ::NotCreated, "swap");
    if (!current()) return fail(d, WindowHostError50DQ::NotCurrent, "swap");
    state_->swapWindow(state_->window);
    return true;
}
bool WindowHost50DQ::setSwapInterval(std::int32_t interval,WindowHostDiagnostic50DQ& d) noexcept {
    d={};
    if(!ownerThread())return fail(d,WindowHostError50DQ::WrongThread,"setSwapInterval");
    if(!created()||!current())return fail(d,WindowHostError50DQ::NotCurrent,"setSwapInterval");
    if(interval < -1||interval > 1)return fail(d,WindowHostError50DQ::InvalidRequest,"setSwapInterval");
#if defined(__linux__)
    // Match create(): an exact current interval needs no unsupported GLX
    // setter. This also verifies unchanged native state after a rejected
    // presentation request; it never substitutes a different interval.
    info_.swapInterval=state_->getSwapInterval();
    if(info_.swapInterval==interval)return true;
#endif
    const auto accepted=state_->setSwapInterval(interval)==0;
    info_.swapInterval=state_->getSwapInterval();
    return (accepted&&info_.swapInterval==interval)||fail(d,WindowHostError50DQ::SwapIntervalFailure,"SDL_GL_SetSwapInterval");
}
bool WindowHost50DQ::rumbleController50KZ(std::uint32_t slot,std::int32_t expected,
    std::uint8_t small,std::uint8_t large,ControllerRumbleReceipt50KZ& receipt,
    WindowHostDiagnostic50DQ& d) noexcept {
    d={};receipt={};
    if(!ownerThread())return fail(d,WindowHostError50DQ::WrongThread,"rumbleController50KZ");
    if(!created())return fail(d,WindowHostError50DQ::NotCreated,"rumbleController50KZ");
    if(slot>=2||small>1)return fail(d,WindowHostError50DQ::InvalidRequest,"rumbleController50KZ");
    auto& s=*state_;
    if(!s.controllersEnabled||!s.controllers[slot]||!s.controllerAttached(s.controllers[slot]))return true;
    receipt.instanceId=s.controllerIds[slot];
    if(expected!=receipt.instanceId){receipt.status=ControllerRumbleStatus50KZ::ChangedDevice;return true;}
    if(!s.controllerHasRumble(s.controllers[slot])){receipt.status=ControllerRumbleStatus50KZ::Unsupported;return true;}
    const auto low=static_cast<std::uint16_t>(static_cast<unsigned>(large)*257u);
    const auto high=static_cast<std::uint16_t>(small?65535u:0u);
    if(s.controllerRumble(s.controllers[slot],low,high,(small||large)?100u:0u)!=0)
        return fail(d,WindowHostError50DQ::ControllerFailure,"SDL_GameControllerRumble",s.getError());
    s.controllerRumbling[slot]=(small||large)!=0;
    receipt.status=ControllerRumbleStatus50KZ::Submitted;
    receipt.smallMotor=small;receipt.largeMotor=large;return true;
}
bool WindowHost50DQ::close(WindowHostDiagnostic50DQ& d) noexcept {
    d = {};
    if (!ownerThread()) return fail(d, WindowHostError50DQ::WrongThread, "close");
    return closeOwned(d);
}
bool WindowHost50DQ::closeOwned(WindowHostDiagnostic50DQ& d) noexcept {
    if (!state_) return true;
    auto& s = *state_;
    s.ready = false;
    if (s.initAttempted) {
        const auto active = s.getCurrentContext();
        if (active && (active != s.context || s.getCurrentWindow() != s.window))
            return fail(d, WindowHostError50DQ::ForeignCurrentContext, "close");
        if (active && s.makeCurrent(s.window, nullptr) != 0)
            return fail(d, WindowHostError50DQ::ReleaseFailure, "SDL_GL_MakeCurrent/unbind", s.getError());
        for(unsigned slot=0;slot<2;++slot)if(s.controllers[slot]){
            if(s.controllerRumbling[slot]&&s.controllerAttached(s.controllers[slot])){
                if(s.controllerRumble(s.controllers[slot],0,0,0)!=0){
                    // A best-effort motor stop cannot hold window/GL/library
                    // resources hostage. Report the failed submission while
                    // completing teardown; the bounded lease also expires.
                    d.cleanupError=WindowHostError50DQ::ControllerFailure;
                    d.cleanupOperation="SDL_GameControllerRumble/close";
                    copyDetail(d.cleanupDetail,s.getError());
                }
            }
            s.controllerRumbling[slot]=false;
            s.closeController(s.controllers[slot]);s.controllers[slot]=nullptr;s.controllerIds[slot]=-1;
        }
        s.controllersEnabled=false;
        if (s.context) { s.deleteContext(s.context); s.context = nullptr; }
        if (s.window) { s.destroyWindow(s.window); s.window = nullptr; }
        s.quit();
        s.initAttempted = false;
    }
    if (s.library) {
        char loaderDetail[256]{};
        if (!native_library50win::close(s.library, loaderDetail))
            return fail(d, WindowHostError50DQ::LibraryCloseFailure,
                native_library50win::closeOperation, loaderDetail);
        s.library = nullptr;
    }
    delete state_;
    state_ = nullptr;
    info_ = {};
    return true;
}
}
