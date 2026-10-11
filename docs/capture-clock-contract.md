# Capture image and native clock contract

Status: source-reviewed, UNEXECUTED / UNTESTED. The earlier central build passed, but the helper rejected the initial TA Main capture because its retained scene tick was 0 and its completed native counter was 1. This happened before driving or GC cold loading. The failed evidence is retained; no successful gameplay result is inferred.

The corrected four-module bundle pairs one committed image acknowledgment with exactly one subsequent `capture_state` and UI acknowledgment. The image and capture-state render ticks must match exactly. `frame.tick` remains the rendered tick for encoder compatibility; separate `rendered_tick` and `native_tick` fields retain both owners' values.

A retained menu image may trail the native counter but cannot lead it. Fresh live images still require exact render/native equality and reject retained recovery frames. There is no universal +1 adjustment or tolerance. During frozen menu intervals, native/mixed counters, actual WAV samples and retained scene tick must remain unchanged. Resume must advance native/mixed clocks with the same session/bank; only the existing single GC cold transition is admitted.

The runner hashes the shared `capture_clock_contract.py` before and after the authorized process. Duplicate, malformed, missing or unsolicited capture-state records fail. The read-only gate recognizes the observed failed-checker terminal status without reserving or launching anything. A later exact central reservation is still required; no retry is introduced.

Receipt labels now match the reviewed incomplete-world disclosure. GC CHARACTER owners already use the existing type2/player response; the PHYSICSBODY-only two-body virtual path is not a missing feature. Remaining labels cover Gate/LostAI detection, opening activation, LostAI branch actions, Gate sound and mode1 configuration. This label correction changes no capture logic or admission.

The earlier passing single-track helper and encoder are unchanged. The separate future Results controller still needs this contract at its final Main capture before integration. No replay or new execution validated this patch.
