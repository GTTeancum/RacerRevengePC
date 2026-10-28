namespace ra_pod_scene50LA {class Owner;}
namespace original_local_flare50LA {class Emission;}
#pragma once
namespace original_main_entry50KG {class NativeCollectorFifo50LA;}
namespace original_cable_selection50LA { class Binding; }
#include "original_racer_entity_cold50la.h"
#include "original_cable_strip_attributes50la.h"
#include "../assets/original_standard_cable_material_reference50la.h"
#include <array>
#include <memory>
#include <optional>
#include <exception>

namespace native_racer50HW { class Cables; }
namespace rr::game {
class OriginalRacerStripOwners50LA;

enum class OriginalPodCableAllocationStatus50LA : std::uint8_t {
    owner,alreadyStarted,allocation,component,stripSetupRequired,closed,topology,
    material,attributeGlobalsRequired,selectionAttachmentComplete
};
struct OriginalPodCableAllocationResult50LA {
    OriginalPodCableAllocationStatus50LA status=OriginalPodCableAllocationStatus50LA::owner;
    std::uint32_t sourcePC=0;
    // No runtime-ready token: completion is only the cold214238 attachment.
};

// Concrete23BCA0 constructor prefix, not a boolean stand-in for a strip.
// Actual Entity identity/counter and150320 fields survive partial setup.
// Material30 and native attributes are acquired only at the reached setup.
class OriginalPodCableStripCold50LA final {
    friend class ::original_local_flare50LA::Emission;
public:
    OriginalPodCableStripCold50LA()=default;
    OriginalPodCableStripCold50LA(const OriginalPodCableStripCold50LA&)=delete;
    OriginalPodCableStripCold50LA& operator=(const OriginalPodCableStripCold50LA&)=delete;
    const OriginalWreckComponent50LA& entity()const noexcept{return entity_;}
    std::uint8_t flags48()const noexcept{return flags48_;}
    // Borrowed inspection; invalidated by close, not a render-ready lease.
    const OriginalCableStripAttributes50LA& attributes()const noexcept{return attributes_;}
private:
    friend class OriginalPodCableAllocation50LA;
    friend class ::ra_pod_scene50LA::Owner; //Concrete14AB30 and1B3BB0 World strip services; no racer-owner substitution.
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    friend class ::original_cable_selection50LA::Binding;
    friend class OriginalRacerStripOwners50LA;
    OriginalWreckComponent50LA entity_;
    std::uint32_t count34_=0,color44_=0x007f7f7fu;
    std::uint8_t format3C_=0,flags48_=0;
    //40 is absent after150320; complete150450 stores its real zero argument.
    std::optional<std::uint32_t> field40_;
    std::optional<std::array<float,4>> color70_;
    struct Primitive {
        rr::assets::OriginalStandardCableMaterialReference50LA reference;
        std::uint8_t byte0C=0,byte0D=0,byte0E=0xffu,flags0F=0;
        const OriginalCableStripAttributes50LA* attributes10=nullptr;
        float scale18=1.f,scale1C=1.f;
        std::uint16_t vertices20=0,groups22=0;
    };
    std::unique_ptr<std::uint32_t[]> groups38_;
    std::unique_ptr<Primitive> primitive30_;
    OriginalCableStripAttributes50LA attributes_;
    // Resource owners survive partial setup without a renderer receipt.
};

// Source-ordered173EB0 construction and214238 cold attachment. Each
// array has independent owned backing, and source-undefined elements are not
// zero-filled and then presented as authored/source values. Completion remains
// inactive; no selection224720 or renderer receipt is produced.
class OriginalPodCableAllocation50LA final {
    friend class ::original_cable_selection50LA::Binding;
    friend class OriginalRacerStripOwners50LA;
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
public:
    using Result=OriginalPodCableAllocationResult50LA;
    using Status=OriginalPodCableAllocationStatus50LA;
    ~OriginalPodCableAllocation50LA(){if(close().status!=Status::closed)std::terminate();}
    OriginalPodCableAllocation50LA(const OriginalPodCableAllocation50LA&)=delete;
    OriginalPodCableAllocation50LA& operator=(const OriginalPodCableAllocation50LA&)=delete;
    std::uint32_t count3C()const noexcept{return count3C_;}
    std::uint32_t samples38()const noexcept{return samples38_;}
    const OriginalPodCableStripCold50LA* firstStrip()const noexcept {
        return strips30_?strips30_[0].get():nullptr;
    }
    const OriginalPodCableStripCold50LA* strip(std::uint32_t index)const noexcept {
        return strips30_&&index<ownedStrips_?strips30_[index].get():nullptr;
    }
private:
    friend class ::native_racer50HW::Cables;
    using V=std::array<float,4>;
    OriginalPodCableAllocation50LA()=default;
    std::shared_ptr<OriginalApplicationEntities50LA> application_;
    OriginalRacerEntityCold50LA* racer_=nullptr;
    OriginalWreckComponent50LA entity_;
    std::uint32_t count3C_=0,samples38_=0,ownedStrips_=0;
    std::optional<float> scale54_; // Not initialized by173CC0.
    std::unique_ptr<V[]> positions58_,positions5C_,tangents60_,tangents64_,vertices68_;
    std::unique_ptr<std::unique_ptr<OriginalPodCableStripCold50LA>[]> strips30_;
    std::unique_ptr<std::int32_t[]> enabled6C_;
    std::unique_ptr<float[]> basis70_,basis74_,basis78_,basis7C_;
    bool closed_=false;
    std::array<OriginalWreckComponent50LA*,10> priorDetached_{};
    std::size_t priorDetachedCount_=0;
    std::uint32_t fifoPins_=0; // Native borrow guard; no extra source refcount.
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    static std::unique_ptr<V[]> vectors(std::uint32_t n) {
        auto p=std::unique_ptr<V[]>(new V[n]);
        for(std::uint32_t i=0;i<n;++i)p[i]={{0.f,0.f,0.f,1.f}}; //1048E0
        return p;
    }
    Result construct(OriginalRacerEntityCold50LA& racer,
        std::shared_ptr<OriginalApplicationEntities50LA> app,std::uint32_t count,
        std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> materials,
        const std::string& directory,const OriginalWreckComponentDomain50LA& prior)noexcept {
        if(racer_||closed_)return {Status::alreadyStarted,0x214200u};
        if(!app||!app->application10()||!racer.prepared()||racer.cables254||
           racer.actualCounter423F60()!=&app->objectCount423F60()||
           racer.actualLock423F58()!=&app->attachmentLock423F58()||
           !materials||!materials->prepared()||!count||count>4||
           racer.detached7F8.parent1C||racer.detached7F8.reference00!=1u||
           prior.sessionGeneration!=racer.racer.sessionGeneration||prior.count>10u||!prior.objects)return {Status::owner,0x214120u};
        application_=std::move(app);racer_=&racer;
        priorDetachedCount_=prior.count;
        for(std::size_t i=0;i<prior.count;++i)priorDetached_[i]=prior.objects[i];
        auto r=initializeOriginalWreckComponent50LA(entity_,
            OriginalWreckComponentKind50LA::podCables173CC0,racer.racer.sessionGeneration,
            key(&entity_),true,application_->objectCount423F60());
        if(r.status!=OriginalWreckComponentStatus50LA::complete){
            racer_=nullptr;application_.reset();return {Status::component,r.sourcePC};
        }
        racer.cables254=this; //214218, before173EB0, retained even on failure.
        count3C_=count;samples38_=9;scale54_=6.5f; //173EF4..173F04
        std::uint32_t pc=0x173f14u;
        try {
            positions58_=vectors(count);pc=0x173f44u;
            positions5C_=vectors(count);pc=0x173f74u;
            tangents60_=vectors(count);pc=0x173fa4u;
            tangents64_=vectors(count);pc=0x173fd0u;
            strips30_.reset(new std::unique_ptr<OriginalPodCableStripCold50LA>[count]);
            pc=0x173fe0u;enabled6C_.reset(new std::int32_t[count]);
            pc=0x173ffcu;vertices68_=vectors(count*18u);
            for(std::uint32_t index=0;index<count;++index){
                enabled6C_[index]=1; //174044, source loop order.
                pc=0x174040u;
                auto first=std::unique_ptr<OriginalPodCableStripCold50LA>(new OriginalPodCableStripCold50LA);
                r=initializeOriginalWreckComponent50LA(first->entity_,
                    OriginalWreckComponentKind50LA::strip23BCA0,racer.racer.sessionGeneration,
                    key(&first->entity_),true,application_->objectCount423F60());
                if(r.status!=OriginalWreckComponentStatus50LA::complete)return {Status::component,r.sourcePC};
                first->flags48_=std::uint8_t((first->flags48_&~3u)|1u); //150378/390
                strips30_[index]=std::move(first);++ownedStrips_; //17406C
                auto& strip=*strips30_[index];
                strip.format3C_=7;strip.count34_=1; //150474/480
                pc=0x150498u;strip.groups38_.reset(new std::uint32_t[1]);
                strip.field40_=0; //source a9=0, stored1504A4
                //1504B4 ->23D750(-1), before any group or material setup.
                strip.color44_=0xffffffffu;strip.flags48_|=1u;
                strip.color70_=std::array<float,4>{{127.f,127.f,127.f,64.f}};
                strip.groups38_[0]=18; //1504CC..EC
                pc=0x1504f0u;strip.primitive30_.reset(new OriginalPodCableStripCold50LA::Primitive);
                rr::assets::OriginalStandardMaterialDiagnostic50LA materialDiagnostic;
                if(!strip.primitive30_->reference.acquire299BE0(materials,directory,materialDiagnostic))
                    return {Status::material,materialDiagnostic.sourcePC};
                strip.primitive30_->byte0C=7; //2465CC after successful299BE0
                strip.attributes_.construct23BEB0(pc);
                strip.primitive30_->attributes10=&strip.attributes_; //23BFD8 lifetime, native borrow
                strip.primitive30_->flags0F&=std::uint8_t(~1u);
                strip.primitive30_->groups22=1;strip.primitive30_->vertices20=18;
                //Actual retained racer3C controls this branch, not race8C/AI58.
                if(racer.cableMaterialMode3C&&
                   !strip.primitive30_->reference.bindGhostPair299B50(materialDiagnostic))
                    return {Status::material,materialDiagnostic.sourcePC};
                std::array<OriginalWreckComponent50LA*,5> objects{};objects[0]=&entity_;
                for(std::uint32_t n=0;n<ownedStrips_;++n)objects[n+1]=&strips30_[n]->entity_;
                const OriginalWreckComponentDomain50LA domain{entity_.sessionGeneration,objects.data(),ownedStrips_+1u};
                r=appendOriginalWreckComponent50LA(domain,entity_,strip.entity_,8u,
                    application_->attachmentLock423F58());
                if(r.status!=OriginalWreckComponentStatus50LA::complete)return {Status::component,r.sourcePC};
                for(std::uint32_t vertex=0;vertex<18;++vertex)
                    strip.attributes_.initializeVertex174138(*application_,vertex);
            }
            pc=0x17419cu;basis70_.reset(new float[samples38_]);
            pc=0x1741acu;basis74_.reset(new float[samples38_]);
            pc=0x1741bcu;basis78_.reset(new float[samples38_]);
            pc=0x1741ccu;basis7C_.reset(new float[samples38_]);
            for(std::uint32_t sample=0;sample<samples38_;++sample){
                //1741EC..2A0, explicit float operations in source order.
                const float denominator=float(samples38_)-1.f;
                const float t=float(sample)/denominator;
                const float textureV=10.f*t;
                const float twiceT=2.f*t;
                const float squared=t*t;
                const float shifted=twiceT-3.f;
                const float scaled=shifted*squared;
                basis70_[sample]=1.f+scaled;
                const float minusTwo=t-2.f;
                const float product=t*minusTwo;
                const float tangent=1.f+product;
                basis74_[sample]=1.f-basis70_[sample];
                basis78_[sample]=t*tangent;
                const float minusOne=t-1.f;
                basis7C_[sample]=squared*minusOne;
                for(std::uint32_t index=0;index<count;++index){
                    strips30_[index]->attributes_.setTexcoord23C800(sample*2u,0.f,textureV);
                    strips30_[index]->attributes_.setTexcoord23C800(sample*2u+1u,1.f,textureV);
                }
            }
            std::array<OriginalWreckComponent50LA*,16> objects{};
            std::size_t used=0;for(std::size_t n=0;n<prior.count;++n)objects[used++]=prior.objects[n];
            objects[used++]=&entity_;
            for(std::uint32_t n=0;n<ownedStrips_;++n)objects[used++]=&strips30_[n]->entity_;
            const OriginalWreckComponentDomain50LA domain{entity_.sessionGeneration,objects.data(),used};
            r=appendOriginalWreckComponent50LA(domain,racer.detached7F8,entity_,0x1008u,
                application_->attachmentLock423F58());
            if(r.status!=OriginalWreckComponentStatus50LA::complete)return {Status::component,r.sourcePC};
            return {Status::selectionAttachmentComplete,0x214240u};
        }catch(...){return {Status::allocation,pc};}
    }
    static bool detached(const OriginalWreckComponent50LA& e)noexcept {
        return !e.parent1C&&!e.previous10&&!e.next14&&!e.firstChild18&&e.reference00==1u;
    }
    void setEnabled174370(std::uint32_t index,std::int32_t value)noexcept {
        if(index>=count3C_)return; //17437C, before any array or strip access.
        enabled6C_[index]=value; //174394 precedes the concrete150580/1505A0.
        auto& flags=strips30_[index]->flags48_;
        flags=std::uint8_t((flags&~2u)|(value?0u:2u));
    }
    Result close()noexcept {
        if(fifoPins_)return {Status::topology,0x173d40u};
        if(closed_)return {Status::closed,0};
        if(!racer_){closed_=true;return {Status::closed,0};}
        if(!application_||racer_->cables254!=this||entity_.next14||
           entity_.reference00!=1u||application_->attachmentLock423F58())
            return {Status::topology,0x173d40u};
        const bool parentAttached=entity_.parent1C==&racer_->detached7F8;
        if(entity_.parent1C&&!parentAttached)return {Status::topology,0x142590u};
        if(!parentAttached&&entity_.previous10)return {Status::topology,0x142590u};
        if(parentAttached){
            if(racer_->detached7F8.parent1C||entity_.mask20!=0x1008u)return {Status::topology,0x142590u};
            OriginalWreckComponent50LA* previous=nullptr;std::size_t remaining=priorDetachedCount_+1u;
            for(auto* p=racer_->detached7F8.firstChild18;p;p=p->next14){
                bool known=p==&entity_;
                for(std::size_t i=0;i<priorDetachedCount_;++i)known|=p==priorDetached_[i];
                if(!remaining--||!known||p->parent1C!=&racer_->detached7F8||p->previous10!=previous)
                    return {Status::topology,0x142590u};
                previous=p;
            }
            if(previous!=&entity_)return {Status::topology,0x142590u};
        }
        OriginalWreckComponent50LA* previous=nullptr;
        auto* cursor=entity_.firstChild18;
        for(std::uint32_t n=0;n<ownedStrips_;++n){
            auto& e=strips30_[n]->entity_;
            if(e.parent1C){
                if(e.parent1C!=&entity_||cursor!=&e||e.previous10!=previous||
                   e.firstChild18||e.reference00!=1u||e.mask20!=8u)
                    return {Status::topology,0x23bcf0u};
                previous=&e;cursor=e.next14;
            }else if(n+1u!=ownedStrips_||!detached(e))
                return {Status::topology,0x23bcf0u};
        }
        if(cursor)return {Status::topology,0x142590u};
        //142590 ->1424E0 releases the sibling chain from tail to head,
        //then104970 reaches each concrete destructor. A detached final partial
        //strip is unwound first by this native owner, before attached siblings.
        for(std::uint32_t n=ownedStrips_;n>0;--n){
            auto& child=*strips30_[n-1u];
            if(child.entity_.parent1C){
                if(child.entity_.previous10)child.entity_.previous10->next14=nullptr;
                else entity_.firstChild18=nullptr;
                child.entity_.next14=nullptr;
            }
            --child.entity_.reference00; //104970, before concrete destruction
            child.attributes_.release23BCF0();
            child.groups38_.reset();child.primitive30_.reset();
            --application_->objectCount423F60(); //1413E0
            strips30_[n-1u].reset();
        }
        ownedStrips_=0;
        if(parentAttached){ //Tail removal preserves genuine binder/ring/model siblings.
            if(entity_.previous10)entity_.previous10->next14=nullptr;
            else racer_->detached7F8.firstChild18=nullptr;
        }
        --entity_.reference00; //104970 ->173D40
        enabled6C_.reset();basis70_.reset();basis74_.reset();basis78_.reset();basis7C_.reset();
        positions58_.reset();positions5C_.reset();tangents60_.reset();tangents64_.reset();
        strips30_.reset();vertices68_.reset();
        --application_->objectCount423F60(); //173E70 ->1413E0
        racer_->cables254=nullptr; //Native alias invalidation after actual release.
        racer_=nullptr;application_.reset();closed_=true;
        return {Status::closed,0x173ea4u};
    }
};
}
