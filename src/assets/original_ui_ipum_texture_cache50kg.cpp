#include "original_ui_ipum_texture_cache50kg.h"
#include <algorithm>
#include <cstring>
#include <new>
#include <utility>

namespace rr::assets {
namespace {
using Error=OriginalUiIpumCacheError50KG;
constexpr const char sourceMember[]="ui\\textures\\19c_1.pst";
constexpr const char sourceSha256[]="ad0bcd92f415d65d8bf68ae9e65fe355043e297e834cdffeae4ceac7cfa60cd3";
constexpr const char archiveSha256[]="112cd71ac9cfcc909b0edb56e0e420bb358b7078b5abae5f48c62b95a6bbf856";
constexpr const char converterSha256[]="e482393f6b384863d17c7cc84aa930affd466eb6c6718dfd1292713408bf9efe";
constexpr const char commandArguments[]="-hide_banner -loglevel error -nostdin -cpuflags 0 -flags +bitexact -idct simple -threads 1 -err_detect explode -f ipu -i pipe:0 -frames:v 1 -pix_fmt yuv420p -f rawvideo pipe:1";
constexpr const char colorPolicyDescription[]="50FC integer BT.601; nearest 2x2 chroma; RGB16 dither; RGB5 high-bit expansion; TEXA TA0=TA1=255 AEM=1; no row flip; hardware pixel parity unverified";
constexpr std::size_t sourceBytes=23096,retainedDecodedBytes=611344;
struct SourceMipRecord {
    std::uint32_t width,height;
    std::size_t header,payload,bytes;
    const char* sha256;
    const char* filename;
};
constexpr SourceMipRecord sourceMips[originalUiIpumMipCount50KG]={
    {16,16,12,32,205,"30260bfadc135b0678023e198c919bbc76769808250bb1d64da95160a5dadc35","19c_1-mip0.rrtex"},
    {32,32,237,257,620,"0febb2bfc984a924942b49caefc5c1d024fd5b3fb54b0bf962f1ee40cf1f36a4","19c_1-mip1.rrtex"},
    {64,64,877,897,1850,"799b622653d923464c9a478e7aebfab286b7d135c5577d217840bb1f3eed7435","19c_1-mip2.rrtex"},
    {128,128,2747,2767,5469,"e017da2329dc9a9101ed1a33d32e87a3d9bb58e2ccd1055261073f6ee5bc5f7b","19c_1-mip3.rrtex"},
    {256,256,8236,8256,14840,"c8e6de01def72409f2cf85de2e6705b77261feb9194b33d50a5e4097e86474b5","19c_1-mip4.rrtex"}
};
struct RetainedMipRecord {
    const char* rrtexSha256;
    const char* rgbaSha256;
    const char* yuv420pSha256;
};
struct RetainedCodecRecord {
    const char* converterSha256;
    const char* generatorSha256;
    const char* ffmpegVersion;
    const char* ffmpegSha256;
    const char* ffmpegCommand;
};
#include "original_ui_ipum_texture_cache50kg_generated.inc"

struct AdditionalSourceRecord {
    const char* member;
    std::size_t sourceBytes;
    const char* sourceSha256;
    std::size_t mipCount;
    SourceMipRecord sourceMips[originalUiIpumMipCount50KG];
    RetainedMipRecord retainedMips[originalUiIpumMipCount50KG];
    RetainedCodecRecord codec;
};
#include "original_ui_ipum_texture_cache50kg_additional_generated.inc"
// Independently retained original whole-member pins. Generated conversion
// records may describe only this finite default-unlocked source set.
struct AdditionalPin {const char* member;std::size_t bytes;const char* sha;};
constexpr AdditionalPin additionalPins[]={
    {"ui\\textures\\04c_1.pst",39441,"ca086fb878b398f6fccdcb2161d65ff38bacdb2ee5a3c96ff06e1ed814dbfd27"},
    {"ui\\textures\\04c_2.pst",33310,"5f5979fa86f0043be411d84bfb33244921482290eda4c0f4df50e4af1faa32ff"},
    {"ui\\textures\\10c_1.pst",23311,"99ccba918bd470d396ebb618b382a81907839a34d0b34b9beab8b5a09e9c0722"},
    {"ui\\textures\\10c_2.pst",18494,"9efd4f229c3203e9a27252cac070a1107c83598a7bc536f51a9c46cbb4852ae5"},
    {"ui\\textures\\12c_1.pst",22581,"eecda390eca1ea261f80ba992a37a225613f12d6bc9d0e9545d2380f64662b32"},
    {"ui\\textures\\12c_4.pst",22860,"c3aaa742f80f395f8fc01e0c564acbdea652835038e219c16c3de0a8803f5c7d"},
    {"ui\\textures\\15c_1.pst",25470,"e02462b1c14fc1498a46b91b4245dc8453da45ffe674b70eae080828fdef557d"},
    {"ui\\textures\\15c_2.pst",31682,"cf7093e713ad87e15301435a51beb6cd2cf958dfe74c72dc2657f7cbf1c1ccd5"}
};

bool fail(OriginalUiIpumCacheDiagnostic50KG& d,Error error,std::size_t mip=0) noexcept {
    d.error=error;d.mip=mip;return false;
}
bool cancelled(const DecodeHooks48C* hooks) noexcept {
    return hooks&&hooks->cancelled&&hooks->cancelled(hooks->context);
}
bool hexHash(const char* s) noexcept {
    if(!s||std::strlen(s)!=64)return false;
    for(unsigned i=0;i<64;++i)if(!((s[i]>='0'&&s[i]<='9')||(s[i]>='a'&&s[i]<='f')))return false;
    return true;
}
bool catalogValid() noexcept {
    if(!retainedCatalogAvailable||!hexHash(retainedCodecRecord.converterSha256)||
        std::strcmp(retainedCodecRecord.converterSha256,converterSha256)!=0||
        !hexHash(retainedCodecRecord.generatorSha256)||!hexHash(retainedCodecRecord.ffmpegSha256)||
        !retainedCodecRecord.ffmpegVersion||!retainedCodecRecord.ffmpegVersion[0]||
        !retainedCodecRecord.ffmpegCommand||
        std::strcmp(retainedCodecRecord.ffmpegCommand,commandArguments)!=0)return false;
    for(const auto& mip:retainedMipRecords)
        if(!hexHash(mip.rrtexSha256)||!hexHash(mip.rgbaSha256)||!hexHash(mip.yuv420pSha256))return false;
    return true;
}
bool additionalCatalogValid() noexcept {
    if(!additionalCatalogAvailable||!additionalSourceCount||additionalSourceCount>8||
       additionalSourceCount>sizeof(additionalSources)/sizeof(additionalSources[0]))return false;
    for(std::size_t index=0;index<additionalSourceCount;++index){
        const auto& a=additionalSources[index];const auto& c=a.codec;
        if(!a.member||!hexHash(a.sourceSha256)||!a.mipCount||a.mipCount>originalUiIpumMipCount50KG)return false;
        bool pinned=false;
        for(const auto& pin:additionalPins)if(!std::strcmp(a.member,pin.member)&&a.sourceBytes==pin.bytes&&
            !std::strcmp(a.sourceSha256,pin.sha))pinned=true;
        if(!pinned)return false;
        for(std::size_t j=0;j<index;++j)if(!std::strcmp(a.member,additionalSources[j].member))return false;
        if(!hexHash(c.converterSha256)||std::strcmp(c.converterSha256,converterSha256)||
           !hexHash(c.generatorSha256)||!hexHash(c.ffmpegSha256)||!c.ffmpegVersion||!c.ffmpegVersion[0]||
           !c.ffmpegCommand||std::strcmp(c.ffmpegCommand,commandArguments))return false;
        for(std::size_t i=0;i<a.mipCount;++i){const auto& m=a.sourceMips[i];const auto& r=a.retainedMips[i];
            if(m.width<16||m.height<16||m.width>1024||m.height>1024||
               (m.width&(m.width-1))||(m.height&(m.height-1))||
               (i&&(m.width<=a.sourceMips[i-1].width||m.height<=a.sourceMips[i-1].height))||
               !hexHash(m.sha256)||!hexHash(r.rrtexSha256)||!hexHash(r.rgbaSha256)||!hexHash(r.yuv420pSha256)||
               !m.filename||!m.filename[0]||std::strchr(m.filename,'/')||std::strchr(m.filename,'\\')||
               std::strstr(m.filename,".."))return false;
        }
    }
    return true;
}
std::uint16_t u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(std::uint16_t(p[0])|(std::uint16_t(p[1])<<8));
}
std::uint32_t u32(const std::uint8_t* p) noexcept {
    return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);
}

// The repository's existing standard SHA-256 authentication algorithm, copied
// from original_ui_archive50kg.cpp. Fixed stack workspace; no codec behavior.
std::uint32_t rotateRight(std::uint32_t x,unsigned n) noexcept {
    return (x>>n)|(x<<(32u-n));
}
void shaBlock(const std::uint8_t* block,std::array<std::uint32_t,8>& h) noexcept {
    constexpr std::uint32_t k[64]={
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
    std::uint32_t w[64]{};
    for(unsigned i=0;i<16;++i)for(unsigned j=0;j<4;++j)w[i]=(w[i]<<8)|block[i*4+j];
    for(unsigned i=16;i<64;++i){
        const auto s0=rotateRight(w[i-15],7)^rotateRight(w[i-15],18)^(w[i-15]>>3);
        const auto s1=rotateRight(w[i-2],17)^rotateRight(w[i-2],19)^(w[i-2]>>10);
        w[i]=w[i-16]+s0+w[i-7]+s1;
    }
    auto a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],v=h[7];
    for(unsigned i=0;i<64;++i){
        const auto s1=rotateRight(e,6)^rotateRight(e,11)^rotateRight(e,25);
        const auto ch=(e&f)^(~e&g);
        const auto t1=v+s1+ch+k[i]+w[i];
        const auto s0=rotateRight(a,2)^rotateRight(a,13)^rotateRight(a,22);
        const auto maj=(a&b)^(a&c)^(b&c);
        const auto t2=s0+maj;
        v=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
    }
    h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=v;
}
enum class HashResult { match,mismatch,cancelled };
HashResult hashMatches(const std::uint8_t* data,std::size_t size,const char* expected,
    const DecodeHooks48C* hooks) noexcept {
    std::array<std::uint32_t,8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at=0;
    for(;size-at>=64;at+=64){
        if(at%65536==0&&cancelled(hooks))return HashResult::cancelled;
        shaBlock(data+at,h);
    }
    std::array<std::uint8_t,128> tail{};
    const std::size_t remainder=size-at;
    if(remainder)std::memcpy(tail.data(),data+at,remainder);
    tail[remainder]=0x80;
    const std::size_t padded=remainder<56?64:128;
    const std::uint64_t bits=static_cast<std::uint64_t>(size)*8u;
    for(unsigned i=0;i<8;++i)tail[padded-1-i]=static_cast<std::uint8_t>(bits>>(i*8));
    shaBlock(tail.data(),h);if(padded==128)shaBlock(tail.data()+64,h);
    if(cancelled(hooks))return HashResult::cancelled;
    constexpr char hex[]="0123456789abcdef";
    for(unsigned i=0;i<64;++i)
        if(expected[i]!=hex[(h[i/8]>>((7u-i%8)*4))&15u])return HashResult::mismatch;
    return HashResult::match;
}
bool checkedHash(const std::uint8_t* data,std::size_t bytes,const char* expected,
    const DecodeHooks48C* hooks,OriginalUiIpumCacheDiagnostic50KG& d,Error error,std::size_t mip=0) noexcept {
    const auto result=hashMatches(data,bytes,expected,hooks);
    if(result==HashResult::cancelled)return fail(d,Error::cancelled,mip);
    return result==HashResult::match?true:fail(d,error,mip);
}
} // namespace

struct OriginalUiIpumTextureCache50KG::Storage {
    std::shared_ptr<const OriginalUiArchive50KG> archive;
    OriginalUiMemberView50KG source{};
    Texture48E texture;
    OriginalUiIpumProvenance50KG evidence;
    std::size_t decodedBytes=0;
};
OriginalUiIpumTextureCache50KG::OriginalUiIpumTextureCache50KG() noexcept=default;
OriginalUiIpumTextureCache50KG::~OriginalUiIpumTextureCache50KG()=default;
bool OriginalUiIpumTextureCache50KG::catalogAvailable() noexcept{return catalogValid();}
const char* OriginalUiIpumTextureCache50KG::retainedMipFilename(std::size_t mip) noexcept {
    return mip<originalUiIpumMipCount50KG?sourceMips[mip].filename:nullptr;
}
bool OriginalUiIpumTextureCache50KG::load(std::shared_ptr<const OriginalUiArchive50KG> archive,
    const std::vector<std::vector<std::uint8_t>>& retained,OriginalUiIpumCacheDiagnostic50KG& d,
    const DecodeHooks48C* hooks,const TextureLimits48E& limits) noexcept {
    d={};
    try {
        if(storage_)return fail(d,Error::alreadyLoaded);
        if(cancelled(hooks))return fail(d,Error::cancelled);
        if(!catalogValid())return fail(d,Error::cacheUnavailable);
        if(!archive||!archive->loaded()||
            archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider)||
            std::strcmp(archive->identity()->sha256,archiveSha256)!=0)return fail(d,Error::archive);
        const auto source=archive->find(sourceMember);
        if(!source.identity||!source.data||source.size!=sourceBytes||
            source.identity->bytes!=sourceBytes||
            std::strcmp(source.identity->sha256,sourceSha256)!=0)return fail(d,Error::source);
        std::size_t totalDecodedBytes=retainedDecodedBytes;
        if(storage_)totalDecodedBytes+=storage_->decodedBytes;
        for(const auto& existing:additional_)if(existing)totalDecodedBytes+=existing->decodedBytes;
        if(sourceBytes>std::min(limits.input_bytes,std::size_t(8u*1024u*1024u))||
            totalDecodedBytes>std::min(limits.decoded_bytes,std::size_t(16u*1024u*1024u)))
            return fail(d,Error::limit);
        if(retained.size()!=originalUiIpumMipCount50KG)return fail(d,Error::imageCount);
        if(!checkedHash(source.data,source.size,sourceSha256,hooks,d,Error::source))return false;
        if(u16(source.data)!=1||u16(source.data+2)!=0||u32(source.data+4)!=2||
            u32(source.data+8)!=originalUiIpumMipCount50KG)return fail(d,Error::sourceLayout);
        // Validate every source extent and every retained output before any
        // decoded pixel allocation/publication. Constants are exact originals.
        constexpr std::uint8_t terminators[]{0,0,1,0xb0,0,0,1,0xb1};
        for(std::size_t i=0;i<originalUiIpumMipCount50KG;++i){
            if(cancelled(hooks))return fail(d,Error::cancelled,i);
            const auto& s=sourceMips[i];const auto& r=retainedMipRecords[i];const auto& image=retained[i];
            if(s.header>source.size||20>source.size-s.header||s.payload!=s.header+20||
                s.payload>source.size||s.bytes>source.size-s.payload||s.bytes<25||
                (i==0?s.header!=12:s.header!=sourceMips[i-1].payload+sourceMips[i-1].bytes)||
                (i+1==originalUiIpumMipCount50KG&&s.payload+s.bytes!=source.size))
                return fail(d,Error::sourceLayout,i);
            const auto* h=source.data+s.header;const auto* p=source.data+s.payload;
            if(u32(h)!=s.height||u32(h+4)!=s.width||u32(h+8)||u32(h+12)||u32(h+16)!=s.bytes||
                std::memcmp(p,"ipum",4)||u32(p+4)!=s.bytes-8||u16(p+8)!=s.width||
                u16(p+10)!=s.height||u32(p+12)!=1||p[16]!=0||
                std::memcmp(p+s.bytes-8,terminators,8))return fail(d,Error::sourceLayout,i);
            if(!checkedHash(p,s.bytes,s.sha256,hooks,d,Error::encodedIdentity,i))return false;
            const std::size_t rgbaBytes=std::size_t(s.width)*s.height*4;
            if(image.size()!=rgbaBytes+16)return fail(d,Error::imageFormat,i);
            if(!checkedHash(image.data(),image.size(),r.rrtexSha256,hooks,d,Error::imageIdentity,i)||
                !checkedHash(image.data()+16,rgbaBytes,r.rgbaSha256,hooks,d,Error::imageIdentity,i))return false;
            if(std::memcmp(image.data(),"RRTEX001",8)||u32(image.data()+8)!=s.width||
                u32(image.data()+12)!=s.height)return fail(d,Error::imageFormat,i);
            for(std::size_t pixelAt=16;pixelAt<image.size();pixelAt+=4){
                if((pixelAt-16)%65536==0&&cancelled(hooks))return fail(d,Error::cancelled,i);
                if((image[pixelAt]&7u)||(image[pixelAt+1]&7u)||(image[pixelAt+2]&7u)||
                    image[pixelAt+3]!=(image[pixelAt]||image[pixelAt+1]||image[pixelAt+2]?255:0))
                    return fail(d,Error::imageFormat,i);
            }
        }
        auto next=std::make_unique<Storage>();next->archive=std::move(archive);next->source=source;
        next->decodedBytes=retainedDecodedBytes;
        auto& texture=next->texture;texture.revision=1;texture.flags=0;texture.kind=2;
        // No authored palette exists for kind2; zero storage is not a palette.
        texture.mips.reserve(originalUiIpumMipCount50KG);
        auto& evidence=next->evidence;
        evidence.colorPolicyKind=OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1;
        evidence.sourceMember=sourceMember;
        evidence.sourcePstSha256=sourceSha256;evidence.sourceArchiveSha256=archiveSha256;
        evidence.converterSha256=retainedCodecRecord.converterSha256;
        evidence.generatorSha256=retainedCodecRecord.generatorSha256;
        evidence.ffmpegVersion=retainedCodecRecord.ffmpegVersion;
        evidence.ffmpegSha256=retainedCodecRecord.ffmpegSha256;
        evidence.ffmpegCommand=retainedCodecRecord.ffmpegCommand;evidence.colorPolicy=colorPolicyDescription;
        if(hooks&&hooks->progress)hooks->progress(hooks->context,"pst.ipum_cache",0,originalUiIpumMipCount50KG);
        for(std::size_t i=0;i<originalUiIpumMipCount50KG;++i){
            if(cancelled(hooks))return fail(d,Error::cancelled,i);
            const auto& s=sourceMips[i];const auto& r=retainedMipRecords[i];const auto& image=retained[i];
            TextureMip48E mip;mip.width=s.width;mip.height=s.height;
            mip.source_header_offset=s.header;mip.source_indices_offset=s.payload;
            mip.rgba.assign(image.begin()+16,image.end());texture.mips.push_back(std::move(mip));
            auto& e=evidence.mips[i];e.width=s.width;e.height=s.height;e.sourceHeaderOffset=s.header;
            e.sourcePayloadOffset=s.payload;e.sourcePayloadBytes=s.bytes;e.sourcePayloadSha256=s.sha256;
            e.rrtexSha256=r.rrtexSha256;e.rgbaSha256=r.rgbaSha256;e.yuv420pSha256=r.yuv420pSha256;
            e.retainedFilename=s.filename;
            if(hooks&&hooks->progress)hooks->progress(hooks->context,"pst.ipum_cache",
                static_cast<std::uint32_t>(i+1),originalUiIpumMipCount50KG);
        }
        texture.base_rrtex=retained.back();
        if(cancelled(hooks))return fail(d,Error::cancelled,originalUiIpumMipCount50KG-1);
        storage_=std::move(next);return true;
    }catch(const std::bad_alloc&){return fail(d,Error::allocation);}
    catch(...){return fail(d,Error::unexpectedFailure);}
}
bool OriginalUiIpumTextureCache50KG::loadAdditional(std::size_t sourceIndex,std::shared_ptr<const OriginalUiArchive50KG> archive,
    const std::vector<std::vector<std::uint8_t>>& retained,OriginalUiIpumCacheDiagnostic50KG& d,
    const DecodeHooks48C* hooks,const TextureLimits48E& limits) noexcept {
    d={};
    try {
        if(!additionalCatalogValid()||sourceIndex>=additionalSourceCount)return fail(d,Error::cacheUnavailable);
        if(additional_[sourceIndex])return fail(d,Error::alreadyLoaded);
        const auto& record=additionalSources[sourceIndex];
        const auto* sourceMember=record.member;const auto* sourceSha256=record.sourceSha256;
        const auto sourceBytes=record.sourceBytes;const auto mipCount=record.mipCount;
        const auto& sourceMips=record.sourceMips;const auto& retainedMipRecords=record.retainedMips;
        const auto& retainedCodecRecord=record.codec;
        std::size_t retainedDecodedBytes=0;
        for(std::size_t i=0;i<mipCount;++i){
            const auto pixels=std::size_t(sourceMips[i].width)*sourceMips[i].height;
            retainedDecodedBytes+=pixels*4;
        }
        // Mip RGBA plus independently retained full base RRTEX allocation.
        retainedDecodedBytes+=std::size_t(sourceMips[mipCount-1].width)*sourceMips[mipCount-1].height*4+16;
        if(cancelled(hooks))return fail(d,Error::cancelled);
        if(!archive||!archive->loaded()||
            archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider)||
            std::strcmp(archive->identity()->sha256,archiveSha256)!=0)return fail(d,Error::archive);
        const auto source=archive->find(sourceMember);
        if(!source.identity||!source.data||source.size!=sourceBytes||
            source.identity->bytes!=sourceBytes||
            std::strcmp(source.identity->sha256,sourceSha256)!=0)return fail(d,Error::source);
        std::size_t totalDecodedBytes=retainedDecodedBytes;
        if(storage_)totalDecodedBytes+=storage_->decodedBytes;
        for(const auto& existing:additional_)if(existing)totalDecodedBytes+=existing->decodedBytes;
        if(sourceBytes>std::min(limits.input_bytes,std::size_t(8u*1024u*1024u))||
            totalDecodedBytes>std::min(limits.decoded_bytes,std::size_t(16u*1024u*1024u)))
            return fail(d,Error::limit);
        if(retained.size()!=mipCount)return fail(d,Error::imageCount);
        if(!checkedHash(source.data,source.size,sourceSha256,hooks,d,Error::source))return false;
        if(u16(source.data)!=1||u16(source.data+2)!=0||u32(source.data+4)!=2||
            u32(source.data+8)!=mipCount)return fail(d,Error::sourceLayout);
        // Validate every source extent and every retained output before any
        // decoded pixel allocation/publication. Constants are exact originals.
        constexpr std::uint8_t terminators[]{0,0,1,0xb0,0,0,1,0xb1};
        for(std::size_t i=0;i<mipCount;++i){
            if(cancelled(hooks))return fail(d,Error::cancelled,i);
            const auto& s=sourceMips[i];const auto& r=retainedMipRecords[i];const auto& image=retained[i];
            if(s.header>source.size||20>source.size-s.header||s.payload!=s.header+20||
                s.payload>source.size||s.bytes>source.size-s.payload||s.bytes<25||
                (i==0?s.header!=12:s.header!=sourceMips[i-1].payload+sourceMips[i-1].bytes)||
                (i+1==mipCount&&s.payload+s.bytes!=source.size))
                return fail(d,Error::sourceLayout,i);
            const auto* h=source.data+s.header;const auto* p=source.data+s.payload;
            if(u32(h)!=s.height||u32(h+4)!=s.width||u32(h+8)||u32(h+12)||u32(h+16)!=s.bytes||
                std::memcmp(p,"ipum",4)||u32(p+4)!=s.bytes-8||u16(p+8)!=s.width||
                u16(p+10)!=s.height||u32(p+12)!=1||p[16]!=0||
                std::memcmp(p+s.bytes-8,terminators,8))return fail(d,Error::sourceLayout,i);
            if(!checkedHash(p,s.bytes,s.sha256,hooks,d,Error::encodedIdentity,i))return false;
            const std::size_t rgbaBytes=std::size_t(s.width)*s.height*4;
            if(image.size()!=rgbaBytes+16)return fail(d,Error::imageFormat,i);
            if(!checkedHash(image.data(),image.size(),r.rrtexSha256,hooks,d,Error::imageIdentity,i)||
                !checkedHash(image.data()+16,rgbaBytes,r.rgbaSha256,hooks,d,Error::imageIdentity,i))return false;
            if(std::memcmp(image.data(),"RRTEX001",8)||u32(image.data()+8)!=s.width||
                u32(image.data()+12)!=s.height)return fail(d,Error::imageFormat,i);
            for(std::size_t pixelAt=16;pixelAt<image.size();pixelAt+=4){
                if((pixelAt-16)%65536==0&&cancelled(hooks))return fail(d,Error::cancelled,i);
                if((image[pixelAt]&7u)||(image[pixelAt+1]&7u)||(image[pixelAt+2]&7u)||
                    image[pixelAt+3]!=(image[pixelAt]||image[pixelAt+1]||image[pixelAt+2]?255:0))
                    return fail(d,Error::imageFormat,i);
            }
        }
        auto next=std::make_unique<Storage>();next->archive=std::move(archive);next->source=source;
        next->decodedBytes=retainedDecodedBytes;
        auto& texture=next->texture;texture.revision=1;texture.flags=0;texture.kind=2;
        // No authored palette exists for kind2; zero storage is not a palette.
        texture.mips.reserve(mipCount);
        auto& evidence=next->evidence;
        evidence.colorPolicyKind=OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1;
        evidence.sourceMember=sourceMember;
        evidence.sourcePstSha256=sourceSha256;evidence.sourceArchiveSha256=archiveSha256;
        evidence.converterSha256=retainedCodecRecord.converterSha256;
        evidence.generatorSha256=retainedCodecRecord.generatorSha256;
        evidence.ffmpegVersion=retainedCodecRecord.ffmpegVersion;
        evidence.ffmpegSha256=retainedCodecRecord.ffmpegSha256;
        evidence.ffmpegCommand=retainedCodecRecord.ffmpegCommand;evidence.colorPolicy=colorPolicyDescription;
        if(hooks&&hooks->progress)hooks->progress(hooks->context,"pst.ipum_cache",0,mipCount);
        for(std::size_t i=0;i<mipCount;++i){
            if(cancelled(hooks))return fail(d,Error::cancelled,i);
            const auto& s=sourceMips[i];const auto& r=retainedMipRecords[i];const auto& image=retained[i];
            TextureMip48E mip;mip.width=s.width;mip.height=s.height;
            mip.source_header_offset=s.header;mip.source_indices_offset=s.payload;
            mip.rgba.assign(image.begin()+16,image.end());texture.mips.push_back(std::move(mip));
            auto& e=evidence.mips[i];e.width=s.width;e.height=s.height;e.sourceHeaderOffset=s.header;
            e.sourcePayloadOffset=s.payload;e.sourcePayloadBytes=s.bytes;e.sourcePayloadSha256=s.sha256;
            e.rrtexSha256=r.rrtexSha256;e.rgbaSha256=r.rgbaSha256;e.yuv420pSha256=r.yuv420pSha256;
            e.retainedFilename=s.filename;
            if(hooks&&hooks->progress)hooks->progress(hooks->context,"pst.ipum_cache",
                static_cast<std::uint32_t>(i+1),mipCount);
        }
        texture.base_rrtex=retained.back();
        if(cancelled(hooks))return fail(d,Error::cancelled,mipCount-1);
        additional_[sourceIndex]=std::move(next);return true;
    }catch(const std::bad_alloc&){return fail(d,Error::allocation);}
    catch(...){return fail(d,Error::unexpectedFailure);}
}
bool OriginalUiIpumTextureCache50KG::loaded() const noexcept {
    if(storage_)return true;
    for(const auto& entry:additional_)if(entry)return true;
    return false;
}
OriginalUiIpumColorPolicy50KG OriginalUiIpumTextureCache50KG::colorPolicy() const noexcept {
    return loaded()?OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1:
        OriginalUiIpumColorPolicy50KG::unavailable;
}
const Texture48E* OriginalUiIpumTextureCache50KG::textureFor(const OriginalUiMemberView50KG& source,
    std::size_t offset,std::size_t bytes) const noexcept {
    if(offset)return nullptr;
    const auto matches=[&](const Storage* entry){return entry&&bytes==entry->source.size&&
        source.identity==entry->source.identity&&source.data==entry->source.data&&source.size==entry->source.size;};
    if(matches(storage_.get()))return &storage_->texture;
    for(const auto& entry:additional_)if(matches(entry.get()))return &entry->texture;
    return nullptr;
}
const OriginalUiIpumProvenance50KG* OriginalUiIpumTextureCache50KG::provenance() const noexcept {
    return storage_?&storage_->evidence:nullptr;
}
std::size_t OriginalUiIpumTextureCache50KG::additionalCatalogSourceCount() noexcept {
    return additionalCatalogValid()?additionalSourceCount:0;
}
const char* OriginalUiIpumTextureCache50KG::additionalSourceMember(std::size_t source) noexcept {
    return source<additionalCatalogSourceCount()?additionalSources[source].member:nullptr;
}
std::size_t OriginalUiIpumTextureCache50KG::additionalMipCount(std::size_t source) noexcept {
    return source<additionalCatalogSourceCount()?additionalSources[source].mipCount:0;
}
const char* OriginalUiIpumTextureCache50KG::additionalRetainedMipFilename(std::size_t source,std::size_t mip) noexcept {
    return mip<additionalMipCount(source)?additionalSources[source].sourceMips[mip].filename:nullptr;
}
const OriginalUiIpumProvenance50KG* OriginalUiIpumTextureCache50KG::provenanceFor(
    const OriginalUiMemberView50KG& source,std::size_t offset,std::size_t bytes) const noexcept {
    if(offset)return nullptr;
    const auto matches=[&](const Storage* entry){return entry&&bytes==entry->source.size&&
        source.identity==entry->source.identity&&source.data==entry->source.data&&source.size==entry->source.size;};
    if(matches(storage_.get()))return &storage_->evidence;
    for(const auto& entry:additional_)if(matches(entry.get()))return &entry->evidence;
    return nullptr;
}
} // namespace rr::assets
