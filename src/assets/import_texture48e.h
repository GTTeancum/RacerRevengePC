#pragma once
#include "import_decode48c.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rr { namespace assets {
struct TextureLimits48E {
    std::size_t input_bytes = 8u * 1024u * 1024u;
    // Includes retained RGBA mip bytes and base RRTEX bytes, not source input.
    std::size_t decoded_bytes = 16u * 1024u * 1024u;
};
struct TextureMip48E {
    std::uint32_t width = 0, height = 0;
    // The index payload is packed two texels per byte for kind0, one for kind1.
    // A typed authenticated IPUM cache uses source_indices_offset for the
    // whole kind2 IPUM payload (including its16-byte header), not index bytes.
    std::size_t source_header_offset = 0, source_indices_offset = 0;
    // Original row order, unpremultiplied RGBA. Indexed kinds retain raw
    // palette alpha. The separate kind2 cache retains source-backed RGB5
    // high-bit expansion and TEXA255/AEM1 sampled alpha; see its provenance.
    std::vector<std::uint8_t> rgba;
};
struct Texture48E {
    std::uint16_t revision = 1, flags = 0;
    std::uint32_t kind = 1; // Original PST:0=4-bit indexed,1=8-bit indexed,2=IPUM.
    // kind0 uses only the first16 entries; the unused remainder is zero-initialized
    // storage, not additional authored palette data. kind1 uses all256 entries.
    // kind2 has no authored palette; its zero storage does not invent one.
    std::array<std::array<std::uint8_t, 4>, 256> original_palette{};
    std::vector<TextureMip48E> mips; // Original ascending dimensions; base is back().
    std::vector<std::uint8_t> base_rrtex; // RRTEX001; no shader/alpha normalization.
};
struct MaterialField48E {
    std::string name, value;
    std::size_t source_offset = 0;
};
struct MaterialFields48E {
    std::vector<MaterialField48E> authored; // Only nonempty values; no interpretation.
    std::vector<MaterialField48E> unspecified; // Empty authored values remain empty.
};
struct MaterialTexture48E {
    std::uint32_t slot = 0; // Original one-based TextureMap_N slot.
    std::string name; // Original basename; unresolved against RES by this parser.
};
struct MaterialPass48E {
    std::uint32_t pass = 0; // Original one-based ShaderPass_N section.
    std::string microprogram;
    bool texture_count_defaulted = false; // Proven original missing-count default 1.
    std::vector<MaterialTexture48E> textures;
    MaterialFields48E fields;
};
struct MaterialInactiveSection48E {
    std::string name; // Original numbered section above TotalShaderPasses.
    std::size_t source_offset = 0, source_length = 0;
    // Lexical evidence only: no program, texture, defaults or render state.
    MaterialFields48E fields;
};
struct Material48E {
    MaterialFields48E general;
    std::vector<MaterialPass48E> passes; // Only declared ShaderPass_1..count.
    std::vector<MaterialInactiveSection48E> inactive_sections; // Source order.
    // Populated for inactive sections or admitted empty duplicate keys. Original text
    // is retained byte-for-byte, including comments, whitespace and newlines;
    // each inactive section's range refers into this bounded evidence buffer.
    std::vector<std::uint8_t> original_text;
};

// Bounded48D kind1 subset plus source-backed50KG kind0 packed-index expansion.
// Revision1/flags0,1..7 ascending mips, power-of-two16..1024 remain required.
// This function still rejects kind2; only a separate authenticated cold cache
// may publish the supported exact source-derived IPUM Texture48E result.
// No files, JSON, guest execution, GS upload swizzle,
// guessed shader/default render state, texture lookup, or platform dependency.
// Transactional outputs on all failure/cancellation paths. Limits only reduce
// fixed hard ceilings. Optional synchronous callbacks follow DecodeHooks48C.
bool decodeTexture48E(const std::vector<std::uint8_t>& input, Texture48E& output,
    DecodeError48C& error, const DecodeHooks48C* hooks = nullptr,
    const TextureLimits48E& limits = TextureLimits48E{});
bool encodeTextureLevel48E(const TextureMip48E& mip, std::vector<std::uint8_t>& output,
    DecodeError48C& error);
// Original29B718/29B7F0 counts declared passes, not physical section headers.
// Extra ShaderPass_N sections within1..8 remain inactive lexical evidence.
// Prologue/Epilogue and unrecognized sections are still outside this subset.
bool decodeMaterial48E(const std::vector<std::uint8_t>& input, Material48E& output,
    DecodeError48C& error, const DecodeHooks48C* hooks = nullptr,
    std::size_t maxBytes = 64u * 1024u);
// Separate exact-source admission for original TC AHUTS03 and TC_BLEND8.
// Retains authored TextureMap strings and all original bytes. No lookup occurs.
// Every other byte sequence, including mutations of those sources, is rejected.
bool decodeOriginalTcMaterial50TC(const std::vector<std::uint8_t>& input, Material48E& output,
    DecodeError48C& error, const DecodeHooks48C* hooks = nullptr,
    std::size_t maxBytes = 64u * 1024u);
} }
