#pragma once
#include "type2_resource49x.h"
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
        std::uint32_t normalMode44=0;
    };
    struct Result {
        SpringVector4_48N first00{},second10{},normal20{},point30{},correction40{};
        std::unique_ptr<Type2Ray49X[]> pairs50;
        std::unique_ptr<SpringVector4_48N[]> normals54,points58;
        std::uint32_t count5C=1;
        std::unique_ptr<std::uint32_t[]> words64,flags68;
        std::unique_ptr<float[]> fractions6C;
        std::unique_ptr<std::uint16_t[]> tags70;
        float selectedFraction74=0.f; //not constructor-initialized source state
    };
    std::unique_ptr<Shape> shape8C_;
    std::unique_ptr<Result> result90_;
    bool attempted_=false,loaded_=false,retired_=false;
    std::uint32_t boundary_=0;
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
    bool attempted()const noexcept{return attempted_;}
    bool loaded()const noexcept{return loaded_;}
    bool retired()const noexcept{return retired_;}
    std::uint32_t sourceBoundary()const noexcept{return boundary_;}
    const SpringVector4_48N* endpoints()const noexcept{return loaded_?shape8C_->endpoints08.get():nullptr;}
    const std::uint32_t* metadata()const noexcept{return loaded_?shape8C_->metadata04.get():nullptr;}
};
}
