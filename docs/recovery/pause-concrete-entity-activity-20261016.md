# Concrete Pause enable/disable

Source-only; no runtime activation or execution checks.

The actual Pause table3CF420 has slot24=141630 and slot1C=1415A0.
The retained entity traversal and full static words prove that both methods
change flags29 bits0/1 on their own object, then visit non-hidden children
(bit3 clear), dispatching the corresponding concrete child slot.1415B4 is
ANDI from zero, so disable clears both bits.141644 loads one, so enable sets
both. Neither function writes parent/sibling links or mask20/descendants24.

The existing privately owned Pause EntityPrefix now has `setActivity141630`.
It authenticates its canonical Application registry enrollment and complete
forest, current source object identity, and genuine empty child topology.
Attached sibling/parent links are accepted without replacing or detaching them.
Any future child or descendant-mask state is rejected before effects because
its concrete dispatch has not been joined. Source flags other than bits0/1
are preserved. The existing cold1A8ADC disable now calls this same checked leaf.

This is private storage, not a public host-toggle interface. The original
open1BE8A0 must reach this enable after actual Application11C, selection880,
opening30 and race10C feedback effects; its remaining200/884, race1ACF50 and
device suffix follow. Close1BE830 disables only after its preceding actual
state/audio calls. No ordering callback, parent attachment, readiness or
completed open/close is claimed here. Existing final release still requires
the exact detached source prefix and rejects attached ownership.
