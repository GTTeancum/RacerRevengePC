// Renderer-private native four-sample UI lowering. Source raster stays 640x448;
// presentation resize never resamples the in-progress per-sample destination.
// No GS edge/sample-position parity is asserted. No AA1-to-AA0 fallback.
    // Requested-format payload: 640*448*(72 color bytes +27 depth bytes)
    // =28,385,280 bytes. Budget depth24 as32 storage:30,965,760 bytes.
    // Driver tiling/metadata are not observable and are not claimed exact.
    static constexpr std::size_t uiCoverageStorageBound=640u*448u*108u;
    static_assert(uiCoverageStorageBound==30965760u);
    struct UiCoverageStorage {
        U scene=0,color=0,depth=0,snapshot=0,snapshotColor=0;
        U raster[2]{},payload[2]{},rasterDepth[2]{}; // center AA0, four-sample AA1
        U program=0,vertex=0,fragment=0,array=0;
        I mode=-1,blend=-1;
        bool ready=false,attempted=false,coherentFetch=false;
        bool owns()const noexcept {return scene||color||depth||snapshot||snapshotColor||
            raster[0]||raster[1]||payload[0]||payload[1]||rasterDepth[0]||rasterDepth[1]||program||vertex||fragment||array;}
    } uiCoverage;
    bool deleteUiCoverage(VehicleWindowDiagnostic50DS& d)noexcept {
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
    bool uiCoverageDepthTexture(U& name)noexcept {
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
    using UiCoverageClock=std::chrono::steady_clock;
    static std::uint64_t uiCoverageNs(UiCoverageClock::time_point begin)noexcept {
        return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(UiCoverageClock::now()-begin).count());
    }
    struct UiCoverageTimer {std::uint64_t& elapsed;UiCoverageClock::time_point begin=UiCoverageClock::now();
        ~UiCoverageTimer(){elapsed=uiCoverageNs(begin);}};
    std::uint64_t uiCoverageAllocationNs=0,uiCoverageProgramNs=0;
    unsigned uiCoverageSetupReports=0,uiCoverageDrawReports=0,uiCoverageCompletionReports=0,uiCoveragePublishReports=0;
    unsigned uiCoverageSlowReports=0,uiCoverageSlowTotalReports=0;
    bool uiCoveragePromotionReported=false;
    bool uiCoverageSourceDiagnostic=false; // Observation only, never publication authority.
    bool prepareUiCoverageRaw()noexcept {
        if(uiCoverage.ready)return true;
        // Failed private allocations remain owned for explicit renderer cleanup.
        if(uiCoverage.attempted)return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"UI coverage preparation previously failed");
        uiCoverage.attempted=true;
        I major=0,samples=0,colorSamples=0,depthSamples=0,units=0,limit=0;
        gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x8D57,&samples);gl.getIntegerv(0x910E,&colorSamples);
        gl.getIntegerv(0x910F,&depthSamples);gl.getIntegerv(0x8872,&units);gl.getIntegerv(0x0D33,&limit);
        if(!check("query demand-bound UI four-sample limits"))return false;
        if(major<4||samples<4||colorSamples<4||depthSamples<4||units<5||limit<640)
            return fail(*diagnostic,VehicleWindowError50DS::limits,"UI AA1 requires GL4, five texture units and exact four samples; no fallback");
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
    bool prepareUiCoverage(const char* phase)noexcept {
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
    void unbindUiCoverageInputs()noexcept {
        for(U i=0;i<5;++i){gl.activeTexture(textureUnit0+i);gl.bindTexture(texture2D,0);gl.bindTexture(coverageTexture,0);if(gl.bindSampler)gl.bindSampler(i,0);}
        gl.activeTexture(textureUnit0);
    }
    bool beginUiCoverageSpan(bool aa1,I sourceBlend)noexcept {
        unbindUiCoverageInputs();
        gl.disable(0x0BE2);gl.enable(0x0C11);
        gl.disable(0x809E);gl.disable(0x809F);gl.disable(0x80A0);gl.disable(0x8E51);gl.disable(0x8C36);
        gl.colorMask(1,1,1,1);gl.enable(0x0B71);
        gl.useProgram(uiCoverage.program);
        gl.uniform1i(uiCoverage.mode,aa1?1:0);gl.uniform1i(uiCoverage.blend,sourceBlend);
        gl.useProgram(uiProgram);gl.uniform1i(uiCoveragePayloadUniform,1);
        return check("begin ordered UI coverage span");
    }
    bool endUiCoverageSpan(U image,U depth,bool write)noexcept {
        // Runs even after bounds/raster/composition failure. Restore the UI VAO
        // before the outer submission restores its immutable element bank.
        unbindUiCoverageInputs();gl.disable(0x0C11);gl.disable(0x809D);
        gl.useProgram(uiProgram);gl.uniform1i(uiCoveragePayloadUniform,0);
        gl.bindTexture(texture2D,image);drawGl.bindVertexArray(uiVertexArray);
        gl.bindFramebuffer(framebuffer,uiCoverage.scene);
        gl.enable(0x0B71);gl.depthFunc(depth);gl.depthMask(write?1:0);
        return check("restore ordered UI state after coverage span");
    }
    bool drawUiCoverageRange(bool aa1,U index,U indexCount,U image,U depth,bool write,I sourceBlend,
        bool opaqueBatch,const OriginalUiCoverageBounds50KG& region,U sourceSampler=0)noexcept {
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
    bool promoteUiCoveragePrefix()noexcept {
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
    bool resolveUiCoverage()noexcept {
        unbindUiCoverageInputs();gl.disable(0x0C11);gl.disable(0x809D);
        gl.bindFramebuffer(readFramebuffer,uiCoverage.scene);gl.bindFramebuffer(drawFramebuffer,uiSourceTarget);
        gl.blitFramebuffer(0,0,640,448,0,0,640,448,colorBufferBit,0x2600);
        gl.bindFramebuffer(framebuffer,uiSourceTarget);return check("resolve completed UI sample domain once");
    }
