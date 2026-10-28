#pragma once
#include <array>
#include <cstdint>
#include <string_view>
namespace rr::assets {
// Exact original-disc race-intro VAG identities. No PCM pack, path substitution,
// caller-issued readiness or generated sound data. End-career tracks excluded.
struct OriginalRaceIntroPcmIdentity50LA {std::string_view filename;std::uint64_t bytes;std::string_view sha256;std::uint32_t sampleRate;};
inline constexpr std::array<OriginalRaceIntroPcmIdentity50LA,15> originalRaceIntroPcmCatalog50LA{{
    {"speech\\Dax\\Tracks\\Gamorr\\daxgma09.vag",212416,"470460b7657587482491263fc7688348484cbea7b29b99b20b6a5e722e8afabf",22050},
    {"speech\\Dax\\Tracks\\Gamorr\\daxgma10.vag",169184,"e37897ef497771db332dce08936ff0ea84d7dc61813c10c6bada7689ea733280",22050},
    {"speech\\Dax\\Tracks\\Sullust\\daxsta29.vag",78736,"f30599245bfbfae9489f0a9270c1fbcbd474e6b52634fcb562b24f683e5fc935",22050},
    {"speech\\Dax\\Tracks\\Sullust\\daxsta30.vag",347728,"7dac53795e213a6c7174eb1edc03b25c5f9cb0e3c51ced6f4683a3cbc7a48eee",22050},
    {"speech\\Dax\\Tracks\\MonCal\\daxmca16.vag",171776,"ab1e79261912fa1a6cff23a74a26d92858bdf4b46d3d0a9fc9f9bd2b805f9437",22050},
    {"speech\\Dax\\Tracks\\MonCal\\daxmca17.vag",178176,"359857006b7fb1682001221efc0b723391ac17c14d602036fd3f40589fcf3eb8",22050},
    {"speech\\Dax\\Tracks\\MonCal\\daxmca18.vag",27872,"a4befe6cd78310a8a845bdbb05bdaf65b1190ad4efb7f1d9f0f0483df357728f",22050},
    {"speech\\Dax\\Tracks\\Gamorr\\daxgmc13.vag",110048,"009e3be55e81bff6bd25be754d17ab8379e5c3583fff52988e2b987b4a8b07d6",22050},
    {"speech\\Dax\\Tracks\\Gamorr\\daxgmc14.vag",61664,"177b030a05641e3782fa8fa1a25369056248fef00b7c4b8f63e15a67a3d27694",22050},
    {"speech\\Dax\\Tracks\\Gamorr\\daxgmc15.vag",168400,"faffd957fdf0c24ea9f84d7aec8ae4accf45283fe3a0266c40e22971739fa9fe",22050},
    {"speech\\Dax\\Tracks\\Ryloth\\daxrya37.vag",183472,"0bf2151b189c085429c7b7113867ace94c300cff2aff6b152ee7ae75683fae5f",22050},
    {"speech\\Dax\\Tracks\\Ryloth\\daxrya38.vag",313936,"3a2f9d44f4e64af0d0049fa2dc6d92971267343c38ab9dc87395aca6f7f85204",22050},
    {"speech\\Dax\\Tracks\\Tatooine\\daxtaa01.vag",188208,"5796e1a4874411ceaf65d4a1a846ca05f1da881446e135722e5718a8387f079e",22050},
    {"speech\\Dax\\Tracks\\Tatooine\\daxtaa02.vag",62608,"6c7eb6c9e64aec42ee5a23c351e99515d2e8e9d6b97afad45aa2a3427035fde7",22050},
    {"speech\\Dax\\Tracks\\Tatooine\\daxtaa03.vag",62240,"2275e85c776d830edaaaea3a6bcd88c58e955c2c79c7d6fe4595c6438584cf5f",22050},
}};
inline const OriginalRaceIntroPcmIdentity50LA* originalRaceIntroPcmIdentity50LA(std::string_view authored)noexcept {
    for(const auto& row:originalRaceIntroPcmCatalog50LA)if(row.filename==authored)return &row;
    return nullptr;
}
}
