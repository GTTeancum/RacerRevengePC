#include "original_results104_storage50la.h"
#include <algorithm>
#include <bit>
#include <cstring>
namespace rr::game {
void OriginalResults10450LA::Impl::dimension(Texture& t,float& field,float value)noexcept {
    if(field==value)return;field=value;t.flags5C|=1;if(t.field3C!=0.f)t.flags5C|=2;
}
void OriginalResults10450LA::Impl::position(Texture& t,float x,float y)noexcept {
    if(t.x10==x&&t.y14==y)return;t.x10=x;t.y14=y;t.flags5C|=1;
    if(t.field3C!=0.f)t.flags5C|=2;
}
void OriginalResults10450LA::Impl::position(Text& t,float x,float y)noexcept {
    if(t.x10==x&&t.y14==y)return;t.x10=x;t.y14=y;t.flags5A|=3;
}
void OriginalResults10450LA::Impl::justification(Text& t,std::uint32_t value)noexcept {
    if(t.justification50==value)return;t.justification50=value;t.flags5A|=3;
}
void OriginalResults10450LA::Impl::scale(Text& t,float value)noexcept {
    if(t.field40==value)return;t.field40=value;t.flags5A|=3;
}
void OriginalResults10450LA::Impl::color(Text& t,std::uint32_t value)noexcept {
    if(t.color54==value)return;
    for(std::size_t i=0;i<t.capacity5B;++i)for(auto& v:t.glyphsC[i].vertices){
        auto& c=v.packet->rgba;c[0]=((value>>16)&255u)>>1;c[1]=((value>>8)&255u)>>1;c[2]=(value&255u)>>1;
    }
    t.color54=value;
}
void OriginalResults10450LA::Impl::color(Texture& t,std::uint32_t value)noexcept {
    if(t.color44==value)return;
    for(auto& v:t.buffer58->vertices){v.rgba[0]=((value>>16)&255u)>>1;
        v.rgba[1]=((value>>8)&255u)>>1;v.rgba[2]=(value&255u)>>1;}
    t.color44=value;
}
void OriginalResults10450LA::Impl::assign(Text& t,const char* bytes,std::size_t length)noexcept {
    if(t.length5C==length&&!std::memcmp(t.string8.get(),bytes,length))return;
    const auto n=std::min<std::size_t>(length,t.capacity5B);
    t.length5C=static_cast<std::uint8_t>(n);std::memcpy(t.string8.get(),bytes,n);
    t.string8[n]=0;t.flags5A|=3;
}
bool OriginalResults10450LA::Impl::localized(Text& t,std::size_t id,D& d) {
    const auto* entry=sourceData&&sourceData->english()?sourceData->english()->at(id):nullptr;
    if(!entry)return stop(d,S::source,0x18ac60,"Results actual localization entry");
    assign(t,entry->text.data(),entry->text.size());return true;
}
bool OriginalResults10450LA::Impl::texture(Texture& t,Role role,std::uint32_t pc,
    std::uint32_t offset,std::size_t ordinal,std::uint32_t pod,const std::string& directory,D& d) {
    d.object=offset;d.sourcePC=pc;
    t.primitive54=std::make_unique<Primitive>();t.primitive54->reference=references.associatedCount();
    if(!references.acquireNext(role,pc,offset,ordinal,pod,directory,materialDiagnostic))
        return stop(d,S::material,materialDiagnostic.sourcePC,"Results ScreenTexture association");
    if(!domain->populateTimeStatsResources2DF144(context,materialDiagnostic))
        return stop(d,S::material,materialDiagnostic.sourcePC,"Results actual whole-table population");
    const auto* ref=references.reference(t.primitive54->reference);
    const assets::OriginalStandardMaterialObject50LA* object=nullptr;
    assets::OriginalStandardMaterialPassView50LA pass;
    assets::OriginalStandardTextureRowView50LA row;
    if(!ref||!ref->retained||!domain->materials382B38()->resolve298E60(ref->material00,object,materialDiagnostic)||
       !object||!object->constructed29B550()||!object->pass4C(0,pass)||!pass.mapCount20||!*pass.mapCount20||
       !pass.texture1A||(*pass.texture1A)[0]<0||!domain->textures383400()->row((*pass.texture1A)[0],row)||
       !row.object04||!row.references08||*row.references08<=0)
        return stop(d,S::material,0x2df174,"Results actual first pass/map texture");
    t.atlas=row.object04;const auto* width=t.atlas->sourceWidth14();const auto* height=t.atlas->sourceHeight18();
    if(!width||!height||*width<=0||*height<=0)return stop(d,S::source,0x2df180,"Results source texture extent");
    t.width34=static_cast<float>(*width);t.height38=static_cast<float>(*height);
    t.invWidth4C=1.f/t.width34;t.invHeight50=1.f/t.height38;
    t.buffer58=std::make_unique<Packet>();t.primitive54->packet10=t.buffer58.get();
    for(std::size_t i=0;i<4;++i){auto& v=t.buffer58->vertices[i];
        v.rgba={{127,127,127,63}};v.xyz={{0,0,static_cast<float>(depth)-t.field40,1}};
        v.stq={{(i&1)?1.f:0.f,(i&2)?1.f:0.f,1,0}};}
    t.flags5C|=1;return true;
}
bool OriginalResults10450LA::Impl::text(Text& t,Role role,std::uint32_t pc,
    std::uint32_t offset,std::size_t ordinal,std::uint8_t capacity,
    const std::shared_ptr<const assets::OriginalApplicationFonts50LA::Entry>& font,
    std::string& directory,const char* initial,D& d) {
    d.object=offset;d.sourcePC=pc;
    if(!font||!font->metrics().decoded()||!font->texture281F90()||!capacity)
        return stop(d,S::font,0x2dfd90,"Results actual registered font");
    t.flags5A|=8;t.font4C=font;t.capacity5B=capacity;
    const auto count=(std::size_t(capacity)+35)/36;
    t.packetCount5D=t.active5E=static_cast<std::uint8_t>(count);
    t.string8=std::unique_ptr<char[]>(new char[std::size_t(capacity)+1]);
    t.glyphsC=std::make_unique<Glyph[]>(capacity);
    struct Scope {std::string& actual;std::string saved;
        explicit Scope(std::string& a):actual(a),saved(a){actual="font\\";}
        ~Scope(){actual.swap(saved);}};
    {
        Scope scope(directory);t.count0=static_cast<std::uint32_t>(count);
        t.primitives4=std::make_unique<std::unique_ptr<TextPrimitive>[]>(count);
        t.packets64=std::make_unique<TextPacket[]>(count);
        for(std::size_t p=0;p<count;++p){auto& packet=t.packets64[p];
            packet.count=2*std::min<std::size_t>(36,std::size_t(capacity)-p*36);
            packet.vertices=std::make_unique<PacketVertex[]>(packet.count);
            for(std::size_t v=0;v<packet.count;++v){packet.vertices[v].xyz={{0,0,1,0}};
                t.glyphsC[p*36+v/2].vertices[v%2].packet=&packet.vertices[v];}
            t.primitives4[p]=std::make_unique<TextPrimitive>();
            t.primitives4[p]->reference=references.associatedCount();
            if(!references.acquireNext(role,pc,offset,ordinal,0,directory,materialDiagnostic))
                return stop(d,S::material,materialDiagnostic.sourcePC,"Results ScreenText primitive association");
            t.primitives4[p]->packet10=&packet;
        }
    } //The SAME36B970 is restored, including host partial failure.
    t.string8[0]=0;t.length5C=0;t.alpha58=255;t.color54=0xffffffffu;
    for(std::size_t p=0;p<count;++p)for(std::size_t v=0;v<t.packets64[p].count;++v)
        t.packets64[p].vertices[v].rgba={{127,127,127,63}};
    t.flags5A=std::uint8_t((t.flags5A|3u)&~4u);
    if(initial)assign(t,initial,std::strlen(initial));return true;
}
bool OriginalResults10450LA::Impl::baseLoad(std::string& directory,const std::string& fontDirectory,D& d) {
    //141C20 is the genuine return-self leaf, not a missing callback.
    for(std::size_t i=0;i<3;++i){auto& t=i<2?(*backdropsA0)[i]:*center40;
        if(!texture(t,i?Role::baseResults2:Role::baseResults1,
            i==0?0x1b8840:i==1?0x1b898c:0x1b8ad8,i==0?0xa0:i==1?0x100:0x40,0,0,directory,d))return false;
        if(i<2){dimension(t,t.width34,640.f);dimension(t,t.height38,16.f);t.field40=1.f;
            for(auto& v:t.buffer58->vertices)v.xyz[2]=static_cast<float>(depth)-1.f;
        }else{const float tenth=std::bit_cast<float>(std::uint32_t{0x3dcccccd});
            for(std::size_t v=0;v<4;++v){t.buffer58->vertices[v].stq[0]=(v&1)?tenth:0.f;
                t.buffer58->vertices[v].stq[1]=(v&2)?tenth:0.f;}dimension(t,t.width34,640.f);}
    }
    for(std::size_t i=0;i<2;++i){
        polygons[i]=std::make_unique<Polygon>();auto& p=*polygons[i];
        p.type0C=1;p.count04=p.count08=4;
        p.vertices00=std::make_unique<OriginalPauseStorage50LA::PolygonVertex[]>(4);
        p.owns38=p.dirty3C=1;p.count34=p.count48=p.capacity4C=1;
        p.packets44=std::make_unique<Packet[]>(1);
        for(std::size_t v=0;v<4;++v){auto& packet=p.packets44[0].vertices[v];packet.xyz={{0,0,1,0}};
            p.vertices00[v].stq14=&packet.stq;p.vertices00[v].rgba1C=&packet.rgba;p.vertices00[v].xyz24=&packet.xyz;}
        p.primitives30=std::make_unique<std::unique_ptr<Primitive>[]>(1);
        p.primitives30[0]=std::make_unique<Primitive>();p.primitives30[0]->reference=references.associatedCount();
        if(!references.acquireNext(Role::letterBox,0x1b8e10,static_cast<std::uint32_t>(0x38+4*i),0,0,directory,materialDiagnostic))
            return stop(d,S::material,materialDiagnostic.sourcePC,"Results shared-base polygon association");
        p.primitives30[0]->packet10=&p.packets44[0];
        for(std::size_t v=0;v<4;++v)*p.vertices00[v].rgba1C={{0,0,0,63}};
    }
    for(std::size_t i=0;i<2;++i)for(std::size_t v=0;v<4;++v){auto& vertex=polygons[i]->vertices00[v];
        vertex.authored00[0]=(v&1)?640.f:0.f;vertex.authored00[1]=i?0.f:448.f;*vertex.stq14=vertex.authored00;}
    mode1F0=0;
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive(sourceData,sourceData->overlays());
    if(!fonts->loadPauseBase1B8BC4(archive,directory,fontDirectory,fontDiagnostic))
        return stop(d,S::font,fontDiagnostic.sourcePC,"Results shared-base Application328 font list");
    const auto font=fonts->find282210(assets::OriginalApplicationFonts50LA::Name::denmark16);
    if(!text(*base180,Role::baseTextPause,0x1b8c08,0x180,0,40,font,directory,nullptr,d))return false;
    position(*base180,320.f,200.f);justification(*base180,3);color(*base180,base->grey170);
    scale(*base180,std::bit_cast<float>(std::uint32_t{0x3f99999a}));return true;
}
}
