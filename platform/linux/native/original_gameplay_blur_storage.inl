// Included only inside VehicleWindowGl50DS::Storage. Native renderer resources,
// not source gameplay activation or an independently executable render path.
    U gameplaySceneTarget=0,gameplaySceneColor=0;
    U gameplayHistoryTargets[2]{},gameplayHistoryColors[2]{};
    U gameplayBlurProgram=0,gameplayBlurVertexShader=0,gameplayBlurFragmentShader=0;
    U gameplayBlurVertexArray=0;
    I gameplayHistoryUniform=-1,gameplayCurrentUniform=-1,gameplayOriginalSizeUniform=-1;
    I gameplayNativeSizeUniform=-1,gameplayHistorySizeUniform=-1;
    I gameplayViewportUniform=-1,gameplayFixedUniform=-1;
    game::GameplayBlurVideoState gameplayBlurVideo{};
    GameplayBlurFrame50DS frameGameplayBlur{};
    bool frameGameplayBlurEnabled=false;
    std::uint8_t gameplayHistoryPublished=0;
    I gameplayHistoryWidth=0,gameplayHistoryHeight=0;
    I gameplayStagedWidth=0,gameplayStagedHeight=0;
    bool gameplayHistoryValid=false,gameplayHistoryStaged=false;
    std::uint64_t gameplayHistoryGeneration=0;
    // Successful renderer window copy or headless private commit awaiting the
    // explicit display owner below. A private race commit is never history.
    U gameplayAwaitingTarget=0;
    I gameplayAwaitingWidth=0,gameplayAwaitingHeight=0;

    bool hasGameplayBlurResources() const noexcept {
        return gameplaySceneTarget || gameplaySceneColor || gameplayHistoryTargets[0] ||
            gameplayHistoryTargets[1] || gameplayHistoryColors[0] || gameplayHistoryColors[1] ||
            gameplayBlurProgram || gameplayBlurVertexShader || gameplayBlurFragmentShader ||
            gameplayBlurVertexArray;
    }
    bool createGameplayBlurTarget(U& target,U& color) noexcept {
        gl.genFramebuffers(1,&target);
        if(!check("allocate gameplay history framebuffer") || !target)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero gameplay history framebuffer");
        gl.genTextures(1,&color);
        if(!check("allocate gameplay history texture") || !color)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero gameplay history texture");
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,color);
        if(gl.bindSampler)gl.bindSampler(0,0);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        // A null pointer must allocate storage, never name an unpack-PBO offset.
        drawGl.bindBuffer(0x88EC,0);
        gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
        if(!check("allocate cold maximum-size gameplay RGBA8 storage"))return false;
        I w=0,h=0,format=0;
        gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
        gl.getTexLevelParameteriv(texture2D,0,0x1001,&h);
        gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
        if(!check("verify gameplay history texture storage"))return false;
        if(w!=maximumWidth || h!=maximumHeight || format!=static_cast<I>(rgba8))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"gameplay history texture storage mismatch");
        gl.bindFramebuffer(framebuffer,target);
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,color,0);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);
        if(!check("verify gameplay history framebuffer"))return false;
        if(status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete gameplay history framebuffer");}
        return true;
    }
    bool createGameplayBlur() noexcept {
        if(!request.enableOriginalGameplayBlur)return true;
        I textureLimit=0,units=0;
        gl.getIntegerv(0x0D33,&textureLimit);gl.getIntegerv(0x8872,&units);
        if(!check("query gameplay blur native texture limits"))return false;
        if(textureLimit<maximumWidth || textureLimit<maximumHeight || units<2)
            return fail(*diagnostic,VehicleWindowError50DS::limits,"gameplay blur needs two fragment units and bounded texture dimensions");
        if(!createGameplayBlurTarget(gameplaySceneTarget,gameplaySceneColor))return false;
        for(U b=0;b<2;++b)
            if(!createGameplayBlurTarget(gameplayHistoryTargets[b],gameplayHistoryColors[b]))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::vertex;
        if(!compileTrackShader(0x8B31,native::gameplayBlurNativeVertex,gameplayBlurVertexShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::fragment;
        if(!compileTrackShader(0x8B30,native::gameplayBlurNativeFragment,gameplayBlurFragmentShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::link;
        gameplayBlurProgram=shaderGl.createProgram();
        if(!check("create gameplay blur program") || !gameplayBlurProgram)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"zero gameplay blur program");
        shaderGl.attachShader(gameplayBlurProgram,gameplayBlurVertexShader);
        shaderGl.attachShader(gameplayBlurProgram,gameplayBlurFragmentShader);
        shaderGl.bindFragDataLocation(gameplayBlurProgram,vehicleGlColorOutput50CZ,"composedColor");
        shaderGl.linkProgram(gameplayBlurProgram);
        I linked=0;shaderGl.getProgramiv(gameplayBlurProgram,0x8B82,&linked);
        if(!check("link gameplay blur program"))return false;
        if(!linked){
            shaderGl.getProgramInfoLog(gameplayBlurProgram,static_cast<I>(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
            check("read gameplay blur program log");
            return fail(*diagnostic,VehicleWindowError50DS::shader,"gameplay blur program link failed");
        }
        gameplayHistoryUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"historyImage");
        gameplayCurrentUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"currentImage");
        gameplayOriginalSizeUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"originalSize");
        gameplayNativeSizeUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"nativeViewportSize");
        gameplayHistorySizeUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"historyViewportSize");
        gameplayViewportUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"originalViewport");
        gameplayFixedUniform=shaderGl.getUniformLocation(gameplayBlurProgram,"fixedCoefficient");
        if(!check("locate gameplay blur uniforms"))return false;
        if(gameplayHistoryUniform<0 || gameplayCurrentUniform<0 || gameplayOriginalSizeUniform<0 ||
            gameplayNativeSizeUniform<0 || gameplayHistorySizeUniform<0 || gameplayViewportUniform<0 || gameplayFixedUniform<0)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"missing gameplay blur uniform");
        gl.genVertexArrays(1,&gameplayBlurVertexArray);
        if(!check("allocate gameplay blur vertex array") || !gameplayBlurVertexArray)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero gameplay blur vertex array");
        gl.bindTexture(texture2D,0);
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::none;
        return check("finish cold gameplay blur resources");
    }
    bool validateGameplayBlurFrame(const GameplayBlurFrame50DS* input,VehicleWindowDiagnostic50DS& d) const noexcept {
        if(!input)return true;
        if(!request.enableOriginalGameplayBlur || !input->pending || input->logicalView!=0 || input->physicalView!=0)
            return fail(d,VehicleWindowError50DS::frame,"gameplay blur requires real producer queue and admitted single view0");
        const auto& r=input->cameraRectangle;
        const native::GameplayBlurNativePassParameters p{input->originalWidth,input->originalHeight,
            r.left,r.top,r.right,r.bottom,0};
        if(!native::admitGameplayBlurNativePass(p))
            return fail(d,VehicleWindowError50DS::frame,"gameplay blur original camera rectangle unavailable");
        // This join renders one original view. Never silently consume or discard
        // another view's real queue while pretending split-screen is supported.
        for(U i=1;i<4;++i)if(input->pending->requested[i] || gameplayBlurVideo.view[i].fixed)
            return fail(d,VehicleWindowError50DS::frame,"gameplay blur split-view owner is not joined");
        return true;
    }
    bool restoreGameplayBlurState(U target) noexcept {
        gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);
        if(gl.bindSampler)gl.bindSampler(1,0);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,0);
        if(gl.bindSampler)gl.bindSampler(0,0);
        drawGl.bindVertexArray(0);
        gl.bindFramebuffer(framebuffer,target);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        gl.viewport(0,0,width,height);gl.colorMask(1,1,1,1);
        resetPodState();
        return check("restore native scene state after gameplay blur without clearing depth");
    }
    bool drawGameplayBlur(std::uint8_t bank) noexcept {
        if(!frameGameplayBlurEnabled)return true;
        // Original manager consumption is a Render3D event. It does not advance
        // on simulation ticks, a readback, a pause menu or display fade samples.
        const auto result=game::consumeGameplayBlur(*frameGameplayBlur.pending,0,0,
            frameGameplayBlur.cameraRectangle,gameplayBlurVideo);
        if(result!=game::GameplayBlurConsumeResult::consumed && result!=game::GameplayBlurConsumeResult::no_request)
            return fail(*diagnostic,VehicleWindowError50DS::frame,"original gameplay blur queue consumption rejected");
        diagnostic->gameplayBlurRequestConsumed=result==game::GameplayBlurConsumeResult::consumed;
        const auto& video=gameplayBlurVideo.view[0];
        diagnostic->gameplayBlurFixed=video.fixed;
        if(!video.fixed)return true;
        if(!gameplayHistoryValid)
            return fail(*diagnostic,VehicleWindowError50DS::noImage,"positive gameplay blur has no actual completed display history");
        const auto& r=video.rectangle;
        const native::GameplayBlurNativePassParameters p{frameGameplayBlur.originalWidth,frameGameplayBlur.originalHeight,
            r.left,r.top,r.right,r.bottom,video.fixed};
        if(!native::admitGameplayBlurNativePass(p))
            return fail(*diagnostic,VehicleWindowError50DS::frame,"persistent gameplay blur rectangle exceeds current original video extent");
        // Renderbuffer scene -> distinct immutable texture. The output remains
        // the inactive native race FBO; neither sampled image is attached there.
        gl.disable(0x0C11);gl.disable(0x8DB9);
        gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,gameplaySceneTarget);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        bool accepted=check("snapshot completed Render3D scene for gameplay blur");
        if(accepted){
            gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
            if(gl.clipControl)gl.clipControl(0x8CA1,0x935E);
            gl.viewport(0,0,width,height);
            constexpr U disabled[]={0x0BE2,0x0B71,0x0B90,0x0C11,0x0B44,0x8C89,0x0BF2,0x0BD0,0x8DB9,
                0x8037,0x809D,0x80A0,0x809E};
            for(U capability:disabled)gl.disable(capability);
            for(I i=0;i<clipDistances;++i)gl.disable(0x3000+static_cast<U>(i));
            gl.depthMask(0);gl.colorMask(1,1,1,1);gl.polygonMode(0x0408,0x1B02);
            gl.useProgram(gameplayBlurProgram);drawGl.bindVertexArray(gameplayBlurVertexArray);
            gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,gameplayHistoryColors[gameplayHistoryPublished]);
            if(gl.bindSampler)gl.bindSampler(0,0);
            gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,gameplaySceneColor);
            if(gl.bindSampler)gl.bindSampler(1,0);
            gl.uniform1i(gameplayHistoryUniform,0);gl.uniform1i(gameplayCurrentUniform,1);
            gl.uniform2i(gameplayOriginalSizeUniform,p.originalWidth,p.originalHeight);
            gl.uniform2i(gameplayNativeSizeUniform,width,height);
            gl.uniform2i(gameplayHistorySizeUniform,gameplayHistoryWidth,gameplayHistoryHeight);
            gl.uniform4i(gameplayViewportUniform,p.left,p.top,p.right,p.bottom);
            gl.uniform1i(gameplayFixedUniform,p.fixed);
            accepted=check("bind native original gameplay blur inputs");
            if(accepted){gl.drawArrays(0x0004,0,3);accepted=check("draw original displayed-history gameplay blur");}
            if(accepted)++diagnostic->gameplayBlurDraws;
        }
        const bool restored=restoreGameplayBlurState(targets[bank]);
        return accepted && restored;
    }
    void noteGameplayDisplayPending(U target,I w,I h) noexcept {
        if(!request.enableOriginalGameplayBlur)return;
        gameplayAwaitingTarget=target;gameplayAwaitingWidth=w;gameplayAwaitingHeight=h;
    }
    bool stageGameplayDisplayedHistory(U target,I w,I h) noexcept {
        if(!request.enableOriginalGameplayBlur)return true;
        gameplayHistoryStaged=false;
        if(!target || target==gameplayHistoryTargets[0] || target==gameplayHistoryTargets[1] ||
            w<=0 || h<=0 || w>maximumWidth || h>maximumHeight ||
            gameplayHistoryGeneration==std::numeric_limits<std::uint64_t>::max())
            return fail(*diagnostic,VehicleWindowError50DS::frame,"actual display history source, extent or generation unavailable");
        const auto pending=static_cast<std::uint8_t>(gameplayHistoryPublished^1u);
        gl.disable(0x0C11);gl.disable(0x8DB9);
        gl.bindFramebuffer(readFramebuffer,target);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,gameplayHistoryTargets[pending]);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,w,h,0,0,w,h,colorBufferBit,0x2600);
        if(!check("stage final actual display into inactive gameplay history"))return false;
        gameplayStagedWidth=w;gameplayStagedHeight=h;gameplayHistoryStaged=true;
        return true;
    }
    void acceptGameplayDisplayedHistory() noexcept {
        if(!request.enableOriginalGameplayBlur)return;
        // Caller has completed staging GPU work and the actual swap, or the
        // explicit headless equivalent. Nothing here can fail after that event.
        gameplayHistoryPublished^=1u;
        gameplayHistoryWidth=gameplayStagedWidth;gameplayHistoryHeight=gameplayStagedHeight;
        gameplayHistoryValid=true;gameplayHistoryStaged=false;++gameplayHistoryGeneration;
        gameplayAwaitingTarget=0;gameplayAwaitingWidth=gameplayAwaitingHeight=0;
    }
    bool deleteGameplayBlurNames(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!deleteShaderName(gameplayBlurProgram,true,d) ||
            !deleteShaderName(gameplayBlurVertexShader,false,d) ||
            !deleteShaderName(gameplayBlurFragmentShader,false,d) ||
            !deleteName(gameplayBlurVertexArray,gl.deleteVertexArrays,d))return false;
        for(U b=0;b<2;++b)
            if(!deleteName(gameplayHistoryTargets[b],gl.deleteFramebuffers,d) ||
                !deleteName(gameplayHistoryColors[b],gl.deleteTextures,d))return false;
        return deleteName(gameplaySceneTarget,gl.deleteFramebuffers,d) &&
            deleteName(gameplaySceneColor,gl.deleteTextures,d);
    }
