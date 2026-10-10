# Minimal native asset contract 48B

Implementation contract for the first original static-mesh experiment, not the
final streaming format. All integers/floats are little endian, with fixed widths.
No host pointers, PS2 instructions, material microprograms or register state.

## Mesh version 1

File extension `.rrmesh`. Header: eight ASCII bytes `RRMESH01`, uint32 vertex
count, uint32 triangle-list index count. Vertices follow, each 24 bytes: five
IEEE754 float32 values (x,y,z,u,v), then four uint8 values (r,g,b,a). uint32
indices follow. File size must equal 16 + 24*vertex_count + 4*index_count;
index_count must be divisible by three; all indices must be in bounds and all
float values finite. Limit counts and total bytes before allocating.

Positions are decoded original mesh-space positions including the format's
quantization origin when required. Any recentering for inspection is a camera/
model transform recorded separately, not an edit to saved vertices. UVs and
colors preserve recovered values. Unresolved strip winding/ADC/restarts,
coordinate space or attribute semantics must remain explicit in the conversion
report; a candidate export does not constitute geometry acceptance.

## Texture version 1

File extension `.rrtex`. Header: eight ASCII bytes `RRTEX001`, uint32 width,
uint32 height. Tightly packed RGBA8 pixels follow in top-to-bottom rows. File
size must equal 16 + width*height*4 with checked arithmetic. This minimal format
stores the base level only; preserve original mip evidence in the conversion
report and plan a complete mip/texture schema later. No resampling or generated
content. Unverified palette/swizzle/alpha behavior must be reported.

## Inspection tool interface

Native viewer takes `--mesh PATH`, optional `--texture PATH`, `--capture PATH`
for a PPM, `--frames N`, `--report PATH`, and `--interactive` for a visible
window. Default is offscreen rendering and bounded execution. Viewer must not
load original RES/PSG/PST files or depend on any PS2 runtime. A texture-free
geometry inspection may use a neutral material, explicitly labeled as such.
Output report records actual asset paths, counts, bounds, frame count, timing,
device type, modules and limitations. Scene-viewer timing is not gameplay FPS.

Converter outputs separate provenance JSON with input container/member hashes,
logical offsets, output hashes, decoded fields, validation performed and unknowns.
The initial experiment has a single mesh/material; no implied whole-track or
assembled-pod support.
