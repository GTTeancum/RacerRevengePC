#include "../../src/assets/original_ui_ipum_texture_cache50kg.h"
#include "../../src/assets/original_ui_screen_assets50kg.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>

// Root-owned focused native check. Source-only staged, never run by its author.
// Requires the real generated catalog plus its matching original-derived mips.
// Never launches FFmpeg, game code, a renderer or a guest/emulator process.
namespace {
namespace a=rr::assets;
using Bytes=std::vector<std::uint8_t>;
using E=a::OriginalUiIpumCacheError50KG;
void need(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
Bytes read(const std::filesystem::path& path,std::size_t cap){
    need(!std::filesystem::is_symlink(path)&&std::filesystem::is_regular_file(path),"Regular input file required");
    const auto n=std::filesystem::file_size(path);need(n<=cap,"Input byte cap");
    std::ifstream stream(path,std::ios::binary);need(bool(stream),"Input open");
    Bytes result(static_cast<std::size_t>(n));
    if(n)stream.read(reinterpret_cast<char*>(result.data()),static_cast<std::streamsize>(n));
    need(bool(stream)&&stream.peek()==std::char_traits<char>::eof(),"Exact file read");return result;
}
struct Cancellation { std::uint32_t last=0,stopAt=0;bool progressSeen=false,immediate=false; };
bool cancel(void* p){const auto& c=*static_cast<Cancellation*>(p);return c.immediate||(c.progressSeen&&c.last>=c.stopAt);}
void progress(void* p,const char*,std::uint32_t complete,std::uint32_t){
    auto& c=*static_cast<Cancellation*>(p);c.last=complete;c.progressSeen=true;
}
}
int main(int argc,char** argv){
    try {
        need(argc==3,"Usage: original_ui_ipum_texture_cache50kg_check UIRIDER.RES CACHE_IMAGE_DIRECTORY");
        need(a::OriginalUiIpumTextureCache50KG::catalogAvailable(),
            "No generated real output catalog: run root-authorized cold conversion before this check");
        const auto archiveBytes=read(argv[1],5552683);
        auto archive=std::make_shared<a::OriginalUiArchive50KG>();a::OriginalUiArchiveDiagnostic50KG ad;
        need(archive->load(a::OriginalUiArchiveId50KG::rider,archiveBytes,ad),"Authentic Rider archive");
        const auto member=archive->find("ui/textures/19c_1.pst");
        need(member.identity&&member.data&&member.size==23096,"Authentic Rider19c_1 member");
        const std::filesystem::path cacheDirectory=argv[2];
        need(!std::filesystem::is_symlink(cacheDirectory),"Cache directory symlink rejected");
        std::vector<Bytes> images;
        for(std::size_t i=0;i<a::originalUiIpumMipCount50KG;++i){
            const auto* filename=a::OriginalUiIpumTextureCache50KG::retainedMipFilename(i);
            need(filename!=nullptr,"Fixed retained filename");images.push_back(read(cacheDirectory/filename,262160));
        }
        need(!a::OriginalUiIpumTextureCache50KG::retainedMipFilename(5),"No unregistered output filename");
        a::OriginalUiIpumCacheDiagnostic50KG d;
        a::OriginalUiIpumTextureCache50KG noArchive;
        need(!noArchive.load(nullptr,images,d)&&d.error==E::archive&&!noArchive.loaded()&&
            noArchive.colorPolicy()==a::OriginalUiIpumColorPolicy50KG::unavailable,"Archive owner required");
        a::OriginalUiIpumTextureCache50KG wrongCount;auto missing=images;missing.pop_back();
        need(!wrongCount.load(archive,missing,d)&&d.error==E::imageCount&&!wrongCount.loaded(),"All five original mips required");
        unsigned rejected=0;
        for(std::size_t i=0;i<images.size();++i){
            for(unsigned mutation=0;mutation<3;++mutation){
                auto changed=images;
                if(mutation==0)changed[i][changed[i].size()/2]^=1;
                else if(mutation==1)changed[i].pop_back();else changed[i].push_back(0);
                a::OriginalUiIpumTextureCache50KG candidate;
                need(!candidate.load(archive,changed,d)&&!candidate.loaded()&&!candidate.provenance(),
                    "Changed/truncated/suffixed source-derived mip rejected transactionally");
                need(d.error==(mutation==0?E::imageIdentity:E::imageFormat)&&d.mip==i,"Exact corrupted mip diagnostic");
                ++rejected;
                need(candidate.load(archive,images,d),"Failed cold load is retryable");
            }
        }
        for(unsigned stop=0;stop<=5;++stop){
            a::OriginalUiIpumTextureCache50KG candidate;Cancellation c;c.stopAt=stop;
            a::DecodeHooks48C hooks{&c,cancel,progress};
            need(!candidate.load(archive,images,d,&hooks)&&d.error==E::cancelled&&!candidate.loaded(),
                "Cancellation before/after every mip is transactional");
        }
        a::OriginalUiIpumTextureCache50KG cancelledOwner;Cancellation immediate;immediate.immediate=true;
        a::DecodeHooks48C initialHooks{&immediate,cancel,progress};
        need(!cancelledOwner.load(archive,images,d,&initialHooks)&&d.error==E::cancelled,"Initial cancellation");
        a::TextureLimits48E limits;limits.decoded_bytes=611343;
        a::OriginalUiIpumTextureCache50KG limited;
        need(!limited.load(archive,images,d,nullptr,limits)&&d.error==E::limit,"All RGBA mips plus base copy count toward limit");
        limits.decoded_bytes=611344;limits.input_bytes=23095;
        need(!limited.load(archive,images,d,nullptr,limits)&&d.error==E::limit,"Original PST input cap");
        limits.input_bytes=23096;
        auto cache=std::make_unique<a::OriginalUiIpumTextureCache50KG>();
        need(cache->load(archive,images,d,nullptr,limits),"Exact byte limits accepted");
        need(cache->colorPolicy()==a::OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1,
            "Loaded owner attests exact typed color policy");
        const auto* texture=cache->textureFor(member,0,member.size);const auto* evidence=cache->provenance();
        need(texture&&evidence&&texture->revision==1&&texture->flags==0&&texture->kind==2&&texture->mips.size()==5,
            "Original kind2/revision/flags/mip count preserved");
        need(texture->base_rrtex==images.back(),"Base is exact largest stored original mip");
        for(const auto& entry:texture->original_palette)for(auto component:entry)need(component==0,"Kind2 invents no palette");
        const std::size_t headers[]{12,237,877,2747,8236},payloads[]{32,257,897,2767,8256};
        for(std::size_t i=0;i<5;++i){
            const auto& mip=texture->mips[i];const auto& proof=evidence->mips[i];
            const auto dimension=16u<<i;
            need(mip.width==dimension&&mip.height==dimension&&mip.source_header_offset==headers[i]&&
                mip.source_indices_offset==payloads[i],"All exact original mip dimensions/offsets retained");
            need(mip.rgba==Bytes(images[i].begin()+16,images[i].end()),"RGBA copied without another color/alpha operation");
            need(proof.sourceHeaderOffset==headers[i]&&proof.sourcePayloadOffset==payloads[i]&&
                proof.sourcePayloadSha256.size()==64&&proof.rrtexSha256.size()==64&&
                proof.rgbaSha256.size()==64&&proof.yuv420pSha256.size()==64,"Per-mip codec/source/output provenance retained");
        }
        need(evidence->colorPolicyKind==a::OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1&&
            !evidence->hardwarePixelParityVerified&&evidence->ffmpegVersion.rfind("ffmpeg version ",0)==0&&
            evidence->ffmpegSha256.size()==64&&evidence->converterSha256.size()==64&&evidence->generatorSha256.size()==64,
            "Codec identity retained without hardware-parity claim");
        auto copiedIdentity=*member.identity;auto wrongIdentity=member;wrongIdentity.identity=&copiedIdentity;
        auto copiedSource=Bytes(member.data,member.data+member.size);auto wrongData=member;wrongData.data=copiedSource.data();
        need(!cache->textureFor(wrongIdentity,0,member.size)&&!cache->textureFor(wrongData,0,member.size)&&
            !cache->textureFor(member,1,member.size-1)&&!cache->textureFor(member,0,member.size-1),
            "Only same authenticated archive member and whole source range accepted");
        need(!cache->load(archive,images,d)&&d.error==E::alreadyLoaded&&cache->textureFor(member,0,member.size)==texture,
            "Reload preserves immutable published cache");
        a::Texture48E sentinel;sentinel.kind=99;a::DecodeError48C decodeError;
        need(!a::decodeTexture48E(copiedSource,sentinel,decodeError)&&decodeError.code=="pst.kind"&&sentinel.kind==99,
            "Default decoder continues rejecting unowned kind2 transactionally");
        a::OriginalUiImageSource50KG source;source.identity=17;source.member=member;source.pstBytes=member.size;
        a::OriginalUiScreenAssets50KG noCache,screen;a::OriginalUiScreenAssetDiagnostic50KG sd;
        need(!noCache.load({source},sd)&&!noCache.loaded()&&sd.texture.code=="pst.kind","Screen kind2 needs explicit typed owner");
        need(screen.load({source},sd,cache.get()),"Immutable ScreenAssets cold bridge");
        const auto* stored=screen.decodedTexture(0);const auto* storedProof=screen.ipumProvenance(0);
        need(stored&&storedProof&&stored->kind==2&&stored->mips.size()==5&&stored->base_rrtex==images.back()&&
            storedProof->colorPolicyKind==a::OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1,
            "Screen retained original kind and every mip");
        need(screen.sourceProvenance(0)&&screen.sourceProvenance(0)->memberIdentity==member.identity&&
            screen.sourceProvenance(0)->pstBytes==member.size,"Exit source-provenance dependency preserved");
        need(!screen.ipumProvenance(1)&&!screen.decodedTexture(1),"Invalid screen indices reject");
        cache.reset();
        need(screen.decodedTexture(0)==stored&&screen.ipumProvenance(0)==storedProof&&
            storedProof->mips[4].sourcePayloadOffset==8256&&stored->base_rrtex==images.back(),
            "Screen owns pixel and evidence copies independently of input cache lifetime");
        std::cout<<"PASS authentic Rider IPUM cache: five source mips; "<<rejected
            <<" rejected cache mutations; explicit cold owner; hardware pixel parity unverified\n";
        return 0;
    }catch(const std::exception& e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}
}
