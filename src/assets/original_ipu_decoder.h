#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rr::assets {

// Cold-load one authenticated original IPUM image into host RGBA pixels.
// ipum contains the full 16-byte header, one frame and both B0/B1 terminators.
// The caller must authenticate the original resource and the returned pixels
// against its existing pins before adoption. This format decoder does not mint
// authenticity/lease objects and is not a replacement for source verification.
//
// Bounded subset: 16..1024 power-of-two dimensions, one frame, zero frame flags,
// at most 8 MiB encoded bytes. Pixels reproduce decode_source_textures50fc.py:
// scalar simple IDCT, integer IPU CSC, enabled RGB16 dither, high-five-bit RGB
// expansion, TEXA255/AEM1, no row flip. No subprocesses or generated data files.
//
// Calls made through this adapter are serialized. Use during the cold-load
// stage, without unrelated concurrent FFmpeg use: scalar decoding temporarily
// overrides FFmpeg's process-wide CPU flags, then restores their prior value.
// Output is empty on failure, with a nonempty diagnostic. On success output has
// width*height*4 bytes and diagnostic is empty. The RRTEX header is not included.
bool decodeOriginalIpuRgba(const std::uint8_t* ipum, std::size_t ipumBytes,
                           std::uint32_t width, std::uint32_t height,
                           std::vector<std::uint8_t>& rgba,
                           std::string& diagnostic);

} // namespace rr::assets
