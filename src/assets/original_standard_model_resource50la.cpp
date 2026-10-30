#include "original_standard_model_resource50la.h"
#include "scene_asset48d.h"
#include <cstring>
#include <atomic>
#include "original_standard_material_table50la.h"
#include "../game/original_racer_model_scene50la.h"
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
template<class Bytes>
bool OriginalStandardModelResource50LA::prepareOriginalLimitsImpl(const std::string& member,
    Bytes&& bytes,const SceneLimits48D& limits,D& d) noexcept {
    d={};if(storage_)return fail(d,S::alreadyPrepared,0x26E820);
    if(member.empty()||member.size()>260||bytes.empty()||bytes.size()>maxPsgBytes49O)
        return fail(d,S::input,0x26E990);
    try {
        auto next=std::make_unique<Storage>();next->member=member;next->bytes=std::forward<Bytes>(bytes);
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
bool OriginalStandardModelResource50LA::prepareOriginalLimits(const std::string& member,
    const std::vector<std::uint8_t>& bytes,const SceneLimits48D& limits,D& d) noexcept {
    return prepareOriginalLimitsImpl(member,bytes,limits,d);
}
bool OriginalStandardModelResource50LA::prepareOriginalLimits(const std::string& member,
    std::vector<std::uint8_t>&& bytes,const SceneLimits48D& limits,D& d) noexcept {
    return prepareOriginalLimitsImpl(member,std::move(bytes),limits,d);
}
bool OriginalStandardModelResource50LA::prepared() const noexcept{return bool(storage_);}
const std::string* OriginalStandardModelResource50LA::member() const noexcept{return storage_?&storage_->member:nullptr;}
const std::vector<std::uint8_t>* OriginalStandardModelResource50LA::originalBytes() const noexcept{return storage_?&storage_->bytes:nullptr;}
const Scene48D* OriginalStandardModelResource50LA::scene() const noexcept{return storage_?&storage_->scene:nullptr;}
const PsgNode49O* OriginalStandardModelResource50LA::sourceNodes() const noexcept{return storage_?storage_->rows.data():nullptr;}
std::uint32_t OriginalStandardModelResource50LA::nodeCount() const noexcept{return storage_?storage_->count:0u;}
const float* OriginalStandardModelResource50LA::sourceLodScale14() const noexcept{return storage_?&storage_->lodScale14:nullptr;}

namespace {
//Source process state: lazy byte3D64BC and enabled word442F40.
std::atomic<std::uint32_t> sourceGeometryAssertion26E1C8{0};
constexpr std::uint32_t sourceGeometryAssertionUnknown=0x80000000u;
bool sourceGeometryAssertionStore(unsigned bit)noexcept {
    auto value=sourceGeometryAssertion26E1C8.load(std::memory_order_acquire);
    for(;;){if(value&sourceGeometryAssertionUnknown)return false;
        if(sourceGeometryAssertion26E1C8.compare_exchange_weak(value,value|(1u<<bit),
            std::memory_order_acq_rel,std::memory_order_acquire))return true;}
}
}
struct OriginalStandardModelGeometry50LA::Storage {    struct SourceSurface {
        std::int32_t material00=-1;
        std::uint32_t vtable08=0;
        std::uint8_t attributes0C=0,uvCount0D=0,palette0E=0xff,flags0F=0,temp=0;
        std::array<std::uint32_t,2> scale{{0,0}};
        std::uint16_t vertexCount20=0,stripCount22=0;
        bool constructed=false,returned=false,payloadAllocated=false,payloadRead=false,headerStored=false;
        std::uint16_t readMask=0;
        std::uint8_t scalesRead=0;
        std::uint32_t rawPayloadSize=0,roundedPayloadSize=0;
        std::unique_ptr<std::uint8_t[]> payloadAllocation;
        std::uint8_t* packet10=nullptr;
        const std::uint8_t* headerAddress04=nullptr;
        std::uint16_t headerQwc00=0;
        std::uint8_t headerZero02=0,headerTag03=0;
        std::uint32_t headerZero08=0,headerZero0C=0;
    };
    struct SourceLod {
        std::uint32_t surfaceCount=0,cutoffBits=0;
        bool zeroed=false,cutoffRead=false,countRead=false,allocationReturned=false,cookieWritten=false,published=false;
        std::uint32_t constructed=0,loaded=0;
        std::unique_ptr<SourceSurface[]> surfaces;
        SourceSurface* published08=nullptr;
    };
    struct SourceLoad {
        bool attempted=false,countRead=false,allocationReturned=false,zeroed=false,lodsReturned=false;
        std::uint8_t assertionHistory=0;
        std::uint32_t lod=0,surface=0,totalSurfaces=0,pc=0;
        std::size_t cursorStart=0,cursorEnd=0;
        const std::vector<std::uint8_t>* bytes=nullptr;
        std::unique_ptr<SourceLod[]> lods;
        SourceLod* published18=nullptr;
    } sourceLoad;
    struct SourcePaletteDescriptor {
        std::uint32_t count00=0;
        bool countStored=false,sizesAllocated=false,pointersAllocated=false,bytesAllocated=false;
        std::unique_ptr<std::uint32_t[]> sizes;
        std::unique_ptr<const std::uint8_t*[]> pointers;
        std::unique_ptr<std::uint8_t[]> bytes;
        const std::uint32_t* published04=nullptr;
        const std::uint8_t* const* published08=nullptr;
        const std::uint8_t* published0C=nullptr;
    };
    struct SourcePalette {
        bool attempted=false,countRead=false,descriptorAllocated=false,published=false;
        bool byteCountRead=false,dataRead=false,returned=false;
        std::uint32_t rawCount=0,byteCount=0,index=0,offset=0,pc=0;
        //index is native completed-slot custody, not original early S1.
        std::uint32_t sourceIterationS1=0,sourceStrideS2=0;
        //Existing native adapter admits one palette. Receipt capacity is not
        //a source count or allocation; source count stores precede this bound.
        std::array<bool,1> lengthRead{{false}},sizeStored{{false}},pointerStored{{false}};
        std::array<std::uint32_t,1> rawLength{{0}},pointerOffset{{0}};
        std::size_t cursorStart=0,cursorEnd=0;
        std::unique_ptr<SourcePaletteDescriptor> descriptor;
        SourcePaletteDescriptor* published20=nullptr;
    } sourcePalette;


    std::shared_ptr<const OriginalStandardModelResource50LA> payload;
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver;
    OriginalStandardModelFileId50LA file{};
    const rr::game::OriginalRacerModelSceneOwner50LA* prototype=nullptr;
    bool sourceMode=false,sourceReferenceKnown=false,sourceScaleKnown=false;
    bool sourcePointer18Known=false,sourcePointer20Known=false,sourcePrototypeKnown=false;
    bool sourceConstructorReturned=false;
    const OriginalSourcePsgFile50LA* sourcePrototypeFile=nullptr;
    const rr::game::OriginalRacerModelSceneOwner50LA* sourceOwner=nullptr;
    std::uint32_t sourceVtable08=0;
    std::optional<std::uint32_t> sourceCount10;
    struct SourceWarmBorrow {
        const rr::game::OriginalRacerModelSceneOwner50LA* target=nullptr;
        std::shared_ptr<OriginalSourcePsgFile50LA> file;
        bool retained=false;
    };
    std::vector<SourceWarmBorrow> sourceWarmBorrows;
    std::int64_t references=1; //1048F0 reached through26E050.
    float scale=1.f; //26E078/26E07C.
    std::vector<std::vector<std::int32_t>> surfaceMaterials;
    std::vector<std::vector<OriginalStandardSurface246300_50LA>> runtimeSurfaces;
    std::vector<std::vector<std::uint8_t>> palettes;
    //Native provenance for each real26E820 owner, not another source ref.
    std::vector<const rr::game::OriginalRacerModelSceneOwner50LA*> models;
    bool released=false;
};
bool OriginalStandardModelGeometry50LA::constructSource240110(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,OriginalStandardModelFileId50LA file,
    const rr::game::OriginalRacerModelSceneOwner50LA* owner,D& d)noexcept {
    d={};if(storage_||!resolver||file.resolver!=resolver.get()||!owner)
        return fail(d,S::phase,0x240110);
    //Native bookkeeping admitted before any source1048F0 store; failure leaves
    //the caller's actual outer allocation owned, not a source NULL return.
    std::unique_ptr<Storage> next;try{next=std::make_unique<Storage>();}
    catch(...){return fail(d,S::allocation,0x240110);}
    next->sourceMode=true;next->sourceOwner=owner;next->resolver=std::move(resolver);next->file=file;
    next->references=1;next->sourceReferenceKnown=true; //104900 SD1, no423F60 count.
    next->sourceVtable08=0x3d2460; //26E06C
    next->sourceCount10=0; //26E070
    next->sourcePointer18Known=true; //26E074 NULL; no LOD allocations.
    next->scale=1.f;next->sourceScaleKnown=true; //26E07C
    next->sourceVtable08=0x3d2010; //24012C
    next->sourcePointer20Known=true; //240130 NULL; no palette allocations.
    //26E050/240110 never write prototype1C, field0C or field24. Native
    //null/default backing is not known source state for those fields.
    next->sourceConstructorReturned=true;storage_=std::move(next);return true;
}
bool OriginalStandardModelGeometry50LA::sourceWarmLedgerCurrent()const noexcept {
    if(!storage_||!storage_->sourceMode)return false;
    const auto& s=*storage_;std::int64_t expected=1;
    for(std::size_t i=0;i<s.sourceWarmBorrows.size();++i){const auto& b=s.sourceWarmBorrows[i];
        if(!b.target||b.target==s.sourceOwner||!b.file||b.file->warmResourceBorrower_!=b.target)return false;
        for(std::size_t j=0;j<i;++j)if(s.sourceWarmBorrows[j].target==b.target||s.sourceWarmBorrows[j].file==b.file)return false;
        if(b.retained){if(expected==std::numeric_limits<std::int64_t>::max())return false;++expected;}
    }
    return s.references==expected;
}
bool OriginalStandardModelGeometry50LA::registerSourceWarmBorrow(
    const rr::game::OriginalRacerModelSceneOwner50LA* target,
    const std::shared_ptr<OriginalSourcePsgFile50LA>& file,D& d)noexcept {
    d={};if(!storage_||!target||target==storage_->sourceOwner||!file||file->warmResourceBorrower_||
       !sourceConstructorCurrent(storage_->sourceOwner,storage_->resolver.get(),storage_->file)||
       !file->warmGeometryCurrent(this,storage_->sourceOwner))return fail(d,S::phase,0x26e9f0);
    for(const auto& b:storage_->sourceWarmBorrows)if(b.target==target)return fail(d,S::phase,0x26e9f0);
    //Native ledger allocation precedes source1B4 publication. No source ref yet.
    try{storage_->sourceWarmBorrows.push_back({target,file,false});}
    catch(...){return fail(d,S::allocation,0x26e9f0);}
    file->warmResourceBorrower_=target;return true;
}
bool OriginalStandardModelGeometry50LA::sourceWarmBorrowCurrent(
    const rr::game::OriginalRacerModelSceneOwner50LA* target,
    const OriginalSourcePsgFile50LA* file,bool retained)const noexcept {
    if(!storage_||!sourceConstructorCurrent(storage_->sourceOwner,storage_->resolver.get(),storage_->file))return false;
    for(const auto& b:storage_->sourceWarmBorrows)
        if(b.target==target&&b.file.get()==file)return b.retained==retained;
    return false;
}
bool OriginalStandardModelGeometry50LA::retainSourceWarm104960(
    const rr::game::OriginalRacerModelSceneOwner50LA* target,const OriginalSourcePsgFile50LA* file,D& d)noexcept {
    d={};if(!file||!sourceWarmBorrowCurrent(target,file,false)||
       !file->warmGeometryCurrent(this,storage_->sourceOwner))return fail(d,S::phase,0x104960);
    //104960 LD/DADDIU/JR delay SD. Native signed overflow is bounded before
    //unrepresented wrap; the caller's actual1B4 publication remains intact.
    if(storage_->references==std::numeric_limits<std::int64_t>::max())return fail(d,S::overflow,0x104964);
    for(auto& b:storage_->sourceWarmBorrows)if(b.target==target){
        ++storage_->references;b.retained=true;d.sourcePC=0x10496c;return true;}
    return fail(d,S::phase,0x104960);
}
bool OriginalStandardModelGeometry50LA::withdrawSourceWarmBorrowNative(
    const rr::game::OriginalRacerModelSceneOwner50LA* target,OriginalSourcePsgFile50LA* file)noexcept {
    if(!storage_||!file||!sourceConstructorCurrent(storage_->sourceOwner,storage_->resolver.get(),storage_->file))return false;
    for(auto it=storage_->sourceWarmBorrows.begin();it!=storage_->sourceWarmBorrows.end();++it){
        if(it->target!=target||it->file.get()!=file)continue;
        if(it->retained)--storage_->references;
        file->warmResourceBorrower_=nullptr;storage_->sourceWarmBorrows.erase(it);return true;
    }
    return false;
}
bool OriginalStandardModelGeometry50LA::sourceConstructorCurrent(
    const rr::game::OriginalRacerModelSceneOwner50LA* owner,
    const OriginalStandardTextureResolver50LA* resolver,OriginalStandardModelFileId50LA file)const noexcept {
    if(!storage_)return false;const auto& s=*storage_;
    std::uint32_t scaleBits;std::memcpy(&scaleBits,&s.scale,4);
    return scaleBits==0x3f800000u&&s.sourceMode&&s.sourceConstructorReturned&&!s.released&&s.sourceOwner==owner&&
        s.resolver.get()==resolver&&s.file.resolver==file.resolver&&s.file.row==file.row&&
        s.sourceReferenceKnown&&sourceWarmLedgerCurrent()&&s.sourceVtable08==0x3d2010&&
        s.sourceCount10&&(*s.sourceCount10==0||s.sourceLoad.attempted)&&sourceLoadCurrent()&&sourcePaletteCurrent()&&s.sourcePointer18Known&&s.sourcePointer20Known&&s.sourceScaleKnown&&
        (s.sourcePrototypeKnown?(s.prototype==owner&&s.sourcePrototypeFile):(!s.prototype&&!s.sourcePrototypeFile))&&
        !s.payload&&s.models.empty()&&
        s.surfaceMaterials.empty()&&s.runtimeSurfaces.empty()&&s.palettes.empty();
}
bool OriginalStandardModelGeometry50LA::cancelSourceConstructorNative(
    const rr::game::OriginalRacerModelSceneOwner50LA* owner,
    const OriginalStandardTextureResolver50LA* resolver,OriginalStandardModelFileId50LA file)noexcept {
    if(!sourceConstructorCurrent(owner,resolver,file)||!storage_->sourceWarmBorrows.empty())return false;
    //Source-only raw arrays/payloads carry no material retain. Native
    //cancellation withdraws the actual prototype borrow below and frees arrays
    //with Storage; any future cache association must retire first. Raw indices
    //must NOT trigger299AC0 release. No source240150 call is claimed.
    storage_->references=0;storage_->prototype=nullptr;storage_->sourcePrototypeFile=nullptr;
    storage_->released=true;storage_->sourceOwner=nullptr;return true;
}

bool OriginalStandardModelGeometry50LA::sourceLoadCurrent()const noexcept {
    if(!storage_)return false;const auto& s=*storage_;const auto& h=s.sourceLoad;
    if(!h.attempted)return !h.countRead&&!h.allocationReturned&&!h.lods&&!h.published18&&!h.zeroed&&!h.lodsReturned;
    if(!h.bytes||h.cursorStart>h.cursorEnd||h.cursorEnd>h.bytes->size())return false;
    if(!h.countRead)return !h.allocationReturned&&!h.lods&&!h.published18&&!h.zeroed&&!h.lodsReturned;
    if(!s.sourceCount10)return false;const auto count=*s.sourceCount10;
    if(!h.allocationReturned)return !h.lods&&!h.published18&&!h.zeroed&&!h.lodsReturned;
    if(count>4||h.published18!=h.lods.get()||h.lod>count||h.totalSurfaces>16384)return false;
    if(!h.zeroed)return !h.lodsReturned&&h.lod==0&&h.totalSurfaces==0;
    if(count&&!h.lods)return false;
    std::uint32_t total=0;
    for(std::uint32_t i=0;i<count;++i){const auto& l=h.lods[i];
        if(!l.zeroed||l.loaded>l.constructed||l.constructed>l.surfaceCount)return false;
        if(l.countRead&&!l.cutoffRead)return false;
        if(!l.countRead&&(l.surfaceCount||l.allocationReturned||l.surfaces||l.published))return false;
        if(!l.allocationReturned&&(l.surfaces||l.cookieWritten||l.constructed||l.loaded||l.published||l.published08))return false;
        if(l.allocationReturned){if(l.surfaceCount>16384u-total)return false;total+=l.surfaceCount;
            if(l.cookieWritten!=bool(l.surfaces))return false;}
        if(l.published){if(l.published08!=l.surfaces.get()||(l.surfaces&&l.constructed!=l.surfaceCount))return false;}
        else if(l.published08||l.loaded)return false;
        if(i<h.lod&&(!l.published||l.loaded!=l.surfaceCount))return false;
        if(l.surfaces)for(std::uint32_t j=0;j<l.constructed;++j){const auto& f=l.surfaces[j];
            if(!f.constructed||f.vtable08!=0x3d20c0||f.flags0F!=(f.payloadAllocated?1u:0u)||f.scalesRead>2||f.scalesRead>f.uvCount0D)return false;
            //Raw-read masks form a prefix of eight independent source reads.
            if(f.readMask>255u||(f.readMask&(f.readMask+1u)))return false;
            if(f.scalesRead&&!(f.readMask&16u))return false;
            if((f.readMask&32u)&&f.scalesRead!=f.uvCount0D)return false;
            if(!f.payloadAllocated&&(f.payloadAllocation||f.packet10||f.payloadRead||f.headerStored))return false;
            if(f.payloadAllocated){
                if(f.readMask!=255u||!f.rawPayloadSize||f.rawPayloadSize>max_scene_bytes48d||
                   f.rawPayloadSize>0xfffffff0u||f.roundedPayloadSize!=((f.rawPayloadSize+15u)&~15u))return false;
                if(bool(f.payloadAllocation)!=bool(f.packet10))return false;
                if(f.payloadAllocation){const auto raw=reinterpret_cast<std::uintptr_t>(f.payloadAllocation.get());
                    if(reinterpret_cast<std::uintptr_t>(f.packet10)!=((raw+63u)&~std::uintptr_t(63u)))return false;}
            }
            if(f.payloadRead&&!f.packet10)return false;
            if(f.headerStored&&(!f.payloadRead||f.headerAddress04!=f.packet10+16||
                f.headerQwc00!=std::uint16_t(f.roundedPayloadSize>>4)||f.headerZero02||f.headerTag03!=0x30||f.headerZero08||f.headerZero0C))return false;
            if(f.returned&&(f.readMask!=255u||f.scalesRead!=f.uvCount0D||
                (f.rawPayloadSize?!f.headerStored:f.payloadAllocated)))return false;
            if((j<l.loaded)!=f.returned)return false;
        }
    }
    if(total!=h.totalSurfaces)return false;
    return !h.lodsReturned||(h.lod==count&&h.pc==0x26e39c);
}
bool OriginalStandardModelGeometry50LA::loadSourceOrdinary26E170(
    const rr::game::OriginalRacerModelSceneOwner50LA* owner,
    const std::vector<std::uint8_t>& bytes,std::size_t& cursor,OriginalSourcePsgFile50LA& sourceFile,D& d)noexcept {
    d={};if(!storage_||!sourceConstructorCurrent(owner,storage_->resolver.get(),storage_->file)||
        storage_->sourceLoad.attempted||cursor>bytes.size())return fail(d,S::phase,0x240230);
    if(!sourceFile.current()||sourceFile.nativeFile().resolver!=storage_->resolver.get()||
       sourceFile.nativeFile().row!=storage_->file.row)return fail(d,S::cache,0x240230);
    auto& s=*storage_;auto& h=s.sourceLoad;
    OriginalStandardModelFileView50LA file;
    if(!s.resolver->modelFile(s.file,file,d.cache)||file.bytes!=&bytes||file.geometry240110)
        return fail(d,S::cache,0x240230);
    h.attempted=true;h.bytes=&bytes;h.cursorStart=h.cursorEnd=cursor;
    const auto stop=[&](S status,std::uint32_t pc){h.pc=pc;h.cursorEnd=cursor;return fail(d,status,pc);};
    const auto read=[&](void* destination,std::size_t count,std::uint32_t pc){
        h.pc=pc;OriginalStandardTextureDiagnostic50LA io;
        const bool okay=sourceFile.readExactNative(destination,count,cursor,io);
        h.cursorEnd=cursor;if(!okay&&io.sourcePC)h.pc=io.sourcePC;return okay;
    };
    const auto read32=[&](std::uint32_t& value,std::uint32_t pc){
        std::array<std::uint8_t,4> raw{};if(!read(raw.data(),4,pc))return false;
        value=std::uint32_t(raw[0])|(std::uint32_t(raw[1])<<8)|(std::uint32_t(raw[2])<<16)|(std::uint32_t(raw[3])<<24);return true;
    };
    const auto read16=[&](std::uint16_t& value,std::uint32_t pc){
        std::array<std::uint8_t,2> raw{};if(!read(raw.data(),2,pc))return false;
        value=std::uint16_t(std::uint16_t(raw[0])|(std::uint16_t(raw[1])<<8));return true;
    };
    std::uint32_t count;
    if(!read32(count,0x26e1a8))return stop(S::input,h.pc);
    s.sourceCount10=count;h.countRead=true;
    //Existing Scene48D bounds, not guest signed arithmetic or a larger budget.
    if(count>4)return stop(S::overflow,0x26e1bc);
    h.lods.reset(new(std::nothrow) Storage::SourceLod[count]);h.allocationReturned=true;
    h.published18=h.lods.get(); //26E1C4, including NULL.
    h.assertionHistory=1;
    auto assertion=sourceGeometryAssertion26E1C8.load(std::memory_order_acquire);
    if(assertion&sourceGeometryAssertionUnknown)return stop(S::phase,0x26e1c8);
    if(assertion&1u)h.assertionHistory|=2u;
    else{
        if(!sourceGeometryAssertionStore(1))return stop(S::phase,0x26e1dc);h.assertionHistory|=4u;
        if(!sourceGeometryAssertionStore(0))return stop(S::phase,0x26e1e0);h.assertionHistory|=8u;
    }
    assertion=sourceGeometryAssertion26E1C8.load(std::memory_order_acquire);
    if(assertion&sourceGeometryAssertionUnknown)return stop(S::phase,0x26e1e8);
    if(assertion&2u){h.assertionHistory|=16u;if(!h.published18)return stop(S::allocation,0x26e200);}
    h.assertionHistory|=32u;
    if(count&&!h.published18)return stop(S::allocation,0x26e244);
    for(std::uint32_t i=0;i<count;++i){auto& l=h.lods[i];l.surfaceCount=0;l.cutoffBits=0;l.published08=nullptr;l.zeroed=true;}
    h.zeroed=true; //26E244 zero-byte memset permits NULL only when count0.
    for(h.lod=0;h.lod<count;++h.lod){
        auto& l=h.lods[h.lod];
        if(!read32(l.cutoffBits,0x26e27c))return stop(S::input,h.pc);l.cutoffRead=true;
        std::uint32_t surfaceCount;
        if(!read32(surfaceCount,0x26e298))return stop(S::input,h.pc);
        l.surfaceCount=surfaceCount;l.countRead=true; //26E2B8 before allocation admission.
        if(surfaceCount>16384u-h.totalSurfaces)return stop(S::overflow,0x26e2cc);
        h.totalSurfaces+=surfaceCount;
        l.surfaces.reset(new(std::nothrow) Storage::SourceSurface[surfaceCount]);l.allocationReturned=true;
        //2E8990 NULL returns NULL without touching cookie or calling constructors.
        if(l.surfaces){l.cookieWritten=true;
            for(;l.constructed<surfaceCount;++l.constructed){auto& f=l.surfaces[l.constructed];
                f.material00=-1; //299AB0; aux04=NULL is owned native absence.
                f.vtable08=0x3d20c0;f.attributes0C=0;f.uvCount0D=0;f.palette0E=0xff;f.flags0F=0;
                f.scale={{0x3f800000u,0x3f800000u}};f.packet10=nullptr;
                f.flags0F=0; //24622C ANDI a0,$zero,1 is zero;24623C keeps bit0 clear.
                f.vertexCount20=0;f.stripCount22=0;f.constructed=true;
            }
        }
        l.published08=l.surfaces.get();l.published=true; //26E310 delay slot.
        for(h.surface=0;h.surface<surfaceCount;++h.surface){
            if(!l.published08)return stop(S::allocation,0x299bc8);
            auto& f=l.surfaces[h.surface];std::uint32_t material;
            if(!read32(material,0x299bc8))return stop(S::input,h.pc);
            std::memcpy(&f.material00,&material,4);f.readMask|=1u;
            if(!read(&f.attributes0C,1,0x24634c))return stop(S::input,h.pc);f.readMask|=2u;
            if(!read(&f.uvCount0D,1,0x246368))return stop(S::input,h.pc);f.readMask|=4u;
            if(!read(&f.palette0E,1,0x246384))return stop(S::input,h.pc);f.readMask|=8u;
            if(!read(&f.temp,1,0x2463a0))return stop(S::input,h.pc);f.readMask|=16u;
            for(;f.scalesRead<f.uvCount0D;++f.scalesRead){
                if(f.scalesRead>=2)return stop(S::geometry,0x2463c8);
                if(!read32(f.scale[f.scalesRead],0x2463c8))return stop(S::input,h.pc);
            }
            if(!read16(f.vertexCount20,0x2463fc))return stop(S::input,h.pc);f.readMask|=32u;
            if(!read16(f.stripCount22,0x246418))return stop(S::input,h.pc);f.readMask|=64u;
            if(!read32(f.rawPayloadSize,0x246434))return stop(S::input,h.pc);f.readMask|=128u;
            //26ED60 proves actual NULL auxiliary input; no24645C repack read.
            if(f.rawPayloadSize){
                if(f.rawPayloadSize>max_scene_bytes48d||f.rawPayloadSize>0xfffffff0u)
                    return stop(S::overflow,0x2464b4);
                f.roundedPayloadSize=(f.rawPayloadSize+15u)&~15u;
                //Native extra63 bytes implement the source64-byte alignment;
                //they are not additional guest bytes or positive allocation proof.
                f.payloadAllocation.reset(new(std::nothrow) std::uint8_t[std::size_t(f.roundedPayloadSize)+16u+63u]);
                if(f.payloadAllocation){const auto address=reinterpret_cast<std::uintptr_t>(f.payloadAllocation.get());
                    f.packet10=reinterpret_cast<std::uint8_t*>((address+63u)&~std::uintptr_t(63u));}
                f.payloadAllocated=true; //2464D4 publishes nullable pointer10.
                f.flags0F=std::uint8_t((f.flags0F&~1u)|1u); //2464F4 before read.
                if(!f.packet10)return stop(S::allocation,0x246508);
                if(!read(f.packet10+16,f.roundedPayloadSize,0x246508))return stop(S::input,h.pc);f.payloadRead=true;
                f.headerQwc00=std::uint16_t(f.roundedPayloadSize>>4); //246568 SH truncation.
                f.headerZero02=0;f.headerTag03=0x30;f.headerAddress04=f.packet10+16;
                f.headerZero08=0;f.headerZero0C=0;f.headerStored=true;
            }
            f.returned=true;++l.loaded; //2465A4 return1 then26E354.
        }
    }
    h.lodsReturned=true;h.pc=0x26e39c;h.cursorEnd=cursor;d.sourcePC=h.pc;
    return true; //Only26E170 returned; next actual24024C palette read is separate.
}
bool OriginalStandardModelGeometry50LA::sourcePaletteCurrent()const noexcept {
    if(!storage_)return false;const auto& s=*storage_;const auto& h=s.sourcePalette;
    if(!h.attempted)return !h.countRead&&!h.descriptorAllocated&&!h.descriptor&&!h.published20&&!h.published&&!h.returned;
    if(!s.sourceLoad.lodsReturned||!s.sourceLoad.bytes||h.cursorStart!=s.sourceLoad.cursorEnd||
       h.cursorStart>h.cursorEnd||h.cursorEnd>s.sourceLoad.bytes->size())return false;
    if(!h.countRead)return !h.descriptorAllocated&&!h.descriptor&&!h.published&&!h.published20&&!h.returned;
    if(!h.rawCount)return !h.descriptorAllocated&&!h.descriptor&&!h.published&&!h.published20&&
        !h.byteCountRead&&!h.dataRead&&(!h.returned||h.pc==0x240394u);
    if(!h.descriptorAllocated||!h.published||h.published20!=h.descriptor.get())return false;
    if(!h.descriptor)return !h.byteCountRead&&!h.dataRead&&!h.returned&&h.index==0;
    const auto& p=*h.descriptor;
    if(!p.countStored||p.count00!=h.rawCount)return false;
    if(h.rawCount>1)return !p.sizesAllocated&&!p.pointersAllocated&&!p.bytesAllocated&&
        !p.sizes&&!p.pointers&&!p.bytes&&!h.byteCountRead&&!h.returned;
    if(p.published04!=p.sizes.get()||p.published08!=p.pointers.get()||p.published0C!=p.bytes.get()||
       (!p.sizesAllocated&&p.sizes)||(!p.pointersAllocated&&p.pointers)||(!p.bytesAllocated&&p.bytes))return false;
    if(p.pointersAllocated&&!p.sizesAllocated)return false;
    if(h.byteCountRead&&!p.pointersAllocated)return false;
    if(p.bytesAllocated&&(!h.byteCountRead||h.byteCount>max_scene_bytes48d))return false;
    if(h.dataRead&&(!p.bytesAllocated||(h.byteCount&&!p.bytes)))return false;
    if(h.index>h.rawCount)return false;
    for(std::uint32_t i=0;i<h.rawCount;++i){
        if(h.lengthRead[i]&&!h.dataRead)return false;
        if(h.sizeStored[i]&&(!h.lengthRead[i]||!p.sizes||p.sizes[i]!=h.rawLength[i]))return false;
        if(h.pointerStored[i]){
            if(!h.sizeStored[i]||!p.pointers||h.pointerOffset[i]>h.byteCount||(!p.bytes&&h.pointerOffset[i]))return false;
            const auto* expected=p.bytes?p.bytes.get()+h.pointerOffset[i]:nullptr;
            if(p.pointers[i]!=expected)return false;
        }
        if(i<h.index&&!h.pointerStored[i])return false;
    }
    if(h.sourceIterationS1!=(h.lengthRead[0]?1u:0u)||
       h.sourceStrideS2!=(h.pointerStored[0]?4u:0u))return false;
    //Only one admitted entry: exact source pointer offset starts at0; the
    //post-store addition is unsigned32 and may wrap without pointer access.
    if(h.pointerStored[0]&&h.pointerOffset[0]!=0)return false;
    if(h.index==0){if(h.offset!=0)return false;}
    else {const std::uint32_t length=h.rawLength[0];
        if(h.offset!=std::uint32_t(length+((4u-(length&3u))&3u)))return false;}
    return !h.returned||(h.index==h.rawCount&&h.pc==0x240394u);
}
bool OriginalStandardModelGeometry50LA::loadSourcePalette240210(
    const rr::game::OriginalRacerModelSceneOwner50LA* owner,
    const std::vector<std::uint8_t>& bytes,std::size_t& cursor,OriginalSourcePsgFile50LA& sourceFile,D& d)noexcept {
    d={};if(!storage_||!sourceConstructorCurrent(owner,storage_->resolver.get(),storage_->file)||
       !storage_->sourceLoad.lodsReturned||storage_->sourceLoad.bytes!=&bytes||
       cursor!=storage_->sourceLoad.cursorEnd||storage_->sourcePalette.attempted)
        return fail(d,S::phase,0x24024c);
    if(!sourceFile.current()||sourceFile.nativeFile().resolver!=storage_->resolver.get()||
       sourceFile.nativeFile().row!=storage_->file.row)return fail(d,S::cache,0x24024c);
    auto& h=storage_->sourcePalette;h.attempted=true;h.cursorStart=h.cursorEnd=cursor;
    const auto stop=[&](S status,std::uint32_t pc){h.pc=pc;h.cursorEnd=cursor;return fail(d,status,pc);};
    const auto read=[&](void* destination,std::size_t count,std::uint32_t pc){
        h.pc=pc;OriginalStandardTextureDiagnostic50LA io;
        const bool okay=sourceFile.readExactNative(destination,count,cursor,io);
        h.cursorEnd=cursor;if(!okay&&io.sourcePC)h.pc=io.sourcePC;return okay;
    };
    const auto read32=[&](std::uint32_t& value,std::uint32_t pc){
        std::array<std::uint8_t,4> raw{};if(!read(raw.data(),4,pc))return false;
        value=std::uint32_t(raw[0])|(std::uint32_t(raw[1])<<8)|(std::uint32_t(raw[2])<<16)|(std::uint32_t(raw[3])<<24);return true;
    };
    if(!read32(h.rawCount,0x24024c))return stop(S::input,h.pc);h.countRead=true;
    if(h.rawCount){
        h.descriptor.reset(new(std::nothrow) Storage::SourcePaletteDescriptor);h.descriptorAllocated=true;
        h.published20=h.descriptor.get();h.published=true; //240268 nullable publication.
        if(!h.published20)return stop(S::allocation,0x240274);
        auto& p=*h.descriptor;p.count00=h.rawCount;p.countStored=true; //240274 before native limit.
        if(h.rawCount>1)return stop(S::overflow,0x24027c);
        p.sizes.reset(new(std::nothrow) std::uint32_t[h.rawCount]);p.sizesAllocated=true;p.published04=p.sizes.get(); //240288
        p.pointers.reset(new(std::nothrow) const std::uint8_t*[h.rawCount]);p.pointersAllocated=true;p.published08=p.pointers.get(); //2402A8
        if(!read32(h.byteCount,0x2402b4))return stop(S::input,h.pc);h.byteCountRead=true;
        if(h.byteCount>max_scene_bytes48d)return stop(S::overflow,0x2402bc);
        p.bytes.reset(new(std::nothrow) std::uint8_t[h.byteCount]);p.bytesAllocated=true;p.published0C=p.bytes.get(); //2402D0
        if(h.byteCount&&!p.bytes)return stop(S::allocation,0x2402e4);
        if(!read(p.bytes.get(),h.byteCount,0x2402e4))return stop(S::input,h.pc);h.dataRead=true;
        for(h.index=0;h.index<h.rawCount;++h.index){
            if(!read32(h.rawLength[h.index],0x240310))return stop(S::input,h.pc);h.lengthRead[h.index]=true;
            ++h.sourceIterationS1; //240324 precedes even a NULL size-slot refusal.
            if(!p.sizes)return stop(S::allocation,0x240330);
            p.sizes[h.index]=h.rawLength[h.index];h.sizeStored[h.index]=true;
            if(!p.pointers)return stop(S::allocation,0x240348);
            //Do not form an invalid host pointer or invent readable palette
            //data. The preceding original size store is retained on refusal.
            if(h.offset>h.byteCount||(!p.bytes&&h.offset))return stop(S::geometry,0x240348);
            p.pointers[h.index]=p.bytes?p.bytes.get()+h.offset:nullptr;
            h.pointerOffset[h.index]=h.offset;h.pointerStored[h.index]=true;
            h.sourceStrideS2+=4u; //240350 before padded offset240360/364.
            const std::uint32_t length=h.rawLength[h.index];
            const std::uint32_t padding=(4u-(length&3u))&3u;
            h.offset+=length+padding; //240360/364 unsigned32 source wrapping.
        }
    }
    h.returned=true;h.pc=0x240394;h.cursorEnd=cursor;d.sourcePC=h.pc;
    return true; //Actual void-style source return; no invented v0 or cache claim.
}
bool OriginalStandardModelGeometry50LA::publishSourcePrototype26EE74(
    const rr::game::OriginalRacerModelSceneOwner50LA* owner,const OriginalSourcePsgFile50LA& sourceFile,D& d)noexcept {
    d={};if(!storage_||!sourceConstructorCurrent(owner,storage_->resolver.get(),storage_->file)||
       storage_->sourcePrototypeKnown||!storage_->sourcePalette.returned||
       !sourceFile.current()||sourceFile.resolver()!=storage_->resolver.get()||
       sourceFile.nativeFile().resolver!=storage_->file.resolver||sourceFile.nativeFile().row!=storage_->file.row)
        return fail(d,S::phase,0x26ee74);
    std::size_t cursor=0;
    if(!sourceFile.cursor12F520(cursor,d.cache)||cursor!=storage_->sourcePalette.cursorEnd)
        return fail(d,S::cache,0x26ee74);
    //Actual raw prototype store, NOT a source retain or native payload proof.
    //Keep native models membership empty so generic warm retain/release cannot
    //adopt this raw source resource before its own genuinely joined path.
    storage_->prototype=owner;storage_->sourcePrototypeKnown=true;
    storage_->sourcePrototypeFile=&sourceFile;d.sourcePC=0x26ee74;return true;
}
bool OriginalStandardModelGeometry50LA::sourcePrototypeIdentityCurrent(
    const rr::game::OriginalRacerModelSceneOwner50LA* owner,const OriginalSourcePsgFile50LA* sourceFile)const noexcept {
    //Pure retained identity/history: no reader.current()/cursor callback and
    //no dereference of either borrowed identity. Valid after real file close;
    //it does not authorize another read, association, close or source call.
    return storage_&&sourceFile&&storage_->sourcePrototypeFile==sourceFile&&
        storage_->sourcePrototypeKnown&&storage_->prototype==owner&&
        sourceConstructorCurrent(owner,storage_->resolver.get(),storage_->file)&&storage_->sourcePalette.returned;
}
bool OriginalStandardModelGeometry50LA::sourcePrototypeCurrent(
    const rr::game::OriginalRacerModelSceneOwner50LA* owner,const OriginalSourcePsgFile50LA* sourceFile)const noexcept {
    if(!sourcePrototypeIdentityCurrent(owner,sourceFile))return false;
    //The passed reader is a live typed occurrence; the stored raw pointer is
    //identity history only and is never independently dereferenced.
    if(!sourceFile->current()||sourceFile->resolver()!=storage_->resolver.get()||
       sourceFile->nativeFile().resolver!=storage_->file.resolver||sourceFile->nativeFile().row!=storage_->file.row)return false;
    OriginalStandardTextureDiagnostic50LA diagnostic;std::size_t cursor=0;
    return sourceFile->cursor12F520(cursor,diagnostic)&&cursor==storage_->sourcePalette.cursorEnd;
}
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
        next->surfaceMaterials.reserve(scene.lods.size());next->runtimeSurfaces.reserve(scene.lods.size());
        for(std::size_t level=0;level<scene.lods.size();++level){
            const auto& lod=scene.lods[level];
            std::vector<std::int32_t> materials;materials.reserve(lod.surfaces.size());
            std::vector<OriginalStandardSurface246300_50LA> runtime;runtime.reserve(lod.surfaces.size());
            for(std::size_t ordinal=0;ordinal<lod.surfaces.size();++ordinal){
                const auto& surface=lod.surfaces[ordinal];const auto at=surface.source_offset;
                if(surface.material_index>static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))
                    return fail(d,S::geometry,0x299BB0);
                if(at>bytes.size()||bytes.size()-at<20||read32(at)!=surface.material_index||
                   read32(at+4)!=surface.component_word||bytes[at+5]!=1u)
                    return fail(d,S::geometry,0x246344);
                materials.push_back(static_cast<std::int32_t>(surface.material_index));
                OriginalStandardSurface246300_50LA loaded;
                loaded.material00=&materials.back();loaded.attributes0C=bytes[at+4];
                loaded.uvCount0D=bytes[at+5];loaded.palette0E=bytes[at+6];
                //24637C reads source+6, not the unrelated strip count at+14.
                //Source+7 is consumed into a temporary. Runtime flags0F are
                //constructor/allocation state, not copied from that byte.
                const auto scale=read32(at+8);std::memcpy(&loaded.uvScale18[0],&scale,sizeof(float));
                loaded.vertexCount20=static_cast<std::uint16_t>(std::uint16_t(bytes[at+12])|(std::uint16_t(bytes[at+13])<<8));
                loaded.stripCount22=static_cast<std::uint16_t>(std::uint16_t(bytes[at+14])|(std::uint16_t(bytes[at+15])<<8));
                loaded.lodOrdinal=static_cast<std::uint32_t>(level);loaded.surfaceOrdinal=static_cast<std::uint32_t>(ordinal);
                loaded.sourceOffset=at;runtime.push_back(loaded);
            }
            //Moving each reserved inner vector preserves actual element and
            //material-slot addresses. No descriptor aliases another LOD.
            next->surfaceMaterials.push_back(std::move(materials));next->runtimeSurfaces.push_back(std::move(runtime));
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
        //Existing transactional adapter does not expose individual guard writes.
        //A later source call cannot invent startup-zero after its commitment.
        sourceGeometryAssertion26E1C8.fetch_or(sourceGeometryAssertionUnknown,std::memory_order_release);
        storage_=std::move(next);return true;
    }catch(const std::bad_alloc&){return fail(d,S::allocation,0x26EE38);}
    catch(...){return fail(d,S::input,0x240210);}
}
bool OriginalStandardModelGeometry50LA::cachePublicationReady1326A0()const noexcept {
    //Native payload construction remains unchanged. The new source constructor
    //is not a loader return: no source240210/prototype26EE74 receipts exist yet.
    //Keep source-mode cache publication closed until that genuine join exists.
    return storage_&&!storage_->released&&!storage_->sourceMode;
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
const OriginalStandardSurface246300_50LA* OriginalStandardModelGeometry50LA::surface246300(
    std::size_t lod,std::size_t surface)const noexcept {
    if(!constructed240110()||lod>=storage_->runtimeSurfaces.size()||surface>=storage_->runtimeSurfaces[lod].size())return nullptr;
    const auto& row=storage_->runtimeSurfaces[lod][surface];
    return row.material00==surfaceMaterial00(lod,surface)?&row:nullptr;
}
std::size_t OriginalStandardModelGeometry50LA::surfaceCount26E2B8(std::size_t lod)const noexcept {
    return constructed240110()&&lod<storage_->runtimeSurfaces.size()?storage_->runtimeSurfaces[lod].size():0u;
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
bool OriginalStandardModelGeometry50LA::holdsModelReference(
    const rr::game::OriginalRacerModelSceneOwner50LA* model) const noexcept {
    if(!storage_||!model)return false;
    for(const auto* retained:storage_->models)if(retained==model)return true;
    return false;
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
    // A warm26E9F4 retain precedes hierarchy/material allocation. If that
    // native prefix fails before26EB44 starts, the ACTUAL constructor count0
    // skips26E4C0's loop. Do not invent a released material allocation.
    const bool untouchedZeroMaterials=!materials.started()&&model&&
        model->fields().materialCount1C0==0&&model->fields().materials1C4==nullptr;
    if(!materials.released26E4C0()&&!untouchedZeroMaterials)return fail(d,S::phase,0x26E4C0);
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
        std::vector<std::vector<OriginalStandardSurface246300_50LA>>().swap(s.runtimeSurfaces);
        std::vector<std::vector<std::int32_t>>().swap(s.surfaceMaterials);
        std::vector<const rr::game::OriginalRacerModelSceneOwner50LA*>().swap(s.models);
        s.payload.reset(); //240150/26E0A0: release all owned resource backing.
        s.resolver->clearGeometry1327D0(this); //26E5BC..26E5C8, after destruction.
        s.released=true;
        // The selected temporary bank may have closed while this resource
        // was live. Only now can its native duplicate PSG bytes be reclaimed.
        // Active banks and other payload families remain retained unchanged.
        s.resolver->reclaimRetiredPsgBacking26E5C8(*this);
    }
    return true;
}
}

namespace rr::game {
OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::loadSourceMaterials26E640(
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table,
    const rr::assets::OriginalStandardMaterialPath50LA& path,
    rr::assets::OriginalStandardMaterialDiagnostic50LA& diagnostic)noexcept {
    auto& s=sourceMaterials_;auto& input=sourceDirectRows_;diagnostic={};
    if(s.attempted||!input.returned||input.nativeScratchWithdrawn||!input.bytes||!input.sourceFile||!input.sourceFile->current()||!input.resolver||
       !rowsLoaded_||!sourceConstructor_.rowsBound||referenceReleased_||released_||
       fields_.flags1B3&4u||!path.actualPrefix36B970||!table||!table->prepared()||
       fields_.materialCount1C0||fields_.materials1C4||fields_.geometry1B4||fields_.packets1F0)
        return {S::phase,0x26ee18u};
    rr::assets::OriginalStandardModelFileView50LA current;
    rr::assets::OriginalStandardTextureDiagnostic50LA fileDiagnostic;
    if(!input.resolver->modelFile(input.file,current,fileDiagnostic)||current.bytes!=input.bytes||
       current.geometry240110||current.kind!=rr::assets::OriginalStandardModelFileKind50LA::psg)
        return {S::binding,0x26ee18u};
    s.attempted=true;s.prefixAddress=path.actualPrefix36B970;
    const auto stop=[&](S status,std::uint32_t pc){s.pc=pc;return R{status,pc};};
    const auto read=[&](void* destination,std::size_t count,std::uint32_t pc){
        s.pc=pc;rr::assets::OriginalStandardTextureDiagnostic50LA io;
        if(!input.sourceFile)return false;
        const bool okay=input.sourceFile->readExactNative(destination,count,input.cursor,io);
        if(!okay&&io.sourcePC)s.pc=io.sourcePC;return okay;
    };
    std::array<std::uint8_t,4> scalar{};
    if(!read(scalar.data(),4,0x26e670))return stop(S::sourceRowTable,s.pc);
    const auto bits=std::uint32_t(scalar[0])|(std::uint32_t(scalar[1])<<8)|
        (std::uint32_t(scalar[2])<<16)|(std::uint32_t(scalar[3])<<24);
    std::memcpy(&fields_.materialCount1C0,&bits,4);s.count=bits;s.countStored=true;
    //Same bounded material capacity as the existing native decoder, but only
    //AFTER the actual raw signed count store. Zero is a genuine empty loop.
    if(fields_.materialCount1C0<0||bits>4096)return stop(S::sourceRowTable,0x26e67c);
    try{s.references=std::make_shared<rr::assets::OriginalStandardMaterialReferences50LA>();}
    catch(...){return stop(S::allocation,0x26e67c);}
    if(!s.references->beginSource26E670(bits,table,diagnostic))
        return stop(diagnostic.status==rr::assets::OriginalStandardMaterialStatus50LA::allocation?
            S::allocation:S::binding,diagnostic.sourcePC);
    s.pc=0x26e684;fields_.materials1C4=s.references->allocationBase1C4();
    s.published1C4=fields_.materials1C4;s.allocationPublished=true;
    for(std::uint32_t row=0;row<bits;++row){s.row=row;
        if(!read(s.rawName.data(),64,0x26e6a4))return stop(S::sourceRowTable,s.pc);
        ++s.namesRead;std::size_t length=0;while(length<64&&s.rawName[length])++length;
        if(length==64)return stop(S::sourceRowTable,0x26e6ac); //No fabricated terminator.
        const auto& prefix=*path.actualPrefix36B970;
        if(prefix.size()>255||prefix.find('\0')!=std::string::npos)return stop(S::binding,0x26e6c4);
        for(unsigned char c:prefix)if(c<32||c>=127)return stop(S::binding,0x26e6c4);
        std::string requested;
        try{requested=prefix;
            if(!requested.empty()&&requested.back()!='\\')requested+='\\';
            if(requested.size()+length>255)return stop(S::binding,prefix.empty()?0x26e720u:0x26e70cu);
            requested.append(reinterpret_cast<const char*>(s.rawName.data()),length);}
        catch(...){return stop(S::allocation,prefix.empty()?0x26e720u:0x26e70cu);}
        std::int32_t found=-1;
        if(!table->find298C80(requested,found,diagnostic)){
            //Actual a2=1 enters its own assertion before any possible-1 return.
            if(diagnostic.status==rr::assets::OriginalStandardMaterialStatus50LA::missingName)
                return stop(S::binding,0x298cf8);
            return stop(S::binding,diagnostic.sourcePC);
        }
        s.lookupReturn=found;++s.lookupsReturned;
        //Includes original lazy assertion history BEFORE source slot store;
        //NULL1C4 is bounded only after name/lookup/assertion were reached.
        if(!s.references->storeSource26E7C8(found,diagnostic))return stop(S::binding,diagnostic.sourcePC);
        ++s.retainsReturned;
    }
    if(!s.references->finishSource26E810())return stop(S::phase,0x26e810);
    s.returned=true;s.pc=0x26e810;return {S::complete,s.pc}; //Next is240570, not full26E820 return.
}
bool OriginalRacerModelSceneOwner50LA::preflightSourceMaterialsNative()const noexcept {
    const auto& s=sourceMaterials_;if(!s.attempted)return true;
    std::uint32_t actual;std::memcpy(&actual,&fields_.materialCount1C0,4);
    if(s.countStored?actual!=s.count:actual!=0)return false;
    if(s.nativeWithdrawn)return fields_.materials1C4==nullptr&&(!s.references||!s.references->started()||s.references->released26E4C0());
    if(s.retainsReturned>s.lookupsReturned||
       s.lookupsReturned>s.namesRead||(s.returned&&(!s.references||!s.references->complete())))return false;
    if(s.allocationPublished){if(!s.references||!s.references->started()||
        s.published1C4!=s.references->allocationBase1C4()||fields_.materials1C4!=s.published1C4)return false;}
    else if(fields_.materials1C4)return false;
    return !s.references||(s.references.use_count()==1&&s.references->canRelease26E4C0());
}
bool OriginalRacerModelSceneOwner50LA::releaseSourceMaterialsNative()noexcept {
    auto& s=sourceMaterials_;if(!s.attempted||s.nativeWithdrawn)return true;
    if(sourcePacketBorrower_||sourceObserverBorrower_||sourceSmokerBorrower_||sourceWarmLifetimeBorrowedNative())return false;
    if((fields_.geometry1B4&&(!sourceGeometry_.attempted||!preflightSourceGeometryNative()))||
       fields_.packets1F0||(sourceRowTable_.attempted?!preflightSourceRowTableNative():bool(rowTable_))||!preflightSourceMaterialsNative())return false;
    if(s.references){rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
        if(!s.references->release26E4C0(diagnostic))return false;
        if(s.references->started()&&!s.references->freeAllocation26E5EC())return false;}
    //Native cancellation, not a claim that source240450/26E460 was invoked.
    //Preserve count and actual publication value as history; withdraw borrow.
    fields_.materials1C4=nullptr;s.nativeWithdrawn=true;return true;
}
}

namespace rr::game {
OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::loadSourceQuantization240570()noexcept {
    auto& q=sourceQuantization_;auto& input=sourceDirectRows_;const auto& materials=sourceMaterials_;
    if(q.attempted||!materials.returned||materials.nativeWithdrawn||!materials.references||
       !materials.references->complete()||!materials.allocationPublished||
       fields_.materials1C4!=materials.published1C4||fields_.materials1C4!=materials.references->allocationBase1C4()||
       !input.returned||input.nativeScratchWithdrawn||!input.bytes||!input.sourceFile||!input.sourceFile->current()||!input.resolver||
       !rowsLoaded_||quantizationLoaded_||referenceReleased_||released_||fields_.geometry1B4||fields_.packets1F0)
        return {S::phase,0x240570u};
    rr::assets::OriginalStandardModelFileView50LA current;
    rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
    if(!input.resolver->modelFile(input.file,current,diagnostic)||current.bytes!=input.bytes||
       current.geometry240110||current.kind!=rr::assets::OriginalStandardModelFileKind50LA::psg)
        return {S::binding,0x240570u};
    q.attempted=true;
    constexpr std::uint32_t reads[8]={0x240598,0x2405b4,0x2405d0,0x2405ec,
        0x240608,0x240624,0x240640,0x24065c};
    for(unsigned lane=0;lane<8;++lane){q.pc=reads[lane];
        std::array<std::uint8_t,4> raw{};rr::assets::OriginalStandardTextureDiagnostic50LA io;
        if(!input.sourceFile||!input.sourceFile->readExactNative(raw.data(),4,input.cursor,io))
            return {S::sourceRowTable,io.sourcePC?io.sourcePC:q.pc};
        const auto* p=raw.data();const auto bits=std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|
            (std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
        q.words[lane]=bits;
        if(lane<4)std::memcpy(&fields_.scale1E0[lane],&bits,4);
        else if(lane<7)std::memcpy(&fields_.offset1D0[lane-4],&bits,4);
        else fields_.offsetEnabled1DC=bits;
        q.readMask=static_cast<std::uint8_t>(q.readMask|(1u<<lane));
    }
    //Native exact C.EQ.S adapter on finite normal/zero operands only. Other
    //six lanes are merely source stores: never validate/normalize them here.
    const auto comparable=[](std::uint32_t bits){const auto magnitude=bits&0x7fffffffu;
        const auto exponent=bits&0x7f800000u;
        return magnitude==0u||(exponent!=0u&&exponent!=0x7f800000u);};
    q.pc=0x240674;if(!comparable(q.words[0]))return {S::phase,q.pc};
    q.xEqualsOne=q.words[0]==0x3f800000u;
    bool scaled=true;
    if(*q.xEqualsOne){ //Original BC1F skips Y comparison when X differs.
        q.pc=0x240688;if(!comparable(q.words[1]))return {S::phase,q.pc};
        q.yEqualsOne=q.words[1]==0x3f800000u;scaled=!*q.yEqualsOne;
    }
    q.pc=0x2406c8;fields_.flags1B2=static_cast<std::uint8_t>((fields_.flags1B2&~0x40u)|(scaled?0x40u:0u));
    q.flagStored=true;quantizationLoaded_=true;q.returned=true;q.pc=0x2406dc;
    return {S::complete,q.pc}; //Next source26EE38 nullable geometry allocation is separate.
}
}

namespace rr::game {
OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::allocateSourceGeometry26EE38()noexcept {
    auto& s=sourceGeometry_;auto& input=sourceDirectRows_;
    if(s.attempted||!sourceQuantization_.returned||!quantizationLoaded_||!sourceMaterials_.returned||
       sourceMaterials_.nativeWithdrawn||!sourceMaterials_.references||!sourceMaterials_.references->complete()||
       !input.returned||input.nativeScratchWithdrawn||!input.bytes||!input.sourceFile||!input.sourceFile->current()||!input.resolver||
       referenceReleased_||released_||fields_.geometry1B4||fields_.packets1F0||rowTable_)
        return {S::phase,0x26ee38u};
    rr::assets::OriginalStandardModelFileView50LA current;
    rr::assets::OriginalStandardTextureDiagnostic50LA fileDiagnostic;
    if(!input.resolver->modelFile(input.file,current,fileDiagnostic)||current.bytes!=input.bytes||
       current.geometry240110||current.kind!=rr::assets::OriginalStandardModelFileKind50LA::psg)
        return {S::binding,0x26ee38u};
    s.attempted=true;s.pc=0x26ee38;
    s.pending.reset(new(std::nothrow) rr::assets::OriginalStandardModelGeometry50LA);
    s.allocationReturned=true;s.allocationNonNull=bool(s.pending);
    if(s.pending){
        //Native shared-control custody is not guest constructor execution.
        try{s.owner=std::shared_ptr<rr::assets::OriginalStandardModelGeometry50LA>(std::move(s.pending));}
        catch(...){s.pc=0x26ee48;return {S::allocation,s.pc};}
        rr::assets::OriginalStandardResourceDiagnostic50LA diagnostic;
        s.pc=0x26ee48;
        if(!s.owner->constructSource240110(input.resolver,input.file,this,diagnostic))
            return {diagnostic.status==rr::assets::OriginalStandardResourceStatus50LA::allocation?S::allocation:S::binding,s.pc};
        s.constructorReturned=true;
    }
    s.pc=0x26ee50;fields_.geometry1B4=s.owner.get();s.published1B4=fields_.geometry1B4;s.published=true;
    s.pc=fields_.geometry1B4?0x26ee64u:0x26ee5cu;
    return {S::phase,s.pc}; //Genuine240210 call/NULL dereference boundary, NOT loader0.
}
OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::loadSourceGeometry26E170()noexcept {
    auto& s=sourceGeometry_;auto& input=sourceDirectRows_;
    if(!preflightSourceGeometryNative()||s.nativeWithdrawn||!s.constructorReturned||!s.published||
       s.loadAttempted||!s.owner||s.pc!=0x26ee64u||!sourceQuantization_.returned||
       !sourceMaterials_.returned||sourceMaterials_.nativeWithdrawn||!sourceMaterials_.references||
       !sourceMaterials_.references->complete()||!input.returned||input.nativeScratchWithdrawn||
       !input.bytes||!input.sourceFile||!input.sourceFile->current()||input.cursor>input.bytes->size()||!input.resolver||
       (fields_.flags1B3&4u)||referenceReleased_||released_||fields_.packets1F0||rowTable_)
        return {S::phase,0x26ee64u};
    rr::assets::OriginalStandardModelFileView50LA current;
    rr::assets::OriginalStandardTextureDiagnostic50LA fileDiagnostic;
    if(!input.resolver->modelFile(input.file,current,fileDiagnostic)||current.bytes!=input.bytes||
       current.geometry240110||current.kind!=rr::assets::OriginalStandardModelFileKind50LA::psg)
        return {S::binding,0x26ee64u};
    //Actual ordinary26ED60 S1=NULL, preserved through26EE68. No repack/global
    //descriptor owner is synthesized from the native decoded scene.
    s.loadAttempted=true;rr::assets::OriginalStandardResourceDiagnostic50LA diagnostic;
    if(!s.owner->loadSourceOrdinary26E170(this,*input.bytes,input.cursor,*input.sourceFile,diagnostic)){
        s.pc=diagnostic.sourcePC;return {diagnostic.status==rr::assets::OriginalStandardResourceStatus50LA::allocation?
            S::allocation:S::phase,s.pc};
    }
    s.lodsReturned=true;s.pc=0x24024c;
    return {S::phase,s.pc}; //26E170 returned; actual palette read still required.
}
OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::finishSourceGeometry240210()noexcept {
    auto& s=sourceGeometry_;auto& input=sourceDirectRows_;
    if(!preflightSourceGeometryNative()||s.nativeWithdrawn||!s.lodsReturned||s.paletteAttempted||
       !s.owner||s.pc!=0x24024cu||!sourceMaterials_.returned||sourceMaterials_.nativeWithdrawn||
       !sourceMaterials_.references||!sourceMaterials_.references->complete()||!input.returned||
       input.nativeScratchWithdrawn||!input.bytes||!input.sourceFile||!input.sourceFile->current()||!input.resolver||referenceReleased_||released_||
       fields_.packets1F0||rowTable_)return {S::phase,0x24024cu};
    rr::assets::OriginalStandardModelFileView50LA current;
    rr::assets::OriginalStandardTextureDiagnostic50LA fileDiagnostic;
    if(!input.resolver->modelFile(input.file,current,fileDiagnostic)||current.bytes!=input.bytes||current.geometry240110)
        return {S::binding,0x24024cu};
    s.paletteAttempted=true;rr::assets::OriginalStandardResourceDiagnostic50LA diagnostic;
    if(!s.owner->loadSourcePalette240210(this,*input.bytes,input.cursor,*input.sourceFile,diagnostic)){
        s.pc=diagnostic.sourcePC;return {diagnostic.status==rr::assets::OriginalStandardResourceStatus50LA::allocation?
            S::allocation:S::phase,s.pc};
    }
    s.geometryCallReturned=true;s.pc=0x240394;
    return {S::complete,s.pc}; //Only geometry virtual call returned;26EE74 remains next.
}
OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::publishSourceGeometryPrototype26EE74()noexcept {
    auto& s=sourceGeometry_;auto& input=sourceDirectRows_;
    if(!preflightSourceGeometryNative()||s.nativeWithdrawn||!s.geometryCallReturned||s.prototypeAttempted||
       !s.owner||s.pc!=0x240394u||!input.sourceFile||!input.sourceFile->current()||
       !input.resolver||!sourceMaterials_.returned||sourceMaterials_.nativeWithdrawn||
       referenceReleased_||released_||fields_.packets1F0||rowTable_)return {S::phase,0x26ee74u};
    s.prototypeAttempted=true;s.pc=0x26ee74;
    rr::assets::OriginalStandardResourceDiagnostic50LA diagnostic;
    if(!s.owner->publishSourcePrototype26EE74(this,*input.sourceFile,diagnostic))return {S::binding,s.pc};
    s.prototypeStored=true;s.pc=0x26ee78;
    return {S::complete,s.pc}; //Next source member-cache association is NOT performed here.
}
bool OriginalRacerModelSceneOwner50LA::preflightSourceGeometryNative()const noexcept {
    const auto& s=sourceGeometry_;if(!s.attempted)return true;
    if(s.nativeWithdrawn)return !s.pending&&!s.owner&&fields_.geometry1B4==nullptr;
    if(!s.allocationReturned||(s.pending&&s.owner)||
       (s.allocationNonNull?(!s.pending&&!s.owner):(s.pending||s.owner))||
       (s.constructorReturned&&!s.owner))return false;
    if(s.published){if(s.pending||s.published1B4!=s.owner.get()||fields_.geometry1B4!=s.published1B4||
        bool(s.owner)!=s.constructorReturned)return false;
        if(!s.loadAttempted){if(s.pc!=(s.owner?0x26ee64u:0x26ee5cu)||s.lodsReturned||s.paletteAttempted||s.geometryCallReturned||s.prototypeAttempted||s.prototypeStored)return false;}
        else {if(!s.owner||!s.owner->storage_)return false;const auto& h=s.owner->storage_->sourceLoad;
            if(!h.attempted){if(s.lodsReturned||s.pc!=0x240230u)return false;}
            else {
                if(h.bytes!=sourceDirectRows_.bytes||s.lodsReturned!=h.lodsReturned)return false;
                if(!s.paletteAttempted){if(s.geometryCallReturned||h.cursorEnd!=sourceDirectRows_.cursor||
                    s.pc!=(h.lodsReturned?0x24024cu:h.pc))return false;}
                else {const auto& p=s.owner->storage_->sourcePalette;
                    if(!h.lodsReturned)return false;
                    if(!p.attempted){if(s.geometryCallReturned||s.pc!=0x24024cu)return false;}
                    else {
                        if(p.cursorStart!=h.cursorEnd||p.cursorEnd!=sourceDirectRows_.cursor||s.geometryCallReturned!=p.returned)return false;
                        if(!s.prototypeAttempted){if(s.prototypeStored||s.pc!=p.pc)return false;}
                        else if(!p.returned||s.pc!=(s.prototypeStored?0x26ee78u:0x26ee74u))return false;
                        if(s.prototypeStored!=s.owner->storage_->sourcePrototypeKnown)return false;
                        if(s.prototypeStored&&(s.owner->storage_->prototype!=this||
                            s.owner->storage_->sourcePrototypeFile!=sourceDirectRows_.sourceFile.get()))return false;
                    }
                }
            }
        }}
    else if(fields_.geometry1B4||s.constructorReturned||s.loadAttempted||s.lodsReturned||s.paletteAttempted||s.geometryCallReturned||s.prototypeAttempted||s.prototypeStored||s.pc!=0x26ee48u)return false;
    if(s.owner){if(s.owner.use_count()!=1)return false;
        if(s.constructorReturned){if(!s.owner->sourceConstructorCurrent(this,sourceDirectRows_.resolver.get(),sourceDirectRows_.file))return false;}
        else if(s.owner->references00())return false;
    }
    return !s.pending||s.pending->references00()==nullptr;
}
bool OriginalRacerModelSceneOwner50LA::releaseSourceGeometryNative()noexcept {
    auto& s=sourceGeometry_;if(!s.attempted||s.nativeWithdrawn)return true;
    if(sourcePacketBorrower_||sourceObserverBorrower_||sourceSmokerBorrower_||sourceWarmLifetimeBorrowedNative())return false;
    if(!preflightSourceGeometryNative()||fields_.packets1F0||
       (sourceRowTable_.attempted?!preflightSourceRowTableNative():bool(rowTable_))||
       (sourceMaterials_.attempted&&!sourceMaterials_.nativeWithdrawn))return false;
    //Actual source member1C/native weak identity must stop borrowing this
    //prototype before its resource/model can be destroyed by native abort.
    if(sourceDirectRows_.sourceFile&&sourceDirectRows_.sourceFile->sourceAssociationStored()&&
       !sourceDirectRows_.sourceFile->associationWithdrawnNative())return false;
    if(s.constructorReturned&&!s.owner->cancelSourceConstructorNative(this,sourceDirectRows_.resolver.get(),sourceDirectRows_.file))return false;
    //Native withdrawal of live1B4 borrow; historical nullable store remains.
    fields_.geometry1B4=nullptr;s.owner.reset();s.pending.reset();s.nativeWithdrawn=true;return true;
}
}

namespace rr::game {
OriginalRacerModelSceneResult50LA OriginalRacerModelSceneOwner50LA::allocateClosedSourceRowTable26EE9C()noexcept {
    auto& h=sourceRowTable_;auto& input=sourceDirectRows_;
    if(h.attempted||!sourceConstructor_.modelReturned||!input.returned||input.nativeScratchWithdrawn||
       !input.sourceFile||!input.sourceFile->sourceCloseReturned()||!input.sourceFile->preflightNative()||
       !sourceGeometry_.prototypeStored||sourceGeometry_.nativeWithdrawn||!sourceGeometry_.owner||
       !preflightSourceGeometryNative()||!sourceMaterials_.attempted||!sourceMaterials_.returned||
       sourceMaterials_.nativeWithdrawn||!preflightSourceMaterialsNative()||
       !input.sourceFile->associatedGeometryCurrent(sourceGeometry_.owner.get(),this)||
       !rowsLoaded_||rootMode_!=RootMode::direct||warmChildlessRows_||
       referenceReleased_||released_||rowTable_||fields_.sourceRows1B8||rowTableFreed_||
       !input.returnCount||*input.returnCount!=binding_.sourceRowCount||
       !binding_.sourceRowCount||binding_.sourceRowCount>rr::assets::maxModelNodes49O||
       model_.nodeCount1A0!=binding_.sourceRowCount||sourceRowObject(0)!=&model_)
        return {S::phase,0x26ee98u};
    //Native preadmission proves the retained original stack row identities;
    //it neither allocates source rows nor fills a prepared native scene.
    for(std::uint32_t i=0;i<binding_.sourceRowCount;++i){const auto* row=sourceRowObject(i);
        if(!row||input.publishedRows[i]!=row||!row->constructed||row->released)
            return {S::sourceRowTable,0x26ee98u};
    }
    h.attempted=true;h.count=*input.returnCount;h.closedFile=input.sourceFile.get();
    h.ignoredCloseResult=input.sourceFile->sourceCloseResult(); //Original caller never branches on this value.
    h.pc=0x26ee9c;rowTable_.reset(new(std::nothrow) O*[h.count]);h.allocationReturned=true;
    h.pc=0x26eea4;fields_.sourceRows1B8=rowTable_.get();h.published1B8=rowTable_.get();h.published=true;
    h.pc=0x26eeb4;if(!rowTable_)return {S::allocation,h.pc}; //Actual NULL memcpy destination, NOT source return0.
    //Logical original pointer slots, not serialization/truncation of native
    //pointer bits to source4-byte ABI. No fallible work remains in this copy.
    for(std::uint32_t i=0;i<h.count;++i)rowTable_[i]=input.publishedRows[i];h.copied=true;
    h.pc=0x26eec8;h.returnedEntityV0=&model_.entity8;h.returned=true;
    h.pc=0x26eee8;return {S::complete,h.pc}; //26E820 only, NOT later240740 packet/flags/return.
}
bool OriginalRacerModelSceneOwner50LA::preflightSourceRowTableNative()const noexcept {
    const auto& h=sourceRowTable_;if(!h.attempted)return true;
    if(h.nativeWithdrawn)return !rowTable_&&rowTableFreed_&&!fields_.sourceRows1B8;
    if(!h.count||h.count>rr::assets::maxModelNodes49O||h.count!=binding_.sourceRowCount||
       h.count!=model_.nodeCount1A0||!sourceDirectRows_.returned||
       sourceDirectRows_.returnCount!=std::optional<std::uint32_t>{h.count}||sourceRowObject(0)!=&model_||
       !h.allocationReturned||!h.published||h.published1B8!=rowTable_.get()||
       fields_.sourceRows1B8!=h.published1B8||rowTableFreed_||
       !sourceDirectRows_.sourceFile||h.closedFile!=sourceDirectRows_.sourceFile.get()||
       !sourceDirectRows_.sourceFile->sourceCloseReturned()||
       h.ignoredCloseResult!=sourceDirectRows_.sourceFile->sourceCloseResult())return false;
    if(!rowTable_)return !h.copied&&!h.returned&&!h.returnedEntityV0&&h.pc==0x26eeb4u;
    if(!h.copied||!h.returned||h.returnedEntityV0!=&model_.entity8||h.pc!=0x26eee8u)return false;
    for(std::uint32_t i=0;i<h.count;++i){const auto* row=sourceRowObject(i);
        if(!row||sourceDirectRows_.publishedRows[i]!=row||rowTable_[i]!=row||!row->constructed||row->released)return false;
    }
    return true;
}
bool OriginalRacerModelSceneOwner50LA::withdrawSourceRowTableNative()noexcept {
    if(sourcePacketBorrower_||sourceWarmLifetimeBorrowedNative())return false;
    auto& h=sourceRowTable_;if(!h.attempted||h.nativeWithdrawn)return true;
    if(!preflightSourceRowTableNative()||!sourceGeometry_.nativeWithdrawn||
       (sourceMaterials_.attempted&&!sourceMaterials_.nativeWithdrawn)||fields_.packets1F0)return false;
    //Native cancellation after resource/material borrowers and BEFORE actual
    //counted scene retirement. Original source history remains in h.
    fields_.sourceRows1B8=nullptr;rowTable_.reset();rowTableFreed_=true;h.nativeWithdrawn=true;return true;
}
OriginalRacerModelSceneOwner50LA::R OriginalRacerModelSceneOwner50LA::acquireSourceWarmGeometry26E9F0(
    std::shared_ptr<rr::assets::OriginalSourcePsgFile50LA> file)noexcept {
    auto& h=sourceWarmGeometry_;
    if(h.attempted||!file||!file->returnedWarm()||!sourceConstructor_.modelReturned||
       !sourceConstructor_.noOp141C20Returned||!directRootPrepared_||sourceConstructor_.rowsBound||
       rootMode_!=RootMode::direct||sourceDirectRows_.attempted||sourceGeometry_.attempted||
       sourceMaterials_.attempted||sourceRowTable_.attempted||referenceReleased_||released_||
       !model_.constructed||model_.released||fields_.geometry1B4||fields_.packets1F0||rowTable_)
        return {S::phase,0x26e9f0};
    const auto* creator=file->warmPrototypeOwner();
    if(!creator||creator==this||!creator->preflightSourceRenderableReturn26E820Native()||
       creator->binding_.actualObjectCount423F60!=binding_.actualObjectCount423F60||
       creator->binding_.sessionGeneration!=binding_.sessionGeneration||
       !creator->sourceGeometry_.owner||file->warmGeometry()!=creator->sourceGeometry_.owner.get()||
       file->creatorSourceFile()!=creator->sourceDirectRows_.sourceFile.get()||
       !file->warmGeometryCurrent(creator->sourceGeometry_.owner.get(),creator))return {S::binding,0x26e9f0};
    h.attempted=true;h.pc=0x26e9f0;h.file=std::move(file);h.creator=creator;
    h.geometry=creator->sourceGeometry_.owner.get();
    rr::assets::OriginalStandardResourceDiagnostic50LA diagnostic;
    if(!h.geometry->registerSourceWarmBorrow(this,h.file,diagnostic))return {S::binding,diagnostic.sourcePC};
    h.registered=true;
    fields_.geometry1B4=h.geometry;h.published=true; //26E9F0 before virtual0C.
    h.pc=0x26ea00;
    if(!h.geometry->retainSourceWarm104960(this,h.file.get(),diagnostic)){
        h.pc=diagnostic.sourcePC;return {S::binding,h.pc};}
    h.retained=true;
    //26EA10 is the branch delay-slot raw prototype1C load. No Entity retain.
    h.pc=0x26ea10;h.prototype=h.geometry->storage_->prototype;h.prototypeRead=true;
    h.pc=0x26ea88;return {S::sourceRowTable,h.pc}; //Actual direct hierarchy branch remains unjoined.
}
bool OriginalRacerModelSceneOwner50LA::preflightSourceWarmGeometryNative()const noexcept {
    if(!preflightSourceWarmRowsNative()||!preflightSourceWarmMaterialsNative()||!preflightSourceWarmReturnNative()||
       !preflightSourceWarmCloneNative()||!preflightSourceWarmEnumerationNative())return false;
    const auto& h=sourceWarmGeometry_;if(!h.attempted)return true;
    if(h.nativeWithdrawn)return !h.file&&!fields_.geometry1B4;
    if(!h.file||referenceReleased_||released_||!model_.constructed||model_.released)return false;
    //No raw creator/geometry access after failed native ledger allocation or
    //after exact ledger withdrawal. The caller may already have closed the
    //warm lookup and retired its creator; those pointers are history only.
    if(!h.registered)return !h.published&&!h.retained&&!h.prototypeRead&&!h.nativeLedgerWithdrawn&&
        !fields_.geometry1B4&&h.pc==0x26e9f0&&h.file->preflightNative();
    if(h.nativeLedgerWithdrawn)return !fields_.geometry1B4&&h.file->preflightNative();
    if(!h.creator||!h.geometry||h.creator==this||!h.file->warmGeometryCurrent(h.geometry,h.creator))return false;
    if(!h.published||fields_.geometry1B4!=h.geometry||
       !h.geometry->sourceWarmBorrowCurrent(this,h.file.get(),h.retained))return false;
    if(h.prototypeRead)return h.retained&&h.prototype==h.creator&&h.pc==0x26ea88;
    return !h.retained&&(h.pc==0x26ea00||h.pc==0x104960||h.pc==0x104964);
}
bool OriginalRacerModelSceneOwner50LA::withdrawSourceWarmGeometryNative()noexcept {
    auto& h=sourceWarmGeometry_;if(!h.attempted||h.nativeWithdrawn)return true;
    if(sourcePacketBorrower_||sourceObserverBorrower_||sourceSmokerBorrower_||
       (sourceWarmMaterials_.entered&&!sourceWarmMaterials_.nativeWithdrawn)||!preflightSourceWarmGeometryNative()||
       fields_.packets1F0||fields_.materials1C4||
       (sourceWarmReturn_.rowAllocationAttempted?!preflightSourceWarmReturnNative():
        bool(fields_.sourceRows1B8||rowTable_)))return false;
    if(h.registered&&!h.nativeLedgerWithdrawn){
        if(!h.geometry->withdrawSourceWarmBorrowNative(this,h.file.get()))return false;
        fields_.geometry1B4=nullptr;h.nativeLedgerWithdrawn=true;
    }
    //Retain the actual lookup owner through its explicit native close. If
    //close refuses, the preceding ledger withdrawal is recorded and retryable.
    //A prior external close is accepted through the file's historical path.
    rr::assets::OriginalStandardTextureDiagnostic50LA diagnostic;
    if(!h.file->closeNative(diagnostic))return false;
    //No131040,26E598/104970 or source loader failure return is claimed.
    fields_.geometry1B4=nullptr;h.file.reset();h.nativeWithdrawn=true;return true;
}
OriginalRacerModelSceneOwner50LA::R OriginalRacerModelSceneOwner50LA::continueSourceWarmRows26EA88(
    const SceneGraph49P& graph,const SceneGraph49P& prototypeGraph)noexcept {
    auto& h=sourceWarmRows_;const auto& w=sourceWarmGeometry_;
    if(h.entered||!preflightSourceWarmGeometryNative()||w.nativeWithdrawn||w.nativeLedgerWithdrawn||
       !w.prototypeRead||!w.retained||w.prototype!=w.creator||!w.creator||
       !w.creator->preflightSourceRenderableReturn26E820Native()||sourceConstructor_.rowsBound||
       graph.nodes!=sourceConstructor_.canonicalNodes||graph.count!=1||
       prototypeGraph.nodes!=w.creator->sourceConstructor_.canonicalNodes||
       prototypeGraph.count!=w.creator->binding_.sourceRowCount||
       !validateSceneGraph49P(graph)||!validateSceneGraph49P(prototypeGraph)||
       !freshNode(graph,model_.sceneIndex)||rowsLoaded_||directRowsStarted_)
        return {S::phase,0x26ea88};
    h.entered=true;h.pc=0x26ea88;const auto& p=*w.creator;
    if(!(p.sourceConstructor_.knownFlags191&2u))return {S::binding,h.pc};
    h.bit191=std::uint8_t(p.model_.flags191&2u);h.bitRead=true;
    auto& target=at(graph,model_.sceneIndex);const auto& prototype=at(prototypeGraph,p.model_.sceneIndex);
    const auto stop=[&](S status,std::uint32_t pc){h.pc=pc;return R{status,pc};};
    h.childRead=true;
    if(h.bit191||prototype.firstChild184!=none){
        //The actual branch picks prototype.child184 for bit1, otherwise
        //prototype MODEL-base.28E5D0 clears destination parent before clone.
        h.selectedCloneSource=h.bit191?p.object(prototype.firstChild184):&p.model_;
        target.parent180=none;h.cloneParentCleared=true;
        return stop(S::hierarchy,0x28e5f0); //Ordered clone allocations/stores remain separate.
    }
    h.pc=0x26ead4;std::memcpy(model_.bounds140.data(),p.model_.bounds140.data(),16);
    std::memcpy(h.bounds140.data(),model_.bounds140.data(),16);h.copyMask|=1u;
    h.pc=0x26eae0;std::memcpy(model_.bounds150.data(),p.model_.bounds150.data(),16);
    std::memcpy(h.bounds150.data(),model_.bounds150.data(),16);h.copyMask|=2u;
    h.pc=0x26eae4;if(!p.model_.radius194)return {S::binding,h.pc};
    h.pc=0x26eae8;model_.radius194=p.model_.radius194;
    std::memcpy(&h.radius194,&*model_.radius194,4);h.copyMask|=4u;
    h.pc=0x26eaf8;target.sceneFlags190=std::uint8_t(target.sceneFlags190|0x20u);h.flagStoreMask|=1u;
    h.pc=0x26eb0c;target.sceneFlags190=std::uint8_t(target.sceneFlags190|4u);h.flagStoreMask|=2u;
    h.flagsAfter=target.sceneFlags190;
    h.pc=0x26eb34;h.count=1;h.countSeeded=true;
    h.pc=0x26eb3c;h.rowZeroStored=true;
    //Actual28C400 reads target.child184 twice and returns without extra rows.
    if(target.firstChild184!=none)return stop(S::hierarchy,0x28c41c);
    h.enumerationReturned=true;
    //Native stable index map, NOT28CAC0/read/constructor or source1A0 write.
    indices_[0]=model_.sceneIndex;binding_.sourceRowCount=1;binding_.sourceRowIndices=indices_.data();
    sourceConstructor_.rowsBound=true;sourceParents_[0]=-1;
    directRowsStarted_=true;warmChildlessRows_=true;rowsLoaded_=true;
    h.pc=0x26eb40;return {S::complete,h.pc};
}
bool OriginalRacerModelSceneOwner50LA::preflightSourceWarmRowsNative()const noexcept {
    const auto& h=sourceWarmRows_;if(!h.entered)return true;
    if(!sourceWarmGeometry_.attempted||!sourceWarmGeometry_.prototypeRead)return false;
    if((h.copyMask&1u)&&std::memcmp(h.bounds140.data(),model_.bounds140.data(),16))return false;
    if((h.copyMask&2u)&&std::memcmp(h.bounds150.data(),model_.bounds150.data(),16))return false;
    if(h.copyMask&4u){if(!model_.radius194)return false;std::uint32_t word;
        std::memcpy(&word,&*model_.radius194,4);if(word!=h.radius194)return false;}
    if(h.cloneParentCleared&&sourceWarmEnumeration_.entered)
        return !h.copyMask&&!h.flagStoreMask&&preflightSourceWarmEnumerationNative();
    if(h.enumerationReturned)return h.bitRead&&!h.bit191&&h.childRead&&!h.cloneParentCleared&&
        h.copyMask==7u&&h.flagStoreMask==3u&&h.countSeeded&&h.rowZeroStored&&h.count==1&&
        binding_.sourceRowCount==1&&indices_[0]==model_.sceneIndex&&warmChildlessRows_&&rowsLoaded_&&h.pc==0x26eb40;
    return !h.countSeeded&&!h.rowZeroStored&&(!h.cloneParentCleared||h.pc==0x28e5f0);
}
OriginalRacerModelSceneOwner50LA::R OriginalRacerModelSceneOwner50LA::copySourceWarmMaterials26EB40()noexcept {
    auto& h=sourceWarmMaterials_;const auto& w=sourceWarmGeometry_;
    if(h.entered||!preflightSourceWarmGeometryNative()||w.nativeWithdrawn||w.nativeLedgerWithdrawn||
       !preflightSourceWarmRowsNative()||!sourceWarmRows_.enumerationReturned||
       (sourceWarmRows_.cloneParentCleared&&!sourceWarmEnumeration_.mapBound)||
       !w.creator||!w.creator->preflightSourceRenderableReturn26E820Native()||
       sourceMaterials_.attempted||fields_.materials1C4||fields_.packets1F0||rowTable_)
        return {S::phase,0x26eb40};
    const auto& p=*w.creator;const auto& refs=p.sourceMaterials_.references;
    if(!refs||!refs->complete()||!refs->count1C0()||*refs->count1C0()!=p.fields_.materialCount1C0||
       refs->allocationBase1C4()!=p.fields_.materials1C4)return {S::binding,0x26eb40};
    h.entered=true;h.pc=0x26eb44;h.count=p.fields_.materialCount1C0;
    fields_.materialCount1C0=h.count;h.countStored=true;
    //Native control backing may refuse after actual count store. No source
    //allocation/publication is fabricated when that occurs.
    try{h.references=std::make_shared<rr::assets::OriginalStandardMaterialReferences50LA>();}
    catch(...){return {S::allocation,h.pc};}
    rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
    if(!h.references->beginSourceWarm26EB44(*refs,diagnostic))return {S::binding,diagnostic.sourcePC};
    h.pc=0x26eb4c;
    if(!h.references->allocateSourceWarm26EB4C(diagnostic))return {S::binding,diagnostic.sourcePC};
    h.pc=0x26eb54;fields_.materials1C4=h.references->allocationBase1C4();
    h.published1C4=fields_.materials1C4;h.allocationPublished=true;
    h.pc=0x26eb64;
    if(!h.references->copySourceWarm26EB64(*refs,diagnostic))return {S::binding,diagnostic.sourcePC};
    h.copyReturned=true;h.pc=0x26eba0;
    //Actual signed-positive loop. Supported count domain was already bounded
    //at allocation, after the genuine count store.
    while(h.retained<h.count){h.pc=0x26eb90;
        if(!h.references->retainSourceWarm26EB90(diagnostic))return {S::binding,diagnostic.sourcePC};
        h.pc=0x26eb98;++h.retained;h.pc=0x26eba0;}
    if(!h.references->finishSourceWarm26EBA8())return {S::phase,0x26eba8};
    h.returned=true;h.pc=0x26ebb0;return {S::complete,h.pc}; //Next actual2406E0 parameter copy.
}
bool OriginalRacerModelSceneOwner50LA::preflightSourceWarmMaterialsNative()const noexcept {
    const auto& h=sourceWarmMaterials_;if(!h.entered)return true;
    if(h.nativeWithdrawn)return !fields_.materials1C4&&
        (!h.references||!h.references->started()||h.references->released26E4C0());
    if(!h.countStored||fields_.materialCount1C0!=h.count||h.retained>h.count||
       !sourceWarmGeometry_.registered||sourceWarmGeometry_.nativeLedgerWithdrawn||sourceWarmGeometry_.nativeWithdrawn)
        return false;
    if(!h.references)return !h.allocationPublished&&!h.copyReturned&&!h.retained&&!h.returned&&!fields_.materials1C4;
    if(h.references.use_count()!=1)return false;
    if(!h.references->started())return !h.allocationPublished&&!h.copyReturned&&!h.retained&&!h.returned&&!fields_.materials1C4;
    if(!h.references->preflightSourceWarmNative()||!h.references->count1C0()||*h.references->count1C0()!=h.count)return false;
    const auto* receipt=h.references->sourceWarmReceipt();if(!receipt||receipt->retained!=h.retained)return false;
    if(h.allocationPublished){if(!receipt->allocationReturned||h.published1C4!=h.references->allocationBase1C4()||
        fields_.materials1C4!=h.published1C4)return false;}
    else if(receipt->allocationReturned||fields_.materials1C4)return false;
    return h.copyReturned==receipt->copied&&h.returned==receipt->finished&&
        (!h.returned||(h.retained==h.count&&h.references->complete()&&h.pc==0x26ebb0));
}
bool OriginalRacerModelSceneOwner50LA::withdrawSourceWarmMaterialsNative()noexcept {
    auto& h=sourceWarmMaterials_;if(!h.entered||h.nativeWithdrawn)return true;
    if(sourcePacketBorrower_||sourceObserverBorrower_||sourceSmokerBorrower_||fields_.packets1F0||
       !preflightSourceWarmMaterialsNative())return false;
    if(h.references){rr::assets::OriginalStandardMaterialDiagnostic50LA diagnostic;
        if(!h.references->release26E4C0(diagnostic))return false;
        if(h.references->started()&&!h.references->freeAllocation26E5EC())return false;}
    fields_.materials1C4=nullptr;h.nativeWithdrawn=true;return true;
}
OriginalRacerModelSceneOwner50LA::R OriginalRacerModelSceneOwner50LA::copySourceWarmParameters2406E0()noexcept {
    auto& h=sourceWarmReturn_;const auto& w=sourceWarmGeometry_;
    if(h.parametersEntered||!preflightSourceWarmGeometryNative()||w.nativeWithdrawn||w.nativeLedgerWithdrawn||
       !sourceWarmMaterials_.returned||sourceWarmMaterials_.nativeWithdrawn||
       !w.creator||!w.creator->preflightSourceRenderableReturn26E820Native()||
       !w.creator->quantizationLoaded_||quantizationLoaded_||rowTable_)
        return {S::phase,0x2406e0};
    h.parametersEntered=true;const auto& p=w.creator->fields_;
    for(unsigned lane=0;lane<4;++lane){h.pc=0x2406e4+8u*lane;
        std::memcpy(&fields_.scale1E0[lane],&p.scale1E0[lane],4);
        std::memcpy(&h.parameterWords[lane],&p.scale1E0[lane],4);h.parameterStoreMask|=std::uint16_t(1u<<lane);}
    for(unsigned lane=0;lane<3;++lane){h.pc=0x240704+8u*lane;
        std::memcpy(&fields_.offset1D0[lane],&p.offset1D0[lane],4);
        std::memcpy(&h.parameterWords[4+lane],&p.offset1D0[lane],4);h.parameterStoreMask|=std::uint16_t(1u<<(4+lane));}
    h.pc=0x24071c;fields_.offsetEnabled1DC=p.offsetEnabled1DC;
    h.parameterWords[7]=p.offsetEnabled1DC;h.parameterStoreMask|=0x80u;
    h.pc=0x240724;fields_.flags1B2=p.flags1B2;h.flags1B2=p.flags1B2;h.parameterStoreMask|=0x100u;
    h.pc=0x240730;fields_.flags1B3=p.flags1B3;h.flags1B3=p.flags1B3;h.parameterStoreMask|=0x200u;
    quantizationLoaded_=true;h.parametersReturned=true;
    h.pc=0x26ebc8;return {S::complete,h.pc}; //Delay-slot reload of genuine stack row count next.
}
OriginalRacerModelSceneOwner50LA::R OriginalRacerModelSceneOwner50LA::allocateSourceWarmRowTable26EE9C()noexcept {
    auto& h=sourceWarmReturn_;
    if(!h.parametersReturned||h.rowAllocationAttempted||!preflightSourceWarmGeometryNative()||
       sourceWarmGeometry_.nativeWithdrawn||sourceWarmGeometry_.nativeLedgerWithdrawn||
       !sourceWarmMaterials_.returned||sourceWarmMaterials_.nativeWithdrawn||
       !sourceWarmRows_.enumerationReturned||!sourceWarmRows_.count||sourceWarmRows_.count>rr::assets::maxModelNodes49O||
       binding_.sourceRowCount!=sourceWarmRows_.count||!(warmChildlessRows_||warmHierarchyRows_)||
       (warmHierarchyRows_&&!sourceWarmEnumeration_.mapBound)||
       !rowsLoaded_||model_.nodeCount1A0!=0||sourceRowObject(0)!=&model_||
       sourceRowTable_.attempted||rowTable_||fields_.sourceRows1B8||rowTableFreed_)
        return {S::phase,0x26ebc8};
    h.count=sourceWarmRows_.count;h.rowAllocationAttempted=true;
    h.pc=0x26ee9c;rowTable_.reset(new(std::nothrow) O*[h.count]);h.rowAllocationReturned=true;
    h.pc=0x26eea4;fields_.sourceRows1B8=rowTable_.get();h.published1B8=rowTable_.get();h.rowPublished=true;
    h.pc=0x26eeb4;if(!rowTable_)return {S::allocation,h.pc};
    for(std::uint32_t row=0;row<h.count;++row)
        rowTable_[row]=warmHierarchyRows_?sourceWarmEnumeration_.slots[row]:&model_;
    h.rowCopied=true; //Logical source slots, not native pointer-bit serialization.
    h.pc=0x26eec8;h.returnedEntity=&model_.entity8;h.returned=true;
    h.pc=0x26eee8;return {S::complete,h.pc}; //Actual26E820 only, not240740 final return.
}
bool OriginalRacerModelSceneOwner50LA::preflightSourceWarmReturnNative()const noexcept {
    const auto& h=sourceWarmReturn_;if(!h.parametersEntered)return !h.rowAllocationAttempted;
    if(!sourceWarmRows_.enumerationReturned||h.parameterStoreMask!=0x3ffu||!h.parametersReturned||!quantizationLoaded_)return false;
    //Copied bytes are history: later genuine240740/placement may change flags.
    //The lifetime check does not demand the copied flag snapshot forever.
    if(h.nativeRowsWithdrawn)return !rowTable_&&!fields_.sourceRows1B8&&rowTableFreed_;
    if(!h.rowAllocationAttempted)return !h.rowAllocationReturned&&!h.rowPublished&&!h.rowCopied&&!h.returned;
    if(!h.count||h.count>rr::assets::maxModelNodes49O||h.count!=sourceWarmRows_.count||
       !h.rowAllocationReturned||!h.rowPublished||h.published1B8!=rowTable_.get()||
       fields_.sourceRows1B8!=h.published1B8||binding_.sourceRowCount!=h.count||model_.nodeCount1A0!=0)return false;
    if(!rowTable_)return !h.rowCopied&&!h.returned&&!h.returnedEntity&&h.pc==0x26eeb4;
    for(std::uint32_t row=0;row<h.count;++row)
        if(rowTable_[row]!=sourceRowObject(row)||
           rowTable_[row]!=(warmHierarchyRows_?sourceWarmEnumeration_.slots[row]:&model_))return false;
    return h.rowCopied&&h.returned&&h.returnedEntity==&model_.entity8&&rowTable_[0]==&model_&&h.pc==0x26eee8;
}
bool OriginalRacerModelSceneOwner50LA::withdrawSourceWarmRowTableNative()noexcept {
    auto& h=sourceWarmReturn_;if(!h.rowAllocationAttempted||h.nativeRowsWithdrawn)return true;
    if(sourcePacketBorrower_||sourceObserverBorrower_||sourceSmokerBorrower_||fields_.packets1F0||
       !sourceWarmGeometry_.nativeWithdrawn||!sourceWarmMaterials_.nativeWithdrawn||
       !preflightSourceWarmReturnNative())return false;
    fields_.sourceRows1B8=nullptr;rowTable_.reset();rowTableFreed_=true;h.nativeRowsWithdrawn=true;return true;
}
bool OriginalRacerModelSceneOwner50LA::preflightSourceWarmRenderable26E820Native()const noexcept {
    const auto& w=sourceWarmGeometry_;const auto& h=sourceWarmReturn_;
    return h.returned&&!h.nativeRowsWithdrawn&&h.returnedEntity==&model_.entity8&&
        !w.nativeWithdrawn&&!w.nativeLedgerWithdrawn&&w.retained&&w.prototypeRead&&
        preflightSourceWarmGeometryNative()&&sourceWarmMaterials_.returned&&!sourceWarmMaterials_.nativeWithdrawn&&
        sourceWarmMaterials_.references&&sourceWarmMaterials_.references->complete()&&
        preflightSourceWarmReturnNative()&&!referenceReleased_&&!released_&&model_.constructed&&!model_.released;
}
OriginalRacerModelSceneOwner50LA::R OriginalRacerModelSceneOwner50LA::cloneSourceWarmHierarchy28E5F0(
    SceneGraph49P& graph,std::uint32_t backingCapacity,const SceneGraph49P& prototypeGraph)noexcept {
    auto& h=sourceWarmClone_;const auto& w=sourceWarmGeometry_;const auto& branch=sourceWarmRows_;
    if(h.entered||!preflightSourceWarmGeometryNative()||w.nativeWithdrawn||w.nativeLedgerWithdrawn||
       !branch.entered||!branch.cloneParentCleared||branch.pc!=0x28e5f0||branch.enumerationReturned||
       !w.creator||!w.creator->preflightSourceRenderableReturn26E820Native()||
       graph.nodes!=sourceConstructor_.canonicalNodes||graph.count!=1||backingCapacity!=rr::assets::maxModelNodes49O||
       prototypeGraph.nodes!=w.creator->sourceConstructor_.canonicalNodes||
       prototypeGraph.count!=w.creator->binding_.sourceRowCount||
       !validateSceneGraph49P(graph)||!validateSceneGraph49P(prototypeGraph)||
       (!branch.bit191&&at(prototypeGraph,w.creator->model_.sceneIndex).nextSibling188!=none)||
       (branch.bit191?branch.selectedCloneSource!=w.creator->object(
            at(prototypeGraph,w.creator->model_.sceneIndex).firstChild184):
            branch.selectedCloneSource!=&w.creator->model_)||
       sourceConstructor_.rowsBound||rowsLoaded_||directRowsStarted_)
        return {S::phase,0x28e5f0};
    h.entered=true;h.selectedChild=bool(branch.bit191);h.canonicalNodes=graph.nodes;h.slotCount=1;
    h.rows[0].source=branch.selectedCloneSource;indices_[0]=model_.sceneIndex;
    binding_.sourceRowCount=1;binding_.sourceRowIndices=indices_.data();sourceConstructor_.rowsBound=true;
    directRowsStarted_=true;sourceParents_[0]=-1;
    const auto stop=[&](S status,std::uint32_t pc){h.pc=pc;return R{status,pc};};
    const auto targetFor=[&](std::uint32_t slot)->O* {return slot?rows_[slot].get():&model_;};
    //Physical native slots stay stable. They are not a source row count or
    //a predecoded/preallocated hierarchy. Each source allocator is reached
    //only after the preceding recursive call has genuinely returned.
    const auto allocate=[&](std::uint32_t parent,bool sibling,const O* original)->R {
        if(h.slotCount>=backingCapacity||*binding_.actualObjectCount423F60==std::numeric_limits<std::uint32_t>::max())
            return stop(S::allocation,0x28d070);
        const auto slot=h.slotCount++;auto& receipt=h.rows[slot];receipt.source=original;
        receipt.allocationEntered=true;indices_[slot]=static_cast<I>(slot);
        auto& node=graph.nodes[slot];node.parent180=node.firstChild184=node.nextSibling188=none;
        graph.count=h.slotCount;binding_.sourceRowCount=h.slotCount;sourceParents_[slot]=-1;
        h.pc=0x28d070;rows_[slot].reset(new(std::nothrow) O);receipt.allocationNonNull=bool(rows_[slot]);
        if(rows_[slot]){const auto result=constructScene2893A0(*rows_[slot],indices_[slot],graph,true,K::scene2893A0);
            if(!result.okay())return stop(result.status,result.sourcePC);
            receipt.constructorReturned=true;}
        h.pc=0x28d090;auto& parentReceipt=h.rows[parent];auto& parentNode=at(graph,targetFor(parent)->sceneIndex);
        if(sibling){parentReceipt.siblingSlot=slot;parentNode.nextSibling188=rows_[slot]?indices_[slot]:none;}
        else {parentReceipt.childSlot=slot;parentNode.firstChild184=rows_[slot]?indices_[slot]:none;}
        receipt.incomingPublished=true;return {S::complete,0x28d0a0};
    };
    const auto clone=[&](auto&& self,std::uint32_t slot)->R {
        auto& receipt=h.rows[slot];auto* destination=targetFor(slot);const auto* source=receipt.source;
        if(receipt.entered||!destination||!destination->constructed||!source||!source->constructed||source->released||
           !inRange(prototypeGraph,source->sceneIndex))return stop(S::hierarchy,0x28e610);
        receipt.entered=true;auto& dst=*destination;const auto& src=*source;
        auto& dn=at(graph,dst.sceneIndex);const auto& sn=at(prototypeGraph,src.sceneIndex);
        h.pc=0x28e614;try{dst.name38=src.name38;}catch(...){return stop(S::allocation,h.pc);}receipt.nameCopied=true;
        for(unsigned pair=0;pair<8;++pair){h.pc=0x28e634;
            std::memcpy(&dn.localC0.v[2*pair],&sn.localC0.v[2*pair],4);
            h.pc=0x28e638;std::memcpy(&dn.localC0.v[2*pair+1],&sn.localC0.v[2*pair+1],4);
            receipt.localPairMask|=std::uint8_t(1u<<pair);}
        h.pc=0x28e660;dn.sceneFlags190=std::uint8_t(dn.sceneFlags190&~1u);receipt.flagStoreMask|=1u;
        h.pc=0x28e68c;dst.flags03=std::uint8_t((dst.flags03&~4u)|(src.flags03&4u));receipt.flagStoreMask|=2u;
        h.pc=0x28e6b0;dn.sceneFlags190=std::uint8_t((dn.sceneFlags190&~4u)|(sn.sceneFlags190&4u));receipt.flagStoreMask|=4u;
        h.pc=0x28e6d4;dn.sceneFlags190=std::uint8_t((dn.sceneFlags190&~32u)|(sn.sceneFlags190&32u));receipt.flagStoreMask|=8u;
        h.pc=0x28e6dc;std::memcpy(dst.bounds140.data(),src.bounds140.data(),16);receipt.boxStoreMask|=1u;
        h.pc=0x28e6e4;std::memcpy(dst.bounds150.data(),src.bounds150.data(),16);receipt.boxStoreMask|=2u;
        h.pc=0x28e6e8;
        if(src.radius194){std::uint32_t raw;std::memcpy(&raw,&*src.radius194,4);receipt.copiedRadius194=raw;}
        h.pc=0x28e6ec;
        //Raw move propagates unknown source bits without inventing a float.
        //No arithmetic or source readiness follows merely from this store.
        if(receipt.copiedRadius194){float value;const auto raw=*receipt.copiedRadius194;
            std::memcpy(&value,&raw,4);dst.radius194=value;}
        else dst.radius194.reset();
        receipt.radius194Stored=true;
        h.pc=0x28e6f4;std::memcpy(dst.bounds160.data(),src.bounds160.data(),16);receipt.boxStoreMask|=4u;
        h.pc=0x28e6fc;std::memcpy(dst.bounds170.data(),src.bounds170.data(),16);receipt.boxStoreMask|=8u;
        h.pc=0x28e700;
        if(src.radius198){std::uint32_t raw;std::memcpy(&raw,&*src.radius198,4);receipt.copiedRadius198=raw;}
        h.pc=0x28e704;
        //Raw move propagates unknown source bits without inventing a float.
        //No arithmetic or source readiness follows merely from this store.
        if(receipt.copiedRadius198){float value;const auto raw=*receipt.copiedRadius198;
            std::memcpy(&value,&raw,4);dst.radius198=value;}
        else dst.radius198.reset();
        receipt.radius198Stored=true;
        h.pc=0x28e728;dn.sceneFlags190=std::uint8_t((dn.sceneFlags190&~64u)|(sn.sceneFlags190&64u));receipt.flagStoreMask|=16u;
        if(sn.nextSibling188!=none){const auto* next=w.creator->object(sn.nextSibling188);
            if(!next)return stop(S::hierarchy,0x28e72c);
            const auto made=allocate(slot,true,next);if(!made.okay())return made;
            const auto childSlot=*receipt.siblingSlot;auto* sibling=targetFor(childSlot);
            h.pc=0x28e754;if(!sibling)return stop(S::hierarchy,h.pc);
            at(graph,sibling->sceneIndex).parent180=dn.parent180;receipt.siblingParentStored=true;
            sourceParents_[childSlot]=dn.parent180==none?-1:static_cast<std::int32_t>(dn.parent180);
            h.pc=0x28e75c;sibling->previous18C=dst.sceneIndex;receipt.siblingPreviousStored=true;
            const auto result=self(self,childSlot);if(!result.okay())return result;
        }else {h.pc=0x28e780;dn.nextSibling188=none;receipt.siblingNullStored=true;}
        if(sn.firstChild184!=none){const auto* original=w.creator->object(sn.firstChild184);
            if(!original)return stop(S::hierarchy,0x28e784);
            const auto made=allocate(slot,false,original);if(!made.okay())return made;
            const auto childSlot=*receipt.childSlot;auto* child=targetFor(childSlot);
            h.pc=0x28e7a8;if(!child)return stop(S::hierarchy,h.pc);
            at(graph,child->sceneIndex).parent180=dst.sceneIndex;receipt.childParentStored=true;
            sourceParents_[childSlot]=static_cast<std::int32_t>(dst.sceneIndex);
            const auto result=self(self,childSlot);if(!result.okay())return result;
        }else {h.pc=0x28e7cc;dn.firstChild184=none;receipt.childNullStored=true;}
        receipt.returned=true;h.pc=0x28e7e4;return {S::complete,h.pc};
    };
    //Selected NULL child is not an empty clone.28E5D0 parent clear already
    //happened;28E618 forms source+38 in the call delay slot. The actual
    //309230 source-name read is the next unsupported effect.
    if(!h.rows[0].source)return stop(S::hierarchy,0x28e618);
    const auto result=clone(clone,0);if(!result.okay())return result;
    h.returned=true;h.pc=h.selectedChild?0x26eab0:0x26eb24;
    return {S::complete,h.pc}; //Selected branch then preparesV0=1 at26EAB4. No1A0 write.
}
bool OriginalRacerModelSceneOwner50LA::preflightSourceWarmCloneNative()const noexcept {
    const auto& h=sourceWarmClone_;if(!h.entered)return true;
    if(!sourceWarmRows_.cloneParentCleared||h.selectedChild!=bool(sourceWarmRows_.bit191)||
       h.rows[0].source!=sourceWarmRows_.selectedCloneSource||
       !h.canonicalNodes||h.canonicalNodes!=sourceConstructor_.canonicalNodes||
       !h.slotCount||h.slotCount>rr::assets::maxModelNodes49O||binding_.sourceRowCount!=h.slotCount||
       !sourceConstructor_.rowsBound||!directRowsStarted_||rowsLoaded_!=sourceWarmEnumeration_.mapBound)return false;
    const auto physical=[&](std::uint32_t slot)->const O* {
        if(!slot)return &model_;
        return sourceWarmEnumeration_.mapBound?this->object(static_cast<I>(slot)):rows_[slot].get();
    };
    for(std::uint32_t slot=0;slot<h.slotCount;++slot){const auto& r=h.rows[slot];
        const auto* object=physical(slot);
        if((sourceWarmEnumeration_.mapBound?(object&&object->sceneIndex!=static_cast<I>(slot)):
            indices_[slot]!=static_cast<I>(slot))||
           (!r.source&&!(slot==0&&h.selectedChild&&!r.entered&&!h.returned&&h.pc==0x28e618))||
           (slot&&(!r.allocationEntered||r.allocationNonNull!=bool(object)||
            (r.constructorReturned&&(!object||!object->constructed)))))return false;
        if(r.entered&&(!object||!object->constructed))return false;
        const auto copied=[](const std::optional<float>& value,const std::optional<std::uint32_t>& receipt){
            if(bool(value)!=bool(receipt))return false;if(!value)return true;
            std::uint32_t raw;std::memcpy(&raw,&*value,4);return raw==*receipt;
        };
        if(r.radius194Stored&&(!object||!copied(object->radius194,r.copiedRadius194)))return false;
        if(r.radius198Stored){if(!object)return false;
            if(slot==0&&(sourceBounds_.aggregateStoreMask&4u)){
                if(!copied(object->radius198,std::optional<std::uint32_t>{sourceBounds_.radius198}))return false;
            }else if(!copied(object->radius198,r.copiedRadius198))return false;
        }
        if((r.copiedRadius194&&!r.radius194Stored)||(r.copiedRadius198&&!r.radius198Stored))return false;
        if(r.localPairMask&&!r.nameCopied)return false;
        if(r.flagStoreMask&&r.localPairMask!=255u)return false;
        if((r.boxStoreMask&12u)&&!r.radius194Stored)return false;
        if((r.flagStoreMask&16u)&&!r.radius198Stored)return false;
        if(r.siblingSlot&&(*r.siblingSlot<=slot||*r.siblingSlot>=h.slotCount||!h.rows[*r.siblingSlot].incomingPublished))return false;
        if(r.childSlot&&(*r.childSlot<=slot||*r.childSlot>=h.slotCount||!h.rows[*r.childSlot].incomingPublished))return false;
        if(object&&object->constructed){const auto& node=h.canonicalNodes[slot];
            if(r.siblingSlot){const auto next=*r.siblingSlot;const auto* sibling=physical(next);
                const bool detached=h.nativeForestDetached&&node.parent180==none;
                if(node.nextSibling188!=(!detached&&sibling?static_cast<I>(next):none))return false;
                if(r.siblingParentStored&&(!sibling||h.canonicalNodes[next].parent180!=node.parent180))return false;
                if(r.siblingPreviousStored&&(!sibling||sibling->previous18C!=(detached?none:static_cast<I>(slot))))return false;
            }else if(r.siblingNullStored&&node.nextSibling188!=none)return false;
            if(r.childSlot){const auto next=*r.childSlot;const auto* child=physical(next);
                if(node.firstChild184!=(child?static_cast<I>(next):none))return false;
                if(r.childParentStored&&(!child||h.canonicalNodes[next].parent180!=static_cast<I>(slot)))return false;
            }else if(r.childNullStored&&node.firstChild184!=none)return false;
        }
        if(r.returned&&(!r.entered||r.localPairMask!=255u||r.flagStoreMask!=31u||r.boxStoreMask!=15u||
           !r.radius194Stored||!r.radius198Stored||(!r.siblingSlot&&!r.siblingNullStored)||
           (!r.childSlot&&!r.childNullStored)))return false;
    }
    return !h.returned||(h.rows[0].returned&&h.pc==(h.selectedChild?0x26eab0u:0x26eb24u));
}
OriginalRacerModelSceneOwner50LA::R OriginalRacerModelSceneOwner50LA::enumerateSourceWarmRows28C400(
    const SceneGraph49P& graph)noexcept {
    auto& h=sourceWarmEnumeration_;auto& rows=sourceWarmRows_;
    if(h.entered||!preflightSourceWarmGeometryNative()||sourceWarmGeometry_.nativeWithdrawn||
       sourceWarmGeometry_.nativeLedgerWithdrawn||!sourceWarmClone_.returned||
       sourceWarmClone_.pc!=(sourceWarmClone_.selectedChild?0x26eab0u:0x26eb24u)||
       rows.enumerationReturned||rows.countSeeded||rows.rowZeroStored||rowsLoaded_||
       graph.nodes!=sourceWarmClone_.canonicalNodes||graph.count!=sourceWarmClone_.slotCount||
       !preflightSourceWarmForestGraphNative(graph)||model_.nodeCount1A0!=0)return {S::phase,0x26eb24};
    h.entered=true;h.pc=0x26eb34;rows.count=1;rows.countSeeded=true;
    h.pc=0x26eb3c;h.slots[0]=&model_;h.written[0]=true;rows.rowZeroStored=true;
    const auto stop=[&](S status,std::uint32_t pc){h.pc=pc;return R{status,pc};};
    const auto append=[&](I index,std::uint32_t store,std::uint32_t countStore)->R {
        h.pc=store;
        if(rows.count>=h.slots.size())return {S::sourceRowTable,h.pc};
        auto* row=object(index);if(!row||!row->constructed||row->released)return {S::hierarchy,h.pc};
        h.slots[rows.count]=row;h.written[rows.count]=true; //Actual pointer slot FIRST.
        h.pc=countStore;++rows.count; //28C448/488 branch-delay count store.
        return {S::complete,h.pc};
    };
    const auto enumerate=[&](auto&& self,O* parent)->R {
        if(!parent||!inRange(graph,parent->sceneIndex))return stop(S::hierarchy,0x28c41c);
        const auto first=at(graph,parent->sceneIndex).firstChild184;
        for(auto child=first;child!=none;child=at(graph,child).nextSibling188){
            const auto result=append(child,0x28c434,0x28c448);if(!result.okay())return result;
        }
        //Not DFS: append all children, then for each child append all its
        //children before recursively visiting those grandchildren in order.
        for(auto child=first;child!=none;child=at(graph,child).nextSibling188){
            const auto grandchildren=at(graph,child).firstChild184;
            for(auto next=grandchildren;next!=none;next=at(graph,next).nextSibling188){
                const auto result=append(next,0x28c474,0x28c488);if(!result.okay())return result;
            }
            for(auto next=grandchildren;next!=none;next=at(graph,next).nextSibling188){
                h.pc=0x28c4a8;const auto result=self(self,object(next));if(!result.okay())return result;
            }
        }
        h.pc=0x28c4e8;return {S::complete,h.pc};
    };
    const auto result=enumerate(enumerate,&model_);if(!result.okay())return result;
    h.returned=true;rows.enumerationReturned=true;rows.pc=0x26eb40;
    //Only after actual source enumeration: remap native unique owners and
    //indices to source row order without moving any SceneObject/graph node.
    if(rows.count!=sourceWarmClone_.slotCount)return stop(S::sourceRowTable,0x26eb40);
    std::array<std::uint32_t,rr::assets::maxModelNodes49O> oldSlots{};
    for(std::uint32_t row=1;row<rows.count;++row){bool found=false;
        for(std::uint32_t slot=1;slot<sourceWarmClone_.slotCount;++slot)if(rows_[slot].get()==h.slots[row]){
            oldSlots[row]=slot;found=true;break;}
        if(!found)return stop(S::sourceRowTable,0x26eb40);
        for(std::uint32_t prior=0;prior<row;++prior)if(h.slots[prior]==h.slots[row])return stop(S::hierarchy,0x26eb40);
    }
    std::array<std::int32_t,rr::assets::maxModelNodes49O> parents{};parents[0]=-1;
    for(std::uint32_t row=1;row<rows.count;++row){parents[row]=-1;
        const auto parent=at(graph,h.slots[row]->sceneIndex).parent180;
        for(std::uint32_t prior=0;prior<row;++prior)if(h.slots[prior]->sceneIndex==parent){parents[row]=static_cast<std::int32_t>(prior);break;}
        if(parents[row]<0)return stop(S::hierarchy,0x26eb40);
    }
    std::array<std::unique_ptr<O>,rr::assets::maxModelNodes49O> reordered;
    for(std::uint32_t row=1;row<rows.count;++row)reordered[row]=std::move(rows_[oldSlots[row]]);
    rows_=std::move(reordered);
    for(std::uint32_t row=0;row<rows.count;++row){indices_[row]=h.slots[row]->sceneIndex;sourceParents_[row]=parents[row];}
    binding_.sourceRowCount=rows.count;binding_.sourceRowIndices=indices_.data();
    warmHierarchyRows_=true;rowsLoaded_=true;h.mapBound=true;h.pc=0x26eb40;return {S::complete,h.pc};
}
bool OriginalRacerModelSceneOwner50LA::preflightSourceWarmEnumerationNative()const noexcept {
    const auto& h=sourceWarmEnumeration_;if(!h.entered)return true;
    const auto& rows=sourceWarmRows_;
    if(!sourceWarmClone_.returned||!rows.countSeeded||!rows.rowZeroStored||!h.written[0]||h.slots[0]!=&model_||
       !rows.count||rows.count>h.slots.size())return false;
    for(std::uint32_t row=0;row<rows.count;++row){if(!h.written[row]||!h.slots[row])return false;
        if(h.mapBound&&(sourceRowObject(row)!=h.slots[row]||indices_[row]!=h.slots[row]->sceneIndex))return false;}
    if(h.returned!=rows.enumerationReturned)return false;
    return !h.mapBound||(h.returned&&rowsLoaded_&&warmHierarchyRows_&&!warmChildlessRows_&&
        rows.count==sourceWarmClone_.slotCount&&binding_.sourceRowCount==rows.count&&h.pc==0x26eb40);
}
bool OriginalRacerModelSceneOwner50LA::preflightSourceRenderableReturn26E820Native()const noexcept {
    if(sourceWarmGeometry_.attempted)return preflightSourceWarmRenderable26E820Native();
    const auto& h=sourceRowTable_;const auto& input=sourceDirectRows_;
    return h.attempted&&h.returned&&!h.nativeWithdrawn&&h.returnedEntityV0==&model_.entity8&&
        !released_&&!referenceReleased_&&model_.constructed&&!model_.released&&
        input.returned&&!input.nativeScratchWithdrawn&&input.sourceFile&&
        input.sourceFile.get()==h.closedFile&&input.sourceFile->sourceCloseReturned()&&
        input.sourceFile->preflightNative()&&
        sourceMaterials_.returned&&!sourceMaterials_.nativeWithdrawn&&preflightSourceMaterialsNative()&&
        sourceGeometry_.prototypeStored&&!sourceGeometry_.nativeWithdrawn&&sourceGeometry_.owner&&
        preflightSourceGeometryNative()&&
        input.sourceFile->associatedGeometryCurrent(sourceGeometry_.owner.get(),this)&&
        preflightSourceRowTableNative();
}
const OriginalRacerModelSceneOwner50LA::C* OriginalRacerModelSceneOwner50LA::sourceRenderableEntity26E820Native()const noexcept {
    if(!preflightSourceRenderableReturn26E820Native())return nullptr;
    return sourceWarmGeometry_.attempted?sourceWarmReturn_.returnedEntity:sourceRowTable_.returnedEntityV0;
}
bool OriginalRacerModelSceneOwner50LA::borrowSourcePacketPalette240210Native(
    const rr::assets::OriginalStandardModelGeometry50LA* geometry,SourcePacketPaletteBorrow240210& out)const noexcept {
    const auto* actual=sourceWarmGeometry_.attempted?sourceWarmGeometry_.geometry:sourceGeometry_.owner.get();
    if(!preflightSourceRenderableReturn26E820Native()||geometry!=actual||
       !geometry||!geometry->storage_||!geometry->sourcePaletteCurrent())return false;
    const auto& h=geometry->storage_->sourcePalette;if(!h.returned)return false;
    SourcePacketPaletteBorrow240210 view;view.descriptor20=h.published20;
    if(h.published20){const auto& d=*h.published20;
        if(!d.countStored)return false;
        view.count00=d.count00;view.sizes04=d.published04;
    }
    //Typed borrowed identities/nullable lanes only. No decoded palette,
    //source retain, NULL substitution, pointer-bit serialization or readiness.
    out=view;return true;
}
}

namespace rr::assets {
bool OriginalSourcePsgFile50LA::sourceGeometryWithdrawalAllowedNative(const rr::game::OriginalRacerModelSceneOwner50LA* model)const noexcept {
    return model&&model==associatedModel_&&!model->sourcePacketBorrower_&&!model->sourceObserverBorrower_&&!model->sourceSmokerBorrower_;
}
}
