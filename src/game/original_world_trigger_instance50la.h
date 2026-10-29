#pragma once
#include "original_collision_base_owner50la.h"
#include "trigger_sphere50in.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <memory>
#include <new>
namespace ra_pod_scene50LA {class Owner;}
namespace rr::game {
// One actual 2A0290 source object. This is not a ready TriggerActions owner.
// Only the World source descriptor owns it. All source aliases are separate
// from native allocation custody, including allocations not yet published.
class OriginalWorldTriggerInstance50LA final {
    friend class ::ra_pod_scene50LA::Owner;
    using C=OriginalWreckComponent50LA;
    using B=OriginalCollisionBaseOwner50LA;
    using S=OriginalWreckComponentStatus50LA;
    C entity_;
    OriginalApplicationEntities50LA* application_=nullptr;
    bool counted_=false,enrolled_=false,colliderEnrolled_=false;
    bool constructed_=false,setupAttempted_=false,setupReturned_=false,nativeClosed_=false;
    std::uint32_t pc_=0,vtable08_=0;
    SpringVector4_48N position30_{},box40_{};
    BodyMatrix48G matrix50_{};
    std::uint8_t flags1B8_=0,flagsKnown_=0;
    std::optional<std::uint32_t> random90_,capacity94_,count98_,field1A8_,field1B4_;
    std::optional<float> radius1AC_;
    std::array<std::optional<char>,260> parentA4_{}; //Unknown until actual2DC068 strcpy.
    bool oneTimeWritten_=false,parentWritten_=false;
    std::unique_ptr<const void*[]> callbacks9C_,argumentsA0_;
    std::optional<const void*> source9C_,sourceA0_;
    std::unique_ptr<B> collider_;
    std::optional<B*> source1B0_; // Constructor does not write 1B0.
    struct Shape {
        TriggerSphereShape50IN value;
        std::uint8_t known=0;
        std::optional<const void*> owner60;
    };
    struct Result {
        std::optional<std::uint32_t> capacity00,count04;
        // Source never writes 08. Native zero backing is not knowledge.
        std::optional<float> distance08;
        SpringVector4_48N center20{};
        std::array<SpringVector4_48N,3> normals30{},points60{};
    };
    std::unique_ptr<Shape> shape_;
    std::unique_ptr<Result> result_;
    bool shapeAttempted_=false,shapeReturned_=false,refreshReturned_=false;
    bool colliderAllocationReturned_=false,shapeAllocationReturned_=false,resultAllocationReturned_=false;
    bool callbacksAllocationReturned_=false,argumentsAllocationReturned_=false;
    // Code-address identity, not a cast host callback or execution receipt.
    std::optional<std::uint32_t> callbackE8PC_;
    static void identity(BodyMatrix48G& m)noexcept {
        std::memset(m.v,0,sizeof m.v);m.v[15]=1;m.v[10]=1;m.v[5]=1;m.v[0]=1;
    }
    bool construct2A0290(OriginalApplicationEntities50LA& app,std::uint64_t generation,
        std::uint64_t key,const std::array<std::uint32_t,4>& actual4438D0,std::uint32_t& pc)noexcept {
        if(application_||counted_||constructed_||nativeClosed_)return false;
        application_=&app;pc=pc_=0x2a029c;
        const auto r=initializeOriginalWreckComponent50LA(entity_,OriginalWreckComponentKind50LA::trigger2A0290,
            generation,key,true,app.objectCount423F60());
        if(r.status!=S::complete){pc=pc_=r.sourcePC;return false;}counted_=true;
        pc_=0x2a02b0;vtable08_=0x3d3930;
        position30_={{0,0,0,1}};box40_={{0,0,0,1}}; // 2A02BC/2A02C8
        flags1B8_|=1;flagsKnown_|=1; //2A02FC
        flags1B8_|=2;flagsKnown_|=2; //2A032C
        source9C_=nullptr;sourceA0_=nullptr;field1A8_=0; //2A0338/340/348
        for(unsigned bit=2;bit<6;++bit){flags1B8_&=static_cast<std::uint8_t>(~(1u<<bit));flagsKnown_|=1u<<bit;}
        random90_=100; //2A03A0; upper two flag bits remain unknown.
        std::memcpy(&position30_,actual4438D0.data(),16); //2A03AC
        radius1AC_=0.f; //2A03B8 delay slot before genuine identity14D840.
        identity(matrix50_); // source eight 8-byte copies2A03D8/DC.
        capacity94_=0;count98_=0;field1B4_=0;
        constructed_=true;pc=pc_=0x2dbffc;return true;
    }
    // Native finite/FE_TONEAREST adaptation explicitly authorized for this
    // authored cohort. NOT an EE bit-exact MUL.S emulation claim. Refusal at
    // the operation retains every preceding allocation/publication/store.
    bool sphere11C0B0(const SpringVector4_48N& position,float radius,std::uint32_t& pc)noexcept {
        auto& b=*collider_;if(!b.freshShapeNull()||shapeAttempted_)return false;
        shapeAttempted_=true;pc=pc_=0x11c0dc; //11B600 genuine fresh-NULL branch.
        b.shape8C_=nullptr;b.result90_=nullptr;
        pc=pc_=0x11c0e4;try{shape_.reset(new Shape);}catch(...){shape_.reset();}shapeAllocationReturned_=true;
        if(shape_){shape_->value.localCenter={{0,0,0,1}};shape_->known|=1;} //11C0F4
        b.shape8C_=shape_.get(); //11C0F8 even NULL.
        pc=pc_=0x11c104;b.shape88_=5; //11C108 delay slot BEFORE malloc160.
        try{result_.reset(new Result);}catch(...){result_.reset();}resultAllocationReturned_=true;
        if(result_){const SpringVector4_48N vf0{{0,0,0,1}};result_->center20=vf0;
            for(auto& v:result_->normals30)v=vf0;for(auto& v:result_->points60)v=vf0;}
        pc=pc_=0x11c160;
        if(std::fegetround()!=FE_TONEAREST||!std::isfinite(radius))return false;
        volatile float squared=radius*radius;if(!std::isfinite(squared))return false;
        pc=pc_=0x11c164;if(!result_)return false;
        result_->capacity00=3;result_->count04=0;b.result90_=result_.get(); //164/168/16C
        pc=pc_=0x11c180;if(!shape_)return false;
        shape_->owner60=nullptr;
        auto& s=shape_->value;s.localCenter=position;shape_->known|=1; //11C18C
        s.radius50=radius;shape_->known|=2;s.radiusSquared54=squared;shape_->known|=4;
        s.radiusScale58=1;shape_->known|=8;s.centerScale5C=1;shape_->known|=16;
        identity(s.world);shape_->known|=32;shapeReturned_=true;return true;
    }
    bool setup2A04B0(const std::array<std::uint32_t,4>& position,const std::array<std::uint32_t,4>& box,
        const std::array<std::uint32_t,4>& look,const std::array<std::uint32_t,4>& up,
        std::uint32_t radiusBits,std::int32_t capacity,std::int32_t random,
        const std::array<std::uint32_t,4>& actualSentinel,std::uint32_t actualCategory,
        std::uint8_t& actualCollisionValid,std::uint32_t& pc)noexcept {
        if(!constructed_||setupAttempted_||nativeClosed_)return false;
        setupAttempted_=true;
        // Four actual stack vector copies precede no-op141C20. They are
        // consumed immediately; look/up affect only the unjoined box branch.
        SpringVector4_48N p,bx,lk,u;std::memcpy(&p,position.data(),16);std::memcpy(&bx,box.data(),16);
        std::memcpy(&lk,look.data(),16);std::memcpy(&u,up.data(),16);
        pc=pc_=0x2a0514;position30_=p;box40_=bx;radius1AC_=std::bit_cast<float>(radiusBits);
        pc=pc_=0x2a0524;random90_=static_cast<std::uint32_t>(random); //2A0528 delay slot.
        try{collider_.reset(new B);}catch(...){collider_.reset();}colliderAllocationReturned_=true;
        if(collider_){SpringVector4_48N sentinel;std::memcpy(&sentinel,actualSentinel.data(),16);
            const auto r=collider_->construct11B190(*application_,entity_.sessionGeneration,static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&collider_->entity_)),sentinel);
            if(r.status!=S::complete){pc=pc_=r.sourcePC;return false;}}
        source1B0_=collider_.get();pc=pc_=0x11b574;if(!collider_)return false; //2A053C then11B570.
        auto& cb=*collider_;
        cb.flagsF8_&=static_cast<std::uint8_t>(~8u);cb.flagsF8_|=2;cb.flagsF8_|=4;actualCollisionValid=0;
        pc=pc_=0x2a055c;cb.categoryA4_=actualCategory;
        pc=pc_=0x2a0564;
        if(!std::isfinite(*radius1AC_)||!(*radius1AC_>0.f))return false; //Box branch not implemented.
        if(!sphere11C0B0(p,*radius1AC_,pc))return false;
        cb.modeC8_=1; //124004 return delay reached by2A0704.
        cb.owner9C_=&entity_;cb.callbacksE4_[1].reset();callbackE8PC_=0x2a00a0; //2A0718/720. Typed code-PC lane supersedes base null pointer knowledge; no callback invocation.
        // Actual11F570 shape5 with source owner60 NULL ->11ED60(NULL).
        // Both11F3B0/B4 and11FCAC/B0 write identity. VU identity-transform
        // result is exact for the admitted finite, positive authored XYZ,W=1;
        // there is no general matrix/bounds projection or query readiness.
        pc=pc_=0x11f3a4;identity(shape_->value.world);
        cb.cachedCenter60_=shape_->value.localCenter;cb.radius80_=shape_->value.radius50; //11F404/410
        pc=pc_=0x11f430;
        for(unsigned k=0;k<3;++k)if(!std::isfinite(cb.cachedCenter60_.v[k])||!(cb.cachedCenter60_.v[k]>0.f))return false;
        if(std::bit_cast<std::uint32_t>(cb.cachedCenter60_.v[3])!=0x3f800000u)return false;
        //11F444 exact identity result already equals cached60.
        identity(shape_->value.world); //11F680 shape5 NULL, second copy.
        cb.cachedCenter60_=shape_->value.localCenter;cb.radius80_=shape_->value.radius50; //11FD00/0C
        refreshReturned_=true;
        C* nodes[]{&entity_,&cb.entity_};OriginalWreckComponentDomain50LA domain{entity_.sessionGeneration,nodes,2};
        pc=pc_=0x2a073c;const auto r=cb.append141960(domain,entity_,2);
        if(r.status!=S::complete){pc=pc_=r.sourcePC;return false;}
        if(capacity<=0)capacity=1;
        pc=pc_=0x2a0754;try{callbacks9C_.reset(new const void*[static_cast<std::size_t>(capacity)]);}catch(...){callbacks9C_.reset();}
        callbacksAllocationReturned_=true;source9C_=callbacks9C_.get(); //2A075C; elements unknown.
        pc=pc_=0x2a0760;try{argumentsA0_.reset(new const void*[static_cast<std::size_t>(capacity)]);}catch(...){argumentsA0_.reset();}
        argumentsAllocationReturned_=true;sourceA0_=argumentsA0_.get(); //2A0768; elements unknown.
        capacity94_=static_cast<std::uint32_t>(capacity);count98_=0; //2A076C/770
        setupReturned_=true;pc=pc_=0x2dc024;return true;
    }
    C* colliderEntity()noexcept{return collider_?&collider_->entity_:nullptr;}
    const C* colliderEntity()const noexcept{return collider_?&collider_->entity_:nullptr;}
    std::size_t countedEntities()const noexcept{return std::size_t(counted_)+(collider_&&collider_->entity_.reference00?1u:0u);}
    bool preflightNative()const noexcept {
        if(nativeClosed_)return !counted_&&!collider_&&!shape_&&!result_&&!callbacks9C_&&!argumentsA0_;
        if(!application_||application_->attachmentLock423F58())return false;
        if(counted_&&(entity_.reference00!=1||entity_.previous10||entity_.next14||entity_.parent1C||entity_.mask20||entity_.nativeFlags29))return false;
        if(!counted_&&entity_.reference00)return false;
        if(setupReturned_&&(!constructed_||!refreshReturned_||!capacity94_||*capacity94_!=1||!count98_||*count98_!=0||
            !callbacksAllocationReturned_||!argumentsAllocationReturned_))return false;
        if(source9C_&&*source9C_!=callbacks9C_.get())return false;
        if(sourceA0_&&*sourceA0_!=argumentsA0_.get())return false;
        if(source1B0_&&*source1B0_!=collider_.get())return false;
        if(!collider_)return !entity_.firstChild18&&!entity_.descendants24&&!shape_&&!result_;
        const auto& b=*collider_;const auto& c=b.entity_;
        if(c.reference00&&c.reference00!=1)return false;
        if(c.firstChild18||c.previous10||c.next14||c.nativeFlags29)return false;
        if(b.appended_){if(c.parent1C!=&entity_||entity_.firstChild18!=&c||c.mask20!=2||entity_.descendants24!=2)return false;}
        else if(c.parent1C||entity_.firstChild18||entity_.descendants24||c.mask20)return false;
        if(b.phase_==B::Phase::complete){
            if(!b.constructorNullListsCurrent()||!b.bodyA0_||*b.bodyA0_)return false;
            if(b.owner9C_&&*b.owner9C_&&*b.owner9C_!=&entity_)return false;
            for(unsigned i=0;i<4;++i){if(i==1&&callbackE8PC_){
                if(*callbackE8PC_!=0x2a00a0||b.callbacksE4_[i])return false;
            }else if(!b.callbacksE4_[i]||*b.callbacksE4_[i])return false;}
            if(b.shape8C_&&*b.shape8C_!=shape_.get())return false;
            //Result can be allocated but unpublished before11C16C.
            if(b.result90_&&*b.result90_&&*b.result90_!=result_.get())return false;
        }else if(shape_||result_||b.appended_)return false;
        return application_->objectCount423F60()>=countedEntities();
    }
    // Called only after all actual authority entries have been withdrawn by
    // the same World preflight. No fallible source destructor/dispatch claim.
    void closeNativeAfterAuthority()noexcept {
        source9C_.reset();sourceA0_.reset();source1B0_.reset();
        callbacks9C_.reset();argumentsA0_.reset();shape_.reset();result_.reset();
        if(collider_){auto& b=*collider_;if(b.appended_){entity_.firstChild18=nullptr;entity_.descendants24=0;}
            if(b.entity_.reference00){--b.entity_.reference00;--application_->objectCount423F60();}
            b.phase_=B::Phase::released;collider_.reset();}
        if(counted_){--entity_.reference00;--application_->objectCount423F60();counted_=false;}
        nativeClosed_=true;
    }
public:
    OriginalWorldTriggerInstance50LA()=default;
    OriginalWorldTriggerInstance50LA(const OriginalWorldTriggerInstance50LA&)=delete;
    OriginalWorldTriggerInstance50LA& operator=(const OriginalWorldTriggerInstance50LA&)=delete;
    ~OriginalWorldTriggerInstance50LA(){if(entity_.reference00||collider_)std::terminate();}
};
} //namespace rr::game
