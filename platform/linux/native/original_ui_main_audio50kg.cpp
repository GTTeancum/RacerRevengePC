#include "original_ui_main_audio50kg.h"
#include "original_ui_garage_watto_sound_group50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_audio_backing50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_sound_mode50kg.h"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <exception>

namespace rr::platform {
struct OriginalUiMainAudio50KG::QueuedGroupControl final {
    using Group=OriginalUiGarageWattoSoundGroup50KG;
    //A pending command owns native custody of the SAME real allocation and
    //grant. Source Entity reference00 remains1. The Main/group retention cycle
    //is deliberate while no genuine ordered apply/retirement path exists.
    NativeGroupControlKind kind=NativeGroupControlKind::watto;
    std::shared_ptr<Group> group;
    // An alias of Root's genuine shared Page, not a second Scene allocation
    // or a no-op deleter. It retains the complete Page/unique Scene lifetime.
    std::shared_ptr<OriginalUiGaragePodScene50KG> scene;
    std::shared_ptr<game::OriginalUiEntityScope50KG::GarageConstructionGrant> grant;
    std::weak_ptr<OriginalUiMainAudio50KG> main;
    const game::OriginalWreckComponent50LA* entity=nullptr;
    const AudioMixer50HD* mixer=nullptr;
    const OriginalSoundMode50KG* soundMode=nullptr;
    const OriginalApplicationAural50KG* wetOwner=nullptr;
    const NativeSourceReverb50KG* wetProcessor=nullptr;
    std::uint64_t sequence=0,generation=0,objectKey=0,epoch=0,acceptedControlRevision=0;
    std::size_t voiceCount=0;
    std::uint32_t outputRate=0,command=25;
    std::int32_t slot=-1;
    std::array<std::uint32_t,3> arguments{};
    bool routingApplied=false,retainedHistory=false;
    std::uint64_t routingRevision=0;
    std::uint64_t acceptedFrame=0,routingFrame=0;
    // Only the real Mixer routing consumer writes this phase. A captured DSP
    // lifetime/boundary is not routed playback admission. Retained custody
    // continues through actual source retirement.
};
namespace {
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
// Original UI_MUSIC Song6 WATTOSHP,265328 bytes/channel,464296 frames.
// Its authenticated JUB pitch1881 is represented by the Garage-only native
// renderer ratio. All pre-existing music calls retain their pitch1 policy.
constexpr OriginalFile garageOriginal{
    "MUSIC/UI/SCREENS/WATTOSHP.VAG",532672,
    "89aaf2b551342331a5f1f812b4e3eb2dc6703f8238b58ef8e7bb4bbbc8da07f1",464296};
constexpr OriginalFile courseOriginal{
    "MUSIC/UI/SCREENS/UI_TRKS.VAG",448480,
    "b00ea0f7d26f88970eaa87899b5e185b821d82a2bd01a07a33ccf14ded42b40d",390628};
// Authenticated UI_MUSIC.JUK/JUB Song12: 22050 Hz, 561232 compressed
// bytes per channel, duration44.542221, pitch1881. Original stereo extent is
// 0x7e0 + 2*561232; each channel's end block35075 yields982128 frames.
constexpr OriginalFile creditsOriginal{
    "MUSIC/UI/SCREENS/CREDITS.VAG",1124480,
    "87f6f10e1faf403b383d478fc7230061ed9f4d6400082b26a3a455aaa9fb60a2",982128};
// Authenticated UI_MUSIC.JUK/JUB Song5: 22050 Hz, 353744 compressed
// bytes per channel, duration28.074921, pitch1881. The original Gallery
// asset is not Song7 despite matching stereo extent and decoded frame count.
constexpr OriginalFile hallOfFameOriginal{
    "MUSIC/UI/GALLERY/GALLERY.VAG",709504,
    "de3e6947ec4e4dfe1cc35c599361342152d55ec6f3783675933e88745c82af3f",619024};
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
    return prepareImpl(audioRoot,mixer,error,false,{});
}
bool OriginalUiMainAudio50KG::prepareGarageFrontend(const std::filesystem::path& audioRoot,
    std::shared_ptr<const assets::OriginalStreamMetadata50KZ> metadata,AudioMixer50HD& mixer,std::string& error) noexcept {
    return prepareImpl(audioRoot,mixer,error,true,std::move(metadata));
}
bool OriginalUiMainAudio50KG::prepareGarageSceneFrontend(const std::filesystem::path& audioRoot,
    std::shared_ptr<const assets::OriginalStreamMetadata50KZ> metadata,AudioMixer50HD& mixer,std::string& error) noexcept {
    return prepareImpl(audioRoot,mixer,error,true,std::move(metadata),true);
}
bool OriginalUiMainAudio50KG::prepareGarageSceneColdTransport(const std::filesystem::path& root,
    std::shared_ptr<const assets::OriginalStreamMetadata50KZ> metadata,AudioMixer50HD& mixer,std::string& error) noexcept {
    return prepareImpl(root,mixer,error,true,std::move(metadata),true,true);
}
bool OriginalUiMainAudio50KG::bindRetainedGarageTransport(
    const std::shared_ptr<OriginalUiGarageAudioBacking50KG>& backing,std::string& error)noexcept {
    const auto fail=[&](const char* text)noexcept {try{error=text;}catch(...){error.clear();}return false;};
    if(std::this_thread::get_id()!=nativeControlThread_||!backing||
       garageSourceLifetime_!=GarageSourceLifetime::unbound||!prepared_||!garageColdPending_||
       garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||nativeGroupControlCount_||
       &backing->transport_!=this||mixer_!=&backing->mixer_||backing->failed_||
       backing->stage_!=OriginalUiGarageAudioBacking50KG::Stage::transportPrepared||
       !mixer_||mixer_->mixedFrames()||ownsMusic_||paused_||
       !garageDroidAudio_.sourceConstructionPrefixCurrent()||!garageAudio_.sourceGroupConstructionCurrent())
        return fail("Retained Garage transport requires this exact fresh Application backing");
    std::uint32_t pause=0;
    if(!backing->issuer_||!backing->issuerLifetime_.readPause11C(pause)||
       !backing->inputsCurrent(*backing->issuer_)||!ordinaryVoiceOwnershipCurrent()||
       !silentGarageVoiceRangesCurrent()||!canRecordControlChange())
        return fail("Retained Garage transport has no authenticated stable root/voice ownership");
    garagePersistentBacking_=backing;garageSourceLifetime_=GarageSourceLifetime::initialPrefix;
    noteControlChange();error.clear();return true;
}
bool OriginalUiMainAudio50KG::retainedGarageTransportCurrent()const noexcept {
    return !finalNativePcmRetired_&&retainedGarageStorageCurrent();
}
bool OriginalUiMainAudio50KG::retainedGarageStorageCurrent()const noexcept {
    if((nativeWetOwner_||nativeWetProcessor_||nativeWetEpoch_||nativeWetSetRawMasters_||nativeWetPlaySong_||nativeWetPlayEffect_||nativeWetPlayRider_||nativeWetPlayWatto_||nativeWetCanPublish_)&&!nativeWetInterfaceCurrent())return false;
    if(std::this_thread::get_id()!=nativeControlThread_||
       garageSourceLifetime_==GarageSourceLifetime::unbound||!prepared_||!mixer_||
       mixerCount_!=garageSceneRequiredVoices||mixer_->count()!=mixerCount_||
       mixer_->groupEpoch50IC()!=mixerEpoch_||mixer_->sampleRate()!=mixerRate_||
       !mixerRate_||mixer_->originalSoundMode50KG()!=preparedSoundMode_||
       !preparedSoundMode_||!preparedSoundMode_->ready()||mixer_->requiresVoiceGroupGains50IC()||
       mixer_->masterGain()!=1.f||!controlRevision_||!canRecordControlChange(0)||
       !mixer_->canRecordControlChanges())return false;
    const auto backing=garagePersistentBacking_.lock();std::uint32_t pause=0;
    // Lifetime BEFORE the raw Application read. The weak binding never makes
    // a root/Main ownership cycle or pins a retired Page/source manager.
    return backing&&backing->issuer_&&backing->issuerLifetime_.readPause11C(pause)&&
        backing->inputsCurrent(*backing->issuer_)&&!backing->failed_&&
        &backing->transport_==this&&mixer_==&backing->mixer_&&
        mixer_->voices_==backing->voices_.data()&&mixerEpoch_==backing->mixerEpoch_&&
        mixerRate_==backing->nativeOutputRate_&&preparedSoundMode_==backing->soundMode_.get()&&
        (backing->stage_==OriginalUiGarageAudioBacking50KG::Stage::transportPrepared||
         backing->stage_==OriginalUiGarageAudioBacking50KG::Stage::transportFinished);
}
bool OriginalUiMainAudio50KG::silentGarageVoiceRangesCurrent()const noexcept {
    if(!mixer_||mixer_->count()!=garageSceneRequiredVoices||mixer_->groupEpoch50IC()!=mixerEpoch_)return false;
    // Deliberately range-local: ordinary music, effects, Confirm and the
    // persistent mixer clock may already be live. Garage playback is NOT.
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot){
        AudioVoiceObservation50HD v;
        if(!mixer_->inspectVoice(slot,v)||v.playbackRevision||v.voice.active||
           v.voice.clip.samples||v.voice.clip.frames||v.voice.clip.sampleRate||v.voice.clip.channels||
           v.voice.clip.loopStart||v.voice.clip.loopEnd||v.voice.sourceFrame!=0.0||
           v.voice.gain!=1.f||v.voice.pitch!=1.f||v.voice.pan!=0.f||v.voice.loop||
           v.groupGain!=1.f||v.groupPaused||v.originalLeft!=1.f||v.originalRight!=1.f||
           (slot<OriginalUiGarageDroidAudio50GN::firstVoice&&!v.groupKnown))return false;
    }
    return true;
}
bool OriginalUiMainAudio50KG::ordinaryVoiceOwnershipCurrent()const noexcept {
    if(!prepared_||!mixer_||mixer_->count()!=garageSceneRequiredVoices||
       mixer_->groupEpoch50IC()!=mixerEpoch_||mixer_->sampleRate()!=mixerRate_||
       mixer_->originalSoundMode50KG()!=preparedSoundMode_||!preparedSoundMode_||
       !preparedSoundMode_->ready()||mixer_->masterGain()!=1.f||mixer_->requiresVoiceGroupGains50IC())return false;
    game::OriginalSoundRouting50KG routing;
    if(!game::projectOriginalSoundRouting26CBA050KG(preparedSoundMode_->fields(),0,100,routing)||
       routing.phaseSelector||routing.positional||routing.leftPercent!=100||routing.rightPercent!=100)return false;
    std::array<AudioVoiceObservation50HD,courseRequiredVoices> voices;
    for(std::size_t i=0;i<voices.size();++i){auto& v=voices[i];
        if(!mixer_->inspectVoice(i,v)||v.groupKnown||v.groupGain!=1.f||v.groupPaused||
           v.originalLeft!=1.f||v.originalRight!=1.f||!std::isfinite(v.voice.sourceFrame)||
           v.voice.sourceFrame<0.0)return false;
    }
    const auto pristine=[](const AudioVoiceObservation50HD& v)noexcept {
        return !v.playbackRevision&&!v.voice.active&&!v.voice.clip.samples&&!v.voice.clip.frames&&
            !v.voice.clip.sampleRate&&!v.voice.clip.channels&&!v.voice.clip.loopStart&&!v.voice.clip.loopEnd&&
            v.voice.sourceFrame==0.0&&v.voice.gain==1.f&&v.voice.pitch==1.f&&v.voice.pan==0.f&&!v.voice.loop;
    };
    const auto& music=voices[musicVoice];
    if(music.playbackRevision!=musicPlaybackRevision_)return false;
    if(!musicPlaybackRevision_){
        if(ownsMusic_||fading_||musicSong_||!pristine(music))return false;
    }else {
        if((ownsMusic_!=music.voice.active)||(!ownsMusic_&&(fading_||musicSong_))||
           !std::isfinite(musicGain_)||musicGain_<0.f||musicGain_>0.4f||
           !std::isfinite(music.voice.gain)||music.voice.gain<0.f||music.voice.gain>0.4f||
           (ownsMusic_&&music.voice.gain!=musicGain_)||music.voice.pan!=0.f||!music.voice.loop)return false;
        // A stopped voice still borrows its real clip. Authenticate that
        // retained history too; never erase it to certify a fresh voice.
        const auto matchMusic=[&](const AudioClip50HD& clip,const assets::AudioPcm50HC& pcm,
            const OriginalFile& original,bool prepared,std::uint32_t song,float pitch)noexcept {
            return prepared&&preparedClipMatches(clip,pcm,original,2)&&sameClip(music.voice.clip,clip)&&
                (!ownsMusic_||musicSong_==song)&&music.voice.pitch==pitch&&musicPitch_==pitch&&
                (!music.voice.active||music.voice.sourceFrame<static_cast<double>(clip.frames));
        };
        if(!matchMusic(clips_[0],pcm_[0],originals[0],true,9,1.f)&&
           !matchMusic(riderClip_,riderPcm_,riderOriginal,riderPrepared_,7,1.f)&&
           !matchMusic(courseClip_,coursePcm_,courseOriginal,coursePrepared_,11,1.f)&&
           !matchMusic(garageClip_,garagePcm_,garageOriginal,true,6,OriginalUiGarageWattoAudio50KG::nativeRendererPitch)&&
           !matchMusic(creditsClip_,creditsPcm_,creditsOriginal,creditsPrepared_,12,1.f)&&
           !matchMusic(hallOfFameClip_,hallOfFamePcm_,hallOfFameOriginal,hallOfFamePrepared_,5,1.f))return false;
        if(fading_&&(!std::isfinite(fadeElapsed_)||fadeElapsed_<0.f||!std::isfinite(fadeStartGain_)||
           fadeStartGain_<0.f||fadeStartGain_>0.4f||fadeDuration_!=1.f||fadeRetireAt_!=1.35f))return false;
    }
    for(std::size_t i=0;i<effectVoiceCount;++i){const auto& v=voices[firstEffectVoice+i];const auto clip=i+1;
        if(v.playbackRevision!=effectPlaybackRevisions_[i]||(v.voice.active&&!ownsEffect_[i]))return false;
        if(!v.playbackRevision){if(ownsEffect_[i]||!pristine(v))return false;continue;}
        if(!preparedClipMatches(clips_[clip],pcm_[clip],originals[clip],1)||!sameClip(v.voice.clip,clips_[clip])||
           v.voice.gain!=1.f||v.voice.pitch!=1.f||v.voice.pan!=0.f||v.voice.loop||
           (v.voice.active&&v.voice.sourceFrame>=static_cast<double>(clips_[clip].frames)))return false;
    }
    const auto& speech=voices[riderConfirmVoice];
    if(speech.playbackRevision!=riderConfirmPlaybackRevision_||(speech.voice.active&&!ownsRiderConfirm_))return false;
    if(!speech.playbackRevision)return !ownsRiderConfirm_&&pristine(speech);
    if(!coursePrepared_||speech.voice.gain!=1.f||speech.voice.pitch!=1.f||speech.voice.pan!=0.f||speech.voice.loop)return false;
    for(std::size_t i=0;i<riderConfirmClips_.size();++i)
        if(preparedClipMatches(riderConfirmClips_[i],riderConfirmPcm_[i],riderConfirmOriginals[i].file,1)&&
           sameClip(speech.voice.clip,riderConfirmClips_[i])&&
           (!speech.voice.active||speech.voice.sourceFrame<static_cast<double>(riderConfirmClips_[i].frames)))return true;
    return false;
}
bool OriginalUiMainAudio50KG::reconstructibleGarageRowsCurrent()const noexcept {
    // Source row identity is distinct from Application pause. A legitimate
    // pause may silence ordinary output without manufacturing source work.
    if(!garageDroidAudio_.current()||garageDroidAudio_.retired_||garageDroidAudio_.nextActor_||
       garageDroidAudio_.constructedSounds_||garageDroidAudio_.sourceSceneGroup_||garageDroidAudio_.sourceSceneGroupReleased_||
       !garageAudio_.current()||garageAudio_.sourceGroupAttempted_||garageAudio_.sourceGroupReleased_||
       garageAudio_.sourceGroup_||
       (garageAudio_.nextToken_&&!garageAudio_.retainedSourceRearm_)||
       garageDroidAudio_.paused_!=paused_||garageAudio_.paused_!=paused_)return false;
    for(const auto& sound:garageAudio_.sounds_)
        if(sound.source||sound.latest!=OriginalUiGarageWattoAudio50KG::instanceCount||sound.latestRevision||sound.latestToken!=-1)return false;
    bool retained=false;
    for(std::size_t i=0;i<OriginalUiGarageWattoAudio50KG::instanceCount;++i)
        if(garageAudio_.soundIds_[i]||garageAudio_.playbackRevisions_[i]){
            if(!garageAudio_.retainedSourceRearm_)return false;retained=true;
        }
    for(std::size_t i=0;i<OriginalUiGarageDroidAudio50GN::soundCount;++i)
        if(mixer_->playbackRevision(OriginalUiGarageDroidAudio50GN::firstVoice+i)){
            if(!garageDroidAudio_.retainedSceneRearmed_||!garageDroidAudio_.retainedStoppedVoiceCurrent(i))return false;
            retained=true;
        }
    return !retained||ownedStoppedGarageClipHistoryCurrent();
}
bool OriginalUiMainAudio50KG::terminalPartialSceneRowsCurrent()const noexcept {
    return garageSourceLifetime_==GarageSourceLifetime::partialSceneEntryRetired&&
        terminalUnresolvedSceneRowsCurrent();
}
bool OriginalUiMainAudio50KG::terminalLateBorrowRowsCurrent()const noexcept {
    return garageSourceLifetime_==GarageSourceLifetime::lateBorrowEntryRetired&&
        terminalUnresolvedSceneRowsCurrent();
}
bool OriginalUiMainAudio50KG::terminalLoadedPageRowsCurrent()const noexcept {
    return garageSourceLifetime_==GarageSourceLifetime::loadedPageEntryRetired&&
        terminalUnresolvedSceneRowsCurrent();
}
bool OriginalUiMainAudio50KG::terminalWattoBindingRowsCurrent()const noexcept {
    return garageSourceLifetime_==GarageSourceLifetime::wattoBindingEntryRetired&&
        terminalUnresolvedSceneRowsCurrent();
}
bool OriginalUiMainAudio50KG::terminalUnresolvedSceneRowsCurrent()const noexcept {
    // Shared row mechanics only. Each caller above retains its distinct actual
    // terminal commit state; neither is a fresh Scene constructor or rearm.
    if(!garageEntryGeneration_||!garageEntryEpoch_||!garageEntryCustody_.expired()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       paused_||nativeGroupControlCount_||!garageDroidAudio_.current()||
       !garageDroidAudio_.nativeUnresolvedEntryRetired_||
       !garageDroidAudio_.retiredSceneTransportCurrent()||
       !garageAudio_.sourceGroupCold()||garageAudio_.sourceGroup_||garageAudio_.sourceGroupReleased_||
       !garageAudio_.nativeSampleLeasesEmpty()||!garageDroidAudio_.nativeSampleLeasesEmpty()||
       !silentGarageVoiceRangesCurrent()||mixer_->nativeWattoRouting_.command||mixer_->nativeSceneRouting_.command)return false;
    for(const auto& command:nativeGroupControls_)if(command)return false;
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot)
        if(mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    return true;
}
bool OriginalUiMainAudio50KG::retainedGarageEntryCurrent()const noexcept {
    if(!retainedGarageTransportCurrent())return false;
    if(garageSourceLifetime_==GarageSourceLifetime::initialPrefix||garageSourceLifetime_==GarageSourceLifetime::retired){
        return garageColdPending_&&!garageColdFinishAttempted_&&!garagePrepared_&&!garageScenePrepared_&&
            !nativeGroupControlCount_&&reconstructibleGarageRowsCurrent()&&
            (garageSourceLifetime_==GarageSourceLifetime::initialPrefix?
                (!garageEntryGeneration_&&!garageEntryEpoch_&&garageEntryCustody_.expired()):
                (garageEntryGeneration_&&garageEntryEpoch_&&lastRearmedGarageGeneration_==garageEntryGeneration_));
    }
    const auto backing=garagePersistentBacking_.lock();const auto entry=garageEntryCustody_.lock();
    if(!backing||!entry||entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_))return false;
    std::uint64_t generation=0,epoch=0;
    const bool constructing=entry->nativeAudioConstructionIdentity(*backing->issuer_,*this,generation,epoch)&&
        generation==garageEntryGeneration_&&epoch==garageEntryEpoch_;
    // Normal Page/Actor teardown still uses the original cold source backend.
    // Its genuine retiring capsule must not invalidate unrelated ordinary
    // audio or be mistaken for a fresh construction grant.
    const bool retiring=entry->nativeAudioRetirementIdentity(*backing->issuer_,*this,
        garageEntryGeneration_,garageEntryEpoch_);
    // Only an already-admitted GUI81 native terminal cleanup may borrow
    // these same source accessors while failed_ stays true. The initial
    // readonly proof is NOT construction/readiness, and no proof survives
    // beyond the actual Source retirement cursor or into normal reentry.
    const bool failedObservationRetiring=entry->retiring_&&
        entry->normalRetirementPhase_!=OriginalUiGarageSession50KG::NormalRetirementPhase::notStarted&&
        entry->nativeAudioPublishedObservationFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,false);
    // The exact failed first Actor sample has already performed Aural ages
    // and possibly Watto source effects. Only its admitted physical teardown
    // may borrow these accessors; it is not the never-framed GUI81 identity.
    const bool failedActorSampleRetiring=entry->retiring_&&
        entry->normalRetirementPhase_!=OriginalUiGarageSession50KG::NormalRetirementPhase::notStarted&&
        entry->nativeAudioActorSampleFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,false);
    // A completed update may own real voices even when its unsealed Draw
    // fails. Borrow source accessors only after that exact Draw was released
    // and the distinct terminal source-retirement cursor was admitted.
    const bool failedUnsealedRenderRetiring=entry->retiring_&&
        entry->normalRetirementPhase_!=OriginalUiGarageSession50KG::NormalRetirementPhase::notStarted&&
        entry->nativeAudioUnsealedRenderFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,false);
    if(!constructing&&!retiring&&!failedObservationRetiring&&!failedActorSampleRetiring&&
       !failedUnsealedRenderRetiring)return false;
    if(garageSourceLifetime_==GarageSourceLifetime::constructing)
        return garageColdPending_&&!garageColdFinishAttempted_&&!garagePrepared_&&!garageScenePrepared_;
    return garageSourceLifetime_==GarageSourceLifetime::constructed&&!garageColdPending_&&
        garageColdFinishAttempted_&&garagePrepared_&&garageScenePrepared_;
}
bool OriginalUiMainAudio50KG::retainedGarageOrdinaryCurrent()const noexcept {
    // An incomplete/failed source entry does not own ordinary slots. Keep
    // their authenticated PCM/transport usable without certifying that entry
    // as constructible, ready or retired. Source accessors prove it separately.
    return !ordinaryOutputRetired_&&retainedGarageTransportCurrent()&&
        (silentGarageVoiceRangesCurrent()||liveGarageVoiceRangesCurrent())&&ordinaryVoiceOwnershipCurrent();
}
bool OriginalUiMainAudio50KG::canRetireOrdinaryFrontendOutput()const noexcept {
    return !ordinaryOutputRetired_&&retainedGarageOrdinaryCurrent()&&(reconstructibleGarageRowsCurrent()||terminalPartialSceneRowsCurrent()||terminalLateBorrowRowsCurrent()||terminalLoadedPageRowsCurrent()||terminalWattoBindingRowsCurrent())&&
        (garageSourceLifetime_==GarageSourceLifetime::initialPrefix||garageSourceLifetime_==GarageSourceLifetime::retired||
         garageSourceLifetime_==GarageSourceLifetime::emptyEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::sharedPageEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::partialSceneEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::lateBorrowEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::loadedPageEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::wattoBindingEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::publishedObservationEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::actorSampleEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::unsealedRenderEntryRetired)&&
        !nativeGroupControlCount_&&((garageDroidAudio_.nativeSampleLeasesEmpty()&&garageAudio_.nativeSampleLeasesEmpty())||ownedStoppedGarageClipHistoryCurrent())&&
        canRecordControlChange()&&mixer_->canRecordControlChanges(courseRequiredVoices);
}
bool OriginalUiMainAudio50KG::retiredOrdinaryFrontendOutputCurrent()const noexcept {
    if(finalNativePcmRetired_)return finalNativePcmRetirementCurrent();
    return ordinaryOutputRetired_&&retainedGarageTransportCurrent()&&(reconstructibleGarageRowsCurrent()||terminalPartialSceneRowsCurrent()||terminalLateBorrowRowsCurrent()||terminalLoadedPageRowsCurrent()||terminalWattoBindingRowsCurrent())&&
        (garageSourceLifetime_==GarageSourceLifetime::initialPrefix||garageSourceLifetime_==GarageSourceLifetime::retired||
         garageSourceLifetime_==GarageSourceLifetime::emptyEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::sharedPageEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::partialSceneEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::lateBorrowEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::loadedPageEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::wattoBindingEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::publishedObservationEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::actorSampleEntryRetired||
         garageSourceLifetime_==GarageSourceLifetime::unsealedRenderEntryRetired)&&
        (silentGarageVoiceRangesCurrent()||ownedStoppedGarageClipHistoryCurrent())&&ordinaryVoiceOwnershipCurrent()&&!nativeGroupControlCount_&&
        ((garageDroidAudio_.nativeSampleLeasesEmpty()&&garageAudio_.nativeSampleLeasesEmpty())||ownedStoppedGarageClipHistoryCurrent());
}
bool OriginalUiMainAudio50KG::retireOrdinaryFrontendOutputAfterDeviceClose()noexcept {
    if(!canRetireOrdinaryFrontendOutput())return false;
    // All eight real voices were authenticated by exact owned revisions and
    // PCM above. Stop only these ordinary slots after the device callback has
    // joined. Stop leaves clip, cursor and playback history intact; Main keeps
    // every PCM allocation until actual root reclamation after source release.
    for(std::size_t slot=0;slot<courseRequiredVoices;++slot)mixer_->stop(slot);
    ownsMusic_=false;ownsEffect_.fill(false);ownsRiderConfirm_=false;
    musicSong_=0;musicGain_=0.f;fading_=false;
    ordinaryOutputRetired_=true;noteControlChange();return true;
}
bool OriginalUiMainAudio50KG::rebindOrdinaryFrontendOutput(std::uint32_t applicationPause)noexcept {
    // A failed-entry receipt permits terminal cleanup only, never native
    // output reattachment or another Garage construction attempt.
    if(garageSourceLifetime_==GarageSourceLifetime::emptyEntryRetired||
       garageSourceLifetime_==GarageSourceLifetime::sharedPageEntryRetired||
       garageSourceLifetime_==GarageSourceLifetime::partialSceneEntryRetired||
       garageSourceLifetime_==GarageSourceLifetime::lateBorrowEntryRetired||
       garageSourceLifetime_==GarageSourceLifetime::loadedPageEntryRetired||
       garageSourceLifetime_==GarageSourceLifetime::wattoBindingEntryRetired||
       garageSourceLifetime_==GarageSourceLifetime::publishedObservationEntryRetired||
       garageSourceLifetime_==GarageSourceLifetime::actorSampleEntryRetired||
       garageSourceLifetime_==GarageSourceLifetime::unsealedRenderEntryRetired||
       finalNativePcmRetired_||!retiredOrdinaryFrontendOutputCurrent()||!canRecordControlChange(2))return false;
    // Root has authenticated the actual prior device-close receipt and the
    // fresh output borrow. This is native output reattachment, not a source
    // constructor replay: clips, revisions, source cursors and clock survive.
    ordinaryOutputRetired_=false;pause(applicationPause!=0);noteControlChange();return true;
}
bool OriginalUiMainAudio50KG::canRetireFinalNativePcm(
    const OriginalAudioSampleCache50KG& cache,FinalNativePcmRetirement& out)const noexcept {
    if(finalNativePcmRetired_||!retiredOrdinaryFrontendOutputCurrent()||
       !nativeWetInterfaceCurrent()||!garageEntryCustody_.expired()||
       (!reconstructibleGarageRowsCurrent()&&!terminalPartialSceneRowsCurrent()&&!terminalLateBorrowRowsCurrent()&&!terminalLoadedPageRowsCurrent()&&!terminalWattoBindingRowsCurrent())||nativeGroupControlCount_)return false;
    std::array<const OriginalAudioSampleCache50KG::NativeSampleBinding*,finalGarageClipCount> bindings{};
    std::size_t count=0;
    for(const auto& sound:garageAudio_.sounds_)
        if(sound.nativeSample.retainsBacking()||sound.source)return false;
    for(std::size_t i=0;i<OriginalUiGarageWattoAudio50KG::instanceCount;++i){
        const auto& binding=garageAudio_.nativeVoices_[i];
        if(binding.retainsBacking()){
            // Only the native sample28 wing has a cache lease. Stream IDs
            // remain their actual persistent Main-owned PCM, never row A0.
            if(garageAudio_.soundIds_[i]!=OriginalUiGarageWattoAudio50KG::soundCount||
               !garageAudio_.playbackRevisions_[i])return false;
            bindings[count++]=&binding;
        }
    }
    for(std::size_t i=0;i<OriginalUiGarageDroidAudio50GN::soundCount;++i){
        const auto& row=garageDroidAudio_.rows_[i];const auto& voice=garageDroidAudio_.retiredVoices_[i];
        if(row.source||row.sound||row.nativeSample.retainsBacking()||row.nativeVoice.retainsBacking()||
           row.played108||row.playbackRevision)return false;
        if(voice.present){
            if(!voice.pcm.retainsBacking()||!garageDroidAudio_.retainedStoppedVoiceCurrent(i))return false;
            bindings[count++]=&voice.pcm;
        }else if(voice.pcm.retainsBacking())return false;
    }
    FinalNativePcmRetirement next;
    for(std::size_t i=0;i<finalGarageClipCount;++i)
        if(!mixer_->inspectVoice(courseRequiredVoices+i,next.voices[i]))return false;
    if(!mixer_->canRetireStoppedNativeGarageClips(*this,next.voices.data(),next.voices.size())||
       !cache.canCloseAfterStoppedNativeVoices(*this,bindings.data(),count))return false;
    next.mainRevision=controlRevision_;next.mixerRevision=mixer_->controlRevision();next.mixedFrames=mixer_->mixedFrames();
    next.wattoRevision=garageAudio_.revision_;next.droidRevision=garageDroidAudio_.revision_;
    out=next;return true;
}
void OriginalUiMainAudio50KG::retireFinalNativePcmPreflighted(const FinalNativePcmRetirement& receipt)noexcept {
    // The actual Root/namespace caller has completed EVERY rejecting check.
    // No callback, allocation, validation, source operation or failure return
    // occurs from the first raw-clip withdrawal through namespace destruction.
    mixer_->retireStoppedNativeGarageClipsPreflighted();
    finalNativePcmReceipt_=receipt;
    for(auto& voice:finalNativePcmReceipt_.voices)voice.voice.clip={};
    // Raw Mixer borrows are gone before the first strong binding is released.
    for(auto& binding:garageAudio_.nativeVoices_)binding={};
    garageDroidAudio_.retiredVoices_={};
    finalNativePcmRetired_=true;
}
bool OriginalUiMainAudio50KG::finalNativePcmRetirementCurrent()const noexcept {
    if(!finalNativePcmRetired_||!ordinaryOutputRetired_||!retainedGarageStorageCurrent()||
       !ordinaryVoiceOwnershipCurrent()||nativeGroupControlCount_||!garageEntryCustody_.expired()||
       !garageAudio_.nativeSampleLeasesEmpty()||!garageDroidAudio_.nativeSampleLeasesEmpty()||
       garageAudio_.sourceGroup_||garageAudio_.sourceGroupAttempted_||garageDroidAudio_.sourceSceneGroup_||
       controlRevision_!=finalNativePcmReceipt_.mainRevision||mixer_->controlRevision()!=finalNativePcmReceipt_.mixerRevision||
       mixer_->mixedFrames()!=finalNativePcmReceipt_.mixedFrames||garageAudio_.revision_!=finalNativePcmReceipt_.wattoRevision||
       garageDroidAudio_.revision_!=finalNativePcmReceipt_.droidRevision||
       mixer_->nativeWattoRouting_.command||mixer_->nativeSceneRouting_.command)return false;
    for(std::size_t slot=0;slot<mixerCount_;++slot)
        if(mixer_->active(slot)||mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    for(const auto& sound:garageAudio_.sounds_)if(sound.source)return false;
    for(const auto& row:garageDroidAudio_.rows_)
        if(row.source||row.sound||row.played108||row.playbackRevision)return false;
    for(std::size_t i=0;i<finalGarageClipCount;++i){
        AudioVoiceObservation50HD now;const auto& old=finalNativePcmReceipt_.voices[i];
        if(!mixer_->inspectVoice(courseRequiredVoices+i,now)||now.voice.active||
           mixer_->nativeSourceVoiceReserved(courseRequiredVoices+i,now.playbackRevision)||
           !sameClip(now.voice.clip,old.voice.clip)||now.playbackRevision!=old.playbackRevision||
           now.voice.sourceFrame!=old.voice.sourceFrame||now.voice.gain!=old.voice.gain||now.voice.pitch!=old.voice.pitch||
           now.voice.pan!=old.voice.pan||now.voice.loop!=old.voice.loop||now.groupKnown!=old.groupKnown||
           now.groupPaused!=old.groupPaused||now.groupGain!=old.groupGain||
           now.originalLeft!=old.originalLeft||now.originalRight!=old.originalRight)return false;
    }
    return true;
}
bool OriginalUiMainAudio50KG::beginRetainedGarageEntry(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    const auto fail=[&](const char* text)noexcept {try{error=text;}catch(...){error.clear();}return false;};
    const auto backing=garagePersistentBacking_.lock();std::uint64_t generation=0,epoch=0;
    if(!entry||!backing||!retainedGarageOrdinaryCurrent()||!retainedGarageEntryCurrent()||paused_||!canRecordControlChange()||
       (garageSourceLifetime_!=GarageSourceLifetime::initialPrefix&&garageSourceLifetime_!=GarageSourceLifetime::retired)||
       !entry->nativeAudioConstructionIdentity(*backing->issuer_,*this,generation,epoch)||
       epoch<=garageEntryEpoch_||entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_))
        return fail("Garage source reconstruction requires a new actual root capsule/grant on the same retained transport");
    // Generation is an actual source key and may be a reused address. Only
    // the real monotonically issued Scope epoch orders separate lifetimes.
    garageEntryCustody_=entry;garageEntryGeneration_=generation;garageEntryEpoch_=epoch;
    garageSourceLifetime_=GarageSourceLifetime::constructing;noteControlChange();error.clear();return true;
}
bool OriginalUiMainAudio50KG::emptyGarageEntryFailureCurrent(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,bool managerRetired)const noexcept {
    // Ordinary Song/effects may still own active PCM. This discharges only an
    // entry which never constructed a Garage Sound, Scene, Watto or group.
    if(!entry||garageSourceLifetime_!=GarageSourceLifetime::constructing||
       !retainedGarageOrdinaryCurrent()||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       !reconstructibleGarageRowsCurrent()||nativeGroupControlCount_||
       mixer_->nativeWattoRouting_.command||mixer_->nativeSceneRouting_.command||
       !(silentGarageVoiceRangesCurrent()||ownedStoppedGarageClipHistoryCurrent())||
       !canRecordControlChange())return false;
    for(const auto& command:nativeGroupControls_)if(command)return false;
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot)
        if(mixer_->active(slot)||mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    const auto owned=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!owned||owned!=entry||owned.owner_before(entry)||entry.owner_before(owned)||
       !backing||!backing->issuer_||entry->audio_.get()!=this||
       entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_)||
       !garageEntryGeneration_||!garageEntryEpoch_)return false;
    return entry->nativeAudioEmptyEntryFailureIdentity(*backing->issuer_,*this,
        garageEntryGeneration_,garageEntryEpoch_,managerRetired);
}
bool OriginalUiMainAudio50KG::preflightEmptyGarageEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)const noexcept {
    if(!emptyGarageEntryFailureCurrent(entry,false)){
        try{error="Failed Garage base-cache entry does not own an exact empty constructing audio prefix";}
        catch(...){error.clear();}return false;
    }
    error.clear();return true;
}
bool OriginalUiMainAudio50KG::retireEmptyGarageEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    if(!emptyGarageEntryFailureCurrent(entry,true)){
        try{error="Failed Garage audio retirement requires actual selected, transfer, bank and empty manager retirement";}
        catch(...){error.clear();}return false;
    }
    // The Session's separate completed failed-prefix receipt authenticates
    // the real source cleanup. No ordinary voice, raw clip, strong PCM lease,
    // source-route age, pending namespace, Aural/DSP state or sample clock is
    // changed. Retain actual entry generation/epoch; never set Page rearm.
    garageSourceLifetime_=GarageSourceLifetime::emptyEntryRetired;
    garageEntryCustody_.reset();noteControlChange();error.clear();return true;
}
bool OriginalUiMainAudio50KG::unresolvedLateBorrowActorAudioCurrent(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGaragePodScene50KG& scene)const noexcept {
    // GUI77 is a distinct real count10/Camera3/zero-late failed call. The
    // constructor accessor correctly rejects Session.failed_; do not reopen it.
    if(!page.lateSceneBorrowFailureCurrent()||
       garageSourceLifetime_!=GarageSourceLifetime::constructing||paused_||
       !retainedGarageOrdinaryCurrent()||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       nativeGroupControlCount_!=2||scene.main_.get()!=this||scene.backend_!=&garageDroidAudio_||
       !precedingGroupControlsCurrent()||!silentGarageVoiceRangesCurrent())return false;
    const auto entry=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!entry||!backing||!backing->issuer_||backing->issuer_!=page.issuer_||
       entry->page_.get()!=&page||entry->podScene_!=&scene||entry->audio_.get()!=this||
       entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_)||
       garageEntryGeneration_!=page.failedLateBorrowGeneration_||
       garageEntryEpoch_!=page.failedLateBorrowEpoch_)return false;
    const auto& watto=nativeGroupControls_[0];const auto& command=nativeGroupControls_[1];
    if(!watto||watto->kind!=NativeGroupControlKind::watto||!watto->routingApplied||
       !command||command->kind!=NativeGroupControlKind::scene||command->scene.get()!=&scene||
       command->scene.owner_before(entry->page_)||entry->page_.owner_before(command->scene)||
       !queuedGroupIdentityCurrent(*command)||!queuedGroupRoutingCurrent(*command)||
       !queuedGroupHistoryCurrent(*command))return false;
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot)
        if(mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    return true;
}
bool OriginalUiMainAudio50KG::unresolvedLoadedPageActorAudioCurrent(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGaragePodScene50KG& scene)const noexcept {
    // GUI78 reached the actual loaded Page/table publication and failed hide.
    // Its distinct receipt proves initial10/Camera3/late3 construction history,
    // with no Page entry, final sample flush or Garage frame. Child teardown
    // must not resolve Sounds, consume accepted controls or alter ordinary PCM.
    if(!page.loadedPageHideFailureCurrent()||
       garageSourceLifetime_!=GarageSourceLifetime::constructing||paused_||
       !retainedGarageOrdinaryCurrent()||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       nativeGroupControlCount_!=2||scene.main_.get()!=this||scene.backend_!=&garageDroidAudio_||
       !precedingGroupControlsCurrent()||!silentGarageVoiceRangesCurrent())return false;
    const auto entry=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!entry||!backing||!backing->issuer_||backing->issuer_!=page.issuer_||
       entry->page_.get()!=&page||entry->podScene_!=&scene||entry->audio_.get()!=this||
       entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_)||
       garageEntryGeneration_!=page.failedLoadedPageHideGeneration_||
       garageEntryEpoch_!=page.failedLoadedPageHideEpoch_)return false;
    const auto& watto=nativeGroupControls_[0];const auto& command=nativeGroupControls_[1];
    if(!watto||watto->kind!=NativeGroupControlKind::watto||!watto->routingApplied||
       !command||command->kind!=NativeGroupControlKind::scene||command->scene.get()!=&scene||
       command->scene.owner_before(entry->page_)||entry->page_.owner_before(command->scene)||
       !queuedGroupIdentityCurrent(*command)||!queuedGroupRoutingCurrent(*command)||
       !queuedGroupHistoryCurrent(*command))return false;
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot)
        if(mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    return true;
}
bool OriginalUiMainAudio50KG::unresolvedWattoBindingActorAudioCurrent(
    const OriginalUiGaragePageOwner50KG& page,const OriginalUiGaragePodScene50KG& scene)const noexcept {
    // GUI80 completed activation and the real profile/appearance prefix,
    // then failed the initial Watto binding guard before any binding effect.
    // Its distinct live Page receipt preserves that history through child
    // teardown; it proves no sample flush, Garage voice or runtime frame.
    if(!page.wattoBindingFailureCurrent()||
       garageSourceLifetime_!=GarageSourceLifetime::constructing||paused_||
       !retainedGarageOrdinaryCurrent()||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       nativeGroupControlCount_!=2||scene.main_.get()!=this||scene.backend_!=&garageDroidAudio_||
       !precedingGroupControlsCurrent()||!silentGarageVoiceRangesCurrent())return false;
    const auto entry=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!entry||!backing||!backing->issuer_||backing->issuer_!=page.issuer_||
       entry->page_.get()!=&page||entry->podScene_!=&scene||entry->audio_.get()!=this||
       entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_)||
       garageEntryGeneration_!=page.failedWattoBindingGeneration_||
       garageEntryEpoch_!=page.failedWattoBindingEpoch_)return false;
    const auto& watto=nativeGroupControls_[0];const auto& command=nativeGroupControls_[1];
    if(!watto||watto->kind!=NativeGroupControlKind::watto||!watto->routingApplied||
       !command||command->kind!=NativeGroupControlKind::scene||command->scene.get()!=&scene||
       command->scene.owner_before(entry->page_)||entry->page_.owner_before(command->scene)||
       !queuedGroupIdentityCurrent(*command)||!queuedGroupRoutingCurrent(*command)||
       !queuedGroupHistoryCurrent(*command))return false;
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot)
        if(mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    return true;
}
bool OriginalUiMainAudio50KG::unresolvedSceneEntryFailureCurrent(
    const OriginalUiGarageSession50KG& entry,const OriginalUiGaragePodScene50KG& scene,
    bool sceneRetiring)const noexcept {
    // Exact pre-flush failed entry only. Ordinary Song/PCM are deliberately not
    // stopped here. This is not a normal Page/Scene retirement or rearm receipt.
    if(garageSourceLifetime_!=GarageSourceLifetime::constructing||
       !retainedGarageOrdinaryCurrent()||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       paused_||nativeGroupControlCount_!=2||scene.main_.get()!=this||
       scene.backend_!=&garageDroidAudio_||!precedingGroupControlsCurrent())return false;
    const auto owned=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!owned||owned.get()!=&entry||!backing||!backing->issuer_||entry.audio_.get()!=this||
       entry.audio_.owner_before(backing)||backing.owner_before(entry.audio_)||
       entry.podScene_!=&scene||!garageEntryGeneration_||!garageEntryEpoch_||
       !entry.nativeAudioPartialSceneFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,sceneRetiring))return false;
    const auto& watto=nativeGroupControls_[0];const auto& command=nativeGroupControls_[1];
    if(!watto||watto->kind!=NativeGroupControlKind::watto||!watto->routingApplied||
       !command||command->kind!=NativeGroupControlKind::scene||command->scene.get()!=&scene||
       !queuedGroupIdentityCurrent(*command)||!queuedGroupRoutingCurrent(*command)||
       !queuedGroupHistoryCurrent(*command))return false;
    const std::uint64_t pending=command->routingApplied?0u:1u;
    if(!canRecordControlChange(pending)||!mixer_->canRecordControlChanges(pending))return false;
    // Never turn a completed/played source row into an unresolved cancellation.
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot)
        if(mixer_->active(slot)||mixer_->playbackRevision(slot)||
           mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    return true;
}
bool OriginalUiMainAudio50KG::unresolvedLateBorrowEntryFailureCurrent(
    const OriginalUiGarageSession50KG& entry,const OriginalUiGaragePodScene50KG& scene,
    bool sceneRetiring)const noexcept {
    // Exact GUI77 late-borrow failure only. Ordinary Song/PCM are deliberately not
    // stopped here. This is not a normal Page/Scene retirement or rearm receipt.
    if(garageSourceLifetime_!=GarageSourceLifetime::constructing||
       !retainedGarageOrdinaryCurrent()||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       paused_||nativeGroupControlCount_!=2||scene.main_.get()!=this||
       scene.backend_!=&garageDroidAudio_||!precedingGroupControlsCurrent())return false;
    const auto owned=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!owned||owned.get()!=&entry||!backing||!backing->issuer_||entry.audio_.get()!=this||
       entry.audio_.owner_before(backing)||backing.owner_before(entry.audio_)||
       entry.podScene_!=&scene||!garageEntryGeneration_||!garageEntryEpoch_||
       !entry.nativeAudioLateBorrowFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,sceneRetiring))return false;
    const auto& watto=nativeGroupControls_[0];const auto& command=nativeGroupControls_[1];
    if(!watto||watto->kind!=NativeGroupControlKind::watto||!watto->routingApplied||
       !command||command->kind!=NativeGroupControlKind::scene||command->scene.get()!=&scene||
       !queuedGroupIdentityCurrent(*command)||!queuedGroupRoutingCurrent(*command)||
       !queuedGroupHistoryCurrent(*command))return false;
    const std::uint64_t pending=command->routingApplied?0u:1u;
    if(!canRecordControlChange(pending)||!mixer_->canRecordControlChanges(pending))return false;
    // Never turn a completed/played source row into an unresolved cancellation.
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot)
        if(mixer_->active(slot)||mixer_->playbackRevision(slot)||
           mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    return true;
}
bool OriginalUiMainAudio50KG::unresolvedLoadedPageEntryFailureCurrent(
    const OriginalUiGarageSession50KG& entry,const OriginalUiGaragePodScene50KG& scene,
    bool sceneRetiring)const noexcept {
    // Exact GUI78 loaded-Page hide failure only. Ordinary Song/PCM are deliberately not
    // stopped here. This is not a normal Page/Scene retirement or rearm receipt.
    if(garageSourceLifetime_!=GarageSourceLifetime::constructing||
       !retainedGarageOrdinaryCurrent()||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       paused_||nativeGroupControlCount_!=2||scene.main_.get()!=this||
       scene.backend_!=&garageDroidAudio_||!precedingGroupControlsCurrent())return false;
    const auto owned=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!owned||owned.get()!=&entry||!backing||!backing->issuer_||entry.audio_.get()!=this||
       entry.audio_.owner_before(backing)||backing.owner_before(entry.audio_)||
       entry.podScene_!=&scene||!garageEntryGeneration_||!garageEntryEpoch_||
       !entry.nativeAudioLoadedPageFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,sceneRetiring))return false;
    const auto& watto=nativeGroupControls_[0];const auto& command=nativeGroupControls_[1];
    if(!watto||watto->kind!=NativeGroupControlKind::watto||!watto->routingApplied||
       !command||command->kind!=NativeGroupControlKind::scene||command->scene.get()!=&scene||
       !queuedGroupIdentityCurrent(*command)||!queuedGroupRoutingCurrent(*command)||
       !queuedGroupHistoryCurrent(*command))return false;
    const std::uint64_t pending=command->routingApplied?0u:1u;
    if(!canRecordControlChange(pending)||!mixer_->canRecordControlChanges(pending))return false;
    // Never turn a completed/played source row into an unresolved cancellation.
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot)
        if(mixer_->active(slot)||mixer_->playbackRevision(slot)||
           mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    return true;
}
bool OriginalUiMainAudio50KG::unresolvedWattoBindingEntryFailureCurrent(
    const OriginalUiGarageSession50KG& entry,const OriginalUiGaragePodScene50KG& scene,
    bool sceneRetiring)const noexcept {
    // Exact GUI80 pre-bind entry failure only. Ordinary Song/PCM are deliberately not
    // stopped here. This is not a normal Page/Scene retirement or rearm receipt.
    if(garageSourceLifetime_!=GarageSourceLifetime::constructing||
       !retainedGarageOrdinaryCurrent()||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       paused_||nativeGroupControlCount_!=2||scene.main_.get()!=this||
       scene.backend_!=&garageDroidAudio_||!precedingGroupControlsCurrent())return false;
    const auto owned=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!owned||owned.get()!=&entry||!backing||!backing->issuer_||entry.audio_.get()!=this||
       entry.audio_.owner_before(backing)||backing.owner_before(entry.audio_)||
       entry.podScene_!=&scene||!garageEntryGeneration_||!garageEntryEpoch_||
       !entry.nativeAudioWattoBindingFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,sceneRetiring))return false;
    const auto& watto=nativeGroupControls_[0];const auto& command=nativeGroupControls_[1];
    if(!watto||watto->kind!=NativeGroupControlKind::watto||!watto->routingApplied||
       !command||command->kind!=NativeGroupControlKind::scene||command->scene.get()!=&scene||
       !queuedGroupIdentityCurrent(*command)||!queuedGroupRoutingCurrent(*command)||
       !queuedGroupHistoryCurrent(*command))return false;
    const std::uint64_t pending=command->routingApplied?0u:1u;
    if(!canRecordControlChange(pending)||!mixer_->canRecordControlChanges(pending))return false;
    // Never turn a completed/played source row into an unresolved cancellation.
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot)
        if(mixer_->active(slot)||mixer_->playbackRevision(slot)||
           mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    return true;
}
bool OriginalUiMainAudio50KG::sharedPageEntryFailureCurrent(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,bool managerRetired)const noexcept {
    // This first-entry branch has never played a Garage voice. Ordinary
    // Song/effects and the persistent mixer clock may already be running.
    if(!entry||garageSourceLifetime_!=GarageSourceLifetime::constructing||paused_||
       !retainedGarageOrdinaryCurrent()||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       !garageDroidAudio_.coldConstructionCurrent()||garageDroidAudio_.sourceSceneGroup_||
       garageDroidAudio_.sourceSceneGroupReleased_||!garageAudio_.current()||
       !silentGarageVoiceRangesCurrent()||!canRecordControlChange(managerRetired?1u:2u))return false;
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot)
        if(mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    const auto owned=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!owned||owned!=entry||owned.owner_before(entry)||entry.owner_before(owned)||
       !backing||!backing->issuer_||entry->audio_.get()!=this||
       entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_)||
       !garageEntryGeneration_||!garageEntryEpoch_||
       !entry->nativeAudioSharedPageFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,managerRetired))return false;
    if(managerRetired){
        // Real235150 has already released the source slot, all Sounds, the
        // Pending borrow and applied command. No Scene receipt is invented.
        if(nativeGroupControlCount_||mixer_->nativeWattoRouting_.command||
           mixer_->nativeSceneRouting_.command||!garageAudio_.canRearmRetiredSourceGroupCold())return false;
        for(const auto& command:nativeGroupControls_)if(command)return false;
        return true;
    }
    if(nativeGroupControlCount_!=1||!nativeGroupControls_[0]||
       mixer_->nativeSceneRouting_.command||!garageAudio_.canRevise(2))return false;
    for(std::size_t i=1;i<nativeGroupControls_.size();++i)if(nativeGroupControls_[i])return false;
    const auto& c=*nativeGroupControls_[0];
    return c.kind==NativeGroupControlKind::watto&&c.group&&c.routingApplied&&c.wetProcessor&&
        c.group.get()==garageAudio_.sourceGroup_&&c.grant==entry->grant_&&
        !c.grant.owner_before(entry->grant_)&&!entry->grant_.owner_before(c.grant)&&
        queuedGroupSourceLifetimeCurrent(c)&&c.group->routingHistoryCurrent()&&
        c.group->wingResolved26D9F0(entry->pending_)&&c.group->preflightPendingRetirement235150();
}
bool OriginalUiMainAudio50KG::preflightSharedPageEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)const noexcept {
    if(!sharedPageEntryFailureCurrent(entry,false)){
        try{error="Failed shared Garage Page does not own the exact resolved, never-played Watto prefix";}
        catch(...){error.clear();}return false;
    }
    error.clear();return true;
}
bool OriginalUiMainAudio50KG::retireSharedPageEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    if(!sharedPageEntryFailureCurrent(entry,true)){
        try{error="Failed shared Page audio requires actual Page, Watto, bank and manager retirement";}
        catch(...){error.clear();}return false;
    }
    // The existing operation clears ONLY already-retired SourceSound records.
    // No source constructor, Scene rearm, PCM stop/clear or DSP mutation occurs.
    // Its native prefix is retained solely for terminal process reclamation.
    if(!garageAudio_.rearmRetiredSourceGroupCold())std::terminate();
    garageSourceLifetime_=GarageSourceLifetime::sharedPageEntryRetired;
    garageEntryCustody_.reset();noteControlChange();error.clear();return true;
}
bool OriginalUiMainAudio50KG::unresolvedEntryFailureCurrent(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,bool managerRetired,
    UnresolvedEntryFailure kind)const noexcept {
    // Include the existing235150 Sound-stop/drain capacity check, then the
    // accepted Scene application and the two exact routing-claim retirements.
    const std::uint64_t mixerBudget=managerRetired?1u:
        OriginalUiGarageWattoAudio50KG::soundCount+OriginalUiGarageWattoAudio50KG::instanceCount+3u;
    if(!entry||garageSourceLifetime_!=GarageSourceLifetime::constructing||paused_||
       !retainedGarageOrdinaryCurrent()||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       !silentGarageVoiceRangesCurrent()||!canRecordControlChange(managerRetired?2u:4u)||
       !mixer_->canRecordControlChanges(mixerBudget))return false;
    for(std::size_t slot=courseRequiredVoices;slot<garageSceneRequiredVoices;++slot)
        if(mixer_->nativeSourceVoiceRoutes_[slot].count)return false;
    const auto owned=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!owned||owned!=entry||owned.owner_before(entry)||entry.owner_before(owned)||
       !backing||!backing->issuer_||entry->audio_.get()!=this||
       entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_)||
       !garageEntryGeneration_||!garageEntryEpoch_)return false;
    bool sourceIdentity=false;
    switch(kind){
    case UnresolvedEntryFailure::partialAttachment:
        sourceIdentity=entry->nativeAudioPartialSceneTerminalFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,managerRetired);break;
    case UnresolvedEntryFailure::lateBorrow:
        sourceIdentity=entry->nativeAudioLateBorrowTerminalFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,managerRetired);break;
    case UnresolvedEntryFailure::loadedPageHide:
        sourceIdentity=entry->nativeAudioLoadedPageTerminalFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,managerRetired);break;
    case UnresolvedEntryFailure::wattoBinding:
        sourceIdentity=entry->nativeAudioWattoBindingTerminalFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,managerRetired);break;
    }
    if(!sourceIdentity)return false;
    if(!managerRetired){
        const auto* scene=entry->podScene_;
        if(!scene||!garageAudio_.canRevise(2))return false;
        bool groupIdentity=false;
        switch(kind){
        case UnresolvedEntryFailure::partialAttachment:
            groupIdentity=unresolvedSceneEntryFailureCurrent(*entry,*scene,false)&&
                scene->canRetireUnresolvedSoundGroupForEntryFailure(*entry,false);break;
        case UnresolvedEntryFailure::lateBorrow:
            groupIdentity=unresolvedLateBorrowEntryFailureCurrent(*entry,*scene,false)&&
                scene->canRetireUnresolvedSoundGroupForLateBorrowFailure(*entry,false);break;
        case UnresolvedEntryFailure::loadedPageHide:
            groupIdentity=unresolvedLoadedPageEntryFailureCurrent(*entry,*scene,false)&&
                scene->canRetireUnresolvedSoundGroupForLoadedPageFailure(*entry,false);break;
        case UnresolvedEntryFailure::wattoBinding:
            groupIdentity=unresolvedWattoBindingEntryFailureCurrent(*entry,*scene,false)&&
                scene->canRetireUnresolvedSoundGroupForWattoBindingFailure(*entry,false);break;
        }
        if(!groupIdentity)return false;
        const auto& watto=nativeGroupControls_[0];
        return watto&&watto->group&&watto->group.get()==garageAudio_.sourceGroup_&&
            watto->group->routingHistoryCurrent()&&
            watto->group->wingResolved26D9F0(entry->pending_)&&
            watto->group->preflightPendingRetirement235150();
    }
    // No live constructor/group predicate can prove a destroyed source owner.
    // Session owns the actual completed failed-manager receipt; Page retains
    // the dead Scene allocation separately after its genuine 1FA1BC null store.
    const auto& page=entry->page_;
    if(!page||!page->retiredSceneBacking_||page->sceneBacking_||page->scene4C0_||
       !page->sceneReferenceReleased_||page->entityLive_||!page->entityRefReleased_||
       !page->entityCountReleased_||page->entity_.reference00||page->entity_.firstChild18||
       page->stage_!=OriginalUiGaragePageOwner50KG::Stage::pagePayloadRetired||
       !garageDroidAudio_.nativeUnresolvedEntryRetired_||
       !garageDroidAudio_.retiredSceneTransportCurrent()||
       !garageAudio_.canRearmRetiredSourceGroupCold()||nativeGroupControlCount_!=1||
       mixer_->nativeWattoRouting_.command)return false;
    auto& scene=*page->retiredSceneBacking_;
    // A shared mechanical body never substitutes one failure's actual receipt
    // for another. New loaded-Page authority also excludes both earlier owners.
    bool sceneOwner=false;
    switch(kind){
    case UnresolvedEntryFailure::partialAttachment:
        sceneOwner=scene.failedPageRetirementOwner_==page.get();break;
    case UnresolvedEntryFailure::lateBorrow:
        sceneOwner=scene.failedLateBorrowRetirementOwner_==page.get()&&!scene.failedPageRetirementOwner_;break;
    case UnresolvedEntryFailure::loadedPageHide:
        sceneOwner=scene.failedLoadedHideRetirementOwner_==page.get()&&
            !scene.failedPageRetirementOwner_&&!scene.failedLateBorrowRetirementOwner_;break;
    case UnresolvedEntryFailure::wattoBinding:
        sceneOwner=scene.failedWattoBindingRetirementOwner_==page.get()&&
            !scene.failedPageRetirementOwner_&&!scene.failedLateBorrowRetirementOwner_&&
            !scene.failedLoadedHideRetirementOwner_;break;
    }
    if(!sceneOwner||entry->podScene_!=&scene||
       !scene.nativeUnresolvedEntryRetiring_||!page->grant_||page->grant_!=scene.grant_||
       page->grant_.owner_before(scene.grant_)||scene.grant_.owner_before(page->grant_)||
       scene.sceneLive_||scene.groupLive_||scene.podScene_.reference00||scene.podScene_.firstChild18||
       scene.retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete||
       scene.soundGroupRetirement_!=OriginalUiGaragePodScene50KG::SoundGroupRetirement::complete||
       scene.group4D8_.entity.reference00||scene.group4D8_.count48_||scene.group4D8_.members4C_||
       scene.group4D8_.capacity54_||scene.pendingWaves_||scene.backend_!=&garageDroidAudio_||
       scene.group4D8_.manager5C!=this||scene.main_.get()!=this||
       scene.group4D8_.entity.sessionGeneration!=page->entity_.sessionGeneration||
       !nativeDroidSampleIdentity(scene.backend_,mixer_,mixerEpoch_))return false;
    for(const auto& sound:scene.group4D8_.sounds_)if(sound)return false;
    for(std::size_t i=1;i<nativeGroupControls_.size();++i)if(nativeGroupControls_[i])return false;
    const std::shared_ptr<OriginalUiGaragePodScene50KG> actual(page,&scene);
    const auto issued=scene.nativeRoutingLifetime_.lock();const auto& c=nativeGroupControls_[0];
    if(!issued||issued!=actual||issued.owner_before(actual)||actual.owner_before(issued)||
       !c||c->kind!=NativeGroupControlKind::scene||c->group||c->scene!=actual||
       c->scene.owner_before(actual)||actual.owner_before(c->scene)||
       !c->routingApplied||!c->sequence||c->sequence>nativeGroupControlSequence_||
       c->command!=25||c->slot<0||c->slot>=25||
       c->arguments!=std::array<std::uint32_t,3>{static_cast<std::uint32_t>(c->slot),0u,1u}||
       c->grant!=scene.grant_||c->grant.owner_before(scene.grant_)||scene.grant_.owner_before(c->grant)||
       c->entity!=&scene.group4D8_.entity||c->slot!=scene.group4D8_.slot40_||
       c->generation!=page->entity_.sessionGeneration||c->objectKey!=scene.group4D8_.entity.objectKey||
       c->objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&scene.group4D8_.entity))||
       c->mixer!=mixer_||c->epoch!=mixerEpoch_||c->voiceCount!=mixerCount_||
       c->outputRate!=mixerRate_||c->soundMode!=preparedSoundMode_||!queuedGroupProcessorCurrent(*c)||
       !mixer_->canRetireNativeSceneRouting(c.get()))return false;
    const auto main=c->main.lock();
    return main==scene.main_&&!main.owner_before(scene.main_)&&!scene.main_.owner_before(main);
}
bool OriginalUiMainAudio50KG::partialSceneEntryFailureCurrent(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,bool managerRetired)const noexcept {
    return unresolvedEntryFailureCurrent(entry,managerRetired,UnresolvedEntryFailure::partialAttachment);
}
bool OriginalUiMainAudio50KG::preflightPartialSceneEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)const noexcept {
    if(!partialSceneEntryFailureCurrent(entry,false)){
        try{error="Failed partial Scene entry lacks exact unresolved Scene and resolved Watto cleanup custody";}
        catch(...){error.clear();}return false;
    }
    error.clear();return true;
}
bool OriginalUiMainAudio50KG::retireUnresolvedEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,UnresolvedEntryFailure kind,
    std::string& error)noexcept {
    GarageSourceLifetime terminalState=GarageSourceLifetime::unbound;
    const char* why=nullptr;
    switch(kind){
    case UnresolvedEntryFailure::partialAttachment:
        terminalState=GarageSourceLifetime::partialSceneEntryRetired;
        why="Failed partial Scene audio requires actual Page, both Groups and complete manager retirement";break;
    case UnresolvedEntryFailure::lateBorrow:
        terminalState=GarageSourceLifetime::lateBorrowEntryRetired;
        why="Failed late-borrow Scene audio requires actual Page, both Groups and complete manager retirement";break;
    case UnresolvedEntryFailure::loadedPageHide:
        terminalState=GarageSourceLifetime::loadedPageEntryRetired;
        why="Failed loaded Page audio requires actual Page, both Groups and complete manager retirement";break;
    case UnresolvedEntryFailure::wattoBinding:
        terminalState=GarageSourceLifetime::wattoBindingEntryRetired;
        why="Failed Watto binding audio requires actual Page, both Groups and complete manager retirement";break;
    }
    if(!why||!unresolvedEntryFailureCurrent(entry,true,kind)){
        try{error=why?why:"Unknown native failed-entry family";}catch(...){error.clear();}return false;
    }
    // The command really applied before its Group died. Only now can its raw
    // Mixer borrow and strong Page alias retire. Nothing resets routing bytes,
    // ordinary voices, PCM, source ages, sample clock or the retained wet DSP.
    if(!mixer_->retireNativeSceneRouting(nativeGroupControls_[0].get()))std::terminate();
    noteControlChange();nativeGroupControls_[0].reset();nativeGroupControlCount_=0;
    // This existing operation frees only genuinely dead Watto SourceSound
    // records. Droid remains explicitly unresolved-retired, never rearmed.
    if(!garageAudio_.rearmRetiredSourceGroupCold())std::terminate();
    entry->page_->retiredSceneBacking_.reset();
    garageSourceLifetime_=terminalState;
    garageEntryCustody_.reset();noteControlChange();error.clear();return true;
}
bool OriginalUiMainAudio50KG::retirePartialSceneEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    return retireUnresolvedEntryFailure(entry,UnresolvedEntryFailure::partialAttachment,error);
}
bool OriginalUiMainAudio50KG::lateBorrowEntryFailureCurrent(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,bool managerRetired)const noexcept {
    return unresolvedEntryFailureCurrent(entry,managerRetired,UnresolvedEntryFailure::lateBorrow);
}
bool OriginalUiMainAudio50KG::loadedPageEntryFailureCurrent(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,bool managerRetired)const noexcept {
    return unresolvedEntryFailureCurrent(entry,managerRetired,UnresolvedEntryFailure::loadedPageHide);
}
bool OriginalUiMainAudio50KG::wattoBindingEntryFailureCurrent(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,bool managerRetired)const noexcept {
    return unresolvedEntryFailureCurrent(entry,managerRetired,UnresolvedEntryFailure::wattoBinding);
}
bool OriginalUiMainAudio50KG::preflightLateBorrowEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)const noexcept {
    if(!lateBorrowEntryFailureCurrent(entry,false)){
        try{error="Failed late-borrow Scene entry lacks exact unresolved Scene and resolved Watto cleanup custody";}
        catch(...){error.clear();}return false;
    }
    error.clear();return true;
}
bool OriginalUiMainAudio50KG::retireLateBorrowEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    return retireUnresolvedEntryFailure(entry,UnresolvedEntryFailure::lateBorrow,error);
}
bool OriginalUiMainAudio50KG::preflightLoadedPageEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)const noexcept {
    if(!loadedPageEntryFailureCurrent(entry,false)){
        try{error="Failed loaded Page entry lacks exact unresolved Scene and resolved Watto cleanup custody";}
        catch(...){error.clear();}return false;
    }
    error.clear();return true;
}
bool OriginalUiMainAudio50KG::preflightWattoBindingEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)const noexcept {
    if(!wattoBindingEntryFailureCurrent(entry,false)){
        try{error="Failed Watto binding entry lacks exact unresolved Scene and resolved Watto cleanup custody";}
        catch(...){error.clear();}return false;
    }
    error.clear();return true;
}
bool OriginalUiMainAudio50KG::retireLoadedPageEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    return retireUnresolvedEntryFailure(entry,UnresolvedEntryFailure::loadedPageHide,error);
}
bool OriginalUiMainAudio50KG::retireWattoBindingEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    return retireUnresolvedEntryFailure(entry,UnresolvedEntryFailure::wattoBinding,error);
}
OriginalUiGarageDroidAudio50GN* OriginalUiMainAudio50KG::garageDroidAudioCold() noexcept {
    if(!prepared_||!garageColdPending_||garageColdFinishAttempted_||garagePrepared_||!mixer_||
       mixer_->groupEpoch50IC()!=mixerEpoch_||mixer_->count()!=garageSceneRequiredVoices||
       mixer_->sampleRate()!=mixerRate_||mixer_->originalSoundMode50KG()!=preparedSoundMode_||
       paused_||!garageDroidAudio_.transportValid())return nullptr;
    if(garageSourceLifetime_!=GarageSourceLifetime::unbound){
        if(garageSourceLifetime_!=GarageSourceLifetime::constructing||
           !retainedGarageEntryCurrent()||!retainedGarageOrdinaryCurrent())return nullptr;
    }else if(mixer_->mixedFrames()||ownsMusic_)return nullptr;
    return &garageDroidAudio_;
}
bool OriginalUiMainAudio50KG::finishGarageSceneColdTransport(std::string& error) noexcept {
    if(!garageDroidAudioCold()||!garageDroidAudio_.sourceConstructionComplete()||!garageColdMetadata_||!nativeWattoSampleTransport()){
        error="Garage audio source construction requires all real actor registrations and the earlier Watto transport";return false;
    }
    // Registration completion is not native sample publication or playback readiness.
    // Source233110 returns1 before sample28 resolves at the later loader flush.
    garageColdFinishAttempted_=true;
    garagePrepared_=true;garageScenePrepared_=true;garageColdPending_=false;
    garageColdMetadata_.reset();garageColdRoot_.clear();
    if(garageSourceLifetime_!=GarageSourceLifetime::unbound)garageSourceLifetime_=GarageSourceLifetime::constructed;
    noteControlChange();error.clear();return true;
}
bool OriginalUiMainAudio50KG::prepareImpl(const std::filesystem::path& audioRoot,
    AudioMixer50HD& mixer,std::string& error,bool garage,
    std::shared_ptr<const assets::OriginalStreamMetadata50KZ> metadata,bool scene,bool incremental) noexcept {
    if (attempted_) { error="Main audio needs a fresh stable owner"; return false; }
    attempted_=true;
    try {
        need((garage?mixer.count()==(scene?garageSceneRequiredVoices:garageRequiredVoices):(mixer.count()==requiredVoices||mixer.count()==courseRequiredVoices)) && mixer.sampleRate()!=0 && !mixer.requiresVoiceGroupGains50IC(),
            "Main audio requires a dedicated fresh frontend mixer");
        for (std::size_t i=0; i<mixer.count(); ++i)
            need(!mixer.active(i) && !mixer.hasVoiceGroupGain50IC(i), "Frontend mixer is already owned");
        authenticatedRead(audioRoot,"UIEVENTS.INI",790,"ed897d482c72b158f209c20baa6c4e8270a10cdbb9031315cd143a8666557115");
        authenticatedRead(audioRoot,"UI_MUSIC.JUK",1517,"4b814f8e4fec2fbcfa25807d5e147ecc164dbb0b0cb9ce5d064e2f3e3c89de98");
        authenticatedRead(audioRoot,"UI_MUSIC.JUB",3623,"f97a5cbceec8056dfc9b277501087aaa54d86a0ccdc0c71cfde73522a048c661");
        // The feature owner retains authentic immutable registry rows. This
        // does not add a registry dependency to ordinary dry Main preparation.
        if(garage){
            need(metadata&&metadata->prepared(),"Feature source voices require authenticated APPSTART stream metadata");
            nativeSourceMetadata_=metadata;
        }
        std::array<assets::AudioPcm50HC,7> next;
        for (std::size_t i=0; i<originals.size(); ++i) {
            const auto& def=originals[i]; const auto bytes=authenticatedRead(audioRoot,def.path,def.bytes,def.sha);
            if (i==0) next[i]=decodeMenuStereo(bytes,def,219504);
            else {
                if(garage){
                    //2332F0/268930 -> PS2SOUND encoded-header producer ->26C200.
                    // This is NOT decoded PCM length, and these are not guest IDs.
                    static constexpr std::array<std::uint32_t,6> events{{1,3,4,7,5,2}};
                    const auto payload=be32(bytes.data()+12),rate=be32(bytes.data()+16);
                    need(bytes.size()>=48&&payload==bytes.size()-48&&rate==22050,
                        "Original effect encoded source metadata differs");
                    const std::uint64_t divisor=std::uint64_t(rate/28u)*16u;
                    need(divisor!=0,"Original effect duration divisor is zero");
                    auto& source=nativeEffectSources_[i-1];
                    source.event=events[i-1];source.payloadBytes=payload;source.headerRate=rate;
                    source.durationMs=static_cast<std::uint32_t>(std::uint64_t(payload)*1000u/divisor);
                    source.duration20=static_cast<float>(source.durationMs)/1000.f;
                    source.pitch=static_cast<std::uint32_t>((std::uint64_t(rate)<<12u)/48000u);
                }
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
        if(garage){
            garagePcm_=decodeMenuStereo(authenticatedRead(audioRoot,garageOriginal.path,garageOriginal.bytes,garageOriginal.sha),garageOriginal,265328);
            garageClip_={garagePcm_.samples.data(),garagePcm_.frames,garagePcm_.sampleRate,garagePcm_.channels,0,0};
            need(preparedClipMatches(garageClip_,garagePcm_,garageOriginal,2),"Original Garage Song6 PCM differs");
            std::string detail;
            if(incremental){
                need(scene&&metadata&&metadata->prepared(),"Garage incremental cold transport requires actual authenticated retained metadata");
                garageColdRoot_=audioRoot;garageColdMetadata_=std::move(metadata);
                if(!garageDroidAudio_.prepareColdTransport(audioRoot,mixer,detail))throw std::runtime_error(detail);
                // Native ranges/backing exist before widgets. Actual Watto widget
                // construction precedes Page/Scene/Droid source Sound construction.
                if(!garageAudio_.prepare(audioRoot,garageColdMetadata_,mixer,detail,&garageDroidAudio_))throw std::runtime_error(detail);
                garageColdPending_=true;
            }else{
                if(scene&&!garageDroidAudio_.prepare(audioRoot,mixer,detail))throw std::runtime_error(detail);
                if(!garageAudio_.prepare(audioRoot,std::move(metadata),mixer,detail,scene?&garageDroidAudio_:nullptr))throw std::runtime_error(detail);
                garagePrepared_=true;garageScenePrepared_=scene;
            }
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
    if((nativeWetOwner_||nativeWetProcessor_||nativeWetEpoch_||nativeWetSetRawMasters_||nativeWetPlaySong_||nativeWetPlayEffect_||nativeWetPlayRider_||nativeWetPlayWatto_||nativeWetCanPublish_)&&!nativeWetInterfaceCurrent())return false;
    // An actual root-issued retained lifetime does not become unowned merely
    // because another source Page is constructing or has finished retirement.
    if(garageSourceLifetime_!=GarageSourceLifetime::unbound)return retainedGarageOrdinaryCurrent();
    return prepared_ && !garageColdPending_ && mixer_ && mixer_->groupEpoch50IC()==mixerEpoch_ && mixer_->count()==mixerCount_ &&
        !mixer_->requiresVoiceGroupGains50IC() && (!garagePrepared_||garageGroupCurrent()) &&
        (!garageSong6LeafPrepared_||musicSong_!=6||garageSong6LeafCurrent());
}
bool OriginalUiMainAudio50KG::prepareGarageSong6LeafPcm(const std::filesystem::path& root,
    std::string& error) noexcept {
    if(garageSong6LeafAttempted_){error="Garage Song6 leaf PCM needs one cold preparation";return false;}
    garageSong6LeafAttempted_=true;
    try {
        need(courseMenuReady()&&mixerCount_==courseRequiredVoices&&!garagePrepared_&&!garageScenePrepared_&&
            !garageColdPending_&&!paused_&&!ownsMusic_&&!mixer_->mixedFrames()&&
            mixer_->sampleRate()==mixerRate_&&mixer_->originalSoundMode50KG()==preparedSoundMode_&&
            preparedSoundMode_&&preparedSoundMode_->ready()&&mixer_->masterGain()==1.f&&canRecordControlChange(),
            "Garage Song6 leaf PCM requires the existing cold ordinary8 Course transport");
        for(std::size_t slot=0;slot<mixerCount_;++slot)
            need(!mixer_->active(slot)&&!mixer_->hasVoiceGroupGain50IC(slot)&&!mixer_->playbackRevision(slot),
                "Garage Song6 leaf PCM cannot replace live or previously used voices");
        authenticatedRead(root,"UI_MUSIC.JUK",1517,"4b814f8e4fec2fbcfa25807d5e147ecc164dbb0b0cb9ce5d064e2f3e3c89de98");
        authenticatedRead(root,"UI_MUSIC.JUB",3623,"f97a5cbceec8056dfc9b277501087aaa54d86a0ccdc0c71cfde73522a048c661");
        auto pcm=decodeMenuStereo(authenticatedRead(root,garageOriginal.path,garageOriginal.bytes,garageOriginal.sha),garageOriginal,265328);
        const AudioClip50HD clip{pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,0,0};
        need(preparedClipMatches(clip,pcm,garageOriginal,2),"Garage Song6 leaf authentic PCM differs");
        garagePcm_=std::move(pcm);
        garageClip_={garagePcm_.samples.data(),garagePcm_.frames,garagePcm_.sampleRate,garagePcm_.channels,0,0};
        garageSong6LeafPrepared_=true;noteControlChange();error.clear();return true;
    }catch(const std::exception& e){error=e.what();}
    catch(...){error="Garage Song6 leaf PCM preparation failed";}
    return false;
}
bool OriginalUiMainAudio50KG::garageSong6LeafCurrent() const noexcept {
    // Deliberately does not call current(): current validates an active Song6
    // through this gate before ordinary update/mix/retirement can touch it.
    if(!prepared_||!garageSong6LeafPrepared_||!riderPrepared_||!coursePrepared_||
        garagePrepared_||garageScenePrepared_||garageColdPending_||!mixer_||
        mixerCount_!=courseRequiredVoices||mixer_->count()!=mixerCount_||
        mixer_->groupEpoch50IC()!=mixerEpoch_||mixer_->sampleRate()!=mixerRate_||!mixerRate_||
        mixer_->requiresVoiceGroupGains50IC()||mixer_->masterGain()!=1.f||
        mixer_->originalSoundMode50KG()!=preparedSoundMode_||!preparedSoundMode_||!preparedSoundMode_->ready()||
        !canRecordControlChange(0)||!mixer_->canRecordControlChanges()||
        !preparedClipMatches(garageClip_,garagePcm_,garageOriginal,2)||
        !preparedClipMatches(courseClip_,coursePcm_,courseOriginal,2))return false;
    game::OriginalSoundRouting50KG routing;
    if(!game::projectOriginalSoundRouting26CBA050KG(preparedSoundMode_->fields(),0,100,routing)||
        routing.phaseSelector||routing.positional||routing.leftPercent!=100||routing.rightPercent!=100)return false;
    AudioVoiceObservation50HD voice;
    if(!mixer_->inspectVoice(musicVoice,voice)||voice.groupKnown||voice.groupPaused||voice.groupGain!=1.f||
        voice.playbackRevision!=musicPlaybackRevision_||voice.voice.active!=ownsMusic_)return false;
    if(!ownsMusic_&&(fading_||musicSong_))return false;
    if(!musicPlaybackRevision_)return !ownsMusic_;
    if(ownsMusic_&&musicSong_!=11&&musicSong_!=6)return false;
    const bool course=sameClip(voice.voice.clip,courseClip_);
    const bool garage=sameClip(voice.voice.clip,garageClip_);
    if((!course&&!garage)||(ownsMusic_&&((musicSong_==11&&!course)||(musicSong_==6&&!garage))))return false;
    const float pitch=garage?OriginalUiGarageWattoAudio50KG::nativeRendererPitch:1.f;
    return voice.voice.loop&&voice.voice.gain==musicGain_&&voice.voice.pitch==musicPitch_&&musicPitch_==pitch&&
        voice.voice.pan==0.f&&voice.originalLeft==1.f&&voice.originalRight==1.f&&
        std::isfinite(musicGain_)&&musicGain_>=0.f&&musicGain_<=0.4f&&
        std::isfinite(voice.voice.sourceFrame)&&voice.voice.sourceFrame>=0.0&&
        voice.voice.sourceFrame<static_cast<double>(voice.voice.clip.frames);
}
bool OriginalUiMainAudio50KG::canStartGarageSong6Leaf1FAF58() const noexcept {
    return garageSong6LeafCurrent()&&!paused_&&canRecordControlChange()&&mixer_->canRecordControlChanges(1);
}
bool OriginalUiMainAudio50KG::startGarageSong6Leaf1FAF58() noexcept {
    return canStartGarageSong6Leaf1FAF58()&&startMusicLoop(garageClip_,6,OriginalUiGarageWattoAudio50KG::nativeRendererPitch);
}
bool OriginalUiMainAudio50KG::canFadeGarageSong6Leaf1FB09C() const noexcept {
    return garageSong6LeafCurrent()&&!paused_&&canRecordControlChange(ownsMusic_?1u:0u)&&(!ownsMusic_||musicSong_==6);
}
bool OriginalUiMainAudio50KG::fadeGarageSong6Leaf1FB09C() noexcept {
    return canFadeGarageSong6Leaf1FB09C()&&fadeMain(1.f);
}
bool OriginalUiMainAudio50KG::prepareCourseMenu(const std::filesystem::path& audioRoot,
    std::string& error) noexcept {
    if(courseAttempted_){error="Course audio needs a fresh cold preparation";return false;}
    courseAttempted_=true;
    try {
        need(riderMenuReady()&&!paused_&&!ownsMusic_&&!mixer_->mixedFrames()&&
            courseMixerShape(),"Course audio requires the cold extended frontend transport");
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
            if(nativeSourceMetadata_){
                // UIACCEPT is authenticated above. Its explicit IsStream1 and
                // absent stereo/loop defaults construct flags2, not PLAY flags0.
                std::string request=original.path;
                for(auto& ch:request){if(ch=='/')ch='\\';else if(ch>='A'&&ch<='Z')ch=char(ch-'A'+'a');}
                const auto* row=nativeSourceMetadata_->find(request);
                need(row&&row->sourceName==request&&row->originalSize+48u==original.bytes&&
                    row->originalPitch==1881&&row->originalSampleRate==22050&&
                    std::isfinite(row->admittedDurationSeconds)&&row->admittedDurationSeconds>0.f,
                    "Rider Confirm authentic stream registry row differs");
                nativeRiderSources_[i]=row;
            }
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
bool OriginalUiMainAudio50KG::prepareCreditsMenu(const std::filesystem::path& audioRoot,
    std::string& error) noexcept {
    if(creditsAttempted_){error="Credits music needs a fresh cold preparation";return false;}
    creditsAttempted_=true;
    try {
        need(current()&&!paused_&&!ownsMusic_&&!mixer_->mixedFrames(),
            "Credits music requires the prepared cold frontend transport");
        for(std::size_t i=0;i<mixer_->count();++i)
            need(!mixer_->active(i),"Credits music preparation cannot alter live playback");
        // Both original authoring files establish Song12's exact identity;
        // never borrow Main/Rider/Course PCM or accept a filename substitute.
        authenticatedRead(audioRoot,"UI_MUSIC.JUK",1517,"4b814f8e4fec2fbcfa25807d5e147ecc164dbb0b0cb9ce5d064e2f3e3c89de98");
        authenticatedRead(audioRoot,"UI_MUSIC.JUB",3623,"f97a5cbceec8056dfc9b277501087aaa54d86a0ccdc0c71cfde73522a048c661");
        auto pcm=decodeMenuStereo(authenticatedRead(audioRoot,creditsOriginal.path,creditsOriginal.bytes,creditsOriginal.sha),
            creditsOriginal,561232);
        const AudioClip50HD clip{pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,0,0};
        need(preparedClipMatches(clip,pcm,creditsOriginal,2),"Original Credits Song12 PCM metadata differs");
        creditsPcm_=std::move(pcm);
        creditsClip_={creditsPcm_.samples.data(),creditsPcm_.frames,creditsPcm_.sampleRate,creditsPcm_.channels,0,0};
        creditsPrepared_=true;noteControlChange();error.clear();return true;
    }catch(const std::exception& e){error=e.what();}
    catch(...){error="Credits music cold preparation failed";}
    return false;
}
bool OriginalUiMainAudio50KG::prepareHallOfFameMenu(const std::filesystem::path& audioRoot,
    std::string& error) noexcept {
    if(hallOfFameAttempted_){error="Hall of Fame music needs a fresh cold preparation";return false;}
    hallOfFameAttempted_=true;
    try {
        need(current()&&!paused_&&!ownsMusic_&&!mixer_->mixedFrames(),
            "Hall of Fame music requires the prepared cold frontend transport");
        for(std::size_t i=0;i<mixer_->count();++i)
            need(!mixer_->active(i),"Hall of Fame music preparation cannot alter live playback");
        // Both original authoring files establish Song5's exact identity;
        // never borrow Main/Rider/Course PCM or accept a filename substitute.
        authenticatedRead(audioRoot,"UI_MUSIC.JUK",1517,"4b814f8e4fec2fbcfa25807d5e147ecc164dbb0b0cb9ce5d064e2f3e3c89de98");
        authenticatedRead(audioRoot,"UI_MUSIC.JUB",3623,"f97a5cbceec8056dfc9b277501087aaa54d86a0ccdc0c71cfde73522a048c661");
        auto pcm=decodeMenuStereo(authenticatedRead(audioRoot,hallOfFameOriginal.path,hallOfFameOriginal.bytes,hallOfFameOriginal.sha),
            hallOfFameOriginal,353744);
        const AudioClip50HD clip{pcm.samples.data(),pcm.frames,pcm.sampleRate,pcm.channels,0,0};
        need(preparedClipMatches(clip,pcm,hallOfFameOriginal,2),"Original Hall of Fame Song5 PCM metadata differs");
        hallOfFamePcm_=std::move(pcm);
        hallOfFameClip_={hallOfFamePcm_.samples.data(),hallOfFamePcm_.frames,hallOfFamePcm_.sampleRate,hallOfFamePcm_.channels,0,0};
        hallOfFamePrepared_=true;noteControlChange();error.clear();return true;
    }catch(const std::exception& e){error=e.what();}
    catch(...){error="Hall of Fame music cold preparation failed";}
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
    return riderMenuReady()&&coursePrepared_&&courseMixerShape()&&
        preparedClipMatches(courseClip_,coursePcm_,courseOriginal,2);
}
bool OriginalUiMainAudio50KG::garageGroupCurrent() const noexcept {
    if(!garagePrepared_||!mixer_||!garageAudio_.ready())return false;
    if(ownsMusic_&&musicSong_==6){
        AudioVoiceObservation50HD music;
        if(!mixer_->inspectVoice(musicVoice,music)||!music.voice.active||music.groupKnown||
           music.groupGain!=1.f||music.groupPaused||music.playbackRevision!=musicPlaybackRevision_||
           !sameClip(music.voice.clip,garageClip_)||music.voice.gain!=musicGain_||
           music.voice.pitch!=OriginalUiGarageWattoAudio50KG::nativeRendererPitch||
           music.voice.pan!=0.f||!music.voice.loop||music.originalLeft!=1.f||music.originalRight!=1.f||
           !std::isfinite(music.voice.sourceFrame)||music.voice.sourceFrame<0.0||
           music.voice.sourceFrame>=static_cast<double>(garageClip_.frames))return false;
    }
    return garageScenePrepared_?(mixerCount_==garageSceneRequiredVoices&&garageDroidAudio_.transportValid()):
        mixerCount_==garageRequiredVoices;
}
bool OriginalUiMainAudio50KG::courseMixerShape() const noexcept {
    return mixer_&&(mixer_->count()==courseRequiredVoices||retainedGarageOrdinaryCurrent()||garageGroupCurrent());
}
bool OriginalUiMainAudio50KG::garageSceneMenuReady() const noexcept {
    return garageMenuReady()&&garageScenePrepared_&&garageDroidAudio_.ready();
}
OriginalUiGarageDroidAudio50GN* OriginalUiMainAudio50KG::garageDroidAudio() noexcept {
    return garageSceneMenuReady()?&garageDroidAudio_:nullptr;
}
const OriginalUiGarageDroidAudio50GN* OriginalUiMainAudio50KG::garageDroidAudio() const noexcept {
    return garageSceneMenuReady()?&garageDroidAudio_:nullptr;
}
bool OriginalUiMainAudio50KG::nativeDroidSampleIdentity(const OriginalUiGarageDroidAudio50GN* audio,
 const AudioMixer50HD* mixer,std::uint64_t epoch) const noexcept {
    return prepared_&&(garageColdPending_||garageScenePrepared_)&&audio==&garageDroidAudio_&&
        mixer_&&mixer==mixer_&&epoch==mixerEpoch_&&mixer_->groupEpoch50IC()==mixerEpoch_&&
        mixer_->count()==garageSceneRequiredVoices&&mixer_->count()==mixerCount_&&
        mixer_->sampleRate()==mixerRate_&&mixer_->originalSoundMode50KG()==preparedSoundMode_&&
        !mixer_->requiresVoiceGroupGains50IC();
}
const OriginalUiGarageDroidAudio50GN* OriginalUiMainAudio50KG::nativeDroidSampleTransport() const noexcept {
    if(!nativeDroidSampleIdentity(&garageDroidAudio_,mixer_,mixerEpoch_))return nullptr;
    return garageDroidAudio_.transportValid()?&garageDroidAudio_:nullptr;
}
bool OriginalUiMainAudio50KG::nativeWattoSampleIdentity(const OriginalUiGarageWattoAudio50KG* audio,
 const AudioMixer50HD* mixer,std::uint64_t epoch) const noexcept {
    // Private structural transport half. The actual Audio::current caller
    // checks its own prepared/poisoned/routing/voice state, without recursion.
    return prepared_&&(garageColdPending_||garagePrepared_)&&audio==&garageAudio_&&
        mixer_&&mixer==mixer_&&epoch==mixerEpoch_&&mixer_->groupEpoch50IC()==mixerEpoch_&&
        mixer_->count()==mixerCount_&&!mixer_->requiresVoiceGroupGains50IC();
}
const OriginalUiGarageWattoAudio50KG* OriginalUiMainAudio50KG::nativeWattoSampleTransport() const noexcept {
    // The reached sample-loading phase precedes reverb/playback completion.
    // Validate the actual retained native owner and backend, not the aggregate
    // Garage-ready predicate (which intentionally closes during construction).
    if(!nativeWattoSampleIdentity(&garageAudio_,mixer_,mixerEpoch_))return nullptr;
    OriginalUiGarageWattoAudio50KG::Observation observed;
    return garageAudio_.inspect(observed)&&observed.mixer==mixer_&&
        observed.mixerEpoch==mixerEpoch_?&garageAudio_:nullptr;
}
OriginalUiGarageWattoAudio50KG* OriginalUiMainAudio50KG::garageWattoAudioCold() noexcept {
    if(garageSourceLifetime_!=GarageSourceLifetime::unbound&&
       (garageSourceLifetime_!=GarageSourceLifetime::constructing||
        !retainedGarageEntryCurrent()||!retainedGarageOrdinaryCurrent()))return nullptr;
    return nativeWattoSampleTransport()==&garageAudio_?&garageAudio_:nullptr;
}
bool OriginalUiMainAudio50KG::sceneGroupIdentityCurrent(const OriginalUiGaragePodScene50KG& scene)const noexcept {
    // PodScene alone authenticates its private source registry/grant. This
    // structural half deliberately avoids backend.current() during the real
    // atomic unresolved-Sound -> native logical-row publication interval.
    return std::this_thread::get_id()==nativeControlThread_&&(scene.coldRoutingIdentityCurrent(this)||scene.retiredSoundGroupRoutingIdentity(this))&&
        scene.backend_==&garageDroidAudio_&&nativeDroidSampleIdentity(scene.backend_,mixer_,mixerEpoch_);
}
bool OriginalUiMainAudio50KG::sceneRoutingHistoryCurrent(const OriginalUiGaragePodScene50KG& scene)const noexcept {
    if(scene.retiredSoundGroupRoutingIdentity(this))
        return nativeDroidSampleTransport()==scene.backend_&&garageDroidAudio_.retiredSceneTransportCurrent();
    if(!sceneGroupIdentityCurrent(scene)||nativeDroidSampleTransport()!=scene.backend_||
       !garageDroidAudio_.sourceConstructionComplete()||garageDroidAudio_.retired_||garageDroidAudio_.paused_)return false;
    for(std::size_t i=0;i<OriginalUiGarageDroidAudio50GN::soundCount;++i){
        const auto& row=garageDroidAudio_.rows_[i];const auto& source=*scene.group4D8_.sounds_[i];
        if(row.played108||row.playbackRevision||source.channel2C!=-1||
           mixer_->playbackRevision(OriginalUiGarageDroidAudio50GN::firstVoice+i)||
           mixer_->active(OriginalUiGarageDroidAudio50GN::firstVoice+i))return false;
        if(row.sound&&(row.sound->channel2C!=-1||row.sound->elapsedBits24||row.sound->word7C))return false;
    }
    return true;
}
bool OriginalUiMainAudio50KG::queuedGroupIdentityCurrent(const QueuedGroupControl& c)const noexcept {
    if(std::this_thread::get_id()!=nativeControlThread_||!c.grant||!c.sequence||!c.entity||c.slot<0||c.slot>=25||
       c.command!=25||c.arguments!=std::array<std::uint32_t,3>{static_cast<std::uint32_t>(c.slot),0u,1u}||
       c.objectKey!=c.entity->objectKey||c.generation!=c.entity->sessionGeneration||
       c.mixer!=mixer_||c.epoch!=mixerEpoch_||c.voiceCount!=mixerCount_||c.outputRate!=mixerRate_||
       c.soundMode!=preparedSoundMode_||!mixer_||mixer_->sampleRate()!=c.outputRate||
       mixer_->groupEpoch50IC()!=c.epoch||mixer_->count()!=c.voiceCount||
       mixer_->originalSoundMode50KG()!=c.soundMode||!queuedGroupProcessorCurrent(c))return false;
    const auto main=c.main.lock();if(!main||main.get()!=this)return false;
    if(c.kind==NativeGroupControlKind::scene){
        if(c.group||!c.scene||c.voiceCount!=garageSceneRequiredVoices||
           !sceneGroupIdentityCurrent(*c.scene)||c.entity!=&c.scene->group4D8_.entity||
           c.slot!=c.scene->group4D8_.slot40_||c.grant!=c.scene->grant_||
           c.grant.owner_before(c.scene->grant_)||c.scene->grant_.owner_before(c.grant)||
           main!=c.scene->main_||main.owner_before(c.scene->main_)||c.scene->main_.owner_before(main))return false;
        const auto issued=c.scene->nativeRoutingLifetime_.lock();
        return issued==c.scene&&!issued.owner_before(c.scene)&&!c.scene.owner_before(issued);
    }
    if(!c.group||c.scene||c.entity!=&c.group->entity_||
       !c.group->queuedIdentityCurrent(c.grant,c.generation,c.slot,this)||
       nativeWattoSampleTransport()!=c.group->audio_)return false;
    const auto self=c.group->weak_from_this().lock();
    return main==c.group->main_&&!main.owner_before(c.group->main_)&&!c.group->main_.owner_before(main)&&
        self==c.group&&!self.owner_before(c.group)&&!c.group.owner_before(self);
}
bool OriginalUiMainAudio50KG::queuedGroupRoutingCurrent(const QueuedGroupControl& c)const noexcept {
    if(!mixer_||!queuedGroupProcessorCurrent(c))return false;
    if(c.routingApplied&&c.wetProcessor)return c.kind==NativeGroupControlKind::scene?
        mixer_->nativeSceneRoutingLiveCurrent(&c):mixer_->nativeWattoRoutingLiveCurrent(&c);
    if(!c.routingApplied&&c.wetProcessor&&c.kind==NativeGroupControlKind::scene)
        return mixer_->nativeSceneImmediateRoutingCurrent(&c);
    if(c.retainedHistory)return c.kind==NativeGroupControlKind::scene?
        mixer_->nativeRetainedSceneRoutingCurrent(&c,c.routingApplied):mixer_->nativeRetainedWattoRoutingCurrent(&c,c.routingApplied);
    return c.kind==NativeGroupControlKind::scene?mixer_->nativeSceneRoutingCurrent(&c,c.routingApplied):
        mixer_->nativeWattoRoutingCurrent(&c,c.routingApplied);
}
bool OriginalUiMainAudio50KG::wattoConstructionHistoryCurrent(const OriginalUiGarageWattoSoundGroup50KG& group)const noexcept {
    if(!nativeWetInterfaceCurrent()||group.audio_!=&garageAudio_||!group.routingSourceCurrent(false)||
       !garageAudio_.stoppedInstancesCurrent())return false;
    for(const auto& s:garageAudio_.sounds_)if(!s.source||s.source->channel2C!=-1||s.source->elapsedBits24||
        s.source->word7C||s.latest!=OriginalUiGarageWattoAudio50KG::instanceCount||s.latestRevision||s.latestToken!=-1)return false;
    return true;
}
bool OriginalUiMainAudio50KG::sceneConstructionHistoryCurrent(const OriginalUiGaragePodScene50KG& scene)const noexcept {
    if(!nativeWetInterfaceCurrent()||!sceneGroupIdentityCurrent(scene)||scene.backend_!=&garageDroidAudio_||
       !garageDroidAudio_.sourceConstructionComplete()||!garageDroidAudio_.stoppedVoiceHistoryCurrent())return false;
    for(std::size_t i=0;i<OriginalUiGarageDroidAudio50GN::soundCount;++i){const auto& row=garageDroidAudio_.rows_[i];
        const auto& source=*scene.group4D8_.sounds_[i];
        if(row.played108||row.playbackRevision||source.channel2C!=-1)return false;
        if(row.sound&&(row.sound->channel2C!=-1||row.sound->elapsedBits24||row.sound->word7C))return false;
    }
    return true;
}
bool OriginalUiMainAudio50KG::queuedGroupHistoryCurrent(const QueuedGroupControl& c)const noexcept {
    if(c.retainedHistory)return c.kind==NativeGroupControlKind::scene?
        (c.scene&&sceneConstructionHistoryCurrent(*c.scene)):(c.group&&wattoConstructionHistoryCurrent(*c.group));
    return c.kind==NativeGroupControlKind::scene?(c.scene&&sceneRoutingHistoryCurrent(*c.scene)):
        (c.group&&c.group->routingHistoryCurrent());
}
bool OriginalUiMainAudio50KG::queuedRetiredSceneLifetimeCurrent(const QueuedGroupControl& c)const noexcept {
    if(c.kind!=NativeGroupControlKind::scene||!c.scene||c.group||!c.routingApplied||!c.wetProcessor||
       !nativeWetInterfaceCurrent()||!queuedGroupProcessorCurrent(c)||!queuedGroupRoutingCurrent(c)||
       !c.sequence||c.sequence>nativeGroupControlSequence_||c.command!=25||c.slot<0||c.slot>=25||
       c.arguments!=std::array<std::uint32_t,3>{static_cast<std::uint32_t>(c.slot),0u,1u})return false;
    const auto& scene=*c.scene;const auto& g=scene.group4D8_;
    const auto issued=scene.nativeRoutingLifetime_.lock();const auto main=c.main.lock();
    if(!issued||issued!=c.scene||issued.owner_before(c.scene)||c.scene.owner_before(issued)||
       !main||main.get()!=this||main!=scene.main_||main.owner_before(scene.main_)||scene.main_.owner_before(main)||
       !c.grant||c.grant!=scene.grant_||c.grant.owner_before(scene.grant_)||scene.grant_.owner_before(c.grant)||
       c.entity!=&g.entity||c.slot!=g.slot40_||c.generation!=g.entity.sessionGeneration||
       c.objectKey!=g.entity.objectKey||c.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&g.entity))||
       c.mixer!=mixer_||c.epoch!=mixerEpoch_||c.voiceCount!=mixerCount_||c.outputRate!=mixerRate_||c.soundMode!=preparedSoundMode_||
       scene.groupLive_||scene.soundGroupRetirement_!=OriginalUiGaragePodScene50KG::SoundGroupRetirement::complete||
       scene.retirementStage_<OriginalUiGaragePodScene50KG::RetirementStage::groupReleased||
       scene.backend_!=&garageDroidAudio_||g.manager5C!=this||g.entity.reference00||g.count48_||g.members4C_||g.capacity54_||
       scene.pendingWaves_||garageDroidAudio_.sourceSceneGroup_||!garageDroidAudio_.sourceSceneGroupReleased_)return false;
    for(const auto& sound:g.sounds_)if(sound)return false;
    // Grant's manager may now be retired. This same strong Page alias and the
    // actual completed source destructor retain ownership, not a stale live
    // grant lookup. Only the explicit Page/manager proof may release custody.
    return garageDroidAudio_.current()&&garageDroidAudio_.retiredSceneTransportCurrent();
}
bool OriginalUiMainAudio50KG::queuedSceneImmediateLifetimeCurrent(const QueuedGroupControl& c)const noexcept {
    // Distinct retained source custody while Scene25 is still queued. The
    // original Droid233410(flags2) issues RPC8 immediately, without266A10.
    // All source rows/handles and played PCM leases remain independently live;
    // this neither applies25 nor authenticates a normal retired Scene.
    if(c.kind!=NativeGroupControlKind::scene||!c.scene||c.group||c.routingApplied||
       !c.wetProcessor||!nativeWetInterfaceCurrent()||!queuedGroupIdentityCurrent(c)||
       !queuedGroupRoutingCurrent(c)||c.scene->backend_!=&garageDroidAudio_||
       garageDroidAudio_.sourceSceneGroup_!=c.scene.get()||
       garageDroidAudio_.sourceSceneGroupReleased_||!garageDroidAudio_.resolvedRowsCurrent())return false;
    for(std::size_t n=0;n<OriginalUiGarageDroidAudio50GN::soundCount;++n){
        const auto slot=OriginalUiGarageDroidAudio50GN::firstVoice+n;
        if((mixer_->active(slot)||mixer_->nativeSourceVoiceRoutes_[slot].count)&&
           !mixer_->nativeSceneSampleVoiceCurrent(slot))return false;
    }
    return true;
}
bool OriginalUiMainAudio50KG::queuedGroupSourceLifetimeCurrent(const QueuedGroupControl& c)const noexcept {
    // This is a live source/PCM custody proof, not zero playback history and
    // not permission to manufacture a source play or its expiration service.
    if(c.kind==NativeGroupControlKind::scene&&garageDroidAudio_.sourceSceneGroupReleased_)
        return queuedRetiredSceneLifetimeCurrent(c);
    if(!c.routingApplied||!c.wetProcessor||!nativeWetInterfaceCurrent()||
       !queuedGroupIdentityCurrent(c)||!queuedGroupRoutingCurrent(c))return false;
    if(c.kind==NativeGroupControlKind::watto)
        return c.group&&c.group->audio_==&garageAudio_&&c.group->routingSourceCurrent(false);
    return c.scene&&c.scene->backend_==&garageDroidAudio_&&garageDroidAudio_.current()&&
        (c.scene->retiredSoundGroupRoutingIdentity(this)||garageDroidAudio_.sourceConstructionComplete());
}
bool OriginalUiMainAudio50KG::nativeWattoWetLifetimeCurrent(const OriginalUiGarageWattoSoundGroup50KG& group)const noexcept {
    for(std::size_t n=0;n<nativeGroupControlCount_;++n){const auto& c=nativeGroupControls_[n];
        if(c&&c->kind==NativeGroupControlKind::watto&&c->group.get()==&group)
            return queuedGroupSourceLifetimeCurrent(*c);
    }
    return false;
}
bool OriginalUiMainAudio50KG::publishNativeWattoSample(OriginalUiGarageWattoAudio50KG& audio,
    std::size_t sound,std::size_t slot,const AudioClip50HD& clip)noexcept {
    if(&audio!=&garageAudio_||!nativeWetInterfaceCurrent()||!audio.sourceGroup_||
       slot<audio.firstVoice||slot-audio.firstVoice>=audio.instanceCount)return false;
    AudioMixer50HD::NativeSourceSampleStart50KG source;
    if(!audio.sourceSampleStartCurrent(sound,clip,source))return false;
    const auto& actual=*audio.sounds_[sound].source;
    for(std::size_t i=0;i<nativeGroupControlCount_;++i){const auto& c=nativeGroupControls_[i];
        if(c&&c->kind==NativeGroupControlKind::watto&&c->group.get()==audio.sourceGroup_){
            if(!queuedGroupSourceLifetimeCurrent(*c))return false;
            return mixer_->playNativeSourceSampleVolume(c.get(),slot,clip,actual.volume38,
                OriginalUiGarageWattoAudio50KG::nativeRendererPitch,actual.pan3C,source);
        }
    }
    return false;
}
const assets::OriginalStreamMetadataRow50KZ* OriginalUiMainAudio50KG::nativeWattoStreamSourceCurrent(
    std::size_t sound,const AudioClip50HD& clip)const noexcept {
    const auto& audio=garageAudio_;
    if(!prepared_||!nativeSourceMetadata_||sound>=audio.responseCount||!audio.prepared_||
       audio.mixer_!=mixer_||audio.epoch_!=mixerEpoch_||audio.metadata_!=nativeSourceMetadata_||
       audio.metadata_.owner_before(nativeSourceMetadata_)||nativeSourceMetadata_.owner_before(audio.metadata_))return nullptr;
    const auto& retained=audio.sounds_[sound];const auto* row=retained.metadata;
    if(!row||nativeSourceMetadata_->row(row->sourceIndex)!=row||!sameClip(retained.clip,clip)||
       clip.samples!=retained.pcm.samples.data()||clip.frames!=retained.pcm.frames||
       clip.sampleRate!=22050||clip.channels!=1||!AudioMixer50HD::validClip(clip)||
       row->originalPitch!=1881||row->originalSampleRate!=22050||
       !std::isfinite(row->admittedDurationSeconds)||row->admittedDurationSeconds<=0.f)return nullptr;
    // The original cold prepare authenticated every file and exact registry
    // row; immutable native PCM and metadata survive source Group destruction.
    return row;
}
bool OriginalUiMainAudio50KG::nativeWattoStreamStartCurrent(std::size_t sound,std::size_t slot,
    const AudioClip50HD& clip,const void* command)const noexcept {
    const auto* row=nativeWattoStreamSourceCurrent(sound,clip);const auto& audio=garageAudio_;
    if(!row||!nativeWetInterfaceCurrent()||!audio.sourceGroup_||audio.paused_||
       slot<audio.firstVoice||slot-audio.firstVoice>=audio.instanceCount||!audio.canRevise(1)||
       audio.nextToken_==std::numeric_limits<std::int32_t>::max())return false;
    const auto& retained=audio.sounds_[sound];
    if(!audio.sourceSoundCurrent(retained)||!retained.source)return false;
    const auto& source=*retained.source;
    if(source.flags10!=2||!source.stream1A8||source.stereo1AC||source.loop1B0||
       source.nativeSample28||source.resourceA0||source.volume38!=100||source.pan3C||
       source.basePitch50!=1881||source.pitch40!=1881||source.pitch4C!=4096||
       std::bit_cast<std::uint32_t>(source.duration20)!=std::bit_cast<std::uint32_t>(row->admittedDurationSeconds))return false;
    for(std::size_t n=0;n<nativeGroupControlCount_;++n){const auto& c=nativeGroupControls_[n];
        if(c&&c.get()==command&&c->kind==NativeGroupControlKind::watto&&c->group.get()==audio.sourceGroup_)
            return queuedGroupSourceLifetimeCurrent(*c);
    }
    return false;
}
bool OriginalUiMainAudio50KG::publishNativeWattoStream(OriginalUiGarageWattoAudio50KG& audio,
    std::size_t sound,std::size_t slot,const AudioClip50HD& clip)noexcept {
    if(&audio!=&garageAudio_||!nativeWetOwner_||!nativeWetPlayWatto_)return false;
    for(std::size_t n=0;n<nativeGroupControlCount_;++n){const auto& c=nativeGroupControls_[n];
        if(c&&nativeWattoStreamStartCurrent(sound,slot,clip,c.get()))
            return (nativeWetOwner_->*nativeWetPlayWatto_)(*this,c.get(),sound,slot,clip,100,
                OriginalUiGarageWattoAudio50KG::nativeRendererPitch,0);
    }
    return false;
}
bool OriginalUiMainAudio50KG::publishNativeSceneSample(OriginalUiGarageDroidAudio50GN& audio,
    std::size_t ordinal,const AudioClip50HD& clip)noexcept {
    if(&audio!=&garageDroidAudio_||!nativeSceneImmediatePlaybackCurrent(audio)||
       !audio.sourceSceneGroup_||ordinal>=audio.soundCount)return false;
    AudioMixer50HD::NativeSourceSampleStart50KG source;
    if(!audio.sourceSampleStartCurrent(ordinal,clip,source))return false;
    const auto* actual=audio.rows_[ordinal].sound;if(!actual)return false;
    for(std::size_t i=0;i<nativeGroupControlCount_;++i){const auto& c=nativeGroupControls_[i];
        if(c&&c->kind==NativeGroupControlKind::scene&&c->scene.get()==audio.sourceSceneGroup_){
            if(!(c->routingApplied?queuedGroupSourceLifetimeCurrent(*c):queuedSceneImmediateLifetimeCurrent(*c)))return false;
            return mixer_->playNativeSceneImmediateSampleVolume(c.get(),audio.firstVoice+ordinal,clip,
                actual->volume38,OriginalUiGarageDroidAudio50GN::sourceRendererPitch(),actual->angle3C,source);
        }
    }
    return false;
}
bool OriginalUiMainAudio50KG::nativeSceneWetLifetimeCurrent(const OriginalUiGaragePodScene50KG& scene)const noexcept {
    for(std::size_t n=0;n<nativeGroupControlCount_;++n){const auto& c=nativeGroupControls_[n];
        if(c&&c->kind==NativeGroupControlKind::scene&&c->scene.get()==&scene)
            return queuedGroupSourceLifetimeCurrent(*c);
    }
    return false;
}
bool OriginalUiMainAudio50KG::liveGarageVoiceRangesCurrent()const noexcept {
    if(!nativeWetInterfaceCurrent()||mixerCount_!=garageSceneRequiredVoices)return false;
    bool watto=false,scene=false;
    for(std::size_t n=0;n<nativeGroupControlCount_;++n){const auto& c=nativeGroupControls_[n];
        if(!c||!(queuedGroupSourceLifetimeCurrent(*c)||queuedSceneImmediateLifetimeCurrent(*c)))return false;
        if(c->kind==NativeGroupControlKind::watto){if(watto)return false;watto=true;}
        else {if(scene)return false;scene=true;}
    }
    // No claim means stopped PCM still needs its exact retained owner. EOF
    // alone does not discharge a source reservation or release a clip lease.
    return (watto||garageAudio_.stoppedInstancesCurrent())&&
        (scene||garageDroidAudio_.stoppedVoiceHistoryCurrent());
}
bool OriginalUiMainAudio50KG::ownedStoppedGarageClipHistoryCurrent()const noexcept {
    // This also remains a valid ownership proof after actual Aural interface
    // retirement, which first proved no active/reserved routes or group claims.
    // It is NOT a substitute processor or permission to resume audio output.
    if((!nativeWetInterfaceCurrent()&&!nativeWetInterfaceRetired_)||nativeGroupControlCount_||
       !mixer_||mixer_->groupEpoch50IC()!=mixerEpoch_||mixer_->count()!=garageSceneRequiredVoices||
       garageAudio_.sourceGroup_||garageAudio_.sourceGroupAttempted_||garageDroidAudio_.sourceSceneGroup_)
        return false;
    return garageAudio_.stoppedInstancesCurrent()&&garageDroidAudio_.stoppedVoiceHistoryCurrent();
}
bool OriginalUiMainAudio50KG::nativeSourcePlaybackCurrent(std::size_t maximumMonoPlays)const noexcept {
    return nativeWetInterfaceCurrent()&&nativeWetCanPublish_&&
        (nativeWetOwner_->*nativeWetCanPublish_)(*this,maximumMonoPlays);
}
bool OriginalUiMainAudio50KG::nativeWattoPlaybackCurrent(const OriginalUiGarageWattoAudio50KG& audio)const noexcept {
    if(&audio!=&garageAudio_||!audio.sourceGroup_||!nativeSourcePlaybackCurrent()||
       !nativeWattoWetLifetimeCurrent(*audio.sourceGroup_))return false;
    const QueuedGroupControl* command=nullptr;
    for(std::size_t n=0;n<nativeGroupControlCount_;++n){const auto& c=nativeGroupControls_[n];
        if(c&&c->kind==NativeGroupControlKind::watto&&c->group.get()==audio.sourceGroup_)command=c.get();}
    if(!command)return false;
    for(std::size_t n=0;n<audio.responseCount;++n)
        if(!nativeWattoStreamStartCurrent(n,audio.firstVoice,audio.sounds_[n].clip,command))return false;
    const auto& wing=audio.sounds_[audio.responseCount];AudioClip50HD clip;
    AudioMixer50HD::NativeSourceSampleStart50KG sample;
    return wing.source&&wing.source->nativeSample28&&
        wing.nativeSample.clipForResolved(&audio,*wing.source->nativeSample28,clip)&&
        audio.sourceSampleStartCurrent(audio.responseCount,clip,sample);
}
bool OriginalUiMainAudio50KG::nativeSceneImmediatePlaybackCurrent(const OriginalUiGarageDroidAudio50GN& audio)const noexcept {
    if(&audio!=&garageDroidAudio_||!audio.sourceSceneGroup_||!nativeSourcePlaybackCurrent()||
       !audio.resolvedRowsCurrent())return false;
    bool owned=false;
    for(std::size_t n=0;n<nativeGroupControlCount_;++n){const auto& c=nativeGroupControls_[n];
        if(c&&c->kind==NativeGroupControlKind::scene&&c->scene.get()==audio.sourceSceneGroup_){
            if(owned||!(c->routingApplied?queuedGroupSourceLifetimeCurrent(*c):queuedSceneImmediateLifetimeCurrent(*c)))return false;
            owned=true;
        }
    }
    if(!owned)return false;
    for(std::size_t n=0;n<audio.soundCount;++n){const auto& row=audio.rows_[n];
        if(!row.played108){AudioMixer50HD::NativeSourceSampleStart50KG sample;
            if(!row.sound||!audio.sourceSampleStartCurrent(n,row.sound->clip,sample))return false;}
    }
    return true;
}
bool OriginalUiMainAudio50KG::nativeScenePlaybackCurrent(const OriginalUiGarageDroidAudio50GN& audio)const noexcept {
    if(&audio!=&garageDroidAudio_||!audio.sourceSceneGroup_||!nativeSourcePlaybackCurrent()||
       !audio.resolvedRowsCurrent()||!nativeSceneWetLifetimeCurrent(*audio.sourceSceneGroup_))return false;
    for(std::size_t n=0;n<audio.soundCount;++n){const auto& row=audio.rows_[n];
        // current() already proves exact retained live/played leases. Only a
        // not-yet-played cue must still satisfy its actual first-start fields.
        if(!row.played108){AudioMixer50HD::NativeSourceSampleStart50KG sample;
            if(!row.sound||!audio.sourceSampleStartCurrent(n,row.sound->clip,sample))return false;}
    }
    return true;
}
bool OriginalUiMainAudio50KG::nativeWetInterfaceCurrent()const noexcept {
    return std::this_thread::get_id()==nativeControlThread_&&!nativeWetInterfaceRetired_&&nativeWetOwner_&&nativeWetProcessor_&&nativeWetSetRawMasters_&&nativeWetPlaySong_&&nativeWetPlayEffect_&&nativeWetPlayRider_&&nativeWetPlayWatto_&&nativeWetCanPublish_&&
        prepared_&&mixer_&&nativeWetEpoch_==mixerEpoch_&&nativeWetEpoch_==mixer_->groupEpoch50IC()&&
        mixerCount_==mixer_->count()&&mixerRate_==mixer_->sampleRate()&&mixerRate_==48000&&
        mixer_->nativeSourceWetProcessorCurrent(*nativeWetOwner_,*nativeWetProcessor_);
}
bool OriginalUiMainAudio50KG::queuedGroupProcessorCurrent(const QueuedGroupControl& c)const noexcept {
    if(!mixer_||c.acceptedFrame>mixer_->mixedFrames()||
       (c.routingApplied?(c.routingFrame<c.acceptedFrame||c.routingFrame>mixer_->mixedFrames()):c.routingFrame!=0))return false;
    // Earlier metadata-only cold clients remain metadata-only. They cannot
    // acquire a processor by swapping pointers or upgrading a queued receipt.
    if(!c.wetOwner&&!c.wetProcessor)return !nativeWetOwner_&&!nativeWetProcessor_&&!nativeWetEpoch_&&!nativeWetSetRawMasters_&&!nativeWetPlaySong_&&!nativeWetPlayEffect_&&!nativeWetPlayRider_&&!nativeWetPlayWatto_&&!nativeWetCanPublish_;
    return c.wetOwner==nativeWetOwner_&&c.wetProcessor==nativeWetProcessor_&&nativeWetInterfaceCurrent();
}
bool OriginalUiMainAudio50KG::nativeGaragePageControlCustodyCurrent(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    // The native envelope precedes this Root's dispatcher and267BE0. The
    // actual final Scene sample resolution does not submit queued command25.
    // Validate retained source/PCM/control custody without pretending it ran.
    if(!current()||!retainedGarageEntryCurrent()||!page.inputsCurrent()||
       nativeGroupControlCount_!=2u||!garageAudio_.sourceGroup_||
       !garageAudio_.sourceGroup_->sourceSoundsComplete()||
       !garageDroidAudio_.sourceSceneGroup_||!garageDroidAudio_.resolvedRowsCurrent()||
       !nativeSongSourceCurrent(6u,garageClip_)||!page.entityLive_||page.entityRefReleased_||
       page.entityCountReleased_||!page.managerAttached1D1C40_||
       page.entryPhase_!=OriginalUiGaragePageOwner50KG::EntryPhase::returned||
       page.scene4C0_!=garageDroidAudio_.sourceSceneGroup_||page.sceneBacking_.get()!=page.scene4C0_||
       !nativeWetInterfaceCurrent()||!precedingGroupControlsCurrent())return false;
    // precedingGroupControlsCurrent proves order, uniqueness, exact claims,
    // epoch/history and queued-vs-applied state. Bind both rows to THIS entry.
    for(std::size_t n=0;n<nativeGroupControlCount_;++n){const auto& c=nativeGroupControls_[n];
        if(c->kind==NativeGroupControlKind::watto){
            if(c->group.get()!=garageAudio_.sourceGroup_)return false;
        }else if(c->scene.get()!=garageDroidAudio_.sourceSceneGroup_)return false;
    }
    const auto entry=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    return entry&&backing&&entry->page_.get()==&page&&page.issuer_==backing->issuer_&&
        page.entity_.sessionGeneration==garageEntryGeneration_&&entry->audio_.get()==this&&
        !entry->audio_.owner_before(backing)&&!backing.owner_before(entry->audio_);
}
bool OriginalUiMainAudio50KG::nativeGaragePageAudioCurrent(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    // The genuine manager owns constructing-state source rows throughout this
    // path. Optional finishGarageAudioBackingCold is not a retail predecessor.
    // Prove the actual resolved Watto/Scene owners and applied DSP claims, not
    // a synthetic transition to the historical garageMenuReady state.
    if(!current()||!retainedGarageEntryCurrent()||!page.inputsCurrent()||
       nativeGroupControlCount_!=2u||!nativeWattoPlaybackCurrent(garageAudio_)||
       !nativeScenePlaybackCurrent(garageDroidAudio_)||!nativeSongSourceCurrent(6u,garageClip_)||
       !page.entityLive_||page.entityRefReleased_||page.entityCountReleased_||
       !page.managerAttached1D1C40_||
       page.entryPhase_!=OriginalUiGaragePageOwner50KG::EntryPhase::returned||
       !nativeWetInterfaceCurrent()||pendingNativeGroupControls())return false;
    const auto entry=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    return entry&&backing&&entry->page_.get()==&page&&page.issuer_==backing->issuer_&&
        page.entity_.sessionGeneration==garageEntryGeneration_&&entry->audio_.get()==this&&
        !entry->audio_.owner_before(backing)&&!backing.owner_before(entry->audio_);
}
bool OriginalUiMainAudio50KG::nativeGaragePageInputAudioCurrent(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    return nativeGaragePageAudioCurrent(page)&&page.base_.input268;
}
bool OriginalUiMainAudio50KG::nativeGaragePageSong6StartCurrent(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    const std::uint64_t changes=ownsMusic_?2u:1u; //234440(0), then234170.
    if(!nativeGaragePageAudioCurrent(page)||paused_||!canRecordControlChange(changes)||
       !mixer_->canRecordControlChanges(changes)||!nativeSourcePlaybackCurrent(2u)||
       !nativeSongSourceCurrent(6u,garageClip_))return false;
    AudioVoiceObservation50HD voice;
    return mixer_->inspectVoice(musicVoice,voice)&&!voice.groupKnown&&!voice.groupPaused&&
        voice.groupGain==1.f&&voice.playbackRevision==musicPlaybackRevision_&&
        voice.voice.active==ownsMusic_;
}
bool OriginalUiMainAudio50KG::canStartGaragePageSong6_234900(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    // Before26A820(master25): master, optional active234440(0), then234170.
    // Native stopped streams remain issued; stereo Song6 needs two OTHER free
    // Aural slots. Neither preview aging nor a speculative stop frees them.
    const std::uint64_t changes=ownsMusic_?3u:2u;
    return nativeGaragePageSong6StartCurrent(page)&&canRecordControlChange(changes)&&
        mixer_->canRecordControlChanges(changes);
}
bool OriginalUiMainAudio50KG::startGaragePageSong6_234900(
    const OriginalUiGaragePageOwner50KG& page)noexcept {
    // The exact actual Page master command must already have returned at this
    // sample boundary. No leaf-ready flag, private wet clear or normalized.25.
    if(!nativeGaragePageSong6StartCurrent(page)||!nativeWetMasterCount_||
       nativeWetMasterPercent_!=25u||nativeWetMasterRaw_!=4095u||
       nativeWetMasterFrame_!=mixer_->mixedFrames()||
       nativeWetMasterSequence_!=nativeGroupControlSequence_)return false;
    // Actual234900(restart1) first invokes234440(0.f), even for an inactive
    // handle. Preserve an executed stop if the subsequent start fails. The
    // Page must not replay a failed source callback or fabricate rollback.
    if(!fadeMain(0.f))return false;
    return startMusicLoop(garageClip_,6u,OriginalUiGarageWattoAudio50KG::nativeRendererPitch);
}
bool OriginalUiMainAudio50KG::canFadeGaragePageSong6_234440(
    const OriginalUiGaragePageOwner50KG& page)const noexcept {
    if(!nativeGaragePageAudioCurrent(page)||paused_||
       !canRecordControlChange(ownsMusic_?1u:0u))return false;
    AudioVoiceObservation50HD voice;
    if(!mixer_->inspectVoice(musicVoice,voice)||voice.groupKnown||voice.groupPaused||
       voice.groupGain!=1.f||voice.playbackRevision!=musicPlaybackRevision_||
       voice.voice.active!=ownsMusic_)return false;
    //234480 returns without effects for the actual inactive stream. A true
    // native result means handled, not a fabricated original return1.
    return !ownsMusic_||(musicSong_==6u&&nativeSongSourceCurrent(6u,voice.voice.clip));
}
bool OriginalUiMainAudio50KG::fadeGaragePageSong6_234440(
    const OriginalUiGaragePageOwner50KG& page)noexcept {
    return canFadeGaragePageSong6_234440(page)&&fadeMain(1.f);
}
bool OriginalUiMainAudio50KG::advanceGarageNativeMusicControl(
    const OriginalUiGaragePageOwner50KG& page,float seconds)noexcept {
    // The sole caller is this Page's retained Root dispatcher cursor, after
    // actual Aural103720 and before current-frame Page/manager commands. Its
    // factor is the SAME capped23C used by ordinary prepareUpdateAudio, not
    // raw250, an elapsed wall interval, or another sample/voice-age advance.
    if(!std::isfinite(seconds)||seconds<0.f||seconds>0.06f||
       !nativeGaragePageControlCustodyCurrent(page))return false;
    if(paused_||!fading_)return true;
    AudioVoiceObservation50HD voice;
    if(!ownsMusic_||!mixer_->inspectVoice(musicVoice,voice)||!voice.voice.active||
       voice.groupKnown||voice.groupPaused||voice.groupGain!=1.f||
       voice.playbackRevision!=musicPlaybackRevision_||
       !nativeSongSourceCurrent(musicSong_,voice.voice.clip)||
       !std::isfinite(fadeElapsed_)||fadeElapsed_<0.f||
       !std::isfinite(fadeStartGain_)||fadeStartGain_<0.f||fadeStartGain_>0.4f||
       fadeDuration_!=1.f||fadeRetireAt_!=1.35f)return false;
    // Preserve the established native one-second envelope and0.35-second
    // silent retirement policy. This is not an invented original callback.
    // Stage once before mutation and reserve the exact controls needed.
    volatile float elapsed=fadeElapsed_+seconds;
    const float nextElapsed=elapsed;
    const float nextGain=fadeStartGain_*std::max(0.f,1.f-nextElapsed/fadeDuration_);
    if(!std::isfinite(nextElapsed)||!std::isfinite(nextGain)||nextGain<0.f||nextGain>0.4f)return false;
    const bool retire=nextElapsed>=fadeRetireAt_;
    const std::uint64_t mixerChanges=(nextGain!=musicGain_?1u:0u)+(retire?1u:0u);
    const std::uint64_t transportChanges=(nextElapsed!=fadeElapsed_||nextGain!=musicGain_||mixerChanges)?1u:0u;
    if(!canRecordControlChange(transportChanges)||!mixer_->canRecordControlChanges(mixerChanges))return false;
    const float previousElapsed=fadeElapsed_,previousGain=musicGain_;
    const auto revision=mixer_->controlRevision();
    fadeElapsed_=nextElapsed;musicGain_=nextGain;
    if(!mixer_->controls(musicVoice,musicGain_,musicPitch_,0.f)){
        if(previousElapsed!=fadeElapsed_||previousGain!=musicGain_)noteControlChange();
        return false; // Root retains this failed prefix; it may not replay it.
    }
    if(retire){
        // Explicit native stop retains raw clip/PCM and issued Aural stream
        // associations. Only later genuine source service can age those slots.
        mixer_->stop(musicVoice);ownsMusic_=false;fading_=false;musicSong_=0;musicGain_=0.f;
    }
    if(previousElapsed!=fadeElapsed_||previousGain!=musicGain_||revision!=mixer_->controlRevision()||
       !mixer_->canRecordControlChanges())noteControlChange();
    return true;
}
bool OriginalUiMainAudio50KG::canPlayGaragePageEffect1FDE70(
    const OriginalUiGaragePageOwner50KG& page,std::uint32_t event)const noexcept {
    const std::size_t clip=event==1u?1u:event==3u?2u:event==4u?3u:event==7u?4u:event==5u?5u:0u;
    if(!clip||!nativeGaragePageInputAudioCurrent(page)||
       nativeEffectSources_[clip-1].event!=event||!canRecordControlChange()||
       !mixer_->canRecordControlChanges(2u)||!mixer_->canReserveNativeSourceMonoVoices(1u))return false;
    return canPlayEffectClip(clip);
}
bool OriginalUiMainAudio50KG::playGaragePageEffect1FDE70(
    const OriginalUiGaragePageOwner50KG& page,std::uint32_t event)noexcept {
    if(!canPlayGaragePageEffect1FDE70(page,event))return false;
    const std::size_t clip=event==1u?1u:event==3u?2u:event==4u?3u:event==7u?4u:5u;
    // Same real Main-owned dry effect Sound/PCM and typed Aural publication.
    // Actual292360(...,1,0) uses this233410 stop-before-play/restart policy.
    // No Options route, guest group number or unrelated clip is substituted.
    return playEffectClip(clip);
}
bool OriginalUiMainAudio50KG::canApplyGarageMasterReverb26A820(
    const OriginalUiGaragePageOwner50KG& page,std::uint32_t percent,std::uint64_t frame)const noexcept {
    // Exact source calls are absolute25 at1FAF40 and absolute0 at1FB0E0.
    // Do not invent relative commands, preset changes or a clear request.
    if((percent!=0&&percent!=25)||!nativeWetInterfaceCurrent()||
       frame!=mixer_->mixedFrames()||frame!=nativeWetProcessor_->nextFrame()||
       !retainedGarageEntryCurrent()||!page.inputsCurrent()||!page.entityLive_||
       page.entityRefReleased_||page.entityCountReleased_||
       !canRecordControlChange()||!mixer_->canRecordControlChanges()||
       nativeWetMasterCount_==std::numeric_limits<std::uint64_t>::max()||
       nativeGroupControlSequence_==std::numeric_limits<std::uint64_t>::max())return false;
    const auto entry=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!entry||!backing||entry->page_.get()!=&page||page.issuer_!=backing->issuer_||
       page.entity_.sessionGeneration!=garageEntryGeneration_||
       entry->audio_.get()!=this||entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_))return false;
    for(std::size_t n=0;n<nativeGroupControlCount_;++n){const auto& c=nativeGroupControls_[n];
        if(!c||!c->routingApplied||!queuedGroupIdentityCurrent(*c)||!queuedGroupRoutingCurrent(*c))return false;
    }
    return true;
}
bool OriginalUiMainAudio50KG::applyGarageMasterReverb26A820(
    const OriginalUiGaragePageOwner50KG& page,std::uint32_t percent,std::uint64_t frame)noexcept {
    if(!canApplyGarageMasterReverb26A820(page,percent,frame))return false;
    // Supplied IRX BA D8..BA F4: ((percent << 14) - percent) / 100,
    // stored as a signed halfword, then clamped0..16383. In this bounded
    // nonnegative0/25 domain the exact results are0/4095, not Q15*0.25.
    const auto raw=static_cast<std::uint16_t>((std::uint32_t(16383)*percent)/100);
    const std::array<NativeSourceReverb50KG::RawMaster,2> values{{{raw,raw},{raw,raw}}};
    if(!(nativeWetProcessor_->*nativeWetSetRawMasters_)(values,frame))return false;
    // No fallible steps after the numerical write. Neither command2 nor its
    // source optional-parameter -1/-1 path resets any retained DSP history.
    ++nativeWetMasterCount_;nativeWetMasterSequence_=++nativeGroupControlSequence_;
    nativeWetMasterFrame_=frame;nativeWetMasterPercent_=percent;nativeWetMasterRaw_=raw;
    mixer_->noteControlChange();noteControlChange();return true;
}
OriginalUiMainAudio50KG::NativeMasterControlObservation OriginalUiMainAudio50KG::inspectNativeMasterControl()const noexcept {
    NativeMasterControlObservation out;
    if(std::this_thread::get_id()!=nativeControlThread_)return out;
    out.processor=nativeWetProcessor_;out.processorBound=nativeWetInterfaceCurrent();
    out.appliedCount=nativeWetMasterCount_;out.sequence=nativeWetMasterSequence_;
    out.acceptedFrame=nativeWetMasterFrame_;out.absolutePercent=nativeWetMasterPercent_;
    out.rawLeft=out.rawRight=nativeWetMasterRaw_;out.numericallyApplied=nativeWetMasterCount_!=0;
    return out;
}
bool OriginalUiMainAudio50KG::precedingGroupControlsCurrent()const noexcept {
    std::uint64_t previous=0;
    for(std::size_t i=0;i<nativeGroupControlCount_;++i){const auto& c=nativeGroupControls_[i];
        if(!c||c->sequence<=previous||c->sequence>nativeGroupControlSequence_||
           !(c->routingApplied&&c->wetProcessor?queuedGroupSourceLifetimeCurrent(*c):
             (queuedSceneImmediateLifetimeCurrent(*c)||
              (queuedGroupIdentityCurrent(*c)&&queuedGroupHistoryCurrent(*c))))||
           !queuedGroupRoutingCurrent(*c))return false;
        for(std::size_t j=0;j<i;++j)
            if(nativeGroupControls_[j]->slot==c->slot||nativeGroupControls_[j]->kind==c->kind)return false;
        previous=c->sequence;
    }
    return true;
}
bool OriginalUiMainAudio50KG::appendColdGroupControl(std::shared_ptr<QueuedGroupControl> next,std::string& error)noexcept {
    // Source full-buffer handling processes preceding commands before append.
    // The bounded two-kind consumer cannot pretend to flush arbitrary work.
    if(std::this_thread::get_id()!=nativeControlThread_||!next||!canRecordControlChange()||
       nativeGroupControlSequence_==std::numeric_limits<std::uint64_t>::max()||
       nativeGroupControlCount_>=nativeGroupControls_.size()||!precedingGroupControlsCurrent()){
        error="26D6C0 ordered64-command capacity, custody or revision unavailable";return false;
    }
    for(std::size_t i=0;i<nativeGroupControlCount_;++i){const auto& prior=nativeGroupControls_[i];
        if(prior->slot==next->slot||prior->kind==next->kind){
            error="26D6C0 same source group or slot is already retained";return false;}
    }
    next->mixer=mixer_;next->epoch=mixerEpoch_;next->voiceCount=mixerCount_;
    next->outputRate=mixerRate_;next->soundMode=preparedSoundMode_;
    next->wetOwner=nativeWetOwner_;next->wetProcessor=nativeWetProcessor_;
    next->acceptedFrame=mixer_?mixer_->mixedFrames():0;
    next->sequence=nativeGroupControlSequence_+1;next->acceptedControlRevision=controlRevision_+1;
    next->arguments={static_cast<std::uint32_t>(next->slot),0u,1u};
    if(next->wetProcessor){
        // The real prior owner may have retired without playing this range,
        // while the other kind retains history. Keep its exact rearm proof
        // rather than failing cold claim and silently falling back.
        next->retainedHistory=next->kind==NativeGroupControlKind::scene?
            garageDroidAudio_.retainedSceneRearmed_:garageAudio_.retainedSourceRearm_;
        const auto first=next->kind==NativeGroupControlKind::scene?OriginalUiGarageDroidAudio50GN::firstVoice:OriginalUiGarageWattoAudio50KG::firstVoice;
        const auto count=next->kind==NativeGroupControlKind::scene?OriginalUiGarageDroidAudio50GN::soundCount:OriginalUiGarageWattoAudio50KG::instanceCount;
        for(std::size_t n=0;n<count;++n)next->retainedHistory=next->retainedHistory||mixer_->playbackRevision(first+n)!=0;
    }
    if(!queuedGroupIdentityCurrent(*next)||!queuedGroupHistoryCurrent(*next)){
        error="26D6C0 source group identity or never-played history changed before acceptance";return false;}
    const auto fill=[&](AudioMixer50HD::NativeGarageRoutingClaim& binding){
        binding.command=next.get();binding.main=this;binding.group=next->group.get();binding.scene=next->scene.get();
        binding.entity=next->entity;binding.storage=mixer_->voices_;binding.sequence=next->sequence;
        binding.generation=next->generation;binding.objectKey=next->objectKey;binding.epoch=next->epoch;
        binding.sourceGroupSlot=next->slot;
    };
    bool claimed=false;
    if(next->kind==NativeGroupControlKind::scene){
        AudioMixer50HD::NativeSceneRoutingClaim binding;fill(binding);
        binding.firstVoice=OriginalUiGarageDroidAudio50GN::firstVoice;
        binding.voiceCount=OriginalUiGarageDroidAudio50GN::soundCount;
        claimed=next->retainedHistory?mixer_->claimRetainedNativeSceneRouting(binding):mixer_->claimNativeSceneRouting(binding);
    }else {
        AudioMixer50HD::NativeWattoRoutingClaim binding;fill(binding);
        binding.firstVoice=OriginalUiGarageWattoAudio50KG::firstVoice;
        binding.voiceCount=OriginalUiGarageWattoAudio50KG::instanceCount;
        claimed=next->retainedHistory?mixer_->claimRetainedNativeWattoRouting(binding):mixer_->claimNativeWattoRouting(binding);
    }
    if(!claimed){error="26D6C0 cannot claim the exact never-played native source-group range";return false;}
    // Every allocation/identity/capacity check precedes this accepted append.
    // Native custody never bumps source Entity or Sound reference counts.
    nativeGroupControls_[nativeGroupControlCount_++]=std::move(next);
    nativeGroupControlSequence_=nativeGroupControls_[nativeGroupControlCount_-1]->sequence;
    noteControlChange();error.clear();return true;
}
bool OriginalUiMainAudio50KG::enqueueWattoReverb26D6C0(
    const std::shared_ptr<OriginalUiGarageWattoSoundGroup50KG>& group,
    ReverbEnqueueReceipt& receipt,std::string& error)noexcept {
    if(!group||!receipt.command_.expired()||receipt.sequence_||group->main_.get()!=this||
       !group->queuedIdentityCurrent(group->grant_,group->entity_.sessionGeneration,group->slot40_,this)||
       nativeWattoSampleTransport()!=group->audio_||
       !(group->routingHistoryCurrent()||wattoConstructionHistoryCurrent(*group))||
       !group->wingResolved26D9F0(group->pendingWaves_)){
        error="235520 requires the exact resolved cold Watto group and unused receipt";return false;}
    try {
        auto next=std::make_shared<QueuedGroupControl>();next->group=group;
        next->grant=group->grant_;next->main=group->main_;next->entity=&group->entity_;
        next->generation=group->entity_.sessionGeneration;next->objectKey=group->entity_.objectKey;next->slot=group->slot40_;
        // Keep this strong command only until append returns; queue owns it
        // thereafter. Receipt remains the original unforgeable weak witness.
        if(!appendColdGroupControl(next,error))return false;
        receipt.command_=next;receipt.sequence_=next->sequence;return true;
    }catch(...){error="26D6C0 native Watto command allocation failed before enqueue";return false;}
}
bool OriginalUiMainAudio50KG::enqueueSceneReverb26D6C0(
    const std::shared_ptr<OriginalUiGaragePodScene50KG>& scene,std::string& error)noexcept {
    if(!scene||!scene->groupLive_||!(sceneRoutingHistoryCurrent(*scene)||sceneConstructionHistoryCurrent(*scene))){
        error="1FB594 requires the actual cold four-Sound Scene group";return false;}
    const auto issued=scene->nativeRoutingLifetime_.lock();
    if(issued!=scene||issued.owner_before(scene)||scene.owner_before(issued)){
        error="1FB594 Scene custody is not Root's genuine Page alias";return false;}
    try {
        auto next=std::make_shared<QueuedGroupControl>();next->kind=NativeGroupControlKind::scene;next->scene=scene;
        next->grant=scene->grant_;next->main=scene->main_;next->entity=&scene->group4D8_.entity;
        next->generation=next->entity->sessionGeneration;next->objectKey=next->entity->objectKey;
        next->slot=scene->group4D8_.slot40_;
        return appendColdGroupControl(std::move(next),error);
    }catch(...){error="26D6C0 native Scene command allocation failed before enqueue";return false;}
}
bool OriginalUiMainAudio50KG::queuedWattoReverbCurrent(
    const OriginalUiGarageWattoSoundGroup50KG& group,const ReverbEnqueueReceipt& receipt)const noexcept {
    const auto command=receipt.command_.lock();
    if(!command||!receipt.sequence_||command->sequence!=receipt.sequence_||
       command->kind!=NativeGroupControlKind::watto||command->group.get()!=&group||
       !queuedGroupIdentityCurrent(*command)||!queuedGroupRoutingCurrent(*command))return false;
    for(std::size_t i=0;i<nativeGroupControlCount_;++i)if(nativeGroupControls_[i]==command&&
       !nativeGroupControls_[i].owner_before(command)&&!command.owner_before(nativeGroupControls_[i]))return true;
    return false;
}
bool OriginalUiMainAudio50KG::queuedSceneReverbCurrent(const OriginalUiGaragePodScene50KG& scene)const noexcept {
    for(std::size_t i=0;i<nativeGroupControlCount_;++i){const auto& c=nativeGroupControls_[i];
        if(c&&c->kind==NativeGroupControlKind::scene&&c->scene.get()==&scene)
            return c->wetProcessor&&c->routingApplied?queuedGroupSourceLifetimeCurrent(*c):
                (queuedSceneImmediateLifetimeCurrent(*c)||
                 (queuedGroupIdentityCurrent(*c)&&queuedGroupRoutingCurrent(*c)&&queuedGroupHistoryCurrent(*c)));
    }
    return false;
}
bool OriginalUiMainAudio50KG::mayRetireQueuedWattoGroup(const OriginalUiGarageWattoSoundGroup50KG& group)const noexcept {
    for(std::size_t i=0;i<nativeGroupControlCount_;++i){const auto& c=nativeGroupControls_[i];
        if(!c)return false;
        if(c->kind!=NativeGroupControlKind::watto)continue;
        if(!c->group)return false;
        if(c->group.get()!=&group)continue;
        if(c->wetProcessor)return queuedGroupSourceLifetimeCurrent(*c)&&canRecordControlChange()&&
            group.audio_&&group.audio_->canRevise(1)&&
            mixer_->canRecordControlChanges(group.count48_+OriginalUiGarageWattoAudio50KG::instanceCount+1);
        return c->routingApplied&&queuedGroupIdentityCurrent(*c)&&group.routingHistoryCurrent()&&
            canRecordControlChange()&&mixer_->canRetireColdNativeWattoRouting(c.get());
    }
    return true;
}
bool OriginalUiMainAudio50KG::mayRetireQueuedSceneGroup(const OriginalUiGaragePodScene50KG& scene)const noexcept {
    if(std::this_thread::get_id()!=nativeControlThread_)return false;
    for(std::size_t i=0;i<nativeGroupControlCount_;++i){const auto& c=nativeGroupControls_[i];
        if(!c)return false;
        if(c->kind!=NativeGroupControlKind::scene)continue;
        if(!c->scene)return false;
        if(c->scene.get()==&scene){
            if(c->wetProcessor)return queuedGroupSourceLifetimeCurrent(*c)&&canRecordControlChange()&&
                garageDroidAudio_.preflightSourceSceneRetirement2353D0(scene)&&
                mixer_->canRecordControlChanges(OriginalUiGarageDroidAudio50GN::soundCount+1);
            return c->routingApplied&&queuedGroupIdentityCurrent(*c)&&queuedGroupHistoryCurrent(*c)&&canRecordControlChange()&&
                garageDroidAudio_.preflightSourceSceneRetirement2353D0(scene)&&mixer_->canRetireColdNativeSceneRouting(c.get());
        }
    }
    return true;
}
bool OriginalUiMainAudio50KG::publishColdSceneSampleGroupGain(OriginalUiGarageDroidAudio50GN& backend,
    std::size_t ordinal,const AudioClip50HD& authenticatedClip)noexcept {
    auto* scene=backend.sourceSceneGroup_;
    if(std::this_thread::get_id()!=nativeControlThread_||&backend!=&garageDroidAudio_||!scene||
       scene->backend_!=&backend||scene->main_.get()!=this||
       !nativeDroidSampleIdentity(&backend,mixer_,mixerEpoch_)||
       ordinal>=OriginalUiGarageDroidAudio50GN::soundCount||!AudioMixer50HD::validClip(authenticatedClip)||
       !scene->sourceRowIdentityCurrent(backend,ordinal,true)||!scene->group4D8_.enabled_||
       scene->group4D8_.manager5C!=this||scene->group4D8_.volume38_!=100||
       scene->group4D8_.pan3C_||scene->group4D8_.state44_)return false;
    const auto& source=*scene->group4D8_.sounds_[ordinal];const auto& row=backend.rows_[ordinal];
    const auto* logical=row.sound;const auto slot=OriginalUiGarageDroidAudio50GN::firstVoice+ordinal;
    // This is the actual mid-publication state:2332F0 has not yet written
    // source28/20/40/4C. Droid already authenticated its binding+handle and
    // registered this exact native clip. No fake row, replay, or ready flag.
    if(!source.sampleContinuationPending||source.nativeSample28||source.nativeSound||
       !source.sourceRegistered||source.channel2C!=-1||source.duration20!=0.f||source.basePitch50!=4096||
       !row.source||row.source!=source.authenticatedSource||!logical||logical!=backend.logical_.sound(ordinal)||
       !sameClip(logical->clip,authenticatedClip)||!logical->available||logical->channel2C!=-1||
       logical->elapsedBits24||logical->word7C||logical->volume38!=100||logical->volume44!=100||
       logical->angle3C||logical->angle48||logical->pitch40!=4096||logical->pitch4C!=4096||logical->loop1B0||
       logical->multiplier54!=1.f||logical->multiplier58!=1.f||row.played108||row.playbackRevision||
       row.basePitch50!=4096||row.sourceDuration20!=0.f||mixer_->hasVoiceGroupGain50IC(slot))return false;
    // Source2350D0 initial Group38=100/pan3C=0 and the actual source row
    // were checked above. Reverb never changes either of those fields.
    const float sourceGain=static_cast<float>(scene->group4D8_.volume38_)/100.f;
    for(std::size_t i=0;i<nativeGroupControlCount_;++i){const auto& c=nativeGroupControls_[i];
        if(!c)return false;
        if(c->kind!=NativeGroupControlKind::scene)continue;
        if(c->scene.get()!=scene||!queuedGroupIdentityCurrent(*c)||!queuedGroupRoutingCurrent(*c))return false;
        // The accepted routing claim owns the whole four-voice range. A new
        // never-played Sound can share that range with retained stopped history
        // from another Sound; it is not a cold range merely because this slot
        // has no prior playback. Keep the existing exact per-slot lease proof.
        if(c->retainedHistory){
            if(!nativeWetInterfaceCurrent()||
               (backend.retiredVoices_[ordinal].present&&
                !backend.retainedStoppedVoiceCurrent(ordinal)))return false;
            return mixer_->publishRetainedNativeSceneVoiceGroup50IC(c.get(),slot,sourceGain);
        }
        return mixer_->publishColdNativeSceneVoiceGroup50IC(c.get(),slot,sourceGain);
    }
    // Before1FB594 the exact same source publication uses the ordinary
    // backend method. No routing claim or routing-byte write is fabricated.
    return mixer_->setVoiceGroupGain50IC(slot,sourceGain);
}
bool OriginalUiMainAudio50KG::serviceColdGarageRouting(std::string& error)noexcept {
    if(!nativeGroupControlCount_){error.clear();return true;}
    if(!precedingGroupControlsCurrent()){
        error="Native routing service requires every ordered exact source-group owner";return false;}
    std::uint64_t pending=0;
    for(std::size_t i=0;i<nativeGroupControlCount_;++i)if(!nativeGroupControls_[i]->routingApplied)++pending;
    if(!canRecordControlChange(pending)||!mixer_->canRecordControlChanges(pending)){
        error="Native cold command25 batch revisions unavailable before any application";return false;}
    // All queued owners/DSP identities/clock positions and the complete Main
    // and Mixer revision budget were checked before the first routing write.
    const auto appliedFrame=mixer_->mixedFrames();
    for(std::size_t i=0;i<nativeGroupControlCount_;++i){auto& c=nativeGroupControls_[i];
        if(c->routingApplied)continue;
        // Scene flags2 may already have started a genuine source sample while
        // this25 remained queued. Consume it HERE in its existing order, not
        // during sample resolution or immediate play. Other cold paths stay
        // unchanged, including Watto and unresolved failed-entry disposal.
        const bool applied=queuedSceneImmediateLifetimeCurrent(*c)?
            mixer_->applyNativeSceneImmediateRouting(c.get()):c->retainedHistory?
            (c->kind==NativeGroupControlKind::scene?mixer_->applyRetainedNativeSceneRouting(c.get()):mixer_->applyRetainedNativeWattoRouting(c.get())):
            (c->kind==NativeGroupControlKind::scene?mixer_->applyColdNativeSceneRouting(c.get()):mixer_->applyColdNativeWattoRouting(c.get()));
        if(!applied){error="Native command25 routing application refused before mutation";return false;}
        c->routingApplied=true;c->routingFrame=appliedFrame;
        c->routingRevision=mixer_->controlRevision();noteControlChange();
    }
    error.clear();return true;
}
bool OriginalUiMainAudio50KG::releaseRetiredColdWattoRouting(OriginalUiGarageWattoSoundGroup50KG& group)noexcept {
    for(std::size_t i=0;i<nativeGroupControlCount_;++i){auto c=nativeGroupControls_[i];
        if(!c)return false;
        if(c->kind!=NativeGroupControlKind::watto)continue;
        if(!c->group)return false;
        if(c->group.get()!=&group)continue;
        const auto main=c->main.lock();const auto self=group.weak_from_this().lock();
        //Source235150 has really released the registry slot, stopped/released
        //all Sounds and finished the counted Entity. Do not use live-group
        //readiness here, and never reinterpret a stale epoch as retirement.
        if(std::this_thread::get_id()!=nativeControlThread_||!c->routingApplied||!main||main.get()!=this||
           main!=group.main_||main.owner_before(group.main_)||group.main_.owner_before(main)||
           self!=c->group||self.owner_before(c->group)||c->group.owner_before(self)||
           c->grant!=group.grant_||c->grant.owner_before(group.grant_)||group.grant_.owner_before(c->grant)||
           !group.grant_||c->entity!=&group.entity_||c->slot!=group.slot40_||
           c->generation!=group.entity_.sessionGeneration||c->objectKey!=group.entity_.objectKey||
           !group.retired_||group.attached_||group.entity_.reference00||group.count48_||group.members4C_||
           !group.audio_||!group.audio_->sourceGroupReleased_||group.audio_->sourceGroup_||
           !group.routingHistoryCurrent(true)||c->mixer!=mixer_||c->epoch!=mixerEpoch_||!queuedGroupProcessorCurrent(*c)||
           !canRecordControlChange()||!mixer_->canRetireColdNativeWattoRouting(c.get()))return false;
        if(!mixer_->retireColdNativeWattoRouting(c.get()))return false;
        noteControlChange();
        //Ordered accepted work was APPLIED, not canceled. Drop its native
        //custody only now. The backend's per-slot routing byte persists.
        for(std::size_t j=i+1;j<nativeGroupControlCount_;++j)nativeGroupControls_[j-1]=std::move(nativeGroupControls_[j]);
        nativeGroupControls_[--nativeGroupControlCount_].reset();return true;
    }
    return true; //No accepted command belongs to an early partial group.
}
bool OriginalUiMainAudio50KG::releaseRetiredWattoRouting(OriginalUiGarageWattoSoundGroup50KG& group)noexcept {
    for(std::size_t n=0;n<nativeGroupControlCount_;++n){const auto c=nativeGroupControls_[n];
        if(!c)return false;
        if(c->kind!=NativeGroupControlKind::watto||c->group.get()!=&group)continue;
        if(!c->wetProcessor)return releaseRetiredColdWattoRouting(group);
        const auto main=c->main.lock();const auto self=group.weak_from_this().lock();
        // Real source235150 has completed stop233670/release104970 for each
        // Sound and the all-instance native drain. PCM leases still pin every
        // stopped raw clip. Do not call the now-dead live Group predicate.
        if(std::this_thread::get_id()!=nativeControlThread_||!c->routingApplied||
           !c->sequence||c->sequence>nativeGroupControlSequence_||c->command!=25||
           c->slot<0||c->slot>=25||
           c->arguments!=std::array<std::uint32_t,3>{static_cast<std::uint32_t>(c->slot),0u,1u}||
           main.get()!=this||main!=group.main_||main.owner_before(group.main_)||group.main_.owner_before(main)||
           self!=c->group||self.owner_before(c->group)||c->group.owner_before(self)||
           c->grant!=group.grant_||c->grant.owner_before(group.grant_)||group.grant_.owner_before(c->grant)||
           !group.grant_||c->entity!=&group.entity_||c->slot!=group.slot40_||
           c->generation!=group.entity_.sessionGeneration||c->objectKey!=group.entity_.objectKey||
           c->objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&group.entity_))||
           group.audio_!=&garageAudio_||!group.routingSourceCurrent(true)||
           c->mixer!=mixer_||c->epoch!=mixerEpoch_||c->voiceCount!=mixerCount_||
           c->outputRate!=mixerRate_||c->soundMode!=preparedSoundMode_||!queuedGroupProcessorCurrent(*c)||
           !canRecordControlChange()||!mixer_->canRetireNativeWattoRouting(c.get()))return false;
        if(!mixer_->retireNativeWattoRouting(c.get()))return false;
        noteControlChange();
        for(std::size_t i=n+1;i<nativeGroupControlCount_;++i)nativeGroupControls_[i-1]=std::move(nativeGroupControls_[i]);
        nativeGroupControls_[--nativeGroupControlCount_].reset();return true;
    }
    return true;
}
bool OriginalUiMainAudio50KG::releaseRetiredColdSceneRouting(
    const std::shared_ptr<OriginalUiGaragePageOwner50KG>& page,OriginalUiGaragePodScene50KG& scene)noexcept {
    if(!garageDroidAudio_.canRearmRetiredSceneCold())return false;
    return releaseRetiredSceneRouting(page,scene);
}
bool OriginalUiMainAudio50KG::releaseRetiredSceneRouting(
    const std::shared_ptr<OriginalUiGaragePageOwner50KG>& page,OriginalUiGaragePodScene50KG& scene)noexcept {
    const bool live=nativeWetInterfaceCurrent();
    // The actual Page allocation retains its dead Scene in a DIFFERENT native
    // slot after the source1FA1BC null store. Neither a stale source pointer nor
    // a queued alias alone is authority to dereference a destroyed Scene.
    if(std::this_thread::get_id()!=nativeControlThread_||!page||
       page->retiredSceneBacking_.get()!=&scene||page->sceneBacking_||page->scene4C0_||
       !page->sceneReferenceReleased_||page->entityLive_||!page->entityRefReleased_||
       !page->entityCountReleased_||page->entity_.reference00||page->entity_.firstChild18||
       page->stage_!=OriginalUiGaragePageOwner50KG::Stage::pagePayloadRetired||
       !page->grant_||page->grant_!=scene.grant_||page->grant_.owner_before(scene.grant_)||
       scene.grant_.owner_before(page->grant_)||scene.sceneLive_||scene.groupLive_||
       scene.podScene_.reference00||scene.podScene_.firstChild18||
       scene.retirementStage_!=OriginalUiGaragePodScene50KG::RetirementStage::complete||
       scene.soundGroupRetirement_!=OriginalUiGaragePodScene50KG::SoundGroupRetirement::complete||
       scene.group4D8_.entity.reference00||scene.group4D8_.count48_||scene.group4D8_.members4C_||
       scene.group4D8_.capacity54_||scene.pendingWaves_||scene.backend_!=&garageDroidAudio_||
       scene.group4D8_.manager5C!=this||scene.main_.get()!=this||
       scene.group4D8_.entity.sessionGeneration!=page->entity_.sessionGeneration||
       !nativeDroidSampleIdentity(scene.backend_,mixer_,mixerEpoch_)||
       !(live?garageDroidAudio_.canRearmRetiredSceneRetainingHistory():garageDroidAudio_.canRearmRetiredSceneCold())||nativeGroupControlCount_!=1)return false;
    for(const auto& sound:scene.group4D8_.sounds_)if(sound)return false;
    const std::shared_ptr<OriginalUiGaragePodScene50KG> actual(page,&scene);
    const auto issued=scene.nativeRoutingLifetime_.lock();
    const auto& c=nativeGroupControls_[0];
    if(!issued||issued!=actual||issued.owner_before(actual)||actual.owner_before(issued)||
       !c||c->kind!=NativeGroupControlKind::scene||c->group||c->scene!=actual||
       c->scene.owner_before(actual)||actual.owner_before(c->scene)||
       !c->routingApplied||!c->sequence||c->sequence>nativeGroupControlSequence_||
       c->command!=25||c->slot<0||c->slot>=25||
       c->arguments!=std::array<std::uint32_t,3>{static_cast<std::uint32_t>(c->slot),0u,1u}||
       c->grant!=scene.grant_||c->grant.owner_before(scene.grant_)||scene.grant_.owner_before(c->grant)||
       c->entity!=&scene.group4D8_.entity||c->slot!=scene.group4D8_.slot40_||
       c->generation!=page->entity_.sessionGeneration||c->objectKey!=scene.group4D8_.entity.objectKey||
       c->objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&scene.group4D8_.entity))||
       c->mixer!=mixer_||c->epoch!=mixerEpoch_||c->voiceCount!=mixerCount_||
       c->outputRate!=mixerRate_||c->soundMode!=preparedSoundMode_||!queuedGroupProcessorCurrent(*c)||
       !canRecordControlChange()||!(live?mixer_->canRetireNativeSceneRouting(c.get()):mixer_->canRetireColdNativeSceneRouting(c.get())))return false;
    const auto main=c->main.lock();
    if(main!=scene.main_||main.owner_before(scene.main_)||scene.main_.owner_before(main))return false;
    // This command really applied its routing metadata. No pending command is
    // cancelled, no wet DSP is claimed and the persistent source-slot byte stays.
    if(!(live?mixer_->retireNativeSceneRouting(c.get()):mixer_->retireColdNativeSceneRouting(c.get())))return false;
    noteControlChange();nativeGroupControls_[0].reset();nativeGroupControlCount_=0;return true;
}
bool OriginalUiMainAudio50KG::resolvedEntryFailureSourceCustodyCurrent(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,ResolvedEntryFailure kind)const noexcept {
    using Phase=OriginalUiGarageSession50KG::NormalRetirementPhase;
    if(!entry||garageSourceLifetime_!=GarageSourceLifetime::constructing||paused_||
       !retainedGarageOrdinaryCurrent()||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       (entry->normalRetirementPhase_!=Phase::notStarted&&entry->normalRetirementPhase_!=Phase::dispatcherPrefix)||
       !garageEntryGeneration_||!garageEntryEpoch_||!canRecordControlChange(4)||
       !mixer_->canRecordControlChanges(OriginalUiGarageWattoAudio50KG::soundCount+
           OriginalUiGarageWattoAudio50KG::instanceCount+OriginalUiGarageDroidAudio50GN::soundCount+4u)||
       !garageAudio_.canRevise(2)||garageDroidAudio_.revision_>=std::numeric_limits<std::uint64_t>::max()-2)
        return false;
    const auto owned=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!owned||owned!=entry||owned.owner_before(entry)||entry.owner_before(owned)||
       !backing||!backing->issuer_||entry->audio_.get()!=this||
       entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_))return false;
    bool sourceIdentity=false;
    switch(kind){
    case ResolvedEntryFailure::publishedObservation:
        sourceIdentity=entry->nativeAudioPublishedObservationFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,false);break;
    case ResolvedEntryFailure::actorSample:
        sourceIdentity=entry->nativeAudioActorSampleFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,false);break;
    case ResolvedEntryFailure::unsealedRender:
        sourceIdentity=entry->nativeAudioUnsealedRenderFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,false);break;
    }
    if(!sourceIdentity)return false;
    const auto& page=entry->page_;const auto* scene=entry->podScene_;
    if(!page||!scene||page->sceneBacking_.get()!=scene||page->scene4C0_!=scene||
       page->retiredSceneBacking_||scene->main_.get()!=this||scene->backend_!=&garageDroidAudio_||
       scene->nativeUnresolvedEntryRetiring_||garageDroidAudio_.nativeUnresolvedEntryRetired_||
       !scene->sceneLive_||!scene->groupLive_||
       scene->soundGroupRetirement_!=OriginalUiGaragePodScene50KG::SoundGroupRetirement::live||
       !entry->pending_||scene->pendingWaves_!=entry->pending_||
       scene->pendingWaves_.owner_before(entry->pending_)||entry->pending_.owner_before(scene->pendingWaves_)||
       garageDroidAudio_.sourceSceneGroup_!=scene||garageDroidAudio_.sourceSceneGroupReleased_||
       !garageDroidAudio_.resolvedRowsCurrent()||nativeGroupControlCount_!=2||
       !precedingGroupControlsCurrent())return false;
    // Resolved source/Sound/PCM custody is separate from applied wet routing.
    // No assumed silence: the existing live row proof retains any real issued
    // immediate sample and its exact native voice/epoch/lease until233670.
    for(std::size_t i=0;i<OriginalUiGarageDroidAudio50GN::soundCount;++i){
        const auto& source=scene->group4D8_.sounds_[i];
        if(!source||source->reference00!=1||source->sampleContinuationPending||
           !source->nativeSample28||!source->nativeSample28->current()||
           !scene->sourceRowIdentityCurrent(garageDroidAudio_,i,true))return false;
    }
    const auto& watto=nativeGroupControls_[0];const auto& command=nativeGroupControls_[1];
    if(!watto||watto->kind!=NativeGroupControlKind::watto||!watto->routingApplied||
       !watto->group||watto->group.get()!=garageAudio_.sourceGroup_||
       !watto->group->wingResolved26D9F0(entry->pending_)||
       !watto->group->preflightPendingRetirement235150()||
       !command||command->kind!=NativeGroupControlKind::scene||command->scene.get()!=scene||
       !(command->routingApplied?queuedGroupSourceLifetimeCurrent(*command):
           queuedSceneImmediateLifetimeCurrent(*command)))return false;
    return true;
}
bool OriginalUiMainAudio50KG::preflightPublishedObservationEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)const noexcept {
    if(!entry||entry->retiring_||
       entry->normalRetirementPhase_!=OriginalUiGarageSession50KG::NormalRetirementPhase::notStarted||
       !resolvedEntryFailureSourceCustodyCurrent(entry,ResolvedEntryFailure::publishedObservation)){
        try{error="Published observation failure lacks its exact resolved source/control/PCM custody";}
        catch(...){error.clear();}return false;
    }
    error.clear();return true;
}
bool OriginalUiMainAudio50KG::preflightActorSampleEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)const noexcept {
    if(!entry||entry->retiring_||
       entry->normalRetirementPhase_!=OriginalUiGarageSession50KG::NormalRetirementPhase::notStarted||
       !resolvedEntryFailureSourceCustodyCurrent(entry,ResolvedEntryFailure::actorSample)){
        try{error="Partial Actor failure lacks its exact resolved source/control/PCM custody";}
        catch(...){error.clear();}return false;
    }
    error.clear();return true;
}
bool OriginalUiMainAudio50KG::preflightUnsealedRenderEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)const noexcept {
    if(!entry||entry->retiring_||
       entry->normalRetirementPhase_!=OriginalUiGarageSession50KG::NormalRetirementPhase::notStarted||
       !resolvedEntryFailureSourceCustodyCurrent(entry,ResolvedEntryFailure::unsealedRender)){
        try{error="Unsealed render failure lacks its exact completed-update source/control/PCM custody";}
        catch(...){error.clear();}return false;
    }
    error.clear();return true;
}
bool OriginalUiMainAudio50KG::drainPublishedObservationEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    return drainResolvedEntryFailure(entry,ResolvedEntryFailure::publishedObservation,error);
}
bool OriginalUiMainAudio50KG::drainActorSampleEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    return drainResolvedEntryFailure(entry,ResolvedEntryFailure::actorSample,error);
}
bool OriginalUiMainAudio50KG::drainUnsealedRenderEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    return drainResolvedEntryFailure(entry,ResolvedEntryFailure::unsealedRender,error);
}
bool OriginalUiMainAudio50KG::drainResolvedEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,ResolvedEntryFailure kind,std::string& error)noexcept {
    if(!entry||!entry->retiring_||
       entry->normalRetirementPhase_!=OriginalUiGarageSession50KG::NormalRetirementPhase::dispatcherPrefix||
       !resolvedEntryFailureSourceCustodyCurrent(entry,kind)){
        try{error="Resolved failed-entry control drain requires exact entered terminal cleanup before source teardown";}
        catch(...){error.clear();}return false;
    }
    // Native failure handling only. Command25 was already accepted and owns
    // a real claim; its normal retirement contract requires actual application.
    // Preserve its order/history instead of canceling or forging an applied
    // receipt. The existing consumer skips applied rows on a genuine retry.
    // No guest flush PC, sample resolution, voice age, PCM or clock is advanced.
    if(!serviceColdGarageRouting(error))return false;
    if(!entry->podScene_->preflightRoutingRetirement235150()){
        try{error="Resolved failed-entry controls drained but actual Scene retirement remains unavailable";}
        catch(...){error.clear();}return false;
    }
    error.clear();return true;
}
bool OriginalUiMainAudio50KG::retirePublishedObservationEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    return retireResolvedEntryFailure(entry,ResolvedEntryFailure::publishedObservation,error);
}
bool OriginalUiMainAudio50KG::retireActorSampleEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    return retireResolvedEntryFailure(entry,ResolvedEntryFailure::actorSample,error);
}
bool OriginalUiMainAudio50KG::retireUnsealedRenderEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,std::string& error)noexcept {
    return retireResolvedEntryFailure(entry,ResolvedEntryFailure::unsealedRender,error);
}
bool OriginalUiMainAudio50KG::retireResolvedEntryFailure(
    const std::shared_ptr<OriginalUiGarageSession50KG>& entry,ResolvedEntryFailure kind,std::string& error)noexcept {
    const auto fail=[&](const char* why)noexcept {try{error=why;}catch(...){error.clear();}return false;};
    if(!entry||garageSourceLifetime_!=GarageSourceLifetime::constructing||paused_||
       !retainedGarageTransportCurrent()||!ordinaryVoiceOwnershipCurrent()||
       !(silentGarageVoiceRangesCurrent()||liveGarageVoiceRangesCurrent())||!nativeWetInterfaceCurrent()||
       !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
       !garageColdMetadata_||!garageColdMetadata_->prepared()||
       garageColdMetadata_!=garageAudio_.metadata_||garageColdMetadata_.owner_before(garageAudio_.metadata_)||
       garageAudio_.metadata_.owner_before(garageColdMetadata_)||
       !canRecordControlChange(2)||!mixer_->canRecordControlChanges(2))
        return fail("Resolved failed-entry terminal audio lost its retained transport or completed source prefix");
    const auto owned=garageEntryCustody_.lock();const auto backing=garagePersistentBacking_.lock();
    if(!owned||owned!=entry||owned.owner_before(entry)||entry.owner_before(owned)||
       !backing||!backing->issuer_||entry->audio_.get()!=this||
       entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_))
        return fail("Resolved failed-entry terminal audio requires the same retained manager custody");
    bool sourceIdentity=false;GarageSourceLifetime terminalState=GarageSourceLifetime::unbound;
    switch(kind){
    case ResolvedEntryFailure::publishedObservation:
        sourceIdentity=entry->nativeAudioPublishedObservationFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,true);
        terminalState=GarageSourceLifetime::publishedObservationEntryRetired;break;
    case ResolvedEntryFailure::actorSample:
        sourceIdentity=entry->nativeAudioActorSampleFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,true);
        terminalState=GarageSourceLifetime::actorSampleEntryRetired;break;
    case ResolvedEntryFailure::unsealedRender:
        sourceIdentity=entry->nativeAudioUnsealedRenderFailureIdentity(*backing->issuer_,*this,
            garageEntryGeneration_,garageEntryEpoch_,true);
        terminalState=GarageSourceLifetime::unsealedRenderEntryRetired;break;
    }
    if(!sourceIdentity)return fail("Resolved failed-entry terminal audio requires actual same-manager source retirement");
    const auto& page=entry->page_;
    if(!page||page->entity_.sessionGeneration!=garageEntryGeneration_||!page->retiredSceneBacking_||
       page->retiredSceneBacking_->nativeUnresolvedEntryRetiring_||
       garageDroidAudio_.nativeUnresolvedEntryRetired_||
       !garageDroidAudio_.canRearmRetiredSceneRetainingHistory()||
       !garageAudio_.canRearmRetiredSourceGroupRetainingHistory()||
       mixer_->nativeWattoRouting_.command)
        return fail("Resolved failed-entry terminal audio still owns unfinished source Sounds or voices");
    // This exact existing method authenticates dead Page/Scene/group identity,
    // the applied command, stopped reservations and strong retained PCM before
    // retiring its real routing claim and then releasing the command alias.
    if(!releaseRetiredSceneRouting(page,*page->retiredSceneBacking_))
        return fail("Resolved failed-entry terminal audio lacks its exact applied retired Scene claim");
    // Same physical native-borrow discharge as normal completed retirement,
    // but NO lastRearmedGaragePage/generation or GarageSourceLifetime::retired.
    // Strong stopped PCM, clips, cursors, revisions and DSP history survive.
    if(!mixer_->recycleRetiredSceneBindings()||!garageDroidAudio_.rearmRetiredSceneRetainingHistory()||
       !garageAudio_.rearmRetiredSourceGroupRetainingHistory())std::terminate();
    page->retiredSceneBacking_.reset();garageEntryCustody_.reset();
    garageSourceLifetime_=terminalState;
    noteControlChange();error.clear();return true;
}
bool OriginalUiMainAudio50KG::rearmGarageAfterManagerRetirementCold(
    const std::shared_ptr<OriginalUiGaragePageOwner50KG>& page,std::uint64_t generation,
    const std::shared_ptr<const assets::OriginalStreamMetadata50KZ>& metadata,std::string& error)noexcept {
    return rearmGarageAfterManagerRetirement(page,generation,metadata,GarageReentryTransport::pristineCold,error);
}
bool OriginalUiMainAudio50KG::rearmGarageAfterManagerRetirementRetainingTransport(
    const std::shared_ptr<OriginalUiGaragePageOwner50KG>& page,std::uint64_t generation,
    const std::shared_ptr<const assets::OriginalStreamMetadata50KZ>& metadata,std::string& error)noexcept {
    const auto fail=[&](const char* why)noexcept {try{error=why;}catch(...){error.clear();}return false;};
    if(!retainedGarageTransportCurrent()||!ordinaryVoiceOwnershipCurrent()||!(silentGarageVoiceRangesCurrent()||liveGarageVoiceRangesCurrent())||paused_)
        return fail("Garage reentry cannot authenticate the retained ordinary transport and silent source ranges");
    const auto backing=garagePersistentBacking_.lock();const auto entry=garageEntryCustody_.lock();
    if(!backing||!entry||!entry->nativeAudioRetirementIdentity(*backing->issuer_,*this,generation,garageEntryEpoch_)||
       entry->audio_.owner_before(backing)||backing.owner_before(entry->audio_)||
       (garageSourceLifetime_!=GarageSourceLifetime::constructing&&
        garageSourceLifetime_!=GarageSourceLifetime::constructed&&garageSourceLifetime_!=GarageSourceLifetime::retired)||
       generation!=garageEntryGeneration_||entry->normalManagerGeneration_!=generation||
       entry->normalRetirementPhase_!=OriginalUiGarageSession50KG::NormalRetirementPhase::currentCleared||
       entry->page_!=page||entry->page_.owner_before(page)||page.owner_before(entry->page_))
        return fail("Garage reentry requires this completed source manager's exact retained capsule");
    assets::OriginalStandardMaterialDiagnostic50LA retired;
    if(!entry->normalSourceOwnersRetired(retired,error))return false;
    return rearmGarageAfterManagerRetirement(page,generation,metadata,GarageReentryTransport::retainedOrdinary,error);
}
bool OriginalUiMainAudio50KG::rearmGarageAfterManagerRetirement(
    const std::shared_ptr<OriginalUiGaragePageOwner50KG>& page,std::uint64_t generation,
    const std::shared_ptr<const assets::OriginalStreamMetadata50KZ>& metadata,
    GarageReentryTransport transport,std::string& error)noexcept {
    const auto fail=[&](const char* why)noexcept {try{error=why;}catch(...){error.clear();}return false;};
    if(finalNativePcmRetired_||std::this_thread::get_id()!=nativeControlThread_||!page||!generation||
       page->entity_.sessionGeneration!=generation)return fail("Garage audio reentry requires the exact completed Page generation");
    const auto completed=lastRearmedGaragePage_.lock();
    if(completed==page&&!completed.owner_before(page)&&!page.owner_before(completed)&&
       lastRearmedGarageGeneration_==generation){
        if(page->sceneBacking_||page->retiredSceneBacking_||page->scene4C0_||nativeGroupControlCount_||
           !garageColdPending_||garageColdFinishAttempted_||garagePrepared_||garageScenePrepared_||
           !garageDroidAudio_.sourceConstructionPrefixCurrent()||!garageAudio_.sourceGroupConstructionCurrent())
            return fail("Garage audio reentry receipt no longer names its untouched construction prefix");
        error.clear();return true;
    }
    // The legacy cold path still requires never-played ranges and clock0.
    // Only actual retained source retirement can carry stopped, exactly owned
    // Garage clip histories into new source construction without resetting them.
    const bool cold=transport==GarageReentryTransport::pristineCold;
    const bool live=!cold&&nativeWetInterfaceCurrent();
    if((cold&&garageSourceLifetime_!=GarageSourceLifetime::unbound)||
       (!cold&&(!retainedGarageTransportCurrent()||!ordinaryVoiceOwnershipCurrent()||!(silentGarageVoiceRangesCurrent()||liveGarageVoiceRangesCurrent()))))
        return fail("Garage reentry transport proof does not match its exact native lifetime");
    // The normal source path keeps its cold source accessor through Actor
    // retirement and does not call the optional native finish seam. Actual
    // completed Page/manager destruction (proved above) retires that prefix;
    // do not invent a finish call or ready flags just to satisfy native reuse.
    const bool finished=!garageColdPending_&&garageColdFinishAttempted_&&garagePrepared_&&garageScenePrepared_;
    const bool retiredPrefix=!cold&&garageSourceLifetime_==GarageSourceLifetime::constructing&&
        garageColdPending_&&!garageColdFinishAttempted_&&!garagePrepared_&&!garageScenePrepared_&&
        garageColdMetadata_==metadata&&!garageColdMetadata_.owner_before(metadata)&&!metadata.owner_before(garageColdMetadata_);
    if(!prepared_||(!finished&&!retiredPrefix)||
       !metadata||!metadata->prepared()||metadata!=garageAudio_.metadata_||
       metadata.owner_before(garageAudio_.metadata_)||garageAudio_.metadata_.owner_before(metadata)||
       !mixer_||mixer_->groupEpoch50IC()!=mixerEpoch_||
       mixer_->count()!=garageSceneRequiredVoices||mixerCount_!=garageSceneRequiredVoices||
       mixer_->sampleRate()!=mixerRate_||mixer_->originalSoundMode50KG()!=preparedSoundMode_||
       (cold&&(mixer_->mixedFrames()||ownsMusic_))||paused_||!canRecordControlChange(2)||
       !mixer_->canRecordControlChanges(2)||!page->retiredSceneBacking_||
       !(live?garageDroidAudio_.canRearmRetiredSceneRetainingHistory():garageDroidAudio_.canRearmRetiredSceneCold())||
       !(live?garageAudio_.canRearmRetiredSourceGroupRetainingHistory():garageAudio_.canRearmRetiredSourceGroupCold()))
        return fail("Garage audio reentry needs exact retired cold owners; active or changed transport remains untouched");
    for(std::size_t i=cold?0u:courseRequiredVoices;i<OriginalUiGarageDroidAudio50GN::firstVoice;++i)
        if(mixer_->active(i))return fail("Garage source reentry refuses to silence a real owned or foreign voice");
    if(!(live?releaseRetiredSceneRouting(page,*page->retiredSceneBacking_):releaseRetiredColdSceneRouting(page,*page->retiredSceneBacking_)))
        return fail("Garage audio reentry has no exact applied retired Scene routing receipt");
    // All ownership, identity, stopped-source/PCM and revision checks preceded
    // the first mutation. There are no callbacks, allocations or source operations
    // in this commit suffix. Any contradiction is an invariant violation.
    if(live){
        if(!mixer_->recycleRetiredSceneBindings()||!garageDroidAudio_.rearmRetiredSceneRetainingHistory()||
           !garageAudio_.rearmRetiredSourceGroupRetainingHistory())std::terminate();
    }else if(!mixer_->recycleRetiredColdSceneBindings()||!garageDroidAudio_.rearmRetiredSceneCold()||
       !garageAudio_.rearmRetiredSourceGroupCold())std::terminate();
    garageColdMetadata_=metadata;garageColdPending_=true;garageColdFinishAttempted_=false;
    garagePrepared_=false;garageScenePrepared_=false;
    page->retiredSceneBacking_.reset();
    lastRearmedGaragePage_=page;lastRearmedGarageGeneration_=generation;
    if(!cold)garageSourceLifetime_=GarageSourceLifetime::retired;
    noteControlChange();
    error.clear();return true;
}
std::size_t OriginalUiMainAudio50KG::pendingNativeGroupControls()const noexcept {
    std::size_t n=0;for(std::size_t i=0;i<nativeGroupControlCount_;++i)
        if(nativeGroupControls_[i]&&!nativeGroupControls_[i]->routingApplied)++n;
    return n;
}
bool OriginalUiMainAudio50KG::inspectNativeGroupControl(std::size_t index,NativeGroupControlObservation& out)const noexcept {
    out={};if(index>=nativeGroupControlCount_||!nativeGroupControls_[index])return false;
    const auto& c=*nativeGroupControls_[index];out.kind=c.kind;out.scene=c.scene.get();
    out.group=c.group.get();out.countedEntity=c.entity;out.mixer=c.mixer;
    out.sequence=c.sequence;out.groupGeneration=c.generation;out.objectKey=c.objectKey;out.mixerEpoch=c.epoch;
    out.acceptedControlRevision=c.acceptedControlRevision;out.groupSlot=c.slot;
    out.wetProcessor=c.wetProcessor;out.acceptedFrame=c.acceptedFrame;out.routingFrame=c.routingFrame;
    out.processorBound=c.wetProcessor&&queuedGroupProcessorCurrent(c);
    out.sourceCommand=c.command;out.sourceArguments=c.arguments;out.queued=!c.routingApplied;
    out.routingApplied=c.routingApplied;out.applied=false;out.neverPlayed=queuedGroupHistoryCurrent(c);
    out.identityCurrent=queuedGroupIdentityCurrent(c)&&queuedGroupRoutingCurrent(c);return true;
}
bool OriginalUiMainAudio50KG::garageMenuReady() const noexcept {
    if(!current()||!garageGroupCurrent()||!preparedClipMatches(garageClip_,garagePcm_,garageOriginal,2))return false;
    if(!nativeGroupControlCount_)return garageSourceLifetime_==GarageSourceLifetime::unbound&&!nativeWetOwner_; // Historical unbound transport only.
    if(garageSourceLifetime_!=GarageSourceLifetime::constructed||!nativeSourcePlaybackCurrent()||
       pendingNativeGroupControls()||nativeGroupControlCount_!=(garageScenePrepared_?2u:1u))return false;
    bool watto=false,scene=false;
    for(std::size_t n=0;n<nativeGroupControlCount_;++n){const auto& c=nativeGroupControls_[n];
        if(!c||!queuedGroupSourceLifetimeCurrent(*c))return false;
        if(c->kind==NativeGroupControlKind::watto){if(watto)return false;watto=true;}
        else {if(scene)return false;scene=true;}
    }
    return watto&&scene==garageScenePrepared_;
}
OriginalUiGarageWattoAudio50KG* OriginalUiMainAudio50KG::garageWattoAudio() noexcept {
    return garageMenuReady()?&garageAudio_:nullptr;
}
const OriginalUiGarageWattoAudio50KG* OriginalUiMainAudio50KG::garageWattoAudio() const noexcept {
    return garageMenuReady()?&garageAudio_:nullptr;
}
bool OriginalUiMainAudio50KG::canStartGarageLoop6() const noexcept {
    return garageMenuReady()&&!paused_&&canRecordControlChange()&&mixer_->canRecordControlChanges(1)&&
        (!mixer_->active(musicVoice)||(ownsMusic_&&mixer_->playbackRevision(musicVoice)==musicPlaybackRevision_))&&canPlayEffectClip(1);
}
bool OriginalUiMainAudio50KG::startGarageLoop6() noexcept {return canStartGarageLoop6()&&startMusicLoop(garageClip_,6,OriginalUiGarageWattoAudio50KG::nativeRendererPitch);}
bool OriginalUiMainAudio50KG::canFadeGarageMusic() const noexcept {
    return garageMenuReady()&&!paused_&&canRecordControlChange()&&
      ((!ownsMusic_&&!mixer_->active(musicVoice))||(ownsMusic_&&musicSong_==6&&mixer_->active(musicVoice)&&mixer_->playbackRevision(musicVoice)==musicPlaybackRevision_));
}
bool OriginalUiMainAudio50KG::fadeGarageMusic() noexcept {return canFadeGarageMusic()&&fadeMain(1.f);}
bool OriginalUiMainAudio50KG::creditsMenuReady() const noexcept {
    return current()&&creditsPrepared_&&preparedClipMatches(creditsClip_,creditsPcm_,creditsOriginal,2);
}
bool OriginalUiMainAudio50KG::canStartCreditsLoop12() const noexcept {
    return creditsMenuReady()&&!paused_&&canRecordControlChange()&&mixer_->canRecordControlChanges(1)&&
        (!mixer_->active(musicVoice)||(ownsMusic_&&mixer_->playbackRevision(musicVoice)==musicPlaybackRevision_))&&
        canPlayEffectClip(1); // Same centered routing check, without an effect command.
}
bool OriginalUiMainAudio50KG::startCreditsLoop12() noexcept {
    return canStartCreditsLoop12()&&startMusicLoop(creditsClip_,12);
}
bool OriginalUiMainAudio50KG::canFadeCreditsMusic() const noexcept {
    return creditsMenuReady()&&!paused_&&canRecordControlChange()&&
        ((!ownsMusic_&&!mixer_->active(musicVoice))||
         (ownsMusic_&&musicSong_==12&&mixer_->active(musicVoice)&&
          mixer_->playbackRevision(musicVoice)==musicPlaybackRevision_));
}
bool OriginalUiMainAudio50KG::fadeCreditsMusic() noexcept {
    return canFadeCreditsMusic()&&fadeMain(1.f);
}
bool OriginalUiMainAudio50KG::hallOfFameMenuReady() const noexcept {
    return current()&&hallOfFamePrepared_&&preparedClipMatches(hallOfFameClip_,hallOfFamePcm_,hallOfFameOriginal,2);
}
bool OriginalUiMainAudio50KG::canStartHallOfFameLoop5() const noexcept {
    return hallOfFameMenuReady()&&!paused_&&canRecordControlChange()&&mixer_->canRecordControlChanges(1)&&
        (!mixer_->active(musicVoice)||(ownsMusic_&&mixer_->playbackRevision(musicVoice)==musicPlaybackRevision_))&&
        canPlayEffectClip(1); // Same centered routing check, without an effect command.
}
bool OriginalUiMainAudio50KG::startHallOfFameLoop5() noexcept {
    return canStartHallOfFameLoop5()&&startMusicLoop(hallOfFameClip_,5);
}
bool OriginalUiMainAudio50KG::canFadeHallOfFameMusic() const noexcept {
    return hallOfFameMenuReady()&&!paused_&&canRecordControlChange()&&
        ((!ownsMusic_&&!mixer_->active(musicVoice))||
         (ownsMusic_&&musicSong_==5&&mixer_->active(musicVoice)&&
          mixer_->playbackRevision(musicVoice)==musicPlaybackRevision_));
}
bool OriginalUiMainAudio50KG::fadeHallOfFameMusic() noexcept {
    return canFadeHallOfFameMusic()&&fadeMain(1.f);
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
    if(nativeWetOwner_){
        if(!nativeWetPlayRider_||!(nativeWetOwner_->*nativeWetPlayRider_)(*this,clip,riderConfirmClips_[clip],1.f,1.f))return false;
    }else if(!mixer_->play(riderConfirmVoice,riderConfirmClips_[clip],1.f,1.f,false,0.f))return false;
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
       !canRecordControlChange()||!mixer_->canRecordControlChanges(
           (mixer_->active(riderConfirmVoice)||
            mixer_->nativeSourceVoiceReserved(riderConfirmVoice,riderConfirmPlaybackRevision_))?1u:0u))return false;
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
const assets::OriginalStreamMetadataRow50KZ* OriginalUiMainAudio50KG::nativeSongSourceCurrent(
    std::uint32_t song,const AudioClip50HD& clip)const noexcept {
    if(!prepared_||!nativeSourceMetadata_||!nativeSourceMetadata_->prepared())return nullptr;
    struct Source {std::uint32_t song,ordinal,size;const char* name;const char* duration;};
    // UI_MUSIC.JUK/JUB were authenticated by this same retained Main. Ordinals
    // are original Streams.TXT positions, never invented stream/sample handles.
    static constexpr std::array<Source,6> sources{{
        {9,1692,219504,"music\\ui\\screens\\ui_main.vag","17.420952"},
        {6,1689,265328,"music\\ui\\screens\\wattoshp.vag","21.057777"},
        {7,1690,353744,"music\\ui\\screens\\ui_peopl.vag","28.074921"},
        {11,1694,223232,"music\\ui\\screens\\ui_trks.vag","17.716825"},
        {12,1695,561232,"music\\ui\\screens\\credits.vag","44.542221"},
        {5,1688,353744,"music\\ui\\gallery\\gallery.vag","28.074921"}
    }};
    const Source* source=nullptr;for(const auto& candidate:sources)if(candidate.song==song){source=&candidate;break;}
    if(!source)return nullptr;
    const auto* row=nativeSourceMetadata_->row(source->ordinal);
    if(!row||nativeSourceMetadata_->find(source->name)!=row||row->sourceIndex!=source->ordinal||
       row->sourceName!=source->name||row->originalSize!=source->size||row->originalPitch!=1881||
       row->originalSampleRate!=22050||row->authoredDuration.value!=source->duration||
       !std::isfinite(row->admittedDurationSeconds)||row->admittedDurationSeconds<=0.f)return nullptr;
    const AudioClip50HD* retained=nullptr;bool valid=false;
    switch(song){
    case 9:retained=&clips_[0];valid=preparedClipMatches(clips_[0],pcm_[0],originals[0],2);break;
    case 6:retained=&garageClip_;valid=preparedClipMatches(garageClip_,garagePcm_,garageOriginal,2);break;
    case 7:retained=&riderClip_;valid=riderPrepared_&&preparedClipMatches(riderClip_,riderPcm_,riderOriginal,2);break;
    case 11:retained=&courseClip_;valid=coursePrepared_&&preparedClipMatches(courseClip_,coursePcm_,courseOriginal,2);break;
    case 12:retained=&creditsClip_;valid=creditsPrepared_&&preparedClipMatches(creditsClip_,creditsPcm_,creditsOriginal,2);break;
    case 5:retained=&hallOfFameClip_;valid=hallOfFamePrepared_&&preparedClipMatches(hallOfFameClip_,hallOfFamePcm_,hallOfFameOriginal,2);break;
    default:return nullptr;
    }
    // Deliberately independent of latest musicSong_/playbackRevision: outgoing
    // source stream slots survive replacement while this same Main keeps PCM.
    return valid&&retained&&sameClip(*retained,clip)?row:nullptr;
}
const assets::OriginalStreamMetadataRow50KZ* OriginalUiMainAudio50KG::nativeRiderSourceCurrent(
    std::size_t index,const AudioClip50HD& clip)const noexcept {
    if(!prepared_||!coursePrepared_||!nativeSourceMetadata_||index>=nativeRiderSources_.size()||
       !sameClip(riderConfirmClips_[index],clip)||
       !preparedClipMatches(riderConfirmClips_[index],riderConfirmPcm_[index],riderConfirmOriginals[index].file,1))return nullptr;
    const auto* row=nativeRiderSources_[index];
    return row&&nativeSourceMetadata_->row(row->sourceIndex)==row&&row->originalPitch==1881&&
        row->originalSampleRate==22050&&row->originalSize+48u==riderConfirmOriginals[index].file.bytes&&
        std::isfinite(row->admittedDurationSeconds)&&row->admittedDurationSeconds>0.f?row:nullptr;
}
bool OriginalUiMainAudio50KG::nativeEffectSourceCurrent(std::size_t index,const AudioClip50HD& clip,
    AudioMixer50HD::NativeSourceSampleStart50KG& out)const noexcept {
    if(!prepared_||!nativeSourceMetadata_||index<1||index>nativeEffectSources_.size()||
       !sameClip(clips_[index],clip)||!preparedClipMatches(clips_[index],pcm_[index],originals[index],1))return false;
    static constexpr std::array<std::uint32_t,6> events{{1,3,4,7,5,2}};
    static constexpr std::array<std::uint32_t,6> milliseconds{{1538,54,44,491,914,223}};
    static constexpr std::array<std::uint32_t,6> durationBits{{0x3fc4dd2f,0x3d5d2f1b,0x3d343958,0x3efb645a,0x3f69fbe7,0x3e645a1d}};
    const auto& source=nativeEffectSources_[index-1];
    if(source.event!=events[index-1]||source.payloadBytes!=originals[index].bytes-48||
       source.headerRate!=22050||source.durationMs!=milliseconds[index-1]||source.pitch!=1881||
       std::bit_cast<std::uint32_t>(source.duration20)!=durationBits[index-1])return false;
    // Actual UIEVENTS flags1 ->233110 fields: mono, nonstream, nonloop.
    out={source.duration20,false,false,false};return true;
}
bool OriginalUiMainAudio50KG::startMusicLoop(const AudioClip50HD& clip,std::uint32_t song,float pitch) noexcept {
    if (!current() || paused_) return false;
    if (!ownsMusic_ && mixer_->active(musicVoice)) return false;
    //234900(restart1) replaces the outgoing music, including any pending
    //234440 retirement, before beginning the selected original clip at0.
    if(nativeWetOwner_){
        if(!nativeWetPlaySong_||!(nativeWetOwner_->*nativeWetPlaySong_)(*this,song,clip,0.4f,pitch))return false;
    }else if(!mixer_->play(musicVoice,clip,0.4f,pitch,true,0.f))return false;
    musicPlaybackRevision_=mixer_->playbackRevision(musicVoice);noteControlChange();
    ownsMusic_=true; fading_=false; musicSong_=song;
    musicGain_=0.4f; musicPitch_=pitch; fadeElapsed_=fadeDuration_=fadeRetireAt_=0.f;
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
    if((out.voiceCount!=requiredVoices&&out.voiceCount!=courseRequiredVoices&&!retainedGarageOrdinaryCurrent()&&!((out.voiceCount==garageRequiredVoices||out.voiceCount==garageSceneRequiredVoices)&&garageGroupCurrent()&&garageAudio_.quiet()&&(!garageScenePrepared_||garageDroidAudio_.quiet())))||
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
    if(out.voiceCount==courseRequiredVoices||retainedGarageOrdinaryCurrent()||garageGroupCurrent()){
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
bool OriginalUiMainAudio50KG::canPlayMenuEffect(std::uint32_t originalId) const noexcept {
    if(!current()||paused_)return false;
    const std::size_t clip=originalId==1?1:originalId==3?2:originalId==4?3:originalId==7?4:clips_.size();
    return canPlayEffectClip(clip);
}
bool OriginalUiMainAudio50KG::playMenuEffect(std::uint32_t originalId) noexcept {
    if (!current() || paused_) return false;
    const std::size_t clip = originalId==1?1 : originalId==3?2 : originalId==4?3 : originalId==7?4 : clips_.size();
    return playEffectClip(clip);
}
bool OriginalUiMainAudio50KG::canPlayOptionsEffect(std::uint32_t originalId) const noexcept {
    const std::size_t clip=originalId==1?1:originalId==3?2:originalId==4?3:originalId==5?5:clips_.size();
    return canPlayEffectClip(clip);
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
    if(nativeWetOwner_){AudioMixer50HD::NativeSourceSampleStart50KG source;
        if(!nativeWetPlayEffect_||!nativeEffectSourceCurrent(clip,clips_[clip],source))return false;}
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
    const bool played=nativeWetOwner_?
        (nativeWetPlayEffect_&&(nativeWetOwner_->*nativeWetPlayEffect_)(*this,clip,clips_[clip],1.f,1.f)):
        mixer_->play(voice,clips_[clip],1.f,1.f,false,0.f);
    if(!played){
        if(revision!=mixer_->controlRevision()||!mixer_->canRecordControlChanges())noteControlChange();
        return false;
    }
    ownsEffect_[clip-1]=true;effectPlaybackRevisions_[clip-1]=mixer_->playbackRevision(voice);
    noteControlChange();return true;
}
bool OriginalUiMainAudio50KG::update(float seconds) noexcept {
    if (!current() || !std::isfinite(seconds) || seconds<0.f || seconds>1.f) return false;
    if (paused_) return true;
    // Only the new ordinary8 reached Song6 leaf gains this stricter boundary.
    // Stage the exact existing float operations once, BEFORE even completion
    // bookkeeping. A changed gain records one mixer control; active retirement
    // records one stop. Elapsed-only progress records no mixer change. The
    // transport records at most one combined change, or zero for a true no-op.
    const bool stagedGarageFade=garageSong6LeafPrepared_&&musicSong_==6&&fading_;
    float stagedElapsed=fadeElapsed_,stagedGain=musicGain_;
    if(stagedGarageFade){
        volatile float elapsed=fadeElapsed_+seconds;stagedElapsed=elapsed;
        stagedGain=fadeStartGain_*std::max(0.f,1.f-stagedElapsed/fadeDuration_);
        if(!std::isfinite(stagedElapsed)||!std::isfinite(stagedGain)||stagedGain<0.f||stagedGain>0.4f)return false;
        const bool retire=stagedElapsed>=fadeRetireAt_; // current() proves owned active Song6.
        const std::uint64_t mixerChanges=(stagedGain!=musicGain_?1u:0u)+(retire?1u:0u);
        const std::uint64_t transportChanges=(stagedElapsed!=fadeElapsed_||stagedGain!=musicGain_||mixerChanges)?1u:0u;
        if(!canRecordControlChange(transportChanges)||!mixer_->canRecordControlChanges(mixerChanges))return false;
    }
    if (!ownsMusic_ && mixer_->active(musicVoice)) return false;
    for (std::size_t i=0; i<effectVoiceCount; ++i) {
        if (!ownsEffect_[i] && mixer_->active(firstEffectVoice+i)) return false;
        if (ownsEffect_[i] && !mixer_->active(firstEffectVoice+i)) ownsEffect_[i]=false;
    }
    if((mixerCount_==courseRequiredVoices||garagePrepared_||garageSourceLifetime_!=GarageSourceLifetime::unbound)&&mixer_->active(riderConfirmVoice)&&
       (!ownsRiderConfirm_||mixer_->playbackRevision(riderConfirmVoice)!=riderConfirmPlaybackRevision_))return false;
    if (ownsMusic_ && !mixer_->active(musicVoice)) { ownsMusic_=false; fading_=false; musicSong_=0; }
    if (!fading_) return true;
    const auto previousElapsed=fadeElapsed_,previousGain=musicGain_;
    const auto revision=mixer_->controlRevision();
    if(stagedGarageFade){fadeElapsed_=stagedElapsed;musicGain_=stagedGain;}
    else {
        volatile float elapsed=fadeElapsed_+seconds; fadeElapsed_=elapsed;
        musicGain_=fadeStartGain_*std::max(0.f,1.f-fadeElapsed_/fadeDuration_);
    }
    if (!mixer_->controls(musicVoice,musicGain_,musicPitch_,0.f)) {
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
    // Exact retained source/PCM ownership was checked above. Application pause
    // consumes no PCM, no queue command or clock;
    // it need not pretend a paused source group is construction/play ready.
    if(paused_&&garageSourceLifetime_!=GarageSourceLifetime::unbound){
        if(frames)std::fill(output,output+frames*2,std::int16_t(0));
        return true;
    }
    //No pending wet group may leak dry voices into a published audio frame.
    //Unrelated existing music/effect voices do not acquire a global wet gate.
    for(std::size_t i=0;i<nativeGroupControlCount_;++i){const auto& c=nativeGroupControls_[i];
        if(!c||!(c->wetProcessor&&c->routingApplied?queuedGroupSourceLifetimeCurrent(*c):
           (queuedGroupIdentityCurrent(*c)&&queuedGroupRoutingCurrent(*c)&&queuedGroupHistoryCurrent(*c))))return false;
    }
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
    //Never erase accepted controls or release/reuse their counted group slot.
    //Callers must retain the cold aggregate when its retirement preflight fails.
    if(nativeGroupControlCount_||nativeWetOwner_||nativeWetProcessor_||nativeWetEpoch_||nativeWetSetRawMasters_||nativeWetPlaySong_||nativeWetPlayEffect_||nativeWetPlayRider_||nativeWetPlayWatto_||nativeWetCanPublish_)std::terminate();
    const bool changed=garagePrepared_||prepared_||riderPrepared_||coursePrepared_||creditsPrepared_||hallOfFamePrepared_||mixer_||ownsMusic_||ownsRiderConfirm_||fading_||paused_||musicSong_;
    if (prepared_&&mixer_&&mixer_->groupEpoch50IC()==mixerEpoch_&&mixer_->count()==mixerCount_) {
        if (ownsMusic_&&mixer_->playbackRevision(musicVoice)==musicPlaybackRevision_) mixer_->stop(musicVoice);
        for (std::size_t i=0; i<effectVoiceCount; ++i)
            if (ownsEffect_[i]&&mixer_->playbackRevision(firstEffectVoice+i)==effectPlaybackRevisions_[i]) mixer_->stop(firstEffectVoice+i);
        if(ownsRiderConfirm_&&mixer_->playbackRevision(riderConfirmVoice)==riderConfirmPlaybackRevision_)
            mixer_->stop(riderConfirmVoice);
    }
    garageAudio_.reset();garageDroidAudio_.reset();garagePrepared_=false;garageScenePrepared_=false;
    garageColdPending_=false;garageColdMetadata_.reset();garageColdRoot_.clear();
    garageSong6LeafPrepared_=false;
    nativeSourceMetadata_.reset();nativeEffectSources_={};nativeRiderSources_={};
    garagePersistentBacking_.reset();garageEntryCustody_.reset();garageEntryGeneration_=garageEntryEpoch_=0;
    garageSourceLifetime_=GarageSourceLifetime::unbound;ordinaryOutputRetired_=false;
    finalNativePcmRetired_=false;finalNativePcmReceipt_={};
    prepared_=false; riderPrepared_=false; coursePrepared_=false; creditsPrepared_=false; hallOfFamePrepared_=false; mixer_=nullptr;
    ownsMusic_=false; ownsRiderConfirm_=false; fading_=false; paused_=false; musicSong_=0;
    ownsEffect_.fill(false); musicGain_=0.f; musicPitch_=1.f;
    if(changed)noteControlChange();
}
}
