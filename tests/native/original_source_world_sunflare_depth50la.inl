#pragma once
#include "../../src/game/original_smoke_numeric50rc.h"
#include <cfenv>
#include <cstring>
namespace ra_cold_aggregate50LA {
inline bool Owner::continueSourceSunFlareDepth2C5CF0(std::uint32_t& pc)noexcept {
    if(sourceSunFlarePhase_!=SourceSunFlarePhase::retrieveResourcesReturned||sourceSunFlareDepthFrame_||
       !nativeSunFlares_||!nativeSunFlares_->sourceMode||!nativeSunFlares_->owner)return false;
    sourceSunFlareDepthFrame_.emplace();auto& frame=*sourceSunFlareDepthFrame_;
    frame.entered=true;sourceSunFlarePhase_=SourceSunFlarePhase::depthEntered;
    auto at=[&](std::uint32_t p){pc=sourceSunFlarePC_=p;};
    auto stop=[&](std::uint32_t p){at(p);sourceSunFlarePhase_=SourceSunFlarePhase::bounded;return false;};
    //Unknown/revoked selection is NOT knownNULL and cannot manufacture the
    //source2C5F60 depth-zero branch. Presence does not read future matrices.
    at(0x2c5d00);if(!sourceSunFlareSelectedCameraPresent2C5CF0())return stop(pc);
    at(0x2c5d1c);frame.result20={{0u,0u,0u,0x3f800000u}}; //SQC2 vf0, W=1.
    std::array<std::uint32_t,16> projectionWords;at(0x2c5d38);
    if(!readSourceSunFlareCameraWords2C5CF0(0x80,16,projectionWords.data()))return stop(pc);
    for(unsigned i=0;i<16;i+=2){at(0x2c5d44);frame.projection30[i]=projectionWords[i];
        at(0x2c5d48);frame.projection30[i+1]=projectionWords[i+1];}
    at(0x2c5d64);frame.projection30[10]=0x3f800054u;
    at(0x2c5d74);frame.projection30[14]=0xbf800054u; //NEG.S exact sign change.
    std::array<std::uint32_t,16> viewWords;
    at(0x2c5d88);if(!readSourceSunFlareCameraWords2C5CF0(0xc0,16,viewWords.data()))return stop(pc);
    frame.viewC0=viewWords;
    //Explicit native finite/FE_TONEAREST instruction-step policy, NOT EE/VU
    //bit equivalence. No legacy computed-matrix/Session result is imported.
    at(0x2c5da0);if(std::fegetround()!=FE_TONEAREST)return stop(pc);
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ != 0)
    return stop(0x2c5da0);
#endif
    namespace numeric=g::original_smoke_numeric_detail50RC;
    numeric::Math math;
    const auto word=[](float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;};
    using Mat=std::array<float,16>;
    Mat projection,view;
    for(unsigned i=0;i<16;++i){projection[i]=numeric::bits(*frame.projection30[i]);view[i]=numeric::bits(viewWords[i]);}
    const auto product=[&](const Mat& x,const Mat& y,std::uint32_t firstPC,Mat& out){
        constexpr std::uint32_t rowOffset[]{0,20,40,56};
        for(unsigned row=0;row<4;++row)for(unsigned lane=0;lane<4;++lane){const auto p=firstPC+rowOffset[row];
            auto v=math.mul(y[lane],x[row*4],p);if(math.badPC)return false;
            v=math.add(v,math.mul(y[4+lane],x[row*4+1],p+4),p+4);if(math.badPC)return false;
            v=math.add(v,math.mul(y[8+lane],x[row*4+2],p+8),p+8);if(math.badPC)return false;
            out[row*4+lane]=math.add(v,math.mul(y[12+lane],x[row*4+3],p+12),p+12);if(math.badPC)return false;
        }return true;
    };
    Mat combined;if(!product(view,projection,0x2c5da0,combined))return stop(math.badPC);
    for(unsigned row=0;row<4;++row){at(0x2c5de8+4u*row);for(unsigned lane=0;lane<4;++lane)frame.combined70[row*4+lane]=word(combined[row*4+lane]);}
    std::array<std::uint32_t,16> screenWords;
    at(0x2c5e10);if(!readSourceSunFlareCameraWords2C5CF0(0x140,16,screenWords.data()))return stop(pc);
    frame.screen140=screenWords;Mat screenInput;
    for(unsigned i=0;i<16;++i)screenInput[i]=numeric::bits(screenWords[i]);
    Mat screen;if(!product(combined,screenInput,0x2c5e24,screen))return stop(math.badPC);
    for(unsigned row=0;row<4;++row){at(0x2c5e6c+4u*row);for(unsigned lane=0;lane<4;++lane)frame.screenB0[row*4+lane]=word(screen[row*4+lane]);}
    std::array<std::uint32_t,3> lookWords;at(0x2c5e94);
    if(!readSourceSunFlareCameraWords2C5CF0(0x190,3,lookWords.data()))return stop(pc);
    frame.look190=lookWords;
    constexpr std::uint32_t farPC[]{0x2c5ea0,0x2c5ea4,0x2c5eac};
    constexpr std::uint32_t scalePC[]{0x2c5ec8,0x2c5ed0,0x2c5ed8};
    std::array<float,3> farVector,scaled;
    for(unsigned i=0;i<3;++i){farVector[i]=math.mul(numeric::bits(0x46c80000),numeric::bits(lookWords[i]),farPC[i]);if(math.badPC)return stop(math.badPC);}
    for(unsigned i=0;i<3;++i){at(0x106790+4u*i);frame.far120[i]=word(farVector[i]);}
    at(0x1067a0);frame.far120[3]=0x3f800000;
    for(unsigned i=0;i<3;++i){scaled[i]=math.mul(numeric::bits(0x3f3504f3),farVector[i],scalePC[i]);if(math.badPC)return stop(math.badPC);}
    for(unsigned i=0;i<3;++i){at(0x106790+4u*i);frame.scaled110[i]=word(scaled[i]);}
    at(0x1067a0);frame.scaled110[3]=0x3f800000;
    std::array<std::uint32_t,3> positionWords;at(0x2c5f08);
    if(!readSourceSunFlareCameraWords2C5CF0(0x180,3,positionWords.data()))return stop(pc);
    frame.position180=positionWords;
    std::array<float,3> point;
    point[0]=math.add(numeric::bits(positionWords[0]),scaled[0],0x2c5f1c);if(math.badPC)return stop(math.badPC);
    point[1]=math.add(numeric::bits(positionWords[1]),scaled[1],0x2c5f20);if(math.badPC)return stop(math.badPC);
    at(0x2c5f24);frame.point100[0]=word(point[0]);
    point[2]=math.add(numeric::bits(positionWords[2]),scaled[2],0x2c5f28);if(math.badPC)return stop(math.badPC);
    at(0x2c5f2c);frame.point100[1]=word(point[1]);
    at(0x2c5f30);frame.point100[2]=word(point[2]);
    at(0x2c5f34);frame.point100[3]=0x3f800000;
    at(0x2c5f40);frame.inputF0=frame.point100;
    std::array<float,4> transformed;
    const float forcedW=math.add(1.f,0.f,0x14eb50);if(math.badPC)return stop(math.badPC);
    for(unsigned lane=0;lane<4;++lane){auto v=math.mul(screen[lane],point[0],0x14eb54);if(math.badPC)return stop(math.badPC);
        v=math.add(v,math.mul(screen[4+lane],point[1],0x14eb58),0x14eb58);if(math.badPC)return stop(math.badPC);
        v=math.add(v,math.mul(screen[8+lane],point[2],0x14eb5c),0x14eb5c);if(math.badPC)return stop(math.badPC);
        transformed[lane]=math.add(v,math.mul(screen[12+lane],forcedW,0x14eb60),0x14eb60);if(math.badPC)return stop(math.badPC);}
    at(0x14eb64);for(unsigned i=0;i<4;++i)frame.result20[i]=word(transformed[i]);
    const auto depth=math.div(transformed[2],transformed[3],0x2c5f4c);if(math.badPC)return stop(math.badPC);
    //Same explicitly native signed conversion policy as existing scalar owner;
    //not an unlabelled claim that host truncation is every original CVT.W.S.
    const auto integer=math.cvtWord(depth,0x2c5f50);if(math.badPC)return stop(math.badPC);
    at(0x2c5f5c);nativeSunFlares_->owner->depth40=integer;nativeSunFlares_->owner->sourceDepthStored=true;
    frame.returned=true;at(0x2720d0);sourceSunFlarePhase_=SourceSunFlarePhase::depthReturned;return true;
}
}
