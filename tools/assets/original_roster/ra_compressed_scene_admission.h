#pragma once
// Cold preparation only: exact original RA compressed-position admission.
// Default callers receive the existing strict V4_16 limits. No source execution.
#include "../../../src/assets/import_decode48c.h"
#include "../../../src/assets/import_scene48d.h"
#include "../../../src/assets/import_texture48e.h"
#include <array>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace ra_compressed_scene {
namespace detail {
using Bytes = std::vector<std::uint8_t>;
struct Identity { const char* name; std::size_t bytes; const char* sha256; };
// Raw identities from the authenticated original RA.RES intake. These are
// source-admission pins, not geometry, colors, decoded output or a substitute.
// RA inventory: 580 color-bearing PrelitCompressed scenes and three no-color
// DiffuseCompressed scenes (loft01, loft02, smokenull01). Existing 48D already
// supports their command shapes and single-node 00ff0107 variant.
// Static data evidence only; root must validate native conversion after wiring.
constexpr Identity wideScenes[] = {
    {"geometry/box01.psg", 1744, "2c8194e2a3176c3bd068db9a5ec20853d21720e5f242123b229e9f0940b04524"},
    {"geometry/box39.psg", 1504, "5669939f804bf5966e50ea9d8ad40925c1829cd8fb5c4822725b071e32e7eaba"},
    {"geometry/box40.psg", 1744, "16b3f3499618a34337f1b954141d0b34ccbc15cc94f024edf11e17e23ce6a717"},
    {"geometry/box41.psg", 1504, "49b021e8dabca8cc03c065048035e209b16b9099939c37ae03911d0f23ea9e24"},
    {"geometry/box43.psg", 1504, "883a0ce8f4505eb76e5ceeeda5d0a6935a7e24543b65015a9dc456a0f8f7d2f7"},
    {"geometry/box44.psg", 1744, "752dd17c8fe88c8082b1feb9f9dcbb41c354e21ef7705aad4c9d4b2c414b06d5"},
    {"geometry/box45.psg", 1504, "ca1a58ea7ac9a377944c0c547c114bfa7078b3989422da203f5f457b4903bef9"},
    {"geometry/box46.psg", 1744, "b1d857961e022cdeeb55c3c5488dce2461d28608aa60486a0f652124b18088d2"},
    {"geometry/box47.psg", 1504, "13dc9d88aca5ef5f8915cf40c64cff3e5759f9a456c953683d032dda11693288"},
    {"geometry/box48.psg", 1744, "7b909cdb543a4a479e1b1d8e43a5baad09d1c97bb6e416d9191d1d3d0f789e9f"},
    {"geometry/box49.psg", 1744, "dccc81e2e6c33f0020c240508884b8121268ac72e0571d951a7b7e5c5d7635f8"},
    {"geometry/box50.psg", 1504, "4f863da6516aa43240d5c12f400c60d225c29fc92a77d6ea7e31e533c6f38f61"},
    {"geometry/box51.psg", 2112, "6a4353e0f5f35bdc5faed48f993cf24f72f0595a1a64df58d4590ef1627c95f0"},
    {"geometry/box52.psg", 2112, "a726ee4142182d95f0e5003e2f12fc3fa1112df6a8bc7d5a5622e12053367808"},
    {"geometry/box53.psg", 2112, "bea2c515fecebcdb3cf7e804eb19531b8f40850819d0c02bb42f66106d0ba9ef"},
    {"geometry/box54.psg", 2112, "7e74a4eedf3f7218eab8778e97d4fd9fa94c07092c5ea5b9c5598847a90b64a1"},
    {"geometry/box55.psg", 2112, "6fc6ffbe993b310facb95c88599e205969fe02c1ad830db828d76f310a29578b"},
    {"geometry/box56.psg", 2112, "42dbb468461b4361ff367fa6544939a74f61f722ba835d393286614231019bf7"},
    {"geometry/box57.psg", 2112, "19acd075f497c0453007e390ea280865a209a0ecc3e64d4ace66603dfda965f7"},
    {"geometry/box58.psg", 2112, "75e0e346a3ea5873cdf542eb62fbf2cebb8baaf87fc3f55c5ad9150c1a2790b7"},
    {"geometry/box59.psg", 2112, "d33fc9c1b71bf5a71a73658303261b07c591f0dd0cd70af8353c74eaa50374bf"},
    {"geometry/box60.psg", 2112, "00a95f99bdb7381d339de84cf71fb685d3e269708939a99313c0e4d7236cf795"},
    {"geometry/box61.psg", 2112, "c5d20fe620e5d1625d48c777681e7ba09c45878d67042c40104ef3f9da505d1f"},
    {"geometry/box62.psg", 2112, "83a077671a69586ffe89149b760bfc0906b6e352cfdf527530cfd25cda4f0ba6"},
    {"geometry/box63.psg", 2112, "fecaeb998ebbeba1dbccde4a5c366f52d12e3580112a28f71ce7bfb5f1e57ee9"},
    {"geometry/box64.psg", 2112, "7d8b0c71abaeac518ba398f7a59941cbac29f68e2f866b1d1e3b3bb9ddc8e6a6"},
    {"geometry/box65.psg", 2112, "4d927c898152fa0344da4530127bad264638a803e233258f80041d4eedf891b2"},
    {"geometry/box66.psg", 2112, "d5d60ef847a798d43a1d04b9b1045bcbb70fc8c5707c5c4c4f37836099acea08"},
    {"geometry/box67.psg", 2112, "b8f19691155aad131ba6129e9987d6629210b7564dfdf7b8733448a1f0732ee0"},
    {"geometry/box68.psg", 2112, "2484c85083b727ecd10dbfb6f1a8100e6ad6730cc84f904238cc61fc33813059"},
    {"geometry/bridgecoupler.psg", 992, "946ffd0716a25850441aaae6e3f792c2bd49cfd37b67165eb6b26ef3315b3775"},
    {"geometry/bridgecoupler01.psg", 992, "be8b317018ea40041401e967118b5b5f062965a9064dac3d7ef47e1b54a67345"},
    {"geometry/bridgecoupler02.psg", 992, "494beb45f4a119d58be1df29526a0ae66246fd6d16ccfa28da224c0215d68fe2"},
    {"geometry/bridgecoupler03.psg", 992, "9f02d01034000e23e9676779a3f2bd602a328a7c2407d0deadb835d3ff10f077"},
    {"geometry/bridgecoupler04.psg", 992, "8333b5bc0090b6c5c6d26fe30ba93a06786a12aa5bd00dc68142d9dfcf6b9474"},
    {"geometry/bridgecoupler05.psg", 992, "72e514880627a95877b6576c5a60a623a538072c8fc30ef673c63607eac9c361"},
    {"geometry/building001.psg", 1216, "517ad29fda314fca1a4e34d900e49890e4988be84564502e0f97664e22d05e26"},
    {"geometry/building02.psg", 1216, "73dfa177615c591708520460913b49f3f7da0c88da61f165a9f7c8ca7999747f"},
    {"geometry/building03.psg", 1216, "975c616065be13198a7eb9ee90c22452687f38d888aa86382a7545d45c56ee5f"},
    {"geometry/building04.psg", 1216, "ecf305eea5b7405b64e35a03ada2db5a4a9b5c7b9097d5f386fc8eb3499d5171"},
    {"geometry/building05.psg", 1216, "0a040f1e9abbef759265296b2a69e2027aa50bc66f631b8a6ecb01cdcfbff2a2"},
    {"geometry/building07.psg", 1216, "ff65ea222840ddd63d83c5da5a7e3e8f04d986a7bb7cda5357ec764c90abb533"},
    {"geometry/building08.psg", 1216, "b3a077a6cc17e1375d842604438ab6e74565cfcdbc03a5ebe142064a747b2e33"},
    {"geometry/building09.psg", 1216, "c5b8767365f03cb62c2dd649cecf825bbad0da7b1f9b9c9c860ad1d60d8cf464"},
    {"geometry/building13.psg", 1216, "7cfee7b09007741e3951d873ed826d59377de795c6a080a2e64aa4f06983efd1"},
    {"geometry/building16.psg", 1104, "7a938e0d9ff78567afacb2afcf6f545cede77f923457c8f693c0049c30afce0a"},
    {"geometry/building17.psg", 1216, "a26037010bb20ce882db0734ecd5c7b5ca0417d5adc73661d8cbd94b4d651f32"},
    {"geometry/building18.psg", 1216, "40eb3f6f232db87c0552930f3ae35e869a8dd61243cacf2eb6af8d345bfbc89e"},
    {"geometry/building19.psg", 1216, "9b3e5e67ae6b36754138220c29435717048ce9b576ba983b855a359b2ac5b582"},
    {"geometry/building20.psg", 1216, "2d3cedc4790d361927d4d97021848c6821fe6595cd649d5decb13bf824e7131e"},
    {"geometry/building21.psg", 1216, "fa6f483e3870dadd7c97f674c5c04bf76eebcd25fed903f11934406d7b9be471"},
    {"geometry/building22.psg", 1216, "1d32cc48b779f734267ac6af0bd3a123108afc2e2a6ccb3c3346c9b7ce083452"},
    {"geometry/building23.psg", 1216, "3adf731d4e55eb6003db4770c5244f6b4ccdc1378c825673acb81f74f408dd1a"},
    {"geometry/building24.psg", 1216, "fa051ac59387d27796c27391b02e572624d2ec7150f4a45f944ca3f609f4c34d"},
    {"geometry/building25.psg", 1216, "ae1bac95cc10e4c3ba905dca6d353e5d0cf638e6e079768980e44a24b8b46dd3"},
    {"geometry/building26.psg", 1216, "cc5028ee090a8c9007c29ccf24cd56b92223871e2a2e11dd64dfe80ca6c7f055"},
    {"geometry/building27.psg", 1104, "8653c66a3b4c59f1f271535adabaa7585df436aef04e7a85b78670fa86de0671"},
    {"geometry/building28.psg", 1104, "499e035c39b0a613be2f1ea93a9e1c5568d64b4ac9a1ca5b4be57577385bca47"},
    {"geometry/building29.psg", 1104, "bb9f378fe9e2b39c0204b83e1fc3019bfd116d02ecee63ea3510869cbf587cc4"},
    {"geometry/building30.psg", 1104, "a7855df1d02adffacc2972739bb675f333bf64e233740cb6a42239f3bdbc2c4f"},
    {"geometry/building32.psg", 1104, "6c58251356e75c791fe5e62f38eb2149d4db5007abd9fb0779804fea0f7b9e11"},
    {"geometry/building34.psg", 1104, "af22b1a4145309b8d5db0db7412c55f1187716af36c8fa8bc98be0974da8c938"},
    {"geometry/building36.psg", 1104, "267e0e20b05057c24d57e126b3cda96faa4b84e3d3c48fdd08477cc552733784"},
    {"geometry/building38.psg", 1104, "87ad4cb5056e98a8da05f622e73535642a8d6814e028ca14d0c3bafbc15c8cee"},
    {"geometry/building41.psg", 1104, "25d5be742d8c3548e8f2760d1329704b395847c294fa86dfcfc8575723ba3a01"},
    {"geometry/buildingcut.psg", 2000, "9c5897f2c255605156165e10cbb31725e302af5ba9319049435eac1ec0dffd71"},
    {"geometry/cylinder01.psg", 1136, "d9b5831220942e3f79115357ad657c05b65ee4929514f9b3961ac2cdc32dd603"},
    {"geometry/cylinder02.psg", 2336, "110df3130291693fa74c3c131176759f9a972d5bb5955764227efb3f71569138"},
    {"geometry/cylinder05.psg", 2336, "01db0b043ab0d2d0a7f438ae0b544713137bcaa08b5a76c60fedf6fb8421ce7b"},
    {"geometry/cylinder06.psg", 1136, "55b026d0c20fef361b8d43a6015064772b622bae4c078e546203d90f42be3388"},
    {"geometry/cylinder07.psg", 2336, "978f8557182bd6e638645ec5c2f950a62ab01e56370057efa440eecc8731dbd7"},
    {"geometry/cylinder08.psg", 1136, "624003c5a6e60b476b5a487b35ba522b603e48eac64e9b3cba5d052d8e685747"},
    {"geometry/cylinder09.psg", 1136, "6171221e522f58c7525004b3d1bfed2833b3dc1d29f93ed2646b507e7a93d1be"},
    {"geometry/cylinder10.psg", 2336, "67e65b17daadbc0ea5c68c09f62357e28d796621ccbf18266dddfb64814effc9"},
    {"geometry/cylinder11.psg", 1136, "811231a5dbe07a186a2f6796738b650163d2f60468f3332db745e3b40f21a348"},
    {"geometry/cylinder12.psg", 2336, "caf3c8129ef13838d2a9c1b2423e5de73f819a15378e1966ec2f1cdd080e8faa"},
    {"geometry/cylinder13.psg", 2336, "e3785bdf6421d4a85a4f123a89dcf8967e421d7ec39c5914f1adcacc42146967"},
    {"geometry/cylinder14.psg", 2336, "fcd2e9595777d2ffc0cc65fe9fe2182aa0b39f16014455be02d4424f709f7d72"},
    {"geometry/cylinder15.psg", 1136, "4c6b54a2158a06be18ad7d60e6caca3446c2ddc37be7730e6af31b479871de09"},
    {"geometry/cylinder16.psg", 1136, "762dd036611e9c87a7c145821b341a37dfdf50655ddd7ce7c7add2292b5fa795"},
    {"geometry/cylinder17.psg", 2336, "c9fe613d4d58648cd536f5d9b4172a86703d99fb0721ef91037710cdb054ea56"},
    {"geometry/cylinder18.psg", 2336, "9e723536ca1ae37fe1c38715a41a277b68a27d6ebee494f020babae1e0d3d01d"},
    {"geometry/cylinder19.psg", 1136, "fc08f461e7a47d92a15e96f662d031e73f018e2a4806c28af4e9553db68448fe"},
    {"geometry/cylinder20.psg", 2336, "12c3a6d70a5b56e3cf4790d83ce1904a54adba3c68a9ffca3977f5a4c6a08ded"},
    {"geometry/cylinder21.psg", 1136, "68311ff9a2ba5cbe65720ab3c666e1605f0b2403c24f194d64a018435c5a8c36"},
    {"geometry/cylinder22.psg", 2336, "b7c4eee7f361394c0d78f624d53ae3213bfb7508a4a920cafd87232b75c5e9c5"},
    {"geometry/cylinder23.psg", 1136, "e5fc8849af9daea4803473cdccb6124f0b3ee98a0295c3c0cbb5717f2fe6bc86"},
    {"geometry/cylinder24.psg", 1136, "e2e7e86d1c94bda3aba8f7138c00247e199413bb32479daa38618addefeed1c8"},
    {"geometry/cylinder25.psg", 1136, "7310546ba3fd60411a6a221d6a9a0dd5259444c2fbbee249ef898f2055a84a7e"},
    {"geometry/cylinder26.psg", 2336, "78025fc186e79f709cc6a774c9ea144cf3902280c3568f96d82e6416bf1dd14f"},
    {"geometry/cylinder27.psg", 2336, "8f2d86f2c8b347518e2acd9c80010c679100267820dc022f154dd2b69b1209f1"},
    {"geometry/cylinder28.psg", 1136, "366ba0fc0bbd5f7d42da6899e22dca1fbe6c0e1f0f48d6e0df2f40614c62d7e1"},
    {"geometry/cylinder29.psg", 2336, "d8b4d253482b9dbbcd9f5dab97d2063b1b2b6227f96ef3554b3ee212053e6adc"},
    {"geometry/cylinder30.psg", 1136, "23f9ac8da33ecb035cf4d47bfac77c7492b7538933d284073d1d74ff2132574d"},
    {"geometry/cylinder31.psg", 2336, "b586d57b389111d541483cc2140fe3f72ef2911427cbcfe861f607ec99714372"},
    {"geometry/cylinder32.psg", 1136, "2be6a784bb5d8ef4759cf4a92bd1d9e2bf27547765ef05d71c89bcb2296b9c18"},
    {"geometry/cylinder33.psg", 1136, "a17dc3e4530307772eb1aad239ed12734b06e3682817276889430dfddc910fb3"},
    {"geometry/cylinder34.psg", 1136, "a289b6dd29b2308a8d36f0eecee9a3ed3ab126d887d1098fabc86e0ab214a796"},
    {"geometry/cylinder35.psg", 2336, "ecc12c40513e2e17e6f59f5ace1d33d85b88705f7734b316fe53baca0f03ebf5"},
    {"geometry/cylinder36.psg", 2336, "4e173ceda0e765aede9a45769fd16a58c2fc0ca1f4d3dc4bc9e3a4e86c4aa8c5"},
    {"geometry/cylinder37.psg", 1136, "080fba593171b3178611449c5a0952631b823f85b44994ee431dc3cf386a1d1e"},
    {"geometry/cylinder38.psg", 2336, "e7da37953dd264b1b35bc0b3902ceb0d5aafa887d96c93eece86c823c42f1098"},
    {"geometry/cylinder39.psg", 1136, "8a30cb8ce0b58d51c288f91d4997e65a96546eb411dfd8140b10b0f98bdc98e9"},
    {"geometry/cylinder40.psg", 2336, "f464cd7c29d2f3cac6c14fc41848440c77e4f38c3ced31e3c12241fb01c5369e"},
    {"geometry/cylinder41.psg", 1136, "2f7748409dc72c5c94b5bf22576bf0dfc3da373a98549e50255084e315a6e75f"},
    {"geometry/cylinder42.psg", 1136, "782e96b30c167f7d9a8745ce4c7be62f91400a98f61a64efb5f1e645b3728070"},
    {"geometry/cylinder44.psg", 1136, "1a2f7597198fd7f78b3ec4936066a8df52035d12b7b61d2068d559ec99e4bcc6"},
    {"geometry/cylinder45.psg", 2336, "d8d120580d6a3813b111402d8058e154de0b4ce7eb95b5979917fd65b8517d87"},
    {"geometry/cylinder46.psg", 1136, "8dfd52834ca493cc4d10285171a1a9b886e6942c3d3c625ca8205fcb6c8479a8"},
    {"geometry/cylinder47.psg", 2336, "407fd83ef6c7ffef5087956454c0524dcb1afafaaab2939f53139f6d78bdd08b"},
    {"geometry/cylinder48.psg", 1136, "a08b94b10424e56e6faf11db05b7e655483f79431d1466a708e0793e175c2202"},
    {"geometry/cylinder49.psg", 1136, "fc42007bd953e79b6d7c3eccfb4c99260f64703365533f64499c8d3de3eb636c"},
    {"geometry/cylinder50.psg", 2336, "37c5067701171f2c1a0f68b87cf560f8f2b0cc050f93646306718e6c3ae04399"},
    {"geometry/cylinder51.psg", 1136, "2e0c47b3cb03a91f5dcb7fc55ba4d1a82e36c9e153ecdc43c793703cad08c680"},
    {"geometry/cylinder52.psg", 2336, "e202f01c992109be81b4a41510544423e083c3b8728dfbcfc020b8a355020e73"},
    {"geometry/cylinder53.psg", 1136, "7a6e311b940c6f7fa78392b204e6a75c03ca0f79defbad575c00d3aeba78c541"},
    {"geometry/cylinder54.psg", 2336, "fad97d0e9249a13f900f842c175682e91f0ebfed79a66de3283be8474753088a"},
    {"geometry/cylinder55.psg", 1136, "b5a03f9c3dc65eb85b0d6e3fc3d007b260621cfe4a00e0029120371cd05b0b92"},
    {"geometry/cylinder56.psg", 1136, "b41cd85548912c66a8a1621b5536a0ad1ceaf3509c784d94ee830992f882e3bc"},
    {"geometry/cylinder57.psg", 2336, "8459bd9f79608a5f6c6e28aa8a58e5c6e93421112ec91e7e01f0ba934038fd73"},
    {"geometry/cylinder58.psg", 1136, "ccfd079a940d0cf58f0a9d654d43d814fe9124aa7b8d166f2e43a595e67d0651"},
    {"geometry/cylinder59.psg", 1136, "d8060d887cc813402a58e0c09ab0b458f0145e59aef7eba9925af76df79141cc"},
    {"geometry/cylinder60.psg", 1136, "547a2f52386bf0002679fdc952ceb3a0597780a20ee5b08178b4b3383f74cb47"},
    {"geometry/cylinder61.psg", 1136, "57c2ba208a0f0c8eb2f350217a93a9cf3ea7c745fc6dd3e01280a1453b20ac6f"},
    {"geometry/cylinder62.psg", 2336, "0bc7bc07f882877d9fa9a109364b9ace5c211abe9c58a0ffb6aaa49eb4e48161"},
    {"geometry/cylinder63.psg", 1136, "d6756afa642eb97438b9f7f07dda717f82cfcb2cf31020a96a4f5dd585aa9110"},
    {"geometry/cylinder64.psg", 1136, "1f4090c69922a768bc47c51c9e81c482dcc513e104d3cd812d1b837073caba8e"},
    {"geometry/cylinder65.psg", 1136, "c3232e3839b2efeb0e8ded3fcc34f2a8c752ccc5cc586322c80a52eb603d4c9b"},
    {"geometry/cylinder66.psg", 1136, "d0d128f20a88a37f1dd09c07de022967992bddaa80703bbc1f06648bb278d361"},
    {"geometry/cylinder67.psg", 2336, "3b3e4c057b2edde2555c1cb5a666bcef8e9b1bf34546250b170fde9575b68fab"},
    {"geometry/cylinder68.psg", 1136, "aee5ded360e3540efa3853b18b91fed8589b4fbc665b618bae9b80a94f699e49"},
    {"geometry/cylinder69.psg", 2336, "74e7810d31a826a93b8b138f958d1ee19455e20867ee817c5d29ed14d1b965ec"},
    {"geometry/cylinder70.psg", 1136, "9077f7ce6ddbc00c6856c7c0b75d8794b4d007b781dd57612c2916cca47319ce"},
    {"geometry/cylinder71.psg", 2336, "d8016247d894cffe69a8db96516f17e5c4bf143e5097ec5dda11e0a16771ceac"},
    {"geometry/cylinder72.psg", 1136, "923d1258703ee1c8a2baf9439ad23f639d44a10848999f159e816840fe8f85e2"},
    {"geometry/cylinder73.psg", 1136, "f732368af8e93b0245a7dcb40ff691d7ae35fbabe9b5ffab8998d194174977fd"},
    {"geometry/cylinder74.psg", 1136, "f00ae2646c98107382cdc8b59fa4dfa5c20dd4e25bb8efdc0ee398d998d75a43"},
    {"geometry/cylinder75.psg", 2336, "43ed901300f3b7286d2fb848a56900470dc89e26c85686b4aeace060997bf608"},
    {"geometry/cylinder76.psg", 2336, "90bc10d4009156369324ee35964485556961c8a6691fa5026cab9f4eb7bceb97"},
    {"geometry/cylinder77.psg", 1136, "6185449bf4b9e3bb6140964c60f7d0268db526119dcf6a64654d13d22bfdfe44"},
    {"geometry/cylinder78.psg", 2336, "5001c5fd09bc0adc24d052e474e010c9c4516a08ad103d653e11a3eff33f827f"},
    {"geometry/cylinder79.psg", 1136, "adbca69997feae75652b9269709e14db3daed17b09d955456bd6ba6a00514447"},
    {"geometry/cylinder80.psg", 2336, "e9894b9731e999623c2bb1b7b8eceac9486ffca10151f539c2f626f6f04ddf83"},
    {"geometry/cylinder81.psg", 1136, "5994a94a77471a7b59e27f7e4f78fe879b5375894d3f4d873da3063a38a3f710"},
    {"geometry/cylinder82.psg", 1136, "07fe9c101153f19d397df7070752fe57ebb62ac9d24a0ddb41eaff71ee53970d"},
    {"geometry/detailbuilding01.psg", 5728, "77add84e7783d157b5bc65dfdaee9ebdc1077d5dde1c367f1594518918e95fe6"},
    {"geometry/detailbuilding03.psg", 4080, "531852fc1455849dddb6c56789ce065080de9c4c6be21f7ea04fdd8088784013"},
    {"geometry/detailbuilding04.psg", 4080, "d2348ec2fa18d855da975abec0748fd7557f52ab47e78e5a8afcf808ccc5136b"},
    {"geometry/detailbuilding07.psg", 5728, "51ca64ace6c8e0c87ce4e5590b276873e650cb57d29950bbd02c51b8c9fdc718"},
    {"geometry/detailbuilding08.psg", 5728, "5b085f86f7a630cf48b97e44f08d6886468924afbc94be75cf3240262befcf03"},
    {"geometry/detailbuilding09.psg", 5728, "4b50d02a0737be2ad9137299a1b81ac9847e379eb16f8a02e500eb4a9db1539b"},
    {"geometry/detailbuilding10.psg", 4080, "616d3eef0a4193d896ba483ab37db5cf3ad401e88a2dd042bd57ee4f28ed949d"},
    {"geometry/detailbuilding11.psg", 5728, "568f2a59adac3c8d340d3ab619164c36b41f7232d65c17c40e1d9fc73d0b6b58"},
    {"geometry/detailbuilding12.psg", 5728, "533e23b4379775d672f93bd4525189edfe4c0b18df0a3ebc37c3db7ea6827e40"},
    {"geometry/detailbuilding13.psg", 7344, "cc7479d6ee515f0c9328269274f5b64992c694fbe98bc0421aab5829b8695c23"},
    {"geometry/largeflag01.psg", 2880, "fa955e22e1381631b097484813c8f9fb0951d1ced3f9f6df69b188b322965cd8"},
    {"geometry/largeflag02.psg", 2880, "12c039359604b1f894c1ac4859d483069e90c41483c65010fa0fe121d569f822"},
    {"geometry/largeflag03.psg", 2880, "dbc1ba8883aad91f840b246627398570192a55c7d4422de7df34c5cb6e7787d6"},
    {"geometry/largeflag04.psg", 2880, "1279e4e359ce805dbfe80e5bf6072285af69ebcfc7649cc0a11451dbee0ea218"},
    {"geometry/largeflag05.psg", 2880, "d969e1da9bfcc071c5f64599271dd02496f2d47366e305c5f9f16613f3c9ecc3"},
    {"geometry/largeflag06.psg", 2880, "24de11176dbaf62c6a7408ce2e41d8619de25f545434dd8305937961051a7449"},
    {"geometry/lightpole01.psg", 1312, "70ae52e0dc648a08e68a1a4e582fbc05093086f82158b241fb5bfedf6edaffde"},
    {"geometry/lightpole02.psg", 1312, "ef1052908fba6cca081c40bb6bbd301a5204cd271bdfa3442d3ac7795f0e95e3"},
    {"geometry/lightpole03.psg", 1312, "52837201b7ed03c8362a90547a90b4373e97924fc718e65b92bd1fc8d5ee8506"},
    {"geometry/lightpole04.psg", 1312, "87e272ae463a3b3d6444ce7141b3e32a30168c7bad4291595c56b5092dbeee81"},
    {"geometry/lightpole05.psg", 1312, "1ee4a8a2102a92ebb23e2b7d8842667bfbaa4888387b78f98a5b9e8e5b061301"},
    {"geometry/lightpole06.psg", 1312, "960b81f6b643d8b1cf2dac187461b657717cd3f260988c6008419335372f246a"},
    {"geometry/lightpole07.psg", 1312, "26b89c8944f992e35f2963d37c085b2deed308fdd78b99cfef8dd95df716a02c"},
    {"geometry/lightpole08.psg", 1312, "e9189f4101702980349b686f629e09cc496c700ea38913847bb99908448ce6a2"},
    {"geometry/lightpole09.psg", 1312, "e6089baccb7a8b2a0452b03683fb438ed991d6e8c42789a7e75c1c6c012c2c8b"},
    {"geometry/lightpole12.psg", 1312, "019a511ae9b3233e2155857631eec6b759299fd70f5cf0e607ccf596842558cc"},
    {"geometry/lightpole15.psg", 1312, "d3e74338be462db9f769c6bbed51a7e003b24e3c7ad72c7f14d796cca3718f35"},
    {"geometry/lightpole16.psg", 1312, "3191a2408fe1e78064879a9a06ee6b94fa38958cba8962030cba0695aaf54399"},
    {"geometry/lightpole17.psg", 1312, "7df405d28975a62d414a59a80038e2f625d6ece77ec014bc44fbad959ebd3885"},
    {"geometry/lightpole18.psg", 1312, "6b1a31930d81320610555f8bb32a011abaf3891148e58eb64ffc72e0b74c579c"},
    {"geometry/lightpole19.psg", 1312, "4421562f48dea404c35480cbe4fc71baeadf2472102f2ec89c106eaefe223d6f"},
    {"geometry/line05.psg", 2160, "a1d49bebd40e82f12a30ab1a942da4b26db2c6031a0489675d2a61fd541ae9c0"},
    {"geometry/line06.psg", 2160, "526cbd0fb8e6ebff37577488af5565d3cbf41747709183c4604535f26ff6f0dd"},
    {"geometry/line07.psg", 2160, "bdab33f181622dd5facd5f8dcd90a3233a214a8e4430fddf27e44b809438ddd6"},
    {"geometry/line08.psg", 2160, "8fd488df9fe20e5c45ceb130f9a070a1cc530e4c217bd0a94136a4ba44f5c026"},
    {"geometry/line09.psg", 2160, "7a329fd04dc1f6694184590b96db39978e7f4a798612bd5f6034ec2aa3f00802"},
    {"geometry/line10.psg", 2160, "43837c7fdc9346b9d3b9c057e5cda1d1c7d6bb9bf6da1074322fef91d67d9be2"},
    {"geometry/loft01.psg", 1968, "592300e0deacb20b957d13a7d1a1b3895b53d53cb3fe98a8ed733444b443b1e8"},
    {"geometry/loft02.psg", 1968, "16e7f4a0bd5c9cdd993e0551777e9fcdf801303343788d18e0b7aadc1ccca351"},
    {"geometry/loft03.psg", 4848, "55ca9222da5ad2013bf5122f6ff4a2931a9a079aff73b3eb654c94874ecb179a"},
    {"geometry/loft04.psg", 4848, "05abb92e6be311b127dd96748ca5370c34c0e284a2fd71fcc58974f5f6f5a50d"},
    {"geometry/newarch01.psg", 2160, "5f786cb2b523bc4fca3e4955c2d7094d8ded4c27f9a846ca13b3090075050056"},
    {"geometry/newarch02.psg", 2160, "7c50f7a84d8a873b0e4720c1ec2dcf45fb2560c51469be61468ae657781db933"},
    {"geometry/newarch03.psg", 2160, "72ec68c700fdb186da225d0f05fdda91e42ca9b5b2722243f77d7c7a7c76adce"},
    {"geometry/newarch04.psg", 2160, "3fa698b7bfb580482ccb47bfed61c7f50dccac73e8f28105e6b5581628e8c03b"},
    {"geometry/newarch05.psg", 2160, "f8cdc8e1120142e5734a5c17099cae321ba97f7988aec235385106f53d3b45ac"},
    {"geometry/newarch06.psg", 2160, "ea5f5bbc6136ce207b6d5223a6253b816e96a390ecee636ecc8ab46f20142038"},
    {"geometry/newarch07.psg", 2048, "6665e693f55cb953ef0e895ad3b030f31be78551017a69f60d302b7564208699"},
    {"geometry/newarch08.psg", 2048, "c6cdd4f9ecf89b7a2160422c76885cf77bdbff5b0721e3548e38a7d048ab52c5"},
    {"geometry/newarch09.psg", 2048, "eaed08a45f7fd645bb30088fdb24cfdf57dfb0f063b47bf931c1c359cc81f92d"},
    {"geometry/newbuilding01.psg", 1216, "6b4bb24a567b2b52690ff4a30c2422c978892432373ea36aba9fb24ef5f500a8"},
    {"geometry/newbuilding02.psg", 1104, "97254d8466ab8c89ef98ffa41de2aaa428b8a41241a3c464113ec21f44d1f3a9"},
    {"geometry/newbuilding03.psg", 4080, "adb46613804798947de4249ef1245523e286fc2238ffa0570973beafb7c14751"},
    {"geometry/newbuilding05.psg", 1216, "c14d7e8bc445623109654210abc3ded4845ed10364d8fb4f354107b1524cc5fb"},
    {"geometry/newbuilding06.psg", 1216, "2e93e953c034d8c7217edb72f800df3c3142c6ea28daf80a978d5d32701c2a53"},
    {"geometry/newbuilding07.psg", 1216, "441462cf4b02bdb790fd9e902995ec8866b91c40534dcdd03f1785a6f56f4e51"},
    {"geometry/newbuilding08.psg", 1216, "0cdb9b10641a73c0c993b1527c012434adca793f4d4dffa8291aad44b5de9cf7"},
    {"geometry/newbuilding09.psg", 1216, "80cf3e18e8492cf3d3a946a1bdb73e32db5beae233e09e72dcf9ea462cd40938"},
    {"geometry/newbuilding10.psg", 1104, "b17c8407c57376f8a61a980b983713250ec1c470009807269fa809d52a5b7844"},
    {"geometry/newbuilding11.psg", 1104, "be43be1c39d6c16b6e936ed35e0ae460cad8faf6684370767dd76b5558a1a69f"},
    {"geometry/newshape01.psg", 2032, "17f04e5ccaefba011f2344b6d3bf875ef49900184783a3e0676f8ebec885f9c7"},
    {"geometry/object01.psg", 736, "d60c685aa771b968e153e129306b97c65e81b3e6c42af3fedee5cc03a2798fc3"},
    {"geometry/object03.psg", 1216, "3813b558fe6b9af3ce8b4434c9d6db1af449bada72c0439294b48e141b0c920e"},
    {"geometry/object04.psg", 1840, "9edb5c78f617096026daed2d35859fe782e1115ee0c8a4ea394a9278d0aa3251"},
    {"geometry/object05.psg", 1216, "0d5c3097e589ad782d031b09d0aa6d9c2b2024842e680862ee63358e0a4fd70d"},
    {"geometry/object06.psg", 1216, "80fcc04133947aadaa927bffc1dbd84ce071d16ba19d97f5fd5aa13d1dc8d5ba"},
    {"geometry/object07.psg", 1216, "3a97fdf91674abc1e11160b7290584c9b5b9ded27fd6989c49802fdc3ae0621b"},
    {"geometry/object08.psg", 1216, "ec1fd38ce507c3c6472f05a57ffc15b905f6b6f8833540357b17e883cabe01de"},
    {"geometry/object09.psg", 1216, "65fb6a902c26826e0688f056d57cc7c1eb2951203883f58a9986f91750e9d329"},
    {"geometry/object10.psg", 1216, "a2a5ad17f09bd959a8f61e800c64aa3ed6d12aeafb51c8470adcc153e65d3f3f"},
    {"geometry/object11.psg", 1216, "20f15f5667db36f1a4346597f441843713ae5f79f780e25b0c3a9db857d1cf50"},
    {"geometry/object12.psg", 1856, "8f14c604470f521fe1fbddb5b1bcc673923ae05bca56a378efedefccd33acdf7"},
    {"geometry/object13.psg", 1216, "5c6be370d5a0382c3a47c173b47a77bfd0c85c56710987c73625915106980dbc"},
    {"geometry/object14.psg", 1216, "0915273eafa06db401003492de709f63172d7c93f85dd00fc9abfc35830bbec7"},
    {"geometry/object15.psg", 1744, "3546ddbb31d316dad9d7efdd0b4e90037b90fb6839a1fe9fbb143c7516f0a975"},
    {"geometry/object16.psg", 1216, "ca8cac329cdc3a1bcb6e7c57383d6301bbd1cdb7402e09f725d860f766a69ea4"},
    {"geometry/object17.psg", 1216, "b0cf90553f4616b5c71a27449235369608e17c1f1c8ffcb796cd8455cea628cd"},
    {"geometry/object18.psg", 2032, "8fd5a75a4bd1a026e9b19abb43e5e565cf654bbe7ad0c33ceba9d550390093b0"},
    {"geometry/pipe01.psg", 736, "1544be1f0d0c37d3b622ecc804dc880041218e8cc70bcf60ef4e3abe708506f8"},
    {"geometry/pipe02.psg", 736, "ee82a7bfb4153b19cc381c20222784fd722b1bfda1f83a5bb4af499c8c7b4bbd"},
    {"geometry/pipe03.psg", 736, "cc96f6e5b9f826634f8f06d7f57096afda84fde68c9911f1778b0425d832c811"},
    {"geometry/pipe04.psg", 736, "fe07c0d6a37ebd346b629e3e24e3c20ec35ae2a6a77c5e89fada98e46e7ec0cd"},
    {"geometry/pipe05.psg", 736, "3b254b743e857178ff62d7baa52f9c9c26a323aea7a667ee0a7621474b44dec6"},
    {"geometry/pipe06.psg", 736, "e6f5167c0a1dbfd21ad7abe7f516341e361c2e11d3e7f0412ad2c992816d56f4"},
    {"geometry/pipe07.psg", 736, "4fb48040232bb4389abfc8c0ecc3d6d8dcf87afae5e43738568e1df63b9ff76d"},
    {"geometry/pipe08.psg", 736, "d222b820002d626d9350d43100e318b8335d221d9774fc2ecacf5d09c07dc5a7"},
    {"geometry/pipe09.psg", 736, "bb387bdbcc3c990f9abe96ff06378bf743347a04378ede4830346f9a79a4b71a"},
    {"geometry/pipe10.psg", 736, "af181332d043959df35af9b99fc7500ad8d531db8f3bc080bb478f2727e9eaf3"},
    {"geometry/pipe11.psg", 736, "00ca47304205ccf219551e618f31cadf270494e295b42ded083ee9fff3aa2953"},
    {"geometry/pipe12.psg", 736, "9adeec0c7ca3884c20c8100db1ba26e19b4944adecb5bb0a5ae479404d8e7278"},
    {"geometry/pipe13.psg", 736, "618e094b86f685ac24df40b4d8552a971af40344ee2f215ef45485c0e651d726"},
    {"geometry/pipe14.psg", 736, "3ae6ecdf0c19dab66c816768fab9994b7e1ed60e51a2e2f5cc5d77749e1fbe25"},
    {"geometry/pipe16.psg", 736, "d5126bce1b979f536e3d4ecd0986a819a797bcb0e72c72b45788d275b51bc9e9"},
    {"geometry/pipe17.psg", 736, "ad9bc15cbbd7d4cc83079f67501a5382055770e63004ef2da8c6c6df054fe77e"},
    {"geometry/pipe18.psg", 736, "c5af27c297e22a1fbc42a29a19bd1989ac6d02d691f8beb9dde96ad16833b83b"},
    {"geometry/pipe19.psg", 736, "00692dc5745a43159ac306c567b1ad31cfc59f57452bb351eba3df10310e149d"},
    {"geometry/pipe20.psg", 736, "dafdbeee15fff1180b896336118904a11e04a64e18fb5015f0252370d7d257d3"},
    {"geometry/pipe21.psg", 736, "57b7c6533e342780e3b68c8329190ecc83d61cd6ac1750a56592fda69a4963a0"},
    {"geometry/pipe22.psg", 736, "22cb2507098dbe3c30ca3c93e8f7a53e68da286292baa7ab95e3104411c01a35"},
    {"geometry/pipe23.psg", 736, "67aab6c51639ddc6154b032cd13b8630811d4bbb64193be912e84b04e9718d7a"},
    {"geometry/pipe24.psg", 736, "9234db0a40caad2ffb843a871f02023dde9e092e40f12f52dbdd038eef457229"},
    {"geometry/pipe25.psg", 736, "28637a6adb36193a43b78fe36588db260b918e9a7d3d2bde29e50eff8451cf62"},
    {"geometry/pipe26.psg", 736, "daa7ed8785ffe2aeb06272e5ecf24f1f2b025f202d9a81baa27b372405a8c554"},
    {"geometry/pipe27.psg", 736, "5b640a11bf25c4d26513476f8a15da5c645f1e355a7549bfec7a3607b93b54d8"},
    {"geometry/pipe28.psg", 736, "44073c0cad4a9ed22bafc28c1e694e4a312ab2b4077ac693b65daa912c885fd0"},
    {"geometry/pipe29.psg", 736, "72aba2fa4052deffa95c7aff3439882d137280d024b62e2894bbc31b15c82e9b"},
    {"geometry/pipe30.psg", 736, "998c7246de0fbe656591507c622fec86470bcca3be1f151d375e208f3d255155"},
    {"geometry/pipe31.psg", 736, "55713da3e80983a6df97aae6955ad969ad56fa51a3543de76614edc14e24250b"},
    {"geometry/pipe32.psg", 736, "e85f03e5b56500ea399b051a966eeeaca6b16fd3eb32dc03e9cf57e15b77fb7f"},
    {"geometry/pipe33.psg", 736, "70410309118000f8efef0800dbf545dfbe84c0411591fc428d19e5ea22324bb9"},
    {"geometry/pipe34.psg", 736, "8fa17a7ebc81fdd7964abe061e9453b2394db1abafe8e3eb01cd272775339d13"},
    {"geometry/pipe35.psg", 736, "e7abfc7fea9494e8d1d2aadefc9668cfb9480c29d17f6d78427bd76d487f06ce"},
    {"geometry/pipe36.psg", 736, "a5ebbb46bd7ac4d5cc1e5da36fedd348784ab36b24ebd6591494af7ab806cedb"},
    {"geometry/pipe37.psg", 736, "99a61d7344862bfe4d527a7c755d43713bc9bf306b8e117a4e18d0b5db36f9ca"},
    {"geometry/pipe38.psg", 736, "6e8ef27478bc53d4241761bc828f34491763e7e72350e5a9a6a6a2f70af295b3"},
    {"geometry/pipe39.psg", 736, "dd3d3c4ec74b603890157346a6e87184d62264d425ee5ac4adc72bb8428b7ffa"},
    {"geometry/pipe40.psg", 736, "ba11f3ac3b63e9c3117dad18121faa3067b436594e88645cbd8b99cfbf8a55d9"},
    {"geometry/pipe41.psg", 736, "e92b0ba76a147284fe2b18a2be1d2c8ea209e805af10393186d83b8217488ce3"},
    {"geometry/pipe42.psg", 736, "48624cbc3d4fc2bec1bed0a980564b57288979799e0814175e0b095e5f641e09"},
    {"geometry/pipe43.psg", 736, "f2e12f6ef883a8e1918c68ff5b7b6b47592f950e1ce79323d5d9fbffb4550dc7"},
    {"geometry/pipe44.psg", 736, "72aab89d02f727d410b51ad23dd09f41ee2dd6084ce8059abddcd16a0d30e140"},
    {"geometry/pipe45.psg", 736, "b065a22e7534279107e95acd806ee908a23e152961f9defc8ca87a110598d870"},
    {"geometry/pipe46.psg", 736, "4ec275d852312d2081cd180c8f14dcfb005bd1aca2c1652f3a1b0e8fd5104908"},
    {"geometry/pipe47.psg", 736, "53d733df0cac6ec7a03d7e2e7239d4298cfd9df9f8e2858440024985870e2de6"},
    {"geometry/pipe48.psg", 736, "4d55e1ab2ec685ec783c3370d5ccf03ee01cd5736f4bfa8dbfc02c0c0c732caf"},
    {"geometry/pipe49.psg", 736, "df6a7ae3dec7674bfe1cc5951cda065f49e0ae65a79b81aeb44b226b692fca10"},
    {"geometry/pipe50.psg", 736, "079557fac650ed3ed54e4f0e7127feaa8aa8bfd38161ae7a48e3b45eb66887bb"},
    {"geometry/pipe51.psg", 736, "8111df36099886b66a53e5518bd526585f04adaf1ce8dd82ff1642e54095501e"},
    {"geometry/pipe52.psg", 736, "19989e1dc610a594d9c3b763d4e70198e613d209a0f8736aa1098484ea279682"},
    {"geometry/plane01.psg", 3296, "9c66b247692d74fc83a2f0d46da791da2f619ef29a43f95beedfabbe8091f1d3"},
    {"geometry/plane02.psg", 2000, "6dad1b89c421f27d49b67a3a0c80a5854aa51f0376460d11fcd74d69af8692c6"},
    {"geometry/plane03.psg", 3296, "8f0bee20fa48d632952d521d920de63ff3a7770e71801c9a4b6f1e237facd29b"},
    {"geometry/plane04.psg", 3296, "fc3c28ba83f38f8f891229a1df327c4779ba7a02c66e2759e7d24688cb34ac6a"},
    {"geometry/plane05.psg", 3296, "5f82ef313e36248636fb4ab44a4450aeca912589b69e9871783f324721354412"},
    {"geometry/plane06.psg", 3296, "8b977e998f26545c8ffe706946e93c59e9eb736f302b834ab322e6cdc88eda05"},
    {"geometry/rib01.psg", 4080, "1344628c2655ac2e85945a9bd749d609e5b9b95bf16a410fc7546aaf6215cac7"},
    {"geometry/rib02.psg", 4080, "aec77d051a825465a8011003d0d0f8a3db2dc80a7b60ebffb3f6368e0754f188"},
    {"geometry/rib03.psg", 4080, "96a798a2a8bcc93a3d934059e15227489c32f2afafee605bf23738a4c919923f"},
    {"geometry/rib04.psg", 4080, "2fe9419de0aa749de239511d1c66c86bfa5e885d35f2f5e765c32443ed5dba15"},
    {"geometry/rib05.psg", 4080, "1132ca2243fa246ba24387f6c211aee1f81fc44c46a53f59d2dd9530afc1b832"},
    {"geometry/rib06.psg", 4288, "513175ea34b7d08d4d21cab367c67a59e49fc54aa9ebc6ad6c39dda3fa6e015f"},
    {"geometry/rib07.psg", 4288, "7d2ae17b44434f4685bf5989b02cdd47a9322c1a12fc1ca6ff0ea610317f34d4"},
    {"geometry/rib08.psg", 4080, "6169c0cc1f3cf389e9b06b564a51ddeac2b41743d5201242494f024c2685f447"},
    {"geometry/rib09.psg", 4080, "3a5ac7e5c984480f40dece6af00c131d5cb2133ef0f2a8f68e65753a38610544"},
    {"geometry/rib10.psg", 4288, "410bbab1450108c5cd5d4282ac2724f3c87b4c2e8e8e29a2ff3b68342b8fd1ac"},
    {"geometry/rib11.psg", 4080, "7b3af3f59c1383b87ed1c224c7c314cf744dc731a24750ecc39af04827615456"},
    {"geometry/shaftclasp03.psg", 1104, "806382a71ebdb9f1c0b092e3fe7ccce5b3ada68e229d0b4ac974484debbbb7f6"},
    {"geometry/shaftclasp06.psg", 1104, "4f7f4faf06e8a0e7a6da117f755f3867724136c143dae84335725f95c22f882a"},
    {"geometry/shaftclasp07.psg", 1104, "84efbb3b444b9a1d799a972cbdbe1bfc73b4b9fa8bafa43c59f3e6902f81b3a9"},
    {"geometry/shaftclasp08.psg", 1104, "20ace0b7aad5196f96b939e109a163625096050a14b823f38b708d175a25fb81"},
    {"geometry/shaftclasp09.psg", 1104, "723c7309ceae1817f47e58b6ded89fb7d8518936c332c043292318a7505db47f"},
    {"geometry/shaftclasp10.psg", 1104, "f9756ea3b57b5e1a22ffd3b649644196ebdf027e5bbb6028e2c2badd2e9e342c"},
    {"geometry/shaftclasp11.psg", 1104, "5830f05af00fc0358b77c7868169d4d911abc511d83bcba8c9aca388d9cd3828"},
    {"geometry/shaftclasp12.psg", 1104, "34d69783f695b8a773618199f8fdac99629d62bdb7bce6a4cfc94bf04c636d2a"},
    {"geometry/shaftclasp13.psg", 1104, "eda17c98d8108918c51091f6fe7a045503c024635cc05e5be04b6c0a12238c2a"},
    {"geometry/shaftclasp14.psg", 1104, "f70053fa023ae6ddaec2e729a03fd2b94297eaa8251b3a7a66ef5532ed18bf7e"},
    {"geometry/smokenull01.psg", 992, "b7273c7a28541ccacab3acf3df25e2e2afd98995564cedbe42b6e9d96a67ac1f"},
    {"geometry/spearpart100.psg", 992, "d4b1bd7911ea07e994461e72bbc8f5657cee4210f11398e30e970722b6f8e6f1"},
    {"geometry/spearpart101.psg", 992, "99e5f85f2b09a1c690f7c1df963e491c66d1de2f3158b73d24cf0d1d8a6b2f78"},
    {"geometry/spearpart102.psg", 2048, "ffec1c0ff7061d054f5aa145564c83cac81a4f26aeaaa1ac3a91f0d12dcebede"},
    {"geometry/spearpart103.psg", 2128, "9d4754df6a0d84feafef049eef4aa35c6c0988d56b491523aa4c8e79092f5cc5"},
    {"geometry/spearpart104.psg", 1424, "5dcf042688f5a049cafdab9ca744624ef820c172e182ca45c77497e1e53e73c9"},
    {"geometry/spearpart105.psg", 1104, "ec0af36559b0c0759758b6b5b7ce6b878e991afad70edc066d47704f3436dede"},
    {"geometry/spearpart106.psg", 2048, "13862376f2fb94a57724d377947dbce07b2c9d8cafa1038422595feb7fcb43e3"},
    {"geometry/spearpart107.psg", 992, "001ccbee8675c587ad494f93b7905a8f37a5e080a56d8876fc61dbe8e68bf553"},
    {"geometry/spearpart108.psg", 992, "688de299f9be8d0099c8ae129919a57c896e58969d0f994ef01a955f630b4d05"},
    {"geometry/spearpart109.psg", 2128, "ec2b9bc2b76f99bde305be4a963389189b2a0349627e884c315901788ba7337a"},
    {"geometry/spearpart110.psg", 1424, "83fe8f84e4e40388643d22e27ea0ed4b85dde071179e27a5895cdbefe6f0da25"},
    {"geometry/spearpart111.psg", 1104, "8fd85c4713642c6594cba16dbf39b90989973bf1573766f76292f9c7e1837f58"},
    {"geometry/spearpart112.psg", 2048, "59c88f8983bdcfbbfb3754737dfd36d419b5d5e491ae209e4a5c608f0f3d1683"},
    {"geometry/spearpart113.psg", 992, "cfa1de8de280f546716efd4b4af7b5f440fd22a5b09b94a6dc98b2ba2da19db1"},
    {"geometry/spearpart114.psg", 992, "aafa3f11d87f95874fe40d158ee09550b120036b57c953d2a986ca9409cbed11"},
    {"geometry/spearpart115.psg", 2128, "fd262489caec092faa6cdd8730eb1b6d69b4da89d138ff3224bef496eb1ca13d"},
    {"geometry/spearpart116.psg", 1424, "a34661d47014645924a45a5520d69c3a21ce0359d6248cdffaf7d533a4d350a0"},
    {"geometry/spearpart117.psg", 1104, "b75626a08d91e15c2de7a327288f19d06322adb18e61bd6b782dad09da5920f4"},
    {"geometry/spearpart118.psg", 2048, "dc68ba780e1cfe204ecb5ffa21001ce2d2cb3ab79ff9bdb0ac0d0d0f3753c39a"},
    {"geometry/spearpart119.psg", 992, "6fdbcc6446902f17b7883c9554f78930c05556fbcdfd30286fb79bf976968997"},
    {"geometry/spearpart120.psg", 992, "34cf3eecb16ebde531b17281f9931f87fc9fc66863720bd9f29c78ed4c7e70b7"},
    {"geometry/spearpart121.psg", 2048, "fb761b36d3e5b7b51a9b2966f67ca4d9c5f95f3bc1491aab16aeacf04cb036b4"},
    {"geometry/spearpart122.psg", 992, "003358a2b561d8ae17697173b2f721db5d7c1d0e5eb0155785cb019547733a9c"},
    {"geometry/spearpart123.psg", 992, "1694b47d7917228a90bf2106dbf63ee5ccd63748e1df3fca861281eb52485959"},
    {"geometry/spearpart124.psg", 2128, "6a646a53662449ebd2ba55a9921695bf83150cc9211f0cc8584dbda7a13f5e59"},
    {"geometry/spearpart125.psg", 1424, "573b559c024737088bae7c0d98ecb53bcdeb2ed72a17c01f7e96303eb8368860"},
    {"geometry/spearpart126.psg", 1104, "c831abc9bfc63202d4d038ae8636d41f0b86b9a0c74ca5bccbd9b257603d9734"},
    {"geometry/spearpart127.psg", 992, "5ed3a6e870c94a6b3b5321481b1d576d7662b082d7ea0eb08430498b0252727a"},
    {"geometry/spearpart128.psg", 992, "9c11f182a056510adf764a2f6e4ab84676715dc94146048bf31cc6a2e13283b9"},
    {"geometry/spearpart129.psg", 2048, "1ba8cbd139b1718f71b591b0043acd78141fbd812f958615dae0dc3923676e63"},
    {"geometry/spearpart130.psg", 816, "6946d278072cf5678b749bfe593c38989c9b784bc3b47c58568f15c3ead95a41"},
    {"geometry/spearpart131.psg", 2048, "b283758d571429092b29c63c572cad57bc81833fe3548dad85007907649d26e8"},
    {"geometry/spearpart132.psg", 992, "82bb4d5dc92c9b9aad0d74165d215ee124cac983f4dba755512a32c9a75997e4"},
    {"geometry/spearpart133.psg", 992, "7baf4fadddc4fec428124a60c999b3cc8322e28b4db8cebde042e73e53e22ce6"},
    {"geometry/spearpart134.psg", 816, "70c50fa566b0f1382f12aa1621a400a3ad2da7de0b047c0e6e4fd7e66d70f8d2"},
    {"geometry/spearpart135.psg", 2048, "28366dd93dc72fce6d4f7b2d4bd5c6588c08ea59f46a1d587f7a1a498c32a808"},
    {"geometry/spearpart136.psg", 992, "14d7c4d7f10129731ee461a22603df8104d4cae050cbdf000e09be12cbeb9830"},
    {"geometry/spearpart137.psg", 992, "4ec8c85e71c54444b195d81aa77d5464653bffbac222d50cdcf5a9a915a486bf"},
    {"geometry/spearpart138.psg", 816, "16695af27867415b103e904a837e5394ccf76791d7448a88f2563e4fb604e111"},
    {"geometry/spearpart139.psg", 992, "fa05e2d6cd25227066e5af39f4ecc9d768aa1bb97d0e5aa39751c6be9c97fd5d"},
    {"geometry/spearpart140.psg", 992, "08360f01e54bfea19552e28327de0d6f5203b3d19b9448f0fa01670bb008550a"},
    {"geometry/spearpart141.psg", 2048, "afed9e181f3cdde7cea21a20e4bab328fda99beaf45d69ca6a64c5f68f0c721f"},
    {"geometry/spearpart142.psg", 816, "728e0618ffaeed93abb57c34067383db1ff0ceec6e5999d718916632723615f4"},
    {"geometry/spearpart143.psg", 992, "e89e4332239ef569fb40ccf0876070127b310dc62efcfc1ccee468d51419b104"},
    {"geometry/spearpart144.psg", 992, "fede060b7c2f3be4a0a0b54f82839852730814163f3d4e7417f47568299c22eb"},
    {"geometry/spearpart145.psg", 2048, "c48a93941b14c5717067c916f1568ac42f0f72dc7cf75e4d8f7f3cdaa928ea03"},
    {"geometry/spearpart146.psg", 816, "dbf1e6b4fc417748b190a0010a0fa39fe07c2baa827f5c69b58c4548d4915e80"},
    {"geometry/spearpart147.psg", 2048, "f1bda88f3b275923db5b84a0e6d58d1a06d53c3d1e03b7eb9a6c4ee80f28a314"},
    {"geometry/spearpart148.psg", 992, "4cf7107d36beeb9209589d92644b4de7467c5964d3764375aac604266b181782"},
    {"geometry/spearpart149.psg", 992, "c46cdf37e1951c3c4fafa0741cd2a8743554fe39eee79fd5c412239c016c325d"},
    {"geometry/spearpart150.psg", 816, "a5becda7fc06417b528cf2a6d8e3eda64ce0cba5de696302437ad6052f0b9396"},
    {"geometry/spearpart151.psg", 2048, "ea53b8e256612d0f44b1ee290679678163418242209c76989310dd66b8cefb98"},
    {"geometry/spearpart152.psg", 992, "da0a36cde0c6d4855faeec8669065da02d09cf326392a918cfd41e58bae2667d"},
    {"geometry/spearpart153.psg", 992, "54a3215dc7f446c2d9da263afc9b4443652211a881bda62d5c5a2c82891e2473"},
    {"geometry/spearpart154.psg", 992, "757c2cec8579dda927f3a40927cc3d489dac0ba89c697946720f91cdbf0bc3ff"},
    {"geometry/spearpart155.psg", 992, "b31c9a24e1848e9bd63093d273d2fde49b2a0baf1cb6023028b2cab981588970"},
    {"geometry/spearpart156.psg", 816, "af334dec9a4184da5af65b1d996d252e43a73875c1c80b8bb7f9d44667bd0d44"},
    {"geometry/spearpart157.psg", 2048, "bf22a179a0a08ae26ea51915c1a00b872f71227372cda96ab17285f900260241"},
    {"geometry/spearpart158.psg", 992, "336476346d4b937ec8f6b68dcaa1d5f4e76d29e002eacd0841cd54ef5d32d360"},
    {"geometry/spearpart159.psg", 992, "2ae060d6b46687775f267622020c6c49d631cd526edecc4be5d8f3465bc99ede"},
    {"geometry/spearpart160.psg", 992, "51f8a5f8c35b85ec341316f8b7ce4bfc62c2d1ec31909f1616dead4dbe197f0e"},
    {"geometry/spearpart161.psg", 816, "20a95a433915e4ee34df143315442e68eaaf43053457873b124aaf3ca4917c31"},
    {"geometry/spearpart162.psg", 2048, "bfd364e33294c74f90071544b1f11765db9d3a9c8ea6f4d07a4c3d2238c9e49f"},
    {"geometry/spearpart163.psg", 2048, "81956c6ba39afc6856ff74660e9a249ce8ed211f2116ee6deaa7f04089f74f5e"},
    {"geometry/spearpart164.psg", 816, "011c0827cefccee34ae1191e6992f65ed1c0b55c983be30915fbfcc4f4d83397"},
    {"geometry/spearpart165.psg", 992, "107a7f5cebf5a4b6aa5c1d7617892edeead1e67417c5f15e1b4b00a2ebd81dc4"},
    {"geometry/spearpart166.psg", 816, "18a7df6b1d80d2b6f3913e4df735b27174f80c68287e2b3d4bc066ffa1034736"},
    {"geometry/spearpart167.psg", 992, "8c1dd7179e993412db832b7082b72c108eef72428f7f8d87f97f6257f9abf35f"},
    {"geometry/spearpart168.psg", 2048, "2ba72d1d38ef4d7d0ad89f25c0f10bbbbec95bfdad27414112cc517c9c356fd9"},
    {"geometry/spearpart169.psg", 992, "47e1d79abd7d3ab664007f825d64b796b6a9b5c7edc44a29fa776604a73847af"},
    {"geometry/spearpart170.psg", 816, "fa1663f4a8457936cf35e8d892a3a68436e76ae402f24e9c46ceb20b8c4c6a7f"},
    {"geometry/spearpart171.psg", 992, "efcce724e172114f3d758dedd33d002e488cddcf389b869417fb13e0ecc54ba0"},
    {"geometry/spearpart172.psg", 992, "cca4feb449eff51081b03ad68767323a6aadd8f4e6f53e56e1a71714e7fa9bf4"},
    {"geometry/spearpart173.psg", 2048, "60a1c465759978cfa2c6c1884d0a667690f963420996f42f80d59b505b6be903"},
    {"geometry/spearpart174.psg", 816, "6acd0e17be40536801fa8c4c12cde77c0c6441c0f15f92a9b545f790895c1264"},
    {"geometry/spearpart175.psg", 816, "fc347af3b9d2ef4c845f595966dcb95d38c60677d7990cf247314f3907ef6dda"},
    {"geometry/spearpart176.psg", 2048, "9367d311dfb37cad85c77112e53c113858d47e76e3352071c039f3a0ae761a66"},
    {"geometry/spearpart177.psg", 992, "2f14959a0831b29f92e400c8df943e347b8d72be0bf8b19a78ac182c05ddb780"},
    {"geometry/spearpart178.psg", 992, "721f536bdbdceb89d56c42d0b12bfe45053352eb0a9883390c66a76685274ea3"},
    {"geometry/spearpart179.psg", 816, "7d38004d1dba9056a0dfae73b0a2aca2c73306e979fc8d620c22e08e83dad8ee"},
    {"geometry/spearpart180.psg", 2048, "e20c56c18ac6df1d96418941687c614e857138928e906fd5441c81ed37d03dcb"},
    {"geometry/spearpart181.psg", 992, "f3628f3028d7c8d5041699b9f20fd268365792590c8a846078f127e0a063903b"},
    {"geometry/spearpart182.psg", 992, "062858b82be7d443605942653797e6f4b96dac4658554d731a7c4c72bcd19df2"},
    {"geometry/spearpart183.psg", 2048, "380b0b3d18b46ab7c82adbadc02850fdfaf12094a0bb1934ad83105560767be9"},
    {"geometry/spearpart184.psg", 992, "d6e48b5c0cf0658c9a3fbfbf6dab7c04f64ed07579496e442f50d766e489a754"},
    {"geometry/spearpart185.psg", 992, "64f599df85ac576f898fbe56a5e64d050583ef96f23e5bec916c89bce14793ab"},
    {"geometry/spearpart186.psg", 816, "47272588ecaa3d77f82214733c8ae1e3012b9475017e39d5acfa879acd4be47e"},
    {"geometry/spearpart187.psg", 816, "6f5c0de87cca572db8f528d4cd548fa05df940fc0e89c786a4b5b48970c6606c"},
    {"geometry/spearpart188.psg", 992, "2fce7f613395d73ff5e5ec25a902f66dc8d57cb71b086b1ab9b834abc4e34e86"},
    {"geometry/spearpart189.psg", 2048, "3a4a6afc773a19f2c228e953795d0f2c7480e1937989c0f20fa8c3b9200f636b"},
    {"geometry/spearpart19.psg", 1424, "6fe9956ee8778726cd0db6a5b00f6fd13c7b14d2bc884ac931ccd79663e1b53a"},
    {"geometry/spearpart190.psg", 992, "a251b29a9bc9e89390c3ff17b8a91f2a1525f31b4d056d8512841003b6b3759b"},
    {"geometry/spearpart191.psg", 2048, "1d5f4436224870f28073ed4f4fa15c71e48e622d2567bf8d75bbf74c30984adb"},
    {"geometry/spearpart192.psg", 992, "df27b963e66e4766f0f1bd6aebccaac6b6f347a43ada699c416cdb7026c83f5f"},
    {"geometry/spearpart193.psg", 992, "9aa64ff7a260e78a873bb2e40ff8bff03be59183d98f855e1b029f1ec07e4c58"},
    {"geometry/spearpart194.psg", 816, "811d65e8398d417f95cf9751613db7cb5f87f0c7ee638a8bfef262fddf2ef785"},
    {"geometry/spearpart195.psg", 816, "4bda19240de9aa911bd544c9a3bfed9530557bcd0de3bf352390115213cf697c"},
    {"geometry/spearpart196.psg", 992, "7ef69a46170ebf9462986ddd9ca8226b2cd4eb2c697f02549ba80c6c380c62d7"},
    {"geometry/spearpart197.psg", 2048, "c9d3fc55c8c23ed4ae9f2fbe7546f3f7a515942aa9fcafa8e174e015ced8ec9a"},
    {"geometry/spearpart198.psg", 992, "32c3f54189db50a34bb07160041d105afaa0476a676be6da96113bd5838e37ae"},
    {"geometry/spearpart20.psg", 2128, "ccf468888a184a6522f780757e737dbff1dc2fded80289a3cfeca04666d75865"},
    {"geometry/spearpart21.psg", 992, "ec6a85fb3b71f03273f369d7f841d5222c8fcfc3fb2d62ca6c2cec76e40eac6d"},
    {"geometry/spearpart22.psg", 2048, "18eb02c7bc52e3040102c0d7569a20c956b172052ccf0b492ac2094f93befdb6"},
    {"geometry/spearpart23.psg", 1104, "5edd04491231c0fcd3d71c332f1bfb31f1830a4d7d5bdeb416619a31d6e2f4fd"},
    {"geometry/spearpart24.psg", 992, "78447f34c84a4881197c2252c2547e1807497a0592cea2f261e19a894f25e36f"},
    {"geometry/spearpart73.psg", 992, "30b73ec5c99d93b3de88368d0395cff81c59e4eb8c8334319fe3a468ee61b178"},
    {"geometry/spearpart74.psg", 992, "1f27693d1fbc5629c84ad4a2c9f0493de5191a1e870109d371969bb1f594c853"},
    {"geometry/spearpart75.psg", 2048, "4b6723492b65ab53acd0791183c3af68c61a16cd8855f43ab87c6ae32fc3e0e9"},
    {"geometry/spearpart76.psg", 2128, "a5986bfea12dc2ece959a1aa382cef962245f37071e46618224daaced505cb06"},
    {"geometry/spearpart77.psg", 1424, "dd68be38ddf6b0f30c5cf6d6aa54f64f680dd746cdcd0f7a4c846dc86d637e90"},
    {"geometry/spearpart78.psg", 1104, "69412c706eee9a0fdc8b011c37bb43b02a4731bf3083525871934454fee773ed"},
    {"geometry/spearpart79.psg", 992, "a4f69596818277e1e430957765b9064aebe3f892d05508572dedb1333959ac13"},
    {"geometry/spearpart80.psg", 992, "ac686cad96e6efe5434d04de451616c8a3c23e825484e3f27a922dcadcb14c06"},
    {"geometry/spearpart81.psg", 2048, "f3d950a79799dcbe99617cfe85219bba2ab8280f3a4ae8f8f0bca7f13917ed2a"},
    {"geometry/spearpart82.psg", 2128, "4775d40339d9320b1474bb4a913269a016e82d258c24dab501cd51b8899fade7"},
    {"geometry/spearpart83.psg", 1424, "0e3135109477435599056709f76dcddf68b7f52093de6015097ee5510ab79c2f"},
    {"geometry/spearpart84.psg", 1104, "0e761c71ba880f918810eda01d28e279dd934078e88be1ea2f1cb10c9ab2b8e2"},
    {"geometry/spearpart85.psg", 2128, "19461f0ce2f232b854ee9a6ed15ad00a8f9f66c0c5c1df726bc27222658ad95b"},
    {"geometry/spearpart86.psg", 2048, "432368adf1b9c47322e519f84e87056f559163d4d0b48ebddb0959401a4b0de0"},
    {"geometry/spearpart87.psg", 992, "da2d2e5c724a5a4b99877d83f2e2605de0fec9ef0042f698568dc9663208c19d"},
    {"geometry/spearpart88.psg", 992, "af3c19bd7e427f0e0ec140d29905e21541a1510a5e8e6f5f625d4d5e852e62d7"},
    {"geometry/spearpart89.psg", 1424, "6cba953d64578ba2eefe5954a01e0c59b30f6acc9d6539df2ea6d326e433b561"},
    {"geometry/spearpart90.psg", 1104, "100e63cb67af959a4e27d8c33e8f076bf40c412df94db0a52e7e1bb582a47f57"},
    {"geometry/spearpart91.psg", 2048, "23b70e2c91e05fbb68088a3b5baa78d6b167ddf9034b49a7eaa30732cb2f6dba"},
    {"geometry/spearpart92.psg", 2128, "12ebdb44b183e4f4602f1f341daac8cce1bbf27b1cdfb849797de962f1a56e9a"},
    {"geometry/spearpart93.psg", 992, "85d15f7603a92c7948f82aad6618d0dd83a0909ecfa3d06c684ae135fbe4c082"},
    {"geometry/spearpart94.psg", 992, "12f42d51153bc6df506fbdb519c22bf3fa0d64a3aa7a9994eccbcb06f2b365ab"},
    {"geometry/spearpart95.psg", 1104, "bb14eda72cc33a4a5f7a53c7bd81f69e3e0a4178e849831833e23faf9f8b6aa2"},
    {"geometry/spearpart96.psg", 1424, "f07d442cd28030e17ce86525e9b820214f437c208f5d3109eb9b1ae13aabc18f"},
    {"geometry/spearpart97.psg", 2128, "e6333c8bb4a62281e1105d2a263f3b5a0497ec53ab76770a89a54cc5381d3146"},
    {"geometry/spearpart98.psg", 1424, "9bcde1c7b205917b21dfb7a8a9dfa9a776a2cddff7fc7d64dac37d69020fb492"},
    {"geometry/spearpart99.psg", 1104, "7d04836c3eceec9bec836eea8a5835ac6712004b71cc72e811c1a74da7d7a892"},
    {"geometry/theskull.psg", 40400, "f21371a06e312c4583903e303ac7ccec3b7d726af40096773350c46349570c38"},
    {"geometry/zone01_arenal.psg", 21276, "c581109245ba9fecf4f72ac828efef5dc3c162537a35bddf8fe38c46893634f2"},
    {"geometry/zone01_arenal03.psg", 22364, "b3a1eb0bbc10262cb31a0d672c4eefcda33d26da43c4f5b41020a02811d6733f"},
    {"geometry/zone01_arenapost01.psg", 848, "6a51f1b0cf4a232de44a706b18f10e74f050ecf6daa9a85732205d95ee9e5f7d"},
    {"geometry/zone01_arenapost02.psg", 848, "32630697af0c7c7729183af7e6baeeae69ba02e162af9e895856b0339069dcf2"},
    {"geometry/zone01_arenapost03.psg", 848, "b7d79040f1991e05fd4708a4a1b94756372c934eb65ed63d72e2c81d84424441"},
    {"geometry/zone01_arenapost04.psg", 848, "5a44d19e6c9948bedf7c4beb01f15a8f45563790f00d17a025097d5b8a634d9d"},
    {"geometry/zone01_arenapost13.psg", 848, "d7cbfad29b06d7f0da30b5d4690dd59642666fd13e2fd22f9cbc11e8457ce88a"},
    {"geometry/zone01_arenapost14.psg", 848, "46db7c9dbba67f257df9729ee55651bee12bb504ab161549ff56568eff756197"},
    {"geometry/zone01_arenapost15.psg", 848, "652dc448628320e163e7105e2299e89d6556b7812accfc43c8b1622b6baf27ad"},
    {"geometry/zone01_arenapost16.psg", 848, "f50d9d5acb53689f337bb796c6932fe2e660e7e0884a1d06448bc5ba3964bdf6"},
    {"geometry/zone01_arm01.psg", 33140, "a84c5cfb6200be8dcaeabaa135fcfad0fc449176ad5bab6ace80243509e0aff4"},
    {"geometry/zone01_arm08.psg", 32596, "762eca2bc3be6bb89d588c9f0e362c1fac10944f3677a26c4d6faf824ecb15c7"},
    {"geometry/zone01_arm09.psg", 32596, "bd1ae6b5cb06293861f2a6326b6d31b8f99ae9c5099a18ef6f4223660dbaeede"},
    {"geometry/zone01_arm10.psg", 33140, "fab8ad3b1f31382fff637d2fe1314f39787ae687b866a1d1af9cbd6b0e65c300"},
    {"geometry/zone01_building15.psg", 1104, "d1ed4199ce75c61118ecfa23ac18d5e20a0ae1d2179112d4fac95a2f580528a0"},
    {"geometry/zone01_flag04.psg", 1520, "a3ceb694a5d081fce5cf2590dca94363719ff78405827b749422fb0bb4225e12"},
    {"geometry/zone01_flag05.psg", 1520, "da18a88ca0627df9527178e34df90f16d5dbb5091e66fc695900bfc27dce1e28"},
    {"geometry/zone01_flag06.psg", 1520, "568f138872025c74f2f1d0c288e9119c5936fc075ce5f9b5d9c826ea97eeaa91"},
    {"geometry/zone01_flag08.psg", 1520, "8c4e0a6cf2eae5ff08e941805f89643bc65deef0dac9544593c7e22f07a40c67"},
    {"geometry/zone01_flag09.psg", 1520, "087e5a2b46e2d884603cdd0ea111ce1fdea1edf8de0f1510ccd869e877261560"},
    {"geometry/zone01_flag10.psg", 1520, "b5b8f19e8a3fdbc91e64f7d3fd3ceb33186e2b7794bc81c7c1cb7e17e5e8b6d7"},
    {"geometry/zone01_flag23.psg", 1312, "039ee18b040fd63cdc0234771b676db1c3a5e5869666892d33233d8734ff4612"},
    {"geometry/zone01_flag24.psg", 1312, "bf5418b2ae9cc4dad488baffb564bc638ab37ee88d0e795bf45bd743f87ba548"},
    {"geometry/zone01_flag25.psg", 1312, "b317022376354de26e556159ed60de70bc17f932a82e98c873d7096c0b29ba06"},
    {"geometry/zone01_flag26.psg", 1312, "7f1db696d76eecb1f38dee0cd7eecca053c423240f404b5ab256a51a88fe40ae"},
    {"geometry/zone01_flag27.psg", 1312, "30c7944b258b6a34c64d84c6673d5619c063a8759ba66c2c8d28e50fc70bc460"},
    {"geometry/zone01_flag28.psg", 1312, "55d64b472805502281b3644e033ea50f478c6c014e285c18c4a0d1bb85c58a9d"},
    {"geometry/zone01_ground.psg", 11344, "9968806ca62c6d86fb94c3956db534950e5b72832a3cafdd6d05e2bfd9b59801"},
    {"geometry/zone01_newgargoyle10.psg", 24840, "2198eaf9394dcc189eee5853e54d65f28a13925283fa6496d9e499ab4a231397"},
    {"geometry/zone01_newgargoyle14.psg", 25176, "cd7dc885f83f808650f409ec420bc3335eb1109f83cb14a35347606e461940d0"},
    {"geometry/zone01_spear04.psg", 1232, "26204617fa830a856fa0fc78d0f324826f58e351b8ee6a334c9069834827f73d"},
    {"geometry/zone01_spear05.psg", 1232, "6b33a8a8a7448a118494f048a76d778105bc4bdaabd1701256379fa4f7f53918"},
    {"geometry/zone01_spear06.psg", 1232, "cf48122ee7529b0a95dc6c7e371ccc7194553e7ad1b492e866edc66f8f95312f"},
    {"geometry/zone01_spear08.psg", 1232, "863086d318888a06426386529fbe94cac08441119dac897dba8654816f46f78b"},
    {"geometry/zone01_spear09.psg", 1232, "acde1a9e7a173b1a0a4b7281baf11d53d3d0175c315290ccb0e53ae1ccac6683"},
    {"geometry/zone01_spear10.psg", 1232, "822d86a18288d8a081a6f7e2962864e402e520749ea4e41b48bcb9a7f3f54ef8"},
    {"geometry/zone01_spear23.psg", 1136, "207543dc182df397b7da3e20ed0c1711de9b4ebfe24e8f6da992d093e5c9b999"},
    {"geometry/zone01_spear24.psg", 1136, "f71bb4c95329e985eaa711f7faf3f1073107f8174b9a6c1588d52d9a140f7987"},
    {"geometry/zone01_spear25.psg", 1136, "32a8c8363db7ee76dd4f0ef06453684db7fea099322145a41865f62b49dd2bd7"},
    {"geometry/zone01_spear26.psg", 1136, "c8b0927f3b5912d191ef07f494cfea14931fb1312bb13bcf68dfe319814f9d45"},
    {"geometry/zone01_spear27.psg", 1136, "00bece68d85571dd612a27599c97deb66f15c4106722edfc172ad54a43f7a25b"},
    {"geometry/zone01_spear28.psg", 1136, "e2fe734a2bbc5b8ab7e58e1806f77d869358f60caea9d6a7d49f42ed309d4ea0"},
    {"geometry/zone01_wall01.psg", 12128, "c3dcb8acccd4fbc4c3b6628f3d4301cd391ab5fc0a685e9cd2e016eb52ec306c"},
    {"geometry/zone01_wall02.psg", 2880, "f6b59f2fed1720794b77a81537b81fe12fef8cec208b6e029059fc03939b6d6e"},
    {"geometry/zone02_road.psg", 3104, "0b737ee849ecbc5f9bd7c132bf5c45045745a3e2743f83c815ccbcdbab222813"},
    {"geometry/zone02_wall01.psg", 8704, "2767d3bb82afebacf4b495860e32cb30416bbd4f370ba3ee6795a904167d2250"},
    {"geometry/zone02_wall03.psg", 12320, "8abc433394d13b11f462459a4200db050e4514996fe114033f042988263c4c6c"},
    {"geometry/zone03_cliff.psg", 20048, "66a63a76d7337a4ef22b98e4f95822b972145c354e60b2e375ac550b3f304915"},
    {"geometry/zone03_cylinder03.psg", 5284, "2d555f12130f627c092dc8e8121e6e0a8b05c39ef2e3c4bd8acbcd687f491819"},
    {"geometry/zone03_cylinder04.psg", 5284, "fb32ec1b4367e59033ae50627fce76b49d814bc1a2e6daec081573cb75c6006f"},
    {"geometry/zone03_exitgates.psg", 10800, "7858faa7b9be886a3c0b0671d775b93646e40cfa1dc6b21706f29fc6c61a6fbb"},
    {"geometry/zone03_exitgatewall01.psg", 9216, "bfb6059be15b84ac131640f69afb7e05711416d47a88196188fefb7474ad18d8"},
    {"geometry/zone03_exitgatewall02.psg", 12800, "8f8ca98a7603b230225310f079e7e4b7a72cbf54812c2e62953ef090e0d600b5"},
    {"geometry/zone03_gateseat02.psg", 2592, "58be9d88ce599bbca1639e57da7528c533df3688afef3cf6e4794241cdbf3761"},
    {"geometry/zone03_heavygate02.psg", 19620, "388912906cfa119a5368bbbf3cffb593f225751c78031659c62ea6c683e46416"},
    {"geometry/zone03_largespear03.psg", 1232, "c039e0d9fabc271778fe72361c59a905e51cf3d860767fd16561a16f1b94b480"},
    {"geometry/zone03_largespear04.psg", 1232, "b2b0d0d8c2ee07e497670da570b4ff4e63c287b9fa3933bb6583ff2a788b3534"},
    {"geometry/zone03_newgargoyle08.psg", 68484, "190a43d138d50d91c77dcfb2d8fef45711b085c5694814f34e60f64fd0d04f57"},
    {"geometry/zone03_newgargoyle09.psg", 68484, "6459e4d51dec0e174a7b2ef7e8fa3b8d87540f43af952bb13b3617b9e18192b1"},
    {"geometry/zone03_pillar04.psg", 9572, "d4ce7a68e697fcce8046b656c5ab1c6578503b2b7e7099ed0183de7467275233"},
    {"geometry/zone03_road01.psg", 47968, "c523ba4e3d7ab945eb84454968b920393664df0175445e11c3ea0c745f1ce0d1"},
    {"geometry/zone03_wallledge02.psg", 1632, "19210b72d2ed157e2ddcaf8abff889144ff0010e02188954f8489bf7100965db"},
    {"geometry/zone03_wallstrut04.psg", 848, "0962f8c419c9f24d66dfb02c8545983cddb078db8addf64df85900ec1edc0d0a"},
    {"geometry/zone03pillar03.psg", 9572, "708e6e0e25fd77fa05d7ab7ce2870d4edae8c1c84fd646bbc0e4390155bc766b"},
    {"geometry/zone04_box300.psg", 2064, "d3abf8b4cd7b4e40bd73135ddf201c7d9ea55e654b445dc780e0e081593a2feb"},
    {"geometry/zone04_caveceiling.psg", 16356, "12925e0f24d3ee4a922941a56e3677098f421916aee9c5f7a5d0730b6b2b9e78"},
    {"geometry/zone04_cavefloor.psg", 12468, "25383b0c500179a25ce7a16cd338df46feafdbc23d81484419ce4aae12259df0"},
    {"geometry/zone04_cavewalls.psg", 23364, "774fad576938cc12b8354e4fc9b8fc9c26b79abe2719122cb7a65b5ad205018d"},
    {"geometry/zone04_groundtrick.psg", 45248, "f5a2e1f86965896fd7198dbbd5a1cb76ec8b23854dc397ba272dfa0705c26b67"},
    {"geometry/zone04_road.psg", 156044, "8bf9a61a7ab97b38cd626a016bbd30e0c865c8fa5e6ae1a6302851f07b882718"},
    {"geometry/zone04_rocka18.psg", 4944, "cfb62ffdff8022295381a72091b79f1bbcf220a16461d2b30aaf621cd5e1ce3e"},
    {"geometry/zone04_rocka200.psg", 4656, "28106e7116309b9a0b1716ab86a9b8e552949cc5121f70fb7dec8358247ad8f4"},
    {"geometry/zone04_rocka230.psg", 4848, "9100e16409bc5ea44b9fa68df4d4348b6bea0b5d4154cc1f9b6a8ec5a20d44f7"},
    {"geometry/zone04_rocka235.psg", 5056, "b3f5b8f8a2f1b58f96028a637b152bf52d3093b03d448ea29b7f7c0a89fb755e"},
    {"geometry/zone04_rocka47.psg", 5200, "f6effec262ac7cef858828664e93fc73cd345f47646370ce4342daebd90b8a5c"},
    {"geometry/zone04_rocka86.psg", 5056, "a4c5d718b27c79b6882cd2ec57dd7f031ccf826dafd11ef00f14cf7e5eb6be3c"},
    {"geometry/zone04_rockc03.psg", 3056, "423139408820bcd510e76e1ecaf5215bc99a4b79b52831fdb30a9abcd5c43395"},
    {"geometry/zone04_rockd01.psg", 4608, "08e44ba74928ffa444a5f44c55fe42328444f9b4220f5a714527067d0b947e21"},
    {"geometry/zone04_rockd02.psg", 4688, "4e2b6fb69e16e6e21a87d5b07432f824d9f336447d993686982ba006d178661c"},
    {"geometry/zone04_rockg02.psg", 6576, "a044e2c83c1c67f62b05a0b361da34ee6d7f867991ebe1628c8d986af680bab8"},
    {"geometry/zone04_wallpiece01.psg", 1888, "90182dc578e21fb02ed88adc5841964af77db99d160c3fcd9ea7f901a91de51a"},
    {"geometry/zone04_wallpiece02.psg", 1888, "c36719925d2379312a197b390ea31718f9d0cbc73ed8aae9d74fd301144c17a4"},
    {"geometry/zone04_wallpiece03.psg", 1888, "97776d824afa40cac0991b3f7de7352b2190815f439394ed10a9ed2a5aac604e"},
    {"geometry/zone04_wallpiece04.psg", 1888, "ada1f0e6e83320c5f9cf8d6d8fbc2d56878d061ec8cd5bc05251c951f98d499d"},
    {"geometry/zone04_wallpiece05.psg", 1888, "41f6cacfb0c9c4b36e2f0b83f7664c8dc20dabd0c5f942ea68c6d36768d4bde1"},
    {"geometry/zone04_wallstrut01.psg", 1680, "c612f0cad1f6d424a5cedc6268a36111a78ad49d00bc8bcb8c433a3785db1f3d"},
    {"geometry/zone04_wallstrut02.psg", 1680, "bdbc0db50f18e24a1861fdb596369463080aefe671df747b2ef5dfdf70387d77"},
    {"geometry/zone04_walltrick02.psg", 2736, "86f54320f12ba9376a0483d609b16c04cdfc443402b9371242369407a8f64573"},
    {"geometry/zone04_walltrick03.psg", 1888, "9b7ed6a0d389fad269fd474a357d6c86131a4c6401ed106b545a019f698f2bcc"},
    {"geometry/zone04_walltrick04.psg", 1888, "68e92cdf1a30252ed13818d7c0b2ccd06a6e562bba287610dcd283a32b4c9024"},
    {"geometry/zone04_walltrim01.psg", 3776, "0b83e5cb552d6c7508367bcac5e01d9a3f172bd442618a3535232b05613f70f4"},
    {"geometry/zone05_caveceiling.psg", 26756, "f30496904b6c1ff8ab32befd479be6b88a2a20641893b18e0e4ef6581b95042a"},
    {"geometry/zone05_cavefloor.psg", 19488, "b9fc94d84009a7279413a3a24d45bd12f7f9c9dd9ffcd9daad78fcbddbd2f4dc"},
    {"geometry/zone05_cavewall.psg", 39860, "42413912eacb3f3b4c5748eb8a800281acca9f03403e2d9dba7f6cba97bcb5da"},
    {"geometry/zone05_cliff.psg", 7536, "f3c6fc672ded0899927c91bdf57a5af6ec846d20f9360612f007b4bd64ff3c18"},
    {"geometry/zone05_ground.psg", 146236, "a591eb8dae74c3de688b691f1f3cac87812042a593c31c87ee57d4a72e2fef53"},
    {"geometry/zone05_rocka103.psg", 6480, "7b15251eab0ce2c419149ec3fa155cdb1262da032847783d6b22a90339f23baf"},
    {"geometry/zone05_rocka94.psg", 4656, "3337e5b7c5277a8cac7a7f2c037729783930706fcd1f1b11c8f2588ab5df9e57"},
    {"geometry/zone06_bridge.psg", 8784, "5556543bd323c4d3f09aca98d68d23ebc2acb867694448a7785212fa5310b73b"},
    {"geometry/zone06_ground.psg", 257388, "49266762eb36e83610e5ad796201b41e1722775c8cffa79fd5ea2ccd67e8435f"},
    {"geometry/zone06_pillar01.psg", 7348, "58f3e142c10066bb89f66e30d03fde118a61b462b4f9d8f3340ac6a14bbeda70"},
    {"geometry/zone06_pillar02.psg", 5524, "3bfa7c6dc619e396410c3a63856cdf948bc71850e9e1a7d806ff0125a92923b8"},
    {"geometry/zone06_pillar03.psg", 7348, "fadb8912e341dd41e95aac749aa5866029988aa88c4c57f2977856fd94a17b2f"},
    {"geometry/zone06_pillar04.psg", 7348, "ae9f7480eabf8af0074c96010fb941ab4feda3e87003de49c934f4bc2e2431f2"},
    {"geometry/zone06_pillar08.psg", 7348, "641be46b509259f25d6592f719e4ab75b8f58098d85e261ebdb6a2b94b5f615f"},
    {"geometry/zone06_pillar14.psg", 5524, "4a959766adefb0ecf9e817212f786cc6f3f8e6d8b46177981e99745022b5869f"},
    {"geometry/zone06_pillar15.psg", 5524, "70518b565e37e6a2c45aa20c5355971d7f67234fe966399281d5224d6bbfc705"},
    {"geometry/zone06_pillar16.psg", 5524, "1d263dbb5c56d07d2eccc36bf687a5f331744e0d8413d6a98648d846111d8bb2"},
    {"geometry/zone06_pillar17.psg", 5524, "2d49d78bd4335f54919fe051b7eb56a319bebf33ca3a377d7753af530cb699fd"},
    {"geometry/zone06_pillar18.psg", 5524, "fb67b7ca73fbc1818211114e6d653b816eeaaac742340783400a21b4f840904e"},
    {"geometry/zone06_pillar19.psg", 5524, "b05cf33fd106886791a538724e2ddefe28fc4570ea7d96cdb434fd7efda215b2"},
    {"geometry/zone06_pillar20.psg", 5524, "162a20fb2670dc891aefa25e0196551d6a28afa6aef2daecbcb1498c73afceb1"},
    {"geometry/zone06_rocka36.psg", 4656, "e05b8871d56542987c3de3e872d6c336fe238058ea77c659e6ac26dbd21d248e"},
    {"geometry/zone06_rocka58.psg", 3664, "10c8de025aa5e85ad566a8aeaf60ec045faa2344965fcdc3740120354683a1a9"},
    {"geometry/zone07_box39.psg", 2064, "f6aa7b4ca41f5a6ce6ddc74c681baac4866347c447ec65a61956b47023a50530"},
    {"geometry/zone07_cylinder01.psg", 5284, "6d6bc237304a5492acc1e3dcb2a4b5be18ebac5470aa3e2b1abc2cb8519ca0f8"},
    {"geometry/zone07_cylinder02.psg", 5284, "ded2627d8cc446f645c84d370e9038eece86a6fb290bee390f7231ea3b689ad7"},
    {"geometry/zone07_gateseat01.psg", 2592, "e3c62290971bd73a1f3424ddcb47aa18d8e7f02c3d6b94c467789c0f739db4b2"},
    {"geometry/zone07_gatewall01.psg", 5600, "2da9fd94ed7d7485bab2cbfd976a7863a1e9e69e2eb0489b23038b9bb45a469f"},
    {"geometry/zone07_ground.psg", 115772, "65e22a12f1e519cf6257ff3f80df31c0b8ccd20ad89a425f50563668d55fa4d6"},
    {"geometry/zone07_groundpiecesmall.psg", 1568, "004f49dec2ea2e220327c808d54402c5708dd0cda7ad432b4090be1b6030996a"},
    {"geometry/zone07_groundtrick01.psg", 16656, "d709f57050b15a7bdb02aaf97a1ba94615fda544cb3c2b5afa246d9ee9c8d849"},
    {"geometry/zone07_heavygate01.psg", 19620, "f9d130ebe7900f8b0345aaf216a9f4757fdb8448d2df67a717a6e26237a19e91"},
    {"geometry/zone07_largespear01.psg", 1232, "268dec20f6b00852931b0f38de3893762cf93106c865364e1241aa05d4863c50"},
    {"geometry/zone07_largespear02.psg", 1232, "e9cc6e13f8e650bb48e78050403eaa90f45919aca2b7f29e755b2274b50a54e5"},
    {"geometry/zone07_newgargoyle01.psg", 68484, "e22a1e25b21deec100545db84141a0227814087712607e44698e6b83833ed698"},
    {"geometry/zone07_newgargoyle07.psg", 68484, "c01e9f55f52b597a1383d74c7e7cd77daf55145bbbf8adf8077eb36030a4f0c0"},
    {"geometry/zone07_object01.psg", 4144, "1bc983d48e58bef2223460612440fe333ae70b7665527df6da9a0f2ff5e48749"},
    {"geometry/zone07_object08.psg", 4144, "ee64e21aa71e380d968daac35998e14a981a67d63db332f1edc28e79d7628ed6"},
    {"geometry/zone07_pillar01.psg", 9572, "07596169eae0e60069a88e6f507cb05e0efa1e09972d25681000d060a46fb04d"},
    {"geometry/zone07_pillar02.psg", 9572, "7b4296355f03394143e674ed97e4f84ddafe52fbed3b9ced6c80d80ceceba26b"},
    {"geometry/zone07_rocka90.psg", 5040, "41595f87ae6548a8fd67aa1669a8ba9efa82a3d5c9dd9a8d31e2b1baba996b4f"},
    {"geometry/zone07_rockd03.psg", 4800, "9e201b3b49fce4c2e5172604c963b7cc00b0f4af390501c7361049965d569bcd"},
    {"geometry/zone07_sand.psg", 9760, "962e06f5ac7916675dc19f93fc837785adbabf83e41101622bf7fa305a7e73f6"},
    {"geometry/zone07_tusk01.psg", 31520, "1a2848fc82b316b8c23f5e96337b01f55f1dd0b39c23ccf8a5fb31c673534b97"},
    {"geometry/zone07_tusk02.psg", 25856, "8322ea49b1b1bec4c00caee86f02068d24b2746f0cdcb283fcf90bf7ef2cb80d"},
    {"geometry/zone07_wallgate02.psg", 7600, "2663dc644b7bd2313bc9de016a6da5c95c64a222c280021cd80013b4a0bd2c53"},
    {"geometry/zone07_wallledge01.psg", 2736, "135e13f599794c6795436c07d0dd4a4e5e0905988445ef069b72b7c484c97848"},
    {"geometry/zone07_wallpiece06.psg", 1888, "b4f187d446da98a35f82ba4a594d48f26de2482824203cc02b9add0aa8f791c2"},
    {"geometry/zone07_walltrick02.psg", 1680, "a79310a37fb4f4233077349adfcdaea69327626122d55f410dc0911c38fa9480"},
    {"geometry/zone07_walltrick03.psg", 3776, "4cb27cba24ea007d2a4fc5e12e763e84da0090d6cbdf4faef15a97525908d91b"},
    {"geometry/zone07_walltrick04.psg", 1888, "ec518094caaa19c529f78b59ed3de2d648a8199bfc21383219760a4b85b074d6"},
    {"geometry/zone08_road.psg", 6640, "608e3237d24cb01967751dd45efa6481b23fca77674cdcf609b9999c5f38ed5e"},
    {"geometry/zone08_wall01.psg", 13680, "6cf7e4203f97c98125d6de7075e5cbebd692d1253244f65bafc8e637ecdc3792"},
    {"geometry/zone08_wall02.psg", 10256, "a0e771d6ec1056c39714b94b610dd7ec34a6637bb28874613b1584dbc9251c76"},
    {"geometry/zone09_mountainchunktrick.psg", 34176, "717cb1bbe7840f78d4d9b2dbb109dbc15003e65a938c302eb3d41bfa0cf61919"},
    {"geometry/zone09_road.psg", 162108, "ab3e423ebfd206a3e63905811f68e218909f16f5450d00a9c036607a45a8c8ee"},
    {"geometry/zone10_ground.psg", 101304, "ba913973a6f1fd29ee82af4074717e7de2e6e916f9ad882f8882d539d69e7442"},
};
constexpr Identity compressedMaterials[] = {
    {"textures/arch01.psm", 259, "700cbab2d3ad624ba89ce50d0ac8416ae0343110961cb3e054bf38e5afa573da"},
    {"textures/arena01.psm", 260, "b8993847ae13b42e7dad0751bf2b72b69aaa4ffe0da39ca000359e6c7abe847c"},
    {"textures/arenahead01.psm", 365, "ecef52f1be0b25e9b14e05531c2bbe96905a716dca312f9c9da695a21b47eef4"},
    {"textures/arenatile.psm", 262, "e32db184c4e00912d458f5a722b1b679f5390111139a0cf8bc4dcf15b50125aa"},
    {"textures/black.psm", 259, "cbdad0b39874455433389179982b16d8f8a132853011bc90607c419c96796755"},
    {"textures/bridgeground01.psm", 267, "d8b8c791933fc0721308bf915e07271648d59a6289b4ac829350ed299755abc6"},
    {"textures/building01.psm", 263, "dc55294e03c47c577a0eeb38e878b8235a31c94b06250012ade665d50b1210c1"},
    {"textures/building02.psm", 263, "eebcfe5d3747f8a2ab0634d5aabbf4e04f29eb37af21540e8fcd5686f77dff78"},
    {"textures/cityscape.psm", 2530, "a62f0c354df99bf946d61f37803a729bdf5733f69fdcdae4f6881c46cb139b4d"},
    {"textures/cityscape02.psm", 2532, "550f20bf42c409e27bf985cde15209a7b799900aedb92c4fbc1132a7d47171eb"},
    {"textures/gargoylebody.psm", 362, "8540bd531107844a55c9aa4e8973f1860b834c7ae2e861a9265f68d70204cf47"},
    {"textures/gargoyleface.psm", 362, "0544d8103a4ce3e4e64ee1340b56ced73151ec2593d4ccc6ab0ae5d70c0ca648"},
    {"textures/gatebottom01.psm", 265, "b3acce2f2f699eb32938814f607687ab3006c4fc2056390441da6f5d4a695ec7"},
    {"textures/gatedoor01.psm", 5174, "a6b8d8b71d2ee389369d40dc57dd65a46b68ca5ce6ab411ceb872c2aaf5c8189"},
    {"textures/gatepost01.psm", 5174, "506c4bd56470d4faaa31a7192b33e2418ec3a52de4a126216c2d629f1169c8d3"},
    {"textures/grayrough01.psm", 264, "f6a88cd9ed9c9d086a196185225285a307606fcb2b47cf49857f516d3e5db0bf"},
    {"textures/largeflag.psm", 2585, "2b6a629518ae0b994d7608568d6d58864b930f440be99400e269daa84b1ef9fe"},
    {"textures/lightcoloredtile.psm", 269, "88fe5f27bcd378b23fe69b39dc13583a23018adb12f723ad6d12aa237b903b3b"},
    {"textures/rockcavewall.psm", 265, "068198274a63ba568597132533749d6a13c84204ace912a7b2539b26de2bce57"},
    {"textures/rockcavewall02.psm", 267, "1b3e61d674308b469010ac944f4fd86d816ef40e2974d942bed48b07e353ea8e"},
    {"textures/rockfloor.psm", 262, "f4f68497167504da6fcf5502367399b8da27b42fda87a0f0d73ddb522c225de4"},
    {"textures/rockfloorfade.psm", 266, "3e4eb86d2c8a65ef1729d0b1d828dc9d810b42a992af6727fd357f898ca6c710"},
    {"textures/rockwall.psm", 310, "986e11725576128fa9aa3b2b8fa738596e818f698fddbc13e8b224f593a09deb"},
    {"textures/sand10.psm", 356, "f0a49b1aea55c264e16439500b3e4bee16a26719689fb2c88d3db21c1c1b7ce8"},
    {"textures/sand11.psm", 259, "320968c362db6a80da4bada7ca4fced589c758713c8855f5bc8ef32b95a17d4f"},
    {"textures/sand8.psm", 355, "24103e904052a040f1869f11ceba90c31942839491246803ca786e40756866d5"},
    {"textures/spear.psm", 258, "c09b8a0c31ea04255c905b153a4ff7df7cc6dbc42ce80ac256175428cef5bafc"},
    {"textures/spearcabledark.psm", 267, "cf685214ee0dca28be15eae78271110215277d40475d9c2089e26df5aed4e391"},
    {"textures/spearholder.psm", 264, "85a38479ea3b01cbef916da4f79308562924da02c2a6b12fc0926937603bb8ee"},
    {"textures/stairs01.psm", 261, "3f8ab335c709e10f55ed06b69bea14b3645cf95a97bda1948b9381eacfaecb1e"},
    {"textures/theskull01.psm", 263, "65fe970f2ae8d4a8b5c3e08a1a68560a5e04a888f049f261435c9197046d1e37"},
    {"textures/tile003.psm", 5168, "16b617d5fec0a745706f5b989361d76f43161daaf873d7306e2a7228f5de62db"},
    {"textures/twidesigns.psm", 263, "4ac8a3eaa8e59e857b43e435456ae023fb4cd47f0aa7363d03f2cbb05b5fa7ff"},
    {"textures/twidesigns02.psm", 265, "675453947b6b308444b9fe98d0bafeb848b38d4c7eee52ab1cb677afab12493c"},
    {"textures/twilekflag01.psm", 265, "92ed32f7acb7005b7081fd69151ab50ff87565bbf075b3dd7a59b6804da4ce84"},
    {"textures/wall002.psm", 260, "2a822b4d7adc9c7fb0de9bbc6f4e75d178070f716bb6d9c66a5c1b1838b8a20e"},
    {"textures/wall_design01.psm", 266, "914b53ffec4a9ad7692cda45014362eed482f5b9423669c729a775ae8ed9504c"},
    {"textures/wallside01.psm", 263, "1100dfbe72aa14df4ae6b808a63fa1b8fb4316302694a4b7993df56b9afa1d63"},
    {"textures/wallside01_blend.psm", 269, "1483e2a57221286ac31387918c11ff102e9f5efb847ea4d0a88dbc29fc3683bf"},
};
static_assert(sizeof(wideScenes) / sizeof(wideScenes[0]) == 583, "Exact RA static wide-position source set");

inline void require(bool good, const std::string& reason) {
    if (!good) throw std::runtime_error("RA compressed admission: " + reason);
}
inline std::string normalized(std::string name) {
    for (char& c : name) {
        if (c == '\\') c = '/';
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + ('a' - 'A'));
    }
    return name;
}
// Standard SHA-256 steps reused from original_ui_archive50kg.cpp. This cold
// admission has no cancellation callback and never modifies input bytes.
inline std::uint32_t rotateRight(std::uint32_t x, unsigned n) noexcept {
    return (x >> n) | (x << (32u - n));
}
// Standard SHA-256, fixed stack workspace; no member-sized temporary copies.
// Constants are the SHA-256 constants also retained in audio_assets50hc.cpp.
inline void shaBlock(const std::uint8_t* block, std::array<std::uint32_t, 8>& h) noexcept {
    constexpr std::uint32_t k[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
    std::uint32_t w[64]{};
    for (unsigned i = 0; i < 16; ++i)
        for (unsigned j = 0; j < 4; ++j) w[i] = (w[i] << 8) | block[i * 4 + j];
    for (unsigned i = 16; i < 64; ++i) {
        const auto s0 = rotateRight(w[i-15],7) ^ rotateRight(w[i-15],18) ^ (w[i-15] >> 3);
        const auto s1 = rotateRight(w[i-2],17) ^ rotateRight(w[i-2],19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    auto a=h[0], b=h[1], c=h[2], d=h[3], e=h[4], f=h[5], g=h[6], v=h[7];
    for (unsigned i = 0; i < 64; ++i) {
        const auto s1 = rotateRight(e,6) ^ rotateRight(e,11) ^ rotateRight(e,25);
        const auto ch = (e & f) ^ (~e & g);
        const auto t1 = v + s1 + ch + k[i] + w[i];
        const auto s0 = rotateRight(a,2) ^ rotateRight(a,13) ^ rotateRight(a,22);
        const auto maj = (a & b) ^ (a & c) ^ (b & c);
        const auto t2 = s0 + maj;
        v=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=v;
}
inline bool hashMatches(const std::uint8_t* data, std::size_t size, const char* expected) noexcept {
    std::array<std::uint32_t, 8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at = 0;
    for (; size - at >= 64; at += 64) {
        shaBlock(data + at, h);
    }
    std::array<std::uint8_t, 128> tail{};
    const std::size_t remainder = size - at;
    if (remainder) std::memcpy(tail.data(), data + at, remainder);
    tail[remainder] = 0x80;
    const std::size_t padded = remainder < 56 ? 64 : 128;
    const std::uint64_t bits = static_cast<std::uint64_t>(size) * 8u;
    for (unsigned i = 0; i < 8; ++i)
        tail[padded - 1 - i] = static_cast<std::uint8_t>(bits >> (i * 8));
    shaBlock(tail.data(), h);
    if (padded == 128) shaBlock(tail.data() + 64, h);
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned i = 0; i < 64; ++i)
        if (expected[i] != hex[(h[i / 8] >> ((7u - i % 8) * 4)) & 15u])
            return false;
    return true;
}
inline void requireIdentity(const Bytes& bytes, const Identity& identity) {
    require(bytes.size() == identity.bytes &&
        hashMatches(bytes.data(), bytes.size(), identity.sha256),
        std::string("source identity differs: ") + identity.name);
}
template<std::size_t N>
inline const Identity* findIdentity(const Identity (&identities)[N], const std::string& name) {
    for (const auto& identity : identities) if (name == identity.name) return &identity;
    return nullptr;
}
inline Bytes sourceMember(const rr::assets::ResArchive48C& archive, const Identity& identity) {
    const rr::assets::ResourceEntry48C* found = nullptr;
    for (const auto& entry : archive.entries) if (normalized(entry.name) == identity.name) {
        require(!found, std::string("ambiguous source member: ") + identity.name);
        found = &entry;
    }
    require(found != nullptr, std::string("missing source member: ") + identity.name);
    Bytes bytes;
    rr::assets::DecodeError48C error;
    require(rr::assets::extractResMember48C(archive, found->name, bytes, error),
        std::string("source extraction failed: ") + identity.name + " " + error.code);
    requireIdentity(bytes, identity);
    return bytes;
}
inline std::uint32_t word(const Bytes& bytes, std::size_t at) {
    require(at <= bytes.size() && bytes.size() - at >= 4, "truncated source material table");
    return std::uint32_t(bytes[at]) | (std::uint32_t(bytes[at + 1]) << 8) |
        (std::uint32_t(bytes[at + 2]) << 16) | (std::uint32_t(bytes[at + 3]) << 24);
}
inline void requireCompressedMaterials(const rr::assets::ResArchive48C& archive, const Bytes& scene) {
    // Exact pinned inputs are all PSG3 with one 224-byte node. This reads only
    // the actual source material name table; geometry stays with decodeScene48D.
    require(word(scene, 0) == 0x00677370 && word(scene, 4) == 3 && word(scene, 8) == 1,
        "wide-position source must be the pinned single-node PSG3 form");
    constexpr std::size_t countAt = 12 + 224;
    const auto count = word(scene, countAt);
    require(count >= 1 && count <= 5 && scene.size() >= countAt + 4 + count * 64u,
        "source material table extent differs");
    for (std::uint32_t i = 0; i < count; ++i) {
        const auto begin = countAt + 4 + std::size_t(i) * 64;
        std::size_t length = 0;
        while (length < 64 && scene[begin + length]) {
            require(scene[begin + length] < 128, "source material name is not ASCII");
            ++length;
        }
        require(length > 0 && length < 64, "invalid source material name");
        const std::string name(scene.begin() + begin, scene.begin() + begin + length);
        const auto path = "textures/" + normalized(name) + ".psm";
        const auto* identity = findIdentity(compressedMaterials, path);
        require(identity != nullptr, "unproven compressed source material: " + path);
        const auto bytes = sourceMember(archive, *identity);
        rr::assets::Material48E material;
        rr::assets::DecodeError48C error;
        require(rr::assets::decodeMaterial48E(bytes, material, error),
            "source material decode failed: " + path + " " + error.code);
        require(material.passes.size() == 1 &&
            (normalized(material.passes.front().microprogram) == "prelitcompressed" ||
             normalized(material.passes.front().microprogram) == "diffusecompressed"),
            "source material does not select the proven compressed program: " + path);
    }
}
} // namespace detail

class Admission {
    bool authenticated_ = false;
public:
    void authenticate(const detail::Bytes& originalArchive, const rr::assets::ResArchive48C& archive) {
        authenticated_ = false;
        constexpr detail::Identity original{"TRACKS/RA.RES", 4604561,
            "1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2"};
        detail::requireIdentity(originalArchive, original);
        detail::require(archive.entries.size() == 1010 && archive.decoded.size() == 29687808,
            "original RA resource extent differs");
        // Authenticate all opt-in dependencies before creating any output files.
        for (const auto& identity : detail::wideScenes) {
            const auto bytes = detail::sourceMember(archive, identity);
            detail::requireCompressedMaterials(archive, bytes);
        }
        authenticated_ = true;
    }
    rr::assets::SceneLimits48D limitsFor(const rr::assets::ResourceEntry48C& member,
        const detail::Bytes& bytes) const {
        rr::assets::SceneLimits48D limits;
        if (!authenticated_) return limits;
        const auto* identity = detail::findIdentity(detail::wideScenes, detail::normalized(member.name));
        if (!identity) return limits;
        detail::requireIdentity(bytes, *identity);
        limits.allow_signed32_quantized_positions = true;
        return limits;
    }
};
} // namespace ra_compressed_scene
