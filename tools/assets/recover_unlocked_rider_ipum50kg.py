#!/usr/bin/env python3
"""Cold preparation of authenticated fresh-unlocked Rider IPUM sources.

Run only with explicit authorization. Uses the unchanged native host50FC
converter, never guest code. Existing Rider19 cache/catalog is not overwritten.
Gasgano07c_2 is explicit opt-in; source pins do not establish decoded output.
Selecting it regenerates the complete original batch plus Gasgano, never a
Gasgano-only replacement catalog. All outputs retain this run's provenance.
Clegg03c_1/2 opt-in regenerates that complete batch plus both Clegg sources.
This generator does not merge historical provenance: central integration must
retain the previous eight records and append only genuine new Clegg records.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import shutil
import struct
import subprocess
import sys
sys.dont_write_bytecode = True

DEFAULT_SOURCES = (
    ('04c_1', 39441, 'ca086fb878b398f6fccdcb2161d65ff38bacdb2ee5a3c96ff06e1ed814dbfd27'),
    ('04c_2', 33310, '5f5979fa86f0043be411d84bfb33244921482290eda4c0f4df50e4af1faa32ff'),
    ('10c_1', 23311, '99ccba918bd470d396ebb618b382a81907839a34d0b34b9beab8b5a09e9c0722'),
    ('10c_2', 18494, '9efd4f229c3203e9a27252cac070a1107c83598a7bc536f51a9c46cbb4852ae5'),
    ('12c_1', 22581, 'eecda390eca1ea261f80ba992a37a225613f12d6bc9d0e9545d2380f64662b32'),
    ('12c_4', 22860, 'c3aaa742f80f395f8fc01e0c564acbdea652835038e219c16c3de0a8803f5c7d'),
    ('15c_1', 25470, 'e02462b1c14fc1498a46b91b4245dc8453da45ffe674b70eae080828fdef557d'),
    ('15c_2', 31682, 'cf7093e713ad87e15301435a51beb6cd2cf958dfe74c72dc2657f7cbf1c1ccd5'),
)
# Independently observed original UIRIDER bytes; preserve the default batch.
GASGANO_SOURCES = DEFAULT_SOURCES + (
    ('07c_2', 29512, 'cbd5e15d11e7b1b9336a1e8f965b18ecdb93d948e770baa2bc3005e3576b11ce'),
)
SOURCES = GASGANO_SOURCES + (
    ('03c_1', 21039, 'abfcdd11b1c519b9e5a66015016a7f93875b4050fd802e961a7ffc90f11b4293'),
    ('03c_2', 25184, '8d1e23f9284bd6eb2b22b0d2e6e1ffce48d2474efecaa46a553086ddb8537916'),
)
# Existing seven generated identities, in their retained catalog order.
# This is a membership requirement, not copied pixel/codec provenance.
BASELINE_KIND2_NAMES = ('04c_1', '04c_2', '10c_1', '10c_2', '12c_1', '15c_1', '15c_2')
# dimension, PST header, payload offset, compressed payload bytes, SHA-256.
# These are input identities only, never generated pixel hashes.
GASGANO_MIPS = (
    (16, 12, 32, 216, '51c6ffdc77b9e50ffff5d9b7d00241f4da0c6a1d9de99cc84423f24e660321a2'),
    (32, 248, 268, 776, 'a93eec935f3de3fec135477ff4f22f9ebb9889acbeacc1d85f1197548ebef856'),
    (64, 1044, 1064, 2392, 'd525e93bf42b571ebaf2327a5f00bc4ab2c2b80b2dd242aeb43c79644cc2ba87'),
    (128, 3456, 3476, 7101, 'fbb471df280643efeb1afa45fdc8b9d29b44da9a12c270900ba83aa25e021424'),
    (256, 10577, 10597, 18915, '89b135287cd4fbc7a0ecbeda1105396abe6eb8ca1dfab643553f8416fb7f3e6d'),
)
CLEGG_MIPS = {
    '03c_1': (
        (16, 12, 32, 224, '4a53e555980dae8c11723806cc8b51da3ba474b64eb6c5ba1edcdef490b1675b'),
        (32, 256, 276, 628, '66bb59bf9ab1089a29fc6cda009f938b1c4f6093e8b51e78f89ee1b2f804d327'),
        (64, 904, 924, 1860, '770a5f874c1cf9077fdd3e45634f937f93b17cd5269b414e1d27ac35d18ab427'),
        (128, 2784, 2804, 5001, 'e773799648dde43f8f4785c435c2f83b5897c92e111bd4ea7effc3c17cdce726'),
        (256, 7805, 7825, 13214, 'b1fa6aabd54ade02cc17157a3e711791f7b03351e8174292a27090adee8528ec'),
    ),
    '03c_2': (
        (16, 12, 32, 185, 'b95b149068cf247b47a96f8daaca6719851c8713c480614f98f54329b07ecdbd'),
        (32, 217, 237, 610, '274762b0dd7bab0534eb92fccf77d57e748e92173b584d25be349ef92cb11d6c'),
        (64, 847, 867, 1947, 'f2f492634bbec0b5d122a3b7539420546a3af5cfffa512471bc4ec83e0d56369'),
        (128, 2814, 2834, 5680, '0e2b4a5996a8cc6c9174d73700109c5c86956c18c9ebf899f6fa916653b39236'),
        (256, 8514, 8534, 16650, '99453b54a9ed3114e879a459ba2cb62f1ac08ec0e6c98ba8ba04700e92325b4e'),
    ),
}

# Independent original source/layout pins for explicit complete-preview preparation.
COMPLETE_PREVIEW_CAPACITY = 29
COMPLETE_PREVIEW_SOURCES = (
    ('04c_1', 39441, 'ca086fb878b398f6fccdcb2161d65ff38bacdb2ee5a3c96ff06e1ed814dbfd27'),
    ('04c_2', 33310, '5f5979fa86f0043be411d84bfb33244921482290eda4c0f4df50e4af1faa32ff'),
    ('10c_1', 23311, '99ccba918bd470d396ebb618b382a81907839a34d0b34b9beab8b5a09e9c0722'),
    ('10c_2', 18494, '9efd4f229c3203e9a27252cac070a1107c83598a7bc536f51a9c46cbb4852ae5'),
    ('12c_1', 22581, 'eecda390eca1ea261f80ba992a37a225613f12d6bc9d0e9545d2380f64662b32'),
    ('15c_1', 25470, 'e02462b1c14fc1498a46b91b4245dc8453da45ffe674b70eae080828fdef557d'),
    ('15c_2', 31682, 'cf7093e713ad87e15301435a51beb6cd2cf958dfe74c72dc2657f7cbf1c1ccd5'),
    ('07c_2', 29512, 'cbd5e15d11e7b1b9336a1e8f965b18ecdb93d948e770baa2bc3005e3576b11ce'),
    ('03c_1', 21039, 'abfcdd11b1c519b9e5a66015016a7f93875b4050fd802e961a7ffc90f11b4293'),
    ('03c_2', 25184, '8d1e23f9284bd6eb2b22b0d2e6e1ffce48d2474efecaa46a553086ddb8537916'),
    ('01c_1', 20202, '6c6fbaa2447cdf6232532f12d07f6307a1ed9b073e0b4f7c32361706ae5d4aed'),
    ('01c_2', 21731, '163fb5c6d28bff55e7eeed4f91adedb0402b0d5ad4e2f6be2a6a7e7856e8a154'),
    ('08c_1', 38348, 'bdbe402cf0569f79bace8e19b773b5ceeaa51e0792dccfab3739dfae8fdab33a'),
    ('08c_2', 31598, 'f9d8b1e23a8633342f7befd597b9f8ba8864d150b9087545a36da48d65225e65'),
    ('09c_1', 25778, '3e59ec8fcb97d760a97c140dc49428c62feb72cc40f6e5166bcf7e7b83fe75d5'),
    ('14c_2', 35909, '8d5e381345a903586560c00a977068011a348477ea81c47984bfb400f1386899'),
    ('16c_1', 44455, '347dcbadc92150ef675a170fb61dc065d4dd63a75b3e518be84c84665f4aecf1'),
    ('16c_2', 35313, '98714dbc1865359e959179503db9025c7f5f208760515188a43cff63bfd06ea5'),
    ('17c_1', 38598, '250b5fc43687171fc04ca3477a4d1bd55881f778155b81f22ecc07e3e2cec2b7'),
    ('17c_2', 38761, '9aad2297988bec55e8dca4d21664604661273da961d1ba2e7010724857fa6f60'),
    ('18c_1', 34026, '70e8503a0e413489d8c6236a48847a21a82e7421dfc3351ad2e87b422206e0ff'),
    ('18c_2', 35391, 'c88c2dcd81b9bf680d8b0e91e064a9787c8a717e3a9b2425a82831ef3abb5b09'),
    ('22c_1', 41051, 'c189a488d0ad67d1a7a24ba97ec5f7777c2432c3019c7dcd1818b1fbe2d056da'),
    ('25c_1', 33012, '3b34c48a254652b9f2d52d9425d3a7016a6e275c11b528cd7b9c16e64dad590a'),
    ('25c_2', 36257, '1d461875c5f73d8acdd0a2c3a75dae62fc2a64328dc0ca43a514e45b9eff9613'),
    ('25c_3', 7908, '9beeb102cd83c2c2efe8b9a4c75c4e0837132e5ba8981619a54e62fbba5f6f8e'),
    ('25c_4', 5825, '3672ccb0eca3dc4f5c180f90f585d7a3f126bc6d8545a8250044f40b9961622e'),
    ('25c_5', 1572, '2ade36706c9f0037265d9c46062fb6b67cf2fcace6a090a74fc4611c61b2b54c'),
    ('25c_7', 2591, '1666b0d687043b785aad88cf78aa06aed446a10daa91a28f0cea0f561c0e0af5'),
)
COMPLETE_PREVIEW_MIPS = {
    '04c_1': (
        (16, 12, 32, 244, '064b4ca5f65497a405dc9925a6e3ff91bcc977339d63ddc1fa738a56d5ea7949'),
        (32, 276, 296, 832, '9a491ee97b2e17741985bb8dffa36f6e09a3fe5ec86e198f2d91d3587c73998b'),
        (64, 1128, 1148, 2883, '490b3414939b75d9e16debd0d4f37971ef397903e14b1060a2ffb6136ea373c3'),
        (128, 4031, 4051, 9063, '7c1aea0b0bc918946855cdf8aaafd72b843fa34fc0e0506a74710b4585c65599'),
        (256, 13114, 13134, 26307, 'ada03cfc2aba63bc53ca8c545be3b9be4947a49d89fe6b41c02440b53b30cf20'),
    ),
    '04c_2': (
        (16, 12, 32, 230, 'c8cbde141dc2a6f71f52a0ee182a0c0031f69923e031cd023487cf574c4b6a48'),
        (32, 262, 282, 743, '8c35fd5933638443dbf0d200d781a4cb6e2f8fb0b13a5a110ad7ef865033cebb'),
        (64, 1025, 1045, 2476, '7c1114e31d5a18f4a00e32497e4249370b3468ac0709f8b7181718d5f64f4322'),
        (128, 3521, 3541, 7463, 'ed1a614b2e8827c7a1061c64ddb61ebd21381caab86042e05add77027142ede4'),
        (256, 11004, 11024, 22286, '623351be7ebc1c6833d1104dcf99708126a61b0295c2582e4f6f79a2fe50468b'),
    ),
    '10c_1': (
        (16, 12, 32, 220, '112569e2541fced9df0a877765da5677e3e3cbc53f6be523f6c662c228267aab'),
        (32, 252, 272, 651, '069e0d01774efac41d4aff3321b94811e0f4efe41017f096fa3edd03f09fcf1c'),
        (64, 923, 943, 1969, 'f787c7e092af9646490a97260c1f1ac260ac8d03b348f499c1927866d9f89e34'),
        (128, 2912, 2932, 5522, '9dbd9f3167a527e60fda04c7f389596e8cdf4e859fbbff548bec3ce54727ad41'),
        (256, 8454, 8474, 14837, 'a2a6578a40d4d31ca0accebacc4fa1a84f49913cbb986ec7d0c2b20fece52df5'),
    ),
    '10c_2': (
        (16, 12, 32, 177, '428fc57e65e8ae56408fabb1edd40d868080f7bb1a10adab5599b65431cba0cf'),
        (32, 209, 229, 438, 'a33c607667a73eea8e04440044cb4488bf6548ed039256df43d8608255f9036a'),
        (64, 667, 687, 1359, 'd3ea1a118647206bfcbc10a8c961f23f2ae6cd11b7c608dbe0dd02ac78b1032b'),
        (128, 2046, 2066, 4101, '18f585985a66b160fbad9c714fa876010d5718f184c37a837890e6da8008b22d'),
        (256, 6167, 6187, 12307, '5d1786675118a0f746d075c8f4886389daa6664bb95c0c301fb3c77e4bc3d5c8'),
    ),
    '12c_1': (
        (16, 12, 32, 169, '3dd44524376ca3df4d890d023052c809fe1f285b8f55fdde103540565d07fe14'),
        (32, 201, 221, 492, 'cf2c2cf2d8e8644ad1c6e0136fb458c1dafad450645f6a7215b163c50769bc92'),
        (64, 713, 733, 1487, '3cd4f0b1ee1c04379a7709a1829c5ad4cd263c68611af93d66775e8bb46acfb1'),
        (128, 2220, 2240, 4947, '0261e520ab702b88239b187a404aeecc10dc0fadbc6140bfbdf8ebb812830e1a'),
        (256, 7187, 7207, 15374, '7b23ad7b6836896b7b1d5b1b1748f986128a910f6d0b7cbe0a5aebc3abe9aec2'),
    ),
    '15c_1': (
        (16, 12, 32, 190, '14e9fce2e84ca530ee87674895818b37fef2cc2148cc2fe66558a8cbcca82256'),
        (32, 222, 242, 541, 'e60aaedc04fd7137021c89e2cfd63f12aebb8d80bb7fb9c74e2bb151e347aeb5'),
        (64, 783, 803, 1796, '9dc4d093d374752263f408953ff0d44f496b1e8c2b5db7d4772e41975be88424'),
        (128, 2599, 2619, 5691, 'd56bf2774be34c1f88d77eb6516ed2bb94e3606d9e7eadd131f361f80c22b095'),
        (256, 8310, 8330, 17140, '073186ba4379c2766043729002deec7eb2aba0caff01397d15c5143569f55570'),
    ),
    '15c_2': (
        (16, 12, 32, 204, '088fa0ccf387498d7fd2bd0f6a47312d48bd27ced025460cf2ddc9581b606538'),
        (32, 236, 256, 652, '8214047ecb31394ec19042bc4e30ca86deb6f318e1b9b341691452ab84b5d55d'),
        (64, 908, 928, 2159, '863cc0d2b76c103f77608dd0d6032c7b8fa2c6a57d2b1cf8e733a5f1d4e1de90'),
        (128, 3087, 3107, 6953, '4e2f9b6f21ca07081e3ad53309f0eb969b10076c848b9e6d1f475321b2909877'),
        (256, 10060, 10080, 21602, 'f39928eeb077db7be0f40b0699383d7439bb819007d1ddf705fdfbeb70140eda'),
    ),
    '07c_2': (
        (16, 12, 32, 216, '51c6ffdc77b9e50ffff5d9b7d00241f4da0c6a1d9de99cc84423f24e660321a2'),
        (32, 248, 268, 776, 'a93eec935f3de3fec135477ff4f22f9ebb9889acbeacc1d85f1197548ebef856'),
        (64, 1044, 1064, 2392, 'd525e93bf42b571ebaf2327a5f00bc4ab2c2b80b2dd242aeb43c79644cc2ba87'),
        (128, 3456, 3476, 7101, 'fbb471df280643efeb1afa45fdc8b9d29b44da9a12c270900ba83aa25e021424'),
        (256, 10577, 10597, 18915, '89b135287cd4fbc7a0ecbeda1105396abe6eb8ca1dfab643553f8416fb7f3e6d'),
    ),
    '03c_1': (
        (16, 12, 32, 224, '4a53e555980dae8c11723806cc8b51da3ba474b64eb6c5ba1edcdef490b1675b'),
        (32, 256, 276, 628, '66bb59bf9ab1089a29fc6cda009f938b1c4f6093e8b51e78f89ee1b2f804d327'),
        (64, 904, 924, 1860, '770a5f874c1cf9077fdd3e45634f937f93b17cd5269b414e1d27ac35d18ab427'),
        (128, 2784, 2804, 5001, 'e773799648dde43f8f4785c435c2f83b5897c92e111bd4ea7effc3c17cdce726'),
        (256, 7805, 7825, 13214, 'b1fa6aabd54ade02cc17157a3e711791f7b03351e8174292a27090adee8528ec'),
    ),
    '03c_2': (
        (16, 12, 32, 185, 'b95b149068cf247b47a96f8daaca6719851c8713c480614f98f54329b07ecdbd'),
        (32, 217, 237, 610, '274762b0dd7bab0534eb92fccf77d57e748e92173b584d25be349ef92cb11d6c'),
        (64, 847, 867, 1947, 'f2f492634bbec0b5d122a3b7539420546a3af5cfffa512471bc4ec83e0d56369'),
        (128, 2814, 2834, 5680, '0e2b4a5996a8cc6c9174d73700109c5c86956c18c9ebf899f6fa916653b39236'),
        (256, 8514, 8534, 16650, '99453b54a9ed3114e879a459ba2cb62f1ac08ec0e6c98ba8ba04700e92325b4e'),
    ),
    '01c_1': (
        (16, 12, 32, 184, '841f94fbc864d392e53f102480086c31b03f63a03e5b58b8c176592a94fe7b29'),
        (32, 216, 236, 544, 'b5d1f269624d1555cd81ea49060431b7b275c930846cc6bfa2ad74db878d97f4'),
        (64, 780, 800, 1673, '8da782b1630ec36acb801ec9cd6a8149802afe5cb71f52beed2657ea891243b3'),
        (128, 2473, 2493, 4651, '54ca4ac51bd7b02521e3bd37887f3fb61ea50c96c085c56b0b95002016c57f9f'),
        (256, 7144, 7164, 13038, '09d2839daf5ba34e433ed6327b051fc65067b837b9b131b3f3fa951e602f291d'),
    ),
    '01c_2': (
        (16, 12, 32, 188, '65256c33278a91e52c047b02f5711a6c18782dbc0521a6b5b2b6c36fc74b6b44'),
        (32, 220, 240, 486, '802cf88138129e809ec5c522336989a71f1d75d764a4cbdb6ce1b67965709373'),
        (64, 726, 746, 1416, '95fba8079da2919d1905555567718b827055ea9ac62c69241eb3ae12d2dcc5e2'),
        (128, 2162, 2182, 4572, 'ddb6422654e7e251503b1965a58951d95df73b4ab45981344901f6b9af784985'),
        (256, 6754, 6774, 14957, 'b48dc0171c56649c1b9e58dc0413364968fb7d7d0e47e50b7cba44619aa551c6'),
    ),
    '08c_1': (
        (16, 12, 32, 285, '159e55000aca2625cb2acd17b383ec67e3f9b404449b20975a93b677b8ee5568'),
        (32, 317, 337, 879, '25038a6d42edb12b97935517c2a2b174b517aaa4eac1ccd8a89c99e617d49af4'),
        (64, 1216, 1236, 2767, '29ad51212be026f7401e200da311a7e46680b0ab5647388fe2aa841b53c25d05'),
        (128, 4003, 4023, 8821, '70ecc19bc778f77d8b4643263e11aa21ca9ce40e7ded5bc3bfdec7268f2c7f71'),
        (256, 12844, 12864, 25484, '07d143ba1f953a251c7b6bdc503562d5a244278165275ba90e00f80360d4a1c0'),
    ),
    '08c_2': (
        (16, 12, 32, 214, '79d7acf913d8f173f01a7fe5e89e3ef62836624224bf2f9ec0c5294838ee098d'),
        (32, 246, 266, 683, 'ee8df29836f7dea032bad072a92e0ca9bd2487d7c2423767114457512aafb6e1'),
        (64, 949, 969, 2258, '2c86ab06b6ab0f8afb3b0f6140e252d226feb0b72c8217ef735eac239bfb9f7b'),
        (128, 3227, 3247, 7329, 'd0d9a63651e78a56092a4fa90424a532516b918fbd041d4e5bd5aa8c31d586ae'),
        (256, 10576, 10596, 21002, '2841153ba123749ffe80b1cd6446ef9d8e7080e9893d1b0278771b7463e6de0f'),
    ),
    '09c_1': (
        (16, 12, 32, 213, 'dbf517782054d8d3bca31ecf7ed2d7a1fd71905e5ef1f843583b086a82986e9e'),
        (32, 245, 265, 620, '66350afef20d90846897454a38c752d75f485c506238f15e7ff72a0190c00e03'),
        (64, 885, 905, 1837, '7fee78c699be6808110f6d4cb597c7b14d4566e7400d13dafb40ac43c5152dd7'),
        (128, 2742, 2762, 5715, 'c4c7ee574ca52b163aad12e2daf68e3c3ac5eda432530c6f49e003b5b3c0e00d'),
        (256, 8477, 8497, 17281, '6f67b318890dc22935ceeff8e7a3637f81a8f44fdd46e012a3dcf73d61ed56df'),
    ),
    '14c_2': (
        (16, 12, 32, 196, 'eda0b81052ddd92697695a236ee6b635d100ec672fc58a49ce155a0ef6224292'),
        (32, 228, 248, 658, 'ed39b16d1d1d6546641e7c7d62d992ddb028ee4e16adf6b2e787ff6d09e4d0bc'),
        (64, 906, 926, 2391, '15c961b47c75ee969fb13341531016b8558dfb14bd3365fa3a331cffc173fb88'),
        (128, 3317, 3337, 7881, '06d469ef94c17d62398457129be440142675ffa0c87731eca885b08a58268062'),
        (256, 11218, 11238, 24671, 'dad1cc91d33e4d549d9a2f449cee2cea9313046e93b9d8d1cb5f3b85038c2aa6'),
    ),
    '16c_1': (
        (16, 12, 32, 270, '30048efde56a2c4442aaa2b1d0f94876ece01ef3260f950404b4b7ec36deb4cb'),
        (32, 302, 322, 822, '992816b7910e93fa698c3a80b6446ee84cc577347fd16d32c1578ae6c27d6359'),
        (64, 1144, 1164, 2755, '2e9daa117ea480a798480568a43e50ecdf1f64ad729b9a5b83c1b3cd660cf40e'),
        (128, 3919, 3939, 9492, 'ddbebb4e45037e33e20ff57f389aa6c8d8bf33a0e5a96d5345797cf5c669fdcc'),
        (256, 13431, 13451, 31004, '054fc1a47d9c8215a3ec25628db52b100c5dbba0aa6ca9368235fd308a3ed977'),
    ),
    '16c_2': (
        (16, 12, 32, 205, '93278bc1bd915be95bad42bb77dd3eab64d64b3404723098e09f2dbb80ce0725'),
        (32, 237, 257, 706, '7a1bc5d3b7648e1c930f7159e67c9b005229f506160feb9a6ea52f3f799a6566'),
        (64, 963, 983, 2299, '48757ff14c6a431a17d07262649a6444c7cee69361a6048aecc7ee1cfda26687'),
        (128, 3282, 3302, 7442, '4070ef250f23639f7ce277c3cea691891ce59f0796edea42b01397ed9093359d'),
        (256, 10744, 10764, 24549, '778b28d4c186d3a29e33c0c69087f701f503a53153a1fb9f43f1408546ad57c3'),
    ),
    '17c_1': (
        (16, 12, 32, 204, 'e030c6da277942b652b941669820bd69d4313e82f6418cb47421f5b1a6e04ab6'),
        (32, 236, 256, 620, 'd6fba88d47c7e6049a4146cb48dce6c6c6e601b430a5aebc6933f4a1d1e18f04'),
        (64, 876, 896, 2260, '816d3a109ea477b08afd2d02020637814b32e5d47f6b9ce90adcfd073093ba82'),
        (128, 3156, 3176, 8245, '4d4bcd5e62bd3bf9cad4f70df584560a84940ea336c518bba92a516315b1cfd7'),
        (256, 11421, 11441, 27157, 'c42ddf75497bf24b347d21d75fe563ab57d8a6234001c645c1bc04457ab37104'),
    ),
    '17c_2': (
        (16, 12, 32, 211, '4bbe8d9e3ffc9dbe1d748210f849d16e5e3eec714e7ecac08c38c91909620b4b'),
        (32, 243, 263, 671, '19828db9bba4eb9a628ce82044627172f03841462a097579f9dd08f83c098b19'),
        (64, 934, 954, 2204, '3cc3e1f82601deda14eec10a4279d54cecb2ab4983b4e1155fb8cd86af3c1f70'),
        (128, 3158, 3178, 8018, 'c280ee077086ef11ef9b0fa463bc82fc6dca312b74691d92c315849b280173aa'),
        (256, 11196, 11216, 27545, '350c5fab2ccaf157c645043daddca718c74cde3cd525b65d5a16bcf2511be5b8'),
    ),
    '18c_1': (
        (16, 12, 32, 200, 'a5a8ac1f00147310042e3ee815838f641da6c2ea3e7d0e8ddb1ac2ef911b74af'),
        (32, 232, 252, 687, 'b7a08c353ebe2f16d06b449544d81b5104b42b085c53bf4178928b958adc6e9e'),
        (64, 939, 959, 2320, '6fd5eec31e2baf21e3aba880a0defc634fc7c8c068938a2445ca933703f2f131'),
        (128, 3279, 3299, 7444, 'aa4edbbc0c06da26b2f71282db6b9716cc0fef396d6c1d6a0a07663198600742'),
        (256, 10743, 10763, 23263, 'ce58fa0ddb9de24bcbe13cc2c8e469a6b6480e57c634156cf4b354211e77a633'),
    ),
    '18c_2': (
        (16, 12, 32, 190, '13184f5ece9ca405aef52fbecdc01aaf4fc533b49e382fc991ced4829140a960'),
        (32, 222, 242, 648, '4cbfe2fd368c19c3833c065c9acb03c3be7a112473b15c8e55acf5394e857ee4'),
        (64, 890, 910, 2348, '3bbf81ef9c95928b15b133c22708a742e99354cbd65f332d52651ea9f4761e75'),
        (128, 3258, 3278, 7753, 'c772f2f0bcaea9b90ea640fb9f8b2a7c61beb459d687240c250796fe0771a392'),
        (256, 11031, 11051, 24340, 'b833bb375f0784ccb6534178a65f835008dae6305e00ee21f881fed2e7b67514'),
    ),
    '22c_1': (
        (16, 12, 32, 230, '973b1871408390e8ead24725eec93e06257f01133e9f7356019446c0fc681dcd'),
        (32, 262, 282, 744, '9c7c9fe81964c90b960f8fb2c4424be46f6ebf4fbdfc9025cdc08e88e2160659'),
        (64, 1026, 1046, 2621, 'e5a76b5e38c708e54e22ad9e170c57c3807210f2a65157277626963c8b5234e2'),
        (128, 3667, 3687, 8948, 'dfa372c18dd40cc84fd140da32e61c5369649a94c4fcbb5b0f04337ac0bfc884'),
        (256, 12635, 12655, 28396, '40d172d86bfd525608fff5f9370470271e8b98950422f7051757a2b5943d29b5'),
    ),
    '25c_1': (
        (16, 12, 32, 230, 'd32f8462e7b00c87c26ed6e253c1ceab3e832096977636babfa58eb913f1e5ea'),
        (32, 262, 282, 722, 'c117673c8879744e00b5607ef077515bc5e5daeeb146187f24c6133c3d4a74f3'),
        (64, 1004, 1024, 2319, '1ae7885332bd3ed01be915ab92218627aa8718d246c622709470fb52c3fe3092'),
        (128, 3343, 3363, 7309, '7e0e94224db5913ae91257416d1e9bb859c00f047168f1d78cbf092c51b38974'),
        (256, 10672, 10692, 22320, '20c3ad57eb94661a92f8b58837ebf009b2fd02bf6a443ed3051e4c78f4a2b67a'),
    ),
    '25c_2': (
        (16, 12, 32, 265, 'ebc242e34a3ea16a1b1e9ac8cef22be6e0224b84737f007d5402670eae83f5ee'),
        (32, 297, 317, 832, 'eeff56fd9956e0950aef8f3b01a8b39efa5626b12d3b48cd2764791ffd416552'),
        (64, 1149, 1169, 2618, '87608430c9a4a7310038e6333e6dcf2ef1ec980a6aa10b15906b156c585fdaed'),
        (128, 3787, 3807, 8208, 'f1be06c7402c0916693af4427e31a9b7861088b275ea145b991a3b1bb63d0a8c'),
        (256, 12015, 12035, 24222, '11e5cb781cf7a191568e751be4f7b4cb9e03066237836abe11f0cdf9ae1a681c'),
    ),
    '25c_3': (
        (16, 12, 32, 253, 'cc5a76095859cb3466646abefcc318c8480d0a1d880604f1fd91548c720eacef'),
        (32, 285, 305, 654, '64665b61275091715325de0a3d4646ea81cd320efc0eccdc3a6026e0e82883c8'),
        (64, 959, 979, 1805, '3819b3d60ec85d7dbecb5449e5dda5cdfcc6ebed9f7091782de36d7958b0602a'),
        (128, 2784, 2804, 5104, 'abf24e948f0a087a5348b3f1623501cdf7804a1b6545f09c53b21ea0aaa90e75'),
    ),
    '25c_4': (
        (16, 12, 32, 188, '518eee389f6cefd2bc0d2f85a75dcf87393c0a0fbb7d7418e7218fd6283ac215'),
        (32, 220, 240, 515, 'eeba8ff5fd852911542c0ff4791f7b8af1a090adbeee5e5de01aa32a03937cc3'),
        (64, 755, 775, 1369, '7ea87df0c122cb6754256b74e5cf254ac7cd53211093e233e654d3eecf5dad65'),
        (128, 2144, 2164, 3661, '2a9039348e50d93e3a985bb9356af544cbef8bef2f6c20b63e60205c6374c325'),
    ),
    '25c_5': (
        (16, 12, 32, 146, '6b0ace3a5a9c16aa52eb289333e03cacb6fb0beb22812e03e3d822d5c721b72c'),
        (32, 178, 198, 384, 'cc2696254c2c9914149a08e19f1159a351a4a061c4d2c98fe1923d325803623a'),
        (64, 582, 602, 970, '25078b81b1c9d5ccab0737d0d927cf68fe0cab4fedff584f6209f11a05db593c'),
    ),
    '25c_7': (
        (16, 12, 32, 187, '7703f5f9ac39c75bd3ea5688cdfc8eabf54f6350c33748cf396e816502d0df65'),
        (32, 219, 239, 648, 'ae1f69d452e98d20666c63ec4893cbba35385ce3c53a1c54080ba5f92ccfe0fd'),
        (64, 887, 907, 1684, 'ec3de160ca4e16fd5f42c9eb7484d170ae0e160e6823590d6f1e61460d45d0bd'),
    ),
}

ARCHIVE_BYTES = 5552683
ARCHIVE_SHA = '112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856'
HELPER_SHA = 'e482393f6b384863d17c7cc84aa930affd466eb6c6718dfd1292713408bf9efe'
COMMAND = ('-hide_banner -loglevel error -nostdin -cpuflags 0 -flags +bitexact '
           '-idct simple -threads 1 -err_detect explode -f ipu -i pipe:0 '
           '-frames:v 1 -pix_fmt yuv420p -f rawvideo pipe:1')


def need(ok, why):
    if not ok:
        raise ValueError(why)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def read_exact(path, count, digest):
    need(path.is_file() and path.stat().st_size == count, 'Input extent: ' + str(path))
    data = path.read_bytes()
    need(sha(data) == digest, 'Input identity: ' + str(path))
    return data


def write_new(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open('xb') as output:
        output.write(data)
    need(path.read_bytes() == data, 'Output readback: ' + str(path))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, required=True)
    parser.add_argument('--source-dir', type=Path, required=True,
                        help='Directory containing exact original flat-named PST files')
    parser.add_argument('--rider-archive', type=Path, required=True)
    parser.add_argument('--elf', type=Path, required=True)
    parser.add_argument('--output-root', type=Path, required=True)
    parser.add_argument('--source', action='append', choices=[s[0] for s in SOURCES],
                        help='Omit for original eight inputs. 07c_2 regenerates the existing Gasgano batch. Either 03c_1 or 03c_2 explicitly regenerates the complete ten-entry Clegg-additive batch; prior record provenance must be retained separately during integration.')
    parser.add_argument('--complete-preview', action='store_true',
                        help='Explicit 29-source raw preparation; preserve historical records separately when merging. Incompatible with --source.')
    parser.add_argument('--ffmpeg', default='ffmpeg')
    parser.add_argument('--execute-cold-conversion', action='store_true')
    args = parser.parse_args()
    need(args.execute_cold_conversion, 'Explicit cold-conversion authorization required')
    need(not args.output_root.exists(), 'Choose a fresh output root')
    need(not (args.complete_preview and args.source is not None),
         '--complete-preview cannot be combined with --source')
    gasgano_additive = args.source is not None and '07c_2' in args.source
    clegg_additive = args.source is not None and any(name in CLEGG_MIPS for name in args.source)
    additive = gasgano_additive or clegg_additive
    selected = DEFAULT_SOURCES if args.source is None else [s for s in SOURCES if s[0] in args.source]
    if clegg_additive:
        selected = SOURCES
    elif gasgano_additive:
        selected = GASGANO_SOURCES
    if args.complete_preview:
        selected = COMPLETE_PREVIEW_SOURCES
    # Authenticate every required input before importing the helper or invoking
    # FFmpeg. Missing baseline inputs cannot produce a partial additive catalog.
    read_exact(args.rider_archive, ARCHIVE_BYTES, ARCHIVE_SHA)
    inputs = [(name, count, digest, read_exact(args.source_dir / (name + '.pst'), count, digest))
              for name, count, digest in selected]
    helper_path = args.repo / 'tools/assets/decode_source_textures50fc.py'
    need(helper_path.is_file() and helper_path.stat().st_size <= 65536, 'Bounded helper')
    need(sha(helper_path.read_bytes()) == HELPER_SHA, 'Retained50FC helper identity')
    spec = importlib.util.spec_from_file_location('unlocked_original_50fc', helper_path)
    need(spec is not None and spec.loader is not None, 'Retained converter module')
    helper = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(helper)
    anchors = helper.verify_elf(args.elf)
    ffmpeg = shutil.which(args.ffmpeg)
    need(ffmpeg is not None, 'Installed native FFmpeg required')
    ffmpeg = Path(ffmpeg).resolve()
    need(ffmpeg.is_file() and ffmpeg.stat().st_size <= 128 * 1024 * 1024, 'Bounded FFmpeg')
    ffmpeg_sha = sha(ffmpeg.read_bytes())
    version = subprocess.run([str(ffmpeg), '-version'], capture_output=True, text=True,
                             check=True, timeout=15)
    need(version.stdout.startswith('ffmpeg version ') and not version.stderr, 'FFmpeg provenance')
    generator_sha = sha(Path(__file__).read_bytes())
    codec = [HELPER_SHA, generator_sha, version.stdout.splitlines()[0], ffmpeg_sha, COMMAND]
    outputs, sources, skipped = [], [], []
    for name, count, digest, source in inputs:
        frames = helper.parse_resource(name + '.pst', source)
        need(len(frames) == 1 and frames[0]['start'] == 0 and frames[0]['end'] == count,
             'Whole standalone PST')
        frame = frames[0]
        if args.complete_preview:
            need(frame['kind'] == 2, 'Complete-preview source must be original kind2: ' + name)
        if additive:
            expected_kind = 1 if name == '12c_4' else 2
            need(frame['kind'] == expected_kind,
                 'Additive source kind differs; refusing to drop an existing catalog entry: ' + name)
        exact_mips = GASGANO_MIPS if name == '07c_2' else CLEGG_MIPS.get(name)
        if args.complete_preview:
            exact_mips = COMPLETE_PREVIEW_MIPS[name]
        if exact_mips is not None:
            need(frame['kind'] == 2 and frame['palette'] is None and
                 len(frame['mips']) == len(exact_mips), 'Exact original kind2 mip count: ' + name)
            for actual, expected in zip(frame['mips'], exact_mips):
                dim, header, payload, size, source_sha = expected
                need((actual['width'], actual['height'], actual['header_offset'],
                      actual['data_offset'], len(actual['data']), sha(actual['data'])) ==
                     (dim, dim, header, payload, size, source_sha),
                     'Original IPUM payload/layout differs: ' + name)
        if frame['kind'] != 2:
            skipped.append(dict(name=name, kind=frame['kind'], source_sha256=digest))
            continue
        need(frame['palette'] is None and 1 <= len(frame['mips']) <= 5, 'Bounded original IPUM mips')
        records = []
        for level, mip in enumerate(frame['mips']):
            w, h = mip['width'], mip['height']
            need(0 < w <= 1024 and 0 < h <= 1024 and w % 2 == 0 and h % 2 == 0,
                 'Bounded YUV420P source dimensions')
            rgba, yuv_sha = helper.decode_ipu(mip['data'], w, h, str(ffmpeg))
            need(len(rgba) == w * h * 4, 'RGBA extent')
            need(all((rgba[i] | rgba[i+1] | rgba[i+2]) & 7 == 0 and
                     rgba[i+3] == (255 if rgba[i] or rgba[i+1] or rgba[i+2] else 0)
                     for i in range(0, len(rgba), 4)), 'Original RGB16/TEXA policy')
            image = struct.pack('<8sII', b'RRTEX001', w, h) + rgba
            filename = f'{name}-mip{level}.rrtex'
            record = dict(width=w, height=h, header=mip['header_offset'],
                          payload=mip['data_offset'], bytes=len(mip['data']),
                          source_sha256=sha(mip['data']), filename=filename,
                          rrtex_sha256=sha(image), rgba_sha256=sha(rgba), yuv420p_sha256=yuv_sha)
            records.append(record)
            outputs.append((filename, image))
        sources.append(dict(member='ui\\textures\\' + name + '.pst', source_bytes=count,
                            source_sha256=digest, mips=records))
    need(sources, 'No original kind2 sources selected')
    need(len(sources) <= (COMPLETE_PREVIEW_CAPACITY if args.complete_preview else 10),
         'Native additional cache source capacity')
    if args.complete_preview:
        need([s['member'] for s in sources] == ['ui\\textures\\' + s[0] + '.pst' for s in COMPLETE_PREVIEW_SOURCES]
             and not skipped, 'Complete-preview exact source order/membership')
        decoded_bytes = 611344 + sum(sum(m['width'] * m['height'] * 4 for m in s['mips']) +
                                    s['mips'][-1]['width'] * s['mips'][-1]['height'] * 4 + 16 for s in sources)
        need(decoded_bytes <= 16 * 1024 * 1024, 'Complete-preview retained cache byte limit')
    if additive:
        expected_names = (*BASELINE_KIND2_NAMES, '07c_2')
        if clegg_additive:
            expected_names += ('03c_1', '03c_2')
        expected_members = ['ui\\textures\\' + name + '.pst'
                            for name in expected_names]
        need([source['member'] for source in sources] == expected_members,
             'Additive catalog must retain the complete required source order')
        need([source['name'] for source in skipped] == ['12c_4'],
             'Unexpected indexed source in additive batch')
    need(sha(ffmpeg.read_bytes()) == ffmpeg_sha and sha(helper_path.read_bytes()) == HELPER_SHA and
         sha(Path(__file__).read_bytes()) == generator_sha, 'Converter changed during preparation')
    q = lambda value: json.dumps(value, ensure_ascii=True)
    lines = ['// Actual authenticated native-host conversion; no hardware parity claim.',
             'constexpr bool additionalCatalogAvailable = true;',
             f'constexpr std::size_t additionalSourceCount = {len(sources)};',
             'constexpr AdditionalSourceRecord additionalSources[additionalSourceCount] = {']
    for source in sources:
        lines.append('    {' + q(source['member']) + ',' + str(source['source_bytes']) + 'u,' +
                     q(source['source_sha256']) + ',' + str(len(source['mips'])) + 'u,{')
        for mip in source['mips']:
            lines.append('        {' + ','.join(str(mip[k]) + 'u' for k in
                         ('width', 'height', 'header', 'payload', 'bytes')) + ',' +
                         q(mip['source_sha256']) + ',' + q(mip['filename']) + '},')
        lines.append('    },{')
        for mip in source['mips']:
            lines.append('        {' + ','.join(q(mip[k]) for k in
                         ('rrtex_sha256', 'rgba_sha256', 'yuv420p_sha256')) + '},')
        lines.append('    },{' + ','.join(q(v) for v in codec) + '}},')
    lines.extend(['};', ''])
    generated = '\n'.join(lines).encode('ascii')
    manifest = dict(schema='RR_UNLOCKED_RIDER_IPUM_50KG_1', source_archive_sha256=ARCHIVE_SHA,
                    elf_sha256=helper.ELF_SHA, elf_verified_anchors=anchors, codec=codec,
                    hardware_pixel_parity_verified=False, sources=sources, indexed_sources=skipped,
                    generated_catalog_sha256=sha(generated))
    if additive:
        manifest.update(catalog_policy='clegg_additive_regeneration' if clegg_additive else 'gasgano_additive_regeneration',
                        requested_sources=list(args.source),
                        effective_sources=[name for name, count, digest in selected],
                        required_baseline_members=expected_members[:-2] if clegg_additive else expected_members[:-1])
    if args.complete_preview:
        manifest.update(catalog_policy='complete_preview_raw_regeneration',
                        effective_sources=[s[0] for s in COMPLETE_PREVIEW_SOURCES],
                        retained_cache_bytes_including_base19=decoded_bytes,
                        integration_policy='Append only new records; preserve historical records, pixels and per-source provenance verbatim.')
    for filename, image in outputs:
        write_new(args.output_root / 'evidence/ui-rider-ipum50kg' / filename, image)
    write_new(args.output_root / 'evidence/ui-rider-ipum50kg/unlocked-manifest.json',
              (json.dumps(manifest, indent=2) + '\n').encode('utf-8'))
    write_new(args.output_root / 'src/assets/original_ui_ipum_texture_cache50kg_additional_generated.inc', generated)
    print(json.dumps(dict(status='converted_original_unlocked_rider_ipum', sources=len(sources),
                          mips=len(outputs), hardware_pixel_parity_verified=False)))


if __name__ == '__main__':
    main()
