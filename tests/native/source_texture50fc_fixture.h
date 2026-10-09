#pragma once
#include "pod_materials50ed_fixture.h"

// Cold-only adapter for the eight source-decoded TA textures from 50FC.
// No codec runs in the client. The generated table pins original member bytes
// and genuine decoded output bytes; this adapter authenticates both. ATM uses
// the actual first stored frame, not a fabricated replacement or animation.
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
} // namespace detail

// Matches track_materials50fa_fixture::SourceTextureDecoder. Root explicitly
// chooses largest retained mip and enables static ATM frame0. Caller's output
// bytes and mipIndex remain unchanged on failure; only error is replaced.
inline bool decodeSourceTexture(const char* sourceMember,
    const std::vector<std::uint8_t>& source, std::vector<std::uint8_t>& rrtex,
    std::uint32_t& mipIndex, std::string& error) {
    using namespace detail;
    try {
        shared::need(sourceMember != nullptr && source.size() <= 1024u * 1024u,
            "50FC exact source member and bounded bytes required");
        const auto member = shared::normalized(sourceMember);
        const Entry* selected = nullptr;
        for (const auto& entry : entries) if (member == entry.member) {
            shared::need(!selected, "50FC duplicate source member"); selected = &entry;
        }
        shared::need(selected, "50FC source member was not decoded");
        shared::need(source.size() == selected->sourceBytes && shared::sha256(source) == selected->sourceSha256,
            "50FC original source hash/extent mismatch");
        const std::filesystem::path root("evidence/textures50FC");
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
} // namespace source_texture50fc_fixture
