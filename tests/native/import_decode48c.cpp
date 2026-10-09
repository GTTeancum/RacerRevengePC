#include "import_decode48c.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using Bytes = std::vector<std::uint8_t>;
unsigned checks = 0, failures = 0;
void expect(bool condition, const char* message) {
    ++checks;
    if (!condition) { ++failures; std::fprintf(stderr, "FAIL: %s\n", message); }
}
Bytes readFile(const std::string& path) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) throw std::runtime_error("missing required fixture: " + path);
    stream.seekg(0, std::ios::end);
    const auto size = stream.tellg();
    if (size < 0 || size > 64 * 1024 * 1024) throw std::runtime_error("fixture size outside limits");
    stream.seekg(0);
    Bytes bytes(static_cast<std::size_t>(size));
    if (!stream.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size())))
        throw std::runtime_error("fixture read failed");
    return bytes;
}
void put32(Bytes& data, std::size_t offset, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) data[offset + i] = static_cast<std::uint8_t>(value >> (8 * i));
}
void append16(Bytes& data, unsigned value) {
    data.push_back(static_cast<std::uint8_t>(value)); data.push_back(static_cast<std::uint8_t>(value >> 8));
}
void append32(Bytes& data, std::uint32_t value) {
    const std::size_t offset = data.size(); data.resize(offset + 4); put32(data, offset, value);
}
Bytes storedZlib(std::size_t size = 24576) {
    // An independently constructed RFC1950 stream with one uncompressed DEFLATE block.
    Bytes bytes{0x78, 0x01, 0x01};
    append16(bytes, static_cast<unsigned>(size)); append16(bytes, static_cast<unsigned>(size) ^ 0xffffu);
    std::uint32_t a = 1, b = 0;
    for (std::size_t i = 0; i < size; ++i) {
        const auto value = static_cast<std::uint8_t>(i * 17);
        bytes.push_back(value); a = (a + value) % 65521; b = (b + a) % 65521;
    }
    const std::uint32_t adler = (b << 16) | a;
    for (int shift = 24; shift >= 0; shift -= 8) bytes.push_back(static_cast<std::uint8_t>(adler >> shift));
    return bytes;
}
Bytes simpleResource(const Bytes& compressed = storedZlib()) {
    Bytes bytes;
    append32(bytes, 3); append32(bytes, 16); append32(bytes, 0x340); append32(bytes, 0); append32(bytes, 4);
    append16(bytes, static_cast<unsigned>(compressed.size())); append16(bytes, 0);
    append32(bytes, 1); append32(bytes, 22);
    append32(bytes, 10);
    const std::string name = "sample.bin";
    bytes.insert(bytes.end(), name.begin(), name.end());
    append32(bytes, 0); append32(bytes, 24576);
    bytes.insert(bytes.end(), compressed.begin(), compressed.end()); bytes.push_back(255);
    return bytes;
}
struct HookState { bool cancel = false; unsigned stopAfter = 0xffffffffu, last = 0, total = 0, calls = 0; };
bool cancelled(void* pointer) { return static_cast<HookState*>(pointer)->cancel; }
void progressed(void* pointer, const char*, std::uint32_t done, std::uint32_t total) {
    auto& state = *static_cast<HookState*>(pointer);
    state.last = done; state.total = total; ++state.calls;
    if (done >= state.stopAfter) state.cancel = true;
}
}
int main(int argc, char** argv) {
    using namespace rr::assets;
    try {
        if (argc != 2) throw std::runtime_error("usage: racer_native_import_check PROJECT_ROOT");
        const std::string root = argv[1];
        const auto original = simpleResource();
        ResArchive48C resource;
        DecodeError48C error;
        expect(decodeResource48C(original, resource, error), "valid independent stored-zlib RES fixture");
        expect(error.kind == DecodeKind48C::none && error.code.empty(), "success clears error");
        expect(resource.entries.size() == 1 && resource.entries[0].name == "sample.bin" &&
            resource.decoded.size() == 24576 && resource.blocks.size() == 1, "synthetic RES structure");
        expect(resource.decoded[19] == static_cast<std::uint8_t>(19 * 17), "synthetic decoded content");
        auto rejectResource = [&](const Bytes& candidate, const char* description) {
            expect(!decodeResource48C(candidate, resource, error) && error.kind != DecodeKind48C::none && !error.code.empty(), description);
            expect(resource.entries.size() == 1 && resource.entries[0].name == "sample.bin" && resource.decoded.size() == 24576,
                "failed RES parse is transactional");
        };
        for (std::size_t size : {std::size_t(0), std::size_t(4), std::size_t(16), std::size_t(20), std::size_t(24),
                std::size_t(28), std::size_t(32), std::size_t(53), original.size() - 1})
            rejectResource(Bytes(original.begin(), original.begin() + size), "RES structural truncation rejected");
        Bytes changed = original; changed.push_back(0); rejectResource(changed, "RES suffix rejected");
        changed = original; put32(changed, 0, 4); rejectResource(changed, "RES version unsupported");
        changed = original; put32(changed, 4, 0xffffffffu); rejectResource(changed, "RES metadata overflow rejected");
        changed = original; put32(changed, 8, 0); rejectResource(changed, "RES codec unsupported");
        changed = original; put32(changed, 12, 1); rejectResource(changed, "RES flags unsupported");
        changed = original; put32(changed, 16, 0xffffffffu); rejectResource(changed, "RES size-table arithmetic bounded");
        changed = original; changed[22] = 1; rejectResource(changed, "RES terminator rejected");
        changed = original; changed[20] = changed[21] = 0; rejectResource(changed, "RES zero block rejected");
        changed = original; put32(changed, 24, 10001); rejectResource(changed, "RES entry limit checked");
        changed = original; put32(changed, 28, 0xffffffffu); rejectResource(changed, "RES index extent bounded");
        changed = original; put32(changed, 32, 1025); rejectResource(changed, "RES name length bounded");
        changed = original; changed[36] = 0; rejectResource(changed, "RES embedded-NUL name rejected");
        changed = original; changed[36] = 255; rejectResource(changed, "RES non-ASCII name rejected");
        changed = original; put32(changed, 46, 0xfffffff0u); rejectResource(changed, "RES member offset overflow rejected");
        changed = original; put32(changed, 50, 24577); rejectResource(changed, "RES member end bounded");
        changed = original; changed.back() = 0; rejectResource(changed, "RES block boundary validated");
        changed = original; changed[changed.size() - 2] ^= 1; rejectResource(changed, "RES zlib checksum corruption rejected");
        Bytes extra = storedZlib(); extra.push_back(0); rejectResource(simpleResource(extra), "RES unused compressed suffix rejected");
        rejectResource(simpleResource(storedZlib(24575)), "RES short decompressed block rejected");
        rejectResource(simpleResource(storedZlib(24577)), "RES oversized decompressed block rejected");
        DecodeLimits48C limits;
        limits.resource_file_bytes = original.size() - 1;
        expect(!decodeResource48C(original, resource, error, nullptr, limits) && error.kind == DecodeKind48C::limit, "custom compressed-byte limit");
        limits = {}; limits.resource_decoded_bytes = 24575;
        expect(!decodeResource48C(original, resource, error, nullptr, limits) && error.code == "res.decoded_limit", "custom decoded-byte limit");
        limits = {}; limits.resource_entries = 0;
        expect(!decodeResource48C(original, resource, error, nullptr, limits) && error.code == "res.entry_limit", "custom entry limit");
        limits = {}; limits.resource_name_bytes = 9;
        expect(!decodeResource48C(original, resource, error, nullptr, limits) && error.code == "res.name_limit", "custom name limit");
        HookState hookState;
        DecodeHooks48C hooks{&hookState, cancelled, progressed};
        hookState.cancel = true;
        expect(!decodeResource48C(original, resource, error, &hooks) && error.kind == DecodeKind48C::cancelled, "RES initial cancellation");
        hookState = {}; hookState.stopAfter = 1;
        expect(!decodeResource48C(original, resource, error, &hooks) && error.kind == DecodeKind48C::cancelled && resource.entries[0].name == "sample.bin",
            "RES final callback cancellation before commit");
        hookState = {};
        expect(decodeResource48C(original, resource, error, &hooks) && hookState.calls == 2 && hookState.last == 1 && hookState.total == 1,
            "RES progress reports block completion");
        Bytes extracted{1,2,3};
        expect(extractResMember48C(resource, "SAMPLE.BIN", extracted, error) && extracted == resource.decoded, "case-insensitive member extraction");
        expect(!extractResMember48C(resource, "missing", extracted, error) && error.code == "res.member_missing", "missing member named error");
        expect(!extractResMember48C(resource, "sample.bin", extracted, error, 12) && error.kind == DecodeKind48C::limit, "member extraction limit");
        resource.entries.push_back(resource.entries[0]);
        expect(!extractResMember48C(resource, "sample.bin", extracted, error) && error.code == "res.member_ambiguous", "duplicate member rejection");
        resource.entries.pop_back();
        resource.entries[0].logical_offset = 0xffffffffu;
        expect(!extractResMember48C(resource, "sample.bin", extracted, error) && error.code == "res.member_extent", "external archive metadata revalidated");

        const Bytes ta = readFile(root + "/work/native/project/disc/TRACKS/TA.RES");
        ResArchive48C track;
        hookState = {};
        expect(decodeResource48C(ta, track, error, &hooks), "actual TA.RES decoded natively");
        if (error.kind != DecodeKind48C::none) throw std::runtime_error(error.code + ": " + error.detail);
        expect(track.entries.size() == 383 && track.decoded.size() == 13099008 && track.blocks.size() == 533, "actual TA counts match frozen evidence");
        expect(hookState.last == 533 && hookState.total == 533 && hookState.calls == 534, "actual TA progress totals");
        const char* names[] = {"geometry\\zone01groundstrips16.psg", "geometry\\ta_bigrocka.psg"};
        const char* directories[] = {"ground", "rock"};
        const unsigned vertexCounts[] = {53, 73}, triangleCounts[] = {25, 41};
        for (unsigned fixture = 0; fixture < 2; ++fixture) {
            const std::string directory = root + "/evidence/assets48B/" + directories[fixture];
            const Bytes originalMesh = readFile(directory + "/original.psg");
            const Bytes frozenMesh = readFile(directory + "/mesh.rrmesh");
            expect(extractResMember48C(track, names[fixture], extracted, error), "actual TA member extraction");
            expect(extracted == originalMesh, "C++ extraction equals frozen original PSG bytes");
            PsgMesh48C mesh;
            hookState = {};
            expect(decodeMesh48C(extracted, mesh, error, &hooks), "actual PSG3 conversion");
            if (error.kind != DecodeKind48C::none) throw std::runtime_error(error.code + ": " + error.detail);
            expect(mesh.rrmesh == frozenMesh, "C++ mesh bytes exactly equal frozen Python48B output");
            expect(mesh.vertices == vertexCounts[fixture] && mesh.triangles == triangleCounts[fixture], "mesh count metadata");
            expect(mesh.world_space == (fixture == 0), "original world/local flag retained");
            expect(mesh.positive_normal_alignment + mesh.degenerate_normal_alignment == mesh.triangles, "normal checks account for every triangle");
            expect(hookState.last == mesh.vertices && hookState.total == mesh.vertices, "PSG progress reaches full vertex count");
            std::printf("fixture=%s original_bytes=%zu rrmesh_bytes=%zu vertices=%u triangles=%u exact48B=1\n",
                directories[fixture], originalMesh.size(), mesh.rrmesh.size(), mesh.vertices, mesh.triangles);
            if (fixture != 0) continue;
            auto rejectMesh = [&](const Bytes& candidate, const char* description) {
                expect(!decodeMesh48C(candidate, mesh, error) && error.kind != DecodeKind48C::none && !error.code.empty(), description);
                expect(mesh.rrmesh == frozenMesh && mesh.vertices == 53, "PSG failure preserves previous complete output");
            };
            for (std::size_t length = 0; length < originalMesh.size(); ++length)
                rejectMesh(Bytes(originalMesh.begin(), originalMesh.begin() + length), "every PSG truncation rejected");
            changed = originalMesh; changed.push_back(0); rejectMesh(changed, "PSG payload length mismatch");
            changed = originalMesh; put32(changed, 4, 5); rejectMesh(changed, "PSG5 unsupported");
            changed = originalMesh; put32(changed, 8, 2); rejectMesh(changed, "multiple hierarchy objects unsupported");
            changed = originalMesh; std::fill(changed.begin() + 12, changed.begin() + 140, 'x'); rejectMesh(changed, "unterminated fixed string");
            changed = originalMesh; changed[139] = 1; rejectMesh(changed, "nonzero fixed-string padding");
            changed = originalMesh; put32(changed, 140, 0); rejectMesh(changed, "nonidentity transform unsupported");
            changed = originalMesh; put32(changed, 204, 0x7f800000u); rejectMesh(changed, "nonfinite hierarchy bounds");
            changed = originalMesh; put32(changed, 232, 0); rejectMesh(changed, "hierarchy parent unsupported");
            changed = originalMesh; put32(changed, 236, 2); rejectMesh(changed, "multiple materials unsupported");
            changed = originalMesh; put32(changed, 304, 0x7fc00000u); rejectMesh(changed, "nonfinite quantization rejected");
            changed = originalMesh; put32(changed, 304, 0); rejectMesh(changed, "zero quantization rejected");
            changed = originalMesh; put32(changed, 332, 2); rejectMesh(changed, "unknown world flag rejected");
            changed = originalMesh; put32(changed, 332, 0); rejectMesh(changed, "local nonzero origin unsupported");
            changed = originalMesh; put32(changed, 336, 2); rejectMesh(changed, "LOD variant unsupported");
            changed = originalMesh; put32(changed, 352, 0); rejectMesh(changed, "surface flags unsupported");
            changed = originalMesh; put32(changed, 356, 0x7fc00000u); rejectMesh(changed, "nonfinite UV metadata");
            changed = originalMesh; changed[360] = changed[361] = 0; rejectMesh(changed, "zero debug vertex count");
            changed = originalMesh; put32(changed, 364, 0xfffffff0u); rejectMesh(changed, "large payload extent rejected");
            changed = originalMesh; put32(changed, 368, 0x6c028000u); rejectMesh(changed, "upload command shape unsupported");
            changed = originalMesh; put32(changed, 372, 0x8002u); rejectMesh(changed, "small packed group rejected");
            changed = originalMesh; changed[398] = 16; changed[399] = 0; rejectMesh(changed, "strip ADC start unsupported");
            changed = originalMesh; put32(changed, 204, 0); rejectMesh(changed, "independent hierarchy bounds check");
            changed = originalMesh;
            // Frozen ground has four upload groups, not one 53-vertex group.
            // These offsets/counts are recorded in evidence/assets48B/ground/conversion.json.
            const std::size_t groupOffsets[] = {368, 696, 1040, 1384};
            const unsigned groupCounts[] = {15, 16, 16, 6};
            for (unsigned group = 0; group < 4; ++group) {
                const std::size_t normalsStart = groupOffsets[group] + 28 + 8 * groupCounts[group];
                for (std::size_t i = 0; i < groupCounts[group] * 3; ++i)
                    changed[normalsStart + i] = static_cast<std::uint8_t>(0u - changed[normalsStart + i]);
            }
            rejectMesh(changed, "independent original-normal winding check");
            expect(error.code == "psg.normal_winding", "normal failure is named specifically");
            changed = originalMesh; changed[362] = 13;
            rejectMesh(changed, "differing source/group strip accounting stays unsupported");
            expect(error.kind == DecodeKind48C::unsupported && error.code == "psg.strip_count_variant",
                "original source-strip splits are not classified as corrupt vertex data");
            limits = {}; limits.mesh_file_bytes = originalMesh.size() - 1;
            expect(!decodeMesh48C(originalMesh, mesh, error, nullptr, limits) && error.code == "psg.file_limit", "custom mesh limit");
            hookState = {}; hookState.cancel = true;
            expect(!decodeMesh48C(originalMesh, mesh, error, &hooks) && error.kind == DecodeKind48C::cancelled, "PSG initial cancellation");
            hookState = {}; hookState.stopAfter = 53;
            expect(!decodeMesh48C(originalMesh, mesh, error, &hooks) && error.kind == DecodeKind48C::cancelled && mesh.rrmesh == frozenMesh,
                "PSG cancellation retains frozen output");
        }
        std::printf("native-import48c pointer_bits=%u checks=%u failures=%u\n", unsigned(sizeof(void*) * 8), checks, failures);
        return failures ? 1 : 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "Fixture failure: %s\n", error.what());
        return 2;
    }
}
