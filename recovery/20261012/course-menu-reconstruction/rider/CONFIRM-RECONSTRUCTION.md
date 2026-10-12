# Rider19 Confirm and shared clock/RNG join

New, unverified source-only reconstruction after workspace rollback. No previous candidate hashes or exact original candidate bytes were retained. No project execution or compiler/test/syntax checks were performed.

## Files and build input

The eleven C++ replacements/new files are:

- platform/linux/native/original_ui_rider_session50kg.h
- platform/linux/native/original_ui_rider_session50kg.cpp
- platform/linux/native/original_ui_main_session50kg.h
- src/game/original_ui_rider_character50kg.h
- src/game/original_ui_rider_character50kg.cpp
- src/game/original_ui_rider_model_scene50kg.h
- src/game/original_ui_rider_model_scene50kg.cpp
- src/game/original_ui_rider_dynamic_owner50kg.h
- src/game/original_ui_rider_dynamic_owner50kg.cpp
- src/game/original_ui_rider_voice_choice50kg.h
- src/game/original_ui_rider_voice_choice50kg.cpp

Only `original_ui_rider_voice_choice50kg.cpp` is a new build source in this Rider subset. The clock backend is the existing inline `src/game/lap_host_clock50id.h`, unchanged. Runtime ownership edits are in `../frontend/RUNTIME-COURSE-THREADING.patch`, not in a full runtime replacement. The historical `reconstruct_confirm.py` is a partial transformation record and MUST NOT be rerun over the final candidate.

## Concrete transaction

Muted mode0, one human, exact selected Rider19, selected control containing four genuine widgets with fourth widget53. The actual model must be attached, selected and shown. Confirmation has a distinct proposal flag and exact three source profile writes to remembered6F0, rider bits1125D and comparison100C0. Frontend prepares an authentic Course owner/bank/GPU first; source proposal and profile generation are rechecked before private commit. notifyAccepted checks the actual next generation, packed rider bits, comparison and manager request6/1 before effects.

The animation preflight runs the existing real transitionModelPlayback50KG leaf against copied native playback and a copied native graph. It consumes no clock or RNG and performs no live mutation. Actual acceptance calls the real dynamic confirmCharacter(widget53), which starts `_sel`. No Back bypass or fabricated ready bit is applied. Existing panel progression and character >80% source readiness remain the closing gate.

Independent full-join static review completed without a remaining definite blocker in this bounded path. Root explicitly retained `riderConfirmReviewClosed=false` and the frontend gate until after the pending GC fog observation. Admission is deliberately deferred; the review is no longer the blocker. No runtime admission, renderer or visual result is claimed.

## Source evidence and host clock mapping

Static ELF extracts in `evidence/` are plain word/field disassembly; they do not execute guest code.

- 1EA604 requests screen6/page1. 1EA620 writes fadeMusic4E8=1.
- 1ECF40 checks selected control size4. 1ECF70 returns its fourth widget.
- 1EA694 calls source timer101A50 only when the function-static seed cache3D6B30 is zero. The gp80A8 guard initializes that cache to zero once.
- 1EA6A8 calls308020(seed). Both308020 and308030 use `*(0x38544C)+0x58`; this is the same RNG word represented by application.sharedNoise308030/rawRandom308030, not a separate audio RNG.
- 308030 updates raw state with 0x41C64E6D*x+0x3039 and returns low31 bits. Exactly one draw occurs for this voice choice.
- Constructor1E7C80/1E7D6C supplies Rider19 count5 at+5408. 1E7CF8/1E7E3C supplies first voice39 at+5474.
- Selection uses a fixed-domain native mathematical evaluation of source toward-zero CVT.S.W and `5 * (float(draw) * 2^-31)`, then positive truncation. For the low31-bit draw, clear any low bits beyond24 significant binary bits to obtain q, then compute `(uint64(q)*5)>>31`. The power-of-two scale is exact; positive toward-zero binary32 reduction of the final product cannot cross an integer floor because every integer0..5 is representable. This avoids a synthetic host-nearest choice5 without process-global rounding changes, modulo, clamp, retry, extra draw, chip emulation or a general EE ALU parity claim. Independent reviewer identified the original FCR31/CVT.S.W rounding requirement and supplied this bounded evaluation; the earlier nearest-only reconstruction was corrected before admission.
- Source voice play1EA780 precedes1EA790→1D2690; effect1 follows at1EA7B0. Muted mode suppresses only those output calls, after preserving seed/draw/choice chronology.

Timer101A50 reads and extends the hardware counter, multiplies by source float bits3AE38E39, and converts to a uint32 timestamp. The pre-existing HostMonotonicClock50ID facility explicitly maps native steady-clock elapsed nanoseconds through576000 ticks/second and quantization49J to source milliseconds. The runtime patch changes that ONE existing backend to shared ownership; startup seed, material clock and all existing race/lap consumers retain the same reference/epoch. Application state retains the same clock owner and process-lifetime voice statics across Rider/Course destruction. The voice owner samples it at the actual accepted-confirm call, never from a fabricated timestamp or a second epoch. A zero sample remains zero, so the source cache==0 reseed branch can repeat on a later call.

No hardware/MMIO timing equality or original boot-seed parity is claimed. Cold preparation happens before accepted source effects in the existing native transaction, so native sampled wall time includes real preparation duration; no guest timestamp is invented to erase that difference.

## Shared selector correction, 03:11 UTC

The original lazy80A8/seed3D6B30 and lazy80AC/range3D6B38 are shared application statics, not private voice state. The candidate now retains one SharedRandomOwner50ID in OriginalUiMainApplication50KG. The stateless voice selector borrows that owner with the same application clock and sharedNoise308030. Main startup raw seeding does not initialize either lazy projection. Source mutation prefixes, cached-zero reseeding, exactly one draw and the accepted bounded integer choice formula are unchanged. This uses main 9fba56eb's existing shared_random_owner50id.h and adds no translation unit. Independent source review accepted all four hash-pinned changes at 03:11 UTC: ownership only, with unchanged chronology, arithmetic and failure prefixes.

The live runtime retains its separate fresh-race sessionRandom owner unchanged. Menu-to-race remains disabled; these ownership domains are not falsely joined. Both Course admission gates remain false. COMPLETE-INTEGRATION.patch and source preimage manifest are refreshed against current main; no patch/build/test/syntax or gameplay execution occurred.
