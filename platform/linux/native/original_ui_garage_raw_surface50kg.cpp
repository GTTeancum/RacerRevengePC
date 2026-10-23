#include "original_ui_garage_raw_surface50kg.h"
#include "original_ui_garage_model_resource50kg.h"
#include "../../../src/game/original_ui_lit_geometry50km.h"
#include <algorithm>
#include <utility>

namespace rr::platform {
namespace {
using S=OriginalUiGarageRawStatus50KG;
using D=OriginalUiGarageRawDiagnostic50KG;
bool fail(D& d,S s,std::uint32_t pc,std::size_t offset)noexcept{d={s,pc,offset};return false;}
std::uint16_t u16(const std::vector<std::uint8_t>& b,std::size_t p)noexcept{
    return std::uint16_t(unsigned(b[p])|(unsigned(b[p+1])<<8));
}
std::uint32_t u32(const std::vector<std::uint8_t>& b,std::size_t p)noexcept{
    return std::uint32_t(b[p])|(std::uint32_t(b[p+1])<<8)|
        (std::uint32_t(b[p+2])<<16)|(std::uint32_t(b[p+3])<<24);
}
std::int16_t i16(const std::vector<std::uint8_t>& b,std::size_t p)noexcept{
    const int v=u16(b,p);return static_cast<std::int16_t>(v<32768?v:v-65536);
}
std::int8_t i8(std::uint8_t b)noexcept{return static_cast<std::int8_t>(b<128?int(b):int(b)-256);}
bool zero(const std::vector<std::uint8_t>& b,std::size_t p,std::size_t end)noexcept{
    for(;p<end;++p){if(b[p])return false;}
    return true;
}
}
bool OriginalUiGarageRawSurface50KG::prepare(
    std::shared_ptr<const OriginalUiGarageModelResource50KG> model,
    std::uint32_t lod,std::uint32_t surfaceIndex,D& d)noexcept{
    if(model_||!model||!model->current()||!model->geometry()||
       !model->geometry()->holdsModelReference(&model->scene())||
       !model->source()||!model->source()->payload)
        return fail(d,S::phase,0x240e00,0);
    const auto* payload=model->geometry()->payload();
    const auto* authored=model->source()->payload.get();
    if(!payload||!payload->prepared()||!payload->scene()||!payload->originalBytes()||
       !payload->member()||!authored->member()||!authored->originalBytes()||
       *payload->member()!=*authored->member()||*payload->originalBytes()!=*authored->originalBytes())
        return fail(d,S::source,0x246344,0);
    // Warm clones legitimately share the canonical geometry's immutable
    // payload, rather than the new caller's equal decoded payload object.
    const auto& scene=*payload->scene();const auto& bytes=*payload->originalBytes();
    if(lod>=scene.lods.size()||surfaceIndex>=scene.lods[lod].surfaces.size())
        return fail(d,S::source,0x246344,0);
    const auto& surface=scene.lods[lod].surfaces[surfaceIndex];
    const auto* runtime=model->geometry()->surface246300(lod,surfaceIndex);
    if(!runtime||!runtime->material00||*runtime->material00<0||
       static_cast<std::uint32_t>(*runtime->material00)!=surface.material_index||
       runtime->sourceOffset!=surface.source_offset||runtime->uvCount0D!=1u||
       runtime->attributes0C!=static_cast<std::uint8_t>(surface.component_word))
        return fail(d,S::source,0x246344,surface.source_offset);
    const auto begin=surface.source_offset;
    if(begin>bytes.size()||bytes.size()-begin<20||surface.vertices.size()<3||
       surface.vertices.size()>game::originalUiLitMaximumSurfaceVertices50KM)
        return fail(d,S::capacity,0x246344,begin);
    const auto vertexCount=u16(bytes,begin+12);const auto size=u32(bytes,begin+16);
    const auto payloadBegin=begin+20;
    if(u32(bytes,begin)!=surface.material_index||u32(bytes,begin+4)!=surface.component_word||
       vertexCount!=surface.vertices.size()||u16(bytes,begin+14)!=surface.source_strips||
       !size||size%16||size>bytes.size()-payloadBegin||
       surface.material_index>=scene.materials.size()||!scene.nodes.size()||
       scene.nodes.size()>game::maxModelPaletteEntries49S||surface.indices.size()%3)
        return fail(d,S::source,0x246344,begin);
    const auto end=payloadBegin+size;
    std::size_t cursor=payloadBegin;std::uint32_t groups=0;std::size_t stripCursor=0;
    std::vector<Vertex> vertices;
    std::vector<std::array<std::uint32_t,3>> triangles;
    try{
        vertices.reserve(vertexCount);triangles.reserve(vertexCount);
        auto take=[&](std::uint32_t command,std::uint32_t count,std::uint32_t address,
            std::size_t elementBytes,std::size_t& location)noexcept{
            if(cursor>end||end-cursor<4||u32(bytes,cursor)!=((command<<24)|(count<<16)|address))
                return fail(d,S::source,0x24640c,cursor);
            cursor+=4;const auto length=elementBytes*count;const auto padded=(length+3)&~std::size_t(3);
            if(padded>end-cursor||!zero(bytes,cursor+length,cursor+padded))
                return fail(d,S::source,0x24640c,cursor);
            location=cursor;cursor+=padded;return true;
        };
        while(cursor<end){
            if(end-cursor<4||!u32(bytes,cursor))break;
            std::size_t tag=0,xyz=0,normal=0,uv=0,color=0;
            if(!take(0x6c,1,0x8000,16,tag))return false;
            const auto count=u32(bytes,tag)&0x7fffu;
            if(count<3||count>16||u32(bytes,tag)!=(0x8000u|count)||
               u32(bytes,tag+4)!=0x30024000u||u32(bytes,tag+8)!=0x412u||u32(bytes,tag+12)||
               count>vertexCount-vertices.size())return fail(d,S::source,0x24640c,tag);
            // The authentic Garage resource decoder admits V4_16 only. This
            // reader does not broaden it to V4_32 or reconstruct integer data.
            if(!take(0x6d,count,0x8001,8,xyz)||!take(0x6a,count,0x8001+count,3,normal)||
               !take(0x65,count,0x8001+2*count,4,uv))return false;
            if(surface.has_original_colors&&!take(0x6e,count,0xc001+3*count,4,color))return false;
            // The canonical PSG decoder requires this exact group-end command.
            if(cursor>end||end-cursor<4||u32(bytes,cursor)!=0x17000000u)
                return fail(d,S::source,0x24640c,cursor);
            cursor+=4;
            const auto base=static_cast<std::uint32_t>(vertices.size());
            for(std::uint32_t i=0;i<count;++i){
                Vertex v;v.paletteAdcWord=i16(bytes,xyz+8*i+6);
                const int word=v.paletteAdcWord<0?-int(v.paletteAdcWord):int(v.paletteAdcWord);
                if(!word||word%16||static_cast<std::size_t>(word/16)>scene.nodes.size())
                    return fail(d,S::source,0x24640c,xyz+8*i+6);
                for(unsigned a=0;a<3;++a){v.position[a]=i16(bytes,xyz+8*i+2*a);v.normal[a]=i8(bytes[normal+3*i+a]);}
                for(unsigned a=0;a<2;++a)v.uv[a]=i16(bytes,uv+4*i+2*a);
                if(surface.has_original_colors)for(unsigned a=0;a<4;++a)v.rgba[a]=bytes[color+4*i+a];
                const auto& semantic=surface.vertices[base+i];
                if(semantic.bone!=static_cast<unsigned>(word/16-1)||
                   (surface.has_original_colors&&semantic.color!=v.rgba))
                    return fail(d,S::source,0x24640c,xyz+8*i);
                vertices.push_back(v);
            }
            std::uint32_t start=0;
            while(start<count){
                if(start+2>=count||vertices[base+start].paletteAdcWord>=0||
                   vertices[base+start+1].paletteAdcWord>=0)
                    return fail(d,S::source,0x24640c,xyz+8*start);
                std::uint32_t finish=start+2;
                while(finish<count&&vertices[base+finish].paletteAdcWord>0){
                    const std::array<std::uint32_t,3> chronological{{base+finish-2,base+finish-1,base+finish}};
                    auto normalized=chronological;if((finish-start)&1u)std::swap(normalized[0],normalized[1]);
                    const auto index=triangles.size()*3;
                    if(index>surface.indices.size()||surface.indices.size()-index<3||
                       normalized[0]!=surface.indices[index]||normalized[1]!=surface.indices[index+1]||
                       normalized[2]!=surface.indices[index+2])return fail(d,S::source,0x24640c,xyz+8*finish);
                    triangles.push_back(chronological);++finish;
                }
                if(finish==start+2||stripCursor>=surface.strips.size())return fail(d,S::source,0x24640c,xyz+8*start);
                const auto& strip=surface.strips[stripCursor++];
                if(strip.group!=groups||strip.start!=start||strip.end!=finish||strip.vertex_start!=base+start)
                    return fail(d,S::source,0x24640c,xyz+8*start);
                start=finish;
            }
            ++groups;
        }
    }catch(...){return fail(d,S::allocation,0x24640c,cursor);}
    if(end-cursor>=16||!zero(bytes,cursor,end)||vertices.size()!=vertexCount||
       groups!=surface.groups||stripCursor!=surface.strips.size()||
       triangles.size()*3!=surface.indices.size()||triangles.empty())
        return fail(d,S::source,0x24640c,cursor);
    // No output or owner is published until every original group/strip and
    // trailing byte has been checked. Continuation groups remain distinct;
    // their repeated suppressed endpoints never become duplicate triangles.
    vertices_=std::move(vertices);triangles_=std::move(triangles);payload_=payload;runtimeSurface_=runtime;
    modelGeneration_=model->generation();lod_=lod;surface_=surfaceIndex;
    sourceOffset_=begin;materialSlot_=surface.material_index;hasStoredColors_=surface.has_original_colors;
    model_=std::move(model);d={S::complete,0x24640c,begin};return true;
}
bool OriginalUiGarageRawSurface50KG::current()const noexcept{
    return model_&&model_->current()&&model_->generation()==modelGeneration_&&model_->geometry()&&
        model_->geometry()->payload()==payload_&&model_->geometry()->holdsModelReference(&model_->scene())&&
        payload_&&payload_->prepared()&&runtimeSurface_&&
        model_->geometry()->surface246300(lod_,surface_)==runtimeSurface_&&!triangles_.empty();
}
bool OriginalUiGarageRawSurface50KG::describe(Metadata& out,D& d)const noexcept{
    if(!current())return fail(d,S::phase,0x24640c,sourceOffset_);
    out={static_cast<std::uint32_t>(triangles_.size()),materialSlot_,hasStoredColors_,runtimeSurface_};
    d={S::complete,0x24640c,sourceOffset_};return true;
}
bool OriginalUiGarageRawSurface50KG::triangle(std::uint32_t ordinal,std::array<Vertex,3>& out,D& d)const noexcept{
    if(!current()||ordinal>=triangles_.size())return fail(d,S::phase,0x24640c,sourceOffset_);
    const auto& t=triangles_[ordinal];std::array<Vertex,3> result{};
    for(unsigned i=0;i<3;++i){
        if(t[i]>=vertices_.size())return fail(d,S::source,0x24640c,sourceOffset_);
        result[i]=vertices_[t[i]];
    }
    out=result;d={S::complete,0x24640c,sourceOffset_};return true;
}
}
