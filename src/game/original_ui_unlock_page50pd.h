#pragma once
// UIUnlock screen 10 / page 1: PodUIPageUnlock (ctor 209A80, vtable 3D0620),
// transcribed from SLUS_202.68. Pure page logic: no profile, audio, renderer
// or manager effect happens here. The session preflights and applies plans.
//
// Page vtable: +38 update 20CA30, +48 render 20B710, +74 activate 20AE40,
// +78 input 20B480, +84 init 209BE0, +AC open-start 20B3E0, +B0 opened
// 20B400, +B4 close-start 20B420, +B8 closed 20B470. Generic +90/+94 are the
// shared UIPage reset/hide traversals (2A57D8/2A4D58), as in every page.
#include "original_ui_options_menu_controller50kg.h"
#include "original_ui_route_decisions50kg.h"
#include "original_unlock_checks50pd.h"
#include "../assets/original_ui_pages50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace rr::game {
inline constexpr std::uint8_t originalUiUnlockScreen50PD=10,originalUiUnlockPage50PD=1;
// Authored UIUnlock Widgets.Dat [General] NumberOfWidgets.
inline constexpr std::uint16_t originalUiUnlockWidgetCount50PD=53;
// Widget roles fixed by the 20B710 manager+154 offsets (+0,+4,+C,+10,+14,+18).
inline constexpr std::uint16_t originalUiUnlockContinueWidget50PD=1;   // +0, STR_CONTINUE
inline constexpr std::uint16_t originalUiUnlockTitleWidget50PD=2;      // +4, STR_UNLOCK_PAGE_TITLE
inline constexpr std::uint16_t originalUiUnlockCongratsWidget50PD=3;   // never recoloured
inline constexpr std::uint16_t originalUiUnlockMessageWidget50PD=4;    // +C, dynamic 2 lines
inline constexpr std::array<std::uint16_t,3> originalUiUnlockBandWidgets50PD{{5,6,7}}; // +10..+18
// 20B710 localization: 0x151 "YOU HAVE UNLOCKED", galleries 0x152..0x154.
inline constexpr std::uint32_t originalUiUnlockHeaderLoc50PD=0x151;
inline constexpr std::array<std::uint32_t,3> originalUiUnlockGalleryLoc50PD{{0x152,0x153,0x154}};
// Fixed pod-name IDs for checks 2..7 (20B954..20C44C): 0xB1/0xBB/0xB3/0xC6/0xC2/0xC7.
inline constexpr std::array<std::int32_t,8> originalUiUnlockFixedNameLoc50PD{{-1,0xB1,0xBB,0xB3,0xC6,0xC2,0xC7,-1}};
// 20C468/20C4A0: gallery text and icon colours.
inline constexpr std::uint32_t originalUiUnlockGalleryTextColor50PD=0x00B4B4B4u;
inline constexpr std::uint32_t originalUiUnlockGalleryIconColor50PD=0x00141414u;
// 209C80..209CC4: init builds the four %02d_bkg sprites for these fixed pods
// after the rivals of course orders 4..12.
inline constexpr std::array<std::uint8_t,6> originalUiUnlockFixedSpritePods50PD{{3,1,14,25,21,26}};

enum class OriginalUiUnlockPhase50PD : std::uint8_t {
    requestedPage90,previousPage94,entry74,openingAC,openedB0,closingB4,closedB8
};
// 20B2C4..20B344: page vtable+8C background names for gallery tier 1/2/3.
enum class OriginalUiUnlockGallery50PD : std::uint8_t { none=0,podracers=1,pilots=2,planets=3 };
const char* originalUiUnlockGalleryTexture50PD(OriginalUiUnlockGallery50PD) noexcept; // artpod01/artpil01/artpla01

struct OriginalUiUnlockState50PD {
    OriginalUiNavigation50KG navigation{};
    bool loaded=false;
    std::uint32_t inputFlags42C=0;
    std::int32_t pod2D40=-1;               // displayed pod, -1 for none/gallery
    OriginalUnlockChecks50PD pending{};    // +2D44..+2D5C flags, +2D60 tier
    bool refreshText2D64=false;            // set by activate, cleared by render
    std::uint32_t activations=0;           // native count of 20AE40 runs
};
// 20B480 outcome after its grant: vtable+A4(this,1) or +A0(this,1,13|6).
enum class OriginalUiUnlockNext50PD : std::uint8_t { none,reshow,exit };
struct OriginalUiUnlockPlan50PD {
    OriginalUiUnlockState50PD state{};
    std::array<OriginalUiOptionsMenuCallback50KG,256> callbacks{};
    std::size_t callbackCount=0;
    bool writesInputFlags=false;
    bool clearScene3D8=false,applyBackground430=false; // 20B398..20B3B8
    bool setsPageBackground8C=false;                   // 20B2C4..20B344
    OriginalUiUnlockGallery50PD pageBackground8C=OriginalUiUnlockGallery50PD::none;
    bool startSong7=false,fadeMusic=false;             // 20B3E0 / 20B420
    bool saveUpdateDelta=false;float updateDelta=0;    // 20CA30
    // Input 20B480. Grant and request are one atomic route transaction in
    // the session; nothing here has been committed.
    bool pageAccepted=false;
    bool grantRequested=false;std::size_t grantCheck=0;std::uint32_t grantTier=0;
    OriginalUiUnlockNext50PD next=OriginalUiUnlockNext50PD::none;
    OriginalUiRouteDecision50KG route{};
    bool soundRequested=false;std::uint32_t soundId=0,soundPC=0,flushPC=0;
};
// 20B710 text/colour work, resolved by the session against real owners.
struct OriginalUiUnlockTextPlan50PD {
    bool gallery=false;
    OriginalUiUnlockGallery50PD galleryTier=OriginalUiUnlockGallery50PD::none;
    // Pod whose P+A8+8r (title/message/band) and P+AC+8r (Continue) colours apply.
    std::int32_t colorPod=-1;
    // Rival check: name ID is profile P+3C[colorPod]; otherwise nameLoc.
    bool rivalName=false;
    std::int32_t nameLoc=-1;
    std::uint32_t headerLoc=originalUiUnlockHeaderLoc50PD;
    std::uint32_t sourcePC=0;
};

// Authenticated UIUnlock ui.dat/unlock.dat/Widgets.Dat owner.
class OriginalUiUnlockPage50PD final {
public:
    OriginalUiUnlockPage50PD() noexcept;
    ~OriginalUiUnlockPage50PD();
    OriginalUiUnlockPage50PD(const OriginalUiUnlockPage50PD&)=delete;
    OriginalUiUnlockPage50PD& operator=(const OriginalUiUnlockPage50PD&)=delete;
    OriginalUiOptionsMenuStatus50KG bind(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        assets::OriginalUiPageDiagnostic50KG&) noexcept;
    const assets::OriginalUiPage50KG* page() const noexcept;
    const assets::OriginalUiDat50KG* pageDat() const noexcept;
    const assets::OriginalUiDat50KG* widgetsDat() const noexcept;
    // PodUIPodControl +40 (authored PodIndex), -1 when the control has none.
    std::int32_t podIndex(std::size_t controlId) const noexcept;
    // Group-2 control showing a pod, or -1. Three authored widgets: User_Rotate
    // pod, Huttese Overlay_Text, Soultree_Character (in that order).
    std::int32_t podControl(std::int32_t pod) const noexcept;
    bool initialState(OriginalUiUnlockState50PD&) const noexcept;
    // checks/rivalPod are consumed only by entry74 (20AE40 runs all eight
    // 187E30 leaves; rivalPod is the actual R of the rival check, -1 if none).
    OriginalUiOptionsMenuStatus50KG phase(const OriginalUiUnlockState50PD&,OriginalUiUnlockPhase50PD,
        const OriginalUnlockChecks50PD& checks,std::int32_t rivalPod,OriginalUiUnlockPlan50PD&) const noexcept;
    // controller/event are the original container arguments a1/a2.
    OriginalUiOptionsMenuStatus50KG input(const OriginalUiUnlockState50PD&,std::uint8_t controller,
        std::uint8_t event,std::uint32_t mode34,OriginalUiUnlockPlan50PD&) const noexcept;
    OriginalUiOptionsMenuStatus50KG update(const OriginalUiUnlockState50PD&,float elapsedSeconds,
        OriginalUiUnlockPlan50PD&) const noexcept;
    // 20B72C: only while +2D64 is set; clears it in the returned state.
    bool composeText(const OriginalUiUnlockState50PD&,OriginalUiUnlockTextPlan50PD&,
        OriginalUiUnlockState50PD& after) const noexcept;
private:
    struct Storage;std::unique_ptr<Storage> storage_;
};
// Exact route shapes produced by input, used by the session to revalidate.
bool originalUiUnlockRouteShape50PD(const OriginalUiRouteDecision50KG&,OriginalUiUnlockNext50PD) noexcept;
}
