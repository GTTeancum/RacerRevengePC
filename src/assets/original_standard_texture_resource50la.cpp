#include "original_standard_texture_resource50la.h"
#include "original_texture_candidate50tc.h"
#include "original_standard_model_resource50la.h"
#include "../game/original_racer_animation_cold50la.h"
#include "original_standard_collision_type0_resource50la.h"
#include "original_standard_material_context50la.h"
#include "original_standard_texture_demand_math50la.h"
#include "original_standard_archive_provider50la.h"
#include "original_ui_atm50kg.h"
#include "original_ui_ipum_texture_cache50kg.h"
#include "original_tga_conversion50la.h"
#include "original_standard_ipum_framing50la.h"
#include "original_standard_ipum_image_lease50la.h"
#include <cstring>
#include <new>
#include <algorithm>
#include <exception>
#include <limits>
#include <utility>

namespace rr::assets {
namespace {
using D=OriginalStandardTextureDiagnostic50LA;
using E=OriginalStandardTextureStatus50LA;
bool fail(D& d,E status,std::uint32_t pc,std::size_t at=0) noexcept {
    d.status=status;d.sourcePC=pc;d.byteOffset=at;return false;
}
unsigned char fold(unsigned char c) noexcept {
    return c>='A'&&c<='Z'?static_cast<unsigned char>(c+('a'-'A')):c;
}
bool same(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i)
        if(fold(static_cast<unsigned char>(a[i]))!=fold(static_cast<unsigned char>(b[i])))return false;
    return true;
}
bool nameDomain(const std::string& name) noexcept {
    if(name.empty()||name.size()>127)return false;
    for(unsigned char c:name)if(c<32||c>=127)return false;
    return true;
}
bool pstName(const std::string& name) noexcept {
    const auto n=name.size();
    return nameDomain(name)&&n>=4&&name[n-4]=='.'&&
        fold(name[n-3])=='p'&&fold(name[n-2])=='s'&&fold(name[n-1])=='t';
}
bool tgaName(const std::string& name) noexcept {
    const auto n=name.size();
    return nameDomain(name)&&n>=4&&name[n-4]=='.'&&
        fold(name[n-3])=='t'&&fold(name[n-2])=='g'&&fold(name[n-1])=='a';
}
std::uint32_t u32(const std::vector<std::uint8_t>& b,std::size_t p) noexcept {
    return std::uint32_t(b[p])|(std::uint32_t(b[p+1])<<8)|
        (std::uint32_t(b[p+2])<<16)|(std::uint32_t(b[p+3])<<24);
}
bool range(const std::vector<std::uint8_t>& b,std::size_t at,std::size_t bytes) noexcept {
    return at<=b.size()&&bytes<=b.size()-at;
}
bool dimension(std::uint32_t n) noexcept {return n>=16&&n<=1024&&!(n&(n-1));}

//Literal original data words382FD0..3831CC, not a GS/PCSX2 address table.
//24D3E0 alternates two64-entry byte maps over four64-byte input portions.
constexpr std::array<std::uint8_t,128> bytePermutation382FD0={{
     0,36, 8,44, 1,37, 9,45, 2,38,10,46, 3,39,11,47,
     4,32,12,40, 5,33,13,41, 6,34,14,42, 7,35,15,43,
    16,52,24,60,17,53,25,61,18,54,26,62,19,55,27,63,
    20,48,28,56,21,49,29,57,22,50,30,58,23,51,31,59,
     4,32,12,40, 5,33,13,41, 6,34,14,42, 7,35,15,43,
     0,36, 8,44, 1,37, 9,45, 2,38,10,46, 3,39,11,47,
    20,48,28,56,21,49,29,57,22,50,30,58,23,51,31,59,
    16,52,24,60,17,53,25,61,18,54,26,62,19,55,27,63
}};
//Keep the distinct original tables even though their words match.24D470
//uses383250 to build inverse output coordinates and3831D0 for source blocks.
constexpr std::array<std::uint8_t,32> block8Table3831D0={{
     0, 1, 4, 5,16,17,20,21, 2, 3, 6, 7,18,19,22,23,
     8, 9,12,13,24,25,28,29,10,11,14,15,26,27,30,31
}};
constexpr std::array<std::uint8_t,32> block32Table383250={{
     0, 1, 4, 5,16,17,20,21, 2, 3, 6, 7,18,19,22,23,
     8, 9,12,13,24,25,28,29,10,11,14,15,26,27,30,31
}};
void scalar24D3E0(const std::array<std::uint8_t,256>& source,
    std::array<std::uint8_t,256>& output) noexcept {
    for(std::size_t portion=0;portion<4;++portion)
        for(std::size_t i=0;i<64;++i)
            output[64*portion+i]=source[64*portion+bytePermutation382FD0[64*(portion&1)+i]];
}
void scalar24D470(const std::array<std::uint8_t,8192>& source,
    std::array<std::uint8_t,8192>& output) noexcept {
    //24D4C4..24D510: inverse coordinates for every original output block.
    std::array<std::uint8_t,32> outputX{},outputY{};
    for(std::size_t y=0;y<4;++y)for(std::size_t x=0;x<8;++x){
        const auto block=block32Table383250[8*y+x];
        outputX[block]=static_cast<std::uint8_t>(x);
        outputY[block]=static_cast<std::uint8_t>(y);
    }
    std::array<std::uint8_t,256> block{},converted{};
    for(std::size_t by=0;by<4;++by)for(std::size_t bx=0;bx<8;++bx){
        //24D5C0: gather16 source rows of16 bytes at128-byte stride.
        for(std::size_t y=0;y<16;++y)for(std::size_t x=0;x<16;++x)
            block[16*y+x]=source[(16*by+y)*128+16*bx+x];
        scalar24D3E0(block,converted);
        const auto identity=block8Table3831D0[8*by+bx];
        //24D60C/618/620: scatter8 rows of32 bytes at256-byte stride.
        const auto dx=std::size_t(outputX[identity]),dy=std::size_t(outputY[identity]);
        for(std::size_t y=0;y<8;++y)for(std::size_t x=0;x<32;++x)
            output[(8*dy+y)*256+32*dx+x]=converted[32*y+x];
    }
}
void scalar24D6F0(const std::uint8_t* source,std::size_t width,std::size_t height,
    std::vector<std::uint8_t>& output) {
    //The caller has structurally admitted power-of-two dimensions16..1024.
    //24D7CC/7DC zero the two page buffers once. With this subset, every page
    //uses the same copy extent: narrow/short images have only one page on
    //that axis; larger images contain complete pages. No stale edge bytes.
    std::array<std::uint8_t,8192> page{},converted{};
    const auto pagesX=(width+127)/128,pagesY=(height+63)/64;
    const auto copyWidth=width<128?width:128,copyHeight=height<64?height:64;
    output.resize(width*height);
    for(std::size_t py=0;py<pagesY;++py)for(std::size_t px=0;px<pagesX;++px){
        //24D900: source stride is the original width, page stride128.
        for(std::size_t y=0;y<copyHeight;++y)for(std::size_t x=0;x<copyWidth;++x)
            page[128*y+x]=source[(64*py+y)*width+128*px+x];
        scalar24D470(page,converted); //24D928 always supplies128x64.
        //24D958: half as many rows, twice as many bytes per output row.
        for(std::size_t y=0;y<copyHeight/2;++y)for(std::size_t x=0;x<copyWidth*2;++x)
            output[(32*py+y)*(width*2)+px*256+x]=converted[256*y+x];
    }
}
}
// Only the actual resolver can mint this bridge from the cache's fixed
// Garage catalog and a positive current provider lookup. It owns immutable
// pixels and original archive bytes; bank closure does not invalidate a live
// Resource's already-loaded image. No decoded arrays or codec claims are copied.
class OriginalStandardGarageIpumLease50KG final {
    friend class OriginalStandardTextureResolver50LA;
    friend class OriginalStandardTextureResource50LA;
    std::shared_ptr<const OriginalUiIpumTextureCache50KG> cache;
    std::shared_ptr<const OriginalUiArchive50KG> archive;
    OriginalUiMemberView50KG member{};
    const Texture48E* texture=nullptr;
    const OriginalUiIpumProvenance50KG* provenance=nullptr;
    OriginalStandardGarageIpumLease50KG() noexcept=default;
public:
    bool current() const noexcept {
        return cache&&archive&&archive->loaded()&&member.identity&&member.data&&
            texture&&provenance&&cache->textureFor(member,0,member.size)==texture&&
            cache->provenanceFor(member,0,member.size)==provenance&&
            provenance->decodedInProcess&&
            provenance->colorPolicyKind==OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1;
    }
    bool matches(const std::string& name,const std::vector<std::uint8_t>& bytes)const noexcept {
        return current()&&same(provenance->sourceMember,name)&&bytes.size()==member.size&&
            std::equal(bytes.begin(),bytes.end(),member.data);
    }
};
struct OriginalStandardTextureResolver50LA::Storage {
    // Allocation identity is pinned by opaque leases; resolver address reuse
    // cannot revive a capability issued by a destroyed manager.
    const std::shared_ptr<const int> managerIdentity=std::make_shared<const int>(0);
    enum class FileKind : std::uint8_t { pst,tga,atm,mcf,psg,colType0 };
    struct File {
        bool retired=false;
        std::size_t bank=static_cast<std::size_t>(-1),bankOrdinal=static_cast<std::size_t>(-1);
        std::uint64_t bankGeneration=0;
        std::string member;
        std::vector<std::uint8_t> bytes;
        FileKind kind=FileKind::pst;
        OriginalStandardTextureSourceFormat50LA format=OriginalStandardTextureSourceFormat50LA::pstIndexed;
        // Actual source-pointer borrows, including partial constructed PSTs.
        // Unlike cached/cacheIdentity, bank12E5E0 cannot clear this authority.
        std::size_t pstBorrowers=0;
        std::uint64_t pstPublication=0;
        struct FinalPstRelease {
            std::size_t bank=static_cast<std::size_t>(-1),bytes=0;
            std::uint64_t generation=0,publication=0;
        } finalPstRelease;
        std::weak_ptr<const OriginalStandardIpumImageSet50LA> ipumImages;
        std::shared_ptr<const OriginalStandardGarageIpumLease50KG> garageIpum;
        std::weak_ptr<OriginalStandardTextureResource50LA> cached;
        const OriginalStandardTextureResource50LA* cacheIdentity=nullptr;
        const rr::game::OriginalRacerAnimationCold50LA* animation=nullptr;
        // Stable publication identity, not another animation reference and
        // never dereferenced after its owner dies. Bank closure clears the
        // source cache, but cannot impersonate actual153F50/final1413E0.
        struct AnimationPublication {
            std::uintptr_t owner=0;
            std::uint64_t sessionGeneration=0,objectKey=0;
        } animationPublication;
        struct FinalAnimationRelease {
            std::size_t bank=static_cast<std::size_t>(-1),bytes=0;
            std::uint64_t generation=0;
            AnimationPublication publication;
        } finalAnimationRelease;
        std::weak_ptr<OriginalStandardModelGeometry50LA> geometry;
        const OriginalStandardModelGeometry50LA* geometryIdentity=nullptr;
        // Native payload borrower, distinct from the source1326A0 cache.
        //12E5E0 removes the cache while this exact resource may remain live.
        // This weak proof neither retains a source reference nor republishes it.
        std::weak_ptr<OriginalStandardModelGeometry50LA> payloadGeometry;
        // Source-final-release receipt, minted only from the actual live
        // resource after refs00==0/payload/1327D0. Weak expiration alone is
        // never sufficient. Every new1326A0 publication invalidates this.
        struct FinalGeometryRelease {
            std::size_t bank=static_cast<std::size_t>(-1),bytes=0;
            std::uint64_t generation=0;
            std::weak_ptr<OriginalStandardModelGeometry50LA> publication;
        } finalGeometryRelease;
        std::weak_ptr<OriginalStandardCollisionType0Resource50LA> collision;
        const OriginalStandardCollisionType0Resource50LA* collisionIdentity=nullptr;
    };
    struct Level {
        std::int32_t width=0,height=0;
        std::size_t header=0,indices=0,bytes=0;
    };
    struct Entry {
        File* file=nullptr;
        std::array<Level,5> levels{}; //File order, smallest first.
        std::int32_t count=0;
        std::uint32_t kind=0;
        std::size_t pstOffset=0,pstBytes=0;
        //Internal same-file substreams, never provider rows or cached aliases.
        std::vector<Entry> frames;
    };
    //Pointees stay stable when later authenticated model rows are admitted.
    //Every source family shares this one name/association domain.
    std::vector<std::unique_ptr<File>> files;
    std::vector<Entry> entries;
    // At most the existing11 fixed Garage catalog sources; no new image limit.
    std::vector<std::shared_ptr<const OriginalStandardGarageIpumLease50KG>> garageIpum;
    std::size_t modelBytes=0,textureBytes=0;
    std::weak_ptr<const OriginalStandardMaterialContext50LA> construction;
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider;
    bool bound=false,texturesPrepared=false;
    // Provenance is minted by the bound authenticated provider's actual lookup,
    // including selected ordinal and exact bytes, never a filename prefix/list.
    static bool matchBank(const File& file,const OriginalStandardArchiveProvider50LA& provider,
        OriginalStandardArchiveProvider50LA::MemberView& view) noexcept {
        return provider.lookup132820(file.member,view)==OriginalStandardArchiveProvider50LA::Lookup::present&&
            view.member&&same(*view.member,file.member)&&view.data&&view.size==file.bytes.size()&&
            view.bankGeneration&&view.revision==provider.revision()&&
            std::equal(file.bytes.begin(),file.bytes.end(),view.data);
    }
    static void ownBank(File& file,const OriginalStandardArchiveProvider50LA::MemberView& view) noexcept {
        file.bank=view.mount;file.bankOrdinal=view.ordinal;file.bankGeneration=view.bankGeneration;
        file.finalPstRelease={}; // New real bank association invalidates old proof.
    }
};
struct OriginalStandardIpumCompressedFrames50LA::Storage {
    struct FreeAligned {
        void operator()(std::uint8_t* p)const noexcept {
            ::operator delete(p,std::align_val_t(64));
        }
    };
    struct Frame {
        FrameView view{};
        std::unique_ptr<std::uint8_t,FreeAligned> bytes;
        bool everAdopted=false;
        const OriginalStandardTextureResource50LA* canonicalOwner=nullptr;
    };
    // First: source and provider backing outlive every compressed child.
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver;
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider;
    std::size_t source=0,mount=0,memberOrdinal=0;
    std::string archiveDigest;
    std::vector<Frame> frames;
    std::size_t copied=0;
    bool complete=false;
};
OriginalStandardIpumCompressedFrames50LA::OriginalStandardIpumCompressedFrames50LA() noexcept=default;
OriginalStandardIpumCompressedFrames50LA::~OriginalStandardIpumCompressedFrames50LA(){
    // One backing owner. Immutable image borrows can pin bytes after logical
    // resource retirement; this is native CPU retention, never a GPU drain.
    if(storage_)for(auto& f:storage_->frames){
        if(f.canonicalOwner)std::terminate();f.view={};f.bytes.reset();
    }
}
bool OriginalStandardIpumCompressedFrames50LA::canAdopt(std::size_t i)const noexcept {
    return storage_&&storage_->complete&&i<storage_->frames.size()&&
        !storage_->frames[i].everAdopted&&!storage_->frames[i].canonicalOwner;
}
bool OriginalStandardIpumCompressedFrames50LA::adopt(std::size_t i,const OriginalStandardTextureResource50LA* owner)const noexcept {
    if(!owner||!canAdopt(i))return false;
    auto& f=storage_->frames[i];f.everAdopted=true;f.canonicalOwner=owner;return true;
}
void OriginalStandardIpumCompressedFrames50LA::retire(std::size_t i,const OriginalStandardTextureResource50LA* owner)const noexcept {
    if(!storage_||i>=storage_->frames.size()||storage_->frames[i].canonicalOwner!=owner)std::terminate();
    storage_->frames[i].canonicalOwner=nullptr; // everAdopted is NEVER reset.
}
bool OriginalStandardIpumCompressedFrames50LA::compressedComplete()const noexcept {
    return storage_&&storage_->complete;
}
std::size_t OriginalStandardIpumCompressedFrames50LA::copiedFrameCount()const noexcept {
    return storage_?storage_->copied:0;
}
bool OriginalStandardIpumCompressedFrames50LA::frame(std::size_t i,FrameView& out)const noexcept {
    out={};if(!storage_||i>=storage_->copied)return false;
    out=storage_->frames[i].view;return true;
}
const OriginalStandardTextureResolver50LA* OriginalStandardIpumCompressedFrames50LA::resolverIdentity()const noexcept {
    return storage_?storage_->resolver.get():nullptr;
}
std::size_t OriginalStandardIpumCompressedFrames50LA::sourceIndex()const noexcept {
    return storage_?storage_->source:static_cast<std::size_t>(-1);
}
const std::vector<std::uint8_t>* OriginalStandardIpumCompressedFrames50LA::originalBytes()const noexcept {
    return storage_?storage_->resolver->originalBytes(storage_->source):nullptr;
}
const std::string* OriginalStandardIpumCompressedFrames50LA::sourceMember()const noexcept {
    return storage_?storage_->resolver->member(storage_->source):nullptr;
}
const std::string* OriginalStandardIpumCompressedFrames50LA::archiveSha256()const noexcept {
    return storage_?&storage_->archiveDigest:nullptr;
}
bool OriginalStandardTextureResolver50LA::stageIpumFlags0(
    std::shared_ptr<OriginalStandardTextureResolver50LA> actualSelf,std::size_t index,
    std::shared_ptr<const OriginalStandardIpumCompressedFrames50LA>& out,D& d) noexcept {
    d={};d.source=index;
    if(out)return fail(d,E::alreadyPrepared,0x102230);
    if(actualSelf.get()!=this||!prepared()||index>=storage_->entries.size())
        return fail(d,E::source,0x132820);
    using Provider=OriginalStandardArchiveProvider50LA;
    const auto& entry=storage_->entries[index];
    if(entry.file->retired||entry.file->format!=OriginalStandardTextureSourceFormat50LA::atmPstFrames||
       entry.kind!=2||entry.frames.empty())return fail(d,E::unsupported,0x24F148);
    const auto provider=storage_->provider;
    Provider::MemberView member;Provider::MountView mount;
    if(!provider||!provider->sealed()||
       provider->lookup132820(entry.file->member,member)!=Provider::Lookup::present||
       !provider->mount(member.mount,mount)||!mount.archiveSha256||
       member.size!=entry.file->bytes.size()||!member.data||
       std::memcmp(member.data,entry.file->bytes.data(),member.size))
        return fail(d,E::source,0x132820);
    const auto revision=provider->revision();
    std::uint32_t pc=0x102344;std::size_t offset=0;
    try {
        auto next=std::shared_ptr<OriginalStandardIpumCompressedFrames50LA>(
            new OriginalStandardIpumCompressedFrames50LA());
        next->storage_=std::make_unique<OriginalStandardIpumCompressedFrames50LA::Storage>();
        auto& state=*next->storage_;state.resolver=std::move(actualSelf);
        state.provider=provider;state.source=index;state.mount=member.mount;
        state.memberOrdinal=member.ordinal;state.archiveDigest=*mount.archiveSha256;
        state.frames.resize(entry.frames.size());
        // Preserve a genuine partial CPU prefix on failure. This is host
        // staging publication, not a claim of original resource side effects.
        out=next;
        for(std::size_t ordinal=0;ordinal<entry.frames.size();++ordinal){
            const auto& f=entry.frames[ordinal];const auto mip=static_cast<std::size_t>(f.count-1);
            const auto& level=f.levels[mip];auto& dest=state.frames[ordinal];
            pc=0x2507E8;offset=level.indices;
            dest.bytes.reset(static_cast<std::uint8_t*>(::operator new(level.bytes,std::align_val_t(64))));
            // Original2507F0 stores allocation before24F198 reads payload.
            std::memcpy(dest.bytes.get(),entry.file->bytes.data()+level.indices,level.bytes);
            dest.view={ordinal,f.pstOffset,f.pstBytes,mip,level.header,level.indices,level.bytes,
                static_cast<std::uint32_t>(level.width),static_cast<std::uint32_t>(level.height),
                dest.bytes.get(),dest.bytes.get()+16,level.bytes-16};
            ++state.copied;
        }
        if(!provider->sealed()||provider->revision()!=revision)return fail(d,E::phase,0x132820);
        state.complete=true;return true;
    }catch(...){return fail(d,E::allocation,pc,offset);}
}
bool OriginalStandardTextureResolver50LA::bindIpumImages(
    const std::shared_ptr<const OriginalStandardIpumImageSet50LA>& images,
    const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,D& d) noexcept {
    d={};
    if(!images||images->count()!=6||!images->frame(0)||!storage_||!context||!context->live())
        return fail(d,E::input,0x24F148);
    const auto index=images->frame(0)->sourceIndex();
    if(index>=storage_->entries.size())return fail(d,E::source,0x132820);
    auto& root=storage_->entries[index];auto& file=*root.file;
    if(file.retired||root.kind!=2||root.frames.size()!=6||file.format!=OriginalStandardTextureSourceFormat50LA::atmPstFrames||
       file.cacheIdentity||!file.cached.expired())return fail(d,E::phase,0x1326A0);
    if(const auto existing=file.ipumImages.lock()){
        if(existing!=images)return fail(d,E::phase,0x24F148);
    }
    const auto* stage=images->frame(0)->compressedIdentity();
    for(std::size_t i=0;i<6;++i){const auto& image=images->frame(i);
        if(!image||image->resolverIdentity()!=this||image->sourceIndex()!=index||
           image->compressedIdentity()!=stage||image->sourceFrame().frame!=i||!stage->canAdopt(i))
            return fail(d,E::source,0x24F148);
    }
    // Existing binding validates SAME context and all live provider caches.
    // Re-resolve exact mounted bytes/absence with current sealed revision.
    if(!bind(context,d))return false;
    const auto revision=storage_->provider?storage_->provider->revision():0;
    std::size_t resolved=static_cast<std::size_t>(-1);std::string name;
    if(!resolve24DADC(file.member,resolved,name,d)||resolved!=index||!storage_->provider||
       !storage_->provider->sealed()||storage_->provider->revision()!=revision)
        return fail(d,E::source,0x132820);
    file.ipumImages=images;return true;
}
// Same resolver's eight source88..130 slots. This is native CPU storage,
// not guest addresses or an emulated allocator. No file reader currently borrows it.
struct OriginalStandardTextureResolver50LA::Scratch {
    static constexpr std::size_t bytes=0x6000,alignment=64;
    struct Slot {
        std::uint8_t* pointer00=nullptr;
        std::int64_t status08=-2; //12C920/944: actual constructor store.
        std::int32_t offset10=-0x6000; //12C924/958.
    };
    Scratch()=default;
    Scratch(const Scratch&)=delete;Scratch& operator=(const Scratch&)=delete;
    std::array<Slot,8> slots;
    std::int32_t selected148=-1;
    bool initialAllocationReached=false;
    static bool allocate(Slot& slot)noexcept {
        slot.pointer00=static_cast<std::uint8_t*>(
            ::operator new(bytes,std::align_val_t(alignment),std::nothrow));
        slot.status08=slot.pointer00?-1:-2;
        slot.offset10=-0x6000;
        return slot.pointer00!=nullptr;
    }
    ~Scratch(){
        // Exact scratch-only12CB4C..84 suffix; this does not stand in for
        // resolver12CA10 file/list closure. No scratch borrowers exist yet.
        for(auto& slot:slots){
            if(slot.pointer00)::operator delete(slot.pointer00,std::align_val_t(alignment));
            slot.pointer00=nullptr;slot.status08=-2;slot.offset10=-0x6000;
        }
    }
};
bool OriginalStandardTextureResolver50LA::initializeScratch12CDF0(D& d)noexcept {
    d={};
    if(scratch_)return fail(d,E::phase,0x12CDF0);
    try {scratch_=std::make_unique<Scratch>();}
    catch(...){return fail(d,E::allocation,0x12C928);}
    //12CDF0..CE3C attempts every allocation, retaining partial success.
    std::size_t successful=0;
    for(std::size_t i=0;i<scratch_->slots.size();++i)
        if(Scratch::allocate(scratch_->slots[i])){
            ++successful;scratch_->selected148=static_cast<std::int32_t>(i);
        }
    scratch_->initialAllocationReached=true;d.sourceEffectsCommitted=true;
    //12CE40..68 requires at least two. This is only the allocation suffix,
    // not a receipt for12CBA0's separate file/index initialization.
    return successful>=2?true:fail(d,E::allocation,0x12CE48);
}
bool OriginalStandardTextureResolver50LA::restoreScratch132BD0(D& d)noexcept {
    d={};if(!scratch_||!scratch_->initialAllocationReached)return fail(d,E::phase,0x132BD0);
    // Original assertions require null pointer for the missing(-2) case.
    // Reject corrupt ownership before mutating or replacing any live buffer.
    for(const auto& slot:scratch_->slots)
        if((slot.status08==-2&&slot.pointer00)||
           (slot.status08!=-2&&!slot.pointer00))return fail(d,E::phase,0x132C28);
    bool allocated=true;
    for(auto& slot:scratch_->slots){
        if(slot.status08!=-2)continue; //132BF0: preserve live slot and offset.
        d.sourceEffectsCommitted=true;
        if(!Scratch::allocate(slot))allocated=false;
    }
    //132BD0 leaves selected148 unchanged. Failures preserve every completed
    // allocation and each failed slot's authentic -2/negative-size state.
    return allocated?true:fail(d,E::allocation,0x132C74);
}
OriginalStandardTextureResolver50LA::OriginalStandardTextureResolver50LA() noexcept=default;
OriginalStandardTextureResolver50LA::~OriginalStandardTextureResolver50LA() {
    //Every cached live resource pins this resolver. Only cleared associations
    //can remain when the final resolver owner goes away.
    if(storage_)for(const auto& file:storage_->files)
        if(file->cacheIdentity||!file->cached.expired()||file->animation||
           file->geometryIdentity||!file->geometry.expired()||
           file->collisionIdentity||!file->collision.expired())
            std::terminate();
}
bool OriginalStandardTextureResolver50LA::bindGarageStaticIpumCache24E880(
    const std::shared_ptr<const OriginalUiIpumTextureCache50KG>& cache,
    const std::shared_ptr<const OriginalUiArchive50KG>& archive,D& d) noexcept {
    d={};
    if(!storage_||!storage_->provider||!storage_->provider->sealed()||!cache||!cache->loaded()||
       !archive||!archive->loaded()||!archive->identity())return fail(d,E::phase,0x24E880);
    try {
        auto next=storage_->garageIpum;bool found=false;
        const auto count=OriginalUiIpumTextureCache50KG::garageCatalogSourceCount();
        if(!count||count>11)return fail(d,E::source,0x24E880);
        for(std::size_t i=0;i<count;++i){
            if(OriginalUiIpumTextureCache50KG::garageSourceArchive(i)!=archive->identity())continue;
            found=true;const auto* name=OriginalUiIpumTextureCache50KG::garageSourceMember(i);
            if(!name)return fail(d,E::source,0x24E880);
            const auto member=archive->find(name);
            const auto* texture=cache->textureFor(member,0,member.size);
            const auto* provenance=cache->provenanceFor(member,0,member.size);
            OriginalStandardArchiveProvider50LA::MemberView actual;
            OriginalStandardArchiveProvider50LA::MountView mount;
            if(!member.identity||!member.data||!texture||!provenance||!provenance->decodedInProcess||
               provenance->colorPolicyKind!=OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1||
               provenance->sourcePstSha256!=member.identity->sha256||
               provenance->sourceArchiveSha256!=archive->identity()->sha256||
               texture->revision!=1||texture->flags||texture->kind!=2||texture->mips.empty()||
               texture->mips.size()>5||provenance->mipCount!=texture->mips.size()||
               storage_->provider->lookup132820(name,actual)!=OriginalStandardArchiveProvider50LA::Lookup::present||
               !storage_->provider->mount(actual.mount,mount)||!mount.archiveSha256||
               *mount.archiveSha256!=archive->identity()->sha256||mount.archiveBytes!=archive->identity()->bytes||
               !actual.bankGeneration||actual.revision!=storage_->provider->revision()||!actual.data||
               actual.size!=member.size||std::memcmp(actual.data,member.data,member.size))
                return fail(d,E::source,0x24E880);
            bool retained=false;
            for(const auto& old:next)if(old&&old->archive->identity()==archive->identity()&&
                same(old->provenance->sourceMember,provenance->sourceMember)){
                if(!old->current()||old->archive!=archive||old->cache!=cache||old->member.identity!=member.identity||
                   old->member.data!=member.data||old->texture!=texture||old->provenance!=provenance)
                    return fail(d,E::phase,0x24E880);
                retained=true;break;
            }
            if(retained)continue;
            if(next.size()>=11)return fail(d,E::range,0x24E880);
            auto lease=std::shared_ptr<OriginalStandardGarageIpumLease50KG>(new OriginalStandardGarageIpumLease50KG);
            lease->cache=cache;lease->archive=archive;lease->member=member;
            lease->texture=texture;lease->provenance=provenance;next.push_back(std::move(lease));
        }
        if(!found)return fail(d,E::source,0x24E880);
        storage_->garageIpum=std::move(next);return true;
    }catch(...){return fail(d,E::allocation,0x24E880);}
}
bool OriginalStandardTextureResolver50LA::prepareFresh(
    const std::vector<OriginalStandardTextureSourceInput50LA>& inputs,D& d) noexcept {
    d={};
    if(storage_&&storage_->texturesPrepared)return fail(d,E::alreadyPrepared,0x132820);
    if(inputs.empty())return fail(d,E::input,0x132820);
    return appendOriginalSources(inputs,d);
}
bool OriginalStandardTextureResolver50LA::appendOriginalSources(
    const std::vector<OriginalStandardTextureSourceInput50LA>& inputs,D& d) noexcept {
    return appendSources(inputs,false,d);
}
bool OriginalStandardTextureResolver50LA::appendRaTgaVerified(
    const std::shared_ptr<const OriginalStandardArchiveProvider50LA>& provider,
    const std::vector<OriginalStandardTextureSourceInput50LA>& inputs,D& d) noexcept {
    d={};
    if(!storage_||!provider||storage_->provider!=provider||!provider->sealed()||inputs.size()!=3)
        return fail(d,E::source,0x132820);
    // The sole private friend has already hashed these actual immutable bytes
    // against the three compiled source pins. Recheck the actual mounted owner.
    for(const auto& input:inputs){
        OriginalStandardArchiveProvider50LA::MemberView member;
        OriginalStandardArchiveProvider50LA::MountView mount;
        if(provider->lookup132820(input.member,member)!=OriginalStandardArchiveProvider50LA::Lookup::present||
           !provider->mount(member.mount,mount)||!mount.archiveSha256||
           *mount.archiveSha256!="1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2"||
           mount.archiveBytes!=4604561||mount.memberCount!=1010||!member.data||
           member.size!=input.bytes.size()||!std::equal(input.bytes.begin(),input.bytes.end(),member.data))
            return fail(d,E::source,0x132820);
    }
    return appendSources(inputs,true,d);
}
bool OriginalStandardTextureResolver50LA::appendSources(
    const std::vector<OriginalStandardTextureSourceInput50LA>& inputs,bool verifiedRaTga,D& d) noexcept {
    d={};
    if(inputs.empty())return prepared()?true:fail(d,E::input,0x132820);
    if(inputs.size()>32768)return fail(d,E::range,0x132820);
    try {
        auto next=std::make_unique<Storage>();next->entries.reserve(inputs.size());
        std::size_t totalBytes=storage_?storage_->textureBytes:0;
        for(std::size_t ordinal=0;ordinal<inputs.size();++ordinal){
            d.source=ordinal;const auto& source=inputs[ordinal];const auto& bytes=source.bytes;
            std::shared_ptr<const OriginalStandardGarageIpumLease50KG> garageImage;
            const bool tga=tgaName(source.member);
            const auto n=source.member.size();
            const bool atm=nameDomain(source.member)&&n>=4&&
                same(source.member.substr(n-4),".atm");
            if(!tga&&!atm&&!pstName(source.member))return fail(d,E::input,0x24DADC);
            const auto fileKind=atm?Storage::FileKind::atm:
                tga?Storage::FileKind::tga:Storage::FileKind::pst;
            bool duplicate=false;
            for(const auto& file:next->files)if(!file->retired&&same(file->member,source.member)){
                if(file->kind!=fileKind||file->bytes!=bytes)return fail(d,E::duplicateSource,0x132820);
                duplicate=true;break;
            }
            if(storage_)for(const auto& file:storage_->files)if(!file->retired&&same(file->member,source.member)){
                if(file->kind!=fileKind||file->bytes!=bytes)return fail(d,E::duplicateSource,0x132820);
                duplicate=true;break;
            }
            if(duplicate)continue;
            if(next->files.size()+(storage_?storage_->files.size():0)>=32768)
                return fail(d,E::range,0x132820);
            //Bounds are host admission, not an original missing-file result.
            if(bytes.size()>8u*1024u*1024u||bytes.size()>256u*1024u*1024u-totalBytes)
                return fail(d,E::range,0x24E880);
            totalBytes+=bytes.size();
            Storage::Entry entry;
            if(tga){
                if(verifiedRaTga){
                    const bool flag=same(source.member,"textures\\LargeFlag.tga");
                    const bool grate=same(source.member,"textures\\MetalGrate.tga");
                    const bool design=same(source.member,"textures\\TwiDesigns02.tga");
                    if(!flag&&!grate&&!design)return fail(d,E::unsupported,0x29F538);
                    const std::size_t width=grate?256:128;
                    const std::uint8_t depth=flag?32:24;
                    if(!detail::OriginalTgaConversion50LA::sourceShape(bytes,width,width,depth))
                        return fail(d,E::unsupported,0x29F648,12);
                    entry.count=grate?5:4;
                    entry.levels[0]={static_cast<std::int32_t>(width),static_cast<std::int32_t>(width),0,18,width*width*(depth/8)};
                }else {
                    // Preserve the original public specmap5 structural subset.
                    if(bytes.size()!=12332)return fail(d,E::unsupported,0x29F538);
                    if(bytes[0]||bytes[1]||bytes[2]!=2)return fail(d,E::unsupported,0x29F700);
                    for(std::size_t i=3;i<12;++i)if(bytes[i])return fail(d,E::unsupported,0x29F59C,i);
                    if(bytes[12]!=64||bytes[13]||bytes[14]!=64||bytes[15]||bytes[16]!=24||bytes[17])
                        return fail(d,E::unsupported,0x29F648,12);
                    entry.count=3;
                    entry.levels[0]={64,64,0,18,64u*64u*3u};
                }
            }else {
            const auto parsePst=[&](Storage::Entry& frame,std::size_t begin,std::size_t extent) {
                if(!range(bytes,begin,extent)||extent<12)return fail(d,E::malformed,0x24E8B8,begin);
                if(u32(bytes,begin)!=1)return fail(d,E::unsupported,0x24E938,begin);
                const auto kind=u32(bytes,begin+4),count=u32(bytes,begin+8);
                if(kind==2&&!atm&&begin==0&&extent==bytes.size()){
                    OriginalStandardArchiveProvider50LA::MemberView selected;
                    OriginalStandardArchiveProvider50LA::MountView mount;
                    if(!storage_||!storage_->provider||
                       storage_->provider->lookup132820(source.member,selected)!=OriginalStandardArchiveProvider50LA::Lookup::present||
                       !storage_->provider->mount(selected.mount,mount)||!mount.archiveSha256)
                        return fail(d,E::source,0x24E880);
                    for(const auto& lease:storage_->garageIpum)
                        if(lease&&lease->matches(source.member,bytes)&&
                           *mount.archiveSha256==lease->archive->identity()->sha256){garageImage=lease;break;}
                    if(!garageImage||count<1||count>5||count!=garageImage->texture->mips.size())
                        return fail(d,E::unsupported,0x24E994,begin+4);
                    frame.count=static_cast<std::int32_t>(count);frame.kind=2;
                    frame.pstOffset=begin;frame.pstBytes=extent;
                    std::size_t at=12;std::uint32_t priorWidth=0,priorHeight=0;
                    for(std::size_t i=0;i<count;++i){
                        const auto& mip=garageImage->texture->mips[i];
                        const auto& proof=garageImage->provenance->mips[i];
                        if(!range(bytes,at,20))return fail(d,E::malformed,0x24EB20,at);
                        const auto height=u32(bytes,at),width=u32(bytes,at+4),size=u32(bytes,at+16);
                        if(!dimension(width)||!dimension(height)||u32(bytes,at+8)||u32(bytes,at+12)||
                           (i&&(width!=priorWidth*2||height!=priorHeight*2))||size<25||!range(bytes,at+20,size)||
                           mip.width!=width||mip.height!=height||mip.source_header_offset!=at||
                           mip.source_indices_offset!=at+20||mip.rgba.size()!=std::size_t(width)*height*4||
                           proof.width!=width||proof.height!=height||proof.sourceHeaderOffset!=at||
                           proof.sourcePayloadOffset!=at+20||proof.sourcePayloadBytes!=size)
                            return fail(d,E::source,0x24F158,at);
                        // The cache authenticated the exact original IPUM payload and
                        // native RGB5/TEXA255/AEM1 output before this source admission.
                        frame.levels[i]={static_cast<std::int32_t>(width),static_cast<std::int32_t>(height),at,at+20,size};
                        at+=20+size;priorWidth=width;priorHeight=height;
                    }
                    return at==bytes.size()?true:fail(d,E::malformed,0x24EBD0,at);
                }
                if(kind>1)return fail(d,E::unsupported,0x24E994,begin+4);
                if(count<1||count>5)return fail(d,E::unsupported,0x24EB10,begin+8);
                const std::size_t end=begin+extent,paletteBytes=kind==0?64:1024;
                if(paletteBytes>end-begin-12)return fail(d,E::malformed,0x24E994,begin+12);
                frame.count=static_cast<std::int32_t>(count);frame.kind=kind;
                frame.pstOffset=begin;frame.pstBytes=extent;
                std::size_t at=begin+12+paletteBytes;
                std::uint32_t previousWidth=0,previousHeight=0;
                for(std::uint32_t i=0;i<count;++i){
                    if(at>end||end-at<16)return fail(d,E::malformed,0x24EB20,at);
                    const auto height=u32(bytes,at),width=u32(bytes,at+4);
                    if(!dimension(width)||!dimension(height))return fail(d,E::unsupported,0x250580,at);
                    if(u32(bytes,at+8)||u32(bytes,at+12))return fail(d,E::unsupported,0x24EB20,at+8);
                    if(i&&(width!=previousWidth*2||height!=previousHeight*2))
                        return fail(d,E::malformed,0x24EAD4,at);
                    const auto texels=std::size_t(width)*height,packed=kind==0?texels/2:texels;
                    if(packed>end-at-16)return fail(d,E::malformed,kind==0?0x24EDE4:0x24EE38,at+16);
                    frame.levels[i]={static_cast<std::int32_t>(width),static_cast<std::int32_t>(height),at,at+16,packed};
                    at+=16+packed;previousWidth=width;previousHeight=height;
                }
                return at==end?true:fail(d,E::malformed,0x24EBD0,at);
            };
            if(atm&&range(bytes,0,24)&&u32(bytes,16)==2){
                // Separate kind2-only ATM source framing. This does not make
                // a canonical texture constructible without an image bridge.
                std::vector<detail::OriginalIpumFrameExtent50LA> frames;
                std::size_t bad=0;
                if(!detail::frameOriginalIpumAtm50LA(bytes,frames,bad))
                    return fail(d,E::malformed,0x24F158,bad);
                entry.kind=2;entry.frames.reserve(frames.size());
                for(const auto& f:frames){
                    Storage::Entry frame;frame.kind=2;
                    frame.count=static_cast<std::int32_t>(f.count);
                    frame.pstOffset=f.offset;frame.pstBytes=f.bytes;
                    for(std::size_t i=0;i<f.count;++i){const auto& m=f.mips[i];
                        frame.levels[i]={static_cast<std::int32_t>(m.width),
                            static_cast<std::int32_t>(m.height),m.header,m.payload,m.bytes};
                    }
                    entry.frames.push_back(std::move(frame));
                }
            }else if(atm){
                OriginalUiAtm50KG parsed;DecodeError48C error;
                if(!splitOriginalUiAtm50KG(bytes.data(),bytes.size(),parsed,error))
                    return fail(d,E::unsupported,0x102230,error.offset);
                entry.frames.reserve(parsed.frames.size());
                for(const auto& extent:parsed.frames){
                    Storage::Entry frame;
                    if(!parsePst(frame,extent.pstOffset,extent.pstBytes))return false;
                    //10239C/1023A0 passes loader flags0. Retain/validate all
                    //authored indexed mip extents;24EADC..24EB84 skips every
                    //level except the last stored mip during construction.
                    entry.frames.push_back(std::move(frame));
                }
            }else if(!parsePst(entry,0,bytes.size()))return false;
            }
            auto file=std::make_unique<Storage::File>();
            file->member=source.member;file->bytes=bytes;file->kind=fileKind;file->garageIpum=garageImage;
            file->format=atm?OriginalStandardTextureSourceFormat50LA::atmPstFrames:
                tga?(bytes[16]==32?OriginalStandardTextureSourceFormat50LA::tgaBgra32:
                    OriginalStandardTextureSourceFormat50LA::tgaBgr24):
                garageImage?OriginalStandardTextureSourceFormat50LA::pstIpum:OriginalStandardTextureSourceFormat50LA::pstIndexed;
            if(storage_&&storage_->provider){
                OriginalStandardArchiveProvider50LA::MemberView view;
                if(Storage::matchBank(*file,*storage_->provider,view))Storage::ownBank(*file,view);
            }
            entry.file=file.get();
            for(auto& frame:entry.frames)frame.file=file.get();
            next->files.push_back(std::move(file));
            next->entries.push_back(std::move(entry));
        }
        if(storage_){
            //Reserve every throwing append before changing logical rows. All
            //existing model/texture IDs and live associations remain stable.
            storage_->files.reserve(storage_->files.size()+next->files.size());
            storage_->entries.reserve(storage_->entries.size()+next->entries.size());
            for(auto& file:next->files)storage_->files.push_back(std::move(file));
            for(auto& entry:next->entries)storage_->entries.push_back(std::move(entry));
            storage_->textureBytes=totalBytes;
            storage_->texturesPrepared=true;
        }else {next->textureBytes=totalBytes;next->texturesPrepared=true;storage_=std::move(next);}
        d={};return true;
    }catch(...){return fail(d,E::allocation,0x24E880);}
}
bool OriginalStandardTextureResolver50LA::prepared() const noexcept {return storage_&&storage_->texturesPrepared;}
std::size_t OriginalStandardTextureResolver50LA::sourceCount() const noexcept {
    return storage_?storage_->entries.size():0;
}
const std::string* OriginalStandardTextureResolver50LA::member(std::size_t i) const noexcept {
    return storage_&&i<storage_->entries.size()?&storage_->entries[i].file->member:nullptr;
}
const std::vector<std::uint8_t>* OriginalStandardTextureResolver50LA::originalBytes(std::size_t i) const noexcept {
    return storage_&&i<storage_->entries.size()?&storage_->entries[i].file->bytes:nullptr;
}
const OriginalStandardTextureSourceFormat50LA* OriginalStandardTextureResolver50LA::sourceFormat(std::size_t i) const noexcept {
    return storage_&&i<storage_->entries.size()?&storage_->entries[i].file->format:nullptr;
}
bool OriginalStandardTextureResolver50LA::bindProvider3E1A70(
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider,D& d) noexcept {
    d={};
    if(!provider||!provider->sealed())return fail(d,E::unresolved,0x132820);
    if(storage_&&storage_->provider&&storage_->provider!=provider)return fail(d,E::phase,0x132820);
    try {
        if(!storage_)storage_=std::make_unique<Storage>();
        storage_->provider=std::move(provider);
        // Ordinary provider binding keeps its original acceptance behavior.
        // Capture only genuine positive provenance; absence/ambiguity here
        // does not become a new global source-admission restriction.
        for(auto& file:storage_->files)if(!file->retired&&!file->bankGeneration){
            OriginalStandardArchiveProvider50LA::MemberView view;
            if(Storage::matchBank(*file,*storage_->provider,view))Storage::ownBank(*file,view);
        }
        return validateProviderCache(d);
    }catch(...){return fail(d,E::allocation,0x132820);}
}
bool OriginalStandardTextureResolver50LA::issueGarageBank12E5E0(
    const std::shared_ptr<OriginalStandardArchiveProvider50LA>& provider,
    const std::shared_ptr<const OriginalUiArchive50KG>& archive,GarageBankLease50LA& lease,D& d) noexcept {
    return issueGarageBank(provider,archive,lease,false,d);
}
bool OriginalStandardTextureResolver50LA::issueGarageSelectedBank1D1D68(
    const std::shared_ptr<OriginalStandardArchiveProvider50LA>& provider,
    const std::shared_ptr<const OriginalUiArchive50KG>& archive,GarageBankLease50LA& lease,D& d) noexcept {
    return issueGarageBank(provider,archive,lease,true,d);
}
bool OriginalStandardTextureResolver50LA::issueGarageBank(
    const std::shared_ptr<OriginalStandardArchiveProvider50LA>& provider,
    const std::shared_ptr<const OriginalUiArchive50KG>& archive,GarageBankLease50LA& lease,
    bool selected19E,D& d) noexcept {
    d={};
    const auto expected=selected19E?OriginalUiArchiveId50KG::garagePod19:OriginalUiArchiveId50KG::garage;
    if(lease.resolver_||!storage_||!provider||storage_->provider!=provider||!provider->sealed_||
       !provider->retainedSourcesAccessible()||!archive||!archive->loaded()||!archive->identity()||
       archive->identity()->id!=expected)
        return fail(d,E::phase,0x12E5E0);
    std::size_t bank=static_cast<std::size_t>(-1);
    for(std::size_t i=0;i<provider->mounts_.size();++i){const auto& mount=*provider->mounts_[i];
        if(!mount.active||mount.admittedUi.lock()!=archive)continue;
        if(bank!=static_cast<std::size_t>(-1))return fail(d,E::source,0x12E5E0);
        bank=i;
    }
    if(bank==static_cast<std::size_t>(-1))return fail(d,E::unresolved,0x12E5E0);
    if(!validateGarageBankRows12EAA0(bank,provider->mounts_[bank]->generation,d))return false;
    lease.closedBacking_={}; // A new real capability cannot reuse old closure history.
    lease.provider_=provider;lease.archive_=archive;lease.managerIdentity_=storage_->managerIdentity;
    lease.resolver_=this;lease.mount_=bank;
    lease.generation_=provider->mounts_[bank]->generation;lease.selected19E_=selected19E;
    lease.thread_=std::this_thread::get_id();
    return true;
}
bool OriginalStandardTextureResolver50LA::canRetireGarageBank12E5E0(
    const GarageBankLease50LA& lease,D& d) const noexcept {
    d={};const auto& provider=lease.provider_;
    if(lease.resolver_!=this||lease.thread_!=std::this_thread::get_id()||!storage_||!provider||
       lease.managerIdentity_!=storage_->managerIdentity||storage_->provider!=provider||
       !provider->sealed_||!provider->retainedSourcesAccessible()||
       lease.mount_>=provider->mounts_.size()||
       provider->revision_==std::numeric_limits<std::uint64_t>::max())
        return fail(d,E::phase,0x12E5E0);
    const auto& bank=*provider->mounts_[lease.mount_];
    if(!bank.active||bank.generation!=lease.generation_||bank.admittedUi.lock()!=lease.archive_||
       !lease.archive_||!bank.ui||!bank.ui->identity()||
       bank.ui->identity()->id!=(lease.selected19E_?OriginalUiArchiveId50KG::garagePod19:
            OriginalUiArchiveId50KG::garage))return fail(d,E::phase,0x12E6D0);
    return validateGarageBankRows12EAA0(lease.mount_,lease.generation_,d);
}
bool OriginalStandardTextureResolver50LA::validateGarageBankRows12EAA0(
    std::size_t mountIndex,std::uint64_t generation,D& d) const noexcept {
    if(!storage_||!storage_->provider||mountIndex>=storage_->provider->mounts_.size())
        return fail(d,E::phase,0x12EAA0);
    const auto& provider=*storage_->provider;
    const auto& bank=*provider.mounts_[mountIndex];
    if(!bank.active||bank.generation!=generation)return fail(d,E::phase,0x12EAA0);
    for(const auto& file:storage_->files){
        if(file->retired)continue;
        bool possibleTarget=false;
        for(const auto& row:bank.rows)if(same(row.name,file->member)){possibleTarget=true;break;}
        // The complete authenticated directory proves nonmembers unrelated.
        // A matching name is only a reason to demand provenance, never proof
        // of ownership. Do not silently omit an unproven possible target row.
        if(!possibleTarget&&file->bank!=mountIndex)continue;
        if(!file->bankGeneration||file->bank>=provider.mounts_.size())
            return fail(d,E::source,0x12EAA0);
        const auto& owner=*provider.mounts_[file->bank];
        if(!owner.active||owner.generation!=file->bankGeneration||
           file->bankOrdinal>=owner.rows.size())return fail(d,E::source,0x12EAA0);
        const auto& row=owner.rows[file->bankOrdinal];
        if(!same(row.name,file->member)||row.size!=file->bytes.size()||
           !std::equal(file->bytes.begin(),file->bytes.end(),row.data))return fail(d,E::source,0x12EAA0);
        if(file->bank==mountIndex&&(!possibleTarget||file->bankGeneration!=generation))
            return fail(d,E::source,0x12EAA0);
        // Proven equal-name rows owned by a different active bank stay intact.
    }
    return true;
}
bool OriginalStandardTextureResolver50LA::retireGarageBank12E5E0(GarageBankLease50LA& lease,D& d) noexcept {
    if(!canRetireGarageBank12E5E0(lease,d))return false;
    //12E6D0(force1)->12EAA0 removes row associations even while canonical
    //resources survive. Never release texture/material/geometry/animation refs.
    //Stable native rows/bytes remain tombstoned for outstanding resource views;
    //new generation admission appends fresh rows rather than reviving old IDs.
    for(auto& file:storage_->files){
        if(file->retired||file->bank!=lease.mount_||file->bankGeneration!=lease.generation_)continue;
        file->cached.reset();file->cacheIdentity=nullptr;file->animation=nullptr;
        file->geometry.reset();file->geometryIdentity=nullptr;
        file->collision.reset();file->collisionIdentity=nullptr;file->ipumImages.reset();
        file->retired=true;
    }
    //12EB60: remove precisely this bank from all new lookup/encoded-reader
    //issuance. Native immutable retained views are not active manager readers.
    lease.provider_->mounts_[lease.mount_]->active=false;
    lease.provider_->mounts_[lease.mount_]->admittedUi.reset();++lease.provider_->revision_;
    // Native history only, AFTER the actual bank close. Do not free even a
    // released resource's backing in this source association-removal method.
    lease.closedBacking_.provider=lease.provider_;
    lease.closedBacking_.managerIdentity=lease.managerIdentity_;
    lease.closedBacking_.resolver=this;lease.closedBacking_.mount=lease.mount_;
    lease.closedBacking_.generation=lease.generation_;
    lease.closedBacking_.selected19E=lease.selected19E_;lease.closedBacking_.thread=lease.thread_;
    lease.resolver_=nullptr;lease.generation_=0;lease.archive_.reset();lease.provider_.reset();lease.managerIdentity_.reset();
    d={};d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::bindModelMember(const std::string& name,
    OriginalStandardModelFileKind50LA kind,const std::vector<std::uint8_t>& bytes,
    OriginalStandardModelFileId50LA& out,D& d) noexcept {
    d={};out={};
    const char* extension=kind==OriginalStandardModelFileKind50LA::mcf?".mcf":
        kind==OriginalStandardModelFileKind50LA::psg?".psg":
        kind==OriginalStandardModelFileKind50LA::colType0?".col":nullptr;
    if(!extension||name.empty()||name.size()>259||name.size()<4||bytes.empty())
        return fail(d,E::input,0x132820);
    for(unsigned char c:name)if(c<32||c>=127)return fail(d,E::input,0x132820);
    for(std::size_t i=0;i<4;++i)
        if(fold(static_cast<unsigned char>(name[name.size()-4+i]))!=
           static_cast<unsigned char>(extension[i]))return fail(d,E::input,0x132820);
    if(!storage_){
        try {storage_=std::make_unique<Storage>();}
        catch(...){return fail(d,E::allocation,0x132820);}
    }
    if(kind==OriginalStandardModelFileKind50LA::colType0){
        using Provider=OriginalStandardArchiveProvider50LA;Provider::MemberView actual;
        if(!storage_->provider||!storage_->provider->sealed()||
           storage_->provider->lookup132820(name,actual)!=Provider::Lookup::present)
            return fail(d,E::unresolved,0x132820);
        if(!actual.member||!same(name,*actual.member)||!actual.data||actual.size!=bytes.size()||
           actual.revision!=storage_->provider->revision()||!std::equal(bytes.begin(),bytes.end(),actual.data))
            return fail(d,E::source,0x132820);
    }
    const auto fileKind=kind==OriginalStandardModelFileKind50LA::mcf?
        Storage::FileKind::mcf:kind==OriginalStandardModelFileKind50LA::psg?
        Storage::FileKind::psg:Storage::FileKind::colType0;
    for(std::size_t i=0;i<storage_->files.size();++i){const auto& file=*storage_->files[i];
        if(file.retired||!same(file.member,name))continue;
        if(file.kind!=fileKind||file.bytes!=bytes)return fail(d,E::duplicateSource,0x132820);
        out={this,i};d.source=i;return true;
    }
    if(storage_->files.size()>=32768||bytes.size()>8u*1024u*1024u||
       bytes.size()>256u*1024u*1024u-storage_->modelBytes)
        return fail(d,E::range,0x132820);
    try {
        auto file=std::make_unique<Storage::File>();
        file->member=name;file->bytes=bytes;file->kind=fileKind;
        if(storage_->provider){
            OriginalStandardArchiveProvider50LA::MemberView view;
            if(Storage::matchBank(*file,*storage_->provider,view))Storage::ownBank(*file,view);
        }
        const auto row=storage_->files.size();
        storage_->files.push_back(std::move(file));storage_->modelBytes+=bytes.size();
        out={this,row};d.source=row;return true;
    }catch(...){return fail(d,E::allocation,0x132820);}
}
bool OriginalStandardTextureResolver50LA::modelFile(OriginalStandardModelFileId50LA id,
    OriginalStandardModelFileView50LA& out,D& d) const noexcept {
    d={};out={};
    if(!storage_)return fail(d,E::unprepared,0x132820);
    if(id.resolver!=this||id.row>=storage_->files.size())return fail(d,E::range,0x1329A0);
    const auto& file=*storage_->files[id.row];d.source=id.row;
    // Direct stable-ID inspection remains available for already-owned resource
    // release preflights. Retired rows expose bytes but no cache associations
    // until proved-final PSG/MCF backing is reclaimed. The tombstone ID
    // survives reclamation but cannot become a new positive file view.
    if(file.retired&&(file.kind==Storage::FileKind::psg||file.kind==Storage::FileKind::mcf)&&file.bytes.empty())
        return fail(d,E::unresolved,0x1329A0);
    if((file.kind!=Storage::FileKind::mcf&&file.kind!=Storage::FileKind::psg&&file.kind!=Storage::FileKind::colType0)||file.cacheIdentity||!file.cached.expired())
        return fail(d,E::phase,0x1329A0);
    if(!file.retired&&file.kind==Storage::FileKind::colType0){
        using Provider=OriginalStandardArchiveProvider50LA;Provider::MemberView actual;
        if(!storage_->provider||!storage_->provider->sealed()||
           storage_->provider->lookup132820(file.member,actual)!=Provider::Lookup::present)
            return fail(d,E::unresolved,0x132820);
        if(!actual.member||!same(file.member,*actual.member)||!actual.data||actual.size!=file.bytes.size()||
           actual.revision!=storage_->provider->revision()||!std::equal(file.bytes.begin(),file.bytes.end(),actual.data))
            return fail(d,E::source,0x132820);
    }
    const auto geometry=file.geometry.lock();
    const auto collision=file.collision.lock();
    if(file.collisionIdentity!=collision.get()||
       (file.kind!=Storage::FileKind::colType0&&collision)||
       (file.kind==Storage::FileKind::colType0&&(geometry||file.animation)))return fail(d,E::phase,0x1329A0);
    if(collision&&(!collision->live()||collision->fileId().resolver!=this||
       collision->fileId().row!=id.row))return fail(d,E::phase,0x1329A0);
    if(file.geometryIdentity!=geometry.get())return fail(d,E::phase,0x1329A0);
    if((file.kind==Storage::FileKind::mcf&&geometry)||
       (file.kind==Storage::FileKind::psg&&file.animation))return fail(d,E::phase,0x1329A0);
    if(geometry&&(!geometry->constructed240110()||
       geometry->fileId().resolver!=this||geometry->fileId().row!=id.row))
        return fail(d,E::phase,0x1329A0);
    out.id=id;out.kind=file.kind==Storage::FileKind::mcf?
        OriginalStandardModelFileKind50LA::mcf:file.kind==Storage::FileKind::psg?
        OriginalStandardModelFileKind50LA::psg:OriginalStandardModelFileKind50LA::colType0;
    out.member=&file.member;out.bytes=&file.bytes;
    out.animation1536F0=file.animation;out.geometry240110=geometry;out.collision125014=collision;return true;
}
bool OriginalStandardTextureResolver50LA::lookupModel132820(const std::string& name,
    OriginalStandardModelFileKind50LA kind,OriginalStandardModelFileView50LA& out,D& d) const noexcept {
    d={};out={};if(!storage_)return fail(d,E::unprepared,0x132820);
    if(name.empty()||name.size()>259)return fail(d,E::input,0x132820);
    for(unsigned char c:name)if(c<32||c>=127)return fail(d,E::input,0x132820);
    for(std::size_t i=0;i<storage_->files.size();++i)if(!storage_->files[i]->retired&&same(storage_->files[i]->member,name)){
        if(!modelFile({this,i},out,d))return false;
        if(out.kind!=kind){out={};return fail(d,E::phase,0x1329A0);}
        //1329A0 reads the association without retaining it. A null value is
        //the cold, positively resolved file path, not an unknown-file miss.
        return true;
    }
    return fail(d,E::unresolved,0x13294C);
}
OriginalStandardCollisionLookup50LA OriginalStandardTextureResolver50LA::lookupCollision132820(
    const std::string& name,OriginalStandardModelFileView50LA& out,D& d) const noexcept {
    using Result=OriginalStandardCollisionLookup50LA;
    using Provider=OriginalStandardArchiveProvider50LA;
    d={};out={};
    if(name.size()<4||name.size()>259){fail(d,E::input,0x132820);return Result::rejected;}
    for(unsigned char c:name)if(c<32||c>=127){fail(d,E::input,0x132820);return Result::rejected;}
    constexpr char extension[]=".col";
    for(std::size_t i=0;i<4;++i)
        if(fold(static_cast<unsigned char>(name[name.size()-4+i]))!=
           static_cast<unsigned char>(extension[i])){
            fail(d,E::input,0x132820);return Result::rejected;
        }
    if(!storage_||!storage_->provider||!storage_->provider->sealed()){
        fail(d,E::unresolved,0x132820);return Result::rejected;
    }
    Provider::MemberView actual;
    const auto resolution=storage_->provider->lookup132820(name,actual);
    if(resolution==Provider::Lookup::unknown){
        fail(d,E::unresolved,0x132820);return Result::rejected;
    }
    if(resolution==Provider::Lookup::absent){
        // A retained positive row contradicting the current namespace must
        // never be converted into a successful negative-load cleanup path.
        for(const auto& file:storage_->files)if(!file->retired&&same(file->member,name)){
            fail(d,E::phase,0x132820);return Result::rejected;
        }
        d.sourcePC=0x13294c;return Result::absent;
    }
    if(!lookupModel132820(name,OriginalStandardModelFileKind50LA::colType0,out,d))
        return Result::rejected;
    return Result::present;
}
bool OriginalStandardTextureResolver50LA::publishAnimation1326A0(OriginalStandardModelFileId50LA id,
    const rr::game::OriginalRacerAnimationCold50LA* animation,D& d) noexcept {
    OriginalStandardModelFileView50LA file;
    if(!modelFile(id,file,d))return false;
    if(storage_->files[id.row]->retired)return fail(d,E::phase,0x1326A0);
    if(!animation||file.kind!=OriginalStandardModelFileKind50LA::mcf||
       file.animation1536F0||file.geometry240110)return fail(d,E::phase,0x1327B0);
    auto& target=*storage_->files[id.row];
    target.finalAnimationRelease={}; // A later publication cannot reuse history.
    target.animationPublication={};
    target.animationPublication.owner=reinterpret_cast<std::uintptr_t>(animation);
    if(const auto* fields=animation->fields();fields&&fields->self00){
        target.animationPublication.sessionGeneration=fields->self00->sessionGeneration;
        target.animationPublication.objectKey=fields->self00->objectKey;
    }
    target.animation=animation;d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::publishGeometry1326A0(OriginalStandardModelFileId50LA id,
    const std::shared_ptr<OriginalStandardModelGeometry50LA>& geometry,D& d) noexcept {
    OriginalStandardModelFileView50LA file;
    if(!modelFile(id,file,d))return false;
    if(storage_->files[id.row]->retired)return fail(d,E::phase,0x1326A0);
    if(!geometry||!geometry->constructed240110()||geometry->fileId().resolver!=this||
       geometry->fileId().row!=id.row||file.kind!=OriginalStandardModelFileKind50LA::psg||
       file.animation1536F0||file.geometry240110)return fail(d,E::phase,0x1327B0);
    storage_->files[id.row]->finalGeometryRelease={};
    storage_->files[id.row]->geometry=geometry;
    storage_->files[id.row]->payloadGeometry=geometry;
    storage_->files[id.row]->geometryIdentity=geometry.get();d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::publishCollision1326A0(OriginalStandardModelFileId50LA id,
    const std::shared_ptr<OriginalStandardCollisionType0Resource50LA>& resource,D& d) noexcept {
    OriginalStandardModelFileView50LA file;
    if(!modelFile(id,file,d))return false;
    if(storage_->files[id.row]->retired)return fail(d,E::phase,0x1326A0);
    if(!resource||!resource->live()||resource->fileId().resolver!=this||resource->fileId().row!=id.row||
       file.kind!=OriginalStandardModelFileKind50LA::colType0||file.collision125014||
       file.animation1536F0||file.geometry240110)return fail(d,E::phase,0x1327B0);
    storage_->files[id.row]->collision=resource;
    storage_->files[id.row]->collisionIdentity=resource.get();d.sourceEffectsCommitted=true;return true;
}
void OriginalStandardTextureResolver50LA::clearCollision1327D0(
    const OriginalStandardCollisionType0Resource50LA* resource) noexcept {
    if(!storage_||!resource)return;
    //11B650 clears by resource identity BEFORE11AEE0, even if ref60 remains
    //positive. A later cold resource at the same file must survive old users.
    for(auto& file:storage_->files)if(file->collisionIdentity==resource){
        file->collision.reset();file->collisionIdentity=nullptr;return;
    }
}
void OriginalStandardTextureResolver50LA::clearAnimation1327D0(
    const rr::game::OriginalRacerAnimationCold50LA* animation) noexcept {
    if(!storage_||!animation)return;
    for(auto& file:storage_->files)if(file->animation==animation){file->animation=nullptr;return;}
}
void OriginalStandardTextureResolver50LA::clearGeometry1327D0(
    const OriginalStandardModelGeometry50LA* geometry) noexcept {
    if(!storage_||!geometry)return;
    for(auto& file:storage_->files)if(file->geometryIdentity==geometry){
        file->geometry.reset();file->geometryIdentity=nullptr;return;
    }
}
void OriginalStandardTextureResolver50LA::reclaimRetiredPsgBacking26E5C8(
    const OriginalStandardModelGeometry50LA& geometry) noexcept {
    const auto id=geometry.fileId();
    if(!storage_||id.resolver!=this||id.row>=storage_->files.size())return;
    auto& file=*storage_->files[id.row];
    // Only private Garage12E5E0 marks a File retired. An active row may
    // record release history only for its genuine authenticated Garage bank;
    // ordinary Rider/Main/race and every other payload family stay untouched.
    if(file.kind!=Storage::FileKind::psg||file.bytes.empty()||
       file.bank==static_cast<std::size_t>(-1)||!file.bankGeneration)return;
    if(!file.retired){
        if(!storage_->provider||file.bank>=storage_->provider->mounts_.size())return;
        const auto& bank=*storage_->provider->mounts_[file.bank];
        if(!bank.active||bank.generation!=file.bankGeneration||!bank.ui||!bank.ui->identity()||
           (bank.ui->identity()->id!=OriginalUiArchiveId50KG::garage&&
            bank.ui->identity()->id!=OriginalUiArchiveId50KG::garagePod19))return;
    }
    const auto owner=file.payloadGeometry.lock();
    const auto* references=geometry.references00();
    if(owner.get()!=&geometry||!references||*references!=0||geometry.constructed240110()||
       geometry.payload()||file.geometryIdentity||!file.geometry.expired()||
       file.cacheIdentity||!file.cached.expired()||file.animation||
       file.collisionIdentity||!file.collision.expired()||file.garageIpum||
       !file.ipumImages.expired())return;
    const auto bytes=file.bytes.size();
    if(bytes>storage_->modelBytes)return; // Retain on inconsistent accounting.
    file.finalGeometryRelease.bank=file.bank;file.finalGeometryRelease.generation=file.bankGeneration;
    file.finalGeometryRelease.bytes=bytes;file.finalGeometryRelease.publication=file.payloadGeometry;
    if(!file.retired)return; // Genuine final release, but bank still owns lookup backing.
    // Actual no-throw allocation release, not clear() with retained capacity.
    // Stable File/row/generation metadata remains. A new bank must still
    // authenticate and append a fresh row; no old ID is revived or reused.
    std::vector<std::uint8_t>().swap(file.bytes);
    storage_->modelBytes-=bytes;
    file.payloadGeometry.reset();file.finalGeometryRelease={};
}
void OriginalStandardTextureResolver50LA::recordGarageFinalMcfRelease153F50(
    OriginalStandardModelFileId50LA id,const rr::game::OriginalRacerAnimationCold50LA& animation) noexcept {
    if(!storage_||id.resolver!=this||id.row>=storage_->files.size()||!storage_->provider)return;
    auto& file=*storage_->files[id.row];const auto* fields=animation.fields();
    if(file.kind!=Storage::FileKind::mcf||file.bytes.empty()||!file.bankGeneration||
       file.bank>=storage_->provider->mounts_.size()||!animation.released()||!fields||!fields->self00)
        return;
    const auto& entity=*fields->self00;const auto& published=file.animationPublication;
    using K=rr::game::OriginalWreckComponentKind50LA;
    if(published.owner!=reinterpret_cast<std::uintptr_t>(&animation)||!published.sessionGeneration||
       !published.objectKey||entity.sessionGeneration!=published.sessionGeneration||
       entity.objectKey!=published.objectKey||entity.reference00||entity.firstChild18||
       (entity.kind!=K::animation1536F0&&entity.kind!=K::character11A750)||
       fields->dispatch21C!=K::animation1536F0||fields->motion18||fields->count208||
       file.animation||file.cacheIdentity||!file.cached.expired()||file.geometryIdentity||
       !file.geometry.expired()||!file.payloadGeometry.expired()||file.collisionIdentity||
       !file.collision.expired()||file.garageIpum||!file.ipumImages.expired())return;
    const auto& bank=*storage_->provider->mounts_[file.bank];
    if(bank.generation!=file.bankGeneration||bank.active==file.retired||
       !bank.ui||!bank.ui->identity()||bank.ui->identity()->id!=OriginalUiArchiveId50KG::garage||
       file.bankOrdinal>=bank.rows.size()||file.bytes.size()>storage_->modelBytes)return;
    const auto& source=bank.rows[file.bankOrdinal];
    if(!same(source.name,file.member)||source.size!=file.bytes.size()||!source.data||
       !std::equal(file.bytes.begin(),file.bytes.end(),source.data))return;
    // The private caller has just completed the counted last-reference
    // callback. Incomplete callbacks never reach here. No allocation, byte
    // release or source-state mutation occurs, even for an already closed bank.
    file.finalAnimationRelease.bank=file.bank;
    file.finalAnimationRelease.generation=file.bankGeneration;
    file.finalAnimationRelease.bytes=file.bytes.size();
    file.finalAnimationRelease.publication=published;
}
bool OriginalStandardTextureResolver50LA::reclaimGaragePsgBackingAfterManagerRetirement(
    GarageBankLease50LA& lease,D& d) noexcept {
    d={};const auto& receipt=lease.closedBacking_;
    if(!storage_||lease.resolver_||lease.generation_||lease.provider_||lease.archive_||
       receipt.resolver!=this||receipt.thread!=std::this_thread::get_id()||receipt.selected19E||
       !receipt.provider||receipt.provider!=storage_->provider||
       receipt.managerIdentity!=storage_->managerIdentity||!receipt.generation||
       !receipt.provider->sealed_||!receipt.provider->retainedSourcesAccessible()||
       receipt.mount>=receipt.provider->mounts_.size())return fail(d,E::phase,0x12EB60);
    const auto& bank=*receipt.provider->mounts_[receipt.mount];
    if(bank.active||bank.generation!=receipt.generation||!bank.admittedUi.expired()||
       !bank.ui||!bank.ui->identity()||bank.ui->identity()->id!=OriginalUiArchiveId50KG::garage)
        return fail(d,E::phase,0x12EB60);
    // No allocations, source callbacks or mutations during this complete
    // preflight. The exact creator thread cannot reopen/publish midway.
    std::size_t residentBytes=0,releaseBytes=0,residentTextureBytes=0,releasePstBytes=0;
    for(std::size_t index=0;index<storage_->files.size();++index){
        const auto& file=*storage_->files[index];
        if(file.kind==Storage::FileKind::pst||file.kind==Storage::FileKind::tga||file.kind==Storage::FileKind::atm){
            if(file.bytes.size()>std::numeric_limits<std::size_t>::max()-residentTextureBytes)
                return fail(d,E::range,0x1327D0);
            residentTextureBytes+=file.bytes.size();
        }
        if(file.retired&&file.kind==Storage::FileKind::pst&&!file.bytes.empty()&&
           file.bank==receipt.mount&&file.bankGeneration==receipt.generation){
            bool eligible=false;
            if(!preflightRetiredGaragePstBacking(index,true,eligible,d))return false;
            if(eligible){
                if(file.bytes.size()>std::numeric_limits<std::size_t>::max()-releasePstBytes)
                    return fail(d,E::range,0x1327D0);
                releasePstBytes+=file.bytes.size();
            }
        }
        if(file.kind==Storage::FileKind::mcf||file.kind==Storage::FileKind::psg||
           file.kind==Storage::FileKind::colType0){
            if(file.bytes.size()>std::numeric_limits<std::size_t>::max()-residentBytes)
                return fail(d,E::range,0x26E5C8);
            residentBytes+=file.bytes.size();
        }
        if(file.retired&&file.kind==Storage::FileKind::mcf&&!file.bytes.empty()&&
           file.bank==receipt.mount&&file.bankGeneration==receipt.generation){
            const auto& final=file.finalAnimationRelease;
            // Only an explicit final callback, never cache null/weak expiry.
            if(!final.generation)continue;
            d.source=index;const auto& published=file.animationPublication;
            if(final.bank!=file.bank||final.generation!=file.bankGeneration||final.bytes!=file.bytes.size()||
               !final.publication.owner||!final.publication.sessionGeneration||!final.publication.objectKey||
               final.publication.owner!=published.owner||
               final.publication.sessionGeneration!=published.sessionGeneration||
               final.publication.objectKey!=published.objectKey||file.animation||
               file.geometryIdentity||!file.geometry.expired()||!file.payloadGeometry.expired()||
               file.cacheIdentity||!file.cached.expired()||file.collisionIdentity||!file.collision.expired()||
               file.garageIpum||!file.ipumImages.expired()||file.bankOrdinal>=bank.rows.size())
                return fail(d,E::phase,0x1540E0);
            const auto& source=bank.rows[file.bankOrdinal];
            if(!same(source.name,file.member)||source.size!=file.bytes.size()||!source.data||
               !std::equal(file.bytes.begin(),file.bytes.end(),source.data))return fail(d,E::source,0x1329A0);
            if(file.bytes.size()>std::numeric_limits<std::size_t>::max()-releaseBytes)
                return fail(d,E::range,0x1540E0);
            releaseBytes+=file.bytes.size();
            continue;
        }
        if(!file.retired||file.kind!=Storage::FileKind::psg||file.bytes.empty()||
           file.bank!=receipt.mount||file.bankGeneration!=receipt.generation)continue;
        const auto& final=file.finalGeometryRelease;
        // Never-constructed or unproved rows remain retained. Expiration of
        // payloadGeometry is NOT a substitute for this destructor receipt.
        if(!final.generation)continue;
        d.source=index;
        if(final.bank!=file.bank||final.generation!=file.bankGeneration||final.bytes!=file.bytes.size()||
           final.publication.owner_before(file.payloadGeometry)||file.payloadGeometry.owner_before(final.publication)||
           file.geometryIdentity||!file.geometry.expired()||file.cacheIdentity||!file.cached.expired()||
           file.animation||file.collisionIdentity||!file.collision.expired()||file.garageIpum||
           !file.ipumImages.expired()||file.bankOrdinal>=bank.rows.size())
            return fail(d,E::phase,0x26E5C8);
        const auto& source=bank.rows[file.bankOrdinal];
        if(!same(source.name,file.member)||source.size!=file.bytes.size()||!source.data||
           !std::equal(file.bytes.begin(),file.bytes.end(),source.data))return fail(d,E::source,0x1329A0);
        // If the exact native tombstone still exists, require the same actual
        // released state too. Its absence is allowed ONLY by the above proof.
        if(const auto owner=file.payloadGeometry.lock()){
            const auto id=owner->fileId();const auto* refs=owner->references00();
            if(id.resolver!=this||id.row!=index||!refs||*refs||owner->constructed240110()||owner->payload())
                return fail(d,E::phase,0x26E5C8);
        }
        if(file.bytes.size()>std::numeric_limits<std::size_t>::max()-releaseBytes)
            return fail(d,E::range,0x26E5C8);
        releaseBytes+=file.bytes.size();
    }
    if(residentBytes!=storage_->modelBytes||releaseBytes>residentBytes)
        return fail(d,E::phase,0x26E5C8);
    if(residentTextureBytes!=storage_->textureBytes||releasePstBytes>residentTextureBytes)
        return fail(d,E::phase,0x1327D0);
    // All identities and complete accounting are proved before the first
    // irreversible swap. No allocation/throw/source code occurs below.
    for(auto& row:storage_->files){auto& file=*row;
        if(file.retired&&file.kind==Storage::FileKind::pst&&!file.bytes.empty()&&
           file.bank==receipt.mount&&file.bankGeneration==receipt.generation&&!file.pstBorrowers&&
           (!file.pstPublication||file.finalPstRelease.generation)){
            std::vector<std::uint8_t>().swap(file.bytes);file.finalPstRelease={};
            continue;
        }
        if(file.retired&&file.kind==Storage::FileKind::mcf&&!file.bytes.empty()&&
           file.bank==receipt.mount&&file.bankGeneration==receipt.generation&&
           file.finalAnimationRelease.generation){
            std::vector<std::uint8_t>().swap(file.bytes);
            file.animationPublication={};file.finalAnimationRelease={};
            continue;
        }
        if(!file.retired||file.kind!=Storage::FileKind::psg||file.bytes.empty()||
           file.bank!=receipt.mount||file.bankGeneration!=receipt.generation||
           !file.finalGeometryRelease.generation)continue;
        std::vector<std::uint8_t>().swap(file.bytes);
        file.payloadGeometry.reset();file.finalGeometryRelease={};
    }
    storage_->modelBytes-=releaseBytes;storage_->textureBytes-=releasePstBytes;
    lease.closedBacking_={}; // Consumed historical receipt, never a live lease.
    d={};d.sourceEffectsCommitted=releaseBytes!=0||releasePstBytes!=0;return true;
}
bool OriginalStandardTextureResolver50LA::bind(
    const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,D& d) noexcept {
    if(!prepared())return fail(d,E::unprepared,0x132820);
    if(!context||!context->live())return fail(d,E::video,0x29C768);
    //29E340 skips already populated rows. Validate their SAME associations
    //before that skip when native mounts have been extended or resealed.
    if(!validateProviderCache(d))return false;
    if(storage_->bound){
        if(storage_->construction.lock()!=context)return fail(d,E::phase,0x132820);
    }else {storage_->construction=context;storage_->bound=true;}
    return true;
}
bool OriginalStandardTextureResolver50LA::resolve24DADC(
    const std::string& requested,std::size_t& index,std::string& resolvedName,D& d) const noexcept {
    index=static_cast<std::size_t>(-1);
    if(!prepared())return fail(d,E::unprepared,0x132820);
    if(!nameDomain(requested))return fail(d,E::input,0x24DA90);
    try {
        std::string pst=detail::originalPstCandidate24DADC(requested);
        if(!nameDomain(pst))return fail(d,E::range,0x24DF10);
        using Provider=OriginalStandardArchiveProvider50LA;
        const auto& provider=storage_->provider;
        if(provider&&!provider->sealed())return fail(d,E::unresolved,0x132820);
        const auto revision=provider?provider->revision():0;
        const auto resolvePositive=[&](const std::string& name,std::uint32_t pc,
            const Provider::MemberView* view) {
            for(std::size_t i=0;i<storage_->entries.size();++i){
                const auto& entry=storage_->entries[i];
                if(entry.file->retired||!same(name,entry.file->member))continue;
                if(view&&(!view->member||!same(*view->member,entry.file->member)||!view->data||
                   view->size!=entry.file->bytes.size()||view->revision!=revision||
                   !std::equal(entry.file->bytes.begin(),entry.file->bytes.end(),view->data)))
                    return fail(d,E::source,pc);
                resolvedName=name;index=i;d.source=i;return true;
            }
            return fail(d,E::unresolved,pc);
        };
        if(!provider){
            //Positive PST admission remains available to existing callers.
            //No source-vector miss is ever a missing-file/fallback proof.
            return resolvePositive(pst,0x24DAF8,nullptr);
        }
        Provider::MemberView found;
        auto lookup=provider->lookup132820(pst,found);
        if(lookup==Provider::Lookup::present)return resolvePositive(pst,0x24DAF8,&found);
        if(lookup!=Provider::Lookup::absent)return fail(d,E::unresolved,0x24DAF8);
        //24DB10..60 rewrites the same last dot to.ipu. A present IPU must
        //stop here even when an admitted TGA exists; no substitute image.
        const auto dot2=pst.find_last_of('.');
        pst.replace(dot2,std::string::npos,".ipu");
        lookup=provider->lookup132820(pst,found);
        if(lookup==Provider::Lookup::present)return fail(d,E::unsupported,0x24DB60);
        if(lookup!=Provider::Lookup::absent)return fail(d,E::unresolved,0x24DB60);
        pst.replace(dot2,std::string::npos,".tga"); //24DB78..24DBC8.
        lookup=provider->lookup132820(pst,found);
        if(provider->revision()!=revision||!provider->sealed())return fail(d,E::phase,0x132820);
        if(lookup==Provider::Lookup::present)return resolvePositive(pst,0x24DBC8,&found);
        if(lookup!=Provider::Lookup::absent)return fail(d,E::unresolved,0x24DBC8);
        pst.replace(dot2,std::string::npos,".atm");
        lookup=provider->lookup132820(pst,found);
        if(provider->revision()!=revision||!provider->sealed())return fail(d,E::phase,0x132820);
        if(lookup==Provider::Lookup::present)return resolvePositive(pst,0x24DC30,&found);
        return fail(d,E::unresolved,0x24DC30);
    }catch(...){return fail(d,E::allocation,0x24DA90);}
}
bool OriginalStandardTextureResolver50LA::validateProviderCache(D& d) const noexcept {
    if(!storage_||!storage_->provider)return true;
    if(!storage_->provider->sealed())return fail(d,E::unresolved,0x132820);
    for(std::size_t i=0;i<storage_->entries.size();++i){
        const auto& file=*storage_->entries[i].file;
        const auto resource=file.cached.lock();
        if(resource.get()!=file.cacheIdentity)return fail(d,E::phase,0x1329A0);
        if(!resource)continue;
        if(!resource->constructed24DA30())return fail(d,E::phase,0x1329A0);
        std::size_t resolved=static_cast<std::size_t>(-1);std::string name;
        if(!resolve24DADC(resource->name20_,resolved,name,d))return false;
        //A newly mounted prior-priority PST/IPU or conflicting TGA cannot
        //silently retain the old resource's meaning. Existing rows and refs
        //are preserved on failure; the caller must stop this population.
        if(resolved!=i)return fail(d,E::source,0x132820);
    }
    return true;
}
bool OriginalStandardTextureResolver50LA::cached132820(std::size_t i,
    std::shared_ptr<OriginalStandardTextureResource50LA>& out,D& d) noexcept {
    out.reset();
    if(!storage_||i>=storage_->entries.size())return fail(d,E::range,0x1329A0);
    auto& entry=*storage_->entries[i].file;
    if(entry.retired)return fail(d,E::unresolved,0x1329A0);
    out=entry.cached.lock();
    if((entry.cacheIdentity!=out.get())||(out&&!out->constructed24DA30()))
        return fail(d,E::phase,0x1329A0);
    return true;
}
bool OriginalStandardTextureResolver50LA::cache1326A0(std::size_t i,
    const std::shared_ptr<OriginalStandardTextureResource50LA>& resource) noexcept {
    if(!storage_||i>=storage_->entries.size()||!resource||!resource->constructed24DA30()||
       resource->resolver_.get()!=this||resource->sourceIndex_!=i)return false;
    auto& entry=*storage_->entries[i].file;if(entry.retired||entry.cacheIdentity||!entry.cached.expired())return false;
    entry.cached=resource;entry.cacheIdentity=resource.get();
    //TGA source24DFD8 publishes1326A0 before24DFE4 releases29F770's
    //temporary reader image. The final1555 level owners stay untouched.
    std::vector<std::uint8_t>().swap(resource->tgaReaderScratch_);
    return true;
}
void OriginalStandardTextureResolver50LA::clear1327D0(const OriginalStandardTextureResource50LA* resource) noexcept {
    if(!storage_)return;
    for(auto& file:storage_->files)if(file->cacheIdentity==resource){
        file->cached.reset();file->cacheIdentity=nullptr;return;
    }
}
bool OriginalStandardTextureResolver50LA::reclaimNeverBorrowedSelectedPstAfterBankClose(
    GarageBankLease50LA& lease,D& d) noexcept {
    d={};const auto& receipt=lease.closedBacking_;
    if(!storage_||lease.resolver_||lease.generation_||lease.provider_||lease.archive_||
       receipt.resolver!=this||receipt.thread!=std::this_thread::get_id()||!receipt.selected19E||
       !receipt.provider||receipt.provider!=storage_->provider||
       receipt.managerIdentity!=storage_->managerIdentity||!receipt.generation||
       !receipt.provider->sealed_||!receipt.provider->retainedSourcesAccessible()||
       receipt.mount>=receipt.provider->mounts_.size())return fail(d,E::phase,0x12EB60);
    const auto& bank=*receipt.provider->mounts_[receipt.mount];
    if(bank.active||bank.generation!=receipt.generation||!bank.admittedUi.expired()||
       !bank.ui||!bank.ui->identity()||bank.ui->identity()->id!=OriginalUiArchiveId50KG::garagePod19)
        return fail(d,E::phase,0x12EB60);
    std::size_t residentBytes=0,releaseBytes=0;
    for(std::size_t index=0;index<storage_->files.size();++index){
        const auto& file=*storage_->files[index];
        if(file.kind==Storage::FileKind::pst||file.kind==Storage::FileKind::tga||file.kind==Storage::FileKind::atm){
            if(file.bytes.size()>std::numeric_limits<std::size_t>::max()-residentBytes)
                return fail(d,E::range,0x1327D0);
            residentBytes+=file.bytes.size();
        }
        if(!file.retired||file.kind!=Storage::FileKind::pst||file.bytes.empty()||
           file.bank!=receipt.mount||file.bankGeneration!=receipt.generation||file.pstPublication)continue;
        bool eligible=false;
        if(file.pstBorrowers)return fail(d,E::phase,0x1327D0);
        if(!preflightRetiredGaragePstBacking(index,true,eligible,d))return false;
        if(!eligible)return fail(d,E::phase,0x1327D0);
        if(file.bytes.size()>std::numeric_limits<std::size_t>::max()-releaseBytes)
            return fail(d,E::range,0x1327D0);
        releaseBytes+=file.bytes.size();
    }
    if(residentBytes!=storage_->textureBytes||releaseBytes>residentBytes)
        return fail(d,E::phase,0x1327D0);
    // Complete source identity and accounting preflight above. Preserve all
    // consumed row bytes, final-release receipts, pixel leases and stable IDs.
    for(auto& row:storage_->files){auto& file=*row;
        if(file.retired&&file.kind==Storage::FileKind::pst&&!file.bytes.empty()&&
           file.bank==receipt.mount&&file.bankGeneration==receipt.generation&&!file.pstPublication){
            std::vector<std::uint8_t>().swap(file.bytes);
        }
    }
    storage_->textureBytes-=releaseBytes;lease.closedBacking_={};
    d={};d.sourceEffectsCommitted=releaseBytes!=0;return true;
}
bool OriginalStandardTextureResolver50LA::beginPstSourceBorrow24E1B0(
    OriginalStandardTextureResource50LA& resource,D& d) noexcept {
    if(!storage_||resource.resolver_.get()!=this||resource.sourceIndex_>=storage_->entries.size()||
       resource.pstSourceBorrowIdentity_||resource.pstSourceBorrowPublication_)
        return fail(d,E::phase,0x24E1B0);
    auto& file=*storage_->entries[resource.sourceIndex_].file;
    // ATM aggregate/child and TGA ownership remain exactly their old paths.
    if(file.kind!=Storage::FileKind::pst)return true;
    if(file.bytes.empty()||file.pstBorrowers==std::numeric_limits<std::size_t>::max()||
       file.pstPublication==std::numeric_limits<std::uint64_t>::max())
        return fail(d,E::range,0x24E1B0);
    ++file.pstBorrowers;++file.pstPublication;file.finalPstRelease={};
    resource.pstSourceBorrowIdentity_=&file;
    resource.pstSourceBorrowPublication_=file.pstPublication;
    // No throwing operation follows publication here. Any later constructor
    // failure retains this real resource and discharges at the same destructor.
    return true;
}
bool OriginalStandardTextureResolver50LA::preflightRetiredGaragePstBacking(
    std::size_t index,bool allowNeverBorrowed,bool& eligible,D& d)const noexcept {
    eligible=false;
    if(!storage_||index>=storage_->files.size())return fail(d,E::phase,0x1327D0);
    const auto& file=*storage_->files[index];
    if(!file.retired||file.kind!=Storage::FileKind::pst||file.bytes.empty()||file.pstBorrowers)return true;
    // No weak expiration inference: zero publication means no real resource
    // ever borrowed this row; otherwise require the actual final destructor.
    if(!file.pstPublication){
        if(!allowNeverBorrowed)return true;
        if(file.finalPstRelease.generation||file.finalPstRelease.publication)
            return fail(d,E::phase,0x1327D0);
    }else {
        const auto& final=file.finalPstRelease;
        if(!final.generation)return true; // Uncompleted/failed source prefix retained.
        if(final.bank!=file.bank||final.generation!=file.bankGeneration||
           final.bytes!=file.bytes.size()||final.publication!=file.pstPublication)
            return fail(d,E::phase,0x1327D0);
    }
    const auto& provider=storage_->provider;
    if(!provider||!provider->sealed_||!provider->retainedSourcesAccessible()||
       !file.bankGeneration||file.bank>=provider->mounts_.size())return fail(d,E::phase,0x12EB60);
    const auto& bank=*provider->mounts_[file.bank];
    if(bank.active||bank.generation!=file.bankGeneration||!bank.admittedUi.expired()||
       !bank.ui||!bank.ui->identity()||
       (bank.ui->identity()->id!=OriginalUiArchiveId50KG::garage&&
        bank.ui->identity()->id!=OriginalUiArchiveId50KG::garagePod19)||file.bankOrdinal>=bank.rows.size())
        return fail(d,E::phase,0x12EB60);
    if(file.cacheIdentity||!file.cached.expired()||file.animation||file.geometryIdentity||
       !file.geometry.expired()||!file.payloadGeometry.expired()||file.collisionIdentity||
       !file.collision.expired()||!file.ipumImages.expired())return fail(d,E::phase,0x1327D0);
    const auto& source=bank.rows[file.bankOrdinal];
    if(!same(source.name,file.member)||!source.data||source.size!=file.bytes.size()||
       !std::equal(file.bytes.begin(),file.bytes.end(),source.data)||
       (file.garageIpum&&!file.garageIpum->matches(file.member,file.bytes)))
        return fail(d,E::source,0x1329A0);
    eligible=true;return true;
}
void OriginalStandardTextureResolver50LA::finishPstSourceBorrow29C7A0(
    OriginalStandardTextureResource50LA& resource) noexcept {
    if(!resource.pstSourceBorrowIdentity_)return;
    if(!storage_||resource.resolver_.get()!=this||resource.sourceIndex_>=storage_->entries.size())std::terminate();
    auto& file=*storage_->entries[resource.sourceIndex_].file;
    if(resource.pstSourceBorrowIdentity_!=&file||!resource.pstSourceBorrowPublication_||
       resource.pstSourceBorrowPublication_>file.pstPublication||file.kind!=Storage::FileKind::pst||
       !file.pstBorrowers||resource.references00_||resource.registered_||resource.levelCountB0_||
       resource.ipumLease_||resource.ipumImages_||resource.garageIpumLease_||!resource.frames24_.empty())
        std::terminate();
    --file.pstBorrowers;
    resource.pstSourceBorrowIdentity_=nullptr;resource.pstSourceBorrowPublication_=0;
    if(file.pstBorrowers||!resource.constructed_)return;
    // Minted only at actual destructor tail, never from clear1327D0 alone.
    file.finalPstRelease={file.bank,file.bytes.size(),file.bankGeneration,file.pstPublication};
    if(!file.retired)return;
    std::size_t index=0;
    for(;index<storage_->files.size()&&storage_->files[index].get()!=&file;++index){}
    D diagnostic;bool eligible=false;
    if(!preflightRetiredGaragePstBacking(index,false,eligible,diagnostic)||!eligible)return;
    std::size_t residentBytes=0;
    for(const auto& row:storage_->files){
        if(row->kind!=Storage::FileKind::pst&&row->kind!=Storage::FileKind::tga&&row->kind!=Storage::FileKind::atm)continue;
        if(row->bytes.size()>std::numeric_limits<std::size_t>::max()-residentBytes)return;
        residentBytes+=row->bytes.size();
    }
    if(residentBytes!=storage_->textureBytes||file.bytes.size()>residentBytes)return;
    const auto bytes=file.bytes.size();
    std::vector<std::uint8_t>().swap(file.bytes);storage_->textureBytes-=bytes;
    file.finalPstRelease={};
    // Retain indices, bank history and shared immutable decoded IPUM leases.
}
OriginalStandardTextureResource50LA::OriginalStandardTextureResource50LA(AllocationKey) noexcept
    :lifetimeLink_(*this) {}
std::shared_ptr<OriginalStandardTextureResource50LA> OriginalStandardTextureResource50LA::allocate() {
    return std::make_shared<OriginalStandardTextureResource50LA>(AllocationKey{});
}
OriginalStandardTextureResource50LA::~OriginalStandardTextureResource50LA() {
    if(references00_!=0)std::terminate();
    //1021A0 retires the actually constructed prefix in ascending array order,
    //before102144 clears the aggregate cache. No fake ordinary-list entry.
    for(auto& frame:frames24_){
        if(frame&&!frame->release104970())std::terminate();
        frame.reset();
    }
    std::vector<std::shared_ptr<OriginalStandardTextureResource50LA>>().swap(frames24_);frameCount20_=0;
    //Drop this resource's indexed views before29C7A0 unlinks the actual
    //Application texture-manager list, then1327D0 clears the weak file cache.
    //Immutable source backing stays with the pinned resolver. Drop owned
    //kind1 transfer backing as well, without claiming PS2 heap addresses.
    levels_={};levelCountB0_=0;
    if(ipumLease_){
        ipumLease_->compressed_->retire(ipumLease_->sourceFrame().frame,this);
        ipumLease_.reset();
    }
    ipumImages_.reset();garageIpumLease_.reset();
    for(auto& backing:transferBacking_)std::vector<std::uint8_t>().swap(backing);
    for(auto& backing:colorBacking_)std::vector<std::uint16_t>().swap(backing);
    for(auto& backing:rgbaBacking_)std::vector<std::uint8_t>().swap(backing);
    std::vector<std::uint8_t>().swap(tgaReaderScratch_);
    if(registered_&&(!construction_||!construction_->unregisterTexture29C7E0(lifetimeLink_)))
        std::terminate();
    registered_=false;
    if(resolver_){
        resolver_->clear1327D0(this);
        resolver_->finishPstSourceBorrow29C7A0(*this);
    }
}
bool OriginalStandardTextureResource50LA::construct24DA30(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,std::size_t index,
    const std::string& resolvedName,
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context,D& d,std::size_t frame) noexcept {
    if(resolver_||construction_||constructed_||registered_)return fail(d,E::phase,0x24DEF8);
    if(!resolver||!resolver->storage_||index>=resolver->storage_->entries.size())
        return fail(d,E::source,0x24DAF8);
    if(!context||!context->live())return fail(d,E::video,0x29C768);
    const auto& root=resolver->storage_->entries[index];
    // Stable reclaimed tombstones cannot become fresh resource constructors.
    if(root.file->retired&&root.file->bytes.empty())return fail(d,E::unresolved,0x132820);
    const bool child=frame!=static_cast<std::size_t>(-1);
    if(child&&(root.file->format!=OriginalStandardTextureSourceFormat50LA::atmPstFrames||
        frame>=root.frames.size()))return fail(d,E::source,0x10237C);
    const auto& entry=child?root.frames[frame]:root;
    // Lock the entire actual six-frame bridge before any aggregate effects.
    // Bind/adoption are lifetime-once per stage; no reset under old leases.
    std::shared_ptr<const OriginalStandardIpumImageSet50LA> images;
    const auto garageImage=root.file->garageIpum;
    if(entry.kind==2&&(!garageImage||child)){
        images=root.file->ipumImages.lock();
        if(!images||root.frames.size()!=6||images->count()!=6)
            return fail(d,E::unsupported,0x24F148,entry.pstOffset);
        const auto* stage=images->frame(0)?images->frame(0)->compressedIdentity():nullptr;
        for(std::size_t i=0;i<6;++i){const auto& image=images->frame(i);
            if(!image||image->resolverIdentity()!=resolver.get()||image->sourceIndex()!=index||
               image->compressedIdentity()!=stage||image->sourceFrame().frame!=i||
               ((!child||i==frame)&&!stage->canAdopt(i)))
                return fail(d,E::source,0x24F148,entry.pstOffset);
        }
    }
    if(entry.kind==2&&garageImage&&(!garageImage->matches(root.file->member,root.file->bytes)||
       child||root.file->format!=OriginalStandardTextureSourceFormat50LA::pstIpum||
       entry.count<1||static_cast<std::size_t>(entry.count)!=garageImage->texture->mips.size()))
        return fail(d,E::source,0x24F148);
    std::uint32_t allocationPC=0x29EB00;
    std::size_t allocationAt=0;
    try {
        if(!child&&entry.file->format==OriginalStandardTextureSourceFormat50LA::atmPstFrames){
            resolver_=std::move(resolver);construction_=std::move(context);sourceIndex_=index;
            ipumImages_=images;
            aggregate_=true;sourceFormat_=OriginalStandardTextureSourceFormat50LA::atmPstFrames;
            name20_=resolvedName; //Native resolver identity only; original20 is count.
            frameCount20_=static_cast<std::int32_t>(entry.frames.size());
            allocationPC=0x102344;frames24_.reserve(entry.frames.size());
            for(std::size_t ordinal=0;ordinal<entry.frames.size();++ordinal){
                allocationPC=0x102354;
                auto next=allocate();
                //10237C stores the real owner before its24E880 load. Reserve
                //above ensures a failed load retains this partial child.
                frames24_.push_back(std::move(next));
                d.sourceEffectsCommitted=true;
                if(!frames24_.back()->construct24DA30(resolver_,index,{},construction_,d,ordinal))return false;
            }
            width14_=*frames24_.front()->sourceWidth14();height18_=*frames24_.front()->sourceHeight18();
            if(!construction_->live())return fail(d,E::video,0x102508);
            constructed_=true;return true;
        }
        std::vector<std::uint8_t> tgaRgb;
        const bool tga32=entry.file->format==OriginalStandardTextureSourceFormat50LA::tgaBgra32;
        const bool tga=tga32||entry.file->format==OriginalStandardTextureSourceFormat50LA::tgaBgr24;
        const auto tgaWidth=static_cast<std::size_t>(entry.levels[0].width);
        if(tga){
            //Original29F770 completes its temporary image before24E1B0's
            //base/resource registration. Failure here creates no list member.
            allocationAt=18;
            const bool read=tga32?
                detail::OriginalTgaConversion50LA::read32_29EE40(entry.file->bytes,tgaWidth,tgaWidth,tgaRgb):
                detail::OriginalTgaConversion50LA::read24_29EA90(entry.file->bytes,tgaWidth,tgaWidth,tgaRgb);
            if(!read)return fail(d,E::unsupported,tga32?0x29EE40:0x29EA90,18);
        }
        resolver_=std::move(resolver);construction_=std::move(context);sourceIndex_=index;
        if(!resolver_->beginPstSourceBorrow24E1B0(*this,d))return false;
        if(!construction_->registerTexture29C780(lifetimeLink_))return fail(d,E::video,0x29C780);
        registered_=true;d.sourceEffectsCommitted=true;
        //24E310..24E320 clears the binding-policy bit. Neither admitted PST0/1 nor
        //TGA construction takes kind2's24EA40 bit-setting path. Other bits
        //remain unknown; native zero storage is not evidence for their value.
        flagsF8_=0;flagsF8KnownMask_=1;
        tgaReaderScratch_=std::move(tgaRgb);
        allocationPC=0x24DF10;allocationAt=0;
        //24DF10 retains the first request's actual rewritten path spelling,
        //not the file table's potentially different canonical capitalization.
        name20_=resolvedName;
        atmChild_=child;pstOffset_=entry.pstOffset;pstBytes_=entry.pstBytes;
        sourceFormat_=child?(entry.kind==2?OriginalStandardTextureSourceFormat50LA::pstIpum:
            OriginalStandardTextureSourceFormat50LA::pstIndexed):entry.file->format;
        sourceKind_=entry.kind;
        if(tga){
            //250F10/14,251038/40: dimensions and actual format before allocation.
            width14_=height18_=static_cast<std::int32_t>(tgaWidth);
            // Both250F7C..98 (1555) and250F9C..B8 (8888) preserve
            // other bits and write (F8 & ~4) | 4 before251038/40.
            flagsF8_=static_cast<std::uint8_t>((flagsF8_&~4u)|4u);
            flagsF8KnownMask_=static_cast<std::uint8_t>(flagsF8KnownMask_|4u);
            formatAC_=tga32?8888:1555;bitsPerPixelE8_=tga32?32:16;
            levelCountB0_=entry.count;
            for(std::size_t slot=0;slot<static_cast<std::size_t>(entry.count);++slot){
                const auto width=tgaWidth>>slot;
                allocationPC=slot?0x2502AC:0x25009C;
                auto& destination=levels_[slot];
                // Allocate all final levels before251228 conversion, as250040.
                if(tga32){
                    rgbaBacking_[slot].resize(width*width*4);
                    destination.rgba8888=rgbaBacking_[slot].data();
                    destination.rgba8888Bytes=rgbaBacking_[slot].size();
                    destination.storage=slot?OriginalStandardTextureLevelStorage50LA::tgaGenerated8888:
                        OriginalStandardTextureLevelStorage50LA::tgaConverted8888;
                }else{
                    colorBacking_[slot].resize(width*width);
                    destination.packed1555=colorBacking_[slot].data();
                    destination.packed1555Words=colorBacking_[slot].size();
                    destination.storage=slot?OriginalStandardTextureLevelStorage50LA::tgaGenerated1555:
                        OriginalStandardTextureLevelStorage50LA::tgaConverted1555;
                }
                destination.width104=destination.height110=static_cast<std::int32_t>(width);
                destination.sourceHeaderOffset=slot?static_cast<std::size_t>(-1):0;
                destination.sourceIndicesOffset=static_cast<std::size_t>(-1);
                destination.sourceMipOrdinal=static_cast<std::uint32_t>(-1);
            }
            if(tga32){
                //16A530 reduction0: preserve all8888 bytes, no TEXA or coverage conversion.
                std::copy(tgaReaderScratch_.begin(),tgaReaderScratch_.end(),rgbaBacking_[0].begin());
            }else if(!detail::OriginalTgaConversion50LA::convert1555_16C0D0(
                tgaReaderScratch_,tgaWidth,tgaWidth,colorBacking_[0]))return fail(d,E::range,0x16C0D0);
            //25124C->251720: every level reads its immediate predecessor.
            for(std::size_t slot=1;slot<static_cast<std::size_t>(entry.count);++slot){
                const auto width=tgaWidth>>(slot-1);
                const bool generated=tga32?
                    detail::OriginalTgaConversion50LA::mip8888_168470(rgbaBacking_[slot-1],width,width,rgbaBacking_[slot]):
                    detail::OriginalTgaConversion50LA::mip1555_168980(colorBacking_[slot-1],width,width,colorBacking_[slot]);
                if(!generated)return fail(d,E::range,tga32?0x168470:0x168980);
            }
            //29FEF0 runs after1326A0. Keep the temporary image until the
            //same resolver actually publishes this resource association.
            if(!construction_->live())return fail(d,E::video,0x24DFD8);
            constructed_=true;return true;
        }
        if(entry.kind==2&&garageImage&&!child){
            // Static table loader uses flags2: every actual authored mip is
            // retained in descending native slots. RA flags0 remains below.
            garageIpumLease_=garageImage;formatAC_=1555;bitsPerPixelE8_=16;
            flagsF8_=static_cast<std::uint8_t>(flagsF8_|1u);
            for(std::int32_t i=0;i<entry.count;++i){
                const auto ordinal=static_cast<std::size_t>(i),slot=static_cast<std::size_t>(entry.count-1-i);
                const auto& source=entry.levels[ordinal];const auto& image=garageImage->texture->mips[ordinal];
                auto& level=levels_[slot];level.storage=OriginalStandardTextureLevelStorage50LA::garageIpumCompressed1555;
                level.compressed00=entry.file->bytes.data()+source.indices;level.compressedBytes=source.bytes;
                level.codec18=level.compressed00+16;level.codecBytes1C=source.bytes-16;
                level.sourceHeaderOffset=source.header;level.sourceIndicesOffset=source.indices;
                level.sourceMipOrdinal=static_cast<std::uint32_t>(i);level.width104=source.width;level.height110=source.height;
                level.rgba8888=image.rgba.data();level.rgba8888Bytes=image.rgba.size();
                level.garageIpumProvenance=garageImage->provenance;
            }
            const auto& base=entry.levels[static_cast<std::size_t>(entry.count-1)];
            width14_=base.width;height18_=base.height;levelCountB0_=entry.count;
            if(!construction_->live())return fail(d,E::video,0x24E090);
            constructed_=true;return true; // No upload, second decode or image copy.
        }
        if(entry.kind==2){
            if(!child||!images||!images->frame(frame))return fail(d,E::phase,0x24F148);
            //24E960/96C before24EA40: actual kind2 format/bits, no palette.
            formatAC_=1555;bitsPerPixelE8_=16;flagsF8_=static_cast<std::uint8_t>(flagsF8_|1u);
            const auto& image=images->frame(frame);const auto& f=image->sourceFrame();
            const auto& loaded=entry.levels[static_cast<std::size_t>(entry.count-1)];
            if(f.sourceMipOrdinal!=static_cast<std::size_t>(entry.count-1)||
               f.pstOffset!=entry.pstOffset||f.pstBytes!=entry.pstBytes||
               f.sourceHeaderOffset!=loaded.header||f.sourcePayloadOffset!=loaded.indices||
               f.sourcePayloadBytes!=loaded.bytes||f.width!=static_cast<std::uint32_t>(loaded.width)||
               f.height!=static_cast<std::uint32_t>(loaded.height)||!f.allocation00||
               f.codec18!=f.allocation00+16||f.codecBytes1C!=f.sourcePayloadBytes-16)
                return fail(d,E::source,0x24F158);
            // Host staging performed allocation/read earlier without source
            // effects. This source-phase adoption publishes that SAME backing
            // once; it neither allocates a duplicate nor claims a new read.
            if(!image->compressed_->adopt(frame,this))return fail(d,E::phase,0x2507F0);
            ipumLease_=image;d.sourceEffectsCommitted=true;
            auto& level=levels_[0];level.compressed00=f.allocation00;level.compressedBytes=f.sourcePayloadBytes;
            level.codec18=f.codec18;level.codecBytes1C=f.codecBytes1C;level.hostImage=image.get();
            level.storage=OriginalStandardTextureLevelStorage50LA::ipumCompressed1555;
            level.sourceHeaderOffset=f.sourceHeaderOffset;level.sourceIndicesOffset=f.sourcePayloadOffset;
            level.sourceMipOrdinal=static_cast<std::uint32_t>(f.sourceMipOrdinal);
            level.width104=loaded.width;level.height110=loaded.height;
            width14_=loaded.width;height18_=loaded.height;levelCountB0_=1;
            if(!construction_->live())return fail(d,E::video,0x24E090);
            constructed_=true;return true; // CPU-only; no GPU submission route added.
        }
        formatAC_=entry.kind==0?4:8; //24E960/968 ->29E790.
        bitsPerPixelE8_=entry.kind==0?4:8; //24E96C/974 ->29E6D0.
        //Ordinary table flags2 fills descending native slots in file order.
        //ATM child flags0 consumes all stored extents, but24EAE8 skips each
        //nonzero descending slot. Only the final/largest mip reaches24ED50.
        //The resolver already validated the skipped headers/payload bounds.
        const std::int32_t firstLoadedMip=child?entry.count-1:0;
        for(std::int32_t i=firstLoadedMip;i<entry.count;++i){
            const auto& level=entry.levels[static_cast<std::size_t>(i)];
            const auto slot=static_cast<std::size_t>(entry.count-1-i);
            auto& destination=levels_[slot];
            destination={entry.file->bytes.data()+level.indices,level.bytes,level.header,level.indices,
                level.width,level.height};
            destination.linearSourceIndices=entry.file->bytes.data()+level.indices;
            destination.linearSourceBytes=level.bytes;
            destination.sourceMipOrdinal=static_cast<std::uint32_t>(i);
            destination.storage=entry.kind==0?OriginalStandardTextureLevelStorage50LA::pstPacked4:
                OriginalStandardTextureLevelStorage50LA::pstTransfer8;
            if(entry.kind==1&&uploadConversion382FC8_!=0){
                //24EE44 sees original cold initial382FC8=1. Every admitted
                //width belongs to24EE50..24EF1C's accepted width set. Do not
                //borrow a water caller's temporary zero to skip this branch.
                //The source zero branch24F128 is deliberately not admitted.
                allocationPC=0x24EFFC;allocationAt=level.indices;
                scalar24D6F0(destination.linearSourceIndices,
                    static_cast<std::size_t>(level.width),static_cast<std::size_t>(level.height),
                    transferBacking_[slot]);
                destination.packedIndices100=transferBacking_[slot].data(); //24F024.
                destination.flags30=static_cast<std::uint8_t>(destination.flags30|4u); //24F044.
                destination.flags30KnownMask=static_cast<std::uint8_t>(destination.flags30KnownMask|4u);
            }
        }
        width14_=levels_[0].width104;height18_=levels_[0].height110;
        levelCountB0_=child?1:entry.count; //24EBB0..24EBC8, actual loader flags.
        if(!construction_->live())return fail(d,E::video,0x24E090);
        constructed_=true;return true;
    }catch(...){return fail(d,E::allocation,allocationPC,allocationAt);}
}
bool OriginalStandardTextureResource50LA::constructed24DA30() const noexcept {return constructed_;}
bool OriginalStandardTextureResource50LA::animated102070() const noexcept {return aggregate_;}
const std::int32_t* OriginalStandardTextureResource50LA::frameCount20() const noexcept {
    return constructed_&&aggregate_?&frameCount20_:nullptr;
}
const OriginalStandardTextureResource50LA* OriginalStandardTextureResource50LA::frame102050(std::int32_t ordinal) const noexcept {
    if(!constructed_||!aggregate_||references00_<=0||ordinal<0||ordinal>=frameCount20_)return nullptr;
    return frames24_[static_cast<std::size_t>(ordinal)].get();
}
bool OriginalStandardTextureResource50LA::pstExtent24E880(std::size_t& offset,std::size_t& bytes) const noexcept {
    offset=bytes=0;if(!sourceKind())return false;offset=pstOffset_;bytes=pstBytes_;return true;
}
const std::int64_t* OriginalStandardTextureResource50LA::references00() const noexcept {return &references00_;}
const float* OriginalStandardTextureResource50LA::demand10() const noexcept {return &demand10_;}
const std::int32_t* OriginalStandardTextureResource50LA::sourceWidth14() const noexcept {return constructed_?&width14_:nullptr;}
const std::int32_t* OriginalStandardTextureResource50LA::sourceHeight18() const noexcept {return constructed_?&height18_:nullptr;}
const std::int32_t* OriginalStandardTextureResource50LA::levelCountB0() const noexcept {return constructed_&&!aggregate_?&levelCountB0_:nullptr;}
const std::string* OriginalStandardTextureResource50LA::name20() const noexcept {return constructed_&&!aggregate_&&!atmChild_?&name20_:nullptr;}
const std::string* OriginalStandardTextureResource50LA::sourceMember() const noexcept {
    return constructed_?&resolver_->storage_->entries[sourceIndex_].file->member:nullptr;
}
const std::vector<std::uint8_t>* OriginalStandardTextureResource50LA::originalBytes() const noexcept {
    return constructed_?&resolver_->storage_->entries[sourceIndex_].file->bytes:nullptr;
}
const OriginalStandardTextureSourceFormat50LA* OriginalStandardTextureResource50LA::sourceFormat() const noexcept {
    return constructed_?&sourceFormat_:nullptr;
}
const std::uint32_t* OriginalStandardTextureResource50LA::sourceKind() const noexcept {
    return constructed_&&(sourceFormat_==OriginalStandardTextureSourceFormat50LA::pstIndexed||
        (sourceFormat_==OriginalStandardTextureSourceFormat50LA::pstIpum&&garageIpumLease_))?&sourceKind_:nullptr;
}
const std::int32_t* OriginalStandardTextureResource50LA::formatAC() const noexcept {return constructed_&&!aggregate_?&formatAC_:nullptr;}
const std::int32_t* OriginalStandardTextureResource50LA::bitsPerPixelE8() const noexcept {return constructed_&&!aggregate_?&bitsPerPixelE8_:nullptr;}
const std::uint8_t* OriginalStandardTextureResource50LA::flagsF8() const noexcept {
    return constructed_&&!aggregate_?&flagsF8_:nullptr;
}
const std::uint8_t* OriginalStandardTextureResource50LA::flagsF8KnownMask() const noexcept {
    return constructed_&&!aggregate_?&flagsF8KnownMask_:nullptr;
}
const std::uint32_t* OriginalStandardTextureResource50LA::uploadConversion382FC8() const noexcept {
    return constructed_&&sourceFormat_==OriginalStandardTextureSourceFormat50LA::pstIndexed?&uploadConversion382FC8_:nullptr;
}
const std::uint8_t* OriginalStandardTextureResource50LA::palette204() const noexcept {
    return sourceKind()&&sourceKind_==0?resolver_->storage_->entries[sourceIndex_].file->bytes.data()+pstOffset_+12:nullptr;
}
const std::uint8_t* OriginalStandardTextureResource50LA::palette208() const noexcept {
    return sourceKind()&&sourceKind_==1?resolver_->storage_->entries[sourceIndex_].file->bytes.data()+pstOffset_+12:nullptr;
}
bool OriginalStandardTextureResource50LA::level100(std::size_t i,OriginalStandardTextureLevelView50LA& out) const noexcept {
    out={};if(!constructed_||aggregate_||i>=static_cast<std::size_t>(levelCountB0_))return false;
    out=levels_[i];return true;
}
bool OriginalStandardTextureResource50LA::retain104960(D& d) noexcept {
    if(!constructed_||references00_<=0)return fail(d,E::reference,0x104960);
    if(references00_==std::numeric_limits<std::int64_t>::max())return fail(d,E::overflow,0x104964);
    ++references00_;return true;
}
bool OriginalStandardTextureResource50LA::release104970() noexcept {
    if(references00_<=0)return false;
    --references00_;return true;
}
bool OriginalStandardTextureResource50LA::demandGarageFloor1FDD60(
    std::uint32_t pc,bool& written,D& d) noexcept {
    written=false;
    if(pc!=0x1FDDA8&&pc!=0x1FDDE8)return fail(d,E::unsupported,pc);
    if(!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,pc);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,pc);
    // Both original compares are10<=current. Do not104A00, choose an ATM
    // child, clear demand, or reinterpret this as GPU residency.
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;}
    return true;
}
bool OriginalStandardTextureResource50LA::demandTournamentMarker1F264C(bool& written,D& d) noexcept {
    written=false;
    if(!aggregate_||!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,0x1F264C);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,0x1F263C);
    //1F262C..264C is a literal floor on the resolved aggregate itself.
    //No child iteration, selected-frame substitution,104A00 or102570 call.
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;}
    return true;
}
bool OriginalStandardTextureResource50LA::demandTournamentOutline1F2530(bool& written,D& d) noexcept {
    written=false;
    if(!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,0x1F2530);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,0x1F2520);
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;}
    return true;
}
bool OriginalStandardTextureResource50LA::demandTournamentFallbackFloor10(
    std::uint32_t storePC,bool& written,D& d) noexcept {
    written=false;
    switch(storePC){
    case 0x1F24FC:case 0x1F2564:case 0x1F25AC:
    case 0x1F25E4:case 0x1F261C:case 0x1F2690:break;
    default:return fail(d,E::unsupported,storePC);
    }
    if(!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,storePC);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,storePC);
    // Literal C.LE.S10,current; no size cap, logarithm, rounding or104A00.
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;}
    return true;
}
bool OriginalStandardTextureResource50LA::demandSpeedBoost18BA70(
    std::uint32_t storePC,bool& written,D& d) noexcept {
    written=false;
    if(storePC!=0x18BAB8u&&storePC!=0x18BB40u&&storePC!=0x18BB88u)
        return fail(d,E::unsupported,storePC);
    if(aggregate_||!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,storePC);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,storePC);
    //Original C.LE.S10,current followed by conditional store. This neither
    //resets demand nor transforms a projected size as104A00 does.
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;}
    return true;
}
bool OriginalStandardTextureResource50LA::demandTimeStats194E20(
    std::uint32_t storePC,bool& written,D& d) noexcept {
    written=false;
    if(storePC!=0x194E58u&&storePC!=0x194EB8u&&storePC!=0x194F18u)
        return fail(d,E::unsupported,storePC);
    if(aggregate_||!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,storePC);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,storePC);
    //Original C.LE.S10,current followed by conditional store. This neither
    //resets demand nor transforms a projected size as104A00 does.
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;}
    return true;
}
bool OriginalStandardTextureResource50LA::demandPause1BAE00(
    std::uint32_t storePC,bool& written,D& d) noexcept {
    written=false;
    if(storePC!=0x1BAE90u&&storePC!=0x1BAEC0u&&storePC!=0x1BAF18u)
        return fail(d,E::unsupported,storePC);
    if(aggregate_||!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,storePC);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,storePC);
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;}
    return true;
}
bool OriginalStandardTextureResource50LA::demandPositionStats191AF0(
    std::uint32_t storePC,bool& written,D& d) noexcept {
    written=false;
    if(storePC!=0x191B28u&&storePC!=0x191B88u)
        return fail(d,E::unsupported,storePC);
    if(aggregate_||!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,storePC);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,storePC);
    // Both source branches skip their store when10 <= current demand.
    // No projected-size104A00, reset14D180 or GPU residency is implied.
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;}
    return true;
}
bool OriginalStandardTextureResource50LA::demandDamage18F470(
    bool& written,D& d) noexcept {
    constexpr std::uint32_t storePC=0x18F4A8u;
    written=false;
    if(aggregate_||!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,storePC);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,storePC);
    //Original C.LE.S10,current followed by conditional store. This neither
    //resets demand nor transforms a projected size as104A00 does.
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;}
    return true;
}
bool OriginalStandardTextureResource50LA::demandSelection249580(bool& written,D& d) noexcept {
    written=false;
    if(!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,0x2495bcu);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,0x2495ccu);
    // Direct source common10 floor, including a real aggregate itself.
    // No104A00 logarithm, width cap, child selection or GPU operation.
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;d.sourceEffectsCommitted=true;}
    return true;
}
bool OriginalStandardTextureResource50LA::demand104A00(float request,bool& written,D& d) noexcept {
    using Math=detail::StandardTextureDemandMath50LA;
    written=false;
    //Aggregate projected-demand consumption is not admitted here.102570 is
    //a format/storage-summary fold, NOT a demand-forwarding virtual.
    if(aggregate_)return fail(d,E::unsupported,0x104A00);
    if(!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,0x104A00);
    //A bounded finite nonnegative demand is all that the recovered material
    //caller can produce. Reject unsupported host numeric modes before mutation.
    if(!Math::supported()||!Math::ordinary(request)||request<0.f||width14_<=0||
       !Math::ordinary(demand10_)||demand10_<0.f)return fail(d,E::number,0x104A00);
    float cap=0.f;
    if(!Math::multiply(2.f,static_cast<float>(width14_),cap))return fail(d,E::number,0x104A14);
    if(request>cap)request=cap; //104A18 C.LE.S request,cap; branch delay preserved.
    //Sony EE CVT.W.S truncates toward zero. Truncation selects thresholds;
    //the original unrounded binary32 request still supplies the fraction.
    const auto integer=static_cast<std::int32_t>(request);
    std::int32_t base=16,level=4;
    if(integer>=256){base=256;level=8;}
    else if(integer>=128){base=128;level=7;}
    else if(integer>=64){base=64;level=6;}
    else if(integer>=32){base=32;level=5;}
    const float baseValue=static_cast<float>(base);
    float difference=0.f,fraction=0.f,next=0.f;
    // Preserve104AAC SUB.S,104AB0 DIV.S, then104ADC ADD.S as separate
    // binary32 results. A rounded material request need not produce an exact
    // level+fraction sum. Use the bounded directed path, not host nearest-even;
    // division by the actual power-of-two base remains exact-only.
    if(!Math::subtractTowardZero(request,baseValue,difference)||
       !Math::divideByPowerOfTwo(difference,baseValue,fraction))
        return fail(d,E::number,0x104AB0);
    if(fraction<0.f)fraction=0.f;
    if(!Math::addTowardZero(static_cast<float>(level),fraction,next))return fail(d,E::number,0x104ADC);
    if(next>demand10_){demand10_=next;written=true;d.sourceEffectsCommitted=true;}
    return true;
}
void OriginalStandardTextureResource50LA::resetDemand14D1A8() noexcept {demand10_=0.f;}
void OriginalStandardTextureLifetime50LA::resetDemand14D180() noexcept {
    //No public/current caller exists. The future source254AD0 frame entry must
    //invoke this exact shared-list reset; demand calls themselves never reset.
    for(cursor08_=head00_;cursor08_;cursor08_=cursor08_->nextA0_)
        cursor08_->resource_->resetDemand14D1A8();
}
}
