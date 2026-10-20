# Source checkpoint: October 20, 2026

This checkpoint publishes the current implementation, including work in progress. It is not a release or a claim of complete game coverage.

The integrated source includes intervening Wan/MB, Gasgano/SC, Tzidik/TC and Scorch/RC work, plus the current Sebulba/TB implementation. Prior Scorch/RC GUI07 completed its bounded controlled gameplay flow and normal close. That observation does not establish full-race completion, every track/racer combination, or hardware-equivalent rendering and arithmetic.

The combined TB source includes compact original-source ownership, navigation, world/material/image/sky/animated-texture handling, authentic projected materials, lighting, audio, camera/session joins and the animated door collision/visual path. Component checks and static reviews are narrower than an integrated gameplay result. The combined TB Application build and GUI validation are pending at this checkpoint.

Native measured image/frame hashes remain regression measurements; they are not independent pixel-parity evidence. Existing independently checked rows keep their original labels. Genuine race finish/Results, broader gameplay coverage and further failure-path coverage remain pending.

Users must supply their own original game assets separately. This publication contains source and documentation only: no original archives, decoded/generated asset packs, private input inventories, binaries, objects or runtime recordings. Earlier dated development notes describe historical milestones and should be read with this current status.

The tested Application builds used explicit strict translation-unit lists and assertions, not an assumed ordinary CMake build. The current source adds the TB navigation implementation; the complete current Application build is expected to contain 357 translation units. This checkpoint does not claim that the repository's default build registration has been updated or validated for that complete list.
