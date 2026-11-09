#include "original_simple_vcr50la.h"
#include "original_application_entities50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <bit>
#include <exception>
#include <limits>
#include <new>
#include <thread>
#include <utility>

namespace rr::game {
struct OriginalSimpleVCR50LA::Impl final {
    const std::thread::id thread=std::this_thread::get_id();
    std::shared_ptr<OriginalApplicationEntities50LA> application;
    OriginalWreckComponent50LA component;
    bool counted=false,closed=false,setupAttempted=false,setupComplete=false;
    //287430 writes these words. B0 bits0/6 remain UNKNOWN until a reached
    //writer; no zero-fill of the absent lanes is exposed as source knowledge.
    // +30 record time, +34 play time, +38 last applied play time, +3C/+54
    // the two bracketing frame times, +40 keyframe gate, +44 sample timer.
    float time30=0,play34=0,last38=0,prev3C=0,keyframe40=0,sample44=0;
    float scalar48=1.f,scalar4C=0x1.111112p-4f,scalar50=1.f;
    float next54=0;std::uint32_t header58=0x80000000u;
    std::unique_ptr<std::uint8_t[]> buffer5C,bufferB8;
    std::uint8_t* cursor60=nullptr;
    std::uint8_t* cursor64=nullptr;
    std::uint32_t word68=0,word6C=0,word70=0,word74=0;
    std::uint32_t capacity78=0,attempted7C=0,count80=0;
    // Class-0 channels address a binary32 word; class-1 channels one byte.
    std::unique_ptr<void*[]> pointers84,pointers88;
    std::uint32_t keyframes=0,deltas=0,escapes=0,framesRead=0; //Native diagnostics only.
    std::unique_ptr<std::uint32_t[]> nativeWords;std::unique_ptr<std::uint8_t[]> nativeBytes;
    std::unique_ptr<std::uint32_t[]> words8C,enabled90,kind94,wordA0,wordA8,wordA4;
    std::unique_ptr<float[]> scale98,inverse9C;
    std::unique_ptr<std::uint8_t[]> bytesAC;
    std::uint8_t flagsB0Known=0xbeu,flagsB0Value=0;
    std::uint32_t bytesB4=0;
    // Native borrow provenance only, not another source count or channel.
    const OriginalRaceConstructor50LA* registeredRace90=nullptr;
    explicit Impl(std::shared_ptr<OriginalApplicationEntities50LA> a,std::uint64_t generation)
        :application(std::move(a)) {
        if(!application||application->nativeOwnerThread_!=thread||!application->application10()||
           !generation||application->attachmentLock423F58()||
           application->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return;
        auto& authority=application->entityAuthority_;
        const auto key=std::uint64_t(reinterpret_cast<std::uintptr_t>(&component));
        if(!authority.reserveAdditional(1)||!authority.canEnroll(component,key))return;
        //28743C setsA1=1 for141300 before any SimpleVCR-specific stores.
        const auto made=initializeOriginalWreckComponent50LA(component,
            OriginalWreckComponentKind50LA::entity141300,generation,key,true,
            application->objectCount423F60());
        if(made.status!=OriginalWreckComponentStatus50LA::complete)return;
        counted=true;if(!authority.enroll(component))std::terminate();
        //3D2F70 is represented by this concrete native type, not a guest
        //callable pointer. Initial scalars/pointers above are exactly287454..580.
    }
    bool current()const noexcept {
        return thread==std::this_thread::get_id()&&counted&&!closed&&application&&
            application->application10()&&!application->attachmentLock423F58()&&
            component.kind==OriginalWreckComponentKind50LA::entity141300&&
            component.reference00==1&&component.sessionGeneration&&
            component.objectKey==std::uint64_t(reinterpret_cast<std::uintptr_t>(&component))&&
            !component.parent1C&&!component.previous10&&!component.next14&&!component.firstChild18&&
            !component.mask20&&!component.descendants24&&component.category28==0xffu&&
            (component.flags29&15u)==3u&&application->entityAuthority_.canRetire(component);
    }
    static bool stop(OriginalSimpleVCRDiagnostic50LA& d,std::uint32_t pc,const char* why)noexcept {
        d.sourcePC=pc;d.operation=why;return false;
    }
    void reset()noexcept {
        //287BE0 never clears count80/attempted7C or registrations. It resets
        //playback/buffer cursors and bit5, preserving all other known lanes.
        flagsB0Value=std::uint8_t(flagsB0Value&~0x20u);
        word74=0;time30=0;keyframe40=0;sample44=0;play34=0;next54=0;header58=0x80000000u;
        word68=capacity78*4u+8u;word6C=capacity78*5u+8u;word70=0;
        cursor60=buffer5C.get();cursor64=(flagsB0Value&1u)?bufferB8.get():buffer5C.get();
    }
    bool bit(std::uint8_t mask)const noexcept {return (flagsB0Value&mask)!=0;}
    void setBit(std::uint8_t mask,bool value)noexcept {
        flagsB0Value=std::uint8_t(value?(flagsB0Value|mask):(flagsB0Value&~mask));
        flagsB0Known=std::uint8_t(flagsB0Known|mask);
    }
    // Write bounds: [buffer5C, buffer5C+bytesB4). Every store is checked.
    bool writable(std::size_t bytes)const noexcept {
        const auto* base=buffer5C.get();
        return base&&cursor60>=base&&std::size_t(cursor60-base)+bytes<=bytesB4;
    }
    const std::uint8_t* readBase()const noexcept {return (flagsB0Value&1u)?bufferB8.get():buffer5C.get();}
    bool readable(std::size_t bytes)const noexcept {
        const auto* base=readBase();
        return base&&cursor64>=base&&std::size_t(cursor64-base)+bytes<=bytesB4;
    }
    void put(const void* bytes,std::size_t count)noexcept {
        std::memcpy(cursor60,bytes,count);cursor60+=count;word74+=std::uint32_t(count);word70+=std::uint32_t(count);
    }
    static float f(std::uint32_t w)noexcept {return std::bit_cast<float>(w);}
    static std::uint32_t w(float v)noexcept {return std::bit_cast<std::uint32_t>(v);}
    std::uint32_t source(std::uint32_t i)const noexcept {
        std::uint32_t v;std::memcpy(&v,pointers84[i],4);return v;
    }
    // 3062C0(cursor,{-1000.0,0x80000000|70},8): trailer, cursor NOT advanced.
    void trailer()noexcept {
        const std::uint32_t t[2]={0xc479c000u,0x80000000u|word70};
        std::memcpy(cursor60,t,8);
    }
    void full()noexcept {if(!bit(0x80u))setBit(0x80u,true);} //287FF4 prints once.
    // 287DD0.
    void keyframe()noexcept {
        if(!(word74+word68+8u<bytesB4)){full();return;}
        if(!writable(std::size_t(word68)+8u))return; //Native fence; source size bound above.
        const std::uint32_t header[2]={w(time30),0x80000000u|word70};
        word70=0;put(header,8);
        for(std::uint32_t i=0;i<count80;++i){
            if(words8C[i]==0u){
                const auto v=source(i);put(&v,4);wordA0[i]=v;
            }else if(words8C[i]==1u)put(pointers84[i],1);
        }
        trailer();++keyframes;
    }
    // 288030.
    void delta()noexcept {
        if(!(word74+word6C+8u<bytesB4)){full();return;}
        if(!writable(std::size_t(word6C)+8u))return;
        const std::uint32_t header[2]={w(time30),word70};
        word70=0;put(header,8);
        for(std::uint32_t i=0;i<count80;++i){
            if(words8C[i]==0u){
                const float v=f(source(i));
                const float d=v-f(wordA0[i]);
                const float scaled=d*scale98[i];
                const float sign=d<0.f?-1.f:1.f;
                // CVT.W.S truncates toward zero.
                const float rounded=scaled+0.5f*sign;
                std::int32_t q=0;
                const bool finite=rounded==rounded&&rounded<2147483520.f&&rounded>-2147483520.f;
                if(finite)q=static_cast<std::int32_t>(rounded);
                if(!finite||q>=0x7f||q<-0x7f){
                    const std::uint8_t escape=0x7fu;put(&escape,1);
                    const auto raw=w(v);put(&raw,4);wordA0[i]=raw;++escapes;
                }else{
                    const auto byte=static_cast<std::uint8_t>(static_cast<std::int8_t>(q));put(&byte,1);
                    wordA0[i]=w((v+float(q)*inverse9C[i])-d); //2882B8/BC order.
                }
            }else if(words8C[i]==1u)put(pointers84[i],1);
        }
        trailer();++deltas;
    }
    // 2883B0. Returns true at the trailer (end of the recording).
    bool readFrame()noexcept {
        if(!readable(8))return true; //Native fence: never read past the buffer.
        std::uint32_t peek[2];std::memcpy(peek,cursor64,8);
        if(f(peek[0])==-1000.f)return true;
        prev3C=next54;next54=f(peek[0]);header58=peek[1];cursor64+=8;++framesRead;
        const bool key=(header58&0x80000000u)!=0;
        for(std::uint32_t i=0;i<count80;++i){
            if(words8C[i]==0u){
                wordA4[i]=wordA8[i];
                if(key){
                    if(!readable(4))return true;
                    std::memcpy(&wordA8[i],cursor64,4);cursor64+=4;
                    if(!(header58&0x7fffffffu))wordA4[i]=wordA8[i];
                }else{
                    if(!readable(1))return true;
                    const auto q=static_cast<std::int8_t>(*cursor64);cursor64+=1;
                    if(q==0x7f){
                        if(!readable(4))return true;
                        std::memcpy(&wordA8[i],cursor64,4);cursor64+=4;
                    }else wordA8[i]=w(f(wordA8[i])+float(q)*inverse9C[i]);
                }
            }else if(words8C[i]==1u){
                if(!readable(1))return true;
                bytesAC[i]=*cursor64;cursor64+=1;
            }
        }
        return false;
    }
    // 14FD00: slerp without renormalization; dot >= 1 copies q0.
    static void slerp(const float* q0,const float* q1,float t,float* out)noexcept {
        float dot=q0[1]*q1[1]+q0[2]*q1[2]+q0[3]*q1[3]+q0[0]*q1[0];
        float sign=1.f;
        if(dot<0.f){sign=-1.f;dot=-dot;}
        if(!(dot<1.f)){for(int k=0;k<4;++k)out[k]=q0[k];return;}
        const float theta=std::acos(dot);
        const float inv=1.f/std::sin(theta);
        const float b=sign*(inv*std::sin(t*theta));
        const float a=inv*std::sin(theta-t*theta);
        for(int k=0;k<4;++k)out[k]=q0[k]*a+q1[k]*b;
    }
    void store(std::uint32_t i,std::uint32_t value)noexcept {std::memcpy(pointers88[i],&value,4);}
    // 288670.
    void play(float dt)noexcept {
        if(bit(0x08u)){
            const float before=play34;reset();
            play34=std::max(before-dt*scalar50,0.f);
        }else play34+=dt*scalar50;
        if(bit(0x20u))return;
        if(last38==play34)return;
        last38=play34;
        while(!bit(0x20u)&&!(play34<next54))setBit(0x20u,readFrame());
        if(next54==prev3C)return;
        const float t=(play34-prev3C)/(next54-prev3C);
        for(std::uint32_t i=0;i<count80;++i){
            if(!pointers88[i]||!enabled90[i])continue;
            const auto mode=kind94[i];
            if(mode==3u){
                if(i+3u>=count80)return; //Native fence for a malformed quad.
                float q0[4],q1[4],out[4];
                for(int k=0;k<4;++k){q0[k]=f(wordA4[i+k]);q1[k]=f(wordA8[i+k]);}
                slerp(q0,q1,t,out);
                for(std::uint32_t k=0;k<4;++k)if(pointers88[i+k])store(i+k,w(out[k]));
                i+=3u;continue;
            }
            const float a=f(wordA4[i]),b=f(wordA8[i]);
            const bool lerp=mode==0u||(mode==1u&&!(b<=a))||(mode==2u&&b<a);
            if(lerp&&words8C[i]==0u){store(i,w(a+t*(b-a)));continue;}
            if(words8C[i]==1u)std::memcpy(pointers88[i],&bytesAC[i],1);
            else store(i,wordA8[i]);
        }
    }
    std::int32_t add(void* source,void* destination,std::uint32_t cls,std::uint32_t interpolation,
        float scale,std::uint32_t enabled,bool checkScale)noexcept {
        ++attempted7C;
        if((checkScale&&scale<=0.f)||count80>=capacity78||bit(0x10u)||!source)return -1;
        const auto i=count80;
        pointers84[i]=source;pointers88[i]=destination?destination:source;
        words8C[i]=cls;enabled90[i]=enabled;kind94[i]=interpolation;
        scale98[i]=scale;inverse9C[i]=1.f/scale;
        ++count80;return std::int32_t(i);
    }
};

OriginalSimpleVCR50LA::OriginalSimpleVCR50LA(
    std::shared_ptr<OriginalApplicationEntities50LA> a,std::uint64_t generation)
    :impl_(new Impl(std::move(a),generation)){}
OriginalSimpleVCR50LA::~OriginalSimpleVCR50LA()noexcept {
    OriginalSimpleVCRDiagnostic50LA d;
    if(impl_&&!closeDetached2875A0(d))std::terminate();
}
bool OriginalSimpleVCR50LA::constructed()const noexcept {
    return impl_&&impl_->current();
}
bool OriginalSimpleVCR50LA::setup287710(std::uint32_t capacity,std::uint32_t bytes,bool backup,
    OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};if(!impl_)return Impl::stop(d,0x287710,"SimpleVCR constructor absent");auto& p=*impl_;
    if(!p.current()||p.setupAttempted||
       (backup?(capacity!=0x28u||bytes!=0x20000u):(capacity!=0xa0u||bytes!=0x40000u)))
        return Impl::stop(d,0x287710,"Actual fresh Race SimpleVCR setup shape unavailable");
    p.setupAttempted=true; //Native no-replay guard, not source readiness.
    p.flagsB0Value=std::uint8_t((p.flagsB0Value&~1u)|(backup?1u:0u));
    p.flagsB0Known=std::uint8_t(p.flagsB0Known|1u); //287734.
    p.bytesB4=bytes;p.capacity78=capacity; //287738/73C precede allocation.
    try {
        //Uninitialized allocation is intentional where original malloc writes
        //nothing. Preserve source allocation/store/zero order, including8C/AC.
        d.sourcePC=0x287740;p.buffer5C.reset(new std::uint8_t[bytes]);
        d.sourcePC=0x287750;p.pointers84.reset(new void*[capacity]);
        d.sourcePC=0x287760;p.pointers88.reset(new void*[capacity]);
        d.sourcePC=0x287770;p.words8C.reset(new std::uint32_t[capacity]);
        std::memset(p.buffer5C.get(),0,bytes); //287784.
        if(backup){
            d.sourcePC=0x28779c;p.bufferB8.reset(new std::uint8_t[bytes]);
            std::memset(p.bufferB8.get(),0,bytes); //2877B0.
        }
        d.sourcePC=0x2877bc;p.enabled90.reset(new std::uint32_t[capacity]);
        d.sourcePC=0x2877cc;p.kind94.reset(new std::uint32_t[capacity]);
        d.sourcePC=0x2877dc;p.scale98.reset(new float[capacity]);
        d.sourcePC=0x2877ec;p.inverse9C.reset(new float[capacity]);
        d.sourcePC=0x2877fc;p.wordA0.reset(new std::uint32_t[capacity]);
        d.sourcePC=0x28780c;p.wordA8.reset(new std::uint32_t[capacity]);
        d.sourcePC=0x28781c;p.wordA4.reset(new std::uint32_t[capacity]);
        d.sourcePC=0x287828;p.bytesAC.reset(new std::uint8_t[capacity]);
    }catch(...){
        //Native allocation rejection, not a fabricated original null-return
        //path. Earlier source stores/backing remain owned for exact cleanup.
        d.operation="SimpleVCR native allocation unavailable; partial setup retained, no retry";return false;
    }
    for(std::uint32_t i=0;i<capacity;++i){
        p.pointers84[i]=nullptr;p.pointers88[i]=nullptr;p.enabled90[i]=1;
        p.kind94[i]=0;p.scale98[i]=0.f;p.inverse9C[i]=0.f;
        p.wordA0[i]=0;p.wordA8[i]=0;p.wordA4[i]=0;
    }
    p.reset();p.setupComplete=true;d.sourcePC=0x2878dc;return true;
}
bool OriginalSimpleVCR50LA::registerRaceTime1A6E98(OriginalRaceConstructor50LA& race,
    OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};if(!impl_)return Impl::stop(d,0x1a6e98,"SimpleVCR constructor absent");auto& p=*impl_;
    if(p.thread!=std::this_thread::get_id()||race.resultsFeedbackThread_!=std::this_thread::get_id())
        return Impl::stop(d,0x1a6e98,"Race/SimpleVCR registration requires their owner thread");
    if(!p.current()||!p.setupComplete||p.registeredRace90||race.application_!=p.application||
       race.vcr78Owner_.get()!=this||race.vcr78Source_!=this||
       !race.counted_||race.rolledBack_||race.entity_.sessionGeneration!=p.component.sessionGeneration||
       !p.application->entityAuthority_.canRetire(race.entity_)||p.count80||p.attempted7C)
        return Impl::stop(d,0x1a6e98,"Actual first Race90 registration identity/order unavailable");
    ++p.attempted7C; //28790C delay slot, before any source registration gate.
    //The reached caller supplies100.f,0,0,1. Its positive-rate comparison
    //cannot fail, but source capacity/bit4 still precede pointer stores.
    if(p.count80>=p.capacity78||(p.flagsB0Known&0x10u)!=0x10u||(p.flagsB0Value&0x10u))
        return Impl::stop(d,0x287a04,"Actual SimpleVCR registration rejected after attempted7C increment");
    const auto i=p.count80;
    p.pointers84[i]=&race.field90_;p.pointers88[i]=&race.field90_; //A2=0 aliasesA1.
    p.words8C[i]=0;p.enabled90[i]=1;p.kind94[i]=0;
    p.scale98[i]=100.f;p.inverse9C[i]=1.f/100.f;
    ++p.count80;p.registeredRace90=&race;
    d.sourcePC=0x287a18;return true; //Actual return index0 is unused by1A6E98.
}
bool OriginalSimpleVCR50LA::belongsToRace(const OriginalRaceConstructor50LA& race)const noexcept {
    if(race.resultsFeedbackThread_!=std::this_thread::get_id()||!impl_)return false;
    const auto& p=*impl_;
    return p.current()&&p.setupComplete&&p.registeredRace90==&race&&
       race.vcr78Owner_.get()==this&&race.vcr78Source_==this&&
       race.application_==p.application&&race.counted_&&!race.rolledBack_&&
       race.entity_.sessionGeneration==p.component.sessionGeneration&&
       p.application->entityAuthority_.canRetire(race.entity_);
}
bool OriginalSimpleVCR50LA::readEndCallerBit5(const OriginalRaceConstructor50LA& race,
    bool& value,OriginalSimpleVCRDiagnostic50LA& d)const noexcept {
    d={};
    if(!belongsToRace(race)||(impl_->flagsB0Known&0x20u)!=0x20u)
        return Impl::stop(d,0x1a815c,"Published same-Race SimpleVCR bit5 unavailable");
    value=(impl_->flagsB0Value&0x20u)!=0;d.sourcePC=0x1a8168;return true;
}
bool OriginalSimpleVCR50LA::startRecord1A80C8(const OriginalRaceConstructor50LA& race,
    OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};
    if(!belongsToRace(race))
        return Impl::stop(d,0x1a80c8,"Actual same-Race SimpleVCR recording owner unavailable");
    auto& p=*impl_;
    //287B64 ANDI t2,zero,1 yields ZERO, not1. Preserve original five
    //ordered bit stores before the tail jump287BE0; bit0/4/6 untouched.
    constexpr std::uint8_t masks[]={0x20,0x02,0x04,0x08,0x80};
    for(const auto mask:masks){
        p.flagsB0Value=std::uint8_t(p.flagsB0Value&~mask);
        p.flagsB0Known=std::uint8_t(p.flagsB0Known|mask);
    }
    p.reset(); //Actual287BD8 tail jump, same registrations/count preserved.
    //Source caller1A80D0..EC after that return sets only bit1. All operations
    //are infallible after owner admission, with no callback or yield between.
    p.flagsB0Value=std::uint8_t(p.flagsB0Value|2u);
    p.flagsB0Known=std::uint8_t(p.flagsB0Known|2u);
    d.sourcePC=0x1a8158;return true;
}
bool OriginalSimpleVCR50LA::enableEndChannelsAndBackup1ABA68(
    const OriginalRaceConstructor50LA& race,std::int32_t& indexS0,
    OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};
    if(!belongsToRace(race))
        return Impl::stop(d,0x1abaa0,"Actual saved Race78 channel owner unavailable");
    auto& p=*impl_;
    //1ABAA4/AA8 reload count80 and compare SIGNED each iteration. The inner
    //1ABA74/78 reload is UNSIGNED. No cached capacity replaces either read.
    while(indexS0<std::bit_cast<std::int32_t>(p.count80)){
        if(indexS0>=0&&static_cast<std::uint32_t>(indexS0)<p.count80){
            //Native backing fence at the reached90 access, not a prevalidation
            //of untouched channels. Any earlier stores/index remain retained.
            if(!p.enabled90||static_cast<std::uint32_t>(indexS0)>=p.capacity78)
                return Impl::stop(d,0x1aba84,"Reached actual VCR90 channel backing unavailable");
            p.enabled90[static_cast<std::uint32_t>(indexS0)]=1u; //1ABA90.
        }
        ++indexS0; //1ABA80 likely delay or1ABA98; signed loop bounds prevent overflow.
    }
    //No callback/yield in the loop. The exact existing copy leaf retains its
    //real bit0-clear no-copy branch; no cursor/count/record flags are reset.
    return copyBackup288630(d);
}
bool OriginalSimpleVCR50LA::reset287BE0(OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};if(!impl_||!impl_->current()||!impl_->setupComplete)
        return Impl::stop(d,0x287be0,"Actual initialized SimpleVCR owner absent");
    impl_->reset();d.sourcePC=0x287c70;return true;
}
bool OriginalSimpleVCR50LA::copyBackup288630(OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};if(!impl_||!impl_->current()||!impl_->setupComplete)
        return Impl::stop(d,0x288630,"Actual initialized SimpleVCR owner absent");
    auto& p=*impl_;
    if(p.flagsB0Value&1u)std::memcpy(p.bufferB8.get(),p.buffer5C.get(),p.bytesB4);
    d.sourcePC=0x28865c;return true; //Bit0-clear branch is an actual no-copy return.
}
bool OriginalSimpleVCR50LA::closeDetached2875A0(OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};if(!impl_)return true;auto& p=*impl_;
    if(p.thread!=std::this_thread::get_id())
        return Impl::stop(d,0x2875a0,"SimpleVCR retirement requires its construction thread");
    if(p.closed)return true;
    if(p.counted){
        if(!p.current()||!p.application->objectCount423F60()||
           !p.application->entityAuthority_.retire(p.component))
            return Impl::stop(d,0x2875a0,"SimpleVCR detached actual Entity retirement unavailable");
        --p.component.reference00; //Actual last-reference release before2875A0.
    }
    //Exact nonnull free order2875C8..6C8; no registered pointer is deleted.
    p.buffer5C.reset();p.pointers84.reset();p.bufferB8.reset();p.pointers88.reset();
    p.words8C.reset();p.enabled90.reset();p.kind94.reset();p.scale98.reset();
    p.inverse9C.reset();p.wordA0.reset();p.wordA8.reset();p.bytesAC.reset();p.wordA4.reset();
    p.cursor60=nullptr;p.cursor64=nullptr;p.registeredRace90=nullptr; //Native borrow withdrawal.
    p.nativeWords.reset();p.nativeBytes.reset(); //After the channel arrays above.
    if(p.counted)--p.application->objectCount423F60(); //2876D0 ->1413E0.
    p.closed=true;d.sourcePC=0x287700;return true;
}
std::int32_t OriginalSimpleVCR50LA::addChannel2878F0(std::uint32_t* source,std::uint32_t* destination,
    std::uint32_t interpolation94,float scale,std::uint32_t enabled90)noexcept {
    if(!impl_||!impl_->current()||!impl_->setupComplete)return -1;
    return impl_->add(source,destination,0u,interpolation94,scale,enabled90,true);
}
bool OriginalSimpleVCR50LA::reserveNativeChannelBacking(std::size_t words,std::size_t bytes,
    std::uint32_t*& wordBase,std::uint8_t*& byteBase)noexcept {
    wordBase=nullptr;byteBase=nullptr;
    if(!impl_||!impl_->current()||!impl_->setupComplete||impl_->nativeWords||impl_->nativeBytes||
       !words||!bytes||words>4096u||bytes>4096u)return false;
    try {
        impl_->nativeWords.reset(new std::uint32_t[words]());
        impl_->nativeBytes.reset(new std::uint8_t[bytes]());
    }catch(...){impl_->nativeWords.reset();impl_->nativeBytes.reset();return false;}
    wordBase=impl_->nativeWords.get();byteBase=impl_->nativeBytes.get();return true;
}
std::int32_t OriginalSimpleVCR50LA::addByteChannel287A30(std::uint8_t* source,std::uint8_t* destination,
    std::uint32_t enabled90)noexcept {
    if(!impl_||!impl_->current()||!impl_->setupComplete)return -1;
    return impl_->add(source,destination,1u,4u,1.f,enabled90,false);
}
bool OriginalSimpleVCR50LA::setRate287C80(float rate,OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};if(!impl_||!impl_->current()||!impl_->setupComplete)
        return Impl::stop(d,0x287c80,"Actual initialized SimpleVCR owner absent");
    impl_->scalar50=rate;d.sourcePC=0x287c84;return true;
}
bool OriginalSimpleVCR50LA::update287C90(float dt,bool paused11C,OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};if(!impl_||!impl_->current()||!impl_->setupComplete)
        return Impl::stop(d,0x287c90,"Actual initialized SimpleVCR owner absent");
    auto& p=*impl_;
    if(paused11C){d.sourcePC=0x287cd8;return true;}
    p.setBit(0x10u,true); //287CE0..CF4: registration closes at the first update.
    if(p.bit(0x02u)){
        p.time30+=dt;
        if(p.keyframe40==0.f){p.keyframe();p.sample44=p.scalar4C;}
        else if(p.sample44<=0.f){p.delta();p.sample44=p.scalar4C;}
        p.keyframe40-=dt;p.sample44-=dt;
    }
    if(p.bit(0x04u))p.play(dt);
    d.sourcePC=0x287db0;return true;
}
bool OriginalSimpleVCR50LA::recordSampleDue()const noexcept {
    if(!impl_||!impl_->current()||!impl_->setupComplete)return false;
    const auto& p=*impl_;
    return p.bit(0x02u)&&(p.keyframe40==0.f||p.sample44<=0.f);
}
bool OriginalSimpleVCR50LA::setPlayback(bool clearRecord,OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};if(!impl_||!impl_->current()||!impl_->setupComplete)
        return Impl::stop(d,0x1abb44,"Actual initialized SimpleVCR owner absent");
    if(clearRecord)impl_->setBit(0x02u,false);
    impl_->setBit(0x04u,true);d.sourcePC=0x1abb68;return true;
}
OriginalSimpleVCR50LA::State OriginalSimpleVCR50LA::state()const noexcept {
    State s;if(!impl_)return s;const auto& p=*impl_;
    s.recording=p.bit(0x02u);s.playing=p.bit(0x04u);s.ended=p.bit(0x20u);s.full=p.bit(0x80u);
    s.channels=p.count80;s.bytesUsed=p.word74;s.capacityBytes=p.bytesB4;
    s.keyframes=p.keyframes;s.deltas=p.deltas;s.escapes=p.escapes;s.framesRead=p.framesRead;
    s.time30=p.time30;s.play34=p.play34;return s;
}

bool OriginalRaceConstructor50LA::constructVcr78At1A6DF4(
    OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};
    const auto stop=[&](std::uint32_t pc,const char* why)noexcept {
        d.sourcePC=pc;d.operation=why;return false;
    };
    // This source leaf has NO caller. Its lexical source predecessor is the
    // return/skip of loading-window1A6DEC, AFTER1A9240/music/archive/1A9EE0/
    //1A95B0. No existing native scene or Camera state substitutes for that.
    // The guards below authenticate owners, NOT those missing returns.
    if(resultsFeedbackThread_!=std::this_thread::get_id())
        return stop(0x1a6df4,"SimpleVCR producer requires actual Race owner thread");
    if(!counted_||rolledBack_||!constructorSuffix_||!loadPrefixReached_||
       !application_||!application_->application10()||application_->attachmentLock423F58()||
       !application_->entityAuthority_.canRetire(entity_)||!profile_||
       !original_entity_chain_detail50LA::forest(application_->entityAuthority_.view()))
        return stop(0x1a6df4,"Actual counted Race/Application/Profile unavailable");
    using Cut=VcrLoadCut;
    if(vcrLoadCut_==Cut::retired)
        return stop(0x1a6df4,"Canceled native VCR invocation cannot restart");
    platform::OriginalUiRiderProfileOwner50KG::CourseRaceSelection current;
    if(!profile_->courseRaceSnapshot(current))
        return stop(0x1a6e0c,"Actual Profile subtype read unavailable");
    const auto stamp=profile_->stamp();
    if(vcrLoadCut_==Cut::subtype){
        if(vcr78Owner_||vcr78Source_)
            return stop(0x1a6df4,"Existing Race78 custody cannot be replaced");
        vcrLoadProfile_=stamp;vcrLoadSubtype_=current.subtype;
        vcrLoadCut_=Cut::allocate;
    }else if(!vcrLoadProfile_||stamp.generation!=vcrLoadProfile_->generation||
             stamp.mode34!=vcrLoadProfile_->mode34||current.subtype!=vcrLoadSubtype_)
        return stop(0x1a6e0c,"Suspended VCR producer Profile changed; prior effects retained");
    const bool backup=vcrLoadSubtype_==3u;
    if(vcrLoadCut_==Cut::allocationRejected)
        return stop(backup?0x1a6e18:0x1a6e50,"Native allocation/constructor rejected; no automatic retry");
    if(vcrLoadCut_==Cut::setupRejected)
        return stop(0x287710,"Partial SimpleVCR setup retained; no replay or source publication");
    if(vcrLoadCut_==Cut::registrationRejected)
        return stop(0x1a6e98,"Attempted Race90 registration retained; no duplicate attempt");
    if(vcrLoadCut_==Cut::allocate){
        // Native fail-stop for an allocation rejection, NOT the original
        // unchecked-null287710 call. Keep any counted partial object owned.
        vcrLoadCut_=Cut::allocationRejected;
        try {vcr78Owner_.reset(new OriginalSimpleVCR50LA(application_,entity_.sessionGeneration));}
        catch(...){return stop(backup?0x1a6e18:0x1a6e50,"Native SimpleVCR allocation unavailable");}
        if(!vcr78Owner_->constructed())
            return stop(0x287430,"SimpleVCR constructor not complete; custody retained for native cleanup");
        vcrLoadCut_=Cut::setup;
    }
    if(!vcr78Owner_||!vcr78Owner_->constructed())
        return stop(0x287710,"Suspended SimpleVCR custody unavailable");
    if(vcrLoadCut_==Cut::setup){
        if(vcr78Source_)
            return stop(backup?0x1a6e3c:0x1a6e74,"Source78 was published before setup returned");
        vcrLoadCut_=Cut::setupRejected; //Before fallible allocating body.
        if(!vcr78Owner_->setup287710(backup?0x28u:0xa0u,backup?0x20000u:0x40000u,backup,d))
            return false;
        vcrLoadCut_=Cut::publish;
    }
    if(vcrLoadCut_==Cut::publish){
        if(vcr78Source_)
            return stop(backup?0x1a6e48:0x1a6e7c,"Race78 changed before its source publication");
        vcr78Source_=vcr78Owner_.get(); //Exactly1A6E48/7C, after287710 return.
        vcrLoadCut_=Cut::register90;
    }
    if(vcr78Source_!=vcr78Owner_.get())
        return stop(0x1a6e80,"Actual published Race78 was replaced");
    if(vcrLoadCut_==Cut::register90){
        vcrLoadCut_=Cut::registrationRejected; //28790C attempt may commit before rejection.
        if(!vcr78Source_->registerRaceTime1A6E98(*this,d))return false;
        vcrLoadCut_=Cut::afterRegistration;
    }
    if(vcrLoadCut_!=Cut::afterRegistration||!vcr78Source_->belongsToRace(*this))
        return stop(0x1a6e98,"Actual same-Race90 registration unavailable");
    // Bounded leaf only: next instruction is another loading-window block.
    // No racer channels, replay child attachment, or full-load return follows.
    d.sourcePC=0x1a6ea0;d.operation="Race78 setup/publication/actual Race90 registration returned; later load remains";
    return true;
}

bool OriginalRaceConstructor50LA::retireNativeVcr78Detached(
    OriginalSimpleVCRDiagnostic50LA& d)noexcept {
    d={};
    const auto stop=[&](const char* why)noexcept {
        d.sourcePC=0x2875a0;d.operation=why;return false;
    };
    if(resultsFeedbackThread_!=std::this_thread::get_id())
        return stop("SimpleVCR native cleanup requires actual Race thread");
    // No live replay/Results borrow or reached end-caller may survive target
    // withdrawal. This is a detached native cancel body, not source teardown.
    if(!endHud_.expired()||endEntryStep_!=EndEntryStep::subtype||endCallerVcr_||
       nativeRootAttached_||!application_||application_->attachmentLock423F58())
        return stop("SimpleVCR native cleanup requires withdrawn live borrows");
    if(vcr78Source_&&vcr78Source_!=vcr78Owner_.get())
        return stop("SimpleVCR source/custody identity changed");
    if(vcr78Owner_&&!vcr78Owner_->closeDetached2875A0(d))return false;
    // close withdraws registered90 before Race custody can release its target.
    // Retirement does not restore a load-time null78 fact or enable retries.
    vcr78Source_=nullptr;vcr78Owner_.reset();vcrLoadCut_=VcrLoadCut::retired;
    d.sourcePC=0x287700;return true;
}

}
