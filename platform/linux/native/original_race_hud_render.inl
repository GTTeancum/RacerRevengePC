// Included inside VehicleWindowGl50DS::Storage. Native bounded gameplay layer;
// no guest packets, guest execution, host font or caller-created readiness.
struct RaceHud {
    static constexpr U maximumQuads=164,maximumPackets=8;
    struct Packet {U first=0,count=0,material=0;};
    const game::OriginalRaceHud50KG* owner=nullptr;
    const game::OriginalCameraViewportFields* viewport=nullptr;
    std::uint64_t scene=0,generation=0,publication=0;
    MaterialTextureGl50EB textures;
    std::array<MaterialTextureSampler50EB,4> samplers{};
    std::array<assets::OriginalUiNativeMaterial50KG,7> policies{};
    std::array<U,7> ranks{};
    std::array<assets::Vertex48B,maximumQuads*4> vertices{};
    std::array<Packet,maximumPackets> packets{};
    game::OriginalUiTextLayout50KG glyphs{};
    U target=0,color=0,depth=0,quads=0,packetCount=0;
    bool ready=false,frame=false;
};
std::unique_ptr<RaceHud> raceHud;
bool raceHudPreparing=false,raceHudUiCreationFailed=false;
bool hasRaceHudResources() const noexcept {
    return raceHud&&(raceHud->target||raceHud->color||raceHud->depth||raceHud->textures.ownsResources());
}
static float hudMul(float a,float b) noexcept {volatile float v=a*b;return v;}
static float hudAdd(float a,float b) noexcept {volatile float v=a+b;return v;}
static float hudSub(float a,float b) noexcept {volatile float v=a-b;return v;}
static float hudDiv(float a,float b) noexcept {volatile float v=a/b;return v;}
static bool hudName(const std::uint8_t* p,std::size_t n,const char* expected) noexcept {
    std::size_t i=0;for(;i<n&&expected[i];++i){char c=char(p[i]);
        if(c=='\\')c='/';if(c>='A'&&c<='Z')c=char(c+32);
        if(c!=expected[i])return false;}
    return i==n&&!expected[i];
}
static bool hudSampler(const assets::OriginalUiNativeMaterial50KG& p,MaterialTextureSampler50EB& s) noexcept {
    if(p.minification!=p.magnification||p.environmentContext||p.depthMethod!=2||!p.depthWrite||
        p.colorScale!=255.f/128.f||p.storedAlphaScale!=255.f/128.f)return false;
    s.minification=s.magnification=p.minification==assets::OriginalUiMaterialFilter50KG::nearest?
        MaterialTextureFilter50EB::nearest:MaterialTextureFilter50EB::linear;
    s.s=p.u==assets::OriginalUiMaterialWrap50KG::repeat?MaterialTextureWrap50EB::repeat:MaterialTextureWrap50EB::clampToEdge;
    s.t=p.v==assets::OriginalUiMaterialWrap50KG::repeat?MaterialTextureWrap50EB::repeat:MaterialTextureWrap50EB::clampToEdge;
    return true;
}
bool prepareRaceHudResources(const game::OriginalRaceHud50KG& owner) noexcept {
    if(raceHud||frontendOnly||active||frameSet||!owner.loaded()||owner.viewCount()!=1)
        return fail(*diagnostic,VehicleWindowError50DS::request,"fresh single-view original race HUD required");
    raceHud.reset(new(std::nothrow) RaceHud);
    if(!raceHud)return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate original race HUD storage");
    auto& h=*raceHud;const auto* t=owner.textAt(0,0x12c);const auto* images=owner.images();
    const auto* bank=images?images->textures():nullptr;
    if(!t||!t->viewport||!t->viewport->prepared||!t->viewport->sceneKey||!t->viewport->sessionGeneration||
        t->viewport->x1E0||t->viewport->y1E4||t->viewport->width1E8!=640||t->viewport->height1EC!=448||!bank||bank->textureCount()!=4)
        return fail(*diagnostic,VehicleWindowError50DS::assets,"original HUD viewport and four authentic images required");
    h.owner=&owner;h.viewport=t->viewport;h.scene=t->viewport->sceneKey;h.generation=t->viewport->sessionGeneration;
    constexpr const char* names[]={"font/denmarkbold14gleam","font/denmarkbold16ingame","font/denmarkbold30gleam",
        "font/denmarkbold16explosion","font/denmarkbold30explosion","ui/textures/gleam","ui/textures/gleamclear"};
    const auto table=owner.materialRegistry();
    if(!table.identity||!table.data||table.size!=70962)
        return fail(*diagnostic,VehicleWindowError50DS::assets,"original HUD material registry unavailable");
    std::array<bool,7> found{};std::size_t cursor=0;U token=0;
    const auto delimiter=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    while(cursor<table.size){while(cursor<table.size&&delimiter(table.data[cursor]))++cursor;
        if(cursor==table.size)break;const auto start=cursor;
        while(cursor<table.size&&!delimiter(table.data[cursor]))++cursor;
        for(U i=0;i<7;++i)if(hudName(table.data+start,cursor-start,names[i])){
            if(found[i])return fail(*diagnostic,VehicleWindowError50DS::material,"duplicate original HUD registration");
            found[i]=true;h.ranks[i]=token;}
        ++token;
    }
    for(bool value:found)if(!value)return fail(*diagnostic,VehicleWindowError50DS::material,"missing original HUD registration");
    if(token!=3235||!(h.ranks[6]<h.ranks[0]&&h.ranks[0]<h.ranks[2]&&h.ranks[2]<h.ranks[5]))
        return fail(*diagnostic,VehicleWindowError50DS::material,"original clear, glyph and gleam order differs");
    assets::OriginalUiMaterialDiagnostic50KG materialDiagnostic;
    for(U i: {0u,1u,2u}){const auto* m=owner.material(i);const auto* decoded=images->decodedTexture(i);
        if(!m||m->passes.size()!=1||!decoded||
            !assets::prepareOriginalUiNativeMaterial50KG(m->passes[0],*decoded,h.policies[i],materialDiagnostic)||
            h.policies[i].blend!=assets::OriginalUiMaterialBlend50KG::sourceAlpha||!hudSampler(h.policies[i],h.samplers[i]))
            return fail(*diagnostic,VehicleWindowError50DS::material,"original HUD font policy rejected");
        const auto& group=m->passes[0].shaderPassGroup;
        if(!hudName(reinterpret_cast<const std::uint8_t*>(group.data()),group.size(),i==1?"none":"gleam"))
            return fail(*diagnostic,VehicleWindowError50DS::material,"original HUD font group differs");
    }
    for(U i:{5u,6u}){const auto* m=owner.material(i);assets::OriginalRaceHudGleamMaterial50KG policy;
        if(!m||m->passes.size()!=1||!assets::prepareOriginalRaceHudGleamMaterial50KG(m->passes[0],*images,3,policy,materialDiagnostic)||
            policy.clearFramebufferOnly!=(i==6)||!hudSampler(policy.raster,h.samplers[3]))
            return fail(*diagnostic,VehicleWindowError50DS::material,"original stored-alpha gleam policy rejected");
        h.policies[i]=policy.raster;
    }
    // The same gleam image has exactly one immutable sampler in both passes.
    const auto& a=h.policies[5];const auto& b=h.policies[6];
    if(a.minification!=b.minification||a.magnification!=b.magnification||a.u!=b.u||a.v!=b.v)
        return fail(*diagnostic,VehicleWindowError50DS::material,"original gleam sampler conflict");
    // A texture-feedback path is needed only for this native private target.
    // Do not change the already compiled UI program's coherent-fetch variant.
    if(!uiSourceCoherentFramebufferFetch){
        I major=0,minor=0,units=0,count=0;gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);
        gl.getIntegerv(0x8872,&units);bool barrier=major>4||(major==4&&minor>=5);
        if(!load(gl.getStringi,"glGetStringi")||!check("query original HUD source-alpha capabilities"))return false;
        gl.getIntegerv(0x821D,&count);
        if(!check("query original HUD extensions")||count<0||count>65536||units<2)
            return fail(*diagnostic,VehicleWindowError50DS::context,"bounded original HUD source-alpha support unavailable");
        for(I i=0;i<count;++i){const auto* p=gl.getStringi(0x1F03,U(i));
            if(!check("read original HUD extension")||!p)return false;
            if(!std::strcmp(reinterpret_cast<const char*>(p),"GL_ARB_texture_barrier"))barrier=true;}
        if(!barrier||!load(gl.textureBarrier,"glTextureBarrier"))
            return fail(*diagnostic,VehicleWindowError50DS::context,"original HUD requires GL4.5 or ARB_texture_barrier");
    }
    if(!load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
        !load(gl.scissor,"glScissor")||!load(gl.activeTexture,"glActiveTexture")||!load(gl.bindTexture,"glBindTexture")||
        !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
        !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||!load(gl.framebufferTexture2D,"glFramebufferTexture2D")||
        !load(gl.blitFramebuffer,"glBlitFramebuffer"))return false;
    MaterialTextureGlRequest50EB requestTextures{};requestTextures.context=request.context;
    requestTextures.originalUploadBinding=request.originalUploadBinding;
    requestTextures.isCurrent=request.isCurrent;requestTextures.samplers=h.samplers.data();requestTextures.samplerCount=4;
    requestTextures.maximumGpuBytes=16u*1024u*1024u;const char* missing=nullptr;
    MaterialTextureGlDiagnostic50EB textureDiagnostic;
    if(!loadMaterialTextureFunctions50EE(request.context,request.procAddress,requestTextures.gl,missing)||
        !h.textures.create(requestTextures,*bank,textureDiagnostic))
        return fail(*diagnostic,VehicleWindowError50DS::material,"upload authentic original HUD images");
    h.publication=h.textures.publicationToken();
    if(!uiProgram){
        // A failed late compilation can retain partially created shared names.
        // They remain renderer-owned for close; never reuse them as readiness.
        raceHudUiCreationFailed=true;raceHudPreparing=true;
        const bool okay=createOriginalUi();raceHudPreparing=false;if(!okay)return false;
        raceHudUiCreationFailed=false;
    }
    I limit=0;gl.getIntegerv(0x0D33,&limit);
    if(!check("query original HUD texture extent")||limit<maximumWidth||limit<maximumHeight)
        return fail(*diagnostic,VehicleWindowError50DS::limits,"original HUD private target exceeds texture limit");
    gl.genFramebuffers(1,&h.target);gl.bindFramebuffer(framebuffer,h.target);
    gl.genTextures(1,&h.color);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,h.color);
    gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
    gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
    gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);drawGl.bindBuffer(0x88EC,0);
    gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
    if(!check("allocate native-size original HUD source target")||!h.target||!h.color)return false;
    I w=0,height=0,format=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
    gl.getTexLevelParameteriv(texture2D,0,0x1001,&height);gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
    if(!check("verify original HUD source storage")||w!=maximumWidth||height!=maximumHeight||format!=I(rgba8))return false;
    gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,h.color,0);
    if(!createAttachment(h.depth,depth24,depthAttachment))return false;
    gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    const U status=gl.checkFramebufferStatus(framebuffer);gl.bindTexture(texture2D,0);gl.bindFramebuffer(framebuffer,0);
    if(!check("complete original HUD private target")||status!=framebufferComplete){diagnostic->framebufferStatus=status;return false;}
    h.ready=true;return true;
}
bool prepareRaceHudFrame(const game::OriginalRaceHud50KG* owner) noexcept {
    if(raceHud){raceHud->frame=false;raceHud->quads=raceHud->packetCount=0;}
    if(!owner)return true;
    if(!raceHud||!raceHud->ready||owner!=raceHud->owner)return false;
    auto& h=*raceHud;const auto* v=h.viewport;
    if(!owner->loaded()||owner->viewCount()!=1||!v||!v->prepared||v->sceneKey!=h.scene||
        v->sessionGeneration!=h.generation||v->x1E0||v->y1E4||v->width1E8!=640||v->height1EC!=448||
        !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    const auto* images=owner->images();const auto* bank=images?images->textures():nullptr;if(!bank)return false;
    // These four objects are the only joined lap emitters. Never accidentally
    // render countdown, status, numeric-count, KO or an inactive second view.
    constexpr U offsets[]={0x12c,0x130,0x134,0x138,0x13c,0x140,0x144,0x164,0x16c,0x170,0x17c};
    for(U offset:offsets){const auto* t=owner->textAt(0,offset);if(!t||t->viewport!=v)return false;
        if(t->enabled70&&offset!=0x12c&&offset!=0x130&&offset!=0x134&&offset!=0x164)return false;}
    const auto put=[&](float x,float y,float u,float vv,const std::array<std::uint8_t,4>& color,U index){
        if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(u)||!std::isfinite(vv)||color[3]>128)return false;
        auto& vertex=h.vertices[index];vertex={};vertex.position[0]=x;vertex.position[1]=y;vertex.position[2]=16777214.f;
        vertex.uv[0]=u;vertex.uv[1]=vv;for(U c=0;c<4;++c)vertex.color[c]=color[c];return true;};
    // 2E6960 prepares both embedded ScreenTextures, then enqueues bright and
    // clear. Their real registry leaves drain clear before text before bright.
    for(U offset:{0x184u,0x194u}){const auto* g=owner->gleamAt(0,offset);if(!g||g->viewport!=v)return false;
        if(!g->active11C)continue;
        for(float f:{g->x104,g->y100,g->width108,g->height10C,g->travel110,g->offset114,g->duration120,g->elapsed124,g->angle118})
            if(!std::isfinite(f))return false;
        if(g->width108!=30||!(g->height10C==50||g->height10C==100)||g->offset114!=0||g->duration120!=.75f||
            g->elapsed124<0||g->elapsed124>g->duration120||g->angle118!=.523598730564117431640625f)return false;
        const float ratio=hudDiv(g->elapsed124,g->duration120);
        const float x=hudAdd(g->x104,hudMul(g->travel110,ratio)),y=hudAdd(g->y100,hudMul(g->offset114,ratio));
        const float cx=hudAdd(hudMul(.5f,g->width108),x),cy=hudAdd(hudMul(.5f,g->height10C),y);
        game::VisualVector49R trig{};
        if(!game::detail::rotateVector49U({{0,0,1,0}},{{1,0,0,0}},g->angle118,trig))return false;
        const float cosine=trig.v[0],sine=trig.v[1];
        const float xs[]={x,hudAdd(x,g->width108),hudAdd(x,g->width108),x};
        const float ys[]={y,y,hudAdd(y,g->height10C),hudAdd(y,g->height10C)};
        constexpr U sourceLane[]={0,1,3,2};constexpr float uv[4][2]={{0,0},{1,0},{1,1},{0,1}};
        for(U material:{5u,6u}){const auto first=h.quads;
            if(h.quads>=RaceHud::maximumQuads||h.packetCount>=RaceHud::maximumPackets)return false;
            for(U i=0;i<4;++i){const float dx=hudSub(xs[i],cx),dy=hudSub(ys[i],cy);
                const float px=hudAdd(cx,hudSub(hudMul(dx,cosine),hudMul(dy,sine)));
                const float py=hudAdd(cy,hudAdd(hudMul(dx,sine),hudMul(dy,cosine)));
                const auto& color=material==5?g->brightColor[sourceLane[i]]:g->clearColor[sourceLane[i]];
                // Clear coverage must dominate every later destination-alpha
                // read. Preserve source zero packet RGBA, not a host opacity.
                if(material==6&&color!=std::array<std::uint8_t,4>{{0,0,0,0}})return false;
                if(!put(px,py,uv[i][0],uv[i][1],color,h.quads*4+i))return false;}
            ++h.quads;h.packets[h.packetCount++]={first,1,material};}
    }
    for(U offset:{0x12cu,0x130u,0x134u,0x164u}){const auto& t=*owner->textAt(0,offset);
        if(!t.enabled70||!t.blinkOn80)continue;
        const U font=offset==0x164?2u:0u;
        if(t.fontIndex!=font||t.materialIndex!=font||t.textLength>40||t.text[t.textLength]!=0||t.justification74!=3||
            t.rotation17C!=0||!std::isfinite(t.scale178)||t.scale178<=0||!std::isfinite(t.alpha174)||t.alpha174<0||t.alpha174>255)return false;
        const auto* atlas=bank->texture(font);const auto* f=images->font(font);if(!atlas||!f)return false;
        game::OriginalUiTextInput50KG in{f,&atlas->texture,reinterpret_cast<const std::uint8_t*>(t.text.data()),t.textLength,0,0};
        game::OriginalUiTextDiagnostic50KG d;
        if(!game::layoutOriginalUiText50KG(in,h.glyphs,d))return false;
        I measuredWidth=0,measuredHeight=0;
        if(!game::roundOriginalUiTextMetric50KG(h.glyphs.widthBeforeScaleAndRounding,t.scale178,measuredWidth)||
            !game::roundOriginalUiTextMetric50KG(h.glyphs.firstGlyphHeightBeforeScaleAndRounding,t.scale178,measuredHeight))return false;
        //2E13A0 CVT.W.S follows scaled integer metrics;2E0A30 divides the
        //positioned origin before the genuine CEL half-pixel glyph arithmetic.
        float x=float(t.position[0]),y=float(t.position[1]);
        if(t.textLength){x=hudSub(x,hudMul(float(measuredWidth),.5f));y=hudSub(y,hudMul(float(measuredHeight),.5f));}
        I px=0,py=0;if(!game::roundOriginalUiTextMetric50KG(x,1,px)||!game::roundOriginalUiTextMetric50KG(y,1,py))return false;
        in.localX=hudDiv(float(px),t.scale178);in.localY=hudDiv(float(py),t.scale178);
        if(!game::layoutOriginalUiText50KG(in,h.glyphs,d)||h.quads+h.glyphs.count>RaceHud::maximumQuads)return false;
        if(!h.glyphs.count)continue;
        if(h.packetCount>=RaceHud::maximumPackets)return false;
        const auto color=game::originalUiTextPacketColor50KG(t.color170,static_cast<std::int16_t>(t.alpha174));
        const U first=h.quads;
        for(U q=0;q<h.glyphs.count;++q){for(U i=0;i<4;++i){const auto& vertex=h.glyphs.quads[q].vertices[i];
                if(!put(hudMul(vertex.x,t.scale178),hudMul(vertex.y,t.scale178),vertex.u,vertex.v,color,h.quads*4+i))return false;}
            ++h.quads;}
        h.packets[h.packetCount++]={first,h.quads-first,font};
    }
    // Stable insertion sort on the authenticated registry leaf rank. The three
    // font14 packets retain their actual object FIFO within that shared leaf.
    for(U i=1;i<h.packetCount;++i){auto p=h.packets[i];U j=i;
        while(j&&h.ranks[h.packets[j-1].material]>h.ranks[p.material]){h.packets[j]=h.packets[j-1];--j;}h.packets[j]=p;}
    h.frame=h.quads!=0;return true;
}
bool drawRaceHud(U bank) noexcept {
    if(!raceHud||!raceHud->frame)return true;
    auto& h=*raceHud;
    if(!h.ready||!h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    // Keep the native image at its real drawable resolution. Only this private
    // scratch target receives raw source alpha and a separate UI depth plane.
    gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);
    gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
    gl.bindFramebuffer(drawFramebuffer,h.target);gl.drawBuffer(colorAttachment);
    gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
    gl.bindFramebuffer(framebuffer,h.target);gl.viewport(0,0,width,height);
    gl.depthMask(1);gl.clearDepth(0);gl.clear(depthBufferBit);
    // Initial scratch alpha is deliberately irrelevant: every bright sweep's
    // entire raster coverage is first written by its identical clear quad.
    // Glyph source-over never reads Ad. No genuine gameplay alpha is claimed.
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
    bool okay=check("prepare authentic lap glyph/gleam source pass");
    constexpr U image[]={0,1,2,1,2,3,3};
    for(U p=0;okay&&p<h.packetCount;++p){const auto& packet=h.packets[p];
        // Original clear's failed alpha test keeps framebuffer writes while
        // suppressing depth writes. Its exact zero RGBA leaves RGB untouched
        // and replaces source A with zero through the ordinary source equation.
        gl.depthMask(packet.material==6?0:1);gl.uniform1i(uiSourceBlendUniform,packet.material==5?3:1);
        gl.bindTexture(texture2D,h.textures.texture(image[packet.material]));
        if(uiSourceCoherentFramebufferFetch){
            drawGl.drawElementsBaseVertex(0x0004,I(packet.count*6),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(packet.first)*6*sizeof(U)),0);
            okay=check("draw ordered original HUD source leaf");
        }else{
            for(U i=packet.first*6;okay&&i<(packet.first+packet.count)*6;i+=3){gl.textureBarrier();
                drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(i)*sizeof(U)),0);
                okay=check("draw ordered original HUD source triangle");}
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
        okay=check("copy authentic original HUD into pending gameplay image");
    }
    gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    // This native presentation owns opaque alpha. Source Ad stays private and
    // never becomes the input to a later gameplay effect or retained UI frame.
    gl.colorMask(0,0,0,1);gl.clearColor(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
    gl.depthMask(1);gl.clearDepth(1);gl.depthFunc(0x0201);gl.useProgram(shader->program());
    drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after original HUD pass");
    if(okay&&restored){diagnostic->originalRaceHudQuads=h.quads;diagnostic->originalRaceHudLeaves=h.packetCount;}
    return okay&&restored;
}
bool deleteRaceHudNames(VehicleWindowDiagnostic50DS& d) noexcept {
    if(!raceHud)return true;auto& h=*raceHud;h.ready=h.frame=false;
    MaterialTextureGlDiagnostic50EB td;
    if(!h.textures.close(td)){if(td.cleanupIncomplete){d.cleanupIncomplete=true;return false;}
        if(!d.cleanupGlError)d.cleanupGlError=td.glError?td.glError:td.cleanupGlError;}
    if(!deleteName(h.target,gl.deleteFramebuffers,d)||!deleteName(h.color,gl.deleteTextures,d)||
        !deleteName(h.depth,gl.deleteRenderbuffers,d))return false;
    raceHud.reset();return true;
}
