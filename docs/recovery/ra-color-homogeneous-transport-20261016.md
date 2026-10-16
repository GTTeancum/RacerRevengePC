# RA native color interpolation transport

Source-only recovery candidate against0f65ffa. No build, test, syntax check,
sanitizer, gameplay or screenshot inspection performed. Root owns integration
and recovery validation. No geometry shader is restored.

The existing RA vertex shader sends original packed RGBA byte values through
noperspective color interpolation, while fog already uses homogeneous smooth
transport. This candidate applies that same transport to all four color lanes:
vertex H=RGBA*clipW; fragment RGBA=interpolated(H)*gl_FragCoord.w. The existing
floor/clamp to byte range is applied AFTER reconstruction, in the same place
as before. Texture modulation, TEXA, alpha test, alpha blending, fog, primitive
order, batching, coverage and depth policies are unchanged.

For screen-space barycentric weights lambda_i, smooth H evaluates as
sum(lambda_i*H_i/W_i)/sum(lambda_i/W_i). gl_FragCoord.w equals the denominator.
Their product is sum(lambda_i*RGBA_i), the existing screen-linear RGBA rule.
This identity holds independently for R,G,B and alpha. It does not replace
source alpha with an opaque constant or normalize original byte magnitudes.

For a generated clipping endpoint at homogeneous edge parameter t, carrying
H linearly produces ((1-t)*W0*C0+t*W1*C1)/((1-t)*W0+t*W1) after reconstruction,
the noperspective screen-linear clipping rule. Keeping the numerator in
homogeneous form avoids an intermediate near-zero-W division in generated
color attributes until valid fragments are rasterized. The candidate does
not change that rule to homogeneous interpolation of raw source color.

In finite precision the two forms can round differently. Near-zero W,
cancellation, overflow, and final byte-floor boundaries are the relevant
limits; there is no added epsilon, changed clamp or discarded source vertex.
Raster fragments are expected to have finite positive W after native clipping.
The source clipper's separate near-plane raw-color interpolation policy is
still not implemented: original3669C0 multiplies color by W only AFTER its
first plane; original367160 divides it before output. This candidate is an
algebraically equivalent implementation of the CURRENT native interpolation,
not a claim to resolve that original clip-policy difference or prove a driver
bug. Root must retain it only if centralized recovery evidence supports it.

Only the RA shader source inra_static_world_gl_storage.inl and this note
change. No asset bytes, CPU color production, input vertex layout, material
state, other renderer or broad shared decoder are modified.
