# Graphics experiment 47L

The user-supplied PCSX2 renderer already owns the pixels. The native GS frontend
still interprets packets to maintain registers, transfer/IRQ state, presentation
observations, and debug history. `vertexKick` also constructs a complete native
draw batch and submits it to PCSX2's adapter, where `Submit` is empty.

An isolated copy at `build/gs47L/gs_frontend.cpp` removes that batch construction
only for the current raw-GIF backend. The preferred-display predicate uses the
same original values directly; vertex queue progression and full history remain
unchanged. Original GS source and the PCSX2 bridge are preserved. This assumes
the current raw-GIF backend's documented ownership of pixels; a future backend
that consumes both raw packets and structured draws needs an explicit capability
contract before reusing this shortcut.

`RR_RAW_GS_FAST47L` selects the experiment:

| Value | Batch construction | Native history |
| --- | --- | --- |
| 0 or unset | Original | Original |
| 1 | Elided for raw-GIF backend | Original |
| 2 | Elided for raw-GIF backend | Omitted for raw-GIF backend |

Non-raw backends retain draws and history in every mode. Mode 2 is diagnostic
reduction, not a faster guest instruction implementation. Ordinary GS history
defaults to paused, but the native test probe explicitly enables it whenever
frame/sequence capture is requested. Thus mode 2 quantifies capture-related
overhead, and any benefit must be reported separately from mode 1.

The actual original and candidate GS objects pass eight test configurations,
each with 42 fixtures across all seven primitive types, both contexts, forward,
reversed and clipped vertices, non-drawing kicks, positive preferred-display
selection, SIGNAL/LABEL/FINISH, and 504 intermediate preferred-state observations.
Raw mode receives 2,016 unchanged original packets. Mode 1 eliminates all 334
redundant `Submit` calls while retaining identical full history, snapshots,
test pixels, VRAM, preferred-source states and recorded sync/flush counts.
Mode 2 differs only by the intentionally empty history and missing redundant
submissions. Non-raw submissions remain 334 with identical output in all modes.

Two initial fixture attempts are retained, not silently discarded. The first
used a 2-pixel row where the existing capture API assumes a 640-pixel stride.
The second left native history at its paused default, so it could not prove
history equivalence. The final fixture uses the expected stride and explicitly
enables history. A batch-launch path error and its log are also retained.
These were fixture/launch failures; no game candidate was accepted from them.

All three modes complete native startup to VBlank 500 and match the accepted
47J2 EE RAM, IOP RAM, PCM, final image, reported guest/device state, exact cycle
and unchanged memory cards. This is a startup result, not long-race acceptance.

`Run-NativeSweep47L.ps1` freezes one executable and its runtime DLLs, then runs
0/1/2 sequentially with identical input and VBlank-7,000 target. It compares mode
0 to accepted 47J2, then the candidates to mode 0. Timing uses the same 6,000–6,900
capture window. Host profiling is off. Mode 0–1 estimates the batch effect;
mode 1–2 estimates extra diagnostic reduction. The new guards also exist in the
mode-0 control. One ordered run per setting is preliminary and is not a hardware
Xbox forecast. No new performance result is accepted yet.
