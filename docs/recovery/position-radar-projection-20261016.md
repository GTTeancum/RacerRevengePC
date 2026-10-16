# Original radar point projection: source-only native leaf

Recovered 1922B0 through 1924B0 from authenticated SLUS data (SHA256
c1f1b63eb422b624189e68eb0140b318455e341d73182703017298fea6ce6c30).
The adjacent source listing is static byte-to-text inspection only. No guest
instructions, builds, syntax checks, numerical tests or gameplay were executed.

The routine projects X/Z relative to owner32C0/32C8 with actual owner cosine32D4,
sine32D8 and scale32DC, then offsets by owner32E4/32E8. Y is negated after the
source `-dx*sin + dz*cos` sum; multiplication and addition stay separate native
binary32 operations. Original finite input arithmetic is retained; nonfinite
inputs/results reject without publishing output as an explicit native limit.

The point is outside when X exceeds +/-45 or Y exceeds [-57,21]. Inclusive
boundary points remain inside. Alpha is 255 in the inner rectangle +/-39 and
[-51,15], then 192,128,64 across successive two-unit outer bands. Outside alpha
is zero. Screen coordinates are still returned for outside points so the later
original segment clip can use them. This routine does not clip segments itself.

The simple static decoder marks four relevant COP1 forms UNKNOWN. Their fields
were read directly: 4604101A is MULA.S f2,f4; 4603009C is MADD.S f2,f0,f3;
function54 is C.OLE.S and function52 is C.OLT.S. These are source evidence only;
the implementation contains native float operations, not an instruction engine.

This leaf is not yet called by the PositionStats owner. The original1924C0 and
192C70 traversal, map/primitive writes and native ScreenObject draw path remain
required. No new HUD, minimap or gameplay result is claimed.
