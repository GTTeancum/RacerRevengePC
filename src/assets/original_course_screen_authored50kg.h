#pragma once
#include "original_standard_archive_provider50la.h"
#include "original_course_popup_resource_proof50kg.h"
#include "original_standard_material_source50la.h"
#include "original_standard_material_context50la.h"
#include <memory>
#include <cstring>
namespace rr::platform {class OriginalUiCourseOwnedLifecycle50KG;}
namespace rr::assets {
class OriginalStandardMaterialTable50LA;
class OriginalStandardMaterialObject50LA;
class OriginalStandardTextureTable50LA;
// Private native admission for a real retained Course positive PSM. Pending
// evidence is NOT a source construction receipt; only29B628 can capture one.
class OriginalCourseScreenAuthored50KG final {
    friend class rr::platform::OriginalUiCourseOwnedLifecycle50KG;
    friend class rr::platform::OriginalUiCoursePopupPayload50KG;
    friend class OriginalStandardMaterialTable50LA;
    friend class OriginalStandardMaterialObject50LA;
    OriginalCourseScreenAuthored50KG()=default;
    std::weak_ptr<const int> occurrence_;
    std::shared_ptr<const OriginalCoursePopupResourceProof50KG> popup_;
    std::int32_t row_=-1;std::size_t ordinal_=84,slot_=2;
    std::shared_ptr<const OriginalStandardMaterialSource50LA> source_;
    std::shared_ptr<const OriginalUiArchive50KG> course_;
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider_;
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<OriginalStandardTextureTable50LA> textures_;
    enum class PassKind : std::uint8_t { unsupported,uniqueTexture,staticOverlay };
    static PassKind authoredPassKind(std::size_t ordinal,std::size_t slot)noexcept {
        if(slot==0&&(ordinal==64||ordinal==65||ordinal==66||ordinal==81||ordinal==82||ordinal==83))
            return PassKind::staticOverlay;
        if(slot<2&&(ordinal==6||ordinal==7||ordinal==52||ordinal==53||ordinal==54||ordinal==55))
            return PassKind::uniqueTexture;
        return PassKind::unsupported;
    }
    static const char* authoredName(std::size_t ordinal,std::size_t slot)noexcept {
        if(authoredPassKind(ordinal,slot)==PassKind::staticOverlay)return "ui\\textures\\overlay1.psm";
        if(authoredPassKind(ordinal,slot)!=PassKind::uniqueTexture)return nullptr;
        if(ordinal==6)return slot?"ui\\textures\\arrwlefttop_d.psm":"ui\\textures\\arrwlefttop.psm";
        if(ordinal==7)return slot?"ui\\textures\\arrwrghttop_d.psm":"ui\\textures\\arrwrghttop.psm";
        if(ordinal==52||ordinal==54)return slot?"ui\\textures\\arrwleft_d.psm":"ui\\textures\\arrwleft.psm";
        if(ordinal==53||ordinal==55)return slot?"ui\\textures\\arrwrght_d.psm":"ui\\textures\\arrwrght.psm";
        return nullptr;
    }
    struct Construction {
        std::int32_t row=-1;std::size_t ordinal=84,slot=2;
        std::shared_ptr<const OriginalCoursePopupResourceProof50KG> popup;
        std::shared_ptr<const OriginalStandardMaterialSource50LA> source;
        std::shared_ptr<const OriginalUiArchive50KG> course;
        std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider;
        std::shared_ptr<const OriginalStandardMaterialContext50LA> context;
        std::shared_ptr<OriginalStandardTextureTable50LA> textures;
        OriginalUiEncodedArchiveView50KG archive;
        const OriginalUiMemberIdentity50KG* member=nullptr;
        const std::uint8_t* data=nullptr;std::size_t bytes=0,mount=0,memberOrdinal=0;
        std::uint64_t generation=0,revision=0;
        bool current(const std::string& name,std::int32_t expectedRow,
            const std::shared_ptr<const OriginalStandardMaterialContext50LA>& expectedContext,
            const std::shared_ptr<OriginalStandardTextureTable50LA>& expectedTextures)const noexcept {
            try {
                using P=OriginalStandardArchiveProvider50LA;
                const auto* expected=popup?OriginalCoursePopupResourceProof50KG::name(popup->role_):authoredName(ordinal,slot);
                if(popup&&!popup->current(name,expectedRow,expectedContext,expectedTextures,true))return false;
                if(!expected||row<0||row!=expectedRow||!P::sameName(name,expected)||
                   !source||!source->prepared()||!source->member()||!source->originalBytes()||
                   !P::sameName(*source->member(),name)||!context||context!=expectedContext||!context->live()||
                   !textures||textures!=expectedTextures||!provider||!provider->sealed()||
                   !course||!course->loaded()||course->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::course)||
                   !archive.identity()||archive.providerIdentity()!=provider.get()||!member||!data||!generation||!revision)return false;
                const auto authored=course->find(expected);const auto& original=*source->originalBytes();
                if(!authored.identity||!authored.data||authored.size!=bytes||original.size()!=bytes||
                   member->bytes!=bytes||!P::sameName(member->name,expected)||
                   std::strcmp(member->sha256,authored.identity->sha256)||
                   (bytes&&(std::memcmp(data,authored.data,bytes)||std::memcmp(data,original.data(),bytes))))return false;
                P::MemberView selected;
                if(provider->lookup132820(name,selected)!=P::Lookup::present||!selected.member||
                   !P::sameName(*selected.member,name)||selected.data!=data||selected.size!=bytes||
                   selected.mount!=mount||selected.ordinal!=memberOrdinal||selected.bankGeneration!=generation||
                   selected.revision!=provider->revision())return false;
                OriginalUiEncodedArchiveView50KG actual;
                return provider->encodedUiInput(mount,actual)&&actual.providerIdentity()==provider.get()&&
                    actual.identity()==archive.identity()&&
                    originalUiMemberIdentity50KG(actual.identity()->id,memberOrdinal)==member;
            }catch(...){return false;}
        }
    };
    bool observePending(const std::string& name,std::int32_t row,
        const std::shared_ptr<const OriginalStandardMaterialSource50LA>& actualSource,
        const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
        const std::shared_ptr<OriginalStandardTextureTable50LA>& textures,Construction& output)const noexcept {
        try {
            using P=OriginalStandardArchiveProvider50LA;
            if(occurrence_.expired()||row_!=row||row<0||!source_||!source_->prepared()||!source_->member()||
               !source_->originalBytes()||!actualSource||!actualSource->prepared()||!actualSource->member()||
               !actualSource->originalBytes()||!P::sameName(*source_->member(),*actualSource->member())||
               *source_->originalBytes()!=*actualSource->originalBytes()||!provider_)return false;
            // appendOriginalSources preserves an older identical canonical
            // source owner. Capture THAT actual29B628 source, never relabel the
            // newly prepared native copy as the source the material consumed.
            P::MemberView selected;
            if(provider_->lookup132820(name,selected)!=P::Lookup::present)return false;
            Construction next;next.popup=popup_;next.row=row_;next.ordinal=ordinal_;next.slot=slot_;
            next.source=actualSource;next.course=course_;next.provider=provider_;next.context=context_;next.textures=textures_;
            if(!provider_->encodedUiInput(selected.mount,next.archive)||!next.archive.identity())return false;
            next.member=originalUiMemberIdentity50KG(next.archive.identity()->id,selected.ordinal);
            next.data=selected.data;next.bytes=selected.size;next.mount=selected.mount;next.memberOrdinal=selected.ordinal;
            next.generation=selected.bankGeneration;next.revision=selected.revision;
            if(!next.current(name,row,context,textures))return false;
            output=std::move(next);return true;
        }catch(...){return false;}
    }
    bool current(const std::string& name)const noexcept {
        // Comparison-only native pending snapshot; no material receipt is stored.
        Construction pending;return observePending(name,row_,source_,context_,textures_,pending);
    }
    bool capture(const std::string& name,std::int32_t row,
        const std::shared_ptr<const OriginalStandardMaterialSource50LA>& source,
        const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
        const std::shared_ptr<OriginalStandardTextureTable50LA>& textures,Construction& output)const noexcept {
        // Only Object's genuine resolved-source29B628 call stores this result.
        return observePending(name,row,source,context,textures,output);
    }
public:
    OriginalCourseScreenAuthored50KG(const OriginalCourseScreenAuthored50KG&)=delete;
    OriginalCourseScreenAuthored50KG& operator=(const OriginalCourseScreenAuthored50KG&)=delete;
};
}
