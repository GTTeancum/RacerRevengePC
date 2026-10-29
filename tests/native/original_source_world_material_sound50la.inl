#pragma once
#include "original_source_world_light_read50la.inl"
#include <new>
#include <cstring>
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldMaterialSoundsNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceMaterialSoundPhase_==SourceMaterialSoundPhase::unreached){
        for(const auto& l:nativeMaterialSounds3BC_)if(l&&l->sourceMode)return false;return true;}
    if(sourceMaterialSoundPhase_==SourceMaterialSoundPhase::nativeClosed)
        return !nativeMaterialSounds3BC_[0]&&!nativeMaterialSounds3BC_[1]&&
            !sourceMaterialSounds3BC_[0]&&!sourceMaterialSounds3BC_[1]&&!sourceMaterialSoundConfigArg_;
    if(!worldApplication_||worldEntity_.reference00!=1||runtimeWorldBorrows_||runtimeCameraBorrows_||constructorRacerWorldBorrows_||
       (sourceMaterialSoundConfigArg_&&sourceMaterialSoundConfigArg_!=sourceWorldAudioConfig_.get()))return false;
    for(unsigned kind=0;kind<2;++kind){const auto& owner=nativeMaterialSounds3BC_[kind];
        if(sourceMaterialSoundNativeWithdrawn_[kind]){if(owner||sourceMaterialSounds3BC_[kind])return false;continue;}
        if(sourceMaterialSoundPublished_[kind]){if(!sourceMaterialSounds3BC_[kind]||*sourceMaterialSounds3BC_[kind]!=owner.get())return false;}
        else if(!sourceMaterialSounds3BC_[kind]||*sourceMaterialSounds3BC_[kind])return false;
        if(!owner){if(sourceMaterialSoundPhase_==SourceMaterialSoundPhase::returned)return false;continue;}
        const auto& l=*owner;const bool drive=kind==1;
        if(!l.sourceMode||l.drive!=drive||l.complete||l.actualOwner||l.actualCatalog||l.loaded||l.rows34||
           (l.sourceConstructorMask&~7u)||bool(l.sourceVtableActual08)!=bool(l.sourceConstructorMask&1u)||
           (l.sourceVtableActual08&&*l.sourceVtableActual08!=(drive?0x3d2780u:0x3d2800u))||
           (l.sourceConstructorReturned&&l.sourceConstructorMask!=7u)||
           !preflightSourceMaterialSoundListCountNative(l.entity,l.counted,l.sourceEnrolled))return false;
        if(sourceMaterialSoundPublished_[kind]&&!l.sourceConstructorReturned)return false;
        if(l.sourceArrayAttempted){
            if(!l.sourceCountReturned||l.count30!=12||!l.sourceArrayPublished||!l.sourceRows34||
               *l.sourceRows34!=(l.sourceArray?l.sourceArray->rows.data():nullptr)||
               l.sourceRowsConstructed!=(l.sourceArray?12u:0u))return false;
        }else if(l.sourceArray||l.sourceArrayPublished||l.sourceRowsConstructed||
            (l.sourceRows34&&*l.sourceRows34)||(!l.sourceCountReturned&&(l.sourceConstructorMask&2u)&&l.count30))return false;
        if(bool(l.sourceRows34)!=bool(l.sourceConstructorMask&4u))return false;
        if(l.sourceArray){if(!l.sourceArrayHeaderStride00||*l.sourceArrayHeaderStride00!=(drive?496u:464u)||
            !l.sourceArrayHeaderCount04||*l.sourceArrayHeaderCount04!=12)return false;}
        else if(l.sourceArrayHeaderStride00||l.sourceArrayHeaderCount04)return false;
        if(l.sourceLoaded>12||(!l.sourceArray&&l.sourceLoaded))return false;
        if(l.sourceArray)for(std::size_t i=0;i<12;++i){const auto& d=l.sourceArray->rows[i];
            if(!d.sourceMode||!d.sourceConstructorReturned||!d.resource00.empty()||d.actualSource||d.actualCatalogName||d.actualDrive||d.loaded||
               !d.sourceResourceLength||*d.sourceResourceLength>=260||d.sourceResource00[*d.sourceResourceLength]!=0)return false;
            for(auto k:d.sourceKnown)if(k>1)return false;
            if(!d.sourceKnown[0x1c4/4]||d.sourceWords[0x1c4/4]!=(drive?0x3d2a80u:0x3d2a90u)||
               (i<l.sourceLoaded&&!d.sourceCallReturned))return false;
        }
        if(sourceMaterialSoundPhase_==SourceMaterialSoundPhase::returned&&l.sourceLoaded!=12)return false;
    }
    return sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::returned||
        (sourceMaterialSoundKind_&&*sourceMaterialSoundKind_==1&&sourceMaterialSoundIndexS0_&&*sourceMaterialSoundIndexS0_==12&&
         sourceMaterialSoundOffsetS1_&&*sourceMaterialSoundOffsetS1_==12*496);
}
bool Owner::abortSourceWorldMaterialSoundsNative(std::uint32_t& pc)noexcept {
    if(sourceMaterialAssignmentPhase_!=SourceMaterialAssignmentPhase::unreached&&sourceMaterialAssignmentPhase_!=SourceMaterialAssignmentPhase::nativeClosed)return false;
    //Parent's checked floor includes both actual counted lists, even unpublished orphans.
    if(!preflightSourceWorldParticleCountNative())return false;
    if(sourceMaterialSoundPhase_==SourceMaterialSoundPhase::unreached||sourceMaterialSoundPhase_==SourceMaterialSoundPhase::nativeClosed)return true;
    for(unsigned kind=2;kind-->0;){auto& owner=nativeMaterialSounds3BC_[kind];
        if(owner){auto& l=*owner;
            if(!releaseSourceMaterialSoundListCountNative(l.entity,l.counted,l.sourceEnrolled))return false;
            sourceMaterialSounds3BC_[kind].reset();l.sourceRows34.reset();l.sourceArray.reset();owner.reset();
        }else sourceMaterialSounds3BC_[kind].reset();
        sourceMaterialSoundNativeWithdrawn_[kind]=true;
    }
    sourceMaterialSoundConfigArg_=nullptr;sourceMaterialSoundPhase_=SourceMaterialSoundPhase::nativeClosed;return true;
}
bool Owner::constructSourceMaterialSoundDescriptor278650(NativeMaterialSoundDescriptor& d,bool drive,
    const std::array<std::uint32_t,4>& origin,std::uint32_t& pc)noexcept {
    if(d.sourceMode||d.sourceConstructorReturned)return false;d.sourceMode=true;
    const auto put=[&](std::uint32_t at,std::size_t off,std::uint32_t word){pc=at;d.sourceWords[off/4]=word;d.sourceKnown[off/4]=1;};
    put(0x27866c,0x1c4,0x3d2a90);
    const std::size_t offsets[]={0x110,0x160,0x170,0x190};
    const std::uint32_t pcs[]={0x278674,0x27867c,0x278684,0x27868c};
    for(unsigned j=0;j<4;++j)for(unsigned i=0;i<4;++i)put(pcs[j],offsets[j]+i*4,i==3?0x3f800000u:0u); //SQC2 VF0.
    //260!=4 genuinely bypasses shared guard8B94. Only actual copied bytes known.
    pc=0x278738;std::memcpy(d.sourceResource00.data(),"none",4);
    pc=0x278744;d.sourceResource00[4]=0;d.sourceResourceLength=4;
    for(unsigned i=0;i<4;++i)put(0x27875c,0x110+4*i,origin[i]);
    for(unsigned i=0;i<9;++i)put(i==8?0x278784:0x278760+4*i,0x120+4*i,0);
    pc=0x278780;for(std::size_t off=0x150;off<0x1c0;off+=4)put(0x278780,off,0);
    put(0x27878c,0x1c0,10);
    if(drive){put(0x27896c,0x1c4,0x3d2a80);
        for(unsigned i=0;i<6;++i)put(0x278970+4*i,0x1d0+4*i,0);}
    d.sourceConstructorReturned=true;pc=drive?0x278998:0x2787a0;return true;
}
bool Owner::loadSourceMaterialSoundDescriptor2787B0(NativeMaterialSoundDescriptor& d,bool drive,
    const std::array<std::uint32_t,4>& origin,bool& sourceReturned,std::uint32_t& pc)noexcept {
    sourceReturned=false;auto& config=*sourceWorldAudioConfig_;
    a::OriginalStandardTextureDiagnostic50LA diagnostic;
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept{
        if(!config.readSourceMaterialSoundField158240(key,value,found,copy,diagnostic)){
            pc=diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};bool found=false;
    const auto put=[&](std::uint32_t at,std::size_t off,std::uint32_t word){pc=at;d.sourceWords[off/4]=word;d.sourceKnown[off/4]=1;};
    pc=0x2787e0;if(!read.text("soundresourcefile","none",d.sourceResource00.data(),260,found))return false;
    d.sourceResourceLength=std::strlen(d.sourceResource00.data());
    if(!found){pc=0x2787f4;return true;} //Actual common false; list still advances without freeing inline row.
    pc=0x278800;const auto n=*d.sourceResourceLength;
    if(n+4>=260)return false;std::memcpy(d.sourceResource00.data()+n,".wav",5);d.sourceResourceLength=n+4;
    std::string_view value;std::uint32_t copy=0;pc=0x27881c;
    if(!fetch("position",value,found,copy))return false;
    if(!found){for(unsigned i=0;i<3;++i)put(0x158cd0+8*i,0x110+4*i,origin[i]);}
    else {std::size_t at=0;for(unsigned i=0;i<3;++i){const auto end=i<2?value.find(',',at):value.find('\n',at);
        const auto length=end==std::string_view::npos?value.size()-at:end-at;
        pc=i==0?0x158c64:i==1?0x158c88:0x158cac;std::uint32_t bits;
        if(!read.numberToken(value.substr(at,length),bits))return false;
        put(i==0?0x158c7c:i==1?0x158ca0:0x158cc4,0x110+4*i,bits);
        if(i<2&&end==std::string_view::npos)return false;at=end==std::string_view::npos?value.size():end+1;
    }}
    const char* ints[]={"looping","oneshot","resetwheninaudible","force2d","stereo"};
    const std::uint32_t calls[]={0x278834,0x27884c,0x278864,0x27887c,0x278894};
    for(unsigned i=0;i<5;++i){std::int32_t result;pc=calls[i];if(!read.integer(ints[i],0,&result,found))return false;
        std::uint32_t bits;std::memcpy(&bits,&result,4);d.sourceWords[(0x128+4*i)/4]=bits;d.sourceKnown[(0x128+4*i)/4]=1;}
    const char* floats[]={"oneshotdistance","randomtriggerpercent","maxdistance","mindistance"};
    const std::uint32_t floatCalls[]={0x2788b0,0x2788cc,0x2788e4,0x2788fc};
    const std::uint32_t defaults[]={0x41a00000,0x42c80000,0,0};const std::size_t dest[]={0x13c,0x140,0x120,0x124};
    for(unsigned i=0;i<4;++i){pc=floatCalls[i];if(!read.number(floats[i],defaults[i],d.sourceWords[dest[i]/4],found))return false;d.sourceKnown[dest[i]/4]=1;}
    std::int32_t priority;pc=0x278914;if(!read.integer("priority",10,&priority,found))return false;
    std::memcpy(&d.sourceWords[0x1c0/4],&priority,4);d.sourceKnown[0x1c0/4]=1;
    for(unsigned i=0;i<4;++i){if(!d.sourceKnown[(0x110+4*i)/4]){pc=0x27891c;return false;}put(0x278924,0x160+4*i,d.sourceWords[(0x110+4*i)/4]);}
    put(0x27892c,0x1a8,d.sourceWords[0x124/4]);put(0x278934,0x1ac,d.sourceWords[0x120/4]);
    if(drive){
        const char* keys[]={"materialsamplespeed","materialminvolume","materialminspeed","materialmaxspeed","materialresonantfreqspeed"};
        const std::uint32_t calls[]={0x2789dc,0x2789f4,0x278a0c,0x278a24,0x278a3c};
        for(unsigned i=0;i<5;++i){std::int32_t v;pc=calls[i];if(!read.integer(keys[i],0,&v,found))return false;
            std::memcpy(&d.sourceWords[(0x1d0+4*i)/4],&v,4);d.sourceKnown[(0x1d0+4*i)/4]=1;}
        pc=0x278a54;if(!read.number("materialfreqslope",0,d.sourceWords[0x1e4/4],found))return false;d.sourceKnown[0x1e4/4]=1;
    }
    sourceReturned=true;pc=drive?0x278a5c:0x278938;return true;
}
bool Owner::loadSourceWorldMaterialSounds271530(std::uint32_t& pc)noexcept {
    pc=sourceMaterialSoundPC_;std::array<std::uint32_t,4> origin,forward;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::unreached||!sourceWorldMaterialSoundsInvocationCurrent()||
       nativeMaterialSounds3BC_[0]||nativeMaterialSounds3BC_[1]||!sourceWorldAudioConfig_||
       !sourceMaterialSounds3BC_[0]||*sourceMaterialSounds3BC_[0]||!sourceMaterialSounds3BC_[1]||*sourceMaterialSounds3BC_[1]||
       !copySourceWorldStartDefaults(origin,forward)||!reserveSourceMaterialSoundListNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourceMaterialSoundPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourceMaterialSoundPhase_=SourceMaterialSoundPhase::bounded;return false;};
    sourceMaterialSoundPhase_=SourceMaterialSoundPhase::entered;
    auto& config=*sourceWorldAudioConfig_;a::OriginalStandardTextureDiagnostic50LA diagnostic;
    bool returned=false,found=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& present,std::uint32_t& copy)noexcept{
        if(!config.readSourceMaterialSoundField158240(key,value,present,copy,diagnostic)){
            pc=diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    for(unsigned kind=0;kind<2;++kind){const bool drive=kind==1;sourceMaterialSoundKind_=kind;
        //Native authority capacity refusal does not impersonate source mallocNULL.
        if(kind&&!reserveSourceMaterialSoundListNative())return stop(0x271588);
        at(drive?0x271588:0x271530);
        auto& owner=nativeMaterialSounds3BC_[kind];owner.reset(new(std::nothrow) NativeMaterialSoundList);
        if(owner){auto& l=*owner;l.sourceMode=true;l.drive=drive;
            if(!constructSourceMaterialSoundList279AA0(l.entity,drive,l.counted,l.sourceEnrolled,pc))return stop(pc);
            at(drive?0x279dac:0x279abc);l.sourceVtableActual08=drive?0x3d2780:0x3d2800;l.sourceConstructorMask|=1;
            at(drive?0x279db0:0x279ac0);l.count30=0;l.sourceConstructorMask|=2;
            at(drive?0x279db4:0x279ac4);l.sourceRows34=nullptr;l.sourceConstructorMask|=4;l.sourceConstructorReturned=true;
        }
        at(drive?0x2715a4:0x27154c);sourceMaterialSounds3BC_[kind]=owner.get();sourceMaterialSoundPublished_[kind]=true;
        at(drive?0x2715ac:0x271554);if(!owner)return stop(pc);
        auto& l=*owner;at(drive?0x2715b8:0x271560);sourceMaterialSoundConfigArg_=&config;
        at(drive?0x279e78:0x279b88);
        if(!config.selectSourceMaterialSoundSection157D00(drive?13:0,returned,diagnostic))return stop(diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_);
        at(drive?0x279e90:0x279ba0);
        if(!read.integer(drive?"nummaterialdriveonsounds":"nummaterialcollisionsounds",0,&l.count30,found))return stop(pc);
        l.sourceCountReturned=true;at(drive?0x279e98:0x279ba8);
        if(l.count30!=12)return stop(pc); //Only authenticated positive cohort; no fabricated zero destructor.
        //One nullable semantic array allocation, with inert same-type SourceTag rows.
        //Original requests5584/5968 bytes incl16 header; host layout is explicitly typed.
        at(drive?0x279eb8:0x279bd0);l.sourceArrayAttempted=true;
        l.sourceArray.reset(new(std::nothrow) SourceMaterialSoundArray);
        at(drive?0x279ed4:0x279bec);
        if(l.sourceArray){at(0x2e89d0);l.sourceArrayHeaderStride00=drive?496:464;at(0x2e89d4);l.sourceArrayHeaderCount04=12;
            for(auto& row:l.sourceArray->rows){if(!constructSourceMaterialSoundDescriptor278650(row,drive,origin,pc))return stop(pc);++l.sourceRowsConstructed;}}
        at(drive?0x279edc:0x279bf4);l.sourceRows34=l.sourceArray?l.sourceArray->rows.data():nullptr;l.sourceArrayPublished=true;
        at(drive?0x279ee0:0x279bf8);sourceMaterialSoundIndexS0_=0;
        at(drive?0x279ee8:0x279c00);sourceMaterialSoundOffsetS1_=0;
        for(std::uint32_t i=0;i<12;++i){
            const auto stride=drive?496u:464u;
            if(!sourceMaterialSoundIndexS0_||*sourceMaterialSoundIndexS0_!=i||!sourceMaterialSoundOffsetS1_||*sourceMaterialSoundOffsetS1_!=i*stride)return stop(drive?0x279eec:0x279c04);
            at(drive?0x279ef8:0x279c10);const char* prefix=drive?"MaterialDriveOnSound":"MaterialCollisionSound";
            std::size_t n=std::strlen(prefix);std::memcpy(sourceMaterialSoundName50_.data(),prefix,n);
            if(i+1>=10)sourceMaterialSoundName50_[n++]=char('0'+(i+1)/10);
            sourceMaterialSoundName50_[n++]=char('0'+(i+1)%10);sourceMaterialSoundName50_[n]=0;sourceMaterialSoundNameProduced_=n+1;
            at(drive?0x279f04:0x279c1c);
            if(!config.selectSourceMaterialSoundSection157D00((drive?14:1)+i,returned,diagnostic))return stop(diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_);
            //Selector integer ignored, including with actualNULL array; getter's
            //strict selected prefix refuses any unsupported missing-section path.
            at(drive?0x279f14:0x279c2c);if(!l.sourceArray)return stop(pc);
            auto& row=l.sourceArray->rows[i];bool childValue=false;
            at(drive?0x279f1c:0x279c34);
            if(!loadSourceMaterialSoundDescriptor2787B0(row,drive,origin,childValue,pc))return stop(pc);
            row.sourceCallReturned=true;row.sourceCallValue=childValue;
            at(drive?0x279f24:0x279c3c);*sourceMaterialSoundOffsetS1_+=stride;
            at(drive?0x279f28:0x279c40);++*sourceMaterialSoundIndexS0_;++l.sourceLoaded;
        }
        at(drive?0x279f40:0x279c58); //Actual list1; child results never gate loop advance.
    }
    sourceMaterialSoundPhase_=SourceMaterialSoundPhase::returned;at(0x2715dc);return true;
}
}
