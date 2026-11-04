// Included once inside rr::platform. This is an explicitly native compute
// lowering of the 2611CC ALPHA arithmetic, not an entered source fade/sample.
namespace {
constexpr U nativeVideoImageBindingQueries50LA[]={0x8F3A,0x8F3B,0x8F3C,0x8F3D,0x8F3E,0x906E};
// NAME, LEVEL, LAYERED, LAYER, ACCESS, FORMAT.
bool nativeVideoOverlayPoliciesSupported50LA(const NativeVideoPacketOverlayPolicies50LA& p) noexcept {
    using P=NativeVideoPacketOverlayPolicies50LA;
    return p.rounding==P::Rounding::signedFloor128 && p.colorRange==P::ColorRange::clamp8 &&
        p.pabe==P::Pabe::zero && p.alphaStore==P::AlphaStore::packedSourceByte;
}
constexpr char nativeVideoOverlayCompute50LA[]=R"GLSL(#version 430 core
layout(local_size_x=8,local_size_y=8,local_size_z=1) in;
layout(rgba8,binding=0) readonly uniform image2D retainedImage;
layout(rgba8,binding=1) writeonly uniform image2D overlayImage;
uniform ivec2 extent;
uniform uint packedRgba;
int signedFloor128(int value) {
    return value>=0 ? value/128 : -((-value+127)/128);
}
void main() {
    ivec2 p=ivec2(gl_GlobalInvocationID.xy);
    if(any(greaterThanEqual(p,extent)))return;
    ivec4 source=ivec4(int(packedRgba&255u),int((packedRgba>>8)&255u),
        int((packedRgba>>16)&255u),int((packedRgba>>24)&255u));
    ivec3 destination=ivec3(floor(imageLoad(retainedImage,p).rgb*255.0+0.5));
    ivec3 product=(source.rgb-destination)*source.a;
    ivec3 rgb=destination+ivec3(signedFloor128(product.r),
        signedFloor128(product.g),signedFloor128(product.b));
    // Explicit native COLCLAMP1/PABE0/FBA0 policy; alpha is NOT blended.
    imageStore(overlayImage,p,vec4(vec3(clamp(rgb,ivec3(0),ivec3(255))),float(source.a))/255.0);
}
)GLSL";
}

bool VehicleWindowGl50DS::Storage::prepareNativeVideoOverlayFunctions50LA() noexcept {
    I major=0,minor=0,profile=0;
    gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);gl.getIntegerv(0x9126,&profile);
    if(!check("query native compute overlay context"))return false;
    if(major<4 || (major==4 && minor<5) || !(profile&1))
        return fail(*diagnostic,VehicleWindowError50DS::context,"native compute overlay requires actual desktop GL 4.5 core");
    auto& f=nativeVideoOverlayGl50LA;
    if(!load(f.createTextures,"glCreateTextures") || !load(f.textureStorage2D,"glTextureStorage2D") ||
       !load(f.textureParameteri,"glTextureParameteri") || !load(f.getTextureLevelParameteriv,"glGetTextureLevelParameteriv") ||
       !load(f.getIntegeri,"glGetIntegeri_v") || !load(f.bindImageTexture,"glBindImageTexture") ||
       !load(f.uniform1ui,"glUniform1ui") || !load(f.dispatchCompute,"glDispatchCompute") ||
       !load(f.memoryBarrier,"glMemoryBarrier"))return false;
    if(!gl.deleteTextures && !load(gl.deleteTextures,"glDeleteTextures"))return false;
    if(!gl.uniform2i && !load(gl.uniform2i,"glUniform2i"))return false;
    if(!gl.copyImageSubData && !load(gl.copyImageSubData,"glCopyImageSubData"))return false;
    I textureLimit=0,imageUnits=0,computeImages=0,invocations=0;
    gl.getIntegerv(0x0D33,&textureLimit);gl.getIntegerv(0x8F38,&imageUnits);
    gl.getIntegerv(0x91BD,&computeImages);gl.getIntegerv(0x90EB,&invocations);
    I count[3]{},size[3]{};
    for(U axis=0;axis<3;++axis){f.getIntegeri(0x91BE,axis,&count[axis]);f.getIntegeri(0x91BF,axis,&size[axis]);}
    if(!check("query actual native overlay texture and compute limits"))return false;
    const U groupsX=(U(nativeVideoPacketWidth50LA)+7u)/8u,groupsY=(U(nativeVideoPacketHeight50LA)+7u)/8u;
    if(textureLimit<nativeVideoPacketWidth50LA || textureLimit<nativeVideoPacketHeight50LA ||
       imageUnits<2 || computeImages<2 || invocations<64 || size[0]<8 || size[1]<8 || size[2]<1 ||
       count[0]<1 || count[1]<1 || count[2]<1 || groupsX>U(count[0]) || groupsY>U(count[1]))
        return fail(*diagnostic,VehicleWindowError50DS::limits,"native overlay exceeds actual image/dispatch limits");
    return true;
}

bool VehicleWindowGl50DS::Storage::allocateNativeVideoOverlay50LA() noexcept {
    auto& f=nativeVideoOverlayGl50LA;
    f.createTextures(texture2D,2,nativeVideoOverlayTextures50LA);
    if(!check("create native overlay texture names"))return false;
    for(U texture:nativeVideoOverlayTextures50LA){
        if(!texture)return fail(*diagnostic,VehicleWindowError50DS::allocation,"native overlay texture name is zero");
        f.textureStorage2D(texture,1,rgba8,nativeVideoPacketWidth50LA,nativeVideoPacketHeight50LA);
        f.textureParameteri(texture,0x2801,0x2600);f.textureParameteri(texture,0x2800,0x2600);
        f.textureParameteri(texture,0x813C,0);f.textureParameteri(texture,0x813D,0);
        if(!check("allocate immutable native overlay RGBA8 texture"))return false;
        I width=0,height=0,format=0;
        f.getTextureLevelParameteriv(texture,0,0x1000,&width);
        f.getTextureLevelParameteriv(texture,0,0x1001,&height);
        f.getTextureLevelParameteriv(texture,0,0x1003,&format);
        if(!check("inspect native overlay texture storage"))return false;
        if(width!=nativeVideoPacketWidth50LA || height!=nativeVideoPacketHeight50LA || format!=I(rgba8))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"native overlay texture extent/format mismatch");
    }
    diagnostic->shader.stage=VehicleGlShaderStage50CZ::none; // Operation names identify the compute stage.
    nativeVideoOverlayShader50LA=shaderGl.createShader(0x91B9); // COMPUTE_SHADER.
    if(!check("create native overlay compute shader"))return false;
    if(!nativeVideoOverlayShader50LA)return fail(*diagnostic,VehicleWindowError50DS::shader,"native overlay compute shader name is zero");
    const char* source=nativeVideoOverlayCompute50LA;
    shaderGl.shaderSource(nativeVideoOverlayShader50LA,1,&source,nullptr);
    shaderGl.compileShader(nativeVideoOverlayShader50LA);
    I compiled=0;shaderGl.getShaderiv(nativeVideoOverlayShader50LA,0x8B81,&compiled);
    if(!check("compile native overlay compute shader"))return false;
    if(!compiled){
        shaderGl.getShaderInfoLog(nativeVideoOverlayShader50LA,I(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
        diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
        check("read native overlay compute log");
        return fail(*diagnostic,VehicleWindowError50DS::shader,"native overlay compute compilation failed");
    }
    diagnostic->shader.stage=VehicleGlShaderStage50CZ::link;
    nativeVideoOverlayProgram50LA=shaderGl.createProgram();
    if(!check("create native overlay compute program"))return false;
    if(!nativeVideoOverlayProgram50LA)return fail(*diagnostic,VehicleWindowError50DS::shader,"native overlay compute program name is zero");
    shaderGl.attachShader(nativeVideoOverlayProgram50LA,nativeVideoOverlayShader50LA);
    shaderGl.linkProgram(nativeVideoOverlayProgram50LA);
    I linked=0;shaderGl.getProgramiv(nativeVideoOverlayProgram50LA,0x8B82,&linked);
    if(!check("link native overlay compute program"))return false;
    if(!linked){
        shaderGl.getProgramInfoLog(nativeVideoOverlayProgram50LA,I(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
        diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
        check("read native overlay compute link log");
        return fail(*diagnostic,VehicleWindowError50DS::shader,"native overlay compute linking failed");
    }
    I local[3]{};shaderGl.getProgramiv(nativeVideoOverlayProgram50LA,0x8267,local);
    nativeVideoOverlayExtent50LA=shaderGl.getUniformLocation(nativeVideoOverlayProgram50LA,"extent");
    nativeVideoOverlayRgba50LA=shaderGl.getUniformLocation(nativeVideoOverlayProgram50LA,"packedRgba");
    if(!check("inspect linked native overlay program"))return false;
    if(local[0]!=8 || local[1]!=8 || local[2]!=1 || nativeVideoOverlayExtent50LA<0 || nativeVideoOverlayRgba50LA<0)
        return fail(*diagnostic,VehicleWindowError50DS::shader,"native overlay workgroup/uniform contract mismatch");
    diagnostic->shader.stage=VehicleGlShaderStage50CZ::none;
    return true;
}

bool VehicleWindowGl50DS::prepareNativeVideoPacketOverlay(NativeVideoPacketOverlayPolicies50LA policies,
    std::size_t maximumOverlayGpuColorBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options viewport excludes native overlay preparation");
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"native overlay needs actual renderer");
    auto& s=*storage_;
    if(!s.nativeVideoPacketIdle50LA(d))return false;
    if(!s.nativeVideoPacketReady50LA || !s.nativeVideoSnapshotReady50LA)
        return fail(d,VehicleWindowError50DS::request,"native overlay needs actual prepared packet and snapshot owners");
    if(s.nativeVideoOverlayAttempted50LA)
        return fail(d,VehicleWindowError50DS::alreadyOwned,"native overlay preparation already attempted; close before retry");
    if(!nativeVideoOverlayPoliciesSupported50LA(policies))
        return fail(d,VehicleWindowError50DS::request,"native overlay requires explicit rounding, saturation, PABE0 and packed-alpha policies");
    if(s.nativeVideoPacketRgbaBytes50LA>std::numeric_limits<std::size_t>::max()/2 ||
       s.nativeVideoPacketRgbaBytes50LA*2>maximumOverlayGpuColorBytes)
        return fail(d,VehicleWindowError50DS::limits,"native overlay two-texture color budget is insufficient");
    s.nativeVideoOverlayAttempted50LA=true;s.nativeVideoOverlayPolicies50LA=policies;s.diagnostic=&d;
    const bool accepted=s.check("before native compute overlay preparation") &&
        s.prepareNativeVideoOverlayFunctions50LA() && s.allocateNativeVideoOverlay50LA();
    s.diagnostic=nullptr;
    // Direct-state allocation and compilation do not change existing bindings.
    if(accepted)s.nativeVideoOverlayReady50LA=true;
    return accepted;
}

bool VehicleWindowGl50DS::Storage::saveNativeVideoOverlayState50LA(NativeVideoOverlaySavedState50LA& saved) noexcept {
    gl.getIntegerv(0x8B8D,&saved.program); // CURRENT_PROGRAM.
    for(U unit=0;unit<2;++unit)for(unsigned field=0;field<6;++field)
        nativeVideoOverlayGl50LA.getIntegeri(nativeVideoImageBindingQueries50LA[field],unit,&saved.image[unit][field]);
    return check("save native overlay program and full image bindings");
}

bool VehicleWindowGl50DS::Storage::restoreNativeVideoOverlayState50LA(const NativeVideoOverlaySavedState50LA& saved) noexcept {
    // Replay all queried descriptors, restoring the previous bound/unbound
    // image selection. No blanket defaults from plural BindImageTextures.
    for(U unit=0;unit<2;++unit){const auto& b=saved.image[unit];
        nativeVideoOverlayGl50LA.bindImageTexture(unit,U(b[0]),b[1],std::uint8_t(b[2]!=0),b[3],U(b[4]),U(b[5]));
    }
    gl.useProgram(U(saved.program));
    return check("restore native overlay program and full image bindings");
}

bool VehicleWindowGl50DS::overlayNativeVideoPacketSnapshot(U packedRgba,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options viewport excludes native packet overlay");
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"native packet overlay needs actual renderer");
    auto& s=*storage_;
    if(!s.nativeVideoPacketIdle50LA(d))return false;
    if(!s.nativeVideoPacketReady50LA || !s.nativeVideoSnapshotReady50LA || !s.nativeVideoSnapshotImage50LA ||
       !s.nativeVideoSnapshotClearSerial50LA || !s.nativeVideoOverlayReady50LA)
        return fail(d,VehicleWindowError50DS::noImage,"native overlay requires an accepted retained snapshot and prepared compute owner");
    if(s.nativeVideoPacketSerial50LA==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::limits,"native packet image acceptance serial exhausted");
    const auto bank=std::uint8_t(s.nativeVideoPacketPublished50LA^1u);
    s.diagnostic=&d;auto& f=s.nativeVideoOverlayGl50LA;
    bool accepted=s.check("before native retained-image overlay");
    Storage::NativeVideoOverlaySavedState50LA saved;
    bool restore=false;
    if(accepted){accepted=s.saveNativeVideoOverlayState50LA(saved);restore=accepted;}
    if(accepted){
        // Both images are genuine same-context single-sample RGBA8. Source is
        // retained snapshot storage, never implicitly the last output. An
        // explicit capture may establish an earlier overlay as the snapshot.
        s.gl.copyImageSubData(s.nativeVideoSnapshotColors50LA[s.nativeVideoSnapshotPublished50LA],renderbuffer,0,0,0,0,
            s.nativeVideoOverlayTextures50LA[0],texture2D,0,0,0,0,s.nativeVideoPacketWidth50LA,s.nativeVideoPacketHeight50LA,1);
        accepted=s.check("copy retained native snapshot into distinct compute input");
    }
    if(accepted){
        s.gl.useProgram(s.nativeVideoOverlayProgram50LA);
        f.bindImageTexture(0,s.nativeVideoOverlayTextures50LA[0],0,0,0,0x88B8,rgba8); // READ_ONLY.
        f.bindImageTexture(1,s.nativeVideoOverlayTextures50LA[1],0,0,0,0x88B9,rgba8); // WRITE_ONLY.
        s.gl.uniform2i(s.nativeVideoOverlayExtent50LA,s.nativeVideoPacketWidth50LA,s.nativeVideoPacketHeight50LA);
        f.uniform1ui(s.nativeVideoOverlayRgba50LA,packedRgba);
        accepted=s.check("bind native compute overlay images and numeric RGBA bytes");
    }
    if(accepted){
        f.dispatchCompute((U(s.nativeVideoPacketWidth50LA)+7u)/8u,(U(s.nativeVideoPacketHeight50LA)+7u)/8u,1);
        accepted=s.check("dispatch native source-formula overlay");
    }
    if(accepted){
        // Orders actual shader writes before API image copy and later reuse.
        // This is a native memory dependency, NOT CPU/GPU/source completion.
        f.memoryBarrier(0xffffffffu); // ALL_BARRIER_BITS.
        accepted=s.check("order native overlay shader image writes before copy");
    }
    if(accepted){
        s.gl.copyImageSubData(s.nativeVideoOverlayTextures50LA[1],texture2D,0,0,0,0,
            s.nativeVideoPacketColors50LA[bank],renderbuffer,0,0,0,0,s.nativeVideoPacketWidth50LA,s.nativeVideoPacketHeight50LA,1);
        accepted=s.check("copy native overlay output into unpublished packet color bank");
    }
    if(accepted){
        // Reuse hazard: a later dispatch must not overwrite this output image
        // while the preceding API copy is still reading it. The earlier
        // post-dispatch barrier alone cannot order accesses after that barrier.
        f.memoryBarrier(0x20u); // SHADER_IMAGE_ACCESS_BARRIER_BIT.
        accepted=s.check("order native output-copy reads before future shader image stores");
    }
    if(restore){const bool restored=s.restoreNativeVideoOverlayState50LA(saved);accepted=accepted && restored;}
    s.diagnostic=nullptr;
    if(!accepted)return false;
    s.nativeVideoPacketPublished50LA=bank;s.nativeVideoPacketImage50LA=true;++s.nativeVideoPacketSerial50LA;
    s.nativeVideoPacketClearAncestor50LA=s.nativeVideoSnapshotClearSerial50LA;
    s.nativeVideoPacketLastWrite50LA=NativeVideoPacketWrite50LA::overlay;
    s.nativeVideoOverlayImageSerial50LA=s.nativeVideoPacketSerial50LA;
    s.nativeVideoOverlayCaptureSerial50LA=s.nativeVideoSnapshotSerial50LA;s.nativeVideoOverlayPackedRgba50LA=packedRgba;
    return true;
}

bool VehicleWindowGl50DS::overlayNativeVideoPacketBlockingColor(
    const game::OriginalVideoFadeColorCoefficients50LA& color,std::int32_t startAlpha,
    std::int32_t currentAlpha,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};std::uint64_t rgbaq=0;
    // Pure numeric admission before any GL work. No source state, sample,
    // clock, ordinal, timer or readiness is manufactured from these inputs.
    if(!game::computeOriginalVideoBlockingColor50LA(color,startAlpha,currentAlpha,rgbaq))
        return fail(d,VehicleWindowError50DS::request,"native blocking-color arithmetic is outside the admitted finite nearest-rounding subset");
    // Preserve the actual low word after unmasked signed OR packing. This
    // is not a clean per-lane clamp/conversion. Untextured native blend ignores Q.
    return overlayNativeVideoPacketSnapshot(std::uint32_t(rgbaq),d);
}

bool VehicleWindowGl50DS::nativeVideoPacketOverlayInfo(NativeVideoPacketOverlayInfo50LA& out) const noexcept {
    out={};
    if(!initialized())return false;
    const auto& s=*storage_;
    if(s.thread!=std::this_thread::get_id() || !s.nativeVideoOverlayReady50LA || s.nativeGlFaultObserved50KG)return false;
    out.policies=s.nativeVideoOverlayPolicies50LA;out.gpuColorBytes=s.nativeVideoPacketRgbaBytes50LA*2;
    out.acceptedImageSerial=s.nativeVideoOverlayImageSerial50LA;out.snapshotCaptureSerial=s.nativeVideoOverlayCaptureSerial50LA;
    out.packedRgba=s.nativeVideoOverlayPackedRgba50LA;out.commandsAccepted=out.acceptedImageSerial!=0;
    out.stillCurrent=out.commandsAccepted && s.nativeVideoPacketImage50LA &&
        s.nativeVideoPacketSerial50LA==out.acceptedImageSerial && s.nativeVideoPacketLastWrite50LA==NativeVideoPacketWrite50LA::overlay;
    return true;
}

bool VehicleWindowGl50DS::Storage::deleteNativeVideoOverlay50LA(VehicleWindowDiagnostic50DS& d) noexcept {
    nativeVideoOverlayReady50LA=false;nativeVideoOverlayImageSerial50LA=nativeVideoOverlayCaptureSerial50LA=0;
    if(nativeVideoOverlayTextures50LA[0] || nativeVideoOverlayTextures50LA[1]){
        auto& f=nativeVideoOverlayGl50LA;
        if(!f.getIntegeri || !f.bindImageTexture || !gl.deleteTextures)
            return fail(d,VehicleWindowError50DS::cleanup,"native overlay texture cleanup lacks its actual function owner");
        // A failed state restore may leave our private textures bound. Do not
        // disturb unrelated image bindings. There is no external name borrower.
        for(U unit=0;unit<2;++unit){
            I name=0;f.getIntegeri(0x8F3A,unit,&name);
            U error=drawGl.getError();
            if(error){nativeGlFaultObserved50KG=true;if(!d.cleanupGlError)d.cleanupGlError=error;return false;}
            if(name && (U(name)==nativeVideoOverlayTextures50LA[0] || U(name)==nativeVideoOverlayTextures50LA[1])){
                f.bindImageTexture(unit,0,0,0,0,0x88B8,rgba8);
                error=drawGl.getError();
                if(error){nativeGlFaultObserved50KG=true;if(!d.cleanupGlError)d.cleanupGlError=error;return false;}
            }
        }
    }
    if(!deleteShaderName(nativeVideoOverlayProgram50LA,true,d) ||
       !deleteShaderName(nativeVideoOverlayShader50LA,false,d))return false;
    for(auto& name:nativeVideoOverlayTextures50LA)if(!deleteName(name,gl.deleteTextures,d))return false;
    return true;
}
