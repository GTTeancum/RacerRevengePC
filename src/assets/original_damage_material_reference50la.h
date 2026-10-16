#pragma once
#include "original_standard_material_domain50la.h"
#include "../game/original_damage_state50la.h"
#include <cstring>
#include <exception>
#include <string>
#include <utility>

namespace rr::game { class OriginalDamage50LA; }
namespace rr::assets {
// One authentic Damage primitive's material00. Numeric row references
// belong to the SAME Application382B38 table; shared_ptr only pins its life.
// No caller outside the concrete Damage owner can acquire through this
// facet. Population and renderer retirement remain loader/frame operations.
class OriginalDamageMaterialReference50LA final {
    friend class rr::game::OriginalDamage50LA;
    std::shared_ptr<OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const OriginalUiArchive50KG> overlays_;
    std::int32_t material00_=-1;
    std::uint32_t objectOffset_=0,sourceCallerPC_=0;
    bool started_=false,retained_=false;

    static bool fail(OriginalStandardMaterialDiagnostic50LA& d,
        OriginalStandardMaterialStatus50LA status,std::uint32_t pc)noexcept {
        d.status=status;d.sourcePC=pc;return false;
    }
    static bool directoryMatches(const std::string& actual,const char* expected)noexcept {
        std::size_t n=actual.size();if(n&&actual[n-1]=='\\')--n;
        std::size_t i=0;
        for(;i<n&&expected[i];++i){
            char c=actual[i];if(c>='A'&&c<='Z')c=char(c+('a'-'A'));
            if(c!=expected[i])return false;
        }
        return i==n&&!expected[i];
    }
    //32 distinct source ScreenTexture primitives; exact loader ordering is
    //owned by OriginalDamage50LA, not encoded as independent material rows.
    bool acquire(std::uint8_t object,
        std::shared_ptr<OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const OriginalUiArchive50KG> overlays,
        const std::string& actualDirectory36B970,
        OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        using S=OriginalStandardMaterialStatus50LA;d={};
        if(started_)return fail(d,S::alreadyPrepared,0x299BE0);
        if(object>=32)return fail(d,S::input,0x18C6F0);
        if(!domain||!domain->prepared()||!domain->application_||
            !domain->materials382B38()->screenApplicationMatches(*domain->application_)||
            !directoryMatches(actualDirectory36B970,"ui\\textures"))
            return fail(d,S::input,0x2DF11C);
        const auto& table=domain->materials382B38();
        if(table->populationTerminal()||table->demandTerminal()||
            domain->textures383400()->populationTerminal())
            return fail(d,S::phase,0x14D370);
        if(!overlays||!overlays->loaded()||overlays->identity()!=
            originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,S::archive,0x18C6F0);
        try {
            const char* name="dmgradar";
            const std::string path="ui\\textures\\dmgradar";
            const auto psm=overlays->find(path+".psm"),pst=overlays->find(path+".pst");
            if(!pst.identity||!pst.data||pst.size!=pst.identity->bytes||
                psm.identity!=nullptr)
                return fail(d,S::source,0x18C6F0);
            const auto provider=domain->archiveProvider3E1A70();
            if(!provider||!provider->sealed())return fail(d,S::source,0x132820);
            const auto mountedMatches=[&](const char* extension,const OriginalUiMemberView50KG& member){
                OriginalStandardArchiveProvider50LA::MemberView mounted;
                return provider->lookup132820(std::string(path)+extension,mounted)==
                    OriginalStandardArchiveProvider50LA::Lookup::present&&mounted.data&&
                    mounted.size==member.size&&
                    (!member.size||std::memcmp(mounted.data,member.data,member.size)==0);
            };
            OriginalStandardArchiveProvider50LA::MemberView missing;
            if(!mountedMatches(".pst",pst)||provider->lookup132820(path+".psm",missing)!=OriginalStandardArchiveProvider50LA::Lookup::absent)
                return fail(d,S::source,0x132820);
            // OVERLAYS absence is separate from actual sealed-provider absence.
            std::string requested=actualDirectory36B970;
            if(requested.back()!='\\')requested+='\\';
            requested+=name;
            domain_=std::move(domain);overlays_=std::move(overlays);
            objectOffset_=rr::game::damage_detail50LA::authored[object].offset;
            sourceCallerPC_=rr::game::damage_detail50LA::loadCallerPC[object];
            started_=true;
            std::int32_t row=-1;
            const bool found=table->find298C80(requested,row,d);
            material00_=row;d.row=row;d.storedSlots=1;d.sourceEffectsCommitted=true;
            if(!found)return false;
            if(!table->retainDamageFallback14D590(row,overlays_,domain_->textures383400(),d))return false;
            retained_=true;d.retainedSlots=1;return true;
        }catch(...){
            d.sourceEffectsCommitted=started_;d.storedSlots=started_?1:0;
            d.row=material00_;return fail(d,S::allocation,0x299BE0);
        }
    }
public:
    OriginalDamageMaterialReference50LA()noexcept=default;
    ~OriginalDamageMaterialReference50LA(){release299AC0();}
    OriginalDamageMaterialReference50LA(const OriginalDamageMaterialReference50LA&)=delete;
    OriginalDamageMaterialReference50LA& operator=(const OriginalDamageMaterialReference50LA&)=delete;
    bool retained()const noexcept{return retained_;}
    bool started()const noexcept{return started_;}
    std::int32_t material00()const noexcept{return material00_;}
    std::uint32_t objectOffset()const noexcept{return objectOffset_;}
    std::uint32_t sourceCallerPC()const noexcept{return sourceCallerPC_;}
    const OriginalStandardMaterialTable50LA* table382B38()const noexcept {
        return domain_?domain_->materials382B38().get():nullptr;
    }
    // Enclosing owner must retire all primitive submissions before release.
    // Idempotence is host cleanup bookkeeping, not repeated source teardown.
    void release299AC0()noexcept {
        if(!retained_)return;
        if(!domain_||!domain_->materials382B38()->release14D500(material00_))std::terminate();
        retained_=false;
    }
};
}
