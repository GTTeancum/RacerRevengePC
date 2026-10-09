// Six 1x1 fixture draws verify the actual GLSL helper only. This is deliberately
// synthetic, not a gameplay capture, renderer integration or original parity.
#include "../../platform/linux/native/native_pod_shadow_glsl50hm.h"
#include "../../platform/linux/native/offscreen_gl_context50dc.h"
#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cstdint>
namespace p = rr::platform;
using U = std::uint32_t;
using I = std::int32_t;
void need(bool value, const char* message) { if (!value) throw std::runtime_error(message); }
template<class T> T load(p::OffscreenGlContext50DC& context,const char* name) {
    auto value=reinterpret_cast<T>(context.procAddress(name));need(value!=nullptr,name);return value;
}
int main() {
    p::OffscreenGlContext50DC context;
    p::OffscreenGlDiagnostic50DC diagnostic{};
    p::OffscreenGlRequest50DC request{};
    request.platform=p::OffscreenGlPlatform50DC::MesaSurfaceless;
    request.width=request.height=1;request.major=3;request.minor=2;
    request.profile=p::OffscreenGlProfile50DC::Core;
    request.redBits=request.greenBits=request.blueBits=request.alphaBits=8;
    request.depthBits=24;
    if(!context.create(request,diagnostic)) {
        std::cerr<<"GL fixture blocked: "<<unsigned(diagnostic.error)<<" "
            <<(diagnostic.operation?diagnostic.operation:"")<<'\n';return 2;
    }
#define FN(type,name) auto name=load<type>(context,"gl" #name)
    FN(U(*)(U),CreateShader);FN(void(*)(U,I,const char* const*,const I*),ShaderSource);
    FN(void(*)(U),CompileShader);FN(void(*)(U,U,I*),GetShaderiv);
    FN(void(*)(U,I,I*,char*),GetShaderInfoLog);FN(U(*)(),CreateProgram);
    FN(void(*)(U,U),AttachShader);FN(void(*)(U,U,const char*),BindFragDataLocation);
    FN(void(*)(U),LinkProgram);FN(void(*)(U,U,I*),GetProgramiv);
    FN(void(*)(U),UseProgram);FN(I(*)(U,const char*),GetUniformLocation);
    FN(void(*)(I,I),Uniform1i);FN(void(*)(I,float,float,float,float),Uniform4f);
    FN(void(*)(I,U*),GenVertexArrays);FN(void(*)(U),BindVertexArray);
    FN(void(*)(U,I,I),DrawArrays);FN(void(*)(I,I,I,I,U,U,void*),ReadPixels);
    FN(void(*)(U),DeleteShader);FN(void(*)(U),DeleteProgram);
    FN(void(*)(I,const U*),DeleteVertexArrays);FN(U(*)(),GetError);
#undef FN
    const char* vertex=R"GLSL(#version 150 core
void main() {
    vec2 p=gl_VertexID==0?vec2(-1,-1):(gl_VertexID==1?vec2(3,-1):vec2(-1,3));
    gl_Position=vec4(p,0,1);
}
)GLSL";
    const char* fragment=R"GLSL(#version 150 core
uniform vec4 receiver;
uniform vec4 receiverNormal;
out vec4 color;
)GLSL" RR_NATIVE_POD_SHADOW_GLSL50HM R"GLSL(
void main(){float v=rrNativePodShadow(receiver.xyz,receiverNormal.xyz);color=vec4(v,v,v,1);}
)GLSL";
    const auto compile=[&](U kind,const char* source) {
        const U shader=CreateShader(kind);ShaderSource(shader,1,&source,nullptr);CompileShader(shader);
        I status=0;GetShaderiv(shader,0x8B81,&status);
        if(!status){char log[2048]{};GetShaderInfoLog(shader,2048,nullptr,log);std::cerr<<log<<'\n';}
        need(status!=0,"shadow GLSL compile failed");return shader;
    };
    const U vs=compile(0x8B31,vertex),fs=compile(0x8B30,fragment),program=CreateProgram();
    AttachShader(program,vs);AttachShader(program,fs);BindFragDataLocation(program,0,"color");
    LinkProgram(program);I linked=0;GetProgramiv(program,0x8B82,&linked);need(linked!=0,"shadow link failed");
    UseProgram(program);U vao=0;GenVertexArrays(1,&vao);BindVertexArray(vao);
    const auto uniform=[&](const char* name){const I value=GetUniformLocation(program,name);
        need(value>=0,"missing shadow uniform");return value;};
    const I enabled=uniform("nativePodShadowEnabled"),receiver=uniform("receiver"),normal=uniform("receiverNormal");
    for(unsigned i=0;i<3;++i) {
        const auto suffix="["+std::to_string(i)+"]";
        const I center=uniform(("nativePodShadowCenter"+suffix).c_str());
        const I shape=uniform(("nativePodShadowShape"+suffix).c_str());
        Uniform4f(center,0,10,0,100);Uniform4f(shape,1.f/25,0,1.f/25,i?0.f:.48f);
    }
    const auto pixel=[&](bool on,float x,float y,float z,float ny) {
        Uniform1i(enabled,on?1:0);Uniform4f(receiver,x,y,z,0);Uniform4f(normal,0,ny,0,0);
        DrawArrays(0x0004,0,3);std::array<unsigned char,4> rgba{};
        ReadPixels(0,0,1,1,0x1908,0x1401,rgba.data());need(GetError()==0,"shadow fixture GL error");
        return unsigned(rgba[0]);
    };
    const float x=-10.f*.35f/.85f,z=-10.f*.30f/.85f;
    const unsigned shadow=pixel(true,x,0,z,1);
    need(shadow>=132&&shadow<=134,"projected receiver not darkened");
    need(pixel(false,x,0,z,1)==255,"disabled transparent/sky policy affected");
    need(pixel(true,100,0,100,1)==255,"distant receiver affected");
    need(pixel(true,0,11,0,1)==255,"receiver above caster affected");
    need(pixel(true,x,0,z,0)==255,"vertical receiver affected");
    need(pixel(true,-120.f*.35f/.85f,-110,-120.f*.30f/.85f,1)==255,"receiver beyond reach affected");
    BindVertexArray(0);DeleteVertexArrays(1,&vao);UseProgram(0);
    DeleteProgram(program);DeleteShader(vs);DeleteShader(fs);
    need(context.close(diagnostic),"GL fixture cleanup failed");
    std::cout<<"native shadow GLSL: 6 synthetic 1x1 receiver checks PASS; shadow_pixel="<<shadow<<'\n';
}
