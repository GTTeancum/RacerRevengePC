#include "audio_output50hd.h"
#include "native_dynamic_library50win.h"
#include <algorithm>
#include <atomic>
#include <bit>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <limits>
#include <new>
#if defined(_WIN32)
#include <SDL_audio.h>
#include <io.h>
#include <share.h>
#include <sys/stat.h>
#else
#include <unistd.h>
#endif
namespace rr::platform {
namespace {
constexpr std::uint32_t sdlAudioInit = 0x10;
constexpr std::uint16_t sdlS16LittleEndian = 0x8010;
constexpr std::size_t queueFrames = 32768, blockFrames = 1024;
constexpr std::uint64_t maximumWavFrames = (std::uint64_t{0xffffffff} - 36) / 4;
// Use the official SDL2 audio ABI on Windows. Linux retains its existing
// SDK-independent declaration. The shared loader rejects non-x64 Windows.
#if defined(_WIN32)
using SdlAudioSpec = SDL_AudioSpec;
#else
struct SdlAudioSpec {
    int freq;
    std::uint16_t format;
    std::uint8_t channels, silence;
    std::uint16_t samples, padding;
    std::uint32_t size;
    void (RR_SDL50WIN_CALL *callback)(void*, std::uint8_t*, int);
    void* userdata;
};
#endif
static_assert(sizeof(int) == 4 && offsetof(SdlAudioSpec, format) == 4);
static_assert(offsetof(SdlAudioSpec, channels) == 6 && offsetof(SdlAudioSpec, silence) == 7);
static_assert(offsetof(SdlAudioSpec, samples) == 8 && offsetof(SdlAudioSpec, padding) == 10);
static_assert(offsetof(SdlAudioSpec, size) == 12 && offsetof(SdlAudioSpec, callback) == 16);
static_assert(offsetof(SdlAudioSpec, userdata) == 16 + sizeof(void*));
static_assert(sizeof(SdlAudioSpec) == (sizeof(void*) == 8 ? 32 : 24));
static_assert(std::endian::native == std::endian::little,
              "PCM capture and SDL must share the same little-endian bytes");
static_assert(std::atomic<std::uint64_t>::is_always_lock_free,
              "The real-time callback requires lock-free counters");
template<std::size_t N> void copyText(std::array<char, N>& output, const char* text) noexcept {
    output.fill(0);
    if (!text) return;
    for (std::size_t i = 0; i + 1 < N && text[i]; ++i) output[i] = text[i];
}
bool fail(AudioOutputDiagnostic50HD& d, AudioOutputError50HD error,
          const char* operation, const char* text = nullptr) noexcept {
    d.error = error; d.operation = operation; copyText(d.detail, text); return false;
}
void little16(std::uint8_t* out, std::uint16_t value) noexcept {
    out[0] = static_cast<std::uint8_t>(value); out[1] = static_cast<std::uint8_t>(value >> 8);
}
void little32(std::uint8_t* out, std::uint32_t value) noexcept {
    for (unsigned i = 0; i < 4; ++i) out[i] = static_cast<std::uint8_t>(value >> (i * 8));
}
std::array<std::uint8_t, 44> wavHeader(std::uint32_t rate, std::uint64_t frames) noexcept {
    std::array<std::uint8_t, 44> header{};
    const auto bytes = static_cast<std::uint32_t>(frames * 4);
    std::memcpy(header.data(), "RIFF", 4); little32(header.data() + 4, 36 + bytes);
    std::memcpy(header.data() + 8, "WAVEfmt ", 8); little32(header.data() + 16, 16);
    little16(header.data() + 20, 1); little16(header.data() + 22, 2);
    little32(header.data() + 24, rate); little32(header.data() + 28, rate * 4);
    little16(header.data() + 32, 4); little16(header.data() + 34, 16);
    std::memcpy(header.data() + 36, "data", 4); little32(header.data() + 40, bytes);
    return header;
}
// Capture descriptors never enter the SDL callback. Keep exact byte writes and
// exclusive creation on both hosts; Windows CRT paths use its native narrow
// filename encoding, just like the other char-path file APIs in this host.
int openCapture(const char* path) noexcept {
#if defined(_WIN32)
    int fd = -1;
    const auto error = ::_sopen_s(&fd, path,
        _O_WRONLY | _O_CREAT | _O_EXCL | _O_BINARY | _O_NOINHERIT,
        _SH_DENYWR, _S_IREAD | _S_IWRITE);
    if (error) { errno = error; return -1; }
    return fd;
#else
    return ::open(path, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
#endif
}
bool seekCaptureStart(int fd) noexcept {
#if defined(_WIN32)
    return ::_lseeki64(fd, 0, SEEK_SET) >= 0;
#else
    return ::lseek(fd, 0, SEEK_SET) >= 0;
#endif
}
bool truncateCapture(int fd, std::uint64_t size) noexcept {
#if defined(_WIN32)
    // The RIFF bound is below 2^32, but the CRT long is only 32 bits on x64.
    const auto error = ::_chsize_s(fd, static_cast<__int64>(size));
    if (error) { errno = error; return false; }
    return true;
#else
    return ::ftruncate(fd, static_cast<off_t>(size)) == 0;
#endif
}
int closeCapture(int fd) noexcept {
#if defined(_WIN32)
    return ::_close(fd);
#else
    return ::close(fd);
#endif
}
bool writeAll(int fd, const void* bytes, std::size_t size) noexcept {
    const auto* source = static_cast<const std::uint8_t*>(bytes);
    while (size) {
#if defined(_WIN32)
        // _write accepts unsigned int but returns signed int. Bound each call
        // before narrowing, even if a future caller submits a larger buffer.
        const auto chunk = static_cast<unsigned int>(std::min<std::size_t>(
            size, static_cast<std::size_t>((std::numeric_limits<int>::max)())));
        const auto written = ::_write(fd, source, chunk);
#else
        const auto written = ::write(fd, source, size);
#endif
        if (written < 0 && errno == EINTR) continue;
        if (written == 0) { errno = EIO; return false; }
        if (written < 0) return false;
        source += written; size -= static_cast<std::size_t>(written);
    }
    return true;
}
}
const char* audioDeviceStatusName50HD(AudioDeviceStatus50HD status) noexcept {
    switch (status) {
        case AudioDeviceStatus50HD::NotRequested: return "not_requested";
        case AudioDeviceStatus50HD::Ready: return "ready";
        case AudioDeviceStatus50HD::Unavailable: return "unavailable";
        case AudioDeviceStatus50HD::RejectedNonPlayback: return "rejected_non_playback";
    }
    return "unknown";
}
struct AudioOutput50HD::State {
    AudioOutputInfo50HD info{};
    AudioSampleClock50HD clock{};
    native_library50win::Handle library = nullptr;
    std::uint32_t device = 0;
    int capture = -1;
    bool subsystemOwned = false, started = false, failed = false;
    std::uint16_t prebuffer = 1536;
    std::array<std::int16_t, queueFrames * 2> ring{};
    std::array<std::int16_t, blockFrames * 2> mixed{};
    std::atomic<std::uint64_t> read{0}, write{0}, consumed{0}, underruns{0};
    int (RR_SDL50WIN_CALL *initSubsystem)(std::uint32_t) = nullptr;
    void (RR_SDL50WIN_CALL *quitSubsystem)(std::uint32_t) = nullptr;
    const char* (RR_SDL50WIN_CALL *getError)() = nullptr;
    const char* (RR_SDL50WIN_CALL *getDriver)() = nullptr;
    std::uint32_t (RR_SDL50WIN_CALL *openDevice)(const char*, int, const SdlAudioSpec*, SdlAudioSpec*, int) = nullptr;
    void (RR_SDL50WIN_CALL *closeDevice)(std::uint32_t) = nullptr;
    void (RR_SDL50WIN_CALL *pauseDevice)(std::uint32_t, int) = nullptr;
    void (RR_SDL50WIN_CALL *lockDevice)(std::uint32_t) = nullptr;
    void (RR_SDL50WIN_CALL *unlockDevice)(std::uint32_t) = nullptr;
    int (RR_SDL50WIN_CALL *deviceStatus)(std::uint32_t) = nullptr;

    static void RR_SDL50WIN_CALL callback(void* opaque, std::uint8_t* output, int bytes) noexcept {
        if (!opaque || !output || bytes <= 0) return;
        auto& s = *static_cast<State*>(opaque);
        std::memset(output, 0, static_cast<std::size_t>(bytes));
        const auto requested = static_cast<std::uint64_t>(bytes) / 4;
        const auto reader = s.read.load(std::memory_order_relaxed);
        const auto writer = s.write.load(std::memory_order_acquire);
        const auto frames = std::min(requested, writer - reader);
        const auto offset = static_cast<std::size_t>(reader % queueFrames);
        const auto first = static_cast<std::size_t>(std::min(frames, queueFrames - offset));
        if (first) std::memcpy(output, s.ring.data() + offset * 2, first * 4);
        if (frames > first)
            std::memcpy(output + first * 4, s.ring.data(), static_cast<std::size_t>(frames - first) * 4);
        s.read.store(reader + frames, std::memory_order_release);
        s.consumed.fetch_add(frames, std::memory_order_relaxed);
        s.underruns.fetch_add(requested - frames, std::memory_order_relaxed);
    }
    void releaseDevice() noexcept {
        // SDL_CloseAudioDevice joins its callback before storage/library go away.
        if (device && closeDevice) closeDevice(device);
        device = 0; started = false;
        if (subsystemOwned && quitSubsystem) quitSubsystem(sdlAudioInit);
        subsystemOwned = false;
        if (library) {
            char detail[256]{};
            native_library50win::close(library, detail);
        }
        library = nullptr;
    }
    void deviceFailure(const char* detail,
                       AudioDeviceStatus50HD status = AudioDeviceStatus50HD::Unavailable) noexcept {
        info.deviceStatus = status; copyText(info.deviceDetail, detail); releaseDevice();
    }
    void prepareDevice(const AudioOutputRequest50HD& request) noexcept {
        info.deviceStatus = AudioDeviceStatus50HD::Unavailable;
        char loaderDetail[256]{};
        library = native_library50win::openSdl2(loaderDetail);
        if (!library) { deviceFailure(loaderDetail); return; }
#define RR_AUDIO50HD_LOAD(member, entryName) \
        member = reinterpret_cast<decltype(member)>(native_library50win::symbol(library, entryName, loaderDetail)); \
        if (!member) { deviceFailure(loaderDetail); return; }
#if defined(_WIN32)
        // This host owns main rather than linking SDL2main's entry-point shim.
        void (RR_SDL50WIN_CALL *setMainReady)() = nullptr;
        RR_AUDIO50HD_LOAD(setMainReady, "SDL_SetMainReady");
#endif
        RR_AUDIO50HD_LOAD(initSubsystem, "SDL_InitSubSystem");
        RR_AUDIO50HD_LOAD(quitSubsystem, "SDL_QuitSubSystem");
        RR_AUDIO50HD_LOAD(getError, "SDL_GetError");
        RR_AUDIO50HD_LOAD(getDriver, "SDL_GetCurrentAudioDriver");
        RR_AUDIO50HD_LOAD(openDevice, "SDL_OpenAudioDevice");
        RR_AUDIO50HD_LOAD(closeDevice, "SDL_CloseAudioDevice");
        RR_AUDIO50HD_LOAD(pauseDevice, "SDL_PauseAudioDevice");
        RR_AUDIO50HD_LOAD(lockDevice, "SDL_LockAudioDevice");
        RR_AUDIO50HD_LOAD(unlockDevice, "SDL_UnlockAudioDevice");
        RR_AUDIO50HD_LOAD(deviceStatus, "SDL_GetAudioDeviceStatus");
#undef RR_AUDIO50HD_LOAD
#if defined(_WIN32)
        setMainReady();
#endif
        if (initSubsystem(sdlAudioInit) != 0) { deviceFailure(getError()); return; }
        subsystemOwned = true;
        const char* driver = getDriver();
        copyText(info.driver, driver);
        if (!driver || !std::strcmp(driver, "dummy") || !std::strcmp(driver, "disk")) {
            deviceFailure("SDL dummy/disk backend is not genuine device playback",
                          AudioDeviceStatus50HD::RejectedNonPlayback); return;
        }
        SdlAudioSpec desired{};
        desired.freq = static_cast<int>(request.sampleRate);
        desired.format = sdlS16LittleEndian; desired.channels = 2;
        desired.samples = request.callbackFrames;
        desired.callback = callback; desired.userdata = this;
        SdlAudioSpec obtained{};
        device = openDevice(nullptr, 0, &desired, &obtained, 0);
        if (!device) { deviceFailure(getError()); return; }
        if (obtained.freq != desired.freq || obtained.format != desired.format || obtained.channels != 2) {
            deviceFailure("SDL did not preserve requested PCM16 stereo sample format"); return;
        }
        info.deviceStatus = AudioDeviceStatus50HD::Ready;
        copyText(info.deviceDetail, "SDL playback device opened; output audibility is not independently verified");
    }
    void submit(std::size_t frames, AudioOutputDiagnostic50HD& d) noexcept {
        if (!device || info.paused) return;
        if (started && deviceStatus(device) == 0) {
            deviceFailure("SDL playback device stopped or disconnected; WAV capture continues"); return;
        }
        const auto writer = write.load(std::memory_order_relaxed);
        const auto reader = read.load(std::memory_order_acquire);
        if (frames > queueFrames - (writer - reader)) {
            info.deviceOverflowFrames += frames; d.droppedDeviceFrames += frames; return;
        }
        const auto offset = static_cast<std::size_t>(writer % queueFrames);
        const auto first = std::min(frames, queueFrames - offset);
        std::memcpy(ring.data() + offset * 2, mixed.data(), first * 4);
        if (frames > first) std::memcpy(ring.data(), mixed.data() + first * 2, (frames - first) * 4);
        write.store(writer + frames, std::memory_order_release);
        if (!started && writer + frames - reader >= prebuffer) {
            started = true; pauseDevice(device, 0);
        }
    }
};
AudioOutput50HD::~AudioOutput50HD() noexcept {
    AudioOutputDiagnostic50HD d; close(d);
}
bool AudioOutput50HD::open(const AudioOutputRequest50HD& request,
                         AudioOutputDiagnostic50HD& d) noexcept {
    d = {};
    if (state_) return fail(d, AudioOutputError50HD::AlreadyOpen, "audio.open");
    if (request.sampleRate < 8000 || request.sampleRate > 192000 ||
        request.callbackFrames < 128 || request.callbackFrames > 8192 ||
        (request.callbackFrames & (request.callbackFrames - 1)) ||
        !request.prebufferFrames || request.prebufferFrames > queueFrames ||
        (request.wavPath && !request.wavPath[0]))
        return fail(d, AudioOutputError50HD::InvalidRequest, "audio.open/request");
    state_ = new (std::nothrow) State;
    if (!state_) return fail(d, AudioOutputError50HD::Allocation, "audio.open/storage");
    auto& s = *state_;
    s.info.sampleRate = request.sampleRate; s.prebuffer = request.prebufferFrames;
    s.clock.initialize(request.sampleRate);
    if (request.wavPath) {
        s.capture = openCapture(request.wavPath);
        if (s.capture < 0) {
            fail(d, AudioOutputError50HD::CaptureOpen, "audio.open/WAV", std::strerror(errno));
            delete state_; state_ = nullptr; return false;
        }
        const auto header = wavHeader(request.sampleRate, 0);
        if (!writeAll(s.capture, header.data(), header.size())) {
            fail(d, AudioOutputError50HD::CaptureWrite, "audio.open/WAV header", std::strerror(errno));
            closeCapture(s.capture); delete state_; state_ = nullptr; return false;
        }
        s.info.captureActive = true;
    }
    if (request.tryDevice) s.prepareDevice(request);
    return true;
}
bool AudioOutput50HD::advanceMicroseconds(AudioMixer50HD& mixer, std::uint32_t microseconds,
                                        AudioOutputDiagnostic50HD& d) noexcept {
    // Preserve the ordinary gameplay API and its exact mixer admission. This
    // stack adapter is never retained; both overloads use one PCM output path.
    class MixerProducer final : public AudioStereoProducer50HD {
    public:
        explicit MixerProducer(AudioMixer50HD& value) noexcept : mixer_(value) {}
        bool ready() const noexcept override { return mixer_.count()!=0; }
        std::uint32_t sampleRate() const noexcept override { return mixer_.sampleRate(); }
        bool mixStereo16(std::int16_t* output,std::size_t frames) noexcept override {
            return mixer_.mixStereo16(output,frames);
        }
    private:
        AudioMixer50HD& mixer_;
    } producer(mixer);
    return advanceMicroseconds(producer,microseconds,d);
}
bool AudioOutput50HD::advanceMicroseconds(AudioStereoProducer50HD& producer, std::uint32_t microseconds,
                                        AudioOutputDiagnostic50HD& d) noexcept {
    d = {};
    if (!state_ || state_->failed) return fail(d, AudioOutputError50HD::NotOpen, "audio.advance");
    auto& s = *state_;
    if (!producer.ready() || producer.sampleRate() != s.info.sampleRate)
        return fail(d, AudioOutputError50HD::Mixer, "audio.advance/sample-rate mismatch");
    std::uint32_t remaining = 0;
    if (!s.clock.advance(microseconds, remaining))
        return fail(d, AudioOutputError50HD::Clock, "audio.advance/clock");
    s.info.simulationMicroseconds = s.clock.microseconds();
    if (s.capture >= 0 && remaining > maximumWavFrames - s.info.capturedFrames) {
        s.failed = true;
        return fail(d, AudioOutputError50HD::CaptureLimit, "audio.advance/RIFF 4 GiB limit");
    }
    while (remaining) {
        const auto frames = std::min<std::uint32_t>(remaining, blockFrames);
        if (!producer.mixStereo16(s.mixed.data(), frames)) {
            s.failed = true; return fail(d, AudioOutputError50HD::Mixer, "audio.advance/mix");
        }
        s.info.mixedFrames += frames;
        if (s.capture >= 0) {
            if (!writeAll(s.capture, s.mixed.data(), frames * 4)) {
                s.failed = true;
                return fail(d, AudioOutputError50HD::CaptureWrite, "audio.advance/WAV payload", std::strerror(errno));
            }
            s.info.capturedFrames += frames;
        }
        s.submit(frames, d);
        remaining -= frames;
    }
    return true;
}
void AudioOutput50HD::pauseDevice(bool paused) noexcept {
    if (!state_ || state_->info.paused == paused) return;
    auto& s = *state_;
    s.info.paused = paused;
    if (!s.device) return;
    s.pauseDevice(s.device, 1);
    s.lockDevice(s.device);
    s.read.store(0, std::memory_order_relaxed);
    s.write.store(0, std::memory_order_relaxed);
    s.unlockDevice(s.device);
    s.started = false;
    // Resume is delayed until fresh mixed data satisfies the prebuffer again.
}
AudioOutputInfo50HD AudioOutput50HD::info() const noexcept {
    if (!state_) return finalInfo_;
    auto result = state_->info;
    // Reader never overtakes writer. Load reader first for a coherent bound.
    const auto reader = state_->read.load(std::memory_order_acquire);
    const auto writer = state_->write.load(std::memory_order_acquire);
    result.deviceQueuedFrames = writer - reader;
    result.deviceConsumedFrames = state_->consumed.load(std::memory_order_relaxed);
    result.deviceUnderrunFrames = state_->underruns.load(std::memory_order_relaxed);
    return result;
}
bool AudioOutput50HD::close(AudioOutputDiagnostic50HD& d) noexcept {
    d = {};
    if (!state_) return true;
    auto& s = *state_;
    s.releaseDevice();
    finalInfo_ = info();
    bool ok = true;
    if (s.capture >= 0) {
        const auto header = wavHeader(s.info.sampleRate, s.info.capturedFrames);
        if (!seekCaptureStart(s.capture) || !writeAll(s.capture, header.data(), header.size()))
            ok = fail(d, AudioOutputError50HD::CaptureFinalize, "audio.close/WAV header", std::strerror(errno));
        // A failed partial write is excluded from RIFF's advertised data range.
        if (!truncateCapture(s.capture, 44 + s.info.capturedFrames * 4) && ok)
            ok = fail(d, AudioOutputError50HD::CaptureFinalize, "audio.close/WAV length", std::strerror(errno));
        if (closeCapture(s.capture) != 0 && ok)
            ok = fail(d, AudioOutputError50HD::CaptureFinalize, "audio.close/WAV file", std::strerror(errno));
    }
    finalInfo_.captureActive = false;
    delete state_; state_ = nullptr;
    return ok;
}
}
