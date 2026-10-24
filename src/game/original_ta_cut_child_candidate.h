#pragma once
// PRIVATE, UNACTIVATED CANDIDATE. No registry, Scene, camera manager or audio
// integration. No compilation or execution is claimed for this source packet.
// Source282860/282CD0/282C20/2829C0, bounded to the actual175510 TA call:
// mode40=0, loop54=0, one node, null load/activation offsets, and process443200
// in its source3CAAA0-initialized(+0,+0,+0,1) state. This is NOT a provider of
// the actual current process global or any inherited camera registration.
// Numerical contract: strict host binary32 FE_TONEAREST, independently rounded
// add/sub/mul, rounded sqrt then division, truncation for CVT.W.S. Host FTZ/DAZ
// must be disabled; fegetround and post-operation reads do not verify those
// modes. Build flags and host execution mode have NOT been checked. This is NOT
// physical EE/VU low-bit or exact source frame-threshold fidelity. All consumed
// operands/intermediates must be finite normal or zero; zero/subnormal norms
// are rejected as an explicit native safety boundary, not a source branch.
#include "../assets/original_ta_cut_authoring_candidate.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <utility>
#include <vector>
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "TA cut candidate requires strict independently rounded binary32 arithmetic"
#endif

namespace rr::game {
using OriginalTaCutVector3Candidate = std::array<float,3>;
using OriginalTaCutVector4Candidate = std::array<float,4>;
enum class OriginalTaCutChildStatusCandidate : std::uint8_t {
    complete, input, phase, arithmetic, frameRange, allocation, terminal
};
struct OriginalTaCutChildDiagnosticCandidate {
    OriginalTaCutChildStatusCandidate status = OriginalTaCutChildStatusCandidate::input;
    std::uint32_t sourcePC = 0;
    unsigned cutOrdinal = 0;
    std::uint32_t frame = 0;
    // Native operation result only. Never a construction/registration receipt.
    bool okay() const noexcept {return status==OriginalTaCutChildStatusCandidate::complete;}
};
struct OriginalTaCutChildFieldsCandidate {
    OriginalTaCutVector4Candidate offset10{{0.f,0.f,0.f,1.f}};
    float rate2C = 30.f;
    std::uint64_t frameCount30 = 0;
    float elapsed38 = 0.f, field3C = 80.f;
    std::int32_t mode40 = 0, lower44 = 0, upper48 = 0, active4C = 1, loop54 = 0;
};
namespace original_ta_cut_numeric_candidate_detail {
inline std::uint32_t bits(float x) noexcept {std::uint32_t v;std::memcpy(&v,&x,4);return v;}
inline float value(std::uint32_t x) noexcept {float v;std::memcpy(&v,&x,4);return v;}
struct Math {
    std::uint32_t badPC=0;
    float read(float x,std::uint32_t pc) noexcept {
        const auto u=bits(x);
        if(!badPC&&(!std::isfinite(x)||((u&0x7f800000u)==0&&(u&0x7fffffffu)!=0)))badPC=pc;
        return x;
    }
    float add(float a,float b,std::uint32_t pc) noexcept {
        read(a,pc);read(b,pc);volatile float x=a+b;return read(x,pc);
    }
    float sub(float a,float b,std::uint32_t pc) noexcept {
        read(a,pc);read(b,pc);volatile float x=a-b;return read(x,pc);
    }
    float mul(float a,float b,std::uint32_t pc) noexcept {
        read(a,pc);read(b,pc);volatile float x=a*b;return read(x,pc);
    }
    float div(float a,float b,std::uint32_t pc) noexcept {
        read(a,pc);read(b,pc);
        if(b==0.f){if(!badPC)badPC=pc;return 0.f;}
        volatile float x=a/b;return read(x,pc);
    }
};
using V=OriginalTaCutVector3Candidate;
inline V cross(const V& a,const V& b,Math& m,std::uint32_t mulaPC,std::uint32_t msubPC) noexcept {
    V first{},out{};
    // VOPMULA calculates all ACC products before any VOPMSUB destination lane.
    for(unsigned k=0;k<3;++k)first[k]=m.mul(a[(k+1)%3],b[(k+2)%3],mulaPC);
    for(unsigned k=0;k<3;++k)out[k]=m.sub(first[k],m.mul(b[(k+1)%3],a[(k+2)%3],msubPC),msubPC);
    return out;
}
inline V normalize(const V& a,Math& m,std::uint32_t mulPC,std::uint32_t addYPC,
    std::uint32_t addZPC,std::uint32_t rsqrtPC,std::uint32_t mulQPC) noexcept {
    V square{},out{};
    for(unsigned k=0;k<3;++k)square[k]=m.mul(a[k],a[k],mulPC);
    const float xy=m.add(square[0],square[1],addYPC);
    const float sum=m.add(xy,square[2],addZPC); // plain VADDZ, not fused MADD
    if(m.badPC)return out;
    if(!(sum>0.f)){m.badPC=rsqrtPC;return out;} // native safety, no guessed fallback
    volatile float root=std::sqrt(sum);
    const float q=m.div(1.f,m.read(root,rsqrtPC),rsqrtPC);
    for(unsigned k=0;k<3;++k)out[k]=m.mul(a[k],q,mulQPC);
    return out;
}
}

class OriginalTaCutChildrenCandidate;
class OriginalTaCutChildCandidate final {
public:
    using V3=OriginalTaCutVector3Candidate;
    using V4=OriginalTaCutVector4Candidate;
    using D=OriginalTaCutChildDiagnosticCandidate;
    using S=OriginalTaCutChildStatusCandidate;
    OriginalTaCutChildCandidate(const OriginalTaCutChildCandidate&)=delete;
    OriginalTaCutChildCandidate& operator=(const OriginalTaCutChildCandidate&)=delete;
    OriginalTaCutChildCandidate(OriginalTaCutChildCandidate&&)=delete;
    OriginalTaCutChildCandidate& operator=(OriginalTaCutChildCandidate&&)=delete;
    ~OriginalTaCutChildCandidate()=default;
    // Stable address of this actual collection-owned child is its identity.
    // None of these observations attest to live registration or Scene custody.
    unsigned ordinal() const noexcept{return ordinal_;}
    OriginalTaCutChildFieldsCandidate fields() const noexcept{return fields_;}
    bool failed() const noexcept{return failed_;}
    bool activated() const noexcept{return activated_;}
    std::int32_t active282CB0() const noexcept{return fields_.active4C;}
    const V3* look20() const noexcept{return arrays_?arrays_->look20.data():nullptr;}
    const V3* up24() const noexcept{return arrays_?arrays_->up24.data():nullptr;}
    const V3* position28() const noexcept{return arrays_?arrays_->position28.data():nullptr;}
    const float* scalar50() const noexcept{return arrays_?arrays_->scalar50.data():nullptr;}
private:
    friend class OriginalTaCutChildrenCandidate;
    struct Node {
        std::array<std::uint32_t,10> originalPayloadWords{};
        std::array<std::uint8_t,64> originalNameBytes{};
    };
    struct Arrays {
        std::vector<V3> look20,up24,position28;
        std::vector<float> scalar50;
        std::vector<std::array<std::uint32_t,2>> frameHeaders5C;
        std::vector<std::uint32_t> nodeIdentifiers58;
        std::vector<Node> nodes60;
        ~Arrays() noexcept {
            //283460 frees actual allocations20/24/28,50,5C,58,60 in this order.
            std::vector<V3>().swap(look20);std::vector<V3>().swap(up24);
            std::vector<V3>().swap(position28);std::vector<float>().swap(scalar50);
            std::vector<std::array<std::uint32_t,2>>().swap(frameHeaders5C);
            std::vector<std::uint32_t>().swap(nodeIdentifiers58);
            std::vector<Node>().swap(nodes60);
        }
    };
    explicit OriginalTaCutChildCandidate(unsigned ordinal) noexcept:ordinal_(ordinal){}
    unsigned ordinal_=0;
    OriginalTaCutChildFieldsCandidate fields_{};
    std::unique_ptr<const Arrays> arrays_;
    bool activated_=false,failed_=false;
    D result(S s,std::uint32_t pc,std::uint32_t frame=0) const noexcept{return {s,pc,ordinal_,frame};}
    D stop(S s,std::uint32_t pc,std::uint32_t frame=0) noexcept {
        failed_=true;return result(s,pc,frame);
    }
    D load(const assets::OriginalTaCutAuthoringCandidate& source) noexcept {
        using namespace original_ta_cut_numeric_candidate_detail;
        if(arrays_||failed_||std::fegetround()!=FE_TONEAREST)return result(S::phase,0x282CD0);
        const auto count=source.frameCount(ordinal_);
        if(count==0||count>461)return result(S::input,0x282EBC);
        try {
            auto next=std::unique_ptr<Arrays>(new Arrays);
            next->look20.resize(count);next->up24.resize(count);next->position28.resize(count);
            next->scalar50.resize(count);next->frameHeaders5C.resize(count);
            next->nodeIdentifiers58.resize(count);next->nodes60.resize(count);
            for(std::uint32_t frame=0;frame<count;++frame){
                assets::OriginalTaCutFrameBitsCandidate raw;
                if(!source.frameBits(ordinal_,frame,raw))return result(S::input,0x282FDC,frame);
                next->frameHeaders5C[frame]=raw.frameWords;
                next->nodeIdentifiers58[frame]=raw.nodeWord;
                next->nodes60[frame]={raw.payloadWords,raw.nameBytes};
                Math m;V3 look{},up{},position{};
                const std::uint32_t lookPC[3]={0x2831A8,0x2831C0,0x2831D4};
                const std::uint32_t upPC[3]={0x2831E8,0x2831F0,0x2831F8};
                const std::uint32_t positionPC[3]={0x283234,0x283238,0x283240};
                for(unsigned k=0;k<3;++k){
                    look[k]=m.read(value(raw.payloadWords[k]),lookPC[k]);
                    up[k]=m.read(value(raw.payloadWords[k+3]),upPC[k]);
                    //Source adds source-initialized zero load offset, not memcpy.
                    position[k]=m.add(value(raw.payloadWords[k+6]),0.f,positionPC[k]);
                }
                const float scalar=m.read(value(raw.payloadWords[9]),0x283264);
                const auto right0=cross(up,look,m,0x2832B8,0x2832BC);
                //Forced t0=1 takes corrected-UP branch, preserving original look.
                const auto correctedUp=cross(look,right0,m,0x2832CC,0x2832D0);
                const auto normalUp=normalize(correctedUp,m,0x2832E8,0x2832EC,0x2832F0,0x2832F4,0x283304);
                const auto normalLook=normalize(look,m,0x283308,0x28330C,0x283310,0x283314,0x283324);
                const auto right=cross(normalUp,normalLook,m,0x283328,0x28332C);
                (void)right; //Original matrix constructs it; child arrays retain other rows.
                if(m.badPC)return result(S::arithmetic,m.badPC,frame);
                next->look20[frame]=normalLook;next->up24[frame]=normalUp;
                next->position28[frame]=position;next->scalar50[frame]=scalar;
            }
            //282F08 stores frame count;282FD8 stores N, not N-1, before activation.
            fields_.frameCount30=count;fields_.upper48=static_cast<std::int32_t>(count);
            arrays_=std::unique_ptr<const Arrays>(next.release());
            return result(S::complete,0x283428);
        }catch(...){return result(S::allocation,0x282EC4);}
    }
    D activate() noexcept {
        if(failed_)return result(S::terminal,0x282C20);
        if(!arrays_)return result(S::phase,0x282C20);
        //282C20, exact reached forward/null-offset branch; no pose emitted.
        fields_.active4C=1;fields_.elapsed38=0.f;fields_.lower44=0;
        fields_.upper48=static_cast<std::int32_t>(fields_.frameCount30)-1;
        fields_.offset10={{0.f,0.f,0.f,1.f}};activated_=true;
        return result(S::complete,0x282C98);
    }
    D update(float dt,std::uint32_t& requiredWord,V4* position,V4* look,V4* up,float* scalar) noexcept {
        using namespace original_ta_cut_numeric_candidate_detail;
        if(failed_)return result(S::terminal,0x2829C0);
        if(!arrays_||!activated_)return result(S::phase,0x2829C0);
        if(fields_.active4C==0){requiredWord=0;return result(S::complete,0x282A04);}
        //Native input boundary only; inactive source ignores even an invalid dt.
        Math m;m.read(dt,0x282A90);
        if(std::fegetround()!=FE_TONEAREST||m.badPC||dt<0.f)return result(S::input,0x282A90);
        const float nextTime=m.add(fields_.elapsed38,dt,0x282A90);
        fields_.elapsed38=nextTime; //282A94 precedes product and all output decisions.
        const float product=m.mul(nextTime,fields_.rate2C,0x282AA0);
        //Source CVT.W.S toward-zero. Do not claim hardware ADD/MUL threshold parity.
        if(m.badPC)return stop(S::arithmetic,m.badPC);
        if(!(product>=0.f&&product<2147483648.f))return stop(S::frameRange,0x282AA4);
        const auto candidate=static_cast<std::int32_t>(product);
        std::int32_t index=candidate;
        if(candidate<fields_.upper48){fields_.lower44=candidate;} //282AE4
        else{index=fields_.upper48;fields_.active4C=0;} //282AE0; lower44 stays unchanged.
        if(index<0||static_cast<std::uint64_t>(index)>=fields_.frameCount30)
            return stop(S::frameRange,0x282B00,static_cast<std::uint32_t>(index));
        const auto at=static_cast<std::size_t>(index);
        V4 outLook{{arrays_->look20[at][0],arrays_->look20[at][1],arrays_->look20[at][2],1.f}};
        V4 outUp{{arrays_->up24[at][0],arrays_->up24[at][1],arrays_->up24[at][2],1.f}};
        V4 outPosition{{0.f,0.f,0.f,1.f}};
        const std::uint32_t addPC[3]={0x282B84,0x282B94,0x282BA4};
        for(unsigned k=0;k<3;++k)outPosition[k]=m.add(arrays_->position28[at][k],fields_.offset10[k],addPC[k]);
        if(m.badPC)return stop(S::arithmetic,m.badPC,static_cast<std::uint32_t>(index));
        //Preserve source optional-output order for legally typed vector/scalar aliases.
        //requiredWord storage must remain disjoint from all float/vector outputs.
        if(position)*position=outPosition; //282BB4
        if(look)*look=outLook;             //282BC4
        if(up)*up=outUp;                   //282BD4
        if(scalar)*scalar=arrays_->scalar50[at]; //282BF0
        requiredWord=0;                   //282BF4; NOT completion authority.
        return result(S::complete,0x282BF4,static_cast<std::uint32_t>(index));
    }
};

class OriginalTaCutChildrenCandidate final {
public:
    using D=OriginalTaCutChildDiagnosticCandidate;
    using S=OriginalTaCutChildStatusCandidate;
    using V4=OriginalTaCutVector4Candidate;
    OriginalTaCutChildrenCandidate(const OriginalTaCutChildrenCandidate&)=delete;
    OriginalTaCutChildrenCandidate& operator=(const OriginalTaCutChildrenCandidate&)=delete;
    OriginalTaCutChildrenCandidate(OriginalTaCutChildrenCandidate&&)=delete;
    OriginalTaCutChildrenCandidate& operator=(OriginalTaCutChildrenCandidate&&)=delete;
    ~OriginalTaCutChildrenCandidate() noexcept {
        //Caller must retire real manager/registry/Scene borrows BEFORE destruction.
        //No automatic deregistration or unrelated audio retirement is fabricated.
        for(auto& child:children_)child.reset(); //actual controller74 source order
        authoring_.reset();
    }
    static std::unique_ptr<OriginalTaCutChildrenCandidate> createInitializedTa(
        std::unique_ptr<const assets::OriginalTaCutAuthoringCandidate> authoring,D& diagnostic) noexcept {
        static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
            std::numeric_limits<float>::radix==2&&std::numeric_limits<float>::digits==24,"binary32 host required");
        diagnostic={S::input,0x174FA4,0,0};
        if(!authoring)return nullptr;
        if(std::fegetround()!=FE_TONEAREST){diagnostic={S::arithmetic,0x282CD0,0,0};return nullptr;}
        try{
            auto next=std::unique_ptr<OriginalTaCutChildrenCandidate>(new OriginalTaCutChildrenCandidate);
            next->authoring_=std::move(authoring);
            //Real distinct stable child allocations; never caller-supplied handles.
            for(unsigned i=0;i<3;++i)next->children_[i].reset(new OriginalTaCutChildCandidate(i+1));
            for(auto& child:next->children_){
                diagnostic=child->load(*next->authoring_);
                if(!diagnostic.okay())return nullptr;
            }
            diagnostic={S::complete,0x175610,0,0};return next;
        }catch(...){diagnostic={S::allocation,0x174FC4,0,0};return nullptr;}
    }
    const OriginalTaCutChildCandidate* child(unsigned ordinal) const noexcept {
        return ordinal>=1&&ordinal<=3?children_[ordinal-1].get():nullptr;
    }
    D activate(unsigned ordinal) noexcept {
        if(ordinal<1||ordinal>3)return {S::input,0x282C20,ordinal,0};
        return children_[ordinal-1]->activate();
    }
    //Only these real owned children can advance; no sample/state replacement API.
    //Output storage must not alias this owner, its authoring or derived arrays.
    //requiredWord must be disjoint from every float/V4 output. Only legally
    //typed vector/scalar aliases are supported; no raw uint32/float aliasing.
    D update(unsigned ordinal,float dt,std::uint32_t& requiredWord,V4* position,
        V4* look,V4* up,float* scalar) noexcept {
        if(ordinal<1||ordinal>3)return {S::input,0x2829C0,ordinal,0};
        return children_[ordinal-1]->update(dt,requiredWord,position,look,up,scalar);
    }
private:
    OriginalTaCutChildrenCandidate()=default;
    std::unique_ptr<const assets::OriginalTaCutAuthoringCandidate> authoring_;
    std::array<std::unique_ptr<OriginalTaCutChildCandidate>,3> children_{};
};
}
