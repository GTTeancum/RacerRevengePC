#pragma once
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>

namespace rr { namespace windows {
struct ImportProgress48C {
    std::string stage, item;
    std::uint32_t completed = 0, total = 0;
    std::uint32_t converted = 0, reused = 0, unsupported = 0;
};
struct ImportOptions48C {
    std::filesystem::path source, cache;
    // Optional diagnostic subset, e.g. TRACKS/TA.RES. Empty imports every RES.
    std::string container;
};
struct ImportResult48C {
    bool success = false, cancelled = false;
    std::string error, generation, manifest_path;
    std::uint32_t containers = 0, converted = 0, reused = 0, unsupported = 0;
};
// Worker-thread entry. Progress callback executes synchronously on that thread.
// Cancellation never replaces current.json. Completed verified objects survive
// cancellation and are reused after their hashes and structure are rechecked.
ImportResult48C importGame48C(const ImportOptions48C&, std::atomic<bool>& cancel,
    const std::function<void(const ImportProgress48C&)>& progress = {});
std::string importResultJson48C(const ImportResult48C&);
} }
