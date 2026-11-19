#include "original_ui_rider_stat_projection50kg.h"
#include "original_ui_text_position50kg.h"
#include "../assets/dlc_registry50dl.h"
#include <cmath>

namespace rr::game {
namespace {
float add(float a,float b) noexcept {volatile float v=a+b;return v;}
float mul(float a,float b) noexcept {volatile float v=a*b;return v;}
bool count(float f,std::int32_t& n) noexcept {
    // 188D70/188DE0: separate MUL.S and ADD.S, then EE CVT.W.S truncation.
    if(!std::isfinite(f)||!roundOriginalUiTextMetric50KG(add(mul(20.f,f),.5f),1.f,n))return false;
    if(n>20)n=20;
    return n>=0; // explicit bounded domain; source negative counts are not clamped.
}
float gap(unsigned i) noexcept {return i<4?0.f:(i<8?12.f:24.f);}
bool stockRider(std::uint32_t r) noexcept {return r>=1&&r<=26&&r!=5&&r!=6&&r!=13;}
// PORT EXTENSION, NOT ORIGINAL (DLC): a registry add-on number (32..99) is a
// valid identity here; its stats come from the caller (the pod PHY) and it has
// no original portrait object, so none is emitted for it.
bool rider(std::uint32_t r) noexcept {return stockRider(r)||rr::assets::dlc50DL::addonNumberRange(r);}
}
bool projectOriginalUiRiderStats50KG(const OriginalUiRiderStatInput50KG& in,
    OriginalUiRiderStatProjection50KG& output) noexcept {
    if(!in.profileReady||!rider(in.selectedRider)||in.selectedRider!=in.observedRider||
       !std::isfinite(in.panelY4DC))return false;
    OriginalUiRiderStatProjection50KG p;
    if(!roundOriginalUiTextMetric50KG(add(320.f,in.panelY4DC),1.f,p.lockedWidgetY))return false;
    auto emit=[&](OriginalUiRiderStatKind50KG k,std::uint32_t off,std::uint32_t pc,
                  float x,float y,bool colored=false,std::uint32_t color=0,
                  float w=0,float h=0,std::uint16_t text=0) noexcept {
        if(p.count>=p.draws.size()||!std::isfinite(x)||!std::isfinite(y))return false;
        p.draws[p.count++]={k,off,pc,x,y,w,h,colored,color,text};return true;
    };
    using K=OriginalUiRiderStatKind50KG;
    const auto color=in.accentColor17FA00;
    if(in.mode34==2&&in.unlocked10FAC) {
        const float y=add(245.f,in.panelY4DC);
        for(unsigned i=0;i<13;++i) {
            const float x=add(260.f,add(mul(22.f,float(i)),gap(i)));
            if(!emit(K::medalOutline,0x540+i*0x60,0x1EBF40,x,y,true,color))return false;
            const auto m=in.medal1938[i];
            if(m>=1&&m<=3) {
                const K kinds[]={K::medalGold,K::medalSilver,K::medalBronze};
                if(!emit(kinds[m-1],0xA20+i*0x120+(m-1)*0x60,
                    m==1?0x1EBF94:(m==2?0x1EBFA8:0x1EBFBC),x,y))return false;
            }
        }
        const auto track=in.careerTrack219C&31u;
        const float x=track<13?add(gap(track),add(254.f,mul(22.f,float(track)))):641.f;
        const float yMarker=track<13?add(239.f,in.panelY4DC):449.f;
        if(!emit(K::careerMarker,0x18C0,0x1EC2D8,x,yMarker))return false;
    }
    const float panelY=add(in.mode34==2?244.f:251.f,in.panelY4DC);
    if(!emit(K::panelLeft,0x2340,in.mode34==2?0x1EC410:0x1EC65C,
        220.f,panelY,true,color,0,in.mode34==2?135.f:0.f)||
       !emit(K::panelRight,0x23A0,in.mode34==2?0x1EC588:0x1EC768,
        348.f,panelY,true,color,292.f,in.mode34==2?135.f:0.f))return false;
    if(in.unlocked10FAC) {
        if(stockRider(in.selectedRider)&&!emit(K::riderPortrait,0x1920+in.selectedRider*0x60,0x1EC85C,
            250.f,add(300.f,in.panelY4DC)))return false;
        for(unsigned i=0;i<6;++i) {
            if(!count(in.current1920[i],p.current[i])||!count(in.maximum0408[i],p.maximum[i]))return false;
            const float y=add(add(271.f,mul(18.f,float(i))),in.panelY4DC);
            for(int j=0;j<p.maximum[i];++j) {
                if(!emit(K::notch,0x2400+i*0x780+unsigned(j)*0x60,0x1ECAC8,
                    add(450.f,mul(7.f,float(j))),y,true,
                    j<p.current[i]?color:0x333333u,5.f,10.f))return false;
            }
        }
        for(unsigned i=0;i<6;++i)
            if(!emit(K::label,0x5100+i*0x70,0x1ECB9C,445.f,
                add(add(268.f,mul(18.f,float(i))),in.panelY4DC),true,color,
                0,0,std::uint16_t(200+i)))return false;
    }
    output=p;return true;
}
}
