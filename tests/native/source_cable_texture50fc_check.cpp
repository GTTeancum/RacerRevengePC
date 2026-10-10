#include "source_cable_texture50fc_fixture.h"
#include "../../src/assets/import_decode48c.h"
#include <iostream>
int main() {
    namespace d = pod_materials50ed_fixture::detail;
    namespace f = source_cable_texture50fc_fixture;
    namespace a = rr::assets;
    try {
        d::Inputs input; input.capacity = 8 * 1024 * 1024;
        const auto bytes = input.read("work/native/project/disc/PODS/POD01/POD01.RES", 8 * 1024 * 1024);
        d::need(d::sha256(bytes) == "08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3", "Original POD01 identity");
        a::ResArchive48C archive; a::DecodeError48C error;
        d::need(a::decodeResource48C(bytes, archive, error), "Native RES decode");
        d::Bytes source; d::need(a::extractResMember48C(archive, "textures\\cable.pst", source, error), "Native source member");
        d::Bytes got; std::string why;
        d::need(f::decodeCableTexture50FC(source, got, why), "Actual source decode");
        const auto expected = input.read("evidence/cableTexture50FC/cable.rrtex", 16400);
        d::need(got == expected, "Independent Python/C++ full output equality");
        for (std::size_t pixel = 0; pixel < 4096; ++pixel) {
            const unsigned index = pixel % 2 ? source[92 + pixel / 2] / 16 : source[92 + pixel / 2] % 16;
            d::need(std::equal(source.begin() + 12 + 4 * index, source.begin() + 16 + 4 * index,
                got.begin() + 16 + 4 * pixel), "Exact source RGBA per texel");
        }
        std::size_t rejected = 0;
        auto reject = [&](const d::Bytes& corrupt) {
            d::Bytes sentinel{9, 8, 7};
            d::need(!f::decodeCableTexture50FC(corrupt, sentinel, why) && sentinel == d::Bytes({9, 8, 7}),
                "Invalid source rejected transactionally"); ++rejected;
        };
        for (std::size_t cut = 0; cut < source.size(); ++cut) reject(d::Bytes(source.begin(), source.begin() + cut));
        auto corrupt = source; corrupt.push_back(0); reject(corrupt);
        for (std::size_t at = 0; at < source.size(); ++at) { corrupt = source; corrupt[at] ^= 1; reject(corrupt); }
        std::cout << "PASS native cable kind0:4096 exact source texels;" << rejected
            << " truncation/substitution cases rejected;64x64 RRTEX SHA256=" << d::sha256(got) << '\n';
        return 0;
    } catch (const std::exception& e) { std::cerr << "FAIL: " << e.what() << '\n'; return 1; }
}
