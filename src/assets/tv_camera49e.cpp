#include "tv_camera49e.h"
#include <cctype>
#include <cfenv>
#include <charconv>
#include <cstddef>
#include <cstring>
#include <limits>

namespace rr { namespace assets {
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559,
    "TV camera data requires IEEE binary32");
static_assert(sizeof(double) == 8 && std::numeric_limits<double>::is_iec559,
    "TV camera data requires IEEE binary64");
static_assert(sizeof(TVCameraVector49E) == 16, "Unexpected camera vector layout");
static_assert(sizeof(TVCameraRecord49E) == 136, "Unexpected camera record layout");
static_assert(offsetof(TVCameraRecord49E, position) == 0x00 &&
    offsetof(TVCameraRecord49E, look) == 0x10 &&
    offsetof(TVCameraRecord49E, up) == 0x20 &&
    offsetof(TVCameraRecord49E, stationary) == 0x30 &&
    offsetof(TVCameraRecord49E, max_fov) == 0x34 &&
    offsetof(TVCameraRecord49E, min_fov) == 0x38 &&
    offsetof(TVCameraRecord49E, max_fov_distance) == 0x3c &&
    offsetof(TVCameraRecord49E, min_fov_distance) == 0x40 &&
    offsetof(TVCameraRecord49E, initial_zone) == 0x44,
    "Proven camera fields must retain their source offsets");

bool convertTVCameraNumber49E(double input, float& output) noexcept {
    std::uint64_t bits = 0;
    std::memcpy(&bits, &input, sizeof(bits));
    const std::uint32_t sign = static_cast<std::uint32_t>(bits >> 63) << 31;
    const unsigned exponent = static_cast<unsigned>((bits >> 52) & 0x7ffu);
    if (exponent == 0x7ffu) return false;
    std::uint32_t result = sign;
    if (exponent != 0) {
        int e = static_cast<int>(exponent) - 1023;
        const std::uint64_t significand = (std::uint64_t(1) << 52) |
            (bits & ((std::uint64_t(1) << 52) - 1));
        std::uint32_t m = static_cast<std::uint32_t>(significand >> 22);
        if (significand & ((std::uint64_t(1) << 22) - 1)) m |= 1;
        if (e < -126) {
            const int shift = -126 - e;
            m = shift < 26 ? m >> shift : 0;
            result |= m >> 7; // Original underflow path truncates, without rounding.
        } else {
            if (e > 127) return false;
            if ((m & 127u) == 64u) {
                if (m & 128u) m += 64u;
            } else m += 63u;
            if (m & 0x80000000u) { m >>= 1; ++e; }
            if (e > 127) return false;
            result |= static_cast<std::uint32_t>(e + 127) << 23;
            result |= (m >> 7) & 0x7fffffu;
        }
    }
    std::memcpy(&output, &result, sizeof(output));
    return true;
}

namespace {
struct Slice {
    const char* data = nullptr;
    std::size_t size = 0, offset = 0;
};
bool fail(TVCameraDiagnostic49E& d, TVCameraError49E code, std::size_t offset) noexcept {
    d = {code, offset}; return false;
}
bool space(char c) noexcept { return c == ' ' || c == '\t' || c == '\r'; }
bool digit(char c) noexcept { return c >= '0' && c <= '9'; }
Slice trim(Slice s) noexcept {
    while (s.size && space(s.data[0])) { ++s.data; --s.size; ++s.offset; }
    while (s.size && space(s.data[s.size - 1])) --s.size;
    return s;
}
template<std::size_t N> bool equal(Slice s, const char (&literal)[N]) noexcept {
    return s.size == N - 1 && std::memcmp(s.data, literal, N - 1) == 0;
}
template<std::size_t N> bool prefix(Slice s, const char (&literal)[N]) noexcept {
    return s.size >= N - 1 && std::memcmp(s.data, literal, N - 1) == 0;
}
class Lines {
public:
    explicit Lines(TVCameraSpan49E<const char> text) noexcept : text_(text) {}
    bool next(Slice& out) noexcept {
        while (at_ < text_.size) {
            const std::size_t begin = at_;
            while (at_ < text_.size && text_.data[at_] != '\n') ++at_;
            std::size_t end = at_;
            if (at_ < text_.size) ++at_;
            for (std::size_t i = begin; i < end; ++i)
                if (text_.data[i] == ';') { end = i; break; }
            out = trim({text_.data + begin, end - begin, begin});
            if (out.size) return true;
        }
        return false;
    }
private:
    TVCameraSpan49E<const char> text_;
    std::size_t at_ = 0;
};
bool split(Slice line, Slice& key, Slice& value, TVCameraDiagnostic49E& d) noexcept {
    std::size_t i = 0;
    while (i < line.size && line.data[i] != '=') ++i;
    if (i == line.size) return fail(d, TVCameraError49E::syntax, line.offset);
    key = trim({line.data, i, line.offset});
    value = trim({line.data + i + 1, line.size - i - 1, line.offset + i + 1});
    if (!key.size) return fail(d, TVCameraError49E::syntax, line.offset);
    return true;
}
bool integer(Slice s, std::int32_t& output, TVCameraDiagnostic49E& d) noexcept {
    // The original integer read (158A34 -> strtol(s, 0, 0), 30AE84) converts
    // the longest valid prefix in base zero: optional sign, 0x/0X hex, a
    // leading 0 octal, else decimal; no digits reads 0. Course data relies on
    // it: MC "InitialZone=09" reads 0 (octal stops at 9), RC "InitialZone=10,"
    // reads 10, TC "RightSide=" reads 0. A value outside int32 is refused
    // (native limit; the original truncates its 64-bit long).
    std::size_t i = 0;
    while (i < s.size && (s.data[i] == ' ' || s.data[i] == '\t')) ++i;
    bool negative = false;
    if (i < s.size && (s.data[i] == '-' || s.data[i] == '+')) negative = s.data[i++] == '-';
    unsigned base = 10;
    if (i + 1 < s.size && s.data[i] == '0' && (s.data[i + 1] == 'x' || s.data[i + 1] == 'X') &&
        i + 2 < s.size && std::isxdigit(static_cast<unsigned char>(s.data[i + 2]))) { base = 16; i += 2; }
    else if (i < s.size && s.data[i] == '0') base = 8;
    const std::uint32_t limit = negative ? 0x80000000u : 0x7fffffffu;
    std::uint32_t magnitude = 0;
    for (; i < s.size; ++i) {
        const char c = s.data[i];
        unsigned next = 0;
        if (digit(c)) next = static_cast<unsigned>(c - '0');
        else if (c >= 'a' && c <= 'f') next = static_cast<unsigned>(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') next = static_cast<unsigned>(c - 'A' + 10);
        else break;
        if (next >= base) break;
        if (magnitude > (limit - next) / base)
            return fail(d, TVCameraError49E::integer_range, s.offset);
        magnitude = magnitude * base + next;
    }
    output = negative ? (magnitude == 0x80000000u ?
        std::numeric_limits<std::int32_t>::min() : -static_cast<std::int32_t>(magnitude)) :
        static_cast<std::int32_t>(magnitude);
    return true;
}
bool ordinal(Slice s, std::uint32_t& output, TVCameraDiagnostic49E& d) noexcept {
    if (!s.size || s.data[0] < '1' || s.data[0] > '9')
        return fail(d, TVCameraError49E::syntax, s.offset);
    std::int32_t number = 0;
    if (!integer(s, number, d)) return false;
    output = static_cast<std::uint32_t>(number);
    return true;
}
// Global section is zero; camera sections have their one-based authored number.
constexpr std::uint32_t unlisted_section = 0xffffffffu;
bool section(Slice line, std::uint32_t& id, TVCameraDiagnostic49E& d) noexcept {
    if (line.size < 3 || line.data[0] != '[' || line.data[line.size - 1] != ']')
        return fail(d, TVCameraError49E::syntax, line.offset);
    const Slice name{line.data + 1, line.size - 2, line.offset + 1};
    if (equal(name, "CameraPositions")) { id = 0; return true; }
    // 2D29F0 asks 157D00 for "CameraPositions" and sprintf("Camera%d") names
    // only. Any other section ("[Camera09]" in SA) is never looked up: its
    // lines are never read (the camera it was meant to be then repeats the
    // previous record, see process()).
    bool canonical = prefix(name, "Camera") && name.size > 6 && name.data[6] >= '1' && name.data[6] <= '9';
    for (std::size_t i = 6; canonical && i < name.size; ++i) canonical = digit(name.data[i]);
    if (!canonical) { id = unlisted_section; return true; }
    if (!ordinal({name.data + 6, name.size - 6, name.offset + 6}, id, d)) return false;
    if (id > max_tv_cameras49e) return fail(d, TVCameraError49E::camera_limit, name.offset);
    return true;
}
bool decimal(Slice s, float& output, TVCameraDiagnostic49E& d) noexcept {
    // 158B80/158C00 convert each value with atof (302E10 -> strtod(s, 0)):
    // the longest numeric prefix, 0.0 when there is none (MC Camera "2117.51w").
    if (!s.size) { output = 0.f; return true; }
    if (s.size > 128) return fail(d, TVCameraError49E::number_range, s.offset);
    if (s.data[s.size - 1] == 'f' || s.data[s.size - 1] == 'F') --s.size;
    if (!s.size) { output = 0.f; return true; }
    std::size_t i = 0;
    const bool negative = s.data[0] == '-';
    if (negative || s.data[0] == '+') ++i;
    int before_point = 0, digits_seen = 0, first_nonzero = -1;
    while (i < s.size && digit(s.data[i])) {
        if (first_nonzero < 0 && s.data[i] != '0') first_nonzero = digits_seen;
        ++i; ++before_point; ++digits_seen;
    }
    if (i < s.size && s.data[i] == '.') {
        ++i;
        while (i < s.size && digit(s.data[i])) {
            if (first_nonzero < 0 && s.data[i] != '0') first_nonzero = digits_seen;
            ++i; ++digits_seen;
        }
    }
    if (!digits_seen) { output = 0.f; return true; }
    int exponent = 0;
    if (i < s.size && (s.data[i] == 'e' || s.data[i] == 'E')) {
        const std::size_t mark = i;
        ++i;
        const bool exponent_negative = i < s.size && s.data[i] == '-';
        if (i < s.size && (s.data[i] == '+' || s.data[i] == '-')) ++i;
        const std::size_t exponent_begin = i;
        while (i < s.size && digit(s.data[i])) {
            if (exponent < 10000) exponent = exponent * 10 + (s.data[i] - '0');
            ++i;
        }
        if (i == exponent_begin) { i = mark; exponent = 0; }   // strtod backs off a bare 'e'
        else if (exponent_negative) exponent = -exponent;
    }
    s.size = i;                                                   // atof prefix
    double parsed = 0;
    const char* begin = s.data + (s.data[0] == '+' ? 1 : 0);
    const auto result = std::from_chars(begin, s.data + s.size, parsed, std::chars_format::general);
    if (result.ec == std::errc::result_out_of_range &&
        (first_nonzero < 0 || exponent + before_point - first_nonzero - 1 < -308)) {
        parsed = negative ? -0.0 : 0.0;
    } else if (result.ec != std::errc{} || result.ptr != s.data + s.size) {
        return fail(d, TVCameraError49E::number_range, s.offset);
    }
    if (!convertTVCameraNumber49E(parsed, output))
        return fail(d, TVCameraError49E::number_range, s.offset);
    return true;
}
bool vector(Slice s, TVCameraVector49E& output, TVCameraDiagnostic49E& d) noexcept {
    // The original lookup copies at most 128 bytes before component tokenizing.
    if (s.size > 127) return fail(d, TVCameraError49E::number_range, s.offset);
    TVCameraVector49E value;
    std::size_t begin = 0;
    for (unsigned component = 0; component < 3; ++component) {
        std::size_t end = begin;
        while (end < s.size && s.data[end] != ',') ++end;
        if ((component == 2) != (end == s.size))
            return fail(d, TVCameraError49E::number_syntax, s.offset + end);
        float parsed = 0;
        if (!decimal(trim({s.data + begin, end - begin, s.offset + begin}), parsed, d)) return false;
        if (component == 0) value.x = parsed;
        else if (component == 1) value.y = parsed;
        else value.z = parsed;
        begin = end + 1;
    }
    output = value; return true;
}
enum class Key : unsigned {
    position, look, up, stationary, fov, max_fov, min_fov, max_distance,
    min_distance, zone, next_count, tv, follow, left_side, right_side,
    look_back, first_person, blur, link, unknown
};
Key identify(Slice key) noexcept {
    if (equal(key, "Position")) return Key::position;
    if (equal(key, "Look")) return Key::look;
    if (equal(key, "Up")) return Key::up;
    if (equal(key, "Stationary")) return Key::stationary;
    if (equal(key, "FOV")) return Key::fov;
    if (equal(key, "MaxFOV")) return Key::max_fov;
    if (equal(key, "MinFOV")) return Key::min_fov;
    if (equal(key, "MaxFOVDistance")) return Key::max_distance;
    if (equal(key, "MinFOVDistance")) return Key::min_distance;
    if (equal(key, "InitialZone")) return Key::zone;
    if (equal(key, "NumNextCameras")) return Key::next_count;
    if (equal(key, "TV")) return Key::tv;
    if (equal(key, "Follow")) return Key::follow;
    if (equal(key, "LeftSide")) return Key::left_side;
    if (equal(key, "RightSide")) return Key::right_side;
    if (equal(key, "LookBack")) return Key::look_back;
    if (equal(key, "FirstPerson")) return Key::first_person;
    if (equal(key, "Blur")) return Key::blur;
    return prefix(key, "NextCamera") ? Key::link : Key::unknown;
}
float integerFloat(std::int32_t input) noexcept {
    float output = 0;
    (void)convertTVCameraNumber49E(static_cast<double>(input), output);
    return output;
}
std::int32_t wrappedComplement(std::int32_t input) noexcept {
    const std::uint32_t bits = 100u - static_cast<std::uint32_t>(input);
    return bits <= 0x7fffffffu ? static_cast<std::int32_t>(bits) :
        -1 - static_cast<std::int32_t>(0xffffffffu - bits);
}
float subtract(float a, float b) noexcept { volatile float result = a - b; return result; }
void thresholds(TVCameraRecord49E& camera) noexcept {
    const auto& p = camera.probabilities;
    auto& t = camera.thresholds;
    t.tv = integerFloat(wrappedComplement(p.tv));
    t.follow = subtract(t.tv, integerFloat(p.follow));
    t.left_side = subtract(t.follow, integerFloat(p.left_side));
    t.right_side = subtract(t.left_side, integerFloat(p.right_side));
    t.look_back = subtract(t.right_side, integerFloat(p.look_back));
    t.first_person = subtract(t.look_back, integerFloat(p.first_person));
    t.blur = integerFloat(wrappedComplement(p.blur));
}
struct Metadata { std::uint32_t cameras = 0; float switch_every = 5; };
bool metadata(TVCameraSpan49E<const char> text, Metadata& result,
    TVCameraDiagnostic49E& d) noexcept {
    Lines lines(text); Slice line;
    bool global_seen = false, any_section = false;
    std::uint32_t current = 0, largest_camera = 0, seen = 0;
    while (lines.next(line)) {
        if (line.data[0] == '[') {
            if (!section(line, current, d)) return false;
            any_section = true;
            if (current == unlisted_section) continue;
            if (current == 0) {
                if (global_seen) return fail(d, TVCameraError49E::duplicate_section, line.offset);
                global_seen = true;
            } else if (current > largest_camera) largest_camera = current;
            continue;
        }
        if (!any_section) return fail(d, TVCameraError49E::missing_section, line.offset);
        Slice key, value;
        if (!split(line, key, value, d)) return false;
        if (current != 0) continue;
        unsigned bit = 0;
        if (equal(key, "NumberOfCameras")) bit = 1;
        else if (equal(key, "SwitchEvery")) bit = 2;
        else continue;   // 2D29F0 reads only these two global keys
        if (seen & bit) return fail(d, TVCameraError49E::duplicate_key, key.offset);
        seen |= bit;
        if (bit == 1) {
            std::int32_t count = 0;
            if (!integer(value, count, d)) return false;
            if (count <= 0) return fail(d, TVCameraError49E::nonpositive_cameras, value.offset);
            if (static_cast<std::uint32_t>(count) > max_tv_cameras49e)
                return fail(d, TVCameraError49E::camera_limit, value.offset);
            result.cameras = static_cast<std::uint32_t>(count);
        } else if (!decimal(value, result.switch_every, d)) return false;
    }
    if (!global_seen) return fail(d, TVCameraError49E::missing_section, 0);
    if (result.cameras == 0) return fail(d, TVCameraError49E::nonpositive_cameras, 0);
    // Sections beyond NumberOfCameras are never looked up by 2D2BAC's loop.
    (void)largest_camera;
    return true;
}
bool cameraRecord(TVCameraSpan49E<const char> text, std::uint32_t id,
    TVCameraRecord49E& output, TVCameraDiagnostic49E& d) noexcept {
    Lines lines(text); Slice line, maximum, minimum;
    bool active = false, found = false;
    std::uint32_t current = 0, seen = 0, observed_links = 0, largest_link = 0;
    float fov = -1;
    std::int32_t next_count = 0;
    while (lines.next(line)) {
        if (line.data[0] == '[') {
            if (!section(line, current, d)) return false;
            active = current == id;
            if (active) {
                if (found) return fail(d, TVCameraError49E::duplicate_section, line.offset);
                found = true;
            }
            continue;
        }
        if (!active) continue;
        Slice key, value;
        if (!split(line, key, value, d)) return false;
        const Key kind = identify(key);
        // 2D29F0 asks only for the keys it knows; any other line is never read
        // (TC Camera17 "FirstPersonq=0" -> FirstPerson keeps its default 0).
        if (kind == Key::unknown) continue;
        if (kind == Key::link) {
            std::uint32_t link = 0;
            if (!ordinal({key.data + 10, key.size - 10, key.offset + 10}, link, d)) return false;
            if (link > max_tv_camera_links49e || observed_links >= max_tv_camera_links49e)
                return fail(d, TVCameraError49E::link_limit, key.offset);
            ++observed_links;
            if (link > largest_link) largest_link = link;
            continue;
        }
        const std::uint32_t bit = 1u << static_cast<unsigned>(kind);
        if (seen & bit) return fail(d, TVCameraError49E::duplicate_key, key.offset);
        seen |= bit;
        switch (kind) {
        case Key::position: if (!vector(value, output.position, d)) return false; break;
        case Key::look: if (!vector(value, output.look, d)) return false; break;
        case Key::up: if (!vector(value, output.up, d)) return false; break;
        case Key::stationary: if (!decimal(value, output.stationary, d)) return false; break;
        case Key::fov: if (!decimal(value, fov, d)) return false; break;
        case Key::max_fov: maximum = value; break;
        case Key::min_fov: minimum = value; break;
        case Key::max_distance: if (!decimal(value, output.max_fov_distance, d)) return false; break;
        case Key::min_distance: if (!decimal(value, output.min_fov_distance, d)) return false; break;
        case Key::zone: if (!integer(value, output.initial_zone, d)) return false; break;
        case Key::next_count: if (!integer(value, next_count, d)) return false; break;
        case Key::tv: if (!integer(value, output.probabilities.tv, d)) return false; break;
        case Key::follow: if (!integer(value, output.probabilities.follow, d)) return false; break;
        case Key::left_side: if (!integer(value, output.probabilities.left_side, d)) return false; break;
        case Key::right_side: if (!integer(value, output.probabilities.right_side, d)) return false; break;
        case Key::look_back: if (!integer(value, output.probabilities.look_back, d)) return false; break;
        case Key::first_person: if (!integer(value, output.probabilities.first_person, d)) return false; break;
        case Key::blur: if (!integer(value, output.probabilities.blur, d)) return false; break;
        default: break;
        }
    }
    if (!found) return fail(d, TVCameraError49E::missing_section, 0);
    if (fov == -1) {
        if ((seen & (1u << static_cast<unsigned>(Key::max_fov))) && !decimal(maximum, output.max_fov, d)) return false;
        if ((seen & (1u << static_cast<unsigned>(Key::min_fov))) && !decimal(minimum, output.min_fov, d)) return false;
    } else output.max_fov = output.min_fov = fov;
    if (next_count <= 0) return fail(d, TVCameraError49E::nonpositive_links, 0);
    if (static_cast<std::uint32_t>(next_count) > max_tv_camera_links49e)
        return fail(d, TVCameraError49E::link_limit, 0);
    output.next_count = static_cast<std::uint32_t>(next_count);
    if (largest_link > output.next_count) return fail(d, TVCameraError49E::unsupported_key, 0);
    if (observed_links < output.next_count) return fail(d, TVCameraError49E::missing_link, 0);
    thresholds(output);
    return true;
}
bool linkTarget(TVCameraSpan49E<const char> text, std::uint32_t camera_id,
    std::uint32_t link_id, std::uint32_t camera_count, std::uint32_t& target,
    TVCameraDiagnostic49E& d) noexcept {
    Lines lines(text); Slice line;
    bool active = false, found = false;
    std::uint32_t current = 0;
    while (lines.next(line)) {
        if (line.data[0] == '[') {
            if (!section(line, current, d)) return false;
            active = current == camera_id; continue;
        }
        if (!active) continue;
        Slice key, value;
        if (!split(line, key, value, d)) return false;
        if (!prefix(key, "NextCamera")) continue;
        std::uint32_t ordinal_value = 0;
        if (!ordinal({key.data + 10, key.size - 10, key.offset + 10}, ordinal_value, d)) return false;
        if (ordinal_value != link_id) continue;
        if (found) return fail(d, TVCameraError49E::duplicate_key, key.offset);
        found = true;
        std::int32_t authored = -1;
        if (!integer(value, authored, d)) return false;
        // 2D2D3C..2D2D68 stores value - 1 with no range check (SA Camera22
        // "NextCamera1=24" of 23 cameras). Kept as authored; the selector
        // skips a target outside the camera list (see tv_camera_selection49e).
        (void)camera_count;
        target = static_cast<std::uint32_t>(authored) - 1u;
    }
    if (!found) return fail(d, TVCameraError49E::missing_link, 0);
    return true;
}
bool admission(TVCameraSpan49E<const char> text, TVCameraDiagnostic49E& d) noexcept {
    if (text.size > max_tv_camera_text_bytes49e) return fail(d, TVCameraError49E::input_limit, 0);
    if (text.size && !text.data) return fail(d, TVCameraError49E::null_input, 0);
    if (std::fegetround() != FE_TONEAREST) return fail(d, TVCameraError49E::unsupported_rounding, 0);
    for (std::size_t i = 0; i < text.size; ++i) {
        const auto c = static_cast<unsigned char>(text.data[i]);
        if ((c < 32 && c != '\t' && c != '\r' && c != '\n') || c > 126)
            return fail(d, TVCameraError49E::syntax, i);
        if (c == '\r' && (i + 1 == text.size || text.data[i + 1] != '\n'))
            return fail(d, TVCameraError49E::syntax, i);
    }
    return true;
}
bool process(TVCameraSpan49E<const char> text, const Metadata& meta,
    const TVCameraStorage49E* storage, TVCameraRequirements49E& requirements,
    TVCameraDiagnostic49E& d) noexcept {
    std::size_t links = 0;
    // 2D2B94..2D2BB0 ignores 157D00's result. 157D00 returns before replacing
    // the current section's line list when [CameraN] is absent, so camera N
    // reads the previous section's lines: an identical record and links
    // (GA declares 16 cameras and authors Camera1..15). Camera1 absent would
    // read [CameraPositions] and reach the NumNextCameras assert: still refused.
    TVCameraRecord49E previous; std::uint32_t previous_id = 0;
    for (std::uint32_t i = 1; i <= meta.cameras; ++i) {
        TVCameraRecord49E record;
        std::uint32_t source_id = i;
        if (!cameraRecord(text, i, record, d)) {
            if (d.code != TVCameraError49E::missing_section || !previous_id) return false;
            d = {}; record = previous; source_id = previous_id;
        }
        if (record.next_count > max_tv_camera_links49e - links)
            return fail(d, TVCameraError49E::link_limit, 0);
        record.next_begin = static_cast<std::uint32_t>(links);
        for (std::uint32_t j = 1; j <= record.next_count; ++j) {
            std::uint32_t target = 0;
            if (!linkTarget(text, source_id, j, meta.cameras, target, d)) return false;
            if (storage) storage->links[links + j - 1] = target;
        }
        if (storage) storage->cameras[i - 1] = record;
        links += record.next_count;
        previous = record; previous_id = source_id;
    }
    requirements = {meta.cameras, links,
        meta.cameras * sizeof(TVCameraRecord49E) + links * sizeof(std::uint32_t)};
    return true;
}
} // namespace

struct TVCameraAccess49E {
    static TVCameraView49E view(const TVCameraStorage49E& storage,
        const TVCameraRequirements49E& requirements, float switch_every) noexcept {
        TVCameraView49E result;
        result.cameras_ = {storage.cameras.data, requirements.cameras};
        result.links_ = {storage.links.data, requirements.links};
        result.switch_every_ = switch_every;
        result.loaded_ = true;
        return result;
    }
};
TVCameraRequirements49E TVCameraView49E::requirements() const noexcept {
    return {cameras_.size, links_.size,
        cameras_.size * sizeof(TVCameraRecord49E) + links_.size * sizeof(std::uint32_t)};
}
const TVCameraRecord49E* TVCameraView49E::camera(std::uint32_t id) const noexcept {
    return id < cameras_.size ? cameras_.data + id : nullptr;
}
TVCameraSpan49E<const std::uint32_t> TVCameraView49E::nextCameras(std::uint32_t id) const noexcept {
    const auto* record = camera(id);
    return record ? TVCameraSpan49E<const std::uint32_t>{links_.data + record->next_begin, record->next_count} :
        TVCameraSpan49E<const std::uint32_t>{};
}
bool inspectTVCamera49E(TVCameraSpan49E<const char> text,
    TVCameraRequirements49E& output, TVCameraDiagnostic49E& diagnostic) noexcept {
    if (!admission(text, diagnostic)) return false;
    Metadata meta;
    TVCameraRequirements49E requirements;
    if (!metadata(text, meta, diagnostic) || !process(text, meta, nullptr, requirements, diagnostic)) return false;
    output = requirements;
    diagnostic = {};
    return true;
}
bool loadTVCamera49E(TVCameraSpan49E<const char> text, const TVCameraStorage49E& storage,
    TVCameraView49E& output, TVCameraDiagnostic49E& diagnostic) noexcept {
    if (!admission(text, diagnostic)) return false;
    Metadata meta;
    TVCameraRequirements49E requirements;
    if (!metadata(text, meta, diagnostic) || !process(text, meta, nullptr, requirements, diagnostic)) return false;
    if (storage.cameras.size < requirements.cameras || storage.links.size < requirements.links)
        return fail(diagnostic, TVCameraError49E::insufficient_storage, 0);
    if ((requirements.cameras && !storage.cameras.data) || (requirements.links && !storage.links.data))
        return fail(diagnostic, TVCameraError49E::null_storage, 0);
    // All input was validated without writes. With the documented stable input
    // and typed/disjoint storage, this deterministic second pass cannot fail.
    (void)process(text, meta, &storage, requirements, diagnostic);
    output = TVCameraAccess49E::view(storage, requirements, meta.switch_every);
    diagnostic = {};
    return true;
}
} }
