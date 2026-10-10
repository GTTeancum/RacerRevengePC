#pragma once
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace rr::game {
// Borrowed source projections only. No constructor, resource decoder, time
// advance, frame selection, or bucket update is supplied by this query leaf.
// All spans and fields must describe one stable, authentic water-owner state.
struct WaterBytes50JC {
    const std::uint8_t* data=nullptr;
    std::size_t size=0;
    bool bound=false; // Bound includes the original format, not just storage.
};
struct WaterBucket50JC {
    float inverseScale04=0,originX10=0,originZ18=0;
    std::int32_t activeBuffer30=0;
    WaterBytes50JC buffers38[2]{};
    bool bound=false;
};
struct WaterSignature50JC {
    std::uint32_t dimension04=0,rowStride08=0,pixelStride0C=0;
    float extentX10=0,extentZ14=0,inverseX18=0,inverseZ1C=0,scale20=0;
    WaterBytes50JC current2C{},next30{};
    float originXC0=0,originZC4=0;
    std::uint32_t infiniteD4=0,channelD8=0,effect110=0;
    // Last genuine render-side +32C ->2BEBD0 publication. Time stepping
    // must preserve this flag and the prior origins, not republish them.
    bool originsPublished2BEBD0=false;
    bool bound=false;
};
struct WaterSignatureOwner50JC {
    // Exact original null-terminated +328 order, projected to count. A bound
    // empty list is valid; an unbound/missing list is never treated as empty.
    const WaterSignature50JC* signatures328=nullptr;
    std::size_t count=0;
    bool completeListBound=false;
    // Actual C4 time state, produced by constructor/setup and 2B6F00/2BEFA0.
    // Preserve pause/app+11C behavior and the original update/query ordering.
    const float* time388=nullptr;
    const float* phase34C=nullptr;
    const float* phase350=nullptr;
    const float* complement360=nullptr;
    const float* complement364=nullptr;
    bool sourceTimeStateBound=false;
};
struct WaterHeightGlobals50JC {
    const std::int32_t* renderInfinitePlane3834E0=nullptr;
    const float* wakeScale443B88=nullptr;
    const float* waterHeightRange443B90=nullptr; // Includes real load-time *16.
    const float* poolBase443E90=nullptr;
    const float* poolDepth443B98=nullptr;
    // These are fields of the actual shared pointer443B80, which must not be
    // assumed identical to the provider passed to the query.
    const float* channelScale98=nullptr;
    const float* channelScale9C=nullptr;
    const float* channelScaleA0=nullptr;
    const float* channelScaleA4=nullptr;
};
enum class WaterNoisePresence50JC : std::uint8_t {Unbound,Absent,Present};
struct WaterHeightProvider50JC {
    const float* baseHeight3C=nullptr;
    const WaterBucket50JC* buckets40=nullptr;
    std::int32_t count4C=0,dimensionB0=0,blocksPerRowB4=0;
    bool bucketsBound=false;
    WaterNoisePresence50JC noisePresence=WaterNoisePresence50JC::Unbound;
    WaterBytes50JC noise5C{};
    std::int32_t noiseDimensionAC=0,noisePixelStrideCC=0;
    float noiseCoordinateScaleBC=0;
    const float* heightScaleC0=nullptr;
    const WaterSignatureOwner50JC* signaturesC4=nullptr;
    bool sourceStateBound=false;
};
enum class WaterHeightStatus50JC : std::uint8_t {
    Complete,Owner,Arithmetic,Storage,Unsupported
};
struct WaterHeightResult50JC {
    WaterHeightStatus50JC status=WaterHeightStatus50JC::Owner;
    std::uint32_t sourcePC=0;
    std::size_t index=0;
};
namespace water_height_detail50jc {
using Status=WaterHeightStatus50JC;
inline WaterHeightResult50JC fail(Status s,std::uint32_t pc,std::size_t i=0) noexcept {
    return {s,pc,i};
}
struct Math {
    std::uint32_t badPC=0;
    float read(float x,std::uint32_t pc) noexcept {
        std::uint32_t b=0;std::memcpy(&b,&x,sizeof(b));
        if(!badPC&&(!std::isfinite(x)||
            ((b&0x7f800000u)==0&&(b&0x7fffffffu)!=0)))badPC=pc;
        return x;
    }
    float add(float x,float y,std::uint32_t pc) noexcept {
        read(x,pc);read(y,pc);volatile float z=x+y;return read(z,pc);
    }
    float sub(float x,float y,std::uint32_t pc) noexcept {
        read(x,pc);read(y,pc);volatile float z=x-y;return read(z,pc);
    }
    float mul(float x,float y,std::uint32_t pc) noexcept {
        read(x,pc);read(y,pc);volatile float z=x*y;return read(z,pc);
    }
    std::int32_t cvtWord(float x,std::uint32_t pc) noexcept {
        read(x,pc);
        // EE CVT.W.S truncation; signed overflow/exception domain is rejected.
        if(!std::isfinite(x)||x< -2147483648.0f||x>=2147483648.0f){
            if(!badPC)badPC=pc;return 0;
        }
        return static_cast<std::int32_t>(x);
    }
    float integer(std::int32_t x,std::uint32_t pc) noexcept {
        volatile float z=static_cast<float>(x);return read(z,pc);
    }
};
inline WaterHeightResult50JC arithmetic(const Math& m) noexcept {
    return fail(Status::Arithmetic,m.badPC);
}
inline bool at(const WaterBytes50JC& b,std::uint64_t index) noexcept {
    return b.bound&&b.data&&index<b.size;
}
// Exact 2B72B0 channel accumulator order, including current/next interpolation
// for channels0/1 and current-only contributions for channels2/3.
inline WaterHeightResult50JC signatures(const WaterSignatureOwner50JC& s,
    const WaterHeightGlobals50JC& g,float x,float z,float& out,
    std::uint32_t& marker,Math& m) noexcept {
    if(!s.completeListBound||(s.count&&!s.signatures328)||
        !s.sourceTimeStateBound||!s.time388||!s.phase34C||!s.phase350||
        !s.complement360||!s.complement364||!g.channelScale98||
        !g.channelScale9C||!g.channelScaleA0||!g.channelScaleA4)
        return fail(Status::Owner,0x2b7538u);
    // This establishes a bound time owner; the query does not advance/read it
    // arithmetically. The phase factors and buffers are its already-live state.
    float sum[4]{},next[2]{};marker=0;
    for(std::size_t i=0;i<s.count;++i){
        const auto& w=s.signatures328[i];
        if(!w.bound)return fail(Status::Owner,0x2b72dcu,i);
        if(w.channelD8==4u)continue;
        if(w.channelD8>4u)return fail(Status::Unsupported,0x2b7484u,i);
        if(!w.originsPublished2BEBD0)return fail(Status::Owner,0x2bec50u,i);
        const float dx=m.sub(x,w.originXC0,0x2b730cu);
        const float dz=m.sub(z,w.originZC4,0x2b7320u);
        if(!w.infiniteD4){
            const float ex=m.read(w.extentX10,0x2b7330u);
            const float ez=m.read(w.extentZ14,0x2b7378u);
            if(m.badPC)return arithmetic(m);
            if(dx<0.f||dx>ex||dz<0.f||dz>ez)continue;
        }
        // The source converts dimension as unsigned. This bounded dimension
        // range is exactly representable and excludes unsigned/size overflow.
        if(!w.dimension04||w.dimension04>0x01000000u)
            return fail(Status::Unsupported,0x2b73b0u,i);
        const float n=m.integer(static_cast<std::int32_t>(w.dimension04),0x2b73c0u);
        const auto ix=m.cvtWord(m.mul(w.inverseX18,m.mul(n,dx,0x2b73f4u),0x2b73f8u),0x2b73fcu);
        const auto iz=m.cvtWord(m.mul(w.inverseZ1C,m.mul(n,dz,0x2b740cu),0x2b741cu),0x2b7428u);
        if(m.badPC)return arithmetic(m);
        // Original DIVU consumes the two's-complement CVT.W.S result. Do not
        // replace this with signed modulo or a mathematical Euclidean wrap.
        const std::uint32_t cx=static_cast<std::uint32_t>(ix)%w.dimension04;
        const std::uint32_t cz=static_cast<std::uint32_t>(iz)%w.dimension04;
        const std::uint64_t offset=std::uint64_t(cz)*w.rowStride08+
            std::uint64_t(cx)*w.pixelStride0C;
        if(offset>0x7fffffffu)return fail(Status::Unsupported,0x2b7468u,i);
        if(!at(w.current2C,offset))return fail(Status::Storage,0x2b7470u,i);
        const float v=m.mul(w.scale20,m.integer(std::int32_t(w.current2C.data[offset])-127,0x2b7480u),0x2b7488u);
        sum[w.channelD8]=m.add(sum[w.channelD8],v,
            w.channelD8==0?0x2b74b8u:w.channelD8==1?0x2b74e0u:
            w.channelD8==2?0x2b7504u:0x2b7510u);
        if(w.channelD8<2u){
            if(!at(w.next30,offset))return fail(Status::Storage,0x2b74bcu,i);
            const float vn=m.mul(w.scale20,m.integer(std::int32_t(w.next30.data[offset])-127,0x2b74ccu),0x2b74d0u);
            next[w.channelD8]=m.add(next[w.channelD8],vn,0x2b74d8u);
        }
        if(w.effect110)marker=w.effect110; // Last eligible nonzero source owner.
        if(m.badPC)return arithmetic(m);
    }
    const float a=m.mul(sum[1],*s.complement364,0x2b7564u);
    const float b=m.mul(next[1],*s.phase350,0x2b7570u);
    const float acc0=m.mul(sum[0],*s.complement360,0x2b7574u);
    const float pair0=m.add(acc0,m.mul(next[0],*s.phase34C,0x2b7578u),0x2b7578u);
    const float pair1=m.add(a,b,0x2b7584u);
    const float acc1=m.mul(*g.channelScale98,pair0,0x2b7588u);
    const float ch2=m.mul(*g.channelScaleA0,sum[2],0x2b758cu);
    const float first=m.add(acc1,m.mul(*g.channelScale9C,pair1,0x2b7590u),0x2b7590u);
    const float acc2=m.add(ch2,first,0x2b7594u);
    out=m.add(acc2,m.mul(*g.channelScaleA4,sum[3],0x2b759cu),0x2b759cu);
    return m.badPC?arithmetic(m):fail(Status::Complete,0x2b7598u);
}
}
// Native bounded 2B10F0 ->2B72B0. Only position[1] and, if the signature query
// is reached, optionalEffectOwner are committed. Failure preserves both.
// Numerical domain: FE_TONEAREST, binary32 normal/signed-zero inputs and
// intermediates, no fast-math/contraction. EE exceptional/overflow arithmetic,
// malformed spans, unknown wave channels and unbound owners fail explicitly.
// An authentic empty/absent owner is admitted only through its explicit binding.
inline WaterHeightResult50JC queryWaterHeight50JC(
    const WaterHeightProvider50JC& p,const WaterHeightGlobals50JC& g,
    float (&position)[3],std::uint32_t* optionalEffectOwner=nullptr) noexcept {
    using namespace water_height_detail50jc;
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::Arithmetic,0x2b10f0u);
    if(!p.sourceStateBound||!p.bucketsBound||p.count4C<0||
        (p.count4C&&!p.buckets40)||!g.wakeScale443B88)
        return fail(Status::Owner,0x2b110cu);
    Math m;
    const float x=m.read(position[0],0x2b1110u),z=m.read(position[2],0x2b1114u);
    const float wakeScale=m.read(*g.wakeScale443B88,0x2b1118u);
    float bucketSum=0.f;
    if(m.badPC)return arithmetic(m);
    for(std::int32_t i=0;i<p.count4C;++i){
        const auto& b=p.buckets40[i];
        if(!b.bound)return fail(Status::Owner,0x2b1134u,std::size_t(i));
        const auto bx=m.cvtWord(m.mul(b.inverseScale04,m.sub(x,b.originX10,0x2b1148u),0x2b1150u),0x2b1158u);
        const auto bz=m.cvtWord(m.mul(b.inverseScale04,m.sub(z,b.originZ18,0x2b114cu),0x2b1154u),0x2b115cu);
        if(m.badPC)return arithmetic(m);
        if(bx<0||bx>=p.dimensionB0||bz<0||bz>=p.dimensionB0)continue;
        if(p.dimensionB0<=0||p.blocksPerRowB4<=0||b.activeBuffer30<0||b.activeBuffer30>1)
            return fail(Status::Unsupported,0x2b1194u,std::size_t(i));
        const auto group=std::uint64_t(std::uint32_t(bz))*std::uint32_t(p.blocksPerRowB4)+std::uint32_t(bx/8);
        if(group>0x01ffffffu)return fail(Status::Unsupported,0x2b11c0u,std::size_t(i));
        const auto offset=group*64u+std::uint32_t(bx%8)*2u;
        const auto& bytes=b.buffers38[b.activeBuffer30];
        if(offset>0x7ffffffdu)return fail(Status::Unsupported,0x2b11c4u,std::size_t(i));
        if(!at(bytes,offset)||!at(bytes,offset+1))return fail(Status::Storage,0x2b11ccu,std::size_t(i));
        const std::uint32_t word=std::uint32_t(bytes.data[offset])|(std::uint32_t(bytes.data[offset+1])<<8u);
        const auto sample=word<0x8000u?std::int32_t(word):std::int32_t(word)-65536;
        bucketSum=m.add(bucketSum,m.mul(wakeScale,m.integer(sample,0x2b11d8u),0x2b11dcu),0x2b11e0u);
        if(m.badPC)return arithmetic(m);
    }
    float noise=0.f;
    if(p.noisePresence==WaterNoisePresence50JC::Unbound)
        return fail(Status::Owner,0x2b11fcu);
    if(p.noisePresence==WaterNoisePresence50JC::Present){
        if(!g.waterHeightRange443B90)return fail(Status::Owner,0x2b1294u);
        if(p.noiseDimensionAC<=0||p.noisePixelStrideCC<=0)
            return fail(Status::Unsupported,0x2b1224u);
        const auto nx=m.cvtWord(m.mul(x,p.noiseCoordinateScaleBC,0x2b1214u),0x2b1218u);
        const auto nz=m.cvtWord(m.mul(z,p.noiseCoordinateScaleBC,0x2b1228u),0x2b122cu);
        if(m.badPC)return arithmetic(m);
        const auto cx=nx%p.noiseDimensionAC,cz=nz%p.noiseDimensionAC;
        const std::int64_t pixel=std::int64_t(cz)*p.noiseDimensionAC+cx;
        if(pixel<0||pixel>0x7fffffffu)return fail(Status::Unsupported,0x2b1250u);
        const std::int64_t offset=pixel*p.noisePixelStrideCC;
        // Source signed DIV remainder can address before the texture. That
        // domain has no admitted owner here and must not be silently wrapped.
        if(offset<0||offset>0x7fffffffu)return fail(Status::Unsupported,0x2b1258u);
        if(!at(p.noise5C,std::uint64_t(offset)))return fail(Status::Storage,0x2b125cu);
        noise=m.mul(*g.waterHeightRange443B90,m.integer(p.noise5C.data[offset],0x2b1270u),0x2b129cu);
        if(m.badPC)return arithmetic(m);
    }
    if(!g.renderInfinitePlane3834E0)return fail(Status::Owner,0x2b12a8u);
    if(!*g.renderInfinitePlane3834E0&&noise==0.f){
        position[1]=0.f; // Actual early source store; optional owner untouched.
        return fail(Status::Complete,0x2b12d0u);
    }
    if(!p.signaturesC4||!p.heightScaleC0||!p.baseHeight3C||!g.poolDepth443B98)
        return fail(Status::Owner,0x2b12d4u);
    float signature=0.f;std::uint32_t marker=0;
    const auto sample=signatures(*p.signaturesC4,g,x,z,signature,marker,m);
    if(sample.status!=Status::Complete)return sample;
    const float combined=m.add(signature,m.add(bucketSum,noise,0x2b12e0u),0x2b12e8u);
    float candidate=m.mul(*p.heightScaleC0,combined,0x2b12ecu);
    if(noise==0.f)candidate=m.add(candidate,*p.baseHeight3C,0x2b1310u);
    else {
        if(!g.poolBase443E90)return fail(Status::Owner,0x2b1318u);
        candidate=m.add(candidate,*g.poolBase443E90,0x2b131cu);
    }
    const float delta=m.sub(candidate,position[1],0x2b132cu);
    const float depth=m.read(*g.poolDepth443B98,0x2b1328u);
    if(m.badPC)return arithmetic(m);
    const float result=delta<=depth?candidate:m.read(*p.baseHeight3C,0x2b1340u);
    if(m.badPC)return arithmetic(m);
    position[1]=result;
    if(optionalEffectOwner)*optionalEffectOwner=marker;
    return fail(Status::Complete,0x2b1350u);
}
} // namespace rr::game

namespace rr::game {
// Pure proposals for the scalar 2B6F00 ->2BEFA0 time prefix. The owner must
// commit them once, in original +328 order, at the actual virtual+38 update.
// They are not a substitute for 141ED0's child continuation or 2BEBD0's later
// +32C render-side origin publication. No time/default/frame is manufactured.
struct WaterTimeInput50JC {
    const std::uint32_t* application11C=nullptr;
    const float* time388=nullptr;
    const float* framesPerSecond84[5]{}; // Actual shared443B80 +84..+94.
    float delta=0;
    bool sourceVirtual38DeltaBound=false;
};
struct WaterTimeProposal50JC {
    bool paused=false;
    bool advanced=false; // Other fields are valid only when advanced.
    float time388=0;
    float phase34C[5]{},complement360[5]{};
    std::uint32_t frameNumber310F38[5]{};
};
inline WaterHeightResult50JC projectWaterTime50JC(
    const WaterTimeInput50JC& in,WaterTimeProposal50JC& out) noexcept {
    using namespace water_height_detail50jc;
    if(!in.application11C)return fail(Status::Owner,0x2b6f24u);
    WaterTimeProposal50JC next;
    if(*in.application11C){next.paused=true;out=next;return fail(Status::Complete,0x2b6f34u);}
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::Arithmetic,0x2b6f00u);
    if(!in.sourceVirtual38DeltaBound||!in.time388)
        return fail(Status::Owner,0x2b6f38u);
    Math m;
    next.time388=m.add(*in.time388,in.delta,0x2b6f40u);
    if(m.badPC)return arithmetic(m);
    if(next.time388<0.f)return fail(Status::Unsupported,0x2b6f54u);
    constexpr std::uint32_t productPC[5]={0x2b6f54u,0x2b6facu,0x2b7008u,0x2b701cu,0x2b7048u};
    for(std::size_t c=0;c<5;++c){
        if(!in.framesPerSecond84[c])return fail(Status::Owner,productPC[c],c);
        const float scaled=m.mul(next.time388,*in.framesPerSecond84[c],productPC[c]);
        if(m.badPC)return arithmetic(m);
        // Bounded exact310F38 domain: nonnegative finite binary32 below2^32.
        // Negative/overflow unsigned conversion remains an explicit boundary.
        if(scaled<0.f||scaled>=4294967296.0f)
            return fail(Status::Unsupported,productPC[c],c);
        const auto whole=static_cast<std::uint32_t>(scaled);
        next.frameNumber310F38[c]=whole;
        // Genuine stores: 2B7024/28 zero +358/+354; 2B7030/34
        // write 1.0 to+36C/+368. These are not padding fields.
        if(c==2||c==3){next.phase34C[c]=0.f;next.complement360[c]=1.f;}
        else {
            // 311038 ->302938 fmod(x,1.0) ->310440. With the admitted input,
            // exact binary64 subtraction yields that same representable frac.
            volatile float fraction=static_cast<float>(double(scaled)-double(whole));
            next.phase34C[c]=m.read(fraction,0x2b6f84u);
            next.complement360[c]=m.sub(1.f,fraction,0x2b6f98u);
        }
    }
    if(m.badPC)return arithmetic(m);
    next.advanced=true;out=next;
    return fail(Status::Complete,0x2b7098u); // Actual signature loop is next.
}
struct WaterTextureFrame50JC {
    WaterBytes50JC pixels{}; // Actual virtual+44 view, through original mip path.
    std::uint32_t rowStride=0;
};
struct WaterSignatureTimeInput50JC {
    const WaterSignature50JC* query=nullptr;
    const WaterTextureFrame50JC* frames00=nullptr;
    std::uint32_t frameCount24=0;
    float scrollB0=0,scrollB4=0,speedB8=0,speedBC=0;
    const std::uint32_t* renderFoam3834F0=nullptr;
    bool sourceFieldsBound=false;
    bool completeTextureFramesBound=false;
};
struct WaterSignatureTimeProposal50JC {
    bool skipped=false;
    bool advanced=false;
    std::uint32_t frame28=0;
    float scrollB0=0,scrollB4=0;
    WaterSignature50JC query{}; // OriginsC0/C4 intentionally retain old bytes.
};
inline WaterHeightResult50JC projectWaterSignatureTime50JC(
    const WaterSignatureTimeInput50JC& in,const WaterTimeInput50JC& tick,
    const WaterTimeProposal50JC& time,WaterSignatureTimeProposal50JC& out) noexcept {
    using namespace water_height_detail50jc;
    if(!time.advanced||time.paused||!tick.sourceVirtual38DeltaBound)
        return fail(Status::Owner,0x2b709cu);
    if(!in.sourceFieldsBound||!in.query||!in.query->bound||!in.renderFoam3834F0)
        return fail(Status::Owner,0x2befb8u);
    WaterSignatureTimeProposal50JC next;
    if(!*in.renderFoam3834F0&&in.query->channelD8==4u){
        next.skipped=true;out=next;return fail(Status::Complete,0x2befdcu);
    }
    if(std::fegetround()!=FE_TONEAREST)return fail(Status::Arithmetic,0x2befa0u);
    if(in.query->channelD8>4u)return fail(Status::Unsupported,0x2beff0u);
    if(!in.completeTextureFramesBound||!in.frames00||!in.frameCount24)
        return fail(Status::Owner,0x2befe4u);
    next.frame28=time.frameNumber310F38[in.query->channelD8]%in.frameCount24;
    const std::uint32_t following=next.frame28+1u==in.frameCount24?0u:next.frame28+1u;
    const auto& current=in.frames00[next.frame28];
    const auto& after=in.frames00[following];
    if(!current.pixels.bound||!current.pixels.data||!current.pixels.size||
        !after.pixels.bound||!after.pixels.data||!after.pixels.size)
        return fail(Status::Storage,0x2bf030u);
    next.query=*in.query;
    next.query.current2C=current.pixels;
    next.query.next30=after.pixels;
    next.query.rowStride08=current.rowStride;
    Math m;
    const float movedX=m.add(in.scrollB0,m.mul(in.speedB8,tick.delta,0x2bf12cu),0x2bf130u);
    const float movedZ=m.add(in.scrollB4,m.mul(in.speedBC,tick.delta,0x2bf168u),0x2bf16cu);
    const float ex=m.read(in.query->extentX10,0x2bf13cu);
    const float ez=m.read(in.query->extentZ14,0x2bf178u);
    if(m.badPC)return arithmetic(m);
    if(ex<=0.f||ez<=0.f)return fail(Status::Unsupported,0x2bf14cu);
    // Exact float->double promotion; original finite302938/2FF398 is fmod,
    // preserving negative remainder. Do not wrap negative scroll to positive.
    volatile float rx=static_cast<float>(std::fmod(double(movedX),double(ex)));
    volatile float rz=static_cast<float>(std::fmod(double(movedZ),double(ez)));
    next.scrollB0=m.read(rx,0x2bf15cu);next.scrollB4=m.read(rz,0x2bf19cu);
    if(m.badPC)return arithmetic(m);
    next.advanced=true;out=next;
    return fail(Status::Complete,0x2bf198u);
}
} // namespace rr::game
