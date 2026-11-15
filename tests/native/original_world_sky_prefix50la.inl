namespace ra_pod_scene50LA {
bool Owner::beginNativeSkyPalette274AE0(const ra_cold_sources50LA::Owner& source,
    const sky50gw_fixture::Owner& images,const std::string& directory,std::uint8_t bits,
    std::shared_ptr<rr::assets::OriginalStandardArchiveProvider50LA> provider,std::uint32_t& pc)noexcept {
    pc=0x274ae0;
    const auto* decoded=source.sourcesDecoded();
    if(!decoded||!sceneApplication_||sceneApplication_!=worldApplication_||
       sceneApplication_->attachmentLock423F58()||!lighting_||!provider||directory!="skies\\"||
       !images.storage_||!images.storage_->nativeRaProof)return false;
    const auto& proof=*images.storage_->nativeRaProof;
    const auto* name=lighting_->nativeRaSkySelector274AE0(this,decoded->world,visibilityRows_,pc);
    if(!name||proof.source.get()!=&source||proof.archive!=&decoded->archive||!proof.member||
       *proof.member!="skies\\ra.cub"||!proof.memberData||proof.memberBytes!=161825u||
       !proof.memberSha256||std::strcmp(proof.memberSha256,
           "c497927d2fe78de6f2f9bbf9e319ac0ea2b4939e020df45a4ae570e5abb73960")||
       proof.consumedTiles!=48u||proof.mesh!=images.mesh()||proof.bank!=images.bank()||
       proof.spans!=images.spans()||proof.memberOrdinal>=decoded->archive.entries.size())return false;
    const auto& member=decoded->archive.entries[proof.memberOrdinal];
    if(&member.name!=proof.member||member.logical_offset!=proof.memberOffset||member.bytes!=proof.memberBytes||
       proof.memberOffset>decoded->archive.decoded.size()||
       proof.memberBytes>decoded->archive.decoded.size()-proof.memberOffset||
       proof.memberData!=decoded->archive.decoded.data()+proof.memberOffset||
       images.bank()->textureCount()!=48u||images.spans()->size()!=48u)return false;
    for(std::size_t i=0;i<48u;++i)if(proof.tiles[i].image!=images.bank()->texture(static_cast<std::uint32_t>(i))||
        !proof.tiles[i].sourceSha256||!proof.tiles[i].imageSha256)return false;
    const auto policy=(((bits>>5u)&3u)<2u)?1u:0u; //Exact branch; no semantic label.
    if(nativeSky3F8_){
        const auto& a=*nativeSky3F8_;
        if(a.terminal||!a.owner||!a.owner->prefixComplete||a.profileBits!=bits||a.policy!=policy||
           a.owner->images!=&images||a.owner->environmentName!=name||a.owner->tailTerminal)return false;
        if(a.owner->tailEntered){
            const auto& c=*a.owner;
            if(!c.tailComplete||c.palette||c.palettePublished7D0||!c.file.closed_||
               c.file.opened_||c.file.nativeSelectedPalette_||!c.offsetAE8||*c.offsetAE8!=1024.f||
               !c.acbReader||!c.acbReader->absenceRetired_||!c.commands||
               !c.commands->geometryComplete||!canRetireNativeSky3F8())return false;
            pc=0x129750;return true; //Authenticate completed ownership, never reopen/replay.
        }
        if(!a.owner->palette||
           !a.owner->palette->current()||!a.owner->palettePublished7D0||!a.owner->file.current())return false;
        std::size_t cursor=0;if(!a.owner->file.tell(cursor)||
            (!a.owner->resourceEntered&&cursor!=12u)||a.owner->resourceTerminal)return false;
        pc=0x129750;return true; //Only this paused prefix, never1295B0 return.
    }
    try {
        nativeSky3F8_=std::make_unique<NativeSkyArray3F8>();auto& a=*nativeSky3F8_;
        a.entered=true;a.terminal=true;a.profileBits=bits;a.policy=policy;
        pc=0x274b24;a.pointers=std::unique_ptr<NativeSkyCube288A50*[]>(new NativeSkyCube288A50*[1]);
        //World3F8 is now a real owned pointer array. Unpublished slot is unknown.
        pc=0x274b60;a.owner=std::make_unique<NativeSkyCube288A50>();auto& cube=*a.owner;
        cube.images=&images;cube.environmentName=name;
        auto& authority=sceneApplication_->entityAuthority_;auto& count=sceneApplication_->objectCount423F60();
        if(count==std::numeric_limits<std::uint32_t>::max()||!authority.reserveAdditional(1)||
           !authority.canEnroll(cube.entity,key(&cube.entity)))return false;
        pc=0x27e13c;
        if(g::initializeOriginalWreckComponent50LA(cube.entity,g::OriginalWreckComponentKind50LA::entity141300,
           sceneGeneration_,key(&cube.entity),true,count).status!=g::OriginalWreckComponentStatus50LA::complete||
           !authority.enroll(cube.entity))std::terminate();
        cube.counted=true;cube.sourceVtable08=0x3ccee0u;
        cube.query30=0;cube.type32=0xff;cube.flags33=1u;cube.knownFlags33=7u;
        pc=0x27e1bc;const auto type=sceneApplication_->types383408_.searchableGameObject27E1BC();
        if(!type.okay()){pc=type.sourcePC;return false;}cube.type32=type.type;
        pc=0x1292ac;cube.sourceVtable08=0x3cce60u;cube.flags33=3u;
        //Actual NEW native source process table, not a retrospective reset of
        //renderer state.129330 zeroes0x480 then96 triples acquire{1,0,0}.
        pc=0x129330;sceneApplication_->skyTable3E09A0_.emplace();
        for(auto& triple:*sceneApplication_->skyTable3E09A0_)triple={{0x3f800000u,0u,0u}};
        pc=0x288a6c;cube.sourceVtable08=0x3d2ff0u;cube.constructorComplete=true;
        pc=0x274b9c;a.pointers[0]=&cube;cube.published=true;
        pc=0x12a100;sceneApplication_->skyPolicy3E0990_=1u;
        pc=0x12a110;sceneApplication_->skyPolicy36C0F0_=policy;
        //Source globals persist after this owner retires; no restoration of
        //unproved prior state and no native decoder-policy reinterpretation.
        pc=0x288abc;cube.filename=directory+*name;
        if(!cube.file.open131230(provider,cube.filename,proof.memberData,proof.memberBytes,pc))return false;
        std::uint32_t value=0;
        pc=0x129630;if(!cube.file.readWord(value))return false;cube.paletteCount=value;
        if(value!=1u)return false;
        pc=0x129654;if(!cube.file.readWord(cube.count7D8)||cube.count7D8!=1u)return false;
        pc=0x129684;if(!cube.file.readWord(value))return false;cube.paletteOffset=value;
        if(value!=16u)return false;
        std::size_t saved=0;pc=0x1296a0;if(!cube.file.tell(saved)||saved!=12u)return false;
        cube.savedCursor=static_cast<std::uint32_t>(saved);
        pc=0x1296c0;if(!cube.file.seek(*cube.paletteOffset))return false;
        pc=0x1296f4;cube.palette=std::make_unique<original_world_sky50LA::NativeSkyPalette2E38C0>();
        //Native partial ownership before the source constructor's return;
        //7D0 publication remains false if an actual read/allocation fails.
        if(!cube.palette->load2E38C0(cube.file,pc))return false;
        pc=0x129710;cube.palettePublished7D0=true;
        pc=0x129728;if(!cube.file.seek(*cube.savedCursor))return false;
        cube.prefixComplete=true;a.terminal=false;
        pc=0x129750;return true; //BEFORE inner cube array/resources; no1295B0/13C return.
    }catch(...){return false;} //All reached owners remain until exact native partial cleanup.
}
bool Owner::canRetireNativeSky3F8()const noexcept {
    if(!nativeSky3F8_)return true;
    if(!worldApplication_||worldApplication_->attachmentLock423F58())return false;
    const auto& a=*nativeSky3F8_;if(!a.owner)return true;
    const auto& c=*a.owner;
    if(!canRetireNativeSkyInner128930(c)||!canRetireNativeSkyCommands1298AC(c))return false;
    if(c.published&&(!a.pointers||a.pointers[0]!=&c||!c.constructorComplete))return false;
    if(c.counted&&(c.entity.reference00!=1||c.entity.sessionGeneration!=worldEntity_.sessionGeneration||
       c.entity.objectKey!=key(&c.entity)||c.entity.parent1C||c.entity.previous10||c.entity.next14||
       c.entity.firstChild18||c.entity.mask20||c.entity.descendants24||
       !worldApplication_->entityAuthority_.canRetire(c.entity)||!worldApplication_->objectCount423F60()))return false;
    if(c.palette){const auto& palette=*c.palette;
        if(palette.released_||palette.references00_!=1u||
           (palette.attempted_&&(palette.thread_!=std::this_thread::get_id()||palette.file_!=&c.file||
             c.file.nativeSelectedPalette_!=&palette)))return false;
    }else if(c.palettePublished7D0||c.file.nativeSelectedPalette_)return false;
    return !c.file.attempted_||c.file.thread_==std::this_thread::get_id();
}
bool Owner::retireNativeSky3F8()noexcept {
    if(!canRetireNativeSky3F8())return false;
    if(!nativeSky3F8_)return true;
    auto& a=*nativeSky3F8_;
    if(a.owner){auto& c=*a.owner;std::uint32_t pc=0;
        //Native paused-call cleanup owns reached command/resource allocations.
        //Guest-global destructor effects are not replayed.
        if(!retireNativeSkyCommands1298AC(c))std::terminate();
        //Withdraw temporary native palette borrows before source palette
        //release, without retiring any texture or changing source release order.
        if(c.innerOwner){auto& inner=*c.innerOwner;
            if(inner.partialTexture&&!inner.partialTexture->withdrawCubPaletteBorrow())std::terminate();
            for(auto& face:inner.faces)for(auto& texture:face.textures58)
                if(texture&&!texture->withdrawCubPaletteBorrow())std::terminate();
        }
        if(c.palette){if(!c.palette->release104970(pc))std::terminate();c.palette.reset();}
        c.palettePublished7D0=false;
        if(!retireNativeSkyInner128930(c))std::terminate();
        if(!c.file.close131040())std::terminate();
        c.images=nullptr;c.environmentName=nullptr;
        if(c.counted){if(!worldApplication_->entityAuthority_.retire(c.entity))std::terminate();
            --c.entity.reference00;--worldApplication_->objectCount423F60();c.counted=false;}
        if(c.published)a.pointers[0]=nullptr;
        a.owner.reset();
    }
    a.pointers.reset();nativeSky3F8_.reset();return true;
}
}

namespace ra_cold_aggregate50LA {
bool Owner::continueNativeSky274AE0(const original_main_entry50KG::Application& app,
    std::uint32_t& pc)noexcept {
    pc=0x1aa2e0;
    if(nativeScbThread_!=std::this_thread::get_id()||nativeScbRevoked_||
       !nativeTriggerList3D0_||!nativeTriggerList3D0_->serviceComplete||
       nativeTriggerList3D0_->serviceTerminal||!nativeSceneInfo270900Current(app,pc)||
       !world_||!world_->domain_||!runtimeQueries_||
       !nativeTrackLoadBorrowsAdmitted()||!worldApplication_||
       worldApplication_->attachmentLock423F58()||!source_||!skyImages_||
       !skyImages_->mesh()||!skyImages_->bank()||!app.riderProfile)return false;
    const auto* profile=app.riderProfile->commonProfile();
    if(!profile||!profile->known)return false;
    //Read actual App+2DC Profile byte at the reached call, not cached launch mode.
    const auto bits=profile->playerBits1125C;
    const auto display=runtimeQueries_->nativeColdModelDisplay274160(*worldApplication_,*source_,worldEntity_.sessionGeneration);
    if(!display||!runtimeQueries_->camera_||!runtimeQueries_->camera_->session||
       !nativeWorldPublicationCurrent(*display)||
       !display->nativeSkyMetadataCurrent129280(*runtimeQueries_->camera_->session,
           *skyImages_->mesh(),*skyImages_->bank()))return false;
    if(!beginNativeSkyPalette274AE0(*source_,*skyImages_,app.skyDirectory36B750(),bits,
        world_->domain_->archiveProvider3E1A70(),pc))return false;
    try {
        if(!world_->context_||!world_->context_->live()||world_->domain_!=rosterMountDomain_||
           app.standardMaterialDomain()!=rosterMountDomain_||
           !world_->context_->applicationAnimationClock().sameOwner(app.animationClock()))return false;
        if(!continueNativeSkyTextures129750(world_->context_,pc))return false;
        if(!continueNativeSkyCommands1298AC(world_->context_,pc))return false;
        if(!continueNativeSkyGeometry12A8A0(*skyImages_,pc))return false;
        if(!continueNativeSkyTail129924(world_->domain_->archiveProvider3E1A70(),pc))return false;
        return continueNativeWater274570(app,pc); //Same source-ordered caller, no fullWorld.
    }catch(...){return false;} //Preserve actual reached prefix if source-domain admission throws.
}
}
