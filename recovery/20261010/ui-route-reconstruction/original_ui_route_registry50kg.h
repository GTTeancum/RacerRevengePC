#pragma once
#include <array>
#include <cstdint>

namespace rr::game {
// CLOSED metadata translation of original 0x1B7DF0 / table 0x3AC790.
// Addresses are provenance only. Never execute original code or use this table
// as an admission mask: asset loading, native page owners, and callbacks must
// independently be available before enabling an entry. No RR_RACE_MODE mapping.
struct OriginalUiScreenFactory50KG {
    std::uint8_t screen;
    const char* resourceToken; // Literal passed to the original loader, not a guessed PageFile.
    std::uint32_t branchPC;
    std::uint32_t constructorPC;
    std::uint32_t loaderPC;
    std::uint8_t galleryVariant; // Only gallery cases pass an extra 1/2/3 argument.
    std::uint8_t forcedInitialPage; // Screen 14 explicitly passes 1; zero means caller page.
};
inline constexpr std::array<OriginalUiScreenFactory50KG,14> originalUiScreenFactories50KG{{
    { 0,"UILang",  0x1B8190,0x1D11F0,0x1CCBA0,0,0},
    { 1,"UIArt1",  0x1B80A0,0x1D1350,0x1D1430,1,0},
    { 2,"UIArt2",  0x1B80F0,0x1D1350,0x1D1430,2,0},
    { 3,"UIArt3",  0x1B8140,0x1D1350,0x1D1430,3,0},
    { 4,"UIMain",  0x1B7E3C,0x1CF1B0,0x1CF2C0,0,0},
    { 5,"UIRider", 0x1B7E88,0x1CFD90,0x1CFE80,0,0},
    { 6,"UICourse",0x1B7ED4,0x1D0070,0x1D0140,0,0},
    { 7,"UIWin",   0x1B7F20,0x1D1550,0x1CCBA0,0,0},
    // Screen 8 uses the original undefined-choice branch 0x1B8278.
    { 9,"UITourn", 0x1B7F70,0x1D0270,0x1D0360,0,0},
    {10,"UIUnlock",0x1B7FBC,0x1D16B0,0x1D1780,0,0},
    {11,"UIHOF",   0x1B8008,0x1CF030,0x1CF100,0,0},
    {12,"UIOption",0x1B8054,0x1D0470,0x1D0580,0,0},
    {13,"UIGarage",0x1B822C,0x1D1860,0x1D1950,0,0},
    {14,"UITest",  0x1B81E0,0x1D1060,0x1D1130,0,1},
}};
inline constexpr const OriginalUiScreenFactory50KG*
findOriginalUiScreenFactory50KG(std::uint32_t id) noexcept {
    for(const auto& entry:originalUiScreenFactories50KG) if(entry.screen==id) return &entry;
    return nullptr;
}

// CLOSED identities for the bounded main/rider/course/options page factories.
// ui.dat's [Page N].PageFile supplies the actual pathname at 0x1CEC8C..1CECD4;
// the page index passed to the factory is N, independently of the filename.
struct OriginalUiPageFactory50KG {
    std::uint8_t screen,page;
    const char* originalType;
    std::uint32_t factoryPC,constructorPC,vtableVA,inputPC;
    std::uint8_t variant; // Rider player index; options controller index.
};
inline constexpr std::array<OriginalUiPageFactory50KG,15> originalUiBoundedPageFactories50KG{{
    {4,1,"PodUIPageMain",            0x1CFB80,0x1E5400,0x3D11E0,0x1E5E00,0},
    {4,2,"PodUIPageLegal",           0x1CFB80,0x1E49C0,0x3D12B0,0x1E5130,0},
    {4,3,"PodUIPageOpening",         0x1CFB80,0x1D7C80,0x3D15F0,0x1D7DF0,0},
    {4,4,"PodUIPageDemo",            0x1CFB80,0x1D8500,0x3D1520,0x1D8930,0},
    {4,5,"PodUIPageGalleryMenu",     0x1CFB80,0x20CA40,0x3D0550,0x20CEE0,0},
    {5,1,"PodUIPageRiderSelect",     0x1CFF00,0x1E7AC0,0x3D1110,0x1EA460,0},
    {5,2,"PodUIPageRiderSelect",     0x1CFF00,0x1E7AC0,0x3D1110,0x1EA460,1},
    {6,1,"PodUIPageCourseSelect",    0x1D0170,0x1D8D70,0x3D1450,0x1DEB90,0},
    {9,1,"PodUIPageStandings",       0x1D03E0,0x1ED7E0,0x3D0EF0,0x1F2310,0},
    {12,1,"PodUIPageOptionsMenu",    0x1D0E30,0x1F8DD0,0x3D0AE0,0x1F9640,0},
    {12,2,"PodUIPageControllerConfig",0x1D0E30,0x1F2F60,0x3D0E20,0x1F3950,0},
    {12,3,"PodUIPageControllerConfig",0x1D0E30,0x1F2F60,0x3D0E20,0x1F3950,1},
    {12,4,"PodUIPageGameOptions",    0x1D0E30,0x1F46A0,0x3D0D50,0x1F50F0,0},
    {12,5,"PodUIPageMemoryCard",     0x1D0E30,0x1F6880,0x3D0C80,0x1F6AB0,0},
    {12,6,"PodUIPageCredits",        0x1D0E30,0x1F7500,0x3D0BB0,0x1F8520,0},
}};
inline constexpr const OriginalUiPageFactory50KG*
findOriginalUiBoundedPageFactory50KG(std::uint32_t screen,std::uint32_t page) noexcept {
    for(const auto& entry:originalUiBoundedPageFactories50KG)
        if(entry.screen==screen && entry.page==page) return &entry;
    return nullptr;
}
} // namespace rr::game
