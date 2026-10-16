#pragma once
#include "original_standard_material_domain50la.h"
#include <cstring>
#include <exception>
#include <memory>
#include <string>
namespace rr::game { class OriginalPositionStatsPrimitiveOwner50LA; }
namespace rr::assets {
// One original2461E0/299BE0 primitive reference. Only the genuine owner can
// acquire or release; a retained reference must NEVER auto-release before drain.
class OriginalRadarOverlayMaterialReference50LA final {
    friend class rr::game::OriginalPositionStatsPrimitiveOwner50LA;
    std::shared_ptr<OriginalStandardMaterialTable50LA> table_;
    std::int32_t material00_=-1;
    bool started_=false,retained_=false;
    static bool fail(OriginalStandardMaterialDiagnostic50LA& d,
        OriginalStandardMaterialStatus50LA status,std::uint32_t pc)noexcept {
        d.status=status;d.sourcePC=pc;return false;
    }
    bool acquire299BE0(const std::shared_ptr<OriginalStandardMaterialDomain50LA>& domain,
        const std::shared_ptr<const OriginalUiArchive50KG>& overlays,
        const std::string& actualDirectory36B970,OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        using S=OriginalStandardMaterialStatus50LA;d={};
        if(started_)return fail(d,S::alreadyPrepared,0x299be0);
        std::size_t n=actualDirectory36B970.size();
        if(n&&actualDirectory36B970[n-1]=='\\')--n;
        constexpr char expected[]="ui\\textures";
        if(n!=sizeof(expected)-1)return fail(d,S::input,0x299be0);
        for(std::size_t i=0;i<n;++i){
            auto c=static_cast<unsigned char>(actualDirectory36B970[i]);
            if(c>='A'&&c<='Z')c=static_cast<unsigned char>(c+32);
            if(c!=static_cast<unsigned char>(expected[i]))return fail(d,S::input,0x299be0);
        }
        if(!domain||!domain->prepared())return fail(d,S::unprepared,0x299be0);
        const auto table=domain->materials382B38();
        if(!table||table->populationTerminal()||table->demandTerminal())return fail(d,S::phase,0x14d590);
        if(!overlays||!overlays->loaded()||overlays->identity()!=
            originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))return fail(d,S::archive,0x1911f0);
        try {
            const auto psm=overlays->find("ui\\textures\\RadarOverlay.psm");
            const auto provider=domain->archiveProvider3E1A70();
            OriginalStandardArchiveProvider50LA::MemberView mounted;
            if(!psm.identity||!psm.data||psm.size!=psm.identity->bytes||!provider||!provider->sealed()||
               provider->lookup132820("ui\\textures\\RadarOverlay.psm",mounted)!=
                   OriginalStandardArchiveProvider50LA::Lookup::present||
               !mounted.data||mounted.size!=psm.size||std::memcmp(mounted.data,psm.data,psm.size))
                return fail(d,S::source,0x132820);
            std::string requested=actualDirectory36B970;
            if(requested.back()!='\\')requested+='\\';
            requested+="RadarOverlay";
            table_=table;started_=true;
            std::int32_t found=-1;
            const bool lookedUp=table_->find298C80(requested,found,d);
            material00_=found; //299CB4 store precedes retain, even on failed lookup.
            d.row=found;d.storedSlots=1;d.sourceEffectsCommitted=true;
            if(!lookedUp||!table_->retainScreen14D590(found,d))return false;
            retained_=true;d.retainedSlots=1;return true;
        }catch(...){d.sourceEffectsCommitted=started_;return fail(d,S::allocation,0x299be0);}
    }
    void release299AC0()noexcept {
        if(!retained_)return;
        if(!table_||!table_->release14D500(material00_))std::terminate();
        retained_=false;
    }
public:
    OriginalRadarOverlayMaterialReference50LA()noexcept=default;
    OriginalRadarOverlayMaterialReference50LA(const OriginalRadarOverlayMaterialReference50LA&)=delete;
    OriginalRadarOverlayMaterialReference50LA& operator=(const OriginalRadarOverlayMaterialReference50LA&)=delete;
    ~OriginalRadarOverlayMaterialReference50LA(){if(retained_)std::terminate();}
    bool retained()const noexcept{return retained_;}
    std::int32_t material00()const noexcept{return material00_;}
};
}
