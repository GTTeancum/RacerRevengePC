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
// The owner's course identity (146r). Default = this namespace's stock row: its
// pins, counts and SHAs are receipts (stock=1 on the stock bytes). A registry
// course on the common route (registryIdentity) forms every member name from its
// own code, carries its LISTED archive SHA/bytes, and has no stock row: no pin
// tables, no stock counts (receipts log stock_row=none / stock=0).
// PORT EXTENSION, NOT ORIGINAL for the registry row; MA only uses its stock row.
struct Identity {
    std::string code{stockCode};           // upper-case course code (log lines/receipts)
    std::string container{archiveMember};  // "TRACKS/XX.RES": provider mount name
    std::string scb{scbMember};            // "TRACKS/XX.SCB"
    std::string sha{archiveSha};           // stock SHA, or the registry row's listed SHA
    std::size_t bytes=archiveBytes;
    std::uint64_t asset=assetIdentity;     // texture identity namespace (asset<<48)
    track_source_fixture::Track track=selectedTrack;
    bool stockRow=true;                    // stock pins/counts exist for this row
    // inherit.particles (registry row only; PORT EXTENSION, NOT ORIGINAL): the
    // particle closure members (particle\demoprt.prt/.pst/.psm) of the stock
    // course archive the row names, mounted beside the course archive in the
    // SAME native namespace before the seal, so the particle owner and the
    // canonical material domain authenticate them like members of the course
    // archive. Name/SHA/bytes are the stock archive's own (read from the install).
    std::string particleContainer,particleSha;
    std::size_t particleBytes=0;
    std::shared_ptr<const a::ResArchive48C> particleArchive;
};
// Row of a course whose code is not stock: names spelled as the stock row spells
// its own ("TRACKS/SA.RES" -> "TRACKS/NV.RES"); asset identity = the two code
// bytes, the rule every stock owner uses (MA 0x4d41, SA 0x5341, ...), so a
// non-stock code cannot collide with a stock one. false on a malformed code.
inline bool registryIdentity(const char* code,const char* listedSha,std::size_t listedBytes,Identity& out){
    if(!code||!code[0]||!code[1]||code[2]||!listedSha)return false;
    std::string upper(code,2);
    for(auto& c:upper){if(c>='a'&&c<='z')c=static_cast<char>(c-'a'+'A');
        if(!((c>='A'&&c<='Z')||(c>='0'&&c<='9')))return false;}
    Identity next;next.code=upper;next.container="TRACKS/"+upper+".RES";next.scb="TRACKS/"+upper+".SCB";
    next.sha=listedSha;next.bytes=listedBytes;
    next.asset=(std::uint64_t(std::uint8_t(upper[0]))<<8)|std::uint8_t(upper[1]);
    next.track=track_source_fixture::Track::course;next.stockRow=false;
    out=std::move(next);return true;
}
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
        shared::need(!found,"Duplicate original MA/SA material field");found=true;result=item.value;
    }
    return result;
}
inline bool clamp(const a::MaterialFields48E& value,const char* name){
    const auto text=field(value,name);
    shared::need(text.empty()||text=="0"||text=="1","Unsupported original MA/SA texture wrap mode");
    return text=="1";
}
inline a::detail::OriginalIpumMipExtent50LA framePst(const Bytes& source,std::uint32_t& mipIndex){
    const auto range=[&](std::size_t at,std::size_t n){return at<=source.size()&&n<=source.size()-at;};
    const auto word=[&](std::size_t at){shared::need(range(at,4),"Truncated original MA/SA IPUM PST");return shared::u32(source.data()+at);};
    const auto dimension=[](std::uint32_t n){return n>=16&&n<=1024&&!(n&(n-1));};
    shared::need(source.size()<=8u*1024u*1024u&&word(0)==1&&word(4)==2&&word(8)>=1&&word(8)<=5,
        "Original MA/SA IPUM PST header rejected");
    const auto count=word(8);std::size_t at=12;std::uint32_t priorW=0,priorH=0;
    a::detail::OriginalIpumMipExtent50LA largest;
    for(std::uint32_t i=0;i<count;++i){
        shared::need(range(at,20),"Original MA/SA IPUM mip header extent");
        const auto h=word(at),w=word(at+4),bytes=word(at+16);const auto p=at+20;
        shared::need(dimension(w)&&dimension(h)&&!word(at+8)&&!word(at+12)&&
            (!i||(w==priorW*2&&h==priorH*2))&&bytes>=25&&range(p,bytes),"Original MA/SA IPUM mip extent/order");
        shared::need(!std::memcmp(source.data()+p,"ipum",4)&&word(p+4)==bytes-8&&
            (std::uint32_t(source[p+8])|(std::uint32_t(source[p+9])<<8))==w&&
            (std::uint32_t(source[p+10])|(std::uint32_t(source[p+11])<<8))==h&&
            word(p+12)==1&&source[p+16]==0,"Original MA/SA IPUM frame header");
        constexpr std::uint8_t end[]{0,0,1,0xb0,0,0,1,0xb1};
        shared::need(!std::memcmp(source.data()+p+bytes-8,end,8),"Original MA/SA IPUM terminators");
        largest={w,h,at,p,bytes};at=p+bytes;priorW=w;priorH=h;
    }
    shared::need(at==source.size(),"Original MA/SA IPUM PST trailing bytes");mipIndex=count-1;return largest;
}
inline const SourceImage* sourceImage(const std::string& member){
    const auto name=key(member);const SourceImage* found=nullptr;
    for(const auto& entry:sourceImages)if(key(entry.member)==name){
        shared::need(!found,"Ambiguous original MA/SA source image pin");found=&entry;
    }
    return found;
}
// Common course-archive rule (course_archive_receipt50ci.h): a stock pin is
// the stock identity of a member NAME. When the bytes under that name are the
// stock bytes the pin still verifies exactly; when they differ (a converted
// archive reusing a generic stock name) the pin is a receipt and the image is
// admitted like any unpinned image, from its own data.
// image() logs the receipt; the cold loader asks again without logging.
inline const SourceImage* matchedSourceImage(const std::string& member,const Bytes& source,
    const std::string& sourceSha,bool logReceipt=true){
    const auto* pin=sourceImage(member);
    if(pin&&!(source.size()==pin->sourceBytes&&sourceSha==pin->sourceSha256)){
        if(logReceipt)course_archive_receipt50CI::memberReceipt("static_image_pin",member,source,0,0,
            pin->sourceBytes,pin->sourceSha256);
        pin=nullptr;
    }
    return pin;
}
inline std::uint32_t initialFrame(const a::MaterialFields48E& value){
    const auto text=field(value,"animatedMapInitialFrame");if(text.empty())return 0;
    std::uint32_t result=0;const auto parsed=std::from_chars(text.data(),text.data()+text.size(),result);
    shared::need(parsed.ec==std::errc{}&&parsed.ptr==text.data()+text.size()&&result<64,
        "Original MA/SA ATM initial frame syntax/extent");return result;
}
// Decode only supplied original bytes. The optional output records whether an
// already retained independent image hash was available and checked. Ordinary
// image hashes are recorded after decoding, never used to select pixel data.
// stockPins=false (a row without a stock row): no pin table is consulted; every
// image is admitted from its own data with its runtime identity.
inline Bytes image(const std::string& member,const Bytes& source,std::uint32_t& mipIndex,
    bool& animated,a::DecodeError48C& error,bool* independentlyPinned=nullptr,std::uint32_t authoredFrame=0,
    bool stockPins=true){
    shared::need(source.size()>=12&&source.size()<=8u*1024u*1024u,"Original MA/SA texture source extent");
    const auto name=key(member);const auto sourceSha=shared::sha256(source);
    const auto* pin=stockPins?matchedSourceImage(name,source,sourceSha):nullptr;
    const source_texture50fc_fixture::detail::Entry* outputPin=nullptr;
    if(stockPins)for(const auto& entry:exceptionalImages)if(key(entry.member)==name){
        shared::need(!outputPin,"Ambiguous original MA/SA decoded image pin");outputPin=&entry;
    }
    if(stockPins)for(const auto& entry:indexedPins)if(key(entry.member)==name){
        if(outputPin)shared::need(std::string(outputPin->outputSha256)==entry.outputSha256,"MA/SA independent pixel pin disagreement");
        outputPin=&entry;
    }
    // No pin: decoded and admitted with its runtime identity (counted by the loader). Pins still verify exactly
    // when the bytes are the stock bytes; a pin whose bytes differ is a receipt (matchedSourceImage).
    if(outputPin&&!(source.size()==outputPin->sourceBytes&&sourceSha==outputPin->sourceSha256)){
        course_archive_receipt50CI::memberReceipt("decoded_image_pin",name,source,0,0,
            outputPin->sourceBytes,outputPin->sourceSha256);
        outputPin=nullptr;
    }
    animated=name.ends_with(".ATM");
    shared::need(animated||name.ends_with(".PST"),"Unsupported original MA/SA image extension");
    shared::need(!animated||(source.size()>=24&&(!pin||pin->initialFrame<pin->frameCount)),
        "Original MA/SA ATM lacks its exact frame selection");
    const auto frameIndex=pin?pin->initialFrame:authoredFrame; // unpinned ATM: the frame the material authors
    const auto kind=shared::u32(source.data()+(animated?16:4));
    if(pin)shared::need(kind==pin->kind,"Original MA/SA source image kind differs");
    Bytes output;
    if(kind==2){
        a::detail::OriginalIpumMipExtent50LA mip;
        if(animated){
            std::vector<a::detail::OriginalIpumFrameExtent50LA> frames;std::size_t bad=0;
            shared::need(a::detail::frameOriginalIpumAtm50LA(source,frames,bad)&&
                (!pin||frames.size()==pin->frameCount)&&frameIndex<frames.size(),
                "Original MA/SA IPUM ATM framing failed at "+std::to_string(bad));
            const auto& frame=frames[frameIndex];mipIndex=frame.count-1;mip=frame.mips[mipIndex];
        }else mip=framePst(source,mipIndex);
        a::TextureMip48E decoded;decoded.width=mip.width;decoded.height=mip.height;std::string why;
        shared::need(a::decodeOriginalIpuRgba(source.data()+mip.payload,mip.bytes,mip.width,mip.height,decoded.rgba,why),
            "Original MA/SA IPUM decode: "+why);
        shared::need(a::encodeTextureLevel48E(decoded,output,error),"Original MA/SA IPUM output framing: "+error.code);
    }else{
        shared::need(kind==0||kind==1,"Original MA/SA indexed texture kind");Bytes pst;
        if(animated){
            a::OriginalUiAtm50KG frames;
            shared::need(a::splitOriginalUiAtm50KG(source.data(),source.size(),frames,error)&&
                (!pin||frames.frames.size()==pin->frameCount)&&frameIndex<frames.frames.size(),
                "Original MA/SA indexed ATM framing: "+error.code);
            const auto& frame=frames.frames[frameIndex];
            pst.assign(source.begin()+frame.pstOffset,source.begin()+frame.pstOffset+frame.pstBytes);
        }
        a::Texture48E decoded;
        shared::need(a::decodeTexture48E(animated?pst:source,decoded,error)&&!decoded.mips.empty(),
            "Original MA/SA indexed PST decode: "+error.code);
        mipIndex=static_cast<std::uint32_t>(decoded.mips.size()-1);output=std::move(decoded.base_rrtex);
    }
    std::uint32_t w=0,h=0,expectedMip=mipIndex;
    if(pin){w=pin->width;h=pin->height;expectedMip=pin->mipIndex;}
    else if(outputPin){w=outputPin->width;h=outputPin->height;expectedMip=outputPin->mipIndex;}
    else shared::need(wma50CM::decodedExtent(output,w,h),"Unpinned original image decoded framing"); // runtime identity
    shared::need(mipIndex==expectedMip&&output.size()==std::size_t(w)*h*4+16&&
        !std::memcmp(output.data(),"RRTEX001",8)&&shared::u32(output.data()+8)==w&&shared::u32(output.data()+12)==h,
        "Original MA/SA decoded image source framing differs");
    if(outputPin)shared::need(mipIndex==outputPin->mipIndex&&w==outputPin->width&&h==outputPin->height&&
        shared::sha256(output)==outputPin->outputSha256,"Original MA/SA independently pinned decoded image differs");
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
        "Original MA/SA borrowed world fields differ from authenticated SCB");
    const auto& actual=world.parameters;const auto& source=canonical.parameters;
    shared::need(actual.version==source.version&&actual.header_table_offset==source.header_table_offset&&
        actual.preservesDuplicateLabels==source.preservesDuplicateLabels&&actual.blocks.size()==source.blocks.size(),
        "Original MA/SA parameter provenance header differs");
    for(std::size_t i=0;i<source.blocks.size();++i){
        const auto& x=actual.blocks[i];const auto& y=source.blocks[i];
        shared::need(x.name==y.name&&x.name_offset==y.name_offset&&x.table_entry_offset==y.table_entry_offset&&
            x.offset==y.offset&&x.size_without_count==y.size_without_count&&x.fields.size()==y.fields.size(),
            "Original MA/SA parameter block provenance differs");
        for(std::size_t j=0;j<y.fields.size();++j){const auto& f=x.fields[j];const auto& g=y.fields[j];
            shared::need(f.name==g.name&&f.value==g.value&&f.entry_offset==g.entry_offset&&
                f.value_offset==g.value_offset&&f.value_bytes==g.value_bytes,
                "Original MA/SA parameter field provenance differs");
        }
    }
    shared::need(world.scene_info_block==canonical.scene_info_block&&
        world.instances.size()==canonical.instances.size()&&
        world.zones.size()==canonical.zones.size()&&world.portals.size()==canonical.portals.size(),
        "Original MA/SA typed world source extent differs");
    for(std::size_t i=0;i<canonical.zones.size();++i){const auto& x=world.zones[i];const auto& y=canonical.zones[i];
        shared::need(x.zone==y.zone&&x.models==y.models&&x.parameter_block==y.parameter_block,
            "Original MA/SA typed zone order/provenance differs");
    }
    const auto same=[](const auto& x,const auto& y){
        return !std::memcmp(x.data(),y.data(),sizeof(x[0])*x.size());
    };
    for(std::size_t i=0;i<canonical.instances.size();++i){const auto& x=world.instances[i];const auto& y=canonical.instances[i];
        shared::need(x.section==y.section&&x.geometry_member==y.geometry_member&&x.collision_member==y.collision_member&&
            x.zone==y.zone&&x.ordinal==y.ordinal&&x.parameter_block==y.parameter_block&&
            x.source_block_offset==y.source_block_offset&&x.use_lighting==y.use_lighting&&
            same(x.position,y.position)&&same(x.look,y.look)&&same(x.up,y.up)&&same(x.matrix,y.matrix),
            "Original MA/SA typed instance order/provenance differs");
    }
    for(std::size_t i=0;i<canonical.portals.size();++i){const auto& x=world.portals[i];const auto& y=canonical.portals[i];
        shared::need(x.section==y.section&&x.parameter_block==y.parameter_block&&x.zones==y.zones,
            "Original MA/SA typed portal order/provenance differs");
        for(std::size_t j=0;j<y.vertices.size();++j)shared::need(same(x.vertices[j],y.vertices[j]),
            "Original MA/SA typed portal vertices differ");
    }
}
} // namespace detail

// Compatible direct-source callback for independently owned MA/SA material consumers.
// Never reads a retained image filename; failures preserve the caller's image.
inline bool decodeSourceTexture(const char* member,const Bytes& source,Bytes& output,
    std::uint32_t& mipIndex,std::string& error)noexcept {
    try{
        shared::need(member&&*member,"Original MA/SA source texture member required");
        a::DecodeError48C diagnostic;std::uint32_t mip=0;bool animated=false;
        auto next=detail::image(member,source,mip,animated,diagnostic);
        output=std::move(next);mipIndex=mip;error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="Original MA/SA source texture decode failed";}catch(...){}return false;}
}

class Owner final {
    struct Scene {a::Scene48D decoded;std::uint64_t identity=0;std::vector<std::uint32_t> materials;};
    struct Storage {
        a::ResArchive48C archive;
        std::map<std::string,std::size_t> entries;
        std::map<std::string,Scene> scenes;
        std::vector<a::Material48E> materials;
        std::vector<std::vector<std::uint32_t>> materialPasses;
        std::vector<std::uint8_t> independentlyPinnedImages;
        a::Mesh48B mesh;
        std::vector<a::WorldNormal50HK> normals;
        std::vector<a::TrackProjectedUv> projectionUvs;
        a::MaterialTextureAssets50EA bank;
        std::vector<tm::Span> spans;
        std::vector<tm::Selection> selections;
        std::vector<tm::TextureSelection> textures;
        std::shared_ptr<a::OriginalStandardMaterialDomain50LA> canonicalDomain;
        bool canonicalAttempted=false;
        const a::ResourceEntry48C* entry(const std::string& name)const{
            const auto found=entries.find(detail::key(name));
            return found==entries.end()?nullptr:&archive.entries[found->second];
        }
        Bytes member(const a::ResourceEntry48C& entry,a::DecodeError48C& error)const{
            Bytes bytes;shared::need(entry.bytes&&entry.bytes<=8u*1024u*1024u&&
                a::extractResMember48C(archive,entry.name,bytes,error,entry.bytes)&&bytes.size()==entry.bytes,
                "Original MA/SA archive member extraction: "+entry.name+" "+error.code);return bytes;
        }
    };
    std::unique_ptr<Storage> storage_;
    Identity identity_{};
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    explicit Owner(Identity identity):identity_(std::move(identity)){}
    const Identity& identity()const noexcept{return identity_;}
    bool loaded()const noexcept{return bool(storage_);}
    const a::Mesh48B* mesh()const noexcept{return storage_?&storage_->mesh:nullptr;}
    const std::vector<a::WorldNormal50HK>* normals()const noexcept{return storage_?&storage_->normals:nullptr;}
    const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
    const std::vector<tm::Span>* spans()const noexcept{return storage_?&storage_->spans:nullptr;}
    const std::vector<tm::Selection>* selections()const noexcept{return storage_?&storage_->selections:nullptr;}
    const std::vector<tm::TextureSelection>* textures()const noexcept{return storage_?&storage_->textures:nullptr;}
    const std::vector<a::Material48E>* materials()const noexcept{return storage_?&storage_->materials:nullptr;}
    const std::vector<std::uint8_t>* independentlyPinnedImages()const noexcept{return storage_?&storage_->independentlyPinnedImages:nullptr;}
    const std::vector<a::TrackProjectedUv>* projectionUvs()const noexcept{return storage_?&storage_->projectionUvs:nullptr;}

    bool load(const Bytes& original,const a::World48F& world,Diagnostic& d)noexcept;

    // Runs inside populateCanonicalRacers after the racer sources are admitted
    // and before the single population, as on TA/GA: the original race load
    // 1A8A40 (Pause100 / Results104 / F8) runs for every race.
    std::function<bool()> beforeRacerPopulation;

    // Same Application-owned domain as the actual installed racers. This does
    // not register synthetic static models or claim source render dispatch for
    // the native world above. Mounting the real complete MA/SA namespace preserves
    // the provider's local resource lookup before genuine racer population.
    bool populateCanonicalRacers(const tm::Request& request,tm::Diagnostic& d)noexcept{
        try{
            d={};shared::need(storage_&&!storage_->canonicalAttempted,"MA/SA canonical population requires fresh native source owner");
            const bool canonical=bool(request.canonicalDomain);
            shared::need(canonical==bool(request.canonicalContext)&&canonical==(request.canonicalMaterialPath.actualPrefix36B970!=nullptr),
                "MA/SA canonical Application/Video/path must be supplied together");
            if(!canonical){shared::need(request.canonicalRacerModels.empty(),"MA/SA racer sources lack their canonical domain");return true;}
            shared::need(request.track==identity_.track&&request.canonicalDomain->prepared()&&
                request.canonicalContext->live()&&*request.canonicalMaterialPath.actualPrefix36B970=="textures\\",
                "MA/SA canonical racers require their actual gameplay directory and live Video");
            storage_->canonicalAttempted=true;storage_->canonicalDomain=request.canonicalDomain;
            const auto& provider=request.canonicalDomain->archiveProvider3E1A70();
            a::OriginalStandardArchiveProvider50LA::Diagnostic mounted;d.stage="ma_sa_original_archive_mount";
            shared::need(provider&&provider->mountAuthenticatedRes(identity_.container,::rr::assets::originalTrackMountDigest50LA(identity_.container,identity_.sha.c_str()),::rr::assets::originalTrackMountBytes50LA(identity_.container,identity_.bytes),storage_->archive,mounted)&&
                (!identity_.particleArchive||provider->mountAuthenticatedRes(identity_.particleContainer,
                    ::rr::assets::originalTrackMountDigest50LA(identity_.particleContainer,identity_.particleSha.c_str()),
                    ::rr::assets::originalTrackMountBytes50LA(identity_.particleContainer,identity_.particleBytes),*identity_.particleArchive,mounted))&&
                provider->sealNativeMounts(mounted)&&request.canonicalDomain->resolver3E1A70()->bindProvider3E1A70(provider,d.canonical.texture),
                "Original MA/SA canonical provider mount rejected");
            std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials;
            std::vector<a::OriginalStandardTextureSourceInput50LA> textures;
            d.stage="ma_sa_original_racer_sources";
            for(const auto& racer:request.canonicalRacerModels)
                shared::need(racer.resource&&racer.references&&racer.references->table382B38()==request.canonicalDomain->materials382B38().get()&&
                    original_racer_material_sources50LA::appendModelSources(*racer.resource,*racer.references,
                        *request.canonicalMaterialPath.actualPrefix36B970,*provider,materials,textures,d.racerSource),
                    "Original MA/SA canonical racer material/texture closure rejected");
            shared::need(request.canonicalDomain->appendOriginalSources(materials,textures,d.canonical),"Original MA/SA canonical racer source admission rejected");
            if(beforeRacerPopulation){
                d.stage="ma_sa_original_race_load";
                shared::need(beforeRacerPopulation(),"Original MA/SA race load before population rejected");
            }
            d.stage="ma_sa_original_racer_population";
            shared::need(request.canonicalDomain->populate(request.canonicalContext,d.canonical),"Original MA/SA canonical racer population rejected");
            return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Original MA/SA canonical racer preparation failed";}catch(...){}return false;}
    }

    // Preserve the established MA/SA material-keyed single-pass alpha policy.
    bool alpha(std::size_t selection,const a::OriginalUiMaterialTemplates50KG& templates,
        const a::OriginalUiMaterialRuntime50KG& runtime,material_alpha50gx_fixture::Policy& output)const noexcept{
        try{return storage_&&selection<storage_->selections.size()&&
            material_alpha50gx_fixture::selectMaSa(storage_->selections[selection],templates,runtime,output);}
        catch(...){return false;}
    }
    // Borrow the same authenticated archive for a separate MA/SA cold decoder. The
    // archive lifetime is the world lifetime and cannot be replaced by reload.
    const a::ResArchive48C* archive()const noexcept{return storage_?&storage_->archive:nullptr;}
};

