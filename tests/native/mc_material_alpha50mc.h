#pragma once
#include "material_alpha50gx_fixture.h"
namespace material_alpha50gx_fixture {
inline bool selectMc(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output) {
    using Field = rr::assets::OriginalUiGsField50KG;
    if (selection.track != track_source_fixture::Track::mc || selection.assetIdentity != 0x4d43 || selection.slot != 1 ||
        !selection.pass || selection.pass>selection.passCount || !selection.passCount || selection.passCount>2 ||
        !runtime.overrideKnown || runtime.override435670) return false;
    // These material keys belong to the authenticated MC world source closure.
    struct Identity {const char* name;const char* key;const char* program;const char* map;std::uint32_t pass,count;};
    static constexpr Identity identities[]={
        {"camdroid03","16963672e995753d1619b9b11f06cbe620ac30e77ffd71abd26244549b682bd6","PrelitCompressed","camdroid03",1,1},
        {"vent01","d0ffe12c1fdaa4a4ada24ec60b387c59d408c71b6f93fc30aa092795043e12eb","PrelitCompressed","Vent01",1,1},
        {"fc_lpost","095809a010e4801efa3532eca90739b963114097c2bb6583419819498ae8916d","PrelitCompressed","fc_lpost",1,1},
        {"forcefield_dome","cdb3417a9042ed1f12aa09b15aa1dcbf4160562ac3a7ee1573f79ef103e37e7a","DiffuseCompressed","ForceField_Dome",1,1},
        {"ter_fount-top","149dd36a2780851e11361817c1aee708211326ac8bcfec2976e83d142344689c","DiffuseCompressed","ter_fount-top",1,1},
        {"ter_fount-side","fe28b8dbe36de2662c4d9ac289b2e4cbc4afa6a4c51373b2db551ca7208da932","DiffuseCompressed","ter_fount-side",1,1},
        {"wall_scales-arch","d800ee8eca5139beb629000750f2b5eafbdbd4b0deb4ffe64b0fa6b3c85fa4bc","PrelitCompressed","Wall_Scales-Arch",1,1},
        {"bld_ledge-b","e6e1bf66a1499668cfa16e0eec012f056e4fd518d5768a20a3afb38eee6d8ec7","PrelitCompressed","Bld_Ledge-B",1,1},
        {"stands_seats","c27d3c27cfd2692d70e8f10628be66d90487855b11b56eedf1938898180b4f32","PrelitCompressed","Stands_Seats",1,1},
        {"wall_shell-plates","4a66d7ec73e04d48887ffcf01468392bcfba05103d28f786e912b4b91ace1bb1","PrelitCompressed","Wall_Shell-Plates",1,1},
        {"mc_coral10","9702c594d04b5aca7b494792913a274478e2dcf5156dea32a7c5a05feaf08efb","PrelitCompressed","mc_coral10",1,1},
        {"wall_vert-bump","832e0507af98c17165499894ddc0abba9afa37239d89023f55c48ba6df0e9bbb","PrelitCompressed","Wall_Vert-Bump",1,1},
        {"hoh_backside","6da45fee5d1f9c2889e039788a705116bc55243988e3310469e0f5ea0c145dd7","PrelitCompressed","hoh_backside",1,1},
        {"calciumroad","4dbc0f69304432d6563e8c2cf71d62728f1298bcad354de29f428d9a9ce3024b","PreLitCompressed","calciumroad",1,1},
        {"pipe_a-red","a9e1372c924353cbb4bfcb397554cb33c0cbd06f90f24ac656f0633104a1def5","PrelitCompressed","Pipe_A-Red",1,1},
        {"tongue_deco","887a2490b70db41a2d2d459db819fa3eeb0b053abd93d026d8ad2feeaba52549","PrelitCompressed","Tongue_Deco",1,1},
        {"mall_walls","af211648c7703bc7a5ba87e0a5efbbc2476a52101c9dab394e3f552bad796e6a","PrelitCompressed","mall_walls",1,1},
        {"hoh_exterior","b8908a6929f0343bc2b1dd4208c425581b3d8f09104e9d80e988f4059923084e","PrelitCompressed","hoh_exterior",1,1},
        {"ma_vent-cylinder","4ca77400affdd21407815ed68020181ba78547a2b9ceedccd8b32624795972a1","PrelitCompressed","MA_Vent-Cylinder",1,1},
        {"hoh_shells","29849832eddb9c4e6800e44cefc184c38a67110ebf96e3563a8e0eb7f3f393a9","PrelitCompressed","HOH_Shells",1,1},
        {"white","658a95f5ebc12e90b6a5a3aea131588e00c21fa8b767b511d850225275b9c948","PrelitCompressed","White",1,1},
        {"vent03","9716630b5771990d13e1a4e7f11f5d85d7a5bbfab6b308f86a9a10c8370ea594","PrelitCompressed","Vent03",1,1},
        {"stairs_bowl","1486f95dfaf39116a44309cb733050bb4da4d12e091c96b29240cf7d34e6d303","PrelitCompressed","stairs_bowl",1,1},
        {"gen_shell","8ec1304c625ee31f9722949b6ef2a632180f58b3310d23f76470f26fcae227c2","DiffuseCompressed","gen_shell",1,1},
        {"bridge_split-cap-trim","9c69884c0be395c0613efc6bb99089b5e4270a3bd6e52b78f4a2ab78e960d708","PrelitCompressed","Bridge_Split-Cap-Trim",1,1},
        {"hoh_road","efab3fa618d7f20476124e811b89d8ae65b7773a4863cb5d4b86dcc87b18e643","PrelitCompressed","hoh_road",1,1},
        {"ground_trim","df169eee8b25b94b14c0021d5816a3a5cea9af4b4470451581f12dd277b83e19","DiffuseCompressed","ground_trim",1,1},
        {"wall_terracewindows","9684646507a8c073ed34771684e6205ed689fc06cba734b37427a2742da108f7","PrelitCompressed","Wall_TerraceWindows",1,1},
        {"bld_win-c","3366f933e68aed4d40cd666d0be3974cefe17fe67a03027c4fae5da6a4bd3603","PrelitCompressed","Bld_Win-C",1,1},
        {"ma_crowd","ca1af44c7cd19a662211d4741edb08aa86226e3080be95be474cac1e512c8753","PrelitCompressed","MA_Crowd",1,1},
        {"camdroid01","594a3715fc69b3b4d0d15aca0a8af911fc888e7d175da85d1a79f1f8b897ea18","PrelitCompressed","camdroid01",1,1},
        {"camdroid02","12e987ff75f638b1bdc3ea05fad7a84432e6ba2e9db8a2044426a275c21a8a4a","PrelitCompressed","camdroid02",1,1},
        {"mc_tile","315b0a99a2cec74137aaeacd6e43f88bf7dee269c72c1058d2430b1da6f5039b","PreLitCompressed","mc_tile",1,1},
        {"mc_coral12","293b9ed948975ad0c740ebe9e17eed61fbc16c0087a5f5460c95e4b946e6f709","PreLitCompressed","mc_coral12",1,1},
        {"mc_coral11","e7d2738dd08cf220b50d5701aa59acfa7b310cc631ccf728c2f288fa28f752e2","DiffuseCompressed","mc_coral11",1,1},
        {"mc_shield","69463a1e42bf9ea4f0a2d8372891b1c2f853b06a65e3304982498cdd6748417e","diffuseTextransCompressed","mc_shield",1,1},
        {"mc_binder","533e6d2a993849999bcec3c01fcef70602382590c74b39555e2892521be3799c","DiffuseCompressed","mc_binder",1,1},
        {"arrowdroid03","777bf3fb8d960848c40a05417f672bd211a859900a7c13deb84f02bf7e93ea10","PreLitCompressed","arrowdroid03",1,1},
        {"arrowred","b7ca19102a16afe6ac29384d9b42380070526ee5b80a901a9acf06b15d2f6960","PrelitCompressed","arrowred",1,1},
        {"arrowdroid01","5c3e3b274e39735e703c9b200c4ab3933ad17d96ceec41e045d7382b99a6fe80","PreLitCompressed","arrowdroid01",1,1},
        {"cord","f412f61c202d72b1f0ba4ef66dc13d1be0bbbbafc9d297c65b9df1f4e2252270","PreLitCompressed","cord",1,1},
        {"mc_gel","8ac4cb0ef49fdf817ddd700f0438620afd084737fee2b601d206057f55ef9f35","PrelitCompressed","mc_gel",1,1},
        {"mc_coral2","516a03d7be615b5a1dea3e60053dbafa0dcc1afd0205ba29abecea034accea2d","PreLitCompressed","mc_coral2",1,1},
        {"mall_exterior","bf5178045f3e5dedad2ba4dd34d6ef41d3d743addf21e46322eae023bc5252e5","PrelitCompressed","mall_exterior",1,1},
        {"blg_surf-calcium","850976f0cb9a251365030597ac423c4e9a4dd867d9708801a4c1fb294c0e903e","PrelitCompressed","Blg_Surf-Calcium",1,1},
        {"bld_win-a","df770ce18cd3c2c98b78567801ef9c31a205c5ff3c41d65fcd778930047db2e8","PrelitCompressed","Bld_Win-A",1,1},
        {"wall_calcium-plates","d6b6a2456087893552d218715f7db60e3cc17a7349e806b3ffc0b9ee294fd326","PrelitCompressed","Wall_Calcium-Plates",1,1},
        {"hoh_exterior-spike","b7526f15efe7b5e5021191548ee55da8251424139966b1422baf23b45b27332a","PrelitCompressed","hoh_exterior-spike",1,1},
        {"bonerim_a","97a106510c3e73eb15f4565589bda49d90bc98eedd5583784457416edacfc861","PrelitCompressed","bonerim_a",1,1},
        {"road_top","033c2cdc61ebade3a39d184c707f348d799c1cfc61146430f1332dc21f42c9ed","PrelitCompressed","road_top",1,1},
        {"road_bot","a4c4365717b346d8ce6005edd07001fffffc4f46ba16687b68b486b026b5a54c","PrelitCompressed","road_bot",1,1},
        {"mcwall3","3724c040d9856d8740521d2b780936fe05cf307b4b94e6a80a8bde12741421de","PrelitCompressed","mcwall3",1,1},
        {"statue3","d9b639e0530ae004da75757c8596b3c27dbf9ad0c6c3f9e9c9a7d7d73b0bb385","PrelitCompressed","statue3",1,1},
        {"mc_coblend","c7a4cfd9f26f394c2eb9e13c8dfba05d59a490f73ce69e4aab833b9691c4ce95","PrelitCompressed","mc_coblend",1,1},
        {"bld_dome-copperplates","bb772149278c84720abdcab0759e709d99c3dfee7e50e94f9d14371da04f5873","PrelitCompressed","Bld_Dome-CopperPlates",1,1},
        {"wall_shell-scales","0a10dfb02ee397d8b5b68cb8714aaa29f5a0b08b7db016b43e396fc934e4847d","PrelitCompressed","Wall_Shell-Scales",1,1},
        {"mc_coralwall","63212b266e8fe2f850d0572f8f8b680979a045534cd2d93e4a30ac41846c7788","PrelitCompressed","mc_coralwall",1,1},
        {"mc_brick2","d60f09319c524055389a0a6679a101764176d9c7ab81e13ad2475ccef2c11817","PrelitCompressed","mc_brick2",1,1},
        {"terrace_ledge-tiles","637a698ac774fb721c776551416ae5bb2f0e4329a3b3580a0f9055cfa2ba5fcf","DiffuseCompressed","terrace_ledge-tiles",1,1},
        {"mc_brick4","81d6fb698502db8b91d30e1f56aa6372e1b7d957f7eb8a49c06524b3094f80ac","PrelitCompressed","mc_brick4",1,1},
    };
    bool identity=false;
    for(const auto& row:identities)if(selection.materialKey==row.key&&detail::lower(selection.materialName)==row.name&&
        selection.program==row.program&&selection.textureName==row.map&&selection.pass==row.pass&&selection.passCount==row.count){identity=true;break;}
    // Stock material identities: a receipt list, not an admission gate. A
    // material outside it (new or changed bytes) takes the same field rule below.
    (void)identity;
    const auto* inherited = templates.get(rr::assets::OriginalUiTemplate50KG::defaultMt);
    if (!inherited || !inherited->identity || !inherited->identity->sha256 ||
        std::string(inherited->identity->sha256) !=
            "1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7") return false;
    const auto& fields = selection.sourcePassFields;
    if (!fields.is_object() || !fields.contains("authored") || !fields.at("authored").is_array()) return false;
    auto sourceInteger = [&](const char* name, std::int32_t& result) {
        bool found = false;
        for (const auto& item : fields.at("authored")) {
            if (!item.is_object() || !item.contains("name") || !item.contains("value")) return false;
            if (!item.at("name").is_string()) return false;
            if (detail::lower(item.at("name").get<std::string>()) != detail::lower(name)) continue;
            if (found || !item.at("value").is_string()) return false;
            const auto text = item.at("value").get<std::string>();
            const auto parsed = std::from_chars(text.data(), text.data()+text.size(), result);
            if (parsed.ec != std::errc{} || parsed.ptr != text.data()+text.size()) return false;
            found = true;
        }
        return true;
    };
    std::array<std::int32_t, rr::assets::originalUiGsFieldCount50KG> gs{};
    for (std::size_t i = 0; i < gs.size(); ++i) {
        const auto field = static_cast<Field>(i);
        gs[i] = inherited->gs[field].value;
        if (field == Field::environmentContext0or1) gs[i] = 0; // 247C88 pass-local default.
        if (field == Field::inputColorD && !sourceInteger("inputAlphaD", gs[i])) return false;
        if (!sourceInteger(rr::assets::originalUiGsFieldName50KG(field), gs[i])) return false;
    }
    auto get = [&](Field field) { return gs[static_cast<std::size_t>(field)]; };
    auto flag = [&](Field field) { return get(field) == 0 || get(field) == 1; };
    if (!flag(Field::alphaBlendingOnOff) || !flag(Field::alphaTestOnOff) ||
        !flag(Field::zWriteDisabled) || !flag(Field::foggingOnOff) || get(Field::destinationAlphaTestOnOff) != 0 ||
        get(Field::depthTestMethod) != 2 || get(Field::textureColorComponent) != 1 ||
        get(Field::textureFunction) != 0 || get(Field::alphaCorrectionValue) != 0 ||
        get(Field::alphaBlendingControlInUnitsOfPixels) != 0) return false;
    const auto program = detail::lower(selection.program);
    if(selection.projectedPrelit){
        if(selection.program!="ProjectedPrelit"||selection.passCount!=2||get(Field::alphaTestOnOff)!=0||
            get(Field::alphaBlendingOnOff)!=(selection.pass==1?0:1))return false;
    }else if(selection.passCount!=1||selection.pass!=1||
        (program!="prelitcompressed"&&program!="diffusecompressed"&&!(detail::lower(selection.materialName)=="mc_shield"&&detail::lower(selection.textureName)=="mc_shield"&&program=="diffusetextranscompressed")))return false;
    Policy next;
    next.depthWrite = get(Field::zWriteDisabled) == 0;
    next.depthEqual = true; // Source GEQUAL becomes LEQUAL in the native depth map.
    next.sourcePrelit = selection.projectedPrelit || program == "prelitcompressed";
    next.unlit = selection.projectedPrelit; // Existing source-space ProjectedPrelit presentation path.
    next.sourceFogKnown = true;
    next.sourceFogEnabled = get(Field::foggingOnOff) != 0;
    for (const auto& item : fields.at("authored"))
        if (detail::lower(item.at("name").get<std::string>()) == "zwritedisabled") next.sourceDepthWriteSpecified = true;
    if (get(Field::alphaTestOnOff)) {
        if (get(Field::alphaTestMethod) != 6 || (get(Field::alphaReferenceValue) < 0 || get(Field::alphaReferenceValue) > 255) ||
            get(Field::processingMethodWhenFailedInAlphaTest) != 0) return false;
        next.sourceAlphaTest = true;
        next.sourceAlphaReference = static_cast<std::uint8_t>(get(Field::alphaReferenceValue));
    }
    if (get(Field::alphaBlendingOnOff)) {
        if (get(Field::inputColorA) != 0 || get(Field::inputAlphaC) != 0 ||
            get(Field::inputColorD) != 1 || (get(Field::inputColorB) != 1 && get(Field::inputColorB) != 2) ||
            selection.textureIndex == track_materials50fa_fixture::unresolved) return false;
        next.blend = get(Field::inputColorB) == 2 ? Blend::additive : Blend::sourceAlpha;
        // Existing declared native GS-unit modulation policy. This does not
        // assert PS2 fragment quantization, microprogram or framebuffer parity.
        next.alphaScale = (255.0f/128.0f)*(255.0f/128.0f);
    }
    output = next;
    return true;
}

}
