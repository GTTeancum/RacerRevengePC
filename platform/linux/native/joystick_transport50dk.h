#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::platform {
enum class JoystickConnection50DK { Closed, Open, Disconnected, Faulted };
enum class JoystickError50DK {
    None, InvalidRequest, AlreadyOpen, NotOpen, TransportUnavailable,
    OpenFailed, StatFailed, NotCharacterDevice, UnsupportedDevice, QueryFailed,
    Interrupted, ReadFailed, EndOfStream, ShortRead, InvalidEvent, CloseFailed
};
struct JoystickDiagnostic50DK {
    JoystickError50DK error = JoystickError50DK::None;
    const char* operation = nullptr; // Static string; no retained path pointer.
    int systemError = 0; // Captured errno, or zero for a non-syscall rejection.
    int cleanupSystemError = 0; // A separate close failure after another error.
};
struct JoystickEvent50DK {
    std::uint32_t kernelTime = 0; // Opaque js_event.time; no clock conversion.
    std::int16_t value = 0;
    std::uint8_t type = 0, number = 0; // Unchanged Linux joystick UAPI fields.
};
struct JoystickField50DK {
    std::int16_t value = 0; // Placeholder only until observed is true.
    std::uint32_t kernelTime = 0;
    bool observed = false;
    bool initializationSeen = false; // At least one JS_EVENT_INIT for this index.
};
struct JoystickSnapshot50DK {
    // UAPI indices/counts are unsigned bytes. No controller layout is assumed.
    std::array<JoystickField50DK, 256> axes{}, buttons{};
    std::uint32_t driverVersion = 0;
    std::uint16_t axisCount = 0, buttonCount = 0;
    std::uint16_t observedAxes = 0, observedButtons = 0;
    bool metadataKnown = false;
    bool allFieldsObserved() const noexcept {
        return metadataKnown && observedAxes == axisCount && observedButtons == buttonCount;
    }
};
enum class JoystickPollStop50DK { QueueEmpty, BudgetReached, Interrupted, Failed };
struct JoystickPollResult50DK {
    JoystickPollStop50DK stop = JoystickPollStop50DK::Failed;
    JoystickDiagnostic50DK diagnostic{};
    std::size_t readCalls = 0, bytesRead = 0;
    // Every complete consumed event is written to the caller's buffer, even an
    // invalid terminal event. Only its accepted prefix updates the snapshot.
    std::size_t receivedEvents = 0, appliedEvents = 0;
};

// Raw Linux joydev transport. Construct/use/destroy serially on one thread;
// no concurrent/reentrant calls. Stable, noncopyable and nonmovable owner.
// No discovery, hotplug retry, calibration, packet mapping or neutral fallback.
class JoystickTransport50DK {
public:
    static constexpr std::size_t MaxPollEvents = 64;
    JoystickTransport50DK() noexcept = default;
    ~JoystickTransport50DK() noexcept;
    JoystickTransport50DK(const JoystickTransport50DK&) = delete;
    JoystickTransport50DK& operator=(const JoystickTransport50DK&) = delete;
    JoystickTransport50DK(JoystickTransport50DK&&) = delete;
    JoystickTransport50DK& operator=(JoystickTransport50DK&&) = delete;

    // Caller explicitly supplies an absolute device path. No default device.
    // A valid attempt with no open fd resets the old snapshot, even on failure.
    // InvalidRequest/AlreadyOpen leave state unchanged. Successful open queries
    // metadata only: all values remain unobserved until their events arrive.
    bool open(const char* absoluteDevicePath, JoystickDiagnostic50DK&) noexcept;

    // events is a separate live writable buffer with capacity in [1,64]. That
    // capacity also bounds read attempts. No allocation, waiting or EINTR retry.
    // Only [0,receivedEvents) is written. QueueEmpty is an EAGAIN observation,
    // not a simultaneous full-device sample. BudgetReached does not mean empty.
    // Progress is retained even on failure; kernel reads cannot be rolled back.
    // A terminal error closes the fd and keeps the historical snapshot/events.
    JoystickPollResult50DK poll(JoystickEvent50DK* events, std::size_t capacity) noexcept;

    // One close attempt, never a retry on EINTR (Linux may already release fd).
    // fd ownership ends even when close reports failure. Snapshot is retained.
    // Call explicitly for a teardown diagnostic; destruction also closes once.
    bool close(JoystickDiagnostic50DK&) noexcept;
    // Open means an owned descriptor with successful metadata queries; removal
    // can remain undetected until a read. Other states forbid treating retained
    // observations as current connected-device input.
    JoystickConnection50DK connection() const noexcept { return connection_; }
    const JoystickSnapshot50DK& snapshot() const noexcept { return snapshot_; }
private:
    int closeDescriptor() noexcept;
    int fd_ = -1;
    JoystickConnection50DK connection_ = JoystickConnection50DK::Closed;
    JoystickSnapshot50DK snapshot_{};
};
}
