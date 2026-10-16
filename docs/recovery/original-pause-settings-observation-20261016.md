# Pause settings observation segment

Source-only increment against c63a462; no builds, tests, syntax checks or guest
execution. This is a bounded settings-string segment of1BB010, not its complete
layout/render callback.

The private `refreshSettings1BB590` is reachable only through a narrow friend
observation adapter in tests/native/original_pause_observation50la.h. Bind uses
the actual Application's existing controlsProfile/gameSettingsProfile and
requires the same material Application as the concrete Pause context. The
adapter weakly borrows the concrete Pause owner and both profiles. Capture
rechecks the profile identities and live concrete context; it cannot accept
fabricated frames, arbitrary settings, player indexes or selected rows.
Application must outlive its explicit nonowning borrow; unbind before destruction.

Source1BA130 initializes owner880 and884 to zero. They now have private native
storage and no host mutation API. Future original input callbacks own changes.
The subtype11258 consumed by actual derived loading is retained, rather than
replaced with a host race-mode label. Changing race identity still needs the
future real parent/race observation binding and must not reuse this cold owner.

Original source3ACB60 row dispatch:
- row2 /1BB590 consumes controls slot.field10F94. Source3ACB40 selects eight
  cases. Label403 plus values410,412,413,414,415,416,417,411 respectively.
  For subtype2 or3, options3/5/6 use the literal source formats '%s LAP/BEST',
  '%s TIME/BEST', '%s BEST' instead. These literal suffixes are original ELF
  bytes3ACAE8/3ACAF8/3ACB08, not invented translations.
- row3 /1BC258 consumes slot.field10F9C. Label404 plus values
  410,418,419,420,421,422,423,411. Original3ACB20 eight-way table.
- row4 /1BCCA8 consumes slot.field10FA4; label405 plus entry2 when nonzero,
  entry3 when zero. All pair cases use source3ACAE0 '%s %s'.
- rows5..8 /1BCFD8,1BD148,1BD2B8,1BD428 consume the four actual profile
  volume11020 words. Labels406..409 and source3ACB10 '%s %d%%'.

The original source string comparison, capacity40 truncation, NUL and dirty
flags use the existing concrete text helper. Updates follow row order2..8 and
retain earlier effects on a later failure. Out-of-range8-way switch values are
rejected: the source falls through to use unspecified stack text, which is not
an acceptable native substitute label. No settings are changed by observation.

Remaining joins: bind actual race title helper187390 and subtype lifetime,
complete title/selection geometry and source2E0170 traversal, retain actual
font/PST metric/regeneration path, selected-text GetWidth2E16F0 arrow positioning,
true Entity parent/input callbacks and retirement-gated GPU ownership. This
segment adds no readiness flag, packet submission or activation call. The
adapter's captureSettings name deliberately does not claim a complete frame.
