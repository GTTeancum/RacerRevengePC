// Included only by vehicle_window_gl50ds_world.cpp, inside namespace rr::platform.
// The ONE track light-flare pass (declarations and guards: track_flare_gl_storage.inl).
//
// Source of each flare: the lighting owner's trackFlares() rows (roles.lights, 05.13), one per
// LightN that authors HasLightFlare: position (+14), the flare strip's GS byte colour, size
// LightFlareSize (+A0), zone, and LightFlareTextureMapName (+A4). The image is that name's own
// material in the course archive (textures\<name>.psm, ShaderPass_1 TextureMap_1 ->
// textures\<map>.pst), the same lookup on every course.
//
// Raster: the flare materials are GS ALPHA A=Cs,B=0,C=As,D=Cd (additive weighted by source alpha,
// out = Cd + Cs*As/128) with Cs = texel x strip colour / 128 and As = texel A x strip A / 128,
// radial world fog, depth tested and not written. That is exactly the track program's effect
// mode 1 with the 128-unit colour/alpha scales and SRC_ALPHA,ONE blending (the existing
// additive effect path), so no further program is created.
//
// PORT STAND-IN: geometry. The original flare is a 4-vertex StripOptFog strip built by 14AB30
// (UV 0,0 / 0,1 / 1,0 / 1,1) and turned by its entity every frame; its exact per-frame vertex
// placement is not recovered. Here every flare is a camera-facing square of half-extent
// LightFlareSize about the light position, in the same strip UV order, every course alike.
// PORT STAND-IN: the material's ALPHA fields are not re-selected here (the APPSTART DefaultMT
// templates live in the Application, not the renderer); every stock flare material authors the
// additive form drawn here (scan_all.tsv: ABE=1 A0B2C0D1 on every flare PSM).

bool VehicleWindowGl50DS::Storage::hasTrackFlareResources() const noexcept {
        return trackFlareTextures.ownsResources()||trackFlareVertexBuffer||trackFlareIndexBuffer||trackFlareVertexArray;
    }

bool VehicleWindowGl50DS::Storage::createTrackFlares() noexcept {
        trackFlareCount=0;trackFlareImage.clear();trackFlareVertices.clear();trackFlareFrameRows.clear();
        // Frontend, viewer and host-check routes supply no race track; a track
        // whose lights author no flare has no rows. Neither is a refusal.
        if(!request.commonTrack||!request.commonTrack->roles.lights)return true;
        const auto& track=*request.commonTrack;
        const auto flares=track.roles.lights->trackFlares();
        if(flares.empty()){std::fprintf(stderr,"track_flare_renderer flares=0 images=0 unresolved=0\n");return true;}
        if(flares.size()>trackFlareLimit)
            return fail(*diagnostic,VehicleWindowError50DS::limits,"track flare rows exceed the lights block ceiling");
        if(!trackProgram)
            return fail(*diagnostic,VehicleWindowError50DS::request,"track flares require the textured track program");
        try {
            trackFlareImage.assign(flares.size(),trackFlareUnresolved);
            std::vector<std::string> names;  // distinct lower-case flare map names, first-use order
            std::vector<U> rowName(flares.size(),trackFlareUnresolved);
            for(std::size_t i=0;i<flares.size();++i){
                const auto& f=flares[i];
                for(float v:f.position)if(!std::isfinite(v))return fail(*diagnostic,VehicleWindowError50DS::assets,"nonfinite track flare position");
                for(float v:f.color)if(!std::isfinite(v)||v<0.f||v>255.f)
                    return fail(*diagnostic,VehicleWindowError50DS::assets,"track flare colour outside GS bytes");
                if(!std::isfinite(f.size)||f.size<0.f)return fail(*diagnostic,VehicleWindowError50DS::assets,"track flare size invalid");
                const void* end=std::memchr(f.texture,0,sizeof(f.texture));
                if(!end)return fail(*diagnostic,VehicleWindowError50DS::assets,"track flare texture name unterminated");
                std::string name(f.texture,static_cast<const char*>(end));
                for(char& c:name)if(c>='A'&&c<='Z')c=static_cast<char>(c-'A'+'a');
                if(name.empty()||name=="none")continue;   // authored without an image: nothing to draw
                std::size_t at=0;while(at<names.size()&&names[at]!=name)++at;
                if(at==names.size()){
                    if(names.size()>=trackFlareImageLimit)
                        return fail(*diagnostic,VehicleWindowError50DS::limits,"track flare images exceed the image guard");
                    names.push_back(name);
                }
                rowName[i]=static_cast<U>(at);
            }
            // Resolve each name through its own material, then its own PST.
            std::vector<std::vector<std::uint8_t>> rrtex(names.size());
            std::vector<U> nameImage(names.size(),trackFlareUnresolved);
            std::vector<assets::MaterialTextureAssetInput50EA> inputs;
            std::size_t rgbaBytes=0;U images=0;
            for(std::size_t n=0;n<names.size();++n){
                const auto refuse=[&](const char* why,const std::string& detail){
                    std::fprintf(stderr,"track_flare_image_refused name=%s reason=%s detail=%s\n",names[n].c_str(),why,detail.c_str());};
                common_track50CR::Diagnostic md;common_track50CR::Bytes psm,pst;
                if(!track.archive.member("textures\\"+names[n]+".psm",psm,64u*1024u,md)){refuse("material",md.reason);continue;}
                assets::Material48E material;assets::DecodeError48C decode;
                if(!assets::decodeMaterial48E(psm,material,decode)){refuse("material_decode",decode.code);continue;}
                const assets::MaterialTexture48E* map=nullptr;
                for(const auto& pass:material.passes)if(pass.pass==1)
                    for(const auto& t:pass.textures)if(t.slot==1&&!t.name.empty())map=&t;
                if(!map){refuse("material_map","ShaderPass_1 TextureMap_1 absent");continue;}
                std::string mapName=map->name;
                for(char& c:mapName)if(c>='A'&&c<='Z')c=static_cast<char>(c-'A'+'a');
                if(!track.archive.member("textures\\"+mapName+".pst",pst,4u*1024u*1024u,md)){refuse("texture",md.reason);continue;}
                assets::Texture48E decoded;
                if(!assets::decodeTexture48E(pst,decoded,decode)||decoded.mips.empty()||decoded.base_rrtex.empty()){
                    refuse("texture_decode",decode.code);continue;}
                const auto& base=decoded.mips.back();
                const std::size_t bytes=std::size_t(base.width)*base.height*4u;
                if(!base.width||!base.height||base.width>1024u||base.height>1024u||base.rgba.size()!=bytes){
                    refuse("texture_extent",mapName);continue;}
                if(bytes>trackFlareImageBytes-rgbaBytes)
                    return fail(*diagnostic,VehicleWindowError50DS::limits,"track flare images exceed the RGBA guard");
                rgbaBytes+=bytes;rrtex[n]=std::move(decoded.base_rrtex);
                nameImage[n]=images++;
                std::fprintf(stderr,"track_flare_image name=%s map=%s width=%u height=%u program=%s\n",names[n].c_str(),
                    mapName.c_str(),unsigned(base.width),unsigned(base.height),
                    material.passes.empty()?"":material.passes.front().microprogram.c_str());
            }
            inputs.reserve(images);
            for(std::size_t n=0;n<names.size();++n)if(nameImage[n]!=trackFlareUnresolved)
                inputs.push_back({std::uint64_t(nameImage[n])+1u,&rrtex[n]});
            std::size_t unresolved=0;
            for(std::size_t i=0;i<flares.size();++i){
                if(rowName[i]!=trackFlareUnresolved)trackFlareImage[i]=nameImage[rowName[i]];
                if(trackFlareImage[i]==trackFlareUnresolved)++unresolved;
            }
            if(images){
                assets::MaterialTextureAssetsInput50EA input;input.textures=inputs.data();input.textureCount=images;
                input.budgets={images,0,assets::maxMaterialTextureSourceBytes50EA,rgbaBytes,1024u};
                assets::MaterialTextureAssetDiagnostic50EA pixels;
                if(!trackFlarePixels.load(input,pixels))
                    return fail(*diagnostic,VehicleWindowError50DS::assets,"track flare native pixel bank");
                MaterialTextureGlRequest50EB upload;upload.context=request.context;upload.isCurrent=request.isCurrent;
                upload.originalUploadBinding=request.originalUploadBinding;const char* missing=nullptr;
                if(!loadMaterialTextureFunctions50EE(request.context,request.procAddress,upload.gl,missing))
                    return fail(*diagnostic,VehicleWindowError50DS::entryPoint,"track flare image upload functions");
                // Base-level linear sampling, repeat (the flare maps author no clamp).
                std::vector<MaterialTextureSampler50EB> samplers(images,MaterialTextureSampler50EB{
                    MaterialTextureFilter50EB::linear,MaterialTextureFilter50EB::linear,
                    MaterialTextureWrap50EB::repeat,MaterialTextureWrap50EB::repeat});
                upload.samplers=samplers.data();upload.samplerCount=images;upload.maximumGpuBytes=rgbaBytes;
                I active=0,saved[6]{};gl.getIntegerv(0x84E0,&active);
                if(!check("capture track flare active texture unit"))return false;
                gl.activeTexture(textureUnit0);
                constexpr U keys[]={0x8069,0x88EF,0x0CF5,0x0CF2,0x0CF3,0x0CF4};
                for(U i=0;i<6;++i)gl.getIntegerv(keys[i],&saved[i]);
                if(!check("capture track flare image upload state")){gl.activeTexture(U(active));return false;}
                MaterialTextureGlDiagnostic50EB td;const bool created=trackFlareTextures.create(upload,trackFlarePixels,td);
                upload.gl.bindTexture(texture2D,U(saved[0]));upload.gl.bindBuffer(0x88EC,U(saved[1]));
                for(U i=2;i<6;++i)upload.gl.pixelStorei(keys[i],saved[i]);gl.activeTexture(U(active));
                const bool restored=check("restore track flare image upload state");
                if(!created||!restored)return fail(*diagnostic,VehicleWindowError50DS::material,"track flare image upload incomplete");
                trackFlarePublication=trackFlareTextures.publicationToken();
                if(!trackFlarePublication)return fail(*diagnostic,VehicleWindowError50DS::material,"track flare image publication missing");
            }
            trackFlareVertices.resize(flares.size()*4u);
            trackFlareFrameRows.reserve(flares.size());
            std::vector<U> indices(flares.size()*6u);
            for(std::size_t q=0;q<flares.size();++q){
                const U b=static_cast<U>(q*4u);U* i=indices.data()+q*6u;
                i[0]=b;i[1]=b+1u;i[2]=b+2u;i[3]=b+2u;i[4]=b+1u;i[5]=b+3u;   // strip 0,1,2,3
            }
            gl.genBuffers(1,&trackFlareVertexBuffer);
            drawGl.bindBuffer(arrayBuffer,trackFlareVertexBuffer);
            gl.bufferData(arrayBuffer,static_cast<std::intptr_t>(trackFlareVertices.size()*sizeof(assets::Vertex48B)),nullptr,dynamicDraw);
            if(!check("allocate track flare vertices")||!trackFlareVertexBuffer)
                return fail(*diagnostic,VehicleWindowError50DS::allocation,"track flare vertex buffer");
            gl.genBuffers(1,&trackFlareIndexBuffer);
            drawGl.bindBuffer(arrayBuffer,trackFlareIndexBuffer);
            gl.bufferData(arrayBuffer,static_cast<std::intptr_t>(indices.size()*sizeof(U)),indices.data(),staticDraw);
            if(!check("upload track flare indices")||!trackFlareIndexBuffer)
                return fail(*diagnostic,VehicleWindowError50DS::allocation,"track flare index buffer");
            gl.genVertexArrays(1,&trackFlareVertexArray);
            drawGl.bindVertexArray(trackFlareVertexArray);
            drawGl.bindBuffer(arrayBuffer,trackFlareVertexBuffer);
            drawGl.bindBuffer(elementBuffer,trackFlareIndexBuffer);
            constexpr I sizes[]={3,2,4};
            constexpr std::uintptr_t offsets[]={0,12,20};
            for(U a=0;a<3;++a){
                gl.enableVertexAttribArray(a);
                gl.vertexAttribPointer(a,sizes[a],a==2?unsignedByte:floatType,a==2?1:0,24,reinterpret_cast<const void*>(offsets[a]));
            }
            drawGl.bindVertexArray(0);
            if(!check("create track flare Vertex48B layout")||!trackFlareVertexArray)
                return fail(*diagnostic,VehicleWindowError50DS::allocation,"track flare vertex array");
            trackFlareCount=flares.size();
            std::fprintf(stderr,"track_flare_renderer flares=%zu images=%u unresolved=%zu rgba_bytes=%zu limit_flares=%zu limit_images=%u limit_bytes=%zu\n",
                trackFlareCount,unsigned(images),unresolved,rgbaBytes,trackFlareLimit,unsigned(trackFlareImageLimit),trackFlareImageBytes);
            return true;
        }catch(...){return fail(*diagnostic,VehicleWindowError50DS::allocation,"track flare cold allocation");}
    }

bool VehicleWindowGl50DS::Storage::drawTrackFlares() noexcept {
        if(!trackFlareCount||!trackFlareVertexArray||!trackProgram)return true;
        const auto* lights=request.commonTrack?request.commonTrack->roles.lights.get():nullptr;
        if(!lights)return fail(*diagnostic,VehicleWindowError50DS::frame,"track flare lighting owner released");
        const auto flares=lights->trackFlares();
        if(flares.size()!=trackFlareCount||trackFlareImage.size()!=trackFlareCount||trackFlareVertices.size()!=trackFlareCount*4u)
            return fail(*diagnostic,VehicleWindowError50DS::frame,"track flare rows changed after the cold load");
        if(trackFlarePublication&&!trackFlareTextures.validBorrow(request.context,request.isCurrent,trackFlarePublication))
            return fail(*diagnostic,VehicleWindowError50DS::material,"track flare image publication/context changed");
        // Camera-facing axes from the frame's own world-to-clip matrix (row-vector,
        // column-major: column j = matrix[4j..4j+3]); the w column is the view axis.
        double right[3]={matrix[0],matrix[1],matrix[2]},up[3]={matrix[4],matrix[5],matrix[6]};
        const double view[3]={matrix[12],matrix[13],matrix[14]};
        const auto dot=[](const double* a,const double* b){return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];};
        const auto scale=[](double* a,double s){a[0]*=s;a[1]*=s;a[2]*=s;};
        const double viewLength=std::sqrt(dot(view,view));
        if(std::isfinite(viewLength)&&viewLength>0.){
            double forward[3]={view[0]/viewLength,view[1]/viewLength,view[2]/viewLength};
            const double r=dot(right,forward),u=dot(up,forward);
            for(U k=0;k<3;++k){right[k]-=r*forward[k];up[k]-=u*forward[k];}
        }
        const double rightLength=std::sqrt(dot(right,right));
        if(!std::isfinite(rightLength)||!(rightLength>0.))return fail(*diagnostic,VehicleWindowError50DS::frame,"track flare camera axes degenerate");
        scale(right,1./rightLength);
        const double along=dot(up,right);for(U k=0;k<3;++k)up[k]-=along*right[k];
        const double upLength=std::sqrt(dot(up,up));
        if(!std::isfinite(upLength)||!(upLength>0.))return fail(*diagnostic,VehicleWindowError50DS::frame,"track flare camera axes degenerate");
        scale(up,1./upLength);
        // Strip order 0..3 with its 14AB30 UVs; corner signs along (right, up).
        constexpr float corner[4][2]={{-1.f,1.f},{-1.f,-1.f},{1.f,1.f},{1.f,-1.f}};
        constexpr float uv[4][2]={{0.f,0.f},{0.f,1.f},{1.f,0.f},{1.f,1.f}};
        trackFlareFrameRows.clear();
        for(std::size_t i=0;i<trackFlareCount;++i){
            if(trackFlareImage[i]==trackFlareUnresolved)continue;
            const auto& f=flares[i];
            // Same section rule as the zoned static spans: a flare of a zone not
            // drawn this frame is not drawn; zone 0 or an unlisted zone is kept.
            if(f.zone&&f.zone<=frameTrackZoneCount&&!frameTrackZones[f.zone-1u].drawn)continue;
            bool finite=std::isfinite(f.size)&&f.size>=0.f;
            for(float v:f.position)finite=finite&&std::isfinite(v);
            for(float v:f.color)finite=finite&&std::isfinite(v);
            if(!finite)return fail(*diagnostic,VehicleWindowError50DS::frame,"nonfinite live track flare row");
            std::uint8_t color[4];
            for(U k=0;k<4;++k)color[k]=static_cast<std::uint8_t>(std::floor(std::max(0.f,std::min(255.f,f.color[k]))));
            const std::size_t slot=trackFlareFrameRows.size();   // < trackFlareCount (bounded by the loop)
            for(U v=0;v<4;++v){
                auto& out=trackFlareVertices[slot*4u+v];
                for(U k=0;k<3;++k)out.position[k]=static_cast<float>(
                    double(f.position[k])+(double(corner[v][0])*right[k]+double(corner[v][1])*up[k])*double(f.size));
                out.uv[0]=uv[v][0];out.uv[1]=uv[v][1];
                for(U k=0;k<4;++k)out.color[k]=color[k];
            }
            trackFlareFrameRows.push_back(static_cast<U>(i));
        }
        if(trackFlareFrameRows.empty())return true;
        resetPodState();coverageAa0();
        gl.useProgram(trackProgram);gl.uniform1i(gsAlphaPassUniform,0);gl.uniform1i(environmentMapUniform,0);gl.uniform1i(gsAlphaPayloadUniform,0);
        gl.uniformMatrix4fv(trackMatrixUniform,1,0,matrix);
        gl.uniform1f(trackColorScaleUniform,255.f/128.f);
        gl.uniform1i(trackUseTextureUniform,1);gl.uniform1i(trackImageUniform,0);
        gl.uniform1i(trackDiscardZeroAlphaUniform,1);
        gl.uniform1f(trackAlphaReferenceUniform,0.f);
        gl.uniform1i(trackEffectModeUniform,1);
        gl.uniform1f(trackAlphaScaleUniform,(255.f/128.f)*(255.f/128.f));
        gl.uniform1i(trackUnlitUniform,1);
        gl.uniform1i(trackSourceColorAbsentUniform,0);
        gl.uniform1i(trackUseSourceNormalsUniform,0);gl.uniform1i(trackProjectionPassUniform,0);
        gl.uniform4f(trackUvTranslationUniform,0.f,0.f,0.f,0.f);
        const auto& fog=request.track.worldFog;
        gl.uniform1i(worldFogEnabledUniform,fog.enabled?1:0);   // StripOptFog: radial fog before the blend
        gl.uniform4f(worldFogCameraUniform,worldFogCamera[0],worldFogCamera[1],worldFogCamera[2],0.f);
        gl.uniform4f(worldFogColorUniform,fog.color[0],fog.color[1],fog.color[2],1.f);
        gl.uniform4f(worldFogRangeUniform,fog.start,fog.end,0.f,0.f);
        gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
        drawGl.bindVertexArray(trackFlareVertexArray);
        drawGl.bindBuffer(arrayBuffer,trackFlareVertexBuffer);
        drawGl.bufferSubData(arrayBuffer,0,static_cast<std::intptr_t>(trackFlareFrameRows.size()*4u*sizeof(assets::Vertex48B)),
            trackFlareVertices.data());
        drawGl.bindBuffer(elementBuffer,trackFlareIndexBuffer);
        gl.enable(0x0BE2);gl.blendEquation(0x8006);gl.blendFunc(0x0302,0x0001);   // SRC_ALPHA, ONE
        gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(0);gl.disable(0x0B44);
        gl.colorMask(1,1,1,0);
        gl.disable(0x0C11);boundTrackZone=0;
        bool accepted=check("bind track flare state and upload quads");
        U boundImage=trackFlareUnresolved;
        for(std::size_t n=0;accepted&&n<trackFlareFrameRows.size();++n){
            const U row=trackFlareFrameRows[n];
            const U image=trackFlareImage[row];
            if(image!=boundImage){
                const auto* asset=trackFlareTextures.asset(image);
                if(!asset||!asset->name){accepted=fail(*diagnostic,VehicleWindowError50DS::material,"track flare image missing");break;}
                gl.bindTexture(texture2D,asset->name);boundImage=image;
            }
            bindTrackZoneScissor(flares[row].zone);
            drawGl.drawElementsBaseVertex(0x0004,6,0x1405,reinterpret_cast<const void*>(std::uintptr_t(n)*6u*sizeof(U)),0);
        }
        accepted=accepted&&check("draw track flare quads");
        if(accepted&&(++trackFlareFrames==1||trackFlareFrames%625==0))
            std::fprintf(stderr,"track_flares frames=%llu drawn=%zu of=%zu\n",
                static_cast<unsigned long long>(trackFlareFrames),trackFlareFrameRows.size(),trackFlareCount);
        gl.disable(0x0C11);boundTrackZone=0;
        gl.colorMask(1,1,1,1);gl.depthMask(1);gl.blendFunc(0x0302,0x0303);gl.disable(0x0BE2);
        drawGl.bindVertexArray(0);gl.bindTexture(texture2D,0);
        resetPodState();
        return accepted&&check("restore state after track flares");
    }

bool VehicleWindowGl50DS::Storage::deleteTrackFlareNames(VehicleWindowDiagnostic50DS& d) noexcept {
        trackFlareCount=0;trackFlareFrameRows.clear();
        MaterialTextureGlDiagnostic50EB texture;
        if(!trackFlareTextures.close(texture)){
            d.cleanupIncomplete=true;d.cleanupGlError=texture.cleanupGlError?texture.cleanupGlError:texture.glError;
            return false;
        }
        trackFlarePublication=0;
        return deleteName(trackFlareVertexArray,gl.deleteVertexArrays,d)&&
            deleteName(trackFlareVertexBuffer,gl.deleteBuffers,d)&&
            deleteName(trackFlareIndexBuffer,gl.deleteBuffers,d);
    }
