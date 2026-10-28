// Included only by the existing world renderer TU, inside rr::platform.
// Explicit native four-vertex StripOptFog projection/fog/GL clipping adapter.
// This does not emulate VU preamble/EFU/fixedpoint or claim366450 parity.
bool VehicleWindowGl50DS::Storage::deleteRaLocalFlareResources(VehicleWindowDiagnostic50DS& d)noexcept {
    raLocalFlareFrames.clear();
    MaterialTextureGlDiagnostic50EB texture;
    if(!raLocalFlareTexture.close(texture)){
        d.cleanupIncomplete=true;d.cleanupGlError=texture.cleanupGlError?texture.cleanupGlError:texture.glError;return false;
    }
    raLocalFlarePublication=0;
    if(!deleteShaderName(raLocalFlareProgram,true,d)||!deleteShaderName(raLocalFlareVertexShader,false,d)||
       !deleteShaderName(raLocalFlareFragmentShader,false,d)||!deleteName(raLocalFlareArray,gl.deleteVertexArrays,d)||
       !deleteName(raLocalFlareBuffer,gl.deleteBuffers,d))return false;
    raLocalFlareArchive.reset();raLocalFlareMember=nullptr;raLocalFlareResource=nullptr;
    raLocalFlareFailed=false;return true;
}
bool VehicleWindowGl50DS::Storage::prepareRaLocalFlareImage(const LocalFlareEmission::Operands& operands)noexcept {
    if(raLocalFlareFailed||!diagnostic||!raWorldCurrent(*diagnostic)||!LocalFlareEmission::nativeRasterProfile(operands))return false;
    std::shared_ptr<const std::vector<std::uint8_t>> archive;const std::string* member=nullptr;
    const std::uint8_t* data=nullptr;std::size_t bytes=0;
    if(!LocalFlareEmission::borrowImage(operands,archive,member,data,bytes))
        return fail(*diagnostic,VehicleWindowError50DS::material,"flare first-provider PST custody unavailable");
    const auto* resource=LocalFlareEmission::map(operands).texture;
    if(raLocalFlarePublication){
        return resource==raLocalFlareResource&&archive==raLocalFlareArchive&&member==raLocalFlareMember&&
            raLocalFlareTexture.validBorrow(request.context,request.isCurrent,raLocalFlarePublication)&&
            raLocalFlareTexture.matchesSourceBank(raLocalFlarePixels,raLocalFlarePublication);
    }
    if(raLocalFlareTexture.ownsResources())return false;
    // Existing RA producer gives the complete49-image bank64MiB, not a new
    // flare budget. Charge this one16KiB image against that SAME total.
    constexpr std::size_t worldImageCeiling=64u*1024u*1024u,flareImageBytes=64u*64u*4u;
    if(!request.raStaticWorldTextures||request.raStaticWorldTextures->gpuByteCount()>worldImageCeiling-flareImageBytes)
        return fail(*diagnostic,VehicleWindowError50DS::limits,"flare exceeds existing total RA image ceiling");
    raLocalFlareFailed=true; //Any partial upload stays cleanup-owned; no retry/replay.
    try {
        if(!data||!member||bytes!=6460u)
            return fail(*diagnostic,VehicleWindowError50DS::assets,"unsupported actual flare PST extent");
        std::vector<std::uint8_t> source(data,data+bytes);
        assets::Texture48E decoded;assets::DecodeError48C decode;
        if(!assets::decodeTexture48E(source,decoded,decode)||decoded.kind!=1||decoded.mips.size()!=3||
           decoded.mips.back().width!=64||decoded.mips.back().height!=64||decoded.mips.back().rgba.size()!=64u*64u*4u)
            return fail(*diagnostic,VehicleWindowError50DS::assets,"actual flare indexed64 image unsupported");
        assets::MaterialTextureAssetInput50EA image{1,&decoded.base_rrtex};
        assets::MaterialTextureAssetsInput50EA input;input.textures=&image;input.textureCount=1;
        input.budgets={1,0,assets::maxMaterialTextureSourceBytes50EA,64u*64u*4u,64};
        assets::MaterialTextureAssetDiagnostic50EA pixels;
        if(!raLocalFlarePixels.load(input,pixels))return fail(*diagnostic,VehicleWindowError50DS::assets,"actual flare native pixel bank");
        // Immutable first selected archive plus actual source object survive
        // partial GPU failures until this renderer's ordinary close succeeds.
        raLocalFlareArchive=std::move(archive);raLocalFlareMember=member;raLocalFlareResource=resource;
        MaterialTextureGlRequest50EB upload;upload.context=request.context;upload.isCurrent=request.isCurrent;
        upload.originalUploadBinding=request.originalUploadBinding;const char* missing=nullptr;
        if(!loadMaterialTextureFunctions50EE(request.context,request.procAddress,upload.gl,missing))
            return fail(*diagnostic,VehicleWindowError50DS::entryPoint,"flare image upload functions");
        // Same explicit native base-level linear sampling as the RA backend,
        // not a claim of original GS K/mip residency or filtering parity.
        const MaterialTextureSampler50EB sampler{MaterialTextureFilter50EB::linear,MaterialTextureFilter50EB::linear,
            MaterialTextureWrap50EB::repeat,MaterialTextureWrap50EB::repeat};
        upload.samplers=&sampler;upload.samplerCount=1;upload.maximumGpuBytes=64u*64u*4u;
        I active=0,saved[6]{};gl.getIntegerv(0x84E0,&active);
        if(!check("capture flare active texture unit"))return false;
        gl.activeTexture(textureUnit0);
        constexpr U keys[]={0x8069,0x88EF,0x0CF5,0x0CF2,0x0CF3,0x0CF4};
        for(U i=0;i<6;++i)gl.getIntegerv(keys[i],&saved[i]);
        if(!check("capture flare image upload state")){gl.activeTexture(U(active));return false;}
        MaterialTextureGlDiagnostic50EB td;const bool created=raLocalFlareTexture.create(upload,raLocalFlarePixels,td);
        upload.gl.bindTexture(texture2D,U(saved[0]));upload.gl.bindBuffer(0x88EC,U(saved[1]));
        for(U i=2;i<6;++i)upload.gl.pixelStorei(keys[i],saved[i]);gl.activeTexture(U(active));
        const bool restored=check("restore flare image upload state");
        if(!created||!restored)return fail(*diagnostic,VehicleWindowError50DS::material,"flare image upload incomplete");
        raLocalFlarePublication=raLocalFlareTexture.publicationToken();
        if(!raLocalFlarePublication)return false;
        raLocalFlareFailed=false;return true;
    }catch(...){return fail(*diagnostic,VehicleWindowError50DS::allocation,"flare image allocation");}
}
bool VehicleWindowGl50DS::Storage::createRaLocalFlareGeometry()noexcept {
    if(raLocalFlareProgram)return !raLocalFlareFailed&&raLocalFlareArray&&raLocalFlareBuffer;
    if(raLocalFlareFailed||!diagnostic||!coverageProgram)return false;
    // The dedicated RA backend already bounds each native geometry/operand
    // stream by max_mesh_vertices. Charge the six expanded flare vertices
    // within that existing count/byte ceiling; no separate unbounded stream.
    if(raWorldGeometry.size()>assets::max_mesh_vertices-6u||
       raWorldFrame.size()!=raWorldGeometry.size()||
       sizeof(RaLocalFlareVertex)!=sizeof(RaWorldVertex)+sizeof(RaWorldFrameVertex))
        return fail(*diagnostic,VehicleWindowError50DS::limits,"flare exceeds existing RA vertex stream ceiling");
    I previousArray=0,previousBuffer=0;
    gl.getIntegerv(0x85B5,&previousArray);gl.getIntegerv(0x8894,&previousBuffer);
    if(!check("capture actual flare preparation vertex bindings"))return false;
    const bool allocated=[&]()noexcept {
    raLocalFlareFailed=true;
    constexpr const char* vertex=R"GLSL(#version 400 core
in vec3 position;in vec2 uv;in vec4 color;in float fog;
uniform mat4 worldViewProjection;
out vec2 sourceUv;noperspective out vec4 sourceRgba;noperspective out float sourceFog;
void main(){gl_Position=vec4(position,1.0)*worldViewProjection;
    sourceUv=uv;sourceRgba=color;sourceFog=fog;}
)GLSL";
    constexpr const char* fragment=R"GLSL(#version 400 core
in vec2 sourceUv;noperspective in vec4 sourceRgba;noperspective in float sourceFog;
uniform sampler2D image;uniform vec4 fogColor;
out vec4 sourceColor;out float sourceDepth;
void main(){
    vec4 sampled=floor(clamp(texture(image,sourceUv)*255.0,0.0,255.0));
    vec4 packet=floor(clamp(sourceRgba,0.0,255.0));
    vec4 bytes=floor(clamp(sampled*packet/128.0,0.0,255.0));
    if(bytes.a<=0.0)discard;
    float f=floor(clamp(sourceFog,0.0,255.0));
    bytes.rgb=clamp(floor((bytes.rgb*f+fogColor.rgb*(255.0-f))/256.0),0.0,255.0);
    sourceColor=bytes;sourceDepth=gl_FragCoord.z;
}
)GLSL";
    if(!compileTrackShader(0x8B31,vertex,raLocalFlareVertexShader)||
       !compileTrackShader(0x8B30,fragment,raLocalFlareFragmentShader))return false;
    raLocalFlareProgram=shaderGl.createProgram();if(!raLocalFlareProgram)return false;
    shaderGl.attachShader(raLocalFlareProgram,raLocalFlareVertexShader);
    shaderGl.attachShader(raLocalFlareProgram,raLocalFlareFragmentShader);
    const char* names[]={"position","uv","color","fog"};
    for(U i=0;i<4;++i)shaderGl.bindAttribLocation(raLocalFlareProgram,i,names[i]);
    shaderGl.bindFragDataLocation(raLocalFlareProgram,0,"sourceColor");shaderGl.bindFragDataLocation(raLocalFlareProgram,1,"sourceDepth");
    shaderGl.linkProgram(raLocalFlareProgram);I linked=0;shaderGl.getProgramiv(raLocalFlareProgram,0x8B82,&linked);
    if(!check("link distinct native LightFlare strip shader")||!linked)return false;
    raLocalFlareMatrix=shaderGl.getUniformLocation(raLocalFlareProgram,"worldViewProjection");
    raLocalFlareImage=shaderGl.getUniformLocation(raLocalFlareProgram,"image");
    raLocalFlareFog=shaderGl.getUniformLocation(raLocalFlareProgram,"fogColor");
    if(raLocalFlareMatrix<0||raLocalFlareImage<0||raLocalFlareFog<0)return false;
    gl.genVertexArrays(1,&raLocalFlareArray);drawGl.bindVertexArray(raLocalFlareArray);
    gl.genBuffers(1,&raLocalFlareBuffer);drawGl.bindBuffer(arrayBuffer,raLocalFlareBuffer);
    constexpr std::size_t bytes=6*sizeof(RaLocalFlareVertex);
    gl.bufferData(arrayBuffer,bytes,nullptr,dynamicDraw);
    const I counts[]={3,2,4,1};const std::size_t offsets[]={offsetof(RaLocalFlareVertex,position),offsetof(RaLocalFlareVertex,uv),
        offsetof(RaLocalFlareVertex,color),offsetof(RaLocalFlareVertex,fog)};
    for(U i=0;i<4;++i){gl.enableVertexAttribArray(i);gl.vertexAttribPointer(i,counts[i],floatType,0,
        sizeof(RaLocalFlareVertex),reinterpret_cast<const void*>(offsets[i]));}
    if(!raLocalFlareArray||!raLocalFlareBuffer||!verifyBuffer(arrayBuffer,bytes,dynamicDraw)||
       !check("allocate fixed six native flare triangle vertices"))return false;
    return true;
    }();
    drawGl.bindVertexArray(static_cast<U>(previousArray));
    drawGl.bindBuffer(arrayBuffer,static_cast<U>(previousBuffer));
    const bool restored=check("restore actual flare preparation vertex bindings");
    if(!allocated||!restored){raLocalFlareFailed=true;return false;}
    raLocalFlareFailed=false;return true;
}
bool VehicleWindowGl50DS::Storage::finishRaLocalFlareFrame(RaLocalFlareFrame& frame)noexcept {
    if(!frame.operands||!LocalFlareEmission::nativeRasterProfile(*frame.operands)||
       !prepareRaLocalFlareImage(*frame.operands)||!createRaLocalFlareGeometry())return false;
    const auto& source=*frame.operands;
    // Named native radial interpolation of the actual camera's fog range.
    // Source VU computes radial fog per vertex; exact VF18/preamble and EFU
    // rounding are not claimed. No fragment-distance replacement is used.
    const double start=raWorldCamera.source.start34,end=raWorldCamera.source.end38;
    if(!std::isfinite(start)||!std::isfinite(end)||!(end>start))return false;
    constexpr U order[]={0,1,2,2,1,3};
    for(U i=0;i<6;++i){auto& out=frame.vertices[i];const auto& in=source.vertices[order[i]];
        double squared=0;
        for(U k=0;k<3;++k){out.position[k]=in.position[k];
            const double camera=raWorldCamera.position[k];if(!std::isfinite(camera))return false;
            const double delta=double(in.position[k])-camera;squared+=delta*delta;}
        const double distance=std::sqrt(squared),factor=(end-distance)/(end-start);
        if(!std::isfinite(distance)||!std::isfinite(factor))return false;
        out.fog=float(std::floor(std::max(0.,std::min(1.,factor))*255.));
        for(U k=0;k<2;++k)out.uv[k]=in.uv[k];for(U k=0;k<4;++k)out.color[k]=source.color[k];
    }
    return true;
}
bool VehicleWindowGl50DS::Storage::drawRaLocalFlare(U bank,std::size_t ordinal,const std::array<I,4>& region)noexcept {
    bool completed=false;
    struct Failure {bool& failed;bool& completed;~Failure(){if(!completed)failed=true;}} failure{raLocalFlareFailed,completed};
    if(ordinal>=raLocalFlareFrames.size()||raLocalFlareFailed||!raLocalFlareProgram||
       !raLocalFlareTexture.validBorrow(request.context,request.isCurrent,raLocalFlarePublication))return false;
    const auto& frame=raLocalFlareFrames[ordinal];
    if(!frame.operands||!LocalFlareEmission::materialCurrent(*frame.operands))return false;
    const auto* image=raLocalFlareTexture.asset(0);if(!image||!image->name)return false;
    drawGl.bindVertexArray(raLocalFlareArray);drawGl.bindBuffer(arrayBuffer,raLocalFlareBuffer);
    drawGl.bufferSubData(arrayBuffer,0,sizeof(frame.vertices),frame.vertices.data());
    gl.disable(0x0BE2);gl.disable(0x0B44);gl.disable(0x8DB9);gl.colorMask(1,1,1,1);
    for(U first=0;first<6;first+=3){
        gl.scissor(region[0],region[1],region[2]-region[0],region[3]-region[1]);gl.enable(0x0C11);
        if(!snapshotRaWorld(bank,region))return false;
        gl.bindFramebuffer(framebuffer,coverageLive.raster);coverageAa0();gl.disable(0x0B71);gl.depthMask(0);
        const float invalid[]={0,0,0,-1};gl.clearBufferfv(0x1800,0,invalid);
        gl.useProgram(raLocalFlareProgram);drawGl.bindVertexArray(raLocalFlareArray);
        gl.uniformMatrix4fv(raLocalFlareMatrix,1,0,matrix);gl.uniform1i(raLocalFlareImage,0);
        const U fog=raWorldCamera.source.color30;
        gl.uniform4f(raLocalFlareFog,float((fog>>16)&255),float((fog>>8)&255),float(fog&255),1.f);
        gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);gl.bindTexture(texture2D,image->name);
        gl.drawArrays(0x0004,I(first),3);
        gl.bindFramebuffer(framebuffer,coverageLive.scene[bank]);gl.enable(0x809D);gl.enable(0x0B71);
        gl.depthFunc(0x0203);gl.depthMask(1);gl.useProgram(coverageProgram);drawGl.bindVertexArray(coverageArray);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,coverageLive.payload);
        gl.activeTexture(textureUnit0+1);if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,coverageLive.rasterDepth);
        gl.activeTexture(textureUnit0+2);if(gl.bindSampler)gl.bindSampler(2,0);gl.bindTexture(coverageTexture,coverageLive.snapshotColor);
        gl.uniform1i(coverageRaCompositionUniform,2);gl.drawArrays(0x0004,0,3);
        if(!check("draw typed native flare triangle with exact additive byte composition"))return false;
    }
    completed=true;return true;
}
