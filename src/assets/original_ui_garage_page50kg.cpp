// NEW reconstruction from authenticated US-disc data and SLUS_202.68 evidence.
#include "original_ui_garage_page50kg.h"
#include <new>
#include <utility>

namespace rr::assets {
namespace {
bool fail(OriginalUiGaragePageDiagnostic50KG& d,
    OriginalUiGaragePageStatus50KG status,std::uint32_t pc) noexcept {
    d.status=status;d.sourcePC=pc;return false;
}
bool readCamera(const OriginalUiDat50KG& dat,OriginalUiGarageCameraConfig50KG& camera,
    OriginalUiPageDiagnostic50KG& d) {
    const auto* fields=dat.section("PageInfo");
    if(!fields){d={OriginalUiPageError50KG::missingSection,0,0x1D7A90};return false;}
    if(!originalUiTriple50KG(*fields,"CameraPosition1",{{0,0,0}},camera.position,d,0x1D7B74)||
       !originalUiTriple50KG(*fields,"CameraLook1",{{0,-1,0}},camera.look,d,0x1D7BB0)||
       !originalUiTriple50KG(*fields,"CameraUp1",{{0,0,1}},camera.up,d,0x1D7BE8)||
       !originalUiNumber50KG(*fields,"CameraFOV1",35,camera.fovDegrees,d,0x1D7C10)||
       !originalUiNumber50KG(*fields,"CameraRoll1",0,camera.rollRadians,d,0x1D7C34))return false;
    camera.identity=dat.identity();return true;
}
}
bool OriginalUiGaragePageData50KG::load(
    std::shared_ptr<const OriginalUiArchive50KG> archive,
    OriginalUiGaragePageDiagnostic50KG& d) noexcept {
    using S=OriginalUiGaragePageStatus50KG;d={};
    if(storage_)return fail(d,S::alreadyLoaded,0x1F9FF0);
    if(!archive||!archive->loaded()||archive->identity()!=
       originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garage))
        return fail(d,S::archive,0x1D1950);
    try {
        auto s=std::make_unique<Storage>();OriginalUiPageIndex50KG index;
        if(!readOriginalUiDat50KG(*archive,"UI/UIGarage/ui.dat",s->ui,d.parse)||
           !readOriginalUiPageIndex50KG(s->ui,index,d.parse))
            return fail(d,S::parse,0x1D1950);
        // The source1D1BD0 page factory admits page1/ctor1F9FF0. Resolve its
        // path from the authenticated index, never from a guessed page name.
        if(index.pageFiles.size()!=1||index.pageFiles[0]!="garage.dat")
            return fail(d,S::page,0x1D1BD0);
        if(!readOriginalUiDat50KG(*archive,"UI/UIGarage/"+index.pageFiles[0],s->pageDat,d.parse)||
           !readOriginalUiDat50KG(*archive,"UI/UIGarage/Widgets.Dat",s->widgets,d.parse,
                OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
           !readOriginalUiPage50KG(s->pageDat,s->widgets,s->page,d.parse)||
           !readCamera(s->pageDat,s->camera,d.parse)||
           !readOriginalUiMovementControl50KG(s->widgets,6,s->movement,d.parse))
            return fail(d,S::parse,d.parse.sourcePC?d.parse.sourcePC:0x1FA2D0);
        // These are authenticated data-shape checks, not session admission.
        // Counts2/7 are inclusive source loops, so retain group0 and control0.
        if(s->page.pageNo!=1||s->page.groups.size()!=3||s->page.controls.size()!=8||
           s->page.widgetCount!=22||s->page.hasPopup||s->page.openSize!=69||
           s->page.openTime!=0.5f||s->page.start!="Closed")
            return fail(d,S::page,0x1FA2D0);
        const auto* watto=s->widgets.section("Widget 10");
        const auto* type=watto?watto->find("Type"):nullptr;
        const auto* mcf=watto?watto->find("MCFFile"):nullptr;
        if(!type||type->text!="Watto"||!mcf||mcf->text!="ui\\anim\\wg.mcf")
            return fail(d,S::page,0x1FA318);
        s->wattoMcf=archive->find(mcf->text);
        if(!s->wattoMcf.identity||!s->wattoMcf.data||!s->wattoMcf.size)
            return fail(d,S::resource,0x1FA318);
        s->archive=std::move(archive);storage_=std::move(s);return true;
    }catch(const std::bad_alloc&){return fail(d,S::allocation,0x1F9FF0);}
     catch(...){return fail(d,S::parse,0x1FA2D0);}
}
const OriginalUiPage50KG* OriginalUiGaragePageData50KG::page()const noexcept {
    return storage_?&storage_->page:nullptr;
}
const OriginalUiDat50KG* OriginalUiGaragePageData50KG::uiDat()const noexcept {
    return storage_?&storage_->ui:nullptr;
}
const OriginalUiDat50KG* OriginalUiGaragePageData50KG::pageDat()const noexcept {
    return storage_?&storage_->pageDat:nullptr;
}
const OriginalUiDat50KG* OriginalUiGaragePageData50KG::widgetsDat()const noexcept {
    return storage_?&storage_->widgets:nullptr;
}
const OriginalUiGarageCameraConfig50KG* OriginalUiGaragePageData50KG::camera()const noexcept {
    return storage_?&storage_->camera:nullptr;
}
const OriginalUiMovementControl50KG* OriginalUiGaragePageData50KG::movement()const noexcept {
    return storage_?&storage_->movement:nullptr;
}
OriginalUiMemberView50KG OriginalUiGaragePageData50KG::wattoMcf()const noexcept {
    return storage_?storage_->wattoMcf:OriginalUiMemberView50KG{};
}
std::shared_ptr<const OriginalUiArchive50KG> OriginalUiGaragePageData50KG::archive()const noexcept {
    return storage_?storage_->archive:nullptr;
}
}
