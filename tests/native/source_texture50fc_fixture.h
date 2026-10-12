#pragma once
#include "pod_materials50ed_fixture.h"

// Cold-only adapters for authenticated TA/GC/GA source textures.
// No codec runs in the client. The generated table pins original member bytes
// and genuine decoded output bytes; this adapter authenticates both. ATM uses
// genuine retained source frames; their live timing belongs to the separate owner.
namespace source_texture50fc_fixture {
namespace detail {
namespace shared = pod_materials50ed_fixture::detail;
struct Entry {
    const char* member;
    const char* sourceSha256;
    const char* outputFilename;
    const char* outputSha256;
    std::uint32_t sourceBytes, width, height, mipIndex;
};
inline constexpr Entry entries[] = {
#include "source_texture50fc_index.inc"
};
static_assert(sizeof(entries) / sizeof(entries[0]) == 8, "Exact retained TA source texture closure");
inline constexpr Entry gcEntries[] = {
#include "source_texture_gc_index.inc"
};
static_assert(sizeof(gcEntries) / sizeof(gcEntries[0]) == 4, "Exact retained GC source animation closure");
#if __has_include("source_texture_ga_index.inc")
inline constexpr Entry gaEntries[] = {
#include "source_texture_ga_index.inc"
};
static_assert(sizeof(gaEntries)/sizeof(gaEntries[0])==9,"Exact GA four-ATM/five-dynamic-PST closure");
#endif
} // namespace detail

// Matches track_materials50fa_fixture::SourceTextureDecoder. Root explicitly
// chooses largest retained mip and enables static ATM frame0. Caller's output
// bytes and mipIndex remain unchanged on failure; only error is replaced.
inline bool decodeRetainedSource(const detail::Entry* entries, std::size_t entryCount,
    const std::filesystem::path& root, const char* sourceMember,
    const std::vector<std::uint8_t>& source, std::vector<std::uint8_t>& rrtex,
    std::uint32_t& mipIndex, std::string& error) {
    using namespace detail;
    try {
        shared::need(sourceMember != nullptr && source.size() <= 1024u * 1024u,
            "50FC exact source member and bounded bytes required");
        const auto member = shared::normalized(sourceMember);
        const Entry* selected = nullptr;
        for (std::size_t i = 0; i < entryCount; ++i) {
            const auto& entry = entries[i]; if (member != entry.member) continue;
            shared::need(!selected, "50FC duplicate source member"); selected = &entry;
        }
        shared::need(selected, "50FC source member was not decoded");
        shared::need(source.size() == selected->sourceBytes && shared::sha256(source) == selected->sourceSha256,
            "50FC original source hash/extent mismatch");
        shared::need(!std::filesystem::is_symlink(root), "50FC retained image directory symlink rejected");
        const auto expected = std::size_t(selected->width) * selected->height * 4 + 16;
        shared::need(selected->width && selected->height && selected->width <= 1024 && selected->height <= 1024 &&
            selected->mipIndex <= 6 && expected <= 4u * 1024u * 1024u + 16,
            "50FC decoded image metadata bounds");
        shared::Inputs input; input.capacity = 4u * 1024u * 1024u + 16;
        auto decoded = input.read(root / selected->outputFilename, expected);
        shared::need(decoded.size() == expected && shared::sha256(decoded) == selected->outputSha256 &&
            std::memcmp(decoded.data(), "RRTEX001", 8) == 0 &&
            shared::u32(decoded.data() + 8) == selected->width &&
            shared::u32(decoded.data() + 12) == selected->height,
            "50FC genuine source-decoded output hash/framing mismatch");
        // Full native texture validation remains in the shared 50EA bank.
        rrtex = std::move(decoded); mipIndex = selected->mipIndex; error.clear(); return true;
    } catch (const std::exception& e) { try { error = e.what(); } catch (...) {} return false; }
    catch (...) { try { error = "Unexpected 50FC cold source image failure"; } catch (...) {} return false; }
}
// Existing TA callers and retained output identities are unchanged.
inline bool decodeSourceTexture(const char* sourceMember,
    const std::vector<std::uint8_t>& source, std::vector<std::uint8_t>& rrtex,
    std::uint32_t& mipIndex, std::string& error) {
    return decodeRetainedSource(detail::entries, sizeof(detail::entries)/sizeof(detail::entries[0]),
        "evidence/textures50FC", sourceMember, source, rrtex, mipIndex, error);
}
// GC uses its own original-byte/output table, including the exact authored
// start frames of s_flaga/b/c. No same-name TA resource is selected.
inline bool decodeSourceTextureGc(const char* sourceMember,
    const std::vector<std::uint8_t>& source, std::vector<std::uint8_t>& rrtex,
    std::uint32_t& mipIndex, std::string& error) {
    return decodeRetainedSource(detail::gcEntries, sizeof(detail::gcEntries)/sizeof(detail::gcEntries[0]),
        "evidence/texturesGC", sourceMember, source, rrtex, mipIndex, error);
}
// GA has its own authenticated four-ATM/five-dynamic-PST table. No source
// member or pixel aliases another track; full ATM timing has a separate owner.
inline bool decodeSourceTextureGa(const char* sourceMember,
    const std::vector<std::uint8_t>& source, std::vector<std::uint8_t>& rrtex,
    std::uint32_t& mipIndex, std::string& error) {
#if __has_include("source_texture_ga_index.inc")
    return decodeRetainedSource(detail::gaEntries, sizeof(detail::gaEntries)/sizeof(detail::gaEntries[0]),
        "evidence/texturesGA", sourceMember, source, rrtex, mipIndex, error);
#else
    (void)sourceMember;(void)source;(void)rrtex;(void)mipIndex;
    error="GA original source frames have not been cold-prepared and pinned";return false;
#endif
}
} // namespace source_texture50fc_fixture
