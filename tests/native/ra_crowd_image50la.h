#pragma once
#include "pod_materials50ed_fixture.h"
#include "../../src/assets/original_standard_ipum_image_lease50la.h"
#include <array>
#include <utility>
#include <cstring>
#include <memory>
#include <vector>

namespace ra_crowd_image50LA {
namespace a=rr::assets;
class Verifier final {
    struct Pin {std::size_t pst,pstBytes,header,payload,payloadBytes;const char* pstHash;const char* payloadHash;const char* outputHash;};
    // Root-produced manifest bf9fb839e1569bd9875960e27cd9e52016f6b8ccf97d3936df98b154824d62fe.
    // Actual fixed outputs; these are not supplied by a runtime caller.
    inline static constexpr Pin pins_[6]={
        {12u,35304u,11623u,11643u,23673u,"8c9c285f924853b221d9a1bf4a69f8c05f160dd256d73307f645d0bc09f6e219","6a189a6e6f2ce8d0a253108477dc051c47d1b445bd8b24c73952b935bf7dbe7d","fcfeb0c00938ab1bbae791a24d705c18989b4b222e9d492ab4593d67dff79e98"},
        {35316u,35486u,46945u,46965u,23837u,"365736f6a3e4bc336d165a919c0132e799e714264559903fc91b1c6bdc1144aa","8d36c299f0c0e0bf4b03ebd91b032a0b1ec907080c1d991d9f897d11e049f7af","93c38e0dbe0fd044a5e4b88b59cf5937ea106461d96e6ae484bfbab99cd78c60"},
        {70802u,35949u,82565u,82585u,24166u,"fc80a4063bd98d2ba3d6cca1c9cf81fcb37ad797b209bf9b3f44595d252b0b1f","b92f0253da647ed99434376e0a455d5c709fd590851eb5730bde00c9600c1ec9","2d5612b9cc8539d2b9d8045ace3d0319448ca730cbd99a1210d2e13327f72890"},
        {106751u,36392u,118726u,118746u,24397u,"0f651ba46edeb4e158d9462606f4b92956ac57b004e0e204d676675f4d45caa3","41252808860b102808dcaf983452cc86a37c54882a6e95e76bd56fad3961c2ac","f8239452fb91c2c4f3845990604d2d7990105807867f8617500eb1c687f5e215"},
        {143143u,35949u,154906u,154926u,24166u,"fc80a4063bd98d2ba3d6cca1c9cf81fcb37ad797b209bf9b3f44595d252b0b1f","b92f0253da647ed99434376e0a455d5c709fd590851eb5730bde00c9600c1ec9","2d5612b9cc8539d2b9d8045ace3d0319448ca730cbd99a1210d2e13327f72890"},
        {179092u,35486u,190721u,190741u,23837u,"365736f6a3e4bc336d165a919c0132e799e714264559903fc91b1c6bdc1144aa","8d36c299f0c0e0bf4b03ebd91b032a0b1ec907080c1d991d9f897d11e049f7af","93c38e0dbe0fd044a5e4b88b59cf5937ea106461d96e6ae484bfbab99cd78c60"},
    };
public:
    enum class Status : std::uint8_t {complete,input,sourceIdentity,sourceFrame,outputIdentity,allocation};
    struct Diagnostic {Status status=Status::input;const char* failure=nullptr;};
    // Existing frame0 API remains source/output-compatible.
    static bool verifyRaFrame0Mip4(
        const a::OriginalStandardTextureResolver50LA& resolver,std::size_t source,
        std::shared_ptr<const a::OriginalStandardIpumCompressedFrames50LA> compressed,
        const std::vector<std::uint8_t>& bytes,
        std::shared_ptr<const a::OriginalStandardIpumImageLease50LA>& out,Diagnostic& d) noexcept {
        return verifyFrame(resolver,source,std::move(compressed),0,bytes,out,d);
    }
    static bool verifyRaSixMip4(
        const a::OriginalStandardTextureResolver50LA& resolver,std::size_t source,
        std::shared_ptr<const a::OriginalStandardIpumCompressedFrames50LA> compressed,
        const std::array<std::vector<std::uint8_t>,6>& supplied,
        std::shared_ptr<const a::OriginalStandardIpumImageSet50LA>& out,Diagnostic& d) noexcept {
        d={};if(out){d.failure="output already owns a verified set";return false;}
        try {
            auto next=std::shared_ptr<a::OriginalStandardIpumImageSet50LA>(new a::OriginalStandardIpumImageSet50LA());
            for(std::size_t i=0;i<6;++i)
                if(!verifyFrame(resolver,source,compressed,i,supplied[i],next->frames_[i],d))return false;
            out=std::move(next);d.status=Status::complete;return true;
        }catch(...){d.status=Status::allocation;d.failure="six-frame image verification allocation failed";return false;}
    }
    static bool bindVerifiedRaSix(
        a::OriginalStandardTextureResolver50LA& resolver,
        const std::shared_ptr<const a::OriginalStandardIpumImageSet50LA>& images,
        const std::shared_ptr<const a::OriginalStandardMaterialContext50LA>& actualContext,
        a::OriginalStandardTextureDiagnostic50LA& d) noexcept {
        return resolver.bindIpumImages(images,actualContext,d);
    }
private:
    // No IO/codec/GL. Owned bytes are hashed before immutable publication.
    static bool verifyFrame(
        const a::OriginalStandardTextureResolver50LA& actualResolver,
        std::size_t actualSourceIndex,
        std::shared_ptr<const a::OriginalStandardIpumCompressedFrames50LA> compressed,
        std::size_t ordinal,const std::vector<std::uint8_t>& suppliedOriginal50FcRrtex,
        std::shared_ptr<const a::OriginalStandardIpumImageLease50LA>& out,
        Diagnostic& d) noexcept {
        d={};
        const auto reject=[&](Status status,const char* why){d.status=status;d.failure=why;return false;};
        if(out||ordinal>=6||!compressed||!compressed->compressedComplete()||
           compressed->copiedFrameCount()!=6||compressed->resolverIdentity()!=&actualResolver||
           compressed->sourceIndex()!=actualSourceIndex||!actualResolver.prepared())
            return reject(Status::input,"actual same-resolver complete compressed RA stage required");
        try {
            namespace hash=pod_materials50ed_fixture::detail;
            const auto* source=compressed->originalBytes();const auto* member=compressed->sourceMember();
            const auto* archive=compressed->archiveSha256();
            if(!source||source!=actualResolver.originalBytes(actualSourceIndex)||!member||
               hash::normalized(*member)!="TEXTURES/CROWD.ATM"||!archive||
               *archive!="1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2"||
               source->size()!=214578||hash::sha256(*source)!=
                 "76b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031")
                return reject(Status::sourceIdentity,"actual RA archive/member bytes differ");
            const auto& pin=pins_[ordinal];
            a::OriginalStandardIpumCompressedFrames50LA::FrameView frame;
            if(!compressed->frame(ordinal,frame)||frame.frame!=ordinal||frame.pstOffset!=pin.pst||frame.pstBytes!=pin.pstBytes||
               frame.sourceMipOrdinal!=4||frame.sourceHeaderOffset!=pin.header||frame.sourcePayloadOffset!=pin.payload||
               frame.sourcePayloadBytes!=pin.payloadBytes||frame.width!=256||frame.height!=256||
               !frame.allocation00||frame.codec18!=frame.allocation00+16||frame.codecBytes1C!=pin.payloadBytes-16||
               std::memcmp(frame.allocation00,source->data()+pin.payload,pin.payloadBytes))
                return reject(Status::sourceFrame,"actual distinct compressed frame allocation differs");
            const std::vector<std::uint8_t> pst(source->begin()+pin.pst,source->begin()+pin.pst+pin.pstBytes);
            const std::vector<std::uint8_t> payload(source->begin()+pin.payload,source->begin()+pin.payload+pin.payloadBytes);
            if(hash::sha256(pst)!=pin.pstHash||hash::sha256(payload)!=pin.payloadHash)
                return reject(Status::sourceFrame,"actual PST/payload fixed source pins differ");
            if(suppliedOriginal50FcRrtex.size()!=262160)
                return reject(Status::outputIdentity,"actual pinned256x256 RRTEX required");
            auto next=std::shared_ptr<a::OriginalStandardIpumImageLease50LA>(new a::OriginalStandardIpumImageLease50LA());
            next->compressed_=std::move(compressed);next->frame_=frame;
            next->rrtex_=suppliedOriginal50FcRrtex;
            const auto& image=next->rrtex_;
            // Frame0 preserves source_texture50fc_index.inc; other pins are real root outputs.
            if(hash::sha256(image)!=pin.outputHash||
               std::memcmp(image.data(),"RRTEX001",8)||hash::u32(image.data()+8)!=256||hash::u32(image.data()+12)!=256)
                return reject(Status::outputIdentity,"RRTEX differs from existing fixed50FC output");
            for(std::size_t p=16;p<image.size();p+=4)
                if((image[p]&7u)||(image[p+1]&7u)||(image[p+2]&7u)||
                   image[p+3]!=(image[p]||image[p+1]||image[p+2]?255:0))
                    return reject(Status::outputIdentity,"existing RGB5 expansion/TEXA255 AEM1 policy differs");
            out=std::move(next);d.status=Status::complete;return true;
        }catch(...){return reject(Status::allocation,"frame image verification allocation failed");}
    }
};
}
