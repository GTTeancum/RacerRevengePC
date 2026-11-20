#include "original_ui_course_background50kg.h"
#include <cstring>
#include <utility>
namespace rr::assets {
namespace {
std::uint32_t word(const std::uint8_t* p)noexcept{return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);}
void append(std::vector<std::uint8_t>& b,std::uint32_t n){for(unsigned i=0;i<4;++i)b.push_back(static_cast<std::uint8_t>(n>>(i*8)));}
}
const char* originalUiCourseBackgroundName50KG(std::uint32_t planet)noexcept{
    //1DDF00 jump table3AF300, not lexical planet order.
    switch(planet){case 0:return "UI\\Textures\\tat_bkg.pst";
    case 1:return "UI\\Textures\\gam_bkg.pst";
    case 2:return "UI\\Textures\\ryl_bkg.pst"; //3AF308 ->1DE000, string3AF278
    case 4:return "UI\\Textures\\sul_bkg.pst";
    case 5:return "UI\\Textures\\mon_bkg.pst";default:return nullptr;}
}
bool isOriginalUiCourseBackgroundIdentity50KG(const OriginalUiMemberIdentity50KG* source)noexcept{
    const auto* archive=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::course);
    if(!archive||!source||source->bytes!=287772)return false;
    bool canonical=false;for(std::size_t i=0;i<archive->memberCount;++i)
        canonical|=originalUiMemberIdentity50KG(archive->id,i)==source;
    if(!canonical)return false;
    for(auto planet:{0u,1u,2u,4u,5u})if(std::strcmp(source->name,originalUiCourseBackgroundName50KG(planet))==0)return true;
    return false;
}
const char originalUiAddonCourseBackgroundTag50DL[]="port-extension:dlc-course-background";
bool isOriginalUiAddonCourseBackgroundIdentity50DL(const OriginalUiMemberIdentity50KG* source)noexcept{
    return source&&source->bytes==287772&&source->name&&source->sha256==originalUiAddonCourseBackgroundTag50DL;
}
bool isOriginalUiCourseBackground50KG(const OriginalUiMemberView50KG& source)noexcept{
    return source.data&&source.size==287772&&(isOriginalUiCourseBackgroundIdentity50KG(source.identity)||
        isOriginalUiAddonCourseBackgroundIdentity50DL(source.identity));
}
bool decodeOriginalUiCourseBackground50KG(const OriginalUiMemberView50KG& source,
    Texture48E& output,DecodeError48C& diagnostic)noexcept{
    diagnostic={};if(!isOriginalUiCourseBackground50KG(source))return false;
    // Indexed palette interpretation retained from24EA00 and generic48E.
    // Only the five authenticated complete640x448 members extend NPOT admission.
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
