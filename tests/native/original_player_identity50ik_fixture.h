#pragma once
// Cold identities for the eight retained originals. Digests identify source
// bytes, not generated output. Constructor21079C/21081C/210820 binds racer60
// to Pod%02d; the human's host slot never determines its resource/audio ID.
#include "../../src/game/original_racer_selection50ij.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace original_player_identity50IK {
struct Identity {
    std::uint32_t racer60;
    const char* podId;
    std::size_t archiveBytes;
    const char* archiveSha256;
    const char* phySha256;
    const char* cursesSha256;
    const char* tauntsSha256;
    const char* specialSha256;
    std::size_t announcerBytes;
    unsigned announcerEvents;
    const char* announcerSha256;
};
inline constexpr std::array<Identity,8> originals{{
    {1,"POD01",349110,
     "08b4d7655cb796da8a29d9b9a5318c0364561c659794ac9de6c9dfed7416eca3",
     "47fcfbaab278d10506b302520adc51af4e3388ceb601daa3dbd588aea1f39a1b",
     "928a76c4565ec9f6b1b02b74493d68d76c3e6d87079e860d7b3ebef9cef13530",
     "29b90e8f830a7ddab0113e1584db5075a0f7f7aba75d01b16f536edca4a35bfa",
     "237afa12cc59366f2f4ec6e7a2b2233accd25656ea082379f53218a78142e745",
     2403,29,"8c4f9b759c8fe933ab85341eff4d42e45e2c391e14f05ccad01238d04139a591"},
    {3,"POD03",328378,
     "4ad6a07ac02bc9ef65e336777ff27e6823060d333b0a885e99f14f58bd3ee26e",
     "147a9ed4d2449592bc6752fc6b44bdacff56120f1b19f1aa29864d3baf694d47",
     "3495668c8acf694239c31818e534cd6ed67392252c5c3f0a2a35524f91b13455",
     "6c14175eb500b4843b0bf18dfcd2e71a9f554b34c4fc564261ed986c9e8b1d7d",
     "3df60c54984509b6ca231d414ee59e9647406a0341676220b94007a9d4af230c",
     2496,30,"a3e18679511e73adc664978a0d75d334d710b9f7d0cdd2a53cde1e1b7383f7f7"},
    {4,"POD04",341925,
     "fd6546c9b05646b4f30462e973550abffd6249d8688e09173251fbd09aaac17a",
     "a1ca33400508684c6b8440f113658458c4efce666dcb68de5cdf1f1209845b81",
     "973875a4e2e7b444125d33fc35535fc48e3705d7fc6d3b38d569afa8877f6dac",
     "8774eab8444f75e11f29a1e3f74bcc5e42321dc923c067219d086190a5fb3010",
     "c6aa4313ed94a81f3eca97f50c7e05702a12dde21bd22c8a5ada923dd2d974c2",
     2496,30,"9933018d40d588764c0d8b11b7dc84f06fa4d66e3a0f6825678e02525db64504"},
    {7,"POD07",354874,
     "0cde5fac5d9a379e1ca426409809ba5c04eec6895013577f03639c69859fd8a8",
     "8615aca792d1efb68abcda048dd2b6cc9207c6c044300bf2306b6618edaffa48",
     "b64a320ffbe23f0676e327983470076c862b1c6bd40139e895ff82cbd0af3102",
     "d74158fb8097b441968fff62f83c3c5bc51a65aef3fa7d07b982cddf8cc5f952",
     "92e9203f008c2c0afc47087ca72d2b37625a93a542371182facc60a6247ca41d",
     2496,30,"ee0e83e2bc80e896c783d0f0f4058d381677faf8da7ce8f8e9cbfb06671d17e9"},
    {8,"POD08",363656,
     "9873d6cf7ce8b01fd76184859e043c1334d84b3962f987910e3d5b9ba4ccd86a",
     "73ee964a157938c301b8e0da0684c1245e00e723323a0c8ffaffa2642b12af98",
     "95f2e85cb30f28587c75504fc70b5944b040736670e1a74946614e0622f66110",
     "96daa7ff8d2315a5bc2be63783bd8d41a2c75a43817a0a6cf66054ae5657972f",
     "4f213f6d6a8f482477ec44ff92ff86aec918d5546ee4ce44a53b89a857341d44",
     2478,30,"60aef30ee566823c5d39a76d435aa436439a40ea4c0714cd8be24f2d2d189012"},
    {9,"POD09",348837,
     "5dc7b33724f703057f633794a51ae130cb8a976106a814f5777ddd1578416827",
     "d5aa6a3979b616e38d61e475369ea98216f351ca795f10f68e335be90cf50626",
     "1c5a4d381bfb82db40c174478937165e4721cad6bb80a3c6e01f4a9d57b357ee",
     "434e3e38a4d3b06b7bddbe0d5a873b3c41b0d1c4ea6bc05c7446a1166ff0bf5d",
     "daca02b9e09a6ee3877858c570ee3c9895db7eaca69f29380352b0ff74043fa6",
     2478,30,"65271132dcdfef5c385183ef55835989e7953e2f7fcf95f88a78c1069ddbb6e7"},
    {10,"POD10",342474,
     "8a29e7e26074d3549343d025317f754df6259fb6099e7b824f37463b30593a3c",
     "44b3339b338a3447014b2a2df464b1a54b39238256c11b754c1f94ab9c26c2f6",
     "dc7c7620c5765d8bb737ca58dc8a50da984870baf9f66becee957a270101cdb6",
     "625ed541ca57b17162eeff7bf79b267c7731d5a217b045456824f52ff2fbfc08",
     "39708bf9a60e2889e7e88208f90af7885138ebf4c67b3c61ba32c15e1113bbd1",
     2478,30,"46795b5e8b6fd052749d1a4a66dade491c80289a6698ba5ac6c9a32ee2a96305"},
    {11,"POD11",332761,
     "ce2a122c117137cae6cab2b20cc6132758c319186d79da5d0d26593d0713a81c",
     "fdee08787847253b29a83022bae0249427de5872a5ff409ef657a4c48fe12f12",
     "31ba56ca84e4aaf1dc70e8c9e4ef6b575eb236528651df1fbb293e101bcc0ca0",
     "26b025acda37d462ac5f83c02957ef332a513f34e83c7f27d4c5534d83b99ac6",
     "4cb7d5f99b1b61470ed7f7d409ecbdb92d2fbae0a4a30a1e1e32189118d6ed4b",
     2403,29,"128a15666d6a71f78be27e82624fe8694ca936836fcd8cc8e984ddf6d8a02939"}
}};
inline constexpr const Identity* find(std::uint32_t racer60) noexcept {
    for(const auto& entry:originals)if(entry.racer60==racer60)return &entry;
    return nullptr;
}
inline constexpr const Identity* find(std::string_view podId) noexcept {
    for(const auto& entry:originals)if(entry.podId==podId)return &entry;
    return nullptr;
}
inline std::filesystem::path archivePath(const std::filesystem::path& discRoot,const Identity& identity) {
    return discRoot/"PODS"/identity.podId/(std::string(identity.podId)+".RES");
}
inline std::filesystem::path announcerPath(const std::filesystem::path& discRoot,const Identity& identity) {
    return discRoot/"AUDIO/SPEECH/DAX/INRACE"/(std::string(identity.podId)+"ANN.INI");
}
// Seven independently retained originals author byte-identical counts. This
// does not make their category names/PCM interchangeable: those hashes differ.
inline constexpr std::array<std::uint8_t,9> speechCounts(const Identity& identity) noexcept {
    return identity.racer60==1?std::array<std::uint8_t,9>{4,4,6,5,6,6,4,3,2}:
        std::array<std::uint8_t,9>{2,3,3,3,4,4,3,3,3};
}
inline constexpr const char* speechCountsSha256(const Identity& identity) noexcept {
    return identity.racer60==1?"57639f26df9b7eac14d08e1aa0b67689cfe1674a03e52c1dbd856ba523a6834e":
        "f0b3b4950bd40f71736964ee6cdd9c6e97424ff84e6b57ca464170cb2ab72be2";
}
} // namespace original_player_identity50IK
