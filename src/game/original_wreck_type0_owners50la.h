#pragma once
#include "original_wreck_assets50la.h"
#include "original_wreck_components50la.h"
#include <array>
#include <cstring>
#include "../assets/original_standard_collision_type0_resource50la.h"
#include <limits>
#include <optional>

namespace rr::game {
class OriginalWreckQueries50LA;
class OriginalRacerQuery694Owner50LA;
struct OriginalRacerSceneObject50LA;
// ONE11B190 query's shape8C, with the genuine shared resource124. Source90
// remains NULL on the11B920/type0 branch. The existing numeric PodShapeState
// is kept separately; it does not own or retain shared array storage.
class OriginalWreckType0Owners50LA final {
    friend class OriginalWreckQueries50LA;
    friend class OriginalRacerQuery694Owner50LA;
    using Resource=rr::assets::OriginalStandardCollisionType0Resource50LA;
    struct Shape {
        SpringSceneId48N model110{}; //borrow, no independent model retain
        const OriginalRacerSceneObject50LA* racerModel110=nullptr; //Typed alternative, never a numeric scene ID.
        std::shared_ptr<Resource> resource124;
    };
    std::unique_ptr<Shape> shape8C_;
    //Native ownership staging for125890's arrays before125014 exists. Its
    //ref60 remains zero until the actual resource constructor store1250D4.
    std::shared_ptr<Resource> loading_;
    bool attempted_=false,loaded_=false,retired_=false;
    std::uint32_t boundary_=0;
    enum class FreshWreckRelease { none,sourceFull11B600,nativeIncompleteCancellation };
    bool freshWreck_=false,freshWarm_=false;
    const OriginalRacerSceneObject50LA* freshModel_=nullptr;
    const OriginalWreckComponent50LA* freshE4_=nullptr;
    const std::optional<const void*>* freshField8C_=nullptr;
    const std::optional<const void*>* freshField90_=nullptr;
    //Borrow of the SAME Query.fullType0Source.local40, never a second matrix.
    BodyMatrix48G* freshShape40_=nullptr;
    std::uint16_t freshShape40KnownMask_=0;
    const Shape* freshShapeHistory_=nullptr;
    const Resource* freshResourceHistory_=nullptr;
    const Resource* freshLoadingHistory_=nullptr;
    const rr::assets::OriginalStandardTextureResolver50LA* freshResolver_=nullptr;
    rr::assets::OriginalStandardModelFileId50LA freshFile_{};
    std::array<const void*,5> freshArrayAddresses_{};
    std::array<std::size_t,5> freshArraySizes_{};
    std::uint32_t freshStoppedPC_=0;
    FreshWreckRelease freshRelease_=FreshWreckRelease::none;
    static std::array<const void*,5> arrayAddresses(const Resource& r)noexcept {
        const auto& m=r.mesh();return {{m.nodes.data(),r.radii().data(),m.faces.data(),m.vertices.data(),m.vertex_normals.data()}};
    }
    static std::array<std::size_t,5> arraySizes(const Resource& r)noexcept {
        const auto& m=r.mesh();return {{m.nodes.size(),r.radii().size(),m.faces.size(),m.vertices.size(),m.vertex_normals.size()}};
    }
    void observeFreshWreckCut()noexcept {
        freshStoppedPC_=boundary_;freshShapeHistory_=shape8C_.get();
        freshResourceHistory_=shape8C_?shape8C_->resource124.get():nullptr;
        freshLoadingHistory_=loading_.get();
        const auto* arrays=freshResourceHistory_?freshResourceHistory_:freshLoadingHistory_;
        if(arrays){freshArrayAddresses_=arrayAddresses(*arrays);freshArraySizes_=arraySizes(*arrays);}
    }
    //Exact same typed Model, actual E4 and source field addresses. Native file
    //transport admission is caller-owned; this adds no FileManager chronology.
    bool loadFreshWreck11B920(const rr::assets::OriginalStandardModelFileView50LA& file,
        const std::vector<std::uint8_t>& raw,const rr::assets::CollisionMesh48I& mesh,
        const std::vector<float>& radii,const OriginalRacerSceneObject50LA& model,
        const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        std::optional<const void*>& actual8C,std::optional<const void*>& actual90,
        const OriginalWreckComponent50LA& e4,BodyMatrix48G& actualShape40,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        if(freshWreck_||attempted_||retired_||!actual8C||*actual8C||!actual90||*actual90||&actual8C==&actual90||
           !resolver||e4.reference00!=1u||e4.kind!=OriginalWreckComponentKind50LA::collision11B190||
           e4.parent1C||e4.previous10||e4.next14||e4.firstChild18)return false;
        freshWreck_=true;freshModel_=&model;freshE4_=&e4;freshField8C_=&actual8C;freshField90_=&actual90;
        freshShape40_=&actualShape40;
        freshResolver_=resolver.get();freshFile_=file.id;freshWarm_=bool(file.collision125014);
        const auto before=freshWarm_?file.collision125014->sourceReference60():0;
        const bool done=loadRacer11B920(file,raw,mesh,radii,model,resolver,actual8C,diagnostic);
        observeFreshWreckCut();
        if(done&&freshWarm_&&(!shape8C_||shape8C_->resource124!=file.collision125014||
           before<=0||before==std::numeric_limits<std::int32_t>::max()||
           shape8C_->resource124->sourceReference60()!=before+1))return false;
        return done;
    }
    bool freshWreckCutCurrent(const OriginalRacerSceneObject50LA& model,
        const OriginalWreckComponent50LA& e4,const std::optional<const void*>& actual8C,
        const std::optional<const void*>& actual90,std::uint32_t references,
        const OriginalWreckComponent50LA* parent,const OriginalWreckComponent50LA* previous,
        const OriginalWreckComponent50LA* expectedNextE8=nullptr)const noexcept {
        if(!freshWreck_||!attempted_||retired_||freshRelease_!=FreshWreckRelease::none||
           freshModel_!=&model||freshE4_!=&e4||freshField8C_!=&actual8C||freshField90_!=&actual90||
           !actual8C||!actual90||*actual90||*actual8C!=shape8C_.get()||
           e4.reference00!=references||e4.kind!=OriginalWreckComponentKind50LA::collision11B190||
           e4.parent1C!=parent||e4.previous10!=previous||e4.next14!=expectedNextE8||e4.firstChild18||
           freshStoppedPC_!=boundary_||freshShapeHistory_!=shape8C_.get()||freshLoadingHistory_!=loading_.get()||
           freshResourceHistory_!=(shape8C_?shape8C_->resource124.get():nullptr))return false;
        if(expectedNextE8&&(!references||!parent||expectedNextE8->reference00!=1u||
           expectedNextE8->kind!=OriginalWreckComponentKind50LA::collision11B190||
           expectedNextE8->parent1C!=parent||expectedNextE8->previous10!=&e4||expectedNextE8->next14||
           expectedNextE8->firstChild18||expectedNextE8->sessionGeneration!=e4.sessionGeneration))return false;
        if(bool(parent)!=bool(previous))return false;
        if(parent&&(parent->reference00!=1u||parent->kind!=OriginalWreckComponentKind50LA::piece2CB4B0||
           parent->sessionGeneration!=e4.sessionGeneration||previous->reference00!=1u||
           previous->kind!=OriginalWreckComponentKind50LA::detector295290||previous->parent1C!=parent||
           previous->sessionGeneration!=e4.sessionGeneration||
           previous->next14!=(references?&e4:nullptr)))return false;
        if(!freshShape40_||freshShape40KnownMask_!=(shape8C_?0xffffu:0u))return false;
        if(shape8C_&&(shape8C_->racerModel110!=&model||shape8C_->model110.value))return false;
        const auto* r=freshResourceHistory_?freshResourceHistory_:freshLoadingHistory_;
        if(r&&(r->resolver_.get()!=freshResolver_||r->file_.resolver!=freshFile_.resolver||r->file_.row!=freshFile_.row||
           arrayAddresses(*r)!=freshArrayAddresses_||arraySizes(*r)!=freshArraySizes_))return false;
        if(freshResourceHistory_){
            if(loading_||!r->live())return false;
            return freshWarm_?(loaded_&&boundary_==0x1255a4u):
                ((loaded_&&boundary_==0x1250ecu)||(!loaded_&&boundary_==0x1250e4u));
        }
        if(loaded_)return false;
        if(loading_&&(!shape8C_||loading_->constructed_||loading_->retired_||loading_->reference60_))return false;
        if(freshWarm_)return !shape8C_&&!loading_&&boundary_==0x125500u;
        switch(boundary_){
        case 0x124ff0u:return !shape8C_&&!loading_;
        case 0x125890u:return shape8C_&&!loading_;
        case 0x125ac4u:case 0x125bb0u:case 0x125cd4u:case 0x125db0u:
        case 0x126038u:case 0x12625cu:return shape8C_&&loading_;
        default:return false;
        }
    }
    bool preflightFreshWreckPayload(const OriginalRacerSceneObject50LA& model,
        const OriginalWreckComponent50LA& e4,const std::optional<const void*>& actual8C,
        const std::optional<const void*>& actual90,
        const OriginalWreckComponent50LA* parent=nullptr,
        const OriginalWreckComponent50LA* previous=nullptr,
        const OriginalWreckComponent50LA* expectedNextE8=nullptr)const noexcept {
        return freshWreckCutCurrent(model,e4,actual8C,actual90,1u,parent,previous,expectedNextE8);
    }
    bool releaseFreshWreckPayloadAfterReferenceDrop(const OriginalRacerSceneObject50LA& model,
        const OriginalWreckComponent50LA& e4,std::optional<const void*>& actual8C,
        std::optional<const void*>& actual90,
        const OriginalWreckComponent50LA* parent=nullptr,
        const OriginalWreckComponent50LA* previous=nullptr)noexcept {
        //Source1424E0 has already unlinked D8.next. Dying E4 parent/previous
        //remain history; the same live Model/parent/D8 are still pinned.
        if(!freshWreckCutCurrent(model,e4,actual8C,actual90,0u,parent,previous))return false;
        if(freshResourceHistory_){
            //Resource124 is genuinely constructed/ref60-positive, including
            //failed native1326A0 publication. Its source destructor is readable.
            if(!release11B600())std::terminate();
            actual8C=nullptr;actual90=nullptr; //11B900/904, source shape lifetime ends.
            freshRelease_=FreshWreckRelease::sourceFull11B600;
        }else{
            //Before125024, guest shape124 was never initialized. Native
            //staging allocations cannot be interpreted as source124=NULL.
            //Cancel only owned prefixes, no cache clear/ref60 decrement or
            //guest11B918 completion. Preserve actual alias/stoppedPC history.
            if(loading_){freeArrays11AEE0(*loading_);loading_.reset();}
            shape8C_.reset();loaded_=false;retired_=true;
            freshRelease_=FreshWreckRelease::nativeIncompleteCancellation;
        }
        return true;
    }
    FreshWreckRelease freshWreckReleaseKind()const noexcept{return freshRelease_;}

    bool freshShape40Current(const BodyMatrix48G& actualShape40)const noexcept {
        return freshWreck_&&loaded_&&!retired_&&freshShape40_==&actualShape40&&
            freshShape40KnownMask_==0xffffu&&shape8C_&&shape8C_->resource124&&
            shape8C_->resource124->live();
    }
    void storeFreshShape40Word(unsigned lane,std::uint32_t bits)noexcept {
        std::memcpy(&freshShape40_->v[lane],&bits,sizeof(bits));
        freshShape40KnownMask_=std::uint16_t(freshShape40KnownMask_|(1u<<lane));
    }
    void loadFreshShape40Cold125890(const std::vector<std::uint8_t>& raw)noexcept {
        //Native decoded-COL transport: the SAME admitted raw file's body starts
        //at16. Actual1258C0..125A00 reads XYZ, then125900/958/9B0/A0C writes W.
        //No finite/affine refusal here; these are bit-preserving source stores.
        for(unsigned row=0;row<4u;++row){
            for(unsigned col=0;col<3u;++col)
                storeFreshShape40Word(4u*row+col,word(raw,16u+4u*(3u*row+col)));
            storeFreshShape40Word(4u*row+3u,row==3u?0x3f800000u:0u);
        }
    }
    void loadFreshShape40Warm125570(const Resource& resource)noexcept {
        //12557C/580: eight ordered word-pairs from genuine shared resource20.
        const auto& matrix=resource.mesh().local_matrix;
        for(unsigned pair=0;pair<8u;++pair)for(unsigned part=0;part<2u;++part){
            const unsigned lane=2u*pair+part;std::uint32_t bits;
            std::memcpy(&bits,&matrix[lane],sizeof(bits));storeFreshShape40Word(lane,bits);
        }
    }

    static std::uint32_t word(const std::vector<std::uint8_t>& b,std::size_t at)noexcept {
        return std::uint32_t(b[at])|(std::uint32_t(b[at+1])<<8)|
            (std::uint32_t(b[at+2])<<16)|(std::uint32_t(b[at+3])<<24);
    }
    static void freeArrays11AEE0(Resource& r)noexcept {
        //11AF24/3C/5C/7C. Shared RA-backed resources retire their actual
        //borrowed view; immutable native cache pins may remain afterwards.
        r.releaseArrays11AEE0();
    }
    bool load11B920(const OriginalWreckAssets50LA& assets,SpringSceneId48N model,
        const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        using Kind=rr::assets::OriginalStandardModelFileKind50LA;
        if(attempted_||retired_||!assets.prepared()||!model.value||!resolver)return false;
        const auto id=assets.targetFileIdentity();const auto& raw=assets.originalMembers()[3];
        const auto& target=assets.targetE4();rr::assets::OriginalStandardModelFileView50LA file;
        if(id.resolver!=resolver.get()||!resolver->modelFile(id,file,diagnostic)||
           file.kind!=Kind::colType0||!file.bytes||*file.bytes!=raw||raw.size()<84u||
           !file.member||
           target.mesh.nodes.empty()||target.mesh.faces.empty()||target.mesh.vertices.empty()||
           target.radii.size()!=target.mesh.nodes.size()||target.mesh.reverse_mode!=0u||
           word(raw,68)!=target.mesh.vertices.size()||word(raw,72)!=target.mesh.nodes.size()||
           word(raw,76)!=target.mesh.faces.size()||
           (word(raw,80)&&!target.mesh.vertex_normals.empty()))return false;
        return loadValidatedType0(file,raw,target.mesh,target.radii,model,nullptr,resolver,nullptr,diagnostic);
    }
    //Same allocation/cache/ref60 machinery, supplied by the SAME retained
    //Racer COL owner. No OriginalWreckAssets facade or invented model ID.
    bool loadRacer11B920(const rr::assets::OriginalStandardModelFileView50LA& file,
        const std::vector<std::uint8_t>& raw,const rr::assets::CollisionMesh48I& mesh,
        const std::vector<float>& radii,const OriginalRacerSceneObject50LA& model,
        const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        std::optional<const void*>& actualShape8C,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        using Kind=rr::assets::OriginalStandardModelFileKind50LA;
        if(attempted_||retired_||!resolver||!actualShape8C.has_value()||*actualShape8C||
           file.id.resolver!=resolver.get()||file.kind!=Kind::colType0||!file.bytes||
           *file.bytes!=raw||raw.size()<84u||!file.member||file.animation1536F0||file.geometry240110||
           mesh.nodes.empty()||mesh.faces.empty()||mesh.vertices.empty()||
           radii.size()!=mesh.nodes.size()||mesh.reverse_mode!=0u||
           word(raw,68)!=mesh.vertices.size()||word(raw,72)!=mesh.nodes.size()||
           word(raw,76)!=mesh.faces.size()||(word(raw,80)&&!mesh.vertex_normals.empty()))return false;
        return loadValidatedType0(file,raw,mesh,radii,{},&model,resolver,&actualShape8C,diagnostic);
    }
    bool loadValidatedType0(const rr::assets::OriginalStandardModelFileView50LA& file,
        const std::vector<std::uint8_t>& raw,const rr::assets::CollisionMesh48I& mesh,
        const std::vector<float>& radii,SpringSceneId48N model,const OriginalRacerSceneObject50LA* racerModel,
        const std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>& resolver,
        std::optional<const void*>* actualShape8C,
        rr::assets::OriginalStandardTextureDiagnostic50LA& diagnostic)noexcept {
        const auto id=file.id;
        auto resource=file.collision125014;
        if(resource&&(!resource->live()||resource->reference60_==std::numeric_limits<std::int32_t>::max()))return false;
        attempted_=true;boundary_=resource?0x125500u:0x124ff0u;
        try {shape8C_=std::make_unique<Shape>();}catch(...){return false;}
        //Source125508/124FF8 publication precedes the model110 store.
        if(actualShape8C)*actualShape8C=shape8C_.get();
        shape8C_->model110=model;shape8C_->racerModel110=racerModel;
        if(resource){
            if(freshWreck_)loadFreshShape40Warm125570(*resource);
            //125594 followed by12559C/5A0. Native lookup alone did not retain.
            shape8C_->resource124=std::move(resource);++shape8C_->resource124->reference60_;
            loaded_=true;boundary_=0x1255a4u;return true;
        }
        boundary_=0x125890u;
        if(freshWreck_)loadFreshShape40Cold125890(raw);
        try {
            //Host bookkeeping allocation is not an emulated retail0x70 block.
            loading_=std::make_shared<Resource>();loading_->resolver_=resolver;loading_->file_=id;
            auto& r=*loading_;if(!freshWreck_)r.mesh_.local_matrix=mesh.local_matrix;r.mesh_.reverse_mode=mesh.reverse_mode;
            r.vertexCount04_=word(raw,68);r.packed18_=word(raw,80);r.root1C_=0;
            boundary_=r.packed18_?0x125ac4u:0x125bb0u;r.mesh_.vertices=mesh.vertices;
            boundary_=r.packed18_?0x125cd4u:0x125db0u;r.mesh_.vertex_normals=mesh.vertex_normals;
            boundary_=0x126038u;r.mesh_.nodes=mesh.nodes;r.radii_=radii;
            boundary_=0x12625cu;r.mesh_.faces=mesh.faces;
        }catch(...){return false;}
        //Only now does resource124 exist in source terms. Arrays retain the
        //same native addresses on warm sharing; no per-query mesh clones.
        auto& r=*loading_;
        if(freshWreck_){
            //1250B0/B4: shape40 -> resource20, before1250D4 ref60/cache.
            //loading_ is only native staging until this real resource prefix.
            for(unsigned pair=0;pair<8u;++pair)for(unsigned part=0;part<2u;++part){
                const unsigned lane=2u*pair+part;
                std::memcpy(&r.mesh_.local_matrix[lane],&freshShape40_->v[lane],sizeof(float));
            }
        }
        r.constructed_=true;r.reference60_=1;
        shape8C_->resource124=std::move(loading_);boundary_=0x1250e4u;
        if(!resolver->publishCollision1326A0(id,shape8C_->resource124,diagnostic))return false;
        loaded_=true;boundary_=0x1250ecu;return true;
    }
    bool release11B600()noexcept {
        if(!attempted_||retired_)return false;
        if(shape8C_&&shape8C_->resource124){
            auto& r=*shape8C_->resource124;if(!r.live()||!r.resolver_)return false;
            r.resolver_->clearCollision1327D0(&r); //11B650, even for nonfinal release
            --r.reference60_; //11AEFC..11AF04
            if(!r.reference60_){freeArrays11AEE0(r);r.retired_=true;}
            //At positive refs source shape124 remains stale until shape8C
            //is freed. Native pins can now end because this shape ends too.
        }else if(loading_){freeArrays11AEE0(*loading_);loading_.reset();} //source124 NULL branch
        shape8C_.reset();loaded_=false;retired_=true;boundary_=0x11b918u;return true;
    }
public:
    OriginalWreckType0Owners50LA()=default;
    OriginalWreckType0Owners50LA(const OriginalWreckType0Owners50LA&)=delete;
    OriginalWreckType0Owners50LA& operator=(const OriginalWreckType0Owners50LA&)=delete;
    ~OriginalWreckType0Owners50LA(){if(shape8C_||loading_)std::terminate();}
    bool attempted()const noexcept{return attempted_;}
    bool loaded()const noexcept{return loaded_;}
    bool retired()const noexcept{return retired_;}
    std::uint32_t sourceBoundary()const noexcept{return boundary_;}
    const Resource* resource()const noexcept{return shape8C_?shape8C_->resource124.get():nullptr;}
};
}
