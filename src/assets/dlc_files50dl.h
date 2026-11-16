#pragma once
// DLC virtual file overlay (phase 1). NOT ORIGINAL BEHAVIOUR: PC enhancement.
// The only place that turns a game file path into the host file to open.
// Narrow on purpose: every file-open site includes it, so it must stay small.
//
// With no admitted replacement (no DLC folder, an empty one, RR_DLC=0, the
// replacement switch off, or every package refused) host() returns its input
// unchanged and the game opens exactly what it opened before.
#include <filesystem>

namespace rr::assets::dlc50DL {

// Reads and checks every package under the DLC root once, logs the result and
// freezes the overlay. Call on the main thread after RR_ORIGINAL_ASSET_ROOT is
// set and before the first original file is opened. Later calls do nothing.
// Never throws: a bad package is refused and the game continues with stock.
void startup() noexcept;

// The host file to open for `path`. A path naming a stock file that an admitted
// package replaces (flagged "replace": true, built for exactly the installed
// stock bytes) returns that package's file, logging one dlc_open line per call.
// Every other path is returned unchanged. Throws std::runtime_error when the
// replacing file changed on disk after startup (never falls back to stock).
std::filesystem::path host(const std::filesystem::path& path);

// True when at least one replacement is active this session.
bool replacing() noexcept;

} // namespace rr::assets::dlc50DL
