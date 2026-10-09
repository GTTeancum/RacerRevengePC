# Current native race validation

The 2026-10-09 10:16 UTC Linux check built successfully and completed one
full eight-racer race followed by one restart in the same process. Exit was normal.

- All eight distinct original racers completed three laps. Player finished
  first at 160.928 race seconds; the field completed by 223.664.
- Finished opponents left the render list only after their sealed finish:
  zero submitted, seven hidden finished, all eight results still displayed.
- One restart produced a real tick-1 countdown grid, zero completed laps,
  seven submitted opponents and all eight racers advancing past ten seconds.
- Both sessions retained original engine and race audio. Four countdown/GO
  events were observed again in the new session.
- First WAV: 232.912 seconds. Second WAV: 14.688 seconds. The first PCM prefix
  and finalized whole-file hash survived session two unchanged.
- 49 completed-render samples: 53.1286–62.5601 FPS in this cloud VM.
- Six acknowledged real framebuffer captures document race, clean finish,
  restarted countdown and subsequent driving.

Tested source: `ff0763ca42cd0f67127816df30e5c0a8aea3d333`.
Executable SHA-256:
`411e794bfb074e404848a03f34379944b74e0db4a79a843f363cd6be4cbb5282`.

This validates the host finish-display policy, not a change to collision
physics or source finish behavior. Direct player controls with autodrive off
are the next focused check. Listening quality, broader platform coverage and
unfinished original-game integration remain open. Earlier audio captures
reported clipping; it has not been represented as eliminated.

Fresh Linux results only; no Windows execution occurred during this check.
