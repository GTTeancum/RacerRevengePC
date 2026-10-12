# Course recorder launch-contract source review

2026-10-12 15:28 UTC. Reviewed corrected `tools/gameplay/record_original_course_roundtrip.py` against the integrated runtime, UI entry/frontend, native headless host and renderer source. No import, syntax check, compiler, test, gameplay or other executable software check was performed. The reviewer changed no repository or running-session source.

## Result

The failed launch exposed one definite environment contradiction: explicitly setting `RR_NATIVE_GC_SELECTED_LIGHTS=1` while `RR_ORIGINAL_MAIN_MENU=1`. Runtime lines 225–235 make default selected-track lighting conditional on a fresh gameplay race, then reject an explicit selected-light policy in Main. The corrected recorder omits the override. The prior recorder review missed this cross-check; its no-must-fix conclusion did not establish this launch contract.

No additional definite environment/startup contradiction was found in the corrected recorder. This is source acceptance, not evidence that Main will reach Ready. The original failed snapshot must remain unchanged.

## Explicit environment checklist

Locations without a filename prefix are in `tests/native/terrain_interactive_drive50du.cpp`.

| Recorder setting | Actual source contract and conclusion |
|---|---|
| `RR_TRACK=GC` | Main resolves the selected track at 4563–4574. GC is admitted with the following two explicit settings. |
| `RR_GC_INCOMPLETE_WORLD=1` | Required for GC at 197–207; accepts the documented unjoined Gate/LostAI limits. No cold Main exemption exists, so retain it. |
| `RR_RACE_MODE=2` | Required GC host-mode admission at 254–265. All-original-AI rejects mode1. This configures the retained cold world; Main never activates that race. |
| `RR_RACE=1` | Needed by original AI and roster rigs at 245–250. Do not disable it merely because this is a menu observation. |
| `RR_ORIGINAL_AI_ALL_RACERS=1` | Implies original AI and dynamic contacts at 236–246. Cold all-eight preparation is supported; Main bypasses neutral setup physics/rank/observations at 2510–2605. Final cold fleet checks at 2109–2128 and 3343–3345 require actual initialized owners but do not advance them. |
| `RR_AUTODRIVE=0` | Satisfies the all-original-AI prohibition on host autodrive at 795–798. |
| `RR_DISABLE_RACER_CONTACTS=0` | Leaves contacts enabled, as required by original AI at 267–271. |
| `RR_AUDIO=1`, explicit `RR_AUDIO_ROOT` | Required by dynamic-contact cold preparation at 849–855. Live/race/Dax/music/material assets are genuinely loaded at 867 onward. Setting race audio to zero would introduce another startup failure. |
| `RR_AUDIO_DEVICE=0` | Valid disabled-device setting. Main skips world output opening at 2863–2893; muted UI does not prepare an output device. |
| `RR_START_MENU=0` | Valid at 4551–4554. This selects the host front-menu setting, not original Main admission. Original Main takes ownership independently before host menus or driving at 3383–3481. |
| `RR_AUTHORED_WORLD_FOG=1` | Accepted at 2990–3008 for the retained GC renderer. No Main exclusion applies. This does not substitute for Course/UI fog or claim world rendering occurred. |
| Both selected-track-light variables omitted | Clean explicit environment means neither GC nor TA override is inherited. Default `freshLevelLighting` is false in Main; UI uses its genuine application light owner. Fresh gameplay defaults are unchanged. |
| `RR_ORIGINAL_MAIN_MENU=1`, explicit `RR_ORIGINAL_UI_ROOT` | `original_ui_main_entry50kg.h:50–56` creates actual Main application ownership and requires the root. The recorder supplies the staged authenticated root. |
| `RR_ORIGINAL_MAIN_AUDIO=muted` | Entry header 58–69 permits this mode and forbids `RR_AUDIO_WAV`. The recorder's clean environment has no WAV key and no enabled UI audio-root requirement. Frontend `processReady` requires no audio owner for mute. |
| `RR_ORIGINAL_RIDER_IPUM_CACHE` | Entry header 97–109 initializes the retained profile from real PODPHYS. Header 252–299 loads actual Rider/background/pod-texture archives and all five catalog-named mips. `OriginalUiIpumTextureCache50KG::load` independently authenticates source/member/image identities before publication. |
| PATH, LANG, LC_ALL, TZ | Fixed ordinary execution/locale settings. No inherited RR policy or loader override is copied. |
| HOME, TMPDIR, XDG_CACHE_HOME | Recorder creates these private directories before native Popen. They do not fabricate native assets or UI state. Individual child-file limits remain explicit; aggregate ancillary storage is not claimed bounded. |

The absent `RR_GC_HOST_TRIGGERS` passes its dormant-policy guard. Native Gate remains unrequested. Optional trace, culling/shadow opt-outs, checkpoint, restart, capture-wrong-way, autodrive tuning and audio-WAV variables are not inherited.

## Non-environment prerequisites and limits

- The exact native ELF and report header are hashed, with the live kernel executable inode checked after launch. The external root runner must establish the actual clean committed build provenance and ledger/lock ownership; the recorder deliberately does not claim that its commit string proves binary provenance.
- Headless argv is exactly four arguments with `--headless-live`; runtime creates a 640×448 Mesa-surfaceless context. Its pipe remains open, making the headless host focused/active without an X/Wayland display. The original-UI loop handles input/render/capture and continues before world stepping. Actual context creation, GPU allocation and required source-RGBA framebuffer-fetch/texture-barrier capability can still fail honestly.
- Rider and Course readiness still depends on authenticated archives, real profile linkage and renderer capability. Course requires muted mode0, one human, fixed Course1/subtype1/laps3/entrants8, Planet0 and inactive popup state. The cold all-AI world profile is separate from this real menu profile; it is not used to invent those predicates.
- Runtime/Main suppresses world audio output, initial race observations and race activation; recorder still requires zero world ticks and PCM receipts. Muted effects may change the genuine UI RNG. No successful frame or visual fidelity has yet been established by the corrected launch.
- Corrected output root is fresh `course-validation-02`. The final source read confirms `EARLIEST` was updated to **16:22:19.242379 UTC**. The next root runner must still enforce/pass the actual current ledger gate and freeze the final correction commit. Eligibility is not authorization for a retry.

## Terminal ownership follow-up

The separate unexecuted `cleanup-kernel-proof.NOT-EXECUTED.patch` addresses the transient `/proc` snapshot/reparenting race. Its `waitpid(-1, WNOHANG)` loop reaching ECHILD is valid only in the actual verified Linux subreaper, after Popen has consumed the phase leader. The proposed placement respects that ownership. A later unrelated shell cannot retroactively supply ECHILD proof for the exited runner.

The completed attempt's retained summary reports native return code1, no forced signals, no Ready, no frames and no input edges; the session reports empty remaining descendants. Those are the actual retained observations, not an ECHILD receipt. Preserve the distinction and the immutable failure evidence.

## Identities read

- Corrected recorder, including updated earliest gate: `717749e1089dc2c865de99177e83675f6e62f416f31db775abf0e5c709734c0c`.
- Runtime `terrain_interactive_drive50du.cpp`: `730aadde705ee80168281674cc0380c1553834032be49995902b1db545efea5f`.
- Entry `original_ui_main_entry50kg.h`: `7183336a7adf81ef68423e846383df97667865c0277e85926db17702d006f210`.
- Original failed recorder snapshot: `18fb2086d49b55074773ca72448a08f50580db4b6f9adaf8b101f86ecd60475a`.
- Executed root runner: `e78794db0684c3134f4b94acd89d85dfcf0d17f23eba284707b1e9f6e9f76c23`.

Later source changes require a new identity pin; this report does not authorize execution or a second launch.
