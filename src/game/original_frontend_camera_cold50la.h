#pragma once
#include "original_application_entities50la.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <exception>
#include <memory>
#include <limits>
#include <thread>
#include <utility>

namespace original_main_entry50KG { struct Application; }
namespace rr::game {

// Actual-Application-issued236180 plus an optional private scalar prefix of
//2361C0 ending after1149D4, BEFORE entering114A00. No full initializer, renderer,
// visibility, parent, observer or live camera callback is implemented here.
// Registry operations are serialized on the actual entities owner's original
// thread. Allocators/hooks must not reenter or overlap ANY shared-registry
// operation; this owner does not pretend to retrofit a universal registry lock.
class OriginalFrontendCameraCold50LA final {
    friend struct ::original_main_entry50KG::Application;
public:
    enum class Status : std::uint8_t {
        complete,outputNotEmpty,owner,affinity,phase,identity,topology,
        attachmentLocked,reservation,allocation,arithmetic
    };
    enum class Phase : std::uint8_t {
        empty,constructedCold,released,
        unavailable, // diagnostic only, never an owned construction phase
        beforeViewport114A00 // partial initializer; pending service is NOT entered
    };
    struct Result {
        Status status=Status::owner;
        // Source boundary for diagnosis, not a claim this instruction executed.
        std::uint32_t sourcePC=0;
        bool okay()const noexcept{return status==Status::complete;}
    };
    struct KnownWord {
        std::uint32_t value=0,knownMask=0;
    };
private:
    using Entities=OriginalApplicationEntities50LA;
    using Component=OriginalWreckComponent50LA;
    using Kind=OriginalWreckComponentKind50LA;
    using Words4=std::array<std::uint32_t,4>;
    using Words16=std::array<std::uint32_t,16>;
    struct Fields {
        // Host-safe backing alone conveys no known source state. These fields
        // become observable only after the entire source constructor completes.
        std::uint16_t searchable30=0;
        std::uint8_t type32=0,flags33=0;
        const void* registration34=nullptr;
        const void* parent38=nullptr;
        Words16 matrix40{},matrix140{};
        Words4 position180{},look190{},up1A0{},previousPosition1B0{},previousLook1C0{};
        std::uint32_t fov1D0=0,roll1D4=0;
        // Safe native backing is not known source state in constructedCold.
        std::uint32_t aspect1DC=0,aspect1F8=0,fullWidth204=0,fullHeight208=0;
        std::array<std::uint32_t,6> viewport1E0{};
        std::uint32_t near1FC=0,far200=0,count20C=0,count210=0;
        std::uint32_t word214=0,word218=0,word21C=0,word220=0;
        const void* resource224=nullptr;
        std::uint32_t word228=0,word22C=0,word230=0,word234=0,word238=0,word23C=0;
        std::uint32_t movement240=0,angular244=0,zoom328=0,zoom32C=0;
        // The four words above acquire source knowledge ONLY in the private
        // beforeViewport114A00 phase. All other450 wholly unwritten bytes
        // remain absent, including bounds318..324, projection290 and guards.
        // There is no initialized/live camera or matrix readiness state.
    };
    const std::shared_ptr<Entities> entities_;
    // Comparison only. Destructor never dereferences an expired Application.
    const ::original_main_entry50KG::Application* const issuer_;
    Component entity_;
    const std::uint64_t generation_,key_;
    Phase phase_=Phase::empty;
    bool entityConstructed_=false,fieldsComplete_=false,enrolled_=false;
    std::uint32_t ancestryTable_=0; // source type evidence, never callable
    Fields fields_;

    static std::uint64_t nativeKey(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    OriginalFrontendCameraCold50LA(const ::original_main_entry50KG::Application* issuer,
        const std::shared_ptr<Entities>& entities)noexcept
        :entities_(entities),issuer_(issuer),generation_(nativeKey(entities.get())),
         key_(nativeKey(&entity_)){}
    bool onAffinity()const noexcept {
        return entities_&&entities_->nativeOwnerThread_==std::this_thread::get_id();
    }
    static Words16 identityBits()noexcept {
        return {{0x3f800000u,0,0,0,0,0x3f800000u,0,0,
                 0,0,0x3f800000u,0,0,0,0,0x3f800000u}};
    }
    bool actualEntitiesAlive()const noexcept {
        // Called only after affinity admission and under the shared registry's
        // existing no-overlap/no-reentrancy contract.
        return entities_&&entities_->constructed_&&!entities_->released_&&
            generation_==nativeKey(entities_.get())&&
            entities_->rootDomain_.sessionGeneration==generation_&&
            entities_->entityAuthority_.view().valid();
    }
    bool freshComponent()const noexcept {
        return !entityConstructed_&&!fieldsComplete_&&!enrolled_&&phase_==Phase::empty&&
            entity_.kind==Kind::unbound&&!entity_.reference00&&!entity_.sessionGeneration&&
            !entity_.objectKey&&!entity_.nativeEffectControlId&&
            !entity_.previous10&&!entity_.next14&&!entity_.firstChild18&&!entity_.parent1C&&
            !entity_.mask20&&!entity_.descendants24&&generation_&&key_;
    }
    void constructFields1143F0_236180()noexcept {
        // Source order after141300. Unknown upper33 bits are not made known by
        // native safe initialization; public observation always masks them off.
        fields_.searchable30=0;                        //114414
        fields_.type32=0xffu;                          //114428
        fields_.flags33=static_cast<std::uint8_t>(fields_.flags33&0xf8u); //114448/45C/470
        ancestryTable_=0x3cca00u;                     //114478
        const Words4 vuZero{{0,0,0,0x3f800000u}};
        fields_.position180=vuZero;                    //114484
        fields_.look190=vuZero;                        //114490
        fields_.up1A0=vuZero;                          //11449C
        fields_.previousPosition1B0=vuZero;            //1144A8
        fields_.previousLook1C0=vuZero;                //1144B4
        fields_.word21C=0x42da0000u;                   //1144D0
        fields_.word220=0x41200000u;                   //1144E4
        fields_.fov1D0=0x429a0000u;                    //1144EC
        fields_.resource224=nullptr;                  //1144F4
        fields_.word228=0;                            //1144FC
        fields_.word22C=0;                            //114504
        fields_.near1FC=0x3f800000u;                   //11450C
        fields_.far200=0x47c35000u;                    //114514
        fields_.count20C=0;                           //11451C
        fields_.count210=0;                           //114524
        fields_.viewport1E0.fill(0);                  //11452C,18 bytes through1F7
        fields_.matrix40=identityBits();              //114558/55C copy loop
        fields_.look190={{0,0,0x3f800000u,0x3f800000u}}; //114598
        fields_.up1A0={{0,0x3f800000u,0,0x3f800000u}}; //1145B4
        fields_.position180=vuZero;                   //1145D0
        fields_.position180={{0,0x42480000u,0x420c0000u,0x3f800000u}}; //1145EC
        fields_.roll1D4=0;                            //1145F4
        fields_.word214=1;                            //1145FC,integer bits
        fields_.word218=1;                            //114604,integer bits
        fields_.word230=0;                            //11460C
        fields_.word23C=0;                            //114614,before234/238
        fields_.word234=0;                            //11461C
        fields_.word238=0;                            //114624
        fields_.movement240=0;                        //11462C
        fields_.angular244=0;                         //114634
        fields_.previousLook1C0=fields_.look190;       //114640
        fields_.previousPosition1B0=fields_.position180; //114650,delay slot
        fields_.matrix140=identityBits();             //114670/674 copy loop
        fields_.registration34=nullptr;              //114688
        fields_.parent38=nullptr;                    //114690
        ancestryTable_=0x3d1c10u;                     //23619C
        fields_.zoom328=0x3f800000u;                  //2361A4
        fields_.zoom32C=0x3f800000u;                  //2361A8
        fieldsComplete_=true;
    }
    static Result construct(const ::original_main_entry50KG::Application* issuer,
        const std::shared_ptr<Entities>& entities,
        std::shared_ptr<OriginalFrontendCameraCold50LA>& output)noexcept {
        if(output)return {Status::outputNotEmpty,0x236180u};
        if(!issuer||!entities)return {Status::owner,0x236180u};
        // This immutable owner property is read before mutable registry state.
        if(entities->nativeOwnerThread_!=std::this_thread::get_id())
            return {Status::affinity,0x236180u};
        if(!entities->constructed_||entities->released_||
           entities->rootDomain_.sessionGeneration!=nativeKey(entities.get())||
           !entities->entityAuthority_.view().valid())return {Status::owner,0x236180u};
        if(entities->attachmentLock423F58_)return {Status::attachmentLocked,0x236180u};
        std::shared_ptr<OriginalFrontendCameraCold50LA> next;
        try {
            // Both object storage and the shared control block precede141300.
            // Control-block failure destroys only an inert, empty owner.
            next=std::shared_ptr<OriginalFrontendCameraCold50LA>(
                new OriginalFrontendCameraCold50LA(issuer,entities));
        }catch(...){return {Status::allocation,0x236180u};}
        auto& authority=entities->entityAuthority_;
        if(!authority.reserveAdditional(1))return {Status::reservation,0x141300u};
        if(!next->actualEntitiesAlive()||entities->attachmentLock423F58_||
           !next->freshComponent()||!authority.canEnroll(next->entity_,next->key_))
            return {Status::identity,0x141300u};
        // Native allocations end before this source141300 boundary.
        const auto initialized=initializeOriginalWreckComponent50LA(next->entity_,
            Kind::camera236180,next->generation_,next->key_,true,entities->objectCount423F60_);
        if(initialized.status!=OriginalWreckComponentStatus50LA::complete)std::terminate();
        next->entityConstructed_=true;
        next->constructFields1143F0_236180();
        if(!authority.enroll(next->entity_))std::terminate();
        next->enrolled_=true;
        // Canonical enrollment completed; no source service was invoked.
        next->phase_=Phase::constructedCold;
        output=std::move(next);
        return {Status::complete,0x2361bcu};
    }
    Result coldRetirementPreflight()const noexcept {
        if(!onAffinity())return {Status::affinity,0x104970u};
        if(!actualEntitiesAlive())return {Status::owner,0x104970u};
        if((phase_!=Phase::constructedCold&&phase_!=Phase::beforeViewport114A00)||
           !entityConstructed_||!fieldsComplete_||!enrolled_)
            return {Status::phase,0x104970u};
        if(entities_->attachmentLock423F58_)return {Status::attachmentLocked,0x104970u};
        if(entity_.kind!=Kind::camera236180||entity_.objectKey!=key_||
           key_!=nativeKey(&entity_)||entity_.sessionGeneration!=generation_)
            return {Status::identity,0x104970u};
        if(entity_.reference00!=1||entity_.previous10||entity_.next14||
           entity_.firstChild18||entity_.parent1C||entity_.mask20||entity_.descendants24||
           entity_.category28!=0xffu||(entity_.flags29&0x0fu)!=3u||
           entity_.nativeEffectControlId)return {Status::topology,0x104970u};
        if(ancestryTable_!=0x3d1c10u||fields_.searchable30||fields_.type32!=0xffu||
           (fields_.flags33&7u)||fields_.registration34||fields_.parent38||fields_.resource224)
            return {Status::phase,0x104970u};
        if(!scalarPrefixStateValid())return {Status::phase,0x104970u};
        if(!entities_->entityAuthority_.canRetire(entity_))return {Status::identity,0x104970u};
        return {Status::complete,0x104970u};
    }
    bool scalarPrefixStateValid()const noexcept {
        // Neither admitted phase has entered114A00's renderer-counter stores.
        if(fields_.count20C||fields_.count210)return false;
        if(phase_==Phase::constructedCold){
            // Zero host backing for unwritten source words is only an invariant;
            // knownWord continues to report these four offsets as unknown.
            return !fields_.aspect1DC&&!fields_.aspect1F8&&!fields_.fullWidth204&&
                !fields_.fullHeight208&&fields_.viewport1E0==std::array<std::uint32_t,6>{};
        }
        return phase_==Phase::beforeViewport114A00&&
            fields_.fullWidth204==640u&&fields_.fullHeight208==448u&&
            fields_.aspect1DC==0x3faaaaabu&&fields_.aspect1F8==0x3f400000u&&
            fields_.viewport1E0==std::array<std::uint32_t,6>{{0,0,640,448,0,0x3f800000u}};
    }
    static std::uint32_t floatBits(float value)noexcept {
        static_assert(sizeof(float)==sizeof(std::uint32_t),"binary32 float storage required");
        std::uint32_t bits=0;std::memcpy(&bits,&value,sizeof(bits));return bits;
    }
    static float floatFromBits(std::uint32_t bits)noexcept {
        float value=0;std::memcpy(&value,&bits,sizeof(value));return value;
    }
    static bool admitViewportArithmetic114960(std::uint32_t& aspectBits)noexcept {
        // The existing native instruction-step policy, NOT universal EE/FCR31
        // equivalence. Strict compilation (-fno-fast-math -frounding-math
        // -ffp-contract=off) remains required. Never change the caller's mode.
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
        (void)aspectBits;return false;
#else
        if(!std::numeric_limits<float>::is_iec559||std::numeric_limits<float>::radix!=2||
           std::numeric_limits<float>::digits!=24||std::fegetround()!=FE_TONEAREST)return false;
        // Host-only arithmetic admission precedes source-field publication. Each
        // volatile result is a separate binary32 step; do not cancel ratios,
        // substitute a literal final aspect, contract or fuse these operations.
        volatile std::int32_t width=640,height=448;
        volatile float viewportHeight=static_cast<float>(height); //11496C
        volatile float viewportWidth=static_cast<float>(width);   //114980
        volatile float viewportRatio=viewportHeight/viewportWidth; //114998
        volatile float inputAspect=floatFromBits(0x3faaaaabu);    //1149A8 input
        volatile float fullHeight=static_cast<float>(height);     //1149B4
        volatile float fullWidth=static_cast<float>(width);       //1149B8
        volatile float fullRatio=fullHeight/fullWidth;           //1149BC
        volatile float product=inputAspect*fullRatio;            //1149C0
        volatile float aspect=viewportRatio/product;             //1149C4
        const float vh=viewportHeight,vw=viewportWidth,vr=viewportRatio;
        const float fh=fullHeight,fw=fullWidth,fr=fullRatio,p=product,a=aspect;
        if(!std::isfinite(vh)||!std::isfinite(vw)||!std::isfinite(vr)||
           !std::isfinite(fh)||!std::isfinite(fw)||!std::isfinite(fr)||
           !std::isfinite(p)||!std::isfinite(a)||vw==0||fw==0||p==0)return false;
        // Exact normal expected intermediates reject an incompatible arithmetic
        // execution before any object write. The final value is still computed.
        if(floatBits(vh)!=0x43e00000u||floatBits(vw)!=0x44200000u||
           floatBits(fh)!=0x43e00000u||floatBits(fw)!=0x44200000u||
           floatBits(vr)!=0x3f333333u||floatBits(fr)!=0x3f333333u||
           floatBits(p)!=0x3f6eeeefu||floatBits(a)!=0x3f400000u)return false;
        aspectBits=floatBits(a);return true;
#endif
    }
    Result stageViewportPrefix1149D4(const ::original_main_entry50KG::Application* issuer,
        const std::shared_ptr<Entities>& entities)noexcept {
        // Issuer and exact retained entities identity precede phase decisions.
        if(!issuer||issuer!=issuer_||!entities||entities.get()!=entities_.get())
            return {Status::owner,0x2361c0u};
        if(!onAffinity())return {Status::affinity,0x2361c0u};
        if(phase_!=Phase::constructedCold)return {Status::phase,0x2361c0u};
        const auto admitted=coldRetirementPreflight();
        if(!admitted.okay())return admitted;
        std::uint32_t aspectBits=0;
        if(!admitViewportArithmetic114960(aspectBits))return {Status::arithmetic,0x114960u};
        // Native atomic preflight above is not an original failure branch. The
        // authentic Main1CF320/shared-page1D7B28 arguments are fixed640/448 and
        //3FAAAAAB. Source2361C0 enters1146B0, whose inert141C20 returns
        // before114920 enters114960. No service/callback is run.
        // All ten source stores publish in recovered order after admission.
        fields_.fullWidth204=640u;                   //114928 AC850204
        fields_.fullHeight208=448u;                  //11492C AC860208
        fields_.aspect1DC=0x3faaaaabu;               //114930 E48C01DC
        fields_.viewport1E0[0]=0;                    //11497C AC8501E0
        fields_.viewport1E0[1]=0;                    //114984 AC8601E4
        fields_.viewport1E0[2]=640u;                 //114988 AC8701E8
        fields_.viewport1E0[3]=448u;                 //11498C AC8801EC
        fields_.viewport1E0[4]=0;                    //114990 AC8001F0
        fields_.viewport1E0[5]=0x3f800000u;           //11499C AC8201F4
        fields_.aspect1F8=aspectBits;                //1149D4 E48001F8
        // Continuation evidence only: next source entry is114A00(this,0).
        // No raw pending JAL/return pointer escapes and no continuation API is
        // supplied. Full initializer return, renderer20C/210, +78,26F940 and
        // bounds/guard writes remain unreached. Registry/count are unchanged.
        phase_=Phase::beforeViewport114A00;
        return {Status::complete,0x1149d4u};
    }
    Result retireRetained()noexcept {
        const auto admitted=coldRetirementPreflight();
        if(!admitted.okay())return admitted;
        // Complete admission precedes the first mutation. Compaction preserves
        // unrelated identities/order; no count baseline or capacity is reset.
        if(!entities_->entityAuthority_.retire(entity_))std::terminate();
        enrolled_=false;
        --entity_.reference00;                        //104970's local ref effect
        --entities_->objectCount423F60_;              //1413E0's source32 count effect
        phase_=Phase::released;
        return {Status::complete,0x141420u};
    }
    Result release(const ::original_main_entry50KG::Application* issuer,
        const std::shared_ptr<Entities>& entities)noexcept {
        // Identity checks MUST precede already-released/idempotent success.
        if(!issuer||issuer!=issuer_||!entities||entities.get()!=entities_.get())
            return {Status::owner,0x104970u};
        if(!onAffinity())return {Status::affinity,0x104970u};
        if(phase_==Phase::released)return {Status::complete,0x141420u};
        return retireRetained();
    }
    static KnownWord known(std::uint32_t value)noexcept{return {value,0xffffffffu};}
public:
    OriginalFrontendCameraCold50LA(const OriginalFrontendCameraCold50LA&)=delete;
    OriginalFrontendCameraCold50LA& operator=(const OriginalFrontendCameraCold50LA&)=delete;
    OriginalFrontendCameraCold50LA(OriginalFrontendCameraCold50LA&&)=delete;
    OriginalFrontendCameraCold50LA& operator=(OriginalFrontendCameraCold50LA&&)=delete;
    ~OriginalFrontendCameraCold50LA()noexcept {
        // Even an empty/released camera retains the immutable entities pin.
        // Its member destruction may release the last pin and destroy actual
        // Application roots; therefore EVERY last drop must keep affinity.
        if(!onAffinity())std::terminate();
        if(phase_==Phase::empty){
            if(entityConstructed_||fieldsComplete_||enrolled_)std::terminate();
            return;
        }
        if(phase_==Phase::released){
            if(enrolled_||entity_.reference00)std::terminate();
            return; // camera retirement is inert; retained-pin destruction is not
        }
        // No dereference of issuer_, and no Application/renderer services. The
        // exact entities owner remains pinned until this destructor finishes.
        if(!retireRetained().okay())std::terminate();
    }
    Phase phase()const noexcept {
        // Off-thread diagnostics do not read mutable owner/registry state.
        return onAffinity()?phase_:Phase::unavailable;
    }
    KnownWord knownWord(std::uint16_t offset)const noexcept {
        // Value-only diagnostic, never a camera/Entity pointer or readiness
        // receipt. No source observation exists for an unwritten/aligned hole.
        if(!onAffinity()||(phase_!=Phase::constructedCold&&phase_!=Phase::beforeViewport114A00)||
           !fieldsComplete_||(offset&3u))return {};
        if(offset>=0x40u&&offset<0x80u)return known(fields_.matrix40[(offset-0x40u)/4u]);
        if(offset>=0x140u&&offset<0x180u)return known(fields_.matrix140[(offset-0x140u)/4u]);
        if(offset>=0x180u&&offset<0x190u)return known(fields_.position180[(offset-0x180u)/4u]);
        if(offset>=0x190u&&offset<0x1a0u)return known(fields_.look190[(offset-0x190u)/4u]);
        if(offset>=0x1a0u&&offset<0x1b0u)return known(fields_.up1A0[(offset-0x1a0u)/4u]);
        if(offset>=0x1b0u&&offset<0x1c0u)return known(fields_.previousPosition1B0[(offset-0x1b0u)/4u]);
        if(offset>=0x1c0u&&offset<0x1d0u)return known(fields_.previousLook1C0[(offset-0x1c0u)/4u]);
        if(offset>=0x1e0u&&offset<0x1f8u)return known(fields_.viewport1E0[(offset-0x1e0u)/4u]);
        switch(offset){
        case 0x000:return known(static_cast<std::uint32_t>(entity_.reference00));
        case 0x004:return known(static_cast<std::uint32_t>(entity_.reference00>>32u));
        case 0x008:return known(ancestryTable_);
        case 0x010:return entity_.previous10?KnownWord{}:known(0);
        case 0x014:return entity_.next14?KnownWord{}:known(0);
        case 0x018:return entity_.firstChild18?KnownWord{}:known(0);
        case 0x01c:return entity_.parent1C?KnownWord{}:known(0);
        case 0x020:return known(entity_.mask20);
        case 0x024:return known(entity_.descendants24);
        case 0x028:return {static_cast<std::uint32_t>(entity_.category28)|
            (static_cast<std::uint32_t>(entity_.flags29&0x0fu)<<8u),0x00000fffu};
        case 0x030:return {static_cast<std::uint32_t>(fields_.searchable30)|
            (static_cast<std::uint32_t>(fields_.type32)<<16u)|
            (static_cast<std::uint32_t>(fields_.flags33&7u)<<24u),0x07ffffffu};
        case 0x034:return fields_.registration34?KnownWord{}:known(0);
        case 0x038:return fields_.parent38?KnownWord{}:known(0);
        case 0x1d0:return known(fields_.fov1D0);
        case 0x1d4:return known(fields_.roll1D4);
        case 0x1dc:return phase_==Phase::beforeViewport114A00?known(fields_.aspect1DC):KnownWord{};
        case 0x1f8:return phase_==Phase::beforeViewport114A00?known(fields_.aspect1F8):KnownWord{};
        case 0x1fc:return known(fields_.near1FC);
        case 0x200:return known(fields_.far200);
        case 0x204:return phase_==Phase::beforeViewport114A00?known(fields_.fullWidth204):KnownWord{};
        case 0x208:return phase_==Phase::beforeViewport114A00?known(fields_.fullHeight208):KnownWord{};
        case 0x20c:return known(fields_.count20C);
        case 0x210:return known(fields_.count210);
        case 0x214:return known(fields_.word214);
        case 0x218:return known(fields_.word218);
        case 0x21c:return known(fields_.word21C);
        case 0x220:return known(fields_.word220);
        case 0x224:return fields_.resource224?KnownWord{}:known(0);
        case 0x228:return known(fields_.word228);
        case 0x22c:return known(fields_.word22C);
        case 0x230:return known(fields_.word230);
        case 0x234:return known(fields_.word234);
        case 0x238:return known(fields_.word238);
        case 0x23c:return known(fields_.word23C);
        case 0x240:return known(fields_.movement240);
        case 0x244:return known(fields_.angular244);
        case 0x328:return known(fields_.zoom328);
        case 0x32c:return known(fields_.zoom32C);
        default:return {};
        }
    }
};
} // namespace rr::game
