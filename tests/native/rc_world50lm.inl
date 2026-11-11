// Proposal only. Intentionally included within an authenticated track profile namespace.
// Shared implementation derived from reviewed TA owner; no runtime file inputs.
struct Diagnostic {
    const char* stage="input";
    std::string member,message;
    std::size_t scenes=0,placements=0,collisionOnly=0,surfaces=0,zeroNormals=0;
    std::size_t materials=0,selections=0,secondPasses=0;
    // A runtime digest records a decoded result; it is not an independent pin.
    std::size_t independentlyPinnedImages=0,runtimeIdentityImages=0;
    a::DecodeError48C decode{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
namespace detail {
inline std::string key(std::string name){return shared::normalized(std::move(name));}
inline Json fields(const a::MaterialFields48E& value){
    Json out={{"authored",Json::array()},{"unspecified",Json::array()}};
    for(const auto* list:{&value.authored,&value.unspecified})for(const auto& item:*list)
        out[list==&value.authored?"authored":"unspecified"].push_back(
            {{"name",item.name},{"value",item.value},{"source_offset",item.source_offset}});
    return out;
}
inline std::string field(const a::MaterialFields48E& value,const char* name){
    const auto wanted=key(name);std::string result;bool found=false;
    for(const auto& item:value.authored)if(key(item.name)==wanted){
        shared::need(!found,"Duplicate original RC material field");found=true;result=item.value;
    }
    return result;
}
inline bool clamp(const a::MaterialFields48E& value,const char* name){
    const auto text=field(value,name);
    shared::need(text.empty()||text=="0"||text=="1","Unsupported original RC texture wrap mode");
    return text=="1";
}
inline a::detail::OriginalIpumMipExtent50LA framePst(const Bytes& source,std::uint32_t& mipIndex){
    const auto range=[&](std::size_t at,std::size_t n){return at<=source.size()&&n<=source.size()-at;};
    const auto word=[&](std::size_t at){shared::need(range(at,4),"Truncated original RC IPUM PST");return shared::u32(source.data()+at);};
    const auto dimension=[](std::uint32_t n){return n>=16&&n<=1024&&!(n&(n-1));};
    shared::need(source.size()<=8u*1024u*1024u&&word(0)==1&&word(4)==2&&word(8)>=1&&word(8)<=5,
        "Original RC IPUM PST header rejected");
    const auto count=word(8);std::size_t at=12;std::uint32_t priorW=0,priorH=0;
    a::detail::OriginalIpumMipExtent50LA largest;
    for(std::uint32_t i=0;i<count;++i){
        shared::need(range(at,20),"Original RC IPUM mip header extent");
        const auto h=word(at),w=word(at+4),bytes=word(at+16);const auto p=at+20;
        shared::need(dimension(w)&&dimension(h)&&!word(at+8)&&!word(at+12)&&
            (!i||(w==priorW*2&&h==priorH*2))&&bytes>=25&&range(p,bytes),"Original RC IPUM mip extent/order");
        shared::need(!std::memcmp(source.data()+p,"ipum",4)&&word(p+4)==bytes-8&&
            (std::uint32_t(source[p+8])|(std::uint32_t(source[p+9])<<8))==w&&
            (std::uint32_t(source[p+10])|(std::uint32_t(source[p+11])<<8))==h&&
            word(p+12)==1&&source[p+16]==0,"Original RC IPUM frame header");
        constexpr std::uint8_t end[]{0,0,1,0xb0,0,0,1,0xb1};
        shared::need(!std::memcmp(source.data()+p+bytes-8,end,8),"Original RC IPUM terminators");
        largest={w,h,at,p,bytes};at=p+bytes;priorW=w;priorH=h;
    }
    shared::need(at==source.size(),"Original RC IPUM PST trailing bytes");mipIndex=count-1;return largest;
}
// Native regression outputs observed in the genuine parent cold run. These
// are not independent decoder or original-hardware pixel evidence.
struct NativeOutputPin {const char* member;const char* sourceSha;const char* outputSha;};
inline constexpr NativeOutputPin nativeOutputPins[]={
{"TEXTURES/RC_TRACK4.PST","3c5df581af47780e26cab024cc348bdcef4aa29415aa4eb2ce898f85a7b0cc37","9d18509b28a7fc4a26ddb6a282d3f9f6e7cf01ba404867faee444a34cf135ba1"},
{"TEXTURES/DARKTUSKS.PST","71102b85f2b220eccb3769a641f60bfc100d0b9c974daf1fe587690c763f622f","c59f772329faa91c70da1f2f4a17583b4afc3047507ee4a3827587253a26f6f0"},
{"TEXTURES/DARKTUSKSDESIGNS.PST","df6fdaceac16e6ab1d941ad20503af50016db62162c96da25834eb57e7414988","744c774b9c56afaca1f19291474ef6ae05813b11a86c157ba3472f02f6d6a38f"},
{"TEXTURES/STAIRS01.PST","b73db8ca765f38950320917f0f7424eb6e7eb805e5448f86a1cc3190c42589ef","7e93f3e59cc5047977d918c3c2dba11f367f42f425ef76666af8f355931beeca"},
{"TEXTURES/BLOCKWALL.PST","21d5056d73e9f59d3a60005f5dbc77f5f07495ef2a0207bee61c2f398d93cc14","46efeae7cc18561a13187bcdb843ce0f0d09a354cdcc9c77245db64029e19f01"},
{"TEXTURES/BUILDING01.PST","5dff386796b0c7261e9e713abca4df0d5c8b4429f6a05f3bdffb0178d0871f52","d04836a14fde9ca0a3986352516824a022c1f6e0a13f4758c8ea545ba1ca7475"},
{"TEXTURES/CROWD.ATM","054ce086e15886a331f0da2fb610479e665413c917a9f9ab634ab133fc582b38","66c8b8d045c25264987772de735e94ede277fa61a058a2d796dcc9be3808d375"},
{"TEXTURES/GARGOYLEBODY.PST","a72f848331bae9be135ce772a35a69bdaf1bda6e98de69b3ae7e589430a25f43","fb1e80499b37564b563797023d60501df171de8441b1896c7779fed64e4b4866"},
{"TEXTURES/LARGEPILLARBASE02.PST","d7b76af0e1fe6feccb3840a236c04b6e3b1569139e3b14e1edc2e507b3df8b84","5a0c2e2d7d9438469e02f16325c91afa4c0abb288b95241253fd4a0ce814da9a"},
{"TEXTURES/LIGHTINGRIGLIGHTBOTTOM.PST","f3ae291f8887019c0098cdf50de287f226b9e9c37bb3bacd8b6abe4b47dfd0f9","cc2de92395f1ef8fb70f759ba7c8a9573b118d3e56220899a526a388b6a00220"},
{"TEXTURES/LIGHTINGRIGLIGHTSIDE.PST","bde4f1e341110fa77daa3c8957ba3d2391c7acd2dac9a331c2f3fc1bf2d89e0e","47f2a6e4ab2bafece2c6bbfc0927c37f39596d7f07898aeee3e8207dce2fa6ca"},
{"TEXTURES/LIGHTINGRIGSIDE.PST","59f11ee833cda543ed5a1eadebe7806b5af40db9d8105f16714d9215fe2fffba","d83c56467f491f635606e38b5b3d182a3ca007135b1f5bfae1a9d783731f688e"},
{"TEXTURES/TWILEKFLAG01.PST","ba9fc527c7549f3907fdbbeb0350a79f88cbdd49998f2b00718239e6906716a5","48844cafb0a1df0b28d132c8d3a3cf92fdb79bf92b5ae380e601a16ee66e9b92"},
{"TEXTURES/RC_SAND02.PST","0c3a5abd729f999debeb00e9adc90ae9516522ac6508a3d439bb5b27de32c8a9","16a15a0686c2342af5b0800dc0431d231d1d055b99796a8dd0ae11a99edf8717"},
{"TEXTURES/RC_NEWROCK.PST","186200a3f27b2eb8a7f0f1b9e16ed0733bf74a7e48bda4c0f32c77b7b386245b","f63bedcc18af499a126bfed42b1cebb0af36db7667740d608d25cd498bed79d4"},
{"TEXTURES/RC_WALLSIDE01.PST","b1a619c80a83278753be90d617bd4a9c2020e02b75adb108334c69a18565c607","6cb25914b352cf4687a6fb767161971fa244b0310f53d0ebc1c85cd62de56b61"},
{"TEXTURES/RC_WALLBLEND.PST","f5ab8d212661e31c6fc87994212cdaa02c50e38c5c36a12b8ab2dfa593426cbe","d4c38de985f62a16b72c36de68fae0bd762dfec810a77ab84b51a318f203948e"},
{"TEXTURES/ARROWDROID03.TGA","efe13aee3c3ab36aeb8b9208fc0794d85b3056c77ff9017517c621aea8d64d15","324209b05299eb49f17e9e12a6f1d688cc4d98baf900cd2a74d55dcd57cdcd91"},
{"TEXTURES/ARROWDROID01.TGA","06e5ceebbefa498778fabb8f975b919b77262f2c9d38b643df971d7ddc864faf","daf1e95effcba5ad5cf7d2b4e0aa3e7a945e7d7efd5c208675c724748b6f61ac"},
{"TEXTURES/CORD.PST","91cb74edd2d0cb4f3ca102abea2f06cc5e539f21f6b5fb42b54d42baafdc39a1","ce5f2c98e85fb10437ac76198e9186d2782f10163717fd563c6fb2e4accb670b"},
{"TEXTURES/RC_CLIFF01.PST","0c9ebd69892d5030c3d68fd6fd0069981b2f0be137a8c8cbb828dae06e56bdcb","743f93d40749f72d49f6fa62984fe9b017478fecaa40d923f0266c31949e9634"},
{"TEXTURES/SPECMAP5.TGA","0c9838ec2d3a1936b6dc1a5e218accfd29f47307c5a7d9801614a2d1858bc7ea","167b19b138d8417cbcdad37513f43a6346ae86ddf44706c91603ef980ac2462a"},
{"TEXTURES/PLANTSTEM.TGA","f84e083c073aa756b3739acf7e1d05630d4098a1a3f51b15c6612cb6e409ed2d","70c22dcaf5b4f3f71d78eb872efa25967b557067d0ab0501a50f4e0a646c9f9e"},
{"TEXTURES/PLANTTOP.TGA","0deb48ef798cdb2d1587eec48bd47258b4249ff843982894a05aeb72dd0f56d1","fbbaee67d3a55136f5aa1f4e7f097902c0122694b32aae6970633ea5c0d94b19"},
{"TEXTURES/PLANTTOP02.TGA","618bb0cca7f95193a43708bb0438734aca844eef2acd2b9e3dbefd0a21d44dd9","00148dec1e94638cb9114eaf778796cbc16e790fb3dc1d2832958eb3159c6a50"},
{"TEXTURES/GARGOYLEFACE.PST","a6e8af21befe0a4e39e2ba6a3272a8eb1f66e9f923a0b268fbcaf4dbe48b710a","1714be905f0a034e00a488ed9293125588e36fc73c3a29c195cbd8512ab6fc03"},
};
inline void checkNativeOutput(const std::string& member,const Bytes& source,const Bytes& output){
    const auto normalized=key(member);
    for(const auto& pin:nativeOutputPins)if(normalized==pin.member){
        shared::need(shared::sha256(source)==pin.sourceSha&&shared::sha256(output)==pin.outputSha,
            "Original RC native regression output differs");return;
    }
    shared::need(false,"Original RC native regression output pin missing");
}
inline const SourceImage* sourceImage(const std::string& member){
    const auto name=key(member);const SourceImage* found=nullptr;
    for(const auto& entry:sourceImages)if(key(entry.member)==name){
        shared::need(!found,"Ambiguous original RC source image pin");found=&entry;
    }
    return found;
}
inline std::uint32_t initialFrame(const a::MaterialFields48E& value){
    const auto text=field(value,"animatedMapInitialFrame");if(text.empty())return 0;
    std::uint32_t result=0;const auto parsed=std::from_chars(text.data(),text.data()+text.size(),result);
    shared::need(parsed.ec==std::errc{}&&parsed.ptr==text.data()+text.size()&&result<64,
        "Original RC ATM initial frame syntax/extent");return result;
}
// Decode only supplied original bytes. The optional output records whether an
// already retained independent image hash was available and checked. Ordinary
// image hashes are recorded after decoding, never used to select pixel data.
inline Bytes image(const std::string& member,const Bytes& source,std::uint32_t& mipIndex,
    bool& animated,a::DecodeError48C& error,bool* independentlyPinned=nullptr){
    shared::need(source.size()>=12&&source.size()<=8u*1024u*1024u,"Original RC texture source extent");
    const auto name=key(member);const auto* pin=sourceImage(name);
    const source_texture50fc_fixture::detail::Entry* outputPin=nullptr;
    for(const auto& entry:exceptionalImages)if(key(entry.member)==name){
        shared::need(!outputPin,"Ambiguous original RC decoded image pin");outputPin=&entry;
    }
    for(const auto& entry:indexedPins)if(key(entry.member)==name){
        if(outputPin)shared::need(std::string(outputPin->outputSha256)==entry.outputSha256,"RC independent pixel pin disagreement");
        outputPin=&entry;
    }
    shared::need(pin||outputPin,"Original RC image has no authenticated source identity");
    const auto sourceSha=shared::sha256(source);
    if(pin)shared::need(source.size()==pin->sourceBytes&&sourceSha==pin->sourceSha256,
        "Original RC static image source identity differs");
    if(outputPin)shared::need(source.size()==outputPin->sourceBytes&&sourceSha==outputPin->sourceSha256,
        "Original RC exceptional image source identity differs");
    if(name.ends_with(".TGA")){
        shared::need(pin&&pin->kind==3&&pin->mipIndex==0&&pin->frameCount==1&&pin->initialFrame==0,
            "Original RC TGA exact source profile");
        using Convert=a::detail::OriginalTgaConversion50LA;
        a::TextureMip48E image;image.width=pin->width;image.height=pin->height;
        const auto depth=source[16];
        if(depth==24){
            Bytes rgb;std::vector<std::uint16_t> words(std::size_t(pin->width)*pin->height);
            shared::need(Convert::read24_29EA90(source,pin->width,pin->height,rgb)&&
                Convert::convert1555_16C0D0(rgb,pin->width,pin->height,words),"Original RC BGR24 conversion");
            image.rgba.resize(words.size()*4);
            for(std::size_t i=0;i<words.size();++i){const auto word=words[i];
                image.rgba[4*i]=static_cast<std::uint8_t>((word&31u)<<3);
                image.rgba[4*i+1]=static_cast<std::uint8_t>(((word>>5)&31u)<<3);
                image.rgba[4*i+2]=static_cast<std::uint8_t>(((word>>10)&31u)<<3);
                image.rgba[4*i+3]=255; // Exact inherited TEXA255/255,AEM0 for these four authenticated consumers.
            }
        }else shared::need(depth==32&&Convert::read32_29EE40(source,pin->width,pin->height,image.rgba),
            "Original RC BGRA32 source-alpha conversion");
        Bytes output;shared::need(a::encodeTextureLevel48E(image,output,error),"Original RC TGA native framing");
        checkNativeOutput(member,source,output);
        animated=false;mipIndex=0;if(independentlyPinned)*independentlyPinned=false;return output;
    }
    animated=name.ends_with(".ATM");
    shared::need(animated||name.ends_with(".PST"),"Unsupported original RC image extension");
    shared::need(!animated||(pin&&source.size()>=24&&pin->initialFrame<pin->frameCount),
        "Original RC ATM lacks its exact frame selection");
    const auto frameIndex=pin?pin->initialFrame:0u;
    const auto kind=shared::u32(source.data()+(animated?16:4));
    if(pin)shared::need(kind==pin->kind,"Original RC source image kind differs");
    Bytes output;
    if(kind==2){
        a::detail::OriginalIpumMipExtent50LA mip;
        if(animated){
            std::vector<a::detail::OriginalIpumFrameExtent50LA> frames;std::size_t bad=0;
            shared::need(a::detail::frameOriginalIpumAtm50LA(source,frames,bad)&&
                frames.size()==pin->frameCount&&frameIndex<frames.size(),
                "Original RC IPUM ATM framing failed at "+std::to_string(bad));
            const auto& frame=frames[frameIndex];mipIndex=frame.count-1;mip=frame.mips[mipIndex];
        }else mip=framePst(source,mipIndex);
        a::TextureMip48E decoded;decoded.width=mip.width;decoded.height=mip.height;std::string why;
        shared::need(a::decodeOriginalIpuRgba(source.data()+mip.payload,mip.bytes,mip.width,mip.height,decoded.rgba,why),
            "Original RC IPUM decode: "+why);
        shared::need(a::encodeTextureLevel48E(decoded,output,error),"Original RC IPUM output framing: "+error.code);
    }else{
        shared::need(kind==0||kind==1,"Original RC indexed texture kind");Bytes pst;
        if(animated){
            a::OriginalUiAtm50KG frames;
            shared::need(a::splitOriginalUiAtm50KG(source.data(),source.size(),frames,error)&&
                frames.frames.size()==pin->frameCount&&frameIndex<frames.frames.size(),
                "Original RC indexed ATM framing: "+error.code);
            const auto& frame=frames.frames[frameIndex];
            pst.assign(source.begin()+frame.pstOffset,source.begin()+frame.pstOffset+frame.pstBytes);
        }
        a::Texture48E decoded;
        shared::need(a::decodeTexture48E(animated?pst:source,decoded,error)&&!decoded.mips.empty(),
            "Original RC indexed PST decode: "+error.code);
        mipIndex=static_cast<std::uint32_t>(decoded.mips.size()-1);output=std::move(decoded.base_rrtex);
    }
    const auto w=pin?pin->width:outputPin->width,h=pin?pin->height:outputPin->height;
    const auto expectedMip=pin?pin->mipIndex:outputPin->mipIndex;
    shared::need(mipIndex==expectedMip&&output.size()==std::size_t(w)*h*4+16&&
        !std::memcmp(output.data(),"RRTEX001",8)&&shared::u32(output.data()+8)==w&&shared::u32(output.data()+12)==h,
        "Original RC decoded image source framing differs");
    if(outputPin)shared::need(mipIndex==outputPin->mipIndex&&w==outputPin->width&&h==outputPin->height&&
        shared::sha256(output)==outputPin->outputSha256,"Original RC independently pinned decoded image differs");
    if(!outputPin)checkNativeOutput(member,source,output);
    if(independentlyPinned)*independentlyPinned=outputPin!=nullptr;
    return output;
}
// The retained original byte buffer alone does not authenticate mutable parsed
// fields or their cached typed order. Re-encode the entire parameter document,
// then compare every typed/source-provenance field with a fresh exact decode.
inline void validateBorrowedWorld(const Bytes& scb,const a::World48F& world,a::DecodeError48C& error){
    Bytes roundtrip;a::World48F canonical;
    shared::need(world.parameters.original_bytes==scb&&
        a::encodeParameters48F(world.parameters,roundtrip,error)&&roundtrip==scb&&
        a::decodeWorld48F(scb,canonical,error)&&a::validateWorldSemantics48F(world,error),
        "Original RC borrowed world fields differ from authenticated SCB");
    const auto& actual=world.parameters;const auto& source=canonical.parameters;
    shared::need(actual.version==source.version&&actual.header_table_offset==source.header_table_offset&&
        actual.preservesDuplicateLabels==source.preservesDuplicateLabels&&actual.blocks.size()==source.blocks.size(),
        "Original RC parameter provenance header differs");
    for(std::size_t i=0;i<source.blocks.size();++i){
        const auto& x=actual.blocks[i];const auto& y=source.blocks[i];
        shared::need(x.name==y.name&&x.name_offset==y.name_offset&&x.table_entry_offset==y.table_entry_offset&&
            x.offset==y.offset&&x.size_without_count==y.size_without_count&&x.fields.size()==y.fields.size(),
            "Original RC parameter block provenance differs");
        for(std::size_t j=0;j<y.fields.size();++j){const auto& f=x.fields[j];const auto& g=y.fields[j];
            shared::need(f.name==g.name&&f.value==g.value&&f.entry_offset==g.entry_offset&&
                f.value_offset==g.value_offset&&f.value_bytes==g.value_bytes,
                "Original RC parameter field provenance differs");
        }
    }
    shared::need(world.scene_info_block==canonical.scene_info_block&&world.instances.size()==expectedInstances&&
        world.instances.size()==canonical.instances.size()&&world.zones.size()==expectedZones&&
        world.zones.size()==canonical.zones.size()&&world.portals.size()==expectedPortals&&world.portals.size()==canonical.portals.size(),
        "Original RC typed world source extent differs");
    for(std::size_t i=0;i<canonical.zones.size();++i){const auto& x=world.zones[i];const auto& y=canonical.zones[i];
        shared::need(x.zone==y.zone&&x.models==y.models&&x.parameter_block==y.parameter_block,
            "Original RC typed zone order/provenance differs");
    }
    const auto same=[](const auto& x,const auto& y){
        return !std::memcmp(x.data(),y.data(),sizeof(x[0])*x.size());
    };
    for(std::size_t i=0;i<canonical.instances.size();++i){const auto& x=world.instances[i];const auto& y=canonical.instances[i];
        shared::need(x.section==y.section&&x.geometry_member==y.geometry_member&&x.collision_member==y.collision_member&&
            x.zone==y.zone&&x.ordinal==y.ordinal&&x.parameter_block==y.parameter_block&&
            x.source_block_offset==y.source_block_offset&&x.use_lighting==y.use_lighting&&
            same(x.position,y.position)&&same(x.look,y.look)&&same(x.up,y.up)&&same(x.matrix,y.matrix),
            "Original RC typed instance order/provenance differs");
    }
    for(std::size_t i=0;i<canonical.portals.size();++i){const auto& x=world.portals[i];const auto& y=canonical.portals[i];
        shared::need(x.section==y.section&&x.parameter_block==y.parameter_block&&x.zones==y.zones,
            "Original RC typed portal order/provenance differs");
        for(std::size_t j=0;j<y.vertices.size();++j)shared::need(same(x.vertices[j],y.vertices[j]),
            "Original RC typed portal vertices differ");
    }
}
} // namespace detail

// Compatible direct-source callback for independently owned RC material consumers.
// Never reads a retained image filename; failures preserve the caller's image.
inline bool decodeSourceTexture(const char* member,const Bytes& source,Bytes& output,
    std::uint32_t& mipIndex,std::string& error)noexcept {
    try{
        shared::need(member&&*member,"Original RC source texture member required");
        a::DecodeError48C diagnostic;std::uint32_t mip=0;bool animated=false;
        auto next=detail::image(member,source,mip,animated,diagnostic);
        output=std::move(next);mipIndex=mip;error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="Original RC source texture decode failed";}catch(...){}return false;}
}

class Owner final {
    struct Scene {a::Scene48D decoded;std::uint32_t crystalSource=~0u;std::uint64_t identity=0;std::vector<std::uint32_t> materials;};
    struct Storage {
        std::shared_ptr<const rc_cold_sources50LA::Owner> source;
        std::vector<CrystalSource> crystalSources;
        std::vector<CrystalPlacement> crystalPlacements;
        std::size_t rawPayloadBytes=0;
        std::map<std::string,std::size_t> entries;
        std::map<std::string,Scene> scenes;
        std::vector<a::Material48E> materials;
        std::vector<std::vector<std::uint32_t>> materialPasses;
        std::vector<std::uint8_t> independentlyPinnedImages;
        a::Mesh48B mesh;
        std::vector<a::WorldNormal50HK> normals;

        a::MaterialTextureAssets50EA bank;
        std::vector<tm::Span> spans;
        std::vector<tm::Selection> selections;
        std::vector<tm::TextureSelection> textures;
        std::shared_ptr<a::OriginalStandardMaterialDomain50LA> canonicalDomain;
        bool canonicalAttempted=false;
        const a::ResourceEntry48C* entry(const std::string& name)const{
            const auto found=entries.find(detail::key(name));
            return found==entries.end()?nullptr:&source->archive().entries()[found->second];
        }
        Bytes member(const a::ResourceEntry48C& entry,a::DecodeError48C& error)const{
            Bytes bytes;shared::need(entry.bytes&&entry.bytes<=8u*1024u*1024u&&
                source->extractMember(entry.name,bytes,error,entry.bytes)&&bytes.size()==entry.bytes,
                "Original RC archive member extraction: "+entry.name+" "+error.code);return bytes;
        }
    };
    std::unique_ptr<Storage> storage_;
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    bool loaded()const noexcept{return bool(storage_);}
    const a::Mesh48B* mesh()const noexcept{return storage_?&storage_->mesh:nullptr;}
    const std::vector<a::WorldNormal50HK>* normals()const noexcept{return storage_?&storage_->normals:nullptr;}
    const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
    const std::vector<tm::Span>* spans()const noexcept{return storage_?&storage_->spans:nullptr;}
    const std::vector<tm::Selection>* selections()const noexcept{return storage_?&storage_->selections:nullptr;}
    const std::vector<tm::TextureSelection>* textures()const noexcept{return storage_?&storage_->textures:nullptr;}
    const std::vector<a::Material48E>* materials()const noexcept{return storage_?&storage_->materials:nullptr;}
    const std::vector<std::uint8_t>* independentlyPinnedImages()const noexcept{return storage_?&storage_->independentlyPinnedImages:nullptr;}
    const std::vector<CrystalSource>* crystalSources()const noexcept{return storage_?&storage_->crystalSources:nullptr;}
    const std::vector<CrystalPlacement>* crystalPlacements()const noexcept{return storage_?&storage_->crystalPlacements:nullptr;}
    std::shared_ptr<const rc_cold_sources50LA::Owner> sourceOwner()const noexcept{return storage_?storage_->source:nullptr;}
    std::size_t rawPayloadBytes()const noexcept{return storage_?storage_->rawPayloadBytes:0;}

    // Incremental crystal payload only; source, ordinary scene/mesh/image storage
    // and allocator bookkeeping are separate existing owner charges.
    static constexpr std::size_t additionalRawReservationBytes()noexcept{
        // Each crystal identity: member name and 64-hex sha as std::string. The
        // string allocator may round capacity up (MSVC: 64 -> 79), so 128 each.
        return 4*sizeof(CrystalSource)+79*sizeof(CrystalPlacement)+4*(128+128)+
            776*sizeof(rr::game::original_track_environment50RC::Vertex)+
            430*sizeof(a::SceneRawVertex48D)+sizeof(a::SceneRawSurface48D);
    }
    bool load(std::shared_ptr<const rc_cold_sources50LA::Owner> sourceOwner,std::size_t availableRawPayloadBytes,Diagnostic& d)noexcept{
        try{
            d={};shared::need(!storage_&&std::fegetround()==FE_TONEAREST,"RC direct cold owner already loaded or rounding mode differs");
            shared::need(sourceOwner&&sourceOwner->ready(),"Original RC shared cold source absent");
            shared::need(availableRawPayloadBytes>=additionalRawReservationBytes(),"RC raw metadata reservation unavailable");
            auto next=std::make_unique<Storage>();next->source=std::move(sourceOwner);d.stage="archive";
            next->crystalSources.reserve(4);next->crystalPlacements.reserve(79);
            shared::need(next->crystalSources.capacity()==4&&next->crystalPlacements.capacity()==79,
                "RC crystal descriptor allocation exceeds exact reservation");
            next->rawPayloadBytes=next->crystalSources.capacity()*sizeof(CrystalSource)+
                next->crystalPlacements.capacity()*sizeof(CrystalPlacement);
            const auto& archive=next->source->archive();const auto& world=next->source->world();
            shared::need(archive.entries().size()==entryCount&&world.instances.size()==expectedInstances&&
                world.zones.size()==expectedZones&&world.portals.size()==expectedPortals&&
                world.parameters.original_bytes.size()==scbBytesCount&&shared::sha256(world.parameters.original_bytes)==scbSha,
                "Original RC exact shared source/world identity");
            for(std::size_t i=0;i<archive.entries().size();++i)
                shared::need(next->entries.emplace(detail::key(archive.entries()[i].name),i).second,"Ambiguous original RC member");
            std::map<std::string,std::uint32_t> materialIndices,textureIndices;
            std::vector<Bytes> images;
            std::vector<a::MaterialTextureSlotInput50EA> slots;
            constexpr std::uint64_t textureBase=assetIdentity<<48;
            std::size_t rgbaBytes=0,serializedBytes=0;
            auto material=[&](const std::string& materialName)->std::uint32_t{
                const auto normalized=detail::key(materialName);const auto existing=materialIndices.find(normalized);
                if(existing!=materialIndices.end())return existing->second;
                shared::need(shared::name(materialName),"Invalid original RC material basename");
                d.stage="material";const auto* psm=next->entry("textures\\"+materialName+".psm");
                shared::need(psm,"Original RC world material absent: "+materialName);d.member=psm->name;
                const auto source=next->member(*psm,d.decode);a::Material48E authored;
                const bool decoded=a::decodeMaterial48E(source,authored,d.decode);
                shared::need(decoded&&!authored.passes.empty()&&authored.passes.size()==(normalized=="CRYSTAL01"?2u:1u),
                    "Original RC material/program pass count");
                const auto materialIndex=static_cast<std::uint32_t>(next->materials.size());
                shared::need(materialIndex<expectedMaterials,"Original RC PSM count bound");
                const auto materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+shared::sha256(source));
                std::vector<std::uint32_t> selectedPasses;
                for(std::size_t pi=0;pi<authored.passes.size();++pi){
                    const auto& pass=authored.passes[pi];const bool projected=false;
                    shared::need(pass.pass==pi+1&&pass.textures.size()==1&&pass.textures[0].slot==1&&
                        (detail::key(pass.microprogram)=="PRELITCOMPRESSED"||
                         (normalized=="CRYSTAL01"&&pi==1&&pass.microprogram=="EnvironmentMapCompressed")),
                        "Unjoined original RC pass/program/texture subset");
                    const auto& map=pass.textures[0];shared::need(shared::name(map.name),"Invalid original RC texture basename");
                    const auto pstCandidate=map.name+".pst";
                    const auto dot=pstCandidate.find_last_of('.');
                    shared::need(dot!=std::string::npos,"RC source texture candidate lacks extension");
                    const auto stem=pstCandidate.substr(0,dot);
                    const a::ResourceEntry48C* texture=nullptr;
                    for(const char* extension:{".pst",".ipu",".tga",".atm"}){
                        texture=next->entry("textures\\"+stem+extension);if(texture)break;
                    }
                    shared::need(texture,"Original RC required texture absent: "+map.name);
                    const auto textureKey=detail::key(texture->name);const auto* pin=detail::sourceImage(textureKey);
                    shared::need(pin,"Original RC static image lacks exact source pin");
                    const bool animated=textureKey.ends_with(".ATM");
                    if(animated)shared::need(authored.passes.size()==1&&detail::initialFrame(pass.fields)==pin->initialFrame,
                        "Original RC material/ATM initial frame differs");
                    auto found=textureIndices.find(textureKey);
                    if(found==textureIndices.end()){
                        d.stage="texture";d.member=texture->name;const auto bytes=next->member(*texture,d.decode);
                        std::uint32_t mip=0;bool decodedAnimated=false,independent=false;
                        auto image=detail::image(texture->name,bytes,mip,decodedAnimated,d.decode,&independent);
                        shared::need(decodedAnimated==animated,"Original RC texture animation source classification");
                        const auto index=static_cast<std::uint32_t>(images.size());
                        shared::need(index<expectedImages&&image.size()>=16,"RC original texture bank bound");
                        const auto w=shared::u32(image.data()+8),h=shared::u32(image.data()+12);
                        shared::need(w&&h&&w<=1024&&h<=1024&&image.size()==std::size_t(w)*h*4+16,"RC native image extent");
                        shared::add(rgbaBytes,std::size_t(w)*h*4,16u*1024u*1024u,"RC texture RGBA budget");
                        shared::add(serializedBytes,image.size(),16u*1024u*1024u,"RC texture framing budget");
                        tm::TextureSelection selected;selected.identity=textureBase+index+1;selected.bankIndex=index;
                        selected.mipIndex=mip;selected.width=w;selected.height=h;selected.path=texture->name;
                        selected.key="disc:"+textureKey+":"+pin->sourceSha256;
                        selected.sha256=shared::sha256(image); // Actual runtime output identity, not necessarily an independent pin.
                        next->textures.push_back(std::move(selected));images.push_back(std::move(image));
                        next->independentlyPinnedImages.push_back(independent?1:0);
                        if(independent)++d.independentlyPinnedImages;else ++d.runtimeIdentityImages;
                        found=textureIndices.emplace(textureKey,index).first;
                    }
                    const auto index=static_cast<std::uint32_t>(next->selections.size());
                    shared::need(index<expectedSelections,"Original RC authored pass count bound");
                    const auto& image=next->textures[found->second];tm::Selection selected;
                    selected.assetIdentity=assetIdentity;selected.materialIndex=materialIndex;selected.pass=pass.pass;selected.slot=1;
                    selected.bankSlot=static_cast<std::uint32_t>(slots.size());selected.textureIndex=found->second;selected.mipIndex=image.mipIndex;
                    selected.materialName=materialName;selected.materialKey=materialKey;
                    selected.program=pass.microprogram;selected.textureName=map.name;selected.textureKey=image.key;
                    selected.track=selectedTrack;selected.passCount=static_cast<std::uint32_t>(authored.passes.size());
                    selected.clampU=detail::clamp(pass.fields,"wrapModeHorizontal");selected.clampV=detail::clamp(pass.fields,"wrapModeVertical");
                    selected.sourcePassFields=detail::fields(pass.fields);selected.staticAnimationFrame=animated;
                    selected.projectedPrelit=projected;
                    if(projected)selected.texGenMatrix=tm::detail::projection(selected.sourcePassFields);
                    selected.decodedSourceMember=texture->name;selected.decodedSourceSha256=pin->sourceSha256;
                    slots.push_back({{selected.assetIdentity,materialIndex,pass.pass,1},image.identity,a::MaterialTextureRequirement50EA::required});
                    selectedPasses.push_back(index);next->selections.push_back(std::move(selected));
                }
                next->materials.push_back(std::move(authored));next->materialPasses.push_back(std::move(selectedPasses));
                materialIndices.emplace(normalized,materialIndex);return materialIndex;
            };
            constexpr std::array<double,16> identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
            for(std::size_t wi=0;wi<world.instances.size();++wi){
                const auto& placement=world.instances[wi];d.stage="scene";d.member=placement.geometry_member;
                const auto* entry=next->entry(placement.geometry_member);
                if(!entry){shared::need(next->entry(placement.collision_member),"RC placement lacks both original PSG and COL");++d.collisionOnly;continue;}
                const auto name=detail::key(entry->name);auto found=next->scenes.find(name);
                if(found==next->scenes.end()){
                    Scene scene;scene.identity=next->scenes.size()+1;
                    const auto originalScene=next->member(*entry,d.decode);
                    a::SceneLimits48D sceneLimits;sceneLimits.allow_signed32_quantized_positions=true;
                    const detail::CrystalPin* crystal=nullptr;
                    for(const auto& pin:detail::crystalPins)if(detail::key(pin.member)==name){shared::need(!crystal,"Duplicate crystal source pin");crystal=&pin;}
                    if(crystal){
                        shared::need(originalScene.size()==crystal->bytes&&shared::sha256(originalScene)==crystal->sha,
                            "Original RC crystal source identity");
                        a::SceneRawCapture48D raw;
                        const auto captureBytes=std::size_t(crystal->vertices)*sizeof(a::SceneRawVertex48D)+sizeof(a::SceneRawSurface48D);
                        shared::need(next->rawPayloadBytes<=availableRawPayloadBytes&&captureBytes<=availableRawPayloadBytes-next->rawPayloadBytes,
                            "Original RC crystal raw capture available payload");
                        a::SceneRawCaptureLimits48D captureLimits{crystal->vertices,1,captureBytes};
                        shared::need(a::decodeSceneWithRaw48D(originalScene,scene.decoded,raw,captureLimits,d.decode,nullptr,sceneLimits),
                            "Original RC captured PSG decode: "+entry->name+" "+d.decode.code);
                        shared::need(!scene.decoded.world_space&&!scene.decoded.identity_matrix_palette&&scene.decoded.nodes.size()==1&&
                            scene.decoded.lods.size()==1&&scene.decoded.lods[0].surfaces.size()==1&&raw.surfaces.size()==1&&
                            raw.vertices.size()==crystal->vertices&&raw.surfaces[0].lod==0&&raw.surfaces[0].surface==0&&
                            scene.decoded.quantization[4]==0&&scene.decoded.quantization[5]==0&&scene.decoded.quantization[6]==0&&
                            static_cast<float>(scene.decoded.quantization[0])==crystal->qPosition&&
                            static_cast<float>(scene.decoded.quantization[1])==0.007874015718698502f,
                            "Original RC crystal exact local source shape");
                        shared::need(next->crystalSources.size()<4,"RC crystal source descriptor overflow");
                        CrystalSource retained;retained.member=entry->name;retained.sourceSha=crystal->sha;
                        const auto identityBytes=retained.member.capacity()+1+retained.sourceSha.capacity()+1;
                        shared::need(retained.member.capacity()+1<=128&&retained.sourceSha.capacity()+1<=128&&
                            identityBytes<=availableRawPayloadBytes-next->rawPayloadBytes,
                            "RC crystal identity allocation exceeds reservation");
                        next->rawPayloadBytes+=identityBytes;
                        retained.qPosition=static_cast<float>(scene.decoded.quantization[0]);retained.qNormal=static_cast<float>(scene.decoded.quantization[1]);
                        const auto retainedBytes=std::size_t(crystal->vertices)*sizeof(rr::game::original_track_environment50RC::Vertex);
                        shared::need(raw.retainedPayloadBytes<=availableRawPayloadBytes-next->rawPayloadBytes&&
                            retainedBytes<=availableRawPayloadBytes-next->rawPayloadBytes-raw.retainedPayloadBytes,
                            "Original RC raw transfer simultaneous payload");
                        retained.vertices.reserve(crystal->vertices);
                        for(const auto& v:raw.vertices)retained.vertices.push_back({v.position,v.normal});
                        const auto actualBytes=retained.vertices.capacity()*sizeof(rr::game::original_track_environment50RC::Vertex);
                        shared::need(actualBytes<=availableRawPayloadBytes-next->rawPayloadBytes-raw.retainedPayloadBytes,
                            "Original RC retained raw actual capacity");
                        next->rawPayloadBytes+=actualBytes;
                        scene.crystalSource=static_cast<std::uint32_t>(next->crystalSources.size());next->crystalSources.push_back(std::move(retained));
                    }else shared::need(a::decodeScene48D(originalScene,scene.decoded,d.decode,nullptr,sceneLimits),
                        "Original RC PSG decode: "+entry->name+" "+d.decode.code);
                    shared::need(scene.decoded.nodes.size()==1&&scene.decoded.nodes[0].parent==-1&&
                        !scene.decoded.identity_matrix_palette&&!scene.decoded.lods.empty(),"Original RC static hierarchy subset");
                    for(const auto& name:scene.decoded.materials)scene.materials.push_back(material(name));
                    found=next->scenes.emplace(name,std::move(scene)).first;
                }
                const auto& scene=found->second;const auto& matrix=scene.decoded.world_space?identity:placement.matrix;
                a::WorldNormalTransform50HK normalTransform;
                shared::need(a::prepareWorldNormalTransform50HK(matrix,normalTransform),"Original RC visual/normal transform");
                for(std::size_t si=0;si<scene.decoded.lods.front().surfaces.size();++si){
                    const auto& surface=scene.decoded.lods.front().surfaces[si];d.stage="surface";d.member=entry->name;
                    shared::need(surface.material_index<scene.materials.size()&&!surface.vertices.empty()&&!surface.indices.empty()&&
                        !(surface.indices.size()%3)&&surface.vertices.size()<=a::max_mesh_vertices-next->mesh.vertices.size()&&
                        surface.indices.size()<=a::max_mesh_indices-next->mesh.indices.size(),"RC original surface/material/geometry bounds");
                    tm::Span span;span.vertexStart=static_cast<std::uint32_t>(next->mesh.vertices.size());
                    span.vertexCount=static_cast<std::uint32_t>(surface.vertices.size());span.indexStart=static_cast<std::uint32_t>(next->mesh.indices.size());
                    span.indexCount=static_cast<std::uint32_t>(surface.indices.size());span.worldInstanceIndex=static_cast<std::uint32_t>(wi);
                    span.surfaceIndex=static_cast<std::uint32_t>(si);span.hasOriginalColors=surface.has_original_colors;
                    const auto& selectedPasses=next->materialPasses.at(scene.materials[surface.material_index]);
                    shared::need(!selectedPasses.empty()&&selectedPasses.size()<=(scene.crystalSource==~0u?1u:2u),"Original RC surface material pass closure");
                    for(const auto& source:surface.vertices){
                        shared::need(source.bone==0,"RC original static vertex bone");a::Vertex48B vertex{};
                        for(unsigned axis=0;axis<3;++axis){double value=0;
                            for(unsigned k=0;k<3;++k)value+=source.position_local[k]*matrix[k*4+axis];value+=matrix[12+axis];
                            vertex.position[axis]=static_cast<float>(value);shared::need(std::isfinite(value)&&std::isfinite(vertex.position[axis]),"RC nonfinite original vertex");
                        }
                        for(unsigned axis=0;axis<2;++axis){vertex.uv[axis]=static_cast<float>(source.uv[axis]);shared::need(std::isfinite(vertex.uv[axis]),"RC nonfinite original UV");}
                        for(unsigned axis=0;axis<4;++axis)vertex.color[axis]=surface.has_original_colors?source.color[axis]:255;
                        a::WorldNormal50HK normal;shared::need(a::transformPreparedWorldNormal50HK(source.normal_local,normalTransform,normal),"RC original normal transform");
                        if(!normal.xyz[0]&&!normal.xyz[1]&&!normal.xyz[2])++d.zeroNormals;
                        next->mesh.vertices.push_back(vertex);next->normals.push_back(normal);
                    }
                    for(const auto index:surface.indices){shared::need(index<surface.vertices.size(),"RC original surface index");next->mesh.indices.push_back(span.vertexStart+index);}
                    if(scene.crystalSource!=~0u){
                        shared::need(selectedPasses.size()==2&&si==0&&
                            next->crystalSources[scene.crystalSource].vertices.size()==span.vertexCount,
                            "Original RC crystal exact pass/range association");
                        CrystalPlacement association;association.sourceIndex=scene.crystalSource;
                        association.firstSpan=static_cast<std::uint32_t>(next->spans.size());association.vertexStart=span.vertexStart;
                        association.vertexCount=span.vertexCount;association.indexStart=span.indexStart;association.indexCount=span.indexCount;
                        association.matrix=matrix; // Exact same retained double placement used for native mesh expansion.
                        shared::need(next->crystalPlacements.size()<79,"RC crystal placement descriptor overflow");
                        next->crystalPlacements.push_back(association);
                    }
                    // Duplicate draw spans only. Vertex/index storage stays shared.
                    for(const auto selectedPass:selectedPasses){
                        const auto& selection=next->selections[selectedPass];
                        span.materialSelection=selectedPass;span.textureIndex=selection.textureIndex;
                        span.sourcePass=selection.pass;span.sourcePassCount=selection.passCount;
                        span.projectionPass=selection.projectedPrelit?selection.pass:0;
                        next->spans.push_back(span);if(selection.pass==2)++d.secondPasses;
                    }
                    ++d.surfaces;
                }
                ++d.placements;
            }
            d.scenes=next->scenes.size();d.stage="closure";d.member.clear();
            d.materials=next->materials.size();d.selections=next->selections.size();
            shared::need(d.scenes==expectedScenes&&d.placements==expectedPlacements&&d.collisionOnly==expectedCollisionOnly&&d.surfaces==expectedSurfaces&&
                d.materials==expectedMaterials&&d.selections==expectedSelections&&d.secondPasses==expectedSecondPasses&&next->spans.size()==expectedSurfaces+expectedSecondPasses&&
                next->mesh.vertices.size()==expectedVertices&&next->mesh.indices.size()==expectedIndices&&images.size()==expectedImages&&
                next->normals.size()==next->mesh.vertices.size()&&
                next->crystalSources.size()==4&&next->crystalPlacements.size()==79&&rgbaBytes==expectedRgbaBytes&&d.independentlyPinnedImages==expectedPinnedImages&&d.runtimeIdentityImages==expectedImages-expectedPinnedImages,
                "Original RC full static source closure differs");
            for(unsigned axis=0;axis<3;++axis){next->mesh.bounds.minimum[axis]=std::numeric_limits<float>::infinity();
                next->mesh.bounds.maximum[axis]=-std::numeric_limits<float>::infinity();}
            for(const auto& vertex:next->mesh.vertices)for(unsigned axis=0;axis<3;++axis){
                next->mesh.bounds.minimum[axis]=std::min(next->mesh.bounds.minimum[axis],vertex.position[axis]);
                next->mesh.bounds.maximum[axis]=std::max(next->mesh.bounds.maximum[axis],vertex.position[axis]);}
            std::vector<a::MaterialTextureAssetInput50EA> textureInput;
            for(std::size_t i=0;i<images.size();++i)textureInput.push_back({next->textures[i].identity,&images[i]});
            a::MaterialTextureAssetsInput50EA input;input.textures=textureInput.data();input.textureCount=static_cast<std::uint32_t>(textureInput.size());
            input.slots=slots.data();input.slotCount=static_cast<std::uint32_t>(slots.size());
            input.budgets={expectedImages,expectedSelections,16u*1024u*1024u,16u*1024u*1024u,1024};
            d.stage="pixel_bank";shared::need(next->bank.load(input,d.bank),"Original RC pixel bank rejected: "+d.bank.parserMessage);
            storage_=std::move(next);d.stage="complete";return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Original RC native cold preparation failed";}catch(...){}return false;}
    }

    // Same Application-owned domain as the actual installed racers. This does
    // not register synthetic static models or claim source render dispatch for
    // the native world above. Mounting the real complete RC namespace preserves
    // the provider's local resource lookup before genuine racer population.
    // Runs inside populateCanonicalRacers after the racer sources are admitted
    // and before the single population, as on every consumed course: the
    // original race load 1A8A40 (Pause100 / Results104 / F8).
    std::function<bool()> beforeRacerPopulation;

    bool populateCanonicalRacers(const tm::Request& request,std::size_t availableAdditionalPayloadBytes,tm::Diagnostic& d)noexcept{
        try{
            d={};shared::need(storage_&&!storage_->canonicalAttempted,"RC canonical population requires fresh native source owner");
            const bool canonical=bool(request.canonicalDomain);
            shared::need(canonical==bool(request.canonicalContext)&&canonical==(request.canonicalMaterialPath.actualPrefix36B970!=nullptr),
                "RC canonical Application/Video/path must be supplied together");
            if(!canonical){shared::need(request.canonicalRacerModels.empty(),"RC racer sources lack their canonical domain");return true;}
            shared::need(request.track==selectedTrack&&request.canonicalDomain->prepared()&&
                request.canonicalContext->live()&&*request.canonicalMaterialPath.actualPrefix36B970=="textures\\",
                "RC canonical racers require their actual gameplay directory and live Video");
            storage_->canonicalAttempted=true;storage_->canonicalDomain=request.canonicalDomain;
            const auto& provider=request.canonicalDomain->archiveProvider3E1A70();
            a::OriginalStandardArchiveProvider50LA::Diagnostic mounted;d.stage="rc_original_archive_mount";
            shared::need(provider&&storage_->source->mountCanonicalArchive(*provider,availableAdditionalPayloadBytes,mounted)&&
                provider->sealNativeMounts(mounted)&&request.canonicalDomain->resolver3E1A70()->bindProvider3E1A70(provider,d.canonical.texture),
                "Original RC canonical provider mount rejected");
            std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials;
            std::vector<a::OriginalStandardTextureSourceInput50LA> textures;
            d.stage="rc_original_racer_sources";
            for(const auto& racer:request.canonicalRacerModels)
                shared::need(racer.resource&&racer.references&&racer.references->table382B38()==request.canonicalDomain->materials382B38().get()&&
                    original_racer_material_sources50LA::appendModelSources(*racer.resource,*racer.references,
                        *request.canonicalMaterialPath.actualPrefix36B970,*provider,materials,textures,d.racerSource),
                    "Original RC canonical racer material/texture closure rejected");
            shared::need(request.canonicalDomain->appendOriginalSources(materials,textures,d.canonical),"Original RC canonical racer source admission rejected");
            if(beforeRacerPopulation){
                d.stage="rc_original_race_load";
                shared::need(beforeRacerPopulation(),"Original RC race load before population rejected");
            }
            d.stage="rc_original_racer_population";
            shared::need(request.canonicalDomain->populate(request.canonicalContext,d.canonical),"Original RC canonical racer population rejected");
            return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Original RC canonical racer preparation failed";}catch(...){}return false;}
    }

    // Preserve the authenticated RC material-keyed authored-pass alpha policy.
    bool alpha(std::size_t selection,const a::OriginalUiMaterialTemplates50KG& templates,
        const a::OriginalUiMaterialRuntime50KG& runtime,material_alpha50gx_fixture::Policy& output)const noexcept{
        try{return storage_&&selection<storage_->selections.size()&&
            material_alpha50gx_fixture::selectRc(storage_->selections[selection],templates,runtime,output);}
        catch(...){return false;}
    }
    // Borrow the same authenticated archive for a separate RC cold decoder. The
    // archive lifetime is the world lifetime and cannot be replaced by reload.
    const a::IndexedResArchive48C* archive()const noexcept{return storage_?&storage_->source->archive():nullptr;}
};

