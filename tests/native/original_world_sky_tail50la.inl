namespace ra_pod_scene50LA {
bool Owner::continueNativeSkyTail129924(
    std::shared_ptr<rr::assets::OriginalStandardArchiveProvider50LA> provider,std::uint32_t& pc)noexcept {
    pc=0x129924;
    if(!nativeSky3F8_||!nativeSky3F8_->owner||!worldApplication_||
       worldApplication_->attachmentLock423F58()||!provider)return false;
    auto& cube=*nativeSky3F8_->owner;
    if(!cube.commands||!cube.commands->geometryComplete||cube.commands->geometryTerminal||
       cube.commands->geometryCells!=96||cube.commands->geometryFaces!=6||
       !cube.innerOwner||!cube.resourceComplete||cube.resourceTerminal||cube.tailTerminal||
       !canRetireNativeSky3F8())return false;
    if(cube.tailEntered){
        if(!cube.tailComplete||cube.palette||cube.palettePublished7D0||!cube.file.closed_||
           !cube.acbReader||!cube.acbReader->absenceRetired_||!cube.offsetAE8||*cube.offsetAE8!=1024.f)return false;
        pc=0x1aa33c;return true; //Actual prior effects retained, no source calls replayed.
    }
    if(!cube.palette||!cube.palettePublished7D0||!cube.palette->current()||
       !cube.file.current()||cube.file.provider_!=provider||cube.offsetAE8)return false;
    cube.tailEntered=true;cube.tailTerminal=true;
    //Native temporary resource borrows must die before source7D0; resources,
    //same immutable image bank and existing renderer allocations stay intact.
    auto& inner=*cube.innerOwner;
    if(inner.partialTexture)return false;
    for(auto& face:inner.faces)for(auto& texture:face.textures58)
        if(texture&&!texture->withdrawCubPaletteBorrow())return false;
    pc=0x129930;if(!cube.palette->release104970(pc))return false;
    cube.palette.reset();pc=0x129938;cube.palettePublished7D0=false;
    pc=0x129944;if(!cube.file.close131040())return false;
    //Source leaves file40's closed handle value; native pointer remains an
    //identity of that SAME closed owner, explicitly unusable for future reads.
    inner.file40KnownInvalid=true;
    cube.acbReader.emplace();
    if(!cube.acbReader->attempt1577D0(provider,cube.filename,pc))return false;
    if(!cube.acbReader->retire1573B0(pc)||!cube.acbReader->absenceRetired_)return false;
    //Only the genuine native mounted-provider negative path is represented.
    //No guest global FileManager cache/fallback or positive ACB animations.
    pc=0x288ad8;cube.offsetAE8=1024.f;
    //One actual RA cube completed. No geometry submission/update/draw occurs.
    cube.tailComplete=true;cube.tailTerminal=false;pc=0x1aa33c;return true;
}
}
