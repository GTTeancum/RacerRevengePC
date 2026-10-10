#include "audio_output50hd.h"
#include <algorithm>
#include <atomic>
#include <bit>
#include <cerrno>
#include <cstring>
#include <dlfcn.h>
#include <fcntl.h>
#include <limits>
#include <new>
#include <unistd.h>

#if !defined(__linux__)
#error "AudioOutput50HD implements the Linux SDL2 runtime ABI"
#endif
namespace rr::platform {
namespace {
constexpr std::uint32_t sdlAudioInit = 0x10;
constexpr std::uint16_t sdlS16LittleEndian = 0x8010;
constexpr std::size_t queueFrames = 32768, blockFrames = 1024;
constexpr std::uint64_t maximumWavFrames = (std::uint64_t{0xffffffff} - 36) / 4;
// SDL2's documented public SDL_AudioSpec layout; no SDK headers required.
struct SdlAudioSpec {
    int freq;
    std::uint16_t format;
    std::uint8_t channels, silence;
    std::uint16_t samples, padding;
    std::uint32_t size;
    void (*callback)(void*, std::uint8_t*, int);
    void* userdata;
};
static_assert(sizeof(int) == 4 && offsetof(SdlAudioSpec, callback) == 16);
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
bool writeAll(int fd, const void* bytes, std::size_t size) noexcept {
    const auto* source = static_cast<const std::uint8_t*>(bytes);
    while (size) {
        const auto written = ::write(fd, source, size);
        if (written < 0 && errno == EINTR) continue;
        if (written <= 0) return false;
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
    void* library = nullptr;
    std::uint32_t device = 0;
    int capture = -1;
    bool subsystemOwned = false, started = false, failed = false;
    std::uint16_t prebuffer = 1536;
    std::array<std::int16_t, queueFrames * 2> ring{};
    std::array<std::int16_t, blockFrames * 2> mixed{};
    std::atomic<std::uint64_t> read{0}, write{0}, consumed{0}, underruns{0};
    int (*initSubsystem)(std::uint32_t) = nullptr;
    void (*quitSubsystem)(std::uint32_t) = nullptr;
    const char* (*getError)() = nullptr;
    const char* (*getDriver)() = nullptr;
    std::uint32_t (*openDevice)(const char*, int, const SdlAudioSpec*, SdlAudioSpec*, int) = nullptr;
    void (*closeDevice)(std::uint32_t) = nullptr;
    void (*pauseDevice)(std::uint32_t, int) = nullptr;
    void (*lockDevice)(std::uint32_t) = nullptr;
    void (*unlockDevice)(std::uint32_t) = nullptr;
    int (*deviceStatus)(std::uint32_t) = nullptr;

    static void callback(void* opaque, std::uint8_t* output, int bytes) noexcept {
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
        if (library) dlclose(library);
        library = nullptr;
    }
    void deviceFailure(const char* detail,
                       AudioDeviceStatus50HD status = AudioDeviceStatus50HD::Unavailable) noexcept {
        info.deviceStatus = status; copyText(info.deviceDetail, detail); releaseDevice();
    }
    void prepareDevice(const AudioOutputRequest50HD& request) noexcept {
        info.deviceStatus = AudioDeviceStatus50HD::Unavailable;
        library = dlopen("libSDL2-2.0.so.0", RTLD_NOW | RTLD_LOCAL);
        if (!library) library = dlopen("libSDL2.so", RTLD_NOW | RTLD_LOCAL);
        if (!library) { deviceFailure(dlerror()); return; }
#define RR_AUDIO50HD_LOAD(member, symbol) \
        member = reinterpret_cast<decltype(member)>(dlsym(library, symbol)); \
        if (!member) { deviceFailure("Missing SDL2 entry point: " symbol); return; }
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
        s.capture = ::open(request.wavPath, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
        if (s.capture < 0) {
            fail(d, AudioOutputError50HD::CaptureOpen, "audio.open/WAV", std::strerror(errno));
            delete state_; state_ = nullptr; return false;
        }
        const auto header = wavHeader(request.sampleRate, 0);
        if (!writeAll(s.capture, header.data(), header.size())) {
            fail(d, AudioOutputError50HD::CaptureWrite, "audio.open/WAV header", std::strerror(errno));
            ::close(s.capture); delete state_; state_ = nullptr; return false;
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
        if (::lseek(s.capture, 0, SEEK_SET) < 0 || !writeAll(s.capture, header.data(), header.size()))
            ok = fail(d, AudioOutputError50HD::CaptureFinalize, "audio.close/WAV header", std::strerror(errno));
        // A failed partial write is excluded from RIFF's advertised data range.
        if (::ftruncate(s.capture, static_cast<off_t>(44 + s.info.capturedFrames * 4)) != 0 && ok)
            ok = fail(d, AudioOutputError50HD::CaptureFinalize, "audio.close/WAV length", std::strerror(errno));
        if (::close(s.capture) != 0 && ok)
            ok = fail(d, AudioOutputError50HD::CaptureFinalize, "audio.close/WAV file", std::strerror(errno));
    }
    finalInfo_.captureActive = false;
    delete state_; state_ = nullptr;
    return ok;
}
}
