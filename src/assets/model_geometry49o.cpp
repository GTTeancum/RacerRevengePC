#include "model_geometry49o.h"
#include <cmath>
#include <cstring>
#include <initializer_list>
#include <limits>

namespace rr::assets {
namespace {
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559,
              "49O reader requires IEEE binary32");
std::uint32_t u32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
           (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}
float f32(const std::uint8_t* p) noexcept {
    const auto b = u32(p); float f; std::memcpy(&f, &b, 4); return f;
}
bool reject(GeometryDiagnostic49O& d, GeometryError49O c, std::size_t at) noexcept {
    d = {c, at}; return false;
}
bool range(std::size_t bytes, std::size_t at, std::size_t count) noexcept {
    return at <= bytes && count <= bytes - at;
}
}
bool readPsgHierarchy49O(const std::uint8_t* data, std::size_t bytes,
    PsgHierarchyView49O& output, GeometryDiagnostic49O& d) noexcept {
    d = {};
    if (!data) return reject(d, GeometryError49O::invalid_input, 0);
    if (bytes > maxPsgBytes49O) return reject(d, GeometryError49O::file_limit, 0);
    if (bytes < 12) return reject(d, GeometryError49O::truncated, bytes);
    if (u32(data) != 0x00677370u || u32(data + 4) != 3)
        return reject(d, GeometryError49O::version, 0);
    const auto count = u32(data + 8);
    if (!count || count > maxModelNodes49O) return reject(d, GeometryError49O::node_count, 8);
    if (!range(bytes, 12, std::size_t(count) * 224)) return reject(d, GeometryError49O::truncated, bytes);
    for (std::uint32_t i = 0; i < count; ++i) {
        const std::size_t at = 12 + std::size_t(i) * 224;
        bool ended = false;
        for (std::size_t j = 0; j < 128; ++j) {
            const auto c = data[at + j];
            if ((!j && !c) || (ended && c) || (c && (c < 32 || c > 126)))
                return reject(d, GeometryError49O::node_name, at + j);
            ended = ended || !c;
        }
        if (!ended) return reject(d, GeometryError49O::node_name, at + 127);
        for (std::size_t j = 0; j < 16; ++j)
            if (!std::isfinite(f32(data + at + 128 + 4 * j)))
                return reject(d, GeometryError49O::matrix_nonfinite, at + 128 + 4 * j);
        for (const std::size_t j : {3u, 7u, 11u, 15u})
            if (f32(data + at + 128 + 4 * j) != (j == 15 ? 1.0f : 0.0f))
                return reject(d, GeometryError49O::matrix_nonaffine, at + 128 + 4 * j);
        for (std::size_t j = 0; j < 7; ++j) {
            const float f = f32(data + at + 192 + 4 * j);
            if (!std::isfinite(f) || (j >= 3 && f < 0)) return reject(d, GeometryError49O::bounds, at + 192 + 4 * j);
        }
        const auto parent = u32(data + at + 220);
        if (parent != 0xffffffffu && parent >= i) return reject(d, GeometryError49O::parent_order, at + 220);
    }
    output = {data, bytes, count}; return true;
}
bool readPsgNode49O(const PsgHierarchyView49O& v, std::uint32_t index,
    PsgNode49O& output) noexcept {
    if (!v.data || !v.nodeCount || v.nodeCount > maxModelNodes49O || index >= v.nodeCount) return false;
    const std::size_t at = 12 + std::size_t(index) * 224;
    if (!range(v.bytes, at, 224)) return false;
    PsgNode49O n;
    std::memcpy(n.name, v.data + at, 128);
    for (std::size_t j = 0; j < 16; ++j) n.local[j] = f32(v.data + at + 128 + 4 * j);
    for (std::size_t j = 0; j < 7; ++j) n.bounds[j] = f32(v.data + at + 192 + 4 * j);
    const auto parent = u32(v.data + at + 220);
    n.parent = parent == 0xffffffffu ? -1 : static_cast<std::int32_t>(parent);
    output = n; return true;
}
bool readMot49O(const std::uint8_t* data, std::size_t bytes,
    MotionView49O& output, GeometryDiagnostic49O& d) noexcept {
    d = {};
    if (!data) return reject(d, GeometryError49O::invalid_input, 0);
    if (bytes > maxMotBytes49O) return reject(d, GeometryError49O::file_limit, 0);
    if (bytes < 28) return reject(d, GeometryError49O::truncated, bytes);
    if (u32(data) != 0xfffffffeu) return reject(d, GeometryError49O::version, 0);
    MotionView49O v{}; v.data = data; v.bytes = bytes;
    v.frameCount = u32(data + 4); v.duration = f32(data + 8);
    if (!v.frameCount || v.frameCount > maxMotKeysPerTrack49O) return reject(d, GeometryError49O::frame_count, 4);
    if (!std::isfinite(v.duration) || !(v.duration > 0)) return reject(d, GeometryError49O::duration, 8);
    v.rotationType = u32(data + 12);
    if (v.rotationType > 1 || u32(data + 16) != 1) return reject(d, GeometryError49O::track_type, 12);
    v.positionCount = u32(data + 20); v.rotationCount = u32(data + 24);
    if (v.positionCount > maxMotTracks49O || v.rotationCount > maxMotTracks49O)
        return reject(d, GeometryError49O::track_count, 20);
    std::size_t at = 28;
    for (unsigned channel = 0; channel < 2; ++channel) {
        const auto count = channel ? v.rotationCount : v.positionCount;
        const std::size_t stride = channel ? (v.rotationType == 1 ? 12 : 8) : 40;
        for (std::uint32_t i = 0; i < count; ++i) {
            if (!range(bytes, at, 8)) return reject(d, GeometryError49O::truncated, bytes);
            MotTrack49O& t = channel ? v.rotation[i] : v.position[i];
            t.keyCount = u32(data + at); t.rowIndex = u32(data + at + 4);
            if (!t.keyCount || t.keyCount > maxMotKeysPerTrack49O) return reject(d, GeometryError49O::key_count, at);
            if (t.keyCount > maxMotTotalKeys49O - v.totalKeys) return reject(d, GeometryError49O::total_keys, at);
            if (t.rowIndex >= maxModelNodes49O) return reject(d, GeometryError49O::row_index, at + 4);
            v.totalKeys += t.keyCount; at += 8; t.keyDataOffset = static_cast<std::uint32_t>(at);
            if (!range(bytes, at, std::size_t(t.keyCount) * stride)) return reject(d, GeometryError49O::truncated, bytes);
            float previous = 0;
            for (std::uint32_t k = 0; k < t.keyCount; ++k) {
                const auto p = at + k * stride; const float time = f32(data + p);
                if (!std::isfinite(time)) return reject(d, GeometryError49O::key_nonfinite, p);
                if (time < 0 || time > v.duration || (k && !(time > previous)))
                    return reject(d, GeometryError49O::key_time, p);
                previous = time;
                if (channel && v.rotationType == 1 &&
                    (std::uint32_t(data[p + 4]) | (std::uint32_t(data[p + 5]) << 8)) >= 4096)
                    return reject(d, GeometryError49O::rotation_angle, p + 4);
                if (!channel) for (std::size_t j = 1; j < 10; ++j)
                    if (!std::isfinite(f32(data + p + 4 * j))) return reject(d, GeometryError49O::key_nonfinite, p + 4 * j);
            }
            at += std::size_t(t.keyCount) * stride;
        }
    }
    if (at != bytes) return reject(d, GeometryError49O::trailing_bytes, at);
    output = v; return true;
}
bool readPositionKey49O(const MotionView49O& v, std::uint32_t ti,
    std::uint32_t ki, BezierKey49O& output) noexcept {
    if (!v.data || v.positionCount > maxMotTracks49O || ti >= v.positionCount) return false;
    const auto& t = v.position[ti];
    if (!t.keyCount || t.keyCount > maxMotKeysPerTrack49O || ki >= t.keyCount) return false;
    const std::size_t at = std::size_t(t.keyDataOffset) + std::size_t(ki) * 40;
    if (!range(v.bytes, at, 40)) return false;
    BezierKey49O k; k.time = f32(v.data + at);
    for (std::size_t j = 0; j < 3; ++j) {
        k.position[j] = f32(v.data + at + 4 + j * 4);
        k.incoming[j] = f32(v.data + at + 16 + j * 4);
        k.outgoing[j] = f32(v.data + at + 28 + j * 4);
    }
    output = k; return true;
}
bool readRotationKey49O(const MotionView49O& v, std::uint32_t ti,
    std::uint32_t ki, ByteAxisKey49O& output) noexcept {
    if (!v.data || v.rotationType != 0 || v.rotationCount > maxMotTracks49O || ti >= v.rotationCount) return false;
    const auto& t = v.rotation[ti];
    if (!t.keyCount || t.keyCount > maxMotKeysPerTrack49O || ki >= t.keyCount) return false;
    const std::size_t at = std::size_t(t.keyDataOffset) + std::size_t(ki) * 8;
    if (!range(v.bytes, at, 8)) return false;
    ByteAxisKey49O k; k.time = f32(v.data + at); k.angle = v.data[at + 4];
    for (std::size_t j = 0; j < 3; ++j) {
        const auto b = v.data[at + 5 + j];
        k.axis[j] = static_cast<std::int8_t>(b < 128 ? int(b) : int(b) - 256);
    }
    output = k; return true;
}
bool readRotationKey49O(const MotionView49O& v, std::uint32_t ti,
    std::uint32_t ki, ShortAxisKey49O& output) noexcept {
    if (!v.data || v.rotationType != 1 || v.rotationCount > maxMotTracks49O || ti >= v.rotationCount) return false;
    const auto& t = v.rotation[ti];
    if (!t.keyCount || t.keyCount > maxMotKeysPerTrack49O || ki >= t.keyCount) return false;
    const std::size_t at = std::size_t(t.keyDataOffset) + std::size_t(ki) * 12;
    if (!range(v.bytes, at, 12)) return false;
    ShortAxisKey49O k; k.time = f32(v.data + at);
    k.angle = static_cast<std::uint16_t>(std::uint32_t(v.data[at + 4]) | (std::uint32_t(v.data[at + 5]) << 8));
    if (k.angle >= 4096) return false;
    for (std::size_t j = 0; j < 3; ++j) {
        const auto b = std::uint32_t(v.data[at + 6 + 2 * j]) | (std::uint32_t(v.data[at + 7 + 2 * j]) << 8);
        k.axis[j] = static_cast<std::int16_t>(b < 32768 ? int(b) : int(b) - 65536);
    }
    output = k; return true;
}

}
