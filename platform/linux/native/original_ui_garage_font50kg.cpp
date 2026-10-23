#include "original_ui_garage_screen_text50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
namespace original_main_entry50KG {
namespace {
template<class A,class B>bool sameFontOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
    return a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
}
bool Application::loadGarageTextFont29CB74(
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> archive,
    rr::assets::OriginalApplicationFontDiagnostic50LA& d)const noexcept {
    return loadGarageTextFont29CB74(std::move(archive),rr::assets::OriginalApplicationFonts50LA::Name::frontendDenmrk16,d);
}
bool Application::loadGarageTextFont29CB74(
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> archive,
    rr::assets::OriginalApplicationFonts50LA::Name name,
    rr::assets::OriginalApplicationFontDiagnostic50LA& d)const noexcept {
    using S=rr::assets::OriginalApplicationFontStatus50LA;d={};std::uint32_t pause=0;
    if(!rr::platform::OriginalUiGarageScreenText50KG::authorityThread(entitiesOwner_)){
        d.status=S::phase;d.sourcePC=0x29CB74;return false;
    }
    if(name!=rr::assets::OriginalApplicationFonts50LA::Name::frontendDenmrk16&&
       name!=rr::assets::OriginalApplicationFonts50LA::Name::frontendVenus16&&
       name!=rr::assets::OriginalApplicationFonts50LA::Name::frontendDenmrk14){
        d.status=S::source;d.sourcePC=0x29CB74;return false;
    }
    // Private downstream source leaf only. No archive mount, source-corpus
    // replacement, dictionary selection, Widget Entity or renderer readiness.
    if(!animationClock().readPause11C(pause)||!entitiesOwner_||!entitiesOwner_->application10()||
       !videoOwner_||!videoOwner_->applicationAnimation_.sameOwner(animationClock())||
       !materialContextOwner_||!materialContextOwner_->live()||
       materialContextOwner_->videoIdentity()!=videoOwner_.get()||
       !materialContextOwner_->applicationAnimationClock().sameOwner(animationClock())||
       !sameFontOwner(materialContextOwner_->materialApplication(),materialApplication)||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||
       !rr::assets::OriginalApplicationFonts50LA::domainApplicationMatches(standardMaterialDomainOwner_,materialApplication)||
       (fonts328Owner_&&(!sameFontOwner(fonts328Owner_->domain_,standardMaterialDomainOwner_)||
                        !sameFontOwner(fonts328Owner_->context_,materialContextOwner_)))) {
        d.status=S::phase;d.sourcePC=0x29CB74;return false;
    }
    if(!archive||!archive->loaded()||archive->identity()!=rr::assets::originalUiArchiveIdentity50KG(
            rr::assets::OriginalUiArchiveId50KG::fonts)){
        d.status=S::source;d.sourcePC=0x29CB74;return false;
    }
    try {return fonts328()->loadGarageText29CB74(std::move(archive),materialDirectory36B970_,fontDirectory36B420_,name,d);}
    catch(const std::bad_alloc&){d.status=S::allocation;d.sourcePC=0x2824D0;return false;}
    catch(...){d.status=S::phase;d.sourcePC=0x29CB74;return false;}
}
}
