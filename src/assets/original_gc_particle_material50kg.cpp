#include "original_gc_particle_material50kg.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>

namespace rr::assets {
namespace {
using Field=OriginalUiGsField50KG;
using Error=OriginalUiMaterialError50KG;
constexpr OriginalGcParticleMember50KG psmIdentity{
    "particle\\demoprt.psm",5022u,
    "ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1"};
constexpr OriginalGcParticleMember50KG pstIdentity{
    "particle\\demoprt.pst",132124u,
    "3c454a0bd95c12cd75168ec3da3c4ced43e60e5a1d1c4536ec65583d0350a165"};
std::uint32_t rotateRight(std::uint32_t x, unsigned n) noexcept {
    return (x >> n) | (x << (32u - n));
}
// Standard SHA-256, fixed stack workspace; no member-sized temporary copies.
// Constants are the SHA-256 constants also retained in audio_assets50hc.cpp.
void shaBlock(const std::uint8_t* block, std::array<std::uint32_t, 8>& h) noexcept {
    constexpr std::uint32_t k[64] = {
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
    for (unsigned i = 0; i < 16; ++i)
        for (unsigned j = 0; j < 4; ++j) w[i] = (w[i] << 8) | block[i * 4 + j];
    for (unsigned i = 16; i < 64; ++i) {
        const auto s0 = rotateRight(w[i-15],7) ^ rotateRight(w[i-15],18) ^ (w[i-15] >> 3);
        const auto s1 = rotateRight(w[i-2],17) ^ rotateRight(w[i-2],19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    auto a=h[0], b=h[1], c=h[2], d=h[3], e=h[4], f=h[5], g=h[6], v=h[7];
    for (unsigned i = 0; i < 64; ++i) {
        const auto s1 = rotateRight(e,6) ^ rotateRight(e,11) ^ rotateRight(e,25);
        const auto ch = (e & f) ^ (~e & g);
        const auto t1 = v + s1 + ch + k[i] + w[i];
        const auto s0 = rotateRight(a,2) ^ rotateRight(a,13) ^ rotateRight(a,22);
        const auto maj = (a & b) ^ (a & c) ^ (b & c);
        const auto t2 = s0 + maj;
        v=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=v;
}
bool hashMatches(const std::uint8_t* data, std::size_t size,
    const char* expected) noexcept {
    std::array<std::uint32_t, 8> h{{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
        0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}};
    std::size_t at = 0;
    for (; size - at >= 64; at += 64) {
        shaBlock(data + at, h);
    }
    std::array<std::uint8_t, 128> tail{};
    const std::size_t remainder = size - at;
    if (remainder) std::memcpy(tail.data(), data + at, remainder);
    tail[remainder] = 0x80;
    const std::size_t padded = remainder < 56 ? 64 : 128;
    const std::uint64_t bits = static_cast<std::uint64_t>(size) * 8u;
    for (unsigned i = 0; i < 8; ++i)
        tail[padded - 1 - i] = static_cast<std::uint8_t>(bits >> (i * 8));
    shaBlock(tail.data(), h);
    if (padded == 128) shaBlock(tail.data() + 64, h);
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned i = 0; i < 64; ++i)
        if (expected[i] != hex[(h[i / 8] >> ((7u - i % 8) * 4)) & 15u])
            return false;
    return true;
}
struct Spec {Field field;std::uint32_t pc[2];bool global=false;};
// Actual2480D0 getter PCs. Shared COLCLAMP/PABE are unsuffixed; PRMODE CTXT
// is forced0/1 by the builder, not inherited or read from the authored PSM.
constexpr Spec specs[]={
    {Field::textureColorComponent,{0x248494,0x2484f8}},
    {Field::textureFunction,{0x2484ac,0x248510}},
    {Field::sourceAlphaValueWhenAis0,{0x24876c,0x2487f8}},
    {Field::sourceAlphaValueWhenAis1,{0x248784,0x248810}},
    {Field::methodOfExpandingTextureAlpha,{0x24879c,0x248828}},
    {Field::inputColorA,{0x248894,0x2489b0}},
    {Field::inputColorB,{0x2488ac,0x2489c8}},
    {Field::inputAlphaC,{0x2488c4,0x2489e0}},
    {Field::inputColorD,{0x2488f4,0x248a10}},
    {Field::fixedAlphaValue,{0x24890c,0x248a28}},
    {Field::wrapModeHorizontal,{0x248ad4,0x248be4}},
    {Field::wrapModeVertical,{0x248aec,0x248bfc}},
    {Field::minClampParameterInUDirection,{0x248b04,0x248c14}},
    {Field::maxClampParameterInUDirection,{0x248b1c,0x248c2c}},
    {Field::minClampParameterInVDirection,{0x248b34,0x248c44}},
    {Field::maxClampParameterInVDirection,{0x248b4c,0x248c5c}},
    {Field::colorClampingMethod,{0x248424,0x248424},true},
    {Field::alphaCorrectionValue,{0x248ccc,0x248d00}},
    {Field::alphaBlendingControlInUnitsOfPixels,{0x248458,0x248458},true},
    {Field::gouraudShadingOnOff,{0x248d64,0x248ec0}},
    {Field::textureMappingOnOff,{0x248d7c,0x248ed8}},
    {Field::foggingOnOff,{0x248d94,0x248ef0}},
    {Field::alphaBlendingOnOff,{0x248dac,0x248f08}},
    {Field::onePassAntiAliasingOnOff,{0x248dc4,0x248f20}},
    {Field::textureCoordinatesSTQorUV,{0x248ddc,0x248f38}},
    {Field::fragmentValueControlFixedOrNot,{0x248df4,0x248f50}},
    {Field::alphaTestOnOff,{0x249020,0x24917c}},
    {Field::alphaTestMethod,{0x249038,0x249194}},
    {Field::alphaReferenceValue,{0x249050,0x2491ac}},
    {Field::processingMethodWhenFailedInAlphaTest,{0x249068,0x2491c4}},
    {Field::destinationAlphaTestOnOff,{0x249080,0x2491dc}},
    {Field::destinationAlphaTestMode,{0x249098,0x2491f4}},
    {Field::depthTestMethod,{0x2490b0,0x24920c}},
    {Field::zWriteDisabled,{0x2492a8,0x249314}}
};
bool fail(OriginalUiMaterialDiagnostic50KG& d,Error error,const char* field,std::uint32_t pc=0) noexcept {
    d.error=error;d.sourcePC=pc;
    try{d.field=field;}catch(...){d.field.clear();}
    return false;
}
std::string key(const Spec& spec,unsigned state) {
    const auto* name=originalUiGsFieldName50KG(spec.field);
    return spec.global?std::string(name):std::string(name)+"_"+std::to_string(state+1);
}
const OriginalUiMemberIdentity50KG* defaultMtIdentity() noexcept {
    const auto* archive=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart);
    if(!archive)return nullptr;
    for(std::size_t i=0;i<archive->memberCount;++i){
        const auto* member=originalUiMemberIdentity50KG(OriginalUiArchiveId50KG::appStart,i);
        if(member&&std::strcmp(member->name,"AppStart\\DefaultMT.psm")==0)return member;
    }
    return nullptr;
}
}
const OriginalGcParticleMember50KG& originalGcParticlePsm50KG() noexcept {return psmIdentity;}
const OriginalGcParticleMember50KG& originalGcParticlePst50KG() noexcept {return pstIdentity;}
bool authenticOriginalGcParticlePsm50KG(const std::vector<std::uint8_t>& bytes) noexcept {
    return bytes.size()==psmIdentity.bytes&&hashMatches(bytes.data(),bytes.size(),psmIdentity.sha256);
}
bool authenticOriginalGcParticlePst50KG(const std::vector<std::uint8_t>& bytes) noexcept {
    return bytes.size()==pstIdentity.bytes&&hashMatches(bytes.data(),bytes.size(),pstIdentity.sha256);
}
bool prepareOriginalGcParticleMaterial50KG(const std::vector<std::uint8_t>& bytes,
    const OriginalUiMaterialTemplates50KG& templates,const OriginalUiMaterialRuntime50KG& runtime,
    const Texture48E& texture,OriginalGcParticleMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};d.pass=1;
    try{
        // The stock PSM identity is a receipt (all 13 stock PSMs are GC's bytes today); the
        // material is admitted by its own structure below, on every course.
        if(bytes.empty()||bytes.size()>64u*1024u)return fail(d,Error::archive,"track particle PSM extent");
        std::fprintf(stderr,"common_particle_psm bytes=%zu receipt=%s\n",
            bytes.size(),authenticOriginalGcParticlePsm50KG(bytes)?"match":"differs(gc)");
        const auto* inherited=templates.get(OriginalUiTemplate50KG::defaultMt);
        const auto* expectedDefault=defaultMtIdentity();
        if(!templates.loaded()||!inherited||!expectedDefault||inherited->identity!=expectedDefault)
            return fail(d,Error::templateOwner,"actual APPSTART DefaultMT",0x160ff4);
        if(!runtime.overrideKnown)return fail(d,Error::templateOwner,"owned particle material runtime",0x248924);
        if(runtime.override435670)return fail(d,Error::unsupportedValue,"435670 override",0x248924);
        Material48E source;DecodeError48C decode;
        if(!decodeMaterial48E(bytes,source,decode,nullptr,bytes.size())){
            d.document.byteOffset=decode.offset;
            return fail(d,Error::document,"GC demoprt.psm");
        }
        if(source.passes.size()!=1||!source.inactive_sections.empty())return fail(d,Error::passCount,"GC single ParticleCode pass");
        const auto& pass=source.passes[0];
        if(pass.pass!=1||pass.microprogram!="ParticleCode"||pass.texture_count_defaulted||pass.textures.size()!=2)
            return fail(d,Error::profile,"GC ParticleCode two contexts",0x2474a0);
        // Both ParticleCode contexts name the one atlas the course's PRT names (C11: the name
        // is data, not "demoprt").
        for(unsigned state=0;state<2;++state)
            if(pass.textures[state].slot!=state+1||pass.textures[state].name.empty()||
               pass.textures[state].name!=pass.textures[0].name)
                return fail(d,Error::textureName,"track ParticleCode context map",0x247338);
        // Generic PSM lexical decoder preserves only present values here.
        // Its source_offset is the original LINE start, not a value offset;
        // retain that for diagnostics without inventing a UI DAT identity.
        OriginalUiDatSection50KG section;section.name="ShaderPass_1";
        for(const auto& field:pass.fields.authored)
            section.values.push_back({field.name,field.value,field.source_offset,field.source_offset,true});
        const auto integer=[&](const std::string& name,std::int32_t fallback,
            std::int32_t& value,std::uint32_t pc=0){
            return originalUiInteger50KG(section,name,fallback,value,d.document,pc)||
                fail(d,Error::value,name.c_str(),pc);
        };
        if(texture.revision!=1||texture.flags||texture.kind!=1||texture.mips.size()!=1)
            return fail(d,Error::texture,"GC single indexed8 demoprt mip");
        const auto& mip=texture.mips[0];
        // The atlas extent is the PST's own (stock 128x1024); PRT frame UVs are normalized.
        if(!mip.width||!mip.height||mip.width>1024u||mip.height>1024u||
           mip.rgba.size()!=std::size_t(mip.width)*mip.height*4u)
            return fail(d,Error::texture,"track particle atlas extent");
        OriginalGcParticleMaterial50KG next;
        next.identity=&psmIdentity;next.textureIdentity=&pstIdentity;next.templateIdentity=inherited->identity;
        for(std::size_t i=3;i<mip.rgba.size();i+=4)
            if(mip.rgba[i]>next.maximumTextureAlpha)next.maximumTextureAlpha=mip.rgba[i];
        for(unsigned state=0;state<2;++state){
            const auto suffix="_"+std::to_string(state+1);
            std::int32_t uv=0;
            if(!integer("TextureCoordinateIndex"+suffix,0,uv,0x247338)||uv!=0)
                return fail(d,Error::textureName,"GC particle UV0",0x247338);
            auto gs=inherited->gs;
            for(const auto& spec:specs){
                auto& value=gs.fields[static_cast<std::size_t>(spec.field)].value;
                if(spec.field==Field::inputColorD&&
                    !integer("inputAlphaD"+suffix,value,value,state?0x2489f8:0x2488dc))return false;
                if(!integer(key(spec,state),value,value,spec.pc[state]))return false;
            }
            // CTXT is forced by the two-map builder, not inherited DefaultMT.
            gs.fields[static_cast<std::size_t>(Field::environmentContext0or1)].value=static_cast<std::int32_t>(state);
            const auto expect=[&](Field field,std::int32_t value){
                return gs[field].value==value||fail(d,Error::unsupportedValue,originalUiGsFieldName50KG(field));
            };
            if(!expect(Field::textureColorComponent,1)||!expect(Field::textureFunction,0)||
                !expect(Field::colorClampingMethod,1)||!expect(Field::alphaCorrectionValue,0)||
                !expect(Field::alphaBlendingControlInUnitsOfPixels,0)||!expect(Field::gouraudShadingOnOff,1)||
                !expect(Field::textureMappingOnOff,1)||!expect(Field::foggingOnOff,1)||
                !expect(Field::alphaBlendingOnOff,1)||!expect(Field::onePassAntiAliasingOnOff,0)||
                !expect(Field::textureCoordinatesSTQorUV,0)||!expect(Field::fragmentValueControlFixedOrNot,0)||
                !expect(Field::alphaTestOnOff,1)||!expect(Field::alphaTestMethod,6)||
                !expect(Field::alphaReferenceValue,0)||!expect(Field::processingMethodWhenFailedInAlphaTest,0)||
                !expect(Field::destinationAlphaTestOnOff,0)||!expect(Field::depthTestMethod,2)||
                !expect(Field::zWriteDisabled,1)||!expect(Field::inputColorA,0)||
                !expect(Field::inputColorB,state?2:1)||!expect(Field::inputAlphaC,0)||
                !expect(Field::inputColorD,1)||!expect(Field::wrapModeHorizontal,0)||!expect(Field::wrapModeVertical,0))
                return false;
            // Original pass-local TEX1 defaults from24853C/24863C; these are
            // not substituted template defaults. Exact GC leaves them empty.
            const char* names[]={"lodCalculationMethod","maximumMipLevel","magFilter","minFilter","L","K"};
            constexpr std::int32_t samplerDefaults[]={0,0,1,5,0,-75};
            for(unsigned i=0;i<6;++i){
                std::int32_t value=samplerDefaults[i];
                if(!integer(std::string(names[i])+suffix,samplerDefaults[i],value)||value!=samplerDefaults[i])
                    return fail(d,Error::unsupportedValue,"GC ParticleCode TEX1 sampler",state?0x24863c:0x24853c);
            }
            auto& policy=next.states[state];
            policy.blend=state?OriginalUiMaterialBlend50KG::additive:OriginalUiMaterialBlend50KG::sourceAlpha;
            policy.minification=policy.magnification=OriginalUiMaterialFilter50KG::linear;
            policy.u=policy.v=OriginalUiMaterialWrap50KG::repeat;
            policy.depthMethod=2;policy.depthWrite=false;policy.environmentContext=static_cast<std::uint8_t>(state);
            policy.requiresOriginalDepthOwner=true;policy.requiresSeparateAlphaStore=true;
        }
        next.fogEnabled=true;next.alphaTestGreaterZero=true;
        // This leaf has DATE=0. The scene owner must independently account for
        // later consumers and target transitions; this is not blanket waiver.
        next.noDestinationAlphaConsumer=true;
        output=next;d={};return true;
    }catch(...){return fail(d,Error::allocation,"GC particle material");}
}
}
