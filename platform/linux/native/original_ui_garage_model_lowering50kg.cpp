#include "original_ui_garage_model_lowering50kg.h"
#include "original_ui_garage_model_resource50kg.h"
#include "original_ui_garage_lowering_proof50kg.h"
#include "../../../src/game/original_ui_garage_palette_vertex50kg.h"
#include <array>
#include <cstring>
#include <algorithm>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <vector>

namespace rr::platform {
// Exactness harness, off by default (RR_GARAGE_KERNEL_SHADOW=1): the former
// volatile-spill kernels, compiled renamed in original_ui_garage_kernel_shadow50kg.cpp.
// Every lowered triangle is projected by both and the packets must be identical.
namespace garage_kernel_shadow50KG {
std::size_t size(int which,int part)noexcept;
int prepare(int which,const void* context,const game::OriginalRacerModelPacket50LA* packet,int family,void* preamble)noexcept;
int project(int which,const void* preamble,const void* raw3,void* polygon)noexcept;
}
namespace {
bool kernelShadowEnabled50()noexcept{
    static const bool on=[]{const char* v=std::getenv("RR_GARAGE_KERNEL_SHADOW");return v&&v[0]=='1'&&!v[1];}();
    return on;
}
struct KernelShadow50KG {
    struct alignas(64) Block{unsigned char bytes[64];};
    const void* owner=nullptr;int which=-1;int status=-1;std::vector<Block> preamble;
    unsigned long long prepared=0,compared=0;
};
thread_local KernelShadow50KG kernelShadow50KG;
[[noreturn]] void kernelShadowMismatch50(const char* what,int which,unsigned long long n)noexcept{
    std::fprintf(stderr,"garage_kernel_shadow MISMATCH %s kernel=%d after=%llu\n",what,which,n);std::fflush(stderr);
    std::terminate();
}
template<class Context,class Preamble>
void kernelShadowPrepare50(const void* owner,int which,const Context& context,const game::OriginalRacerModelPacket50LA* packet,
    int family,game::garage_prelit50KG::Status real)noexcept{
    auto& s=kernelShadow50KG;s.owner=nullptr;
    if(garage_kernel_shadow50KG::size(which,0)!=sizeof(Context)||garage_kernel_shadow50KG::size(which,1)!=sizeof(Preamble))
        kernelShadowMismatch50("layout",which,s.prepared);
    try{s.preamble.resize((sizeof(Preamble)+63)/64);}catch(...){kernelShadowMismatch50("allocation",which,s.prepared);}
    s.status=garage_kernel_shadow50KG::prepare(which,&context,packet,family,s.preamble.data());
    if(s.status!=int(real))kernelShadowMismatch50("prepare-status",which,s.prepared);
    s.owner=owner;s.which=which;++s.prepared;
}
bool samePacket50(const OriginalUiGaragePrelitPacket50KG& a,const OriginalUiGaragePrelitPacket50KG& b)noexcept{
    if(a.quadCount!=b.quadCount)return false;
    const std::size_t n=std::min<std::size_t>(std::size_t(a.quadCount)*4u,a.vertices.size());
    return !std::memcmp(a.vertices.data(),b.vertices.data(),n*sizeof(a.vertices[0]))&&
        !std::memcmp(a.stq.data(),b.stq.data(),n*sizeof(a.stq[0]))&&
        !std::memcmp(a.rasterW.data(),b.rasterW.data(),n*sizeof(float))&&
        !std::memcmp(a.fog.data(),b.fog.data(),n*sizeof(float));
}
}
namespace {
// Host memo for ONE prepared palette lowering (its Preamble is fixed between
// prepare() and the next prepare()): a vertex's prepared operand and guard bits
// are a pure function of that Preamble and the vertex's own source words, so an
// identical strip vertex reuses its first successful result bit-for-bit.
struct PaletteVertexCache50KG {
    static constexpr std::size_t capacity=8192;
    struct Key {std::array<std::int16_t,3> position;std::array<std::int8_t,3> normal;std::array<std::int16_t,2> uv;std::int16_t adc;};
    struct Entry {std::uint32_t epoch=0;Key key{};game::garage_prelit50KG::PreparedVertex prepared{};std::uint8_t guard=0;};
    std::array<Entry,capacity> entries{};
    const void* owner=nullptr;std::uint32_t epoch=0;std::size_t stored=0;
    static Key key(const game::garage_palette50KG::RawVertex& v)noexcept{return {v.position,v.normal,v.uv,v.paletteAdcWord};}
    static bool same(const Key& a,const Key& b)noexcept{
        return a.position==b.position&&a.normal==b.normal&&a.uv==b.uv&&a.adc==b.adc;}
    static std::size_t hash(const Key& k)noexcept{
        std::uint32_t h=2166136261u;auto mix=[&](std::uint32_t v){h^=v;h*=16777619u;};
        for(auto x:k.position)mix(std::uint16_t(x));for(auto x:k.normal)mix(std::uint8_t(x));
        for(auto x:k.uv)mix(std::uint16_t(x));mix(std::uint16_t(k.adc));return h&(capacity-1);}
    void reset(const void* lowering)noexcept{owner=lowering;stored=0;if(++epoch==0){for(auto& e:entries)e.epoch=0;epoch=1;}}
    bool find(const void* lowering,const game::garage_palette50KG::RawVertex& v,
        game::garage_prelit50KG::PreparedVertex& out,std::uint8_t& guard)const noexcept{
        if(owner!=lowering)return false;const auto k=key(v);
        for(std::size_t i=hash(k),n=0;n<capacity;++n,i=(i+1)&(capacity-1)){
            const auto& e=entries[i];if(e.epoch!=epoch)return false;
            if(same(e.key,k)){out=e.prepared;guard=e.guard;return true;}
        }
        return false;
    }
    void store(const void* lowering,const game::garage_palette50KG::RawVertex& v,
        const game::garage_prelit50KG::PreparedVertex& prepared,std::uint8_t guard)noexcept{
        if(owner!=lowering||stored>=capacity/2)return;const auto k=key(v);
        for(std::size_t i=hash(k),n=0;n<capacity;++n,i=(i+1)&(capacity-1)){
            auto& e=entries[i];
            if(e.epoch!=epoch){e.epoch=epoch;e.key=k;e.prepared=prepared;e.guard=guard;++stored;return;}
            if(same(e.key,k))return;
        }
    }
};
thread_local PaletteVertexCache50KG paletteVertexCache50KG;
}
// The kernel shadow compares inside projectTriangle on the caller's thread.
bool loweringSerialOnly50KG()noexcept{return kernelShadowEnabled50();}
bool OriginalUiGarageModelLowering50KG::prepare(
    std::shared_ptr<const OriginalUiGarageRawSurface50KG> raw,std::uint32_t ordinal,
    const game::OriginalRacerModelPacket50LA& packet,Program program,const Context& c,Diagnostic& d)noexcept {
    d={};const auto fail=[&](Status s,std::uint32_t pc){d.status=s;d.sourcePC=pc;return false;};
    paletteVertexCache50KG.reset(this); // Any earlier lowering's memo ends here.
    kernelShadow50KG.owner=nullptr;
    if(raw_||kernel_!=Kernel::none||!raw||!raw->current()||!raw->model_)
        return fail(Status::phase,0x24b4a0u);
    const auto& model=*raw->model_;const auto& packets=model.packets();
    const auto* geometry=model.geometry();
    if(!geometry||!packets.constructed240740()||!packets.matricesPrepared_||!packets.matrixRevision_||
       packets.model_!=&model.scene()||packets.geometry_!=geometry||
       packets.packet(ordinal)!=&packet||!packet.garageSourceFlags20)
        return fail(Status::packet,0x241790u);
    OriginalUiGarageRawSurface50KG::Metadata metadata;
    if(!raw->describe(metadata,d.raw))return fail(Status::raw,d.raw.sourcePC);
    if(!metadata.runtimeSurface||!metadata.runtimeSurface->material00||
       *metadata.runtimeSurface->material00<0||metadata.runtimeSurface->uvCount0D!=1u||
       packet.extraMatrixCount!=((*packet.garageSourceFlags20&0x100u)?2u:0u)||
       packet.matrixSlots.size()!=packet.extraMatrixCount+1u+packet.sourcePaletteRows.size())
        return fail(Status::packet,0x241488u);
    const bool palette=geometry->hasPalette20();
    if(palette){
        const auto* sourceRows=geometry->palette20(ordinal);
        if(metadata.runtimeSurface->palette0E!=ordinal||!sourceRows||sourceRows->empty()||
           packet.sourcePaletteRows!=*sourceRows)
            return fail(Status::packet,0x241818u);
        //26E434 initializes1C8 null. Palette2415B4 only sets08 and jumps;
        // the sole reached1C8 writer24174C is in the NON-palette branch.
        if(model.scene().fields().field1C8)return fail(Status::packet,0x24138cu);
    }else if(ordinal||packets.count()!=1u||!packet.sourcePaletteRows.empty())
        return fail(Status::packet,0x241790u);
    if(c.sourceXyOffset!=std::array<std::uint32_t,2>{{1728u,1824u}}||
       c.camera.canvasOffset!=std::array<float,2>{{1728.f,1824.f}})
        return fail(Status::packet,0x243d40u);
    // This retained raw reader admits the authenticated V4_16 Garage source,
    // not a reconstructed float stream. Plain Diffuse is absent from this
    // inventory and stays distinct/unavailable rather than aliased to337690.
    if(program==Program::diffuse)return fail(Status::program,0x33a580u);
    if(program==Program::prelitCompressed&&(!metadata.hasStoredColors||palette))
        return fail(Status::program,0x339600u);
    auto camera=c.camera;
    if(program!=Program::prelitCompressed){
        if(packet.extraMatrixCount==2u){
            for(unsigned row=0;row<4;++row){
                std::copy_n(packet.matrixSlots[0].v+4u*row,4,camera.lights.directions[row].begin());
                std::copy_n(packet.matrixSlots[1].v+4u*row,4,camera.lights.diffuse[row].begin());
            }
        }else{
            if(!c.globalLights1499A0)return fail(Status::packet,0x24bd7cu);
            camera.lights=*c.globalLights1499A0;
        }
    }
    const auto flags=static_cast<std::uint8_t>(*packet.garageSourceFlags20);
    Numeric result=Numeric::input;Kernel selected=Kernel::none;
    // All output remains private until the selected actual program completes.
    // A failed preparation may be retried; it has no collector side effects.
    if(palette){
        if(program!=Program::diffuseCompressed&&program!=Program::specularMapCompressed)
            return fail(Status::program,0x337690u);
        game::garage_palette50KG::Context context;
        context.camera=camera;context.guardX334=c.guardX334;context.guardY338=c.guardY338;
        context.modelFlags1B2=flags;
        const auto family=program==Program::diffuseCompressed?
            game::garage_palette50KG::Family::diffuseCompressed:
            game::garage_palette50KG::Family::specularMapCompressed;
        result=game::garage_palette50KG::prepare(packet,context,family,palette_);selected=Kernel::palette;
        if(kernelShadowEnabled50())kernelShadowPrepare50<game::garage_palette50KG::Context,game::garage_palette50KG::Preamble>(
            this,3,context,&packet,int(family),result);
    }else{
        game::garage_prelit50KG::Context geometryContext;
        std::copy_n(packet.matrixSlots[packet.modelMatrixIndex()].v,16,geometryContext.modelWorld100.begin());
        geometryContext.viewC0=camera.viewC0;geometryContext.projection80=camera.projection80;
        geometryContext.sourceScreen140=camera.screen140;geometryContext.cameraPosition180=camera.cameraPosition180;
        geometryContext.positionScale1E0=packet.scale40[0];geometryContext.uvScale1E8=packet.scale40[2];
        geometryContext.fogEnd33Z=camera.fogEnd33Z;geometryContext.fogSlope33W=camera.fogSlope33W;
        geometryContext.modelFlags1B2=flags;
        switch(program){
        case Program::prelitCompressed:
            result=game::garage_prelit50KG::prepare(geometryContext,prelit_);selected=Kernel::prelit;
            if(kernelShadowEnabled50())kernelShadowPrepare50<game::garage_prelit50KG::Context,game::garage_prelit50KG::Preamble>(
                this,0,geometryContext,nullptr,0,result);
            break;
        case Program::diffuseCompressed:{
            game::garage_diffuse50KG::Context context;
            context.geometry=geometryContext;context.lighting=camera;context.normalScale1E4=packet.scale40[1];
            result=game::garage_diffuse50KG::prepare(context,diffuse_);selected=Kernel::diffuse;
            if(kernelShadowEnabled50())kernelShadowPrepare50<game::garage_diffuse50KG::Context,game::garage_diffuse50KG::Preamble>(
                this,1,context,nullptr,0,result);
            break;}
        case Program::specularMapCompressed:{
            game::garage_specular50KG::Context context;
            context.geometry=geometryContext;context.lighting=camera;context.normalScale1E4=packet.scale40[1];
            result=game::garage_specular50KG::prepare(context,specular_);selected=Kernel::specular;
            if(kernelShadowEnabled50())kernelShadowPrepare50<game::garage_specular50KG::Context,game::garage_specular50KG::Preamble>(
                this,2,context,nullptr,0,result);
            break;}
        default:return fail(Status::program,0x24b1c0u);
        }
    }
    if(result!=Numeric::complete){d.numeric=result;return fail(Status::numeric,0x24b4bcu);}
    raw_=std::move(raw);packet_=&packet;packetOrdinal_=ordinal;matrixRevision_=packets.matrixRevision_;
    sourceFlags20_=*packet.garageSourceFlags20;triangleCount_=metadata.triangleCount;kernel_=selected;
    d.status=Status::complete;d.sourcePC=0x24b4bcu;d.numeric=Numeric::complete;return true;
}
bool OriginalUiGarageModelLowering50KG::current()const noexcept {
    if(!raw_||!raw_->current()||!raw_->model_||!packet_||kernel_==Kernel::none)return false;
    const auto& packets=raw_->model_->packets();
    return packets.constructed240740()&&packets.matricesPrepared_&&matrixRevision_&&
        packets.matrixRevision_==matrixRevision_&&packets.packet(packetOrdinal_)==packet_&&
        packet_->garageSourceFlags20&&*packet_->garageSourceFlags20==sourceFlags20_;
}
bool OriginalUiGarageModelLowering50KG::readTriangleCount(std::uint32_t& out)const noexcept {
    if(!current())return false;
    out=triangleCount_;return true;
}
bool OriginalUiGarageModelLowering50KG::projectTriangle(std::uint32_t ordinal,
    OriginalUiGaragePrelitPacket50KG& out,Diagnostic& d)const noexcept {
    d={};if((OriginalUiGarageLoweringProof50KG::provenLowering!=this&&!current())||ordinal>=triangleCount_){d.sourcePC=0x24b4a0u;return false;}
    std::array<OriginalUiGarageRawSurface50KG::Vertex,3> source;
    if(!raw_->triangle(ordinal,source,d.raw)){d.status=Status::raw;d.sourcePC=d.raw.sourcePC;return false;}
    game::garage_prelit50KG::Polygon polygon;Numeric result=Numeric::input;
    const bool shadow=kernelShadowEnabled50()&&kernelShadow50KG.owner==this;
    // Byte copy of this triangle's kernel input, for the shadow harness only.
    alignas(16) unsigned char shadowRaw[96]{};std::size_t shadowRawSize=0;
    const auto keep=[&](const auto& vertices){shadowRawSize=sizeof(vertices);
        if(shadowRawSize<=sizeof shadowRaw)std::memcpy(shadowRaw,&vertices,shadowRawSize);};
    switch(kernel_){
    case Kernel::prelit:{
        std::array<game::garage_prelit50KG::RawVertex,3> vertices;
        for(unsigned i=0;i<3;++i){vertices[i].position=source[i].position;vertices[i].uv=source[i].uv;vertices[i].rgba=source[i].rgba;}
        if(shadow)keep(vertices);
        result=game::garage_prelit50KG::projectTriangle(prelit_,vertices,polygon);break;}
    case Kernel::diffuse:{
        std::array<game::garage_diffuse50KG::RawVertex,3> vertices;
        for(unsigned i=0;i<3;++i){vertices[i].position=source[i].position;vertices[i].normal=source[i].normal;vertices[i].uv=source[i].uv;}
        if(shadow)keep(vertices);
        result=game::garage_diffuse50KG::projectTriangle(diffuse_,vertices,polygon);break;}
    case Kernel::specular:{
        std::array<game::garage_specular50KG::RawVertex,3> vertices;
        for(unsigned i=0;i<3;++i){vertices[i].position=source[i].position;vertices[i].normal=source[i].normal;}
        if(shadow)keep(vertices);
        result=game::garage_specular50KG::projectTriangle(specular_,vertices,polygon);break;}
    case Kernel::palette:{
        std::array<game::garage_palette50KG::RawVertex,3> vertices;
        for(unsigned i=0;i<3;++i){vertices[i].position=source[i].position;vertices[i].normal=source[i].normal;
            vertices[i].uv=source[i].uv;vertices[i].paletteAdcWord=source[i].paletteAdcWord;}
        if(shadow)keep(vertices);
        // Exactly projectTriangle(): triangle checks, three vertices in order,
        // then projection; an identical earlier vertex reuses its result.
        result=game::garage_palette50KG::checkTriangle(palette_,vertices);
        std::array<game::garage_prelit50KG::PreparedVertex,3> prepared{};std::uint8_t guardAny=0;
        for(unsigned i=0;result==Numeric::complete&&i<3;++i){
            std::uint8_t guard=0;
            if(!paletteVertexCache50KG.find(this,vertices[i],prepared[i],guard)){
                result=game::garage_palette50KG::prepareVertex(palette_,vertices[i],prepared[i],guard);
                if(result==Numeric::complete)paletteVertexCache50KG.store(this,vertices[i],prepared[i],guard);
            }
            guardAny|=guard;
        }
        if(result==Numeric::complete)result=game::garage_palette50KG::projectPrepared(palette_,prepared,guardAny,polygon);
        break;}
    default:d.sourcePC=0x24b4bcu;return false;
    }
    const Numeric kernelResult=result;
    if(result==Numeric::complete)result=lowerOriginalUiGaragePrelitPacket50KG(polygon,out);
    if(shadow){
        auto& s=kernelShadow50KG;const int which=s.which;
        if(!shadowRawSize||shadowRawSize>sizeof shadowRaw||garage_kernel_shadow50KG::size(which,2)*3!=shadowRawSize||
           garage_kernel_shadow50KG::size(which,3)!=sizeof(polygon))kernelShadowMismatch50("layout",which,s.compared);
        game::garage_prelit50KG::Polygon other;
        const int status=garage_kernel_shadow50KG::project(which,s.preamble.data(),shadowRaw,&other);
        if(status!=int(kernelResult))kernelShadowMismatch50("project-status",which,s.compared);
        if(kernelResult==Numeric::complete){
            OriginalUiGaragePrelitPacket50KG lowered;
            const auto l=lowerOriginalUiGaragePrelitPacket50KG(other,lowered);
            if(l!=result||(l==Numeric::complete&&!samePacket50(lowered,out)))kernelShadowMismatch50("packet",which,s.compared);
        }
        if(!(++s.compared%200000ull))
            std::fprintf(stderr,"garage_kernel_shadow compared=%llu prepared=%llu mismatches=0\n",s.compared,s.prepared);
    }
    d.numeric=result;d.status=result==Numeric::complete?Status::complete:Status::numeric;
    d.sourcePC=0x24b50cu;return result==Numeric::complete;
}
}
