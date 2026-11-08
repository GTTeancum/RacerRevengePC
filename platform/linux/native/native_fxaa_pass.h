#pragma once
// PC-native FXAA (fast approximate anti-aliasing) for the completed 3D scene.
// One full-screen triangle samples a copy of the scene with bilinear
// filtering. Flat areas (local luma contrast below the edge threshold) are
// returned unchanged, so only detected edges are blended. Alpha is kept.
#include <cstdlib>

namespace rr::platform::native {

inline bool fxaaRequested50DS() noexcept {
    const char* v=std::getenv("RR_VIDEO_FXAA");
    return v&&v[0]=='1'&&!v[1];
}

inline constexpr char fxaaNativeVertex[] = R"GLSL(#version 150 core
void main() {
    vec2 corner=vec2(float((gl_VertexID<<1)&2),float(gl_VertexID&2));
    gl_Position=vec4(corner*2.0-1.0,0.0,1.0);
}
)GLSL";

inline constexpr char fxaaNativeFragment[] = R"GLSL(#version 150 core
uniform sampler2D sceneImage;
uniform vec2 inverseTextureSize; // 1 / texture storage size (maximum extent)
uniform vec2 maximumUv;          // last valid texel centre of the drawn extent
out vec4 smoothedColor;

float luma(vec3 c) { return dot(c,vec3(0.299,0.587,0.114)); }
vec3 fetch(vec2 uv) {
    // The texture is the renderer's maximum size; never read past the frame.
    return texture(sceneImage,clamp(uv,0.5*inverseTextureSize,maximumUv)).rgb;
}

void main() {
    vec2 uv=gl_FragCoord.xy*inverseTextureSize;
    vec4 centre=texture(sceneImage,uv);
    vec3 nw=fetch(uv+vec2(-1.0,-1.0)*inverseTextureSize);
    vec3 ne=fetch(uv+vec2( 1.0,-1.0)*inverseTextureSize);
    vec3 sw=fetch(uv+vec2(-1.0, 1.0)*inverseTextureSize);
    vec3 se=fetch(uv+vec2( 1.0, 1.0)*inverseTextureSize);
    float lNW=luma(nw),lNE=luma(ne),lSW=luma(sw),lSE=luma(se),lM=luma(centre.rgb);
    float lMin=min(lM,min(min(lNW,lNE),min(lSW,lSE)));
    float lMax=max(lM,max(max(lNW,lNE),max(lSW,lSE)));
    if (lMax-lMin<max(0.0312,lMax*0.125)) { smoothedColor=centre; return; }
    vec2 direction=vec2(-((lNW+lNE)-(lSW+lSE)),(lNW+lSW)-(lNE+lSE));
    float reduce=max((lNW+lNE+lSW+lSE)*(0.25/8.0),1.0/128.0);
    float scale=1.0/(min(abs(direction.x),abs(direction.y))+reduce);
    direction=clamp(direction*scale,vec2(-8.0),vec2(8.0))*inverseTextureSize;
    vec3 a=0.5*(fetch(uv+direction*(1.0/3.0-0.5))+fetch(uv+direction*(2.0/3.0-0.5)));
    vec3 b=a*0.5+0.25*(fetch(uv-direction*0.5)+fetch(uv+direction*0.5));
    float lB=luma(b);
    smoothedColor=vec4((lB<lMin||lB>lMax)?a:b,centre.a);
}
)GLSL";

} // namespace rr::platform::native
