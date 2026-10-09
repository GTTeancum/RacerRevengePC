#pragma once
#include "joystick_transport50dk.h"
#include "../../../src/game/device_held48y.h"
#include <cstdint>

namespace rr::platform {
struct JoystickAxisMapping50DL {
    std::uint16_t rawIndex;
    // Explicit joydev-value calibration, all within [-32768,32767].
    // minimum < center < maximum; values outside this range are clamped.
    std::int32_t minimum, center, maximum;
    bool inverted;
};
enum class JoystickButtonSource50DL : std::uint8_t {
    Unspecified, Button, AxisThreshold, Released
};
struct JoystickButtonMapping50DL {
    // Zero/default initialization is deliberately not a released binding.
    JoystickButtonSource50DL source = JoystickButtonSource50DL::Unspecified;
    // Button uses rawIndex/inverted only (calibration members are ignored).
    // AxisThreshold uses this complete axis mapping, including its inversion.
    // Released ignores input/threshold and explicitly binds this slot released.
    JoystickAxisMapping50DL input{};
    // AxisThreshold only: pressed iff the converted axis byte >= threshold.
    // Must be1..255. Quantization and inversion precede this comparison.
    std::uint8_t threshold = 0;
};
struct JoystickPacketMapping50DL {
    // Array positions select packet offsets 4,5,6,7, not physical stick labels.
    JoystickAxisMapping50DL axes[4]{};
    // Array positions select original numeric descriptor codes 0 through 15.
    JoystickButtonMapping50DL buttons[16];
};

enum class JoystickPacketError50DL : std::uint8_t {
    None, NotOpen, MetadataUnavailable, InvalidCounts,
    InvalidAxisMapping, InvalidButtonMapping, UnspecifiedButtonSource,
    InvalidButtonSource, InvalidAxisThreshold, UnobservedAxis, UnobservedButton,
    NonbinaryButton, ConflictingDestination
};
struct JoystickPacketDiagnostic50DL {
    JoystickPacketError50DL error = JoystickPacketError50DL::None;
    // Mapping array index for a field-specific error; otherwise -1. Axis errors
    // inside a button binding report that button slot and buttonMapping=true.
    std::int32_t mappingIndex = -1;
    bool buttonMapping = false;
};

// Small host adapter, NOT recovered PS2 transport. Requires all four axes and
// all sixteen button slots explicitly configured. Repeated raw source indices
// are allowed, including independent calibrations/inversions for each binding.
// Only the exact initialDualShockDescriptors48Y() layout with
// dualShockActiveButtons48Y (16) is supported at the downstream reader.
// There is no custom-descriptor, omitted-field, preset or physical-label path.
// Released is an explicit disabled binding, not an unobserved-input fallback.
//
// connection and snapshot must come from the same50DK owner/open session with
// no intervening mutation. Open + metadataKnown + every mapped field observed
// are necessary. Open is not a guarantee against as-yet-undetected unplugging;
// cached observations are not an atomic/fresh whole-device read. Caller owns
// polling, cadence, initialization edges and the policy for failed conversion.
// No packet or ready/neutral fallback is published on a failure.
//
// Host axis policy: clamp to explicit min/max, normalize each side of center
// to [-1,+1], invert that signed value, then map to [0,255] and round nearest,
// with exact half-byte ties toward the larger byte. Integer rational arithmetic
// makes center exactly128 with either inversion.48X still applies its own
// packet4 inversion and source deadband; neither is removed or duplicated here.
// Button sources accept only observed0/1 and apply explicit inversion. A caller
// can instead select AxisThreshold, comparing a calibrated axis byte with an
// explicit threshold, or Released. Every kind writes binary pressure0/255 or an
// active-low digital bit according to the original layout. No analog pressure
// or inferred nonzero-button convention is provided. Destinations are fixed by
// array position; any descriptor collision with axes/ready/another button fails.
//
// Writes only byte0=0, axes4..7, pressure8..19 and the four consumed digital
// mask bits0x0F00 (in byte2). All other bytes/bits come verbatim from the template,
// including ignored digital mirrors of pressure buttons. The result is only
// sufficient for these48Y/48X readers, not a complete physical PS2 packet.
//
// Stateless, bounded and allocation-free. Failure preserves output; diagnostic
// describes every call. Template and output may be the same complete object.
// Other inputs/diagnostic/output must be separate live storage. No references
// are retained. Pass output to50CX only after this call returns true.
bool composeJoystickPacket50DL(JoystickConnection50DK connection,
    const JoystickSnapshot50DK& snapshot, const JoystickPacketMapping50DL& mapping,
    const game::DevicePacket48Y& packetTemplate, game::DevicePacket48Y& output,
    JoystickPacketDiagnostic50DL& diagnostic) noexcept;
}
