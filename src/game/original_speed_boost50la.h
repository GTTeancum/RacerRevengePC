#pragma once
// Detached, native-host-scheduled original Speed/Boost resources and CPU draw
// preparation. Native renderer retirement is separate; no retail ancestry claim.
#include "original_speed_boost_state50la.h"
#include "original_speed_boost_frame50la.h"
#include "original_application_entities50la.h"
#include "original_ui_text50kg.h"
#include "../assets/original_application_fonts50la.h"
#include "../assets/original_ui_localization50kg.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include "../assets/original_speed_boost_material_reference50la.h"
#include <utility>
#include <limits>

namespace original_main_entry50KG { struct Application; }
namespace rr::game {
struct OriginalSpeedBoostNativePrimitive50LA {
    std::array<OriginalUiTextVertex50KG,4> vertices{};
    std::array<std::array<std::uint8_t,4>,4> colors{};
    float depth=0;
    std::uint32_t vertexCount=4;
};
struct OriginalSpeedBoostNativePacket50LA {
    std::uint32_t first=0,count=0,material=0;
    std::int32_t leaf=-1;
    std::uint32_t vertexCount=4;
};
struct OriginalSpeedBoostNativeDraw50LA {
    static constexpr std::size_t maximumPrimitives=95,maximumPackets=19;
    std::array<OriginalSpeedBoostNativePrimitive50LA,maximumPrimitives> primitives{};
    std::array<OriginalSpeedBoostNativePacket50LA,maximumPackets> packets{};
    std::uint32_t primitiveCount=0,packetCount=0;
    std::uint64_t serial=0,cameraPublication=0;
    const OriginalCameraViewportFields* viewport=nullptr;
    bool prepared=false; //CPU only, never a completed GPU/display receipt.
};
struct OriginalSpeedBoostDiagnostic50LA {
    enum class Error:std::uint8_t {none,phase,source,material,font,allocation,geometry} error=Error::none;
    std::uint32_t sourcePC=0;
    std::size_t object=0,batch=0;
    bool sourceEffectsCommitted=false;
    assets::OriginalStandardMaterialDiagnostic50LA material;
    assets::OriginalUiMaterialDiagnostic50KG uiMaterial;
    OriginalUiTextDiagnostic50KG text;
};
class OriginalSpeedBoost50LA final {
    friend struct original_main_entry50KG::Application;
    using Domain=assets::OriginalStandardMaterialDomain50LA;
    using Context=assets::OriginalStandardMaterialContext50LA;
    using Archive=assets::OriginalUiArchive50KG;
    using Fonts=assets::OriginalApplicationFonts50LA;
    using Reference=assets::OriginalSpeedBoostMaterialReference50LA;
    using Diagnostic=OriginalSpeedBoostDiagnostic50LA;
    using Error=Diagnostic::Error;
    enum class Stage:std::uint8_t {fresh,textures,triangle,fonts,texts,failed};
    // Declared first: derived leases/storage die before actual Entity count.
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
            --application->objectCount423F60(); //189570->14141C exactly once
        }
    } entity_;
    std::shared_ptr<Domain> domain_;
    std::shared_ptr<const Context> context_;
    std::shared_ptr<const Archive> overlays_,localization_;
    std::shared_ptr<const assets::OriginalUiEnglish50KG> english_;
    std::array<std::shared_ptr<const Fonts::Entry>,2> fonts_{};
    using Quad=OriginalSpeedBoostNativePrimitive50LA;
    struct TextureResource {
        Reference material;
        const assets::OriginalStandardTextureResource50LA* atlas=nullptr;
        std::unique_ptr<Quad> storage58;
    };
    struct TextBatch {
        Reference material;
        Quad* geometry10=nullptr;
        std::size_t firstGlyph=0,capacity=0,activeGlyphs=0;
    };
    struct TextResource {
        speed_boost_detail50LA::Text* state=nullptr;
        std::array<Quad,40> glyphsC{};
        std::array<std::unique_ptr<TextBatch>,2> primitives4{};
        float positionedX20=0,positionedY24=0;
        std::size_t activeBatches5E=0;
        ~TextResource(){for(auto& p:primitives4){if(p)p->geometry10=nullptr;p.reset();}}
    };
    struct TrianglePrimitive {Reference material;std::array<speed_boost_detail50LA::Vertex,3>* geometry10=nullptr;};
    struct TriangleResource {
        // ONE actual native numeric owner, state_.triangle; these are borrows.
        // Fixed host storage reserves capacity before retail heap allocation,
        // but primitive attachment and material effects retain source order.
        speed_boost_detail50LA::Triangle* vertices00=nullptr;
        std::array<speed_boost_detail50LA::Vertex,3>* packet44=nullptr;
        std::unique_ptr<TrianglePrimitive> primitive30;
        std::array<std::array<std::uint8_t,4>,3> colors{};
        ~TriangleResource(){
            //2E1980 requires renderer drain before reaching this destruction.
            //The renderer must strongly retain the owner until real retirement; no bool
            //parameter may certify that work is absent or retired.
            if(primitive30)primitive30->geometry10=nullptr;
            primitive30.reset();packet44=nullptr;vertices00=nullptr;
        }
    };
    speed_boost_detail50LA::State state_;
    std::array<std::unique_ptr<TextureResource>,15> textures_{};
    std::unique_ptr<TriangleResource> triangle_;
    std::array<std::unique_ptr<TextResource>,2> texts_{};
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> pixels_;
    std::array<std::uint32_t,12> images_{};
    std::array<const assets::OriginalStandardTextureResource50LA*,12> atlases_{};
    std::array<std::int16_t,12> demandTokens_{};
    std::array<assets::OriginalUiMaterial50KG,12> materials_{};
    std::array<std::int32_t,12> leaves_{};
    assets::OriginalStandardMaterialCollectorReceipt50LA collector_;
    std::uint32_t videoDepth60_=0;
    Stage stage_=Stage::fresh;
    bool effects_=false,cpuPrepared_=false;
    std::uint64_t verifiedProviderRevision_=0;
    Diagnostic terminal_;
    OriginalSpeedBoostNativeDraw50LA nativeDraw_;
    const OriginalCameraViewportFields* nativeViewport_=nullptr;
    std::uint64_t nativeScene_=0;
    static const char* imageName(std::size_t i)noexcept {
        static const char* names[]={"spdbost1","boost_a1","boost_a2","boost_a3","boost_a4","boost_a5",
            "boost_a6","boost_a7","boost_a8","boost_a9","eurostileboldbolditalic26ingame","eurostileboldbold13ingame"};
        return i<12?names[i]:nullptr;
    }
    static std::string imageBase(std::size_t i){return std::string(i<10?"ui\\textures\\":"font\\")+imageName(i);}
    static std::size_t textureImage(std::size_t object)noexcept{return object<7?0:object-6;}
    OriginalSpeedBoost50LA(std::shared_ptr<OriginalApplicationEntities50LA> entities,
        std::uint64_t actualSessionGeneration,std::shared_ptr<Domain> domain,
        std::shared_ptr<const Context> context,std::shared_ptr<const Archive> overlays,
        std::shared_ptr<const Archive> localization,std::shared_ptr<const assets::OriginalUiEnglish50KG> english)
        :entity_(std::move(entities),actualSessionGeneration),domain_(std::move(domain)),
         context_(std::move(context)),overlays_(std::move(overlays)),localization_(std::move(localization)),
         english_(std::move(english)){
        demandTokens_.fill(-1);leaves_.fill(-1);effects_=entity_.initialized;
        if(!entity_.initialized){terminal_.error=Error::source;terminal_.sourcePC=0x18945C;stage_=Stage::failed;}
    }
    bool stop(Diagnostic& d,Error error,std::uint32_t pc)noexcept {
        cpuPrepared_=false;nativeDraw_.prepared=false;d.error=error;d.sourcePC=pc;
        d.sourceEffectsCommitted=effects_||d.material.sourceEffectsCommitted;
        terminal_=d;stage_=Stage::failed;return false;
    }
    bool enter(Stage expected,Diagnostic& d)const noexcept {
        d={};d.sourceEffectsCommitted=effects_;
        if(stage_==Stage::failed){d=terminal_;return false;}
        if(stage_!=expected||!entity_.detached()||entity_.releasing||!domain_||!domain_->prepared()||
           !context_||!context_->live()||context_->materialApplication()!=domain_->application_){
            d.error=Error::phase;d.sourcePC=0x1895B0;return false;}
        return true;
    }
    static bool bytesMatch(const std::vector<std::uint8_t>* bytes,const assets::OriginalUiMemberView50KG& member)noexcept {
        return bytes&&member.identity&&member.data&&member.size==member.identity->bytes&&bytes->size()==member.size&&
            (!member.size||!std::memcmp(bytes->data(),member.data,member.size));
    }
    bool resolved(const Reference& reference,std::size_t image,
        const assets::OriginalStandardTextureResource50LA*& atlas,Diagnostic& d)const {
        if(image>=12||!reference.retained()||reference.table382B38()!=domain_->materials382B38().get())return false;
        const auto base=imageBase(image),psm=base+".psm";
        const auto pst=overlays_->find(base+".pst");
        const auto* screen=domain_->application_->templates().get(assets::OriginalUiTemplate50KG::screenMt);
        assets::OriginalStandardMaterialRowView50LA row;
        const assets::OriginalStandardMaterialObject50LA* material=nullptr;
        if(!screen||!domain_->materials382B38()->row(reference.material00(),row)||!row.references08||
           *row.references08<=0||row.associated30!=screen||
           !domain_->materials382B38()->resolve298E60(reference.material00(),material,d.material)||
           !material||!material->constructed29B550()||material->textures383400()!=domain_->textures383400().get()||
           !material->passCount48()||*material->passCount48()!=1)return false;
        if(image<10){
            static constexpr std::uint32_t callers[]={0x1895F4,0x189F7C,0x18A080,0x18A184,0x18A288,
                0x18A38C,0x18A490,0x18A594,0x18A698,0x18A7C0};
            const auto* proof=material->speedBoostFallbackProvenance();
            const auto origin=image==9?assets::OriginalUiMaterialOrigin50KG::speedBoostTriangleMissingPsmFallback:
                assets::OriginalUiMaterialOrigin50KG::speedBoostTextureMissingPsmFallback;
            if(material->source()||overlays_->find(psm).identity||!proof||proof->origin!=origin||
               proof->archive!=overlays_->identity()||proof->fallbackTexture!=pst.identity||
               proof->registeredName!=base||proof->attemptedPsmMember!=psm||
               proof->creationPC!=0x2475C0||proof->sourceCallerPC!=callers[image]||
               proof->fallbackPageSource||proof->fallbackWidgetSource)return false;
        }else if(!material->source()||material->speedBoostFallbackProvenance()||
                 !bytesMatch(material->source()->originalBytes(),overlays_->find(psm)))return false;
        assets::OriginalStandardMaterialPassView50LA pass;
        if(!material->pass4C(0,pass)||pass.constructionTemplate!=screen||!pass.group22||*pass.group22!=-1||
           !pass.mapCount20||*pass.mapCount20!=1||!pass.texture1A||(*pass.texture1A)[0]<0||
           pass.animation10||pass.textureAnimation10)return false;
        assets::OriginalStandardTextureRowView50LA texture;
        if(!domain_->textures383400()->row((*pass.texture1A)[0],texture)||!texture.object04||
           !texture.references08||*texture.references08<=0||texture.object04->animated102070()||
           !bytesMatch(texture.object04->originalBytes(),pst))return false;
        atlas=texture.object04;return true;
    }
    bool stageTextures1895B0(const std::string& actualDirectory36B970,std::uint32_t actualVideoDepth60,
        Diagnostic& d)noexcept {
        if(!enter(Stage::fresh,d))return false;
        if(actualVideoDepth60!=16777215u)return stop(d,Error::source,0x2DF384);
        try {
            if(!overlays_||!overlays_->loaded()||overlays_->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::overlays)||
               !localization_||!localization_->loaded()||localization_->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::localization)||
               !english_||!english_->loaded()||english_->indexIdentity()!=localization_->find("loc\\english\\UI.loc").identity||
               english_->textIdentity()!=localization_->find("loc\\english\\L_UI_ENG.bin").identity)
                return stop(d,Error::source,0x1895B0);
            videoDepth60_=actualVideoDepth60;
            for(std::size_t i=0;i<15;++i){
                d.object=i;textures_[i]=std::make_unique<TextureResource>();auto& current=*textures_[i];
                if(!current.material.acquire(Reference::Kind::screenTexture,static_cast<std::uint8_t>(i),0,
                    domain_,overlays_,actualDirectory36B970,d.material))return stop(d,Error::material,0x2DF11C);
                effects_=true;
                if(!domain_->populateSpeedBoostResources2DF144(context_,d.material))return stop(d,Error::material,0x2DF144);
                if(!resolved(current.material,textureImage(i),current.atlas,d)||
                   !current.atlas->sourceWidth14()||!current.atlas->sourceHeight18())return stop(d,Error::source,0x2DF180);
                current.storage58=std::make_unique<Quad>();
                current.storage58->depth=speed_boost_detail50LA::sub(static_cast<float>(videoDepth60_),1.f);
                for(auto& color:current.storage58->colors)color={{127,127,127,63}};
                if(!state_.initializeTexture(i,*current.atlas->sourceWidth14(),*current.atlas->sourceHeight18()))
                    return stop(d,Error::geometry,0x1895F4);
                auto& q=current.storage58->vertices;const auto& t=state_.textures[i];
                // Source strip1,0,2,3 preserves its diagonal in the native fan.
                q[0].u=q[3].u=t.u1;q[1].u=q[2].u=t.u0;
                q[0].v=q[1].v=t.v0;q[2].v=q[3].v=t.v1;
            }
            stage_=Stage::textures;d.sourceEffectsCommitted=effects_;return true;
        }catch(...){return stop(d,Error::allocation,0x2DF1CC);}
    }
    bool stageTriangle18A77C(const std::string& actualDirectory36B970,Diagnostic& d)noexcept {
        if(!enter(Stage::textures,d))return false;
        try {
            triangle_=std::make_unique<TriangleResource>();
            triangle_->vertices00=&state_.triangle; //source2E1E88 vertex publication
            triangle_->packet44=&state_.triangle.packet; //source2E1FC8 backing precedes primitive
            triangle_->primitive30=std::make_unique<TrianglePrimitive>();
            if(!triangle_->primitive30->material.acquire(Reference::Kind::triangle,0,0,
                domain_,overlays_,actualDirectory36B970,d.material))return stop(d,Error::material,0x2E20F0);
            effects_=true;
            triangle_->primitive30->geometry10=triangle_->packet44; //2E2144 AFTER material acquisition
            for(auto& color:triangle_->colors)color={{127,127,127,63}};
            //No material/texture population here, no premature triangle transform.
            stage_=Stage::triangle;d.sourceEffectsCommitted=effects_;return true;
        }catch(...){return stop(d,Error::allocation,0x2E1FC8);}
    }
    bool borrowFonts18A9B4(const Fonts& registry,Diagnostic& d)noexcept {
        if(!enter(Stage::triangle,d))return false;
        if(registry.terminal())return stop(d,Error::font,0x18A99C);
        try {
            const std::array<Fonts::Name,2> names{{Fonts::Name::eurostileItalic26,Fonts::Name::eurostile13}};
            for(std::size_t i=0;i<2;++i){
                d.object=i;const auto entry=registry.find282210(names[i]);
                const auto base=imageBase(10+i);const auto cel=overlays_->find(base+".cel"),pst=overlays_->find(base+".pst");
                std::int16_t token=-1;assets::OriginalStandardTextureRowView50LA row;
                if(!entry||!entry->metrics().decoded()||entry->celIdentity()!=cel.identity||entry->atlasIdentity()!=pst.identity||
                   !domain_->textures383400()->find(base,token)||!domain_->textures383400()->row(token,row)||
                   !row.object04||row.object04!=entry->texture281F90()||!row.references08||*row.references08<=0||
                   !bytesMatch(row.object04->originalBytes(),pst))return stop(d,Error::font,0x282210);
                fonts_[i]=entry;
            }
            stage_=Stage::fonts;d.sourceEffectsCommitted=effects_;return true;
        }catch(...){return stop(d,Error::allocation,0x282210);}
    }
    bool stageTextReferences18A9DC(std::string& actualDirectory36B970,Diagnostic& d)noexcept {
        if(!enter(Stage::fonts,d))return false;
        try {
            for(std::size_t i=0;i<2;++i){
                d.object=i;texts_[i]=std::make_unique<TextResource>();auto& text=*texts_[i];text.state=&state_.texts[i];
                struct DirectoryScope {
                    std::string& actual;std::string previous;bool active=true;
                    explicit DirectoryScope(std::string& value):actual(value),previous(value){actual="font\\";}
                    void restore()noexcept{if(active){actual.swap(previous);active=false;}}
                    ~DirectoryScope(){restore();}
                } directory(actualDirectory36B970);
                for(std::size_t batch=0;batch<2;++batch){
                    d.batch=batch;text.primitives4[batch]=std::make_unique<TextBatch>();auto& primitive=*text.primitives4[batch];
                    primitive.firstGlyph=batch*36;primitive.capacity=batch?4:36;
                    primitive.geometry10=text.glyphsC.data()+primitive.firstGlyph; //before2465B0/299BE0
                    if(!primitive.material.acquire(Reference::Kind::fontBatch,static_cast<std::uint8_t>(i),
                        static_cast<std::uint8_t>(batch),domain_,overlays_,actualDirectory36B970,d.material))
                        return stop(d,Error::material,0x2DFAB0);
                    effects_=true;
                }
                directory.restore(); //2DFB10 before independent alpha/RGB/string setters
                for(auto& glyph:text.glyphsC){glyph.depth=speed_boost_detail50LA::sub(static_cast<float>(videoDepth60_),1.f);
                    for(auto& color:glyph.colors)color=originalUiTextPacketColor50KG(text.state->color,text.state->alpha);}
                text.state->dirty|=3;
                if(i==0){if(!state_.assignText(0,"",0))return stop(d,Error::geometry,0x18AA54);}
                else {const auto* label=english_->at(456);
                    if(!label||label->text!="TEMP"||!state_.assignText(1,label->text.data(),label->text.size()))
                        return stop(d,Error::source,0x18ABA4);}
            }
            //No population per text batch. Later shared phase resolves them.
            stage_=Stage::texts;d.sourceEffectsCommitted=effects_;return true;
        }catch(...){return stop(d,Error::allocation,0x2DF8E0);}
    }
    // After the genuine shared later population/collector. Never rebuilds or
    // acquires anything, and never publishes a native GPU/render capability.
    bool finishNativeColdPreparation50LA(std::shared_ptr<const assets::OriginalUiScreenAssets50KG> pixels,
        const std::array<std::uint32_t,12>& images,std::int32_t actualViewportWidth,
        std::int32_t actualViewportHeight,Diagnostic& d)noexcept {
        if(!enter(Stage::texts,d))return false;
        if(cpuPrepared_||actualViewportWidth!=640||actualViewportHeight!=448||!pixels||!pixels->loaded()||
           !pixels->textures()||pixels->textures()->textureCount()!=12)return stop(d,Error::source,0x1A7B14);
        try {
            const auto provider=domain_->archiveProvider3E1A70();
            if(!provider||!provider->sealed())return stop(d,Error::source,0x132820);
            const auto providerRevision=provider->revision();
            const auto table=domain_->materials382B38();
            collector_=domain_->coldCollector14D440();
            if(!table->coldCollectorCurrent(collector_))return stop(d,Error::material,0x1A7B14);
            std::array<const Reference*,12> representative{};
            for(std::size_t i=0;i<15;++i){
                d.object=i;const auto image=textureImage(i);
                const assets::OriginalStandardTextureResource50LA* atlas=nullptr;
                if(!textures_[i]||!textures_[i]->storage58||!resolved(textures_[i]->material,image,atlas,d)||
                   atlas!=textures_[i]->atlas||(atlases_[image]&&atlases_[image]!=atlas))return stop(d,Error::source,0x1AA5A0);
                atlases_[image]=atlas;if(!representative[image])representative[image]=&textures_[i]->material;
            }
            if(!triangle_||triangle_->vertices00!=&state_.triangle||triangle_->packet44!=&state_.triangle.packet||
               !triangle_->primitive30||triangle_->primitive30->geometry10!=triangle_->packet44||
               !resolved(triangle_->primitive30->material,9,atlases_[9],d))return stop(d,Error::source,0x2E20F0);
            representative[9]=&triangle_->primitive30->material;
            for(std::size_t i=0;i<2;++i){
                if(!texts_[i]||texts_[i]->state!=&state_.texts[i]||!fonts_[i])return stop(d,Error::source,0x2DFD90);
                for(std::size_t b=0;b<2;++b){
                    const auto& primitive=texts_[i]->primitives4[b];
                    const assets::OriginalStandardTextureResource50LA* atlas=nullptr;
                    if(!primitive||primitive->firstGlyph!=b*36||primitive->capacity!=(b?4u:36u)||
                       primitive->geometry10!=texts_[i]->glyphsC.data()+b*36||
                       !resolved(primitive->material,10+i,atlas,d)||atlas!=fonts_[i]->texture281F90())
                        return stop(d,Error::source,0x1AA5A0);
                    atlases_[10+i]=atlas;
                }
                representative[10+i]=&texts_[i]->primitives4[0]->material;
            }
            for(std::size_t i=0;i<12;++i){
                d.object=i;if(images[i]>=12)return stop(d,Error::source,0x1A7B14);
                for(std::size_t j=0;j<i;++j)if(images[j]==images[i])return stop(d,Error::source,0x1A7B14);
                const auto base=imageBase(i);const auto pst=overlays_->find(base+".pst");
                const auto* provenance=pixels->sourceProvenance(images[i]);
                const auto* texture=pixels->textures()->texture(images[i]);
                const auto* atlas=atlases_[i];
                if(!provenance||!texture||!pixels->decodedTexture(images[i])||!atlas||
                   provenance->memberIdentity!=pst.identity||provenance->pstOffset||provenance->pstBytes!=pst.size||
                   provenance->celIdentity!=(i<10?nullptr:fonts_[i-10]->celIdentity())||
                   !atlas->sourceWidth14()||!atlas->sourceHeight18()||*atlas->sourceWidth14()<=0||*atlas->sourceHeight18()<=0||
                   texture->texture.width!=static_cast<std::uint32_t>(*atlas->sourceWidth14())||
                   texture->texture.height!=static_cast<std::uint32_t>(*atlas->sourceHeight18()))return stop(d,Error::source,0x2DF180);
                assets::OriginalStandardMaterialMapBindingView50LA map;
                if(!table->prepareColdScreenPrototypeMap249610(collector_,representative[i]->material00(),0,0,map,d.material)||
                   map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||
                   map.texture!=atlas||map.origin.finalLeaf<0)return stop(d,Error::material,0x249610);
                const assets::OriginalStandardMaterialObject50LA* material=nullptr;
                assets::OriginalStandardMaterialPassView50LA pass;
                if(!table->resolve298E60(representative[i]->material00(),material,d.material)||!material||
                   !material->pass4C(0,pass)||!pass.state30||pass.state30->contextCount()!=1)return stop(d,Error::material,0x298E60);
                const bool parsed=i<10?assets::readOriginalSpeedBoostFallbackMaterial50LA(*overlays_,
                    static_cast<assets::OriginalSpeedBoostImage50LA>(i),domain_->application_->templates(),
                    assets::OriginalUiTemplate50KG::screenMt,context_->materialRuntime(),materials_[i],d.uiMaterial):
                    assets::readOriginalUiMaterial50KG(*overlays_,base+".psm",domain_->application_->templates(),
                    assets::OriginalUiTemplate50KG::screenMt,context_->materialRuntime(),materials_[i],d.uiMaterial);
                if(!parsed||materials_[i].passes.size()!=1)return stop(d,Error::material,0x247110);
                const auto& policy=materials_[i].passes[0];
                for(std::size_t field=0;field<assets::originalUiGsFieldCount50KG;++field)
                    if(policy.gs.fields[field].value!=pass.state30->fields[field].value||
                       policy.gs.fields[field].value!=map.contextAfter249610.fields[field].value)
                        return stop(d,Error::material,0x2476A0);
                const std::int32_t sampler[]={policy.lodCalculationMethod,policy.maximumMipLevel,policy.magFilter,
                    policy.minFilter,policy.lodL,policy.lodK};
                for(std::size_t field=0;field<6;++field)
                    if(sampler[field]!=pass.state30->sampler[field].value)return stop(d,Error::material,0x247F38);
                leaves_[i]=map.origin.finalLeaf;
                assets::OriginalStandardTextureResource50LA* demanded=nullptr;
                if(!domain_->textures383400()->find(base,demandTokens_[i])||
                   !domain_->textures383400()->resolve298E60(demandTokens_[i],demanded,d.material.texture)||
                   demanded!=atlas)return stop(d,Error::source,0x18BA70);
            }
            // The collector does not version archive mounts. Publish readiness
            // only for the same sealed provider revision observed above.
            if(!provider->sealed()||provider->revision()!=providerRevision)
                return stop(d,Error::source,0x132820);
            verifiedProviderRevision_=providerRevision;
            images_=images;pixels_=std::move(pixels);cpuPrepared_=true;
            d.sourceEffectsCommitted=effects_;return true;
        }catch(...){return stop(d,Error::allocation,0x1A7B14);}
    }
    const Reference& imageReference(std::size_t i)const noexcept {
        return i==0?textures_[0]->material:i<9?textures_[i+6]->material:
            i==9?triangle_->primitive30->material:texts_[i-10]->primitives4[0]->material;
    }
    bool prepareTextureQuad(std::size_t i,Diagnostic& d)noexcept {
        if(i>=15||!textures_[i]||!textures_[i]->storage58)return stop(d,Error::geometry,0x2DF550);
        auto& t=state_.textures[i];auto& q=*textures_[i]->storage58;
        for(float f:{t.x,t.y,t.width,t.height,t.u0,t.v0,t.u1,t.v1})
            if(!std::isfinite(f))return stop(d,Error::geometry,0x2DF550);
        if(t.dirty&1u){const float right=speed_boost_detail50LA::add(t.x,t.width);
            const float bottom=speed_boost_detail50LA::add(t.y,t.height);
            q.vertices[0].x=q.vertices[3].x=right;q.vertices[1].x=q.vertices[2].x=t.x;
            q.vertices[0].y=q.vertices[1].y=t.y;q.vertices[2].y=q.vertices[3].y=bottom;
            t.dirty=static_cast<std::uint8_t>(t.dirty&~1u);}
        // Original heat writes UV aliases directly even without a position dirty bit.
        q.vertices[0].u=q.vertices[3].u=t.u1;q.vertices[1].u=q.vertices[2].u=t.u0;
        q.vertices[0].v=q.vertices[1].v=t.v0;q.vertices[2].v=q.vertices[3].v=t.v1;
        return true;
    }
    bool prepareTextGeometry(std::size_t i,Diagnostic& d)noexcept {
        if(i>=2||!texts_[i]||!fonts_[i]||!pixels_||!pixels_->textures())return stop(d,Error::geometry,0x2E0F50);
        auto& resource=*texts_[i];auto& text=*resource.state;
        if(text.length>40||text.bytes[text.length]||!std::isfinite(text.x)||!std::isfinite(text.y)||
           (text.justification!=0&&text.justification!=4))return stop(d,Error::geometry,0x2E13A0);
        if(!(text.dirty&3u))return true;
        const auto* atlas=pixels_->textures()->texture(images_[10+i]);
        if(!atlas)return stop(d,Error::source,0x2E0A30);
        OriginalUiTextInput50KG input{&fonts_[i]->metrics(),&atlas->texture,
            reinterpret_cast<const std::uint8_t*>(text.bytes.data()),text.length,0,0};
        OriginalUiTextLayout50KG layout;
        float x=resource.positionedX20,y=resource.positionedY24;
        if(text.dirty&1u){x=text.x;y=text.y;
            if(text.length&&text.justification==4){
                if(!layoutOriginalUiText50KG(input,layout,d.text))return stop(d,Error::geometry,0x2E16F0);
                std::int32_t width=0,height=0,position=0;
                if(!roundOriginalUiTextMetric50KG(layout.widthBeforeScaleAndRounding,1.f,width)||
                   !roundOriginalUiTextMetric50KG(layout.firstGlyphHeightBeforeScaleAndRounding,1.f,height)||
                   !roundOriginalUiTextMetric50KG(speed_boost_detail50LA::sub(text.x,static_cast<float>(width)),1.f,position))
                    return stop(d,Error::geometry,0x2E153C);
                x=static_cast<float>(position);
            }
        }
        input.localX=x;input.localY=y;
        if(!layoutOriginalUiText50KG(input,layout,d.text)||layout.count!=text.length)
            return stop(d,Error::geometry,0x2E0A30);
        resource.positionedX20=x;resource.positionedY24=y;
        resource.activeBatches5E=(text.length+35u)/36u;
        for(auto& batch:resource.primitives4)batch->activeGlyphs=text.length>batch->firstGlyph?
            std::min<std::size_t>(batch->capacity,text.length-batch->firstGlyph):0;
        const auto color=originalUiTextPacketColor50KG(text.color,text.alpha);
        for(std::size_t n=0;n<layout.count;++n){auto& glyph=resource.glyphsC[n];
            glyph.vertices=layout.quads[n].vertices;glyph.vertexCount=4;
            glyph.depth=speed_boost_detail50LA::sub(static_cast<float>(videoDepth60_),1.f);
            for(auto& c:glyph.colors)c=color;}
        text.dirty=static_cast<std::uint8_t>(text.dirty&~3u);return true;
    }
public:
    const assets::OriginalUiScreenAssets50KG* nativePixels()const noexcept{return pixels_.get();}
    const assets::OriginalUiMaterial50KG* nativeMaterial(std::size_t i)const noexcept {
        return nativeCpuResourcesPrepared()&&i<12?&materials_[i]:nullptr;
    }
    std::uint32_t nativeImage(std::size_t i)const noexcept{return i<12?images_[i]:12u;}
    const OriginalSpeedBoostNativeDraw50LA& nativeDraw()const noexcept{return nativeDraw_;}
    void invalidateNativeDraw()noexcept{nativeDraw_.prepared=false;}
    bool prepareNativeDisplay(const OriginalSpeedBoostNativeFrame50LA& frame,Diagnostic& d)noexcept {
        d={};nativeDraw_.prepared=false;nativeDraw_.primitiveCount=nativeDraw_.packetCount=0;
        if(!nativeCpuResourcesPrepared()||!frame.viewport||!frame.preferences||frame.viewIndex||
           frame.selectedSlot>=8||frame.raceMode8C!=0||
           (frame.raceFlags1B0&3u)!=1u||frame.sessionGeneration!=entity_.session||!frame.cameraPublication||
           frame.cameraPublication<nativeDraw_.cameraPublication||
           nativeDraw_.serial==std::numeric_limits<std::uint64_t>::max())return stop(d,Error::phase,0x18BBB0);
        const auto& view=*frame.viewport;
        if(!view.prepared||!view.matricesReady||!view.sceneKey||view.sessionGeneration!=entity_.session||
           view.x1E0||view.y1E4||view.width1E8!=640||view.height1EC!=448||
           (nativeViewport_&&(nativeViewport_!=&view||nativeScene_!=view.sceneKey)))return stop(d,Error::source,0x18AD30);
        nativeViewport_=&view;nativeScene_=view.sceneKey;nativeDraw_.viewport=&view;
        nativeDraw_.cameraPublication=frame.cameraPublication;++nativeDraw_.serial;
        state_.raceMode6CC=frame.raceMode8C;
        if(!state_.prepareSelected18BBB0(frame.selectedBodySpeed34,frame.viewIndex+1))
            return stop(d,Error::geometry,0x18BC44);
        if((frame.raceFlags1B0&16u)||(frame.raceEntityFlags29&4u)||(frame.uiFlags1E4&1u)){
            nativeDraw_.prepared=true;return true;}
        for(std::size_t i=0;i<12;++i){assets::OriginalStandardMaterialMapBindingView50LA map;
            if(!domain_->materials382B38()->prepareColdScreenPrototypeMap249610(collector_,
                imageReference(i).material00(),0,0,map,d.material)||
               map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||
               map.texture!=atlases_[i]||map.origin.finalLeaf!=leaves_[i])return stop(d,Error::material,0x249610);
            for(std::size_t f=0;f<assets::originalUiGsFieldCount50KG;++f)
                if(map.contextAfter249610.fields[f].value!=materials_[i].passes[0].gs.fields[f].value)
                    return stop(d,Error::material,0x249610);
        }
        const auto demand=[&](std::size_t image,std::uint32_t pc){
            assets::OriginalStandardTextureResource50LA* resource=nullptr;
            if(!domain_->textures383400()->resolve298E60(demandTokens_[image],resource,d.material.texture)||
               resource!=atlases_[image])return false;
            bool written=false;return resource->demandSpeedBoost18BA70(pc,written,d.material.texture);
        };
        // Explicit native eligible-display service of original18BA70 leaves.
        // No independent demand value, global traversal or GPU residency claim.
        for(std::size_t i=5;i<15;++i)if(!demand(textureImage(i),0x18BAB8))return stop(d,Error::material,0x18BAB8);
        for(std::size_t i=10;i<12;++i)if(!demand(i,0x18BB40))return stop(d,Error::material,0x18BB40);
        if(!demand(9,0x18BB88))return stop(d,Error::material,0x18BB88);
        if(frame.viewVehicleState40){nativeDraw_.prepared=true;return true;}
        const auto append=[&](const Quad* primitives,std::size_t count,std::uint32_t material,std::uint32_t vertices){
            if(!count)return true;
            if(!primitives||material>=12||(vertices!=3&&vertices!=4)||(vertices==3&&count!=1)||
               nativeDraw_.packetCount>=nativeDraw_.packets.size()||
               count>nativeDraw_.primitives.size()-nativeDraw_.primitiveCount)return false;
            const auto first=nativeDraw_.primitiveCount;
            for(std::size_t i=0;i<count;++i)nativeDraw_.primitives[nativeDraw_.primitiveCount++]=primitives[i];
            nativeDraw_.packets[nativeDraw_.packetCount++]={first,static_cast<std::uint32_t>(count),material,leaves_[material],vertices};
            return true;
        };
        const bool showSpeed=frame.preferences->slots[frame.viewIndex].field10FA4!=0;
        std::array<speed_boost_detail50LA::Submission,15> sequence;std::size_t count=0;
        if(!state_.heatSequence18BDF0(frame.selectedHeat7C,frame.selectedBoost64,showSpeed,frame.raceBlinkD8,sequence,count))
            return stop(d,Error::geometry,0x18BE98);
        for(std::size_t i=0;i<count;++i){const auto& item=sequence[i];
            if(item.kind==speed_boost_detail50LA::Submission::Kind::texture){
                if(!prepareTextureQuad(item.index,d)||!append(textures_[item.index]->storage58.get(),1,
                    static_cast<std::uint32_t>(textureImage(item.index)),4))return stop(d,Error::geometry,0x2DF550);
            }else {
                // Explicit native-host triangle preparation. Retail draw-dispatch
                // Application22C/383680 is not fabricated by this CPU snapshot.
                // True source triangle: dirty transform only when its actual
                // submission is reached. Three vertices, never a replacement quad.
                if(!state_.triangle.prepareKind2Angle0(videoDepth60_))return stop(d,Error::geometry,0x2E2ED0);
                Quad primitive;primitive.vertexCount=3;primitive.depth=state_.triangle.packet[0].z;
                for(std::size_t v=0;v<3;++v){const auto& vertex=state_.triangle.packet[v];
                    if(vertex.z!=primitive.depth)return stop(d,Error::geometry,0x2E37D0);
                    primitive.vertices[v]={vertex.x,vertex.y,vertex.u,vertex.v};primitive.colors[v]=triangle_->colors[v];}
                if(!append(&primitive,1,9,3))return stop(d,Error::geometry,0x246760);
            }
        }
        for(std::size_t i=0;i<2;++i){
            if(i==0&&!showSpeed)continue;
            if(i==0&&!state_.prepareNumeric18C320())return stop(d,Error::geometry,0x18C3E4);
            if(!prepareTextGeometry(i,d))return false;
            for(std::size_t b=0;b<texts_[i]->activeBatches5E;++b){const auto& primitive=*texts_[i]->primitives4[b];
                if(!append(primitive.geometry10,primitive.activeGlyphs,static_cast<std::uint32_t>(10+i),4))
                    return stop(d,Error::geometry,0x2E0170);}
        }
        for(std::size_t i=1;i<nativeDraw_.packetCount;++i){auto p=nativeDraw_.packets[i];std::size_t j=i;
            while(j&&nativeDraw_.packets[j-1].leaf>p.leaf){nativeDraw_.packets[j]=nativeDraw_.packets[j-1];--j;}
            nativeDraw_.packets[j]=p;
        }
        nativeDraw_.prepared=true;d.sourceEffectsCommitted=true;return true;
    }
    OriginalSpeedBoost50LA(const OriginalSpeedBoost50LA&)=delete;
    OriginalSpeedBoost50LA& operator=(const OriginalSpeedBoost50LA&)=delete;
    ~OriginalSpeedBoost50LA(){
        entity_.beginRelease104970();
        // Source189534 triangle before reverse texture/text arrays. Future
        // renderer MUST own a strong reference through actual drain/retirement;
        // there is no boolean or callback certifying that obligation here.
        triangle_.reset();
        for(std::size_t i=15;i>0;--i)textures_[i-1].reset();
        for(std::size_t i=2;i>0;--i)texts_[i-1].reset();
        fonts_[1].reset();fonts_[0].reset();
    }
    bool nativeCpuResourcesPrepared()const noexcept {
        if(!(cpuPrepared_&&stage_==Stage::texts&&entity_.detached()&&!entity_.releasing&&
            context_&&context_->live()&&domain_&&domain_->materials382B38()->coldCollectorCurrent(collector_)))
            return false;
        // A later mount invalidates readiness even after the provider is resealed.
        const auto& provider=domain_->archiveProvider3E1A70();
        return provider&&provider->sealed()&&provider->revision()==verifiedProviderRevision_;
    }
    // Native renderer must strongly retain this owner until actual GPU work
    // and every pixel/primitive borrow retire. CPU preparation is not display.
};
} // namespace rr::game
