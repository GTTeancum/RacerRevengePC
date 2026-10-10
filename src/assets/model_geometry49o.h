#pragma once
#include <cstddef>
#include <cstdint>

namespace rr::assets {
constexpr std::size_t maxPsgBytes49O = 8u * 1024u * 1024u;
constexpr std::uint32_t maxModelNodes49O = 64;
constexpr std::size_t maxMotBytes49O = 256u * 1024u;
constexpr std::uint32_t maxMotTracks49O = 64; // Per position/rotation channel.
constexpr std::uint32_t maxMotKeysPerTrack49O = 256;
constexpr std::uint32_t maxMotTotalKeys49O = 4096;
enum class GeometryError49O : std::uint32_t {
    none, invalid_input, file_limit, truncated, version, node_count, node_name,
    matrix_nonfinite, matrix_nonaffine, bounds, parent_order, frame_count,
    duration, track_type, track_count, key_count, total_keys, row_index,
    key_nonfinite, key_time, trailing_bytes, rotation_angle
};
struct GeometryDiagnostic49O { GeometryError49O code; std::size_t byteOffset; };
struct PsgHierarchyView49O {
    const std::uint8_t* data;
    std::size_t bytes;
    std::uint32_t nodeCount;
};
struct PsgNode49O {
    char name[128];
    float local[16];
    float bounds[7]; // center XYZ, half-extent XYZ, radius.
    std::int32_t parent;
};
struct MotTrack49O {
    std::uint32_t rowIndex; // Original model binding row, NOT an automatic PSG node index.
    std::uint32_t keyCount;
    std::uint32_t keyDataOffset;
};
struct MotionView49O {
    const std::uint8_t* data;
    std::size_t bytes;
    std::uint32_t frameCount;
    float duration;
    std::uint32_t positionCount, rotationCount, totalKeys;
    MotTrack49O position[maxMotTracks49O], rotation[maxMotTracks49O];
    std::uint32_t rotationType = 0; // Original MOT +12: ByteAxis0 or ShortAxis1.
};
struct BezierKey49O {
    float time, position[3], incoming[3], outgoing[3];
};
struct ByteAxisKey49O { float time; std::uint8_t angle; std::int8_t axis[3]; };
struct ShortAxisKey49O { float time; std::uint16_t angle; std::int16_t axis[3]; };

// Load-time bounded, allocation-free views over original decoded member bytes.
// Inputs remain alive and unchanged while their views are used. No float is
// canonicalized: all admitted bits, including signed zero, survive access.
// Output is unchanged on failure; diagnostic is always set. These admission
// rules deliberately narrow original loaders' behavior, not emulate all assets.
// PSG: only version3 12+224*N hierarchy prefix is validated. Trailing geometry,
// materials and render streams are NOT checked. Accept 1..64 parent-first nodes,
// nonempty printable-ASCII NUL-terminated/zero-padded names, finite affine local
// matrices, finite centers and nonnegative finite half extents/radii.
bool readPsgHierarchy49O(const std::uint8_t* data, std::size_t bytes,
    PsgHierarchyView49O& output, GeometryDiagnostic49O& diagnostic) noexcept;
// MOT: -2, ByteAxis rotation0 or ShortAxis rotation1 / Bezier position1.
// ShortAxis keys are12 bytes; unsigned angle indexes the original4096-entry
// half-angle tables. Values4096..65535 are rejected before sampling.
// Exact full consumption; neither format is converted into the other.
// Header frameCount1..256, positive finite duration; <=64 tracks per channel,
// 1..256 keys per track, <=4096 total keys, model binding row0..63. Key times finite,
// nonnegative, strictly increasing and <=duration; Bezier values finite.
// Channels/tracks/keys remain in source order, including repeated row indices.
bool readMot49O(const std::uint8_t* data, std::size_t bytes,
    MotionView49O& output, GeometryDiagnostic49O& diagnostic) noexcept;
// Accessors consume a successfully admitted, unmodified view; invalid indexes
// return false without changing output. They perform bytewise LE reads, so no
// host alignment or packed-struct layout assumptions are required.
bool readPsgNode49O(const PsgHierarchyView49O& view, std::uint32_t index,
    PsgNode49O& output) noexcept;
bool readPositionKey49O(const MotionView49O& view, std::uint32_t trackIndex,
    std::uint32_t keyIndex, BezierKey49O& output) noexcept;
bool readRotationKey49O(const MotionView49O& view, std::uint32_t trackIndex,
    std::uint32_t keyIndex, ByteAxisKey49O& output) noexcept;
bool readRotationKey49O(const MotionView49O& view, std::uint32_t trackIndex,
    std::uint32_t keyIndex, ShortAxisKey49O& output) noexcept;
}
