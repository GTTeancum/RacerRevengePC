// Included after sky50gw_fixture::Owner (bottom of sky50gw_fixture.h).
// Course-neutral single-cube sky loaded from the course's own data: the
// decoded course archive and the World48F decoded from that archive's SCB.
// Nothing here names a course. Original chain followed:
//  276C00 EnvironmentInfo read: NumberOfSkyCubes default 1 (276C4C), CubeFile1
//         then, only when that key is missing, unnumbered CubeFile (276CD8),
//         0x104-byte name allocation (276C80), sole 'none' -> zero (276E40);
//  274AE0 joins the application sky directory (36B750, "skies\") and the name;
//  1295B0/1289C0/1290E0 frame the CUB; 128D50/24F2C0 tile descriptor + mips;
//  12A8A0 cube geometry; the viewport clear consumes [Fog] ColorRGB.
// The CUB format checks (tables, texture flags, face masks/sizes, pose) are
// enforced; there are no identity pins (archive/CUB/SCB/tile/image SHA-256,
// fixed fog byte offset, per-tile tables): the data is read rather than matched.
// This is the only sky image load (the per-course load()/loadGb were retired).
namespace sky50gw_fixture {
namespace course_data_detail50GW {
inline const a::ParameterBlock48F& uniqueBlock(const a::ParameterDocument48F& p,
    const char* name, const char* what) {
    const a::ParameterBlock48F* found=nullptr;
    for(const auto& b:p.blocks)if(b.name==name){
        detail::need(!found,std::string(what)+" block duplicated");found=&b;}
    detail::need(found!=nullptr,std::string(what)+" block missing");
    return *found;
}
inline const std::string* uniqueField(const a::ParameterBlock48F& b, const char* name) {
    const std::string* found=nullptr;
    for(const auto& f:b.fields)if(f.name==name){
        detail::need(!found,std::string("Course sky field duplicated: ")+name);found=&f.value;}
    return found;
}
} // namespace course_data_detail50GW

template<class Extract> inline bool Owner::loadCourseDataWith(Extract&& extract,
    const a::World48F& world, Diagnostic& d) noexcept {
    using namespace detail;namespace cd=course_data_detail50GW;
    try {
        d={};
        need(!storage_,"Sky assets already loaded");
        need(std::fegetround()==FE_TONEAREST,"Sky geometry requires FE_TONEAREST");
        const auto& environment=cd::uniqueBlock(world.parameters,"environment","Course sky [Environment]");
        // 276C4C: NumberOfSkyCubes with default 1. Strict decimal admission.
        std::int32_t cubes=1;
        if(const auto* text=cd::uniqueField(environment,"numberofskycubes")){
            const auto r=std::from_chars(text->data(),text->data()+text->size(),cubes);
            need(!text->empty()&&r.ec==std::errc{}&&r.ptr==text->data()+text->size(),
                "Course sky NumberOfSkyCubes unreadable");
        }
        need(cubes>0,"Course sky authors no cube; the zero-sky owner applies");
        // FLAG stand-in: 274AE0 builds one SkyCube child per authored cube; this
        // owner builds exactly one. More than one is refused, never truncated.
        need(cubes==1,"Course sky authors more than one cube: multi-cube data-driven sky not built");
        const auto* cubeFile=cd::uniqueField(environment,"cubefile1");
        if(!cubeFile)cubeFile=cd::uniqueField(environment,"cubefile");
        need(cubeFile!=nullptr,"Course sky [Environment] CubeFile1/CubeFile missing");
        need(*cubeFile!="none","Course sky CubeFile1 is 'none' (zero effective cubes)");
        // Bounded by the original 0x104-byte name allocation. Path separators
        // inside the authored name are unproven and refused.
        need(!cubeFile->empty()&&cubeFile->size()<0x104u&&
            cubeFile->find_first_of("/\\:")==std::string::npos&&
            cubeFile->find('\0')==std::string::npos,"Course sky CubeFile1 name unsupported");
        const std::string member="skies\\"+*cubeFile;
        Bytes cub;a::DecodeError48C error;
        need(extract(member,cub,error),"Course sky CUB member "+member+": "+error.detail);
        // The CUB's own cube table (1295B0/1289C0): u32 1, u32 cube count n,
        // u32 table end (12+4n), then n cube-header offsets. A CUB may hold
        // more than one internal cube (GB's 7: the base cube plus the
        // ambient-flash cubes); cube 0 is the base cube drawn, exactly what
        // the GB loader submits. FLAG: further internal cubes are read for
        // their extent only (no flash chronology is invented).
        need(cub.size()>=16,"Course sky CUB shorter than its cube table");
        const std::uint32_t internalCubes=u32(cub.data()+4);
        need(u32(cub.data())==1&&internalCubes>=1&&internalCubes<=64&&u32(cub.data()+8)==12u+4u*internalCubes&&
            cub.size()>=12u+4u*internalCubes,"Course sky CUB cube table");
        const std::size_t cubeHeader=u32(cub.data()+12);
        const std::size_t cubeEnd=internalCubes>1?u32(cub.data()+16):cub.size();
        need(cubeHeader>=12u+4u*internalCubes&&cubeHeader+80u<=cubeEnd&&cubeEnd<=cub.size(),"Course sky CUB base cube extent");
        {   // Same per-cube format facts load() enforces through requireCubFormat.
            const auto* header=cub.data()+cubeHeader;
            need(u32(header)==32&&u16(header+4)==0&&u16(header+6)==0,"CUB exact supported texture/flags");
            for(unsigned face=0;face<6;++face)
                need(u16(header+8+face*2)==cubMasks[face]&&u32(header+20+face*4)==cubSizes[face],"CUB authored tile mask/sizes");
            for(unsigned i=0;i<9;++i)need(f32(header+44+i*4)==cubSourcePose[i],"CUB authored origin/look/up");
        }
        if(internalCubes>1)std::cerr<<"course_sky_data cub_internal_cubes="<<internalCubes<<" drawn=base\n";
        // The 13E830 clear colour (World3EC, [Fog] ColorRGB) belongs to the 05.11 sky selection
        // (original_track_sky50KX::Owner::clearRgba over the tolerant 05.05 fog reader; a track
        // may author no [Fog]). The image loader does not re-parse [Fog]; its own clearRgba stays
        // opaque black and nothing draws it.
        auto next=std::make_unique<Storage>();
        next->clearRgba={{0.f,0.f,0.f,1.f}};
        next->cubeMember=member;next->cubeBytes=cub.size();next->internalCubes=internalCubes;next->cubeSha256=sha256(cub);
        next->nativeRaProof=std::make_unique<NativeRaSourceProof>();
        auto& proof=*next->nativeRaProof;
        next->mesh.vertices.reserve(vertexCount); next->mesh.indices.reserve(indexCount);
        next->spans.reserve(tileCount);
        std::array<Bytes, tileCount> images;
        std::array<a::MaterialTextureAssetInput50EA, tileCount> textures{};
        std::array<a::MaterialTextureSlotInput50EA, tileCount> slots{};
        std::size_t offset=cubeHeader+80u; std::uint32_t index=0;
        for (std::uint32_t face = 0; face < 6; ++face) for (std::uint32_t tile = 0; tile < 16; ++tile) {
            if (!(cubMasks[face] & (1u << tile))) continue;
            need(index < tileCount, "Sky tile count bound");
            need(offset <= cubeEnd, "Course sky tile offset");
            // Tile extent = its own 44-byte descriptor + four length-prefixed
            // mips, walked inside the base cube's bytes only.
            std::size_t largest=0,largestBytes=0;
            const auto bytes=walkTile(cub.data()+offset,cubeEnd-offset,largest,largestBytes);
            need(bytes<=0xffffffffu&&offset<=0xffffffffu,"Course sky tile extent");
            proof.tiles[index]={face,tile,static_cast<std::uint32_t>(offset),static_cast<std::uint32_t>(bytes),nullptr,nullptr,nullptr};
            ++proof.consumedTiles;
            decodeLargestMip(cub.data()+offset,largest,largestBytes,images[index]);
            need(images[index].size() == 65552 && std::memcmp(images[index].data(), "RRTEX001", 8) == 0 &&
                u32(images[index].data()+8) == 128 && u32(images[index].data()+12) == 128,
                "Decoded sky image dimensions");
            offset += bytes;
            textures[index] = {index, &images[index]};
            slots[index] = {{0, index, 1, 1}, index, a::MaterialTextureRequirement50EA::required};
            appendTileQuad(next->mesh, next->spans, face, tile, index);
            ++index;
        }
        need(index == tileCount && offset == cubeEnd, "Complete CUB base cube tile closure");
        closeBounds(next->mesh);
        const a::MaterialTextureAssetsInput50EA bankInput{textures.data(), tileCount, slots.data(), tileCount,
            {tileCount,tileCount,3146496,3145728,128}};
        need(next->bank.load(bankInput, d.bank), "Sky texture bank: " + d.bank.parserMessage);
        for(std::uint32_t i=0;i<tileCount;++i){const auto* image=next->bank.texture(i);
            need(image&&image->identity==i,"Course sky bank image identity");proof.tiles[i].image=image;}
        proof.mesh=&next->mesh;proof.bank=&next->bank;proof.spans=&next->spans;
        proof.memberBytes=cub.size();
        storage_ = std::move(next); return true;
    } catch (const std::exception& e) { try { d.message = e.what(); } catch (...) {} return false; }
    catch (...) { try { d.message = "Unexpected cold course-data sky load failure"; } catch (...) {} return false; }
}
inline bool Owner::loadFromCourseData(const a::ResArchive48C& archive,
    const a::World48F& world, Diagnostic& d) noexcept {
    // Bounded by the shared extractor ceiling (8 MiB) and the member's own extent.
    if(!loadCourseDataWith([&](const std::string& member,std::vector<std::uint8_t>& out,a::DecodeError48C& error){
        return a::extractResMember48C(archive,member,out,error);},world,d))return false;
    // Source proof: the archive member that produced these images (same bytes).
    auto& proof=*storage_->nativeRaProof;const auto& wanted=storage_->cubeMember;
    const auto lower=[](unsigned char c){return c>='A'&&c<='Z'?static_cast<unsigned char>(c-'A'+'a'):c;};
    for(std::size_t i=0;i<archive.entries.size();++i){const auto& entry=archive.entries[i];
        if(entry.name.size()!=wanted.size())continue;
        bool same=true;for(std::size_t k=0;k<wanted.size()&&same;++k)
            same=lower(static_cast<unsigned char>(entry.name[k]))==lower(static_cast<unsigned char>(wanted[k]));
        if(!same)continue;
        if(proof.member){d.message="Course sky CUB member ambiguous";storage_.reset();return false;}
        if(entry.logical_offset>archive.decoded.size()||entry.bytes>archive.decoded.size()-entry.logical_offset||
           entry.bytes!=storage_->cubeBytes){d.message="Course sky CUB member extent differs";storage_.reset();return false;}
        proof.archive=&archive;proof.member=&entry.name;proof.memberOrdinal=i;
        proof.memberOffset=entry.logical_offset;proof.memberBytes=entry.bytes;
        proof.memberData=archive.decoded.data()+entry.logical_offset;
    }
    if(!proof.member){d.message="Course sky CUB member not in the course archive";storage_.reset();return false;}
    return true;
}
inline bool Owner::loadFromCourseData(const a::IndexedResArchive48C& archive,
    const a::World48F& world, Diagnostic& d) noexcept {
    return loadCourseDataWith([&](const std::string& member,std::vector<std::uint8_t>& out,a::DecodeError48C& error){
        return archive.extractMember(member,out,error,8u*1024u*1024u);},world,d);
}
} // namespace sky50gw_fixture
