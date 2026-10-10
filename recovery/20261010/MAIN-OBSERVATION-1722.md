# Original Main observation, 2026-10-10 17:22 UTC

Source: `f60d16378c37e75925cb4b38858034fdf2f5afd7`.
Linux native build passed: 58 translation units compiled, 171 reused after
dependency/content validation, no supplied prebuilt objects.
Executable SHA-256: `232f03868ede3dd1410aae4f15903c80d6c84de79746f09684f12dc8c6871577`.

One native process loaded the real assets, bound the original Main session and
submitted one 640x448 frame through the actual GL renderer. The recorder rejected
the startup receipt because `world_ticks` was 1 where Main-only ownership required
0. It sent quit; native exit was 0 and no forced termination occurred.

No capture requests or acknowledged image files were produced. This is not a
visual pass, opening-animation pass, navigation pass, gameplay capture or audio
proof. The requested 20-second opening/idle window was not completed.

Source inspection identified the pre-window race graphics setup loop: it still
advances each racer's native frame once before the Main-only branch. The correction
must isolate Main cold startup from that physics step, preserve ordinary gameplay
startup, and retain genuine assets/renderer ownership. Do not hide the counter or
relax the recorder's no-gameplay requirement.

The hourly slot was consumed at 17:22:22.417511 UTC. The next centralized check is
not eligible before 18:23:22.417511 UTC. Follow-on source fixes are untested until
that later check; the process must not be rerun as an incidental capture retry.
