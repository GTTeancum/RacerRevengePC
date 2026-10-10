#pragma once
#include <array>
#include <cstdint>

namespace rr::game {
// Opaque caller-owned identity bits, never a host or guest pointer. The proven
// callback does not dereference, reject zero, or compare against its own body.
struct BodyContactIdentity48R { std::uint32_t value; };
inline constexpr std::uint32_t BodyContactCapacity48R=10;
struct BodyContactListState48R {
    std::int32_t count20C=0;
    std::array<BodyContactIdentity48R,BodyContactCapacity48R> slots210{};
};
// Diagnostics, not an invented original return value (the original is void).
enum class BodyContactListStatus48R { Appended,Duplicate,Full,InvalidInput };
// Known PhysicsRigidBody vtable3CE000+98 -> original165430 only. Signed
// count20C>=10 is the original no-op, including overfull positive counts. With
// 0<=count<10, compare other.value against slots[0,count); append only if absent.
// Every other slot is preserved. Zero, self and arbitrary32-bit identities are
// accepted unchanged. Negative counts are unsupported unsafe original states:
// InvalidInput preserves the entire state; no signed-to-size conversion occurs.
// Capacity is fixed inline, so caller-supplied pointer/length OOB states cannot
// be represented. The state must be a valid, aligned C++ object. Identity is
// captured BY VALUE before mutation: any state slot may be its source, including
// the slot about to be appended. No distinct source/output storage is required.
// No arbitrary virtual callbacks, allocations, guest memory or retained pointers.
BodyContactListStatus48R applyBodyContactList48R(
    BodyContactIdentity48R other,BodyContactListState48R& state) noexcept;
// Separate lifecycle operation: known vtable3CE000+94 -> original165420.
// Clear count20C for every signed bit pattern; preserve all ten slots. Caller
// controls timing. In particular, applyBodyContactList48R never resets itself.
void resetBodyContactList48R(BodyContactListState48R& state) noexcept;
}
