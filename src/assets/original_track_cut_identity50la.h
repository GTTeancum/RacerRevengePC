#pragma once
// Bounded TA/RA/GC/SA/MA/GA/MB/SC identity/layout catalog from the supplied original RES members.
// Other track families remain outside this integration boundary.
// Digests identify bytes; no curves, PCM, or runtime assets are embedded.
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
namespace rr::assets {
struct OriginalTrackMemberIdentity50LA {const char* name=nullptr;std::size_t size=0,ordinal=0;const char* sha256=nullptr;};
struct OriginalTrackIntroEventIdentity50LA {
 std::size_t filenameOffset=0,filenameBytes=0,cameraOffset=0,cameraBytes=0;
 std::size_t streamOffset=0,stereoOffset=0,loopOffset=0;
};
struct OriginalTrackCutIdentity50LA {
 const char* trackId;const char* archiveName;const char* archiveSha256;
 std::size_t archiveBytes,memberCount;unsigned cutCount;std::size_t cutCountOffset;
 std::array<OriginalTrackMemberIdentity50LA,8> members;
 std::array<std::uint32_t,7> frames,zones;
 std::array<std::size_t,7> zoneOffsets;
 unsigned eventCount;
 std::array<OriginalTrackMemberIdentity50LA,2> introMembers;
 std::array<OriginalTrackIntroEventIdentity50LA,6> events;
};
inline constexpr std::array<OriginalTrackCutIdentity50LA,8> originalTrackCutIdentities50LA{{
 {"TA","TRACKS/TA.RES","b491e34bf6cbd27869d5b9dc8db394b101ebaf4ac7f5e0d44fcca33a00d41ab8",3334562,383,3,3256,
  {{{"tracks\\ta.tob",13887,0,"11e4c4909bf866fee626977c970d9907d4f191c29eeff32bddd162f173916f6b"},{"tracks\\ta01.vub",53488,376,"ad0a75b4e37e7e21f3fa29162c00dc4bc482b2f2d04066e5255edbd49a41147c"},{"tracks\\ta02.vub",17528,377,"ba15e426727250fdf7d25a0c7378d5323624a7504eb79cd9792eadd85f079e05"},{"tracks\\ta03.vub",17528,378,"fe7f6b12b3ddcb695f39b47d0a4e9cb89847e9639461d0b8ca03d4c05ed78456"}}},
  {{461,151,151}},{{9,5,6}},{{3109,3158,3207}},
  3,{{{"audio\\introAud.ini",346,381,"22e1afe07f5f34033b63a75fa8b5c2a30f49869f57cfa96926ba5c51eba9bf50"},{"audio\\scrptLns.ini",119,380,"1a325c68bbccaa8dfa94f87a8d1dcb743d029c59d96771797ce7045135cf6a0f"}}},
  {{{57,39,56,1,109,123,135},{161,39,85,1,213,227,239},{265,39,114,1,317,331,343}}}},
 {"RA","TRACKS/RA.RES","1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2",4604561,1010,7,15496,
  {{{"tracks\\ra.tob",95558,0,"7336c6bab40265adb5e82aec8ed3ee66669c19b26a597cfacedddfca4be48524"},{"tracks\\ra01.vub",21008,999,"f955e6576f7262e85e60f52cb04571b1ff26c8298595cf78841f88bb28276491"},{"tracks\\ra02.vub",7088,1000,"040d0676981edeaefb4276586d52cf08004ea92284242e311ab1e2c1e48064a6"},{"tracks\\ra03.vub",7668,1001,"b093cbfc91667a64eaf97f96955782266128c1404ad1f5b074f1f95f0fbe5253"},{"tracks\\ra04.vub",18456,1002,"12e2dbdc837c5875c2067e07e99cd92184a35380e9d2f6afb80fcc097ef06840"},{"tracks\\ra05.vub",47572,1003,"8f4a89493b7edc2a42aeacc346956110b3874750bea3e5728868e34b90858112"},{"tracks\\ra06.vub",15788,1004,"3c92f104f0723ae18dd01e9f34d044b02259dea5290472fb2aac5569aa00d5df"},{"tracks\\ra07.vub",23792,1005,"4871a0b8355e983f0fa2beaac4aa7ae44c5650cd27fb8750df1eff96ab2e491e"}}},
  {{181,61,66,159,410,136,205}},{{10,7,1,6,3,7,6}},{{15152,15202,15251,15300,15349,15398,15447}},
  2,{{{"audio\\introAud.ini",238,1008,"72aa1f6ad04da68e6c7f345a05471e0d7b6cfcc8166dc18e9ed2fe8de6df76ce"},{"audio\\scrptLns.ini",98,1007,"f7234be2254765e73546711e11d44c7b02828b17c023ef0aeacb75b9cd316bf7"}}},
  {{{57,37,56,7,107,121,133},{159,37,91,5,209,223,235}}}},
 {"GC","TRACKS/GC.RES","69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9",4205067,1190,4,2511,
  {{{"tracks\\gc.tob",9623,0,"e677f80d12da2c2225c8ba030e2a29e3a358c52bcfd4f9c981b79c3c53b72454"},{"tracks\\gc01.vub",34928,1180,"2e3ffe0515e0a8b2eaf8e81d7c8594c7886103abba2b7342716574ebd72291ac"},{"tracks\\gc02.vub",17528,1181,"ac689f7c2826e9f4554d0e68c7d34451411d599d2ba8912ab89e4eef82344d4c"},{"tracks\\gc03.vub",34928,1182,"90d9b61ec1ecfd7f2ce3d9bd805d004dfa9fde68c14a40cddfb4c91fd12f69cc"},{"tracks\\gc04.vub",14628,1183,"729bd135802a8400748759c131c05bd7714fe82c4b0229cfe64901250528b5a3"}}},
  {{301,151,301,126}},{{8,3,6,6}},{{2315,2364,2413,2462}},
  3,{{{"audio\\introAud.ini",340,1186,"9270868317ef863dc0d5044346e712049b0edf4688e634ddb46b349b04c54826"},{"audio\\scrptLns.ini",117,1185,"a59dc183ec92b00a2cd0e7940f66552207074b10ceba9ffb61461f7eeed529c8"}}},
  {{{57,37,56,1,107,121,133},{159,37,85,1,209,223,235},{261,37,114,1,311,325,337}}}},
 {"SA","TRACKS/SA.RES","34826bbb3a3690ee20ac40f70515f7e903291cdf4546f7d8ab8d58849f30d96a",3786696,1065,5,7803,
  {{{"tracks\\sa.tob",13314,0,"b2b53df201ddfc5bd0283cd50911c6e94937f3633bba33b864da13557f415542"},{"tracks\\sa01.vub",24488,1056,"ccf6f354e449a41f3713fd7e8562610609a602cb30dfda028efce2f63fa4968b"},{"tracks\\sa02.vub",38408,1057,"e9652fc98786de69b3ada13c27073a01707dd206cfa53652eaa4d63a22bab168"},{"tracks\\sa03.vub",14048,1058,"bbe649e60bb5872b7cc10a953dbf4a082331da1ac083aaf24816faa9c8384b46"},{"tracks\\sa04.vub",17528,1059,"3fffed1badb89564814bdd23e19a0fbaa14d28e31b743b83b16f2d5da7c4d1d6"},{"tracks\\sa05.vub",24488,1060,"00004155f36a7759cb6be875c3097bead6b14dd1db9ee7554f7716b1ba3ce1c3"}}},
  {{211,331,121,151,211}},{{23,1,4,12,18}},{{7555,7605,7654,7703,7753}},
  2,{{{"audio\\introAud.ini",240,1063,"9f9d20f76a7b729a0cc471a2b8e4cd4c269a75803be1256be127644b471df15a"},{"audio\\scrptLns.ini",88,1062,"d6aad56024e0ac3bfaf0bc3c138ab089cc0953b52bf395b78c58d7b70984b1cb"}}},
  {{{57,38,56,1,108,122,134},{160,38,85,1,211,225,237}}}},
 {"MA","TRACKS/MA.RES","2a674f62e9d9d9ef9e6c2f7224d2aa0c55aeb883e55c499d58dfd3b00cddb228",3691592,485,4,4269,
  {{{"tracks\\ma.tob",32691,0,"4a3bcecce43a53b3ec3ac28df9c9672a61e825a2987c11bbd0ab055b29f7da55"},{"tracks\\ma01.vub",50008,477,"981f4bf465827618a72289b57067e9e7fa95c1f0173a1264c83c3302e8a11c3f"},{"tracks\\ma02.vub",21588,478,"447d169dac9c8e2429d855015327e92a041012ad778691283a9ff696f31cc7b0"},{"tracks\\ma03.vub",29128,479,"98fde622bc738c494a0b78a86a8e4449e86e111e89ba621dd62611cc0ca4cfc6"},{"tracks\\ma04.vub",9524,480,"e69aec57cc4ea49fba2f19a7b7e491123d58e6d0c4f1ee3078adebf9f02d52c0"}}},
  {{431,186,251,82}},{{6,3,5,1}},{{4073,4122,4171,4220}},
  3,{{{"audio\\introAud.ini",340,483,"0ff6ce708dad3507cfb5de7107e9f14d2fcc5cde1ef3a53b7ec1e84b3fbd302f"},{"audio\\scrptLns.ini",117,482,"bacbdb0d6ac4bc793f0d459f5a88e73fb09c469976757eb26c21610a059bbd86"}}},
  {{{57,37,56,1,107,121,133},{159,37,85,1,209,223,235},{261,37,114,1,311,325,337}}}},
 {"GA","TRACKS/GA.RES","063b65cc9a0410ba0393362309fb3f80c4516a3c65ffee2b9f7bc76d1afe25d9",2834942,970,5,2556,
  {{{"tracks\\ga.tob",9689,0,"e96450990e23c97f6d686707a183789b08d45857187d5c8463960cb323e75aa3"},{"tracks\\ga01.vub",34928,961,"45cb1e8fc4754750f09dce8e7e406386a7008e38ed18de2370b76d601e15b725"},{"tracks\\ga02.vub",29128,962,"445b54d8169038852ba30fabf808092e1f5569aac7f9dc40065f9b4700cecaeb"},{"tracks\\ga03.vub",11728,963,"2359da47fe8a3628b0f0ed8fe2885f111207c75f076514bf7c913e4c1516895a"},{"tracks\\ga04.vub",20428,964,"47159fea426fdb61dd352baf86279dd2ac476354f7e4edf36b0ae24cde928a77"},{"tracks\\ga05.vub",15208,965,"6e62d45f10e1c9d0686e2042b5938ba4c9695703ca83838a2c80d96fc475bc03"}}},
  {{301,251,101,176,131}},{{9,2,1,6,7}},{{2311,2360,2409,2458,2507}},
  2,{{{"audio\\introAud.ini",238,968,"3b99e6b992b8ebe102a4fd89841be92d80793d348aa683abb3fc3992eccf9785"},{"audio\\scrptLns.ini",88,967,"c28784d774a7a1d65fe118d3e94d5c3c7aa8bff490ee81bfaeeb0e7adfc58d90"}}},
  {{{57,37,56,1,107,121,133},{159,37,85,1,209,223,235}}}},
 {"MB","TRACKS/MB.RES","6aae7d59981cae6c741b14e147021960fdb4d8e68e21c217a446f779d3c821ca",4421779,487,5,3134,
  {{{"tracks\\mb.tob",19909,0,"f25f10fff204f124e5525ff2c4a7c1e72f9ce17476e1404d1932f6ab6e8c776d"},{"tracks\\mb01.vub",17528,478,"3f79c50a52e409fb1a669c7f2cdf837f5afb1f521d7514f10a454251d47f7278"},{"tracks\\mb02.vub",24488,479,"1921a20021dd82033464fb7a6aa817ba261a8646307f86c2fe0e09782e2c02b2"},{"tracks\\mb03.vub",17528,480,"ce2d3b99852bd5651b6f9e8e6d94a93819c4bbe7c8a6b9029c47e8a9519aa9c4"},{"tracks\\mb04.vub",24488,481,"3fe7059bf7683e839b73f63600b27071017c298a8a1aa824c97de5f7f3c9a7ae"},{"tracks\\mb05.vub",14048,482,"c32c23733639ec2cdeac3576d589c256e24028d0baee7b037e1c1aa700dab75f"}}},
  {{151,211,151,211,121}},{{1,9,8,9,9}},{{2889,2938,2987,3036,3085}},
  2,{{{"audio\\introAud.ini",238,485,"1ce76eba38a94afb4a9e0ecaf82b7416781fef75480683299f734747b46638e4"},{"audio\\scrptLns.ini",88,484,"c28784d774a7a1d65fe118d3e94d5c3c7aa8bff490ee81bfaeeb0e7adfc58d90"}}},
  {{{57,37,56,1,107,121,133},{159,37,85,1,209,223,235}}}},
 {"SC","TRACKS/SC.RES","8305641f0ac8fefeae6603145ad4f8441612e41926751ff853170763e66dfa4d",2974353,984,3,10175,
  {{{"tracks\\sc.tob",64047,0,"473351310a098008a477f9e72a6fc7e03d8bb808adcd79f5d810f90aa332b727"},{"tracks\\sc01.vub",40728,977,"113c6f1df2f8db3aa9fc2faaa2e04fff05d9fe58f7fd8dcccc6a949ad480abd0"},{"tracks\\sc02.vub",23328,978,"2279e7fafb56d67bcbc41c4cb4a02ef458f2166aa40396957b896c8c901272ca"},{"tracks\\sc03.vub",9408,979,"5ebb96eb7218d91bb38dfd0e06cb139e4bfb9cc31f2173faec2178089757281f"}}},
  {{351,201,81}},{{11,3,17}},{{10026,10076,10125}},
  3,{{{"audio\\introAud.ini",343,982,"a74d454fd671470aa9f4a507a026369973936999535f39606ce84595c3dc9d1f"},{"audio\\scrptLns.ini",117,981,"a59dc183ec92b00a2cd0e7940f66552207074b10ceba9ffb61461f7eeed529c8"}}},
  {{{57,38,56,1,108,122,134},{160,38,85,1,211,225,237},{263,38,114,1,314,328,340}}}},
}};
inline const OriginalTrackCutIdentity50LA* originalTrackCutIdentity50LA(std::string_view id)noexcept {
 for(const auto& row:originalTrackCutIdentities50LA)if(id==row.trackId)return &row;
 return nullptr;
}
}
