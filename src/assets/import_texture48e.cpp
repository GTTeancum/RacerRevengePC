#include "import_texture48e.h"
#include <algorithm>
#include <limits>
#include <new>
#include <utility>

namespace rr { namespace assets {
namespace {
using Bytes = std::vector<std::uint8_t>;
struct Failure { DecodeError48C error; };
void require(bool ok, DecodeKind48C kind, const char* code, std::size_t at, const char* detail) {
    if (!ok) throw Failure{{kind, code, at, detail}};
}
void allocationError(DecodeError48C& error) {
    error = {DecodeKind48C::limit, "texture_material.allocation", 0, "Allocation failed within bounded decoder"};
}
void checkCancel(const DecodeHooks48C* hooks, std::size_t at) {
    require(!hooks || !hooks->cancelled || !hooks->cancelled(hooks->context), DecodeKind48C::cancelled,
        "texture_material.cancelled", at, "Cancelled before output commit");
}
void progress(const DecodeHooks48C* hooks, const char* stage, std::uint32_t done, std::uint32_t total) {
    if (hooks && hooks->progress) hooks->progress(hooks->context, stage, done, total);
}
struct Reader {
    const Bytes& bytes;
    void range(std::size_t at, std::size_t count) const {
        require(at <= bytes.size() && count <= bytes.size() - at, DecodeKind48C::malformed,
            "pst.truncated", at, "Read exceeds PST extent");
    }
    std::uint16_t u16(std::size_t at) const {
        range(at, 2); return std::uint16_t(unsigned(bytes[at]) | (unsigned(bytes[at + 1]) << 8));
    }
    std::uint32_t u32(std::size_t at) const {
        range(at, 4); return std::uint32_t(bytes[at]) | (std::uint32_t(bytes[at + 1]) << 8) |
            (std::uint32_t(bytes[at + 2]) << 16) | (std::uint32_t(bytes[at + 3]) << 24);
    }
};
bool powerOfTwo(std::uint32_t n) { return n && !(n & (n - 1)); }
void append32(Bytes& bytes, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) bytes.push_back(static_cast<std::uint8_t>(value >> (8 * i)));
}
Bytes serialize(const TextureMip48E& mip) {
    require(powerOfTwo(mip.width) && powerOfTwo(mip.height) && mip.width >= 16 && mip.height >= 16 &&
        mip.width <= 1024 && mip.height <= 1024, DecodeKind48C::unsupported,
        "rrtex.dimensions", 0, "48E output requires power-of-two dimensions 16 through 1024");
    const std::size_t size = std::size_t(mip.width) * mip.height * 4;
    require(mip.rgba.size() == size, DecodeKind48C::malformed, "rrtex.extent", 0, "RGBA size differs from dimensions");
    Bytes bytes; bytes.reserve(16 + size);
    const char magic[] = "RRTEX001"; bytes.insert(bytes.end(), magic, magic + 8);
    append32(bytes, mip.width); append32(bytes, mip.height);
    bytes.insert(bytes.end(), mip.rgba.begin(), mip.rgba.end()); return bytes;
}
bool whitespace(unsigned char c) { return c == 32 || (c >= 9 && c <= 13) || (c >= 28 && c <= 31); }
bool lineBreak(unsigned char c) { return c == 10 || c == 11 || c == 12 || c == 13 || (c >= 28 && c <= 30); }
std::string trim(const std::string& s) {
    std::size_t begin = 0, end = s.size();
    while (begin < end && whitespace(static_cast<unsigned char>(s[begin]))) ++begin;
    while (end > begin && whitespace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(begin, end - begin);
}
std::string lower(std::string s) {
    for (char& c : s) if (c >= 'A' && c <= 'Z') c = char(c + ('a' - 'A'));
    return s;
}
bool identifier(const std::string& s, bool dash = false) {
    if (s.empty()) return false;
    for (char c : s) if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || c == '_' || (dash && c == '-'))) return false;
    return true;
}
struct Section {
    std::string name;
    std::size_t offset = 0;
    std::vector<MaterialField48E> fields;
    const MaterialField48E* find(const std::string& key) const {
        for (const auto& field : fields) if (field.name == key) return &field;
        return nullptr;
    }
};
std::uint32_t number(const std::string& s, std::uint32_t low, std::uint32_t high, std::size_t at, const char* code) {
    require(!s.empty(), DecodeKind48C::malformed, code, at, "Missing or empty decimal count");
    std::uint32_t value = 0;
    for (char c : s) {
        require(c >= '0' && c <= '9', DecodeKind48C::malformed, code, at, "Count is not an unsigned decimal integer");
        const unsigned digit = unsigned(c - '0');
        require(value <= (std::numeric_limits<std::uint32_t>::max() - digit) / 10,
            DecodeKind48C::limit, code, at, "Decimal count exceeds uint32");
        value = value * 10 + digit;
    }
    require(value >= low && value <= high, DecodeKind48C::unsupported, code, at, "Count is outside supported bounds");
    return value;
}
MaterialFields48E retainFields(const Section& section) {
    MaterialFields48E result;
    for (const auto& field : section.fields)
        (field.value.empty() ? result.unspecified : result.authored).push_back(field);
    return result;
}
}

bool encodeTextureLevel48E(const TextureMip48E& mip, Bytes& output, DecodeError48C& error) {
    error = {};
    try { auto bytes = serialize(mip); output = std::move(bytes); return true; }
    catch (const Failure& failure) { error = failure.error; }
    catch (const std::bad_alloc&) { allocationError(error); }
    return false;
}

bool decodeTexture48E(const Bytes& input, Texture48E& output, DecodeError48C& error,
    const DecodeHooks48C* hooks, const TextureLimits48E& limits) {
    error = {};
    try {
        checkCancel(hooks, 0);
        require(input.size() <= std::min(limits.input_bytes, std::size_t(8u * 1024u * 1024u)),
            DecodeKind48C::limit, "pst.input_limit", 0, "PST input exceeds byte limit");
        Reader read{input}; read.range(0, 12);
        require(read.u16(0) == 1, DecodeKind48C::unsupported, "pst.revision", 0, "Only original PST revision 1 is supported");
        require(read.u16(2) == 0, DecodeKind48C::unsupported, "pst.flags", 2, "Only observed flags zero are supported");
        require(read.u32(4) == 1, DecodeKind48C::unsupported, "pst.kind", 4, "Only 256-entry indexed kind 1 is supported");
        const auto count = read.u32(8);
        require(count >= 1 && count <= 7, DecodeKind48C::unsupported, "pst.mip_count", 8, "Only one through seven mip levels are supported");
        read.range(12, 1024);
        Texture48E parsed;
        for (unsigned i = 0; i < 256; ++i) for (unsigned c = 0; c < 4; ++c)
            parsed.original_palette[i][c] = input[12 + 4 * i + c];
        std::size_t at = 1036, decoded = 16, nonPowerOffset = 0, smallOffset = 0;
        std::uint32_t previousW = 0, previousH = 0;
        // First validate every level/extent before allocating pixel arrays.
        for (std::uint32_t level = 0; level < count; ++level) {
            read.range(at, 16); TextureMip48E mip;
            mip.source_header_offset = at; mip.source_indices_offset = at + 16;
            mip.height = read.u32(at); mip.width = read.u32(at + 4);
            require(mip.width && mip.height, DecodeKind48C::malformed,
                "pst.dimensions", at, "Zero texture dimension");
            require(mip.width <= 1024 && mip.height <= 1024, DecodeKind48C::unsupported,
                "pst.dimension_subset", at, "Dimensions exceed the bounded 1024 maximum");
            if ((!powerOfTwo(mip.width) || !powerOfTwo(mip.height)) && !nonPowerOffset) nonPowerOffset = at;
            if ((mip.width < 16 || mip.height < 16) && !smallOffset) smallOffset = at;
            require(read.u32(at + 8) == 0 && read.u32(at + 12) == 0, DecodeKind48C::unsupported,
                "pst.surface_words", at + 8, "Unrecognized PST1 mip header words");
            require(level == 0 || (mip.width == previousW * 2 && mip.height == previousH * 2),
                DecodeKind48C::malformed, "pst.mip_order", at, "Original mip dimensions must ascend by factors of two");
            const std::size_t texels = std::size_t(mip.width) * mip.height;
            read.range(at + 16, texels); at += 16 + texels;
            decoded += texels * 4 * (level + 1 == count ? 2 : 1);
            require(decoded <= std::min(limits.decoded_bytes, std::size_t(16u * 1024u * 1024u)),
                DecodeKind48C::limit, "pst.output_limit", mip.source_header_offset, "Retained RGBA and base RRTEX exceed byte limit");
            previousW = mip.width; previousH = mip.height; parsed.mips.push_back(std::move(mip));
        }
        require(at == input.size(), DecodeKind48C::malformed, "pst.trailing_bytes", at, "Unconsumed PST suffix");
        // Original UI textures use complete640x448 levels; small original mips
        // also exist. Reject these unproved interpretations only after checking
        // all known-kind1 extents, so truncation/corruption stays fatal.
        require(!nonPowerOffset, DecodeKind48C::unsupported, "pst.non_power_of_two", nonPowerOffset,
            "Structurally complete non-power-of-two PST needs separate layout proof");
        require(!smallOffset, DecodeKind48C::unsupported, "pst.dimension_subset", smallOffset,
            "Structurally complete mip below16 needs separate layout proof");
        progress(hooks, "pst.rgba", 0, count); checkCancel(hooks, 1036);
        for (std::uint32_t level = 0; level < count; ++level) {
            auto& mip = parsed.mips[level]; mip.rgba.resize(std::size_t(mip.width) * mip.height * 4);
            for (std::uint32_t y = 0; y < mip.height; ++y) {
                if (!(y & 63)) checkCancel(hooks, mip.source_indices_offset + std::size_t(y) * mip.width);
                for (std::uint32_t x = 0; x < mip.width; ++x) {
                    const std::size_t pixel = std::size_t(y) * mip.width + x;
                    const unsigned index = input[mip.source_indices_offset + pixel];
                    const unsigned entry = (index & ~24u) | ((index & 8u) << 1) | ((index & 16u) >> 1);
                    for (unsigned c = 0; c < 4; ++c) mip.rgba[4 * pixel + c] = parsed.original_palette[entry][c];
                }
            }
            progress(hooks, "pst.rgba", level + 1, count); checkCancel(hooks, mip.source_indices_offset);
        }
        parsed.base_rrtex = serialize(parsed.mips.back()); checkCancel(hooks, at);
        output = std::move(parsed); return true;
    } catch (const Failure& failure) { error = failure.error; }
    catch (const std::bad_alloc&) { allocationError(error); }
    return false;
}

bool decodeMaterial48E(const Bytes& input, Material48E& output, DecodeError48C& error,
    const DecodeHooks48C* hooks, std::size_t maxBytes) {
    error = {};
    try {
        checkCancel(hooks, 0);
        require(!input.empty(), DecodeKind48C::malformed, "psm.empty", 0, "Empty material");
        require(input.size() <= std::min(maxBytes, std::size_t(64u * 1024u)), DecodeKind48C::limit,
            "psm.input_limit", 0, "PSM exceeds 64 KiB or caller limit");
        for (std::size_t i = 0; i < input.size(); ++i) {
            require(input[i] != 0, DecodeKind48C::malformed, "psm.nul", i, "NUL is not accepted in material text");
            require(input[i] < 128, DecodeKind48C::unsupported, "psm.encoding", i, "Only original ASCII material text is supported");
        }
        std::vector<Section> sections; bool duplicateKey = false; std::size_t duplicateOffset = 0;
        for (std::size_t at = 0; at < input.size();) {
            checkCancel(hooks, at); const std::size_t begin = at;
            while (at < input.size() && !lineBreak(input[at])) ++at;
            std::string line(input.begin() + begin, input.begin() + at);
            if (at < input.size()) { const auto c = input[at++]; if (c == 13 && at < input.size() && input[at] == 10) ++at; }
            const auto comment = line.find(';'); if (comment != std::string::npos) line.resize(comment);
            line = trim(line); if (line.empty()) continue;
            if (line.front() == '[' && line.back() == ']') {
                const std::string name = trim(line.substr(1, line.size() - 2));
                require(!name.empty(), DecodeKind48C::malformed, "psm.section", begin, "Empty material section");
                for (const auto& section : sections) require(section.name != name, DecodeKind48C::malformed,
                    "psm.duplicate_section", begin, "Repeated material section");
                require(sections.size() < 9, DecodeKind48C::unsupported, "psm.section_count", begin, "Only General plus eight passes are supported");
                sections.push_back({name, begin, {}}); continue;
            }
            const auto equal = line.find('=');
            require(!sections.empty() && equal != std::string::npos, DecodeKind48C::malformed,
                "psm.line", begin, "Expected key=value inside a section");
            MaterialField48E field{trim(line.substr(0, equal)), trim(line.substr(equal + 1)), begin};
            require(!field.name.empty(), DecodeKind48C::malformed, "psm.key", begin, "Empty material key");
            const auto folded = lower(field.name);
            for (const auto& previous : sections.back().fields) if (lower(previous.name) == folded && !duplicateKey) {
                duplicateKey = true; duplicateOffset = begin;
            }
            sections.back().fields.push_back(std::move(field));
        }
        // Repeated authored settings occur in complete original PSMs. Their
        // precedence is unproved; do not choose a winner. Complete lexical
        // validation above still makes malformed lines/NUL/sections fatal.
        require(!duplicateKey, DecodeKind48C::unsupported, "psm.duplicate_key", duplicateOffset,
            "Repeated authored key requires unproved original precedence semantics");
        const Section* general = nullptr;
        for (const auto& section : sections) if (section.name == "General") general = &section;
        require(general || sections.size() != 1 || sections[0].name != "DefaultShaderParams", DecodeKind48C::unsupported,
            "psm.default_parameters", 0, "Original DefaultShaderParams schema is outside numbered-pass support");
        require(general != nullptr, DecodeKind48C::malformed, "psm.general", 0, "Missing General section");
        const auto* countField = general->find("TotalShaderPasses");
        require(countField != nullptr, DecodeKind48C::malformed, "psm.pass_count", general->offset, "Missing TotalShaderPasses");
        const auto count = number(countField->value, 1, 8, countField->source_offset, "psm.pass_count");
        for (std::uint32_t pass = 1; pass <= count; ++pass) {
            bool present = false; const auto name = "ShaderPass_" + std::to_string(pass);
            for (const auto& section : sections) present = present || section.name == name;
            require(present, DecodeKind48C::malformed, "psm.sections", 0, "Missing declared numbered ShaderPass section");
        }
        bool knownExtraSections = true;
        for (const auto& section : sections) {
            bool known = section.name == "General" || section.name == "Prologue" || section.name == "Epilogue";
            for (unsigned pass = 1; pass <= 8; ++pass) known = known || section.name == "ShaderPass_" + std::to_string(pass);
            knownExtraSections = knownExtraSections && known;
        }
        require(sections.size() == count + 1, knownExtraSections ? DecodeKind48C::unsupported : DecodeKind48C::malformed,
            "psm.sections", 0, "Extra inactive passes or Prologue/Epilogue need separate original semantics");
        Material48E parsed; parsed.general = retainFields(*general);
        progress(hooks, "psm.bindings", 0, count); checkCancel(hooks, 0);
        for (std::uint32_t pass = 1; pass <= count; ++pass) {
            const Section* section = nullptr; const auto name = "ShaderPass_" + std::to_string(pass);
            for (const auto& candidate : sections) if (candidate.name == name) section = &candidate;
            require(section != nullptr, DecodeKind48C::malformed, "psm.sections", 0, "Missing numbered ShaderPass section");
            MaterialPass48E p; p.pass = pass; p.fields = retainFields(*section);
            const auto* program = section->find("microProgramName");
            require(program && identifier(program->value), DecodeKind48C::unsupported, "psm.program_name", section->offset,
                "Missing or unsupported microProgramName; no shader is guessed");
            p.microprogram = program->value;
            const auto* maps = section->find("totalTextureMaps"); p.texture_count_defaulted = maps == nullptr;
            const auto mapCount = maps ? number(maps->value, 1, 2, maps->source_offset, "psm.map_count") : 1;
            for (std::uint32_t slot = 1; slot <= mapCount; ++slot) {
                const auto* map = section->find("TextureMap_" + std::to_string(slot));
                require(map && identifier(map->value, true), DecodeKind48C::unsupported, "psm.texture_name", section->offset,
                    "Missing or unsupported texture basename/path; binding is not invented");
                p.textures.push_back({slot, map->value});
            }
            for (const auto& field : section->fields) if (field.name.compare(0, 11, "TextureMap_") == 0) {
                const auto suffix = field.name.substr(11);
                bool digits = !suffix.empty(); for (char c : suffix) digits = digits && c >= '0' && c <= '9';
                if (digits) {
                    const auto slot = number(suffix, 1, 2, field.source_offset, "psm.map_declaration");
                    require(slot <= mapCount, DecodeKind48C::malformed, "psm.map_declaration", field.source_offset,
                        "Texture map declarations disagree with count");
                }
            }
            parsed.passes.push_back(std::move(p)); progress(hooks, "psm.bindings", pass, count); checkCancel(hooks, section->offset);
        }
        output = std::move(parsed); return true;
    } catch (const Failure& failure) { error = failure.error; }
    catch (const std::bad_alloc&) { allocationError(error); }
    return false;
}
} }
