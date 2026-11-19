#pragma once
#include "../../src/assets/dlc_files50dl.h"
#include "../../src/assets/dlc_registry50dl.h"
// Pod identities for every pod, player and AI: ONE place defines the rule.
// Constructor21079C/21081C/210820 binds racer60 to Pod%02d; the human's host
// slot never determines its resource/audio ID.
//
// The original reads whatever PODS/PODnn/PODnn.RES the disc holds. The pod
// load (2129C0) opens the RES bank (12CEA0: header, block-size table, member
// directory, zlib blocks), finds members by name (132820) and reads the PHY
// and the speech INIs through Config (1577D0, [Indices] NumberOf*Sounds).
// Nothing checks a checksum, byte size or member count. The 23 stock rows in
// `originals` are therefore RECEIPTS, not admission gates: find() returns the
// identity of the pod archive actually on the disc (admission:: below) and
// logs one line "pod_archive_identity pod=.. bytes=.. stock=1|0". Every
// consumer keeps comparing the bytes it reads against that identity, so a
// file that changes after admission is still refused. A stock archive gets
// its stock row unchanged (same object, same values: bit-identical).
// The loose PODnnANN.INI announcer pins below are NOT part of this rule yet
// (no converted announcer exists); FLAG: retire them the same way when one does.
#include "../../src/game/original_racer_selection50ij.h"
#include "../../src/assets/import_decode48c.h"
#include "pod_materials50ed_fixture.h"
#include "runtime_asset_paths50lb.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

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
inline constexpr std::array<Identity,23> originals{{
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
     2403,29,"128a15666d6a71f78be27e82624fe8694ca936836fcd8cc8e984ddf6d8a02939"},
    {19,"POD19",388750,
     "7fd2b69f04351dc73d563527df8b67a1ed896c7d47f6a47fe9ee753f2394e826",
     "6267bb1fb33bd798a589c5afd64f7197211fa47a1a78a495f2859d150bd28874",
     "480758ea0adeeca293db02ccc197d9760acbe3faaf08cd216b5f281af312e388",
     "71177c4e7dee02bbc31ac0bb12958ba4a8df4e4f62e84755913454f8511b4670",
     "24a47cd627c6070ca586c699133373e7383971aa4fe006e277cfb107f6c344fa",
     2496,30,"464c99a77c4b7970e96e4c823d782971f6140d86570c76a00638d0c542c89cf0"},
    {2,"POD02",385218,
     "7cee08d049a47d088e8329ab63d622471299945d32f76ce9c9fb21b358e845e0",
     "cd5822d6a8a8ebde0bacf5849788540ef20b4a57f2abb9a71f9b8376fa7b869b",
     "68485fc4691a0294e70974a39b6ffa26eeaf8689ca81f3b28179b19029fdba70",
     "0d18fb1534e1744d3314d115473a752055506cc8d9dfa29b6aed7de205afef40",
     "ce4c18dac55809ce173309ecdb8bf2284046d2720d29ef63f76a9e3e5cf82ea7",
     2496,30,"40132dddcf4801ae128be8ffa82398489efec49e7b65f9036516a1667378eb62"},
    {12,"POD12",389534,
     "de144b1067eefd3037f4f0a7325beedc5c1c98e806b3802a08cdfec91bbf6f19",
     "7711c1d6523f6a6366e2cf29def88b51f5945dc7788528f970941b814b3d4ae0",
     "a1ea8aa445414ea7a5e3dacb1360a973d969ee4b07d8d15e504a2935f5cefeca",
     "b87553b5cbe9850cfcd412a64ed8c9eb3d5448758c56d067772a862b577a818b",
     "05eab8c605d0be40796c9681ca1cd2ecc04e2949b6b946f7d6bb1ecf983a61b7",
     2403,29,"ff89c669b057ba170f837685040ac9eb414ef73de25ed755d5fbb8db3246f8b9"},
    {15,"POD15",324173,
     "3501ca0a81e0485575c7a5ff217f3ac6a9248c15824d8dff9bbd1bc2f7aae326",
     "e1402abbadcc67420a10466392e8a3db18c9aa5196b7baf60c8e256746afa23a",
     "74822ac1b18a776ec62f6aa1032ca244da93b783aefd3b44a802035fa86d9c28",
     "c6f4f0b0eafc3a3829858f828b38544a2fabfdf6bdcbda0926737d1f9ca5ce51",
     "0ac3f057abb9bb8503d5c73cb49709598e6f2b74d0c4ace5bdebf8f953dc6be2",
     2478,30,"e089aff382fb352d8c62aa7f0eb288ec32d490886f05e0d05b393d298d7f1ee4"},
    {20,"POD20",309724,
     "8c5f282b53c230861e26acf797b8ab2ba1ef8856b8b476e3cfea55b48e3040ec",
     "a047bb90bb1275ad5d31e596cea5eaa02fd63ba9bbdc4ef931835f567c87d19d",
     "5e76e78f9deaec2af43a67baabb0005c778f874beb57e6d3999bc11d15e31d11",
     "8e01cc5d2a95859a9269d49bf217f2da1e75f64af7d621aaa74d4a63fed2b3fd",
     "f7f92d8446f162ada65cb0dfac270d292208e0cce00fb250f9252cb3118ff6d7",
     2403,29,"9a0f88d81772acda11fdbb7ed89285a09ebef0e600e6d5049d71cb7efac62efa"},
    {16,"POD16",325188,
     "faa14c5663a3aeb4e0b033bd61a5e826d9816a9fc43e638cdbb933dbbfd9bab1",
     "152445fa467efcb2b34af309ca72b2039b3f7101714904319fbd9b36285aa342",
     "8ce9c4738c5d8b83c9bd61ca9425aa61bdb813df15144cae3a82540854017c8e",
     "d43e53e5cbee7cecc95f5d471fd3dc285834d0994c3f42309e60ee996b48e7dd",
     "a77b15825f888596e16f2e30dc9302a06856289f65249d59a11668c1affd7737",
     2478,30,"156a10c28450fdd5d7e04e1606b836b5a2ec8eb3d952700a3a14cd21726922fb"},
    {17,"POD17",377255,
     "10a33edac421f915382c8b171c97c99978a5136789ae38abe9f6e97c68e4a51e",
     "8eb00002893ca0bdeef5a1cfde5681261cfb58ad7717b987688128572e685515",
     "b1341f4f820c1e95db6660874a82d904a288c6c0cccd4db3444d7af591f9cdd8",
     "cab3f02116b7e14afff9c72a86b8b61651fda8a6313ae320267e3275ea8a3bbf",
     "44dbc772336bf2eb2ec7c72514c7e0705c3b457145fe2abe0b15cc3de3aa1123",
     2478,29,"ff897119ce1d9eba85e7f9b69f1811bd9d182dfb87db6afe3f022442ec6a2398"},
    {18,"POD18",336720,
     "db065fdd6dd1c8dc9996191a07ff549f2fad1706701b69cbc881a470812834a1",
     "84af45e358253d76eac2910326adb54370da00ea36ffdbbf35408587dfe2df7a",
     "54d788c4eddcb7921f6d48599d758585f5d30ff27979a7dc5710971e559c4d6b",
     "75cff02a66638232ff6864d7d737341687935e20cc4031ec9264df0e96a3e25e",
     "7c890de154bf4f25399800e57a5789afd72b578fd6581ead37501f738d2a8427",
     2478,30,"18a17632e1c5fa1095316c65b50ef1544a138c7f4f0c57544805dd42a7b35daa"},
    {24,"POD24",365855,
     "6aab8cbb5184e56c413513f9e294d8bc67b07e04616ed91692a1e6df55150e2f",
     "36318ecabb93d4d71e1cd581946cfcaafaaa434ca3611bca490dbf32a6582328",
     "3deeade60edbd2b9ddbd99f40e057d205a209fd26e278ccb0289dda88ef07387",
     "a111f319cd742bc28962917028a3ce0934297424600c459f57564095e4618f58",
     "0238927008218ca2c9c663b07c149aaa131ea60c6712d8e70844ec688289eb3f",
     2478,30,"093c96f3c7b39a863ada2b11c93f76da348bf2d91b13c32ffeefc31b46654955"},
    {22,"POD22",376982,
     "6dc249bc0e9c7acb236e67bcfa817bd5a87e338e428c35690854a23cc532cabf",
     "0509b5eb4e572827fec66ab77e18d8d9216c2f9d46cb6a930f58bf7c912143a5",
     "c2a4122d47c2c5a0eea120a207f1cf657eb9ef728752f1c9d6dfd92458d77edd",
     "10fd2390804377f4b9e674d8d2c4df47797944b67ea27acfd96d6142edf32f0a",
     "958ed93d705ab47ee013a66462cd15f6f378e91c46c5ba4b61b38a26f5108dac",
     2478,30,"4b1437bae906f37fe3a341bd61e77aea636b23c4ade69327964b568950456094"},
    {23,"POD23",374646,
     "2edb056d14c79d70de96a2c5538f9ae03c9a7d2fcfd22bd49a02a3b0dbaa402d",
     "bff832abc9bfaefcc9f40e323440c6017869314b196d8825f050cba8791e56cd",
     "e61460979a3a1df75d1869a14221933afb4b1d83e83d2231d822644ecb6bb386",
     "351d0090b63bf83a29a943cd0cf8d6c8d74e742cfded2e6c7dbb0807bd994095",
     "a595c149cec3552a7c6ec1262b1ce4bde5fea61da6e5a440b3dfc9c2965ccaac",
     2478,30,"0f657db3f1808c4421e6ae3bbce1a69ebe82fae7378130d2369cca77672b82c7"} ,
    {14,"POD14",330051,
     "921f3e143e01256bf044a10174f1dfecd176914a3702b95e7985bcef65bb374f",
     "8c8961bd275e0cf38722454412b5e9028fada380d5bbe46ed2b9bfe5d87f8ac5",
     "76eb62330c38cb9d629c87e2462d355d2dcca1b85efb73443a982ccceeba639c",
     "94333d296378c84e0a69dfd6194558384de990c5bf0747ebe943df29882b1fe4",
     "c5920bfea8e3a76ab07eee135bf36d85616432a4f4ec58f5198902b287b034b8",
     2403,29,"aa2fb2f121397900daa9131acf8a3547717d81b078b2568c5cbce28c63de61fd"},
    {21,"POD21",378544,
     "7484042e95aa2f40d02ceeec91bfd59904076af3054fe4d797fe67773ed5f569",
     "80c112799e219387065697af821c8a856ac6f7da4fa4aa0491964383a6ed03da",
     "a7971091c45b6f570693cc81f0662e84746db52bed6adefe6bebe7cee387e119",
     "6e304d92b3c0e3eb35cc7d4ee7e483a9f7f80da416b0891f2222952a8e070116",
     "c927f511f3f521ea082ebbae9fe8232d76a6b40ad9c9516fe55f7bd4ef67c8c0",
     2403,29,"03abdf908838822ddb5f881dbf2377713cd0179c8d2823927c602e2c39923b3b"},
    {25,"POD25",362828,
     "ee8c2ee81f5303dcb114b1db57f6bac176a4338b30920b0617ad23dba748e224",
     "2cd531531650714d85259643a4a85ebc172dd332bf251fe1a44aa2dda84677a7",
     "d39202ff0fa18ace316daf1c6f5a8406ed564d47c0f513e3261b280399c8b0fc",
     "566e817ef5d812f24defa38f11a7202552306228a9fe2531e58ce38bd9a5dba3",
     "f1584648c1a5d4205b503c0044480a566085654e9c0649c568bdc0ff7f14beb8",
     2478,30,"245b78a21f4581c4f4429290880b09fd5cdba4addb1c01493654cfdf6eda8017"},
    {26,"POD26",360724,
     "857ef67eba2164509b7700aa51d2cbcf83745417adeaca175806e6bcdb89be7d",
     "695e94107b68747d25af0c6bd0aca5f75940968fffba09809ea5e2065b831415",
     "a59af01e25ddc0be7e2c9994fc8b5f2e4ecb608eae514c770dbdd37da933290e",
     "e08babe6168cd4aa5de1d8840fdbe805fbd3b85df33e86d3f4134cfaf34a5780",
     "6c1bf5e505b1cd6ec434513387d9bcd28aed94802173349df3897c7fedd486b9",
     2478,30,"4168d648c1714682a34c316845b4e927abad2432e9b0e746d868dd6bafc91a49"}
}};
// Stock receipt lookups (the shipped rows). Admission and every consumer use
// find() further down, never these, except to report "stock=1|0".
inline constexpr const Identity* stockFind(std::uint32_t racer60) noexcept {
    for(const auto& entry:originals)if(entry.racer60==racer60)return &entry;
    return nullptr;
}
// ---- One rule for a pod's files and member names, stock and add-on rows.
// A stock row keeps the original Pod%02d spelling and the disc paths exactly
// as before; an add-on row (PORT EXTENSION, NOT ORIGINAL: DLC registry, racer
// 32..99) names its package files (dlc:/ paths, opened through host()) and
// spells its stem where the original formats Pod%02d.
inline const rr::assets::dlc50DL::AddonPod* addonPodOf(std::uint32_t racer60) noexcept {
    return rr::assets::dlc50DL::addonNumberRange(racer60)?rr::assets::dlc50DL::addonPod(racer60):nullptr;
}
inline const rr::assets::dlc50DL::AddonPod* addonPodOf(std::string_view podId) noexcept {
    if(rr::game::original_racer_selection50IJ::stockPodId(podId))return nullptr;
    try{return rr::assets::dlc50DL::addonPodByPodId(std::string(podId));}catch(...){return nullptr;}
}
// A pod id the common pod load admits: the stock spelling or an add-on row.
inline bool admittedPodId(std::string_view podId) noexcept {
    return rr::game::original_racer_selection50IJ::stockPodId(podId)||addonPodOf(podId)!=nullptr;
}
// <podsRoot>/<id>/<id>.RES for a stock pod; the add-on row's package archive.
inline std::filesystem::path podArchive(const std::filesystem::path& podsRoot,const std::string& podId) {
    if(const auto* pod=addonPodOf(podId))return rr::assets::dlc50DL::host(pod->archive);
    return podsRoot/podId/(podId+".RES");
}
// <podsRoot>/<id>/<id>.CAM for a stock pod; the add-on row's package camera.
inline std::filesystem::path podCamera(const std::filesystem::path& podsRoot,const std::string& podId) {
    if(const auto* pod=addonPodOf(podId))return rr::assets::dlc50DL::host(pod->camera);
    return podsRoot/podId/(podId+".CAM");
}
// [Pod Setup] Prefix the PHY carries: the two digits of a stock pod, the stem
// of an add-on pod (dlc-contract-2: Prefix must equal the stem).
inline std::string phyPrefix(const std::string& podId) {
    if(const auto* pod=addonPodOf(podId))return pod->stem;
    return podId.size()>=3?podId.substr(3):std::string{};
}
// Pod folder member prefix "Pods\Pod04\" (stock, as before) or "Pods\<stem>\".
inline std::string podFolderMember(const std::string& podId) {
    if(const auto* pod=addonPodOf(podId))return "Pods\\"+pod->stem+"\\";
    return "Pods\\Pod"+(podId.size()>=3?podId.substr(3):std::string{})+"\\";
}
inline std::filesystem::path archivePath(const std::filesystem::path& discRoot,const Identity& identity) {
    if(const auto* pod=addonPodOf(identity.racer60))return rr::assets::dlc50DL::host(pod->archive);
    return discRoot/"PODS"/identity.podId/(std::string(identity.podId)+".RES");
}
// An add-on row has no named announcer INI (generic lines only): empty path,
// announcerBytes 0. Readers skip the named bank for such a row.
inline std::filesystem::path announcerPath(const std::filesystem::path& discRoot,const Identity& identity) {
    if(addonPodOf(identity.racer60))return {};
    return discRoot/"AUDIO/SPEECH/DAX/INRACE"/(std::string(identity.podId)+"ANN.INI");
}
// Stock receipt: what each shipped sounds.ini [Indices] says. Eleven
// independently authored originals have byte-identical counts; their category
// names/PCM still differ. Admitted pods take their counts from their own
// sounds.ini (admission:: below); these rows only feed the stock receipt.
inline constexpr std::array<std::uint8_t,9> stockSpeechCounts(const Identity& identity) noexcept {
    if(identity.racer60==14)return {2,3,3,3,4,4,3,3,3};
    if(identity.racer60==21)return {2,4,3,5,3,5,3,3,3};
    if(identity.racer60==25)return {3,2,5,4,5,4,5,5,1};
    if(identity.racer60==26)return {1,4,3,5,5,3,3,1,2};
    if(identity.racer60==19)return {5,6,7,6,6,6,3,4,4};
    return identity.racer60==1?std::array<std::uint8_t,9>{4,4,6,5,6,6,4,3,2}:
        std::array<std::uint8_t,9>{2,3,3,3,4,4,3,3,3};
}
inline constexpr const char* stockSpeechCountsSha256(const Identity& identity) noexcept {
    if(identity.racer60==14)return "f0b3b4950bd40f71736964ee6cdd9c6e97424ff84e6b57ca464170cb2ab72be2";
    if(identity.racer60==21)return "a8113a836b77c3fe9c4cc7aa1cbeba5dcdc34b6b9c4c90b6ba3990b5cef19333";
    if(identity.racer60==25)return "5bb23574bb9c0d3be740d67153091ac5aa767dee0e6e97cfe35f5e3411a9f799";
    if(identity.racer60==26)return "15fa058384528f9cd2ecfee8095276b7047b9f888f76452c9df5e22c534b46bd";
    if(identity.racer60==19)return "24c40bf4fc181e88c0361742dec94c76d91c522ba358c5533bde2769fdfaa3ec";
    return identity.racer60==1?"57639f26df9b7eac14d08e1aa0b67689cfe1674a03e52c1dbd856ba523a6834e":
        "f0b3b4950bd40f71736964ee6cdd9c6e97424ff84e6b57ca464170cb2ab72be2";
}

// ---- Common pod-archive admission (the one rule; host memory only) ----
namespace admission {
// Shared pod-archive capacity, the one place it is defined. These are the
// tightest native caps any pod reader already applies (selected-player audio
// decode: 512 entries, 16 MiB decoded, 256-byte names; INI/PHY members are
// extracted with an 8 KiB cap by every pod reader). The file itself may not
// exceed the decoded cap. Every pod reader bounds its own writes to these.
// The values live in rr::assets::dlc50DL::RegistryLimits (one owner for the
// stock and add-on pod-archive capacity): 16 MiB file and decoded, 512
// entries, 256-byte names, 8 KiB text members.
inline rr::assets::DecodeLimits48C limits() noexcept {
    using L=rr::assets::dlc50DL::RegistryLimits;
    rr::assets::DecodeLimits48C l;
    l.resource_file_bytes=L::podArchiveBytes;l.resource_decoded_bytes=L::podDecodedBytes;
    l.resource_entries=L::podEntries;l.resource_name_bytes=L::podNameBytes;
    return l;
}
inline constexpr std::size_t maximumTextMemberBytes=rr::assets::dlc50DL::RegistryLimits::textMemberBytes;
inline constexpr const char* speechKeys[9]={
    "numberoflowdamagesounds","numberofwallhitsounds","numberofpassedsounds",
    "numberofattackedsounds","numberofpassingsounds","numberofattackingsounds",
    "numberofdeathsounds","numberofwinsounds","numberoflosesounds"};
// One admitted row per stock slot (the racer60 -> Pod%02d binding is the
// original's own data). Strings are owned here and never change after the
// row is built, so the Identity pointers into them stay valid for the process.
struct Row {
    Identity identity{};
    bool stock=true;
    std::array<std::uint8_t,9> speech{};
    std::string archive,phy,curses,taunts,special,sounds;
};
inline std::string folded(std::string_view s) {
    std::string out(s);
    for(auto& c:out){if(c=='/')c='\\';else if(c>='A'&&c<='Z')c=char(c+('a'-'A'));}
    return out;
}
inline std::string_view trimmed(std::string_view t) noexcept {
    while(!t.empty()&&(t.front()==' '||t.front()=='\t'||t.front()=='\r'))t.remove_prefix(1);
    while(!t.empty()&&(t.back()==' '||t.back()=='\t'||t.back()=='\r'))t.remove_suffix(1);
    return t;
}
// sounds.ini [Indices] NumberOf*Sounds, as Config reads them. Each count is
// stored in a uint8_t (PodSpeechCounts50HG); a larger value is refused here,
// before any store. Missing or repeated keys are refused (every port speech
// reader requires all nine).
inline std::array<std::uint8_t,9> speechFrom(const std::vector<std::uint8_t>& ini) {
    std::array<std::uint8_t,9> out{};std::array<bool,9> seen{};
    const std::string_view text(reinterpret_cast<const char*>(ini.data()),ini.size());
    bool indices=false;
    for(std::size_t at=0;at<text.size();){
        auto end=text.find('\n',at);if(end==std::string_view::npos)end=text.size();
        const auto line=trimmed(text.substr(at,end-at));at=end+1;
        if(line.empty()||line.front()==';')continue;
        if(line.front()=='['){indices=line.size()>=2&&line.back()==']'&&folded(line.substr(1,line.size()-2))=="indices";continue;}
        if(!indices)continue;
        const auto equal=line.find('=');if(equal==std::string_view::npos)continue;
        const auto key=folded(trimmed(line.substr(0,equal)));const auto value=trimmed(line.substr(equal+1));
        for(std::size_t i=0;i<9;++i)if(key==speechKeys[i]){
            if(seen[i])throw std::runtime_error("sounds.ini repeats "+key);
            if(value.empty()||value.size()>3)throw std::runtime_error("sounds.ini count extent "+key);
            unsigned v=0;for(const char c:value){if(c<'0'||c>'9')throw std::runtime_error("sounds.ini count "+key);v=v*10u+unsigned(c-'0');}
            if(v>255u)throw std::runtime_error("sounds.ini count exceeds its uint8 storage "+key);
            out[i]=static_cast<std::uint8_t>(v);seen[i]=true;
        }
    }
    for(std::size_t i=0;i<9;++i)if(!seen[i])throw std::runtime_error(std::string("sounds.ini lacks ")+speechKeys[i]);
    return out;
}
// The members the pod load reads by name (132820), found by the pod's member
// stem ("pod04", or an add-on stem): speech counts and digests of its PHY and
// speech INIs. Throws when the archive cannot be decoded within the shared
// capacity or lacks one of them.
inline void readMembers(const std::vector<std::uint8_t>& bytes,const std::string& pod,
    rr::assets::ResArchive48C& archive,Row& next) {
    namespace d=pod_materials50ed_fixture::detail;
    const auto lim=limits();
    rr::assets::DecodeError48C why;
    if(!rr::assets::decodeResource48C(bytes,archive,why,nullptr,lim))throw std::runtime_error("archive decode "+why.code);
    const auto member=[&](const std::string& wanted){
        const auto key=folded(wanted);const rr::assets::ResourceEntry48C* found=nullptr;
        for(const auto& e:archive.entries)if(folded(e.name)==key){
            if(found)throw std::runtime_error("ambiguous member "+wanted);found=&e;}
        if(!found)throw std::runtime_error("archive lacks "+wanted);
        std::vector<std::uint8_t> out;
        if(!rr::assets::extractResMember48C(archive,found->name,out,why,maximumTextMemberBytes))
            throw std::runtime_error("member outside the 8 KiB text capacity "+wanted);
        return out;
    };
    const auto speech="audio\\speech\\"+pod+"\\";
    const auto sounds=member(speech+"sounds.ini");
    next.speech=speechFrom(sounds);
    next.sounds=d::sha256(sounds);
    next.phy=d::sha256(member("pods\\"+pod+"\\"+pod+".phy"));
    next.curses=d::sha256(member(speech+"curses.ini"));
    next.taunts=d::sha256(member(speech+"taunts.ini"));
    next.special=d::sha256(member(speech+"special.ini"));
}
// Reads the pod archive on the disc and fills the row. Stock bytes keep the
// stock row; any other archive gets a row derived from its own data: size
// and digest of the file, digests of its PHY and speech INIs found by name,
// speech counts from its sounds.ini. Throws when the archive cannot be read
// within the shared capacity or lacks a member the pod load needs; the caller
// then keeps the stock row, so every consumer refuses the bytes as before.
inline void derive(Row& row) {
    namespace d=pod_materials50ed_fixture::detail;
    const Identity stock=row.identity; // a copy: row is replaced below
    const auto lim=limits();
    const auto path=rr::assets::dlc50DL::host(archivePath(runtime_asset_paths50LB::resolve("work/native/project/disc"),stock));
    std::error_code error;const auto size=std::filesystem::file_size(path,error);
    if(error)throw std::runtime_error("archive unreadable");
    if(!size||size>lim.resource_file_bytes)throw std::runtime_error("archive outside the shared pod capacity");
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
    {std::ifstream in(path,std::ios::binary);
     in.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
     if(!in||in.peek()!=std::char_traits<char>::eof())throw std::runtime_error("archive read/extent");}
    const auto digest=d::sha256(bytes);
    if(bytes.size()==stock.archiveBytes&&digest==stock.archiveSha256){
        std::cerr<<"pod_archive_identity pod="<<stock.podId<<" bytes="<<bytes.size()<<" stock=1\n";
        return;
    }
    rr::assets::ResArchive48C archive;
    Row next;next.identity=stock;next.stock=false;
    readMembers(bytes,folded(stock.podId),archive,next);
    next.archive=digest;
    row=std::move(next);
    auto& id=row.identity;
    id.archiveBytes=bytes.size();id.archiveSha256=row.archive.c_str();id.phySha256=row.phy.c_str();
    id.cursesSha256=row.curses.c_str();id.tauntsSha256=row.taunts.c_str();id.specialSha256=row.special.c_str();
    std::cerr<<"pod_archive_identity pod="<<id.podId<<" bytes="<<id.archiveBytes<<" stock=0 members="
             <<archive.entries.size()<<" phy_stock="<<(row.phy==stock.phySha256?1:0)<<" speech=";
    for(std::size_t i=0;i<9;++i)std::cerr<<(i?",":"")<<unsigned(row.speech[i]);
    std::cerr<<'\n';
}
inline const Row& row(std::size_t slot) noexcept {
    static std::array<Row,originals.size()> rows;
    static std::array<std::once_flag,originals.size()> once;
    std::call_once(once[slot],[slot]{
        auto& r=rows[slot];r.identity=originals[slot];r.stock=true;r.speech=stockSpeechCounts(originals[slot]);
        try{derive(r);}
        catch(const std::exception& e){
            r=Row{};r.identity=originals[slot];r.stock=true;r.speech=stockSpeechCounts(originals[slot]);
            try{std::cerr<<"pod_archive_identity pod="<<originals[slot].podId<<" refused="<<e.what()<<'\n';}catch(...){}
        }catch(...){r=Row{};r.identity=originals[slot];r.stock=true;r.speech=stockSpeechCounts(originals[slot]);}
    });
    return rows[slot];
}
inline std::size_t slotOf(std::uint32_t racer60) noexcept {
    for(std::size_t i=0;i<originals.size();++i)if(originals[i].racer60==racer60)return i;
    return originals.size();
}
// PORT EXTENSION, NOT ORIGINAL: an add-on pod's identity (DLC registry row,
// racer 32..99), built from its own data the way derive() builds one for a
// non-stock disc archive: the package archive's bytes and digest (it must be
// the file the registry admitted), PHY and speech INIs found by its stem,
// speech counts from its sounds.ini. No named announcer (generic lines only).
// Host memory only; one row per registry number, built once.
inline void deriveAddon(Row& row,const rr::assets::dlc50DL::AddonPod& pod) {
    namespace d=pod_materials50ed_fixture::detail;
    const auto lim=limits();
    const auto path=rr::assets::dlc50DL::host(pod.archive);
    std::error_code error;const auto size=std::filesystem::file_size(path,error);
    if(error)throw std::runtime_error("archive unreadable");
    if(!size||size>lim.resource_file_bytes)throw std::runtime_error("archive outside the shared pod capacity");
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
    {std::ifstream in(path,std::ios::binary);
     in.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
     if(!in||in.peek()!=std::char_traits<char>::eof())throw std::runtime_error("archive read/extent");}
    const auto digest=d::sha256(bytes);
    if((pod.archiveBytes&&bytes.size()!=pod.archiveBytes)||(!pod.archiveSha256.empty()&&digest!=pod.archiveSha256))
        throw std::runtime_error("archive differs from the admitted package file");
    rr::assets::ResArchive48C archive;
    Row next;next.stock=false;
    readMembers(bytes,pod.stem,archive,next);
    next.archive=digest;
    row=std::move(next);
    auto& id=row.identity;
    id.racer60=pod.number;id.podId=pod.podId.c_str(); // frozen registry storage
    id.archiveBytes=bytes.size();id.archiveSha256=row.archive.c_str();id.phySha256=row.phy.c_str();
    id.cursesSha256=row.curses.c_str();id.tauntsSha256=row.taunts.c_str();id.specialSha256=row.special.c_str();
    id.announcerBytes=0;id.announcerEvents=0;id.announcerSha256="";
    std::cerr<<"pod_archive_identity pod="<<id.podId<<" bytes="<<id.archiveBytes<<" stock=0 addon=1 key="<<pod.key
             <<" members="<<archive.entries.size()<<" speech=";
    for(std::size_t i=0;i<9;++i)std::cerr<<(i?",":"")<<unsigned(row.speech[i]);
    std::cerr<<" announcer=generic\n";
}
// nullptr for any number that is not an admitted add-on pod, or one whose
// package archive was refused (logged once).
inline const Row* addon(std::uint32_t racer60) noexcept {
    namespace reg=rr::assets::dlc50DL;
    if(!reg::addonNumberRange(racer60))return nullptr;
    const auto* pod=reg::addonPod(racer60);
    if(!pod)return nullptr;
    struct Slot {std::once_flag once;Row row;bool ready=false;};
    static std::array<Slot,reg::RegistryLimits::pods> rows;
    auto& slot=rows[racer60-reg::RegistryLimits::firstNumber]; // bounds: addonNumberRange above
    try{std::call_once(slot.once,[&]{
        try{deriveAddon(slot.row,*pod);slot.ready=true;}
        catch(const std::exception& e){
            slot.row=Row{};
            try{std::cerr<<"pod_archive_identity pod="<<pod->podId<<" addon=1 key="<<pod->key<<" refused="<<e.what()<<'\n';}catch(...){}
        }catch(...){slot.row=Row{};}
    });}catch(...){return nullptr;}
    return slot.ready?&slot.row:nullptr;
}
} // namespace admission

// Every consumer: the identity of the pod archive on the disc. A stock
// archive (or one that cannot be admitted) returns the stock row itself.
// An add-on racer number returns its package row (admission::addon).
inline const Identity* find(std::uint32_t racer60) noexcept {
    const auto slot=admission::slotOf(racer60);
    if(slot==originals.size()){const auto* a=admission::addon(racer60);return a?&a->identity:nullptr;}
    const auto& r=admission::row(slot);
    return r.stock?&originals[slot]:&r.identity;
}
inline const Identity* find(std::string_view podId) noexcept {
    for(const auto& entry:originals)if(entry.podId==podId)return find(entry.racer60);
    if(const auto* pod=addonPodOf(podId))return find(pod->number);
    return nullptr;
}
inline std::array<std::uint8_t,9> speechCounts(const Identity& identity) noexcept {
    const auto slot=admission::slotOf(identity.racer60);
    if(slot==originals.size()){
        if(const auto* a=admission::addon(identity.racer60))return a->speech;
        return stockSpeechCounts(identity);
    }
    const auto& r=admission::row(slot);
    return r.stock?stockSpeechCounts(originals[slot]):r.speech;
}
inline const char* speechCountsSha256(const Identity& identity) noexcept {
    const auto slot=admission::slotOf(identity.racer60);
    if(slot==originals.size()){
        if(const auto* a=admission::addon(identity.racer60))return a->sounds.c_str();
        return stockSpeechCountsSha256(identity);
    }
    const auto& r=admission::row(slot);
    return r.stock?stockSpeechCountsSha256(originals[slot]):r.sounds.c_str();
}
} // namespace original_player_identity50IK
