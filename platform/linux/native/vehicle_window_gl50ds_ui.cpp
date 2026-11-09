#include <cstdio>
#include <chrono>
#include <cstdlib>
#include <type_traits>
#include "vehicle_window_gl50ds_storage.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include "original_ui_garage_canonical_submission50kg.h"
#include "original_ui_coverage_batch50kg.h"
#include "original_ui_coverage_disjoint50kg.h"
#include "original_ui_rider_draw_contribution50kg.h"
#include <vector>

namespace rr::platform {
bool VehicleWindowGl50DS::Storage::waitGarageTargetFence50KG()noexcept {
    // Exactly the completion the former glFinish established, observed later.
    if(!garageTargetFence50KG)return true;
    auto* fence=garageTargetFence50KG;garageTargetFence50KG=nullptr;
    bool waited=false;
    if(gl.clientWaitSync)for(;;){
        const U r=gl.clientWaitSync(fence,1u,1000000000ull); //SYNC_FLUSH_COMMANDS_BIT, 1 s
        if(r==0x911Au||r==0x911Cu){waited=true;break;}   //ALREADY_SIGNALED / CONDITION_SATISFIED
        if(r!=0x911Bu)break;                               //not TIMEOUT_EXPIRED: WAIT_FAILED
    }
    if(!waited&&gl.finish)gl.finish();
    if(gl.deleteSync)gl.deleteSync(fence);
    return true;
}
namespace {
// Original UI fragment program (shared by the main UI program and the
// Garage dual-source specular variant, which adds RR_UI_DUAL_SOURCE_SPECULAR).
constexpr const char* originalUiFragmentSource50KG = R"GLSL(
in vec2 uiUv;
noperspective in vec3 uiSourceStq;
uniform int uiSourceRaster;
noperspective in vec4 uiColor;
noperspective in float uiParticleFog;
uniform sampler2D uiImage;
uniform float uiAlphaScale;
uniform int uiTextured;
uniform int uiSourceFogEnabled;
uniform int uiSourceAlphaTest;
uniform vec4 uiParticleFogColor;
#ifndef RR_UI_COHERENT_FRAMEBUFFER_FETCH
uniform sampler2D uiSourceTarget;
#endif
uniform int uiSourceComposition;
uniform int uiCoveragePayload;
uniform int uiSourceBlend;
uniform int gcParticleMode;
#ifdef RR_UI_COHERENT_FRAMEBUFFER_FETCH
inout vec4 viewerOutput;
#else
out vec4 viewerOutput;
#endif
#ifdef RR_UI_DUAL_SOURCE_SPECULAR
out vec4 uiSpecularFactor;
#endif
void main() {
    if (uiSourceComposition != 0) {
        // Explicit native byte representation. Native GL sampling/interpolation
        // remains separately documented; no PS2 rasterizer/device is executed.
        vec4 vertexByte=clamp(uiColor*255.0,0.0,255.0);
        vec2 sampleUv=uiUv;
        // Q was separately rounded by the original packet/clipper. Do not
        // replace this division with semantic UV perspective interpolation.
        if(uiSourceRaster==2)sampleUv=uiSourceStq.xy/uiSourceStq.z;
        vec4 source=uiTextured!=0 ?
            floor(clamp(texture(uiImage,sampleUv)*255.0*vertexByte/128.0,0.0,255.0)) :
            floor(vertexByte);
        if (uiSourceAlphaTest!=0 && source.a<1.0) discard;
        if (uiSourceFogEnabled!=0) {
            float f=clamp(uiParticleFog*256.0,0.0,255.0);
            source.rgb=floor((source.rgb*f+uiParticleFogColor.rgb*255.0*(255.0-f))/256.0);
        }
        // Float scratch stores exact byte payload, including A=0. Negative A
        // is reserved for uncovered samples by the private raster clear.
        if(uiCoveragePayload!=0){viewerOutput=source;return;}
#ifdef RR_UI_DUAL_SOURCE_SPECULAR
        // Garage PC/Xbox fixed-function destination-alpha specular (ALPHA
        // A=0 B=2 C=1 D=2: Cs*Ad/128 + Cd). Every source triangle is issued
        // twice in a row; the GPU blends RGB as src*DST_ALPHA + dst. Copy one
        // adds Cs*Ad/255 and keeps A (alpha factors ONE, 1-SRC1_ALPHA with
        // src A=0, SRC1 A=0); copy two adds Cs*(127/128)*Ad/255 and replaces
        // A with source A (SRC1 A=1). Together: Cs*Ad/128 from the same Ad,
        // in API primitive order, with no framebuffer read.
        if (uiSourceBlend==-3) {
            bool second=(gl_PrimitiveID&1)!=0;
            viewerOutput=vec4(second ? source.rgb*(127.0/128.0) : source.rgb, second ? source.a : 0.0)/255.0;
            uiSpecularFactor=vec4(0.0,0.0,0.0,second ? 1.0 : 0.0);
            return;
        }
#endif
        vec3 result=source.rgb;
        if (uiSourceBlend<0) {
            // Fixed-function ADD form of ALPHA modes 2/4/6 (no destination
            // term inside the product): emit only floor(Cs*coefficient/128);
            // the blender adds Cd and clamps. Admitted only after this GPU's
            // exhaustive readback self-test proved min(255,term+Cd) for every
            // term 0..511 and Cd 0..255. Same coefficients as below.
            float coefficient=uiSourceBlend==-4 ? 255.0 : uiSourceBlend==-6 ? 64.0 : source.a;
            result=floor(source.rgb*coefficient/128.0);
        } else if (uiSourceBlend!=0) {
            // Opaque spans never enter this destination-fetch branch.
#ifdef RR_UI_COHERENT_FRAMEBUFFER_FETCH
            // Coherent EXT fetch observes prior writes in API primitive order,
            // including overlapping triangles within this indexed span draw.
            vec4 destination=floor(viewerOutput*255.0+0.5);
#else
            // ARB_texture_barrier: feedback uses one actual triangle per draw,
            // one same-pixel read/write per texel, with a barrier before each.
            vec4 destination=floor(texelFetch(uiSourceTarget,ivec2(gl_FragCoord.xy),0)*255.0+0.5);
#endif
            float coefficient=uiSourceBlend==3 ? destination.a : uiSourceBlend==4 ? 255.0 :
                uiSourceBlend==6 ? 64.0 : source.a;
            // Exact shadow ALPHA A=2 selects zero, B=1 selects destination.
            // Floor the signed product before adding Cd, as for other ALPHA
            // modes; never replace the original shadow texture or source A.
            vec3 difference=uiSourceBlend==5 ? -destination.rgb :
                uiSourceBlend==1 ? source.rgb-destination.rgb : source.rgb;
            result=floor(difference*coefficient/128.0)+destination.rgb;
        }
        // ALPHA changes RGB only. Every surviving fragment replaces stored A;
        // it never accumulates source-over alpha or clamps a FIX255 factor to1.
        viewerOutput=vec4(clamp(result,0.0,255.0),source.a)/255.0;
        return;
    }
    if (uiTextured != 0) {
        vec4 raw = texture(uiImage, uiUv) * uiColor;
        // MODULATE clamps before blend, not after addition to destination RGB.
        viewerOutput = vec4(clamp(raw.rgb * (255.0 / 128.0), 0.0, 1.0),
            clamp(raw.a * uiAlphaScale, 0.0, 1.0));
        // Actual DefaultMT ATE/GREATER/AREF0/KEEP. Source MODULATE alpha
        // is byte-quantized before the test: values below one byte fail.
        if (uiSourceAlphaTest != 0 && raw.a * (255.0 * 255.0 / 128.0) < 1.0) discard;
        if (uiSourceFogEnabled != 0) {
            // Source F is a byte, with both fog weights divided by256.
            // Fog changes RGB only; A/128 remains the blend coefficient.
            viewerOutput.rgb = viewerOutput.rgb * uiParticleFog +
                uiParticleFogColor.rgb * (255.0 / 256.0 - uiParticleFog);
        }
        if(gcParticleMode!=0) {
            //Source MODULATE A is byte-quantized/clamped before both ALPHA
            //and independent destination-A replacement. Never store A/128
            //into a framebuffer whose source byte representation is A/255.
            float sourceAlphaByte=floor(clamp(raw.a*(255.0*255.0/128.0),0.0,255.0));
            if(gcParticleMode==3) {
                viewerOutput=vec4(0.0,0.0,0.0,sourceAlphaByte/255.0);
            } else if(gcParticleMode==2) {
                //ONE/ONE permits genuine glow coefficients above1 without
                //SRC_ALPHA clamp. Positive fixed-target additive saturation
                //is unchanged if premultiplied source exceeds framebuffer1.
                viewerOutput.rgb*=sourceAlphaByte/128.0;
                viewerOutput.a=1.0;
            } else viewerOutput.a=sourceAlphaByte/128.0;
        }
    } else {
        // TME=0 uses raw interpolated vertex color without texture modulation.
        // Nonopaque A/128 uses one 255/128 scale; RGB retains its raw /255.
        viewerOutput = vec4(uiColor.rgb, clamp(uiColor.a * uiAlphaScale, 0.0, 1.0));
    }
}
)GLSL";
}
namespace {
// Host scheduling scratch for the source-RGBA destination-feedback fallback
// (no coherent framebuffer fetch). Each blended source triangle reads its own
// pixel's destination through a texture after a texture barrier. Two triangles
// whose conservative pixel regions do not intersect cannot observe or alter
// each other's pixels, so they may share one barrier+draw; every pair whose
// regions do intersect keeps its original relative order. The per-pixel
// fragment sequence (blend inputs, depth tests and writes) is therefore
// identical to the one-triangle-per-draw order. Regions are the existing
// conservative coverage bounds (one-pixel halo), widened to 4x4 tiles.
struct UiFeedbackBatches50KG {
    static constexpr int tile=4,tilesX=640/tile,tilesY=448/tile;
    std::vector<std::uint32_t> tileStamp,tileBatch;std::uint32_t stamp=0;
    std::vector<std::uint32_t> batchOf,offsets,scratch;
    std::vector<std::uint32_t> rangeFirst,rangeCount; // index ranges, all spans
    std::vector<std::uint32_t> spanFirst,spanCount;    // per span into ranges
};
thread_local UiFeedbackBatches50KG uiFeedbackBatches50KG;
// Exact fixed-function blending for the destination-free additive source
// ALPHA modes (2: Cs*As, 4: Cs*255, 6: Cs*64, each /128 floored, plus Cd).
// The shader then emits only the floored product term and GL blending
// (ONE,ONE / ONE,ZERO, FUNC_ADD) adds Cd on the RGBA8 target. Additions of
// nonnegative terms clamp identically in any grouping, and one indexed draw
// keeps API primitive order per pixel, so no destination fetch/barrier is
// needed. Admitted only when this process's exhaustive GPU readback proves
// stored = min(255,term+Cd) for every term 0..511 and Cd 0..255 on all three
// colour channels and stored A = source A (log-only result). Off with
// RR_GARAGE_LEGACY_BLEND=1. Never used for coverage (AA1) spans.
struct GarageFixedAdditive50KG {
    bool attempted=false,passed=false;
    static bool legacy()noexcept{static const bool on=[]{const char* v=std::getenv("RR_GARAGE_LEGACY_BLEND");return v&&v[0]=='1'&&!v[1];}();return on;}
    template<class S> void selfTest(S& s)noexcept{
        if(attempted)return;attempted=true;
        if(legacy()){std::fprintf(stderr,"garage_fixed_additive_selftest skipped=legacy_switch admitted=0\n");return;}
        const auto get=[&](const char* n)noexcept{return s.request.procAddress(s.request.context,n);};
        using FI=void(*)(U,I*);using FB=void(*)(U,std::uint8_t*);using FE=std::uint8_t(*)(U);using FU=void(*)(U);
        using F2=void(*)(U,U);using F4=void(*)(U,U,U,U);using FN=void(*)(I,U*);using FD=void(*)(I,const U*);
        auto getIntegerv=reinterpret_cast<FI>(get("glGetIntegerv"));auto getBooleanv=reinterpret_cast<FB>(get("glGetBooleanv"));
        auto isEnabled=reinterpret_cast<FE>(get("glIsEnabled"));auto enable=reinterpret_cast<FU>(get("glEnable"));
        auto disable=reinterpret_cast<FU>(get("glDisable"));auto genTextures=reinterpret_cast<FN>(get("glGenTextures"));
        auto deleteTextures=reinterpret_cast<FD>(get("glDeleteTextures"));auto bindTexture=reinterpret_cast<F2>(get("glBindTexture"));
        auto activeTexture=reinterpret_cast<FU>(get("glActiveTexture"));
        auto texImage2D=reinterpret_cast<void(*)(U,I,I,I,I,I,U,U,const void*)>(get("glTexImage2D"));
        auto texParameteri=reinterpret_cast<void(*)(U,U,I)>(get("glTexParameteri"));
        auto genFramebuffers=reinterpret_cast<FN>(get("glGenFramebuffers"));auto deleteFramebuffers=reinterpret_cast<FD>(get("glDeleteFramebuffers"));
        auto bindFramebuffer=reinterpret_cast<F2>(get("glBindFramebuffer"));
        auto framebufferTexture2D=reinterpret_cast<void(*)(U,U,U,U,I)>(get("glFramebufferTexture2D"));
        auto checkFramebufferStatus=reinterpret_cast<U(*)(U)>(get("glCheckFramebufferStatus"));
        auto drawBuffer=reinterpret_cast<FU>(get("glDrawBuffer"));auto readBuffer=reinterpret_cast<FU>(get("glReadBuffer"));
        auto viewport=reinterpret_cast<void(*)(I,I,I,I)>(get("glViewport"));auto useProgram=reinterpret_cast<FU>(get("glUseProgram"));
        auto genVertexArrays=reinterpret_cast<FN>(get("glGenVertexArrays"));auto deleteVertexArrays=reinterpret_cast<FD>(get("glDeleteVertexArrays"));
        auto bindVertexArray=reinterpret_cast<FU>(get("glBindVertexArray"));auto drawArrays=reinterpret_cast<void(*)(U,I,I)>(get("glDrawArrays"));
        auto blendFuncSeparate=reinterpret_cast<F4>(get("glBlendFuncSeparate"));auto blendEquationSeparate=reinterpret_cast<F2>(get("glBlendEquationSeparate"));
        auto colorMask=reinterpret_cast<void(*)(std::uint8_t,std::uint8_t,std::uint8_t,std::uint8_t)>(get("glColorMask"));
        auto pixelStorei=reinterpret_cast<void(*)(U,I)>(get("glPixelStorei"));
        auto readPixels=reinterpret_cast<void(*)(I,I,I,I,U,U,void*)>(get("glReadPixels"));auto bindBuffer=reinterpret_cast<F2>(get("glBindBuffer"));
        auto createShader=reinterpret_cast<U(*)(U)>(get("glCreateShader"));
        auto shaderSource=reinterpret_cast<void(*)(U,I,const char* const*,const I*)>(get("glShaderSource"));
        auto compileShader=reinterpret_cast<FU>(get("glCompileShader"));auto getShaderiv=reinterpret_cast<void(*)(U,U,I*)>(get("glGetShaderiv"));
        auto createProgram=reinterpret_cast<U(*)()>(get("glCreateProgram"));auto attachShader=reinterpret_cast<F2>(get("glAttachShader"));
        auto linkProgram=reinterpret_cast<FU>(get("glLinkProgram"));auto getProgramiv=reinterpret_cast<void(*)(U,U,I*)>(get("glGetProgramiv"));
        auto deleteShader=reinterpret_cast<FU>(get("glDeleteShader"));auto deleteProgram=reinterpret_cast<FU>(get("glDeleteProgram"));
        auto bindFragDataLocation=reinterpret_cast<void(*)(U,U,const char*)>(get("glBindFragDataLocation"));
        if(!getIntegerv||!getBooleanv||!isEnabled||!enable||!disable||!genTextures||!deleteTextures||!bindTexture||!activeTexture||
           !texImage2D||!texParameteri||!genFramebuffers||!deleteFramebuffers||!bindFramebuffer||!framebufferTexture2D||
           !checkFramebufferStatus||!drawBuffer||!readBuffer||!viewport||!useProgram||!genVertexArrays||!deleteVertexArrays||
           !bindVertexArray||!drawArrays||!blendFuncSeparate||!blendEquationSeparate||!colorMask||!pixelStorei||!readPixels||
           !bindBuffer||!createShader||!shaderSource||!compileShader||!getShaderiv||!createProgram||!attachShader||!linkProgram||
           !getProgramiv||!deleteShader||!deleteProgram||!bindFragDataLocation){
            std::fprintf(stderr,"garage_fixed_additive_selftest entry_points=missing admitted=0\n");return;}
        constexpr int W=256,H=512;
        // Saved state (everything this test touches).
        I drawFb=0,readFb=0,vp[4]{},program=0,vao=0,active=0,tex0=0,unpackBuf=0,packBuf=0,unpackAlign=4,packAlign=4;
        I srcRgb=1,dstRgb=0,srcA=1,dstA=0,eqRgb=0x8006,eqA=0x8006;std::uint8_t mask[4]{1,1,1,1};
        getIntegerv(0x8CA6,&drawFb);getIntegerv(0x8CAA,&readFb);getIntegerv(0x0BA2,vp);getIntegerv(0x8B8D,&program);
        getIntegerv(0x85B5,&vao);getIntegerv(0x84E0,&active);getIntegerv(0x88EF,&unpackBuf);getIntegerv(0x88ED,&packBuf);
        getIntegerv(0x0CF5,&unpackAlign);getIntegerv(0x0D05,&packAlign);getIntegerv(0x80C9,&srcRgb);getIntegerv(0x80C8,&dstRgb);
        getIntegerv(0x80CB,&srcA);getIntegerv(0x80CA,&dstA);getIntegerv(0x8009,&eqRgb);getIntegerv(0x883D,&eqA);getBooleanv(0x0C23,mask);
        activeTexture(0x84C0);getIntegerv(0x8069,&tex0);
        const U caps[6]{0x0BE2,0x0B71,0x0C11,0x0B90,0x0B44,0x8C89};std::uint8_t capOn[6]{};
        for(int c=0;c<6;++c)capOn[c]=isEnabled(caps[c]);
        std::vector<std::uint8_t> init(std::size_t(W)*H*4),out(std::size_t(W)*H*4);
        for(int y=0;y<H;++y)for(int x=0;x<W;++x){auto* p=&init[(std::size_t(y)*W+x)*4];
            p[0]=std::uint8_t(x);p[1]=std::uint8_t(255-x);p[2]=std::uint8_t((x*37+11)&255);p[3]=std::uint8_t((x*5+y)&255);}
        static constexpr const char* vs="#version 150 core\nvoid main(){vec2 p=vec2(float((gl_VertexID<<1)&2),float(gl_VertexID&2));gl_Position=vec4(p*2.0-1.0,0.0,1.0);}\n";
        // Same output expression as the UI shader: vec4(clamp(result,0,255),source.a)/255.
        static constexpr const char* fs="#version 150 core\nout vec4 viewerOutput;\nvoid main(){ivec2 p=ivec2(gl_FragCoord.xy);"
            "vec3 result=vec3(float(p.y));vec4 source=vec4(0.0,0.0,0.0,float((p.x*7+p.y*3)&255));"
            "viewerOutput=vec4(clamp(result,0.0,255.0),source.a)/255.0;}\n";
        U texture=0,fbo=0,array=0,v=0,f=0,prog=0;bool okay=true;
        genTextures(1,&texture);bindTexture(0x0DE1,texture);
        texParameteri(0x0DE1,0x2801,0x2600);texParameteri(0x0DE1,0x2800,0x2600);
        texParameteri(0x0DE1,0x813C,0);texParameteri(0x0DE1,0x813D,0);
        bindBuffer(0x88EC,0);bindBuffer(0x88EB,0);pixelStorei(0x0CF5,1);pixelStorei(0x0D05,1);
        texImage2D(0x0DE1,0,0x8058,W,H,0,0x1908,0x1401,init.data());
        genFramebuffers(1,&fbo);bindFramebuffer(0x8D40,fbo);framebufferTexture2D(0x8D40,0x8CE0,0x0DE1,texture,0);
        drawBuffer(0x8CE0);readBuffer(0x8CE0);
        okay=checkFramebufferStatus(0x8D40)==0x8CD5;
        v=createShader(0x8B31);f=createShader(0x8B30);shaderSource(v,1,&vs,nullptr);shaderSource(f,1,&fs,nullptr);
        compileShader(v);compileShader(f);I cv=0,cf=0,linked=0;getShaderiv(v,0x8B81,&cv);getShaderiv(f,0x8B81,&cf);
        prog=createProgram();attachShader(prog,v);attachShader(prog,f);bindFragDataLocation(prog,0,"viewerOutput");
        linkProgram(prog);getProgramiv(prog,0x8B82,&linked);okay=okay&&cv&&cf&&linked;
        std::size_t mismatches=0,checked=0;
        if(okay){
            genVertexArrays(1,&array);bindVertexArray(array);useProgram(prog);viewport(0,0,W,H);
            for(int c=1;c<6;++c)disable(caps[c]);colorMask(1,1,1,1);
            enable(0x0BE2);blendEquationSeparate(0x8006,0x8006);blendFuncSeparate(1,1,1,0);
            drawArrays(0x0004,0,3);
            readPixels(0,0,W,H,0x1908,0x1401,out.data());
            for(int y=0;y<H;++y)for(int x=0;x<W;++x){const auto* p=&out[(std::size_t(y)*W+x)*4];const auto* q=&init[(std::size_t(y)*W+x)*4];
                for(int ch=0;ch<3;++ch){++checked;if(p[ch]!=std::min(255,y+int(q[ch])))++mismatches;}
                ++checked;if(p[3]!=std::uint8_t((x*7+y*3)&255))++mismatches;}
        }
        // Restore every saved binding and capability.
        bindFramebuffer(0x8CA9,U(drawFb));bindFramebuffer(0x8CA8,U(readFb));viewport(vp[0],vp[1],vp[2],vp[3]);
        useProgram(U(program));bindVertexArray(U(vao));activeTexture(0x84C0);bindTexture(0x0DE1,U(tex0));activeTexture(U(active));
        bindBuffer(0x88EC,U(unpackBuf));bindBuffer(0x88EB,U(packBuf));pixelStorei(0x0CF5,unpackAlign);pixelStorei(0x0D05,packAlign);
        blendFuncSeparate(U(srcRgb),U(dstRgb),U(srcA),U(dstA));blendEquationSeparate(U(eqRgb),U(eqA));
        colorMask(mask[0],mask[1],mask[2],mask[3]);
        for(int c=0;c<6;++c){if(capOn[c])enable(caps[c]);else disable(caps[c]);}
        if(array)deleteVertexArrays(1,&array);if(prog)deleteProgram(prog);if(v)deleteShader(v);if(f)deleteShader(f);
        if(fbo)deleteFramebuffers(1,&fbo);if(texture)deleteTextures(1,&texture);
        passed=okay&&checked==std::size_t(W)*H*4&&mismatches==0;
        std::fprintf(stderr,"garage_fixed_additive_selftest terms=0..%d destinations=0..%d channels=4 compared=%zu mismatches=%zu setup=%u admitted=%u\n",
            H-1,W-1,checked,mismatches,unsigned(okay),unsigned(passed));
    }
};
thread_local GarageFixedAdditive50KG garageFixedAdditive50KG;
bool garageFixedAdditiveBlend50KG(OriginalUiBlend50KG b)noexcept{
    return b==OriginalUiBlend50KG::additive||b==OriginalUiBlend50KG::fixed255Additive||b==OriginalUiBlend50KG::fixed64Additive;
}
// PC/Xbox fixed-function Garage destination-alpha specular (user-approved
// PC-native blending, the Garage counterpart of the race "fixed-function
// map-2 blend"). Default on; RR_EXACT_PS2_SPECULAR=1 (the race switch) or
// RR_GARAGE_LEGACY_BLEND=1 keeps the byte-exact per-triangle feedback path.
// Exact: min(255, Cd + floor(Cs*Ad/128)), A = As. Fixed: each triangle is
// drawn twice (see RR_UI_DUAL_SOURCE_SPECULAR in the UI fragment program);
// GPU blending computes Cd + Cs*Ad/255 + Cs*(127/128)*Ad/255 with rounding,
// so RGB may differ from exact by the readback-measured maximum (expected 1)
// and stored A is unchanged. Admitted only if this process's readback over
// every Cs 0..255 x Ad 0..255 (12 Cd values each) shows A exact and RGB
// within 2. Never used for coverage (AA1) spans.
struct GarageFixedSpecular50KG {
    bool attempted=false,passed=false;int maxDifference=-1;
    std::uint64_t draws=0,triangles=0;
    U program=0,builtFor=0;const void* builtStorage=nullptr;bool buildFailed=false;
    I canvas=-1,image=-1,alpha=-1,textured=-1,fogColor=-1,raster=-1,particle=-1,fogEnabled=-1,alphaTest=-1,
        composition=-1,target=-1,blend=-1,payload=-1;
    static bool exact()noexcept{static const bool on=[]{const char* v=std::getenv("RR_EXACT_PS2_SPECULAR");return v&&v[0]=='1'&&!v[1];}();return on;}
    template<class S> bool ready(const S& s)const noexcept{
        return passed&&program&&builtStorage==&s&&builtFor==s.uiProgram&&s.uiProgram;
    }
    template<class S> void prepare(S& s)noexcept{
        selfTest(s);
        if(!passed||!s.uiProgram||!s.uiVertexShader||s.uiSourceCoherentFramebufferFetch)return;
        if(builtStorage==&s&&builtFor==s.uiProgram&&(program||buildFailed))return;
        // A different Storage/UI program: the previous variant (if any) belongs
        // to that owner's lifetime and is never deleted from here.
        program=0;buildFailed=true;builtStorage=&s;builtFor=s.uiProgram;
        const auto get=[&](const char* n)noexcept{return s.request.procAddress(s.request.context,n);};
        auto bindIndexed=reinterpret_cast<void(*)(U,U,U,const char*)>(get("glBindFragDataLocationIndexed"));
        if(!bindIndexed){std::fprintf(stderr,"garage_fixed_specular_variant entry_point=missing ready=0\n");return;}
        const char* sources[]={"#version 150 core\n#define RR_UI_DUAL_SOURCE_SPECULAR 1\n",originalUiFragmentSource50KG};
        const U fragment=s.shaderGl.createShader(0x8B30);if(!fragment)return;
        s.shaderGl.shaderSource(fragment,2,sources,nullptr);s.shaderGl.compileShader(fragment);
        I compiled=0;s.shaderGl.getShaderiv(fragment,0x8B81,&compiled);
        if(!compiled){char log[512]{};s.shaderGl.getShaderInfoLog(fragment,sizeof(log),nullptr,log);
            std::fprintf(stderr,"garage_fixed_specular_variant compile=0 log=%.400s ready=0\n",log);s.shaderGl.deleteShader(fragment);return;}
        const U p=s.shaderGl.createProgram();if(!p){s.shaderGl.deleteShader(fragment);return;}
        s.shaderGl.attachShader(p,s.uiVertexShader);s.shaderGl.attachShader(p,fragment);
        const char* attributes[]={"rrPosition","rrUv","rrColor","rrParticleFog","rrSourceRasterW","rrSourceStq"};
        for(U a=0;a<6;++a)s.shaderGl.bindAttribLocation(p,a,attributes[a]);
        bindIndexed(p,vehicleGlColorOutput50CZ,0,"viewerOutput");bindIndexed(p,vehicleGlColorOutput50CZ,1,"uiSpecularFactor");
        s.shaderGl.linkProgram(p);I linked=0;s.shaderGl.getProgramiv(p,0x8B82,&linked);
        s.shaderGl.deleteShader(fragment); // stays alive while attached
        if(!linked){char log[512]{};s.shaderGl.getProgramInfoLog(p,sizeof(log),nullptr,log);
            std::fprintf(stderr,"garage_fixed_specular_variant link=0 log=%.400s ready=0\n",log);s.shaderGl.deleteProgram(p);return;}
        const auto at=[&](const char* n)noexcept{return s.shaderGl.getUniformLocation(p,n);};
        canvas=at("canvasToClip");image=at("uiImage");alpha=at("uiAlphaScale");textured=at("uiTextured");
        fogColor=at("uiParticleFogColor");raster=at("uiSourceRaster");particle=at("gcParticleMode");
        fogEnabled=at("uiSourceFogEnabled");alphaTest=at("uiSourceAlphaTest");composition=at("uiSourceComposition");
        target=at("uiSourceTarget");blend=at("uiSourceBlend");payload=at("uiCoveragePayload");
        const bool located=canvas>=0&&image>=0&&textured>=0&&fogColor>=0&&raster>=0&&fogEnabled>=0&&alphaTest>=0&&
            composition>=0&&blend>=0&&payload>=0;
        const U error=s.drawGl.getError();
        if(!located||error){std::fprintf(stderr,"garage_fixed_specular_variant uniforms=%u gl_error=%u ready=0\n",unsigned(located),error);
            s.shaderGl.deleteProgram(p);return;}
        program=p;buildFailed=false;
        std::fprintf(stderr,"garage_fixed_specular_variant program=%u ready=1\n",p);
    }
    template<class S> void selfTest(S& s)noexcept{
        if(attempted)return;attempted=true;
        if(exact()||GarageFixedAdditive50KG::legacy()){
            std::fprintf(stderr,"garage_fixed_specular_selftest skipped=exact_switch admitted=0\n");return;}
        const auto get=[&](const char* n)noexcept{return s.request.procAddress(s.request.context,n);};
        using FI=void(*)(U,I*);using FB=void(*)(U,std::uint8_t*);using FE=std::uint8_t(*)(U);using FU=void(*)(U);
        using F2=void(*)(U,U);using F4=void(*)(U,U,U,U);using FN=void(*)(I,U*);using FD=void(*)(I,const U*);
        auto getIntegerv=reinterpret_cast<FI>(get("glGetIntegerv"));auto getBooleanv=reinterpret_cast<FB>(get("glGetBooleanv"));
        auto isEnabled=reinterpret_cast<FE>(get("glIsEnabled"));auto enable=reinterpret_cast<FU>(get("glEnable"));
        auto disable=reinterpret_cast<FU>(get("glDisable"));auto genTextures=reinterpret_cast<FN>(get("glGenTextures"));
        auto deleteTextures=reinterpret_cast<FD>(get("glDeleteTextures"));auto bindTexture=reinterpret_cast<F2>(get("glBindTexture"));
        auto activeTexture=reinterpret_cast<FU>(get("glActiveTexture"));
        auto texImage2D=reinterpret_cast<void(*)(U,I,I,I,I,I,U,U,const void*)>(get("glTexImage2D"));
        auto texParameteri=reinterpret_cast<void(*)(U,U,I)>(get("glTexParameteri"));
        auto genFramebuffers=reinterpret_cast<FN>(get("glGenFramebuffers"));auto deleteFramebuffers=reinterpret_cast<FD>(get("glDeleteFramebuffers"));
        auto bindFramebuffer=reinterpret_cast<F2>(get("glBindFramebuffer"));
        auto framebufferTexture2D=reinterpret_cast<void(*)(U,U,U,U,I)>(get("glFramebufferTexture2D"));
        auto checkFramebufferStatus=reinterpret_cast<U(*)(U)>(get("glCheckFramebufferStatus"));
        auto drawBuffer=reinterpret_cast<FU>(get("glDrawBuffer"));auto readBuffer=reinterpret_cast<FU>(get("glReadBuffer"));
        auto viewport=reinterpret_cast<void(*)(I,I,I,I)>(get("glViewport"));auto useProgram=reinterpret_cast<FU>(get("glUseProgram"));
        auto genVertexArrays=reinterpret_cast<FN>(get("glGenVertexArrays"));auto deleteVertexArrays=reinterpret_cast<FD>(get("glDeleteVertexArrays"));
        auto bindVertexArray=reinterpret_cast<FU>(get("glBindVertexArray"));auto drawArrays=reinterpret_cast<void(*)(U,I,I)>(get("glDrawArrays"));
        auto blendFuncSeparate=reinterpret_cast<F4>(get("glBlendFuncSeparate"));auto blendEquationSeparate=reinterpret_cast<F2>(get("glBlendEquationSeparate"));
        auto colorMask=reinterpret_cast<void(*)(std::uint8_t,std::uint8_t,std::uint8_t,std::uint8_t)>(get("glColorMask"));
        auto pixelStorei=reinterpret_cast<void(*)(U,I)>(get("glPixelStorei"));
        auto readPixels=reinterpret_cast<void(*)(I,I,I,I,U,U,void*)>(get("glReadPixels"));auto bindBuffer=reinterpret_cast<F2>(get("glBindBuffer"));
        auto createShader=reinterpret_cast<U(*)(U)>(get("glCreateShader"));
        auto shaderSource=reinterpret_cast<void(*)(U,I,const char* const*,const I*)>(get("glShaderSource"));
        auto compileShader=reinterpret_cast<FU>(get("glCompileShader"));auto getShaderiv=reinterpret_cast<void(*)(U,U,I*)>(get("glGetShaderiv"));
        auto createProgram=reinterpret_cast<U(*)()>(get("glCreateProgram"));auto attachShader=reinterpret_cast<F2>(get("glAttachShader"));
        auto linkProgram=reinterpret_cast<FU>(get("glLinkProgram"));auto getProgramiv=reinterpret_cast<void(*)(U,U,I*)>(get("glGetProgramiv"));
        auto deleteShader=reinterpret_cast<FU>(get("glDeleteShader"));auto deleteProgram=reinterpret_cast<FU>(get("glDeleteProgram"));
        auto bindIndexed=reinterpret_cast<void(*)(U,U,U,const char*)>(get("glBindFragDataLocationIndexed"));
        if(!getIntegerv||!getBooleanv||!isEnabled||!enable||!disable||!genTextures||!deleteTextures||!bindTexture||!activeTexture||
           !texImage2D||!texParameteri||!genFramebuffers||!deleteFramebuffers||!bindFramebuffer||!framebufferTexture2D||
           !checkFramebufferStatus||!drawBuffer||!readBuffer||!viewport||!useProgram||!genVertexArrays||!deleteVertexArrays||
           !bindVertexArray||!drawArrays||!blendFuncSeparate||!blendEquationSeparate||!colorMask||!pixelStorei||!readPixels||
           !bindBuffer||!createShader||!shaderSource||!compileShader||!getShaderiv||!createProgram||!attachShader||!linkProgram||
           !getProgramiv||!deleteShader||!deleteProgram||!bindIndexed){
            std::fprintf(stderr,"garage_fixed_specular_selftest entry_points=missing admitted=0\n");return;}
        // x = Cs (R: x, G: 255-x, B: odd permutation of x), y&255 = Ad, band
        // y>>8 selects one of four Cd patterns; As is a separate pattern.
        constexpr int W=256,H=1024;
        I drawFb=0,readFb=0,vp[4]{},program0=0,vao=0,active=0,tex0=0,unpackBuf=0,packBuf=0,unpackAlign=4,packAlign=4;
        I srcRgb=1,dstRgb=0,srcA=1,dstA=0,eqRgb=0x8006,eqA=0x8006;std::uint8_t mask[4]{1,1,1,1};
        getIntegerv(0x8CA6,&drawFb);getIntegerv(0x8CAA,&readFb);getIntegerv(0x0BA2,vp);getIntegerv(0x8B8D,&program0);
        getIntegerv(0x85B5,&vao);getIntegerv(0x84E0,&active);getIntegerv(0x88EF,&unpackBuf);getIntegerv(0x88ED,&packBuf);
        getIntegerv(0x0CF5,&unpackAlign);getIntegerv(0x0D05,&packAlign);getIntegerv(0x80C9,&srcRgb);getIntegerv(0x80C8,&dstRgb);
        getIntegerv(0x80CB,&srcA);getIntegerv(0x80CA,&dstA);getIntegerv(0x8009,&eqRgb);getIntegerv(0x883D,&eqA);getBooleanv(0x0C23,mask);
        activeTexture(0x84C0);getIntegerv(0x8069,&tex0);
        const U caps[6]{0x0BE2,0x0B71,0x0C11,0x0B90,0x0B44,0x8C89};std::uint8_t capOn[6]{};
        for(int c=0;c<6;++c)capOn[c]=isEnabled(caps[c]);
        const auto cdOf=[](int x,int y,int ch)noexcept{const int band=y>>8;
            return (x*(3+2*ch)+(y&255)*(5+4*band)+band*61+ch*29)&255;};
        std::vector<std::uint8_t> init(std::size_t(W)*H*4),out(std::size_t(W)*H*4);
        for(int y=0;y<H;++y)for(int x=0;x<W;++x){auto* p=&init[(std::size_t(y)*W+x)*4];
            for(int ch=0;ch<3;++ch)p[ch]=std::uint8_t(cdOf(x,y,ch));p[3]=std::uint8_t(y&255);}
        static constexpr const char* vs="#version 150 core\nvoid main(){int k=gl_VertexID%3;vec2 p=vec2(float((k<<1)&2),float(k&2));gl_Position=vec4(p*2.0-1.0,0.0,1.0);}\n";
        // Same output expressions as the RR_UI_DUAL_SOURCE_SPECULAR branch.
        static constexpr const char* fs="#version 150 core\nout vec4 viewerOutput;\nout vec4 uiSpecularFactor;\nvoid main(){ivec2 p=ivec2(gl_FragCoord.xy);"
            "vec4 source=vec4(float(p.x),float(255-p.x),float((p.x*37+11)&255),float((p.x*7+p.y*3)&255));"
            "bool second=(gl_PrimitiveID&1)!=0;"
            "viewerOutput=vec4(second ? source.rgb*(127.0/128.0) : source.rgb, second ? source.a : 0.0)/255.0;"
            "uiSpecularFactor=vec4(0.0,0.0,0.0,second ? 1.0 : 0.0);}\n";
        U texture=0,fbo=0,array=0,v=0,f=0,prog=0;bool okay=true;
        genTextures(1,&texture);bindTexture(0x0DE1,texture);
        texParameteri(0x0DE1,0x2801,0x2600);texParameteri(0x0DE1,0x2800,0x2600);
        texParameteri(0x0DE1,0x813C,0);texParameteri(0x0DE1,0x813D,0);
        bindBuffer(0x88EC,0);bindBuffer(0x88EB,0);pixelStorei(0x0CF5,1);pixelStorei(0x0D05,1);
        texImage2D(0x0DE1,0,0x8058,W,H,0,0x1908,0x1401,init.data());
        genFramebuffers(1,&fbo);bindFramebuffer(0x8D40,fbo);framebufferTexture2D(0x8D40,0x8CE0,0x0DE1,texture,0);
        drawBuffer(0x8CE0);readBuffer(0x8CE0);
        okay=checkFramebufferStatus(0x8D40)==0x8CD5;
        v=createShader(0x8B31);f=createShader(0x8B30);shaderSource(v,1,&vs,nullptr);shaderSource(f,1,&fs,nullptr);
        compileShader(v);compileShader(f);I cv=0,cf=0,linked=0;getShaderiv(v,0x8B81,&cv);getShaderiv(f,0x8B81,&cf);
        prog=createProgram();attachShader(prog,v);attachShader(prog,f);
        bindIndexed(prog,0,0,"viewerOutput");bindIndexed(prog,0,1,"uiSpecularFactor");
        linkProgram(prog);getProgramiv(prog,0x8B82,&linked);okay=okay&&cv&&cf&&linked;
        std::size_t checked=0,differing=0,alphaMismatch=0,plus=0,minus=0;int worst=0;
        if(okay){
            genVertexArrays(1,&array);bindVertexArray(array);useProgram(prog);viewport(0,0,W,H);
            for(int c=1;c<6;++c)disable(caps[c]);colorMask(1,1,1,1);
            enable(0x0BE2);blendEquationSeparate(0x8006,0x8006);
            blendFuncSeparate(0x0304,1,1,0x88FB); // RGB: DST_ALPHA, ONE; A: ONE, ONE_MINUS_SRC1_ALPHA
            drawArrays(0x0004,0,6);
            readPixels(0,0,W,H,0x1908,0x1401,out.data());
            for(int y=0;y<H;++y)for(int x=0;x<W;++x){const auto* p=&out[(std::size_t(y)*W+x)*4];
                const int ad=y&255;const int cs[3]{x,255-x,(x*37+11)&255};
                for(int ch=0;ch<3;++ch){++checked;
                    const int expected=std::min(255,cdOf(x,y,ch)+cs[ch]*ad/128);const int diff=int(p[ch])-expected;
                    if(diff){++differing;if(diff>0)++plus;else ++minus;}
                    worst=std::max(worst,diff<0?-diff:diff);}
                ++checked;if(p[3]!=std::uint8_t((x*7+y*3)&255))++alphaMismatch;}
        }
        bindFramebuffer(0x8CA9,U(drawFb));bindFramebuffer(0x8CA8,U(readFb));viewport(vp[0],vp[1],vp[2],vp[3]);
        useProgram(U(program0));bindVertexArray(U(vao));activeTexture(0x84C0);bindTexture(0x0DE1,U(tex0));activeTexture(U(active));
        bindBuffer(0x88EC,U(unpackBuf));bindBuffer(0x88EB,U(packBuf));pixelStorei(0x0CF5,unpackAlign);pixelStorei(0x0D05,packAlign);
        blendFuncSeparate(U(srcRgb),U(dstRgb),U(srcA),U(dstA));blendEquationSeparate(U(eqRgb),U(eqA));
        colorMask(mask[0],mask[1],mask[2],mask[3]);
        for(int c=0;c<6;++c){if(capOn[c])enable(caps[c]);else disable(caps[c]);}
        if(array)deleteVertexArrays(1,&array);if(prog)deleteProgram(prog);if(v)deleteShader(v);if(f)deleteShader(f);
        if(fbo)deleteFramebuffers(1,&fbo);if(texture)deleteTextures(1,&texture);
        maxDifference=okay?worst:-1;
        passed=okay&&checked==std::size_t(W)*H*4&&alphaMismatch==0&&worst<=2;
        std::fprintf(stderr,"garage_fixed_specular_selftest cs=0..255 ad=0..255 cd_patterns=12 compared=%zu rgb_differing=%zu above=%zu below=%zu max_rgb_difference=%d alpha_mismatches=%zu setup=%u admitted=%u\n",
            checked,differing,plus,minus,worst,alphaMismatch,unsigned(okay),unsigned(passed));
    }
};
thread_local GarageFixedSpecular50KG garageFixedSpecular50KG;
// Hot per-vertex validation helpers (same results as std::isfinite and
// std::floor(x)==x; the latter only for 0 <= x < 2^31, where truncation is floor).
inline bool uiFinite50KG(float v)noexcept{std::uint32_t b;std::memcpy(&b,&v,sizeof b);return (b&0x7f800000u)!=0x7f800000u;}
inline bool uiNonnegativeIntegral50KG(float v)noexcept{return float(std::int32_t(v))==v;}
}
void VehicleWindowGl50DS::Storage::clearMenuRasterLease50KG()noexcept {
        menuRasterShared50KG=nullptr;menuRasterOwner50KG=nullptr;menuRasterInputs50KG={};
        menuRasterEpoch50KG=menuRasterInvocation50KG=menuRasterDisplay50KG=0;
        menuRasterTextureEpoch50KG=menuRasterTargetGeneration50KG=menuRasterFrame50KG=0;
        menuRasterTarget50KG=menuRasterColor50KG=menuRasterDepth50KG=0;menuRasterNative50KG=nullptr;
        menuRasterApplied50KG=false;menuRasterFailed50KG=false;menuRasterBackendActive50KG=false;
        menuRasterSourceAccepted50KG=false;
    }

bool VehicleWindowGl50DS::Storage::validateOriginalUiBank(const VehicleWindowRequest50DS& input,std::uint8_t* stagedAlpha) noexcept {
        if (!input.enableOriginalUi)
            return (!input.originalUiCpuTextures && !input.originalUiTextures) ||
                fail(*diagnostic, VehicleWindowError50DS::request, "UI banks require explicit original UI opt-in");
        const auto* cpu = input.originalUiCpuTextures;
        const auto* gpu = input.originalUiTextures;
        // No bank is needed by an explicitly untextured-only UI. A missing
        // textured resource never selects this path: each span is typed.
        if (!cpu && !gpu) return true;
        if (!cpu || !cpu->loaded() || !gpu || gpu->riderSparse() || !gpu->validBorrow(input.context, input.isCurrent,
            gpu->publicationToken()) || !cpu->textureCount() ||
            cpu->textureCount() > assets::maxMaterialTextureAssets50EA || cpu->textureCount() != gpu->textureCount())
            return fail(*diagnostic, VehicleWindowError50DS::material, "UI requires matching immutable CPU/GPU publication");
        // Cold publication scan. Stage maxima so a rejected destination cannot
        // corrupt the currently retained bank's blend-admission bounds.
        std::uint8_t maximumAlphaByTexture[assets::maxMaterialTextureAssets50EA]{};
        for (U i = 0; i < cpu->textureCount(); ++i) {
            const auto* source = cpu->texture(i); const auto* image = gpu->asset(i);
            if (!source || !image || !image->name || image->identity != source->identity)
                return fail(*diagnostic, VehicleWindowError50DS::material, "UI CPU/GPU texture identity mismatch");
            std::uint8_t maximumAlpha = 0;
            for (std::size_t a = 3; a < source->texture.rgba.size(); a += 4)
                maximumAlpha = std::max(maximumAlpha, source->texture.rgba[a]);
            maximumAlphaByTexture[i] = maximumAlpha;
        }
        std::memcpy(stagedAlpha?stagedAlpha:uiTextureMaximumAlpha, maximumAlphaByTexture, sizeof(uiTextureMaximumAlpha));
        return true;
    }

bool VehicleWindowGl50DS::Storage::createOriginalUi() noexcept {
        if (!request.enableOriginalUi && !request.gcParticleTextures && !request.track.rcParticleWindow && !request.track.gbParticleWindow && !request.track.mcParticleWindow && !raceHudPreparing && !timeStatsPreparing && !positionStatsPreparing && !damagePreparing && !pauseDerivedPreparing && !speedBoostPreparing && !nativeF8BarPreparing && !nativeResultsDrawPreparing) return true;
        constexpr const char* vertexSource = R"GLSL(#version 150 core
in vec3 rrPosition;
in vec2 rrUv;
in vec4 rrColor;
in float rrParticleFog;
in float rrSourceRasterW;
in vec3 rrSourceStq;
uniform vec4 canvasToClip;
uniform int uiSourceRaster;
uniform int gcParticleMode;
out vec2 uiUv;
noperspective out vec3 uiSourceStq;
noperspective out vec4 uiColor;
noperspective out float uiParticleFog;
void main() {
    float w = uiSourceRaster != 0 ? rrSourceRasterW : 1.0;
    // Source screen Z is reversed. Gameplay uses core49F depth[0,1],
    // shared with the already drawn opaque world, not the UI reversed target.
    float z=gcParticleMode!=0 ? (8388591.0-rrPosition.z)/8388590.0 : rrPosition.z/16777215.0;
    gl_Position = vec4(rrPosition.xy * canvasToClip.xy + canvasToClip.zw,
        z * 2.0 - 1.0, 1.0) * w;
    uiUv = rrUv; uiColor = rrColor; uiParticleFog = rrParticleFog;
    // Modes0/1 retain the existing semantic-UV path. Mode2 transports the
    // actual packet lanes screen-affinely, independently of projected W.
    uiSourceStq = uiSourceRaster == 2 ? rrSourceStq : vec3(0.0,0.0,1.0);
}
)GLSL";
        constexpr const char* fragmentPrefix = "#version 150 core\n";
        constexpr const char* coherentFragmentPrefix =
            "#version 150 core\n"
            "#extension GL_EXT_shader_framebuffer_fetch : require\n"
            "#define RR_UI_COHERENT_FRAMEBUFFER_FETCH 1\n";
        const char* const fragmentSource = originalUiFragmentSource50KG;
        const auto compile = [&](U type, const char* source, U& name, const char* prefix=nullptr) noexcept {
            name = shaderGl.createShader(type);
            if (!check("create original UI shader")) return false;
            if (!name) return fail(*diagnostic, VehicleWindowError50DS::shader, "zero original UI shader");
            const char* sources[] = {prefix, source};
            shaderGl.shaderSource(name, prefix ? 2 : 1, prefix ? sources : &source, nullptr);
            shaderGl.compileShader(name);
            I compiled = 0; shaderGl.getShaderiv(name, 0x8B81, &compiled);
            if (!check("compile original UI shader")) return false;
            if (compiled) return true;
            shaderGl.getShaderInfoLog(name, static_cast<I>(sizeof(diagnostic->shader.log)), nullptr, diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1] = '\0';
            check("read original UI shader log");
            return fail(*diagnostic, VehicleWindowError50DS::shader, "original UI shader compilation failed");
        };
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::vertex;
        if (!compile(0x8B31, vertexSource, uiVertexShader)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::fragment;
        if (!compile(0x8B30, fragmentSource, uiFragmentShader,
            uiSourceCoherentFramebufferFetch ? coherentFragmentPrefix : fragmentPrefix)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::link;
        uiProgram = shaderGl.createProgram();
        if (!check("create original UI program")) return false;
        if (!uiProgram) return fail(*diagnostic, VehicleWindowError50DS::shader, "zero original UI program");
        shaderGl.attachShader(uiProgram, uiVertexShader); shaderGl.attachShader(uiProgram, uiFragmentShader);
        shaderGl.bindAttribLocation(uiProgram, 0, "rrPosition");
        shaderGl.bindAttribLocation(uiProgram, 1, "rrUv");
        shaderGl.bindAttribLocation(uiProgram, 2, "rrColor");
        shaderGl.bindAttribLocation(uiProgram, 3, "rrParticleFog");
        shaderGl.bindAttribLocation(uiProgram, 4, "rrSourceRasterW");
        shaderGl.bindAttribLocation(uiProgram, 5, "rrSourceStq");
        shaderGl.bindFragDataLocation(uiProgram, vehicleGlColorOutput50CZ, "viewerOutput");
        shaderGl.linkProgram(uiProgram);
        I linked = 0; shaderGl.getProgramiv(uiProgram, 0x8B82, &linked);
        if (!check("link original UI program")) return false;
        if (!linked) {
            shaderGl.getProgramInfoLog(uiProgram, static_cast<I>(sizeof(diagnostic->shader.log)), nullptr, diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1] = '\0';
            check("read original UI program log");
            return fail(*diagnostic, VehicleWindowError50DS::shader, "original UI program link failed");
        }
        uiCanvasUniform = shaderGl.getUniformLocation(uiProgram, "canvasToClip");
        uiImageUniform = shaderGl.getUniformLocation(uiProgram, "uiImage");
        uiAlphaUniform = shaderGl.getUniformLocation(uiProgram, "uiAlphaScale");
        uiTexturedUniform = shaderGl.getUniformLocation(uiProgram, "uiTextured");
        uiParticleFogColorUniform = shaderGl.getUniformLocation(uiProgram, "uiParticleFogColor");
        uiSourceRasterUniform = shaderGl.getUniformLocation(uiProgram, "uiSourceRaster");
        gcParticleModeUniform = shaderGl.getUniformLocation(uiProgram, "gcParticleMode");
        uiSourceFogEnabledUniform = shaderGl.getUniformLocation(uiProgram, "uiSourceFogEnabled");
        uiSourceAlphaTestUniform = shaderGl.getUniformLocation(uiProgram, "uiSourceAlphaTest");
        uiSourceCompositionUniform=shaderGl.getUniformLocation(uiProgram,"uiSourceComposition");
        if (!uiSourceCoherentFramebufferFetch)
            uiSourceTargetUniform=shaderGl.getUniformLocation(uiProgram,"uiSourceTarget");
        uiSourceBlendUniform=shaderGl.getUniformLocation(uiProgram,"uiSourceBlend");
        uiCoveragePayloadUniform=shaderGl.getUniformLocation(uiProgram,"uiCoveragePayload");
        if (!check("locate original UI uniforms")) return false;
        if (uiCanvasUniform < 0 || uiImageUniform < 0 || uiAlphaUniform < 0 || uiTexturedUniform < 0 ||
            uiParticleFogColorUniform < 0 || uiSourceRasterUniform < 0 || gcParticleModeUniform < 0 ||
            uiSourceFogEnabledUniform < 0 || uiSourceAlphaTestUniform < 0 ||
            uiSourceCompositionUniform < 0 || (!uiSourceCoherentFramebufferFetch && uiSourceTargetUniform < 0) ||
            uiSourceBlendUniform < 0 || uiCoveragePayloadUniform < 0)
            return fail(*diagnostic, VehicleWindowError50DS::shader, "original UI uniform missing");
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::none;
        const U uiCapacity=request.enableOriginalUiSourceRgba?originalUiMaximumSourceQuads50KM:originalUiMaximumQuads50KG;
        const std::intptr_t vertexBytes = uiCapacity * 4 * sizeof(assets::Vertex48B);
        gl.genBuffers(1, &uiVertexBuffer); drawGl.bindBuffer(arrayBuffer, uiVertexBuffer);
        gl.bufferData(arrayBuffer, vertexBytes, nullptr, dynamicDraw);
        if (!check("allocate bounded original UI vertices")) return false;
        if (!uiVertexBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI vertex buffer");
        if (!verifyBuffer(arrayBuffer, vertexBytes, dynamicDraw)) return false;
        const std::intptr_t fogBytes = uiCapacity * 4 * sizeof(float);
        gl.genBuffers(1, &uiParticleFogBuffer); drawGl.bindBuffer(arrayBuffer, uiParticleFogBuffer);
        gl.bufferData(arrayBuffer, fogBytes, nullptr, dynamicDraw);
        if (!check("allocate bounded original UI particle fog")) return false;
        if (!uiParticleFogBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI fog buffer");
        if (!verifyBuffer(arrayBuffer, fogBytes, dynamicDraw)) return false;
        gl.genBuffers(1, &uiSourceRasterWBuffer); drawGl.bindBuffer(arrayBuffer, uiSourceRasterWBuffer);
        gl.bufferData(arrayBuffer, fogBytes, nullptr, dynamicDraw);
        if (!check("allocate bounded original UI source W")) return false;
        if (!uiSourceRasterWBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI source W buffer");
        if (!verifyBuffer(arrayBuffer, fogBytes, dynamicDraw)) return false;
        const std::intptr_t stqBytes = uiCapacity * 4 * sizeof(std::array<float,3>);
        gl.genBuffers(1, &uiSourcePacketStqBuffer); drawGl.bindBuffer(arrayBuffer, uiSourcePacketStqBuffer);
        gl.bufferData(arrayBuffer, stqBytes, nullptr, dynamicDraw);
        if (!check("allocate bounded original UI packet STQ")) return false;
        if (!uiSourcePacketStqBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI packet STQ buffer");
        if (!verifyBuffer(arrayBuffer, stqBytes, dynamicDraw)) return false;
        // Heap allocation is cold-only; no large transient frame stack.
        std::unique_ptr<U[]> indices(new (std::nothrow) U[uiCapacity * 6]);
        if (!indices) return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate original UI quad indices");
        constexpr U corners[] = {0, 1, 2, 0, 2, 3};
        for (U i = 0; i < uiCapacity * 6; ++i) indices[i] = (i / 6) * 4 + corners[i % 6];
        const std::intptr_t indexBytes = uiCapacity * 6 * sizeof(U);
        gl.genBuffers(1, &uiIndexBuffer); drawGl.bindBuffer(arrayBuffer, uiIndexBuffer);
        gl.bufferData(arrayBuffer, indexBytes, indices.get(), staticDraw);
        if (!check("upload original UI quad indices")) return false;
        if (!uiIndexBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI index buffer");
        if (!verifyBuffer(arrayBuffer, indexBytes, staticDraw)) return false;
        U uploaded[256];
        for (U i = 0; i < uiCapacity * 6; i += 256) {
            const auto bytes = std::min<U>(256, uiCapacity * 6 - i) * sizeof(U);
            gl.getBufferSubData(arrayBuffer, std::intptr_t(i) * sizeof(U), bytes, uploaded);
            if (!check("verify original UI index bytes")) return false;
            if (std::memcmp(indices.get() + i, uploaded, bytes))
                return fail(*diagnostic, VehicleWindowError50DS::indices, "original UI GPU index mismatch");
        }
        if (request.enableOriginalUiSourceRgba) {
            uiSourceIndices.reset(new (std::nothrow) UiSourceIndexStream);
            if (!uiSourceIndices) return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "allocate bounded original UI source indices and span ranges");
            gl.genBuffers(1, &uiSourceIndexBuffer); drawGl.bindBuffer(arrayBuffer, uiSourceIndexBuffer);
            gl.bufferData(arrayBuffer, indexBytes, nullptr, dynamicDraw);
            if (!check("allocate bounded original UI source index buffer")) return false;
            if (!uiSourceIndexBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "zero UI source index buffer");
            if (!verifyBuffer(arrayBuffer, indexBytes, dynamicDraw)) return false;
        }
        gl.genVertexArrays(1, &uiVertexArray); drawGl.bindVertexArray(uiVertexArray);
        drawGl.bindBuffer(arrayBuffer, uiVertexBuffer); drawGl.bindBuffer(elementBuffer, uiIndexBuffer);
        constexpr I sizes[] = {3, 2, 4};
        constexpr std::uintptr_t offsets[] = {0, 12, 20};
        for (U a = 0; a < 3; ++a) {
            gl.enableVertexAttribArray(a);
            gl.vertexAttribPointer(a, sizes[a], a == 2 ? unsignedByte : floatType,
                a == 2 ? 1 : 0, 24, reinterpret_cast<const void*>(offsets[a]));
        }
        if (!check("create original UI Vertex48B layout")) return false;
        if (!uiVertexArray) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI vertex array");
        if (!verifyLayout(uiVertexBuffer, uiIndexBuffer)) return false;
        drawGl.bindBuffer(arrayBuffer, uiParticleFogBuffer);
        gl.enableVertexAttribArray(3);
        gl.vertexAttribPointer(3, 1, floatType, 0, sizeof(float), nullptr);
        if (!check("create original UI particle fog layout")) return false;
        constexpr U fogProperties[] = {0x8622, 0x8623, 0x8624, 0x8625, 0x886A, 0x889F, 0x88FD};
        const I fogExpected[] = {1, 1, sizeof(float), static_cast<I>(floatType), 0,
            static_cast<I>(uiParticleFogBuffer), 0};
        for (U p = 0; p < 7; ++p) {
            I value = 0; gl.getVertexAttribiv(3, fogProperties[p], &value);
            if (!check("verify original UI particle fog attribute")) return false;
            if (value != fogExpected[p]) return fail(*diagnostic, VehicleWindowError50DS::layout,
                "particle fog attribute mismatch");
        }
        void* fogPointer = nullptr; gl.getVertexAttribPointerv(3, 0x8645, &fogPointer);
        if (!check("verify original UI particle fog offset")) return false;
        if (fogPointer) return fail(*diagnostic, VehicleWindowError50DS::layout, "particle fog offset mismatch");
        drawGl.bindBuffer(arrayBuffer, uiSourceRasterWBuffer);
        gl.enableVertexAttribArray(4);
        gl.vertexAttribPointer(4, 1, floatType, 0, sizeof(float), nullptr);
        if (!check("create original UI source W layout")) return false;
        const I wExpected[] = {1, 1, sizeof(float), static_cast<I>(floatType), 0,
            static_cast<I>(uiSourceRasterWBuffer), 0};
        for (U p = 0; p < 7; ++p) {
            I value = 0; gl.getVertexAttribiv(4, fogProperties[p], &value);
            if (!check("verify original UI source W attribute")) return false;
            if (value != wExpected[p]) return fail(*diagnostic, VehicleWindowError50DS::layout,
                "source W attribute mismatch");
        }
        void* wPointer = nullptr; gl.getVertexAttribPointerv(4, 0x8645, &wPointer);
        if (!check("verify original UI source W offset")) return false;
        if (wPointer) return fail(*diagnostic, VehicleWindowError50DS::layout, "source W offset mismatch");
        drawGl.bindBuffer(arrayBuffer, uiSourcePacketStqBuffer);
        gl.enableVertexAttribArray(5);
        gl.vertexAttribPointer(5, 3, floatType, 0, sizeof(std::array<float,3>), nullptr);
        if (!check("create original UI packet STQ layout")) return false;
        const I stqExpected[] = {1, 3, sizeof(std::array<float,3>), static_cast<I>(floatType), 0,
            static_cast<I>(uiSourcePacketStqBuffer), 0};
        for (U p = 0; p < 7; ++p) {
            I value = 0; gl.getVertexAttribiv(5, fogProperties[p], &value);
            if (!check("verify original UI packet STQ attribute")) return false;
            if (value != stqExpected[p]) return fail(*diagnostic, VehicleWindowError50DS::layout,
                "source packet STQ attribute mismatch");
        }
        void* stqPointer = nullptr; gl.getVertexAttribPointerv(5, 0x8645, &stqPointer);
        if (!check("verify original UI packet STQ offset")) return false;
        return !stqPointer || fail(*diagnostic, VehicleWindowError50DS::layout, "source packet STQ offset mismatch");
    }

bool VehicleWindowGl50DS::Storage::ensureUiSourceReceiptLifetime(VehicleWindowDiagnostic50DS& d) noexcept {
        if (uiSourceReceiptLifetime)return true;
        try {uiSourceReceiptLifetime=std::make_shared<const std::uint8_t>(0);return true;}
        catch (...) {return fail(d,VehicleWindowError50DS::allocation,"allocate source receipt lifetime");}
    }

bool VehicleWindowGl50DS::Storage::createRetainedUiSourceTarget() noexcept {
        gl.genFramebuffers(1,&uiRetainedTarget);gl.bindFramebuffer(framebuffer,uiRetainedTarget);
        gl.genTextures(1,&uiRetainedColor);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,uiRetainedColor);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        drawGl.bindBuffer(0x88EC,0);
        gl.texImage2D(texture2D,0,rgba8,640,448,0,0x1908,0x1401,nullptr);
        if (!check("allocate distinct retained source RGBA8 texture"))return false;
        if (!uiRetainedTarget || !uiRetainedColor || uiRetainedTarget==uiSourceTarget || uiRetainedColor==uiSourceColor)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"retained source names must be distinct");
        I w=0,h=0,format=0;
        gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);gl.getTexLevelParameteriv(texture2D,0,0x1001,&h);
        gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
        if (!check("verify retained source texture"))return false;
        if (w!=640 || h!=448 || format!=static_cast<I>(rgba8))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"retained source texture storage mismatch");
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,uiRetainedColor,0);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);gl.bindTexture(texture2D,0);
        if (!check("complete retained source framebuffer"))return false;
        if (status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete retained source framebuffer");}
        return true;
    }

bool VehicleWindowGl50DS::Storage::hasUiByteBlendNames()const noexcept {
        return uiBlendIncomingTarget || uiBlendIncomingColor || uiBlendEffectTarget || uiBlendEffectColor ||
            uiBlendProgram || uiBlendVertexShader || uiBlendFragmentShader || uiBlendVertexArray;
    }

bool VehicleWindowGl50DS::Storage::createUiByteBlendTarget(U& target,U& color) noexcept {
        gl.genFramebuffers(1,&target);
        if(!check("allocate byte-blend framebuffer name"))return false;
        if(!target || target==uiSourceTarget || target==uiRetainedTarget ||
            (&target==&uiBlendEffectTarget && target==uiBlendIncomingTarget))
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"byte-blend framebuffer must be distinct");
        gl.genTextures(1,&color);
        if(!check("allocate byte-blend texture name"))return false;
        if(!color || color==uiSourceColor || color==uiRetainedColor ||
            (&color==&uiBlendEffectColor && color==uiBlendIncomingColor))
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"byte-blend texture must be distinct");
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,color);
        if(gl.bindSampler)gl.bindSampler(0,0);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        drawGl.bindBuffer(0x88EC,0);
        gl.texImage2D(texture2D,0,rgba8,640,448,0,rgba,unsignedByte,nullptr);
        if(!check("allocate immutable-pair byte-blend RGBA8 storage"))return false;
        I w=0,h=0,format=0;
        gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);gl.getTexLevelParameteriv(texture2D,0,0x1001,&h);
        gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
        if(!check("verify byte-blend RGBA8 storage"))return false;
        if(w!=640 || h!=448 || format!=static_cast<I>(rgba8))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"byte-blend storage must be640x448 RGBA8");
        gl.bindFramebuffer(framebuffer,target);
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,color,0);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);
        if(!check("verify byte-blend framebuffer"))return false;
        if(status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete byte-blend framebuffer");}
        return true;
    }

bool VehicleWindowGl50DS::Storage::createUiByteBlendProgram() noexcept {
        // No new loader, shader, VAO or allocation on the normal UI path.
        if(!load(gl.drawArrays,"glDrawArrays"))return false;
        static const char* vertexSource=R"GLSL(#version 150 core
void main() {
    vec2 p=gl_VertexID==0 ? vec2(-1.0,-1.0) :
           gl_VertexID==1 ? vec2(3.0,-1.0) : vec2(-1.0,3.0);
    gl_Position=vec4(p,0.0,1.0);
}
)GLSL";
        static const char* fragmentSource=R"GLSL(#version 150 core
uniform sampler2D oldImage;
uniform sampler2D incomingImage;
uniform int sourceAlpha;
out vec4 viewerOutput;
int signedFloor128(int value) {
    return value>=0 ? value/128 : -((-value+127)/128);
}
void main() {
    // Both source textures have identical lower-left GL orientation. Source
    // top-down639x447 coverage excludes GL row0 and column639. No UV mix,
    // rescaling, filtering, gamma conversion or preceding-effect feedback.
    ivec2 q=ivec2(gl_FragCoord.xy);
    ivec4 oldByte=ivec4(floor(texelFetch(oldImage,q,0)*255.0+0.5));
    if(q.x==639 || q.y==0) {
        viewerOutput=vec4(oldByte)/255.0;
        return;
    }
    ivec3 incomingByte=ivec3(floor(texelFetch(incomingImage,q,0).rgb*255.0+0.5));
    ivec3 product=(incomingByte-oldByte.rgb)*sourceAlpha;
    ivec3 result=oldByte.rgb+ivec3(signedFloor128(product.r),
        signedFloor128(product.g),signedFloor128(product.b));
    // Alpha is the raw source byte, not alpha/128. Endpoints still draw.
    viewerOutput=vec4(vec3(result),float(sourceAlpha))/255.0;
}
)GLSL";
        const auto compile=[&](U type,const char* source,U& name) noexcept {
            name=shaderGl.createShader(type);
            if(!check("create byte-blend shader"))return false;
            if(!name)return fail(*diagnostic,VehicleWindowError50DS::shader,"zero byte-blend shader");
            shaderGl.shaderSource(name,1,&source,nullptr);shaderGl.compileShader(name);
            I compiled=0;shaderGl.getShaderiv(name,0x8B81,&compiled);
            if(!check("compile byte-blend shader"))return false;
            if(compiled)return true;
            shaderGl.getShaderInfoLog(name,static_cast<I>(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
            check("read byte-blend shader log");
            return fail(*diagnostic,VehicleWindowError50DS::shader,"byte-blend shader compilation failed");
        };
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::vertex;
        if(!compile(0x8B31,vertexSource,uiBlendVertexShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::fragment;
        if(!compile(0x8B30,fragmentSource,uiBlendFragmentShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::link;
        uiBlendProgram=shaderGl.createProgram();
        if(!check("create byte-blend program"))return false;
        if(!uiBlendProgram)return fail(*diagnostic,VehicleWindowError50DS::shader,"zero byte-blend program");
        shaderGl.attachShader(uiBlendProgram,uiBlendVertexShader);shaderGl.attachShader(uiBlendProgram,uiBlendFragmentShader);
        shaderGl.bindFragDataLocation(uiBlendProgram,vehicleGlColorOutput50CZ,"viewerOutput");
        shaderGl.linkProgram(uiBlendProgram);
        I linked=0;shaderGl.getProgramiv(uiBlendProgram,0x8B82,&linked);
        if(!check("link byte-blend program"))return false;
        if(!linked){
            shaderGl.getProgramInfoLog(uiBlendProgram,static_cast<I>(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
            check("read byte-blend program log");
            return fail(*diagnostic,VehicleWindowError50DS::shader,"byte-blend program link failed");
        }
        uiBlendOldUniform=shaderGl.getUniformLocation(uiBlendProgram,"oldImage");
        uiBlendIncomingUniform=shaderGl.getUniformLocation(uiBlendProgram,"incomingImage");
        uiBlendAlphaUniform=shaderGl.getUniformLocation(uiBlendProgram,"sourceAlpha");
        if(!check("locate byte-blend uniforms"))return false;
        if(uiBlendOldUniform<0 || uiBlendIncomingUniform<0 || uiBlendAlphaUniform<0)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"missing byte-blend uniform");
        gl.genVertexArrays(1,&uiBlendVertexArray);
        if(!check("allocate byte-blend vertex array"))return false;
        if(!uiBlendVertexArray)return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero byte-blend vertex array");
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::none;return true;
    }

bool VehicleWindowGl50DS::Storage::restoreUiByteBlendBindings() noexcept {
        // Explicit renderer baseline, not restoration of arbitrary foreign GL
        // state. No EBO is touched and no source depth clear is performed here.
        gl.useProgram(0);drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
        for(U unit=0;unit<2;++unit){gl.activeTexture(textureUnit0+unit);gl.bindTexture(texture2D,0);
            if(gl.bindSampler)gl.bindSampler(unit,0);}
        gl.activeTexture(textureUnit0);gl.bindFramebuffer(framebuffer,0);
        gl.disable(0x0BE2);gl.disable(0x0C11);gl.enable(0x0B71);
        gl.depthMask(1);gl.depthFunc(0x0201);gl.clearDepth(1.0);gl.colorMask(1,1,1,1);
        return check("restore byte-blend renderer bindings without depth clear");
    }

bool VehicleWindowGl50DS::Storage::deleteUiByteBlendNames(VehicleWindowDiagnostic50DS& d) noexcept {
        return deleteShaderName(uiBlendProgram,true,d) &&
            deleteShaderName(uiBlendVertexShader,false,d) && deleteShaderName(uiBlendFragmentShader,false,d) &&
            deleteName(uiBlendVertexArray,gl.deleteVertexArrays,d) &&
            deleteName(uiBlendEffectTarget,gl.deleteFramebuffers,d) && deleteName(uiBlendEffectColor,gl.deleteTextures,d) &&
            deleteName(uiBlendIncomingTarget,gl.deleteFramebuffers,d) && deleteName(uiBlendIncomingColor,gl.deleteTextures,d);
    }

bool VehicleWindowGl50DS::Storage::copyUiSourceToOpaque(U sourceTarget,const float* clear,I outputWidth,I outputHeight) noexcept {
        const auto pending=static_cast<std::uint8_t>(uiPublished^1u);
        // Only this independent private presentation surface gets alpha1.
        gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);
        gl.colorMask(1,1,1,1);
        gl.bindFramebuffer(framebuffer,uiTargets[pending]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        gl.viewport(0,0,outputWidth,outputHeight);gl.clearColor(clear[0],clear[1],clear[2],1.f);gl.clear(colorBufferBit);
        const double fit=std::min(double(outputWidth)/640.0,double(outputHeight)/448.0);
        const I rw=std::max<I>(1,static_cast<I>(std::floor(640.0*fit)));
        const I rh=std::max<I>(1,static_cast<I>(std::floor(448.0*fit)));
        const I x=(outputWidth-rw)/2,y=(outputHeight-rh)/2;
        gl.bindFramebuffer(readFramebuffer,sourceTarget);gl.readBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,640,448,x,y,x+rw,y+rh,colorBufferBit,0x2600);
        gl.bindFramebuffer(framebuffer,uiTargets[pending]);
        gl.colorMask(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
        const bool copied=check("copy source UI RGB to separate opaque presentation");
        // Preserve the previous combined path's post-copy baseline, including
        // a failed opaque copy. Source acceptance/history is already separate.
        // Headless historically leaves this private UI FBO bound; window
        // success reaches framebuffer0 through copyToWindow below.
        gl.disable(0x0C11);gl.disable(0x0BE2);gl.depthMask(1);gl.depthFunc(0x0201);
        gl.colorMask(1,1,1,1);gl.clearDepth(1.0);
        drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
        const bool restored=check("finish source publication baseline");
        return copied && restored;
    }

bool VehicleWindowGl50DS::Storage::publishUiSource(const float* clear,I outputWidth,I outputHeight) noexcept {
        const auto pending=static_cast<std::uint8_t>(uiPublished^1u);
        if(!copyUiSourceToOpaque(uiSourceTarget,clear,outputWidth,outputHeight))return false;
        uiPrivatePublicationGeneration=0;
        uiPublished=pending;displayedTarget=uiTargets[pending];
        width=committedWidth=outputWidth;height=committedHeight=outputHeight;
        imageCommitted=true;diagnostic->originalUiAccepted=true;
        // Private publication may survive a failed window copy, as before.
        // Such a partial window operation must not mint a retained publication.
        uiSourcePublishedTargetGeneration=0;
        bool accepted=true;
        if (request.output==VehicleWindowOutput50DS::defaultBackBuffer){
            accepted=copyToWindow();
            if (!accepted && diagnostic->error==VehicleWindowError50DS::gl)
                diagnostic->error=VehicleWindowError50DS::presentation;
        }
        if (accepted){
            uiSourcePublishedTargetGeneration=uiSourceTargetGeneration;
            diagnostic->originalUiPublicationGeneration=++uiSourcePublicationGeneration;
        }
        return accepted;
    }

bool VehicleWindowGl50DS::Storage::createOriginalUiSourceTarget() noexcept {
        if (!request.enableOriginalUiSourceRgba)return true;
        I textureLimit=0;gl.getIntegerv(0x0D33,&textureLimit);
        if (!check("query source UI texture limits"))return false;
        if (textureLimit<640)return fail(*diagnostic,VehicleWindowError50DS::limits,"source UI texture dimensions unavailable");
        gl.genFramebuffers(1,&uiSourceTarget);gl.bindFramebuffer(framebuffer,uiSourceTarget);
        gl.genTextures(1,&uiSourceColor);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,uiSourceColor);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        drawGl.bindBuffer(0x88EC,0); // Null data is allocation, never an inherited unpack-PBO offset.
        gl.texImage2D(texture2D,0,rgba8,640,448,0,0x1908,0x1401,nullptr);
        if (!check("allocate fixed source UI RGBA8 texture") || !uiSourceTarget || !uiSourceColor)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"source UI framebuffer/texture allocation");
        I w=0,h=0,format=0;
        gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);gl.getTexLevelParameteriv(texture2D,0,0x1001,&h);
        gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
        if (!check("verify source UI texture") || w!=640 || h!=448 || format!=static_cast<I>(rgba8))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"source UI texture storage mismatch");
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,uiSourceColor,0);
        gl.genRenderbuffers(1,&uiSourceDepth);gl.bindRenderbuffer(renderbuffer,uiSourceDepth);
        gl.renderbufferStorage(renderbuffer,depth24,640,448);
        I samples=0;gl.getRenderbufferParameteriv(renderbuffer,0x8D42,&w);
        gl.getRenderbufferParameteriv(renderbuffer,0x8D43,&h);gl.getRenderbufferParameteriv(renderbuffer,0x8D44,&format);
        gl.getRenderbufferParameteriv(renderbuffer,0x8CAB,&samples);
        if (!check("allocate source UI depth") || !uiSourceDepth || w!=640 || h!=448 ||
            format!=static_cast<I>(depth24) || samples)
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"source UI depth storage mismatch");
        gl.framebufferRenderbuffer(framebuffer,depthAttachment,renderbuffer,uiSourceDepth);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);
        gl.bindTexture(texture2D,0);
        if (!check("complete source UI framebuffer"))return false;
        if (status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete source UI framebuffer");}
        return true;
    }

U VehicleWindowGl50DS::Storage::garageSampler50KG(const MaterialTextureSampler50EB& sampler)const noexcept {
        const auto filter=[](MaterialTextureFilter50EB v){return v==MaterialTextureFilter50EB::nearest||v==MaterialTextureFilter50EB::linear;};
        const auto wrap=[](MaterialTextureWrap50EB v){return v==MaterialTextureWrap50EB::repeat||v==MaterialTextureWrap50EB::clampToEdge;};
        if(!garageSamplersReady50KG||!filter(sampler.minification)||sampler.minification!=sampler.magnification||
           !wrap(sampler.s)||!wrap(sampler.t))return 0;
        const U index=(sampler.minification==MaterialTextureFilter50EB::linear?4u:0u)|
            (sampler.s==MaterialTextureWrap50EB::clampToEdge?2u:0u)|(sampler.t==MaterialTextureWrap50EB::clampToEdge?1u:0u);
        return garageSamplers50KG[index];
    }

bool VehicleWindowGl50DS::Storage::ensureGarageSamplers50KG()noexcept {
        if(garageSamplersReady50KG){
            for(U name:garageSamplers50KG)if(!name)return fail(*diagnostic,VehicleWindowError50DS::material,"Garage sampler owner lost a published name");
            return true;
        }
        // A partial creation owns its names until real cleanup, never replay it.
        for(U name:garageSamplers50KG)if(name)return fail(*diagnostic,VehicleWindowError50DS::material,"partial Garage sampler creation retained for cleanup");
        // bindSampler exists only after the unchanged advertised capability check.
        // Created lazily for authenticated Garage or Rider material sampling.
        if(!gl.bindSampler)return fail(*diagnostic,VehicleWindowError50DS::context,"Garage per-material sampling requires advertised sampler objects");
        if(!load(gl.genSamplers,"glGenSamplers")||!load(gl.deleteSamplers,"glDeleteSamplers")||
           !load(gl.samplerParameteri,"glSamplerParameteri")||!load(gl.getSamplerParameteriv,"glGetSamplerParameteriv"))return false;
        constexpr U parameters[]={0x2801,0x2800,0x2802,0x2803};
        for(U i=0;i<8;++i){
            gl.genSamplers(1,&garageSamplers50KG[i]);
            if(!check("create bounded Garage sampler object"))return false;
            if(!garageSamplers50KG[i])return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero Garage sampler name");
            const I values[]={(i&4u)?0x2601:0x2600,(i&4u)?0x2601:0x2600,
                (i&2u)?0x812F:0x2901,(i&1u)?0x812F:0x2901};
            for(U j=0;j<4;++j)gl.samplerParameteri(garageSamplers50KG[i],parameters[j],values[j]);
            if(!check("configure immutable Garage sampler object"))return false;
            for(U j=0;j<4;++j){
                I actual=-1;gl.getSamplerParameteriv(garageSamplers50KG[i],parameters[j],&actual);
                if(!check("verify immutable Garage sampler object"))return false;
                if(actual!=values[j])return fail(*diagnostic,VehicleWindowError50DS::material,"Garage sampler parameter publication mismatch");
            }
        }
        garageSamplersReady50KG=true;return true;
    }

bool VehicleWindowGl50DS::Storage::checkOriginalUiSourceSupport(I major,I minor) noexcept {
        if (!request.enableOriginalUiSourceRgba)return true;
        I units=0;gl.getIntegerv(0x8872,&units);
        if (!check("query source UI composition texture units"))return false;
        bool supported=major>4 || (major==4 && minor>=5);
        bool clipControl=supported;
        bool coherentFetch=false;
        if (!load(gl.getStringi,"glGetStringi"))return false;
        I count=0;gl.getIntegerv(0x821D,&count);
        if (!check("query source UI extension count"))return false;
        if (count<0 || count>65536)return fail(*diagnostic,VehicleWindowError50DS::limits,"invalid extension count");
        for (I i=0;i<count;++i) {
            const auto* extension=gl.getStringi(0x1F03,static_cast<U>(i));
            if (!check("query source UI fetch, texture barrier and clip capabilities"))return false;
            if (!extension)return fail(*diagnostic,VehicleWindowError50DS::context,"null extension name");
            const auto* name=reinterpret_cast<const char*>(extension);
            if (!std::strcmp(name,"GL_ARB_texture_barrier"))supported=true;
            if (!std::strcmp(name,"GL_ARB_clip_control"))clipControl=true;
            // The non_coherent extension alone cannot order overlapping triangles.
            if (!std::strcmp(name,"GL_EXT_shader_framebuffer_fetch"))coherentFetch=true;
        }
        if (units<(coherentFetch?1:2))return fail(*diagnostic,VehicleWindowError50DS::context,
            "source UI has insufficient fragment texture units");
        if (!coherentFetch && !supported)return fail(*diagnostic,VehicleWindowError50DS::context,
            "source UI requires coherent EXT_shader_framebuffer_fetch, GL4.5 or ARB_texture_barrier");
        if ((!coherentFetch && !load(gl.textureBarrier,"glTextureBarrier")) ||
            (clipControl && !load(gl.clipControl,"glClipControl")))return false;
        uiSourceCoherentFramebufferFetch=coherentFetch;
        return true;
    }

bool VehicleWindowGl50DS::Storage::initializeOriginalFrontend(const VehicleWindowRequest50DS& input) noexcept {
        request=input;frontendOnly=true;
        maximumWidth=input.maximumWidth;maximumHeight=input.maximumHeight;
        if(!validateOriginalUiBank(input))return false;
        uiTexturePublication=input.originalUiTextures->publicationToken();
        if(!current(*diagnostic)||!texturesCurrent(*diagnostic))return false;
        if(!loadFunctions()||!check("initial frontend GL error state")||!checkContext())return false;
        if(!createOriginalUi()||!createTargets())return false;
        if(request.output==VehicleWindowOutput50DS::defaultBackBuffer)gl.bindFramebuffer(framebuffer,0);
        if(!check("finish dedicated original frontend creation"))return false;
        initialized=true;
        diagnostic->originalUiCoherentFramebufferFetch=uiSourceCoherentFramebufferFetch;
        return true;
    }

bool VehicleWindowGl50DS::Storage::deleteUiCoverage(VehicleWindowDiagnostic50DS& d)noexcept {
        uiCoverage.ready=false;
        for(unsigned i=0;i<2;++i)if(!deleteName(uiCoverage.raster[i],gl.deleteFramebuffers,d)||
            !deleteName(uiCoverage.payload[i],gl.deleteTextures,d)||!deleteName(uiCoverage.rasterDepth[i],gl.deleteTextures,d))return false;
        if(!deleteName(uiCoverage.scene,gl.deleteFramebuffers,d)||!deleteName(uiCoverage.color,gl.deleteTextures,d)||
            !deleteName(uiCoverage.depth,gl.deleteTextures,d)||!deleteName(uiCoverage.snapshot,gl.deleteFramebuffers,d)||
            !deleteName(uiCoverage.snapshotColor,gl.deleteTextures,d)||!deleteShaderName(uiCoverage.program,true,d)||
            !deleteShaderName(uiCoverage.vertex,false,d)||!deleteShaderName(uiCoverage.fragment,false,d)||
            !deleteName(uiCoverage.array,gl.deleteVertexArrays,d))return false;
        uiCoverage={};return true;
    }

bool VehicleWindowGl50DS::Storage::uiCoverageDepthTexture(U& name)noexcept {
        gl.genTextures(1,&name);if(!check("allocate UI AA0 depth name")||!name)return false;
        gl.bindTexture(texture2D,name);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        drawGl.bindBuffer(0x88EC,0);
        gl.texImage2D(texture2D,0,depth24,640,448,0,0x1902,0x1405,nullptr);
        I w=0,h=0,f=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
        gl.getTexLevelParameteriv(texture2D,0,0x1001,&h);gl.getTexLevelParameteriv(texture2D,0,0x1003,&f);
        if(!check("verify UI center depth storage")||w!=640||h!=448||f!=static_cast<I>(depth24))return false;
        gl.framebufferTexture2D(framebuffer,depthAttachment,texture2D,name,0);gl.bindTexture(texture2D,0);
        return check("attach UI center depth");
    }

std::uint64_t VehicleWindowGl50DS::Storage::uiCoverageNs(UiCoverageClock::time_point begin)noexcept {
        return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(UiCoverageClock::now()-begin).count());
    }

bool VehicleWindowGl50DS::Storage::prepareUiCoverageRaw()noexcept {
        if(uiCoverage.ready)return true;
        // Failed private allocations remain owned for explicit renderer cleanup.
        if(uiCoverage.attempted)return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"UI coverage preparation previously failed");
        uiCoverage.attempted=true;
        I major=0,samples=0,colorSamples=0,depthSamples=0,units=0,limit=0;
        gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x8D57,&samples);gl.getIntegerv(0x910E,&colorSamples);
        gl.getIntegerv(0x910F,&depthSamples);gl.getIntegerv(0x8872,&units);gl.getIntegerv(0x0D33,&limit);
        if(!check("query demand-bound UI four-sample limits"))return false;
        if(major<4||samples<4||colorSamples<4||depthSamples<4||units<5||limit<640){
            std::fprintf(stderr,"ui_aa1_limits major=%d samples=%d color=%d depth=%d units=%d limit=%d\n",int(major),int(samples),int(colorSamples),int(depthSamples),int(units),int(limit));
            return fail(*diagnostic,VehicleWindowError50DS::limits,"UI AA1 requires GL4, five texture units and exact four samples; no fallback");}
        if(!load(gl.texImage2DMultisample,"glTexImage2DMultisample")||!load(gl.clearBufferfv,"glClearBufferfv")||
            !load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
            !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
            !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||!load(gl.framebufferTexture2D,"glFramebufferTexture2D")||
            !load(gl.blitFramebuffer,"glBlitFramebuffer")||!load(gl.drawArrays,"glDrawArrays"))return false;
        {UiCoverageTimer timer{uiCoverageAllocationNs};
        if(!coverageFramebuffer(uiCoverage.scene)||!coverageTextureStorage(uiCoverage.color,rgba8,640,448,true,colorAttachment)||
            !coverageTextureStorage(uiCoverage.depth,depth24,640,448,true,depthAttachment))return false;
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);if(!coverageComplete())return false;
        if(!coverageFramebuffer(uiCoverage.snapshot)||!coverageTextureStorage(uiCoverage.snapshotColor,rgba8,640,448,true,colorAttachment))return false;
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);if(!coverageComplete())return false;
        for(unsigned i=0;i<2;++i){
            if(!coverageFramebuffer(uiCoverage.raster[i])||!coverageTextureStorage(uiCoverage.payload[i],0x881A,640,448,i!=0,colorAttachment))return false;
            if(i? !coverageTextureStorage(uiCoverage.rasterDepth[i],depth24,640,448,true,depthAttachment):!uiCoverageDepthTexture(uiCoverage.rasterDepth[i]))return false;
            gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);if(!coverageComplete())return false;
        }
        }
        constexpr const char* vertex=R"GLSL(#version 400 core
void main(){vec2 p=vec2((gl_VertexID<<1)&2,gl_VertexID&2);gl_Position=vec4(p*2.0-1.0,0.0,1.0);}
)GLSL";
        constexpr const char* fragment=R"GLSL(#version 400 core
uniform sampler2D payload0;uniform sampler2D depth0;
uniform sampler2DMS destination;uniform sampler2DMS payload1;uniform sampler2DMS depth1;
uniform int coverageMode;uniform int sourceBlend;
out vec4 composedColor;
void main(){
    ivec2 p=ivec2(gl_FragCoord.xy);int sampleIndex=gl_SampleID;
    vec4 source=coverageMode!=0?texelFetch(payload1,p,sampleIndex):texelFetch(payload0,p,0);
    if(source.a<0.0)discard;
    gl_FragDepth=coverageMode!=0?texelFetch(depth1,p,sampleIndex).r:texelFetch(depth0,p,0).r;
    vec3 rgb=source.rgb;
    if(sourceBlend!=0){
        vec4 dest=floor(texelFetch(destination,p,sampleIndex)*255.0+0.5);
        float coefficient=sourceBlend==3?dest.a:sourceBlend==4?255.0:sourceBlend==6?64.0:source.a;
        vec3 difference=sourceBlend==5?-dest.rgb:sourceBlend==1?source.rgb-dest.rgb:source.rgb;
        rgb=floor(difference*coefficient/128.0)+dest.rgb;
    }
    composedColor=vec4(clamp(rgb,0.0,255.0),source.a)/255.0;
}
)GLSL";
        constexpr const char* coherentFragment=R"GLSL(#version 400 core
#extension GL_EXT_shader_framebuffer_fetch : require
uniform sampler2D payload0;uniform sampler2D depth0;
uniform sampler2DMS payload1;uniform sampler2DMS depth1;
uniform int coverageMode;uniform int sourceBlend;
inout vec4 composedColor;
void main(){
    ivec2 p=ivec2(gl_FragCoord.xy);int sampleIndex=gl_SampleID;
    vec4 source=coverageMode!=0?texelFetch(payload1,p,sampleIndex):texelFetch(payload0,p,0);
    if(source.a<0.0)discard;
    gl_FragDepth=coverageMode!=0?texelFetch(depth1,p,sampleIndex).r:texelFetch(depth0,p,0).r;
    vec3 rgb=source.rgb;
    if(sourceBlend!=0){
        vec4 dest=floor(composedColor*255.0+0.5);
        float coefficient=sourceBlend==3?dest.a:sourceBlend==4?255.0:sourceBlend==6?64.0:source.a;
        vec3 difference=sourceBlend==5?-dest.rgb:sourceBlend==1?source.rgb-dest.rgb:source.rgb;
        rgb=floor(difference*coefficient/128.0)+dest.rgb;
    }
    composedColor=vec4(clamp(rgb,0.0,255.0),source.a)/255.0;
}
)GLSL";
        {UiCoverageTimer timer{uiCoverageProgramNs};
        // Actual coherent extension admission chooses one matching shader and
        // snapshot policy. Compile/link errors reject; never silently downgrade.
        uiCoverage.coherentFetch=uiSourceCoherentFramebufferFetch;
        if(!compileTrackShader(0x8B31,vertex,uiCoverage.vertex)||
           !compileTrackShader(0x8B30,uiCoverage.coherentFetch?coherentFragment:fragment,uiCoverage.fragment))return false;
        uiCoverage.program=shaderGl.createProgram();if(!check("create UI sample composition")||!uiCoverage.program)return false;
        shaderGl.attachShader(uiCoverage.program,uiCoverage.vertex);shaderGl.attachShader(uiCoverage.program,uiCoverage.fragment);
        shaderGl.bindFragDataLocation(uiCoverage.program,0,"composedColor");shaderGl.linkProgram(uiCoverage.program);
        I linked=0;shaderGl.getProgramiv(uiCoverage.program,0x8B82,&linked);
        if(!check("link UI sample composition")||!linked)return fail(*diagnostic,VehicleWindowError50DS::shader,"UI sample composition link failed");
        }
        gl.useProgram(uiCoverage.program);const char* samplers[]={"payload0","depth0","destination","payload1","depth1"};
        for(I i=0;i<5;++i){
            if(i==2&&uiCoverage.coherentFetch)continue; // No sampled destination in this variant.
            const I location=shaderGl.getUniformLocation(uiCoverage.program,samplers[i]);
            if(location<0)return fail(*diagnostic,VehicleWindowError50DS::shader,"UI sample input absent");gl.uniform1i(location,i);}
        uiCoverage.mode=shaderGl.getUniformLocation(uiCoverage.program,"coverageMode");uiCoverage.blend=shaderGl.getUniformLocation(uiCoverage.program,"sourceBlend");
        gl.genVertexArrays(1,&uiCoverage.array);
        if(!check("finish UI sample resources")||!uiCoverage.array||uiCoverage.mode<0||uiCoverage.blend<0)return false;
        uiCoverage.ready=true;return true;
    }

bool VehicleWindowGl50DS::Storage::prepareUiCoverage(const char* phase)noexcept {
        const auto begin=UiCoverageClock::now();const bool readyBefore=uiCoverage.ready;
        uiCoverageAllocationNs=uiCoverageProgramNs=0;
        bool accepted=true;
        if(!readyBefore){
            // This setup is allowed while an outgoing source is retained. Save
            // only real GL bindings; no frame token, draw, resolve or source
            // generation is fabricated. The allocator touches texture unit0,
            // unpack buffer, program and both framebuffer targets.
            I read=0,draw=0,program=0,active=0,texture=0,multi=0,sampler=0,unpack=0,array=0;
            gl.getIntegerv(0x8CAA,&read);gl.getIntegerv(0x8CA6,&draw);gl.getIntegerv(0x8B8D,&program);
            gl.getIntegerv(0x84E0,&active);gl.getIntegerv(0x88EF,&unpack);gl.getIntegerv(0x85B5,&array);
            accepted=check("capture idle UI coverage setup bindings");
            if(accepted){
                gl.activeTexture(textureUnit0);gl.getIntegerv(0x8069,&texture);gl.getIntegerv(0x9104,&multi);
                if(gl.bindSampler)gl.getIntegerv(0x8919,&sampler);
                accepted=check("capture UI setup unit-zero bindings");
                if(!accepted){
                    gl.activeTexture(static_cast<U>(active));
                    if(!check("restore active unit after rejected coverage state capture"))diagnostic->cleanupIncomplete=true;
                }else{
                accepted=prepareUiCoverageRaw();
                // Restore before deleting any partial allocation; the prior
                // outgoing FBO/program/texture owners remain untouched.
                gl.useProgram(static_cast<U>(program));drawGl.bindVertexArray(static_cast<U>(array));
                drawGl.bindBuffer(0x88EC,static_cast<U>(unpack));
                gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,static_cast<U>(texture));
                gl.bindTexture(coverageTexture,static_cast<U>(multi));if(gl.bindSampler)gl.bindSampler(0,static_cast<U>(sampler));
                gl.activeTexture(static_cast<U>(active));gl.bindFramebuffer(readFramebuffer,static_cast<U>(read));
                gl.bindFramebuffer(drawFramebuffer,static_cast<U>(draw));
                const bool restored=check("restore idle UI coverage setup bindings");accepted=accepted&&restored;
                if(!accepted&&!deleteUiCoverage(*diagnostic))diagnostic->cleanupIncomplete=true;
                }
            }
        }
        if(uiCoverageSetupReports<8||!accepted){
            ++uiCoverageSetupReports;
            std::fprintf(stderr,"ui_coverage_setup phase=%s ready_before=%u allocation_wall_ns=%llu compile_link_wall_ns=%llu total_wall_ns=%llu accepted=%u scope=cpu_wall_no_gpu_finish\n",
                phase,unsigned(readyBefore),static_cast<unsigned long long>(uiCoverageAllocationNs),
                static_cast<unsigned long long>(uiCoverageProgramNs),static_cast<unsigned long long>(uiCoverageNs(begin)),unsigned(accepted));
        }
        return accepted;
    }

void VehicleWindowGl50DS::Storage::unbindUiCoverageInputs()noexcept {
        for(U i=0;i<5;++i){gl.activeTexture(textureUnit0+i);gl.bindTexture(texture2D,0);gl.bindTexture(coverageTexture,0);if(gl.bindSampler)gl.bindSampler(i,0);}
        gl.activeTexture(textureUnit0);
    }

bool VehicleWindowGl50DS::Storage::beginUiCoverageSpan(bool aa1,I sourceBlend)noexcept {
        unbindUiCoverageInputs();
        gl.disable(0x0BE2);gl.enable(0x0C11);
        gl.disable(0x809E);gl.disable(0x809F);gl.disable(0x80A0);gl.disable(0x8E51);gl.disable(0x8C36);
        gl.colorMask(1,1,1,1);gl.enable(0x0B71);
        gl.useProgram(uiCoverage.program);
        gl.uniform1i(uiCoverage.mode,aa1?1:0);gl.uniform1i(uiCoverage.blend,sourceBlend);
        gl.useProgram(uiProgram);gl.uniform1i(uiCoveragePayloadUniform,1);
        return check("begin ordered UI coverage span");
    }

bool VehicleWindowGl50DS::Storage::endUiCoverageSpan(U image,U depth,bool write)noexcept {
        // Runs even after bounds/raster/composition failure. Restore the UI VAO
        // before the outer submission restores its immutable element bank.
        unbindUiCoverageInputs();gl.disable(0x0C11);gl.disable(0x809D);
        gl.useProgram(uiProgram);gl.uniform1i(uiCoveragePayloadUniform,0);
        gl.bindTexture(texture2D,image);drawGl.bindVertexArray(uiVertexArray);
        gl.bindFramebuffer(framebuffer,uiCoverage.scene);
        gl.enable(0x0B71);gl.depthFunc(depth);gl.depthMask(write?1:0);
        return check("restore ordered UI state after coverage span");
    }

bool VehicleWindowGl50DS::Storage::drawUiCoverageRange(bool aa1,U index,U indexCount,U image,U depth,bool write,I sourceBlend,
        bool opaqueBatch,const OriginalUiCoverageBounds50KG& region,U sourceSampler)noexcept {
        // No scratch texture remains sampled while attached for raster writes.
        // Keep the full feedback fence for every range. A private Garage
        // sampler applies only to source raster, never the coverage scratch.
        for(U i=0;i<5;++i){gl.activeTexture(textureUnit0+i);gl.bindTexture(texture2D,0);gl.bindTexture(coverageTexture,0);}
        gl.activeTexture(textureUnit0);
        // Same conservative rectangle bounds scratch clear, source raster,
        // destination copy and per-sample composition. Stale scratch elsewhere
        // cannot be read by this triangle's composition invocation domain.
        gl.scissor(region.x,region.y,region.width,region.height);
        gl.bindFramebuffer(framebuffer,uiCoverage.raster[aa1?1:0]);
        gl.depthMask(1);
        // Only the shared predicate's opaque AA0 writing GE/GREATER span
        // enters reduction. GE retains the last equal depth; GREATER the first.
        // Other policies retain one triangle with unconditional scratch depth.
        gl.depthFunc(opaqueBatch?depth:0x0207);
        if(aa1)gl.enable(0x809D);else gl.disable(0x809D);
        const float empty[4]={0,0,0,-1};const float zero=0;
        gl.clearBufferfv(0x1800,0,empty);gl.clearBufferfv(0x1801,0,&zero);
        gl.useProgram(uiProgram);
        gl.bindTexture(texture2D,image);drawGl.bindVertexArray(uiVertexArray);
        if(sourceSampler)gl.bindSampler(0,sourceSampler);
        drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(indexCount),0x1405,reinterpret_cast<const void*>(std::uintptr_t(index)*sizeof(U)),0);
        // Restore sampler0 even on raster failure, before scratch/feedback reads.
        if(sourceSampler)gl.bindSampler(0,0);
        bool accepted=check("raster ordered original UI range into native coverage payload");
        if(accepted&&sourceBlend!=0&&!uiCoverage.coherentFetch){
            gl.bindFramebuffer(readFramebuffer,uiCoverage.scene);gl.bindFramebuffer(drawFramebuffer,uiCoverage.snapshot);
            gl.blitFramebuffer(region.x,region.y,region.x+region.width,region.y+region.height,
                region.x,region.y,region.x+region.width,region.y+region.height,colorBufferBit,0x2600);
            accepted=check("snapshot ordered UI destination samples");
        }
        if(accepted){
            gl.bindFramebuffer(framebuffer,uiCoverage.scene);gl.enable(0x809D);gl.depthFunc(depth);gl.depthMask(write?1:0);
            gl.useProgram(uiCoverage.program);
            // Keep every declared sampler complete, even when the uniform
            // opaque branch performs no destination fetch or snapshot copy.
            const U textures[]={uiCoverage.payload[0],uiCoverage.rasterDepth[0],uiCoverage.snapshotColor,uiCoverage.payload[1],uiCoverage.rasterDepth[1]};
            for(U i=0;i<5;++i){gl.activeTexture(textureUnit0+i);gl.bindTexture(i<2?texture2D:coverageTexture,textures[i]);}
            drawGl.bindVertexArray(uiCoverage.array);gl.drawArrays(0x0004,0,3);
            accepted=check("compose original UI bytes and depth per native sample");
        }
        // No baseline restoration between ranges: the next raster supplies
        // every changing state. Preserve this existing error observation point.
        const bool finished=check("finish ordered UI coverage range");return accepted&&finished;
    }

bool VehicleWindowGl50DS::Storage::promoteUiCoveragePrefix()noexcept {
        // Core ARB framebuffer blit replicates the exact single source sample
        // into every destination sample. Both retained owners verify matching
        // RGBA8/D24 formats and640x448 extents; no resolve or shader conversion.
        unbindUiCoverageInputs();gl.disable(0x0C11);gl.disable(0x809D);
        gl.bindFramebuffer(readFramebuffer,uiSourceTarget);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,uiCoverage.scene);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,640,448,0,0,640,448,colorBufferBit|depthBufferBit,0x2600);
        const bool copied=check("broadcast sample-uniform UI prefix RGBA8 and D24");
        gl.bindFramebuffer(framebuffer,uiCoverage.scene);
        gl.useProgram(uiProgram);drawGl.bindVertexArray(uiVertexArray);
        const bool restored=check("restore source state after UI coverage promotion");
        return copied&&restored;
    }

bool VehicleWindowGl50DS::Storage::resolveUiCoverage()noexcept {
        unbindUiCoverageInputs();gl.disable(0x0C11);gl.disable(0x809D);
        gl.bindFramebuffer(readFramebuffer,uiCoverage.scene);gl.bindFramebuffer(drawFramebuffer,uiSourceTarget);
        gl.blitFramebuffer(0,0,640,448,0,0,640,448,colorBufferBit,0x2600);
        gl.bindFramebuffer(framebuffer,uiSourceTarget);return check("resolve completed UI sample domain once");
    }

bool VehicleWindowGl50DS::bindOriginalUiTextures(const assets::MaterialTextureAssets50EA& cpu,
    const MaterialTextureGl50EB& gpu, VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if (!s.current(d)) return false;
    if (s.active || s.frameSet || !s.request.enableOriginalUi || s.diagnostic ||
        s.uiTextureEpoch==std::numeric_limits<std::uint64_t>::max() || (s.riderTextureBorrow&&s.uiSourceFrameOwner))
        return fail(d, VehicleWindowError50DS::frame, "UI bank replacement requires idle original UI renderer and retired Rider source");
    auto candidate = s.request;
    candidate.originalUiCpuTextures = &cpu;
    candidate.originalUiTextures = &gpu;
    s.diagnostic = &d;
    std::uint8_t stagedAlpha[assets::maxMaterialTextureAssets50EA]{};
    const bool accepted = s.validateOriginalUiBank(candidate,stagedAlpha);
    s.diagnostic = nullptr;
    if (!accepted) return false;
    if(s.riderTextureBorrow&&!s.riderTextureBorrow->releaseRiderBorrow(&s))
        return fail(d,VehicleWindowError50DS::material,"Rider renderer borrower retirement mismatch");
    s.riderTextureBorrow=nullptr;
    std::memcpy(s.uiTextureMaximumAlpha,stagedAlpha,sizeof(stagedAlpha));++s.uiTextureEpoch;
    s.request.originalUiCpuTextures = &cpu;
    s.request.originalUiTextures = &gpu;
    s.uiTexturePublication = gpu.publicationToken();
    return true;
}

bool VehicleWindowGl50DS::prevalidateRiderTextures(const OriginalUiRiderResidencyDemand50KG& demand,
    const MaterialTextureGl50EB& gpu,OriginalUiRiderTextureValidation50KG& out,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};out={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"Rider validation requires actual initialized renderer");
    auto& s=*storage_;
    if(!s.current(d))return false;
    if(s.active||s.frameSet||s.diagnostic||!s.request.enableOriginalUi||!demand.session_||!demand.serial_||
       demand.consumed_||!demand.bank_||!gpu.riderSparse()||gpu.riderSession()!=demand.session_||
       gpu.riderBank()!=demand.bank_.get()||gpu.riderDemandSerial()!=demand.serial_||
       !gpu.validBorrow(s.request.context,s.request.isCurrent,gpu.publicationToken()))
        return fail(d,VehicleWindowError50DS::material,"private pending Rider demand and exact sparse publication required");
    const auto images=demand.bank_->images();const auto* cpu=images?images->textures():nullptr;
    if(!cpu||!cpu->loaded()||!cpu->textureCount()||cpu->textureCount()>assets::maxMaterialTextureAssets50EA||cpu->textureCount()!=gpu.textureCount())
        return fail(d,VehicleWindowError50DS::material,"Rider logical CPU/GPU bank extent mismatch");
    OriginalUiRiderTextureValidation50KG next;
    for(U i=0;i<cpu->textureCount();++i){const auto* source=cpu->texture(i);const auto* image=gpu.asset(i);
        if(!source||bool(image)!=demand.includes(i)||(image&&(!image->name||image->identity!=source->identity)))
            return fail(d,VehicleWindowError50DS::material,"Rider closure residency/CPU identity mismatch");
        std::uint8_t maximum=0;for(std::size_t a=3;a<source->texture.rgba.size();a+=4)maximum=std::max(maximum,source->texture.rgba[a]);
        next.maximumAlpha_[i]=maximum;
    }
    if(demand.requiresCoverage_){
        if(!s.request.enableOriginalUiSourceRgba||s.uiBlendOldSnapshot||s.displayFadeOwner)
            return fail(d,VehicleWindowError50DS::frame,"coverage prevalidation requires idle source UI without active display lease");
        s.diagnostic=&d;const bool prepared=s.prepareUiCoverage("cold_prevalidation");s.diagnostic=nullptr;
        if(!prepared)return false;
    }
    next.requiresCoverage_=demand.requiresCoverage_;
    next.renderer_=this;next.session_=demand.session_;next.bank_=demand.bank_.get();next.gpu_=&gpu;
    next.serial_=demand.serial_;next.token_=gpu.publicationToken();next.rendererEpoch_=s.uiTextureEpoch;next.ready_=true;
    out=next;return true;
}
bool VehicleWindowGl50DS::commitRiderTextures(const OriginalUiRiderResidencyDemand50KG& demand,
    OriginalUiRiderTextureValidation50KG& receipt,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"Rider commit requires initialized renderer");
    auto& s=*storage_;const auto* gpu=receipt.gpu_;
    if(!s.current(d))return false;
    if(s.active||s.frameSet||s.diagnostic||s.uiSourceFrameOwner||!s.request.enableOriginalUi||
       !receipt.ready_||receipt.renderer_!=this||receipt.rendererEpoch_!=s.uiTextureEpoch||
       s.uiTextureEpoch==std::numeric_limits<std::uint64_t>::max()||!demand.consumed_||
       receipt.session_!=demand.session_||receipt.serial_!=demand.serial_||receipt.bank_!=demand.bank_.get()||
       receipt.requiresCoverage_!=demand.requiresCoverage_||(receipt.requiresCoverage_&&!s.uiCoverage.ready)||
       !gpu||!gpu->riderSparse()||gpu->riderSession()!=demand.session_||gpu->riderBank()!=demand.bank_.get()||
       gpu->riderDemandSerial()!=demand.serial_||!gpu->validBorrow(s.request.context,s.request.isCurrent,receipt.token_))
        return fail(d,VehicleWindowError50DS::material,"stale Rider prevalidation or unretired outgoing source");
    const auto images=demand.bank_->images();const auto* cpu=images?images->textures():nullptr;
    if(!cpu||cpu->textureCount()!=gpu->textureCount())return fail(d,VehicleWindowError50DS::material,"Rider commit bank changed");
    for(U i=0;i<cpu->textureCount();++i)if(bool(gpu->asset(i))!=demand.includes(i))
        return fail(d,VehicleWindowError50DS::material,"Rider demand changed after renderer prevalidation");
    if(!gpu->acquireRiderBorrow(&s))return fail(d,VehicleWindowError50DS::material,"Rider candidate renderer lease rejected");
    if(s.riderTextureBorrow&&!s.riderTextureBorrow->releaseRiderBorrow(&s)){
        gpu->releaseRiderBorrow(&s);return fail(d,VehicleWindowError50DS::material,"Rider outgoing renderer lease mismatch");}
    s.riderTextureBorrow=gpu;s.request.originalUiCpuTextures=cpu;s.request.originalUiTextures=gpu;
    s.uiTexturePublication=receipt.token_;++s.uiTextureEpoch;
    std::memcpy(s.uiTextureMaximumAlpha,receipt.maximumAlpha_.data(),sizeof(s.uiTextureMaximumAlpha));
    receipt.ready_=false;return true;
}

bool VehicleWindowGl50DS::submitOriginalUi(const OriginalUiDraw50KG&,
    std::int32_t, std::int32_t, VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d = {};
    return fail(d, VehicleWindowError50DS::material,
        "draw-only original UI lacks explicit depth and alpha-consumer ownership; use native descriptor");
}

bool VehicleWindowGl50DS::submitOriginalUi(const OriginalUiNativeDraw50KG& native,
    std::int32_t width, std::int32_t height, VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    return submitOriginalUiImpl(native,width,height,false,d);
}
bool VehicleWindowGl50DS::originalUiSourceRgbaAvailable() const noexcept {
    return initialized() && storage_->request.enableOriginalUiSourceRgba &&
        (storage_->uiSourceCoherentFramebufferFetch || storage_->gl.textureBarrier) &&
        storage_->uiSourceTarget && storage_->uiSourceColor && storage_->uiSourceDepth &&
        storage_->uiSourceIndexBuffer && storage_->uiSourceIndices;
}
bool VehicleWindowGl50DS::prepareOriginalUiDirectVideo(OriginalUiDirectVideoCapability50KG& out,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};out={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"direct video requires actual source RGBA renderer");
    auto& s=*storage_;
    if(!s.current(d))return false;
    if(s.active||s.frameSet||s.diagnostic||!s.uiProgram||!s.uiVertexArray||!s.uiVertexBuffer)
        return fail(d,VehicleWindowError50DS::frame,"direct video capability requires idle actual UI draw owner");
    // Dedicated frontend creation does not load gameplay-blur/display-fade
    // entry points. Resolve this actual direct sprite dependency explicitly,
    // using the same checked loader before minting any renderer capability.
    s.diagnostic=&d;
    const bool directDrawReady=s.gl.drawArrays||s.load(s.gl.drawArrays,"glDrawArrays");
    s.diagnostic=nullptr;
    if(!directDrawReady)return false;
    try {if(!s.uiDirectVideoLifetime)s.uiDirectVideoLifetime=std::make_shared<const std::uint8_t>(0);}
    catch(...){return fail(d,VehicleWindowError50DS::frame,"direct video capability allocation failed");}
    out.lifetime_=s.uiDirectVideoLifetime;out.renderer_=this;return true;
}
bool VehicleWindowGl50DS::submitOriginalUiSourceRgba(const OriginalUiSourceFrame50KM& frame,
    std::int32_t width,std::int32_t height,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    return submitOriginalUiSourceFrame(frame,width,height,true,d);
}
bool VehicleWindowGl50DS::submitOriginalUiSourceFrame(const OriginalUiSourceFrame50KM& frame,
    std::int32_t width,std::int32_t height,bool publishSource,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if (!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;
    if(s.menuRasterShared50KG&&(!menuRasterPreludeCurrent50KG(*s.menuRasterShared50KG)||
       frame.owner_!=s.menuRasterOwner50KG||frame.native_!=s.menuRasterNative50KG||
       !s.menuRasterFrame50KG||frame.frameGeneration_!=s.menuRasterFrame50KG||frame.garageCanonical_))
        return fail(d,VehicleWindowError50DS::frame,"source frame differs from retained native menu raster contract");
    if(s.garagePreludeOwner50KG&&(frame.owner_!=s.garagePreludeOwner50KG||
       frame.frameGeneration_!=s.garagePreludePublication50KG||s.garagePreludeFailed50KG))
        return fail(d,VehicleWindowError50DS::frame,"source frame differs from actual Garage viewport prelude");
    if (!frame.native_ || !frame.owner_ || !frame.frameGeneration_ ||
        (s.uiSourceFrameOwner && frame.owner_!=s.uiSourceFrameOwner) ||
        (frame.owner_==s.uiSourceFrameOwner && frame.frameGeneration_<=s.uiSourceFrameGeneration) ||
        s.uiSourceTargetGeneration==std::numeric_limits<std::uint64_t>::max() ||
        (publishSource && s.uiSourcePublicationGeneration==std::numeric_limits<std::uint64_t>::max()))
        return fail(d,VehicleWindowError50DS::frame,"unsealed or stale complete original UI frame");
    if(frame.directVideoPresent_&&(!validOriginalUiDirectVideo50KG(frame.directVideo_)||
       frame.directVideoCapability_.renderer_!=this||!frame.directVideoCapability_.valid()||
       frame.directVideoCapability_.lifetime_.lock()!=s.uiDirectVideoLifetime))
        return fail(d,VehicleWindowError50DS::frame,"direct video tuple or live renderer capability rejected");
    const auto* riderGpu=frame.native_->draw.textures;
    const bool riderSparse=riderGpu&&riderGpu->riderSparse();
    if(riderSparse&&(frame.riderSession_!=riderGpu->riderSession()||frame.riderTextures_!=riderGpu||
       frame.riderResidencySerial_!=riderGpu->riderDemandSerial()||s.riderTextureBorrow!=riderGpu))
        return fail(d,VehicleWindowError50DS::material,"Rider complete-frame receipt session/publication/serial mismatch");
    if(frame.native_->draw.spanCount>originalUiMaximumSpans50KG||
       (frame.native_->draw.spanCount&&!frame.native_->draw.spans))
        return fail(d,VehicleWindowError50DS::frame,"bounded source span array is missing");
    bool hasGaragePrelit=false;
    for(std::uint32_t i=0;i<frame.native_->draw.spanCount;++i)
        if(frame.native_->draw.spans[i].materialMode==OriginalUiMaterialMode50KG::sourcePrelitCompressed)hasGaragePrelit=true;
    if(hasGaragePrelit&&!frame.garageCanonical_)
        return fail(d,VehicleWindowError50DS::material,"Garage PreLit requires its actual canonical pass transaction");
    if(frame.garageCanonical_&&!frame.garageCanonical_->preflight(frame,frame.owner_,frame.frameGeneration_,frame.native_,*this))
        return fail(d,VehicleWindowError50DS::material,"Garage canonical handles, source image bank or effective state rejected");
    if (!frame.owner_->beginSourceSubmission(frame.frameGeneration_,*this))
        return fail(d,VehicleWindowError50DS::frame,"original UI source binding-history receipt rejected");
    if(frame.garageCanonical_&&!frame.garageCanonical_->begin(frame,frame.owner_,frame.frameGeneration_,frame.native_,*this)){
        frame.owner_->finishSourceSubmission(frame.frameGeneration_,false,false);
        return fail(d,VehicleWindowError50DS::material,"Garage canonical submission changed before target work");
    }
    bool sourceWorkStarted=false;
    const auto sourceTargetBefore=s.uiSourceTargetGeneration;
    const auto submitDiagnosticStart=Storage::UiCoverageClock::now();
    s.menuRasterBackendActive50KG=s.menuRasterShared50KG!=nullptr;
    const bool accepted=submitOriginalUiImpl(*frame.native_,width,height,true,d,&sourceWorkStarted,publishSource,riderSparse,frame.directVideoPresent_?&frame.directVideo_:nullptr);
    s.menuRasterBackendActive50KG=false;
    if(s.menuRasterShared50KG){
        s.menuRasterSourceAccepted50KG=d.originalUiSourceAccepted;
        if(!d.originalUiSourceAccepted)s.menuRasterFailed50KG=true;
    }
    const auto submitDiagnosticNs=Storage::uiCoverageNs(submitDiagnosticStart);
    if(s.uiCoverageSourceDiagnostic&&submitDiagnosticNs>=100000000ull&&s.uiCoverageSlowTotalReports<8){
        ++s.uiCoverageSlowTotalReports;
        std::fprintf(stderr,"ui_coverage_slow_submit frame=%llu target_before=%llu target=%llu total_wall_ns=%llu accepted=%u source_accepted=%u scope=cpu_submit_including_diagnostic_logging_no_added_wait\n",
            (unsigned long long)frame.frameGeneration_,(unsigned long long)sourceTargetBefore,
            (unsigned long long)s.uiSourceTargetGeneration,(unsigned long long)submitDiagnosticNs,
            unsigned(accepted),unsigned(d.originalUiSourceAccepted));
    }
    const bool canonicalHistoryAccepted=!frame.garageCanonical_||frame.garageCanonical_->finish(
        frame.frameGeneration_,d.originalUiSourceAccepted,sourceWorkStarted);
    frame.owner_->finishSourceSubmission(frame.frameGeneration_,d.originalUiSourceAccepted,sourceWorkStarted);
    // A void SourceOwner callback can reject its history/Shared acceptance.
    // Retained native custody proves that rejection; do not report overall
    // success merely because the actual source pixels were accepted earlier.
    const bool menuRasterFinishAccepted=!d.originalUiSourceAccepted||!s.menuRasterShared50KG;
    d.originalUiSourceFrameGeneration=frame.frameGeneration_;
    d.originalUiSourceTargetGeneration=s.uiSourceTargetGeneration;
    // Keep the attempted owner borrowed after any potentially partial source
    // work. Only explicit successful retirement may release that lifetime.
    if (sourceWorkStarted || d.originalUiSourceAccepted || s.uiSourceTargetGeneration!=sourceTargetBefore)
        s.uiSourceFrameOwner=frame.owner_;
    // Only the actual successful complete source drain seals these values.
    // A later opaque copy failure does not undo a completed material bind.
    if (d.originalUiSourceAccepted){
        s.uiSourceFrameGeneration=frame.frameGeneration_;
        s.uiSourceAcceptedTargetGeneration=s.uiSourceTargetGeneration;
        s.uiSourceAcceptedTexturePublication=frame.native_->draw.texturePublication;
        s.uiSourceTerminal=frame.terminal_;
        for(unsigned i=0;i<4;++i)s.uiSourceClear[i]=frame.native_->draw.clearRgba[i];
    }
    if(!canonicalHistoryAccepted)
        return fail(d,VehicleWindowError50DS::material,"Garage canonical history indeterminate after actual source work");
    if(!menuRasterFinishAccepted)
        return fail(d,VehicleWindowError50DS::frame,"menu SourceOwner acceptance failed after actual source work");
    return accepted;
}
bool VehicleWindowGl50DS::matchesOriginalUiSource(const OriginalUiSourceSubmission50KN& receipt)const noexcept {
    if (!storage_)return false;const auto& s=*storage_;
    const auto lifetime=receipt.lifetime_.lock();
    return lifetime && lifetime==s.uiSourceReceiptLifetime && receipt.targetGeneration_ &&
        receipt.targetGeneration_==s.uiSourceTargetGeneration &&
        receipt.targetGeneration_==s.uiSourceAcceptedTargetGeneration &&
        receipt.frameGeneration_==s.uiSourceFrameGeneration &&
        receipt.texturePublication_==s.uiSourceAcceptedTexturePublication;
}
bool VehicleWindowGl50DS::originalUiSourceSubmission(OriginalUiSourceSubmission50KN& receipt,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    receipt={};d={};
    if (!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !s.uiSourceFrameOwner || !s.uiSourceFrameGeneration ||
        !s.uiSourceAcceptedTargetGeneration || s.uiSourceAcceptedTargetGeneration!=s.uiSourceTargetGeneration)
        return fail(d,VehicleWindowError50DS::frame,"no current accepted source drain for receipt");
    if(!s.ensureUiSourceReceiptLifetime(d))return false;
    receipt.lifetime_=s.uiSourceReceiptLifetime;receipt.frameGeneration_=s.uiSourceFrameGeneration;
    receipt.targetGeneration_=s.uiSourceAcceptedTargetGeneration;receipt.texturePublication_=s.uiSourceAcceptedTexturePublication;
    receipt.terminal_=s.uiSourceTerminal;s.uiSourceLeaseWork=true;
    d.originalUiSourceAccepted=true;d.originalUiSourceFrameGeneration=receipt.frameGeneration_;
    d.originalUiSourceTargetGeneration=receipt.targetGeneration_;return true;
}
bool VehicleWindowGl50DS::submitOriginalUiSourceUnpublished(const OriginalUiSourceFrame50KM& frame,
    std::int32_t width,std::int32_t height,OriginalUiSourceSubmission50KN& receipt,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    receipt={};d={};
    if (!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;
    if(!s.current(d) || !s.ensureUiSourceReceiptLifetime(d))return false;
    s.uiSourceLeaseWork=true;
    if(!submitOriginalUiSourceFrame(frame,width,height,false,d))return false;
    VehicleWindowDiagnostic50DS receiptDiagnostic;
    if(!originalUiSourceSubmission(receipt,receiptDiagnostic)){d=receiptDiagnostic;return false;}
    return true;
}
bool VehicleWindowGl50DS::completeOriginalUiSource(const OriginalUiSourceSubmission50KN& input,
    OriginalUiSourceSubmission50KN& completed,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(&input==&completed)return fail(d,VehicleWindowError50DS::request,"source completion input and output must be distinct");
    completed={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !matchesOriginalUiSource(input))
        return fail(d,VehicleWindowError50DS::frame,"source completion requires the exact current accepted target");
    s.diagnostic=&d;bool accepted=s.check("before explicit source completion");
    if(accepted){const auto finishStart=Storage::UiCoverageClock::now();
        s.gl.finish();accepted=s.check("complete exact source target work");
        if(s.uiCoverageSourceDiagnostic&&(s.uiCoverageCompletionReports<8||!accepted)){++s.uiCoverageCompletionReports;
            std::fprintf(stderr,"ui_coverage_completion target=%llu finish_wall_ns=%llu accepted=%u scope=existing_completion_wait\n",
                static_cast<unsigned long long>(s.uiSourceTargetGeneration),static_cast<unsigned long long>(Storage::uiCoverageNs(finishStart)),unsigned(accepted));}
    }
    s.diagnostic=nullptr;
    if(accepted){completed=input;completed.completed_=true;d.submittedWorkCompleted=true;
        d.originalUiSourceAccepted=true;d.originalUiSourceFrameGeneration=input.frameGeneration_;
        d.originalUiSourceTargetGeneration=input.targetGeneration_;}
    return accepted;
}
bool VehicleWindowGl50DS::publishOriginalUiSource(const OriginalUiSourceSubmission50KN& source,
    std::int32_t width,std::int32_t height,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !source.completed_ || !matchesOriginalUiSource(source) ||
        width<=0 || height<=0 || width>s.maximumWidth || height>s.maximumHeight ||
        s.uiSourcePublicationGeneration==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,"source publication requires exact completed target and bounded viewport");
    s.diagnostic=&d;
    bool accepted=s.check("before separate source publication") &&
        (s.request.output==VehicleWindowOutput50DS::headless || s.checkDefaultFramebuffer());
    if(accepted)accepted=s.publishUiSource(s.uiSourceClear,width,height);
    s.diagnostic=nullptr;
    d.originalUiSourceAccepted=true;d.originalUiSourceFrameGeneration=source.frameGeneration_;
    d.originalUiSourceTargetGeneration=source.targetGeneration_;
    return accepted;
}
bool VehicleWindowGl50DS::retainPublishedOriginalUiSource(OriginalUiRetainedSource50KN& retained,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    retained={};d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || s.uiRetainedTarget || s.uiRetainedColor ||
        !s.uiSourcePublishedTargetGeneration || s.uiSourcePublishedTargetGeneration!=s.uiSourceAcceptedTargetGeneration ||
        s.uiSourcePublishedTargetGeneration!=s.uiSourceTargetGeneration ||
        !s.imageCommitted || s.displayedTarget!=s.uiTargets[s.uiPublished] ||
        s.uiSnapshotSerial==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,"retain requires the current successful source publication and no prior snapshot");
    OriginalUiSourceSubmission50KN source;
    if(!originalUiSourceSubmission(source,d))return false;
    s.diagnostic=&d;bool accepted=s.check("before retaining renderer-published source RGBA");
    if(accepted)accepted=s.createRetainedUiSourceTarget();
    if(accepted){
        // Blit all four raw channels before any later ordinary source clear.
        // Scissor must not crop the source rightmost column or bottom row.
        ++s.uiSnapshotSerial;s.gl.disable(0x0C11);s.gl.disable(0x8DB9);
        s.gl.bindFramebuffer(readFramebuffer,s.uiSourceTarget);s.gl.readBuffer(colorAttachment);
        s.gl.bindFramebuffer(drawFramebuffer,s.uiRetainedTarget);s.gl.drawBuffer(colorAttachment);
        s.gl.blitFramebuffer(0,0,640,448,0,0,640,448,colorBufferBit,0x2600);
        accepted=s.check("copy immutable retained source RGBA");
    }
    if(accepted){s.gl.finish();accepted=s.check("complete retained source RGBA copy");}
    // Failed attempts keep every unresolved name; release/close owns retirement.
    // No partial snapshot or fabricated completion receipt is published.
    if(accepted){
        s.gl.bindFramebuffer(framebuffer,0);accepted=s.check("finish retained source copy bindings");
    }
    if(accepted){
        s.uiSnapshotGeneration=s.uiSnapshotSerial;s.uiRetainedSourceTargetGeneration=source.targetGeneration_;
        s.uiRetainedPublicationGeneration=s.uiSourcePublicationGeneration;
        source.completed_=true;retained.source_=source;retained.snapshotGeneration_=s.uiSnapshotGeneration;
        retained.publicationGeneration_=s.uiRetainedPublicationGeneration;
        d.submittedWorkCompleted=true;d.originalUiSnapshotGeneration=s.uiSnapshotGeneration;
        d.originalUiPublicationGeneration=s.uiRetainedPublicationGeneration;
    }
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::readRetainedOriginalUiSourceRgba(const OriginalUiRetainedSource50KN& retained,
    std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    const auto lifetime=retained.source_.lifetime_.lock();
    if(s.active || s.frameSet || s.diagnostic || !lifetime || lifetime!=s.uiSourceReceiptLifetime ||
        !retained.snapshotGeneration_ || retained.snapshotGeneration_!=s.uiSnapshotGeneration ||
        retained.publicationGeneration_!=s.uiRetainedPublicationGeneration ||
        retained.source_.targetGeneration_!=s.uiRetainedSourceTargetGeneration ||
        !s.uiRetainedTarget || !s.uiRetainedColor)
        return fail(d,VehicleWindowError50DS::frame,"retained source readback requires its exact live immutable snapshot");
    d.readbackWidth=640;d.readbackHeight=448;d.readbackBytes=std::size_t(640)*448*4;
    if(!rgba || capacityBytes<d.readbackBytes)
        return fail(d,VehicleWindowError50DS::readback,"retained RGBA readback requires full640x448 destination");
    s.diagnostic=&d;const bool accepted=s.copyRgba(rgba,s.uiRetainedTarget,640,448);s.diagnostic=nullptr;
    d.originalUiSnapshotGeneration=retained.snapshotGeneration_;return accepted;
}
bool VehicleWindowGl50DS::releaseRetainedOriginalUiSource(VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || s.uiBlendOldSnapshot)
        return fail(d,VehicleWindowError50DS::frame,"retained source release requires idle renderer without a blend borrow");
    if(!s.uiRetainedTarget && !s.uiRetainedColor)return true;
    s.diagnostic=&d;
    bool accepted=s.check("before retained source retirement");
    if(accepted){s.gl.finish();accepted=s.check("complete retained source work before deletion");}
    if(accepted){
        d.submittedWorkCompleted=true;s.uiSnapshotGeneration=0;
        s.gl.bindFramebuffer(framebuffer,0);s.gl.activeTexture(textureUnit0);s.gl.bindTexture(texture2D,0);
        accepted=s.check("unbind retained source before deletion");
        if(accepted)accepted=s.deleteName(s.uiRetainedTarget,s.gl.deleteFramebuffers,d) &&
            s.deleteName(s.uiRetainedColor,s.gl.deleteTextures,d);
        if(!accepted && d.error==VehicleWindowError50DS::none)
            fail(d,VehicleWindowError50DS::cleanup,"retained source deletion requires retry");
        if(accepted){s.uiRetainedSourceTargetGeneration=0;s.uiRetainedPublicationGeneration=0;}
    }
    s.diagnostic=nullptr;return accepted;
}
namespace {
bool exactUiByteBlendTerminal(const OriginalUiSourceTerminal50KN& terminal) noexcept {
    using F=assets::OriginalUiGsField50KG;
    const auto value=[&](F field) noexcept {return terminal.gs[static_cast<std::size_t>(field)];};
    // Authenticated Mat_Stbl first-match tokens3216/3217 are icon_tri/icon_x.
    // Locked Rider appearance draws only triangle (promptMask42C=2); both
    // prompts inherit the same ScreenMT state. Preserve every GS check below.
    // Read the real completed drain's owned fields, including inactive FIX and
    // clamp limits. Effect TEX1=0 is separate from this ordinary sampler state.
    return (terminal.canonicalToken==3216 || terminal.canonicalToken==3217) && terminal.pass==1 &&
        terminal.registryGeneration && terminal.bankGeneration && terminal.enabled==1 &&
        terminal.textureCount==1 && terminal.textureCoordinateIndex==0 &&
        value(F::environmentContext0or1)==0 &&
        value(F::inputColorA)==0 && value(F::inputColorB)==1 && value(F::inputAlphaC)==0 &&
        value(F::inputColorD)==1 && value(F::fixedAlphaValue)==128 &&
        value(F::wrapModeHorizontal)==1 && value(F::wrapModeVertical)==1 &&
        value(F::minClampParameterInUDirection)==0 && value(F::maxClampParameterInUDirection)==0 &&
        value(F::minClampParameterInVDirection)==0 && value(F::maxClampParameterInVDirection)==0 &&
        value(F::colorClampingMethod)==1 && value(F::alphaBlendingControlInUnitsOfPixels)==0 &&
        value(F::alphaCorrectionValue)==0;
}
}
bool VehicleWindowGl50DS::matchesOriginalUiByteBlend(const OriginalUiByteBlend50KO& blend)const noexcept {
    if(!storage_)return false;const auto& s=*storage_;
    const auto oldLifetime=blend.old_.source_.lifetime_.lock();
    const auto incomingLifetime=blend.incoming_.lifetime_.lock();
    return oldLifetime && oldLifetime==s.uiSourceReceiptLifetime && incomingLifetime==oldLifetime &&
        !s.uiBlendPoisoned && blend.generation_ && blend.generation_==s.uiBlendGeneration &&
        blend.old_.snapshotGeneration_==s.uiBlendOldSnapshot && s.uiBlendOldSnapshot==s.uiSnapshotGeneration &&
        blend.old_.publicationGeneration_==s.uiRetainedPublicationGeneration &&
        blend.old_.source_.targetGeneration_==s.uiRetainedSourceTargetGeneration &&
        blend.incoming_.targetGeneration_==s.uiBlendIncomingSource &&
        s.uiRetainedTarget && s.uiRetainedColor && s.uiBlendIncomingTarget && s.uiBlendIncomingColor &&
        s.uiBlendEffectTarget && s.uiBlendEffectColor && s.uiBlendProgram && s.uiBlendVertexArray;
}
bool VehicleWindowGl50DS::matchesOriginalUiByteBlendSample(const OriginalUiByteBlendSample50KO& sample)const noexcept {
    if(!storage_)return false;const auto& s=*storage_;
    const auto lifetime=sample.lifetime_.lock();
    return lifetime && lifetime==s.uiSourceReceiptLifetime && !s.uiBlendPoisoned &&
        sample.blendGeneration_ && sample.blendGeneration_==s.uiBlendGeneration &&
        sample.sampleGeneration_ && sample.sampleGeneration_==s.uiBlendSampleGeneration &&
        sample.alpha_==s.uiBlendAlpha && s.uiBlendOldSnapshot &&
        s.uiBlendOldSnapshot==s.uiSnapshotGeneration && s.uiBlendEffectTarget && s.uiBlendEffectColor;
}
bool VehicleWindowGl50DS::beginOriginalUiByteBlend(const OriginalUiRetainedSource50KN& oldSource,
    const OriginalUiSourceSubmission50KN& incoming,OriginalUiByteBlend50KO& blend,
    VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(&oldSource==&blend.old_ || &incoming==&blend.incoming_ || &incoming==&blend.old_.source_)
        return fail(d,VehicleWindowError50DS::request,"byte-blend input and output storage must be distinct");
    blend={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    const auto oldLifetime=oldSource.source_.lifetime_.lock();
    if(s.active || s.frameSet || s.diagnostic || s.uiBlendOldSnapshot || s.hasUiByteBlendNames() ||
        !oldLifetime || oldLifetime!=s.uiSourceReceiptLifetime || !oldSource.source_.completed_ ||
        !oldSource.snapshotGeneration_ || oldSource.snapshotGeneration_!=s.uiSnapshotGeneration ||
        oldSource.publicationGeneration_!=s.uiRetainedPublicationGeneration ||
        oldSource.source_.targetGeneration_!=s.uiRetainedSourceTargetGeneration ||
        !s.uiRetainedTarget || !s.uiRetainedColor || !incoming.completed_ || !matchesOriginalUiSource(incoming) ||
        incoming.targetGeneration_<=oldSource.source_.targetGeneration_ ||
        s.uiBlendSerial==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,"byte-blend requires live retained old and newer exact completed incoming");
    if(!exactUiByteBlendTerminal(incoming.terminal_))
        return fail(d,VehicleWindowError50DS::material,"byte-blend requires actual terminal icon_tri/icon_x pass1 and exact inherited GS state");
    s.diagnostic=&d;
    bool accepted=s.check("before immutable byte-blend pair creation");
    if(accepted){
        // This borrow is installed BEFORE any fallible allocation/copy. It
        // survives failure and cannot be replaced until explicit retirement.
        s.uiBlendOldSnapshot=oldSource.snapshotGeneration_;s.uiBlendPoisoned=true;s.uiSourceLeaseWork=true;
        accepted=s.createUiByteBlendTarget(s.uiBlendIncomingTarget,s.uiBlendIncomingColor) &&
            s.createUiByteBlendTarget(s.uiBlendEffectTarget,s.uiBlendEffectColor) && s.createUiByteBlendProgram();
        if(accepted){
            s.gl.disable(0x0C11);s.gl.disable(0x8DB9);
            s.gl.bindFramebuffer(readFramebuffer,s.uiSourceTarget);s.gl.readBuffer(colorAttachment);
            s.gl.bindFramebuffer(drawFramebuffer,s.uiBlendIncomingTarget);s.gl.drawBuffer(colorAttachment);
            s.gl.blitFramebuffer(0,0,640,448,0,0,640,448,colorBufferBit,0x2600);
            accepted=s.check("copy actual completed incoming into private immutable byte-blend image");
        }
        if(accepted){s.gl.finish();accepted=s.check("complete immutable incoming byte-blend copy");}
        const bool restored=s.restoreUiByteBlendBindings();accepted=accepted && restored;
    }
    if(accepted){
        s.uiBlendIncomingSource=incoming.targetGeneration_;s.uiBlendGeneration=++s.uiBlendSerial;
        s.uiBlendSampleGeneration=0;s.uiBlendPoisoned=false;
        for(unsigned i=0;i<4;++i)s.uiBlendClear[i]=s.uiSourceClear[i];
        blend.old_=oldSource;blend.incoming_=incoming;blend.generation_=s.uiBlendGeneration;
        d.submittedWorkCompleted=true;d.originalUiByteBlendGeneration=s.uiBlendGeneration;
    }
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::renderOriginalUiByteBlendSample(const OriginalUiByteBlend50KO& blend,
    std::uint32_t alpha,OriginalUiByteBlendSample50KO& sample,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    sample={};d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || alpha>OriginalUiByteBlendContract50KO::maximumAlpha ||
        !matchesOriginalUiByteBlend(blend) || s.uiBlendSampleSerial==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,"byte-blend sample requires exact live immutable pair and alpha0..128");
    // Invalidate previous sample authority before any attempted write. Failed
    // samples poison the pair until real retirement; never publish partial E.
    s.uiBlendSampleGeneration=0;s.diagnostic=&d;
    bool accepted=s.check("before byte-blend sample");
    if(accepted){
        const auto generation=++s.uiBlendSampleSerial;
        s.gl.bindFramebuffer(framebuffer,s.uiBlendEffectTarget);s.gl.drawBuffer(colorAttachment);s.gl.readBuffer(colorAttachment);
        if(s.gl.clipControl)s.gl.clipControl(0x8CA1,0x935E);
        s.gl.viewport(0,0,640,448);
        constexpr U disabled[]={0x0BE2,0x0B44,0x0C11,0x0B90,0x8037,0x0B20,
            0x8F9D,0x0BD0,0x809D,0x80A0,0x809E,0x809F,0x8E51,0x8C89,0x8DB9,0x864F,0x0BF2,0x0B71};
        for(U capability:disabled)s.gl.disable(capability);
        for(I i=0;i<s.clipDistances;++i)s.gl.disable(0x3000+static_cast<U>(i));
        s.gl.colorMask(1,1,1,1);s.gl.depthMask(0);s.gl.polygonMode(0x0408,0x1B02);
        s.resetNativeGcLighting();
        s.gl.useProgram(s.uiBlendProgram);s.drawGl.bindVertexArray(s.uiBlendVertexArray);
        s.gl.activeTexture(textureUnit0);s.gl.bindTexture(texture2D,s.uiRetainedColor);
        if(s.gl.bindSampler)s.gl.bindSampler(0,0);
        s.gl.activeTexture(textureUnit0+1);s.gl.bindTexture(texture2D,s.uiBlendIncomingColor);
        if(s.gl.bindSampler)s.gl.bindSampler(1,0);
        s.gl.uniform1i(s.uiBlendOldUniform,0);s.gl.uniform1i(s.uiBlendIncomingUniform,1);
        s.gl.uniform1i(s.uiBlendAlphaUniform,static_cast<I>(alpha));
        accepted=s.check("bind immutable inputs and exact byte-blend state");
        if(accepted){s.gl.drawArrays(0x0004,0,3);accepted=s.check("draw documented byte-blend sample");}
        const bool restored=s.restoreUiByteBlendBindings();accepted=accepted && restored;
        if(accepted){
            s.uiBlendSampleGeneration=generation;s.uiBlendAlpha=alpha;
            sample.lifetime_=s.uiSourceReceiptLifetime;sample.blendGeneration_=s.uiBlendGeneration;
            sample.sampleGeneration_=generation;sample.alpha_=alpha;
            d.originalUiByteBlendAccepted=true;d.originalUiByteBlendGeneration=s.uiBlendGeneration;
            d.originalUiByteBlendSampleGeneration=generation;
        }
    }
    if(!accepted)s.uiBlendPoisoned=true;
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::completeOriginalUiByteBlendSample(const OriginalUiByteBlendSample50KO& input,
    OriginalUiByteBlendSample50KO& completed,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(&input==&completed)return fail(d,VehicleWindowError50DS::request,"byte-blend completion input and output must be distinct");
    completed={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !matchesOriginalUiByteBlendSample(input))
        return fail(d,VehicleWindowError50DS::frame,"byte-blend completion requires exact accepted sample");
    s.diagnostic=&d;bool accepted=s.check("before byte-blend sample completion");
    if(accepted){s.gl.finish();accepted=s.check("complete byte-blend sample GPU work");}
    if(accepted){
        completed=input;completed.completed_=true;d.submittedWorkCompleted=true;d.originalUiByteBlendAccepted=true;
        d.originalUiByteBlendGeneration=input.blendGeneration_;d.originalUiByteBlendSampleGeneration=input.sampleGeneration_;
    }else{s.uiBlendPoisoned=true;s.uiBlendSampleGeneration=0;}
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::readOriginalUiByteBlendSampleRgba(const OriginalUiByteBlendSample50KO& sample,
    std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !sample.completed_ || !matchesOriginalUiByteBlendSample(sample))
        return fail(d,VehicleWindowError50DS::frame,"byte-blend readback requires exact current completed sample");
    d.readbackWidth=640;d.readbackHeight=448;d.readbackBytes=std::size_t(640)*448*4;
    if(!rgba || capacityBytes<d.readbackBytes)
        return fail(d,VehicleWindowError50DS::readback,"byte-blend readback requires full640x448 RGBA destination");
    s.diagnostic=&d;const bool accepted=s.copyRgba(rgba,s.uiBlendEffectTarget,640,448);s.diagnostic=nullptr;
    d.originalUiByteBlendGeneration=sample.blendGeneration_;d.originalUiByteBlendSampleGeneration=sample.sampleGeneration_;
    return accepted;
}
bool VehicleWindowGl50DS::matchesOriginalUiPrivatePublication(const OriginalUiPrivatePublication50KP& publication)const noexcept {
    if(!storage_)return false;const auto& s=*storage_;
    const auto lifetime=publication.lifetime_.lock();
    return lifetime && lifetime==s.uiSourceReceiptLifetime && publication.generation_ &&
        publication.generation_==s.uiPrivatePublicationGeneration && s.imageCommitted &&
        s.displayedTarget==s.uiTargets[s.uiPublished] &&
        publication.width_==s.committedWidth && publication.height_==s.committedHeight;
}
bool VehicleWindowGl50DS::publishOriginalUiBlendImage(const OriginalUiByteBlend50KO& blend,
    const OriginalUiByteBlendSample50KO* sample,std::int32_t width,std::int32_t height,
    OriginalUiPrivatePublication50KP& publication,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    publication={};d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !matchesOriginalUiByteBlend(blend) ||
        width<=0 || height<=0 || width>s.maximumWidth || height>s.maximumHeight ||
        s.uiPrivatePublicationSerial==std::numeric_limits<std::uint64_t>::max() ||
        (sample && (!sample->completed_ || !matchesOriginalUiByteBlendSample(*sample))) ||
        (!sample && (s.uiPromotedIncomingTarget || s.uiPromotedIncomingColor)))
        return fail(d,VehicleWindowError50DS::frame,"private publication requires live immutable pair, completed sample or unowned promotion, and bounded viewport");
    const auto pending=static_cast<std::uint8_t>(s.uiPublished^1u);
    s.diagnostic=&d;
    bool accepted=s.check("before private immutable-image publication");
    if(accepted)accepted=s.copyUiSourceToOpaque(sample?s.uiBlendEffectTarget:s.uiBlendIncomingTarget,
        s.uiBlendClear,width,height);
    // The pending surface is independent of the last committed image. Failed
    // copies neither transfer incoming ownership nor invalidate that prior
    // image. Inputs remain immutable and an explicit retry can overwrite the
    // whole pending surface. No default framebuffer is checked or written.
    if(accepted){
        s.uiPublished=pending;s.displayedTarget=s.uiTargets[pending];
        s.width=s.committedWidth=width;s.height=s.committedHeight=height;s.imageCommitted=true;
        s.uiSourcePublishedTargetGeneration=0;
        s.uiPrivatePublicationGeneration=++s.uiPrivatePublicationSerial;
        publication.lifetime_=s.uiSourceReceiptLifetime;
        publication.kind_=sample?OriginalUiPrivatePublicationKind50KP::byteBlendSample:
            OriginalUiPrivatePublicationKind50KP::immutableIncoming;
        publication.generation_=s.uiPrivatePublicationGeneration;publication.blendGeneration_=blend.generation_;
        publication.oldSnapshotGeneration_=blend.old_.snapshotGeneration_;
        publication.incomingFrameGeneration_=blend.incoming_.frameGeneration_;
        publication.incomingTargetGeneration_=blend.incoming_.targetGeneration_;
        publication.width_=width;publication.height_=height;
        if(sample){publication.sampleGeneration_=sample->sampleGeneration_;publication.alpha_=sample->alpha_;}
        d.originalUiPrivatePublicationAccepted=true;d.originalUiPrivatePublicationGeneration=publication.generation_;
        d.originalUiByteBlendGeneration=publication.blendGeneration_;
        d.originalUiByteBlendSampleGeneration=publication.sampleGeneration_;
    }
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::publishOriginalUiByteBlendSample(const OriginalUiByteBlend50KO& blend,
    const OriginalUiByteBlendSample50KO& sample,std::int32_t width,std::int32_t height,
    OriginalUiPrivatePublication50KP& publication,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    return publishOriginalUiBlendImage(blend,&sample,width,height,publication,d);
}
bool VehicleWindowGl50DS::promoteOriginalUiByteBlendIncoming(const OriginalUiByteBlend50KO& blend,
    std::int32_t width,std::int32_t height,OriginalUiPrivatePublication50KP& publication,
    OriginalUiPromotedIncoming50KP& incoming,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    incoming={};
    if(!publishOriginalUiBlendImage(blend,nullptr,width,height,publication,d))return false;
    auto& s=*storage_;
    // Nothing fallible follows the successful private copy: this is a CPU
    // ownership move of the already completed immutable incoming names, not a
    // new GPU copy, source submission, completion barrier or host presentation.
    s.uiPromotedIncomingTarget=s.uiBlendIncomingTarget;s.uiBlendIncomingTarget=0;
    s.uiPromotedIncomingColor=s.uiBlendIncomingColor;s.uiBlendIncomingColor=0;
    s.uiPromotedIncomingGeneration=blend.generation_;
    incoming.source_=blend.incoming_;incoming.generation_=s.uiPromotedIncomingGeneration;
    // No sample or promotion retry is legal after transfer. The old borrow and
    // remaining effect objects stay owned until explicit blend retirement.
    s.uiBlendGeneration=s.uiBlendSampleGeneration=0;s.uiBlendPoisoned=true;
    d.originalUiPromotedIncomingGeneration=incoming.generation_;return true;
}
bool VehicleWindowGl50DS::completeOriginalUiPrivatePublication(const OriginalUiPrivatePublication50KP& input,
    OriginalUiPrivatePublication50KP& completed,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(&input==&completed)return fail(d,VehicleWindowError50DS::request,"publication completion input and output must be distinct");
    completed={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !matchesOriginalUiPrivatePublication(input))
        return fail(d,VehicleWindowError50DS::frame,"publication completion requires exact current private image");
    s.diagnostic=&d;bool accepted=s.check("before private publication completion");
    if(accepted){s.gl.finish();accepted=s.check("complete private opaque publication GPU work");}
    if(accepted){
        completed=input;completed.completed_=true;d.submittedWorkCompleted=true;
        d.originalUiPrivatePublicationAccepted=true;d.originalUiPrivatePublicationGeneration=input.generation_;
        d.originalUiByteBlendGeneration=input.blendGeneration_;
        d.originalUiByteBlendSampleGeneration=input.sampleGeneration_;
    }else s.uiPrivatePublicationGeneration=0;
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::copyOriginalUiPrivatePublicationToWindow(const OriginalUiPrivatePublication50KP& publication,
    OriginalUiWindowBackBufferCopy& copied,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(&publication==&copied.publication_)
        return fail(d,VehicleWindowError50DS::request,"window copy input and output must be distinct");
    copied={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic ||
        s.request.output!=VehicleWindowOutput50DS::defaultBackBuffer ||
        !publication.completed_ || !matchesOriginalUiPrivatePublication(publication) ||
        publication.width_<=0 || publication.height_<=0 ||
        publication.width_>s.maximumWidth || publication.height_>s.maximumHeight ||
        s.uiWindowBackBufferCopySerial==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,
            "window copy requires idle window renderer, exact completed private image, bounded viewport and copy sequence");
    s.diagnostic=&d;
    bool accepted=s.check("before private publication window copy") && s.checkDefaultFramebuffer();
    if(accepted){
        // Working frame dimensions may differ after a failed later submit.
        // Copy only the authenticated selected image's exact recorded extent,
        // without changing working dimensions or any private image ownership.
        // The helper checks restoration of framebuffer0 before it succeeds.
        accepted=s.copyToWindow(publication.width_,publication.height_);
        if(!accepted && d.error==VehicleWindowError50DS::gl)d.error=VehicleWindowError50DS::presentation;
    }
    if(accepted){
        copied.lifetime_=s.uiSourceReceiptLifetime;copied.publication_=publication;
        copied.sequence_=++s.uiWindowBackBufferCopySerial;
        copied.kind_=OriginalUiWindowBackBufferCopyKind::windowBack;copied.commandsAccepted_=true;
    }
    // Failed blits preserve the selected private image and its completion.
    // The helper retains possible partial BACK modification in d; no receipt
    // or completion fence is issued and this renderer never swaps the window.
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::readOriginalUiPrivatePublicationRgba(const OriginalUiPrivatePublication50KP& publication,
    std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !publication.completed_ || !matchesOriginalUiPrivatePublication(publication))
        return fail(d,VehicleWindowError50DS::frame,"private readback requires exact current completed publication");
    d.readbackWidth=publication.width_;d.readbackHeight=publication.height_;
    d.readbackBytes=std::size_t(publication.width_)*std::size_t(publication.height_)*4;
    if(!rgba || capacityBytes<d.readbackBytes)
        return fail(d,VehicleWindowError50DS::readback,"private publication readback requires full viewport RGBA destination");
    s.diagnostic=&d;const bool accepted=s.copyRgba(rgba,s.displayedTarget,publication.width_,publication.height_);
    s.diagnostic=nullptr;d.originalUiPrivatePublicationGeneration=publication.generation_;
    d.originalUiByteBlendGeneration=publication.blendGeneration_;
    d.originalUiByteBlendSampleGeneration=publication.sampleGeneration_;return accepted;
}
bool VehicleWindowGl50DS::readPromotedOriginalUiIncomingRgba(const OriginalUiPromotedIncoming50KP& incoming,
    std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    const auto lifetime=incoming.source_.lifetime_.lock();
    if(s.active || s.frameSet || s.diagnostic || !lifetime || lifetime!=s.uiSourceReceiptLifetime ||
        !incoming.generation_ || incoming.generation_!=s.uiPromotedIncomingGeneration ||
        !s.uiPromotedIncomingTarget || !s.uiPromotedIncomingColor)
        return fail(d,VehicleWindowError50DS::frame,"promoted incoming readback requires its exact immutable raw owner");
    d.readbackWidth=640;d.readbackHeight=448;d.readbackBytes=std::size_t(640)*448*4;
    if(!rgba || capacityBytes<d.readbackBytes)
        return fail(d,VehicleWindowError50DS::readback,"promoted incoming readback requires full640x448 RGBA destination");
    s.diagnostic=&d;const bool accepted=s.copyRgba(rgba,s.uiPromotedIncomingTarget,640,448);s.diagnostic=nullptr;
    d.originalUiPromotedIncomingGeneration=incoming.generation_;return accepted;
}
bool VehicleWindowGl50DS::releasePromotedOriginalUiIncoming(VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"promoted incoming retirement requires idle renderer");
    if(!s.uiPromotedIncomingTarget && !s.uiPromotedIncomingColor)return true;
    s.diagnostic=&d;bool accepted=s.check("before promoted incoming retirement");
    if(accepted){s.gl.finish();accepted=s.check("complete promoted incoming reads before retirement");}
    if(accepted){
        d.submittedWorkCompleted=true;s.uiPromotedIncomingGeneration=0;
        accepted=s.restoreUiByteBlendBindings();
        if(accepted)accepted=s.deleteName(s.uiPromotedIncomingTarget,s.gl.deleteFramebuffers,d) &&
            s.deleteName(s.uiPromotedIncomingColor,s.gl.deleteTextures,d);
        if(!accepted && d.error==VehicleWindowError50DS::none)
            fail(d,VehicleWindowError50DS::cleanup,"promoted incoming name retirement requires retry");
    }
    if(!accepted)d.cleanupIncomplete=true;
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::releaseOriginalUiByteBlend(VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"byte-blend retirement requires idle renderer");
    if(!s.uiBlendOldSnapshot && !s.hasUiByteBlendNames())return true;
    s.diagnostic=&d;bool accepted=s.check("before byte-blend retirement");
    if(accepted){s.gl.finish();accepted=s.check("complete all byte-blend work before retirement");}
    if(accepted){
        d.submittedWorkCompleted=true;s.uiBlendGeneration=s.uiBlendSampleGeneration=0;s.uiBlendPoisoned=true;
        accepted=s.restoreUiByteBlendBindings();
        if(accepted)accepted=s.deleteUiByteBlendNames(d);
        if(!accepted && d.error==VehicleWindowError50DS::none)
            fail(d,VehicleWindowError50DS::cleanup,"byte-blend name retirement requires retry");
        if(accepted){s.uiBlendOldSnapshot=s.uiBlendIncomingSource=0;s.uiBlendPoisoned=false;}
    }
    if(!accepted)d.cleanupIncomplete=true;
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::applyMenuRasterPrelude50KG(OriginalUiSharedDialogs50KG& shared,
    VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!storage_||!shared.menuRasterAttempted_||shared.menuRasterFailed_||
       !shared.menu470RenderCurrent()||shared.menu470Phase_!=decltype(shared.menu470Phase_)::callbacks||
       shared.menu470Slot_!=0x48||shared.menu470Callback_!=&shared.dialog470_.entity||
       !shared.menuDisplay_||sessionDisplayOwner50LA_!=shared.menuDisplay_.get())
        return fail(d,VehicleWindowError50DS::frame,"menu raster lacks actual Shared Session/display invocation");
    auto& s=*storage_;
    if(s.menuRasterShared50KG)
        return fail(d,VehicleWindowError50DS::frame,"another or failed menu raster lease is still pending");
    // Capture exact attempted ownership before the first fallible GL operation.
    s.menuRasterShared50KG=&shared;s.menuRasterOwner50KG=shared.menu470Owner();
    s.menuRasterInputs50KG=shared.menuRaster_;s.menuRasterEpoch50KG=shared.menu470Epoch_;
    s.menuRasterInvocation50KG=shared.menu470Invocation_;s.menuRasterDisplay50KG=shared.menuDisplay_->generation();
    s.menuRasterTextureEpoch50KG=s.uiTextureEpoch;s.menuRasterTargetGeneration50KG=s.uiSourceTargetGeneration;
    s.menuRasterTarget50KG=s.uiSourceTarget;s.menuRasterColor50KG=s.uiSourceColor;s.menuRasterDepth50KG=s.uiSourceDepth;
    s.menuRasterRetiredShared50KG=nullptr;s.menuRasterRetiredOwner50KG=nullptr;
    s.menuRasterRetiredEpoch50KG=0;s.menuRasterRetiredInvocation50KG=0;
    const auto reject=[&]()noexcept {s.menuRasterFailed50KG=true;return false;};
    OriginalUiMenuRasterInputs50KG observed;
    if(!shared.readMenu470RasterInputs(observed)||!sameOriginalUiMenuRasterInputs50KG(observed,s.menuRasterInputs50KG)||
       !originalUiSourceRgbaAvailable()||std::fegetround()!=FE_TONEAREST||!s.current(d,nullptr,&shared)||!s.frontendOnly||
       s.active||s.frameSet||s.diagnostic||s.nativeGlFaultObserved50KG||s.garagePreludeOwner50KG||
       s.garagePollFence50KG||s.uiBlendOldSnapshot||s.uiBlendIncomingSource||
       (s.uiSourceFrameOwner&&s.uiSourceFrameOwner!=s.menuRasterOwner50KG)||
       !s.uiProgram||s.uiCanvasUniform<0||s.request.originalUiCpuTextures!=observed.cpu||
       s.request.originalUiTextures!=observed.gpu||s.uiTexturePublication!=observed.publication||
       !observed.gpu||!observed.gpu->matchesSourceBank(*observed.cpu,observed.publication)||
       !observed.gpu->validBorrow(s.request.context,s.request.isCurrent,observed.publication)){
        if(d.error==VehicleWindowError50DS::none)fail(d,VehicleWindowError50DS::frame,"menu raster owner/bank/target is not idle and current");
        return reject();
    }
    s.diagnostic=&d;
    bool okay=s.check("before native menu raster prelude");
    if(okay){
        // Exact existing full-source native contract. This writes no pixels,
        // target generation, clear, material history or source acceptance flag.
        s.gl.bindFramebuffer(framebuffer,s.uiSourceTarget);
        s.gl.drawBuffer(colorAttachment);s.gl.readBuffer(colorAttachment);
        if(s.gl.clipControl)s.gl.clipControl(0x8CA1,0x935E);
        s.gl.viewport(0,0,640,448);
        s.gl.scissor(0,0,640,448);s.gl.enable(0x0C11);
        s.gl.useProgram(s.uiProgram);
        s.gl.uniform4f(s.uiCanvasUniform,float(2.0/640.0),float(-2.0/448.0),-1.f,1.f);
        okay=s.check("apply native menu source target viewport and canvas");
    }
    s.diagnostic=nullptr;
    if(!okay)return reject();
    s.menuRasterApplied50KG=true;return true;
}
bool VehicleWindowGl50DS::menuRasterPreludeCurrent50KG(const OriginalUiSharedDialogs50KG& shared)const noexcept {
    if(!storage_||!shared.menu470Current()||!shared.menuRasterAttempted_||shared.menuRasterFailed_||
       !shared.menuDisplay_||sessionDisplayOwner50LA_!=shared.menuDisplay_.get())return false;
    const auto& s=*storage_;VehicleWindowDiagnostic50DS observed;
    return s.menuRasterShared50KG==&shared&&s.menuRasterOwner50KG==shared.menu470Owner()&&
        s.menuRasterEpoch50KG==shared.menu470Epoch_&&s.menuRasterInvocation50KG==shared.menu470Invocation_&&
        s.menuRasterDisplay50KG==shared.menuDisplay_->generation()&&
        sameOriginalUiMenuRasterInputs50KG(s.menuRasterInputs50KG,shared.menuRaster_)&&
        s.menuRasterApplied50KG&&!s.menuRasterFailed50KG&&!s.menuRasterBackendActive50KG&&std::fegetround()==FE_TONEAREST&&
        !s.nativeGlFaultObserved50KG&&!s.active&&!s.frameSet&&!s.diagnostic&&!s.garagePreludeOwner50KG&&
        s.current(observed,nullptr,&shared)&&s.uiSourceTarget==s.menuRasterTarget50KG&&
        s.uiSourceColor==s.menuRasterColor50KG&&s.uiSourceDepth==s.menuRasterDepth50KG&&
        s.uiSourceTargetGeneration==s.menuRasterTargetGeneration50KG&&s.uiTextureEpoch==s.menuRasterTextureEpoch50KG&&
        s.request.originalUiCpuTextures==s.menuRasterInputs50KG.cpu&&s.request.originalUiTextures==s.menuRasterInputs50KG.gpu&&
        s.uiTexturePublication==s.menuRasterInputs50KG.publication&&
        s.menuRasterInputs50KG.gpu->matchesSourceBank(*s.menuRasterInputs50KG.cpu,s.menuRasterInputs50KG.publication)&&
        s.menuRasterInputs50KG.gpu->validBorrow(s.request.context,s.request.isCurrent,s.menuRasterInputs50KG.publication);
}
bool VehicleWindowGl50DS::sealMenuRasterSourceFrame50KG(const OriginalUiSharedDialogs50KG& shared,
    VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!menuRasterPreludeCurrent50KG(shared))
        return fail(d,VehicleWindowError50DS::frame,"menu raster changed before SourceFrame seal");
    auto& s=*storage_;const auto* native=shared.menuRasterNative_;
    if(s.menuRasterFrame50KG||s.menuRasterNative50KG||!shared.menuRasterFrame_||!native||
       native->draw.canvasWidth!=float(s.menuRasterInputs50KG.viewport[2])||
       native->draw.canvasHeight!=float(s.menuRasterInputs50KG.viewport[3])||
       native->draw.textures!=s.menuRasterInputs50KG.gpu||native->draw.texturePublication!=s.menuRasterInputs50KG.publication||
       native->opaquePresentationAlphaOnly||native->draw.retainRaceImage){
        s.menuRasterFailed50KG=true;
        return fail(d,VehicleWindowError50DS::frame,"sealed menu draw is not the applied native source contract");}
    s.menuRasterFrame50KG=shared.menuRasterFrame_;s.menuRasterNative50KG=native;return true;
}
bool VehicleWindowGl50DS::acceptMenuRasterSource50KG(const OriginalUiSharedDialogs50KG& shared)noexcept {
    if(!storage_)return false;auto& s=*storage_;VehicleWindowDiagnostic50DS observed;
    if(s.menuRasterShared50KG!=&shared||s.menuRasterOwner50KG!=shared.menu470Owner()||
       s.menuRasterEpoch50KG!=shared.menu470Epoch_||s.menuRasterInvocation50KG!=shared.menu470Invocation_||
       shared.menu470Phase_!=decltype(shared.menu470Phase_)::submitting||
       !s.menuRasterSourceAccepted50KG||s.menuRasterFailed50KG||s.menuRasterBackendActive50KG||
       s.menuRasterFrame50KG!=shared.menuRasterFrame_||s.menuRasterFrame50KG!=shared.menu470Frame_||
       s.menuRasterNative50KG!=shared.menuRasterNative_||!s.menuRasterFrame50KG||
       s.menuRasterTargetGeneration50KG==std::numeric_limits<std::uint64_t>::max()||
       s.uiSourceTargetGeneration!=s.menuRasterTargetGeneration50KG+1u||
       !s.current(observed,nullptr,&shared))return false;
    // SourceOwner completion has observed the actual backend accepted result.
    // A later publication/window failure does not retroactively erase it.
    s.clearMenuRasterLease50KG();return true;
}
bool VehicleWindowGl50DS::menuRasterRetired50KG(const OriginalUiSharedDialogs50KG& shared)const noexcept {
    return storage_&&!storage_->menuRasterShared50KG&&
        storage_->menuRasterRetiredShared50KG==&shared&&storage_->menuRasterRetiredOwner50KG==shared.menu470Owner()&&
        storage_->menuRasterRetiredEpoch50KG==shared.menu470Epoch_&&
        storage_->menuRasterRetiredInvocation50KG==shared.menu470Invocation_;
}

bool VehicleWindowGl50DS::applyGarageFogColor244A40(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t publication,std::uint32_t argb,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!originalUiSourceRgbaAvailable()||!ownsGarageSourcePrelude50KG(owner,publication))
        return fail(d,VehicleWindowError50DS::context,"Garage Fog command requires exact live source-target owner");
    auto& s=*storage_;
    if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG||
       s.nativeGlFaultObserved50KG||!s.uiProgram||s.uiParticleFogColorUniform<0)
        return fail(d,VehicleWindowError50DS::frame,"Garage Fog command requires current idle source renderer");
    s.diagnostic=&d;s.gl.useProgram(s.uiProgram);
    //Native equivalent of the single13EBBC GSFOGCOL write. Apply the actual
    //retained source-RGBA shader state in command order; do not draw/present.
    s.gl.uniform4f(s.uiParticleFogColorUniform,float((argb>>16)&255u)/255.f,
        float((argb>>8)&255u)/255.f,float(argb&255u)/255.f,1.f);
    const bool accepted=s.check("apply actual Garage Fog GSFOGCOL command");
    s.diagnostic=nullptr;if(!accepted)s.garagePreludeFailed50KG=true;return accepted;
}
bool VehicleWindowGl50DS::clearGarageSourceViewport50KG(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t publication,bool color,std::uint32_t argb,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!publication||!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"Garage source clear requires actual source target and publication");
    auto& s=*storage_;if(!s.current(d))return false;
    if(!s.frontendOnly||s.active||s.frameSet||s.diagnostic||s.nativeGlFaultObserved50KG||
       s.garagePreludeFailed50KG||(s.garagePreludeOwner50KG&&
       (s.garagePreludeOwner50KG!=&owner||s.garagePreludePublication50KG!=publication))||
       (s.uiSourceFrameOwner&&s.uiSourceFrameOwner!=&owner))
        return fail(d,VehicleWindowError50DS::frame,"Garage source clear requires idle exact source-target ownership");
    const bool first=!s.garagePreludeOwner50KG;
    if(first&&s.uiSourceTargetGeneration==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,"Garage source target generation exhausted");
    s.garagePreludeOwner50KG=&owner;s.garagePreludePublication50KG=publication;
    if(first)++s.uiSourceTargetGeneration;
    s.uiSourceAcceptedTargetGeneration=0;s.uiSourcePublishedTargetGeneration=0;
    s.diagnostic=&d;
    //Actual full640x448 source viewport.2458C0's sprite has ATE=NEVER,
    //AFAIL=ZB_ONLY,Z=0. This real D24 clear has no color/image-publication effect.
    s.gl.bindFramebuffer(framebuffer,s.uiSourceTarget);s.gl.viewport(0,0,640,448);
    s.gl.disable(0x0C11);s.gl.depthMask(1);s.gl.clearDepth(0.0);
    U mask=depthBufferBit;
    if(color){
        s.gl.colorMask(1,1,1,1);
        //25C0A0 emits alpha128 regardless of the incoming ARGB high byte.
        s.gl.clearColor(float((argb>>16)&255u)/255.f,float((argb>>8)&255u)/255.f,
            float(argb&255u)/255.f,128.f/255.f);mask|=colorBufferBit;
    }
    s.gl.clear(mask);
    const bool accepted=s.check("apply actual Garage full-viewport source clear");
    if(accepted&&color){s.uiSourceClear[0]=float((argb>>16)&255u)/255.f;
        s.uiSourceClear[1]=float((argb>>8)&255u)/255.f;s.uiSourceClear[2]=float(argb&255u)/255.f;
        s.uiSourceClear[3]=1.f;} //Actual clear provenance for later opaque target copy.
    s.diagnostic=nullptr;s.garagePreludeFailed50KG=!accepted;
    //No source frame, source alpha receipt, image acceptance or timing commit.
    return accepted;
}
bool VehicleWindowGl50DS::ownsGarageSourcePrelude50KG(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t publication)const noexcept {
    return storage_&&storage_->garagePreludeOwner50KG==&owner&&storage_->garagePreludePublication50KG==publication;
}
bool VehicleWindowGl50DS::garageUnsealedRenderQuiescent50KG(
    const OriginalUiGarageDrawContribution50KG& draw,std::uint64_t publication)const noexcept {
    if(!storage_||!publication||draw.publication_!=publication||
       !draw.frontendTextureRendererCurrent(*this)||!originalUiSourceRgbaAvailable())return false;
    const auto& s=*storage_;VehicleWindowDiagnostic50DS observed;
    if(!s.current(observed)||!s.frontendOnly||s.active||s.frameSet||s.diagnostic||
       s.garagePollFence50KG||s.nativeGlFaultObserved50KG||s.garagePreludeFailed50KG||
       s.uiSourceFrameOwner||s.uiSourceFrameGeneration||s.garageCompletedPublication50KG||
       s.garageCompletedTarget50KG||s.garagePublishedPublication50KG)return false;
    // A prior prelude may not have started, or may already be truly released
    // on this native retry. A live one must be this exact Draw/publication.
    // Poll fences and observed GL faults are independent owners/history and
    // cannot be discharged by this read-only observation or inferred finish.
    return s.garagePreludeOwner50KG?
        s.garagePreludeOwner50KG==&draw&&s.garagePreludePublication50KG==publication:
        s.garagePreludePublication50KG==0;
}
bool VehicleWindowGl50DS::releaseGarageSourcePrelude50KG(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t publication,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!storage_)return false;auto& s=*storage_;
    if(!s.garagePreludeOwner50KG)return true;
    if(s.garagePreludeOwner50KG!=&owner||s.garagePreludePublication50KG!=publication||
       s.active||s.frameSet||s.diagnostic||!s.current(d))return false;
    //Retire only this native source-target work. Failed GL completion retains
    //the exact owner; it cannot be hidden by dropping the SourceOwner pointer.
    s.waitGarageTargetFence50KG();
    s.diagnostic=&d;bool okay=s.check("before Garage source prelude retirement");
    if(okay){s.gl.finish();okay=s.check("retire actual Garage source-target work");}
    s.diagnostic=nullptr;
    if(okay){s.garagePreludeOwner50KG=nullptr;s.garagePreludePublication50KG=0;s.garagePreludeFailed50KG=false;
        s.garageCompletedPublication50KG=0;s.garageCompletedTarget50KG=0;s.garagePublishedPublication50KG=0;}
    return okay;
}
bool VehicleWindowGl50DS::pollOriginalRenderer245030(bool& complete,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};complete=false;
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"Garage poll requires source RGBA renderer");
    auto& s=*storage_;if(!s.current(d,nullptr,s.menuRasterShared50KG))return false;
    if(!s.frontendOnly||s.active||s.frameSet||s.diagnostic||s.garagePollFence50KG||s.nativeGlFaultObserved50KG)
        return fail(d,VehicleWindowError50DS::frame,"Garage poll requires idle current frontend without a failed poll");
    s.waitGarageTargetFence50KG(); // The prior target is complete, as after its former glFinish.
    s.diagnostic=&d;
    const bool loaded=(s.gl.fenceSync||s.load(s.gl.fenceSync,"glFenceSync"))&&
        (s.gl.getSynciv||s.load(s.gl.getSynciv,"glGetSynciv"))&&
        (s.gl.isSync||s.load(s.gl.isSync,"glIsSync"))&&
        (s.gl.deleteSync||s.load(s.gl.deleteSync,"glDeleteSync"));
    if(!loaded||!s.check("before Garage nonblocking renderer poll")){s.diagnostic=nullptr;return false;}
    // Observe the real context's preceding native work. This inserts no draw,
    // flush, wait, source timing sample or completion receipt. The sync object
    // is a native observation marker, not a reconstruction of guest DMA pools.
    s.garagePollFence50KG=s.gl.fenceSync(0x9117u,0u); //SYNC_GPU_COMMANDS_COMPLETE
    if(!s.check("create Garage native poll marker")||!s.garagePollFence50KG){
        if(d.error==VehicleWindowError50DS::none)fail(d,VehicleWindowError50DS::gl,"Garage native poll marker is null");
        s.diagnostic=nullptr;return false;
    }
    I length=0,status=0;s.gl.getSynciv(s.garagePollFence50KG,0x9114u,1,&length,&status); //SYNC_STATUS
    if(!s.check("observe Garage native completion without waiting")||length!=1||
       (status!=0x9118&&status!=0x9119)){
        if(d.error==VehicleWindowError50DS::none)fail(d,VehicleWindowError50DS::gl,"Garage native poll status is unsupported");
        s.diagnostic=nullptr;return false;
    }
    s.gl.deleteSync(s.garagePollFence50KG);
    if(!s.check("release Garage native poll marker")){s.diagnostic=nullptr;return false;}
    s.garagePollFence50KG=nullptr;s.diagnostic=nullptr;
    complete=status==0x9119;return true;
}
bool VehicleWindowGl50DS::completeGarageSourceTarget140A38(
    OriginalUiGarageDrawContribution50KG& draw,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!originalUiSourceRgbaAvailable()||!draw.identityCurrent()||
       draw.phase_!=decltype(draw.phase_)::finalPass||draw.sourcePC_!=0x140A38||
       !draw.finalPassReturned140844_||!draw.rendererEndReturned2442CC_||
       !draw.terminalQueue105990Current()||draw.targetCompleted140A38_||
       !ownsGarageSourcePrelude50KG(draw,draw.publication_))
        return fail(d,VehicleWindowError50DS::frame,"Garage target requires the actual complete Root render and retired FIFO");
    auto& s=*storage_;
    if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG||
       s.garageCompletedPublication50KG||!s.uiSourceTarget||!s.uiSourceTargetGeneration)
        return fail(d,VehicleWindowError50DS::frame,"Garage target completion requires the same idle actual context");
    s.diagnostic=&d;
    {   // Mark completion with a fence instead of a CPU-blocking glFinish. GL
        // executes this context's later commands after the target's, and the
        // marker is waited before any later CPU observation (see storage).
        static const bool legacyFinish=[]{const char* v=std::getenv("RR_GARAGE_LEGACY_FINISH");return v&&v[0]=='1'&&!v[1];}();
        const auto lookup=[&s](auto& pointer,const char* name){
            if(!pointer)pointer=reinterpret_cast<std::remove_reference_t<decltype(pointer)>>(s.request.procAddress(s.request.context,name));
            return pointer!=nullptr;};
        s.waitGarageTargetFence50KG();
        const bool fenced=!legacyFinish&&s.gl.flush&&lookup(s.gl.fenceSync,"glFenceSync")&&
            lookup(s.gl.clientWaitSync,"glClientWaitSync")&&lookup(s.gl.deleteSync,"glDeleteSync");
        if(fenced)s.garageTargetFence50KG=s.gl.fenceSync(0x9117u,0u); //SYNC_GPU_COMMANDS_COMPLETE
        if(s.garageTargetFence50KG)s.gl.flush();else s.gl.finish();
    }
    const bool okay=s.check("complete actual Garage Root render target");
    if(okay){s.garageCompletedPublication50KG=draw.publication_;s.garageCompletedTarget50KG=s.uiSourceTargetGeneration;}
    else s.garagePreludeFailed50KG=true;
    d.submittedWorkCompleted=okay;s.diagnostic=nullptr;return okay;
}
bool VehicleWindowGl50DS::publishGarageSourceTarget140A68(
    OriginalUiGarageDrawContribution50KG& draw,std::int32_t width,std::int32_t height,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!originalUiSourceRgbaAvailable()||!draw.issuer_||!draw.renderReturned140230Current(*draw.issuer_)||
       !draw.targetPublishAttempted_||draw.targetPublished140A68_||!ownsGarageSourcePrelude50KG(draw,draw.publication_))
        return fail(d,VehicleWindowError50DS::frame,"Garage publication requires this actual completed Root target");
    auto& s=*storage_;
    if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG||
       s.garageCompletedPublication50KG!=draw.publication_||s.garageCompletedTarget50KG!=s.uiSourceTargetGeneration||
       s.garagePublishedPublication50KG||width<=0||height<=0)
        return fail(d,VehicleWindowError50DS::frame,"Garage target publication is stale or already attempted");
    s.diagnostic=&d;
    //Copy the genuine completed source-RGBA target. Do not replay any packets,
    //make an empty SourceFrame, or manufacture a generic byte-blend terminal.
    const bool okay=s.publishUiSource(s.uiSourceClear,width,height);
    if(okay)s.garagePublishedPublication50KG=draw.publication_;
    else s.garagePreludeFailed50KG=true;
    s.diagnostic=nullptr;return okay;
}
bool VehicleWindowGl50DS::rearmGarageSourceTarget140230(
    OriginalUiGarageDrawContribution50KG& draw,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!storage_||!draw.issuer_||!draw.renderReturned140230Current(*draw.issuer_)||
       !draw.targetPublished140A68_||!ownsGarageSourcePrelude50KG(draw,draw.publication_))
        return fail(d,VehicleWindowError50DS::frame,"Garage rearm requires its published completed target");
    auto& s=*storage_;
    if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG||
       s.garageCompletedPublication50KG!=draw.publication_||s.garagePublishedPublication50KG!=draw.publication_||
       s.garageCompletedTarget50KG!=s.uiSourceTargetGeneration||!draw.drainGpu_||
       s.request.originalUiTextures!=draw.drainGpu_.get()||s.uiTexturePublication!=draw.drainGpu_->publicationToken())
        return fail(d,VehicleWindowError50DS::frame,"Garage prior target or actual bank lifetime changed before rearm");
    //Consume only this native invocation's target lease. The published image
    //and its backing survive; next source clear mints the actual next target
    //generation. No GL bank, source history, FIFO epoch or Camera is reset.
    s.garagePreludeOwner50KG=nullptr;s.garagePreludePublication50KG=0;
    s.garageCompletedPublication50KG=0;s.garageCompletedTarget50KG=0;s.garagePublishedPublication50KG=0;
    return true;
}
bool VehicleWindowGl50DS::retireOriginalUiSourceOwner(
    const OriginalUiSourceOwner50KM& owner,VehicleWindowDiagnostic50DS& d) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if (!originalUiSourceRgbaAvailable())
        return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;
    if(s.menuRasterShared50KG&&s.menuRasterOwner50KG!=&owner)
        return fail(d,VehicleWindowError50DS::frame,"menu raster retirement requires its exact SourceOwner");
    if (!s.current(d,nullptr,s.menuRasterShared50KG))return false;
    if (s.active || s.frameSet || s.diagnostic ||
        (s.uiSourceFrameOwner && s.uiSourceFrameOwner!=&owner))
        return fail(d,VehicleWindowError50DS::frame,"source UI retirement requires idle matching live owner");
    s.waitGarageTargetFence50KG();
    s.diagnostic=&d;
    bool accepted=s.check("before explicit source UI owner retirement");
    if (accepted){
        s.gl.finish();
        accepted=s.check("drain prior GL work before source UI owner retirement");
    }
    d.submittedWorkCompleted=accepted;
    if (accepted){s.uiSourceFrameOwner=nullptr;s.uiSourceFrameGeneration=0;
        s.uiSourceAcceptedTargetGeneration=0;s.uiSourcePublishedTargetGeneration=0;}
    s.diagnostic=nullptr;
    if(accepted&&s.menuRasterShared50KG){
        s.menuRasterRetiredShared50KG=s.menuRasterShared50KG;s.menuRasterRetiredOwner50KG=s.menuRasterOwner50KG;
        s.menuRasterRetiredEpoch50KG=s.menuRasterEpoch50KG;s.menuRasterRetiredInvocation50KG=s.menuRasterInvocation50KG;
        s.clearMenuRasterLease50KG();
    }
    return accepted;
}
bool VehicleWindowGl50DS::completeGarageCollector24C160(OriginalUiGarageDrawContribution50KG& draw,
    VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};if(!initialized()||!draw.identityCurrent()||draw.phase_!=decltype(draw.phase_)::collectorDrain24B7B0||
       draw.sourcePC_!=0x105990u||!draw.drainContextCaptured_||!ownsGarageSourcePrelude50KG(draw,draw.publication_))
        return fail(d,VehicleWindowError50DS::frame,"actual completed Garage leaf traversal required");
    const auto fifo=draw.fifoForBackendDrain();
    if(!fifo||!fifo->draining_||fifo->epoch04_!=draw.drainEpoch_||draw.drainCursor_!=draw.drainOrder_.size()||
       draw.drainOrder_.size()!=fifo->records_.size()||draw.drainBatchPending_||draw.drainBatchFailed_||
       (!fifo->records_.empty()&&!draw.drainBatchSubmitted_))return fail(d,VehicleWindowError50DS::frame,"actual FIFO epoch and all accepted leaves");
    auto& s=*storage_;if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG)return false;
    if(!s.gl.garageFlush24C160){s.diagnostic=&d;const bool loaded=s.load(s.gl.garageFlush24C160,"glFlush");s.diagnostic=nullptr;
        if(!loaded)return false;}
    //All retained primitive bytes have been copied to actual GL-owned buffers.
    //Only now perform source105990 and24B954. A later real command error
    //retains this completed prefix/epoch, never reconstructs discarded records.
    ::original_main_entry50KG::NativeCollectorFifo50LA::CompletedLowering receipt(*fifo);
    if(!fifo->commitActualDrain(receipt))return fail(d,VehicleWindowError50DS::frame,"actual105990 FIFO release rejected");
    draw.sourcePC_=0x24C160;
    //24C274/278 emits GS TEXFLUSH(1). Native image writes and draws use the
    //same current ordered GL context, which owns texture coherence; submit
    //that real command stream without a fabricated guest packet or GPU wait.
    s.diagnostic=&d;s.gl.garageFlush24C160();const bool okay=s.check("actual Garage ordered texture command tail");s.diagnostic=nullptr;
    if(!okay)s.garagePreludeFailed50KG=true;
    return okay;
}
bool VehicleWindowGl50DS::submitGarageCollectorBatch24B26C(
    OriginalUiGarageDrawContribution50KG& draw,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d={};
    if(!initialized()||!draw.identityCurrent()||
       draw.phase_!=decltype(draw.phase_)::collectorDrain24B7B0||draw.sourcePC_!=0x24B26Cu||
       !draw.drainBatchPending_||draw.drainBatchSubmitted_||draw.drainBatchFailed_||
       !draw.drainBatchCanonical_||!ownsGarageSourcePrelude50KG(draw,draw.publication_))
        return fail(d,VehicleWindowError50DS::frame,"actual retained Garage collector batch required");
    if(draw.drainParticleGeometry_&&!draw.drainParticleRecordCurrent())
        return fail(d,VehicleWindowError50DS::frame,"actual pinned particle record required");
    auto& s=*storage_;
    if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG)
        return fail(d,VehicleWindowError50DS::material,"actual Garage batch binding transaction rejected");
    // One-time native resource preparation, before this real canonical transaction.
    // No frame creates additional sampler names once this bounded owner is published.
    s.diagnostic=&d;const bool samplers=s.ensureGarageSamplers50KG();s.diagnostic=nullptr;
    if(!samplers)return false;
    garageFixedAdditive50KG.selfTest(s); // once per process, before any Garage batch state
    garageFixedSpecular50KG.prepare(s);  // self-test once per process; variant per UI program
    if(!draw.drainBatchCanonical_->beginGarageBatch(draw,*this))
        return fail(d,VehicleWindowError50DS::material,"actual Garage batch binding transaction rejected");
    bool started=false;
    bool accepted=false;
    if(draw.drainModelClippedEmptyCurrent()||draw.drainParticleClippedEmptyCurrent()){
        //The exact queued numeric owner processed every source triangle and
        //returned clipped-empty. GPU image upload and canonical bindings are
        //real; no raster command exists, so do not manufacture a one-quad draw.
        const auto& input=draw.drainBatchNative_.draw;
        accepted=input.quadCount==0&&input.spanCount==0&&input.textures==s.request.originalUiTextures&&
            input.texturePublication==s.uiTexturePublication&&input.textures&&
            input.textures->validBorrow(s.request.context,s.request.isCurrent,s.uiTexturePublication);
    }else accepted=submitOriginalUiImpl(draw.drainBatchNative_,640,448,true,d,&started,false,false,nullptr,true);
    const bool committed=draw.drainBatchCanonical_->finish(draw.publication_,accepted,started);
    //No SourceOwner begin/finish, SourceFrame acceptance, publication or swap.
    //A failure retains the real FIFO records and blocks replay of this batch.
    draw.drainBatchPending_=false;draw.drainBatchSubmitted_=accepted&&committed;
    draw.drainBatchFailed_=!draw.drainBatchSubmitted_;
    if(draw.drainBatchFailed_)s.garagePreludeFailed50KG=true;
    if(draw.drainBatchFailed_){
        std::fprintf(stderr,"original_garage_native_batch_rejected publication=%llu epoch=%u cursor=%zu record=%zu accepted=%u started=%u committed=%u renderer_error=%u operation=%.256s gl_error=%u cleanup_gl_error=%u batch_quads=%u batch_spans=%u ui_draws=%u ui_quads=%u ui_triangles=%u prelude_failed=%u\n",
            static_cast<unsigned long long>(draw.publication_),draw.drainEpoch_,draw.drainCursor_,draw.drainActiveRecordOrdinal_,
            accepted?1u:0u,started?1u:0u,committed?1u:0u,static_cast<unsigned>(d.error),d.operation?d.operation:"",
            d.glError,d.cleanupGlError,draw.drainBatchNative_.draw.quadCount,draw.drainBatchNative_.draw.spanCount,
            d.originalUiDraws,d.originalUiQuads,d.originalUiSourceTriangles,s.garagePreludeFailed50KG?1u:0u);
    }
    d.originalUiSourceAccepted=false;d.originalUiAccepted=false;
    return draw.drainBatchSubmitted_;
}

bool VehicleWindowGl50DS::submitOriginalUiImpl(const OriginalUiNativeDraw50KG& native,
    std::int32_t width,std::int32_t height,bool sourceRgba,VehicleWindowDiagnostic50DS& d,
    bool* sourceWorkStarted,bool publishSource,bool riderReceiptChecked,const OriginalUiDirectVideo50KG* directVideo,bool garageBatch) noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted()){return fail(d,VehicleWindowError50DS::frame,"Options viewport owns native mutation until cancellation");}

    d = {};
    if(sourceWorkStarted)*sourceWorkStarted=false;
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_; const auto& input = native.draw;
    // Rider's typed receipt authenticates each span's source policy. Shared
    // images can be used by materials with different filters (e.g. popup fonts).
    const bool perMaterialSamplers=garageBatch||riderReceiptChecked;
    if(s.menuRasterShared50KG&&(!s.menuRasterBackendActive50KG||!sourceRgba||garageBatch||
       &native!=s.menuRasterNative50KG||!s.menuRasterApplied50KG||s.menuRasterFailed50KG||
       input.canvasWidth!=float(s.menuRasterInputs50KG.viewport[2])||
       input.canvasHeight!=float(s.menuRasterInputs50KG.viewport[3])||
       input.textures!=s.menuRasterInputs50KG.gpu||input.texturePublication!=s.menuRasterInputs50KG.publication||
       s.request.originalUiCpuTextures!=s.menuRasterInputs50KG.cpu||
       s.uiTextureEpoch!=s.menuRasterTextureEpoch50KG||s.uiSourceTarget!=s.menuRasterTarget50KG||
       s.uiSourceColor!=s.menuRasterColor50KG||s.uiSourceDepth!=s.menuRasterDepth50KG||
       s.uiSourceTargetGeneration!=s.menuRasterTargetGeneration50KG))
        return fail(d,VehicleWindowError50DS::frame,"native source draw changed its retained menu raster contract");
    if(garageBatch&&(!sourceRgba||publishSource||directVideo||!s.garagePreludeOwner50KG||s.garagePreludeFailed50KG))
        return fail(d,VehicleWindowError50DS::frame,"Garage batch requires retained unaccepted source target");
    if(s.garagePreludeOwner50KG&&!sourceRgba)
        return fail(d,VehicleWindowError50DS::frame,"ordinary UI draw cannot interleave with retained Garage source work");
    const auto submitStageStart=Storage::UiCoverageClock::now();
    std::uint64_t preDrawDiagnosticNs=0,drawDiagnosticNs=0,resolveDiagnosticNs=0,publishDiagnosticNs=0;
    U diagnosticPrefixDraws=0,diagnosticMixedDraws=0,diagnosticEmptyAa1=0;
    bool diagnosticPromoted=false;std::uint64_t diagnosticPromotionNs=0;
    if(input.textures&&input.textures->riderSparse()&&(!sourceRgba||!riderReceiptChecked||s.riderTextureBorrow!=input.textures))
        return fail(d,VehicleWindowError50DS::material,"sparse Rider draw requires actual sealed complete-frame residency receipt");
    d.originalUiCoherentFramebufferFetch=sourceRgba && s.uiSourceCoherentFramebufferFetch;
    if (!s.current(d)) return false;
    if (s.active || s.frameSet || !s.request.enableOriginalUi || !s.uiProgram ||
        !s.uiVertexBuffer || !s.uiIndexBuffer || !s.uiVertexArray || !s.uiParticleFogBuffer || !s.uiSourceRasterWBuffer || !s.uiSourcePacketStqBuffer ||
        !s.uiTargets[0] || !s.uiTargets[1] ||
        width <= 0 || height <= 0 || width > s.maximumWidth || height > s.maximumHeight ||
        input.canvasWidth != 640.f || input.canvasHeight != 448.f ||
        !input.quadCount || input.quadCount > (sourceRgba?originalUiMaximumSourceQuads50KM:originalUiMaximumQuads50KG) || !input.vertices ||
        !input.spanCount || input.spanCount > originalUiMaximumSpans50KG || !input.spans ||
        !native.states || native.stateCount != input.spanCount ||
        (sourceRgba ? native.opaquePresentationAlphaOnly : !native.opaquePresentationAlphaOnly))
        return fail(d, VehicleWindowError50DS::frame, "original UI requires explicit native policy and bounded 640x448 draw");
    if(directVideo&&(!sourceRgba||!validOriginalUiDirectVideo50KG(*directVideo)))
        return fail(d,VehicleWindowError50DS::frame,"unsupported direct video source command");
    if (sourceRgba && (!originalUiSourceRgbaAvailable() || input.retainRaceImage))
        return fail(d,VehicleWindowError50DS::frame,"source UI requires fresh native source target, never retained race alpha");
    if (input.textures != s.request.originalUiTextures || input.texturePublication != s.uiTexturePublication ||
        (input.textures && !input.textures->validBorrow(s.request.context, s.request.isCurrent, s.uiTexturePublication)) ||
        (!input.textures && input.texturePublication != 0))
        return fail(d, VehicleWindowError50DS::material, "original UI texture publication/context/thread changed");
    for (float value : input.clearRgba)
        if (!uiFinite50KG(value) || value < 0.f || value > 1.f)
            return fail(d, VehicleWindowError50DS::frame, "invalid original UI clear color");
    if (input.clearRgba[3] != 1.f)
        return fail(d, VehicleWindowError50DS::frame, "original UI presentation alpha must be opaque");
    if (input.retainRaceImage && (s.raceWidth <= 0 || s.raceHeight <= 0 || !s.gl.blitFramebuffer))
        return fail(d, VehicleWindowError50DS::noImage, "requested original UI race backdrop is unavailable");
    for (U i = 0; i < input.quadCount * 4; ++i) {
        const auto& v = input.vertices[i];
        for (float value : v.position) if (!uiFinite50KG(value))
            return fail(d, VehicleWindowError50DS::frame, "nonfinite original UI position");
        for (float value : v.uv) if (!uiFinite50KG(value))
            return fail(d, VehicleWindowError50DS::frame, "nonfinite original UI UV");
        if (v.position[2] < 0.f || v.position[2] > 16777215.f ||
            !uiNonnegativeIntegral50KG(v.position[2]) || v.color[3] > 128)
            return fail(d, VehicleWindowError50DS::frame, "UI requires original 24-bit packet depth and raw alpha <=128");
    }
    if ((native.sourcePacketStq == nullptr) != (native.sourcePacketStqCount == 0) ||
        (native.sourcePacketStq && native.sourcePacketStqCount != input.quadCount * 4) ||
        (native.sourcePacketStq && !sourceRgba))
        return fail(d, VehicleWindowError50DS::frame, "invalid or non-source packet STQ stream");
    U next = 0;
    bool anyFog = false, anySourceRaster = false, anyCoverage = false, anySourcePacketStq = false;
    for (U i = 0; i < input.spanCount; ++i) {
        const auto& span = input.spans[i]; const auto& state = native.states[i];
        if(state.coverage!=OriginalUiCoverage50KG::aa0&&state.coverage!=OriginalUiCoverage50KG::fourSampleAa1)
            return fail(d,VehicleWindowError50DS::material,"unknown original UI coverage policy");
        if(state.coverage==OriginalUiCoverage50KG::fourSampleAa1&&!sourceRgba)
            return fail(d,VehicleWindowError50DS::material,"UI AA1 requires source-RGBA sample composition");
        if(state.coverage==OriginalUiCoverage50KG::fourSampleAa1&&
            (span.materialMode!=OriginalUiMaterialMode50KG::sourceDiffuseCompressed||
             span.blend!=OriginalUiBlend50KG::sourceAlpha||state.sourceAlphaTestGreaterZero))
            return fail(d,VehicleWindowError50DS::material,"UI AA1 requires original compressed Diffuse source-alpha without alpha test");
        anyCoverage=anyCoverage||state.coverage==OriginalUiCoverage50KG::fourSampleAa1;
        const bool particle = span.materialMode == OriginalUiMaterialMode50KG::mainParticle;
        const bool specular=span.materialMode==OriginalUiMaterialMode50KG::sourceSpecularMapCompressed;
        const bool sourceRaster = span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuse ||
            span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuseCompressed || specular ||
            span.materialMode == OriginalUiMaterialMode50KG::sourcePrelitCompressed;
        const bool fixed64Specular=span.blend==OriginalUiBlend50KG::fixed64Additive;
        const bool specularBlend=span.blend==OriginalUiBlend50KG::destinationAlphaAdditive ||
            span.blend==OriginalUiBlend50KG::fixed255Additive || fixed64Specular;
        const bool shadowBlend=span.blend==OriginalUiBlend50KG::destinationSourceAlphaAttenuation;
        const bool sourceSpecialBlend=specularBlend||shadowBlend;
        if (shadowBlend && (!sourceRgba || span.materialMode!=OriginalUiMaterialMode50KG::sourceDiffuseCompressed ||
            state.depth!=OriginalUiDepth50KG::greaterEqual || !state.depthWrite ||
            !state.sourceFogEnabled || !state.sourceAlphaTestGreaterZero))
            return fail(d,VehicleWindowError50DS::material,"shadow requires actual typed source-RGBA policy");
        // UIWin trophy pass2 (prepareOriginalUiWinTrophySpecularMaterial50PD) is the one
        // admitted destination-alpha specular that authors zWriteDisabled=0 and
        // alphaTestOnOff=0; its depth mask and alpha test are carried per span.
        const bool depthWritingDestinationSpecular=specular&&
            span.blend==OriginalUiBlend50KG::destinationAlphaAdditive&&state.depthWrite&&!state.sourceAlphaTestGreaterZero;
        if ((specular && !sourceRgba) || specularBlend!=specular ||
            (specular && (state.depth!=OriginalUiDepth50KG::greaterEqual ||
                (state.depthWrite!=fixed64Specular&&!depthWritingDestinationSpecular) ||
                !state.sourceFogEnabled || (!state.sourceAlphaTestGreaterZero&&!depthWritingDestinationSpecular))))
            return fail(d,VehicleWindowError50DS::material,"specular requires actual typed source-RGBA policy");
        const bool textured = span.materialMode == OriginalUiMaterialMode50KG::texturedModulate || particle || sourceRaster;
        const bool untextured = span.materialMode == OriginalUiMaterialMode50KG::untexturedVertexColor;
        const float blendAlphaScale = textured ? (255.f / 128.f) * (255.f / 128.f) : 255.f / 128.f;
        if (!span.quadCount || span.firstQuad != next || span.quadCount > input.quadCount - next ||
            (!textured && !untextured) || span.colorScale != (textured ? 255.f / 128.f : 1.f) ||
            (span.blend != OriginalUiBlend50KG::opaque && span.blend != OriginalUiBlend50KG::sourceAlpha &&
                span.blend != OriginalUiBlend50KG::additive && !sourceSpecialBlend) ||
            span.alphaScale != (span.blend == OriginalUiBlend50KG::opaque || sourceSpecialBlend ? 1.f : blendAlphaScale) ||
            (state.depth != OriginalUiDepth50KG::never && state.depth != OriginalUiDepth50KG::always &&
                state.depth != OriginalUiDepth50KG::greaterEqual && state.depth != OriginalUiDepth50KG::greater))
            return fail(d, VehicleWindowError50DS::material, "invalid ordered original UI material/depth policy");
        if (!sourceRaster && (state.sourceFogEnabled || state.sourceAlphaTestGreaterZero))
            return fail(d, VehicleWindowError50DS::material, "source Diffuse flags on another material identity");
        if (state.sourcePacketStq && (!sourceRgba || !sourceRaster || !native.sourcePacketStq))
            return fail(d, VehicleWindowError50DS::material, "raw packet STQ requires sealed source raster ownership");
        if (span.materialMode == OriginalUiMaterialMode50KG::sourcePrelitCompressed &&
            (!sourceRgba || !state.sourcePacketStq))
            return fail(d, VehicleWindowError50DS::material, "original PreLit requires its raw packet STQ source route");
        anySourcePacketStq = anySourcePacketStq || state.sourcePacketStq;
        anyFog = anyFog || particle || (sourceRaster && state.sourceFogEnabled);
        anySourceRaster = anySourceRaster || sourceRaster;
        if (particle) {
            if (span.blend == OriginalUiBlend50KG::opaque || state.depth != OriginalUiDepth50KG::greaterEqual ||
                state.depthWrite || state.sampler.minification != MaterialTextureFilter50EB::linear ||
                state.sampler.magnification != MaterialTextureFilter50EB::linear ||
                state.sampler.s != MaterialTextureWrap50EB::repeat || state.sampler.t != MaterialTextureWrap50EB::repeat)
                return fail(d, VehicleWindowError50DS::material, "invalid actual Main particle policy");
        }
        if (untextured) {
            if (span.textureIndex != assets::unresolvedMaterialTexture50EA ||
                state.sampler.minification != MaterialTextureFilter50EB::unspecified ||
                state.sampler.magnification != MaterialTextureFilter50EB::unspecified ||
                state.sampler.s != MaterialTextureWrap50EB::unspecified ||
                state.sampler.t != MaterialTextureWrap50EB::unspecified)
                return fail(d, VehicleWindowError50DS::material,
                    "untextured UI requires no texture index or sampler");
            next += span.quadCount;
            continue;
        }
        const auto filter = [](MaterialTextureFilter50EB v) noexcept {
            return v == MaterialTextureFilter50EB::nearest || v == MaterialTextureFilter50EB::linear;
        };
        const auto wrap = [](MaterialTextureWrap50EB v) noexcept {
            return v == MaterialTextureWrap50EB::repeat || v == MaterialTextureWrap50EB::clampToEdge;
        };
        if (!filter(state.sampler.minification) || !filter(state.sampler.magnification) ||
            state.sampler.minification != state.sampler.magnification ||
            !wrap(state.sampler.s) || !wrap(state.sampler.t))
            return fail(d, VehicleWindowError50DS::material, "UI requires explicit admitted matching min/mag source sampler");
        const auto* texture = input.textures ? input.textures->asset(span.textureIndex) : nullptr;
        if (!texture || !texture->name)
            return fail(d, VehicleWindowError50DS::material, "original UI span lacks its real published texture");
        if(input.textures->riderSparse()){
            const auto* cpu=s.request.originalUiCpuTextures?s.request.originalUiCpuTextures->texture(span.textureIndex):nullptr;
            if(!cpu||cpu->identity!=texture->identity)
                return fail(d,VehicleWindowError50DS::material,"Rider actual textured span lacks exact resident identity");
        }
        if (span.textureIndex >= assets::maxMaterialTextureAssets50EA)
            return fail(d, VehicleWindowError50DS::material, "UI texture alpha metadata index out of bounds");
        if (!sourceRgba && span.blend != OriginalUiBlend50KG::opaque) {
            const U maximumAlpha = s.uiTextureMaximumAlpha[span.textureIndex];
            const U firstVertex = span.firstQuad * 4, endVertex = (span.firstQuad + span.quadCount) * 4;
            for (U v = firstVertex; v < endVertex; ++v)
                if (U(input.vertices[v].color[3]) * maximumAlpha > 128u * 128u)
                    return fail(d, VehicleWindowError50DS::material,
                        "source texture times vertex alpha exceeds native blend factor domain");
        }
        next += span.quadCount;
    }
    if (next != input.quadCount)
        return fail(d, VehicleWindowError50DS::frame, "original UI spans must partition all quads exactly");
    if (anyFog && (!native.particleFog || native.particleFogCount != input.quadCount * 4))
        return fail(d, VehicleWindowError50DS::frame, "fog-enabled materials require the parallel source fog stream");
    if ((native.particleFog == nullptr) != (native.particleFogCount == 0) ||
        (native.particleFog && native.particleFogCount != input.quadCount * 4))
        return fail(d, VehicleWindowError50DS::frame, "invalid particle fog stream bounds");
    for (U i = 0; i < native.particleFogCount; ++i)
        if (!uiFinite50KG(native.particleFog[i]) || native.particleFog[i] < 0.f ||
            native.particleFog[i] > 255.f / 256.f)
            return fail(d, VehicleWindowError50DS::frame, "invalid source particle fog coefficient");
    if ((native.sourceRasterW == nullptr) != (native.sourceRasterWCount == 0) ||
        (native.sourceRasterW && native.sourceRasterWCount != input.quadCount * 4) ||
        (anySourceRaster && !native.sourceRasterW))
        return fail(d, VehicleWindowError50DS::frame, "invalid or missing source perspective W stream");
    for (U i = 0; i < native.sourcePacketStqCount; ++i) {
        const auto& stq = native.sourcePacketStq[i];
        if (!uiFinite50KG(stq[0]) || !uiFinite50KG(stq[1]) || !uiFinite50KG(stq[2]) || stq[2] <= 0.f ||
            !uiFinite50KG(stq[0] / stq[2]) || !uiFinite50KG(stq[1] / stq[2]))
            return fail(d, VehicleWindowError50DS::frame, "invalid original packet STQ sampling range");
    }
    for (U i = 0; i < native.sourceRasterWCount; ++i)
        if (!uiFinite50KG(native.sourceRasterW[i]) || native.sourceRasterW[i] <= 0.f)
            return fail(d, VehicleWindowError50DS::frame, "source perspective W must be finite and positive");
    for (U i = 0; i < input.spanCount; ++i) {
        const auto& span = input.spans[i]; const auto& state = native.states[i];
        const bool particle = span.materialMode == OriginalUiMaterialMode50KG::mainParticle;
        if (!particle && !state.sourceFogEnabled) continue;
        for (U v = span.firstQuad * 4; v < (span.firstQuad + span.quadCount) * 4; ++v) {
            const float f = native.particleFog[v];
            if ((particle && f > 254.f / 256.f) || (!particle && !uiNonnegativeIntegral50KG(f * 256.f)) ||
                (span.materialMode==OriginalUiMaterialMode50KG::sourceSpecularMapCompressed && f!=255.f/256.f))
                return fail(d, VehicleWindowError50DS::frame, "fog stream does not match source program byte domain");
        }
    }
    for (float value : native.particleFogColor)
        if (!uiFinite50KG(value) || value < 0.f || value > 1.f)
            return fail(d, VehicleWindowError50DS::frame, "invalid authored particle fog color");

    U sourceIndexCount = 0;
    if (sourceRgba) {
        // Keep exactly the old source triangle sequence and equal-XY skip.
        // No projection, Z/W, clipping or raster bounds enter this decision.
        // Validated spans partition at most the cold source quad capacity;
        // each quad writes at most six indices, with no frame allocation.
        constexpr U corners[2][3] = {{0, 1, 2}, {0, 2, 3}};
        for (U i = 0; i < input.spanCount; ++i) {
            const auto& span = input.spans[i];
            auto& range = s.uiSourceIndices->spans[i];
            range.firstIndex = sourceIndexCount;
            for (U q = span.firstQuad; q < span.firstQuad + span.quadCount; ++q) {
                const auto* v = &input.vertices[q * 4];
                const auto samePointXY = [&](U a, U b) {
                    return v[a].position[0] == v[b].position[0] &&
                        v[a].position[1] == v[b].position[1];
                };
                for (const auto& c : corners) {
                    if (samePointXY(c[0], c[1]) || samePointXY(c[1], c[2]) ||
                        samePointXY(c[0], c[2])) continue;
                    for (U corner : c) s.uiSourceIndices->indices[sourceIndexCount++] = q * 4 + corner;
                }
            }
            range.indexCount = sourceIndexCount - range.firstIndex;
        }
    }

    // A descriptor does not imply a covered AA1 sample. Prove the prefix
    // boundary from exactly the same retained triples and conservative XY/W
    // domains as the mixed renderer, never from alpha, pose or rider identity.
    U firstCoveredAa1Span=input.spanCount;
    if(sourceRgba&&anyCoverage){
        for(U si=0;si<input.spanCount&&firstCoveredAa1Span==input.spanCount;++si){
            if(native.states[si].coverage!=OriginalUiCoverage50KG::fourSampleAa1)continue;
            const auto& range=s.uiSourceIndices->spans[si];
            for(U index=range.firstIndex;index<range.firstIndex+range.indexCount;index+=3){
                const U ia=s.uiSourceIndices->indices[index],ib=s.uiSourceIndices->indices[index+1],ic=s.uiSourceIndices->indices[index+2];
                OriginalUiCoverageBounds50KG region;
                if(ia>=input.quadCount*4||ib>=input.quadCount*4||ic>=input.quadCount*4||
                   !originalUiCoverageBounds50KG(input.vertices[ia],input.vertices[ib],input.vertices[ic],region))
                    return fail(d,VehicleWindowError50DS::frame,"invalid original UI coverage triangle bounds");
                // Admitted AA1 is sourceDiffuseCompressed and has a validated
                // W stream. Extreme finite W must never be classified empty.
                if(!originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ia])||
                   !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ib])||
                   !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ic]))region={0,0,640,448};
                if(!region.empty()){firstCoveredAa1Span=si;break;}
            }
        }
    }

    // Exact feedback batching (see UiFeedbackBatches50KG). Only spans that will
    // take the per-triangle barrier fallback are reordered, before the index
    // upload; a span that cannot be scheduled keeps its original stream.
    const bool fixedAdditive=garageBatch&&sourceRgba&&!s.uiSourceCoherentFramebufferFetch&&
        garageFixedAdditive50KG.passed&&!GarageFixedAdditive50KG::legacy();
    // Fixed-function destination-alpha specular (GarageFixedSpecular50KG):
    // each admitted span's retained triangles are appended once more as
    // consecutive copy pairs after the source stream, inside the same
    // bounded index storage. A span that does not fit keeps the exact path.
    const bool fixedSpecular=garageBatch&&sourceRgba&&!s.uiSourceCoherentFramebufferFetch&&
        garageFixedSpecular50KG.ready(s);
    thread_local std::vector<std::uint32_t> specularFirst,specularCount;
    U extendedIndexCount=sourceIndexCount;
    if(fixedSpecular){
        try{specularFirst.assign(input.spanCount,0u);specularCount.assign(input.spanCount,0u);}
        catch(...){specularCount.clear();}
        constexpr U capacity=originalUiMaximumSourceQuads50KM*6;
        for(U si=0;si<input.spanCount&&specularCount.size()==input.spanCount;++si){
            const auto& span=input.spans[si];const auto& state=native.states[si];
            if(span.blend!=OriginalUiBlend50KG::destinationAlphaAdditive||
               state.coverage==OriginalUiCoverage50KG::fourSampleAa1||(anyCoverage&&si>=firstCoveredAa1Span))continue;
            const auto& range=s.uiSourceIndices->spans[si];
            if(!range.indexCount||range.indexCount%3||range.indexCount>(capacity-extendedIndexCount)/2)continue;
            specularFirst[si]=extendedIndexCount;specularCount[si]=range.indexCount*2;
            for(U t=0;t<range.indexCount;t+=3)for(U copy=0;copy<2;++copy)for(U k=0;k<3;++k)
                s.uiSourceIndices->indices[extendedIndexCount++]=s.uiSourceIndices->indices[range.firstIndex+t+k];
        }
    }
    const auto specularSpan=[&](U si)noexcept{return fixedSpecular&&si<specularCount.size()&&specularCount[si]!=0;};
    bool feedbackBatches=false;auto& fb=uiFeedbackBatches50KG;
    if(sourceRgba&&!s.uiSourceCoherentFramebufferFetch){
        try{
            if(fb.tileStamp.size()!=std::size_t(fb.tilesX*fb.tilesY)){
                fb.tileStamp.assign(std::size_t(fb.tilesX*fb.tilesY),0u);fb.tileBatch.assign(std::size_t(fb.tilesX*fb.tilesY),0u);fb.stamp=0;}
            fb.spanFirst.assign(input.spanCount,0u);fb.spanCount.assign(input.spanCount,0u);
            fb.rangeFirst.clear();fb.rangeCount.clear();feedbackBatches=true;
        }catch(...){feedbackBatches=false;}
    }
    for(U si=0;feedbackBatches&&si<input.spanCount;++si){
        const auto& span=input.spans[si];const auto& state=native.states[si];
        if(span.blend==OriginalUiBlend50KG::opaque||state.coverage==OriginalUiCoverage50KG::fourSampleAa1||
           (anyCoverage&&si>=firstCoveredAa1Span))continue;
        if(fixedAdditive&&garageFixedAdditiveBlend50KG(span.blend))continue; // one fixed-function draw, original order
        if(specularSpan(si))continue; // one doubled fixed-function draw, original order
        const auto& range=s.uiSourceIndices->spans[si];const U triangles=range.indexCount/3;
        if(triangles<2)continue;
        const bool raster=span.materialMode==OriginalUiMaterialMode50KG::sourceDiffuse||
            span.materialMode==OriginalUiMaterialMode50KG::sourceDiffuseCompressed||
            span.materialMode==OriginalUiMaterialMode50KG::sourceSpecularMapCompressed||
            span.materialMode==OriginalUiMaterialMode50KG::sourcePrelitCompressed;
        try{fb.batchOf.resize(triangles);fb.scratch.resize(range.indexCount);}catch(...){continue;}
        if(++fb.stamp==0){std::fill(fb.tileStamp.begin(),fb.tileStamp.end(),0u);fb.stamp=1;}
        bool scheduled=true;std::uint32_t batches=0;
        for(U t=0;t<triangles;++t){
            const U at=range.firstIndex+3*t;
            const U ia=s.uiSourceIndices->indices[at],ib=s.uiSourceIndices->indices[at+1],ic=s.uiSourceIndices->indices[at+2];
            OriginalUiCoverageBounds50KG r;
            if(ia>=input.quadCount*4||ib>=input.quadCount*4||ic>=input.quadCount*4||
               !originalUiCoverageBounds50KG(input.vertices[ia],input.vertices[ib],input.vertices[ic],r)){scheduled=false;break;}
            if(raster&&(!originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ia])||
               !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ib])||
               !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ic])))r={0,0,640,448};
            std::uint32_t batch=0;
            if(!r.empty()){
                const int x0=r.x/fb.tile,x1=(r.x+r.width-1)/fb.tile,y0=r.y/fb.tile,y1=(r.y+r.height-1)/fb.tile;
                for(int y=y0;y<=y1;++y)for(int x=x0;x<=x1;++x){const auto c=std::size_t(y*fb.tilesX+x);
                    if(fb.tileStamp[c]==fb.stamp&&fb.tileBatch[c]>batch)batch=fb.tileBatch[c];}
                for(int y=y0;y<=y1;++y)for(int x=x0;x<=x1;++x){const auto c=std::size_t(y*fb.tilesX+x);
                    fb.tileStamp[c]=fb.stamp;fb.tileBatch[c]=batch+1;}
            }
            fb.batchOf[t]=batch;if(batch+1>batches)batches=batch+1;
        }
        if(!scheduled||batches>=triangles)continue; // original per-triangle order
        try{fb.offsets.assign(std::size_t(batches)+1,0u);
            fb.rangeFirst.reserve(fb.rangeFirst.size()+batches);fb.rangeCount.reserve(fb.rangeCount.size()+batches);}
        catch(...){continue;}
        for(U t=0;t<triangles;++t)++fb.offsets[fb.batchOf[t]+1];
        for(std::uint32_t b=0;b<batches;++b)fb.offsets[b+1]+=fb.offsets[b];
        fb.spanFirst[si]=static_cast<std::uint32_t>(fb.rangeFirst.size());fb.spanCount[si]=batches;
        for(std::uint32_t b=0;b<batches;++b){
            fb.rangeFirst.push_back(range.firstIndex+3*fb.offsets[b]);fb.rangeCount.push_back(3*(fb.offsets[b+1]-fb.offsets[b]));}
        for(U t=0;t<triangles;++t){const auto slot=fb.offsets[fb.batchOf[t]]++;
            for(U k=0;k<3;++k)fb.scratch[3*slot+k]=s.uiSourceIndices->indices[range.firstIndex+3*t+k];}
        for(U k=0;k<range.indexCount;++k)s.uiSourceIndices->indices[range.firstIndex+k]=fb.scratch[k];
    }

    s.diagnostic = &d;
    bool accepted = s.check("before original UI submission") &&
        (!perMaterialSamplers || s.ensureGarageSamplers50KG()) &&
        ((sourceRgba && !publishSource) || s.request.output == VehicleWindowOutput50DS::headless || s.checkDefaultFramebuffer());
    const auto pending = static_cast<std::uint8_t>(s.uiPublished ^ 1u);
    if (accepted) {
        // Authenticated Rider and Garage spans use Window-owned per-policy
        // samplers. Image base/max levels remain separately immutable.
        // Other spans continue using the uploaded default through sampler0.
        // Cache this submission's exact image+sampler observations, never readiness.
        struct SamplerObservation { U image, sampler; I values[6]; };
        SamplerObservation observed[originalUiMaximumSpans50KG];
        U observedCount = 0;
        constexpr U parameters[] = {0x2801, 0x2800, 0x2802, 0x2803, 0x813C, 0x813D};
        s.gl.activeTexture(textureUnit0); if (s.gl.bindSampler) s.gl.bindSampler(0, 0);
        for (U i = 0; accepted && i < input.spanCount; ++i) {
            if (input.spans[i].materialMode == OriginalUiMaterialMode50KG::untexturedVertexColor) continue;
            const auto& sampler = native.states[i].sampler;
            const U image = input.textures->texture(input.spans[i].textureIndex);
            const U samplerName=perMaterialSamplers?s.garageSampler50KG(sampler):0;
            if(perMaterialSamplers&&!samplerName){accepted=fail(d,VehicleWindowError50DS::material,"actual UI span has no owned effective sampler");break;}
            U observation = 0;
            while (observation < observedCount && (observed[observation].image != image||observed[observation].sampler!=samplerName)) ++observation;
            const bool firstObservation = observation == observedCount;
            // At most one new entry per span; spanCount was bounded above.
            auto& actual = observed[observation];
            if (firstObservation) {
                actual.image = image;actual.sampler=samplerName;
                s.gl.bindTexture(texture2D, image);
                if(perMaterialSamplers){
                    s.gl.bindSampler(0,samplerName);I bound=0;s.gl.getIntegerv(0x8919,&bound);
                    accepted=s.check("verify actual UI sampler binding");
                    if(accepted&&static_cast<U>(bound)!=samplerName)
                        accepted=fail(d,VehicleWindowError50DS::material,"UI sampler binding differs from its retained owner");
                }
            }
            const I expected[] = {
                sampler.minification == MaterialTextureFilter50EB::nearest ? 0x2600 : 0x2601,
                sampler.magnification == MaterialTextureFilter50EB::nearest ? 0x2600 : 0x2601,
                sampler.s == MaterialTextureWrap50EB::repeat ? 0x2901 : 0x812F,
                sampler.t == MaterialTextureWrap50EB::repeat ? 0x2901 : 0x812F, 0, 0};
            for (U p = 0; accepted && p < 6; ++p) {
                if (firstObservation) {
                    actual.values[p] = -1;
                    if(perMaterialSamplers&&p<4)s.gl.getSamplerParameteriv(samplerName,parameters[p],&actual.values[p]);
                    else s.gl.getTexParameteriv(texture2D, parameters[p], &actual.values[p]);
                    accepted = s.check("verify per-span original UI sampler");
                }
                if (accepted && actual.values[p] != expected[p])
                    accepted = fail(d, VehicleWindowError50DS::material,
                        perMaterialSamplers?"UI span sampler/image parameters differ from actual retained owners":
                            "UI span source sampler differs from immutable bank texture parameters");
            }
            if (firstObservation && accepted) ++observedCount;
        }
        if(perMaterialSamplers){s.gl.bindSampler(0,0);const bool restored=s.check("restore sampler0 after UI binding preflight");accepted=accepted&&restored;}
    }
    // Observation only, after authenticated frame/index/sampler preflight.
    // It cannot grant readiness, modify native diagnostics, or alter drawing.
    const char* disjointTrace=std::getenv("RR_TRACE_UI_COVERAGE_DISJOINT");
    if(accepted&&sourceRgba&&anyCoverage&&!s.uiCoverageDisjointTraced&&
       disjointTrace&&std::strcmp(disjointTrace,"1")==0){
        s.uiCoverageDisjointTraced=true;
        const auto traceStart=Storage::UiCoverageClock::now();
        struct Totals {std::uint64_t current=0,proposed=0,area=0,unionArea=0,rectangleArea=0,
            overlap=0,cap=0,empty=0,full=0,wFallback=0,xyFallback=0,invalid=0,comparisons=0;
            U maxGroup=0,rows=0,truncated=0,nonemptySpans=0;} total;
        for(U si=0;si<input.spanCount;++si){
            const auto& span=input.spans[si];const auto& state=native.states[si];
            const auto& range=s.uiSourceIndices->spans[si];
            const bool opaque=originalUiCoverageOpaqueBatch50KG(span,state);
            const bool raster=span.materialMode==OriginalUiMaterialMode50KG::sourceDiffuse||
                span.materialMode==OriginalUiMaterialMode50KG::sourceDiffuseCompressed||
                span.materialMode==OriginalUiMaterialMode50KG::sourceSpecularMapCompressed||
                span.materialMode==OriginalUiMaterialMode50KG::sourcePrelitCompressed;
            OriginalUiCoverageDisjoint50KG measure;OriginalUiCoverageBounds50KG whole;
            std::uint64_t full=0,wFallback=0,xyFallback=0,invalid=0;
            for(U index=range.firstIndex;index<range.firstIndex+range.indexCount;index+=3){
                const U ia=s.uiSourceIndices->indices[index],ib=s.uiSourceIndices->indices[index+1],ic=s.uiSourceIndices->indices[index+2];
                OriginalUiCoverageBounds50KG r;
                if(ia>=input.quadCount*4||ib>=input.quadCount*4||ic>=input.quadCount*4||
                   !originalUiCoverageBounds50KG(input.vertices[ia],input.vertices[ib],input.vertices[ic],r)){++invalid;continue;}
                bool extremeXY=false;
                for(U vertex:{ia,ib,ic})for(U axis=0;axis<2;++axis)
                    extremeXY=extremeXY||std::fabs(input.vertices[vertex].position[axis])>1048576.f;
                if(extremeXY)++xyFallback;
                if(raster&&(!originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ia])||
                   !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ib])||
                   !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ic]))){r={0,0,640,448};++wFallback;}
                if(r.x==0&&r.y==0&&r.width==640&&r.height==448)++full;
                if(!measure.add(r)){++invalid;continue;}
                if(!r.empty()){
                    if(whole.empty())whole=r;
                    else {const I right=std::max(whole.x+whole.width,r.x+r.width),top=std::max(whole.y+whole.height,r.y+r.height);
                        whole.x=std::min(whole.x,r.x);whole.y=std::min(whole.y,r.y);
                        whole.width=right-whole.x;whole.height=top-whole.y;}
                }
            }
            measure.finish();
            const std::uint64_t current=opaque?std::uint64_t(!whole.empty()):measure.nonempty;
            const std::uint64_t proposed=opaque?current:measure.groups;
            const std::uint64_t area=opaque?std::uint64_t(whole.width)*whole.height:measure.rectangleArea;
            const std::uint64_t unionArea=opaque?area:measure.unionArea;
            total.current+=current;total.proposed+=proposed;total.area+=area;total.unionArea+=unionArea;
            total.rectangleArea+=measure.rectangleArea;total.overlap+=opaque?0:measure.overlapFlushes;
            total.cap+=opaque?0:measure.capFlushes;total.empty+=measure.empty;total.full+=full;
            total.wFallback+=wFallback;total.xyFallback+=xyFallback;total.invalid+=invalid;
            total.comparisons+=measure.comparisons;total.maxGroup=std::max(total.maxGroup,opaque?0u:measure.maxGroup);
            if(current||invalid){++total.nonemptySpans;
                if(total.rows<256){++total.rows;
                    std::fprintf(stderr,"ui_coverage_disjoint_span span=%u cap=64 coverage=%u blend=%u depth=%u depth_write=%u material=%u opaque_unchanged=%u triangles=%u current=%llu proposed=%llu rectangle_area=%llu current_area=%llu proposed_union_area=%llu overlap_flushes=%llu cap_flushes=%llu empty=%llu full=%llu xy_fallback=%llu w_fallback=%llu invalid=%llu max_group=%u\n",
                        si,unsigned(state.coverage),unsigned(span.blend),unsigned(state.depth),unsigned(state.depthWrite),unsigned(span.materialMode),unsigned(opaque),range.indexCount/3,
                        (unsigned long long)current,(unsigned long long)proposed,(unsigned long long)measure.rectangleArea,(unsigned long long)area,(unsigned long long)unionArea,
                        (unsigned long long)(opaque?0:measure.overlapFlushes),(unsigned long long)(opaque?0:measure.capFlushes),(unsigned long long)measure.empty,
                        (unsigned long long)full,(unsigned long long)xyFallback,(unsigned long long)wFallback,(unsigned long long)invalid,opaque?0u:measure.maxGroup);
                }else ++total.truncated;
            }
        }
        std::fprintf(stderr,"ui_coverage_disjoint_total cap=64 spans=%u nonempty_spans=%u logged_rows=%u truncated_rows=%u current=%llu proposed=%llu rectangle_area=%llu current_area=%llu proposed_union_area=%llu overlap_flushes=%llu cap_flushes=%llu empty=%llu full=%llu xy_fallback=%llu w_fallback=%llu invalid=%llu comparisons=%llu max_group=%u analysis_wall_ns=%llu scope=diagnostic_hypothetical_all_mixed_baseline\n",
            input.spanCount,total.nonemptySpans,total.rows,total.truncated,(unsigned long long)total.current,(unsigned long long)total.proposed,
            (unsigned long long)total.rectangleArea,(unsigned long long)total.area,(unsigned long long)total.unionArea,(unsigned long long)total.overlap,
            (unsigned long long)total.cap,(unsigned long long)total.empty,(unsigned long long)total.full,(unsigned long long)total.xyFallback,
            (unsigned long long)total.wFallback,(unsigned long long)total.invalid,(unsigned long long)total.comparisons,total.maxGroup,
            (unsigned long long)Storage::uiCoverageNs(traceStart));
    }
    if(accepted&&anyCoverage){
        accepted=s.prepareUiCoverage("active_submit");
    }
    if (accepted) {
        s.gl.bindFramebuffer(framebuffer,sourceRgba?s.uiSourceTarget:s.uiTargets[pending]);
        if(sourceRgba){
            s.gl.disable(0x0BF2); // No inherited color-logic operation on raw source bytes.
            if(s.gl.clipControl)s.gl.clipControl(0x8CA1,0x935E); // Conventional lower-left/-1..1.
        }
        s.gl.drawBuffer(colorAttachment); s.gl.readBuffer(colorAttachment);
        const I rasterWidth=sourceRgba?640:width,rasterHeight=sourceRgba?448:height;
        s.gl.viewport(0,0,rasterWidth,rasterHeight);
        constexpr U disabled[] = {0x0BE2, 0x0B44, 0x0C11, 0x0B90, 0x8037, 0x0B20,
            0x8F9D, 0x0BD0, 0x809D, 0x80A0, 0x809E, 0x8C89, 0x8DB9, 0x864F};
        for (U capability : disabled) s.gl.disable(capability);
        for (I i = 0; i < s.clipDistances; ++i) s.gl.disable(0x3000 + static_cast<U>(i));
        s.gl.colorMask(1, 1, 1, 1); s.gl.depthMask(1);
        s.gl.depthRange(0.0, 1.0); s.gl.clearDepth(0.0);
        s.gl.polygonMode(0x0408, 0x1B02);
        s.gl.clearColor(input.clearRgba[0],input.clearRgba[1],input.clearRgba[2],sourceRgba?128.f/255.f:1.f);
        //Actual mid-Camera24B7B0 must preserve all preceding callback draws.
        //Only a fresh complete-frame invocation owns this old clear path.
        if(!garageBatch)s.gl.clear(colorBufferBit | depthBufferBit);
        if(sourceRgba&&!garageBatch){s.uiCoverageSourceDiagnostic=anyCoverage;++s.uiSourceTargetGeneration;s.uiSourceAcceptedTargetGeneration=0;
            s.uiSourcePublishedTargetGeneration=0;}

        if (input.retainRaceImage) {
            // Preserve the race aspect ratio too; blit bounds are whole pixels.
            const double fit = std::min(double(width) / s.raceWidth, double(height) / s.raceHeight);
            const I rw = std::max<I>(1, static_cast<I>(std::floor(s.raceWidth * fit)));
            const I rh = std::max<I>(1, static_cast<I>(std::floor(s.raceHeight * fit)));
            const I x = (width - rw) / 2, y = (height - rh) / 2;
            s.gl.bindFramebuffer(readFramebuffer, s.targets[s.published]); s.gl.readBuffer(colorAttachment);
            s.gl.blitFramebuffer(0, 0, s.raceWidth, s.raceHeight, x, y, x + rw, y + rh, colorBufferBit, 0x2600);
            s.gl.bindFramebuffer(framebuffer, s.uiTargets[pending]);
            // Blit ignores color masks. Reset only presentation alpha afterward.
            s.gl.colorMask(0, 0, 0, 1); s.gl.clear(colorBufferBit);
        }
        s.gl.colorMask(1,1,1,sourceRgba?1:0);
        const double scale = std::min(double(rasterWidth)/640.0,double(rasterHeight)/448.0);
        const double ox=(rasterWidth-640.0*scale)*0.5,oy=(rasterHeight-448.0*scale)*0.5;
        // One scale for both axes. Raster scissor encloses fractional boundaries;
        // shader geometry is never rounded, stretched, or half-pixel adjusted.
        const I sx = static_cast<I>(std::floor(ox)), sy = static_cast<I>(std::floor(oy));
        const I ex = std::min<I>(rasterWidth, static_cast<I>(std::ceil(ox + 640.0 * scale)));
        const I ey = std::min<I>(rasterHeight, static_cast<I>(std::ceil(oy + 448.0 * scale)));
        s.gl.scissor(sx, sy, ex - sx, ey - sy); s.gl.enable(0x0C11);
        s.resetNativeGcLighting();
        s.gl.enable(0x0B71); s.gl.useProgram(s.uiProgram);
        const float canvasToClip[4]{float(2.0*scale/rasterWidth),float(-2.0*scale/rasterHeight),
            float(2.0*ox/rasterWidth-1.0),float(1.0-2.0*oy/rasterHeight)};
        s.gl.uniform4f(s.uiCanvasUniform,canvasToClip[0],canvasToClip[1],canvasToClip[2],canvasToClip[3]);
        if(s.menuRasterShared50KG&&!s.check("reapply same native menu source raster before spans")){
            s.menuRasterFailed50KG=true;s.diagnostic=nullptr;return false;
        }
        s.gl.uniform1i(s.gcParticleModeUniform,0);
        s.gl.uniform1i(s.uiSourceCompositionUniform,sourceRgba?1:0);
        s.gl.uniform1i(s.uiCoveragePayloadUniform,0);
        if(!s.uiSourceCoherentFramebufferFetch)s.gl.uniform1i(s.uiSourceTargetUniform,1);
        if(sourceRgba && !s.uiSourceCoherentFramebufferFetch){
            s.gl.activeTexture(textureUnit0+1);if(s.gl.bindSampler)s.gl.bindSampler(1,0);
            s.gl.bindTexture(texture2D,s.uiSourceColor);
        }
        s.gl.activeTexture(textureUnit0); if (s.gl.bindSampler) s.gl.bindSampler(0, 0);
        s.gl.uniform1i(s.uiImageUniform, 0);
        s.gl.uniform4f(s.uiParticleFogColorUniform, native.particleFogColor[0],
            native.particleFogColor[1], native.particleFogColor[2], 1.f);
        s.drawGl.bindVertexArray(s.uiVertexArray); s.drawGl.bindBuffer(arrayBuffer, s.uiVertexBuffer);
        if(directVideo){
            // TEST=31001: alpha NEVER/FB_ONLY, depth ALWAYS; source writes
            // RGBA but not Z. ALPHA=(Cd-0)*As/128+Cs with Cs=As=0 is exactly
            // zero, independent of destination. This is an actual queued sprite
            // draw, not the frame clear and not a fabricated material pass.
            const assets::Vertex48B vertices[4]={
                {{0,0,0},{0,0},{0,0,0,0}},{{0,448,0},{0,0},{0,0,0,0}},
                {{640,0,0},{0,0},{0,0,0,0}},{{640,448,0},{0,0},{0,0,0,0}}};
            s.gl.disable(0x0BE2);s.gl.depthFunc(0x0207);s.gl.depthMask(0);
            s.gl.uniform1i(s.uiTexturedUniform,0);s.gl.uniform1i(s.uiSourceRasterUniform,0);
            s.gl.uniform1i(s.uiSourceFogEnabledUniform,0);s.gl.uniform1i(s.uiSourceAlphaTestUniform,0);
            s.gl.uniform1i(s.uiSourceBlendUniform,0);s.gl.uniform1f(s.uiAlphaUniform,1.f);
            s.gl.bindTexture(texture2D,0);
            if(!s.uiSourceCoherentFramebufferFetch){s.gl.activeTexture(textureUnit0+1);
                s.gl.bindTexture(texture2D,0);s.gl.activeTexture(textureUnit0);}
            s.drawGl.bufferSubData(arrayBuffer,0,sizeof(vertices),vertices);
            if(sourceWorkStarted)*sourceWorkStarted=true;
            s.gl.drawArrays(0x0005,0,4);
            accepted=s.check("source direct video 25E610 before material drain");
            if(accepted){++d.originalUiDraws;d.originalUiSourceTriangles+=2;}
            // Subsequent spans establish their own depth/blend/material state;
            // all source vertex/index/auxiliary streams are restored below.
            // Fallback destination sampling is rebound by each source span;
            // zero-source direct output must not sample the attached target.
        }
        s.drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(input.quadCount) * 4 * sizeof(assets::Vertex48B), input.vertices);
        if (anyFog) {
            s.drawGl.bindBuffer(arrayBuffer, s.uiParticleFogBuffer);
            s.drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(native.particleFogCount) * sizeof(float), native.particleFog);
        }
        if (anySourceRaster) {
            s.drawGl.bindBuffer(arrayBuffer, s.uiSourceRasterWBuffer);
            s.drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(native.sourceRasterWCount) * sizeof(float), native.sourceRasterW);
        }
        if (anySourcePacketStq) {
            s.drawGl.bindBuffer(arrayBuffer, s.uiSourcePacketStqBuffer);
            s.drawGl.bufferSubData(arrayBuffer, 0,
                std::intptr_t(native.sourcePacketStqCount) * sizeof(std::array<float,3>), native.sourcePacketStq);
        }
        s.drawGl.bindBuffer(elementBuffer, sourceRgba ? s.uiSourceIndexBuffer : s.uiIndexBuffer);
        if (sourceRgba)
            s.drawGl.bufferSubData(elementBuffer, 0, std::intptr_t(extendedIndexCount) * sizeof(U),
                s.uiSourceIndices->indices);
        accepted = s.check("prepare authenticated original UI frame") && accepted;
        // No material representative has bound before this point. Read-only
        // preflight/context/window failures remain retryable with the same
        // composer generation. A later partial source pass failure is unknown.
        if(sourceRgba&&accepted&&input.spanCount&&sourceWorkStarted)*sourceWorkStarted=true;
        preDrawDiagnosticNs=Storage::uiCoverageNs(submitStageStart);
        const auto coverageDrawStart=Storage::UiCoverageClock::now();
        bool coverageMixed=false;U coveragePrefixDraws=0,coverageMixedDraws=0,coverageEmptyAa1Spans=0;
        std::uint64_t coveragePromotionNs=0;
        for (U i = 0; accepted && i < input.spanCount; ++i) {
            const auto& span = input.spans[i]; const auto& state = native.states[i];
            const U depth = state.depth == OriginalUiDepth50KG::never ? 0x0200 :
                state.depth == OriginalUiDepth50KG::always ? 0x0207 :
                state.depth == OriginalUiDepth50KG::greaterEqual ? 0x0206 : 0x0204;
            s.gl.depthFunc(depth); s.gl.depthMask(state.depthWrite ? 1 : 0);
            if (sourceRgba || span.blend == OriginalUiBlend50KG::opaque) s.gl.disable(0x0BE2);
            else {
                s.gl.enable(0x0BE2); s.gl.blendEquation(0x8006);
                s.gl.blendFunc(0x0302, span.blend == OriginalUiBlend50KG::additive ? 1 : 0x0303);
            }
            s.gl.uniform1f(s.uiAlphaUniform, span.alphaScale);
            const I sourceBlend=span.blend==OriginalUiBlend50KG::opaque?0:
                span.blend==OriginalUiBlend50KG::sourceAlpha?1:
                span.blend==OriginalUiBlend50KG::additive?2:
                span.blend==OriginalUiBlend50KG::destinationAlphaAdditive?3:
                span.blend==OriginalUiBlend50KG::fixed255Additive?4:
                span.blend==OriginalUiBlend50KG::fixed64Additive?6:5;
            s.gl.uniform1i(s.uiSourceBlendUniform,sourceBlend);
            const bool particle = span.materialMode == OriginalUiMaterialMode50KG::mainParticle;
            const bool sourceRaster = span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuse ||
                span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuseCompressed ||
                span.materialMode == OriginalUiMaterialMode50KG::sourceSpecularMapCompressed ||
                span.materialMode == OriginalUiMaterialMode50KG::sourcePrelitCompressed;
            const bool textured = span.materialMode == OriginalUiMaterialMode50KG::texturedModulate || particle || sourceRaster;
            s.gl.uniform1i(s.uiTexturedUniform, textured ? 1 : 0);
            s.gl.uniform1i(s.uiSourceRasterUniform, sourceRaster ? (state.sourcePacketStq ? 2 : 1) : 0);
            s.gl.uniform1i(s.uiSourceFogEnabledUniform, (particle || state.sourceFogEnabled) ? 1 : 0);
            s.gl.uniform1i(s.uiSourceAlphaTestUniform, (particle || state.sourceAlphaTestGreaterZero) ? 1 : 0);
            s.gl.bindTexture(texture2D, textured ? input.textures->texture(span.textureIndex) : 0);
            const U sourceSampler=perMaterialSamplers&&textured?s.garageSampler50KG(state.sampler):0;
            if(perMaterialSamplers)s.gl.bindSampler(0,sourceSampler);
            if(sourceRgba&&anyCoverage&&!coverageMixed&&state.coverage==OriginalUiCoverage50KG::fourSampleAa1){
                if(i<firstCoveredAa1Span){
                    // All retained bounds in this AA1 span were proven empty.
                    // Keep original retained-triangle/quad accounting intact.
                    ++coverageEmptyAa1Spans;
                    d.originalUiSourceTriangles+=s.uiSourceIndices->spans[i].indexCount/3;
                    d.originalUiQuads+=span.quadCount;continue;
                }
                const auto promotionStart=Storage::UiCoverageClock::now();
                accepted=s.promoteUiCoveragePrefix();coveragePromotionNs=Storage::uiCoverageNs(promotionStart);
                if(!s.uiCoveragePromotionReported){s.uiCoveragePromotionReported=true;
                    std::fprintf(stderr,"ui_coverage_first_promotion target=%llu span=%u spans=%u prefix_draws=%u promotion_wall_ns=%llu accepted=%u scope=cpu_blit_call_no_added_wait\n",
                        (unsigned long long)s.uiSourceTargetGeneration,i,input.spanCount,coveragePrefixDraws,
                        (unsigned long long)coveragePromotionNs,unsigned(accepted));}
                if(!accepted)break; // Existing partial-work receipt forbids publication.
                coverageMixed=true; // Never collapse again, even for later AA0.
                s.gl.bindTexture(texture2D,textured?input.textures->texture(span.textureIndex):0);
            }
            if(sourceRgba&&coverageMixed){
                const auto& range=s.uiSourceIndices->spans[i];U draws=0;
                const U image=textured?input.textures->texture(span.textureIndex):0;
                const bool opaqueBatch=originalUiCoverageOpaqueBatch50KG(span,state);
                OriginalUiCoverageBounds50KG spanRegion;
                accepted=s.beginUiCoverageSpan(state.coverage==OriginalUiCoverage50KG::fourSampleAa1,sourceBlend);
                // Non-opaque ranges: consecutive source triangles whose
                // conservative regions (half-open, halo included) are pairwise
                // disjoint share one scratch clear/raster/snapshot/compose
                // range over their union rectangle. No sample is touched by two
                // triangles of one range, uncovered union samples keep the
                // cleared A=-1 and are discarded, and every overlapping pair
                // stays in separate ranges in source order: per-sample results
                // equal one range per triangle. RR_GARAGE_COVERAGE_PER_TRIANGLE=1
                // restores one range per triangle.
                static const bool perTriangle=[]{const char* v=std::getenv("RR_GARAGE_COVERAGE_PER_TRIANGLE");return v&&v[0]=='1'&&!v[1];}();
                // Verification harness, off by default (RR_GARAGE_COVERAGE_VERIFY=1):
                // every non-opaque coverage span is drawn one range per triangle,
                // resolved and read back, the sample domain is restored, the span
                // is drawn batched, and both resolved images are compared byte-for-
                // byte. The batched result is kept.
                static const bool verifyBatches=[]{const char* v=std::getenv("RR_GARAGE_COVERAGE_VERIFY");return v&&v[0]=='1'&&!v[1];}();
                constexpr U batchCap=64;
                OriginalUiCoverageBounds50KG batchRects[batchCap],batchBounds;U batchCount=0,batchFirst=0;std::uint64_t batchArea=0;
                const auto flushBatch=[&]()noexcept{
                    if(batchCount&&accepted){
                        accepted=s.drawUiCoverageRange(state.coverage==OriginalUiCoverage50KG::fourSampleAa1,
                            batchFirst,3*batchCount,image,depth,state.depthWrite,sourceBlend,false,batchBounds,sourceSampler);
                        if(accepted)++draws;
                    }
                    batchCount=0;batchArea=0;batchBounds={};
                };
                const auto runSpan=[&](bool allowJoin)noexcept{
                for(U index=range.firstIndex;accepted&&index<range.firstIndex+range.indexCount;index+=3){
                    const U ia=s.uiSourceIndices->indices[index],ib=s.uiSourceIndices->indices[index+1],
                        ic=s.uiSourceIndices->indices[index+2];
                    OriginalUiCoverageBounds50KG region;
                    if(ia>=input.quadCount*4||ib>=input.quadCount*4||ic>=input.quadCount*4||
                        !originalUiCoverageBounds50KG(input.vertices[ia],input.vertices[ib],input.vertices[ic],region)){
                        accepted=fail(d,VehicleWindowError50DS::frame,"invalid original UI coverage triangle bounds");break;
                    }
                    // Extreme positive W may under/overflow clipXY*W in the
                    // source shader. Preserve the full domain rather than infer
                    // a narrow XY bound; geometry/coverage policy is unchanged.
                    if(sourceRaster){
                        if(!originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ia])||
                           !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ib])||
                           !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ic]))region={0,0,640,448};
                    }
                    // Only a clipped-empty conservative region omits GPU work.
                    // Source order, indices, coverage, ALPHA and depth stay intact.
                    if(region.empty()){flushBatch();continue;} // keeps every range contiguous
                    if(opaqueBatch){
                        if(spanRegion.empty())spanRegion=region;
                        else {const I right=std::max(spanRegion.x+spanRegion.width,region.x+region.width);
                            const I top=std::max(spanRegion.y+spanRegion.height,region.y+region.height);
                            spanRegion.x=std::min(spanRegion.x,region.x);spanRegion.y=std::min(spanRegion.y,region.y);
                            spanRegion.width=right-spanRegion.x;spanRegion.height=top-spanRegion.y;}
                    }else{
                        const std::uint64_t area=std::uint64_t(region.width)*region.height;
                        OriginalUiCoverageBounds50KG joined=region;
                        bool join=allowJoin&&batchCount>0&&batchCount<batchCap&&batchFirst+3*batchCount==index;
                        for(U k=0;join&&k<batchCount;++k){const auto& a=batchRects[k];
                            if(region.x<a.x+a.width&&a.x<region.x+region.width&&
                               region.y<a.y+a.height&&a.y<region.y+region.height)join=false;}
                        if(join){
                            const I right=std::max(batchBounds.x+batchBounds.width,region.x+region.width);
                            const I top=std::max(batchBounds.y+batchBounds.height,region.y+region.height);
                            joined.x=std::min(batchBounds.x,region.x);joined.y=std::min(batchBounds.y,region.y);
                            joined.width=right-joined.x;joined.height=top-joined.y;
                            // Bound the extra scratch/compose area of the union.
                            if(std::uint64_t(joined.width)*joined.height>2*(batchArea+area)+16384u)join=false;
                        }
                        if(!join){flushBatch();if(!accepted)break;joined=region;batchFirst=index;}
                        batchBounds=joined;batchRects[batchCount++]=region;batchArea+=area;
                    }
                }
                if(!opaqueBatch)flushBatch();
                };
                if(verifyBatches&&!opaqueBatch&&accepted){
                    struct Verify{const void* owner=nullptr;bool ready=false,failed=false;
                        U save=0,saveColor=0,saveDepth=0,resolve=0,resolveColor=0;std::vector<std::uint8_t> a,b;};
                    thread_local Verify v;
                    if(v.owner!=&s){v=Verify{};v.owner=&s;}
                    if(!v.ready&&!v.failed){
                        v.failed=true;
                        try{v.a.resize(640u*448u*4u);v.b.resize(640u*448u*4u);}catch(...){}
                        if(v.b.size()==640u*448u*4u&&
                           s.coverageFramebuffer(v.save)&&s.coverageTextureStorage(v.saveColor,rgba8,640,448,true,colorAttachment)&&
                           s.coverageTextureStorage(v.saveDepth,depth24,640,448,true,depthAttachment)){
                            s.gl.drawBuffer(colorAttachment);s.gl.readBuffer(colorAttachment);
                            if(s.coverageComplete()&&s.coverageFramebuffer(v.resolve)&&
                               s.coverageTextureStorage(v.resolveColor,rgba8,640,448,false,colorAttachment)){
                                s.gl.drawBuffer(colorAttachment);s.gl.readBuffer(colorAttachment);
                                if(s.coverageComplete()){v.ready=true;v.failed=false;}
                            }
                        }
                        s.gl.bindFramebuffer(framebuffer,s.uiCoverage.scene);
                        std::fprintf(stderr,"garage_coverage_verify_setup ready=%u\n",unsigned(v.ready));
                    }
                    if(v.ready){
                        const auto blit=[&](U from,U to,U bits)noexcept{
                            s.gl.disable(0x0C11);
                            s.gl.bindFramebuffer(readFramebuffer,from);s.gl.bindFramebuffer(drawFramebuffer,to);
                            s.gl.blitFramebuffer(0,0,640,448,0,0,640,448,bits,0x2600);
                            s.gl.bindFramebuffer(framebuffer,s.uiCoverage.scene);s.gl.enable(0x0C11);
                        };
                        const auto read=[&](std::vector<std::uint8_t>& out)noexcept{
                            blit(s.uiCoverage.scene,v.resolve,colorBufferBit);
                            s.gl.bindFramebuffer(readFramebuffer,v.resolve);s.drawGl.bindBuffer(0x88EB,0);
                            s.gl.readPixels(0,0,640,448,rgba,unsignedByte,out.data());
                            s.gl.bindFramebuffer(framebuffer,s.uiCoverage.scene);
                        };
                        blit(s.uiCoverage.scene,v.save,colorBufferBit|depthBufferBit);
                        const U before=draws;runSpan(false);const U single=draws-before;
                        if(accepted){
                            read(v.a);
                            blit(v.save,s.uiCoverage.scene,colorBufferBit|depthBufferBit);
                            spanRegion={};const U mid=draws;runSpan(true);const U batched=draws-mid;
                            if(accepted){
                                read(v.b);std::size_t differing=0;
                                for(std::size_t k=0;k<v.a.size();++k)differing+=v.a[k]!=v.b[k];
                                std::fprintf(stderr,"garage_coverage_verify span=%u aa1=%u blend=%d triangles=%u ranges_per_triangle=%u ranges_batched=%u differing_bytes=%zu\n",
                                    i,unsigned(state.coverage==OriginalUiCoverage50KG::fourSampleAa1),sourceBlend,range.indexCount/3,single,batched,differing);
                            }
                        }
                        accepted=s.check("garage coverage verification")&&accepted;
                    }else runSpan(!perTriangle);
                }else runSpan(!perTriangle);
                if(accepted&&opaqueBatch&&!spanRegion.empty()){
                    // Uniform opaque AA0 GE/GREATER writing span: source-order
                    // center-depth reduction selects the same surviving payload
                    // for comparison against every retained destination sample.
                    accepted=s.drawUiCoverageRange(false,range.firstIndex,range.indexCount,image,depth,
                        true,0,true,spanRegion,sourceSampler);if(accepted)++draws;
                }
                const bool spanRestored=s.endUiCoverageSpan(image,depth,state.depthWrite);
                accepted=accepted&&spanRestored;
                if(accepted){coverageMixedDraws+=draws;d.originalUiDraws+=draws;d.originalUiSourceTriangles+=range.indexCount/3;d.originalUiQuads+=span.quadCount;}
            }else if(sourceRgba){
                // Coherent fetch reads the framebuffer directly. Fallback
                // opaque writes unbind destination RGBA; only fallback feedback
                // requires one real triangle and texture barrier per draw.
                const bool fixedSpan=fixedAdditive&&garageFixedAdditiveBlend50KG(span.blend);
                const bool doubledSpecular=specularSpan(i);
                const bool feedback=span.blend!=OriginalUiBlend50KG::opaque&&!fixedSpan&&!doubledSpecular;
                if(!s.uiSourceCoherentFramebufferFetch){
                    s.gl.activeTexture(textureUnit0+1);
                    s.gl.bindTexture(texture2D,feedback?s.uiSourceColor:0);
                    s.gl.activeTexture(textureUnit0);
                }
                const auto& range=s.uiSourceIndices->spans[i];
                const U sourceTriangles=range.indexCount/3;
                U sourceDraws=0;
                if(doubledSpecular){
                    // PC/Xbox fixed-function specular (GarageFixedSpecular50KG):
                    // one draw of the consecutive copy pairs, same VAO/texture/
                    // sampler/depth state, through the dual-source variant.
                    auto& v=garageFixedSpecular50KG;
                    s.gl.useProgram(v.program);
                    s.gl.uniform4f(v.canvas,canvasToClip[0],canvasToClip[1],canvasToClip[2],canvasToClip[3]);
                    s.gl.uniform1i(v.image,0);if(v.alpha>=0)s.gl.uniform1f(v.alpha,span.alphaScale);
                    s.gl.uniform1i(v.textured,textured?1:0);
                    s.gl.uniform4f(v.fogColor,native.particleFogColor[0],native.particleFogColor[1],native.particleFogColor[2],1.f);
                    s.gl.uniform1i(v.raster,sourceRaster?(state.sourcePacketStq?2:1):0);
                    if(v.particle>=0)s.gl.uniform1i(v.particle,0);
                    s.gl.uniform1i(v.fogEnabled,(particle||state.sourceFogEnabled)?1:0);
                    s.gl.uniform1i(v.alphaTest,(particle||state.sourceAlphaTestGreaterZero)?1:0);
                    s.gl.uniform1i(v.composition,1);s.gl.uniform1i(v.payload,0);
                    if(v.target>=0)s.gl.uniform1i(v.target,1);
                    s.gl.uniform1i(v.blend,-3);
                    s.gl.enable(0x0BE2);s.gl.blendEquation(0x8006);
                    s.gl.blendFuncSeparate(0x0304,1,1,0x88FB); // RGB: DST_ALPHA, ONE; A: ONE, ONE_MINUS_SRC1_ALPHA
                    s.drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(specularCount[i]),0x1405,
                        reinterpret_cast<const void*>(std::uintptr_t(specularFirst[i])*sizeof(U)),0);
                    s.gl.disable(0x0BE2);s.gl.useProgram(s.uiProgram);sourceDraws=1;
                    ++v.draws;v.triangles+=range.indexCount/3;
                    if(v.draws==1||v.draws%2000==0)
                        std::fprintf(stderr,"garage_fixed_specular_draws draws=%llu triangles=%llu max_rgb_difference=%d\n",
                            (unsigned long long)v.draws,(unsigned long long)v.triangles,v.maxDifference);
                }else if(fixedSpan){
                    // Exact fixed-function ADD (see GarageFixedAdditive50KG):
                    // shader emits the floored product; blender adds Cd.
                    if(range.indexCount){
                        s.gl.uniform1i(s.uiSourceBlendUniform,-sourceBlend);
                        s.gl.enable(0x0BE2);s.gl.blendEquation(0x8006);s.gl.blendFuncSeparate(1,1,1,0);
                        s.drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(range.indexCount),0x1405,
                            reinterpret_cast<const void*>(std::uintptr_t(range.firstIndex)*sizeof(U)),0);
                        s.gl.disable(0x0BE2);sourceDraws=1;
                    }
                }else if(s.uiSourceCoherentFramebufferFetch || !feedback){
                    // Only retained original triples enter the batched draw;
                    // discarded equal-XY halves are never submitted to GL.
                    if(range.indexCount){
                        s.drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(range.indexCount),0x1405,
                            reinterpret_cast<const void*>(std::uintptr_t(range.firstIndex)*sizeof(U)),0);
                        sourceDraws=1;
                    }
                }else if(feedbackBatches&&fb.spanCount[i]){
                    // Region-disjoint triangle batches, one barrier each.
                    for(std::uint32_t b=0;b<fb.spanCount[i];++b){
                        const auto k=fb.spanFirst[i]+b;
                        s.gl.textureBarrier();++d.originalUiTextureBarriers;
                        s.drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(fb.rangeCount[k]),0x1405,
                            reinterpret_cast<const void*>(std::uintptr_t(fb.rangeFirst[k])*sizeof(U)),0);
                        ++sourceDraws;
                    }
                }else{
                    for(U index=range.firstIndex;index<range.firstIndex+range.indexCount;index+=3){
                        s.gl.textureBarrier();++d.originalUiTextureBarriers;
                        s.drawGl.drawElementsBaseVertex(0x0004,3,0x1405,
                            reinterpret_cast<const void*>(std::uintptr_t(index)*sizeof(U)),0);
                        ++sourceDraws;
                    }
                }
                // GL errors remain sticky until this span boundary. A failed
                // span publishes no successful counters; partial source work
                // still poisons its pending history through the existing receipt.
                accepted=s.check("draw ordered source-RGBA span");
                if(accepted){coveragePrefixDraws+=sourceDraws;d.originalUiDraws+=sourceDraws;d.originalUiSourceTriangles+=sourceTriangles;
                    d.originalUiQuads+=span.quadCount;}
            }else{
                s.drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.quadCount * 6), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(span.firstQuad) * 6 * sizeof(U)), 0);
                accepted = s.check("draw ordered original UI span");
                if (accepted) { ++d.originalUiDraws; d.originalUiQuads += span.quadCount; }
            }
        }
        if(sourceRgba && !s.uiSourceCoherentFramebufferFetch){
            s.gl.activeTexture(textureUnit0+1);s.gl.bindTexture(texture2D,0);
            if(s.gl.bindSampler)s.gl.bindSampler(1,0);
            s.gl.activeTexture(textureUnit0);
        }
        if(anyCoverage){
            const auto drawNs=Storage::uiCoverageNs(coverageDrawStart);const auto resolveStart=Storage::UiCoverageClock::now();
            if(accepted&&coverageMixed)accepted=s.resolveUiCoverage();
            const auto resolveNs=Storage::uiCoverageNs(resolveStart);
            drawDiagnosticNs=drawNs;resolveDiagnosticNs=resolveNs;
            diagnosticPrefixDraws=coveragePrefixDraws;diagnosticMixedDraws=coverageMixedDraws;
            diagnosticEmptyAa1=coverageEmptyAa1Spans;diagnosticPromoted=coverageMixed;diagnosticPromotionNs=coveragePromotionNs;
            s.unbindUiCoverageInputs();s.gl.disable(0x809D);
            if(s.uiCoverageDrawReports<8||!accepted){++s.uiCoverageDrawReports;
                std::fprintf(stderr,"ui_coverage_active target=%llu source_draws=%u triangles=%u ordered_draw_wall_ns=%llu resolve_wall_ns=%llu accepted=%u prefix_draws=%u mixed_draws=%u empty_aa1_spans=%u promoted=%u coherent_fetch=%u promotion_wall_ns=%llu scope=cpu_submission_no_gpu_finish\n",
                    static_cast<unsigned long long>(s.uiSourceTargetGeneration),d.originalUiDraws,d.originalUiSourceTriangles,
                    static_cast<unsigned long long>(drawNs),static_cast<unsigned long long>(resolveNs),unsigned(accepted),
                    coveragePrefixDraws,coverageMixedDraws,coverageEmptyAa1Spans,unsigned(coverageMixed),unsigned(s.uiSourceCoherentFramebufferFetch),
                    static_cast<unsigned long long>(coveragePromotionNs));}
        }
        // Restore renderer-owned baseline state even on draw failure. The next
        // world begin supplies its own program, matrix, viewport and geometry.
        s.gl.disable(0x0C11); s.gl.disable(0x0BE2); s.gl.depthMask(1); s.gl.depthFunc(0x0201);
        s.gl.colorMask(1, 1, 1, 1); s.gl.clearDepth(1.0);
        // Element binding belongs to the VAO. Restore its immutable RGB bank
        // while the UI VAO is still current, including after source failures.
        if(sourceRgba)s.drawGl.bindBuffer(elementBuffer,s.uiIndexBuffer);
        s.drawGl.bindVertexArray(0); s.drawGl.bindBuffer(arrayBuffer, 0);
        const bool restored = s.check("finish private original UI frame");
        accepted = accepted && restored;
        if (accepted && sourceRgba){
            d.originalUiSourceAccepted=!garageBatch;
            if(publishSource){const auto publishStart=Storage::UiCoverageClock::now();
                accepted=s.publishUiSource(input.clearRgba,width,height);
                publishDiagnosticNs=Storage::uiCoverageNs(publishStart);
                if(anyCoverage&&(s.uiCoveragePublishReports<8||!accepted)){++s.uiCoveragePublishReports;
                    std::fprintf(stderr,"ui_coverage_publication target=%llu publish_wall_ns=%llu accepted=%u scope=existing_source_and_window_copy_no_gpu_finish\n",
                        static_cast<unsigned long long>(s.uiSourceTargetGeneration),static_cast<unsigned long long>(Storage::uiCoverageNs(publishStart)),unsigned(accepted));}
            }
        }else if (accepted) {
            s.uiPrivatePublicationGeneration=0;
            s.uiSourcePublishedTargetGeneration=0;
            s.uiPublished = pending; s.displayedTarget = s.uiTargets[pending];
            s.width = s.committedWidth = width; s.height = s.committedHeight = height;
            s.imageCommitted = true; d.originalUiAccepted = true;
            if (s.request.output == VehicleWindowOutput50DS::defaultBackBuffer) {
                accepted = s.copyToWindow();
                if (!accepted && d.error == VehicleWindowError50DS::gl) d.error = VehicleWindowError50DS::presentation;
            }
        }
    }
    // Also restore after rejected preflight, which never entered the draw body.
    // This precedes the sole return and never clears batch/GL failure history.
    if(perMaterialSamplers&&s.gl.bindSampler){s.gl.bindSampler(0,0);const bool restored=s.check("restore sampler0 after UI submission");accepted=accepted&&restored;}
    const auto stageTotalNs=Storage::uiCoverageNs(submitStageStart);
    if(anyCoverage&&stageTotalNs>=100000000ull&&s.uiCoverageSlowReports<8){++s.uiCoverageSlowReports;
        std::fprintf(stderr,"ui_coverage_slow_stages target=%llu total_wall_ns=%llu pre_draw_wall_ns=%llu ordered_draw_wall_ns=%llu resolve_wall_ns=%llu publication_wall_ns=%llu first_aa1_span=%u spans=%u prefix_draws=%u mixed_draws=%u empty_aa1_spans=%u promoted=%u promotion_wall_ns=%llu accepted=%u scope=cpu_stage_times_no_added_wait\n",
            (unsigned long long)s.uiSourceTargetGeneration,(unsigned long long)stageTotalNs,
            (unsigned long long)preDrawDiagnosticNs,(unsigned long long)drawDiagnosticNs,
            (unsigned long long)resolveDiagnosticNs,(unsigned long long)publishDiagnosticNs,
            firstCoveredAa1Span,input.spanCount,diagnosticPrefixDraws,diagnosticMixedDraws,diagnosticEmptyAa1,
            unsigned(diagnosticPromoted),(unsigned long long)diagnosticPromotionNs,unsigned(accepted));
    }
    s.diagnostic = nullptr;
    return accepted;
}


} // namespace rr::platform
