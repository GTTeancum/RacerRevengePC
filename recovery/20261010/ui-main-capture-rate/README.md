# Main capture-rate correction

The 17:55 one-off run rendered 1636 native frames over the20-second observation
but retained94 images at the recorder's5Hz ceiling. The delivered94-frame VFR
video is therefore visibly choppy; it does not show native animation cadence.
The user reported this at18:01UTC. No interpolation or fabricated smoothness
was applied to the capture.

A separate higher-rate recorder candidate targets30Hz, caps requests at620
and reduces bounded receipt polling delay to5ms. Serialized request/ack/copy,
exact image/source provenance, no driving/navigation input,20-second window,
explicit quit and one-process guarantees stay intact. The previous recorder
and delivered evidence are unchanged. At640x448 this caps retained PPM bytes
at533308500 (about509MiB), plus bounded logs. Available cloud disk was26GiB.

Candidate: ui-main-recording-reconstruction/files/tools/gameplay/
record_original_main_30fps_recovery.py under the recovery staging directory.
No syntax check or run has occurred. Actual attainable capture rate must be
measured at the next authorized centralized run;30Hz is a target, not a pass.
Normal next slot remains18:56:57.403928UTC.
