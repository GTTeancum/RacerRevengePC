#pragma once
#include "original_source_world_light_read50la.inl"
#include <new>
#include <cstring>
namespace ra_cold_aggregate50LA {
bool Owner::preflightSourceWorldSoulTreeSoundsNative()const noexcept {
    if(nativeScbThread_!=std::this_thread::get_id())return false;
    if(sourceSoulTreeSoundPhase_==SourceSoulTreeSoundPhase::unreached)
        return !nativeSoulTreeSounds3B8_||!nativeSoulTreeSounds3B8_->sourceMode;
    if(sourceSoulTreeSoundPhase_==SourceSoulTreeSoundPhase::nativeClosed)
        return !nativeSoulTreeSounds3B8_&&!sourceSoulTreeSounds3B8_&&!sourceSoulTreeSoundConfigArg_;
    if(!worldApplication_||worldEntity_.reference00!=1||runtimeWorldBorrows_||runtimeCameraBorrows_||constructorRacerWorldBorrows_||
       (sourceSoulTreeSoundConfigArg_&&sourceSoulTreeSoundConfigArg_!=sourceWorldAudioConfig_.get()))return false;
    if(sourceSoulTreeSoundPublished_){if(!sourceSoulTreeSounds3B8_||*sourceSoulTreeSounds3B8_!=nativeSoulTreeSounds3B8_.get())return false;}
    else if(!sourceSoulTreeSounds3B8_||*sourceSoulTreeSounds3B8_)return false;
    if(!nativeSoulTreeSounds3B8_)return sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::returned;
    const auto& l=*nativeSoulTreeSounds3B8_;
    if(!l.sourceMode||l.actualOwner||l.complete||l.loaded||(l.sourceConstructorMask&~7u)||
       (bool(l.sourceVtableActual08)!=bool(l.sourceConstructorMask&1u))||
       (l.sourceVtableActual08&&*l.sourceVtableActual08!=0x3d2880)||
       (l.sourceConstructorReturned&&l.sourceConstructorMask!=7u)||
       !preflightSourceSoulTreeSoundListCountNative(l.entity,l.counted,l.sourceEnrolled))return false;
    if(sourceSoulTreeSoundPublished_&&!l.sourceConstructorReturned)return false;
    if(l.sourceArrayAttempted){
        if(!sourceSoulTreeSoundCountReturned_||l.count30!=42||!l.sourceArrayPublished||
           !l.sourceRows34||*l.sourceRows34!=l.rows34.get()||l.sourceRowsConstructed!=(l.rows34?42u:0u))return false;
    }else if(l.rows34||l.sourceArrayPublished||l.sourceRows34||l.sourceRowsConstructed||
        (!sourceSoulTreeSoundCountReturned_&&(l.sourceConstructorMask&2u)&&l.count30))return false;
    if(l.rows34){if(!l.sourceArrayHeaderStride00||*l.sourceArrayHeaderStride00!=12||
        !l.sourceArrayHeaderCount04||*l.sourceArrayHeaderCount04!=42)return false;}
    else if(l.sourceArrayHeaderStride00||l.sourceArrayHeaderCount04)return false;
    if(l.sourceLoaded>42||(!l.rows34&&l.sourceLoaded))return false;
    if(l.rows34)for(std::size_t i=0;i<42;++i){const auto& r=l.rows34[i];
        if(!r.sourceMode||r.sourceConstructorMask!=15u||!r.sourceVtableActual00||*r.sourceVtableActual00!=0x3d2a30||r.runtime04)return false;
        if(r.sourceDescriptorPublished){if(!r.sourceDescriptorAttempted||!r.sourceDescriptor08||*r.sourceDescriptor08!=r.descriptor08.get())return false;}
        else if(r.sourceDescriptorAttempted||r.sourceDescriptor08||r.descriptor08)return false;
        if(i<l.sourceLoaded&&!r.sourceCallReturned)return false;
        if(!r.descriptor08){if(r.sourceCallReturned&&r.sourceCallValue)return false;continue;}
        const auto& d=*r.descriptor08;
        if(!d.sourceMode||!d.sourceConstructorReturned||d.actualSource||d.loaded||!d.resource00.empty()||!d.attach1D0.empty()||
           !d.sourceResourceLength||*d.sourceResourceLength>=260||d.sourceResource00[*d.sourceResourceLength]!=0||
           !d.sourceAttachLength||*d.sourceAttachLength>=260||d.sourceAttach1D0[*d.sourceAttachLength]!=0)return false;
        for(auto k:d.sourceKnown)if(k>1)return false;
        if(!d.sourceKnown[0x1c4/4]||d.sourceWords[0x1c4/4]!=0x3d2a60||
           (r.sourceCallReturned&&r.sourceCallValue&&!d.sourceLoaded))return false;
    }
    return sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::returned||
        (l.sourceLoaded==42&&sourceSoulTreeSoundIndexS0_&&*sourceSoulTreeSoundIndexS0_==42&&
         sourceSoulTreeSoundOffsetS1_&&*sourceSoulTreeSoundOffsetS1_==42*12);
}
bool Owner::abortSourceWorldSoulTreeSoundsNative(std::uint32_t& pc)noexcept {
    if(sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::unreached&&sourceMaterialSoundPhase_!=SourceMaterialSoundPhase::nativeClosed)return false;
    if(!preflightSourceWorldParticleCountNative())return false;
    if(sourceSoulTreeSoundPhase_==SourceSoulTreeSoundPhase::unreached||sourceSoulTreeSoundPhase_==SourceSoulTreeSoundPhase::nativeClosed)return true;
    if(nativeSoulTreeSounds3B8_){auto& l=*nativeSoulTreeSounds3B8_;
        if(!releaseSourceSoulTreeSoundListCountNative(l.entity,l.counted,l.sourceEnrolled))return false;
        sourceSoulTreeSounds3B8_.reset();
        if(l.rows34)for(std::size_t i=42;i-->0;){auto& r=l.rows34[i];r.sourceDescriptor08.reset();r.descriptor08.reset();}
        l.sourceRows34.reset();l.rows34.reset();nativeSoulTreeSounds3B8_.reset();
    }else sourceSoulTreeSounds3B8_.reset();
    sourceSoulTreeSoundConfigArg_=nullptr;sourceSoulTreeSoundPhase_=SourceSoulTreeSoundPhase::nativeClosed;
    return true; //Native disposal, not a fabricated source list destructor or NULL.
}
bool Owner::constructSourceSoulTreeDescriptor278C00(NativeSoulTreeSoundDescriptor278C00& d,
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
    put(0x278c20,0x1c4,0x3d2a60);
    //Derived260!=4 also bypasses8B9C; no source first-use history minted.
    pc=0x278ccc;std::memcpy(d.sourceAttach1D0.data(),"body",4);
    pc=0x278cd8;d.sourceAttach1D0[4]=0;d.sourceAttachLength=4;
    d.sourceConstructorReturned=true;pc=0x278cec;return true;
}
bool Owner::loadSourceSoulTreeDescriptor278D00(NativeSoulTreeSoundDescriptor278C00& d,
    const std::array<std::uint32_t,4>& origin,bool& sourceReturned,std::uint32_t& pc)noexcept {
    sourceReturned=false;auto& config=*sourceWorldAudioConfig_;
    a::OriginalStandardTextureDiagnostic50LA diagnostic;
    auto fetch=[&](const char* key,std::string_view& value,bool& found,std::uint32_t& copy)noexcept{
        if(!config.readSourceSoulTreeSoundField158240(key,value,found,copy,diagnostic)){
            pc=diagnostic.sourcePC?diagnostic.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};bool found=false;
    const auto put=[&](std::uint32_t at,std::size_t off,std::uint32_t word){pc=at;d.sourceWords[off/4]=word;d.sourceKnown[off/4]=1;};
    pc=0x2787e0;if(!read.text("soundresourcefile","none",d.sourceResource00.data(),260,found))return false;
    d.sourceResourceLength=std::strlen(d.sourceResource00.data());
    if(!found){pc=0x2787f4;return true;} //Actual false propagates through278D28 and278E50 free/NULL.
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
    pc=0x278d44;if(!read.text("attachtoobject","body",d.sourceAttach1D0.data(),260,found))return false;
    d.sourceAttachLength=std::strlen(d.sourceAttach1D0.data());
    pc=0x278d4c;sourceReturned=true;d.sourceLoaded=true;return true;
}
bool Owner::loadSourceWorldSoulTreeSounds2714D8(std::uint32_t& pc)noexcept {
    pc=sourceSoulTreeSoundPC_;std::array<std::uint32_t,4> origin,forward;
    if(nativeScbThread_!=std::this_thread::get_id()||closed_||terminal_||nativeScbRevoked_||
       sourceSoulTreeSoundPhase_!=SourceSoulTreeSoundPhase::unreached||!sourceWorldSoulTreeSoundsInvocationCurrent()||
       nativeSoulTreeSounds3B8_||!sourceSoulTreeSounds3B8_||*sourceSoulTreeSounds3B8_||!sourceWorldAudioConfig_||
       !copySourceWorldStartDefaults(origin,forward)||!reserveSourceSoulTreeSoundListNative())return false;
    const auto at=[&](std::uint32_t p){pc=sourceSoulTreeSoundPC_=p;};
    const auto stop=[&](std::uint32_t p){at(p);sourceSoulTreeSoundPhase_=SourceSoulTreeSoundPhase::bounded;return false;};
    sourceSoulTreeSoundPhase_=SourceSoulTreeSoundPhase::entered;
    at(0x2714d8);nativeSoulTreeSounds3B8_.reset(new(std::nothrow) NativeSoulTreeSoundList3B8);
    if(nativeSoulTreeSounds3B8_){auto& l=*nativeSoulTreeSounds3B8_;l.sourceMode=true;
        if(!constructSourceSoulTreeSoundList279850(l.entity,l.counted,l.sourceEnrolled,pc))return stop(pc);
        at(0x27986c);l.sourceVtableActual08=0x3d2880;l.sourceConstructorMask|=1;
        at(0x279870);l.count30=0;l.sourceConstructorMask|=2;
        at(0x279874);l.rows34.reset();l.sourceConstructorMask|=4;l.sourceConstructorReturned=true;
    }
    at(0x2714f4);sourceSoulTreeSounds3B8_=nativeSoulTreeSounds3B8_.get();sourceSoulTreeSoundPublished_=true;
    at(0x2714fc);if(!nativeSoulTreeSounds3B8_)return stop(pc);
    auto& l=*nativeSoulTreeSounds3B8_;auto& config=*sourceWorldAudioConfig_;
    at(0x271508);sourceSoulTreeSoundConfigArg_=&config;
    a::OriginalStandardTextureDiagnostic50LA d;bool returned=false,found=false;
    auto fetch=[&](const char* key,std::string_view& value,bool& present,std::uint32_t& copy)noexcept{
        if(!config.readSourceSoulTreeSoundField158240(key,value,present,copy,d)){pc=d.sourcePC?d.sourcePC:config.sourcePC_;return false;}return true;};
    source_world_light50LA::Reader<decltype(fetch)> read{fetch,pc};
    at(0x279938);if(!config.selectSourceSoulTreeSoundSection157D00(0,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
    at(0x279950);if(!read.integer("numsoultreesounds",0,&l.count30,found))return stop(pc);
    sourceSoulTreeSoundCountReturned_=true;
    at(0x279958);if(l.count30!=42)return stop(pc); //Zero/other-authored cohorts not projected into this branch.
    //Actual520-byte request includes16-byte array header. Native semantic array
    //owns the same42 rows; no native constructor is a source row constructor.
    at(0x279978);l.sourceArrayAttempted=true;l.rows34.reset(new(std::nothrow) NativeSoulTreeSoundRow278DD0[42]);
    at(0x279994);
    if(l.rows34){at(0x2e89d0);l.sourceArrayHeaderStride00=12;at(0x2e89d4);l.sourceArrayHeaderCount04=42;}
    if(l.rows34)for(std::size_t i=0;i<42;++i){auto& r=l.rows34[i];r.sourceMode=true;
        at(0x278dd8);r.sourceVtableActual00=0x3d2a40;r.sourceConstructorMask|=1;
        at(0x278de4);r.sourceVtableActual00=0x3d2a30;r.sourceConstructorMask|=2;
        at(0x278de8);r.runtime04=nullptr;r.sourceConstructorMask|=4;
        at(0x278dec);r.descriptor08.reset();r.sourceConstructorMask|=8;++l.sourceRowsConstructed;
    }
    at(0x27999c);l.sourceRows34=l.rows34.get();l.sourceArrayPublished=true;
    at(0x2799a0);sourceSoulTreeSoundIndexS0_=0;
    at(0x2799a8);sourceSoulTreeSoundOffsetS1_=0;
    for(std::uint32_t i=0;i<42;++i){
        if(!sourceSoulTreeSoundIndexS0_||*sourceSoulTreeSoundIndexS0_!=i||
           !sourceSoulTreeSoundOffsetS1_||*sourceSoulTreeSoundOffsetS1_!=i*12)return stop(0x2799ac);
        at(0x2799b8);constexpr char prefix[]="SoultreeSound";
        std::memcpy(sourceSoulTreeSoundName50_.data(),prefix,sizeof(prefix)-1);std::size_t n=sizeof(prefix)-1;
        if(i+1>=10)sourceSoulTreeSoundName50_[n++]=char('0'+(i+1)/10);
        sourceSoulTreeSoundName50_[n++]=char('0'+(i+1)%10);sourceSoulTreeSoundName50_[n]=0;sourceSoulTreeSoundNameProduced_=n+1;
        at(0x2799c4);if(!config.selectSourceSoulTreeSoundSection157D00(i+1,returned,d))return stop(d.sourcePC?d.sourcePC:config.sourcePC_);
        //Selector return ignored. Raw-arrayNULL still performs the actual first
        //row selection before the actual vtable dereference fails.
        at(0x2799d4);if(!l.rows34)return stop(pc);
        auto& r=l.rows34[i];
        at(0x278e18);r.sourceDescriptorAttempted=true;
        r.descriptor08.reset(new(std::nothrow) NativeSoulTreeSoundDescriptor278C00(NativeSoulTreeSoundDescriptor278C00::SourceTag{}));
        if(r.descriptor08&&!constructSourceSoulTreeDescriptor278C00(*r.descriptor08,origin,pc))return stop(pc);
        at(0x278e30);r.sourceDescriptor08=r.descriptor08.get();r.sourceDescriptorPublished=true;
        at(0x278e38);if(!r.descriptor08)return stop(pc);
        bool childReturned=false;at(0x278e40);
        if(!loadSourceSoulTreeDescriptor278D00(*r.descriptor08,origin,childReturned,pc))return stop(pc);
        if(!childReturned){at(0x278e50);r.descriptor08.reset();at(0x278e58);r.sourceDescriptor08=nullptr;}
        r.sourceCallReturned=true;r.sourceCallValue=childReturned;
        //List ignores each child's integer result. Preserve both outcomes' real
        //loop stores without synthesizing a loaded runtime Sound.
        at(0x2799e4);*sourceSoulTreeSoundOffsetS1_+=12;
        at(0x2799e8);++*sourceSoulTreeSoundIndexS0_;++l.sourceLoaded;
    }
    at(0x279a00);sourceSoulTreeSoundPhase_=SourceSoulTreeSoundPhase::returned;
    at(0x271530);return true; //Real list loader1; before next MaterialCollision list allocation.
}
}
