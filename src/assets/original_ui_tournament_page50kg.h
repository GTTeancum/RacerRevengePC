#pragma once
#include "original_ui_pages50kg.h"
#include <memory>

namespace rr::assets {
enum class OriginalUiTournamentPageStatus50KG : std::uint8_t {
    ok,alreadyLoaded,archive,page,parse,allocation
};
struct OriginalUiTournamentPageDiagnostic50KG {
    OriginalUiTournamentPageStatus50KG status=OriginalUiTournamentPageStatus50KG::ok;
    std::uint32_t sourcePC=0;
    OriginalUiPageDiagnostic50KG parse{};
};
// Authenticated cold data owner for screen9/page1. Retains the same archive
// backing every borrowed identity and all authored DAT values. Not a rendering
// owner or a completed native session; load success cannot admit screen9.
class OriginalUiTournamentPageData50KG final {
public:
    OriginalUiTournamentPageData50KG()=default;
    OriginalUiTournamentPageData50KG(const OriginalUiTournamentPageData50KG&)=delete;
    OriginalUiTournamentPageData50KG& operator=(const OriginalUiTournamentPageData50KG&)=delete;
    bool load(std::shared_ptr<const OriginalUiArchive50KG>,
        OriginalUiTournamentPageDiagnostic50KG&) noexcept;
    bool loaded() const noexcept {return bool(storage_);}
    const OriginalUiPage50KG* page() const noexcept;
    const OriginalUiDat50KG* uiDat() const noexcept;
    const OriginalUiDat50KG* pageDat() const noexcept;
    const OriginalUiDat50KG* widgetsDat() const noexcept;
    std::shared_ptr<const OriginalUiArchive50KG> archive() const noexcept;
private:
    struct Storage {
        std::shared_ptr<const OriginalUiArchive50KG> archive;
        OriginalUiDat50KG ui,pageDat,widgets;
        OriginalUiPage50KG page;
    };
    std::unique_ptr<Storage> storage_;
};
}
