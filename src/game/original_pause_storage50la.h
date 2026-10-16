#pragma once
#include "../assets/original_pause_primitive_references50la.h"
#include "../assets/original_application_fonts50la.h"
#include <array>
#include <algorithm>
#include <bit>
#include <cstring>
#include "control_presets49d.h"
#include "original_application_entities50la.h"
#include "original_application_animation_clock50kg.h"
#include "original_ui_game_options_controller50kg.h"
#include <memory>
#include <utility>

namespace original_pause_observation50LA { class Owner; }
namespace original_main_entry50KG { struct Application; }
namespace rr::game {
class OriginalPauseLayout50LA;
class OriginalPauseTextGeometry50LA;
class OriginalPauseTextureGeometry50LA;
class OriginalPauseMaterialAdmission50LA;
class OriginalPauseNativeResources50LA;
class OriginalPauseNativeDraw50LA;
class OriginalPausePolygonGeometry50LA;
// Concrete cold storage for shared Pause load1B8810 and derived1BA390:
// original textures, polygons and texts with canonical references. No Entity attachment
// or draw API. Progress stages are not original full-Pause-load receipts.
class OriginalPauseStorage50LA final {
    friend class original_pause_observation50LA::Owner;
    friend class OriginalPauseLayout50LA;
    friend class OriginalPauseTextGeometry50LA;
    friend class OriginalPauseTextureGeometry50LA;
    friend class OriginalPauseMaterialAdmission50LA;
    friend class OriginalPauseNativeResources50LA;
    friend class OriginalPauseNativeDraw50LA;
    friend class OriginalPausePolygonGeometry50LA;
    friend struct original_main_entry50KG::Application;
    // Genuine1BA140->141300 prefix, constructed before resource members and
    // released after them. Detached only until the actual race forest is joined.
    struct EntityPrefix {
        std::shared_ptr<OriginalApplicationEntities50LA> application;
        OriginalWreckComponent50LA component;
        bool initialized=false,releasing=false,disabledAfterLoad=false;
        EntityPrefix(std::shared_ptr<OriginalApplicationEntities50LA> a,std::uint64_t session)noexcept
            :application(std::move(a)) {
            if(!application||!application->application10()||!session)return;
            const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&component));
            // Native bounded admission precedes source construction; no invented unwind.
            if(!application->entityAuthority_.canEnroll(component,key))return;
            const auto r=initializeOriginalWreckComponent50LA(component,
                OriginalWreckComponentKind50LA::entity141300,session,key,true,
                application->objectCount423F60()); //Actual1A8AAC A1=1.
            initialized=r.status==OriginalWreckComponentStatus50LA::complete;
            if(initialized&&!application->entityAuthority_.enroll(component))std::terminate();
        }
        bool detached()const noexcept {
            return initialized&&application&&application->application10()&&
                component.kind==OriginalWreckComponentKind50LA::entity141300&&
                component.sessionGeneration&&component.objectKey==
                    static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&component))&&
                component.reference00==(releasing?0u:1u)&&!component.previous10&&
                !component.next14&&!component.firstChild18&&!component.parent1C&&
                !component.mask20&&!component.descendants24&&component.category28==0xffu&&
                (component.flags29&15u)==(disabledAfterLoad?0u:3u);
        }
        bool disable1A8ADC()noexcept {
            if(!detached()||releasing||disabledAfterLoad)return false;
            // Actual table3CF420+1C=1415A0. No children exist in this bounded
            // concrete load; no external recursive callback is skipped.
            component.flags29=std::uint8_t(component.flags29&~3u);
            disabledAfterLoad=true;return true;
        }
        void beginRelease104970()noexcept {
            if(!initialized)return;
            if(releasing||!detached())std::terminate();
            if(!application->entityAuthority_.retire(component))std::terminate();
            --component.reference00;releasing=true;
        }
        ~EntityPrefix(){
            if(!initialized)return;
            if(!releasing)beginRelease104970();
            if(!detached())std::terminate();
            --application->objectCount423F60(); //1BA2C0->1B8730->1413E0.
        }
    };
    std::unique_ptr<EntityPrefix> entity_;
public:
    enum class Stage : std::uint8_t { fresh,baseBackdrops,basePolygons,baseText,derivedResources,failed,closed };
    struct Diagnostic {
        std::uint32_t sourcePC=0;
        std::size_t object=0;
        assets::OriginalStandardMaterialDiagnostic50LA material{};
        assets::OriginalApplicationFontDiagnostic50LA font{};
    };
    struct PacketVertex {
        std::array<std::uint32_t,4> rgba{};
        std::array<float,4> xyz{},stq{};
    };
    struct Packet {std::array<PacketVertex,4> vertices{};};
    struct Primitive {Packet* packet10=nullptr;std::size_t reference=0;};
    struct Texture {
        //2DEF40 defaults, then2DF0B0 writes dimensions and packet resource data.
        std::unique_ptr<Primitive> primitive54;
        std::unique_ptr<Packet> buffer58;
        const assets::OriginalStandardTextureResource50LA* atlas=nullptr;
        float x10=0,y14=0,field30=1,width34=0,height38=0,field3C=0,field40=1;
        float invWidth4C=1,invHeight50=1;
        std::uint32_t color44=0xffffff;
        std::int16_t alpha48=255;
        std::uint8_t flags5C=0;
    };
    struct PolygonVertex {
        std::array<float,4> authored00{{0,0,1,1}}; //2E2190.
        std::array<float,4>* stq14=nullptr;
        std::array<std::uint32_t,4>* rgba1C=nullptr;
        std::array<float,4>* xyz24=nullptr;
    };
    struct Polygon {
        //2E1920 constructor state; only2E1AE0 creates the below arrays.
        std::unique_ptr<PolygonVertex[]> vertices00;
        std::uint32_t count04=0,count08=0,type0C=2;
        float scale10=1,field14=0,field18=1;
        std::array<float,4> offset20{{0,0,0,1}};
        std::unique_ptr<std::unique_ptr<Primitive>[]> primitives30;
        std::unique_ptr<Packet[]> packets44;
        std::uint32_t count34=0,owns38=0,dirty3C=0,count48=0,capacity4C=0;
        std::uint8_t flags40=3;
    };
    struct TextVertex {
        std::array<float,4> authored00{{0,0,1,1}}; //2DFBA0.
        PacketVertex* packet=nullptr;
    };
    struct Glyph {std::array<TextVertex,2> vertices;};
    struct TextPacket {std::unique_ptr<PacketVertex[]> vertices;std::size_t count=0;};
    struct TextPrimitive {TextPacket* packet10=nullptr;std::size_t reference=0;};
    struct Text {
        std::unique_ptr<char[]> string8;
        std::unique_ptr<Glyph[]> glyphsC;
        std::unique_ptr<std::unique_ptr<TextPrimitive>[]> primitives4;
        std::unique_ptr<TextPacket[]> packets64;
        std::shared_ptr<const assets::OriginalApplicationFonts50LA::Entry> font4C;
        std::uint32_t count0=0,justification50=0,color54=0;
        std::uint8_t capacity5B=0,length5C=0,packetCount5D=0,active5E=0,flags5A=3;
        std::int16_t alpha58=-1;
        float x10=0,y14=0,x20=0,y24=0,field30=0,field34=0,field40=1,field44=0,field48=1;
    };
    // Resource-only cold construction remains available; it has no Entity
    // and cannot be admitted by the new actual Application ownership route.
    OriginalPauseStorage50LA()noexcept=default;
    OriginalPauseStorage50LA(const OriginalPauseStorage50LA&)=delete;
    OriginalPauseStorage50LA& operator=(const OriginalPauseStorage50LA&)=delete;
    ~OriginalPauseStorage50LA(){closeCold();}

    bool loadBaseBackdrops1B8810(
        std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context,
        std::shared_ptr<const assets::OriginalPauseSourceData50LA> data,
        const std::string& actualDirectory36B970,std::uint32_t actualVideoMaximum60,
        Diagnostic& d)noexcept {
        d={};
        if(stage_!=Stage::fresh)return false;
        if(!domain||!domain->prepared()||!context||!context->live()||!data||
           !data->sourceLoaded()||actualVideoMaximum60!=0xffffffu)
            return stop(d,0x1B8810);
        domain_=std::move(domain);context_=std::move(context);data_=std::move(data);
        try {
            for(std::size_t i=0;i<3;++i){
                d.object=i;auto& t=backdrops_[i];
                t.primitive54=std::make_unique<Primitive>();t.primitive54->reference=i;
                if(!references_.acquireNext(domain_,context_->materialApplication(),data_,
                        actualDirectory36B970,d.material))return stop(d,0x2DF11C);
                //Source2DF144/150: actual whole-table material then texture
                //population after EACH ScreenTexture, without collector rebuild.
                if(!domain_->populateTimeStatsResources2DF144(context_,d.material))
                    return stop(d,0x2DF144);
                if(!resolveBackdrop(i,t.atlas,d))return stop(d,0x2DF174);
                const auto* w=t.atlas->sourceWidth14();const auto* h=t.atlas->sourceHeight18();
                if(!w||!h||*w<=0||*h<=0)return stop(d,0x2DF180);
                t.width34=static_cast<float>(*w);t.height38=static_cast<float>(*h);
                t.invWidth4C=1.f/t.width34;t.invHeight50=1.f/t.height38;
                t.buffer58=std::make_unique<Packet>();t.primitive54->packet10=t.buffer58.get();
                const float z=static_cast<float>(actualVideoMaximum60)-t.field40;
                for(std::size_t v=0;v<4;++v){auto& p=t.buffer58->vertices[v];
                    p.rgba={{127,127,127,63}};p.xyz={{0,0,z,1}};
                    p.stq={{(v&1)?1.f:0.f,(v&2)?1.f:0.f,1,0}};
                }
                t.flags5C|=1; //2DF510; XY remains dirty/unregenerated.
                if(i<2){
                    setDimension(t,t.width34,640.f);
                    setDimension(t,t.height38,16.f); //1B88D4 /1B8A20.
                    t.field40=1.f; //1B893C /1B8A88; packetZ explicitly rewritten.
                    for(auto& v:t.buffer58->vertices)v.xyz[2]=static_cast<float>(actualVideoMaximum60)-1.f;
                } else {
                    //1B8AF4..1B8B2C writes original ST directly, before width.
                    const float tenth=std::bit_cast<float>(std::uint32_t{0x3dcccccd});
                    for(std::size_t v=0;v<4;++v){
                        t.buffer58->vertices[v].stq[0]=(v&1)?tenth:0.f;
                        t.buffer58->vertices[v].stq[1]=(v&2)?tenth:0.f;
                    }
                    setDimension(t,t.width34,640.f);
                }
            }
            stage_=Stage::baseBackdrops;return true;
        }catch(...){return stop(d,0x2DF0B0);}
    }
    bool loadBasePolygons1B8DB0(const std::string& actualDirectory36B970,Diagnostic& d)noexcept {
        d={};if(stage_!=Stage::baseBackdrops)return false;
        try {
            for(std::size_t i=0;i<2;++i){
                d.object=i;polygons_[i]=std::make_unique<Polygon>();auto& p=*polygons_[i];
                p.type0C=1;p.count04=p.count08=4;
                p.vertices00=std::make_unique<PolygonVertex[]>(4);
                p.owns38=p.dirty3C=1;p.count34=p.count48=p.capacity4C=1;
                p.packets44=std::make_unique<Packet[]>(1);
                for(std::size_t v=0;v<4;++v){auto& packet=p.packets44[0].vertices[v];
                    packet.xyz={{0,0,1,0}}; //Common2E27C0 resource initialization.
                    p.vertices00[v].stq14=&packet.stq;
                    p.vertices00[v].rgba1C=&packet.rgba;
                    p.vertices00[v].xyz24=&packet.xyz;
                }
                p.primitives30=std::make_unique<std::unique_ptr<Primitive>[]>(1);
                p.primitives30[0]=std::make_unique<Primitive>();
                p.primitives30[0]->reference=3+i;
                if(!references_.acquireNext(domain_,context_->materialApplication(),data_,
                        actualDirectory36B970,d.material))return stop(d,0x2E20F0);
                p.primitives30[0]->packet10=&p.packets44[0];
                //1B8E50 source133F20(0), followed by direct alpha63 writes.
                for(std::size_t v=0;v<4;++v)*p.vertices00[v].rgba1C={{0,0,0,63}};
            }
            //Only after BOTH complete allocations,1B8EC4..1B9158 writes XY
            //and copies authored vectors to actual+14 aliases. The original
            //initial quads are degenerate horizontal lines; do not add height.
            for(std::size_t i=0;i<2;++i)for(std::size_t v=0;v<4;++v){
                auto& vertex=polygons_[i]->vertices00[v];
                vertex.authored00[0]=(v&1)?640.f:0.f;
                vertex.authored00[1]=i?0.f:448.f;
                *vertex.stq14=vertex.authored00;
            }
            stage_=Stage::basePolygons;return true;
        }catch(...){return stop(d,0x2E1AE0);}
    }
    bool loadBaseText1B8B9C(std::shared_ptr<assets::OriginalApplicationFonts50LA> fonts,
        std::string& actualDirectory36B970,const std::string& actualFontDirectory36B420,
        Diagnostic& d)noexcept {
        d={};if(stage_!=Stage::basePolygons)return false;
        if(!fonts||fonts->terminal()||fonts->domain_!=domain_||fonts->context_!=context_)
            return stop(d,0x1B8BC4);
        fonts_=std::move(fonts);mode1F0_=0; //1B8BA4 before source font load.
        try {
            //Aliasing shared ownership pins the SAME authenticated archive
            //already retained by data_; it neither clones nor remounts bytes.
            std::shared_ptr<const assets::OriginalUiArchive50KG> overlays(data_,data_->overlays());
            if(!fonts_->loadPauseBase1B8BC4(std::move(overlays),actualDirectory36B970,
                    actualFontDirectory36B420,d.font))return stop(d,0x1B8BC4);
            const auto font=fonts_->find282210(assets::OriginalApplicationFonts50LA::Name::denmark16);
            if(!font||!font->metrics().decoded()||!font->texture281F90()||
               font->celIdentity()!=data_->member(assets::OriginalPauseSourceMember50LA::ingameFontCells).identity||
               font->atlasIdentity()!=data_->member(assets::OriginalPauseSourceMember50LA::ingameFontPixels).identity)
                return stop(d,0x1B8BDC);
            auto& t=baseText_;t.flags5A|=8;t.font4C=font;
            t.capacity5B=40;t.packetCount5D=t.active5E=2;
            t.string8=std::unique_ptr<char[]>(new char[41]);
            t.glyphsC=std::make_unique<Glyph[]>(40);
            struct DirectoryScope {
                std::string& actual;std::string saved;
                explicit DirectoryScope(std::string& a):actual(a),saved(a){actual="font\\";}
                ~DirectoryScope(){actual.swap(saved);}
            };
            {
                DirectoryScope directory(actualDirectory36B970);
                t.count0=2;t.primitives4=std::make_unique<std::unique_ptr<TextPrimitive>[]>(2);
                t.packets64=std::make_unique<TextPacket[]>(2);
                for(std::size_t p=0;p<2;++p){auto& packet=t.packets64[p];packet.count=p?8:72;
                    packet.vertices=std::make_unique<PacketVertex[]>(packet.count);
                    for(std::size_t v=0;v<packet.count;++v){
                        packet.vertices[v].xyz={{0,0,1,0}};
                        t.glyphsC[p*36+v/2].vertices[v%2].packet=&packet.vertices[v];
                    }
                    t.primitives4[p]=std::make_unique<TextPrimitive>();t.primitives4[p]->reference=5+p;
                    if(!references_.acquireNext(domain_,context_->materialApplication(),data_,
                            actualDirectory36B970,d.material))return stop(d,0x2DFAB0);
                    t.primitives4[p]->packet10=&packet;
                }
            } //2DFB10 restores the real directory even on partial failure.
            t.string8[0]=0;t.length5C=0;t.alpha58=255;t.color54=0xffffffff;
            for(std::size_t p=0;p<2;++p)for(std::size_t v=0;v<t.packets64[p].count;++v)
                t.packets64[p].vertices[v].rgba={{127,127,127,63}};
            //2E0108..144 preserves bit3; it sets bits0/1 and clears bit2.
            t.flags5A=static_cast<std::uint8_t>((t.flags5A|3u)&~4u);
            //1B8C50/5C,1B8CA4,1B8D30,1B8D58. These are load values;
            //positions remain dirty and no glyph draw projection runs here.
            t.x10=320;t.y14=200;t.justification50=3;t.color54=0x00cccccc;
            t.field40=std::bit_cast<float>(std::uint32_t{0x3f99999a});t.flags5A|=3;
            for(std::size_t p=0;p<2;++p)for(std::size_t v=0;v<t.packets64[p].count;++v){
                auto& c=t.packets64[p].vertices[v].rgba;c[0]=c[1]=c[2]=102;
            }
            stage_=Stage::baseText;return true;
        }catch(...){return stop(d,0x2DFD90);}
    }
    //Caller supplies the actual Application+2DC subtype11258 sampled at this
    //load boundary, not a host menu selection. No route is activated here.
    bool loadDerived1BA390(std::string& actualDirectory36B970,
        const std::string& actualFontDirectory36B420,std::int32_t actualSubtype11258,
        std::uint32_t actualVideoMaximum60,Diagnostic& d)noexcept {
        d={};if(stage_!=Stage::baseText)return false;
        if(!fonts_||fonts_->terminal()||!context_||!context_->live()||actualVideoMaximum60!=0xffffffu)
            return stop(d,0x1BA390);
        try {
            std::shared_ptr<const assets::OriginalUiArchive50KG> overlays(data_,data_->overlays());
            if(!fonts_->loadPauseDerived1BA3E4(std::move(overlays),actualDirectory36B970,
                    actualFontDirectory36B420,d.font))return stop(d,0x1BA3E4);
            if(!loadAuthoredTexture(icon_,7,actualDirectory36B970,actualVideoMaximum60,d))return false;
            icon_.x10=std::bit_cast<float>(std::uint32_t{0x43e70000});
            icon_.y14=std::bit_cast<float>(std::uint32_t{0x43bf0000});
            icon_.flags5C|=1;if(icon_.field3C!=0)icon_.flags5C|=2;
            const auto font=fonts_->find282210(assets::OriginalApplicationFonts50LA::Name::denmark16);
            if(!font||font!=baseText_.font4C)return stop(d,0x1BA4AC);
            if(!loadDerivedText(derivedTexts_[0],60,8,font,actualDirectory36B970,d))return false;
            auto& title=derivedTexts_[0];title.justification50=3;setTextColor(title,0x00e5d96c);
            title.field40=std::bit_cast<float>(std::uint32_t{0x3f99999a});title.flags5A|=3;
            if(!loadDerivedText(derivedTexts_[1],40,10,font,actualDirectory36B970,d))return false;
            auto& prompt=derivedTexts_[1];prompt.justification50=0;
            prompt.field40=std::bit_cast<float>(std::uint32_t{0x3f99999a});prompt.flags5A|=3;
            prompt.x10=std::bit_cast<float>(std::uint32_t{0x43f68000});
            prompt.y14=std::bit_cast<float>(std::uint32_t{0x43c28000});prompt.flags5A|=3;
            if(!assignLocalized(prompt,4,d))return false;
            prompt.field40=1;prompt.flags5A|=3;setTextColor(prompt,0x00669bcc);
            for(std::size_t i=0;i<11;++i){auto& t=derivedTexts_[2+i];
                if(!loadDerivedText(t,40,12+2*i,font,actualDirectory36B970,d))return false;
                t.justification50=3;t.field40=std::bit_cast<float>(std::uint32_t{0x3f99999a});t.flags5A|=3;
            }
            if(!assignLocalized(derivedTexts_[2],8,d)||!assignLocalized(derivedTexts_[3],401,d)||
               !assignLocalized(derivedTexts_[11],424,d))return false;
            if(actualSubtype11258==3)assignText(derivedTexts_[12],"END TIME TRIAL",14);
            else if(!assignLocalized(derivedTexts_[12],425,d))return false;
            for(std::size_t i=0;i<2;++i){
                if(!loadAuthoredTexture(arrows_[i],34+i,actualDirectory36B970,actualVideoMaximum60,d))return false;
                auto& t=arrows_[i];
                if(t.color44!=0x00ffffff){
                    for(auto& v:t.buffer58->vertices)v.rgba[0]=v.rgba[1]=v.rgba[2]=127;
                    t.color44=0x00ffffff;
                }
            }
            subtype11258_=actualSubtype11258;
            stage_=Stage::derivedResources;return true;
        }catch(...){return stop(d,0x1BA390);}
    }
    // Source1BAE00 leaf only. The caller must bind actual Application+0C and
    // +264, not a host pause/menu flag. No Entity141C30 receipt is published.
    bool demandResources1BAE00(std::uint32_t actualCameraIndex0C,
        std::uint32_t actualCameraCount264,Diagnostic& d)noexcept {
        d={};if(stage_!=Stage::derivedResources)return false;
        if(!context_||!context_->live())return stop(d,0x1BAE00);
        // Source addiu wraps the32-bit count, then xor/sltiu tests equality.
        if(actualCameraIndex0C!=actualCameraCount264-1u)return true;
        try {
            const auto font=fonts_->find282210(assets::OriginalApplicationFonts50LA::Name::denmark16);
            bool written=false;
            if(font){
                if(font!=baseText_.font4C)return stop(d,0x1BAE44);
                if(!demandTexture(font->texture408_,font->texture281F90(),0x1BAE90,written,d))return false;
            }
            if(!demandScreenTexture(7,icon_.atlas,0x1BAEC0,written,d))return false;
            //1BAF10 bc1tl targets1BAF24: taken delay slot1BAF14 increments
            //once and skips1BAF20; untaken annuls14 and increments20.
            for(std::size_t i=0;i<2;++i){
                if(!demandScreenTexture(i,backdrops_[i].atlas,0x1BAF18,written,d))return false;
            }
            return true;
        }catch(...){return stop(d,0x1BAE00);}
    }
    Stage stage()const noexcept{return stage_;}
    const Texture* backdrop(std::size_t i)const noexcept{return i<3?&backdrops_[i]:nullptr;}
    //This bounded owner cannot submit any geometry, so it has no GPU users.
    //Future drawable extension must replace this cold-only release with actual
    //retirement gating BEFORE exposing a frame/submission API.
    void closeCold()noexcept {
        if(materialAdmissionBorrows_)std::terminate();
        if(stage_==Stage::closed)return;
        if(entity_)entity_->beginRelease104970();
        //1BA2C0 derived destruction precedes shared1B8730. Array text
        //destruction reverses object order, each text releases batches forward.
        closeTexture(icon_,7);
        for(std::size_t i=11;i>0;--i)closeText(derivedTexts_[1+i],12+2*(i-1));
        closeText(derivedTexts_[1],10);closeText(derivedTexts_[0],8);
        closeTexture(arrows_[1],35);closeTexture(arrows_[0],34);
        //Base1B8768 loop releases +38 then+3C before base text/textures.
        for(std::size_t i=0;i<2;++i){references_.release299AC0(3+i);polygons_[i].reset();}
        //1B8794 ->2E1340/2DFC60: string/glyph storage, primitives in
        //ascending order, then packet storage. Font is a native lifetime borrow.
        baseText_.string8.reset();baseText_.glyphsC.reset();
        for(std::size_t i=0;i<2;++i){
            references_.release299AC0(5+i);
            if(baseText_.primitives4)baseText_.primitives4[i].reset();
        }
        baseText_.primitives4.reset();baseText_.packets64.reset();baseText_.font4C.reset();
        //Base1B87AC array destruction of+A0/+100, then+40 at1B87B8.
        for(auto i:std::array<std::size_t,3>{{1,0,2}}){
            references_.release299AC0(i);
            backdrops_[i].primitive54.reset();backdrops_[i].buffer58.reset();backdrops_[i].atlas=nullptr;
        }
        fonts_.reset();stage_=Stage::closed;entity_.reset();
    }
private:
    OriginalPauseStorage50LA(std::shared_ptr<OriginalApplicationEntities50LA> application,
        std::uint64_t session):entity_(std::make_unique<EntityPrefix>(std::move(application),session)) {
        if(!entity_->initialized)stage_=Stage::failed;
    }
    // Geometry/state segment1B9324..1B9C20 only. The eventual original parent
    // must admit the last actual Application camera before calling this, then
    // execute the1B9C24..1B9F3C draw tail in source order. Not a callback receipt.
    bool advanceGeometry1B9324(const OriginalApplicationAnimationClockView50KG& clock,
        Diagnostic& d)noexcept {
        d={};OriginalApplicationUpdateClockSample50KG sample;
        if(stage_!=Stage::derivedResources||!context_||!context_->live()||
           !clock.readUpdate250(sample)){d.sourcePC=0x1B9324;return false;}
        // Concrete storage was created by the original kind1 four-vertex load.
        // Validate before mutation so a failed cold owner cannot partially tick.
        for(const auto& p:polygons_)if(!p||p->type0C!=1||p->count04!=4||!p->vertices00){
            d.sourcePC=0x1B9324;return false;
        }
        for(const auto& t:backdrops_)if(!t.buffer58){d.sourcePC=0x1B9324;return false;}
        if(opening30_>=0.f){
            const float ratio=opening30_/.25f;
            const float progress=1.f-ratio;
            animateBackdrops1B938C(progress);
            const float band=65.f*progress;
            const float y=448.f-band;
            polygonY1B95E0(*polygons_[0],0,y);
            polygonY1B95E0(*polygons_[0],1,y);
            advanceCountdown1B971C(opening30_,sample.elapsed250);
        }else if(transition164_>=0.f){
            const float ratio=transition164_/.25f;
            const float progress=hidden160_?ratio:1.f-ratio;
            animateBackdrops1B938C(progress);
            const float inverse=1.f-progress;
            const float y=65.f*inverse;
            polygonY1B95E0(*polygons_[1],2,y);
            polygonY1B95E0(*polygons_[1],3,y);
            advanceCountdown1B971C(transition164_,sample.elapsed250);
        }
        //1B9BF4..1B9C20 runs even when both countdowns are negative.
        for(auto& p:polygons_){p->field18=1.f;p->dirty3C=1;}
        return true;
    }
    // Exact shared callback1BA120. Its source callers/parent input dispatch
    // remain unbound; there is intentionally no host-visible toggle or setter.
    void reveal1BA120()noexcept {transition164_=.25f;hidden160_=0;}
    static void advanceCountdown1B971C(float& value,float actual250)noexcept {
        if(value>0.f){value=value-actual250;if(value<=0.f)value=0.f;}
        else if(value==0.f)value=-1.f;
    }
    static void polygonY1B95E0(Polygon& p,std::size_t index,float y)noexcept {
        auto& v=p.vertices00[index];v.authored00[1]=y;
        // Concrete kind1 has one STQ alias; no optional source+18 alias was
        // allocated by2E1AE0 for these polygons. Preserve authored X/Z/W.
        *v.stq14=v.authored00;p.dirty3C=1;
    }
    void animateBackdrops1B938C(float progress)noexcept {
        const float scaled=128.f*progress;
        const float extent=std::min(128.f,scaled); //1B9368/1B97E0 MIN.S; no lower clamp.
        const float top=196.f-extent,bottom=196.f+extent;
        auto& center=backdrops_[2];
        setDimension(center,center.x10,0.f);setDimension(center,center.y14,top);
        setDimension(center,center.height38,bottom-top);
        center.field40=1.f;
        for(auto& v:center.buffer58->vertices)v.xyz[2]=16777214.f;
        auto& upper=backdrops_[0];
        setDimension(upper,upper.x10,0.f);setDimension(upper,upper.y14,top-16.f);
        auto& lower=backdrops_[1];
        setDimension(lower,lower.x10,0.f);setDimension(lower,lower.y14,bottom);
    }
    bool refreshTitle1BB010(std::uint8_t actualRaceFlags1B0,
        std::size_t actualTrackTextId,Diagnostic& d)noexcept {
        d={};if(stage_!=Stage::derivedResources||controller884_>=2)return false;
        try {
            const auto* track=data_->english()->at(actualTrackTextId);
            if(!track)return stop(d,0x187390);
            std::string bytes;
            if((actualRaceFlags1B0&3u)==1u)bytes=track->text; //3ACAD0 '%s'.
            else {
                const auto* prefix=data_->english()->at(controller884_?173:172);
                if(!prefix)return stop(d,0x18AC60);
                bytes=prefix->text+" - "+track->text; //3ACAD8 '%s - %s'.
            }
            assignText(derivedTexts_[0],bytes.data(),bytes.size());return true;
        }catch(...){return stop(d,0x1BB010);}
    }
    // Only the actual Application observation facet may refresh this bounded
    // settings segment. It is not the complete1BB010 render callback.
    bool refreshSettings1BB590(const ControlPresets49D& controls,
        const OriginalUiGameSettingsProfile50KG& profile,Diagnostic& d)noexcept {
        d={};if(stage_!=Stage::derivedResources||!context_||!context_->live())return false;
        if(controller884_>=controlPresetSlots49D||!validOriginalUiGameSettingsProfile50KG(profile))
            return stop(d,0x1BB590);
        try {
            const auto& slot=controls.slots[controller884_];
            // Invalid switch indices leave the source stack string unspecified;
            // reject that unsupported state rather than inventing a label.
            if(slot.field10F94>=8||slot.field10F9C>=8)return stop(d,0x1BB5B4);
            const auto text=[&](std::size_t id)->const std::string* {
                const auto* e=data_->english()->at(id);return e?&e->text:nullptr;
            };
            const auto join=[&](std::size_t row,std::size_t label,std::size_t value)->bool {
                const auto* a=text(label);const auto* b=text(value);
                if(!a||!b)return stop(d,0x18AC60);
                const auto bytes=*a+" "+*b;assignText(derivedTexts_[2+row],bytes.data(),bytes.size());return true;
            };
            constexpr std::size_t firstValues[]={410,412,413,414,415,416,417,411};
            constexpr std::size_t secondValues[]={410,418,419,420,421,422,423,411};
            const auto option=slot.field10F94;
            if((subtype11258_==2||subtype11258_==3)&&(option==3||option==5||option==6)){
                const auto* label=text(403);if(!label)return stop(d,0x18AC60);
                const auto bytes=*label+(option==3?" LAP/BEST":option==5?" TIME/BEST":" BEST");
                assignText(derivedTexts_[4],bytes.data(),bytes.size());
            }else if(!join(2,403,firstValues[option]))return false;
            if(!join(3,404,secondValues[slot.field10F9C])||
               !join(4,405,slot.field10FA4?2:3))return false;
            for(std::size_t i=0;i<4;++i){
                const auto* label=text(406+i);if(!label)return stop(d,0x18AC60);
                // Original3ACB10 '%s %d%%', actual32-bit race audio fields.
                const auto bytes=*label+" "+std::to_string(std::bit_cast<std::int32_t>(profile.volume11020[i]))+"%";
                assignText(derivedTexts_[7+i],bytes.data(),bytes.size());
            }
            return true;
        }catch(...){return stop(d,0x1BB590);}
    }
    bool demandScreenTexture(std::size_t ref,
        const assets::OriginalStandardTextureResource50LA* expected,
        std::uint32_t storePC,bool& written,Diagnostic& d)noexcept {
        const auto* reference=references_.reference(ref);
        const assets::OriginalStandardMaterialObject50LA* material=nullptr;
        assets::OriginalStandardMaterialPassView50LA pass;
        if(!reference||!reference->retained||
           !domain_->materials382B38()->resolve298E60(reference->material00,material,d.material)||
           !material||!material->constructed29B550()||!material->pass4C(0,pass)||
           !pass.mapCount20||!*pass.mapCount20||!pass.texture1A)
            return stop(d,0x2DF8A0);
        return demandTexture((*pass.texture1A)[0],expected,storePC,written,d);
    }
    bool demandTexture(std::int16_t token,
        const assets::OriginalStandardTextureResource50LA* expected,
        std::uint32_t storePC,bool& written,Diagnostic& d)noexcept {
        assets::OriginalStandardTextureResource50LA* actual=nullptr;
        if(!expected||!domain_->textures383400()->resolve298E60(token,actual,d.material.texture)||actual!=expected)
            return stop(d,storePC);
        if(!actual->demandPause1BAE00(storePC,written,d.material.texture))return stop(d,storePC);
        return true;
    }
    bool stop(Diagnostic& d,std::uint32_t pc)noexcept{d.sourcePC=pc;stage_=Stage::failed;return false;}
    static void setDimension(Texture& t,float& field,float value)noexcept {
        if(field==value)return;field=value;t.flags5C|=1;if(t.field3C!=0)t.flags5C|=2;
    }
    void closeTexture(Texture& t,std::size_t ref)noexcept {
        references_.release299AC0(ref);t.primitive54.reset();t.buffer58.reset();t.atlas=nullptr;
    }
    void closeText(Text& t,std::size_t first)noexcept {
        t.string8.reset();t.glyphsC.reset();
        for(std::size_t i=0;i<2;++i){references_.release299AC0(first+i);if(t.primitives4)t.primitives4[i].reset();}
        t.primitives4.reset();t.packets64.reset();t.font4C.reset();
    }
    static void setTextColor(Text& t,std::uint32_t color)noexcept {
        if(t.color54==color)return;
        for(std::size_t p=0;p<2;++p)for(std::size_t v=0;v<t.packets64[p].count;++v){
            auto& c=t.packets64[p].vertices[v].rgba;
            c[0]=((color>>16)&255u)>>1;c[1]=((color>>8)&255u)>>1;c[2]=(color&255u)>>1;
        }
        t.color54=color;
    }
    static void assignText(Text& t,const char* bytes,std::size_t length)noexcept {
        if(t.length5C==length&&!std::memcmp(t.string8.get(),bytes,length))return;
        const auto n=std::min<std::size_t>(length,t.capacity5B);
        t.length5C=static_cast<std::uint8_t>(n);std::memcpy(t.string8.get(),bytes,n);
        t.string8[n]=0;t.flags5A|=3;
    }
    bool assignLocalized(Text& t,std::size_t id,Diagnostic& d)noexcept {
        const auto* entry=data_->english()?data_->english()->at(id):nullptr;
        if(!entry)return stop(d,0x18AC60);
        assignText(t,entry->text.data(),entry->text.size());return true;
    }
    bool loadDerivedText(Text& t,std::uint8_t capacity,std::size_t first,
        std::shared_ptr<const assets::OriginalApplicationFonts50LA::Entry> font,
        std::string& actualDirectory,Diagnostic& d) {
        t.flags5A|=8;t.font4C=std::move(font);t.capacity5B=capacity;t.packetCount5D=t.active5E=2;
        t.string8=std::unique_ptr<char[]>(new char[std::size_t(capacity)+1]);
        t.glyphsC=std::make_unique<Glyph[]>(capacity);
        struct Scope {std::string& actual;std::string old;
            explicit Scope(std::string& a):actual(a),old(a){actual="font\\";}
            ~Scope(){actual.swap(old);}};
        {
            Scope scope(actualDirectory);t.count0=2;
            t.primitives4=std::make_unique<std::unique_ptr<TextPrimitive>[]>(2);
            t.packets64=std::make_unique<TextPacket[]>(2);
            for(std::size_t p=0;p<2;++p){auto& packet=t.packets64[p];packet.count=2*(p?capacity-36:36);
                packet.vertices=std::make_unique<PacketVertex[]>(packet.count);
                for(std::size_t v=0;v<packet.count;++v){
                    packet.vertices[v].xyz={{0,0,1,0}};
                    t.glyphsC[p*36+v/2].vertices[v%2].packet=&packet.vertices[v];
                }
                t.primitives4[p]=std::make_unique<TextPrimitive>();t.primitives4[p]->reference=first+p;
                if(!references_.acquireNext(domain_,context_->materialApplication(),data_,actualDirectory,d.material))
                    return stop(d,0x2DFAB0);
                t.primitives4[p]->packet10=&packet;
            }
        }
        t.string8[0]=0;t.length5C=0;t.alpha58=255;t.color54=0xffffffff;t.flags5A=0x0b;
        for(std::size_t p=0;p<2;++p)for(std::size_t v=0;v<t.packets64[p].count;++v)
            t.packets64[p].vertices[v].rgba={{127,127,127,63}};
        return true;
    }
    bool loadAuthoredTexture(Texture& t,std::size_t ref,const std::string& directory,
        std::uint32_t depth,Diagnostic& d) {
        using namespace assets;d.object=ref;
        t.primitive54=std::make_unique<Primitive>();t.primitive54->reference=ref;
        if(!references_.acquireNext(domain_,context_->materialApplication(),data_,directory,d.material))return stop(d,0x2DF11C);
        if(!domain_->populateTimeStatsResources2DF144(context_,d.material))return stop(d,0x2DF144);
        const auto* reference=references_.reference(ref);const auto spec=references_.source(ref);
        const auto* screen=context_->materialApplication()->templates().get(OriginalUiTemplate50KG::screenMt);
        OriginalStandardMaterialRowView50LA row;const OriginalStandardMaterialObject50LA* object=nullptr;
        if(!reference||!reference->retained||!screen||
           !domain_->materials382B38()->row(reference->material00,row)||
           !row.references08||*row.references08<=0||row.associated30!=screen||
           !domain_->materials382B38()->resolve298E60(reference->material00,object,d.material)||
           !object||!object->constructed29B550()||!object->source()||
           object->textures383400()!=domain_->textures383400().get()||!object->passCount48()||*object->passCount48()<1)
            return stop(d,0x2DF164);
        const auto psm=data_->overlays()->find(std::string("ui\\textures\\")+spec.name+".psm");
        const auto* bytes=object->source()->originalBytes();
        if(!psm.identity||!psm.data||!bytes||bytes->size()!=psm.size||std::memcmp(bytes->data(),psm.data,psm.size))
            return stop(d,0x2DF164);
        OriginalStandardMaterialPassView50LA pass;OriginalStandardTextureRowView50LA texture;
        //Original2DF164/174 reads first pass/map, not a guessed icon/ATM name.
        //All authored passes and animation owners remain on the canonical object.
        if(!object->pass4C(0,pass)||!pass.mapCount20||!*pass.mapCount20||!pass.texture1A||
           (*pass.texture1A)[0]<0||!domain_->textures383400()->row((*pass.texture1A)[0],texture)||
           !texture.object04||!texture.references08||*texture.references08<=0)return stop(d,0x2DF174);
        t.atlas=texture.object04;const auto* w=t.atlas->sourceWidth14();const auto* h=t.atlas->sourceHeight18();
        if(!w||!h||*w<=0||*h<=0)return stop(d,0x2DF180);
        t.width34=static_cast<float>(*w);t.height38=static_cast<float>(*h);
        t.invWidth4C=1.f/t.width34;t.invHeight50=1.f/t.height38;
        t.buffer58=std::make_unique<Packet>();t.primitive54->packet10=t.buffer58.get();
        for(std::size_t v=0;v<4;++v){auto& p=t.buffer58->vertices[v];
            p.rgba={{127,127,127,63}};p.xyz={{0,0,static_cast<float>(depth)-t.field40,1}};
            p.stq={{(v&1)?1.f:0.f,(v&2)?1.f:0.f,1,0}};
        }
        t.flags5C|=1;return true;
    }
    bool resolveBackdrop(std::size_t i,const assets::OriginalStandardTextureResource50LA*& atlas,
        Diagnostic& d)const {
        using namespace assets;
        const auto* ref=references_.reference(i);const auto spec=references_.source(i);
        const auto* screen=context_->materialApplication()->templates().get(OriginalUiTemplate50KG::screenMt);
        OriginalStandardMaterialRowView50LA row;const OriginalStandardMaterialObject50LA* object=nullptr;
        if(!ref||!ref->retained||!screen||!domain_->materials382B38()->row(ref->material00,row)||
           !row.references08||*row.references08<=0||row.associated30!=screen||
           !domain_->materials382B38()->resolve298E60(ref->material00,object,d.material)||
           !object||!object->constructed29B550()||object->source()||
           object->textures383400()!=domain_->textures383400().get()||
           !object->passCount48()||*object->passCount48()!=1)return false;
        const std::string base=std::string("ui\\textures\\")+spec.name;
        const auto pst=data_->overlays()->find(base+".pst");
        const auto* proof=object->pauseBackdropFallbackProvenance();
        if(!proof||proof->origin!=OriginalUiMaterialOrigin50KG::pauseBackdropMissingPsmFallback||
           proof->archive!=data_->overlays()->identity()||proof->fallbackTexture!=pst.identity||
           proof->registeredName!=base||proof->attemptedPsmMember!=base+".psm"||
           proof->creationPC!=0x2475C0||proof->sourceCallerPC!=(i?0x1B898Cu:0x1B8840u)||
           proof->fallbackPageSource||proof->fallbackWidgetSource)return false;
        OriginalStandardMaterialPassView50LA pass;OriginalStandardTextureRowView50LA texture;
        if(!object->pass4C(0,pass)||pass.constructionTemplate!=screen||!pass.mapCount20||
           *pass.mapCount20!=1||!pass.texture1A||(*pass.texture1A)[0]<0||
           pass.animation10||pass.textureAnimation10||
           !domain_->textures383400()->row((*pass.texture1A)[0],texture)||!texture.object04||
           !texture.references08||*texture.references08<=0||texture.object04->animated102070())return false;
        const auto* bytes=texture.object04->originalBytes();
        if(!pst.identity||!pst.data||!bytes||bytes->size()!=pst.size||std::memcmp(bytes->data(),pst.data,pst.size))return false;
        atlas=texture.object04;return true;
    }
    std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<const assets::OriginalPauseSourceData50LA> data_;
    assets::OriginalPausePrimitiveReferences50LA references_;
    std::shared_ptr<assets::OriginalApplicationFonts50LA> fonts_;
    Text baseText_; //Original+180.
    Texture icon_; //+890.
    std::array<Texture,2> arrows_{}; //+210,+270.
    std::array<Text,13> derivedTexts_{}; //+2D0,+340,eleven+3B0.
    //1BA130 initializes both genuine Pause fields to zero. Future input
    //callbacks own changes; no public host-selection/player setter exists.
    std::uint32_t selection880_=0,controller884_=0;
    std::int32_t subtype11258_=0; //Actual value consumed during derived load.
    //1BA1C4/1DC/1E4 constructor stores; parent callbacks alone own transitions.
    float opening30_=.25f,transition164_=0.f;
    std::uint32_t hidden160_=0;
    std::uint32_t materialAdmissionBorrows_=0; // Native CPU lifetime guard, never a guest field.
    //Actual constructor1BA1F4/1FC color words.
    std::uint32_t normalColor16C_=0x00e5d96c,selectedColor170_=0x00cccccc;
    std::uint32_t mode1F0_=0;
    std::array<std::unique_ptr<Polygon>,2> polygons_{};
    std::array<Texture,3> backdrops_{}; //Source+A0,+100,+40 allocation/load order.
    Stage stage_=Stage::fresh;
};
}
