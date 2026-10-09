#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "import_ui48c.h"
#include "import_service48c.h"
#include <windows.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shellapi.h>
#include <wrl/client.h>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

namespace rr { namespace windows {
namespace {
using Microsoft::WRL::ComPtr;
namespace fs = std::filesystem;
constexpr UINT progressMessage = WM_APP + 48, finishedMessage = WM_APP + 49;
enum : int { sourceEdit = 100, cacheEdit, folderButton, isoButton, cacheButton, startButton, cancelButton, statusText, scopeText };
std::wstring wide(const std::string& text) {
    if (text.empty()) return {};
    const int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), nullptr, 0);
    if (!size) return L"Unable to display text";
    std::wstring out(static_cast<std::size_t>(size), 0);
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), out.data(), size);
    return out;
}
std::string utf8(const std::wstring& text) {
    if (text.empty()) return {};
    const int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    if (!size) throw std::runtime_error("Invalid command line text");
    std::string out(static_cast<std::size_t>(size), 0);
    WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), out.data(), size, nullptr, nullptr);
    return out;
}
std::wstring textOf(HWND control) {
    const int count = GetWindowTextLengthW(control);
    std::wstring text(static_cast<std::size_t>(count) + 1, 0);
    const int copied = GetWindowTextW(control, text.data(), count + 1);
    text.resize(static_cast<std::size_t>(copied)); return text;
}
struct UI {
    HWND window = nullptr;
    HFONT font = nullptr;
    std::thread worker;
    std::atomic<bool> cancel{false};
    std::atomic<bool> progressQueued{false};
    std::mutex mutex;
    ImportProgress48C progress;
    ImportResult48C result;
    bool running = false, closing = false;
    bool automatedCheck = false, capturePassed = false;
    fs::path capture;
    ~UI() { cancel.store(true); if (worker.joinable()) worker.join(); if (font) DeleteObject(font); }
    HWND child(int id) const { return GetDlgItem(window, id); }
};
bool captureWindow(UI& ui) {
    RECT rect{}; if (!GetWindowRect(ui.window, &rect)) return false;
    const int width = rect.right - rect.left, height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0 || width > 4096 || height > 4096) return false;
    HDC screen = GetDC(ui.window), memory = CreateCompatibleDC(screen);
    BITMAPINFO info{}; info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width; info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1; info.bmiHeader.biBitCount = 32; info.bmiHeader.biCompression = BI_RGB;
    void* pixels = nullptr;
    HBITMAP bitmap = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
    bool success = false;
    if (memory && bitmap && pixels) {
        const auto old = SelectObject(memory, bitmap);
        if (PrintWindow(ui.window, memory, 0)) {
            BITMAPFILEHEADER header{}; header.bfType = 0x4d42;
            header.bfOffBits = sizeof(header) + sizeof(BITMAPINFOHEADER);
            header.bfSize = header.bfOffBits + static_cast<DWORD>(width * height * 4);
            std::ofstream out(ui.capture, std::ios::binary);
            out.write(reinterpret_cast<const char*>(&header), sizeof(header));
            out.write(reinterpret_cast<const char*>(&info.bmiHeader), sizeof(BITMAPINFOHEADER));
            out.write(static_cast<const char*>(pixels), static_cast<std::streamsize>(width) * height * 4);
            out.close(); success = bool(out);
        }
        SelectObject(memory, old);
    }
    if (bitmap) DeleteObject(bitmap);
    if (memory) DeleteDC(memory);
    if (screen) ReleaseDC(ui.window, screen);
    return success;
}
void pick(UI& ui, bool directory, bool cache) {
    ComPtr<IFileOpenDialog> dialog;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog)))) return;
    DWORD flags = 0; dialog->GetOptions(&flags);
    dialog->SetOptions(flags | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | (directory ? FOS_PICKFOLDERS : FOS_FILEMUSTEXIST));
    if (!directory) {
        const COMDLG_FILTERSPEC filter[] = {{L"Original game ISO", L"*.iso"}};
        dialog->SetFileTypes(1, filter);
    }
    dialog->SetTitle(cache ? L"Choose the native asset cache folder" : directory ? L"Choose the extracted original game folder" : L"Choose the original game ISO");
    if (SUCCEEDED(dialog->Show(ui.window))) {
        ComPtr<IShellItem> item;
        if (SUCCEEDED(dialog->GetResult(&item))) {
            PWSTR path = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
                SetWindowTextW(ui.child(cache ? cacheEdit : sourceEdit), path); CoTaskMemFree(path);
            }
        }
    }
}
void enable(UI& ui, bool idle) {
    for (int id : {sourceEdit, cacheEdit, folderButton, isoButton, cacheButton, startButton}) EnableWindow(ui.child(id), idle);
    EnableWindow(ui.child(cancelButton), !idle);
}
void start(UI& ui) {
    if (ui.running) return;
    ImportOptions48C options;
    options.source = textOf(ui.child(sourceEdit)); options.cache = textOf(ui.child(cacheEdit));
    if (options.source.empty() || options.cache.empty()) {
        SetWindowTextW(ui.child(statusText), L"Choose original game data and a separate cache folder first."); return;
    }
    if (ui.worker.joinable()) ui.worker.join();
    ui.cancel.store(false); ui.running = true; enable(ui, false);
    SetWindowTextW(ui.child(statusText), L"Validating original game data...");
    ui.worker = std::thread([&ui, options] {
        auto result = importGame48C(options, ui.cancel, [&](const ImportProgress48C& progress) {
            { std::lock_guard<std::mutex> lock(ui.mutex); ui.progress = progress; }
            if (!ui.progressQueued.exchange(true)) PostMessageW(ui.window, progressMessage, 0, 0);
        });
        { std::lock_guard<std::mutex> lock(ui.mutex); ui.result = std::move(result); }
        PostMessageW(ui.window, finishedMessage, 0, 0);
    });
}
LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    auto* ui = reinterpret_cast<UI*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        ui = static_cast<UI*>(reinterpret_cast<CREATESTRUCTW*>(lparam)->lpCreateParams);
        ui->window = window; SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ui));
    }
    if (!ui) return DefWindowProcW(window, message, wparam, lparam);
    switch (message) {
    case WM_COMMAND:
        switch (LOWORD(wparam)) {
        case folderButton: pick(*ui, true, false); return 0;
        case isoButton: pick(*ui, false, false); return 0;
        case cacheButton: pick(*ui, true, true); return 0;
        case startButton:
            try { start(*ui); }
            catch (const std::exception& error) { ui->running = false; enable(*ui, true); SetWindowTextW(ui->child(statusText), wide(error.what()).c_str()); }
            return 0;
        case cancelButton: ui->cancel.store(true); SetWindowTextW(ui->child(statusText), L"Cancelling safely..."); return 0;
        }
        break;
    case progressMessage: {
        ui->progressQueued.store(false);
        ImportProgress48C progress;
        { std::lock_guard<std::mutex> lock(ui->mutex); progress = ui->progress; }
        std::wstring text = wide(progress.stage) + L"\r\n" + wide(progress.item) + L"\r\n\r\n";
        text += L"Converted " + std::to_wstring(progress.converted) + L"   Reused " + std::to_wstring(progress.reused)
            + L"   Unsupported " + std::to_wstring(progress.unsupported);
        if (progress.total) text += L"\r\nCurrent stage: " + std::to_wstring(progress.completed) + L" / " + std::to_wstring(progress.total);
        SetWindowTextW(ui->child(statusText), text.c_str()); return 0;
    }
    case finishedMessage: {
        if (ui->worker.joinable()) ui->worker.join();
        ui->running = false; enable(*ui, true);
        ImportResult48C result;
        { std::lock_guard<std::mutex> lock(ui->mutex); result = ui->result; }
        const std::wstring text = result.success
            ? L"Content import complete.\r\nConverted " + std::to_wstring(result.converted) + L", reused " + std::to_wstring(result.reused)
                + L", unsupported " + std::to_wstring(result.unsupported) + L".\r\n\r\nThis cache is partial; the game is not playable yet.\r\n"
                + wide(result.manifest_path)
            : wide(result.error);
        SetWindowTextW(ui->child(statusText), text.c_str());
        if (ui->automatedCheck) {
            UpdateWindow(window);
            ui->capturePassed = result.success && captureWindow(*ui);
            DestroyWindow(window);
            return 0;
        }
        if (ui->closing) DestroyWindow(window);
        return 0;
    }
    case WM_CLOSE:
        if (ui->running) { ui->closing = true; ui->cancel.store(true); SetWindowTextW(ui->child(statusText), L"Cancelling safely before closing..."); }
        else DestroyWindow(window);
        return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(window, message, wparam, lparam);
}
int showUI(const ImportOptions48C* automated = nullptr) {
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(com)) throw std::runtime_error("Cannot initialize the Windows file picker");
    struct ComCleanup { ~ComCleanup() { CoUninitialize(); } } cleanup;
    UI ui;
    const HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSW wc{}; wc.lpfnWndProc = windowProc; wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)); wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = L"RacerRevengeNativeImport48C";
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) throw std::runtime_error("Cannot register importer window");
    const HWND window = CreateWindowExW(0, wc.lpszClassName, L"Racer Revenge PC - Import original game data",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 800, 510,
        nullptr, nullptr, instance, &ui);
    if (!window) throw std::runtime_error("Cannot create importer window");
    ui.font = CreateFontW(-17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    auto child = [&](const wchar_t* type, const wchar_t* text, DWORD style, int x, int y, int w, int h, int id) {
        HWND control = CreateWindowExW(type == std::wstring(L"EDIT") ? WS_EX_CLIENTEDGE : 0, type, text,
            WS_CHILD | WS_VISIBLE | style, x, y, w, h, window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), instance, nullptr);
        SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(ui.font), TRUE); return control;
    };
    child(L"STATIC", L"Import your original game data", 0, 24, 18, 740, 28, -1);
    child(L"STATIC", L"Converts supported tracks, vehicles, textures, materials and collision data. Gameplay is still being recovered.", 0, 24, 52, 740, 42, scopeText);
    child(L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 24, 108, 740, 27, sourceEdit);
    child(L"BUTTON", L"Choose ISO...", WS_TABSTOP, 24, 146, 160, 32, isoButton);
    child(L"BUTTON", L"Choose extracted folder...", WS_TABSTOP, 196, 146, 225, 32, folderButton);
    child(L"STATIC", L"Native asset cache", 0, 24, 191, 740, 24, -1);
    child(L"EDIT", L"", ES_AUTOHSCROLL | WS_TABSTOP, 24, 221, 595, 27, cacheEdit);
    child(L"BUTTON", L"Choose folder...", WS_TABSTOP, 632, 218, 132, 32, cacheButton);
    child(L"BUTTON", L"Convert / Resume", BS_DEFPUSHBUTTON | WS_TABSTOP, 24, 265, 195, 34, startButton);
    child(L"BUTTON", L"Cancel", WS_TABSTOP, 232, 265, 100, 34, cancelButton);
    child(L"EDIT", L"Choose an ISO or extracted data. Original files remain unchanged.\r\nPreviously verified conversions are reused automatically.",
        ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL, 24, 317, 740, 135, statusText);
    PWSTR local = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &local))) {
        const auto cache = fs::path(local) / L"RacerRevengePC" / L"NativeCache";
        SetWindowTextW(ui.child(cacheEdit), cache.c_str()); CoTaskMemFree(local);
    }
    enable(ui, true); ShowWindow(window, automated ? SW_SHOWNOACTIVATE : SW_SHOW); UpdateWindow(window);
    if (automated) {
        ui.automatedCheck = true; ui.capture = automated->cache / "import-ui-check.bmp";
        SetWindowTextW(ui.child(sourceEdit), automated->source.c_str());
        SetWindowTextW(ui.child(cacheEdit), automated->cache.c_str());
        SendMessageW(window, WM_COMMAND, startButton, 0);
    }
    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageW(window, &message)) { TranslateMessage(&message); DispatchMessageW(&message); }
    }
    if (automated) { std::cout << importResultJson48C(ui.result) << '\n'; return ui.capturePassed ? 0 : 1; }
    return 0;
}
}
bool importEntry48C(int argc, char** argv, int& exitCode) {
    if (argc != 1 && std::string(argv[1]) != "--import" && std::string(argv[1]) != "--import-ui-check") return false;
    try {
        if (argc == 1) { exitCode = showUI(); return true; }
        int count = 0; LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &count);
        if (!args) throw std::runtime_error("Cannot read import command line");
        struct ArgsCleanup { LPWSTR* args; ~ArgsCleanup() { LocalFree(args); } } cleanup{args};
        ImportOptions48C options;
        std::uint32_t cancelAfter = 0;
        for (int i = 2; i < count; ++i) {
            const std::wstring key = args[i];
            if (++i >= count) throw std::runtime_error("Missing import option value");
            if (key == L"--source") options.source = args[i];
            else if (key == L"--cache") options.cache = args[i];
            else if (key == L"--container") options.container = utf8(args[i]);
            else if (key == L"--cancel-after") {
                const std::wstring value = args[i]; std::size_t consumed = 0;
                const auto parsed = std::stoul(value, &consumed);
                if (consumed != value.size() || parsed == 0 || parsed > 1000000) throw std::runtime_error("Invalid cancellation check interval");
                cancelAfter = parsed;
            } else throw std::runtime_error("Unknown import option");
        }
        std::atomic<bool> cancel{false};
        if (std::string(argv[1]) == "--import-ui-check") {
            if (options.source.empty() || options.cache.empty() || cancelAfter || !options.container.empty())
                throw std::runtime_error("UI check requires only source and cache paths");
            exitCode = showUI(&options); return true;
        }
        const auto result = importGame48C(options, cancel, [&](const ImportProgress48C& progress) {
            if (cancelAfter && progress.converted + progress.reused >= cancelAfter) cancel.store(true);
        });
        std::cout << importResultJson48C(result) << '\n';
        exitCode = result.success ? 0 : result.cancelled ? 130 : 1;
    } catch (const std::exception& error) {
        ImportResult48C result; result.error = error.what(); std::cout << importResultJson48C(result) << '\n'; exitCode = 1;
    }
    return true;
}
} }
