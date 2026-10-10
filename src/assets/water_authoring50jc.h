#pragma once
#include "ai_authoring50iu.h"
#include "import_texture48e.h"
#include <array>
#include <utility>

namespace rr::assets {
// Consumed native physics/time subset of2B92F0. Defaults below are its actual
//158B80/158B30 arguments, never values copied from one selected WTT asset.
struct WaterGeneralAuthoring50JC {
    float baseHeight=20.f,worldWidthHeight=2304.f,heightRange=40.f;
    float channelScale[4]{},framesPerSecond[5]{};
    float wakeScale=10.f,poolDepth=100.f,swellHeight=0.f,swellPeriod=5.f;
    std::int32_t renderInfinitePlane=1,enableBaseDisplacement=1;
    std::int32_t signatureCount=0,poolCount=0;
    std::uint64_t loaded=0;
};
struct WaterSignatureAuthoring50JC {
    std::array<char,128> filename{};
    float position[3]{},width[2]{},velocity[2]{},rotation=0.f,tileScale[2]{1.f,1.f};
    std::int32_t frameCount=0,tile=1,channel=0,physicsType=0;
    std::uint32_t loaded=0;
    std::size_t sourceSection=0,sourceFilename=0;
    bool sectionPresent=false;
};
inline constexpr std::size_t maxWaterSignatures50JC=32;
struct WaterAuthoring50JC {
    WaterGeneralAuthoring50JC general{};
    std::array<WaterSignatureAuthoring50JC,maxWaterSignatures50JC> signatures{};
    bool bound=false;
};
// ASCII INI is the actual GC WTT representation. Reuses the existing bounded
// source scanner/decimal conversion. Quoted filenames follow15881C..158864;
// escapes, embedded quotes and unpaired quotes are outside this native subset.
// Only selected original signature sections are consumed; pool construction
// stays explicit through general.poolCount and is not silently erased.
inline bool readWaterAuthoring50JC(const char* text,std::size_t bytes,
    WaterAuthoring50JC& output,AiAuthoringDiagnostic50IU& diagnostic) noexcept {
    using namespace ai_authoring_detail50IU;
    diagnostic={};WaterAuthoring50JC staged{};
    bool generalSeen=false;std::int32_t signature=-1;
    auto visit=[&](Slice section,Slice key,Slice value) noexcept {
        if(!key.n){
            signature=-1;
            if(equal(section,"General Information")){generalSeen=true;return true;}
            constexpr const char prefix[]="Signature Wave ";
            if(section.n>sizeof(prefix)-1u&&
               equal({section.p,sizeof(prefix)-1u,section.offset},prefix)){
                const Slice suffix{section.p+sizeof(prefix)-1u,section.n-(sizeof(prefix)-1u),
                    section.offset+sizeof(prefix)-1u};
                // Source formats %d and looks up the exact section name.
                // Reject numeric aliases that generic integer() would accept.
                if(!suffix.n||suffix.p[0]<'1'||suffix.p[0]>'9')
                    return fail(diagnostic,AiAuthoringError50IU::Text,0x2b9900u,suffix.offset);
                std::int32_t number=0;
                if(!integer(suffix,number)||number<1||
                    number>static_cast<std::int32_t>(maxWaterSignatures50JC))
                    return fail(diagnostic,AiAuthoringError50IU::Limit,0x2b98f4u,section.offset);
                signature=number-1;auto& s=staged.signatures[signature];
                if(s.sectionPresent)return fail(diagnostic,AiAuthoringError50IU::Duplicate,0x2b9900u,section.offset);
                s.sectionPresent=true;s.sourceSection=section.offset;
            }
            return true;
        }
        auto number=[&](float& destination) noexcept {
            if(!parsePodPhysicsNumber49N(value.p,value.n,destination))
                return fail(diagnostic,AiAuthoringError50IU::Number,0x158b80u,value.offset);
            return true;
        };
        auto whole=[&](std::int32_t& destination) noexcept {
            if(!integer(value,destination))
                return fail(diagnostic,AiAuthoringError50IU::Number,0x158b30u,value.offset);
            return true;
        };
        if(equal(section,"General Information")){
            auto& g=staged.general;
            struct Float {const char* key;float* value;};
            const Float floats[]={
                {"WaterBaseHeight",&g.baseHeight},{"WorldWidthHeightInFeet",&g.worldWidthHeight},
                {"WaterHeightRange",&g.heightRange},{"ChannelScale0",&g.channelScale[0]},
                {"ChannelScale1",&g.channelScale[1]},{"ChannelScale2",&g.channelScale[2]},
                {"ChannelScale3",&g.channelScale[3]},{"FramesPerSecond0",&g.framesPerSecond[0]},
                {"FramesPerSecond1",&g.framesPerSecond[1]},{"FramesPerSecond2",&g.framesPerSecond[2]},
                {"FramesPerSecond3",&g.framesPerSecond[3]},{"FramesPerSecond4",&g.framesPerSecond[4]},
                {"WakeScale",&g.wakeScale},{"PoolDepth",&g.poolDepth},
                {"WaterBaseSwellHeight",&g.swellHeight},{"WaterBaseSwellPeriod",&g.swellPeriod}};
            for(unsigned i=0;i<sizeof(floats)/sizeof(floats[0]);++i)if(equal(key,floats[i].key)){
                if(!number(*floats[i].value))return false;g.loaded|=std::uint64_t(1)<<i;return true;
            }
            struct Int {const char* key;std::int32_t* value;};
            const Int integers[]={{"RenderInfinitePlane",&g.renderInfinitePlane},
                {"EnableBaseDisplacement",&g.enableBaseDisplacement},
                {"NumSignatureWaves",&g.signatureCount},{"NumPools",&g.poolCount}};
            for(unsigned i=0;i<sizeof(integers)/sizeof(integers[0]);++i)if(equal(key,integers[i].key)){
                if(!whole(*integers[i].value))return false;g.loaded|=std::uint64_t(1)<<(16u+i);return true;
            }
            return true;
        }
        if(signature<0)return true;
        auto& s=staged.signatures[signature];
        if(equal(key,"filename")){
            auto name=value;
            if(name.n&&name.p[0]=='"'){
                if(name.n<2||name.p[name.n-1]!='"')return fail(diagnostic,AiAuthoringError50IU::Text,0x15881cu,value.offset);
                ++name.p;name.n-=2;++name.offset;
            }
            if(name.n>=s.filename.size())return fail(diagnostic,AiAuthoringError50IU::Limit,0x2b9920u,value.offset);
            for(std::size_t i=0;i<name.n;++i)if(name.p[i]=='"')return fail(diagnostic,AiAuthoringError50IU::Text,0x158854u,value.offset+i);
            std::memcpy(s.filename.data(),name.p,name.n);s.sourceFilename=value.offset;s.loaded|=1u;return true;
        }
        struct Float {const char* key;float* value;};
        const Float floats[]={{"positionX",&s.position[0]},{"positionY",&s.position[1]},
            {"positionZ",&s.position[2]},{"widthX",&s.width[0]},{"widthZ",&s.width[1]},
            {"velocityX",&s.velocity[0]},{"velocityZ",&s.velocity[1]},
            {"rotation",&s.rotation},{"tileX",&s.tileScale[0]},{"tileZ",&s.tileScale[1]}};
        for(unsigned i=0;i<sizeof(floats)/sizeof(floats[0]);++i)if(equal(key,floats[i].key)){
            if(!number(*floats[i].value))return false;s.loaded|=1u<<(i+1u);return true;
        }
        struct Int {const char* key;std::int32_t* value;};
        const Int integers[]={{"numFrames",&s.frameCount},{"tile",&s.tile},
            {"channel",&s.channel},{"PhysicsType",&s.physicsType}};
        for(unsigned i=0;i<sizeof(integers)/sizeof(integers[0]);++i)if(equal(key,integers[i].key)){
            if(!whole(*integers[i].value))return false;s.loaded|=1u<<(i+11u);return true;
        }
        return true;
    };
    if(!scan(text,bytes,64u*1024u,0x2b92f0u,visit,diagnostic))return false;
    if(!generalSeen)return fail(diagnostic,AiAuthoringError50IU::Missing,0x2b9354u,0);
    const auto& g=staged.general;
    if(g.signatureCount<0||g.signatureCount>static_cast<std::int32_t>(maxWaterSignatures50JC)||g.poolCount<0)
        return fail(diagnostic,AiAuthoringError50IU::Limit,0x2b98ccu,0);
    for(std::int32_t i=0;i<g.signatureCount;++i)if(!staged.signatures[i].sectionPresent)
        return fail(diagnostic,AiAuthoringError50IU::Missing,0x2b9900u,0);
    staged.bound=true;output=staged;return true;
}

struct WaterSignatureTexture50JC {
    std::uint32_t dimension=0,rowStride=0,pixelStride=0;
    std::size_t sourceHeader=0,sourceIndices=0;
    std::vector<std::uint8_t> indices;
    bool bound=false;
};
//2BE250 ->24DA30 ->24E880, with actual flags0 and scoped382FC8=0.
// PST kind1 sets textureAC=8 (29E790), so2518E0/29E850 expose raw one-byte
// indices, not palette-expanded RGBA. Source loading skips smaller mips and
// retains the final/largest level as texture100. No GPU swizzle is performed
// while382FC8 is zero. Caller must supply the authenticated selected member.
inline bool readWaterSignatureTexture50JC(const std::vector<std::uint8_t>& input,
    WaterSignatureTexture50JC& output,DecodeError48C& diagnostic) {
    Texture48E decoded;
    if(!decodeTexture48E(input,decoded,diagnostic))return false;
    const auto& mip=decoded.mips.back();
    if(mip.width!=mip.height){
        diagnostic={DecodeKind48C::unsupported,"water.signature_square",mip.source_header_offset,
            "Signature query wraps both axes using original width"};return false;
    }
    const auto count=std::size_t(mip.width)*mip.height;
    if(mip.source_indices_offset>input.size()||count>input.size()-mip.source_indices_offset){
        diagnostic={DecodeKind48C::malformed,"water.signature_extent",mip.source_indices_offset,
            "Signature source plane exceeds authenticated member"};return false;
    }
    WaterSignatureTexture50JC staged{};staged.dimension=mip.width;
    staged.rowStride=mip.width;staged.pixelStride=1;
    staged.sourceHeader=mip.source_header_offset;staged.sourceIndices=mip.source_indices_offset;
    staged.indices.assign(input.begin()+mip.source_indices_offset,
        input.begin()+mip.source_indices_offset+count);
    staged.bound=true;output=std::move(staged);return true;
}
} // namespace rr::assets
