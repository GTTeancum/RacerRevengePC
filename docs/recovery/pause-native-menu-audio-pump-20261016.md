# Native Pause audio service

SDL AudioOutput50HD::State::callback only drains already mixed ring-buffer
samples. It never calls the shared mixer. A paused physics scheduler therefore
must explicitly service native menu audio if real feedback is to advance.

serviceNativeMenuAudio consumes actual host elapsed microseconds in positive
chunks no larger than the existing16ms quantum. The driver owns clock sampling,
carry, focus-reset and chunk admission. This method accepts neither a synthetic
world step nor an original callback receipt. It requires the actual counted
seven-group route, live prepared output, no global device pause, and actual
World4D8 source state44 paused. It preserves every group's pause state and each
voice's corresponding mixer cursor rule. It does not make frozen UI voices
exempt: authentic source group/filter admission must remain correct separately.

The same existing output/mixer advances, with real announcer completion polls
before and after (including a possible output failure after consumption).
No physics, AI, source announcer scheduling update, music envelope update or
committed-world-step counter advances. Existing output/mixed-frame telemetry
continues to describe actual consumed samples. RR_AUDIO=0 has no output and
must skip this call; its separate muted counted group initialization provides
control semantics only.

Do not use advanceSourceAudioWorld for this path: that driver helper performs
music envelope and source announcer leader/update work in addition to completion.
Do not call liveAudio.pause(true) merely because the Pause menu opens: that
stops the SDL device and prevents feedback service. Focus suspension may still
use the existing explicit native device policy, resetting the host menu clock
on resumption rather than catching up the suspended interval.
