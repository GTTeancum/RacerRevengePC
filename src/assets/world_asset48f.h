#pragma once
#include "import_world48f.h"

namespace rr { namespace assets {
constexpr std::size_t max_world_asset_bytes48f = 32u * 1024u * 1024u;
// Neutral RRWORLD1 stores typed world data and reversible raw provenance.
// These operations never invoke the original BINARY/SCB parameter parser.
// All integers and IEEE binary64 values are explicit little endian; no pointers.
// On failure output remains unchanged and error contains a named reason.
bool encodeWorldAsset48F(const World48F& world, std::vector<std::uint8_t>& output, std::string& error);
bool parseWorldAsset48F(const std::vector<std::uint8_t>& input, World48F& output, std::string& error);
} }
