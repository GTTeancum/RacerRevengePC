# Interactive host pacing 50DT

Caller supplies monotonic nanoseconds, an explicit fixed simulation interval and
maximum catch-up count. The allocation-free clock emits bounded work and the
remaining wait. It exposes discarded stalled-host time and timestamp regression.
Pause/rebase on lost focus or minimization so restoration does not fast-forward.
Game timing conversion remains the existing native frame implementation. This
utility neither sleeps nor invokes gameplay. Its policy is a host choice, not a
claim about the original console's scheduler. Written and source-reviewed only;
not compiled or executed. Root owns the single shared hourly test slot.
