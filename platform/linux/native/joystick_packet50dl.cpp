#include "joystick_packet50dl.h"
#include <cstring>

namespace rr::platform {
static_assert(game::dualShockActiveButtons48Y == 16,
    "50DL requires the original sixteen-button descriptor domain");
namespace {
bool reject(JoystickPacketDiagnostic50DL& diagnostic,
    JoystickPacketError50DL error, std::int32_t index = -1,
    bool buttonMapping = false) noexcept {
    diagnostic = {error, index, buttonMapping};
    return false;
}

bool validCalibration(const JoystickAxisMapping50DL& mapping) noexcept {
    return mapping.minimum >= -32768 && mapping.maximum <= 32767 &&
        mapping.minimum < mapping.center && mapping.center < mapping.maximum;
}

std::uint8_t axisByte(std::int16_t raw, const JoystickAxisMapping50DL& mapping) noexcept {
    std::int32_t value = raw;
    if (value < mapping.minimum) value = mapping.minimum;
    if (value > mapping.maximum) value = mapping.maximum;
    std::int64_t distance = std::int64_t(value) - mapping.center;
    const std::int64_t denominator = distance < 0 ?
        std::int64_t(mapping.center) - mapping.minimum :
        std::int64_t(mapping.maximum) - mapping.center;
    // Invert the centered rational value, never an already-quantized byte.
    if (mapping.inverted) distance = -distance;
    // Exact round-half-up of 255 * (1 + distance/denominator) / 2.
    // All operands are bounded, denominator is positive, and numerator >= 0.
    const std::int64_t numerator = 255 * (denominator + distance) + denominator;
    return static_cast<std::uint8_t>(numerator / (2 * denominator));
}

bool readAxis(const JoystickSnapshot50DK& snapshot, const JoystickAxisMapping50DL& entry,
    std::uint8_t& output, JoystickPacketDiagnostic50DL& diagnostic,
    unsigned index, bool buttonMapping) noexcept {
    if (entry.rawIndex >= snapshot.axisCount || !validCalibration(entry))
        return reject(diagnostic, JoystickPacketError50DL::InvalidAxisMapping, index, buttonMapping);
    const auto& field = snapshot.axes[entry.rawIndex];
    if (!field.observed)
        return reject(diagnostic, JoystickPacketError50DL::UnobservedAxis, index, buttonMapping);
    output = axisByte(field.value, entry);
    return true;
}

bool validDestinations(const game::DeviceButtonDescriptors48Y& descriptors) noexcept {
    // Targets are not configurable. Still refuse an incompatible/colliding
    // numeric profile rather than let future profile edits overwrite axes.
    bool pressureBytes[12]{};
    std::uint16_t digitalMasks = 0;
    for (unsigned i = 0; i < game::dualShockActiveButtons48Y; ++i) {
        const auto& descriptor = descriptors.records[i];
        if (descriptor.flags & 2u) {
            if (descriptor.packetOffset < 8 || descriptor.packetOffset > 19) return false;
            const unsigned index = descriptor.packetOffset - 8;
            if (pressureBytes[index]) return false;
            pressureBytes[index] = true;
        } else {
            const auto mask = descriptor.digitalMask;
            if (!mask || (mask & (mask - 1u)) || (mask & ~0x0f00u) || (mask & digitalMasks))
                return false;
            digitalMasks = static_cast<std::uint16_t>(digitalMasks | mask);
        }
    }
    return digitalMasks == 0x0f00u;
}
}

bool composeJoystickPacket50DL(JoystickConnection50DK connection,
    const JoystickSnapshot50DK& snapshot, const JoystickPacketMapping50DL& mapping,
    const game::DevicePacket48Y& packetTemplate, game::DevicePacket48Y& output,
    JoystickPacketDiagnostic50DL& diagnostic) noexcept {
    diagnostic = {};
    if (connection != JoystickConnection50DK::Open)
        return reject(diagnostic, JoystickPacketError50DL::NotOpen);
    if (!snapshot.metadataKnown)
        return reject(diagnostic, JoystickPacketError50DL::MetadataUnavailable);
    //50DK obtains counts from unsigned-byte ioctls; index storage has256 slots.
    if (snapshot.axisCount > 255 || snapshot.buttonCount > 255)
        return reject(diagnostic, JoystickPacketError50DL::InvalidCounts);

    std::uint8_t axes[4];
    bool pressed[16];
    for (unsigned i = 0; i < 4; ++i) {
        if (!readAxis(snapshot, mapping.axes[i], axes[i], diagnostic, i, false)) return false;
    }
    for (unsigned i = 0; i < game::dualShockActiveButtons48Y; ++i) {
        const auto& entry = mapping.buttons[i];
        switch (entry.source) {
        case JoystickButtonSource50DL::Unspecified:
            return reject(diagnostic, JoystickPacketError50DL::UnspecifiedButtonSource, i, true);
        case JoystickButtonSource50DL::Button: {
            if (entry.input.rawIndex >= snapshot.buttonCount)
                return reject(diagnostic, JoystickPacketError50DL::InvalidButtonMapping, i, true);
            const auto& field = snapshot.buttons[entry.input.rawIndex];
            if (!field.observed)
                return reject(diagnostic, JoystickPacketError50DL::UnobservedButton, i, true);
            if (field.value != 0 && field.value != 1)
                return reject(diagnostic, JoystickPacketError50DL::NonbinaryButton, i, true);
            pressed[i] = (field.value == 1) != entry.input.inverted;
            break;
        }
        case JoystickButtonSource50DL::AxisThreshold: {
            if (!entry.threshold)
                return reject(diagnostic, JoystickPacketError50DL::InvalidAxisThreshold, i, true);
            std::uint8_t value;
            if (!readAxis(snapshot, entry.input, value, diagnostic, i, true)) return false;
            pressed[i] = value >= entry.threshold;
            break;
        }
        case JoystickButtonSource50DL::Released:
            pressed[i] = false;
            break;
        default:
            return reject(diagnostic, JoystickPacketError50DL::InvalidButtonSource, i, true);
        }
    }

    // Reuse the recovered numeric profile rather than copying a physical-label
    // table or accepting arbitrary offsets that could alias axes/ready bytes.
    const auto descriptors = game::initialDualShockDescriptors48Y();
    if (!validDestinations(descriptors))
        return reject(diagnostic, JoystickPacketError50DL::ConflictingDestination);
    game::DevicePacket48Y result;
    std::memcpy(&result, &packetTemplate, sizeof result);
    result.bytes[0] = 0;
    for (unsigned i = 0; i < 4; ++i) result.bytes[4 + i] = axes[i];
    std::uint16_t inactive = static_cast<std::uint16_t>(
        (std::uint32_t(result.bytes[2]) << 8) | result.bytes[3]);
    for (unsigned i = 0; i < game::dualShockActiveButtons48Y; ++i) {
        const auto& descriptor = descriptors.records[i];
        if (descriptor.flags & 2u) {
            result.bytes[descriptor.packetOffset] = pressed[i] ? 255u : 0u;
        } else if (pressed[i]) {
            inactive = static_cast<std::uint16_t>(inactive & ~descriptor.digitalMask);
        } else {
            inactive = static_cast<std::uint16_t>(inactive | descriptor.digitalMask);
        }
    }
    result.bytes[2] = static_cast<std::uint8_t>(inactive >> 8);
    result.bytes[3] = static_cast<std::uint8_t>(inactive & 0xffu);
    std::memcpy(&output, &result, sizeof output);
    return true;
}
}
