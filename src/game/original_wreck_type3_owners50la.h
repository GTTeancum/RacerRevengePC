#pragma once
#include "type2_resource49x.h"
#include "original_camera_world_query.h"
#include <cstring>
#include <exception>
#include <memory>

namespace rr::game {
class OriginalWreckQueries50LA;
// Actual11C710(1,NULL,NULL) shape/result allocations. Inline query transaction
// state is separate; no scalar hit, length, radius or selected fraction is
// certified by fresh host backing. Only the original constructor vectors and
// counts below are source initialization claims.
class OriginalWreckType3Owners50LA final {
    friend class OriginalWreckQueries50LA;
    struct Shape {
        std::uint32_t count00=1;
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
        std::uint32_t count5C=1,hitCount60=0; //60 first becomes known at11DD00
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
    bool allocate11C710(const SpringVector4_48N& actualSentinel3E02C0)noexcept {
        if(attempted_||retired_)return false;
        attempted_=true;boundary_=0x11c740u;
        const SpringVector4_48N vf0{{0.f,0.f,0.f,1.f}};
        const auto vectors=[&](std::size_t n){auto p=std::make_unique<SpringVector4_48N[]>(n);
            for(std::size_t i=0;i<n;++i)p[i]=vf0;return p;};
        try {
            shape8C_=std::make_unique<Shape>();auto& s=*shape8C_;s.center20=vf0;s.center30=vf0;
            boundary_=0x11c774u;s.metadata04=std::make_unique<std::uint32_t[]>(2);
            boundary_=0x11c78cu;s.endpoints08=vectors(2);
            boundary_=0x11c7bcu;s.localStarts0C=vectors(1);
            boundary_=0x11c7ecu;s.localDirections10=vectors(1);
            boundary_=0x11c818u;s.lengths14=std::make_unique<float[]>(1);
            boundary_=0x11c82cu;s.worldStarts18=vectors(1);
            boundary_=0x11c85cu;s.worldDirections1C=vectors(1);
            s.normalMode44=0; //11C898, not global3DF5B0
            for(std::size_t i=0;i<2;++i){s.metadata04[i]=0;s.endpoints08[i]=actualSentinel3E02C0;}
            boundary_=0x11c934u;result90_=std::make_unique<Result>();auto& r=*result90_;
            r.first00=vf0;r.second10=vf0;r.normal20=vf0;r.point30=vf0;r.correction40=vf0;
            boundary_=0x11c98cu;r.words64=std::make_unique<std::uint32_t[]>(1);
            boundary_=0x11c99cu;r.flags68=std::make_unique<std::uint32_t[]>(1);
            boundary_=0x11c9acu;r.fractions6C=std::make_unique<float[]>(1);
            boundary_=0x11c9bcu;r.tags70=std::make_unique<std::uint16_t[]>(1);
            boundary_=0x11c9d4u;r.pairs50=std::make_unique<Type2Ray49X[]>(1);
            r.pairs50[0].first=vf0;r.pairs50[0].second=vf0;
            boundary_=0x11ca04u;r.normals54=vectors(1);
            boundary_=0x11ca34u;r.points58=vectors(1);
        }catch(...){return false;}
        loaded_=true;boundary_=0x11ca5cu;return true;
    }
    bool canRelease11B600()const noexcept {
        // Original0x50 constructor initializes20/30, not every pointer. A
        // partial allocation cannot borrow host zeroes as source NULL fields.
        return attempted_&&!retired_&&(loaded_||(!shape8C_&&!result90_));
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
public:
    OriginalWreckType3Owners50LA()=default;
    OriginalWreckType3Owners50LA(const OriginalWreckType3Owners50LA&)=delete;
    OriginalWreckType3Owners50LA& operator=(const OriginalWreckType3Owners50LA&)=delete;
    ~OriginalWreckType3Owners50LA(){if(shape8C_||result90_)std::terminate();}
    bool producerPrepared()const noexcept{return preparedByProducer_;}
    bool attempted()const noexcept{return attempted_;}
    bool loaded()const noexcept{return loaded_;}
    bool retired()const noexcept{return retired_;}
    std::uint32_t sourceBoundary()const noexcept{return boundary_;}
    const SpringVector4_48N* endpoints()const noexcept{return loaded_?shape8C_->endpoints08.get():nullptr;}
    const std::uint32_t* metadata()const noexcept{return loaded_?shape8C_->metadata04.get():nullptr;}
};
}
