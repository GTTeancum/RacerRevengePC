#include "original_ui_garage_model_lowering50kg.h"
#include "original_ui_garage_model_resource50kg.h"
#include <algorithm>
#include <utility>

namespace rr::platform {
bool OriginalUiGarageModelLowering50KG::prepare(
    std::shared_ptr<const OriginalUiGarageRawSurface50KG> raw,std::uint32_t ordinal,
    const game::OriginalRacerModelPacket50LA& packet,Program program,const Context& c,Diagnostic& d)noexcept {
    d={};const auto fail=[&](Status s,std::uint32_t pc){d.status=s;d.sourcePC=pc;return false;};
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
            result=game::garage_prelit50KG::prepare(geometryContext,prelit_);selected=Kernel::prelit;break;
        case Program::diffuseCompressed:{
            game::garage_diffuse50KG::Context context;
            context.geometry=geometryContext;context.lighting=camera;context.normalScale1E4=packet.scale40[1];
            result=game::garage_diffuse50KG::prepare(context,diffuse_);selected=Kernel::diffuse;break;}
        case Program::specularMapCompressed:{
            game::garage_specular50KG::Context context;
            context.geometry=geometryContext;context.lighting=camera;context.normalScale1E4=packet.scale40[1];
            result=game::garage_specular50KG::prepare(context,specular_);selected=Kernel::specular;break;}
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
    d={};if(!current()||ordinal>=triangleCount_){d.sourcePC=0x24b4a0u;return false;}
    std::array<OriginalUiGarageRawSurface50KG::Vertex,3> source;
    if(!raw_->triangle(ordinal,source,d.raw)){d.status=Status::raw;d.sourcePC=d.raw.sourcePC;return false;}
    game::garage_prelit50KG::Polygon polygon;Numeric result=Numeric::input;
    switch(kernel_){
    case Kernel::prelit:{
        std::array<game::garage_prelit50KG::RawVertex,3> vertices;
        for(unsigned i=0;i<3;++i){vertices[i].position=source[i].position;vertices[i].uv=source[i].uv;vertices[i].rgba=source[i].rgba;}
        result=game::garage_prelit50KG::projectTriangle(prelit_,vertices,polygon);break;}
    case Kernel::diffuse:{
        std::array<game::garage_diffuse50KG::RawVertex,3> vertices;
        for(unsigned i=0;i<3;++i){vertices[i].position=source[i].position;vertices[i].normal=source[i].normal;vertices[i].uv=source[i].uv;}
        result=game::garage_diffuse50KG::projectTriangle(diffuse_,vertices,polygon);break;}
    case Kernel::specular:{
        std::array<game::garage_specular50KG::RawVertex,3> vertices;
        for(unsigned i=0;i<3;++i){vertices[i].position=source[i].position;vertices[i].normal=source[i].normal;}
        result=game::garage_specular50KG::projectTriangle(specular_,vertices,polygon);break;}
    case Kernel::palette:{
        std::array<game::garage_palette50KG::RawVertex,3> vertices;
        for(unsigned i=0;i<3;++i){vertices[i].position=source[i].position;vertices[i].normal=source[i].normal;
            vertices[i].uv=source[i].uv;vertices[i].paletteAdcWord=source[i].paletteAdcWord;}
        result=game::garage_palette50KG::projectTriangle(palette_,vertices,polygon);break;}
    default:d.sourcePC=0x24b4bcu;return false;
    }
    if(result==Numeric::complete)result=lowerOriginalUiGaragePrelitPacket50KG(polygon,out);
    d.numeric=result;d.status=result==Numeric::complete?Status::complete:Status::numeric;
    d.sourcePC=0x24b50cu;return result==Numeric::complete;
}
}
