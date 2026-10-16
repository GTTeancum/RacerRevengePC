#pragma once
#include "original_race_display_lifetime50la.h"
#include "../../src/game/original_position_stats_clip50la.h"
#include "../../src/game/original_position_stats_numeric50la.h"
#include "../../src/game/original_ui_text_position50kg.h"
#include <limits>
#include "../../src/assets/original_radar_overlay_material_reference50la.h"
#include <array>
#include <cstdint>
#include <exception>
#include <memory>
#include <new>

namespace rr::game {
struct OriginalPositionStatsPrimitiveDiagnostic50LA {
    enum class Error:std::uint8_t {none,owner,phase,allocation,material,renderer,geometry} error=Error::none;
    std::uint32_t sourcePC=0;
    std::size_t element=0,packet=0;
    assets::OriginalStandardMaterialDiagnostic50LA material;
    platform::VehicleWindowDiagnostic50DS renderer;
    OriginalCameraStartResult drain;
    OriginalPositionStatsClip50LA::Result clip=OriginalPositionStatsClip50LA::Result::accepted;
};
// Genuine191180 resource fragment only. No PositionStats Entity, full190190
// loader, visibility, collector submission, population or draw readiness claim.
// Root retains this owner; renderer MUST NOT retain it (display-lease cycle).
class OriginalPositionStatsPrimitiveOwner50LA final {
    friend struct original_main_entry50KG::Application;
    friend class OriginalPositionStatsResources50LA;
    friend class OriginalPositionStatsHud50LA;
    using D=OriginalPositionStatsPrimitiveDiagnostic50LA;
    using E=D::Error;
    using Domain=assets::OriginalStandardMaterialDomain50LA;
    using Context=assets::OriginalStandardMaterialContext50LA;
    using Archive=assets::OriginalUiArchive50KG;
    using Reference=assets::OriginalRadarOverlayMaterialReference50LA;
    struct PacketVertex {
        std::array<std::uint32_t,4> rgba{};
        std::array<float,4> stq{};
        std::array<float,4> xyz{{0.f,0.f,1.f,0.f}};
    };
    struct Vertex {
        std::array<float,4> authored00{{0.f,0.f,1.f,1.f}}; //2E2190
        std::uint32_t copies10=0;
        std::array<std::array<float,4>*,2> stq14{};
        std::array<std::array<std::uint32_t,4>*,2> rgba1C{};
        std::array<std::array<float,4>*,2> xyz24{};
    };
    struct Packet {
        std::unique_ptr<PacketVertex[]> vertices;
        std::size_t first=0,count=0,activeCount=0;
    };
    struct Primitive {
        Reference material;
        std::uint8_t passMask0C=0,parameterCount0D=0,field0E=0xff,flags0F=0;
        Packet* packet10=nullptr;
        std::array<float,2> scales18{{1.f,1.f}};
        std::uint16_t field20=0,field22=0;
    };
    struct Element {
        //2E1920 constructor projection; allocations happen later, in2E1AE0.
        std::unique_ptr<Vertex[]> vertices00;
        std::uint32_t count04=0,count08=0,type0C=2;
        float scale10=1.f,field14=0.f,field18=1.f;
        std::array<float,4> offset20{{0.f,0.f,0.f,1.f}};
        std::unique_ptr<std::unique_ptr<Primitive>[]> primitives30;
        std::uint32_t count34=0,owns38=0,dirty3C=0;
        std::uint8_t flags40=3;
        std::unique_ptr<Packet[]> packets44;
        std::uint32_t count48=0,capacity4C=0;
    };
    //Retain canonical resources/context through every real renderer drain/free.
    std::shared_ptr<Domain> domain_;
    std::shared_ptr<const Context> context_;
    std::shared_ptr<const Archive> overlays_;
    std::shared_ptr<const assets::OriginalStandardMaterialSource50LA> source_;
    std::unique_ptr<original_main_entry50KG::PositionStatsRendererLease50LA> lease_;
    std::unique_ptr<Element[]> elements_;
    bool attempted_=false,loaded_=false,closed_=false;
    OriginalPositionStatsPrimitiveOwner50LA(std::shared_ptr<Domain> domain,
        std::shared_ptr<const Context> context,std::shared_ptr<const Archive> overlays,
        std::shared_ptr<const assets::OriginalStandardMaterialSource50LA> source)
        :domain_(std::move(domain)),context_(std::move(context)),overlays_(std::move(overlays)),source_(std::move(source)){}
    static bool fail(D& d,E e,std::uint32_t pc)noexcept {d.error=e;d.sourcePC=pc;return false;}
    bool loadElement(Element& e,const std::string& directory,D& d) {
        e.type0C=5;e.count04=e.count08=236; //2E1B10 input118 doubled.
        e.vertices00=std::make_unique<Vertex[]>(236);e.owns38=1;e.dirty3C=1;
        e.count34=e.count48=e.capacity4C=5;
        e.packets44=std::make_unique<Packet[]>(5);
        constexpr std::array<std::size_t,5> first{{0,70,140,210,234}};
        constexpr std::array<std::size_t,5> count{{72,72,72,26,2}};
        //All packet allocations/aliases precede primitive allocation/retains.
        for(std::size_t p=0;p<5;++p){
            d.packet=p;auto& packet=e.packets44[p];packet.first=first[p];packet.count=count[p];
            packet.vertices=std::make_unique<PacketVertex[]>(packet.count);
            for(std::size_t i=0;i<packet.count;++i){
                auto& v=e.vertices00[packet.first+i];const auto alias=v.copies10;
                if(alias>=2)return fail(d,E::phase,0x2e27c0);
                v.rgba1C[alias]=&packet.vertices[i].rgba;
                v.stq14[alias]=&packet.vertices[i].stq;
                v.xyz24[alias]=&packet.vertices[i].xyz;++v.copies10;
            }
        }
        e.primitives30=std::make_unique<std::unique_ptr<Primitive>[]>(5);
        for(std::size_t p=0;p<5;++p){
            d.packet=p;e.primitives30[p]=std::make_unique<Primitive>();
            auto& primitive=*e.primitives30[p];
            //299BE0 retains the actual canonical ScreenMT association once.
            if(!primitive.material.acquire299BE0(domain_,overlays_,directory,d.material))
                return fail(d,E::material,0x2e20f0);
            primitive.passMask0C=15;primitive.flags0F=std::uint8_t(primitive.flags0F&~1u);
            primitive.packet10=&e.packets44[p];
        }
        //191200..191250:118 iterations, type5 vertex stride60, NOT30.
        //133F20(00FFFFFF) sets RGB127 only on the first vertex of each pair;
        //it updates both packet aliases when present. Alpha stays packet-zero.
        for(std::size_t pair=0;pair<118;++pair){
            auto& v=e.vertices00[pair*2];
            for(auto* rgba:v.rgba1C)if(rgba)for(unsigned c=0;c<3;++c)(*rgba)[c]=127;
        }
        return true;
    }
    //192C70 after the actual route-point lookup/project1922B0. Native writes
    //target the existing type5 pair's FIRST vertex and every packet alias.
    //No active-count publication, dirty lowering or renderer submission here.
    bool appendProjected192C70(std::size_t element,std::size_t point,
        const position_stats_detail50LA::RadarProjectedPoint& projected,
        const OriginalPositionStatsClip50LA& clip,std::uint32_t& alpha,D& d)noexcept {
        d={};d.element=element;
        if(!resourceFragmentLoaded()||element>=16||point>=118||
           !std::isfinite(projected.x)||!std::isfinite(projected.y)||
           projected.alpha>255||(projected.outside&&projected.alpha!=0))
            return fail(d,E::geometry,0x192c70);
        auto& e=elements_[element];
        if(e.type0C!=5||!e.vertices00||e.count04!=236)return fail(d,E::phase,0x192d20);
        auto& current=e.vertices00[point*2];
        Vertex* previous=point?&e.vertices00[(point-1)*2]:nullptr;
        const auto valid=[](const Vertex& v)noexcept {
            if(!v.copies10||v.copies10>2)return false;
            for(std::size_t i=0;i<v.copies10;++i)if(!v.stq14[i]||!v.rgba1C[i])return false;
            return true;
        };
        if(!valid(current)||(previous&&!valid(*previous)))return fail(d,E::phase,0x193040);
        std::array<float,4> next{{projected.x,projected.y,0.f,0.f}};
        std::array<float,4> revisedPrevious{};
        bool revisePrevious=false;
        std::uint32_t nextAlpha=projected.alpha;
        if(previous){
            //192D60/192E60 reads the first packet alpha byte, then shifts2.
            const auto previousAlpha=((*previous->rgba1C[0])[3]&255u)<<2;
            const bool exiting=projected.outside&&previousAlpha>0;
            const bool entering=!projected.outside&&nextAlpha>0&&previousAlpha==0;
            if(exiting||entering){
                OriginalPositionStatsClip50LA::Segment input,output{};
                input=exiting?OriginalPositionStatsClip50LA::Segment{{previous->authored00,next}}:
                    OriginalPositionStatsClip50LA::Segment{{next,previous->authored00}};
                d.clip=clip.clip119FC0(input,output);
                //Retail ignores the clip return. The admitted transition must
                //intersect; unsupported/rejected clipping is a visible native
                //failure, never silently emitted zero scratch coordinates.
                if(d.clip!=OriginalPositionStatsClip50LA::Result::accepted)
                    return fail(d,E::geometry,exiting?0x192de8:0x192ef0);
                if(exiting){next=output[1];nextAlpha=0;}
                else {revisedPrevious=output[1];revisePrevious=true;}
            }
        }
        const auto write=[](Vertex& vertex,const std::array<float,4>& value,std::uint32_t a)noexcept {
            vertex.authored00=value;
            for(std::size_t i=0;i<vertex.copies10;++i){
                *vertex.stq14[i]=value; //193044/193060: STQ aliases, not XYZ.
                (*vertex.rgba1C[i])[3]=a;
            }
        };
        if(revisePrevious)write(*previous,revisedPrevious,0); //192F50..192FE8.
        write(current,next,(nextAlpha>>2)&255u); //1930B4..1930DC.
        alpha=nextAlpha;return true;
    }
    //192620/1927B4 plus the type5 validation in2E2450. The subsequent
    //2E21D0 draw is not replaced by publishing these CPU geometry fields.
    bool finishRadarElement192624(std::size_t element,std::size_t points,D& d)noexcept {
        d={};d.element=element;
        if(!resourceFragmentLoaded()||element>=16||points<2||points>118)
            return fail(d,E::geometry,0x2e25e4);
        auto& e=elements_[element];
        if(e.type0C!=5||!e.vertices00||e.count04!=236)return fail(d,E::phase,0x192610);
        e.count08=static_cast<std::uint32_t>(points*2);e.dirty3C=1;
        //2E26A0..2E27A0: ceil(count08/72) active primitives; each packet
        //count is min(remaining,72). Keep actual allocation/alias boundaries.
        e.capacity4C=(e.count08+71u)/72u;
        std::size_t remaining=e.count08;
        for(std::size_t p=0;p<e.count48;++p){
            auto& packet=e.packets44[p];packet.activeCount=0;
            if(p<e.capacity4C){packet.activeCount=remaining>72?72:remaining;
                if(packet.activeCount>packet.count)return fail(d,E::geometry,0x2e2780);
                remaining-=packet.activeCount;}
        }
        return true;
    }
    //2E2ED0 type5, native CPU screen geometry. Original constructor domain:
    //scale10=1, rotation14=0, depth18=1, width flags40=3. Geometry lands in
    //the genuine packet STQ aliases; this does not execute or emulate VU code.
    bool lowerRadarElement2E2ED0(std::size_t element,std::uint32_t videoMaximum,D& d)noexcept {
        d={};d.element=element;
        if(!resourceFragmentLoaded()||element>=16||videoMaximum!=0xffffffu)
            return fail(d,E::geometry,0x2e2ed0);
        auto& e=elements_[element];
        if(e.type0C!=5||e.count08<4||e.count08>236||(e.count08&1u)||
           e.scale10!=1.f||e.field14!=0.f||e.field18!=1.f||e.flags40!=3||
           !e.vertices00||!e.packets44)return fail(d,E::geometry,0x2e3050);
        if(!e.dirty3C)return true;
        const auto add=[](float a,float b)noexcept{volatile float r=a+b;return r;};
        const auto sub=[](float a,float b)noexcept{volatile float r=a-b;return r;};
        const auto mul=[](float a,float b)noexcept{volatile float r=a*b;return r;};
        std::array<std::array<float,4>,118> centers{};
        const auto points=std::size_t(e.count08/2);
        const float depth=sub(static_cast<float>(videoMaximum),e.field18);
        //Preflight before any writes; source-compatible finite conversion uses
        //the existing shared EE CVT.W.S native contract, never host lrint/fenv.
        for(std::size_t i=0;i<points;++i){
            auto& even=e.vertices00[i*2];auto& odd=e.vertices00[i*2+1];
            std::int32_t x=0,y=0;
            if(!roundOriginalUiTextMetric50KG(even.authored00[0],e.scale10,x)||
               !roundOriginalUiTextMetric50KG(even.authored00[1],e.scale10,y))
                return fail(d,E::geometry,0x2e30dc);
            centers[i]={{static_cast<float>(x),static_cast<float>(y),depth,0.f}};
            for(auto* v:{&even,&odd}){
                if(!v->copies10||v->copies10>2)return fail(d,E::geometry,0x2e31d0);
                for(std::size_t alias=0;alias<v->copies10;++alias)
                    if(!v->stq14[alias]||!v->rgba1C[alias]||!v->xyz24[alias])
                        return fail(d,E::geometry,0x2e31d0);
            }
        }
        //2E30F0..2E3300: source Z, paired source/STQ, odd RGBA and XYZ.xy.
        for(std::size_t i=0;i<points;++i){
            auto& even=e.vertices00[i*2];auto& odd=e.vertices00[i*2+1];
            even.authored00[2]=depth;odd.authored00=even.authored00;
            const auto color=*even.rgba1C[0];const auto xyz=*even.xyz24[0];
            for(std::size_t alias=0;alias<even.copies10;++alias)*even.stq14[alias]=centers[i];
            for(std::size_t alias=0;alias<odd.copies10;++alias){
                *odd.stq14[alias]=odd.authored00;
                for(std::size_t lane=0;lane<4;++lane)(*odd.rgba1C[alias])[lane]=color[lane]&255u;
                (*odd.xyz24[alias])[0]=xyz[0];(*odd.xyz24[alias])[1]=xyz[1];
            }
        }
        const auto writePair=[&](std::size_t i,float nx,float ny)noexcept{
            std::array<float,4> plus{{add(centers[i][0],nx),add(centers[i][1],ny),add(depth,3.f),1.f}};
            std::array<float,4> minus{{sub(centers[i][0],nx),sub(centers[i][1],ny),sub(depth,3.f),1.f}};
            for(std::size_t alias=0;alias<e.vertices00[i*2].copies10;++alias)
                *e.vertices00[i*2].stq14[alias]=plus;
            for(std::size_t alias=0;alias<e.vertices00[i*2+1].copies10;++alias)
                *e.vertices00[i*2+1].stq14[alias]=minus;
        };
        for(std::size_t i=0;i+1<points;++i){
            const float dx=sub(centers[i+1][0],centers[i][0]);
            const float dy=sub(centers[i+1][1],centers[i][1]);
            const float squared=add(add(mul(dx,dx),mul(dy,dy)),0.f);
            if(!std::isfinite(squared))return fail(d,E::geometry,0x2e33b0);
            float inverse=std::numeric_limits<float>::max();
            if(squared!=0.f){volatile float length=std::sqrt(squared);volatile float reciprocal=1.f/length;inverse=reciprocal;}
            //13E3F0 native normalization: a zero segment remains zero XY.
            const float nx=mul(-mul(dy,inverse),3.f),ny=mul(mul(dx,inverse),3.f);
            if(!std::isfinite(nx)||!std::isfinite(ny))return fail(d,E::geometry,0x2e3410);
            if(!i)writePair(0,nx,ny); //Source does not average joins.
            writePair(i+1,nx,ny);
        }
        e.dirty3C=0;return true;
    }
    bool stage191180(const std::shared_ptr<original_main_entry50KG::RaceDisplayLifetime50LA>& display,
        const std::string& directory,D& d)noexcept {
        d={};if(attempted_||closed_)return fail(d,E::phase,0x191180);
        attempted_=true;
        if(!display||!context_||!context_->live()||context_->videoIdentity()!=display->video_.get())
            return fail(d,E::owner,0x191180);
        try {
            lease_=display->retainPositionStats16();
            if(!lease_)return fail(d,E::owner,0x191180);
            //All16 nonthrowing default constructors complete before any load.
            elements_=std::make_unique<Element[]>(16);
            if(!lease_->constructedAll16())std::terminate();
            for(std::size_t i=0;i<16;++i){d.element=i;if(!loadElement(elements_[i],directory,d))return false;}
            loaded_=true;return true;
        }catch(const std::bad_alloc&){return fail(d,E::allocation,0x2e1ae0);}
        catch(...){return fail(d,E::phase,0x2e1ae0);}
    }
public:
    OriginalPositionStatsPrimitiveOwner50LA(const OriginalPositionStatsPrimitiveOwner50LA&)=delete;
    OriginalPositionStatsPrimitiveOwner50LA& operator=(const OriginalPositionStatsPrimitiveOwner50LA&)=delete;
    ~OriginalPositionStatsPrimitiveOwner50LA(){if(elements_||lease_)std::terminate();}
    //A successful resource fragment is not renderer/draw readiness.
    bool resourceFragmentLoaded()const noexcept{return loaded_&&!closed_;}
    bool close(D& d)noexcept {
        d={};if(closed_)return true;
        if(!elements_){lease_.reset();closed_=true;loaded_=false;return true;}
        if(!lease_)return fail(d,E::owner,0x2e1980);
        while(lease_->nextElement()>=0){
            const int index=lease_->nextElement();d.element=static_cast<std::size_t>(index);
            d.drain=lease_->drainNext2E1980(d.renderer);
            if(d.drain.status!=OriginalCameraStartStatus::complete)return fail(d,E::renderer,d.drain.sourcePC);
            auto& e=elements_[static_cast<std::size_t>(index)];
            //Original ascending inner destroys, after BOTH real drains.
            if(e.primitives30)for(std::size_t p=0;p<e.count34;++p){
                d.packet=p;auto& primitive=e.primitives30[p];
                if(primitive){primitive->material.release299AC0();primitive.reset();}
            }
            e.primitives30.reset();
            if(e.packets44)for(std::size_t p=0;p<e.count48;++p)e.packets44[p].vertices.reset();
            e.packets44.reset();
            if(e.owns38)e.vertices00.reset();
            //Unused/partially loaded elements took the identical real drain path.
            if(!lease_->finishedElementResources(index))std::terminate();
        }
        elements_.reset();lease_.reset();loaded_=false;closed_=true;return true;
    }
};
}

namespace original_main_entry50KG {
struct PositionStatsPrimitiveResult50LA {
    bool attempted=false,resourceFragmentLoaded=false;
    std::shared_ptr<rr::game::OriginalPositionStatsPrimitiveOwner50LA> owner;
    rr::game::OriginalPositionStatsPrimitiveDiagnostic50LA diagnostic;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material;
    rr::assets::OriginalUiPageDiagnostic50KG source;
    std::string failure;
};
//Uncalled detached resource fragment. Root activates no load or draw through it.
inline bool Application::stagePositionStatsPrimitives50LA(const std::filesystem::path& originalRoot,
    PositionStatsPrimitiveResult50LA& out)const noexcept {
    using namespace rr::assets;
    if(out.attempted||out.owner)return false;
    out.attempted=true;
    const auto fail=[&](const char* reason)noexcept{try{out.failure=reason;}catch(...){}return false;};
    try {
        const auto display=raceDisplayOwner50LA_.lock();
        if(!enabled||!display||display->video_!=videoOwner_||!positionStatsPrimitivesOwner50LA_.expired()||
           !videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
           !materialContextOwner_||!materialContextOwner_->live()||
           materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           materialContextOwner_->materialApplication()!=materialApplication||
           !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared()||!display->canRetain())
            return fail("PositionStats primitives require actual live Application/display/material owners");
        const auto overlays=readArchive(originalRoot,OriginalUiArchiveId50KG::overlays);
        const auto psm=overlays->find("ui\\textures\\RadarOverlay.psm");
        if(!psm.identity||!psm.data||psm.size!=psm.identity->bytes)return fail("original RadarOverlay PSM missing");
        auto source=std::make_shared<OriginalStandardMaterialSource50LA>();
        if(!source->prepare(psm.identity->name,std::vector<std::uint8_t>(psm.data,psm.data+psm.size),out.source))
            return fail("original RadarOverlay PSM parse rejected");
        const auto* general=source->section("General");const auto* pass=source->section("ShaderPass_1");
        const auto* passes=general?general->find("TotalShaderPasses"):nullptr;
        const auto* maps=pass?pass->find("totalTextureMaps"):nullptr;
        const auto* program=pass?pass->find("microProgramName"):nullptr;
        if(!passes||passes->text!="1"||!maps||maps->text!="0"||!program||program->text!="ScreenObject")
            return fail("authentic RadarOverlay zero-map ScreenObject domain rejected");
        const auto domain=standardMaterialDomainOwner_;
        const auto provider=domain->archiveProvider3E1A70();
        if(!provider||!provider->mountAuthenticatedUi(overlays,out.provider)||!provider->sealNativeMounts(out.provider))
            return fail("RadarOverlay authenticated provider mount rejected");
        std::vector<std::shared_ptr<const OriginalStandardMaterialSource50LA>> materials{source};
        std::vector<OriginalStandardTextureSourceInput50LA> textures; //Authored zero maps.
        if(!domain->appendOriginalSources(materials,textures,out.material))return fail("RadarOverlay canonical source append rejected");
        out.owner=std::shared_ptr<rr::game::OriginalPositionStatsPrimitiveOwner50LA>(
            new rr::game::OriginalPositionStatsPrimitiveOwner50LA(domain,materialContextOwner_,overlays,source));
        positionStatsPrimitivesOwner50LA_=out.owner;
        struct DirectoryScope {
            std::string& actual;std::string previous;
            explicit DirectoryScope(std::string& value):actual(value),previous(value){actual="ui\\textures\\";}
            ~DirectoryScope(){actual.swap(previous);}
        } directory(materialDirectory36B970_);
        if(!out.owner->stage191180(display,materialDirectory36B970_,out.diagnostic))
            return fail("PositionStats primitive fragment failed; retain owner for ordered close");
        out.resourceFragmentLoaded=true;return true;
    }catch(...){return fail("PositionStats primitive fragment exception; retain any partial owner");}
}
}
