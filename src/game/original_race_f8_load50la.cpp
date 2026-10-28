#include "original_race_f8_storage50la.h"
#include <algorithm>
#include <cstring>
#include <vector>
namespace rr::game {
bool OriginalRaceF850LA::Impl::acquire(std::uint32_t caller,std::uint32_t offset,
    const std::string& directory,D& d) {
    using Provider=assets::OriginalStandardArchiveProvider50LA;
    d.sourcePC=caller;d.object=offset;
    if(nextReference>=references.size()||!domain||!context||!context->live()||!context->materialApplication())
        return stop(d,caller,"F8 private fixed occurrence/owner");
    const auto ordinal=nextReference;
    const bool polygon=ordinal<4,texture=ordinal>=4&&ordinal<6;
    const auto expectedPC=polygon?0x198670u:texture?(ordinal==4?0x198810u:0x1988c0u):0x19897cu;
    const auto expectedOffset=polygon?0x30u+4u*std::uint32_t(ordinal):
        texture?(ordinal==4?0x40u:0xa0u):(ordinal<8?0x100u:0x170u);
    const char* expectedDirectory=polygon?"textures\\":texture?"ui\\textures\\":"font\\";
    const char* name=polygon?"SplitScreenBars":texture?(ordinal==4?"icon_x":"icon_sqr"):"denmarkbold16ingame";
    if(caller!=expectedPC||offset!=expectedOffset||directory!=expectedDirectory)
        return stop(d,caller,"F8 occurrence/directory order");
    const auto table=domain->materials382B38();const auto provider=domain->archiveProvider3E1A70();
    if(!table||!table->screenApplicationMatches(*context->materialApplication())||
       table->populationTerminal()||table->demandTerminal()||!provider||!provider->sealed())
        return stop(d,caller,"F8 same canonical table/provider");
    const std::string requested=directory+name;
    auto& ref=references[nextReference];ref.started=true;
    //299BE0: store actual numeric handle before14D590 retain. Partial failure
    //keeps this exact reference slot; no name remap, replay or prewarm.
    if(!table->find298C80(requested,ref.row,materialDiagnostic))
        return stop(d,materialDiagnostic.sourcePC,"F8 actual material lookup");
    if(!table->retainScreen14D590(ref.row,materialDiagnostic))
        return stop(d,materialDiagnostic.sourcePC,"F8 actual material retain");
    ref.retained=true;
    assets::OriginalStandardMaterialRowView50LA row;
    if(!table->row(ref.row,row))return stop(d,0x298e60,"F8 material row");
    if(row.object04){
        if(!row.object04->constructed29B550()||row.object04->textures383400()!=domain->textures383400().get())
            return stop(d,0x298e60,"F8 existing canonical material object");
    }else if(polygon){
        //The only new authored family in this connected route is the actual
        //zero-texture SplitScreenBars PSM. Compare authentic retained OVERLAYS
        //bytes to the current provider winner; retain real source admission.
        Provider::MemberView member;
        const auto original=sourceData->overlays()->find(requested+".psm");
        if(!original.identity||!original.data||original.size!=original.identity->bytes||
           provider->lookup132820(requested+".psm",member)!=Provider::Lookup::present||
           !member.member||!member.data||member.size!=original.size||
           std::memcmp(member.data,original.data,member.size))
            return stop(d,0x29b628,"F8 actual SplitScreenBars provider bytes");
        auto source=std::make_shared<assets::OriginalStandardMaterialSource50LA>();
        if(!source->prepare(*member.member,{member.data,member.data+member.size},materialDiagnostic.document))
            return stop(d,0x29b628,"F8 authored SplitScreenBars parse");
        const auto* general=source->section("General");const auto* pass=source->section("ShaderPass_1");
        std::int32_t passes=0,maps=-1;
        if(!general||!pass||!assets::originalUiInteger50KG(*general,"TotalShaderPasses",0,passes,
                materialDiagnostic.document,0x29b718)||passes!=1||
           !assets::originalUiInteger50KG(*pass,"totalTextureMaps",1,maps,materialDiagnostic.document,0x247214)||maps!=0)
            return stop(d,0x247214,"F8 authored zero-map SplitScreenBars");
        if(!domain->appendOriginalSources({source},{},materialDiagnostic))
            return stop(d,materialDiagnostic.sourcePC,"F8 reached source corpus admission");
    }else{
        //This private caller follows COMPLETE Results on the same table. Its
        //actual icon/font objects already exist; preserve their first provenance.
        //No Results/Pause occurrence certificate is reused or invented here.
        return stop(d,0x29b638,"F8 lost preceding Results canonical icon/font object");
    }
    ++nextReference;return true;
}
bool OriginalRaceF850LA::Impl::polygon(std::size_t i,const std::string& directory,D& d) {
    d.object=0x30+4*i;d.sourcePC=0x198640;
    polygons30[i]=std::make_unique<Polygon>();auto& p=*polygons30[i]; //198658 publication.
    p.type0C=1;p.count04=p.count08=4;
    p.vertices00=std::make_unique<PolygonStorage::Vertex[]>(4);
    p.owns38=p.dirty3C=1;p.count34=p.count48=p.capacity4C=1;
    p.packets44=std::make_unique<PolygonStorage::Descriptor[]>(1);
    auto& descriptor=p.packets44[0];
    auto* packet=static_cast<PolygonStorage::Packet*>(
        ::operator new(sizeof(PolygonStorage::Packet),std::align_val_t{64}));
    std::construct_at(packet);descriptor.allocation00.reset(packet); //2E1FF8 zero288.
    packet->vif10={{0x11000000u,0x03000000u,0x02000200u,0x6c0e8000u}};
    descriptor.vif04=&packet->vif10;
    packet->gif20={{0x8004u,0x30064000u,0x512u,0}};
    descriptor.gif08=&packet->gif20;descriptor.tag0C=&packet->viewport30;
    for(std::size_t v=0;v<4;++v){auto& target=packet->vertices[v];auto& vertex=p.vertices00[v];
        target.xyz={{0,0,1,0}};vertex.stq14=&target.stq;
        vertex.rgba1C=&target.rgba;vertex.xyz24=&target.xyz;vertex.aliases10=1;
    }
    packet->end100={{0x140000b0u,0x10000000u,0,0}};
    packet->dma00={{0x30000011u,0,0,0}};descriptor.dmaData4=&packet->vif10;
    descriptor.pointersKnown=true;p.packetRevision=1;
    p.primitives30=std::make_unique<std::unique_ptr<PolygonStorage::Primitive>[]>(1);
    p.primitives30[0]=std::make_unique<PolygonStorage::Primitive>();
    auto& primitive=*p.primitives30[0];primitive.reference=nextReference;
    const bool acquired=acquire(0x198670,std::uint32_t(0x30+4*i),directory,d);
    const auto& reference=references[primitive.reference];
    primitive.material00=reference.row;primitive.materialStored=reference.started;
    primitive.materialRetained=reference.retained;
    if(!acquired)return false;
    primitive.type0C=15;primitive.flags0F&=std::uint8_t(~1u);
    primitive.packet10=packet; //2E2144, after the SAME canonical retain.
    //133F20 sets RGB; caller1986E8 separately writes alpha63. Type1 has no
    //secondary color/position/texture alias. No submission is fabricated.
    for(auto& vertex:packet->vertices)vertex.rgba={{0,0,0,63}};
    p.field18=10.f;p.dirty3C=1;return true;
}
bool OriginalRaceF850LA::Impl::texture(Texture& t,std::size_t i,const std::string& directory,D& d) {
    t.primitive54=std::make_unique<Primitive>();t.primitive54->reference=nextReference;
    if(!acquire(i?0x1988c0u:0x198810u,i?0xa0u:0x40u,directory,d))return false;
    if(!domain->populateTimeStatsResources2DF144(context,materialDiagnostic))
        return stop(d,materialDiagnostic.sourcePC,"F8 actual whole-table material/texture population");
    const auto& ref=references[t.primitive54->reference];
    const assets::OriginalStandardMaterialObject50LA* object=nullptr;
    assets::OriginalStandardMaterialPassView50LA pass;assets::OriginalStandardTextureRowView50LA row;
    if(!domain->materials382B38()->resolve298E60(ref.row,object,materialDiagnostic)||!object||
       !object->constructed29B550()||!object->pass4C(0,pass)||!pass.mapCount20||!*pass.mapCount20||
       !pass.texture1A||(*pass.texture1A)[0]<0||!domain->textures383400()->row((*pass.texture1A)[0],row)||
       !row.object04||!row.references08||*row.references08<=0)
        return stop(d,0x2df174,"F8 actual first-pass texture");
    t.atlas=row.object04;const auto* width=t.atlas->sourceWidth14();const auto* height=t.atlas->sourceHeight18();
    if(!width||!height||*width<=0||*height<=0)return stop(d,0x2df180,"F8 source image extent");
    t.width34=float(*width);t.height38=float(*height);t.invWidth4C=1.f/t.width34;t.invHeight50=1.f/t.height38;
    t.buffer58=std::make_unique<Packet>();t.primitive54->packet10=t.buffer58.get();
    for(std::size_t v=0;v<4;++v){auto& p=t.buffer58->vertices[v];p.rgba={{127,127,127,63}};
        p.xyz={{0,0,float(depth)-t.field40,1}};p.stq={{(v&1)?1.f:0.f,(v&2)?1.f:0.f,1,0}};}
    t.flags5C|=1;return true;
}
bool OriginalRaceF850LA::Impl::text(Text& t,std::size_t i,
    const std::shared_ptr<const assets::OriginalApplicationFonts50LA::Entry>& font,std::string& directory,D& d) {
    d.object=0x100+0x70*i;d.sourcePC=0x19897c;
    if(!font||!font->metrics().decoded()||!font->texture281F90())return stop(d,0x2dfd90,"F8 actual registered font");
    t.flags5A|=8;t.font4C=font;t.capacity5B=40;t.packetCount5D=t.active5E=2;
    t.string8=std::unique_ptr<char[]>(new char[41]);t.glyphsC=std::make_unique<Glyph[]>(40);
    struct Scope {std::string& actual;std::string saved;explicit Scope(std::string& s):actual(s),saved(s){actual="font\\";}
        ~Scope(){actual.swap(saved);}};
    {
        Scope scope(directory);t.count0=2;t.primitives4=std::make_unique<std::unique_ptr<TextPrimitive>[]>(2);
        t.packets64=std::make_unique<TextPacket[]>(2);
        for(std::size_t p=0;p<2;++p){auto& packet=t.packets64[p];packet.count=p?8:72;
            packet.vertices=std::make_unique<PacketVertex[]>(packet.count);
            for(std::size_t v=0;v<packet.count;++v){packet.vertices[v].xyz={{0,0,1,0}};
                t.glyphsC[p*36+v/2].vertices[v%2].packet=&packet.vertices[v];}
            t.primitives4[p]=std::make_unique<TextPrimitive>();t.primitives4[p]->reference=nextReference;
            if(!acquire(0x19897c,std::uint32_t(0x100+0x70*i),directory,d))return false;
            t.primitives4[p]->packet10=&packet;
        }
    }
    t.string8[0]=0;t.length5C=0;t.alpha58=255;t.color54=0xffffffffu;
    for(std::size_t p=0;p<2;++p)for(std::size_t v=0;v<t.packets64[p].count;++v)
        t.packets64[p].vertices[v].rgba={{127,127,127,63}};
    t.flags5A=std::uint8_t((t.flags5A|3u)&~4u);return true;
}
void OriginalRaceF850LA::Impl::position(Texture& t,float x,float y)noexcept {
    if(t.x10==x&&t.y14==y)return;t.x10=x;t.y14=y;t.flags5C|=1;if(t.field3C!=0)t.flags5C|=2;
}
void OriginalRaceF850LA::Impl::position(Text& t,float x,float y)noexcept {
    if(t.x10==x&&t.y14==y)return;t.x10=x;t.y14=y;t.flags5A|=3;
}
void OriginalRaceF850LA::Impl::color(Text& t,std::uint32_t value)noexcept {
    if(t.color54==value)return;
    for(std::size_t i=0;i<t.capacity5B;++i)for(auto& v:t.glyphsC[i].vertices){auto& c=v.packet->rgba;
        c[0]=((value>>16)&255u)>>1;c[1]=((value>>8)&255u)>>1;c[2]=(value&255u)>>1;}
    t.color54=value;
}
bool OriginalRaceF850LA::Impl::localized(Text& t,std::size_t id,D& d) {
    const auto* entry=sourceData&&sourceData->english()?sourceData->english()->at(id):nullptr;if(!entry)return stop(d,0x18ac60,"F8 actual localized string");
    if(t.length5C==entry->text.size()&&!std::memcmp(t.string8.get(),entry->text.data(),entry->text.size()))return true;
    const auto n=std::min<std::size_t>(entry->text.size(),t.capacity5B);
    t.length5C=std::uint8_t(n);std::memcpy(t.string8.get(),entry->text.data(),n);t.string8[n]=0;t.flags5A|=3;return true;
}
bool OriginalRaceF850LA::load1985F0(std::shared_ptr<assets::OriginalStandardMaterialDomain50LA> domain,
    std::shared_ptr<const assets::OriginalStandardMaterialContext50LA> context,
    std::shared_ptr<assets::OriginalApplicationFonts50LA> fonts,
    std::shared_ptr<const assets::OriginalPauseSourceData50LA> data,std::string& directory,
    const std::string& fontDirectory,std::uint32_t maximum,OriginalRaceF8Diagnostic50LA& d)noexcept {
    d={};if(!constructed())return false;auto& p=*impl_;
    if(p.loadAttempted||!p.detached()||!domain||!domain->prepared()||!context||!context->live()||
       !fonts||!data||!data->sourceLoaded()||!data->overlays()||directory!="textures\\"||
       maximum!=0xffffffu)return p.stop(d,0x1985f0,"F8 connected cold loading inputs");
    p.loadAttempted=true;p.domain=std::move(domain);p.context=std::move(context);p.fonts=std::move(fonts);
    p.sourceData=std::move(data);p.depth=maximum;
    try {
        //141C20 returns this. Actual arg1=1 sets bit1; no zero-content branch.
        p.fields->flags1E4Low5|=2u;
        for(std::size_t i=0;i<4;++i)if(!p.polygon(i,directory,d))return false;
        p.fields->flags1E4Low5|=4u; p.fields->flags1E4Low5|=8u;
        std::shared_ptr<const assets::OriginalUiArchive50KG> archive(p.sourceData,p.sourceData->overlays());
        if(!p.fonts->loadRaceF8198798(archive,directory,fontDirectory,p.fontDiagnostic))
            return p.stop(d,p.fontDiagnostic.sourcePC,"F8 actual Application328 font load");
        const auto font=p.fonts->find282210(assets::OriginalApplicationFonts50LA::Name::denmark16);
        if(!font||!assets::OriginalApplicationFonts50LA::currentResource281F90(*p.domain,*font))
            return p.stop(d,0x282210,"F8 same retained denmark16 entry");
        struct Scope {std::string& actual;std::string saved;explicit Scope(std::string& s):actual(s),saved(s){actual="ui\\textures\\";}
            ~Scope(){actual.swap(saved);}} scope(directory);
        if(!p.texture((*p.textures40)[0],0,directory,d))return false;
        Impl::position((*p.textures40)[0],462.f,382.f);
        if(!p.texture((*p.textures40)[1],1,directory,d))return false;
        Impl::position((*p.textures40)[1],260.f,382.f);
        for(std::size_t i=0;i<2;++i){auto& t=(*p.texts100)[i];
            if(!p.text(t,i,font,directory,d))return false;
            Impl::color(t,0xff669bccu);
            //1989F8 loads file-initialized3C4738=0. Existing Text constructor
            //has that same value, so the source comparison takes the no-write arm.
            if(t.justification50!=0){t.justification50=0;t.flags5A|=3;}
        }
        if(!p.localized((*p.texts100)[0],8,d))return false;
        Impl::position((*p.texts100)[0],495.f,389.f);
        if(!p.localized((*p.texts100)[1],401,d))return false;
        Impl::position((*p.texts100)[1],293.f,389.f);
        if(p.nextReference!=p.references.size())return p.stop(d,0x198cc4,"F8 incomplete owned reference schedule");
        p.loadComplete=true;d={};return true; //Scope restores SAME36B970 at198CB8.
    }catch(...){return p.stop(d,d.sourcePC?d.sourcePC:0x1985f0,"F8 allocation/resource failure; acquired prefix retained");}
}
}
