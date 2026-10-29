#pragma once
#include "body_scene48g.h"
#include "spring_records48n.h"
#include "../assets/collision_mesh48i.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace rr::game {
class OriginalWreckQueries50LA;
// Actual type0 branches of11ED60 and its nested11F680. Caller authenticates
// SAME retained shape40, owned first node, stack20 and canonical matrix/sphere
// lanes. Finite nearest separate binary32 is an explicit native adaptation,
// not exact EE/VU arithmetic. Refusal preserves every preceding source store.
class OriginalWreckSourceType0Prepare50LA final {
    friend class OriginalWreckQueries50LA;
    static bool pass(bool second,const BodyMatrix48G& incoming,
        const BodyMatrix48G& actual40,const rr::assets::CollisionMeshNode48I& actualRoot,
        BodyMatrix48G& composed00,BodyMatrix48G& world80,BodyMatrix48G& previousC0,
        SpringVector4_48N& canonicalCenter,float& canonicalRadius,std::uint32_t& pc)noexcept {
        //First11ED60 creates both histories from incoming. Second11F680
        //copies the real previous world80 first. Raw stores precede arithmetic.
        const auto& old=second?world80:incoming;
        for(unsigned pair=0;pair<8u;++pair){
            pc=second?0x11f6e4u:0x11edc4u;
            std::memcpy(&previousC0.v[2u*pair],&old.v[2u*pair],4);
            pc=second?0x11f6e8u:0x11edc8u;
            std::memcpy(&previousC0.v[2u*pair+1u],&old.v[2u*pair+1u],4);
        }
        for(unsigned pair=0;pair<8u;++pair){
            pc=second?0x11f714u:0x11edf4u;
            std::memcpy(&world80.v[2u*pair],&incoming.v[2u*pair],4);
            pc=second?0x11f718u:0x11edf8u;
            std::memcpy(&world80.v[2u*pair+1u],&incoming.v[2u*pair+1u],4);
        }
        //All four VU output vectors are computed BEFORE the first SQ.
        //Do not expose a partly composed matrix after native arithmetic refusal.
        BodyMatrix48G result;
        constexpr std::uint32_t firstPC[4]={0x11ee28u,0x11ee3cu,0x11ee50u,0x11ee60u};
        constexpr std::uint32_t secondPC[4]={0x11f748u,0x11f75cu,0x11f770u,0x11f780u};
        for(unsigned row=0;row<4u;++row)for(unsigned lane=0;lane<4u;++lane){
            const auto base=second?secondPC[row]:firstPC[row];pc=base;
            if(std::fegetround()!=FE_TONEAREST||!std::isfinite(incoming.v[lane])||
               !std::isfinite(actual40.v[4u*row]))return false;
            volatile float value=incoming.v[lane]*actual40.v[4u*row];
            if(!std::isfinite(value))return false;
            for(unsigned term=1;term<4u;++term){
                pc=base+4u*term;
                if(!std::isfinite(incoming.v[4u*term+lane])||!std::isfinite(actual40.v[4u*row+term]))return false;
                volatile float product=incoming.v[4u*term+lane]*actual40.v[4u*row+term];
                if(!std::isfinite(product))return false;
                volatile float sum=value+product;if(!std::isfinite(sum))return false;
                value=sum;
            }
            result.v[4u*row+lane]=value;
        }
        for(unsigned row=0;row<4u;++row){
            pc=(second?0x11f790u:0x11ee70u)+4u*row;
            std::memcpy(&composed00.v[4u*row],&result.v[4u*row],16);
        }
        if(second){
            for(unsigned axis=0;axis<3u;++axis){
                pc=0x11f7b4u+8u*axis;
                std::memcpy(&canonicalCenter.v[axis],&actualRoot.center[axis],4);
            }
            pc=0x11f7c8u;canonicalCenter.v[3]=1.f;
        }else{
            //14EBB0(a2=3) copies incoming translation XYZ with VF0.W=1;
            //11EE9C stores this vector, NOT the tree center used in pass two.
            SpringVector4_48N translation{{0.f,0.f,0.f,1.f}};
            for(unsigned axis=0;axis<3u;++axis)
                std::memcpy(&translation.v[axis],&incoming.v[12u+axis],4);
            pc=0x11ee9cu;std::memcpy(&canonicalCenter,&translation,sizeof translation);
        }
        //The just-written local center is consumed only AFTER the radius store.
        pc=second?0x11f7fcu:0x11eed0u;
        if(!std::isfinite(actualRoot.half[0]))return false;
        volatile float xx=actualRoot.half[0]*actualRoot.half[0];if(!std::isfinite(xx))return false;
        pc=second?0x11f800u:0x11eed4u;
        if(!std::isfinite(actualRoot.half[1]))return false;
        volatile float yy=actualRoot.half[1]*actualRoot.half[1];if(!std::isfinite(yy))return false;
        pc=second?0x11f804u:0x11eed8u;
        volatile float xy=xx+yy;if(!std::isfinite(xy))return false;
        pc=second?0x11f808u:0x11eedcu;
        if(!std::isfinite(actualRoot.half[2]))return false;
        volatile float zz=actualRoot.half[2]*actualRoot.half[2];if(!std::isfinite(zz))return false;
        volatile float sum=xy+zz;if(!std::isfinite(sum))return false;
        pc=second?0x11f80cu:0x11eee0u;
        volatile float radius=std::sqrt(static_cast<float>(sum));if(!std::isfinite(radius))return false;
        pc=second?0x11f81cu:0x11eef0u;canonicalRadius=radius;
        SpringVector4_48N transformed{{0.f,0.f,0.f,1.f}};
        for(unsigned axis=0;axis<3u;++axis){
            pc=second?0x11f838u:0x11ef0cu;
            const auto x=canonicalCenter.v[0],y=canonicalCenter.v[1],z=canonicalCenter.v[2];
            const auto a=incoming.v[axis],b=incoming.v[4u+axis],c=incoming.v[8u+axis],d=incoming.v[12u+axis];
            if(!std::isfinite(x)||!std::isfinite(a))return false;
            volatile float ax=a*x;if(!std::isfinite(ax))return false;
            pc=second?0x11f83cu:0x11ef10u;
            if(!std::isfinite(y)||!std::isfinite(b))return false;
            volatile float by=b*y;if(!std::isfinite(by))return false;
            volatile float ab=ax+by;if(!std::isfinite(ab))return false;
            pc=second?0x11f840u:0x11ef14u;
            if(!std::isfinite(z)||!std::isfinite(c))return false;
            volatile float cz=c*z;if(!std::isfinite(cz))return false;
            volatile float abc=ab+cz;if(!std::isfinite(abc))return false;
            pc=second?0x11f844u:0x11ef18u;
            if(!std::isfinite(d))return false;
            volatile float value=abc+d;if(!std::isfinite(value))return false;
            transformed.v[axis]=value;
        }
        pc=second?0x11f84cu:0x11ef1cu;
        std::memcpy(&canonicalCenter,&transformed,sizeof transformed);
        pc=second?0x11fe28u:0x11ef24u;return true;
    }
};
} //namespace rr::game
