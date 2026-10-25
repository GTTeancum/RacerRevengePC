#pragma once
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace rr::platform {
// Called only by GameHost after the old Display has died and released its
// private host pointer/pins. This resets context state, never source Video,
// materials, profiles or counters, and neither creates nor deletes GL objects.
// The existing strict renderer/resource drains remain prerequisites.
template<class Host>
bool resetPlayerSessionGl50DU(Host& host)noexcept {
    using U=std::uint32_t;using I=std::int32_t;using B=std::uint8_t;
#define RR_SESSION_PROC(name,type) const auto name=reinterpret_cast<type>(host.procAddress(#name));if(!name)return false
    RR_SESSION_PROC(glGetError,U(*)());
    RR_SESSION_PROC(glGetIntegerv,void(*)(U,I*));
    RR_SESSION_PROC(glGetStringi,const unsigned char*(*)(U,U));
    RR_SESSION_PROC(glBindFramebuffer,void(*)(U,U));
    RR_SESSION_PROC(glBindRenderbuffer,void(*)(U,U));
    RR_SESSION_PROC(glUseProgram,void(*)(U));
    RR_SESSION_PROC(glBindVertexArray,void(*)(U));
    RR_SESSION_PROC(glBindBuffer,void(*)(U,U));
    RR_SESSION_PROC(glActiveTexture,void(*)(U));
    RR_SESSION_PROC(glBindTexture,void(*)(U,U));
    RR_SESSION_PROC(glDisable,void(*)(U));
    RR_SESSION_PROC(glEnable,void(*)(U));
    RR_SESSION_PROC(glViewport,void(*)(I,I,I,I));
    RR_SESSION_PROC(glScissor,void(*)(I,I,I,I));
    RR_SESSION_PROC(glColorMask,void(*)(B,B,B,B));
    RR_SESSION_PROC(glDepthMask,void(*)(B));
    RR_SESSION_PROC(glDepthFunc,void(*)(U));
    RR_SESSION_PROC(glDepthRange,void(*)(double,double));
    RR_SESSION_PROC(glClearDepth,void(*)(double));
    RR_SESSION_PROC(glClearColor,void(*)(float,float,float,float));
    RR_SESSION_PROC(glClearStencil,void(*)(I));
    RR_SESSION_PROC(glStencilMask,void(*)(U));
    RR_SESSION_PROC(glStencilFunc,void(*)(U,I,U));
    RR_SESSION_PROC(glStencilOp,void(*)(U,U,U));
    RR_SESSION_PROC(glBlendFunc,void(*)(U,U));
    RR_SESSION_PROC(glBlendEquation,void(*)(U));
    RR_SESSION_PROC(glFrontFace,void(*)(U));
    RR_SESSION_PROC(glCullFace,void(*)(U));
    RR_SESSION_PROC(glPolygonMode,void(*)(U,U));
    RR_SESSION_PROC(glPolygonOffset,void(*)(float,float));
    RR_SESSION_PROC(glPixelStorei,void(*)(U,I));
    RR_SESSION_PROC(glDrawBuffer,void(*)(U));
    RR_SESSION_PROC(glReadBuffer,void(*)(U));
#undef RR_SESSION_PROC
    // Existing errors remain a failure, not silently erased to claim readiness.
    if(glGetError()!=0)return false;
    I textureUnits=0;glGetIntegerv(0x8B4D,&textureUnits); // MAX_COMBINED_TEXTURE_IMAGE_UNITS
    if(glGetError()!=0||textureUnits<1||textureUnits>1024)return false;
    I major=0,minor=0;glGetIntegerv(0x821B,&major);glGetIntegerv(0x821C,&minor);
    bool samplerObjects=major>3||(major==3&&minor>=3);
    if(!samplerObjects){
        I count=0;glGetIntegerv(0x821D,&count);
        if(count<0||count>65536||glGetError()!=0)return false;
        for(I i=0;i<count;++i){
            const auto* extension=glGetStringi(0x1F03,static_cast<U>(i));
            if(extension&&std::strcmp(reinterpret_cast<const char*>(extension),
                "GL_ARB_sampler_objects")==0){samplerObjects=true;break;}
        }
    }
    const auto glBindSampler=samplerObjects?
        reinterpret_cast<void(*)(U,U)>(host.procAddress("glBindSampler")):nullptr;
    if(glGetError()!=0||(samplerObjects&&!glBindSampler))return false;
    glBindFramebuffer(0x8D40,0);glBindRenderbuffer(0x8D41,0);
    glUseProgram(0);glBindVertexArray(0);
    // Do not bind ELEMENT_ARRAY_BUFFER with VAO0 in a core profile.
    for(const U target:{0x8892u,0x88EBu,0x88ECu})glBindBuffer(target,0);
    for(I unit=0;unit<textureUnits;++unit){
        glActiveTexture(0x84C0u+static_cast<U>(unit));
        glBindTexture(0x0DE1,0);glBindTexture(0x8513,0);glBindTexture(0x8C1A,0);
        if(glBindSampler)glBindSampler(static_cast<U>(unit),0);
    }
    glActiveTexture(0x84C0);
    for(const U capability:{0x0B71u,0x0B90u,0x0BE2u,0x0B44u,0x0C11u,0x8DB9u,
        0x8C89u,0x8037u,0x809Eu,0x80A0u,0x8F9Du,0x0BF2u})glDisable(capability);
    glEnable(0x0BD0);glEnable(0x809D); // DITHER, MULTISAMPLE: context defaults.
    glColorMask(1,1,1,1);glDepthMask(1);glDepthFunc(0x0201);glDepthRange(0,1);
    glClearDepth(1);glClearStencil(0);glClearColor(0,0,0,0);
    glStencilMask(~U{0});glStencilFunc(0x0207,0,~U{0});glStencilOp(0x1E00,0x1E00,0x1E00);
    glBlendFunc(1,0);glBlendEquation(0x8006);glFrontFace(0x0901);glCullFace(0x0405);
    glPolygonMode(0x0408,0x1B02);glPolygonOffset(0,0);
    for(const U parameter:{0x0D00u,0x0D01u,0x0D02u,0x0D03u,0x0D04u,0x0CF0u,0x0CF1u,0x0CF2u,0x0CF3u,0x0CF4u,
        0x806Bu,0x806Cu,0x806Du,0x806Eu})glPixelStorei(parameter,0);
    glPixelStorei(0x0D05,4);glPixelStorei(0x0CF5,4);
    glDrawBuffer(0x0405);glReadBuffer(0x0405);
    const auto& size=host.info();
    glViewport(0,0,size.drawableWidth,size.drawableHeight);
    glScissor(0,0,size.drawableWidth,size.drawableHeight);
    return glGetError()==0;
}
}
