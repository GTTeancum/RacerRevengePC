#include "static_impact49y.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cinttypes>
#include <cerrno>
#if defined(__SSE__)
#include <xmmintrin.h>
#endif

namespace rr::game {
namespace {
// Diagnostic-only, transient receipt. No game/Bank state or public API changes.
// Arithmetic and its first rejecting predicate still execute exactly once.
struct ImpactFailureTrace {
    const char* stage="entry";std::uint32_t anchor=0x223030,lane=0;
    const char* operation="none";std::uint32_t ordinal=0,arity=0,readIndex=0;
    std::uint32_t operands[2]{};bool active=false,captured=false;
    const char* firstStage="none";const char* firstOperation="none";
    std::uint32_t firstAnchor=0,firstLane=0,firstOrdinal=0,firstArity=0;
    std::uint32_t firstRole=0,firstOperands[2]{},rejectedBits=0,resultBits=0;
    bool resultAvailable=false;int firstRounding=0,firstExceptions=0;
    std::uint32_t firstMxcsr=0;bool firstMxcsrAvailable=false;
    static std::uint32_t bits(const float& x) noexcept {std::uint32_t b;std::memcpy(&b,&x,4);return b;}
    static std::uint32_t mxcsr(bool& available) noexcept {
#if defined(__SSE__)
        available=true;return _mm_getcsr();
#else
        available=false;return 0;
#endif
    }
    void begin(const char* op,const float& x,const float* y=nullptr) noexcept {
        active=true;operation=op;++ordinal;arity=y?2u:1u;readIndex=0;
        operands[0]=bits(x);operands[1]=y?bits(*y):0;
    }
    void reject(std::uint32_t b) noexcept {
        if(captured)return;
        captured=true;firstStage=stage;firstAnchor=anchor;firstLane=lane;
        firstOperation=operation;firstOrdinal=ordinal;firstArity=arity;
        firstRole=readIndex;firstOperands[0]=operands[0];firstOperands[1]=operands[1];
        rejectedBits=b;firstRounding=std::fegetround();firstExceptions=std::fetestexcept(FE_ALL_EXCEPT);
        firstMxcsr=mxcsr(firstMxcsrAvailable);
    }
    void finish(const float& result) noexcept {
        if(captured&&firstOrdinal==ordinal){resultBits=bits(result);resultAvailable=true;}
        active=false;
    }
};
void impactWords(const char* label,const float* data,unsigned count) noexcept {
    std::fprintf(stderr,"static-impact49Y-diag %s",label);
    for(unsigned i=0;i<count;++i)std::fprintf(stderr," %08" PRIx32,ImpactFailureTrace::bits(data[i]));
    std::fputc('\n',stderr);
}
void emitImpactFailure(const StaticImpactInput49Y& in,const StaticImpactState49Y& state,
    const ImpactFailureTrace& t,int entryRounding,const char* reason) noexcept {
    // All snapshot words are initialized by the real vehicle_frame49w caller.
    // Float words (including copy-only W) are copied as bits, never formatted
    // or classified numerically. No raw object padding is read or emitted.
    const int savedErrno=errno;
    const int finalRounding=std::fegetround(),finalExceptions=std::fetestexcept(FE_ALL_EXCEPT);
    bool mxcsrAvailable=false;const auto finalMxcsr=ImpactFailureTrace::mxcsr(mxcsrAvailable);
    std::fprintf(stderr,"static-impact49Y-diag BEGIN schema=1 source=223030 reason=%s caller_slot=unavailable\n",reason);
    std::fprintf(stderr,"static-impact49Y-diag fp entry_round=%d first_available=%u first_round=%d first_except=%x first_mxcsr_available=%u first_mxcsr=%08" PRIx32 " final_round=%d final_except=%x final_mxcsr_available=%u final_mxcsr=%08" PRIx32 "\n",
        entryRounding,unsigned(t.captured),t.firstRounding,unsigned(t.firstExceptions),unsigned(t.firstMxcsrAvailable),t.firstMxcsr,
        finalRounding,unsigned(finalExceptions),unsigned(mxcsrAvailable),finalMxcsr);
    std::fprintf(stderr,"static-impact49Y-diag first available=%u stage=%s anchor=%08" PRIx32 " lane=%" PRIu32 " op=%s ordinal=%" PRIu32 " arity=%" PRIu32 " read_index=%" PRIu32 " rejected=%08" PRIx32 " operands=%08" PRIx32 ",%08" PRIx32 " result_available=%u result=%08" PRIx32 "\n",
        unsigned(t.captured),t.firstStage,t.firstAnchor,t.firstLane,t.firstOperation,t.firstOrdinal,t.firstArity,t.firstRole,
        t.rejectedBits,t.firstOperands[0],t.firstOperands[1],unsigned(t.resultAvailable),t.resultBits);
    std::fprintf(stderr,"static-impact49Y-diag integers vehicle40=%08" PRIx32 " vehicle58=%08" PRIx32 " bodyContactPartnerCount20C=%08" PRIx32 " flagsCD8=%02x sceneFlags190=%02x vehicleCFC=%08" PRIx32 " partnerState=%08" PRIx32 " aiOwnersBound=%u aiEnvironmentMultiplierBound=%u\n",
        in.vehicle40,in.vehicle58,in.bodyContactPartnerCount20C,unsigned(in.flagsCD8),unsigned(in.sceneFlags190),in.vehicleCFC,in.partnerState,
        unsigned(in.aiOwnersBound),unsigned(in.tuning.aiEnvironmentMultiplierBound));
    impactWords("readyMainScene190",in.readyMainScene190.v,16);
    impactWords("accumulatedImpact1C0",in.accumulatedImpact1C0,4);
    impactWords("reductionCF0",&in.reductionCF0,1);
    impactWords("thresholds00",in.tuning.thresholds00,5);
    impactWords("severities14",in.tuning.severities14,5);
    impactWords("aiEnvironmentMultiplier60",&in.tuning.aiEnvironmentMultiplier60,1);
    impactWords("priorVf9W",&in.priorVf9W,1);
    impactWords("damage7D0",&in.damage7D0,1);impactWords("damage7D4",&in.damage7D4,1);impactWords("damage7DC",&in.damage7DC,1);
    impactWords("prior.directionalD08",state.directionalD08,8);impactWords("prior.impactBA0",state.impactBA0,4);
    std::fputs("static-impact49Y-diag END\n",stderr);std::fflush(stderr);errno=savedErrno;
}
struct Math {
    bool valid=true;ImpactFailureTrace* trace=nullptr;
    void stage(const char* name,std::uint32_t anchor,std::uint32_t lane=0) noexcept {
        if(trace){trace->stage=name;trace->anchor=anchor;trace->lane=lane;}
    }
    void begin(const char* op,const float& x,const float* y=nullptr) noexcept {if(trace)trace->begin(op,x,y);}
    void finish(const float& result) noexcept {if(trace)trace->finish(result);}
    float read(float x) noexcept {
        const bool standalone=trace&&!trace->active;if(standalone)begin("read",x);
        std::uint32_t b;std::memcpy(&b,&x,4);const bool wasValid=valid;
        // The admitted ordinary domain excludes denormal consumed operands and
        // intermediates. PCSX2 SQRT_S and VU vuDouble flush those differently
        // from host IEEE; a finite-only check would not establish equivalence.
        valid=valid&&std::isfinite(x)&&((b&0x7f800000u)!=0||(b&0x7fffffffu)==0);
        if(trace){if(wasValid&&!valid)trace->reject(b);++trace->readIndex;}
        if(standalone)finish(x);
        return x;
    }
    float add(float x,float y) noexcept {begin("add",x,&y);read(x);read(y);volatile float z=x+y;const float r=read(z);finish(r);return r;}
    float sub(float x,float y) noexcept {begin("sub",x,&y);read(x);read(y);volatile float z=x-y;const float r=read(z);finish(r);return r;}
    float mul(float x,float y) noexcept {begin("mul",x,&y);read(x);read(y);volatile float z=x*y;const float r=read(z);finish(r);return r;}
    // Narrow native value policy for the six static223030 square results only.
    // Scalar223084/88/MADD product223090 and VU VMUL.xyz2230D8 have
    // signed-zero underflow publication in pinned PCSX2v2.4.0. Inputs retain
    // the strict normal-or-zero guard; this is not global FTZ or flag parity.
    float squareResult50MB(float x) noexcept {
        begin("square-result-underflow",x,&x);read(x);read(x);
        volatile float z=x*x;float result=z;
        std::uint32_t word;std::memcpy(&word,&result,4);
        if((word&0x7f800000u)==0&&(word&0x007fffffu)!=0){word&=0x80000000u;std::memcpy(&result,&word,4);}
        const float r=read(result);finish(r);return r;
    }
    float sqrt(float x) noexcept {begin("sqrt",x);read(x);volatile float z=static_cast<float>(std::sqrt(static_cast<double>(x)));const float r=read(z);finish(r);return r;}
    float reciprocal(float x) noexcept {begin("reciprocal",x);read(x);volatile float z=1.0f/x;const float r=read(z);finish(r);return r;}
    float maximum(float x,float y) noexcept {begin("maximum",x,&y);read(x);read(y);const float r=detail::scalarMax48O(x,y);finish(r);return r;}
};
float maxFinite() noexcept {const std::uint32_t bits=0x7f7fffffu;float x;std::memcpy(&x,&bits,4);return x;}
void inverseDirection(const BodyMatrix48G& m,const float* in,float* out,Math& f) noexcept {
    BodyMatrix48G t=m;
    //28BC60..28BD30: preserve original transpose ADDs and unused translation
    // arithmetic; no general inverse or orthonormality repair is substituted.
    for(unsigned i=0;i<12;++i){f.stage("inverse-transpose",0x28bc9c,i);t.v[i]=f.add(m.v[i],0.0f);}
    f.stage("inverse-transpose-offdiagonal",0x28bca8);
    t.v[4]=f.add(0.0f,m.v[1]);t.v[8]=f.add(0.0f,m.v[2]);
    t.v[9]=f.add(0.0f,m.v[6]);t.v[1]=f.add(0.0f,m.v[4]);
    t.v[2]=f.add(0.0f,m.v[8]);t.v[6]=f.add(0.0f,m.v[9]);
    for(unsigned i=12;i<16;++i){f.stage("inverse-translation-copy",0x28bcc8,i);t.v[i]=f.add(m.v[i],0.0f);}
    for(unsigned i=0;i<3;++i){f.stage("inverse-translation",0x28bccc,i);float a=f.mul(t.v[i],m.v[12]);a=f.add(a,f.mul(t.v[4+i],m.v[13]));a=f.add(a,f.mul(t.v[8+i],m.v[14]));t.v[12+i]=f.mul(a,-1.0f);}
    for(unsigned i=0;i<3;++i){f.stage("inverse-direction",0x28bd14,i);float a=f.mul(t.v[i],in[0]);a=f.add(a,f.mul(t.v[4+i],in[1]));a=f.add(a,f.mul(t.v[8+i],in[2]));out[i]=f.add(a,0.0f);}
}
}
StaticImpactStatus49Y recordStaticImpact49Y(const StaticImpactInput49Y& in,
    const StaticImpactState49Y& state,StaticImpactState49Y& out,float& nextW) noexcept {
    using S=StaticImpactStatus49Y;
    if((in.flagsCD8&1u)||in.vehicle40==3||in.vehicle40==4){out=state;nextW=in.priorVf9W;return S::skipped;}
    if(in.vehicle58>1u||in.bodyContactPartnerCount20C!=0||!(in.sceneFlags190&1u))return S::unsupported;
    const bool ai=in.vehicle58==1u;
    if(ai&&(!in.aiOwnersBound||(!in.vehicleCFC&&!in.tuning.aiEnvironmentMultiplierBound)))return S::unsupported;
    const int entryRounding=std::fegetround();
    ImpactFailureTrace trace;
    if(entryRounding!=FE_TONEAREST){emitImpactFailure(in,state,trace,entryRounding,"rounding");return S::arithmetic;}
    Math f;f.trace=&trace;
    //223078..22309C scalar world magnitude; no vector/hypot replacement.
    f.stage("world-magnitude",0x223078);
    const float yy=f.squareResult50MB(in.accumulatedImpact1C0[1]);
    const float xx=f.squareResult50MB(in.accumulatedImpact1C0[0]);
    const float magnitude=f.sqrt(f.add(f.add(xx,yy),f.squareResult50MB(in.accumulatedImpact1C0[2])));
    float local[3];inverseDirection(in.readyMainScene190,in.accumulatedImpact1C0,local,f);
    f.stage("scratch-w",0x28bd20);
    const float w=f.add(in.priorVf9W,1.0f);
    //2230D4..F8: dotXYZ, original VRSQRT zero result, VMULq.xyz.
    f.stage("local-squared-norm",0x2230d4);
    const float x2=f.squareResult50MB(local[0]),y2=f.squareResult50MB(local[1]),z2=f.squareResult50MB(local[2]);
    const float dot=f.add(f.add(x2,y2),f.mul(1.0f,z2));
    f.stage("reciprocal-norm",0x2230e8);
    const float q=dot==0.0f?maxFinite():f.reciprocal(f.sqrt(dot));
    float normal[3];for(unsigned i=0;i<3;++i){f.stage("normalized-direction",0x2230f4,i);normal[i]=f.mul(local[i],q);}
    //222798..22281C first reached threshold; unsorted authored values allowed.
    float chosen=0.0f;unsigned thresholdIndex=5;
    for(unsigned i=0;i<5;++i){f.stage("threshold-selection",0x222798,i);if(!(magnitude<f.read(in.tuning.thresholds00[i]))){chosen=f.read(in.tuning.severities14[i]);thresholdIndex=i;break;}}
    //2227AC/2227C8/2227E4/222800 branch straight to22286C. The weaker
    // XS match alone falls through222820; even no match bypasses this gate.
    if(ai&&thresholdIndex==4u&&in.partnerState){
        f.stage("ai-damage-gate",0x222820);
        const float first=f.read(in.damage7D0),second=f.read(in.damage7D4);
        const float pair=f.maximum(second,first);
        const float third=f.read(in.damage7DC);
        //223508 retains the source MAX.S third,third before the final MAX.
        if(f.maximum(pair,f.maximum(third,third))>0.9f)chosen=0.0f;
    }
    //22286C..22289C. This multiply also executes for zero chosen severity.
    f.stage("ai-environment-multiplier",0x22289c);
    if(ai&&!in.vehicleCFC)chosen=f.mul(chosen,in.tuning.aiEnvironmentMultiplier60);
    f.stage("reduction",0x2228b0);
    const float severity=f.mul(chosen,f.sub(1.0f,in.reductionCF0));
    StaticImpactState49Y s=state;
    auto raise=[&](unsigned index,float value){if(f.trace)f.trace->lane=index;if(f.read(s.directionalD08[index])<value)s.directionalD08[index]=value;};
    //22313C..2231A0 unscaled lateral maxima always run, even severity<=0.1.
    f.stage("lateral-maxima",0x22313c);
    if(local[0]<-0.1f)raise(7,-local[0]);
    else if(local[0]>0.1f)raise(6,local[0]);
    if(severity>0.1f){
        f.stage("directional-products",0x2231dc);
        const float pz=f.mul(severity,normal[2]),py=f.mul(severity,normal[1]),px=f.mul(severity,normal[0]);
        f.stage("directional-maxima",0x223204);
        if(local[0]<-0.1f)raise(0,-px);else if(local[0]>0.1f)raise(1,px);
        if(local[1]<-0.1f)raise(3,-py);else if(local[1]>0.1f)raise(2,py);
        if(local[2]<-0.1f&&normal[2]<-0.75f){
            //22334C compares the old maximum before the AI-specific gate.
            const float rear=-pz;
            f.stage("rear-maximum",0x22334c,4);
            if(f.read(s.directionalD08[4])<rear&&
                (!ai||!in.vehicleCFC||normal[2]<-0.85f))s.directionalD08[4]=rear;
        }
        else if(local[2]>0.1f)raise(5,pz);
    }
    //2233F0..2234C4 independent absolute-coordinate selection; ties keep bits.
    for(unsigned i=0;i<3;++i){f.stage("absolute-maxima",0x2233f0,i);if(std::fabs(f.read(in.accumulatedImpact1C0[i]))>std::fabs(f.read(s.impactBA0[i])))s.impactBA0[i]=in.accumulatedImpact1C0[i];}
    if(!f.valid){emitImpactFailure(in,state,trace,entryRounding,"math");return S::arithmetic;}
    out=s;nextW=w;return S::applied;
}

StaticImpactStatus49Y prepareImpactGeometry50IW(const StaticImpactInput49Y& in,
    ImpactGeometry50IW& out) noexcept {
    using S=StaticImpactStatus49Y;
    if((in.flagsCD8&1u)||in.vehicle40==3||in.vehicle40==4)return S::skipped;
    if(in.vehicle58>1u||!(in.sceneFlags190&1u))return S::unsupported;
    if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    Math f;ImpactGeometry50IW g{};
    const float yy=f.mul(in.accumulatedImpact1C0[1],in.accumulatedImpact1C0[1]);
    const float xx=f.mul(in.accumulatedImpact1C0[0],in.accumulatedImpact1C0[0]);
    g.magnitude=f.sqrt(f.add(f.add(xx,yy),f.mul(in.accumulatedImpact1C0[2],in.accumulatedImpact1C0[2])));
    inverseDirection(in.readyMainScene190,in.accumulatedImpact1C0,g.local,f);
    g.nextScratchW=f.add(in.priorVf9W,1.0f);
    const float x2=f.mul(g.local[0],g.local[0]),y2=f.mul(g.local[1],g.local[1]),z2=f.mul(g.local[2],g.local[2]);
    const float dot=f.add(f.add(x2,y2),f.mul(1.0f,z2));
    const float q=dot==0.0f?maxFinite():f.reciprocal(f.sqrt(dot));
    for(unsigned i=0;i<3;++i)g.normal[i]=f.mul(g.local[i],q);
    if(!f.valid)return S::arithmetic;out=g;return S::applied;
}
StaticImpactStatus49Y applyImpactSeverity50IW(const StaticImpactInput49Y& in,
    const ImpactGeometry50IW& g,float sourceSeverity,
    const StaticImpactState49Y& state,StaticImpactState49Y& out) noexcept {
    using S=StaticImpactStatus49Y;
    if((in.flagsCD8&1u)||in.vehicle40==3||in.vehicle40==4){out=state;return S::skipped;}
    if(in.vehicle58>1u||!(in.sceneFlags190&1u))return S::unsupported;
    if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    Math f;const auto severity=f.read(sourceSeverity);const auto* local=g.local;const auto* normal=g.normal;
    StaticImpactState49Y s=state;
    auto raise=[&](unsigned index,float value){if(f.read(s.directionalD08[index])<value)s.directionalD08[index]=value;};
    if(f.read(local[0])<-0.1f)raise(7,-local[0]);
    else if(local[0]>0.1f)raise(6,local[0]);
    if(severity>0.1f){
        const float pz=f.mul(severity,normal[2]),py=f.mul(severity,normal[1]),px=f.mul(severity,normal[0]);
        if(local[0]<-0.1f)raise(0,-px);else if(local[0]>0.1f)raise(1,px);
        if(f.read(local[1])<-0.1f)raise(3,-py);else if(local[1]>0.1f)raise(2,py);
        if(f.read(local[2])<-0.1f&&normal[2]<-0.75f){
            const float rear=-pz;
            if(f.read(s.directionalD08[4])<rear&&
                (in.vehicle58!=1u||!in.vehicleCFC||normal[2]<-0.85f))s.directionalD08[4]=rear;
        }else if(local[2]>0.1f)raise(5,pz);
    }
    for(unsigned i=0;i<3;++i)if(std::fabs(f.read(in.accumulatedImpact1C0[i]))>std::fabs(f.read(s.impactBA0[i])))s.impactBA0[i]=in.accumulatedImpact1C0[i];
    if(!f.valid)return S::arithmetic;out=s;return S::applied;
}

}
