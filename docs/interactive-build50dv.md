# Interactive source build 50DV

Run tools/prepare_interactive_build50dv.py with two distinct output paths:
command JSON and future executable. The existing output directory is required.
The generator writes argv only. It replaces the offscreen harness and owned EGL
renderer with50DQ window host,50DR keyboard adapter and50DS window renderer.
50DO supplies the retained gameplay and asset dependencies plus strict float,
pthread,zlib and dl flags. Runtime SDL2/window/display availability is separate.

No generator, build or interactive execution has yet been performed. Root alone
owns the shared hourly test slot; this addition does not authorize another test.
The50DU development client uses retained scene fixtures, not a full-game loader.
