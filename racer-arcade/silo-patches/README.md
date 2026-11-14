# Silo test-capture patches

Test tooling from the silo worktree branch `silo/arcade-sc-override` (that branch itself stays local).
Apply with `git am` on top of the port (they touch only the unattended test loop and the capture runner):

1. timed F12 framebuffer captures while racing (`RR_TEST_CAPTURE_EVERY_SECONDS`, runner `--images --capture-every N`)
2. timed captures before green: intro cut cameras and countdown (`RR_TEST_CAPTURE_INTRO_MS`, runner `--capture-intro-ms N`)

Look-checks only: each capture stalls the frame while the image is written.
