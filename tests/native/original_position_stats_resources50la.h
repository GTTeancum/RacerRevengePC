#pragma once
#include "original_position_stats_primitives50la.h"
#include "original_position_stats_adapter50la.h"
#include "../../src/game/original_position_stats_numeric50la.h"
#include "../../src/game/original_position_stats_clip50la.h"
#include "../../src/game/original_position_stats_loader_data50la.h"
#include "../../src/game/original_ui_text_position50kg.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <optional>
#include <cstring>

namespace rr::game {
struct OriginalPositionStatsResourceDiagnostic50LA {
    enum class Error:std::uint8_t {none,phase,entity,material,font,allocation,renderer} error=Error::none;
    std::uint32_t sourcePC=0,offset=0;
    assets::OriginalStandardMaterialDiagnostic50LA material;
    platform::VehicleWindowDiagnostic50DS renderer;
    OriginalCameraStartResult drain;
    OriginalPositionStatsPrimitiveDiagnostic50LA primitive;
};
//Detached cold18FF80/190190 resource owner with Application-mediated scalar
//observation. No race+CC hierarchy, demand, collector, predraw or draw API.
//Renderer must NEVER strongly retain it.
class OriginalPositionStatsResources50LA final {
    friend struct original_main_entry50KG::Application;
    using Domain=assets::OriginalStandardMaterialDomain50LA;
    using Context=assets::OriginalStandardMaterialContext50LA;
    using Archive=assets::OriginalUiArchive50KG;
    using Fonts=assets::OriginalApplicationFonts50LA;
    using D=OriginalPositionStatsResourceDiagnostic50LA;
    using E=D::Error;
    struct Entity {
        std::shared_ptr<OriginalApplicationEntities50LA> application;
        OriginalWreckComponent50LA component;
        std::uint64_t generation=0,key=0;bool initialized=false,released=false;
        Entity(std::shared_ptr<OriginalApplicationEntities50LA> a,std::uint64_t g)noexcept
            :application(std::move(a)),generation(g),key(std::uint64_t(reinterpret_cast<std::uintptr_t>(&component))) {
            if(!application||!application->application10()||!g||!key)return;
            const auto r=initializeOriginalWreckComponent50LA(component,
                OriginalWreckComponentKind50LA::entity141300,g,key,true,application->objectCount423F60());
            initialized=r.status==OriginalWreckComponentStatus50LA::complete;
        }
        bool detached()const noexcept {
            return initialized&&!released&&component.kind==OriginalWreckComponentKind50LA::entity141300&&
                component.sessionGeneration==generation&&component.objectKey==key&&component.reference00==1&&
                !component.previous10&&!component.next14&&!component.parent1C&&!component.firstChild18&&
                !component.mask20&&!component.descendants24&&component.category28==0xff&&
                (component.flags29&15u)==3u;
        }
        bool close1413E0()noexcept {
            if(released||!initialized){released=true;return true;}
            if(!detached())return false;
            --component.reference00;--application->objectCount423F60();released=true;return true;
        }
        ~Entity(){if(initialized&&!released)std::terminate();}
    };
    struct Material {
        std::shared_ptr<assets::OriginalStandardMaterialTable50LA> table;
        std::int32_t row=-1;bool retained=false;
        ~Material(){if(retained)std::terminate();}
    };
    struct PacketVertex {
        std::array<std::uint32_t,4> rgba{};
        std::array<float,4> xyz{{0,0,1,0}},stq{}; //2E03F0 text packet starts STQ all0.
    };
    struct SourceVertex {
        std::array<float,4> authored{{0,0,1,1}};
        PacketVertex* packet=nullptr; //2DFBA0/2E03F0 first-copy alias, no overlap here.
    };
    struct Glyph {std::array<SourceVertex,2> vertices;}; //Original stride60 pair.
    struct Packet {std::unique_ptr<PacketVertex[]> vertices;std::size_t count=0,activeVertices=0;};
    struct Primitive {Material material;Packet* packet10=nullptr;};
    struct Text {
        //2E02F0 constructor; storage is allocated only by2DFD90/2DF8E0.
        std::unique_ptr<char[]> string8;
        std::unique_ptr<Glyph[]> glyphsC;
        std::unique_ptr<std::unique_ptr<Primitive>[]> primitives4;
        std::unique_ptr<Packet[]> packets64;
        std::shared_ptr<const Fonts::Entry> font4C;
        std::uint32_t count0=0,justification50=0,color54=0;
        std::uint8_t capacity5B=0,length5C=0,packetCount5D=0,active5E=0,flags5A=3;
        std::int16_t alpha58=-1;
        float x10=0,y14=0,x20=0,y24=0,field30=0,field34=0,field40=1,field44=0,field48=1;
    };
    struct Texture {
        std::unique_ptr<Primitive> primitive54;
        std::unique_ptr<Packet> buffer58;
        const assets::OriginalStandardTextureResource50LA* atlas=nullptr;
        float x10=0,y14=0,field30=1,width34=0,height38=0,field3C=0,field40=1,invWidth4C=1,invHeight50=1;
        std::uint32_t color44=0xffffff;std::int16_t alpha48=255;std::uint8_t flags5C=0;
    };
    //Retained before construction, released only after real Entity retirement.
    std::unique_ptr<original_main_entry50KG::PositionStatsTextureLease50LA> lease_;
    Entity entity_;
    std::shared_ptr<Domain> domain_;
    std::shared_ptr<const Context> context_;
    std::shared_ptr<const Archive> overlays_,localization_;
    std::shared_ptr<const assets::OriginalUiEnglish50KG> english_;
    std::array<Text,5> texts_{}; //Original order:5,9,118,clip.
    std::array<Texture,9> fixed_{};
    std::array<Texture,118> map_{};
    std::optional<OriginalPositionStatsClip50LA> clip_{std::in_place};
    std::shared_ptr<OriginalPositionStatsPrimitiveOwner50LA> primitives_;
    const void* race30_=nullptr; //Detached: no fabricated original race+CC/30 join.
    float field32B0_=0,field32B4_=0;
    std::array<float,4> field32C0_{{0,0,0,1}}; //3C9790 initializes433EB0.
    float field32D0_=0,field32D4_=0,field32D8_=0;
    float field32B8_=7,field32DC_=0,field32E0_=0,field32E4_=0,field32E8_=0;
    enum class Stage:std::uint8_t {fresh,textures,texts,complete,failed,closing,closed};
    Stage stage_=Stage::fresh;
    OriginalPositionStatsResources50LA(std::unique_ptr<original_main_entry50KG::PositionStatsTextureLease50LA> lease,
        std::shared_ptr<OriginalApplicationEntities50LA> entities,std::uint64_t generation,
        std::shared_ptr<Domain> domain,std::shared_ptr<const Context> context,std::shared_ptr<const Archive> overlays)noexcept
        :lease_(std::move(lease)),entity_(std::move(entities),generation),domain_(std::move(domain)),
         context_(std::move(context)),overlays_(std::move(overlays)) {
        if(!lease_||!lease_->constructedFixedResources18FF80())std::terminate();
    }
    bool stop(D& d,E error,std::uint32_t pc)noexcept {d.error=error;d.sourcePC=pc;stage_=Stage::failed;return false;}
    static bool sameBytes(const std::vector<std::uint8_t>* bytes,const assets::OriginalUiMemberView50KG& member)noexcept {
        return bytes&&member.identity&&member.data&&bytes->size()==member.size&&
            (!member.size||!std::memcmp(bytes->data(),member.data,member.size));
    }
    bool acquire(Material& m,const std::string& directory,const char* name,D& d) {
        const auto table=domain_->materials382B38();
        if(m.table||!table||table->populationTerminal()||table->demandTerminal())return stop(d,E::phase,0x299be0);
        std::string path=directory;if(path.empty())return stop(d,E::material,0x299be0);
        if(path.back()!='\\')path+='\\';path+=name;
        const auto psm=overlays_->find(path+".psm");
        const auto provider=domain_->archiveProvider3E1A70();
        assets::OriginalStandardArchiveProvider50LA::MemberView mounted;
        if(!psm.identity||!psm.data||!provider||!provider->sealed()||
           provider->lookup132820(path+".psm",mounted)!=assets::OriginalStandardArchiveProvider50LA::Lookup::present||
           !mounted.data||mounted.size!=psm.size||std::memcmp(mounted.data,psm.data,psm.size))return stop(d,E::material,0x132820);
        m.table=table;std::int32_t found=-1;
        const bool lookedUp=table->find298C80(path,found,d.material);m.row=found;
        if(!lookedUp||!table->retainScreen14D590(found,d.material))return stop(d,E::material,0x299be0);
        m.retained=true;return true;
    }
    static void release(Material& m)noexcept {
        if(!m.retained)return;
        if(!m.table||!m.table->release14D500(m.row))std::terminate();
        m.retained=false;
    }
    bool resolveAtlas(const Material& m,const char* materialName,
        const assets::OriginalStandardTextureResource50LA*& atlas,D& d) {
        const auto* screen=domain_->application_->templates().get(assets::OriginalUiTemplate50KG::screenMt);
        assets::OriginalStandardMaterialRowView50LA row;
        const assets::OriginalStandardMaterialObject50LA* object=nullptr;
        if(!m.retained||!screen||!m.table->row(m.row,row)||!row.references08||*row.references08<=0||row.associated30!=screen||
           !m.table->resolve298E60(m.row,object,d.material)||!object||!object->constructed29B550()||!object->source()||
           object->textures383400()!=domain_->textures383400().get()||!object->passCount48()||*object->passCount48()!=1)
            return stop(d,E::material,0x2df164);
        if(!sameBytes(object->source()->originalBytes(),overlays_->find(std::string("ui\\textures\\")+materialName+".psm")))
            return stop(d,E::material,0x2df164);
        assets::OriginalStandardMaterialPassView50LA pass;
        assets::OriginalStandardTextureRowView50LA texture;
        if(!object->pass4C(0,pass)||!pass.mapCount20||*pass.mapCount20!=1||!pass.texture1A||(*pass.texture1A)[0]<0||
           pass.animation10||pass.textureAnimation10||pass.constructionTemplate!=screen||
           !domain_->textures383400()->row((*pass.texture1A)[0],texture)||!texture.object04||
           !texture.references08||*texture.references08<=0||
           !sameBytes(texture.object04->originalBytes(),overlays_->find("ui\\textures\\stats.pst")))
            return stop(d,E::material,0x2df174);
        atlas=texture.object04;return true; //maptop's real first map is stats.
    }
    bool loadTexture(Texture& t,const position_stats_loader50LA::Texture& data,
        const std::string& directory,std::uint32_t depth,D& d) {
        d.offset=data.offset;t.primitive54=std::make_unique<Primitive>();
        if(!acquire(t.primitive54->material,directory,data.material,d))return false;
        //Exact whole-table14D370 then29E340 for EACH of the1272DF0B0 calls.
        if(!domain_->populateTimeStatsResources2DF144(context_,d.material))return stop(d,E::material,0x2df144);
        if(!resolveAtlas(t.primitive54->material,data.material,t.atlas,d))return false;
        const auto* w=t.atlas->sourceWidth14();const auto* h=t.atlas->sourceHeight18();
        if(!w||!h||*w<=0||*h<=0)return stop(d,E::material,0x2df180);
        t.invWidth4C=1.f/static_cast<float>(*w);t.invHeight50=1.f/static_cast<float>(*h);
        t.width34=static_cast<float>(*w);t.height38=static_cast<float>(*h);
        t.buffer58=std::make_unique<Packet>();t.buffer58->count=4;
        t.buffer58->vertices=std::make_unique<PacketVertex[]>(4);t.primitive54->packet10=t.buffer58.get();
        for(auto i=0u;i<4;++i){auto& v=t.buffer58->vertices[i];v.rgba={{127,127,127,63}};
            v.xyz={{0,0,static_cast<float>(depth)-1.f,1.f}};
            v.stq={{std::bit_cast<float>((i&1)?data.buffer90_F0:data.buffer60_C0),
                std::bit_cast<float>((i&2)?data.bufferC4_F4:data.buffer64_94),1,0}};
        }
        //Source loader overrides dimensions after buffer construction; positions
        //stay unregenerated. Native data only, never a guest command packet.
        t.flags5C|=1; //2DF510.
        if(t.width34!=data.width34){t.width34=data.width34;t.flags5C|=1;if(t.field3C==0)t.flags5C|=2;}
        if(t.height38!=data.height38){t.height38=data.height38;t.flags5C|=1;if(t.field3C==0)t.flags5C|=2;}
        return true;
    }
    bool stageTextures(std::string& directory,std::uint32_t depth,D& d)noexcept {
        d={};if(stage_!=Stage::fresh||!entity_.detached()||depth!=0xffffffu)return stop(d,E::phase,0x190190);
        //141C20 at1901B0 is the original no-op identity return.
        try {
            for(std::size_t i=0;i<9;++i)if(!loadTexture(fixed_[i],position_stats_loader50LA::fixedTextures[i],directory,depth,d))return false;
            for(std::size_t i=0;i<118;++i){auto data=position_stats_loader50LA::mapTexture;
                data.offset+=std::uint32_t(i)*0x60;if(!loadTexture(map_[i],data,directory,depth,d))return false;}
            stage_=Stage::textures;return true;
        }catch(...){return stop(d,E::allocation,0x2df0b0);}
    }
    bool stageTexts(const Fonts& fonts,std::string& actualDirectory,D& d)noexcept {
        if(stage_!=Stage::textures||fonts.terminal())return stop(d,E::phase,0x190e7c);
        const auto font=fonts.find282210(Fonts::Name::eurostile13);
        if(!font||!font->metrics().decoded()||font->celIdentity()!=overlays_->find("font\\eurostileboldbold13ingame.cel").identity||
           font->atlasIdentity()!=overlays_->find("font\\eurostileboldbold13ingame.pst").identity||!font->texture281F90())
            return stop(d,E::font,0x190e7c);
        try {
            for(std::size_t i=0;i<5;++i){auto& t=texts_[i];d.offset=0x40+std::uint32_t(i)*0x70;
                t.font4C=font;t.capacity5B=40;t.packetCount5D=t.active5E=2;
                t.string8=std::unique_ptr<char[]>(new char[41]); //Only byte0 is authored below.
                t.glyphsC=std::make_unique<Glyph[]>(40);
                struct DirectoryScope {std::string& actual;std::string old;
                    explicit DirectoryScope(std::string& a):actual(a),old(a){actual="font\\";}
                    ~DirectoryScope(){actual.swap(old);}};
                {
                    DirectoryScope directory(actualDirectory);
                    t.count0=2;t.primitives4=std::make_unique<std::unique_ptr<Primitive>[]>(2);
                    t.packets64=std::make_unique<Packet[]>(2);
                    for(std::size_t p=0;p<2;++p){auto& packet=t.packets64[p];packet.count=p?8:72;
                        packet.vertices=std::make_unique<PacketVertex[]>(packet.count);
                        for(std::size_t v=0;v<packet.count;++v)
                            t.glyphsC[p*36+v/2].vertices[v%2].packet=&packet.vertices[v];
                        t.primitives4[p]=std::make_unique<Primitive>();
                        if(!acquire(t.primitives4[p]->material,actualDirectory,position_stats_loader50LA::font,d))return false;
                        t.primitives4[p]->packet10=&packet;
                    }
                } //2DFB10 restores directory before alpha/RGB setters.
                t.string8[0]=0;t.length5C=0;t.alpha58=255;t.color54=0xffe5d96c;t.flags5A=3;
                for(std::size_t p=0;p<2;++p)for(std::size_t v=0;v<t.packets64[p].count;++v)
                    t.packets64[p].vertices[v].rgba={{114,108,54,63}};
                if(i==1)t.field40=std::bit_cast<float>(std::uint32_t{0x3fd9999a});
                if(i==4)t.justification50=4;
            }
            //191098..191114 arithmetic, with source32B8=7 constructor value.
            field32E0_=45;field32E4_=554;field32E8_=142;
            volatile float product=4.f*field32E0_;volatile float divided=product/720.f;
            field32DC_=divided/field32B8_;
            if(!clip_||!clip_->set119F60(field32E4_-field32E0_,field32E8_-57.f,
                field32E0_+field32E4_,21.f+field32E8_))return stop(d,E::phase,0x191110);
            stage_=Stage::texts;return true;
        }catch(...){return stop(d,E::allocation,0x2df8e0);}
    }
    static void assignText191DB0(Text& t,const char* bytes,std::size_t length)noexcept {
        if(t.length5C==length&&std::memcmp(t.string8.get(),bytes,length)==0)return;
        const auto count=std::min<std::size_t>(length,t.capacity5B);
        t.length5C=static_cast<std::uint8_t>(count);
        std::memcpy(t.string8.get(),bytes,count);t.string8[count]=0;t.flags5A|=3;
    }
    static void justifyText191DB0(Text& t,std::uint32_t value)noexcept {
        if(t.justification50!=value){t.justification50=value;t.flags5A|=3;}
    }
    // Bounded zero-rotation2E0F50 CPU leaf. Reads the actual font281F90 atlas
    // dimensions, just as2E0A88 does; it does not fabricate a decoded pixel
    // image to satisfy a host quad builder. Native upload is still separate.
    bool prepareText2E0F50(std::size_t index,std::uint32_t depth,D& d)noexcept {
        auto& t=texts_[index];d.offset=0x40+std::uint32_t(index)*0x70;
        const auto reject=[&](std::uint32_t pc)noexcept{d.error=E::font;d.sourcePC=pc;return false;};
        if(!t.font4C||!t.string8||!t.glyphsC||!t.packets64||t.length5C>t.capacity5B||
           t.capacity5B!=40||t.string8[t.length5C]||t.field44!=0||t.field40<=0||
           !std::isfinite(t.field40)||(t.justification50!=0&&t.justification50!=4)||
           !std::isfinite(t.x10)||!std::isfinite(t.y14)||depth!=0xffffffu)return reject(0x2e0f50);
        if(!(t.flags5A&3u))return true;
        const auto* atlas=t.font4C->texture281F90();
        if(!atlas||!atlas->sourceWidth14()||!atlas->sourceHeight18()||
           *atlas->sourceWidth14()<=0||*atlas->sourceHeight18()<=0)return reject(0x2e0a88);
        const float width=static_cast<float>(*atlas->sourceWidth14());
        const float height=static_cast<float>(*atlas->sourceHeight18());
        std::array<const assets::OriginalUiGlyph50KG*,40> glyphs{};
        float measured=0,firstHeight=0;bool pixel=false;
        for(std::size_t n=0;n<t.length5C;++n){
            const auto* g=t.font4C->metrics().glyph(static_cast<std::uint8_t>(t.string8[n]));
            if(!g||g->widthMinusOne< -1||g->heightMinusOne< -1)return reject(0x2e0b18);
            if(n==0)pixel=g->hasPixelMetrics;
            else if(pixel!=g->hasPixelMetrics)return reject(0x2e18a8); //Bounded mixed-run rejection.
            glyphs[n]=g;
            measured=measured+(pixel?float(int(g->leftBearing)+int(g->advanceAfterBearing)):(g->right-g->left));
            if(n==0)firstHeight=pixel?float(int(g->heightMinusOne)+1):height*(g->bottom-g->top);
            for(std::size_t v=0;v<2;++v){const auto batch=n/36,vertex=(n%36)*2+v;
                if(!t.packets64[batch].vertices||t.glyphsC[n].vertices[v].packet!=
                    &t.packets64[batch].vertices[vertex])return reject(0x2e03f0);}
        }
        if(!pixel)measured=measured*width;
        float x=t.x20,y=t.y24;
        if(t.flags5A&1u){
            x=t.x10;y=t.y14;
            if(t.length5C&&t.justification50==4){
                std::int32_t w=0,h=0,position=0;
                if(!roundOriginalUiTextMetric50KG(measured,t.field40,w)||
                   !roundOriginalUiTextMetric50KG(firstHeight,t.field40,h)||
                   !roundOriginalUiTextMetric50KG(t.x10-static_cast<float>(w),1.f,position))return reject(0x2e153c);
                x=static_cast<float>(position);
            }
        }
        //2E13A0 sets bit2 when positioning changes;2E0AD0 divides the local
        //copies, not persistent20/24. Preserve original division then multiply.
        const bool inverse=(t.flags5A&5u)!=0;
        float pen=inverse?x/t.field40:x;
        const float localY=inverse?y/t.field40:y;
        struct Pair {std::array<std::array<float,2>,2> xy,uv;};
        std::array<Pair,40> prepared{};
        for(std::size_t n=0;n<t.length5C;++n){const auto& g=*glyphs[n];auto& p=prepared[n];
            if(pixel){
                pen=pen+float(g.leftBearing);
                p.xy[0]={{pen-.5f,localY-.5f}};
                p.xy[1]={{p.xy[0][0]+float(int(g.widthMinusOne)+1),p.xy[0][1]+float(int(g.heightMinusOne)+1)}};
                p.uv[0]={{(g.left*(width-1.f))/width,(g.top*(height-1.f))/height}};
                p.uv[1]={{(g.right*(width-1.f))/width,(g.bottom*(height-1.f))/height}};
                pen=pen+float(g.advanceAfterBearing);
            }else{
                p.xy[0]={{pen,localY}};pen=pen+width*(g.right-g.left);
                p.xy[1]={{pen-1.f,localY+height*(g.bottom-g.top)}};
                p.uv[0]={{g.left,g.top}};p.uv[1]={{g.right,g.bottom}};
            }
            for(std::size_t v=0;v<2;++v)for(std::size_t axis=0;axis<2;++axis)
                if(!std::isfinite(p.xy[v][axis])||!std::isfinite(p.uv[v][axis])||
                   !std::isfinite(t.field40*p.xy[v][axis]))return reject(0x2e1240);
        }
        t.active5E=std::uint8_t((t.length5C+35u)/36u);t.x20=x;t.y24=y;
        for(std::size_t batch=0;batch<2;++batch)t.packets64[batch].activeVertices=
            2*std::min<std::size_t>(36,t.length5C>batch*36?t.length5C-batch*36:0);
        const float z=static_cast<float>(depth)-t.field48; //Source-admitted positive field48=1.
        for(std::size_t n=0;n<t.length5C;++n)for(std::size_t v=0;v<2;++v){
            auto& source=t.glyphsC[n].vertices[v];const auto& p=prepared[n];
            source.authored[0]=p.xy[v][0];source.authored[1]=p.xy[v][1];source.authored[2]=z;
            //2E0A30 writes only S/T. Preserve Q and all RGBA lanes from the
            //actual packet allocation/setters; never synthesize an atlas quad.
            source.packet->stq[0]=p.uv[v][0];source.packet->stq[1]=p.uv[v][1];
            source.packet->xyz=source.authored;
            source.packet->xyz[0]=t.field40*p.xy[v][0];source.packet->xyz[1]=t.field40*p.xy[v][1];
        }
        t.flags5A=std::uint8_t(t.flags5A&~7u);return true;
    }
    bool prepareText191DB0(const OriginalPositionStatsNumbers50LA& observation,std::uint32_t depth,D& d)noexcept {
        bool visible=false;position_stats_detail50LA::Visibility preference;
        if(!english_||!english_->loaded()||!position_stats_detail50LA::ownLeafVisible191BE0(
                observation.viewIndex,observation.raceFlags1B0,observation.raceEntityFlags29,observation.uiFlags1E4,visible)||
           !position_stats_detail50LA::preferenceVisibility17FB90(observation.preference10F9C,
                observation.viewState40,observation.subtype11258,preference)){
            d.error=E::phase;d.sourcePC=0x191be0;return false;}
        if(!visible)return true;
        const auto number=[&](std::size_t index,std::int32_t value,bool slash)noexcept{
            std::array<char,16> bytes{};char* first=bytes.data();if(slash)*first++='/';
            const auto result=std::to_chars(first,bytes.data()+bytes.size(),value);
            if(result.ec!=std::errc{}){d.error=E::phase;d.sourcePC=0x308c90;return false;}
            assignText191DB0(texts_[index],bytes.data(),std::size_t(result.ptr-bytes.data()));return true;
        };
        if(preference.position){
            const auto* label=english_->at(455);if(!label){d.error=E::font;d.sourcePC=0x191e1c;return false;}
            assignText191DB0(texts_[0],label->text.data(),label->text.size());justifyText191DB0(texts_[0],0);
            if(!prepareText2E0F50(0,depth,d)||!number(2,static_cast<std::int32_t>(observation.activeCount50),true))return false;
            justifyText191DB0(texts_[2],0);
            if(!prepareText2E0F50(2,depth,d)||!number(1,observation.placeCCC,false))return false;
            justifyText191DB0(texts_[1],4);if(!prepareText2E0F50(1,depth,d))return false;
        }
        if(preference.countCD0){
            const auto* label=english_->at(457);if(!label){d.error=E::font;d.sourcePC=0x192130;return false;}
            assignText191DB0(texts_[3],label->text.data(),label->text.size());
            if(!prepareText2E0F50(3,depth,d)||!number(4,std::bit_cast<std::int32_t>(observation.viewCountCD0),false)||
               !prepareText2E0F50(4,depth,d))return false;
        }
        return true;
    }
    // Complete191290..191AE8 predraw position/dirty writes. This updates the
    // actual resource fields, never a host panel or replacement geometry.
    // Source191150 obtains3210 from race8C;191BB0 supplies Application268+1.
    bool layout191290(const OriginalPositionStatsNumbers50LA& observation,D& d)noexcept {
        position_stats_detail50LA::LayoutOffsets offsets;
        if(!observationDisplay()||observation.viewIndex!=0||
           !position_stats_detail50LA::layoutOffsets191290(observation.raceMode8C,
                observation.viewIndex+1,offsets)){
            d.error=E::phase;d.sourcePC=0x191290;return false;
        }
        field32B0_=offsets.x;field32B4_=offsets.y;
        constexpr std::array<float,9> x{{485,517,522,554,586,505,546,485,517}};
        constexpr std::array<float,9> y{{52,52,52,52,52,74,134,30,30}};
        for(std::size_t i=0;i<fixed_.size();++i){
            auto& t=fixed_[i];const float nextX=x[i]+field32B0_,nextY=y[i]+field32B4_;
            if(t.x10==nextX&&t.y14==nextY)continue;
            t.x10=nextX;t.y14=nextY;t.flags5C=std::uint8_t((t.flags5C&~1u)|1u);
            if(t.field3C==0.f)t.flags5C=std::uint8_t((t.flags5C&~2u)|2u);
        }
        constexpr std::array<float,5> tx{{500,570,571,500,590}};
        constexpr std::array<float,5> ty{{53,51,53,31,31}};
        for(std::size_t i=0;i<texts_.size();++i){
            auto& t=texts_[i];const float nextX=tx[i]+field32B0_,nextY=ty[i]+field32B4_;
            if(t.x10==nextX&&t.y14==nextY)continue;
            t.x10=nextX;t.y14=nextY;
            t.flags5A=std::uint8_t((t.flags5A&~1u)|1u);
            t.flags5A=std::uint8_t((t.flags5A&~2u)|2u);
        }
        return true;
    }
    const original_main_entry50KG::RaceDisplayLifetime50LA* observationDisplay()const noexcept {
        return stage_==Stage::complete&&entity_.detached()&&lease_&&
            !lease_->retirementStarted_&&primitives_&&primitives_->resourceFragmentLoaded()?
            lease_->owner_.get():nullptr;
    }
    static void destroyTexture(Texture& t)noexcept {
        t.buffer58.reset(); //2DF038 BEFORE clearing10 and destroying54.
        if(t.primitive54){t.primitive54->packet10=nullptr;release(t.primitive54->material);t.primitive54.reset();}
        t.atlas=nullptr;
    }
    static void destroyText(Text& t)noexcept {
        t.string8.reset();t.glyphsC.reset(); //2DFC60:8, C,4,64 in that order.
        if(t.primitives4)for(std::size_t i=0;i<t.count0;++i)if(t.primitives4[i]){
            release(t.primitives4[i]->material);t.primitives4[i].reset();}
        t.primitives4.reset();
        if(t.packets64)for(std::size_t i=0;i<t.packetCount5D;++i)t.packets64[i].vertices.reset();
        t.packets64.reset();t.font4C.reset();
    }
public:
    OriginalPositionStatsResources50LA(const OriginalPositionStatsResources50LA&)=delete;
    OriginalPositionStatsResources50LA& operator=(const OriginalPositionStatsResources50LA&)=delete;
    ~OriginalPositionStatsResources50LA(){if(stage_!=Stage::closed)std::terminate();}
    bool close(D& d)noexcept {
        d={};if(stage_==Stage::closed)return true;stage_=Stage::closing;
        if(primitives_){if(!primitives_->close(d.primitive))return stop(d,E::renderer,d.primitive.sourcePC);primitives_.reset();}
        clip_.reset(); //Embedded119F10, no allocation/drain.
        if(!lease_->beginAfterClip119F10())return stop(d,E::renderer,0x190100);
        while(const auto offset=lease_->nextTextureOffset()){
            d.offset=offset;d.drain=lease_->drainNext2DEFE0(d.renderer);
            if(d.drain.status!=OriginalCameraStartStatus::complete)return stop(d,E::renderer,d.drain.sourcePC);
            if(offset>=0x5d0)destroyTexture(map_[(offset-0x5d0)/0x60]);else destroyTexture(fixed_[(offset-0x270)/0x60]);
            if(!lease_->finishedTextureResources(offset))std::terminate();
        }
        while(const auto offset=lease_->nextTextOffset()){
            d.offset=offset;destroyText(texts_[(offset-0x40)/0x70]);
            if(!lease_->finishedTextResources(offset))std::terminate();
        }
        if(!entity_.close1413E0())return stop(d,E::entity,0x19014c);
        if(!lease_->finishedEntity1413E0())std::terminate();lease_.reset();stage_=Stage::closed;return true;
    }
};
}
namespace original_main_entry50KG {
// A synchronous borrow: all camera/manager checks and bank reads finish before
// return. Neither the resource owner nor Application stores the adapter/camera.
inline bool Application::capturePositionStatsNumbers50LA(
    const rr::game::OriginalPositionStatsResources50LA& resources,
    const original_camera_session50KX::Session& camera,const native_ai50IV::Session& manager,
    rr::game::OriginalPositionStatsNumbers50LA& output,
    rr::game::OriginalPositionStatsResourceDiagnostic50LA& d)const noexcept {
    using Adapter=original_position_stats_adapter50LA::Owner;
    using Error=rr::game::OriginalPositionStatsResourceDiagnostic50LA::Error;
    d={};
    const auto reject=[&]()noexcept{d.error=Error::phase;d.sourcePC=0x191db0;return false;};
    const auto owner=positionStatsResourcesOwner50LA_.lock();
    const auto display=raceDisplayOwner50LA_.lock();
    if(!enabled||owner.get()!=&resources||!display||display->application_!=this||
       display->video_!=videoOwner_||!display->canRetain()||
       resources.observationDisplay()!=display.get()||
       resources.entity_.application!=entitiesOwner_||
       resources.domain_!=standardMaterialDomainOwner_||
       resources.context_!=materialContextOwner_||!materialContextOwner_||
       !materialContextOwner_->live()||materialContextOwner_->videoIdentity()!=videoOwner_.get()||
       !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())return reject();
    const auto table=standardMaterialDomainOwner_->materials382B38();
    if(!table||!Adapter::belongsToApplication(camera,entitiesOwner_.get(),table.get(),
        &materialDirectory36B970_,&display->renderer_,&display->dispatch_))return reject();
    Adapter observation;Adapter::Diagnostic adapterDiagnostic;
    rr::game::OriginalPositionStatsNumbers50LA next;
    if(!observation.bind(camera,manager,adapterDiagnostic)||
       !observation.capture(next,adapterDiagnostic))return reject();
    output=next;return true;
}
inline bool Application::preparePositionStatsLayout50LA(
    rr::game::OriginalPositionStatsResources50LA& resources,
    const original_camera_session50KX::Session& camera,const native_ai50IV::Session& manager,
    rr::game::OriginalPositionStatsResourceDiagnostic50LA& d)const noexcept {
    rr::game::OriginalPositionStatsNumbers50LA observation;
    return capturePositionStatsNumbers50LA(resources,camera,manager,observation,d)&&
        resources.layout191290(observation,d);
}
inline bool Application::preparePositionStatsText50LA(
    rr::game::OriginalPositionStatsResources50LA& resources,
    const original_camera_session50KX::Session& camera,const native_ai50IV::Session& manager,
    rr::game::OriginalPositionStatsResourceDiagnostic50LA& d)const noexcept {
    rr::game::OriginalPositionStatsNumbers50LA observation;
    return capturePositionStatsNumbers50LA(resources,camera,manager,observation,d)&&
        resources.layout191290(observation,d)&&
        resources.prepareText191DB0(observation,std::uint32_t(videoOwner_->packetZMaximum),d);
}
struct PositionStatsResourceResult50LA {
    bool attempted=false,coldResourcesLoaded=false;
    std::shared_ptr<rr::game::OriginalPositionStatsResources50LA> owner;
    rr::game::OriginalPositionStatsResourceDiagnostic50LA diagnostic;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material;
    rr::assets::OriginalUiPageDiagnostic50KG source;
    rr::assets::OriginalApplicationFontDiagnostic50LA font;
    rr::assets::OriginalUiLocalizationDiagnostic50KG localization;
    PositionStatsPrimitiveResult50LA primitive;
    std::string failure;
};
//Uncalled full cold resource sequence. No race/view registration or draw claim.
inline bool Application::stagePositionStatsResources50LA(const std::filesystem::path& originalRoot,
    PositionStatsResourceResult50LA& out)const noexcept {
    using namespace rr::assets;
    if(out.attempted||out.owner)return false;out.attempted=true;
    const auto fail=[&](const char* why)noexcept{try{out.failure=why;}catch(...){}return false;};
    try {
        const auto display=raceDisplayOwner50LA_.lock();
        if(!enabled||!display||display->video_!=videoOwner_||!display->canRetain()||
           !positionStatsResourcesOwner50LA_.expired()||!positionStatsPrimitivesOwner50LA_.expired()||
           !entitiesOwner_||!entitiesOwner_->application10()||!videoOwner_||
           !Video::materialConstructionPinLive(*videoOwner_)||!materialContextOwner_||!materialContextOwner_->live()||
           materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           materialContextOwner_->materialApplication()!=materialApplication||
           !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())
            return fail("PositionStats requires actual live Application/display/domain and no outstanding owner");
        const auto overlays=readArchive(originalRoot,OriginalUiArchiveId50KG::overlays);
        auto lease=display->retainPositionStatsFixedResources();
        if(!lease)return fail("actual fixed-resource display lease rejected");
        //Allocate the shared control block BEFORE source fixed construction.
        //A control-block failure must not invoke127 undrained destructors.
        using Owner=rr::game::OriginalPositionStatsResources50LA;
        struct Allocation {
            alignas(Owner) unsigned char bytes[sizeof(Owner)];Owner* value=nullptr;
            ~Allocation(){if(value)value->~Owner();}
        };
        auto allocation=std::make_shared<Allocation>();
        allocation->value=new (allocation->bytes) Owner(std::move(lease),entitiesOwner_,display->generation(),
            standardMaterialDomainOwner_,materialContextOwner_,overlays);
        out.owner=std::shared_ptr<Owner>(allocation,allocation->value); //No allocation.
        positionStatsResourcesOwner50LA_=out.owner;
        if(!out.owner->entity_.detached())return fail("PositionStats detached Entity constructor rejected");
        const auto domain=standardMaterialDomainOwner_;
        const auto provider=domain->archiveProvider3E1A70();
        if(!provider||!provider->mountAuthenticatedUi(overlays,out.provider)||!provider->sealNativeMounts(out.provider))
            return fail("PositionStats authentic provider mount rejected; retain constructed owner");
        std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>> materials;
        for(const char* path:{"ui\\textures\\stats.psm","ui\\textures\\maptop.psm",
            "font\\eurostileboldbold13ingame.psm","ui\\textures\\RadarOverlay.psm"}) {
            const auto member=overlays->find(path);
            if(!member.identity||!member.data||member.size!=member.identity->bytes)return fail("PositionStats original PSM missing");
            auto source=std::make_shared<OriginalStandardMaterialSource50LA>();
            if(!source->prepare(member.identity->name,std::vector<std::uint8_t>(member.data,member.data+member.size),out.source))
                return fail("PositionStats original PSM parse rejected");
            materials.push_back(std::move(source));
        }
        std::vector<OriginalStandardTextureSourceInput50LA> textures;
        for(const char* path:{"ui\\textures\\stats.pst","font\\eurostileboldbold13ingame.pst"}) {
            const auto member=overlays->find(path);
            if(!member.identity||!member.data||member.size!=member.identity->bytes)return fail("PositionStats original PST missing");
            textures.push_back({member.identity->name,std::vector<std::uint8_t>(member.data,member.data+member.size)});
        }
        if(!domain->appendOriginalSources(materials,textures,out.material))return fail("PositionStats canonical source append rejected");
        struct DirectoryScope {std::string& actual;std::string previous;
            explicit DirectoryScope(std::string& a):actual(a),previous(a){actual="ui\\textures\\";}
            ~DirectoryScope(){actual.swap(previous);}} directory(materialDirectory36B970_);
        if(!out.owner->stageTextures(materialDirectory36B970_,std::uint32_t(videoOwner_->packetZMaximum),out.diagnostic))
            return fail("PositionStats texture stage failed; retain partial owner");
        if(!loadPositionStatsFont190E64(overlays,out.font)||!fonts328Owner_)
            return fail("PositionStats original one-font stage failed; retain partial owner");
        if(!out.owner->stageTexts(*fonts328Owner_,materialDirectory36B970_,out.diagnostic))
            return fail("PositionStats text/clip stage failed; retain partial owner");
        //Native retained localization bridge, matching existing TimeStats/SpeedBoost
        //ownership. Runtime191DB0 still selects the original numeric IDs455/457.
        out.owner->localization_=readArchive(originalRoot,OriginalUiArchiveId50KG::localization);
        auto english=std::make_shared<OriginalUiEnglish50KG>();
        if(!readOriginalUiEnglish50KG(*out.owner->localization_,*english,out.localization))
            return fail("PositionStats authentic English localization rejected; retain partial owner");
        out.owner->english_=std::move(english);
        //Only after127 textures, font,5 texts and119F60: original191118->191180.
        const bool okay=stagePositionStatsPrimitives50LA(originalRoot,out.primitive);
        out.owner->primitives_=out.primitive.owner; //Retain even a failed partial16 array.
        if(!okay)return fail("PositionStats primitive stage failed; retain both actual leases");
        out.owner->stage_=rr::game::OriginalPositionStatsResources50LA::Stage::complete;
        out.coldResourcesLoaded=true;return true;
    }catch(...){return fail("PositionStats resource exception; retain any constructed/partial owner");}
}
}
