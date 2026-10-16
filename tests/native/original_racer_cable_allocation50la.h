#pragma once
// Explicit inactive owner adapter. No current aggregate/runtime call site.
#include "native_racer_cables50hw_fixture.h"
#include "native_racer_model_lifetime50la_fixture.h"

namespace native_racer50HW {
inline g::OriginalPodCableAllocationResult50LA Cables::beginLateStripPrefix(
    g::OriginalRacerEntityCold50LA& racer,const native_racer_model50LA::Owner& chariot,
    std::shared_ptr<g::OriginalApplicationEntities50LA> app)noexcept {
    using S=g::OriginalPodCableAllocationStatus50LA;
    if(lateStrips_)return {S::alreadyStarted,0x213a40u};
    if(!sourceRig_||!app||!chariot.finished1538B0()||chariot.actualRig()!=sourceRig_||
       chariot.modelRole()!=Rig::chariot||chariot.modelScene()!=racer.chariotModel1B4||
       chariot.sessionGeneration()!=racer.racer.sessionGeneration||!chariot.bank_||
       chariot.racer_!=&racer||chariot.application_!=app||!chariot.materialReferences()||
       chariot.materialReferences()->table382B38()!=racer.selectionMaterialTable_.get()||
       !racer.binding_.actualMaterialDirectory36B970)return {S::owner,0x213a40u};
    try {
        // Genuine215C08/215C98 producer, from this SAME retained racer's PHY.
        // Accept only the bounded decimal domain; do not guess malformed atoi input.
        const auto ini=rig_detail::ini(sourceRig_->phy);std::uint32_t color=0x00ffffffu;
        const auto section=ini.find("binder");
        if(section!=ini.end()){
            const auto field=section->second.find("vertexcolor");
            if(field!=section->second.end()){
                const auto& text=field->second;std::size_t pos=0;std::array<std::uint32_t,3> rgb{};
                for(unsigned n=0;n<3;++n){
                    while(pos<text.size()&&(text[pos]==' '||text[pos]=='\t'))++pos;
                    if(pos==text.size()||text[pos]<'0'||text[pos]>'9')return {S::owner,0x215c48u};
                    unsigned value=0;while(pos<text.size()&&text[pos]>='0'&&text[pos]<='9'){
                        value=value*10u+unsigned(text[pos++]-'0');if(value>255u)return {S::owner,0x215c98u};}
                    rgb[n]=value;while(pos<text.size()&&(text[pos]==' '||text[pos]=='\t'||text[pos]=='\r'))++pos;
                    if(n<2){if(pos==text.size()||text[pos++]!=',')return {S::owner,0x215c40u};}
                    else if(pos!=text.size())return {S::owner,0x215c80u};
                }
                color=(rgb[0]<<16)|(rgb[1]<<8)|rgb[2];
            }
        }
        racer.color260_=color; //Reached original PHY color producer, not a default ready flag.
        const auto& id=sourceRig_->podId;
        if(id.size()!=5||id.compare(0,3,"POD")||id[3]<'0'||id[3]>'9'||id[4]<'0'||id[4]>'9')
            return {S::owner,0x213ea8u};
        const auto racer60=std::uint32_t((id[3]-'0')*10+id[4]-'0');
        const auto& binding=racer.binding_;std::size_t length=0;
        while(length<binding.materialDirectoryCapacity&&binding.actualMaterialDirectory36B970[length])++length;
        if(length==binding.materialDirectoryCapacity||length>240u)return {S::material,0x299be0u};
        const std::string directory(binding.actualMaterialDirectory36B970,length);
        lateStrips_.reset(new g::OriginalRacerStripOwners50LA);
        // Read the actual model graph only inside its owner's canonical Bank visit.
        return chariot.bank_->visit(chariot.slot_,[&](){return lateStrips_->construct(racer,std::move(app),
            chariot.graph(),racer60,directory);});
    }catch(...){return {S::allocation,0x213a40u};}
}
inline g::OriginalPodCableAllocationResult50LA Cables::beginSelectionAllocationPrefix(
    g::OriginalRacerEntityCold50LA& racer,const native_racer_model50LA::Owner& chariot,
    std::shared_ptr<g::OriginalApplicationEntities50LA> app)noexcept {
    using S=g::OriginalPodCableAllocationStatus50LA;
    if(selectionState_.allocation_)return {S::alreadyStarted,0x214200u};
    if(!lateStrips_||lateStrips_->racer_!=&racer||!lateStrips_->binderReturned_||!lateStrips_->ringReturned_)
        return {S::owner,0x214120u};
    if(!sourceRig_||!app||!app->application10()||!racer.prepared()||racer.cables254||
       selectionState_.membership50_!=0u||
       !chariot.finished1538B0()||chariot.modelRole()!=Rig::chariot||
       chariot.actualRig()!=sourceRig_||chariot.modelScene()!=racer.chariotModel1B4||
       chariot.sessionGeneration()!=racer.racer.sessionGeneration||
       racer.actualCounter423F60()!=&app->objectCount423F60()||
       racer.actualLock423F58()!=&app->attachmentLock423F58()||
       !racer.leftModel1BC||!racer.rightModel1B8||
       (sourceRig_->modelCount!=3u&&sourceRig_->modelCount!=5u)||
       bool(racer.bottomLeftModel1C4)!=(sourceRig_->modelCount==5u)||
       bool(racer.bottomRightModel1C0)!=(sourceRig_->modelCount==5u))return {S::owner,0x214120u};
    // Same retained PSG lookup as original211C08..211CBC. A second anchor on
    // either primary engine sets actual258 to2 only without that bottom model.
    // This count is independent of the existing connected mesh's cableCount.
    std::uint32_t multiplicity=1; //20F710 ->20F94C
    try {
        // Recover the canonical table pin from the retained cold binding;
        // the holder's AIDamage policy is NOT used for cable acquisition.
        const auto& binding=racer.binding_;
        if(!binding.actualAIDamageMaterial||!binding.actualMaterialDirectory36B970||
           !binding.materialDirectoryCapacity||!chariot.materialReferences())
            return {S::material,0x299be0u};
        auto materials=binding.actualAIDamageMaterial->table382B38();
        if(!materials||chariot.materialReferences()->table382B38()!=materials.get())
            return {S::material,0x299be0u};
        std::size_t length=0;
        while(length<binding.materialDirectoryCapacity&&
              binding.actualMaterialDirectory36B970[length])++length;
        if(length==binding.materialDirectoryCapacity||length>248u)
            return {S::material,0x299be0u};
        const std::string directory(binding.actualMaterialDirectory36B970,length);
        if(!racer.bottomLeftModel1C4&&sourceRig_->find(Rig::left,"enginecableleft2")!=Index::none)
            multiplicity=2; //211C4C
        if(!racer.bottomRightModel1C0&&sourceRig_->find(Rig::right,"enginecableright2")!=Index::none)
            multiplicity=2; //211CA8
        const std::uint32_t count=multiplicity+multiplicity+
            (racer.bottomLeftModel1C4?1u:0u)+(racer.bottomRightModel1C0?1u:0u);
        if(!count||count>4u)return {S::owner,0x2141c0u};
        racer.cableMultiplicity258=multiplicity;
        selectionState_.allocation_.reset(new g::OriginalPodCableAllocation50LA);
        std::array<g::OriginalWreckComponent50LA*,12> objects{};
        const auto domain=lateStrips_->detachedDomain(objects);
        return selectionState_.allocation_->construct(racer,std::move(app),count,
            std::move(materials),directory,domain);
    }catch(...){return {S::allocation,0x214200u};}
}
inline g::OriginalPodCableAllocationResult50LA Cables::closeSelectionAllocationPrefix()noexcept {
    using S=g::OriginalPodCableAllocationStatus50LA;
    if(nativeFifoPins_)return {S::topology,0x173d40u};
    if(selectionState_.allocation_){
        const auto result=selectionState_.allocation_->close();if(result.status!=S::closed)return result;
    }
    return lateStrips_?lateStrips_->close():g::OriginalPodCableAllocationResult50LA{S::closed,0};
}
}
