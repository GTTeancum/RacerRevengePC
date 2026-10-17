# Reuse adjacent original HUD material setup

UNTESTED. Source-only change; no build, smoke, syntax check or FPS measurement.

In original_race_hud_render.inl::drawRaceHud, write depthMask, uiSourceBlend and
texture binding only for the first packet or a change in packet.material. Every
packet and triangle still draws in its original order. No packets are merged,
no texture barriers are removed, and all error checks remain.

Preparation stable-sorts packets by authenticated leaf rank while retaining FIFO
inside a shared leaf. A gleam packet has literal count1; text emits a packet only
after nonzero glyph count and increments quads for every glyph. Thus every
emitted packet reaches a draw error gate in either coherent framebuffer-fetch
or triangle-barrier mode before another packet can execute successfully.

Only draws, textureBarrier and error queries intervene between adjacent packet
setups. They do not change the depth-write mask, UI uniform, active texture unit,
or texture binding. The same material ID selects exactly the same values and
texture index; the texture getter only returns retained initialized metadata,
without a callback. validBorrow remains at pass entry. New passes always begin
with p==0 and establish state; there is no persistent cache.

Clear and bright gleam material IDs remain distinct even though they share an
image. Their different depth/alpha semantics still receive explicit setup.
Error exits and final gameplay restoration remain unchanged. Three setup calls
are removed per adjacent equal-material packet. Actual benefit is unmeasured.
A successor may validate text/gleam order in both fetch modes and error cleanup.
