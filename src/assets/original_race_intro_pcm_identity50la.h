#pragma once
#include <array>
#include <cstdint>
#include <string_view>
namespace rr::assets {
// Exact original-disc race-intro VAG identities. No PCM pack, path substitution,
// caller-issued readiness or generated sound data. End-career tracks excluded.
struct OriginalRaceIntroPcmIdentity50LA {std::string_view filename;std::uint64_t bytes;std::string_view sha256;std::uint32_t sampleRate;};
inline constexpr std::array<OriginalRaceIntroPcmIdentity50LA,28> originalRaceIntroPcmCatalog50LA{{
    {"speech\\Dax\\Tracks\\Gamorr\\daxgmb11.vag",186080,"d03771a10d3979d6b7a1c8ddbdf5bd3359b470a93079091702d3325a1c702840",22050},
    {"speech\\Dax\\Tracks\\Gamorr\\daxgmb12.vag",114944,"95d363507743dbfedf2e1d4d7d7f0eeedd57006077cbe215ae4cecc74fb65b3e",22050},
    {"speech\\Dax\\Tracks\\Ryloth\\daxryc41.vag",88128,"56d3e11c8939999a0bd461e80fa3c5309550c451af3fec4f5e599b221f367478",22050},
    {"speech\\Dax\\Tracks\\Ryloth\\daxryc42.vag",228896,"031e589f2f4856c8db52c094bff75ea8ff637bd897e76aaa731e257265593fb1",22050},
    {"speech\\Dax\\Tracks\\Ryloth\\daxryc43.vag",145568,"5ae63346f868c9c7ee0c21de2eb74caff05bcff93518d0d9ab246771805f7560",22050},
    {"speech\\Dax\\Tracks\\Tatooine\\daxtab06.vag",193632,"a247a7bbf6b24f5b7d0333518f3d71d7801173c566160253264a37f0f904cb6e",22050},
    {"speech\\Dax\\Tracks\\Tatooine\\daxtab07.vag",85616,"392674414b80d9620664017c51ddcd834bb5dadbcbd43c1001b458ccc716be46",22050},
    {"speech\\Dax\\Tracks\\Tatooine\\daxtab08.vag",76736,"13ef9f5ede455b927f3983a5464c42239ee2200d8975dc7da27ed9f3b867e05d",22050},
    {"speech\\Dax\\Tracks\\Sullust\\daxstc34.vag",136064,"f597788ae2c0bd2d1b240a62918d17c6c86199d4fdfdd5bf17b80acafe86cd58",22050},
    {"speech\\Dax\\Tracks\\Sullust\\daxstc35.vag",73680,"01490966f669e4f671587fea87aea3967986397aba033a3dbd2eb9edad0c553f",22050},
    {"speech\\Dax\\Tracks\\Sullust\\daxstc36.vag",28368,"8403f40e0cfef678e4c98f6987fd092b610c2c801695d45581b376c991a301cc",22050},
    {"speech\\Dax\\Tracks\\MonCal\\daxmcb19.vag",143680,"03efcacaf0849c34d9fc91fca1d3b957df109efce6959875747bfbd2beea10b1",22050},
    {"speech\\Dax\\Tracks\\MonCal\\daxmcb20.vag",189792,"aad15f5c2ad921ebff0e233f10d1d173bd225960c3d69febbfff6daa3701d889",22050},
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
