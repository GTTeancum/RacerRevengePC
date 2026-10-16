# Gasgano original UI confirmation voice evidence
Static original-data inspection only. No decoder/converter, build, test, syntax/import check, project executable, gameplay or guest execution was run. No PCM or original ADPCM sample payload is included. Actual decoding and playback remain unverified.
Root reports applying the coherent21-clip/events15-16/rider7-first15-count2 change for source review. This archive provides evidence; it does not certify that integration or its execution.

## Exact original identities
Original-only source root: D:/Programming/GitHub/RacerRevengePC/work/native/project/disc/AUDIO/SPEECH/UI
| File | Bytes | SHA256 | Rate | Contract frames |
|---|---:|---|---:|---:|
| GASSEL01.VAG |26656|4076d3b4943d5828a0b7977ca0bc9a6e1fdffc1e4a0925f41aed560713183c90|22050|46536|
| GASSEL02.VAG |26512|c647576fb9e438d58b921142c0592041ba2a9ccb0a9db539e0f6ceeb34eb28e9|22050|46284|

Both are48-byte retained zero-header variants, not VAGp magic files. Version and reserved fields are0. Only BE payload length at0x0C and rate0x5622 at0x10 are nonzero. Full exact header hex is in voice-evidence.json.
GASSEL01 payload0x67F0=26608; file ends0x6820.
GASSEL02 payload0x6760=26464; file ends0x6790.
Both payloads exactly reach EOF, with zero external alignment bytes.

## Full block extents and termination
All indices below are zero-based; offsets are file-relative.
GASSEL01:1663 declared16-byte blocks. Flags0 at indices0..1660, file[0x30,0x6800). Flags1 at index1661, file[0x6800,0x6810). Flags0 at final index1662, file[0x6810,0x6820).
GASSEL02:1654 declared16-byte blocks. Flags0 at indices0..1651, file[0x30,0x6770). Flags1 at index1652, file[0x6770,0x6780). Flags0 at final index1653, file[0x6780,0x6790).
Both first-end block headers are0C01: predictor0,shift12,flags1. Both unconsumed suffix block headers are0700: predictor0,shift7,flags0. The07 is NOT a flag7. Each suffix is16 bytes inside the declared payload, not external padding. Exact suffix SHA256 and complete block-header rows are in the JSON/CSVs, without sample nibbles.
No flags4/5/6/7 loop-start/loop-end blocks occur. All consumed predictor fields are0..4 and shifts0..12. No VAGp markers occur at the reader's interleave probe offsets0x800,0x1000,0x6000.

## Frame-count derivation from the existing reader
src/assets/audio_assets50hc.cpp:130..209 is included as a line-numbered source excerpt, with full source-file hash in reader-source-references.json.
- Header checks recognize the retained zero-header variant and require payload=size-48.
- Each entered block emits28 mono samples BEFORE flags&1 is inspected.
- The first flags&1 block is therefore included; decoding then stops.
- GASSEL01 consumes1662 blocks:1662*28=46536 frames.
- GASSEL02 consumes1653 blocks:1653*28=46284 frames.
- The declared maxima46564 and46312 include one extra block each and are NOT the correct final frame counts.
- No embedded loop is selected; successful results would have channels1,hasEmbeddedLoop=false,loopStart=loopEnd=0.
This derives exact output LENGTH under the reader contract. It is not a claim that the decoder or predictor arithmetic was executed or that PCM/playback was validated.

## Complete summaries
voice-evidence.json provides full-payload and consumed-only histograms of predictor,shift,flags, complete flag runs, header checks, first-end offsets and block digests, and suffix facts.
Each .block-headers.csv enumerates EVERY declared block with index,file offset,predictor,shift,flags,exact two-byte block header and whether the native reader would consume it. No compressed audio data beyond block headers is present.
collect_static_voice_evidence.py reproduces this metadata by inspecting only the two original VAGs and reading existing source files; it contains no ADPCM predictor/sample decoder.

## Coherent root-owned integration boundary
OriginalUiMainAudio50KG::riderConfirmClipCount19->21, add exact events15/16 entries with the identities/frame counts above. Keep the same riderConfirmVoice and courseRequiredVoices: clips expand, not mixer voices.
Existing prepareCourseMenu authenticates full original bytes, invokes the native reader only during later authorized execution, then checks preparedClipMatches. That ownership predicate requires exact source hash,frame count,22050Hz,mono,no loops,owned sample extent and a clip pointing at that PCM.
Existing all-choice preflight must succeed before the real RNG draw. Common Course cold preparation consequently requires these two additional originals.
OriginalUiRiderVoiceChoice50KG needs only {7,15,2,"Gasgano"} in its authored range table. Preserve lazy seed/timestamp/shared RNG ownership, exactly one draw, original count arithmetic/range assertion, and playback/lifecycle chronology.
Unlock/profile/session/course admission remains root-owned. No claim that Gasgano is admitted or tested is made here.

