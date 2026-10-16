// VehicleWindowGl50DS::Storage. Native RA submission ownership, not a guest
// command stream. Original world topology, material profiles and byte operands
// remain owned by the actual aggregate. Source polygon clipping is native GPU
// math; raster/depth/MSAA remain GL.
    std::shared_ptr<ra_static_world50LA::Owner> raWorld;
    const VehicleWindowGl50DS* raWorldRenderer=nullptr;
    std::uint64_t raWorldImagePublication=0;
    U raWorldArray=0,raWorldVertices=0,raWorldIndices=0;
    U raWorldVertexShader=0,raWorldGeometryShader=0,raWorldFragment[2]{},raWorldProgram[2]{};
    struct RaWorldUniforms { I matrix=-1,sourceMatrix=-1,image=-1,fog=-1,alpha=-1,payload=-1,destination=-1; } raWorldUniform[2];
    struct RaWorldVertex { float position[3],uv[2],color[4],fog; };
    std::vector<RaWorldVertex> raWorldFrame;
    std::vector<U> raWorldFrameImages;
    struct RaWorldFrameDraw { std::size_t surface=0;std::array<I,4> scissor{}; };
    std::vector<RaWorldFrameDraw> raWorldFrameDraws;
    ra_static_fog50LA::CameraObservation raWorldCamera;
    float raWorldSourceMatrix[16]{};
    bool raWorldFramePrepared=false;
    bool raPixelProvenanceLogged=false;
    bool hasRaWorldResources() const noexcept {
        return raWorldArray||raWorldVertices||raWorldIndices||raWorldVertexShader||raWorldGeometryShader||
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
        return deleteShaderName(raWorldGeometryShader,false,d)&&
            deleteShaderName(raWorldVertexShader,false,d)&&
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
            // import_scene48d emits chronological consecutive strip records,
            // swapping only A/B for odd strip triangles. Restarts begin a new
            // chronological run; lowering adds one constant placement offset.
            // This invariant lets the GS recover source fan anchors from actual
            // indexed vertex IDs, independently of draw batching/primitive ID.
            for(U offset=0;offset<plan.indexCount;offset+=3){
                const auto a=raWorld->indices_[plan.indexStart+offset];
                const auto b=raWorld->indices_[plan.indexStart+offset+1];
                const auto c=raWorld->indices_[plan.indexStart+offset+2];
                const std::uint64_t oldest=std::min(a,b),middle=std::max(a,b);
                if(oldest<plan.vertexStart||middle!=oldest+1||std::uint64_t(c)!=oldest+2||
                   std::uint64_t(c)>=std::uint64_t(plan.vertexStart)+plan.vertexCount||
                   c>static_cast<U>(std::numeric_limits<I>::max()))
                    return fail(*diagnostic,VehicleWindowError50DS::assets,
                        "RA source triangle lacks retained chronological strip indices");
            }
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
uniform mat4 worldViewProjection,sourceWorldViewProjection;
out SourceVertex { vec4 clip;vec2 uv;vec4 color;float fog;flat int sourceIndex; } operand;
void main(){
    gl_Position=vec4(position,1.0)*worldViewProjection;
    operand.clip=vec4(position,1.0)*sourceWorldViewProjection;
    operand.uv=uv;operand.color=color;operand.fog=fogByte;operand.sourceIndex=gl_VertexID;
}
)GLSL";
        // Authenticated common clipper 366450, called by PrelitCompressed.
        // Source near is Z+W; native depth conversion makes it GL Z+3W.
        // Keep both positions: GL still owns final depth clipping/rasterization.
        constexpr const char* geometry=R"GLSL(#version 400 core
layout(triangles) in;
layout(triangle_strip,max_vertices=21) out;
in SourceVertex { vec4 clip;vec2 uv;vec4 color;float fog;flat int sourceIndex; } operand[];
out vec2 sourceUv;smooth out vec4 packetColorTimesClipW;
smooth out float packetFogTimesClipW;
struct Vertex { vec4 clip;vec4 nativeClip;vec2 uv;vec4 color;float fog; };
Vertex polygon[9],nextPolygon[9];
float distanceToPlane(vec4 p,int plane){
    int axis=2-plane/2;
    return (plane%2==0)?p.w+p[axis]:p.w-p[axis];
}
Vertex intersection(Vertex a,Vertex b,float da,float db,bool aInside){
    // 367330 / 367380 retain the entering/leaving endpoint forms.
    float q=(aInside?da:db)/(da-db);
    Vertex origin=a;
    if(!aInside)origin=b;
    Vertex result;
    result.clip=origin.clip+q*(b.clip-a.clip);
    result.nativeClip=origin.nativeClip+q*(b.nativeClip-a.nativeClip);
    result.uv=origin.uv+q*(b.uv-a.uv);
    result.color=origin.color+q*(b.color-a.color);
    result.fog=origin.fog+q*(b.fog-a.fog);
    return result;
}
void emitPacket(Vertex v){
    gl_Position=v.nativeClip;sourceUv=v.uv;
    // 367160 / 367180: undo post-near color weighting, then FTOI0.
    vec4 rawColor=floor(clamp(v.color/v.clip.w,0.0,255.0));
    // Source clipping's C*sourceW representation ends above. The separate
    // native transport below carries final byte RGBA through GL clipping.
    packetColorTimesClipW=rawColor*gl_Position.w;
    // 367188 FTOI4 packs F in bits 4..11. Fog was never W-weighted
    // during source clipping. Transport only AFTER polygon generation.
    packetFogTimesClipW=floor(clamp(v.fog,0.0,255.0))*gl_Position.w;
    EmitVertex();
}
void main(){
    int count=3;bool clipped=false;
    for(int i=0;i<3;++i){
        polygon[i].clip=operand[i].clip;polygon[i].nativeClip=gl_in[i].gl_Position;
        polygon[i].uv=operand[i].uv;polygon[i].color=operand[i].color;
        polygon[i].fog=operand[i].fog;
        for(int plane=0;plane<6;++plane)
            clipped=clipped || distanceToPlane(polygon[i].clip,plane)<0.0;
    }
    if(!clipped){
        // No source clip-generated operands: keep the original byte values.
        for(int i=0;i<3;++i){
            gl_Position=gl_in[i].gl_Position;sourceUv=operand[i].uv;
            packetColorTimesClipW=operand[i].color*gl_Position.w;
            packetFogTimesClipW=operand[i].fog*gl_Position.w;EmitVertex();
        }
        EndPrimitive();return;
    }
    // 3665F8..3667F0 assembles oldest, middle, current source records.
    // The importer swaps the first two indices for odd strip triangles;
    // undo that only for clipped polygons, preserving the source fan anchor.
    if(operand[0].sourceIndex>operand[1].sourceIndex){
        Vertex first=polygon[0];polygon[0]=polygon[1];polygon[1]=first;
    }
    // Convex triangle intersected by six half spaces has at most 3+6
    // vertices. Each pass adds at most one; fan output is at most 21.
    // Source order: -Z,+Z,-Y,+Y,-X,+X (366870..366FD8).
    for(int plane=0;plane<6;++plane){
        int nextCount=0;
        for(int i=0;i<count;++i){
            Vertex a=polygon[(i+count-1)%count],b=polygon[i];
            float da=distanceToPlane(a.clip,plane),db=distanceToPlane(b.clip,plane);
            bool aInside=da>=0.0,bInside=db>=0.0;
            // The source stores A on inside edges, starting last -> first.
            if(aInside)nextPolygon[nextCount++]=a;
            // Preserve coincident boundary records: later source plane
            // rotations depend on their count and therefore the fan anchor.
            // At most two crossings, with at least one excluded vertex,
            // imply nextCount<=count+1 even with endpoint intersections.
            if(aInside!=bInside)nextPolygon[nextCount++]=intersection(a,b,da,db,aInside);
        }
        count=nextCount;
        if(count<3)return;
        for(int i=0;i<count;++i){
            polygon[i]=nextPolygon[i];
            // 3669C0: raw RGBA at first near intersection, C*W thereafter.
            if(plane==0)polygon[i].color*=polygon[i].clip.w;
        }
    }
    for(int i=1;i<count-1;++i){
        emitPacket(polygon[0]);emitPacket(polygon[i]);emitPacket(polygon[i+1]);
        EndPrimitive();
    }
}
)GLSL";
        constexpr const char* fragment=R"GLSL(
in vec2 sourceUv;smooth in vec4 packetColorTimesClipW;smooth in float packetFogTimesClipW;
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
    // Carry all original RGBA lanes homogeneously, as with fog below.
    // Reconstruct screen-linear color before the unchanged byte operations.
    vec4 packet=floor(clamp(packetColorTimesClipW*gl_FragCoord.w,0.0,255.0));
    vec4 bytes=floor(clamp(sampled*packet/128.0,0.0,255.0));
    if(alphaReference>=0.0 && bytes.a<=alphaReference)discard;
    if(fogColorAndEnable.w!=0.0){
        // Smooth transport of F*clipW, then multiply by interpolated 1/W.
        // This reconstructs screen-linear F without a clip-generated F/W
        // division near W=0. Preserve the existing byte clamp/quantization.
        float f=floor(clamp(packetFogTimesClipW*gl_FragCoord.w,0.0,255.0));
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
        I geometryVertices=0,geometryComponents=0;
        gl.getIntegerv(0x8DE0,&geometryVertices); // GL_MAX_GEOMETRY_OUTPUT_VERTICES
        gl.getIntegerv(0x8DE1,&geometryComponents); // GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS
        if(!check("query RA source clip geometry limits")||geometryVertices<21||geometryComponents<21*11)
            return fail(*diagnostic,VehicleWindowError50DS::limits,"RA source clip geometry limits insufficient");
        if(!compileTrackShader(0x8B31,vertex,raWorldVertexShader)||
           !compileTrackShader(0x8DD9,geometry,raWorldGeometryShader))return false;
        try {
        for(U i=0;i<2;++i){
            std::string source=i?"#version 400 core\n#define RA_SAMPLE_DESTINATION\n":"#version 400 core\n";
            source+=fragment;
            if(!compileTrackShader(0x8B30,source.c_str(),raWorldFragment[i]))return false;
            auto& p=raWorldProgram[i];p=shaderGl.createProgram();
            if(!p)return fail(*diagnostic,VehicleWindowError50DS::shader,"zero RA world program");
            shaderGl.attachShader(p,raWorldVertexShader);shaderGl.attachShader(p,raWorldGeometryShader);
            shaderGl.attachShader(p,raWorldFragment[i]);
            const char* names[]={"position","uv","color","fogByte"};
            for(U a=0;a<4;++a)shaderGl.bindAttribLocation(p,a,names[a]);
            shaderGl.bindFragDataLocation(p,0,"sourceColor");if(!i)shaderGl.bindFragDataLocation(p,1,"sourceDepth");
            shaderGl.linkProgram(p);I linked=0;shaderGl.getProgramiv(p,0x8B82,&linked);
            if(!check("link native RA source-byte material shader")||!linked){
                shaderGl.getProgramInfoLog(p,sizeof(diagnostic->shader.log),nullptr,diagnostic->shader.log);
                diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]=0;
                return fail(*diagnostic,VehicleWindowError50DS::shader,"native RA world shader link failed");}
            auto& u=raWorldUniform[i];u.matrix=shaderGl.getUniformLocation(p,"worldViewProjection");
            u.sourceMatrix=shaderGl.getUniformLocation(p,"sourceWorldViewProjection");
            u.image=shaderGl.getUniformLocation(p,"image");u.fog=shaderGl.getUniformLocation(p,"fogColorAndEnable");
            u.alpha=shaderGl.getUniformLocation(p,"alphaReference");
            u.payload=shaderGl.getUniformLocation(p,"payload");u.destination=shaderGl.getUniformLocation(p,"destinationImage");
            if(u.matrix<0||u.sourceMatrix<0||u.image<0||u.fog<0||u.alpha<0||(i?u.destination<0:u.payload<0))
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
        ra_cold_aggregate50LA::Diagnostic visibility;
        if(!request.raStaticWorld->prepareNativeStaticDrawPublication(visibility)){
            std::fprintf(stderr,"RA native visibility rejected: stage=%u failure=%s lighting_status=%u source_pc=0x%08x\n",
                unsigned(visibility.stage),visibility.failure?visibility.failure:"",
                unsigned(visibility.lighting.status),unsigned(visibility.lighting.sourcePC));
            return fail(d,VehicleWindowError50DS::frame,"RA current original portal/model/LOD selection failed");
        }
        const auto* selected=request.raStaticWorld->nativeStaticDrawPublication();
        if(!selected)return fail(d,VehicleWindowError50DS::frame,"RA native draw publication expired");
        raWorldFrameDraws.clear();
        try {
            for(const auto& record:*selected){
                const ra_static_world50LA::Owner::Placement* placement=nullptr;
                for(const auto& owned:raWorld->placements_)if(owned->worldInstance==record.worldInstance){placement=owned.get();break;}
                if(!placement||record.sourceLod>=placement->lodRanges.size())
                    return fail(d,VehicleWindowError50DS::assets,"RA selected authentic LOD has no retained geometry");
                const auto& range=placement->lodRanges[record.sourceLod];
                if(range.firstSurface>raWorld->surfaceDrawPlan_.size()||
                   range.surfaceCount>raWorld->surfaceDrawPlan_.size()-range.firstSurface||
                   record.viewport[2]<=0||record.viewport[3]<=0||
                   record.scissor[0]>record.scissor[1]||record.scissor[2]>record.scissor[3])
                    return fail(d,VehicleWindowError50DS::assets,"RA selected surface/scissor range differs");
                // Original scissor endpoints are inclusive and top-origin.
                // Native framebuffer uses bottom-origin and actual pixel extent.
                const double x=record.viewport[0],y=record.viewport[1];
                const double w=record.viewport[2],h=record.viewport[3];
                const double bounds[4]={
                    (double(record.scissor[0])-x)/w*frame.width,
                    (y+h-double(record.scissor[3])-1.)/h*frame.height,
                    (double(record.scissor[1])+1.-x)/w*frame.width,
                    (y+h-double(record.scissor[2]))/h*frame.height};
                std::array<I,4> scissor{};
                for(U axis=0;axis<4;++axis){
                    const double extent=(axis&1u)?frame.height:frame.width;
                    const double rounded=axis<2?std::floor(bounds[axis]):std::ceil(bounds[axis]);
                    scissor[axis]=static_cast<I>(std::max(0.,std::min(extent,rounded)));
                }
                if(scissor[0]>=scissor[2]||scissor[1]>=scissor[3])continue;
                for(std::size_t surface=range.firstSurface;surface<range.firstSurface+range.surfaceCount;++surface){
                    const auto& plan=raWorld->surfaceDrawPlan_[surface];
                    if(plan.worldInstance!=record.worldInstance||plan.sourceLod!=record.sourceLod||plan.surface!=surface)
                        return fail(d,VehicleWindowError50DS::assets,"RA native selection and source draw inventory differ");
                    raWorldFrameDraws.push_back({surface,scissor});
                }
            }
        }catch(...){return fail(d,VehicleWindowError50DS::allocation,"allocate ordered RA selected frame surfaces");}
        // Empty visibility is valid; it still owns the same actual camera.
        ra_static_fog50LA::PlacementObservation firstPlacement;
        if(raWorld->surfaceDrawPlan_.empty()||!request.raStaticWorld->readNativeStaticFogObservation(
            raWorld->surfaceDrawPlan_.front().placement,raWorldCamera,firstPlacement))
            return fail(d,VehicleWindowError50DS::frame,"RA current source camera/fog binding required");
        NativeCameraViewContract50DX contract;
        contract.provenance=NativeCameraViewProvenance50DX::currentCore49FForOutput;
        contract.width=frame.width;contract.height=frame.height;contract.clipDepth=NativeCameraClipDepth50DX::negativeOneToOne;
        NativeCameraMatrix50DX projected;NativeCameraViewError50DX error;
        if(!convertNativeCameraView50DX(raWorldCamera.camera,contract,projected,error)||
           std::memcmp(projected.worldViewProjection,frame.worldViewProjection,sizeof(projected.worldViewProjection)))
            return fail(d,VehicleWindowError50DS::frame,"RA world requires exact current fog-prepared camera matrix");
        for(U row=0;row<4;++row)for(U column=0;column<4;++column)
            raWorldSourceMatrix[column*4+row]=raWorldCamera.camera.combined[row*4+column];
        for(const auto& selectedDraw:raWorldFrameDraws){
            const auto& plan=raWorld->surfaceDrawPlan_[selectedDraw.surface];
            ra_static_fog50LA::CameraObservation camera;ra_static_fog50LA::PlacementObservation placement;
            if(!request.raStaticWorld->readNativeStaticFogObservation(plan.placement,camera,placement))
                return fail(d,VehicleWindowError50DS::frame,"current RA source camera/fog binding required");
            if(camera.generation!=raWorldCamera.generation||camera.publication!=raWorldCamera.publication||
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
        if(!request.raStaticWorld->nativeStaticDrawPublication()||!raWorldCurrent(d))return fail(d,VehicleWindowError50DS::frame,"RA current frame has no complete source plan");
        raWorldFramePrepared=true;return true;
    }
    bool raWorldFrameCurrent(VehicleWindowDiagnostic50DS& d) const noexcept {
        if(!raWorldFramePrepared||!raWorldCurrent(d)||!request.raStaticWorld->nativeStaticDrawPublication()||raWorld->surfaceDrawPlan_.empty())
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
        gl.uniformMatrix4fv(u.matrix,1,0,matrix);
        gl.uniformMatrix4fv(u.sourceMatrix,1,0,raWorldSourceMatrix);gl.uniform1i(u.image,0);
        const U color=raWorldCamera.source.color30;
        //Original color30 is packed ARGB, not a GS-native low-byte-R word.
        //RA SCB colorrgb180,180,165 is retained as0xffb4b4a5 by the actual
        //fog owner;13E990 passes that packed word unchanged to renderer+28.
        gl.uniform4f(u.fog,float((color>>16)&255),float((color>>8)&255),float(color&255),field(F::foggingOnOff)?1.f:0.f);
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
                    // Same row-vector product as GLSL; each output lane
                    // reads one column of the column-major uniform storage.
                    const double w=double(matrix[3*4+k])*p[k];
                    const double a=double(matrix[axis*4+k])*p[k];
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
            // gl_Position = vec4(position,1) * worldViewProjection;
            // uniformMatrix4fv(...,GL_FALSE,...) keeps column-major storage.
            for(U column=0;column<4;++column){
                double magnitude=0;
                for(U k=0;k<4;++k){const double term=double(matrix[column*4+k])*p[k];clip[corner][column]+=term;magnitude+=std::abs(term);}
                error[corner][column]=32.*std::numeric_limits<float>::epsilon()*magnitude+0.0001;
                if(!std::isfinite(clip[corner][column])||!std::isfinite(error[corner][column]))return full;
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
    // Capture-only attestation of existing GL attribute/storage, never a draw,
    // source replay, visibility test or pixel-winner claim. Six vertex reads
    // are bounded to the two explicitly requested source floor triangles.
    void traceRaFogGpuAttestation() noexcept {
        if(!gl.getVertexAttribiv||!gl.getVertexAttribPointerv||!gl.getBufferSubData||
           !gl.getIntegerv||!gl.getBufferParameteri64v||!drawGl.bindVertexArray||
           !drawGl.bindBuffer||!drawGl.getError){
            std::fprintf(stderr,"ra_fog_gpu_attestation unavailable=missing_loaded_api gpu_winner_unknown=1\n");return;
        }
        const U priorError=drawGl.getError();
        if(priorError){
            std::fprintf(stderr,"ra_fog_gpu_attestation unavailable=preexisting_gl_error error=%u gpu_winner_unknown=1\n",priorError);return;
        }
        I oldArray=0,oldBuffer=0;
        gl.getIntegerv(0x85B5,&oldArray);gl.getIntegerv(0x8894,&oldBuffer);
        U error=drawGl.getError();
        if(error||oldArray<0||oldBuffer<0){
            std::fprintf(stderr,"ra_fog_gpu_attestation unavailable=binding_query error=%u gpu_winner_unknown=1\n",error);return;
        }
        drawGl.bindVertexArray(raWorldArray);
        constexpr U properties[]={0x8622,0x8623,0x8624,0x8625,0x886A,0x889F,0x88FD};
        // enabled,size,stride,type,normalized,associatedVBO,integer.
        I actual[7]{};
        for(U i=0;i<7;++i)gl.getVertexAttribiv(3,properties[i],&actual[i]);
        void* pointer=nullptr;gl.getVertexAttribPointerv(3,0x8645,&pointer);
        I element=0;gl.getIntegerv(0x8895,&element);
        error=drawGl.getError();
        const auto offset=reinterpret_cast<std::uintptr_t>(pointer);
        const bool layout=!error&&actual[0]==1&&actual[1]==1&&
            actual[2]==I(sizeof(RaWorldVertex))&&actual[3]==I(floatType)&&actual[4]==0&&
            actual[5]==I(raWorldVertices)&&actual[6]==0&&offset==offsetof(RaWorldVertex,fog);
        std::fprintf(stderr,"ra_fog_gpu_attribute vao=%u attribute=3 query_error=%u enabled=%d size=%d stride=%d type=%d normalized=%d vbo=%d integer=%d offset=%llu expected_vbo=%u expected_stride=%zu expected_offset=%zu layout_match=%u element_buffer=%d expected_element=%u gpu_winner_unknown=1\n",
            raWorldArray,error,actual[0],actual[1],actual[2],actual[3],actual[4],actual[5],actual[6],
            static_cast<unsigned long long>(offset),raWorldVertices,sizeof(RaWorldVertex),offsetof(RaWorldVertex,fog),unsigned(layout),element,raWorldIndices);
        drawGl.bindBuffer(arrayBuffer,raWorldVertices);
        std::int64_t vertexBytes=-1;gl.getBufferParameteri64v(arrayBuffer,bufferSize,&vertexBytes);
        const U bufferError=drawGl.getError();
        const std::size_t surface=438;
        if(error||bufferError||vertexBytes<0||surface>=raWorld->surfaceDrawPlan_.size()){
            std::fprintf(stderr,"ra_fog_gpu_attestation unavailable=source_or_buffer query_error=%u buffer_error=%u vertex_bytes=%lld surface=%zu\n",
                error,bufferError,static_cast<long long>(vertexBytes),surface);
        }else{
            const auto& plan=raWorld->surfaceDrawPlan_[surface];
            bool selected=false;for(const auto& draw:raWorldFrameDraws)if(draw.surface==surface)selected=true;
            for(U triangle:{116u,117u}){
                const std::size_t relative=std::size_t(triangle)*3;
                if(relative>plan.indexCount||plan.indexCount-relative<3||
                   std::size_t(plan.indexStart)>raWorld->indices_.size()||
                   relative+3>raWorld->indices_.size()-std::size_t(plan.indexStart)){
                    std::fprintf(stderr,"ra_fog_gpu_triangle surface=%zu triangle=%u unavailable=index_range\n",surface,triangle);continue;
                }
                for(U corner=0;corner<3;++corner){
                    const auto index=raWorld->indices_[std::size_t(plan.indexStart)+relative+corner];
                    const std::uint64_t byte=std::uint64_t(index)*sizeof(RaWorldVertex)+offsetof(RaWorldVertex,fog);
                    if(index>=raWorldFrame.size()||byte>std::uint64_t(vertexBytes)||
                       sizeof(float)>std::uint64_t(vertexBytes)-byte||
                       byte>std::uint64_t(std::numeric_limits<std::intptr_t>::max())){
                        std::fprintf(stderr,"ra_fog_gpu_vertex surface=%zu triangle=%u corner=%u index=%u unavailable=vertex_range\n",surface,triangle,corner,index);continue;
                    }
                    float gpu=0;gl.getBufferSubData(arrayBuffer,static_cast<std::intptr_t>(byte),sizeof(gpu),&gpu);
                    const U readError=drawGl.getError();const float cpu=raWorldFrame[index].fog;
                    U gpuBits=0,cpuBits=0;std::memcpy(&gpuBits,&gpu,sizeof(gpuBits));std::memcpy(&cpuBits,&cpu,sizeof(cpuBits));
                    std::fprintf(stderr,"ra_fog_gpu_vertex surface=%zu triangle=%u corner=%u selected=%u index=%u byte=%llu buffer=%u read_error=%u cpu=%.9g gpu=%.9g cpu_bits=%08x gpu_bits=%08x bytes_match=%u fetch_layout_match=%u gpu_winner_unknown=1\n",
                        surface,triangle,corner,unsigned(selected),index,static_cast<unsigned long long>(byte),raWorldVertices,readError,
                        double(cpu),double(gpu),cpuBits,gpuBits,unsigned(!readError&&cpuBits==gpuBits),unsigned(layout));
                }
            }
        }
        // ARRAY_BUFFER is global; VAO restoration does not restore it. Neither
        // queried VAO nor its element/attribute associations were modified.
        drawGl.bindVertexArray(static_cast<U>(oldArray));drawGl.bindBuffer(arrayBuffer,static_cast<U>(oldBuffer));
        I restoredArray=-1,restoredBuffer=-1;gl.getIntegerv(0x85B5,&restoredArray);gl.getIntegerv(0x8894,&restoredBuffer);
        const U restoreError=drawGl.getError();
        std::fprintf(stderr,"ra_fog_gpu_attestation_end old_vao=%d restored_vao=%d old_array_buffer=%d restored_array_buffer=%d restore_error=%u restored=%u gpu_winner_unknown=1\n",
            oldArray,restoredArray,oldBuffer,restoredBuffer,restoreError,unsigned(!restoreError&&oldArray==restoredArray&&oldBuffer==restoredBuffer));
    }
    // Optional read-only source provenance for the actual capture-selected RA frame.
    // These conservative rectangle candidates are NOT GPU coverage/winners:
    // texture alpha, depth, samples, later racers and HUD are not evaluated.
    void traceRaPixelProvenance() noexcept {
        if(raPixelProvenanceLogged)return;
        const char* enabled=std::getenv("RR_RA_PIXEL_PROVENANCE");
        if(!enabled||std::strcmp(enabled,"1"))return;
        raPixelProvenanceLogged=true;
        traceRaFogGpuAttestation();
        // Requested reference probe: GL pixel(50,28) in a640x448 capture.
        // Select the corresponding pixel center when the framebuffer differs.
        const I px=std::min(width-1,static_cast<I>(50.5*double(width)/640.));
        const I py=std::min(height-1,static_cast<I>(28.5*double(height)/448.));
        std::fprintf(stderr,"ra_pixel_provenance_begin cpu_candidates_only=1 capture_selected_frame=1 width=%d height=%d pixel=%d,%d camera_generation=%llu camera_publication=%llu camera_frame=%llu fog_revision=%llu draws=%zu\n",
            width,height,px,py,static_cast<unsigned long long>(raWorldCamera.generation),
            static_cast<unsigned long long>(raWorldCamera.publication),
            static_cast<unsigned long long>(raWorldCamera.frame),
            static_cast<unsigned long long>(raWorldCamera.fogRevision),raWorldFrameDraws.size());
        constexpr std::size_t limit=512;
        std::size_t triangles=0,candidates=0,logged=0,drawOrdinal=0;
        for(const auto& draw:raWorldFrameDraws){
            const auto& plan=raWorld->surfaceDrawPlan_[draw.surface];
            const auto& raster=raWorld->rasterInventory_[plan.raster];
            const auto& material=raWorld->imageFamilies_[raster.family].materialMember;
            for(U first=plan.indexStart;first<plan.indexStart+plan.indexCount;first+=3){
                ++triangles;
                if(px<draw.scissor[0]||px>=draw.scissor[2]||py<draw.scissor[1]||py>=draw.scissor[3])continue;
                // Same matrix/index/vertex operands and conservative near-clip
                // fallback as the actual RA blended-triangle raster region.
                const auto region=raTriangleRasterRegion(first);
                if(px<region[0]||px>=region[2]||py<region[1]||py>=region[3])continue;
                ++candidates;if(logged==limit)continue;++logged;
                std::fprintf(stderr,"ra_pixel_candidate draw=%zu worldInstance=%zu lod=%u sourceSurface=%u surface=%zu triangle=%u material=%s materialRow=%d blend=%u aa=%u scissor=%d,%d,%d,%d conservative_region=%d,%d,%d,%d",
                    drawOrdinal,plan.worldInstance,plan.sourceLod,plan.sourceSurface,plan.surface,
                    (first-plan.indexStart)/3,material.c_str(),raster.materialRow,
                    unsigned(raster.fragmentDemands.orderedCurrentDestination),unsigned(raster.requiresNativeCoverage),
                    draw.scissor[0],draw.scissor[1],draw.scissor[2],draw.scissor[3],
                    region[0],region[1],region[2],region[3]);
                for(U corner=0;corner<3;++corner){
                    const auto& v=raWorldFrame[raWorld->indices_[first+corner]];
                    std::fprintf(stderr," v%u_world=%.9g,%.9g,%.9g v%u_uv=%.9g,%.9g v%u_rgba=%.9g,%.9g,%.9g,%.9g v%u_fog=%.9g",
                        corner,double(v.position[0]),double(v.position[1]),double(v.position[2]),
                        corner,double(v.uv[0]),double(v.uv[1]),corner,double(v.color[0]),double(v.color[1]),
                        double(v.color[2]),double(v.color[3]),corner,double(v.fog));
                    double clip[4]{};
                    for(U column=0;column<4;++column){
                        for(U axis=0;axis<3;++axis)clip[column]+=double(matrix[column*4+axis])*v.position[axis];
                        clip[column]+=matrix[column*4+3];
                    }
                    std::fprintf(stderr," v%u_clip=%.17g,%.17g,%.17g,%.17g",corner,clip[0],clip[1],clip[2],clip[3]);
                }
                std::fprintf(stderr,"\n");
            }
            ++drawOrdinal;
        }
        std::fprintf(stderr,"ra_pixel_provenance_end triangles=%zu conservative_candidates=%zu logged=%zu truncated=%zu gpu_winner_unknown=1\n",
            triangles,candidates,logged,candidates-logged);
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
        for(const auto& selectedDraw:raWorldFrameDraws){
            const auto& plan=raWorld->surfaceDrawPlan_[selectedDraw.surface];
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
                    auto region=raTriangleRasterRegion(first);
                    for(U axis=0;axis<2;++axis){region[axis]=std::max(region[axis],selectedDraw.scissor[axis]);
                        region[axis+2]=std::min(region[axis+2],selectedDraw.scissor[axis+2]);}
                    if(region[2]<=region[0]||region[3]<=region[1])continue;
                    gl.scissor(region[0],region[1],region[2]-region[0],region[3]-region[1]);gl.enable(0x0C11);
                    if(!snapshotRaWorld(bank,region)){accepted=false;break;}
                }else {
                    const auto& region=selectedDraw.scissor;
                    gl.scissor(region[0],region[1],region[2]-region[0],region[3]-region[1]);gl.enable(0x0C11);
                }
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
