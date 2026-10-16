#pragma once
#include "../game/authored_parameters49n.h"
#include <cstddef>
#include <cstdint>

namespace rr::assets {
constexpr std::size_t maxPodPhysicsBytes49N = 32768;
constexpr std::size_t maxPodPhysicsLine49N = 255;
constexpr std::size_t maxPodPhysicsSections49N = 32;
constexpr std::size_t maxPodPhysicsKeys49N = 512;
enum class PodPhysicsError49N : std::uint32_t {
    none, invalid_input, text_limit, invalid_character, line_limit, syntax,
    section_limit, key_limit, duplicate_section, duplicate_key,
    numeric_syntax, numeric_range, vector_syntax, integer_syntax, missing_key
};
struct PodPhysicsDiagnostic49N { PodPhysicsError49N code; std::size_t byteOffset; };
struct PodPhysicsToken49N { std::uint32_t keyOffset, valueOffset, valueBytes; };
struct PodPhysicsProvenance49N {
    PodPhysicsToken49N scalars[rr::game::authoredKeyCount49N];
    PodPhysicsToken49N leftEnginePosition, rightEnginePosition;
    PodPhysicsToken49N tuning[5]; // UseValues, UseAValues, UseBValues, UseCValues, UpgradePercent.
};
// Closed native lexical subset of original302E10 ->309C18 ->310440:
// optional +/-; one or more ASCII integer digits; optional dot with >=1 digit;
// <=9 total digits and <=6 fractional digits. No spaces, exponent, suffix,
// locale, NaN/Inf, or trailing text. Zero, including authored -0, becomes +0,
// as original software0-minus-value does. Entirely integer, exact binary64
// nearest-even followed by original normal binary32 nearest-even narrowing.
// Atomic failure; binary64Bits is optional audit output, not retained state.
bool parsePodPhysicsNumber49N(const char* text, std::size_t bytes, float& output,
                             std::uint64_t* binary64Bits = nullptr) noexcept;
// UI DAT extension of the same integer-only conversion: at most seven
// fractional digits, still nine total digits and eleven bytes. Original
// Gasgano Widget15 authors -22.0652073; PHY admission above stays unchanged.
// Same exact binary64 then binary32 rounding, zero and atomic-failure rules.
bool parseOriginalUiNumber49N(const char* text, std::size_t bytes, float& output) noexcept;
// Load-time, serial, bounded and allocation-free. Input is original decoded PHY
// text, not its compressed RES bytes. Caller owns fixed output/provenance.
// All61 mapped scalars and both Engine vectors are required (narrower native
// admission). Tuning section/keys are optional presence only; no defaults here.
// Unknown authored values remain unconsumed. Case-insensitive ASCII names,
// blank lines, whole-line ';' comments, LF/CRLF and external spaces are admitted.
// Reject duplicates even in unconsumed sections, >32 sections/>512 keys,
// >255-byte lines, embedded NUL/non-ASCII, malformed text and unsupported numbers.
// Original lookups allow more forms, first-match duplicates, and scalar missing
// lookup skips writes. These native bounds/atomicity are not original behavior.
// Output and optional provenance are unchanged on failure. Input/output must
// not overlap; source bytes are read-only. Diagnostic is always written.
bool readPodPhysics49N(const char* text, std::size_t bytes,
    rr::game::AuthoredParameters49N& output, PodPhysicsDiagnostic49N& diagnostic,
    PodPhysicsProvenance49N* provenance = nullptr) noexcept;
}
