#include "original_standard_model_resource50la.h"
#include "original_standard_material_table50la.h"
#include <exception>
#include <cstdio>
#include <limits>
#include <new>
#include <utility>

namespace rr::assets {
namespace {
using D=OriginalStandardResourceDiagnostic50LA;
using S=OriginalStandardResourceStatus50LA;
bool fail(D& d,S status,std::uint32_t pc) noexcept {d.status=status;d.sourcePC=pc;return false;}
}
struct OriginalStandardModelResource50LA::Storage {
    std::string member;
    std::vector<std::uint8_t> bytes;
    Scene48D scene;
    std::array<PsgNode49O,maxModelNodes49O> rows{};
    std::uint32_t count=0;
    float lodScale14=1.f; //26E050 constructor,26E078..7C.
};
OriginalStandardModelResource50LA::OriginalStandardModelResource50LA() noexcept=default;
OriginalStandardModelResource50LA::~OriginalStandardModelResource50LA()=default;
bool OriginalStandardModelResource50LA::prepare(const std::string& member,
    const std::vector<std::uint8_t>& bytes,D& d) noexcept {
    return prepareOriginalLimits(member,bytes,SceneLimits48D{},d);
}
bool OriginalStandardModelResource50LA::prepareOriginalLimits(const std::string& member,
    const std::vector<std::uint8_t>& bytes,const SceneLimits48D& limits,D& d) noexcept {
    d={};if(storage_)return fail(d,S::alreadyPrepared,0x26E820);
    if(member.empty()||member.size()>260||bytes.empty()||bytes.size()>maxPsgBytes49O)
        return fail(d,S::input,0x26E990);
    try {
        auto next=std::make_unique<Storage>();next->member=member;next->bytes=bytes;
        PsgHierarchyView49O hierarchy{};
        if(!readPsgHierarchy49O(next->bytes.data(),next->bytes.size(),hierarchy,d.hierarchy))
            return fail(d,S::hierarchy,0x28CAC0);
        if(!decodeScene48D(next->bytes,next->scene,d.geometry,nullptr,limits))return fail(d,S::geometry,0x26EE24);
        if(next->scene.nodes.size()!=hierarchy.nodeCount)return fail(d,S::hierarchy,0x28CB0C);
        next->count=hierarchy.nodeCount;
        for(std::uint32_t i=0;i<next->count;++i){
            auto& node=next->rows[i];
            if(!readPsgNode49O(hierarchy,i,node)||(i==0?node.parent!=-1:node.parent<0))
                return fail(d,S::hierarchy,0x28CC90);
        }
        storage_=std::move(next);return true;
    }catch(const std::bad_alloc&){return fail(d,S::allocation,0x26EE38);}
    catch(...){return fail(d,S::input,0x26E820);}
}
bool OriginalStandardModelResource50LA::prepared() const noexcept{return bool(storage_);}
const std::string* OriginalStandardModelResource50LA::member() const noexcept{return storage_?&storage_->member:nullptr;}
const std::vector<std::uint8_t>* OriginalStandardModelResource50LA::originalBytes() const noexcept{return storage_?&storage_->bytes:nullptr;}
const Scene48D* OriginalStandardModelResource50LA::scene() const noexcept{return storage_?&storage_->scene:nullptr;}
const PsgNode49O* OriginalStandardModelResource50LA::sourceNodes() const noexcept{return storage_?storage_->rows.data():nullptr;}
std::uint32_t OriginalStandardModelResource50LA::nodeCount() const noexcept{return storage_?storage_->count:0u;}
const float* OriginalStandardModelResource50LA::sourceLodScale14() const noexcept{return storage_?&storage_->lodScale14:nullptr;}

struct OriginalStandardModelGeometry50LA::Storage {
    std::shared_ptr<const OriginalStandardModelResource50LA> payload;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver;
    OriginalStandardModelFileId50LA file{};
    const rr::game::OriginalRacerModelSceneOwner50LA* prototype=nullptr;
    std::int64_t references=1; //1048F0 reached through26E050.
    float scale=1.f; //26E078/26E07C.
    std::vector<std::vector<std::int32_t>> surfaceMaterials;
    std::vector<std::vector<std::uint8_t>> palettes;
    //Native provenance for each real26E820 owner, not another source ref.
    std::vector<const rr::game::OriginalRacerModelSceneOwner50LA*> models;
    bool released=false;
};
OriginalStandardModelGeometry50LA::OriginalStandardModelGeometry50LA() noexcept=default;
OriginalStandardModelGeometry50LA::~OriginalStandardModelGeometry50LA(){
    if(storage_&&!storage_->released)std::terminate();
}
bool OriginalStandardModelGeometry50LA::construct240110(
    std::shared_ptr<const OriginalStandardModelResource50LA> payload,
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,OriginalStandardModelFileId50LA file,
    const rr::game::OriginalRacerModelSceneOwner50LA* prototype,D& d) noexcept {
    d={};
    struct FailureReport {
        const std::string* member; D& diagnostic;
        ~FailureReport(){if(diagnostic.status!=S::complete)
            std::fprintf(stderr,"original_geometry_failure member=%s status=%u pc=%08x cache=%u cache_pc=%08x\n",
                member?member->c_str():"",unsigned(diagnostic.status),diagnostic.sourcePC,
                unsigned(diagnostic.cache.status),diagnostic.cache.sourcePC);}
    } failureReport{payload?payload->member():nullptr,d};
    if(storage_)return fail(d,S::alreadyPrepared,0x240110);
    if(!payload||!payload->prepared()||!payload->scene()||!payload->member()||
       !payload->originalBytes()||!resolver||!prototype)return fail(d,S::input,0x26EE38);
    OriginalStandardModelFileView50LA source;
    if(!resolver->modelFile(file,source,d.cache))return fail(d,S::cache,0x26EE50);
    if(source.kind!=OriginalStandardModelFileKind50LA::psg||source.geometry240110||
       !source.bytes||!source.member||*source.bytes!=*payload->originalBytes())
        return fail(d,S::cache,0x26EE50);
    //Provider spelling may differ in ASCII case, but never in separators.
    const auto& name=*payload->member();const auto& sourceName=*source.member;
    if(name.size()!=sourceName.size())return fail(d,S::cache,0x26EE50);
    const auto fold=[](unsigned char c){return c>='A'&&c<='Z'?c+32:c;};
    for(std::size_t i=0;i<name.size();++i)
        if(fold(static_cast<unsigned char>(name[i]))!=fold(static_cast<unsigned char>(sourceName[i])))
            return fail(d,S::cache,0x26EE50);
    try {
        auto next=std::make_unique<Storage>();const auto& scene=*payload->scene();
        const auto& bytes=*payload->originalBytes();
        const auto read32=[&](std::size_t at){return std::uint32_t(bytes[at])|
            (std::uint32_t(bytes[at+1])<<8)|(std::uint32_t(bytes[at+2])<<16)|
            (std::uint32_t(bytes[at+3])<<24);};
        if(scene.lods.empty()||scene.lods.back().surfaces.empty())return fail(d,S::geometry,0x26E1A8);
        next->surfaceMaterials.reserve(scene.lods.size());
        for(const auto& lod:scene.lods){
            std::vector<std::int32_t> materials;materials.reserve(lod.surfaces.size());
            for(const auto& surface:lod.surfaces){
                if(surface.material_index>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))
                    return fail(d,S::geometry,0x299BB0);
                materials.push_back(static_cast<std::int32_t>(surface.material_index));
            }
            next->surfaceMaterials.push_back(std::move(materials));
        }
        //240210 reads the actual count after the final surface payload. The
        //decoded identity-palette flag alone is not proof of a zero trailer.
        const auto offset=scene.lods.back().surfaces.back().source_offset;
        if(offset>bytes.size()||bytes.size()-offset<20)return fail(d,S::geometry,0x24024C);
        const auto packetBytes=read32(offset+16);
        if(packetBytes>bytes.size()-offset-20)return fail(d,S::geometry,0x24024C);
        const auto at=offset+20+packetBytes;
        // Original static one-node PSGs end exactly after surface data. They
        // carry no matrix-palette trailer (the authenticated decoder enforces
        // this). Represent that real native no-palette resource directly;
        // never read past EOF or invent a zero word in the original file.
        const bool staticNoPalette=payload->nodeCount()==1&&at==bytes.size()&&
            !scene.identity_matrix_palette;
        if(!staticNoPalette&&bytes.size()-at<4)return fail(d,S::geometry,0x24024C);
        const auto count=staticNoPalette?0u:read32(at);
        if(staticNoPalette){
            // No palette allocation or synthesized source-read result.
        }else if(count==0){
            if(at+4!=bytes.size()||scene.identity_matrix_palette)return fail(d,S::geometry,0x240258);
        }else {
            if(count!=1||!scene.identity_matrix_palette||bytes.size()-at<8)
                return fail(d,S::geometry,0x240258);
            const auto padded=read32(at+4);const auto nodes=payload->nodeCount();
            if(padded!=((nodes+3u)&~3u)||bytes.size()-at-8!=std::size_t(padded)+4u||
               read32(at+8+padded)!=nodes)return fail(d,S::geometry,0x2402B4);
            std::vector<std::uint8_t> palette;palette.reserve(nodes);
            for(std::uint32_t i=0;i<nodes;++i){
                if(bytes[at+8+i]!=i)return fail(d,S::geometry,0x240310);
                palette.push_back(bytes[at+8+i]);
            }
            next->palettes.push_back(std::move(palette));
        }
        next->models.push_back(prototype);
        next->payload=std::move(payload);next->resolver=std::move(resolver);
        next->file=file;next->prototype=prototype; //26EE74, after240210.
        storage_=std::move(next);return true;
    }catch(const std::bad_alloc&){return fail(d,S::allocation,0x26EE38);}
    catch(...){return fail(d,S::input,0x240210);}
}
bool OriginalStandardModelGeometry50LA::constructed240110() const noexcept {
    return storage_&&!storage_->released;
}
const std::int64_t* OriginalStandardModelGeometry50LA::references00() const noexcept {
    return storage_?&storage_->references:nullptr;
}
const float* OriginalStandardModelGeometry50LA::scale14() const noexcept {
    return constructed240110()?&storage_->scale:nullptr;
}
const rr::game::OriginalRacerModelSceneOwner50LA* OriginalStandardModelGeometry50LA::prototype1C() const noexcept {
    return constructed240110()?storage_->prototype:nullptr;
}
const OriginalStandardModelResource50LA* OriginalStandardModelGeometry50LA::payload() const noexcept {
    return constructed240110()?storage_->payload.get():nullptr;
}
OriginalStandardModelFileId50LA OriginalStandardModelGeometry50LA::fileId() const noexcept {
    return storage_?storage_->file:OriginalStandardModelFileId50LA{};
}
bool OriginalStandardModelGeometry50LA::hasPalette20() const noexcept {
    return constructed240110()&&!storage_->palettes.empty();
}
std::size_t OriginalStandardModelGeometry50LA::paletteCount20() const noexcept {
    return constructed240110()?storage_->palettes.size():0;
}
const std::vector<std::uint8_t>* OriginalStandardModelGeometry50LA::palette20(std::size_t ordinal) const noexcept {
    return constructed240110()&&ordinal<storage_->palettes.size()?&storage_->palettes[ordinal]:nullptr;
}
const std::int32_t* OriginalStandardModelGeometry50LA::surfaceMaterial00(std::size_t lod,std::size_t ordinal) const noexcept {
    return constructed240110()&&lod<storage_->surfaceMaterials.size()&&ordinal<storage_->surfaceMaterials[lod].size()?
        &storage_->surfaceMaterials[lod][ordinal]:nullptr;
}
bool OriginalStandardModelGeometry50LA::retain104960(
    const rr::game::OriginalRacerModelSceneOwner50LA* model,D& d) noexcept {
    d={};if(!constructed240110()||!model||storage_->references<=0||
        static_cast<std::uint64_t>(storage_->references)!=storage_->models.size())
        return fail(d,S::reference,0x104960);
    for(const auto* existing:storage_->models)if(existing==model)return fail(d,S::reference,0x104960);
    OriginalStandardModelFileView50LA file;
    if(!storage_->resolver->modelFile(storage_->file,file,d.cache)||file.geometry240110.get()!=this)
        return fail(d,S::cache,0x26E9F4);
    if(storage_->references==std::numeric_limits<std::int64_t>::max())return fail(d,S::overflow,0x104960);
    try {storage_->models.push_back(model);}
    catch(...){return fail(d,S::allocation,0x104960);}
    ++storage_->references;return true;
}
bool OriginalStandardModelGeometry50LA::canReleaseForModel(
    const rr::game::OriginalRacerModelSceneOwner50LA* model) const noexcept {
    if(!constructed240110()||!model||storage_->references<=0||
       static_cast<std::uint64_t>(storage_->references)!=storage_->models.size())return false;
    bool found=false;for(const auto* existing:storage_->models)if(existing==model)found=true;
    if(!found)return false;
    //The original resource keeps its first creator at1C. Releasing that
    //creator early would leave the next warm26EA10 with an invalid pointer.
    if((model==storage_->prototype)!=(storage_->references==1))return false;
    OriginalStandardModelFileView50LA file;OriginalStandardTextureDiagnostic50LA diagnostic;
    return storage_->resolver->modelFile(storage_->file,file,diagnostic)&&
        (!file.geometry240110||file.geometry240110.get()==this);
}
bool OriginalStandardModelGeometry50LA::release26E598(
    const rr::game::OriginalRacerModelSceneOwner50LA* model,
    const OriginalStandardMaterialReferences50LA& materials,D& d) noexcept {
    d={};if(!canReleaseForModel(model))return fail(d,S::reference,0x26E598);
    if(!materials.released26E4C0())return fail(d,S::phase,0x26E4C0);
    auto& s=*storage_;
    if(s.references==1){
        //26E510..594 repeats this sweep once per model material. Repeating
        //the identical stores is immaterial to these private CPU fields.
        for(auto& lod:s.surfaceMaterials)for(auto& material:lod)material=-1;
    }
    --s.references; //Resource104970, not Entity142590 or423F60.
    for(auto it=s.models.begin();it!=s.models.end();++it)if(*it==model){s.models.erase(it);break;}
    if(s.references==0){
        std::vector<std::vector<std::uint8_t>>().swap(s.palettes);
        std::vector<std::vector<std::int32_t>>().swap(s.surfaceMaterials);
        std::vector<const rr::game::OriginalRacerModelSceneOwner50LA*>().swap(s.models);
        s.payload.reset(); //240150/26E0A0: release all owned resource backing.
        s.resolver->clearGeometry1327D0(this); //26E5BC..26E5C8, after destruction.
        s.released=true;
    }
    return true;
}
}
