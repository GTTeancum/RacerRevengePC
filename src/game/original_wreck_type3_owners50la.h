#pragma once
#include "type2_resource49x.h"
#include "original_wreck_components50la.h"
#include <array>
#include "original_camera_world_query.h"
#include <cstring>
#include <exception>
#include <memory>
#include <optional>

namespace rr::game {
class OriginalWreckQueries50LA;
class OriginalRacerQuery7A4Owner50LA;
// Actual11C710(1,NULL,NULL) shape/result allocations. Inline query transaction
// state is separate; no scalar hit, length, radius or selected fraction is
// certified by fresh host backing. Only the original constructor vectors and
// counts below are source initialization claims.
class OriginalWreckType3Owners50LA final {
    friend class OriginalWreckQueries50LA;
    friend class OriginalRacerQuery7A4Owner50LA;
    friend class OriginalSourcePhysicsCollision50LA;
    struct Shape {
        std::uint32_t count00=1; //Native backing only until sourceCount00Known.
        bool sourceCount00Known=false,sourceNormalMode44Known=false,sourceVectors20_30Known=false;
        std::uint8_t sourcePointerMask=0; //04/08/0C/10/14/18/1C, NOT payload-content validity.
        std::unique_ptr<std::uint32_t[]> metadata04;
        std::unique_ptr<SpringVector4_48N[]> endpoints08,localStarts0C,localDirections10;
        std::unique_ptr<float[]> lengths14;
        std::unique_ptr<SpringVector4_48N[]> worldStarts18,worldDirections1C;
        SpringVector4_48N center20{},center30{};
        float radius40=0.f; // produced by11DD00, not constructor initialized
        std::uint32_t normalMode44=0;
    };
    struct Result {
        SpringVector4_48N first00{},second10{},normal20{},point30{},correction40{};
        std::unique_ptr<Type2Ray49X[]> pairs50;
        std::unique_ptr<SpringVector4_48N[]> normals54,points58;
        std::uint32_t count5C=1,hitCount60=0; //Backing only;60 first becomes known at11DD00.
        bool sourceCount5CKnown=false,sourceVectors00_40Known=false;
        std::uint8_t sourcePointerMask=0; //64/68/6C/70/50/54/58 pointer stores only.
        std::unique_ptr<std::uint32_t[]> words64,flags68;
        std::unique_ptr<float[]> fractions6C;
        std::unique_ptr<std::uint16_t[]> tags70;
        float selectedFraction74=0.f; //not constructor-initialized source state
    };
    std::unique_ptr<Shape> shape8C_;
    std::unique_ptr<Result> result90_;
    bool attempted_=false,loaded_=false,retired_=false;
    std::uint32_t boundary_=0;
    bool preparedByProducer_=false,hitsProduced_=false,fractionsProduced_=false;
    enum class FreshWreckRelease {none,sourceFull11B600,nativeIncompleteCancellation};
    bool freshWreck_=false;
    const OriginalWreckComponent50LA* freshE8_=nullptr;
    const SpringVector4_48N* freshSentinel_=nullptr;
    const std::optional<const void*>* freshField8C_=nullptr;
    const std::optional<const void*>* freshField90_=nullptr;
    const std::uint32_t* freshField88_=nullptr;
    const Shape* freshShapeHistory_=nullptr;
    const Result* freshResultHistory_=nullptr;
    std::array<const void*,14> freshArrayAddresses_{};
    std::uint32_t freshStoppedPC_=0;
    FreshWreckRelease freshRelease_=FreshWreckRelease::none;
    std::array<const void*,14> freshArrayAddresses()const noexcept {
        std::array<const void*,14> a{};
        if(shape8C_){const auto& s=*shape8C_;
            a[0]=s.metadata04.get();a[1]=s.endpoints08.get();a[2]=s.localStarts0C.get();
            a[3]=s.localDirections10.get();a[4]=s.lengths14.get();a[5]=s.worldStarts18.get();a[6]=s.worldDirections1C.get();}
        if(result90_){const auto& r=*result90_;
            a[7]=r.words64.get();a[8]=r.flags68.get();a[9]=r.fractions6C.get();a[10]=r.tags70.get();
            a[11]=r.pairs50.get();a[12]=r.normals54.get();a[13]=r.points58.get();}
        return a;
    }
    bool allocateFreshWreck11C710(const SpringVector4_48N& sentinel,
        std::optional<const void*>& actual8C,std::uint32_t& actual88,
        std::optional<const void*>& actual90,const OriginalWreckComponent50LA& e8)noexcept {
        using K=OriginalWreckComponentKind50LA;
        if(freshWreck_||attempted_||retired_||!actual8C||*actual8C||!actual90||*actual90||
           &actual8C==&actual90||actual88||e8.reference00!=1u||e8.kind!=K::collision11B190||
           !e8.parent1C||!e8.previous10||e8.next14||e8.firstChild18||
           e8.parent1C->reference00!=1u||e8.parent1C->kind!=K::piece2CB4B0||
           e8.previous10->reference00!=1u||e8.previous10->kind!=K::collision11B190||
           e8.previous10->parent1C!=e8.parent1C||e8.previous10->next14!=&e8||
           e8.parent1C->sessionGeneration!=e8.sessionGeneration||e8.previous10->sessionGeneration!=e8.sessionGeneration)return false;
        freshWreck_=true;freshE8_=&e8;freshSentinel_=&sentinel;
        freshField8C_=&actual8C;freshField88_=&actual88;freshField90_=&actual90;
        //Actual11B600 receives genuine constructorNULL aliases. The existing
        //allocator publishes8C/88 and90 at their own source stores only.
        const bool done=allocate11C710(sentinel,&actual8C,&actual88,&actual90);
        freshStoppedPC_=boundary_;freshShapeHistory_=shape8C_.get();freshResultHistory_=result90_.get();
        freshArrayAddresses_=freshArrayAddresses();return done;
    }
    bool freshWreckCutCurrent(const OriginalWreckComponent50LA& e8,
        const std::optional<const void*>& actual8C,const std::uint32_t& actual88,
        const std::optional<const void*>& actual90,std::uint32_t references,
        const OriginalWreckComponent50LA* parent,const OriginalWreckComponent50LA* previous)const noexcept {
        using K=OriginalWreckComponentKind50LA;
        if(!freshWreck_||!attempted_||retired_||freshRelease_!=FreshWreckRelease::none||!freshSentinel_||
           freshE8_!=&e8||freshField8C_!=&actual8C||freshField88_!=&actual88||freshField90_!=&actual90||
           !actual8C||!actual90||*actual8C!=shape8C_.get()||*actual90!=(loaded_?result90_.get():nullptr)||
           actual88!=(shape8C_?3u:0u)||e8.reference00!=references||e8.kind!=K::collision11B190||
           e8.parent1C!=parent||e8.previous10!=previous||e8.next14||e8.firstChild18||bool(parent)!=bool(previous)||
           preparedByProducer_||hitsProduced_||fractionsProduced_||freshStoppedPC_!=boundary_||
           freshShapeHistory_!=shape8C_.get()||freshResultHistory_!=result90_.get()||
           freshArrayAddresses_!=freshArrayAddresses())return false;
        if(parent&&(parent->reference00!=1u||parent->kind!=K::piece2CB4B0||
           parent->sessionGeneration!=e8.sessionGeneration||previous->reference00!=1u||
           previous->kind!=K::collision11B190||previous->parent1C!=parent||
           previous->sessionGeneration!=e8.sessionGeneration||previous->next14!=(references?&e8:nullptr)))return false;
        unsigned shapeMask=0,resultMask=0;bool shapeExpected=true,resultExpected=false,normalKnown=false;
        switch(boundary_){
        case 0x11c740u:shapeExpected=false;break;
        case 0x11c774u:break;
        case 0x11c78cu:shapeMask=1u;break;
        case 0x11c7bcu:shapeMask=3u;break;
        case 0x11c7ecu:shapeMask=7u;break;
        case 0x11c818u:shapeMask=15u;break;
        case 0x11c82cu:shapeMask=31u;break;
        case 0x11c85cu:shapeMask=63u;break;
        case 0x11c934u:shapeMask=127u;normalKnown=true;break;
        case 0x11c98cu:shapeMask=127u;normalKnown=true;resultExpected=true;break;
        case 0x11c99cu:shapeMask=127u;normalKnown=true;resultExpected=true;resultMask=1u;break;
        case 0x11c9acu:shapeMask=127u;normalKnown=true;resultExpected=true;resultMask=3u;break;
        case 0x11c9bcu:shapeMask=127u;normalKnown=true;resultExpected=true;resultMask=7u;break;
        case 0x11c9d4u:shapeMask=127u;normalKnown=true;resultExpected=true;resultMask=15u;break;
        case 0x11ca04u:shapeMask=127u;normalKnown=true;resultExpected=true;resultMask=31u;break;
        case 0x11ca34u:shapeMask=127u;normalKnown=true;resultExpected=true;resultMask=63u;break;
        case 0x11ca5cu:shapeMask=127u;normalKnown=true;resultExpected=true;resultMask=127u;break;
        default:return false;
        }
        if(bool(shape8C_)!=shapeExpected||bool(result90_)!=resultExpected||loaded_!=(boundary_==0x11ca5cu))return false;
        if(shape8C_){const auto& s=*shape8C_;
            if(!s.sourceVectors20_30Known||!s.sourceCount00Known||s.count00!=1u||s.sourcePointerMask!=shapeMask||
               s.sourceNormalMode44Known!=normalKnown||(normalKnown&&s.normalMode44))return false;
        }
        if(result90_){const auto& r=*result90_;
            if(!r.sourceVectors00_40Known||!r.sourceCount5CKnown||r.count5C!=1u||r.sourcePointerMask!=resultMask)return false;
        }
        for(unsigned i=0;i<7u;++i){
            if(bool(freshArrayAddresses_[i])!=bool(shapeMask&(1u<<i))||
               bool(freshArrayAddresses_[7u+i])!=bool(resultMask&(1u<<i)))return false;
        }
        return true;
    }
    bool preflightFreshWreckPayload(const OriginalWreckComponent50LA& e8,
        const std::optional<const void*>& actual8C,const std::uint32_t& actual88,
        const std::optional<const void*>& actual90,const OriginalWreckComponent50LA* parent=nullptr,
        const OriginalWreckComponent50LA* previous=nullptr)const noexcept {
        return freshWreckCutCurrent(e8,actual8C,actual88,actual90,1u,parent,previous);
    }
    bool releaseFreshWreckPayloadAfterReferenceDrop(const OriginalWreckComponent50LA& e8,
        std::optional<const void*>& actual8C,const std::uint32_t& actual88,
        std::optional<const void*>& actual90,const OriginalWreckComponent50LA* parent=nullptr,
        const OriginalWreckComponent50LA* previous=nullptr)noexcept {
        if(!freshWreckCutCurrent(e8,actual8C,actual88,actual90,0u,parent,previous))return false;
        if(loaded_){
            //Genuine published90:11B734 really clears selectedFraction74 after
            //the6C free/check. Existing full destructor follows actual order.
            if(!release11B600())std::terminate();
            actual8C=nullptr;actual90=nullptr;freshRelease_=FreshWreckRelease::sourceFull11B600;
        }else{
            //Unknown guest pointer lanes are NOT nativeNULL source fields.
            //Unpublished result90 is only native staging: no source74 store.
            if(shape8C_){auto& s=*shape8C_;
                s.metadata04.reset();s.endpoints08.reset();s.localStarts0C.reset();s.localDirections10.reset();
                s.lengths14.reset();s.worldStarts18.reset();s.worldDirections1C.reset();}
            if(result90_){auto& r=*result90_;
                r.fractions6C.reset();r.pairs50.reset();r.tags70.reset();r.normals54.reset();
                r.flags68.reset();r.words64.reset();r.points58.reset();}
            shape8C_.reset();result90_.reset();loaded_=false;retired_=true;
            freshRelease_=FreshWreckRelease::nativeIncompleteCancellation;
            //Retain actual8C/88/90 and stoppedPC as history through withdrawal.
        }
        return true;
    }
    FreshWreckRelease freshWreckReleaseKind()const noexcept{return freshRelease_;}

    // Inline numeric records are transaction staging; these arrays remain the
    //actual persistent source owner. Unwritten scalar backing is never read.
    bool readProducerState(Type3QueryState48P& state,OriginalCameraSingleRay& ray)const noexcept {
        if(!loaded_||retired_)return false;
        const auto& s=*shape8C_;const auto& r=*result90_;
        std::memcpy(state.localCenter.v,s.center20.v,sizeof(Vector3_48C));
        state.worldCenter=s.center30;
        state.result00=r.first00;state.result10=r.second10;
        state.selectedNormal=r.normal20;state.selectedPoint=r.point30;state.selectedCorrection=r.correction40;
        std::memcpy(ray.localStart.v,s.localStarts0C[0].v,sizeof(Vector3_48C));
        std::memcpy(ray.localDirection.v,s.localDirections10[0].v,sizeof(Vector3_48C));
        ray.worldStart=s.worldStarts18[0];ray.worldDirection=s.worldDirections1C[0];
        ray.normal=r.normals54[0];ray.point=r.points58[0];
        if(preparedByProducer_){state.radius=s.radius40;state.hitCount=r.hitCount60;
            state.selectedFraction=r.selectedFraction74;ray.length=s.lengths14[0];}
        if(hitsProduced_)ray.hit=r.flags68[0];
        if(fractionsProduced_)ray.fraction=r.fractions6C[0];
        return true;
    }
    bool commitProducerState(const Type3QueryState48P& state,const OriginalCameraSingleRay& ray,
        const OriginalCameraWorldResult& receipt)noexcept {
        if(!loaded_||retired_||receipt.status!=OriginalCameraWorldStatus::Complete)return false;
        if(!receipt.resetGlobalsWritten)return true; //disabled query did not reach11DD00
        auto& s=*shape8C_;auto& r=*result90_;
        // Native candidate-local scratch represents XYZ only. Do not invent
        // original unused W/register-carry writes in center20/0C/10.
        std::memcpy(s.center20.v,state.localCenter.v,sizeof(Vector3_48C));
        std::memcpy(s.localStarts0C[0].v,ray.localStart.v,sizeof(Vector3_48C));
        std::memcpy(s.localDirections10[0].v,ray.localDirection.v,sizeof(Vector3_48C));
        s.center30=state.worldCenter;s.radius40=state.radius;
        s.worldStarts18[0]=ray.worldStart;s.worldDirections1C[0]=ray.worldDirection;s.lengths14[0]=ray.length;
        r.first00=state.result00;r.second10=state.result10;r.normal20=state.selectedNormal;
        r.point30=state.selectedPoint;r.correction40=state.selectedCorrection;
        r.normals54[0]=ray.normal;r.points58[0]=ray.point;
        r.hitCount60=state.hitCount;r.selectedFraction74=state.selectedFraction;preparedByProducer_=true;
        // Every reached126F40 dispatch clears68, even if its inner coarse test
        // rejects.6C is written only after that coarse test admits the pair.
        if(receipt.normalModeWritten){r.flags68[0]=ray.hit;hitsProduced_=true;}
        if(receipt.type3FractionsWritten){r.fractions6C[0]=ray.fraction;fractionsProduced_=true;}
        //50/64/70 and opaque78/7C are not produced by this route.
        return true;
    }
    bool allocate11C710(const SpringVector4_48N& actualSentinel3E02C0,
        std::optional<const void*>* actualShape8C=nullptr,std::uint32_t* actualShape88=nullptr,
        std::optional<const void*>* actualResult90=nullptr)noexcept {
        if(attempted_||retired_)return false;
        if(actualShape8C||actualShape88||actualResult90){
            if(!actualShape8C||!actualShape88||!actualResult90||!actualShape8C->has_value()||
               **actualShape8C||!actualResult90->has_value()||**actualResult90)return false;
        }
        attempted_=true;boundary_=0x11c740u;
        const SpringVector4_48N vf0{{0.f,0.f,0.f,1.f}};
        const auto vectors=[&](std::size_t n){auto p=std::make_unique<SpringVector4_48N[]>(n);
            for(std::size_t i=0;i<n;++i)p[i]=vf0;return p;};
        try {
            shape8C_=std::make_unique<Shape>();auto& s=*shape8C_;s.center20=vf0;s.center30=vf0;
            s.sourceVectors20_30Known=true; //Actual11C754/75C constructor stores, before8C publication.
            //Actual publication before the first subordinate allocation. The
            //source wrapper retains this prefix if any later allocation fails.
            if(actualShape8C)*actualShape8C=shape8C_.get(); //11C760
            if(actualShape88)*actualShape88=3u; //11C768
            s.count00=1u;s.sourceCount00Known=true; //11C778, actual caller a1=1.
            boundary_=0x11c774u;s.metadata04=std::make_unique<std::uint32_t[]>(2);s.sourcePointerMask|=1u;
            boundary_=0x11c78cu;s.endpoints08=vectors(2);s.sourcePointerMask|=2u;
            boundary_=0x11c7bcu;s.localStarts0C=vectors(1);s.sourcePointerMask|=4u;
            boundary_=0x11c7ecu;s.localDirections10=vectors(1);s.sourcePointerMask|=8u;
            boundary_=0x11c818u;s.lengths14=std::make_unique<float[]>(1);s.sourcePointerMask|=16u;
            boundary_=0x11c82cu;s.worldStarts18=vectors(1);s.sourcePointerMask|=32u;
            boundary_=0x11c85cu;s.worldDirections1C=vectors(1);s.sourcePointerMask|=64u;
            s.normalMode44=0;s.sourceNormalMode44Known=true; //11C898, not global3DF5B0
            for(std::size_t i=0;i<2;++i){s.metadata04[i]=0;s.endpoints08[i]=actualSentinel3E02C0;}
            boundary_=0x11c934u;result90_=std::make_unique<Result>();auto& r=*result90_;
            r.first00=vf0;r.second10=vf0;r.normal20=vf0;r.point30=vf0;r.correction40=vf0;
            r.sourceVectors00_40Known=true;
            r.count5C=1u;r.sourceCount5CKnown=true; //11C980 AFTER vector constructors, before result arrays.
            boundary_=0x11c98cu;r.words64=std::make_unique<std::uint32_t[]>(1);r.sourcePointerMask|=1u;
            boundary_=0x11c99cu;r.flags68=std::make_unique<std::uint32_t[]>(1);r.sourcePointerMask|=2u;
            boundary_=0x11c9acu;r.fractions6C=std::make_unique<float[]>(1);r.sourcePointerMask|=4u;
            boundary_=0x11c9bcu;r.tags70=std::make_unique<std::uint16_t[]>(1);r.sourcePointerMask|=8u;
            boundary_=0x11c9d4u;r.pairs50=std::make_unique<Type2Ray49X[]>(1);
            r.pairs50[0].first=vf0;r.pairs50[0].second=vf0;r.sourcePointerMask|=16u;
            boundary_=0x11ca04u;r.normals54=vectors(1);r.sourcePointerMask|=32u;
            boundary_=0x11ca34u;r.points58=vectors(1);r.sourcePointerMask|=64u;
        }catch(...){return false;}
        if(actualResult90)*actualResult90=result90_.get(); //11CA5C only AFTER complete result arrays.
        loaded_=true;boundary_=0x11ca5cu;return true;
    }
    bool canRelease11B600()const noexcept {
        // Original0x50 constructor initializes20/30, not every pointer. A
        // partial allocation cannot borrow host zeroes as source NULL fields.
        return !sourceShape_.attempted&&attempted_&&!retired_&&(loaded_||(!shape8C_&&!result90_));
    }
    bool release11B600()noexcept {
        if(!canRelease11B600())return false;
        if(loaded_){
            auto& s=*shape8C_;s.metadata04.reset();s.endpoints08.reset();s.localStarts0C.reset();
            s.localDirections10.reset();s.lengths14.reset();s.worldStarts18.reset();s.worldDirections1C.reset();
            auto& r=*result90_;r.fractions6C.reset();r.selectedFraction74=0.f;
            r.pairs50.reset();r.tags70.reset();r.normals54.reset();r.flags68.reset();r.words64.reset();r.points58.reset();
            shape8C_.reset();result90_.reset();
        }
        loaded_=false;retired_=true;boundary_=0x11b904u;return true;
    }
#include "original_source_collision_type3_shape50la.inl"
#include "original_source_collision_type3_result50la.inl"
public:
    OriginalWreckType3Owners50LA()=default;
    OriginalWreckType3Owners50LA(const OriginalWreckType3Owners50LA&)=delete;
    OriginalWreckType3Owners50LA& operator=(const OriginalWreckType3Owners50LA&)=delete;
    ~OriginalWreckType3Owners50LA(){if(shape8C_||result90_||(sourceShape_.attempted&&!sourceShape_.nativeWithdrawn))std::terminate();}
    bool producerPrepared()const noexcept{return preparedByProducer_;}
    bool attempted()const noexcept{return attempted_;}
    bool loaded()const noexcept{return loaded_;}
    bool retired()const noexcept{return retired_;}
    std::uint32_t sourceBoundary()const noexcept{return boundary_;}
    const SpringVector4_48N* endpoints()const noexcept{return loaded_?shape8C_->endpoints08.get():nullptr;}
    const std::uint32_t* metadata()const noexcept{return loaded_?shape8C_->metadata04.get():nullptr;}
};
}
