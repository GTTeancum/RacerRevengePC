#pragma once
// Unactivated native-host-scheduled resource owner. Direct text CPU geometry
// is bounded to authentic TimeStats scale1/angle0 and justification0/4.
// ScreenTexture geometry is bounded to source scale1/rotation0. No renderer
// publication still requires an explicit native renderer join.
#include "original_time_stats_state50la.h"
#include "original_ui_text50kg.h"
#include "original_ui_text_position50kg.h"
#include "original_application_entities50la.h"
#include "original_camera_start_effects.h"
#include "../assets/original_ui_screen_assets50kg.h"
#include "../assets/original_time_stats_material_reference50la.h"
#include <utility>

namespace original_main_entry50KG { struct Application; }
namespace rr::game {
// Read-only coherent native projection. The actual bank adapter fills these
//scalar copies after manager updates; they are not independent timing owners
//or proof of completed gameplay. No human/controller role is changed.
struct OriginalTimeStatsNativeRecord50LA {
    float timeCB0=0,bestLapCB4=0,timeCB8=0,valueCBC=0;
    std::uint32_t lapCC8=0,vehicleState40=0;
    std::int32_t placeCCC=0;
    std::uint8_t flagsCD8=0;
};
struct OriginalTimeStatsNativeFrame50LA {
    const OriginalCameraViewportFields* viewport=nullptr;
    const ControlPresets49D* preferences=nullptr;
    std::array<OriginalTimeStatsNativeRecord50LA,8> records{};
    std::uint32_t count=0,selectedSlot=0,viewIndex=0,subtype11258=0,activeCount50=0;
    std::int32_t raceMode8C=0;
    std::uint8_t raceFlags1B0=0,raceEntityFlags29=0,uiFlags1E4=0,managerFlags58=0,lapCount1125C=0;
    std::uint64_t cameraPublication=0,sessionGeneration=0;
};
struct OriginalTimeStatsNativeQuad50LA {
    std::array<OriginalUiTextVertex50KG,4> vertices{};
    std::array<std::array<std::uint8_t,4>,4> packetColors{};
    float depth=0;
};
struct OriginalTimeStatsNativePacket50LA {
    std::uint32_t first=0,count=0,material=0;
    std::int32_t leaf=-1;
};
struct OriginalTimeStatsNativeDraw50LA {
    static constexpr std::size_t maximumQuads=246,maximumPackets=18;
    std::array<OriginalTimeStatsNativeQuad50LA,maximumQuads> quads{};
    std::array<OriginalTimeStatsNativePacket50LA,maximumPackets> packets{};
    std::uint32_t quadCount=0,packetCount=0;
    std::uint64_t serial=0,cameraPublication=0;
    const OriginalCameraViewportFields* viewport=nullptr;
    // CPU preparation only; a renderer must still upload, submit and retire.
    bool prepared=false;
};
struct OriginalTimeStatsDiagnostic50LA {
    enum class Error : std::uint8_t {none,phase,source,material,font,allocation,geometry} error=Error::none;
    std::uint32_t sourcePC=0;
    std::size_t object=0,batch=0;
    bool sourceEffectsCommitted=false;
    assets::OriginalStandardMaterialDiagnostic50LA material;
    OriginalUiTextDiagnostic50KG text;
    assets::OriginalUiMaterialDiagnostic50KG uiMaterial;
};
class OriginalTimeStats50LA final {
    friend struct original_main_entry50KG::Application;
    using Domain=assets::OriginalStandardMaterialDomain50LA;
    using Context=assets::OriginalStandardMaterialContext50LA;
    using Archive=assets::OriginalUiArchive50KG;
    using Reference=assets::OriginalTimeStatsMaterialReference50LA;
    using Fonts=assets::OriginalApplicationFonts50LA;
    using Error=OriginalTimeStatsDiagnostic50LA::Error;
    // These are resource stage states, NEVER renderer readiness. Viewport,
    // canonical collector and primitive submission need the documented joins.
    enum class ResourceStage : std::uint8_t {fresh,statsReferences,fontsBorrowed,textReferences,failed};
    // Genuine detached193100->141300 prefix. Future display is explicitly
    // native-host-scheduled; original1AAD40/1891A0 hierarchy is NOT present.
    // Declared first so actual1413E0 counter cleanup occurs after all derived
    // native storage and resource leases, including constructor unwinding.
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
            --application->objectCount423F60(); //193208->14141C exactly once
        }
    } entity_;
    std::shared_ptr<Domain> domain_;
    std::shared_ptr<const Context> context_;
    std::shared_ptr<const Archive> overlays_,localization_;
    std::shared_ptr<const assets::OriginalUiEnglish50KG> english_;
    std::shared_ptr<const Fonts::Entry> font13_,font16_;
    using QuadStorage=OriginalTimeStatsNativeQuad50LA;
    struct StatsResource {
        Reference material;
        const assets::OriginalStandardTextureResource50LA* atlas=nullptr;
        float reciprocalWidth4C=0,reciprocalHeight50=0;
        // Actual native owned storage; copied into bounded synchronous draws.
        std::unique_ptr<QuadStorage> storage58;
    };
    struct TextBatch {
        Reference material;
        std::size_t firstGlyph=0,capacity=0;
        std::size_t activeGlyphs=0;
        // Borrow into owning TextResource, never a second owning array.
        QuadStorage* geometry10=nullptr;
    };
    struct TextResource {
        // Sole string storage is State::text[i].bytes. The resource borrows
        // that same state, never a second independently mutable string.
        time_stats_detail50LA::ScreenText* state=nullptr;
        float positionedX20=0,positionedY24=0;
        std::size_t activeBatches5E=0;
        std::array<QuadStorage,40> glyphsC{};
        std::array<std::unique_ptr<TextBatch>,2> primitives4{};
        ~TextResource(){
            //2DFC60 visits primitive array ascending; each borrowed geometry
            //belongs to this object and is cleared before native destruction.
            for(auto& p:primitives4){if(p)p->geometry10=nullptr;p.reset();}
        }
    };
    std::array<std::unique_ptr<StatsResource>,6> stats_{};
    std::array<std::unique_ptr<TextResource>,6> texts_{};
    time_stats_detail50LA::State state_;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> textPixels_;
    const assets::Texture48B* textAtlas_=nullptr;
    std::uint32_t fontImage_=0,statsImage_=0,videoDepth60_=0;
    std::array<std::int16_t,3> nativeDemandTokens_{{-1,-1,-1}};
    std::array<assets::OriginalUiMaterial50KG,2> nativeMaterials_;
    assets::OriginalStandardMaterialCollectorReceipt50LA collector_;
    std::array<std::int32_t,2> nativeLeaves_{{-1,-1}};
    bool nativeColdPrepared_=false;
    std::uint64_t verifiedProviderRevision_=0;
    OriginalTimeStatsNativeDraw50LA nativeDraw_;
    const OriginalCameraViewportFields* nativeViewport_=nullptr;
    std::uint64_t nativeScene_=0;
    ResourceStage stage_=ResourceStage::fresh;
    bool effects_=false;
    OriginalTimeStatsDiagnostic50LA terminal_{};
    OriginalTimeStats50LA(std::shared_ptr<OriginalApplicationEntities50LA> entities,
        std::uint64_t actualSessionGeneration,
        std::shared_ptr<Domain> domain,std::shared_ptr<const Context> context,
        std::shared_ptr<const Archive> overlays,std::shared_ptr<const Archive> localization,
        std::shared_ptr<const assets::OriginalUiEnglish50KG> english)
        :entity_(std::move(entities),actualSessionGeneration),
         domain_(std::move(domain)),context_(std::move(context)),overlays_(std::move(overlays)),
         localization_(std::move(localization)),english_(std::move(english)){
        effects_=entity_.initialized;
        if(!entity_.initialized){terminal_.error=Error::source;terminal_.sourcePC=0x19310C;
            stage_=ResourceStage::failed;}
    }
    bool stop(OriginalTimeStatsDiagnostic50LA& d,Error error,std::uint32_t pc) noexcept {
        nativeDraw_.prepared=false;
        d.error=error;d.sourcePC=pc;
        d.sourceEffectsCommitted=effects_||d.material.sourceEffectsCommitted;
        terminal_=d;stage_=ResourceStage::failed;return false;
    }
    bool enter(ResourceStage expected,OriginalTimeStatsDiagnostic50LA& d)const noexcept {
        d={};d.sourceEffectsCommitted=effects_;
        if(stage_==ResourceStage::failed){d=terminal_;return false;}
        if(stage_!=expected||!entity_.detached()||entity_.releasing||
           !domain_||!domain_->prepared()||!context_||!context_->live()||
           context_->materialApplication()!=domain_->application_){
            d.error=Error::phase;d.sourcePC=0x193240;return false;
        }
        return true;
    }
    static bool bytesMatch(const std::vector<std::uint8_t>* actual,
        const assets::OriginalUiMemberView50KG& expected) noexcept {
        return actual&&expected.identity&&expected.data&&actual->size()==expected.size&&
            (!expected.size||!std::memcmp(actual->data(),expected.data,expected.size));
    }
    bool resolved(const Reference& reference,const char* base,
        const assets::OriginalStandardTextureResource50LA*& atlas,
        OriginalTimeStatsDiagnostic50LA& d)const {
        if(!reference.retained()||reference.table382B38()!=domain_->materials382B38().get())return false;
        const auto* screen=domain_->application_->templates().get(assets::OriginalUiTemplate50KG::screenMt);
        assets::OriginalStandardMaterialRowView50LA materialRow;
        if(!screen||!domain_->materials382B38()->row(reference.material00(),materialRow)||
           !materialRow.references08||*materialRow.references08<=0||materialRow.associated30!=screen)return false;
        const assets::OriginalStandardMaterialObject50LA* material=nullptr;
        if(!domain_->materials382B38()->resolve298E60(reference.material00(),material,d.material)||
           !material||!material->constructed29B550()||!material->source()||
           material->textures383400()!=domain_->textures383400().get())return false;
        const auto psm=overlays_->find(std::string(base)+".psm");
        const auto pst=overlays_->find(std::string(base)+".pst");
        if(!bytesMatch(material->source()->originalBytes(),psm)||!material->passCount48()||
           *material->passCount48()!=1)return false;
        assets::OriginalStandardMaterialPassView50LA pass;
        if(!material->pass4C(0,pass)||!pass.mapCount20||*pass.mapCount20!=1||
           !pass.texture1A||(*pass.texture1A)[0]<0||pass.animation10||pass.textureAnimation10||
           pass.constructionTemplate!=screen||!pass.group22||
           *pass.group22!=(std::strcmp(base,"ui\\textures\\stats")==0?-2:-1))return false;
        assets::OriginalStandardTextureRowView50LA row;
        if(!domain_->textures383400()->row((*pass.texture1A)[0],row)||!row.object04||
           !row.references08||*row.references08<=0||!bytesMatch(row.object04->originalBytes(),pst))return false;
        atlas=row.object04;return true;
    }
    //Application calls under actual outerui\textures\ scope. Source resource
    //effects stay in order. Application supplies actual Video+60, restricted
    //to the proven native domain; it is not a readiness flag or chosen HUD Z.
    bool stageStats193240(const std::string& actualDirectory36B970,
        std::uint32_t actualVideoDepth60,OriginalTimeStatsDiagnostic50LA& d)noexcept {
        if(!enter(ResourceStage::fresh,d))return false;
        if(actualVideoDepth60!=16777215u)return stop(d,Error::source,0x2DF384);
        try {
            if(!overlays_||!overlays_->loaded()||overlays_->identity()!=
               assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::overlays)||
               !localization_||!localization_->loaded()||localization_->identity()!=
               assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::localization)||
               !english_||!english_->loaded())return stop(d,Error::source,0x193240);
            for(std::size_t i=0;i<6;++i){
                d.object=i;stats_[i]=std::make_unique<StatsResource>();auto& current=*stats_[i];
                if(!current.material.acquire(Reference::Kind::statsTexture,static_cast<std::uint8_t>(i),0,
                    domain_,overlays_,actualDirectory36B970,d.material))return stop(d,Error::material,0x2DF11C);
                effects_=true;
                if(!domain_->populateTimeStatsResources2DF144(context_,d.material))
                    return stop(d,Error::material,0x2DF144);
                if(!resolved(current.material,"ui\\textures\\stats",current.atlas,d))
                    return stop(d,Error::source,0x2DF164);
                const auto* width=current.atlas->sourceWidth14();const auto* height=current.atlas->sourceHeight18();
                if(!width||!height||*width<=0||*height<=0)return stop(d,Error::source,0x2DF180);
                current.reciprocalWidth4C=1.f/static_cast<float>(*width);
                current.reciprocalHeight50=1.f/static_cast<float>(*height);
                current.storage58=std::make_unique<QuadStorage>();
                auto& geometry=*current.storage58;
                videoDepth60_=actualVideoDepth60;
                geometry.depth=static_cast<float>(actualVideoDepth60)-1.f;
                for(auto& color:geometry.packetColors)color={{127,127,127,63}};
                auto& state=state_.texture[i];
                state.width34=i%2?32.f:149.f;state.height38=19.f;
                state.u0=i%2?.7890625f:.5f;state.u1=i%2?1.f:.78125f;
                state.v0=.796875f;state.v1=.9375f;
                state.field3C=0;state.flags5C|=1;
                // Native order TL,TR,BR,BL expands original0,1,3,2.
                geometry.vertices[0].u=state.u0;geometry.vertices[0].v=state.v0;
                geometry.vertices[1].u=state.u1;geometry.vertices[1].v=state.v0;
                geometry.vertices[2].u=state.u1;geometry.vertices[2].v=state.v1;
                geometry.vertices[3].u=state.u0;geometry.vertices[3].v=state.v1;
            }
            stage_=ResourceStage::statsReferences;d.sourceEffectsCommitted=effects_;return true;
        }catch(...){return stop(d,Error::allocation,0x2DF1CC);}
    }
    // Original2DF550 CPU dirty regeneration only; actual viewport copy,
    // draw gating and246760 collector submission belong to the root adapter.
    bool prepareStatsGeometry2DF550(std::size_t object,
        OriginalTimeStatsDiagnostic50LA& d)noexcept {
        if(!enter(ResourceStage::textReferences,d))return false;
        d.object=object;
        if(object>=stats_.size()||!stats_[object]||!stats_[object]->storage58)
            return stop(d,Error::source,0x2DF550);
        auto& state=state_.texture[object];auto& geometry=*stats_[object]->storage58;
        if(state.field3C!=0||!std::isfinite(state.x10)||!std::isfinite(state.y14)||
           !std::isfinite(state.width34)||!std::isfinite(state.height38))
            return stop(d,Error::geometry,0x2DF694);
        if(state.flags5C&1u){
            // Authenticated scale30 remains1: no alternate scale setter is
            // exposed. Keep source add sequence and host/source corner map.
            const float width=state.width34*1.f,height=state.height38*1.f;
            const float right=state.x10+width,bottom=state.y14+height;
            if(!std::isfinite(right)||!std::isfinite(bottom))
                return stop(d,Error::geometry,0x2DF6C0);
            geometry.vertices[0].x=state.x10;geometry.vertices[0].y=state.y14;
            geometry.vertices[1].x=right;geometry.vertices[1].y=state.y14;
            geometry.vertices[2].x=right;geometry.vertices[2].y=bottom;
            geometry.vertices[3].x=state.x10;geometry.vertices[3].y=bottom;
            state.flags5C=static_cast<std::uint8_t>(state.flags5C&~1u);
        }
        d.sourceEffectsCommitted=effects_;return true;
    }
    //Only Application may call this after its real193A0C font-manager call.
    //No callback, caller completion bool or independently constructed font.
    bool borrowFonts193A24(const Fonts& actualRegistry,OriginalTimeStatsDiagnostic50LA& d)noexcept {
        if(!enter(ResourceStage::statsReferences,d))return false;
        if(actualRegistry.terminal())return stop(d,Error::font,0x193A24);
        const auto first=actualRegistry.find282210(Fonts::Name::eurostile13);
        const auto second=actualRegistry.find282210(Fonts::Name::denmark16);
        const auto check=[&](const std::shared_ptr<const Fonts::Entry>& entry,const char* name){
            if(!entry||!entry->metrics().decoded())return false;
            const auto cel=overlays_->find(std::string(name)+".cel");
            const auto pst=overlays_->find(std::string(name)+".pst");
            if(entry->celIdentity()!=cel.identity||entry->atlasIdentity()!=pst.identity)return false;
            std::int16_t token=-1;assets::OriginalStandardTextureRowView50LA row;
            return domain_->textures383400()->find(name,token)&&domain_->textures383400()->row(token,row)&&
                row.object04&&row.object04==entry->texture281F90()&&row.references08&&*row.references08>0&&
                bytesMatch(row.object04->originalBytes(),pst);
        };
        try {
            if(!check(first,"font\\eurostileboldbold13ingame")||!check(second,"font\\denmarkbold16ingame"))
                return stop(d,Error::font,0x281F90);
            font13_=first;font16_=second;stage_=ResourceStage::fontsBorrowed;
            d.sourceEffectsCommitted=effects_;return true;
        }catch(...){return stop(d,Error::allocation,0x193A24);}
    }
    bool stageTextReferences193A58(std::string& actualDirectory36B970,
        OriginalTimeStatsDiagnostic50LA& d)noexcept {
        if(!enter(ResourceStage::fontsBorrowed,d))return false;
        try {
            for(std::size_t i=0;i<6;++i){
                d.object=i;texts_[i]=std::make_unique<TextResource>();auto& text=*texts_[i];
                text.state=&state_.text[i];
                struct DirectoryScope {
                    std::string& actual;std::string previous;bool active=true;
                    explicit DirectoryScope(std::string& value):actual(value),previous(value){actual="font\\";}
                    void restore()noexcept{if(active){actual.swap(previous);active=false;}}
                    ~DirectoryScope(){restore();}
                } directory(actualDirectory36B970);
                for(std::size_t batch=0;batch<2;++batch){
                    d.batch=batch;text.primitives4[batch]=std::make_unique<TextBatch>();
                    auto& primitive=*text.primitives4[batch];
                    primitive.firstGlyph=batch*36;primitive.capacity=batch?4:36;
                    primitive.geometry10=text.glyphsC.data()+primitive.firstGlyph;
                    if(!primitive.material.acquire(Reference::Kind::fontBatch,static_cast<std::uint8_t>(i),
                        static_cast<std::uint8_t>(batch),domain_,overlays_,actualDirectory36B970,d.material))
                        return stop(d,Error::material,0x2DFAB0);
                    effects_=true;
                }
                directory.restore(); //2DFB10 before2DFD90 alpha and193A8C RGB.
                text.state->flags5A=3;
                text.state->justification50=i<3?0u:4u;
                //Independent alpha setter2E0094 / RGB setter193A8C; all40
                //capacity glyphs, not only active characters (initially none).
                for(auto& glyph:text.glyphsC)for(auto& color:glyph.packetColors)color={{114,108,54,63}};
            }
            //No14D370/29E340 here. Canonical text material may be null.
            stage_=ResourceStage::textReferences;d.sourceEffectsCommitted=effects_;return true;
        }catch(...){return stop(d,Error::allocation,0x2DF8E0);}
    }
    // Immutable decode bridge only. Canonical font Entry and material leases
    // above remain the sole source resource-demand ownership. Root supplies
    // this existing native decoder owner from exact OVERLAYS bytes; the
    // duplicate CEL decoded by that bridge is never used as our font owner.
    bool borrowTextPixels(std::shared_ptr<const assets::OriginalUiScreenAssets50KG> pixels,
        std::uint32_t image,OriginalTimeStatsDiagnostic50LA& d)noexcept {
        if(!enter(ResourceStage::textReferences,d))return false;
        if(textPixels_||!pixels||!pixels->loaded()||!font13_||!pixels->textures())
            return stop(d,Error::source,0x2E0A80);
        const auto* provenance=pixels->sourceProvenance(image);
        const auto* decoded=pixels->decodedTexture(image);
        const auto* imageAsset=pixels->textures()->texture(image);
        const auto* canonical=font13_->texture281F90();
        if(!provenance||!decoded||!imageAsset||!canonical||
           provenance->memberIdentity!=font13_->atlasIdentity()||
           provenance->celIdentity!=font13_->celIdentity()||provenance->pstOffset||
           provenance->pstBytes!=font13_->atlasIdentity()->bytes||
           !canonical->sourceWidth14()||!canonical->sourceHeight18()||
           *canonical->sourceWidth14()<=0||*canonical->sourceHeight18()<=0||
           imageAsset->texture.width!=static_cast<std::uint32_t>(*canonical->sourceWidth14())||
           imageAsset->texture.height!=static_cast<std::uint32_t>(*canonical->sourceHeight18()))
            return stop(d,Error::source,0x2E0A88);
        textAtlas_=&imageAsset->texture; fontImage_=image;textPixels_=std::move(pixels);
        d.sourceEffectsCommitted=effects_;return true;
    }
    // Called immediately before the genuine direct ScreenText submission,
    // after source text assignment and source layout. Does not resolve pending
    // materials, populate tables, rebuild the collector or publish to GPU.
    // actualVideoDepth60 is read from the actual Video, never an overlay Z.
    bool prepareTextGeometry2E0F50(std::size_t object,std::uint32_t actualVideoDepth60,
        OriginalTimeStatsDiagnostic50LA& d)noexcept {
        if(!enter(ResourceStage::textReferences,d))return false;
        d.object=object;
        if(object>=texts_.size()||!texts_[object]||!textAtlas_||!font13_||
           actualVideoDepth60!=16777215u)return stop(d,Error::source,0x2E10DC);
        auto& resource=*texts_[object];auto& text=*resource.state;
        if(text.length>40||text.bytes[text.length]!=0||
           (text.justification50!=0&&text.justification50!=4)||
           !std::isfinite(text.x10)||!std::isfinite(text.y14))
            return stop(d,Error::geometry,0x2E13A0);
        if(!(text.flags5A&3u)){d.sourceEffectsCommitted=effects_;return true;}
        // Native validation is staged before mutating owned geometry. All
        // source-admitted success writes retain original numeric order; a
        // rejected native input never leaves a partially publishable quad.
        float positionedX=resource.positionedX20,positionedY=resource.positionedY24;
        OriginalUiTextInput50KG input{&font13_->metrics(),textAtlas_,
            reinterpret_cast<const std::uint8_t*>(text.bytes.data()),text.length,0,0};
        OriginalUiTextLayout50KG layout;
        if(text.flags5A&1u){
            positionedX=text.x10;positionedY=text.y14;
            if(text.length&&text.justification50==4){
                if(!layoutOriginalUiText50KG(input,layout,d.text))
                    return stop(d,Error::geometry,0x2E16F0);
                std::int32_t width=0,height=0,coordinate=0;
                if(!roundOriginalUiTextMetric50KG(layout.widthBeforeScaleAndRounding,1.f,width))
                    return stop(d,Error::geometry,0x2E18A8);
                // 2E146C calls GetHeight even though right justification does
                // not consume the returned height in its final coordinate.
                if(!roundOriginalUiTextMetric50KG(layout.firstGlyphHeightBeforeScaleAndRounding,1.f,height))
                    return stop(d,Error::geometry,0x2E15B0);
                const float shifted=text.x10-static_cast<float>(width);
                if(!roundOriginalUiTextMetric50KG(shifted,1.f,coordinate))
                    return stop(d,Error::geometry,0x2E153C);
                positionedX=static_cast<float>(coordinate);
            }
        }
        // 2E0AD0/DC divide by source scale1. 2E1240..1274 later multiply
        // by1; zero rotation performs no trigonometry or pivot transform.
        input.localX=positionedX;input.localY=positionedY;
        if(!layoutOriginalUiText50KG(input,layout,d.text)||layout.count!=text.length)
            return stop(d,Error::geometry,0x2E0A30);
        const float depth=static_cast<float>(actualVideoDepth60)-1.f;
        const auto color=originalUiTextPacketColor50KG(text.color54,text.alpha58);
        // Source 2E0880 runs first, using count already set by string setter.
        resource.activeBatches5E=(text.length+35u)/36u;
        for(std::size_t batch=0;batch<2;++batch){
            auto& primitive=*resource.primitives4[batch];
            primitive.activeGlyphs=text.length>primitive.firstGlyph?
                std::min<std::size_t>(primitive.capacity,text.length-primitive.firstGlyph):0;
        }
        resource.positionedX20=positionedX;resource.positionedY24=positionedY;
        for(std::size_t glyph=0;glyph<layout.count;++glyph){
            resource.glyphsC[glyph].vertices=layout.quads[glyph].vertices;
            resource.glyphsC[glyph].depth=depth;
            for(auto& c:resource.glyphsC[glyph].packetColors)c=color;
        }
        // Position clears bit0/sets bits1,2; inverse scale clears2; final
        // transform clears1 (2E12E8..1300). Preserve unrelated high bits.
        text.flags5A=static_cast<std::uint8_t>(text.flags5A&~7u);
        d.sourceEffectsCommitted=effects_;return true;
    }
    // Explicit native cold completion, after the actual later population and
    //collector. This certifies CPU/source resources only, never a GPU target,
    //retail parent hierarchy or submitted image.
    bool finishNativeColdPreparation50LA(
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG> pixels,
        std::uint32_t statsImage,std::uint32_t fontImage,
        std::int32_t actualViewportWidth,std::int32_t actualViewportHeight,
        OriginalTimeStatsDiagnostic50LA& d)noexcept {
        if(!enter(ResourceStage::textReferences,d))return false;
        if(nativeColdPrepared_||actualViewportWidth!=640||actualViewportHeight!=448||
           statsImage==fontImage||!pixels||!pixels->textures()||pixels->textures()->textureCount()!=2)
            return stop(d,Error::source,0x1A7B14);
        if(!textPixels_){if(!borrowTextPixels(pixels,fontImage,d))return false;}
        else if(textPixels_!=pixels||fontImage_!=fontImage)return stop(d,Error::source,0x2E0A30);
        try {
            const auto provider=domain_->archiveProvider3E1A70();
            if(!provider||!provider->sealed())return stop(d,Error::source,0x132820);
            const auto providerRevision=provider->revision();
            const auto pst=overlays_->find("ui\\textures\\stats.pst");
            const auto* provenance=pixels->sourceProvenance(statsImage);
            const auto* texture=pixels->textures()->texture(statsImage);
            const auto* statsAtlas=stats_[0]?stats_[0]->atlas:nullptr;
            if(!provenance||!texture||!statsAtlas||provenance->memberIdentity!=pst.identity||
               provenance->celIdentity||provenance->pstOffset||provenance->pstBytes!=pst.size||
               !statsAtlas->sourceWidth14()||!statsAtlas->sourceHeight18()||
               texture->texture.width!=static_cast<std::uint32_t>(*statsAtlas->sourceWidth14())||
               texture->texture.height!=static_cast<std::uint32_t>(*statsAtlas->sourceHeight18()))
                return stop(d,Error::source,0x2DF180);
            for(std::size_t i=0;i<6;++i){
                const assets::OriginalStandardTextureResource50LA* actual=nullptr;
                if(!stats_[i]||!texts_[i]||!resolved(stats_[i]->material,"ui\\textures\\stats",actual,d)||actual!=statsAtlas)
                    return stop(d,Error::source,0x1AA5A0);
                for(const auto& batch:texts_[i]->primitives4)
                    if(!batch||!resolved(batch->material,"font\\eurostileboldbold13ingame",actual,d)||
                       actual!=font13_->texture281F90())return stop(d,Error::source,0x1AA5A0);
            }
            collector_=domain_->coldCollector14D440();
            const auto& table=domain_->materials382B38();
            if(!table->coldCollectorCurrent(collector_))return stop(d,Error::material,0x1A7B14);
            const char* names[]={"ui\\textures\\stats.psm","font\\eurostileboldbold13ingame.psm"};
            for(std::size_t i=0;i<2;++i){
                const auto& reference=i?texts_[0]->primitives4[0]->material:stats_[0]->material;
                assets::OriginalStandardMaterialMapBindingView50LA map;
                if(!table->prepareColdScreenPrototypeMap249610(collector_,reference.material00(),0,0,map,d.material)||
                   map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||
                   map.texture!=(i?font13_->texture281F90():statsAtlas)||map.origin.finalLeaf<0)
                    return stop(d,Error::material,0x249610);
                const assets::OriginalStandardMaterialObject50LA* material=nullptr;
                assets::OriginalStandardMaterialPassView50LA pass;
                if(!table->resolve298E60(reference.material00(),material,d.material)||!material||!material->pass4C(0,pass)||
                   !pass.state30||pass.state30->contextCount()!=1)
                    return stop(d,Error::material,0x298E60);
                if(!assets::readOriginalUiMaterial50KG(*overlays_,names[i],domain_->application_->templates(),
                    assets::OriginalUiTemplate50KG::screenMt,context_->materialRuntime(),nativeMaterials_[i],d.uiMaterial)||
                   nativeMaterials_[i].passes.size()!=1)return stop(d,Error::material,0x247110);
                const auto& parsed=nativeMaterials_[i].passes[0];
                for(std::size_t field=0;field<assets::originalUiGsFieldCount50KG;++field)
                    if(parsed.gs.fields[field].value!=pass.state30->fields[field].value||
                       parsed.gs.fields[field].value!=map.contextAfter249610.fields[field].value)
                        return stop(d,Error::material,0x2476A0);
                const std::int32_t sampler[]={parsed.lodCalculationMethod,parsed.maximumMipLevel,
                    parsed.magFilter,parsed.minFilter,parsed.lodL,parsed.lodK};
                for(std::size_t field=0;field<6;++field)
                    if(sampler[field]!=pass.state30->sampler[field].value)
                        return stop(d,Error::material,0x247F38);
                nativeLeaves_[i]=map.origin.finalLeaf;
            }
            const char* demanded[]={"ui\\textures\\stats","font\\eurostileboldbold13ingame","font\\denmarkbold16ingame"};
            for(std::size_t i=0;i<3;++i){
                assets::OriginalStandardTextureResource50LA* texture=nullptr;
                if(!domain_->textures383400()->find(demanded[i],nativeDemandTokens_[i])||
                   !domain_->textures383400()->resolve298E60(nativeDemandTokens_[i],texture,d.material.texture)||
                   texture!=(i==0?statsAtlas:i==1?font13_->texture281F90():font16_->texture281F90()))
                    return stop(d,Error::source,0x194E20);
            }
            // The collector does not version archive mounts. Publish readiness
            // only for the same sealed provider revision observed above.
            if(!provider->sealed()||provider->revision()!=providerRevision)
                return stop(d,Error::source,0x132820);
            verifiedProviderRevision_=providerRevision;
            statsImage_=statsImage;nativeColdPrepared_=true;
            d.sourceEffectsCommitted=effects_;return true;
        }catch(...){return stop(d,Error::allocation,0x1A7B14);}
    }
public:
    bool nativeCpuResourcesPrepared()const noexcept {
        if(!(nativeColdPrepared_&&videoDepth60_==16777215u&&stage_==ResourceStage::textReferences&&entity_.detached()&&
            !entity_.releasing&&context_&&context_->live()&&domain_&&
            domain_->materials382B38()->coldCollectorCurrent(collector_)))return false;
        // A later mount invalidates readiness even after the provider is resealed.
        const auto& provider=domain_->archiveProvider3E1A70();
        return provider&&provider->sealed()&&provider->revision()==verifiedProviderRevision_;
    }
    const assets::OriginalUiScreenAssets50KG* nativePixels()const noexcept{return textPixels_.get();}
    const assets::OriginalUiMaterial50KG* nativeMaterial(std::size_t i)const noexcept {
        return nativeCpuResourcesPrepared()&&i<2?&nativeMaterials_[i]:nullptr;
    }
    std::uint32_t nativeImage(std::size_t material)const noexcept{return material?fontImage_:statsImage_;}
    const OriginalTimeStatsNativeDraw50LA& nativeDraw()const noexcept{return nativeDraw_;}
    // One accepted native world update, never a wall-clock or host timer.
    // The detached component has no children; this is not1891A0 traversal.
    bool advanceNativeWorld(float seconds)noexcept {
        nativeDraw_.prepared=false;
        return nativeCpuResourcesPrepared()&&state_.advance194D00(seconds);
    }
    // Explicit single-view native display policy. Source records are coherent
    //scalar observations from the actual bank, not HOST standings or drivers.
    bool prepareNativeDisplay(const OriginalTimeStatsNativeFrame50LA& frame,
        OriginalTimeStatsDiagnostic50LA& d)noexcept {
        d={};nativeDraw_.prepared=false;nativeDraw_.quadCount=nativeDraw_.packetCount=0;
        if(!nativeCpuResourcesPrepared()||!frame.viewport||!frame.preferences||frame.viewIndex||
           frame.count!=8||frame.selectedSlot>=frame.count||frame.raceMode8C!=0||
           (frame.raceFlags1B0&3u)!=1u||frame.sessionGeneration!=entity_.session||
           !frame.cameraPublication||frame.cameraPublication<nativeDraw_.cameraPublication)
            return stop(d,Error::phase,0x194FF0);
        const auto& view=*frame.viewport;
        if(!view.prepared||!view.matricesReady||view.sessionGeneration!=entity_.session||!view.sceneKey||
           view.width1E8!=640||view.height1EC!=448||view.x1E0||view.y1E4||
           (nativeViewport_&&(nativeViewport_!=&view||nativeScene_!=view.sceneKey)))
            return stop(d,Error::source,0x193BB0);
        if(nativeDraw_.serial==std::numeric_limits<std::uint64_t>::max())return stop(d,Error::phase,0x194FF0);
        nativeViewport_=&view;nativeScene_=view.sceneKey;nativeDraw_.viewport=&view;
        nativeDraw_.cameraPublication=frame.cameraPublication;++nativeDraw_.serial;
        state_.mode520=0;
        const auto& selected=frame.records[frame.selectedSlot];
        if(!state_.prepare194F40(0,*frame.preferences,selected.vehicleState40,selected.lapCC8,frame.managerFlags58))
            return stop(d,Error::geometry,0x194F40);
        if(!time_stats_detail50LA::displayGate(0,frame.raceFlags1B0,frame.raceEntityFlags29,frame.uiFlags1E4)){
            nativeDraw_.prepared=true;return true;
        }
        // Reacquire actual effective map state each display. This first
        //indexed-PST renderer subset rejects unsupported TEXA changes instead
        //of silently drawing stale constructor alpha state.
        for(std::size_t i=0;i<2;++i){
            const auto& reference=i?texts_[0]->primitives4[0]->material:stats_[0]->material;
            assets::OriginalStandardMaterialMapBindingView50LA map;
            if(!domain_->materials382B38()->prepareColdScreenPrototypeMap249610(collector_,reference.material00(),0,0,map,d.material)||
               map.preparation!=assets::OriginalStandardMaterialMapPreparation50LA::ready||
               map.texture!=(i?font13_->texture281F90():stats_[0]->atlas)||map.origin.finalLeaf!=nativeLeaves_[i])
                return stop(d,Error::material,0x249610);
            const auto& raster=nativeMaterials_[i].passes[0];
            for(std::size_t field=0;field<assets::originalUiGsFieldCount50KG;++field)
                if(map.contextAfter249610.fields[field].value!=raster.gs.fields[field].value)
                    return stop(d,Error::material,0x249610);
        }
        // Native eligible-display service of the exact194E20 leaf operations;
        //full parent/global traversal and resource residency remain unclaimed.
        constexpr std::uint32_t stores[]={0x194E58u,0x194EB8u,0x194F18u};
        for(std::size_t i=0;i<3;++i){
            assets::OriginalStandardTextureResource50LA* texture=nullptr;
            if(!domain_->textures383400()->resolve298E60(nativeDemandTokens_[i],texture,d.material.texture)||
               texture!=(i==0?stats_[0]->atlas:i==1?font13_->texture281F90():font16_->texture281F90()))
                return stop(d,Error::source,stores[i]);
            bool written=false;
            if(!texture->demandTimeStats194E20(stores[i],written,d.material.texture))
                return stop(d,Error::material,stores[i]);
        }
        const auto append=[&](const QuadStorage* quads,std::size_t count,std::uint32_t material){
            if(!count)return true;
            if(!quads||nativeDraw_.packetCount>=nativeDraw_.packets.size()||
               count>nativeDraw_.quads.size()-nativeDraw_.quadCount)return false;
            const auto first=nativeDraw_.quadCount;
            for(std::size_t i=0;i<count;++i)nativeDraw_.quads[nativeDraw_.quadCount++]=quads[i];
            nativeDraw_.packets[nativeDraw_.packetCount++]={first,static_cast<std::uint32_t>(count),material,nativeLeaves_[material]};
            return true;
        };
        std::array<bool,3> active{},visible{};
        for(std::size_t row=0;row<3;++row){
            active[row]=time_stats_detail50LA::predicate(row,frame.preferences->slots[0],selected.vehicleState40);
            visible[row]=active[row]||state_.views[0].offset534!=-165.f;
            if(!visible[row])continue;
            for(std::size_t side=0;side<2;++side){const auto object=row*2+side;
                if(!prepareStatsGeometry2DF550(object,d)||!append(stats_[object]->storage58.get(),1,0))
                    return stop(d,Error::geometry,0x2DF874);
            }
        }
        const auto record=[&](std::size_t i){const auto& r=frame.records[i];
            return time_stats_detail50LA::Record{r.timeCB0,r.bestLapCB4,r.timeCB8,r.valueCBC,r.lapCC8,r.placeCCC,r.flagsCD8};};
        const std::array<time_stats_detail50LA::Record,8> records{{record(0),record(1),record(2),record(3),record(4),record(5),record(6),record(7)}};
        constexpr std::size_t labels[]={2,0,1},values[]={5,3,4};
        const auto textPacket=[&](std::size_t object,const char* bytes,std::size_t length,std::uint32_t justification){
            auto& text=state_.text[object];
            if(text.justification50!=justification){text.justification50=justification;text.flags5A|=3;}
            time_stats_detail50LA::assign(text,bytes,length);
            if(!prepareTextGeometry2E0F50(object,videoDepth60_,d))return false;
            const auto& resource=*texts_[object];
            for(std::size_t batch=0;batch<resource.activeBatches5E;++batch){const auto& primitive=*resource.primitives4[batch];
                if(!append(primitive.geometry10,primitive.activeGlyphs,1))return false;}
            return true;
        };
        for(std::size_t row=0;row<3;++row){
            if(!visible[row])continue;
            const auto labelIndex=row==0?458u:row==1?459u:frame.subtype11258==1?460u:461u;
            const auto* label=english_->at(labelIndex);
            if(!label||!textPacket(labels[row],label->text.data(),label->text.size(),0))
                return stop(d,Error::geometry,0x195250);
            time_stats_detail50LA::RowContent content;
            if(!time_stats_detail50LA::prepareRow(row,active[row],records[frame.selectedSlot],records.data(),frame.count,
                frame.activeCount50,frame.subtype11258,frame.lapCount1125C,*english_,content)||
               !textPacket(values[row],content.value.data(),content.valueLength,4))
                return stop(d,Error::geometry,0x195250);
        }
        // Actual canonical final leaves, stable source submission order within
        //a leaf. Unique stats remains one unmerged pass with six submissions.
        for(std::size_t i=1;i<nativeDraw_.packetCount;++i){auto p=nativeDraw_.packets[i];std::size_t j=i;
            while(j&&nativeDraw_.packets[j-1].leaf>p.leaf){nativeDraw_.packets[j]=nativeDraw_.packets[j-1];--j;}
            nativeDraw_.packets[j]=p;
        }
        nativeDraw_.prepared=true;d.sourceEffectsCommitted=true;return true;
    }
    OriginalTimeStats50LA(const OriginalTimeStats50LA&)=delete;
    OriginalTimeStats50LA& operator=(const OriginalTimeStats50LA&)=delete;
    ~OriginalTimeStats50LA(){
        // Check actual refs/topology BEFORE destroying any derived resource.
        // No fake ancestor, detach or recursive child release is performed.
        entity_.beginRelease104970();
        // A native renderer must retain this owner strongly until all GPU work
        // and pixel borrows retire. Frame views are synchronous, never queued
        // across world updates or camera teardown.
        for(std::size_t i=6;i>0;--i)stats_[i-1].reset();
        for(std::size_t i=6;i>0;--i)texts_[i-1].reset();
        font16_.reset();font13_.reset();
    }
    // No public resource-stage setters. CPU preparation never certifies GPU
    // readiness or a completed gameplay display; those belong to the renderer.
};
} // namespace rr::game
