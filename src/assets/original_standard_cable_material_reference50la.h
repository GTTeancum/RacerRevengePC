#pragma once
namespace rr::game {class OriginalRacerStripOwners50LA;}
namespace original_cable_selection50LA { class Binding; }
#include "original_standard_material_table50la.h"
#include <array>
#include <exception>

namespace rr::assets {
// Concrete150450 ->2465B0 ->299BE0 with actual DefaultMT3833E8.
// This is one numeric reference in the canonical Application table, separate
// from the immutable decoded texture bank and from the AIDamage ScreenMT path.
class OriginalStandardCableMaterialReference50LA final {
public:
    OriginalStandardCableMaterialReference50LA()=default;
    ~OriginalStandardCableMaterialReference50LA(){release299AC0();}
    OriginalStandardCableMaterialReference50LA(const OriginalStandardCableMaterialReference50LA&)=delete;
    OriginalStandardCableMaterialReference50LA& operator=(const OriginalStandardCableMaterialReference50LA&)=delete;
    bool acquire299BE0(std::shared_ptr<OriginalStandardMaterialTable50LA> table,
        const std::string& actualDirectory36B970,OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        return acquireNamed299BE0(std::move(table),actualDirectory36B970,"cable",d);
    }
private:
    friend class rr::game::OriginalRacerStripOwners50LA;
    bool acquireNamed299BE0(std::shared_ptr<OriginalStandardMaterialTable50LA> table,
        const std::string& actualDirectory36B970,const std::string& sourceName,
        OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        using S=OriginalStandardMaterialStatus50LA;d={};
        if(started_||released_){d.status=S::alreadyPrepared;d.sourcePC=0x299be0;return false;}
        if(!table||!table->prepared()||table->populationTerminal()||table->demandTerminal()||
           actualDirectory36B970.find('\0')!=std::string::npos||actualDirectory36B970.size()>248u){
            d.status=S::input;d.sourcePC=0x299be0;return false;}
        try {
            std::string requested=actualDirectory36B970;
            if(!requested.empty()&&requested.back()!='\\')requested+='\\';
            requested+=sourceName; //Private concrete source caller, DefaultMT retained.
            const auto dot=requested.find_last_of('.');
            if(dot!=std::string::npos)requested.resize(dot); //299C74
            table_=std::move(table);started_=true;
            const bool found=table_->find298C80(requested,material00_,d);
            d.row=material00_;d.storedSlots=1;d.sourceEffectsCommitted=true;
            if(!found||!table_->retainDefault14D590(material00_,d))return false;
            retained_=true;d.retainedSlots=1;return true;
        }catch(...){d.status=S::allocation;d.sourcePC=0x299be0;return false;}
    }
public:
    //1740A4..F4: both registry lookups precede299B50. That method stores
    // handles only; it acquires NO extra numeric references for the pair.
    bool bindGhostPair299B50(OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        if(!retained_||!table_){d.status=OriginalStandardMaterialStatus50LA::phase;d.sourcePC=0x174098;return false;}
        std::int32_t ordinary=-1,ghost=-1;
        if(!table_->find298C80("textures\\cable",ordinary,d)||
           !table_->find298C80("textures\\cableGhost",ghost,d))return false;
        try {
            if(!optional04_)optional04_=std::make_unique<std::array<std::int32_t,2>>();
            *optional04_={{ordinary,ghost}};
            return true;
        }catch(...){d.status=OriginalStandardMaterialStatus50LA::allocation;
            d.sourcePC=0x299b50u;return false;}
    }
    void release299AC0()noexcept {
        if(released_)return;
        if(retained_){if(!table_||!table_->release14D500(material00_))std::terminate();retained_=false;}
        optional04_.reset();released_=true;
    }
    std::int32_t material00()const noexcept{return material00_;}
    bool retained()const noexcept{return retained_;}
private:
    friend class ::original_cable_selection50LA::Binding;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    bool inspectEmission24B9B0(const OriginalStandardMaterialTable50LA& actualTable,
        std::size_t pendingPass,OriginalCableEmissionBinding50LA& binding,
        OriginalStandardMaterialDiagnostic50LA& d) const noexcept {
        if(!retained_||released_||!table_||table_.get()!=&actualTable){
            d.status=OriginalStandardMaterialStatus50LA::reference;d.sourcePC=0x24a790u;return false;
        }
        return table_->inspectCableEmission24B9B0(material00_,pendingPass,binding,d);
    }
    OriginalCableMaterialCallbackStage50LA service23CA50(
        const OriginalStandardMaterialTable50LA& actualTable,
        OriginalStandardMaterialDiagnostic50LA& d) noexcept {
        d={};
        if(!retained_||released_||!table_||table_.get()!=&actualTable){
            d.status=OriginalStandardMaterialStatus50LA::reference;d.sourcePC=0x2467c4u;
            return OriginalCableMaterialCallbackStage50LA::blocked;
        }
        // No override, object key or row supplied by the caller. Source2467C4
        // reads THIS retained primitive handle;23CA50 supplies the null key.
        return table_->serviceCableNullKey24A620(material00_,d);
    }
    std::shared_ptr<OriginalStandardMaterialTable50LA> table_;
    std::int32_t material00_=-1;
    std::unique_ptr<std::array<std::int32_t,2>> optional04_;
    bool started_=false,retained_=false,released_=false;
};
}
