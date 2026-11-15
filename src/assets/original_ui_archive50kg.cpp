// NEW 2026-10-10 reconstruction, not recovered original lost source.
#include "original_ui_archive50kg.h"
#include <array>
#include <atomic>
#include <cstring>
#include <deque>
#include <iostream>
#include <mutex>
#include <new>
#include <string>
#include <utility>

namespace rr::assets {
namespace {
#include "original_ui_archive_catalog50kg.inc"

constexpr bool validCatalogLayout() noexcept {
    constexpr auto archiveCount = sizeof(archiveCatalog) / sizeof(archiveCatalog[0]);
    constexpr auto memberCount = sizeof(memberCatalog) / sizeof(memberCatalog[0]);
    if (archiveCount != 43 || memberCount != 3085) return false;
    std::size_t next = 0;
    for (std::size_t i = 0; i < archiveCount; ++i) {
        const auto& archive = archiveCatalog[i];
        if (static_cast<std::size_t>(archive.id) != i || archive.firstMember != next ||
            archive.memberCount > memberCount - next) return false;
        next += archive.memberCount;
    }
    return next == memberCount;
}
static_assert(validCatalogLayout(), "Original UI catalog ranges and IDs must agree");

bool isCancelled(const DecodeHooks48C* hooks) noexcept {
    return hooks && hooks->cancelled && hooks->cancelled(hooks->context);
}
std::uint32_t rotateRight(std::uint32_t x, unsigned n) noexcept {
    return (x >> n) | (x << (32u - n));
}
// Standard SHA-256, fixed stack workspace; no member-sized temporary copies.
// Constants are the SHA-256 constants also retained in audio_assets50hc.cpp.
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
enum class HashResult { match, mismatch, cancelled };
HashResult hashMatches(const std::uint8_t* data, std::size_t size,
    const char* expected, const DecodeHooks48C* hooks) noexcept {
    std::array<std::uint32_t, 8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at = 0;
    for (; size - at >= 64; at += 64) {
        if ((at % 65536 == 0) && isCancelled(hooks)) return HashResult::cancelled;
        shaBlock(data + at, h);
    }
    std::array<std::uint8_t, 128> tail{};
    const std::size_t remainder = size - at;
    if (remainder) std::memcpy(tail.data(), data + at, remainder);
    tail[remainder] = 0x80;
    const std::size_t padded = remainder < 56 ? 64 : 128;
    const std::uint64_t bits = static_cast<std::uint64_t>(size) * 8u;
    for (unsigned i = 0; i < 8; ++i)
        tail[padded - 1 - i] = static_cast<std::uint8_t>(bits >> (i * 8));
    shaBlock(tail.data(), h);
    if (padded == 128) shaBlock(tail.data() + 64, h);
    if (isCancelled(hooks)) return HashResult::cancelled;
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned i = 0; i < 64; ++i)
        if (expected[i] != hex[(h[i / 8] >> ((7u - i % 8) * 4)) & 15u])
            return HashResult::mismatch;
    return HashResult::match;
}
// Lower-case hex SHA-256 of a buffer (same block function as hashMatches).
std::string sha256Hex(const std::uint8_t* data, std::size_t size) {
    std::array<std::uint32_t, 8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at = 0;
    for (; size - at >= 64; at += 64) shaBlock(data + at, h);
    std::array<std::uint8_t, 128> tail{};
    const std::size_t remainder = size - at;
    if (remainder) std::memcpy(tail.data(), data + at, remainder);
    tail[remainder] = 0x80;
    const std::size_t padded = remainder < 56 ? 64 : 128;
    const std::uint64_t bits = static_cast<std::uint64_t>(size) * 8u;
    for (unsigned i = 0; i < 8; ++i)
        tail[padded - 1 - i] = static_cast<std::uint8_t>(bits >> (i * 8));
    shaBlock(tail.data(), h);
    if (padded == 128) shaBlock(tail.data() + 64, h);
    constexpr char hex[] = "0123456789abcdef";
    std::string out(64, '0');
    for (unsigned i = 0; i < 64; ++i) out[i] = hex[(h[i / 8] >> ((7u - i % 8) * 4)) & 15u];
    return out;
}
// One rule for every UI and LOC archive (the original reads whatever UI\*.RES
// and LOC.RES the disc holds: no checksum, byte size or member count gate).
// The catalog's archive and member rows are RECEIPTS. A stock archive keeps
// the exact catalog path below unchanged. Any other archive is decoded within
// the shared capacity and each member whose bytes differ from the catalog gets
// ONE process-wide identity row built from the bytes actually loaded; the
// member lookup returns that row, so every consumer that compares identity
// pointers, sizes or digests sees one consistent object. Rows are host memory,
// indexed like memberCatalog (bounds-checked), never removed or changed.
// FLAG stand-ins: (1) member NAMES and ORDER must still equal the catalog,
// because ~80 port consumers address UI members by catalog ordinal where the
// original looks them up by name; (2) the archive identity object (and so the
// provider's mount label) stays the catalog's, the stand-in the course commit
// retired for course mounts; mount checks stay self-consistent because the
// provider takes its label from that same object.
struct AdmittedMemberRegistry {
    std::mutex lock;                                 // serialises publication only
    std::deque<OriginalUiMemberIdentity50KG> rows;   // stable addresses
    std::deque<std::string> text;                    // owns the digests
    // Published once per index (release); readers load without the lock.
    std::array<std::atomic<const OriginalUiMemberIdentity50KG*>,
        sizeof(memberCatalog) / sizeof(memberCatalog[0])> byCatalog{};
};
AdmittedMemberRegistry& admittedRegistry() {
    static AdmittedMemberRegistry registry;
    return registry;
}
const OriginalUiMemberIdentity50KG* admittedRow(std::size_t catalogIndex) noexcept {
    auto& r = admittedRegistry();
    if (catalogIndex >= r.byCatalog.size()) return nullptr;
    return r.byCatalog[catalogIndex].load(std::memory_order_acquire);
}
unsigned char fold(unsigned char c) noexcept {
    if (c == '\\') return '/';
    return c >= 'A' && c <= 'Z' ? static_cast<unsigned char>(c + ('a' - 'A')) : c;
}
bool sameName(const std::string& input, const char* original) noexcept {
    const auto length = std::strlen(original);
    if (input.size() != length) return false;
    for (std::size_t i = 0; i < length; ++i)
        if (fold(static_cast<unsigned char>(input[i])) != fold(static_cast<unsigned char>(original[i])))
            return false;
    return true;
}
}

const OriginalUiArchiveIdentity50KG* originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG id) noexcept {
    const auto ordinal = static_cast<std::size_t>(id);
    if (ordinal >= sizeof(archiveCatalog) / sizeof(archiveCatalog[0])) return nullptr;
    return &archiveCatalog[ordinal];
}
const OriginalUiArchiveIdentity50KG* originalUiGaragePodArchive50KG(std::uint32_t career) noexcept {
    switch(career){
    case 1: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod01);
    case 2: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod02);
    case 3: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod03);
    case 4: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod04);
    case 7: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod07);
    case 8: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod08);
    case 9: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod09);
    case 10: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod10);
    case 11: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod11);
    case 12: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod12);
    case 14: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod14);
    case 15: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod15);
    case 16: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod16);
    case 17: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod17);
    case 18: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod18);
    case 19: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod19);
    case 20: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod20);
    case 21: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod21);
    case 22: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod22);
    case 23: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod23);
    case 24: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod24);
    case 25: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod25);
    case 26: return originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garagePod26);
    default: return nullptr;
    }
}
int originalUiGaragePodCareer50KG(OriginalUiArchiveId50KG id) noexcept {
    switch(id){
    case OriginalUiArchiveId50KG::garagePod01: return 1;
    case OriginalUiArchiveId50KG::garagePod02: return 2;
    case OriginalUiArchiveId50KG::garagePod03: return 3;
    case OriginalUiArchiveId50KG::garagePod04: return 4;
    case OriginalUiArchiveId50KG::garagePod07: return 7;
    case OriginalUiArchiveId50KG::garagePod08: return 8;
    case OriginalUiArchiveId50KG::garagePod09: return 9;
    case OriginalUiArchiveId50KG::garagePod10: return 10;
    case OriginalUiArchiveId50KG::garagePod11: return 11;
    case OriginalUiArchiveId50KG::garagePod12: return 12;
    case OriginalUiArchiveId50KG::garagePod14: return 14;
    case OriginalUiArchiveId50KG::garagePod15: return 15;
    case OriginalUiArchiveId50KG::garagePod16: return 16;
    case OriginalUiArchiveId50KG::garagePod17: return 17;
    case OriginalUiArchiveId50KG::garagePod18: return 18;
    case OriginalUiArchiveId50KG::garagePod19: return 19;
    case OriginalUiArchiveId50KG::garagePod20: return 20;
    case OriginalUiArchiveId50KG::garagePod21: return 21;
    case OriginalUiArchiveId50KG::garagePod22: return 22;
    case OriginalUiArchiveId50KG::garagePod23: return 23;
    case OriginalUiArchiveId50KG::garagePod24: return 24;
    case OriginalUiArchiveId50KG::garagePod25: return 25;
    case OriginalUiArchiveId50KG::garagePod26: return 26;
    default: return -1;
    }
}
const OriginalUiMemberIdentity50KG* originalUiMemberIdentity50KG(
    OriginalUiArchiveId50KG id, std::size_t ordinal) noexcept {
    const auto* archive = originalUiArchiveIdentity50KG(id);
    if (!archive || ordinal >= archive->memberCount) return nullptr;
    if (const auto* admitted = admittedRow(archive->firstMember + ordinal)) return admitted;
    return &memberCatalog[archive->firstMember + ordinal];
}
struct OriginalUiArchive50KG::Storage {
    const OriginalUiArchiveIdentity50KG* identity = nullptr;
    std::vector<std::uint8_t> decoded;
    std::vector<std::uint8_t> encoded;
    std::vector<ResourceBlock48C> blocks;
};
OriginalUiArchive50KG::OriginalUiArchive50KG() noexcept = default;
OriginalUiArchive50KG::~OriginalUiArchive50KG() = default;

bool OriginalUiArchive50KG::load(OriginalUiArchiveId50KG id,
    const std::vector<std::uint8_t>& input, OriginalUiArchiveDiagnostic50KG& diagnostic,
    const DecodeHooks48C* hooks) noexcept {
    return loadImpl(id,input,nullptr,diagnostic,hooks);
}
bool OriginalUiArchive50KG::loadOwned(OriginalUiArchiveId50KG id,
    std::vector<std::uint8_t>&& input, OriginalUiArchiveDiagnostic50KG& diagnostic,
    const DecodeHooks48C* hooks) noexcept {
    return loadImpl(id,input,&input,diagnostic,hooks);
}
bool OriginalUiArchive50KG::loadImpl(OriginalUiArchiveId50KG id,
    const std::vector<std::uint8_t>& input, std::vector<std::uint8_t>* transfer,
    OriginalUiArchiveDiagnostic50KG& diagnostic, const DecodeHooks48C* hooks) noexcept {
    diagnostic = {};
    using Status = OriginalUiArchiveStatus50KG;
    if (storage_) { diagnostic.status = Status::alreadyLoaded; return false; }
    const auto* expected = originalUiArchiveIdentity50KG(id);
    if (!expected) { diagnostic.status = Status::unknownArchive; return false; }
    try {
        if (isCancelled(hooks)) { diagnostic.status = Status::cancelled; return false; }
        // Archive identity is a receipt: stock bytes take the exact catalog
        // path; any other archive is admitted by its own structure below.
        bool stock = input.size() == expected->bytes;
        if (stock) {
            const auto archiveHash = hashMatches(input.data(), input.size(), expected->sha256, hooks);
            if (archiveHash == HashResult::cancelled) { diagnostic.status = Status::cancelled; return false; }
            stock = archiveHash == HashResult::match;
        }
        if (!stock) {
            try { std::cerr << "ui_archive_identity archive=" << expected->discPath << " bytes=" << input.size()
                            << " stock=0\n"; } catch (...) {}
        }
        // Shared UI decode capacity (DecodeLimits48C defaults: 64 MiB file,
        // 128 MiB decoded). A stock archive keeps its exact stock limits.
        DecodeLimits48C limits;
        if (stock) limits.resource_file_bytes = expected->bytes;
        limits.resource_entries = static_cast<std::uint32_t>(expected->memberCount);
        ResArchive48C parsed;
        if (!decodeResource48C(input, parsed, diagnostic.resource, hooks, limits)) {
            diagnostic.status = diagnostic.resource.kind == DecodeKind48C::cancelled ? Status::cancelled : Status::resource;
            return false;
        }
        // FLAG stand-in: the member count and names/order are the catalog's.
        if (parsed.entries.size() != expected->memberCount) { diagnostic.status = Status::memberTable; return false; }
        // Non-stock: every member's own extent and digest; changed rows are
        // staged here and published only after the whole archive is admitted.
        struct Staged { std::size_t index; std::uint32_t offset, bytes; std::string sha; };
        std::vector<Staged> staged;
        for (std::size_t i = 0; !stock && i < expected->memberCount; ++i) {
            diagnostic.member = i;
            if (isCancelled(hooks)) { diagnostic.status = Status::cancelled; return false; }
            const auto& actual = parsed.entries[i];
            const auto& member = memberCatalog[expected->firstMember + i];
            if (actual.name != member.name || actual.logical_offset > parsed.decoded.size() ||
                actual.bytes > parsed.decoded.size() - actual.logical_offset) {
                diagnostic.status = Status::memberTable; return false;
            }
            const auto* data = parsed.decoded.data() + actual.logical_offset;
            if (actual.logical_offset == member.logicalOffset && actual.bytes == member.bytes) {
                const auto result = hashMatches(data, actual.bytes, member.sha256, hooks);
                if (result == HashResult::cancelled) { diagnostic.status = Status::cancelled; return false; }
                if (result == HashResult::match && !admittedRow(expected->firstMember + i)) continue;
            }
            staged.push_back({expected->firstMember + i, actual.logical_offset, actual.bytes, sha256Hex(data, actual.bytes)});
        }
        for (std::size_t i = 0; stock && i < expected->memberCount; ++i) {
            diagnostic.member = i;
            if (isCancelled(hooks)) { diagnostic.status = Status::cancelled; return false; }
            const auto& actual = parsed.entries[i];
            const auto& member = memberCatalog[expected->firstMember + i];
            // Authentication is exact, unlike the case-insensitive lookup API.
            if (actual.name != member.name || actual.logical_offset != member.logicalOffset || actual.bytes != member.bytes ||
                member.logicalOffset > parsed.decoded.size() || member.bytes > parsed.decoded.size() - member.logicalOffset) {
                diagnostic.status = Status::memberTable; return false;
            }
            // A stock load never mixes with rows admitted from other bytes.
            if (admittedRow(expected->firstMember + i)) { diagnostic.status = Status::memberIdentity; return false; }
            const auto result = hashMatches(parsed.decoded.data() + member.logicalOffset, member.bytes, member.sha256, hooks);
            if (result != HashResult::match) {
                diagnostic.status = result == HashResult::cancelled ? Status::cancelled : Status::memberIdentity;
                return false;
            }
        }
        auto candidate = std::make_unique<Storage>();
        candidate->identity = expected;
        candidate->decoded = std::move(parsed.decoded);
        candidate->blocks = std::move(parsed.blocks);
        // Const-input callers need one owned encoded allocation. The actual
        // file reader transfers its allocation instead; all fallible work and
        // cancellation checks finish before that no-throw ownership transfer.
        if (!transfer) candidate->encoded = input;
        if (isCancelled(hooks)) { diagnostic.status = Status::cancelled; return false; }
        // Non-stock member rows are published after every check and before the
        // no-throw ownership transfer, so a refused load leaves input unchanged.
        if (!stock) {
            auto& r = admittedRegistry();
            std::lock_guard<std::mutex> guard(r.lock);
            // A row already published for this index must describe the same
            // bytes (a second load of the same archive); otherwise refuse.
            for (const auto& s : staged) {
                if (s.index >= r.byCatalog.size()) { diagnostic.status = Status::memberTable; return false; }
                const auto* existing = r.byCatalog[s.index].load(std::memory_order_relaxed);
                if (existing && (existing->logicalOffset != s.offset || existing->bytes != s.bytes ||
                    s.sha != existing->sha256)) { diagnostic.status = Status::memberIdentity; return false; }
            }
            for (const auto& s : staged) {
                if (r.byCatalog[s.index].load(std::memory_order_relaxed)) continue;
                r.text.push_back(s.sha);
                r.rows.push_back({memberCatalog[s.index].name, s.offset, s.bytes, r.text.back().c_str()});
                r.byCatalog[s.index].store(&r.rows.back(), std::memory_order_release);
            }
            try { std::cerr << "ui_archive_members archive=" << expected->discPath << " members="
                            << expected->memberCount << " changed=" << staged.size() << "\n"; } catch (...) {}
        }
        if (transfer) candidate->encoded.swap(*transfer);
        storage_ = std::move(candidate); // Only publication point, after all authentication.
        diagnostic = {};
        return true;
    } catch (const std::bad_alloc&) { diagnostic.status = Status::allocation; }
    catch (...) { diagnostic.status = Status::unexpectedFailure; }
    return false;
}
bool OriginalUiArchive50KG::loaded() const noexcept { return bool(storage_); }
const OriginalUiArchiveIdentity50KG* OriginalUiArchive50KG::identity() const noexcept {
    return storage_ ? storage_->identity : nullptr;
}
std::size_t OriginalUiArchive50KG::memberCount() const noexcept {
    return storage_ ? storage_->identity->memberCount : 0;
}
OriginalUiMemberView50KG OriginalUiArchive50KG::member(std::size_t ordinal) const noexcept {
    if (!storage_ || ordinal >= storage_->identity->memberCount) return {};
    const auto* identity = originalUiMemberIdentity50KG(storage_->identity->id, ordinal);
    if (!identity || identity->logicalOffset > storage_->decoded.size() ||
        identity->bytes > storage_->decoded.size() - identity->logicalOffset) return {};
    return {identity, storage_->decoded.data() + identity->logicalOffset, identity->bytes};
}
OriginalUiMemberView50KG OriginalUiArchive50KG::find(const std::string& name) const noexcept {
    if (!storage_) return {};
    for (std::size_t i = 0; i < storage_->identity->memberCount; ++i) {
        const auto view = member(i);
        if (sameName(name, view.identity->name)) return view;
    }
    return {};
}
const OriginalUiArchiveIdentity50KG* OriginalUiEncodedArchiveView50KG::identity() const noexcept {
    return archive_ ? archive_->identity() : nullptr;
}
const std::uint8_t* OriginalUiEncodedArchiveView50KG::data() const noexcept {
    return archive_ && archive_->storage_ ? archive_->storage_->encoded.data() : nullptr;
}
std::size_t OriginalUiEncodedArchiveView50KG::size() const noexcept {
    return archive_ && archive_->storage_ ? archive_->storage_->encoded.size() : 0;
}
std::size_t OriginalUiEncodedArchiveView50KG::blockCount() const noexcept {
    return archive_ && archive_->storage_ ? archive_->storage_->blocks.size() : 0;
}
bool OriginalUiEncodedArchiveView50KG::block(std::size_t i, ResourceBlock48C& out) const noexcept {
    out={};
    if(!archive_ || !archive_->storage_ || i>=archive_->storage_->blocks.size()) return false;
    out=archive_->storage_->blocks[i]; return true;
}

}

