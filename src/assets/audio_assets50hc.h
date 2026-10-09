#pragma once
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace rr::assets {
// Cold, native PCM ownership. No device, synthesis, guest state or filesystem
// access occurs in decodeVagMono50HC. This is not SPU2 hardware emulation.
struct AudioPcm50HC {
    std::vector<std::int16_t> samples; // Interleaved; supported VAG subset is mono.
    std::uint32_t sampleRate = 0;
    std::uint8_t channels = 0;
    std::uint64_t frames = 0, loopStart = 0, loopEnd = 0;
    bool hasEmbeddedLoop = false;
    std::string sourceSha256;
};
struct AudioDecodeLimits50HC {
    std::size_t encodedBytes = 32u * 1024u * 1024u;
    std::size_t pcmFrames = 32u * 1024u * 1024u;
};
// Standard big-endian VAGp and the verified Racer Revenge zeroed-header
// variant, mono, 0x30-byte header, PSX ADPCM. The zeroed variant retains
// BE length/rate and requires every other header byte zero + exact extent.
// Other variants fail explicitly. Sample rate comes only from the header.
// Output is unchanged on failure. Loop range is [start,end), both in frames.
bool decodeVagMono50HC(const std::uint8_t*, std::size_t, AudioPcm50HC&,
    std::string& error, const AudioDecodeLimits50HC& = {});

struct AudioRequest50HC {
    std::string name; // Original AUDIO-relative name, case-insensitive '\\' or '/'.
    bool loop = false, stereo = false, stream = false;
};
enum class AudioAssetState50HC : std::uint8_t { missing, ready, unsupported, invalid };
struct AudioClip50HC {
    AudioRequest50HC request;
    AudioAssetState50HC state = AudioAssetState50HC::missing;
    AudioPcm50HC pcm;
    std::string sourcePath, sourceSha256, detail;
    std::uint64_t sourceBytes = 0;
};
struct AudioBank50HC {
    std::vector<AudioClip50HC> clips;
    std::size_t ready = 0, missing = 0, unsupported = 0, invalid = 0;
    const AudioClip50HC* find(const std::string& name) const noexcept;
};
// Cold load only. Root is AUDIO itself, never a guessed external path.
// Missing/unsupported clips remain explicit records. At most 4096 requests,
// 16384 directory entries, and 256 MiB total decoded PCM. No fallback sounds.
// Original stream=true mono clips use explicit native preload delivery;
// authored stream metadata is preserved, without emulating the IOP streamer.
// No symlinks are followed. Output is transactional on a bank-level failure.
bool loadAudioBank50HC(const std::filesystem::path& audioRoot,
    const std::vector<AudioRequest50HC>&, AudioBank50HC&, std::string& error);
// A catalog of exact retained Pod01/TA authoring; archive identities checked.
// Includes response effects/voices, engine and beam. It does not assign live
// gameplay triggers, invent missing event authoring, or claim loaded samples.
bool readRetainedAudioRequests50HC(const std::filesystem::path& discRoot,
    std::vector<AudioRequest50HC>&, std::string& error);
}
