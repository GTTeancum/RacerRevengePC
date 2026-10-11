#include "original_ui_rider_stat_draw50kg.h"
#include <cmath>
#include <utility>
namespace rr::platform {
namespace {
bool owns(const assets::OriginalUiRiderStatAssets50KG& cold,const assets::OriginalUiRiderStatResource50KG* resource) noexcept {
    if(!resource)return false;
    for(unsigned i=0;i<=static_cast<unsigned>(assets::OriginalUiRiderStatAsset50KG::labelFont);++i)
        if(cold.resource(static_cast<assets::OriginalUiRiderStatAsset50KG>(i))==resource)return true;
    for(std::uint8_t rider=1;rider<=26;++rider)if(cold.portrait(rider)==resource&&resource)return true;
    return false;
}
bool finite(const game::OriginalUiTextVertex50KG& v) noexcept {
    return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.u)&&std::isfinite(v.v);
}
}
bool OriginalUiRiderStatDraw50KG::project(std::shared_ptr<const assets::OriginalUiRiderStatAssets50KG> cold,
    const game::OriginalUiRiderStatFrame50KG& frame,std::shared_ptr<const assets::OriginalUiImageBank50KG> bank,
    std::uint32_t owner,OriginalUiRiderStatDrawDiagnostic50KG& d) noexcept {
    d={};using E=OriginalUiRiderStatDrawDiagnostic50KG::Error;
    const auto fail=[&](E error){d.error=error;return false;};
    if(!cold||!cold->loaded()||!cold->images()||!bank||!bank->loaded()||!bank->images()||
       frame.packets.size()>originalUiMaximumSpans50KG)return fail(E::input);
    try {
        std::vector<assets::Vertex48B> vertices;std::vector<OriginalUiRiderStatDrawPacket50KG> packets;
        vertices.reserve(2048);packets.reserve(frame.packets.size());
        const auto* sourceTextures=cold->images()->textures();const auto combined=bank->images();
        const auto* textures=combined->textures();if(!sourceTextures||!textures)return fail(E::resource);
        for(const auto& source:frame.packets){d.objectOffset=source.objectOffset;
            if(!owns(*cold,source.resource)||source.resource->material.passes.size()!=1)return fail(E::resource);
            bool declared=false;for(auto index:source.resource->images)if(index==source.imageIndex)declared=true;
            if(!declared)return fail(E::resource);
            const auto* mapping=bank->mapping(owner,source.imageIndex);
            const auto* original=sourceTextures->texture(source.imageIndex);
            const auto* image=mapping?textures->texture(mapping->combinedIndex):nullptr;
            const auto* decoded=mapping?combined->decodedTexture(mapping->combinedIndex):nullptr;
            if(!mapping||!original||!image||!decoded||mapping->sourceIdentity!=original->identity||
               mapping->combinedIdentity!=image->identity)return fail(E::mapping);
            const auto* originalSource=cold->images()->sourceProvenance(source.imageIndex);
            const auto* combinedSource=combined->sourceProvenance(mapping->combinedIndex);
            if(!originalSource||!combinedSource||originalSource->memberIdentity!=combinedSource->memberIdentity||
               originalSource->celIdentity!=combinedSource->celIdentity||originalSource->pstOffset!=combinedSource->pstOffset||
               originalSource->pstBytes!=combinedSource->pstBytes)return fail(E::mapping);
            OriginalUiRiderStatDrawPacket50KG packet{};
            packet.firstQuad=static_cast<std::uint32_t>(vertices.size()/4);packet.imageIndex=mapping->combinedIndex;
            packet.objectOffset=source.objectOffset;packet.sourcePC=source.sourcePC;packet.resource=source.resource;
            const auto& pass=source.resource->material.passes[0];
            if(!assets::prepareOriginalUiNativeMaterial50KG(pass,*decoded,packet.policy,d.material))return fail(E::material);
            if(source.labelIndex>=0){
                const auto label=static_cast<unsigned>(source.labelIndex);
                if(label>=6||source.resource!=cold->resource(assets::OriginalUiRiderStatAsset50KG::labelFont))return fail(E::geometry);
                const auto& text=frame.labels[label];const auto& color=frame.labelColors[label];
                if(text.atlas!=&original->texture||text.count>game::originalUiTextCapacity50KG||color[3]>128)return fail(E::geometry);
                packet.quadCount=text.count;
                if(vertices.size()/4+text.count>originalUiMaximumQuads50KG)return fail(E::capacity);
                for(unsigned q=0;q<text.count;++q)for(const auto& glyph:text.quads[q].vertices){
                    if(!finite(glyph))return fail(E::geometry);
                    assets::Vertex48B vertex{};vertex.position[0]=glyph.x;vertex.position[1]=glyph.y;vertex.position[2]=16777214.f;
                    vertex.uv[0]=glyph.u;vertex.uv[1]=glyph.v;
                    for(unsigned c=0;c<4;++c)vertex.color[c]=color[c];vertices.push_back(vertex);
                }
            }else {
                if(source.labelIndex!=-1||source.sprite.authoredDepth!=1||source.sprite.sourcePacketColor[3]>128)
                    return fail(E::geometry);
                if(vertices.size()/4>=originalUiMaximumQuads50KG)return fail(E::capacity);
                constexpr unsigned order[]={0,1,3,2};packet.quadCount=1;
                for(unsigned i:order){const auto& v=source.sprite.vertices[i];
                    if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.u)||!std::isfinite(v.v))return fail(E::geometry);
                    assets::Vertex48B vertex{};vertex.position[0]=v.x;vertex.position[1]=v.y;vertex.position[2]=16777214.f;
                    vertex.uv[0]=v.u;vertex.uv[1]=v.v;
                    for(unsigned c=0;c<4;++c)vertex.color[c]=source.sprite.sourcePacketColor[c];vertices.push_back(vertex);
                }
            }
            if(packet.quadCount)packets.push_back(packet);
        }
        assets_=std::move(cold);bank_=std::move(bank);vertices_=std::move(vertices);packets_=std::move(packets);d={};return true;
    }catch(...){return fail(E::allocation);}
}
void OriginalUiRiderStatDraw50KG::drain() noexcept {packets_.clear();vertices_.clear();bank_.reset();assets_.reset();}
}
