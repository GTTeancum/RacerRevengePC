#include "keyboard_packet50dr.h"
#include <cstring>

namespace rr::platform {
static_assert(game::dualShockActiveButtons48Y == 16,
    "50DR requires the original sixteen-button descriptor domain");
namespace {
bool reject(KeyboardPacketDiagnostic50DR& diagnostic,
    KeyboardPacketError50DR error, std::int32_t index = -1,
    bool buttonMapping = false, std::int32_t scancode = -1) noexcept {
    diagnostic = {error, index, buttonMapping, scancode,
        KeyboardPacketDisposition50DR::None};
    return false;
}

bool validScancode(std::uint16_t scancode) noexcept {
    return scancode > 0 && scancode < keyboardScancodeCount50DR;
}

bool validateMapping(const KeyboardPacketMapping50DR& mapping,
    KeyboardPacketDiagnostic50DR& diagnostic) noexcept {
    for (unsigned i = 0; i < 4; ++i) {
        const auto& entry = mapping.axes[i];
        switch (entry.source) {
        case KeyboardAxisSource50DR::Unspecified:
            return reject(diagnostic, KeyboardPacketError50DR::UnspecifiedAxisSource, i);
        case KeyboardAxisSource50DR::Centered:
            break;
        case KeyboardAxisSource50DR::KeyPair:
            if (!validScancode(entry.negativeScancode))
                return reject(diagnostic, KeyboardPacketError50DR::InvalidAxisScancode,
                    i, false, entry.negativeScancode);
            if (!validScancode(entry.positiveScancode))
                return reject(diagnostic, KeyboardPacketError50DR::InvalidAxisScancode,
                    i, false, entry.positiveScancode);
            if (entry.negativeScancode == entry.positiveScancode)
                return reject(diagnostic, KeyboardPacketError50DR::DuplicateAxisScancode, i);
            if (entry.opposingKeys != KeyboardOpposingKeys50DR::Center &&
                entry.opposingKeys != KeyboardOpposingKeys50DR::NegativeWins &&
                entry.opposingKeys != KeyboardOpposingKeys50DR::PositiveWins)
                return reject(diagnostic, KeyboardPacketError50DR::InvalidOpposingKeys, i);
            break;
        default:
            return reject(diagnostic, KeyboardPacketError50DR::InvalidAxisSource, i);
        }
    }
    for (unsigned i = 0; i < game::dualShockActiveButtons48Y; ++i) {
        const auto& entry = mapping.buttons[i];
        switch (entry.source) {
        case KeyboardButtonSource50DR::Unspecified:
            return reject(diagnostic, KeyboardPacketError50DR::UnspecifiedButtonSource, i, true);
        case KeyboardButtonSource50DR::Released:
            break;
        case KeyboardButtonSource50DR::Key:
            if (!validScancode(entry.scancode))
                return reject(diagnostic, KeyboardPacketError50DR::InvalidButtonScancode,
                    i, true, entry.scancode);
            break;
        default:
            return reject(diagnostic, KeyboardPacketError50DR::InvalidButtonSource, i, true);
        }
    }
    return true;
}

bool readKey(KeyboardHeldSpan50DR held, std::uint16_t scancode, bool& pressed,
    KeyboardPacketDiagnostic50DR& diagnostic, unsigned index,
    bool buttonMapping) noexcept {
    const std::uint8_t value = held.held[scancode];
    if (value != 0 && value != 1)
        return reject(diagnostic, KeyboardPacketError50DR::NonbinaryKey,
            index, buttonMapping, scancode);
    pressed = value == 1;
    return true;
}

std::uint8_t axisByte(bool negative, bool positive,
    KeyboardOpposingKeys50DR opposingKeys) noexcept {
    if (!negative && !positive) return 128;
    if (negative && positive) {
        if (opposingKeys == KeyboardOpposingKeys50DR::NegativeWins) return 0;
        if (opposingKeys == KeyboardOpposingKeys50DR::PositiveWins) return 255;
        return 128; // Center; validateMapping already rejected other policies.
    }
    return negative ? 0 : 255;
}

bool validDestinations(const game::DeviceButtonDescriptors48Y& descriptors) noexcept {
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

bool composeKeyboardPacket50DR(KeyboardAvailability50DR availability,
    KeyboardHeldSpan50DR held, const KeyboardPacketMapping50DR& mapping,
    KeyboardUnavailablePolicy50DR unavailablePolicy,
    const game::DevicePacket48Y& packetTemplate, game::DevicePacket48Y& output,
    KeyboardPacketDiagnostic50DR& diagnostic) noexcept {
    diagnostic = {};
    if (unavailablePolicy != KeyboardUnavailablePolicy50DR::Reject &&
        unavailablePolicy != KeyboardUnavailablePolicy50DR::ReleaseAll)
        return reject(diagnostic, KeyboardPacketError50DR::InvalidUnavailablePolicy);
    if (!validateMapping(mapping, diagnostic)) return false;

    const bool live = availability.connected && availability.focused;
    if (!live && unavailablePolicy == KeyboardUnavailablePolicy50DR::Reject)
        return reject(diagnostic, availability.connected ?
            KeyboardPacketError50DR::NotFocused : KeyboardPacketError50DR::NotConnected);
    if (live && (!held.held || held.count != keyboardScancodeCount50DR))
        return reject(diagnostic, KeyboardPacketError50DR::InvalidHeldSpan);

    std::uint8_t axes[4] = {128, 128, 128, 128};
    bool pressed[16]{};
    if (live) {
        for (unsigned i = 0; i < 4; ++i) {
            const auto& entry = mapping.axes[i];
            if (entry.source == KeyboardAxisSource50DR::Centered) continue;
            bool negative, positive;
            if (!readKey(held, entry.negativeScancode, negative, diagnostic, i, false) ||
                !readKey(held, entry.positiveScancode, positive, diagnostic, i, false))
                return false;
            axes[i] = axisByte(negative, positive, entry.opposingKeys);
        }
        for (unsigned i = 0; i < game::dualShockActiveButtons48Y; ++i) {
            const auto& entry = mapping.buttons[i];
            if (entry.source == KeyboardButtonSource50DR::Released) continue;
            if (!readKey(held, entry.scancode, pressed[i], diagnostic, i, true)) return false;
        }
    }

    const auto descriptors = game::initialDualShockDescriptors48Y();
    if (!validDestinations(descriptors))
        return reject(diagnostic, KeyboardPacketError50DR::ConflictingDestination);
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
    diagnostic.disposition = live ? KeyboardPacketDisposition50DR::Live :
        (availability.connected ? KeyboardPacketDisposition50DR::ReleasedUnfocused :
            KeyboardPacketDisposition50DR::ReleasedDisconnected);
    return true;
}
}
