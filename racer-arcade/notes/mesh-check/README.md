# Mesh decode visual check (Nov 12)
tools\mesh_render.py renders hikaru_gpu static-vertex runs flat-shaded.
- m2C0033C (mask 0x02C0033C, 1650 v / 1193 t): clean, coherent object (awning/building piece). Positions decode right.
- m2C13D3C (0x02C13D3C, 1853 v / 1518 t): recognisable curved hull, but with holes and a few detached
  bits -> my strip/fan assembly (pivot/winding/"tri on uv" rules) is incomplete, or sub-parts use
  their own transforms. Fix triangle assembly against Odin antarctic_mesh.cpp before export.

Update (Nov 12, later): fixed. tools\hikaru_mesh.py follows Odin exactly (the 158 UV push closes the
triangle and its header sets winding/no-cull; pivot = swap of ring slots 1/2). Boundary edges 1201 -> 249,
flipped shared edges 1415 -> 41 for model 3 (start at record 0x02C13D20, not 0x02C13D3C, so the 901
precision word is read). Renders: export\meshcheck\m02C13D20.png, m02C00320.png. See notes\courses.md.
