# Native Linux validation, 2026-10-10 15:18 UTC

Candidate: `8bb2bfe946c62b0f65d5dde0803f2c24e8552b5f`.
Executable SHA-256: `a757b130093be978cdfeda86f5f264ac8c3846e6b33e0f0cdd77f0967f0e7263`.

One centralized build/observation slot was used. Fresh source-only g++ C++20
build compiled 210 units with no reused or externally supplied objects; link
exit 0. This includes the restored, inactive UI modules. It does not establish
rendering or finished menu behavior.

The one GC native process initialized all eight original AI racers, including
camera slot 0. Readiness counters reported human_count=0, ai_count=8,
host_driver_calls=0, controller_samples=0, keyboard_packets=0. No simulated
input or alternate driving controller was used.

The recorder failed immediately after its first capture request: its event loop
requested another capture while the first acknowledgment was pending, triggering
`RuntimeError: A second capture cannot be outstanding`. Exception cleanup sent
SIGTERM. Recorder exit 2; process return -15; zero acknowledged frames; no valid
WAV; no observed driving growth. There is no usable gameplay video, braking
result, lap result, or successful runtime acceptance from this attempt.

A source-only fix is pending. A separate telemetry issue printed uint8 impact
flags as characters and is corrected after this candidate. Neither later fix
has been retested. Historical Windows results do not apply to this check.
