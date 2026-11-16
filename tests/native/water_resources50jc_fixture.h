#pragma once
#include "../../src/assets/water_signature_cold50jc.h"
#include "../../src/assets/water_noise_cold50jc.h"
#include "pod_materials50ed_fixture.h"
#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace water_resources50JC {
// Cold source metadata and real byte storage. No live provider-ready flag.
struct Member {
    std::string name,sha256;
    std::uint32_t logicalOffset=0;
    std::size_t sourceBytes=0;
};
struct Signature {
    std::vector<Member> members;
    std::vector<rr::assets::WaterSignatureTexture50JC> textures;
    std::vector<rr::game::WaterTextureFrame50JC> frames;
    rr::assets::WaterSignatureCold50JC cold{};
};
struct Owner {
    rr::assets::WaterAuthoring50JC authored{};
    std::vector<Signature> signatures;
    rr::assets::WaterNoiseTexture50JC noise{};
    Member waterMember,noiseMember;
    bool coldResourcesBound=false;
    Owner()=default;
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
};
inline bool sameName(const std::string& a,const std::string& b) noexcept {
    if(a.size()!=b.size())return false;
    for(std::size_t i=0;i<a.size();++i){
        const auto lower=[](char c){return c>='A'&&c<='Z'?char(c+32):c;};
        if(lower(a[i])!=lower(b[i]))return false;
    }
    return true;
}
// A unique actual catalog hit is required. No disk probing, alternate mount,
// inferred missing resource, low-mip substitution or generated texture.
inline bool member(const rr::assets::ResArchive48C& archive,const std::string& name,
    std::vector<std::uint8_t>& bytes,Member& identity,std::string& error) {
    const rr::assets::ResourceEntry48C* found=nullptr;
    for(const auto& entry:archive.entries)if(sameName(entry.name,name)){
        if(found){error="water resource has duplicate catalog identities";return false;}found=&entry;
    }
    if(!found){error="water resource requires an unrepresented original catalog continuation: "+name;return false;}
    rr::assets::DecodeError48C d;
    if(!rr::assets::extractResMember48C(archive,found->name,bytes,d)){
        error="water resource extraction rejected: "+d.code;return false;
    }
    identity={found->name,pod_materials50ed_fixture::detail::sha256(bytes),
        found->logical_offset,bytes.size()};return true;
}
// One common water resource loader for every course whose SCB
// Environment.WaterFile (2771D4 -> Environment+0x110) is not `none`. The WTT
// drives everything: signature list and frames (2B9B9C/2BE2E0/24DA30), base
// displacement (EnableBaseDisplacement, 24DA30 PST, IPU, then TGA). Archive
// access goes through a member reader, so the plain ResArchive48C mount and the
// retained IndexedResArchive48C course sources share this body. Callers must
// pass members of the same authenticated mount the WTT came from.
using MemberReader=std::function<bool(const std::string&,std::vector<std::uint8_t>&,Member&,std::string&)>;
inline bool prepareAuthored(const MemberReader& readMember,
    const std::function<bool(const std::string&)>& memberExists,
    const std::string& waterName,std::uint32_t waterLogicalOffset,
    const std::vector<std::uint8_t>& waterBytes,std::shared_ptr<Owner>& output,std::string& error) {
    if(waterName.empty()||waterBytes.empty()){error="water resources require the course WaterFile member";return false;}
    auto staged=std::make_shared<Owner>();rr::assets::AiAuthoringDiagnostic50IU authoring;
    if(!rr::assets::readWaterAuthoring50JC(reinterpret_cast<const char*>(waterBytes.data()),
        waterBytes.size(),staged->authored,authoring)){
        error="original WTT field admission rejected at "+std::to_string(authoring.sourcePC);return false;
    }
    // Pools and channel4 have additional concrete construction owners. Keep
    // their real authored values and reject before claiming this cold subset.
    if(staged->authored.general.poolCount!=0){error="native water pool construction is unjoined";return false;}
    staged->waterMember={waterName,pod_materials50ed_fixture::detail::sha256(waterBytes),waterLogicalOffset,waterBytes.size()};
    const auto count=static_cast<std::size_t>(staged->authored.general.signatureCount);
    staged->signatures.resize(count);
    for(std::size_t i=0;i<count;++i){
        const auto& a=staged->authored.signatures[i];auto& s=staged->signatures[i];
        if(a.frameCount<=0||a.frameCount>4096||a.channel<0||a.channel>3){
            error="water signature frame/channel owner lies outside the joined cold subset";return false;
        }
        const std::string name=a.filename.data();
        // Genuine path composition: fresh1A9EE0 sets root tracks\\;2B0800
        // joins the original empty suffix;2B9B9C adds Sigs\\ and authored name;
        //2BE2E0 appends decimal frame index;24DA30 selects the PST extension.
        if(name.empty()||name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-")!=std::string::npos){
            error="water signature name needs an unrepresented source path interpretation";return false;
        }
        const auto frames=static_cast<std::size_t>(a.frameCount);
        s.members.resize(frames);s.textures.resize(frames);s.frames.resize(frames);
        for(std::size_t f=0;f<frames;++f){
            char number[16]{};std::snprintf(number,sizeof(number),"%04u",static_cast<unsigned>(f));
            const auto path=std::string("tracks\\Sigs\\")+name+number+".pst";
            std::vector<std::uint8_t> bytes;
            if(!readMember(path,bytes,s.members[f],error))return false;
            rr::assets::DecodeError48C decode;
            if(!rr::assets::readWaterSignatureTexture50JC(bytes,s.textures[f],decode)){
                error="original signature texture admission rejected: "+decode.code;return false;
            }
            const auto& texture=s.textures[f];
            if(f&&(texture.dimension!=s.textures[0].dimension||
                   texture.pixelStride!=s.textures[0].pixelStride)){
                error="water signature frame layout changes need a broader source owner";return false;
            }
            s.frames[f]={{texture.indices.data(),texture.indices.size(),true},texture.rowStride};
        }
        const auto cold=rr::assets::prepareWaterSignatureCold50JC(staged->authored,i,
            s.frames.data(),s.frames.size(),s.textures[0].dimension,s.textures[0].pixelStride,s.cold);
        if(cold.status!=rr::game::WaterHeightStatus50JC::Complete){
            error="original signature cold projection rejected at "+std::to_string(cold.sourcePC);return false;
        }
    }
    if(!staged->authored.general.enableBaseDisplacement){
        // Water+0x49C = 0 would select an Absent noise owner, which no shipped
        // course authors and the runtime has no admitted path for yet.
        error="water base displacement disabled needs an Absent noise owner";return false;
    }
    // Original24DA30 tries PST then IPU before TGA. No shipped course has the
    // earlier candidates; if a mount does, do not silently choose TGA.
    if(memberExists("tracks\\baseDisplacement.pst")||memberExists("tracks\\baseDisplacement.ipu")){
        error="water base displacement has an earlier unjoined texture branch";return false;
    }
    std::vector<std::uint8_t> noiseBytes;
    if(!readMember(rr::assets::gcWaterNoiseMember50JC,noiseBytes,staged->noiseMember,error))return false;
    // The shipped baseDisplacement.tga (GC and MC copies are byte-identical) is a
    // RECEIPT: the original reads whatever noise member the course archive holds
    // (24DA30, by name). Its decode below bounds it (readWaterNoiseTexture50JC).
    if(staged->noiseMember.sha256!=rr::assets::gcWaterNoiseSha25650JC)
        std::fprintf(stderr,"course_member_identity site=water_noise member=%s bytes=%zu stock=0\n",
            rr::assets::gcWaterNoiseMember50JC,noiseBytes.size());
    rr::assets::DecodeError48C decode;
    if(!rr::assets::readWaterNoiseTexture50JC(noiseBytes,staged->noise,decode)){
        error="original noise texture admission rejected: "+decode.code;return false;
    }
    staged->coldResourcesBound=true;output=std::move(staged);error.clear();return true;
}
// Plain decoded-archive mount (authored_runtime_ground50cp): the archive bytes
// were authenticated by the caller (request.archiveSha256).
inline bool prepare(const rr::assets::ResArchive48C& archive,
    const char* authenticatedArchiveSha256,const std::string& waterName,
    std::uint32_t waterLogicalOffset,const std::vector<std::uint8_t>& waterBytes,
    std::shared_ptr<Owner>& output,std::string& error) {
    if(!authenticatedArchiveSha256||!*authenticatedArchiveSha256){
        error="native water resources require an authenticated track archive";return false;
    }
    return prepareAuthored(
        [&](const std::string& name,std::vector<std::uint8_t>& bytes,Member& identity,std::string& e){
            return member(archive,name,bytes,identity,e);},
        [&](const std::string& name){
            for(const auto& entry:archive.entries)if(sameName(entry.name,name))return true;return false;},
        waterName,waterLogicalOffset,waterBytes,output,error);
}
} // namespace water_resources50JC
