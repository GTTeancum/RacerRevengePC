# Original-disc development launch

Latest bounded runtime result (October18 04:58 UTC): Aldar Beedo / Soro Suub Facility reached real GO and throttle movement, Pause, and confirmed Restart rebuilt session3 from original disc files with generated-root absent. Same-run selection and in-race proofs retained. Restarted movement, Main return and finish are still pending; MA has no GUI result yet. The audit and component limits below remain applicable.

This is a development launcher, not a validated all-track release. Current normal human Course source integration includes TA/Mos Espa, GA/Watchtower, MA/Orotoru G'am and SA/Soro Suub Facility. TA/GA have bounded GUI-flow evidence; newly compiled MA/SA joins still require GUI verification. Other track and rider coverage remains incomplete.

Run `python tools/launch_original_disc.py --assets /path/to/extracted-original-disc --executable /path/to/racer-revenge` (Windows binary: `RacerRevengePC.exe`). `--executable` is necessary when this script remains in tools instead of beside the binary. Add `--check` for shallow path checks without starting the game.

Supply your original disc files separately. No conversion step, generated corpus, prepared runtime pack or rider cache is required by this launcher. It clears inherited RR overrides, sets human keyboard input and host timing mode2, disables the separate host menu, and opens original Main through `--assets`. Audio is muted by this development configuration; audible playback is not validated here.

The executable needs SDL2, suitable graphics drivers, and the native IPU decoder libraries. Its current decoder requires FFmpeg7.1.5 ABI versions avcodec61.19.101 and avutil59.39.100. Windows requires SDL2.dll, avcodec-61.dll and avutil-59.dll beside the executable, plus their actual dependency closure. Filename presence is not ABI or Windows runtime verification.

Use a clean executable directory with no evidence/work/cacheobjects, generated meshes/textures/scenes, corpus JSON or rider cache. The launcher does not globally prohibit every legacy reader still present in source: when generated-root is unset, older paths can resolve relative to the executable directory. Each supported route needs absent-pack runtime and file-access verification before release. Original asset files must not be included in public packages.

Historical retained-runtime tools remain for diagnostic provenance and tests; do not use their staged-runtime output as this release path. Ordinary compiler objects, build receipts and test screenshots are not runtime asset packs.

