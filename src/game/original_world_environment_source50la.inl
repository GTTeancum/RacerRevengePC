// Included inside rr::game::original_world_environment50KG after fail/Storage.
struct Owner::Source {
    const std::thread::id thread=std::this_thread::get_id();
    enum class Phase {constructed,loading,bounded,returnedZero,returnedOne,retired};
    Phase phase=Phase::constructed;std::uint32_t pc=0x276950;
    std::int32_t count00;char** files04;std::int32_t* zones08;
    std::array<char,260> terrain0C,water110,unknown214,ecosystem318,particle41C;
    // Only explicit prefix bytes are source-known. 214 is never written here.
    std::array<std::size_t,5> stringKnown{{0,0,0,0,0}};
    std::array<std::uint32_t,11> scalar520; //520..548; last2 unwritten byctor.
    std::uint16_t scalarKnown=0;
    std::uint32_t vtable54C;
    std::unique_ptr<char*[]> fileSlots;
    std::unique_ptr<char[]> firstFile;
    std::unique_ptr<std::int32_t[]> zoneRows;
    bool fileSlotWritten=false,fileTextReturned=false;
    std::size_t zoneValuesWritten=0,zoneValuesAdjusted=0;
};
Owner::Owner(SourceTag,std::unique_ptr<Source> source,const OriginalWreckComponent50LA& world,
    const rr::platform::OriginalWorldSourceConfig50LA& config)noexcept
    :source_(std::move(source)),sourceMode_(true),sourceWorld_(&world),sourceConfig_(&config),
     sourceGeneration_(world.sessionGeneration) {
    auto& s=*source_;s.pc=0x276968;s.vtable54C=0x3d2b50;
    s.pc=0x27696c;s.count00=0;s.pc=0x276970;s.files04=nullptr;
    s.pc=0x276984;s.zones08=nullptr;
    //Fixed104!=4 skips assertion first-use8B80/8B84 entirely.
    s.pc=0x276a20;std::memcpy(s.terrain0C.data(),"none",4);
    s.pc=0x276a2c;s.terrain0C[4]=0;s.stringKnown[0]=5;
    s.pc=0x276ad8;std::memcpy(s.water110.data(),"none",4);
    s.pc=0x276ae4;s.water110[4]=0;s.stringKnown[1]=5;
    for(unsigned i=0;i<9;++i){s.pc=0x276ae8+i*4;s.scalar520[i]=0;s.scalarKnown|=std::uint16_t(1u<<i);}
    s.pc=0x276b20;
}
bool Owner::allocateSource270A58(const OriginalWreckComponent50LA& world,
    const rr::platform::OriginalWorldSourceConfig50LA& config,std::unique_ptr<Owner>& out,Diagnostic& d)noexcept {
    d={};if(out||world.reference00!=1||!world.sessionGeneration)return fail(d,Status::sourceShape,0);
    std::unique_ptr<Source> capacity;
    try{capacity.reset(new Source);}catch(...){return fail(d,Status::allocation,0);}
    // Native PIMPL capacity refusal is above the original allocation interval.
    // This actual Owner allocation is the nullable guest malloc(550) analogue.
    try{out.reset(new Owner(SourceTag{},std::move(capacity),world,config));}catch(...){out.reset();}
    d.status=Status::complete;d.sourcePC=0x270a70;return true;
}
bool Owner::loadSource276C00(const OriginalWreckComponent50LA& world,
    rr::platform::OriginalWorldSourceConfig50LA& config,std::int32_t zones,
    bool& returned,Diagnostic& d)noexcept {
    returned=false;d={};
    if(!sourceMode_||sourceClosed_||!source_||storage_||sourceLoadAttempted_||
       sourceWorld_!=&world||sourceConfig_!=&config||sourceGeneration_!=world.sessionGeneration||
       world.reference00!=1||zones!=10||source_->thread!=std::this_thread::get_id())
        return fail(d,Status::sourceShape,0);
    sourceLoadAttempted_=true;auto& s=*source_;s.phase=Source::Phase::loading;
    assets::OriginalStandardTextureDiagnostic50LA io;bool valueReturned=false;
    const auto bounded=[&](std::uint32_t pc,Status st=Status::sourceShape)noexcept {
        s.phase=Source::Phase::bounded;s.pc=pc;return fail(d,st,pc);};
    const auto ioPC=[&]()noexcept{return io.sourcePC?io.sourcePC:config.sourcePC_;};
    s.pc=0x276c2c;
    if(!config.selectEnvironment157D00(valueReturned,io))return bounded(ioPC());
    if(!valueReturned){s.phase=Source::Phase::returnedZero;s.pc=0x277200;d.status=Status::complete;d.sourcePC=s.pc;return true;}
    s.pc=0x276c54;
    if(!config.readEnvironmentInteger158B30("NumberOfSkyCubes",1,s.count00,valueReturned,io))return bounded(ioPC());
    // This source occurrence is authenticated RA; retain reached differing
    // count rather than allocate an unbounded native array or normalize it.
    if(s.count00!=1)return bounded(0x276c5c);
    s.pc=0x276c68;try{s.fileSlots.reset(new char*[1]);}catch(...){s.fileSlots.reset();}
    s.pc=0x276c70;s.files04=s.fileSlots.get();
    if(!s.files04)return bounded(0x276c7c); // Pre next allocation; no invented innerNULL.
    s.pc=0x276c7c;try{s.firstFile.reset(new char[260]);}catch(...){s.firstFile.reset();}
    s.pc=0x276c9c;s.files04[0]=s.firstFile.get();s.fileSlotWritten=true;
    s.pc=0x276cd0;
    if(!config.readEnvironmentString158AD0("CubeFile1","none",s.firstFile.get(),260,valueReturned,io))return bounded(ioPC());
    if(!valueReturned){s.pc=0x276d18;
        if(!config.readEnvironmentString158AD0("CubeFile","none",s.firstFile.get(),260,valueReturned,io))return bounded(ioPC());}
    s.fileTextReturned=true;
    s.pc=0x276d40;try{s.zoneRows.reset(new std::int32_t[10]);}catch(...){s.zoneRows.reset();}
    s.pc=0x276d48;s.zones08=s.zoneRows.get();
    if(!s.zones08)return bounded(0x276d84);
    static constexpr const char* keys[]={"CubeFileForZone1","CubeFileForZone2","CubeFileForZone3",
        "CubeFileForZone4","CubeFileForZone5","CubeFileForZone6","CubeFileForZone7",
        "CubeFileForZone8","CubeFileForZone9","CubeFileForZone10"};
    for(unsigned i=0;i<10;++i){s.pc=0x276d84;
        if(!config.readEnvironmentInteger158B30(keys[i],1,s.zones08[i],valueReturned,io))return bounded(ioPC());
        s.zoneValuesWritten=i+1;
        if(!EnvironmentHistory50LA::enter(0,s.zones08[i]<=s.count00,s.pc))return bounded(s.pc);
        //276DD8 SLT(count,value), then276DE0 XORI1 in JAL delay slot:
        //actual assertion a0=(value<=count). Authored zero therefore becomes-1.
        if(s.zones08[i]==std::numeric_limits<std::int32_t>::min())return bounded(0x276e24);
        s.pc=0x276e28;--s.zones08[i];s.zoneValuesAdjusted=i+1;
    }
    s.pc=0x276e64;
    if(std::strcmp(s.firstFile.get(),"none")==0){s.pc=0x276e78;s.firstFile.reset();
        s.pc=0x276e84;s.files04[0]=nullptr;s.pc=0x276e88;s.count00=0;}
    const auto text=[&](const char* key,std::array<char,260>& dest,unsigned known,std::uint32_t pc)noexcept {
        s.pc=pc;if(!config.readEnvironmentString158AD0(key,"none",dest.data(),260,valueReturned,io))return false;
        s.stringKnown[known]=std::strlen(dest.data())+1;return true;};
    if(!text("EcosystemFile",s.ecosystem318,3,0x276ea4))return bounded(ioPC());
    if(std::strcmp(s.ecosystem318.data(),"none"))return bounded(0x276ec8);
    if(!text("TerrainFile",s.terrain0C,0,0x276f34))return bounded(ioPC());
    const auto number=[&](const char* key,unsigned i,std::uint32_t fallback,std::uint32_t pc)noexcept {
        s.pc=pc;if(!config.readEnvironmentFloat158B80(key,fallback,s.scalar520[i],valueReturned,io))return false;
        s.scalarKnown|=std::uint16_t(1u<<i);return true;};
    if(!number("TerrainWidth",0,0,0x276f4c)||!number("TerrainBreadth",1,0,0x276f64)||
       !number("TerrainWidthScale",2,0,0x276f7c)||!number("TerrainBreadthScale",3,0,0x276f94)||
       !number("TerrainBorderWidth",4,0,0x276fac))return bounded(ioPC());
    if(!number("Xmin",5,0,0x276fc4))return bounded(ioPC());
    if(!valueReturned){s.pc=0x276fdc;s.scalar520[5]=s.scalar520[4]^0x80000000u;}
    if(!number("Xmax",6,0,0x276ff0))return bounded(ioPC());
    if(!valueReturned){
        //Exact admitted binary32 5*24=120,256*120=30720,+positive0.
        //Each integer is <2^24 and exactly representable; no host FPU/libm,
        //fused arithmetic or general finite fallback. Refuse BEFORE firstMUL.
        if(s.scalar520[0]!=0x40a00000u||s.scalar520[2]!=0x41c00000u||s.scalar520[4]!=0)
            return bounded(0x277014,Status::arithmetic);
        s.pc=0x277020;s.scalar520[6]=0x46f00000u;
    }
    if(!number("Zmin",7,0,0x277034))return bounded(ioPC());
    if(!valueReturned){s.pc=0x27704c;s.scalar520[7]=s.scalar520[4]^0x80000000u;}
    if(!number("Zmax",8,0,0x277060))return bounded(ioPC());
    if(!valueReturned){
        if(s.scalar520[1]!=0x40a00000u||s.scalar520[3]!=0x41c00000u||s.scalar520[4]!=0)
            return bounded(0x277084,Status::arithmetic);
        s.pc=0x277090;s.scalar520[8]=0x46f00000u;
    }
    //SUB and trunc.W assertions admitted only for these exact original lanes.
    const bool width=s.scalar520[6]==0x46f00000u&&s.scalar520[5]==0x80000000u;
    const bool breadth=s.scalar520[8]==0x46f00000u&&s.scalar520[7]==0x80000000u;
    if(!EnvironmentHistory50LA::enter(1,width,s.pc))return bounded(s.pc,Status::arithmetic);
    if(!EnvironmentHistory50LA::enter(2,breadth,s.pc))return bounded(s.pc,Status::arithmetic);
    if(!number("EcoDistance2D",9,0x43340000u,0x277198)||
       !number("EcoDistance3D",10,0x42f00000u,0x2771b4))return bounded(ioPC());
    if(!text("WaterFile",s.water110,1,0x2771d4)||
       !text("ParticleTextureMapName",s.particle41C,4,0x2771f4))return bounded(ioPC());
    returned=true;s.phase=Source::Phase::returnedOne;s.pc=0x277200;d.status=Status::complete;d.sourcePC=s.pc;return true;
}
bool Owner::preflightSourceNative()const noexcept {
    if(!sourceMode_)return false;if(sourceClosed_)return !source_;
    if(!source_||storage_||source_->thread!=std::this_thread::get_id())return false;
    const auto& s=*source_;
    if(s.files04!=s.fileSlots.get()||s.zones08!=s.zoneRows.get())return false;
    if(s.fileSlotWritten&&(!s.fileSlots||s.files04[0]!=s.firstFile.get()))return false;
    //Unwritten slots/tail strings/zone rows must not be read during cleanup.
    return true;
}
bool Owner::abortSourceNative(Diagnostic& d)noexcept {
    d={};if(!preflightSourceNative())return fail(d,Status::sourceShape,0);
    if(sourceClosed_){d.status=Status::complete;return true;}
    source_->firstFile.reset();source_->fileSlots.reset();source_->zoneRows.reset();
    source_.reset();sourceWorld_=nullptr;sourceConfig_=nullptr;sourceClosed_=true;
    d.status=Status::complete;return true; // Native disposal only; no source276B30receipt.
}
bool Owner::retireSource276B30(std::int32_t deleting,Diagnostic& d)noexcept {
    d={};if(deleting!=1||!preflightSourceNative()||sourceClosed_||
       (source_->phase!=Source::Phase::returnedZero&&source_->phase!=Source::Phase::returnedOne))
        return fail(d,Status::sourceShape,0x276b30);
    auto& s=*source_;s.pc=0x276b5c;s.vtable54C=0x3d2b50;
    if(s.count00>0){if(s.count00!=1||!s.fileSlotWritten||!s.files04)return fail(d,Status::sourceShape,0x276b6c);
        if(s.files04[0]){s.pc=0x276b80;s.firstFile.reset();}}
    if(s.files04){s.pc=0x276bac;s.fileSlots.reset();}
    if(s.zones08){s.pc=0x276bc0;s.zoneRows.reset();}
    s.pc=0x276bd8;source_.reset();sourceClosed_=true;sourceWorld_=nullptr;sourceConfig_=nullptr;
    d.status=Status::complete;d.sourcePC=0x276be4;return true;
    //Parent drops SAME actualOwner backing at source276BD8 then NULL3A4 at270AA0.
}

// Borrow only the explicit source getter's known NUL-terminated prefix.
// Never expose a native Environment selector or constructor-unknown bytes.
bool Owner::readSourceParticleName2771F4(const OriginalWreckComponent50LA& world,
    const rr::platform::OriginalWorldSourceConfig50LA& config,const char*& text,std::size_t& length)const noexcept {
    if(!sourceMode_||sourceClosed_||!source_||sourceWorld_!=&world||sourceConfig_!=&config||
       sourceGeneration_!=world.sessionGeneration||source_->thread!=std::this_thread::get_id()||
       source_->phase!=Source::Phase::returnedOne)return false;
    const auto known=source_->stringKnown[4];
    if(!known||known>source_->particle41C.size()||source_->particle41C[known-1]!=0)return false;
    for(std::size_t i=0;i+1<known;++i)if(source_->particle41C[i]==0)return false;
    text=source_->particle41C.data();length=known-1;return true;
}

//Same completed source Environment occurrence, never native spatialBounds534().
bool Owner::readSourceVisibilityBounds273028(const OriginalWreckComponent50LA& world,
    const rr::platform::OriginalWorldSourceConfig50LA& config,std::array<std::uint32_t,4>& words)const noexcept {
    if(!sourceMode_||sourceClosed_||!source_||sourceWorld_!=&world||sourceConfig_!=&config||
       sourceGeneration_!=world.sessionGeneration||source_->thread!=std::this_thread::get_id()||
       source_->phase!=Source::Phase::returnedOne||(source_->scalarKnown&0x1e0u)!=0x1e0u)return false;
    words={{source_->scalar520[5],source_->scalar520[6],source_->scalar520[7],source_->scalar520[8]}};return true;
}

//Count00 was initialized27696C, then read276C54 and possibly collapsed276E88.
//This entry is only the new274B0C load; it does not admit prepared sky resources.
bool Owner::readSourceSkyCount274B0C(const OriginalWreckComponent50LA& world,
    const rr::platform::OriginalWorldSourceConfig50LA& config,std::int32_t& out)const noexcept {
    if(!preflightSourceNative()||sourceClosed_||!source_||sourceWorld_!=&world||sourceConfig_!=&config||
       sourceGeneration_!=world.sessionGeneration||source_->thread!=std::this_thread::get_id()||
       source_->phase!=Source::Phase::returnedOne)return false;
    out=source_->count00;return true;
}

//Source files04 ownership and getter-produced terminating NUL, not prepared cubeFile().
bool Owner::readSourceSkyFile274B54(const OriginalWreckComponent50LA& world,
    const rr::platform::OriginalWorldSourceConfig50LA& config,std::uint32_t index,
    const char*& out,std::size_t& length)const noexcept {
    std::int32_t count=0;
    if(!readSourceSkyCount274B0C(world,config,count)||index!=0||count!=1||
       !source_->fileSlotWritten||!source_->fileTextReturned||!source_->files04||
       !source_->firstFile||source_->files04[0]!=source_->firstFile.get())return false;
    std::size_t n=0;while(n<260&&source_->firstFile[n])++n;
    if(n==260)return false;
    out=source_->firstFile.get();length=n;return true;
}
