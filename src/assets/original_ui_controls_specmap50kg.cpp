#include "original_ui_controls_specmap50kg.h"
#include <cstring>
#include <utility>
namespace rr::assets {
bool isOriginalUiControlsSpecmap50KG(const OriginalUiMemberIdentity50KG* identity) noexcept {
    if(!identity||identity->bytes!=196652)return false;
    const auto* archive=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::options);
    if(!archive)return false;
    for(std::size_t i=0;i<archive->memberCount;++i){const auto* member=originalUiMemberIdentity50KG(archive->id,i);
        if(member==identity)return std::strcmp(member->name,"ui\\textures\\specmap3.tga")==0;}
    return false;
}
bool decodeOriginalUiControlsSpecmap50KG(const OriginalUiMemberView50KG& source,Texture48E& output,DecodeError48C& d) noexcept {
    d={};const auto fail=[&](std::size_t at){d={DecodeKind48C::unsupported,"controls.specmap_tga1555",at,"Only the authenticated Controls RGB24-to1555 source is admitted"};return false;};
    if(!isOriginalUiControlsSpecmap50KG(source.identity)||!source.data||source.size!=196652)return fail(0);
    const auto* p=source.data;
    if(p[0]||p[1]||p[2]!=2)return fail(0);
    for(unsigned i=3;i<12;++i)if(p[i])return fail(i);
    if(p[12]!=0||p[13]!=1||p[14]!=0||p[15]!=1||p[16]!=24||p[17]!=0)return fail(12);
    try{
        Texture48E next;next.kind=originalUiControlsTga1555Kind50KG;
        TextureMip48E mip;mip.width=mip.height=256;mip.source_header_offset=0;mip.source_indices_offset=18;
        mip.rgba.resize(256u*256u*4u);
        for(std::size_t y=0;y<256;++y)for(std::size_t x=0;x<256;++x){
            const auto src=18+((255-y)*256+x)*3,dst=(y*256+x)*4;
            //29EA90 first swaps B/R;2511B0->24E0E0 swaps back for
            //16C0D0's BGR888-to1555 packing. GS expands each5-bit lane
            //by left-shift3, never bit replication. Magenta's source key
            //retains the same RGB5 and chooses A=0; TA0=TA1=255/AEM0
            //therefore gives the same alpha255 as every A=1 pixel.
            mip.rgba[dst]=p[src+2]&0xf8u;mip.rgba[dst+1]=p[src+1]&0xf8u;
            mip.rgba[dst+2]=p[src]&0xf8u;mip.rgba[dst+3]=255;
        }
        if(!encodeTextureLevel48E(mip,next.base_rrtex,d))return false;
        next.mips.push_back(std::move(mip));output=std::move(next);return true;
    }catch(...){return fail(0);}
}
}
