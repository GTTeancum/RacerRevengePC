#pragma once
#include <cstddef>
#include <cstdint>

namespace rr { namespace assets {
template<class T> struct TVCameraSpan49E {
    T* data = nullptr;
    std::size_t size = 0;
    T* begin() const noexcept { return data; }
    T* end() const noexcept { return size ? data + size : data; }
    bool empty() const noexcept { return size == 0; }
    T& operator[](std::size_t i) const noexcept { return data[i]; }
};
struct TVCameraVector49E { float x = 0, y = 0, z = 0, w = 1; };
struct TVCameraProbabilities49E {
    std::int32_t tv = 50, follow = 10, left_side = 10, right_side = 10;
    std::int32_t look_back = 10, first_person = 0, blur = 25;
};
struct TVCameraThresholds49E {
    float tv = 50, follow = 40, left_side = 30, right_side = 20;
    float look_back = 10, first_person = 10, blur = 75;
};
struct TVCameraRecord49E {
    // The first 72 bytes are the proven fields of the original 80-byte camera
    // record. The two untouched words at +48/+4C are deliberately not modeled.
    TVCameraVector49E position, look, up;
    float stationary = 0, max_fov = 70, min_fov = 10;
    float max_fov_distance = 200, min_fov_distance = 20;
    std::int32_t initial_zone = -1; // Authored/default integer, NOT decremented.
    TVCameraProbabilities49E probabilities; // Exact authored/default integers.
    TVCameraThresholds49E thresholds; // Original ordered binary32 preparation.
    std::uint32_t next_begin = 0, next_count = 0;
};
struct TVCameraRequirements49E {
    std::size_t cameras = 0, links = 0, payload_bytes = 0;
};
struct TVCameraStorage49E {
    TVCameraSpan49E<TVCameraRecord49E> cameras;
    TVCameraSpan49E<std::uint32_t> links; // Zero-based camera IDs, authored order.
};
struct TVCameraAccess49E;
class TVCameraView49E {
public:
    bool loaded() const noexcept { return loaded_; }
    float switchEvery() const noexcept { return switch_every_; }
    TVCameraSpan49E<const TVCameraRecord49E> cameras() const noexcept { return cameras_; }
    TVCameraSpan49E<const std::uint32_t> links() const noexcept { return links_; }
    TVCameraRequirements49E requirements() const noexcept;
    const TVCameraRecord49E* camera(std::uint32_t zero_based_id) const noexcept;
    TVCameraSpan49E<const std::uint32_t> nextCameras(std::uint32_t zero_based_id) const noexcept;
private:
    friend struct TVCameraAccess49E;
    TVCameraSpan49E<const TVCameraRecord49E> cameras_;
    TVCameraSpan49E<const std::uint32_t> links_;
    float switch_every_ = 5;
    bool loaded_ = false;
};

enum class TVCameraError49E {
    none, null_input, input_limit, syntax, unsupported_section, duplicate_section,
    unsupported_key, duplicate_key, integer_syntax, integer_range, number_syntax,
    number_range, camera_limit, link_limit, missing_section, nonpositive_cameras, nonpositive_links,
    missing_link, link_target, insufficient_storage, null_storage,
    unsupported_rounding
};
struct TVCameraDiagnostic49E {
    TVCameraError49E code = TVCameraError49E::none;
    std::size_t byte_offset = 0;
};
constexpr std::size_t max_tv_camera_text_bytes49e = 64u * 1024u;
constexpr std::size_t max_tv_cameras49e = 128;
constexpr std::size_t max_tv_camera_links49e = 1024;

// Native admission limits above are NOT claimed as original loader limits.
// Supported text: ASCII spaces/tabs, LF or CRLF lines, semicolon comments;
// exact case-sensitive CameraPositions and Camera1..N sections/known keys.
// Decimal numbers accept a sign, decimal point, e/E exponent and optional f/F
// suffix (at most 128 bytes per token); integer fields accept signed decimal
// int32 without nontrivial leading zeros. Vectors have exactly three
// comma-separated numbers and at most 127 value bytes. Unknown/duplicate
// sections or keys, malformed
// numbers, missing Camera1..N sections and unsafe link targets are rejected.
// Missing known scalar/vector keys use original defaults. NumNextCameras must
// be positive for each camera; NextCamera1..K must all resolve into 1..N.
// NumberOfCameras must be positive, as in the original loader. Self-links,
// repeated targets, cycles, and reversed FOV/distance pairs are admitted.
// A converted FOV other than -1 copies to both FOV fields and ignores the
// MaxFOV/MinFOV values, even when those unused values are not numeric.
//
// Decimal text -> binary64 uses the native standard-library parser, followed
// by the exact recovered soft narrowing helper. This is an explicit ingestion
// boundary, not a reimplementation or proof of the entire original numeric
// parser. Original prefix parsing, base-zero integers and boolean aliases are
// outside admission. No normalization, reordering, clamping or unit changes.
//
// Both functions allocate nothing and use scalar fixed scratch. Requirements
// validates all input and leaves output unchanged on failure. Load validates
// before writing, then deterministically fills caller storage. On failure the
// destination bytes and output view are unchanged; only diagnostic is changed.
// This atomic native contract is stronger than the original incremental loader.
//
// Input must remain unchanged for each call. Every nonempty storage span must
// name live, correctly typed contiguous objects for its whole stated capacity.
// Input, both full storage capacity ranges, storage descriptor, output view,
// and diagnostic must be mutually disjoint. These ordinary typed-storage
// preconditions are not dynamically checked; this is not a pointer diagnostic API.
// Do not mutate/free/reallocate storage while a borrowed view is being used.
// The source bytes may be freed after success. IEEE binary32/binary64 and the
// default nearest-even host rounding mode are required for decimal ingestion
// and probability arithmetic; unsupported rounding is rejected before writes.
bool inspectTVCamera49E(TVCameraSpan49E<const char> text,
    TVCameraRequirements49E& output, TVCameraDiagnostic49E& diagnostic) noexcept;
bool loadTVCamera49E(TVCameraSpan49E<const char> text, const TVCameraStorage49E& storage,
    TVCameraView49E& output, TVCameraDiagnostic49E& diagnostic) noexcept;

// Original integer-only soft binary64 -> binary32 conversion, exposed so the
// byte-derived reference can independently check the ingestion boundary.
// Nonfinite input or a nonfinite original result is outside native admission;
// output is unchanged on failure. Preserves signed zero and original underflow.
bool convertTVCameraNumber49E(double input, float& output) noexcept;
} }
