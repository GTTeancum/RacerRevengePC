#include "original_ui_unlock_background50pd.h"
#include <cstring>
#include <utility>
namespace rr::assets {
namespace {
std::uint32_t word(const std::uint8_t* p)noexcept{return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);}
void append(std::vector<std::uint8_t>& b,std::uint32_t n){for(unsigned i=0;i<4;++i)b.push_back(static_cast<std::uint8_t>(n>>(i*8)));}
}
bool isOriginalUiUnlockBackgroundIdentity50PD(const OriginalUiMemberIdentity50KG* source)noexcept{
    const auto* archive=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::unlock);
    if(!archive||!source||source->bytes!=287772)return false;
    bool canonical=false;for(std::size_t i=0;i<archive->memberCount;++i)
        canonical|=originalUiMemberIdentity50KG(archive->id,i)==source;
    if(!canonical)return false;
    for(const char* name:{"ui\\textures\\artpod01.pst","ui\\textures\\artpil01.pst","ui\\textures\\artpla01.pst"})
        if(std::strcmp(source->name,name)==0)return true;
    return false;
}
bool isOriginalUiUnlockBackground50PD(const OriginalUiMemberView50KG& source)noexcept{
    return source.data&&source.size==287772&&isOriginalUiUnlockBackgroundIdentity50PD(source.identity);
}
bool decodeOriginalUiUnlockBackground50PD(const OriginalUiMemberView50KG& source,
    Texture48E& output,DecodeError48C& diagnostic)noexcept{
    diagnostic={};if(!isOriginalUiUnlockBackground50PD(source))return false;
    // Same indexed palette interpretation as the Hall/Course 640x448 members
    // (24EA00 and generic 48E: CLUT index bits 3/4 swapped).
    const auto* p=source.data;
    if(word(p)!=1||word(p+4)!=1||word(p+8)!=1||word(p+1036)!=448||
       word(p+1040)!=640||word(p+1044)!=0||word(p+1048)!=0)return false;
    try{
        Texture48E result;result.kind=1;
        for(unsigned i=0;i<256;++i)for(unsigned c=0;c<4;++c)result.original_palette[i][c]=p[12+4*i+c];
        TextureMip48E mip;mip.width=640;mip.height=448;mip.source_header_offset=1036;mip.source_indices_offset=1052;
        mip.rgba.resize(640u*448u*4u);
        for(std::size_t i=0;i<640u*448u;++i){const auto index=p[1052+i];
            const auto entry=(index&~24u)|((index&8u)<<1)|((index&16u)>>1);
            for(unsigned c=0;c<4;++c)mip.rgba[4*i+c]=result.original_palette[entry][c];}
        result.base_rrtex={'R','R','T','E','X','0','0','1'};append(result.base_rrtex,640);append(result.base_rrtex,448);
        result.base_rrtex.insert(result.base_rrtex.end(),mip.rgba.begin(),mip.rgba.end());
        result.mips.push_back(std::move(mip));output=std::move(result);return true;
    }catch(...){return false;}
}
}
