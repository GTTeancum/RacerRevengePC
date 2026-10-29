#pragma once
#include <bit>
#include <cfenv>
#include <cmath>
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Source Portal numeric adaptation requires strict finite binary32 arithmetic"
#endif
namespace source_portal_native_numeric50LA {
//Authorized native finite/nearest arithmetic, NOT EE/FCSR/VU bit equivalence.
using Words=std::array<std::uint32_t,4>;
inline std::uint32_t bits(float x)noexcept{return std::bit_cast<std::uint32_t>(x);}
inline float value(std::uint32_t x)noexcept{return std::bit_cast<float>(x);}
struct Arithmetic {
    bool valid=true;
    float keep(float x)noexcept {valid=valid&&std::isfinite(x);return x;}
    bool admit(float a,float b)noexcept {valid=valid&&std::fegetround()==FE_TONEAREST&&std::isfinite(a)&&std::isfinite(b);return valid;}
    float add(float a,float b)noexcept {if(!admit(a,b))return 0;volatile float r=a+b;return keep(r);}
    float sub(float a,float b)noexcept {if(!admit(a,b))return 0;volatile float r=a-b;return keep(r);}
    float mul(float a,float b)noexcept {if(!admit(a,b))return 0;volatile float r=a*b;return keep(r);}
    float div(float a,float b)noexcept {if(!admit(a,b)||b==0){valid=false;return 0;}volatile float r=a/b;return keep(r);}
    float root(float x)noexcept {if(!admit(x,0)||x<0){valid=false;return 0;}volatile float r=std::sqrt(x);return keep(r);}
    bool cross(const Words& a,const Words& b,Words& out)noexcept {
        const float x=mul(value(a[1]),value(b[2]));const float y=mul(value(a[2]),value(b[0]));const float z=mul(value(a[0]),value(b[1]));
        const float rx=sub(x,mul(value(b[1]),value(a[2])));const float ry=sub(y,mul(value(b[2]),value(a[0])));const float rz=sub(z,mul(value(b[0]),value(a[1])));
        if(!valid)return false;out[0]=bits(rx);out[1]=bits(ry);out[2]=bits(rz);return true; //W intentionally not read/written.
    }
    float squared(const Words& a)noexcept {
        const float x=mul(value(a[0]),value(a[0]));const float y=mul(value(a[1]),value(a[1]));const float z=mul(value(a[2]),value(a[2]));
        const float one=add(0,1);const float xy=add(x,y);return add(xy,mul(one,z));
    }
    bool normalize(const Words& raw,Words& out,std::uint32_t& pc,std::uint32_t dotPC,std::uint32_t qPC,std::uint32_t mulPC)noexcept {
        pc=dotPC;const float d=squared(raw);if(!valid)return false;
        pc=qPC;if(d==0){valid=false;return false;}const float q=div(1,root(d));if(!valid)return false;
        pc=mulPC;const float x=mul(value(raw[0]),q),y=mul(value(raw[1]),q),z=mul(value(raw[2]),q);if(!valid)return false;
        out[0]=bits(x);out[1]=bits(y);out[2]=bits(z);return true;
    }
};
}
namespace ra_pod_scene50LA {
bool Owner::loadSourcePortal230450(std::size_t i,std::uint32_t& pc)noexcept {
    using namespace source_portal_native_numeric50LA;
    pc=0x230450;if(i>=10||!sourcePortalCohortAdmitted_||!sourcePortalFrames_[i].attempted||sourcePortalFrames_[i].loadAttempted)return false;
    auto& f=sourcePortalFrames_[i];f.loadAttempted=true;
    const auto at=[&](std::uint32_t p){pc=f.pc=p;};
    at(0x230468);
    if(!nativePortalParameters3A0_||!nativePortalParameters3A0_->sourceMode||nativePortalParameters3A0_->sourceReleased||
       !sourcePortal3A0_||!*sourcePortal3A0_||*sourcePortal3A0_!=nativePortalParameters3A0_->sourceArray.get())return false;
    const auto& param=(*sourcePortal3A0_)[i];
    for(std::size_t row=0;row<4;++row){at(0x230468);f.lastParameterLoad=param.vertices00[row];++f.parameterLoads;
        at(0x230474);if(!nativePortals_[i])return false;
        auto& p=*nativePortals_[i];if(!p.sourceMode||!p.source.constructorReturned)return false;
        p.source.vertices30[row]=*f.lastParameterLoad;p.vertexW[row]=(*f.lastParameterLoad)[3];++p.source.copiedVertices;
    }
    auto& p=*nativePortals_[i];auto& s=p.source;
    //262FD0 is a no-op.262FC0 writes the actual two-word temporaries;
    //subsequent scalar copies fill the loader's stack20..3C.
    constexpr std::uint32_t xy[4][2]={{0xbf800000,0xbf800000},{0xbf800000,0x3f800000},{0x3f800000,0x3f800000},{0x3f800000,0xbf800000}};
    constexpr std::uint32_t copyX[4]={0x2304d0,0x2304f8,0x23051c,0x230540};
    constexpr std::uint32_t copyY[4]={0x2304dc,0x230500,0x230528,0x230548};
    for(std::size_t row=0;row<4;++row){at(0x262fc0);s.loaderStack[16+row*2]=xy[row][0];at(0x262fc4);s.loaderStack[17+row*2]=xy[row][1];
        at(copyX[row]);s.loaderStack[8+row*2]=s.loaderStack[16+row*2];at(copyY[row]);s.loaderStack[9+row*2]=s.loaderStack[17+row*2];}
    if(!worldApplication_)return false;
    for(std::size_t row=0;row<4;++row){at(0x263100);worldApplication_->portalViewport434940_.xy[row][0]=value(*s.loaderStack[8+row*2]);sourcePortalViewportKnown_|=1u<<(row*2);
        at(0x263108);worldApplication_->portalViewport434940_.xy[row][1]=value(*s.loaderStack[9+row*2]);sourcePortalViewportKnown_|=1u<<(row*2+1);}
    s.quadCopied=true; //Do NOT set legacy portalViewportConstructed_.
    for(std::size_t row=0;row<4;++row){at(0x26374c);s.reorderStack[row*2]=bits(worldApplication_->portalViewport434940_.xy[row][0]);
        at(0x263754);s.reorderStack[row*2+1]=bits(worldApplication_->portalViewport434940_.xy[row][1]);}
    //Exact fixed source arithmetic proof: both signed determinants are -4,
    //all intermediates integers in[-4,4].263800 taken branch-likely returns
    //without any reorder output stores. No generic transactional helper.
    for(std::size_t row=0;row<4;++row)for(std::size_t k=0;k<2;++k)if(s.reorderStack[row*2+k]!=xy[row][k]){at(0x263780);return false;}
    at(0x26379c);s.quadAreaA=0xc0800000;at(0x2637e8);s.quadAreaB=0xc0800000;at(0x2639a8);s.quadReturned=true;
    Arithmetic a;const auto& v=s.vertices30;
    constexpr Words vf0{{0,0,0,0x3f800000}};
    at(0x230588);for(std::size_t k=0;k<4;++k)s.diagonalStack[k]=vf0[k];
    at(0x230590);for(std::size_t k=0;k<4;++k)s.diagonalStack[4+k]=vf0[k];
    //230580 scalar SUB/store ordering; a later refusal retains earlier stack stores.
    at(0x2305b0);float x=a.sub(value(v[2][0]),value(v[0][0]));if(!a.valid)return false;
    at(0x2305b4);float y=a.sub(value(v[2][1]),value(v[0][1]));if(!a.valid)return false;
    at(0x2305b8);s.diagonalStack[8]=bits(x);
    at(0x2305bc);float z=a.sub(value(v[2][2]),value(v[0][2]));if(!a.valid)return false;
    at(0x2305c0);s.diagonalStack[9]=bits(y);at(0x2305c4);s.diagonalStack[10]=bits(z);at(0x2305c8);s.diagonalStack[11]=0x3f800000;
    at(0x2305d0);Words d0;for(std::size_t k=0;k<4;++k){d0[k]=*s.diagonalStack[8+k];s.diagonalStack[k]=d0[k];}
    at(0x2305ec);x=a.sub(value(v[3][0]),value(v[1][0]));if(!a.valid)return false;
    at(0x2305f0);y=a.sub(value(v[3][1]),value(v[1][1]));if(!a.valid)return false;
    at(0x2305f4);s.diagonalStack[12]=bits(x);
    at(0x2305f8);z=a.sub(value(v[3][2]),value(v[1][2]));if(!a.valid)return false;
    at(0x2305fc);s.diagonalStack[13]=bits(y);at(0x230600);s.diagonalStack[14]=bits(z);at(0x230604);s.diagonalStack[15]=0x3f800000;
    at(0x23060c);Words d1;for(std::size_t k=0;k<4;++k){d1[k]=*s.diagonalStack[12+k];s.diagonalStack[4+k]=d1[k];}
    at(0x230614);const float d0sq=a.squared(d0);if(!a.valid)return false;
    at(0x230634);const float r0=a.root(d0sq);if(!a.valid)return false;at(0x230638);s.diagonalStack[17]=bits(r0);
    at(0x230644);const float d1sq=a.squared(d1);if(!a.valid)return false;
    at(0x230664);const float r1=a.root(d1sq);if(!a.valid)return false;at(0x230668);s.diagonalStack[18]=bits(r1);
    at(0x230674);const float maximum=r0>r1?r0:r1;at(0x23067c);s.diagonalStack[19]=bits(maximum);
    at(0x23068c);p.maximumDiagonalC0=maximum;s.diagonalReturned=true;
    //230690 first two difference vectors and their actual scalar/copy stores.
    at(0x2306ac);x=a.sub(value(v[1][0]),value(v[0][0]));if(!a.valid)return false;
    at(0x2306b8);y=a.sub(value(v[1][1]),value(v[0][1]));if(!a.valid)return false;
    at(0x2306bc);s.normalStack[8]=bits(x);
    at(0x2306c4);z=a.sub(value(v[1][2]),value(v[0][2]));if(!a.valid)return false;
    at(0x2306c8);s.normalStack[9]=bits(y);at(0x2306d0);s.normalStack[10]=bits(z);at(0x2306d4);s.normalStack[11]=0x3f800000;
    at(0x2306dc);Words first;for(std::size_t k=0;k<4;++k){first[k]=*s.normalStack[8+k];s.normalStack[k]=first[k];}
    at(0x2306f8);x=a.sub(value(v[2][0]),value(v[0][0]));if(!a.valid)return false;
    at(0x2306fc);y=a.sub(value(v[2][1]),value(v[0][1]));if(!a.valid)return false;
    at(0x230700);s.normalStack[12]=bits(x);
    at(0x230704);z=a.sub(value(v[2][2]),value(v[0][2]));if(!a.valid)return false;
    at(0x230708);s.normalStack[13]=bits(y);at(0x23070c);s.normalStack[14]=bits(z);at(0x230710);s.normalStack[15]=0x3f800000;
    at(0x230718);Words diagonal;for(std::size_t k=0;k<4;++k){diagonal[k]=*s.normalStack[12+k];s.normalStack[4+k]=diagonal[k];}
    at(0x230728);Words raw;if(!a.cross(first,diagonal,raw))return false;
    at(0x230730);for(std::size_t k=0;k<3;++k)s.normal70[k]=raw[k];s.normalKnown=7;s.normalCrossStored=true;
    //VSUB.w vf6,vf6,vf6 has no proven incoming W. SQC2 overwrites its
    //former constructorW1 with UNKNOWN; XYZ calculations never consume W.
    Words normalized;if(!a.normalize(raw,normalized,pc,0x230738,0x230748,0x230754)){f.pc=pc;return false;}
    at(0x230758);for(std::size_t k=0;k<3;++k)s.normal70[k]=normalized[k];s.normalKnown=7;s.normalNormalized=true;
    for(std::size_t edge=0;edge<4;++edge){const auto next=(edge+1)&3u;
        at(0x23079c);z=a.sub(value(v[next][2]),value(v[edge][2]));if(!a.valid)return false;
        at(0x2307cc);y=a.sub(value(v[next][1]),value(v[edge][1]));if(!a.valid)return false;
        at(0x230800);x=a.sub(value(v[next][0]),value(v[edge][0]));if(!a.valid)return false;
        at(0x230804);s.normalStack[16]=bits(x);at(0x230808);s.normalStack[17]=bits(y);at(0x23080c);s.normalStack[18]=bits(z);at(0x230810);s.normalStack[19]=0x3f800000;
        at(0x230818);Words delta;for(std::size_t k=0;k<4;++k){delta[k]=*s.normalStack[16+k];s.normalStack[k]=delta[k];}
        at(0x230828);if(!a.cross(delta,s.normal70,raw))return false;
        at(0x230830);for(std::size_t k=0;k<3;++k)s.edges80[edge][k]=raw[k];s.edgeKnown[edge]=7;s.edgeCrossStored[edge]=true;
        if(!a.normalize(raw,normalized,pc,0x230838,0x230848,0x230854)){f.pc=pc;return false;}
        at(0x230858);for(std::size_t k=0;k<3;++k)s.edges80[edge][k]=normalized[k];s.edgeKnown[edge]=7;s.edgeNormalized[edge]=true;
    }
    at(0x230578);s.loadReturned=true;return true; //Exact owner v0 at230568, no native geometry/loaded grant.
}
}
