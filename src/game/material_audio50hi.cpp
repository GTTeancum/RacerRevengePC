#include "material_audio50hi.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>
namespace rr::game { namespace {
float add(float a,float b)noexcept{volatile float v=a+b;return v;}
float sub(float a,float b)noexcept{volatile float v=a-b;return v;}
float mul(float a,float b)noexcept{volatile float v=a*b;return v;}
float div(float a,float b)noexcept{volatile float v=a/b;return v;}
bool finite(float x)noexcept{return std::isfinite(x)&&(x==0.f||std::fpclassify(x)!=FP_SUBNORMAL);}
bool integer(float x,std::int32_t&v)noexcept{
    if(!finite(x)||x<-2147483648.f||x>=2147483648.f)return false;
    v=static_cast<std::int32_t>(x);return true;
}
bool random(std::uint32_t& state,std::int32_t maximum,std::int32_t&value)noexcept {
    if(maximum<0||maximum>65535)return false;
    state=state*0x41c64e6du+0x3039u;
    volatile float draw=static_cast<float>(state&0x7fffffffu);
    const auto number=mul(mul(0x1p-31f,draw),static_cast<float>(maximum+1));
    return integer(number,value)&&value<=maximum;
}
bool driveControls(const MaterialDriveParameters50HI&p,std::int32_t speed,
    std::int32_t& slope,std::uint32_t&rng,float&gain,float&pitch)noexcept {
    if(p.sampleSpeed<=0||p.basePitch<1||p.basePitch>65535||!finite(p.frequencySlope)||
       p.frequencySlope<0.f||p.minVolume<0||p.minVolume>100||
       p.minSpeed==p.resonantSpeed||p.maxSpeed==p.resonantSpeed)return false;
    if(slope<0&&!integer(mul(static_cast<float>(p.basePitch/static_cast<std::uint32_t>(p.sampleSpeed)),p.frequencySlope),slope))return false;
    std::int32_t draw=0;if(!random(rng,slope,draw))return false;
    const auto delta=std::int64_t(speed)-p.sampleSpeed;
    const auto offset=delta*slope-draw;
    if(offset<std::numeric_limits<std::int32_t>::min()||offset>std::numeric_limits<std::int32_t>::max())return false;
    std::int32_t pitchWord=0;
    if(!integer(add(static_cast<float>(p.basePitch),static_cast<float>(offset)),pitchWord)||pitchWord<=0)return false;
    float volume=0.f;
    if(speed<=p.resonantSpeed){
        const auto gradient=div(sub(100.f,static_cast<float>(p.minVolume)),sub(static_cast<float>(p.resonantSpeed),static_cast<float>(p.minSpeed)));
        volume=add(static_cast<float>(p.minVolume),sub(mul(gradient,static_cast<float>(speed)),mul(gradient,static_cast<float>(p.minSpeed))));
    }else{
        const auto gradient=div(-sub(100.f,static_cast<float>(p.minVolume)),sub(static_cast<float>(p.maxSpeed),static_cast<float>(p.resonantSpeed)));
        volume=add(static_cast<float>(p.minVolume),sub(mul(gradient,static_cast<float>(speed)),mul(gradient,static_cast<float>(p.maxSpeed))));
    }
    std::int32_t volumeWord=0;if(!integer(std::clamp(volume,0.f,100.f),volumeWord))return false;
    // 104140/233AE0 initially writes spatially attenuated volume44, but
    // source292F30 then calls233750 with raw volume and overwrites44. New
    // starts233410(flags0) likewise use raw38 and overwrite44. Preserve the
    // final source gain rather than applying a second invented attenuation.
    gain=div(static_cast<float>(volumeWord),100.f);
    pitch=div(static_cast<float>(pitchWord),static_cast<float>(p.basePitch));return finite(gain)&&finite(pitch);
}
}
bool resolveContactMaterial50HI(std::uint32_t id,const ContactMaterial50HI*bindings,
    std::size_t count,std::int32_t&out)noexcept {
    if(count>1024||(!bindings&&count))return false;
    if(!id){out=-1;return true;}
    const ContactMaterial50HI*found=nullptr;
    for(std::size_t i=0;i<count;++i)if(bindings[i].candidateIdentity==id){if(found)return false;found=&bindings[i];}
    if(!found)return false;
    out=found->materialA9;return true;
}
AudioEventStatus50HE updateMaterialAudio50HI(const MaterialAudioParameters50HI&p,
    const MaterialAudioInput50HI&in,const MaterialAudioState50HI&previous,MaterialAudioState50HI&output,
    std::uint32_t&randomState,MaterialAudioCommand50HI*commands,std::size_t capacity,std::size_t&count)noexcept {
    using S=AudioEventStatus50HE;
    if((!commands&&capacity)||capacity>1024||in.channelArgument<1||in.channelArgument>3||std::fegetround()!=FE_TONEAREST)return S::Input;
    auto next=previous;auto rng=randomState;MaterialAudioCommand50HI staged[maxMaterialAudioCommands50HI]{};std::size_t n=0;
    auto emit=[&](AudioOperation50HE operation,MaterialCueKind50HI kind,std::uint32_t key,float gain,float pitch,bool loop,std::uint32_t source){staged[n++]={operation,kind,key,gain,pitch,loop,source};};
    auto stop=[&](){if(next.driveKey244>0){if(next.driveKey244>8)return false;emit(AudioOperation50HE::Stop,MaterialCueKind50HI::DriveOn,static_cast<std::uint32_t>(next.driveKey244),0.f,1.f,true,0x2c27d0);}next.driveMaterial240=next.driveKey244=-1;return true;};
    if(in.vehicle220Present){
        if(in.channelArgument==3){next.emitterFlags22C&=static_cast<std::uint8_t>(~8u);if(!stop())return S::Input;}
        else if(in.contactMaterialA9>=0){
            // TA has two four-variant sets. Unknown positive materials are a
            // scope error, never a fallback to a different material family.
            if(in.contactMaterialA9>1)return S::Input;
            const bool collision=in.channelArgument==1||!(next.emitterFlags22C&8u);
            if(in.channelArgument==2)next.emitterFlags22C|=8u;
            if(!in.disabled24C){
                if(!finite(in.listenerDistance)||in.listenerDistance<0||!finite(in.speedRatio70))return S::Input;
                std::int32_t speed=0;if(!integer(mul(100.f,in.speedRatio70),speed))return S::Input;
                if(in.listenerDistance<540.f){
                    if(collision){
                        std::int32_t variant=0;if(!random(rng,3,variant))return S::Input;
                        if(in.listenerDistance<add(400.f,static_cast<float>(speed))){
                            const auto volume=std::clamp(std::max(speed,20),0,100);
                            emit(AudioOperation50HE::Start,MaterialCueKind50HI::Collision,
                                static_cast<std::uint32_t>(4*in.contactMaterialA9+variant+1),
                                div(static_cast<float>(volume),100.f),1.f,false,0x2c247c);
                        }
                    }
                    if(in.channelArgument==2){
                        const bool changed=next.driveMaterial240!=in.contactMaterialA9;
                        if(changed){
                            if(!stop())return S::Input;
                            std::int32_t variant=0;if(!random(rng,3,variant))return S::Input;
                            next.driveMaterial240=in.contactMaterialA9;
                            next.driveKey244=4*in.contactMaterialA9+variant+1;
                        }
                        if(next.driveKey244<1||next.driveKey244>8)return S::Input;
                        if(in.listenerDistance<450.f){
                            const auto key=static_cast<std::uint32_t>(next.driveKey244);float gain=0,pitch=0;
                            if(!driveControls(p.drive[key-1],std::max(speed,50),next.cachedSlope20[key-1],rng,gain,pitch))return S::Input;
                            emit(changed?AudioOperation50HE::Start:AudioOperation50HE::Controls,
                                MaterialCueKind50HI::DriveOn,key,gain,pitch,true,changed?0x2c278c:0x2c2600);
                        }
                    }
                }
            }
        }
    }
    if(n>capacity||(!commands&&n))return S::Capacity;
    if(n)std::memcpy(commands,staged,n*sizeof(*commands));
    output=next;randomState=rng;count=n;return S::Complete;
}
}
