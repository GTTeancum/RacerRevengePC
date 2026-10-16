#pragma once
#include "original_race_display_lifetime50la.h"
#include "../../src/assets/original_radar_overlay_material_reference50la.h"
#include <array>
#include <cstdint>
#include <exception>
#include <memory>
#include <new>

namespace rr::game {
struct OriginalPositionStatsPrimitiveDiagnostic50LA {
    enum class Error:std::uint8_t {none,owner,phase,allocation,material,renderer} error=Error::none;
    std::uint32_t sourcePC=0;
    std::size_t element=0,packet=0;
    assets::OriginalStandardMaterialDiagnostic50LA material;
    platform::VehicleWindowDiagnostic50DS renderer;
    OriginalCameraStartResult drain;
};
// Genuine191180 resource fragment only. No PositionStats Entity, full190190
// loader, visibility, collector submission, population or draw readiness claim.
// Root retains this owner; renderer MUST NOT retain it (display-lease cycle).
class OriginalPositionStatsPrimitiveOwner50LA final {
    friend struct original_main_entry50KG::Application;
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
        std::size_t first=0,count=0;
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
