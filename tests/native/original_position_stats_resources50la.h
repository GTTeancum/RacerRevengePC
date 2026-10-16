#pragma once
#include "original_position_stats_primitives50la.h"
#include "../../src/game/original_position_stats_clip50la.h"
#include "../../src/game/original_position_stats_loader_data50la.h"
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
//Detached cold18FF80/190190 resource owner. No race+CC hierarchy, view adapter,
//demand, collector, predraw or draw API. Renderer must NEVER strongly retain it.
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
    struct Packet {std::unique_ptr<PacketVertex[]> vertices;std::size_t count=0;};
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
    std::shared_ptr<const Archive> overlays_;
    std::array<Text,5> texts_{}; //Original order:5,9,118,clip.
    std::array<Texture,9> fixed_{};
    std::array<Texture,118> map_{};
    std::optional<OriginalPositionStatsClip50LA> clip_{std::in_place};
    std::shared_ptr<OriginalPositionStatsPrimitiveOwner50LA> primitives_;
    const void* race30_=nullptr; //Detached: no fabricated original race+CC/30 join.
    std::uint32_t field32B0_=0,field32B4_=0;
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
struct PositionStatsResourceResult50LA {
    bool attempted=false,coldResourcesLoaded=false;
    std::shared_ptr<rr::game::OriginalPositionStatsResources50LA> owner;
    rr::game::OriginalPositionStatsResourceDiagnostic50LA diagnostic;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material;
    rr::assets::OriginalUiPageDiagnostic50KG source;
    rr::assets::OriginalApplicationFontDiagnostic50LA font;
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
        //Only after127 textures, font,5 texts and119F60: original191118->191180.
        const bool okay=stagePositionStatsPrimitives50LA(originalRoot,out.primitive);
        out.owner->primitives_=out.primitive.owner; //Retain even a failed partial16 array.
        if(!okay)return fail("PositionStats primitive stage failed; retain both actual leases");
        out.owner->stage_=rr::game::OriginalPositionStatsResources50LA::Stage::complete;
        out.coldResourcesLoaded=true;return true;
    }catch(...){return fail("PositionStats resource exception; retain any constructed/partial owner");}
}
}
