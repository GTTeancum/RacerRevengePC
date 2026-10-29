#pragma once
#include "original_standard_archive_provider50la.h"
#include "original_standard_material_context50la.h"
#include <memory>
#include <cstring>
namespace rr::platform {class OriginalUiCoursePopupPayload50KG;}
namespace rr::assets {
class OriginalStandardTextureTable50LA;
class OriginalCourseScreenFallback50KG;
class OriginalCourseScreenAuthored50KG;
class OriginalStandardMaterialObject50LA;
// Distinct Course page438 occurrence, not a Widget ordinal or Options grant.
class OriginalCoursePopupResourceProof50KG final {
    friend class rr::platform::OriginalUiCoursePopupPayload50KG;
    friend class OriginalCourseScreenFallback50KG;
    friend class OriginalCourseScreenAuthored50KG;
    friend class OriginalStandardMaterialObject50LA;
    OriginalCoursePopupResourceProof50KG()=default;
    std::weak_ptr<const int> occurrence_;
    std::size_t role_=7;std::int32_t row_=-1;
    const void* primitive_=nullptr;std::uint32_t offset_=0,callerPC_=0;
    std::shared_ptr<const OriginalUiArchive50KG> course_;
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<OriginalStandardTextureTable50LA> textures_;
    static const char* name(std::size_t role)noexcept {
        constexpr const char* names[]{"ui\\textures\\icon_x.psm","ui\\textures\\icon_tri.psm",
            "ui\\textures\\icon_sqr.psm","ui\\textures\\icon_cir.psm",
            "ui\\textures\\overlay1top.psm","ui\\textures\\overlay1top.psm","ui\\textures\\overlay1top.psm"};
        return role<7?names[role]:nullptr;
    }
    bool current(const std::string& requested,std::int32_t row,
        const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
        const std::shared_ptr<OriginalStandardTextureTable50LA>& textures,bool positive)const noexcept {
        using P=OriginalStandardArchiveProvider50LA;
        constexpr std::uint32_t offsets[]{0x160,0x1C0,0x220,0x280,0x40,0xA0,0x100};
        constexpr std::uint32_t callers[]{0x1FF058,0x1FF110,0x1FF1C8,0x1FF280,0x1FF338,0x1FF4AC,0x1FF68C};
        if(occurrence_.expired()||role_>=7||!primitive_||offset_!=offsets[role_]||callerPC_!=callers[role_]||(role_>=4)!=positive||row_<0||row!=row_||
           !P::sameName(requested,name(role_))||!context_||context!=context_||!context_->live()||
           !textures_||textures!=textures_||!provider_||!provider_->sealed()||!course_||!course_->loaded()||
           course_->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::course))return false;
        try {
            const auto track=course_->find("UI/UICourse/track.dat");
            if(!track.identity||track.size!=4706||std::strcmp(track.identity->sha256,
                "d445012d2342ba33c685e3b2b1f78e3b2d648c4d8057fa1427d5f58639ea7268"))return false;
            P::MemberView selected;
            if(positive){const auto authored=course_->find(name(role_));
                return authored.identity&&authored.data&&provider_->lookup132820(requested,selected)==P::Lookup::present&&
                    selected.member&&selected.data&&selected.size==authored.size&&
                    !std::memcmp(selected.data,authored.data,authored.size);
            }
            if(provider_->lookup132820(requested,selected)!=P::Lookup::absent)return false;
            auto pst=requested;pst.replace(pst.size()-4,4,".pst");
            if(provider_->lookup132820(pst,selected)!=P::Lookup::present||!selected.member||!selected.data)return false;
            OriginalUiEncodedArchiveView50KG encoded;
            if(!provider_->encodedUiInput(selected.mount,encoded)||encoded.providerIdentity()!=provider_.get()||!encoded.identity())return false;
            const auto* member=originalUiMemberIdentity50KG(encoded.identity()->id,selected.ordinal);
            if(!member||!P::sameName(member->name,pst)||member->bytes!=selected.size||selected.revision!=provider_->revision())return false;
            if(role_<2)return encoded.identity()==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::controllerPopup)||
                encoded.identity()==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays);
            const auto authored=course_->find(pst);
            return authored.identity&&authored.data&&authored.size==selected.size&&
                !std::strcmp(authored.identity->sha256,member->sha256)&&!std::memcmp(authored.data,selected.data,selected.size);
        }catch(...){return false;}
    }
};
}
