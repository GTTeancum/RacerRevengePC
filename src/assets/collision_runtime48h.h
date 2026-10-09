#pragma once
#include <array>
#include <cstdint>
namespace rr::assets {
// Original COL1 loader 126320..126378 stores low signed16 bits after
// float32(float32(32767*n)+0.5) and truncating/saturating EE CVT.W.S.
// Vertex normals remain float3. This is only the face-normal representation
// required by the original collision query; RRCOL001 preserves source floats.
// Nonfinite input/intermediate failure leaves output unchanged.
bool packCollisionFaceNormal48H(const std::array<float,3>&,
                               std::array<std::int16_t,3>&) noexcept;
}
