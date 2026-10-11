# Source-only review notes

No recorder execution, Python import, syntax check, parser, build, test, gameplay, or audio decoding was performed. Source reads, file staging, hashing, and unified diff generation only.

A separate source-only reviewer checked the twenty-press sequence against the live native input mapping, Game Options controller, Options session, transition owner, PCM clock/output, and receipt producers. It found no source-derived sequence, settings, audio-accounting, release/capture-barrier, or bounded-cleanup blocker.

Its two proof-strengthening findings were addressed:

1. Detached group -1 / slot 0 is no longer accepted for an arbitrary non-open state. It is permitted only at an incoming Main/Options page1 entry, with both input flags false and lifecycle/phase (2,0) or (4,1). A second reviewer pass found the genuine initial bound receipt occurs before the first tick, at lifecycle0/phase0; that single reason=bound, input_events0, route(4,1,0) case is also admitted. Main's unsupported returning slot1 fallback was removed. Game Options openingAC attaches its saved group in the same accepted source transition.
2. A completed Main return now establishes a strict tail: every later report must remain the exact open final checkpoint, and the recorder waits for an actual capture at least one second after the first acknowledged complete return. It cannot pass merely because wall time elapsed after a regressed final state.

Important existing producer semantics retained:

- Fresh settings are sound mode1, volumes[50,100,80,80], flags5, remembered group1, application sound revision1.
- Cancel updates only remembered group3 in this sequence. Re-entry loads mode1/SFX50 from the unchanged profile.
- Accept from group2 commits mode2/SFX40, remembered group2, Options accept count1, session apply revision2, and application revision2.
- Once fresh Main replaces Options, its Options-session count/revision fields return to0. The application settings and revision2 remain.
- Per-owner GPU publicationToken1 can legitimately repeat across fresh bank owners. The evidence does not treat that integer as a global object ID.
- AudioOutput writes the same real source-produced PCM to its WAV. With no physical device requested, device counters stay0. Exact integer clock equations are checked rather than treating wall-clock capture cadence as an audio clock.
- Native headless quit clears held host keys before its final report; no unacknowledged recovery key is sent. Partial failures preserve the evidence prefix.

The reviewer also found a stale native controls message claiming Accept remained unavailable. Root reports correcting it in c2bdc84. This recorder-only package does not modify the native diagnostic.
