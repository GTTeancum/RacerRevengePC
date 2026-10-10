#pragma once
#include "../../../src/game/device_held48y.h"
#include <cstddef>
#include <cstdint>

namespace rr::platform {
inline constexpr std::size_t keyboardScancodeCount50DR = 512;

struct KeyboardHeldSpan50DR {
    // Borrowed SDL scancode-indexed bytes, not keycodes or text characters.
    // A live input requires exactly512 entries. Mapped values must be0 or1.
    const std::uint8_t* held = nullptr;
    std::size_t count = 0;
};
struct KeyboardAvailability50DR {
    // Caller reports whether its keyboard/window source is usable and focused.
    // This adapter performs no SDL, event-pump or hardware-connection queries.
    bool connected = false;
    bool focused = false;
};

enum class KeyboardAxisSource50DR : std::uint8_t {
    Unspecified, KeyPair, Centered
};
enum class KeyboardOpposingKeys50DR : std::uint8_t {
    Unspecified, Center, NegativeWins, PositiveWins
};
struct KeyboardAxisMapping50DR {
    KeyboardAxisSource50DR source = KeyboardAxisSource50DR::Unspecified;
    // KeyPair requires distinct SDL scancodes1..511 and an explicit both-held
    // policy. Negative alone writes0; positive alone255; neither128.
    // Centered writes128 and ignores both scancodes and opposingKeys.
    std::uint16_t negativeScancode = 0;
    std::uint16_t positiveScancode = 0;
    KeyboardOpposingKeys50DR opposingKeys = KeyboardOpposingKeys50DR::Unspecified;
};
enum class KeyboardButtonSource50DR : std::uint8_t {
    Unspecified, Key, Released
};
struct KeyboardButtonMapping50DR {
    KeyboardButtonSource50DR source = KeyboardButtonSource50DR::Unspecified;
    // Key requires an SDL scancode1..511. Released ignores scancode.
    std::uint16_t scancode = 0;
};
struct KeyboardPacketMapping50DR {
    // Array positions select packet bytes4,5,6,7, not physical stick labels.
    KeyboardAxisMapping50DR axes[4]{};
    // Array positions select original numeric descriptor codes0..15.
    KeyboardButtonMapping50DR buttons[16]{};
};

enum class KeyboardUnavailablePolicy50DR : std::uint8_t {
    Unspecified, Reject, ReleaseAll
};
enum class KeyboardPacketError50DR : std::uint8_t {
    None, InvalidUnavailablePolicy, NotConnected, NotFocused, InvalidHeldSpan,
    UnspecifiedAxisSource, InvalidAxisSource, InvalidAxisScancode,
    DuplicateAxisScancode, InvalidOpposingKeys,
    UnspecifiedButtonSource, InvalidButtonSource, InvalidButtonScancode,
    NonbinaryKey, ConflictingDestination
};
enum class KeyboardPacketDisposition50DR : std::uint8_t {
    None, Live, ReleasedDisconnected, ReleasedUnfocused
};
struct KeyboardPacketDiagnostic50DR {
    KeyboardPacketError50DR error = KeyboardPacketError50DR::None;
    // Field-specific errors identify axes[] or buttons[]; otherwise index=-1.
    std::int32_t mappingIndex = -1;
    bool buttonMapping = false;
    // The offending scancode for range/value errors; otherwise-1.
    std::int32_t scancode = -1;
    KeyboardPacketDisposition50DR disposition = KeyboardPacketDisposition50DR::None;
};

// Stateless, bounded, allocation-free host adapter, NOT recovered PS2 transport.
// All four axes, sixteen button sources and unavailable policy must be explicit.
// Reuse of a key across bindings is allowed. Scancode0 is SDL's unknown value
// and cannot be bound. This API does not include or depend on SDL headers.
//
// Mapping and policy are validated on every call, including ReleaseAll calls.
// If connected and focused, held must name exactly512 readable bytes, stable
// throughout this call. Only mapped entries are read and checked for0/1. Caller
// owns polling, focus changes, stale-key clearing and observation freshness.
//
// If disconnected or unfocused, Reject fails without publishing a packet.
// ReleaseAll explicitly selects centered axes/released buttons without reading
// or validating held; {nullptr,0} is allowed on this path. The diagnostic marks
// this synthetic host sample. Disconnected wins if both flags are false.
// Restoring focus/connection uses caller-supplied current held state: no cached
// keys, latch, resume suppression, edge timestamps or last-key-wins policy exist.
//
// Only initialDualShockDescriptors48Y() with dualShockActiveButtons48Y (16) is
// supported downstream. Writes byte0=0, axes4..7, pressure8..19 and digital mask
// bits0x0F00. Everything else comes verbatim from packetTemplate, including the
// unused digital mirrors of pressure buttons. Pressure is binary0/255; consumed
// digital masks are active-low. This is sufficient for48Y/48X, not a complete
// physical PS2 packet. No smoothing, deadband, preset or original key binding is
// inferred.48X still applies its original packet4 inversion and conditioning.
//
// Failure preserves output; diagnostic describes every call. Template/output
// may be the same complete object; all other live input/output/diagnostic
// storage must be separate. No references are retained. Pass output to50CX only
// on true; a failed conversion does not authorize reuse as a fresh input sample.
bool composeKeyboardPacket50DR(KeyboardAvailability50DR availability,
    KeyboardHeldSpan50DR held, const KeyboardPacketMapping50DR& mapping,
    KeyboardUnavailablePolicy50DR unavailablePolicy,
    const game::DevicePacket48Y& packetTemplate, game::DevicePacket48Y& output,
    KeyboardPacketDiagnostic50DR& diagnostic) noexcept;
}
