#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "import_service48c.h"
#include "import_decode48c.h"
#include "scene_asset48d.h"
#include "import_texture48e.h"
#include "import_service48e.h"
#include "import_service48f.h"
#include "world_asset48f.h"
#include "import_collision48g.h"
#include "import_service48g.h"
#include "import_service48i.h"
#include "native_asset48b.h"
#include "json.hpp"
#include <windows.h>
#include <bcrypt.h>
#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace rr { namespace windows {
namespace {
using Bytes = std::vector<std::uint8_t>;
using Json = nlohmann::json;
namespace fs = std::filesystem;
constexpr char converter[] = "rr-native-content-48G-v1";
constexpr char collisionConverter[] = "rr-native-collision-48G-v1";
constexpr char worldConverter[] = "rr-native-world-48F-v1";
constexpr char sceneConverter[] = "rr-native-scene-48D-v1";
constexpr char textureConverter[] = "rr-native-texture-48E-v1";
constexpr char materialConverter[] = "rr-native-material-48E-v1";
constexpr char gameHash[] = "c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30";
constexpr std::uint64_t maxResource = 64ull * 1024 * 1024;
struct Cancelled {};
void require(bool ok, const std::string& error) { if (!ok) throw std::runtime_error(error); }
void checkCancel(const std::atomic<bool>& cancel) { if (cancel.load()) throw Cancelled{}; }
std::string upper(std::string text) {
    for (auto& c : text) if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');
    return text;
}
bool isHexHash(const std::string& text) {
    return text.size() == 64 && text.find_first_not_of("0123456789abcdef") == std::string::npos;
}
class Hash {
    BCRYPT_ALG_HANDLE algorithm_ = nullptr;
    BCRYPT_HASH_HANDLE hash_ = nullptr;
public:
    Hash() {
        require(BCryptOpenAlgorithmProvider(&algorithm_, BCRYPT_SHA256_ALGORITHM, nullptr, 0) >= 0,
            "Cannot open SHA256 provider");
        if (BCryptCreateHash(algorithm_, &hash_, nullptr, 0, nullptr, 0, 0) < 0) {
            BCryptCloseAlgorithmProvider(algorithm_, 0); algorithm_ = nullptr;
            throw std::runtime_error("Cannot create SHA256 state");
        }
    }
    ~Hash() { if (hash_) BCryptDestroyHash(hash_); if (algorithm_) BCryptCloseAlgorithmProvider(algorithm_, 0); }
    Hash(const Hash&) = delete;
    void add(const std::uint8_t* bytes, std::size_t size) {
        while (size) {
            const ULONG count = static_cast<ULONG>(std::min<std::size_t>(size, 1024 * 1024));
            require(BCryptHashData(hash_, const_cast<PUCHAR>(bytes), count, 0) >= 0, "SHA256 update failed");
            bytes += count; size -= count;
        }
    }
    std::string finish() {
        std::array<std::uint8_t, 32> digest{};
        require(BCryptFinishHash(hash_, digest.data(), static_cast<ULONG>(digest.size()), 0) >= 0, "SHA256 finish failed");
        std::ostringstream text;
        for (auto b : digest) text << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(b);
        return text.str();
    }
};
std::string sha(const Bytes& bytes) { Hash h; h.add(bytes.data(), bytes.size()); return h.finish(); }
std::string sha(const std::string& text) { Hash h; h.add(reinterpret_cast<const std::uint8_t*>(text.data()), text.size()); return h.finish(); }
Bytes readFile(const fs::path& path, std::uint64_t limit) {
    const auto size = fs::file_size(path);
    require(size <= limit, "File exceeds import limit: " + path.u8string());
    std::ifstream input(path, std::ios::binary);
    require(bool(input), "Cannot read: " + path.u8string());
    Bytes data(static_cast<std::size_t>(size));
    if (!data.empty()) input.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
    require(bool(input) && input.peek() == std::char_traits<char>::eof(), "Input changed or truncated: " + path.u8string());
    return data;
}
bool isReparse(const fs::path& path) {
    const DWORD attrs = GetFileAttributesW(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
}
void checkPlainPath(const fs::path& path) {
    fs::path prefix;
    for (const auto& part : fs::absolute(path)) {
        prefix /= part;
        require(!isReparse(prefix), "Import cache paths cannot traverse filesystem links: " + prefix.u8string());
    }
}
bool containsPath(const fs::path& parent, const fs::path& child) {
    const auto p = fs::weakly_canonical(parent), c = fs::weakly_canonical(child);
    auto pi = p.begin(), ci = c.begin();
    for (; pi != p.end(); ++pi, ++ci) {
        if (ci == c.end() || CompareStringOrdinal(pi->c_str(), -1, ci->c_str(), -1, TRUE) != CSTR_EQUAL) return false;
    }
    return true;
}
struct Handle {
    HANDLE value = INVALID_HANDLE_VALUE;
    explicit Handle(HANDLE h) : value(h) {}
    ~Handle() { if (value != INVALID_HANDLE_VALUE) CloseHandle(value); }
    Handle(const Handle&) = delete;
};
void atomicWrite(const fs::path& path, const Bytes& data) {
    checkPlainPath(path);
    static std::atomic<unsigned> serial{0};
    auto temp = path;
    temp += L".stage-" + std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(serial++);
    {
        Handle file(CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr));
        require(file.value != INVALID_HANDLE_VALUE, "Cannot create cache staging file");
        std::size_t offset = 0;
        while (offset < data.size()) {
            const DWORD requested = static_cast<DWORD>(std::min<std::size_t>(data.size() - offset, 1024 * 1024));
            DWORD written = 0;
            require(WriteFile(file.value, data.data() + offset, requested, &written, nullptr) && written == requested,
                "Cannot finish cache staging write");
            offset += written;
        }
        require(FlushFileBuffers(file.value) != 0, "Cannot flush cache staging file");
    }
    require(readFile(temp, assets::max_scene_bytes48d) == data, "Cache staging verification failed");
    require(MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0,
        "Cannot atomically promote cache file");
}
void atomicJson(const fs::path& path, const Json& value) {
    const auto text = value.dump(2) + "\n";
    atomicWrite(path, Bytes(text.begin(), text.end()));
}
Json readJson(const fs::path& path, std::uint64_t limit = 1024 * 1024) {
    const auto bytes = readFile(path, limit);
    return Json::parse(bytes.begin(), bytes.end());
}
std::uint32_t le32(const std::uint8_t* p) {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) | (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}
std::uint32_t be32(const std::uint8_t* p) {
    return std::uint32_t(p[3]) | (std::uint32_t(p[2]) << 8) | (std::uint32_t(p[1]) << 16) | (std::uint32_t(p[0]) << 24);
}
struct SourceEntry { std::string name; fs::path path; std::uint64_t offset = 0, size = 0; };
// Original images are read as files, not mounted or handed to any PS2 runtime.
class Source {
    fs::path root_;
    bool iso_ = false;
    std::ifstream image_;
    std::uint64_t imageSize_ = 0;
    fs::file_time_type imageTime_{};
    std::map<std::string, SourceEntry> entries_;
    std::set<std::uint64_t> directories_;
    std::vector<std::unique_ptr<Handle>> readLocks_;
    void lockOriginal(const fs::path& path) {
        auto lock = std::make_unique<Handle>(CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
            nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
        require(lock->value != INVALID_HANDLE_VALUE,
            "Cannot hold original data unchanged while importing: " + path.u8string());
        readLocks_.push_back(std::move(lock));
    }
    Bytes isoRead(std::uint64_t offset, std::uint64_t count) {
        require(count <= maxResource && offset <= imageSize_ && count <= imageSize_ - offset, "Invalid ISO extent");
        Bytes data(static_cast<std::size_t>(count));
        image_.clear(); image_.seekg(static_cast<std::streamoff>(offset));
        image_.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(count));
        require(bool(image_), "Truncated ISO extent"); return data;
    }
    void isoDirectory(std::uint64_t offset, std::uint64_t size, const std::string& prefix, unsigned depth,
        const std::atomic<bool>& cancel) {
        require(depth <= 12 && size <= 4 * 1024 * 1024 && directories_.insert(offset).second,
            "Unsupported ISO directory depth, size or cycle");
        const Bytes bytes = isoRead(offset, size);
        for (std::size_t at = 0; at < bytes.size();) {
            checkCancel(cancel);
            if (bytes[at] == 0) { at = (at / 2048 + 1) * 2048; continue; }
            const auto* r = bytes.data() + at;
            require(r[0] >= 34 && r[0] <= bytes.size() - at && r[0] <= 2048 - at % 2048,
                "Malformed ISO directory record");
            require(r[32] > 0 && 33u + r[32] <= r[0], "Malformed ISO identifier");
            require(r[1] == 0 && r[26] == 0 && r[27] == 0 && !(r[25] & 0x80), "Unsupported ISO extended/interleaved/multi-extent file");
            const std::uint64_t lba = le32(r + 2), fileSize = le32(r + 10);
            require(lba == be32(r + 6) && fileSize == be32(r + 14), "ISO endian extent mismatch");
            require(lba * 2048 <= imageSize_ && fileSize <= imageSize_ - lba * 2048, "ISO file outside image");
            std::string name(reinterpret_cast<const char*>(r + 33), r[32]);
            at += r[0];
            if (name.size() == 1 && (name[0] == 0 || name[0] == 1)) continue;
            const auto version = name.find(';');
            if (version != std::string::npos) { require(name.substr(version) == ";1", "Unsupported ISO file version"); name.resize(version); }
            require(!name.empty() && name != "." && name != ".." && name.find_first_of("/\\:\0", 0, 4) == std::string::npos,
                "Unsafe ISO identifier");
            for (unsigned char c : name) require(c >= 32 && c < 127, "Unsupported ISO identifier encoding");
            name = upper(prefix + name);
            require(entries_.size() < 10000, "ISO file count limit exceeded");
            if (r[25] & 2) isoDirectory(lba * 2048, fileSize, name + "/", depth + 1, cancel);
            else require(entries_.emplace(name, SourceEntry{name, {}, lba * 2048, fileSize}).second, "Duplicate ISO filename");
        }
    }
public:
    Source(const fs::path& root, const std::atomic<bool>& cancel) : root_(fs::absolute(root)) {
        require(fs::exists(root_), "Original data does not exist");
        if (fs::is_directory(root_)) {
            require(!isReparse(root_), "Select an original data directory without links");
            for (fs::recursive_directory_iterator it(root_), end; it != end; ++it) {
                checkCancel(cancel);
                require(it.depth() <= 12, "Original data directory depth limit exceeded");
                require(!isReparse(it->path()), "Original data contains a filesystem link");
                if (!it->is_regular_file()) continue;
                const auto name = upper(fs::relative(it->path(), root_).generic_u8string());
                require(entries_.size() < 10000, "Original data file count limit exceeded");
                require(entries_.emplace(name, SourceEntry{name, it->path(), 0, it->file_size()}).second,
                    "Duplicate original data filename");
            }
        } else {
            lockOriginal(root_);
            iso_ = true; imageSize_ = fs::file_size(root_); imageTime_ = fs::last_write_time(root_);
            require(imageSize_ >= 18 * 2048 && imageSize_ <= 8ull * 1024 * 1024 * 1024, "Unsupported ISO size");
            image_.open(root_, std::ios::binary); require(bool(image_), "Cannot open original ISO");
            const Bytes pvd = isoRead(16 * 2048, 2048);
            require(pvd[0] == 1 && std::string(reinterpret_cast<const char*>(pvd.data() + 1), 5) == "CD001" && pvd[6] == 1,
                "Select a cooked 2048-byte ISO or extracted game data; raw BIN images are not yet supported");
            require(pvd[128] == 0 && pvd[129] == 8 && pvd[130] == 8 && pvd[131] == 0, "Unsupported ISO sector size");
            require(le32(pvd.data() + 80) == be32(pvd.data() + 84) && std::uint64_t(le32(pvd.data() + 80)) * 2048 <= imageSize_,
                "Invalid ISO volume size");
            const auto* r = pvd.data() + 156;
            require(r[0] == 34 && r[1] == 0 && (r[25] & 2) && !(r[25] & 0x80) && r[26] == 0 && r[27] == 0,
                "Invalid ISO root record");
            require(le32(r + 2) == be32(r + 6) && le32(r + 10) == be32(r + 14), "ISO root endian mismatch");
            isoDirectory(std::uint64_t(le32(r + 2)) * 2048, le32(r + 10), "", 0, cancel);
        }
        const auto original = entries_.find("SLUS_202.68");
        require(original != entries_.end() && sha(read(original->second, 16 * 1024 * 1024)) == gameHash,
            "This importer currently supports the verified SLUS_202.68 original game revision");
    }
    Bytes read(const SourceEntry& entry, std::uint64_t limit = maxResource) {
        require(entry.size <= limit, "Original file exceeds import limit: " + entry.name);
        if (iso_) return isoRead(entry.offset, entry.size);
        // Keep read sharing but deny writes/deletion until the active generation
        // has committed. A pre-existing writer also prevents this lock opening.
        lockOriginal(entry.path);
        const auto time = fs::last_write_time(entry.path);
        const Bytes bytes = readFile(entry.path, limit);
        require(bytes.size() == entry.size && time == fs::last_write_time(entry.path), "Original data changed while reading");
        return bytes;
    }
    std::vector<SourceEntry> resources(const std::string& selection) const {
        std::vector<SourceEntry> result;
        const auto wanted = upper(selection);
        for (const auto& item : entries_) {
            const auto& name = item.first;
            if (name.size() >= 4 && name.substr(name.size() - 4) == ".RES" && (wanted.empty() || name == wanted))
                result.push_back(item.second);
        }
        require(!result.empty(), "No matching original resource containers found"); return result;
    }
    Json description(std::atomic<bool>& cancel) {
        Json result{{"path", root_.u8string()}, {"type", iso_ ? "iso9660-2048" : "extracted-directory"}, {"game_sha256", gameHash}};
        if (iso_) {
            Hash h;
            for (std::uint64_t off = 0; off < imageSize_;) {
                checkCancel(cancel); const auto bytes = isoRead(off, std::min<std::uint64_t>(1024 * 1024, imageSize_ - off));
                h.add(bytes.data(), bytes.size()); off += bytes.size();
            }
            result["image_bytes"] = imageSize_; result["image_sha256"] = h.finish();
        }
        return result;
    }
    void verifyUnchanged() const {
        if (iso_) require(fs::file_size(root_) == imageSize_ && fs::last_write_time(root_) == imageTime_, "Original ISO changed during import");
    }
};
struct HookContext {
    std::atomic<bool>* cancel;
    const std::function<void(const ImportProgress48C&)>* callback;
    ImportProgress48C* state;
};
bool cancelledHook(void* opaque) { return static_cast<HookContext*>(opaque)->cancel->load(); }
void progressHook(void* opaque, const char* stage, std::uint32_t done, std::uint32_t total) {
    auto& h = *static_cast<HookContext*>(opaque);
    if (*h.callback) {
        auto progress = *h.state; progress.stage = stage; progress.completed = done; progress.total = total;
        (*h.callback)(progress);
    }
}
bool loadCached(const fs::path& cache, const std::string& key, const std::string& sourceHash, Json& record) {
    try {
        require(isHexHash(key) && isHexHash(sourceHash) && key == sha(std::string(sceneConverter) + "\n" + sourceHash), "Invalid scene key");
        const auto object = cache / "objects" / (key + ".rrmesh"), meta = cache / "objects" / (key + ".json");
        checkPlainPath(object); checkPlainPath(meta);
        auto candidate = readJson(meta);
        const std::string recordHash = candidate.at("record_sha256");
        auto fields = candidate; fields.erase("record_sha256");
        require(isHexHash(recordHash) && sha(fields.dump()) == recordHash, "Cached metadata hash mismatch");
        require(candidate.at("converter") == sceneConverter && candidate.at("source_sha256") == sourceHash && candidate.at("key") == key,
            "Cache metadata mismatch");
        const auto bytes = readFile(object, maxResource + 16);
        require(candidate.at("mesh_sha256") == sha(bytes), "Cached asset hash mismatch");
        assets::Mesh48B parsed; std::string error;
        require(assets::parseMesh48B(bytes.data(), bytes.size(), parsed, error), "Cached asset structure invalid");
        require(candidate.at("vertices") == parsed.vertices.size() && candidate.at("triangles") == parsed.indices.size() / 3,
            "Cached asset count mismatch");
        const auto scenePath = cache / "objects" / (key + ".rrscene"); checkPlainPath(scenePath);
        const auto semanticBytes = readFile(scenePath, assets::max_scene_bytes48d);
        require(candidate.at("scene_sha256") == sha(semanticBytes), "Cached scene hash mismatch");
        assets::Scene48D scene;
        require(assets::parseSceneAsset48D(semanticBytes, scene, error), "Cached scene structure invalid: " + error);
        require(scene.neutral_lod0 == bytes && candidate.at("world_space") == scene.world_space &&
            candidate.at("materials") == scene.materials && candidate.at("nodes") == scene.nodes.size() &&
            candidate.at("lods") == scene.lods.size() && candidate.at("vertices_all_lods") == scene.vertices_all_lods &&
            candidate.at("surfaces_all_lods") == scene.surfaces_all_lods, "Cached scene metadata mismatch");
        record = std::move(candidate); return true;
    } catch (const std::exception&) { return false; }
}
bool loadCollision(const fs::path& cache, const std::string& key, const std::string& sourceHash, Json& record) {
    try {
        require(isHexHash(key) && isHexHash(sourceHash) && key == sha(std::string(collisionConverter) + "\n" + sourceHash), "Invalid collision key");
        const auto meta = cache / "objects" / (key + ".json"); checkPlainPath(meta);
        auto candidate = readJson(meta), fields = candidate;
        const std::string digest = fields.at("record_sha256"); fields.erase("record_sha256");
        require(isHexHash(digest) && sha(fields.dump()) == digest && candidate.at("converter") == collisionConverter &&
            candidate.at("key") == key && candidate.at("source_sha256") == sourceHash && candidate.at("type") == "collision", "Collision metadata mismatch");
        const auto path = cache / "objects" / (key + ".rrcol"); checkPlainPath(path);
        const auto bytes = readFile(path, assets::max_collision_asset_bytes48g);
        require(candidate.at("collision_sha256") == sha(bytes), "Collision content hash mismatch");
        assets::Collision48G collision; std::string error;
        require(assets::parseCollisionAsset48G(bytes, collision, error), "Invalid native collision: " + error);
        require(sha(collision.original_bytes) == sourceHash && candidate.at("vertices") == collision.vertices.size() &&
            candidate.at("nodes") == collision.nodes.size() && candidate.at("faces") == collision.faces.size() &&
            candidate.at("normals_present") == collision.normals_present, "Collision provenance/count mismatch");
        record = std::move(candidate); return true;
    } catch (const std::exception&) { return false; }
}
bool convertCollision(const fs::path& cache, const Bytes& bytes, const std::string& sourceHash,
    Json& record, assets::DecodeError48C& error, const assets::DecodeHooks48C* hooks) {
    assets::Collision48G collision;
    if (!assets::decodeCollision48G(bytes, collision, error, hooks)) return false;
    Bytes native; std::string problem;
    require(assets::encodeCollisionAsset48G(collision, native, problem), "Native collision encoding failed: " + problem);
    const auto key = sha(std::string(collisionConverter) + "\n" + sourceHash);
    Json result{{"converter", collisionConverter}, {"key", key}, {"source_sha256", sourceHash}, {"type", "collision"},
        {"collision_sha256", sha(native)}, {"vertices", collision.vertices.size()}, {"nodes", collision.nodes.size()},
        {"faces", collision.faces.size()}, {"normals_present", collision.normals_present}};
    result["record_sha256"] = sha(result.dump());
    atomicWrite(cache / "objects" / (key + ".rrcol"), native);
    atomicJson(cache / "objects" / (key + ".json"), result);
    record = std::move(result); return true;
}
bool loadWorld(const fs::path& cache, const std::string& key, const std::string& sourceHash, Json& record) {
    try {
        require(isHexHash(key) && isHexHash(sourceHash) && key == sha(std::string(worldConverter) + "\n" + sourceHash), "Invalid world key");
        const auto meta = cache / "objects" / (key + ".json"); checkPlainPath(meta);
        auto candidate = readJson(meta), fields = candidate;
        const std::string digest = fields.at("record_sha256"); fields.erase("record_sha256");
        require(isHexHash(digest) && sha(fields.dump()) == digest && candidate.at("converter") == worldConverter &&
            candidate.at("key") == key && candidate.at("source_sha256") == sourceHash && candidate.at("type") == "world", "World metadata mismatch");
        const auto path = cache / "objects" / (key + ".rrworld"); checkPlainPath(path);
        const auto bytes = readFile(path, assets::max_world_asset_bytes48f);
        require(candidate.at("world_sha256") == sha(bytes), "World content hash mismatch");
        assets::World48F world; std::string error;
        require(assets::parseWorldAsset48F(bytes, world, error), "Invalid native world: " + error);
        require(sha(world.parameters.original_bytes) == sourceHash && candidate.at("zones") == world.zones.size() &&
            candidate.at("portals") == world.portals.size() && candidate.at("instances") == world.instances.size() &&
            candidate.at("parameter_blocks") == world.parameters.blocks.size(), "World provenance/count mismatch");
        record = std::move(candidate); return true;
    } catch (const std::exception&) { return false; }
}
bool convertWorld(const fs::path& cache, const Bytes& bytes, const std::string& sourceHash,
    Json& record, assets::DecodeError48C& error, const assets::DecodeHooks48C* hooks) {
    assets::World48F world;
    if (!assets::decodeWorld48F(bytes, world, error, hooks)) return false;
    Bytes native; std::string problem;
    require(assets::encodeWorldAsset48F(world, native, problem), "Native world encoding failed: " + problem);
    const auto key = sha(std::string(worldConverter) + "\n" + sourceHash);
    Json result{{"converter", worldConverter}, {"key", key}, {"source_sha256", sourceHash}, {"type", "world"},
        {"world_sha256", sha(native)}, {"zones", world.zones.size()}, {"portals", world.portals.size()},
        {"instances", world.instances.size()}, {"parameter_blocks", world.parameters.blocks.size()}};
    result["record_sha256"] = sha(result.dump());
    atomicWrite(cache / "objects" / (key + ".rrworld"), native);
    atomicJson(cache / "objects" / (key + ".json"), result);
    record = std::move(result); return true;
}
Json materialFields(const assets::MaterialFields48E& input) {
    Json result{{"authored", Json::array()}, {"unspecified", Json::array()}};
    for (const auto* fields : {&input.authored, &input.unspecified}) {
        auto& list = result[fields == &input.authored ? "authored" : "unspecified"];
        for (const auto& f : *fields) list.push_back(Json{{"name", f.name}, {"value", f.value}, {"source_offset", f.source_offset}});
    }
    return result;
}
bool loadContent(const fs::path& cache, const std::string& key, const std::string& sourceHash,
    const std::string& version, Json& record) {
    try {
        require(isHexHash(key) && isHexHash(sourceHash) && key == sha(version + "\n" + sourceHash), "Invalid content key");
        const auto meta = cache / "objects" / (key + ".json"); checkPlainPath(meta);
        auto candidate = readJson(meta); auto fields = candidate;
        const std::string digest = fields.at("record_sha256"); fields.erase("record_sha256");
        require(isHexHash(digest) && sha(fields.dump()) == digest && candidate.at("converter") == version &&
            candidate.at("key") == key && candidate.at("source_sha256") == sourceHash, "Content metadata mismatch");
        if (version == textureConverter) {
            require(candidate.at("type") == "texture" && candidate.at("mips").is_array(), "Invalid texture metadata");
            const auto& mips = candidate.at("mips"); require(!mips.empty() && mips.size() <= 16, "Invalid mip count");
            unsigned lastW = 0, lastH = 0, index = 0;
            for (const auto& m : mips) {
                const auto path = cache / "objects" / (key + "-mip" + std::to_string(index) + ".rrtex"); checkPlainPath(path);
                const auto bytes = readFile(path, assets::max_asset_file_bytes);
                require(m.at("index") == index && m.at("sha256") == sha(bytes), "Mip hash mismatch");
                assets::Texture48B parsed; std::string error;
                require(assets::parseTexture48B(bytes.data(), bytes.size(), parsed, error), "Invalid native mip");
                require(m.at("width") == parsed.width && m.at("height") == parsed.height &&
                    (!index || (parsed.width == lastW * 2 && parsed.height == lastH * 2)), "Invalid mip dimensions");
                lastW = parsed.width; lastH = parsed.height; ++index;
            }
        } else {
            require(version == materialConverter && candidate.at("type") == "material", "Invalid material version");
            const auto& passes = candidate.at("passes"); require(passes.is_array() && !passes.empty() && passes.size() <= 16, "Invalid pass count");
            unsigned pass = 0;
            for (const auto& p : passes) {
                require(p.at("pass") == ++pass && p.at("program").is_string() && p.at("program").get<std::string>().size() <= 1024,
                    "Invalid material program");
                const auto& slots = p.at("textures"); require(slots.is_array() && slots.size() <= 16, "Invalid texture slots");
                unsigned slot = 0;
                for (const auto& t : slots) require(t.at("slot") == ++slot && t.at("name").is_string() &&
                    !t.at("name").get<std::string>().empty() && t.at("name").get<std::string>().size() <= 1024, "Invalid texture name");
            }
        }
        record = std::move(candidate); return true;
    } catch (const std::exception&) { return false; }
}
bool convertContent(const fs::path& cache, const Bytes& bytes, const std::string& sourceHash,
    bool texture, Json& record, assets::DecodeError48C& error, const assets::DecodeHooks48C* hooks) {
    const std::string version = texture ? textureConverter : materialConverter;
    const auto key = sha(version + "\n" + sourceHash);
    Json result{{"converter", version}, {"key", key}, {"source_sha256", sourceHash}, {"type", texture ? "texture" : "material"}};
    if (texture) {
        assets::Texture48E decoded;
        if (!assets::decodeTexture48E(bytes, decoded, error, hooks)) return false;
        result["mips"] = Json::array(); unsigned index = 0;
        for (const auto& mip : decoded.mips) {
            Bytes native; require(assets::encodeTextureLevel48E(mip, native, error), "Native mip encoding failed");
            atomicWrite(cache / "objects" / (key + "-mip" + std::to_string(index) + ".rrtex"), native);
            result["mips"].push_back(Json{{"index", index++}, {"width", mip.width}, {"height", mip.height}, {"sha256", sha(native)}});
        }
    } else {
        assets::Material48E decoded;
        if (!assets::decodeMaterial48E(bytes, decoded, error, hooks)) return false;
        result["general"] = materialFields(decoded.general); result["passes"] = Json::array();
        for (const auto& p : decoded.passes) {
            Json pass{{"pass", p.pass}, {"program", p.microprogram}, {"texture_count_defaulted", p.texture_count_defaulted},
                {"fields", materialFields(p.fields)}, {"textures", Json::array()}};
            for (const auto& t : p.textures) pass["textures"].push_back(Json{{"slot", t.slot}, {"name", t.name}});
            result["passes"].push_back(std::move(pass));
        }
    }
    result["record_sha256"] = sha(result.dump()); atomicJson(cache / "objects" / (key + ".json"), result);
    record = std::move(result); return true;
}
}

namespace {
Json activeManifest48F(const fs::path& cache) {
        checkPlainPath(cache);
        const auto current = readJson(cache / "current.json"); const std::string generation = current.at("generation");
        require(current.at("schema") == "RRCURRENT48C", "Unsupported active cache schema");
        require(isHexHash(generation) && current.at("manifest_sha256") == generation, "Invalid active generation");
        const auto manifestPath = cache / "generations" / (generation + ".json"); checkPlainPath(manifestPath);
        const auto bytes = readFile(manifestPath, assets::max_scene_bytes48d);
        require(sha(bytes) == generation, "Active manifest hash mismatch");
        const auto manifest = Json::parse(bytes.begin(), bytes.end());
        require(manifest.at("schema") == "RRIMPORT48C" && manifest.at("members").is_array(), "Unsupported native manifest");
        return manifest;
}
using MemberIndex48F = std::map<std::string, std::vector<const Json*>>;
bool loadSceneManifest48F(const fs::path& cache, const MemberIndex48F& memberIndex, std::map<std::string, Json>& verifiedObjects,
    const std::string& container, const std::string& member, RuntimeScene48E& output, std::string& error) {
    try {
        auto normal = [](std::string value) { std::replace(value.begin(), value.end(), '\\', '/'); return upper(value); };
        const Json* selected = nullptr;
        const auto sceneMatches = memberIndex.find(normal(member));
        if (sceneMatches != memberIndex.end()) for (const auto* m : sceneMatches->second) if (normal(m->at("container")) == normal(container)) {
            require(!selected, "Ambiguous native scene identity"); selected = m;
        }
        require(selected && selected->at("status") == "converted_geometry", "Scene is not in the active native cache");
        auto validated = [&](const Json& m) {
            const auto& record = m.at("native"); const std::string key = record.at("key"), source = m.at("source_sha256");
            require(isHexHash(key) && isHexHash(source) && record.at("source_sha256") == source, "Invalid native object identity");
            const auto previous = verifiedObjects.find(key);
            if (previous != verifiedObjects.end()) {
                require(previous->second == record, "Conflicting native object metadata");
                return previous->second;
            }
            Json actual;
            const bool valid = record.at("converter") == sceneConverter ? loadCached(cache, key, source, actual) :
                loadContent(cache, key, source, record.at("converter"), actual);
            require(valid && actual == record, "Native object validation failed: " + m.at("member").get<std::string>());
            verifiedObjects.emplace(key, actual);
            return actual;
        };
        RuntimeScene48E loaded; const auto sceneRecord = validated(*selected);
        const auto sceneBytes = readFile(cache / "objects" / (sceneRecord.at("key").get<std::string>() + ".rrscene"), assets::max_scene_bytes48d);
        require(assets::parseSceneAsset48D(sceneBytes, loaded.scene, error), "Invalid native scene: " + error);
        auto resolve = [&](const std::string& name, const std::string& extension, const std::string& status) -> const Json* {
            const auto wanted = normal("textures/" + name + extension);
            std::vector<const Json*> local, global;
            const auto candidates = memberIndex.find(wanted);
            if (candidates != memberIndex.end()) for (const auto* m : candidates->second) {
                global.push_back(m); if (normal(m->at("container")) == normal(container)) local.push_back(m);
            }
            const auto& matches = local.empty() ? global : local;
            if (matches.empty()) { loaded.warnings.push_back("Unresolved " + wanted); return nullptr; }
            const std::string source = matches.front()->at("source_sha256");
            for (const auto* m : matches) if (m->at("source_sha256") != source) {
                loaded.warnings.push_back("Ambiguous " + wanted); return nullptr;
            }
            // A present original local resource owns its name, even when its
            // format is not converted. Never substitute another track's art.
            for (const auto* m : matches) if (m->at("status") != status) {
                loaded.warnings.push_back("Unresolved " + wanted + " (original resource unsupported)"); return nullptr;
            }
            return matches.front();
        };
        std::map<std::string, int> textureIndices;
        std::size_t residentTextureBytes = 0;
        for (const auto& name : loaded.scene.materials) {
            RuntimeMaterial48E material; material.name = name;
            const auto* m = resolve(name, ".psm", "converted_material");
            if (m) {
                const auto record = validated(*m);
                for (const auto& p : record.at("passes")) {
                    RuntimeMaterialPass48E pass; pass.program = p.at("program");
                    for (const auto& t : p.at("textures")) {
                        RuntimeTextureSlot48E slot; slot.slot = t.at("slot"); slot.name = t.at("name");
                        const auto* textureMember = resolve(slot.name, ".pst", "converted_texture");
                        if (textureMember) {
                            const auto textureRecord = validated(*textureMember);
                            const std::string key = textureRecord.at("key");
                            auto found = textureIndices.find(key);
                            if (found != textureIndices.end()) slot.texture = found->second;
                            else {
                                RuntimeTexture48E texture; texture.source_name = textureMember->at("member");
                                for (const auto& mip : textureRecord.at("mips")) {
                                    const auto mipBytes = readFile(cache / "objects" / (key + "-mip" + std::to_string(mip.at("index").get<unsigned>()) + ".rrtex"), assets::max_asset_file_bytes);
                                    assets::Texture48B level;
                                    require(assets::parseTexture48B(mipBytes.data(), mipBytes.size(), level, error), "Invalid cached texture");
                                    residentTextureBytes += level.rgba.size();
                                    require(residentTextureBytes <= 256u * 1024u * 1024u, "Scene texture memory limit");
                                    texture.levels.push_back(std::move(level));
                                }
                                slot.texture = static_cast<int>(loaded.textures.size()); textureIndices.emplace(key, slot.texture);
                                loaded.textures.push_back(std::move(texture));
                            }
                        }
                        pass.textures.push_back(std::move(slot));
                    }
                    material.passes.push_back(std::move(pass));
                }
            }
            loaded.materials.push_back(std::move(material));
        }
        output = std::move(loaded); error.clear(); return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}


} // namespace
bool loadCachedCollisionWorld48G(const fs::path& cache,const std::string& container,const std::string& member,
    RuntimeCollisionWorld48G& output,std::string& error) {
    try {
        const auto manifest=activeManifest48F(cache);
        auto normal=[](std::string value){std::replace(value.begin(),value.end(),'\\','/');return upper(value);};
        std::map<std::string,const Json*> members;
        for(const auto& m:manifest.at("members"))if(normal(m.at("container"))==normal(container))
            require(members.emplace(normal(m.at("member")),&m).second,"Ambiguous collision world member");
        const auto selected=members.find(normal(member));
        require(selected!=members.end()&&selected->second->at("status")=="converted_world","World is not in the active native cache");
        const auto& record=selected->second->at("native");Json verified;
        const std::string key=record.at("key"),source=selected->second->at("source_sha256");
        require(loadWorld(cache,key,source,verified)&&verified==record,"Native world validation failed");
        assets::World48F world;
        require(assets::parseWorldAsset48F(readFile(cache/"objects"/(key+".rrworld"),assets::max_world_asset_bytes48f),world,error),"Invalid collision world: "+error);
        // Original 2764C4..276508 selects static models when a separate collision
        // list is absent; 27BDD4/27A9D4 select default Standard/SLT fallbacks.
        // Preserve explicit unsupported routes rather than infer override semantics.
        for(const auto& block:world.parameters.blocks)for(const auto& field:block.fields) {
            const auto name=upper(field.name);
            require(name!="NUMBEROFCOLLISIONOBJECTS"&&name!="STATICTYPE"&&name!="COL",
                "Authored collision override is not yet supported: "+block.name+"/"+field.name);
        }
        RuntimeCollisionWorld48G result;result.original_instances=static_cast<std::uint32_t>(world.instances.size());
        std::map<std::string,std::uint32_t> loaded;
        std::map<std::string,Json> verifiedRecords;
        for(std::uint32_t i=0;i<world.instances.size();++i) {
            const auto& instance=world.instances[i];const auto found=members.find(normal(instance.collision_member));
            if(found==members.end()){++result.missing_original_members;continue;}
            const auto& m=*found->second;
            require(m.at("status")=="converted_collision","Original collision is present but unconverted: "+instance.collision_member);
            const auto& metadata=m.at("native");const std::string assetKey=metadata.at("key"),assetSource=m.at("source_sha256");
            auto previous=loaded.find(assetKey);std::uint32_t assetIndex;
            if(previous==loaded.end()) {
                Json checked;require(loadCollision(cache,assetKey,assetSource,checked)&&checked==metadata,"Collision cache validation failed: "+instance.collision_member);
                verifiedRecords.emplace(assetKey,checked);
                assets::Collision48G collision;
                require(assets::parseCollisionAsset48G(readFile(cache/"objects"/(assetKey+".rrcol"),assets::max_collision_asset_bytes48g),collision,error),error);
                result.retained_payload_bytes+=sizeof(collision)+collision.original_bytes.size()+collision.vertices.size()*sizeof(collision.vertices[0])+
                    collision.vertex_normals.size()*sizeof(collision.vertex_normals[0])+collision.nodes.size()*sizeof(assets::CollisionNode48G)+collision.faces.size()*sizeof(assets::CollisionFace48G);
                require(result.retained_payload_bytes<=256u*1024u*1024u,"Collision world retained payload limit");
                assetIndex=static_cast<std::uint32_t>(result.assets.size());loaded.emplace(assetKey,assetIndex);result.assets.push_back(std::move(collision));
            } else {
                assetIndex=previous->second;
                require(verifiedRecords.at(assetKey)==metadata&&sha(result.assets[assetIndex].original_bytes)==assetSource,"Collision alias provenance mismatch");
            }
            PlacedCollision48G placed;placed.source_instance=i;placed.asset=assetIndex;std::array<double,16> placement;
            require(assets::collisionPlacement48G(instance.position,instance.look,instance.up,placement,error),error);
            require(assets::collisionWorldMatrix48G(result.assets[assetIndex],placement,placed.matrix,error),error);
            result.instances.push_back(placed);
        }
        result.retained_payload_bytes+=result.instances.size()*sizeof(PlacedCollision48G);
        output=std::move(result);error.clear();return true;
    }catch(const std::exception& e){error=e.what();return false;}
}
bool loadCachedCollisionWorld48I(const fs::path& cache,const std::string& container,const std::string& member,
    RuntimeCollisionWorld48I& output,std::string& error) {
    try {
        const auto manifest=activeManifest48F(cache);
        auto normal=[](std::string value){std::replace(value.begin(),value.end(),'\\','/');return upper(value);};
        std::map<std::string,const Json*> members;
        for(const auto& m:manifest.at("members"))if(normal(m.at("container"))==normal(container))
            require(members.emplace(normal(m.at("member")),&m).second,"Ambiguous collision world member");
        const auto selected=members.find(normal(member));
        require(selected!=members.end()&&selected->second->at("status")=="converted_world","World is not in the active native cache");
        const auto& record=selected->second->at("native");Json verified;
        const std::string key=record.at("key"),source=selected->second->at("source_sha256");
        require(loadWorld(cache,key,source,verified)&&verified==record,"Native world validation failed");
        assets::World48F world;
        require(assets::parseWorldAsset48F(readFile(cache/"objects"/(key+".rrworld"),assets::max_world_asset_bytes48f),world,error),"Invalid collision world: "+error);
        for(const auto& block:world.parameters.blocks)for(const auto& field:block.fields) {
            const auto name=upper(field.name);
            require(name!="NUMBEROFCOLLISIONOBJECTS"&&name!="STATICTYPE"&&name!="COL",
                "Authored collision override is not yet supported: "+block.name+"/"+field.name);
        }
        RuntimeCollisionWorld48I result;result.original_instances=static_cast<std::uint32_t>(world.instances.size());
        std::map<std::string,std::uint32_t> loaded;
        std::map<std::string,Json> verifiedRecords;
        std::map<std::string,std::string> verifiedSources;
        for(std::uint32_t i=0;i<world.instances.size();++i) {
            const auto& instance=world.instances[i];const auto found=members.find(normal(instance.collision_member));
            if(found==members.end()){++result.missing_original_members;continue;}
            const auto& m=*found->second;
            require(m.at("status")=="converted_collision","Original collision is present but unconverted: "+instance.collision_member);
            const auto& metadata=m.at("native");const std::string assetKey=metadata.at("key"),assetSource=m.at("source_sha256");
            auto previous=loaded.find(assetKey);std::uint32_t assetIndex;
            if(previous==loaded.end()) {
                Json checked;require(loadCollision(cache,assetKey,assetSource,checked)&&checked==metadata,"Collision cache validation failed: "+instance.collision_member);
                verifiedRecords.emplace(assetKey,checked);verifiedSources.emplace(assetKey,assetSource);
                assets::CollisionMesh48I collision;
                require(assets::loadCollisionMesh48I(readFile(cache/"objects"/(assetKey+".rrcol"),assets::max_collision_asset_bytes48g),collision,error),error);
                result.retained_payload_bytes+=sizeof(collision)+collision.vertices.size()*sizeof(collision.vertices[0])+
                    collision.vertex_normals.size()*sizeof(collision.vertex_normals[0])+collision.nodes.size()*sizeof(assets::CollisionMeshNode48I)+collision.faces.size()*sizeof(assets::CollisionMeshFace48I);
                require(result.retained_payload_bytes<=256u*1024u*1024u,"Collision world retained payload limit");
                assetIndex=static_cast<std::uint32_t>(result.assets.size());loaded.emplace(assetKey,assetIndex);result.assets.push_back(std::move(collision));
            } else {
                assetIndex=previous->second;
                require(verifiedRecords.at(assetKey)==metadata&&verifiedSources.at(assetKey)==assetSource,"Collision alias provenance mismatch");
            }
            PlacedCollision48I placed;placed.source_instance=i;placed.asset=assetIndex;
            std::array<float,3> position,look,up;
            for(unsigned j=0;j<3;++j){position[j]=static_cast<float>(instance.position[j]);look[j]=static_cast<float>(instance.look[j]);up[j]=static_cast<float>(instance.up[j]);}
            std::array<float,16> placement;
            require(assets::placementCollision48I(position,look,up,placement),"Invalid native collision placement");
            require(assets::composeCollisionWorld48I(result.assets[assetIndex].local_matrix,placement,placed.matrix),"Invalid native collision world matrix");
            result.instances.push_back(placed);
        }
        result.retained_payload_bytes+=result.instances.size()*sizeof(PlacedCollision48I);
        output=std::move(result);error.clear();return true;
    }catch(const std::exception& e){error=e.what();return false;}
}
bool loadCachedWorld48F(const fs::path& cache, const std::string& container, const std::string& member,
    assets::World48F& output, std::vector<bool>& collisionOnly, std::string& error) {
    try {
        const auto manifest = activeManifest48F(cache);
        auto normal = [](std::string value) { std::replace(value.begin(), value.end(), '\\', '/'); return upper(value); };
        std::map<std::string, const Json*> members;
        for (const auto& m : manifest.at("members")) if (normal(m.at("container")) == normal(container)) {
            require(members.emplace(normal(m.at("member")), &m).second, "Ambiguous native world member");
        }
        const auto selected = members.find(normal(member));
        require(selected != members.end() && selected->second->at("status") == "converted_world", "World is not in the active native cache");
        const auto& m = *selected->second; const auto& record = m.at("native");
        const std::string key = record.at("key"), source = m.at("source_sha256"); Json verified;
        require(loadWorld(cache, key, source, verified) && verified == record, "Native world validation failed");
        assets::World48F world;
        const auto bytes = readFile(cache / "objects" / (key + ".rrworld"), assets::max_world_asset_bytes48f);
        require(assets::parseWorldAsset48F(bytes, world, error), "Native world parse failed: " + error);
        std::vector<bool> collision; collision.reserve(world.instances.size());
        for (const auto& instance : world.instances) {
            const auto geometry = members.find(normal(instance.geometry_member));
            if (geometry != members.end()) {
                require(geometry->second->at("status") == "converted_geometry", "Unconverted world geometry: " + instance.geometry_member);
                collision.push_back(false);
            } else {
                const auto col = members.find(normal(instance.collision_member));
                require(col != members.end() && isHexHash(col->second->at("source_sha256")) && col->second->at("source_bytes").get<std::uint64_t>() > 0,
                    "Unresolved world geometry/collision: " + instance.geometry_member);
                collision.push_back(true);
            }
        }
        output = std::move(world); collisionOnly = std::move(collision); error.clear(); return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}
bool loadCachedScenes48F(const fs::path& cache, const std::string& container,
    const std::vector<std::string>& members, std::vector<RuntimeScene48E>& output, std::string& error) {
    try {
        require(members.size() <= 10000, "World unique geometry count limit");
        const auto manifest = activeManifest48F(cache);
        std::map<std::string, Json> verified;
        MemberIndex48F memberIndex;
        for (const auto& m : manifest.at("members")) {
            std::string name = m.at("member"); std::replace(name.begin(), name.end(), '\\', '/');
            memberIndex[upper(name)].push_back(&m);
        }
        std::vector<RuntimeScene48E> loaded; loaded.reserve(members.size());
        std::uint64_t retainedBytes = 0;
        for (const auto& member : members) {
            RuntimeScene48E scene;
            require(loadSceneManifest48F(cache, memberIndex, verified, container, member, scene, error), error);
            // Bound retained semantic and texture payloads across the entire world,
            // not just within each independently bounded scene.
            std::uint64_t payload = sizeof(scene) + scene.scene.neutral_lod0.size();
            payload += scene.scene.nodes.size() * sizeof(assets::SceneNode48D) + scene.scene.spans_lod0.size() * sizeof(assets::SceneSpan48D);
            for (const auto& lod : scene.scene.lods) for (const auto& surface : lod.surfaces)
                payload += sizeof(surface) + surface.vertices.size() * sizeof(assets::SceneVertex48D) + surface.indices.size() * 4;
            for (const auto& t : scene.textures) for (const auto& level : t.levels) payload += level.rgba.size();
            retainedBytes += payload;
            require(retainedBytes <= 512ull * 1024 * 1024, "World retained scene payload limit");
            loaded.push_back(std::move(scene));
        }
        output = std::move(loaded); error.clear(); return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}
bool loadCachedScene48E(const fs::path& cache, const std::string& container,
    const std::string& member, RuntimeScene48E& output, std::string& error) {
    std::vector<RuntimeScene48E> loaded;
    if (!loadCachedScenes48F(cache, container, {member}, loaded, error)) return false;
    output = std::move(loaded.front()); return true;
}

ImportResult48C importGame48C(const ImportOptions48C& options, std::atomic<bool>& cancel,
    const std::function<void(const ImportProgress48C&)>& callback) {
    ImportResult48C result;
    try {
        checkCancel(cancel);
        require(!options.source.empty() && !options.cache.empty(), "Original data and cache paths are required");
        const auto cache = fs::absolute(options.cache).lexically_normal();
        checkPlainPath(cache);
        require(!containsPath(options.source, cache) && !containsPath(cache, options.source),
            "Keep the native cache separate from the original game data");
        ImportProgress48C progress;
        auto publish = [&](const char* stage, const std::string& item, std::uint32_t done, std::uint32_t total) {
            progress.stage = stage; progress.item = item; progress.completed = done; progress.total = total;
            progress.converted = result.converted; progress.reused = result.reused; progress.unsupported = result.unsupported;
            if (callback) callback(progress);
            checkCancel(cancel);
        };
        publish("Validating original data", options.source.u8string(), 0, 0);
        Source source(options.source, cancel);
        auto resources = source.resources(options.container);
        publish("Hashing original data", options.source.u8string(), 0, 0);
        Json sourceDescription = source.description(cancel);
        if (fs::exists(cache) && !fs::is_empty(cache)) {
            require(fs::exists(cache / "cache-format.json"), "Selected cache folder contains unrelated files");
            require(readJson(cache / "cache-format.json").at("schema") == "RRCACHE48C", "Unsupported cache schema");
        }
        fs::create_directories(cache);
        Handle lock(CreateFileW((cache / "import.lock").c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr));
        require(lock.value != INVALID_HANDLE_VALUE, "Another import is already using this cache");
        if (!fs::exists(cache / "cache-format.json")) atomicJson(cache / "cache-format.json", Json{{"schema", "RRCACHE48C"}});
        checkPlainPath(cache / "objects"); checkPlainPath(cache / "generations");
        fs::create_directories(cache / "objects"); fs::create_directories(cache / "generations");
        Json manifest{{"schema", "RRIMPORT48C"}, {"converter", converter}, {"source", sourceDescription},
            {"scope", "Rigid scene geometry, native textures, authored materials, static world placements and collision trees; unsupported assets are listed. Not a complete playable game cache."},
            {"containers", Json::array()}, {"members", Json::array()}};
        HookContext hooksContext{&cancel, &callback, &progress};
        assets::DecodeHooks48C hooks{&hooksContext, cancelledHook, progressHook};
        for (const auto& resource : resources) {
            publish("Reading resource", resource.name, result.containers, static_cast<std::uint32_t>(resources.size()));
            const auto raw = source.read(resource);
            Json container{{"name", resource.name}, {"bytes", raw.size()}, {"sha256", sha(raw)}};
            assets::ResArchive48C archive; assets::DecodeError48C error;
            if (!assets::decodeResource48C(raw, archive, error, &hooks)) {
                checkCancel(cancel);
                require(error.kind == assets::DecodeKind48C::unsupported,
                    "Invalid original resource " + resource.name + ": " + error.code + ": " + error.detail);
                container["status"] = "unsupported"; container["reason"] = error.code + ": " + error.detail;
                ++result.unsupported;
            } else {
                container["status"] = "decoded"; container["entries"] = archive.entries.size();
                for (const auto& entry : archive.entries) {
                    checkCancel(cancel);
                    Json record{{"container", resource.name}, {"member", entry.name}, {"source_bytes", entry.bytes},
                        {"logical_offset", entry.logical_offset}};
                    const auto first = archive.decoded.begin() + entry.logical_offset;
                    Bytes bytes(first, first + entry.bytes);
                    const auto sourceHash = sha(bytes); record["source_sha256"] = sourceHash;
                    const auto extension = entry.name.size() < 4 ? "" : upper(entry.name.substr(entry.name.size() - 4));
                    if (extension == ".COL") {
                        const auto key = sha(std::string(collisionConverter) + "\n" + sourceHash); Json cached;
                        if (loadCollision(cache, key, sourceHash, cached)) ++result.reused;
                        else if (convertCollision(cache, bytes, sourceHash, cached, error, &hooks)) ++result.converted;
                        else {
                            checkCancel(cancel);
                            require(error.kind == assets::DecodeKind48C::unsupported,
                                "Invalid original collision " + entry.name + ": " + error.code + ": " + error.detail);
                            ++result.unsupported; record["status"] = "unsupported"; record["reason"] = error.code + ": " + error.detail;
                        }
                        if (!cached.is_null()) { record["status"] = "converted_collision"; record["native"] = cached; }
                    } else if (extension == ".SCB") {
                        const auto key = sha(std::string(worldConverter) + "\n" + sourceHash); Json cached;
                        if (loadWorld(cache, key, sourceHash, cached)) ++result.reused;
                        else if (convertWorld(cache, bytes, sourceHash, cached, error, &hooks)) ++result.converted;
                        else {
                            checkCancel(cancel);
                            require(error.kind == assets::DecodeKind48C::unsupported,
                                "Invalid original world " + entry.name + ": " + error.code + ": " + error.detail);
                            ++result.unsupported; record["status"] = "unsupported"; record["reason"] = error.code + ": " + error.detail;
                        }
                        if (!cached.is_null()) { record["status"] = "converted_world"; record["native"] = cached; }
                    } else if (extension == ".PST" || extension == ".PSM") {
                        const bool texture = extension == ".PST"; const std::string version = texture ? textureConverter : materialConverter;
                        const auto key = sha(version + "\n" + sourceHash); Json cached;
                        if (loadContent(cache, key, sourceHash, version, cached)) ++result.reused;
                        else if (convertContent(cache, bytes, sourceHash, texture, cached, error, &hooks)) ++result.converted;
                        else {
                            checkCancel(cancel);
                            require(error.kind == assets::DecodeKind48C::unsupported,
                                "Invalid original content " + entry.name + ": " + error.code + ": " + error.detail);
                            ++result.unsupported; record["status"] = "unsupported"; record["reason"] = error.code + ": " + error.detail;
                        }
                        if (!cached.is_null()) { record["status"] = texture ? "converted_texture" : "converted_material"; record["native"] = cached; }
                    } else if (extension != ".PSG") {
                        record["status"] = "unsupported"; record["reason"] = "Asset type is outside the current native content converter";
                        ++result.unsupported;
                    } else {
                        const auto key = sha(std::string(sceneConverter) + "\n" + sourceHash);
                        Json cached;
                        if (loadCached(cache, key, sourceHash, cached)) ++result.reused;
                        else {
                            assets::Scene48D scene;
                            if (assets::decodeScene48D(bytes, scene, error, &hooks)) {
                                Bytes semanticBytes; std::string sceneError;
                                require(assets::encodeSceneAsset48D(scene, semanticBytes, sceneError), "Scene encoding failed: " + sceneError);
                                assets::Mesh48B mesh;
                                require(assets::parseMesh48B(scene.neutral_lod0.data(), scene.neutral_lod0.size(), mesh, sceneError), "Invalid converted mesh");
                                cached = Json{{"converter", sceneConverter}, {"key", key}, {"source_sha256", sourceHash},
                                    {"mesh_sha256", sha(scene.neutral_lod0)}, {"scene_sha256", sha(semanticBytes)},
                                    {"vertices", mesh.vertices.size()}, {"triangles", mesh.indices.size() / 3},
                                    {"materials", scene.materials}, {"world_space", scene.world_space},
                                    {"nodes", scene.nodes.size()}, {"lods", scene.lods.size()},
                                    {"vertices_all_lods", scene.vertices_all_lods}, {"surfaces_all_lods", scene.surfaces_all_lods}};
                                cached["record_sha256"] = sha(cached.dump());
                                const auto object = cache / "objects" / (key + ".rrmesh");
                                atomicWrite(object, scene.neutral_lod0);
                                atomicWrite(cache / "objects" / (key + ".rrscene"), semanticBytes);
                                atomicJson(cache / "objects" / (key + ".json"), cached);
                                ++result.converted;
                            } else {
                                checkCancel(cancel);
                                require(error.kind == assets::DecodeKind48C::unsupported,
                                    "Invalid original mesh " + entry.name + ": " + error.code + ": " + error.detail);
                                ++result.unsupported;
                                record["status"] = "unsupported"; record["reason"] = error.code + ": " + error.detail;
                            }
                        }
                        if (!cached.is_null()) { record["status"] = "converted_geometry"; record["native"] = cached; }
                    }
                    manifest["members"].push_back(std::move(record));
                    publish("Converting assets", resource.name + ": " + entry.name, result.converted + result.reused + result.unsupported, 0);
                }
            }
            manifest["containers"].push_back(std::move(container)); ++result.containers;
        }
        require(result.converted + result.reused > 0, "No supported native assets were found; the previous cache remains active");
        publish("Verifying converted cache", "", 0, result.converted + result.reused);
        std::uint32_t verified = 0;
        for (const auto& member : manifest["members"]) {
            checkCancel(cancel);
            if (!member.contains("native")) continue;
            const auto& record = member["native"];
            const std::string key = record.at("key"), sourceHash = record.at("source_sha256");
            require(isHexHash(key) && isHexHash(sourceHash), "Invalid generated cache key");
            Json verifiedRecord;
            const bool valid = record.at("converter") == sceneConverter ? loadCached(cache, key, sourceHash, verifiedRecord) :
                record.at("converter") == worldConverter ? loadWorld(cache, key, sourceHash, verifiedRecord) :
                record.at("converter") == collisionConverter ? loadCollision(cache, key, sourceHash, verifiedRecord) :
                loadContent(cache, key, sourceHash, record.at("converter"), verifiedRecord);
            require(valid && verifiedRecord == record,
                "Converted asset failed final verification");
            publish("Verifying converted cache", member.at("member"), ++verified, result.converted + result.reused);
        }
        source.verifyUnchanged(); checkCancel(cancel);
        const auto text = manifest.dump(2) + "\n";
        result.generation = sha(text);
        const auto generation = cache / "generations" / (result.generation + ".json");
        atomicWrite(generation, Bytes(text.begin(), text.end()));
        // Single commit point: cancellation before this preserves the old generation.
        checkCancel(cancel);
        atomicJson(cache / "current.json", Json{{"schema", "RRCURRENT48C"}, {"generation", result.generation},
            {"manifest_sha256", result.generation}, {"scope", "partial scene, texture, material, world and collision import"}});
        result.manifest_path = generation.u8string(); result.success = true;
    } catch (const Cancelled&) { result.cancelled = true; result.error = "Import cancelled; the previous active cache is unchanged"; }
      catch (const std::exception& error) { result.error = error.what(); }
    return result;
}
std::string importResultJson48C(const ImportResult48C& result) {
    return Json{{"success", result.success}, {"cancelled", result.cancelled}, {"error", result.error},
        {"generation", result.generation}, {"manifest_path", result.manifest_path}, {"containers", result.containers},
        {"converted", result.converted}, {"reused", result.reused}, {"unsupported", result.unsupported},
        {"scope", "partial scene, texture, material, world and collision import; not a playable game cache"}}.dump(2);
}
} }
