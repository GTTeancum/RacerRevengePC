#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "import_texture48e.h"
#include "json.hpp"
#include <windows.h>
#include <bcrypt.h>
#include <algorithm>
#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

namespace {
using namespace rr::assets;
using Bytes = std::vector<std::uint8_t>;
using Json = nlohmann::json;
namespace fs = std::filesystem;
unsigned checks = 0, failures = 0;
void expect(bool ok, const char* message) {
    ++checks; if (!ok) { ++failures; if (failures <= 30) std::fprintf(stderr, "FAIL: %s\n", message); }
}
Bytes readFile(const fs::path& path, std::uint64_t limit = 64ull * 1024 * 1024) {
    const auto size = fs::file_size(path);
    if (size > limit) throw std::runtime_error("Fixture exceeds cap: " + path.u8string());
    std::ifstream f(path, std::ios::binary); Bytes bytes(static_cast<std::size_t>(size));
    if (size) f.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(size));
    if (!f || f.peek() != std::char_traits<char>::eof()) throw std::runtime_error("Fixture read failed: " + path.u8string());
    return bytes;
}
Json readJson(const fs::path& path) { const auto bytes = readFile(path); return Json::parse(bytes.begin(), bytes.end()); }
std::string sha(const Bytes& bytes) {
    BCRYPT_ALG_HANDLE algorithm = nullptr; BCRYPT_HASH_HANDLE hash = nullptr;
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0) throw std::runtime_error("SHA256 unavailable");
    if (BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0) < 0) {
        BCryptCloseAlgorithmProvider(algorithm, 0); throw std::runtime_error("SHA256 initialization failed");
    }
    std::array<std::uint8_t, 32> digest{};
    const auto update = BCryptHashData(hash, const_cast<PUCHAR>(bytes.data()), static_cast<ULONG>(bytes.size()), 0);
    const auto finish = BCryptFinishHash(hash, digest.data(), static_cast<ULONG>(digest.size()), 0);
    BCryptDestroyHash(hash); BCryptCloseAlgorithmProvider(algorithm, 0);
    if (update < 0 || finish < 0) throw std::runtime_error("SHA256 calculation failed");
    std::ostringstream s; for (auto b : digest) s << std::hex << std::setw(2) << std::setfill('0') << unsigned(b); return s.str();
}
void put32(Bytes& bytes, std::size_t at, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) bytes[at + i] = static_cast<std::uint8_t>(value >> (8 * i));
}
Bytes ascii(const std::string& s) { return Bytes(s.begin(), s.end()); }
Json fields(const MaterialFields48E& fields) {
    Json result = Json::object();
    for (const auto& f : fields.authored) { expect(!f.value.empty(), "authored field remains nonempty"); result[f.name] = f.value; }
    for (const auto& f : fields.unspecified) { expect(f.value.empty(), "unspecified field remains empty"); result[f.name] = f.value; }
    expect(result.size() == fields.authored.size() + fields.unspecified.size(), "all material fields retained exactly once");
    return result;
}
void compareMaterial(const Material48E& actual, const Json& reference) {
    expect(fields(actual.general) == reference.at("source_sections").at("General"), "all original General fields retained");
    expect(actual.passes.size() == reference.at("passes").size(), "original pass count");
    for (std::size_t i = 0; i < std::min(actual.passes.size(), reference.at("passes").size()); ++i) {
        const auto& p = actual.passes[i]; const auto& wanted = reference.at("passes")[i];
        expect(p.pass == wanted.at("pass") && p.microprogram == wanted.at("microprogram"), "program and pass identity");
        const auto section = "ShaderPass_" + std::to_string(p.pass);
        const auto& authored = reference.at("source_sections").at(section);
        expect(fields(p.fields) == authored, "all original pass fields including blanks retained");
        expect(p.texture_count_defaulted == !authored.contains("totalTextureMaps"), "only missing map count uses original default");
        expect(p.textures.size() == wanted.at("textures").size(), "original map slot count");
        for (std::size_t j = 0; j < std::min(p.textures.size(), wanted.at("textures").size()); ++j)
            expect(p.textures[j].slot == wanted.at("textures")[j].at("slot") && p.textures[j].name == wanted.at("textures")[j].at("name"), "original texture name and slot");
    }
}
struct Hooks { bool cancel = false; unsigned calls = 0, stop = 0xffffffffu; };
bool cancelled(void* context) { return static_cast<Hooks*>(context)->cancel; }
void progress(void* context, const char*, std::uint32_t, std::uint32_t) {
    auto& state = *static_cast<Hooks*>(context); if (++state.calls >= state.stop) state.cancel = true;
}
Texture48E sentinelTexture() {
    Texture48E texture; texture.kind = 0xabcdef; texture.base_rrtex = {8, 7, 6};
    texture.mips.push_back({7, 9, 11, 13, {5, 4}}); texture.original_palette[3] = {{1, 2, 3, 4}}; return texture;
}
bool textureUnchanged(const Texture48E& t) {
    return t.kind == 0xabcdef && t.base_rrtex == Bytes({8, 7, 6}) && t.mips.size() == 1 &&
        t.mips[0].width == 7 && t.mips[0].rgba == Bytes({5, 4}) && t.original_palette[3] == std::array<std::uint8_t, 4>{{1, 2, 3, 4}};
}
Material48E sentinelMaterial() { Material48E m; m.general.authored.push_back({"marker", "unchanged", 3}); return m; }
bool materialUnchanged(const Material48E& m) {
    return m.passes.empty() && m.general.authored.size() == 1 && m.general.authored[0].name == "marker" && m.general.authored[0].value == "unchanged";
}
Bytes textureFixture(std::uint32_t w = 16, std::uint32_t h = 16, std::uint32_t count = 1) {
    Bytes bytes(1036); put32(bytes, 0, 1); put32(bytes, 4, 1); put32(bytes, 8, count);
    for (unsigned i = 0; i < 256; ++i) {
        bytes[12 + 4 * i] = static_cast<std::uint8_t>(i); bytes[13 + 4 * i] = static_cast<std::uint8_t>(255 - i);
        bytes[14 + 4 * i] = static_cast<std::uint8_t>(i ^ 37); bytes[15 + 4 * i] = static_cast<std::uint8_t>(i);
    }
    for (std::uint32_t level = 0; level < count; ++level) {
        const auto at = bytes.size(); bytes.resize(at + 16 + std::size_t(w) * h);
        put32(bytes, at, h); put32(bytes, at + 4, w);
        for (std::size_t i = 0; i < std::size_t(w) * h; ++i) bytes[at + 16 + i] = static_cast<std::uint8_t>(i);
        w *= 2; h *= 2;
    }
    return bytes;
}
void adversarial(const fs::path& root) {
    DecodeError48C error; const Bytes fixture = textureFixture();
    for (std::size_t cut = 0; cut < fixture.size(); ++cut) {
        Texture48E output = sentinelTexture(); Bytes truncated(fixture.begin(), fixture.begin() + cut);
        expect(!decodeTexture48E(truncated, output, error) && error.kind == DecodeKind48C::malformed, "every PST prefix rejected as malformed");
        expect(textureUnchanged(output), "truncation keeps texture output unchanged");
    }
    const std::vector<std::pair<std::size_t, std::uint32_t>> mutations{
        {0, 2}, {0, 0x10001}, {4, 0}, {8, 0}, {8, 8}, {1036, 0}, {1036, 15}, {1036, 8},
        {1040, 0x80000000}, {1044, 1}, {1048, 1}};
    for (const auto& change : mutations) {
        auto bytes = fixture; put32(bytes, change.first, change.second); auto output = sentinelTexture();
        expect(!decodeTexture48E(bytes, output, error) && !error.code.empty(), "unsupported or malformed PST mutation rejected with code");
        expect(textureUnchanged(output), "mutation rejection transactional");
    }
    auto suffix = fixture; suffix.push_back(0); Texture48E texture = sentinelTexture();
    expect(!decodeTexture48E(suffix, texture, error) && error.code == "pst.trailing_bytes" && textureUnchanged(texture), "PST suffix rejected");
    auto nonPower = textureFixture(640, 448); texture = sentinelTexture();
    expect(!decodeTexture48E(nonPower, texture, error) && error.kind == DecodeKind48C::unsupported &&
        error.code == "pst.non_power_of_two" && textureUnchanged(texture), "complete original UI shape is unsupported, not corrupt");
    nonPower.pop_back(); expect(!decodeTexture48E(nonPower, texture, error) && error.kind == DecodeKind48C::malformed, "truncated unsupported-size PST remains malformed");
    auto smallMip = textureFixture(16, 8, 3);
    expect(!decodeTexture48E(smallMip, texture, error) && error.kind == DecodeKind48C::unsupported, "complete original small-mip chain is unsupported");
    smallMip.pop_back(); expect(!decodeTexture48E(smallMip, texture, error) && error.kind == DecodeKind48C::malformed, "truncated small-mip chain remains malformed");
    TextureLimits48E limits; limits.input_bytes = fixture.size() - 1;
    expect(!decodeTexture48E(fixture, texture, error, nullptr, limits) && error.kind == DecodeKind48C::limit, "caller input cap");
    limits = {}; limits.decoded_bytes = 2063;
    expect(!decodeTexture48E(fixture, texture, error, nullptr, limits) && error.code == "pst.output_limit", "decoded allocation cap includes base RRTEX");
    limits.decoded_bytes = 2064;
    expect(decodeTexture48E(fixture, texture, error, nullptr, limits), "exact output byte cap accepted");
    const auto mapping = readJson(root / "evidence/textures48D/layout-oracle.json").at("palette_mapping").get<std::vector<unsigned>>();
    for (unsigned h = 16; h <= 1024; h *= 2) for (unsigned w = 16; w <= 1024; w *= 2) {
        const auto bytes = textureFixture(w, h); expect(decodeTexture48E(bytes, texture, error), "all proven dimension pairs decode");
        bool pixels = texture.mips.size() == 1;
        if (pixels) for (std::size_t i = 0; i < std::size_t(w) * h; ++i) for (unsigned c = 0; c < 4; ++c)
            pixels = pixels && texture.mips[0].rgba[i * 4 + c] == bytes[12 + mapping[i & 255] * 4 + c];
        expect(pixels, "independent frozen GS palette mapping and original row order, all RGBA channels");
    }
    const auto chain = textureFixture(16, 16, 7); expect(decodeTexture48E(chain, texture, error), "maximum ascending mip chain");
    for (unsigned stop = 1; stop <= 8; ++stop) {
        Hooks state; state.stop = stop; DecodeHooks48C hooks{&state, cancelled, progress}; texture = sentinelTexture();
        expect(!decodeTexture48E(chain, texture, error, &hooks) && error.kind == DecodeKind48C::cancelled && textureUnchanged(texture), "cancellation before/after every mip is transactional");
    }
    Hooks before; before.cancel = true; DecodeHooks48C hooks{&before, cancelled, progress}; texture = sentinelTexture();
    expect(!decodeTexture48E(fixture, texture, error, &hooks) && textureUnchanged(texture), "initial cancellation");
    auto wrongOrder = textureFixture(16, 16, 2); put32(wrongOrder, 1036 + 16 + 256, 16);
    expect(!decodeTexture48E(wrongOrder, texture, error) && error.code == "pst.mip_order", "mip order invariant");
    Bytes encoded{1, 2, 3}; TextureMip48E invalid; invalid.width = invalid.height = 16;
    expect(!encodeTextureLevel48E(invalid, encoded, error) && encoded == Bytes({1, 2, 3}), "export checks RGBA extent transactionally");
    const std::string material = "[General]\r\nTotalShaderPasses=1\n[ShaderPass_1]\nmicroProgramName=PreLitCompressed\nTextureMap_1=ta_sand8\nalphaBlend=\nunknownAuthored=keepThis ; original comment\n";
    Material48E parsed; expect(decodeMaterial48E(ascii(material), parsed, error), "ASCII mixed newline material fixture");
    expect(parsed.passes[0].texture_count_defaulted && parsed.passes[0].fields.unspecified.size() == 1 &&
        fields(parsed.passes[0].fields)["unknownAuthored"] == "keepThis", "authored unknown and blank fields retained without guessed defaults");
    // Original 29B718/29B7F0 counts declared passes; these two historical
    // rejection fixtures now retain ShaderPass_2 as inactive source evidence.
    for (const auto& inactive : std::vector<std::pair<std::string, Json>>{
        {"[ShaderPass_2]\n", Json::object()},
        {"[ShaderPass_2]\nmicroProgramName=SpecularMapCompressed\n", Json{{"microProgramName", "SpecularMapCompressed"}}}}) {
        const auto original = ascii(material + inactive.first); parsed = sentinelMaterial();
        const bool accepted = decodeMaterial48E(original, parsed, error);
        expect(accepted && error.kind == DecodeKind48C::none && error.code.empty(), "inactive numbered pass is accepted");
        if (!accepted) continue;
        expect(parsed.passes.size() == 1 && parsed.passes[0].pass == 1 &&
            parsed.passes[0].microprogram == "PreLitCompressed" && parsed.passes[0].textures.size() == 1 &&
            parsed.passes[0].textures[0].name == "ta_sand8", "only the declared pass supplies program and texture bindings");
        expect(parsed.original_text == original && parsed.inactive_sections.size() == 1,
            "inactive pass retains the complete original source and one evidence section");
        if (parsed.inactive_sections.size() == 1) {
            const auto& section = parsed.inactive_sections[0];
            expect(section.name == "ShaderPass_2" && section.source_offset == material.size() &&
                section.source_length == inactive.first.size() && fields(section.fields) == inactive.second,
                "inactive pass retains its exact range and authored fields without requiring active bindings");
        }
    }
    const std::vector<std::string> invalidMaterials{
        "", "bad", "[General]\nTotalShaderPasses=999999999999999999999999999999\n",
        material + "TextureMap_1=duplicate\n", material + "texturemap_1=duplicate\n", material + "[General]\n",
        material + "TextureMap_2=extra\n", material + "totalTextureMaps=2\n", material + "totalTextureMaps=\n",
        material + "totalTextureMaps=0\n", material + std::string(1, '\0'), material + std::string(1, '\xff'),
        "[General]\nTotalShaderPasses=1\n[ShaderPass_1]\nmicroProgramName=Unsafe Name\nTextureMap_1=x\n",
        "[General]\nTotalShaderPasses=1\n[ShaderPass_1]\nmicroProgramName=UnknownProgram\nTextureMap_1=../path\n"};
    for (const auto& bad : invalidMaterials) {
        parsed = sentinelMaterial(); expect(!decodeMaterial48E(ascii(bad), parsed, error) && !error.code.empty(), "malformed/unsupported PSM rejection named");
        expect(materialUnchanged(parsed), "PSM failure transactional");
    }
    for (const auto& bad : std::vector<std::string>{"", "bad", "[Other]\nname=value\n", material + "broken-line", material + "[General]\n",
        material + std::string(1, '\0'), material + "TextureMap_1=duplicate\nbroken-line",
        "[General]\nTotalShaderPasses=2\n[ShaderPass_1]\nmicroProgramName=Diffuse\nTextureMap_1=x\n",
        "[General]\nTotalShaderPasses=garbage\n[ShaderPass_1]\n"}) {
        parsed = sentinelMaterial(); expect(!decodeMaterial48E(ascii(bad), parsed, error) && error.kind == DecodeKind48C::malformed &&
            materialUnchanged(parsed), "malformed PSM syntax/count/missing declared pass remains fatal");
    }
    for (const auto& unsupported : std::vector<std::string>{"[DefaultShaderParams]\nmicroProgramName=DiffuseCompressed\n",
        material + "TextureMap_1=duplicate\n",
        material + "[Prologue]\nsetting=original\n[Epilogue]\nsetting=original\n"}) {
        parsed = sentinelMaterial(); expect(!decodeMaterial48E(ascii(unsupported), parsed, error) && error.kind == DecodeKind48C::unsupported &&
            materialUnchanged(parsed), "recognized complete original PSM variants explicitly unsupported");
    }
    parsed = sentinelMaterial(); expect(!decodeMaterial48E(ascii(material), parsed, error, nullptr, material.size() - 1) &&
        error.kind == DecodeKind48C::limit && materialUnchanged(parsed), "material caller input cap");
    expect(!decodeMaterial48E(Bytes(65537, ' '), parsed, error, nullptr, 999999) && error.kind == DecodeKind48C::limit, "caller cannot raise PSM hard cap");
    for (unsigned stop = 1; stop <= 2; ++stop) {
        Hooks state; state.stop = stop; DecodeHooks48C cancelHooks{&state, cancelled, progress}; parsed = sentinelMaterial();
        expect(!decodeMaterial48E(ascii(material), parsed, error, &cancelHooks) && error.kind == DecodeKind48C::cancelled && materialUnchanged(parsed), "material cancellation transactional");
    }
    expect(decodeMaterial48E(ascii(material), parsed, error) && error.kind == DecodeKind48C::none && error.code.empty(), "successful decode clears prior error");
}
}

int main(int argc, char** argv) {
    try {
        if (argc < 2 || argc > 3) throw std::runtime_error("usage: racer_native_texture_check PROJECT_ROOT [REPORT_JSON]");
        const fs::path root = fs::u8path(argv[1]); const auto reference = readJson(root / "evidence/importTexture48E/reference-index.json");
        const fs::path disc = root / "work/native/project/disc";
        DecodeError48C error; std::map<std::string, unsigned> counts, reasons;
        unsigned archives = 0, mips = 0; std::uint64_t pixels = 0;
        Json rejected = Json::array();
        for (const auto& container : reference.at("containers")) {
            const std::string containerName = container.at("path"); const auto bytes = readFile(disc / fs::u8path(containerName));
            expect(sha(bytes) == container.at("sha256"), "original resource container SHA256");
            ResArchive48C archive;
            if (!decodeResource48C(bytes, archive, error)) throw std::runtime_error(containerName + ": " + error.code + ": " + error.detail);
            expect(!container.contains("container_error"), "native/Python resource acceptance agrees"); ++archives;
            for (const auto& member : container.at("members")) {
                const std::string name = member.at("name"), sourceHash = member.at("sha256"), type = member.at("type");
                Bytes raw; if (!extractResMember48C(archive, name, raw, error)) throw std::runtime_error(name + ": " + error.code);
                expect(sha(raw) == sourceHash, "original member SHA256");
                const auto& expected = reference.at("references").at(sourceHash); bool accepted;
                if (type == "pst") {
                    Texture48E texture; accepted = decodeTexture48E(raw, texture, error);
                    expect(accepted == expected.at("accepted").get<bool>(), "native/Python PST acceptance agrees");
                    if (accepted && expected.at("accepted").get<bool>()) {
                        expect(texture.mips.size() == expected.at("mips").size(), "all original mip levels retained");
                        bool palette = true; for (unsigned i = 0; i < 256; ++i) for (unsigned c = 0; c < 4; ++c)
                            palette = palette && texture.original_palette[i][c] == raw[12 + i * 4 + c];
                        expect(palette, "original raw palette retained");
                        for (std::size_t i = 0; i < texture.mips.size(); ++i) {
                            const auto& mip = texture.mips[i]; const auto& want = expected.at("mips").at(i);
                            expect(mip.width == want.at("width") && mip.height == want.at("height") &&
                                mip.source_header_offset == want.at("header_offset") && mip.source_indices_offset == mip.source_header_offset + 16, "exact mip dimensions and provenance");
                            expect(sha(mip.rgba) == want.at("rgba_sha256"), "full original RGBA matches frozen Python48D");
                            Bytes exportBytes; expect(encodeTextureLevel48E(mip, exportBytes, error), "native mip serialization");
                            expect(sha(exportBytes) == want.at("rrtex_sha256"), "every serialized RRTEX matches frozen converter");
                            ++mips; pixels += std::uint64_t(mip.width) * mip.height;
                        }
                        expect(sha(texture.base_rrtex) == expected.at("mips").back().at("rrtex_sha256"), "base RRTEX selects largest source level");
                    }
                } else {
                    Material48E material; accepted = decodeMaterial48E(raw, material, error);
                    // Frozen Python48D rejected extra numbered sections. Keep
                    // this historical gate strict until per-member evidence
                    // supplies the declared-pass acceptance/semantic rebase;
                    // the synthetic cases above do not establish corpus totals.
                    expect(accepted == expected.at("accepted").get<bool>(), "native/Python PSM acceptance agrees");
                    if (accepted && expected.at("accepted").get<bool>()) compareMaterial(material, expected.at("material"));
                }
                ++counts[type + (accepted ? "_accepted" : "_rejected")];
                if (!accepted) {
                    expect(error.kind != DecodeKind48C::none && !error.code.empty(), "rejection is explicit"); ++reasons[error.code];
                    expect(error.kind == DecodeKind48C::unsupported, "every rejected valid-original member is a nonfatal unsupported variant");
                    rejected.push_back(Json{{"container", containerName}, {"member", name}, {"source_sha256", sourceHash},
                        {"code", error.code}, {"offset", error.offset}, {"detail", error.detail}});
                }
            }
        }
        // Historical totals remain unchanged pending that per-member rebase.
        expect(Json(counts) == reference.at("counts"), "all-disc accepted/rejected totals match frozen Python subset");
        adversarial(root);
        Json report{{"status", failures ? "FAIL" : "PASS"}, {"checks", checks}, {"failures", failures},
            {"architecture_bits", 8 * sizeof(void*)}, {"containers", archives}, {"counts", counts}, {"mip_surfaces", mips},
            {"rgba_texels_compared", pixels}, {"rejection_codes", reasons}, {"rejected", rejected},
            {"limitations", Json::array({"Source texture and authored material semantics only; no shader, GS blend, final visual fidelity or gameplay performance claim.",
                "Known shader names are preserved, never dispatched to guessed native effects; texture lookup is external to this decoder."})}};
        if (argc == 3) { std::ofstream out(fs::u8path(argv[2]), std::ios::binary); out << report.dump(2) << '\n'; if (!out) throw std::runtime_error("Report write failed"); }
        report.erase("rejected"); std::printf("%s\n", report.dump().c_str());
        return failures ? 1 : 0;
    } catch (const std::exception& exc) { std::fprintf(stderr, "ERROR: %s\n", exc.what()); return 2; }
}
