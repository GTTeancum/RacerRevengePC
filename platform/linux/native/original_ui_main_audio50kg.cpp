#include "original_ui_main_audio50kg.h"
#include "original_sound_mode50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace rr::platform { namespace {
using Bytes = std::vector<std::uint8_t>;
void need(bool value, const char* message) { if (!value) throw std::runtime_error(message); }
// Same retained-byte SHA256 routine already used by audio_assets50hc.cpp.
inline std::uint32_t rotr(std::uint32_t x, unsigned n) { return (x >> n) | (x << (32 - n)); }
inline std::string sha256(const Bytes& input) {
    static const std::uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
    std::array<std::uint32_t,8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    Bytes data(input);
    const auto bits = static_cast<std::uint64_t>(data.size()) * 8;
    data.push_back(0x80);
    while (data.size() % 64 != 56) data.push_back(0);
    for (int i = 7; i >= 0; --i) data.push_back(static_cast<std::uint8_t>(bits >> (i * 8)));
    for (std::size_t at = 0; at < data.size(); at += 64) {
        std::uint32_t w[64]{};
        for (unsigned i = 0; i < 16; ++i)
            for (unsigned j = 0; j < 4; ++j) w[i] = (w[i] << 8) | data[at+i*4+j];
        for (unsigned i = 16; i < 64; ++i) {
            const auto s0 = rotr(w[i-15],7) ^ rotr(w[i-15],18) ^ (w[i-15] >> 3);
            const auto s1 = rotr(w[i-2],17) ^ rotr(w[i-2],19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        auto v = h;
        for (unsigned i = 0; i < 64; ++i) {
            const auto s1 = rotr(v[4],6) ^ rotr(v[4],11) ^ rotr(v[4],25);
            const auto ch = (v[4] & v[5]) ^ (~v[4] & v[6]);
            const auto t1 = v[7] + s1 + ch + k[i] + w[i];
            const auto s0 = rotr(v[0],2) ^ rotr(v[0],13) ^ rotr(v[0],22);
            const auto maj = (v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]);
            for (unsigned j = 7; j > 0; --j) v[j] = v[j-1];
            v[4] += t1; v[0] = t1 + s0 + maj;
        }
        for (unsigned i = 0; i < 8; ++i) h[i] += v[i];
    }
    std::ostringstream stream;
    stream << std::hex << std::setfill('0');
    for (const auto word : h) stream << std::setw(8) << word;
    return stream.str();
}

struct OriginalFile { const char* path; std::size_t bytes; const char* sha; std::uint32_t frames; };
constexpr std::array<OriginalFile,7> originals{{
    {"MUSIC/UI/SCREENS/UI_MAIN.VAG",441024,"50617b8f8029f3fa80744a9de62d7c1987ea0e2d4b35487b8ce7ffd34401934a",384104},
    {"SFX/UI/SELECT.VAG",19424,"6688b65b124428d51cc1e3b531426c417be1bc066a8d560ffe0e7384b48d389a",33880},
    {"SFX/UI/LEFT.VAG",736,"b351f9789f73e8a1d41c954d0238d9b7fb93e257a55fea13b798ec60abb9c8eb",1176},
    {"SFX/UI/RIGHT.VAG",608,"6f515380394424ca812087219c7dad6928c535a68dfce1fd7cbccbb0683fe206",952},
    {"SFX/UI/BADCALL.VAG",6240,"f7d9fa2932f0354a472412c8f3ceea6cf7215f702a5d2ef6abb19590170ac385",10808},
    {"SFX/UI/BACK.VAG",11568,"3306a9ef40ab752a26a645f28f96ab61d2a7bb91b7a83c989496bd22e7a24b18",20132},
    {"SFX/UI/ADVANCE.VAG",2864,"e49eedb1fdfdc1c1ecf13d2d34f0357a5be77835f5293b295f4811050cd077ec",4900}
}};
constexpr OriginalFile riderOriginal{
    "MUSIC/UI/SCREENS/UI_PEOPL.VAG",709504,
    "2ac7db7a81ebcad40afa37998ac3e1dba16d5cd774d0da464db03faa411ed9e4",619024};
constexpr OriginalFile courseOriginal{
    "MUSIC/UI/SCREENS/UI_TRKS.VAG",448480,
    "b00ea0f7d26f88970eaa87899b5e185b821d82a2bd01a07a33ccf14ded42b40d",390628};
// UIACCEPT.INI exact events for default-unlocked ranges and retained additions.
// Clegg and Mars keep their own original Event7..8 and17..18 identities.
// These retained audio owners do not grant profile or Course admission.
// Retained 48-byte zero-header VAGs, 22050 Hz mono: frames include the
// first end-flag block (28 samples each), excluding the final padding block.
// Counts and SHA256s come from raw headers/end flags and original bytes.
struct OriginalRiderConfirmFile { std::uint32_t eventId; OriginalFile file; };
constexpr std::array<OriginalRiderConfirmFile,OriginalUiMainAudio50KG::riderConfirmClipCount> riderConfirmOriginals{{
    // Original Rider14 confirm clips, authenticated independently.
    {29,{"SPEECH/UI/SEBSEL01.VAG",30320,"8d2473facab3023d2f4fb98512d8ebb24ed3d34b578a52a153828e8bb761df34",52948}},
    {30,{"SPEECH/UI/SEBSEL02.VAG",79152,"583965165f87ced323bcb73d001d5754d4449fa476a3c379a886b7fe65c35773",138404}},
    // Original Rider21 confirm clips, authenticated independently.
    {46,{"SPEECH/UI/MAUSEL01.VAG",39120,"2dbcb2cda403ed0292e4111d890907d3e5fa481d1dffabe5d4a8aa69770a6cc8",68348}},
    {47,{"SPEECH/UI/MAUSEL02.VAG",31248,"3481b32d847e60f7a47a22912af3a7734249a8cf04752159b9019e3d557d1d69",54572}},
    {48,{"SPEECH/UI/MAUSEL03.VAG",41488,"d12bdf94e2b4abf25ba36d124010ccb2070db14f7d79c6a35046d5aec71e769a",72492}},
    {49,{"SPEECH/UI/MAUSEL04.VAG",44336,"eda30c9bfc1db34d081527dcdfba05a1dcbd061d53e848a396ec9b39ee140c8f",77476}},
    // Original Rider25 confirm clips, authenticated independently.
    {56,{"SPEECH/UI/WATSEL01.VAG",42976,"449364bd9cd7fef32d72143bef931e7fd05602af47dbf24ad165cd2d1c3a1a2e",75096}},
    {57,{"SPEECH/UI/WATSEL02.VAG",53856,"5e1a7253dfd349300c0a749378240e55af3dc63121d32e3a0d1dc1e28ae3be84",94136}},
    {58,{"SPEECH/UI/WATSEL03.VAG",65536,"01a2aa5faac307888d7fe786e3d69a3b2e93fe142778c9494c19c115881e6c50",114576}},
    {59,{"SPEECH/UI/WATSEL04.VAG",60736,"91eb98ceeeb5a8ab377416bef271120c9e5b75d6fb91440d9560131f232634ca",106176}},
    // Original Rider26 confirm clips, authenticated independently.
    {60,{"SPEECH/UI/VADSEL01.VAG",34416,"1e121160449447c40551680fcfd7078d0cdde6ab8dea3eadf099db6c5faa08f4",60116}},
    {61,{"SPEECH/UI/VADSEL02.VAG",46400,"7da71c9ad02eea29ad574e7ff681a8a4920a2358e60fc4648c9c8049f79a738f",81088}},
    {62,{"SPEECH/UI/VADSEL03.VAG",45072,"fe43302c1f6df8e3ee8360af51acff87b788400cc4506f5070b5b0e7964654d7",78764}},
    {63,{"SPEECH/UI/VADSEL04.VAG",38608,"90f73066fb9a020550cf8d98b548b44ae6a0749a94f5c484337fdba0415b271c",67452}},
    // Rider1: Anakin Skywalker (ep1), Event1..4. No profile unlock.
    {1,{"SPEECH/UI/AN1SEL01.VAG",10512,"1f18ec7a0b1abd953659fa2490a69e9a9bec025b764cce4d3a81756442b16967",18284}},
    {2,{"SPEECH/UI/AN1SEL02.VAG",16544,"9366c4771314c25a7955a648dc1912802f27f6b4604d0a1b9054c735deed41d5",28840}},
    {3,{"SPEECH/UI/AN1SEL03.VAG",12992,"1a94a2c61af57c35dfbb88351e175128fdc6283228e344f04a6b68442a1140bc",22624}},
    {4,{"SPEECH/UI/AN1SEL04.VAG",19328,"8dbd5571eecb2d8d34d39e2b5f671f80f407cc69d8a858cba73ebc90b4773902",33712}},
    // Rider2: Ben Quadinaros, Event5..6.
    {5,{"SPEECH/UI/BENSEL01.VAG",18736,"b74f9db0200e94509e85e5c2280c8e7fa5ecb49aa0de721f31b2e4a5a78ed9b2",32676}},
    {6,{"SPEECH/UI/BENSEL02.VAG",14304,"5a90c499cf9e04ee270ea1bb435ee53790c8464b244bd4846843e34223049646",24920}},
    // Rider3: Clegg Holdfast, Event7..8. No profile unlock.
    {7,{"SPEECH/UI/CLESEL01.VAG",27952,"23f773c2e56e91b00de2f70620cb4378410a95df6dfc27de8b91267395886327",48804}},
    {8,{"SPEECH/UI/CLESEL02.VAG",26608,"94bbf2246149ac265ce9e8845f062c82ee56ecd16844dc91b4ad0fe024a455f3",46452}},
    // Rider4: Dud Bolt, Event9..10.
    {9,{"SPEECH/UI/DUDSEL01.VAG",31696,"4b711f32ba5d299a728fe04f0b2dc802caeceae955de94727986a39118cf84b6",55356}},
    {10,{"SPEECH/UI/DUDSEL02.VAG",13664,"ebeaa61ec7190e7b6d97d7c8ea83b563ab3c853062481e619be3bd7445f621f4",23800}},
    // Rider7: Gasgano, Event15..16. First-end blocks1661/1652 inclusive.
    {15,{"SPEECH/UI/GASSEL01.VAG",26656,"4076d3b4943d5828a0b7977ca0bc9a6e1fdffc1e4a0925f41aed560713183c90",46536}},
    {16,{"SPEECH/UI/GASSEL02.VAG",26512,"c647576fb9e438d58b921142c0592041ba2a9ccb0a9db539e0f6ceeb34eb28e9",46284}},
    // Rider8: Mars Guo, Event17..18. No profile unlock.
    {17,{"SPEECH/UI/MARSEL01.VAG",18304,"6fe7952e114a01b0a9afae0124aab1146e96c8e2bdfee2866e4a60abd3255bfe",31920}},
    {18,{"SPEECH/UI/MARSEL02.VAG",37552,"c0744388aeac62a7c4b57fe55199553fedc18a9c71aa1106229b37678e7627f8",65604}},
    // Rider9: Mawhonic, Event19..20. No profile unlock.
    {19,{"SPEECH/UI/MAWSEL01.VAG",21312,"9d754175fee763ab3bbe32f33a32c5acdc104ee21719d92e086c6cd6c0240bfd",37184}},
    {20,{"SPEECH/UI/MAWSEL02.VAG",16656,"d03bd2ae5bcc5441cd7e5bfa98c358f531f102fa011d165c62e0b8918f71131d",29036}},
    // Rider10: Aldar Beedo, Event21..22.
    {21,{"SPEECH/UI/ALDSEL01.VAG",36496,"029332c4f2eba4b37dad0a6fb007f849a607b08bdd686d6976714d3164eab748",63756}},
    {22,{"SPEECH/UI/ALDSEL02.VAG",18208,"71a6c0b1e4fd790d81d4580a0903dac2d846961a7c99e520ff5512264982560d",31752}},
    // Rider11: Ody Mandrell, Event23..24.
    {23,{"SPEECH/UI/ODYSEL01.VAG",14288,"3c8261022bd7aa5417ba9a84e72d5a48b9bdc65b7c760b3e6320b2a54a5c7303",24892}},
    {24,{"SPEECH/UI/ODYSEL02.VAG",17440,"af414fcc3607a2f5710b2c76c4926c1bdef623bd4f8e5667e72b2f6245c27fec",30408}},
    // Rider12: Shrivel Braittrand, Event25..26.
    {25,{"SPEECH/UI/SHRSEL01.VAG",25472,"2c4eb20e3d383daee86599a28ed8d1e7ff190fa6dd3ea46ffe8f919ef1906f52",44464}},
    {26,{"SPEECH/UI/SHRSEL02.VAG",28704,"333c3f5953c731de3ec02535d56f7c810225d6600883e833bfe7f83dee109345",50120}},
    // Rider15: Teemto Pagalies, Event31..32.
    {31,{"SPEECH/UI/TEESEL01.VAG",27008,"40a3f4dda0aae7ef846fa8c49311b17dfbeec7a9eea586d0113933dca6d3f826",47152}},
    {32,{"SPEECH/UI/TEESEL02.VAG",32176,"e93fa4c842954cbb59792b56cc56a8b04314fbffeb1283a2e5706eee8813e0cf",56196}},
    // Rider16: Wan Sandage, original Event33..34. One existing Confirm voice.
    {33,{"SPEECH/UI/WANSEL01.VAG",47536,"c491692a4e115252d10c983f63757273638fb192b3232682e8e783a075d645f6",83076}},
    {34,{"SPEECH/UI/WANSEL02.VAG",42128,"3fec9bc0636e6edbad8a15125f9200354cee146112fed29b9f502af415fc782b",73612}},
    // Rider19: Anakin Skywalker, Event39..43.
    {39,{"SPEECH/UI/A2ASEL01.VAG",25488,"854e09eaf3d5eb28bfccfc35e8a869b5fc10515b6e6c4a3a8dc11dee50cde698",44492}},
    {40,{"SPEECH/UI/A2ASEL02.VAG",19232,"7d11a96798382bde857a80d994317e92ec3a4fc57108c2de9b253e21ad519393",33544}},
    {41,{"SPEECH/UI/A2ASEL03.VAG",18096,"f79ef5212c57bb313c88002bb595352054a2dbb54e059deb8d2dd071ef65ec8c",31556}},
    {42,{"SPEECH/UI/A2ASEL04.VAG",27520,"36f8c4c4c756a36ad6b21c9cd2f4bd083efc53e22f31aa022a8ab1e456e921df",48048}},
    {43,{"SPEECH/UI/A2ASEL05.VAG",17296,"d70fd36cdc0981560f940ad9d74eed809c44cc0a7b05fd8d73e869727a890034",30156}},
    // Rider20: Occo Ninebar, Event44..45.
    {44,{"SPEECH/UI/OCCSEL01.VAG",24384,"f826ff76992d67646856ce9baa455400e807766edf77716d228ef9f7b6931f85",42560}},
    {45,{"SPEECH/UI/OCCSEL02.VAG",29712,"d0c0af3755a80d7840aa3f52b2f1903e3191c4ace12fe9b6c060bcb8573ce41e",51884}},
    // Rider22: Knire Dark, Event50..51. No profile unlock.
    {50,{"SPEECH/UI/KNISEL01.VAG",13472,"3b8f320f75d5ad7a565999f331270d5404d36ac1ada66b0d387ce6db1651592e",23464}},
    {51,{"SPEECH/UI/KNISEL02.VAG",29120,"caf0470db721c4d3fb582356650056fd436aaf7b113827d454539e7224af6c7c",50848}},
    // Rider23: own Event52..53. Catalog entries do not unlock the racer.
    {52,{"SPEECH/UI/KRASEL01.VAG",50656,"254fa9b23c4282733076749572daec501d70f3f876ddd9b28fc59959972a3198",88536}},
    {53,{"SPEECH/UI/KRASEL02.VAG",31536,"355cda116f463c4a9a91ca7f27786e495bcb075bfc828882555d51da3b0c0cd6",55076}},
    // Rider18: Sebulba2, original UIACCEPT Event37..38.
    {37,{"SPEECH/UI/SEBSEL01.VAG",30320,"8d2473facab3023d2f4fb98512d8ebb24ed3d34b578a52a153828e8bb761df34",52948}},
    {38,{"SPEECH/UI/SEBSEL02.VAG",79152,"583965165f87ced323bcb73d001d5754d4449fa476a3c379a886b7fe65c35773",138404}},
    // Rider17: Scorch Zanales, original UIACCEPT Event35..36.
    {35,{"SPEECH/UI/SCOSEL01.VAG",31888,"80ca236eff9b445c4954211b9219d1170c4f1f0784dd2e55a1277ce96063c171",55692}},
    {36,{"SPEECH/UI/SCOSEL02.VAG",53216,"1f4e7f5f9417af8f2b85b30b563bbcda109009a9369386feb3c76c347caaf8c0",93016}},
    // Rider24: Tzidik Wrantojo, original UIACCEPT Event54..55.
    {54,{"SPEECH/UI/TZISEL01.VAG",32144,"5e270d5f9fae2fcc2c1b4df59cae9d6507d89f5feb2e4c04d442bd4404d055a2",56140}},
    {55,{"SPEECH/UI/TZISEL02.VAG",33664,"6080e2e94af66174aa2f3f053d5e9d9b7e096a17c9136434434af35f1e4f4685",58800}},
}};
std::size_t riderConfirmClipIndex(std::uint32_t eventId) noexcept {
    for(std::size_t i=0;i<riderConfirmOriginals.size();++i)
        if(riderConfirmOriginals[i].eventId==eventId)return i;
    return riderConfirmOriginals.size();
}
bool sameClip(const AudioClip50HD& a,const AudioClip50HD& b) noexcept {
    return a.samples==b.samples&&a.frames==b.frames&&a.sampleRate==b.sampleRate&&
        a.channels==b.channels&&a.loopStart==b.loopStart&&a.loopEnd==b.loopEnd;
}
bool preparedClipMatches(const AudioClip50HD& clip,const assets::AudioPcm50HC& pcm,
    const OriginalFile& original,std::uint8_t channels) noexcept {
    // Retained authentication/PCM ownership facts only; no load or decode.
    return AudioMixer50HD::validClip(clip)&&pcm.sourceSha256==original.sha&&
        pcm.frames==original.frames&&pcm.sampleRate==22050&&pcm.channels==channels&&
        !pcm.hasEmbeddedLoop&&!pcm.loopStart&&!pcm.loopEnd&&
        pcm.samples.size()==static_cast<std::size_t>(original.frames)*channels&&
        clip.samples==pcm.samples.data()&&clip.frames==pcm.frames&&clip.sampleRate==pcm.sampleRate&&
        clip.channels==pcm.channels&&!clip.loopStart&&!clip.loopEnd;
}
bool fixedVoiceMatches(const AudioVoiceObservation50HD& observed,const AudioClip50HD& clip,
    std::uint64_t playbackRevision,float gain,bool loop) noexcept {
    const auto& voice=observed.voice;
    return playbackRevision&&observed.playbackRevision==playbackRevision&&sameClip(voice.clip,clip)&&
        voice.gain==gain&&voice.pitch==1.f&&voice.pan==0.f&&voice.loop==loop&&
        observed.originalLeft==1.f&&observed.originalRight==1.f&&
        std::isfinite(voice.sourceFrame)&&voice.sourceFrame>=0.0&&
        voice.sourceFrame<static_cast<double>(clip.frames);
}
Bytes authenticatedRead(const std::filesystem::path& root, const char* relative,
    std::size_t expectedSize, const char* expectedHash) {
    need(!std::filesystem::is_symlink(root) && std::filesystem::is_directory(root), "Main audio root is not a real directory");
    auto path = root;
    for (const auto& part : std::filesystem::path(relative)) {
        need(part != "." && part != "..", "Unsafe Main audio relative path");
        path /= part;
        need(!std::filesystem::is_symlink(path), "Main audio symlink refused");
    }
    need(std::filesystem::is_regular_file(path), "Original Main audio file missing");
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    need(file.is_open() && file.tellg() == static_cast<std::streamoff>(expectedSize), "Main audio original extent differs");
    Bytes data(expectedSize); file.seekg(0);
    if (!data.empty()) file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
    need(file.good() && file.peek() == std::char_traits<char>::eof(), "Main audio original read failed");
    need(sha256(data) == expectedHash, "Main audio original SHA256 differs");
    return data;
}
std::uint32_t be32(const std::uint8_t* p) noexcept {
    return (std::uint32_t(p[0]) << 24) | (std::uint32_t(p[1]) << 16) | (std::uint32_t(p[2]) << 8) | p[3];
}
assets::AudioPcm50HC decodeMenuStereo(const Bytes& data,const OriginalFile& original,std::size_t payload) {
    constexpr std::size_t prefix = 0x7e0;
    const auto frames=original.frames;
    need(data.size() == prefix + 2 * payload && be32(data.data()+12) == payload && be32(data.data()+16) == 22050,
        "Main stereo source framing differs");
    for (std::size_t i=0; i<prefix; ++i)
        if (i<12 || i>=20) need(data[i]==0, "Main stereo source prefix differs");
    const auto blocks = payload / 16;
    for (std::size_t i=0; i<blocks; ++i) {
        const auto at=prefix+i*32;
        const auto flag=static_cast<std::uint8_t>(i+2==blocks ? 1 : 0);
        need(data[at+1]==flag && data[at+17]==flag, "Main stereo channel flags differ");
    }
    for (std::size_t c=0; c<2; ++c) {
        const auto at=data.size()-32+c*16;
        need(data[at]==7 && data[at+1]==0, "Main stereo pad differs");
        for (std::size_t n=2; n<16; ++n) need(data[at+n]==0x77, "Main stereo pad data differs");
    }
    // Deinterleave original compressed channel bytes into the existing bounded
    // zero-header mono decoder. No fixture import or replacement PCM is used.
    std::array<assets::AudioPcm50HC,2> channels;
    Bytes mono(48+payload,0); std::copy(data.begin(),data.begin()+48,mono.begin());
    for (std::size_t c=0; c<2; ++c) {
        for (std::size_t i=0; i<blocks; ++i)
            std::memcpy(mono.data()+48+i*16,data.data()+prefix+i*32+c*16,16);
        std::string error;
        need(assets::decodeVagMono50HC(mono.data(),mono.size(),channels[c],error), "Original Main stereo channel decode rejected");
        need(channels[c].channels==1 && channels[c].sampleRate==22050 && channels[c].frames==frames && !channels[c].hasEmbeddedLoop,
            "Original Main decoded channel metadata differs");
    }
    assets::AudioPcm50HC result;
    result.channels=2; result.sampleRate=22050; result.frames=frames;
    result.sourceSha256=original.sha; result.samples.resize(frames*2);
    for (std::size_t n=0; n<frames; ++n) {
        result.samples[n*2]=channels[0].samples[n]; result.samples[n*2+1]=channels[1].samples[n];
    }
    return result;
}
}

bool OriginalUiMainAudio50KG::prepare(const std::filesystem::path& audioRoot,
    AudioMixer50HD& mixer, std::string& error) noexcept {
    if (attempted_) { error="Main audio needs a fresh stable owner"; return false; }
    attempted_=true;
    try {
        need(mixer.count()>=requiredVoices && mixer.sampleRate()!=0 && !mixer.requiresVoiceGroupGains50IC(),
            "Main audio requires a dedicated fresh frontend mixer");
        for (std::size_t i=0; i<mixer.count(); ++i)
            need(!mixer.active(i) && !mixer.hasVoiceGroupGain50IC(i), "Frontend mixer is already owned");
        authenticatedRead(audioRoot,"UIEVENTS.INI",790,"ed897d482c72b158f209c20baa6c4e8270a10cdbb9031315cd143a8666557115");
        authenticatedRead(audioRoot,"UI_MUSIC.JUK",1517,"4b814f8e4fec2fbcfa25807d5e147ecc164dbb0b0cb9ce5d064e2f3e3c89de98");
        authenticatedRead(audioRoot,"UI_MUSIC.JUB",3623,"f97a5cbceec8056dfc9b277501087aaa54d86a0ccdc0c71cfde73522a048c661");
        std::array<assets::AudioPcm50HC,7> next;
        for (std::size_t i=0; i<originals.size(); ++i) {
            const auto& def=originals[i]; const auto bytes=authenticatedRead(audioRoot,def.path,def.bytes,def.sha);
            if (i==0) next[i]=decodeMenuStereo(bytes,def,219504);
            else {
                std::string decodeError;
                need(assets::decodeVagMono50HC(bytes.data(),bytes.size(),next[i],decodeError), "Original Main effect decode rejected");
            }
            need(next[i].frames==def.frames && next[i].sampleRate==22050 && next[i].channels==(i==0?2:1) &&
                !next[i].hasEmbeddedLoop, "Original Main PCM metadata differs");
        }
        pcm_=std::move(next);
        for (std::size_t i=0; i<clips_.size(); ++i) {
            const auto& p=pcm_[i]; clips_[i]={p.samples.data(),p.frames,p.sampleRate,p.channels,0,0};
            need(AudioMixer50HD::validClip(clips_[i]), "Original Main PCM rejected by native mixer");
        }
        mixer_=&mixer; mixerEpoch_=mixer.groupEpoch50IC(); mixerCount_=mixer.count();
        mixerRate_=mixer.sampleRate();preparedSoundMode_=mixer.originalSoundMode50KG();
        noteControlChange();
        prepared_=true; error.clear(); return true;
    } catch (const std::exception& e) { error=e.what(); }
    catch (...) { error="Main audio cold preparation failed"; }
    return false;
}
bool OriginalUiMainAudio50KG::prepareRiderMenu(const std::filesystem::path& audioRoot,
    std::string& error) noexcept {
    if(riderAttempted_){error="Rider music needs a fresh cold preparation";return false;}
    riderAttempted_=true;
    try {
        need(current()&&!paused_&&!ownsMusic_&&!mixer_->mixedFrames(),
            "Rider music requires the prepared cold frontend transport");
        for(std::size_t i=0;i<mixer_->count();++i)
            need(!mixer_->active(i),"Rider music preparation cannot alter live playback");
        // The optional AUDIO root must carry the exact same original menu
        // authoring. No parsed index, filename guess or alternative sample.
        authenticatedRead(audioRoot,"UI_MUSIC.JUK",1517,"4b814f8e4fec2fbcfa25807d5e147ecc164dbb0b0cb9ce5d064e2f3e3c89de98");
        authenticatedRead(audioRoot,"UI_MUSIC.JUB",3623,"f97a5cbceec8056dfc9b277501087aaa54d86a0ccdc0c71cfde73522a048c661");
        const auto bytes=authenticatedRead(audioRoot,riderOriginal.path,riderOriginal.bytes,riderOriginal.sha);
        auto pcm=decodeMenuStereo(bytes,riderOriginal,353744);
        need(pcm.frames==riderOriginal.frames&&pcm.channels==2&&pcm.sampleRate==22050&&!pcm.hasEmbeddedLoop,
            "Original Rider music PCM metadata differs");
        const AudioClip50HD clip{pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,0,0};
        need(AudioMixer50HD::validClip(clip),"Original Rider music rejected by native mixer");
        riderPcm_=std::move(pcm);
        riderClip_={riderPcm_.samples.data(),riderPcm_.frames,riderPcm_.sampleRate,riderPcm_.channels,0,0};
        riderPrepared_=true;noteControlChange();error.clear();return true;
    }catch(const std::exception& e){error=e.what();}
    catch(...){error="Rider music cold preparation failed";}
    return false;
}
bool OriginalUiMainAudio50KG::current() const noexcept {
    return prepared_ && mixer_ && mixer_->groupEpoch50IC()==mixerEpoch_ && mixer_->count()==mixerCount_ &&
        !mixer_->requiresVoiceGroupGains50IC();
}
bool OriginalUiMainAudio50KG::prepareCourseMenu(const std::filesystem::path& audioRoot,
    std::string& error) noexcept {
    if(courseAttempted_){error="Course audio needs a fresh cold preparation";return false;}
    courseAttempted_=true;
    try {
        need(riderMenuReady()&&!paused_&&!ownsMusic_&&!mixer_->mixedFrames()&&
            mixer_->count()==courseRequiredVoices,"Course audio requires the cold extended frontend transport");
        for(std::size_t i=0;i<mixer_->count();++i)
            need(!mixer_->active(i),"Course audio preparation cannot alter live playback");
        authenticatedRead(audioRoot,"UI_MUSIC.JUK",1517,"4b814f8e4fec2fbcfa25807d5e147ecc164dbb0b0cb9ce5d064e2f3e3c89de98");
        authenticatedRead(audioRoot,"UI_MUSIC.JUB",3623,"f97a5cbceec8056dfc9b277501087aaa54d86a0ccdc0c71cfde73522a048c661");
        authenticatedRead(audioRoot,"UIACCEPT.INI",4411,"245b0bd530e6797637ac72bbf2de8f101f3b1da280e00e8fc24e4b4930d79f3b");
        auto song=decodeMenuStereo(authenticatedRead(audioRoot,courseOriginal.path,courseOriginal.bytes,courseOriginal.sha),
            courseOriginal,223232);
        std::array<assets::AudioPcm50HC,riderConfirmClipCount> speech;
        for(std::size_t i=0;i<speech.size();++i){
            const auto& original=riderConfirmOriginals[i].file;
            const auto bytes=authenticatedRead(audioRoot,original.path,original.bytes,original.sha);
            std::string decodeError;
            need(assets::decodeVagMono50HC(bytes.data(),bytes.size(),speech[i],decodeError),
                "Original Rider Confirm speech decode rejected");
            const auto& pcm=speech[i];
            const AudioClip50HD clip{pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,0,0};
            need(preparedClipMatches(clip,pcm,original,1),"Original Rider Confirm PCM metadata differs");
        }
        coursePcm_=std::move(song);
        courseClip_={coursePcm_.samples.data(),coursePcm_.frames,coursePcm_.sampleRate,coursePcm_.channels,0,0};
        need(preparedClipMatches(courseClip_,coursePcm_,courseOriginal,2),"Original Course Song11 PCM metadata differs");
        riderConfirmPcm_=std::move(speech);
        for(std::size_t i=0;i<speech.size();++i){
            const auto& pcm=riderConfirmPcm_[i];
            riderConfirmClips_[i]={pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,0,0};
        }
        coursePrepared_=true;noteControlChange();error.clear();return true;
    }catch(const std::exception& e){error=e.what();}
    catch(...){error="Course audio cold preparation failed";}
    return false;
}
bool OriginalUiMainAudio50KG::canRecordControlChange(std::uint64_t count) const noexcept {
    return !controlRevisionExhausted_&&count<=std::numeric_limits<std::uint64_t>::max()-controlRevision_;
}
void OriginalUiMainAudio50KG::noteControlChange() noexcept {
    if(canRecordControlChange())++controlRevision_;
    else controlRevisionExhausted_=true;
}
bool OriginalUiMainAudio50KG::ready() const noexcept { return current(); }
bool OriginalUiMainAudio50KG::riderMenuReady() const noexcept {
    return current()&&riderPrepared_&&AudioMixer50HD::validClip(riderClip_);
}
bool OriginalUiMainAudio50KG::courseMenuReady() const noexcept {
    return riderMenuReady()&&coursePrepared_&&mixerCount_==courseRequiredVoices&&
        preparedClipMatches(courseClip_,coursePcm_,courseOriginal,2);
}
bool OriginalUiMainAudio50KG::canStartCourseLoop11() const noexcept {
    return courseMenuReady()&&!paused_&&canRecordControlChange()&&mixer_->canRecordControlChanges(1)&&
        (!mixer_->active(musicVoice)||(ownsMusic_&&mixer_->playbackRevision(musicVoice)==musicPlaybackRevision_))&&
        canPlayEffectClip(1); // Same real centered routing owner, no effect command.
}
bool OriginalUiMainAudio50KG::startCourseLoop11() noexcept {
    return canStartCourseLoop11()&&startMusicLoop(courseClip_,11);
}
bool OriginalUiMainAudio50KG::canFadeCourseMusic() const noexcept {
    return courseMenuReady()&&!paused_&&canRecordControlChange()&&
        ((!ownsMusic_&&!mixer_->active(musicVoice))||
         (ownsMusic_&&musicSong_==11&&mixer_->active(musicVoice)&&
          mixer_->playbackRevision(musicVoice)==musicPlaybackRevision_));
}
bool OriginalUiMainAudio50KG::fadeCourseMusic() noexcept {
    return canFadeCourseMusic()&&fadeMain(1.f);
}
bool OriginalUiMainAudio50KG::canPlayCourseEffect(std::uint32_t id) const noexcept {
    const auto clip=id==1?1u:id==3?2u:id==4?3u:id==5?5u:0u;
    return courseMenuReady()&&canRecordControlChange()&&mixer_->canRecordControlChanges(2)&&canPlayEffectClip(clip);
}
bool OriginalUiMainAudio50KG::playCourseEffect(std::uint32_t id) noexcept {
    if(!canPlayCourseEffect(id))return false;
    return playEffectClip(id==1?1u:id==3?2u:id==4?3u:5u);
}
bool OriginalUiMainAudio50KG::canPlayRiderConfirm() const noexcept {
    if(!courseMenuReady()||paused_||ownsRiderConfirm_||mixer_->active(riderConfirmVoice)||
       !canRecordControlChange(2)||!mixer_->canRecordControlChanges(3)||!canPlayEffectClip(1))return false;
    for(std::size_t i=0;i<riderConfirmClips_.size();++i)
        if(!preparedClipMatches(riderConfirmClips_[i],riderConfirmPcm_[i],riderConfirmOriginals[i].file,1))return false;
    return true;
}
bool OriginalUiMainAudio50KG::playRiderConfirmVoice(std::uint32_t id,RiderConfirmPlayback& out) noexcept {
    const auto clip=riderConfirmClipIndex(id);
    if(out.issued()||clip==riderConfirmOriginals.size()||!canPlayRiderConfirm())return false;
    // Source1EA780 uses flags0. Bounded Confirm has no earlier utterance:
    // never stop/restart a live speech voice or consume a replacement choice.
    if(!mixer_->play(riderConfirmVoice,riderConfirmClips_[clip],1.f,1.f,false,0.f))return false;
    riderConfirmPlaybackRevision_=mixer_->playbackRevision(riderConfirmVoice);ownsRiderConfirm_=true;
    out.transport_=this;out.revision_=riderConfirmPlaybackRevision_;noteControlChange();return true;
}
bool OriginalUiMainAudio50KG::playRiderConfirmSelection1() noexcept {
    return courseMenuReady()&&canRecordControlChange()&&mixer_->canRecordControlChanges(2)&&playEffectClip(1);
}
bool OriginalUiMainAudio50KG::releaseRiderConfirmVoice(RiderConfirmPlayback& playback) noexcept {
    if(!playback.issued())return true;
    if(!current()||playback.transport_!=this||!ownsRiderConfirm_||
       playback.revision_!=riderConfirmPlaybackRevision_||
       mixer_->playbackRevision(riderConfirmVoice)!=playback.revision_||
       !canRecordControlChange()||!mixer_->canRecordControlChanges(mixer_->active(riderConfirmVoice)?1u:0u))return false;
    mixer_->stop(riderConfirmVoice);ownsRiderConfirm_=false;playback={};noteControlChange();return true;
}
const OriginalSoundMode50KG* OriginalUiMainAudio50KG::soundModeOwner50KG() const noexcept {
    return current()?mixer_->originalSoundMode50KG():nullptr;
}
bool OriginalUiMainAudio50KG::canApplySoundMode18874050KG(std::uint32_t mode) const noexcept {
    const auto* owner=soundModeOwner50KG();
    return !paused_&&owner&&canRecordControlChange()&&owner->canApply188740(*mixer_,mode);
}
bool OriginalUiMainAudio50KG::canAcceptGameOptionsAudio50KG(std::uint32_t mode) const noexcept {
    //188740 records one transport change; SELECT records one more and can
    // stop then restart its existing owned voice (two mixer changes).
    return canApplySoundMode18874050KG(mode)&&canRecordControlChange(2)&&
        mixer_->canRecordControlChanges(2)&&canPlayGameOptionsEffect(1);
}
bool OriginalUiMainAudio50KG::applySoundMode18874050KG(std::uint32_t mode) noexcept {
    if(!canApplySoundMode18874050KG(mode)||!mixer_->originalSoundMode50KG()->apply188740(*mixer_,mode))return false;
    noteControlChange();return true;
}
bool OriginalUiMainAudio50KG::startMainLoop9() noexcept {
    return startMusicLoop(clips_[0],9);
}
bool OriginalUiMainAudio50KG::startRiderLoop7() noexcept {
    return riderMenuReady()&&startMusicLoop(riderClip_,7);
}
bool OriginalUiMainAudio50KG::startMusicLoop(const AudioClip50HD& clip,std::uint32_t song) noexcept {
    if (!current() || paused_) return false;
    if (!ownsMusic_ && mixer_->active(musicVoice)) return false;
    //234900(restart1) replaces the outgoing music, including any pending
    //234440 retirement, before beginning the selected original clip at0.
    if (!mixer_->play(musicVoice,clip,0.4f,1.f,true,0.f)) return false;
    musicPlaybackRevision_=mixer_->playbackRevision(musicVoice);noteControlChange();
    ownsMusic_=true; fading_=false; musicSong_=song;
    musicGain_=0.4f; fadeElapsed_=fadeDuration_=fadeRetireAt_=0.f;
    return true;
}
bool OriginalUiMainAudio50KG::fadeRiderMusic() noexcept {
    if(!riderMenuReady()||(ownsMusic_&&musicSong_!=7))return false;
    return fadeMain(1.f);
}
bool OriginalUiMainAudio50KG::canPlayRiderBack5() const noexcept {
    return riderMenuReady()&&canPlayEffectClip(5);
}
bool OriginalUiMainAudio50KG::playRiderBack5() noexcept {
    return canPlayRiderBack5()&&playEffectClip(5);
}
bool OriginalUiMainAudio50KG::inspectSteadyRiderTransport(SteadyTransportObservation& out) const noexcept {
    out={};out.transport=this;out.mixer=mixer_;out.controlRevision=controlRevision_;
    out.paused=paused_;out.ownsMusic=ownsMusic_;out.fading=fading_;
    out.musicSong=musicSong_;out.musicGain=musicGain_;
    auto reject=[&](SteadyRiderError error,const char* detail,std::size_t voice=courseRequiredVoices) noexcept {
        out.error=error;out.detail=detail;out.rejectedVoice=voice;return false;
    };
    if(!prepared_||!riderPrepared_)
        return reject(SteadyRiderError::preparationUnavailable,"Original Rider PCM preparation unavailable");
    if(!mixer_)return reject(SteadyRiderError::mixerUnavailable,"Rider mixer unavailable");
    out.mixerEpoch=mixer_->groupEpoch50IC();out.voiceCount=mixer_->count();out.outputRate=mixer_->sampleRate();
    out.mixerControlRevision=mixer_->controlRevision();out.sourceMixedFrames=mixer_->mixedFrames();
    out.masterGain=mixer_->masterGain();out.soundMode=mixer_->originalSoundMode50KG();
    if(out.mixerEpoch!=mixerEpoch_)
        return reject(SteadyRiderError::mixerEpochChanged,"Rider mixer initialization changed");
    if((out.voiceCount!=requiredVoices&&out.voiceCount!=courseRequiredVoices)||
       out.voiceCount!=mixerCount_||out.outputRate!=mixerRate_||!out.outputRate)
        return reject(SteadyRiderError::mixerShapeChanged,"Rider requires its exact dedicated frontend mixer/rate");
    if(controlRevisionExhausted_||!controlRevision_||!mixer_->canRecordControlChanges())
        return reject(SteadyRiderError::revisionUnavailable,"Rider control identity exhausted or unavailable");
    if(paused_)return reject(SteadyRiderError::paused,"Rider transport paused");
    if(!out.soundMode||!out.soundMode->ready())
        return reject(SteadyRiderError::soundModeUnavailable,"Rider requires the installed application sound-mode owner");
    if(out.soundMode!=preparedSoundMode_)
        return reject(SteadyRiderError::soundModeOwnerChanged,"Rider prepared sound-mode owner changed");
    out.soundModeRevision=out.soundMode->revision();
    out.profileSoundMode=static_cast<std::uint32_t>(out.soundMode->fields().profile1101C);
    game::OriginalSoundRouting50KG routing;
    if(!game::projectOriginalSoundRouting26CBA050KG(out.soundMode->fields(),0,100,routing)||
        routing.phaseSelector!=0||routing.positional||routing.leftPercent!=100||routing.rightPercent!=100)
        return reject(SteadyRiderError::unsupportedRouting,"Rider requires supported source centered routing");
    if(out.masterGain!=1.f||mixer_->requiresVoiceGroupGains50IC())
        return reject(SteadyRiderError::unsupportedGainLayer,"Rider master/group gain layer differs");
    for(std::size_t voice=0;voice<requiredVoices;++voice){
        auto& observed=voice==musicVoice?out.music:out.effects[voice-firstEffectVoice];
        if(!mixer_->inspectVoice(voice,observed))
            return reject(SteadyRiderError::revisionUnavailable,"Rider live voice inspection unavailable",voice);
        if(observed.groupKnown||observed.groupGain!=1.f)
            return reject(SteadyRiderError::unsupportedGainLayer,"Rider foreign per-voice group gain layer",voice);
    }
    if(out.voiceCount==courseRequiredVoices){
        AudioVoiceObservation50HD speech;
        if(!mixer_->inspectVoice(riderConfirmVoice,speech)||speech.groupKnown||speech.groupGain!=1.f||
           ownsRiderConfirm_||speech.voice.active)
            return reject(SteadyRiderError::foreignEffect,"Rider Confirm speech cannot certify steady navigation",riderConfirmVoice);
    }
    if(!ownsMusic_||musicSong_!=7||!out.music.voice.active)
        return reject(SteadyRiderError::wrongMusic,"Rider requires owned active Song7",musicVoice);
    if(fading_||musicGain_!=0.4f||fadeElapsed_!=0.f||fadeDuration_!=0.f||fadeRetireAt_!=0.f)
        return reject(SteadyRiderError::nonsteadyEnvelope,"Rider Song7 envelope is not the settled start state",musicVoice);
    if(!preparedClipMatches(riderClip_,riderPcm_,riderOriginal,2))
        return reject(SteadyRiderError::invalidMusicClip,"Rider authenticated Song7 PCM identity/metadata differs",musicVoice);
    if(!fixedVoiceMatches(out.music,riderClip_,musicPlaybackRevision_,0.4f,true))
        return reject(SteadyRiderError::musicVoiceChanged,"Rider Song7 live playback identity/controls/routing differs",musicVoice);
    for(std::size_t i=0;i<effectVoiceCount;++i){
        const auto& observed=out.effects[i];
        // The actual mixer ends one-shots. Bookkeeping may await a real page
        // update; inspecting or servicing PCM must not clear/restart them.
        if(!observed.voice.active)continue;
        const auto voice=firstEffectVoice+i,clip=i+1;
        if(!ownsEffect_[i]||observed.playbackRevision!=effectPlaybackRevisions_[i])
            return reject(SteadyRiderError::foreignEffect,"Rider active effect is not this transport's playback",voice);
        if(!preparedClipMatches(clips_[clip],pcm_[clip],originals[clip],1))
            return reject(SteadyRiderError::invalidEffectClip,"Rider authenticated effect PCM identity/metadata differs",voice);
        if(!fixedVoiceMatches(observed,clips_[clip],effectPlaybackRevisions_[i],1.f,false))
            return reject(SteadyRiderError::effectVoiceChanged,"Rider live one-shot controls/routing differs",voice);
    }
    out.error=SteadyRiderError::none;out.detail="Owned steady Rider Song7 and bounded effects observed";return true;
}
bool OriginalUiMainAudio50KG::canPlayRiderAdvance2() const noexcept {
    SteadyTransportObservation observed;
    return inspectSteadyRiderTransport(observed)&&canRecordControlChange()&&
        mixer_->canRecordControlChanges(2)&&
        preparedClipMatches(clips_[6],pcm_[6],originals[6],1)&&canPlayEffectClip(6);
}
bool OriginalUiMainAudio50KG::playRiderAdvance2(RiderAdvanceCommandReceipt& receipt) noexcept {
    receipt={};
    if(!canPlayRiderAdvance2())return false;
    RiderAdvanceCommandObservation observed;
    observed.transport=this;observed.mixer=mixer_;observed.soundMode=mixer_->originalSoundMode50KG();
    observed.controlRevisionBefore=controlRevision_;observed.mixerControlRevisionBefore=mixer_->controlRevision();
    // Exactly the existing authenticated ADVANCE.VAG stop-before-play path.
    // The preflight reserves both mixer control identities before any stop.
    if(!playEffectClip(6))return false;
    observed.controlRevisionAfter=controlRevision_;observed.mixerControlRevisionAfter=mixer_->controlRevision();
    observed.mixerEpoch=mixerEpoch_;observed.soundModeRevision=observed.soundMode->revision();
    observed.sourceMixedFrames=mixer_->mixedFrames();observed.eventId=2;observed.voiceSlot=6;
    // Guaranteed by the capacity preflight and exclusive main-thread owner.
    // A violation cannot be rolled back or retried as an unplayed command.
    if(!mixer_->inspectVoice(6,observed.voice))return false;
    receipt.observation_=observed;receipt.issued_=true;return true;
}
bool OriginalUiMainAudio50KG::fadeMain(float seconds) noexcept {
    if (!current() || paused_ || (seconds!=0.f && seconds!=1.f)) return false;
    // Source234480: inactive stream(+5C==-1) returns without effects; Main
    // B4 ignores that source result. Native true means command handled, not
    // a fabricated source return1. A foreign active voice is still rejected.
    if (!ownsMusic_ && !mixer_->active(musicVoice)) return true;
    if (!ownsMusic_ || !mixer_->active(musicVoice)) return false;
    if (seconds==0.f) {
        mixer_->stop(musicVoice); ownsMusic_=false; fading_=false; musicSong_=0; musicGain_=0.f;
        noteControlChange();return true;
    }
    fadeStartGain_=musicGain_; fadeElapsed_=0.f; fadeDuration_=seconds;
    volatile float retirement=seconds+0.35f; fadeRetireAt_=retirement;
    fading_=true;noteControlChange();return true;
}
bool OriginalUiMainAudio50KG::playMenuEffect(std::uint32_t originalId) noexcept {
    if (!current() || paused_) return false;
    const std::size_t clip = originalId==1?1 : originalId==3?2 : originalId==4?3 : originalId==7?4 : clips_.size();
    return playEffectClip(clip);
}
bool OriginalUiMainAudio50KG::playOptionsEffect(std::uint32_t originalId) noexcept {
    const std::size_t clip=originalId==1?1:originalId==3?2:originalId==4?3:originalId==5?5:clips_.size();
    return playEffectClip(clip);
}
bool OriginalUiMainAudio50KG::canPlayGameOptionsEffect(std::uint32_t originalId) const noexcept {
    const std::size_t clip=originalId==1?1:originalId==2?6:originalId==3?2:originalId==4?3:
        originalId==5?5:originalId==7?4:clips_.size();return canPlayEffectClip(clip);
}
bool OriginalUiMainAudio50KG::playGameOptionsEffect(std::uint32_t originalId) noexcept {
    const std::size_t clip=originalId==1?1:originalId==2?6:originalId==3?2:originalId==4?3:
        originalId==5?5:originalId==7?4:clips_.size();return playEffectClip(clip);
}
bool OriginalUiMainAudio50KG::canPlayControlsEffect(std::uint32_t originalId) const noexcept {
    const std::size_t clip=originalId==1?1:originalId==2?6:originalId==3?2:
        originalId==4?3:originalId==5?5:clips_.size();return canPlayEffectClip(clip);
}
bool OriginalUiMainAudio50KG::playControlsEffect(std::uint32_t originalId) noexcept {
    const std::size_t clip=originalId==1?1:originalId==2?6:originalId==3?2:
        originalId==4?3:originalId==5?5:clips_.size();return playEffectClip(clip);
}
bool OriginalUiMainAudio50KG::canPlayEffectClip(std::size_t clip) const noexcept {
    if (!current() || paused_ || !clip || clip>=clips_.size()) return false;
    const auto voice=firstEffectVoice+clip-1;
    if (voice>=mixer_->count() || !AudioMixer50HD::validClip(clips_[clip]) ||
        (!ownsEffect_[clip-1] && mixer_->active(voice))) return false;
    if(const auto* mode=mixer_->originalSoundMode50KG()){
        game::OriginalSoundRouting50KG routing;
        if(!mode->ready()||!game::projectOriginalSoundRouting26CBA050KG(mode->fields(),0,100,routing)||
            routing.phaseSelector!=0)return false;
    }
    return true;
}
bool OriginalUiMainAudio50KG::playEffectClip(std::size_t clip) noexcept {
    if(!canPlayEffectClip(clip))return false;
    const auto voice=firstEffectVoice+clip-1;
    const auto revision=mixer_->controlRevision();
    // Source233458..233488 performs stop-before-play for flags1, not overlap
    // or suppression of a repeated event. AudioMixer::play restarts at frame0.
    if (ownsEffect_[clip-1]) mixer_->stop(voice);
    if (!mixer_->play(voice,clips_[clip],1.f,1.f,false,0.f)) {
        if(revision!=mixer_->controlRevision()||!mixer_->canRecordControlChanges())noteControlChange();
        return false;
    }
    ownsEffect_[clip-1]=true;effectPlaybackRevisions_[clip-1]=mixer_->playbackRevision(voice);
    noteControlChange();return true;
}
bool OriginalUiMainAudio50KG::update(float seconds) noexcept {
    if (!current() || !std::isfinite(seconds) || seconds<0.f || seconds>1.f) return false;
    if (paused_) return true;
    if (!ownsMusic_ && mixer_->active(musicVoice)) return false;
    for (std::size_t i=0; i<effectVoiceCount; ++i) {
        if (!ownsEffect_[i] && mixer_->active(firstEffectVoice+i)) return false;
        if (ownsEffect_[i] && !mixer_->active(firstEffectVoice+i)) ownsEffect_[i]=false;
    }
    if(mixerCount_==courseRequiredVoices&&mixer_->active(riderConfirmVoice)&&
       (!ownsRiderConfirm_||mixer_->playbackRevision(riderConfirmVoice)!=riderConfirmPlaybackRevision_))return false;
    if (ownsMusic_ && !mixer_->active(musicVoice)) { ownsMusic_=false; fading_=false; musicSong_=0; }
    if (!fading_) return true;
    const auto previousElapsed=fadeElapsed_,previousGain=musicGain_;
    const auto revision=mixer_->controlRevision();
    volatile float elapsed=fadeElapsed_+seconds; fadeElapsed_=elapsed;
    musicGain_=fadeStartGain_*std::max(0.f,1.f-fadeElapsed_/fadeDuration_);
    if (!mixer_->controls(musicVoice,musicGain_,1.f,0.f)) {
        if(previousElapsed!=fadeElapsed_||previousGain!=musicGain_)noteControlChange();
        return false;
    }
    if (fadeElapsed_>=fadeRetireAt_) {
        mixer_->stop(musicVoice); ownsMusic_=false; fading_=false; musicSong_=0; musicGain_=0.f;
    }
    if(previousElapsed!=fadeElapsed_||previousGain!=musicGain_||revision!=mixer_->controlRevision()||
        !mixer_->canRecordControlChanges())noteControlChange();
    return true;
}
bool OriginalUiMainAudio50KG::mixStereo16(std::int16_t* output, std::size_t frames) noexcept {
    if (!current() || frames>mixer_->sampleRate() || (!output && frames)) return false;
    if (paused_) {
        if (frames) std::fill(output,output+frames*2,std::int16_t(0));
        return true;
    }
    return mixer_->mixStereo16(output,frames);
}
bool OriginalUiMainAudio50KG::mainLoopActive() const noexcept {
    return activeMusicSong()==9;
}
bool OriginalUiMainAudio50KG::riderLoopActive() const noexcept {
    return activeMusicSong()==7;
}
std::uint32_t OriginalUiMainAudio50KG::activeMusicSong() const noexcept {
    return current()&&ownsMusic_&&mixer_->active(musicVoice)?musicSong_:0;
}
void OriginalUiMainAudio50KG::reset() noexcept {
    const bool changed=prepared_||riderPrepared_||coursePrepared_||mixer_||ownsMusic_||ownsRiderConfirm_||fading_||paused_||musicSong_;
    if (current()) {
        if (ownsMusic_) mixer_->stop(musicVoice);
        for (std::size_t i=0; i<effectVoiceCount; ++i)
            if (ownsEffect_[i]) mixer_->stop(firstEffectVoice+i);
        if(ownsRiderConfirm_&&mixer_->playbackRevision(riderConfirmVoice)==riderConfirmPlaybackRevision_)
            mixer_->stop(riderConfirmVoice);
    }
    prepared_=false; riderPrepared_=false; coursePrepared_=false; mixer_=nullptr;
    ownsMusic_=false; ownsRiderConfirm_=false; fading_=false; paused_=false; musicSong_=0;
    ownsEffect_.fill(false); musicGain_=0.f;
    if(changed)noteControlChange();
}
}
