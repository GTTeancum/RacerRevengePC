#include "original_standard_texture_resource50la.h"
#include "original_standard_model_resource50la.h"
#include "original_standard_collision_type0_resource50la.h"
#include "original_standard_material_context50la.h"
#include "original_standard_texture_demand_math50la.h"
#include "original_standard_archive_provider50la.h"
#include "original_ui_atm50kg.h"
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
struct OriginalStandardTextureResolver50LA::Storage {
    enum class FileKind : std::uint8_t { pst,tga,atm,mcf,psg,colType0 };
    struct File {
        std::string member;
        std::vector<std::uint8_t> bytes;
        FileKind kind=FileKind::pst;
        OriginalStandardTextureSourceFormat50LA format=OriginalStandardTextureSourceFormat50LA::pstIndexed;
        std::weak_ptr<const OriginalStandardIpumImageSet50LA> ipumImages;
        std::weak_ptr<OriginalStandardTextureResource50LA> cached;
        const OriginalStandardTextureResource50LA* cacheIdentity=nullptr;
        const rr::game::OriginalRacerAnimationCold50LA* animation=nullptr;
        std::weak_ptr<OriginalStandardModelGeometry50LA> geometry;
        const OriginalStandardModelGeometry50LA* geometryIdentity=nullptr;
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
    std::size_t modelBytes=0,textureBytes=0;
    std::weak_ptr<const OriginalStandardMaterialContext50LA> construction;
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> provider;
    bool bound=false,texturesPrepared=false;
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
    if(entry.file->format!=OriginalStandardTextureSourceFormat50LA::atmPstFrames||
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
    if(root.kind!=2||root.frames.size()!=6||file.format!=OriginalStandardTextureSourceFormat50LA::atmPstFrames||
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
            const bool tga=tgaName(source.member);
            const auto n=source.member.size();
            const bool atm=nameDomain(source.member)&&n>=4&&
                same(source.member.substr(n-4),".atm");
            if(!tga&&!atm&&!pstName(source.member))return fail(d,E::input,0x24DADC);
            const auto fileKind=atm?Storage::FileKind::atm:
                tga?Storage::FileKind::tga:Storage::FileKind::pst;
            bool duplicate=false;
            for(const auto& file:next->files)if(same(file->member,source.member)){
                if(file->kind!=fileKind||file->bytes!=bytes)return fail(d,E::duplicateSource,0x132820);
                duplicate=true;break;
            }
            if(storage_)for(const auto& file:storage_->files)if(same(file->member,source.member)){
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
            file->member=source.member;file->bytes=bytes;file->kind=fileKind;
            file->format=atm?OriginalStandardTextureSourceFormat50LA::atmPstFrames:
                tga?(bytes[16]==32?OriginalStandardTextureSourceFormat50LA::tgaBgra32:
                    OriginalStandardTextureSourceFormat50LA::tgaBgr24):
                OriginalStandardTextureSourceFormat50LA::pstIndexed;
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
        return validateProviderCache(d);
    }catch(...){return fail(d,E::allocation,0x132820);}
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
        if(!same(file.member,name))continue;
        if(file.kind!=fileKind||file.bytes!=bytes)return fail(d,E::duplicateSource,0x132820);
        out={this,i};d.source=i;return true;
    }
    if(storage_->files.size()>=32768||bytes.size()>8u*1024u*1024u||
       bytes.size()>256u*1024u*1024u-storage_->modelBytes)
        return fail(d,E::range,0x132820);
    try {
        auto file=std::make_unique<Storage::File>();
        file->member=name;file->bytes=bytes;file->kind=fileKind;
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
    if((file.kind!=Storage::FileKind::mcf&&file.kind!=Storage::FileKind::psg&&file.kind!=Storage::FileKind::colType0)||file.cacheIdentity||!file.cached.expired())
        return fail(d,E::phase,0x1329A0);
    if(file.kind==Storage::FileKind::colType0){
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
    for(std::size_t i=0;i<storage_->files.size();++i)if(same(storage_->files[i]->member,name)){
        if(!modelFile({this,i},out,d))return false;
        if(out.kind!=kind){out={};return fail(d,E::phase,0x1329A0);}
        //1329A0 reads the association without retaining it. A null value is
        //the cold, positively resolved file path, not an unknown-file miss.
        return true;
    }
    return fail(d,E::unresolved,0x13294C);
}
bool OriginalStandardTextureResolver50LA::publishAnimation1326A0(OriginalStandardModelFileId50LA id,
    const rr::game::OriginalRacerAnimationCold50LA* animation,D& d) noexcept {
    OriginalStandardModelFileView50LA file;
    if(!modelFile(id,file,d))return false;
    if(!animation||file.kind!=OriginalStandardModelFileKind50LA::mcf||
       file.animation1536F0||file.geometry240110)return fail(d,E::phase,0x1327B0);
    storage_->files[id.row]->animation=animation;d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::publishGeometry1326A0(OriginalStandardModelFileId50LA id,
    const std::shared_ptr<OriginalStandardModelGeometry50LA>& geometry,D& d) noexcept {
    OriginalStandardModelFileView50LA file;
    if(!modelFile(id,file,d))return false;
    if(!geometry||!geometry->constructed240110()||geometry->fileId().resolver!=this||
       geometry->fileId().row!=id.row||file.kind!=OriginalStandardModelFileKind50LA::psg||
       file.animation1536F0||file.geometry240110)return fail(d,E::phase,0x1327B0);
    storage_->files[id.row]->geometry=geometry;
    storage_->files[id.row]->geometryIdentity=geometry.get();d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::publishCollision1326A0(OriginalStandardModelFileId50LA id,
    const std::shared_ptr<OriginalStandardCollisionType0Resource50LA>& resource,D& d) noexcept {
    OriginalStandardModelFileView50LA file;
    if(!modelFile(id,file,d))return false;
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
        std::string pst=requested;
        //1169D0 finds the LAST dot anywhere, without interpreting separators.
        const auto dot=pst.find_last_of('.');
        if(dot==std::string::npos)pst+=".pst";else pst.replace(dot,std::string::npos,".pst");
        if(!nameDomain(pst))return fail(d,E::range,0x24DF10);
        using Provider=OriginalStandardArchiveProvider50LA;
        const auto& provider=storage_->provider;
        if(provider&&!provider->sealed())return fail(d,E::unresolved,0x132820);
        const auto revision=provider?provider->revision():0;
        const auto resolvePositive=[&](const std::string& name,std::uint32_t pc,
            const Provider::MemberView* view) {
            for(std::size_t i=0;i<storage_->entries.size();++i){
                const auto& entry=storage_->entries[i];
                if(!same(name,entry.file->member))continue;
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
    auto& entry=*storage_->entries[i].file;out=entry.cached.lock();
    if((entry.cacheIdentity!=out.get())||(out&&!out->constructed24DA30()))
        return fail(d,E::phase,0x1329A0);
    return true;
}
bool OriginalStandardTextureResolver50LA::cache1326A0(std::size_t i,
    const std::shared_ptr<OriginalStandardTextureResource50LA>& resource) noexcept {
    if(!storage_||i>=storage_->entries.size()||!resource||!resource->constructed24DA30()||
       resource->resolver_.get()!=this||resource->sourceIndex_!=i)return false;
    auto& entry=*storage_->entries[i].file;if(entry.cacheIdentity||!entry.cached.expired())return false;
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
    ipumImages_.reset();
    for(auto& backing:transferBacking_)std::vector<std::uint8_t>().swap(backing);
    for(auto& backing:colorBacking_)std::vector<std::uint16_t>().swap(backing);
    for(auto& backing:rgbaBacking_)std::vector<std::uint8_t>().swap(backing);
    std::vector<std::uint8_t>().swap(tgaReaderScratch_);
    if(registered_&&(!construction_||!construction_->unregisterTexture29C7E0(lifetimeLink_)))
        std::terminate();
    registered_=false;
    if(resolver_)resolver_->clear1327D0(this);
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
    const bool child=frame!=static_cast<std::size_t>(-1);
    if(child&&(root.file->format!=OriginalStandardTextureSourceFormat50LA::atmPstFrames||
        frame>=root.frames.size()))return fail(d,E::source,0x10237C);
    const auto& entry=child?root.frames[frame]:root;
    // Lock the entire actual six-frame bridge before any aggregate effects.
    // Bind/adoption are lifetime-once per stage; no reset under old leases.
    std::shared_ptr<const OriginalStandardIpumImageSet50LA> images;
    if(entry.kind==2){
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
    return constructed_&&sourceFormat_==OriginalStandardTextureSourceFormat50LA::pstIndexed?&sourceKind_:nullptr;
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
    //This exact-arithmetic admitted subset avoids known host rounding
    //differences; it is not a general EE floating-point emulator/parity claim.
    if(!Math::subtract(request,baseValue,difference)||
       !Math::divideByPowerOfTwo(difference,baseValue,fraction))
        return fail(d,E::number,0x104AB0);
    if(fraction<0.f)fraction=0.f;
    if(!Math::add(static_cast<float>(level),fraction,next))return fail(d,E::number,0x104ADC);
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
