# Wreck correction setter shared lazy global

Apply this small follow-on after the flight and query-scratch packets. It adds the real28ADD0 shared lazy gp8C34/4433E8 projection to the application-owned OriginalWreckFlightGlobals50LA. It is not per-piece state; any other native caller of that setter must use the same application field.

Source28ADDC tests gp8C34. If zero,28ADE8 loads-1,28ADF0 stores0xffffffff at4433E8 and28ADF8 stores guard1. These effects occur before the setter's relative/parent branches. The actual166C10 flight call has already computed its corrected position before entering this setter. The patch therefore performs the lazy writes only immediately before that reached localXYZ store, retaining initialized words unchanged. No-hit flight calls do not consume or initialize this sentinel.

This was found during source review of the sibling activation28AC90 setter, whose distinct gp8C30/4433E0 lazy word is being added by the activation owner. The two setters must not share one sentinel accidentally. Orientation28A090 uses the separately owned gp8C2C/4433D8.

The exact prior query-scratch flight header is retained under preimages. Source evidence is copied from the already frozen permitted rawword output. No builds, tests, syntax/import checks, lint, sanitizers, gameplay, asset decoding or PC operations were executed. The central repository was not edited.
