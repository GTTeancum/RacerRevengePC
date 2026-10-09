#pragma once
// Concatenable GLSL 1.50 literal. Insert after #version/uniform declarations and
// before main(). Only the opaque, depth-writing, lit track pass enables it.
// Sky, pod effects, cables and track beams never use these uniforms. This is a
// native projected soft-footprint approximation, not a shadow map: no exact
// caster silhouette, self shadow, world occlusion, or nearest-receiver test.
#define RR_NATIVE_POD_SHADOW_GLSL50HM R"GLSL(
uniform int nativePodShadowEnabled;
uniform vec4 nativePodShadowCenter[24];
uniform vec4 nativePodShadowShape[24];
uniform int nativePodShadowOpponentCount;
uniform vec4 nativePodShadowOpponentBounds[7];
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
bool rrNativePodShadowReceiverWithin(vec3 receiver, vec4 bounds) {
    return all(greaterThanEqual(receiver.xz, bounds.xy)) &&
        all(lessThanEqual(receiver.xz, bounds.zw));
}
float rrNativePodShadow(vec3 receiver, vec3 receiverNormal) {
    if (nativePodShadowEnabled == 0) return 1.0;
    float upward = smoothstep(0.15, 0.65, receiverNormal.y);
    if (upward == 0.0) return 1.0;
    float shadow0 = rrNativePodCasterShadow(receiver, nativePodShadowCenter[0], nativePodShadowShape[0]);
    float shadow1 = rrNativePodCasterShadow(receiver, nativePodShadowCenter[1], nativePodShadowShape[1]);
    float shadow2 = rrNativePodCasterShadow(receiver, nativePodShadowCenter[2], nativePodShadowShape[2]);
    float shadow = max(shadow0, max(shadow1, shadow2));
    // Keep the original player math, including its fast return for solo frames.
    // Each submitted opponent adds a cheap conservative XZ broad phase. Only
    // nearby receiver fragments evaluate its three actual component ellipses.
    // All indices remain constant: no per-fragment loops or dynamic indexing.
    if (nativePodShadowOpponentCount == 0) return 1.0 - shadow * upward;
    if (nativePodShadowOpponentCount > 0 &&
        rrNativePodShadowReceiverWithin(receiver, nativePodShadowOpponentBounds[0])) {
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[3], nativePodShadowShape[3]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[4], nativePodShadowShape[4]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[5], nativePodShadowShape[5]));
    }
    if (nativePodShadowOpponentCount > 1 &&
        rrNativePodShadowReceiverWithin(receiver, nativePodShadowOpponentBounds[1])) {
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[6], nativePodShadowShape[6]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[7], nativePodShadowShape[7]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[8], nativePodShadowShape[8]));
    }
    if (nativePodShadowOpponentCount > 2 &&
        rrNativePodShadowReceiverWithin(receiver, nativePodShadowOpponentBounds[2])) {
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[9], nativePodShadowShape[9]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[10], nativePodShadowShape[10]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[11], nativePodShadowShape[11]));
    }
    if (nativePodShadowOpponentCount > 3 &&
        rrNativePodShadowReceiverWithin(receiver, nativePodShadowOpponentBounds[3])) {
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[12], nativePodShadowShape[12]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[13], nativePodShadowShape[13]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[14], nativePodShadowShape[14]));
    }
    if (nativePodShadowOpponentCount > 4 &&
        rrNativePodShadowReceiverWithin(receiver, nativePodShadowOpponentBounds[4])) {
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[15], nativePodShadowShape[15]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[16], nativePodShadowShape[16]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[17], nativePodShadowShape[17]));
    }
    if (nativePodShadowOpponentCount > 5 &&
        rrNativePodShadowReceiverWithin(receiver, nativePodShadowOpponentBounds[5])) {
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[18], nativePodShadowShape[18]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[19], nativePodShadowShape[19]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[20], nativePodShadowShape[20]));
    }
    if (nativePodShadowOpponentCount > 6 &&
        rrNativePodShadowReceiverWithin(receiver, nativePodShadowOpponentBounds[6])) {
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[21], nativePodShadowShape[21]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[22], nativePodShadowShape[22]));
        shadow = max(shadow, rrNativePodCasterShadow(receiver, nativePodShadowCenter[23], nativePodShadowShape[23]));
    }
    return 1.0 - shadow * upward;
}
)GLSL"
