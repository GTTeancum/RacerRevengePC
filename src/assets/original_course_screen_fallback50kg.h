#pragma once
#include "original_standard_archive_provider50la.h"
#include "original_course_popup_resource_proof50kg.h"
#include "original_standard_material_context50la.h"
#include <memory>
#include <string>
#include <cstring>
namespace rr::platform {class OriginalUiCourseOwnedLifecycle50KG;}
namespace rr::assets {
class OriginalStandardMaterialTable50LA;
class OriginalStandardMaterialObject50LA;
class OriginalStandardTextureTable50LA;
// Private source2475C0 admission issued by the actual retained Course Icon/SelectableIcon
// primitive. Does not create a row, stand in for PSM bytes, or grant draw custody.
class OriginalCourseScreenFallback50KG final {
    friend class rr::platform::OriginalUiCourseOwnedLifecycle50KG;
    friend class rr::platform::OriginalUiCoursePopupPayload50KG;
    friend class OriginalStandardMaterialTable50LA;
    friend class OriginalStandardMaterialObject50LA;
    OriginalCourseScreenFallback50KG()=default;
    std::weak_ptr<const int> occurrence_;
    std::shared_ptr<const OriginalCoursePopupResourceProof50KG> popup_;
    std::int32_t row_=-1;
    std::string name_;
    std::shared_ptr<const OriginalUiArchive50KG> course_;
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<OriginalStandardTextureTable50LA> textures_;
    bool current(const std::string& name,
        const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
        const std::shared_ptr<OriginalStandardTextureTable50LA>& textures)const noexcept {
        if(popup_)return popup_->current(name,row_,context,textures,false);
        if(occurrence_.expired()||row_<0||(name_!="ui\\textures\\nameblur"&&name_!="ui\\textures\\arrwlefttop"&&
            name_!="ui\\textures\\arrwlefttop_d"&&name_!="ui\\textures\\arrwrghttop"&&name_!="ui\\textures\\arrwrghttop_d")||!context_||context!=context_||
           !context_->live()||textures!=textures_||!textures_||!provider_||!provider_->sealed()||
           !course_||!course_->loaded()||course_->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::course))return false;
        try {
            if(!OriginalStandardArchiveProvider50LA::sameName(name,name_+".psm"))return false;
            const auto authored=course_->find(name_+".pst");
            const auto widgets=course_->find("UI/UICourse/Widgets.Dat");
            OriginalStandardArchiveProvider50LA::MemberView absent,selected;
            if(!widgets.identity||!authored.identity||!authored.data||authored.size!=authored.identity->bytes||
               provider_->lookup132820(name_+".psm",absent)!=OriginalStandardArchiveProvider50LA::Lookup::absent||
               provider_->lookup132820(name_+".pst",selected)!=OriginalStandardArchiveProvider50LA::Lookup::present||
               !selected.member||!selected.data||selected.size!=authored.size||
               std::memcmp(selected.data,authored.data,authored.size))return false;
            // Preserve the actual132820 winner. Several original archives own
            // the same nameblur bytes; never substitute the later Course mount.
            OriginalUiEncodedArchiveView50KG encoded;
            if(!provider_->encodedUiInput(selected.mount,encoded)||encoded.providerIdentity()!=provider_.get()||
               !encoded.identity())return false;
            OriginalStandardArchiveProvider50LA::MountView mount;
            return provider_->mount(selected.mount,mount)&&mount.archiveSha256&&
                *mount.archiveSha256==encoded.identity()->sha256&&mount.archiveBytes==encoded.identity()->bytes&&
                selected.revision==provider_->revision();
        }catch(...){return false;}
    }
public:
    OriginalCourseScreenFallback50KG(const OriginalCourseScreenFallback50KG&)=delete;
    OriginalCourseScreenFallback50KG& operator=(const OriginalCourseScreenFallback50KG&)=delete;
};
}
