// Included inside VehicleWindowGl50DS::Storage after original_position_stats_render.inl.
// Explicit native-host scheduling; no retail HUD ancestry or GPU residency claim.
struct Damage {
    static constexpr U maximumQuads=game::OriginalDamageNativeDraw50LA::maximumQuads;
    static constexpr U maximumPackets=game::OriginalDamageNativeDraw50LA::maximumPackets;
    static_assert(maximumQuads<=originalUiMaximumQuads50KG,"shared UI buffers must cover Damage");
    std::shared_ptr<const game::OriginalDamage50LA> owner;
    const game::OriginalCameraViewportFields* viewport=nullptr;
    std::uint64_t scene=0,generation=0,serial=0,publication=0,cameraPublication=0;
    MaterialTextureGl50EB textures;
    std::array<MaterialTextureSampler50EB,1> samplers{};
    std::array<assets::OriginalUiNativeMaterial50KG,1> policies{};
    std::array<U,1> images{};
    std::array<assets::Vertex48B,maximumQuads*4> vertices{};
    std::array<game::OriginalDamageNativePacket50LA,maximumPackets> packets{};
    U target=0,color=0,depth=0,quads=0,packetCount=0;
    bool ready=false,frame=false;
};
std::unique_ptr<Damage> damage;
bool damagePreparing=false,damageUiCreationFailed=false;
bool hasDamageResources()const noexcept {
    return damage&&(damage->target||damage->color||damage->depth||damage->textures.ownsResources());
}
bool prepareDamageResources(std::shared_ptr<const game::OriginalDamage50LA> owner) noexcept {
    if(damage||frontendOnly||active||frameSet||!owner||!owner->nativeCpuResourcesPrepared())
        return fail(*diagnostic,VehicleWindowError50DS::request,"fresh original Damage cold owner required");
    damage.reset(new(std::nothrow) Damage);
    if(!damage)return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate original Damage storage");
    auto& h=*damage;h.owner=std::move(owner);
    const auto* images=h.owner->nativePixels();const auto* bank=images?images->textures():nullptr;
    if(!bank||bank->textureCount()!=1)
        return fail(*diagnostic,VehicleWindowError50DS::assets,"original Damage single-atlas bridge required");
    assets::OriginalUiMaterialDiagnostic50KG md;
    for(U i=0;i<1;++i){
        const auto* m=h.owner->nativeMaterial(i);const U image=h.owner->nativeImage(i);
        const auto* decoded=images->decodedTexture(image);
        if(image!=0||!m||m->passes.size()!=1||!decoded||
           !assets::prepareOriginalUiNativeMaterial50KG(m->passes[0],*decoded,h.policies[i],md)||
           h.policies[i].blend!=assets::OriginalUiMaterialBlend50KG::sourceAlpha||
           !h.policies[i].requiresSeparateAlphaStore||
           !hudSampler(h.policies[i],h.samplers[image]))
            return fail(*diagnostic,VehicleWindowError50DS::material,"unsupported original Damage source raster");
        // The owner proves actual ScreenMT fallback, canonical FINAL leaf,
        // and the dmgradar PST association. No invented PSM/group identity.
        h.images[i]=image;
    }
    // A texture-feedback path is needed only for this native private target.
    // Do not change the already compiled UI program's coherent-fetch variant.
    if(!uiSourceCoherentFramebufferFetch){
        I major=0,minor=0,units=0,count=0;gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);
        gl.getIntegerv(0x8872,&units);bool barrier=major>4||(major==4&&minor>=5);
        if(!load(gl.getStringi,"glGetStringi")||!check("query original Damage source-alpha capabilities"))return false;
        gl.getIntegerv(0x821D,&count);
        if(!check("query original Damage extensions")||count<0||count>65536||units<2)
            return fail(*diagnostic,VehicleWindowError50DS::context,"bounded original Damage source-alpha support unavailable");
        for(I i=0;i<count;++i){const auto* p=gl.getStringi(0x1F03,U(i));
            if(!check("read original Damage extension")||!p)return false;
            if(!std::strcmp(reinterpret_cast<const char*>(p),"GL_ARB_texture_barrier"))barrier=true;}
        if(!barrier||!load(gl.textureBarrier,"glTextureBarrier"))
            return fail(*diagnostic,VehicleWindowError50DS::context,"original Damage requires GL4.5 or ARB_texture_barrier");
    }
    if(!load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
        !load(gl.scissor,"glScissor")||!load(gl.activeTexture,"glActiveTexture")||!load(gl.bindTexture,"glBindTexture")||
        !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
        !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||!load(gl.framebufferTexture2D,"glFramebufferTexture2D")||
        !load(gl.blitFramebuffer,"glBlitFramebuffer"))return false;
    MaterialTextureGlRequest50EB requestTextures{};requestTextures.context=request.context;
    requestTextures.originalUploadBinding=request.originalUploadBinding;
    requestTextures.isCurrent=request.isCurrent;requestTextures.samplers=h.samplers.data();requestTextures.samplerCount=1;
    requestTextures.maximumGpuBytes=16u*1024u*1024u;const char* missing=nullptr;
    MaterialTextureGlDiagnostic50EB textureDiagnostic;
    if(!loadMaterialTextureFunctions50EE(request.context,request.procAddress,requestTextures.gl,missing)||
        !h.textures.create(requestTextures,*bank,textureDiagnostic))
        return fail(*diagnostic,VehicleWindowError50DS::material,"upload authentic original Damage images");
    h.publication=h.textures.publicationToken();
    if(!uiProgram){
        // A failed late compilation can retain partially created shared names.
        // They remain renderer-owned for close; never reuse them as readiness.
        damageUiCreationFailed=true;damagePreparing=true;
        const bool okay=createOriginalUi();damagePreparing=false;if(!okay)return false;
        damageUiCreationFailed=false;
    }
    I limit=0;gl.getIntegerv(0x0D33,&limit);
    if(!check("query original Damage texture extent")||limit<maximumWidth||limit<maximumHeight)
        return fail(*diagnostic,VehicleWindowError50DS::limits,"original Damage private target exceeds texture limit");
    gl.genFramebuffers(1,&h.target);gl.bindFramebuffer(framebuffer,h.target);
    gl.genTextures(1,&h.color);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,h.color);
    gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
    gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
    gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);drawGl.bindBuffer(0x88EC,0);
    gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
    if(!check("allocate native-size original Damage source target")||!h.target||!h.color)return false;
    I w=0,height=0,format=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
    gl.getTexLevelParameteriv(texture2D,0,0x1001,&height);gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
    if(!check("verify original Damage source storage")||w!=maximumWidth||height!=maximumHeight||format!=I(rgba8))return false;
    gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,h.color,0);
    if(!createAttachment(h.depth,depth24,depthAttachment))return false;
    gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    const U status=gl.checkFramebufferStatus(framebuffer);gl.bindTexture(texture2D,0);gl.bindFramebuffer(framebuffer,0);
    if(!check("complete original Damage private target")||status!=framebufferComplete){diagnostic->framebufferStatus=status;return false;}
    h.ready=true;return true;
}
bool prepareDamageFrame(const game::OriginalDamage50LA* owner) noexcept {
    if(damage){damage->frame=false;damage->quads=damage->packetCount=0;}
    if(!owner)return true;
    if(!damage||!damage->ready||owner!=damage->owner.get()||!owner->nativeCpuResourcesPrepared())return false;
    auto& h=*damage;const auto& source=owner->nativeDraw();const auto* v=source.viewport;
    if(!source.prepared||!source.serial||source.serial<h.serial||!source.cameraPublication||
       !v||!v->prepared||!v->matricesReady||!v->sceneKey||!v->sessionGeneration||
       v->x1E0||v->y1E4||v->width1E8!=640||v->height1EC!=448||
       (h.viewport&&(h.viewport!=v||h.scene!=v->sceneKey||h.generation!=v->sessionGeneration))||
       source.quadCount>Damage::maximumQuads||source.packetCount>Damage::maximumPackets||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    std::array<bool,Damage::maximumQuads> covered{};
    std::int32_t previousLeaf=-1;
    for(U p=0;p<source.packetCount;++p){const auto& packet=source.packets[p];
        if(packet.material!=0||packet.leaf<0||packet.leaf<previousLeaf||!packet.count||
           packet.first>source.quadCount||packet.count>source.quadCount-packet.first)return false;
        previousLeaf=packet.leaf;
        for(U q=packet.first;q<packet.first+packet.count;++q){
            if(covered[q])return false;covered[q]=true;
            const auto& quad=source.quads[q];
            if(quad.depth!=16777214.f)return false;
            for(U i=0;i<4;++i){const auto& in=quad.vertices[i];const auto& c=quad.packetColors[i];
                if(!std::isfinite(in.x)||!std::isfinite(in.y)||!std::isfinite(in.u)||!std::isfinite(in.v)||
                   c[3]>h.policies[packet.material].maximumVertexAlpha)return false;
                auto& out=h.vertices[q*4+i];out={};out.position[0]=in.x;out.position[1]=in.y;out.position[2]=quad.depth;
                out.uv[0]=in.u;out.uv[1]=in.v;for(U lane=0;lane<4;++lane)out.color[lane]=c[lane];
            }
        }
        h.packets[p]=packet;
    }
    for(U q=0;q<source.quadCount;++q)if(!covered[q])return false;
    h.viewport=v;h.scene=v->sceneKey;h.generation=v->sessionGeneration;h.serial=source.serial;h.cameraPublication=source.cameraPublication;
    h.quads=source.quadCount;h.packetCount=source.packetCount;h.frame=h.quads!=0;return true;
}
bool drawDamage(U bank) noexcept {
    if(!damage||!damage->frame)return true;
    auto& h=*damage;
    if(!h.ready||!h.owner||!h.owner->nativeCpuResourcesPrepared()||
       !h.owner->nativeDraw().prepared||h.owner->nativeDraw().serial!=h.serial||
       h.owner->nativeDraw().cameraPublication!=h.cameraPublication||h.owner->nativeDraw().viewport!=h.viewport||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    // Keep the native image at its real drawable resolution. Only this private
    // scratch target receives raw source alpha and a separate UI depth plane.
    gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);
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
    bool okay=check("prepare authentic Damage source pass");

    for(U p=0;okay&&p<h.packetCount;++p){const auto& packet=h.packets[p];
        // The actual policy was admitted as source-alpha, GEQUAL/write.
        // Preserve owner packet order; no synthetic registration/rank sorting.
        gl.depthMask(1);gl.uniform1i(uiSourceBlendUniform,1);
        gl.bindTexture(texture2D,h.textures.texture(h.images[packet.material]));
        if(uiSourceCoherentFramebufferFetch){
            drawGl.drawElementsBaseVertex(0x0004,I(packet.count*6),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(packet.first)*6*sizeof(U)),0);
            okay=check("draw ordered original Damage source leaf");
        }else{
            for(U i=packet.first*6;okay&&i<(packet.first+packet.count)*6;i+=3){gl.textureBarrier();
                drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(i)*sizeof(U)),0);
                okay=check("draw ordered original Damage source triangle");}
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
        okay=check("copy authentic original Damage into pending gameplay image");
    }
    gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    // This native presentation owns opaque alpha. Source Ad stays private and
    // never becomes the input to a later gameplay effect or retained UI frame.
    gl.colorMask(0,0,0,1);gl.clearColor(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
    gl.depthMask(1);gl.clearDepth(1);gl.depthFunc(0x0201);gl.useProgram(shader->program());
    drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after original Damage pass");
    if(okay&&restored){diagnostic->originalDamageQuads=h.quads;diagnostic->originalDamageLeaves=h.packetCount;}
    return okay&&restored;
}
bool deleteDamageNames(VehicleWindowDiagnostic50DS& d) noexcept {
    if(!damage)return true;auto& h=*damage;h.ready=h.frame=false;
    MaterialTextureGlDiagnostic50EB td;
    if(!h.textures.close(td)){if(td.cleanupIncomplete){d.cleanupIncomplete=true;return false;}
        if(!d.cleanupGlError)d.cleanupGlError=td.glError?td.glError:td.cleanupGlError;}
    if(!deleteName(h.target,gl.deleteFramebuffers,d)||!deleteName(h.color,gl.deleteTextures,d)||
        !deleteName(h.depth,gl.deleteRenderbuffers,d))return false;
    // Whole-renderer close or failed-bind cleanup has completed glFinish.
    // Release the strong canonical owner only after acknowledged name retirement.
    damage.reset();return true;
}
