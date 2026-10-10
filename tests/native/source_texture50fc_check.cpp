#include "source_texture50fc_fixture.h"
#include "../../src/assets/import_decode48c.h"
#include "../../src/assets/import_texture48e.h"
#include <iostream>

// Focused cold-conversion check. Never starts the game or mutates outputs.
int main(int argc, char** argv) {
    namespace a = rr::assets;
    namespace f = source_texture50fc_fixture;
    namespace d = pod_materials50ed_fixture::detail;
    try {
        const std::filesystem::path sourcePath = argc > 1 ? argv[1] : "work/native/project/disc/TRACKS/TA.RES";
        d::Inputs input; input.capacity = 64u * 1024u * 1024u;
        const auto bytes = input.read(sourcePath, 64u * 1024u * 1024u);
        d::need(d::sha256(bytes) == "b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8", "Exact TA fixture identity");
        a::ResArchive48C archive; a::DecodeError48C error;
        d::need(a::decodeResource48C(bytes, archive, error), "Native RES decode");
        unsigned textures = 0, rejectedMutations = 0, arrowTexels = 0;
        for (const auto& e : f::detail::entries) {
            std::string name(e.member); std::replace(name.begin(), name.end(), '/', '\\');
            d::Bytes source; d::need(a::extractResMember48C(archive, name, source, error), "Native source extraction");
            d::Bytes output; std::uint32_t mip = 999; std::string why;
            d::need(f::decodeSourceTexture(e.member, source, output, mip, why), "Source callback succeeded");
            d::need(d::sha256(output) == e.outputSha256 && mip == e.mipIndex, "Callback output identity");
            ++textures;
            auto corrupt = source; corrupt[corrupt.size() / 2] ^= 1;
            d::Bytes sentinel{1, 2, 3}; std::uint32_t untouched = 999;
            d::need(!f::decodeSourceTexture(e.member, corrupt, sentinel, untouched, why) &&
                sentinel == d::Bytes({1, 2, 3}) && untouched == 999, "Changed source fails transactionally");
            ++rejectedMutations;
            d::need(!f::decodeSourceTexture("TEXTURES/UNREGISTERED.PST", source, sentinel, untouched, why) &&
                sentinel == d::Bytes({1, 2, 3}) && untouched == 999, "Unknown name fails transactionally");
            ++rejectedMutations;
            if (std::string(e.member) == "TEXTURES/ARROWRED.ATM") {
                // Full first PST extent is independently parsed by native48E.
                d::Bytes originalFrame(source.begin() + 12, source.begin() + 22872);
                a::Texture48E decoded;
                d::need(a::decodeTexture48E(originalFrame, decoded, error), "Native indexed first frame decode");
                for (std::size_t i = 0; i < decoded.mips.size(); ++i) {
                    d::Bytes native; d::need(a::encodeTextureLevel48E(decoded.mips[i], native, error), "Native mip encode");
                    const auto path = std::filesystem::path("evidence/textures50FC") /
                        ("arrowred-frame0-mip" + std::to_string(i) + ".rrtex");
                    auto retained = input.read(path, native.size());
                    d::need(native == retained, "Every original indexed texel equals native48E output");
                    arrowTexels += decoded.mips[i].width * decoded.mips[i].height;
                }
            }
        }
        std::cout << "PASS 50FC native source extraction/callback: " << textures << " textures; "
            << rejectedMutations << " rejected substitutions; " << arrowTexels << " exact indexed texels\n";
        return 0;
    } catch (const std::exception& e) { std::cerr << "FAIL: " << e.what() << '\n'; return 1; }
}
