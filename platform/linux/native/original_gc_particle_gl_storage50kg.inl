// Included only in VehicleWindowGl50DS::Storage. Lower-level native GPU reuse;
// no authenticated Main/Options owner or UI presentation target participates.
std::uint64_t gcParticlePublication=0;
std::uint8_t gcParticleMaximumAlpha=0;
const game::OriginalGcParticleDraw50KG* frameGcParticles=nullptr;
I gcParticleModeUniform=-1;
// Distinct RC source authority; the vertex/run/GPU kernel below is shared.
std::uint64_t rcParticlePublication=0;
std::uint8_t rcParticleMaximumAlpha=0;
bool frameRcParticles=false;
bool validateRcParticleBank(const VehicleWindowRequest50DS& input) noexcept {
    if(!input.track.rcParticleWindow)return true;
    const auto& window=*input.track.rcParticleWindow;
    if(input.gcParticleCpuTextures||input.gcParticleTextures||input.gcParticleMaterial||
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

bool validateGcParticleBank(const VehicleWindowRequest50DS& input) noexcept {
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
// Identity-free geometry validation reused only after each typed owner gate.
// Existing GC conditions and diagnostics remain byte-for-byte in this body,
// with only its already-selected bank presence/alpha bound parameterized.
bool validateParticleDraw(const game::OriginalGcParticleDraw50KG* draw,
    const VehicleWindowFrame50DS& frame,bool bankPresent,std::uint8_t maximumAlpha,
    VehicleWindowDiagnostic50DS& d) const noexcept {
    if(!bankPresent||!uiProgram||!draw->generation||!draw->cameraSceneKey||!draw->cameraSessionGeneration||
       draw->sourceParticleCount>game::originalGcParticleCapacity50KG||draw->quadCount>draw->sourceParticleCount||
       draw->runCount>draw->quadCount||!draw->vertices||!draw->fogFactors||!draw->runs||!frame.worldViewProjection)
        return fail(d,VehicleWindowError50DS::frame,"GC particle frame lacks actual manager/camera/bank publication");
    //Bind particles to the exact same current49F matrix as opaque gameplay.
    //50DX changes only packing and Z: this duplicate native arithmetic is a
    //preflight comparison, not a second camera or guessed view reconstruction.
    for(U row=0;row<4;++row)for(U col=0;col<4;++col){
        float expected=draw->sourceCombined[row*4+col];
        if(col==2){volatile float twice=2.f*expected;volatile float z=twice-draw->sourceCombined[row*4+3];expected=z;}
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
bool validateGcParticleFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) const noexcept {
    const auto* draw=frame.gcParticles;if(!draw)return true; //Resident bank while genuine UI owns presentation.
    return validateParticleDraw(draw,frame,request.gcParticleTextures!=nullptr,gcParticleMaximumAlpha,d);
}
bool validateRcParticleFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) const noexcept {
    if(!rcParticleWindow&&!frame.rcParticles)return true;
    if(!rcParticleWindow||!frame.rcParticles||frame.gcParticles||!rcParticlePendingAttempt)
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
bool drawGcParticles() noexcept {
    if(!frameGcParticles||!frameGcParticles->quadCount)return true;
    const auto& draw=*frameGcParticles;
    //Same pending gameplay color AND depth targets remain bound. No clear,
    //framebuffer copy or postprocess fog overlay. The independent source-A
    //store below samples the same authentic atlas after each RGB run.
    resetNativeGcLighting();gl.useProgram(uiProgram);
    gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
    gl.bindTexture(texture2D,frameRcParticles?rcParticleWindow->gpu_.texture(0):request.gcParticleTextures->texture(0));
    constexpr U parameters[]={0x2801,0x2800,0x2802,0x2803,0x813C,0x813D};
    constexpr I expected[]={0x2601,0x2601,0x2901,0x2901,0,0};
    for(U i=0;i<6;++i){I actual=-1;gl.getTexParameteriv(texture2D,parameters[i],&actual);
        if(!check("verify authentic GC particle sampler"))return false;
        if(actual!=expected[i])return fail(*diagnostic,VehicleWindowError50DS::material,"GC particle immutable sampler changed");}
    gl.uniform4f(uiCanvasUniform,2.f/640.f,-2.f/448.f,-1.f,1.f);
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
            if(frameRcParticles)++diagnostic->rcParticleDraws;else ++diagnostic->gcParticleDraws;
            //Actual separate source-A store, not source-over accumulation.
            //Both authenticated states use DATE0 and source (never destination)
            //alpha in ALPHA, so deferred alpha within this run cannot change
            //any RGB fragment. Preserve primitive order, depth and alpha test.
            gl.disable(0x0BE2);gl.colorMask(0,0,0,1);gl.uniform1i(gcParticleModeUniform,3);
            drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(run.quadCount*6),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(run.firstQuad)*6*sizeof(U)),0);
            accepted=check("store authentic GC particle source alpha");
            if(accepted){if(frameRcParticles){++diagnostic->rcParticleAlphaDraws;diagnostic->rcParticleQuads+=run.quadCount;}
                else{++diagnostic->gcParticleAlphaDraws;diagnostic->gcParticleQuads+=run.quadCount;}}
        }
    }
    gl.uniform1i(gcParticleModeUniform,0);gl.disable(0x0BE2);gl.colorMask(1,1,1,1);
    gl.depthMask(1);gl.depthFunc(0x0201);drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after shared GC particles");return accepted&&restored;
}
