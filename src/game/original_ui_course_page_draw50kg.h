// NEW, UNVERIFIED reconstruction after workspace rollback. Source-only WIP.
#pragma once
#include "original_ui_course_widgets50kg.h"
#include "original_ui_text50kg.h"
#include "../assets/original_ui_localization50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include <array>
#include <memory>
#include <string>
#include <vector>
namespace rr::platform { class OriginalUiCourseDrawContribution50KG; }
namespace rr::game {
struct OriginalUiCoursePagePacket50KG {
    const char* materialName=nullptr;
    const assets::OriginalUiMemberIdentity50KG* materialIdentity=nullptr;
    std::vector<assets::Vertex48B> vertices;
    std::uint32_t sourcePC=0;
    std::uint16_t pageMemberOffset=0;
    // 1DE450 -> 2DF8A0 -> 24E670 -> 24E4A0 is resident-mip selection,
    // not draw order. The admitted genuine overlay1 has one level only.
    float minimumTextureResidencyPriority=0;
};
struct OriginalUiCoursePageTextSnapshot50KG {
    std::uint16_t memberOffset=0,capacity=60,count=0,packetCount=0;
    std::size_t formattedByteLength=0;
    std::int32_t measuredWidth=0,measuredHeight=0,authoredX=320,authoredY=0;
    float positionedX=0,positionedY=0;
    std::uint32_t color54=0xFFFFFF;std::int16_t alpha58=255;
    std::string text;
};
// Observations only. Session must obtain these from its coherent selected
// profile/career owner; this value does not grant a route or profile write.
struct OriginalUiCourseTournamentDrawState50KG {
    std::uint32_t career=0,slot=0,tier=0;
    std::uint8_t progress=0,medal=0;
    std::int16_t remainingPrize=0;
    std::array<std::int32_t,7> killPercent{};
    std::int32_t par=0;
};
enum class OriginalUiCoursePageDrawError50KG : std::uint8_t {
    none,alreadyPrepared,notPrepared,archive,localization,images,material,parse,
    geometry,palette,widget,poisoned,allocation
};
struct OriginalUiCoursePageDrawDiagnostic50KG {
    OriginalUiCoursePageDrawError50KG error=OriginalUiCoursePageDrawError50KG::none;
    std::uint32_t sourcePC=0;
    assets::OriginalUiPageDiagnostic50KG parse{};
    OriginalUiTextDiagnostic50KG text{};
    OriginalUiCourseWidgetDiagnostic50KG widget{};
};
// Embedded ScreenText / ScreenTexture objects created by 1D8F10, not DAT
// widgets. Bounded mode0, subtype1, laps1..25, fresh Course slots1..4. The caller must
// enforce retained profile/selection and apply palette before collection.
class OriginalUiCoursePageDraw50KG final {
public:
    bool prepare(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const assets::OriginalUiEnglish50KG&,
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,
        std::uint32_t denmrk16Image,std::uint32_t overlay1Image,
        std::int32_t packetZMaximum,OriginalUiCoursePageDrawDiagnostic50KG&) noexcept;
    bool prepared()const noexcept{return prepared_&&!poisoned_;}
    bool poisoned()const noexcept{return poisoned_;}
    const assets::OriginalUiMemberIdentity50KG* pageIdentity()const noexcept{return pageIdentity_;}
    const std::array<OriginalUiCoursePagePacket50KG,6>* packets()const noexcept{return prepared()?&packets_:nullptr;}
    const std::array<OriginalUiCoursePageTextSnapshot50KG,2>* texts()const noexcept{return prepared()?&texts_:nullptr;}
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images()const noexcept{return images_;}
    // Source1DAF78..1DB258: genuine embedded B30 text, LOC126/133/134.
    bool setEventSelection(std::uint32_t subtype,std::uint32_t laps,std::uint32_t entrants,OriginalUiCoursePageDrawDiagnostic50KG&)noexcept;
    bool eventDetailVisible()const noexcept{return eventSubtype_!=3;}
    bool setLapCount(std::uint32_t laps,OriginalUiCoursePageDrawDiagnostic50KG&) noexcept;
    // Separate original mode2 branch1DB81C..1DD530. No mode0 lap/entrant
    // text is constructed. Retains original40/10/15-character allocations.
    bool prepareTournamentFresh(std::shared_ptr<const assets::OriginalUiArchive50KG>,
        const assets::OriginalUiEnglish50KG&,
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG>,
        std::uint32_t denmrk14Image,const std::array<std::uint32_t,3>& medalImages,
        std::int32_t packetZMaximum,OriginalUiCoursePageDrawDiagnostic50KG&) noexcept;
    bool tournamentFresh()const noexcept{return prepared()&&tournament_;}
    //1DA634 ->1DDF00 initial planet label. Render replaces it with actual tier.
    bool setTournamentPlanetName(std::uint32_t planet,OriginalUiCoursePageDrawDiagnostic50KG&) noexcept;
    bool updateTournamentFresh(OriginalUiCourseWidgets50KG&,
        const OriginalUiCourseTournamentDrawState50KG&,OriginalUiCoursePageDrawDiagnostic50KG&) noexcept;
    const std::array<OriginalUiCoursePagePacket50KG,24>* tournamentPackets()const noexcept {
        return tournamentFresh()&&tournamentFrame_?&tournamentPackets_:nullptr;
    }
    const std::array<OriginalUiCoursePageTextSnapshot50KG,21>* tournamentTexts()const noexcept {
        return tournamentFresh()?&tournamentTexts_:nullptr;
    }
    bool applyPlanetPalette(OriginalUiCourseWidgets50KG&,std::uint32_t primary79C,
        std::uint32_t secondary7A0,OriginalUiCoursePageDrawDiagnostic50KG&) noexcept;
private:
    friend class rr::platform::OriginalUiCourseDrawContribution50KG;
    bool applyTournamentWidgetStage(std::uint8_t,OriginalUiCourseWidgets50KG&,
        OriginalUiCoursePageDrawDiagnostic50KG&) noexcept;
    bool prepared_=false,poisoned_=false;
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive_;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images_;
    const assets::OriginalUiMemberIdentity50KG* pageIdentity_=nullptr;
    std::uint32_t fontImage_=0,lapCount_=0;
    float packetZ_=0;
    std::string lapLabel126_,singularLap133_,pluralLaps134_,eventLabel125_,entrantsLabel127_;
    std::array<std::string,3> eventNames_{};
    std::uint32_t eventSubtype_=1,eventEntrants_=8;
    std::array<OriginalUiCoursePagePacket50KG,6> packets_{};
    std::array<OriginalUiCoursePageTextSnapshot50KG,2> texts_{};
    bool tournament_=false,tournamentFrame_=false,tournamentPlanet_=false;
    std::uint8_t tournamentWidgetStage_=0;
    std::uint32_t primary_=0xFFFFFF,secondary_=0xFFFFFF;
    std::array<std::uint32_t,3> medalImages_{};
    std::array<std::string,6> planetNames_{};
    std::array<std::string,32> tournamentLabels_{}; // English122..153.
    std::array<OriginalUiCoursePagePacket50KG,24> tournamentPackets_{};
    std::array<OriginalUiCoursePageTextSnapshot50KG,21> tournamentTexts_{};
};
}
