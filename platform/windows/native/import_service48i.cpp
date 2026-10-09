#include "import_service48i.h"
#include <windows.h>
#include <utility>

namespace rr::windows {
bool assembleCachedWorld48I(const std::filesystem::path& cache, const std::string& container,
    const std::string& member, RuntimeWorld48I& output, std::string& error) {
    struct Lock { HANDLE handle; ~Lock() { if (handle != INVALID_HANDLE_VALUE) CloseHandle(handle); } };
    Lock lock{CreateFileW((cache / L"import.lock").c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
    if (lock.handle == INVALID_HANDLE_VALUE) { error = "Cannot read world while cache import is active or missing"; return false; }
    RuntimeWorld48I world;
    if (!assembleCachedWorld48F(cache, container, member, world.visuals, error) ||
        !loadCachedCollisionWorld48I(cache, container, member, world.collisions, error)) return false;
    output = std::move(world); error.clear(); return true;
}
}
