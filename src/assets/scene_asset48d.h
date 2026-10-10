#pragma once
#include "import_scene48d.h"

namespace rr { namespace assets {
// RRSCENE1 stores ordinary scene data, independently of source packet encoding.
// Fixed-width little-endian fields; no serialized host pointers or structures.
// Recovery-only strip/source-offset diagnostics are deliberately not persisted.
constexpr std::size_t max_scene_bytes48d = 128u * 1024u * 1024u;
bool encodeSceneAsset48D(const Scene48D&, std::vector<std::uint8_t>&, std::string& error);
bool parseSceneAsset48D(const std::vector<std::uint8_t>&, Scene48D&, std::string& error);
} }
