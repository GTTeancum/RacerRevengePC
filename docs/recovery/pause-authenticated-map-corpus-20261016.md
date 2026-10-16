# Authenticated Pause map corpus

The existing Pause source owner now retains the actual OVERLAYS members referenced by its authored materials: `icon_xpause.psm` uses `icon_x`, and `arrwleftPause.psm` / `arrwrghtPause.psm` use `arrwleft` / `arrwrght`. Their unchanged PST/ATM bytes enter the SAME canonical resolver during prewarm, before FINAL population. Existing enum ordinals are preserved; three members are appended. Archive/member authentication and transactional SourceData publication remain unchanged.

Source archive: `UI/TEXTURES/OVERLAYS.RES`, SHA-256 `ab51e88aef70d4a619ca3e614c64b90bad07deb3b9f0ec918577a44a8effb15c`. The existing authenticated catalog gives:

- `ui\\textures\\icon_x.pst`: logical offset196608,604 bytes, SHA-256 `928b1fd4747c70fc0305a3da694798cde1fc436c0e8a5a5a3485582487a515eb`.
- `ui\\textures\\arrwleft.atm`: offset245760,10392 bytes, SHA-256 `2a7bc92483e25499c8b84f50a3e196322fb4f4b585bf816c91a456ab107514ea`.
- `ui\\textures\\arrwrght.atm`: offset294912,10392 bytes, SHA-256 `f8f84c469b1f865ae1688f488e980919081d71a2b3473b5de1f707708ef008c3`.

Bounded standard-zlib archive inspection and raw integer reads independently matched these hashes. Each arrow is ATM revision1, frame count5, storage0. Each of its five consecutive frames is PST revision1, kind1, one32x32 mip with zero surface words. The final frame ends at exactly10392. Existing `splitOriginalUiAtm50KG` and canonical indexed ATM resource construction support these extents; no IPUM bridge, new decoder, substitute static frame, or discarded animation is introduced.

The authored arrow animation remains unique,16FPS, reverse-loop, and animates while paused. This patch supplies its authentic complete source only. Current Pause projection observes the canonical selected frame; its actual ScreenObject animation-service invocation still requires a separate source-backed join to the existing canonical29AC90 leaf. It is not evidence that arrow animation already advances. Named shader-pass-group admission is a separate prerequisite.

Review was source-only. No build, tests, syntax check, gameplay, or project decoder was executed.
