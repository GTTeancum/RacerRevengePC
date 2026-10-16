# Native RA fog interpolation transport

Source-only candidate against 939089a. No build, test, syntax check, gameplay,
screenshot inspection or guest execution performed. Root owns validation.

The existing capture records floor triangles 116/117 with F=255 at every
original vertex, including negative clip-W endpoints. These inputs cannot
produce full fog through exact constant-preserving interpolation. Static audit
found consistent CPU bytes, absolute indices, VBO upload, FLOAT attribute 3
(stride 40, offset 36), linked attribute name, and fragment FGE/color state.
This does not establish a driver defect or identify the actual GPU winner.

Replace only the fog varying transport: emit smooth H=F*clipW, reconstruct
F=H*gl_FragCoord.w in the fragment. At screen barycentric weights lambda_i,
smooth H is sum(lambda_i*H_i/W_i)/sum(lambda_i/W_i). gl_FragCoord.w is the
latter denominator, so their product is sum(lambda_i*F_i), the existing
screen-linear fog. Color remains noperspective and texture UV is unchanged.
The existing fragment floor/clamp, original fog byte computation, and FGE are
unchanged. No source geometry is removed, moved, or assigned different fog.

At a homogeneous clip edge parameter t, smooth H is linearly interpolated
before division; reconstructing at the generated vertex gives
((1-t)*W0*F0+t*W1*F1)/((1-t)*W0+t*W1), which is the screen-linear clipping
rule. Keeping the numerator homogeneous avoids its intermediate division by a
near-zero generated W. This is an algebraically equivalent native transport,
not a change to original radial fog or a proof of PS2 raster equivalence.

Floating-point implementations can differ, particularly in cancellation,
overflow, and the final floor at byte boundaries. Clipping is expected to leave
finite positive raster W; singular W=0 is not an additional admitted fragment
case. The central basic recovery must assess this candidate and retain it only
if observed results support it. No runtime success is claimed.

Original clip-generated attribute policy remains a separate source audit:
PrelitCompressed calls external VU byte 2B80 at source 33A060; the actual
TriStripClipper program upload starts at source 366450 (micro address 570).
No guest instructions were executed to identify these source records.
