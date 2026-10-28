namespace ra_pod_scene50LA {
bool Owner::continueNativeSkyCommands1298AC(
    std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> context,
    std::uint32_t& pc)noexcept {
    using C=original_world_sky50LA::Commands;
    pc=0x1298ac;
    if(!nativeSky3F8_||!nativeSky3F8_->owner||!context||!context->live()||
       !worldApplication_||worldApplication_->attachmentLock423F58()||
       !worldApplication_->skyPolicy3E0990_||*worldApplication_->skyPolicy3E0990_!=1u)return false;
    auto& cube=*nativeSky3F8_->owner;
    if(!cube.resourceComplete||cube.resourceTerminal||!cube.innerOwner||
       cube.inner38!=cube.innerOwner.get()||!cube.innerPublished||!cube.array3C||
       cube.array3C[0]!=cube.inner38||cube.count40||!canRetireNativeSkyInner128930(cube))return false;
    if(cube.commands){
        const auto& commands=*cube.commands;
        if(commands.terminal||!commands.complete||commands.context!=context||
           !canRetireNativeSkyCommands1298AC(cube)||!commands.packet7E0||!commands.packet7E4||
           !C::current(*commands.packet7E0,*context)||!C::current(*commands.packet7E4,*context))return false;
        for(std::size_t i=0;i<96u;++i)
            if(!commands.objects7E8[i]||!commands.packets968[i]||
               !C::current(*commands.objects7E8[i])||!C::current(*commands.packets968[i],*context))return false;
        pc=0x12991c;return true; //BEFORE12A8A0, not a sourcegeometry/loader return.
    }
    try {
        cube.commands=std::make_unique<NativeSkyCommands1298AC>();auto& commands=*cube.commands;
        commands.entered=true;commands.terminal=true;commands.context=context;
        std::array<float,3> pose{};
        for(std::size_t i=0;i<3u;++i)std::memcpy(&pose[i],&cube.inner38->position10[i],sizeof(float));
        if(!C::create12A120(context,pose,commands.pendingPacket,pc))return false;
        pc=0x1298b4;commands.packet7E0=std::move(commands.pendingPacket);
        for(std::size_t i=0;i<96u;++i){
            pc=0x1298c0;
            if(!C::create133F80(commands.objects7E8[i],pc))return false;
            pc=0x1298d8;++commands.objectsPublished;
            if(!C::create12A510(context,pose,*worldApplication_->skyPolicy3E0990_,commands.pendingPacket,pc))return false;
            pc=0x1298f0;commands.packets968[i]=std::move(commands.pendingPacket);++commands.packetsPublished;
        }
        if(!C::create12A730(context,commands.pendingPacket,pc))return false;
        pc=0x129918;commands.packet7E4=std::move(commands.pendingPacket);
        commands.complete=true;commands.terminal=false;
        pc=0x12991c;return true; //Actual next call12A8A0 remains unjoined.
    }catch(...){return false;} //Every reached native allocation remains under partial custody.
}
bool Owner::canRetireNativeSkyCommands1298AC(const NativeSkyCube288A50& cube)const noexcept {
    using C=original_world_sky50LA::Commands;
    if(!cube.commands)return true;const auto& commands=*cube.commands;
    if(!commands.entered||!commands.context||commands.objectsPublished>96u||
       commands.packetsPublished>commands.objectsPublished||
       commands.objectsPublished>commands.packetsPublished+1u)return false;
    if(commands.packet7E0&&!C::canRetire(*commands.packet7E0))return false;
    if(commands.packet7E4&&!C::canRetire(*commands.packet7E4))return false;
    if(commands.pendingPacket&&!C::canRetire(*commands.pendingPacket))return false;
    for(std::size_t i=0;i<96u;++i){
        if(bool(commands.objects7E8[i])!=(i<commands.objectsPublished)||
           bool(commands.packets968[i])!=(i<commands.packetsPublished))return false;
        if(commands.objects7E8[i]&&!C::canRetire(*commands.objects7E8[i]))return false;
        if(commands.packets968[i]&&!C::canRetire(*commands.packets968[i]))return false;
    }
    return !commands.complete||(commands.packet7E0&&commands.packet7E4&&!commands.pendingPacket&&
        commands.objectsPublished==96u&&commands.packetsPublished==96u);
}
bool Owner::retireNativeSkyCommands1298AC(NativeSkyCube288A50& cube)noexcept {
    if(!canRetireNativeSkyCommands1298AC(cube))return false;
    if(!cube.commands)return true;auto& commands=*cube.commands;
    //A helper that did not return has native pending custody, not a published
    //source pointer. Its actual partial allocation is safely released once.
    commands.pendingPacket.reset();
    //129418..129454: source ascending index, object THEN associated packet.
    for(std::size_t i=0;i<96u;++i){commands.objects7E8[i].reset();commands.packets968[i].reset();}
    commands.objectsPublished=commands.packetsPublished=0;
    commands.packet7E0.reset(); //129464
    commands.packet7E4.reset(); //129478
    //Unowned process3E1970/1978 remain unknown. Do not free/reset them or
    //claim that unjoined guest-global cleanup ran in this native domain.
    commands.context.reset();cube.commands.reset();return true;
}
}
