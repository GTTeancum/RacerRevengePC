#pragma once
namespace ra_cold_aggregate50LA {
inline bool Owner::preflightSourceWorldControllersNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceControllerPhase_==SourceControllerPhase::unreached){
        for(const auto& p:sourceControllers_)if(p)return false;return true;
    }
    if(sourceControllerPhase_==SourceControllerPhase::nativeClosed){
        for(const auto& p:sourceControllers_)if(p)return false;return true;
    }
    if(!worldApplication_||sourceControllerApplication_!=worldApplication_.get()||
       sourceControllerGeneration_!=worldEntity_.sessionGeneration||worldEntity_.reference00!=1||
       runtimeWorldBorrows_||runtimeCameraBorrows_||constructorRacerWorldBorrows_||
       (sourceControllerPublishedMask_&~31u))return false;
    for(std::size_t i=0;i<5;++i){const auto& p=sourceControllers_[i];const auto& role=worldRoles4DCTo4EC_[i];
        if(sourceControllerPublishedMask_&(1u<<i)){
            if(!sourceControllerAllocationReturned_[i]||!role||*role!=p.get())return false;
        }else if(!role||*role)return false; //Exact still-unmodified resetNULL.
        if(p&&(!sourceControllerAllocationReturned_[i]||p->knownMask!=31u))return false;
        if(!sourceControllerBranchTaken_&&(p||sourceControllerAllocationReturned_[i]||
             (sourceControllerPublishedMask_&(1u<<i))))return false;
    }
    return true;
}
inline bool Owner::abortSourceWorldControllersNative(std::uint32_t& pc)noexcept {
    pc=0;if(!preflightSourceWorldControllersNative())return false;
    if(sourceControllerPhase_==SourceControllerPhase::unreached||sourceControllerPhase_==SourceControllerPhase::nativeClosed)return true;
    //All exact pointer/borrow checks precede every destructive step. No source
    //callback, arithmetic, or fallible operation occurs after this preflight.
    for(std::size_t i=5;i-->0;){
        if(sourceControllerPublishedMask_&(1u<<i))worldRoles4DCTo4EC_[i].reset();
        sourceControllers_[i].reset();
    }
    //Preserve allocation/publication/branch history, never restore constructor
    //NULL or imply guest deletion. A skipped branch leaves true resetNULL intact.
    sourceControllerPhase_=SourceControllerPhase::nativeClosed;return true;
}
inline bool Owner::loadSourceWorldControllers270D1C(std::uint32_t& pc)noexcept {
    pc=0;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||
       sourceControllerPhase_!=SourceControllerPhase::unreached||!sourceWorldTailInvocationCurrent()||
       !worldApplication_||worldEntity_.reference00!=1||!worldEntity_.sessionGeneration||
       runtimeWorldBorrows_||runtimeCameraBorrows_||constructorRacerWorldBorrows_)return false;
    for(std::size_t i=0;i<5;++i)if(sourceControllers_[i]||sourceControllerAllocationReturned_[i]||
       !worldRoles4DCTo4EC_[i]||*worldRoles4DCTo4EC_[i])return false;
    sourceControllerApplication_=worldApplication_.get();sourceControllerGeneration_=worldEntity_.sessionGeneration;
    sourceControllerPhase_=SourceControllerPhase::entered;
    pc=sourceControllerPC_=0x270d1c;
    if(!(world4F0KnownMask_&1u)){sourceControllerPhase_=SourceControllerPhase::bounded;return false;}
    sourceControllerBranchKnown_=true;sourceControllerBranchTaken_=(world4F0KnownValue_&1u)!=0;
    if(sourceControllerBranchTaken_){
        static constexpr std::uint32_t allocationPC[5]={0x270d2c,0x270d70,0x270db4,0x270df8,0x270e3c};
        static constexpr std::uint32_t publicationPC[5]={0x270d74,0x270db8,0x270dfc,0x270e40,0x270e7c};
        static constexpr std::uint32_t storePC[5][5]={
            {0x270d44,0x270d4c,0x270d58,0x270d64,0x270d68},
            {0x270d88,0x270d90,0x270d9c,0x270da8,0x270dac},
            {0x270dcc,0x270dd4,0x270de0,0x270dec,0x270df0},
            {0x270e10,0x270e18,0x270e24,0x270e30,0x270e34},
            {0x270e54,0x270e5c,0x270e68,0x270e74,0x270e78}};
        static constexpr unsigned lane[5]={0,1,3,4,2}; //Source00,04,0C,10,08.
        static constexpr std::uint32_t bits[5]={0x3f800000u,0x3dcccccdu,0x7f7fffffu,0xff7fffffu,0x3f800000u};
        for(std::size_t i=0;i<5;++i){
            if(i){
                //The preceding owner's publication is the NEXT JAL delay
                //slot. It happens BEFORE entering that allocation, including
                //when the previous allocation genuinely returned NULL.
                pc=publicationPC[i-1];worldRoles4DCTo4EC_[i-1]=sourceControllers_[i-1].get();
                sourceControllerPublishedMask_|=std::uint8_t(1u<<(i-1));
            }
            pc=allocationPC[i];
            try{sourceControllers_[i].reset(new SourceController20);}catch(...){sourceControllers_[i].reset();}
            sourceControllerAllocationReturned_[i]=true;
            if(sourceControllers_[i]){auto& p=*sourceControllers_[i];
                for(unsigned w=0;w<5;++w){pc=storePC[i][w];p.words[lane[w]]=bits[w];p.knownMask|=std::uint8_t(1u<<lane[w]);}
            }
        }
        pc=publicationPC[4];worldRoles4DCTo4EC_[4]=sourceControllers_[4].get();sourceControllerPublishedMask_|=16u;
    }
    sourceControllerPhase_=SourceControllerPhase::returnedControllers;
    pc=sourceControllerPC_=0x270e80;return true; //Before actual Config pointer read/reselection.
}
} //namespace ra_cold_aggregate50LA
