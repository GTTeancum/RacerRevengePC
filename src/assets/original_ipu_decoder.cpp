#include "original_ipu_decoder.h"

#include <algorithm>
#include <cstring>
#include <mutex>
#include <new>
#include <utility>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#endif
#if defined(__linux__) || defined(_WIN32)
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/cpu.h"
}
#endif

namespace rr::assets {
namespace {
constexpr std::size_t kMaxEncodedBytes = 8u * 1024u * 1024u;

std::uint16_t little16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0] | (std::uint16_t(p[1]) << 8));
}
std::uint32_t little32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
           (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}
bool fail(std::string& diagnostic, const char* reason) {
    diagnostic = reason;
    return false;
}
bool validDimension(std::uint32_t n) noexcept {
    return n >= 16 && n <= 1024 && (n & (n - 1)) == 0;
}

#if defined(__linux__) || defined(_WIN32)
#if defined(_WIN32)
using NativeLibrary = HMODULE;
#else
using NativeLibrary = void*;
#endif

// Public API declarations and layouts come from unmodified n7.1.5 headers.
// Only the matching native ABI is loaded. No private FFmpeg structs or guessed
// offsets, hand-declared enums, generated pixel blobs or demuxer are needed.
struct FfmpegApi final {
    NativeLibrary util = nullptr;
    NativeLibrary codec = nullptr;
    std::string error;

#define RR_AV_FUNCTION(name) decltype(&::name) name = nullptr
    RR_AV_FUNCTION(avutil_version);
    RR_AV_FUNCTION(avcodec_version);
    RR_AV_FUNCTION(avcodec_find_decoder_by_name);
    RR_AV_FUNCTION(avcodec_alloc_context3);
    RR_AV_FUNCTION(avcodec_free_context);
    RR_AV_FUNCTION(avcodec_open2);
    RR_AV_FUNCTION(avcodec_send_packet);
    RR_AV_FUNCTION(avcodec_receive_frame);
    RR_AV_FUNCTION(av_packet_alloc);
    RR_AV_FUNCTION(av_new_packet);
    RR_AV_FUNCTION(av_packet_free);
    RR_AV_FUNCTION(av_frame_alloc);
    RR_AV_FUNCTION(av_frame_free);
    RR_AV_FUNCTION(av_frame_unref);
    RR_AV_FUNCTION(av_get_cpu_flags);
    RR_AV_FUNCTION(av_force_cpu_flags);
    RR_AV_FUNCTION(av_strerror);
#undef RR_AV_FUNCTION

    template<class Function>
    bool symbol(NativeLibrary library, const char* name, Function& function) {
#if defined(_WIN32)
        FARPROC p = GetProcAddress(library, name);
#else
        void* p = dlsym(library, name);
#endif
        // Use the platform loader's function-pointer representation with the
        // unmodified official declaration's exact signature/calling convention.
        static_assert(sizeof(function) == sizeof(p), "Native function pointer size");
        if (!p) {
            error = std::string("IPU decoder: missing FFmpeg symbol ") + name;
            return false;
        }
        std::memcpy(&function, &p, sizeof(function));
        return true;
    }

#if defined(_WIN32)
    bool executableDirectory(std::wstring& directory) {
        // LOAD_LIBRARY_SEARCH_* needs Windows 8+ or the corresponding update
        // on Windows 7. Fail closed if those safe search flags are unavailable;
        // never fall back to the current directory, PATH, or SetDllDirectory.
        HMODULE kernel = GetModuleHandleW(L"kernel32.dll");
        if (!kernel || !GetProcAddress(kernel, "AddDllDirectory")) {
            error = "IPU decoder requires Windows safe DLL search support";
            return false;
        }
        std::vector<wchar_t> path(32768);
        const DWORD length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
        if (!length || length >= path.size()) {
            error = "IPU decoder could not resolve the executable directory";
            return false;
        }
        directory.assign(path.data(), length);
        const auto separator = directory.find_last_of(L"\\/");
        if (separator == std::wstring::npos) {
            error = "IPU decoder requires an absolute executable directory";
            return false;
        }
        directory.resize(separator + 1);
        return true;
    }

    NativeLibrary loadWindowsLibrary(const std::wstring& directory,
                                     const wchar_t* filename, const char* label) {
        const std::wstring path = directory + filename;
        // Documented flags: LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR (0x100) and
        // LOAD_LIBRARY_SEARCH_SYSTEM32 (0x800). Constants keep older SDK headers
        // buildable; executableDirectory verifies OS support before this call.
        constexpr DWORD searchDllDirectory = 0x00000100;
        constexpr DWORD searchSystem32 = 0x00000800;
        NativeLibrary library = LoadLibraryExW(path.c_str(), nullptr,
                                              searchDllDirectory | searchSystem32);
        if (!library) {
            const DWORD code = GetLastError();
            error = std::string("IPU decoder requires matching ") + label +
                    " beside the executable (FFmpeg 7.1.5; Windows loader error " +
                    std::to_string(code) + ")";
        }
        return library;
    }
#endif

    FfmpegApi() {
#if defined(_WIN32)
        std::wstring directory;
        if (!executableDirectory(directory)) return;
        util = loadWindowsLibrary(directory, L"avutil-59.dll", "avutil-59.dll");
        if (!util) return;
        codec = loadWindowsLibrary(directory, L"avcodec-61.dll", "avcodec-61.dll");
        if (!codec) return;
#else
        util = dlopen("libavutil.so.59", RTLD_NOW | RTLD_LOCAL);
        if (!util) {
            error = "IPU decoder requires native libavutil.so.59 (FFmpeg 7.1.5)";
            return;
        }
        codec = dlopen("libavcodec.so.61", RTLD_NOW | RTLD_LOCAL);
        if (!codec) {
            error = "IPU decoder requires native libavcodec.so.61 (FFmpeg 7.1.5)";
            return;
        }
#endif
#define RR_LOAD(library, name) if (!symbol(library, #name, name)) return
        RR_LOAD(util, avutil_version);
        RR_LOAD(codec, avcodec_version);
        if (avutil_version() != LIBAVUTIL_VERSION_INT ||
            avcodec_version() != LIBAVCODEC_VERSION_INT) {
            error = "IPU decoder requires FFmpeg ABI versions libavcodec 61.19.101 and libavutil 59.39.100";
            return;
        }
        RR_LOAD(codec, avcodec_find_decoder_by_name);
        RR_LOAD(codec, avcodec_alloc_context3);
        RR_LOAD(codec, avcodec_free_context);
        RR_LOAD(codec, avcodec_open2);
        RR_LOAD(codec, avcodec_send_packet);
        RR_LOAD(codec, avcodec_receive_frame);
        RR_LOAD(codec, av_packet_alloc);
        RR_LOAD(codec, av_new_packet);
        RR_LOAD(codec, av_packet_free);
        RR_LOAD(util, av_frame_alloc);
        RR_LOAD(util, av_frame_free);
        RR_LOAD(util, av_frame_unref);
        RR_LOAD(util, av_get_cpu_flags);
        RR_LOAD(util, av_force_cpu_flags);
        RR_LOAD(util, av_strerror);
#undef RR_LOAD
    }

    ~FfmpegApi() {
#if defined(_WIN32)
        if (codec) FreeLibrary(codec);
        if (util) FreeLibrary(util);
#else
        if (codec) dlclose(codec);
        if (util) dlclose(util);
#endif
    }
    FfmpegApi(const FfmpegApi&) = delete;
    FfmpegApi& operator=(const FfmpegApi&) = delete;
};

struct DecodeObjects final {
    const FfmpegApi& api;
    AVCodecContext* context = nullptr;
    AVPacket* packet = nullptr;
    AVFrame* frame = nullptr;
    explicit DecodeObjects(const FfmpegApi& a) : api(a) {}
    ~DecodeObjects() {
        if (frame) api.av_frame_free(&frame);
        if (packet) api.av_packet_free(&packet);
        if (context) api.avcodec_free_context(&context);
    }
};

struct ScalarCpuScope final {
    const FfmpegApi& api;
    int savedFlags;
    explicit ScalarCpuScope(const FfmpegApi& a)
        : api(a), savedFlags(a.av_get_cpu_flags()) { api.av_force_cpu_flags(0); }
    ~ScalarCpuScope() { api.av_force_cpu_flags(savedFlags); }
};

bool decoderError(const FfmpegApi& api, std::string& diagnostic,
                  const char* stage, int code) {
    char message[AV_ERROR_MAX_STRING_SIZE]{};
    api.av_strerror(code, message, sizeof(message));
    diagnostic = std::string("IPU decoder ") + stage + ": " + message;
    return false;
}

// Python // rounds negative quotients down; C++ / truncates toward zero.
// Every value here is bounded to small color-channel integers, so negation and
// addition cannot overflow. Preserve each separate floor in the reference CSC.
int floorDivide(int n, int d) noexcept {
    return n >= 0 ? n / d : -((-n + d - 1) / d);
}
int clip255(int n) noexcept { return std::max(0, std::min(255, n)); }

void convertRgba(const AVFrame& frame, std::uint32_t width,
                 std::uint32_t height, std::vector<std::uint8_t>& rgba) {
    static constexpr int dither[4][4] = {
        {-4, 0, -3, 1}, {2, -2, 3, -1}, {-3, 1, -4, 0}, {3, -1, 2, -2}
    };
    rgba.resize(std::size_t(width) * height * 4);
    for (std::uint32_t y = 0; y < height; ++y) {
        const auto* yRow = frame.data[0] + std::size_t(y) * frame.linesize[0];
        const auto* uRow = frame.data[1] + std::size_t(y / 2) * frame.linesize[1];
        const auto* vRow = frame.data[2] + std::size_t(y / 2) * frame.linesize[2];
        for (std::uint32_t x = 0; x < width; ++x) {
            const int u = int(uRow[x / 2]) - 128;
            const int v = int(vRow[x / 2]) - 128;
            const int yy = (149 * std::max(0, int(yRow[x]) - 16)) / 64;
            const int channels[3] = {
                floorDivide(yy + floorDivide(204 * v, 64) + 1, 2),
                floorDivide(yy + floorDivide(-104 * v, 64) + floorDivide(-50 * u, 64) + 1, 2),
                floorDivide(yy + floorDivide(258 * u, 64) + 1, 2)
            };
            auto* out = rgba.data() + (std::size_t(y) * width + x) * 4;
            for (int channel = 0; channel < 3; ++channel)
                out[channel] = static_cast<std::uint8_t>(
                    (clip255(clip255(channels[channel]) + dither[y % 4][x % 4]) / 8) * 8);
            out[3] = (out[0] || out[1] || out[2]) ? 255 : 0;
        }
    }
}

bool decodeFrame(const std::uint8_t* ipum, std::size_t bytes,
                 std::uint32_t width, std::uint32_t height,
                 std::vector<std::uint8_t>& rgba, std::string& diagnostic) {
    static FfmpegApi api;
    static std::mutex decodeMutex;
    if (!api.error.empty()) { diagnostic = api.error; return false; }
    std::lock_guard<std::mutex> lock(decodeMutex);
    ScalarCpuScope scalar(api);
    const AVCodec* decoder = api.avcodec_find_decoder_by_name("ipu");
    if (!decoder || decoder->id != AV_CODEC_ID_IPU)
        return fail(diagnostic, "IPU codec is not enabled in the native FFmpeg library");
    DecodeObjects objects(api);
    objects.context = api.avcodec_alloc_context3(decoder);
    objects.packet = api.av_packet_alloc();
    objects.frame = api.av_frame_alloc();
    if (!objects.context || !objects.packet || !objects.frame)
        return fail(diagnostic, "IPU decoder allocation failed");
    auto* context = objects.context;
    context->width = static_cast<int>(width);
    context->height = static_cast<int>(height);
    // FFmpeg checks its internally aligned allocation dimensions against this
    // limit, not only the visible extent. Small 16/32-wide images can allocate
    // 64-wide rows. Preserve a bounded allowance for that padding; exact visible
    // dimensions are checked again on the decoded frame below.
    const std::uint32_t allocationWidth = (width + 63u) & ~63u;
    const std::uint32_t allocationHeight = (height + 31u) & ~31u;
    context->max_pixels = std::int64_t(allocationWidth) * allocationHeight;
    context->flags |= AV_CODEC_FLAG_BITEXACT;
    context->idct_algo = FF_IDCT_SIMPLE;
    context->thread_count = 1;
    context->thread_type = 0;
    context->err_recognition = AV_EF_EXPLODE;
    int result = api.avcodec_open2(context, decoder, nullptr);
    if (result < 0) return decoderError(api, diagnostic, "open", result);

    // Official IPU demuxer skips 16 bytes. Its parser ends the first packet
    // immediately after B0. B1 belongs only to container framing. A complete
    // original image is already authenticated/bounded, so no libavformat or
    // parser is required. av_new_packet supplies zeroed input padding.
    const int packetBytes = static_cast<int>(bytes - 16 - 4);
    result = api.av_new_packet(objects.packet, packetBytes);
    if (result < 0) return decoderError(api, diagnostic, "packet allocation", result);
    std::memcpy(objects.packet->data, ipum + 16, std::size_t(packetBytes));
    result = api.avcodec_send_packet(context, objects.packet);
    if (result < 0) return decoderError(api, diagnostic, "send", result);
    result = api.avcodec_receive_frame(context, objects.frame);
    if (result < 0) return decoderError(api, diagnostic, "receive", result);
    const AVFrame& frame = *objects.frame;
    if (frame.width != static_cast<int>(width) || frame.height != static_cast<int>(height) ||
        frame.format != AV_PIX_FMT_YUV420P || frame.decode_error_flags != 0 ||
        (frame.flags & AV_FRAME_FLAG_CORRUPT))
        return fail(diagnostic, "IPU decoded frame dimensions, format or integrity mismatch");
    for (int plane = 0; plane < 3; ++plane) {
        const int planeWidth = static_cast<int>(plane == 0 ? width : width / 2);
        if (!frame.data[plane] || frame.linesize[plane] < planeWidth || frame.linesize[plane] > 32768)
            return fail(diagnostic, "IPU decoded plane extent mismatch");
    }
    std::vector<std::uint8_t> pixels;
    convertRgba(frame, width, height, pixels);
    api.av_frame_unref(objects.frame);
    result = api.avcodec_receive_frame(context, objects.frame);
    if (result != AVERROR(EAGAIN))
        return fail(diagnostic, "IPU decoder returned an unexpected extra frame or state");
    result = api.avcodec_send_packet(context, nullptr);
    if (result < 0) return decoderError(api, diagnostic, "drain", result);
    result = api.avcodec_receive_frame(context, objects.frame);
    if (result != AVERROR_EOF)
        return fail(diagnostic, "IPU decoder returned unexpected delayed output");
    rgba.swap(pixels);
    return true;
}
#endif
} // namespace

bool decodeOriginalIpuRgba(const std::uint8_t* ipum, std::size_t ipumBytes,
                           std::uint32_t width, std::uint32_t height,
                           std::vector<std::uint8_t>& rgba,
                           std::string& diagnostic) {
    rgba.clear();
    diagnostic.clear();
    if (!validDimension(width) || !validDimension(height))
        return fail(diagnostic, "IPU dimensions must be powers of two in [16, 1024]");
    if (!ipum || ipumBytes < 25 || ipumBytes > kMaxEncodedBytes)
        return fail(diagnostic, "IPU encoded extent is outside the supported bound");
    if (std::memcmp(ipum, "ipum", 4) != 0 || little32(ipum + 4) != ipumBytes - 8 ||
        little16(ipum + 8) != width || little16(ipum + 10) != height ||
        little32(ipum + 12) != 1 || ipum[16] != 0)
        return fail(diagnostic, "IPU header dimensions, frame count or flags mismatch");
    static constexpr std::uint8_t terminators[8] = {0, 0, 1, 0xb0, 0, 0, 1, 0xb1};
    if (std::memcmp(ipum + ipumBytes - 8, terminators, 8) != 0)
        return fail(diagnostic, "IPU first-frame or sequence terminator mismatch");
    for (std::size_t i = 16; i + 4 <= ipumBytes - 8; ++i)
        if (std::memcmp(ipum + i, terminators, 4) == 0)
            return fail(diagnostic, "IPU contains an unexpected earlier frame terminator");
#if defined(__linux__) || defined(_WIN32)
    try {
        return decodeFrame(ipum, ipumBytes, width, height, rgba, diagnostic);
    } catch (const std::bad_alloc&) {
        rgba.clear();
        return fail(diagnostic, "IPU decoder allocation failed");
    }
#else
    return fail(diagnostic, "In-process IPU decoding requires the Linux or Windows native FFmpeg library adapter");
#endif
}
} // namespace rr::assets
