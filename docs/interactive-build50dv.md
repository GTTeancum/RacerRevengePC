# Linux interactive source build

The Linux build uses the C++ sources in
`platform/windows/native/interactive50du.sources.json`, the Linux EGL context
listed in that manifest, and system zlib. The public download is for Windows x64.

Run the same compile and link check as GitHub Actions with GCC 14, Python 3 and
zlib development headers installed. Give it a new output directory inside the
checkout.

```sh
CXX=g++-14 PYTHONDONTWRITEBYTECODE=1 python3 tools/ci/build_linux.py work/scratch/native-linux
```

The build writes `racer-revenge`, a source inventory, per-file compiler logs and
`RESULT.json` to that directory. To inspect the compiler arguments separately,
run `tools/prepare_interactive_build50dv.py` with paths for the JSON file and
the future executable. Running gameplay also requires original game data, SDL2
and a working graphics driver.
