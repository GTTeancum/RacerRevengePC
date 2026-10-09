#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <psapi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "native_asset48b.h"
#include "scene_asset48d.h"
#include "import_ui48c.h"
#include "import_service48e.h"
#include "import_service48f.h"
#include "import_service48i.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;
using Clock48B = std::chrono::steady_clock;
namespace {
constexpr UINT imageWidth = 960, imageHeight = 720;
struct Options {
    std::string mesh, texture, capture, report, cache, container, member;
    unsigned frames = 1;
    bool interactive = false, semanticScene = false, world = false, worldCollision = false;
};
struct Result {
    std::string error, driver, adapter, hardwareError;
    std::string material = "neutral opaque inspection material; source color and alpha not applied";
    std::vector<std::string> modules;
    std::vector<double> submissionMs;
    rr::assets::Bounds48B bounds{};
    std::array<float, 3> center{}, eye{};
    unsigned vertices = 0, indices = 0, textureWidth = 0, textureHeight = 0;
    unsigned sceneNodes = 0, sceneLods = 0, sceneMaterials = 0, drawSpans = 0;
    unsigned boundTextures = 0, texturedSpans = 0, nativeMipLevels = 0;
    unsigned worldInstances = 0, worldVisualInstances = 0, worldCollisionOnly = 0;
    unsigned worldUniqueGeometry = 0, worldZones = 0, worldPortals = 0, worldRootsReplaced = 0;
    unsigned worldUniqueVertices = 0, worldUniqueIndices = 0;
    unsigned collisionInstances = 0, collisionUniqueAssets = 0, collisionAbsentMembers = 0;
    std::size_t collisionPayloadBytes = 0;
    std::size_t worldSemanticPayloadBytes = 0;
    double worldCacheLoadMs = 0, worldAssemblyMs = 0;
    std::string worldMeshHash;
    std::vector<std::string> bindingWarnings;
    unsigned submitted = 0, presented = 0, featureLevel = 0;
    std::size_t cpuAssetBytes = 0, gpuOwnedBytes = 0;
    double initializationMs = 0, loopMs = 0, readbackMs = 0;
    bool success = false, captureWritten = false, lastFrameReadback = false;
};
double elapsedMs(Clock48B::time_point start) {
    return std::chrono::duration<double, std::milli>(Clock48B::now() - start).count();
}
std::string hexError(HRESULT hr) {
    std::ostringstream stream;
    stream << "0x" << std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned long>(hr);
    return stream.str();
}
void check(HRESULT hr, const char* operation) {
    if (FAILED(hr)) throw std::runtime_error(std::string(operation) + ": " + hexError(hr));
}
std::string utf8(const wchar_t* input) {
    const int required = WideCharToMultiByte(CP_UTF8, 0, input, -1, nullptr, 0, nullptr, nullptr);
    if (required <= 0) return "";
    std::vector<char> buffer(static_cast<std::size_t>(required));
    WideCharToMultiByte(CP_UTF8, 0, input, -1, buffer.data(), required, nullptr, nullptr);
    return std::string(buffer.data());
}
std::string absolutePath(const std::string& input) {
    if (input.empty()) return "";
    const DWORD count = GetFullPathNameA(input.c_str(), 0, nullptr, nullptr);
    if (!count) return input;
    std::vector<char> buffer(static_cast<std::size_t>(count) + 1);
    if (!GetFullPathNameA(input.c_str(), static_cast<DWORD>(buffer.size()), buffer.data(), nullptr)) return input;
    return std::string(buffer.data());
}
std::string quote(const std::string& input) {
    std::ostringstream out;
    out << '"';
    for (unsigned char c : input) {
        switch (c) {
        case '"': out << "\\\""; break;
        case '\\': out << "\\\\"; break;
        case '\n': out << "\\n"; break;
        case '\r': out << "\\r"; break;
        case '\t': out << "\\t"; break;
        default:
            if (c < 32) out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << unsigned(c) << std::dec;
            else out << char(c);
        }
    }
    out << '"';
    return out.str();
}
std::vector<std::string> loadedModules() {
    std::vector<HMODULE> handles(128);
    DWORD required = 0;
    for (;;) {
        if (!EnumProcessModulesEx(GetCurrentProcess(), handles.data(),
                static_cast<DWORD>(handles.size() * sizeof(HMODULE)), &required, LIST_MODULES_ALL))
            throw std::runtime_error("module inventory failed");
        if (required <= handles.size() * sizeof(HMODULE)) break;
        handles.resize(required / sizeof(HMODULE) + 8);
    }
    std::vector<std::string> names;
    std::vector<wchar_t> path(32768);
    for (std::size_t i = 0; i < required / sizeof(HMODULE); ++i) {
        const DWORD length = GetModuleFileNameExW(GetCurrentProcess(), handles[i], path.data(), static_cast<DWORD>(path.size()));
        if (!length || length >= path.size()) throw std::runtime_error("module path inventory failed");
        path[length] = 0;
        names.push_back(utf8(path.data()));
    }
    std::sort(names.begin(), names.end());
    return names;
}
void reportResult(const Options& options, const Result& result) {
    if (options.report.empty()) return;
    std::ofstream out(options.report, std::ios::binary | std::ios::trunc);
    if (!out) throw std::runtime_error("cannot create report");
    out << std::setprecision(10);
    out << "{\n  \"schema\": \"native-scene-inspection48B-v1\",\n  \"success\": " << (result.success ? "true" : "false")
        << ",\n  \"error\": " << quote(result.error)
        << ",\n  \"pointer_bits\": " << sizeof(void*) * 8
        << ",\n  \"mesh_path\": " << quote(absolutePath(options.mesh))
        << ",\n  \"semantic_scene\": " << (options.semanticScene ? "true" : "false")
        << ",\n  \"world_scene\": " << (options.world ? "true" : "false")
        << ",\n  \"native_collision_world_loaded\": " << (options.worldCollision && result.success ? "true" : "false")
        << ",\n  \"collision_placed_instances\": " << result.collisionInstances
        << ",\n  \"collision_unique_assets\": " << result.collisionUniqueAssets
        << ",\n  \"collision_absent_original_members\": " << result.collisionAbsentMembers
        << ",\n  \"collision_retained_payload_bytes\": " << result.collisionPayloadBytes
        << ",\n  \"collision_runtime_format\": \"native48I compact records\""
        << ",\n  \"collision_placement_arithmetic\": \"original-order binary32\""
        << ",\n  \"world_original_instances\": " << result.worldInstances
        << ",\n  \"world_visual_instances\": " << result.worldVisualInstances
        << ",\n  \"world_collision_only_instances\": " << result.worldCollisionOnly
        << ",\n  \"world_unique_geometry\": " << result.worldUniqueGeometry
        << ",\n  \"world_unique_vertices\": " << result.worldUniqueVertices
        << ",\n  \"world_unique_indices\": " << result.worldUniqueIndices
        << ",\n  \"world_zones\": " << result.worldZones
        << ",\n  \"world_portals\": " << result.worldPortals
        << ",\n  \"world_saved_root_matrices_replaced\": " << result.worldRootsReplaced
        << ",\n  \"world_cache_load_ms\": " << result.worldCacheLoadMs
        << ",\n  \"world_assembly_ms\": " << result.worldAssemblyMs
        << ",\n  \"world_source_semantic_payload_bytes\": " << result.worldSemanticPayloadBytes
        << ",\n  \"world_placed_mesh_sha256\": " << quote(result.worldMeshHash)
        << ",\n  \"world_buffer_policy\": " << quote(options.world ? "one-time immutable placed LOD0 buffers; unique source meshes loaded once, placed GPU vertices duplicated by instance" : "not applicable")
        << ",\n  \"cache_path\": " << quote(absolutePath(options.cache))
        << ",\n  \"source_container\": " << quote(options.container)
        << ",\n  \"source_member\": " << quote(options.member)
        << ",\n  \"bound_native_textures\": " << result.boundTextures
        << ",\n  \"native_mip_levels\": " << result.nativeMipLevels
        << ",\n  \"textured_spans_per_frame\": " << result.texturedSpans
        << ",\n  \"scene_nodes\": " << result.sceneNodes
        << ",\n  \"scene_lods\": " << result.sceneLods
        << ",\n  \"scene_materials\": " << result.sceneMaterials
        << ",\n  \"draw_spans_per_frame\": " << result.drawSpans
        << ",\n  \"texture_path\": " << quote(absolutePath(options.texture))
        << ",\n  \"capture_path\": " << quote(absolutePath(options.capture))
        << ",\n  \"capture_written\": " << (result.captureWritten ? "true" : "false")
        << ",\n  \"render_size\": [" << imageWidth << ", " << imageHeight << "]"
        << ",\n  \"driver_type\": " << quote(result.driver)
        << ",\n  \"adapter\": " << quote(result.adapter)
        << ",\n  \"hardware_creation_error\": " << quote(result.hardwareError)
        << ",\n  \"feature_level\": " << result.featureLevel
        << ",\n  \"vertex_count\": " << result.vertices
        << ",\n  \"index_count\": " << result.indices
        << ",\n  \"triangle_count\": " << result.indices / 3
        << ",\n  \"texture_dimensions\": [" << result.textureWidth << ", " << result.textureHeight << "]"
        << ",\n  \"material\": " << quote(result.material)
        << ",\n  \"bounds_min\": [" << result.bounds.minimum[0] << ", " << result.bounds.minimum[1] << ", " << result.bounds.minimum[2] << "]"
        << ",\n  \"bounds_max\": [" << result.bounds.maximum[0] << ", " << result.bounds.maximum[1] << ", " << result.bounds.maximum[2] << "]"
        << ",\n  \"camera_center\": [" << result.center[0] << ", " << result.center[1] << ", " << result.center[2] << "]"
        << ",\n  \"last_camera_eye\": [" << result.eye[0] << ", " << result.eye[1] << ", " << result.eye[2] << "]"
        << ",\n  \"model_transform\": " << quote(options.world ? "original SCB instance placement replaces saved root matrix; local positions transformed once at load" : "identity; original file positions retained")
        << ",\n  \"frames_requested\": " << options.frames
        << ",\n  \"frames_submitted\": " << result.submitted
        << ",\n  \"frames_presented\": " << result.presented
        << ",\n  \"last_frame_completed_readback\": " << (result.lastFrameReadback ? "true" : "false")
        << ",\n  \"interactive\": " << (options.interactive ? "true" : "false")
        << ",\n  \"initialization_ms\": " << result.initializationMs
        << ",\n  \"render_loop_wall_ms\": " << result.loopMs
        << ",\n  \"final_readback_ms\": " << result.readbackMs
        << ",\n  \"gpu_time_ms\": null"
        << ",\n  \"cpu_owned_asset_bytes\": " << result.cpuAssetBytes
        << ",\n  \"asset_byte_scope\": \"packed draw buffers and retained texture mip payloads; excludes semantic structures, transient load buffers and allocator overhead\""
        << ",\n  \"gpu_resource_payload_bytes\": " << result.gpuOwnedBytes
        << ",\n  \"limits\": {\"vertices\": " << rr::assets::max_mesh_vertices
        << ", \"indices\": " << rr::assets::max_mesh_indices
        << ", \"texture_dimension\": " << rr::assets::max_texture_dimension
        << ", \"asset_file_bytes\": " << rr::assets::max_asset_file_bytes << "}"
        << ",\n  \"cpu_submission_ms\": [";
    for (std::size_t i = 0; i < result.submissionMs.size(); ++i) out << (i ? ", " : "") << result.submissionMs[i];
    out << "],\n  \"loaded_modules\": [";
    for (std::size_t i = 0; i < result.modules.size(); ++i) out << (i ? ",\n    " : "\n    ") << quote(result.modules[i]);
    out << "\n  ],\n  \"binding_warnings\": [";
    for (std::size_t i = 0; i < result.bindingWarnings.size(); ++i) out << (i ? ", " : "") << quote(result.bindingWarnings[i]);
    out << "],\n  \"limitations\": ["
        << "\"Static asset inspection, not native gameplay or a game FPS benchmark\", "
        << "\"Geometry, UV, source color, palette and winding semantics need independent validation\", "
        << "\"Two-sided draw, opaque alpha, no lighting, fog, original material program; diagnostic sampling uses the base mip\", "
        << "\"No simulation, vehicle controls, collision, audio, moving actors, sky or original zone visibility\", "
        << "\"Submission timing does not measure GPU execution; offscreen frames are not presented\", "
        << "\"Memory totals are owned payload sizes, excluding driver/CRT overhead and transient file buffers; not Xbox residency\", "
        << "\"Paths use Windows narrow-character file APIs\""
        << "]\n}\n";
    out.close();
    if (!out) throw std::runtime_error("report write failed");
}
struct PathIdentity48B {
    std::wstring absolute, resolved;
    BY_HANDLE_FILE_INFORMATION file{};
    bool exists = false;
};
struct PathHandle48B {
    HANDLE value = INVALID_HANDLE_VALUE;
    explicit PathHandle48B(HANDLE handle) : value(handle) {}
    ~PathHandle48B() { if (value != INVALID_HANDLE_VALUE) CloseHandle(value); }
    PathHandle48B(const PathHandle48B&) = delete;
    PathHandle48B& operator=(const PathHandle48B&) = delete;
};
std::wstring wideFilePath(const std::string& path) {
    // The file streams use Windows narrow paths too, so preserve the same ACP interpretation.
    const int count = MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, nullptr, 0);
    if (count <= 0) throw std::runtime_error("cannot interpret asset/output path");
    std::vector<wchar_t> wide(static_cast<std::size_t>(count));
    if (!MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, wide.data(), count))
        throw std::runtime_error("cannot convert asset/output path");
    const DWORD required = GetFullPathNameW(wide.data(), 0, nullptr, nullptr);
    if (!required) throw std::runtime_error("cannot resolve asset/output path");
    std::vector<wchar_t> absolute(static_cast<std::size_t>(required) + 1);
    const DWORD length = GetFullPathNameW(wide.data(), static_cast<DWORD>(absolute.size()), absolute.data(), nullptr);
    if (!length || length >= absolute.size()) throw std::runtime_error("cannot resolve full asset/output path");
    std::wstring result(absolute.data(), length);
    std::replace(result.begin(), result.end(), L'/', L'\\');
    return result;
}
std::wstring resolvedHandlePath(HANDLE handle) {
    const DWORD required = GetFinalPathNameByHandleW(handle, nullptr, 0, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
    if (!required) throw std::runtime_error("cannot verify resolved asset/output path");
    std::vector<wchar_t> finalPath(static_cast<std::size_t>(required) + 1);
    const DWORD length = GetFinalPathNameByHandleW(handle, finalPath.data(), static_cast<DWORD>(finalPath.size()),
        FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
    if (!length || length >= finalPath.size()) throw std::runtime_error("cannot verify final asset/output path");
    return std::wstring(finalPath.data(), length);
}
PathIdentity48B identifyPath(const std::string& path) {
    PathIdentity48B identity;
    identity.absolute = wideFilePath(path);
    std::wstring probe = identity.absolute, suffix;
    for (;;) {
        PathHandle48B handle(CreateFileW(probe.c_str(), FILE_READ_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS, nullptr));
        if (handle.value != INVALID_HANDLE_VALUE) {
            if (suffix.empty()) {
                identity.exists = true;
                if (!GetFileInformationByHandle(handle.value, &identity.file))
                    throw std::runtime_error("cannot verify asset/output file identity");
            }
            identity.resolved = resolvedHandlePath(handle.value);
            if (!suffix.empty()) {
                if (!identity.resolved.empty() && identity.resolved.back() == L'\\') identity.resolved.pop_back();
                identity.resolved += suffix;
            }
            return identity;
        }
        const DWORD failure = GetLastError();
        if (failure != ERROR_FILE_NOT_FOUND && failure != ERROR_PATH_NOT_FOUND)
            throw std::runtime_error("cannot safely verify asset/output path identity, Windows error " + std::to_string(failure));
        const DWORD attributes = GetFileAttributesW(probe.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_REPARSE_POINT))
            throw std::runtime_error("cannot safely verify an unresolved asset/output link");
        const std::size_t separator = probe.find_last_of(L'\\');
        if (separator == std::wstring::npos || separator == probe.size() - 1)
            throw std::runtime_error("cannot find existing parent for asset/output path");
        std::wstring component = probe.substr(separator + 1);
        // Ordinary Win32 filenames discard final spaces/dots. Preserve extended-path semantics.
        if (identity.absolute.rfind(L"\\\\?\\", 0) != 0)
            while (!component.empty() && (component.back() == L'.' || component.back() == L' ')) component.pop_back();
        if (component.empty()) throw std::runtime_error("ambiguous empty asset/output filename");
        suffix = L"\\" + component + suffix;
        probe.resize(separator + ((separator > 0 && probe[separator - 1] == L':') ? 1 : 0));
        if (probe.empty()) throw std::runtime_error("cannot verify asset/output root");
    }
}
bool samePath(const PathIdentity48B& a, const PathIdentity48B& b) {
    const auto sameName = [](const std::wstring& left, const std::wstring& right) {
        return CompareStringOrdinal(left.c_str(), -1, right.c_str(), -1, TRUE) == CSTR_EQUAL;
    };
    if (sameName(a.absolute, b.absolute) || sameName(a.resolved, b.resolved)) return true;
    return a.exists && b.exists && a.file.dwVolumeSerialNumber == b.file.dwVolumeSerialNumber &&
        a.file.nFileIndexHigh == b.file.nFileIndexHigh && a.file.nFileIndexLow == b.file.nFileIndexLow;
}
void rejectPathAliases(const Options& options) {
    const std::string paths[] = {options.mesh, options.texture, options.capture, options.report};
    const char* names[] = {"mesh", "texture", "capture", "report"};
    std::array<PathIdentity48B, 4> identities;
    for (unsigned i = 0; i < 4; ++i) if (!paths[i].empty()) identities[i] = identifyPath(paths[i]);
    for (unsigned output = 2; output < 4; ++output) {
        if (paths[output].empty()) continue;
        for (unsigned previous = 0; previous < output; ++previous) {
            if (!paths[previous].empty() && samePath(identities[output], identities[previous]))
                throw std::runtime_error(std::string("path alias rejected: ") + names[output] + " overlaps " + names[previous]);
        }
    }
}
Options parseOptions(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string key = argv[i];
        if (key == "--interactive") { options.interactive = true; continue; }
        if (key == "--world") { options.world = true; continue; }
        if (key == "--world-collision") { options.world = options.worldCollision = true; continue; }
        if (key != "--mesh" && key != "--scene" && key != "--cache" && key != "--container" && key != "--member" && key != "--texture" && key != "--capture" && key != "--frames" && key != "--report")
            throw std::runtime_error("unknown option: " + key);
        if (++i >= argc) throw std::runtime_error("missing option value: " + key);
        const std::string value = argv[i];
        if (key == "--mesh" || key == "--scene") {
            if (!options.mesh.empty()) throw std::runtime_error("Select exactly one --mesh or --scene input");
            options.mesh = value; options.semanticScene = key == "--scene";
        }
        else if (key == "--cache") options.cache = value;
        else if (key == "--container") options.container = value;
        else if (key == "--member") options.member = value;
        else if (key == "--texture") options.texture = value;
        else if (key == "--capture") options.capture = value;
        else if (key == "--report") options.report = value;
        else {
            if (value.empty() || value.find_first_not_of("0123456789") != std::string::npos)
                throw std::runtime_error("frames must be an integer in [1,10000]");
            const unsigned long count = std::strtoul(value.c_str(), nullptr, 10);
            if (count < 1 || count > 10000) throw std::runtime_error("frames outside [1,10000]");
            options.frames = static_cast<unsigned>(count);
        }
    }
    if (!options.cache.empty()) {
        if (!options.mesh.empty() || !options.texture.empty() || options.container.empty() || options.member.empty())
            throw std::runtime_error("--cache requires --container and --member, without --mesh/--scene/--texture");
        options.semanticScene = true;
        // Cache inspections use fresh outputs outside the input tree. Reject
        // existing outputs as well, including hardlinks into cache objects.
        const auto root = std::filesystem::weakly_canonical(options.cache);
        for (const auto& output : {options.capture, options.report}) if (!output.empty()) {
            if (std::filesystem::exists(output)) throw std::runtime_error("Cache inspection outputs must be new files");
            const auto path = std::filesystem::weakly_canonical(output);
            auto a = root.begin(), b = path.begin(); bool contained = true;
            for (; a != root.end(); ++a, ++b) if (b == path.end() || _wcsicmp(a->c_str(), b->c_str()) != 0) { contained = false; break; }
            if (contained) throw std::runtime_error("Inspection outputs must be outside the cache");
        }
    } else if (options.world || options.mesh.empty() || !options.container.empty() || !options.member.empty())
        throw std::runtime_error("--mesh/--scene or a complete --cache selection is required");
    // Complete this check before returning Options: main's error handler must not
    // acquire an unsafe --report path when an input/output alias is rejected.
    rejectPathAliases(options);
    return options;
}
LRESULT CALLBACK windowProcedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_CLOSE) { DestroyWindow(window); return 0; }
    if (message == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(window, message, wparam, lparam);
}
struct NativeWindow {
    HWND handle = nullptr;
    ~NativeWindow() { if (handle && IsWindow(handle)) DestroyWindow(handle); }
    void create() {
        WNDCLASSW type{};
        type.lpfnWndProc = windowProcedure;
        type.hInstance = GetModuleHandleW(nullptr);
        type.lpszClassName = L"RacerNativeInspection48B";
        type.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
        if (!RegisterClassW(&type) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            throw std::runtime_error("window registration failed");
        RECT rectangle{0, 0, static_cast<LONG>(imageWidth), static_cast<LONG>(imageHeight)};
        const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        AdjustWindowRect(&rectangle, style, FALSE);
        handle = CreateWindowW(type.lpszClassName, L"Racer Revenge native asset inspection | arrows orbit, +/- zoom, Esc closes",
            style, CW_USEDEFAULT, CW_USEDEFAULT, rectangle.right - rectangle.left, rectangle.bottom - rectangle.top,
            nullptr, nullptr, type.hInstance, nullptr);
        if (!handle) throw std::runtime_error("window creation failed");
        ShowWindow(handle, SW_SHOW);
    }
};
const char* shader = R"HLSL(
cbuffer Camera : register(b0) { float4x4 worldViewProjection; uint useTexture; float3 padding; };
struct Input { float3 position : POSITION; float2 uv : TEXCOORD0; float4 color : COLOR0; };
struct Varying { float4 position : SV_POSITION; float2 uv : TEXCOORD0; float4 color : COLOR0; };
Varying vertexMain(Input input) {
    Varying result;
    result.position = mul(float4(input.position, 1.0), worldViewProjection);
    result.uv = input.uv; result.color = input.color;
    return result;
}
Texture2D image : register(t0);
SamplerState imageSampler : register(s0);
float4 pixelMain(Varying input) : SV_TARGET {
    if (useTexture != 0) return float4(image.Sample(imageSampler, input.uv).rgb * input.color.rgb, 1.0);
    return float4(0.68, 0.72, 0.78, 1.0);
}
)HLSL";
ComPtr<ID3DBlob> compileShader(const char* entry, const char* profile) {
    ComPtr<ID3DBlob> code, errors;
    const HRESULT hr = D3DCompile(shader, std::char_traits<char>::length(shader), "native-inspection48B", nullptr, nullptr,
        entry, profile, D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &code, &errors);
    if (FAILED(hr)) {
        const std::string detail = errors ? std::string(static_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize()) : hexError(hr);
        throw std::runtime_error("native shader compilation: " + detail);
    }
    return code;
}
void writePpm(const std::string& path, const std::vector<std::uint8_t>& rgba) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) throw std::runtime_error("cannot create capture");
    out << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
    std::vector<char> row(std::size_t(imageWidth) * 3);
    for (UINT y = 0; y < imageHeight; ++y) {
        for (UINT x = 0; x < imageWidth; ++x)
            for (unsigned c = 0; c < 3; ++c) row[std::size_t(x) * 3 + c] = char(rgba[(std::size_t(y) * imageWidth + x) * 4 + c]);
        out.write(row.data(), static_cast<std::streamsize>(row.size()));
    }
    out.close();
    if (!out) throw std::runtime_error("capture write failed");
}

void inspect(const Options& options, Result& result) {
    const auto initializationStart = Clock48B::now();
    rr::assets::Mesh48B mesh;
    rr::assets::Scene48D scene;
    std::vector<rr::assets::SceneSpan48D> drawSpans;
    rr::windows::RuntimeScene48E cachedScene;
    rr::windows::RuntimeCollisionWorld48I collisionWorld;
    rr::assets::Texture48B texture;
    std::string error;
    if (options.world) {
        rr::windows::RuntimeWorld48F world;
        if (options.worldCollision) {
            rr::windows::RuntimeWorld48I combined;
            if (!rr::windows::assembleCachedWorld48I(options.cache, options.container, options.member, combined, error))
                throw std::runtime_error("collision world: " + error);
            world = std::move(combined.visuals); collisionWorld = std::move(combined.collisions);
            result.collisionInstances = static_cast<unsigned>(collisionWorld.instances.size());
            result.collisionUniqueAssets = static_cast<unsigned>(collisionWorld.assets.size());
            result.collisionAbsentMembers = collisionWorld.missing_original_members;
            result.collisionPayloadBytes = collisionWorld.retained_payload_bytes;
        } else if (!rr::windows::assembleCachedWorld48F(options.cache, options.container, options.member, world, error))
            throw std::runtime_error("world: " + error);
        mesh = std::move(world.mesh); drawSpans = std::move(world.spans);
        cachedScene.materials = std::move(world.materials); cachedScene.textures = std::move(world.textures);
        result.bindingWarnings = std::move(world.warnings);
        result.worldInstances = world.source_instances; result.worldVisualInstances = world.visual_instances;
        result.worldCollisionOnly = world.collision_only_instances; result.worldUniqueGeometry = world.unique_geometry;
        result.worldZones = world.zones; result.worldPortals = world.portals; result.worldRootsReplaced = world.replaced_root_matrices;
        result.worldUniqueVertices = world.unique_vertices; result.worldUniqueIndices = world.unique_indices;
        result.worldSemanticPayloadBytes = world.semantic_payload_bytes;
        result.worldMeshHash = world.placed_mesh_sha256;
        result.worldCacheLoadMs = world.cache_load_ms; result.worldAssemblyMs = world.assembly_ms;
        result.sceneMaterials = static_cast<unsigned>(cachedScene.materials.size());
        result.drawSpans = static_cast<unsigned>(drawSpans.size());
    } else if (options.semanticScene) {
        if (!options.cache.empty()) {
            if (!rr::windows::loadCachedScene48E(options.cache, options.container, options.member, cachedScene, error))
                throw std::runtime_error("cache: " + error);
            scene = std::move(cachedScene.scene); result.bindingWarnings = cachedScene.warnings;
        } else {
        std::ifstream input(options.mesh, std::ios::binary | std::ios::ate);
        if (!input) throw std::runtime_error("Cannot open native scene");
        const auto size = input.tellg();
        if (size < 0 || static_cast<std::uint64_t>(size) > rr::assets::max_scene_bytes48d) throw std::runtime_error("Native scene size limit");
        std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size)); input.seekg(0);
        if (!input.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size())))
            throw std::runtime_error("Truncated native scene");
        if (!rr::assets::parseSceneAsset48D(bytes, scene, error)) throw std::runtime_error("scene: " + error);
        }
        // Build the inspection draw buffers from native scene attributes and bind
        // transforms. The compatibility mesh is a validation oracle, not the draw source.
        for (const auto& surface : scene.lods[0].surfaces) {
            const auto base = static_cast<std::uint32_t>(mesh.vertices.size());
            for (const auto& source : surface.vertices) {
                rr::assets::Vertex48B v{}; const auto& m = scene.nodes[source.bone].bind_matrix;
                for (unsigned c = 0; c < 3; ++c) v.position[c] = static_cast<float>(
                    source.position_local[0] * m[c] + source.position_local[1] * m[4 + c] +
                    source.position_local[2] * m[8 + c] + m[12 + c]);
                for (unsigned c = 0; c < 2; ++c) v.uv[c] = static_cast<float>(source.uv[c]);
                for (unsigned c = 0; c < 4; ++c) v.color[c] = surface.has_original_colors ? source.color[c] : 255;
                if (mesh.vertices.empty()) for (unsigned c = 0; c < 3; ++c) mesh.bounds.minimum[c] = mesh.bounds.maximum[c] = v.position[c];
                else for (unsigned c = 0; c < 3; ++c) {
                    mesh.bounds.minimum[c] = std::min(mesh.bounds.minimum[c], v.position[c]);
                    mesh.bounds.maximum[c] = std::max(mesh.bounds.maximum[c], v.position[c]);
                }
                mesh.vertices.push_back(v);
            }
            for (auto index : surface.indices) mesh.indices.push_back(base + index);
        }
        result.sceneNodes = static_cast<unsigned>(scene.nodes.size());
        result.sceneLods = static_cast<unsigned>(scene.lods.size());
        result.sceneMaterials = static_cast<unsigned>(scene.materials.size());
        result.drawSpans = static_cast<unsigned>(scene.spans_lod0.size());
        drawSpans = scene.spans_lod0;
    } else {
        if (!rr::assets::loadMesh48B(options.mesh.c_str(), mesh, error)) throw std::runtime_error("mesh: " + error);
        result.drawSpans = 1;
    }
    if (!options.texture.empty() && !rr::assets::loadTexture48B(options.texture.c_str(), texture, error))
        throw std::runtime_error("texture: " + error);
    result.vertices = static_cast<unsigned>(mesh.vertices.size());
    result.indices = static_cast<unsigned>(mesh.indices.size());
    result.bounds = mesh.bounds;
    result.textureWidth = texture.width; result.textureHeight = texture.height;
    result.cpuAssetBytes = mesh.vertices.size() * sizeof(rr::assets::Vertex48B) + mesh.indices.size() * 4 + texture.rgba.size() + result.collisionPayloadBytes;
    if (!texture.rgba.empty()) result.material = "candidate texture RGB multiplied by source vertex RGB; opaque alpha, base mip only";
    double radiusSquared = 0;
    for (unsigned i = 0; i < 3; ++i) {
        const double low = mesh.bounds.minimum[i], high = mesh.bounds.maximum[i];
        result.center[i] = static_cast<float>((low + high) * 0.5);
        if (std::abs(static_cast<double>(result.center[i])) > 1e15)
            throw std::runtime_error("mesh center exceeds inspection camera range");
        const double half = (high - low) * 0.5;
        radiusSquared += half * half;
    }
    const double radiusValue = std::max(std::sqrt(radiusSquared), 0.0001);
    if (!std::isfinite(radiusValue) || radiusValue > 1e15)
        throw std::runtime_error("mesh extent exceeds inspection camera range");
    const float radius = static_cast<float>(radiusValue), distance = radius * 3.2f;

    NativeWindow window;
    if (options.interactive) window.create();
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    D3D_FEATURE_LEVEL level{};
    const D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        levels, static_cast<UINT>(std::size(levels)), D3D11_SDK_VERSION, &device, &level, &context);
    result.driver = "hardware";
    if (FAILED(hr)) {
        result.hardwareError = hexError(hr);
        device.Reset(); context.Reset();
        check(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            levels, static_cast<UINT>(std::size(levels)), D3D11_SDK_VERSION, &device, &level, &context), "D3D11 WARP fallback creation");
        result.driver = "WARP software fallback";
    }
    result.featureLevel = static_cast<unsigned>(level);
    ComPtr<IDXGIDevice> dxgiDevice;
    check(device.As(&dxgiDevice), "DXGI device query");
    ComPtr<IDXGIAdapter> adapter;
    check(dxgiDevice->GetAdapter(&adapter), "DXGI adapter query");
    DXGI_ADAPTER_DESC adapterDescription{};
    check(adapter->GetDesc(&adapterDescription), "DXGI adapter description");
    result.adapter = utf8(adapterDescription.Description);
    ComPtr<IDXGISwapChain> swapchain;
    if (options.interactive) {
        ComPtr<IDXGIFactory> factory;
        check(adapter->GetParent(IID_PPV_ARGS(&factory)), "DXGI factory query");
        DXGI_SWAP_CHAIN_DESC description{};
        description.BufferDesc.Width = imageWidth; description.BufferDesc.Height = imageHeight;
        description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        description.SampleDesc.Count = 1; description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        description.BufferCount = 1; description.OutputWindow = window.handle; description.Windowed = TRUE;
        description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        check(factory->CreateSwapChain(device.Get(), &description, &swapchain), "swapchain creation");
        check(factory->MakeWindowAssociation(window.handle, DXGI_MWA_NO_ALT_ENTER), "window association");
    }
    D3D11_TEXTURE2D_DESC targetDescription{};
    targetDescription.Width = imageWidth; targetDescription.Height = imageHeight;
    targetDescription.MipLevels = 1; targetDescription.ArraySize = 1;
    targetDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    targetDescription.SampleDesc.Count = 1; targetDescription.Usage = D3D11_USAGE_DEFAULT;
    targetDescription.BindFlags = D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D> target;
    check(device->CreateTexture2D(&targetDescription, nullptr, &target), "offscreen target creation");
    ComPtr<ID3D11RenderTargetView> targetView;
    check(device->CreateRenderTargetView(target.Get(), nullptr, &targetView), "render target view creation");
    D3D11_TEXTURE2D_DESC depthDescription = targetDescription;
    depthDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ComPtr<ID3D11Texture2D> depth;
    check(device->CreateTexture2D(&depthDescription, nullptr, &depth), "depth buffer creation");
    ComPtr<ID3D11DepthStencilView> depthView;
    check(device->CreateDepthStencilView(depth.Get(), nullptr, &depthView), "depth view creation");
    auto createBuffer = [&](UINT bytes, UINT bind, const void* data) {
        D3D11_BUFFER_DESC description{};
        description.ByteWidth = bytes; description.BindFlags = bind;
        description.Usage = data ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
        D3D11_SUBRESOURCE_DATA initial{}; initial.pSysMem = data;
        ComPtr<ID3D11Buffer> buffer;
        check(device->CreateBuffer(&description, data ? &initial : nullptr, &buffer), "native GPU buffer creation");
        return buffer;
    };
    const auto vertices = createBuffer(static_cast<UINT>(mesh.vertices.size() * sizeof(rr::assets::Vertex48B)), D3D11_BIND_VERTEX_BUFFER, mesh.vertices.data());
    const auto indices = createBuffer(static_cast<UINT>(mesh.indices.size() * 4), D3D11_BIND_INDEX_BUFFER, mesh.indices.data());
    struct alignas(16) Camera { DirectX::XMFLOAT4X4 matrix; UINT textured; UINT padding[3]; };
    static_assert(sizeof(Camera) == 80, "Shader constant layout");
    const auto constants = createBuffer(sizeof(Camera), D3D11_BIND_CONSTANT_BUFFER, nullptr);
    const auto vertexCode = compileShader("vertexMain", "vs_4_0"), pixelCode = compileShader("pixelMain", "ps_4_0");
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    check(device->CreateVertexShader(vertexCode->GetBufferPointer(), vertexCode->GetBufferSize(), nullptr, &vertexShader), "vertex shader creation");
    check(device->CreatePixelShader(pixelCode->GetBufferPointer(), pixelCode->GetBufferSize(), nullptr, &pixelShader), "pixel shader creation");
    const D3D11_INPUT_ELEMENT_DESC elements[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    ComPtr<ID3D11InputLayout> layout;
    check(device->CreateInputLayout(elements, static_cast<UINT>(std::size(elements)), vertexCode->GetBufferPointer(), vertexCode->GetBufferSize(), &layout), "native input layout");
    D3D11_RASTERIZER_DESC rasterDescription{};
    rasterDescription.FillMode = D3D11_FILL_SOLID; rasterDescription.CullMode = D3D11_CULL_NONE; rasterDescription.DepthClipEnable = TRUE;
    ComPtr<ID3D11RasterizerState> raster;
    check(device->CreateRasterizerState(&rasterDescription, &raster), "two-sided inspection rasterizer");
    ComPtr<ID3D11ShaderResourceView> imageView;
    std::vector<ComPtr<ID3D11ShaderResourceView>> materialViews;
    std::vector<int> surfaceTextures;
    ComPtr<ID3D11SamplerState> sampler;
    for (const auto& source : cachedScene.textures) {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = source.levels.back().width; desc.Height = source.levels.back().height;
        desc.MipLevels = static_cast<UINT>(source.levels.size()); desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_IMMUTABLE; desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        std::vector<D3D11_SUBRESOURCE_DATA> data;
        for (auto it = source.levels.rbegin(); it != source.levels.rend(); ++it) {
            D3D11_SUBRESOURCE_DATA levelData{}; levelData.pSysMem = it->rgba.data(); levelData.SysMemPitch = it->width * 4;
            data.push_back(levelData); result.cpuAssetBytes += it->rgba.size(); ++result.nativeMipLevels;
        }
        ComPtr<ID3D11Texture2D> image; ComPtr<ID3D11ShaderResourceView> view;
        check(device->CreateTexture2D(&desc, data.data(), &image), "native material mip texture");
        check(device->CreateShaderResourceView(image.Get(), nullptr, &view), "native material texture view");
        materialViews.push_back(std::move(view));
    }
    result.boundTextures = static_cast<unsigned>(materialViews.size());
    if (!options.cache.empty()) {
        result.material = "original material/pass/slot bindings; diagnostic first-pass slot-one RGB times vertex RGB, opaque alpha; original shaders not applied";
        for (const auto& span : drawSpans) {
            int textureIndex = -1;
            const auto& material = cachedScene.materials.at(span.material_index);
            if (!material.passes.empty()) {
                for (const auto& slot : material.passes[0].textures) if (slot.slot == 1) textureIndex = slot.texture;
                result.bindingWarnings.push_back("Diagnostic shader for " + material.name + ": " + material.passes[0].program);
                if (material.passes.size() > 1 || material.passes[0].textures.size() > 1)
                    result.bindingWarnings.push_back("Additional passes/maps retained but not drawn: " + material.name);
            }
            surfaceTextures.push_back(textureIndex); if (textureIndex >= 0) ++result.texturedSpans;
        }
    }
    if (!texture.rgba.empty()) {
        D3D11_TEXTURE2D_DESC imageDescription{};
        imageDescription.Width = texture.width; imageDescription.Height = texture.height;
        imageDescription.MipLevels = 1; imageDescription.ArraySize = 1; imageDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        imageDescription.SampleDesc.Count = 1; imageDescription.Usage = D3D11_USAGE_IMMUTABLE;
        imageDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        D3D11_SUBRESOURCE_DATA pixels{}; pixels.pSysMem = texture.rgba.data(); pixels.SysMemPitch = texture.width * 4;
        ComPtr<ID3D11Texture2D> image;
        check(device->CreateTexture2D(&imageDescription, &pixels, &image), "native texture creation");
        check(device->CreateShaderResourceView(image.Get(), nullptr, &imageView), "native texture view");
    }
    if (!texture.rgba.empty() || !materialViews.empty()) {
        D3D11_SAMPLER_DESC samplerDescription{};
        samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDescription.AddressU = samplerDescription.AddressV = samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDescription.MaxLOD = 0; samplerDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
        check(device->CreateSamplerState(&samplerDescription, &sampler), "native sampler creation");
    }
    result.gpuOwnedBytes = result.cpuAssetBytes + sizeof(Camera) + std::size_t(imageWidth) * imageHeight * 4 * (options.interactive ? 4 : 3);
    result.initializationMs = elapsedMs(initializationStart);
    const auto loopStart = Clock48B::now();
    auto previousFrame = loopStart;
    float yaw = 0.72f, pitch = 0.62f, zoom = 1.0f;
    bool quit = false;
    result.submissionMs.reserve(options.frames);
    for (unsigned frame = 0; frame < options.frames && !quit; ++frame) {
        if (options.interactive) {
            MSG message{};
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (message.message == WM_QUIT) quit = true;
                TranslateMessage(&message); DispatchMessageW(&message);
            }
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) quit = true;
            if (quit) break;
            const float delta = std::min(0.1f, static_cast<float>(std::chrono::duration<double>(Clock48B::now() - previousFrame).count()));
            if (GetAsyncKeyState(VK_LEFT) & 0x8000) yaw -= delta;
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) yaw += delta;
            if (GetAsyncKeyState(VK_UP) & 0x8000) pitch = std::min(1.45f, pitch + delta);
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) pitch = std::max(-1.45f, pitch - delta);
            if (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) zoom = std::max(0.2f, zoom - delta);
            if (GetAsyncKeyState(VK_OEM_MINUS) & 0x8000) zoom = std::min(10.0f, zoom + delta);
        }
        previousFrame = Clock48B::now();
        const auto submissionStart = Clock48B::now();
        result.eye = {result.center[0] + std::cos(yaw) * std::cos(pitch) * distance * zoom,
            result.center[1] + std::sin(pitch) * distance * zoom,
            result.center[2] + std::sin(yaw) * std::cos(pitch) * distance * zoom};
        using namespace DirectX;
        const XMMATRIX view = XMMatrixLookAtRH(XMVectorSet(result.eye[0], result.eye[1], result.eye[2], 1),
            XMVectorSet(result.center[0], result.center[1], result.center[2], 1), XMVectorSet(0, 1, 0, 0));
        const XMMATRIX projection = XMMatrixPerspectiveFovRH(XM_PIDIV4, float(imageWidth) / imageHeight,
            std::max(radius * 0.005f, 0.00001f), distance * zoom + radius * 4.0f);
        Camera camera{};
        XMStoreFloat4x4(&camera.matrix, XMMatrixTranspose(view * projection));
        for (unsigned row = 0; row < 4; ++row)
            for (unsigned column = 0; column < 4; ++column)
                if (!std::isfinite(camera.matrix.m[row][column]))
                    throw std::runtime_error("mesh bounds produce an unrepresentable inspection camera");
        camera.textured = texture.rgba.empty() ? 0u : 1u;
        context->UpdateSubresource(constants.Get(), 0, nullptr, &camera, 0, 0);
        const float clearColor[] = {0.035f, 0.045f, 0.065f, 1};
        context->ClearRenderTargetView(targetView.Get(), clearColor);
        context->ClearDepthStencilView(depthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
        ID3D11RenderTargetView* targets[] = {targetView.Get()};
        context->OMSetRenderTargets(1, targets, depthView.Get());
        const D3D11_VIEWPORT viewport{0, 0, float(imageWidth), float(imageHeight), 0, 1};
        context->RSSetViewports(1, &viewport); context->RSSetState(raster.Get());
        context->IASetInputLayout(layout.Get());
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ID3D11Buffer* vertexBuffers[] = {vertices.Get()};
        const UINT stride = sizeof(rr::assets::Vertex48B), offset = 0;
        context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
        context->IASetIndexBuffer(indices.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->VSSetShader(vertexShader.Get(), nullptr, 0); context->PSSetShader(pixelShader.Get(), nullptr, 0);
        ID3D11Buffer* constantBuffers[] = {constants.Get()};
        context->VSSetConstantBuffers(0, 1, constantBuffers); context->PSSetConstantBuffers(0, 1, constantBuffers);
        ID3D11ShaderResourceView* views[] = {imageView.Get()};
        ID3D11SamplerState* samplers[] = {sampler.Get()};
        context->PSSetShaderResources(0, 1, views); context->PSSetSamplers(0, 1, samplers);
        if (options.semanticScene) for (std::size_t s = 0; s < drawSpans.size(); ++s) {
            const auto& span = drawSpans[s];
            if (!options.cache.empty()) {
                const int index = surfaceTextures[s];
                ID3D11ShaderResourceView* selectedViews[] = {index >= 0 ? materialViews.at(index).Get() : nullptr};
                context->PSSetShaderResources(0, 1, selectedViews);
                camera.textured = index >= 0 ? 1u : 0u;
                context->UpdateSubresource(constants.Get(), 0, nullptr, &camera, 0, 0);
            }
            context->DrawIndexed(span.index_count, span.index_start, 0);
        }
        else context->DrawIndexed(result.indices, 0, 0);
        ++result.submitted;
        result.submissionMs.push_back(elapsedMs(submissionStart));
        if (swapchain) {
            ComPtr<ID3D11Texture2D> back;
            check(swapchain->GetBuffer(0, IID_PPV_ARGS(&back)), "swapchain buffer query");
            context->CopyResource(back.Get(), target.Get());
            check(swapchain->Present(1, 0), "native presentation");
            ++result.presented;
        }
    }
    result.loopMs = elapsedMs(loopStart);
    if (result.submitted == 0) throw std::runtime_error("viewer closed before any frame was submitted");
    const auto readbackStart = Clock48B::now();
    D3D11_TEXTURE2D_DESC stagingDescription = targetDescription;
    stagingDescription.Usage = D3D11_USAGE_STAGING; stagingDescription.BindFlags = 0;
    stagingDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    ComPtr<ID3D11Texture2D> staging;
    check(device->CreateTexture2D(&stagingDescription, nullptr, &staging), "readback texture creation");
    context->CopyResource(staging.Get(), target.Get());
    D3D11_MAPPED_SUBRESOURCE mapped{};
    check(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped), "final frame readback");
    std::vector<std::uint8_t> rgba(std::size_t(imageWidth) * imageHeight * 4);
    for (UINT y = 0; y < imageHeight; ++y)
        std::memcpy(rgba.data() + std::size_t(y) * imageWidth * 4,
            static_cast<const std::uint8_t*>(mapped.pData) + std::size_t(y) * mapped.RowPitch, std::size_t(imageWidth) * 4);
    context->Unmap(staging.Get(), 0);
    result.readbackMs = elapsedMs(readbackStart); result.lastFrameReadback = true;
    if (!options.capture.empty()) { writePpm(options.capture, rgba); result.captureWritten = true; }
    result.modules = loadedModules();
    result.success = true;
}
}
int main(int argc, char** argv) {
    int importExit = 0;
    if (rr::windows::importEntry48C(argc, argv, importExit)) return importExit;
    Options options;
    Result result;
    try {
        options = parseOptions(argc, argv);
        inspect(options, result);
        reportResult(options, result);
        std::cout << "Native asset inspection: " << result.vertices << " vertices, " << result.indices / 3
            << " triangles; " << result.driver << "; " << result.submitted << " frames submitted. Not gameplay.\n";
        return 0;
    } catch (const std::exception& error) {
        result.success = false; result.error = error.what();
        std::cerr << "Native inspection failed: " << result.error << "\n";
        try { if (result.modules.empty()) result.modules = loadedModules(); reportResult(options, result); }
        catch (const std::exception& reportingError) { std::cerr << "Report failed: " << reportingError.what() << "\n"; }
        return 1;
    }
}
