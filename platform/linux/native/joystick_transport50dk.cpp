#include "joystick_transport50dk.h"

#if defined(__linux__) && __has_include(<linux/joystick.h>)
#define RR_JOYDEV50DK_AVAILABLE 1
#include <cerrno>
#include <fcntl.h>
#include <linux/joystick.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#else
#define RR_JOYDEV50DK_AVAILABLE 0
#endif

namespace rr::platform {
namespace {
bool fail(JoystickDiagnostic50DK& d, JoystickError50DK error,
    const char* operation, int systemError = 0) noexcept {
    d = {error, operation, systemError, 0};
    return false;
}
}

JoystickTransport50DK::~JoystickTransport50DK() noexcept { closeDescriptor(); }

int JoystickTransport50DK::closeDescriptor() noexcept {
    const int owned = fd_;
    fd_ = -1;
#if RR_JOYDEV50DK_AVAILABLE
    if (owned >= 0 && ::close(owned) < 0) return errno;
#else
    (void)owned;
#endif
    return 0;
}

bool JoystickTransport50DK::close(JoystickDiagnostic50DK& d) noexcept {
    d = {};
    const int error = closeDescriptor();
    connection_ = JoystickConnection50DK::Closed;
    return error ? fail(d, JoystickError50DK::CloseFailed, "close", error) : true;
}

bool JoystickTransport50DK::open(const char* path, JoystickDiagnostic50DK& d) noexcept {
    d = {};
    if (fd_ >= 0) return fail(d, JoystickError50DK::AlreadyOpen, "open");
    if (!path || path[0] != '/' || !path[1])
        return fail(d, JoystickError50DK::InvalidRequest, "absolute device path");
    snapshot_ = {};
    connection_ = JoystickConnection50DK::Faulted;
#if RR_JOYDEV50DK_AVAILABLE
    fd_ = ::open(path, O_RDONLY | O_NONBLOCK | O_CLOEXEC | O_NOCTTY);
    if (fd_ < 0) return fail(d, JoystickError50DK::OpenFailed, "open", errno);
    const auto reject = [&](JoystickError50DK error, const char* operation,
                            int systemError = 0) noexcept {
        fail(d, error, operation, systemError);
        d.cleanupSystemError = closeDescriptor();
        return false;
    };
    struct stat status{};
    if (::fstat(fd_, &status) < 0)
        return reject(JoystickError50DK::StatFailed, "fstat", errno);
    if (!S_ISCHR(status.st_mode))
        return reject(JoystickError50DK::NotCharacterDevice, "character device required");
    __u32 version = 0;
    __u8 axes = 0, buttons = 0;
    const auto query = [&](unsigned long request, void* output, const char* operation) noexcept {
        if (::ioctl(fd_, request, output) >= 0) return true;
        const int error = errno;
        return reject(error == ENOTTY ? JoystickError50DK::UnsupportedDevice :
            JoystickError50DK::QueryFailed, operation, error);
    };
    if (!query(JSIOCGVERSION, &version, "JSIOCGVERSION") ||
        !query(JSIOCGAXES, &axes, "JSIOCGAXES") ||
        !query(JSIOCGBUTTONS, &buttons, "JSIOCGBUTTONS")) return false;
    snapshot_.driverVersion = version;
    snapshot_.axisCount = axes;
    snapshot_.buttonCount = buttons;
    snapshot_.metadataKnown = true;
    connection_ = JoystickConnection50DK::Open;
    return true;
#else
    return fail(d, JoystickError50DK::TransportUnavailable, "Linux joystick UAPI unavailable");
#endif
}

JoystickPollResult50DK JoystickTransport50DK::poll(JoystickEvent50DK* events,
    std::size_t capacity) noexcept {
    JoystickPollResult50DK r;
    if (!events || !capacity || capacity > MaxPollEvents) {
        fail(r.diagnostic, JoystickError50DK::InvalidRequest, "poll buffer/capacity");
        return r;
    }
    if (fd_ < 0) {
        fail(r.diagnostic, JoystickError50DK::NotOpen, "poll");
        return r;
    }
#if RR_JOYDEV50DK_AVAILABLE
    const auto reject = [&](JoystickError50DK error, const char* operation,
                            int systemError, JoystickConnection50DK connection) noexcept {
        r.stop = JoystickPollStop50DK::Failed;
        fail(r.diagnostic, error, operation, systemError);
        r.diagnostic.cleanupSystemError = closeDescriptor();
        connection_ = connection;
        return r;
    };
    for (; r.readCalls < capacity;) {
        js_event event{};
        ++r.readCalls;
        const ssize_t n = ::read(fd_, &event, sizeof event);
        if (n < 0) {
            const int error = errno;
            if (error == EAGAIN || error == EWOULDBLOCK) {
                r.stop = JoystickPollStop50DK::QueueEmpty;
                return r;
            }
            if (error == EINTR) {
                r.stop = JoystickPollStop50DK::Interrupted;
                fail(r.diagnostic, JoystickError50DK::Interrupted, "read", error);
                return r;
            }
            return reject(JoystickError50DK::ReadFailed, "read", error,
                error == ENODEV || error == ENXIO ? JoystickConnection50DK::Disconnected :
                JoystickConnection50DK::Faulted);
        }
        if (!n) return reject(JoystickError50DK::EndOfStream, "read", 0,
            JoystickConnection50DK::Disconnected);
        r.bytesRead += static_cast<std::size_t>(n);
        if (n != static_cast<ssize_t>(sizeof event))
            return reject(JoystickError50DK::ShortRead, "read/js_event size", 0,
                JoystickConnection50DK::Faulted);
        events[r.receivedEvents++] = {event.time, event.value, event.type, event.number};
        const auto kind = event.type & ~JS_EVENT_INIT;
        JoystickField50DK* field = nullptr;
        std::uint16_t* observedCount = nullptr;
        if (kind == JS_EVENT_AXIS && event.number < snapshot_.axisCount) {
            field = &snapshot_.axes[event.number];
            observedCount = &snapshot_.observedAxes;
        } else if (kind == JS_EVENT_BUTTON && event.number < snapshot_.buttonCount) {
            field = &snapshot_.buttons[event.number];
            observedCount = &snapshot_.observedButtons;
        } else return reject(JoystickError50DK::InvalidEvent, "js_event type/index", 0,
            JoystickConnection50DK::Faulted);
        if (!field->observed) ++*observedCount;
        field->value = event.value;
        field->kernelTime = event.time;
        field->observed = true;
        field->initializationSeen = field->initializationSeen || (event.type & JS_EVENT_INIT) != 0;
        ++r.appliedEvents;
    }
    r.stop = JoystickPollStop50DK::BudgetReached;
    return r;
#else
    fail(r.diagnostic, JoystickError50DK::TransportUnavailable, "Linux joystick UAPI unavailable");
    return r;
#endif
}
}
