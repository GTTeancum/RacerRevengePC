#pragma once
#include <cstddef>
#include <cstdio>
#include <cstring>

#if defined(_WIN32)
#if !defined(_WIN64) || (!defined(_M_X64) && !defined(__x86_64__)) || defined(_M_ARM64EC)
#error "The native Windows SDL2 host currently supports x64 (AMD64) only"
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#define RR_SDL50WIN_CALL __cdecl
#elif defined(__linux__)
#include <dlfcn.h>
#define RR_SDL50WIN_CALL
#else
#error "The native SDL2 loader supports Linux and Windows x64 only"
#endif

namespace rr::platform::native_library50win {
using Handle = void*;
using Symbol = void (RR_SDL50WIN_CALL *)();
inline void copyError(char (&detail)[256], const char* source) noexcept {
    detail[0] = 0;
    if (!source) return;
    std::size_t n = 0;
    while (n + 1 < sizeof(detail) && source[n]) {
        detail[n] = source[n];
        ++n;
    }
    detail[n] = 0;
}
#if defined(_WIN32)
inline constexpr const char* openOperation = "LoadLibraryExW(executable-directory SDL2.dll)";
inline constexpr const char* closeOperation = "FreeLibrary(SDL2)";
inline void windowsError(char (&detail)[256], const char* operation, DWORD code) noexcept {
    // Preserve the original Win32 error numerically even if message formatting
    // would itself fail or the OS message text would exceed the bounded buffer.
    std::snprintf(detail, sizeof(detail), "%s failed (Win32 error %lu)",
        operation, static_cast<unsigned long>(code));
    detail[sizeof(detail) - 1] = 0;
}
inline Handle openSdl2(char (&detail)[256]) noexcept {
    detail[0] = 0;
    wchar_t path[32768]{};
    constexpr DWORD capacity = static_cast<DWORD>(sizeof(path) / sizeof(path[0]));
    const DWORD length = GetModuleFileNameW(nullptr, path, capacity);
    if (!length) {
        const DWORD error = GetLastError();
        windowsError(detail, "GetModuleFileNameW", error);
        return nullptr;
    }
    if (length >= capacity) {
        windowsError(detail, "GetModuleFileNameW/path too long", ERROR_INSUFFICIENT_BUFFER);
        return nullptr;
    }
    std::size_t directoryLength = length;
    while (directoryLength && path[directoryLength - 1] != L'\\' && path[directoryLength - 1] != L'/')
        --directoryLength;
    constexpr wchar_t filename[] = L"SDL2.dll";
    if (!directoryLength || directoryLength + sizeof(filename) / sizeof(filename[0]) > capacity) {
        windowsError(detail, "executable-directory SDL2.dll/path", ERROR_BAD_PATHNAME);
        return nullptr;
    }
    std::memcpy(path + directoryLength, filename, sizeof(filename));
    // Never search cwd/PATH or change process DLL-search settings. The package
    // must supply an AMD64 SDL2.dll next to the executable. Dependencies are
    // restricted to that directory and System32; unsupported OS flags fail.
    const HMODULE module = LoadLibraryExW(path, nullptr,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        const DWORD error = GetLastError();
        windowsError(detail, "LoadLibraryExW(SDL2.dll)", error);
    }
    return reinterpret_cast<Handle>(module);
}
inline Symbol symbol(Handle library, const char* name, char (&detail)[256]) noexcept {
    detail[0] = 0;
    const FARPROC result = GetProcAddress(reinterpret_cast<HMODULE>(library), name);
    if (!result) {
        const DWORD error = GetLastError();
        windowsError(detail, "GetProcAddress", error);
    }
    return reinterpret_cast<Symbol>(result);
}
inline bool close(Handle library, char (&detail)[256]) noexcept {
    detail[0] = 0;
    if (FreeLibrary(reinterpret_cast<HMODULE>(library))) return true;
    const DWORD error = GetLastError();
    windowsError(detail, "FreeLibrary", error);
    return false;
}
#else
inline constexpr const char* openOperation = "dlopen(SDL2)";
inline constexpr const char* closeOperation = "dlclose(SDL2)";
inline Handle openSdl2(char (&detail)[256]) noexcept {
    detail[0] = 0;
    Handle library = dlopen("libSDL2-2.0.so.0", RTLD_NOW | RTLD_LOCAL);
    if (!library) library = dlopen("libSDL2.so", RTLD_NOW | RTLD_LOCAL);
    if (!library) copyError(detail, dlerror());
    return library;
}
inline Symbol symbol(Handle library, const char* name, char (&detail)[256]) noexcept {
    detail[0] = 0;
    const auto result = dlsym(library, name);
    if (!result) copyError(detail, dlerror());
    return reinterpret_cast<Symbol>(result);
}
inline bool close(Handle library, char (&detail)[256]) noexcept {
    detail[0] = 0;
    if (dlclose(library) == 0) return true;
    copyError(detail, dlerror());
    return false;
}
#endif
}
