#pragma once
// Concatenable GLSL 1.50 literal. Insert after #version/uniform declarations and
// before main(). Only the opaque, depth-writing, lit track pass enables it.
// Sky, pod effects, cables and track beams never use these uniforms. This is a
// native projected soft-footprint approximation, not a shadow map: no exact
// caster silhouette, self shadow, world occlusion, or nearest-receiver test.
#define RR_NATIVE_POD_SHADOW_GLSL50HM R"GLSL(
uniform int nativePodShadowEnabled;
uniform vec4 nativePodShadowCenter[3];
uniform vec4 nativePodShadowShape[3];
// Explicit calls keep caster indices compile-time constants, allowing software
// renderers to lower fixed SIMD operations without a dynamic uniform-array
// lookup or a per-fragment loop/continue mask. The footprint math is unchanged.
float rrNativePodCasterShadow(vec3 receiver, vec4 center, vec4 shape) {
    float height = center.y - receiver.y;
    if (shape.w <= 0.0 || height < 0.0 || height >= center.w) return 0.0;
    vec2 q = receiver.xz - center.xz + vec2(0.35/0.85, 0.30/0.85) * height;
    float spread = 1.0 + 0.6 * height / center.w;
    float distanceSquared = (shape.x*q.x*q.x + 2.0*shape.y*q.x*q.y + shape.z*q.y*q.y)
        / (spread*spread);
    float edge = 1.0 - smoothstep(0.20, 1.15, distanceSquared);
    float fade = 1.0 - smoothstep(center.w*0.4, center.w, height);
    return shape.w * edge * fade;
}
float rrNativePodShadow(vec3 receiver, vec3 receiverNormal) {
    if (nativePodShadowEnabled == 0) return 1.0;
    float upward = smoothstep(0.15, 0.65, receiverNormal.y);
    if (upward == 0.0) return 1.0;
    float shadow0 = rrNativePodCasterShadow(receiver, nativePodShadowCenter[0], nativePodShadowShape[0]);
    float shadow1 = rrNativePodCasterShadow(receiver, nativePodShadowCenter[1], nativePodShadowShape[1]);
    float shadow2 = rrNativePodCasterShadow(receiver, nativePodShadowCenter[2], nativePodShadowShape[2]);
    return 1.0 - max(shadow0, max(shadow1, shadow2)) * upward;
}
)GLSL"
