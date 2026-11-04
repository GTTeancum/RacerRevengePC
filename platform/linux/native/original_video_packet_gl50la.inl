// Included once inside namespace rr::platform by vehicle_window_gl50ds.cpp.
// Native lowering of the exact decoded 25CAD0 full-color clear. It does not
// execute the source call, submit its DMA, advance startup or certify a mode.
namespace {
using NativeVideoPacketCpu50LA=OriginalVideoPacketCpuRenderer50LA;
constexpr U nativeVideoClearCapabilities50LA[]={0x0C11,0x0BD0,0x8DB9,0x8C89};
// SCISSOR_TEST, DITHER, FRAMEBUFFER_SRGB, RASTERIZER_DISCARD.
bool nativeVideoPacketFullClear50LA(const NativeVideoPacketCpu50LA::DecodedDraw& draw) noexcept {
    const auto& r=draw.rectangle();
    return draw.width() && draw.height() && !r.left && !r.top &&
        r.right==draw.width() && r.bottom==draw.height() &&
        draw.width()<=std::uint32_t(std::numeric_limits<I>::max()) &&
        draw.height()<=std::uint32_t(std::numeric_limits<I>::max());
}
}

bool VehicleWindowGl50DS::Storage::nativeVideoPacketIdle50LA(VehicleWindowDiagnostic50DS& d) const noexcept {
    if(!current(d))return false;
    if(active || frameSet || diagnostic || nativeGlFaultObserved50KG ||
       garagePreludeOwner50KG || garagePollFence50KG || nativeF8BarPreparing ||
       frameNativeF8Bars || nativeF8BarFrame || nativeF8BarRenderer ||
       nativeResultsDrawPreparing || frameNativeResults || nativeResultsFrame || nativeResultsRenderer)
        return fail(d,VehicleWindowError50DS::frame,"native packet clear requires idle fault-free renderer without pending borrowers");
    return true;
}

bool VehicleWindowGl50DS::Storage::prepareNativeVideoPacketFunctions50LA() noexcept {
    if(!gl.clearBufferfv && !load(gl.clearBufferfv,"glClearBufferfv"))return false;
    if(!load(nativeVideoColorMaski50LA,"glColorMaski") ||
       !load(nativeVideoGetBooleani50LA,"glGetBooleani_v"))return false;
    I major=0,minor=0;
    gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);
    if(!check("query native packet context version"))return false;
    nativeVideoIndexedScissor50LA=major>4 || (major==4 && minor>=1);
    if(!nativeVideoIndexedScissor50LA){
        if(!gl.getStringi && !load(gl.getStringi,"glGetStringi"))return false;
        I count=0;gl.getIntegerv(0x821D,&count);
        if(!check("query native packet extension count"))return false;
        if(count<0 || count>65536)
            return fail(*diagnostic,VehicleWindowError50DS::limits,"native packet extension count outside bounded enumeration");
        for(I i=0;i<count;++i){
            const auto* extension=gl.getStringi(0x1F03,U(i));
            if(!check("query native packet viewport-array extension"))return false;
            if(!extension)return fail(*diagnostic,VehicleWindowError50DS::context,"native packet extension string is absent");
            if(std::strcmp(reinterpret_cast<const char*>(extension),"GL_ARB_viewport_array")==0){
                nativeVideoIndexedScissor50LA=true;break;
            }
        }
    }
    if(nativeVideoIndexedScissor50LA &&
       (!load(nativeVideoEnablei50LA,"glEnablei") || !load(nativeVideoDisablei50LA,"glDisablei") ||
        !load(nativeVideoIsEnabledi50LA,"glIsEnabledi")))return false;
    return true;
}

bool VehicleWindowGl50DS::Storage::saveNativeVideoPacketState50LA(NativeVideoPacketSavedState50LA& state) noexcept {
    gl.getIntegerv(0x8CA6,&state.draw);gl.getIntegerv(0x8CAA,&state.read);
    gl.getIntegerv(0x8CA7,&state.renderbuffer);
    nativeVideoGetBooleani50LA(0x0C23,0,state.colorMask); // COLOR_WRITEMASK[0].
    if(nativeVideoIndexedScissor50LA)state.enabled[0]=nativeVideoIsEnabledi50LA(0x0C11,0);
    else gl.getIntegerv(0x0C11,&state.enabled[0]);
    for(unsigned i=1;i<4;++i)gl.getIntegerv(nativeVideoClearCapabilities50LA[i],&state.enabled[i]);
    return check("save native packet clear bindings and indexed state");
}

bool VehicleWindowGl50DS::Storage::restoreNativeVideoPacketState50LA(const NativeVideoPacketSavedState50LA& state) noexcept {
    gl.bindFramebuffer(drawFramebuffer,U(state.draw));gl.bindFramebuffer(readFramebuffer,U(state.read));
    gl.bindRenderbuffer(renderbuffer,U(state.renderbuffer));
    nativeVideoColorMaski50LA(0,state.colorMask[0],state.colorMask[1],state.colorMask[2],state.colorMask[3]);
    if(nativeVideoIndexedScissor50LA){
        if(state.enabled[0])nativeVideoEnablei50LA(0x0C11,0);else nativeVideoDisablei50LA(0x0C11,0);
    }else if(state.enabled[0])gl.enable(0x0C11);else gl.disable(0x0C11);
    for(unsigned i=1;i<4;++i){
        if(state.enabled[i])gl.enable(nativeVideoClearCapabilities50LA[i]);
        else gl.disable(nativeVideoClearCapabilities50LA[i]);
    }
    return check("restore native packet clear bindings and indexed state");
}

bool VehicleWindowGl50DS::Storage::allocateNativeVideoPacketPair50LA(U (&targets)[2],U (&colors)[2]) noexcept {
    I maximum=0;gl.getIntegerv(0x84E8,&maximum); // MAX_RENDERBUFFER_SIZE.
    if(!check("query native packet renderbuffer limit"))return false;
    if(maximum<nativeVideoPacketWidth50LA || maximum<nativeVideoPacketHeight50LA)
        return fail(*diagnostic,VehicleWindowError50DS::limits,"decoded packet extent exceeds actual renderbuffer limit");
    for(U b=0;b<2;++b){
        gl.genFramebuffers(1,&targets[b]);
        if(!check("allocate native packet framebuffer name"))return false;
        if(!targets[b])
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"native packet framebuffer name is zero");
        gl.bindFramebuffer(framebuffer,targets[b]);
        if(!check("bind private native packet framebuffer"))return false;
        gl.genRenderbuffers(1,&colors[b]);
        if(!check("allocate native packet renderbuffer name"))return false;
        if(!colors[b])
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"native packet renderbuffer name is zero");
        gl.bindRenderbuffer(renderbuffer,colors[b]);
        gl.renderbufferStorage(renderbuffer,rgba8,nativeVideoPacketWidth50LA,nativeVideoPacketHeight50LA);
        if(!check("allocate native packet RGBA8 storage"))return false;
        I width=0,height=0,format=0,samples=0;
        gl.getRenderbufferParameteriv(renderbuffer,0x8D42,&width);
        gl.getRenderbufferParameteriv(renderbuffer,0x8D43,&height);
        gl.getRenderbufferParameteriv(renderbuffer,0x8D44,&format);
        gl.getRenderbufferParameteriv(renderbuffer,0x8CAB,&samples);
        if(!check("inspect native packet RGBA8 storage"))return false;
        if(width!=nativeVideoPacketWidth50LA || height!=nativeVideoPacketHeight50LA || format!=I(rgba8) || samples)
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"native packet storage extent/format/sample mismatch");
        gl.framebufferRenderbuffer(framebuffer,colorAttachment,renderbuffer,colors[b]);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);
        if(!check("inspect native packet framebuffer"))return false;
        if(status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"native packet framebuffer incomplete");}
    }
    return true;
}

bool VehicleWindowGl50DS::prepareNativeVideoPacketClear(
    const game::OriginalVideoClearPacket25CAD0Native50LA& packet,
    NativeVideoPacketCpu50LA::NativePolicies policies,std::size_t maximumGpuColorBytes,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options viewport excludes native packet preparation");
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"native packet preparation needs actual renderer");
    auto& s=*storage_;
    if(!s.nativeVideoPacketIdle50LA(d))return false;
    if(s.nativeVideoPacketAttempted50LA)
        return fail(d,VehicleWindowError50DS::alreadyOwned,"native packet preparation already attempted; close renderer before retry");
    auto decoded=NativeVideoPacketCpu50LA::decode(packet);
    if(decoded.status!=NativeVideoPacketCpu50LA::Status::decoded || !decoded.draw ||
       !nativeVideoPacketFullClear50LA(*decoded.draw))
        return fail(d,VehicleWindowError50DS::frame,"native packet requires exact supported full-color clear");
    if(policies.fba!=NativeVideoPacketCpu50LA::FbaPolicy::zero ||
       policies.scanmask!=NativeVideoPacketCpu50LA::ScanmaskPolicy::zero)
        return fail(d,VehicleWindowError50DS::request,"native packet requires explicit FBA0 and SCANMSK0 policies; source values remain unknown");
    const auto& draw=*decoded.draw;
    const std::uint64_t pixels=std::uint64_t(draw.width())*draw.height();
    if(pixels>std::numeric_limits<std::size_t>::max()/8 || pixels*8>maximumGpuColorBytes)
        return fail(d,VehicleWindowError50DS::limits,"native packet two-plane color budget is insufficient");
    s.nativeVideoPacketAttempted50LA=true;
    s.nativeVideoPacketWidth50LA=I(draw.width());s.nativeVideoPacketHeight50LA=I(draw.height());
    s.nativeVideoPacketRgbaBytes50LA=std::size_t(pixels)*4;
    s.nativeVideoPacketPolicies50LA=policies;s.diagnostic=&d;
    bool accepted=s.check("before native packet preparation") && s.prepareNativeVideoPacketFunctions50LA();
    Storage::NativeVideoPacketSavedState50LA saved;
    bool restore=false;
    if(accepted){accepted=s.saveNativeVideoPacketState50LA(saved);restore=accepted;}
    if(accepted)accepted=s.allocateNativeVideoPacketPair50LA(s.nativeVideoPacketTargets50LA,s.nativeVideoPacketColors50LA);
    if(restore){const bool restored=s.restoreNativeVideoPacketState50LA(saved);accepted=accepted && restored;}
    s.diagnostic=nullptr;
    // No further GL call after publication. Partial names stay owned by Storage.
    if(accepted)s.nativeVideoPacketReady50LA=true;
    return accepted;
}

bool VehicleWindowGl50DS::submitNativeVideoPacketClear(
    const game::OriginalVideoClearPacket25CAD0Native50LA& packet,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options viewport excludes native packet clear");
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"native packet clear needs actual renderer");
    auto& s=*storage_;
    if(!s.nativeVideoPacketIdle50LA(d))return false;
    if(!s.nativeVideoPacketReady50LA)
        return fail(d,VehicleWindowError50DS::request,"native packet target was not prepared");
    auto decoded=NativeVideoPacketCpu50LA::decode(packet);
    if(decoded.status!=NativeVideoPacketCpu50LA::Status::decoded || !decoded.draw ||
       !nativeVideoPacketFullClear50LA(*decoded.draw) ||
       decoded.draw->width()!=U(s.nativeVideoPacketWidth50LA) || decoded.draw->height()!=U(s.nativeVideoPacketHeight50LA))
        return fail(d,VehicleWindowError50DS::frame,"native packet draw does not match prepared full-color extent");
    if(s.nativeVideoPacketSerial50LA==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::limits,"native packet acceptance serial exhausted");
    // Source bytes -> native normalized RGBA8 clear. This is not PS2 parity.
    float color[4]{};
    for(unsigned i=0;i<4;++i)color[i]=float(decoded.draw->rgba()[i])/255.0f;
    const std::uint8_t bank=s.nativeVideoPacketPublished50LA^1u;
    s.diagnostic=&d;
    bool accepted=s.check("before native packet clear");
    Storage::NativeVideoPacketSavedState50LA saved;
    bool restore=false;
    if(accepted){accepted=s.saveNativeVideoPacketState50LA(saved);restore=accepted;}
    if(accepted){
        s.gl.bindFramebuffer(drawFramebuffer,s.nativeVideoPacketTargets50LA[bank]);
        s.gl.drawBuffer(colorAttachment);
        s.nativeVideoColorMaski50LA(0,1,1,1,1);
        if(s.nativeVideoIndexedScissor50LA)s.nativeVideoDisablei50LA(0x0C11,0);
        else s.gl.disable(0x0C11);
        for(unsigned i=1;i<4;++i)s.gl.disable(nativeVideoClearCapabilities50LA[i]);
        const U status=s.gl.checkFramebufferStatus(drawFramebuffer);
        accepted=s.check("configure native packet color clear");
        if(accepted && status!=framebufferComplete){d.framebufferStatus=status;
            accepted=fail(d,VehicleWindowError50DS::framebuffer,"native packet draw framebuffer incomplete");}
    }
    if(accepted){s.gl.clearBufferfv(0x1800,0,color);accepted=s.check("clear decoded native packet color target");}
    if(restore){const bool restored=s.restoreNativeVideoPacketState50LA(saved);accepted=accepted && restored;}
    s.diagnostic=nullptr;
    if(!accepted)return false;
    // Only a private native bank is accepted. No finish, swap, callback, source
    // receipt or context-drain generation is inferred from command acceptance.
    s.nativeVideoPacketPublished50LA=bank;s.nativeVideoPacketImage50LA=true;
    ++s.nativeVideoPacketSerial50LA;
    s.nativeVideoPacketClearAncestor50LA=s.nativeVideoPacketSerial50LA;
    s.nativeVideoPacketLastWrite50LA=NativeVideoPacketWrite50LA::clear;
    return true;
}

bool VehicleWindowGl50DS::readNativeVideoPacketClearRgba(std::uint8_t* destination,
    std::size_t capacityBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options viewport excludes native packet readback");
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"native packet readback needs actual renderer");
    auto& s=*storage_;
    if(!s.nativeVideoPacketIdle50LA(d))return false;
    if(!s.nativeVideoPacketReady50LA || !s.nativeVideoPacketImage50LA)
        return fail(d,VehicleWindowError50DS::noImage,"no accepted native packet color bank");
    return s.readNativeVideoPacketImage50LA(s.nativeVideoPacketTargets50LA[s.nativeVideoPacketPublished50LA],
        destination,capacityBytes,d);
}

bool VehicleWindowGl50DS::Storage::readNativeVideoPacketImage50LA(U target,std::uint8_t* destination,
    std::size_t capacityBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    auto& s=*this;
    if(!target)return fail(d,VehicleWindowError50DS::noImage,"native packet readback target is absent");
    d.readbackWidth=s.nativeVideoPacketWidth50LA;d.readbackHeight=s.nativeVideoPacketHeight50LA;
    d.readbackBytes=s.nativeVideoPacketRgbaBytes50LA;
    if(!destination || capacityBytes<d.readbackBytes)
        return fail(d,VehicleWindowError50DS::readback,"native packet RGBA destination is null or too small");
    s.diagnostic=&d;
    bool accepted=s.check("before native packet RGBA readback");
    I previousRead=0,previousPackBuffer=0,previousPack[8]{};
    constexpr U packNames[]={0x0D05,0x0D02,0x0D03,0x0D04,0x0D00,0x0D01,0x806C,0x806B};
    bool restore=false;
    if(accepted){
        s.gl.getIntegerv(0x8CAA,&previousRead);s.gl.getIntegerv(0x88ED,&previousPackBuffer);
        for(unsigned i=0;i<8;++i)s.gl.getIntegerv(packNames[i],&previousPack[i]);
        accepted=s.check("save native packet readback state");restore=accepted;
    }
    if(accepted)accepted=s.copyRgba(destination,target,
        s.nativeVideoPacketWidth50LA,s.nativeVideoPacketHeight50LA);
    if(restore){
        s.gl.bindFramebuffer(readFramebuffer,U(previousRead));
        for(unsigned i=0;i<8;++i)s.gl.pixelStorei(packNames[i],previousPack[i]);
        s.drawGl.bindBuffer(pixelPackBuffer,U(previousPackBuffer));
        const bool restored=s.check("restore native packet readback state");accepted=accepted && restored;
    }
    if(!accepted && d.error==VehicleWindowError50DS::gl)d.error=VehicleWindowError50DS::readback;
    s.diagnostic=nullptr;return accepted;
}

bool VehicleWindowGl50DS::nativeVideoPacketClearInfo(NativeVideoPacketClearInfo50LA& out) const noexcept {
    out={};
    if(!initialized())return false;
    const auto& s=*storage_;
    if(s.thread!=std::this_thread::get_id() || !s.nativeVideoPacketReady50LA || s.nativeGlFaultObserved50KG)return false;
    out.width=U(s.nativeVideoPacketWidth50LA);out.height=U(s.nativeVideoPacketHeight50LA);
    out.rgbaBytes=s.nativeVideoPacketRgbaBytes50LA;out.gpuColorBytes=out.rgbaBytes*2;
    out.policies=s.nativeVideoPacketPolicies50LA;out.acceptedSerial=s.nativeVideoPacketSerial50LA;
    out.commandsAccepted=s.nativeVideoPacketImage50LA;
    out.clearAncestorSerial=s.nativeVideoPacketClearAncestor50LA;out.lastWrite=s.nativeVideoPacketLastWrite50LA;
    return true;
}

bool VehicleWindowGl50DS::Storage::deleteNativeVideoPacketTargets50LA(VehicleWindowDiagnostic50DS& d) noexcept {
    nativeVideoPacketReady50LA=nativeVideoPacketImage50LA=false;nativeVideoPacketSerial50LA=0;
    nativeVideoSnapshotReady50LA=nativeVideoSnapshotImage50LA=false;
    nativeVideoSnapshotSerial50LA=nativeVideoSnapshotImageSerial50LA=nativeVideoSnapshotClearSerial50LA=0;
    nativeVideoPacketClearAncestor50LA=0;nativeVideoPacketLastWrite50LA=NativeVideoPacketWrite50LA::none;
    for(U b=0;b<2;++b)
        if(!deleteName(nativeVideoSnapshotTargets50LA[b],gl.deleteFramebuffers,d) ||
           !deleteName(nativeVideoSnapshotColors50LA[b],gl.deleteRenderbuffers,d))return false;
    for(U b=0;b<2;++b)
        if(!deleteName(nativeVideoPacketTargets50LA[b],gl.deleteFramebuffers,d) ||
           !deleteName(nativeVideoPacketColors50LA[b],gl.deleteRenderbuffers,d))return false;
    return true;
}

// The original 260A04 -> 262830(a1=0) capture and 2610D0 -> 262C70
// recopy use full equal-size local transfers. These native operations preserve
// that image-copy pattern, NOT those source calls, selectors, depth storage,
// fade state, waits, callbacks, DMA or IRQ side effects.
bool VehicleWindowGl50DS::prepareNativeVideoPacketTransfers(std::size_t maximumSnapshotGpuColorBytes,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options viewport excludes native transfer preparation");
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"native transfer preparation needs actual renderer");
    auto& s=*storage_;
    if(!s.nativeVideoPacketIdle50LA(d))return false;
    if(!s.nativeVideoPacketReady50LA)
        return fail(d,VehicleWindowError50DS::request,"native transfers need prepared packet color extent and policies");
    if(s.nativeVideoSnapshotAttempted50LA)
        return fail(d,VehicleWindowError50DS::alreadyOwned,"native snapshot preparation already attempted; close renderer before retry");
    if(s.nativeVideoPacketRgbaBytes50LA>std::numeric_limits<std::size_t>::max()/2 ||
       s.nativeVideoPacketRgbaBytes50LA*2>maximumSnapshotGpuColorBytes)
        return fail(d,VehicleWindowError50DS::limits,"native snapshot two-plane color budget is insufficient");
    s.nativeVideoSnapshotAttempted50LA=true;s.diagnostic=&d;
    bool accepted=s.check("before native snapshot preparation");
    if(accepted && !s.gl.blitFramebuffer)accepted=s.load(s.gl.blitFramebuffer,"glBlitFramebuffer");
    Storage::NativeVideoPacketSavedState50LA saved;
    bool restore=false;
    if(accepted){accepted=s.saveNativeVideoPacketState50LA(saved);restore=accepted;}
    if(accepted)accepted=s.allocateNativeVideoPacketPair50LA(s.nativeVideoSnapshotTargets50LA,s.nativeVideoSnapshotColors50LA);
    if(restore){const bool restored=s.restoreNativeVideoPacketState50LA(saved);accepted=accepted && restored;}
    s.diagnostic=nullptr;
    if(accepted)s.nativeVideoSnapshotReady50LA=true;
    return accepted;
}

bool VehicleWindowGl50DS::Storage::transferNativeVideoPacketImage50LA(U source,U destination) noexcept {
    if(!source || !destination || source==destination || !gl.blitFramebuffer)
        return fail(*diagnostic,VehicleWindowError50DS::frame,"native transfer requires distinct owned color targets and actual blit entry point");
    bool accepted=check("before native full-extent packet image transfer");
    NativeVideoPacketSavedState50LA saved;
    bool restore=false;
    if(accepted){accepted=saveNativeVideoPacketState50LA(saved);restore=accepted;}
    if(accepted){
        gl.bindFramebuffer(readFramebuffer,source);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,destination);gl.drawBuffer(colorAttachment);
        if(nativeVideoIndexedScissor50LA)nativeVideoDisablei50LA(0x0C11,0);
        else gl.disable(0x0C11);
        for(unsigned i=1;i<4;++i)gl.disable(nativeVideoClearCapabilities50LA[i]);
        const U readStatus=gl.checkFramebufferStatus(readFramebuffer);
        const U drawStatus=gl.checkFramebufferStatus(drawFramebuffer);
        accepted=check("configure native packet image transfer");
        if(accepted && (readStatus!=framebufferComplete || drawStatus!=framebufferComplete)){
            diagnostic->framebufferStatus=readStatus!=framebufferComplete?readStatus:drawStatus;
            accepted=fail(*diagnostic,VehicleWindowError50DS::framebuffer,"native transfer source or destination framebuffer incomplete");
        }
    }
    if(accepted){
        // Same-context ordering covers earlier clear/capture work. No finish or
        // source completion is inferred. Exact identity rectangles; no scaling,
        // blending, sRGB conversion, filtering policy guess or depth transfer.
        gl.blitFramebuffer(0,0,nativeVideoPacketWidth50LA,nativeVideoPacketHeight50LA,
            0,0,nativeVideoPacketWidth50LA,nativeVideoPacketHeight50LA,colorBufferBit,0x2600); // NEAREST.
        accepted=check("transfer owned native packet image with identity rectangles");
    }
    if(restore){const bool restored=restoreNativeVideoPacketState50LA(saved);accepted=accepted && restored;}
    return accepted;
}

bool VehicleWindowGl50DS::captureNativeVideoPacketImage(VehicleWindowDiagnostic50DS& d) noexcept {
    d={};OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options viewport excludes native packet capture");
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"native packet capture needs actual renderer");
    auto& s=*storage_;
    if(!s.nativeVideoPacketIdle50LA(d))return false;
    if(!s.nativeVideoPacketReady50LA || !s.nativeVideoSnapshotReady50LA ||
       !s.nativeVideoPacketImage50LA || !s.nativeVideoPacketClearAncestor50LA)
        return fail(d,VehicleWindowError50DS::noImage,"native capture needs a real accepted packet image and prepared snapshots");
    if(s.nativeVideoSnapshotSerial50LA==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::limits,"native capture acceptance serial exhausted");
    const auto bank=std::uint8_t(s.nativeVideoSnapshotPublished50LA^1u);
    s.diagnostic=&d;
    const bool accepted=s.transferNativeVideoPacketImage50LA(
        s.nativeVideoPacketTargets50LA[s.nativeVideoPacketPublished50LA],s.nativeVideoSnapshotTargets50LA[bank]);
    s.diagnostic=nullptr;
    if(!accepted)return false;
    // Only after successful copy AND restoration. Previous snapshot remains
    // published on failure; current packet image is never written by capture.
    s.nativeVideoSnapshotPublished50LA=bank;s.nativeVideoSnapshotImage50LA=true;
    s.nativeVideoSnapshotImageSerial50LA=s.nativeVideoPacketSerial50LA;
    s.nativeVideoSnapshotClearSerial50LA=s.nativeVideoPacketClearAncestor50LA;
    ++s.nativeVideoSnapshotSerial50LA;
    return true;
}

bool VehicleWindowGl50DS::recopyNativeVideoPacketImage(VehicleWindowDiagnostic50DS& d) noexcept {
    d={};OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options viewport excludes native packet recopy");
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"native packet recopy needs actual renderer");
    auto& s=*storage_;
    if(!s.nativeVideoPacketIdle50LA(d))return false;
    if(!s.nativeVideoPacketReady50LA || !s.nativeVideoSnapshotReady50LA ||
       !s.nativeVideoSnapshotImage50LA || !s.nativeVideoSnapshotClearSerial50LA)
        return fail(d,VehicleWindowError50DS::noImage,"native recopy needs a real retained packet snapshot");
    if(s.nativeVideoPacketSerial50LA==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::limits,"native packet acceptance serial exhausted");
    const auto bank=std::uint8_t(s.nativeVideoPacketPublished50LA^1u);
    s.diagnostic=&d;
    const bool accepted=s.transferNativeVideoPacketImage50LA(
        s.nativeVideoSnapshotTargets50LA[s.nativeVideoSnapshotPublished50LA],s.nativeVideoPacketTargets50LA[bank]);
    s.diagnostic=nullptr;
    if(!accepted)return false;
    // Snapshot remains unchanged. A recopy never impersonates a new decoded
    // clear; its actual native clear ancestor is carried forward separately.
    s.nativeVideoPacketPublished50LA=bank;s.nativeVideoPacketImage50LA=true;
    ++s.nativeVideoPacketSerial50LA;
    s.nativeVideoPacketClearAncestor50LA=s.nativeVideoSnapshotClearSerial50LA;
    s.nativeVideoPacketLastWrite50LA=NativeVideoPacketWrite50LA::recopy;
    return true;
}

bool VehicleWindowGl50DS::readNativeVideoPacketSnapshotRgba(std::uint8_t* destination,
    std::size_t capacityBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};OriginalOptionsMutationInterlock50KG::Operation operation;
    if(!operation.admitted())return fail(d,VehicleWindowError50DS::frame,"Options viewport excludes native snapshot readback");
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"native snapshot readback needs actual renderer");
    auto& s=*storage_;
    if(!s.nativeVideoPacketIdle50LA(d))return false;
    if(!s.nativeVideoPacketReady50LA || !s.nativeVideoSnapshotReady50LA || !s.nativeVideoSnapshotImage50LA)
        return fail(d,VehicleWindowError50DS::noImage,"no accepted native packet snapshot");
    return s.readNativeVideoPacketImage50LA(s.nativeVideoSnapshotTargets50LA[s.nativeVideoSnapshotPublished50LA],
        destination,capacityBytes,d);
}

bool VehicleWindowGl50DS::nativeVideoPacketSnapshotInfo(NativeVideoPacketSnapshotInfo50LA& out) const noexcept {
    out={};
    if(!initialized())return false;
    const auto& s=*storage_;
    if(s.thread!=std::this_thread::get_id() || !s.nativeVideoPacketReady50LA ||
       !s.nativeVideoSnapshotReady50LA || s.nativeGlFaultObserved50KG)return false;
    out.width=U(s.nativeVideoPacketWidth50LA);out.height=U(s.nativeVideoPacketHeight50LA);
    out.rgbaBytes=s.nativeVideoPacketRgbaBytes50LA;out.gpuColorBytes=out.rgbaBytes*2;
    out.policies=s.nativeVideoPacketPolicies50LA;
    out.acceptedCaptureSerial=s.nativeVideoSnapshotSerial50LA;out.capturedImageSerial=s.nativeVideoSnapshotImageSerial50LA;
    out.clearAncestorSerial=s.nativeVideoSnapshotClearSerial50LA;out.commandsAccepted=s.nativeVideoSnapshotImage50LA;
    return true;
}

// Native overlay definitions share this existing renderer translation unit.
#include "original_video_packet_overlay50la.inl"
