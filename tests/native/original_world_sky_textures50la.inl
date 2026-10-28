namespace ra_pod_scene50LA {
bool Owner::continueNativeSkyTextures129750(
    std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> context,
    std::uint32_t& pc)noexcept {
    pc=0x129750;
    if(!nativeSky3F8_||!nativeSky3F8_->owner||!context||!context->live()||
       !worldApplication_||worldApplication_->attachmentLock423F58())return false;
    auto& cube=*nativeSky3F8_->owner;
    if(!cube.prefixComplete||cube.tailTerminal||!cube.images||
       !cube.images->storage_||!cube.images->storage_->nativeRaProof)return false;
    if(cube.resourceTerminal)return false;
    if(cube.tailEntered){
        if(!cube.tailComplete||cube.palette||cube.palettePublished7D0||!cube.file.closed_||
           !cube.resourceComplete)return false;
    }else if(!cube.palettePublished7D0||!cube.palette||!cube.palette->current()||!cube.file.current())return false;
    if(cube.resourceComplete){
        if(!cube.innerOwner||!cube.innerOwner->texturesComplete||!canRetireNativeSkyInner128930(cube))return false;
        for(const auto& face:cube.innerOwner->faces)for(const auto& texture:face.textures58)
            if(texture&&!texture->cubSourceCurrent1295B0())return false;
        pc=0x1298ac;return true;
    }
    if(cube.resourceEntered)return false;
    cube.resourceEntered=true;cube.resourceTerminal=true;
    try {
        cube.count40=0;
        pc=0x129758;cube.array3C=std::unique_ptr<NativeSkyInner128930*[]>(new NativeSkyInner128930*[1]);
        std::uint32_t offset=0;pc=0x129784;
        if(!cube.file.readWord(offset))return false;cube.innerOffset=offset;
        if(offset!=99100u)return false;
        std::size_t saved=0;pc=0x1297a0;
        if(!cube.file.tell(saved)||saved!=16u)return false;
        cube.innerSavedCursor=static_cast<std::uint32_t>(saved);
        pc=0x1297c0;if(!cube.file.seek(offset))return false;
        pc=0x1297d0;cube.innerOwner=std::make_unique<NativeSkyInner128930>();
        auto& inner=*cube.innerOwner;
        //1048F0 actual reference owner, never another Entity/count/mesh.
        pc=0x128880;
        inner.position10=worldApplication_->skyOrigin3E0900_;
        inner.look20=worldApplication_->skyOrigin3E0900_;
        inner.up30=worldApplication_->skyOrigin3E0900_;
        for(auto& face:inner.faces){
            face.indicesD8.fill(-1);face.valuesF8.fill(-1);
        }
        inner.constructed=true;
        pc=0x1289e0;inner.file40=&cube.file;inner.file40KnownInvalid=false;
        pc=0x128a00;if(!cube.file.readWord(inner.format48)||inner.format48!=32u)return false;
        const auto readHalf=[&](std::uint16_t& out)noexcept{
            std::array<std::uint8_t,2> bytes;
            if(!cube.file.read(bytes.data(),bytes.size()))return false;
            out=static_cast<std::uint16_t>(bytes[0]|(std::uint16_t(bytes[1])<<8));return true;
        };
        std::uint16_t headerFlags=0;
        pc=0x128a24;if(!readHalf(headerFlags))return false;inner.headerFlags=headerFlags;
        if(headerFlags!=0u)return false; //Actual full-document optional branches absent.
        pc=0x128a48;if(!readHalf(inner.value4E)||inner.value4E!=0u)return false;
        for(std::size_t i=0;i<6u;++i){pc=0x128a78;
            if(!readHalf(inner.faces[i].mask54)||inner.faces[i].mask54!=(i<4u?255u:i==4u?65535u:0u))return false;
        }
        for(std::size_t i=0;i<6u;++i){pc=0x128ab8;
            if(!cube.file.readWord(inner.faces[i].sizes50)||
               inner.faces[i].sizes50!=(i<4u?0xaaaau:i==4u?0xaaaaaaaau:0u))return false;
        }
        //128AEC/128B10/128B34 overwrite only XYZ. W is actual startup1,
        //not a fabricated reader write or newly imposed homogeneous value.
        for(auto* vector:{&inner.position10,&inner.look20,&inner.up30}){
            pc=vector==&inner.position10?0x128aec:vector==&inner.look20?0x128b10:0x128b34;
            std::array<std::uint8_t,12> bytes;
            if(!cube.file.read(bytes.data(),bytes.size()))return false;
            for(std::size_t i=0;i<3u;++i){const auto at=i*4u;
                (*vector)[i]=std::uint32_t(bytes[at])|(std::uint32_t(bytes[at+1u])<<8)|
                    (std::uint32_t(bytes[at+2u])<<16)|(std::uint32_t(bytes[at+3u])<<24);
            }
        }
        if(inner.position10!=std::array<std::uint32_t,4>{{0u,0u,0u,0x3f800000u}}||
           inner.look20!=std::array<std::uint32_t,4>{{0u,0u,0x3f800000u,0x3f800000u}}||
           inner.up30!=std::array<std::uint32_t,4>{{0u,0x3f800000u,0u,0x3f800000u}})return false;
        pc=0x128c6c;std::size_t tileOffset=0;
        if(!cube.file.tell(tileOffset)||tileOffset!=99180u)return false;
        inner.tileOffset650=static_cast<std::uint32_t>(tileOffset);inner.readerComplete=true;
        pc=0x129800;cube.array3C[0]=&inner;cube.innerPublished=true;
        pc=0x12982c;if(!cube.file.seek(*cube.innerSavedCursor))return false;
        //12B750 selects this exact inner owner, then1290E0 traverses masks.
        pc=0x12b750;cube.inner38=&inner;
        pc=0x129118;if(!cube.file.seek(inner.tileOffset650))return false;
        inner.texturesEntered=true;
        const auto& proof=*cube.images->storage_->nativeRaProof;
        std::size_t ordinal=0;
        for(std::uint32_t face=0;face<6u;++face)for(std::uint32_t tile=0;tile<16u;++tile){
            if(!(inner.faces[face].mask54&(1u<<tile)))continue;
            if(ordinal>=proof.tiles.size()||inner.faces[face].textures58[tile])return false;
            const auto& tileProof=proof.tiles[ordinal];
            if(tileProof.face!=face||tileProof.tile!=tile||
               tileProof.image!=proof.bank->texture(static_cast<std::uint32_t>(ordinal))||
               cube.file.cursor_!=tileProof.offset)return false;
            std::uint32_t descriptor=0;pc=0x128e4c;
            if(!cube.file.readWord(descriptor))return false;
            using Resource=rr::assets::OriginalStandardTextureResource50LA;
            Resource::CubSource1295B0 cap;
            cap.sourcePin=proof.source;cap.provider=cube.file.provider_;
            cap.member=cube.file.member_.member;cap.memberData=cube.file.member_.data;
            cap.memberBytes=cube.file.member_.size;cap.mount=cube.file.member_.mount;
            cap.ordinal=cube.file.member_.ordinal;cap.providerRevision=cube.file.member_.revision;
            cap.bankGeneration=cube.file.member_.bankGeneration;
            cap.authenticatedMemberData=proof.memberData;cap.imageOwner=cube.images;
            cap.bank=proof.bank;cap.image=tileProof.image;cap.palette=cube.palette.get();
            cap.tileOrdinal=ordinal;cap.tileOffset=tileProof.offset;cap.tileBytes=tileProof.bytes;
            cap.face=face;cap.tile=tile;cap.descriptorWord0=descriptor;
            pc=0x128e74;inner.partialTexture=Resource::allocate();
            rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
            if(!inner.partialTexture->constructCub24E1B0(cap,context,diagnostic)){
                pc=diagnostic.sourcePC;return false;
            }
            pc=0x128e9c;inner.faces[face].textures58[tile]=std::move(inner.partialTexture);
            ++inner.texturesPublished;
            auto& texture=*inner.faces[face].textures58[tile];
            std::array<std::uint32_t,5> descriptorTail{};
            constexpr std::array<std::uint32_t,5> reads{{0x128ee0u,0x128efcu,0x128f18u,0x128f30u,0x128f50u}};
            for(std::size_t word=0;word<descriptorTail.size();++word){
                pc=reads[word];if(!cube.file.readWord(descriptorTail[word]))return false;
            }
            //All authenticated rows have B8=4096 and bypass29E9F0.128F80's
            //B8=B4 local store is not a passed pointer/operand of24F2C0;
            //descriptorTail remains the exact raw-reader receipt, not that local.
            pc=0x128fc8;
            if(!texture.loadCub24F2C0(cube.file.cursor_,descriptorTail,diagnostic)){
                pc=diagnostic.sourcePC;return false;
            }
            pc=0x12904c;
            if(!texture.finishCub2516F0(1u,0u,0u,diagnostic)){
                pc=diagnostic.sourcePC;return false;
            }
            ++inner.texturesLoaded;++ordinal;
        }
        if(ordinal!=48u||inner.texturesPublished!=48u||inner.texturesLoaded!=48u||
           cube.file.cursor_!=cube.file.member_.size)return false;
        inner.texturesComplete=true;
        pc=0x129890;cube.count40=0;
        //1298B0 delay-slot selects the SAME first inner owner before12A120.
        cube.inner38=cube.array3C[cube.count40];
        cube.resourceComplete=true;cube.resourceTerminal=false;
        pc=0x1298ac;return true; //BEFORE12A120, no command-buffer or1295B0 return.

    }catch(...){return false;} //Retain every reached actual owner and source effect.
}
}
namespace ra_pod_scene50LA {
bool Owner::canRetireNativeSkyInner128930(const NativeSkyCube288A50& cube)const noexcept {
    if(!cube.innerOwner)return !cube.innerPublished&&!cube.inner38;
    const auto& inner=*cube.innerOwner;
    if(inner.references00!=1u||(cube.innerPublished&&(!cube.array3C||cube.array3C[0]!=&inner||
       !inner.readerComplete))||(cube.inner38&&cube.inner38!=&inner)||
       (inner.file40&&inner.file40!=&cube.file)||inner.texturesLoaded>inner.texturesPublished||
       inner.texturesPublished>48u)return false;
    std::size_t owned=0;
    for(const auto& face:inner.faces)for(const auto& texture:face.textures58)if(texture){
        if(texture.use_count()!=1u||!texture->canRetireCub24E370())return false;++owned;
    }
    if(owned!=inner.texturesPublished)return false;
    return !inner.partialTexture||(inner.partialTexture.use_count()==1u&&
        inner.partialTexture->canRetireCub24E370());
}
bool Owner::retireNativeSkyInner128930(NativeSkyCube288A50& cube)noexcept {
    if(!canRetireNativeSkyInner128930(cube))return false;
    if(!cube.innerOwner){cube.array3C.reset();return true;}
    auto& inner=*cube.innerOwner;
    //A constructor that did not return has native exceptional custody, not a
    //published face slot; withdraw its actual ownership before that owner dies.
    if(inner.partialTexture){
        if(!inner.partialTexture->withdrawCubPaletteBorrow()||
           !inner.partialTexture->release104970())std::terminate();
        inner.partialTexture.reset();
    }
    //1291B0's source traversal is face5..0 and tile15..0, not allocation order.
    for(std::size_t face=6u;face-->0;)for(std::size_t tile=16u;tile-->0;){
        auto& texture=inner.faces[face].textures58[tile];if(!texture)continue;
        if(!texture->withdrawCubPaletteBorrow()||!texture->release104970())std::terminate();
        texture.reset();--inner.texturesPublished;
    }
    inner.file40=nullptr;--inner.references00;
    cube.inner38=nullptr;if(cube.innerPublished)cube.array3C[0]=nullptr;
    cube.innerPublished=false;cube.innerOwner.reset();cube.array3C.reset();return true;
}
}
