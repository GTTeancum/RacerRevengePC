// Included inside VehicleWindowGl50DS::Storage after original_damage_render.inl.
// Genuine derived1BB010 or ordered single-view outer-tail batch. One depth
// plane for the entire batch; no complete outer Pause callback is claimed.
// Explicit native-host scheduling; no retail HUD ancestry or GPU residency claim.
struct PauseDerived {
    static constexpr U maximumQuads=game::OriginalPauseNativeDraw50LA::maximumQuads;
    static constexpr U maximumPackets=game::OriginalPauseNativeDraw50LA::maximumPackets;
    static_assert(maximumQuads<=originalUiMaximumQuads50KG,"shared UI buffers must cover PauseDerived");
    std::shared_ptr<const game::OriginalPauseNativeDraw50LA> owner;
    const game::OriginalCameraViewportFields* viewport=nullptr;
    std::uint64_t scene=0,generation=0,serial=0,publication=0,cameraPublication=0;
    MaterialTextureGl50EB textures;
    std::array<MaterialTextureSampler50EB,game::OriginalPauseNativeDraw50LA::maximumMaterials> samplers{};
    std::array<const game::OriginalPauseNativeResources50LA::Pass*,game::OriginalPauseNativeDraw50LA::maximumMaterials> policies{};
    std::array<U,game::OriginalPauseNativeDraw50LA::maximumMaterials> images{};
    std::array<assets::Vertex48B,maximumQuads*4> vertices{};
    std::array<game::OriginalPauseNativeDraw50LA::Packet,maximumPackets> packets{};
    U target=0,color=0,depth=0,quads=0,packetCount=0;
    bool ready=false,frame=false;
};
std::unique_ptr<PauseDerived> pauseDerived;
bool pauseDerivedPreparing=false,pauseDerivedUiCreationFailed=false;
bool hasPauseDerivedResources()const noexcept {
    return pauseDerived&&(pauseDerived->target||pauseDerived->color||pauseDerived->depth||pauseDerived->textures.ownsResources());
}
bool preparePauseDerivedResources(std::shared_ptr<const game::OriginalPauseNativeDraw50LA> owner) noexcept {
    if(pauseDerived||frontendOnly||active||frameSet||!owner||!owner->current())
        return fail(*diagnostic,VehicleWindowError50DS::request,"fresh original PauseDerived cold owner required");
    pauseDerived.reset(new(std::nothrow) PauseDerived);
    if(!pauseDerived)return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate original PauseDerived storage");
    auto& h=*pauseDerived;h.owner=std::move(owner);
    const auto* bank=h.owner->pixels();
    if(!bank||bank->textureCount()>h.samplers.size())
        return fail(*diagnostic,VehicleWindowError50DS::assets,"original Pause derived image bank required");
    for(U i=0;i<h.policies.size();++i){
        const auto* p=h.owner->material(i);h.policies[i]=p;h.images[i]=h.owner->image(i);
        if(!p||!p->enabled)continue;
        const auto blend=p->hasTexture?p->textured.blend:p->untextured.blend;
        const auto depth=p->hasTexture?p->textured.depthMethod:p->untextured.depthMethod;
        const auto context=p->hasTexture?p->textured.environmentContext:p->untextured.environmentContext;
        // The inherited gameplay target does not supply genuine GS destination
        // alpha; never admit a destination-alpha-dependent Pause pass here.
        if(blend==assets::OriginalUiMaterialBlend50KG::destinationAlphaAdditive||depth>3||context)
            return fail(*diagnostic,VehicleWindowError50DS::material,"unsupported original Pause derived raster");
        if(p->hasTexture&&(h.images[i]>=bank->textureCount()||!positionStatsSampler(p->textured,h.samplers[h.images[i]])))
            return fail(*diagnostic,VehicleWindowError50DS::material,"unsupported original Pause sampler");
    }
    // A texture-feedback path is needed only for this native private target.
    // Do not change the already compiled UI program's coherent-fetch variant.
    if(!uiSourceCoherentFramebufferFetch){
        I major=0,minor=0,units=0,count=0;gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);
        gl.getIntegerv(0x8872,&units);bool barrier=major>4||(major==4&&minor>=5);
        if(!load(gl.getStringi,"glGetStringi")||!check("query original PauseDerived source-alpha capabilities"))return false;
        gl.getIntegerv(0x821D,&count);
        if(!check("query original PauseDerived extensions")||count<0||count>65536||units<2)
            return fail(*diagnostic,VehicleWindowError50DS::context,"bounded original PauseDerived source-alpha support unavailable");
        for(I i=0;i<count;++i){const auto* p=gl.getStringi(0x1F03,U(i));
            if(!check("read original PauseDerived extension")||!p)return false;
            if(!std::strcmp(reinterpret_cast<const char*>(p),"GL_ARB_texture_barrier"))barrier=true;}
        if(!barrier||!load(gl.textureBarrier,"glTextureBarrier"))
            return fail(*diagnostic,VehicleWindowError50DS::context,"original PauseDerived requires GL4.5 or ARB_texture_barrier");
    }
    if(!load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
        !load(gl.scissor,"glScissor")||!load(gl.activeTexture,"glActiveTexture")||!load(gl.bindTexture,"glBindTexture")||
        !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
        !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||!load(gl.framebufferTexture2D,"glFramebufferTexture2D")||
        !load(gl.blitFramebuffer,"glBlitFramebuffer"))return false;
    MaterialTextureGlRequest50EB requestTextures{};requestTextures.context=request.context;
    requestTextures.originalUploadBinding=request.originalUploadBinding;
    requestTextures.isCurrent=request.isCurrent;requestTextures.samplers=h.samplers.data();requestTextures.samplerCount=bank->textureCount();
    requestTextures.maximumGpuBytes=assets::maxMaterialTextureRGBABytes50EA;const char* missing=nullptr;
    MaterialTextureGlDiagnostic50EB textureDiagnostic;
    if(!loadMaterialTextureFunctions50EE(request.context,request.procAddress,requestTextures.gl,missing)||
        !h.textures.create(requestTextures,*bank,textureDiagnostic))
        return fail(*diagnostic,VehicleWindowError50DS::material,"upload authentic original PauseDerived images");
    h.publication=h.textures.publicationToken();
    if(!uiProgram){
        // A failed late compilation can retain partially created shared names.
        // They remain renderer-owned for close; never reuse them as readiness.
        pauseDerivedUiCreationFailed=true;pauseDerivedPreparing=true;
        const bool okay=createOriginalUi();pauseDerivedPreparing=false;if(!okay)return false;
        pauseDerivedUiCreationFailed=false;
    }
    I limit=0;gl.getIntegerv(0x0D33,&limit);
    if(!check("query original PauseDerived texture extent")||limit<maximumWidth||limit<maximumHeight)
        return fail(*diagnostic,VehicleWindowError50DS::limits,"original PauseDerived private target exceeds texture limit");
    gl.genFramebuffers(1,&h.target);gl.bindFramebuffer(framebuffer,h.target);
    gl.genTextures(1,&h.color);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,h.color);
    gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
    gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
    gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);drawGl.bindBuffer(0x88EC,0);
    gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
    if(!check("allocate native-size original PauseDerived source target")||!h.target||!h.color)return false;
    I w=0,height=0,format=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
    gl.getTexLevelParameteriv(texture2D,0,0x1001,&height);gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
    if(!check("verify original PauseDerived source storage")||w!=maximumWidth||height!=maximumHeight||format!=I(rgba8))return false;
    gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,h.color,0);
    if(!createAttachment(h.depth,depth24,depthAttachment))return false;
    gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    const U status=gl.checkFramebufferStatus(framebuffer);gl.bindTexture(texture2D,0);gl.bindFramebuffer(framebuffer,0);
    if(!check("complete original PauseDerived private target")||status!=framebufferComplete){diagnostic->framebufferStatus=status;return false;}
    h.ready=true;return true;
}
bool preparePauseDerivedFrame(const game::OriginalPauseNativeDraw50LA* owner) noexcept {
    if(pauseDerived){pauseDerived->frame=false;pauseDerived->quads=pauseDerived->packetCount=0;}
    if(!owner)return true;
    if(!pauseDerived||!pauseDerived->ready||owner!=pauseDerived->owner.get()||!owner->current())return false;
    auto& h=*pauseDerived;const auto* draw=owner->frame();if(!draw)return false;
    const auto& source=*draw;const auto* v=source.viewport;
    if(!source.prepared||!source.serial||source.serial<h.serial||!source.cameraPublication||
       !v||!v->prepared||!v->matricesReady||!v->sceneKey||!v->sessionGeneration||
       v->x1E0||v->y1E4||v->width1E8!=640||v->height1EC!=448||
       (h.viewport&&(h.viewport!=v||h.scene!=v->sceneKey||h.generation!=v->sessionGeneration))||
       source.quadCount>PauseDerived::maximumQuads||source.packetCount>PauseDerived::maximumPackets||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    std::array<bool,PauseDerived::maximumQuads> covered{};
    for(U p=0;p<source.packetCount;++p){const auto& packet=source.packets[p];
        if(packet.material>=h.policies.size()||!h.policies[packet.material]||!h.policies[packet.material]->enabled||packet.leaf<0||!packet.count||
           packet.first>source.quadCount||packet.count>source.quadCount-packet.first)return false;
        for(U q=packet.first;q<packet.first+packet.count;++q){
            if(covered[q])return false;covered[q]=true;
            const auto& quad=source.quads[q];const auto& policy=*h.policies[packet.material];
            if(packet.leaf!=policy.canonical.origin.finalLeaf)return false;
            for(U i=0;i<4;++i){const auto& in=quad.vertices[i];
                if(in.position[2]!=16777214.f||!std::isfinite(in.position[0])||!std::isfinite(in.position[1])||
                   !std::isfinite(in.uv[0])||!std::isfinite(in.uv[1])||
                   in.color[3]>(policy.hasTexture?policy.textured.maximumVertexAlpha:policy.untextured.maximumVertexAlpha))return false;
                h.vertices[q*4+i]=in;
            }
        }
        h.packets[p]=packet;
    }
    for(U q=0;q<source.quadCount;++q)if(!covered[q])return false;
    h.viewport=v;h.scene=v->sceneKey;h.generation=v->sessionGeneration;h.serial=source.serial;h.cameraPublication=source.cameraPublication;
    h.quads=source.quadCount;h.packetCount=source.packetCount;h.frame=h.quads!=0;return true;
}
bool drawPauseDerived(U bank) noexcept {
    if(!pauseDerived||!pauseDerived->frame)return true;
    auto& h=*pauseDerived;
    if(!h.ready||!h.owner||!h.owner->current()||
       !h.owner->frame()||!h.owner->frame()->prepared||h.owner->frame()->serial!=h.serial||
       h.owner->frame()->cameraPublication!=h.cameraPublication||h.owner->frame()->viewport!=h.viewport||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    // Keep the native image at its real drawable resolution. Only this private
    // scratch target receives raw source alpha and a separate UI depth plane.
    gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);gl.disable(0x0B44);
    gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
    gl.bindFramebuffer(drawFramebuffer,h.target);gl.drawBuffer(colorAttachment);
    gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
    gl.bindFramebuffer(framebuffer,h.target);gl.viewport(0,0,width,height);
    gl.depthMask(1);gl.clearDepth(0);gl.clear(depthBufferBit);
    // The authenticated source-alpha equation uses As, never Ad.
    // Source alpha remains private; gameplay framebuffer alpha is not claimed.
    gl.colorMask(1,1,1,1);gl.enable(0x0B71);gl.depthFunc(0x0206);gl.depthRange(0,1);
    resetNativeGcLighting();gl.useProgram(uiProgram);
    const double scale=std::min(double(width)/640.0,double(height)/448.0);
    const double ox=(width-640.0*scale)*.5,oy=(height-448.0*scale)*.5;
    gl.uniform4f(uiCanvasUniform,float(2*scale/width),float(-2*scale/height),float(2*ox/width-1),float(1-2*oy/height));
    gl.scissor(I(std::floor(ox)),I(std::floor(oy)),I(std::ceil(ox+640*scale))-I(std::floor(ox)),
        I(std::ceil(oy+448*scale))-I(std::floor(oy)));gl.enable(0x0C11);
    gl.uniform1i(gcParticleModeUniform,0);gl.uniform1i(uiSourceCompositionUniform,1);
    gl.uniform1i(uiTexturedUniform,1);gl.uniform1i(uiSourceRasterUniform,0);gl.uniform1i(uiSourceFogEnabledUniform,0);
    gl.uniform1i(uiSourceAlphaTestUniform,0);gl.uniform1f(uiAlphaUniform,1);gl.uniform1i(uiImageUniform,0);
    if(!uiSourceCoherentFramebufferFetch){gl.uniform1i(uiSourceTargetUniform,1);gl.activeTexture(textureUnit0+1);
        if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,h.color);}
    gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
    drawGl.bindVertexArray(uiVertexArray);drawGl.bindBuffer(arrayBuffer,uiVertexBuffer);
    drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(h.quads)*4*sizeof(assets::Vertex48B),h.vertices.data());
    drawGl.bindBuffer(elementBuffer,uiIndexBuffer);
    bool okay=check("prepare authentic PauseDerived source pass");

    for(U p=0;okay&&p<h.packetCount;++p){const auto& packet=h.packets[p];
        const auto& policy=*h.policies[packet.material];
        const auto depth=policy.hasTexture?policy.textured.depthMethod:policy.untextured.depthMethod;
        gl.depthFunc(depth==0?0x0200:depth==1?0x0207:depth==2?0x0206:0x0204);
        gl.depthMask((policy.hasTexture?policy.textured.depthWrite:policy.untextured.depthWrite)?1:0);
        const auto blend=policy.hasTexture?policy.textured.blend:policy.untextured.blend;
        gl.uniform1i(uiSourceBlendUniform,static_cast<I>(blend));
        gl.uniform1i(uiTexturedUniform,policy.hasTexture?1:0);
        gl.bindTexture(texture2D,policy.hasTexture?h.textures.texture(h.images[packet.material]):0);
        if(uiSourceCoherentFramebufferFetch){
            drawGl.drawElementsBaseVertex(0x0004,I(packet.count*6),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(packet.first)*6*sizeof(U)),0);
            okay=check("draw ordered original PauseDerived source leaf");
        }else{
            for(U i=packet.first*6;okay&&i<(packet.first+packet.count)*6;i+=3){gl.textureBarrier();
                drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(i)*sizeof(U)),0);
                okay=check("draw ordered original PauseDerived source triangle");}
        }
    }
    if(!uiSourceCoherentFramebufferFetch){gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);}
    gl.bindTexture(texture2D,0);gl.disable(0x0C11);gl.disable(0x0BE2);
    if(okay){
        // This is the sole write to the pending gameplay color target. A blit
        // failure aborts the existing whole-frame transaction before publish.
        gl.bindFramebuffer(readFramebuffer,h.target);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        okay=check("copy authentic original PauseDerived into pending gameplay image");
    }
    gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    // This native presentation owns opaque alpha. Source Ad stays private and
    // never becomes the input to a later gameplay effect or retained UI frame.
    gl.colorMask(0,0,0,1);gl.clearColor(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
    gl.depthMask(1);gl.clearDepth(1);gl.depthFunc(0x0201);gl.useProgram(shader->program());
    drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after original PauseDerived pass");
    if(okay&&restored){diagnostic->originalPauseDerivedQuads=h.quads;diagnostic->originalPauseDerivedLeaves=h.packetCount;}
    return okay&&restored;
}
bool deletePauseDerivedNames(VehicleWindowDiagnostic50DS& d) noexcept {
    if(!pauseDerived)return true;auto& h=*pauseDerived;h.ready=h.frame=false;
    MaterialTextureGlDiagnostic50EB td;
    if(!h.textures.close(td)){if(td.cleanupIncomplete){d.cleanupIncomplete=true;return false;}
        if(!d.cleanupGlError)d.cleanupGlError=td.glError?td.glError:td.cleanupGlError;}
    if(!deleteName(h.target,gl.deleteFramebuffers,d)||!deleteName(h.color,gl.deleteTextures,d)||
        !deleteName(h.depth,gl.deleteRenderbuffers,d))return false;
    // Whole-renderer close or failed-bind cleanup has completed glFinish.
    // Release the strong canonical owner only after acknowledged name retirement.
    pauseDerived.reset();return true;
}
