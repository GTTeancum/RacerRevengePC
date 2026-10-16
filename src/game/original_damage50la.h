#pragma once
// Detached original Damage resources and native HUD frame preparation.
// Source-derived CPU geometry is separate from actual GPU submission/completion.
// Native health admits finite nonnegative damage, including values above1.
// Source MIN bounds health locally; simulation fields are never clamped.
#include "original_damage_state50la.h"
#include "original_damage_frame50la.h"
#include "original_camera_start_effects.h"
#include "original_application_entities50la.h"
#include "original_ui_text50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include "../assets/original_damage_material_reference50la.h"
#include <cstring>
#include <exception>
#include <memory>
#include <utility>
namespace original_main_entry50KG { struct Application; }
namespace rr::game {
struct OriginalDamageDiagnostic50LA {
    enum class Error:std::uint8_t {none,phase,source,material,allocation,geometry} error=Error::none;
    std::uint32_t sourcePC=0;std::size_t object=0;
    bool sourceEffectsCommitted=false;
    assets::OriginalStandardMaterialDiagnostic50LA material;
    assets::OriginalUiMaterialDiagnostic50KG uiMaterial;
};
class OriginalDamage50LA final {
    friend struct original_main_entry50KG::Application;
    friend class OriginalDamageObservation50LA;
    using Domain=assets::OriginalStandardMaterialDomain50LA;
    using Context=assets::OriginalStandardMaterialContext50LA;
    using Archive=assets::OriginalUiArchive50KG;
    using Reference=assets::OriginalDamageMaterialReference50LA;
    using Diagnostic=OriginalDamageDiagnostic50LA;
    using Error=Diagnostic::Error;
    enum class Stage:std::uint8_t {fresh,textures,failed};
    struct EntityPrefix {
        std::shared_ptr<OriginalApplicationEntities50LA> application;
        OriginalWreckComponent50LA component;
        std::uint64_t session=0,key=0;
        bool initialized=false,releasing=false;
        EntityPrefix(std::shared_ptr<OriginalApplicationEntities50LA> owner,
            std::uint64_t actualSession)noexcept
            :application(std::move(owner)),session(actualSession),
             key(static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&component))){
            if(!application||!application->application10()||!session||!key)return;
            const auto result=initializeOriginalWreckComponent50LA(component,
                OriginalWreckComponentKind50LA::entity141300,session,key,true,
                application->objectCount423F60());
            initialized=result.status==OriginalWreckComponentStatus50LA::complete;
        }
        bool detached()const noexcept {
            return initialized&&application&&application->application10()&&
                component.kind==OriginalWreckComponentKind50LA::entity141300&&
                component.sessionGeneration==session&&component.objectKey==key&&
                component.reference00==(releasing?0u:1u)&&
                !component.previous10&&!component.next14&&!component.parent1C&&
                !component.firstChild18&&!component.mask20&&!component.descendants24&&
                component.category28==0xffu&&(component.flags29&15u)==3u;
        }
        void beginRelease104970()noexcept {
            if(!initialized)return;
            if(releasing||!detached())std::terminate();
            --component.reference00;releasing=true;
        }
        ~EntityPrefix(){
            if(!initialized)return;
            // Constructor unwinding also releases a genuine constructed base.
            if(!releasing)beginRelease104970();
            if(!detached())std::terminate();
            --application->objectCount423F60(); //18C6B4->14141C exactly once
        }
    } entity_;
    std::shared_ptr<Domain> domain_;
    std::shared_ptr<const Context> context_;
    std::shared_ptr<const Archive> overlays_;
    struct Quad {
        std::array<OriginalUiTextVertex50KG,4> vertices{};
        std::array<std::array<std::uint8_t,4>,4> colors{};
        float depth=0;
    };
    struct Primitive {Reference material;Quad* geometry10=nullptr;};
    struct TextureResource {
        std::unique_ptr<Primitive> primitive54;
        std::unique_ptr<Quad> storage58;
        const assets::OriginalStandardTextureResource50LA* atlas=nullptr;
        void release()noexcept {
            //2DEFE0: buffer58, clear primitive10, destroy primitive/material.
            storage58.reset();if(primitive54)primitive54->geometry10=nullptr;
            primitive54.reset();atlas=nullptr;
        }
        ~TextureResource(){release();}
    };
    //32 distinct fixed-address source objects exist before any load begins.
    damage_detail50LA::State state_;
    std::array<TextureResource,32> textures_{};
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> pixels_;
    assets::OriginalUiMaterial50KG material_;
    assets::OriginalStandardMaterialCollectorReceipt50LA collector_;
    const assets::OriginalStandardTextureResource50LA* atlas_=nullptr;
    std::int16_t demandToken_=-1;
    std::int32_t leaf_=-1;
    std::uint32_t videoDepth60_=0;
    std::uint64_t verifiedProviderRevision_=0;
    Stage stage_=Stage::fresh;
    bool effects_=false,cpuPrepared_=false;
    Diagnostic terminal_;
    OriginalDamageNativeDraw50LA nativeDraw_;
    OriginalDamage50LA(std::shared_ptr<OriginalApplicationEntities50LA> entities,
        std::uint64_t generation,std::shared_ptr<Domain> domain,
        std::shared_ptr<const Context> context,std::shared_ptr<const Archive> overlays)
        :entity_(std::move(entities),generation),domain_(std::move(domain)),
         context_(std::move(context)),overlays_(std::move(overlays)){
        effects_=entity_.initialized;
        if(!entity_.initialized){terminal_.error=Error::source;terminal_.sourcePC=0x18C55C;stage_=Stage::failed;}
    }
    bool stop(Diagnostic& d,Error error,std::uint32_t pc)noexcept {
        cpuPrepared_=false;d.error=error;d.sourcePC=pc;
        d.sourceEffectsCommitted=effects_||d.material.sourceEffectsCommitted;
        terminal_=d;stage_=Stage::failed;return false;
    }
    bool enter(Stage expected,Diagnostic& d)const noexcept {
        d={};d.sourceEffectsCommitted=effects_;
        if(stage_==Stage::failed){d=terminal_;return false;}
        if(stage_!=expected||!entity_.detached()||entity_.releasing||!domain_||!domain_->prepared()||
           !context_||!context_->live()||context_->materialApplication()!=domain_->application_){
            d.error=Error::phase;d.sourcePC=0x18C6F0;return false;}
        return true;
    }
    static bool bytesMatch(const std::vector<std::uint8_t>* bytes,const assets::OriginalUiMemberView50KG& m)noexcept {
        return bytes&&m.identity&&m.data&&m.size==m.identity->bytes&&bytes->size()==m.size&&
            (!m.size||!std::memcmp(bytes->data(),m.data,m.size));
    }
    bool providerCurrent()const {
        const auto provider=domain_->archiveProvider3E1A70();
        if(!provider||!provider->sealed())return false;
        const auto pst=overlays_->find("ui\\textures\\dmgradar.pst");
        assets::OriginalStandardArchiveProvider50LA::MemberView found,missing;
        return provider->lookup132820("ui\\textures\\dmgradar.psm",missing)==
                assets::OriginalStandardArchiveProvider50LA::Lookup::absent&&
            provider->lookup132820("ui\\textures\\dmgradar.pst",found)==
                assets::OriginalStandardArchiveProvider50LA::Lookup::present&&
            found.data&&pst.data&&found.size==pst.size&&
            (!pst.size||!std::memcmp(found.data,pst.data,pst.size));
    }
    bool resolved(std::size_t index,const assets::OriginalStandardTextureResource50LA*& atlas,Diagnostic& d)const {
        if(index>=32||!textures_[index].primitive54)return false;
        const auto& reference=textures_[index].primitive54->material;
        const auto pst=overlays_->find("ui\\textures\\dmgradar.pst");
        const auto* screen=domain_->application_->templates().get(assets::OriginalUiTemplate50KG::screenMt);
        assets::OriginalStandardMaterialRowView50LA row;
        const assets::OriginalStandardMaterialObject50LA* m=nullptr;
        if(!reference.retained()||reference.objectOffset()!=damage_detail50LA::authored[index].offset||
           reference.sourceCallerPC()!=damage_detail50LA::loadCallerPC[index]||
           reference.table382B38()!=domain_->materials382B38().get()||!screen||
           !domain_->materials382B38()->row(reference.material00(),row)||!row.references08||
           *row.references08<=0||row.associated30!=screen||
           !domain_->materials382B38()->resolve298E60(reference.material00(),m,d.material)||
           !m||!m->constructed29B550()||m->source()||m->textures383400()!=domain_->textures383400().get()||
           !m->passCount48()||*m->passCount48()!=1)return false;
        const auto* proof=m->damageFallbackProvenance();
        if(!proof||proof->origin!=assets::OriginalUiMaterialOrigin50KG::damageTextureMissingPsmFallback||
           proof->archive!=overlays_->identity()||proof->fallbackTexture!=pst.identity||
           proof->registeredName!="ui\\textures\\dmgradar"||
           proof->attemptedPsmMember!="ui\\textures\\dmgradar.psm"||
           proof->sourceCallerPC!=0x18C748||proof->creationPC!=0x2475C0||
           proof->fallbackPageSource||proof->fallbackWidgetSource||
           overlays_->find("ui\\textures\\dmgradar.psm").identity)return false;
        assets::OriginalStandardMaterialPassView50LA pass;
        if(!m->pass4C(0,pass)||pass.constructionTemplate!=screen||!pass.group22||*pass.group22!=-1||
           !pass.mapCount20||*pass.mapCount20!=1||!pass.texture1A||(*pass.texture1A)[0]<0||
           pass.animation10||pass.textureAnimation10)return false;
        assets::OriginalStandardTextureRowView50LA texture;
        if(!domain_->textures383400()->row((*pass.texture1A)[0],texture)||!texture.object04||
           !texture.references08||*texture.references08<=0||texture.object04->animated102070()||
           !bytesMatch(texture.object04->originalBytes(),pst))return false;
        atlas=texture.object04;return true;
    }
    bool stageTextures18C6F0(const std::string& directory,std::uint32_t depth,Diagnostic& d)noexcept {
        if(!enter(Stage::fresh,d))return false;
        if(depth!=16777215u)return stop(d,Error::source,0x2DF384);
        try {
            if(!overlays_||!overlays_->loaded()||overlays_->identity()!=
               assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::overlays)||
               !providerCurrent())return stop(d,Error::source,0x18C6F0);
            videoDepth60_=depth;
            for(auto i:damage_detail50LA::loadOrder){
                d.object=i;auto& t=textures_[i];
                t.primitive54=std::make_unique<Primitive>();
                if(!t.primitive54->material.acquire(i,domain_,overlays_,directory,d.material))
                    return stop(d,Error::material,0x2DF11C);
                effects_=true;
                if(!domain_->populateDamageResources2DF144(context_,d.material))
                    return stop(d,Error::material,0x2DF144);
                if(!resolved(i,t.atlas,d)||!t.atlas->sourceWidth14()||!t.atlas->sourceHeight18())
                    return stop(d,Error::source,0x2DF180);
                t.storage58=std::make_unique<Quad>();t.primitive54->geometry10=t.storage58.get();
                t.storage58->depth=damage_detail50LA::sub(static_cast<float>(depth),1.f);
                for(auto& c:t.storage58->colors)c={{127,127,127,63}};
                if(!state_.initializeTexture(i,*t.atlas->sourceWidth14(),*t.atlas->sourceHeight18()))
                    return stop(d,Error::geometry,damage_detail50LA::loadCallerPC[i]);
                const auto& v=state_.textures[i];auto& q=t.storage58->vertices;
                // Source strip1,0,2,3 preserves its diagonal in the native fan.
                q[0].u=q[3].u=v.u1;q[1].u=q[2].u=v.u0;
                q[0].v=q[1].v=v.v0;q[2].v=q[3].v=v.v1;
            }
            //18EAA0 layout requires actual race mode/view binding. Cold resources
            //alone are not completion of the original full loader or visible HUD.
            stage_=Stage::textures;d.sourceEffectsCommitted=effects_;return true;
        }catch(...){return stop(d,Error::allocation,0x2DF1CC);}
    }
    bool finishNativeColdPreparation50LA(std::shared_ptr<const assets::OriginalUiScreenAssets50KG> pixels,
        std::uint32_t image,Diagnostic& d)noexcept {
        if(!enter(Stage::textures,d))return false;
        if(cpuPrepared_||image!=0||!pixels||!pixels->loaded()||!pixels->textures()||
           pixels->textures()->textureCount()!=1)return stop(d,Error::source,0x1A7B14);
        try {
            const auto provider=domain_->archiveProvider3E1A70();
            if(!provider||!providerCurrent())return stop(d,Error::source,0x132820);
            const auto providerRevision=provider->revision();
            const auto table=domain_->materials382B38();
            collector_=domain_->coldCollector14D440();
            if(!table->coldCollectorCurrent(collector_))return stop(d,Error::material,0x1A7B14);
            for(std::size_t i=0;i<32;++i){
                d.object=i;const auto& t=textures_[i];
                const assets::OriginalStandardTextureResource50LA* atlas=nullptr;
                if(!t.storage58||!t.primitive54||t.primitive54->geometry10!=t.storage58.get()||
                   !resolved(i,atlas,d)||atlas!=t.atlas||(atlas_&&atlas_!=atlas))
                    return stop(d,Error::source,0x1AA5A0);
                atlas_=atlas;
            }
            const auto pst=overlays_->find("ui\\textures\\dmgradar.pst");
            const auto* provenance=pixels->sourceProvenance(image);
            const auto* texture=pixels->textures()->texture(image);
            if(!provenance||!texture||!pixels->decodedTexture(image)||!atlas_||
               provenance->memberIdentity!=pst.identity||provenance->pstOffset||
               provenance->pstBytes!=pst.size||provenance->celIdentity||
               !atlas_->sourceWidth14()||!atlas_->sourceHeight18()||
               *atlas_->sourceWidth14()<=0||*atlas_->sourceHeight18()<=0||
               texture->texture.width!=static_cast<std::uint32_t>(*atlas_->sourceWidth14())||
               texture->texture.height!=static_cast<std::uint32_t>(*atlas_->sourceHeight18()))
                return stop(d,Error::source,0x2DF180);
            const auto& reference=textures_[12].primitive54->material; //actual first load18C748
            assets::OriginalStandardMaterialMapBindingView50LA map;
            if(!table->prepareColdScreenPrototypeMap249610(collector_,reference.material00(),0,0,map,d.material)||
               map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||
               map.texture!=atlas_||map.origin.finalLeaf<0)return stop(d,Error::material,0x249610);
            const assets::OriginalStandardMaterialObject50LA* material=nullptr;
            assets::OriginalStandardMaterialPassView50LA pass;
            if(!table->resolve298E60(reference.material00(),material,d.material)||!material||
               !material->pass4C(0,pass)||!pass.state30||pass.state30->contextCount()!=1||
               !assets::readOriginalDamageFallbackMaterial50LA(*overlays_,domain_->application_->templates(),
                   assets::OriginalUiTemplate50KG::screenMt,context_->materialRuntime(),material_,d.uiMaterial)||
               material_.passes.size()!=1)return stop(d,Error::material,0x247110);
            const auto& policy=material_.passes[0];
            for(std::size_t f=0;f<assets::originalUiGsFieldCount50KG;++f)
                if(policy.gs.fields[f].value!=pass.state30->fields[f].value||
                   policy.gs.fields[f].value!=map.contextAfter249610.fields[f].value)
                    return stop(d,Error::material,0x2476A0);
            const std::int32_t sampler[]={policy.lodCalculationMethod,policy.maximumMipLevel,
                policy.magFilter,policy.minFilter,policy.lodL,policy.lodK};
            for(std::size_t f=0;f<6;++f)if(sampler[f]!=pass.state30->sampler[f].value)
                return stop(d,Error::material,0x247F38);
            assets::OriginalStandardTextureResource50LA* demanded=nullptr;
            if(!domain_->textures383400()->find("ui\\textures\\dmgradar",demandToken_)||
               !domain_->textures383400()->resolve298E60(demandToken_,demanded,d.material.texture)||
               demanded!=atlas_)return stop(d,Error::source,0x18F470);
            // The collector does not version archive mounts. Publish readiness
            // only for the same sealed provider revision verified above.
            if(!provider->sealed()||provider->revision()!=providerRevision)
                return stop(d,Error::source,0x132820);
            verifiedProviderRevision_=providerRevision;
            leaf_=map.origin.finalLeaf;pixels_=std::move(pixels);cpuPrepared_=true;
            d.sourceEffectsCommitted=effects_;return true;
        }catch(...){return stop(d,Error::allocation,0x1A7B14);}
    }
    //Native eligible-display preparation explicitly invokes this source leaf.
    //Resolve+50's actual lease; finalization alone never implies demand.
    bool demand18F470(Diagnostic& d)noexcept {
        if(!nativeCpuResourcesPrepared()){d.error=Error::phase;d.sourcePC=0x18F470;return false;}
        try {
            const assets::OriginalStandardTextureResource50LA* expected=nullptr;
            assets::OriginalStandardTextureResource50LA* resource=nullptr;
            if(!providerCurrent()||!resolved(0,expected,d)||expected!=atlas_||
               !domain_->textures383400()->resolve298E60(demandToken_,resource,d.material.texture)||
               resource!=expected)return stop(d,Error::source,0x2DF8A0);
            bool written=false;
            if(!resource->demandDamage18F470(written,d.material.texture))
                return stop(d,Error::material,0x18F4A8);
            //Detached Entity has no children. Full parent traversal is unjoined.
            d.sourceEffectsCommitted=effects_||written;return true;
        }catch(...){return stop(d,Error::allocation,0x18F470);}
    }
public:
    const assets::OriginalUiScreenAssets50KG* nativePixels()const noexcept{return pixels_.get();}
    const assets::OriginalUiMaterial50KG* nativeMaterial(std::size_t i)const noexcept {
        return i==0&&nativeCpuResourcesPrepared()?&material_:nullptr;
    }
    std::uint32_t nativeImage(std::size_t)const noexcept{return 0;}
    const OriginalDamageNativeDraw50LA& nativeDraw()const noexcept{return nativeDraw_;}
    void invalidateNativeDraw()noexcept{nativeDraw_.prepared=false;}
    // Explicit native ALL8 presentation adaptation. The proximity observation
    // comes from the original scan in a separate HUD owner, not AI extension
    // storage. The original health/marker leaf is complete, never health-only.
    bool prepareNativeDisplay(const OriginalDamageNativeFrame50LA& frame,Diagnostic& d)noexcept {
        d={};nativeDraw_.prepared=false;nativeDraw_.quadCount=nativeDraw_.packetCount=0;
        if(!nativeCpuResourcesPrepared()||!frame.viewport||frame.viewIndex||frame.raceMode8C!=0||
           (frame.raceFlags1B0&3u)!=1u||frame.sessionGeneration!=entity_.session||
           !frame.cameraPublication||frame.cameraPublication<nativeDraw_.cameraPublication||
           nativeDraw_.serial==std::numeric_limits<std::uint64_t>::max())return stop(d,Error::phase,0x18f4d0);
        const auto& view=*frame.viewport;
        if(!view.prepared||!view.matricesReady||!view.sceneKey||view.sessionGeneration!=frame.sessionGeneration||
           view.x1E0||view.y1E4||view.width1E8!=640||view.height1EC!=448||
           (nativeDraw_.viewport&&nativeDraw_.viewport!=&view))return stop(d,Error::source,0x18ead0);
        nativeDraw_.viewport=&view;nativeDraw_.cameraPublication=frame.cameraPublication;++nativeDraw_.serial;
        state_.raceModeC58=frame.raceMode8C;
        if(!state_.predraw18F4D0(frame.viewIndex,frame.damage7D4,frame.damage7DC,frame.damage7D0,frame.damage7D8))
            return stop(d,Error::geometry,0x18f4d0);
        if((frame.raceFlags1B0&16u)||(frame.raceEntityFlags29&4u)||(frame.uiFlags1E4&1u)){
            nativeDraw_.prepared=true;return true;
        }
        assets::OriginalStandardMaterialMapBindingView50LA map;
        if(!domain_->materials382B38()->prepareColdScreenPrototypeMap249610(collector_,
               textures_[0].primitive54->material.material00(),0,0,map,d.material)||
           map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||
           map.texture!=atlas_||map.origin.finalLeaf!=leaf_)return stop(d,Error::material,0x249610);
        for(std::size_t f=0;f<assets::originalUiGsFieldCount50KG;++f)
            if(map.contextAfter249610.fields[f].value!=material_.passes[0].gs.fields[f].value)
                return stop(d,Error::material,0x249610);
        if(!demand18F470(d))return false;
        if(frame.viewVehicleState40){nativeDraw_.prepared=true;return true;}
        damage_detail50LA::State::DrawSequence sequence;
        if(!state_.leaf18F680(frame.viewIndex,frame.flagsB9C,frame.proximity.nearby,sequence))
            return stop(d,Error::geometry,0x18f680);
        for(std::size_t p=0;p<sequence.count;++p){const auto index=sequence.textureIndices[p];
            d.object=index;auto& texture=state_.textures[index];auto& storage=*textures_[index].storage58;
            if(texture.rotation!=0||!std::isfinite(texture.x)||!std::isfinite(texture.y)||
               !std::isfinite(texture.width)||!std::isfinite(texture.height))return stop(d,Error::geometry,0x2df694);
            if(texture.dirty&1u){
                const float right=texture.x+texture.width,bottom=texture.y+texture.height;
                if(!std::isfinite(right)||!std::isfinite(bottom))return stop(d,Error::geometry,0x2df6c0);
                storage.vertices[0].x=storage.vertices[3].x=right;
                storage.vertices[1].x=storage.vertices[2].x=texture.x;
                storage.vertices[0].y=storage.vertices[1].y=texture.y;
                storage.vertices[2].y=storage.vertices[3].y=bottom;texture.dirty=std::uint8_t(texture.dirty&~1u);
            }
            for(auto& color:storage.colors)color=texture.packetColor;
            if(nativeDraw_.quadCount==nativeDraw_.maximumQuads||nativeDraw_.packetCount==nativeDraw_.maximumPackets)
                return stop(d,Error::geometry,0x2df550);
            auto& quad=nativeDraw_.quads[nativeDraw_.quadCount];
            quad.vertices=storage.vertices;quad.packetColors=storage.colors;quad.depth=storage.depth;
            nativeDraw_.packets[nativeDraw_.packetCount++]={nativeDraw_.quadCount++,1,0,leaf_};
        }
        nativeDraw_.prepared=true;return true;
    }
    OriginalDamage50LA(const OriginalDamage50LA&)=delete;
    OriginalDamage50LA& operator=(const OriginalDamage50LA&)=delete;
    OriginalDamage50LA(OriginalDamage50LA&&)=delete;
    OriginalDamage50LA& operator=(OriginalDamage50LA&&)=delete;
    ~OriginalDamage50LA(){
        entity_.beginRelease104970();
        //18C640: B10,A10,base12 reversed; not reverse LOAD order.
        //Future GPU users MUST strongly retain owner until actual retirement.
        for(std::size_t i=32;i>22;--i)textures_[i-1].release();
        for(std::size_t i=22;i>12;--i)textures_[i-1].release();
        for(std::size_t i=12;i>0;--i)textures_[i-1].release();
    }
    bool nativeCpuResourcesPrepared()const noexcept {
        if(!(cpuPrepared_&&stage_==Stage::textures&&entity_.detached()&&!entity_.releasing&&
            context_&&context_->live()&&domain_&&domain_->materials382B38()->coldCollectorCurrent(collector_)))
            return false;
        // Allocation-free observation; a later mount invalidates readiness even
        // when the material collector is unchanged or the provider is resealed.
        const auto& provider=domain_->archiveProvider3E1A70();
        return provider&&provider->sealed()&&provider->revision()==verifiedProviderRevision_;
    }
};
} //namespace rr::game
