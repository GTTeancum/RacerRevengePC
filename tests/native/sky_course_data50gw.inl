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
// The CUB format checks (tables, texture flags, face masks/sizes, pose) are the
// same as load(); only the identity pins (archive/CUB/SCB/tile/image SHA-256,
// fixed fog byte offset, per-tile tables) are absent, because the data is read
// rather than matched.
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

inline bool Owner::loadFromCourseData(const a::ResArchive48C& archive,
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
        // Bounded by the shared extractor ceiling (8 MiB) and the member's own extent.
        need(a::extractResMember48C(archive,member,cub,error),
            "Course sky CUB member "+member+": "+error.detail);
        need(cub.size()>=cubFirstTileOffset,"Course sky CUB shorter than its header");
        requireCubFormat(cub);
        const auto& fog=cd::uniqueBlock(world.parameters,"fog","Course sky [Fog]");
        const auto* rgb=cd::uniqueField(fog,"colorrgb");
        need(rgb&&rgb->find('\0')==std::string::npos,"Course sky [Fog] ColorRGB missing");
        auto next=std::make_unique<Storage>();
        parseFogRgb(rgb->c_str(),next->clearRgba);
        next->mesh.vertices.reserve(vertexCount); next->mesh.indices.reserve(indexCount);
        next->spans.reserve(tileCount);
        std::array<Bytes, tileCount> images;
        std::array<a::MaterialTextureAssetInput50EA, tileCount> textures{};
        std::array<a::MaterialTextureSlotInput50EA, tileCount> slots{};
        std::size_t offset=cubFirstTileOffset; std::uint32_t index=0;
        for (std::uint32_t face = 0; face < 6; ++face) for (std::uint32_t tile = 0; tile < 16; ++tile) {
            if (!(cubMasks[face] & (1u << tile))) continue;
            need(index < tileCount, "Sky tile count bound");
            need(offset <= cub.size(), "Course sky tile offset");
            // Tile extent = its own 44-byte descriptor + four length-prefixed
            // mips, walked inside the remaining CUB bytes only.
            std::size_t largest=0,largestBytes=0;
            const auto bytes=walkTile(cub.data()+offset,cub.size()-offset,largest,largestBytes);
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
        need(index == tileCount && offset == cub.size(), "Complete CUB source tile closure");
        closeBounds(next->mesh);
        const a::MaterialTextureAssetsInput50EA bankInput{textures.data(), tileCount, slots.data(), tileCount,
            {tileCount,tileCount,3146496,3145728,128}};
        need(next->bank.load(bankInput, d.bank), "Sky texture bank: " + d.bank.parserMessage);
        storage_ = std::move(next); return true;
    } catch (const std::exception& e) { try { d.message = e.what(); } catch (...) {} return false; }
    catch (...) { try { d.message = "Unexpected cold course-data sky load failure"; } catch (...) {} return false; }
}
} // namespace sky50gw_fixture
