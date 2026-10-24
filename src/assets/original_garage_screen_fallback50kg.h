#pragma once
#include "original_standard_archive_provider50la.h"
#include "original_standard_material_context50la.h"
#include "original_ui_archive50kg.h"
#include <cstring>
#include <memory>
#include <string>
#include <vector>
namespace rr::platform {class OriginalUiGarageQuad50KG;}
namespace rr::assets {
class OriginalStandardTextureTable50LA;
class OriginalStandardMaterialTable50LA;
class OriginalStandardMaterialObject50LA;
// Private canonical2475C0 source admission, issued only by an actual Garage
// embedded Quad after2465B0 retained its real numeric row. Not a replacement
// PSM, immutable draw policy, public missing-file flag, or readiness receipt.
class OriginalGarageScreenFallback50KG final {
    friend class rr::platform::OriginalUiGarageQuad50KG;
    friend class OriginalStandardMaterialTable50LA;
    friend class OriginalStandardMaterialObject50LA;
    OriginalGarageScreenFallback50KG()=default;
    struct ConstructionObservation {
        OriginalUiEncodedArchiveView50KG selectedArchive;
        const OriginalStandardArchiveProvider50LA* providerIdentity=nullptr;
        const std::uint8_t* selectedBytes=nullptr;
        std::size_t selectedSize=0,mount=static_cast<std::size_t>(-1),ordinal=static_cast<std::size_t>(-1);
        std::uint64_t revision=0,bankGeneration=0;
    };
    std::int32_t row_=-1;
    std::string name_,pstMember_;
    std::vector<std::uint8_t> pstBytes_;
    OriginalUiArchiveId50KG pstArchive_=OriginalUiArchiveId50KG::garage;
    std::shared_ptr<const OriginalUiArchive50KG> garage_;
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<OriginalStandardTextureTable50LA> textures_;
    bool current(const std::string& materialName,
        const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
        const std::shared_ptr<OriginalStandardTextureTable50LA>& textures,
        ConstructionObservation* observation=nullptr)const noexcept {
        if(observation)*observation=ConstructionObservation{};
        if(row_<0||!context_||context!=context_||!context_->live()||textures!=textures_||!textures_||
           !garage_||!garage_->loaded()||garage_->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::garage)||
           !garage_->find("UI/UIGarage/garage.dat").identity||!provider_||!provider_->sealed()||pstBytes_.empty())return false;
        try {
            auto requested=materialName;for(char& c:requested)if(c>='A'&&c<='Z')c=char(c-'A'+'a');
            if(requested!=name_+".psm")return false;
            const bool overlay=name_=="ui\\textures\\icon_x"||name_=="ui\\textures\\icon_tri";
            const bool sharedGarageIcon=name_=="ui\\textures\\icon_sqr"||name_=="ui\\textures\\icon_cir";
            const bool garage=sharedGarageIcon||name_=="ui\\textures\\podstat2"||name_=="ui\\textures\\notch";
            //The proof retains the actual132820 winner, not a preferred
            //archive alias. Only these two shared icons have this authenticated
            //equal-byte CONTPOP/OVERLAYS source relationship.
            const bool sharedSource=pstArchive_==OriginalUiArchiveId50KG::overlays||
                pstArchive_==OriginalUiArchiveId50KG::controllerPopup;
            if((!overlay&&!garage)||(overlay&&!sharedSource)||
               (garage&&pstArchive_!=OriginalUiArchiveId50KG::garage&&
                !(sharedGarageIcon&&pstArchive_==OriginalUiArchiveId50KG::overlays)))return false;
            if(sharedGarageIcon){
                // Both original archives carry the same square/circle image.
                // Retain the actual lookup winner, never replace its bytes.
                const auto authored=garage_->find(name_+".pst");
                if(!authored.identity||!authored.data||authored.size!=pstBytes_.size()||
                   std::memcmp(authored.data,pstBytes_.data(),authored.size))return false;
            }
            OriginalStandardArchiveProvider50LA::MemberView psm,pst;
            if(provider_->lookup132820(requested,psm)!=OriginalStandardArchiveProvider50LA::Lookup::absent||
               provider_->lookup132820(name_+".pst",pst)!=OriginalStandardArchiveProvider50LA::Lookup::present||
               !pst.member||*pst.member!=pstMember_||!pst.data||pst.size!=pstBytes_.size()||
               std::memcmp(pst.data,pstBytes_.data(),pst.size))return false;
            OriginalStandardArchiveProvider50LA::MountView mount;
            const auto* expected=originalUiArchiveIdentity50KG(pstArchive_);
            const bool valid=expected&&provider_->mount(pst.mount,mount)&&mount.archiveSha256&&
                *mount.archiveSha256==expected->sha256&&mount.archiveBytes==expected->bytes;
            // Freeze evidence only while observing the actual constructor's
            // absence/winner branch. Non-UI origins remain uncaptured, not relabeled.
            if(valid&&observation&&(overlay||sharedGarageIcon)&&psm.revision==pst.revision&&
               pst.revision==provider_->revision()&&provider_->sealed()){
                OriginalUiEncodedArchiveView50KG retained;
                if(provider_->encodedUiInput(pst.mount,retained)&&retained.providerIdentity()==provider_.get()&&
                   retained.identity()==expected){
                    observation->selectedArchive=std::move(retained);observation->providerIdentity=provider_.get();
                    observation->selectedBytes=pst.data;observation->selectedSize=pst.size;
                    observation->mount=pst.mount;observation->ordinal=pst.ordinal;
                    observation->revision=pst.revision;observation->bankGeneration=pst.bankGeneration;
                }
            }
            return valid;
        }catch(...){return false;}
    }
public:
    ~OriginalGarageScreenFallback50KG()=default;
    OriginalGarageScreenFallback50KG(const OriginalGarageScreenFallback50KG&)=delete;
    OriginalGarageScreenFallback50KG& operator=(const OriginalGarageScreenFallback50KG&)=delete;
};
}
