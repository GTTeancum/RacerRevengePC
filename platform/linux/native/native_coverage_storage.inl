// Private VehicleWindowGl50DS::Storage members. Demand-gated native coverage;
// no source admission, context creation, quality fallback or GPU receipt here.
    static constexpr U coverageTexture = 0x9100; // TEXTURE_2D_MULTISAMPLE
    bool coverageDemand = false;
    // Per-frame checked bridge submissions, not visible-pixel/GPU-time counts.
    std::uint64_t coveragePairRuns=0,coveragePairTriangles=0,coveragePairCoalescedTriangles=0;
    std::uint64_t coveragePairEmptyTriangles=0,coveragePairFullSingletons=0;
    struct CoverageExtent {
        U scene[2]{}, color[2]{}, depth[2]{};
        U snapshot=0, snapshotColor=0, raster=0, payload=0, rasterDepth=0;
        I width=0,height=0;
        bool owns() const noexcept {
            return scene[0]||scene[1]||color[0]||color[1]||depth[0]||depth[1]||
                snapshot||snapshotColor||raster||payload||rasterDepth;
        }
    } coverageLive, coverageSpare;
    U coverageProgram=0,coverageVertex=0,coverageFragment=0,coverageArray=0;
    I coverageRaCompositionUniform=-1;
    I coveragePayloadUniform=-1,coverageDepthUniform=-1,coverageDestinationUniform=-1;
    bool hasCoverageResources() const noexcept {
        return coverageLive.owns()||coverageSpare.owns()||coverageProgram||
            coverageVertex||coverageFragment||coverageArray;
    }
    bool checkCoverageSupport(I major,I) noexcept {
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
        return true;
    }
    // This helper changes coverage alone, never a depth/blend tuple. Call on
    // every span, after any source-UV callback, independently of local caches.
    void bindTrackCoverage(NativeTrackCoverage50DS policy) noexcept {
        if(!coverageDemand)return;
        gl.disable(0x809E);gl.disable(0x809F);gl.disable(0x80A0);
        gl.disable(0x8E51);gl.disable(0x8C36);
        if(policy==NativeTrackCoverage50DS::fourSampleAa1)gl.enable(0x809D);
        else gl.disable(0x809D);
    }
    void coverageAa0() noexcept { bindTrackCoverage(NativeTrackCoverage50DS::aa0); }
    bool deleteCoverageExtent(CoverageExtent& e,VehicleWindowDiagnostic50DS& d) noexcept {
        for(U b=0;b<2;++b)
            if(!deleteName(e.scene[b],gl.deleteFramebuffers,d)||
               !deleteName(e.color[b],gl.deleteTextures,d)||
               !deleteName(e.depth[b],gl.deleteTextures,d))return false;
        if(!deleteName(e.snapshot,gl.deleteFramebuffers,d)||!deleteName(e.snapshotColor,gl.deleteTextures,d)||
           !deleteName(e.raster,gl.deleteFramebuffers,d)||!deleteName(e.payload,gl.deleteTextures,d)||
           !deleteName(e.rasterDepth,gl.deleteTextures,d))return false;
        e.width=e.height=0;return true;
    }
    bool deleteCoverageResources(VehicleWindowDiagnostic50DS& d) noexcept {
        return deleteCoverageExtent(coverageSpare,d)&&deleteCoverageExtent(coverageLive,d)&&
            deleteShaderName(coverageProgram,true,d)&&deleteShaderName(coverageVertex,false,d)&&
            deleteShaderName(coverageFragment,false,d)&&deleteName(coverageArray,gl.deleteVertexArrays,d);
    }
    bool coverageTextureStorage(U& name,U format,I w,I h,bool multi,U attachment) noexcept {
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
    bool coverageFramebuffer(U& name) noexcept {
        gl.genFramebuffers(1,&name);
        if(!check("allocate native coverage framebuffer name")||!name)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero native coverage framebuffer");
        gl.bindFramebuffer(framebuffer,name);return check("bind owned native coverage framebuffer");
    }
    bool coverageComplete() noexcept {
        const U status=gl.checkFramebufferStatus(framebuffer);
        if(!check("verify native coverage framebuffer completeness"))return false;
        if(status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete native coverage framebuffer");}
        return true;
    }
    bool createCoverageExtent(CoverageExtent& e) noexcept {
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
        if(hasSpecularPairs50LA || request.raStaticWorld){
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
    bool prepareCoverageExtent() noexcept {
        if(!coverageDemand)return true;
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
    bool createCoverageProgram() noexcept {
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
    vec4 destinationByte=floor(clamp(texelFetch(destinationImage,pixel,gl_SampleID)*255.0,0.0,255.0)+0.5);
    sourceByte.rgb=raComposition!=0 ?
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
        return true;
    }
    bool drawCoveragePairTriangle(U bank,U first,U image,const std::array<I,4>& region,U sourceArray,U triangleCount) noexcept {
        // A proven empty native clip region cannot touch any destination
        // sample. Preserve source group order/accounting without GPU work.
        if(region[2]<=region[0]||region[3]<=region[1])return true;
        coverageAa0();gl.disable(0x8DB9);gl.disable(0x0BE2);
        if(request.raStaticWorld){
            gl.scissor(region[0],region[1],region[2]-region[0],region[3]-region[1]);gl.enable(0x0C11);
        }else gl.disable(0x0C11);
        gl.bindFramebuffer(framebuffer,coverageLive.raster);
        // These private FBOs retain createCoverageExtent's selectors:
        // raster draws to attachments0/1; scene/snapshot draw/read0.
        // Binding an FBO does not reset its selectors. No later owner changes
        // these values, so do not reissue eight selector calls per run.
        gl.viewport(0,0,width,height);
        gl.colorMask(1,1,1,1);gl.disable(0x0B71);gl.depthMask(0);
        const float invalid[]={0.f,0.f,0.f,-1.f};gl.clearBufferfv(0x1800,0,invalid);
        gl.useProgram(shader->program());gl.uniform1i(shader->uniforms().sourcePairPass50LA,3);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,image);
        bool accepted=check("bind AA0 original triangle payload target");
        if(accepted){
            drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(triangleCount*3),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(first)*sizeof(U)),0);
            accepted=check("raster original AA0 map-2 payload and center depth");
        }
        if(accepted){
            gl.bindFramebuffer(readFramebuffer,coverageLive.scene[bank]);
            gl.bindFramebuffer(drawFramebuffer,coverageLive.snapshot);
            gl.blitFramebuffer(region[0],region[1],region[2],region[3],
                region[0],region[1],region[2],region[3],colorBufferBit,0x2600);
            accepted=check("copy per-sample destination before ordered map-2 triangle");
        }
        if(accepted){
            gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);
            gl.enable(0x809D);gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(0);
            gl.useProgram(coverageProgram);gl.uniform1i(coverageRaCompositionUniform,0);drawGl.bindVertexArray(coverageArray);
            gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);gl.bindTexture(texture2D,coverageLive.payload);
            gl.activeTexture(textureUnit0+1);if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,coverageLive.rasterDepth);
            gl.activeTexture(textureUnit0+2);if(gl.bindSampler)gl.bindSampler(2,0);gl.bindTexture(coverageTexture,coverageLive.snapshotColor);
            gl.uniform1i(coveragePayloadUniform,0);gl.uniform1i(coverageDepthUniform,1);gl.uniform1i(coverageDestinationUniform,2);
            accepted=check("bind sample-specific original destination-alpha composition");
            if(accepted){gl.drawArrays(0x0004,0,3);accepted=check("compose ordered map-2 triangle per destination sample");}
        }
        // Restore even on rejection. The caller still owns group/span accounting
        // and aborts before publication. No sampled texture remains attached.
        gl.activeTexture(textureUnit0+2);gl.bindTexture(coverageTexture,0);
        gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,image);
        gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);
        gl.useProgram(shader->program());gl.uniform1i(shader->uniforms().sourcePairPass50LA,2);
        drawGl.bindVertexArray(sourceArray);
        coverageAa0();gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(0);
        gl.disable(0x0C11);
        const bool restored=check("restore source-pair state after native coverage bridge");
        return accepted&&restored;
    }
    bool drawCoveragePairGroup(U bank,U slot,U component,U first,U count,U image) noexcept {
        // Never cross the original base/map-2 MSCNT group boundary. A run
        // retains contiguous EBO order and admits only pairwise-disjoint
        // conservative pixel rectangles. Thus no sample can consume another
        // run member's color/alpha or compete with its depth. The unchanged
        // byte shader and per-sample depth comparison can share one snapshot.
        constexpr U maximumRun=14;
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
        while(first<end){
            const auto initial=pendingFirst==first?pending:racerTriangleRasterRegion(slot,component,first);
            if(initial[2]<=initial[0]||initial[3]<=initial[1]){++coveragePairEmptyTriangles;first+=3;continue;}
            std::array<std::array<I,4>,maximumRun> regions{};
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
                if(next[2]<=next[0]||next[3]<=next[1]||fullRegion(next))break;
                bool disjoint=true;
                for(U i=0;i<run;++i){const auto& old=regions[i];
                    if(next[0]<old[2]&&old[0]<next[2]&&next[1]<old[3]&&old[1]<next[3]){
                        disjoint=false;break;
                    }
                }
                if(!disjoint)break;
                auto candidate=united;
                for(U axis=0;axis<2;++axis){candidate[axis]=std::min(candidate[axis],next[axis]);
                    candidate[axis+2]=std::max(candidate[axis+2],next[axis+2]);}
                const auto nextArea=summedArea+area(next);
                // Avoid a sparse run expanding scratch/copy/composition work
                // to an arbitrarily large rectangle. Extents are GL-limited.
                if(area(candidate)>nextArea*2u)break;
                regions[run++]=next;united=candidate;summedArea=nextArea;
            }
            // Singleton uses the same bridge and triangle count as before.
            // Full/uncertain clip fallback always remains its own run.
            if(!sourceArrayRetained){
                gl.getIntegerv(0x85B5,&sourceArray);
                if(!check("retain source-pair vertex array for coverage group"))return false;
                sourceArrayRetained=true;
            }
            if(!drawCoveragePairTriangle(bank,first,image,united,static_cast<U>(sourceArray),run))return false;
            ++coveragePairRuns;coveragePairTriangles+=run;
            if(run>1)coveragePairCoalescedTriangles+=run;
            if(fullRegion(initial))++coveragePairFullSingletons;
            first+=run*3;
        }
        return true;
    }
    bool resolveCoverage(U bank) noexcept {
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
