#pragma once
// Renderer capacity for a resizable/maximizable window: large enough for any
// attached monitor (the window may be maximized on any of them), capped at a
// bounded 4K budget, never below the caller's floor. Same pixel space as the
// SDL drawable (the process's DPI view of the desktop).
#include <cstdint>

#if defined(_WIN32)
extern "C" __declspec(dllimport) int __stdcall GetSystemMetrics(int index);
#endif

namespace native_display_capacity50DU {
inline void capacity(std::int32_t floorWidth, std::int32_t floorHeight,
    std::int32_t& width, std::int32_t& height) noexcept {
    constexpr std::int32_t capWidth = 3840, capHeight = 2160;
    width = floorWidth; height = floorHeight;
#if defined(_WIN32)
    constexpr int virtualScreenWidth = 78, virtualScreenHeight = 79; // SM_CXVIRTUALSCREEN/SM_CYVIRTUALSCREEN
    constexpr int primaryWidth = 0, primaryHeight = 1;               // SM_CXSCREEN/SM_CYSCREEN
    std::int32_t w = GetSystemMetrics(virtualScreenWidth), h = GetSystemMetrics(virtualScreenHeight);
    const std::int32_t pw = GetSystemMetrics(primaryWidth), ph = GetSystemMetrics(primaryHeight);
    if (pw > w) w = pw;
    if (ph > h) h = ph;
    if (w > width) width = w;
    if (h > height) height = h;
#endif
    if (width > capWidth) width = capWidth;
    if (height > capHeight) height = capHeight;
    if (width < floorWidth) width = floorWidth;
    if (height < floorHeight) height = floorHeight;
}
}
