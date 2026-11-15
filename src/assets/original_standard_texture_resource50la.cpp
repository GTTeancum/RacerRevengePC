#include "dlc_files50dl.h"
#include "original_source_psg_missing50la.h"
#include "original_source_psg_file50la.h"
#include "original_source_collision_file50la.h"
#include "original_particle_source_config50la.h"
#include "original_source_sky_file50la.h"
#include "original_source_water_exists131a40.h"
#include "original_water_source_config50la.h"
#include "original_water_pool_source_file50la.h"
#include "original_pod_source_bank50la.h"
#include "original_ra_track_source_file50la.h"
#include "original_standard_texture_resource50la.h"
#include "original_loading_pst_file50la.h"
#include "original_loading_ra_file50la.h"
#include "original_music_source_file50la.h"
#include "original_stream_library_file50la.h"
#include "original_loading_texture_fc50la.h"
#include "original_loading_pst_native50la.h"
#include "original_loading_texture_source50la.h"
#include "original_loading_font_source50la.h"
#include "material_texture_assets50ea.h"
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
#include <cstdio>
#include <atomic>
#include <new>
#include <algorithm>
#include <exception>
#include <limits>
#include <utility>
#include <string_view>
#include <zlib.h>
#include "original_appstart_template_cache50kg.h"

namespace rr::assets {
namespace {
using D=OriginalStandardTextureDiagnostic50LA;
using E=OriginalStandardTextureStatus50LA;
bool fail(D& d,E status,std::uint32_t pc,std::size_t at=0) noexcept {
    d.status=status;d.sourcePC=pc;d.byteOffset=at;return false;
}
bool courseSourceName308F00(std::string_view a,std::string_view b)noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t n=0;n<a.size();++n){auto x=static_cast<unsigned char>(a[n]),y=static_cast<unsigned char>(b[n]);
        if(x>='A'&&x<='Z')x+=32;if(y>='A'&&y<='Z')y+=32;if(x!=y)return false;}
    return true; //308F00 folds case only; '/' and '\\' are not aliases.
}
struct CourseFileAssertions12F290 {
    std::atomic<std::uint32_t> assertion3E1B00{0},assertion3E1B08{0};
    std::atomic<std::uint8_t> guard8228{0},guard822C{0};
};
CourseFileAssertions12F290& courseFileAssertions12F290()noexcept {
    static CourseFileAssertions12F290 state;return state;
}
bool fileAssertions12F290(std::int32_t handle,std::int32_t count,D& d)noexcept {
    auto& a=courseFileAssertions12F290();
    if(!a.guard8228.load(std::memory_order_relaxed)){a.assertion3E1B00.store(1,std::memory_order_relaxed);a.guard8228.store(1,std::memory_order_relaxed);}
    if(a.assertion3E1B00.load(std::memory_order_relaxed)&&handle<0)return fail(d,E::source,0x12F2E0);
    if(!a.guard822C.load(std::memory_order_relaxed)){a.assertion3E1B08.store(1,std::memory_order_relaxed);a.guard822C.store(1,std::memory_order_relaxed);}
    if(a.assertion3E1B08.load(std::memory_order_relaxed)&&handle>=count)return fail(d,E::source,0x12F350);
    return true;
}
struct FileRegistrationAssertions {
    std::atomic<std::uint32_t> word3E1AD0{0},word3E1A98{0},word3E1AA0{0};
    std::atomic<std::uint8_t> guard8210{0},guard81F4{0},guard81F8{0};
};
FileRegistrationAssertions& fileRegistrationAssertions()noexcept {static FileRegistrationAssertions s;return s;}
bool memberNameAssertion12E4E8(const char* name,D& d)noexcept {
    auto& s=fileRegistrationAssertions();
    if(!s.guard8210.load(std::memory_order_relaxed)){s.word3E1AD0.store(1,std::memory_order_relaxed);s.guard8210.store(1,std::memory_order_relaxed);}
    return !s.word3E1AD0.load(std::memory_order_relaxed)||!name||fail(d,E::source,0x12E528);
}
bool deviceAssertions12DAFC(const char* name,std::uint32_t flags,D& d)noexcept {
    auto& s=fileRegistrationAssertions();
    if(!s.guard81F4.load(std::memory_order_relaxed)){s.word3E1A98.store(1,std::memory_order_relaxed);s.guard81F4.store(1,std::memory_order_relaxed);}
    if(s.word3E1A98.load(std::memory_order_relaxed)&&name)return fail(d,E::source,0x12DB3C);
    if(!s.guard81F8.load(std::memory_order_relaxed)){s.word3E1AA0.store(1,std::memory_order_relaxed);s.guard81F8.store(1,std::memory_order_relaxed);}
    return !s.word3E1AA0.load(std::memory_order_relaxed)||(flags&1)||fail(d,E::source,0x12DBA4);
}
struct FileIoAssertion {
    std::atomic<std::uint32_t> word{0};std::atomic<std::uint8_t> guard{0};
    bool reach(bool condition,D& d,std::uint32_t pc)noexcept {
        if(!guard.load(std::memory_order_relaxed)){word.store(1,std::memory_order_relaxed);guard.store(1,std::memory_order_relaxed);}
        return !word.load(std::memory_order_relaxed)||condition||fail(d,E::source,pc);
    }
};
struct FileIoAssertions {
    FileIoAssertion status8218_3E1AE0,status821C_3E1AE8;
    FileIoAssertion active8214_3E1AD8,line8220_3E1AF0,line8224_3E1AF8;
    FileIoAssertion reuse81E8_3E1A80,reuseNext81EC_3E1A88;
    FileIoAssertion eof8230_3E1B10,eof8234_3E1B18;
    FileIoAssertion tell8238_3E1B20,tell823C_3E1B28,seek8240_3E1B30,seek8244_3E1B38;
    FileIoAssertion read8250_3E1B50,read8254_3E1B58,read8258_3E1B60;
    FileIoAssertion read825C_3E1B68,read8260_3E1B70,read8264_3E1B78;
    FileIoAssertion cache827C_3E1BA8,cache8280_3E1BB0;
    FileIoAssertion looseName8208_3E1AC0,looseFree820C_3E1AC8;
    FileIoAssertion close8268_3E1B80,close826C_3E1B88,open8270_3E1B90;
};
FileIoAssertions& fileIoAssertions()noexcept {static FileIoAssertions s;return s;}
bool lineAssertions12EE90(std::int32_t id,std::int32_t count,D& d)noexcept {
    auto& g=fileIoAssertions();return g.line8220_3E1AF0.reach(id>=0,d,0x12EEFC)&&g.line8224_3E1AF8.reach(id<count,d,0x12EF64);
}
bool readAssertions12FB30(std::int32_t id,std::int32_t count,std::uint32_t flags,D& d)noexcept {
    auto& g=fileIoAssertions();return g.read8250_3E1B50.reach(id>=0,d,0x12FBA4)&&g.read8254_3E1B58.reach(id<count,d,0x12FC10)&&g.read8258_3E1B60.reach(flags&4,d,0x12FC94);
}
bool seekAssertions12F640(std::int32_t id,std::int32_t count,D& d)noexcept {
    auto& g=fileIoAssertions();return g.seek8240_3E1B30.reach(id>=0,d,0x12F6A0)&&g.seek8244_3E1B38.reach(id<count,d,0x12F70C);
}
bool tellAssertions12F520(std::int32_t id,std::int32_t count,D& d)noexcept {
    auto& g=fileIoAssertions();return g.tell8238_3E1B20.reach(id>=0,d,0x12F570)&&g.tell823C_3E1B28.reach(id<count,d,0x12F5DC);
}
bool closeAssertions131040(std::int32_t id,std::int32_t count,D& d)noexcept {
    auto& g=fileIoAssertions();return g.close8268_3E1B80.reach(id>=0,d,0x131098)&&g.close826C_3E1B88.reach(id<count,d,0x131104);
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
    enum class FileKind : std::uint8_t { pst,tga,atm,mcf,psg,colType0,colType2 };
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
        std::shared_ptr<const OriginalLoadingTextureSource50LA> loadingRa;
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
        return !file.loadingRa&&provider.lookup132820(file.member,view)==OriginalStandardArchiveProvider50LA::Lookup::present&&
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
// not guest addresses or an emulated allocator. The bounded17CAB8 parser
// reserves one real slot temporarily; no persistent file reader borrows it.
struct OriginalStandardTextureResolver50LA::Scratch {
    static constexpr std::size_t bytes=0x6000,alignment=64;
    struct Slot {
        std::uint8_t* pointer00=nullptr;
        std::int64_t status08=-2; //12C920/944: actual constructor store.
        std::int32_t offset10=-0x6000; //12C924/958.
        std::optional<std::uint32_t> nativeRawValidBytes; // Safety bound, NOT a source slot14 store.
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
// Bounded fresh capacity and first APPSTART registration, on the SAME resolver.
// Native pointers/layout, not guest rows or a second canonical file namespace.
// All fields below are private: no mount/lookup can turn capacity into a handle.
struct OriginalStandardTextureResolver50LA::FreshFileCapacity {
    //Conservative native one-occurrence/whole-table pin, NOT a source
    //singleton or Resource reference. Independent of Water Config custody.
    const OriginalWaterPoolSourceFile50LA* waterPoolFileBorrower=nullptr;
    const OriginalSourceSkyFile50LA* sourceSkyFileBorrower=nullptr; //Distinct native sky occurrence pin.
    const OriginalWaterSourceConfig50LA* waterConfigBorrower=nullptr; //Distinct2B9314 stack occurrence.
    const OriginalParticleSourceConfig50LA* particleConfigBorrower=nullptr; //Native lifetime, not guest state.
    // The actual fresh domain provider, retained before its first native mount.
    // Merely retaining this provider does not establish source history.
    // No provider-to-resolver back edge or second file/association ledger exists.
    // Retain the actual mutable producer owner: owned native mount retirement
    // mutates this same provider revision, never a const view or copied namespace.
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider;
    // Native13C980/13D0F0/13D8F0/13D9F0 adapter over the SAME authenticated
    // immutable encoded input. It is not a decoded-member reader or OS file.
    struct Handle {
        OriginalUiEncodedArchiveView50KG input;
        std::size_t cursor=0;
        std::shared_ptr<const OriginalLoadingTextureSource50LA> looseOwner;
        std::shared_ptr<const std::vector<std::uint8_t>> musicInput;
        explicit Handle(std::shared_ptr<const std::vector<std::uint8_t>> input)noexcept:musicInput(std::move(input)){}
        const std::uint8_t* looseData=nullptr;std::size_t looseBytes=0;
        Handle(std::shared_ptr<const OriginalLoadingTextureSource50LA> owner,const std::uint8_t* data,std::size_t bytes)noexcept
            :looseOwner(std::move(owner)),looseData(data),looseBytes(bytes){}
        const std::uint8_t* data()const noexcept{return musicInput?musicInput->data():looseOwner?looseData:input.data();}
        std::size_t size()const noexcept{return musicInput?musicInput->size():looseOwner?looseBytes:input.size();}
        explicit Handle(OriginalUiEncodedArchiveView50KG value)noexcept:input(std::move(value)){}
        bool seek(std::size_t at)noexcept {if(at>size())return false;cursor=at;return true;}
        bool read(void* output,std::size_t bytes)noexcept {
            if(cursor>size()||bytes>size()-cursor)return false;
            if(bytes)std::memcpy(output,data()+cursor,bytes);cursor+=bytes;return true;
        }
        bool word(std::uint32_t& out)noexcept {
            std::uint8_t b[4];if(!read(b,4))return false;
            out=std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|
                (std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);return true;
        }
    };
    struct Device {
        std::int32_t id00;
        char* name04;
        std::uint32_t cursor08,flags0C;
        Handle* handle10;
        std::uint32_t length14,payload18,tableOffset1C,tableBytes20;
        std::uint8_t byte24;
        void initialize(std::int32_t id)noexcept {
            flags0C=1;name04=nullptr;cursor08=0;length14=0;handle10=nullptr;
            id00=id;payload18=0;tableBytes20=0;tableOffset1C=0;byte24=0;
        }
    };
    struct Member {
        std::int32_t deviceID00,auxiliarySlot04;
        char* filename08;
        std::uint32_t logicalStart0C,cursor10,length14,flags18;
        void* associatedObject1C;
        // Identity-only bridge into the existing authenticated directory, not
        // another cache association, decoded payload or source member index.
        const OriginalUiMemberIdentity50KG* originalMember=nullptr;
        const std::string* canonicalName=nullptr;
        const std::uint8_t* canonicalData=nullptr;
        const OriginalLoadingRaFile50LA* loadingRaOwner=nullptr;
        const OriginalMusicSourceFile50LA* musicOwner=nullptr;
        const void* courseConfigOwner=nullptr; // Exact private native occurrence, never a source handle ID.
        void initialize()noexcept {
            flags18=1;filename08=nullptr;deviceID00=0;logicalStart0C=0;
            cursor10=0;length14=0;auxiliarySlot04=0;associatedObject1C=nullptr;
        }
    };
    struct Input {
        std::uint8_t* pointer=nullptr;
        // Native safety metadata is not an original constructor length write.
        std::uint32_t length; // Unwritten on a later allocation failure.
        bool lengthWritten=false;
    };
    std::int32_t freeDevice10=-1,deviceCount18=0,freeMember14C=-1,memberCount154=0;
    Device* devices14=nullptr;
    Member* members150=nullptr;
    std::array<std::int32_t,4> active; // First filled by the cleanup prefix.
    std::array<Input,8> inputs;
    std::uint32_t total78=0,read174=0;
    std::int32_t current74=-1,window7C=-1,window80=-1;
    std::uint32_t* lengthTable84=nullptr;
    std::int64_t cursor160=-1,cursor168=-1;
    bool sourceResult=false; // Native record, not a new source field.
    enum class Registration { untouched,attempted,complete };
    Registration registration=Registration::untouched;
    std::shared_ptr<const OriginalUiArchive50KG> appStart;
    Scratch* scratchOwner=nullptr; // Same resolver; destroyed strictly after this owner.
    bool namesReadAttempted=false; // Terminal native prefix guard, not source readiness.
    std::int32_t templateStage=0,templateMember=-1;
    bool templateFailed=false;
    enum class CourseStage {none,attempted,registered,joined,failed,retired};
    CourseStage courseStage=CourseStage::none;
    SourceBankFamily sourceBankFamily=SourceBankFamily::course;
    bool loadingFontsSourceRetired=false,loadingIconSourceRetired=false; // Only genuine source12E5E0 produces these predecessors.
    OriginalStreamLibraryFile50LA* streamLibraryOwner=nullptr; // Same-App sequential TXT/JUK native pin.
    OriginalMusicSourceFile50LA* musicOwner=nullptr; // Pins exact Config open, including misses and bounded failures.
    OriginalLoadingRaFile50LA* loadingRaOwner=nullptr; // Distinct simultaneous loose owner, never a bank root.
    struct TrackBank {
        OriginalRaTrackSourceFile50LA* root=nullptr;
        std::shared_ptr<const OriginalUiArchive50KG> archive;
        std::int32_t device=-1,pendingMember=-1,registrationSlot=-1;
        std::unique_ptr<Handle> registrationInput;
        bool attempted=false,registered=false,joined=false,failed=false,cacheAttempted=false,cacheReturned=false;
        std::size_t mount=static_cast<std::size_t>(-1);
        std::uint64_t generation=0;
    } track;
    struct PodBank {
        OriginalPodSourceBank50LA* root=nullptr;
        std::shared_ptr<const OriginalUiArchive50KG> archive;
        std::int32_t device=-1,pendingMember=-1,registrationSlot=-1;
        std::unique_ptr<Handle> registrationInput;
        bool attempted=false,registered=false,joined=false,failed=false,cacheAttempted=false,cacheReturned=false;
        std::size_t mount=static_cast<std::size_t>(-1);
        std::uint64_t generation=0;
    } pod;
    const void* courseRootOwner=nullptr;
    std::size_t courseNativeBorrowers=0,courseWindowBytes=0;
    std::int32_t encodedWindowDevice=-1; // Native provenance for actual resident bytes, not a source field.
    std::size_t courseMount=static_cast<std::size_t>(-1);std::uint64_t courseGeneration=0;
    bool courseOwnsNativeMount=false;
    std::uint32_t sourceOpenFlags158=0; //12C858 actual FileManager constructor; no joined setter changes it.
    std::uint32_t sourceFileMode170=1; // Actual12C844 ->12C8C0 constructor.
    bool courseArchiveOnlyCallReached=false; // Native call-history guard, NOT a duplicate source field.
    std::shared_ptr<const OriginalUiArchive50KG> courseArchive;
    std::int32_t courseDevice=-1,coursePendingMember=-1,courseRegistrationSlot=-1;
    std::unique_ptr<Handle> courseRegistrationInput;
    // Source realloc NULL loses its public pointer, but native final disposal
    // retains the previous allocation instead of leaking/dereferencing NULL.
    Device* failedCourseDevices=nullptr;std::int32_t failedCourseDeviceCapacity=0;
    Member* failedCourseMembers=nullptr;std::int32_t failedCourseMemberCapacity=0;
    static constexpr std::size_t inputBytes=0x100000/8,alignment=64;
    FreshFileCapacity()noexcept {active.fill(-1);} //12CBA0 -> cold12CA10/131670.
    FreshFileCapacity(const FreshFileCapacity&)=delete;
    FreshFileCapacity& operator=(const FreshFileCapacity&)=delete;
    void allocateTablesAndInput()noexcept {
        // One-row retry applies even when the original requested count was one.
        deviceCount18=1;
        devices14=new(std::nothrow) Device[deviceCount18];
        freeDevice10=0;read174=0;
        if(!devices14){
            deviceCount18=1;devices14=new(std::nothrow) Device[deviceCount18];
            if(!devices14){deviceCount18=0;freeDevice10=-1;}
        }
        for(std::int32_t i=0;i<deviceCount18;++i)devices14[i].initialize(i);
        memberCount154=10;
        members150=new(std::nothrow) Member[memberCount154];freeMember14C=0;
        if(!members150){
            memberCount154=1;members150=new(std::nothrow) Member[memberCount154];
            if(!members150){memberCount154=0;freeMember14C=-1;}
        }
        for(std::int32_t i=0;i<memberCount154;++i)members150[i].initialize();
        cursor160=-1;cursor168=-1;total78=0;
        sourceResult=true;
        for(std::size_t i=0;i<inputs.size();++i){auto& input=inputs[i];
            input.pointer=static_cast<std::uint8_t*>(::operator new(
                inputBytes,std::align_val_t(alignment),std::nothrow));
            if(i==0&&!input.pointer){
                input.length=inputBytes;input.lengthWritten=true;sourceResult=false;
            }
            if(input.pointer){
                total78+=inputBytes;input.length=inputBytes;input.lengthWritten=true;
            }
        }
    }
    void retireMember12EAA0(std::int32_t i)noexcept {
        // Bounded APPSTART rows: source virtual open/cache slot may be reached;
        // no typed resource object is invented or owned by this row.
        // Names are genuinely owned, including partial insertion failures.
        auto& row=members150[i];
        if(scratchOwner&&row.auxiliarySlot04>=0&&
           static_cast<std::size_t>(row.auxiliarySlot04)<scratchOwner->slots.size()){
            auto& slot=scratchOwner->slots[row.auxiliarySlot04];
            if(slot.status08==i){slot.status08=-1;scratchOwner->selected148=row.auxiliarySlot04;}
        }
        delete[] row.filename08;row.filename08=nullptr;
        row.originalMember=nullptr;row.canonicalName=nullptr;row.canonicalData=nullptr;row.courseConfigOwner=nullptr;row.loadingRaOwner=nullptr;row.musicOwner=nullptr;
        row.auxiliarySlot04=0;row.cursor10=0;row.deviceID00=0;
        row.flags18=1;row.associatedObject1C=nullptr;row.length14=0;row.logicalStart0C=0;
        freeMember14C=i;
    }
    void retireDevice12EB60(Device& row)noexcept {
        //12EB60 fills the removed active-list hole with the rightmost live entry.
        // This preserves the real APPSTART/Course two-device recency state.
        for(std::size_t at=0;at<active.size();++at)if(active[at]==row.id00){
            for(std::size_t donor=active.size();donor>0;--donor)if(active[donor-1]!=-1){
                active[at]=active[donor-1];active[donor-1]=-1;break;}
            break;
        }
        if(current74==row.id00){
            // Native immutable-input backend has no outstanding asynchronous
            // work;132CD0 synchronization completes before invalidation/close.
            window7C=-1;window80=-1;current74=-1;
        }
        freeDevice10=row.id00;
        delete row.handle10;row.handle10=nullptr;
        delete[] row.name04;row.name04=nullptr;
        row.flags0C=1;row.cursor08=0;row.length14=0;
        row.payload18=0;row.tableBytes20=0;row.tableOffset1C=0;row.byte24=0;
    }
    ~FreshFileCapacity(){
        if(waterPoolFileBorrower)std::terminate();
        if(!devices14&&failedCourseDevices){devices14=failedCourseDevices;deviceCount18=failedCourseDeviceCapacity;failedCourseDevices=nullptr;}
        if(!members150&&failedCourseMembers){members150=failedCourseMembers;memberCount154=failedCourseMemberCapacity;failedCourseMembers=nullptr;}
        // Bounded one-device12CA10 order, including reached first-file activation.
        //12E7E0 force visits even empty devices; matching empty members retire.
        if(devices14&&members150){
            for(std::int32_t i=0;i<deviceCount18;++i){auto& device=devices14[i];
                device.flags0C|=0x20;
                for(std::int32_t j=0;j<memberCount154;++j)
                    if(members150[j].deviceID00==device.id00)retireMember12EAA0(j);
                retireDevice12EB60(device);
            }
            active.fill(-1);
        }
        active.fill(-1);
        if(devices14)for(std::int32_t i=0;i<deviceCount18;++i)retireDevice12EB60(devices14[i]);
        delete[] devices14;devices14=nullptr;
        if(members150)for(std::int32_t i=0;i<memberCount154;++i)retireMember12EAA0(i);
        delete[] members150;members150=nullptr;
        // Device retirement above invalidated current/window before handle close.
        for(auto& input:inputs){
            if(input.pointer)::operator delete(input.pointer,std::align_val_t(alignment));
            input.pointer=nullptr;
        }
        total78=0; //1331C0 does not clear lengths, counts or free indices.
        delete[] lengthTable84;lengthTable84=nullptr;
        // The resolver explicitly releases its EXISTING Scratch owner next.
    }
};
bool OriginalStandardTextureResolver50LA::bootstrapFreshFileCapacity12CBA0(
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider,D& d)noexcept {
    d={};
    // Reject ANY previous native corpus/provider/storage, not merely a warm
    // texture flag. Never clear/reconstruct canonical associations to get here.
    if(storage_||scratch_||freshFileCapacity_||!provider||provider->mountCount()!=0||
       provider->revision()!=0||provider->sealed())return fail(d,E::phase,0x12CBA0);
    freshFileCapacity_.reset(new(std::nothrow) FreshFileCapacity);
    if(!freshFileCapacity_)return fail(d,E::allocation,0x12C820);
    freshFileCapacity_->provider=std::move(provider);
    freshFileCapacity_->allocateTablesAndInput();
    D scratchDiagnostic;
    const bool scratchResult=initializeScratch12CDF0(scratchDiagnostic);
    freshFileCapacity_->scratchOwner=scratch_.get();
    freshFileCapacity_->sourceResult=freshFileCapacity_->sourceResult&&scratchResult;
    freshFileCapacity_->active.fill(-1); //131670 after selected148 publication.
    d.sourceEffectsCommitted=true;
    // Original12C820 retains/returns its owner even on false12CBA0. This native
    // caller instead reports admission failure, retaining ALL partial owners
    // here until ordered destruction; it does not simulate source rollback.
    // Missing capacity tables are an explicit ADDITIONAL native admission gate,
    // not part of the source Boolean (slot0 success && >=2 auxiliary slots).
    if(!freshFileCapacity_->sourceResult||!freshFileCapacity_->devices14||
       !freshFileCapacity_->members150)return fail(d,E::allocation,0x12CBA0);
    return true;
}
std::shared_ptr<const OriginalUiArchive50KG>
OriginalStandardTextureResolver50LA::registeredAppStart17CAB8()const noexcept {
    return freshFileCapacity_&&freshFileCapacity_->registration==FreshFileCapacity::Registration::complete?
        freshFileCapacity_->appStart:std::shared_ptr<const OriginalUiArchive50KG>{};
}
bool OriginalStandardTextureResolver50LA::nativeBeforeAppStart17CAB8()const noexcept {
    if(!freshFileCapacity_||freshFileCapacity_->registration!=FreshFileCapacity::Registration::untouched)
        return false;
    const auto& provider=freshFileCapacity_->provider;
    return provider&&provider->mountCount()==0&&provider->revision()==0&&!provider->sealed();
}
bool OriginalStandardTextureResolver50LA::registerAppStart17CAB8(
    std::shared_ptr<const OriginalUiArchive50KG> archive,D& d)noexcept {
    d={};using Capacity=FreshFileCapacity;
    if(!freshFileCapacity_||!scratch_||!archive||!archive->loaded()||
       archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart))
        return fail(d,E::source,0x17CAB8);
    auto& state=*freshFileCapacity_;
    // Native continuation of THIS completed call after a later material/mount
    // failure, not another12CEA0 invocation or a duplicate archive success.
    if(state.registration==Capacity::Registration::complete)
        return state.appStart==archive?true:fail(d,E::phase,0x17CAB8);
    if(state.registration!=Capacity::Registration::untouched||storage_||!state.provider||
       state.provider->mountCount()!=0||state.provider->revision()!=0||state.provider->sealed()||
       state.deviceCount18!=1||state.freeDevice10!=0||!state.devices14||!state.members150||
       state.memberCount154<1||state.freeMember14C!=0||state.current74!=-1||
       !scratch_->initialAllocationReached)return fail(d,E::phase,0x17CAB8);
    for(const auto active:state.active)if(active!=-1)return fail(d,E::phase,0x17CAB8);
    const auto& cold=state.devices14[0];
    if(cold.id00!=0||cold.name04||cold.handle10||cold.flags0C!=1||cold.cursor08||
       cold.length14||cold.payload18||cold.tableOffset1C||cold.tableBytes20||cold.byte24)
        return fail(d,E::phase,0x17CAB8);
    for(std::int32_t i=0;i<state.memberCount154;++i){const auto& row=state.members150[i];
        if(row.filename08||row.flags18!=1||row.associatedObject1C||row.deviceID00||
           row.auxiliarySlot04||row.logicalStart0C||row.cursor10||row.length14||
           row.originalMember||row.canonicalName||row.canonicalData)return fail(d,E::phase,0x17CAB8);
    }
    for(const auto& slot:scratch_->slots)
        if((slot.status08!=-1&&slot.status08!=-2)||
           (slot.status08==-2?slot.pointer00!=nullptr:slot.pointer00==nullptr))
            return fail(d,E::phase,0x17CAB8);
    // EXACT12CF18..24 branch: count18==1 bypasses the archive duplicate scan.
    // Its pending-match overshoot is not repaired or generalized here. The
    // untouched proof above also forbids replay on a registered one-row service.
    const auto selected=scratch_->selected148;
    if(selected<0||static_cast<std::size_t>(selected)>=scratch_->slots.size()||
       !scratch_->slots[selected].pointer00||scratch_->slots[selected].status08!=-1)
        return fail(d,E::phase,0x12CF98);
    state.registration=Capacity::Registration::attempted;
    state.appStart=archive;d.sourceEffectsCommitted=true;
    auto& slot=scratch_->slots[selected];slot.status08=-1;slot.offset10=-0x6000;
    scratch_->selected148=-1; //12CF98..12CFE0 actual reservation.
    state.freeDevice10=-1; // One source row consumed; no other free device.
    std::unique_ptr<Capacity::Handle> input(new(std::nothrow) Capacity::Handle(
        OriginalUiEncodedArchiveView50KG(state.provider.get(),archive)));
    const auto closeInput=[&]()noexcept {
        if(input){input->seek(input->input.size());input.reset();} // EOF/tell/13D9F0.
        scratch_->selected148=selected; //12DAD8, before success publication.
    };
    bool memberWork=false;
    const auto abort=[&](E error,std::uint32_t pc)noexcept {
        // Insertion failure12DA18..50 retires matching members BEFORE EOF,
        // close and scratch restoration. Open/header failure skips this sweep.
        // A later native-only allocation/validation failure also safely frees
        // its reached names; source unchecked malloc has no equivalent branch.
        if(memberWork)for(std::int32_t i=0;i<state.memberCount154;++i)
            if(state.members150[i].deviceID00==0)state.retireMember12EAA0(i);
        closeInput();
        state.retireDevice12EB60(state.devices14[0]);state.freeDevice10=0;
        // Native failure is terminal for this issuer, not an invented fresh
        // origin. Allocated capacity survives until ordered final destruction.
        return fail(d,error,pc);
    };
    if(!input)return abort(E::allocation,0x13C980);
    std::uint32_t version=0,meta=0,flags=0,user=0,tableBytes=0,count=0,directoryBytes=0;
    if(!input->seek(0)||!input->word(version)||!input->word(meta))return abort(E::source,0x12D318);
    // The authenticated APPSTART subset only. Source v2/optional headers and
    // other archives are deliberately not admitted by this private call.
    if(version!=3||!input->word(flags)||!input->word(user)||flags!=0x340||user!=0||
       !input->word(tableBytes)||tableBytes<2||(tableBytes&1)||
       std::uint64_t(tableBytes)+20!=std::uint64_t(meta)+8||
       !input->seek(std::size_t(tableBytes)+20)||!input->word(count)||!input->word(directoryBytes)||
       count!=archive->memberCount()||!count||directoryBytes>=Capacity::inputBytes||
       directoryBytes>=Scratch::bytes||directoryBytes>input->input.size()-input->cursor)
        return abort(E::source,0x12D51C);
    const auto payload=static_cast<std::uint32_t>(input->cursor+directoryBytes);
    const auto fileLength=static_cast<std::uint32_t>(input->input.size());
    if(!input->read(slot.pointer00,directoryBytes))return abort(E::source,0x12D58C);
    std::size_t at=0;
    const auto word=[&](std::uint32_t& value)noexcept {
        if(at>directoryBytes||directoryBytes-at<4)return false;
        const auto* b=slot.pointer00+at;at+=4;
        value=std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|
            (std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);return true;
    };
    bool inserted=true;
    for(std::uint32_t ordinal=0;ordinal<count;++ordinal){
        std::uint32_t nameBytes=0,offset=0,length=0;
        if(!word(nameBytes)||!nameBytes||nameBytes>1024||at>directoryBytes||
           nameBytes>directoryBytes-at)return abort(E::source,0x12E2B0);
        const auto* name=reinterpret_cast<const char*>(slot.pointer00+at);at+=nameBytes;
        if(!word(offset)||!word(length))return abort(E::source,0x12E2B0);
        const auto member=archive->member(ordinal);
        if(!member.identity||std::strlen(member.identity->name)!=nameBytes||
           std::memcmp(name,member.identity->name,nameBytes)||
           member.identity->logicalOffset!=offset||member.size!=length)
            return abort(E::source,0x12E2B0);
        bool duplicate=false;
        for(std::int32_t i=0;i<state.memberCount154;++i)
            if(state.members150[i].filename08&&OriginalStandardArchiveProvider50LA::sameName(
                std::string_view(name,nameBytes),state.members150[i].filename08)){duplicate=true;break;}
        if(duplicate)continue; //12E2B0 first-name wins, no native dedup/ordinal IDs.
        memberWork=true;
        if(state.freeMember14C==-1){
            const auto growth=static_cast<std::int32_t>(count-ordinal); // Remaining entries.
            const auto oldCount=state.memberCount154;
            auto* grown=new(std::nothrow) Capacity::Member[oldCount+growth];
            if(!grown){inserted=false;continue;}
            // Native allocator safety divergence: keep original owner on OOM,
            // rather than overwrite/leak it like source3080C0's unchecked store.
            for(std::int32_t i=0;i<oldCount;++i)grown[i]=state.members150[i];
            delete[] state.members150;state.members150=grown;state.freeMember14C=oldCount;
            for(std::int32_t i=oldCount;i<oldCount+growth;++i){
                grown[i].initialize();++state.memberCount154;
            }
        }
        const auto index=state.freeMember14C;state.freeMember14C=-1;
        if(index<0||index>=state.memberCount154)return abort(E::phase,0x12E2B0);
        for(std::int32_t n=1;n<state.memberCount154;++n){
            const auto next=(index+n)%state.memberCount154;
            if(state.members150[next].flags18&1){state.freeMember14C=next;break;}
        }
        auto& row=state.members150[index];row.flags18=0;
        if(!memberNameAssertion12E4E8(row.filename08,d))return abort(E::source,d.sourcePC);
        row.filename08=new(std::nothrow) char[std::size_t(nameBytes)+1];
        // Source would copy through null here. Native safe failure is explicit.
        if(!row.filename08){inserted=false;continue;}
        std::memcpy(row.filename08,name,nameBytes);row.filename08[nameBytes]=0;
        row.deviceID00=0;row.logicalStart0C=offset;row.cursor10=0;row.length14=length;
        row.auxiliarySlot04=0;row.associatedObject1C=nullptr;row.originalMember=member.identity;
    }
    if(at!=directoryBytes)return abort(E::source,0x12DAB0);
    if(!inserted)return abort(E::allocation,0x12E2B0);
    closeInput(); // Temporary parser handle is NEVER persistent device.handle10.
    auto& device=state.devices14[0];
    if(!deviceAssertions12DAFC(device.name04,device.flags0C,d))return abort(E::source,d.sourcePC);
    device.flags0C=flags|2;device.handle10=nullptr;
    constexpr char sourceName[]="appstart.res";
    device.name04=new(std::nothrow) char[sizeof(sourceName)];
    if(!device.name04)return abort(E::allocation,0x12DBE4);
    std::memcpy(device.name04,sourceName,sizeof(sourceName));device.cursor08=0;
    device.length14=fileLength;device.payload18=payload;device.tableBytes20=tableBytes;
    device.tableOffset1C=20;device.byte24=0;
    // Actual17CAB8 has activate=0: no12E8B0,130340, persistent handle,
    // current74/active-list mutation, member opens or reader readiness.
    state.registration=Capacity::Registration::complete;return true;
}
bool OriginalStandardTextureResolver50LA::joinAppStartCanonicalMount(D& d)noexcept {
    d={};if(!freshFileCapacity_)return fail(d,E::phase,0x17CAB8);
    auto& state=*freshFileCapacity_;
    if(state.registration==FreshFileCapacity::Registration::untouched)return true; // Legacy route.
    if(state.registration!=FreshFileCapacity::Registration::complete||!state.provider||
       state.provider->mounts_.size()!=1)return fail(d,E::phase,0x17CAB8);
    const auto& mount=*state.provider->mounts_[0];
    if(!mount.active||mount.ui!=state.appStart||mount.rows.size()!=state.appStart->memberCount())
        return fail(d,E::source,0x17CAB8);
    // Reconcile actual source-selected first names to the SAME canonical
    // provider rows. Mount ordinal is never substituted for source member ID.
    for(std::int32_t i=0;i<state.memberCount154;++i){auto& member=state.members150[i];
        if(!member.filename08)continue;
        const OriginalStandardArchiveProvider50LA::Row* selected=nullptr;
        for(const auto& row:mount.rows)if(OriginalStandardArchiveProvider50LA::sameName(row.name,member.filename08)){
            selected=&row;break;
        }
        if(!selected||selected->offset!=member.logicalStart0C||selected->size!=member.length14||
           !member.originalMember||member.originalMember->logicalOffset!=selected->offset)
            return fail(d,E::source,0x12E2B0);
        member.canonicalName=&selected->name;member.canonicalData=selected->data;
    }
    return true;
}
bool OriginalStandardTextureResolver50LA::readAppStartNames17CAD4(
    std::vector<std::uint8_t>& bytes,D& d,bool retainTerminator)noexcept {
    d={};using Capacity=FreshFileCapacity;
    if(!bytes.empty()||!freshFileCapacity_||!scratch_)return fail(d,E::phase,0x17CAD4);
    auto& state=*freshFileCapacity_;
    if(state.registration!=Capacity::Registration::complete||state.namesReadAttempted||
       !state.appStart||!state.provider||state.provider->mountCount()!=1||storage_||
       state.deviceCount18!=1||state.current74!=-1||state.window7C!=-1||state.window80!=-1||
       state.lengthTable84||state.cursor168!=-1||state.cursor160!=-1||state.read174)
        return fail(d,E::phase,0x17CAD4);
    for(const auto id:state.active)if(id!=-1)return fail(d,E::phase,0x17CAD4);
    auto& device=state.devices14[0];
    if(device.id00||device.flags0C!=0x342||device.handle10||device.cursor08||
       device.length14!=58977||device.payload18!=283||device.tableOffset1C!=20||device.tableBytes20!=64)
        return fail(d,E::source,0x17CAD4);
    // Exact first1315B0 search after V16. Source rows, not provider ordinals.
    if(state.memberCount154<1)return fail(d,E::source,0x1315B0);
    auto& member=state.members150[0];
    if(!member.filename08||std::strcmp(member.filename08,"AppStart\\Mat_Stbl.txt")||
       member.deviceID00||member.logicalStart0C||member.length14!=70962||member.flags18||
       member.cursor10||member.associatedObject1C||!member.canonicalName||!member.canonicalData||
       !member.originalMember)return fail(d,E::source,0x1315B0);
    const auto chosen=scratch_->selected148;
    if(chosen<0||static_cast<std::size_t>(chosen)>=scratch_->slots.size()||
       !scratch_->slots[chosen].pointer00||scratch_->slots[chosen].status08!=-1)
        return fail(d,E::phase,0x131380);
    if(!state.inputs[0].pointer||!state.inputs[0].lengthWritten||
       state.inputs[0].length<device.length14-device.payload18)
        return fail(d,E::phase,0x130560);
    for(const auto& input:state.inputs)
        if(input.pointer&&(!input.lengthWritten||input.length!=Capacity::inputBytes))
            return fail(d,E::phase,0x130560);
    state.namesReadAttempted=true;d.sourceEffectsCommitted=true;
    ++state.cursor168; //1315B0 first compare matches row0; cursor160 unchanged.
    if(!fileIoAssertions().open8270_3E1B90.reach(!(member.flags18&4),d,0x1313F4))return false;
    member.flags18|=4;member.cursor10=0; //131380 before scratch selection.
    member.auxiliarySlot04=chosen;
    auto& slot=scratch_->slots[chosen];slot.status08=0;slot.offset10=-0x6000;
    scratch_->selected148=-1;
    const auto close=[&]()noexcept {
        if(!closeAssertions131040(0,state.memberCount154,d))return false;
        //131040: archive member close is NOT device/name/cache retirement.
        member.cursor10=0;member.flags18&=~4u;
        if(slot.status08==0)scratch_->selected148=chosen;return true;
    };
    const auto abort=[&](E error,std::uint32_t pc,std::size_t at=0)noexcept {
        if(!close())return false;return fail(d,error,pc,at);
    };
    try {bytes.resize(std::size_t(member.length14)+1);} //2986D4 size+terminator storage.
    catch(...){return abort(E::allocation,0x2986D4);}
    bytes[member.length14]=0;
    //12FB30 inline activation, before request clamp. One actual fresh device,
    // no fabricated promotion/eviction history or generalized warm admission.
    if(!readAssertions12FB30(0,state.memberCount154,member.flags18,d)||!fileIoAssertions().active8214_3E1AD8.reach(true,d,0x12E908))return false;
    state.active[0]=device.id00;
    state.window7C=-1;state.window80=-1;state.current74=device.id00; //132CD0.
    device.handle10=new(std::nothrow) Capacity::Handle(
        OriginalUiEncodedArchiveView50KG(state.provider.get(),state.appStart));
    if(!device.handle10)return abort(E::allocation,0x12FE7C);
    device.cursor08=member.logicalStart0C+member.cursor10;
    auto& input=*device.handle10;
    //130560 cold blocked-cache refill. The backend's actual immutable reads
    // are synchronous; no emulated asynchronous-media success is asserted.
    state.lengthTable84=new(std::nothrow) std::uint32_t[(device.tableBytes20+3)/4];
    if(!state.lengthTable84)return abort(E::allocation,0x130910);
    if(!input.seek(device.tableOffset1C)||!input.read(state.lengthTable84,device.tableBytes20))
        return abort(E::source,0x130934);
    const auto* table=reinterpret_cast<const std::uint8_t*>(state.lengthTable84);
    const auto lengthAt=[&](std::size_t index)noexcept->std::uint32_t {
        return std::uint32_t(table[2*index])|(std::uint32_t(table[2*index+1])<<8);
    };
    const auto tableCount=device.tableBytes20/2;
    for(auto& buffer:state.inputs)if(buffer.pointer)std::memset(buffer.pointer,0,buffer.length);
    if(!input.seek(device.payload18))return abort(E::source,0x130560);
    const auto requested=std::min<std::uint32_t>(device.length14-device.payload18,state.total78);
    state.read174=1;
    std::int64_t remaining=requested;std::size_t actual=0;
    for(auto& buffer:state.inputs){
        if(!buffer.pointer)continue;
        const auto amount=std::min<std::int64_t>(remaining,buffer.length);
        remaining-=buffer.length;
        if(amount<=0)continue;
        if(!input.read(buffer.pointer,static_cast<std::size_t>(amount)))
            return abort(E::source,0x133290,actual);
        actual+=static_cast<std::size_t>(amount);
    }
    state.courseWindowBytes=actual;state.encodedWindowDevice=device.id00; // Actual first17CAD4 encoded read, never a synthetic refill.
    state.window7C=0;state.window80=0;
    // Exact table-loop order: subtract current, read next, conditionally
    // increment end, then test remaining. Added bounds reject unsafe variants.
    std::int64_t residentRemaining=static_cast<std::int64_t>(actual);std::size_t tableIndex=0;
    do {
        if(tableIndex+1>=tableCount||!lengthAt(tableIndex))return abort(E::source,0x130A00);
        residentRemaining-=std::int64_t(lengthAt(tableIndex))+1;++tableIndex;
        const auto next=lengthAt(tableIndex);
        if(std::int64_t(next)<residentRemaining)++state.window80;
    }while(residentRemaining>0);
    if(residentRemaining!=0||tableIndex+1!=tableCount||lengthAt(tableIndex)!=0||
       actual!=device.length14-device.payload18)return abort(E::source,0x130A30);
    // All original APPSTART input is genuinely resident in the existing first
    //128KiB buffer; never copy decoded canonical bytes into these buffers.
    if(actual>state.inputs[0].length)return abort(E::source,0x133290);
    std::size_t copied=0;
    while(copied<member.length14){
        const auto block=std::size_t(device.cursor08)/Scratch::bytes;
        if(block>=tableIndex||block>static_cast<std::size_t>(state.window80))
            return abort(E::source,0x1308C0,copied);
        const auto blockStart=block*Scratch::bytes;
        if(slot.offset10!=static_cast<std::int32_t>(blockStart)){
            std::size_t encodedOffset=0;
            for(std::size_t i=0;i<block;++i)encodedOffset+=lengthAt(i)+1;
            const auto encodedLength=lengthAt(block);
            ResourceBlock48C authenticated;
            if(!input.input.block(block,authenticated)||!encodedLength||encodedLength>=Scratch::bytes||
               authenticated.compressed_bytes!=encodedLength||
               authenticated.compressed_offset!=device.payload18+encodedOffset||
               encodedOffset>=actual||encodedLength>=actual-encodedOffset)
                return abort(E::source,0x133330,copied);
            //13D880 completion is observed from our synchronous backend after
            // actual reads, not blindly cleared as a general source I/O rule.
            state.read174=0;
            const auto* encoded=state.inputs[0].pointer+encodedOffset;
            if(encoded[encodedLength]!=0xff)return abort(E::source,0x133580,copied);
            slot.offset10=static_cast<std::int32_t>(blockStart); // BEFORE codec.
            z_stream z{};
            if(inflateInit(&z)!=Z_OK)return abort(E::allocation,0x2B0780,copied);
            z.next_in=const_cast<Bytef*>(encoded);z.avail_in=encodedLength;
            z.next_out=slot.pointer00;z.avail_out=Scratch::bytes;
            const auto result=inflate(&z,Z_FINISH);
            const bool decoded=result==Z_STREAM_END&&z.total_in==encodedLength&&
                z.total_out==Scratch::bytes&&z.avail_in==0;
            inflateEnd(&z);
            // Native safety is stricter than the source's unchecked codec.
            // Cached-start stamp and any partial output remain on failure.
            if(!decoded)return abort(result==Z_MEM_ERROR?E::allocation:E::source,0x2B0780,copied);
        }
        const auto within=std::size_t(device.cursor08)-blockStart;
        const auto count=std::min<std::size_t>(member.length14-copied,Scratch::bytes-within);
        std::memcpy(bytes.data()+copied,slot.pointer00+within,count);
        copied+=count;device.cursor08+=static_cast<std::uint32_t>(count);
    }
    member.cursor10+=static_cast<std::uint32_t>(copied); //12FB30 after130560 return.
    // Exactly one full requested item. Source2985B0 ignores this count; native
    // consumer rejects short/failed reads rather than parsing invented bytes.
    if(!close())return false; //298714 before lowercase/tokenization/publication.
    //Source2986F4's actual terminator stays inside the vector's live range for
    //the bounded29871C continuation. The legacy semantic parser consumes only
    //member bytes; neither choice replaces the read or performs a new one.
    if(!retainTerminator)bytes.resize(member.length14);return true;
}
bool OriginalStandardTextureResolver50LA::beginAppStartTemplate24AA10(
    bool screen,OriginalAppStartTemplateCache50KG& cache,D& d)noexcept {
    d={};using Capacity=FreshFileCapacity;
    if(!freshFileCapacity_||!scratch_||cache.identity||!cache.sections.empty()||!cache.lines.empty())
        return fail(d,E::phase,0x24AA10);
    auto& state=*freshFileCapacity_;
    const std::int32_t index=screen?2:1;
    if(state.templateFailed||state.templateMember!=-1||state.templateStage!=(screen?1:0)||
       state.registration!=Capacity::Registration::complete||!state.namesReadAttempted||
       !state.appStart||!state.provider||state.provider->mountCount()!=1||storage_||
       state.deviceCount18!=1||state.memberCount154<=index||state.cursor168!=index-1||
       state.current74!=0||state.active[0]!=0||state.active[1]!=-1||state.active[2]!=-1||state.active[3]!=-1||
       state.window7C!=0||state.window80!=30||!state.lengthTable84||state.read174)
        return fail(d,E::phase,0x24AA10);
    auto& device=state.devices14[0];auto& member=state.members150[index];
    const auto authenticated=state.appStart->member(static_cast<std::size_t>(index));
    const auto expectedOffset=screen?98304u:73728u,expectedBytes=screen?748u:82u;
    const auto previousCursor=screen?73810u:70962u;
    const auto previousBlock=screen?73728:49152;
    const auto selected=scratch_->selected148;
    if(device.id00||device.flags0C!=0x342||!device.handle10||device.cursor08!=previousCursor||
       device.payload18!=283||device.tableOffset1C!=20||device.tableBytes20!=64||device.length14!=58977||
       !authenticated.identity||member.originalMember!=authenticated.identity||!member.filename08||
       std::strcmp(member.filename08,screen?"AppStart\\ScreenMT.psm":"AppStart\\DefaultMT.psm")||
       member.deviceID00||member.logicalStart0C!=expectedOffset||member.length14!=expectedBytes||
       member.flags18||member.cursor10||member.associatedObject1C||!member.canonicalName||!member.canonicalData||
       selected<0||static_cast<std::size_t>(selected)>=scratch_->slots.size())
        return fail(d,E::source,0x24AA10);
    auto& slot=scratch_->slots[selected];
    if(!slot.pointer00||slot.status08!=index-1||slot.offset10!=previousBlock||
       !state.inputs[0].pointer||!state.inputs[0].lengthWritten||state.inputs[0].length<58694)
        return fail(d,E::phase,0x131380);
    for(std::int32_t i=0;i<state.memberCount154;++i)
        if(state.members150[i].flags18&4)return fail(d,E::phase,0x131380);
    // Actual next1315B0 match, not a lookup receipt minted from a native mount.
    ++state.cursor168;state.templateMember=index;d.sourceEffectsCommitted=true;
    if(!fileIoAssertions().open8270_3E1B90.reach(!(member.flags18&4),d,0x1313F4))return false;
    member.flags18|=4;member.cursor10=0;member.auxiliarySlot04=selected;
    slot.status08=index;slot.offset10=-0x6000;scratch_->selected148=-1;
    cache.identity=member.originalMember;
    // Supported warm12FB30/12EE90/130560 branch: same head device, handle and
    // complete resident APPSTART window. No eviction/refill/general warm API.
    const auto readSpan=[&](char* destination,std::size_t count)noexcept {
        if(!fileIoAssertions().active8214_3E1AD8.reach(true,d,0x12E908))return false;
        if(!(member.flags18&4)||member.cursor10>member.length14||count>member.length14-member.cursor10)
            return fail(d,E::phase,0x12FB30);
        device.cursor08=member.logicalStart0C+member.cursor10; // Inline activation repositions.
        const auto block=std::size_t(device.cursor08)/Scratch::bytes;
        const auto blockStart=block*Scratch::bytes;
        if(block!=(screen?4u:3u)||count>Scratch::bytes-(device.cursor08-blockStart))
            return fail(d,E::source,0x130560);
        if(slot.offset10!=static_cast<std::int32_t>(blockStart)){
            const auto* table=reinterpret_cast<const std::uint8_t*>(state.lengthTable84);
            const auto lengthAt=[&](std::size_t n)noexcept {
                return std::uint32_t(table[2*n])|(std::uint32_t(table[2*n+1])<<8);
            };
            std::size_t encodedOffset=0;for(std::size_t i=0;i<block;++i)encodedOffset+=lengthAt(i)+1;
            const auto encodedLength=lengthAt(block);ResourceBlock48C actual;
            if(!device.handle10->input.block(block,actual)||!encodedLength||encodedLength>=Scratch::bytes||
               actual.compressed_bytes!=encodedLength||actual.compressed_offset!=device.payload18+encodedOffset||
               encodedOffset>=58694||encodedLength>=58694-encodedOffset)
                return fail(d,E::source,0x133330);
            const auto* encoded=state.inputs[0].pointer+encodedOffset;
            if(encoded[encodedLength]!=0xff)return fail(d,E::source,0x133580);
            slot.offset10=static_cast<std::int32_t>(blockStart); // Reached BEFORE decoder.
            z_stream z{};if(inflateInit(&z)!=Z_OK)return fail(d,E::allocation,0x2B0780);
            z.next_in=const_cast<Bytef*>(encoded);z.avail_in=encodedLength;
            z.next_out=slot.pointer00;z.avail_out=Scratch::bytes;
            const auto result=inflate(&z,Z_FINISH);
            const bool decoded=result==Z_STREAM_END&&z.total_in==encodedLength&&
                z.total_out==Scratch::bytes&&z.avail_in==0;
            inflateEnd(&z);
            if(!decoded)return fail(d,result==Z_MEM_ERROR?E::allocation:E::source,0x2B0780);
        }
        if(count)std::memcpy(destination,slot.pointer00+device.cursor08-blockStart,count);
        device.cursor08+=static_cast<std::uint32_t>(count);return true;
    };
    const auto readLine=[&](OriginalAppStartTemplateCache50KG::Line& line,bool& present) {
        line={};present=false;
        if(!lineAssertions12EE90(index,state.memberCount154,d))return false;
        if(member.cursor10>=member.length14)return true; //12EE90 EOF: no activation.
        const auto start=member.cursor10;
        const auto count=std::min<std::size_t>(member.length14-start,1022);
        std::array<char,1024> text{};
        if(!readSpan(text.data(),count))return false;
        text[count]=0;
        std::size_t end=0;while(end<count&&text[end]&&text[end]!='\n')++end;
        std::size_t consumed=count;
        if(end<count&&text[end]=='\n'){
            consumed=end+1;
            if(end&&text[end-1]=='\r'){text[end-1]='\n';text[end]=0;}
            else text[end+1]=0;
        }
        member.cursor10+=static_cast<std::uint32_t>(consumed);
        present=true;
        return originalAppStartCleanLine159440(text.data(),std::strlen(text.data()),start,line.text,line.offsets)?
            true:fail(d,E::allocation,0x159440);
    };
    try {
        //1577D0 reads exactly one 7-byte probe, overwrites byte6, then compares
        //BINARY. It does not read the whole file or close before field getters.
        std::array<char,7> probe{};
        if(!readAssertions12FB30(index,state.memberCount154,member.flags18,d)||!readSpan(probe.data(),probe.size()))return false;
        member.cursor10+=7;probe[6]=0;
        if(!std::strcmp(probe.data(),"BINARY"))return fail(d,E::source,0x157914);
        if(!seekAssertions12F640(index,state.memberCount154,d))return false;
        member.cursor10=0; //12F640 SEEK_SET; device/cache remain reached.
        std::size_t physicalLine=0,lastHeader=0;
        for(;;){OriginalAppStartTemplateCache50KG::Line line;bool present=false;
            if(!readLine(line,present))return false;if(!present)break;
            if(!line.text.empty()&&line.text[0]=='['){
                const auto end=line.text.find(']');
                if(end==std::string::npos)return fail(d,E::source,0x157500);
                const auto name=line.text.substr(1,end-1);
                // Duplicate/malformed index variants are outside these two
                // authenticated files; never silently replace source indices.
                for(const auto& old:cache.sections)
                    if(OriginalStandardArchiveProvider50LA::sameName(old.name,name))return fail(d,E::source,0x157AF0);
                if(!cache.sections.empty())cache.sections.back().physicalLines=physicalLine-lastHeader-1;
                if(!tellAssertions12F520(index,state.memberCount154,d))return false;
                cache.sections.push_back({name,member.cursor10,0});lastHeader=physicalLine;
            }
            ++physicalLine;
        }
        if(!cache.sections.empty())cache.sections.back().physicalLines=physicalLine-lastHeader-1;
        const OriginalAppStartTemplateCache50KG::Section* selectedSection=nullptr;
        for(const auto& section:cache.sections)
            if(OriginalStandardArchiveProvider50LA::sameName(section.name,"DefaultShaderParams")){
                selectedSection=&section;break;
            }
        //24A9A0 ignores SetSection failure; genuine getter defaults remain
        //possible. No fabricated section or input-read success is inserted.
        if(!selectedSection)return true;
        cache.sectionFound=true;
        if(!seekAssertions12F640(index,state.memberCount154,d))return false;
        member.cursor10=static_cast<std::uint32_t>(selectedSection->bodyOffset);
        std::size_t retained=0;
        for(std::size_t i=0;i<selectedSection->physicalLines;++i){
            OriginalAppStartTemplateCache50KG::Line line;bool present=false;
            if(!readLine(line,present))return false;
            if(!present)return fail(d,E::source,0x157D00);
            if(!line.text.empty()&&line.text[0]!=';')++retained;
        }
        cache.lines.reserve(retained);
        if(!seekAssertions12F640(index,state.memberCount154,d))return false;
        member.cursor10=static_cast<std::uint32_t>(selectedSection->bodyOffset);
        for(std::size_t i=0;i<selectedSection->physicalLines;++i){
            OriginalAppStartTemplateCache50KG::Line line;bool present=false;
            if(!readLine(line,present))return false;
            if(!present)return fail(d,E::source,0x157D00);
            if(!line.text.empty()&&line.text[0]!=';')cache.lines.push_back(std::move(line));
        }
        if(cache.lines.size()!=retained)return fail(d,E::source,0x157D00);
        return true; // Actual member stays open while template getters consume this cache.
    }catch(...){return fail(d,E::allocation,0x157D00);}
}
bool OriginalStandardTextureResolver50LA::closeAppStartTemplate1573B0(bool success,D& d)noexcept {
    d={};if(!freshFileCapacity_||!scratch_)return fail(d,E::phase,0x1573B0);
    auto& state=*freshFileCapacity_;const auto index=state.templateMember;
    if(index<0)return !success; // Failed begin before open: no fabricated close.
    if(index>=state.memberCount154)return fail(d,E::phase,0x1573B0);
    auto& member=state.members150[index];
    if(!(member.flags18&4)||member.auxiliarySlot04<0||
       static_cast<std::size_t>(member.auxiliarySlot04)>=scratch_->slots.size())
        return fail(d,E::phase,0x131040);
    if(!fileAssertions12F290(index,state.memberCount154,d)||!closeAssertions131040(index,state.memberCount154,d))return false;
    //12F290 calls handle13D9E0, whose recovered body is a no-op return0.
    //131040 closes the virtual member, retaining persistent handle/cache state.
    member.cursor10=0;member.flags18&=~4u;
    const auto slot=member.auxiliarySlot04;
    if(scratch_->slots[slot].status08==index)scratch_->selected148=slot;
    state.templateMember=-1;
    if(success)++state.templateStage;else state.templateFailed=true;
    d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::sourceDefaultTemplate17CAE8Closed(
    const std::shared_ptr<const OriginalUiArchive50KG>& archive)const noexcept {
    if(!freshFileCapacity_||!scratch_||!archive)return false;
    const auto& s=*freshFileCapacity_;
    if(s.registration!=FreshFileCapacity::Registration::complete||s.appStart!=archive||
       s.templateStage!=1||s.templateMember!=-1||s.templateFailed||s.memberCount154<=1||
       s.deviceCount18!=1||!s.devices14||!s.members150||s.cursor168!=1||s.current74!=0||s.active[0]!=0||
       !s.devices14[0].handle10||s.devices14[0].cursor08!=73810||s.read174)return false;
    const auto& member=s.members150[1];
    const auto selected=scratch_->selected148;
    return member.cursor10==0&&!(member.flags18&4)&&selected==member.auxiliarySlot04&&
        selected>=0&&static_cast<std::size_t>(selected)<scratch_->slots.size()&&
        scratch_->slots[selected].pointer00&&scratch_->slots[selected].status08==1&&
        scratch_->slots[selected].offset10==73728;
}
bool OriginalStandardTextureResolver50LA::sourceScreenTemplate17CAFCClosed(
    const std::shared_ptr<const OriginalUiArchive50KG>& archive)const noexcept {
    if(!freshFileCapacity_||!scratch_||!archive||storage_)return false;
    const auto& s=*freshFileCapacity_;
    if(s.registration!=FreshFileCapacity::Registration::complete||s.appStart!=archive||
       !s.namesReadAttempted||s.templateStage!=2||s.templateMember!=-1||s.templateFailed||
       s.memberCount154<=2||s.deviceCount18!=1||!s.devices14||!s.members150||
       s.cursor168!=2||s.current74!=0||s.active[0]!=0||s.active[1]!=-1||s.active[2]!=-1||s.active[3]!=-1||
       s.window7C!=0||s.window80!=30||!s.lengthTable84||s.read174||
       !s.devices14[0].handle10||s.devices14[0].cursor08!=99052)return false;
    //Expected values are observations of actual begin/read/close effects,
    //never assignments that complete a member or fabricate a source return.
    const auto& prior=s.members150[1];const auto& member=s.members150[2];
    const auto selected=scratch_->selected148;
    return prior.cursor10==0&&!(prior.flags18&4)&&
        member.originalMember==archive->member(2).identity&&member.logicalStart0C==98304&&member.length14==748&&
        member.cursor10==0&&!(member.flags18&4)&&selected==member.auxiliarySlot04&&
        selected>=0&&static_cast<std::size_t>(selected)<scratch_->slots.size()&&
        scratch_->slots[selected].pointer00&&scratch_->slots[selected].status08==2&&
        scratch_->slots[selected].offset10==98304;
}
OriginalStandardTextureResolver50LA::OriginalStandardTextureResolver50LA() noexcept=default;
OriginalStandardTextureResolver50LA::~OriginalStandardTextureResolver50LA() {
    if(freshFileCapacity_&&(freshFileCapacity_->streamLibraryOwner||freshFileCapacity_->musicOwner||freshFileCapacity_->waterPoolFileBorrower))std::terminate();
    //Every cached live resource pins this resolver. Only cleared associations
    //can remain when the final resolver owner goes away.
    if(storage_)for(const auto& file:storage_->files)
        if(file->cacheIdentity||!file->cached.expired()||file->animation||
           file->geometryIdentity||!file->geometry.expired()||
           file->collisionIdentity||!file->collision.expired())
            std::terminate();
    // Only after the existing live-association preflight: source rows/device,
    // eight input buffers, then the same scratch free/reset suffix. All later
    // native Storage/corpus retirement remains separate from source service IO.
    // Keep the real provider alive through BOTH ordered allocation owners,
    // even before ordinary canonical Storage/provider binding has occurred.
    std::shared_ptr<const OriginalStandardArchiveProvider50LA> bootstrapProvider;
    if(freshFileCapacity_)bootstrapProvider=std::move(freshFileCapacity_->provider);
    freshFileCapacity_.reset();
    scratch_.reset();
}
bool OriginalStandardTextureResolver50LA::bindGarageStaticIpumCache24E880(
    const std::shared_ptr<const OriginalUiIpumTextureCache50KG>& cache,
    const std::shared_ptr<const OriginalUiArchive50KG>& archive,D& d) noexcept {
    d={};
    if(!storage_||!storage_->provider||!storage_->provider->sealed()||!cache||!cache->loaded()||
       !archive||!archive->loaded()||!archive->identity())return fail(d,E::phase,0x24E880);
    try {
        // Leases minted for an earlier Garage's image cache die with that
        // Garage (source UI manager destroy frees its textures). Withdraw one
        // from new issuance only when its sole other holders are tombstoned
        // files of a retired bank (12E5E0 keeps their lease as history, which
        // stays untouched). A live resource/file borrower keeps it here and
        // the retained-identity check below still refuses the mismatch.
        std::vector<std::shared_ptr<const OriginalStandardGarageIpumLease50KG>> next;
        next.reserve(storage_->garageIpum.size());
        for(const auto& old:storage_->garageIpum){
            if(old&&(old->cache!=cache||!old->current())){
                long retiredHolders=0;
                for(const auto& file:storage_->files)if(file->retired&&file->garageIpum==old)++retiredHolders;
                if(old.use_count()==1+retiredHolders)continue;
            }
            next.push_back(old);
        }
        bool found=false;
        const auto count=OriginalUiIpumTextureCache50KG::garageCatalogSourceCount();
        // The catalog lists 11 base sources plus one per career pod archive;
        // a single Garage binds at most 11 leases (checked below).
        if(!count||count>64){std::fprintf(stderr,"garage_static_ipum_rejected count=%zu\n",count);return fail(d,E::source,0x24E880);}
        for(std::size_t i=0;i<count;++i){
            if(OriginalUiIpumTextureCache50KG::garageSourceArchive(i)!=archive->identity())continue;
            found=true;const auto* name=OriginalUiIpumTextureCache50KG::garageSourceMember(i);
            if(!name){std::fprintf(stderr,"garage_static_ipum_rejected name_missing=%zu\n",i);return fail(d,E::source,0x24E880);}
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
               actual.size!=member.size||std::memcmp(actual.data,member.data,member.size)){
                const auto lookup=storage_->provider->lookup132820(name,actual);
                const bool mounted=lookup==OriginalStandardArchiveProvider50LA::Lookup::present&&storage_->provider->mount(actual.mount,mount);
                std::fprintf(stderr,"garage_static_ipum_rejected member=%s data=%d texture=%d provenance=%d in_process=%d policy=%d pst_sha=%d archive_sha=%d "
                    "shape=%d lookup=%d mount=%d mount_sha=%d bank=%d revision=%d size=%d\n",name,int(member.identity&&member.data),int(texture!=nullptr),
                    int(provenance!=nullptr),int(provenance&&provenance->decodedInProcess),
                    int(provenance&&provenance->colorPolicyKind==OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1),
                    int(provenance&&member.identity&&provenance->sourcePstSha256==member.identity->sha256),
                    int(provenance&&provenance->sourceArchiveSha256==archive->identity()->sha256),
                    int(texture&&texture->revision==1&&!texture->flags&&texture->kind==2&&!texture->mips.empty()&&texture->mips.size()<=5&&provenance&&provenance->mipCount==texture->mips.size()),
                    int(lookup),int(mounted),int(mounted&&mount.archiveSha256&&*mount.archiveSha256==archive->identity()->sha256&&mount.archiveBytes==archive->identity()->bytes),
                    int(actual.bankGeneration!=0),int(actual.revision==storage_->provider->revision()),int(actual.size==member.size));
                return fail(d,E::source,0x24E880);}
            bool retained=false;
            for(const auto& old:next)if(old&&old->archive->identity()==archive->identity()&&
                same(old->provenance->sourceMember,provenance->sourceMember)){
                if(!old->current()||old->archive!=archive||old->cache!=cache||old->member.identity!=member.identity||
                   old->member.data!=member.data||old->texture!=texture||old->provenance!=provenance){
                    std::fprintf(stderr,"garage_static_ipum_rejected stale_lease member=%s same_cache=%d borrowers=%ld\n",
                        name,int(old->cache==cache),long(old.use_count())-2);
                    return fail(d,E::phase,0x24E880);}
                retained=true;break;
            }
            if(retained)continue;
            if(next.size()>=11)return fail(d,E::range,0x24E880);
            auto lease=std::shared_ptr<OriginalStandardGarageIpumLease50KG>(new OriginalStandardGarageIpumLease50KG);
            lease->cache=cache;lease->archive=archive;lease->member=member;
            lease->texture=texture;lease->provenance=provenance;next.push_back(std::move(lease));
        }
        if(!found){std::fprintf(stderr,"garage_static_ipum_rejected not_found archive=%s count=%zu\n",archive->identity()->discPath,count);return fail(d,E::source,0x24E880);}
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
                if(file->loadingRa||file->kind!=fileKind||file->bytes!=bytes)return fail(d,E::duplicateSource,0x132820);
                duplicate=true;break;
            }
            if(storage_)for(const auto& file:storage_->files)if(!file->retired&&same(file->member,source.member)){
                if(file->loadingRa||file->kind!=fileKind||file->bytes!=bytes)return fail(d,E::duplicateSource,0x132820);
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
    // A bootstrapped domain must bind the SAME retained provider. Reject a
    // substitute before Storage allocation, association provenance or mutation.
    // Legacy stand-alone resolvers without this fresh facet remain unchanged.
    if(freshFileCapacity_&&freshFileCapacity_->provider!=provider)
        return fail(d,E::phase,0x132820);
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
bool OriginalStandardTextureResolver50LA::issueLoadingFontsBank1CAAE4(
    const std::shared_ptr<OriginalStandardArchiveProvider50LA>& provider,
    const std::shared_ptr<const OriginalUiArchive50KG>& source,LoadingFontsBankLease& lease,D& d)noexcept {
    //12CEA0 may return an existing canonical mount. A freshly authenticated
    //equal-byte read is not that mount's admitted identity; retain the real
    //active acquisition instead of relabeling its bank/generation.
    if(!source||!source->loaded()||source->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::fonts)||
       !storage_||!provider||provider!=storage_->provider||!provider->sealed_)return fail(d,E::source,0x12cea0);
    std::shared_ptr<const OriginalUiArchive50KG> actual;
    const auto* id=source->identity();
    for(const auto& slot:provider->mounts_){const auto& bank=*slot;
        if(!bank.active||!bank.ui||bank.ui->identity()!=id)continue;
        if(actual||bank.name!=id->discPath||bank.digest!=id->sha256||bank.archiveBytes!=id->bytes||
           bank.rows.size()!=id->memberCount)return fail(d,E::source,0x12cea0);
        actual=bank.admittedUi.lock();
        if(!actual||!actual->loaded()||actual->identity()!=id)return fail(d,E::source,0x12cea0);
    }
    if(!actual)return fail(d,E::unresolved,0x12cea0);
    return issueGarageBank(provider,actual,lease.bank_,false,d,true);
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
    bool selected19E,D& d,bool loadingFonts) noexcept {
    d={};
    // selected19E is the historical selected-pod lease kind, not a racer ID.
    if(lease.resolver_||!storage_||!provider||storage_->provider!=provider||!provider->sealed_||
       !provider->retainedSourcesAccessible()||!archive||!archive->loaded()||!archive->identity()||
       (loadingFonts?selected19E||archive->identity()->id!=OriginalUiArchiveId50KG::fonts:
        selected19E?originalUiGaragePodCareer50KG(archive->identity()->id)<0:
        archive->identity()->id!=OriginalUiArchiveId50KG::garage))
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
    lease.generation_=provider->mounts_[bank]->generation;lease.selected19E_=selected19E;lease.loadingFonts_=loadingFonts;
    lease.thread_=std::this_thread::get_id();
    return true;
}
bool OriginalStandardTextureResolver50LA::canRetireGarageBank12E5E0(
    const GarageBankLease50LA& lease,D& d) const noexcept {
    if(freshFileCapacity_&&freshFileCapacity_->waterPoolFileBorrower)return fail(d,E::phase,0);
    if(freshFileCapacity_&&(freshFileCapacity_->streamLibraryOwner||freshFileCapacity_->musicOwner))return fail(d,E::phase,0);
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
       (lease.loadingFonts_?lease.selected19E_||bank.ui->identity()->id!=OriginalUiArchiveId50KG::fonts:
        lease.selected19E_?originalUiGaragePodCareer50KG(bank.ui->identity()->id)<0:
            bank.ui->identity()->id!=OriginalUiArchiveId50KG::garage))return fail(d,E::phase,0x12E6D0);
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
bool OriginalStandardTextureResolver50LA::retireNativeRaceMounts1316A0(
    NativeRaceMountLease50LA& lease,D& d)noexcept {
    if(freshFileCapacity_&&freshFileCapacity_->waterPoolFileBorrower)return fail(d,E::phase,0);
    if(freshFileCapacity_&&(freshFileCapacity_->streamLibraryOwner||freshFileCapacity_->musicOwner||freshFileCapacity_->waterConfigBorrower))return fail(d,E::phase,0);
    d={};
    if(lease.closed_)return true;
    if(lease.thread_!=std::this_thread::get_id()||!lease.provider_||
       !freshFileCapacity_||freshFileCapacity_->provider.get()!=lease.provider_.get()||
       (storage_&&storage_->provider&&storage_->provider.get()!=lease.provider_.get())||
       (storage_&&!storage_->provider&&!storage_->files.empty())||
       lease.count_>lease.entries_.size())return fail(d,E::source,0x1316a0);
    auto& provider=*lease.provider_;
    if(!provider.retainedSourcesAccessible()||
       lease.count_>std::numeric_limits<std::uint64_t>::max()-provider.revision_)
        return fail(d,E::source,0x1316a0);
    for(std::size_t i=0;i<lease.count_;++i){const auto& entry=lease.entries_[i];
        if(entry.slot>=provider.mounts_.size())return fail(d,E::source,0x1316a0);
        const auto& mount=*provider.mounts_[entry.slot];
        // A recorded course mount is released whatever backs it (plain RES,
        // a typed RC/TB/GB/MC compact source, or TA's encoded binding): the
        // provider keeps that backing, so a later race reactivates the same
        // rows. A UI archive is never a race mount.
        if(entry.identity!=&mount||!mount.active||mount.generation!=entry.generation||mount.ui)
            return fail(d,E::source,0x1316a0);
        for(std::size_t j=0;j<i;++j)if(lease.entries_[j].slot==entry.slot)
            return fail(d,E::source,0x1316a0);
        // An original POD registration pins this exact native namespace until
        // its numeric members/device and encoded owner have been withdrawn.
        // Reject every generation, before any mount in this set is retired.
        // Otherwise current() and exact handle close would lose their owner.
        if(freshFileCapacity_->pod.root&&freshFileCapacity_->pod.mount==entry.slot)
            return fail(d,E::phase,0x1316a0);
        // Existing validator is generic complete-directory/provenance logic:
        // matching names with unknown bank origin are NOT silently skipped.
        if(storage_&&storage_->provider&&!validateGarageBankRows12EAA0(entry.slot,entry.generation,d))return false;
        // Check ALL generations, including foreign retained borrowers from a
        // prior inactive mount. A reactivation never grants their ownership.
        if(storage_)for(const auto& file:storage_->files)if(file->bank==entry.slot){
            if(file->pstBorrowers||!file->cached.expired()||file->animation||
               !file->geometry.expired()||!file->payloadGeometry.expired()||
               !file->collision.expired()||!file->ipumImages.expired()||file->garageIpum){
                std::fprintf(stderr,"race_mount_release_refused member=%s pst=%d cached=%d anim=%d geometry=%d payload=%d collision=%d ipum=%d garage=%d\n",
                    file->member.c_str(),file->pstBorrowers?1:0,!file->cached.expired()?1:0,file->animation?1:0,
                    !file->geometry.expired()?1:0,!file->payloadGeometry.expired()?1:0,!file->collision.expired()?1:0,
                    !file->ipumImages.expired()?1:0,file->garageIpum?1:0);
                return fail(d,E::source,0x1316a0);
            }
            if(!file->retired&&file->bankGeneration!=entry.generation)
                return fail(d,E::source,0x1316a0);
        }
    }
    // No allocations or callbacks after whole-set preflight. Stable backing
    // stays owned by provider; no caller file ID or encoded bytes are reused.
    for(std::size_t i=0;i<lease.count_;++i){const auto& entry=lease.entries_[i];
        if(storage_)for(auto& file:storage_->files)if(file->bank==entry.slot&&
            file->bankGeneration==entry.generation&&!file->retired){
            file->cached.reset();file->cacheIdentity=nullptr;file->animation=nullptr;
            file->geometry.reset();file->geometryIdentity=nullptr;
            file->collision.reset();file->collisionIdentity=nullptr;file->ipumImages.reset();
            file->retired=true;
        }
        provider.mounts_[entry.slot]->active=false;
        provider.mounts_[entry.slot]->admittedUi.reset();++provider.revision_;
    }
    lease.closed_=true;lease.count_=0;lease.provider_.reset();
    d.sourceEffectsCommitted=true;return true;
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
        (kind==OriginalStandardModelFileKind50LA::colType0||kind==OriginalStandardModelFileKind50LA::colType2)?".col":nullptr;
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
    if(kind==OriginalStandardModelFileKind50LA::colType0||kind==OriginalStandardModelFileKind50LA::colType2){
        using Provider=OriginalStandardArchiveProvider50LA;Provider::MemberView actual;
        const auto lookup=storage_->provider&&storage_->provider->sealed()?
            storage_->provider->lookup132820(name,actual):Provider::Lookup::unknown;
        if(lookup!=Provider::Lookup::present){
            std::fprintf(stderr,"model_member_bind_rejected name=%s reason=unresolved provider=%d sealed=%d lookup=%d\n",
                name.c_str(),storage_->provider?1:0,storage_->provider&&storage_->provider->sealed()?1:0,int(lookup));
            return fail(d,E::unresolved,0x132820);
        }
        if(!actual.member||!same(name,*actual.member)||!actual.data||actual.size!=bytes.size()||
           actual.revision!=storage_->provider->revision()||!std::equal(bytes.begin(),bytes.end(),actual.data)){
            std::fprintf(stderr,"model_member_bind_rejected name=%s reason=source size=%zu expected=%zu\n",
                name.c_str(),actual.size,bytes.size());
            return fail(d,E::source,0x132820);
        }
    }
    const auto fileKind=kind==OriginalStandardModelFileKind50LA::mcf?
        Storage::FileKind::mcf:kind==OriginalStandardModelFileKind50LA::psg?
        Storage::FileKind::psg:kind==OriginalStandardModelFileKind50LA::colType0?
        Storage::FileKind::colType0:Storage::FileKind::colType2;
    for(std::size_t i=0;i<storage_->files.size();++i){const auto& file=*storage_->files[i];
        if(file.retired||!same(file.member,name))continue;
        if(file.kind!=fileKind||file.bytes!=bytes){
            std::fprintf(stderr,"model_member_bind_rejected name=%s reason=duplicate kind=%d/%d size=%zu/%zu\n",
                name.c_str(),int(file.kind),int(fileKind),file.bytes.size(),bytes.size());
            return fail(d,E::duplicateSource,0x132820);
        }
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
    if((file.kind!=Storage::FileKind::mcf&&file.kind!=Storage::FileKind::psg&&file.kind!=Storage::FileKind::colType0&&file.kind!=Storage::FileKind::colType2)||file.cacheIdentity||!file.cached.expired())
        return fail(d,E::phase,0x1329A0);
    if(!file.retired&&(file.kind==Storage::FileKind::colType0||file.kind==Storage::FileKind::colType2)){
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
       ((file.kind==Storage::FileKind::colType0||file.kind==Storage::FileKind::colType2)&&(geometry||file.animation)))return fail(d,E::phase,0x1329A0);
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
        OriginalStandardModelFileKind50LA::psg:file.kind==Storage::FileKind::colType0?
        OriginalStandardModelFileKind50LA::colType0:OriginalStandardModelFileKind50LA::colType2;
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
    if(!geometry||!geometry->constructed240110()||!geometry->cachePublicationReady1326A0()||
       geometry->fileId().resolver!=this||
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
            originalUiGaragePodCareer50KG(bank.ui->identity()->id)<0))return;
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
           (file.kind==Storage::FileKind::colType0||file.kind==Storage::FileKind::colType2)){
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
bool OriginalStandardTextureResolver50LA::preflightPopulationOwners(
    const std::shared_ptr<const OriginalStandardMaterialContext50LA>& context,
    const std::shared_ptr<const OriginalStandardArchiveProvider50LA>& provider,D& d)const noexcept {
    d={};
    if(!storage_||!provider||!provider->sealed()||storage_->provider!=provider)
        return fail(d,E::source,0x132820);
    if(!context||!context->live())return fail(d,E::video,0x29C768);
    if(storage_->bound&&storage_->construction.lock()!=context)return fail(d,E::phase,0x132820);
    // Preserve the original global resolver namespace guard, including its
    // actual cached-resource re-resolution. Historical receipt validation is
    // not authority to bypass this separate population prerequisite.
    return validateProviderCache(d);
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
namespace {
// Exact images used by the retained Results ledger. This is first resolver
// selection evidence only, NOT permission to render an arbitrary named asset.
bool resultsNativePstName50LA(const std::string& name)noexcept {
    using P=OriginalStandardArchiveProvider50LA;
    for(const char* fixed:{R"(ui\textures\results1.pst)",R"(ui\textures\results2.pst)",
        R"(ui\textures\dmgradar.pst)",R"(font\eurostileboldbold13ingame.pst)"})
        if(P::sameName(name,fixed))return true;
    constexpr const char prefix[]=R"(ui\textures\)";
    if(name.size()!=sizeof(prefix)-1+10)return false;
    if(!P::sameName(std::string_view(name).substr(0,sizeof(prefix)-1),prefix))return false;
    const auto at=sizeof(prefix)-1;
    if(name[at]<'0'||name[at]>'9'||name[at+1]<'0'||name[at+1]>'9'||
       !P::sameName(std::string_view(name).substr(at+2),"_fsm.pst"))return false;
    const unsigned racer=unsigned(name[at]-'0')*10u+unsigned(name[at+1]-'0');
    return racer>=1&&racer<=26&&racer!=5&&racer!=6&&racer!=13;
}
bool resultsNativeAtmName50LA(const std::string& name)noexcept {
    using P=OriginalStandardArchiveProvider50LA;
    return P::sameName(name,R"(ui\textures\medasmgld.atm)")||
        P::sameName(name,R"(ui\textures\medasmslv.atm)")||P::sameName(name,R"(ui\textures\medasmbrz.atm)");
}
}
bool OriginalStandardTextureResolver50LA::resolve24DADC(
    const std::string& requested,std::size_t& index,std::string& resolvedName,D& d,UiSelection* capture) const noexcept {
    if(capture)*capture=UiSelection{};
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
                if(entry.file->retired||entry.file->loadingRa||!same(name,entry.file->member))continue;
                if(view&&(!view->member||!same(*view->member,entry.file->member)||!view->data||
                   view->size!=entry.file->bytes.size()||view->revision!=revision||
                   !std::equal(entry.file->bytes.begin(),entry.file->bytes.end(),view->data)))
                    return fail(d,E::source,pc);
                // Freeze only these concrete UI images at their actual first
                // positive resolver lookup. Denmark16 can first be constructed
                // by Application328/Pause, before Results constructs its font
                // MATERIAL; those two histories must remain independent.
                // No retrospective lookup, resource backfill or material grant.
                const bool denmark16=Provider::sameName(name,"font\\denmarkbold16ingame.pst");
                const bool resultsPst=resultsNativePstName50LA(name)&&
                    entry.file->format==OriginalStandardTextureSourceFormat50LA::pstIndexed;
                const bool resultsAtm=resultsNativeAtmName50LA(name)&&
                    entry.file->format==OriginalStandardTextureSourceFormat50LA::atmPstFrames;
                if(capture&&view&&provider&&
                   (entry.file->format==OriginalStandardTextureSourceFormat50LA::pstIndexed||resultsAtm)&&
                   (Provider::sameName(name,"ui\\textures\\icon_x.pst")||Provider::sameName(name,"ui\\textures\\icon_tri.pst")||
                    Provider::sameName(name,"ui\\textures\\icon_sqr.pst")||Provider::sameName(name,"ui\\textures\\icon_cir.pst")||
                    Provider::sameName(name,"ui\\textures\\overlay1.pst")||denmark16||resultsPst||resultsAtm)&&
                   view->mount<provider->mounts_.size()){
                    const auto& bank=*provider->mounts_[view->mount];
                    if(bank.active&&bank.ui&&bank.ui->loaded()&&bank.generation==view->bankGeneration&&
                       view->ordinal<bank.rows.size()&&provider->sealed()&&provider->revision()==revision){
                        const auto member=bank.ui->member(view->ordinal);
                        const auto& row=bank.rows[view->ordinal];
                        const bool exactResultsSource=!(denmark16||resultsPst||resultsAtm)||
                            (bank.ui->identity()==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)&&
                             member.identity==originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::overlays,view->ordinal)&&
                             member.identity&&Provider::sameName(member.identity->name,name));
                        if(exactResultsSource&&member.identity&&member.data==view->data&&member.size==view->size&&
                           row.data==view->data&&row.size==view->size&&same(row.name,*view->member)){
                            UiSelection selected;selected.resolver=this;selected.providerIdentity=provider.get();
                            selected.archive=bank.ui;selected.member=member.identity;selected.data=member.data;
                            selected.bytes=member.size;selected.sourceIndex=i;selected.mount=view->mount;
                            selected.ordinal=view->ordinal;selected.providerRevision=revision;
                            selected.bankGeneration=view->bankGeneration;*capture=std::move(selected);
                        }
                    }
                }
                // Freeze the actual first positive raw-RES flare lookup only.
                // It cannot obtain an OriginalUiArchive identity, and no later
                // callback may backfill this from mutable File.bank history.
                if(capture&&view&&provider&&Provider::sameName(name,"textures\\flare1.pst")&&
                   entry.file->format==OriginalStandardTextureSourceFormat50LA::pstIndexed&&
                   view->mount<provider->mounts_.size()){
                    const auto& bank=*provider->mounts_[view->mount];
                    if(bank.active&&!bank.ui&&bank.backing&&bank.generation==view->bankGeneration&&
                       Provider::sameName(bank.name,"TRACKS/RA.RES")&&bank.archiveBytes==4604561u&&
                       bank.digest=="1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2"&&
                       view->ordinal<bank.rows.size()&&provider->sealed()&&provider->revision()==revision){
                        const auto& row=bank.rows[view->ordinal];
                        if(row.offset<=bank.backing->size()&&row.size<=bank.backing->size()-row.offset&&
                           row.data==bank.backing->data()+row.offset&&row.data==view->data&&
                           row.size==view->size&&same(row.name,*view->member)){
                            UiSelection selected;selected.resolver=this;selected.providerIdentity=provider.get();
                            selected.data=row.data;selected.bytes=row.size;selected.sourceIndex=i;
                            selected.mount=view->mount;selected.ordinal=view->ordinal;
                            selected.providerRevision=revision;selected.bankGeneration=view->bankGeneration;
                            selected.nativeFlare={bank.backing,&row.name,&bank,row.offset};
                            *capture=std::move(selected);
                        }
                    }
                }
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
        if(lookup==Provider::Lookup::present){
            const bool positive=resolvePositive(pst,0x24DAF8,&found);
            if(!positive){
                std::fprintf(stderr,"texture_resolve_rejected name=%s lookup=present mount=%zu archive=%s size=%zu status=%u entries=%zu\n",
                    pst.c_str(),found.mount,found.mount<provider->mounts_.size()?provider->mounts_[found.mount]->name.c_str():"?",
                    found.size,unsigned(d.status),storage_->entries.size());
                for(std::size_t m=0;m<provider->mounts_.size();++m)
                    std::fprintf(stderr,"texture_resolve_mount %zu %s active=%d rows=%zu\n",m,provider->mounts_[m]->name.c_str(),
                        provider->mounts_[m]->active?1:0,provider->mounts_[m]->rows.size());
                for(const auto& entry:storage_->entries)if(entry.file&&entry.file->member.find("camdroid")!=std::string::npos)
                    std::fprintf(stderr,"texture_resolve_entry %s retired=%d loadingRa=%d\n",entry.file->member.c_str(),
                        entry.file->retired?1:0,entry.file->loadingRa?1:0);
            }
            return positive;
        }
        if(lookup!=Provider::Lookup::absent){
            std::fprintf(stderr,"texture_resolve_rejected name=%s lookup=unknown\n",pst.c_str());
            for(std::size_t m=0;m<provider->mounts_.size();++m){const auto& mount=*provider->mounts_[m];
                for(const auto& row:mount.rows)if(Provider::sameName(row.name,pst))
                    std::fprintf(stderr,"texture_resolve_candidate mount=%zu archive=%s active=%d size=%zu\n",
                        m,mount.name.c_str(),mount.active?1:0,row.size);}
            return fail(d,E::unresolved,0x24DAF8);
        }
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
        if(file.loadingRa){
            if(!loadingRaSourceCurrent(resource->firstUiSource_))return fail(d,E::source,0x132820);
            continue; //Distinct authenticated loose-file owner, never archive lookup.
        }
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
       !bank.ui||!bank.ui->identity()||originalUiGaragePodCareer50KG(bank.ui->identity()->id)<0)
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
        originalUiGaragePodCareer50KG(bank.ui->identity()->id)<0)||file.bankOrdinal>=bank.rows.size())
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
bool OriginalStandardTextureResource50LA::cubSourceCurrent1295B0()const noexcept {
    if(!cub_||!construction_||!construction_->live()||references00_!=1)return false;
    const auto& c=cub_->source;
    if(!c.sourcePin||!c.provider||!c.member||!c.memberData||!c.authenticatedMemberData||
       c.memberBytes!=161825u||!c.imageOwner||!c.bank||!c.image||c.tileOrdinal>=48u||
       !c.bank->loaded()||c.bank->textureCount()!=48u||c.bank->slotCount()!=48u||
       c.bank->texture(static_cast<std::uint32_t>(c.tileOrdinal))!=c.image||
       c.image->identity!=c.tileOrdinal||c.image->texture.width!=128u||
       c.image->texture.height!=128u||c.image->texture.rgba.size()!=65536u)return false;
    const auto nameEquals=[](const std::string& actual,std::string_view expected)noexcept{
        if(actual.size()!=expected.size())return false;
        for(std::size_t i=0;i<actual.size();++i)if(fold(static_cast<unsigned char>(actual[i]))!=
            fold(static_cast<unsigned char>(expected[i])))return false;
        return true;
    };
    if(!nameEquals(*c.member,"skies\\ra.cub"))return false;
    OriginalStandardArchiveProvider50LA::MemberView current;
    if(c.provider->lookup132820(*c.member,current)!=OriginalStandardArchiveProvider50LA::Lookup::present||
       current.member!=c.member||current.data!=c.memberData||current.size!=c.memberBytes||
       current.mount!=c.mount||current.ordinal!=c.ordinal||current.revision!=c.providerRevision||
       current.bankGeneration!=c.bankGeneration)return false;
    OriginalStandardArchiveProvider50LA::MountView mount;
    if(!c.provider->mount(c.mount,mount)||!mount.archiveName||!mount.archiveSha256||
       !nameEquals(*mount.archiveName,"TRACKS/RA.RES")||mount.archiveBytes!=4604561u||
       *mount.archiveSha256!="1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2"||
       mount.memberCount!=1010u)return false;
    if(registered_&&lifetimeLink_.owner_!=construction_->textureLifetime_.get())return false;
    if(cub_->loadComplete24F2C0){
        if(!cub_->constructed24E1B0||!registered_||constructed_||cub_->source.palette||
           width14_!=128||height18_!=128||levelCountB0_!=1||formatAC_!=1555||bitsPerPixelE8_!=16||
           flagsF8KnownMask_!=0x0fu||(flagsF8_&0x0fu)!=1u||cub_->fieldA8!=16||
           cub_->finalCursor!=c.tileOffset+c.tileBytes||cub_->orderedReadBytes!=c.tileBytes-24u||
           !cub_->bufferFC||cub_->bufferFC!=cub_->bufferFCOwner.get())return false;
        const auto& buffer=*cub_->bufferFC;
        if(!buffer.backing||buffer.first00!=buffer.backing.get()||buffer.current04!=buffer.first00||
           buffer.capacity08!=16u||reinterpret_cast<std::uintptr_t>(buffer.first00)%64u||buffer.field0C||buffer.field10||buffer.field14||buffer.field18||buffer.field1C)return false;
        std::size_t total=0;
        for(std::size_t slot=0;slot<4u;++slot){const auto& mip=cub_->mips[slot];const auto dimension=128u>>slot;
            if(!mip.allocation||!mip.payloadRead||mip.allocationBytes<25u||
               reinterpret_cast<std::uintptr_t>(mip.allocation.get())%64u||
               mip.pointer100!=mip.allocation.get()||mip.codec118!=mip.pointer100+16u||
               mip.bytes11C!=static_cast<std::int32_t>(mip.allocationBytes-16u)||
               mip.width104!=static_cast<std::int32_t>(dimension)||mip.height110!=mip.width104||
               mip.width124!=dimension||mip.blocks108!=(dimension<128u?1:2)||
               mip.logWidth10C!=static_cast<std::int32_t>(7u-slot)||mip.logHeight114!=mip.logWidth10C||
               mip.width12C!=mip.width104||mip.height128!=mip.height110||mip.flags130KnownMask!=3u||
               (mip.flags130&3u)||
               mip.sourcePayloadOffset!=mip.sourceLengthOffset+4u||mip.sourcePayloadOffset>c.memberBytes||
               mip.allocationBytes>c.memberBytes-mip.sourcePayloadOffset||
               std::memcmp(mip.pointer100,c.memberData+mip.sourcePayloadOffset,mip.allocationBytes))return false;
            total+=mip.allocationBytes;
        }
        if(total!=cub_->compressedAllocationBytes||cub_->mips[4].allocation)return false;
        for(auto word:cub_->offsetWords)if(word)return false;
    }
    return true;
}
bool OriginalStandardTextureResource50LA::constructCub24E1B0(const CubSource1295B0& source,
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context,D& d)noexcept {
    d={};
    if(cub_||constructed_||registered_||resolver_||construction_||references00_!=1||
       !context||!context->live()||!context->cubCanRetire2544F0())return fail(d,E::phase,0x24e1b0);
    if(!source.sourcePin||!source.provider||!source.member||!source.memberData||
       !source.authenticatedMemberData||!source.palette||!source.imageOwner||
       !source.bank||!source.image||source.memberBytes!=161825u||source.tileOrdinal>=48u||
       source.descriptorWord0||source.tileOffset<99180u||source.tileOffset>source.memberBytes||
       source.tileBytes<44u||source.tileBytes>source.memberBytes-source.tileOffset||
       source.face!=(source.tileOrdinal<32u?source.tileOrdinal/8u:4u)||
       source.tile!=(source.tileOrdinal<32u?source.tileOrdinal%8u:source.tileOrdinal-32u))
        return fail(d,E::source,0x24e1b0);
    try {
        cub_=std::make_unique<CubFacet1295B0>();cub_->source=source;construction_=std::move(context);
        if(!cubSourceCurrent1295B0()||std::memcmp(source.memberData,source.authenticatedMemberData,source.memberBytes)!=0)
            return fail(d,E::source,0x24e1b0);
        // SAME actual1048F0 ref1 and443870 intrusive registration, no Entity
        // count, resolver cache, texture-table row or second native manager.
        cub_->lastPC=0x29c780;
        if(!construction_->registerTexture29C780(lifetimeLink_))return fail(d,E::video,0x29c780);
        registered_=true;d.sourceEffectsCommitted=true;

        // Only direct24E1B0's proven low nibble is known. Upper bits remain
        // unclaimed. Existing generic constructed24DA30 admission stays false.
        flagsF8_=0;flagsF8KnownMask_=0x0f;bitsPerPixelE8_=-1;
        cub_->constructed24E1B0=true;cub_->lastPC=0x24e360;d.sourcePC=0x24e360;return true;
    }catch(...){return fail(d,E::allocation,cub_?cub_->lastPC:0x24e1b0);}
}
bool OriginalStandardTextureResource50LA::loadCub24F2C0(std::size_t& cursor,
    const std::array<std::uint32_t,5>& descriptor,D& d)noexcept {
    d={};
    if(!cub_||!cub_->constructed24E1B0||cub_->loadAttempted||!registered_||
       !cub_->source.palette||!cubSourceCurrent1295B0())return fail(d,E::phase,0x24f2c0);
    auto& c=*cub_;const auto& source=c.source;c.loadAttempted=true;c.loadActive=true;
    struct EndActive {bool& active;~EndActive(){active=false;}} endActive{c.loadActive};
    const auto reject=[&](E e,std::uint32_t pc){c.lastPC=pc;d.sourceEffectsCommitted=registered_;return fail(d,e,pc,cursor);};
    const auto word=[](const std::uint8_t* p){return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8u)|
        (std::uint32_t(p[2])<<16u)|(std::uint32_t(p[3])<<24u);};
    const auto end=source.tileOffset+source.tileBytes;
    if(cursor!=source.tileOffset+24u||word(source.memberData+source.tileOffset)!=source.descriptorWord0)
        return reject(E::source,0x24f2c0);
    for(std::size_t i=0;i<5u;++i)if(descriptor[i]!=word(source.memberData+source.tileOffset+4u+i*4u))
        return reject(E::source,0x24f2c0);
    // Caller already performed128E4C/128EE0..F50 in exact order. No reread
    // advances this bound actual file cursor; comparisons only authenticate.
    if(descriptor!=std::array<std::uint32_t,5>{{32u,0u,16u,4096u,4u}})
        return reject(E::unsupported,0x24f2c0);
    const auto read=[&](void* destination,std::size_t bytes,std::uint32_t pc){
        c.lastPC=pc;if(!cubSourceCurrent1295B0()||cursor>end||bytes>end-cursor)return false;
        std::memcpy(destination,source.memberData+cursor,bytes);cursor+=bytes;c.orderedReadBytes+=bytes;return true;
    };
    flagsF8_=static_cast<std::uint8_t>((flagsF8_&0xfeu)|1u); //24F3A8, source format32→34.
    bitsPerPixelE8_=16; //24F400 actual29E910(0x613).
    width14_=height18_=128;c.fieldA8=16; //24F568/570/574.
    levelCountB0_=0;
    for(std::size_t i=0;i<5u;++i){std::uint8_t raw[4];
        if(!read(raw,4u,i<4u?0x24f5d8:0x24f678))return reject(E::source,c.lastPC);
        c.offsetWords[i]=word(raw);
        if(c.offsetWords[i])return reject(E::unsupported,c.lastPC);
        // Source compressed path overwrites each temporary offset with zero.
        if(i<4u)++levelCountB0_; //24F640 successful-prefix count, before forced1.
    }
    levelCountB0_=1; //24F6EC flags argument0; distinct from four actual allocations.
    for(std::size_t input=0;input<4u;++input){const auto slot=3u-input;auto& mip=c.mips[slot];
        std::uint8_t raw[4];mip.sourceLengthOffset=cursor;
        if(!read(raw,4u,0x24f784))return reject(E::source,c.lastPC);
        const auto bytes=word(raw);mip.bytes11C=static_cast<std::int32_t>(bytes); //24F7B0 before allocation.
        if(bytes<25u||bytes>65536u||cursor>end||bytes>end-cursor)return reject(E::range,0x24f7b0);
        c.lastPC=0x24f7b8;
        auto* allocation=static_cast<std::uint8_t*>(::operator new(bytes,std::align_val_t(64),std::nothrow));
        if(!allocation)return reject(E::allocation,c.lastPC);
        mip.allocation.reset(allocation);mip.allocationBytes=bytes;c.compressedAllocationBytes+=bytes;
        mip.codec118=allocation; //24F7EC publication BEFORE actual read; partial owner retained.
        mip.sourcePayloadOffset=cursor;
        if(!read(allocation,bytes,0x24f814))return reject(E::source,c.lastPC);
        mip.payloadRead=true;
        const std::uint32_t dimension=16u<<input;
        if(std::memcmp(allocation,"ipum",4u)||word(allocation+4u)!=bytes-8u||
           (std::uint32_t(allocation[8])|(std::uint32_t(allocation[9])<<8u))!=dimension||
           (std::uint32_t(allocation[10])|(std::uint32_t(allocation[11])<<8u))!=dimension||
           word(allocation+12u)!=1u)return reject(E::malformed,0x24f82c);
        mip.width104=mip.height110=static_cast<std::int32_t>(dimension);
        mip.width124=static_cast<std::uint16_t>(dimension);
        mip.blocks108=dimension<128u?1:2;
        // Explicit bounded native numeric adaptation for EXACT powers16/32/64/128.
        // No claim of universal original log/div/int helper/FCSR equivalence.
        mip.logWidth10C=mip.logHeight114=static_cast<std::int32_t>(4u+input);
        mip.width12C=mip.width104;mip.height128=mip.height110;
        mip.bytes11C=static_cast<std::int32_t>(bytes-16u);
        mip.pointer100=allocation;mip.codec118=allocation+16u; //24F914..92C.
    }
    if(cursor!=end||!cubSourceCurrent1295B0())return reject(E::source,0x24fc00);
    levelCountB0_=1;formatAC_=1555;
    //24FC18 ->257750(FC,10). Real CPU owner/backing, NOT a second GPU
    // texture or decoded image. Retain pending object on partial ctor failure.
    if(!OriginalTextureBufferAssertions50LA::historyCurrent())return fail(d,E::phase,0x257750);
    if(!OriginalTextureBufferAssertions50LA::reach257750(c.bufferFC==nullptr))return fail(d,E::source,0x2577A4);
    c.lastPC=0x2577d8;
    c.bufferFCOwner.reset(new(std::nothrow) CubBuffer257750());
    if(!c.bufferFCOwner)return reject(E::allocation,c.lastPC);
    c.lastPC=0x2592d0;auto& buffer=*c.bufferFCOwner;
    auto* bytes=static_cast<std::uint8_t*>(::operator new(256u,std::align_val_t(64),std::nothrow));
    if(!bytes)return reject(E::allocation,c.lastPC);
    buffer.backing.reset(bytes);buffer.first00=bytes;
    std::memset(bytes,0,256u); //259348 actual clear, not initialized packet data.
    buffer.capacity08=16u;buffer.current04=buffer.first00;buffer.field0C=buffer.field10=buffer.field14=buffer.field18=buffer.field1C=0;
    c.bufferFC=&buffer; //2577F8 after actual successful constructor.
    c.finalCursor=cursor;c.source.palette=nullptr;
    c.loadComplete24F2C0=true;c.lastPC=0x25000c;d.sourcePC=0x25000c;d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResource50LA::finishCub2516F0(std::uint32_t a1,std::uint32_t a2,
    std::uint32_t a3,D& d)noexcept {
    d={};if(!cub_||!cub_->loadComplete24F2C0||cub_->loadActive||!cubSourceCurrent1295B0()||
       a1!=1u||a2||a3)return fail(d,E::phase,0x2516f0);
    //2516F0 is literal jr ra with return1. No hidden allocation/UV/state call.
    cub_->finished2516F0=true;cub_->lastPC=0x129050;d.sourcePC=0x129050;return true;
}
bool OriginalStandardTextureResource50LA::canRetireCub24E370()const noexcept {
    if(!cub_)return references00_==1&&!registered_&&!constructed_&&!resolver_&&!construction_;
    // Provider revision/context liveness need NOT survive completed/failed
    // source resource teardown. Only actual owned reference/list custody does.
    return references00_==1&&!cub_->loadActive&&(!registered_||
        (construction_&&construction_->cubCanRetire2544F0()&&
         lifetimeLink_.owner_==construction_->textureLifetime_.get()));
}
bool OriginalStandardTextureResource50LA::withdrawCubPaletteBorrow()noexcept {
    if(!canRetireCub24E370())return false;if(cub_)cub_->source.palette=nullptr;return true;
}

#include "original_loading_pst_native50la.inl"

OriginalStandardTextureResource50LA::OriginalStandardTextureResource50LA(AllocationKey) noexcept
    :lifetimeLink_(*this) {}
std::shared_ptr<OriginalStandardTextureResource50LA> OriginalStandardTextureResource50LA::allocate() {
    return std::make_shared<OriginalStandardTextureResource50LA>(AllocationKey{});
}
OriginalStandardTextureResource50LA::~OriginalStandardTextureResource50LA() {
    if(references00_!=0)std::terminate();
    if(loadingPstFile_){
        // Numeric file custody is explicitly closed before reference disposal.
        if(loadingPstFile_->handle_!=-1)std::terminate();
        D nativeClose;if(!closeLoadingPstNative(nativeClose))std::terminate();
    }

    if(cub_){
        if(cub_->loadActive||cub_->source.palette||
           (registered_&&(!construction_||!construction_->cubCanRetire2544F0())))std::terminate();
        //24E3A8 walks physical slots0..4. Failed native reads also retain
        // pre-publication allocation custody, explicitly safe partial cleanup.
        for(auto& mip:cub_->mips){mip.pointer100=mip.codec118=nullptr;mip.allocation.reset();}
        //2544F0 is the proven SAME-manager null13C no-effect branch above.
        //257820/259380 frees actual FC backing then its owned object.
        cub_->bufferFC=nullptr;
        if(cub_->bufferFCOwner)cub_->bufferFCOwner->backing.reset();
        cub_->bufferFCOwner.reset();
        cub_.reset(); //Withdraw actual image/source/provider borrows before manager unlink.
    }
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
        if(loadingPstFile_&&loadingPstFile_->associated_&&!loadingPstFile_->nativeAssociationWithdrawn_){
            D cleared;if(!resolver_->clearLoadingPst1327D0(*loadingPstFile_,*this,cleared))std::terminate();
        }
        resolver_->clear1327D0(this);
        resolver_->finishPstSourceBorrow29C7A0(*this);
    }
}
bool OriginalStandardTextureResolver50LA::acquireLoadingRa24DA30(
    std::shared_ptr<const OriginalLoadingTextureSource50LA> source,std::size_t& index,
    UiSelection& capture,D& d)noexcept {
    const bool okay=admitLoadingRaCanonicalNative(std::move(source),index,capture,d);
    if(okay)unjoinedLoadingRaFileHistory_=true;
    return okay;
}
bool OriginalStandardTextureResolver50LA::admitLoadingRaCanonicalNative(
    std::shared_ptr<const OriginalLoadingTextureSource50LA> source,std::size_t& index,
    UiSelection& capture,D& d)noexcept {
    index=static_cast<std::size_t>(-1);capture=UiSelection{};d={};
    if(!source||!storage_||!storage_->provider||!storage_->provider->sealed()||
       source->bytes_.size()!=287772u)return fail(d,E::source,0x24DAF8);
    try {
    const std::string name=R"(UI\Textures\LD_RA.PST)";
    OriginalStandardArchiveProvider50LA::MemberView conflicting;
    if(storage_->provider->lookup132820(name,conflicting)!=OriginalStandardArchiveProvider50LA::Lookup::absent)
        return fail(d,E::source,0x24DAF8); //Do not replace an actual archive winner.
        for(const auto& file:storage_->files)if(!file->retired&&same(file->member,name))
            return fail(d,E::duplicateSource,0x132820); //One-shot Screen staging, no alias admission.
        const auto& bytes=source->bytes_;
        if(u32(bytes,0)!=1||u32(bytes,4)!=1||u32(bytes,8)!=1||
           u32(bytes,0x40c)!=448||u32(bytes,0x410)!=640||u32(bytes,0x414)||u32(bytes,0x418))
            return fail(d,E::source,0x24E880);
        if(storage_->files.size()>=32768||bytes.size()>8u*1024u*1024u||
           storage_->textureBytes>256u*1024u*1024u-bytes.size())return fail(d,E::range,0x24E880);
        auto file=std::make_unique<Storage::File>();
        file->member=name;file->bytes=bytes;file->loadingRa=std::move(source);
        Storage::Entry entry;entry.file=file.get();entry.count=1;entry.kind=1;
        entry.pstBytes=bytes.size();entry.levels[0]={640,448,0x40c,0x41c,640u*448u};
        storage_->files.reserve(storage_->files.size()+1);storage_->entries.reserve(storage_->entries.size()+1);
        const auto row=storage_->entries.size();
        capture.resolver=this;capture.providerIdentity=storage_->provider.get();
        capture.sourceIndex=row;capture.data=file->bytes.data();capture.bytes=file->bytes.size();
        capture.loadingRa=file->loadingRa;
        storage_->files.push_back(std::move(file));storage_->entries.push_back(std::move(entry));
        storage_->textureBytes+=capture.bytes;storage_->texturesPrepared=true;index=row;
        return true;
    }catch(...){capture=UiSelection{};return fail(d,E::allocation,0x24E880);}
}
bool OriginalStandardTextureResolver50LA::loadingRaSourceCurrent(const UiSelection& selected)const noexcept {
    if(!storage_||selected.resolver!=this||!selected.loadingRa||selected.archive||
       selected.member||selected.nativeFlare.backing||!storage_->provider||
       selected.providerIdentity!=storage_->provider.get()||!storage_->provider->sealed()||
       selected.sourceIndex>=storage_->entries.size())return false;
    const auto& file=*storage_->entries[selected.sourceIndex].file;
    OriginalStandardArchiveProvider50LA::MemberView conflicting;
    return !file.retired&&file.loadingRa==selected.loadingRa&&file.bankGeneration==0&&
        file.format==OriginalStandardTextureSourceFormat50LA::pstIndexed&&
        OriginalStandardArchiveProvider50LA::sameName(file.member,R"(UI\Textures\LD_RA.PST)")&&file.bytes==selected.loadingRa->bytes_&&
        selected.data==file.bytes.data()&&selected.bytes==file.bytes.size()&&
        storage_->provider->lookup132820(file.member,conflicting)==OriginalStandardArchiveProvider50LA::Lookup::absent;
}
bool OriginalStandardTextureResolver50LA::nativeFlareSourceCurrent(const UiSelection& source)const noexcept {
    if(!storage_||source.resolver!=this||!source.nativeFlare.backing||!source.nativeFlare.member||
       source.archive||source.member||source.sourceIndex>=storage_->entries.size()||!source.providerRevision||
       !source.bankGeneration||!storage_->provider||source.providerIdentity!=storage_->provider.get()||
       source.mount>=storage_->provider->mounts_.size())return false;
    const auto& bank=*storage_->provider->mounts_[source.mount];
    if(!bank.active||&bank!=source.nativeFlare.mountIdentity||bank.ui||bank.backing!=source.nativeFlare.backing||
       !OriginalStandardArchiveProvider50LA::sameName(bank.name,"TRACKS/RA.RES")||bank.archiveBytes!=4604561u||
       bank.digest!="1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2"||
       bank.generation!=source.bankGeneration||source.ordinal>=bank.rows.size())return false;
    const auto& row=bank.rows[source.ordinal];const auto& file=*storage_->entries[source.sourceIndex].file;
    return !file.retired&&file.format==OriginalStandardTextureSourceFormat50LA::pstIndexed&&
        OriginalStandardArchiveProvider50LA::sameName(file.member,"textures\\flare1.pst")&&
        source.nativeFlare.member==&row.name&&source.nativeFlare.offset==row.offset&&
        row.offset<=bank.backing->size()&&row.size<=bank.backing->size()-row.offset&&
        row.data==bank.backing->data()+row.offset&&source.data==row.data&&source.bytes==row.size&&
        OriginalStandardArchiveProvider50LA::sameName(file.member,row.name)&&file.bytes.size()==row.size&&
        std::equal(file.bytes.begin(),file.bytes.end(),row.data);
}
bool OriginalStandardTextureResource50LA::construct24DA30(
    std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,std::size_t index,
    const std::string& resolvedName,
    std::shared_ptr<const OriginalStandardMaterialContext50LA> context,D& d,std::size_t frame,
    OriginalStandardTextureResolver50LA::UiSelection* selection) noexcept {
    if(resolver_||construction_||constructed_||registered_||firstUiSource_.archive||firstUiSource_.nativeFlare.backing||firstUiSource_.loadingRa)return fail(d,E::phase,0x24DEF8);
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
    if(root.file->loadingRa){
        if(child||!selection||selection->sourceIndex!=index||
           !resolver->loadingRaSourceCurrent(*selection)||entry.kind!=1||entry.count!=1||
           entry.pstOffset||entry.pstBytes!=287772u||!same(resolvedName,root.file->member))return fail(d,E::source,0x24DAF8);
        firstUiSource_=std::move(*selection);
    }else if(selection&&selection->loadingRa)return fail(d,E::source,0x24DAF8);
    if(selection&&selection->nativeFlare.backing){
        if(child||selection->sourceIndex!=index||!resolver->nativeFlareSourceCurrent(*selection)||
           root.file->format!=OriginalStandardTextureSourceFormat50LA::pstIndexed||entry.pstOffset||
           entry.pstBytes!=selection->bytes)return fail(d,E::source,0x24DAF8);
        firstUiSource_=std::move(*selection); //Actual original-positive lookup, before construction effects.
    }
    if(selection&&selection->archive){
        const auto member=selection->archive->member(selection->ordinal);
        if(child||selection->resolver!=resolver.get()||selection->sourceIndex!=index||
           selection->providerIdentity!=resolver->storage_->provider.get()||
           !selection->archive->loaded()||member.identity!=selection->member||
           member.data!=selection->data||member.size!=selection->bytes||
           !member.identity||!member.data||!OriginalStandardArchiveProvider50LA::sameName(member.identity->name,root.file->member)||
           member.size!=root.file->bytes.size()||
           !std::equal(root.file->bytes.begin(),root.file->bytes.end(),member.data)||
           !((root.file->format==OriginalStandardTextureSourceFormat50LA::pstIndexed&&
              !entry.pstOffset&&entry.pstBytes==member.size)||
             (root.file->format==OriginalStandardTextureSourceFormat50LA::atmPstFrames&&
              resultsNativeAtmName50LA(root.file->member)&&!entry.frames.empty())))
            return fail(d,E::source,0x24DAF8);
        // Retain before any actual resource/list effects. Construction failure
        // keeps this owner with the existing partial resource until cleanup.
        firstUiSource_=std::move(*selection);
    }

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
    if(loadingPstFile_){
        if(child||entry.kind!=(loadingPstFile_->raRoot_?1:0)||entry.count!=1||root.file->format!=OriginalStandardTextureSourceFormat50LA::pstIndexed||
           loadingPstFile_->boundResource_!=this||!resolver->loadingPstFileCurrent(*loadingPstFile_,true))
            return fail(d,E::phase,0x24DEF8);
        if(!admitLoadingPstNative(d))return false;
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
        if(!resolver_->beginPstSourceBorrow24E1B0(*this,d))return false;
        if(!construction_->registerTexture29C780(lifetimeLink_))return fail(d,E::video,0x29C780);
        registered_=true;d.sourceEffectsCommitted=true;
        if(loadingPstFile_&&(!loadingPstFc_||!loadingPstFc_->publishNull257740()))
            return fail(d,E::phase,0x24E1D8);
        if(loadingPstFile_&&!initializeLoadingPstFields24E1E0(d))return false;

        //24E310..24E320 clears the binding-policy bit. Neither admitted PST0/1 nor
        //TGA construction takes kind2's24EA40 bit-setting path. Other bits
        //remain unknown; native zero storage is not evidence for their value.
        if(!loadingPstFile_){flagsF8_=0;flagsF8KnownMask_=1;}
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
            OriginalTextureAllocationHistory50LA::invalidateCounterNativeProjection();
            //No 442CD0/guard8A64 write: 257750 is their only reader/writer (SLUS_202.68
            //257760..257784; no other access in .text), so the buffer-assertion history stays known.
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
        if(!loadingPstFile_){
            OriginalTextureAllocationHistory50LA::invalidateCounterNativeProjection();
            OriginalTextureAllocationHistory50LA::invalidateVersionNativeProjection();
            //No 442CD0/guard8A64 write: 257750 is their only reader/writer (SLUS_202.68
            //257760..257784; no other access in .text), so the buffer-assertion history stays known.
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
        if(loadingPstFile_)return constructLoadingPst24E880(d);
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
            if(entry.file->loadingRa){
                // Actual24EE50..24EF1C rejects width640 from the conversion
                // set;24F128..144 clears bit2. No padding or transfer buffer.
                // Unrepresented250580 GS log metadata remains unknown.
                destination.storage=OriginalStandardTextureLevelStorage50LA::pstLoadingLinear8;
                destination.flags30=static_cast<std::uint8_t>(destination.flags30&~4u);
                destination.flags30KnownMask=static_cast<std::uint8_t>(destination.flags30KnownMask|4u);
            }else if(entry.kind==1&&uploadConversion382FC8_!=0){
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
bool OriginalStandardTextureResource50LA::firstUiSourceCurrent()const noexcept {
    const auto& source=firstUiSource_;
    if(!constructed_||aggregate_||atmChild_||references00_<=0||!construction_||!construction_->live()||
       !resolver_||source.resolver!=resolver_.get()||source.sourceIndex!=sourceIndex_||
       !source.archive||!source.archive->loaded()||!source.member||!source.data||!source.bytes||
       !source.providerIdentity||!source.providerRevision||!source.bankGeneration||
       sourceFormat_!=OriginalStandardTextureSourceFormat50LA::pstIndexed||pstOffset_||pstBytes_!=source.bytes)
        return false;
    const auto member=source.archive->member(source.ordinal);
    const auto* bytes=originalBytes();const auto* name=sourceMember();
    return member.identity==source.member&&member.data==source.data&&member.size==source.bytes&&
        name&&OriginalStandardArchiveProvider50LA::sameName(*name,source.member->name)&&bytes&&bytes->size()==source.bytes&&
        std::equal(bytes->begin(),bytes->end(),source.data);
}
bool OriginalStandardTextureResource50LA::borrowLocalFlarePstImage50LA(
    std::shared_ptr<const std::vector<std::uint8_t>>& backing,const std::string*& member,
    const std::uint8_t*& data,std::size_t& bytes)const noexcept {
    if(!constructed_||aggregate_||atmChild_||references00_<=0||!construction_||!construction_->live()||
       !resolver_||sourceFormat_!=OriginalStandardTextureSourceFormat50LA::pstIndexed||
       sourceIndex_!=firstUiSource_.sourceIndex||pstOffset_||pstBytes_!=firstUiSource_.bytes||
       !resolver_->nativeFlareSourceCurrent(firstUiSource_))return false;
    backing=firstUiSource_.nativeFlare.backing;member=firstUiSource_.nativeFlare.member;
    data=firstUiSource_.data;bytes=firstUiSource_.bytes;return true;
}
bool OriginalStandardTextureResource50LA::borrowResultsUiPstImage50LA(
    const OriginalStandardTextureResource50LA& selected,
    std::shared_ptr<const OriginalUiArchive50KG>& archive,OriginalUiMemberView50KG& member,
    std::size_t& offset,std::size_t& extent)const noexcept {
    // The aggregate alone retains its real first132820 archive. Children do
    // not receive a relabeled copy or a later provider winner as provenance.
    const auto& source=firstUiSource_;
    if(!constructed_||atmChild_||references00_<=0||!construction_||!construction_->live()||
       !resolver_||source.resolver!=resolver_.get()||source.sourceIndex!=sourceIndex_||
       !source.archive||!source.archive->loaded()||!source.member||!source.data||!source.bytes||
       !source.providerIdentity||!source.providerRevision||!source.bankGeneration)return false;
    const auto actual=source.archive->member(source.ordinal);
    const auto* bytes=originalBytes();const auto* name=sourceMember();
    if(actual.identity!=source.member||actual.data!=source.data||actual.size!=source.bytes||
       !name||!OriginalStandardArchiveProvider50LA::sameName(*name,source.member->name)||
       !bytes||bytes->size()!=source.bytes||!std::equal(bytes->begin(),bytes->end(),source.data))return false;
    std::size_t begin=0,length=0;
    if(aggregate_){
        if(sourceFormat_!=OriginalStandardTextureSourceFormat50LA::atmPstFrames||
           !resultsNativeAtmName50LA(*name)||
           source.archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)||
           frameCount20_<=0||frames24_.size()!=static_cast<std::size_t>(frameCount20_))return false;
        const auto child=std::find_if(frames24_.begin(),frames24_.end(),
            [&](const auto& held){return held.get()==&selected;});
        if(child==frames24_.end()||!selected.constructed_||!selected.atmChild_||selected.aggregate_||
           selected.references00_<=0||selected.construction_!=construction_||selected.resolver_!=resolver_||
           selected.sourceIndex_!=sourceIndex_||!selected.pstExtent24E880(begin,length))return false;
        const auto ordinal=static_cast<std::size_t>(child-frames24_.begin());
        const auto& root=resolver_->storage_->entries[sourceIndex_];
        if(ordinal>=root.frames.size()||begin!=root.frames[ordinal].pstOffset||length!=root.frames[ordinal].pstBytes)
            return false;
    }else if(&selected!=this||!firstUiSourceCurrent()||!pstExtent24E880(begin,length))return false;
    if(!length||begin>actual.size||length>actual.size-begin)return false;
    archive=source.archive;member=actual;offset=begin;extent=length;return true;
}
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
    if(loadingPstFile_)return sourceKind_==0&&constructed_&&loadingPstNative_&&loadingPstNative_->paletteRead&&
        !loadingPstNative_->nativeClosed&&loadingPstNative_->pendingPalette?
        loadingPstNative_->pendingPalette->data08.get():nullptr;
    return sourceKind()&&sourceKind_==0?resolver_->storage_->entries[sourceIndex_].file->bytes.data()+pstOffset_+12:nullptr;
}
const std::uint8_t* OriginalStandardTextureResource50LA::palette208() const noexcept {
    if(loadingPstFile_)return sourceKind_==1&&constructed_&&loadingPstNative_&&loadingPstNative_->paletteRead&&
        !loadingPstNative_->nativeClosed&&loadingPstNative_->pendingPalette&&
        loadingPstNative_->source208==loadingPstNative_->pendingPalette.get()?loadingPstNative_->pendingPalette->data08.get():nullptr;
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
bool OriginalStandardTextureResource50LA::demandSharedWarning204A84(bool& written,D& d) noexcept {
    written=false;
    if(!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,0x204A84);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,0x204A74);
    // 204A74 is c.le.s(10,current); the source skips the store when true.
    // This changes the same canonical texture field, never an image-bank copy.
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;d.sourceEffectsCommitted=true;}
    d.sourcePC=0x204A84;return true;
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
bool OriginalStandardTextureResource50LA::demandRaceF8199350(
    std::uint32_t storePC,bool& written,D& d) noexcept {
    written=false;
    if(storePC!=0x199414u&&storePC!=0x199454u&&storePC!=0x199494u)
        return fail(d,E::unsupported,storePC);
    if(aggregate_||!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,storePC);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,storePC);
    // C.LE.S(10,current) skips the literal store. No projected-size104A00,
    // image-bank replacement, selected animated frame, reset, or GPU receipt.
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;d.sourceEffectsCommitted=true;}
    d.sourcePC=storePC;return true;
}
bool OriginalStandardTextureResource50LA::demandResults1C15B0(
    std::uint32_t storePC,bool& written,D& d) noexcept {
    written=false;
    if(storePC!=0x1c1658u&&storePC!=0x1c168cu&&storePC!=0x1c16d8u&&storePC!=0x1c175cu)
        return fail(d,E::unsupported,storePC);
    if(aggregate_||!constructed_||references00_<=0||!construction_||!construction_->live())
        return fail(d,E::phase,storePC);
    using Math=detail::StandardTextureDemandMath50LA;
    if(!Math::supported()||!Math::ordinary(demand10_)||demand10_<0.f)
        return fail(d,E::number,storePC);
    // All four source C.LE.S forms skip when10 <= current. No reset,
    // projected-size104A00, animation service or GPU completion follows.
    if(!(10.f<=demand10_)){demand10_=10.f;written=true;d.sourceEffectsCommitted=true;}
    d.sourcePC=storePC;return true;
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
bool OriginalStandardTextureLifetime50LA::resetDemand14D180() noexcept {
    // No public/current caller exists. Do not invalidate a retained partial
    // warning callback if the future254AD0 owner attempts a new frame reset.
    if(nativeWarningDemandOwner_||nativeWarningDemandEpoch_)return false;
    //Preflight source alternatives before any demand mutation. They occupy
    //the SAME source list; never reinterpret them as a prepared resource.
    for(auto* node=head00_;node;node=node->nextA0_)
        if(node->sourceResource_&&(!node->sourceResource_->registered_||!node->sourceResource_->topologyCurrentNative()||!node->sourceResource_->storage_))return false;
    for(cursor08_=head00_;cursor08_;cursor08_=cursor08_->nextA0_){
        if(cursor08_->resource_)cursor08_->resource_->resetDemand14D1A8();
        else if(!cursor08_->sourceResource_||!cursor08_->sourceResource_->resetDemand14D1A8())return false;
    }
    return true;
}
bool OriginalStandardTextureResolver50LA::registerSourceBank12CEA0(
    std::shared_ptr<const OriginalUiArchive50KG> archive,const OriginalStandardArchiveProvider50LA* expectedProvider,const void* owner,SourceBankFamily family,bool& acquired,D& d)noexcept {
    if(freshFileCapacity_&&freshFileCapacity_->waterPoolFileBorrower)return fail(d,E::phase,0);
    if(freshFileCapacity_&&(freshFileCapacity_->streamLibraryOwner||freshFileCapacity_->musicOwner))return fail(d,E::phase,0);
    d={};acquired=false;using Capacity=FreshFileCapacity;
    const auto identity=family==SourceBankFamily::course?OriginalUiArchiveId50KG::course:
        family==SourceBankFamily::loadingFonts?OriginalUiArchiveId50KG::fonts:OriginalUiArchiveId50KG::loadingIcon;
    if(!freshFileCapacity_||!scratch_||!owner||!archive||!archive->loaded()||
       archive->identity()!=originalUiArchiveIdentity50KG(identity))return fail(d,E::source,0x12CEA0);
    auto& state=*freshFileCapacity_;
    const bool retainedRa=loadingRaRetainedCurrent();
    const bool ordinaryFirst=!state.loadingRaOwner&&state.deviceCount18==1;
    const bool afterRa=family==SourceBankFamily::loadingFonts&&retainedRa&&state.deviceCount18==2;
    const bool first=state.courseStage==Capacity::CourseStage::none&&(ordinaryFirst||afterRa)&&state.freeDevice10==-1;
    const bool reused=state.courseStage==Capacity::CourseStage::retired&&
        state.deviceCount18==(retainedRa?3:2)&&state.freeDevice10==state.courseDevice&&
        state.freeDevice10>=0&&state.freeDevice10<state.deviceCount18&&!state.courseNativeBorrowers&&
        ((family==SourceBankFamily::loadingIcon&&state.loadingFontsSourceRetired)||
         (family==SourceBankFamily::course&&state.loadingIconSourceRetired));
    if((family==SourceBankFamily::loadingFonts&&!first)||(family==SourceBankFamily::loadingIcon&&!reused)||
       (family==SourceBankFamily::course&&!first&&!reused))return fail(d,E::phase,0x12CEA0);
    if(!expectedProvider||state.provider.get()!=expectedProvider||(!first&&!reused)||state.registration!=Capacity::Registration::complete||
       state.templateStage!=2||state.templateMember!=-1||state.templateFailed||!state.namesReadAttempted||
       !state.provider||!state.devices14||
       !state.members150||state.memberCount154<=0||state.devices14[0].id00||
       !state.devices14[0].name04||state.devices14[0].flags0C!=0x342||state.read174)
        return fail(d,E::phase,0x12CEA0);
    for(std::int32_t n=0;n<state.memberCount154;++n)if(state.members150[n].flags18&4)return fail(d,E::phase,0x12CEA0);
    // Native exact-occurrence custody BEFORE entering the source call. A mount
    // or its ordinal cannot manufacture this actual source device/member ID.
    state.sourceBankFamily=family;state.courseRootOwner=owner;state.courseArchive=archive;state.courseNativeBorrowers=1;acquired=true;
    state.courseStage=Capacity::CourseStage::attempted;
    state.courseDevice=-1;state.coursePendingMember=-1;state.courseRegistrationSlot=-1;
    state.courseMount=static_cast<std::size_t>(-1);state.courseGeneration=0;state.courseOwnsNativeMount=false;
    // With count18==2 the only live original row is AppStart; the requested
    // fixed LoadingIcon/Course name differs. Do not turn native namespace
    // presence into an original duplicate-bank result.
    if(reused){const auto& free=state.devices14[state.freeDevice10];
        if(free.id00!=state.freeDevice10||free.flags0C!=1||free.name04||free.handle10||free.cursor08||free.length14)
            return fail(d,E::phase,0x12CF20);
    }
    auto selected=scratch_->selected148;
    if(selected!=-1){
        if(selected<0||std::size_t(selected)>=scratch_->slots.size()||!scratch_->slots[selected].pointer00)
            return fail(d,E::phase,0x12CF98);
        auto& slot=scratch_->slots[selected];slot.status08=-1;slot.offset10=-0x6000;scratch_->selected148=-1;
    }else{
        for(std::size_t n=0;n<scratch_->slots.size();++n){auto& slot=scratch_->slots[n];
            if(slot.status08==-2)continue;
            if(!slot.pointer00)return fail(d,E::phase,0x12D004);
            if(slot.status08!=-1){if(slot.status08<0||slot.status08>=state.memberCount154)return fail(d,E::phase,0x12D01C);
                if(state.members150[slot.status08].flags18&4)continue;}
            slot.offset10=-0x6000;slot.status08=-1;selected=static_cast<std::int32_t>(n);break;
        }
        if(selected==-1)return fail(d,E::unsupported,0x12D080); // Preserve reached failed scratch reservation; no source return claimed.
    }
    state.courseRegistrationSlot=selected;
    std::int32_t deviceID=-1;
    if(state.freeDevice10==-1){
        const auto oldCount=state.deviceCount18;++state.deviceCount18; //12D098 BEFORE realloc14.
        auto* grown=new(std::nothrow) Capacity::Device[state.deviceCount18];
        if(!grown){state.failedCourseDevices=state.devices14;state.failedCourseDeviceCapacity=oldCount;state.devices14=nullptr;
            state.courseStage=Capacity::CourseStage::failed;d.sourceEffectsCommitted=true;return fail(d,E::allocation,0x12D0C4);}
        std::memcpy(grown,state.devices14,std::size_t(oldCount)*sizeof(Capacity::Device));delete[] state.devices14;state.devices14=grown;
        deviceID=state.deviceCount18-1;state.freeDevice10=-1;
        auto& made=state.devices14[deviceID];
        made.flags0C=1;made.handle10=nullptr;made.id00=deviceID;made.name04=nullptr;made.cursor08=0;made.length14=0;made.byte24=0;
        // New-row constructor does NOT write payload18/table1C/table20.
    }else{
        if(!fileIoAssertions().reuse81E8_3E1A80.reach(state.freeDevice10<state.deviceCount18,d,0x12D1BC))return false;
        if(state.freeDevice10<0||state.freeDevice10>=state.deviceCount18)return fail(d,E::phase,0x12D1E8);
        deviceID=state.freeDevice10;state.freeDevice10=-1; //12D1E8/F0.
        for(std::int32_t n=1;n<state.deviceCount18;++n){const auto next=(deviceID+n)%state.deviceCount18;
            if(state.devices14[next].flags0C&1){state.freeDevice10=next;break;}}
        if(!fileIoAssertions().reuseNext81EC_3E1A88.reach(state.freeDevice10<state.deviceCount18,d,0x12D2A8))return false;
    }
    state.courseDevice=deviceID;auto& device=state.devices14[deviceID];
    d.sourceEffectsCommitted=true;
    state.courseRegistrationInput.reset(new(std::nothrow) Capacity::Handle(OriginalUiEncodedArchiveView50KG(state.provider.get(),archive)));
    auto& input=state.courseRegistrationInput;
    const auto closeInput=[&]()noexcept{if(input){input->seek(input->input.size());input.reset();}scratch_->selected148=selected;state.courseRegistrationSlot=-1;};
    bool memberWork=false;
    const auto abort=[&](E error,std::uint32_t pc)noexcept{
        if(memberWork&&state.members150)for(std::int32_t n=0;n<state.memberCount154;++n)
            if(state.members150[n].deviceID00==deviceID)state.retireMember12EAA0(n);
        closeInput();state.retireDevice12EB60(device);state.freeDevice10=deviceID;
        state.courseStage=Capacity::CourseStage::failed;return fail(d,error,pc);
    };
    if(!input)return abort(E::allocation,0x13C980);
    std::uint32_t version=0,meta=0,flags=0,user=0,tableBytes=0,count=0,directoryBytes=0;
    if(!input->seek(0)||!input->word(version)||!input->word(meta))return abort(E::source,0x12D318);
    if(version!=3||!input->word(flags)||!input->word(user)||flags!=0x340||user||
       !input->word(tableBytes)||tableBytes<2||(tableBytes&1)||
       std::uint64_t(tableBytes)+20!=std::uint64_t(meta)+8||!input->seek(std::size_t(tableBytes)+20)||
       !input->word(count)||!input->word(directoryBytes)||count!=archive->memberCount()||!count||
       directoryBytes>=Scratch::bytes||directoryBytes>input->input.size()-input->cursor)return abort(E::source,0x12D51C);
    const auto payload=static_cast<std::uint32_t>(input->cursor+directoryBytes),fileLength=static_cast<std::uint32_t>(input->input.size());
    auto& scratch=scratch_->slots[selected];if(!input->read(scratch.pointer00,directoryBytes))return abort(E::source,0x12D58C);
    std::size_t at=0;const auto word=[&](std::uint32_t& value)noexcept{
        if(at>directoryBytes||directoryBytes-at<4)return false;const auto* b=scratch.pointer00+at;at+=4;
        value=std::uint32_t(b[0])|(std::uint32_t(b[1])<<8)|(std::uint32_t(b[2])<<16)|(std::uint32_t(b[3])<<24);return true;};
    for(std::uint32_t ordinal=0;ordinal<count;++ordinal){
        std::uint32_t nameBytes=0,offset=0,length=0;
        if(!word(nameBytes)||!nameBytes||nameBytes>1024||at>directoryBytes||nameBytes>directoryBytes-at)return abort(E::source,0x12E2B0);
        const auto* name=reinterpret_cast<const char*>(scratch.pointer00+at);at+=nameBytes;
        if(!word(offset)||!word(length))return abort(E::source,0x12E2B0);
        const auto original=archive->member(ordinal);
        if(!original.identity||std::strlen(original.identity->name)!=nameBytes||std::memcmp(original.identity->name,name,nameBytes)||
           original.identity->logicalOffset!=offset||original.size!=length)return abort(E::source,0x12E2B0);
        bool duplicate=false;for(std::int32_t n=0;n<state.memberCount154;++n)
            if(state.members150[n].filename08&&courseSourceName308F00(std::string_view(name,nameBytes),state.members150[n].filename08)){duplicate=true;break;}
        if(duplicate)continue;
        memberWork=true;
        if(state.freeMember14C==-1){
            const auto old=state.memberCount154;const auto growth=static_cast<std::int32_t>(count-ordinal);
            auto* members=new(std::nothrow) Capacity::Member[std::size_t(old)+growth];
            if(!members){state.failedCourseMembers=state.members150;state.failedCourseMemberCapacity=old;state.members150=nullptr;
                state.courseStage=Capacity::CourseStage::failed;return fail(d,E::allocation,0x12D6F0);}
            std::memcpy(members,state.members150,std::size_t(old)*sizeof(Capacity::Member));delete[] state.members150;state.members150=members;state.freeMember14C=old;
            for(std::int32_t n=old;n<old+growth;++n){members[n].initialize();++state.memberCount154;}
        }
        const auto index=state.freeMember14C;state.freeMember14C=-1;
        if(index<0||index>=state.memberCount154)return abort(E::phase,0x12E2B0);
        for(std::int32_t n=1;n<state.memberCount154;++n){const auto next=(index+n)%state.memberCount154;
            if(state.members150[next].flags18&1){state.freeMember14C=next;break;}}
        auto& row=state.members150[index];row.flags18=0;state.coursePendingMember=index;
        if(!memberNameAssertion12E4E8(row.filename08,d))return false;
        row.filename08=new(std::nothrow) char[std::size_t(nameBytes)+1];
        if(!row.filename08){state.courseStage=Capacity::CourseStage::failed;return fail(d,E::allocation,0x12E578);}
        std::memcpy(row.filename08,name,nameBytes);row.filename08[nameBytes]=0;
        row.deviceID00=deviceID;row.logicalStart0C=offset;row.cursor10=0;row.length14=length;
        row.auxiliarySlot04=0;row.associatedObject1C=nullptr;row.originalMember=original.identity;state.coursePendingMember=-1;
    }
    if(at!=directoryBytes)return abort(E::source,0x12DAB0);
    closeInput();
    if(!deviceAssertions12DAFC(device.name04,device.flags0C,d))return false;
    device.flags0C=flags|2;device.handle10=nullptr;
    const char* sourceName=family==SourceBankFamily::course?"UI\\UICourse\\UICourse.res":
        family==SourceBankFamily::loadingFonts?"ui\\uifonts.res":"UI\\Textures\\ld_icon.lsi";
    const auto sourceNameBytes=std::strlen(sourceName)+1;
    device.name04=new(std::nothrow) char[sourceNameBytes];
    if(!device.name04){state.courseStage=Capacity::CourseStage::failed;return fail(d,E::allocation,0x12DBF4);}
    std::memcpy(device.name04,sourceName,sourceNameBytes);device.cursor08=0;device.length14=fileLength;
    device.payload18=payload;device.tableBytes20=tableBytes;device.tableOffset1C=20;device.byte24=0;
    state.courseStage=Capacity::CourseStage::registered;
    // Actual Course a2=1: activation and initial compressed-window read occur
    // here BEFORE12CEA0 return/bank2A8. Source ignores both return values;
    // safe native rejection is additional and keeps all reached side effects.
    if(family!=SourceBankFamily::course)return true; // Actual Fonts/LSI a2=0; no eager activation/refill.
    const bool activated=activateCourseDevice12E8B0(deviceID,0,d);
    if(!activated)return false; // Do not call a reader through authentic NULL handle.
    return refillCourseWindow130340(deviceID,0,d);
}

bool OriginalStandardTextureResolver50LA::activateCourseDevice12E8B0(std::int32_t id,std::uint32_t position,D& d)noexcept {
    if(!freshFileCapacity_||!scratch_)return fail(d,E::phase,0x12E8B0);
    auto& s=*freshFileCapacity_;if(!s.devices14||id<0||id>=s.deviceCount18)return fail(d,E::phase,0x12E8B0);
    auto& device=s.devices14[id];
    if(!fileIoAssertions().active8214_3E1AD8.reach(true,d,0x12E908))return false;
    if(s.active[0]!=id){
        std::size_t found=s.active.size();for(std::size_t i=1;i<s.active.size();++i)if(s.active[i]==id){found=i;break;}
        if(found==s.active.size()){
            const auto tail=s.active.back();if(tail!=-1){if(tail<0||tail>=s.deviceCount18)return fail(d,E::phase,0x12E8B0);
                auto& old=s.devices14[tail];if(!(old.flags0C&0x10)&&old.handle10){delete old.handle10;old.handle10=nullptr;}}
            found=s.active.size()-1;
        }
        for(std::size_t i=found;i>0;--i)s.active[i]=s.active[i-1];s.active[0]=id;
        if((device.flags0C&0x100)&&s.current74!=id){
            //13D840 synchronizes the previous immutable-input handle. There is
            // no outstanding native asynchronous operation to invent/complete.
            s.current74=id;s.window7C=s.window80=-1;
        }
    }
    if(!device.handle10){
        if(s.musicOwner&&id==s.musicOwner->device_){
            const auto& owner=*s.musicOwner;
            if(!owner.registered_||owner.nativeClosed_||!owner.input_||!device.name04)return fail(d,E::source,0x13C980);
            device.handle10=new(std::nothrow) FreshFileCapacity::Handle(owner.input_);
        }else if(s.loadingRaOwner&&id==s.loadingRaOwner->device_){
            const auto& owner=*s.loadingRaOwner;
            if(!owner.registered_||owner.nativeClosed_||!owner.source_||!device.name04)return fail(d,E::source,0x13C980);
            device.handle10=new(std::nothrow) FreshFileCapacity::Handle(owner.source_,owner.source_->bytes_.data(),owner.source_->bytes_.size());
        }else{
            const auto archive=s.pod.root&&id==s.pod.device?s.pod.archive:
                s.track.root&&id==s.track.device?s.track.archive:
                id==s.courseDevice?s.courseArchive:id==0?s.appStart:nullptr;
            if(!archive||!device.name04)return fail(d,E::source,0x13C980);
            device.handle10=new(std::nothrow) FreshFileCapacity::Handle(OriginalUiEncodedArchiveView50KG(s.provider.get(),archive));
        }
        if(!device.handle10)return fail(d,E::allocation,0x13C980); // Published NULL; active/current/window prefix remains.
    }
    device.cursor08=position;d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::refillCourseWindow130340(std::int32_t id,std::size_t block,D& d)noexcept {
    if(!freshFileCapacity_||!scratch_)return fail(d,E::phase,0x130340);auto& s=*freshFileCapacity_;
    if(!s.devices14||id<0||id>=s.deviceCount18||s.current74!=id)return fail(d,E::phase,0x130340);
    auto& device=s.devices14[id];if(!device.handle10||(device.flags0C&0x340)!=0x340)return fail(d,E::source,0x130340);
    if(s.window7C==-1){
        delete[] s.lengthTable84;s.lengthTable84=nullptr;
        s.lengthTable84=new(std::nothrow) std::uint32_t[(std::size_t(device.tableBytes20)+3)/4];
        if(!s.lengthTable84)return fail(d,E::allocation,0x130340); // Stop before source read through NULL.
        if(!device.handle10->seek(device.tableOffset1C)||!device.handle10->read(s.lengthTable84,device.tableBytes20))return fail(d,E::source,0x130340);
    }
    if(!s.lengthTable84||device.tableBytes20<2||(device.tableBytes20&1))return fail(d,E::source,0x130340);
    const auto* table=reinterpret_cast<const std::uint8_t*>(s.lengthTable84);
    const auto count=std::size_t(device.tableBytes20)/2;
    const auto length=[&](std::size_t n)noexcept{return std::uint32_t(table[2*n])|(std::uint32_t(table[2*n+1])<<8);};
    if(block+1>=count||!length(block))return fail(d,E::source,0x130340);
    const std::size_t marker=(device.flags0C&0x200)?1:0;std::size_t offset=device.payload18;
    for(std::size_t n=0;n<block;++n){if(!length(n))return fail(d,E::source,0x130340);offset+=length(n)+marker;}
    if(offset>device.length14)return fail(d,E::source,0x130340);
    s.encodedWindowDevice=-1;s.courseWindowBytes=0; // Invalidate native provenance before mutating shared resident buffers.
    if(marker)for(auto& input:s.inputs)if(input.pointer){
        if(!input.lengthWritten)return fail(d,E::phase,0x133230);std::memset(input.pointer,0,input.length);}
    if(!device.handle10->seek(offset))return fail(d,E::source,0x130340);
    const auto requested=std::min<std::size_t>(device.length14-offset,s.total78);
    if(marker)s.read174=1; //133290 actual marker/async-read mode; backend itself is synchronous.
    std::size_t actual=0;
    for(auto& input:s.inputs){if(!input.pointer)continue;if(!input.lengthWritten)return fail(d,E::phase,0x133290);
        const auto amount=std::min<std::size_t>(requested-actual,input.length);
        if(amount&&!device.handle10->read(input.pointer,amount))return fail(d,E::source,0x133290,actual);
        actual+=amount;if(actual==requested)break;
    }
    s.courseWindowBytes=actual;s.encodedWindowDevice=id;s.window7C=static_cast<std::int32_t>(block);s.window80=static_cast<std::int32_t>(block);
    std::int64_t remaining=static_cast<std::int64_t>(actual);std::size_t current=block;
    do {
        if(current+1>=count||!length(current))return fail(d,E::source,0x130340);
        remaining-=std::int64_t(length(current)+marker);++current;
        if(std::int64_t(length(current))<remaining)++s.window80;
    }while(remaining>0);
    // Negative residual is authentic for the last partially resident block.
    // It does not make that partial block readable: window80 excludes it.
    d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::adoptSourceBankNativeMount(const void* root,bool owns,D& d)noexcept {
    d={};if(!freshFileCapacity_)return fail(d,E::phase,0);auto& s=*freshFileCapacity_;
    if(!root||s.courseRootOwner!=root||s.courseStage!=FreshFileCapacity::CourseStage::registered||!s.provider||!s.courseArchive)
        return fail(d,E::phase,0);
    std::size_t match=static_cast<std::size_t>(-1);
    for(std::size_t n=0;n<s.provider->mounts_.size();++n){const auto& bank=*s.provider->mounts_[n];
        if(!bank.active||bank.admittedUi.lock()!=s.courseArchive)continue;
        if(match!=static_cast<std::size_t>(-1))return fail(d,E::source,0);match=n;
    }
    if(match==static_cast<std::size_t>(-1))return fail(d,E::source,0);
    s.courseMount=match;s.courseGeneration=s.provider->mounts_[match]->generation;s.courseOwnsNativeMount=owns;return true;
}
bool OriginalStandardTextureResolver50LA::joinSourceBankCanonical(const void* root,bool ownsNativeMount,D& d)noexcept {
    d={};if(!freshFileCapacity_)return fail(d,E::phase,0x12CEA0);auto& s=*freshFileCapacity_;
    if(!root||s.courseRootOwner!=root||s.courseStage!=FreshFileCapacity::CourseStage::registered||!s.provider||!s.provider->sealed()||!s.members150)
        return fail(d,E::phase,0x12CEA0);
    try {for(std::int32_t n=0;n<s.memberCount154;++n){auto& member=s.members150[n];if(member.deviceID00!=s.courseDevice||!member.filename08)continue;
        const std::string requested(member.filename08);
        OriginalStandardArchiveProvider50LA::MemberView canonical;
        if(s.provider->lookup132820(requested,canonical)!=OriginalStandardArchiveProvider50LA::Lookup::present||
           !canonical.member||!canonical.data||!member.originalMember||canonical.size!=member.length14)return fail(d,E::source,0x132820);
        const auto actual=s.courseArchive->find(requested);
        if(actual.identity!=member.originalMember||!actual.data||actual.size!=canonical.size||std::memcmp(actual.data,canonical.data,canonical.size))return fail(d,E::source,0x132820);
        if(s.courseMount==static_cast<std::size_t>(-1)){s.courseMount=canonical.mount;s.courseGeneration=canonical.bankGeneration;s.courseOwnsNativeMount=ownsNativeMount;}
        else if(s.courseMount!=canonical.mount||s.courseGeneration!=canonical.bankGeneration)return fail(d,E::source,0x132820);
        member.canonicalName=canonical.member;member.canonicalData=canonical.data;
    }
    }catch(...){return fail(d,E::allocation,0);}
    s.courseStage=FreshFileCapacity::CourseStage::joined;return true;
}
bool OriginalStandardTextureResolver50LA::enterCourseArchiveOnly132A20(const void* root,D& d)noexcept {
    d={};if(!freshFileCapacity_||!root||freshFileCapacity_->courseRootOwner!=root||
       freshFileCapacity_->courseStage!=FreshFileCapacity::CourseStage::joined||
       freshFileCapacity_->sourceBankFamily!=SourceBankFamily::course)return fail(d,E::phase,0x132A20);
    freshFileCapacity_->sourceFileMode170=0;freshFileCapacity_->courseArchiveOnlyCallReached=true;d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::borrowSourceBankConfigNative(const void* root,bool acquire,D& d)noexcept {
    d={};if(!freshFileCapacity_)return fail(d,E::phase,0);auto& s=*freshFileCapacity_;
    if(!root||s.courseRootOwner!=root||s.courseStage!=FreshFileCapacity::CourseStage::joined||!s.courseNativeBorrowers)return fail(d,E::phase,0);
    if(acquire){if(s.courseNativeBorrowers==std::numeric_limits<std::size_t>::max())return fail(d,E::allocation,0);++s.courseNativeBorrowers;}
    else {if(s.courseNativeBorrowers<=1)return fail(d,E::phase,0);--s.courseNativeBorrowers;}
    return true;
}
bool OriginalStandardTextureResolver50LA::sourcePodBankCurrentNative(const void* root)const noexcept {
    if(!freshFileCapacity_||!scratch_||!root)return false;
    const auto& s=*freshFileCapacity_;const auto& pod=s.pod;
    // Root can only be issued by the existing private original manager path.
    // This predicate performs no enrollment, mode write, or source search.
    if(pod.root!=root||!pod.attempted||!pod.registered||!pod.joined||pod.failed||
       !pod.archive||!s.provider||!s.devices14||pod.device<0||pod.device>=s.deviceCount18||
       pod.mount>=s.provider->mounts_.size())return false;
    const auto& owner=*pod.root;
    if(owner.thread_!=std::this_thread::get_id()||!owner.attempted_||!owner.acquired_||
       !owner.joined_||owner.nativeClosed_||!owner.mode1Reached_||!owner.occurrence_||
       owner.resolver_.get()!=this||owner.provider_!=s.provider||owner.archive_!=pod.archive)return false;
    const auto& bank=*s.provider->mounts_[pod.mount];
    const auto& device=s.devices14[pod.device];
    return bank.active&&bank.generation==pod.generation&&device.name04&&
        (device.flags0C&0x342u)==0x342u;
}
bool OriginalStandardTextureResolver50LA::openSourceBankMember131230(const void* root,const void* occurrence,
    const std::string& name,std::int32_t& handle,D& d)noexcept {
    d={};if(!freshFileCapacity_||!scratch_||!root||!occurrence||handle!=-1)return fail(d,E::phase,0x131230);auto& s=*freshFileCapacity_;
    if(!((s.courseRootOwner==root&&s.courseStage==FreshFileCapacity::CourseStage::joined)||
         (s.track.root==root&&s.track.joined)||sourcePodBankCurrentNative(root))||!s.members150||
       s.memberCount154<=0||s.cursor168< -1||s.cursor168>=s.memberCount154)return fail(d,E::phase,0x1315B0);
    ++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
    auto index=static_cast<std::int32_t>(s.cursor168);const auto start=index;bool found=false;
    do {const auto& row=s.members150[index];if(row.filename08&&courseSourceName308F00(name,row.filename08)){found=true;break;}
        if(++index==s.memberCount154)index=0;
    }while(index!=start);
    d.sourceEffectsCommitted=true;if(!found)return fail(d,E::unresolved,0x1315B0);
    s.cursor168=index;return openSourceBankMember131380(root,occurrence,index,handle,d);
}
// Factor only the numeric body out of openSourceBankMember131230.
// Its search tail becomes: s.cursor168=index; return openSourceBankMember131380(root,occurrence,index,handle,d);
bool OriginalStandardTextureResolver50LA::openSourceBankMember131380(const void* root,const void* occurrence,
    std::int32_t index,std::int32_t& handle,D& d)noexcept {
    if(!freshFileCapacity_||!scratch_||!root||!occurrence||handle!=-1)return fail(d,E::phase,0x131380);
    auto& s=*freshFileCapacity_;
    if(!((s.courseRootOwner==root&&s.courseStage==FreshFileCapacity::CourseStage::joined)||
         (s.track.root==root&&s.track.joined)||sourcePodBankCurrentNative(root))||!s.members150||
       index<0||index>=s.memberCount154)return fail(d,E::phase,0x131380);
    const auto device=s.pod.root==root?s.pod.device:(s.track.root==root?s.track.device:s.courseDevice);
    // Keep the global rotating search's actual winner. A different bank is
    // an unsupported root here, never skipped in favor of a later POD row.
    if(s.members150[index].deviceID00!=device||!s.members150[index].originalMember)
        return fail(d,E::phase,0x131380);
    return openExactSourceMember131380(occurrence,index,handle,d);
}
bool OriginalStandardTextureResolver50LA::openExactSourceMember131380(const void* occurrence,
    std::int32_t index,std::int32_t& handle,D& d)noexcept {
    if(!freshFileCapacity_||!scratch_||!occurrence||handle!=-1)return fail(d,E::phase,0x131380);
    auto& s=*freshFileCapacity_;
    if(!s.members150||index<0||index>=s.memberCount154)return fail(d,E::phase,0x131380);
    auto& member=s.members150[index];
    if(!fileIoAssertions().open8270_3E1B90.reach(!(member.flags18&4),d,0x1313F4))return false;
    if(member.courseConfigOwner||member.flags18&4||(!member.originalMember&&!member.loadingRaOwner&&!member.musicOwner)||!member.canonicalData)
        return fail(d,E::phase,0x131380);
    member.flags18|=4;member.cursor10=0;
    member.courseConfigOwner=occurrence;handle=index;
    const auto finishOpen=[&]()noexcept{if(s.sourceOpenFlags158)member.flags18|=8;return true;}; //131578 after scratch selection.
    auto selected=member.auxiliarySlot04;
    if(selected>=0&&std::size_t(selected)<scratch_->slots.size()&&scratch_->slots[selected].status08==index){
        if(scratch_->selected148==selected)scratch_->selected148=-1;return finishOpen();
    }
    selected=scratch_->selected148;
    if(selected!=-1){
        if(selected<0||std::size_t(selected)>=scratch_->slots.size()||!scratch_->slots[selected].pointer00)return fail(d,E::phase,0x131380);
    }else{
        for(std::size_t n=0;n<scratch_->slots.size();++n){const auto& slot=scratch_->slots[n];if(slot.status08==-2)continue;
            if(!slot.pointer00)return fail(d,E::phase,0x131380);
            if(slot.status08!=-1){if(slot.status08<0||slot.status08>=s.memberCount154)return fail(d,E::phase,0x131380);
                if(s.members150[slot.status08].flags18&4)continue;}
            selected=static_cast<std::int32_t>(n);break;
        }
    }
    member.auxiliarySlot04=selected;
    if(selected==-1){
        //Actual131554 diagnostic precedes131578. Water Config, Pool file and
        //the distinct Announcer occurrence do not inherit the older adapter's
        //omitted diagnostic completion.
        if((s.waterConfigBorrower&&occurrence==s.waterConfigBorrower)||
           (s.waterPoolFileBorrower&&occurrence==s.waterPoolFileBorrower)||
           (s.musicOwner&&occurrence==s.musicOwner->config_&&
            s.musicOwner->occurrence_==OriginalMusicSourceFile50LA::Occurrence::announcerPercentages2C5164))
            return fail(d,E::unsupported,0x131554);
        return finishOpen(); //Existing other domains retain their established scope.
    }
    auto& slot=scratch_->slots[selected];slot.status08=index;slot.offset10=-0x6000;scratch_->selected148=-1;return finishOpen();
}
bool OriginalStandardTextureResolver50LA::sourceBankMemberCurrent(const void* root,const void* occurrence,std::int32_t handle)const noexcept {
    if(!freshFileCapacity_||!scratch_||!root||!occurrence)return false;const auto& s=*freshFileCapacity_;
    if(!((s.courseRootOwner==root&&s.courseStage==FreshFileCapacity::CourseStage::joined)||
         (s.track.root==root&&s.track.joined)||sourcePodBankCurrentNative(root)||streamLibraryRootCurrent(root))||!s.members150||handle<0||handle>=s.memberCount154)return false;
    if(s.track.root==root){
        if(!s.provider||s.track.mount>=s.provider->mounts_.size())return false;
        const auto& bank=*s.provider->mounts_[s.track.mount];
        if(!bank.active||bank.generation!=s.track.generation)return false;
    }
    const auto device=s.streamLibraryOwner==root?0:s.pod.root==root?s.pod.device:(s.track.root==root?s.track.device:s.courseDevice);
    if(s.streamLibraryOwner==root&&(s.streamLibraryOwner->handle_!=handle||s.streamLibraryOwner->occurrence_!=occurrence))return false;
    const auto& member=s.members150[handle];return member.courseConfigOwner==occurrence&&member.deviceID00==device&&
        (member.flags18&4)&&member.cursor10<=member.length14&&member.originalMember&&member.filename08;
}
bool OriginalStandardTextureResolver50LA::readSourceBankMember130560(const void* root,const void* occurrence,std::int32_t handle,
    void* output,std::size_t requested,std::size_t& copied,bool advance,D& d)noexcept {
    d={};copied=0;if(!sourceBankMemberCurrent(root,occurrence,handle))return fail(d,E::phase,0x12FB30);
    auto& s=*freshFileCapacity_;auto& member=s.members150[handle];const auto amount=std::min<std::size_t>(requested,member.length14-member.cursor10);
    //12FEAC..12FEE0 returns0 without touching destination when actual amount
    //is0 (zero request or EOF), even forNULL. Positive copying still requires
    //a genuine native destination; never fake a NULL allocation as readsuccess.
    if(!output&&amount)return fail(d,E::unsupported,0x12FEF8);
    if(!activateCourseDevice12E8B0(member.deviceID00,member.logicalStart0C+member.cursor10,d))return false;
    auto& device=s.devices14[member.deviceID00];
    if(member.auxiliarySlot04<0)return fail(d,E::unsupported,0x130560); // Actual temporary-scratch allocation branch not joined.
    if(std::size_t(member.auxiliarySlot04)>=scratch_->slots.size())return fail(d,E::phase,0x130560);
    auto& slot=scratch_->slots[member.auxiliarySlot04];if(!slot.pointer00||slot.status08!=handle)return fail(d,E::phase,0x130560);
    auto* destination=static_cast<std::uint8_t*>(output);
    while(copied<amount){const auto block=std::size_t(device.cursor08)/Scratch::bytes;const auto blockStart=block*Scratch::bytes;
        if(device.cursor08<std::uint32_t(slot.offset10)||std::uint64_t(device.cursor08)-std::int64_t(slot.offset10)>=Scratch::bytes){
            if(s.window7C<0||block<std::size_t(s.window7C)||block>std::size_t(s.window80))
                if(!refillCourseWindow130340(member.deviceID00,block,d))return false;
            const auto* table=reinterpret_cast<const std::uint8_t*>(s.lengthTable84);const auto count=std::size_t(device.tableBytes20)/2;
            if(!table||block+1>=count)return fail(d,E::source,0x130560,copied);
            const auto length=[&](std::size_t n)noexcept{return std::uint32_t(table[2*n])|(std::uint32_t(table[2*n+1])<<8);};
            const std::size_t marker=(device.flags0C&0x200)?1:0;std::size_t relative=0;
            for(std::size_t n=std::size_t(s.window7C);n<block;++n)relative+=length(n)+marker;
            const auto encodedLength=length(block);if(!encodedLength||encodedLength>Scratch::bytes||
               s.encodedWindowDevice!=member.deviceID00||relative>s.courseWindowBytes||encodedLength+marker>s.courseWindowBytes-relative)return fail(d,E::source,0x133330,copied);
            //130B54..130BA8 checks the outstanding read before the marker.
            //13D880 returns native device completion and writes error/progress.
            // Our immutable synchronous encoded-input backend has completed
            // the actual requested bytes: completion0,error0, no retry fiction.
            if(marker&&s.read174)s.read174=0;
            std::array<std::uint8_t,Scratch::bytes+1> encoded{};std::size_t skip=relative,got=0;
            for(const auto& input:s.inputs){if(!input.pointer)continue;if(!input.lengthWritten)return fail(d,E::phase,0x133330,copied);
                if(skip>=input.length){skip-=input.length;continue;}
                const auto n=std::min<std::size_t>(encodedLength+marker-got,input.length-skip);
                std::memcpy(encoded.data()+got,input.pointer+skip,n);got+=n;skip=0;if(got==encodedLength+marker)break;}
            if(got!=encodedLength+marker||(marker&&encoded[encodedLength]!=0xff))return fail(d,E::source,0x133580,copied);
            std::size_t absolute=device.payload18;for(std::size_t n=0;n<block;++n)absolute+=length(n)+marker;
            ResourceBlock48C actual;if(!device.handle10->input.block(block,actual)||actual.compressed_bytes!=encodedLength||actual.compressed_offset!=absolute)return fail(d,E::source,0x133330,copied);
            slot.offset10=static_cast<std::int32_t>(blockStart); // Authentic pre-codec stamp; failure retains it and partial bytes.
            if(encodedLength==Scratch::bytes)std::memcpy(slot.pointer00,encoded.data(),Scratch::bytes);
            else {z_stream z{};const auto setup=inflateInit(&z);if(setup!=Z_OK)return fail(d,E::allocation,0x2B0780,copied);
                z.next_in=encoded.data();z.avail_in=encodedLength;z.next_out=slot.pointer00;z.avail_out=Scratch::bytes;
                const auto result=inflate(&z,Z_FINISH);const bool okay=result==Z_STREAM_END&&z.total_in==encodedLength&&z.total_out==Scratch::bytes&&z.avail_in==0;inflateEnd(&z);
                if(!okay)return fail(d,result==Z_MEM_ERROR?E::allocation:E::source,0x2B0780,copied);}
        }
        const auto within=std::size_t(device.cursor08)-blockStart;const auto n=std::min<std::size_t>(amount-copied,Scratch::bytes-within);
        std::memcpy(destination+copied,slot.pointer00+within,n);copied+=n;device.cursor08+=static_cast<std::uint32_t>(n);
    }
    if(advance)member.cursor10+=static_cast<std::uint32_t>(copied);d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::readSourceBankMember12FB30(const void* root,const void* occurrence,std::int32_t handle,
    void* output,std::size_t requested,std::size_t& done,D& d)noexcept {
    if(!sourceBankMemberCurrent(root,occurrence,handle))return fail(d,E::phase,0x12FB30);
    if(!readAssertions12FB30(handle,freshFileCapacity_->memberCount154,freshFileCapacity_->members150[handle].flags18,d))return false;
    return readSourceBankMember130560(root,occurrence,handle,output,requested,done,true,d);
}
bool OriginalStandardTextureResolver50LA::readSourceBankLine12EE90(const void* root,const void* occurrence,std::int32_t handle,
    std::array<char,1024>& output,bool& present,std::size_t& position,D& d)noexcept {
    d={};present=false;position=0;if(!sourceBankMemberCurrent(root,occurrence,handle))return fail(d,E::phase,0x12EE90);
    if(!lineAssertions12EE90(handle,freshFileCapacity_->memberCount154,d))return false;
    auto& member=freshFileCapacity_->members150[handle];const auto amount=std::min<std::size_t>(member.length14-member.cursor10,output.size()-2);
    if(!amount){output[0]=0;position=member.cursor10;return true;}
    std::size_t done=0;if(!readSourceBankMember130560(root,occurrence,handle,output.data(),amount,done,false,d))return false;
    if(done!=amount)return fail(d,E::source,0x12EE90);output[amount]=0;std::size_t end=0;while(end<amount&&output[end]&&output[end]!='\n')++end;
    if(end<amount&&output[end]=='\n'){member.cursor10+=static_cast<std::uint32_t>(end+1);
        if(end&&output[end-1]=='\r'){output[end-1]='\n';output[end]=0;}else output[end+1]=0;
    }else member.cursor10+=static_cast<std::uint32_t>(amount);
    present=true;position=member.cursor10;return true;
}
bool OriginalStandardTextureResolver50LA::tellSourceBankMember12F520(const void* root,const void* occurrence,std::int32_t handle,std::size_t& position,D& d)noexcept {
    d={};if(!sourceBankMemberCurrent(root,occurrence,handle))return fail(d,E::phase,0x12F520);
    if(!tellAssertions12F520(handle,freshFileCapacity_->memberCount154,d))return false;
    position=freshFileCapacity_->members150[handle].cursor10;return true;
}
bool OriginalStandardTextureResolver50LA::seekSourceBankMember12F640(const void* root,const void* occurrence,std::int32_t handle,std::size_t position,D& d)noexcept {
    d={};if(!sourceBankMemberCurrent(root,occurrence,handle)||position>freshFileCapacity_->members150[handle].length14)return fail(d,E::phase,0x12F640);
    if(!seekAssertions12F640(handle,freshFileCapacity_->memberCount154,d))return false;
    freshFileCapacity_->members150[handle].cursor10=static_cast<std::uint32_t>(position);d.sourceEffectsCommitted=true;return true;
}

bool OriginalStandardTextureResolver50LA::closeSourceBankMember131040(const void* root,const void* occurrence,
    std::int32_t handle,bool flush,D& d)noexcept {
    d={};if(!freshFileCapacity_||!scratch_||!root||!occurrence)return fail(d,E::phase,0x131040);
    auto& s=*freshFileCapacity_;
    if(flush&&!fileAssertions12F290(handle,s.memberCount154,d))return false;
    if(!closeAssertions131040(handle,s.memberCount154,d))return false;
    if(!sourceBankMemberCurrent(root,occurrence,handle))return fail(d,E::phase,0x131040);
    auto& member=s.members150[handle];
    if(!s.devices14||member.deviceID00<0||member.deviceID00>=s.deviceCount18)return fail(d,E::phase,0x1311A0);
    auto& device=s.devices14[member.deviceID00];
    if(!(device.flags0C&2))return fail(d,E::unsupported,0x1311BC);
    //13D9E0 reached by12F290 is the literal no-op returning zero. The actual
    //131040 archive close below does NOT retire the bank handle or its window.
    member.cursor10=0;member.flags18&=~std::uint32_t(4);
    if(member.auxiliarySlot04>=0&&std::size_t(member.auxiliarySlot04)<scratch_->slots.size()&&
       scratch_->slots[member.auxiliarySlot04].status08==handle)scratch_->selected148=member.auxiliarySlot04;
    member.courseConfigOwner=nullptr;d.sourceEffectsCommitted=true;return true;
}
bool OriginalStandardTextureResolver50LA::retireSourceBankNative(const void* root,D& d)noexcept {
    if(freshFileCapacity_&&freshFileCapacity_->waterPoolFileBorrower)return fail(d,E::phase,0);
    d={};if(!freshFileCapacity_||!root)return fail(d,E::phase,0);
    auto& s=*freshFileCapacity_;if(s.streamLibraryOwner||s.musicOwner||s.particleConfigBorrower||s.sourceSkyFileBorrower||s.waterConfigBorrower)return fail(d,E::phase,0);if(s.courseStage==FreshFileCapacity::CourseStage::none&&!s.courseRootOwner)return true;
    if(s.courseRootOwner!=root)return fail(d,E::phase,0);
    if(s.courseStage==FreshFileCapacity::CourseStage::retired)return true;
    if(s.courseNativeBorrowers!=1)return fail(d,E::phase,0);
    // Native abort admission, not an original forced-bank destructor receipt.
    // Prove actual canonical resources gone, including prior generations. A
    // zero Fresh associatedObject lane alone is NOT a resource-owner proof.
    if(s.courseMount!=static_cast<std::size_t>(-1)){
        if(!s.provider||s.courseMount>=s.provider->mounts_.size())return fail(d,E::source,0);
        const auto& bank=*s.provider->mounts_[s.courseMount];
        if(!bank.active||bank.generation!=s.courseGeneration||bank.admittedUi.lock()!=s.courseArchive)return fail(d,E::source,0);
        if(storage_&&storage_->provider){
            if(storage_->provider.get()!=s.provider.get()||!validateGarageBankRows12EAA0(s.courseMount,s.courseGeneration,d))return false;
            for(const auto& file:storage_->files)if(file->bank==s.courseMount){
                if(file->pstBorrowers||!file->cached.expired()||file->animation||!file->geometry.expired()||
                   !file->payloadGeometry.expired()||!file->collision.expired()||!file->ipumImages.expired()||file->garageIpum)
                    return fail(d,E::phase,0x12EAA0);
                if(!file->retired&&file->bankGeneration!=s.courseGeneration)return fail(d,E::source,0x12EAA0);
            }
        }else if(storage_&&!storage_->files.empty())return fail(d,E::source,0);
        if(s.courseOwnsNativeMount&&s.provider->revision_==std::numeric_limits<std::uint64_t>::max())return fail(d,E::phase,0);
    }
    auto* members=s.members150?s.members150:s.failedCourseMembers;
    const auto memberCount=s.members150?s.memberCount154:s.failedCourseMemberCapacity;
    if(!members&&memberCount)return fail(d,E::phase,0);
    for(std::int32_t n=0;n<memberCount;++n)if((s.courseDevice>=0&&members[n].deviceID00==s.courseDevice)||n==s.coursePendingMember){
        if(members[n].courseConfigOwner||(members[n].flags18&4)||members[n].associatedObject1C)return fail(d,E::phase,0x12EAA0);
    }
    // All refusal gates precede withdrawal. No source allocation or callback is
    // fabricated here; failed source realloc pointers remain NULL historically.
    if(storage_&&s.courseMount!=static_cast<std::size_t>(-1))for(auto& file:storage_->files)if(file->bank==s.courseMount&&file->bankGeneration==s.courseGeneration&&!file->retired){
        file->cached.reset();file->cacheIdentity=nullptr;file->animation=nullptr;
        file->geometry.reset();file->geometryIdentity=nullptr;file->collision.reset();file->collisionIdentity=nullptr;
        file->ipumImages.reset();file->retired=true;
    }
    for(std::int32_t n=0;n<memberCount;++n)if((s.courseDevice>=0&&members[n].deviceID00==s.courseDevice)||n==s.coursePendingMember){
        if(s.members150)s.retireMember12EAA0(n);
        else { // Unpublished old native allocation retained after realloc NULL.
            auto& row=members[n];
            if(row.auxiliarySlot04>=0&&std::size_t(row.auxiliarySlot04)<scratch_->slots.size()&&scratch_->slots[row.auxiliarySlot04].status08==n){
                scratch_->slots[row.auxiliarySlot04].status08=-1;scratch_->selected148=row.auxiliarySlot04;}
            delete[] row.filename08;row.filename08=nullptr;row.originalMember=nullptr;row.canonicalName=nullptr;row.canonicalData=nullptr;
            row.flags18=1;row.deviceID00=0;row.courseConfigOwner=nullptr;
        }
    }
    if(s.courseRegistrationInput){s.courseRegistrationInput->seek(s.courseRegistrationInput->input.size());s.courseRegistrationInput.reset();}
    if(s.courseRegistrationSlot>=0){scratch_->selected148=s.courseRegistrationSlot;s.courseRegistrationSlot=-1;}
    if(s.courseDevice>=0&&s.devices14&&s.courseDevice<s.deviceCount18)s.retireDevice12EB60(s.devices14[s.courseDevice]);
    if(s.courseMount!=static_cast<std::size_t>(-1)&&s.courseOwnsNativeMount){
        s.provider->mounts_[s.courseMount]->active=false;s.provider->mounts_[s.courseMount]->admittedUi.reset();++s.provider->revision_;
    }
    s.coursePendingMember=-1;s.courseNativeBorrowers=0;s.courseStage=FreshFileCapacity::CourseStage::retired;
    s.courseArchive.reset();return true; // Native abort only, not retail12E5E0 completion.
}

bool OriginalStandardTextureResolver50LA::sourceBankFamilyCurrent(const void* root,SourceBankFamily family)const noexcept {
    return freshFileCapacity_&&root&&freshFileCapacity_->courseRootOwner==root&&freshFileCapacity_->sourceBankFamily==family;
}
bool OriginalStandardTextureResolver50LA::registerCourseBank12CEA0(std::shared_ptr<const OriginalUiArchive50KG> archive,
    const OriginalStandardArchiveProvider50LA* provider,const void* owner,bool& acquired,D& d)noexcept {
    return registerSourceBank12CEA0(std::move(archive),provider,owner,SourceBankFamily::course,acquired,d);
}
bool OriginalStandardTextureResolver50LA::adoptCourseNativeMount(const void* root,bool owns,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::course))return fail(d,E::phase,0);
    return adoptSourceBankNativeMount(root,owns,d);
}
bool OriginalStandardTextureResolver50LA::joinCourseBankCanonical(const void* root,bool ownsNativeMount,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::course))return fail(d,E::phase,0);
    return joinSourceBankCanonical(root,ownsNativeMount,d);
}
bool OriginalStandardTextureResolver50LA::borrowCourseConfigNative(const void* root,bool acquire,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::course))return fail(d,E::phase,0);
    return borrowSourceBankConfigNative(root,acquire,d);
}
bool OriginalStandardTextureResolver50LA::openCourseConfig131230(const void* root,const void* occurrence,
    const std::string& name,std::int32_t& handle,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::course))return fail(d,E::phase,0);
    return freshFileCapacity_->courseArchiveOnlyCallReached&&freshFileCapacity_->sourceFileMode170==0&&openSourceBankMember131230(root,occurrence,name,handle,d);
}
bool OriginalStandardTextureResolver50LA::courseConfigFileCurrent(const void* root,const void* occurrence,std::int32_t handle)const noexcept {
    return sourceBankFamilyCurrent(root,SourceBankFamily::course)&&sourceBankMemberCurrent(root,occurrence,handle);
}
bool OriginalStandardTextureResolver50LA::readCourseConfig12FB30(const void* root,const void* occurrence,std::int32_t handle,
    void* output,std::size_t requested,std::size_t& done,D& d)noexcept {
    done=0;d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::course))return fail(d,E::phase,0);
    return readSourceBankMember12FB30(root,occurrence,handle,output,requested,done,d);
}
bool OriginalStandardTextureResolver50LA::readCourseConfig130560(const void* root,const void* occurrence,std::int32_t handle,
    void* output,std::size_t requested,std::size_t& copied,bool advance,D& d)noexcept {
    copied=0;d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::course))return fail(d,E::phase,0);
    return readSourceBankMember130560(root,occurrence,handle,output,requested,copied,advance,d);
}
bool OriginalStandardTextureResolver50LA::readCourseConfigLine12EE90(const void* root,const void* occurrence,std::int32_t handle,
    std::array<char,1024>& output,bool& present,std::size_t& position,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::course))return fail(d,E::phase,0);
    return readSourceBankLine12EE90(root,occurrence,handle,output,present,position,d);
}
bool OriginalStandardTextureResolver50LA::tellCourseConfig12F520(const void* root,const void* occurrence,std::int32_t handle,std::size_t& position,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::course))return fail(d,E::phase,0);
    return tellSourceBankMember12F520(root,occurrence,handle,position,d);
}
bool OriginalStandardTextureResolver50LA::seekCourseConfig12F640(const void* root,const void* occurrence,std::int32_t handle,std::size_t position,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::course))return fail(d,E::phase,0);
    return seekSourceBankMember12F640(root,occurrence,handle,position,d);
}
bool OriginalStandardTextureResolver50LA::closeCourseConfig131040(const void* root,const void* occurrence,
    std::int32_t handle,bool flush,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::course))return fail(d,E::phase,0);
    return closeSourceBankMember131040(root,occurrence,handle,flush,d);
}
bool OriginalStandardTextureResolver50LA::retireCourseBankNative(const void* root,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::course))return fail(d,E::phase,0);
    return retireSourceBankNative(root,d);
}

OriginalLoadingFontSource50LA::~OriginalLoadingFontSource50LA()noexcept {
    if(acquired_&&!sourceRetired_&&!nativeClosed_)std::terminate();
}
bool OriginalLoadingFontSource50LA::mount1CAAE4(std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider,std::shared_ptr<const OriginalUiArchive50KG> archive,
    OriginalStandardTextureResolver50LA::LoadingFontsBankLease& lease,D& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id()||attempted_||!resolver||!provider||!archive)return fail(d,E::phase,0x1CAAE4);
    resolver_=std::move(resolver);provider_=std::move(provider);archive_=std::move(archive);attempted_=true;
    return resolver_->mountLoadingFontSource1CAAE4(*this,lease,d);
}
bool OriginalStandardTextureResolver50LA::mountLoadingFontSource1CAAE4(OriginalLoadingFontSource50LA& owner,
    LoadingFontsBankLease& lease,D& d)noexcept {
    d={};if(owner.resolver_.get()!=this||owner.thread_!=std::this_thread::get_id()||!owner.attempted_||owner.acquired_)
        return fail(d,E::phase,0x12CEA0);
    // Native namespace acquisition retains its real admitted identity. This
    // lease neither manufactures a FileManager device nor means12CEA0 returned.
    if(!issueLoadingFontsBank1CAAE4(owner.provider_,owner.archive_,lease,d))return false;
    owner.archive_=lease.bank_.archive_;
    // Actual preceding loose LD_RA keeps its device/member after successful
    // cache association and1311DC close. Native-only LD_RA projection cannot
    // certify the untouched one-device APPSTART predecessor for Fonts.
    if(unjoinedLoadingRaFileHistory_)return fail(d,E::phase,0x12CEA0);
    if(!registerSourceBank12CEA0(owner.archive_,owner.provider_.get(),&owner,SourceBankFamily::loadingFonts,owner.acquired_,d))return false;
    if(!adoptSourceBankNativeMount(&owner,false,d)||!joinSourceBankCanonical(&owner,false,d))return false;
    owner.joined_=true;return true;
}
bool OriginalLoadingFontSource50LA::currentCel(const void* occurrence)const noexcept {
    return thread_==std::this_thread::get_id()&&joined_&&!sourceRetired_&&!nativeClosed_&&occurrence&&
        celOccurrence_==occurrence&&resolver_&&resolver_->sourceBankFamilyCurrent(this,OriginalStandardTextureResolver50LA::SourceBankFamily::loadingFonts)&&
        resolver_->sourceBankMemberCurrent(this,occurrence,handle_);
}
bool OriginalLoadingFontSource50LA::beginCel281890(const void* occurrence,const std::string& name,
    OriginalUiMemberView50KG expected,D& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id()||!joined_||nativeClosed_||sourceRetired_||!occurrence||celOccurrence_||handle_!=-1||
       !archive_||!provider_||!resolver_||!resolver_->sourceBankFamilyCurrent(this,OriginalStandardTextureResolver50LA::SourceBankFamily::loadingFonts))
        return fail(d,E::phase,0x281890);
    if(!courseSourceName308F00(name,"font\\denmrk16.cel")&&!courseSourceName308F00(name,"font\\venus16.cel"))return fail(d,E::source,0x281890);
    const auto actual=archive_->find(name);OriginalStandardArchiveProvider50LA::MemberView found;
    if(!actual.identity||actual.identity!=expected.identity||actual.data!=expected.data||actual.size!=expected.size||
       provider_->lookup132820(name,found)!=OriginalStandardArchiveProvider50LA::Lookup::present||!found.data||
       found.size!=expected.size||std::memcmp(found.data,expected.data,found.size))return fail(d,E::source,0x281890);
    cel_=expected;celOccurrence_=occurrence;coldCelAttempted_=true;
    // On a retained partial open the genuine handle remains available to native
    // abort. A not-found original return keeps handle=-1, never a fabricated ID.
    return resolver_->openSourceBankMember131230(this,occurrence,name,handle_,d);
}
bool OriginalLoadingFontSource50LA::readCel12FB30(const void* occurrence,void* output,std::size_t requested,std::size_t& done,D& d)noexcept {
    done=0;d={};if(!currentCel(occurrence))return fail(d,E::phase,0x12FB30);
    return resolver_->readSourceBankMember12FB30(this,occurrence,handle_,output,requested,done,d);
}
bool OriginalLoadingFontSource50LA::eofCel12F3E0(const void* occurrence,bool& atEnd,D& d)noexcept {
    atEnd=false;d={};if(!currentCel(occurrence))return fail(d,E::phase,0x12F3E0);
    const auto& state=*resolver_->freshFileCapacity_;
    if(!fileIoAssertions().eof8230_3E1B10.reach(handle_>=0,d,0x12F430)||
       !fileIoAssertions().eof8234_3E1B18.reach(handle_<state.memberCount154,d,0x12F49C))return false;
    const auto& row=state.members150[handle_];atEnd=row.cursor10>=row.length14;return true;
}
bool OriginalLoadingFontSource50LA::closeCel131040(const void* occurrence,D& d)noexcept {
    d={};if(!currentCel(occurrence))return fail(d,E::phase,0x131040);
    const auto ordinal=cel_.identity&&courseSourceName308F00(cel_.identity->name,"font\\denmrk16.cel")?0u:
        cel_.identity&&courseSourceName308F00(cel_.identity->name,"font\\venus16.cel")?1u:2u;
    if(ordinal>=2||sourceClosedCelEntries_[ordinal])return fail(d,E::phase,0x131040);
    if(!resolver_->closeSourceBankMember131040(this,occurrence,handle_,false,d))return false;
    sourceClosedCelEntries_[ordinal]=occurrence;sourceClosedCels_[ordinal]=cel_;
    handle_=-1;celOccurrence_=nullptr;cel_={};return true;
}
bool OriginalLoadingFontSource50LA::abortCelNative(const void* occurrence,D& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id()||!occurrence||celOccurrence_!=occurrence)return fail(d,E::phase,0);
    if(handle_!=-1&&!resolver_->closeSourceBankMember131040(this,occurrence,handle_,false,d))return false;
    handle_=-1;celOccurrence_=nullptr;cel_={};d.sourcePC=0;return true;
}
bool OriginalLoadingFontSource50LA::recordRegistryHit2822F0(const std::shared_ptr<const void>& occurrence,const std::string& name,
    OriginalUiMemberView50KG expected,D& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id()||!joined_||sourceRetired_||nativeClosed_||!occurrence||celOccurrence_||handle_!=-1||!archive_||!provider_)
        return fail(d,E::phase,0x2822F0);
    const auto ordinal=courseSourceName308F00(name,"font\\denmrk16.cel")?0u:
        courseSourceName308F00(name,"font\\venus16.cel")?1u:2u;
    if(ordinal>=2||registryReceipts_[ordinal].kind!=Completion::unknown||(ordinal&&registryReceipts_[0].entry.expired()))return fail(d,E::phase,0x2822F0);
    const auto actual=archive_->find(name);OriginalStandardArchiveProvider50LA::MemberView found;
    // A retained registry Entry may come from an earlier authenticated archive
    // allocation, so require exact catalog identity and bytes, not allocation equivalence.
    if(!actual.identity||actual.identity!=expected.identity||!expected.data||actual.size!=expected.size||
       std::memcmp(actual.data,expected.data,actual.size)||provider_->lookup132820(name,found)!=OriginalStandardArchiveProvider50LA::Lookup::present||
       !found.data||found.size!=actual.size||std::memcmp(found.data,actual.data,actual.size))return fail(d,E::source,0x2822F0);
    registryReceipts_[ordinal].kind=Completion::registryHit;registryReceipts_[ordinal].entry=occurrence;return true; // No original file call on reload0 hit.
}
bool OriginalLoadingFontSource50LA::recordColdRegistry2823EC(const std::shared_ptr<const void>& entry,
    OriginalUiMemberView50KG cel,OriginalUiMemberView50KG pst,const OriginalStandardTextureResource50LA* resource,D& d)noexcept {
    if(!resolver_)return fail(d,E::phase,0x2823EC);
    return resolver_->recordLoadingFontColdRegistry2823EC(*this,entry,cel,pst,resource,d);
}
bool OriginalStandardTextureResolver50LA::recordLoadingFontColdRegistry2823EC(OriginalLoadingFontSource50LA& owner,
    const std::shared_ptr<const void>& entry,OriginalUiMemberView50KG cel,OriginalUiMemberView50KG pst,
    const OriginalStandardTextureResource50LA* resource,D& d)noexcept {
    d={};using Completion=OriginalLoadingFontSource50LA::Completion;
    if(owner.resolver_.get()!=this||owner.thread_!=std::this_thread::get_id()||!entry||!resource||
       !owner.joined_||owner.sourceRetired_||owner.nativeClosed_||owner.celOccurrence_||owner.handle_!=-1||
       !sourceBankFamilyCurrent(&owner,SourceBankFamily::loadingFonts)||!cel.identity||!pst.identity)
        return fail(d,E::phase,0x2823EC);
    const auto ordinal=courseSourceName308F00(cel.identity->name,"font\\denmrk16.cel")?0u:
        courseSourceName308F00(cel.identity->name,"font\\venus16.cel")?1u:2u;
    if(ordinal>=2||owner.registryReceipts_[ordinal].kind!=Completion::unknown||
       (ordinal&&owner.registryReceipts_[0].entry.expired())||owner.sourceClosedCelEntries_[ordinal]!=entry.get())
        return fail(d,E::phase,0x2823EC);
    const auto& closed=owner.sourceClosedCels_[ordinal];
    if(closed.identity!=cel.identity||closed.data!=cel.data||closed.size!=cel.size||
       !courseSourceName308F00(pst.identity->name,ordinal?"font\\venus16.pst":"font\\denmrk16.pst"))
        return fail(d,E::source,0x2823EC);
    const auto& attempt=resource->loadingPstFile_;
    if(!attempt||attempt->root_.get()!=&owner||attempt->raRoot_||attempt->fontEntry_!=entry.get()||
       attempt->boundResource_!=resource||!attempt->associated_||!attempt->closed_||attempt->nativeAborted_||
       attempt->associationCleared_||attempt->sourceBankRetiredAssociation_||attempt->handle_!=-1||
       attempt->expected_.identity!=pst.identity||attempt->expected_.data!=pst.data||attempt->expected_.size!=pst.size||
       !loadingPstFileCurrent(*attempt,false)||!resource->constructed24DA30()||
       resource->resolver_.get()!=this||resource->sourceIndex_!=attempt->sourceIndex_)
        return fail(d,E::phase,0x2823EC);
    const auto& row=freshFileCapacity_->members150[attempt->row_];
    const auto& file=*storage_->entries[attempt->sourceIndex_].file;
    const auto cached=file.cached.lock();
    if(row.associatedObject1C!=resource||(row.flags18&4)||row.courseConfigOwner||
       !cached||cached.get()!=resource||file.cacheIdentity!=resource)return fail(d,E::phase,0x2823EC);
    auto& receipt=owner.registryReceipts_[ordinal];receipt.entry=entry;receipt.resource=cached;
    receipt.kind=Completion::coldReturned;return true;
}
bool OriginalLoadingFontSource50LA::retireBank1CAD94(OriginalStandardTextureResolver50LA::LoadingFontsBankLease& lease,D& d)noexcept {
    if(sourceRetired_){d={};return true;}if(!resolver_)return fail(d,E::phase,0x1CAD94);
    return resolver_->retireLoadingFontSource1CAD94(*this,lease,d);
}
bool OriginalStandardTextureResolver50LA::retireLoadingFontSource1CAD94(OriginalLoadingFontSource50LA& owner,
    LoadingFontsBankLease& lease,D& d)noexcept {
    if(freshFileCapacity_&&freshFileCapacity_->waterPoolFileBorrower)return fail(d,E::phase,0);
    if(freshFileCapacity_&&(freshFileCapacity_->streamLibraryOwner||freshFileCapacity_->musicOwner))return fail(d,E::phase,0);
    d={};if(!sourceBankFamilyCurrent(&owner,SourceBankFamily::loadingFonts)||!owner.joined_||owner.nativeClosed_||owner.sourceRetired_||
       owner.thread_!=std::this_thread::get_id()||owner.celOccurrence_||owner.handle_!=-1||
       owner.registryReceipts_[0].entry.expired()||owner.registryReceipts_[1].entry.expired())return fail(d,E::phase,0x1CAD94);
    auto& s=*freshFileCapacity_;
    if(s.courseStage!=FreshFileCapacity::CourseStage::joined||s.courseNativeBorrowers!=1||!s.members150||!s.devices14||
       s.courseDevice<1||s.courseDevice>=s.deviceCount18||
       (s.loadingRaOwner?!loadingRaRetainedCurrent():s.deviceCount18!=2)||lease.bank_.archive_!=owner.archive_||
       lease.bank_.mount_!=s.courseMount||lease.bank_.generation_!=s.courseGeneration||!lease.bank_.loadingFonts_)
        return fail(d,E::phase,0x12E5E0);
    // Pin exact published Entries and their cold resources throughout force1.
    // Hit receipts authorize no new positive SourceFile1C whatsoever.
    std::array<std::shared_ptr<const void>,2> entries;
    std::array<std::shared_ptr<OriginalStandardTextureResource50LA>,2> resources;
    for(std::size_t i=0;i<2;++i){
        auto& receipt=owner.registryReceipts_[i];entries[i]=receipt.entry.lock();
        if(!entries[i]||receipt.kind==OriginalLoadingFontSource50LA::Completion::unknown)return fail(d,E::phase,0x1CAD94);
        if(receipt.kind==OriginalLoadingFontSource50LA::Completion::coldReturned){
            resources[i]=receipt.resource.lock();if(!resources[i])return fail(d,E::phase,0x1CAD94);
            const auto& p=resources[i]->loadingPstFile_;
            if(!p||p->root_.get()!=&owner||p->fontEntry_!=entries[i].get()||p->boundResource_!=resources[i].get()||
               !p->associated_||!p->closed_||p->nativeAborted_||p->associationCleared_||p->sourceBankRetiredAssociation_||
               !loadingPstFileCurrent(*p,false)||s.members150[p->row_].associatedObject1C!=resources[i].get())
                return fail(d,E::phase,0x12EAA0);
            const auto& file=*storage_->entries[p->sourceIndex_].file;
            if(file.cached.lock()!=resources[i]||file.cacheIdentity!=resources[i].get())return fail(d,E::phase,0x12EAA0);
        }
    }
    for(std::int32_t n=0;n<s.memberCount154;++n)if(s.members150[n].deviceID00==s.courseDevice){
        const auto& row=s.members150[n];if(row.courseConfigOwner||(row.flags18&4))return fail(d,E::phase,0x12EAA0);
        if(row.associatedObject1C){bool matched=false;
            for(const auto& resource:resources)if(resource&&resource.get()==row.associatedObject1C&&resource->loadingPstFile_->row_==n)matched=true;
            if(!matched)return fail(d,E::phase,0x12EAA0);
        }
    }
    if(!canRetireGarageBank12E5E0(lease.bank_,d))return false;
    // Actual force1 clears associated pointers without releasing Resources.
    // Native exact receipts are preflight only, never substitute source effects.
    s.devices14[s.courseDevice].flags0C|=0x20; // Actual12E710 before forced member scan.
    if(!retireLoadingFontsBank1CAD94(lease,d))return false;
    for(std::int32_t n=0;n<s.memberCount154;++n)if(s.members150[n].deviceID00==s.courseDevice){
        s.retireMember12EAA0(n);
        for(const auto& resource:resources)if(resource&&resource->loadingPstFile_->row_==n)
            resource->loadingPstFile_->sourceBankRetiredAssociation_=true;
    }
    s.retireDevice12EB60(s.devices14[s.courseDevice]);
    s.courseNativeBorrowers=0;s.courseStage=FreshFileCapacity::CourseStage::retired;s.loadingFontsSourceRetired=true;
    s.courseArchive.reset();owner.sourceRetired_=true;owner.joined_=false;d.sourceEffectsCommitted=true;return true;
}
bool OriginalLoadingFontSource50LA::closeNative(OriginalStandardTextureResolver50LA::LoadingFontsBankLease& lease,D& d)noexcept {
    d={};if(thread_!=std::this_thread::get_id()||handle_!=-1||celOccurrence_)return fail(d,E::phase,0);
    if(nativeClosed_)return true;
    if(resolver_&&!resolver_->closeLoadingFontSourceNative(*this,lease,d))return false;
    if(!resolver_&&acquired_)return fail(d,E::phase,0);
    nativeClosed_=true;joined_=false;
    // Published textures retain their exact retired bank root/attempt. Keep its
    // resolver identity for later1327D0 tombstone admission, without reopening.
    if(!sourceRetired_){archive_.reset();provider_.reset();resolver_.reset();}
    return true;
}
bool OriginalStandardTextureResolver50LA::closeLoadingFontSourceNative(OriginalLoadingFontSource50LA& owner,
    LoadingFontsBankLease& lease,D& d)noexcept {
    if(freshFileCapacity_&&freshFileCapacity_->waterPoolFileBorrower)return fail(d,E::phase,0);
    d={};if(owner.resolver_.get()!=this||owner.thread_!=std::this_thread::get_id()||owner.handle_!=-1||owner.celOccurrence_||
       (loadingPstFontEntry_&&loadingPstRoot_.lock().get()==&owner))return fail(d,E::phase,0);
    auto& bank=lease.bank_;
    // The namespace lease is a borrowed native capability, not an owned mount.
    // Validate it BEFORE any Fresh withdrawal. A failed source registration may
    // still own this capability even though it acquired no Fresh root.
    if(bank.resolver_){
        if(owner.sourceRetired_||bank.resolver_!=this||!bank.loadingFonts_||bank.selected19E_||
           bank.thread_!=std::this_thread::get_id()||bank.provider_!=owner.provider_||bank.archive_!=owner.archive_||
           !storage_||bank.managerIdentity_!=storage_->managerIdentity||!bank.provider_||
           bank.mount_>=bank.provider_->mounts_.size())return fail(d,E::phase,0);
        const auto& mounted=*bank.provider_->mounts_[bank.mount_];
        if(!mounted.active||mounted.generation!=bank.generation_||mounted.admittedUi.lock()!=bank.archive_)
            return fail(d,E::source,0);
    }else if(owner.joined_&&!owner.sourceRetired_)return fail(d,E::phase,0);
    // Retained preexisting resources remain a refusal, not a blanket cache
    // waiver. Screen retains this owner and the lease when that gate rejects.
    if(owner.acquired_&&!owner.sourceRetired_&&!retireSourceBankNative(&owner,d))return false;
    if(bank.resolver_){
        bank.resolver_=nullptr;bank.generation_=0;bank.archive_.reset();bank.provider_.reset();bank.managerIdentity_.reset();
        // Do not mint closedBacking_: no source bank destructor was called.
    }
    d={};return true;
}

bool OriginalStandardTextureResolver50LA::adoptLoadingIconNativeMount(const void* root,bool owns,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::loadingIcon))return fail(d,E::phase,0);
    return adoptSourceBankNativeMount(root,owns,d);
}
bool OriginalStandardTextureResolver50LA::joinLoadingIconBankCanonical(const void* root,bool ownsNativeMount,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::loadingIcon))return fail(d,E::phase,0);
    return joinSourceBankCanonical(root,ownsNativeMount,d);
}
bool OriginalStandardTextureResolver50LA::openLoadingLsi131230(const void* root,const void* occurrence,
    const std::string& name,std::int32_t& handle,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::loadingIcon))return fail(d,E::phase,0);
    return openSourceBankMember131230(root,occurrence,name,handle,d);
}
bool OriginalStandardTextureResolver50LA::loadingLsiFileCurrent(const void* root,const void* occurrence,std::int32_t handle)const noexcept {
    return sourceBankFamilyCurrent(root,SourceBankFamily::loadingIcon)&&sourceBankMemberCurrent(root,occurrence,handle);
}
bool OriginalStandardTextureResolver50LA::readLoadingLsi12FB30(const void* root,const void* occurrence,std::int32_t handle,
    void* output,std::size_t requested,std::size_t& done,D& d)noexcept {
    done=0;d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::loadingIcon))return fail(d,E::phase,0);
    return readSourceBankMember12FB30(root,occurrence,handle,output,requested,done,d);
}
bool OriginalStandardTextureResolver50LA::readLoadingLsiLine12EE90(const void* root,const void* occurrence,std::int32_t handle,
    std::array<char,1024>& output,bool& present,std::size_t& position,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::loadingIcon))return fail(d,E::phase,0);
    return readSourceBankLine12EE90(root,occurrence,handle,output,present,position,d);
}
bool OriginalStandardTextureResolver50LA::tellLoadingLsi12F520(const void* root,const void* occurrence,std::int32_t handle,std::size_t& position,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::loadingIcon))return fail(d,E::phase,0);
    return tellSourceBankMember12F520(root,occurrence,handle,position,d);
}
bool OriginalStandardTextureResolver50LA::seekLoadingLsi12F640(const void* root,const void* occurrence,std::int32_t handle,std::size_t position,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::loadingIcon))return fail(d,E::phase,0);
    return seekSourceBankMember12F640(root,occurrence,handle,position,d);
}
bool OriginalStandardTextureResolver50LA::closeLoadingLsi131040(const void* root,const void* occurrence,
    std::int32_t handle,bool flush,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::loadingIcon))return fail(d,E::phase,0);
    return closeSourceBankMember131040(root,occurrence,handle,flush,d);
}
bool OriginalStandardTextureResolver50LA::retireLoadingIconBankNative(const void* root,D& d)noexcept {
    d={};if(!sourceBankFamilyCurrent(root,SourceBankFamily::loadingIcon))return fail(d,E::phase,0);
    return retireSourceBankNative(root,d);
}
bool OriginalStandardTextureResolver50LA::registerLoadingIconBank12CEA0(std::shared_ptr<const OriginalUiArchive50KG> archive,
    const OriginalStandardArchiveProvider50LA* provider,const void* owner,bool& acquired,D& d)noexcept {
    return registerSourceBank12CEA0(std::move(archive),provider,owner,SourceBankFamily::loadingIcon,acquired,d);
}

#include "original_loading_pst_file50la.inl"

#include "original_loading_ra_file50la.inl"
#include "original_music_source_file50la.inl"
#include "original_stream_library_file50la.inl"

#include "original_ra_track_source_file50la.inl"
#include "original_source_psg_file50la.inl"
#include "original_source_psg_missing50la.inl"
#include "original_source_collision_file50la.inl"
#include "original_pod_source_bank50la.inl"
#include "original_particle_source_config50la.inl"
#include "original_source_sky_file50la.inl"
#include "original_source_water_exists131a40.inl"
#include "original_water_source_config50la.inl"
#include "original_water_pool_source_file50la.inl"
}
