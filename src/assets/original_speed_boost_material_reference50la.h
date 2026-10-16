#pragma once
#include "original_standard_material_domain50la.h"
#include <cstring>
#include <exception>
#include <string>
#include <utility>

namespace rr::game { class OriginalSpeedBoost50LA; }
namespace rr::assets {
// One authentic Speed/Boost primitive's material00. Numeric row references
// belong to the SAME Application382B38 table; shared_ptr only pins its life.
// No caller outside the concrete Speed/Boost owner can acquire through this
// facet. Population and renderer retirement remain loader/frame operations.
class OriginalSpeedBoostMaterialReference50LA final {
    friend class rr::game::OriginalSpeedBoost50LA;
    enum class Kind : std::uint8_t { screenTexture, triangle, fontBatch };
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
    // Fifteen2DF0B0 ScreenTextures; triangle18A7C0->2E20F0; two texts
    // each with two capacity36 batches. Text backing attaches before acquire;
    // triangle borrowed packet10 attaches after its2E20F0 acquisition.
    // The enclosing owner supplies actual source order and GPU retirement.
    bool acquire(Kind kind,std::uint8_t object,std::uint8_t batch,
        std::shared_ptr<OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const OriginalUiArchive50KG> overlays,
        const std::string& actualDirectory36B970,
        OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        using S=OriginalStandardMaterialStatus50LA;d={};
        if(started_)return fail(d,S::alreadyPrepared,0x299BE0);
        const bool texture=kind==Kind::screenTexture,triangle=kind==Kind::triangle;
        const bool font=kind==Kind::fontBatch;
        if((!texture&&!triangle&&!font)||(texture&&(object>=15||batch!=0))||
            (triangle&&(object!=0||batch!=0))||(font&&(object>=2||batch>=2)))
            return fail(d,S::input,0x1895B0);
        if(!domain||!domain->prepared()||!domain->application_||
            !domain->materials382B38()->screenApplicationMatches(*domain->application_)||
            !directoryMatches(actualDirectory36B970,font?"font":"ui\\textures"))
            return fail(d,S::input,font?0x2DFAB0:(triangle?0x2E20F0:0x2DF11C));
        const auto& table=domain->materials382B38();
        if(table->populationTerminal()||table->demandTerminal()||
            domain->textures383400()->populationTerminal())
            return fail(d,S::phase,0x14D370);
        if(!overlays||!overlays->loaded()||overlays->identity()!=
            originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,S::archive,0x1895B0);
        try {
            static const char* images[]={"spdbost1","boost_a1","boost_a2","boost_a3","boost_a4",
                "boost_a5","boost_a6","boost_a7","boost_a8","boost_a9"};
            const char* name=font?(object==0?"eurostileboldbolditalic26ingame":"eurostileboldbold13ingame"):
                images[triangle?9:(object<7?0:object-6)];
            const std::string path=std::string(font?"font\\":"ui\\textures\\")+name;
            const auto psm=overlays->find(path+".psm"),pst=overlays->find(path+".pst");
            if(!pst.identity||!pst.data||pst.size!=pst.identity->bytes||
                (font?(!psm.identity||!psm.data||psm.size!=psm.identity->bytes):psm.identity!=nullptr))
                return fail(d,S::source,0x1895B0);
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
            if(!mountedMatches(".pst",pst)||(font?!mountedMatches(".psm",psm):
                provider->lookup132820(path+".psm",missing)!=OriginalStandardArchiveProvider50LA::Lookup::absent))
                return fail(d,S::source,0x132820);
            // Authored fonts retain normally; exactly ten image names carry
            // authenticated missing-PSM proof into their SAME canonical rows.
            std::string requested=actualDirectory36B970;
            if(requested.back()!='\\')requested+='\\';
            requested+=name;
            domain_=std::move(domain);overlays_=std::move(overlays);
            objectOffset_=font?0x40u+0x70u*object:(triangle?0x6C0u:0x120u+0x60u*object);
            batch_=batch;started_=true;
            std::int32_t row=-1;
            const bool found=table->find298C80(requested,row,d);
            material00_=row;d.row=row;d.storedSlots=1;d.sourceEffectsCommitted=true;
            if(!found)return false;
            if(font){if(!table->retainScreen14D590(row,d))return false;}
            else if(!table->retainSpeedBoostFallback14D590(row,overlays_,domain_->textures383400(),d))return false;
            retained_=true;d.retainedSlots=1;return true;
        }catch(...){
            d.sourceEffectsCommitted=started_;d.storedSlots=started_?1:0;
            d.row=material00_;return fail(d,S::allocation,0x299BE0);
        }
    }
public:
    OriginalSpeedBoostMaterialReference50LA()noexcept=default;
    ~OriginalSpeedBoostMaterialReference50LA(){release299AC0();}
    OriginalSpeedBoostMaterialReference50LA(const OriginalSpeedBoostMaterialReference50LA&)=delete;
    OriginalSpeedBoostMaterialReference50LA& operator=(const OriginalSpeedBoostMaterialReference50LA&)=delete;
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
