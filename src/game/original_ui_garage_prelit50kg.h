#pragma once
#include <array>
#include <cstdint>

// NEW isolated numeric reconstruction. This is NOT a Garage owner, material
// admission, palette, source-frame capability, or renderer submission path.
// Operands must come from the live model/camera/visibility owners. No native
// GL clipping substitutes for the original packet clipper represented here.
namespace rr::game::garage_prelit50KG {
using Matrix = std::array<float,16>;
enum class Status { complete, input, arithmetic, unsupportedPacketRange, capacity };
struct Context {
    Matrix modelWorld100{}, viewC0{}, projection80{}, sourceScreen140{};
    std::array<float,3> cameraPosition180{};
    float positionScale1E0=0, uvScale1E8=0, fogEnd33Z=0, fogSlope33W=0;
    // Actual row41.x / model1B2. Only bits01/02/04 are consumed here.
    // 01 permits source polygon clipping, 02 computes radial F, 04 selects
    // the original clip-aware program. This helper does not derive these bits.
    std::uint8_t modelFlags1B2=0;
};
struct Preamble {
    Matrix ordinaryClip{}, combinedScreen{}, sourceScreen140{};
    std::array<float,3> localCameraRaw{};
    float uvScale=0, endTimesUnscaledSlope=0, scaledSlope=0;
    std::uint8_t modelFlags1B2=0;
};
struct RawVertex {
    std::array<std::int16_t,3> position{};
    std::array<std::int16_t,2> uv{};
    std::array<std::uint8_t,4> rgba{};
};
// Unpacked original clipper operand. Numerical only, not a source capability.
// UVQ is before perspective divide; colors are packed byte values as floats.
struct PreparedVertex {
    std::array<float,4> clip{},screen{},color{};
    std::array<float,3> uvq{};
    float fog=0;
};
struct PacketVertex {
    // Original GS X/Y after FTOI4, expressed in pixels before XYOFFSET;
    // original 24-bit Z after removing the packet's four fractional bits.
    std::array<float,3> gsPosition{};
    // Actual packet STQ. Do not silently reinterpret these as semantic UV.
    std::array<float,3> stq{};
    std::array<std::uint8_t,4> rgba{};
    float rasterW=0;
    std::uint8_t fogByte=0;
};
struct Polygon {
    std::array<PacketVertex,9> vertices{};
    std::uint32_t count=0;
    // A true clipper call emits the final polygon as a TRIANGLE FAN (PRIM5).
    // An untouched triangle retains the caller's original strip winding.
    bool sourceTriangleFan=false;
};
// ModelWorld100 is the real one-node packet matrix from241684..2416A0.
// It is NOT a synthetic palette node or the imported PSG header transform.
Status prepare(const Context&,Preamble&) noexcept;
// Inputs MUST be chronological oldest/middle/current original strip records,
// not the importer's winding-normalized odd-triangle index order. All output
// commits atomically. count0 is an authenticated source reject, not an error.
// Shared original366430 packet clipper. Other authenticated programs may
// supply their genuine generated colors/UV and original ordinary clip values.
Status projectPreparedTriangle(const Preamble&,const std::array<PreparedVertex,3>&,Polygon&) noexcept;
Status prepareVertex(const Preamble&,const RawVertex&,PreparedVertex&) noexcept;
Status projectTriangle(const Preamble&,const std::array<RawVertex,3>&,Polygon&) noexcept;
}
