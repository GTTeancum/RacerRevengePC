#pragma once
#include "../../src/assets/track_start50ad.h"
#include <cstring>
#include <string_view>
namespace source_world_light50LA {
// Private source wrappers over a caller-owned, real selected Config field view.
// Fetch is an aggregate-member lambda, never a public parser/metadata grant.
template<class Fetch> struct Reader {
    Fetch fetch;std::uint32_t& pc;
    bool text(const char* key,std::string_view fallback,char* output,std::size_t capacity,bool& found)noexcept {
        std::string_view value;std::uint32_t copy=0;
        if(!fetch(key,value,found,copy))return false;
        if(!found){value=fallback;copy=0x158b08;}
        pc=copy;if(!output||capacity<=value.size())return false;
        std::memcpy(output,value.data(),value.size());output[value.size()]=0;pc=0x158b14;return true;
    }
    static bool integerToken(std::string_view v,std::int32_t& out)noexcept {
        if(v=="T"){out=1;return true;}if(v=="F"){out=0;return true;}
        if(v.empty())return false;bool neg=v.front()=='-';std::size_t i=neg||v.front()=='+'?1:0;
        if(i==v.size()||(v[i]=='0'&&i+1<v.size()))return false;std::uint32_t n=0;
        for(;i<v.size();++i){if(v[i]<'0'||v[i]>'9'||n>1000000u)return false;n=n*10u+unsigned(v[i]-'0');}
        if(n>10000000u)return false;out=neg?-static_cast<std::int32_t>(n):static_cast<std::int32_t>(n);return true;
    }
    bool integer(const char* key,std::int32_t fallback,std::int32_t* out,bool& found)noexcept {
        std::string_view value;std::uint32_t copy=0;
        if(!fetch(key,value,found,copy))return false;
        std::int32_t next=fallback;
        if(found){pc=0x158a34;if(!integerToken(value,next))return false;
            pc=value=="T"?0x158968:value=="F"?0x158990:0x158a44;}
        else pc=0x158b60;
        if(!out)return false;*out=next;pc=0x158b68;return true;
    }
    static bool numberToken(std::string_view text,std::uint32_t& bits)noexcept {
        float value;if(!rr::assets::parseTrackNumber50AD(text.data(),text.size(),value))return false;
        std::memcpy(&bits,&value,4);return true;
    }
    bool number(const char* key,std::uint32_t fallback,std::uint32_t& out,bool& found)noexcept {
        std::string_view value;std::uint32_t copy=0;
        if(!fetch(key,value,found,copy))return false;
        if(found){pc=0x158bac;std::uint32_t next;if(!numberToken(value,next))return false;
            pc=0x158bc4;out=next;}
        else{pc=0x158bdc;out=fallback;}
        pc=0x158be4;return true;
    }
    bool vector(const char* key,const std::array<std::uint32_t,4>& fallback,
        std::array<std::uint32_t,4>* out,bool& found)noexcept {
        std::string_view value;std::uint32_t copy=0;
        if(!fetch(key,value,found,copy))return false;
        if(!found){for(unsigned i=0;i<3;++i){pc=0x158cd0+8*i;if(!out)return false;(*out)[i]=fallback[i];}return true;}
        std::size_t at=0;
        for(unsigned i=0;i<3;++i){const auto end=i<2?value.find(',',at):value.find('\n',at);
            const auto n=end==std::string_view::npos?value.size()-at:end-at;
            pc=i==0?0x158c64:i==1?0x158c88:0x158cac;
            std::uint32_t bits;if(!numberToken(value.substr(at,n),bits))return false;
            pc=i==0?0x158c7c:i==1?0x158ca0:0x158cc4;if(!out)return false;(*out)[i]=bits;
            if(i<2&&end==std::string_view::npos)return false;at=end==std::string_view::npos?value.size():end+1;
        }
        return true; // Source W never written by158C00.
    }
    bool color(const char* text,std::array<std::uint32_t,4>& out)noexcept {
        //Actual26FA60 operates on stack tokens first, then returns float4.
        //The admitted authored colors have three signed decimal integer tokens.
        const std::string_view value(text);std::array<std::uint32_t,4> next;
        std::size_t at=0;
        for(unsigned i=0;i<3;++i){const auto end=value.find(',',at);const auto n=end==std::string_view::npos?value.size()-at:end-at;
            pc=i==0?0x26fafc:i==1?0x26fb70:0x26fbe0;
            std::int32_t parsed;if(!integerToken(value.substr(at,n),parsed)||parsed<-255||parsed>255)return false;
            if(!numberToken(value.substr(at,n),next[i]))return false;
            if(i<2&&end==std::string_view::npos)return false;if(i==2&&end!=std::string_view::npos)return false;
            at=end==std::string_view::npos?value.size():end+1;
        }
        next[3]=0x437f0000u; //Exact absent fourth token26FC10/14 =>255.
        pc=0x26fc2c;out[0]=next[0];pc=0x26fc30;out[1]=next[1];
        pc=0x26fc34;out[2]=next[2];pc=0x26fc3c;out[3]=next[3];return true;
    }
};
} //namespace source_world_light50LA
