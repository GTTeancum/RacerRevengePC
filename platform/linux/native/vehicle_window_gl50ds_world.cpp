#include "vehicle_window_gl50ds_storage.h"
#include <cstdio>
#include <cstdlib>
#include "raster_shared_edge.h"
#include "../../../tests/native/rc_crystal50rc.h"
#include "../../../tests/native/rc_particle_visual50rc.h"
#include "../../../tests/native/rc_particle_render50rc.h"
#include "../../../tests/native/gb_particle_visual50gb.h"
#include "../../../tests/native/mc_particle_visual50mc.h"
#include "../../../tests/native/gb_particle_render50gb.h"
#include "../../../tests/native/mc_particle_render50mc.h"
#include "../../../tests/native/ra_cold_aggregate50la.h"
#include "../../../tests/native/ra_static_fog_numeric50la.h"

namespace rr::platform {
#include "vehicle_window_gl50ds_local_flare.inl"
bool VehicleWindowGl50DS::Storage::hasCoverageResources() const noexcept {
        return coverageLive.owns()||coverageSpare.owns()||coverageProgram||
            coverageVertex||coverageFragment||coverageArray||pairCompare.colorPre||pairCompare.depthPre||
            pairCompare.colorBridge||pairCompare.colorFixed||pairCompare.resultTexture||
            pairCompare.resultFramebuffer||pairCompare.program||pairCompare.vertex||pairCompare.fragment;
    }

bool VehicleWindowGl50DS::Storage::checkCoverageSupport(I major,I minor) noexcept {
        gl.copyImageSubData=nullptr;
        if(!coverageDemand)return true;
        if(major<4)return fail(*diagnostic,VehicleWindowError50DS::context,
            "AA1 native coverage requires desktop GL4.0 and GLSL400; no fallback");
        if(!load(gl.texImage2DMultisample,"glTexImage2DMultisample")||
           !load(gl.drawBuffers,"glDrawBuffers")||!load(gl.clearBufferfv,"glClearBufferfv")||
           !load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
           !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
           !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||
           !load(gl.framebufferTexture2D,"glFramebufferTexture2D")||
           !load(gl.blitFramebuffer,"glBlitFramebuffer")||!load(gl.drawArrays,"glDrawArrays"))return false;
        I samples=0,colorSamples=0,depthSamples=0,units=0,buffers=0,attachments=0;
        gl.getIntegerv(0x8D57,&samples);gl.getIntegerv(0x910E,&colorSamples);
        gl.getIntegerv(0x910F,&depthSamples);gl.getIntegerv(0x8872,&units);
        gl.getIntegerv(0x8824,&buffers);gl.getIntegerv(0x8CDF,&attachments);
        if(!check("query strict native coverage capability limits"))return false;
        if(samples<4||colorSamples<4||depthSamples<4||units<3||buffers<2||attachments<2)
            return fail(*diagnostic,VehicleWindowError50DS::limits,"four-sample coverage/AA0 bridge limits unavailable");
        // This optional transfer route needs core capability AND a proc.
        // Do not use mandatory load(): a missing proc retains the exact blit.
        if(major>4||(major==4&&minor>=3)){
            gl.copyImageSubData=reinterpret_cast<CopyImageSubData50DS>(
                request.procAddress(request.context,"glCopyImageSubData"));
            if(!check("resolve optional core coverage image copy"))return false;
        }
        return true;
    }

void VehicleWindowGl50DS::Storage::bindTrackCoverage(NativeTrackCoverage50DS policy) noexcept {
        if(!coverageDemand)return;
        gl.disable(0x809E);gl.disable(0x809F);gl.disable(0x80A0);
        gl.disable(0x8E51);gl.disable(0x8C36);
        if(policy==NativeTrackCoverage50DS::fourSampleAa1)gl.enable(0x809D);
        else gl.disable(0x809D);
    }

void VehicleWindowGl50DS::Storage::coverageAa0() noexcept { bindTrackCoverage(NativeTrackCoverage50DS::aa0); }

bool VehicleWindowGl50DS::Storage::deleteCoverageExtent(CoverageExtent& e,VehicleWindowDiagnostic50DS& d) noexcept {
        for(U b=0;b<2;++b)
            if(!deleteName(e.scene[b],gl.deleteFramebuffers,d)||
               !deleteName(e.color[b],gl.deleteTextures,d)||
               !deleteName(e.depth[b],gl.deleteTextures,d))return false;
        if(!deleteName(e.snapshot,gl.deleteFramebuffers,d)||!deleteName(e.snapshotColor,gl.deleteTextures,d)||
           !deleteName(e.raster,gl.deleteFramebuffers,d)||!deleteName(e.payload,gl.deleteTextures,d)||
           !deleteName(e.rasterDepth,gl.deleteTextures,d))return false;
        e.width=e.height=0;return true;
    }

bool VehicleWindowGl50DS::Storage::deleteCoverageResources(VehicleWindowDiagnostic50DS& d) noexcept {
        return deleteCoverageExtent(coverageSpare,d)&&deleteCoverageExtent(coverageLive,d)&&
            deleteShaderName(coverageProgram,true,d)&&deleteShaderName(coverageVertex,false,d)&&
            deleteShaderName(coverageFragment,false,d)&&deleteName(coverageArray,gl.deleteVertexArrays,d)&&
            deleteName(pairCompare.colorPre,gl.deleteTextures,d)&&deleteName(pairCompare.depthPre,gl.deleteTextures,d)&&
            deleteName(pairCompare.colorBridge,gl.deleteTextures,d)&&deleteName(pairCompare.colorFixed,gl.deleteTextures,d)&&
            deleteName(pairCompare.resultTexture,gl.deleteTextures,d)&&deleteName(pairCompare.resultFramebuffer,gl.deleteFramebuffers,d)&&
            deleteShaderName(pairCompare.program,true,d)&&deleteShaderName(pairCompare.vertex,false,d)&&
            deleteShaderName(pairCompare.fragment,false,d);
    }

bool VehicleWindowGl50DS::Storage::coverageTextureStorage(U& name,U format,I w,I h,bool multi,U attachment) noexcept {
        gl.genTextures(1,&name);
        if(!check("allocate native coverage texture name")||!name)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero native coverage texture");
        const U target=multi?coverageTexture:texture2D;
        gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
        gl.bindTexture(target,name);
        if(multi)gl.texImage2DMultisample(target,4,format,w,h,1);
        else {
            gl.texParameteri(target,0x2801,0x2600);gl.texParameteri(target,0x2800,0x2600);
            gl.texParameteri(target,0x2802,0x812F);gl.texParameteri(target,0x2803,0x812F);
            gl.texParameteri(target,0x813C,0);gl.texParameteri(target,0x813D,0);
            drawGl.bindBuffer(0x88EC,0);
            gl.texImage2D(target,0,static_cast<I>(format),w,h,0,format==0x822E?0x1903:rgba,0x1406,nullptr);
        }
        if(!check("allocate exact native coverage storage"))return false;
        I aw=0,ah=0,af=0,as=0,fixed=0;
        gl.getTexLevelParameteriv(target,0,0x1000,&aw);gl.getTexLevelParameteriv(target,0,0x1001,&ah);
        gl.getTexLevelParameteriv(target,0,0x1003,&af);
        if(multi){gl.getTexLevelParameteriv(target,0,0x9106,&as);gl.getTexLevelParameteriv(target,0,0x9107,&fixed);}
        if(!check("verify native coverage format extent and samples"))return false;
        if(aw!=w||ah!=h||af!=static_cast<I>(format)||(multi&&(as!=4||fixed!=1)))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"native coverage storage differs; no fallback");
        gl.framebufferTexture2D(framebuffer,attachment,target,name,0);gl.bindTexture(target,0);
        return check("attach native coverage texture");
    }

bool VehicleWindowGl50DS::Storage::coverageFramebuffer(U& name) noexcept {
        gl.genFramebuffers(1,&name);
        if(!check("allocate native coverage framebuffer name")||!name)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero native coverage framebuffer");
        gl.bindFramebuffer(framebuffer,name);return check("bind owned native coverage framebuffer");
    }

bool VehicleWindowGl50DS::Storage::coverageComplete() noexcept {
        const U status=gl.checkFramebufferStatus(framebuffer);
        if(!check("verify native coverage framebuffer completeness"))return false;
        if(status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete native coverage framebuffer");}
        return true;
    }

bool VehicleWindowGl50DS::Storage::createCoverageExtent(CoverageExtent& e) noexcept {
        I limit=0;gl.getIntegerv(0x0D33,&limit);
        if(!check("query actual-extent coverage texture limit"))return false;
        if(width<=0||height<=0||width>limit||height>limit)
            return fail(*diagnostic,VehicleWindowError50DS::limits,"native coverage extent exceeds texture limit");
        // Draw/read selectors are framebuffer object state, retained across
        // binds and extent swaps. All users keep these private assignments.
        for(U b=0;b<2;++b){
            if(!coverageFramebuffer(e.scene[b])||
               !coverageTextureStorage(e.color[b],rgba8,width,height,true,colorAttachment)||
               !coverageTextureStorage(e.depth[b],depth24,width,height,true,depthAttachment))return false;
            gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
            if(!coverageComplete())return false;
        }
        if(hasSpecularPairs50LA || request.raStaticWorld || request.track.rcCrystal){
            if(!coverageFramebuffer(e.snapshot)||
               !coverageTextureStorage(e.snapshotColor,rgba8,width,height,true,colorAttachment))return false;
            gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
            if(!coverageComplete()||!coverageFramebuffer(e.raster)||
               !coverageTextureStorage(e.payload,0x881A,width,height,false,colorAttachment)|| // RGBA16F byte payload
               !coverageTextureStorage(e.rasterDepth,0x822E,width,height,false,colorAttachment+1))return false;
            const U outputs[]={colorAttachment,colorAttachment+1};gl.drawBuffers(2,outputs);gl.readBuffer(colorAttachment);
            if(!coverageComplete())return false;
        }
        e.width=width;e.height=height;
        gl.bindFramebuffer(framebuffer,0);return check("finish unpublished coverage extent");
    }

bool VehicleWindowGl50DS::Storage::prepareCoverageExtent() noexcept {
        if(!coverageDemand)return true;
        if(pairCompare.enabled){pairCompare.eligibleLastFrame=pairCompare.eligibleThisFrame;
            pairCompare.eligibleThisFrame=0;++pairCompare.frame;}
        // Called before backend submission, never from a draw callback. The
        // spare set retains partial allocations or failed retirements for retry.
        if(!deleteCoverageExtent(coverageSpare,*diagnostic))return false;
        if(coverageLive.width==width&&coverageLive.height==height)return true;
        const bool created=createCoverageExtent(coverageSpare);
        // Allocation may fail with a private partial texture/FBO still bound.
        // Keep its names for cleanup, but never leak that target into UI entry.
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,0);gl.bindTexture(coverageTexture,0);
        gl.bindFramebuffer(framebuffer,0);
        const bool restored=check("restore bindings after coverage extent preparation");
        if(!created||!restored)return false;
        std::swap(coverageLive,coverageSpare);
        // Published single-sample banks are independent of both extent sets.
        return deleteCoverageExtent(coverageSpare,*diagnostic);
    }

// ---- Test-only pair-route A/B proof (RR_TEST_PAIR_ROUTE_COMPARE=1) ----
// Only private compare names are written: four 4-sample images, one
// single-sample verdict image and one host vector sized exactly
// width*height*4 for that verdict. The live scene receives the fixed
// result, which is what the normal route would have drawn.
bool VehicleWindowGl50DS::Storage::pairCompareEligible(U slot,U archetype,U component,U ordinal,
        const assets::SceneSpan48D& span) noexcept {
        auto& c=pairCompare;
        if(c.failed||!gl.blitFramebuffer||!gl.readPixels||!coverageArray)return false;
        if(component>=sourceBodyCount50LA||slot>frameOpponentCount)return false;
        const auto selected=racerView(archetype);
        if(!selected.materials||!selected.resources||!sourceBodyComponent50LA(component,selected.resources->componentCount))return false;
        const auto& material=selected.materials[selected.materialOffsets[component]+span.material_index];
        if(!material.policy.sourceSpecularPair50LA)return false;
        if(selected.specular[component].singleDiffuse&&!frameDiffuseSelected50LA[slot][component])return false;
        if(!frameSpecularSecond50LA[slot][component]||!fixedSpecularBlend50LA||!gl.blendFuncSeparate)return false;
        if(!specularSpanAlphaAdmitted50LA(slot,archetype,component,ordinal,span))return false;
        // One sampled span every eighth frame, rotating through last
        // frame's admitted spans so every pod/component/span is visited.
        constexpr std::uint64_t interval=8;
        const auto index=c.eligibleThisFrame++;
        if(c.frame%interval)return false;
        const auto population=c.eligibleLastFrame?c.eligibleLastFrame:1;
        return index==(c.frame/interval)%population;
    }

bool VehicleWindowGl50DS::Storage::pairCompareTexture(U& name,U format) noexcept {
        gl.genTextures(1,&name);
        if(!check("allocate pair compare image name")||!name)return false;
        gl.activeTexture(textureUnit0);gl.bindTexture(coverageTexture,name);
        gl.texImage2DMultisample(coverageTexture,4,format,width,height,1);
        gl.bindTexture(coverageTexture,0);
        return check("allocate pair compare four-sample image");
    }

bool VehicleWindowGl50DS::Storage::pairCompareResources() noexcept {
        auto& c=pairCompare;
        if(c.program&&c.resultFramebuffer&&c.width==width&&c.height==height&&
           coverageLive.width==width&&coverageLive.height==height)return true;
        if(coverageLive.width!=width||coverageLive.height!=height||width<=0||height<=0)return false;
        auto& d=*diagnostic;
        if(!deleteName(c.colorPre,gl.deleteTextures,d)||!deleteName(c.depthPre,gl.deleteTextures,d)||
           !deleteName(c.colorBridge,gl.deleteTextures,d)||!deleteName(c.colorFixed,gl.deleteTextures,d)||
           !deleteName(c.resultTexture,gl.deleteTextures,d)||!deleteName(c.resultFramebuffer,gl.deleteFramebuffers,d))return false;
        c.width=c.height=0;
        if(!c.program){
            constexpr const char* vertex=R"GLSL(#version 400 core
void main(){
    vec2 p=vec2((gl_VertexID<<1)&2,gl_VertexID&2);
    gl_Position=vec4(p*2.0-1.0,0.0,1.0);
}
)GLSL";
            constexpr const char* fragment=R"GLSL(#version 400 core
uniform sampler2DMS bridgeImage;
uniform sampler2DMS fixedImage;
uniform sampler2DMS priorImage;
uniform int probe;
out vec4 verdict;
void main(){
    ivec2 p=ivec2(gl_FragCoord.xy);
    // probe 1..3: report sample 0 of bridge/fixed/prior unchanged.
    if(probe==1){verdict=texelFetch(bridgeImage,p,0);return;}
    if(probe==2){verdict=texelFetch(fixedImage,p,0);return;}
    if(probe==3){verdict=texelFetch(priorImage,p,0);return;}
    float differ=0.0,alphaDiffer=0.0,bridged=0.0,fixedChanged=0.0,delta=0.0;
    for(int s=0;s<4;++s){
        vec4 a=floor(texelFetch(bridgeImage,p,s)*255.0+0.5);
        vec4 b=floor(texelFetch(fixedImage,p,s)*255.0+0.5);
        vec4 c=floor(texelFetch(priorImage,p,s)*255.0+0.5);
        // Colour must match byte for byte. Stored alpha is counted apart:
        // the fixed route keeps destination alpha at byte/128 for its blend.
        if(any(notEqual(a.rgb,b.rgb)))differ+=1.0;
        if(a.a!=b.a)alphaDiffer+=1.0;
        if(any(notEqual(a,c)))bridged+=1.0;
        if(any(notEqual(b,c)))fixedChanged+=1.0;
        vec3 e=abs(a.rgb-b.rgb);delta=max(delta,max(e.r,max(e.g,e.b)));
    }
    verdict=vec4(differ,bridged+16.0*fixedChanged,delta,alphaDiffer)/255.0;
}
)GLSL";
            if(!compileTrackShader(0x8B31,vertex,c.vertex)||!compileTrackShader(0x8B30,fragment,c.fragment))return false;
            c.program=shaderGl.createProgram();
            if(!check("allocate pair compare program")||!c.program)return false;
            shaderGl.attachShader(c.program,c.vertex);shaderGl.attachShader(c.program,c.fragment);
            shaderGl.bindFragDataLocation(c.program,0,"verdict");shaderGl.linkProgram(c.program);
            I linked=0;shaderGl.getProgramiv(c.program,0x8B82,&linked);
            if(!check("link pair compare program")||!linked)return false;
            const I bridge=shaderGl.getUniformLocation(c.program,"bridgeImage");
            const I fixed=shaderGl.getUniformLocation(c.program,"fixedImage");
            const I prior=shaderGl.getUniformLocation(c.program,"priorImage");
            if(!check("locate pair compare inputs")||bridge<0||fixed<0||prior<0)return false;
            gl.useProgram(c.program);gl.uniform1i(bridge,0);gl.uniform1i(fixed,1);gl.uniform1i(prior,2);
            if(!check("initialize pair compare samplers"))return false;
        }
        if(!pairCompareTexture(c.colorPre,rgba8)||!pairCompareTexture(c.depthPre,depth24)||
           !pairCompareTexture(c.colorBridge,rgba8)||!pairCompareTexture(c.colorFixed,rgba8))return false;
        gl.genTextures(1,&c.resultTexture);
        if(!check("allocate pair compare verdict name")||!c.resultTexture)return false;
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,c.resultTexture);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        drawGl.bindBuffer(0x88EC,0);
        gl.texImage2D(texture2D,0,static_cast<I>(rgba8),width,height,0,rgba,unsignedByte,nullptr);
        gl.bindTexture(texture2D,0);
        if(!check("allocate pair compare verdict image"))return false;
        gl.genFramebuffers(1,&c.resultFramebuffer);
        if(!check("allocate pair compare framebuffer name")||!c.resultFramebuffer)return false;
        gl.bindFramebuffer(framebuffer,c.resultFramebuffer);
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,c.resultTexture,0);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);
        if(!check("verify pair compare framebuffer")||status!=framebufferComplete)return false;
        try{c.host.assign(std::size_t(width)*std::size_t(height)*4u,0);}catch(...){return false;}
        c.width=width;c.height=height;
        return true;
    }

bool VehicleWindowGl50DS::Storage::pairCompareSpan(std::uint8_t bank,U slot,U archetype,U component,U ordinal,
        const assets::SceneSpan48D& span) noexcept {
        auto& c=pairCompare;
        // Retain every binding/capability this proof touches; restore it
        // exactly before each source draw and after the verdict.
        struct Saved{I draw=0,read=0,program=0,array=0,active=0,pack=0,unpack=0,alignment=4;I viewport[4]{};
            I caps[7]{};I images[3]{};I flat=0;I samplers[3]{};} saved;
        constexpr U caps[7]={0x0BE2,0x0B71,0x0C11,0x0B90,0x0B44,0x809D,0x8DB9};
        const auto retain=[&]() noexcept {
            gl.getIntegerv(0x8CA6,&saved.draw);gl.getIntegerv(0x8CAA,&saved.read);
            gl.getIntegerv(0x8B8D,&saved.program);gl.getIntegerv(0x85B5,&saved.array);
            gl.getIntegerv(0x84E0,&saved.active);gl.getIntegerv(0x88ED,&saved.pack);
            gl.getIntegerv(0x88EF,&saved.unpack);
            gl.getIntegerv(0x0D05,&saved.alignment);gl.getIntegerv(0x0BA2,saved.viewport);
            for(U i=0;i<7;++i)gl.getIntegerv(caps[i],&saved.caps[i]);
            for(U i=0;i<3;++i){gl.activeTexture(textureUnit0+i);gl.getIntegerv(0x9104,&saved.images[i]);
                if(i==0)gl.getIntegerv(0x8069,&saved.flat);
                if(gl.bindSampler)gl.getIntegerv(0x8919,&saved.samplers[i]);}
            gl.activeTexture(static_cast<U>(saved.active));
            return check("retain state for pair compare");
        };
        const auto restore=[&]() noexcept {
            for(U i=0;i<3;++i){gl.activeTexture(textureUnit0+i);gl.bindTexture(coverageTexture,static_cast<U>(saved.images[i]));
                if(i==0)gl.bindTexture(texture2D,static_cast<U>(saved.flat));
                if(gl.bindSampler)gl.bindSampler(i,static_cast<U>(saved.samplers[i]));}
            gl.activeTexture(static_cast<U>(saved.active));
            gl.bindFramebuffer(drawFramebuffer,static_cast<U>(saved.draw));gl.bindFramebuffer(readFramebuffer,static_cast<U>(saved.read));
            gl.useProgram(static_cast<U>(saved.program));drawGl.bindVertexArray(static_cast<U>(saved.array));
            drawGl.bindBuffer(pixelPackBuffer,static_cast<U>(saved.pack));drawGl.bindBuffer(0x88EC,static_cast<U>(saved.unpack));
            gl.pixelStorei(0x0D05,saved.alignment);
            gl.viewport(saved.viewport[0],saved.viewport[1],saved.viewport[2],saved.viewport[3]);
            for(U i=0;i<7;++i){if(saved.caps[i])gl.enable(caps[i]);else gl.disable(caps[i]);}
            return check("restore state after pair compare");
        };
        // The private compare framebuffer is re-pointed at one owned image
        // set at a time; every blit is a same-size, same-format four-sample
        // copy (the bridge's own GL4.0 transfer), with scissor/sRGB off.
        const U scene=coverageLive.scene[bank];
        const auto attach=[&](U color,U depth) noexcept {
            gl.bindFramebuffer(framebuffer,c.resultFramebuffer);
            gl.framebufferTexture2D(framebuffer,colorAttachment,coverageTexture,color,0);
            gl.framebufferTexture2D(framebuffer,depthAttachment,coverageTexture,depth,0);
        };
        const auto blit=[&](U from,U to,U bits) noexcept {
            gl.bindFramebuffer(readFramebuffer,from);gl.bindFramebuffer(drawFramebuffer,to);
            gl.blitFramebuffer(0,0,width,height,0,0,width,height,bits,0x2600);
        };
        constexpr U colorBits=0x4000,depthBits=0x0100;
        if(!retain())return false;
        const bool ready=pairCompareResources()&&scene;
        if(!restore())return false;
        if(!ready){
            c.failed=true;
            std::fprintf(stderr,"pair_route_compare unavailable width=%d height=%d\n",width,height);
            return drawSpecularSpanRoute50LA(bank,slot,archetype,component,ordinal,span,0);
        }
        gl.disable(0x0C11);gl.disable(0x8DB9);
        attach(c.colorPre,c.depthPre);blit(scene,c.resultFramebuffer,colorBits|depthBits);
        if(!check("retain destination before pair compare")||!restore())return false;
        // The route drawn last is the live one: the normal choice for this span.
        const int live=coverageFixedSpecular50LA?2:1,first=3-live;
        if(!drawSpecularSpanRoute50LA(bank,slot,archetype,component,ordinal,span,first))return false;
        gl.disable(0x0C11);gl.disable(0x8DB9);
        attach(first==1?c.colorBridge:c.colorFixed,c.depthPre);blit(scene,c.resultFramebuffer,colorBits);
        attach(c.colorPre,c.depthPre);blit(c.resultFramebuffer,scene,colorBits|depthBits);
        if(!check("restore destination between pair routes")||!restore())return false;
        if(!drawSpecularSpanRoute50LA(bank,slot,archetype,component,ordinal,span,live))return false;
        // Retain the state the live route left, compare, then put it back.
        if(!retain())return false;
        gl.disable(0x0C11);gl.disable(0x8DB9);
        attach(live==2?c.colorFixed:c.colorBridge,c.depthPre);blit(scene,c.resultFramebuffer,colorBits);
        const bool copied=check("retain live pair result");
        const bool evaluated=copied&&pairCompareEvaluate(bank,slot,component,ordinal);
        const bool restored=restore();
        return evaluated&&restored;
    }

bool VehicleWindowGl50DS::Storage::pairCompareEvaluate(std::uint8_t,U slot,U component,U ordinal) noexcept {
        auto& c=pairCompare;
        gl.bindFramebuffer(framebuffer,c.resultFramebuffer);
        gl.framebufferTexture2D(framebuffer,depthAttachment,coverageTexture,0,0);
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,c.resultTexture,0);
        gl.viewport(0,0,width,height);
        constexpr U off[7]={0x0BE2,0x0B71,0x0C11,0x0B90,0x0B44,0x809D,0x8DB9};
        for(const U cap:off)gl.disable(cap);
        gl.useProgram(c.program);drawGl.bindVertexArray(coverageArray);
        const U images[3]={c.colorBridge,c.colorFixed,c.colorPre};
        for(U i=0;i<3;++i){gl.activeTexture(textureUnit0+i);if(gl.bindSampler)gl.bindSampler(i,0);
            gl.bindTexture(coverageTexture,images[i]);}
        const I probe=shaderGl.getUniformLocation(c.program,"probe");
        if(probe>=0)gl.uniform1i(probe,0);
        if(!check("bind pair compare inputs"))return false;
        gl.drawArrays(0x0004,0,3);
        drawGl.bindBuffer(pixelPackBuffer,0);gl.pixelStorei(0x0D05,4);
        const std::size_t bytes=std::size_t(width)*std::size_t(height)*4u;
        if(c.host.size()!=bytes||c.width!=width||c.height!=height)return false;
        gl.readPixels(0,0,width,height,rgba,unsignedByte,c.host.data());
        if(!check("read pair compare verdict"))return false;
        std::uint64_t touched=0,mismatched=0,samples=0,bridged=0,fixedChanged=0,alphaOnly=0;unsigned delta=0;
        std::size_t firstMismatch=0;
        for(std::size_t i=0;i<bytes;i+=4){
            const auto differ=c.host[i],touch=c.host[i+1],d=c.host[i+2];
            if(touch)++touched;
            if(touch&15u)++bridged;
            if(touch>>4)++fixedChanged;
            if(differ){if(!mismatched)firstMismatch=i/4;++mismatched;samples+=differ;}
            else if(c.host[i+3])++alphaOnly;
            if(d>delta)delta=d;
        }
        // First differing pixel: sample 0 of bridge, fixed and prior, read
        // one pixel at a time into a fixed local array.
        unsigned char values[3][4]{};
        if(mismatched&&probe>=0){
            const I x=static_cast<I>(firstMismatch%std::size_t(width)),y=static_cast<I>(firstMismatch/std::size_t(width));
            for(I m=1;m<=3;++m){gl.uniform1i(probe,m);gl.drawArrays(0x0004,0,3);
                gl.readPixels(x,y,1,1,rgba,unsignedByte,values[m-1]);}
            gl.uniform1i(probe,0);
            if(!check("read pair compare probe"))return false;
            std::fprintf(stderr,"pair_route_compare_probe x=%d y=%d bridge=%u,%u,%u,%u fixed=%u,%u,%u,%u prior=%u,%u,%u,%u\n",x,y,
                values[0][0],values[0][1],values[0][2],values[0][3],values[1][0],values[1][1],values[1][2],values[1][3],
                values[2][0],values[2][1],values[2][2],values[2][3]);
        }
        ++c.compared;c.touchedPixels+=touched;c.mismatchedPixels+=mismatched;c.mismatchedSamples+=samples;
        if(delta>c.maxDelta)c.maxDelta=delta;
        std::fprintf(stderr,"pair_route_compare n=%llu frame=%llu slot=%u component=%u span=%u touched_pixels=%llu "
            "bridge_changed=%llu fixed_changed=%llu alpha_only_pixels=%llu "
            "rgb_mismatched_pixels=%llu rgb_mismatched_samples=%llu rgb_max_delta=%u total_touched=%llu total_rgb_mismatched=%llu total_rgb_max_delta=%llu\n",
            static_cast<unsigned long long>(c.compared),static_cast<unsigned long long>(c.frame),slot,component,ordinal,
            static_cast<unsigned long long>(touched),static_cast<unsigned long long>(bridged),
            static_cast<unsigned long long>(fixedChanged),static_cast<unsigned long long>(alphaOnly),
            static_cast<unsigned long long>(mismatched),
            static_cast<unsigned long long>(samples),delta,static_cast<unsigned long long>(c.touchedPixels),
            static_cast<unsigned long long>(c.mismatchedPixels),static_cast<unsigned long long>(c.maxDelta));
        return true;
    }

bool VehicleWindowGl50DS::Storage::createCoverageProgram() noexcept {
        if(!coverageDemand)return true;
        // Compile the strict GLSL400 route even without source pairs, so AA1
        // admission cannot claim a shader capability solely from a proc address.
        constexpr const char* vertex=R"GLSL(#version 400 core
void main(){
    vec2 p=vec2((gl_VertexID<<1)&2,gl_VertexID&2);
    gl_Position=vec4(p*2.0-1.0,0.0,1.0);
}
)GLSL";
        constexpr const char* fragment=R"GLSL(#version 400 core
uniform int raComposition;
uniform sampler2D payloadImage;
uniform sampler2D depthImage;
uniform sampler2DMS destinationImage;
out vec4 composedColor;
void main(){
    ivec2 pixel=ivec2(gl_FragCoord.xy);
    vec4 sourceByte=texelFetch(payloadImage,pixel,0);
    if(sourceByte.a<0.0)discard;
    gl_FragDepth=texelFetch(depthImage,pixel,0).r;
    // A static SampleID use requires separate invocations for every sample.
    // Geometry coverage came from the original AA0 triangle's center raster.
    vec4 destinationByte=floor(texelFetch(destinationImage,pixel,gl_SampleID)*255.0+0.5);
    sourceByte.rgb=raComposition==2 ?
        clamp(floor(sourceByte.rgb*sourceByte.a/128.0)+destinationByte.rgb,0.0,255.0) :
        raComposition!=0 ?
        clamp(destinationByte.rgb+floor((sourceByte.rgb-destinationByte.rgb)*sourceByte.a/128.0),0.0,255.0) :
        clamp(floor(sourceByte.rgb*destinationByte.a/128.0)+destinationByte.rgb,0.0,255.0);
    composedColor=sourceByte/255.0;
}
)GLSL";
        if(!compileTrackShader(0x8B31,vertex,coverageVertex)||!compileTrackShader(0x8B30,fragment,coverageFragment))return false;
        coverageProgram=shaderGl.createProgram();
        if(!check("allocate coverage composition program")||!coverageProgram)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"zero coverage composition program");
        shaderGl.attachShader(coverageProgram,coverageVertex);shaderGl.attachShader(coverageProgram,coverageFragment);
        shaderGl.bindFragDataLocation(coverageProgram,0,"composedColor");shaderGl.linkProgram(coverageProgram);
        I linked=0;shaderGl.getProgramiv(coverageProgram,0x8B82,&linked);
        if(!check("link strict coverage composition program"))return false;
        if(!linked){shaderGl.getProgramInfoLog(coverageProgram,static_cast<I>(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
            check("read coverage composition link log");
            return fail(*diagnostic,VehicleWindowError50DS::shader,"coverage composition link failed");}
        coverageRaCompositionUniform=shaderGl.getUniformLocation(coverageProgram,"raComposition");
        coveragePayloadUniform=shaderGl.getUniformLocation(coverageProgram,"payloadImage");
        coverageDepthUniform=shaderGl.getUniformLocation(coverageProgram,"depthImage");
        coverageDestinationUniform=shaderGl.getUniformLocation(coverageProgram,"destinationImage");
        gl.genVertexArrays(1,&coverageArray);
        if(!check("locate coverage composition inputs and array")||!coverageArray||
           coverageRaCompositionUniform<0||coveragePayloadUniform<0||coverageDepthUniform<0||coverageDestinationUniform<0)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"coverage composition inputs unavailable");
        // These private program sampler assignments never vary between RA and
        // racer composition. Uniform values survive binds and extent changes;
        // initialize once per link rather than three writes per coverage run.
        I previousProgram=0;gl.getIntegerv(0x8B8D,&previousProgram); // GL_CURRENT_PROGRAM
        if(!check("retain program before coverage sampler initialization"))return false;
        gl.useProgram(coverageProgram);
        bool initialized=check("bind coverage program for fixed sampler initialization");
        if(initialized){
            gl.uniform1i(coveragePayloadUniform,0);gl.uniform1i(coverageDepthUniform,1);gl.uniform1i(coverageDestinationUniform,2);
            initialized=check("initialize fixed coverage sampler assignments");
        }
        // Restore the actual prior program even when binding or uniforms fail.
        // Never write uniforms if the coverage program could not be bound.
        gl.useProgram(static_cast<U>(previousProgram));
        const bool restored=check("restore program after coverage sampler initialization");
        return initialized&&restored;
    }

bool VehicleWindowGl50DS::Storage::drawCoveragePairTriangle(U bank,U first,U image,const std::array<I,4>& region,U sourceArray,U triangleCount,
        bool firstRun,bool firstCoverageSetup) noexcept {
        // A proven empty native clip region cannot touch any destination
        // sample. Preserve source group order/accounting without GPU work.
        if(region[2]<=region[0]||region[3]<=region[1])return true;
        // The sole caller owns one synchronous source-pair span. These five
        // sample-policy bits, sRGB/blend, viewport and color mask do not change
        // between its groups. Establish them at the first nonempty bridge,
        // without carrying state across spans or callbacks.
        if(firstCoverageSetup){coverageAa0();gl.disable(0x8DB9);gl.disable(0x0BE2);}
        // Later runs inherit disabled MULTISAMPLE from the checked tail.
        // Racer payloads use the same world-space vertices and clip matrix on
        // every track. Bound scratch clear and composition to the conservative
        // run rectangle; uncertain projection already falls back to full size.
        gl.scissor(region[0],region[1],region[2]-region[0],region[3]-region[1]);gl.enable(0x0C11);
        gl.bindFramebuffer(framebuffer,coverageLive.raster);
        // These private FBOs retain createCoverageExtent's selectors:
        // raster draws to attachments0/1; scene/snapshot draw/read0.
        // Binding an FBO does not reset its selectors. No later owner changes
        // these values, so do not reissue eight selector calls per run.
        if(firstCoverageSetup){gl.viewport(0,0,width,height);gl.colorMask(1,1,1,1);}
        gl.disable(0x0B71);if(firstRun)gl.depthMask(0);
        const float invalid[]={0.f,0.f,0.f,-1.f};gl.clearBufferfv(0x1800,0,invalid);
        // The checked prior tail restored this same source program.
        if(firstRun)gl.useProgram(shader->program());
        gl.uniform1i(shader->uniforms().sourcePairPass50LA,3);
        // Later runs inherit unit0/image from the previous checked tail.
        // The synchronous group performs only CPU lookahead between runs.
        if(firstRun){gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,image);}
        bool accepted=check("bind AA0 original triangle payload target");
        if(accepted){
            drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(triangleCount*3),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(first)*sizeof(U)),0);
            accepted=check("raster original AA0 map-2 payload and center depth");
        }
        if(accepted){
            if(gl.copyImageSubData){
                // Distinct verified RGBA8/four-sample images; preserve every
                // sample at identical texel coordinates, without feedback.
                gl.copyImageSubData(coverageLive.color[bank],coverageTexture,0,
                    region[0],region[1],0,coverageLive.snapshotColor,coverageTexture,0,
                    region[0],region[1],0,region[2]-region[0],region[3]-region[1],1);
            }else{
                gl.bindFramebuffer(readFramebuffer,coverageLive.scene[bank]);
                gl.bindFramebuffer(drawFramebuffer,coverageLive.snapshot);
                gl.blitFramebuffer(region[0],region[1],region[2],region[3],
                    region[0],region[1],region[2],region[3],colorBufferBit,0x2600);
            }
            accepted=check("copy per-sample destination before ordered map-2 triangle");
        }
        if(accepted){
            gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);
            // First-run setup established mask0; checked successful tails preserve it.
            // The checked source-pair span already established LEQUAL.
            gl.enable(0x809D);gl.enable(0x0B71);
            // The first successful bridge in this span establishes the private
            // composition mode and all three sampler-object bindings. Checked
            // bridge tails and base/map-2 group setup preserve those values.
            gl.useProgram(coverageProgram);if(firstCoverageSetup)gl.uniform1i(coverageRaCompositionUniform,0);drawGl.bindVertexArray(coverageArray);
            if(firstCoverageSetup&&gl.bindSampler)gl.bindSampler(0,0);gl.bindTexture(texture2D,coverageLive.payload);
            gl.activeTexture(textureUnit0+1);if(firstCoverageSetup&&gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,coverageLive.rasterDepth);
            gl.activeTexture(textureUnit0+2);if(firstCoverageSetup&&gl.bindSampler)gl.bindSampler(2,0);gl.bindTexture(coverageTexture,coverageLive.snapshotColor);
            accepted=check("bind sample-specific original destination-alpha composition");
            if(accepted){gl.drawArrays(0x0004,0,3);accepted=check("compose ordered map-2 triangle per destination sample");}
        }
        // Restore even on rejection. The caller still owns group/span accounting
        // and aborts before publication. No sampled texture remains attached.
        // Successful composition leaves unit2 selected; rejection restores explicitly.
        if(!accepted)gl.activeTexture(textureUnit0+2);gl.bindTexture(coverageTexture,0);
        gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,image);
        // Successful composition already checked the exact scene binding.
        // Rejected setup still receives the original unconditional retry path.
        if(!accepted)gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);
        gl.useProgram(shader->program());gl.uniform1i(shader->uniforms().sourcePairPass50LA,2);
        drawGl.bindVertexArray(sourceArray);
        // On success only MULTISAMPLE changed. A rejected setup may not have
        // established every sample bit; preserve the full old cleanup retry.
        if(accepted)gl.disable(0x809D);else coverageAa0();
        // Accepted composition retained depth enabled, LEQUAL and mask0.
        if(!accepted){gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(0);}
        gl.disable(0x0C11);
        const bool restored=check("restore source-pair state after native coverage bridge");
        return accepted&&restored;
    }

bool VehicleWindowGl50DS::Storage::racerTrianglesSeparable(U slot,U component,U firstA,U firstB)const noexcept {
        // The coverage bridge rasterizes its payload single-sample (coverageAa0,
        // pixel centers) exactly as the single-sample route does, so the same
        // pixel-center separation proof admits a run there too.
        if(!racerRasterCacheActive||slot!=racerRasterSlot||component!=racerRasterComponent||
            slot>frameOpponentCount)return false;
        const auto& batch=slot?frameOpponentBatches[slot-1]:framePlayerBatch;
        if(component>=batch.componentCount)return false;
        const auto& part=batch.component[component];
        if(!part.indices||firstA>part.indexCount||part.indexCount-firstA<3||
            firstB>part.indexCount||part.indexCount-firstB<3)return false;
        const auto* a=part.indices+firstA;const auto* b=part.indices+firstB;
        if(a[0]==a[1]||a[0]==a[2]||a[1]==a[2]||b[0]==b[1]||b[0]==b[2]||b[1]==b[2])return false;
        const auto point=[&](U index,raster_shared_edge::Point& out) noexcept {
            const auto& entry=racerRasterOperands[index%racerRasterOperands.size()];
            // Reuse only already computed group operands; collisions or
            // uncertain/clipped projections retain the old barrier.
            if(!entry.occupied||entry.index!=index||!entry.finite||!entry.projectionReady)return false;
            const auto& v=entry.operand;
            if(v.clip[3]<=v.error[3]+0.0001)return false;
            for(U axis=0;axis<3;++axis)if(std::abs(v.clip[axis])+v.error[axis]>=v.clip[3]-v.error[3])return false;
            for(U axis=0;axis<2;++axis){
                const auto* q=entry.projectedQuotients.data()+axis*4;
                const double extent=axis?height:width;
                // Core GL3.2 requires at least four raster subpixel bits
                // (table6.37). Allow two minimum-precision grid steps in
                // addition to the full transform interval above.
                constexpr double rounding=1./8.;
                raster_shared_edge::Interval interval{(*std::min_element(q,q+4)+1)*extent*.5-rounding,
                    (*std::max_element(q,q+4)+1)*extent*.5+rounding};
                (axis?out.y:out.x)=interval;
            }
            return true;
        };
        U shared[2]{},matched=0,otherA=0,otherB=0;
        bool same[3][3]{};
        for(U i=0;i<3;++i){
            bool found=false;for(U j=0;j<3;++j)if(a[i]==b[j]){found=true;same[i][j]=true;}
            if(found){if(matched==2)return false;shared[matched++]=a[i];}else otherA=a[i];
        }
        if(matched==2){
            for(U j=0;j<3;++j)if(b[j]!=shared[0]&&b[j]!=shared[1])otherB=b[j];
            const U indices[]={shared[0],shared[1],otherA,otherB};
            raster_shared_edge::Point points[4];
            for(U i=0;i<4;++i)if(!point(indices[i],points[i]))return false;
            return raster_shared_edge::opposite(points[0],points[1],points[2],points[3]);
        }
        // At most one identical vertex: a robust separating edge line,
        // with any shared vertex clear of every pixel center.
        raster_shared_edge::Point pa[3],pb[3];
        for(U i=0;i<3;++i)if(!point(a[i],pa[i])||!point(b[i],pb[i]))return false;
        return raster_shared_edge::separated(pa,pb,same);
    }

void VehicleWindowGl50DS::Storage::classifyUnprovenPair(U slot,U component,U firstA,U firstB) noexcept {
        if(slot>frameOpponentCount)return;
        const auto& batch=slot?frameOpponentBatches[slot-1]:framePlayerBatch;
        if(component>=batch.componentCount)return;
        const auto& part=batch.component[component];
        if(!part.indices||firstA+3>part.indexCount||firstB+3>part.indexCount)return;
        const auto* a=part.indices+firstA;const auto* b=part.indices+firstB;
        U shared=0;bool operands=true;
        for(U i=0;i<3;++i){
            for(U j=0;j<3;++j)shared+=a[i]==b[j];
            for(const U index:{a[i],b[i]}){
                const auto& entry=racerRasterOperands[index%racerRasterOperands.size()];
                operands=operands&&entry.occupied&&entry.index==index&&entry.finite&&entry.projectionReady;
            }
        }
        if(!operands)++coveragePairUnprovenOperands;
        else ++coveragePairUnproven[std::min<U>(shared,3)];
    }

bool VehicleWindowGl50DS::Storage::drawCoveragePairGroup(U bank,U slot,U component,U first,U count,U image,
        bool& firstCoverageSetup) noexcept {
        // Never cross the original base/map-2 MSCNT group boundary. A run
        // retains contiguous EBO order and admits only pairwise-disjoint
        // conservative pixel rectangles, or a proven shared-edge or robust
        // separating-line pair at pixel centers (the coverage payload is AA0). No sample can consume another
        // run member's color/alpha or compete with its depth. The unchanged
        // byte shader and per-sample depth comparison can share one snapshot.
        // A source group has at most 14 triangles; a single-sample batch of
        // proven-disjoint groups has at most 64 (planSeparableGroupBatch).
        constexpr U maximumRun=64;
        const U end=first+count;
        const auto fullRegion=[&](const std::array<I,4>& r) noexcept {
            return r[0]==0&&r[1]==0&&r[2]==width&&r[3]==height;
        };
        // A rejected lookahead becomes the next run's first triangle; avoid
        // evaluating its clip rectangle twice in the common singleton case.
        U pendingFirst=end;std::array<I,4> pending{};
        // The sole caller owns one VAO for the entire original MSCNT group.
        // Each bridge restores it on success AND rejection; region lookahead
        // is CPU-only. Retain the actual binding once, lazily, so all-empty
        // groups still issue no GL queries or bridge operations.
        bool sourceArrayRetained=false;I sourceArray=0;
        bool firstRun=true;
        while(first<end){
            const auto initial=pendingFirst==first?pending:racerTriangleRasterRegion(slot,component,first);
            if(initial[2]<=initial[0]||initial[3]<=initial[1]){++coveragePairEmptyTriangles;first+=3;continue;}
            // Only the assigned prefix [0,run) is read; unused rows never escape.
            std::array<std::array<I,4>,maximumRun> regions;
            regions[0]=initial;auto united=initial;U run=1;
            const auto area=[](const std::array<I,4>& r) noexcept -> std::uint64_t {
                return std::uint64_t(r[2]-r[0])*std::uint64_t(r[3]-r[1]);
            };
            std::uint64_t summedArea=area(initial);
            while(!fullRegion(initial)&&run<maximumRun&&first+run*3<end){
                pendingFirst=first+run*3;
                const auto next=racerTriangleRasterRegion(slot,component,pendingFirst);
                pending=next;
                // Flush before an empty primitive rather than create a hole
                // in this contiguous source range. The next iteration skips it.
                if(next[2]<=next[0]||next[3]<=next[1]||fullRegion(next)){++coveragePairBreakRegion;break;}
                bool disjoint=true;
                for(U i=0;i<run;++i){const auto& old=regions[i];
                    if(next[0]<old[2]&&old[0]<next[2]&&next[1]<old[3]&&old[1]<next[3]){
                        if(!racerTrianglesSeparable(slot,component,first+i*3,first+run*3)){
                            classifyUnprovenPair(slot,component,first+i*3,first+run*3);disjoint=false;break;}
                    }
                }
                if(!disjoint)break;
                auto candidate=united;
                for(U axis=0;axis<2;++axis){candidate[axis]=std::min(candidate[axis],next[axis]);
                    candidate[axis+2]=std::max(candidate[axis+2],next[axis+2]);}
                const auto nextArea=summedArea+area(next);
                // Avoid a sparse run expanding scratch/copy/composition work
                // to an arbitrarily large rectangle. Extents are GL-limited.
                // Only the AA bridge consumes the united rectangle; the
                // single-sample run is one barrier plus one indexed draw.
                if(coverageDemand&&area(candidate)>nextArea*2u)break;
                regions[run++]=next;united=candidate;summedArea=nextArea;
            }
            if(!fullRegion(initial)){
                if(run==maximumRun)++coveragePairBreakCap;
                else if(first+run*3>=end)++coveragePairBreakEnd;
            }
            // Singleton uses the same bridge and triangle count as before.
            // Full/uncertain clip fallback always remains its own run.
            if(coverageDemand){
                if(!sourceArrayRetained){
                    gl.getIntegerv(0x85B5,&sourceArray);
                    if(!check("retain source-pair vertex array for coverage group"))return false;
                    sourceArrayRetained=true;
                }
                if(!drawCoveragePairTriangle(bank,first,image,united,static_cast<U>(sourceArray),run,firstRun,firstCoverageSetup))return false;
                // Only an accepted nonempty bridge authorizes setup reuse.
                firstRun=false;firstCoverageSetup=false;
            }else{
                gl.textureBarrier();
                drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(run*3),0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(first)*sizeof(U)),0);
            }
            ++coveragePairRuns;coveragePairTriangles+=run;
            if(run>1)coveragePairCoalescedTriangles+=run;
            if(fullRegion(initial))++coveragePairFullSingletons;
            first+=run*3;
        }
        return true;
    }

U VehicleWindowGl50DS::Storage::planSeparableGroupBatch(U slot,U component,
        const SpecularGroup50LA* groups,U g,U end) noexcept {
        // Batching reorders a later group's base before an earlier group's
        // map-2, so it is admitted only when no sample can belong to two
        // groups: each pixel then sees exactly its one group's original order.
        constexpr U maximumTriangles=64;
        if(coverageDemand||g>=end||groups[g].indexCount%3||groups[g].indexCount/3>maximumTriangles)return 1;
        const auto empty=[](const std::array<I,4>& r) noexcept {return r[2]<=r[0]||r[3]<=r[1];};
        const auto full=[&](const std::array<I,4>& r) noexcept {
            return r[0]==0&&r[1]==0&&r[2]==width&&r[3]==height;
        };
        std::array<std::array<I,4>,maximumTriangles> regions;
        std::array<U,maximumTriangles> offsets;
        U triangles=0;
        for(U k=0;k<groups[g].indexCount/3;++k){
            offsets[triangles]=groups[g].indexStart+3*k;
            regions[triangles]=racerTriangleRasterRegion(slot,component,offsets[triangles]);
            if(full(regions[triangles]))return 1;
            ++triangles;
        }
        U count=1;
        for(U next=g+1;next<end;++next){
            const auto& previous=groups[next-1];const auto& candidate=groups[next];
            if(candidate.indexStart!=previous.indexStart+previous.indexCount||candidate.indexCount%3||
               candidate.indexCount/3>maximumTriangles-triangles)break;
            const U earlier=triangles;U added=triangles;bool admitted=true;
            for(U k=0;admitted&&k<candidate.indexCount/3;++k){
                const U first=candidate.indexStart+3*k;
                const auto region=racerTriangleRasterRegion(slot,component,first);
                if(full(region)){admitted=false;break;}
                if(!empty(region))
                    for(U m=0;m<earlier;++m){
                        const auto& old=regions[m];
                        if(empty(old)||!(region[0]<old[2]&&old[0]<region[2]&&region[1]<old[3]&&old[1]<region[3]))continue;
                        if(!racerTrianglesSeparable(slot,component,offsets[m],first)){admitted=false;break;}
                    }
                regions[added]=region;offsets[added]=first;++added;
            }
            if(!admitted)break;
            triangles=added;++count;
        }
        return count;
    }

bool VehicleWindowGl50DS::Storage::resolveCoverage(U bank) noexcept {
        if(!coverageDemand)return true;
        coverageAa0();gl.disable(0x0C11);gl.disable(0x8DB9);
        gl.bindFramebuffer(readFramebuffer,coverageLive.scene[bank]);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
        if(!check("bind complete native scene for final color-only resolve"))return false;
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        const bool resolved=check("resolve four-sample scene before HUD blur history and display");
        gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const bool restored=check("bind resolved single-sample presentation bank");
        // Submission only. Existing finish/display/history receipts stay intact.
        return resolved&&restored;
    }

bool VehicleWindowGl50DS::Storage::hasRaWorldResources() const noexcept {
        return raLocalFlareTexture.ownsResources()||raLocalFlareArray||raLocalFlareBuffer||raLocalFlareVertexShader||
            raLocalFlareFragmentShader||raLocalFlareProgram||raWorldArray||raWorldVertices||raWorldOperands||raWorldIndices||raWorldVertexShader||raWorldGeometryShader||
            raWorldFragment[0]||raWorldFragment[1]||raWorldProgram[0]||raWorldProgram[1];
    }

void VehicleWindowGl50DS::Storage::retireRaWorldPublication() noexcept {
        if(raWorld){raWorld->retireRenderer(raWorldRenderer);raWorld.reset();}
        raWorldRenderer=nullptr;raWorldImagePublication=0;raWorldFramePrepared=false;
    }

bool VehicleWindowGl50DS::Storage::raWorldCurrent(VehicleWindowDiagnostic50DS& d) const noexcept {
        if(!request.raStaticWorld)return !raWorld || fail(d,VehicleWindowError50DS::material,"RA aggregate lease missing");
        return (raWorld && request.raStaticWorld->coldStaticWorld()==raWorld.get() &&
            raWorld->rendererPublication_==raWorldRenderer && raWorld->drawPlanCurrent() &&
            request.raStaticWorldTextures && request.raStaticWorldTextures->raStaticWorldOwner()==raWorld.get() &&
            request.raStaticWorldTextures->textureCount()==49 &&
            request.raStaticWorldTextures->validBorrow(request.context,request.isCurrent,raWorldImagePublication)) ||
            fail(d,VehicleWindowError50DS::material,"RA aggregate/world/image publication expired");
    }

bool VehicleWindowGl50DS::Storage::deleteRaWorldResources(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!deleteRaLocalFlareResources(d))return false;
        for(U i=0;i<2;++i)if(!deleteShaderName(raWorldProgram[i],true,d)||
            !deleteShaderName(raWorldFragment[i],false,d))return false;
        return deleteShaderName(raWorldGeometryShader,false,d)&&
            deleteShaderName(raWorldVertexShader,false,d)&&
            deleteName(raWorldArray,gl.deleteVertexArrays,d)&&
            deleteName(raWorldVertices,gl.deleteBuffers,d)&&deleteName(raWorldOperands,gl.deleteBuffers,d)&&
            deleteName(raWorldIndices,gl.deleteBuffers,d);
    }

bool VehicleWindowGl50DS::Storage::createRaWorld() noexcept {
        if(!raWorld)return true;
        if(!raWorldCurrent(*diagnostic))return false;
        // Validate the actual source plan before any GPU allocation. The GL
        // count is signed, while source indices/ranges are unsigned.
        for(const auto& plan:raWorld->surfaceDrawPlan_){
            if(plan.raster>=raWorld->rasterInventory_.size()||
               plan.surface>=raWorld->surfaceDrawPlan_.size()||
               !plan.indexCount||plan.indexCount%3||
               plan.indexCount>static_cast<U>(std::numeric_limits<I>::max())||
               plan.indexStart>raWorld->indices_.size()||
               plan.indexCount>raWorld->indices_.size()-plan.indexStart||
               plan.indexCount>std::numeric_limits<U>::max()-plan.indexStart||
               plan.vertexStart>raWorld->vertices_.size()||
               plan.vertexCount>raWorld->vertices_.size()-plan.vertexStart||
               plan.vertexCount>std::numeric_limits<U>::max()-plan.vertexStart)
                return fail(*diagnostic,VehicleWindowError50DS::limits,"RA source draw range exceeds native limits");
            // import_scene48d emits chronological consecutive strip records,
            // swapping only A/B for odd strip triangles. Restarts begin a new
            // chronological run; lowering adds one constant placement offset.
            // This invariant lets the GS recover source fan anchors from actual
            // indexed vertex IDs, independently of draw batching/primitive ID.
            for(U offset=0;offset<plan.indexCount;offset+=3){
                const auto a=raWorld->indices_[plan.indexStart+offset];
                const auto b=raWorld->indices_[plan.indexStart+offset+1];
                const auto c=raWorld->indices_[plan.indexStart+offset+2];
                const std::uint64_t oldest=std::min(a,b),middle=std::max(a,b);
                if(oldest<plan.vertexStart||middle!=oldest+1||std::uint64_t(c)!=oldest+2||
                   std::uint64_t(c)>=std::uint64_t(plan.vertexStart)+plan.vertexCount||
                   c>static_cast<U>(std::numeric_limits<I>::max()))
                    return fail(*diagnostic,VehicleWindowError50DS::assets,
                        "RA source triangle lacks retained chronological strip indices");
            }
            if(raWorld->rasterInventory_[plan.raster].passFlags&4u)
                return fail(*diagnostic,VehicleWindowError50DS::material,"RA fill-color program is not implemented");
        }
        try {
            raWorldGeometry.resize(raWorld->vertices_.size());
            raWorldFrame.resize(raWorld->vertices_.size());
            raWorldFrameImages.resize(raWorld->surfaceDrawPlan_.size());
        }catch(...){return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate complete RA frame streams");}
        // The retained cold world owns immutable position/UV lanes. Crowd
        // animation selects images. Keep current float RGBA/fog in a separate
        // stream so each frame uploads only those lanes, without repacking.
        // Reinitialize the immutable copies on every GPU creation.
        for(std::size_t v=0;v<raWorldGeometry.size();++v){
            const auto& input=raWorld->vertices_[v];auto& output=raWorldGeometry[v];
            std::copy(input.worldPosition.begin(),input.worldPosition.end(),output.position);
            std::copy(input.sourceUv.begin(),input.sourceUv.end(),output.uv);
        }
        const std::uint64_t vb=raWorldGeometry.size()*sizeof(RaWorldVertex),
            fb=raWorldFrame.size()*sizeof(RaWorldFrameVertex),ib=raWorld->indices_.size()*sizeof(U);
        if(!vb||!fb||!ib||vb>std::uint64_t(std::numeric_limits<std::intptr_t>::max())||
           fb>std::uint64_t(std::numeric_limits<std::intptr_t>::max())||
           ib>std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
            return fail(*diagnostic,VehicleWindowError50DS::limits,"RA source buffer extent exceeds GL limits");
        gl.genVertexArrays(1,&raWorldArray);drawGl.bindVertexArray(raWorldArray);
        gl.genBuffers(1,&raWorldVertices);drawGl.bindBuffer(arrayBuffer,raWorldVertices);
        gl.bufferData(arrayBuffer,static_cast<std::intptr_t>(vb),raWorldGeometry.data(),staticDraw);
        const I staticCounts[]={3,2};const std::size_t staticOffsets[]={offsetof(RaWorldVertex,position),offsetof(RaWorldVertex,uv)};
        for(U i=0;i<2;++i){gl.enableVertexAttribArray(i);gl.vertexAttribPointer(i,staticCounts[i],0x1406,0,sizeof(RaWorldVertex),reinterpret_cast<const void*>(staticOffsets[i]));}
        gl.genBuffers(1,&raWorldOperands);drawGl.bindBuffer(arrayBuffer,raWorldOperands);
        // A fresh renderer's value-initialized CPU records define every GPU
        // operand, including unselected surfaces inspected by diagnostics.
        gl.bufferData(arrayBuffer,static_cast<std::intptr_t>(fb),raWorldFrame.data(),dynamicDraw);
        const I frameCounts[]={4,1};const std::size_t frameOffsets[]={offsetof(RaWorldFrameVertex,color),offsetof(RaWorldFrameVertex,fog)};
        for(U i=0;i<2;++i){gl.enableVertexAttribArray(i+2);gl.vertexAttribPointer(i+2,frameCounts[i],0x1406,0,sizeof(RaWorldFrameVertex),reinterpret_cast<const void*>(frameOffsets[i]));}
        gl.genBuffers(1,&raWorldIndices);drawGl.bindBuffer(elementBuffer,raWorldIndices);
        gl.bufferData(elementBuffer,static_cast<std::intptr_t>(ib),raWorld->indices_.data(),staticDraw);
        if(!check("upload complete RA world geometry/index banks and allocate current source operands")||
           !raWorldArray||!raWorldVertices||!raWorldOperands||!raWorldIndices)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"RA native geometry allocation failed");
        std::int64_t actual=0;gl.getBufferParameteri64v(arrayBuffer,bufferSize,&actual);
        if(actual!=static_cast<std::int64_t>(fb))return fail(*diagnostic,VehicleWindowError50DS::allocation,"RA current operand storage size differs");
        drawGl.bindBuffer(arrayBuffer,raWorldVertices);gl.getBufferParameteri64v(arrayBuffer,bufferSize,&actual);
        if(actual!=static_cast<std::int64_t>(vb))return fail(*diagnostic,VehicleWindowError50DS::allocation,"RA vertex storage size differs");
        drawGl.bindBuffer(arrayBuffer,raWorldOperands);
        gl.getBufferParameteri64v(elementBuffer,bufferSize,&actual);
        if(actual!=static_cast<std::int64_t>(ib))return fail(*diagnostic,VehicleWindowError50DS::allocation,"RA index storage size differs");
        if(!check("verify split RA vertex storage extents"))return false;
        constexpr const char* vertex=R"GLSL(#version 400 core
in vec3 position;in vec2 uv;in vec4 color;in float fogByte;
uniform mat4 worldViewProjection,sourceWorldViewProjection;
out SourceVertex { vec4 clip;vec2 uv;vec4 color;float fog;flat int sourceIndex; } operand;
void main(){
    gl_Position=vec4(position,1.0)*worldViewProjection;
    operand.clip=vec4(position,1.0)*sourceWorldViewProjection;
    operand.uv=uv;operand.color=color;operand.fog=fogByte;operand.sourceIndex=gl_VertexID;
}
)GLSL";
        // Authenticated common clipper 366450, called by PrelitCompressed.
        // Source near is Z+W; native depth conversion makes it GL Z+3W.
        // Keep both positions: GL still owns final depth clipping/rasterization.
        constexpr const char* geometry=R"GLSL(#version 400 core
layout(triangles) in;
layout(triangle_strip,max_vertices=21) out;
in SourceVertex { vec4 clip;vec2 uv;vec4 color;float fog;flat int sourceIndex; } operand[];
out vec2 sourceUv;smooth out vec4 packetColorTimesClipW;
smooth out float packetFogTimesClipW;
struct Vertex { vec4 clip;vec4 nativeClip;vec2 uv;vec4 color;float fog; };
Vertex polygon[9],nextPolygon[9];
float distanceToPlane(vec4 p,int plane){
    int axis=2-plane/2;
    return (plane%2==0)?p.w+p[axis]:p.w-p[axis];
}
Vertex intersection(Vertex a,Vertex b,float da,float db,bool aInside){
    // 367330 / 367380 retain the entering/leaving endpoint forms.
    float q=(aInside?da:db)/(da-db);
    Vertex origin=a;
    if(!aInside)origin=b;
    Vertex result;
    result.clip=origin.clip+q*(b.clip-a.clip);
    result.nativeClip=origin.nativeClip+q*(b.nativeClip-a.nativeClip);
    result.uv=origin.uv+q*(b.uv-a.uv);
    result.color=origin.color+q*(b.color-a.color);
    result.fog=origin.fog+q*(b.fog-a.fog);
    return result;
}
void emitPacket(Vertex v){
    gl_Position=v.nativeClip;sourceUv=v.uv;
    // 367160 / 367180: undo post-near color weighting, then FTOI0.
    vec4 rawColor=floor(clamp(v.color/v.clip.w,0.0,255.0));
    // Source clipping's C*sourceW representation ends above. The separate
    // native transport below carries final byte RGBA through GL clipping.
    packetColorTimesClipW=rawColor*gl_Position.w;
    // 367188 FTOI4 packs F in bits 4..11. Fog was never W-weighted
    // during source clipping. Transport only AFTER polygon generation.
    packetFogTimesClipW=floor(clamp(v.fog,0.0,255.0))*gl_Position.w;
    EmitVertex();
}
void main(){
    int count=3;bool clipped=false;
    for(int i=0;i<3;++i){
        polygon[i].clip=operand[i].clip;polygon[i].nativeClip=gl_in[i].gl_Position;
        polygon[i].uv=operand[i].uv;polygon[i].color=operand[i].color;
        polygon[i].fog=operand[i].fog;
        for(int plane=0;plane<6;++plane)
            clipped=clipped || distanceToPlane(polygon[i].clip,plane)<0.0;
    }
    if(!clipped){
        // No source clip-generated operands: keep the original byte values.
        for(int i=0;i<3;++i){
            gl_Position=gl_in[i].gl_Position;sourceUv=operand[i].uv;
            packetColorTimesClipW=operand[i].color*gl_Position.w;
            packetFogTimesClipW=operand[i].fog*gl_Position.w;EmitVertex();
        }
        EndPrimitive();return;
    }
    // 3665F8..3667F0 assembles oldest, middle, current source records.
    // The importer swaps the first two indices for odd strip triangles;
    // undo that only for clipped polygons, preserving the source fan anchor.
    if(operand[0].sourceIndex>operand[1].sourceIndex){
        Vertex first=polygon[0];polygon[0]=polygon[1];polygon[1]=first;
    }
    // Convex triangle intersected by six half spaces has at most 3+6
    // vertices. Each pass adds at most one; fan output is at most 21.
    // Source order: -Z,+Z,-Y,+Y,-X,+X (366870..366FD8).
    for(int plane=0;plane<6;++plane){
        int nextCount=0;
        for(int i=0;i<count;++i){
            Vertex a=polygon[(i+count-1)%count],b=polygon[i];
            float da=distanceToPlane(a.clip,plane),db=distanceToPlane(b.clip,plane);
            bool aInside=da>=0.0,bInside=db>=0.0;
            // The source stores A on inside edges, starting last -> first.
            if(aInside)nextPolygon[nextCount++]=a;
            // Preserve coincident boundary records: later source plane
            // rotations depend on their count and therefore the fan anchor.
            // At most two crossings, with at least one excluded vertex,
            // imply nextCount<=count+1 even with endpoint intersections.
            if(aInside!=bInside)nextPolygon[nextCount++]=intersection(a,b,da,db,aInside);
        }
        count=nextCount;
        if(count<3)return;
        for(int i=0;i<count;++i){
            polygon[i]=nextPolygon[i];
            // 3669C0: raw RGBA at first near intersection, C*W thereafter.
            if(plane==0)polygon[i].color*=polygon[i].clip.w;
        }
    }
    for(int i=1;i<count-1;++i){
        emitPacket(polygon[0]);emitPacket(polygon[i]);emitPacket(polygon[i+1]);
        EndPrimitive();
    }
}
)GLSL";
        constexpr const char* fragment=R"GLSL(
in vec2 sourceUv;smooth in vec4 packetColorTimesClipW;smooth in float packetFogTimesClipW;
uniform sampler2D image;uniform vec4 fogColorAndEnable;uniform float alphaReference;
#ifndef RA_SAMPLE_DESTINATION
uniform int payload;
out float sourceDepth;
#else
uniform sampler2DMS destinationImage;
#endif
out vec4 sourceColor;
void main(){
    // Texture bank already incorporates source TEXA. Native base-level linear
    // sampling; packet Gouraud/fog are screen-linear, UV is perspective-correct.
    vec4 sampled=floor(clamp(texture(image,sourceUv)*255.0,0.0,255.0));
    // Carry all original RGBA lanes homogeneously, as with fog below.
    // Reconstruct screen-linear color before the unchanged byte operations.
    vec4 packet=floor(clamp(packetColorTimesClipW*gl_FragCoord.w,0.0,255.0));
    vec4 bytes=floor(clamp(sampled*packet/128.0,0.0,255.0));
    if(alphaReference>=0.0 && bytes.a<=alphaReference)discard;
    if(fogColorAndEnable.w!=0.0){
        // Smooth transport of F*clipW, then multiply by interpolated 1/W.
        // This reconstructs screen-linear F without a clip-generated F/W
        // division near W=0. Preserve the existing byte clamp/quantization.
        float f=floor(clamp(packetFogTimesClipW*gl_FragCoord.w,0.0,255.0));
        bytes.rgb=clamp(floor((bytes.rgb*f+fogColorAndEnable.rgb*(255.0-f))/256.0),0.0,255.0);
    }
#ifndef RA_SAMPLE_DESTINATION
    sourceDepth=gl_FragCoord.z;
    sourceColor=payload!=0?bytes:bytes/255.0;
#else
    vec3 destination=floor(texelFetch(destinationImage,ivec2(gl_FragCoord.xy),gl_SampleID).rgb*255.0+0.5);
    bytes.rgb=clamp(destination+floor((bytes.rgb-destination)*bytes.a/128.0),0.0,255.0);
    sourceColor=bytes/255.0;
#endif
}
)GLSL";
        I geometryVertices=0,geometryComponents=0;
        gl.getIntegerv(0x8DE0,&geometryVertices); // GL_MAX_GEOMETRY_OUTPUT_VERTICES
        gl.getIntegerv(0x8DE1,&geometryComponents); // GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS
        if(!check("query RA source clip geometry limits")||geometryVertices<21||geometryComponents<21*11)
            return fail(*diagnostic,VehicleWindowError50DS::limits,"RA source clip geometry limits insufficient");
        if(!compileTrackShader(0x8B31,vertex,raWorldVertexShader)||
           !compileTrackShader(0x8DD9,geometry,raWorldGeometryShader))return false;
        try {
        for(U i=0;i<2;++i){
            std::string source=i?"#version 400 core\n#define RA_SAMPLE_DESTINATION\n":"#version 400 core\n";
            source+=fragment;
            if(!compileTrackShader(0x8B30,source.c_str(),raWorldFragment[i]))return false;
            auto& p=raWorldProgram[i];p=shaderGl.createProgram();
            if(!p)return fail(*diagnostic,VehicleWindowError50DS::shader,"zero RA world program");
            shaderGl.attachShader(p,raWorldVertexShader);shaderGl.attachShader(p,raWorldGeometryShader);
            shaderGl.attachShader(p,raWorldFragment[i]);
            const char* names[]={"position","uv","color","fogByte"};
            for(U a=0;a<4;++a)shaderGl.bindAttribLocation(p,a,names[a]);
            shaderGl.bindFragDataLocation(p,0,"sourceColor");if(!i)shaderGl.bindFragDataLocation(p,1,"sourceDepth");
            shaderGl.linkProgram(p);I linked=0;shaderGl.getProgramiv(p,0x8B82,&linked);
            if(!check("link native RA source-byte material shader")||!linked){
                shaderGl.getProgramInfoLog(p,sizeof(diagnostic->shader.log),nullptr,diagnostic->shader.log);
                diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]=0;
                return fail(*diagnostic,VehicleWindowError50DS::shader,"native RA world shader link failed");}
            auto& u=raWorldUniform[i];u.matrix=shaderGl.getUniformLocation(p,"worldViewProjection");
            u.sourceMatrix=shaderGl.getUniformLocation(p,"sourceWorldViewProjection");
            u.image=shaderGl.getUniformLocation(p,"image");u.fog=shaderGl.getUniformLocation(p,"fogColorAndEnable");
            u.alpha=shaderGl.getUniformLocation(p,"alphaReference");
            u.payload=shaderGl.getUniformLocation(p,"payload");u.destination=shaderGl.getUniformLocation(p,"destinationImage");
            if(u.matrix<0||u.sourceMatrix<0||u.image<0||u.fog<0||u.alpha<0||(i?u.destination<0:u.payload<0))
                return fail(*diagnostic,VehicleWindowError50DS::shader,"RA source uniforms missing");
        }
        }catch(...){return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate RA shader source");}
        drawGl.bindVertexArray(0);return check("finish native RA geometry and material programs");
    }

bool VehicleWindowGl50DS::Storage::prepareRaWorldFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d,bool& flareEntered) noexcept {
        raWorldFramePrepared=false;
        if(!raWorld)return true;
        if(raLocalFlareFailed)return fail(d,VehicleWindowError50DS::frame,"previous flare transaction requires renderer retirement");
        if(thread!=std::this_thread::get_id()||active||frameSet||diagnostic)
            return fail(d,VehicleWindowError50DS::frame,"RA preparation requires idle owning renderer");
        if(!current(d))return false;
        diagnostic=&d;
        struct PreparationDiagnostic {Storage& owner;~PreparationDiagnostic(){owner.diagnostic=nullptr;}} preparationDiagnostic{*this};
        bool completed=false;
        struct FlareFailure {bool& failed;bool& entered;bool& completed;
            ~FlareFailure(){if(entered&&!completed)failed=true;}} flareFailure{raLocalFlareFailed,flareEntered,completed};
        if(!raWorldCurrent(d))return false;
        using N=ra_static_fog_numeric50LA::Status;
        ra_cold_aggregate50LA::Diagnostic visibility;
        if(!request.raStaticWorld->prepareNativeStaticDrawPublication(visibility)){
            std::fprintf(stderr,"RA native visibility rejected: stage=%u failure=%s lighting_status=%u source_pc=0x%08x\n",
                unsigned(visibility.stage),visibility.failure?visibility.failure:"",
                unsigned(visibility.lighting.status),unsigned(visibility.lighting.sourcePC));
            return fail(d,VehicleWindowError50DS::frame,"RA current original portal/model/LOD selection failed");
        }
        const auto* selected=request.raStaticWorld->nativeStaticDrawPublication();
        if(!selected)return fail(d,VehicleWindowError50DS::frame,"RA native draw publication expired");
        raWorldFrameDraws.clear();raLocalFlareFrames.clear();
        try {
            bool havePreviousScissor=false;
            std::array<std::uint32_t,4> previousSourceScissor{};
            std::array<std::int32_t,4> previousViewport{};
            std::array<I,4> previousScissor{};
            for(const auto& record:*selected){
                using Kind=game::original_gc_gameplay_lighting_candidate::NativeRaStaticDraw50LA::Kind;
                const bool localFlare=record.kind==Kind::lightFlare;
                std::size_t firstSurface=0,surfaceCount=0;
                if(!localFlare){
                if(record.kind!=Kind::model)return fail(d,VehicleWindowError50DS::frame,"unknown actual RA draw kind");
                // Cold construction appends each visual placement in strictly
                // increasing source world-instance order. The live renderer
                // publication prevents close/reordering of this same vector.
                const auto found=std::lower_bound(raWorld->placements_.begin(),raWorld->placements_.end(),
                    record.worldInstance,[](const auto& owned,std::size_t instance){return owned->worldInstance<instance;});
                const ra_static_world50LA::Owner::Placement* placement=
                    found!=raWorld->placements_.end()&&(*found)->worldInstance==record.worldInstance?found->get():nullptr;
                if(!placement||record.sourceLod>=placement->lodRanges.size())
                    return fail(d,VehicleWindowError50DS::assets,"RA selected authentic LOD has no retained geometry");
                const auto& range=placement->lodRanges[record.sourceLod];
                firstSurface=range.firstSurface;surfaceCount=range.surfaceCount;
                }
                if(firstSurface>raWorld->surfaceDrawPlan_.size()||
                   surfaceCount>raWorld->surfaceDrawPlan_.size()-firstSurface||
                   record.viewport[2]<=0||record.viewport[3]<=0||
                   record.scissor[0]>record.scissor[1]||record.scissor[2]>record.scissor[3])
                    return fail(d,VehicleWindowError50DS::assets,"RA selected surface/scissor range differs");
                // Original scissor endpoints are inclusive and top-origin.
                // Native framebuffer uses bottom-origin and actual pixel extent.
                std::array<I,4> scissor{};
                if(havePreviousScissor&&record.viewport==previousViewport&&
                   record.scissor==previousSourceScissor){
                    scissor=previousScissor;
                }else{
                    const double x=record.viewport[0],y=record.viewport[1];
                    const double w=record.viewport[2],h=record.viewport[3];
                    const double bounds[4]={
                        (double(record.scissor[0])-x)/w*frame.width,
                        (y+h-double(record.scissor[3])-1.)/h*frame.height,
                        (double(record.scissor[1])+1.-x)/w*frame.width,
                        (y+h-double(record.scissor[2]))/h*frame.height};
                    for(U axis=0;axis<4;++axis){
                        const double extent=(axis&1u)?frame.height:frame.width;
                        const double rounded=axis<2?std::floor(bounds[axis]):std::ceil(bounds[axis]);
                        scissor[axis]=static_cast<I>(std::max(0.,std::min(extent,rounded)));
                    }
                    previousViewport=record.viewport;previousSourceScissor=record.scissor;
                    previousScissor=scissor;havePreviousScissor=true;
                }
                if(localFlare){
                    // Queue identity only. Actual numeric/material services run
                    // in the same ordered pass as preceding model services.
                    raWorldFrameDraws.push_back({0,scissor,true,static_cast<std::size_t>(-1),
                        static_cast<std::size_t>(&record-selected->data())});
                    havePreviousScissor=false;
                    continue;
                }
                if(scissor[0]>=scissor[2]||scissor[1]>=scissor[3])continue;
                for(std::size_t surface=firstSurface;surface<firstSurface+surfaceCount;++surface){
                    const auto& plan=raWorld->surfaceDrawPlan_[surface];
                    if(plan.worldInstance!=record.worldInstance||plan.sourceLod!=record.sourceLod||plan.surface!=surface)
                        return fail(d,VehicleWindowError50DS::assets,"RA native selection and source draw inventory differ");
                    // Never reuse computed FP results across a possible allocator callback.
                    if(raWorldFrameDraws.size()==raWorldFrameDraws.capacity())havePreviousScissor=false;
                    raWorldFrameDraws.push_back({surface,scissor});
                }
            }
        }catch(...){return fail(d,VehicleWindowError50DS::allocation,"allocate ordered RA selected frame surfaces");}
        // Empty visibility is valid; it still owns the same actual camera.
        ra_static_fog50LA::PlacementObservation firstPlacement;
        if(raWorld->surfaceDrawPlan_.empty()||!request.raStaticWorld->readNativeStaticFogObservation(
            raWorld->surfaceDrawPlan_.front().placement,raWorldCamera,firstPlacement))
            return fail(d,VehicleWindowError50DS::frame,"RA current source camera/fog binding required");
        NativeCameraViewContract50DX contract;
        contract.provenance=NativeCameraViewProvenance50DX::currentCore49FForOutput;
        contract.width=frame.width;contract.height=frame.height;contract.clipDepth=NativeCameraClipDepth50DX::negativeOneToOne;
        NativeCameraMatrix50DX projected;NativeCameraViewError50DX error;
        if(!convertNativeCameraView50DX(raWorldCamera.camera,contract,projected,error)||
           std::memcmp(projected.worldViewProjection,frame.worldViewProjection,sizeof(projected.worldViewProjection)))
            return fail(d,VehicleWindowError50DS::frame,"RA world requires exact current fog-prepared camera matrix");
        for(U row=0;row<4;++row)for(U column=0;column<4;++column)
            raWorldSourceMatrix[column*4+row]=raWorldCamera.camera.combined[row*4+column];
        for(auto& selectedDraw:raWorldFrameDraws){
            if(selectedDraw.localFlare){
                const auto* currentSelection=request.raStaticWorld->nativeStaticDrawPublication();
                if(currentSelection!=selected||!currentSelection||selectedDraw.record>=currentSelection->size())
                    return fail(d,VehicleWindowError50DS::frame,"actual mixed flare publication changed");
                const auto& record=(*currentSelection)[selectedDraw.record];
                flareEntered=true;
                RaLocalFlareFrame next;assets::OriginalStandardMaterialDiagnostic50LA material;
                // Native direct visitor replaces unjoined renderer+40/20 in
                // 141DD8, not source141D70/14B8E0 return authority.
                const bool visited=request.raStaticWorld->visitNativeLocalFlareEmission(record,
                    [&](auto& strip,const auto& table,auto& detail)noexcept {
                        try {next.operands=std::make_unique<LocalFlareEmission::Operands>();}
                        catch(...){return false;}
                        const auto result=LocalFlareEmission::capture23CA50(strip,table,*next.operands,detail);
                        if(result==LocalFlareEmission::Outcome::blocked)return false;
                        if(result==LocalFlareEmission::Outcome::childrenOnly)next.operands.reset();
                        return true;
                    },material);
                if(!visited){
                    std::fprintf(stderr,"ra_local_flare_refused record=%zu status=%u pc=%x\n",selectedDraw.record,
                        unsigned(material.status),unsigned(material.sourcePC));
                    return fail(d,VehicleWindowError50DS::frame,"actual local flare numeric/material/strip visitor refused");
                }
                const auto& scissor=selectedDraw.scissor;
                if(next.operands&&scissor[0]<scissor[2]&&scissor[1]<scissor[3]){
                    if(!finishRaLocalFlareFrame(next))
                        return fail(d,VehicleWindowError50DS::frame,"typed flare native frame operands/resources refused");
                    try {selectedDraw.flare=raLocalFlareFrames.size();raLocalFlareFrames.push_back(std::move(next));}
                    catch(...){return fail(d,VehicleWindowError50DS::allocation,"retain actual flare frame operands");}
                }
                continue;
            }
            const auto& plan=raWorld->surfaceDrawPlan_[selectedDraw.surface];
            ra_static_fog50LA::CameraObservation camera;ra_static_fog50LA::PlacementObservation placement;
            if(!request.raStaticWorld->readNativeStaticFogObservation(plan.placement,camera,placement))
                return fail(d,VehicleWindowError50DS::frame,"current RA source camera/fog binding required");
            if(camera.generation!=raWorldCamera.generation||camera.publication!=raWorldCamera.publication||
               camera.frame!=raWorldCamera.frame||camera.fogRevision!=raWorldCamera.fogRevision)
                return fail(d,VehicleWindowError50DS::frame,"RA source camera changed during frame preparation");
            const auto& raster=raWorld->rasterInventory_[plan.raster];
            std::size_t image=raster.firstImage;
            if(raster.dynamicCrowdMap){
                ra_cold_aggregate50LA::Diagnostic service;
                ra_static_world50LA::Owner::ModelCrowdMap selected;ra_static_world50LA::Owner::Diagnostic map;
                if(!request.raStaticWorld->serviceStaticCrowdSurface241848(plan.surface,service)||
                   !raWorld->currentCrowdSurfaceMap24B26C(plan.surface,selected,map))
                    return fail(d,VehicleWindowError50DS::material,"RA actual crowd model current-map service failed");
                image=selected.imageIndex;
            }
            const auto* texture=request.raStaticWorldTextures->asset(static_cast<U>(image));
            if(!texture||!texture->name)return fail(d,VehicleWindowError50DS::material,"RA current map lacks retained GPU image");
            raWorldFrameImages[plan.surface]=texture->name;
            ra_static_fog_numeric50LA::CompressedPreamble fog;
            if(raster.fragmentDemands.sourceFogByte && placement.computeFogCandidate02){
                const float slope=ra_static_fog_numeric50LA::detail::div(255.f,
                    ra_static_fog_numeric50LA::detail::sub(camera.source.start34,camera.source.end38));
                if(ra_static_fog_numeric50LA::prepareCompressed(placement.packetModel,camera.position,
                    placement.positionScale,camera.source.end38,slope,fog)!=N::complete)
                    return fail(d,VehicleWindowError50DS::frame,"RA compressed source fog operands rejected");
            }
            using DiffuseVertex=game::original_racer_lighting50KM::DiffuseVertexRgba;
            bool visited=false,callbackSucceeded=false;
            const auto prepareVertices=[&](const ra_static_lighting50LA::StaticDiffuseSurface* identity,
                const std::vector<DiffuseVertex>* diffuse) noexcept {
                visited=true;
                if(diffuse&&(!identity||identity->cameraGeneration!=camera.generation||
                   identity->cameraPublication!=camera.publication||identity->cameraFrame!=camera.frame||
                   diffuse->size()<plan.vertexCount))
                    return fail(d,VehicleWindowError50DS::frame,"RA current source diffuse bytes unavailable");
                const auto prepareWithRawReader=[&](const auto& readRaw) noexcept {
                    for(U v=0;v<plan.vertexCount;++v){
                        const auto& input=raWorld->vertices_[plan.vertexStart+v];auto& output=raWorldFrame[plan.vertexStart+v];
                        std::array<std::uint8_t,4> color;
                        if(diffuse){
                            color=(*diffuse)[v].rgba;
                        }else {if(!plan.hasOriginalColors)return fail(d,VehicleWindowError50DS::assets,"RA prelit source colors absent");color=input.sourceColor;}
                        for(U a=0;a<4;++a)output.color[a]=color[a];
                        std::uint8_t fogByte=ra_static_fog_numeric50LA::noComputeFogByte;
                        if(raster.fragmentDemands.sourceFogByte&&placement.computeFogCandidate02){
                            std::array<std::int32_t,3> raw;
                            if(!readRaw(v,raw)||
                               ra_static_fog_numeric50LA::radialByte(fog,raw,fogByte)!=N::complete)
                                return fail(d,VehicleWindowError50DS::frame,"RA original raw position/fog byte unavailable");
                        }
                        output.fog=fogByte;
                    }
                    return true;
                };
                if(raster.fragmentDemands.sourceFogByte&&placement.computeFogCandidate02){
                    bool readVisited=false,readSucceeded=false;
                    const bool prepared=request.raStaticWorld->visitNativeStaticRawPositionSurface(plan.surface,
                        [&](const auto& readRaw) noexcept {
                            readVisited=true;readSucceeded=prepareWithRawReader(readRaw);return readSucceeded;
                        });
                    if(!prepared){
                        if(!readVisited||readSucceeded)
                            return fail(d,VehicleWindowError50DS::frame,"RA original raw position/fog byte unavailable");
                        return false;
                    }
                    return true;
                }
                // The actual no-fog branch cannot invoke this reader.
                return prepareWithRawReader([](std::size_t,std::array<std::int32_t,3>&) noexcept {return false;});
            };
            if(raster.requiresNativeStaticLighting){
                const bool prepared=request.raStaticWorld->visitNativeStaticDiffuseSurface(plan.surface,
                    [&](const auto& identity,const auto& diffuse) noexcept {
                        callbackSucceeded=prepareVertices(&identity,&diffuse);
                        return callbackSucceeded;
                    });
                if(!prepared){
                    if(!visited||callbackSucceeded)return fail(d,VehicleWindowError50DS::frame,"RA current source diffuse bytes unavailable");
                    return false;
                }
            }else if(!prepareVertices(nullptr,nullptr))return false;
        }
        if(!request.raStaticWorld->nativeStaticDrawPublication()||!raWorldCurrent(d))return fail(d,VehicleWindowError50DS::frame,"RA current frame has no complete source plan");
        raWorldFramePrepared=true;completed=true;return true;
    }

bool VehicleWindowGl50DS::Storage::raWorldFrameCurrent(VehicleWindowDiagnostic50DS& d) const noexcept {
        if(!raWorldFramePrepared||!raWorldCurrent(d)||!request.raStaticWorld->nativeStaticDrawPublication()||raWorld->surfaceDrawPlan_.empty())
            return fail(d,VehicleWindowError50DS::frame,"RA frame operands were not prepared");
        // Other frame callbacks run after CPU preparation. Recheck the actual
        // camera/fog lease at draw time rather than relying on cached numbers.
        ra_static_fog50LA::CameraObservation camera;
        ra_static_fog50LA::PlacementObservation placement;
        if(!request.raStaticWorld->readNativeStaticFogObservation(
               raWorld->surfaceDrawPlan_.front().placement,camera,placement)||
           camera.generation!=raWorldCamera.generation||camera.publication!=raWorldCamera.publication||
           camera.frame!=raWorldCamera.frame||camera.fogRevision!=raWorldCamera.fogRevision)
            return fail(d,VehicleWindowError50DS::frame,"RA prepared camera/fog publication changed before draw");
        for(const auto& frame:raLocalFlareFrames)
            if(!frame.operands||!LocalFlareEmission::materialCurrent(*frame.operands))
                return fail(d,VehicleWindowError50DS::frame,"flare originating/final material publication expired");
        return true;
    }

void VehicleWindowGl50DS::Storage::bindRaWorldMaterial(const ra_static_world50LA::Owner::RasterInventory& raster,U program,bool payload,
        bool uploadUniforms) noexcept {
        using F=assets::OriginalUiGsField50KG;
        const auto field=[&](F f){return raster.finalContext.fields[static_cast<std::size_t>(f)].value;};
        const auto& u=raWorldUniform[program];
        // Program1 is used only by direct AA1 blended draws: later checked
        // primitives retain it. Program0 still binds for every AA0 payload.
        if(uploadUniforms || program==0)gl.useProgram(raWorldProgram[program]);
        // The selected draw's material/camera uniforms do not vary.
        if(!uploadUniforms)return;
        gl.uniformMatrix4fv(u.matrix,1,0,matrix);
        gl.uniformMatrix4fv(u.sourceMatrix,1,0,raWorldSourceMatrix);gl.uniform1i(u.image,0);
        const U color=raWorldCamera.source.color30;
        //Original color30 is packed ARGB, not a GS-native low-byte-R word.
        //RA SCB colorrgb180,180,165 is retained as0xffb4b4a5 by the actual
        //fog owner;13E990 passes that packed word unchanged to renderer+28.
        gl.uniform4f(u.fog,float((color>>16)&255),float((color>>8)&255),float(color&255),field(F::foggingOnOff)?1.f:0.f);
        gl.uniform1f(u.alpha,field(F::alphaTestOnOff)?float(field(F::alphaReferenceValue)):-1.f);
        if(program)gl.uniform1i(u.destination,2);else gl.uniform1i(u.payload,payload?1:0);
    }

bool VehicleWindowGl50DS::Storage::snapshotRaWorld(U bank,const std::array<I,4>& region) noexcept {
        if(gl.copyImageSubData){
            // Same distinct RGBA8/four-sample snapshot as the racer bridge.
            gl.copyImageSubData(coverageLive.color[bank],coverageTexture,0,
                region[0],region[1],0,coverageLive.snapshotColor,coverageTexture,0,
                region[0],region[1],0,region[2]-region[0],region[3]-region[1],1);
        }else{
            // Extent creation checked these private FBO selectors; binds retain them.
            gl.bindFramebuffer(readFramebuffer,coverageLive.scene[bank]);
            gl.bindFramebuffer(drawFramebuffer,coverageLive.snapshot);
            gl.blitFramebuffer(region[0],region[1],region[2],region[3],
                region[0],region[1],region[2],region[3],colorBufferBit,0x2600);
        }
        return check("retain actual per-sample destination before next RA primitive");
    }

bool VehicleWindowGl50DS::Storage::raTriangleOutsideNativeClip(U first) const noexcept {
        // Skip only triangles provably outside one common GL clip plane.
        // Conservative error bounds cover float dot-product rounding on GPU;
        // uncertain/nonfinite cases retain the original ordered submission.
        unsigned common=63u;
        for(U corner=0;corner<3;++corner){
            if(first+corner>=raWorld->indices_.size())return false;
            const auto index=raWorld->indices_[first+corner];
            if(index>=raWorldGeometry.size())return false;
            const auto& v=raWorldGeometry[index];
            const double p[4]={v.position[0],v.position[1],v.position[2],1.};
            // Reuse only identical products, never reassociate a plane sum.
            // Populate each axis at its first original plane so an earlier
            // nonfinite rejection does not evaluate previously unreached axes.
            double products[4][4];
            unsigned outside=0;
            for(U plane=0;plane<6;++plane){
                const U axis=plane/2;const double sign=(plane&1)?-1.:1.;
                double value=0,scale=0;
                for(U k=0;k<4;++k){
                    // Same row-vector product as GLSL; each output lane
                    // reads one column of the column-major uniform storage.
                    if(plane==0)products[3][k]=double(matrix[3*4+k])*p[k];
                    if(!(plane&1))products[axis][k]=double(matrix[axis*4+k])*p[k];
                    const double w=products[3][k];
                    const double a=products[axis][k];
                    value+=w+sign*a;scale+=std::abs(w)+std::abs(a);
                }
                if(!std::isfinite(value)||!std::isfinite(scale))return false;
                const double margin=32.*std::numeric_limits<float>::epsilon()*scale+0.0001;
                if(value < -margin)outside|=1u<<plane;
            }
            common&=outside;if(!common)return false;
        }
        return common!=0;
    }

void VehicleWindowGl50DS::Storage::beginRacerRasterOperandGroup(U slot,U component,U first,U count)noexcept {
        racerRasterCacheActive=false;
        if(count<3)return;
        for(auto& entry:racerRasterOperands)entry.occupied=false;
        racerRasterSlot=slot;racerRasterComponent=component;
        racerRasterFirst=first;racerRasterCount=count;
        racerRasterCacheActive=true;
    }

void VehicleWindowGl50DS::Storage::endRacerRasterOperandGroup()noexcept {racerRasterCacheActive=false;}

bool VehicleWindowGl50DS::Storage::nativeRasterVertex(const std::array<float,3>& v,NativeRasterVertex& out)const noexcept {
        out={};const double p[4]={v[0],v[1],v[2],1.};
        // Preserve the previous per-column/per-term arithmetic and checks.
        for(U column=0;column<4;++column){
            double magnitude=0;
            for(U k=0;k<4;++k){const double term=double(matrix[column*4+k])*p[k];out.clip[column]+=term;magnitude+=std::abs(term);}
            out.error[column]=32.*std::numeric_limits<float>::epsilon()*magnitude+0.0001;
            if(!std::isfinite(out.clip[column])||!std::isfinite(out.error[column]))return false;
        }
        for(U plane=0;plane<6;++plane){const U axis=plane/2;
            const double value=out.clip[3]+((plane&1)?-out.clip[axis]:out.clip[axis]);
            if(value < -(out.error[3]+out.error[axis]))out.outside|=1u<<plane;
        }
        return true;
    }

std::array<I,4> VehicleWindowGl50DS::Storage::nativeTriangleRasterOperands(const std::array<NativeRasterVertex,3>& vertices,
        const U* cacheIndices)const noexcept {
        const std::array<I,4> full{{0,0,width,height}};
        unsigned common=63u;
        for(const auto& vertex:vertices)common&=vertex.outside;
        // Fully clipped triangles have no payload or destination consumers,
        // including triangles entirely behind the camera. Do not expand
        // those into a costly full-target fallback merely because W is low.
        if(common)return {{0,0,0,0}};
        double low[2]={1.,1.},high[2]={-1.,-1.};
        for(U corner=0;corner<3;++corner){
            const auto& c=vertices[corner].clip;const auto& e=vertices[corner].error;
            if(c[3]<=e[3]+0.0001)return full;
            // The current group supplies indices only after validating its scope.
            // Operand construction can replace colliding slots; reacquire and
            // check each tag here, keeping the local operand copy authoritative.
            CachedNativeRasterVertex* projection=nullptr;
            if(cacheIndices&&racerRasterCacheActive){
                const U index=cacheIndices[corner];
                auto& entry=racerRasterOperands[index%racerRasterOperands.size()];
                if(entry.occupied&&entry.finite&&entry.index==index)projection=&entry;
            }
            const bool reuseProjection=projection&&projection->projectionReady;
            U quotient=0;
            for(U axis=0;axis<2;++axis)for(int a:{-1,1})for(int w:{-1,1}){
                const double value=reuseProjection?projection->projectedQuotients[quotient]:
                    (c[axis]+a*e[axis])/(c[3]+w*e[3]);
                if(!std::isfinite(value))return full;
                low[axis]=std::min(low[axis],value);high[axis]=std::max(high[axis],value);
                if(projection&&!reuseProjection)projection->projectedQuotients[quotient]=value;
                ++quotient;
            }
            // Retain ordered scalar values only after all eight are finite.
            // Never cache merged extrema or a partial/nonfinite projection.
            if(projection&&!reuseProjection)projection->projectionReady=true;
        }
        std::array<I,4> region{};
        for(U axis=0;axis<2;++axis){
            const I extent=axis?height:width;
            const double lo=std::max(-1.,std::min(1.,low[axis]));
            const double hi=std::max(-1.,std::min(1.,high[axis]));
            region[axis]=std::max<I>(0,I(std::floor((lo+1.)*.5*extent))-2);
            region[axis+2]=std::min<I>(extent,I(std::ceil((hi+1.)*.5*extent))+2);
            if(region[axis]>=region[axis+2])return full;
        }
        return region;
    }

std::array<I,4> VehicleWindowGl50DS::Storage::nativeTriangleRasterRegion(const std::array<std::array<float,3>,3>& triangle)const noexcept {
        std::array<NativeRasterVertex,3> vertices;
        for(U corner=0;corner<3;++corner)
            if(!nativeRasterVertex(triangle[corner],vertices[corner]))return {{0,0,width,height}};
        return nativeTriangleRasterOperands(vertices);
    }

std::array<I,4> VehicleWindowGl50DS::Storage::raTriangleRasterRegion(U first)const noexcept {
        std::array<std::array<float,3>,3> triangle;
        for(U corner=0;corner<3;++corner){
            if(first+corner>=raWorld->indices_.size())return {{0,0,width,height}};
            const auto index=raWorld->indices_[first+corner];
            if(index>=raWorldGeometry.size())return {{0,0,width,height}};
            std::copy_n(raWorldGeometry[index].position,3,triangle[corner].begin());
        }
        return nativeTriangleRasterRegion(triangle);
    }

std::array<I,4> VehicleWindowGl50DS::Storage::racerTriangleRasterRegion(U slot,U component,U first)const noexcept {
        const std::array<I,4> full{{0,0,width,height}};
        if(slot>frameOpponentCount)return full;
        const auto& batch=slot?frameOpponentBatches[slot-1]:framePlayerBatch;
        if(component>=batch.componentCount)return full;
        const auto& part=batch.component[component];
        // GPU-skinned parts publish no CPU positions: always uncertain, so any
        // exact-route run keeps one barrier per triangle.
        if(part.skin)return full;
        if(!part.vertices||!part.indices||first>part.indexCount||part.indexCount-first<3)return full;
        const bool cache=racerRasterCacheActive&&slot==racerRasterSlot&&component==racerRasterComponent&&
            first>=racerRasterFirst&&first-racerRasterFirst<=racerRasterCount&&
            racerRasterCount-(first-racerRasterFirst)>=3;
        std::array<NativeRasterVertex,3> vertices;
        for(U corner=0;corner<3;++corner){
            const auto index=part.indices[first+corner];if(index>=part.vertexCount)return full;
            auto& entry=racerRasterOperands[index%racerRasterOperands.size()];
            if(!cache||!entry.occupied||entry.index!=index){
                std::array<float,3> position;
                std::copy_n(part.vertices[index].position,3,position.begin());
                entry.projectionReady=false;
                entry.index=index;entry.finite=nativeRasterVertex(position,entry.operand);
                entry.occupied=cache;
            }
            if(!entry.finite)return full;
            vertices[corner]=entry.operand;
        }
        return nativeTriangleRasterOperands(vertices,cache?part.indices+first:nullptr);
    }

void VehicleWindowGl50DS::Storage::traceRaFogGpuAttestation() noexcept {
        if(!gl.getVertexAttribiv||!gl.getVertexAttribPointerv||!gl.getBufferSubData||
           !gl.getIntegerv||!gl.getBufferParameteri64v||!drawGl.bindVertexArray||
           !drawGl.bindBuffer||!drawGl.getError){
            std::fprintf(stderr,"ra_fog_gpu_attestation unavailable=missing_loaded_api gpu_winner_unknown=1\n");return;
        }
        const U priorError=drawGl.getError();
        if(priorError){
            std::fprintf(stderr,"ra_fog_gpu_attestation unavailable=preexisting_gl_error error=%u gpu_winner_unknown=1\n",priorError);return;
        }
        I oldArray=0,oldBuffer=0;
        gl.getIntegerv(0x85B5,&oldArray);gl.getIntegerv(0x8894,&oldBuffer);
        U error=drawGl.getError();
        if(error||oldArray<0||oldBuffer<0){
            std::fprintf(stderr,"ra_fog_gpu_attestation unavailable=binding_query error=%u gpu_winner_unknown=1\n",error);return;
        }
        drawGl.bindVertexArray(raWorldArray);
        constexpr U properties[]={0x8622,0x8623,0x8624,0x8625,0x886A,0x889F,0x88FD};
        // enabled,size,stride,type,normalized,associatedVBO,integer.
        I actual[7]{};
        for(U i=0;i<7;++i)gl.getVertexAttribiv(3,properties[i],&actual[i]);
        void* pointer=nullptr;gl.getVertexAttribPointerv(3,0x8645,&pointer);
        I element=0;gl.getIntegerv(0x8895,&element);
        error=drawGl.getError();
        const auto offset=reinterpret_cast<std::uintptr_t>(pointer);
        const bool layout=!error&&actual[0]==1&&actual[1]==1&&
            actual[2]==I(sizeof(RaWorldFrameVertex))&&actual[3]==I(floatType)&&actual[4]==0&&
            actual[5]==I(raWorldOperands)&&actual[6]==0&&offset==offsetof(RaWorldFrameVertex,fog);
        std::fprintf(stderr,"ra_fog_gpu_attribute vao=%u attribute=3 query_error=%u enabled=%d size=%d stride=%d type=%d normalized=%d vbo=%d integer=%d offset=%llu expected_vbo=%u expected_stride=%zu expected_offset=%zu layout_match=%u element_buffer=%d expected_element=%u gpu_winner_unknown=1\n",
            raWorldArray,error,actual[0],actual[1],actual[2],actual[3],actual[4],actual[5],actual[6],
            static_cast<unsigned long long>(offset),raWorldOperands,sizeof(RaWorldFrameVertex),offsetof(RaWorldFrameVertex,fog),unsigned(layout),element,raWorldIndices);
        drawGl.bindBuffer(arrayBuffer,raWorldOperands);
        std::int64_t vertexBytes=-1;gl.getBufferParameteri64v(arrayBuffer,bufferSize,&vertexBytes);
        const U bufferError=drawGl.getError();
        const std::size_t surface=438;
        if(error||bufferError||vertexBytes<0||surface>=raWorld->surfaceDrawPlan_.size()){
            std::fprintf(stderr,"ra_fog_gpu_attestation unavailable=source_or_buffer query_error=%u buffer_error=%u vertex_bytes=%lld surface=%zu\n",
                error,bufferError,static_cast<long long>(vertexBytes),surface);
        }else{
            const auto& plan=raWorld->surfaceDrawPlan_[surface];
            bool selected=false;for(const auto& draw:raWorldFrameDraws)if(!draw.localFlare&&draw.surface==surface)selected=true;
            for(U triangle:{116u,117u}){
                const std::size_t relative=std::size_t(triangle)*3;
                if(relative>plan.indexCount||plan.indexCount-relative<3||
                   std::size_t(plan.indexStart)>raWorld->indices_.size()||
                   relative+3>raWorld->indices_.size()-std::size_t(plan.indexStart)){
                    std::fprintf(stderr,"ra_fog_gpu_triangle surface=%zu triangle=%u unavailable=index_range\n",surface,triangle);continue;
                }
                for(U corner=0;corner<3;++corner){
                    const auto index=raWorld->indices_[std::size_t(plan.indexStart)+relative+corner];
                    const std::uint64_t byte=std::uint64_t(index)*sizeof(RaWorldFrameVertex)+offsetof(RaWorldFrameVertex,fog);
                    if(index>=raWorldFrame.size()||byte>std::uint64_t(vertexBytes)||
                       sizeof(float)>std::uint64_t(vertexBytes)-byte||
                       byte>std::uint64_t(std::numeric_limits<std::intptr_t>::max())){
                        std::fprintf(stderr,"ra_fog_gpu_vertex surface=%zu triangle=%u corner=%u index=%u unavailable=vertex_range\n",surface,triangle,corner,index);continue;
                    }
                    float gpu=0;gl.getBufferSubData(arrayBuffer,static_cast<std::intptr_t>(byte),sizeof(gpu),&gpu);
                    // Unselected records outside the current upload interval
                    // can differ after an abandoned CPU preparation. Report
                    // that real staging/GPU difference rather than hiding it.
                    const U readError=drawGl.getError();const float cpu=raWorldFrame[index].fog;
                    U gpuBits=0,cpuBits=0;std::memcpy(&gpuBits,&gpu,sizeof(gpuBits));std::memcpy(&cpuBits,&cpu,sizeof(cpuBits));
                    std::fprintf(stderr,"ra_fog_gpu_vertex surface=%zu triangle=%u corner=%u selected=%u index=%u byte=%llu buffer=%u read_error=%u cpu=%.9g gpu=%.9g cpu_bits=%08x gpu_bits=%08x bytes_match=%u fetch_layout_match=%u gpu_winner_unknown=1\n",
                        surface,triangle,corner,unsigned(selected),index,static_cast<unsigned long long>(byte),raWorldOperands,readError,
                        double(cpu),double(gpu),cpuBits,gpuBits,unsigned(!readError&&cpuBits==gpuBits),unsigned(layout));
                }
            }
        }
        // ARRAY_BUFFER is global; VAO restoration does not restore it. Neither
        // queried VAO nor its element/attribute associations were modified.
        drawGl.bindVertexArray(static_cast<U>(oldArray));drawGl.bindBuffer(arrayBuffer,static_cast<U>(oldBuffer));
        I restoredArray=-1,restoredBuffer=-1;gl.getIntegerv(0x85B5,&restoredArray);gl.getIntegerv(0x8894,&restoredBuffer);
        const U restoreError=drawGl.getError();
        std::fprintf(stderr,"ra_fog_gpu_attestation_end old_vao=%d restored_vao=%d old_array_buffer=%d restored_array_buffer=%d restore_error=%u restored=%u gpu_winner_unknown=1\n",
            oldArray,restoredArray,oldBuffer,restoredBuffer,restoreError,unsigned(!restoreError&&oldArray==restoredArray&&oldBuffer==restoredBuffer));
    }

void VehicleWindowGl50DS::Storage::traceRaPixelProvenance() noexcept {
        if(raPixelProvenanceLogged)return;
        const char* enabled=std::getenv("RR_RA_PIXEL_PROVENANCE");
        if(!enabled||std::strcmp(enabled,"1"))return;
        raPixelProvenanceLogged=true;
        traceRaFogGpuAttestation();
        // Requested reference probe: GL pixel(50,28) in a640x448 capture.
        // Select the corresponding pixel center when the framebuffer differs.
        const I px=std::min(width-1,static_cast<I>(50.5*double(width)/640.));
        const I py=std::min(height-1,static_cast<I>(28.5*double(height)/448.));
        std::fprintf(stderr,"ra_pixel_provenance_begin cpu_candidates_only=1 capture_selected_frame=1 width=%d height=%d pixel=%d,%d camera_generation=%llu camera_publication=%llu camera_frame=%llu fog_revision=%llu draws=%zu\n",
            width,height,px,py,static_cast<unsigned long long>(raWorldCamera.generation),
            static_cast<unsigned long long>(raWorldCamera.publication),
            static_cast<unsigned long long>(raWorldCamera.frame),
            static_cast<unsigned long long>(raWorldCamera.fogRevision),raWorldFrameDraws.size());
        constexpr std::size_t limit=512;
        std::size_t triangles=0,candidates=0,logged=0,drawOrdinal=0;
        for(const auto& draw:raWorldFrameDraws){
            if(draw.localFlare){++drawOrdinal;continue;} //Model-only diagnostic, never relabel flare as surface0.
            const auto& plan=raWorld->surfaceDrawPlan_[draw.surface];
            const auto& raster=raWorld->rasterInventory_[plan.raster];
            const auto& material=raWorld->imageFamilies_[raster.family].materialMember;
            for(U first=plan.indexStart;first<plan.indexStart+plan.indexCount;first+=3){
                ++triangles;
                if(px<draw.scissor[0]||px>=draw.scissor[2]||py<draw.scissor[1]||py>=draw.scissor[3])continue;
                // Same matrix/index/vertex operands and conservative near-clip
                // fallback as the actual RA blended-triangle raster region.
                const auto region=raTriangleRasterRegion(first);
                if(px<region[0]||px>=region[2]||py<region[1]||py>=region[3])continue;
                ++candidates;if(logged==limit)continue;++logged;
                std::fprintf(stderr,"ra_pixel_candidate draw=%zu worldInstance=%zu lod=%u sourceSurface=%u surface=%zu triangle=%u material=%s materialRow=%d blend=%u aa=%u scissor=%d,%d,%d,%d conservative_region=%d,%d,%d,%d",
                    drawOrdinal,plan.worldInstance,plan.sourceLod,plan.sourceSurface,plan.surface,
                    (first-plan.indexStart)/3,material.c_str(),raster.materialRow,
                    unsigned(raster.fragmentDemands.orderedCurrentDestination),unsigned(raster.requiresNativeCoverage),
                    draw.scissor[0],draw.scissor[1],draw.scissor[2],draw.scissor[3],
                    region[0],region[1],region[2],region[3]);
                for(U corner=0;corner<3;++corner){
                    const auto index=raWorld->indices_[first+corner];
                    const auto& v=raWorldGeometry[index];const auto& current=raWorldFrame[index];
                    std::fprintf(stderr," v%u_world=%.9g,%.9g,%.9g v%u_uv=%.9g,%.9g v%u_rgba=%.9g,%.9g,%.9g,%.9g v%u_fog=%.9g",
                        corner,double(v.position[0]),double(v.position[1]),double(v.position[2]),
                        corner,double(v.uv[0]),double(v.uv[1]),corner,double(current.color[0]),double(current.color[1]),
                        double(current.color[2]),double(current.color[3]),corner,double(current.fog));
                    double clip[4]{};
                    for(U column=0;column<4;++column){
                        for(U axis=0;axis<3;++axis)clip[column]+=double(matrix[column*4+axis])*v.position[axis];
                        clip[column]+=matrix[column*4+3];
                    }
                    std::fprintf(stderr," v%u_clip=%.17g,%.17g,%.17g,%.17g",corner,clip[0],clip[1],clip[2],clip[3]);
                }
                std::fprintf(stderr,"\n");
            }
            ++drawOrdinal;
        }
        std::fprintf(stderr,"ra_pixel_provenance_end triangles=%zu conservative_candidates=%zu logged=%zu truncated=%zu gpu_winner_unknown=1\n",
            triangles,candidates,logged,candidates-logged);
    }

bool VehicleWindowGl50DS::Storage::drawRaWorld(U bank) noexcept {
        if(!raWorld)return true;
        bool complete=false;
        struct FlareDrawFailure {bool& failed;const std::vector<RaLocalFlareFrame>& frames;bool& complete;
            ~FlareDrawFailure(){if(!frames.empty()&&!complete)failed=true;}} flareFailure{raLocalFlareFailed,raLocalFlareFrames,complete};
        if(!raWorldFrameCurrent(*diagnostic)||!coverageLive.scene[bank]||!coverageLive.snapshot||!coverageLive.raster)
            return fail(*diagnostic,VehicleWindowError50DS::frame,"RA complete common target/coverage storage missing");
        // Cold admission proves each surface's indices stay in its vertex
        // range. Recompute one containing upload interval for this selection;
        // no prior frame, LOD, or successful-upload receipt is reused.
        std::size_t uploadFirst=raWorldFrame.size(),uploadEnd=0;
        for(const auto& selectedDraw:raWorldFrameDraws){
            if(selectedDraw.localFlare)continue;
            if(selectedDraw.surface>=raWorld->surfaceDrawPlan_.size())
                return fail(*diagnostic,VehicleWindowError50DS::assets,"RA selected upload surface exceeds retained plan");
            const auto& plan=raWorld->surfaceDrawPlan_[selectedDraw.surface];
            const std::size_t first=plan.vertexStart,count=plan.vertexCount;
            if(!count||first>raWorldFrame.size()||count>raWorldFrame.size()-first)
                return fail(*diagnostic,VehicleWindowError50DS::assets,"RA selected upload range exceeds current operands");
            uploadFirst=std::min(uploadFirst,first);uploadEnd=std::max(uploadEnd,first+count);
        }
        drawGl.bindVertexArray(raWorldArray);drawGl.bindBuffer(arrayBuffer,raWorldOperands);
        if(uploadFirst<uploadEnd)
            drawGl.bufferSubData(arrayBuffer,static_cast<std::intptr_t>(uploadFirst*sizeof(RaWorldFrameVertex)),
                static_cast<std::intptr_t>((uploadEnd-uploadFirst)*sizeof(RaWorldFrameVertex)),raWorldFrame.data()+uploadFirst);
        drawGl.bindBuffer(elementBuffer,raWorldIndices);
        gl.disable(0x0BE2);gl.disable(0x0B44);gl.disable(0x0C11);gl.disable(0x8DB9);gl.colorMask(1,1,1,1);
        bool accepted=check("upload same-camera RA packed-color and fog operands");
        for(const auto& selectedDraw:raWorldFrameDraws){
            if(!accepted)break;
            if(selectedDraw.localFlare){
                if(selectedDraw.flare==static_cast<std::size_t>(-1))continue;
                accepted=drawRaLocalFlare(bank,selectedDraw.flare,selectedDraw.scissor);
                // Native mixed ordering is retained. The following model
                // establishes its own program/VAO/texture and blend mode.
                continue;
            }
            const auto& plan=raWorld->surfaceDrawPlan_[selectedDraw.surface];
            const auto& raster=raWorld->rasterInventory_[plan.raster];
            const bool blend=raster.fragmentDemands.orderedCurrentDestination;
            const bool aa=raster.requiresNativeCoverage;
            const U image=raWorldFrameImages[plan.surface];
            // Opaque original triangles can share a draw. Blended triangles
            // observe each preceding primitive's current destination samples.
            const U step=blend?3:plan.indexCount;
            // This uninterrupted selected draw has no callbacks or other
            // writers of its private RA program uniforms. Establish them only
            // after clipping admits a primitive, and reuse only after success.
            bool firstPrimitive=true;
            for(U first=plan.indexStart;accepted&&first<plan.indexStart+plan.indexCount;first+=step){
                if(blend&&raTriangleOutsideNativeClip(first))continue;
                if(blend){
                    // Only this conservative raster rectangle can observe the
                    // next primitive. Snapshot, payload clear and composition
                    // share it; all other samples remain untouched.
                    auto region=raTriangleRasterRegion(first);
                    for(U axis=0;axis<2;++axis){region[axis]=std::max(region[axis],selectedDraw.scissor[axis]);
                        region[axis+2]=std::min(region[axis+2],selectedDraw.scissor[axis+2]);}
                    if(region[2]<=region[0]||region[3]<=region[1])continue;
                    gl.scissor(region[0],region[1],region[2]-region[0],region[3]-region[1]);gl.enable(0x0C11);
                    if(!snapshotRaWorld(bank,region)){accepted=false;break;}
                }else {
                    const auto& region=selectedDraw.scissor;
                    gl.scissor(region[0],region[1],region[2]-region[0],region[3]-region[1]);gl.enable(0x0C11);
                }
                gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);gl.bindTexture(texture2D,image);
                drawGl.bindVertexArray(raWorldArray);
                if(blend&&!aa){
                    // AA0 uses one center payload followed by per-sample
                    // composition, matching the established racer bridge.
                    gl.bindFramebuffer(framebuffer,coverageLive.raster);
                    // Private raster FBO retains both checked attachment selectors.
                    coverageAa0();gl.disable(0x0B71);gl.depthMask(0);
                    const float invalid[]={0,0,0,-1};gl.clearBufferfv(0x1800,0,invalid);
                    bindRaWorldMaterial(raster,0,true,firstPrimitive);
                    drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(first)*sizeof(U)),0);
                    gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);
                    gl.enable(0x809D);gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(1);
                    gl.useProgram(coverageProgram);drawGl.bindVertexArray(coverageArray);
                    gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,coverageLive.payload);
                    gl.activeTexture(textureUnit0+1);if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,coverageLive.rasterDepth);
                    gl.activeTexture(textureUnit0+2);if(gl.bindSampler)gl.bindSampler(2,0);gl.bindTexture(coverageTexture,coverageLive.snapshotColor);
                    // Later primitives retain mode1 from this selected draw's checked first bridge.
                    if(firstPrimitive)gl.uniform1i(coverageRaCompositionUniform,1);gl.drawArrays(0x0004,0,3);
                }else {
                    gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);
                    bindTrackCoverage(aa?NativeTrackCoverage50DS::fourSampleAa1:NativeTrackCoverage50DS::aa0);
                    gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(1);
                    bindRaWorldMaterial(raster,blend?1:0,false,firstPrimitive);
                    if(blend){gl.activeTexture(textureUnit0+2);if(gl.bindSampler)gl.bindSampler(2,0);gl.bindTexture(coverageTexture,coverageLive.snapshotColor);}
                    drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(step),0x1405,reinterpret_cast<const void*>(std::uintptr_t(first)*sizeof(U)),0);
                }
                gl.activeTexture(textureUnit0+2);gl.bindTexture(coverageTexture,0);
                gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);
                accepted=check("draw source-ordered RA material primitive with source depth/alpha rules");
                if(accepted)firstPrimitive=false;
            }
            if(accepted){++diagnostic->trackDraws;diagnostic->trackIndices+=plan.indexCount;}
        }
        gl.activeTexture(textureUnit0+2);gl.bindTexture(coverageTexture,0);
        gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,0);
        gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        gl.disable(0x0C11);
        resetPodState();const bool restored=check("restore common scene target after complete RA world");
        complete=accepted&&restored;return complete;
    }

bool VehicleWindowGl50DS::Storage::validateRcParticleBank(const VehicleWindowRequest50DS& input) noexcept {
    if(!input.track.rcParticleWindow)return true;
    const auto& window=*input.track.rcParticleWindow;
    if(input.track.gbParticleWindow||input.track.mcParticleWindow||input.gcParticleCpuTextures||input.gcParticleTextures||input.gcParticleMaterial||
       rcParticleWindow.get()!=&window||!window.ready()||window.renderer_!=rcParticleRenderer||
       !window.bank_.loaded()||window.bank_.textureCount()!=1||window.gpu_.textureCount()!=1||
       !window.gpu_.validBorrow(input.context,input.isCurrent,window.gpu_.publicationToken())||
       !window.gpu_.matchesSourceBank(window.bank_,window.gpu_.publicationToken()))
        return fail(*diagnostic,VehicleWindowError50DS::material,"RC particle bank requires its exact retained Window publication");
    const auto* source=window.bank_.texture(0);const auto* image=window.gpu_.asset(0);
    if(!source||!image||!image->name||image->identity!=source->identity||
       source->identity!=rc_particle_visual50RC::Window::imageIdentity||
       source->texture.width!=128||source->texture.height!=1024||source->texture.rgba.size()!=128u*1024u*4u)
        return fail(*diagnostic,VehicleWindowError50DS::material,"RC actual particle atlas identity differs");
    std::uint8_t maximum=0;
    for(std::size_t i=3;i<source->texture.rgba.size();i+=4)maximum=std::max(maximum,source->texture.rgba[i]);
    rcParticleMaximumAlpha=maximum;rcParticlePublication=window.gpu_.publicationToken();return true;
}

bool VehicleWindowGl50DS::Storage::validateGbParticleBank(const VehicleWindowRequest50DS& input) noexcept {
    if(!input.track.gbParticleWindow)return true;
    const auto& window=*input.track.gbParticleWindow;
    if(input.track.rcParticleWindow||input.track.mcParticleWindow||input.gcParticleCpuTextures||input.gcParticleTextures||input.gcParticleMaterial||
       gbParticleWindow.get()!=&window||!window.ready()||window.renderer_!=gbParticleRenderer||
       !window.bank_.loaded()||window.bank_.textureCount()!=1||window.gpu_.textureCount()!=1||
       !window.gpu_.validBorrow(input.context,input.isCurrent,window.gpu_.publicationToken())||
       !window.gpu_.matchesSourceBank(window.bank_,window.gpu_.publicationToken()))
        return fail(*diagnostic,VehicleWindowError50DS::material,"GB particle bank requires its exact retained Window publication");
    const auto* source=window.bank_.texture(0);const auto* image=window.gpu_.asset(0);
    if(!source||!image||!image->name||image->identity!=source->identity||
       source->identity!=gb_particle_visual50GB::Window::imageIdentity||
       source->texture.width!=128||source->texture.height!=1024||source->texture.rgba.size()!=128u*1024u*4u)
        return fail(*diagnostic,VehicleWindowError50DS::material,"GB actual particle atlas identity differs");
    std::uint8_t maximum=0;
    for(std::size_t i=3;i<source->texture.rgba.size();i+=4)maximum=std::max(maximum,source->texture.rgba[i]);
    gbParticleMaximumAlpha=maximum;gbParticlePublication=window.gpu_.publicationToken();return true;
}

bool VehicleWindowGl50DS::Storage::validateMcParticleBank(const VehicleWindowRequest50DS& input) noexcept {
    if(!input.track.mcParticleWindow)return true;
    const auto& window=*input.track.mcParticleWindow;
    if(input.track.rcParticleWindow||input.track.gbParticleWindow||input.gcParticleCpuTextures||input.gcParticleTextures||input.gcParticleMaterial||
       mcParticleWindow.get()!=&window||!window.ready()||window.renderer_!=mcParticleRenderer||
       !window.bank_.loaded()||window.bank_.textureCount()!=1||window.gpu_.textureCount()!=1||
       !window.gpu_.validBorrow(input.context,input.isCurrent,window.gpu_.publicationToken())||
       !window.gpu_.matchesSourceBank(window.bank_,window.gpu_.publicationToken()))
        return fail(*diagnostic,VehicleWindowError50DS::material,"MC particle bank requires its exact retained Window publication");
    const auto* source=window.bank_.texture(0);const auto* image=window.gpu_.asset(0);
    if(!source||!image||!image->name||image->identity!=source->identity||
       source->identity!=mc_particle_visual50MC::Window::imageIdentity||
       source->texture.width!=128||source->texture.height!=1024||source->texture.rgba.size()!=128u*1024u*4u)
        return fail(*diagnostic,VehicleWindowError50DS::material,"MC actual particle atlas identity differs");
    std::uint8_t maximum=0;
    for(std::size_t i=3;i<source->texture.rgba.size();i+=4)maximum=std::max(maximum,source->texture.rgba[i]);
    mcParticleMaximumAlpha=maximum;mcParticlePublication=window.gpu_.publicationToken();return true;
}

bool VehicleWindowGl50DS::Storage::validateGcParticleBank(const VehicleWindowRequest50DS& input) noexcept {
    const auto* cpu=input.gcParticleCpuTextures;const auto* gpu=input.gcParticleTextures;
    const auto* material=input.gcParticleMaterial;
    if(!cpu&&!gpu&&!material)return true;
    if(!cpu||!gpu||!material||!cpu->loaded()||cpu->textureCount()!=1||gpu->textureCount()!=1||
       !gpu->validBorrow(input.context,input.isCurrent,gpu->publicationToken())||
       material->identity!=&assets::originalGcParticlePsm50KG()||
       material->textureIdentity!=&assets::originalGcParticlePst50KG()||
       !material->templateIdentity||!material->fogEnabled||!material->alphaTestGreaterZero||
       !material->noDestinationAlphaConsumer)
        return fail(*diagnostic,VehicleWindowError50DS::material,"GC particle real PSM/PST/DefaultMT bank required");
    const auto* source=cpu->texture(0);const auto* image=gpu->asset(0);
    if(!source||!image||!image->name||image->identity!=source->identity||
       source->identity!=0x474344454d4f5052ull||source->texture.width!=128||source->texture.height!=1024||
       source->texture.rgba.size()!=128u*1024u*4u)
        return fail(*diagnostic,VehicleWindowError50DS::material,"GC particle authentic atlas publication differs");
    std::uint8_t maximum=0;
    for(std::size_t i=3;i<source->texture.rgba.size();i+=4)maximum=std::max(maximum,source->texture.rgba[i]);
    if(maximum!=material->maximumTextureAlpha)
        return fail(*diagnostic,VehicleWindowError50DS::material,"GC particle atlas alpha admission differs");
    for(unsigned state=0;state<2;++state){const auto& policy=material->states[state];
        if(policy.depthMethod!=2||policy.depthWrite||policy.environmentContext!=state||
           !policy.requiresOriginalDepthOwner||!policy.requiresSeparateAlphaStore||
           policy.blend!=(state?assets::OriginalUiMaterialBlend50KG::additive:assets::OriginalUiMaterialBlend50KG::sourceAlpha)||
           policy.minification!=assets::OriginalUiMaterialFilter50KG::linear||policy.magnification!=assets::OriginalUiMaterialFilter50KG::linear||
           policy.u!=assets::OriginalUiMaterialWrap50KG::repeat||policy.v!=assets::OriginalUiMaterialWrap50KG::repeat)
            return fail(*diagnostic,VehicleWindowError50DS::material,"GC particle material state unsupported");
    }
    gcParticleMaximumAlpha=maximum;gcParticlePublication=gpu->publicationToken();return true;
}

bool VehicleWindowGl50DS::Storage::validateParticleDraw(const game::OriginalGcParticleDraw50KG* draw,
    const VehicleWindowFrame50DS& frame,bool bankPresent,std::uint8_t maximumAlpha,
    VehicleWindowDiagnostic50DS& d) const noexcept {
    if(!bankPresent||!uiProgram||!draw->generation||!draw->cameraSceneKey||!draw->cameraSessionGeneration||
       draw->sourceParticleCount>game::originalGcParticleCapacity50KG||draw->quadCount>draw->sourceParticleCount||
       draw->runCount>draw->quadCount||!draw->vertices||!draw->fogFactors||!draw->runs||!frame.worldViewProjection)
        return fail(d,VehicleWindowError50DS::frame,"GC particle frame lacks actual manager/camera/bank publication");
    //Bind particles to the exact same current49F matrix as opaque gameplay.
    //50DX changes only packing and Z: this duplicate native arithmetic is a
    //preflight comparison, not a second camera or guessed view reconstruction.
    //Native widescreen scales the world matrix's clip-X column (50DX); the
    //source-projected particle quads take the same factor in drawGcParticles.
    const float widescreenX=nativeWidescreenScaleX50DX(frame.width,frame.height);
    for(U row=0;row<4;++row)for(U col=0;col<4;++col){
        float expected=draw->sourceCombined[row*4+col];
        if(col==2){volatile float twice=2.f*expected;volatile float z=twice-draw->sourceCombined[row*4+3];expected=z;}
        if(col==0&&widescreenX!=1.f){volatile float scaled=expected*widescreenX;expected=scaled;}
        if(!std::isfinite(expected)||expected!=frame.worldViewProjection[col*4+row])
            return fail(d,VehicleWindowError50DS::frame,"GC particle/world camera projection mismatch");
    }
    for(float v:draw->fogColor)if(!std::isfinite(v)||v<0||v>1)
        return fail(d,VehicleWindowError50DS::frame,"GC particle fog color invalid");
    U next=0;
    for(U r=0;r<draw->runCount;++r){const auto& run=draw->runs[r];
        if(run.firstQuad!=next||!run.quadCount||run.quadCount>12||run.quadCount>draw->quadCount-next||run.stateIndex>1)
            return fail(d,VehicleWindowError50DS::frame,"GC particle signed runs do not partition quads");
        for(U i=next*4;i<(next+run.quadCount)*4;++i){const auto& v=draw->vertices[i];
            for(float x:v.position)if(!std::isfinite(x))return fail(d,VehicleWindowError50DS::frame,"nonfinite GC particle position");
            for(float x:v.uv)if(!std::isfinite(x))return fail(d,VehicleWindowError50DS::frame,"nonfinite GC particle UV");
            if(v.position[2]<0||v.position[2]>16777215.f||std::floor(v.position[2])!=v.position[2]||
               !std::isfinite(draw->fogFactors[i])||draw->fogFactors[i]<0||draw->fogFactors[i]>254.f/256.f||
               std::floor(draw->fogFactors[i]*256.f)!=draw->fogFactors[i]*256.f)
                return fail(d,VehicleWindowError50DS::frame,"GC source packet depth/fog domain differs");
            if(!run.stateIndex&&U(v.color[3])*maximumAlpha>128u*128u)
                return fail(d,VehicleWindowError50DS::material,"GC source-alpha coefficient exceeds admitted native domain");
        }
        next+=run.quadCount;
    }
    return next==draw->quadCount||fail(d,VehicleWindowError50DS::frame,"GC particle run coverage differs");
}

bool VehicleWindowGl50DS::Storage::validateGcParticleFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) const noexcept {
    const auto* draw=frame.gcParticles;if(!draw)return true; //Resident bank while genuine UI owns presentation.
    return validateParticleDraw(draw,frame,request.gcParticleTextures!=nullptr,gcParticleMaximumAlpha,d);
}

bool VehicleWindowGl50DS::Storage::validateRcParticleFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) const noexcept {
    if(!rcParticleWindow&&!frame.rcParticles)return true;
    if(!rcParticleWindow||!frame.rcParticles||frame.gbParticles||frame.mcParticles||frame.gcParticles||!rcParticlePendingAttempt)
        return fail(d,VehicleWindowError50DS::frame,"RC particles require the actual scoped owner and current Window draw attempt");
    const auto* draw=frame.rcParticles->rendererDraw();const auto* material=frame.rcParticles->rendererMaterial();
    if(!draw||!material||!rcParticleWindow->acceptsMaterial(*material)||
       rcParticleWindow->materialBank(*material)!=&rcParticleWindow->bank_||
       rcParticleWindow->materialGpu(*material)!=&rcParticleWindow->gpu_||
       !rcParticleWindow->gpu_.validBorrow(request.context,request.isCurrent,rcParticlePublication)||
       !rcParticleWindow->gpu_.matchesSourceBank(rcParticleWindow->bank_,rcParticlePublication))
        return fail(d,VehicleWindowError50DS::material,"RC numeric packet escaped its actual canonical material/atlas borrow");
    return validateParticleDraw(draw,frame,true,rcParticleMaximumAlpha,d);
}

bool VehicleWindowGl50DS::Storage::validateGbParticleFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) const noexcept {
    if(!gbParticleWindow&&!frame.gbParticles)return true;
    if(!gbParticleWindow||!frame.gbParticles||frame.rcParticles||frame.mcParticles||frame.gcParticles||!gbParticlePendingAttempt)
        return fail(d,VehicleWindowError50DS::frame,"GB particles require the actual scoped owner and current Window draw attempt");
    const auto* draw=frame.gbParticles->rendererDraw();const auto* material=frame.gbParticles->rendererMaterial();
    if(!draw||!material||frame.gbParticles->windowOwner()!=gbParticleWindow.get()||!gbParticleWindow->acceptsMaterial(*material)||
       gbParticleWindow->materialBank(*material)!=&gbParticleWindow->bank_||
       gbParticleWindow->materialGpu(*material)!=&gbParticleWindow->gpu_||
       !gbParticleWindow->gpu_.validBorrow(request.context,request.isCurrent,gbParticlePublication)||
       !gbParticleWindow->gpu_.matchesSourceBank(gbParticleWindow->bank_,gbParticlePublication))
        return fail(d,VehicleWindowError50DS::material,"GB numeric packet escaped its actual canonical material/atlas borrow");
    // The very same source-bound fog and current camera drive opaque gameplay
    // and particles; equal caller colors alone do not create this authority.
    const auto& fog=frame.gbParticles->fog_->fields();const auto& opaque=request.track.worldFog;
    if(!opaque.enabled||opaque.start!=fog.start34||opaque.end!=fog.end38||
       opaque.color[0]!=31.f/255.f||opaque.color[1]!=28.f/255.f||opaque.color[2]!=55.f/255.f||!frame.worldFogCamera)
        return fail(d,VehicleWindowError50DS::frame,"GB particle and opaque source-bound fog differ");
    for(unsigned c=0;c<3;++c)if(frame.worldFogCamera[c]!=frame.gbParticles->storage_->cameraPosition[c])
        return fail(d,VehicleWindowError50DS::frame,"GB particle and opaque current camera differ");
    return validateParticleDraw(draw,frame,true,gbParticleMaximumAlpha,d);
}

bool VehicleWindowGl50DS::Storage::validateMcParticleFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) const noexcept {
    if(!mcParticleWindow&&!frame.mcParticles)return true;
    if(!mcParticleWindow||!frame.mcParticles||frame.rcParticles||frame.gbParticles||frame.gcParticles||!mcParticlePendingAttempt)
        return fail(d,VehicleWindowError50DS::frame,"MC particles require the actual scoped owner and current Window draw attempt");
    const auto* draw=frame.mcParticles->rendererDraw();const auto* material=frame.mcParticles->rendererMaterial();
    if(!draw||!material||frame.mcParticles->windowOwner()!=mcParticleWindow.get()||!mcParticleWindow->acceptsMaterial(*material)||
       mcParticleWindow->materialBank(*material)!=&mcParticleWindow->bank_||
       mcParticleWindow->materialGpu(*material)!=&mcParticleWindow->gpu_||
       !mcParticleWindow->gpu_.validBorrow(request.context,request.isCurrent,mcParticlePublication)||
       !mcParticleWindow->gpu_.matchesSourceBank(mcParticleWindow->bank_,mcParticlePublication))
        return fail(d,VehicleWindowError50DS::material,"MC numeric packet escaped its actual canonical material/atlas borrow");
    // The very same source-bound fog and current camera drive opaque gameplay
    // and particles; equal caller colors alone do not create this authority.
    const auto& fog=frame.mcParticles->fog_->fields();const auto& opaque=request.track.worldFog;
    if(!opaque.enabled||opaque.start!=fog.start34||opaque.end!=fog.end38||
       opaque.color[0]!=100.f/255.f||opaque.color[1]!=73.f/255.f||opaque.color[2]!=81.f/255.f||!frame.worldFogCamera)
        return fail(d,VehicleWindowError50DS::frame,"MC particle and opaque source-bound fog differ");
    for(unsigned c=0;c<3;++c)if(frame.worldFogCamera[c]!=frame.mcParticles->storage_->cameraPosition[c])
        return fail(d,VehicleWindowError50DS::frame,"MC particle and opaque current camera differ");
    return validateParticleDraw(draw,frame,true,mcParticleMaximumAlpha,d);
}

bool VehicleWindowGl50DS::Storage::drawGcParticles() noexcept {
    if(!frameGcParticles||!frameGcParticles->quadCount)return true;
    const auto& draw=*frameGcParticles;
    //Same pending gameplay color AND depth targets remain bound. No clear,
    //framebuffer copy or postprocess fog overlay. The independent source-A
    //store below samples the same authentic atlas after each RGB run.
    resetNativeGcLighting();gl.useProgram(uiProgram);
    gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
    gl.bindTexture(texture2D,frameMcParticles?mcParticleWindow->gpu_.texture(0):frameGbParticles?gbParticleWindow->gpu_.texture(0):(frameRcParticles?rcParticleWindow->gpu_.texture(0):request.gcParticleTextures->texture(0)));
    constexpr U parameters[]={0x2801,0x2800,0x2802,0x2803,0x813C,0x813D};
    constexpr I expected[]={0x2601,0x2601,0x2901,0x2901,0,0};
    for(U i=0;i<6;++i){I actual=-1;gl.getTexParameteriv(texture2D,parameters[i],&actual);
        if(!check("verify authentic GC particle sampler"))return false;
        if(actual!=expected[i])return fail(*diagnostic,VehicleWindowError50DS::material,"GC particle immutable sampler changed");}
    {   // Native widescreen: source-projected 640x448 particles follow the
        // same Hor+ factor as the 3D world, centred on the output.
        const float scaleX=nativeWidescreenScaleX50DX(width,height);
        gl.uniform4f(uiCanvasUniform,scaleX*2.f/640.f,-2.f/448.f,-scaleX,1.f);
    }
    gl.uniform1i(uiSourceCompositionUniform,0);gl.uniform1i(uiSourceRasterUniform,0);
    gl.uniform1i(uiTexturedUniform,1);gl.uniform1i(uiImageUniform,0);
    gl.uniform1i(uiSourceFogEnabledUniform,1);gl.uniform1i(uiSourceAlphaTestUniform,1);
    gl.uniform1f(uiAlphaUniform,(255.f/128.f)*(255.f/128.f));
    gl.uniform4f(uiParticleFogColorUniform,draw.fogColor[0],draw.fogColor[1],draw.fogColor[2],1);
    drawGl.bindVertexArray(uiVertexArray);drawGl.bindBuffer(arrayBuffer,uiVertexBuffer);
    drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(draw.quadCount)*4*sizeof(assets::Vertex48B),draw.vertices);
    drawGl.bindBuffer(arrayBuffer,uiParticleFogBuffer);
    drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(draw.quadCount)*4*sizeof(float),draw.fogFactors);
    drawGl.bindBuffer(elementBuffer,uiIndexBuffer);
    gl.disable(0x0B44);gl.disable(0x0C11);gl.enable(0x0B71);gl.depthMask(0);gl.depthFunc(0x0203);
    gl.colorMask(1,1,1,0);gl.enable(0x0BE2);gl.blendEquation(0x8006);
    bool accepted=check("upload actual shared GC particles");
    for(U r=0;accepted&&r<draw.runCount;++r){const auto& run=draw.runs[r];
        //Source reversed GEQUAL becomes native LEQUAL on the same camera Z.
        //Additive source coefficients can exceed1: pre-multiply in shader and
        //use ONE/ONE. Fog/dust/spray source-alpha remains SRC_ALPHA/ONE_MINUS.
        gl.colorMask(1,1,1,0);gl.enable(0x0BE2);
        gl.uniform1i(gcParticleModeUniform,run.stateIndex?2:1);
        gl.uniform1i(uiSourceBlendUniform,run.stateIndex?2:1);
        gl.blendFunc(run.stateIndex?1u:0x0302u,run.stateIndex?1u:0x0303u);
        drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(run.quadCount*6),0x1405,
            reinterpret_cast<const void*>(std::uintptr_t(run.firstQuad)*6*sizeof(U)),0);
        accepted=check("draw authentic GC particle RGB run");
        if(accepted){
            if(frameMcParticles)++diagnostic->mcParticleDraws;else if(frameGbParticles)++diagnostic->gbParticleDraws;else if(frameRcParticles)++diagnostic->rcParticleDraws;else ++diagnostic->gcParticleDraws;
            //Actual separate source-A store, not source-over accumulation.
            //Both authenticated states use DATE0 and source (never destination)
            //alpha in ALPHA, so deferred alpha within this run cannot change
            //any RGB fragment. Preserve primitive order, depth and alpha test.
            gl.disable(0x0BE2);gl.colorMask(0,0,0,1);gl.uniform1i(gcParticleModeUniform,3);
            drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(run.quadCount*6),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(run.firstQuad)*6*sizeof(U)),0);
            accepted=check("store authentic GC particle source alpha");
            if(accepted){if(frameMcParticles){++diagnostic->mcParticleAlphaDraws;diagnostic->mcParticleQuads+=run.quadCount;}
                else if(frameGbParticles){++diagnostic->gbParticleAlphaDraws;diagnostic->gbParticleQuads+=run.quadCount;}
                else if(frameRcParticles){++diagnostic->rcParticleAlphaDraws;diagnostic->rcParticleQuads+=run.quadCount;}
                else{++diagnostic->gcParticleAlphaDraws;diagnostic->gcParticleQuads+=run.quadCount;}}
        }
    }
    gl.uniform1i(gcParticleModeUniform,0);gl.disable(0x0BE2);gl.colorMask(1,1,1,1);
    gl.depthMask(1);gl.depthFunc(0x0201);drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after shared GC particles");return accepted&&restored;
}

bool VehicleWindowGl50DS::Storage::createRcCrystalProjection(const VehicleWindowTrack50DS& track)noexcept {
        if(!track.rcCrystal||!coverageDemand)return fail(*diagnostic,VehicleWindowError50DS::request,
            "environment-map projection requires the coverage targets of its destination-alpha pair");
        rcCrystalUvs.reset(new(std::nothrow) std::array<float,2>[rc_crystal50RC::Owner::uvCount]);
        if(!rcCrystalUvs)return fail(*diagnostic,VehicleWindowError50DS::allocation,"RC compact UV staging allocation");
        const auto bytes=track.mesh->vertices.size()*sizeof(std::array<float,2>);
        gl.genBuffers(1,&trackProjectionBuffer);drawGl.bindBuffer(arrayBuffer,trackProjectionBuffer);
        gl.bufferData(arrayBuffer,static_cast<std::intptr_t>(bytes),nullptr,dynamicDraw);
        if(!check("allocate exact RC projection stream")||!trackProjectionBuffer||!verifyBuffer(arrayBuffer,bytes,dynamicDraw))return false;
        drawGl.bindVertexArray(trackVertexArray);gl.enableVertexAttribArray(4);
        gl.vertexAttribPointer(4,2,floatType,0,sizeof(std::array<float,2>),nullptr);
        constexpr U properties[]={0x8622,0x8623,0x8624,0x8625,0x886A,0x889F,0x88FD};
        const I expected[]={1,2,sizeof(std::array<float,2>),static_cast<I>(floatType),0,static_cast<I>(trackProjectionBuffer),0};
        for(U p=0;p<7;++p){I actual=0;gl.getVertexAttribiv(4,properties[p],&actual);
            if(!check("verify RC projection layout")||actual!=expected[p])return fail(*diagnostic,VehicleWindowError50DS::layout,"RC projection attribute differs");}
        void* offset=nullptr;gl.getVertexAttribPointerv(4,0x8645,&offset);
        if(!check("verify RC projection offset"))return false;
        return !offset||fail(*diagnostic,VehicleWindowError50DS::layout,"RC projection offset differs");
    }

bool VehicleWindowGl50DS::Storage::prepareRcCrystalFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d)noexcept {
        environmentMapPrepared=false;
        if(!request.track.rcCrystal)return true;
        if(!coverageDemand||!rcCrystalUvs||!request.track.rcCrystal->ready()||
           !frame.sourceBodyCamera50LA||!frame.sourceBodyCameraOwner50LA||!frame.sourceBodyCameraPublication50LA)
            return fail(d,VehicleWindowError50DS::frame,"RC crystal needs actual retained source and current camera");
        NativeCameraViewContract50DX contract{};contract.provenance=NativeCameraViewProvenance50DX::currentCore49FForOutput;
        contract.width=frame.width;contract.height=frame.height;contract.clipDepth=NativeCameraClipDepth50DX::negativeOneToOne;
        NativeCameraMatrix50DX converted{};NativeCameraViewError50DX error{};
        const game::CameraViewOutput49F camera=*frame.sourceBodyCamera50LA;
        if(!convertNativeCameraView50DX(camera,contract,converted,error)||
           std::memcmp(converted.worldViewProjection,frame.worldViewProjection,sizeof(converted.worldViewProjection))||
           !request.track.rcCrystal->project(camera,rcCrystalUvs.get(),rc_crystal50RC::Owner::uvCount,environmentAmbient))
            return fail(d,VehicleWindowError50DS::frame,"RC crystal camera/ambient/projection preflight rejected");
        environmentMapPrepared=true;return true;
    }

bool VehicleWindowGl50DS::Storage::uploadRcCrystalUvs()noexcept {
        if(!request.track.rcCrystal)return true;
        if(!environmentMapPrepared||!trackProjectionBuffer)return fail(*diagnostic,VehicleWindowError50DS::frame,"RC projection lacks complete preflight");
        drawGl.bindBuffer(arrayBuffer,trackProjectionBuffer);std::size_t offset=0;
        for(const auto& range:request.track.rcCrystal->ranges()){
            drawGl.bufferSubData(arrayBuffer,std::intptr_t(range.vertexStart)*sizeof(std::array<float,2>),
                std::intptr_t(range.vertexCount)*sizeof(std::array<float,2>),rcCrystalUvs.get()+offset);
            if(!check("upload authenticated RC crystal range"))return false;offset+=range.vertexCount;
        }
        return offset==rc_crystal50RC::Owner::uvCount;
    }

std::array<I,4> VehicleWindowGl50DS::Storage::gsTriangleRegion(U first)const noexcept {
        // The SAME uploaded track mesh (validation requires the environment-map
        // owner's mesh to be this mesh), so no course owner is consulted here.
        const auto* mesh=request.track.mesh;
        if(!mesh||first>mesh->indices.size()||mesh->indices.size()-first<3)return {{0,0,width,height}};
        std::array<std::array<float,3>,3> triangle{};
        for(U c=0;c<3;++c){const U index=mesh->indices[first+c];if(index>=mesh->vertices.size())return {{0,0,width,height}};
            std::copy_n(mesh->vertices[index].position,3,triangle[c].begin());}
        return nativeTriangleRasterRegion(triangle);
    }

bool VehicleWindowGl50DS::Storage::drawDestinationAlphaTriangle(U bank,U first,U pass,U image,const std::array<I,4>& region,U triangleCount,bool environment)noexcept {
        diagnostic->rcCrystalTriangles+=triangleCount;
        if(region[2]<=region[0]||region[3]<=region[1]){diagnostic->rcCrystalEmptyTriangles+=triangleCount;return true;}
        if(region==std::array<I,4>{{0,0,width,height}})++diagnostic->rcCrystalFullRectangles;
        if(!coverageDemand||!coverageLive.raster||!coverageLive.snapshot||(environment&&!environmentMapPrepared)||pass<1||pass>2)
            return fail(*diagnostic,VehicleWindowError50DS::frame,"GS destination-alpha exact bridge resources unavailable");
        coverageAa0();gl.disable(0x8DB9);gl.disable(0x0BE2);gl.colorMask(1,1,1,1);
        gl.viewport(0,0,width,height);gl.scissor(region[0],region[1],region[2]-region[0],region[3]-region[1]);gl.enable(0x0C11);
        gl.bindFramebuffer(framebuffer,coverageLive.raster);gl.disable(0x0B71);gl.depthMask(0);
        const float invalid[]={0.f,0.f,0.f,-1.f};gl.clearBufferfv(0x1800,0,invalid);
        gl.useProgram(trackProgram);gl.uniform1i(gsAlphaPayloadUniform,1);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,image);drawGl.bindVertexArray(trackVertexArray);
        bool accepted=check("bind RC AA0 source byte/depth payload");
        if(accepted){drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(triangleCount*3),0x1405,reinterpret_cast<const void*>(std::uintptr_t(first)*sizeof(U)),0);
            accepted=check("raster ordered GS destination-alpha triangle");}
        if(accepted){
            if(gl.copyImageSubData)gl.copyImageSubData(coverageLive.color[bank],coverageTexture,0,
                region[0],region[1],0,coverageLive.snapshotColor,coverageTexture,0,region[0],region[1],0,
                region[2]-region[0],region[3]-region[1],1);
            else {gl.bindFramebuffer(readFramebuffer,coverageLive.scene[bank]);gl.bindFramebuffer(drawFramebuffer,coverageLive.snapshot);
                gl.blitFramebuffer(region[0],region[1],region[2],region[3],region[0],region[1],region[2],region[3],colorBufferBit,0x2600);}
            accepted=check("copy actual RC per-sample destination bytes");
        }
        if(accepted){
            gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);gl.enable(0x809D);gl.enable(0x0B71);
            gl.depthFunc(0x0203);gl.depthMask(0);gl.useProgram(coverageProgram);
            gl.uniform1i(coverageRaCompositionUniform,pass==1?2:0);drawGl.bindVertexArray(coverageArray);
            if(gl.bindSampler)gl.bindSampler(0,0);gl.bindTexture(texture2D,coverageLive.payload);
            gl.activeTexture(textureUnit0+1);if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,coverageLive.rasterDepth);
            gl.activeTexture(textureUnit0+2);if(gl.bindSampler)gl.bindSampler(2,0);gl.bindTexture(coverageTexture,coverageLive.snapshotColor);
            accepted=check("bind exact RC alpha equation and per-sample destination");
            if(accepted){++diagnostic->rcCrystalCompositionsIssued;gl.drawArrays(0x0004,0,3);
                accepted=check("compose RC crystal RGB and independent source alpha");}
        }
        // Unconditional checked restoration even after partial driver failure.
        gl.activeTexture(textureUnit0+2);gl.bindTexture(coverageTexture,0);
        gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,image);
        gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);gl.useProgram(trackProgram);
        gl.uniform1i(gsAlphaPayloadUniform,0);drawGl.bindVertexArray(trackVertexArray);
        coverageAa0();gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(0);gl.disable(0x0BE2);gl.disable(0x0C11);
        const bool restored=check("restore track state after GS destination-alpha composition");return accepted&&restored;
    }

// Admission of the fixed GPU-blend route for one span this frame. Pass 1
// (C=As) is byte-exact for any source colour: the shader floors Cs*As/128
// and ONE,ONE adds with saturation, as COLCLAMP=1 does. Pass 2 (C=Ad) hands
// Cs/128 to the DST_ALPHA factor, which a UNORM8 colour output clamps at 1,
// so it is admitted only while Cs cannot exceed 128: with the texel taken as
// 255 (no per-texture maximum is retained), the vertex byte must be <= 64
// (ambient row for the environment map, load-time span maximum for prelit),
// and with fog on the fog colour byte <= 128. Otherwise the exact bridge runs.
bool VehicleWindowGl50DS::Storage::fixedDestinationAlphaAdmitted(const TrackSpan& span,U pass)const noexcept {
        static const bool forceExact=[]{const char* e=std::getenv("RR_EXACT_DEST_ALPHA_BLEND");
            return e&&e[0]=='1'&&!e[1];}();
        if(forceExact||!gl.blendFuncSeparate||pass<1||pass>2)return false;
        if(span.sourceEnvironmentMap&&!environmentMapPrepared)return false;
        if(pass==1)return true;
        const auto index=static_cast<std::size_t>(&span-trackSpans.get());
        for(U c=0;c<3;++c){
            const U vertexByte=span.sourceEnvironmentMap?environmentAmbient[c]:
                (gsSpanMaxVertexByte&&index<trackSpanCount?gsSpanMaxVertexByte[index]:255u);
            if((255u*vertexByte)/128u>128u)return false; // pass-2 byte before fog
            if(request.track.worldFog.enabled&&span.worldFogEligible){
                const float f=request.track.worldFog.color[c];
                if(!std::isfinite(f)||f*255.f>128.f)return false;
            }
        }
        return true;
    }

bool VehicleWindowGl50DS::Storage::drawDestinationAlphaRange(U bank,U first,U count,U pass,U image,const TrackSpan& span)noexcept {
        if(count%3)return fail(*diagnostic,VehicleWindowError50DS::indices,"GS destination-alpha triangle extent");
        // Fixed-function route for the GS A=Cs,B=0,C=As/Ad,D=Cd pair (any
        // course, chosen by the material's own ALPHA fields). The GPU blends
        // overlapping fragments in primitive order, per sample, so no scratch
        // clear, copy, composition or disjoint-run split is needed. Same
        // coverage rule as the bridge below: MULTISAMPLE disabled gives
        // pixel-center (AA0) rasterization written to every covered sample,
        // depth LEQUAL. Pass 1 is byte-exact; pass 2 rounds where the GS
        // floors (at most one level): the same deviation as the pod map-2
        // fixed route. RR_EXACT_DEST_ALPHA_BLEND=1 forces the exact bridge.
        if(fixedDestinationAlphaAdmitted(span,pass)){
            diagnostic->rcCrystalTriangles+=count/3;
            if(coverageDemand){
                if(!coverageLive.scene[bank])
                    return fail(*diagnostic,VehicleWindowError50DS::frame,"GS destination-alpha scene target unavailable");
                gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);
                coverageAa0();
            } // Without coverage demand the bound gameplay target is already AA0.
            gl.disable(0x8DB9);gl.colorMask(1,1,1,1);
            gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(span.material.policy.depthWrite?1:0);
            gl.enable(0x0BE2);gl.blendEquation(0x8006);           // FUNC_ADD
            if(pass==1)gl.blendFuncSeparate(1,1,1,0);             // RGB: ONE,ONE;       A: ONE,ZERO
            else gl.blendFuncSeparate(0x0304,1,1,0);              // RGB: DST_ALPHA,ONE; A: ONE,ZERO
            gl.uniform1i(gsAlphaPayloadUniform,2);
            bool accepted=check("bind GS destination-alpha fixed-function blend");
            if(accepted){
                ++diagnostic->rcCrystalCompositionsIssued; // counts fixed draws on this route
                drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(count),0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(first)*sizeof(U)),0);
                accepted=check("draw GS destination-alpha range through fixed-function blend");
            }
            // Same exit state as the bridge restoration.
            gl.uniform1i(gsAlphaPayloadUniform,0);gl.disable(0x0BE2);
            if(coverageDemand)coverageAa0();
            gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(0);gl.disable(0x0C11);
            const bool restored=check("restore track state after fixed destination-alpha draw");
            return accepted&&restored;
        }
        // Same rule as the pod pair bridge: a run keeps the contiguous index
        // order and holds only triangles whose conservative pixel rectangles are
        // pairwise disjoint, so no member can read another's destination or
        // depth. One scratch clear, raster, copy and composition per run; each
        // pixel's result is the one the per-triangle bridge produces.
        constexpr U maximumRun=64;
        const U end=first+count;
        const auto area=[](const std::array<I,4>& r) noexcept -> std::uint64_t {
            return std::uint64_t(r[2]-r[0])*std::uint64_t(r[3]-r[1]);};
        const auto full=[&](const std::array<I,4>& r) noexcept {return r==std::array<I,4>{{0,0,width,height}};};
        const auto empty=[](const std::array<I,4>& r) noexcept {return r[2]<=r[0]||r[3]<=r[1];};
        while(first<end){
            const auto initial=gsTriangleRegion(first);
            if(empty(initial)||full(initial)){if(!drawDestinationAlphaTriangle(bank,first,pass,image,initial,1,span.sourceEnvironmentMap))return false;first+=3;continue;}
            std::array<std::array<I,4>,maximumRun> regions;regions[0]=initial;auto united=initial;
            std::uint64_t summed=area(initial);U run=1;
            while(run<maximumRun&&first+run*3<end){
                const auto next=gsTriangleRegion(first+run*3);
                if(empty(next)||full(next))break;
                bool disjoint=true;
                for(U i=0;i<run&&disjoint;++i){const auto& old=regions[i];
                    disjoint=!(next[0]<old[2]&&old[0]<next[2]&&next[1]<old[3]&&old[1]<next[3]);}
                if(!disjoint)break;
                auto candidate=united;
                for(U axis=0;axis<2;++axis){candidate[axis]=std::min(candidate[axis],next[axis]);
                    candidate[axis+2]=std::max(candidate[axis+2],next[axis+2]);}
                // Do not let a sparse run grow the scratch/copy/composition area.
                if(area(candidate)>(summed+area(next))*2u)break;
                regions[run++]=next;united=candidate;summed+=area(next);
            }
            if(!drawDestinationAlphaTriangle(bank,first,pass,image,united,run,span.sourceEnvironmentMap))return false;
            first+=run*3;
        }
        return true;
    }

} // namespace rr::platform
