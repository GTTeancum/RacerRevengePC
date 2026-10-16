#pragma once
#include "../assets/original_pause_primitive_references50la.h"
#include <array>
#include <bit>
#include <memory>
#include <utility>

namespace rr::game {
// Concrete cold storage for shared Pause load1B8810's three ScreenTextures
// and1B8DB0's two original polygons. No Entity attachment, text completion or
// draw API. Progress stages are not original full-load completion receipts.
class OriginalPauseStorage50LA final {
public:
    enum class Stage : std::uint8_t { fresh,baseBackdrops,basePolygons,failed,closed };
    struct Diagnostic {
        std::uint32_t sourcePC=0;
        std::size_t object=0;
        assets::OriginalStandardMaterialDiagnostic50LA material{};
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
    Stage stage()const noexcept{return stage_;}
    const Texture* backdrop(std::size_t i)const noexcept{return i<3?&backdrops_[i]:nullptr;}
    //This bounded owner cannot submit any geometry, so it has no GPU users.
    //Future drawable extension must replace this cold-only release with actual
    //retirement gating BEFORE exposing a frame/submission API.
    void closeCold()noexcept {
        if(stage_==Stage::closed)return;
        //Base1B8768 loop releases +38 then+3C before base text/textures.
        for(std::size_t i=0;i<2;++i){references_.release299AC0(3+i);polygons_[i].reset();}
        //Base1B87AC array destruction of+A0/+100, then+40 at1B87B8.
        for(auto i:std::array<std::size_t,3>{{1,0,2}}){
            references_.release299AC0(i);
            backdrops_[i].primitive54.reset();backdrops_[i].buffer58.reset();backdrops_[i].atlas=nullptr;
        }
        stage_=Stage::closed;
    }
private:
    bool stop(Diagnostic& d,std::uint32_t pc)noexcept{d.sourcePC=pc;stage_=Stage::failed;return false;}
    static void setDimension(Texture& t,float& field,float value)noexcept {
        if(field==value)return;field=value;t.flags5C|=1;if(t.field3C!=0)t.flags5C|=2;
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
    std::array<std::unique_ptr<Polygon>,2> polygons_{};
    std::array<Texture,3> backdrops_{}; //Source+A0,+100,+40 allocation/load order.
    Stage stage_=Stage::fresh;
};
}
