#pragma once
#include "original_standard_material_domain50la.h"
#include <cstring>
#include <exception>
#include <string>
#include <utility>

namespace rr::game { class OriginalTimeStats50LA; }
namespace rr::assets {
// One authentic TimeStats primitive's material00. Numeric row references
// belong to the SAME Application382B38 table; shared_ptr only pins its life.
// No caller outside the concrete TimeStats owner can acquire through this
// facet. Population and renderer retirement remain loader/frame operations.
class OriginalTimeStatsMaterialReference50LA final {
    friend class rr::game::OriginalTimeStats50LA;
    enum class Kind : std::uint8_t { statsTexture, fontBatch };
    std::shared_ptr<OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const OriginalUiArchive50KG> overlays_;
    std::int32_t material00_=-1;
    std::uint32_t objectOffset_=0;
    std::uint8_t batch_=0;
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
    // Stats:19328C and subsequent2DF0B0 calls, one primitive each.
    // Text:193A58->2DFD90->2DFAB0, two36-character primitive batches for
    // capacity40. Both source families use actual3833F0 ScreenMT.
    bool acquire(Kind kind,std::uint8_t object,std::uint8_t batch,
        std::shared_ptr<OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const OriginalUiArchive50KG> overlays,
        const std::string& actualDirectory36B970,
        OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        using S=OriginalStandardMaterialStatus50LA;d={};
        if(started_)return fail(d,S::alreadyPrepared,0x299BE0);
        const bool stats=kind==Kind::statsTexture;
        if((!stats&&kind!=Kind::fontBatch)||object>=6||(stats?batch!=0:batch>=2))
            return fail(d,S::input,0x193240);
        if(!domain||!domain->prepared()||!domain->application_||
            !domain->materials382B38()->screenApplicationMatches(*domain->application_)||
            !directoryMatches(actualDirectory36B970,stats?"ui\\textures":"font"))
            return fail(d,S::input,stats?0x2DF11C:0x2DFAB0);
        const auto& table=domain->materials382B38();
        if(table->populationTerminal()||table->demandTerminal()||
            domain->textures383400()->populationTerminal())
            return fail(d,S::phase,0x14D370);
        if(!overlays||!overlays->loaded()||overlays->identity()!=
            originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,S::archive,0x193240);
        try {
            const char* path=stats?"ui\\textures\\stats":"font\\eurostileboldbold13ingame";
            const auto psm=overlays->find(std::string(path)+".psm");
            const auto pst=overlays->find(std::string(path)+".pst");
            if(!psm.identity||!psm.data||psm.size!=psm.identity->bytes||
                !pst.identity||!pst.data||pst.size!=pst.identity->bytes)
                return fail(d,S::source,0x193240);
            const auto provider=domain->archiveProvider3E1A70();
            if(!provider||!provider->sealed())return fail(d,S::source,0x132820);
            const auto mountedMatches=[&](const char* extension,const OriginalUiMemberView50KG& member){
                OriginalStandardArchiveProvider50LA::MemberView mounted;
                return provider->lookup132820(std::string(path)+extension,mounted)==
                    OriginalStandardArchiveProvider50LA::Lookup::present&&mounted.data&&
                    mounted.size==member.size&&
                    (!member.size||std::memcmp(mounted.data,member.data,member.size)==0);
            };
            if(!mountedMatches(".psm",psm)||!mountedMatches(".pst",pst))
                return fail(d,S::source,0x132820);
            // No missing-PSM fallback and no fabricated per-object material.
            // Preserve actual path spelling;299C74 suffix removal is a no-op.
            std::string requested=actualDirectory36B970;
            if(requested.back()!='\\')requested+='\\';
            requested+=stats?"stats":"eurostileboldbold13ingame";
            domain_=std::move(domain);overlays_=std::move(overlays);
            objectOffset_=stats?0x2E0u+0x60u*object:0x40u+0x70u*object;
            batch_=batch;started_=true;
            std::int32_t row=-1;
            const bool found=table->find298C80(requested,row,d);
            material00_=row;d.row=row;d.storedSlots=1;d.sourceEffectsCommitted=true;
            if(!found)return false;
            if(!table->retainScreen14D590(row,d))return false;
            retained_=true;d.retainedSlots=1;return true;
        }catch(...){
            d.sourceEffectsCommitted=started_;d.storedSlots=started_?1:0;
            d.row=material00_;return fail(d,S::allocation,0x299BE0);
        }
    }
public:
    OriginalTimeStatsMaterialReference50LA()noexcept=default;
    ~OriginalTimeStatsMaterialReference50LA(){release299AC0();}
    OriginalTimeStatsMaterialReference50LA(const OriginalTimeStatsMaterialReference50LA&)=delete;
    OriginalTimeStatsMaterialReference50LA& operator=(const OriginalTimeStatsMaterialReference50LA&)=delete;
    bool retained()const noexcept{return retained_;}
    bool started()const noexcept{return started_;}
    std::int32_t material00()const noexcept{return material00_;}
    std::uint32_t objectOffset()const noexcept{return objectOffset_;}
    std::uint8_t batch()const noexcept{return batch_;}
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
