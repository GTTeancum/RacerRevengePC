// VehicleWindowGl50DS::Storage. Native RA submission ownership, not a guest
// command stream. Original world topology, material profiles and byte operands
// remain owned by the actual aggregate. GL sampling/clipping/MSAA are native.
    std::shared_ptr<ra_static_world50LA::Owner> raWorld;
    const VehicleWindowGl50DS* raWorldRenderer=nullptr;
    std::uint64_t raWorldImagePublication=0;
    U raWorldArray=0,raWorldVertices=0,raWorldIndices=0;
    U raWorldVertexShader=0,raWorldFragment[2]{},raWorldProgram[2]{};
    struct RaWorldUniforms { I matrix=-1,image=-1,fog=-1,alpha=-1,payload=-1,destination=-1; } raWorldUniform[2];
    struct RaWorldVertex { float position[3],uv[2],color[4],fog; };
    std::vector<RaWorldVertex> raWorldFrame;
    std::vector<U> raWorldFrameImages;
    ra_static_fog50LA::CameraObservation raWorldCamera;
    bool raWorldFramePrepared=false;
    bool hasRaWorldResources() const noexcept {
        return raWorldArray||raWorldVertices||raWorldIndices||raWorldVertexShader||
            raWorldFragment[0]||raWorldFragment[1]||raWorldProgram[0]||raWorldProgram[1];
    }
    void retireRaWorldPublication() noexcept {
        if(raWorld){raWorld->retireRenderer(raWorldRenderer);raWorld.reset();}
        raWorldRenderer=nullptr;raWorldImagePublication=0;raWorldFramePrepared=false;
    }
    bool raWorldCurrent(VehicleWindowDiagnostic50DS& d) const noexcept {
        if(!request.raStaticWorld)return !raWorld || fail(d,VehicleWindowError50DS::material,"RA aggregate lease missing");
        return (raWorld && request.raStaticWorld->coldStaticWorld()==raWorld.get() &&
            raWorld->rendererPublication_==raWorldRenderer && raWorld->drawPlanCurrent() &&
            request.raStaticWorldTextures && request.raStaticWorldTextures->raStaticWorldOwner()==raWorld.get() &&
            request.raStaticWorldTextures->textureCount()==49 &&
            request.raStaticWorldTextures->validBorrow(request.context,request.isCurrent,raWorldImagePublication)) ||
            fail(d,VehicleWindowError50DS::material,"RA aggregate/world/image publication expired");
    }
    bool deleteRaWorldResources(VehicleWindowDiagnostic50DS& d) noexcept {
        for(U i=0;i<2;++i)if(!deleteShaderName(raWorldProgram[i],true,d)||
            !deleteShaderName(raWorldFragment[i],false,d))return false;
        return deleteShaderName(raWorldVertexShader,false,d)&&
            deleteName(raWorldArray,gl.deleteVertexArrays,d)&&
            deleteName(raWorldVertices,gl.deleteBuffers,d)&&deleteName(raWorldIndices,gl.deleteBuffers,d);
    }
    bool createRaWorld() noexcept {
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
            if(raWorld->rasterInventory_[plan.raster].passFlags&4u)
                return fail(*diagnostic,VehicleWindowError50DS::material,"RA fill-color program is not implemented");
        }
        try {
            raWorldFrame.resize(raWorld->vertices_.size());
            raWorldFrameImages.resize(raWorld->surfaceDrawPlan_.size());
        }catch(...){return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate complete RA frame streams");}
        const std::uint64_t vb=raWorldFrame.size()*sizeof(RaWorldVertex),ib=raWorld->indices_.size()*sizeof(U);
        if(!vb||!ib||vb>std::uint64_t(std::numeric_limits<std::intptr_t>::max())||
           ib>std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
            return fail(*diagnostic,VehicleWindowError50DS::limits,"RA source buffer extent exceeds GL limits");
        gl.genVertexArrays(1,&raWorldArray);drawGl.bindVertexArray(raWorldArray);
        gl.genBuffers(1,&raWorldVertices);drawGl.bindBuffer(arrayBuffer,raWorldVertices);
        gl.bufferData(arrayBuffer,static_cast<std::intptr_t>(vb),nullptr,dynamicDraw);
        gl.genBuffers(1,&raWorldIndices);drawGl.bindBuffer(elementBuffer,raWorldIndices);
        gl.bufferData(elementBuffer,static_cast<std::intptr_t>(ib),raWorld->indices_.data(),staticDraw);
        const I counts[]={3,2,4,1};const std::size_t offsets[]={offsetof(RaWorldVertex,position),offsetof(RaWorldVertex,uv),offsetof(RaWorldVertex,color),offsetof(RaWorldVertex,fog)};
        for(U i=0;i<4;++i){gl.enableVertexAttribArray(i);gl.vertexAttribPointer(i,counts[i],0x1406,0,sizeof(RaWorldVertex),reinterpret_cast<const void*>(offsets[i]));}
        if(!check("upload complete RA world index bank and allocate current source operands")||!raWorldArray||!raWorldVertices||!raWorldIndices)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"RA native geometry allocation failed");
        std::int64_t actual=0;gl.getBufferParameteri64v(arrayBuffer,bufferSize,&actual);
        if(actual!=static_cast<std::int64_t>(vb))return fail(*diagnostic,VehicleWindowError50DS::allocation,"RA vertex storage size differs");
        gl.getBufferParameteri64v(elementBuffer,bufferSize,&actual);
        if(actual!=static_cast<std::int64_t>(ib))return fail(*diagnostic,VehicleWindowError50DS::allocation,"RA index storage size differs");
        constexpr const char* vertex=R"GLSL(#version 400 core
in vec3 position;in vec2 uv;in vec4 color;in float fogByte;
uniform mat4 worldViewProjection;
out vec2 sourceUv;noperspective out vec4 packetColor;noperspective out float packetFog;
void main(){gl_Position=vec4(position,1.0)*worldViewProjection;sourceUv=uv;packetColor=color;packetFog=fogByte;}
)GLSL";
        constexpr const char* fragment=R"GLSL(
in vec2 sourceUv;noperspective in vec4 packetColor;noperspective in float packetFog;
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
    vec4 packet=floor(clamp(packetColor,0.0,255.0));
    vec4 bytes=floor(clamp(sampled*packet/128.0,0.0,255.0));
    if(alphaReference>=0.0 && bytes.a<=alphaReference)discard;
    if(fogColorAndEnable.w!=0.0){
        float f=floor(clamp(packetFog,0.0,255.0));
        bytes.rgb=clamp(floor((bytes.rgb*f+fogColorAndEnable.rgb*(255.0-f))/256.0),0.0,255.0);
    }
#ifndef RA_SAMPLE_DESTINATION
    sourceDepth=gl_FragCoord.z;
    sourceColor=payload!=0?bytes:bytes/255.0;
#else
    vec3 destination=floor(clamp(texelFetch(destinationImage,ivec2(gl_FragCoord.xy),gl_SampleID).rgb*255.0,0.0,255.0)+0.5);
    bytes.rgb=clamp(destination+floor((bytes.rgb-destination)*bytes.a/128.0),0.0,255.0);
    sourceColor=bytes/255.0;
#endif
}
)GLSL";
        if(!compileTrackShader(0x8B31,vertex,raWorldVertexShader))return false;
        try {
        for(U i=0;i<2;++i){
            std::string source=i?"#version 400 core\n#define RA_SAMPLE_DESTINATION\n":"#version 400 core\n";
            source+=fragment;
            if(!compileTrackShader(0x8B30,source.c_str(),raWorldFragment[i]))return false;
            auto& p=raWorldProgram[i];p=shaderGl.createProgram();
            if(!p)return fail(*diagnostic,VehicleWindowError50DS::shader,"zero RA world program");
            shaderGl.attachShader(p,raWorldVertexShader);shaderGl.attachShader(p,raWorldFragment[i]);
            const char* names[]={"position","uv","color","fogByte"};
            for(U a=0;a<4;++a)shaderGl.bindAttribLocation(p,a,names[a]);
            shaderGl.bindFragDataLocation(p,0,"sourceColor");if(!i)shaderGl.bindFragDataLocation(p,1,"sourceDepth");
            shaderGl.linkProgram(p);I linked=0;shaderGl.getProgramiv(p,0x8B82,&linked);
            if(!check("link native RA source-byte material shader")||!linked){
                shaderGl.getProgramInfoLog(p,sizeof(diagnostic->shader.log),nullptr,diagnostic->shader.log);
                diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]=0;
                return fail(*diagnostic,VehicleWindowError50DS::shader,"native RA world shader link failed");}
            auto& u=raWorldUniform[i];u.matrix=shaderGl.getUniformLocation(p,"worldViewProjection");
            u.image=shaderGl.getUniformLocation(p,"image");u.fog=shaderGl.getUniformLocation(p,"fogColorAndEnable");
            u.alpha=shaderGl.getUniformLocation(p,"alphaReference");
            u.payload=shaderGl.getUniformLocation(p,"payload");u.destination=shaderGl.getUniformLocation(p,"destinationImage");
            if(u.matrix<0||u.image<0||u.fog<0||u.alpha<0||(i?u.destination<0:u.payload<0))
                return fail(*diagnostic,VehicleWindowError50DS::shader,"RA source uniforms missing");
        }
        }catch(...){return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate RA shader source");}
        drawGl.bindVertexArray(0);return check("finish native RA geometry and material programs");
    }
    bool prepareRaWorldFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d) noexcept {
        raWorldFramePrepared=false;
        if(!raWorld)return true;
        if(!raWorldCurrent(d))return false;
        using N=ra_static_fog_numeric50LA::Status;
        bool first=true;
        for(const auto& plan:raWorld->surfaceDrawPlan_){
            ra_static_fog50LA::CameraObservation camera;ra_static_fog50LA::PlacementObservation placement;
            if(!request.raStaticWorld->readNativeStaticFogObservation(plan.placement,camera,placement))
                return fail(d,VehicleWindowError50DS::frame,"current RA source camera/fog binding required");
            if(first){
                NativeCameraViewContract50DX contract;
                contract.provenance=NativeCameraViewProvenance50DX::currentCore49FForOutput;
                contract.width=frame.width;contract.height=frame.height;contract.clipDepth=NativeCameraClipDepth50DX::negativeOneToOne;
                NativeCameraMatrix50DX projected;NativeCameraViewError50DX error;
                if(!convertNativeCameraView50DX(camera.camera,contract,projected,error)||
                   std::memcmp(projected.worldViewProjection,frame.worldViewProjection,sizeof(projected.worldViewProjection)))
                    return fail(d,VehicleWindowError50DS::frame,"RA world requires exact current fog-prepared camera matrix");
                raWorldCamera=camera;first=false;
            }else if(camera.generation!=raWorldCamera.generation||camera.publication!=raWorldCamera.publication||
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
            for(U v=0;v<plan.vertexCount;++v){
                const auto& input=raWorld->vertices_[plan.vertexStart+v];auto& output=raWorldFrame[plan.vertexStart+v];
                std::copy(input.worldPosition.begin(),input.worldPosition.end(),output.position);
                std::copy(input.sourceUv.begin(),input.sourceUv.end(),output.uv);
                std::array<std::uint8_t,4> color;
                if(raster.requiresNativeStaticLighting){
                    ra_static_lighting50LA::StaticDiffuseSurface identity;
                    game::original_racer_lighting50KM::DiffuseVertexRgba diffuse;
                    if(!request.raStaticWorld->readNativeStaticDiffuseVertex(plan.surface,v,identity,diffuse)||
                       identity.cameraGeneration!=camera.generation||identity.cameraPublication!=camera.publication||identity.cameraFrame!=camera.frame)
                        return fail(d,VehicleWindowError50DS::frame,"RA current source diffuse bytes unavailable");
                    color=diffuse.rgba;
                }else {if(!plan.hasOriginalColors)return fail(d,VehicleWindowError50DS::assets,"RA prelit source colors absent");color=input.sourceColor;}
                for(U a=0;a<4;++a)output.color[a]=color[a];
                std::uint8_t fogByte=ra_static_fog_numeric50LA::noComputeFogByte;
                if(raster.fragmentDemands.sourceFogByte&&placement.computeFogCandidate02){
                    std::array<std::int32_t,3> raw;
                    if(!request.raStaticWorld->readNativeStaticRawPosition(plan.surface,v,raw)||
                       ra_static_fog_numeric50LA::radialByte(fog,raw,fogByte)!=N::complete)
                        return fail(d,VehicleWindowError50DS::frame,"RA original raw position/fog byte unavailable");
                }
                output.fog=fogByte;
            }
        }
        if(first||!raWorldCurrent(d))return fail(d,VehicleWindowError50DS::frame,"RA current frame has no complete source plan");
        raWorldFramePrepared=true;return true;
    }
    bool raWorldFrameCurrent(VehicleWindowDiagnostic50DS& d) const noexcept {
        if(!raWorldFramePrepared||!raWorldCurrent(d)||raWorld->surfaceDrawPlan_.empty())
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
        return true;
    }
    void bindRaWorldMaterial(const ra_static_world50LA::Owner::RasterInventory& raster,U program,bool payload) noexcept {
        using F=assets::OriginalUiGsField50KG;
        const auto field=[&](F f){return raster.finalContext.fields[static_cast<std::size_t>(f)].value;};
        const auto& u=raWorldUniform[program];gl.useProgram(raWorldProgram[program]);
        gl.uniformMatrix4fv(u.matrix,1,0,matrix);gl.uniform1i(u.image,0);
        const U color=raWorldCamera.source.color30;
        gl.uniform4f(u.fog,float(color&255),float((color>>8)&255),float((color>>16)&255),field(F::foggingOnOff)?1.f:0.f);
        gl.uniform1f(u.alpha,field(F::alphaTestOnOff)?float(field(F::alphaReferenceValue)):-1.f);
        if(program)gl.uniform1i(u.destination,2);else gl.uniform1i(u.payload,payload?1:0);
    }
    bool snapshotRaWorld(U bank,const std::array<I,4>& region) noexcept {
        gl.bindFramebuffer(readFramebuffer,coverageLive.scene[bank]);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,coverageLive.snapshot);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(region[0],region[1],region[2],region[3],
            region[0],region[1],region[2],region[3],colorBufferBit,0x2600);
        return check("retain actual per-sample destination before next RA primitive");
    }
    bool raTriangleOutsideNativeClip(U first) const noexcept {
        // Skip only triangles provably outside one common GL clip plane.
        // Conservative error bounds cover float dot-product rounding on GPU;
        // uncertain/nonfinite cases retain the original ordered submission.
        unsigned common=63u;
        for(U corner=0;corner<3;++corner){
            if(first+corner>=raWorld->indices_.size())return false;
            const auto index=raWorld->indices_[first+corner];
            if(index>=raWorldFrame.size())return false;
            const auto& v=raWorldFrame[index];
            const double p[4]={v.position[0],v.position[1],v.position[2],1.};
            unsigned outside=0;
            for(U plane=0;plane<6;++plane){
                const U axis=plane/2;const double sign=(plane&1)?-1.:1.;
                double value=0,scale=0;
                for(U k=0;k<4;++k){
                    const double w=double(matrix[k*4+3])*p[k];
                    const double a=double(matrix[k*4+axis])*p[k];
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
    std::array<I,4> nativeTriangleRasterRegion(const std::array<std::array<float,3>,3>& triangle)const noexcept {
        const std::array<I,4> full{{0,0,width,height}};
        double clip[3][4]{},error[3][4]{};unsigned common=63u;
        for(U corner=0;corner<3;++corner){
            const auto& v=triangle[corner];const double p[4]={v[0],v[1],v[2],1.};
            for(U row=0;row<4;++row){
                double magnitude=0;
                for(U k=0;k<4;++k){const double term=double(matrix[k*4+row])*p[k];clip[corner][row]+=term;magnitude+=std::abs(term);}
                error[corner][row]=32.*std::numeric_limits<float>::epsilon()*magnitude+0.0001;
                if(!std::isfinite(clip[corner][row])||!std::isfinite(error[corner][row]))return full;
            }
            unsigned outside=0;
            for(U plane=0;plane<6;++plane){const U axis=plane/2;
                const double value=clip[corner][3]+((plane&1)?-clip[corner][axis]:clip[corner][axis]);
                if(value < -(error[corner][3]+error[corner][axis]))outside|=1u<<plane;
            }
            common&=outside;
        }
        // Fully clipped triangles have no payload or destination consumers,
        // including triangles entirely behind the camera. Do not expand
        // those into a costly full-target fallback merely because W is low.
        if(common)return {{0,0,0,0}};
        double low[2]={1.,1.},high[2]={-1.,-1.};
        for(U corner=0;corner<3;++corner){
            const auto& c=clip[corner];const auto& e=error[corner];
            if(c[3]<=e[3]+0.0001)return full;
            for(U axis=0;axis<2;++axis)for(int a:{-1,1})for(int w:{-1,1}){
                const double value=(c[axis]+a*e[axis])/(c[3]+w*e[3]);
                if(!std::isfinite(value))return full;
                low[axis]=std::min(low[axis],value);high[axis]=std::max(high[axis],value);
            }
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
    std::array<I,4> raTriangleRasterRegion(U first)const noexcept {
        std::array<std::array<float,3>,3> triangle;
        for(U corner=0;corner<3;++corner){
            if(first+corner>=raWorld->indices_.size())return {{0,0,width,height}};
            const auto index=raWorld->indices_[first+corner];
            if(index>=raWorldFrame.size())return {{0,0,width,height}};
            std::copy_n(raWorldFrame[index].position,3,triangle[corner].begin());
        }
        return nativeTriangleRasterRegion(triangle);
    }
    std::array<I,4> racerTriangleRasterRegion(U slot,U component,U first)const noexcept {
        const std::array<I,4> full{{0,0,width,height}};
        if(!request.raStaticWorld||slot>frameOpponentCount)return full;
        const auto& batch=slot?frameOpponentBatches[slot-1]:framePlayerBatch;
        if(component>=batch.componentCount)return full;
        const auto& part=batch.component[component];
        if(!part.vertices||!part.indices||first>part.indexCount||part.indexCount-first<3)return full;
        std::array<std::array<float,3>,3> triangle;
        for(U corner=0;corner<3;++corner){
            const auto index=part.indices[first+corner];if(index>=part.vertexCount)return full;
            std::copy_n(part.vertices[index].position,3,triangle[corner].begin());
        }
        return nativeTriangleRasterRegion(triangle);
    }
    bool drawRaWorld(U bank) noexcept {
        if(!raWorld)return true;
        if(!raWorldFrameCurrent(*diagnostic)||!coverageLive.scene[bank]||!coverageLive.snapshot||!coverageLive.raster)
            return fail(*diagnostic,VehicleWindowError50DS::frame,"RA complete common target/coverage storage missing");
        drawGl.bindVertexArray(raWorldArray);drawGl.bindBuffer(arrayBuffer,raWorldVertices);
        drawGl.bufferSubData(arrayBuffer,0,static_cast<std::intptr_t>(raWorldFrame.size()*sizeof(RaWorldVertex)),raWorldFrame.data());
        drawGl.bindBuffer(elementBuffer,raWorldIndices);
        gl.disable(0x0BE2);gl.disable(0x0B44);gl.disable(0x0C11);gl.disable(0x8DB9);gl.colorMask(1,1,1,1);
        bool accepted=check("upload same-camera RA packed-color and fog operands");
        for(const auto& plan:raWorld->surfaceDrawPlan_){
            if(!accepted)break;
            const auto& raster=raWorld->rasterInventory_[plan.raster];
            const bool blend=raster.fragmentDemands.orderedCurrentDestination;
            const bool aa=raster.requiresNativeCoverage;
            const U image=raWorldFrameImages[plan.surface];
            // Opaque original triangles can share a draw. Blended triangles
            // observe each preceding primitive's current destination samples.
            const U step=blend?3:plan.indexCount;
            for(U first=plan.indexStart;accepted&&first<plan.indexStart+plan.indexCount;first+=step){
                if(blend&&raTriangleOutsideNativeClip(first))continue;
                if(blend){
                    // Only this conservative raster rectangle can observe the
                    // next primitive. Snapshot, payload clear and composition
                    // share it; all other samples remain untouched.
                    const auto region=raTriangleRasterRegion(first);
                    if(region[2]<=region[0]||region[3]<=region[1])continue;
                    gl.scissor(region[0],region[1],region[2]-region[0],region[3]-region[1]);gl.enable(0x0C11);
                    if(!snapshotRaWorld(bank,region)){accepted=false;break;}
                }else gl.disable(0x0C11);
                gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);gl.bindTexture(texture2D,image);
                drawGl.bindVertexArray(raWorldArray);
                if(blend&&!aa){
                    // AA0 uses one center payload followed by per-sample
                    // composition, matching the established racer bridge.
                    gl.bindFramebuffer(framebuffer,coverageLive.raster);
                    const U outputs[]={colorAttachment,colorAttachment+1};gl.drawBuffers(2,outputs);
                    coverageAa0();gl.disable(0x0B71);gl.depthMask(0);
                    const float invalid[]={0,0,0,-1};gl.clearBufferfv(0x1800,0,invalid);
                    bindRaWorldMaterial(raster,0,true);
                    drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(first)*sizeof(U)),0);
                    gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);gl.drawBuffer(colorAttachment);
                    gl.enable(0x809D);gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(1);
                    gl.useProgram(coverageProgram);drawGl.bindVertexArray(coverageArray);
                    gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,coverageLive.payload);
                    gl.activeTexture(textureUnit0+1);if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,coverageLive.rasterDepth);
                    gl.activeTexture(textureUnit0+2);if(gl.bindSampler)gl.bindSampler(2,0);gl.bindTexture(coverageTexture,coverageLive.snapshotColor);
                    gl.uniform1i(coveragePayloadUniform,0);gl.uniform1i(coverageDepthUniform,1);gl.uniform1i(coverageDestinationUniform,2);
                    gl.uniform1i(coverageRaCompositionUniform,1);gl.drawArrays(0x0004,0,3);
                }else {
                    gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);gl.drawBuffer(colorAttachment);
                    bindTrackCoverage(aa?NativeTrackCoverage50DS::fourSampleAa1:NativeTrackCoverage50DS::aa0);
                    gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(1);
                    bindRaWorldMaterial(raster,blend?1:0,false);
                    if(blend){gl.activeTexture(textureUnit0+2);if(gl.bindSampler)gl.bindSampler(2,0);gl.bindTexture(coverageTexture,coverageLive.snapshotColor);}
                    drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(step),0x1405,reinterpret_cast<const void*>(std::uintptr_t(first)*sizeof(U)),0);
                }
                gl.activeTexture(textureUnit0+2);gl.bindTexture(coverageTexture,0);
                gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);
                accepted=check("draw source-ordered RA material primitive with source depth/alpha rules");
            }
            if(accepted){++diagnostic->trackDraws;diagnostic->trackIndices+=plan.indexCount;}
        }
        gl.activeTexture(textureUnit0+2);gl.bindTexture(coverageTexture,0);
        gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,0);
        gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        gl.disable(0x0C11);
        resetPodState();const bool restored=check("restore common scene target after complete RA world");
        return accepted&&restored;
    }
