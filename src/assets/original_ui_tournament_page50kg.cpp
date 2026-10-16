#include "original_ui_tournament_page50kg.h"
#include <new>
#include <utility>

namespace rr::assets {
namespace {
bool fail(OriginalUiTournamentPageDiagnostic50KG& d,
    OriginalUiTournamentPageStatus50KG status,std::uint32_t pc) noexcept {
    d.status=status;d.sourcePC=pc;return false;
}
}
bool OriginalUiTournamentPageData50KG::load(
    std::shared_ptr<const OriginalUiArchive50KG> archive,
    OriginalUiTournamentPageDiagnostic50KG& d) noexcept {
    using S=OriginalUiTournamentPageStatus50KG;d={};
    if(storage_)return fail(d,S::alreadyLoaded,0x1EDAA0);
    if(!archive||!archive->loaded()||
       archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament))
        return fail(d,S::archive,0x1D0360);
    try {
        auto s=std::make_unique<Storage>();
        OriginalUiPageIndex50KG index;
        if(!readOriginalUiDat50KG(*archive,"UI/UITourn/ui.dat",s->ui,d.parse)||
           !readOriginalUiPageIndex50KG(s->ui,index,d.parse))
            return fail(d,S::parse,0x1CEC8C);
        // Source factory1D03E0 admits exactly page1, constructed as Standings.
        // Path comes from the authenticated index, not the type or screen name.
        if(index.pageFiles.size()!=1||index.pageFiles[0]!="standing.dat")
            return fail(d,S::page,0x1D03FC);
        if(!readOriginalUiDat50KG(*archive,"UI/UITourn/"+index.pageFiles[0],s->pageDat,d.parse)||
           !readOriginalUiDat50KG(*archive,"UI/UITourn/Widgets.Dat",s->widgets,d.parse,
                OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
           !readOriginalUiPage50KG(s->pageDat,s->widgets,s->page,d.parse))
            return fail(d,S::parse,0x1D63B0);
        if(s->page.pageNo!=1||s->page.groups.size()!=3||s->page.controls.size()!=3||
           !s->page.hasPopup||s->page.openSize!=69||s->page.openTime!=0.5f||s->page.start!="Closed")
            return fail(d,S::page,0x1EDAB8);
        s->archive=std::move(archive);storage_=std::move(s);return true;
    }catch(const std::bad_alloc&){return fail(d,S::allocation,0x1EDAA0);}
     catch(...){return fail(d,S::parse,0x1EDAA0);}
}
const OriginalUiPage50KG* OriginalUiTournamentPageData50KG::page()const noexcept {
    return storage_?&storage_->page:nullptr;
}
const OriginalUiDat50KG* OriginalUiTournamentPageData50KG::uiDat()const noexcept {
    return storage_?&storage_->ui:nullptr;
}
const OriginalUiDat50KG* OriginalUiTournamentPageData50KG::pageDat()const noexcept {
    return storage_?&storage_->pageDat:nullptr;
}
const OriginalUiDat50KG* OriginalUiTournamentPageData50KG::widgetsDat()const noexcept {
    return storage_?&storage_->widgets:nullptr;
}
std::shared_ptr<const OriginalUiArchive50KG> OriginalUiTournamentPageData50KG::archive()const noexcept {
    return storage_?storage_->archive:nullptr;
}
}
