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
#include "mc_native_pixel_index.inc"
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
        shared::need(!found,"Duplicate original MC material field");found=true;result=item.value;
    }
    return result;
}
inline bool clamp(const a::MaterialFields48E& value,const char* name){
    const auto text=field(value,name);
    shared::need(text.empty()||text=="0"||text=="1","Unsupported original MC texture wrap mode");
    return text=="1";
}
// One IPUM PST framing for every course (course_world_texture50st.h).
inline a::detail::OriginalIpumMipExtent50LA framePst(const Bytes& source,std::uint32_t& mipIndex){
    return course_world_texture50ST::framePst("MC",source,mipIndex);
}
inline const SourceImage* sourceImage(const std::string& member){
    const auto name=key(member);const SourceImage* found=nullptr;
    for(const auto& entry:sourceImages)if(key(entry.member)==name){
        shared::need(!found,"Ambiguous original MC source image pin");found=&entry;
    }
    return found;
}
// Common course rule (course_world_texture50st.h): a stock pin is the stock
// identity of a member NAME; it applies only when the bytes under that name
// are its stock bytes, otherwise it is a receipt and the image is admitted
// from its own data like any new name. image() logs; the cold loader asks
// again without logging.
inline const SourceImage* matchedSourceImage(const std::string& member,const Bytes& source,
    const std::string& sourceSha,bool logReceipt=true){
    return course_world_texture50ST::matchedPin(sourceImage(member),key(member),source,sourceSha,logReceipt);
}
inline std::uint32_t initialFrame(const a::MaterialFields48E& value){
    const auto text=field(value,"animatedMapInitialFrame");if(text.empty())return 0;
    std::uint32_t result=0;const auto parsed=std::from_chars(text.data(),text.data()+text.size(),result);
    shared::need(parsed.ec==std::errc{}&&parsed.ptr==text.data()+text.size()&&result<64,
        "Original MC ATM initial frame syntax/extent");return result;
}
// Decode only supplied original bytes, through the one common decode: kind,
// frame count, extent and mip come from the image's own data and the ATM
// frame from the material. Stock source, decoded-output and native regression
// pins are receipts (compared and logged, never used to choose or refuse).
// independentlyPinned: an independent decoded-output pin named these exact
// bytes and matched. receipt (optional): the per-image receipt for the loader's tally.
inline Bytes image(const std::string& member,const Bytes& source,std::uint32_t& mipIndex,
    bool& animated,a::DecodeError48C& error,bool* independentlyPinned=nullptr,
    std::uint32_t authoredFrame=course_world_texture50ST::noAuthoredFrame,
    course_world_texture50ST::Receipt* receipt=nullptr){
    namespace cwt=course_world_texture50ST;
    shared::need(source.size()>=12&&source.size()<=8u*1024u*1024u,"Original MC texture source extent");
    const auto name=key(member);const auto sourceSha=shared::sha256(source);
    cwt::Receipt r;r.listed=sourceImage(name)!=nullptr;
    const auto* pin=matchedSourceImage(name,source,sourceSha);r.stockBytes=pin!=nullptr;
    const source_texture50fc_fixture::detail::Entry* outputPin=nullptr;
    for(const auto& entry:exceptionalImages)if(key(entry.member)==name){
        shared::need(!outputPin,"Ambiguous original MC decoded image pin");outputPin=&entry;
    }
    for(const auto& entry:indexedPins)if(key(entry.member)==name){
        if(outputPin)shared::need(std::string(outputPin->outputSha256)==entry.outputSha256,"MC independent pixel pin disagreement");
        outputPin=&entry;
    }
    if(outputPin&&!(source.size()==outputPin->sourceBytes&&sourceSha==outputPin->sourceSha256)){
        course_archive_receipt50CI::memberReceipt("decoded_image_pin",name,source,0,0,
            outputPin->sourceBytes,outputPin->sourceSha256);
        outputPin=nullptr;
    }
    // Without a material (dynamic-material callbacks) stock bytes keep their
    // stock material's frame; anything else starts at frame 0.
    const auto frame=authoredFrame!=cwt::noAuthoredFrame?authoredFrame:pin?pin->initialFrame:0u;
    cwt::Shape shape;auto output=cwt::decode("MC",name,source,frame,shape,error);
    if(pin)r.shapeMatch=cwt::compareShape("MC",name,*pin,shape);
    bool independent=false;
    if(outputPin){
        ++r.outputPins;
        independent=cwt::compareOutput("MC",name,"decoded_output_pin",outputPin->outputSha256,output,true,
            outputPin->width,outputPin->height,outputPin->mipIndex,&shape);
        r.outputMatches+=independent;
    }
    else for(const auto& native:nativePixelPins)if(sourceSha==native.sourceSha){
        // mc_native_pixel_index.inc: native regression output for these stock
        // bytes (looked up by source SHA only): receipt only.
        ++r.outputPins;r.outputMatches+=cwt::compareOutput("MC",name,"native_output_pin",native.outputSha,output);break;
    }
    mipIndex=shape.mipIndex;animated=shape.animated;
    if(independentlyPinned)*independentlyPinned=independent;
    if(receipt)*receipt=r;
    return output;
}
// The retained original byte buffer alone does not authenticate mutable parsed
// fields or their cached typed order. Re-encode the entire parameter document,
// then compare every typed/source-provenance field with a fresh exact decode.
inline void validateBorrowedWorld(const Bytes& scb,const a::World48F& world,a::DecodeError48C& error){
    Bytes roundtrip;a::World48F canonical;
    shared::need(world.parameters.original_bytes==scb&&
        a::encodeParameters48F(world.parameters,roundtrip,error)&&roundtrip==scb&&
        mc_world_semantics50MC::decode(scb,canonical,error)&&mc_world_semantics50MC::validate(world,error),
        "Original MC borrowed world fields differ from authenticated SCB");
    const auto& actual=world.parameters;const auto& source=canonical.parameters;
    shared::need(actual.version==source.version&&actual.header_table_offset==source.header_table_offset&&
        actual.preservesDuplicateLabels==source.preservesDuplicateLabels&&actual.blocks.size()==source.blocks.size(),
        "Original MC parameter provenance header differs");
    for(std::size_t i=0;i<source.blocks.size();++i){
        const auto& x=actual.blocks[i];const auto& y=source.blocks[i];
        shared::need(x.name==y.name&&x.name_offset==y.name_offset&&x.table_entry_offset==y.table_entry_offset&&
            x.offset==y.offset&&x.size_without_count==y.size_without_count&&x.fields.size()==y.fields.size(),
            "Original MC parameter block provenance differs");
        for(std::size_t j=0;j<y.fields.size();++j){const auto& f=x.fields[j];const auto& g=y.fields[j];
            shared::need(f.name==g.name&&f.value==g.value&&f.entry_offset==g.entry_offset&&
                f.value_offset==g.value_offset&&f.value_bytes==g.value_bytes,
                "Original MC parameter field provenance differs");
        }
    }
    shared::need(world.scene_info_block==canonical.scene_info_block&&
        world.instances.size()==canonical.instances.size()&&
        world.zones.size()==canonical.zones.size()&&world.portals.size()==canonical.portals.size(),
        "Original MC typed world source extent differs");
    for(std::size_t i=0;i<canonical.zones.size();++i){const auto& x=world.zones[i];const auto& y=canonical.zones[i];
        shared::need(x.zone==y.zone&&x.models==y.models&&x.parameter_block==y.parameter_block,
            "Original MC typed zone order/provenance differs");
    }
    const auto same=[](const auto& x,const auto& y){
        return !std::memcmp(x.data(),y.data(),sizeof(x[0])*x.size());
    };
    for(std::size_t i=0;i<canonical.instances.size();++i){const auto& x=world.instances[i];const auto& y=canonical.instances[i];
        shared::need(x.section==y.section&&x.geometry_member==y.geometry_member&&x.collision_member==y.collision_member&&
            x.zone==y.zone&&x.ordinal==y.ordinal&&x.parameter_block==y.parameter_block&&
            x.source_block_offset==y.source_block_offset&&x.use_lighting==y.use_lighting&&
            same(x.position,y.position)&&same(x.look,y.look)&&same(x.up,y.up)&&same(x.matrix,y.matrix),
            "Original MC typed instance order/provenance differs");
    }
    for(std::size_t i=0;i<canonical.portals.size();++i){const auto& x=world.portals[i];const auto& y=canonical.portals[i];
        shared::need(x.section==y.section&&x.parameter_block==y.parameter_block&&x.zones==y.zones,
            "Original MC typed portal order/provenance differs");
        for(std::size_t j=0;j<y.vertices.size();++j)shared::need(same(x.vertices[j],y.vertices[j]),
            "Original MC typed portal vertices differ");
    }
}
} // namespace detail

// Compatible direct-source callback for independently owned MC material consumers.
// Never reads a retained image filename; failures preserve the caller's image.
inline bool decodeSourceTexture(const char* member,const Bytes& source,Bytes& output,
    std::uint32_t& mipIndex,std::string& error)noexcept {
    try{
        shared::need(member&&*member,"Original MC source texture member required");
        a::DecodeError48C diagnostic;std::uint32_t mip=0;bool animated=false;
        auto next=detail::image(member,source,mip,animated,diagnostic);
        output=std::move(next);mipIndex=mip;error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="Original MC source texture decode failed";}catch(...){}return false;}
}

class Owner final {
    struct Scene {a::Scene48D decoded;std::uint64_t identity=0;std::vector<std::uint32_t> materials;};
    struct Storage {
        std::shared_ptr<const mc_cold_sources50LA::Owner> source;
        std::vector<a::TrackProjectedUv> projectionUvs;
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
                "Original MC archive member extraction: "+entry.name+" "+error.code);return bytes;
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
    const std::vector<a::TrackProjectedUv>* projectionUvs()const noexcept{return storage_?&storage_->projectionUvs:nullptr;}
    std::shared_ptr<const mc_cold_sources50LA::Owner> sourceOwner()const noexcept{return storage_?storage_->source:nullptr;}
    bool load(std::shared_ptr<const mc_cold_sources50LA::Owner> sourceOwner,Diagnostic& d)noexcept;

    // Same Application-owned domain as the actual installed racers. This does
    // not register synthetic static models or claim source render dispatch for
    // the native world above. Mounting the real complete MC namespace preserves
    // the provider's local resource lookup before genuine racer population.
    // Runs inside populateCanonicalRacers after the racer sources are admitted
    // and before the single population, as on every consumed course: the
    // original race load 1A8A40 (Pause100 / Results104 / F8).
    std::function<bool()> beforeRacerPopulation;

    bool populateCanonicalRacers(const tm::Request& request,std::size_t availableAdditionalPayloadBytes,tm::Diagnostic& d)noexcept{
        try{
            d={};shared::need(storage_&&!storage_->canonicalAttempted,"MC canonical population requires fresh native source owner");
            const bool canonical=bool(request.canonicalDomain);
            shared::need(canonical==bool(request.canonicalContext)&&canonical==(request.canonicalMaterialPath.actualPrefix36B970!=nullptr),
                "MC canonical Application/Video/path must be supplied together");
            if(!canonical){shared::need(request.canonicalRacerModels.empty(),"MC racer sources lack their canonical domain");return true;}
            shared::need(request.track==selectedTrack&&request.canonicalDomain->prepared()&&
                request.canonicalContext->live()&&*request.canonicalMaterialPath.actualPrefix36B970=="textures\\",
                "MC canonical racers require their actual gameplay directory and live Video");
            storage_->canonicalAttempted=true;storage_->canonicalDomain=request.canonicalDomain;
            const auto& provider=request.canonicalDomain->archiveProvider3E1A70();
            a::OriginalStandardArchiveProvider50LA::Diagnostic mounted;d.stage="mc_original_archive_mount";
            shared::need(provider&&storage_->source->mountCanonicalArchive(*provider,availableAdditionalPayloadBytes,mounted)&&
                provider->sealNativeMounts(mounted)&&request.canonicalDomain->resolver3E1A70()->bindProvider3E1A70(provider,d.canonical.texture),
                "Original MC canonical provider mount rejected");
            std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials;
            std::vector<a::OriginalStandardTextureSourceInput50LA> textures;
            d.stage="mc_original_racer_sources";
            for(const auto& racer:request.canonicalRacerModels)
                shared::need(racer.resource&&racer.references&&racer.references->table382B38()==request.canonicalDomain->materials382B38().get()&&
                    original_racer_material_sources50LA::appendModelSources(*racer.resource,*racer.references,
                        *request.canonicalMaterialPath.actualPrefix36B970,*provider,materials,textures,d.racerSource),
                    "Original MC canonical racer material/texture closure rejected");
            shared::need(request.canonicalDomain->appendOriginalSources(materials,textures,d.canonical),"Original MC canonical racer source admission rejected");
            if(beforeRacerPopulation){
                d.stage="mc_original_race_load";
                shared::need(beforeRacerPopulation(),"Original MC race load before population rejected");
            }
            d.stage="mc_original_racer_population";
            shared::need(request.canonicalDomain->populate(request.canonicalContext,d.canonical),"Original MC canonical racer population rejected");
            return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Original MC canonical racer preparation failed";}catch(...){}return false;}
    }

    // Preserve the authenticated MC material-keyed authored-pass alpha policy.
    bool alpha(std::size_t selection,const a::OriginalUiMaterialTemplates50KG& templates,
        const a::OriginalUiMaterialRuntime50KG& runtime,material_alpha50gx_fixture::Policy& output)const noexcept{
        try{return storage_&&selection<storage_->selections.size()&&
            material_alpha50gx_fixture::selectMc(storage_->selections[selection],templates,runtime,output);}
        catch(...){return false;}
    }
    // Borrow the same authenticated archive for a separate MC cold decoder. The
    // archive lifetime is the world lifetime and cannot be replaced by reload.
    const a::IndexedResArchive48C* archive()const noexcept{return storage_?&storage_->source->archive():nullptr;}
};

