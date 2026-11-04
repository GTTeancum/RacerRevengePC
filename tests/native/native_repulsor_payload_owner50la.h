#pragma once
#include "live_terrain_probe50fr_fixture.h"
#include "native_terrain_result_owner50la.h"
#include "../../src/game/original_racer_main_body_owner50la.h"
#include <cfenv>
#include <cmath>
#include <memory>

namespace ra_source_camera50LA {class OwnershipBridge;}
namespace native_repulsor_payload50LA {
//Unbanked native replacement of an ALREADY initialized full-native348 row.
//Not the counted source348 owner, not a Racer694 identity, not a loader return.
//Owns the real one-ray shape and result allocations; views never own a copy.
class Owner final {
    friend class ra_source_camera50LA::OwnershipBridge;
    using Payload=rr::game::OriginalWreckType2Owners50LA;
    using Vec=rr::game::SpringVector4_48N;
    using Ray=rr::game::Type2Ray49X;
    using Body=rr::game::OriginalRacerMainBodyOwner50LA;
    using Scene=rr::game::OriginalRacerSceneObject50LA;
    struct Leaf {std::optional<float> volume00;Ray endpoints10;};
    Payload payload_;
    std::unique_ptr<Leaf> leaf_;
    std::unique_ptr<Vec> midpoint_;
    const Body* body_=nullptr;
    const Scene* scene_=nullptr;
    std::uint64_t generation_=0;
    live_terrain_probe50fr::Owner* rowView_=nullptr;
    live_terrain_probe50fr::Owner* bankView_=nullptr;
    native_terrain_result50LA::Owner* resultView_=nullptr;
    std::uint16_t allocationMask_=0;
    const void* shapeHistory_=nullptr;
    const void* resultHistory_=nullptr;
    bool shapeReadable_=false,projected_=false,withdrawn_=false,retired_=false;
    void observe()noexcept {
        allocationMask_=payload_.freshLiveAllocationMask();
        shapeHistory_=payload_.shape8C_.get();resultHistory_=payload_.result90_.get();
    }
    Owner(const Body& body,live_terrain_probe50fr::Owner& row,
        live_terrain_probe50fr::Owner& bank,native_terrain_result50LA::Owner& result)noexcept
        :body_(&body),scene_(body.scene288()),generation_(body.entity().sessionGeneration),
         rowView_(&row),bankView_(&bank),resultView_(&result){}
    bool bodyCurrent()const noexcept {
        return body_&&scene_&&generation_&&body_->scene288()==scene_&&
            body_->entity().reference00==1u&&body_->entity().sessionGeneration==generation_&&
            scene_->constructed&&!scene_->released&&scene_->entity().reference00&&
            scene_->entity().sessionGeneration==generation_;
    }
    static bool withdrawnLive(const live_terrain_probe50fr::Owner& live)noexcept {
        return !live.initialized&&live.legacyWithdrawn&&!live.nativePayload&&
            !live.sourceShape8C&&!live.sourceWithdrawn&&!live.resource.rays&&!live.resource.nodes;
    }
    bool liveCurrent(const live_terrain_probe50fr::Owner& live)const noexcept {
        if(!projected_||!payload_.loaded_||retired_||withdrawn_||!payload_.shape8C_)return false;
        const auto resource=payload_.resource();
        return live.initialized&&live.legacyWithdrawn&&live.nativePayload==this&&
            !live.sourceShape8C&&!live.sourceWithdrawn&&live.resource.rays==resource.rays&&
            live.resource.rayCount==1u&&!live.resource.nodes&&!live.resource.nodeCount&&
            live.resource.root==rr::game::type2Leaf49X&&!live.resource.maxDepth&&
            !std::memcmp(live.resource.localMatrix.v,resource.localMatrix.v,sizeof(resource.localMatrix.v));
    }
    bool resultCurrent()const noexcept {
        return projected_&&resultView_&&resultView_->constructed_&&!resultView_->revoked_&&
            resultView_->legacyWithdrawn_&&!resultView_->sourceResult90_&&!resultView_->sourceWithdrawn_&&
            resultView_->nativePayload_==this&&resultView_->nativeResult90_==payload_.result90_.get();
    }
    bool current()const noexcept {
        if(retired_||withdrawn_||!bodyCurrent()||!rowView_||!bankView_||rowView_==bankView_||!resultView_||
           payload_.retired_||allocationMask_!=payload_.freshLiveAllocationMask()||
           shapeHistory_!=payload_.shape8C_.get()||resultHistory_!=payload_.result90_.get())return false;
        if(shapeReadable_&&(!payload_.shape8C_||payload_.shape8C_->racerModel44!=scene_||
           payload_.shape8C_->model44.value||payload_.shape8C_->count48!=1u||
           payload_.shape8C_->normalMode54!=1u))return false;
        if(projected_)return payload_.loaded_&&liveCurrent(*rowView_)&&liveCurrent(*bankView_)&&resultCurrent();
        return !payload_.loaded_&&withdrawnLive(*rowView_)&&withdrawnLive(*bankView_)&&
            !resultView_->constructed_&&!resultView_->revoked_&&resultView_->legacyWithdrawn_&&
            !resultView_->sourceResult90_&&!resultView_->sourceWithdrawn_&&
            !resultView_->nativeResult90_&&!resultView_->nativePayload_;
    }
    bool construct11C3F0(const Vec& first130,const Vec& second140,std::uint32_t& pc)noexcept {
        if(!current()||payload_.attempted_||first130.v[3]!=1.f||second140.v[3]!=1.f)return false;
        payload_.attempted_=true;
        const auto at=[&](std::uint32_t address){pc=payload_.boundary_=address;};
        //Old fixed legacy backing was withdrawn by the caller. This is NOT
        //a claimed11B600 release of a counted source8C/90 or unknown guest heap.
        try {
            at(0x11c42cu);payload_.shape8C_=std::make_unique<Payload::Shape>();
            //Actual ownership begins at11C434. Native row publication waits
            //for real return, but failed allocations remain retained here.
            at(0x11c44cu);leaf_=std::make_unique<Leaf>();
            const Vec unit{{0,0,0,1}};
            leaf_->endpoints10.first=leaf_->endpoints10.second=unit;
            auto& s=*payload_.shape8C_;
            s.matrix00={};for(unsigned i=0;i<4;++i)s.matrix00.v[5*i]=1.f;
            at(0x1130b0u);midpoint_=std::make_unique<Vec>();
            if(std::fegetround()!=FE_TONEAREST){observe();return false;}
            //Bounded count-one identity lowering, same original order and
            //known W words as counted348. No arbitrary VU/tree equivalence.
            const auto ordinary=[](float x){return std::isfinite(x)&&
                (x==0.f?!std::signbit(x):std::isnormal(x));};
            for(unsigned i=0;i<3;++i){
                const float a=first130.v[i],b=second140.v[i];
                if(!ordinary(a)||!ordinary(b)){observe();return false;}
                volatile float sum=a+b;volatile float half=.5f*sum;
                if(!ordinary(sum)||!ordinary(half)){observe();return false;}
                midpoint_->v[i]=half;
            }
            midpoint_->v[3]=1;leaf_->volume00=-1.f; //112D8C.
            leaf_->endpoints10.first=first130;leaf_->endpoints10.second=second140; //112D94/DA0.
            midpoint_.reset(); //113250.
            s.root40=rr::game::type2Leaf49X;s.nodeCount=0;s.maxDepth=0;
            s.racerModel44=scene_; //11C51C uses actual linked body128->scene288.
            s.normalMode54=1;s.count48=1;
            at(0x11c588u);s.rays50=std::make_unique<Ray[]>(1);
            s.rays50[0]=leaf_->endpoints10; //113880.
            leaf_.reset();shapeReadable_=true; //113290 before result.
            at(0x11c5b4u);payload_.result90_=std::make_unique<Payload::Result>();
            auto& r=*payload_.result90_;
            r.first00=r.second10=r.normal20=r.point30=r.correction40=unit;
            r.count5C=3;
            at(0x11c614u);r.words64=std::make_unique<std::uint32_t[]>(3);
            at(0x11c624u);r.flags68=std::make_unique<std::uint32_t[]>(3);
            at(0x11c634u);r.fractions6C=std::make_unique<float[]>(3);
            at(0x11c644u);r.tags70=std::make_unique<std::uint16_t[]>(3);
            at(0x11c65cu);r.pairs50=std::make_unique<Ray[]>(3);
            for(unsigned i=0;i<3;++i)r.pairs50[i].first=r.pairs50[i].second=unit;
            at(0x11c68cu);r.normals54=std::make_unique<Vec[]>(3);
            for(unsigned i=0;i<3;++i)r.normals54[i]=unit;
            at(0x11c6bcu);r.points58=std::make_unique<Vec[]>(3);
            for(unsigned i=0;i<3;++i)r.points58[i]=unit;
            payload_.loaded_=true;at(0x11c6e4u);
        }catch(...){observe();return false;} //Host refusal is not a guest NULL-return path.
        observe();
        const auto bind=[&](live_terrain_probe50fr::Owner& live){
            live.resource=payload_.resource();live.nativePayload=this;live.initialized=true;
        };
        bind(*rowView_);bind(*bankView_);
        resultView_->nativeResult90_=payload_.result90_.get();resultView_->nativePayload_=this;
        resultView_->constructed_=true;
        //No11DD00 reset, selected-hit knowledge, scalar-array or contact copy.
        projected_=true;return true;
    }
    void withdrawViews()noexcept {
        const auto withdraw=[](live_terrain_probe50fr::Owner& live){
            live.resource={};live.initialized=false;live.nativePayload=nullptr;live.legacyWithdrawn=true;
        };
        withdraw(*rowView_);withdraw(*bankView_);
        resultView_->nativeResult90_=nullptr;resultView_->nativePayload_=nullptr;
        resultView_->constructed_=false;resultView_->revoked_=true;
        withdrawn_=true;
    }
    void releaseWithdrawn()noexcept {
        if(!withdrawn_||retired_)std::terminate();
        //Every row/active/saved Bank borrow has been withdrawn before here.
        if(payload_.loaded_){if(!payload_.releaseShapeResult11B600())std::terminate();}
        else {
            //Explicit native tracked cancellation, not an invented source
            //destructor return through incomplete pointer/count fields.
            midpoint_.reset();leaf_.reset();
            if(payload_.shape8C_){payload_.shape8C_->nodes4C.reset();payload_.shape8C_->rays50.reset();}
            if(payload_.result90_){auto& r=*payload_.result90_;
                r.fractions6C.reset();r.pairs50.reset();r.tags70.reset();r.normals54.reset();
                r.flags68.reset();r.words64.reset();r.points58.reset();}
            payload_.shape8C_.reset();payload_.result90_.reset();payload_.retired_=true;
        }
        retired_=true;
    }
public:
    ~Owner()=default; //Payload refuses silently abandoning reached allocations.
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;
    static constexpr std::size_t allocationBudget()noexcept {
        //Conservative concrete peak including old fixed owners (already
        //charged separately), new payload, temporary leaf and midpoint.
        return sizeof(Owner)+sizeof(Payload::Shape)+sizeof(Payload::Result)+sizeof(Leaf)+sizeof(Vec)+
            sizeof(Ray)*4u+sizeof(Vec)*6u+sizeof(std::uint32_t)*6u+sizeof(float)*3u+sizeof(std::uint16_t)*3u;
    }
};
}
