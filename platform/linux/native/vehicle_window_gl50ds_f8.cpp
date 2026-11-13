#include "vehicle_window_gl50ds_storage.h"
#include "../../../tests/native/original_race_f8_native_bars50la.h"
#include "../../../src/game/original_results104_storage50la.h"
#include "../../../src/assets/original_standard_material_domain50la.h"
#include <array>
#include <cstdio>
#include <cstdlib>

namespace rr::platform {
bool VehicleWindowGl50DS::retireNativeResultsResources50LA(VehicleWindowDiagnostic50DS& d)noexcept {
    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"Results retirement renderer absent");
    auto& s=*storage_;
    if(s.thread!=std::this_thread::get_id())return fail(d,VehicleWindowError50DS::wrongThread,"Results retirement thread");
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options owns native mutation");
    if(s.active||s.frameSet||s.diagnostic||s.nativeResultsDrawPreparing||
       s.frameNativeF8Bars||s.nativeF8BarFrame||s.nativeF8BarRenderer)
        return fail(d,VehicleWindowError50DS::frame,"Results retirement needs idle actual renderer");
    if(!s.current(d)||!finishInitializedContextWork(d)||!d.contextWorkCompleted)return false;
    // An unsuccessful delete retains every unresolved name for retry.
    return s.deleteNativeResultsResources(d);
}

bool VehicleWindowGl50DS::Storage::hasNativeResultsResources()const noexcept {
    if(nativeResultsTarget||nativeResultsColor||nativeResultsDepth)return true;
    for(const auto& variant:nativeResultsVariants)if(variant&&variant->textures.ownsResources())return true;
    return false;
}

bool VehicleWindowGl50DS::Storage::deleteNativeResultsResources(VehicleWindowDiagnostic50DS& d)noexcept {
    // Caller has completed this actual context's work, as for the other native
    // resource owners. Never drop an entry whose deletion is unacknowledged.
    nativeResultsDrawResourcesReady=false;nativeResultsPackets.clear();
    for(auto& variant:nativeResultsVariants){
        MaterialTextureGlDiagnostic50EB td;
        if(!variant->textures.close(td)){
            d.cleanupIncomplete=true;
            if(!d.cleanupGlError)d.cleanupGlError=td.glError?td.glError:td.cleanupGlError;
            return false;
        }
        variant->publication=0;
    }
    nativeResultsVariants.clear();
    if(!deleteName(nativeResultsTarget,gl.deleteFramebuffers,d)||
       !deleteName(nativeResultsColor,gl.deleteTextures,d)||
       !deleteName(nativeResultsDepth,gl.deleteRenderbuffers,d))return false;
    nativeResultsPreparationFailed=false;return true;
}

bool VehicleWindowGl50DS::Storage::createNativeResultsDrawResources()noexcept {
    if(!diagnostic||!nativeResultsDrawPreparing||nativeResultsTarget||nativeResultsColor||nativeResultsDepth||nativeResultsDrawResourcesReady)return false;
    if(!load(gl.scissor,"glScissor")||!load(gl.blitFramebuffer,"glBlitFramebuffer")||
       !load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
       !load(gl.activeTexture,"glActiveTexture")||!load(gl.bindTexture,"glBindTexture")||
       !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
       !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||!load(gl.framebufferTexture2D,"glFramebufferTexture2D"))return false;
    if(!uiProgram&&!createOriginalUi())return false;
    if(!uiProgram||!uiVertexArray||!uiVertexBuffer||!uiIndexBuffer)
        return fail(*diagnostic,VehicleWindowError50DS::shader,"Results shared native UI program incomplete");
    if(!uiSourceCoherentFramebufferFetch){
        I major=0,minor=0,units=0,count=0;gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);
        gl.getIntegerv(0x8872,&units);bool barrier=major>4||(major==4&&minor>=5);
        if(!load(gl.getStringi,"glGetStringi")||!check("query Results native source-alpha capabilities"))return false;
        gl.getIntegerv(0x821D,&count);
        if(!check("query Results native extensions")||count<0||count>65536||units<2)return false;
        for(I i=0;i<count;++i){const auto* extension=gl.getStringi(0x1F03,U(i));
            if(!check("read Results native extension")||!extension)return false;
            if(!std::strcmp(reinterpret_cast<const char*>(extension),"GL_ARB_texture_barrier"))barrier=true;}
        if(!barrier||!load(gl.textureBarrier,"glTextureBarrier"))
            return fail(*diagnostic,VehicleWindowError50DS::limits,"Results native ordered source-alpha feedback unavailable");
    }
    I limit=0;gl.getIntegerv(0x0D33,&limit);
    if(!check("query Results native scratch extent")||limit<maximumWidth||limit<maximumHeight)return false;
    gl.genFramebuffers(1,&nativeResultsTarget);
    if(!check("create Results native scratch framebuffer")||!nativeResultsTarget)return false;
    gl.bindFramebuffer(framebuffer,nativeResultsTarget);
    gl.genTextures(1,&nativeResultsColor);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,nativeResultsColor);
    gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
    gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
    gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);drawGl.bindBuffer(0x88EC,0);
    gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
    if(!check("allocate Results native RGBA8 scratch")||!nativeResultsColor)return false;
    I w=0,h=0,format=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
    gl.getTexLevelParameteriv(texture2D,0,0x1001,&h);gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
    if(!check("verify Results native RGBA8 scratch")||w!=maximumWidth||h!=maximumHeight||format!=I(rgba8))return false;
    gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,nativeResultsColor,0);
    if(!createAttachment(nativeResultsDepth,depth24,depthAttachment))return false;
    gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    const U status=gl.checkFramebufferStatus(framebuffer);
    if(!check("complete Results native scratch framebuffer")||status!=framebufferComplete){
        diagnostic->framebufferStatus=status;return false;}
    return true;
}

bool VehicleWindowGl50DS::prepareNativeResultsDrawResources50LA(VehicleWindowDiagnostic50DS& d)noexcept {
    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"Results renderer unavailable");
    auto& s=*storage_;
    if(s.thread!=std::this_thread::get_id())return fail(d,VehicleWindowError50DS::wrongThread,"Results scratch creator thread");
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options owns native mutation");
    if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic||s.frameNativeF8Bars||s.nativeF8BarFrame||
       s.nativeF8BarRenderer||s.nativeResultsDrawPreparing||s.nativeResultsPreparationFailed||s.nativeF8BarPreparationFailed||
       s.raceHudUiCreationFailed||s.timeStatsUiCreationFailed||s.positionStatsUiCreationFailed||
       s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::frame,"Results scratch requires idle nonfailed actual renderer");
    if(s.nativeResultsDrawResourcesReady)return s.nativeResultsTarget&&s.nativeResultsColor&&s.nativeResultsDepth&&
        s.uiProgram&&s.uiVertexArray&&s.uiVertexBuffer&&s.uiIndexBuffer;
    s.diagnostic=&d;s.nativeResultsPreparationFailed=true;s.nativeResultsDrawPreparing=true;
    const bool created=s.check("before Results native scratch allocation")&&s.createNativeResultsDrawResources();
    s.nativeResultsDrawPreparing=false; //Scoped across the nonthrowing allocation only, including failure.
    // Existing idle native backend contract restores neutral bindings, not a
    // source framebuffer publication. Every partial name stays cleanup-owned.
    if(s.gl.bindTexture)s.gl.bindTexture(texture2D,0);
    s.gl.bindFramebuffer(framebuffer,0);s.gl.bindRenderbuffer(renderbuffer,0);
    s.drawGl.bindVertexArray(0);s.drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=s.check("restore Results native scratch allocation bindings");s.diagnostic=nullptr;
    if(created&&restored){s.nativeResultsDrawResourcesReady=true;s.nativeResultsPreparationFailed=false;return true;}
    d.cleanupIncomplete=s.hasGlResources();return false;
}

bool VehicleWindowGl50DS::Storage::drawNativeResultsPackets(U bank)noexcept {
    if(!frameNativeResults)return true; //Unselected native route is unchanged.
    if(!nativeResultsFrame||!nativeResultsRenderer||
       !frameNativeResults->current(*nativeResultsRenderer,*nativeResultsFrame))return false;
    // Only the actual scoped accepted Frame can reach this lowering leaf.
    // Its source capture and native projection do not certify PS2 mode5.
    if(!diagnostic||thread!=std::this_thread::get_id()||!active||!frameSet||bank>1||
       !request.isCurrent||!request.isCurrent(request.context)||!nativeResultsDrawResourcesReady||
       nativeResultsPreparationFailed||!nativeResultsTarget||!nativeResultsColor||!nativeResultsDepth||
       !targets[bank]||width<=0||height<=0||width>maximumWidth||height>maximumHeight||
       !uiProgram||!uiVertexArray||!uiVertexBuffer||!uiIndexBuffer||!shader||
       (!uiSourceCoherentFramebufferFetch&&!gl.textureBarrier))return false;
    if(nativeResultsPackets.empty())return true; // No native draw; not source callback success.
    std::size_t quads=0;
    // Validate the entire private batch before touching the pending image.
    for(const auto& packet:nativeResultsPackets){
        if(packet.vertices.empty()||packet.vertices.size()%4||packet.vertices.size()/4>originalUiMaximumQuads50KG-quads)return false;
        quads+=packet.vertices.size()/4;
        const auto depth=packet.hasTexture?packet.textured.depthMethod:packet.untextured.depthMethod;
        const auto blend=packet.hasTexture?packet.textured.blend:packet.untextured.blend;
        const auto context=packet.hasTexture?packet.textured.environmentContext:packet.untextured.environmentContext;
        const auto alpha=packet.hasTexture?packet.textured.maximumVertexAlpha:packet.untextured.maximumVertexAlpha;
        if(depth>3||context||blend==assets::OriginalUiMaterialBlend50KG::destinationAlphaAdditive)return false;
        // The existing byte-composition shader implements these exact scales;
        // a later policy extension cannot silently borrow this native path.
        if(packet.hasTexture){const auto& p=packet.textured;
            if(p.colorScale!=255.f/128.f||p.storedAlphaScale!=255.f/128.f||
               p.blendAlphaScale!=(255.f/128.f)*(255.f/128.f)||p.alphaStoreFromBlendFactor!=128.f/255.f||
               !p.clampModulatedColorBeforeBlend||
               p.requiresSeparateAlphaStore!=(p.blend!=assets::OriginalUiMaterialBlend50KG::opaque)||
               p.requiresOriginalDepthOwner!=(p.depthMethod!=1||p.depthWrite))return false;
        }else{const auto& p=packet.untextured;
            if(p.colorScale!=1.f||p.storedAlphaScale!=1.f||p.blendAlphaScale!=255.f/128.f||
               p.alphaStoreFromBlendFactor!=128.f/255.f||
               p.requiresSeparateAlphaStore!=(p.blend!=assets::OriginalUiMaterialBlend50KG::opaque)||
               p.requiresOriginalDepthOwner!=(p.depthMethod!=1||p.depthWrite))return false;
        }
        for(const auto& v:packet.vertices){
            for(float f:v.position)if(!std::isfinite(f))return false;
            for(float f:v.uv)if(!std::isfinite(f))return false;
            if(v.position[2]!=16777214.f||v.color[3]>alpha)return false;
        }
        if(packet.hasTexture){
            if(packet.variant>=nativeResultsVariants.size())return false;const auto& variant=*nativeResultsVariants[packet.variant];
            if(!variant.publication||!variant.textures.validBorrow(request.context,request.isCurrent,variant.publication)||
               !variant.textures.matchesSourceBank(variant.pixels,variant.publication)||!variant.textures.texture(0))return false;
        }
    }
    gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);gl.disable(0x0B44);
    gl.polygonMode(0x0408,0x1B02);gl.colorMask(1,1,1,1);
    gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
    gl.bindFramebuffer(drawFramebuffer,nativeResultsTarget);gl.drawBuffer(colorAttachment);
    gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
    gl.bindFramebuffer(framebuffer,nativeResultsTarget);gl.viewport(0,0,width,height);
    gl.depthMask(1);gl.clearDepth(0);gl.clear(depthBufferBit);gl.enable(0x0B71);gl.depthRange(0,1);
    resetNativeGcLighting();gl.useProgram(uiProgram);
    const double scale=std::min(double(width)/640.0,double(height)/448.0);
    const double ox=(width-640.0*scale)*.5,oy=(height-448.0*scale)*.5;
    gl.uniform4f(uiCanvasUniform,float(2*scale/width),float(-2*scale/height),float(2*ox/width-1),float(1-2*oy/height));
    const I canvasX=I(std::floor(ox)),canvasY=I(std::floor(oy)),canvasW=I(std::ceil(ox+640*scale))-canvasX,
        canvasH=I(std::ceil(oy+448*scale))-canvasY;
    gl.scissor(canvasX,canvasY,canvasW,canvasH);gl.enable(0x0C11);
    static const bool originalAspect=[]{const char* v=std::getenv("RR_ORIGINAL_ASPECT");return v&&v[0]=='1'&&!v[1];}();
    static I loggedWidth=0,loggedHeight=0;
    bool loggedBars=loggedWidth==width&&loggedHeight==height;
    gl.uniform1i(gcParticleModeUniform,0);gl.uniform1i(uiSourceCompositionUniform,1);
    gl.uniform1i(uiSourceRasterUniform,0);gl.uniform1i(uiSourceFogEnabledUniform,0);
    gl.uniform1i(uiSourceAlphaTestUniform,0);gl.uniform1i(uiCoveragePayloadUniform,0);
    gl.uniform1f(uiAlphaUniform,1);gl.uniform1i(uiImageUniform,0);
    if(!uiSourceCoherentFramebufferFetch){gl.uniform1i(uiSourceTargetUniform,1);gl.activeTexture(textureUnit0+1);
        if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,nativeResultsColor);}
    gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
    drawGl.bindVertexArray(uiVertexArray);drawGl.bindBuffer(arrayBuffer,uiVertexBuffer);drawGl.bindBuffer(elementBuffer,uiIndexBuffer);
    bool okay=check("prepare Results native ordered packet lowering");
    {   // Diagnostic receipt (fault 3, visual-faults NOTES): one line per packet
        // whenever the packet list's shape changes, so a muted logs-only run
        // shows whether the bottom-bar prompt icons/titles (canvas y 382..405)
        // are lowered at all, and whether they precede the opaque bar
        // (untextured, y 383..448) that the loop below draws in list order.
        // Read-only: no GL call, no state change, bounded by the packet count.
        static std::size_t loggedCount=0;static float loggedSignature=-1.f;
        float signature=0.f;
        for(const auto& packet:nativeResultsPackets)
            for(const auto& v:packet.vertices)signature+=v.position[1]+(packet.hasTexture?0.5f:0.f);
        if(nativeResultsPackets.size()!=loggedCount||signature!=loggedSignature){
            loggedCount=nativeResultsPackets.size();loggedSignature=signature;
            std::fprintf(stderr,"results_packet_list count=%zu\n",nativeResultsPackets.size());
            for(std::size_t i=0;i<nativeResultsPackets.size();++i){const auto& packet=nativeResultsPackets[i];
                float x0=1e9f,x1=-1e9f,y0=1e9f,y1=-1e9f;
                for(const auto& v:packet.vertices){x0=std::min(x0,v.position[0]);x1=std::max(x1,v.position[0]);
                    y0=std::min(y0,v.position[1]);y1=std::max(y1,v.position[1]);}
                const int blend=int(packet.hasTexture?packet.textured.blend:packet.untextured.blend);
                const int depth=int(packet.hasTexture?packet.textured.depthMethod:packet.untextured.depthMethod);
                const int zwrite=(packet.hasTexture?packet.textured.depthWrite:packet.untextured.depthWrite)?1:0;
                std::fprintf(stderr,"results_packet i=%zu textured=%d variant=%zu quads=%zu x=%.1f..%.1f y=%.1f..%.1f blend=%d depth=%d zwrite=%d alpha0=%d\n",
                    i,packet.hasTexture?1:0,packet.variant,packet.vertices.size()/4,x0,x1,y0,y1,blend,depth,zwrite,
                    packet.vertices.empty()?-1:int(packet.vertices[0].color[3]));
            }
        }
    }
    for(const auto& packet:nativeResultsPackets){if(!okay)break;
        const auto depth=packet.hasTexture?packet.textured.depthMethod:packet.untextured.depthMethod;
        const auto blend=packet.hasTexture?packet.textured.blend:packet.untextured.blend;
        gl.depthFunc(depth==0?0x0200:depth==1?0x0207:depth==2?0x0206:0x0204);
        gl.depthMask((packet.hasTexture?packet.textured.depthWrite:packet.untextured.depthWrite)?1:0);
        gl.uniform1i(uiSourceBlendUniform,static_cast<I>(blend));gl.uniform1i(uiTexturedUniform,packet.hasTexture?1:0);
        gl.bindTexture(texture2D,packet.hasTexture?nativeResultsVariants[packet.variant]->textures.texture(0):0);
        // 1B8DB0/1B8E10 LetterBoxPause bars (+38 bottom at y 448, +3C top at
        // y 0): the only untextured 4-vertex Results quads with every x at 0
        // or 640. On the PS2 they span the whole 640x448 screen, so the canvas
        // edges map to the window edges here (identity at an exact 10:7
        // window); animated ordinates (383, 65, ...) stay canvas-relative.
        // RR_ORIGINAL_ASPECT=1 (the widescreen switch) keeps them in the canvas.
        bool bar=!originalAspect&&!packet.hasTexture&&packet.vertices.size()==4;
        for(std::size_t k=0;bar&&k<4;++k){const float x=packet.vertices[k].position[0];bar=x==0.f||x==640.f;}
        std::array<assets::Vertex48B,4> edge{};
        if(bar){
            for(std::size_t k=0;k<4;++k){edge[k]=packet.vertices[k];auto& p=edge[k].position;
                p[0]=p[0]==0.f?float(-ox/scale):float((width-ox)/scale);
                if(p[1]==0.f)p[1]=float(-oy/scale);else if(p[1]==448.f)p[1]=float((height-oy)/scale);}
            gl.scissor(0,0,width,height);
        }
        // Upload this occurrence immediately; source packet order is retained
        // even when another occurrence shares its image/sampler variant.
        drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(packet.vertices.size()*sizeof(assets::Vertex48B)),
            bar?static_cast<const void*>(edge.data()):static_cast<const void*>(packet.vertices.data()));
        okay=check("upload Results native occurrence geometry");
        if(bar&&!loggedBars){loggedBars=true;loggedWidth=width;loggedHeight=height;
            std::fprintf(stderr,"results_letterbox_bar window=%dx%d scale=%.4f ox=%.1f oy=%.1f x=%.2f..%.2f y=%.2f,%.2f blend=%d alpha=%d\n",
                int(width),int(height),scale,ox,oy,edge[0].position[0],edge[1].position[0],edge[0].position[1],edge[2].position[1],
                int(blend),int(edge[0].color[3]));}
        const U indices=U(packet.vertices.size()/4)*6;
        if(okay&&uiSourceCoherentFramebufferFetch){
            drawGl.drawElementsBaseVertex(0x0004,I(indices),0x1405,nullptr,0);
            okay=check("draw Results native ordered occurrence");
        }else for(U i=0;okay&&i<indices;i+=3){gl.textureBarrier();
            drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(i)*sizeof(U)),0);
            okay=check("draw Results native ordered source-alpha triangle");}
        if(bar)gl.scissor(canvasX,canvasY,canvasW,canvasH);    // the next packet keeps the canvas
    }
    if(!uiSourceCoherentFramebufferFetch){gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);}
    gl.bindTexture(texture2D,0);gl.disable(0x0C11);gl.disable(0x0BE2);
    if(okay){gl.bindFramebuffer(readFramebuffer,nativeResultsTarget);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        okay=check("copy Results native contribution into pending gameplay target");}
    gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    // Native presentation alpha is opaque. The copied raw source A/depth stay
    // private and confer no original framebuffer or later-effect authority.
    gl.colorMask(0,0,0,1);gl.clearColor(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
    gl.enable(0x0B71);gl.depthMask(1);gl.clearDepth(1);gl.depthFunc(0x0201);gl.depthRange(0,1);
    gl.viewport(0,0,width,height);gl.useProgram(shader->program());
    drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after Results native packet leaf");
    return okay&&restored;
}

bool VehicleWindowGl50DS::prepareNativeResultsResources50LA(
    const game::OriginalResults10450LA& source,VehicleWindowDiagnostic50DS& d)noexcept {
    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"Results renderer unavailable");
    auto& s=*storage_;
    if(s.thread!=std::this_thread::get_id()||!source.impl_||
       source.impl_->nativeCaptureThread!=std::this_thread::get_id())
        return fail(d,VehicleWindowError50DS::wrongThread,"Results immutable renderer/source threads");
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options owns native mutation");
    if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic||s.frameNativeF8Bars||
       s.nativeF8BarFrame||s.nativeF8BarRenderer||s.nativeResultsPreparationFailed)
        return fail(d,VehicleWindowError50DS::frame,"Results upload needs idle nonfailed actual renderer");
    const auto& owner=*source.impl_;const auto* capture=owner.nativeDisplay.get();
    const auto table=owner.domain?owner.domain->materials382B38():nullptr;
    if(!capture||capture->thread!=std::this_thread::get_id()||capture->building||
       !capture->boundedDerivedReturned||capture->generation!=owner.component.sessionGeneration||
       !owner.context||!owner.context->live()||!table||!table->coldCollectorCurrent(capture->receipt))
        return fail(d,VehicleWindowError50DS::frame,"Results bounded native CPU capture absent or stale");
    // This copies only owned CPU bytes. No source pointer escapes into the
    // renderer, and upload success is not Frame/slot48/B4/draw authority.
    const auto sameSampler=[](const auto& a,const auto& b)noexcept {
        return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;
    };
    try {
        std::vector<std::unique_ptr<Storage::NativeResultsVariant>> variants;
        std::vector<Storage::NativeResultsPacket> packets;
        std::size_t quads=0,rgbaBytes=0;
        if(capture->packets.size()>originalUiMaximumQuads50KG)
            return fail(d,VehicleWindowError50DS::limits,"Results native packet budget");
        for(const auto& occurrence:capture->packets){
            if(occurrence.vertices.empty()||occurrence.vertices.size()%4||
               occurrence.vertices.size()/4>originalUiMaximumQuads50KG-quads)
                return fail(d,VehicleWindowError50DS::limits,"Results native quad budget");
            quads+=occurrence.vertices.size()/4;
            Storage::NativeResultsPacket packet;packet.vertices=occurrence.vertices;
            packet.hasTexture=occurrence.material.hasTexture;
            packet.textured=occurrence.material.textured;packet.untextured=occurrence.material.untextured;
            if(packet.hasTexture){
                const auto& image=occurrence.material.image;const auto& policy=packet.textured;
                const auto* decoded=image.images?image.images->decodedTexture(image.image):nullptr;
                if(!decoded||decoded->mips.empty()||
                   policy.blend==assets::OriginalUiMaterialBlend50KG::destinationAlphaAdditive)
                    return fail(d,VehicleWindowError50DS::material,"Results actual selected image/policy required");
                MaterialTextureSampler50EB sampler;
                // Independent min/mag and U/V. Never reuse F8's image-wide
                // equality assumption; DenmarkPause/Denmark16 may share PST.
                sampler.minification=policy.minification==assets::OriginalUiMaterialFilter50KG::nearest?
                    MaterialTextureFilter50EB::nearest:MaterialTextureFilter50EB::linear;
                sampler.magnification=policy.magnification==assets::OriginalUiMaterialFilter50KG::nearest?
                    MaterialTextureFilter50EB::nearest:MaterialTextureFilter50EB::linear;
                sampler.s=policy.u==assets::OriginalUiMaterialWrap50KG::repeat?MaterialTextureWrap50EB::repeat:MaterialTextureWrap50EB::clampToEdge;
                sampler.t=policy.v==assets::OriginalUiMaterialWrap50KG::repeat?MaterialTextureWrap50EB::repeat:MaterialTextureWrap50EB::clampToEdge;
                std::size_t index=0;
                for(;index<variants.size();++index)if(variants[index]->images==image.images&&
                    variants[index]->image==image.image&&sameSampler(variants[index]->sampler,sampler))break;
                if(index==variants.size()){
                    if(index>=Storage::maximumNativeResultsVariants||
                       decoded->mips.back().rgba.size()>Storage::maximumNativeResultsImageBytes-rgbaBytes)
                        return fail(d,VehicleWindowError50DS::limits,"Results bounded image/sampler variant budget");
                    rgbaBytes+=decoded->mips.back().rgba.size();
                    auto variant=std::make_unique<Storage::NativeResultsVariant>();
                    variant->images=image.images;variant->image=image.image;variant->sampler=sampler;
                    variants.push_back(std::move(variant));
                }
                packet.variant=index;
            }
            packets.push_back(std::move(packet));
        }
        bool reuse=variants.size()==s.nativeResultsVariants.size();
        for(std::size_t i=0;reuse&&i<variants.size();++i){const auto& old=*s.nativeResultsVariants[i];const auto& next=*variants[i];
            reuse=old.images==next.images&&old.image==next.image&&sameSampler(old.sampler,next.sampler)&&old.publication&&
                old.textures.validBorrow(s.request.context,s.request.isCurrent,old.publication)&&
                old.textures.matchesSourceBank(old.pixels,old.publication);
        }
        if(reuse){s.nativeResultsPackets=std::move(packets);return true;}
        // Prepare all CPU banks before any GPU mutation. Only the selected
        // frame is copied; other ATM frames are not redundantly uploaded.
        rgbaBytes=0;
        for(auto& variant:variants){
            const auto* decoded=variant->images->decodedTexture(variant->image);
            assets::MaterialTextureAssetInput50EA texture{1,&decoded->base_rrtex};
            assets::MaterialTextureAssetsInput50EA input;input.textures=&texture;input.textureCount=1;
            input.budgets={1,0,assets::maxMaterialTextureSourceBytes50EA,
                assets::maxMaterialTextureRGBABytes50EA,assets::max_texture_dimension};
            assets::MaterialTextureAssetDiagnostic50EA td;
            if(!variant->pixels.load(input,td))return fail(d,VehicleWindowError50DS::assets,"Results selected-frame native bank");
            if(variant->pixels.rgbaByteCount()>Storage::maximumNativeResultsImageBytes-rgbaBytes)
                return fail(d,VehicleWindowError50DS::limits,"Results exact uploaded-byte budget");
            rgbaBytes+=variant->pixels.rgbaByteCount();
        }
        if(s.hasNativeResultsResources()){
            if(!finishInitializedContextWork(d)||!d.contextWorkCompleted)return false;
            if(!s.deleteNativeResultsResources(d)){s.nativeResultsPreparationFailed=true;return false;}
        }
        s.nativeResultsPackets.clear();s.nativeResultsVariants=std::move(variants);
        s.nativeResultsPreparationFailed=true; // Every partial name is now renderer-owned.
        // Load cleanup entrypoints even for a previously untextured renderer.
        s.diagnostic=&d;
        const bool cleanupFunctions=s.load(s.gl.activeTexture,"glActiveTexture")&&s.load(s.gl.bindTexture,"glBindTexture");
        s.diagnostic=nullptr;
        if(!cleanupFunctions)return false;
        MaterialTextureGlRequest50EB request{};request.context=s.request.context;request.isCurrent=s.request.isCurrent;
        request.originalUploadBinding=s.request.originalUploadBinding;
        const char* missing=nullptr;
        if(!loadMaterialTextureFunctions50EE(s.request.context,s.request.procAddress,request.gl,missing))
            return fail(d,VehicleWindowError50DS::entryPoint,"Results native image upload functions");
        // MaterialTextureGl deliberately normalizes unpack state. Restore the
        // actual idle renderer's bindings on both success and partial failure.
        I saved[7]{};constexpr U keys[]{0x84E0,0x8069,0x88EF,0x0CF5,0x0CF2,0x0CF3,0x0CF4};
        for(std::size_t i=0;i<7;++i)request.gl.getIntegerv(keys[i],&saved[i]);
        if(request.gl.getError())return fail(d,VehicleWindowError50DS::gl,"Results upload state capture");
        const auto restore=[&]()noexcept {
            s.gl.activeTexture(U(saved[0]));request.gl.bindTexture(texture2D,U(saved[1]));
            request.gl.bindBuffer(0x88EC,U(saved[2]));
            for(std::size_t i=3;i<7;++i)request.gl.pixelStorei(keys[i],saved[i]);
            const U error=request.gl.getError();
            if(error){d.cleanupIncomplete=true;if(!d.cleanupGlError)d.cleanupGlError=error;return false;}
            return true;
        };
        for(auto& variant:s.nativeResultsVariants){
            request.samplers=&variant->sampler;request.samplerCount=1;request.maximumGpuBytes=variant->pixels.rgbaByteCount();
            MaterialTextureGlDiagnostic50EB td;
            if(!variant->textures.create(request,variant->pixels,td)){
                d.cleanupIncomplete=s.hasNativeResultsResources();
                restore();
                return fail(d,VehicleWindowError50DS::material,"Results native image/sampler upload incomplete");
            }
            variant->publication=variant->textures.publicationToken();
        }
        if(!restore())return fail(d,VehicleWindowError50DS::gl,"Results upload state restoration");
        s.nativeResultsPackets=std::move(packets);s.nativeResultsPreparationFailed=false;
        return true; // Upload only; deliberately no Frame field or drawing caller.
    }catch(...){d.cleanupIncomplete=s.hasNativeResultsResources();
        return fail(d,VehicleWindowError50DS::allocation,"Results native sink allocation");}
}

bool VehicleWindowGl50DS::Storage::hasNativeF8BarResources()const noexcept {
    return nativeF8BarTarget||nativeF8BarColor||nativeF8BarDepth||nativeF8SuffixTextures.ownsResources();
}

bool VehicleWindowGl50DS::Storage::createNativeF8BarResources()noexcept {
    // This explicit native preparation owns every allocated name immediately.
    // A failed prefix is terminal until ordinary renderer cleanup; no retry
    // overwrites an unresolved name or treats a partial UI program as ready.
    if(nativeF8BarReady||hasNativeF8BarResources()||!nativeF8BarPreparing)
        return fail(*diagnostic,VehicleWindowError50DS::request,"fresh F8 native scratch resources required");
    if(!load(gl.drawArrays,"glDrawArrays")||!load(gl.scissor,"glScissor")||
       !load(gl.blitFramebuffer,"glBlitFramebuffer")||!load(gl.genTextures,"glGenTextures")||
       !load(gl.deleteTextures,"glDeleteTextures")||!load(gl.activeTexture,"glActiveTexture")||
       !load(gl.bindTexture,"glBindTexture")||!load(gl.texImage2D,"glTexImage2D")||
       !load(gl.texParameteri,"glTexParameteri")||!load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||
       !load(gl.framebufferTexture2D,"glFramebufferTexture2D"))return false;
    if(!uiProgram&&!createOriginalUi())return false;
    if(!uiProgram||!uiVertexArray||!uiVertexBuffer)
        return fail(*diagnostic,VehicleWindowError50DS::shader,"F8 native UI geometry program incomplete");
    gl.genFramebuffers(1,&nativeF8BarTarget);
    if(!check("create F8 native scratch framebuffer")||!nativeF8BarTarget)
        return fail(*diagnostic,VehicleWindowError50DS::allocation,"F8 scratch framebuffer allocation failed");
    gl.bindFramebuffer(framebuffer,nativeF8BarTarget);
    // Private native RGBA8 source-alpha plane, never a canonical image or
    // world-depth substitute. A texture permits the existing strict shader
    // feedback equation for the icon/font suffix without GL blend shortcuts.
    I limit=0;gl.getIntegerv(0x0D33,&limit);
    if(!check("query F8 private texture extent")||limit<maximumWidth||limit<maximumHeight)return false;
    gl.genTextures(1,&nativeF8BarColor);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,nativeF8BarColor);
    gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
    gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
    gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);drawGl.bindBuffer(0x88EC,0);
    gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
    if(!check("allocate F8 private source-alpha plane")||!nativeF8BarColor)return false;
    I w=0,h=0,format=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
    gl.getTexLevelParameteriv(texture2D,0,0x1001,&h);gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
    if(!check("verify F8 source-alpha storage")||w!=maximumWidth||h!=maximumHeight||format!=I(rgba8))return false;
    gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,nativeF8BarColor,0);
    gl.bindTexture(texture2D,0);
    if(!createAttachment(nativeF8BarDepth,depth24,depthAttachment))return false;
    gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    const U status=gl.checkFramebufferStatus(framebuffer);
    if(!check("complete F8 native scratch target")||status!=framebufferComplete){
        diagnostic->framebufferStatus=status;
        return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"F8 native scratch target incomplete");
    }
    return true;
}

bool VehicleWindowGl50DS::prepareNativeF8BarResources50LA(VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted())return fail(d,VehicleWindowError50DS::frame,
        "Options viewport owns native mutation until cancellation");
    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;
    if(s.thread!=std::this_thread::get_id())return fail(d,VehicleWindowError50DS::wrongThread,"F8 requires renderer creator thread");
    if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic||s.frameNativeF8Bars||
       s.nativeF8BarFrame||s.nativeF8BarRenderer)
        return fail(d,VehicleWindowError50DS::frame,"F8 resources require the idle actual gameplay renderer");
    if(s.nativeF8BarPreparationFailed||s.raceHudUiCreationFailed||s.timeStatsUiCreationFailed||
       s.positionStatsUiCreationFailed||s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||
       s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"failed shared UI preparation remains cleanup-owned");
    if(s.nativeF8BarReady){
        if(s.nativeF8BarTarget&&s.nativeF8BarColor&&s.nativeF8BarDepth&&s.uiProgram&&
           s.uiVertexArray&&s.uiVertexBuffer&&s.gl.drawArrays&&s.gl.scissor&&s.gl.blitFramebuffer)return true;
        return fail(d,VehicleWindowError50DS::frame,"F8 prepared native resource identity changed");
    }
    s.diagnostic=&d;s.nativeF8BarPreparationFailed=true;s.nativeF8BarPreparing=true;
    const bool created=s.check("before explicit F8 native resource preparation")&&s.createNativeF8BarResources();
    s.nativeF8BarPreparing=false;
    s.gl.bindTexture(texture2D,0);s.gl.bindFramebuffer(framebuffer,0);s.gl.bindRenderbuffer(renderbuffer,0);
    s.drawGl.bindVertexArray(0);s.drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=s.check("restore bindings after F8 native resource preparation");
    s.diagnostic=nullptr;
    if(created&&restored){s.nativeF8BarReady=true;s.nativeF8BarPreparationFailed=false;return true;}
    // Keep FBO, renderbuffers and any shared shader/VBO prefix. The ordinary
    // renderer close owns completion and acknowledged deletion of those names.
    d.cleanupIncomplete=s.hasGlResources();return false;
}

bool VehicleWindowGl50DS::prepareNativeF8SuffixResources50LA(
    const ::original_main_entry50KG::NativeF8BarPrefix50LA& batch,VehicleWindowDiagnostic50DS& d)noexcept {
    OriginalOptionsMutationInterlock50KG::Operation optionsMutation;
    if(!optionsMutation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options owns native mutation");
    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"F8 renderer unavailable");
    auto& s=*storage_;
    if(s.thread!=std::this_thread::get_id())return fail(d,VehicleWindowError50DS::wrongThread,"F8 requires renderer creator thread");
    if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic||s.frameNativeF8Bars||
       !s.nativeF8BarReady||s.nativeF8BarPreparationFailed)
        return fail(d,VehicleWindowError50DS::frame,"F8 suffix upload requires actual idle prepared renderer");
    const auto& suffix=batch.suffix();
    if(!suffix.quads)return suffix.packetCount==0;
    const auto* bank=suffix.images?suffix.images->textures():nullptr;
    if(!bank||bank->textureCount()!=3||!suffix.packetCount||suffix.packetCount>batch.maximumSuffixPackets||
       suffix.quads>batch.maximumSuffixQuads)
        return fail(d,VehicleWindowError50DS::assets,"F8 exact privately decoded three-image bank required");
    static_assert(::original_main_entry50KG::NativeF8BarPrefix50LA::maximumSuffixQuads<=originalUiMaximumQuads50KG);
    std::array<bool,3> used{};std::array<MaterialTextureSampler50EB,3> samplers{};
    const auto sameSampler=[](const auto& a,const auto& b)noexcept {
        return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;
    };
    // CURRENT reached primitive policies are supplied by this actual batch,
    // after source-ordered service. No prior frame's FINAL policy is cached.
    for(U p=0;p<suffix.packetCount;++p){const auto& packet=suffix.packets[p];
        MaterialTextureSampler50EB sampler;
        if(packet.image>=3||packet.policy.blend==assets::OriginalUiMaterialBlend50KG::destinationAlphaAdditive||
           !s.positionStatsSampler(packet.policy,sampler))
            return fail(d,VehicleWindowError50DS::material,"F8 supported actual one-map policy required");
        if(used[packet.image]&&!sameSampler(samplers[packet.image],sampler))
            return fail(d,VehicleWindowError50DS::material,"F8 same image has conflicting current packet samplers");
        samplers[packet.image]=sampler;used[packet.image]=true;
    }
    const bool sameImages=s.nativeF8SuffixImages==suffix.images&&s.nativeF8SuffixPublication&&
        s.nativeF8SuffixTextures.validBorrow(s.request.context,s.request.isCurrent,s.nativeF8SuffixPublication)&&
        s.nativeF8SuffixTextures.matchesSourceBank(*bank,s.nativeF8SuffixPublication);
    bool reuse=sameImages;
    for(U i=0;i<3;++i){
        if(used[i]){if(!sameSampler(samplers[i],s.nativeF8SuffixSamplers[i]))reuse=false;}
        else if(sameImages)samplers[i]=s.nativeF8SuffixSamplers[i];
        else samplers[i]={MaterialTextureFilter50EB::nearest,MaterialTextureFilter50EB::nearest,
            MaterialTextureWrap50EB::clampToEdge,MaterialTextureWrap50EB::clampToEdge};
    }
    // Real upload/context publication and immutable bank identity suffice for
    // native pixel reuse; they never acknowledge a source renderer/FIFO phase.
    if(reuse)return true;
    // Changed images or reached-image samplers need real idle completion and
    // acknowledged retirement. A same-key hit performs neither decode/upload
    // nor glFinish, and never mutates a published MaterialTextureGl image.
    if(s.nativeF8SuffixTextures.ownsResources()){
        if(!finishInitializedContextWork(d)||!d.contextWorkCompleted)return false;
        MaterialTextureGlDiagnostic50EB td;
        if(!s.nativeF8SuffixTextures.close(td)){
            d.cleanupIncomplete=td.cleanupIncomplete;s.nativeF8BarPreparationFailed=true;return false;
        }
        s.nativeF8SuffixImages.reset();s.nativeF8SuffixPublication=0;
    }
    s.nativeF8SuffixSamplers=samplers;
    s.diagnostic=&d;s.nativeF8BarPreparationFailed=true;
    bool okay=true;
    if(!s.uiSourceCoherentFramebufferFetch){
        I major=0,minor=0,units=0,count=0;s.gl.getIntegerv(0x821B,&major);s.gl.getIntegerv(0x821C,&minor);
        s.gl.getIntegerv(0x8872,&units);bool barrier=major>4||(major==4&&minor>=5);
        okay=s.load(s.gl.getStringi,"glGetStringi")&&s.check("query F8 native source-alpha capabilities");
        if(okay){s.gl.getIntegerv(0x821D,&count);okay=s.check("query F8 extensions")&&count>=0&&count<=65536&&units>=2;}
        for(I i=0;okay&&i<count;++i){const auto* extension=s.gl.getStringi(0x1F03,U(i));
            okay=s.check("read F8 extension")&&extension;
            if(okay&&!std::strcmp(reinterpret_cast<const char*>(extension),"GL_ARB_texture_barrier"))barrier=true;}
        okay=okay&&barrier&&s.load(s.gl.textureBarrier,"glTextureBarrier");
    }
    if(okay){
        MaterialTextureGlRequest50EB request{};request.context=s.request.context;request.isCurrent=s.request.isCurrent;
        request.originalUploadBinding=s.request.originalUploadBinding;
        request.samplers=s.nativeF8SuffixSamplers.data();request.samplerCount=3;
        request.maximumGpuBytes=assets::maxMaterialTextureRGBABytes50EA;
        const char* missing=nullptr;MaterialTextureGlDiagnostic50EB td;
        s.nativeF8SuffixImages=suffix.images;
        okay=loadMaterialTextureFunctions50EE(s.request.context,s.request.procAddress,request.gl,missing)&&
            s.nativeF8SuffixTextures.create(request,*bank,td);
        if(okay)s.nativeF8SuffixPublication=s.nativeF8SuffixTextures.publicationToken();
        else d.cleanupIncomplete=s.nativeF8SuffixTextures.ownsResources();
    }
    s.gl.activeTexture(textureUnit0);s.gl.bindTexture(texture2D,0);
    const bool restored=s.check("restore F8 suffix upload bindings");s.diagnostic=nullptr;
    if(okay&&restored){s.nativeF8BarPreparationFailed=false;return true;}
    if(d.error==VehicleWindowError50DS::none)fail(d,VehicleWindowError50DS::material,"F8 native source-alpha/image upload incomplete");
    d.cleanupIncomplete=s.hasGlResources();return false;
}

bool VehicleWindowGl50DS::Storage::prepareNativeF8BarFrame(const VehicleWindowGl50DS& renderer,
    const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d)noexcept {
    if(!frame.nativeF8BarPrefix)return true;
    const auto* batch=frame.nativeF8BarPrefix;
    if(!nativeF8BarReady||nativeF8BarPreparationFailed||!nativeF8BarTarget||
       !nativeF8BarColor||!nativeF8BarDepth||!uiProgram||!uiVertexArray||!uiVertexBuffer||
       frame.width<=0||frame.height<=0||frame.width>maximumWidth||frame.height>maximumHeight||
       !batch->current(renderer,frame))
        return fail(d,VehicleWindowError50DS::frame,"F8 same current native batch/Frame/Display required");
    if(batch->enabled()){
        const auto& source=batch->vertices();
        for(const auto& vertex:source){
            for(float value:vertex.position)if(!std::isfinite(value))
                return fail(d,VehicleWindowError50DS::frame,"F8 native packet position is nonfinite");
            if(vertex.position[2]!=16777205.f||vertex.color[0]||vertex.color[1]||
               vertex.color[2]||vertex.color[3]!=63)
                return fail(d,VehicleWindowError50DS::material,"F8 actual depth10 and raw opaque RGBA required");
        }
        nativeF8BarVertices=source;
    }
    const auto& suffix=batch->suffix();
    if(suffix.quads){
        if(!suffix.images||nativeF8SuffixImages!=suffix.images||!nativeF8SuffixPublication||
           !nativeF8SuffixTextures.validBorrow(request.context,request.isCurrent,nativeF8SuffixPublication)||
           !nativeF8SuffixTextures.matchesSourceBank(*suffix.images->textures(),nativeF8SuffixPublication))
            return fail(d,VehicleWindowError50DS::frame,"F8 actual immutable uploaded suffix bank changed");
        std::array<bool,::original_main_entry50KG::NativeF8BarPrefix50LA::maximumSuffixQuads> covered{};
        for(U p=0;p<suffix.packetCount;++p){const auto& packet=suffix.packets[p];
            if(packet.image>=3||!packet.count||packet.first>suffix.quads||packet.count>suffix.quads-packet.first)
                return fail(d,VehicleWindowError50DS::frame,"F8 ordered suffix packet bounds");
            for(U q=packet.first;q<packet.first+packet.count;++q){
                if(covered[q])return fail(d,VehicleWindowError50DS::frame,"F8 duplicate suffix quad");covered[q]=true;
                for(U v=0;v<4;++v){const auto& vertex=suffix.vertices[4*q+v];
                    for(float f:vertex.position)if(!std::isfinite(f))return false;
                    for(float f:vertex.uv)if(!std::isfinite(f))return false;
                    if(vertex.position[2]!=16777214.f||vertex.color[3]>packet.policy.maximumVertexAlpha)return false;
                }
            }
        }
        for(U q=0;q<suffix.quads;++q)if(!covered[q])return false;
    }
    nativeF8Suffix=suffix;
    frameNativeF8Bars=batch;nativeF8BarFrame=&frame;nativeF8BarRenderer=&renderer;
    d.nativeF8BarSuffixPending=batch->suffixPending();
    return true;
}

bool VehicleWindowGl50DS::Storage::drawNativeF8Bars(U bank)noexcept {
    if(!frameNativeF8Bars)return true;
    const auto* batch=frameNativeF8Bars;
    if(bank>1||!nativeF8BarFrame||!nativeF8BarRenderer||
       nativeF8BarFrame->nativeF8BarPrefix!=batch||!nativeF8BarReady||nativeF8BarPreparationFailed||
       !batch->current(*nativeF8BarRenderer,*nativeF8BarFrame))
        return fail(*diagnostic,VehicleWindowError50DS::frame,"F8 native prefix owners changed before drawing");
    if(!batch->enabled()&&!nativeF8Suffix.quads)return true;
    if(batch->enabled()&&std::memcmp(nativeF8BarVertices.data(),batch->vertices().data(),sizeof(nativeF8BarVertices)))
        return fail(*diagnostic,VehicleWindowError50DS::frame,"F8 immutable native packet copy changed");

    gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);
    gl.disable(0x0B44);gl.polygonMode(0x0408,0x1B02);
    gl.colorMask(1,1,1,1);
    gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
    gl.bindFramebuffer(drawFramebuffer,nativeF8BarTarget);gl.drawBuffer(colorAttachment);
    gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
    gl.bindFramebuffer(framebuffer,nativeF8BarTarget);gl.viewport(0,0,width,height);
    gl.depthMask(1);gl.clearDepth(0);gl.clear(depthBufferBit);
    // Source depthMethod1 means ALWAYS, NOT disabled depth testing. Its
    // inherited ZMSK0 still writes actual packet Z to this private depth plane.
    gl.enable(0x0B71);gl.depthFunc(0x0207);gl.depthRange(0,1);
    resetNativeGcLighting();gl.useProgram(uiProgram);
    const double scale=std::min(double(width)/640.0,double(height)/448.0);
    const double ox=(width-640.0*scale)*.5,oy=(height-448.0*scale)*.5;
    gl.uniform4f(uiCanvasUniform,float(2*scale/width),float(-2*scale/height),
        float(2*ox/width-1),float(1-2*oy/height));
    gl.scissor(I(std::floor(ox)),I(std::floor(oy)),I(std::ceil(ox+640*scale))-I(std::floor(ox)),
        I(std::ceil(oy+448*scale))-I(std::floor(oy)));gl.enable(0x0C11);
    gl.uniform1i(gcParticleModeUniform,0);gl.uniform1i(uiSourceCompositionUniform,0);
    gl.uniform1i(uiSourceRasterUniform,0);gl.uniform1i(uiTexturedUniform,0);
    gl.uniform1i(uiSourceFogEnabledUniform,0);gl.uniform1i(uiSourceAlphaTestUniform,0);
    gl.uniform1i(uiSourceBlendUniform,0);gl.uniform1i(uiCoveragePayloadUniform,0);
    gl.uniform1f(uiAlphaUniform,1.f);
    // Opaque TME0 uses normalized raw vertex channels. A63 is stored63/255,
    // never treated as a63/128 source-over coefficient or a white texture.
    drawGl.bindVertexArray(uiVertexArray);drawGl.bindBuffer(arrayBuffer,uiVertexBuffer);
    drawGl.bufferSubData(arrayBuffer,0,sizeof(nativeF8BarVertices),nativeF8BarVertices.data());
    bool okay=check("prepare ordered F8 native bar-prefix strips");
    for(U bar=0;okay&&batch->enabled()&&bar<4;++bar){
        // Actual TL,BL,TR,BR packet order. Four separate strips preserve both
        // source triangles and ascending30/34/38/3C order without a quad diagonal change.
        gl.drawArrays(0x0005,I(bar*4),4);
        okay=check("draw F8 native bar-prefix strip");
        if(okay)++diagnostic->nativeF8BarQuads;
    }
    if(okay&&nativeF8Suffix.quads){
        if(nativeF8SuffixImages!=nativeF8Suffix.images||
           !nativeF8SuffixTextures.validBorrow(request.context,request.isCurrent,nativeF8SuffixPublication))okay=false;
        if(okay){
            gl.uniform1i(uiSourceCompositionUniform,1);gl.uniform1i(uiTexturedUniform,1);gl.uniform1i(uiImageUniform,0);
            if(!uiSourceCoherentFramebufferFetch){gl.uniform1i(uiSourceTargetUniform,1);gl.activeTexture(textureUnit0+1);
                if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,nativeF8BarColor);}
            gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
            drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(nativeF8Suffix.quads)*4*sizeof(assets::Vertex48B),nativeF8Suffix.vertices.data());
            drawGl.bindBuffer(elementBuffer,uiIndexBuffer);okay=check("prepare F8 source-ordered icon/text packets");
            for(U p=0;okay&&p<nativeF8Suffix.packetCount;++p){const auto& packet=nativeF8Suffix.packets[p];const auto& policy=packet.policy;
                gl.depthFunc(policy.depthMethod==0?0x0200:policy.depthMethod==1?0x0207:policy.depthMethod==2?0x0206:0x0204);
                gl.depthMask(policy.depthWrite?1:0);gl.uniform1i(uiSourceBlendUniform,static_cast<I>(policy.blend));
                gl.bindTexture(texture2D,nativeF8SuffixTextures.texture(packet.image));
                if(uiSourceCoherentFramebufferFetch){
                    drawGl.drawElementsBaseVertex(0x0004,I(packet.count*6),0x1405,
                        reinterpret_cast<const void*>(std::uintptr_t(packet.first)*6*sizeof(U)),0);
                    okay=check("draw F8 ordered icon/text primitive");
                }else for(U i=packet.first*6;okay&&i<(packet.first+packet.count)*6;i+=3){
                    gl.textureBarrier();drawGl.drawElementsBaseVertex(0x0004,3,0x1405,
                        reinterpret_cast<const void*>(std::uintptr_t(i)*sizeof(U)),0);
                    okay=check("draw F8 ordered source-alpha triangle");
                }
                if(okay){diagnostic->nativeF8SuffixQuads+=packet.count;++diagnostic->nativeF8SuffixPackets;}
            }
            if(!uiSourceCoherentFramebufferFetch){gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);}
            gl.bindTexture(texture2D,0);
        }
    }
    gl.disable(0x0C11);
    if(okay){
        gl.bindFramebuffer(readFramebuffer,nativeF8BarTarget);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        okay=check("copy F8 native bar prefix into pending gameplay target");
    }
    gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    // Native presentation remains opaque. Raw source A stays private and is
    // not offered as an input to any later original effect or UI receipt.
    gl.colorMask(0,0,0,1);gl.clearColor(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
    gl.disable(0x0BE2);gl.disable(0x0C11);gl.enable(0x0B71);
    gl.depthMask(1);gl.clearDepth(1);gl.depthFunc(0x0201);gl.depthRange(0,1);
    gl.viewport(0,0,width,height);gl.useProgram(shader->program());
    drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after F8 native bar prefix");
    // This is only a native image contribution. No199500 return, source
    // collector record,105990 retirement or child acknowledgment.
    return okay&&restored;
}

bool VehicleWindowGl50DS::Storage::deleteNativeF8BarNames(VehicleWindowDiagnostic50DS& d)noexcept {
    nativeF8BarReady=false;
    MaterialTextureGlDiagnostic50EB td;
    if(!nativeF8SuffixTextures.close(td)){
        if(td.cleanupIncomplete){d.cleanupIncomplete=true;return false;}
        if(!d.cleanupGlError)d.cleanupGlError=td.glError?td.glError:td.cleanupGlError;
    }
    nativeF8SuffixImages.reset();nativeF8SuffixPublication=0;nativeF8Suffix={};
    if(!deleteName(nativeF8BarTarget,gl.deleteFramebuffers,d)||
       !deleteName(nativeF8BarColor,gl.deleteTextures,d)||
       !deleteName(nativeF8BarDepth,gl.deleteRenderbuffers,d))return false;
    return true; //Shared UI names retain their existing later cleanup owner.
}
}
