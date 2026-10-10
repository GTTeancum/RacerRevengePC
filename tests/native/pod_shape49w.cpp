// Test-only projection of original11FF90 ->11F680 ->120230 outputs. Expected
// words come from the authenticated original interpreter, never native math.
#include "pod_shape49w.h"
#include "../../src/assets/pod_shape49w.h"
#include "../../work/native/sources/src/PS2Recomp/ps2xRuntime/include/ee_sqrt14.h"
#include "json.hpp"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <string>
#include <vector>
#if defined(_MSC_VER)
#include <malloc.h>
#endif

namespace {
using namespace rr::game;
using Json = nlohmann::json;
using Words = std::vector<std::uint32_t>;
using Bytes = std::vector<std::uint8_t>;
bool watching = false;
std::size_t allocations = 0, checks = 0, failures = 0, differential = 0, rejections = 0, malformed = 0, persistent = 0;
std::size_t scalarSqrtReferenceCases = 0;
std::string label;
void check(bool value, const char* why) {
    ++checks;
    if (!value) { ++failures; if (failures < 30) std::cerr << "mismatch " << label << ": " << why << '\n'; }
}
float fp(std::uint32_t word) { float value; std::memcpy(&value, &word, 4); return value; }
std::uint32_t bits(float value) { std::uint32_t word; std::memcpy(&word, &value, 4); return word; }
template<class T> bool same(const T& a, const T& b) { return std::memcmp(&a, &b, sizeof(T)) == 0; }
BodyMatrix48G matrix(const Words& words, unsigned first) {
    BodyMatrix48G result{}; for (unsigned i = 0; i < 16; ++i) result.v[i] = fp(words.at(first + i)); return result;
}
PodShapeSource49W source(const Json& row) {
    PodShapeSource49W result{};
    result.local40 = matrix(row.at("shape_words").get<Words>(), 16);
    const auto geometry = row.at("shape_geometry_words").get<Words>();
    for (unsigned i = 0; i < 3; ++i) { result.rootCenter.v[i] = fp(geometry[3 + i]); result.rootHalf.v[i] = fp(geometry[6 + i]); }
    return result;
}
PodShapeState49W initial(const Json& row) {
    PodShapeState49W state{};
    const auto shape = row.at("shape_words").get<Words>(), object = row.at("member_words").get<Words>();
    state.composed00 = matrix(shape, 0); state.world80 = matrix(shape, 32); state.previousC0 = matrix(shape, 48);
    for (unsigned i = 0; i < 4; ++i) state.center60.v[i] = fp(object[24 + i]);
    state.radius80 = fp(object[32]); state.type88 = object[34]; state.registrationE0 = object[56];
    return state;
}
bool call(const PodShapeSource49W& src, const BodyMatrix48G& world, PodShapeState49W& state) {
    watching = true; const bool result = transformPodShape49W(src, world, state); watching = false; return result;
}
void compare(const PodShapeState49W& state, const Json& row) {
    auto shape = row.at("shape_words").get<Words>(), object = row.at("member_words").get<Words>();
    for (unsigned i = 0; i < 16; ++i) {
        shape[i] = bits(state.composed00.v[i]); shape[32 + i] = bits(state.world80.v[i]); shape[48 + i] = bits(state.previousC0.v[i]);
    }
    for (unsigned i = 0; i < 4; ++i) object[24 + i] = bits(state.center60.v[i]);
    object[32] = bits(state.radius80); object[34] = state.type88; object[56] = state.registrationE0;
    check(shape == row.at("expected").at("shape_words").get<Words>(), "all72 shape words, including current/previous and immutable local");
    check(object == row.at("expected").at("member_words").get<Words>(), "all64 object words, including center W/radius/type/current E0");
}
void guards(const Json& row) {
    const auto originalSource = source(row); const auto originalState = initial(row);
    const auto originalWorld = matrix(row.at("input_matrix").get<Words>(), 0);
    auto reject = [&](PodShapeSource49W src, BodyMatrix48G world, PodShapeState49W state) {
        const auto srcBefore = src; const auto worldBefore = world; const auto stateBefore = state;
        check(!call(src, world, state), "rejection expected");
        check(same(src, srcBefore) && same(world, worldBefore) && same(state, stateBefore), "atomic false preserves every byte"); ++rejections;
    };
    for (auto type : {1u, 2u, 6u, 7u, 0xffffffffu}) {
        label = "unadmitted type"; auto s = originalState; s.type88 = type; reject(originalSource, originalWorld, s);
    }
    for (auto e0 : {1u, 0x12345678u, 0x80000000u, 0xffffffffu}) {
        label = "current E0 nonnull despite constructor zero"; auto s = originalState; s.registrationE0 = e0; reject(originalSource, originalWorld, s);
    }
    for (unsigned lane = 0; lane < 22; ++lane) for (std::uint32_t poison : {0x7fc12345u, 0x7f800000u, 0xff800000u}) {
        label = "nonfinite source lane " + std::to_string(lane);
        auto src = originalSource;
        if (lane < 16) src.local40.v[lane] = fp(poison);
        else if (lane < 19) src.rootCenter.v[lane - 16] = fp(poison);
        else src.rootHalf.v[lane - 19] = fp(poison);
        reject(src, originalWorld, originalState);
    }
    for (unsigned lane = 0; lane < 16; ++lane) {
        label = "nonfinite cached world lane " + std::to_string(lane);
        auto world = originalWorld; world.v[lane] = fp(0x7fc12345); reject(originalSource, world, originalState);
    }
    for (int mode : {FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO}) {
        label = "rounding admission"; std::fesetround(mode); reject(originalSource, originalWorld, originalState);
    }
    std::fesetround(FE_TONEAREST);
    { label = "finite multiplication overflow"; auto src = originalSource; auto world = originalWorld;
      src.local40.v[0] = 2; world.v[0] = std::numeric_limits<float>::max(); reject(src, world, originalState); }
    { label = "finite radius overflow"; auto src = originalSource; src.rootHalf.v[2] = std::numeric_limits<float>::max(); reject(src, originalWorld, originalState); }
    { label = "finite center overflow"; auto src = originalSource; auto world = originalWorld;
      src.local40 = {}; src.rootCenter.v[0] = 2; world.v[0] = std::numeric_limits<float>::max(); reject(src, world, originalState); }
    { label = "incoming aliases existing world80"; auto s = originalState, independent = originalState;
      const auto world = s.world80; check(call(originalSource, world, independent), "independent alias reference succeeds");
      check(call(originalSource, s.world80, s) && same(s, independent), "in-place incoming world alias matches all state"); }
    { label = "all unconsumed old history and output lanes are raw"; auto s = originalState;
      for (auto& x : s.composed00.v) x = fp(0x7fa12345);
      for (auto& x : s.previousC0.v) x = fp(0xffa12345);
      for (auto& x : s.center60.v) x = fp(0xff800000);
      s.radius80 = fp(0x7fc12345); check(call(originalSource, originalWorld, s), "raw unconsumed lanes accepted"); compare(s, row); }
    { label = "POD01 identity still evaluates all zero terms"; auto world = originalWorld;
      world.v[4] = std::numeric_limits<float>::infinity(); reject(originalSource, world, originalState); }
    { label = "positive-zero normalization forbidden"; auto src = originalSource; auto world = originalWorld;
      for (auto& x : world.v) x = -0.0f;
      for (auto& x : src.local40.v) x = 1;
      auto s = originalState; check(call(src, world, s), "negative zero route succeeds");
      for (float x : s.composed00.v) check(bits(x) == 0x80000000u, "ordered four negative-zero terms retain sign"); }
}
void scalarRootReference(const Json& row) {
    // Independent direct reference: this calls the retained source interface,
    // not the Python opcode adaptation or any native implementation formula.
    const auto base = source(row);
    const auto world = matrix(row.at("input_matrix").get<Words>(), 0);
    std::vector<std::array<std::uint32_t, 3>> cases;
    for (auto word : {0u, 0x80000000u, 0x1a000000u, 0x1b000000u, 0x1e3ce508u,
                     0x1ffffffeu, 0x1fffffffu, 0x20000000u, 0x20000001u}) {
        for (unsigned axis = 0; axis < 3; ++axis) {
            std::array<std::uint32_t, 3> half{}; half[axis] = word; cases.push_back(half);
        }
    }
    cases.push_back({0x1fb504f3u, 0x1fb504f3u, 0});
    cases.push_back({0x1fb504f3u, 0x1fb504f3u, 0x1a000000u});
    cases.push_back({0x1f000000u, 0x1f000000u, 0x1f000000u});
    bool sawSubnormal = false, sawLargestSubnormal = false, sawSmallestNormal = false, sawZero = false;
    for (const auto& half : cases) {
        label = "independent retained scalar SQRT boundary";
        auto src = base; auto state = initial(row);
        for (unsigned i = 0; i < 3; ++i) src.rootHalf.v[i] = fp(half[i]);
        volatile float xx = src.rootHalf.v[0] * src.rootHalf.v[0];
        volatile float yy = src.rootHalf.v[1] * src.rootHalf.v[1];
        volatile float zz = src.rootHalf.v[2] * src.rootHalf.v[2];
        volatile float xy = xx + yy;
        volatile float norm = xy + zz;
        std::uint32_t flags = 0;
        const float expected = Ps2EeSqrt14(norm, flags);
        check(call(src, world, state), "tiny finite half extents remain admitted");
        check(bits(state.radius80) == bits(expected), "native radius equals retained ee_sqrt14.h directly");
        const auto normBits = bits(norm);
        if (normBits && normBits < 0x00800000u) {
            sawSubnormal = true;
            check(bits(expected) == 0 && bits(std::sqrt(float(norm))) != 0, "retained flush differs from host sqrt on a positive subnormal input");
        }
        sawLargestSubnormal = sawLargestSubnormal || normBits == 0x007fffffu;
        sawSmallestNormal = sawSmallestNormal || normBits == 0x00800000u;
        sawZero = sawZero || normBits == 0;
        ++scalarSqrtReferenceCases;
    }
    check(sawSubnormal && sawLargestSubnormal && sawSmallestNormal && sawZero, "directed scalar root domain covers zero, largest subnormal and smallest normal");
}
Bytes readBytes(const char* filename) {
    std::ifstream in(filename, std::ios::binary); if (!in) throw std::runtime_error("cannot read asset");
    return Bytes(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}
void assetTests(const char* filename, const char* containerFilename, const Json& reference) {
    using namespace rr::assets;
    const Bytes bytes = readBytes(filename);
    PodShapeSource49W decoded{}; DecodeError48C error;
    label = "actual authored POD01";
    ResArchive48C archive; Bytes extracted;
    check(decodeResource48C(readBytes(containerFilename), archive, error), "unchanged48C decoder reads hash-authenticated original RES");
    check(extractResMember48C(archive, "Pods\\Pod01\\podboxf.col", extracted, error) && extracted == bytes, "exact original RES member equals retained authored bytes");
    bool located = false;
    for (const auto& entry : archive.entries) if (entry.name == "Pods\\Pod01\\podboxf.col")
        located = entry.logical_offset == 1818624 && entry.bytes == 1760;
    check(located, "exact member name/count/logical offset verified");
    check(decodePodShapeSource49W(bytes, decoded, error), "complete retained COL decoded");
    check(same(decoded, source(reference)), "original loader mapped local40/rootCenter/rootHalf match exact bytes");
    const auto original = decoded;
    auto reject = [&](const Bytes& changed) {
        auto result = original;
        check(!decodePodShapeSource49W(changed, result, error), "malformed or unadmitted authored record rejected");
        check(same(result, original), "cold decode preserves immutable source on false"); ++malformed;
    };
    for (std::size_t cut = 0; cut < bytes.size(); ++cut) { label = "authored prefix " + std::to_string(cut); reject(Bytes(bytes.begin(), bytes.begin() + cut)); }
    { label = "authored suffix"; auto b = bytes; b.push_back(0); reject(b); }
    for (auto change : std::vector<std::pair<std::size_t, std::uint32_t>>{
        {0, 1235}, {4, 0}, {8, 2}, {12, 5}, {16, 0x7fc12345}, {64, 2},
        {68, 17}, {72, 28}, {76, 29}, {80, 2}, {84, 0x7f800000}, {276, 2},
        {280, 0xbf800000}, {296, 0xbf800000}, {1144, 0x7fc12345}}) {
        label = "authored invalid word " + std::to_string(change.first); auto b = bytes;
        for (unsigned lane = 0; lane < 4; ++lane) b[change.first + lane] = static_cast<std::uint8_t>(change.second >> (lane * 8));
        reject(b);
    }
    for (auto change : std::vector<std::pair<std::size_t, std::uint16_t>>{{308, 0}, {308, 32767}, {308, 65535}, {1160, 65535}}) {
        label = "authored invalid reference"; auto b = bytes;
        b[change.first] = static_cast<std::uint8_t>(change.second); b[change.first + 1] = static_cast<std::uint8_t>(change.second >> 8); reject(b);
    }
    { label = "authored shared subtree"; auto b = bytes; b[310] = b[308]; b[311] = b[309]; reject(b); }
    { label = "authored byte bound"; Bytes b(4097, 0); reject(b); }
    { label = "authored nonidentity matrix remains admitted"; auto b = bytes; const auto word = bits(2.0f);
      for (unsigned i = 0; i < 4; ++i) b[16 + i] = static_cast<std::uint8_t>(word >> (i * 8));
      auto result = original; check(decodePodShapeSource49W(b, result, error), "nonidentity local admitted");
      check(result.local40.v[0] == 2.0f && same(result.rootCenter, original.rootCenter) && same(result.rootHalf, original.rootHalf), "only authored local changes"); }
}
void* allocate(std::size_t n, std::size_t alignment = 0) {
    if (watching) ++allocations;
#if defined(_MSC_VER)
    void* p = alignment ? _aligned_malloc(n ? n : 1, alignment) : std::malloc(n ? n : 1);
#else
    void* p = alignment ? std::aligned_alloc(alignment, ((n ? n : 1) + alignment - 1) / alignment * alignment) : std::malloc(n ? n : 1);
#endif
    if (!p) throw std::bad_alloc(); return p;
}
void releaseAligned(void* p) noexcept {
#if defined(_MSC_VER)
    _aligned_free(p);
#else
    std::free(p);
#endif
}
}
#if defined(_MSC_VER)
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE __attribute__((noinline))
#endif
NOINLINE void* operator new(std::size_t n) { return allocate(n); }
NOINLINE void* operator new[](std::size_t n) { return allocate(n); }
NOINLINE void* operator new(std::size_t n, std::align_val_t a) { return allocate(n, static_cast<std::size_t>(a)); }
NOINLINE void* operator new[](std::size_t n, std::align_val_t a) { return allocate(n, static_cast<std::size_t>(a)); }
NOINLINE void operator delete(void* p) noexcept { std::free(p); }
NOINLINE void operator delete[](void* p) noexcept { std::free(p); }
NOINLINE void operator delete(void* p, std::size_t) noexcept { std::free(p); }
NOINLINE void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
NOINLINE void operator delete(void* p, std::align_val_t) noexcept { releaseAligned(p); }
NOINLINE void operator delete[](void* p, std::align_val_t) noexcept { releaseAligned(p); }
NOINLINE void operator delete(void* p, std::size_t, std::align_val_t) noexcept { releaseAligned(p); }
NOINLINE void operator delete[](void* p, std::size_t, std::align_val_t) noexcept { releaseAligned(p); }

int main(int argc, char** argv) {
    try {
        if (argc != 4) throw std::runtime_error("usage: pod_shape49w fixtures.json podboxf.col POD01.RES");
        std::fesetround(FE_TONEAREST);
        std::ifstream input(argv[1]); Json packet; input >> packet;
        if (packet.at("format") != "pod-shape49w-v1") throw std::runtime_error("fixture format");
        PodShapeState49W carried{};
        for (const auto& row : packet.at("cases")) {
            label = row.at("name").get<std::string>();
            auto src = source(row); const auto srcBefore = src;
            auto world = matrix(row.at("input_matrix").get<Words>(), 0); const auto worldBefore = world;
            auto state = initial(row);
            if (row.value("continue_state", false)) { check(same(state, carried), "prior terminal state is next initial state"); state = carried; ++persistent; }
            check(call(src, world, state), "valid native late consumer returns");
            compare(state, row);
            check(same(src, srcBefore) && same(world, worldBefore), "all immutable source/input bytes preserved");
            carried = state; ++differential;
        }
        guards(packet.at("cases").at(0));
        scalarRootReference(packet.at("cases").at(0));
        assetTests(argv[2], argv[3], packet.at("cases").at(0));
        check(allocations == 0, "no ordinary/array/aligned allocation in hot runtime");
        const Json report = {{"passed", failures == 0}, {"checks", checks}, {"failures", failures},
            {"differential_cases", differential}, {"persistent_transitions", persistent}, {"atomic_rejections", rejections},
            {"malformed_authored_records", malformed}, {"native_allocations", allocations},
            {"scalar_sqrt_reference_cases", scalarSqrtReferenceCases},
            {"native_state_bytes", sizeof(PodShapeState49W)}, {"immutable_source_bytes", sizeof(PodShapeSource49W)}};
        std::cout << report.dump() << '\n'; return failures ? 1 : 0;
    } catch (const std::exception& error) { watching = false; std::cerr << "harness error: " << error.what() << '\n'; return 2; }
}
