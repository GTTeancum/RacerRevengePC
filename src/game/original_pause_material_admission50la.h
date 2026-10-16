#pragma once
#include "original_pause_storage50la.h"
#include <cstring>
#include <limits>
#include <vector>

namespace rr::game {
// Canonical CPU admission only. Holds real resources and the existing FINAL
// registry pin; never rebuilds/populates/seals, submits or claims GPU readiness.
class OriginalPauseMaterialAdmission50LA final {
    using Storage=OriginalPauseStorage50LA;
    using Table=assets::OriginalStandardMaterialTable50LA;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    using S=assets::OriginalStandardMaterialStatus50LA;
    std::shared_ptr<Storage> owner_;
    assets::OriginalStandardMaterialCollectorReceipt50LA seal_;
    std::uint64_t providerRevision_=0;
    bool pinned_=false;
    static bool fail(D& d,S s,std::uint32_t pc)noexcept {d.status=s;d.sourcePC=pc;return false;}
public:
    struct PassState {
        std::int16_t program=-1,group=-1;
        std::uint8_t maps=0,flags=0;
        std::array<std::int16_t,2> textureTokens{};
        std::array<std::uint8_t,2> uv{};
        std::array<std::uint8_t,4> fill{};
        std::uint32_t parameterMask=0;
        assets::OriginalStandardMaterialParameters50LA parameters;
        assets::OriginalStandardMaterialState50LA state;
        const assets::OriginalUiMaterialTemplateState50KG* constructionTemplate=nullptr;
        assets::OriginalStandardMaterialAnimation50LA* animation=nullptr;
        const assets::OriginalStandardMaterialTextureAnimation50LA* textureAnimation=nullptr;
    };
    struct Pass {
        assets::OriginalStandardMaterialCollectorEntryView50LA origin,representative;
        PassState original,final;
        std::array<assets::OriginalStandardMaterialMapBindingView50LA,2> maps;
        bool enabled=false;
    };
    struct Projection {
        std::size_t reference=0;
        std::int32_t row=-1;
        bool missingPsm=false;
        std::vector<Pass> passes; // Every declared originating pass, source order.
    };
    OriginalPauseMaterialAdmission50LA()noexcept=default;
    OriginalPauseMaterialAdmission50LA(const OriginalPauseMaterialAdmission50LA&)=delete;
    OriginalPauseMaterialAdmission50LA& operator=(const OriginalPauseMaterialAdmission50LA&)=delete;
    ~OriginalPauseMaterialAdmission50LA(){close();}
    bool current()const noexcept {
        if(!owner_||!pinned_||owner_->stage_!=Storage::Stage::derivedResources||
           !owner_->context_||!owner_->context_->live())return false;
        const auto provider=owner_->domain_->archiveProvider3E1A70();
        return provider&&provider->sealed()&&provider->revision()==providerRevision_&&
            owner_->domain_->materials382B38()->coldCollectorCurrent(seal_);
    }
    // Borrowed pointers in a Projection are valid only while this owner remains
    // open/current. Reprepare at the original emission point: texture animation
    // and effective249610 TEXA are live state, not cached GPU material receipts.
    bool prepare(std::size_t reference,Projection& output,D& d)const noexcept {
        d={};if(!current()||reference>=assets::OriginalPausePrimitiveReferences50LA::count)
            return fail(d,S::phase,0x246760);
        try {
            const auto* retained=owner_->references_.reference(reference);
            if(!retained||!retained->retained)return fail(d,S::reference,0x299BE0);
            const auto table=owner_->domain_->materials382B38();
            const assets::OriginalStandardMaterialObject50LA* object=nullptr;
            if(!table->resolve298E60(retained->material00,object,d)||!object||
               !object->passCount48()||*object->passCount48()<0||*object->passCount48()>4)
                return fail(d,S::passCount,0x246760);
            Projection next;next.reference=reference;next.row=retained->material00;
            next.missingPsm=assets::OriginalPausePrimitiveReferences50LA::source(reference).missingPsm;
            for(std::size_t p=0;p<std::size_t(*object->passCount48());++p){
                Pass pass;
                if(!table->coldCollectorPass(seal_,next.row,p,pass.origin)||pass.origin.material!=object||
                   pass.origin.finalLeaf<0||!table->coldCollectorEntry(seal_,std::size_t(pass.origin.finalLeaf),pass.representative)||
                   pass.representative.redirect18!=-1||pass.representative.registration!=pass.origin.finalLeaf||
                   !pass.representative.material||pass.representative.pass<0)
                    return fail(d,S::collector,0x24B1A0);
                assets::OriginalStandardMaterialPassView50LA original,final;
                if(!object->pass4C(p,original)||!pass.representative.material->pass4C(
                       std::size_t(pass.representative.pass),final)||
                   !copyPass(original,pass.original,d)||!copyPass(final,pass.final,d))return false;
                pass.enabled=(pass.final.flags&2u)!=0;
                for(std::size_t m=0;m<pass.final.maps;++m){
                    if(!table->prepareColdScreenPrototypeMap249610(seal_,next.row,p,m,pass.maps[m],d))return false;
                    const auto& map=pass.maps[m];
                    if(map.origin.finalLeaf!=pass.origin.finalLeaf)return fail(d,S::collector,0x24B1A0);
                    if(map.preparation==assets::OriginalStandardMaterialMapPreparation50LA::ready&&
                       !textureProvenance(map.texture,d))return false;
                }
                next.passes.push_back(std::move(pass));
            }
            if(!current())return fail(d,S::collector,0x24B1A0);
            output=std::move(next);d.status=S::complete;return true;
        }catch(...){return fail(d,S::allocation,0x246760);}
    }
    bool initialize(std::shared_ptr<Storage> owner,D& d)noexcept {
        d={};if(owner_||!owner||owner->stage_!=Storage::Stage::derivedResources||
           !owner->context_||!owner->context_->live()||owner->references_.failed()||
           owner->references_.associatedCount()!=assets::OriginalPausePrimitiveReferences50LA::count||
           owner->materialAdmissionBorrows_==std::numeric_limits<std::uint32_t>::max())
            return fail(d,S::phase,0x246760);
        const auto table=owner->domain_->materials382B38();
        const auto provider=owner->domain_->archiveProvider3E1A70();
        if(!table||!provider||!provider->sealed())return fail(d,S::source,0x132820);
        const auto seal=owner->domain_->coldCollector14D440();
        if(!table->pinNativeCollector(seal))return fail(d,S::collector,0x14D440);
        owner_=std::move(owner);seal_=seal;providerRevision_=provider->revision();pinned_=true;
        ++owner_->materialAdmissionBorrows_;
        for(std::size_t i=0;i<assets::OriginalPausePrimitiveReferences50LA::count;++i){
            Projection ignored;
            if(!materialProvenance(i,d)||!prepare(i,ignored,d)){close();return false;}
        }
        d.status=S::complete;return true;
    }
    // No GPU work is accepted by this CPU owner. Any future renderer must
    // retain this owner until its genuine retirement, then close before Pause.
    void close()noexcept {
        if(!owner_)return;
        if(!pinned_||!owner_->materialAdmissionBorrows_)std::terminate();
        owner_->domain_->materials382B38()->unpinNativeCollector();pinned_=false;
        --owner_->materialAdmissionBorrows_;owner_.reset();providerRevision_=0;seal_={};
    }
private:
    friend class OriginalPauseNativeResources50LA;
    bool serviceEmission(std::size_t reference,D& d)noexcept {
        d={};if(!current()||reference>=assets::OriginalPausePrimitiveReferences50LA::count)
            return fail(d,S::phase,0x246760);
        const auto* retained=owner_->references_.reference(reference);
        const auto table=owner_->domain_->materials382B38();
        const assets::OriginalStandardMaterialObject50LA* object=nullptr;
        if(!retained||!retained->retained||!table->resolve298E60(retained->material00,object,d)||
           !object||!object->passCount48()||*object->passCount48()<0||*object->passCount48()>4)
            return fail(d,S::reference,0x246760);
        // ScreenObject key is null: service the originating prototype before
        // resolving its FINAL representative, once per actual emitted object.
        for(std::size_t p=0;p<std::size_t(*object->passCount48());++p)
            if(!table->serviceRetainedScreenPrototype24A740(seal_,retained->material00,p,d))return false;
        return true;
    }
    bool copyPass(const assets::OriginalStandardMaterialPassView50LA& in,PassState& out,D& d)const {
        if(!in.references04||*in.references04<=0||!in.program18||!in.group22||!in.mapCount20||
           *in.mapCount20>2||!in.flags21||!in.texture1A||!in.uv1E||!in.fillRgba2C||
           !in.state30||!in.constructionTemplate||!in.parameterMask24||!in.parameterCount28||
           *in.parameterCount28<0||
           (in.parameters34?std::size_t(*in.parameterCount28)!=in.parameters34->size():*in.parameterCount28!=0))
            return fail(d,S::unsupportedPass,0x247110);
        assets::OriginalStandardProgramRowView50LA program;
        if(!owner_->domain_->programs382B70()->row(*in.program18,program)||!program.canonicalName||
           std::strcmp(program.canonicalName,"ScreenObject"))return fail(d,S::program,0x247110);
        out.program=*in.program18;out.group=*in.group22;out.maps=*in.mapCount20;out.flags=*in.flags21;
        out.textureTokens=*in.texture1A;out.uv=*in.uv1E;out.fill=*in.fillRgba2C;
        out.parameterMask=*in.parameterMask24;if(in.parameters34)out.parameters=*in.parameters34;out.state=*in.state30;
        out.constructionTemplate=in.constructionTemplate;out.animation=in.animation10;out.textureAnimation=in.textureAnimation10;
        return true;
    }
    bool textureProvenance(const assets::OriginalStandardTextureResource50LA* texture,D& d)const noexcept {
        if(!texture||!texture->sourceMember()||!texture->originalBytes()||
           !texture->sourceWidth14()||!texture->sourceHeight18()||
           *texture->sourceWidth14()<=0||*texture->sourceHeight18()<=0)return fail(d,S::texture,0x249610);
        const auto member=owner_->data_->overlays()->find(*texture->sourceMember());
        const auto* bytes=texture->originalBytes();std::size_t offset=0,size=0;
        if(!member.identity||!member.data||bytes->size()!=member.size||
           std::memcmp(bytes->data(),member.data,member.size)||
           !texture->pstExtent24E880(offset,size)||offset>bytes->size()||size>bytes->size()-offset)
            return fail(d,S::source,0x24E880);
        return true;
    }
    bool materialProvenance(std::size_t i,D& d)const noexcept {
        try {
            const auto s=assets::OriginalPausePrimitiveReferences50LA::source(i);
            const auto* ref=owner_->references_.reference(i);
            if(!ref||!ref->retained)return fail(d,S::reference,0x299BE0);
            std::string path=s.directory;path+='\\';path+=s.name;path+=".psm";
            const auto member=owner_->data_->overlays()->find(path);
            assets::OriginalStandardArchiveProvider50LA::MemberView mounted;
            const auto lookup=owner_->domain_->archiveProvider3E1A70()->lookup132820(path,mounted);
            using Lookup=assets::OriginalStandardArchiveProvider50LA::Lookup;
            const assets::OriginalStandardMaterialObject50LA* object=nullptr;
            if(!owner_->domain_->materials382B38()->resolve298E60(ref->material00,object,d)||!object)
                return fail(d,S::source,0x29B550);
            if(s.missingPsm){
                // Retained facet already supplied the narrow two-name fallback
                // proof. Recheck actual absence; never reconstruct a fake PSM.
                if(member.identity||lookup!=Lookup::absent||object->source())return fail(d,S::source,0x29B638);
            }else{
                const auto* source=object->source();const auto* bytes=source?source->originalBytes():nullptr;
                if(!member.identity||!member.data||lookup!=Lookup::present||!mounted.data||
                   mounted.size!=member.size||std::memcmp(mounted.data,member.data,member.size)||
                   !bytes||bytes->size()!=member.size||std::memcmp(bytes->data(),member.data,member.size))
                    return fail(d,S::source,0x132820);
            }
            return true;
        }catch(...){return fail(d,S::allocation,0x132820);}
    }
};
}
