#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <utility>

namespace rr::assets::detail {
// Structural admission only. No image codec, cache identity, resource retain,
// palette, GPU state or renderer readiness is supplied by these descriptors.
struct OriginalIpumMipExtent50LA {
    std::uint32_t width=0,height=0;
    std::size_t header=0,payload=0,bytes=0;
};
struct OriginalIpumFrameExtent50LA {
    std::size_t offset=0,bytes=0;
    std::uint32_t count=0;
    std::array<OriginalIpumMipExtent50LA,5> mips{};
};
inline bool frameOriginalIpumAtm50LA(const std::vector<std::uint8_t>& source,
    std::vector<OriginalIpumFrameExtent50LA>& result,std::size_t& badOffset) {
    badOffset=0;
    const auto range=[&](std::size_t at,std::size_t n){return at<=source.size()&&n<=source.size()-at;};
    const auto word=[&](std::size_t at){return std::uint32_t(source[at])|
        (std::uint32_t(source[at+1])<<8)|(std::uint32_t(source[at+2])<<16)|(std::uint32_t(source[at+3])<<24);};
    const auto dimension=[](std::uint32_t n){return n>=16&&n<=1024&&!(n&(n-1));};
    if(source.size()>8u*1024u*1024u||!range(0,12)||word(0)!=1||
       !word(4)||word(4)>64||word(8)!=0)return false;
    std::vector<OriginalIpumFrameExtent50LA> next;next.reserve(word(4));
    std::size_t at=12;
    for(std::uint32_t frame=0;frame<word(4);++frame){
        badOffset=at;
        if(!range(at,12)||word(at)!=1||word(at+4)!=2||!word(at+8)||word(at+8)>5)return false;
        OriginalIpumFrameExtent50LA f;f.offset=at;f.count=word(at+8);at+=12;
        std::uint32_t priorWidth=0,priorHeight=0;
        for(std::uint32_t i=0;i<f.count;++i){
            badOffset=at;if(!range(at,20))return false;
            const auto h=word(at),w=word(at+4),bytes=word(at+16);
            if(!dimension(w)||!dimension(h)||word(at+8)||word(at+12)||
               (i&&(w!=priorWidth*2||h!=priorHeight*2))||bytes<25||!range(at+20,bytes))return false;
            const auto p=at+20;
            if(source[p]!='i'||source[p+1]!='p'||source[p+2]!='u'||source[p+3]!='m'||
               word(p+4)!=bytes-8||
               (std::uint32_t(source[p+8])|(std::uint32_t(source[p+9])<<8))!=w||
               (std::uint32_t(source[p+10])|(std::uint32_t(source[p+11])<<8))!=h||
               word(p+12)!=1||source[p+16]!=0)return false;
            constexpr std::array<std::uint8_t,8> end{{0,0,1,0xb0,0,0,1,0xb1}};
            for(std::size_t j=0;j<end.size();++j)if(source[p+bytes-8+j]!=end[j])return false;
            f.mips[i]={w,h,at,p,bytes};at=p+bytes;priorWidth=w;priorHeight=h;
        }
        f.bytes=at-f.offset;next.push_back(f);
    }
    badOffset=at;if(at!=source.size())return false;
    result=std::move(next);return true;
}
} // namespace rr::assets::detail
