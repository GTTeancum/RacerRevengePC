// Private VehicleWindowGl50DS::Storage. Exact RC source bridge, no extra target
// allocation and no change to existing material modes. AA1 crowd demand is
// mandatory; crystal AA0 uses the actual checked per-sample composition path.
    I rcCrystalPassUniform=-1,rcCrystalPayloadUniform=-1,rcCrystalAmbientUniform=-1;
    std::unique_ptr<std::array<float,2>[]> rcCrystalUvs;
    std::array<std::uint8_t,4> rcCrystalAmbient{};
    bool rcCrystalPrepared=false;
    U activeFrameBank=0;
    struct RcFrameObservation {
        VehicleWindowDiagnostic50DS& diagnostic;bool enabled;
        std::chrono::steady_clock::time_point start;
        RcFrameObservation(Storage& s,VehicleWindowDiagnostic50DS& d)noexcept:
            diagnostic(d),enabled(bool(s.request.track.rcCrystal)&&s.request.enableGameplayDisplayWallTiming),
            start(enabled?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{}){}
        ~RcFrameObservation(){if(!enabled)return;
            const auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count();
            diagnostic.rcCrystalSubmissionWallUs=static_cast<std::uint64_t>(std::max<std::int64_t>(0,elapsed));
            diagnostic.rcCrystalWallMeasured=true;
            std::fprintf(stderr,"rc_crystal_frame triangles=%u empty=%u compositions_issued=%u full_rectangles=%u cpu_submission_wall_us=%llu batch_accepted=%u error=%u\n",
                diagnostic.rcCrystalTriangles,diagnostic.rcCrystalEmptyTriangles,diagnostic.rcCrystalCompositionsIssued,
                diagnostic.rcCrystalFullRectangles,static_cast<unsigned long long>(diagnostic.rcCrystalSubmissionWallUs),
                diagnostic.batchAccepted?1u:0u,static_cast<unsigned>(diagnostic.error));
        }
    };
    bool createRcCrystalProjection(const VehicleWindowTrack50DS& track)noexcept {
        if(!track.rcCrystal||!coverageDemand)return fail(*diagnostic,VehicleWindowError50DS::request,
            "RC crystal requires its authentic crowd AA1 coverage demand");
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
    bool prepareRcCrystalFrame(const VehicleWindowFrame50DS& frame,VehicleWindowDiagnostic50DS& d)noexcept {
        rcCrystalPrepared=false;
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
           !request.track.rcCrystal->project(camera,rcCrystalUvs.get(),rc_crystal50RC::Owner::uvCount,rcCrystalAmbient))
            return fail(d,VehicleWindowError50DS::frame,"RC crystal camera/ambient/projection preflight rejected");
        rcCrystalPrepared=true;return true;
    }
    bool uploadRcCrystalUvs()noexcept {
        if(!request.track.rcCrystal)return true;
        if(!rcCrystalPrepared||!trackProjectionBuffer)return fail(*diagnostic,VehicleWindowError50DS::frame,"RC projection lacks complete preflight");
        drawGl.bindBuffer(arrayBuffer,trackProjectionBuffer);std::size_t offset=0;
        for(const auto& range:request.track.rcCrystal->ranges()){
            drawGl.bufferSubData(arrayBuffer,std::intptr_t(range.vertexStart)*sizeof(std::array<float,2>),
                std::intptr_t(range.vertexCount)*sizeof(std::array<float,2>),rcCrystalUvs.get()+offset);
            if(!check("upload authenticated RC crystal range"))return false;offset+=range.vertexCount;
        }
        return offset==rc_crystal50RC::Owner::uvCount;
    }
    std::array<I,4> rcTriangleRegion(U first)const noexcept {
        const auto* mesh=request.track.rcCrystal->mesh();
        if(!mesh||first>mesh->indices.size()||mesh->indices.size()-first<3)return {{0,0,width,height}};
        std::array<std::array<float,3>,3> triangle{};
        for(U c=0;c<3;++c){const U index=mesh->indices[first+c];if(index>=mesh->vertices.size())return {{0,0,width,height}};
            std::copy_n(mesh->vertices[index].position,3,triangle[c].begin());}
        return nativeTriangleRasterRegion(triangle);
    }
    bool drawRcCrystalTriangle(U bank,U first,U pass,U image,const std::array<I,4>& region)noexcept {
        ++diagnostic->rcCrystalTriangles;
        if(region[2]<=region[0]||region[3]<=region[1]){++diagnostic->rcCrystalEmptyTriangles;return true;}
        if(region==std::array<I,4>{{0,0,width,height}})++diagnostic->rcCrystalFullRectangles;
        if(!coverageDemand||!coverageLive.raster||!coverageLive.snapshot||!rcCrystalPrepared||pass<1||pass>2)
            return fail(*diagnostic,VehicleWindowError50DS::frame,"RC crystal actual coverage resources unavailable");
        coverageAa0();gl.disable(0x8DB9);gl.disable(0x0BE2);gl.colorMask(1,1,1,1);
        gl.viewport(0,0,width,height);gl.scissor(region[0],region[1],region[2]-region[0],region[3]-region[1]);gl.enable(0x0C11);
        gl.bindFramebuffer(framebuffer,coverageLive.raster);gl.disable(0x0B71);gl.depthMask(0);
        const float invalid[]={0.f,0.f,0.f,-1.f};gl.clearBufferfv(0x1800,0,invalid);
        gl.useProgram(trackProgram);gl.uniform1i(rcCrystalPayloadUniform,1);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,image);drawGl.bindVertexArray(trackVertexArray);
        bool accepted=check("bind RC AA0 source byte/depth payload");
        if(accepted){drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(first)*sizeof(U)),0);
            accepted=check("raster actual ordered RC crystal triangle");}
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
        gl.uniform1i(rcCrystalPayloadUniform,0);drawGl.bindVertexArray(trackVertexArray);
        coverageAa0();gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(0);gl.disable(0x0BE2);gl.disable(0x0C11);
        const bool restored=check("restore RC track state after sample composition");return accepted&&restored;
    }
    bool drawRcCrystalRange(U bank,U first,U count,U pass,U image)noexcept {
        if(count%3)return fail(*diagnostic,VehicleWindowError50DS::indices,"RC crystal triangle extent");
        for(U i=first;i<first+count;i+=3)if(!drawRcCrystalTriangle(bank,i,pass,image,rcTriangleRegion(i)))return false;
        return true;
    }
