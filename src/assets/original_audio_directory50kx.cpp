#include "original_audio_directory50kx.h"
#include "material_audio50hi.h"
#include <cstring>
#include <new>
#include <utility>

namespace rr::assets {
namespace {
using Status = AudioDirectoryStatus50KX;
bool fail(AudioDirectoryDiagnostic50KX& d, Status status,
    std::size_t row = static_cast<std::size_t>(-1)) noexcept {
    d = {status, row};
    return false;
}
unsigned char lowerAscii(unsigned char c) noexcept {
    return c >= 'A' && c <= 'Z' ? static_cast<unsigned char>(c + ('a' - 'A')) : c;
}
bool delimiter(unsigned char c) noexcept {
    // 298740..2988D0 recognizes these exact separators, not CSV or generic
    // whitespace. No comma, quoting, tab, comment or empty-row extension.
    return c == ' ' || c == '\r' || c == '\n';
}
bool sameSourceName(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i)
        if (lowerAscii(static_cast<unsigned char>(a[i])) !=
            lowerAscii(static_cast<unsigned char>(b[i]))) return false;
    return true;
}

// Private bounded SHA-256 authentication of exact32438-byte GC authoring.
// Same fixed-workspace algorithm already used in original_ui_archive50kg.cpp;
// no external digest assertion, executable, file I/O or callback is trusted.
std::uint32_t rotateRight(std::uint32_t x, unsigned n) noexcept {
    return (x >> n) | (x << (32u - n));
}
void shaBlock(const std::uint8_t* block, std::array<std::uint32_t, 8>& h) noexcept {
    constexpr std::uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
    std::uint32_t w[64]{};
    for (unsigned i = 0; i < 16; ++i)
        for (unsigned j = 0; j < 4; ++j) w[i] = (w[i] << 8) | block[i * 4 + j];
    for (unsigned i = 16; i < 64; ++i) {
        const auto s0 = rotateRight(w[i-15],7) ^ rotateRight(w[i-15],18) ^ (w[i-15] >> 3);
        const auto s1 = rotateRight(w[i-2],17) ^ rotateRight(w[i-2],19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    auto a=h[0], b=h[1], c=h[2], d=h[3], e=h[4], f=h[5], g=h[6], v=h[7];
    for (unsigned i = 0; i < 64; ++i) {
        const auto s1 = rotateRight(e,6) ^ rotateRight(e,11) ^ rotateRight(e,25);
        const auto ch = (e & f) ^ (~e & g);
        const auto t1 = v + s1 + ch + k[i] + w[i];
        const auto s0 = rotateRight(a,2) ^ rotateRight(a,13) ^ rotateRight(a,22);
        const auto maj = (a & b) ^ (a & c) ^ (b & c);
        const auto t2 = s0 + maj;
        v=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=v;
}
bool authenticGcAub(const std::vector<std::uint8_t>& input) noexcept {
    if (input.size() != 32438u) return false;
    std::array<std::uint32_t, 8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at = 0;
    for (; input.size() - at >= 64; at += 64) shaBlock(input.data() + at, h);
    std::array<std::uint8_t, 128> tail{};
    const std::size_t remainder = input.size() - at;
    if (remainder) std::memcpy(tail.data(), input.data() + at, remainder);
    tail[remainder] = 0x80;
    const std::size_t padded = remainder < 56 ? 64 : 128;
    const std::uint64_t bits = static_cast<std::uint64_t>(input.size()) * 8u;
    for (unsigned i = 0; i < 8; ++i)
        tail[padded - 1 - i] = static_cast<std::uint8_t>(bits >> (i * 8));
    shaBlock(tail.data(), h);
    if (padded == 128) shaBlock(tail.data() + 64, h);
    constexpr char expected[] =
        "eb4046a25b5b08412c8fa8644107ab826499af754f1093283cc4566e2d879c1d";
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned i = 0; i < 64; ++i)
        if (expected[i] != hex[(h[i / 8] >> ((7u - i % 8) * 4)) & 15u]) return false;
    return true;
}
const ParameterField48F* authoredFilename(const ParameterDocument48F& p,
    const std::string& blockName) noexcept {
    const ParameterField48F* result = nullptr;
    for (const auto& block : p.blocks) {
        if (block.name != blockName) continue;
        for (const auto& field : block.fields) if (field.name == "soundresourcefile") {
            if (result) return nullptr;
            result = &field;
        }
    }
    return result;
}
}

struct OriginalAudioDirectory50KX::Storage {
    std::array<OriginalAudioDirectoryRow50KX, originalAudioDirectoryRows50KX> rows;
};
OriginalAudioDirectory50KX::OriginalAudioDirectory50KX() noexcept = default;
OriginalAudioDirectory50KX::~OriginalAudioDirectory50KX() = default;
bool OriginalAudioDirectory50KX::prepare(const OriginalUiArchive50KG& archive,
    AudioDirectoryDiagnostic50KX& diagnostic) noexcept {
    diagnostic = {};
    if (storage_) return fail(diagnostic, Status::alreadyPrepared);
    if (!archive.loaded()) return fail(diagnostic, Status::archiveNotPrepared);
    constexpr auto appStart = OriginalUiArchiveId50KG::appStart;
    if (archive.identity() != originalUiArchiveIdentity50KG(appStart))
        return fail(diagnostic, Status::archiveIdentity);
    const auto member = archive.member(3);
    if (!member.data || member.identity != originalUiMemberIdentity50KG(appStart, 3) ||
        member.size != 7542u || !member.identity ||
        std::strcmp(member.identity->name, "AppStart\\AudioStr.txt") != 0 ||
        member.identity->logicalOffset != 122880u || member.identity->bytes != member.size ||
        std::strcmp(member.identity->sha256,
            "be478aab56545abac25dcb677cd363e84003b13d81fc9a94246ad752741ed534") != 0)
        return fail(diagnostic, Status::memberIdentity);
    try {
        auto next = std::make_unique<Storage>();
        std::size_t at = 0, count = 0;
        while (at < member.size) {
            if (delimiter(member.data[at])) { ++at; continue; }
            const std::size_t start = at;
            while (at < member.size && !delimiter(member.data[at])) {
                // Exact authenticated profile is printable ASCII. A NUL or
                // unsupported whitespace must not create a truncated token.
                if (member.data[at] < 33 || member.data[at] >= 127)
                    return fail(diagnostic, Status::manifestFormat, count);
                ++at;
            }
            if (count == next->rows.size()) return fail(diagnostic, Status::manifestCount, count);
            auto& row = next->rows[count];
            row.sourceIndex = static_cast<std::uint32_t>(count);
            row.manifestByteOffset = static_cast<std::uint32_t>(start);
            row.authoredName.assign(reinterpret_cast<const char*>(member.data + start), at - start);
            row.sourceName = row.authoredName;
            for (auto& c : row.sourceName) c = static_cast<char>(lowerAscii(static_cast<unsigned char>(c)));
            for (std::size_t i = 0; i < count; ++i)
                if (next->rows[i].sourceName == row.sourceName)
                    return fail(diagnostic, Status::duplicateName, count);
            ++count;
        }
        if (count != next->rows.size()) return fail(diagnostic, Status::manifestCount, count);
        storage_ = std::move(next);
        return true;
    } catch (const std::bad_alloc&) { return fail(diagnostic, Status::allocation); }
    catch (...) { return fail(diagnostic, Status::unexpectedFailure); }
}
bool OriginalAudioDirectory50KX::prepared() const noexcept { return bool(storage_); }
std::size_t OriginalAudioDirectory50KX::size() const noexcept {
    return storage_ ? storage_->rows.size() : 0;
}
const OriginalAudioDirectoryRow50KX* OriginalAudioDirectory50KX::row(std::size_t index) const noexcept {
    return storage_ && index < storage_->rows.size() ? &storage_->rows[index] : nullptr;
}
const OriginalAudioDirectoryRow50KX* OriginalAudioDirectory50KX::find(std::string_view name) const noexcept {
    if (storage_) for (const auto& row : storage_->rows)
        if (sameSourceName(name, row.sourceName)) return &row;
    return nullptr;
}

struct GcMaterialResourceBindings50KX::Storage {
    std::array<GcMaterialResourceBinding50KX, gcMaterialSoundRows50KX> collision, drive;
    std::array<GcMaterialDriveAuthoring50KX, gcMaterialSoundRows50KX> authoring;
};
GcMaterialResourceBindings50KX::GcMaterialResourceBindings50KX() noexcept = default;
GcMaterialResourceBindings50KX::~GcMaterialResourceBindings50KX() = default;
bool GcMaterialResourceBindings50KX::prepare(const OriginalAudioDirectory50KX& directory,
    const std::vector<std::uint8_t>& aub, AudioDirectoryDiagnostic50KX& diagnostic) noexcept {
    diagnostic = {};
    if (storage_) return fail(diagnostic, Status::alreadyPrepared);
    if (!directory.prepared()) return fail(diagnostic, Status::directoryNotPrepared);
    if (!authenticGcAub(aub)) return fail(diagnostic, Status::gcAubIdentity);
    try {
        ParameterDocument48F parameters;
        DecodeError48C decode;
        WorldLimits48F limits{};
        limits.retainDuplicateLabels = true; // Preserve unrelated authored ambient labels.
        if (!decodeParameters48F(aub, parameters, decode, nullptr, limits))
            return fail(diagnostic, Status::gcAubFormat);
        MaterialAudioCatalog50HI catalog;
        std::vector<AudioRequest50HC> requests;
        std::string error;
        if (!readMaterialAudioCatalog50HI(aub, catalog, requests, error) ||
            catalog.parameters.rowCount != gcMaterialSoundRows50KX ||
            catalog.materialCount != 4u || requests.size() != 21u)
            return fail(diagnostic, Status::gcCatalog);
        auto next = std::make_unique<Storage>();
        for (std::size_t kind = 0; kind < 2; ++kind) {
            for (std::size_t i = 0; i < gcMaterialSoundRows50KX; ++i) {
                const auto block = std::string(kind ? "materialdriveonsound" : "materialcollisionsound") +
                    std::to_string(i + 1);
                const auto* source = authoredFilename(parameters, block);
                if (!source || source->value.empty() || source->value.find('.') != std::string::npos ||
                    source->value.size() + 4u >= 0x104u || source->value_offset >= aub.size())
                    return fail(diagnostic, Status::gcAubFormat, kind * gcMaterialSoundRows50KX + i);
                auto& binding = kind ? next->drive[i] : next->collision[i];
                binding.sourceKey = static_cast<std::uint32_t>(i + 1);
                binding.aubValueByteOffset = static_cast<std::uint32_t>(source->value_offset);
                binding.sourceFactoryFilename = source->value + ".wav";
                binding.sourceRegistryName = "audio\\" + source->value + ".vag";
                binding.pcmRequestName = kind ? catalog.drive[i] : catalog.collision[i];
                auto normalized = source->value;
                for (auto& c : normalized) if (c == '\\') c = '/';
                normalized += ".vag";
                if (normalized != binding.pcmRequestName)
                    return fail(diagnostic, Status::gcCatalog, kind * gcMaterialSoundRows50KX + i);
                const auto* resource = directory.find(binding.sourceRegistryName);
                if (!resource) return fail(diagnostic, Status::missingResource, kind * gcMaterialSoundRows50KX + i);
                binding.sourceResourceIndex = resource->sourceIndex;
            }
        }
        for (std::size_t i = 0; i < gcMaterialSoundRows50KX; ++i) {
            const auto& source = catalog.parameters.drive[i];
            // Copy only actual authored controls. Deliberately do not publish
            // the older catalog's native-policy basePitch default.
            next->authoring[i] = {source.sampleSpeed, source.minVolume, source.minSpeed,
                source.maxSpeed, source.resonantSpeed, source.frequencySlope};
        }
        storage_ = std::move(next);
        return true;
    } catch (const std::bad_alloc&) { return fail(diagnostic, Status::allocation); }
    catch (...) { return fail(diagnostic, Status::unexpectedFailure); }
}
bool GcMaterialResourceBindings50KX::prepared() const noexcept { return bool(storage_); }
std::size_t GcMaterialResourceBindings50KX::rowCountPerKind() const noexcept {
    return storage_ ? gcMaterialSoundRows50KX : 0;
}
const GcMaterialResourceBinding50KX* GcMaterialResourceBindings50KX::collision(std::uint32_t key) const noexcept {
    return storage_ && key >= 1 && key <= gcMaterialSoundRows50KX ? &storage_->collision[key - 1] : nullptr;
}
const GcMaterialResourceBinding50KX* GcMaterialResourceBindings50KX::drive(std::uint32_t key) const noexcept {
    return storage_ && key >= 1 && key <= gcMaterialSoundRows50KX ? &storage_->drive[key - 1] : nullptr;
}
const GcMaterialDriveAuthoring50KX* GcMaterialResourceBindings50KX::driveAuthoring(std::uint32_t key) const noexcept {
    return storage_ && key >= 1 && key <= gcMaterialSoundRows50KX ? &storage_->authoring[key - 1] : nullptr;
}
}
