#pragma once
#include "ra_cold_sources50la.h"
#include "../../src/game/original_wreck_components50la.h"
#include <array>
#include <memory>
#include <vector>
#include <thread>
namespace ra_pod_scene50LA {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {
// World3D4's actual PhysicsObject model index. This bounded RA route proves
// every authored StaticType resolves Standard; it is not an empty-world claim.
class OriginalWorldPhysicsIndex50LA final {
    friend class ::ra_pod_scene50LA::Owner;
    friend class ::ra_cold_aggregate50LA::Owner;
    //Actual27C010 lanes, not collision Storage or an inferred empty world.
    std::int32_t count00_=0;
    std::unique_ptr<std::uint32_t[]> capacity04_,used08_;
    using Model=const OriginalWreckComponent50LA*;
    std::unique_ptr<Model*[]> models0C_; //per-zone pointer-array ownership, null on admitted load.
    std::shared_ptr<const ra_cold_sources50LA::Owner> source_;
    std::size_t initializedZones_=0;
    std::uint32_t initializeIndexV1_=0,initializeOffsetA0_=0;
    bool constructed_=true,prepared_=false,closed_=false,prepareAttempted_=false;
    std::uint32_t preparePC_=0x27c010;
    struct Reset27C330 {
        bool attempted=false,returned=false;
        std::uint32_t pc=0x27c330,indexS0=0,offsetS3=0;
        Model* reachedZoneV1=nullptr;
    } reset_;
    OriginalWorldPhysicsIndex50LA()noexcept=default; //27C010: all four lanes zero/null.
    struct SourceTag {};
    bool sourceMode_=false,sourcePrepareAttempted_=false,sourcePrepared_=false;
    const OriginalWreckComponent50LA* sourceWorld_=nullptr;
    std::uint64_t sourceWorldGeneration_=0;
    std::thread::id sourceThread_;
    std::size_t sourceModelsProduced_=0,sourceCapacityProduced_=0,sourceUsedProduced_=0;
    std::uint32_t sourcePC_=0x27c010;
    OriginalWorldPhysicsIndex50LA(SourceTag,const OriginalWreckComponent50LA& world)noexcept
        :constructed_(false),sourceMode_(true),sourceWorld_(&world),
         sourceWorldGeneration_(world.sessionGeneration),sourceThread_(std::this_thread::get_id()) {
        sourcePC_=0x27c010;models0C_.reset();
        sourcePC_=0x27c014;capacity04_.reset();
        sourcePC_=0x27c018;used08_.reset();
        sourcePC_=0x27c01c;count00_=0;
        constructed_=true;sourcePC_=0x27c020;
    }
    bool prepareSource27C110(const OriginalWreckComponent50LA& world,
        std::int32_t zones,bool& returned,std::uint32_t& pc)noexcept {
        returned=false;pc=0;
        if(!sourceMode_||!constructed_||closed_||sourcePrepareAttempted_||prepared_||source_||
           sourceWorld_!=&world||sourceWorldGeneration_!=world.sessionGeneration||!world.reference00||
           sourceThread_!=std::this_thread::get_id()||zones!=10||count00_||capacity04_||used08_||models0C_)
            return false; //Private fresh-source admission, not original returned0.
        sourcePrepareAttempted_=true;
        pc=sourcePC_=0x27c124; //Actual positive-count branch reads constructorNULL04.
        pc=sourcePC_=0x27c138;count00_=zones;
        pc=sourcePC_=0x27c140;
        try{capacity04_.reset(new std::uint32_t[count00_]);}catch(...){capacity04_.reset();}
        pc=sourcePC_=0x27c148; //Actual04 publication; continue after NULL.
        pc=sourcePC_=0x27c150;
        try{used08_.reset(new std::uint32_t[count00_]);}catch(...){used08_.reset();}
        pc=sourcePC_=0x27c158;
        pc=sourcePC_=0x27c160;
        try{models0C_.reset(new Model*[count00_]);}catch(...){models0C_.reset();}
        pc=sourcePC_=0x27c168;
        pc=sourcePC_=0x27c16c;initializeIndexV1_=0;
        pc=sourcePC_=0x27c174;initializeOffsetA0_=0;
        for(std::int32_t i=0;i<count00_;++i){
            pc=sourcePC_=0x27c17c;++initializeIndexV1_;
            pc=sourcePC_=0x27c184;if(!models0C_)return false;
            models0C_[i]=nullptr;++sourceModelsProduced_;
            pc=sourcePC_=0x27c190;if(!capacity04_)return false;
            capacity04_[i]=0;++sourceCapacityProduced_;
            pc=sourcePC_=0x27c19c;initializeOffsetA0_+=4;
            pc=sourcePC_=0x27c1a0;if(!used08_)return false;
            used08_[i]=0;++sourceUsedProduced_;++initializedZones_;
        }
        //Different private source completion, NOT legacy decoded Standard-only readiness.
        sourcePrepared_=true;returned=true;pc=sourcePC_=0x27c1c4;return true;
    }
    bool canReleaseSourceNative()const noexcept {
        if(!sourceMode_||!constructed_||sourceThread_!=std::this_thread::get_id()||
           !sourceWorld_||sourceWorldGeneration_!=sourceWorld_->sessionGeneration||!sourceWorld_->reference00||
           prepared_||source_||prepareAttempted_||reset_.attempted||
           (count00_!=0&&count00_!=10)||initializedZones_!=sourceUsedProduced_||
           sourceModelsProduced_>10||sourceCapacityProduced_>sourceModelsProduced_||
           sourceUsedProduced_>sourceCapacityProduced_||
           sourceModelsProduced_-sourceCapacityProduced_>1||sourceCapacityProduced_-sourceUsedProduced_>1||
           (sourceModelsProduced_&&!models0C_)||(sourceCapacityProduced_&&!capacity04_)||
           (sourceUsedProduced_&&!used08_))return false;
        if(sourcePrepared_&&(sourceModelsProduced_!=10||sourceCapacityProduced_!=10||sourceUsedProduced_!=10))return false;
        //Read ONLY reached slots. No27C1D0 per-zone/model reference producer is
        //admitted, so all other allocated slots remain unknown, not zero.
        for(std::size_t i=0;i<sourceModelsProduced_;++i)if(models0C_[i])return false;
        for(std::size_t i=0;i<sourceCapacityProduced_;++i)if(capacity04_[i])return false;
        for(std::size_t i=0;i<sourceUsedProduced_;++i)if(used08_[i])return false;
        return true;
    }

    static bool standardOnlySource(const ra_cold_sources50LA::Owner& source)noexcept {
        if(!source.sourcesDecoded())return false;
        const auto& world=source.sourcesDecoded()->world;
        if(world.zones.size()!=10||world.instances.size()!=1261)return false;
        std::array<std::uint32_t,10> ordinals{};
        for(std::size_t i=0;i<world.zones.size();++i)
            if(world.zones[i].zone!=i+1||!world.zones[i].models||world.zones[i].models>1261)return false;
        for(const auto& instance:world.instances){
            if(!instance.zone||instance.zone>10||instance.parameter_block>=world.parameters.blocks.size())return false;
            const auto z=instance.zone-1;
            if(instance.ordinal!=++ordinals[z]||instance.ordinal>world.zones[z].models)return false;
            const auto& block=world.parameters.blocks[instance.parameter_block];
            if(block.name!=instance.section)return false;
            bool seen=false;
            for(const auto& field:block.fields)if(field.name=="statictype"){
                if(seen||field.value!="Standard")return false;
                seen=true;
            }
            //Absent type defaults Standard27B834; only type2 reaches27C1D0.
        }
        for(std::size_t i=0;i<ordinals.size();++i)if(ordinals[i]!=world.zones[i].models)return false;
        return true;
    }
    bool prepare27C110(std::shared_ptr<const ra_cold_sources50LA::Owner> source)noexcept {
        if(sourceMode_||prepareAttempted_||closed_)return false;
        prepareAttempted_=true;preparePC_=0x27c110;
        if(!source||!standardOnlySource(*source))return false; //Immutable native source custody.
        source_=std::move(source);
        count00_=static_cast<std::int32_t>(source_->sourcesDecoded()->world.zones.size()); //27C138.
        //All three allocation calls occur before the first dereference, even
        //when an earlier allocation returned null. Keep published prefixes.
        try{capacity04_.reset(new std::uint32_t[count00_]);}catch(...){capacity04_.reset();}
        preparePC_=0x27c148;
        try{used08_.reset(new std::uint32_t[count00_]);}catch(...){used08_.reset();}
        preparePC_=0x27c158;
        try{models0C_.reset(new Model*[count00_]);}catch(...){models0C_.reset();}
        preparePC_=0x27c168;
        initializeIndexV1_=0;initializeOffsetA0_=0; //27C16C/174.
        for(std::int32_t i=0;i<count00_;++i){
            ++initializeIndexV1_; //27C17C, BEFORE first zero store.
            preparePC_=0x27c184;if(!models0C_)return false;
            models0C_[i]=nullptr;
            preparePC_=0x27c190;if(!capacity04_)return false;
            capacity04_[i]=0;
            initializeOffsetA0_+=4; //27C19C, BEFORE used08 zero store.
            preparePC_=0x27c1a0;if(!used08_)return false;
            used08_[i]=0;++initializedZones_;
        }
        prepared_=true;preparePC_=0x27c1c4;return true;
    }
    bool currentForSource(const ra_cold_sources50LA::Owner& source)const noexcept {
        return !sourceMode_&&constructed_&&prepared_&&!closed_&&source_.get()==&source&&count00_==10&&
            initializedZones_==10&&capacity04_&&used08_&&models0C_;
    }
    bool reset27C330(std::uint32_t& pc)noexcept {
        if(sourceMode_){pc=0x27c330;return false;} //No later reset/readiness grant.
        auto& r=reset_;if(r.attempted){pc=r.pc;return false;}
        r.attempted=true;pc=r.pc=0x27c34c;
        if(!constructed_||!prepared_||closed_)return false;
        //A prepared positive10 owner has actual outer0C storage, never a
        //fabricated source-null shortcut. Source count is read per iteration.
        if(!models0C_)return false;
        r.indexS0=0;r.offsetS3=0; //27C358/360.
        for(;;){
            pc=r.pc=0x27c400;
            if(static_cast<std::int32_t>(r.indexS0)>=count00_)break;
            if(r.indexS0>=initializedZones_)return false;
            pc=r.pc=0x27c36c;r.reachedZoneV1=models0C_[r.indexS0];
            if(r.reachedZoneV1){
                //Actual positive arrays/model virtual78+D0/90 are unjoined.
                //Never drain/release them or replace a populated list by null.
                pc=r.pc=0x27c378;return false;
            }
            r.offsetS3+=4; //27C370 BEQZL taken delay27C374.
            ++r.indexS0; //27C3FC; no model dereference/callback on null row.
        }
        pc=r.pc=0x27c428;r.returned=true;return true;
    }
    bool canRelease27C030()const noexcept {
        if(closed_)return true;
        if(sourceMode_)return canReleaseSourceNative();
        if(!constructed_)return false;
        for(std::size_t z=0;z<initializedZones_;++z)
            if(capacity04_[z]||used08_[z]||models0C_[z])return false;
        return true;
    }
    bool release27C030()noexcept {
        if(!canRelease27C030())return false;
        if(closed_)return true;
        //Normal all-null rows own no model arrays/references. Partial native
        //allocation cleanup releases only allocations, never unread row data;
        //it is not a receipt that faulted guest constructor/destructor returned.
        models0C_.reset();capacity04_.reset();used08_.reset();source_.reset();
        closed_=true;return true;
    }
public:
    OriginalWorldPhysicsIndex50LA(const OriginalWorldPhysicsIndex50LA&)=delete;
    OriginalWorldPhysicsIndex50LA& operator=(const OriginalWorldPhysicsIndex50LA&)=delete;
    ~OriginalWorldPhysicsIndex50LA()noexcept {
        if(constructed_&&!closed_&&!release27C030())std::terminate();
    }
};
}
