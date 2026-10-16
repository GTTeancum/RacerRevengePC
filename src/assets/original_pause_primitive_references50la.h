#pragma once
#include "original_pause_source_data50la.h"
#include "original_standard_material_domain50la.h"
#include <array>
#include <cstring>
#include <exception>
#include <string>

namespace rr::assets {
// The material-reference facet of original Pause construction. It deliberately
// has no bulk "load" method: each next association must be reached by the real
// ScreenTexture/ScreenText/polygon loader AFTER its preceding resource work.
// Texture population is external, as are geometry, fonts and Entity lifecycle.
class OriginalPausePrimitiveReferences50LA final {
public:
    static constexpr std::size_t count=36;
    struct Source {
        std::uint32_t objectOffset=0,callerPC=0;
        std::uint16_t glyphCount=0;
        std::uint8_t batch=0;
        const char* name=nullptr;
        const char* directory=nullptr;
        bool missingPsm=false;
    };
    struct Reference {
        std::int32_t material00=-1;
        bool started=false,retained=false;
    };
    // Source order:3 base textures,2 polygons,base text(2),icon,
    // derived texts(26),2 arrows. Text allocation capacity is not current text
    // length: max36 glyphs per primitive, including the final partial batch.
    static Source source(std::size_t i) noexcept {
        if(i>=count)return {};
        if(i<3)return {i==0?0xA0u:i==1?0x100u:0x40u,
            i==0?0x1B8840u:i==1?0x1B898Cu:0x1B8AD8u,0,0,
            i==0?"results1":"results2","ui\\textures",true};
        if(i<5)return {i==3?0x38u:0x3Cu,0x1B8E10,0,0,"LetterBoxPause","ui\\textures",false};
        if(i==7)return {0x890,0x1BA3FC,0,0,"icon_xpause","ui\\textures",false};
        if(i>=34)return {i==34?0x210u:0x270u,i==34?0x1BACBCu:0x1BAD50u,
            0,0,i==34?"arrwleftPause":"arrwrghtPause","ui\\textures",false};
        const std::size_t batch=i<7?i-5:(i-8)%2;
        std::uint32_t offset=0x180,pc=0x1B8C08;
        std::uint16_t tail=4;
        if(i>=8&&i<10){offset=0x2D0;pc=0x1BA4D8;tail=24;}
        else if(i>=10&&i<12){offset=0x340;pc=0x1BA5F4;}
        else if(i>=12){offset=0x3B0+static_cast<std::uint32_t>((i-12)/2)*0x70;pc=0x1BA8B0;}
        return {offset,pc,static_cast<std::uint16_t>(batch?tail:36),
            static_cast<std::uint8_t>(batch),"denmarkbold16pause","font",false};
    }
    OriginalPausePrimitiveReferences50LA() noexcept=default;
    OriginalPausePrimitiveReferences50LA(const OriginalPausePrimitiveReferences50LA&)=delete;
    OriginalPausePrimitiveReferences50LA& operator=(const OriginalPausePrimitiveReferences50LA&)=delete;
    ~OriginalPausePrimitiveReferences50LA(){
        for(const auto& r:references_)if(r.retained)std::terminate();
    }
    // Actual sampled36B970 and actual application owner are required. No row
    // can be supplied by callers. Reaching all36 associations is NOT a full
    // Pause-load receipt. Callers retain a failed partial owner for cleanup.
    bool acquireNext(std::shared_ptr<OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const OriginalUiMaterialApplication50LA> application,
        std::shared_ptr<const OriginalPauseSourceData50LA> data,
        const std::string& actualDirectory36B970,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        using S=OriginalStandardMaterialStatus50LA;d={};
        const auto fail=[&](S status,std::uint32_t pc)noexcept {
            failed_=true;d.status=status;d.sourcePC=pc;return false;
        };
        if(failed_||closing_||next_>=count)return fail(S::phase,0x299BE0);
        const auto s=source(next_);
        if(!domain||!domain->prepared()||!application||!data||!data->sourceLoaded())
            return fail(S::input,s.callerPC);
        const auto table=domain->materials382B38();
        if(!table||!table->screenApplicationMatches(*application)||
           (domain_&&(domain_!=domain||application_!=application||data_!=data)))
            return fail(S::input,0x14D590);
        if(table->populationTerminal()||table->demandTerminal()||
           !domain->textures383400()||domain->textures383400()->populationTerminal())return fail(S::phase,0x14D370);
        std::size_t n=actualDirectory36B970.size();
        if(n&&actualDirectory36B970[n-1]=='\\')--n;
        if(n!=std::strlen(s.directory))return fail(S::input,0x299BE0);
        for(std::size_t i=0;i<n;++i){
            auto c=actualDirectory36B970[i];if(c>='A'&&c<='Z')c=char(c+32);
            if(c!=s.directory[i])return fail(S::input,0x299BE0);
        }
        try {
            const auto provider=domain->archiveProvider3E1A70();
            if(!provider||!provider->sealed())return fail(S::source,0x132820);
            std::string path=s.directory;path+='\\';path+=s.name;
            const auto psm=data->overlays()->find(path+".psm");
            OriginalStandardArchiveProvider50LA::MemberView mounted;
            using Lookup=OriginalStandardArchiveProvider50LA::Lookup;
            const auto lookup=provider->lookup132820(path+".psm",mounted);
            if(s.missingPsm) {
                // Exact results1/results2 source requests only. Absence must be
                // proved in the mounted provider as well as authenticated UI.
                if(psm.identity||lookup!=Lookup::absent)return fail(S::source,0x29B638);
                const auto pst=data->overlays()->find(path+".pst");
                if(!pst.identity||!pst.data||pst.size!=pst.identity->bytes||
                   provider->lookup132820(path+".pst",mounted)!=Lookup::present||
                   !mounted.data||mounted.size!=pst.size||std::memcmp(mounted.data,pst.data,pst.size))
                    return fail(S::source,0x132820);
            } else if(!psm.identity||!psm.data||psm.size!=psm.identity->bytes||
                lookup!=Lookup::present||!mounted.data||mounted.size!=psm.size||
                std::memcmp(mounted.data,psm.data,psm.size))return fail(S::source,0x132820);
            std::string requested=actualDirectory36B970;
            if(requested.back()!='\\')requested+='\\';requested+=s.name;
            domain_=std::move(domain);application_=std::move(application);data_=std::move(data);
            auto& r=references_[next_];r.started=true;
            std::int32_t row=-1;
            const bool found=table->find298C80(requested,row,d);
            r.material00=row;d.row=row;d.storedSlots=1;d.sourceEffectsCommitted=true;
            if(!found)return fail(d.status,d.sourcePC);
            if(!(s.missingPsm ? table->retainPauseBackdropFallback14D590(row,data_->overlays_,
                   domain_->textures383400(),d) : table->retainScreen14D590(row,d)))
                return fail(d.status,d.sourcePC);
            r.retained=true;d.retainedSlots=1;++next_;return true;
        } catch(...) {
            d.row=references_[next_].material00;
            d.sourceEffectsCommitted=references_[next_].started;
            d.storedSlots=references_[next_].started?1:0;
            return fail(S::allocation,0x299BE0);
        }
    }
    std::size_t associatedCount()const noexcept{return next_;}
    bool failed()const noexcept{return failed_;}
    const Reference* reference(std::size_t i)const noexcept{return i<count?&references_[i]:nullptr;}
    // Concrete enclosing destructor invokes in original primitive-release order,
    // after actual GPU retirement. No implicit destructor drain or reference leak.
    void release299AC0(std::size_t i)noexcept {
        if(i>=count)std::terminate();
        closing_=true;
        auto& r=references_[i];if(!r.retained)return;
        if(!domain_||!domain_->materials382B38()->release14D500(r.material00))std::terminate();
        r.retained=false;
    }
private:
    std::shared_ptr<OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const OriginalUiMaterialApplication50LA> application_;
    std::shared_ptr<const OriginalPauseSourceData50LA> data_;
    std::array<Reference,count> references_{};
    std::size_t next_=0;
    bool failed_=false,closing_=false;
};
}
