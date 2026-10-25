#include "../../platform/linux/native/loading_progress50du.h"
#include "audio_assets50hc.h"
#include "import_decode48c.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace rr::assets { namespace {
using Bytes = std::vector<std::uint8_t>;
constexpr std::size_t encodedCap = 32u * 1024u * 1024u;
void need(bool value, const char* error) { if (!value) throw std::runtime_error(error); }
// Reused from the existing retained-asset SHA-256 implementation in
// tests/native/pod_materials50ed_fixture.h; no fixture dependency at runtime.
inline std::uint32_t rotr(std::uint32_t x, unsigned n) { return (x >> n) | (x << (32 - n)); }
inline std::string sha256(const Bytes& input) {
    static const std::uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
    std::array<std::uint32_t,8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    Bytes data(input);
    const auto bits = static_cast<std::uint64_t>(data.size()) * 8;
    data.push_back(0x80);
    while (data.size() % 64 != 56) data.push_back(0);
    for (int i = 7; i >= 0; --i) data.push_back(static_cast<std::uint8_t>(bits >> (i * 8)));
    for (std::size_t at = 0; at < data.size(); at += 64) {
        // Bounded host observation during authenticated cold SHA work. The
        // hashed bytes, digest, original identity gates and source order do not change.
        if((at&65535u)==0)rr::platform::loading_progress50DU::pulse();
        std::uint32_t w[64]{};
        for (unsigned i = 0; i < 16; ++i)
            for (unsigned j = 0; j < 4; ++j) w[i] = (w[i] << 8) | data[at+i*4+j];
        for (unsigned i = 16; i < 64; ++i) {
            const auto s0 = rotr(w[i-15],7) ^ rotr(w[i-15],18) ^ (w[i-15] >> 3);
            const auto s1 = rotr(w[i-2],17) ^ rotr(w[i-2],19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        auto v = h;
        for (unsigned i = 0; i < 64; ++i) {
            const auto s1 = rotr(v[4],6) ^ rotr(v[4],11) ^ rotr(v[4],25);
            const auto ch = (v[4] & v[5]) ^ (~v[4] & v[6]);
            const auto t1 = v[7] + s1 + ch + k[i] + w[i];
            const auto s0 = rotr(v[0],2) ^ rotr(v[0],13) ^ rotr(v[0],22);
            const auto maj = (v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]);
            for (unsigned j = 7; j > 0; --j) v[j] = v[j-1];
            v[4] += t1; v[0] = t1 + s0 + maj;
        }
        for (unsigned i = 0; i < 8; ++i) h[i] += v[i];
    }
    std::ostringstream stream;
    stream << std::hex << std::setfill('0');
    for (const auto word : h) stream << std::setw(8) << word;
    return stream.str();
}

std::uint32_t be32(const std::uint8_t* p) noexcept {
    return (std::uint32_t(p[0]) << 24) | (std::uint32_t(p[1]) << 16) |
        (std::uint32_t(p[2]) << 8) | p[3];
}
char fold(char c) noexcept {
    if (c == '\\') return '/';
    return c >= 'A' && c <= 'Z' ? char(c + ('a' - 'A')) : c;
}
bool equal(const std::string& a, const std::string& b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) if (fold(a[i]) != fold(b[i])) return false;
    return true;
}
std::string key(std::string n) {
    need(!n.empty() && n.size() <= 240, "audio.name_extent");
    for (auto& c : n) {
        need((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.' || c == '/' || c == '\\',
            "audio.name_character"); c = fold(c);
    }
    need(n.front() != '/' && n.back() != '/' && n.find("//") == std::string::npos,
        "audio.name_path");
    std::istringstream parts(n); std::string part;
    while (std::getline(parts, part, '/')) need(part != "." && part != "..", "audio.name_traversal");
    need(n.size() > 4 && n.substr(n.size() - 4) == ".vag", "unsupported.audio_extension");
    return n;
}
Bytes read(const std::filesystem::path& p, std::size_t cap) {
    need(!std::filesystem::is_symlink(p), "audio.symlink");
    std::ifstream f(p, std::ios::binary | std::ios::ate);
    need(f.is_open(), "audio.open"); const auto end = f.tellg();
    need(end >= 0 && static_cast<std::uintmax_t>(end) <= cap, "audio.file_limit");
    Bytes b(static_cast<std::size_t>(end)); f.seekg(0);
    if (!b.empty()) f.read(reinterpret_cast<char*>(b.data()), static_cast<std::streamsize>(b.size()));
    need(f.good() && f.peek() == std::char_traits<char>::eof(), "audio.read_extent"); return b;
}
std::string trim(std::string s) {
    const auto first = s.find_first_not_of(" \t\r");
    if (first == std::string::npos) return {};
    return s.substr(first, s.find_last_not_of(" \t\r") - first + 1);
}
void collect(const Bytes& bytes, std::vector<AudioRequest50HC>& out, std::size_t expected) {
    const auto before = out.size(); std::istringstream in(std::string(bytes.begin(), bytes.end()));
    std::string line; AudioRequest50HC request;
    auto flush = [&] { if (!request.name.empty()) { key(request.name); out.push_back(request); } request = {}; };
    while (std::getline(in, line)) {
        line = trim(line); if (line.empty() || line.front() == ';') continue;
        if (line.front() == '[') { flush(); continue; }
        const auto at = line.find('='); if (at == std::string::npos) continue;
        const auto name = trim(line.substr(0, at)); auto value = trim(line.substr(at + 1));
        if (name == "FileName") {
            need(request.name.empty(), "audio.duplicate_filename");
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') value = value.substr(1, value.size()-2);
            request.name = value;
        } else if (name == "IsStereo" || name == "IsStream" || name == "IsLoop") {
            need(value == "0" || value == "1", "audio.invalid_flag");
            if (name == "IsStereo") request.stereo = value == "1";
            if (name == "IsStream") request.stream = value == "1";
            if (name == "IsLoop") request.loop = value == "1";
        } else if (name == "EngineAudioFile" || name == "BeamAudioFile") {
            key(value); out.push_back({value, false, false, false});
        }
    }
    flush(); need(out.size() - before == expected, "audio.authoring_count");
}
} // namespace

bool decodeVagMono50HC(const std::uint8_t* bytes, std::size_t size, AudioPcm50HC& out,
    std::string& error, const AudioDecodeLimits50HC& limits) {
    error.clear(); try {
        need(size <= std::min(encodedCap, limits.encodedBytes), "audio.encoded_limit");
        need(bytes && size >= 48, "audio.header_truncated");
        const bool standardMagic = std::memcmp(bytes, "VAGp", 4) == 0;
        // Actual transferred Racer Revenge files preserve the conventional
        // BE length/rate at 0x0C/0x10 but zero every other header field.
        // This is an explicit 48-byte header variant, never raw/headerless
        // ADPCM guessing. Require its exact payload extent and zero fields.
        bool retainedZeroHeader = true;
        for (std::size_t i = 0; i < 48; ++i)
            if ((i < 12 || i >= 20) && bytes[i] != 0) retainedZeroHeader = false;
        need(standardMagic || retainedZeroHeader, "unsupported.vag_magic");
        const auto version = be32(bytes + 4), payload = be32(bytes + 12), rate = be32(bytes + 16);
        need(version == 0 || version == 2 || version == 3 || version == 0x20, "unsupported.vag_version");
        need(be32(bytes + 8) == 0, "unsupported.vag_reserved");
        // The actual MA material3 originals FLDHIT1/2 have an opaque0FE0
        // word at0x18. Admit only these authenticated mono48-byte-header
        // files for native PCM rendering. This does NOT interpret that word
        // as pitch metadata, infer an IOP mapping, or relax payload checks.
        bool retainedFieldHitHeader=false;
        if(standardMagic&&version==3&&rate==22050&&be32(bytes+20)==0&&
           be32(bytes+24)==0x0fe00000u&&be32(bytes+28)==0&&
           ((size==12336&&payload==12288)||(size==9632&&payload==9584))){
            const auto identity=sha256(Bytes(bytes,bytes+size));
            retainedFieldHitHeader=(size==12336&&identity==
                "1c27d67c3713d551f6400072a122287b607863b18f155aea3d25e9cb4a5d477e")||
                (size==9632&&identity==
                "0d0671026a864ec56f2ffebfb79149b648299552279c59049bfbbb0216ce59bf");
        }
        if(!retainedFieldHitHeader)for(std::size_t i=20;i<32;++i)
            need(bytes[i]==0,"unsupported.vag_header_variant");
        need(payload && payload % 16 == 0 && payload <= size - 48, "audio.payload_extent");
        need(!retainedZeroHeader || payload == size - 48, "unsupported.vag_zero_header_extent");
        // Admit only bounded zero alignment padding, never silently decode a
        // second channel, unrelated suffix, or a guessed interleave layout.
        need(size - 48 - payload <= 2048, "unsupported.vag_trailing_extent");
        for (std::size_t i = 48 + payload; i < size; ++i) need(bytes[i] == 0, "unsupported.vag_trailing_data");
        need(rate >= 4000 && rate <= 192000, "unsupported.vag_sample_rate");
        const std::size_t maximumFrames = (std::size_t(payload) / 16) * 28;
        need(maximumFrames <= std::min<std::size_t>(32u*1024u*1024u, limits.pcmFrames), "audio.pcm_limit");
        for (const auto offset : {0x800u, 0x1000u, 0x6000u})
            need(offset + 4 > size || std::memcmp(bytes + offset, "VAGp", 4) != 0, "unsupported.vag_interleave");
        AudioPcm50HC pcm; pcm.samples.reserve(maximumFrames); pcm.sampleRate = rate; pcm.channels = 1;
        constexpr int a[5] = {0, 60, 115, 98, 122}, b[5] = {0, 0, -52, -55, -60};
        std::int64_t h1 = 0, h2 = 0; bool loopStart = false, ended = false;
        for (std::size_t offset = 48; offset < 48 + payload; offset += 16) {
            if(((offset-48)&65535u)==0)rr::platform::loading_progress50DU::pulse();
            const unsigned filter = bytes[offset] >> 4, shift = bytes[offset] & 15, flags = bytes[offset+1];
            need(filter <= 4 && shift <= 12, "unsupported.vag_predictor");
            need(flags <= 7, "unsupported.vag_flags");
            if ((flags & 4) && flags != 7) {
                need(!loopStart, "unsupported.vag_multiple_loop_starts");
                loopStart = true; pcm.loopStart = pcm.samples.size();
            }
            for (unsigned i = 0; i < 28; ++i) {
                int nibble = (bytes[offset + 2 + i/2] >> ((i%2)*4)) & 15;
                if (nibble >= 8) nibble -= 16;
                // 12-shift is nonnegative; multiplication avoids negative shifts.
                const std::int64_t sample = flags == 7 ? 0 :
                    nibble * (1 << (12-shift)) + (h1*a[filter] + h2*b[filter]) / 64;
                need(sample >= -2147483647 && sample <= 2147483647, "audio.predictor_range");
                pcm.samples.push_back(static_cast<std::int16_t>(std::clamp<std::int64_t>(sample, -32768, 32767)));
                h2 = h1; h1 = sample;
            }
            if (flags & 1) {
                if ((flags & 2) && flags != 7) {
                    need(loopStart, "unsupported.vag_loop_without_start");
                    pcm.loopEnd = pcm.samples.size(); pcm.hasEmbeddedLoop = true;
                }
                ended = true; break;
            }
        }
        (void)ended; // Finite payload without an end flag is a valid bounded clip.
        if (!pcm.hasEmbeddedLoop) { pcm.loopStart = 0; pcm.loopEnd = 0; }
        pcm.frames = pcm.samples.size();
        pcm.sourceSha256 = sha256(Bytes(bytes, bytes + size)); out = std::move(pcm); return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}
const AudioClip50HC* AudioBank50HC::find(const std::string& name) const noexcept {
    for (const auto& clip : clips) if (equal(clip.request.name, name)) return &clip;
    return nullptr;
}
bool loadAudioBank50HC(const std::filesystem::path& root,
    const std::vector<AudioRequest50HC>& requests, AudioBank50HC& out, std::string& error) {
    error.clear(); try {
        need(requests.size() <= 4096, "audio.request_limit");
        std::map<std::string, std::filesystem::path> files;
        if (std::filesystem::exists(root)) {
            need(!std::filesystem::is_symlink(root) && std::filesystem::is_directory(root), "audio.root_not_directory");
            std::size_t entries = 0;
            for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
                need(++entries <= 16384, "audio.directory_limit");
                if (entry.is_symlink() || !entry.is_regular_file()) continue;
                const auto relative = entry.path().lexically_relative(root).generic_string();
                auto normalized = relative; for (auto& c : normalized) c = fold(c);
                if (normalized.size() < 4 || normalized.substr(normalized.size()-4) != ".vag") continue;
                normalized = key(normalized);
                need(files.emplace(normalized, entry.path()).second, "audio.ambiguous_case_names");
            }
        }
        AudioBank50HC bank; bank.clips.reserve(requests.size()); std::size_t total = 0;
        std::map<std::string, bool> seen;
        for (const auto& request : requests) {
            const auto name = key(request.name);
            need(seen.emplace(name, true).second, "audio.duplicate_request");
            AudioClip50HC clip; clip.request = request; const auto found = files.find(name);
            if (found == files.end()) { clip.detail = "Original sample bytes missing"; ++bank.missing; }
            else {
                clip.sourcePath = found->second.string();
                try {
                    auto bytes = read(found->second, encodedCap); clip.sourceBytes = bytes.size(); clip.sourceSha256 = sha256(bytes);
                    if (request.stereo) { clip.state = AudioAssetState50HC::unsupported; clip.detail = "unsupported.authored_stereo"; ++bank.unsupported; }
                    else if (!decodeVagMono50HC(bytes.data(), bytes.size(), clip.pcm, clip.detail)) {
                        clip.state = clip.detail.rfind("unsupported.", 0) == 0 ? AudioAssetState50HC::unsupported : AudioAssetState50HC::invalid;
                        if (clip.state == AudioAssetState50HC::unsupported) ++bank.unsupported; else ++bank.invalid;
                    } else {
                        need(clip.pcm.samples.size() <= (256u*1024u*1024u-total)/sizeof(std::int16_t), "audio.bank_pcm_limit");
                        total += clip.pcm.samples.size()*sizeof(std::int16_t); clip.state = AudioAssetState50HC::ready; ++bank.ready;
                    }
                } catch (const std::exception& e) { clip.pcm = {}; clip.state = AudioAssetState50HC::invalid; clip.detail = e.what(); ++bank.invalid; }
            }
            bank.clips.push_back(std::move(clip));
        }
        out = std::move(bank); return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}
bool readRetainedAudioRequests50HC(const std::filesystem::path& root,
    std::vector<AudioRequest50HC>& out, std::string& error) {
    error.clear(); try {
        struct Archive { const char* path; std::size_t size; const char* hash; };
        const Archive sources[] = {
            {"PODS/POD01/POD01.RES",349110,"08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3"},
            {"TRACKS/TA.RES",3334562,"b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8"}};
        const char* members[] = {"audio\\p_event.ini", "audio\\p_debris.ini", "audio\\wrecks.txt",
            "audio\\speech\\pod01\\curses.ini", "audio\\speech\\pod01\\taunts.ini", "audio\\speech\\pod01\\special.ini", "Pods\\Pod01\\Pod01.phy"};
        const std::size_t counts[] = {6,4,4,19,12,9,2};
        std::vector<AudioRequest50HC> result;
        for (unsigned i = 0; i < 2; ++i) {
            const auto bytes = read(root/sources[i].path, sources[i].size);
            need(bytes.size() == sources[i].size && sha256(bytes) == sources[i].hash, "audio.archive_identity");
            ResArchive48C archive; DecodeError48C decode; DecodeLimits48C limits;
            limits.resource_decoded_bytes = 16u*1024u*1024u; limits.resource_entries = 512; limits.resource_name_bytes = 256;
            need(decodeResource48C(bytes, archive, decode, nullptr, limits), "audio.resource_decode");
            if (i == 0) for (unsigned j = 0; j < 7; ++j) {
                Bytes member; need(extractResMember48C(archive, members[j], member, decode, 8192), "audio.member_missing");
                collect(member, result, counts[j]);
            } else {
                Bytes member; need(extractResMember48C(archive, "audio\\introAud.ini", member, decode, 8192), "audio.member_missing");
                collect(member, result, 3);
            }
        }
        // Direct static literal, not a manufactured engine/boost substitution.
        const auto elf = read(root/"SLUS_202.68",2972720);
        need(elf.size() == 2972720 && sha256(elf) == "c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30", "audio.source_identity");
        constexpr char boost[] = "sfx\\race\\boost.vag";
        need(std::memcmp(elf.data()+0x2a8210, boost, sizeof(boost)) == 0, "audio.boost_source_literal");
        result.push_back({boost, false, false, false});
        need(result.size() == 60, "audio.catalog_count"); out = std::move(result); return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}
} // namespace rr::assets
