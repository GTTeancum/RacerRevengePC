#pragma once
#include "pod_materials50ed_fixture.h"

// Cold, exact-source native decoder for the retained Pod01 cable PST1/kind0.
// The original loader transfers packed indices as PSMT4 (low nibble first),
// and uploads the16 raw RGBA words as8x2 PSMCT32 with CSM1/CSA0. That palette
// upload's physical addresses equal the T32I4 CLUT addresses, so no palette
// permutation is applied. No files, codec, generated substitute, row flip,
// alpha normalization or guest execution occurs in this decoder.
namespace source_cable_texture50fc_fixture {
inline constexpr const char* sourceSha256 = "e6b0abd320de50c784816a4e15dc6bfa056c8241a173eb26481243ff84dc301f";
inline constexpr const char* outputSha256 = "10329f55fd0edd413b71c74915b6694700beee922c36255c410099eccd903d64";
inline bool decodeCableTexture50FC(const std::vector<std::uint8_t>& source,
    std::vector<std::uint8_t>& rrtex, std::string& error) {
    namespace d = pod_materials50ed_fixture::detail;
    try {
        d::need(source.size() == 2140 && d::sha256(source) == sourceSha256,
            "50FC cable exact original source hash/extent required");
        d::need(d::u32(source.data()) == 1 && d::u32(source.data() + 4) == 0 &&
            d::u32(source.data() + 8) == 1 && d::u32(source.data() + 76) == 64 &&
            d::u32(source.data() + 80) == 64 && d::u32(source.data() + 84) == 0 &&
            d::u32(source.data() + 88) == 0, "50FC cable supported PST1/kind0 layout");
        d::Bytes result(16 + 64 * 64 * 4);
        std::memcpy(result.data(), "RRTEX001", 8);
        result[8] = result[12] = 64;
        for (std::size_t pixel = 0; pixel < 64 * 64; ++pixel) {
            const auto packed = source[92 + pixel / 2];
            const unsigned index = (packed >> ((pixel & 1) * 4)) & 15;
            for (unsigned component = 0; component < 4; ++component)
                result[16 + pixel * 4 + component] = source[12 + index * 4 + component];
        }
        d::need(d::sha256(result) == outputSha256, "50FC cable exact decoded RGBA output identity");
        rrtex = std::move(result); error.clear(); return true;
    } catch (const std::exception& e) { try { error = e.what(); } catch (...) {} return false; }
    catch (...) { try { error = "50FC cable cold decode failure"; } catch (...) {} return false; }
}
} // namespace source_cable_texture50fc_fixture
