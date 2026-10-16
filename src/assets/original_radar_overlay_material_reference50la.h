#pragma once
#include "original_standard_material_domain50la.h"
#include <cstring>
#include <exception>
#include <memory>
#include <string>
namespace rr::game { class OriginalPositionStatsPrimitiveOwner50LA; }
namespace rr::assets {
// Borrowed canonical entries plus owned native scalar state. No map, texture,
// residency, geometry, submission or readiness receipt is represented here.
// Reacquire for each synchronous emission; the collector seal is invalidated
// by mutation and the originating primitive must remain retained throughout.
struct OriginalRadarOverlayNativePass50LA {
    OriginalStandardMaterialCollectorEntryView50LA origin{},representative{};
    OriginalStandardMaterialStateContext50LA effectiveState{};
    OriginalUiNativeUntexturedMaterial50KG policy{};
    std::int16_t program=-1;
    bool enabled=false;
};
// One original2461E0/299BE0 primitive reference. Only the genuine owner can
// acquire or release; a retained reference must NEVER auto-release before drain.
class OriginalRadarOverlayMaterialReference50LA final {
    friend class rr::game::OriginalPositionStatsPrimitiveOwner50LA;
    std::shared_ptr<OriginalStandardMaterialTable50LA> table_;
    std::shared_ptr<const OriginalStandardProgramTable50LA> programs_;
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
            table_=table;programs_=domain->programs382B70();started_=true;
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
    //24B1A0 representative selection without the inapplicable per-map249610.
    // The row cannot be supplied by the caller: it belongs to this retained
    //299BE0 reference. No native program identity comes from an enum ordinal.
    bool prepareNativeZeroMap(const OriginalStandardMaterialCollectorReceipt50LA& seal,
        OriginalRadarOverlayNativePass50LA& output,OriginalStandardMaterialDiagnostic50LA& d)const noexcept {
        using S=OriginalStandardMaterialStatus50LA;using F=OriginalUiGsField50KG;
        d={};d.row=material00_;
        if(!retained_||!table_||!programs_||!programs_->prepared())
            return fail(d,S::reference,0x299be0);
        OriginalRadarOverlayNativePass50LA next;
        if(!table_->coldCollectorPass(seal,material00_,0,next.origin)||
           next.origin.finalLeaf<0||!table_->coldCollectorEntry(seal,
               static_cast<std::size_t>(next.origin.finalLeaf),next.representative)||
           next.representative.redirect18!=-1||
           next.representative.registration!=next.origin.finalLeaf||
           !next.origin.material||!next.representative.material||next.representative.pass<0)
            return fail(d,S::collector,0x24b1a0);
        const auto* count=next.origin.material->passCount48();
        if(!count||*count!=1)return fail(d,S::passCount,0x299be0);
        OriginalStandardMaterialPassView50LA originating,pass;
        if(!next.origin.material->pass4C(0,originating)||
           !next.representative.material->pass4C(static_cast<std::size_t>(next.representative.pass),pass))
            return fail(d,S::collector,0x24b1a0);
        const auto bounded=[](const OriginalStandardMaterialPassView50LA& p)noexcept {
            return p.references04&&*p.references04>0&&p.mapCount20&&!*p.mapCount20&&
                p.flags21&&!(*p.flags21&~2u)&&p.program18&&*p.program18>=0&&
                p.state30&&p.state30->contextCount()==1&&p.constructionTemplate&&
                p.parameterCount28&&!*p.parameterCount28&&p.parameterMask24&&!*p.parameterMask24&&
                !p.animation10&&!p.textureAnimation10;
        };
        if(!bounded(originating)||!bounded(pass))return fail(d,S::unsupportedPass,0x2474b8);
        OriginalStandardProgramRowView50LA program;
        if(*originating.program18!=*pass.program18||!programs_->row(*pass.program18,program)||
           !program.canonicalName||std::strcmp(program.canonicalName,"ScreenObject"))
            return fail(d,S::program,0x247110);
        next.program=program.index;next.enabled=(*pass.flags21&2u)!=0;
        next.effectiveState=*pass.state30->context(0);
        // Disabled FINAL leaves do not prepare/submit geometry. This success
        // means skip, never a zero-map GPU-ready or null-map bind receipt.
        if(!next.enabled){output=next;return true;}
        const auto& state=next.effectiveState;
        const auto value=[&](F f)noexcept{return state.fields[static_cast<std::size_t>(f)].value;};
        const auto expect=[&](F f,std::int32_t expected)noexcept {
            if(value(f)==expected)return true;
            d.field=originalUiGsFieldName50KG(f);
            return fail(d,S::unsupportedPass,state.fields[static_cast<std::size_t>(f)].sourcePC);
        };
        // Same supported native TME=0 subset as the original UI untextured
        // renderer policy, read directly from actual ScreenMT-inherited state.
        // Never reconstruct a PSM descriptor or overwrite an inherited field.
        constexpr std::int32_t maxima[]={1,3,255,255,1,2,2,2,2,255,
            3,3,1023,1023,1023,1023,1,1,1,1,1,1,1,1,1,1,1,1,7,255,3,1,1,3,1};
        static_assert(sizeof(maxima)/sizeof(maxima[0])==originalUiGsFieldCount50KG,"GS widths");
        for(std::size_t i=0;i<originalUiGsFieldCount50KG;++i)
            if(state.fields[i].value<0||state.fields[i].value>maxima[i]){
                d.field=originalUiGsFieldName50KG(static_cast<F>(i));
                return fail(d,S::unsupportedPass,state.fields[i].sourcePC);
            }
        if(!expect(F::colorClampingMethod,1)||!expect(F::alphaCorrectionValue,0)||
           !expect(F::alphaBlendingControlInUnitsOfPixels,0)||!expect(F::gouraudShadingOnOff,1)||
           !expect(F::textureMappingOnOff,0)||!expect(F::foggingOnOff,0)||
           !expect(F::onePassAntiAliasingOnOff,0)||!expect(F::environmentContext0or1,0)||
           !expect(F::fragmentValueControlFixedOrNot,0)||!expect(F::alphaTestOnOff,0)||
           !expect(F::destinationAlphaTestOnOff,0))return false;
        auto& policy=next.policy;
        if(value(F::alphaBlendingOnOff)){
            if(!expect(F::inputColorA,0)||!expect(F::inputAlphaC,0)||!expect(F::inputColorD,1))return false;
            const auto b=value(F::inputColorB);
            if(b!=1&&b!=2){d.field=originalUiGsFieldName50KG(F::inputColorB);
                return fail(d,S::unsupportedPass,0x247a4c);}
            policy.blend=b==1?OriginalUiMaterialBlend50KG::sourceAlpha:OriginalUiMaterialBlend50KG::additive;
            policy.requiresSeparateAlphaStore=true;
        }
        policy.depthMethod=static_cast<std::uint8_t>(value(F::depthTestMethod));
        policy.depthWrite=value(F::zWriteDisabled)==0;
        policy.environmentContext=static_cast<std::uint8_t>(value(F::environmentContext0or1));
        policy.requiresOriginalDepthOwner=policy.depthMethod!=1||policy.depthWrite;
        // Raw RGB and stored alpha; only the RGB blend coefficient divides
        // packet alpha by128. All state/provenance survives beside the policy.
        if(!table_->coldCollectorCurrent(seal))return fail(d,S::collector,0x24b1a0);
        output=next;return true;
    }
    bool retained()const noexcept{return retained_;}
    std::int32_t material00()const noexcept{return material00_;}
};
}
