#pragma once
#include "original_standard_archive_provider50la.h"
#include "original_standard_material_context50la.h"
#include <memory>
#include <string>
#include <cstring>
namespace rr::platform {class OriginalUiGarageScreenText50KG;}
namespace rr::assets {
class OriginalStandardMaterialTable50LA;
class OriginalStandardMaterialObject50LA;
class OriginalStandardTextureTable50LA;
// Only an actually retained loading Text primitive issues this pending proof.
// It grants no draw, row creation, font identity replacement or bank closure.
class OriginalLoadingScreenFallback50LA final {
    friend class rr::platform::OriginalUiGarageScreenText50KG;
    friend class OriginalStandardMaterialTable50LA;
    friend class OriginalStandardMaterialObject50LA;
    OriginalLoadingScreenFallback50LA()=default;
    std::weak_ptr<const int> occurrence_;
    std::int32_t row_=-1;
    std::string name_;
    std::shared_ptr<const OriginalUiArchive50KG> fonts_;
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<OriginalStandardTextureTable50LA> textures_;
    bool current(const std::string& name,
        const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
        const std::shared_ptr<OriginalStandardTextureTable50LA>& textures)const noexcept {
        if(occurrence_.expired()||row_<0||!context_||context!=context_||!context_->live()||
           textures!=textures_||!textures_||!provider_||!provider_->sealed()||
           !fonts_||!fonts_->loaded()||fonts_->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::fonts)||
           (name_!="font\\denmrk16"&&name_!="font\\venus16"))return false;
        try {
            if(!OriginalStandardArchiveProvider50LA::sameName(name,name_+".psm"))return false;
            const auto pst=fonts_->find(name_+".pst");
            OriginalStandardArchiveProvider50LA::MemberView absent,selected;
            if(!pst.identity||!pst.data||pst.size!=pst.identity->bytes||
               provider_->lookup132820(name_+".psm",absent)!=OriginalStandardArchiveProvider50LA::Lookup::absent||
               provider_->lookup132820(name_+".pst",selected)!=OriginalStandardArchiveProvider50LA::Lookup::present||
               !selected.data||selected.size!=pst.size||std::memcmp(selected.data,pst.data,pst.size))return false;
            OriginalStandardArchiveProvider50LA::MountView mount;
            const auto* expected=fonts_->identity();
            return provider_->mount(selected.mount,mount)&&mount.archiveName&&mount.archiveSha256&&
                *mount.archiveName==expected->discPath&&*mount.archiveSha256==expected->sha256&&
                mount.archiveBytes==expected->bytes&&mount.memberCount==expected->memberCount;
        }catch(...){return false;}
    }
public:
    OriginalLoadingScreenFallback50LA(const OriginalLoadingScreenFallback50LA&)=delete;
    OriginalLoadingScreenFallback50LA& operator=(const OriginalLoadingScreenFallback50LA&)=delete;
};
}
