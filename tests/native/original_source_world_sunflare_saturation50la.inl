#pragma once
#include "../../src/game/original_sunflare_saturation_history50la.h"
#include <cstring>
#include <string_view>
namespace ra_cold_aggregate50LA {
inline bool Owner::continueSourceSunFlareSaturation2720D0(std::uint32_t& pc)noexcept {
    pc=0;
    if(!sourceWorldSunFlareSaturationInvocationCurrent()||sourceSunFlareSaturationFrame_)return false;
    sourceSunFlareSaturationFrame_.emplace();
    auto& f=*sourceSunFlareSaturationFrame_;auto& o=*nativeSunFlares_->owner;
    auto& config=*sourceWorldObjectConfig_;
    auto at=[&](std::uint32_t p){pc=sourceSunFlarePC_=p;};
    auto stop=[&](std::uint32_t p){at(p);sourceSunFlarePhase_=SourceSunFlarePhase::bounded;return false;};
    sourceSunFlarePhase_=SourceSunFlarePhase::saturationEntered;
    auto fetch=[&](const char* key,std::array<char,64>& text,bool& known,bool& found)noexcept {
        std::string_view value;std::uint32_t copy=0;a::OriginalStandardTextureDiagnostic50LA d;
        if(!config.readSourceSunFlareField158240(key,value,found,copy,d)){
            at(d.sourcePC?d.sourcePC:config.sourcePC_);return false;
        }
        if(!found){at(0x158640);return true;} //Existing caller buffer remains unchanged.
        at(copy);if(value.size()>=text.size())return false;
        std::memcpy(text.data(),value.data(),value.size());text[value.size()]=0;known=true;return true;
    };
    auto color=[&](const std::array<char,64>& text,bool known,SourceSunFlareColorCall& call)noexcept {
        at(0x26fa60);if(!known)return false;
        const std::string_view value(text.data());std::size_t cursor=0;
        constexpr std::uint32_t conversion[]{0x26fafc,0x26fb70,0x26fbe0,0x26fc40};
        constexpr std::uint32_t store[]{0x26fb1c,0x26fb90,0x26fc00,0x26fc54};
        //Bounded decimal-token adapter for the actual four-component TOB colors.
        //Each signed integer is in0..255, hence CVT.S.W is exactly representable.
        for(unsigned lane=0;lane<4;++lane){
            const auto end=value.find(',',cursor);
            if((lane<3&&end==std::string_view::npos)||(lane==3&&end!=std::string_view::npos)){
                at(conversion[lane]);return false;
            }
            const auto token=value.substr(cursor,end==std::string_view::npos?value.size()-cursor:end-cursor);
            at(conversion[lane]);if(token.empty())return false;
            std::uint32_t integer=0;
            for(const auto c:token){if(c<'0'||c>'9'||integer>255)return false;integer=integer*10u+unsigned(c-'0');}
            if(integer>255)return false;
            const float scalar=static_cast<float>(integer);std::uint32_t bits;std::memcpy(&bits,&scalar,4);
            at(store[lane]);call.stack40[lane]=bits;
            cursor=end==std::string_view::npos?value.size():end+1;
        }
        for(unsigned lane=0;lane<4;++lane){at(0x26fc68+4*lane);call.output[lane]=*call.stack40[lane];}
        return true;
    };
    auto format=[&](const char* prefix,std::uint32_t oneBased,std::uint32_t where)noexcept {
        at(where);const auto length=std::strlen(prefix);
        if(oneBased<1||oneBased>10||length+3>f.name350.size())return false;
        std::memcpy(f.name350.data(),prefix,length);std::size_t n=length;
        if(oneBased==10){f.name350[n++]='1';f.name350[n++]='0';}
        else f.name350[n++]=static_cast<char>('0'+oneBased);
        f.name350[n]=0;f.nameKnown=true;return true;
    };
    if(!sourceSunFlareStack_->saturation4A8)return stop(0x2720d0);
    if(*sourceSunFlareStack_->saturation4A8){
        bool found=false;
        at(0x2720ec);if(!fetch("saturationmincolor",f.min290,f.minKnown,found))return stop(pc);
        f.globalMinReturned=found;
        at(0x272108);if(!fetch("saturationmaxcolor",f.max2D0,f.maxKnown,found))return stop(pc);
        f.globalMaxReturned=found;
        //Authentic pair is absent. A newly present global-color branch is not
        //silently converted to the per-zone path or a fabricated parse result.
        if(*f.globalMinReturned&&*f.globalMaxReturned)return stop(0x272120);
        at(*f.globalMinReturned?0x2721c0:0x272114);f.zoneS2=0;
        static constexpr const char* minima[]={
            "saturationmincolorforzone1","saturationmincolorforzone2","saturationmincolorforzone3","saturationmincolorforzone4","saturationmincolorforzone5",
            "saturationmincolorforzone6","saturationmincolorforzone7","saturationmincolorforzone8","saturationmincolorforzone9","saturationmincolorforzone10"};
        static constexpr const char* maxima[]={
            "saturationmaxcolorforzone1","saturationmaxcolorforzone2","saturationmaxcolorforzone3","saturationmaxcolorforzone4","saturationmaxcolorforzone5",
            "saturationmaxcolorforzone6","saturationmaxcolorforzone7","saturationmaxcolorforzone8","saturationmaxcolorforzone9","saturationmaxcolorforzone10"};
        while(*f.zoneS2<10){
            const auto row=*f.zoneS2;
            if(!format("SaturationMinColorForZone",row+1,0x2721e0))return stop(pc);
            at(0x2721f4);if(!fetch(minima[row],f.min290,f.minKnown,found))return stop(pc);
            if(!found)return stop(0x272200); //Before unjoined retained-string parse continuation.
            auto& min=f.colors[2*row];
            if(!color(f.min290,f.minKnown,min))return stop(pc);
            constexpr std::uint32_t minStorePC[]{0x272220,0x272224,0x27222c};
            for(unsigned lane=0;lane<3;++lane){at(minStorePC[lane]);f.min3D0[lane]=*min.output[lane];}
            //The next formatting call's delay slot stores the last min lane.
            at(0x272240);f.min3D0[3]=*min.output[3];
            if(!format("SaturationMaxColorForZone",row+1,0x27223c))return stop(pc);
            at(0x272250);if(!fetch(maxima[row],f.max2D0,f.maxKnown,found))return stop(pc);
            if(!found)return stop(0x27225c);
            auto& max=f.colors[2*row+1];
            if(!color(f.max2D0,f.maxKnown,max))return stop(pc);
            for(unsigned lane=0;lane<4;++lane){at(0x27227c+4*lane);f.max3E0[lane]=*max.output[lane];}
            //Actual2C5F80 stack copies precede the shared first-use guard.
            for(unsigned lane=0;lane<4;++lane){at(0x2c5fac+4*lane);f.calleeMin30[lane]=*f.min3D0[lane];}
            for(unsigned lane=0;lane<4;++lane){at(0x2c5fcc+4*lane);f.calleeMax40[lane]=*f.max3E0[lane];}
            if(!rr::game::OriginalSunFlareSaturationHistory50LA::observe2C5FDC(pc))return stop(pc);
            sourceSunFlarePC_=pc;
            if(row>=o.zones30)return stop(0x2c6018);
            at(0x2c6060);o.flags98KnownValue=static_cast<std::uint8_t>((o.flags98KnownValue&~2u)|2u);
            for(unsigned lane=0;lane<4;++lane){
                at(0x2c6070+8*lane);if(!o.colors44)return stop(pc);
                const auto bits=*f.calleeMin30[lane];std::memcpy(&o.colors44[row][lane],&bits,4);
                o.sourceSaturation44Known[row]|=static_cast<std::uint8_t>(1u<<lane);
            }
            for(unsigned lane=0;lane<4;++lane){
                at(0x2c6098+8*lane);if(!o.colors48)return stop(pc);
                const auto bits=*f.calleeMax40[lane];std::memcpy(&o.colors48[row][lane],&bits,4);
                o.sourceSaturation48Known[row]|=static_cast<std::uint8_t>(1u<<lane);
            }
            at(0x2722a4);++*f.zoneS2;
        }
    }
    at(0x2722b8);++*sourceSunFlareIndex_;
    if(*sourceSunFlareIndex_!=*sourceSunFlareLimit_)return stop(0x2722c0);
    //Actual271DD0 returns through derived tail-jump1B4710 to this vC0 caller.
    at(0x271d80);sourceSunFlarePhase_=SourceSunFlarePhase::saturationReturned;return true;
}
}
