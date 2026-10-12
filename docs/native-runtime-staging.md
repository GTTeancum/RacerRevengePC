# Native runtime staging

`tools/stage_native_runtime.py` stages a tester directory from an existing
prepared runtime manifest, the transferred original camera files and a supplied
native host executable. It does not build, decode assets, launch the program,
or test gameplay. This script is source-reviewed but has not been executed.

Example (replace the input paths with your prepared files):

```sh
python3 tools/stage_native_runtime.py \
  --runtime-root /path/to/runtime \
  --runtime-manifest /path/to/runtime-assets.json \
  --camera-root /path/to/authored-cam/originals \
  --native-executable /path/to/rr-native \
  --platform linux --output /path/to/new-tester-package
```

Use `--platform windows` with the actual Windows native executable. Never
rename a Linux binary to create a Windows package. Required native system
libraries remain a platform dependency; this script does not install them.

The original camera input root contains `manifest.json` and all 23 transferred
`PODS/PODxx/PODxx.CAM` files. These are copied unchanged into the runtime's
`work/native/project/disc/` directory. The runtime manifest must contain the
native fixture corpus, actual GA archive and APPSTART archive. Additional
prepared assets can be provided through repeated `--overlay ROOT MANIFEST`.
Overlay paths are relative to the runtime, and conflicting bytes are rejected.

Inputs and copied outputs are checked against their supplied size/SHA256
identities. The completion manifest is written last. An incomplete output
directory must not be distributed; choose a new directory after correcting
the input rather than overwriting a previous package.

## Current Watchtower direct entry

Run from the package directory using the normal native window launch route.
After setting the environment below, the Linux command is
`./rr-native evidence/staticType2Producer49X/original-word/native-fixtures.json`;
on Windows use `rr-native.exe` with the same corpus argument.
The relevant environment settings are:

```text
RR_TRACK=GA
RR_GA_ORIGINAL_AI_CANDIDATE=1
RR_RACE=1
RR_RACE_MODE=2
RR_START_MENU=0
RR_ORIGINAL_AI_ALL_RACERS=1
RR_AUTODRIVE=0
RR_DISABLE_RACER_CONTACTS=0
RR_AUTHORED_WORLD_FOG=1
RR_AUDIO=1
```

Leave lighting at its authored default. Do not supply a tint override. This
route uses original AI for all eight racers, including the camera-followed
racer. The direct preview currently supports Resume, Controls and Exit;
restart and checkpoint relaunch are unavailable. Original menu-to-race entry
and original camera activation are separate integration changes in progress.

The Linux build and basic initialization smoke passed on commit `8288704` on
October 12, 2026. This is not a Windows build claim, visual acceptance, or
full-race result. Human testers own visual and playthrough acceptance.
